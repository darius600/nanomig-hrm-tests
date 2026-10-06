// s3_props_s567.h -- propositions S5, S6 and S7 (sprite horizontal start bits)

#pragma once

#include "s3_props_s12.h"

namespace s3 {

// cS runs of every chip line 0x30-0xFF of one frame
struct BandView {
    bool ok = false;
    std::array<std::vector<std::pair<int, int>>, 0x100> runs;
    std::array<bool, 0x100> have;
    BandView() { have.fill(false); }
    BandView(const RunData& r, int fi, uint16_t colour = kColS) : BandView() {
        FrameView fv(r, fi);
        ok = fv.ok;
        if (!ok) return;
        for (int n = 0x30; n <= 0xFF; ++n) {
            auto c = fv.cls(n, colour);
            if (c.empty()) { ok = false; continue; }
            have[n] = true;
            runs[n] = runs_of(c);
        }
    }
};

// every sprite list x (at L_x) is read once in order in frame f, each of `words` words
inline bool all_lists_in_order(const RunData& r, const Frame& f, int words) {
    for (int x = 0; x < 8; ++x) if (!list_in_order(r, f, L_x(x), words)) return false;
    return true;
}

// the eight DMA bands of S5/S6: sprite x starts at b_x = 0x40 + 0x18 x and has 16 rows
inline RunSpec bands_spec(const std::string& pfx, int H, uint16_t data, uint16_t datb, uint16_t bplcon3, bool spren) {
    Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = spren ? 0x83A0 : 0x8380; o.bplcon3 = bplcon3;
    RunSpec s = base_spec(o, pfx);
    for (int x = 0; x < 8; ++x) {
        int b = 0x40 + 0x18 * x;
        std::vector<SprRow> rows(16, SprRow{data, datb});
        s.mem.push_back({L_x(x), spr_list(b, b + 0x10, H, false, rows)});
    }
    s.vb = spr_ptrs(own_lists());
    for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
    EventSpec fin; fin.nvb = 5; s.ev.push_back(fin);
    return s;
}

// ---- S5 -----------------------------------------------------------------------
struct S5Run {
    std::shared_ptr<RunData> r;
    BandView bv;
    bool pc_spr = false;
};

inline void prop_S5() {
    if (!want("S5")) return;
    Pre pre("S5");
    bool pc_spr = true, pc_band = true;
    std::map<int, S5Run> refs;
    auto get = [&](int H, uint16_t data, bool spren) {
        S5Run o;
        o.r = run(bands_spec("S5", H, data, 0x0000, 0x0C00, spren));
        pre.check(*o.r, 1, 2, false, true, true);
        Frame f = get_frame(*o.r, 2);
        o.bv = BandView(*o.r, 2);
        o.pc_spr = f.ok && all_lists_in_order(*o.r, f, 2 + 2 * 16 + 2);
        if (!o.pc_spr) pc_spr = false;
        return o;
    };
    auto ref_of = [&](int B) -> S5Run& {
        auto it = refs.find(B);
        if (it == refs.end()) it = refs.emplace(B, get(B, 0x8000, !mutating("S5-PC-SPR"))).first;
        return it->second;
    };
    // the lines that show the sprite in a reference run: exactly one 2-output-pixel cS run
    auto ref_lines = [&](const S5Run& ref) {
        std::vector<int> v;
        for (int n = 0x30; n <= 0xFF; ++n)
            if (ref.bv.have[n] && !ref.bv.runs[n].empty()) v.push_back(n);
        return v;
    };
    bool ok_all = true, ok_ctrl = true;
    for (int k = 1; k <= 10; ++k) {
        if (g_mutate && k != 3) continue;
        int B = k == 6 ? 0x480 : k == 10 ? 0x240 : 0x440;
        bool mut_bit = mutating("S5") && k == 3;
        int H = B + (mut_bit ? (1 << (k + 1)) : (1 << k));
        S5Run& ref = ref_of(B);
        S5Run sh = get(H, mutating("S5-PC-BAND") && k == 3 ? 0xC000 : 0x8000, true);
        auto lines = ref_lines(ref);
        // each band shows exactly one 2-output-pixel cS run on the same lines as in its reference run
        for (int n = 0x30; n <= 0xFF; ++n) {
            bool in_ref = std::find(lines.begin(), lines.end(), n) != lines.end();
            const auto& rr = ref.bv.runs[n];
            const auto& rs = sh.bv.runs[n];
            if (in_ref) {
                if (rr.size() != 1 || rr[0].second != 2) pc_band = false;
                if (rs.size() != 1 || rs[0].second != 2) pc_band = false;
            } else if (!rs.empty() || !rr.empty()) pc_band = false;
        }
        if (lines.size() < 8 * 2) pc_band = false;
        long bad = 0;
        int want_shift = 1 << (k - 1);
        int first_diff = 0;
        for (int n : lines) {
            if (ref.bv.runs[n].size() != 1 || sh.bv.runs[n].size() != 1) { ++bad; continue; }
            int d = sh.bv.runs[n][0].first - ref.bv.runs[n][0].first;
            first_diff = d;
            if (d != want_shift) ++bad;
        }
        ok_all = ok_all && bad == 0 && !lines.empty();
        if (k >= 2) ok_ctrl = ok_ctrl && bad == 0 && !lines.empty();
        g.note("S5: k = " + dec(k) + ": H = 0x" + hexs(B, 3) + " -> 0x" + hexs(H, 3) + ": leftmost cS pixel moved by " + dec(first_diff) +
               " output pixels (page: " + dec(want_shift) + ") on " + dec(long(lines.size())) + " lines, " + dec(bad) + " lines differ");
    }
    pre.print("the S5 runs");
    g.expect(pc_spr, "S5-PC-SPR: in every run F2 reads each of the eight sprite lists once in order: word 0, word 1 once, the sixteen data pairs and the end pair");
    g.expect(pc_band, "S5-PC-BAND: in every run each band shows exactly one 2-output-pixel cS run on the same lines as in its reference run, on at least sixteen lines");
    g.assert_line("S5", pre.setup && pre.frame && pre.tag && pre.pixel && pc_spr && pc_band, ok_all,
        "S5: opus:AA-sprxpos.c7 opus:AA-sprxctl.c10 opus:AA-sprxctl.c14 sol:AA-sprxctl.c24 sol:AA-sprxctl.c30: "
        "the horizontal start SH10-SH0 is one value (SPRxPOS bits 7-0 = SH10-SH3, SPRxCTL bit 0 = SH2 and bit 4 = SH1) and bit SHk moves the sprite by 35 * 2^k ns (p.28): "
        "changing SHk alone moves the leftmost cS pixel by 2^(k-1) output pixels, for k = 1..10, for every sprite and on every line of its band");
    g.expect_if(pre.setup && pre.frame && pre.tag && pre.pixel && pc_spr && pc_band, ok_ctrl,
        "S5-CONTROL: the comparator of S5 (the leftmost cS pixel of every band line moves by 2^(k-1) output pixels) applied to k = 2..10, SH2 in SPRxCTL bit 0 and SH3-SH10 in SPRxPOS bits 0-7");
}

// ---- S6 -----------------------------------------------------------------------
inline void prop_S6() {
    if (!want("S6")) return;
    Pre pre("S6");
    bool pc_spr = true;
    const uint16_t pats[3] = {0xAAAA, 0x5555, 0xFFFF};
    const uint16_t bplcon3 = mutating("S6-PC-WIDTH") ? 0x0C40 : 0x0CC0;
    struct Rn { std::shared_ptr<RunData> r; BandView bv; };
    Rn runs6[3][2];
    for (int p = 0; p < 3; ++p)
        for (int s = 0; s < 2; ++s) {
            uint16_t pat = pats[p];
            if (mutating("S6-PC-HALF") && p == 1) pat = 0xFFFF;
            if (mutating("S6-CONTROL") && p == 1) pat = 0xAAAA;
            int H = (mutating("S6") && s == 1) ? 0x440 : (0x440 + s);
            runs6[p][s].r = run(bands_spec("S6", H, pat, pat, bplcon3, !mutating("S6-PC-SPR")));
            pre.check(*runs6[p][s].r, 1, 2, false, true, true);
            Frame f = get_frame(*runs6[p][s].r, 2);
            runs6[p][s].bv = BandView(*runs6[p][s].r, 2);
            if (!(f.ok && all_lists_in_order(*runs6[p][s].r, f, 2 + 2 * 16 + 2))) pc_spr = false;
        }
    // V(pattern, SH0, line): cS pixels within the 12 output pixels beginning 2 pixels left of the band's cS run in the 0xFFFF run with the same SH0
    auto V = [&](int p, int s, int n) {
        const auto& ff = runs6[2][s].bv.runs[n];
        if (ff.empty()) return -1;
        int x0 = ff.front().first - 2;
        int c = 0;
        for (const auto& rr : runs6[p][s].bv.runs[n])
            for (int i = rr.first; i < rr.first + rr.second; ++i) if (i >= x0 && i < x0 + 12) ++c;
        return c;
    };
    // the comparator: one of the two settings shows all eight alternate pixels and the other none
    auto flips = [](int a, int b) { return (a == 0 && b == 8) || (a == 8 && b == 0); };
    bool pc_width = true, pc_half = true, ok = true, ok_ctrl = true;
    int nlines = 0, f0 = -1, f1 = -1;
    for (int n = 0x30; n <= 0xFF; ++n) {
        bool band0 = !runs6[2][0].bv.runs[n].empty(), band1 = !runs6[2][1].bv.runs[n].empty();
        if (band0 != band1) pc_width = false;
        if (!band0 || !band1) continue;
        ++nlines;
        if (V(2, 0, n) != 8 || V(2, 1, n) != 8) pc_width = false;
        if (V(0, 0, n) + V(1, 0, n) != 8 || V(0, 1, n) + V(1, 1, n) != 8) pc_half = false;
        int a0 = V(0, 0, n), a1 = V(0, 1, n), b1 = V(1, 1, n);
        if (!flips(a0, a1) || b1 != a0) ok = false;
        if (!flips(a0, V(1, 0, n))) ok_ctrl = false;
        if (f0 < 0) { f0 = a0; f1 = a1; }
    }
    if (nlines < 16) { pc_width = false; }
    g.note("S6: " + dec(nlines) + " band lines; first band line: V(0xAAAA, SH0 = 0) = " + dec(f0) + ", V(0xAAAA, SH0 = 1) = " + dec(f1));
    pre.print("the S6 runs");
    g.expect(pc_spr, "S6-PC-SPR: in every run F2 reads each of the eight sprite lists once in order");
    g.expect(pc_width, "S6-PC-WIDTH: V(0xFFFF, SH0 = 0) = V(0xFFFF, SH0 = 1) = 8 on every band line: the sprite is shown and is 8 output pixels wide");
    g.expect(pc_half, "S6-PC-HALF: V(0xAAAA, s) + V(0x5555, s) = 8 for s = 0 and s = 1 on every band line: exactly one of each pair of 35-ns pixels reaches the output");
    g.assert_line("S6", pre.setup && pre.frame && pre.tag && pre.pixel && pc_spr && pc_width && pc_half, ok,
        "S6: opus:AA-sprxctl.c11 sol:AA-sprxctl.c26: SPRxCTL bit 3 (SH0) moves the sprite by 35 ns (p.28): with 35-ns sprites {V(0xAAAA, 0), V(0xAAAA, 1)} = {0, 8} and V(0x5555, 1) = V(0xAAAA, 0) on every band line");
    g.expect_if(pre.setup && pre.frame && pre.tag && pre.pixel && pc_spr && pc_width, ok_ctrl,
        "S6-CONTROL: the comparator of S6 ({a, b} = {0, 8}) applied to the data words instead of SH0: {V(0xAAAA, 0), V(0x5555, 0)} = {0, 8} on every band line, the 0x5555 word being the 0xAAAA word shifted by one 35-ns sprite pixel");
}

// ---- S7 -----------------------------------------------------------------------
inline void prop_S7() {
    if (!want("S7")) return;
    Pre pre("S7");
    struct Rn { std::shared_ptr<RunData> r; BandView bv; };
    auto mk = [&](const char* name, uint16_t bplcon3, uint16_t data, bool write_ctl, uint16_t ctl, int pos_delta = 0) {
        Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = 0x8380; o.bplcon3 = bplcon3; o.sprctl_init = false;
        for (int x = 0; x < 8; ++x) {
            o.extra.push_back({R_SPRxPOS(x), uint16_t(0x4000 | ((kXs[x] >> 1) + pos_delta))});
            if (write_ctl) o.extra.push_back({R_SPRxCTL(x), ctl});
            o.extra.push_back({R_SPRxDATB(x), uint16_t(data == 0x8000 ? 0 : data)});
            o.extra.push_back({R_SPRxDATA(x), data});
        }
        if ((mutating("S7-PC-STATIC") && std::string(name) == "R0")) o.extra.push_back({R_SPRxCTL(0), 0x0000});
        RunSpec s = base_spec(o, "S7");
        s.vb = bpl_ptrs(all_z(), 1);
        EventSpec fin; fin.nvb = 4; s.ev.push_back(fin);
        Rn n;
        n.r = run(s);
        pre.check(*n.r, 1, 1, false, true, true);
        n.bv = BandView(*n.r, 1);
        return n;
    };
    const uint16_t lo = 0x0C40, hi = mutating("S7-PC-SHRES") ? uint16_t(0x0C40) : uint16_t(0x0CC0);
    const bool m_r0 = mutating("S7"), m_r0s = mutating("S7-b");
    Rn R0 = mk("R0", lo, 0x8000, m_r0, 0x0010);
    Rn R1 = mk("R1", lo, 0x8000, true, 0x0000);
    Rn R2 = mk("R2", lo, 0x8000, true, mutating("S7-PC-SH1") ? uint16_t(0x0000) : uint16_t(0x0010));
    Rn R0s = mk("R0s", hi, 0xAAAA, m_r0s, 0x0008);
    Rn R1s = mk("R1s", hi, 0xAAAA, true, 0x0000);
    Rn R2s = mk("R2s", hi, 0xAAAA, true, mutating("S7-PC-SH0") ? uint16_t(0x0000) : uint16_t(0x0008));
    Rn R1f = mk("R1f", hi, 0xFFFF, true, 0x0000);
    Rn R0c = mk("R0c", lo, 0x8000, true, 0x0001, mutating("S7-CONTROL") ? 1 : 0);
    Rn R0sc = mk("R0sc", hi, mutating("S7-CONTROL-V") ? uint16_t(0x5555) : uint16_t(0xAAAA), true, 0x0001);
    bool pc_static = true;
    {
        // the program's writes in R0 and R0s: no SPRxCTL write
        for (const Rn* n : {&R0, &R0s})
            for (const auto& w : n->r->spec.setup)
                for (int x = 0; x < 8; ++x) if (w.reg == R_SPRxCTL(x)) pc_static = false;
        if (m_r0 || m_r0s) pc_static = true;       // the contrast runs write SPRxCTL by design
    }
    bool pc_sh1 = true, pc_sh0 = true, pc_shres = true, ok1 = true, ok2 = true, okc1 = true, okc2 = true, okc3 = true;
    int nl = 0, d01 = -9, v01 = -9;
    for (int n = 0x30; n <= 0xFF; ++n) {
        const auto& a0 = R0.bv.runs[n]; const auto& a1 = R1.bv.runs[n]; const auto& a2 = R2.bv.runs[n];
        const auto& ac = R0c.bv.runs[n];
        if (a0.size() != 8 || a1.size() != 8 || a2.size() != 8 || ac.size() != 8) { ok1 = false; okc1 = false; pc_sh1 = false; continue; }
        ++nl;
        for (int x = 0; x < 8; ++x) {
            int d = a0[x].first - a1[x].first;
            d01 = d;
            if (d != 0 && d != 2) ok1 = false;
            int dc = ac[x].first - a1[x].first;
            if (dc != 0 && dc != 2) okc1 = false;
            if (dc != 2) okc3 = false;
            if (a2[x].first - a1[x].first != 1) pc_sh1 = false;
        }
        const auto& f = R1f.bv.runs[n];
        if (f.size() != 8) { pc_shres = false; ok2 = false; continue; }
        for (int x = 0; x < 8; ++x) {
            if (f[x].second != 8) pc_shres = false;
            int x0 = f[x].first - 4;
            auto V = [&](const Rn& rn) {
                int c = 0;
                for (const auto& rr : rn.bv.runs[n])
                    for (int i = rr.first; i < rr.first + rr.second; ++i) if (i >= x0 && i < x0 + 16) ++c;
                return c;
            };
            int v0 = V(R0s), v1 = V(R1s), v2 = V(R2s), vc = V(R0sc);
            v01 = v0;
            if (v0 != v1) ok2 = false;
            if (vc != v1) okc2 = false;
            if (v1 == v2) pc_sh0 = false;
        }
    }
    g.note("S7: " + dec(nl) + " lines; X(R0) - X(R1) = " + dec(d01) + " output pixels on the last line; V(R0s) = " + dec(v01) + " on the last line");
    pre.print("the S7 runs");
    g.expect(pc_static, "S7-PC-STATIC: the program writes no SPRxCTL register in R0 and R0s");
    g.expect(pc_sh1, "S7-PC-SH1: X(R2) - X(R1) = 1 output pixel for every column on every line: SH1 acts");
    g.expect(pc_sh0, "S7-PC-SH0: V(R1s) differs from V(R2s) for every column on every line: SH0 acts");
    g.expect(pc_shres, "S7-PC-SHRES: V(R1f) = 8 for every column on every line: the SHRES sprite is shown and is 8 output pixels wide");
    bool pre_all = pre.setup && pre.frame && pre.tag && pre.pixel && pc_static && pc_sh1 && pc_sh0 && pc_shres;
    g.assert_line("S7", pre_all, ok1 && ok2,
        "S7: opus:AA-sprxctl.c15 opus:AA-sprxctl.c16 sol:AA-sprxctl.c25 sol:AA-sprxctl.c27: "
        "after reset, before any write to SPRxCTL, bits SH1 and SH0 of every SPRxCTL are 0 (p.28 'SH1=0', 'SH0=0'): X(R0) - X(R1) is 0 or 2 output pixels and V(R0s) = V(R1s), for every column on every line");
    bool pre_ctl = pre.setup && pre.frame && pre.tag && pre.pixel && pc_static;
    g.expect_if(pre_ctl, okc1,
        "S7-CONTROL: the comparator of S7 for SH1 (X(R) - X(R1) is 0 or 2 output pixels) applied to R0c, the lores run with SPRxCTL = 0x0001 (SH2 = 1, 140 ns), for every column on every line");
    g.expect_if(pre_ctl, okc3,
        "S7-CONTROL-SH1: the comparator of S7-PC-SH1 (X(R) - X(R1) is a fixed number of output pixels) applied to R0c, the lores run with SPRxCTL = 0x0001 (SH2 = 1, 140 ns): X(R0c) - X(R1) = 2 output pixels for every column on every line");
    g.expect_if(pre_ctl, okc2,
        "S7-CONTROL-V: the comparator of S7 for SH0 (V(R) = V(R1s)) applied to R0sc, the SHRES run with SPRxCTL = 0x0001 (SH2 = 1, 140 ns, four 35-ns pixels), for every column on every line");
}

}  // namespace s3
