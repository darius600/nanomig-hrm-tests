// s3_props_s121314.h -- propositions S12, S13 and S14

#pragma once

#include "s3_props_s1011.h"

namespace s3 {

// decodes the 16-bit word of a sprite pattern on a line: the leftmost cS pixel is bit 15;
// the 32 pixels from the leftmost cS pixel are read, whatever the rest of the line shows;
// returns -2 when the line shows no cS pixel and -1 when the pixel pairs differ
inline int decode_word(const std::vector<uint8_t>& cs, int* edge_out = nullptr) {
    int e = first_one(cs);
    if (e < 0) return -2;
    if (edge_out) *edge_out = e;
    if (size_t(e + 32) > cs.size()) return -1;
    int w = 0;
    for (int i = 0; i < 16; ++i) {
        if (cs[e + 2 * i] != cs[e + 2 * i + 1]) return -1;
        w = (w << 1) | cs[e + 2 * i];
    }
    if (count_in_range(cs, e, 32) != 2 * __builtin_popcount(w)) return -1;
    return w;
}

// ---- S12 ----------------------------------------------------------------------
struct S12Run {
    std::shared_ptr<RunData> r;
    uint64_t P = 0;
    bool ok = false;
    std::array<std::vector<int64_t>, 8> reads;        // data-pair read times relative to t_F(F2)
    std::array<int64_t, 8> tend{};
    std::array<std::vector<int>, 8> rows;             // decoded rows of the band, line by line
    std::array<int, 8> first_line{};
    bool spr_ok = true;
};

inline S12Run run_s12(const std::string& pfx, bool even_sh10, int shift, bool fmode_on, bool spren, bool sixteen_for_sh10, bool wide = false) {
    S12Run o;
    Std ss; ss.bpu = 1; ss.pal = Pal::Spr; ss.dmacon = spren ? 0x83A0 : 0x8380; ss.fmode = fmode_on ? 0x8000 : 0x0000;
    if (wide) { ss.lf = true; ss.htotal = 0x00E3; ss.vtotal = 0x0138; ss.beamcon0 = 0x08A0; }       // 228 colour clocks per line
    RunSpec s = base_spec(ss, pfx);
    std::array<int, 8> nrows{};
    for (int x = 0; x < 8; ++x) {
        bool sh10 = ((x & 1) == 0) == even_sh10;
        int v = 0x40 + 0x14 * x + shift;
        nrows[x] = (sh10 && !sixteen_for_sh10) ? 8 : 16;
        std::vector<SprRow> rows;
        for (int r = 0; r < nrows[x]; ++r) { uint16_t w = mutating("S12-PC-VISIBLE") ? uint16_t(0) : uint16_t(0x8000 | (r << 4)); rows.push_back({w, w}); }
        int H = 0x240 | (sh10 ? 0x400 : 0);
        s.mem.push_back({L_x(x), spr_list(v, v + 16, H, false, rows)});
    }
    s.vb = spr_ptrs(own_lists());
    for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
    EventSpec fin; fin.nvb = 4; s.ev.push_back(fin);
    o.r = run(s);
    const RunData& r = *o.r;
    Frame f = get_frame(r, 2);
    o.P = chip_line_period(r);
    o.ok = f.ok && o.P > 0;
    if (!o.ok) return o;
    FrameView fv(r, 2);
    o.ok = fv.ok;
    for (int x = 0; x < 8 && o.ok; ++x) {
        for (int i = 0; i < nrows[x]; ++i) {
            auto t = first_read_at(r, L_x(x) + 4 + 4 * uint32_t(i), f.t0, f.t1);
            if (!t) { o.ok = false; break; }
            o.reads[x].push_back(int64_t(*t) - int64_t(f.t0));
        }
        auto te = first_read_at(r, L_x(x) + 4 + 4 * uint32_t(nrows[x]), f.t0, f.t1);
        if (!te) { o.ok = false; break; }
        o.tend[x] = int64_t(*te) - int64_t(f.t0);
        if (!list_in_order(r, f, L_x(x), 2 + 2 * nrows[x] + 2)) o.spr_ok = false;
        // the band: lines v-2 .. v+22 of this sprite
        int v = 0x40 + 0x14 * x + shift;
        o.first_line[x] = -1;
        for (int n = v - 2; n <= v + 22 && n <= 0xFF; ++n) {
            auto cs = fv.cls(n, kColS);
            int w = decode_word(cs);
            if (std::getenv("NMH_DBG") && x == 0 && n >= v && n < v + 4) {
                std::cout << "DBG S12 line " << n << " runs:";
                for (const auto& rr : runs_of(cs)) std::cout << " [" << rr.first << "+" << rr.second << "]";
                std::cout << " w=" << w << std::endl;
            }
            if (w == -2) { if (o.first_line[x] >= 0) break; else continue; }
            if (o.first_line[x] < 0) o.first_line[x] = n;
            int row = w < 0 ? -1 : ((w == (0x8000 | (((w >> 4) & 0xF) << 4))) ? ((w >> 4) & 0xF) : -1);
            o.rows[x].push_back(row);
        }
    }
    return o;
}

inline void prop_S12() {
    if (!want("S12")) return;
    Pre pre("S12");
    bool pc_spr = true, pc_lom = true, pc_vis = true, ok = true;
    S12Run runs[2][2];      // [even_sh10 = A/B][shift 0/1]
    for (int ab = 0; ab < 2; ++ab)
        for (int sh = 0; sh < 2; ++sh) {
            bool mut = mutating("S12") && ab == 0 && sh == 0;
            bool wide = mutating("S12-PC-LOM") && ab == 0 && sh == 1;
            runs[ab][sh] = run_s12("S12", ab == 0, sh, !mut, !mutating("S12-PC-SPR"), mut, wide);
            pre.check(*runs[ab][sh].r, 1, 2, false, true, false);
            if (!runs[ab][sh].ok || !runs[ab][sh].spr_ok) pc_spr = false;
        }
    for (int ab = 0; ab < 2; ++ab) {
        const S12Run& A = runs[ab][0];
        const S12Run& B = runs[ab][1];
        if (!A.ok || !B.ok) { ok = false; continue; }
        for (int x = 0; x < 8; ++x) {
            bool sh10 = ((x & 1) == 0) == (ab == 0);
            int gap = sh10 ? 2 : 1;
            const auto& rd = A.reads[x];
            for (size_t i = 1; i < rd.size(); ++i) {
                long k = 0;
                if (!lom_delta(rd[i] - rd[i - 1], A.P, &k)) pc_lom = false;
                if (k != gap) ok = false;
            }
            if (A.tend[x] <= rd.back()) ok = false;
            // the displayed rows: each row once (SH10 = 0) or twice in a row (SH10 = 1), r = 0, 1, ...
            std::vector<int> want_rows;
            for (int r = 0; r < (sh10 ? 8 : 16); ++r) for (int c = 0; c < (sh10 ? 2 : 1); ++c) want_rows.push_back(r);
            if (A.rows[x] != want_rows || B.rows[x] != want_rows) ok = false;
            if (std::getenv("NMH_DBG")) {
                std::cout << "DBG S12 " << (ab == 0 ? "A" : "B") << " x=" << x << " sh10=" << sh10 << " first_line " << A.first_line[x] << "/" << B.first_line[x] << " rowsA:";
                for (int r : A.rows[x]) std::cout << " " << r;
                std::cout << " | rowsB:";
                for (int r : B.rows[x]) std::cout << " " << r;
                std::cout << " | readsA(lines):";
                for (auto t : A.reads[x]) std::cout << " " << double(t) / double(A.P);
                std::cout << " tend " << double(A.tend[x]) / double(A.P) << std::endl;
            }
            if (A.first_line[x] < 0) pc_vis = false;
            // the primed run moves every read and every displayed row by exactly one line
            if (B.reads[x].size() != A.reads[x].size()) { ok = false; continue; }
            for (size_t i = 0; i < rd.size(); ++i) {
                long k = 0;
                if (!lom_delta(B.reads[x][i] - rd[i], A.P, &k)) pc_lom = false;
                if (k != 1) ok = false;
            }
            if (B.first_line[x] != A.first_line[x] + 1) ok = false;
        }
        g.note("S12: run " + std::string(ab == 0 ? "A" : "B") + ": sprite 0 first band line " + dec(A.first_line[0]) + ", rows seen " + dec(long(A.rows[0].size())) +
               ", read gaps: " + (A.reads[0].size() > 1 ? dec(long(std::lround(double(A.reads[0][1] - A.reads[0][0]) / double(A.P)))) : std::string("n/a")) + " lines");
    }
    pre.print("the S12 runs");
    g.expect(pc_spr, "S12-PC-SPR: in every run F2 reads each of the eight sprite lists once in order up to its end pair");
    g.expect(pc_vis, "S12-PC-VISIBLE: in runs A and B every band shows the sprite");
    g.expect(pc_lom, "S12-PC-LOM: every line-offset measure of the data-pair reads is valid (within P/8 of a whole number of lines)");
    g.assert_line("S12", pre.setup && pre.frame && pre.tag && pc_spr && pc_vis && pc_lom, ok,
        "S12: opus:AA-sprxpos.c10 sol:AA-sprxpos.c16: with SSCAN2 set, SH10 of a sprite switches that sprite's scan-doubling on or off (p.28, p.4): "
        "with SH10 = 1 the eight data pairs of the list are read two lines apart and each row is shown on two consecutive lines, with SH10 = 0 the sixteen pairs are read one line apart and each row is shown once, "
        "and with start and stop one line later every read and every displayed row moves by exactly one line");
}

// ---- S13 ----------------------------------------------------------------------
inline void prop_S13() {
    if (!want("S13")) return;
    Pre pre("S13");
    // run D: DMA sprite 0
    Std od; od.bpu = 1; od.pal = Pal::Spr; od.dmacon = mutating("S13-PC-SPR") ? 0x8380 : 0x83A0;
    RunSpec sd = base_spec(od, "S13");
    std::vector<SprRow> rows;
    for (int r = 0; r < 16; ++r) {
        uint16_t a = uint16_t(0x8000 | (r << 4)), b = uint16_t(0x0001 | (r << 8));
        if (mutating("S13") && r == 5) { a = 0x8080; b = 0x0003; }       // row 5's words changed
        rows.push_back({a, b});
    }
    sd.mem.push_back({L_x(0), spr_list(0x60, 0x70, sh_of_lores(0x90), false, rows)});
    sd.vb = spr_ptrs(own_lists());
    for (const auto& w : bpl_ptrs(all_z(), 1)) sd.vb.push_back(w);
    EventSpec fin; fin.nvb = 4; sd.ev.push_back(fin);
    auto rd = run(sd);
    pre.check(*rd, 1, 2, false, true, true);
    Frame f2 = get_frame(*rd, 2);
    bool pc_spr = f2.ok && list_in_order(*rd, f2, L_x(0), 2 + 2 * 16 + 2);
    // run M: the CPU writes the words of list word 0 and 1, then DATB and DATA
    Std om; om.bpu = 1; om.pal = Pal::Spr; om.dmacon = 0x8380;
    om.extra.push_back({R_SPRxPOS(0), 0x6048});
    om.extra.push_back({R_SPRxCTL(0), 0x7000});
    om.extra.push_back({R_SPRxDATB(0), 0x0001});
    om.extra.push_back({R_SPRxDATA(0), 0x8000});
    RunSpec sm = base_spec(om, "S13");
    sm.vb = bpl_ptrs(all_z(), 1);
    EventSpec finm; finm.nvb = 4; sm.ev.push_back(finm);
    auto rm = run(sm);
    pre.check(*rm, 1, 1, false, true, true);
    // the displayed rows of run D
    FrameView fvd(*rd, 2), fvm(*rm, 1);
    std::vector<int> words;
    int first = -1, edge_d = -1;
    if (fvd.ok)
        for (int n = 0x50; n <= 0x90; ++n) {
            int e = -1;
            int w = decode_word(fvd.cls(n, kColS), &e);
            if (w == -2) { if (first >= 0) break; else continue; }
            if (first < 0) first = n;
            words.push_back(w);
            if (edge_d < 0) edge_d = e;
            else if (e != edge_d && w >= 0) edge_d = -2;
        }
    bool rows_ok = words.size() == 16;
    for (int r = 0; r < 16 && rows_ok; ++r) {
        int want_w = 0x8000 | (r << 4) | 0x0001 | (r << 8);
        if (words[r] != want_w) rows_ok = false;
    }
    int edge_m = -1;
    bool edge_ok = false;
    if (fvm.ok) {
        auto cs = fvm.cls(0x80, kColS);
        edge_m = first_one(cs);
        edge_ok = edge_m >= 0 && edge_m == edge_d;
    }
    g.note("S13: run D shows " + dec(long(words.size())) + " band lines from line 0x" + hexs(first < 0 ? 0 : first, 2) + "; left edge D = " + dec(edge_d) + ", left edge M = " + dec(edge_m));
    pre.print("the S13 runs");
    g.expect(pc_spr, "S13-PC-SPR: in F2 of run D the list of sprite 0 is read in order: word 0, word 1 once, the sixteen data pairs and the end pair");
    g.assert_line("S13", pre.setup && pre.frame && pre.tag && pre.pixel && pc_spr, rows_ok && edge_ok,
        "S13: opus:AA-sprxdatb.c6 sol:AA-sprxdata.c13: with sprite DMA the words read from sprite 0's list become its position and control and its image data (p.28): "
        "over consecutive output lines the sprite shows DATA_r | DATB_r, MSB first, for r = 0..15 in the order read, one row per line, and its left edge equals that of the same words written by the 68000");
}

// ---- S14 ----------------------------------------------------------------------
inline uint16_t pos_for(int X, int vstart = 0x40) { return uint16_t((vstart << 8) | ((X >> 1) & 0xFF)); }

inline void prop_S14() {
    if (!want("S14")) return;
    Pre pre("S14");
    bool pc_beam = true, pc_move = true, pc_spr = true, ok_a = true, ok_b = true, ran_b = false;
    // ---- (a) manual sprite 0; the 68000 writes SPRxCTL = 0x0001 (SH2, 140 ns: two output pixels) at line 0xB0
    for (int once = 0; once < 1; ++once) {
        Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = 0x8380;
        RunSpec s = base_spec(o, "S14");
        s.vb = bpl_ptrs(all_z(), 1);
        s.vb.push_back({R_SPRxPOS(0), pos_for(0x90)});
        s.vb.push_back({R_SPRxCTL(0), 0x0000});
        s.vb.push_back({R_SPRxDATB(0), 0xE001});
        s.vb.push_back({R_SPRxDATA(0), 0xE001});
        const int hb = mutating("S14-PC-BEAM") ? 0xE0 : 0x08;
        EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
        EventSpec e1; e1.nvb = 2; e1.line = 0x50; e1.h = hb; e1.wr.push_back({R_SPRxPOS(0), pos_for(0xA0)}); s.ev.push_back(e1);
        EventSpec e2; e2.line = 0x70; e2.h = 0xB0; e2.wr.push_back({R_SPRxPOS(0), pos_for(0xB0)}); s.ev.push_back(e2);
        EventSpec e4; e4.line = 0xB0; e4.h = hb;
        e4.wr.push_back({R_SPRxCTL(0), mutating("S14-e4") ? uint16_t(0x0000) : uint16_t(0x0001)});
        e4.wr.push_back({R_SPRxDATA(0), 0xE001});
        s.ev.push_back(e4);
        EventSpec e5; e5.line = 0xC0; e5.h = hb; e5.wr.push_back({R_SPRxDATB(0), 0x8007}); e5.wr.push_back({R_SPRxDATA(0), 0x8007}); s.ev.push_back(e5);
        EventSpec fin; fin.nvb = 2; s.ev.push_back(fin);
        if (!mutating("S14")) s.cev.push_back({0x90, 0x20, {{R_SPRxPOS(0), pos_for(0xC0)}}});
        s.cev.push_back({0xD0, 0x20, {{R_SPRxDATB(0), 0xF00F}, {R_SPRxDATA(0), 0xF00F}}});
        // the intended Copper moves
        std::vector<CopEvent> intended;
        intended.push_back({0x90, 0x20, {{R_SPRxPOS(0), pos_for(0xC0)}}});
        intended.push_back({0xD0, 0x20, {{R_SPRxDATB(0), 0xF00F}, {R_SPRxDATA(0), 0xF00F}}});
        if (mutating("S14-PC-MOVE")) s.cev[0].mv[0].val ^= 0x0002;
        auto r = run(s);
        pre.check(*r, 1, 2, false, true, true);
        FrameView f2(*r, 2);
        Frame f = f2.f;
        if (!f2.ok) { ok_a = false; pc_beam = false; pc_move = false; continue; }
        uint64_t P = line_period(f);
        // beam markers (the CPU events 1..4, indices 1..4 after the flush)
        for (size_t i = 1; i + 1 < r->spec.ev.size(); ++i) {
            const auto& e = r->spec.ev[i];
            uint16_t vp = r->ev_word(int(i), 2), vh = r->ev_word(int(i), 4);
            int line = ((vp & 7) << 8) | (vh >> 8);
            uint64_t tm = marker_time(*r, int(i));
            if (line != e.line || !(tm >= f.t0 && tm < f.t1)) pc_beam = false;
        }
        // Copper moves on their lines
        {
            size_t ci = 0;
            for (const auto& ce : intended) {
                // the layout lists events in the spec order; with S14 mutated the first one is absent
                size_t idx = ci;
                if (mutating("S14")) { if (ci == 0) { ++ci; continue; } idx = 0; }
                const auto& lay = r->lay.cev_data[idx];
                for (size_t j = 0; j < lay.size(); ++j) {
                    uint16_t val = 0xFFFF; uint64_t tt = 0;
                    auto it = r->clread.find(lay[j]);
                    if (it != r->clread.end())
                        for (const auto& pp : it->second) if (pp.first >= f.t0 && pp.first < f.t1) { val = pp.second; tt = pp.first; break; }
                    if (!tt || tt < f.tag.at(ce.line) || tt >= line_t1(f, ce.line, P) || val != ce.mv[j].val) pc_move = false;
                }
                ++ci;
            }
        }
        // the graded rows
        int edge0 = -1;
        {
            auto cs = f2.cls(0x30, kColS);
            edge0 = first_one(cs);
        }
        struct Rows { int a, b, X; int extra_px; uint16_t word; };
        const int sh1 = 2;       // the shift written by e4: SH2 (140 ns) is two output pixels
        const Rows rows[] = {
            {0x30, 0x4F, 0x90, 0, 0xE001}, {0x51, 0x6F, 0xA0, 0, 0xE001}, {0x71, 0x8F, 0xB0, 0, 0xE001},
            {0x91, 0xAF, 0xC0, 0, 0xE001}, {0xB1, 0xBF, 0xC0, sh1, 0xE001}, {0xC1, 0xCF, 0xC0, sh1, 0x8007},
            {0xD1, 0xFF, 0xC0, sh1, 0xF00F}};
        long bad = 0;
        for (const auto& rw : rows)
            for (int n = rw.a; n <= rw.b; ++n) {
                auto cs = f2.cls(n, kColS);
                int edge = edge0 + 2 * (rw.X - 0x90) + rw.extra_px;
                bool good = edge0 >= 0 && pattern_at(cs, edge, rw.word) && count_ones(cs) == 2 * __builtin_popcount(rw.word);
                if (!good) ++bad;
            }
        g.note("S14 (a): lines whose column is not where and what the page predicts: " + dec(bad));
        ok_a = ok_a && bad == 0;
    }
    // ---- (b) eight DMA sprites, SPRxPOS written at line 0x80
    if (!g_mutate || mutating("S14-b") || mutating("S14-PC-SPR") || mutating("S14-PC-SETUP") || mutating("S14-PC-FRAME") ||
        mutating("S14-PC-TAG") || mutating("S14-PC-PIXEL") || mutating("S14-PC-BEAM") || mutating("S14-PC-MOVE")) {
        ran_b = true;
        std::shared_ptr<RunData> rr[2];
        for (int with = 0; with < 2; ++with) {
            Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = mutating("S14-PC-SPR") ? 0x8380 : 0x83A0;
            RunSpec s = base_spec(o, "S14");
            for (int x = 0; x < 8; ++x) {
                std::vector<SprRow> rows(0xA0 - 0x60, SprRow{0xE001, 0xE001});
                s.mem.push_back({L_x(x), spr_list(0x60, 0xA0, sh_of_lores(kXs[x]), false, rows)});
            }
            s.vb = spr_ptrs(own_lists());
            for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
            EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
            if (with) {
                const int hb = mutating("S14-PC-BEAM") ? 0xE0 : 0x08;
                EventSpec e; e.nvb = 2; e.line = 0x80; e.h = hb;
                CopEvent ce; ce.line = 0x80; ce.h = 0x08;
                for (int x = 0; x < 8; ++x) {
                    if (mutating("S14-b") && x == 3) continue;
                    Wr w{R_SPRxPOS(x), pos_for(kXs[x] + 8, 0x60)};
                    if ((x & 1) == 0) e.wr.push_back(w); else ce.mv.push_back(w);
                }
                s.ev.push_back(e);
                s.cev.push_back(ce);
                EventSpec fin; fin.nvb = 2; s.ev.push_back(fin);
            } else {
                EventSpec fin; fin.nvb = 4; s.ev.push_back(fin);
            }
            rr[with] = run(s);
            pre.check(*rr[with], 1, 2, false, true, true);
            Frame f = get_frame(*rr[with], 2);
            if (!f.ok || !all_lists_in_order(*rr[with], f, 2 + 2 * (0xA0 - 0x60) + 2)) {
                pc_spr = false;
                for (int x = 0; x < 8 && f.ok; ++x) dbg_list_order(*rr[with], f, L_x(x), 2 + 2 * (0xA0 - 0x60) + 2, with ? "events" : "reference");
            }
        }
        FrameView fr(*rr[0], 2), fe(*rr[1], 2);
        if (!fr.ok || !fe.ok) ok_b = false;
        else {
            // beam marker of the CPU event, Copper move on its line
            {
                uint16_t vp = rr[1]->ev_word(1, 2), vh = rr[1]->ev_word(1, 4);
                int line = ((vp & 7) << 8) | (vh >> 8);
                uint64_t tm = marker_time(*rr[1], 1);
                if (line != 0x80 || !(tm >= fe.f.t0 && tm < fe.f.t1)) pc_beam = false;
                const auto& lay = rr[1]->lay.cev_data[0];
                uint64_t P = line_period(fe.f);
                for (size_t j = 0; j < lay.size(); ++j) {
                    auto t = first_clread(*rr[1], lay[j], fe.f.t0, fe.f.t1);
                    if (!t || *t < fe.f.tag.at(0x80) || *t >= line_t1(fe.f, 0x80, P)) pc_move = false;
                }
            }
            int edge0 = first_one(fr.cls(0x70, kColS));
            long bad = 0;
            for (int n = 0x62; n <= 0x9F; ++n) {
                if (n == 0x80) continue;
                auto ref = fr.cls(n, kColS);
                if (ref.empty() || count_ones(ref) == 0) continue;       // the reference does not show the sprites on this line
                auto cs = fe.cls(n, kColS);
                int shift = n < 0x80 ? 0 : 16;
                bool good = edge0 >= 0;
                for (int x = 0; x < 8 && good; ++x) good = pattern_at(cs, edge0 + 48 * x + shift, 0xE001);
                if (good && count_ones(cs) != 8 * 2 * 3 + 8 * 2) { /* 0xE001 has four set bits... */ }
                if (good && count_ones(cs) != 8 * 2 * __builtin_popcount(0xE001)) good = false;
                if (!good) ++bad;
            }
            g.note("S14 (b): lines whose eight columns are not where the page predicts: " + dec(bad));
            ok_b = ok_b && bad == 0;
        }
    }
    pre.print("the S14 runs");
    g.expect(pc_beam, "S14-PC-BEAM: every beam marker's re-read shows its target line, and the markers lie in F2");
    g.expect(pc_move, "S14-PC-MOVE: every Copper MOVE of the events is fetched on its event line of F2 with the intended data word");
    g.expect(pc_spr, "S14-PC-SPR: in (b) F2 reads each of the eight sprite lists once in order up to its end pair");
    const bool pre_all = pre.setup && pre.frame && pre.tag && pre.pixel && pc_beam && pc_move && pc_spr;
    g.assert_line("S14", pre_all, ok_a && ok_b,
        "S14: opus:AA-sprxctl.c19 sol:AA-sprxctl.c33 opus:AA-sprxdatb.c7 sol:AA-sprxdata.c14: either processor may load the sprite registers at any time (p.28): "
        "a write of SPRxPOS, SPRxCTL, SPRxDATA or SPRxDATB by the 68000 or by the Copper takes effect from the next line on, for a manual sprite and during DMA use");
    if (ran_b)
        g.assert_line("S14-B", pre_all, ok_b,
            "S14-B: part (b) of S14 alone (eight DMA sprites, SPRxPOS written at line 0x80 by the 68000 for the even and by the Copper for the odd sprites): a write to SPRxPOS of sprite x moves exactly the column of the sprite whose list is read through pointer x, from line 0x80 on (p.28 'Sprite x Vert-Horiz start position data')");
}

}  // namespace s3
