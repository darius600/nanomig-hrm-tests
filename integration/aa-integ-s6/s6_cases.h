// s6_cases.h -- the cases of SPECIFICATION.md 4 to 6 and the contrasting variants of their preconditions.
#pragma once
#include "s6_base.h"

namespace s6 {

constexpr int kMVstart0 = 0x50, kMPitch = 10;

// The SHRES marker (SPECIFICATION.md 3.6): beam lines $38 and $39 show kMarker, every later line colour(0).
inline void add_shres_marker(Case& c) {
    append(c.cop_extra, cop_wait(0x38, 0x07));
    append(c.cop_extra, cop_move(R_COLOR00, kMarker));
    append(c.cop_extra, cop_wait(0x3A, 0x07));
    append(c.cop_extra, cop_move(R_COLOR00, colour(0)));
}

inline Case make_m(char res, int spres) {
    Case c;
    set_res(c, res);
    c.spres = spres;
    c.name = std::string("M-") + res + "-" + char('0' + (spres >> 1)) + char('0' + (spres & 1));
    for (int k = 0; k < 8; ++k) {
        Sprite& s = c.spr[k];
        s.hstart = 0xA0; s.vstart = kMVstart0 + kMPitch * k; s.vstop = s.vstart + 6; s.att = false;
        for (int j = 0; j < 3; ++j) s.lines.push_back(line_all(0xFFFF, 0xFFFF, 1));
        for (int j = 0; j < 3; ++j) s.lines.push_back(line_all(0xFFFF, 0xAAAA, 1));
    }
    c.c5_px = 16; c.c5_ns = pixel_ns(res, spres);
    if (res == 'S') add_shres_marker(c);
    return c;
}

// W-<SPAGEM><SPR32>: wide sprites on a LORES screen, SPRES = 00.
inline Case make_w(int idx) {
    Case c;
    set_res(c, 'L');
    c.spres = 0;
    c.name = std::string("W-") + char('0' + (idx >> 1)) + char('0' + (idx & 1));
    static const uint16_t fm[4] = {0x0000, 0x0004, 0x0008, 0x000C};
    static const int nw[4] = {1, 2, 2, 4};
    c.fmode = fm[idx];
    c.n = nw[idx];
    for (int k = 0; k < 8; ++k) {
        Sprite& s = c.spr[k];
        s.hstart = 0xA0; s.vstart = kMVstart0 + kMPitch * k; s.vstop = s.vstart + 6;
        for (int j = 0; j < 3; ++j) s.lines.push_back(line_all(0xFFFF, 0xFFFF, c.n));
        for (int j = 0; j < 3; ++j) {
            Line l;
            l.a[0] = 0xFFFF;     // the first word of each DATA fetch; every other word is zero
            s.lines.push_back(l);
        }
    }
    c.c5_px = 64; c.c5_ns = 140;      // the widest expected sprite, 128u from L (SPECIFICATION.md P6)
    return c;
}

// P7's table: the 4-bit value of the eight 2-pixel groups, line type A (even beam line) and B (odd).
inline int a_value(bool typeB, int g) {
    static const int A[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    static const int B[8] = {9, 10, 11, 12, 13, 14, 15, 0};
    return typeB ? B[g & 7] : A[g & 7];
}

// A-<res>-<spres>: four attached pairs; AW-<fmode>: the same at the wider fetches.
inline Case make_a_common(char res, int spres, int fmode_idx, bool att) {
    Case c;
    set_res(c, res);
    c.spres = spres;
    static const uint16_t fm[4] = {0x0000, 0x0004, 0x0008, 0x000C};
    static const int nw[4] = {1, 2, 2, 4};
    c.fmode = fm[fmode_idx];
    c.n = nw[fmode_idx];
    for (int p = 0; p < 4; ++p) {
        for (int e = 0; e < 2; ++e) {
            Sprite& s = c.spr[2 * p + e];
            s.hstart = 0xA0; s.vstart = 0x50 + 12 * p; s.vstop = s.vstart + 8;
            s.att = (e == 1) && att;
            for (int j = 0; j < 8; ++j) {
                bool B = (j & 1) != 0;
                uint16_t da, db;
                if (e == 0) { da = 0xCCCC; db = 0x3C3C; }
                else { da = 0x03FC; db = B ? 0xFFFC : 0x0003; }
                s.lines.push_back(line_all(da, db, c.n));
            }
        }
    }
    c.c5_px = 16 * c.n; c.c5_ns = pixel_ns(res, spres);
    if (res == 'S') add_shres_marker(c);
    c.bands_expected = 4;
    return c;
}

inline Case make_a(char res, int spres, bool att = true) {
    Case c = make_a_common(res, spres, 0, att);
    c.name = std::string(att ? "A-" : "AX-") + res + "-" + char('0' + (spres >> 1)) + char('0' + (spres & 1));
    return c;
}

inline Case make_aw(int fmode_idx, bool att = true) {
    Case c = make_a_common('L', 0, fmode_idx, att);
    c.name = std::string(att ? "AW-" : "AWX-") + char('0' + (fmode_idx >> 1)) + char('0' + (fmode_idx & 1));
    return c;
}

// B-N and B-A: sixteen colour banks.
inline Case make_b(bool attached) {
    Case c;
    set_res(c, 'L');
    c.spres = 0;
    c.name = attached ? "B-A" : "B-N";
    c.bands_expected = 16;
    if (!attached) {
        for (int k = 0; k < 8; ++k) {
            Sprite& s = c.spr[k];
            s.hstart = 0x90 + 24 * k; s.vstart = 0x40; s.vstop = 0xC0; s.att = false;
            for (int j = 0; j < 128; ++j)
                s.lines.push_back(j % 8 == 0 ? line_all(0, 0, 1) : line_all(0xF0F0, 0x0FF0, 1));
        }
    } else {
        for (int p = 0; p < 4; ++p)
            for (int e = 0; e < 2; ++e) {
                Sprite& s = c.spr[2 * p + e];
                s.hstart = 0x90 + 40 * p; s.vstart = 0x40; s.vstop = 0xC0; s.att = (e == 1);
                int t = 0;
                for (int j = 0; j < 128; ++j) {
                    if (j % 8 == 0) { s.lines.push_back(line_all(0, 0, 1)); t = 0; continue; }
                    bool B = (t & 1) != 0; ++t;
                    uint16_t da, db;
                    if (e == 0) { da = 0xCCCC; db = 0x3C3C; } else { da = 0x03FC; db = B ? 0xFFFC : 0x0003; }
                    s.lines.push_back(line_all(da, db, 1));
                }
            }
    }
    for (int b = 0; b < 16; ++b) {
        append(c.cop_extra, cop_wait(0x40 + 8 * b, 0x07));
        append(c.cop_extra, cop_move(R_BPLCON4, uint16_t((b << 4) | (15 - b))));
    }
    c.c5_px = 16; c.c5_ns = 140;
    return c;
}

// R: BPLCON4 never written.
inline Case make_r() {
    Case c;
    set_res(c, 'L');
    c.spres = 0;
    c.name = "R";
    c.bplcon4.reset();
    c.bands_expected = 1;
    for (int k = 0; k < 8; ++k) {
        Sprite& s = c.spr[k];
        s.hstart = 0x90 + 24 * k; s.vstart = 0x50; s.vstop = 0x58; s.att = false;
        for (int j = 0; j < 8; ++j) s.lines.push_back(line_all(0xF0F0, 0x0FF0, 1));
    }
    c.c5_px = 16; c.c5_ns = 140;
    return c;
}

// The contrasting variants of the common preconditions (SPECIFICATION.md 3.8), applied to any case.
enum class Cp { None, C1, C2, C3, C4, C5, C7a, C7b, C7s };

inline void apply_cp(Case& c, Cp k) {
    switch (k) {
    case Cp::C1: c.prog = "prog_nodone.bin"; c.expect_done = false; break;
    case Cp::C2: c.prog = "prog_vbl.bin"; break;
    case Cp::C3: c.bplcon0 = c.res == 'H' ? 0x1200 : 0x9200; break;
    case Cp::C4: c.dmacon = 0x8380; break;
    case Cp::C5: c.diwstrt = 0x2CB0; break;
    case Cp::C7a: c.calib_bytes = c.spr[0].vstart < 0x50 ? 800 : 1280; break;   // calibration rows end above the first sprite line
    case Cp::C7b: {
        std::vector<uint16_t> x;
        append(x, cop_move(R_BPLCON1, 0x0000));
        append(x, std::vector<uint16_t>{0x3007, 0xFFFE});
        append(x, cop_move(R_BPLCON1, 0x0033));
        x.insert(x.end(), c.cop_extra.begin(), c.cop_extra.end());
        c.cop_extra = x;
        break;
    }
    case Cp::C7s:
        for (auto& w : c.cop_extra) if (w == 0x3A07) { w = 0x3C07; break; }   // the marker ends two beam lines later
        break;
    default: break;
    }
}

}  // namespace s6
