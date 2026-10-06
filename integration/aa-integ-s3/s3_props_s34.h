// s3_props_s34.h -- propositions S3 and S4 (manual sprites: registers, arming, output)

#pragma once

#include "s3_spr.h"

namespace s3 {


// S4's comparator on one frame: on every graded line each of the eight columns
// shows the 32-pixel pattern of `word` and no other cS pixel is shown.
// edge0 is the leftmost cS pixel of the first graded line; column x begins 48 output pixels
// (24 lores pixels, SPECIFICATION.md 2.7) to the right of column x-1.
inline bool eight_columns_ok(const RunData& r, const FrameView& fv, uint16_t word, int* edge0_out, int* bad_lines) {
    int bad = 0, edge0 = -1;
    if (!fv.ok) { if (bad_lines) *bad_lines = -1; return false; }
    for (int n = 0x30; n <= 0xFF; ++n) {
        auto cs = fv.cls(n, kColS);
        if (cs.empty()) { ++bad; continue; }
        if (edge0 < 0) edge0 = first_one(cs);
        bool ok = edge0 >= 0;
        int total = 0;
        for (int x = 0; x < 8 && ok; ++x) {
            ok = pattern_at(cs, edge0 + 48 * x, word);
            total += 2 * __builtin_popcount(word);
        }
        if (ok && count_ones(cs) != total) ok = false;
        if (!ok) ++bad;
    }
    if (edge0_out) *edge0_out = edge0;
    if (bad_lines) *bad_lines = bad;
    return bad == 0 && edge0 >= 0;
}

inline RunSpec manual8_spec(const std::string& pfx, uint16_t setup_word, bool events, int mut_kind) {
    Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = 0x8380;
    if (mutating((pfx + "-PC-STATIC").c_str())) o.dmacon = 0x83A0;
    for (int x = 0; x < 8; ++x) manual_load(o.extra, x, kXs[x], 0x0000, setup_word, setup_word);
    RunSpec s = base_spec(o, pfx);
    s.vb = bpl_ptrs(all_z(), 1);
    EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
    if (events) {
        const int h = mutating((pfx + "-PC-BEAM").c_str()) ? 0xE0 : 0x10;
        bool first = true;
        for (int x = 0; x < 8; ++x) {
            int d = 0x50 + 0x14 * x;
            EventSpec a; a.nvb = first ? 2 : 0; first = false; a.line = d; a.h = h;
            if (!(mut_kind == 1 && x == 3)) a.wr.push_back({R_SPRxCTL(x), 0x0000});
            s.ev.push_back(a);
            EventSpec b; b.line = d + 4; b.h = h;
            b.wr.push_back({mut_kind == 3 && x == 3 ? R_SPRxDATA(x) : R_SPRxDATB(x), 0xE001});
            s.ev.push_back(b);
            EventSpec c; c.line = d + 8; c.h = h;
            if (!(mut_kind == 2 && x == 3)) c.wr.push_back({R_SPRxDATA(x), 0xE001});
            s.ev.push_back(c);
        }
        EventSpec fin; fin.nvb = 2; s.ev.push_back(fin);
    } else {
        EventSpec fin; fin.nvb = 4; s.ev.push_back(fin);
    }
    return s;
}

// ---- S3 -----------------------------------------------------------------------
inline void prop_S3() {
    if (!want("S3")) return;
    Pre pre("S3");
    int kind = mutating("S3") ? 1 : mutating("S3-b") ? 2 : mutating("S3-c") ? 3 : 0;
    uint16_t setup_word = mutating("S3-PC-F1") ? 0x0000 : 0xE001;
    RunSpec s = manual8_spec("S3", setup_word, true, kind);
    auto r = run(s);
    pre.check(*r, 1, 2, false, true, true);
    FrameView f1(*r, 1), f2(*r, 2);
    int edge0 = -1, bad1 = 0;
    bool pc_f1 = eight_columns_ok(*r, f1, 0xE001, &edge0, &bad1);
    bool pc_static = true;
    for (const auto& w : r->spec.setup) if (w.reg == R_DMACON && (w.val & 0x0020)) pc_static = false;
    bool pc_beam = f2.ok;
    for (size_t i = 1; i + 1 < r->spec.ev.size() && pc_beam; ++i) {
        const auto& e = r->spec.ev[i];
        uint16_t vp = r->ev_word(int(i), 2), vh = r->ev_word(int(i), 4);
        int line = ((vp & 7) << 8) | (vh >> 8);
        uint64_t tm = marker_time(*r, int(i));
        if (line != e.line || !(tm >= f2.f.t0 && tm < f2.f.t1)) pc_beam = false;
    }
    // the graded expectation
    bool ok = f2.ok && pc_f1 && edge0 >= 0;
    long bad = 0;
    if (ok) {
        for (int n = 0x30; n <= 0xFF; ++n) {
            auto cs = f2.cls(n, kColS);
            if (cs.empty()) { ++bad; continue; }
            for (int x = 0; x < 8; ++x) {
                int d = 0x50 + 0x14 * x;
                int edge = edge0 + 48 * x;
                bool good;
                if (n == d || n == d + 8) continue;             // lines left open
                if (n < d || n > d + 8) good = pattern_at(cs, edge, 0xE001);
                else good = count_in_range(cs, edge, 32) == 0;
                if (!good) { ++bad; }
            }
        }
        g.note("S3: columns x lines of F2 that do not show what the page predicts: " + dec(bad));
    }
    pre.print("the S3 run");
    g.expect(pc_f1, "S3-PC-F1: in F1 all eight columns show the 0xE001 pattern on every line 0x30-0xFF (" + dec(bad1) + " lines do not)");
    g.expect(pc_beam, "S3-PC-BEAM: every beam marker's re-read shows its target line, and the markers lie in F2");
    g.expect(pc_static, "S3-PC-STATIC: SPREN is never set by the program");
    bool pre_all = pre.setup && pre.frame && pre.tag && pre.pixel && pc_f1 && pc_beam && pc_static;
    g.assert_line("S3", pre_all, ok && bad == 0,
        "S3: opus:AA-sprxpos.c1 opus:AA-sprxpos.c2 sol:AA-sprxpos.c1 sol:AA-sprxpos.c2 opus:AA-sprxctl.c1 opus:AA-sprxctl.c2 opus:AA-sprxctl.c20 sol:AA-sprxctl.c1 sol:AA-sprxctl.c2 sol:AA-sprxctl.c34 sol:AA-sprxctl.c35 opus:AA-sprxdata.c1 opus:AA-sprxdata.c2 sol:AA-sprxdata.c1 sol:AA-sprxdata.c2 sol:AA-sprxdata.c17 opus:AA-sprxdatb.c1 opus:AA-sprxdatb.c2 opus:AA-sprxdatb.c9 opus:AA-sprxdatb.c10 sol:AA-sprxdatb.c1 sol:AA-sprxdatb.c2: "
        "SPRxPOS positions sprite x, a write to SPRxDATA arms it, a write to SPRxDATB alone does not arm it and a write to SPRxCTL disables sprite x and no other sprite (p.28), for each of the eight sprites");
}

// ---- S4 -----------------------------------------------------------------------
inline void prop_S4() {
    if (!want("S4")) return;
    Pre pre("S4");
    uint16_t setup_word = mutating("S4") ? 0x8007 : 0xE001;
    RunSpec s = manual8_spec("S4", setup_word, false, 0);
    auto r = run(s);
    pre.check(*r, 1, 2, false, true, true);
    FrameView f1(*r, 1);
    int edge0 = -1, bad = 0;
    bool ok = eight_columns_ok(*r, f1, 0xE001, &edge0, &bad);
    g.note("S4: F1 leftmost cS pixel of line 0x30 at output pixel " + dec(edge0) + "; lines that do not show eight 0xE001 columns: " + dec(bad));
    pre.print("the S4 run");
    bool pre_all = pre.setup && pre.frame && pre.tag && pre.pixel;
    g.assert_line("S4", pre_all, ok,
        "S4: opus:AA-sprxdatb.c5 opus:AA-sprxdatb.c8 opus:AA-sprxdatb.c11 sol:AA-sprxdata.c12 sol:AA-sprxdata.c15 sol:AA-sprxdata.c16 sol:AA-sprxdata.c18: "
        "while sprite x is enabled the words in its A and B buffers are shifted out, MSB first and leftmost, whenever the beam reaches the horizontal position in SPRxPOS, which means on every line: on every line 0x30-0xFF of F1 each column shows 6 cS, 24 non-cS and 2 cS output pixels");
}

}  // namespace s3
