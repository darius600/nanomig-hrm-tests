// s3_props_s1011.h -- propositions S10 and S11 (attach bit; SSCAN2 and SH10)

#pragma once

#include "s3_props_s89.h"

namespace s3 {

// ---- S10 ----------------------------------------------------------------------
constexpr uint16_t kColE = 0x0F0, kColO = 0xF0F;

inline int xp_of(int p) { return 0x90 + 0x30 * p; }

inline std::shared_ptr<RunData> run_s10(const std::string& pfx, uint16_t ctl_mask_bits_sprites, bool even_visible, bool omit_attach) {
    Std o; o.bpu = 1; o.pal = Pal::Att; o.bplcon4 = 0x0012; o.dmacon = 0x8380;
    for (int p = 0; p < 4; ++p) {
        for (int s = 2 * p; s <= 2 * p + 1; ++s) {
            bool even = (s & 1) == 0;
            uint16_t ctl = (ctl_mask_bits_sprites >> s) & 1 ? 0x0080 : 0x0000;
            if (omit_attach && s == 1) ctl = 0;
            if (omit_attach && s == 5) ctl = 0;
            manual_load(o.extra, s, xp_of(p), ctl, 0x0000, even ? (even_visible ? 0xFFFF : 0x0000) : 0x0000);
        }
    }
    RunSpec sp = base_spec(o, pfx);
    sp.vb = bpl_ptrs(all_z(), 1);
    EventSpec fin; fin.nvb = 4; sp.ev.push_back(fin);
    return run(sp);
}

// the colour of pair p's span on line n: all 32 pixels one colour; returns the colour, 0xFFFF when mixed or absent
inline uint16_t pair_colour(const std::vector<uint8_t>& is_e, const std::vector<uint8_t>& is_o, int edge) {
    if (edge < 0 || size_t(edge + 32) > is_e.size()) return 0xFFFF;
    int e = count_in_range(is_e, edge, 32), o = count_in_range(is_o, edge, 32);
    if (e == 32 && o == 0) return kColE;
    if (o == 32 && e == 0) return kColO;
    return 0xFFFF;
}

inline void prop_S10() {
    if (!want("S10")) return;
    Pre pre("S10");
    // run -> bit mask of sprites whose CTL bit 7 is set
    const uint16_t masks[4] = {0x0000, uint16_t((1 << 1) | (1 << 5)), uint16_t((1 << 3) | (1 << 7)), uint16_t((1 << 0) | (1 << 2) | (1 << 4) | (1 << 6))};
    const char* names[4] = {"a", "b", "c", "d"};
    bool pc_visible = true, ok = true;
    int edge0 = -1;
    for (int ri = 0; ri < 4; ++ri) {
        if (g_mutate && !mutating("S10-PC-VISIBLE") && ri > 1) continue;      // a mutation needs runs a and b only
        if (g_mutate && mutating("S10-PC-VISIBLE") && ri > 0) continue;
        bool omit = mutating("S10") && ri == 1;
        auto r = run_s10("S10", masks[ri], !(mutating("S10-PC-VISIBLE") && ri == 0), omit);
        pre.check(*r, 1, 1, false, true, true);
        FrameView fv(*r, 1);
        if (!fv.ok) { ok = false; pc_visible = false; continue; }
        int bad = 0;
        for (int n = 0x30; n <= 0xFF; ++n) {
            auto e = fv.cls(n, kColE), o = fv.cls(n, kColO);
            std::vector<uint8_t> u(e.size());
            for (size_t i = 0; i < e.size(); ++i) u[i] = e[i] | o[i];
            int ed = first_one(u);
            if (ri == 0 && n == 0x30) edge0 = ed;
            bool good = ed >= 0 && ed == edge0;
            int total = 0;
            for (int p = 0; p < 4 && good; ++p) {
                uint16_t want_c = ((masks[ri] >> (2 * p + 1)) & 1) ? kColO : kColE;
                good = pair_colour(e, o, ed + 96 * p) == want_c;
                total += 32;
            }
            if (good && count_ones(u) != total) good = false;
            if (!good) ++bad;
            if (ri == 0 && !good) pc_visible = false;
        }
        g.note(std::string("S10: run ") + names[ri] + ": lines whose four pair spans do not show the predicted colours: " + dec(bad));
        if (bad) ok = false;
    }
    pre.print("the S10 runs");
    g.expect(pc_visible, "S10-PC-VISIBLE: in run a every even sprite is visible as a 32-output-pixel cE span on every line 0x30-0xFF");
    g.assert_line("S10", pre.setup && pre.frame && pre.tag && pre.pixel && pc_visible, ok,
        "S10: opus:AA-sprxctl.c7 sol:AA-sprxctl.c21: SPRxCTL bit 7 (ATT) attaches an odd sprite to its even partner and the same bit in an even sprite's SPRxCTL attaches nothing (p.28): "
        "a pair shows cO if its odd sprite's CTL has bit 7 set and cE otherwise, in the four runs a-d, for the four pairs on every line");
}

// ---- S11 ----------------------------------------------------------------------
// manual bands (2.8): sprite x is loaded at line a_x = 0x40 + 0x18 x and disarmed at a_x + 0x10
inline std::shared_ptr<RunData> run_s11(const std::string& pfx, bool sscan2, bool sh10, uint16_t data, bool* beam_ok) {
    Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = 0x8380; o.fmode = sscan2 ? 0x8000 : 0x0000;
    RunSpec s = base_spec(o, pfx);
    s.vb = bpl_ptrs(all_z(), 1);
    const int h = mutating((pfx + "-PC-BEAM").c_str()) ? 0xE0 : 0x08;
    EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
    bool first = true;
    uint16_t posw = uint16_t(0x4000 | (sh10 ? 0xC8 : 0x48));
    for (int x = 0; x < 8; ++x) {
        int a = 0x40 + 0x18 * x;
        EventSpec e; e.nvb = first ? 2 : 0; first = false; e.line = a; e.h = h;
        e.wr.push_back({R_SPRxPOS(x), posw});
        e.wr.push_back({R_SPRxCTL(x), 0x0000});
        e.wr.push_back({R_SPRxDATB(x), data});
        e.wr.push_back({R_SPRxDATA(x), data});
        s.ev.push_back(e);
        EventSpec d; d.line = a + 0x10; d.h = h; d.wr.push_back({R_SPRxCTL(x), 0x0000});
        s.ev.push_back(d);
    }
    EventSpec fin; fin.nvb = 2; s.ev.push_back(fin);
    auto r = run(s);
    Frame f2 = get_frame(*r, 2);
    bool ok = f2.ok;
    for (size_t i = 1; i + 1 < r->spec.ev.size() && ok; ++i) {
        const auto& e = r->spec.ev[i];
        uint16_t vp = r->ev_word(int(i), 2), vh = r->ev_word(int(i), 4);
        int line = ((vp & 7) << 8) | (vh >> 8);
        uint64_t tm = marker_time(*r, int(i));
        if (line != e.line || !(tm >= f2.t0 && tm < f2.t1)) ok = false;
    }
    if (beam_ok) *beam_ok = ok;
    return r;
}

inline void prop_S11() {
    if (!want("S11")) return;
    Pre pre("S11");
    bool pc_beam = true, pc_shown = true, pc_shown1 = true, ok = true;
    // runs[sscan2][sh10]
    std::shared_ptr<RunData> runs[2][2];
    BandView bv[2][2];
    for (int sc = 0; sc < 2; ++sc)
        for (int sh = 0; sh < 2; ++sh) {
            bool b = true;
            bool sscan = sc == 1 && !mutating("S11");
            uint16_t data = ((mutating("S11-PC-SHOWN") && sc == 0 && sh == 0) || (mutating("S11-PC-SHOWN1") && sc == 1)) ? uint16_t(0) : uint16_t(0x8000);
            runs[sc][sh] = run_s11("S11", sscan, sh == 1, data, &b);
            if (!b) pc_beam = false;
            pre.check(*runs[sc][sh], 1, 2, false, true, true);
            bv[sc][sh] = BandView(*runs[sc][sh], 2);
        }
    // the band lines: lines a_x + 1 .. a_x + 0x0F
    for (int x = 0; x < 8; ++x) {
        int a = 0x40 + 0x18 * x;
        for (int n = a + 1; n < a + 0x10; ++n) {
            const auto& r00 = bv[0][0].runs[n];
            if (r00.empty()) pc_shown = false;
            if (bv[1][0].runs[n].empty() || bv[1][1].runs[n].empty()) pc_shown1 = false;
            if (!bv[0][1].have[n] || r00.empty() || bv[0][1].runs[n].empty()) { ok = false; continue; }
            int d = bv[0][1].runs[n][0].first - r00[0].first;
            if (d != 512) ok = false;
            if (bv[1][0].runs[n] != bv[1][1].runs[n]) ok = false;
        }
    }
    pre.print("the S11 runs");
    g.expect(pc_beam, "S11-PC-BEAM: in every run every beam marker's re-read shows its target line, and the markers lie in F2");
    g.expect(pc_shown, "S11-PC-SHOWN: with SSCAN2 = 0 and SH10 = 0 every band shows the sprite on lines a_x + 1 .. a_x + 0x0F");
    g.expect(pc_shown1, "S11-PC-SHOWN1: in each of the two SSCAN2 = 1 runs every band shows at least one cS pixel on lines a_x + 1 .. a_x + 0x0F");
    g.assert_line("S11", pre.setup && pre.frame && pre.tag && pre.pixel && pc_beam && pc_shown && pc_shown1, ok,
        "S11: opus:AA-sprxpos.c9 sol:AA-sprxpos.c15: with FMODE bit 15 (SSCAN2) set SH10 takes no part in the horizontal coincidence (p.28): "
        "with SSCAN2 = 0 the sprite with SH10 = 1 is 512 output pixels to the right of the sprite with SH10 = 0 in every band, and with SSCAN2 = 1 the band lines of the two runs are identical pixel for pixel, for all eight sprites");
}

}  // namespace s3
