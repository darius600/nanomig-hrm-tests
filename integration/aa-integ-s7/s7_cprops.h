// s7_cprops.h -- the register cases of C1, C3, C5-C11 (SPECIFICATION.md section 2).
#pragma once

#include "s7_ceval.h"

namespace s7 {

// Blitter writes in (t0, t1) (every write that is not the program's, wherever it lands; `reg` names the proposition's destination); mode 0: (address << 16) | data, 1: address only, 2: data only.
inline Obs wr_obs(const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<Region>& reg, int mode = 0) {
    Obs o;
    for (auto& a : *x.acc) {
        if (!a.write || a.t <= t0 || a.t >= t1 || is_cpu(a.addr)) continue;   // the blitter is the only writer of Chip RAM besides the program
        o.push_back(mode == 0 ? (uint64_t(a.addr) << 16 | a.data) : mode == 1 ? uint64_t(a.addr) << 16 : uint64_t(a.data));
    }
    return o;
}
inline size_t rd_count(const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<Region>& reg) {
    size_t n = 0;
    for (auto& a : *x.acc) if (!a.write && !a.refresh && a.t > t0 && a.t < t1 && in_any(reg, a.addr)) ++n;
    return n;
}

inline Obs wr_expect(uint32_t base, std::vector<uint16_t> data) {
    Obs o;
    for (size_t i = 0; i < data.size(); ++i) o.push_back(uint64_t(base + 2 * i) << 16 | data[i]);
    return o;
}

// Every proposition writes the pointer registers of the blit it observes, then starts it.
inline void blit_start(Script& s, uint16_t size) { s.w16(BLTSIZE, size); }

inline Variant reg_variant(const std::string& name, uint16_t reg, uint16_t v1, uint16_t v0reg, uint16_t v0val, uint16_t obsreg,
                           std::function<uint64_t(uint16_t)> f, uint64_t u, uint64_t w) {
    Variant v;
    v.name = name; v.reg = reg; v.v1 = v1;
    v.v0 = [=](Script& s) { s.w16(v0reg, v0val); };
    v.v1cpu = [=](Script& s) { s.w16(reg, v1); };
    v.obs = [=](Script& s) { return std::vector<int>{s.rd(obsreg)}; };
    v.ex = [=](const Ctx& x, uint64_t, uint64_t, const std::vector<int>& idx) { return Obs{f(logv(*x.cap, idx[0]))}; };
    v.U = {u}; v.W = {w};
    return v;
}

// ---- C1: DSKLEN -------------------------------------------------------------------
inline Prop prop_c1() {
    Prop p; p.id = "C1";
    p.variants.push_back(reg_variant("DSKLEN", DSKLEN, 0x4000, DSKLEN, 0x0000, DSKBYTR, [](uint16_t v) { return uint64_t((v >> 13) & 1); }, 0, 1));
    return p;
}

// ---- C3: VPOSW ---------------------------------------------------------------------
inline Prop prop_c3() {
    Prop p; p.id = "C3";
    p.variants.push_back(reg_variant("VPOSW", VPOSW, 0x8000, VPOSW, 0x0000, VPOSR, [](uint16_t v) { return uint64_t((v >> 15) & 1); }, 0, 1));
    return p;
}

// ---- C5: POTGO ---------------------------------------------------------------------
inline Prop prop_c5() {
    Prop p; p.id = "C5";
    p.variants.push_back(reg_variant("POTGO", POTGO, 0xAA00, POTGO, 0xFF00, POTINP, [](uint16_t v) { return uint64_t(v & 0x5500); }, 0x5500, 0x0000));
    p.variants[0].after = [](Script& s) { s.w16(POTGO, 0x0000); };
    return p;
}

// ---- C7: BLTCON0 and BLTCON0L -------------------------------------------------------
inline std::vector<Region> reg_dst(uint32_t lo, uint32_t hi) { return {{lo, hi}}; }

inline Variant blit_variant(const std::string& name, uint16_t reg, uint16_t v1, std::function<void(Script&)> v0,
                            std::function<void(Script&)> obs_start, std::vector<Region> chan, Obs u, Obs w, int mode) {
    Variant v;
    v.name = name; v.reg = reg; v.v1 = v1; v.blitter = true; v.wide = true; v.chan = chan;
    v.v0 = v0;
    v.v1cpu = [=](Script& s) { s.w16(reg, v1); };
    v.obs = [=](Script& s) { obs_start(s); s.blitdone(); return std::vector<int>{}; };
    v.ex = [=](const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<int>&) { return wr_obs(x, t0, t1, chan, mode); };
    v.U = u; v.W = w;
    return v;
}

inline void set_masks(Script& s) { s.w16(BLTAFWM, 0xFFFF); s.w16(BLTALWM, 0xFFFF); }

inline Prop prop_c7() {
    Prop p; p.id = "C7";
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { s.w16(BLTCON0, 0x0100); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0); };
    auto obs = [](Script& s) { s.w32(BLTDPTH, kDst); blit_start(s, (mut("C7-PC6") && g_frame_b) ? 0x0043 : 0x0044); };   // the contrast: three words
    auto chan = reg_dst(kDst, kDst + 8);
    Obs u = wr_expect(kDst, {0, 0, 0, 0}), w = wr_expect(kDst, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF});
    p.variants.push_back(blit_variant("BLTCON0", BLTCON0, 0x01FF, v0, obs, chan, u, w, 0));
    p.variants.push_back(blit_variant("BLTCON0L", BLTCON0L, 0x00FF, v0, obs, chan, u, w, 0));
    for (auto& v : p.variants) v.pc6 = [chan](const Ctx& x, uint64_t t0, uint64_t t1, const Obs&) { return wr_obs(x, t0, t1, chan).size() == 4; };
    return p;
}

