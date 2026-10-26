// s7_cprops.h -- the register cases of C1, C3, C5-C11 (SPECIFICATION.md section 2).
#pragma once

#include "s7_ceval.h"

namespace s7 {

// Blitter writes in (t0, t1) (every write that is not the program's, wherever it lands; `reg` names the proposition's destination); mode 0: (address << 16) | data, 1: address only, 2: data only.
inline Obs wr_obs(const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<Region>& reg, int mode = 0) {
    Obs o;
    for (size_t i = acc_first_after(*x.acc, t0); i < x.acc->size() && (*x.acc)[i].t < t1; ++i) {
        const Acc& a = (*x.acc)[i];
        if (!a.write || is_cpu(a.addr)) continue;   // the blitter is the only writer of Chip RAM besides the program
        o.push_back(mode == 0 ? (uint64_t(a.addr) << 16 | a.data) : mode == 1 ? uint64_t(a.addr) << 16 : uint64_t(a.data));
    }
    return o;
}
inline size_t rd_count(const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<Region>& reg) {
    size_t n = 0;
    for (size_t i = acc_first_after(*x.acc, t0); i < x.acc->size() && (*x.acc)[i].t < t1; ++i) {
        const Acc& a = (*x.acc)[i];
        if (!a.write && !a.refresh && in_any(reg, a.addr)) ++n;
    }
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

// Walking values: 1 << k and the complement of 1 << k, within `mask`, for k in lo..hi.
inline std::vector<uint16_t> walk(int lo, int hi, uint16_t mask = 0xFFFF) {
    std::vector<uint16_t> v;
    for (int k = lo; k <= hi; ++k) v.push_back(uint16_t((1u << k) & mask));
    for (int k = lo; k <= hi; ++k) v.push_back(uint16_t(~(1u << k) & mask));
    return v;
}

// A mutation run keeps the first case only (SPECIFICATION.md 6.4); this puts a case in front whose written value changes the
// observation, so that the contrasts of the propositions have something to move.
inline void informative_first(std::vector<Variant>& v) {
    auto it = std::find_if(v.begin(), v.end(), [](const Variant& x) { return x.W != x.U; });
    if (it != v.end() && it != v.begin()) std::iter_swap(v.begin(), it);
}

// ---- C1: DSKLEN -------------------------------------------------------------------
// The 32 walking values; DSKBYTR bit 13 mirrors bit 14 of DSKLEN, the only bit of DSKLEN observed.
inline Prop prop_c1() {
    Prop p; p.id = "C1"; p.batch = 32;
    for (uint16_t v : walk(0, 15))
        p.variants.push_back(reg_variant("DSKLEN", DSKLEN, v, DSKLEN, 0x0000, DSKBYTR, [](uint16_t r) { return uint64_t((r >> 13) & 1); }, 0, uint64_t((v >> 14) & 1)));
    informative_first(p.variants);
    return p;
}

// ---- C3: VPOSW ---------------------------------------------------------------------
inline Prop prop_c3() {
    Prop p; p.id = "C3"; p.batch = 1;
    p.variants.push_back(reg_variant("VPOSW", VPOSW, 0x8000, VPOSW, 0x0000, VPOSR, [](uint16_t v) { return uint64_t((v >> 15) & 1); }, 0, 1));
    return p;
}

// ---- C5: POTGO ---------------------------------------------------------------------
// The 16 walking values of bits 15-8 (OUTRY DATRY OUTRX DATRX OUTLY DATLY OUTLX DATLX). V0 = $FF00 drives all four pins high.
// A pin that the value drives (its OUT bit set) takes the level of its DAT bit; the observation is POTINP at the places of the
// DAT bits of the driven pins. A pin that the value does not drive is not observed (its level is the controller's).
inline Prop prop_c5() {
    Prop p; p.id = "C5"; p.batch = 16;
    for (uint16_t v : walk(8, 15, 0xFF00)) {
        uint16_t mask = 0, w = 0;
        for (int pin = 0; pin < 4; ++pin) {
            const int out = 15 - 2 * pin, dat = 14 - 2 * pin;
            if (v & (1u << out)) { mask = uint16_t(mask | (1u << dat)); w = uint16_t(w | (v & (1u << dat))); }
        }
        Variant x = reg_variant("POTGO", POTGO, v, POTGO, 0xFF00, POTINP, [mask](uint16_t r) { return uint64_t(r & mask); }, mask, w);
        x.after = [](Script& s) { s.w16(POTGO, 0x0000); };
        p.variants.push_back(x);
    }
    informative_first(p.variants);
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

// BLTCON0: the value $01FF (USED, LF = $FF) against $0100 (USED, LF = $00), one case. BLTCON0L: the 32 walking values.
// BLTCON0L writes LF7-LF0 of BLTCON0 and no other bit (AA p.13); V0 sets BLTCON0 = $0F00 (USEA-USED, ASH = 0, LF = $00).
// The sources hold A = $F0F0, B = $CCCC, C = $AAAA, so bit i of the low byte of D is bit i of LF (the minterm of the bits i of
// A, B, C), and D = LF in both bytes. The observation is the one word written.
inline Variant bltcon0l_variant(uint16_t v) {
    Variant x;
    x.name = "BLTCON0L"; x.reg = BLTCON0L; x.v1 = v; x.blitter = true; x.wide = true;
    x.chan = {{kDst, kDst + 2}, {kAsrc, kAsrc + 2}, {kBsrc, kBsrc + 2}, {kCsrc, kCsrc + 2}};
    x.v0 = [](Script& s) { s.w16(BLTCON0, 0x0F00); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTAMOD, 0); s.w16(BLTBMOD, 0); s.w16(BLTCMOD, 0); s.w16(BLTDMOD, 0); };
    x.v1cpu = [v](Script& s) { s.w16(BLTCON0L, v); };
    x.obs = [](Script& s) {
        s.w32(BLTAPTH, kAsrc); s.w32(BLTBPTH, kBsrc); s.w32(BLTCPTH, kCsrc); s.w32(BLTDPTH, kDst);
        blit_start(s, 0x0041);
        s.blitdone();
        return std::vector<int>{};
    };
    auto chan = x.chan;
    x.ex = [chan](const Ctx& cx, uint64_t t0, uint64_t t1, const std::vector<int>&) { return wr_obs(cx, t0, t1, chan, 0); };
    x.U = wr_expect(kDst, {0x0000});
    x.W = wr_expect(kDst, {uint16_t((v & 0xFF) * 0x0101)});
    x.pc6 = [chan](const Ctx& cx, uint64_t t0, uint64_t t1, const Obs&) {
        return wr_obs(cx, t0, t1, chan).size() == 1 && rd_count(cx, t0, t1, {{kAsrc, kAsrc + 2}}) == 1 &&
               rd_count(cx, t0, t1, {{kBsrc, kBsrc + 2}}) == 1 && rd_count(cx, t0, t1, {{kCsrc, kCsrc + 2}}) == 1;
    };
    return x;
}

inline Prop prop_c7() {
    Prop p; p.id = "C7"; p.batch = 16;
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    poke_words(p.pokes, kAsrc, {0xF0F0});
    poke_words(p.pokes, kBsrc, {0xCCCC});
    poke_words(p.pokes, kCsrc, {0xAAAA});
    auto v0 = [](Script& s) { s.w16(BLTCON0, 0x0100); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0); };
    auto obs = [](Script& s) { s.w32(BLTDPTH, kDst); blit_start(s, (mut("C7-PC6") && g_frame_b) ? 0x0043 : 0x0044); };   // the contrast: three words
    auto chan = reg_dst(kDst, kDst + 8);
    Obs u = wr_expect(kDst, {0, 0, 0, 0}), w = wr_expect(kDst, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF});
    Variant a = blit_variant("BLTCON0", BLTCON0, 0x01FF, v0, obs, chan, u, w, 0);
    a.pc6 = [chan](const Ctx& x, uint64_t t0, uint64_t t1, const Obs&) { return wr_obs(x, t0, t1, chan).size() == 4; };
    p.variants.push_back(a);
    for (uint16_t v : walk(0, 15)) p.variants.push_back(bltcon0l_variant(v));
    return p;
}

