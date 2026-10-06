// s3_props_b.h -- propositions B1-B4 (bit planes)

#pragma once

#include "s3_vid.h"

namespace s3 {


// ---- B1 -----------------------------------------------------------------------
// Bit-plane pointers: addresses, and the address they hold.
void prop_B1() {
    if (!want("B1")) return;
    auto mk = [&](int bpu) {
        Std o; o.bpu = bpu; o.dmacon = 0x8380;
        if (mutating("B1-b") && bpu == 3) o.bpu = 4;
        if (mutating("B1-PC-BPL")) o.dmacon = 0x8280;
        RunSpec s = base_spec(o, "B1");
        std::array<uint32_t, 9> a{};
        for (int x = 1; x <= 8; ++x) a[x] = T_addr(x);
        if (mutating("B1")) a[3] += 0x100;
        s.vb = bpl_ptrs(a, 8);
        s.vb_intended = s.vb;
        if (mutating("B1-PC-MOVE")) s.vb[1].val ^= 0x0100;
        if (mutating("B1-PC-STATIC")) s.setup.insert(s.setup.begin() + 1, Wr{R_BPLxPTH(1), 0x0011});
        EventSpec e; e.nvb = 5; s.ev.push_back(e);
        return s;
    };
    auto first_reads = [&](const RunData& r, std::array<const Acc*, 9>& fa, std::array<bool, 9>& anyread) {
        Frame f = get_frame(r, 2);
        for (int x = 1; x <= 8; ++x) {
            uint32_t A = T_addr(x);
            fa[x] = f.ok ? first_in(r, f.t0, f.t1, A, A + 0x3000) : nullptr;
            anyread[x] = fa[x] != nullptr && !fa[x]->wr;
        }
    };

    Pre pre("B1");
    auto r8 = run(mk(8));
    pre.check(*r8, 1, 2, false, false, false);
    std::array<const Acc*, 9> fa{};
    std::array<bool, 9> rd{};
    first_reads(*r8, fa, rd);
    Frame f2 = get_frame(*r8, 2);

    bool a_ok = f2.ok;
    for (int x = 1; x <= 8; ++x) {
        bool hit = fa[x] && !fa[x]->wr && fa[x]->waddr == T_addr(x) / 2;
        a_ok = a_ok && hit;
        g.note("B1 (a): plane " + dec(x) + ": first access in F2 to T_" + dec(x) + " " +
               (fa[x] ? std::string(fa[x]->wr ? "write" : "read") + " at word address 0x" + hexs(fa[x]->waddr, 6)
                      : std::string("none")) + ", expected read at 0x" + hexs(T_addr(x) / 2, 6));
    }

    // PC-MOVE: the sixteen pointer MOVEs are fetched in F2 with the intended values before the first T_x read
    bool pc_move = moves_fetched(*r8, f2, intended_vb(*r8), r8->lay.vb_data);
    for (size_t i = 0; i < r8->lay.vb_data.size() && pc_move; ++i) {
        int x = int(i / 2) + 1;
        auto t = first_clread(*r8, r8->lay.vb_data[i], f2.t0, f2.t1);
        pc_move = t && (!fa[x] || *t < fa[x]->cyc);
    }
    // PC-BPL({1..8}): reads of every plane's region on every graded line, no reads outside the set
    auto pc_bpl_of = [&](const RunData& r, int n) {
        Frame f = get_frame(r, 2);
        if (!f.ok) return false;
        for (int x = 1; x <= 8; ++x) {
            uint32_t A = T_addr(x);
            if (x <= n) {
                for (int ln = 0x30; ln <= 0xFE; ++ln) {
                    if (!f.tag.count(ln) || !f.tag.count(ln + 1)) return false;
                    if (count_in(r, f.tag.at(ln), f.tag.at(ln + 1), A, A + 0x3000, 1) < 1) return false;
                }
            } else if (count_in(r, f.t0, f.t1, A, A + 0x3000) != 0) return false;
        }
        return true;
    };
    bool pc_bpl = pc_bpl_of(*r8, 8);
    bool pc_static = static_no_write(r8->spec, 0x0E0, 0x0FE);

    // (b): with BPU = n, T_x is read in F2 if and only if x <= n
    bool b_ok = true;
    bool b_run_ok = true;
    for (int n = 0; n <= 8; ++n) {
        if (g_mutate && !mutating("B1-b")) break;
        std::shared_ptr<RunData> rn;
        if (n == 8) rn = r8; else rn = run(mk(n));
        if (n != 8) pre.check(*rn, 1, 2, false, false, false);
        std::array<const Acc*, 9> fn{};
        std::array<bool, 9> rdn{};
        first_reads(*rn, fn, rdn);
        std::string got;
        for (int x = 1; x <= 8; ++x) {
            bool should = x <= n;
            bool did = rdn[x];
            if (did != should) b_ok = false;
            got += did ? '1' : '0';
        }
        if (n != 8 && !pc_bpl_of(*rn, n)) b_run_ok = false;
        g.note("B1 (b): BPU = " + dec(n) + ": planes read in F2 (plane 1 first) " + got);
    }
    (void)b_run_ok;

    pre.print("the B1 runs");
    g.expect(pc_move, "B1-PC-MOVE: the sixteen pointer MOVEs are fetched in F2's vertical-blank block with the table values, before the first T_x read");
    g.expect(pc_bpl, "B1-PC-BPL: with BPU = 8 the port shows reads in the region of every plane 1..8 on every line 0x30-0xFE of F2");
    g.expect(pc_static, "B1-PC-STATIC: the program contains no write to BPLxPTH/BPLxPTL (0x0E0-0x0FE) after setup");
    bool pre_all = pre.setup && pre.frame && pre.tag && pc_move && pc_bpl && pc_static;
    g.assert_line("B1", pre_all, a_ok && b_ok,
        "B1: opus:AA-bplxpth.c1 opus:AA-bplxpth.c2 opus:AA-bplxpth.c4 opus:AA-bplxpth.c6 opus:AA-bplxpth.c9 sol:AA-bplxpth.c1 sol:AA-bplxpth.c2 sol:AA-bplxpth.c3 sol:AA-bplxpth.c4 sol:AA-bplxpth.c5 sol:AA-bplxpth.c6 sol:AA-bplxpth.c7 sol:AA-bplxpth.c8 sol:AA-bplxpth.c9 sol:AA-bplxpth.c22 opus:AA-bplxptl.c1 opus:AA-bplxptl.c2 opus:AA-bplxptl.c4 opus:AA-bplxptl.c6 opus:AA-bplxptl.c10 sol:AA-bplxptl.c1 sol:AA-bplxptl.c2 sol:AA-bplxptl.c3 sol:AA-bplxptl.c4 sol:AA-bplxptl.c5 sol:AA-bplxptl.c6 sol:AA-bplxptl.c7 sol:AA-bplxptl.c8 sol:AA-bplxptl.c9 sol:AA-bplxptl.c11 sol:AA-bplxptl.c23: "
        "BPLxPTH/BPLxPTL (p.15) hold the address from which bit plane x's DMA data are fetched: with BPU = 8 the first access in F2 to T_x is a read at A_x / 2 for every x = 1..8, and with BPU = n for n = 0..8 plane x is read in F2 if and only if x <= n");
}

// ---- B2 -----------------------------------------------------------------------
// Bit-plane pointer re-initialisation in vertical blank.
void prop_B2() {
    if (!want("B2")) return;
    const uint32_t A_a = 0x090000, A_b = 0x0A8000;
    const uint32_t Ab_int = mutating("B2") ? A_a : A_b;       // the address the stimulus writes for F3
    auto mk = [&](bool cpu) {
        Std o; o.bpu = 1; o.dmacon = 0x8380;
        if (mutating("B2-PC-BPL")) o.dmacon = 0x8280;
        RunSpec s = base_spec(o, "B2");
        uint32_t Ab = Ab_int;
        EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
        if (!cpu) {
            push_ptr(s.vb, R_BPLxPTH(1), R_BPLxPTL(1), A_a);
            s.vb_intended = s.vb;
            EventSpec e; e.nvb = 2; e.line = 0x100; e.h = 0x08;
            if (mutating("B2-PC-MOVE")) { e.nvb = 3; e.line = 0x40; }
            e.mw.push_back({vb_data(0), uint16_t((Ab >> 16) & 0x1F)});
            e.mw.push_back({vb_data(1), uint16_t(Ab & 0xFFFE)});
            s.ev.push_back(e);
            EventSpec fin; fin.nvb = 3; s.ev.push_back(fin);
        } else {
            for (int f = 1; f <= 4; ++f) {
                EventSpec e; e.nvb = 1;
                if (mutating("B2-PC-MARKER")) { e.line = 0x80; e.h = 0x08; }
                uint32_t A = f <= 2 ? A_a : Ab;
                push_ptr(e.wr, R_BPLxPTH(1), R_BPLxPTL(1), A);
                s.ev.push_back(e);
            }
            EventSpec fin; fin.nvb = 1; s.ev.push_back(fin);
        }
        return s;
    };

    Pre pre("B2");
    bool ok_all = true, pc_bpl = true, pc_move = true, pc_marker = true, pc_static = true;
    for (int variant = 0; variant < 2; ++variant) {
        bool cpu = variant == 1;
        if (g_mutate && !cpu && (mutating("B2-PC-MARKER"))) continue;
        if (g_mutate && cpu && mutating("B2-PC-MOVE")) continue;
        auto r = run(mk(cpu));
        pre.check(*r, 1, 3, false, false, false);
        std::string vn = cpu ? "CPU variant" : "Copper variant";
        const uint32_t area[2] = {A_a, A_b};
        uint32_t first_word[4] = {0, 0, 0, 0};
        bool got[4] = {false, false, false, false};
        for (int fi = 2; fi <= 3; ++fi) {
            Frame f = get_frame(*r, fi);
            if (!f.ok) { ok_all = false; continue; }
            const Acc* a0 = first_in(*r, f.t0, f.t1, area[0], area[0] + 0x3000);
            const Acc* a1 = first_in(*r, f.t0, f.t1, area[1], area[1] + 0x3000);
            const Acc* a = a0;
            if (a1 && (!a || a1->cyc < a->cyc)) a = a1;
            got[fi] = a && !a->wr;
            first_word[fi] = a ? a->waddr : 0;
            g.note("B2 " + vn + ": F" + dec(fi) + " first read in the plane areas at word address 0x" + hexs(first_word[fi], 6) +
                   ", expected 0x" + hexs((fi == 2 ? A_a : A_b) / 2, 6));
            // PC-BPL: the plane's own area is read on every line 0x30-0xFE; the planes outside the set are not
            for (int ln = 0x30; ln <= 0xFE && pc_bpl; ++ln) {
                if (!f.tag.count(ln) || !f.tag.count(ln + 1)) { pc_bpl = false; break; }
                int n1 = count_in(*r, f.tag.at(ln), f.tag.at(ln + 1), A_a, A_a + 0x3000, 1) +
                         count_in(*r, f.tag.at(ln), f.tag.at(ln + 1), A_b, A_b + 0x3000, 1);
                if (n1 < 1) pc_bpl = false;
            }
            for (int x = 2; x <= 8; ++x)
                if (count_in(*r, f.t0, f.t1, T_addr(x), T_addr(x) + 0x3000) != 0) pc_bpl = false;
            if (!(got[fi] && first_word[fi] == (fi == 2 ? A_a : A_b) / 2)) ok_all = false;
            // variant preconditions
            if (!cpu) {
                std::vector<Wr> iv;
                push_ptr(iv, R_BPLxPTH(1), R_BPLxPTL(1), fi == 2 ? A_a : Ab_int);
                bool m = moves_fetched(*r, f, iv, r->lay.vb_data);
                auto t = first_clread(*r, r->lay.vb_data[1], f.t0, f.t1);
                m = m && t && (!a || *t < a->cyc);
                if (!m) pc_move = false;
            } else {
                uint64_t tm = marker_time(*r, fi);
                if (!(tm > f.t0 && (!a || tm < a->cyc))) pc_marker = false;
                g.note("B2 CPU variant: marker of F" + dec(fi) + " at t_F + " + dec(long(tm) - long(f.t0)) + " cycles");
            }
        }
        if (!static_no_write(r->spec, 0x0E0, 0x0FE) && !cpu) pc_static = false;
    }
    pre.print("the B2 runs");
    bool have_c = !g_mutate || !mutating("B2-PC-MARKER");
    bool have_p = !g_mutate || !mutating("B2-PC-MOVE");
    (void)have_c; (void)have_p;
    g.expect(pc_bpl, "B2-PC-BPL: the port shows reads of plane 1's data (the area A_a or A_b) on every line 0x30-0xFE of F2 and F3, and no reads in the regions of planes 2-8");
    g.expect(pc_move, "B2-PC-MOVE: in the Copper variant the BPL1PTL data word of F2 and F3 is fetched after t_F(i), before the first plane-1 read of that frame, and carries A_a (F2) and A_b (F3)");
    g.expect(pc_marker, "B2-PC-MARKER: in the CPU variant the marker of F2 and F3 lies after t_F(i) and before the first plane-1 read of that frame");
    bool pre_all = pre.setup && pre.frame && pc_bpl && pc_move && pc_marker;
    g.assert_line("B2", pre_all, ok_all,
        "B2: opus:AA-bplxptl.c7 sol:AA-bplxptl.c12: a bit-plane pointer written by the processor or the coprocessor during vertical blank determines where plane 1's fetch begins in the frame that follows (p.15): in both variants the first read in F2 is at A_a / 2 and the first read in F3 is at A_b / 2");
}


}  // namespace s3
