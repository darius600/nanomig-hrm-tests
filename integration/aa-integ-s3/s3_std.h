// s3_std.h -- the standard display D-STD, the palettes, the Copper list helpers
// and the sprite lists of SPECIFICATION.md 2.4 - 2.9.

#pragma once

#include "s3_run.h"

namespace s3 {

enum class Pal { None, On, Spr, Att };

struct Std {
    int bpu = 1;                    // 0..8
    uint16_t bplcon0_extra = 0;     // e.g. UHRES
    Pal pal = Pal::None;
    uint16_t bplcon3 = 0x0C00;
    uint16_t bplcon4 = 0x0011;
    uint16_t fmode = 0x0000;
    uint16_t dmacon = 0x8380;
    bool lf = false;                // long frame (2.9)
    uint16_t beamcon0 = 0;          // 0: the default of 2.1 / 2.9
    uint16_t htotal = 0x00E2;
    uint16_t vtotal = 0x05FF;
    bool write_bplcon4 = true, write_fmode = true;
    bool sprctl_init = true;        // SPR0CTL .. SPR7CTL written once, value 0
    uint16_t diwstrt = 0x2C81, diwstop = 0x2CC1;
    uint16_t ddfstrt = 0x0038, ddfstop = 0x00D0;
    std::vector<Wr> extra;          // writes after the display registers, before the palette
    std::vector<Wr> late;           // writes after the palette, before COP1LC
};

inline uint16_t bplcon0_for(int bpu) { return bpu >= 8 ? 0x0010 : uint16_t(bpu << 12); }

inline void colour_writes(std::vector<Wr>& v, int bank, int first, int last, uint16_t val, uint16_t bplcon3_base) {
    v.push_back({R_BPLCON3, uint16_t(bplcon3_base | (bank << 13))});
    for (int i = first; i <= last; ++i) v.push_back({uint16_t(0x180 + 2 * i), val});
}

inline std::vector<Wr> std_setup(const Std& o) {
    std::vector<Wr> v;
    if (o.lf) {
        v.push_back({R_HTOTAL, o.htotal});
        v.push_back({R_VTOTAL, o.vtotal});
        v.push_back({R_BEAMCON0, o.beamcon0 ? o.beamcon0 : uint16_t(0x08A0)});
    } else {
        v.push_back({R_BEAMCON0, o.beamcon0 ? o.beamcon0 : uint16_t(0x0020)});
    }
    v.push_back({R_DIWSTRT, o.diwstrt});
    v.push_back({R_DIWSTOP, o.diwstop});
    v.push_back({R_DDFSTRT, o.ddfstrt});
    v.push_back({R_DDFSTOP, o.ddfstop});
    v.push_back({R_BPLCON0, uint16_t(bplcon0_for(o.bpu) | o.bplcon0_extra)});
    v.push_back({R_BPLCON1, 0x0000});
    v.push_back({R_BPLCON2, 0x0000});
    v.push_back({R_BPLCON3, o.bplcon3});
    if (o.write_bplcon4) v.push_back({R_BPLCON4, o.bplcon4});
    if (o.write_fmode) v.push_back({R_FMODE, o.fmode});
    v.push_back({R_BPL1MOD, 0x0000});
    v.push_back({R_BPL2MOD, 0x0000});
    if (o.sprctl_init)
        for (int x = 0; x < 8; ++x) v.push_back({R_SPRxCTL(x), 0x0000});
    for (const auto& w : o.extra) v.push_back(w);
    switch (o.pal) {
    case Pal::On:
        colour_writes(v, 0, 1, 31, kColON, o.bplcon3);
        for (int b = 1; b < 8; ++b) colour_writes(v, b, 0, 31, kColON, o.bplcon3);
        v.push_back({R_BPLCON3, o.bplcon3});
        break;
    case Pal::Spr:
        colour_writes(v, 0, 1, 15, kColON, o.bplcon3);
        colour_writes(v, 0, 16, 31, kColS, o.bplcon3);
        break;
    case Pal::Att:
        colour_writes(v, 0, 16, 31, 0x0F0, o.bplcon3);          // cE
        colour_writes(v, 1, 0, 15, 0xF0F, o.bplcon3);           // 32-47: cO
        v.push_back({R_BPLCON3, o.bplcon3});
        break;
    case Pal::None: break;
    }
    for (const auto& w : o.late) v.push_back(w);
    v.push_back({R_COP1LCH, uint16_t(kCL >> 16)});
    v.push_back({R_COP1LCL, uint16_t(kCL & 0xFFFF)});
    v.push_back({R_COPJMP1, 0x0000});
    v.push_back({R_DMACON, o.dmacon});
    return v;
}

// ---- Copper vertical-blank block helpers ----------------------------------------
inline std::vector<Wr> bpl_ptrs(const std::array<uint32_t, 9>& a, int n) {   // a[1..8]
    std::vector<Wr> v;
    for (int x = 1; x <= n; ++x) push_ptr(v, R_BPLxPTH(x), R_BPLxPTL(x), a[x]);
    return v;
}
inline std::vector<Wr> spr_ptrs(const std::array<uint32_t, 8>& a) {
    std::vector<Wr> v;
    for (int x = 0; x < 8; ++x) push_ptr(v, R_SPRxPTH(x), R_SPRxPTL(x), a[x]);
    return v;
}
inline std::array<uint32_t, 9> all_z() { std::array<uint32_t, 9> a{}; a.fill(kZ); return a; }
inline std::array<uint32_t, 8> own_lists() { std::array<uint32_t, 8> a{}; for (int x = 0; x < 8; ++x) a[x] = L_x(x); return a; }

// ---- sprite lists (2.7) ------------------------------------------------------------
struct SprRow { uint16_t data, datb; };

// start/stop 10 bits, sh = SH10-SH0 in 35 ns units
inline std::vector<uint16_t> spr_list(int start, int stop, int sh, bool att, const std::vector<SprRow>& rows) {
    std::vector<uint16_t> w;
    w.push_back(uint16_t(((start & 0xFF) << 8) | ((sh >> 3) & 0xFF)));
    w.push_back(uint16_t(((stop & 0xFF) << 8) | (att ? 0x80 : 0) | (((start >> 9) & 1) << 6) |
                         (((stop >> 9) & 1) << 5) | (((sh >> 1) & 1) << 4) | ((sh & 1) << 3) |
                         (((start >> 8) & 1) << 2) | (((stop >> 8) & 1) << 1) | ((sh >> 2) & 1)));
    for (const auto& r : rows) { w.push_back(r.data); w.push_back(r.datb); }
    w.push_back(0); w.push_back(0);
    return w;
}
inline int sh_of_lores(int X) { return 4 * X; }

}  // namespace s3
