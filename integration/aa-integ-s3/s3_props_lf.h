// s3_props_lf.h -- proposition LF (the field length of the long frame, p.29 VTOTAL)

#pragma once

#include "s3_props_s89.h"

namespace s3 {

// ---- LF -----------------------------------------------------------------------
// With VARBEAMEN set, VTOTAL = 0x05FF and LACE clear a field has VTOTAL + 1 = 1536 lines.
struct LfRun {
    std::shared_ptr<RunData> r;
    long n[3] = {0, 0, 0};          // field length in lines of F1, F2 (index 1, 2)
    bool lines_ok = true;           // every length is a whole number of lines within P/8
    bool have = false;
};

inline LfRun lf_run(uint16_t vtotal, Pre& pre) {
    LfRun lr;
    Std o; o.bpu = 1; o.dmacon = 0x8380; o.lf = true; o.vtotal = vtotal;
    if (mutating("LF-PC-FRAME")) o.dmacon &= ~0x0080;          // COPEN is not set
    RunSpec s = base_spec(o, "LFB");                           // the stimulus changes of the common preconditions are made here
    if (mutating("LF-PC-SETUP")) s.no_marker = true;
    if (mutating("LF-PC-TAG")) s.no_tag_moves = true;
    s.vb = bpl_ptrs(all_z(), 1);
    EventSpec fin; fin.nvb = 4; s.ev.push_back(fin);
    s.budget = 60'000'000;
    lr.r = run(s);
    pre.check(*lr.r, 1, 2, 0, true, false);
    lr.have = lr.r->tF.size() >= 3;
    for (int i = 1; i <= 2 && lr.have; ++i) {
        Frame f = get_frame(*lr.r, i);
        uint64_t P = line_period(f);
        if (!f.ok || P == 0) { lr.have = false; break; }
        int64_t d = int64_t(lr.r->tF[i]) - int64_t(lr.r->tF[i - 1]);
        long k = 0;
        if (!lom_delta(d, P, &k)) lr.lines_ok = false;
        lr.n[i] = k;
    }
    return lr;
}

inline void prop_LF() {
    if (!want("LF")) return;
    Pre pre("LF");
    const uint16_t vt = mutating("LF-FRAME") ? 0x05FE : 0x05FF;
    LfRun a = lf_run(vt, pre);
    // the PC-FRAME of LF also asks for whole lines: the field length is a measure only then (2.11)
    if (!a.lines_ok) pre.frame = false;
    const long want_n = 1536;  // p.29 for VTOTAL = 0x05FF; fixed, the LF-FRAME mutation changes the stimulus only
    g.note("LF: VTOTAL = 0x" + hexs(vt) + ": the field of F1 lasts " + dec(a.n[1]) + " lines, the field of F2 lasts " + dec(a.n[2]) + " lines (LF-FRAME expects " + dec(want_n) + ")");
    const bool run_control = !g_mutate || mutating("LF-CONTROL");
    LfRun c;
    if (run_control) {
        c = lf_run(mutating("LF-CONTROL") ? 0x05FF : 0x04FF, pre);
        if (!c.lines_ok) pre.frame = false;
        g.note("LF-CONTROL: VTOTAL = 0x" + hexs(mutating("LF-CONTROL") ? 0x05FF : 0x04FF) + ": the field of F1 lasts " + dec(c.n[1]) + " lines, the field of F2 lasts " + dec(c.n[2]) + " lines");
    }
    pre.print("the LF runs");
    const bool pre_all = pre.setup && pre.frame && pre.tag && a.have;
    g.assert_line("LF-FRAME", pre_all, a.n[1] == want_n && a.n[2] == want_n,
        "LF-FRAME: with VARBEAMEN set, VTOTAL = 0x05FF and LACE clear a field has VTOTAL + 1 = 1536 lines (p.29, VTOTAL: 'It's the line number to reset the counter, so there's this many + 1 in a field'): "
        "the fields F1 and F2 of the long-frame run last exactly VTOTAL + 1 lines");
    if (run_control)
        g.assert_line("LF-CONTROL", pre_all && c.have, a.n[1] - c.n[1] == 256 && a.n[2] - c.n[2] == 256,
            "LF-CONTROL: the field length follows VTOTAL line for line (p.29: 'this many + 1 in a field'): with VTOTAL = 0x05FF the fields F1 and F2 last exactly 256 lines more than with VTOTAL = 0x04FF");
}

}  // namespace s3
