// s3_props_b34.h -- propositions B3 and B4 (bit-plane data registers)

#pragma once

#include "s3_vid.h"

namespace s3 {

// ---- B3 -----------------------------------------------------------------------
// Bit-plane data registers receive the DMA data: 16-pixel words, MSB first.
inline uint16_t W3(uint32_t i) { return uint16_t(0x8000 | ((i & 0x3FFF) << 1)); }

inline void prop_B3() {
    if (!want("B3")) return;
    Pre pre("B3");
    bool pc_bpl = true, pc_words = true;
    long lines_graded = 0, lines_bad = 0;
    for (int x = 1; x <= 8; ++x) {
        if (g_mutate && x > 1) break;
        Std o; o.bpu = 8; o.pal = Pal::On; o.dmacon = 0x8380;
        if (mutating("B3-PC-BPL")) o.dmacon = 0x8280;
        if (mutating("B3-PC-WORDS")) o.ddfstop = 0x0040;
        RunSpec s = base_spec(o, "B3");
        auto a = all_z();
        a[x] = P_k(x);
        s.vb = bpl_ptrs(a, 8);
        std::vector<uint16_t> fill(0x2000);
        for (uint32_t i = 0; i < fill.size(); ++i) fill[i] = mutating("B3") ? 0 : W3(i);
        s.mem.push_back({P_k(x), fill});
        EventSpec e; e.nvb = 5; s.ev.push_back(e);
        auto r = run(s);
        pre.check(*r, 1, 2, false, true, true);
        Frame f = get_frame(*r, 2);
        if (!f.ok || f.tag.size() < 200) { pc_bpl = false; continue; }
        uint64_t P = line_period(f);
        for (int k = 1; k <= 8; ++k)
            if (k != x && count_in(*r, f.t0, f.t1, P_k(k), P_k(k) + 0x4000) != 0) pc_bpl = false;
        int bad_here = 0;
        for (int n = 0x30; n <= 0xFF; ++n) {
            if (!f.tag.count(n)) { pc_bpl = false; break; }
            uint64_t t0 = f.tag.at(n), t1 = line_t1(f, n, P);
            if (count_in(*r, t0, t1, kZ, kZ + 0x4000, 1) < 1) pc_bpl = false;
            std::vector<uint16_t> words;
            for (size_t i = acc_from(*r, t0); i < r->acc.size() && r->acc[i].cyc < t1; ++i) {
                const Acc& ac = r->acc[i];
                uint32_t b = ac.waddr * 2;
                if (!ac.wr && b >= P_k(x) && b < P_k(x) + 0x4000) words.push_back(W3((b - P_k(x)) / 2));
            }
            if (words.empty()) pc_bpl = false;
            if (words.size() < 3) { pc_words = false; continue; }
            std::vector<uint16_t> mid(words.begin() + 1, words.end() - 1);
            int L = out_line(*r, f, n);
            auto cls = is_col(*r, L, kColON);
            bool ok = find_seq(cls, word_pixels(mid)) >= 0;
            ++lines_graded;
            if (!ok) { ++lines_bad; ++bad_here; }
        }
        g.note("B3: plane " + dec(x) + ": lines whose output does not contain the fetched words (all but first and last), MSB first, 2 output pixels per bit: " + dec(bad_here) + " of 208");
    }
    pre.print("the B3 runs");
    g.expect(pc_bpl, "B3-PC-BPL: with BPU = 8 the port shows reads of P_x and of Z on every line 0x30-0xFF of F2, and no read in the regions of the other seven planes' P_k");
    g.expect(pc_words, "B3-PC-WORDS: at least 3 words of P_x are read on every graded line");
    bool pre_all = pre.setup && pre.frame && pre.tag && pre.pixel && pc_bpl && pc_words;
    g.assert_line("B3", pre_all, lines_graded > 0 && lines_bad == 0,
        "B3: opus:AA-bplxdat.c4 opus:AA-bplxdat.c6 opus:AA-bplxdat.c11 sol:AA-bplxdat.c21 sol:AA-bplxdat.c22 sol:AA-bplxdat.c27: the words that plane x's pointer fetches reach the display as 16-pixel words (FMODE = 0), most significant bit first and leftmost (p.15, p.16), for every plane x = 1..8 on every graded line");
}

// ---- B4 -----------------------------------------------------------------------
// Bit-plane data registers written by the processors; the plane-1 write triggers the conversion.
inline uint16_t Q4(int k) { return uint16_t(0xC000 | (1 << k)); }

struct Ev4 { int line; std::vector<Wr> wr; };

inline std::vector<Ev4> events_b4() {
    std::vector<Ev4> v;
    const bool omit3 = mutating("B4");
    const bool dnow = mutating("B4-d");
    for (int k = 1; k <= 8; ++k) {
        Ev4 e; e.line = 0x40 + 0x0C * (k - 1);
        for (int p = 8; p >= 2; --p) e.wr.push_back({R_BPLxDAT(p), p == k ? Q4(k) : uint16_t(0)});
        if (!(omit3 && k == 3)) e.wr.push_back({R_BPLxDAT(1), k == 1 ? Q4(1) : uint16_t(0)});
        v.push_back(e);
    }
    {
        Ev4 e; e.line = 0xA0;
        for (int p = 8; p >= 2; --p) e.wr.push_back({R_BPLxDAT(p), Q4(p)});
        e.wr.push_back({R_BPLxDAT(1), Q4(1)});
        v.push_back(e);
    }
    for (int k = 2; k <= 8; ++k) {
        int g0 = 0xAC + 0x08 * (k - 2);
        Ev4 e; e.line = g0;
        for (int p = 8; p >= 2; --p) e.wr.push_back({R_BPLxDAT(p), p == k ? Q4(k) : uint16_t(0)});
        if (dnow) e.wr.push_back({R_BPLxDAT(1), 0});
        v.push_back(e);
        if (!dnow) { Ev4 t; t.line = g0 + 4; t.wr.push_back({R_BPLxDAT(1), 0}); v.push_back(t); }
    }
    std::stable_sort(v.begin(), v.end(), [](const Ev4& a, const Ev4& b) { return a.line < b.line; });
    return v;
}

inline void prop_B4() {
    if (!want("B4")) return;
    Pre pre("B4");
    bool pc_nodma = true, pc_beam = true, pc_move = true;
    bool all_ok = true;
    const int h = mutating("B4-PC-BEAM") ? 0xE0 : 0x60;
    for (int variant = 0; variant < 2; ++variant) {
        bool cpu = variant == 0;
        if (g_mutate && cpu && mutating("B4-PC-MOVE")) continue;
        if (g_mutate && !cpu && mutating("B4-PC-BEAM")) continue;
        Std o; o.bpu = 8; o.pal = Pal::On; o.dmacon = mutating("B4-PC-NODMA") ? 0x8380 : 0x8280;
        RunSpec s = base_spec(o, "B4");
        auto evs = events_b4();
        std::vector<Wr> clear;
        for (int p = 8; p >= 1; --p) clear.push_back({R_BPLxDAT(p), 0});
        EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
        s.vb = bpl_ptrs(all_z(), 8);
        if (cpu) {
            EventSpec c; c.nvb = 2; c.wr = clear; s.ev.push_back(c);
            for (const auto& e : evs) { EventSpec x; x.line = e.line; x.h = h; x.wr = e.wr; s.ev.push_back(x); }
            EventSpec fin; fin.nvb = 2; s.ev.push_back(fin);
        } else {
            for (const auto& w : clear) s.vb.push_back(w);
            for (const auto& e : evs) s.cev.push_back({e.line, 0x60, e.wr});
            if (mutating("B4-PC-MOVE")) s.cev[0].mv[0].val ^= 0x0004;   // poked data differs from the intended
            EventSpec fin; fin.nvb = 5; s.ev.push_back(fin);
        }
        // the intended Copper moves are the ones of the event list
        std::vector<CopEvent> intended;
        for (const auto& e : evs) intended.push_back({e.line, 0x60, e.wr});
        auto r = run(s);
        pre.check(*r, 1, 2, false, true, true);
        Frame f = get_frame(*r, 2);
        std::string vn = cpu ? "CPU run" : "Copper run";
        if (!f.ok || f.tag.size() < 200) { all_ok = false; pc_beam = false; pc_move = false; continue; }
        uint64_t P = line_period(f);
        // no bit-plane DMA in F2
        for (int k = 1; k <= 8; ++k)
            if (count_in(*r, f.t0, f.t1, P_k(k), P_k(k) + 0x4000) != 0) pc_nodma = false;
        if (count_in(*r, f.t0, f.t1, kZ, kZ + 0x4000) != 0) pc_nodma = false;
        // events attributed to their lines
        if (cpu) {
            for (size_t i = 0; i < evs.size(); ++i) {
                int k = int(i) + 2;
                uint16_t vp = r->ev_word(k, 2), vh = r->ev_word(k, 4);
                int line = ((vp & 7) << 8) | (vh >> 8);
                uint64_t tm = marker_time(*r, k);
                if (std::getenv("NMH_DBG")) std::printf("DBG ev %zu want line %X got %X h %02X marker rel %ld\n", i, evs[i].line, line, vh & 0xFF, long(tm) - long(f.t0));
                if (line != evs[i].line || !(tm >= f.t0 && tm < f.t1)) pc_beam = false;
            }
        } else {
            for (size_t i = 0; i < s.cev.size(); ++i) {
                const auto& lay = r->lay.cev_data[i];
                for (size_t j = 0; j < lay.size(); ++j) {
                    uint16_t val = 0xFFFF;
                    uint64_t tt = 0;
                    auto it = r->clread.find(lay[j]);
                    if (it != r->clread.end())
                        for (const auto& pp : it->second)
                            if (pp.first >= f.t0 && pp.first < f.t1) { val = pp.second; tt = pp.first; break; }
                    int ln = evs[i].line;
                    if (!tt || tt < f.tag.at(ln) || tt >= line_t1(f, ln, P) || val != intended[i].mv[j].val) pc_move = false;
                }
            }
        }
        // the outputs
        auto line_has = [&](int n, uint16_t word) {
            int L = out_line(*r, f, n);
            auto cls = is_col(*r, L, kColON);
            return find_seq(cls, word_pixels({word})) >= 0;
        };
        bool e1 = true, e2 = true, e3 = true;
        for (int k = 1; k <= 8; ++k) e1 = e1 && line_has(0x40 + 0x0C * (k - 1), Q4(k));
        e1 = e1 && line_has(0xA0, 0xC1FE);
        for (int k = 2; k <= 8; ++k) {
            int g0 = 0xAC + 0x08 * (k - 2);
            for (int n = g0; n < g0 + 4; ++n) e2 = e2 && !line_has(n, Q4(k));
            e2 = e2 && line_has(g0 + 4, Q4(k));
        }
        for (int n = 0x30; n < 0x40; ++n) {
            int L = out_line(*r, f, n);
            auto cls = is_col(*r, L, kColON);
            if (L < 0 || count_ones(cls) != 0) e3 = false;
        }
        g.note("B4 " + vn + ": expected 1 " + std::string(e1 ? "holds" : "fails") + ", expected 2 " + (e2 ? "holds" : "fails") + ", expected 3 " + (e3 ? "holds" : "fails"));
        all_ok = all_ok && e1 && e2 && e3;
    }
    pre.print("the B4 runs");
    g.expect(pc_nodma, "B4-PC-NODMA: the port shows no read in any P_k or Z region in F2 (no bit-plane DMA)");
    g.expect(pc_beam, "B4-PC-BEAM: in the CPU run every beam marker's re-read shows its target line, and the marker lies in F2");
    g.expect(pc_move, "B4-PC-MOVE: in the Copper run each event's MOVEs are fetched on the event line of F2 with the intended data words");
    bool pre_all = pre.setup && pre.frame && pre.tag && pre.pixel && pc_nodma && pc_beam && pc_move;
    g.assert_line("B4", pre_all, all_ok,
        "B4: opus:AA-bplxdat.c1 opus:AA-bplxdat.c2 opus:AA-bplxdat.c7 opus:AA-bplxdat.c8 opus:AA-bplxdat.c9 opus:AA-bplxdat.c14 sol:AA-bplxdat.c1 sol:AA-bplxdat.c2 sol:AA-bplxdat.c3 sol:AA-bplxdat.c4 sol:AA-bplxdat.c5 sol:AA-bplxdat.c6 sol:AA-bplxdat.c7 sol:AA-bplxdat.c8 sol:AA-bplxdat.c9 sol:AA-bplxdat.c23 sol:AA-bplxdat.c24 sol:AA-bplxdat.c25: "
        "BPL1DAT-BPL8DAT written by the 68000 and by the Copper hold one word per plane and are converted to pixels together, MSB first, when BPL1DAT is written and not when BPL2DAT-BPL8DAT are written (p.15)");
}

}  // namespace s3
