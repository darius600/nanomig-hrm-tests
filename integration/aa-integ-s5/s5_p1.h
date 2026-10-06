// s5_p1.h -- P1: AUDxLEN sets the number of words in each DMA block (AA p.11).
#pragma once

#include "s5_common.h"

namespace s5 {

// The DMACON value that starts the channels in `mask` (2.7).
inline uint16_t start_val(unsigned mask) {
    if (mut("GD")) return uint16_t(0x8000 | mask);   // DMAEN left out
    if (mut("GF")) return 0x8200;                    // AUDxEN left out
    return uint16_t(0x8200 | mask);
}

// The fetches of a block structure with base address `base` and length L (words).
struct Blocks {
    size_t n = 0, returns = 0, distinct = 0, outside = 0, wrap_bad = 0, step_bad = 0;
    bool first_at_base = false;
};

inline Blocks block_check(const std::vector<Fetch>& f, uint32_t base, unsigned L, size_t from = 0) {
    Blocks b;
    std::set<uint32_t> d;
    for (size_t i = from; i < f.size(); ++i) {
        d.insert(f[i].addr);
        if (f[i].addr == base) ++b.returns;
        if (f[i].addr < base || f[i].addr >= base + 2 * L) ++b.outside;
    }
    b.n = f.size() - std::min(from, f.size());
    b.distinct = d.size();
    b.first_at_base = from < f.size() && f[from].addr == base;
    for (size_t i = from; i + 1 < f.size(); ++i) {
        uint32_t last = base + 2 * (L - 1);
        uint32_t want = f[i].addr == last ? base : f[i].addr + 2;
        if (f[i].addr == last && f[i + 1].addr != base) ++b.wrap_bad;
        if (f[i + 1].addr != want) ++b.step_bad;
    }
    return b;
}

struct P1Case {
    int x; unsigned L; unsigned wrote;     // wrote: the length the program wrote
    bool all4 = false;
    int m_start = 0, m_stop = 0, i_dmaconr = -1;
    std::vector<Fetch> f;
    Blocks b;
    bool dmaconr_ok = false;
    uint64_t t_start = 0, t_stop = 0;
};

struct P1Run { Cap cap; Prefix pre; std::vector<P1Case> cases; };

inline uint32_t wait_iters(unsigned ccks) { return std::min<uint32_t>(65000, ccks / 4 + 1); }   // a DBRA iteration takes at least four colour clocks

inline void fill_buf(RunSpec& rs, int x) {
    for (int i = 0; i < 16; ++i) rs.pokes.push_back({kBuf[x] + uint32_t(2 * i), uint16_t(0x0101 * (x * 16 + i + 1) ^ 0x1357)});
}

inline P1Run p1_single(int x, unsigned L) {
    P1Run r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    fill_buf(rs, x);
    P1Case c; c.x = x; c.L = L;
    uint32_t lc = kBuf[x] + (mut("P1-ANCHOR") ? 4 : 0);
    c.wrote = (mut("P1") && L == 7) ? 5 : L;
    s.w32(audlc(x), lc);
    if (mut("P1-ADDR") && x == 0 && L == 7) { s.w16(audlen(0), 3); s.w16(0x0B4, 7); }
    else s.w16(audlen(x), uint16_t(c.wrote));
    s.w16(audvol(x), 0);
    s.w16(audper(x), 200);
    c.m_start = s.w16(DMACON, start_val(1u << x));
    c.i_dmaconr = r.pre.logn++; s.rd(DMACONR);
    s.dly(wait_iters(2000 * L));
    c.m_stop = s.w16(DMACON, uint16_t(1u << x));
    s.dly(200);
    r.cases.push_back(c);
    s.end();
    rs.cycles = 4'000'000 + 8ull * 3000 * L * 2;
    r.cap = run(rs);
    return r;
}

inline P1Run p1_all() {
    P1Run r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    const unsigned Ls[4] = {3, 5, 7, 11};
    const unsigned Ll[4] = {1, 2, 1, 2};
    for (int x = 0; x < 4; ++x) {
        fill_buf(rs, x);
        const unsigned Lx = mut("P1-LIVE") ? Ll[x] : Ls[x];
        P1Case c; c.x = x; c.L = Lx; c.wrote = (mut("P1") && Lx == 7) ? 5 : Lx; c.all4 = true;
        s.w32(audlc(x), kBuf[x] + (mut("P1-ANCHOR") ? 4 : 0));
        s.w16(audlen(x), uint16_t(c.wrote));
        s.w16(audvol(x), 0);
        s.w16(audper(x), 200);
        r.cases.push_back(c);
    }
    int ms = s.w16(DMACON, start_val(0xF));
    int idx = r.pre.logn++; s.rd(DMACONR);
    s.dly(wait_iters(2000 * 11));
    int me = s.w16(DMACON, 0x000F);
    s.dly(200);
    s.end();
    for (auto& c : r.cases) { c.m_start = ms; c.m_stop = me; c.i_dmaconr = idx; }
    rs.cycles = 4'000'000 + 8ull * 2000 * 11 * 2;
    r.cap = run(rs);
    return r;
}

inline void p1_eval(P1Run& r) {
    for (auto& c : r.cases) {
        c.t_start = marker(r.cap, c.m_start);
        c.t_stop = marker(r.cap, c.m_stop + 1);
        uint32_t lo = kBuf[c.x] - 0x40, hi = kBuf[c.x] + 0x1000;
        c.f = fetches(r.cap, lo, hi, c.t_start, c.t_stop);
        c.b = block_check(c.f, kBuf[c.x], c.L);
        uint16_t d = logv(r.cap, c.i_dmaconr);
        c.dmaconr_ok = (d & 0x200) && (d & (1u << c.x));
    }
}

inline void run_p1(Checks& ck) {
    std::vector<P1Run> runs;
    const bool live = mut("P1-LIVE");   // a live input through the same comparators: the lengths whose first block is whole
    for (int x = 0; x < nch(); ++x) {
        std::vector<unsigned> Ls = live ? std::vector<unsigned>{1u, 2u} : std::vector<unsigned>{1u, 2u, 3u, 7u, 16u};
        for (unsigned L : Ls) runs.push_back(p1_single(x, L));
    }
    runs.push_back(p1_all());
    PreAgg agg;
    bool anchor = true, gd = true, gf = true;
    bool a_ok = true, b_ok = true, c_ok = true, step_ok = true;
    size_t ncase = 0;
    for (auto& r : runs) {
        p1_eval(r);
        std::vector<Fetch> all;
        for (auto& c : r.cases) all.insert(all.end(), c.f.begin(), c.f.end());
        Std st = std_pre(r.cap, r.pre, all);
        agg.add(st);
        note_gc(ck, "P1 run " + std::string(r.cases[0].all4 ? "all4" : ("ch" + dec(r.cases[0].x))), st);
        for (auto& c : r.cases) {
            ++ncase;
            anchor &= c.b.first_at_base;
            gd &= c.dmaconr_ok;
            gf &= c.b.returns >= 5;
            a_ok &= c.b.distinct == c.L;
            b_ok &= c.b.wrap_bad == 0;
            c_ok &= c.b.outside == 0;
            step_ok &= c.b.step_bad == 0;
            std::string addrs;
            for (size_t i = 0; i < c.f.size() && i < 24; ++i) addrs += dec((c.f[i].addr - kBuf[c.x]) / 2) + " ";
            ck.note("P1 ch" + dec(c.x) + (c.all4 ? " (all four)" : "") + " L=" + dec(c.L) + " wrote " + dec(c.wrote) + ": fetches=" + dec((long long)c.b.n) +
                    " returns=" + dec((long long)c.b.returns) + " distinct=" + dec((long long)c.b.distinct) + " outside=" + dec((long long)c.b.outside) +
                    " wrap_bad=" + dec((long long)c.b.wrap_bad) + " step_bad=" + dec((long long)c.b.step_bad) + " first_at_base=" + dec(c.b.first_at_base) +
                    " DMACONR=" + hex4(logv(r.cap, c.i_dmaconr)) + " offsets: " + addrs);
        }
    }
    print_std(ck, "P1", agg, runs.size());
    ck.expect(gd, "P1-G-D: after each start DMACONR bit 9 (DMAEN) and the channel's bit read 1 in all " + dec((long long)ncase) + " cases (AA p.22 '09 DMAEN Enable all DMA below')");
    ck.expect(gf, "P1-G-F: each channel's fetches occur: at least five fetches at the location, so at least four complete blocks, in each of the " + dec((long long)ncase) + " cases (AA p.22 '00 AUD0EN Audio channel 0 DMA enable')");
    ck.expect(anchor, "P1-PRE-ANCHOR: the first fetch after each start is at the location written to AUDxLCH/LCL (AA p.11 'the 20 bit starting address (location)')");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && anchor;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F or the first-fetch anchor";
    graded(ck, pre, a_ok, "P1-A: in every case exactly L distinct addresses are fetched (AA p.11 AUDxLEN 'length (number of words)')", un);
    graded(ck, pre, b_ok, "P1-B: in every case a fetch at the last word of the block (location + 2(L-1)) is followed by a fetch at the location (AA p.6 'Used to automaticall[y] restart pointers ... the Audio sample counter (whenever the audio length count is finished)')", un);
    graded(ck, pre, c_ok, "P1-C: in every case no fetch lies outside [location, location + 2L) (AA p.11 AUDxLEN)", un);
    graded(ck, pre, a_ok && b_ok && c_ok && step_ok,
           "P1 sol:AA-audxlen.c1 sol:AA-audxlen.c4: AUDxLEN at 0A4 (0B4, 0C4, 0D4) is the number of words of each DMA block: for every channel and every L the fetches form BUFx, BUFx+2, .. BUFx+2(L-1), BUFx, ... (AA p.11 'This register contains the length (number of words) of Audio Channel x DMA data')", un);
}

} // namespace s5