// ---- C8: BLTCON1 --------------------------------------------------------------------
inline Prop prop_c8() {
    Prop p; p.id = "C8";
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { s.w16(BLTCON0, 0x01FF); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0); };
    auto obs = [](Script& s) { s.w32(BLTDPTH, kDst); blit_start(s, (mut("C8-PC6") && g_frame_b) ? 0x0043 : 0x0042); };   // the contrast: three words
    auto chan = reg_dst(kDst - 2, kDst + 4);
    Obs u = {uint64_t(kDst) << 16, uint64_t(kDst + 2) << 16}, w = {uint64_t(kDst) << 16, uint64_t(kDst - 2) << 16};
    p.variants.push_back(blit_variant("BLTCON1", BLTCON1, 0x0002, v0, obs, chan, u, w, 1));
    p.variants[0].pc6 = [chan](const Ctx& x, uint64_t t0, uint64_t t1, const Obs&) { return wr_obs(x, t0, t1, chan).size() == 2; };
    return p;
}

// ---- C9: BLTAFWM and BLTALWM ---------------------------------------------------------
inline Prop prop_c9() {
    Prop p; p.id = "C9";
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { set_masks(s); s.w16(BLTCON0, 0x09F0); s.w16(BLTCON1, 0); s.w16(BLTAMOD, 0); s.w16(BLTDMOD, 0); };
    auto obs = [](Script& s) { s.w32(BLTAPTH, kAsrc); s.w32(BLTDPTH, kDst); blit_start(s, (mut("C9-PC6") && g_frame_b) ? 0x0044 : 0x0043); };   // the contrast: four words
    poke_words(p.pokes, kAsrc, {0xFFFF, 0xFFFF, 0xFFFF});
    auto chan = std::vector<Region>{{kDst, kDst + 6}, {kAsrc, kAsrc + 6}};
    auto wchan = reg_dst(kDst, kDst + 6);
    Obs u = wr_expect(kDst, {0xFFFF, 0xFFFF, 0xFFFF});
    p.variants.push_back(blit_variant("BLTAFWM", BLTAFWM, 0x0F0F, v0, obs, chan, u, wr_expect(kDst, {0x0F0F, 0xFFFF, 0xFFFF}), 0));
    p.variants.push_back(blit_variant("BLTALWM", BLTALWM, 0x0F0F, v0, obs, chan, u, wr_expect(kDst, {0xFFFF, 0xFFFF, 0x0F0F}), 0));
    for (auto& v : p.variants) {
        v.pc6 = [wchan](const Ctx& x, uint64_t t0, uint64_t t1, const Obs&) {
            return wr_obs(x, t0, t1, wchan).size() == 3 && rd_count(x, t0, t1, {{kAsrc, kAsrc + 6}}) == 3;
        };
    }
    return p;
}

