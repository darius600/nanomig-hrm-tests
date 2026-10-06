// s3_props_s89.h -- propositions S8 and S9 (sprite vertical start and end bits),
// graded by the line-offset measure LOM of SPECIFICATION.md 2.11

#pragma once

#include "s3_props_s567.h"

namespace s3 {

// LOM: delta = round(d / P); valid when |d - delta * P| <= P / 8
inline bool lom_delta(int64_t d, uint64_t P, long* out) {
    long k = std::lround(double(d) / double(P));
    if (out) *out = k;
    return iabs(d - int64_t(k) * int64_t(P)) <= P / 8;
}

struct LomData {
    std::shared_ptr<RunData> r;
    bool ok = false;                 // every read exists
    uint64_t P = 0;
    std::array<int64_t, 8> tfirst{}, tend{};
    int start = 0, stop = 0;
};

// the eight sprites at columns X_x with h = stop - start rows 0x8000 each
inline RunSpec lom_spec(const std::string& pfx, int start, int stop, bool lf, bool spren, bool wide = false) {
    Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = spren ? 0x83A0 : 0x8380; o.lf = lf;
    if (wide) { o.lf = true; o.htotal = 0x0105; o.vtotal = 0x0138; o.beamcon0 = 0x08A0; }       // 262 colour clocks per line
    RunSpec s = base_spec(o, pfx);
    int h = stop - start;
    for (int x = 0; x < 8; ++x) {
        std::vector<SprRow> rows(h, SprRow{0x8000, 0x0000});
        s.mem.push_back({L_x(x), spr_list(start, stop, sh_of_lores(kXs[x]), false, rows)});
    }
    s.vb = spr_ptrs(own_lists());
    for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
    EventSpec fin; fin.nvb = 4; s.ev.push_back(fin);
    if (lf) s.budget = 60'000'000;
    return s;
}

inline std::map<std::string, LomData>& lom_cache() { static std::map<std::string, LomData> c; return c; }

// runs (or fetches) one pair member; folds the common preconditions into pre
inline LomData lom_run(const std::string& pfx, int start, int stop, bool lf, bool spren, Pre& pre, bool& pc_spr, bool& pc_static, bool wide = false) {
    char key[96];
    std::snprintf(key, sizeof key, "%s|%d|%d|%d|%d|%d", g_mutate ? pfx.c_str() : "", start, stop, lf ? 1 : 0, spren ? 1 : 0, wide ? 1 : 0);
    auto it = lom_cache().find(key);
    LomData d;
    if (it != lom_cache().end()) d = it->second;
    else {
        d.start = start; d.stop = stop;
        d.r = run(lom_spec(pfx, start, stop, lf, spren, wide));
        Frame f = get_frame(*d.r, 2);
        d.P = chip_line_period(*d.r);
        d.ok = f.ok && d.P > 0;
        int h = stop - start;
        for (int x = 0; x < 8 && d.ok; ++x) {
            auto a = first_read_at(*d.r, L_x(x) + 4, f.t0, f.t1);
            auto b = first_read_at(*d.r, L_x(x) + 4 + 4 * uint32_t(h), f.t0, f.t1);
            if (!a || !b) { d.ok = false; break; }
            d.tfirst[x] = int64_t(*a) - int64_t(f.t0);
            d.tend[x] = int64_t(*b) - int64_t(f.t0);
        }
        lom_cache()[key] = d;
    }
    pre.check(*d.r, 1, 2, lf && !wide, true, false);
    Frame f = get_frame(*d.r, 2);
    if (!(f.ok && all_lists_in_order(*d.r, f, 2 + 2 * (stop - start) + 2))) {
        pc_spr = false;
        if (std::getenv("NMH_DBG")) {
            std::cout << "DBG list order fails: start 0x" << std::hex << start << " stop 0x" << stop << std::dec << " lf " << lf << std::endl;
            for (int x = 0; x < 8 && f.ok; ++x) {
                auto v = list_seq(*d.r, f, L_x(x), 0x100);
                std::cout << "  list " << x << ": " << v.size() << " words, first 0x" << std::hex << (v.empty() ? 0 : v[0]) << std::dec << std::endl;
                if (x == 0) {
                    for (const auto& p : list_reads(*d.r, f, L_x(0), 0x100))
                        std::cout << "    +0x" << std::hex << (p.second - L_x(0)) << " at line " << std::dec << double(int64_t(p.first) - int64_t(f.t0)) / double(d.P) << std::endl;
                }
            }
        }
    }
    if ((start & 1) != (stop & 1)) pc_static = false;
    return d;
}

struct Pair { int k; int s0, e0, s1, e1; bool lf; };

inline std::vector<Pair> joint_pairs() {
    return {{0, 0x40, 0x48, 0x41, 0x49, false}, {1, 0x40, 0x48, 0x42, 0x4A, false}, {2, 0x40, 0x48, 0x44, 0x4C, false},
            {3, 0x40, 0x50, 0x48, 0x58, false}, {4, 0x40, 0x48, 0x50, 0x58, false}, {5, 0x40, 0x48, 0x60, 0x68, false},
            {6, 0x30, 0x38, 0x70, 0x78, false}, {7, 0x40, 0x48, 0xC0, 0xC8, false}, {8, 0x28, 0x30, 0x128, 0x130, false},
            {9, 0x40, 0x48, 0x240, 0x248, true}};
}

// both runs of a pair, then the LOM deltas of T_first and T_end for every sprite
struct PairResult { bool valid = true; long dfirst[8] = {}, dend[8] = {}; };

inline PairResult compare_pair(const LomData& a, const LomData& b) {
    PairResult pr;
    if (!a.ok || !b.ok) { pr.valid = false; return pr; }
    for (int x = 0; x < 8; ++x) {
        if (!lom_delta(b.tfirst[x] - a.tfirst[x], a.P, &pr.dfirst[x])) pr.valid = false;
        if (!lom_delta(b.tend[x] - a.tend[x], a.P, &pr.dend[x])) pr.valid = false;
    }
    return pr;
}

inline bool all_eq(const long* v, long want) { for (int x = 0; x < 8; ++x) if (v[x] != want) return false; return true; }

// ---- S8 -----------------------------------------------------------------------
inline void prop_S8() {
    if (!want("S8")) return;
    Pre pre("S8");
    bool pc_spr = true, pc_static = true, pc_lom = true;
    bool ok = true;
    int only_k = -1;
    if (g_mutate) only_k = mutating("S8-PC-FRAME") ? 9 : 2;
    const bool spren = !mutating("S8-PC-SPR");
    for (const auto& p : joint_pairs()) {
        if (only_k >= 0 && p.k != only_k) continue;
        int s1 = p.s1, e1 = p.e1;
        if (mutating("S8") && p.k == 2) { s1 = 0x48; e1 = 0x50; }                // shifted by 2^(k+1)
        int s0 = p.s0;
        if (mutating("S8-PC-STATIC") && p.k == 2) s0 = 0x41;                      // start odd, stop even
        LomData a = lom_run("S8", s0, p.e0, p.lf, spren, pre, pc_spr, pc_static);
        LomData b = lom_run("S8", s1, e1, p.lf, true, pre, pc_spr, pc_static, mutating("S8-PC-LOM"));
        PairResult pr = compare_pair(a, b);
        if (!pr.valid) pc_lom = false;
        long want = 1L << p.k;
        bool good = pr.valid && all_eq(pr.dfirst, want);
        g.note("S8: k = " + dec(p.k) + ": start 0x" + hexs(s0, 3) + " -> 0x" + hexs(s1, 3) + ": delta(T_first) = " + dec(pr.dfirst[0]) + " lines for sprite 0 (page: " + dec(want) + ")");
        ok = ok && good;
        if (g_mutate) break;
    }
    // start-only runs for k = 1..3
    for (int k = 1; k <= 3 && !g_mutate; ++k) {
        LomData a = lom_run("S8", 0x40, 0x50, false, true, pre, pc_spr, pc_static);
        LomData b = lom_run("S8", 0x40 + (1 << k), 0x50, false, true, pre, pc_spr, pc_static);
        PairResult pr = compare_pair(a, b);
        if (!pr.valid) pc_lom = false;
        g.note("S8: start-only k = " + dec(k) + ": delta(T_first) = " + dec(pr.dfirst[0]) + " lines for sprite 0 (page: " + dec(1 << k) + ")");
        ok = ok && pr.valid && all_eq(pr.dfirst, 1L << k);
    }
    pre.print("the S8 runs");
    g.expect(pc_spr, "S8-PC-SPR: in every run F2 reads each of the eight sprite lists once in order: word 0, word 1 once, the data pairs of its height and the end pair");
    g.expect(pc_static, "S8-PC-STATIC: the start and the stop of every sprite list have the same parity");
    g.expect(pc_lom, "S8-PC-LOM: every line-offset measure of T_first and T_end is valid (within P/8 of a whole number of lines)");
    g.assert_line("S8", pre.setup && pre.frame && pre.tag && pc_spr && pc_static && pc_lom, ok,
        "S8: opus:AA-sprxpos.c5 opus:AA-sprxpos.c6 sol:AA-sprxpos.c13 opus:AA-sprxctl.c8 opus:AA-sprxctl.c12 sol:AA-sprxctl.c22 sol:AA-sprxctl.c28: "
        "the vertical start of a sprite is one 10-bit value (SPRxPOS bits 15-8 = SV7-SV0, SPRxCTL bit 2 = SV8, bit 6 = SV9) and bit SVk moves the start by 2^k lines (p.28): "
        "changing SVk alone moves the read of the first data word of every list by 2^k lines, for k = 0..9, in joint runs (start and stop together) and, for k = 1..3, in start-only runs");
}

// ---- S9 -----------------------------------------------------------------------
inline void prop_S9() {
    if (!want("S9")) return;
    Pre pre("S9");
    bool pc_spr = true, pc_static = true, pc_lom = true;
    bool ok = true;
    int only_k = -1;
    if (g_mutate) only_k = mutating("S9-PC-FRAME") ? 9 : 2;
    const bool spren = !mutating("S9-PC-SPR");
    for (const auto& p : joint_pairs()) {
        if (only_k >= 0 && p.k != only_k) continue;
        int s1 = p.s1, e1 = p.e1;
        if (mutating("S9") && p.k == 2) { s1 = 0x48; e1 = 0x50; }
        int s0 = p.s0;
        if (mutating("S9-PC-STATIC") && p.k == 2) s0 = 0x41;
        LomData a = lom_run("S9", s0, p.e0, p.lf, spren, pre, pc_spr, pc_static);
        LomData b = lom_run("S9", s1, e1, p.lf, true, pre, pc_spr, pc_static, mutating("S9-PC-LOM"));
        PairResult pr = compare_pair(a, b);
        if (!pr.valid) pc_lom = false;
        long want = 1L << p.k;
        g.note("S9: k = " + dec(p.k) + ": delta(T_end) = " + dec(pr.dend[0]) + " lines for sprite 0 (page: " + dec(want) + ")");
        ok = ok && pr.valid && all_eq(pr.dend, want);
        if (g_mutate) break;
    }
    for (int k = 1; k <= 3 && !g_mutate; ++k) {
        LomData a = lom_run("S9", 0x40, 0x50, false, true, pre, pc_spr, pc_static);
        LomData b = lom_run("S9", 0x40, 0x50 + (1 << k), false, true, pre, pc_spr, pc_static);       // end-only
        LomData c = lom_run("S9", 0x40 + (1 << k), 0x50, false, true, pre, pc_spr, pc_static);       // start-only
        PairResult pe = compare_pair(a, b), ps = compare_pair(a, c);
        if (!pe.valid || !ps.valid) pc_lom = false;
        g.note("S9: k = " + dec(k) + ": end-only delta(T_end) = " + dec(pe.dend[0]) + ", delta(T_first) = " + dec(pe.dfirst[0]) + "; start-only delta(T_end) = " + dec(ps.dend[0]));
        ok = ok && pe.valid && ps.valid && all_eq(pe.dend, 1L << k) && all_eq(pe.dfirst, 0) && all_eq(ps.dend, 0);
    }
    pre.print("the S9 runs");
    g.expect(pc_spr, "S9-PC-SPR: in every run F2 reads each of the eight sprite lists once in order: word 0, word 1 once, the data pairs of its height and the end pair");
    g.expect(pc_static, "S9-PC-STATIC: the start and the stop of every sprite list have the same parity");
    g.expect(pc_lom, "S9-PC-LOM: every line-offset measure of T_first and T_end is valid (within P/8 of a whole number of lines)");
    g.assert_line("S9", pre.setup && pre.frame && pre.tag && pc_spr && pc_static && pc_lom, ok,
        "S9: opus:AA-sprxctl.c6 opus:AA-sprxctl.c9 opus:AA-sprxctl.c13 sol:AA-sprxctl.c20 sol:AA-sprxctl.c23 sol:AA-sprxctl.c29: "
        "the vertical end of a sprite is one 10-bit value (SPRxCTL bits 15-8 = EV7-EV0, bit 1 = EV8, bit 5 = EV9) and bit EVk moves the end by 2^k lines (p.28): "
        "changing EVk alone moves the read of the end pair of every list by 2^k lines, for k = 0..9 in joint runs and, for k = 1..3, in end-only runs, where the first data word does not move, and the start-only runs do not move the end pair");
}

}  // namespace s3
