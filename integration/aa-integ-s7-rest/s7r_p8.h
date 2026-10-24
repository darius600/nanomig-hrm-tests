// s7r_p8.h -- run HV (the memory-cycle conversion) and P8 (HHPOSW / HHPOSR),
// SPECIFICATION.md sections 2 and 5.
#pragma once

#include "s7r_hv.h"

namespace s7r {

inline int mod_(int a, int m) { return ((a % m) + m) % m; }

// The program binary of a run: `normal`, or `alt` when the selector is the active mutation.
inline std::string pick(const std::string& normal, const char* sel, const char* alt) {
    return mut(sel) ? alt : normal;
}

struct HvResult { Cap cap; Conv conv; };

// Run HV and its anchor (section 2).
inline HvResult run_hv(Checks& ck) {
    HvResult r;
    r.cap = runbin(pick(pick("prog_hv.bin", "HV-ANCHOR", "prog_hv_c.bin"), "FIXTURE-HV", "prog_hv_nofix.bin"), 5'000'000);
    r.conv = conversion(r.cap);
    bool fx = fixture_ok(r.cap);
    ck.expect(fx, "FIXTURE-HV: the run loaded, booted and stored the constant $BEEF at its fixture address before any sample is read");
    graded(ck, fx, r.conv.ok,
           "HV-ANCHOR: at least 16 pairs of consecutive VHPOSR samples with the same V field exist, the H field (bits 7-0) advances by at least 1 in each, and no V value has more than one same-V pair whose H field decreased (AA p.29: 'RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)', H8-H1 in bits 7-0)",
           "the run is not usable");
    ck.note("HV: " + dec(r.conv.pairs) + " pairs in " + dec(r.conv.segs) + " segments (S_HV), " + dec(r.conv.wraps) + " same-V pair(s) with a decreasing H field left out, at most " + dec(r.conv.maxdec) +
            " for one V value; J_HV " + num3(r.conv.jmc) + " memory cycles; conversion " + num3(r.conv.K) + " clk_sys cycles per memory cycle (" + cap_core() + ")");
    return r;
}

struct HRun {
    std::string nm;
    int N = 0, X = 0;
    Cap cap;
    Samples s;
    bool stores_ok = false;       // PR-8c: end marker present and 64 sample stores
    double max_iv = 0, min_iv = 0;   // sample intervals in memory cycles
    std::string bin;              // the program the run loads
    uint32_t wr_addr = 0;         // byte address of the last word of the instruction that writes HHPOSW
    uint64_t wr_t = 0;            // port time of the first read of that word (0: not found)
    double lat = 0;               // from that fetch to the first sample store, in memory cycles (0: not measured)
};

// The byte address of the last word of the first instruction `move.w #<imm>,<d16>(a0)` (opcode $317C)
// with d16 = HHPOSW ($01D8) in a program binary, which the harness loads at PROGRAM_ORG ($080400); 0 when none.
inline uint32_t hhposw_instr_end(const std::string& bin) {
    std::ifstream f(g_dir + "/" + bin, std::ios::binary);
    std::vector<uint8_t> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    auto w = [&](size_t i) { return uint16_t((b[i] << 8) | b[i + 1]); };
    for (size_t i = 0; i + 5 < b.size(); i += 2)
        if (w(i) == 0x317C && w(i + 4) == 0x01D8) return 0x080400 + uint32_t(i) + 4;
    return 0;
}

// The port time of the first read at a byte address; 0 when none.
inline uint64_t read_time(const Cap& c, uint32_t addr) {
    for (auto& x : c.acc) if (!x.w && x.a == addr) return x.t;
    return 0;
}

inline void measure_h(HRun& h, double K) {
    h.s = samples(h.cap);
    h.stores_ok = h.cap.done && h.s.full;
    h.wr_addr = hhposw_instr_end(h.bin);
    h.wr_t = h.wr_addr ? read_time(h.cap, h.wr_addr) : 0;
    h.lat = (h.wr_t && h.s.full && h.s.t[0] > h.wr_t) ? double(h.s.t[0] - h.wr_t) / K : 0;
    h.max_iv = 0; h.min_iv = 1e18;
    if (!h.s.full) { h.min_iv = 0; return; }
    for (size_t i = 0; i + 1 < h.s.t.size(); ++i) {
        double iv = double(h.s.t[i + 1] - h.s.t[i]) / K;
        h.max_iv = std::max(h.max_iv, iv); h.min_iv = std::min(h.min_iv, iv);
    }
}

// Words of the program image that differ between two binaries: the byte addresses (logical).
inline std::set<uint32_t> bin_diff(const std::string& a, const std::string& b) {
    std::set<uint32_t> d;
    auto rd = [](const std::string& p) {
        std::ifstream f(p, std::ios::binary);
        return std::vector<uint8_t>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    };
    auto x = rd(g_dir + "/" + a), y = rd(g_dir + "/" + b);
    size_t n = std::max(x.size(), y.size());
    for (size_t i = 0; i + 1 < n + 1; i += 2) {
        uint16_t wx = i + 1 < x.size() ? uint16_t((x[i] << 8) | x[i + 1]) : 0xFFFF;
        uint16_t wy = i + 1 < y.size() ? uint16_t((y[i] << 8) | y[i + 1]) : 0xFFFF;
        if (wx != wy) d.insert(0x080400 + uint32_t(i));
    }
    return d;
}

// Two runs of one program layout agree in time, address and direction for every port
// access; their data differ only at the program words that differ between the two
// binaries and at the sample stores.
inline bool traces_agree(const Cap& a, const Cap& b, const std::set<uint32_t>& prog_diff, uint32_t buf_lo, uint32_t buf_hi) {
    if (a.acc.size() != b.acc.size()) return false;
    for (size_t i = 0; i < a.acc.size(); ++i) {
        const Acc& x = a.acc[i]; const Acc& y = b.acc[i];
        if (x.t != y.t || x.a != y.a || x.w != y.w) return false;
        if (x.d != y.d) {
            bool allowed = (!x.w && prog_diff.count(x.a)) || (x.w && x.a >= buf_lo && x.a < buf_hi);
            if (!allowed) return false;
        }
    }
    return true;
}

inline void run_p8(Checks& ck, const HvResult& hv) {
    const bool kok = hv.conv.ok;
    const double K = kok ? hv.conv.K : 1.0;
    HRun A, B, C, D, W;
    A.nm = "HA"; B.nm = "HB"; C.nm = "HC"; D.nm = "HD"; W.nm = "HW";
    A.N = 0x71; A.X = 0x10; B.N = 0x71; B.X = 0x40; C.N = 0x51; C.X = 0x10; D.N = 0x71; D.X = 0x10; W.N = 0x71; W.X = 0x69;
    const std::string binA = pick(pick(pick("prog_ha.bin", "PR-8b", "prog_ha_slow.bin"), "PR-8c", "prog_ha_noend.bin"), "PR-8d", "prog_ha_wait.bin");
    const std::string binB = pick(pick(pick("prog_hb.bin", "P8-E1", "prog_ha.bin"), "P8-AGG", "prog_ha.bin"), "PR-8a", "prog_hb_nop.bin");
    const std::string binC = pick(pick(pick("prog_hc.bin", "P8-E3", "prog_hc_x.bin"), "P8-E4", "prog_hc_v.bin"), "FIXTURE-P8", "prog_hc_nofix.bin");
    const std::string binW = pick("prog_hw.bin", "P8-E2", "prog_hw_n.bin");
    const std::string binD = pick("prog_ha.bin", "P8-HD", "prog_hb.bin");
    A.bin = binA; B.bin = binB; C.bin = binC; D.bin = binD; W.bin = binW;
    A.cap = runbin(binA); B.cap = runbin(binB); C.cap = runbin(binC); D.cap = runbin(binD); W.cap = runbin(binW);
    for (HRun* h : {&A, &B, &C, &D, &W}) measure_h(*h, K);

    bool fx = fixture_ok(A.cap) && fixture_ok(B.cap) && fixture_ok(C.cap) && fixture_ok(D.cap) && fixture_ok(W.cap);
    ck.expect(fx, "FIXTURE-P8: runs HA, HB, HC, HD and HW loaded, booted and stored the constant $BEEF at their fixture address before any sample is read");

    // PR-8a, PR-8b, PR-8c
    std::set<uint32_t> pd = bin_diff("prog_ha.bin", "prog_hb.bin");
    bool pr8a = traces_agree(A.cap, B.cap, pd, kBuf, kBuf + 128);
    ck.expect(pr8a, "PR-8a: HA and HB agree in time, address and direction for every port access; their data differ only in the fetch of the immediate operand X and in the sample stores, so the counter is read at the same moments in both runs (R3)");
    bool pr8b = kok;
    for (HRun* h : {&A, &B, &C, &W})
        pr8b = pr8b && h->s.full && h->max_iv < double(h->N + 1) / 2.0;
    ck.expect(pr8b, "PR-8b: the largest interval between two sample stores is below (N+1)/2 memory cycles in HA, HB, HC and HW, so each count difference is unambiguous");
    bool pr8c = true;
    for (HRun* h : {&A, &B, &C, &W}) pr8c = pr8c && h->stores_ok;
    ck.expect(pr8c, "PR-8c: HA, HB, HC and HW end with the end marker and 64 sample stores are seen");
    bool pr8d = kok;
    for (HRun* h : {&A, &B, &C, &W}) pr8d = pr8d && h->lat > 0 && h->lat < double(h->N + 1) / 2.0;
    pr8d = pr8d && W.lat > double(W.N - W.X);       // HW: the counter passes N before the first read
    ck.expect(pr8d, "PR-8d: in HA, HB, HC and HW the port shows the fetch of the last word of the instruction that writes HHPOSW, the first sample store follows it, and the latency from that fetch to the first sample store is below (N+1)/2 memory cycles; in HW it is also more than N - X = 8 memory cycles");
    if (kok)
        ck.note("P8: latency from the fetch of the last word of the HHPOSW instruction to the first sample store, in memory cycles: HA " + num3(A.lat) + " HB " + num3(B.lat) + " HC " + num3(C.lat) + " HW " + num3(W.lat) + " (" + cap_core() + ")");
    if (kok)
        ck.note("P8: largest sample interval in memory cycles HA " + num3(A.max_iv) + " HB " + num3(B.max_iv) + " HC " + num3(C.max_iv) + " HW " + num3(W.max_iv) +
                ", smallest HA " + num3(A.min_iv) + " (" + cap_core() + ")");
    {
        std::string s = "P8: first samples (bits 7-0)";
        for (HRun* h : {&A, &B, &C, &D, &W}) {
            s += " " + h->nm + ":";
            for (int i = 0; i < 6 && i < int(h->s.r.size()); ++i) s += " " + hex(h->s.r[size_t(i)] & 0xFF, 2);
        }
        ck.note(s);
        std::string hi = "P8: bits 15-8 of the samples (not graded): HA";
        for (int i = 0; i < 6; ++i) hi += " " + hex(A.s.r[size_t(i)] >> 8, 2);
        ck.note(hi);
    }

    const bool pre = kok && pr8b && pr8c;
    const bool pre3 = pre && pr8d;
    const std::string unmet = "PR-8b / PR-8c / the conversion";

    // E8.1: the write sets the counter (R3 pair HA/HB)
    bool e1 = pre && A.s.full && B.s.full;
    if (e1) for (size_t i = 0; i < 64; ++i)
        if (mod_(int(B.s.r[i] & 0xFF) - int(A.s.r[i] & 0xFF), A.N + 1) != mod_(0x40 - 0x10, A.N + 1)) e1 = false;
    graded(ck, pre && pr8a, e1,
           "P8-E1: reading HHPOSR after HHPOSW = $0040 gives, sample for sample, ($0040 - $0010) mod 114 more than after HHPOSW = $0010 (AA p.24: HHPOSW 'DUAL mode hires H beam counter write', the counter counts up to HTOTAL and starts again)",
           "PR-8a / PR-8b / PR-8c / the conversion");

    // E8.2: HTOTAL is the highest count, and the count after it is 0
    bool e2 = pre3 && W.s.full;
    if (e2) {
        for (HRun* h : {&A, &B, &C, &W}) for (size_t i = 0; i < h->s.r.size(); ++i) if (int(h->s.r[i] & 0xFF) > h->N) e2 = false;     // (i)
        const int r0 = int(W.s.r[0] & 0xFF);
        if (!(r0 < W.X && double(mod_(r0 - W.X, W.N + 1)) <= W.lat + 1.0)) e2 = false;                                                // (ii) the counter passed N and started again
        for (size_t i = 0; i + 1 < 64; ++i)                                                                                           // (iii) the counter moves
            if (mod_(int(W.s.r[i + 1] & 0xFF) - int(W.s.r[i] & 0xFF), W.N + 1) < 1) e2 = false;
        ck.note("P8-E2: HW writes X = $" + hex(unsigned(W.X), 2) + " (N - 8), first sample $" + hex(W.s.r[0] & 0xFF, 2) + ", measured latency " + num3(W.lat) + " memory cycles (" + cap_core() + ")");
    }
    graded(ck, pre3, e2,
           "P8-E2: the counter counts up to HTOTAL and then starts again at 0: every sample of HA, HB, HC and HW (bits 7-0) is at most the run's HTOTAL, N = $71 for HA, HB and HW and $51 for HC; HW, which writes X = N - 8 = $69, has passed N and restarted before its first read (first sample below X, modulo N+1 within the latency plus one) and its count moves between every two samples (AA p.24: HTOTAL 'Highest color clock count in horiz line', 'Horiz line has this many + 1 280ns increments', 'Active if VARBEAMEN=1 or DUAL=1')",
           "PR-8b / PR-8c / PR-8d / the conversion");

    // E8.3: the write is the starting point
    bool e3 = pre3;
    if (pre3)
        for (HRun* h : {&A, &B, &C})
            if (!(double(mod_(int(h->s.r[0] & 0xFF) - h->X, h->N + 1)) <= h->lat + 1.0)) e3 = false;
    graded(ck, pre3, e3,
           "P8-E3: in each of HA, HB and HC the first sample follows the written value X by at most the measured latency of PR-8d plus one count, counted modulo N+1 (AA p.24: HHPOSW writes the DUAL-mode beam counter)",
           "PR-8b / PR-8c / PR-8d / the conversion");

    // E8.4: it counts. The rate (one count per memory cycle) is printed and not graded.
    bool e4 = true;
    std::string e4m;
    for (HRun* h : {&A, &B, &C}) {
        if (!h->s.full) { e4 = false; continue; }
        int sum = 0; bool pos = true;
        for (size_t i = 0; i + 1 < 64; ++i) {
            int d = mod_(int(h->s.r[i + 1] & 0xFF) - int(h->s.r[i] & 0xFF), h->N + 1);
            if (d < 1) pos = false;
            sum += d;
        }
        double elapsed = double(h->s.t[63] - h->s.t[0]) / K;
        e4m += " " + h->nm + ": sum d " + dec(sum) + " counts, elapsed " + num3(elapsed) + " memory cycles";
        if (!pos) e4 = false;
    }
    ck.note("P8-E4 rate (not graded):" + e4m + " (" + cap_core() + ")");
    graded(ck, pre, e4,
           "P8-E4: in each of HA, HB and HC the count advances by at least 1 between consecutive samples (AA p.24 HTOTAL 'this many + 1 280ns increments'; the rate is measured and not graded)",
           unmet);

    // HD against HA
    bool hd = D.s.full && A.s.full;
    if (hd) for (size_t i = 0; i < 64; ++i) if ((D.s.r[i] & 0xFF) != (A.s.r[i] & 0xFF)) hd = false;
    graded(ck, pre, hd,
           "P8-HD: HD, which repeats HA with the same HHPOSW value, reads the same counter value as HA in every sample (a predicted difference of 0)",
           unmet);

    const std::string ids =
        "opus:AA-hhposw.c1 opus:AA-hhposw.c2 opus:AA-hhposw.c5 opus:AA-hhposr.c1 opus:AA-hhposr.c2 opus:AA-hhposr.c5 "
        "sol:AA-hhposw.c1 sol:AA-hhposw.c2 sol:AA-hhposw.c5 sol:AA-hhposr.c1 sol:AA-hhposr.c2 sol:AA-hhposr.c5";
    graded(ck, pre3 && pr8a, e1 && e2 && e3 && e4,
           "P8-AGG: HHPOSW writes and HHPOSR reads the DUAL-mode horizontal beam counter, which counts up to HTOTAL and then starts again (AA p.10 rows 1D8 and 1DA, p.24) " + ids,
           "PR-8a / PR-8b / PR-8c / PR-8d / the conversion");
}

} // namespace s7r