// ---- C10: BLTSIZE and BLTSIZH --------------------------------------------------------
inline Prop prop_c10() {
    Prop p; p.id = "C10";
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { s.w16(BLTCON0, 0x01FF); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0); s.w32(BLTDPTH, kDst); };
    auto chan = reg_dst(kDst, kDst + 8);
    Obs u = {}, w = wr_expect(kDst, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF});
    auto none = [](Script&) {};
    p.variants.push_back(blit_variant("BLTSIZE", BLTSIZE, 0x0044, v0, none, chan, u, w, 0));
    p.variants.push_back(blit_variant("BLTSIZH", BLTSIZH, 0x0004, [v0](Script& s) { v0(s); s.w16(BLTSIZV, 1); }, none, chan, u, w, 0));
    for (auto& v : p.variants) { v.cal1_mark_before_v1 = true; v.b_window_from_start = true; }
    return p;
}

// ---- C11: BLTSIZV --------------------------------------------------------------------
inline Prop prop_c11() {
    Prop p; p.id = "C11";
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { s.w16(BLTCON0, 0x01FF); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0); s.w32(BLTDPTH, kDst); s.w16(BLTSIZV, 2); };
    auto obs = [](Script& s) { s.w32(BLTDPTH, kDst); s.w16(BLTSIZH, 0x0001); };
    auto chan = reg_dst(kDst, kDst + 10);
    Variant v = blit_variant("BLTSIZV", BLTSIZV, 5, v0, obs, chan, {2}, {5}, 0);
    v.ex = [chan](const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<int>&) { return Obs{wr_obs(x, t0, t1, chan).size()}; };
    v.wide = false;
    p.variants.push_back(v);
    return p;
}

// ---- C4: VHPOSW ----------------------------------------------------------------------
inline Prop prop_c4() {
    Prop p; p.id = "C4";
    Variant v;
    v.name = "VHPOSW"; v.reg = VHPOSW; v.v1 = 0xC000;
    v.caselist = {0x4041, 0xFFFE, 0x002C, 0xC000, 0xFFFF, 0xFFFE};
    v.obs_replaces_step6 = true;
    v.obs = [](Script& s) { return std::vector<int>{s.waitline(0x41)}; };
    // the class of R (bits 15-8): 0 = {$41, $42}, 1 = {$C0, $C1}, 3 = below $40 (the calibration read before the write)
    v.ex = [](const Ctx& x, uint64_t, uint64_t, const std::vector<int>& idx) {
        unsigned hi = (logv(*x.cap, idx[0]) >> 8) & 0xFF;
        return Obs{hi == 0xC0 || hi == 0xC1 ? 1u : (hi == 0x41 || hi == 0x42) ? 0u : hi < 0x40 ? 3u : 100u + hi};
    };
    v.U = {0}; v.W = {1}; v.U_cal0 = {3}; v.has_ucal0 = true;
    v.cal = [](Script& s, CalA& c1, CalA& c0) {
        s.waitline(0x08);
        c0.ms = s.mark(); c0.idx = {s.rd(VHPOSR)}; c0.me = s.mark();
        if (!mut("C4-PC4")) s.w16(VHPOSW, 0xC000);   // the contrast: V1 is not written
        c1.ms = s.mark(); c1.idx = {s.rd(VHPOSR)}; c1.me = s.mark();
    };
    p.variants.push_back(v);
    return p;
}

