// s5_serial.h -- the serial line (SPECIFICATION.md 2.8) and P16 to P20 (AA p.27).
#pragma once

#include "s5_p915.h"

namespace s5 {

// ---- TxD -----------------------------------------------------------------------

inline int tx_at(const Cap& c, uint64_t t) {
    if (t < c.base || t - c.base >= c.txd.size()) return -1;
    return c.txd[t - c.base];
}

// The cycles in [t0, t1] at which TxD takes a new level.
inline std::vector<uint64_t> tx_edges(const Cap& c, uint64_t t0, uint64_t t1) {
    std::vector<uint64_t> e;
    if (t0 < c.base + 1) t0 = c.base + 1;
    for (uint64_t t = t0; t <= t1 && t - c.base < c.txd.size(); ++t)
        if (c.txd[t - c.base] != c.txd[t - c.base - 1]) e.push_back(t);
    return e;
}

struct TxFrame { uint64_t s = 0; unsigned bits[10] = {}; unsigned lsb = 0, msb = 0; bool framed = false; };

// Frames of a start cell, eight data cells and a stop cell, T cycles each; the idle line is at the stop level.
inline std::vector<TxFrame> tx_frames(const Cap& c, uint64_t t0, uint64_t t1, int idle, uint64_t T) {
    std::vector<TxFrame> out;
    uint64_t t = t0;
    while (t < t1) {
        uint64_t e = 0;
        for (uint64_t u = std::max(t, c.base + 1); u <= t1 && u - c.base < c.txd.size(); ++u)
            if (c.txd[u - c.base] != idle && c.txd[u - c.base - 1] == idle) { e = u; break; }
        if (!e) break;
        TxFrame f; f.s = e;
        for (int i = 0; i < 10; ++i) f.bits[i] = tx_at(c, e + uint64_t(i) * T + T / 2) == idle ? 1u : 0u;
        for (int i = 0; i < 8; ++i) f.lsb |= f.bits[1 + i] << i;
        for (int i = 0; i < 8; ++i) f.msb |= f.bits[1 + i] << (7 - i);
        f.framed = f.bits[0] == 0 && f.bits[9] == 1;
        out.push_back(f);
        t = e + 9 * T + T / 2;
    }
    return out;
}

// G-T: every edge of the capture lies on a grid of pitch T from the first edge of its frame, within +-K,
// and the shortest interval between edges is T within +-K.
inline bool g_t(const Cap& c, const std::vector<TxFrame>& fr, uint64_t t0, uint64_t t1, uint64_t T, uint64_t K, uint64_t& tmeas) {
    auto ed = tx_edges(c, t0, t1);
    tmeas = 0;
    for (size_t i = 1; i < ed.size(); ++i) { uint64_t d = ed[i] - ed[i - 1]; if (!tmeas || d < tmeas) tmeas = d; }
    bool ok = tmeas > 0 && (tmeas + K >= T && tmeas <= T + K) && !fr.empty();
    for (uint64_t e : ed) {
        bool on = false;
        for (auto& f : fr) {
            if (e < f.s || e > f.s + 10 * T + K) continue;
            uint64_t r = (e - f.s) % T;
            if (std::min(r, T - r) <= K) on = true;
        }
        ok &= on;
    }
    return ok;
}

// ---- RxD drive ----------------------------------------------------------------------

// Drives uart_rx from a schedule of (cycles after the reference marker, level).
struct RxDrive {
    int marker_idx = 0;
    std::vector<std::pair<uint64_t, int>> sched;
    size_t seen = 0, next = 0;
    uint64_t t_ref = 0;
    bool invert = false;
    void operator()(nmh::NanomigHarness& h, uint64_t cycle) {
        if (!t_ref) {
            const auto& lg = h.ram_log();
            for (; seen < lg.size(); ++seen)
                if (lg[seen].is_write && lg[seen].word_addr * 2 == kMarks + 2u * uint32_t(marker_idx - 1) && lg[seen].data == uint16_t(marker_idx)) { t_ref = lg[seen].cycle; break; }
            if (!t_ref) return;
        }
        while (next < sched.size() && cycle >= t_ref + sched[next].first) {
            int lv = sched[next].second;
            h.ports()->uart_rx = (invert ? !lv : lv) ? 1 : 0;
            ++next;
        }
    }
};

// A frame: start cell (0), eight data cells LSB first, stop cell (1), cells of T cycles from time t.
inline void rx_frame(std::vector<std::pair<uint64_t, int>>& s, uint64_t t, unsigned byte, uint64_t T) {
    s.push_back({t, 0});
    for (int i = 0; i < 8; ++i) s.push_back({t + (1 + uint64_t(i)) * T, int((byte >> i) & 1)});
    s.push_back({t + 9 * T, 1});
}

inline uint64_t max_gap(const Cap& c, size_t i0, size_t i1) {
    uint64_t r = 0;
    for (size_t i = i0 + 1; i <= i1 && i < c.log.size(); ++i) r = std::max<uint64_t>(r, c.log[i].first - c.log[i - 1].first);
    return r;
}

// ---- P16 and P17: the transmit buffer --------------------------------------------------

struct TxRun {
    Cap cap; Prefix pre; Frame fr; uint64_t K = 8, T = 512, R = 0, t_idle_start = 0;
    int m_ser = 0, m_w1 = 0, i_poll0 = 0, i_poll1 = 0;
    std::vector<int> m_x;
    std::vector<uint8_t> d;
    uint64_t t_w1 = 0, t_w1a = 0, t_cap0 = 0, t_cap1 = 0;
    uint64_t T_expect = 0;
};

inline TxRun tx_run(const std::string& variant) {
    TxRun r;
    RunSpec rs; rs.bin = run_bin(); rs.sample_txd = true;
    Script& s = rs.sc;
    r.pre = prefix(s, true, 119, brk_hook());
    r.d = {0x81, 0x3C, 0x5A, 0x00, 0xFF, 0x66, 0x18, 0xA5};
    std::vector<uint8_t> wd = r.d;
    if (variant == "D3") wd[2] = 0x24;
    r.m_ser = s.w16(SERPER, mut("GT") ? uint16_t(0x007F) : uint16_t(0x003F));
    r.T_expect = 64;
    int n_wait = mut("P17-R") ? 40 : (mut("P16-IDLE") ? 30 : 130);
    r.i_poll0 = r.pre.logn;
    if (variant == "X") { s.w16(SERDAT, 0x0181); }   // at the start of the wait
    if (variant == "X16") {                          // 10 poll reads in, so that the frame lies inside the 20T before the first write
        s.poll(INTREQR, 59); s.w16(SERDAT, 0x0181); n_wait -= 60; r.pre.logn += 60;
    }
    if (mut("P17-R")) {   // contrast: the poll loop is slowed down so that R exceeds the bound
        for (int i = 0; i < n_wait; ++i) { s.rd(INTREQR); s.dly(8); }
    } else s.poll(INTREQR, unsigned(n_wait));
    r.pre.logn += n_wait + (mut("P17-R") ? 0 : 1);
    r.m_w1 = s.w16(SERDAT, uint16_t(0x100 | wd[0]));
    std::vector<uint16_t> words;
    for (int k = 1; k < 8; ++k) words.push_back(uint16_t(0x100 | wd[size_t(k)]));
    uint16_t cval = mut("P17-NOCLEAR") ? uint16_t(0) : uint16_t(1);
    if (variant == "DELAY") {
        // word 5 only after three frame times of idle
        s.feed(INTREQR, 0x0001, 600, cval, std::vector<uint16_t>(words.begin(), words.begin() + 3));
        s.dly(wait_iters(30 * 64));
        s.w16(INTREQ, 1);
        r.m_x.push_back(s.w16(SERDAT, words[3]));
        s.feed(INTREQR, 0x0001, 600, cval, std::vector<uint16_t>(words.begin() + 4, words.end()));
    } else {
        s.feed(INTREQR, 0x0001, 600, cval, words);
    }
    if (variant == "EXTRA") { s.dly(wait_iters(25 * 64)); s.w16(SERDAT, 0x01FF); }   // contrast: a ninth word after frame 8
    s.poll(INTREQR, 150);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 30000;
    r.cap = run(rs);
    r.fr = analyse_refresh(r.cap);
    r.K = r.fr.K ? r.fr.K : 8;
    r.T = 64 * r.K;
    r.t_w1 = marker(r.cap, r.m_w1);
    r.t_w1a = marker(r.cap, r.m_w1 + 1);
    return r;
}

inline void run_p16_17(Checks& ck) {
    const bool sel16 = mut("P16-D3") || mut("P16-DELAY");
    std::string var = mut("P16-D3") ? "D3" : (mut("P16-DELAY") ? "DELAY" : (mut("P17-X") ? "X" : (mut("P16-X") ? "X16" : (mut("P16-EXTRA") ? "EXTRA" : ""))));
    (void)sel16;
    TxRun r = tx_run(var);
    Std st = std_pre(r.cap, r.pre, {});
    PreAgg agg; agg.add(st);
    note_gc(ck, "P16", st);
    const Cap& c = r.cap;
    const uint64_t K = r.K, T = r.T;
    const uint64_t t_end = c.base + c.txd.size() - 1;
    // TxD from 20T before the first write
    uint64_t t0 = r.t_w1 > 20 * T ? r.t_w1 - 20 * T : c.base + 1;
    uint64_t t_ser0 = marker(c, r.m_ser + 1);   // frames are looked for from the SERPER write on
    int idle = tx_at(c, t_ser0 + 1);
    auto frames = tx_frames(c, t_ser0 + 1, t_end, idle, T);
    uint64_t tmeas = 0;
    bool gt = g_t(c, frames, t_ser0 + 1, t_end, T, K, tmeas);
    bool gbrk = (logv(c, r.pre.i_adkconr) & 0x0800) == 0;
    // the idle precondition: TxD constant from the SERPER marker to the first write, for at least 20T
    uint64_t t_ser = marker(c, r.m_ser + 1);
    bool idle_ok = true;
    for (uint64_t u = t_ser; u < r.t_w1; ++u) if (tx_at(c, u) != idle) idle_ok = false;
    uint64_t idle_len = r.t_w1 > t_ser ? r.t_w1 - t_ser : 0;
    // the stores of the poll loop (everything from the first INTREQR poll on)
    size_t ip0 = size_t(r.i_poll0);
    std::vector<std::pair<uint64_t, uint16_t>> pl(c.log.begin() + std::min(ip0, c.log.size()), c.log.end());
    // the reads of 1 that triggered a write: the first seven after the first write
    std::vector<size_t> hit;
    for (size_t i = ip0; i < c.log.size() && hit.size() < 7; ++i)
        if ((c.log[i].second & 1) && c.log[i].first > r.t_w1a) hit.push_back(i);
    // R: the largest interval between consecutive log stores of the loop; the intervals that hold a marker
    // (the first write) or the end of the feeding loop are no loop interval
    uint64_t R = 0;
    for (size_t i = ip0 + 1; i < c.log.size(); ++i) {
        uint64_t a = c.log[i - 1].first, b = c.log[i].first;
        bool skip = !hit.empty() && i == hit.back() + 1;
        for (uint64_t mt : c.mark_t) if (mt > a && mt < b) skip = true;
        if (!skip) R = std::max(R, b - a);
    }
    ck.note("P16: T=" + dec((long long)T) + " cycles, measured shortest edge interval " + dec((long long)tmeas) + ", frames found " + dec((long long)frames.size()) + ", idle before the first write " + dec((long long)idle_len) + " cycles (" + num2(double(idle_len) / double(T)) + " T), R=" + dec((long long)R) + " cycles (" + num2(double(R) / double(K)) + " CCK), idle level " + dec(idle));
    {
        std::string fs;
        for (auto& f : frames) fs += hex4(f.lsb) + (f.lsb == f.msb ? "" : "!") + (f.framed ? "" : "?") + "@" + dec((long long)((double(f.s) - double(r.t_w1)) / double(T) * 10) ) + " ";
        ck.note("P16 frames (byte, start in tenths of T after the first write marker): " + fs);
    }
    // P16 expectations
    std::vector<uint8_t> d = r.d;   // the grader keeps d3 = 0x5A whatever the stimulus wrote
    bool a_ok = true;
    for (uint64_t u = t0; u <= r.t_w1 && u - c.base < c.txd.size(); ++u) if (tx_at(c, u) != idle) a_ok = false;
    bool b_ok = frames.size() == 8;
    for (size_t k = 0; k < frames.size() && k < 8; ++k) b_ok &= frames[k].framed && frames[k].lsb == d[k] && frames[k].msb == d[k];
    bool c_ok = !frames.empty() && frames[0].s > r.t_w1;
    bool d_ok = frames.size() >= 2;
    for (size_t k = 1; k < frames.size() && k < 8; ++k) d_ok &= frames[k].s <= frames[k - 1].s + 10 * T + T;
    bool e_ok = !frames.empty();
    if (e_ok) {
        auto ed = tx_edges(c, frames.back().s + 10 * T + K, t_end);
        e_ok &= ed.empty() && tx_at(c, t_end) == idle && frames.size() == 8;
    }
    print_std(ck, "P16", agg, 1);
    ck.expect(gbrk, "P16-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 (AA p.10 '11 UARTBRK Forces a UART break (clears TXD) if true')");
    ck.expect(gt, "P16-G-T: every TxD edge lies on a grid of pitch T = (N+1) colour clocks from its frame's first edge, within +-c, and the shortest edge interval is T within +-c (AA p.27 'If this number is N, then the Baud Rate is 1 bit every (N+1)*.2794 Microseconds')");
    ck.expect(idle_ok && idle_len >= 20 * T, "P16-PRE-IDLE: TxD is constant for at least 20T before the first write");
    bool pre = agg.fixture && agg.s0 && agg.gc && gbrk && gt && idle_ok && idle_len >= 20 * T;
    const std::string un = "fixture, G-S0, G-C, G-BRK, G-T or the idle wait";
    graded(ck, pre, a_ok, "P16-A: before the first write TxD is constant (AA p.27 'Data from this buffer is moved into a serial shift register for output transmission, whenever it is empty')", un);
    graded(ck, pre, b_ok, "P16-B: exactly eight frames appear and frame k carries d_k, each with a start cell and a stop cell at the stop level: none missing, repeated or extra (AA p.27 'This address writes data to a Transmit data buffer')", un);
    graded(ck, pre, c_ok, "P16-C: frame 1's start cell begins after the first write marker (AA p.27)", un);
    graded(ck, pre, d_ok, "P16-D: for k = 2..8 frame k's start cell begins no later than T after the end of frame k-1's stop cell: no idle cell (AA p.27 'moved into a serial shift register for output transmission, whenever it is empty')", un);
    graded(ck, pre, e_ok, "P16-E: after frame 8's stop cell TxD stays idle until the end of the capture (AA p.27)", un);
    graded(ck, pre, a_ok && b_ok && c_ok && d_ok && e_ok, "P16 sol:AA-serdat.c6 sol:AA-serdat.c7: every word written to SERDAT goes to the transmit buffer and is transmitted on TxD once, in the order written, and a word waiting in the buffer moves into the shift register as soon as it is empty, so its frame follows the previous frame with no idle cell (AA p.27)", un);

    // ---- P17 ----
    std::vector<TxFrame> fr17;   // the frames of the eight words: those that start after the first write
    for (auto& f : frames) if (f.s > r.t_w1) fr17.push_back(f);
    bool zero_ok = true, one_ok = true, any = fr17.size() == 8 && hit.size() == 7;
    bool clear_ok = true;
    std::string det;
    if (any) {
        for (int k = 1; k <= 8; ++k) {
            uint64_t e, lo;
            if (k == 1) { e = r.t_w1; lo = c.log[size_t(r.pre.i_adkconr)].first; }
            else {
                size_t h = hit[size_t(k - 2)];
                uint64_t wr_early = c.log[h].first;                 // the write of word k happens after this store
                uint64_t stop_start = fr17[size_t(k - 2)].s + 9 * T;
                e = std::max(wr_early, stop_start);
                lo = c.log[h + 1].first;                            // the first read after the clear
                clear_ok &= h + 1 < c.log.size() && !(c.log[h + 1].second & 1);
            }
            for (size_t i = ip0; i < c.log.size(); ++i) {
                uint64_t ti = c.log[i].first;
                if (ti >= lo && ti < e && (c.log[i].second & 1)) zero_ok = false;
            }
            // the first read of 1 after the clear: TBE is set by the start of frame k, a read of 1 follows within two poll intervals
            uint64_t sk = fr17[size_t(k - 1)].s;
            bool found = false; uint64_t t1 = 0;
            for (size_t i = ip0; i < c.log.size(); ++i) if (c.log[i].first >= lo && (c.log[i].second & 1)) { t1 = c.log[i].first; found = true; break; }
            size_t i1 = ip0;
            while (i1 < c.log.size() && c.log[i1].first <= sk) ++i1;
            // a read performed after the start of frame k is the one stored after the first store that follows the start
            one_ok &= found && i1 + 1 < c.log.size() && t1 <= c.log[i1 + 1].first;
            det += dec(k) + ":e-lo=" + dec((long long)e - (long long)lo) + ",first1-s=" + dec(found ? (long long)t1 - (long long)sk : 0LL) + " ";
        }
    }
    // the clear marker of the first word: S0's INTREQ write precedes every poll: the first poll reads 0
    clear_ok &= !(logv(c, r.pre.i_intreqr) & 1) && any;
    ck.note("P17: R=" + dec((long long)R) + " cycles; reads of 1 followed by a write: " + dec((long long)hit.size()) + "; per move k: window e_k - first clear read, and the first read of 1 minus the start of frame k (cycles): " + det);
    print_std(ck, "P17", agg, 1);
    ck.expect(gbrk, "P17-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 (AA p.10 '11 UARTBRK')");
    ck.expect(gt, "P17-G-T: every TxD edge lies on a grid of pitch T from its frame's first edge, within +-c, and the shortest edge interval is T within +-c (AA p.27 SERPER)");
    ck.expect(R > 0 && R <= 32 * K, "P17-G-R: R, the largest interval between consecutive log stores of the poll loop, is at most T/2 = 32 colour clocks (SPECIFICATION.md 2.3)");
    ck.expect(clear_ok && !(logv(c, r.pre.i_intenar) & 0x7FFF), "P17-PRE-CLEAR: INTENAR reads 0 and the first read after each clear reads 0 (AA p.25 INTENA, INTREQ set/clear)");
    bool pre17 = agg.fixture && agg.s0 && agg.gc && gbrk && gt && R > 0 && R <= 32 * K && clear_ok;
    const std::string un17 = "fixture, G-S0, G-C, G-BRK, G-T, G-R or the clear precondition";
    graded(ck, pre17 && any, zero_ok, "P17-ZERO: for every move k = 1..8 every read stored after the clear before word k and before the earliest instant e_k of the move reads 0 in INTREQR bit 0 (AA p.27 'This sets the Interrupt Request TBE (transmit buffer empty)'; AA p.25 '00 TBE 1 Serial port Transmit Buffer Empty')", un17);
    graded(ck, pre17 && any, one_ok, "P17-ONE: for every move k = 1..8 a read of 1 in INTREQR bit 0 is stored no later than the second store after the start of frame k's start cell: the request is set by the time the word is sent (AA p.27 'moved into a serial shift register for output transmission ... This sets the Interrupt Request TBE')", un17);
    graded(ck, pre17 && any, zero_ok && one_ok, "P17 sol:AA-serdat.c8: every move of the transmit buffer into the shift register sets INTREQ bit 0 (TBE), and TBE stays clear while a written word still waits in the buffer (AA p.27 'This sets the Interrupt Request TBE (transmit buffer empty)')", un17);
}

// ---- P18: the stop bit position ------------------------------------------------------

inline void run_p18(Checks& ck) {
    const int ps[6] = {8, 9, 7, 4, 8, 9};
    RunSpec rs; rs.bin = run_bin(); rs.sample_txd = true;
    Script& s = rs.sc;
    Prefix pre0 = prefix(s, true, 119, brk_hook());
    s.w16(SERPER, mut("GT") ? uint16_t(0x007F) : uint16_t(0x003F));
    std::vector<int> mk;
    for (int i = 0; i < 6; ++i) {
        s.dly(wait_iters(mut("P18-IDLE") ? 13 * 64 : 24 * 64));
        int p = ps[i];
        if (mut("P18") && i == 0) p = 9;       // contrast: p = 9 where the grader expects p = 8
        if (mut("P18-REP") && i == 4) p = 7;   // contrast: the second p = 8 is written as p = 7
        mk.push_back(s.w16(SERDAT, uint16_t(1u << p)));
        if (mut("P18-EXTRA") && i == 2) { s.dly(wait_iters(11 * 64)); s.w16(SERDAT, 0x0100); }   // contrast: a word written 2T after the frame's stop cell
    }
    s.dly(wait_iters(24 * 64));
    s.end();
    rs.cycles = 4'000'000 + 8ull * 20000;
    Cap c = run(rs);
    Frame fr = analyse_refresh(c);
    uint64_t K = fr.K ? fr.K : 8, T = 64 * K;
    Std st = std_pre(c, pre0, {});
    PreAgg agg; agg.add(st);
    note_gc(ck, "P18", st);
    uint64_t t_end = c.base + c.txd.size() - 1;
    uint64_t tfirst = marker(c, mk[0]);
    int idle = tx_at(c, tfirst > 12 * T ? tfirst - 12 * T : c.base + 1);
    // the low interval of each write
    std::vector<uint64_t> w(6, 0), tw(6, 0);
    bool idle12 = true, back_idle = true, found = true;
    for (int i = 0; i < 6; ++i) {
        tw[size_t(i)] = marker(c, mk[size_t(i)]);
        uint64_t e0 = 0, e1 = 0;
        for (uint64_t u = tw[size_t(i)]; u <= t_end; ++u)
            if (tx_at(c, u) != idle && tx_at(c, u - 1) == idle) { e0 = u; break; }
        if (!e0) { found = false; continue; }
        for (uint64_t u = e0 + 1; u <= t_end; ++u) if (tx_at(c, u) == idle) { e1 = u; break; }
        if (!e1) { found = false; continue; }
        w[size_t(i)] = e1 - e0;
        // TxD constant for 12T before the write
        for (uint64_t u = tw[size_t(i)] - 12 * T; u < tw[size_t(i)]; ++u) if (tx_at(c, u) != idle) idle12 = false;
        // and at the stop level until the next write
        uint64_t nxt = i + 1 < 6 ? marker(c, mk[size_t(i + 1)]) : t_end;
        for (uint64_t u = e1; u < nxt; ++u) if (tx_at(c, u) != idle) back_idle = false;
    }
    bool gbrk = (logv(c, pre0.i_adkconr) & 0x0800) == 0;
    std::vector<TxFrame> frames;   // every start edge of the capture
    uint64_t tmeas = 0;
    for (uint64_t u = (tfirst > 12 * T ? tfirst - 12 * T : c.base + 1) + 1; u <= t_end; ++u)
        if (tx_at(c, u) != idle && tx_at(c, u - 1) == idle) { TxFrame f; f.s = u; frames.push_back(f); }
    // G-T: the edges of every frame lie on the grid of pitch T from the frame's first edge, within +-c; the rate is the one
    // that a frame with single-cell pulses shows for the same SERPER value (the P16 stimulus), measured in its own run
    bool gt = true;
    {
        auto ed = tx_edges(c, tfirst > 12 * T ? tfirst - 12 * T : c.base + 1, t_end);
        for (uint64_t e : ed) {
            bool on = false;
            for (auto& f : frames) {
                if (!f.s || e < f.s || e > f.s + 12 * T) continue;
                uint64_t r = (e - f.s) % T;
                if (std::min(r, T - r) <= K) on = true;
            }
            gt &= on;
        }
        TxRun cal = tx_run("");
        uint64_t cT = 64 * (analyse_refresh(cal.cap).K ? analyse_refresh(cal.cap).K : 8);
        auto ce = tx_edges(cal.cap, cal.t_w1, cal.cap.base + cal.cap.txd.size() - 1);
        for (size_t i = 1; i < ce.size(); ++i) { uint64_t d = ce[i] - ce[i - 1]; if (!tmeas || d < tmeas) tmeas = d; }
        gt &= tmeas > 0 && tmeas + K >= cT && tmeas <= cT + K;
    }
    auto dif = [&](int i, int j) { return int64_t(w[size_t(i)]) - int64_t(w[size_t(j)]); };
    bool d9 = found && std::llabs(dif(1, 0) - int64_t(T)) <= int64_t(K);
    bool d7 = found && std::llabs(dif(2, 0) + int64_t(T)) <= int64_t(K);
    bool d4 = found && std::llabs(dif(3, 0) + int64_t(4 * T)) <= int64_t(K);
    bool r8 = found && std::llabs(dif(4, 0)) <= int64_t(K);
    bool r9 = found && std::llabs(dif(5, 1)) <= int64_t(K);
    ck.note("P18: T=" + dec((long long)T) + " cycles; w(p) for p = 8 9 7 4 8 9: " + dec((long long)w[0]) + " " + dec((long long)w[1]) + " " + dec((long long)w[2]) + " " + dec((long long)w[3]) + " " + dec((long long)w[4]) + " " + dec((long long)w[5]) +
            " cycles = " + num2(double(w[0]) / double(T)) + " " + num2(double(w[1]) / double(T)) + " " + num2(double(w[2]) / double(T)) + " " + num2(double(w[3]) / double(T)) + " T (the HRM's single start bit gives (p+1)T, recorded, not graded)");
    print_std(ck, "P18", agg, 1);
    ck.expect(gbrk, "P18-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 (AA p.10 '11 UARTBRK')");
    ck.expect(gt, "P18-G-T: every TxD edge lies on a grid of pitch T from its frame's first edge, within +-c, and the shortest edge interval is T within +-c (AA p.27 SERPER)");
    ck.expect(idle12 && found, "P18-PRE-IDLE: TxD is constant for 12T before each write and each write is followed by a frame");
    bool pre = agg.fixture && agg.s0 && agg.gc && gbrk && gt && idle12 && found;
    const std::string un = "fixture, G-S0, G-C, G-BRK, G-T or the idle precondition";
    graded(ck, pre, d9 && d7 && d4, "P18-W: w(p) - w(8) = (p - 8)*T within +-c for p = 9, 7 and 4 (AA p.27 'The length of the data word is set by the position of the stop bit')", un);
    graded(ck, pre, r8 && r9, "P18-REPEAT: both repeats of p = 8 and of p = 9 agree within c (AA p.27)", un);
    graded(ck, pre, back_idle, "P18-STOP: after each low interval TxD is at the stop level and stays there until the next write (AA p.27 'A stop bit must be provided as part of the data word')", un);
    graded(ck, pre, d9 && d7 && d4 && r8 && r9 && back_idle, "P18 sol:AA-serdat.c10: moving the stop bit up by one position lengthens the transmitted data word by one bit time (AA p.27 'The length of the data word is set by the position of the stop bit')", un);
}

// ---- P19: SERDATR reads the receive buffer -----------------------------------------------

inline void run_p19(Checks& ck) {
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    Prefix pre0 = prefix(s);
    int m_ser = s.w16(SERPER, 0x003F);
    const unsigned e[8] = {0x24, 0xC3, 0x7E, 0x99, 0x00, 0xFF, 0x42, 0xBD};
    unsigned eg[8]; for (int i = 0; i < 8; ++i) eg[i] = e[i];
    unsigned ewr[8]; for (int i = 0; i < 8; ++i) ewr[i] = e[i];
    if (mut("P19-E4")) ewr[3] = 0x5A;
    if (mut("P19-E7")) ewr[6] = 0x5A;
    const uint64_t K0 = 8;   // clk_sys cycles per colour clock; the schedule is in units of T = 64 K cycles and checked by G-C
    const uint64_t T = 64 * K0;
    RxDrive drv; drv.marker_idx = m_ser + 1; drv.invert = mut("GRXD");
    // schedule
    uint64_t t = 20 * T;
    uint64_t start[9] = {}, lam[9] = {};
    for (int k = 0; k < 8; ++k) {
        start[k] = t;
        lam[k] = t + 8 * T;                         // start of the last data cell (data cell 8 starts at start + 8T)
        if (!(mut("P19-F4") && k == 3)) rx_frame(drv.sched, t, ewr[k], T);
        t += 10 * T;
        if (k <= 5) t += 5 * T;
    }
    uint64_t t_endseq = t;                          // end of frame 8's stop cell
    uint64_t sigma[8];
    for (int k = 0; k < 7; ++k) sigma[k] = start[k + 1];
    sigma[7] = t_endseq + 5 * T;
    uint64_t t_low = t_endseq + 20 * T;
    drv.sched.push_back({t_low, 0});
    drv.sched.push_back({t_low + 12 * T, 1});
    rs.drive = [&drv](nmh::NanomigHarness& h, uint64_t cy) { drv(h, cy); };
    unsigned total = unsigned((t_low + 16 * T) / (11 * K0)) + 40;   // reads of about 11 colour clocks each
    int i_poll = pre0.logn;
    if (mut("P19-R")) { for (unsigned i = 0; i < total / 8 + 20; ++i) { s.rd(SERDATR); s.dly(12); } }   // contrast: a poll loop with a delay, so that R exceeds the bound
    else s.poll(SERDATR, total);
    s.end();
    rs.cycles = 4'000'000 + t_low + 40 * T + 400000;
    Cap c = run(rs);
    Std st = std_pre(c, pre0, {});
    PreAgg agg; agg.add(st);
    note_gc(ck, "P19", st);
    const uint64_t K = st.frame.K ? st.frame.K : 8;
    // time base: the RxD schedule is relative to the SERPER marker (after the write)
    uint64_t tref = drv.t_ref;
    size_t ip0 = size_t(i_poll);
    uint64_t R = max_gap(c, ip0, c.log.size() - 1);
    bool gr = R > 0 && R <= 16 * K;
    // G-RXD: bit 11 follows the driven level, away from the edges by 2R
    bool grxd = true;
    {
        uint64_t u_low = tref + t_low, u_hi = tref + t_low + 12 * T;
        size_t n_hi = 0, n_lo = 0;
        for (size_t i = ip0; i < c.log.size(); ++i) {
            uint64_t ti = c.log[i].first; int b = (c.log[i].second >> 11) & 1;
            if (ti >= tref + t_endseq + 2 * R && ti < u_low - 2 * R) { ++n_hi; if (b != (drv.invert ? 0 : 1)) grxd = false; }
            if (ti >= u_low + 2 * R && ti < u_hi) { ++n_lo; if (b != (drv.invert ? 1 : 0)) grxd = false; }
        }
        grxd &= n_hi > 10 && n_lo > 10;
    }
    // the check
    bool all_ok = true, back_ok = true;
    std::string det;
    for (int k = 0; k < 8; ++k) {
        uint64_t lo = tref + sigma[k] + R + (k == 6 ? T : 0);
        uint64_t hi = k < 7 ? tref + lam[k + 1] : tref + t_low;
        size_t n = 0, bad = 0;
        for (size_t i = ip0; i < c.log.size(); ++i) {
            uint64_t ti = c.log[i].first;
            if (ti >= lo && ti < hi) { ++n; if ((c.log[i].second & 0xFF) != eg[k]) ++bad; }
        }
        bool good = n >= 1 && bad == 0;
        if (k == 6) {
            // for the back-to-back pair e7 is read at least once between sigma_7 + T + R and lambda_8
            size_t n7 = 0;
            for (size_t i = ip0; i < c.log.size(); ++i) { uint64_t ti = c.log[i].first; if (ti >= lo && ti < hi && (c.log[i].second & 0xFF) == eg[6]) ++n7; }
            back_ok &= n7 >= 1;
        }
        all_ok &= good;
        det += dec(k + 1) + ":" + dec((long long)n) + "/" + dec((long long)bad) + " ";
    }
    ck.note("P19: R=" + dec((long long)R) + " cycles (" + num2(double(R) / double(K)) + " CCK); frames 1..8 (reads in the interval / reads that differ) " + det);
    print_std(ck, "P19", agg, 1);
    ck.expect(gr, "P19-G-R: R, the largest interval between consecutive log stores of the SERDATR poll, is at most T/4 = 16 colour clocks (SPECIFICATION.md 2.3)");
    ck.expect(grxd, "P19-G-RXD: SERDATR bit 11 reads the level the test drives on RxD: 1 while it drives the idle level, 0 during a 12T low (AA p.27 '11 RXD RXD pin receives UART serial data for direct bit test by the micro')");
    bool pre19 = agg.fixture && agg.s0 && agg.gc && gr && grxd;
    const std::string un = "fixture, G-S0, G-C, G-R or G-RXD";
    graded(ck, pre19, all_ok, "P19-DATA: for every frame k = 1..8 every read stored between sigma_k + R and lambda_(k+1) (sigma_7 + T + R for k = 7, sigma_8 for frame 8) reads e_k in bits 7 to 0 (AA p.27 'Data in this buffer is loaded from a receiving shift register whenever it is full')", un);
    graded(ck, pre19, back_ok, "P19-BACK: for the back-to-back pair e7 is read at least once between sigma_7 + T + R and lambda_8 (AA p.27)", un);
    graded(ck, pre19, all_ok && back_ok, "P19 sol:AA-serdatr.c6 sol:AA-serdatr.c7: each frame received on RxD is loaded into the receive buffer when the shift register is full and SERDATR bits 7 to 0 then read its data byte until the next frame is loaded, whether or not the program has acknowledged the previous one (AA p.27 'This address reads data from a Receive data buffer')", un);
}

// ---- P20: RBF and TBE read through SERDATR ----------------------------------------------

inline void run_p20(Checks& ck) {
    const uint64_t K0 = 8, T = 64 * K0;
    // run A: RBF
    RunSpec ra; ra.bin = run_bin();
    Prefix pa = prefix(ra.sc, true, 119, brk_hook());
    int m_ser = ra.sc.w16(SERPER, 0x003F);
    RxDrive drv; drv.marker_idx = m_ser + 1; drv.invert = mut("GRXD");
    rx_frame(drv.sched, 20 * T, 0x5A, T);
    uint64_t t_low = 40 * T;
    drv.sched.push_back({t_low, 0});
    drv.sched.push_back({t_low + 12 * T, 1});
    ra.drive = [&drv](nmh::NanomigHarness& h, uint64_t cy) { drv(h, cy); };
    Script& s = ra.sc;
    s.waitb(INTREQR, 0x0800, 400);
    s.dly(wait_iters(5 * 64));
    const bool no_set = mut("P20-NOSET"), no_int = mut("P20-NOINT");
    s.rd3(SERDATR, INTREQR);                                                   // triple 1
    int ma1 = s.w16(INTREQ, no_int ? uint16_t(0) : uint16_t(0x0800));
    s.rd3(SERDATR, INTREQR);                                                   // triple 2
    int ma2 = s.w16(INTREQ, (no_int || no_set) ? uint16_t(0) : uint16_t(0x8800));
    s.rd3(SERDATR, INTREQR);                                                   // triple 3
    int ma3 = s.w16(INTREQ, no_int ? uint16_t(0) : uint16_t(0x0800));
    s.rd3(SERDATR, INTREQR);                                                   // triple 4
    s.poll(SERDATR, 150);
    s.end();
    ra.cycles = 4'000'000 + 8ull * 40000;
    Cap ca = run(ra);
    // run B: TBE
    RunSpec rb; rb.bin = run_bin();
    Prefix pb = prefix(rb.sc, true, 119, brk_hook());
    Script& t = rb.sc;
    t.w16(SERPER, 0x003F);
    t.dly(wait_iters(14 * 64));
    t.w16(INTREQ, 0x0001);
    t.w16(SERDAT, 0x0181);
    t.waitb(INTREQR, 0x0001, 400);
    t.rd3(SERDATR, INTREQR);                                                   // case (a)
    int mb1 = t.w16(INTREQ, 0x0001);
    int mb2 = t.w16(SERDAT, 0x01FF);
    if (mut("P20-LATE")) t.dly(wait_iters(14 * 64));                           // contrast: the triple after the second word has moved
    t.rd3(SERDATR, INTREQR);                                                   // case (b)
    t.waitb(INTREQR, 0x0001, 600);
    t.rd3(SERDATR, INTREQR);                                                   // case (a) again
    t.dly(wait_iters(14 * 64));
    t.end();
    rb.cycles = 4'000'000 + 8ull * 40000;
    Cap cb = run(rb);

    Std sa = std_pre(ca, pa, {}), sb = std_pre(cb, pb, {});
    PreAgg agg; agg.add(sa); agg.add(sb);
    note_gc(ck, "P20 run A", sa);
    // a triple is three consecutive log stores: the last three before a time, or the first three after one
    auto before = [](const Cap& c, uint64_t t, std::array<uint16_t, 3>& out) {
        size_t n = 0; while (n < c.log.size() && c.log[n].first < t) ++n;
        if (n < 3) return false;
        out = {c.log[n - 3].second, c.log[n - 2].second, c.log[n - 1].second};
        return true;
    };
    auto after = [](const Cap& c, uint64_t t, std::array<uint16_t, 3>& out) {
        size_t n = 0; while (n < c.log.size() && c.log[n].first <= t) ++n;
        if (n + 3 > c.log.size()) return false;
        out = {c.log[n].second, c.log[n + 1].second, c.log[n + 2].second};
        return true;
    };
    std::array<std::array<uint16_t, 3>, 4> ta{};
    bool okA = before(ca, marker(ca, ma1), ta[0]) && after(ca, marker(ca, ma1 + 1), ta[1]) && after(ca, marker(ca, ma2 + 1), ta[2]) && after(ca, marker(ca, ma3 + 1), ta[3]);
    std::array<std::array<uint16_t, 3>, 3> tb{};
    bool okB = before(cb, marker(cb, mb1), tb[0]) && after(cb, marker(cb, mb2 + 1), tb[1]) && ca.log.size() > 0 && cb.log.size() >= 3;
    if (okB) tb[2] = {cb.log[cb.log.size() - 3].second, cb.log[cb.log.size() - 2].second, cb.log[cb.log.size() - 1].second};
    size_t endA = ca.log.size();
    auto bitv = [](uint16_t v, int b) { return (v >> b) & 1; };
    bool a_valid[4], a_ok = okA, a_pat = okA;
    int seen11[2] = {0, 0};
    for (int i = 0; i < 4 && okA; ++i) {
        a_valid[i] = bitv(ta[size_t(i)][0], 14) == bitv(ta[size_t(i)][2], 14);
        int ir = bitv(ta[size_t(i)][1], 11);
        ++seen11[ir];
        if (a_valid[i]) a_ok &= bitv(ta[size_t(i)][0], 14) == ir;
        int want = (i == 0 || i == 2) ? 1 : 0;
        if (a_valid[i]) a_pat &= ir == want && bitv(ta[size_t(i)][0], 14) == want;
    }
    bool b_valid[3], b_ok = okB, b_pat = okB;
    for (int i = 0; i < 3 && okB; ++i) {
        b_valid[i] = bitv(tb[size_t(i)][0], 13) == bitv(tb[size_t(i)][2], 13);
        int ir = bitv(tb[size_t(i)][1], 0);
        if (b_valid[i]) b_ok &= bitv(tb[size_t(i)][0], 13) == ir;
        int want = i == 1 ? 0 : 1;
        if (b_valid[i]) b_pat &= ir == want && bitv(tb[size_t(i)][0], 13) == want;
    }
    bool pre_a = okA && a_valid[1] && a_valid[2] && seen11[0] > 0 && seen11[1] > 0;
    if (okA) ck.note("P20 run A triples (SERDATR, INTREQR, SERDATR): " + hex4(ta[0][0]) + "," + hex4(ta[0][1]) + "," + hex4(ta[0][2]) + " | " + hex4(ta[1][0]) + "," + hex4(ta[1][1]) + "," + hex4(ta[1][2]) + " | " +
                       hex4(ta[2][0]) + "," + hex4(ta[2][1]) + "," + hex4(ta[2][2]) + " | " + hex4(ta[3][0]) + "," + hex4(ta[3][1]) + "," + hex4(ta[3][2]));
    if (okB) ck.note("P20 run B triples: " + hex4(tb[0][0]) + "," + hex4(tb[0][1]) + "," + hex4(tb[0][2]) + " | " + hex4(tb[1][0]) + "," + hex4(tb[1][1]) + "," + hex4(tb[1][2]) + " | " + hex4(tb[2][0]) + "," + hex4(tb[2][1]) + "," + hex4(tb[2][2]));
    // G-RXD from run A: bit 11 during the final idle and the 12T low
    bool grxd = true;
    {
        uint64_t tref = drv.t_ref, R = 0;
        size_t ip0 = endA >= 151 ? endA - 151 : 0;
        R = max_gap(ca, ip0, endA - 1);
        size_t n_hi = 0, n_lo = 0;
        for (size_t i = ip0; i < endA; ++i) {
            uint64_t ti = ca.log[i].first; int b = bitv(ca.log[i].second, 11);
            if (ti >= tref + t_low + 2 * R && ti < tref + t_low + 12 * T) { ++n_lo; if (b != (drv.invert ? 1 : 0)) grxd = false; }
            if (ti < tref + t_low - 2 * R) { ++n_hi; if (b != (drv.invert ? 0 : 1)) grxd = false; }
        }
        grxd &= n_lo > 10;
        (void)n_hi;
    }
    bool gbrk = (logv(ca, pa.i_adkconr) & 0x0800) == 0 && (logv(cb, pb.i_adkconr) & 0x0800) == 0;
    print_std(ck, "P20", agg, 2);
    ck.expect(gbrk, "P20-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 in both runs (AA p.10 '11 UARTBRK')");
    ck.expect(grxd, "P20-G-RXD: SERDATR bit 11 reads 0 during the 12T low the test drives on RxD (AA p.27 '11 RXD')");
    ck.expect(pre_a, "P20-PRE-TRIPLES: in run A triples 2 and 3 are valid (their two SERDATR reads agree in bit 14) and INTREQR bit 11 takes both values across the four triples (AA p.25 '11 RBF'; AA p.24 'Both a set and clear operation are required to load arbitary data into this register')");
    bool pre = agg.fixture && agg.s0 && agg.gc && gbrk && grxd && pre_a;
    const std::string un = "fixture, G-S0, G-C, G-BRK, G-RXD or the triples";
    graded(ck, pre, a_ok && a_pat, "P20-RBF: in every valid triple of run A SERDATR bit 14 equals INTREQR bit 11, and triples 1 and 3 read (1, 1) and triples 2 and 4 read (0, 0) (AA p.27 '14 RBF Serial port Receive Buffer Full(mirror)')", un);
    graded(ck, pre, b_ok && b_pat, "P20-TBE: in every valid triple of run B SERDATR bit 13 equals INTREQR bit 0, the step-2 and step-4 triples read (1, 1) and the step-3 triple reads (0, 0) (AA p.27 '13 TBE Serial port Transmit Buffer Empty(mirror)')", un);
    graded(ck, pre, a_ok && a_pat && b_ok && b_pat, "P20 sol:AA-serdatr.c8: SERDATR bit 14 reads the same as INTREQR bit 11 (RBF) at all times and bit 13 the same as INTREQR bit 0 (TBE) in the two cases on which the pages agree (AA p.27 'Several interrupt request bits are also read at this address, along with the data')", un);
}

} // namespace s5
