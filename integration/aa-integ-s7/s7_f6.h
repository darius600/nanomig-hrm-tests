// s7_f6.h -- F6: BPL2DAT-BPL8DAT receive bitplane x's DMA data, which reaches the display as colour-index bit x-1
// (SPECIFICATION.md section 3, F6).
#pragma once

#include "s7_f2.h"

namespace s7 {

constexpr uint32_t kList6 = 0x090400;
inline uint32_t plane_base(int x) { return 0x180000u + 0x4000u * uint32_t(x - 1); }   // x = 1-8, 40 x 256 bytes each

struct Rgb { uint8_t r, g, b; bool operator==(const Rgb& o) const { return r == o.r && g == o.g && b == o.b; } };
inline Rgb palette_colour(int x) {   // the colour of C(x), x = 2..8 -> index 2^(x-1)
    static const Rgb t[9] = {{0, 0, 0}, {0, 0, 0}, {15, 0, 0}, {0, 15, 0}, {0, 0, 15}, {15, 15, 0}, {0, 15, 15}, {15, 0, 15}, {15, 15, 15}};
    return t[x];
}

struct Band { int c = -1; size_t first_line = 0, height = 0, first_col = 0, last_col = 0; bool uniform = true; };
struct FrameSig {
    bool pixels_ok = true;      // every pixel black or one of C2-C8
    bool ok555 = true;          // $555 never appears
    std::vector<Band> bands;
    std::vector<size_t> gaps;
    size_t lines = 0;
};

inline FrameSig analyse_frame(const std::vector<nmh::VideoSample>& v, nmh::Span fr) {
    FrameSig sg;
    std::vector<nmh::VideoSample> sub(v.begin() + long(fr.begin), v.begin() + long(fr.end));
    auto lines = nmh::find_lines(sub);
    sg.lines = lines.size();
    const Rgb k555{5, 5, 5};
    struct L { bool any = false; int colour = -1; bool single = true; size_t first = 0, last = 0; };
    std::vector<L> ls;
    for (auto& sp : lines) {
        L l;
        for (size_t i = sp.begin; i < sp.end; ++i) {
            Rgb p{sub[i].r, sub[i].g, sub[i].b};
            if (p == Rgb{0, 0, 0}) continue;
            if (p == k555) sg.ok555 = false;
            int ci = -1;
            for (int x = 2; x <= 8; ++x) if (p == palette_colour(x)) ci = x;
            if (ci < 0) { sg.pixels_ok = false; ci = 99; }
            if (!l.any) { l.any = true; l.colour = ci; l.first = i - sp.begin; }
            else if (l.colour != ci) l.single = false;
            l.last = i - sp.begin;
        }
        ls.push_back(l);
    }
    size_t i = 0;
    while (i < ls.size()) {
        if (!ls[i].any) { ++i; continue; }
        Band b; b.first_line = i; b.c = ls[i].colour; b.first_col = ls[i].first; b.last_col = ls[i].last;
        size_t j = i;
        while (j < ls.size() && ls[j].any) {
            if (!ls[j].single || ls[j].colour != b.c || ls[j].first != b.first_col || ls[j].last != b.last_col) b.uniform = false;
            ++j;
        }
        b.height = j - i;
        sg.bands.push_back(b);
        i = j;
    }
    for (size_t k = 1; k < sg.bands.size(); ++k) sg.gaps.push_back(sg.bands[k].first_line - (sg.bands[k - 1].first_line + sg.bands[k - 1].height));
    return sg;
}

inline bool frame_ok(const FrameSig& s) {
    if (!s.pixels_ok || !s.ok555 || s.bands.size() != 7) return false;
    for (size_t k = 0; k < 7; ++k) {
        if (s.bands[k].c != int(k) + 2 || !s.bands[k].uniform) return false;
        if (s.bands[k].height != s.bands[0].height) return false;
        if (s.bands[k].first_col != s.bands[0].first_col || s.bands[k].last_col != s.bands[0].last_col) return false;
    }
    for (size_t g = 0; g < s.gaps.size(); ++g) if (s.gaps[g] != s.gaps[0]) return false;
    return true;
}

inline bool same_frame(const FrameSig& a, const FrameSig& b) {
    if (a.bands.size() != b.bands.size() || a.gaps != b.gaps) return false;
    for (size_t k = 0; k < a.bands.size(); ++k)
        if (a.bands[k].c != b.bands[k].c || a.bands[k].first_line != b.bands[k].first_line || a.bands[k].height != b.bands[k].height ||
            a.bands[k].first_col != b.bands[k].first_col || a.bands[k].last_col != b.bands[k].last_col) return false;
    return true;
}

inline void run_f6(Checks& ck, const std::string& sel) {
    const std::string id = "F6";
    FHead hd = f_head(id, sel, 0x8000);
    Script& s = hd.rs.sc;
    hd.rs.want_video = true;
    // plane memory: plane x = 2-8 holds $FFFF in all 20 words of rows 20+8(x-2) .. 23+8(x-2)
    for (int x = 2; x <= 8; ++x) {
        int xr = x;
        if (sel == id) xr = x == 3 ? 4 : x == 4 ? 3 : x;   // contrast: the band rows of planes 3 and 4 swapped
        int r0 = 20 + 8 * (xr - 2);
        for (int r = r0; r < r0 + 4; ++r)
            for (int w = 0; w < 20; ++w) hd.rs.pokes.push_back({plane_base(x) + 40u * uint32_t(r) + 2u * uint32_t(w), 0xFFFF});
    }
    // the Copper list: BPL1PTH/L-BPL8PTH/L = B1-B8
    std::vector<uint16_t> list;
    for (int x = 1; x <= 8; ++x) {
        list.push_back(uint16_t(BPL1PTH + 4 * (x - 1))); list.push_back(uint16_t(plane_base(x) >> 16));
        list.push_back(uint16_t(BPL1PTH + 4 * (x - 1) + 2)); list.push_back(uint16_t(plane_base(x) & 0xFFFF));
    }
    list.push_back(0xFFFF); list.push_back(0xFFFE);
    poke_words(hd.rs.pokes, kList6, list);
    poke_words(hd.rs.pokes, kList6 + 0x100, list);   // the same list at another address: the Copper of the COPPER contrast runs it instead of the list at kList6
    // palette: 8 banks of 32 entries, LOCT = 0
    auto colour_of = [](int idx) -> uint16_t {
        switch (idx) {
            case 0: return 0x000; case 2: return 0xF00; case 4: return 0x0F0; case 8: return 0x00F; case 16: return 0xFF0;
            case 32: return 0x0FF; case 64: return 0xF0F; case 128: return 0xFFF; default: return 0x555;
        }
    };
    for (int b = 0; b < 8; ++b) {
        s.w16(BPLCON3, uint16_t((b << 13) | 0x0C00));
        for (int r = 0; r < 32; ++r) s.w16(uint16_t(COLOR00 + 2 * r), colour_of(32 * b + r));
    }
    s.w16(BPLCON3, 0x0C00);
    s.w16(BPLCON0, sel == id + "-READS" ? 0x7000 : 0x0010);
    s.w16(BPLCON1, 0); s.w16(BPLCON2, 0); s.w16(BPLCON4, 0x0011); s.w16(FMODE, 0);
    s.w16(DIWSTRT, 0x2C81); s.w16(DIWSTOP, 0x2CC1); s.w16(DDFSTRT, 0x0038); s.w16(DDFSTOP, 0x00D0);
    s.w16(BPL1MOD, 0); s.w16(BPL2MOD, 0);
    s.w32(COP1LCH, sel == id + "-COPPER" ? kList6 + 0x100 : kList6);   // contrast: the Copper runs a copy of the list at another address, so the words of the list at kList6 are not read
    s.w16(DMACON, sel == id + "-DMA" ? 0x8200 : 0x8300);
    int i_dma = s.rd(DMACONR);
    if (sel == id + "-DMA") s.w16(DMACON, 0x8300);   // contrast: bitplane DMA is enabled after DMA_CHECK has read DMACONR
    s.vblsync(); s.vblsync();
    int m_ready = s.mark();
    if (sel == id + "-FRAMES") s.vblsync(); else { s.vblsync(); s.vblsync(); s.vblsync(); s.vblsync(); }
    int m_done = s.mark();
    s.w32(COP1LCH, kIdle);
    s.end();
    hd.rs.cycles = 40'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = (logv(c, i_dma) & 0x0300) == 0x0300;
    uint64_t tr = marker(c, m_ready), td = marker(c, m_done);
    auto frames = nmh::find_frames(c.video);
    // the frames that begin after M_ready: the second and the third are graded
    size_t j0 = frames.size();
    for (size_t j = 0; j < frames.size(); ++j) if (c.base + frames[j].begin > tr) { j0 = j; break; }
    bool have = j0 + 2 < frames.size() && c.base + frames[j0 + 2].end <= td + 1 && tr && td;
    bool reads = have, copper = have;
    FrameSig sg[2];
    if (have) {
        for (int k = 0; k < 2; ++k) {
            nmh::Span fr = frames[j0 + 1 + size_t(k)];
            uint64_t fb = c.base + fr.begin, fe = c.base + fr.end;
            for (int x = 1; x <= 8; ++x) {
                bool any = false;
                for (auto& a : acc) if (!a.write && !a.refresh && a.t >= fb && a.t < fe && a.addr >= plane_base(x) && a.addr < plane_base(x) + 10240) { any = true; break; }
                reads = reads && any;
            }
            // the Copper list read in the vertical blank before the frame: every operand word of the list read in the span of the frame before
            nmh::Span pv = frames[j0 + size_t(k)];
            uint64_t pb = c.base + pv.begin, pe = c.base + pv.end;
            for (int w = 0; w < 16; ++w) {
                uint32_t ad = kList6 + 4u * uint32_t(w) + 2;
                bool any = false;
                for (auto& a : acc) if (!a.write && a.t >= pb && a.t < pe && a.addr == ad) { any = true; break; }
                copper = copper && any;
            }
            sg[k] = analyse_frame(c.video, {fr.begin, fr.end});
        }
    }
    bool ok = have && frame_ok(sg[0]) && frame_ok(sg[1]) && same_frame(sg[0], sg[1]);
    if (have) {
        for (int k = 0; k < 2; ++k) {
            std::string bs;
            for (auto& b : sg[k].bands) bs += " [C" + dec(b.c) + " line " + dec((long long)b.first_line) + " h " + dec((long long)b.height) + " cols " + dec((long long)b.first_col) + "-" + dec((long long)b.last_col) + (b.uniform ? "" : " NOT uniform") + "]";
            ck.note("F6 graded frame " + dec(k + 2) + ": " + dec((long long)sg[k].lines) + " output lines, " + dec((long long)sg[k].bands.size()) + " bands," + bs +
                    "; pixels all black or C2-C8: " + dec(sg[k].pixels_ok) + ", $555 absent: " + dec(sg[k].ok555));
        }
    }
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and BPLEN set (AA p.22)");
    ck.expect(have, id + "-FRAMES: the second and the third complete frames (VSYNC to VSYNC) that begin after M_ready both end before M_done");
    if (have) {
        ck.expect(copper, id + "-COPPER: the Copper list was read in the vertical blank before each graded frame: every pointer word of the list was read in the frame before");
    } else {
        not_graded(ck, id + "-COPPER", id + "-FRAMES");
    }
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    if (!(fixture && dma && have && copper && tmo)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(ok && reads, "F6: in an 8-bitplane lores display the data bitplane DMA fetches for plane x (x = 2-8) (in each graded frame there are bitplane reads in each of B1-B8) goes into BPLxDAT and appears on the video output as colour index 2^(x-1): in each of two frames every pixel is black or one of C2-C8 and never $555, exactly seven bands in the order C2, C3, C4, C5, C6, C7, C8 each a single colour, all of one height with equal gaps and the same first and last column, and the two frames are identical (AA p.15 'These registers receive the DMA data fetched from RAM by the Bit Plane address pointers described above ... They act as a 8 word parallel to serial buffer for up to 8 memory Bit Planes'; AA p.16 BPU '0000-1000 (NONE thru 8 inclusive)'; AA p.17 PF2OF table; AA p.18 BPLAM)");
    // claims: none (the 14 '&' claims of BPL2DAT-BPL8DAT also state 'only', which is not graded)
}

} // namespace s7