// ---- C6: JOYTEST ---------------------------------------------------------------------
inline Prop prop_c6() {
    Prop p; p.id = "C6"; p.pcx = "PC7"; p.mouse_contrast = true;
    Variant v;
    v.name = "JOYTEST"; v.reg = JOYTEST; v.v1 = 0xFCFC;
    auto marks = std::make_shared<std::vector<int>>();
    v.inject_marks = marks;
    v.v0 = [](Script& s) { s.w16(JOYTEST, 0x0000); };
    v.v1cpu = [](Script& s) { s.w16(JOYTEST, 0xFCFC); };
    v.obs = [marks](Script& s) {
        std::vector<int> i;
        i.push_back(s.rd(JOY0DAT)); i.push_back(s.rd(JOY1DAT));
        const int m = s.mark();
        if (g_frame_b) marks->push_back(m);
        s.lines(1);
        i.push_back(s.rd(JOY0DAT)); i.push_back(s.rd(JOY1DAT));
        return i;
    };
    v.ex = [](const Ctx& x, uint64_t, uint64_t, const std::vector<int>& idx) {
        Obs o;
        for (int i : idx) o.push_back(logv(*x.cap, i) & 0xFCFC);
        return o;
    };
    v.U = {0, 0, 0, 0}; v.W = {0xFCFC, 0xFCFC, 0xFCFC, 0xFCFC};
    v.pc6 = [](const Ctx&, uint64_t, uint64_t, const Obs& o) { return o.size() == 4 && o[0] == o[2] && o[1] == o[3]; };
    p.variants.push_back(v);
    return p;
}

// ---- C2: DSKPTH and DSKPTL -----------------------------------------------------------
inline Prop prop_c2() {
    Prop p; p.id = "C2";
    p.dmacon_once = 0x8210; p.dma_enable_bit = 0x0010; p.adkcon_once = 0x9100; p.refresh_pc = true;
    std::vector<Region> chan = {{kRd, kRd + 8}, {kRe, kRe + 8}, {kRe2, kRe2 + 8}};
    auto mk = [&](const std::string& name, uint16_t reg, uint16_t v1, uint32_t wexp) {
        Variant v;
        v.name = name; v.reg = reg; v.v1 = v1; v.blitter = true; v.chan = chan;
        v.v0 = [](Script& s) { s.w32(DSKPTH, kRd); };
        v.v1cpu = [=](Script& s) { s.w16(reg, v1); };
        v.obs = [](Script& s) {
            s.w16(DSKLEN, 0x4000); s.w16(DSKLEN, 0xC004);
            if (!(mut("C2-PC6") && g_frame_b)) s.w16(DSKLEN, 0xC004);   // the contrast, in frame B: DMAEN of DSKLEN is written once only
            int i1 = s.rd(DSKBYTR);
            int i2 = s.pollb(INTREQR, 0x0002, 400);
            s.w16r(DSKLEN, 0x4000, INTREQ, 0x0002);
            return std::vector<int>{i1, i2};
        };
        auto first = [chan](const Ctx& x, uint64_t t0, uint64_t t1) {
            Window w = classify(*x.acc, t0, t1, chan, copper_regions());
            std::vector<Acc> all = w.chan;
            all.insert(all.end(), w.stray.begin(), w.stray.end());
            std::sort(all.begin(), all.end(), [](const Acc& a, const Acc& b) { return a.t < b.t; });
            return all;
        };
        v.ex = [first](const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<int>&) {
            auto all = first(x, t0, t1);
            return all.empty() ? Obs{} : Obs{all[0].addr};
        };
        v.runs_ok = [first](const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<int>& idx) {
            bool dmaon = (logv(*x.cap, idx[0]) & 0x4000) != 0;
            return !dmaon || !first(x, t0, t1).empty();
        };
        v.run_pre = [](const Cap& c, const std::vector<int>& idx) { return (logv(c, idx[0]) & 0x4000) != 0; };
        v.U = {kRd}; v.W = {wexp};
        return v;
    };
    p.variants.push_back(mk("DSKPTH", DSKPTH, uint16_t(kRe >> 16), kRe));
    p.variants.push_back(mk("DSKPTL", DSKPTL, uint16_t(kRe2 & 0xFFFF), kRe2));
    return p;
}

} // namespace s7
