// s3_props_s1516.h -- propositions S15 and S16 (the UHRES sprite pointer)

#pragma once

#include "s3_props_s89.h"

namespace s3 {

struct UData {
    std::shared_ptr<RunData> r;
    bool ok = false;
    uint64_t P = 0;
    std::vector<int64_t> g;              // start time of each group of U accesses, relative to t_F of F2
    int u_reads = 0, u_writes = 0, u2_reads = 0, u2_writes = 0;
};

inline RunSpec uhres_spec(const std::string& pfx, int start, uint16_t stop_reg, bool lf, bool uhres_bit, int bplh_start, int bplh_stop, bool ecs) {
    Std o; o.bpu = 1; o.dmacon = 0x83A0; o.lf = lf;
    o.bplcon0_extra = uhres_bit ? 0x0080 : 0x0000;
    if (ecs) { o.write_bplcon4 = false; o.write_fmode = false; }
    o.extra.push_back({R_SPRHSTRT, uint16_t(start)});
    o.extra.push_back({R_SPRHSTOP, stop_reg});
    o.extra.push_back({R_BPLHSTRT, uint16_t(bplh_start)});
    o.extra.push_back({R_BPLHSTOP, uint16_t(bplh_stop)});
    o.extra.push_back({R_BPLHMOD, 0x0000});
    RunSpec s = base_spec(o, pfx);
    if (ecs) s.chipset = 0x08;
    s.vb = spr_ptrs(own_lists());
    for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
    push_ptr(s.vb, R_SPRHPTH, R_SPRHPTL, kU);
    push_ptr(s.vb, R_BPLHPTH, R_BPLHPTL, kU2);
    s.vb_intended = s.vb;
    if (mutating((pfx + "-PC-MOVE").c_str())) s.vb[s.vb.size() - 3].val ^= 0x0100;       // SPRHPTL
    EventSpec fin; fin.nvb = 4; s.ev.push_back(fin);
    if (lf) s.budget = 60'000'000;
    return s;
}

inline UData u_analyse(std::shared_ptr<RunData> r) {
    UData d;
    d.r = r;
    Frame f = get_frame(*r, 2);
    d.P = chip_line_period(*r);
    d.ok = f.ok && d.P > 0;
    if (!d.ok) return d;
    uint64_t last = 0;
    bool have = false;
    for (size_t i = acc_from(*r, f.t0); i < r->acc.size() && r->acc[i].cyc < f.t1; ++i) {
        const Acc& a = r->acc[i];
        uint32_t b = a.waddr * 2;
        if (b >= kU && b < kU + 0x10000) {
            if (a.wr) ++d.u_writes; else ++d.u_reads;
            if (!have || a.cyc - last >= d.P / 2) d.g.push_back(int64_t(a.cyc) - int64_t(f.t0));
            last = a.cyc;
            have = true;
        } else if (b >= kU2 && b < kU2 + 0x10000) {
            if (a.wr) ++d.u2_writes; else ++d.u2_reads;
        }
    }
    return d;
}

inline UData u_run(const std::string& pfx, int start, uint16_t stop_reg, bool lf, bool uhres, int bs, int be, bool ecs, Pre& pre, bool& pc_move) {
    RunSpec s = uhres_spec(pfx, start, stop_reg, lf, uhres, bs, be, ecs);
    auto r = run(s);
    pre.check(*r, 1, 2, lf, true, false);
    Frame f = get_frame(*r, 2);
    if (!moves_fetched(*r, f, intended_vb(*r), r->lay.vb_data)) pc_move = false;
    return u_analyse(r);
}

// the groups lie one line apart: every gap is one line (LOM)
inline bool consecutive_lines(const UData& d) {
    if (d.g.size() < 1) return false;
    for (size_t i = 1; i < d.g.size(); ++i) {
        long k = 0;
        if (!lom_delta(d.g[i] - d.g[i - 1], d.P, &k) || k != 1) return false;
    }
    return true;
}

// ---- S15 ----------------------------------------------------------------------
inline void prop_S15() {
    if (!want("S15")) return;
    Pre pre("S15");
    bool pc_move = true, pc_groups = true, ok = true, lom_ok = true;
    int only_k = -1;
    if (g_mutate) only_k = mutating("S15-PC-FRAME") ? 9 : 2;
    const bool uhres = !mutating("S15-PC-GROUPS");
    struct P15 { int k, s0, e0, s1, e1; bool lf; };
    std::vector<P15> pairs;
    for (const auto& p : joint_pairs()) pairs.push_back({p.k, p.s0, p.e0, p.s1, p.e1, p.lf});
    pairs.push_back({10, 0x40, 0x48, 0x440, 0x448, true});
    for (const auto& p : pairs) {
        if (only_k >= 0 && p.k != only_k) continue;
        int s1 = p.s1, e1 = p.e1;
        if (mutating("S15") && p.k == 2) { s1 = 0x48; e1 = 0x50; }
        UData a = u_run("S15", p.s0, uint16_t(p.e0), p.lf, uhres, 0, 0, false, pre, pc_move);
        UData b = u_run("S15", s1, uint16_t(e1), p.lf, true, 0, 0, false, pre, pc_move);
        if (a.g.size() < 2) pc_groups = false;
        long df = 0, dl = 0;
        bool v = a.ok && b.ok && !a.g.empty() && !b.g.empty() &&
                 lom_delta(b.g.front() - a.g.front(), a.P, &df) && lom_delta(b.g.back() - a.g.back(), a.P, &dl);
        if (!v) lom_ok = false;
        long want = 1L << p.k;
        bool good = v && df == want && dl == want && consecutive_lines(a) && consecutive_lines(b);
        g.note("S15: k = " + dec(p.k) + ": " + dec(long(a.g.size())) + " groups in the reference run; first group moved " + dec(df) + " lines, last group moved " + dec(dl) + " lines (page: " + dec(want) + ")");
        ok = ok && good;
        if (g_mutate) break;
    }
    for (int k = 1; k <= 3 && !g_mutate; ++k) {
        UData a = u_run("S15", 0x40, 0x50, false, true, 0, 0, false, pre, pc_move);
        UData b = u_run("S15", 0x40 + (1 << k), 0x50, false, true, 0, 0, false, pre, pc_move);      // start-only
        UData c = u_run("S15", 0x40, uint16_t(0x50 + (1 << k)), false, true, 0, 0, false, pre, pc_move);   // stop-only
        long bf = 0, bl = 0, cf = 0, cl = 0;
        bool vb = a.ok && b.ok && !a.g.empty() && !b.g.empty() && lom_delta(b.g.front() - a.g.front(), a.P, &bf) && lom_delta(b.g.back() - a.g.back(), a.P, &bl);
        bool vc = a.ok && c.ok && !a.g.empty() && !c.g.empty() && lom_delta(c.g.front() - a.g.front(), a.P, &cf) && lom_delta(c.g.back() - a.g.back(), a.P, &cl);
        if (!vb || !vc) lom_ok = false;
        long n_a = long(a.g.size()), n_b = long(b.g.size()), n_c = long(c.g.size());
        g.note("S15: k = " + dec(k) + ": start-only first group " + dec(bf) + " lines, groups " + dec(n_a) + " -> " + dec(n_b) + "; stop-only last group " + dec(cl) + " lines, groups " + dec(n_a) + " -> " + dec(n_c));
        ok = ok && vb && vc && bf == (1L << k) && bl == 0 && n_a - n_b == (1L << k) && cl == (1L << k) && cf == 0 && n_c - n_a == (1L << k);
    }
    pre.print("the S15 runs");
    g.expect(pc_move, "S15-PC-MOVE: in every run F2's vertical-blank block fetches the SPRHPTH and SPRHPTL MOVEs with the intended data words");
    g.expect(pc_groups, "S15-PC-GROUPS: in every reference run the port shows at least two groups of accesses in region U in F2");
    g.expect(lom_ok, "S15-PC-LOM: every line-offset measure of the first and the last group is valid (within P/8 of a whole number of lines)");
    g.assert_line("S15", pre.setup && pre.frame && pre.tag && pc_move && pc_groups && lom_ok, ok,
        "S15: sol:AA-sprhstop.c5 opus:AA-sprhstop.c7 sol:AA-sprhstop.c11 sol:AA-sprhstop.c12 sol:AA-sprhstop.c13 sol:AA-sprhstop.c14 sol:AA-sprhstop.c15 sol:AA-sprhstop.c16 sol:AA-sprhstop.c17 sol:AA-sprhstop.c18 sol:AA-sprhstop.c19 sol:AA-sprhstop.c20 sol:AA-sprhstop.c21 "
        "sol:AA-sprhstrt.c5 opus:AA-sprhstrt.c6 sol:AA-sprhstrt.c11 sol:AA-sprhstrt.c12 sol:AA-sprhstrt.c13 sol:AA-sprhstrt.c14 sol:AA-sprhstrt.c15 sol:AA-sprhstrt.c16 sol:AA-sprhstrt.c17 sol:AA-sprhstrt.c18 sol:AA-sprhstrt.c19 sol:AA-sprhstrt.c20 sol:AA-sprhstrt.c21: "
        "SPRHSTRT bits 10-0 set the line where the UHRES sprite starts and SPRHSTOP bits 10-0 the line where it stops (p.28): "
        "the accesses of the UHRES sprite pointer form one run of consecutive-line groups, shifting the start by 2^k lines moves the first group by 2^k lines and shifting the stop by 2^k lines moves the last group by 2^k lines, for k = 0..10, "
        "a start-only shift reduces the number of groups by 2^k and a stop-only shift increases it by 2^k, for k = 1..3");
}

// ---- S16 ----------------------------------------------------------------------
inline void prop_S16() {
    if (!want("S16")) return;
    Pre pre("S16");
    bool pc_move = true, pc_u = true, ok = true;
    const bool uhres = !mutating("S16-PC-U");
    const int bs = mutating("S16-PC-U2") ? 0 : 0x40, be = mutating("S16-PC-U2") ? 0 : 0x80;
    for (int ecs = 0; ecs < 2; ++ecs) {
        if (g_mutate && ecs == 1 && mutating("S16")) {}        // both configurations run under the mutation
        UData d0 = u_run("S16", 0x40, 0x0048, false, uhres, bs, be, ecs == 1, pre, pc_move);
        UData d1 = u_run("S16", 0x40, uint16_t(mutating("S16") ? 0x0048 : 0x8048), false, uhres, bs, be, ecs == 1, pre, pc_move);
        std::string cn = ecs ? "ECS" : "AA";
        if (d0.g.size() < 2 || d1.g.size() < 2 || d0.u2_reads + d0.u2_writes == 0 || d1.u2_reads + d1.u2_writes == 0) pc_u = false;
        g.note("S16 " + cn + ": SPRHWRM = 0: U reads " + dec(d0.u_reads) + " writes " + dec(d0.u_writes) + ", U2 reads " + dec(d0.u2_reads) + " writes " + dec(d0.u2_writes) +
               "; SPRHWRM = 1: U reads " + dec(d1.u_reads) + " writes " + dec(d1.u_writes) + ", U2 reads " + dec(d1.u2_reads) + " writes " + dec(d1.u2_writes));
        bool dir0 = d0.u_writes == 0 && d0.u_reads > 0;
        bool dir1 = d1.u_reads == 0 && d1.u_writes > 0;
        bool u2 = d0.u2_writes == 0 && d1.u2_writes == 0;
        long df = 0, dl = 0;
        bool same = d0.ok && d1.ok && !d0.g.empty() && !d1.g.empty() && d0.g.size() == d1.g.size() &&
                    lom_delta(d1.g.front() - d0.g.front(), d0.P, &df) && lom_delta(d1.g.back() - d0.g.back(), d0.P, &dl) && df == 0 && dl == 0;
        ok = ok && dir0 && dir1 && u2 && same;
    }
    pre.print("the S16 runs");
    g.expect(pc_move, "S16-PC-MOVE: in every run F2's vertical-blank block fetches the SPRHPTH and SPRHPTL MOVEs with the intended data words");
    g.expect(pc_u, "S16-PC-ACCESS: in every run the port shows at least two groups of accesses in region U and at least one access in region U2 in F2");
    g.assert_line("S16", pre.setup && pre.frame && pre.tag && pc_move && pc_u, ok,
        "S16: sol:AA-sprhstop.c6 sol:AA-sprhstop.c22: with SPRHSTOP bit 15 (SPRHWRM) set the memory cycles of the UHRES sprite pointer are writes and with the bit clear they are reads (p.28), with the AA and with the ECS chip set selected: "
        "every U access is a write with SPRHWRM = 1 and a read with SPRHWRM = 0, every U2 access is a read in both runs, and the U groups lie on the same lines in both runs");
}

}  // namespace s3
