// s3_props_b5.h -- proposition B5 (32- and 64-pixel bit-plane words)

#pragma once

#include "s3_props_b34.h"

namespace s3 {

// ---- B5 -----------------------------------------------------------------------
// With FMODE = 0x0001 (fetch increment 4 bytes) and FMODE = 0x0003 (8 bytes) the bytes of one fetch reach the
// display as one 32- resp. 64-pixel word, MSB first, the lowest-address word leftmost.
inline void prop_B5() {
    if (!want("B5")) return;
    struct Mode { const char* name; uint16_t fmode; int g; };
    const Mode modes[2] = {{"32", 0x0001, 2}, {"64", 0x0003, 4}};
    Pre pre("B5");
    bool pc_bpl[2] = {true, true}, pc_stride[2] = {true, true}, pc_words[2] = {true, true};
    long graded[2] = {0, 0}, bad[2] = {0, 0};
    bool ran[2] = {false, false};
    for (int m = 0; m < 2; ++m) {
        const std::string mn = modes[m].name;
        // a mutation names one mode by its suffix, or none (the common preconditions: both modes, plane 1)
        if (g_mutate) {
            const std::string mu = g_mutate;
            const bool is32 = mu.size() >= 3 && mu.compare(mu.size() - 3, 3, "-32") == 0;
            const bool is64 = mu.size() >= 3 && mu.compare(mu.size() - 3, 3, "-64") == 0;
            if ((is32 && m != 0) || (is64 && m != 1)) continue;
        }
        ran[m] = true;
        const int g_words = modes[m].g;
        for (int x = 1; x <= 8; ++x) {
            if (g_mutate && x > 1) break;
            Std o; o.bpu = 8; o.pal = Pal::On; o.dmacon = 0x8380; o.fmode = modes[m].fmode;
            if (mutating(("B5-PC-BPL-" + mn).c_str())) o.dmacon = 0x8280;
            if (mutating(("B5-PC-STRIDE-" + mn).c_str())) o.fmode = 0x0000;
            if (mutating(("B5-PC-WORDS-" + mn).c_str())) o.ddfstop = 0x0040;
            RunSpec s = base_spec(o, "B5");
            auto a = all_z();
            a[x] = P_k(x);
            s.vb = bpl_ptrs(a, 8);
            std::vector<uint16_t> fill(0x2000);
            for (uint32_t i = 0; i < fill.size(); ++i) fill[i] = mutating(("B5-" + mn).c_str()) ? 0 : W3(i);
            s.mem.push_back({P_k(x), fill});
            EventSpec e; e.nvb = 5; s.ev.push_back(e);
            auto r = run(s);
            pre.check(*r, 1, 2, false, true, true);
            Frame f = get_frame(*r, 2);
            if (!f.ok || f.tag.size() < 200) { pc_bpl[m] = false; continue; }
            uint64_t P = line_period(f);
            for (int k = 1; k <= 8; ++k)
                if (k != x && count_in(*r, f.t0, f.t1, P_k(k), P_k(k) + 0x4000) != 0) pc_bpl[m] = false;
            int bad_here = 0, groups_min = 1 << 20, groups_max = 0;
            for (int n = 0x30; n <= 0xFF; ++n) {
                if (!f.tag.count(n)) { pc_bpl[m] = false; break; }
                uint64_t t0 = f.tag.at(n), t1 = line_t1(f, n, P);
                if (count_in(*r, t0, t1, kZ, kZ + 0x4000, 1) < 1) pc_bpl[m] = false;
                std::vector<uint32_t> idx;                 // word index in P_x of each read, in time order
                for (size_t i = acc_from(*r, t0); i < r->acc.size() && r->acc[i].cyc < t1; ++i) {
                    const Acc& ac = r->acc[i];
                    uint32_t b = ac.waddr * 2;
                    if (!ac.wr && b >= P_k(x) && b < P_k(x) + 0x4000) idx.push_back((b - P_k(x)) / 2);
                }
                if (idx.empty()) pc_bpl[m] = false;
                // PC-STRIDE: offsets are multiples of the fetch increment, consecutive reads differ by it
                for (size_t i = 0; i < idx.size(); ++i) {
                    if (idx[i] % uint32_t(g_words) != 0) pc_stride[m] = false;
                    if (i > 0 && idx[i] != idx[i - 1] + uint32_t(g_words)) pc_stride[m] = false;
                }
                groups_min = std::min<int>(groups_min, int(idx.size()));
                groups_max = std::max<int>(groups_max, int(idx.size()));
                if (idx.size() < 3) { pc_words[m] = false; continue; }
                std::vector<uint16_t> mid;
                for (size_t i = 1; i + 1 < idx.size(); ++i)
                    for (int k = 0; k < g_words; ++k) mid.push_back(W3(idx[i] + uint32_t(k)));
                int L = out_line(*r, f, n);
                auto cls = is_col(*r, L, kColON);
                bool ok = find_seq(cls, word_pixels(mid)) >= 0;
                ++graded[m];
                if (!ok) { ++bad[m]; ++bad_here; }
            }
            g.note("B5-" + mn + ": plane " + dec(x) + ": reads of P_x per line " + dec(groups_min) + ".." + dec(groups_max) +
                   ", lines whose output does not contain the fetched words (all but the first and last fetch) in memory order, MSB first, 2 output pixels per bit: " +
                   dec(bad_here) + " of 208");
        }
    }
    pre.print("the B5 runs");
    for (int m = 0; m < 2; ++m) {
        if (!ran[m]) continue;
        const std::string mn = modes[m].name;
        const std::string inc = m == 0 ? "4" : "8";
        g.expect(pc_bpl[m], "B5-PC-BPL-" + mn + ": with BPU = 8 and FMODE = " + (m == 0 ? "0x0001" : "0x0003") +
                 " the port shows reads of P_x and of Z on every line 0x30-0xFF of F2, and no read in the regions of the other seven planes' P_k");
        g.expect(pc_stride[m], "B5-PC-STRIDE-" + mn + ": on every graded line the reads of P_x lie at byte offsets from P_x that are multiples of " + inc +
                 " bytes and consecutive reads differ by exactly " + inc + " bytes (p.23 fetch increment)");
        g.expect(pc_words[m], "B5-PC-WORDS-" + mn + ": at least 3 reads of P_x on every graded line");
    }
    for (int m = 0; m < 2; ++m) {
        if (!ran[m]) continue;
        const std::string mn = modes[m].name;
        const bool pre_all = pre.setup && pre.frame && pre.tag && pre.pixel && pc_bpl[m] && pc_stride[m] && pc_words[m];
        std::string what = m == 0
            ? "B5-32: with FMODE = 0x0001 (p.23: BPAGEM = 0, BPL32 = 1, by 4 bytes, normal CAS, bus width 32) the fetch of plane x advances by 4 bytes and the 4 bytes fetched reach the display as one 32-pixel word, "
              "MSB first, the word at the lower address leftmost (p.15), for every plane x = 1..8 on every graded line"
            : "B5-64: with FMODE = 0x0003 (p.23: BPAGEM = 1, BPL32 = 1, by 8 bytes, double CAS, bus width 32) the fetch of plane x advances by 8 bytes and the 8 bytes fetched reach the display as one 64-pixel word, "
              "MSB first, the words at A, A+2, A+4 and A+6 in this order from the left (p.15), for every plane x = 1..8 on every graded line";
        g.assert_line("B5-" + mn, pre_all, graded[m] > 0 && bad[m] == 0, what);
    }
}

}  // namespace s3