// ---- C8: BLTCON1 --------------------------------------------------------------------
inline Prop prop_c8() {
    Prop p; p.id = "C8"; p.batch = 1;
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
// The 32 walking values for each register. The source A holds $FFFF in all three words, so the first (BLTAFWM) or the last
// (BLTALWM) word written is the mask itself.
inline Prop prop_c9() {
    Prop p; p.id = "C9"; p.batch = 16;
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { set_masks(s); s.w16(BLTCON0, 0x09F0); s.w16(BLTCON1, 0); s.w16(BLTAMOD, 0); s.w16(BLTDMOD, 0); };
    auto obs = [](Script& s) { s.w32(BLTAPTH, kAsrc); s.w32(BLTDPTH, kDst); blit_start(s, (mut("C9-PC6") && g_frame_b) ? 0x0044 : 0x0043); };   // the contrast: four words
    poke_words(p.pokes, kAsrc, {0xFFFF, 0xFFFF, 0xFFFF});
    auto chan = std::vector<Region>{{kDst, kDst + 6}, {kAsrc, kAsrc + 6}};
    auto wchan = reg_dst(kDst, kDst + 6);
    Obs u = wr_expect(kDst, {0xFFFF, 0xFFFF, 0xFFFF});
    for (uint16_t reg : {BLTAFWM, BLTALWM})
        for (uint16_t v : walk(0, 15)) {
            Variant x = blit_variant(reg == BLTAFWM ? "BLTAFWM" : "BLTALWM", reg, v, v0, obs, chan, u,
                                     reg == BLTAFWM ? wr_expect(kDst, {v, 0xFFFF, 0xFFFF}) : wr_expect(kDst, {0xFFFF, 0xFFFF, v}), 0);
            x.pc6 = [wchan](const Ctx& cx, uint64_t t0, uint64_t t1, const Obs&) {
                return wr_obs(cx, t0, t1, wchan).size() == 3 && rd_count(cx, t0, t1, {{kAsrc, kAsrc + 6}}) == 3;
            };
            p.variants.push_back(x);
        }
    return p;
}

// ---- C10: BLTSIZE and BLTSIZH --------------------------------------------------------
// BLTSIZE: w (bits 5-0) and h (bits 15-6), walked field by field with the other field at 1 (the pages leave the value 0 of a
// field open): w = 1 << k and w = 63 minus 1 << k, h = 1 << k and h = 1023 minus 1 << k. BLTSIZH: w = 1 << k and w = 2047
// minus 1 << k, with BLTSIZV = 1. The blit of w x h words writes w x h words at DST + 2 i, $FFFF each.
inline Prop prop_c10() {
    Prop p; p.id = "C10"; p.batch = 8;
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { s.w16(BLTCON0, 0x01FF); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0); s.w32(BLTDPTH, kDst); };
    auto none = [](Script&) {};
    std::vector<unsigned> sizes;   // (h << 6) | w
    for (unsigned k = 0; k < 6; ++k) sizes.push_back((1u << 6) | (1u << k));
    for (unsigned k = 0; k < 6; ++k) sizes.push_back((1u << 6) | (63u ^ (1u << k)));
    for (unsigned k = 0; k < 10; ++k) sizes.push_back(((1u << k) << 6) | 1u);
    for (unsigned k = 0; k < 10; ++k) sizes.push_back(((1023u ^ (1u << k)) << 6) | 1u);
    for (unsigned sz : sizes) {
        const unsigned count = (sz & 63u) * (sz >> 6);
        Variant x = blit_variant("BLTSIZE", BLTSIZE, uint16_t(sz), v0, none, reg_dst(kDst, kDst + 2 * count), {}, wr_expect(kDst, std::vector<uint16_t>(count, 0xFFFF)), 0);
        x.cal1_mark_before_v1 = true; x.b_window_from_start = true;
        p.variants.push_back(x);
    }
    for (uint16_t w : walk(0, 10, 0x07FF)) {
        Variant x = blit_variant("BLTSIZH", BLTSIZH, w, [v0](Script& s) { v0(s); s.w16(BLTSIZV, 1); }, none, reg_dst(kDst, kDst + 2 * w), {}, wr_expect(kDst, std::vector<uint16_t>(w, 0xFFFF)), 0);
        x.cal1_mark_before_v1 = true; x.b_window_from_start = true;
        p.variants.push_back(x);
    }
    return p;
}

