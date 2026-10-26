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

// G-T: every edge of the capture lies on the grid of pitch T from the first edge of its frame, exactly, where T = (N+1) colour clocks
// (AA p.27 'If this number is N, then the Baud Rate is 1 bit every (N+1)*.2794 Microseconds'); with single_cell, the shortest interval
// between two edges is T (a capture with a cell of one bit time between two edges: the frames of P16 and P17).
inline bool g_t(const Cap& c, const std::vector<TxFrame>& fr, uint64_t t0, uint64_t t1, uint64_t T, bool single_cell, uint64_t& tmeas) {
    auto ed = tx_edges(c, t0, t1);
    tmeas = 0;
    for (size_t i = 1; i < ed.size(); ++i) { uint64_t d = ed[i] - ed[i - 1]; if (!tmeas || d < tmeas) tmeas = d; }
    bool ok = !fr.empty() && !ed.empty();
    if (single_cell) ok &= tmeas == T;
    for (uint64_t e : ed) {
        bool on = false;
        for (auto& f : fr) {
            if (e < f.s || e > f.s + 12 * T) continue;
            if ((e - f.s) % T == 0) on = true;
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
    std::vector<int> m_c, m_w;          // markers of the INTREQ clear and of the SERDAT write before words 2..8
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
    uint16_t cval = mut("P17-NOCLEAR") ? uint16_t(0) : uint16_t(1);
    // Words 2..8 are written after fixed numbers of poll reads, whatever TBE reads: before each, INTREQ <- 1 clears the TBE request
    // (a marker pair around each of the two writes logs when they happened). Word 2 follows word 1 by about 5 bit times and each later
    // word follows by about 10, the length of a frame, so that word k is written in the middle of frame k-1: after it began (the
    // buffer is empty) and before it ends (the word waits in the buffer). The read counts are tuned on this design and the
    // position of every write in its frame is printed.
    for (int k = 1; k < 8; ++k) {
        const unsigned gap = k == 1 ? 14 : (variant == "EARLYW" && k == 2 ? 8 : 42);   // EARLYW: word 3 follows word 2 by about 3 bit times
        if (variant == "DELAY" && k == 4) s.dly(wait_iters(30 * 64));   // contrast: word 5 only after 30 bit times of idle
        if (mut("P17-R")) { for (unsigned i = 0; i < gap; ++i) { s.rd(INTREQR); s.dly(8); } }
        else s.poll(INTREQR, gap);
        r.pre.logn += gap + (mut("P17-R") ? 0 : 1);
        r.m_c.push_back(s.w16(INTREQ, cval));
        r.m_w.push_back(s.w16(SERDAT, uint16_t(0x100 | wd[size_t(k)])));
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
    std::string var = mut("P16-D3") ? "D3" : (mut("P16-DELAY") ? "DELAY" : (mut("P17-X") ? "X" : (mut("P16-X") ? "X16" : (mut("P16-EXTRA") ? "EXTRA" : (mut("P16-EARLYW") ? "EARLYW" : "")))));
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
    bool gt = g_t(c, frames, t_ser0 + 1, t_end, T, true, tmeas);
    bool gbrk = (logv(c, r.pre.i_adkconr) & 0x0800) == 0;
    // the idle precondition: TxD constant from the SERPER marker to the first write, for at least 20T
    uint64_t t_ser = marker(c, r.m_ser + 1);
    bool idle_ok = true;
    for (uint64_t u = t_ser; u < r.t_w1; ++u) if (tx_at(c, u) != idle) idle_ok = false;
    uint64_t idle_len = r.t_w1 > t_ser ? r.t_w1 - t_ser : 0;
    // the stores of the poll loop (everything from the first INTREQR poll on)
    size_t ip0 = size_t(r.i_poll0);
    // R: the largest interval between consecutive log stores of the loop; the intervals that hold a marker (a write) are no loop interval
    uint64_t R = 0;
    for (size_t i = ip0 + 1; i < c.log.size(); ++i) {
        uint64_t a = c.log[i - 1].first, b = c.log[i].first;
        bool skip = false;
        for (uint64_t mt : c.mark_t) if (mt > a && mt < b) skip = true;
        if (!skip) R = std::max(R, b - a);
    }
    ck.note("P16: T=" + dec((long long)T) + " cycles, measured shortest edge interval " + dec((long long)tmeas) + ", frames found " + dec((long long)frames.size()) + ", idle before the first write " + dec((long long)idle_len) + " cycles (" + num2(double(idle_len) / double(T)) + " T), R=" + dec((long long)R) + " cycles (" + num2(double(R) / double(K)) + " CCK), idle level " + dec(idle));
    {
        std::string fs;
        for (auto& f : frames) fs += hex4(f.lsb) + (f.lsb == f.msb ? "" : "!") + (f.framed ? "" : "?") + "@" + dec((long long)((double(f.s) - double(r.t_w1)) / double(T) * 10) ) + " ";
        ck.note("P16 frames (byte, start in tenths of T after the first write marker): " + fs);
    }
    {
        std::string ds;
        for (size_t k = 1; k < frames.size(); ++k) ds += dec((long long)(frames[k].s - frames[k - 1].s) - (long long)(10 * T)) + " ";
        ck.note("P16 start of frame k minus the start of frame k-1 minus 10T (cycles), k = 2..: " + ds + "(measured, not graded: no page fixes the reload latency below one bit cell)");
    }
    // where each word k = 2..8 was written against frame k-1 (cycles after the start of frame k-1, in tenths of T): the word waits in
    // the buffer if this is between 0 and 10 T. Recorded, not a precondition: the contrast that writes word 5 late must reach check (d).
    {
        std::string ws;
        std::vector<TxFrame> f17;
        for (auto& f : frames) if (f.s > r.t_w1) f17.push_back(f);
        for (size_t k = 2; k <= 8 && k - 2 < r.m_w.size(); ++k) {
            uint64_t w = marker(c, r.m_w[k - 2] + 1);
            if (f17.size() >= k - 1) ws += dec((long long)k) + ":" + dec((long long)((double(w) - double(f17[k - 2].s)) / double(T) * 10)) + " ";
        }
        ck.note("P16 write of word k against the start of frame k-1 (tenths of T): " + ws);
    }
    // every word k = 2..8 is written after frame k-1 began: the word before it has moved, and no waiting word is overwritten
    bool empty_ok = true;
    {
        std::vector<TxFrame> f17;
        for (auto& f : frames) if (f.s > r.t_w1) f17.push_back(f);
        for (size_t k = 2; k <= 8; ++k) empty_ok &= k - 2 < r.m_w.size() && f17.size() >= k - 1 && marker(c, r.m_w[k - 2]) > f17[k - 2].s;
    }
    // P16 expectations
    std::vector<uint8_t> d = r.d;   // the grader keeps d3 = 0x5A whatever the stimulus wrote
    bool a_ok = true;
    for (uint64_t u = t0; u <= r.t_w1 && u - c.base < c.txd.size(); ++u) if (tx_at(c, u) != idle) a_ok = false;
    bool b_ok = frames.size() == 8;
    for (size_t k = 0; k < frames.size() && k < 8; ++k) b_ok &= frames[k].framed && frames[k].lsb == d[k] && frames[k].msb == d[k];
    bool c_ok = !frames.empty() && frames[0].s > r.t_w1;
    bool d_ok = frames.size() >= 2;
    for (size_t k = 1; k < frames.size() && k < 8; ++k) d_ok &= frames[k].s < frames[k - 1].s + 11 * T;
    bool e_ok = !frames.empty();
    if (e_ok) {
        auto ed = tx_edges(c, frames.back().s + 10 * T, t_end);
        e_ok &= ed.empty() && tx_at(c, t_end) == idle && frames.size() == 8;
    }
    print_std(ck, "P16", agg, 1);
    ck.expect(gbrk, "P16-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 (AA p.10 '11 UARTBRK Forces a UART break (clears TXD) if true')");
    ck.expect(gt, "P16-G-T: every TxD edge lies on a grid of pitch T = (N+1) colour clocks from its frame's first edge, exactly, and the shortest edge interval is T (AA p.27 'If this number is N, then the Baud Rate is 1 bit every (N+1)*.2794 Microseconds')");
    ck.expect(idle_len >= 20 * T, "P16-PRE-IDLE: the wait from the SERPER write to the first write lasts at least 20T");
    ck.expect(empty_ok, "P16-PRE-EMPTY: each of the words 2 to 8 is written after the frame of the word before it began, so that the word before it has moved and the one-word buffer does not lose a waiting word");
    bool pre = agg.fixture && agg.s0 && agg.gc && gbrk && gt && idle_len >= 20 * T && empty_ok;
    const std::string un = "fixture, G-S0, G-C, G-BRK, G-T, the idle wait or the empty buffer at each write";
    graded(ck, pre, a_ok, "P16-A: before the first write TxD is constant (AA p.27 'Data from this buffer is moved into a serial shift register for output transmission, whenever it is empty')", un);
    graded(ck, pre, b_ok, "P16-B: exactly eight frames appear and frame k carries d_k, each with a start cell and a stop cell at the stop level: none missing, repeated or extra (AA p.27 'This address writes data to a Transmit data buffer')", un);
    graded(ck, pre, c_ok, "P16-C: frame 1's start cell begins after the first write marker (AA p.27)", un);
    graded(ck, pre, d_ok, "P16-D: for k = 2..8 frame k's start cell begins less than 11T after frame k-1's start cell, so that no complete idle cell lies between the frames: the word waiting in the buffer moves when the shift register is empty (AA p.27 'moved into a serial shift register for output transmission, whenever it is empty')", un);
    graded(ck, pre, e_ok, "P16-E: after frame 8's stop cell TxD stays idle until the end of the capture (AA p.27)", un);
    graded(ck, pre, a_ok && b_ok && c_ok && d_ok && e_ok, "P16 sol:AA-serdat.c6 sol:AA-serdat.c7: every word written to SERDAT goes to the transmit buffer and is transmitted on TxD once, in the order written, and a word waiting in the buffer moves into the shift register as soon as it is empty, so its frame follows the previous frame with no idle cell (AA p.27)", un);

    // ---- P17 ----
    std::vector<TxFrame> fr17;   // the frames of the eight words: those that start after the first write
    for (auto& f : frames) if (f.s > r.t_w1) fr17.push_back(f);
    bool zero_ok = true, one_ok = true, clear_ok = true;
    std::string det;
    for (int k = 1; k <= 8; ++k) {
        uint64_t e, lo;
        if (k == 1) { e = r.t_w1; lo = c.log[size_t(r.pre.i_adkconr)].first; }
        else {
            uint64_t wr_early = marker(c, r.m_w[size_t(k - 2)]);     // the write of word k happens after this store
            e = wr_early;
            if (fr17.size() >= size_t(k - 1)) e = std::max(wr_early, fr17[size_t(k - 2)].s + 9 * T);   // the stop cell of frame k-1
            lo = marker(c, r.m_c[size_t(k - 2)] + 1);                // the clear of the TBE request happened before this store
            size_t j = ip0;
            while (j < c.log.size() && c.log[j].first <= lo) ++j;
            clear_ok &= j < c.log.size() && !(c.log[j].second & 1);   // the first read after the clear
            if (std::getenv("NMH_DEBUG")) ck.note("P17 debug k=" + dec(k) + " first read after the clear: " + hex4(j < c.log.size() ? c.log[j].second : 0xFFFF) + " at " + dec(j < c.log.size() ? (long long)c.log[j].first - (long long)lo : -1LL) + " cycles after the clear marker");
        }
        for (size_t i = ip0; i < c.log.size(); ++i) {
            uint64_t ti = c.log[i].first;
            if (ti > lo && ti < e && (c.log[i].second & 1)) zero_ok = false;
        }
        // the first read of 1 after the clear: TBE is set by the move, which precedes the start of frame k; a read of 1 follows by the second store after it
        bool found = false; uint64_t t1 = 0;
        for (size_t i = ip0; i < c.log.size(); ++i) if (c.log[i].first > lo && (c.log[i].second & 1)) { t1 = c.log[i].first; found = true; break; }
        long long sk_rel = 0;
        if (fr17.size() >= size_t(k)) {
            uint64_t sk = fr17[size_t(k - 1)].s;
            size_t i1 = ip0;
            while (i1 < c.log.size() && c.log[i1].first <= sk) ++i1;
            one_ok &= found && i1 + 1 < c.log.size() && t1 <= c.log[i1 + 1].first;
            sk_rel = found ? (long long)t1 - (long long)sk : 0LL;
        } else one_ok = false;   // frame k does not appear: no move is seen
        det += dec(k) + ":e-lo=" + dec((long long)e - (long long)lo) + ",first1-s=" + dec(sk_rel) + " ";
    }
    // the clear before word 1 is S0's: INTREQR read after S0 reads 0
    clear_ok &= !(logv(c, r.pre.i_intreqr) & 1);
    ck.note("P17: R=" + dec((long long)R) + " cycles; per move k: window e_k - first clear read, and the first read of 1 minus the start of frame k (cycles): " + det);
    print_std(ck, "P17", agg, 1);
    ck.expect(gbrk, "P17-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 (AA p.10 '11 UARTBRK')");
    ck.expect(gt, "P17-G-T: every TxD edge lies on a grid of pitch T from its frame's first edge, exactly, and the shortest edge interval is T (AA p.27 SERPER)");
    ck.expect(R > 0 && R <= 32 * K, "P17-G-R: R, the largest interval between consecutive log stores of the poll loop, is at most T/2 = 32 colour clocks (SPECIFICATION.md 2.3)");
    ck.expect(clear_ok && !(logv(c, r.pre.i_intenar) & 0x7FFF), "P17-PRE-CLEAR: INTENAR reads 0 and the first read after each clear reads 0 (AA p.25 INTENA, INTREQ set/clear)");
    ck.expect(empty_ok, "P17-PRE-EMPTY: each of the words 2 to 8 is written after the frame of the word before it began (as P16-PRE-EMPTY)");
    bool pre17 = agg.fixture && agg.s0 && agg.gc && gbrk && gt && R > 0 && R <= 32 * K && clear_ok && empty_ok;
    const std::string un17 = "fixture, G-S0, G-C, G-BRK, G-T, G-R, the clear precondition or the empty buffer at each write";
    graded(ck, pre17, zero_ok, "P17-ZERO: for every move k = 1..8 every read stored after the clear before word k and before the earliest instant e_k of the move reads 0 in INTREQR bit 0 (AA p.27 'This sets the Interrupt Request TBE (transmit buffer empty)'; AA p.25 '00 TBE 1 Serial port Transmit Buffer Empty')", un17);
    graded(ck, pre17, one_ok, "P17-ONE: for every move k = 1..8 a read of 1 in INTREQR bit 0 is stored no later than the second store after the start of frame k's start cell, and frame k appears: the request is set by the time the word is sent (AA p.27 'moved into a serial shift register for output transmission ... This sets the Interrupt Request TBE')", un17);
    graded(ck, pre17, zero_ok && one_ok, "P17 sol:AA-serdat.c8: every move of the transmit buffer into the shift register sets INTREQ bit 0 (TBE), and TBE stays clear while a written word still waits in the buffer (AA p.27 'This sets the Interrupt Request TBE (transmit buffer empty)')", un17);
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
    // G-T: the edges of every frame lie, exactly, on the grid of pitch T = (N+1) colour clocks from the frame's first edge
    // (a frame of P18 is all low from its start edge to its stop edge and has no single-cell pulse)
    bool gt = g_t(c, frames, tfirst > 12 * T ? tfirst - 12 * T : c.base + 1, t_end, T, false, tmeas);
    auto dif = [&](int i, int j) { return int64_t(w[size_t(i)]) - int64_t(w[size_t(j)]); };
    bool d9 = found && dif(1, 0) == int64_t(T);
    bool d7 = found && dif(2, 0) == -int64_t(T);
    bool d4 = found && dif(3, 0) == -int64_t(4 * T);
    bool r8 = found && dif(4, 0) == 0;
    bool r9 = found && dif(5, 1) == 0;
    ck.note("P18: T=" + dec((long long)T) + " cycles; w(p) for p = 8 9 7 4 8 9: " + dec((long long)w[0]) + " " + dec((long long)w[1]) + " " + dec((long long)w[2]) + " " + dec((long long)w[3]) + " " + dec((long long)w[4]) + " " + dec((long long)w[5]) +
            " cycles = " + num2(double(w[0]) / double(T)) + " " + num2(double(w[1]) / double(T)) + " " + num2(double(w[2]) / double(T)) + " " + num2(double(w[3]) / double(T)) + " T (the HRM's single start bit gives (p+1)T, recorded, not graded)");
    print_std(ck, "P18", agg, 1);
    ck.expect(gbrk, "P18-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 (AA p.10 '11 UARTBRK')");
    ck.expect(gt, "P18-G-T: every TxD edge lies on a grid of pitch T = (N+1) colour clocks from its frame's first edge, exactly (AA p.27 'If this number is N, then the Baud Rate is 1 bit every (N+1)*.2794 Microseconds')");
    ck.expect(idle12 && found, "P18-PRE-IDLE: TxD is constant for 12T before each write and each write is followed by a frame");
    bool pre = agg.fixture && agg.s0 && agg.gc && gbrk && gt && idle12 && found;
    const std::string un = "fixture, G-S0, G-C, G-BRK, G-T or the idle precondition";
    graded(ck, pre, d9 && d7 && d4, "P18-W: w(p) - w(8) = (p - 8)*T exactly for p = 9, 7 and 4 (AA p.27 'The length of the data word is set by the position of the stop bit')", un);
    graded(ck, pre, r8 && r9, "P18-REPEAT: both repeats of p = 8 and of p = 9 agree exactly (AA p.27)", un);
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
    const uint64_t K0 = mut("P19-K") ? 9 : 8;   // clk_sys cycles per colour clock that the schedule is built for (units of T = 64 K0 cycles); P19-PRE-K compares it with the K measured from refresh
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
    bool gk = st.frame.ok && 64 * st.frame.K == T;   // the bit time that the RxD drive uses is 64 colour clocks of the measured machine
    // G-RXD: bit 11 follows the driven level, away from the edges by 2R
    bool grxd = true;
    {
        uint64_t u_low = tref + t_low, u_hi = tref + t_low + 12 * T;
        size_t n_hi = 0, n_lo = 0;
        for (size_t i = ip0; i < c.log.size(); ++i) {
            uint64_t ti = c.log[i].first; int b = (c.log[i].second >> 11) & 1;
            if (ti >= tref + t_endseq + 2 * R && ti < u_low - 2 * R) { ++n_hi; if (b != 1) grxd = false; }
            if (ti >= u_low + 2 * R && ti < u_hi) { ++n_lo; if (b != 0) grxd = false; }
        }
        grxd &= n_hi > 10 && n_lo > 10;
    }
    // the check
    bool all_ok = true, back_ok = true;
    std::string det;
    for (int k = 0; k < 8; ++k) {
        // frame k is in the receive buffer by the end of its stop cell plus one cell (a receiver that samples once per cell can see the
        // start edge up to one cell late), whether or not another frame follows; the buffer keeps it until frame k+1 can be loaded
        uint64_t lo = tref + start[k] + 11 * T + R;
        uint64_t hi = k < 7 ? tref + lam[k + 1] : tref + t_low;
        size_t n = 0, bad = 0;
        for (size_t i = ip0; i < c.log.size(); ++i) {
            uint64_t ti = c.log[i].first;
            if (ti >= lo && ti < hi) { ++n; if ((c.log[i].second & 0xFF) != eg[k]) ++bad; }
        }
        bool good = n >= 1 && bad == 0;
        if (k == 6) {
            // for the back-to-back pair e7 is read at least once between the end of frame 7's stop cell plus T plus R and lambda_8
            size_t n7 = 0;
            for (size_t i = ip0; i < c.log.size(); ++i) { uint64_t ti = c.log[i].first; if (ti >= lo && ti < hi && (c.log[i].second & 0xFF) == eg[6]) ++n7; }
            back_ok &= n7 >= 1;
        }
        all_ok &= good;
        det += dec(k + 1) + ":" + dec((long long)n) + "/" + dec((long long)bad) + " ";
    }
    ck.note("P19: R=" + dec((long long)R) + " cycles (" + num2(double(R) / double(K)) + " CCK); frames 1..8 (reads in the interval / reads that differ) " + det);
    print_std(ck, "P19", agg, 1);
    ck.expect(gk, "P19-PRE-K: the bit time of the RxD schedule, " + dec((long long)T) + " system clocks, is 64 colour clocks of the colour clock measured from the refresh pattern (SPECIFICATION.md 2.2)");
    ck.expect(gr, "P19-G-R: R, the largest interval between consecutive log stores of the SERDATR poll, is at most T/4 = 16 colour clocks (SPECIFICATION.md 2.3)");
    ck.expect(grxd, "P19-G-RXD: SERDATR bit 11 reads the level the test drives on RxD: 1 while it drives the idle level, 0 during a 12T low (AA p.27 '11 RXD RXD pin receives UART serial data for direct bit test by the micro')");
    bool pre19 = agg.fixture && agg.s0 && agg.gc && gk && gr && grxd;
    const std::string un = "fixture, G-S0, G-C, the colour-clock length, G-R or G-RXD";
    graded(ck, pre19, all_ok, "P19-DATA: for every frame k = 1..8 every read stored between the end of frame k's stop cell plus T plus R and lambda_(k+1) (the start of the 12T low for frame 8) reads e_k in bits 7 to 0 (AA p.27 'Data in this buffer is loaded from a receiving shift register whenever it is full')", un);
    graded(ck, pre19, back_ok, "P19-BACK: for the back-to-back pair e7 is read at least once between the end of frame 7's stop cell plus T plus R and lambda_8 (AA p.27)", un);
    graded(ck, pre19, all_ok && back_ok, "P19 sol:AA-serdatr.c6 sol:AA-serdatr.c7: each frame received on RxD is loaded into the receive buffer when the shift register is full and SERDATR bits 7 to 0 then read its data byte until the next frame is loaded, whether or not the program has acknowledged the previous one (AA p.27 'This address reads data from a Receive data buffer')", un);
}

// ---- P20: RBF and TBE read through SERDATR ----------------------------------------------

inline void run_p20(Checks& ck) {
    const uint64_t K0 = mut("P20-K") ? 9 : 8, T = 64 * K0;   // K0: clk_sys cycles per colour clock that the RxD schedule is built for; P20-PRE-K compares it with the K measured from refresh
    // run A: RBF
    RunSpec ra; ra.bin = run_bin();
    Prefix pa = prefix(ra.sc, true, 119, brk_hook());
    int m_ser = ra.sc.w16(SERPER, 0x003F);
    RxDrive drv, drv2; drv.marker_idx = m_ser + 1; drv.invert = drv2.invert = mut("GRXD");
    rx_frame(drv.sched, 20 * T, 0x5A, T);
    ra.drive = [&drv, &drv2](nmh::NanomigHarness& h, uint64_t cy) { drv(h, cy); drv2(h, cy); };
    Script& s = ra.sc;
    s.waitb(INTREQR, 0x0800, 400);
    s.dly(wait_iters(5 * 64));
    const bool no_set = mut("P20-NOSET"), no_int = mut("P20-NOINT");
    s.rd3(SERDATR, INTREQR);                                                   // triple 1: the frame has been received
    int ma1 = s.w16(INTREQ, no_int ? uint16_t(0) : uint16_t(0x0800));
    s.rd3(SERDATR, INTREQR);                                                   // triple 2: cleared by software
    int ma2 = s.w16(INTREQ, (no_int || no_set) ? uint16_t(0) : uint16_t(0x8800));
    s.rd3(SERDATR, INTREQR);                                                   // triple 3: set by software
    int ma3 = s.w16(INTREQ, no_int ? uint16_t(0) : uint16_t(0x0800));
    s.rd3(SERDATR, INTREQR);                                                   // triple 4: cleared by software
    const uint64_t t_low = mut("P20-NOIDLE") ? 0 : 6 * T;   // the 12T low is driven 6T after the marker that follows triple 4, so that the poll reads the idle level before it; P20-NOIDLE: at the marker, so that it reads none
    drv2.marker_idx = s.mark();
    drv2.sched.push_back({t_low, 0});
    drv2.sched.push_back({t_low + 12 * T, 1});
    s.poll(SERDATR, 200);
    s.end();
    ra.cycles = 4'000'000 + 8ull * 40000;
    Cap ca = run(ra);
    // run B: TBE
    RunSpec rb; rb.bin = run_bin(); rb.sample_txd = true;
    Prefix pb = prefix(rb.sc, true, 119, brk_hook());
    Script& t = rb.sc;
    int mb_ser = t.w16(SERPER, 0x003F);
    t.dly(wait_iters(14 * 64));
    t.w16(INTREQ, 0x0001);
    t.w16(SERDAT, 0x0181);
    t.waitb(INTREQR, 0x0001, 400);
    t.rd3(SERDATR, INTREQR);                                                   // case (a): word 1 has moved, the buffer is empty
    int mb1 = t.w16(INTREQ, mut("P20-SETB") ? uint16_t(0x8001) : uint16_t(0x0001));   // P20-SETB: the request is set by software in place of cleared
    int mb2 = t.w16(SERDAT, 0x01FF);
    if (mut("P20-LATE")) t.dly(wait_iters(14 * 64));                           // contrast: the triple after the second word has moved
    t.rd3(SERDATR, INTREQR);                                                   // case (b): the second word waits in the buffer, the request is clear
    t.waitb(INTREQR, 0x0001, 600);
    t.rd3(SERDATR, INTREQR);                                                   // case (a) again: word 2 has moved, the buffer is empty
    int mb3 = t.w16(INTREQ, mut("P20-NOCLRC") ? uint16_t(0x8001) : uint16_t(0x0001));   // case (c): cleared by software with the buffer empty; P20-NOCLRC: set in place
    t.rd3(SERDATR, INTREQR);                                                   // case (c)
    t.w16(SERDAT, 0x01FE);                                                     // word 3 waits in the buffer while frame 2 is sent
    int mb5 = t.w16(INTREQ, mut("P20-NOSETD") ? uint16_t(0x0001) : uint16_t(0x8001));   // case (d): set by software with the buffer full; P20-NOSETD: cleared in place
    t.rd3(SERDATR, INTREQR);                                                   // case (d)
    int mb6 = t.w16(INTREQ, mut("P20-NOCLRE") ? uint16_t(0x8001) : uint16_t(0x0001));   // case (e): cleared by software with the buffer full; P20-NOCLRE: set in place
    t.rd3(SERDATR, INTREQR);                                                   // case (e)
    t.dly(wait_iters(14 * 64));
    t.end();
    rb.cycles = 4'000'000 + 8ull * 40000;
    Cap cb = run(rb);

    Std sa = std_pre(ca, pa, {}), sb = std_pre(cb, pb, {});
    PreAgg agg; agg.add(sa); agg.add(sb);
    note_gc(ck, "P20 run A", sa);
    // a triple is three consecutive log stores: the last three before a time, or the first three after one
    auto before = [](const Cap& c, uint64_t t, std::array<uint16_t, 3>& out, uint64_t& tlast) {
        size_t n = 0; while (n < c.log.size() && c.log[n].first < t) ++n;
        if (n < 3) return false;
        out = {c.log[n - 3].second, c.log[n - 2].second, c.log[n - 1].second};
        tlast = c.log[n - 1].first;
        return true;
    };
    auto after = [](const Cap& c, uint64_t t, std::array<uint16_t, 3>& out, uint64_t& tlast) {
        size_t n = 0; while (n < c.log.size() && c.log[n].first <= t) ++n;
        if (n + 3 > c.log.size()) return false;
        out = {c.log[n].second, c.log[n + 1].second, c.log[n + 2].second};
        tlast = c.log[n + 2].first;
        return true;
    };
    uint64_t tl = 0;
    std::array<std::array<uint16_t, 3>, 4> ta{};
    bool okA = before(ca, marker(ca, ma1), ta[0], tl) && after(ca, marker(ca, ma1 + 1), ta[1], tl) && after(ca, marker(ca, ma2 + 1), ta[2], tl) && after(ca, marker(ca, ma3 + 1), ta[3], tl);
    // run B: (a), (b), (a), (c), (d), (e)
    std::array<std::array<uint16_t, 3>, 6> tb{};
    uint64_t tlast[6] = {};
    bool okB = before(cb, marker(cb, mb1), tb[0], tlast[0]) && after(cb, marker(cb, mb2 + 1), tb[1], tlast[1]) && before(cb, marker(cb, mb3), tb[2], tlast[2]) &&
               after(cb, marker(cb, mb3 + 1), tb[3], tlast[3]) && after(cb, marker(cb, mb5 + 1), tb[4], tlast[4]) && after(cb, marker(cb, mb6 + 1), tb[5], tlast[5]);
    size_t endA = ca.log.size();
    auto bitv = [](uint16_t v, int b) { return (v >> b) & 1; };
    // run A: in every triple the two SERDATR reads and the INTREQR read agree in the one request, and have the value that the CPU's writes make
    // (1, 0, 1, 0). A disagreement in a case that no event can change is a failure of the mirror, not a case to skip.
    bool a_mirror = okA, a_pat = okA;
    int seen11[2] = {0, 0};
    for (int i = 0; i < 4 && okA; ++i) {
        int b0 = bitv(ta[size_t(i)][0], 14), ir = bitv(ta[size_t(i)][1], 11), b2 = bitv(ta[size_t(i)][2], 14);
        ++seen11[ir];
        a_mirror &= b0 == ir && b2 == ir;
        int want = (i == 0 || i == 2) ? 1 : 0;
        a_pat &= ir == want && b0 == want && b2 == want;
    }
    // run B: SERDATR bit 13 and INTREQR bit 0 (AA p.27 prints bit 13 as a mirror of the request)
    const int wantb[6] = {1, 0, 1, 0, 1, 0};
    bool b_mirror = okB, b_pat = okB;
    for (int i = 0; i < 6 && okB; ++i) {
        int b0 = bitv(tb[size_t(i)][0], 13), ir = bitv(tb[size_t(i)][1], 0), b2 = bitv(tb[size_t(i)][2], 13);
        b_mirror &= b0 == ir && b2 == ir;
        b_pat &= ir == wantb[i] && b0 == wantb[i] && b2 == wantb[i];
    }
    // the cases (b) to (e) have to lie before the earliest instant at which the buffered word can move (the start of the stop cell of the
    // frame in the shift register), so that no move can change the request inside them
    uint64_t Kb = sb.frame.K ? sb.frame.K : 8, Tb = 64 * Kb;
    uint64_t t_b_ser = marker(cb, mb_ser + 1);
    int idle_b = tx_at(cb, t_b_ser + 1);
    auto fb = tx_frames(cb, t_b_ser + 1, cb.base + cb.txd.size() - 1, idle_b, Tb);
    bool stable = okB && fb.size() >= 2 && tlast[1] < fb[0].s + 9 * Tb && tlast[3] < fb[1].s + 9 * Tb && tlast[4] < fb[1].s + 9 * Tb && tlast[5] < fb[1].s + 9 * Tb;
    bool pre_a = okA && seen11[0] > 0 && seen11[1] > 0;
    if (okA) ck.note("P20 run A triples (SERDATR, INTREQR, SERDATR): " + hex4(ta[0][0]) + "," + hex4(ta[0][1]) + "," + hex4(ta[0][2]) + " | " + hex4(ta[1][0]) + "," + hex4(ta[1][1]) + "," + hex4(ta[1][2]) + " | " +
                       hex4(ta[2][0]) + "," + hex4(ta[2][1]) + "," + hex4(ta[2][2]) + " | " + hex4(ta[3][0]) + "," + hex4(ta[3][1]) + "," + hex4(ta[3][2]));
    if (okB) {
        std::string s2;
        for (int i = 0; i < 6; ++i) s2 += hex4(tb[size_t(i)][0]) + "," + hex4(tb[size_t(i)][1]) + "," + hex4(tb[size_t(i)][2]) + (i < 5 ? " | " : "");
        ck.note("P20 run B triples (a, b, a, c, d, e): " + s2);
        ck.note("P20 run B: frames " + dec((long long)fb.size()) + ", last store of triples b, c, d, e against the start of the stop cell of the frame in the shift register (cycles, negative = before): " +
                dec(fb.size() >= 2 ? (long long)tlast[1] - (long long)(fb[0].s + 9 * Tb) : 0LL) + " " + dec(fb.size() >= 2 ? (long long)tlast[3] - (long long)(fb[1].s + 9 * Tb) : 0LL) + " " +
                dec(fb.size() >= 2 ? (long long)tlast[4] - (long long)(fb[1].s + 9 * Tb) : 0LL) + " " + dec(fb.size() >= 2 ? (long long)tlast[5] - (long long)(fb[1].s + 9 * Tb) : 0LL));
    }
    // G-RXD from run A: bit 11 during the final idle and the 12T low
    bool grxd = true;
    {
        uint64_t tref = drv2.t_ref, R = 0;
        size_t ip0 = endA >= 201 ? endA - 201 : 0;
        R = max_gap(ca, ip0, endA - 1);
        size_t n_hi = 0, n_lo = 0;
        for (size_t i = ip0; i < endA; ++i) {
            uint64_t ti = ca.log[i].first; int b = bitv(ca.log[i].second, 11);
            if (ti >= tref + t_low + 2 * R && ti < tref + t_low + 12 * T) { ++n_lo; if (b != 0) grxd = false; }
            if (ti < tref + t_low - 2 * R) { ++n_hi; if (b != 1) grxd = false; }
        }
        grxd &= n_lo > 10 && n_hi > 10;
    }
    bool gbrk = (logv(ca, pa.i_adkconr) & 0x0800) == 0 && (logv(cb, pb.i_adkconr) & 0x0800) == 0;
    bool gk = sa.frame.ok && 64 * sa.frame.K == T;   // the bit time that the RxD drive uses is 64 colour clocks of the measured machine
    print_std(ck, "P20", agg, 2);
    ck.expect(gk, "P20-PRE-K: the bit time of the RxD schedule, " + dec((long long)T) + " system clocks, is 64 colour clocks of the colour clock measured from the refresh pattern in run A (SPECIFICATION.md 2.2)");
    ck.expect(gbrk, "P20-G-BRK: after S0 ADKCONR bit 11 (UARTBRK) reads 0 in both runs (AA p.10 '11 UARTBRK')");
    ck.expect(grxd, "P20-G-RXD: SERDATR bit 11 reads 0 during the 12T low the test drives on RxD (AA p.27 '11 RXD')");
    ck.expect(pre_a, "P20-PRE-TRIPLES: run A takes its four triples and INTREQR bit 11 takes both values across them (AA p.25 '11 RBF'; AA p.24 'Both a set and clear operation are required to load arbitary data into this register')");
    ck.expect(stable, "P20-PRE-STABLE: run B takes its six triples, and the triples of the cases (b), (c), (d) and (e) end before the start of the stop cell of the frame that is in the shift register, the earliest instant at which the buffered word can move (AA p.27 'moved into a serial shift register for output transmission, whenever it is empty')");
    bool preA = agg.fixture && agg.s0 && agg.gc && gbrk && gk && grxd && pre_a;
    bool preB = agg.fixture && agg.s0 && agg.gc && gbrk && gk && grxd && pre_a && stable;
    const std::string un = "fixture, G-S0, G-C, G-BRK, the colour-clock length, G-RXD, the triples or P20-PRE-STABLE";
    graded(ck, preA, a_mirror && a_pat, "P20-RBF: in each of the four triples of run A (the frame received, cleared by software, set by software, cleared by software) SERDATR bit 14 equals INTREQR bit 11 at both reads, and the triples read (1, 1, 1), (0, 0, 0), (1, 1, 1) and (0, 0, 0) (AA p.27 '14 RBF Serial port Receive Buffer Full(mirror)')", un);
    graded(ck, preB, b_mirror && b_pat, "P20-TBE: in each of the six triples of run B SERDATR bit 13 equals INTREQR bit 0 at both reads, and the triples read (1, 1, 1) after a move with the buffer empty, (0, 0, 0) with a word waiting and the request cleared, (1, 1, 1) after the next move, (0, 0, 0) after a software clear with the buffer empty, (1, 1, 1) after a software set with a word waiting and (0, 0, 0) after a software clear with a word waiting (AA p.27 '13 TBE Serial port Transmit Buffer Empty(mirror)')", un);
    graded(ck, preB, a_mirror && a_pat && b_mirror && b_pat, "P20 sol:AA-serdatr.c8: SERDATR bit 14 reads the same as INTREQR bit 11 (RBF) and bit 13 the same as INTREQR bit 0 (TBE) in every stable case: after a frame is received, after a move with the buffer empty and with a word waiting, and after a software set and a software clear of the request with the buffer empty and with a word waiting (AA p.27 'Several interrupt request bits are also read at this address, along with the data')", un);
}

} // namespace s5