// ---- C11: BLTSIZV --------------------------------------------------------------------
// The heights 1, 2, 4, ..., 128 (bits 0-7 of the 15-bit height, walked one at a time); the height of V0 is 2. The blit is one
// word wide, so it writes as many words as its height.
inline Prop prop_c11() {
    Prop p; p.id = "C11"; p.batch = 8;
    p.dmacon_once = 0x8240; p.dma_enable_bit = 0x0040;
    auto v0 = [](Script& s) { s.w16(BLTCON0, 0x01FF); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0); s.w32(BLTDPTH, kDst); s.w16(BLTSIZV, 2); };
    auto obs = [](Script& s) { s.w32(BLTDPTH, kDst); s.w16(BLTSIZH, 0x0001); };
    for (unsigned k = 0; k < 8; ++k) {
        auto chan = reg_dst(kDst, kDst + 2 * (1u << k) + 4);
        Variant v = blit_variant("BLTSIZV", BLTSIZV, uint16_t(1u << k), v0, obs, chan, {2}, {uint64_t(1u << k)}, 0);
        v.ex = [chan](const Ctx& x, uint64_t t0, uint64_t t1, const std::vector<int>&) { return Obs{wr_obs(x, t0, t1, chan).size()}; };
        v.wide = false;
        p.variants.push_back(v);
    }
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
    // the observation is the written bit V7 (R & $80, VHPOSR bit 15): 1 when set; 0 when clear; 3 when clear and bits 15-8
    // are below $40, the frame anchor of the calibration read before the write
    v.ex = [](const Ctx& x, uint64_t, uint64_t, const std::vector<int>& idx) {
        unsigned hi = (logv(*x.cap, idx[0]) >> 8) & 0xFF;
        return Obs{(hi & 0x80) ? 1u : hi < 0x40 ? 3u : 0u};
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
// The 32 walking values. JOYTEST writes the counters Y7-Y2 and X7-X2 of JOY0DAT and JOY1DAT (bits 15-10 and 7-2); the other
// bits are the state of the input pairs and are masked. The observation is each register & $FCFC, twice.
inline Variant joytest_variant(uint16_t value) {
    Variant v;
    v.name = "JOYTEST"; v.reg = JOYTEST; v.v1 = value;
    auto marks = std::make_shared<std::vector<int>>();
    v.inject_marks = marks;
    v.v0 = [](Script& s) { s.w16(JOYTEST, 0x0000); };
    v.v1cpu = [value](Script& s) { s.w16(JOYTEST, value); };
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
    const uint64_t w = value & 0xFCFC;
    v.U = {0, 0, 0, 0}; v.W = {w, w, w, w};
    v.pc6 = [](const Ctx&, uint64_t, uint64_t, const Obs& o) { return o.size() == 4 && o[0] == o[2] && o[1] == o[3]; };
    return v;
}

inline Prop prop_c6() {
    Prop p; p.id = "C6"; p.pcx = "PC7"; p.mouse_contrast = true; p.batch = 16;
    for (uint16_t value : walk(0, 15)) p.variants.push_back(joytest_variant(value));
    informative_first(p.variants);
    return p;
}

// ---- C2: DSKPTH and DSKPTL -----------------------------------------------------------
// DSKPTH: the 32 walking values; the first disk read is at ((V1 & $1F) << 16) | (low word of Rd), the high 5 bits of the pointer
// (bits 15-5 of the register are not part of it). DSKPTL: the 32 walking values; the first disk read is at (high word of Rd) |
// (V1 & $FFFE), the low 15 bits (bit 0 is not part of it). V0 writes DSKPTH/DSKPTL = Rd. The step 8 sequence reads at most 4 words.
inline Variant dskpt_variant(uint16_t reg, uint16_t value) {
    const uint32_t wexp = reg == DSKPTH ? ((uint32_t(value & 0x1F) << 16) | (kRd & 0xFFFF)) : ((kRd & 0xFFFF0000u) | (value & 0xFFFE));
    Variant v;
    v.name = reg == DSKPTH ? "DSKPTH" : "DSKPTL"; v.reg = reg; v.v1 = value; v.blitter = true;
    std::vector<Region> chan = {{kRd, kRd + 8}, {wexp, wexp + 8}};
    v.chan = chan;
    v.v0 = [](Script& s) { s.w32(DSKPTH, kRd); };
    v.v1cpu = [=](Script& s) { s.w16(reg, value); };
    v.obs = [](Script& s) {
        s.w16(DSKLEN, 0x4000); s.w16(DSKLEN, 0xC004);
        if (!(mut("C2-PC6") && g_frame_b)) s.w16(DSKLEN, 0xC004);   // the contrast, in frame B: DMAEN of DSKLEN is written once only
        int i1 = s.rd(DSKBYTR);
        int i2 = s.pollb(INTREQR, 0x0002, 220);
        s.w16r(DSKLEN, 0x4000, INTREQ, 0x0002);
        if (!(mut("C2-PCR") && g_frame_b)) s.lines(kC2Lines);   // the contrast of PCR, in frame B: the LINES wait that closes the window is left out
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
}

inline Prop prop_c2() {
    Prop p; p.id = "C2"; p.batch = 5;
    p.dmacon_once = 0x8210; p.dma_enable_bit = 0x0010; p.adkcon_once = 0x9100; p.refresh_pc = true;
    for (uint16_t reg : {DSKPTH, DSKPTL})
        for (uint16_t value : walk(0, 15)) p.variants.push_back(dskpt_variant(reg, value));
    return p;
}
} // namespace s7
