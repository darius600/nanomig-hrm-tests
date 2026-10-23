// s3_props_s12.h -- propositions S1 and S2 (sprite pointers)

#pragma once

#include "s3_spr.h"

namespace s3 {

// the distinct word addresses read in [base, base + len) during frame f, in time order,
// consecutive repeats collapsed
inline std::vector<uint32_t> list_seq(const RunData& r, const Frame& f, uint32_t base, uint32_t len) {
    std::vector<uint32_t> v;
    for (const auto& p : list_reads(r, f, base, len))
        if (v.empty() || v.back() != p.second) v.push_back(p.second);
    return v;
}

// PC-SPR: the list at `base` with `words` words is read once, in order, in frame f:
// word 0, word 1 once, the data pairs, the end pair. Reads after the end pair do not
// belong to the list and are not graded.
inline bool list_in_order(const RunData& r, const Frame& f, uint32_t base, int words) {
    if (!f.ok) return false;
    auto v = list_seq(r, f, base, 0x800);
    if (int(v.size()) < words) return false;
    for (int i = 0; i < words; ++i) if (v[i] != base + 2u * i) return false;
    return true;
}

// development aid (NMH_DBG): why a list is not read in order
inline void dbg_list_order(const RunData& r, const Frame& f, uint32_t base, int words, const char* tag) {
    if (!std::getenv("NMH_DBG")) return;
    auto v = list_seq(r, f, base, 0x400);
    int bad = -1;
    for (int i = 0; i < words && i < int(v.size()); ++i) if (v[i] != base + 2u * i) { bad = i; break; }
    std::cout << "DBG " << tag << " list 0x" << std::hex << base << std::dec << ": " << v.size() << " distinct reads, want " << words << ", first mismatch " << bad << std::endl;
}

// ---- S1 -----------------------------------------------------------------------
inline void prop_S1() {
    if (!want("S1")) return;
    Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = 0x83A0;
    if (mutating("S1-PC-SPR")) o.dmacon = 0x8380;
    RunSpec s = base_spec(o, "S1");
    std::array<uint32_t, 8> where{};
    for (int x = 0; x < 8; ++x) {
        uint32_t A = T_addr(x + 1);
        where[x] = A + ((mutating("S1") && x == 4) ? 0x100u : 0u);       // the pointer's value (l_5 + 0x0100 for sprite 4)
        std::vector<SprRow> rows(4, SprRow{0x8000, 0x8000});
        s.mem.push_back({where[x], spr_list(0x60, 0x64, sh_of_lores(kXs[x]), false, rows)});
    }
    s.vb = spr_ptrs(where);
    for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
    s.vb_intended = s.vb;
    if (mutating("S1-PC-MOVE")) s.vb[1].val ^= 0x0100;
    EventSpec fin; fin.nvb = 5; s.ev.push_back(fin);
    auto r = run(s);
    Pre pre("S1");
    pre.check(*r, 1, 2, false, false, false);
    Frame f2 = get_frame(*r, 2);
    bool ok = f2.ok;
    bool pc_spr = f2.ok;
    for (int x = 0; x < 8; ++x) {
        uint32_t A = T_addr(x + 1);
        const Acc* a = f2.ok ? first_list_read(*r, f2, A) : nullptr;
        bool hit = a && !a->wr && a->waddr == A / 2;
        ok = ok && hit;
        g.note("S1: sprite " + dec(x) + ": first access in F2 to [A_" + dec(x + 1) + ", +0x20): " +
               (a ? std::string(a->wr ? "write" : "read") + " at word address 0x" + hexs(a->waddr, 6) : std::string("none")) +
               ", expected read at 0x" + hexs(A / 2, 6));
        if (!list_in_order(*r, f2, where[x], 12)) pc_spr = false;
    }
    bool pc_move = moves_fetched(*r, f2, intended_vb(*r), r->lay.vb_data, 0, 16);
    pre.print("the S1 run");
    g.expect(pc_move, "S1-PC-MOVE: the sixteen sprite-pointer MOVEs are fetched in F2's vertical-blank block with the intended data words");
    g.expect(pc_spr, "S1-PC-SPR: in F2 each of the eight sprite lists is read in order: word 0, word 1 once, the four data pairs and the end pair");
    g.assert_line("S1", pre.setup && pre.frame && pc_move && pc_spr, ok,
        "S1: opus:AA-sprxpth.c1 opus:AA-sprxpth.c2 opus:AA-sprxpth.c4 sol:AA-sprxpth.c1 sol:AA-sprxpth.c2 sol:AA-sprxpth.c12 opus:AA-sprxptl.c1 opus:AA-sprxptl.c2 opus:AA-sprxptl.c4 sol:AA-sprxptl.c1 sol:AA-sprxptl.c2 sol:AA-sprxptl.c12: "
        "SPRxPTH and SPRxPTL (p.28) hold the high 5 and the low 15 bits of the address of sprite x's DMA data: the first access in F2 to the list region of every sprite x = 0..7 is a read at A_(x+1) / 2");
}

// ---- S2 -----------------------------------------------------------------------
inline void prop_S2() {
    if (!want("S2")) return;
    const uint32_t LA = L_x(0), LB = L_alt(0);
    const uint32_t LB_int = mutating("S2") ? LA : LB;       // the list the stimulus points to in F3
    auto mk = [&](bool cpu) {
        Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = 0x83A0;
        if (mutating("S2-PC-SPR")) o.dmacon = 0x8380;
        RunSpec s = base_spec(o, "S2");
        std::vector<SprRow> rows(8, SprRow{0x8000, 0x8000});
        s.mem.push_back({LA, spr_list(0x60, 0x68, sh_of_lores(0x90), false, rows)});
        s.mem.push_back({LB, spr_list(0x60, 0x68, sh_of_lores(0x90), false, rows)});
        EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
        std::array<uint32_t, 8> where = own_lists();
        if (!cpu) {
            where[0] = LA;
            s.vb = spr_ptrs(where);
            for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
            s.vb_intended = s.vb;
            EventSpec e; e.nvb = 2; e.line = 0x100; e.h = 0x08;
            if (mutating("S2-PC-MOVE")) { e.nvb = 3; e.line = 0x40; }
            e.mw.push_back({kVbData0, uint16_t((LB_int >> 16) & 0x1F)});
            e.mw.push_back({kVbData0 + 4, uint16_t(LB_int & 0xFFFE)});
            s.ev.push_back(e);
            EventSpec fin; fin.nvb = 3; s.ev.push_back(fin);
        } else {
            for (int x = 1; x < 8; ++x) push_ptr(s.vb, R_SPRxPTH(x), R_SPRxPTL(x), where[x]);
            for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
            for (int f = 1; f <= 4; ++f) {
                EventSpec e; e.nvb = 1;
                if (mutating("S2-PC-MARKER")) { e.line = 0x80; e.h = 0x08; }
                push_ptr(e.wr, R_SPRxPTH(0), R_SPRxPTL(0), f <= 2 ? LA : LB_int);
                s.ev.push_back(e);
            }
            EventSpec fin; fin.nvb = 1; s.ev.push_back(fin);
        }
        return s;
    };
    Pre pre("S2");
    bool ok_all = true, pc_spr = true, pc_move = true, pc_marker = true;
    for (int variant = 0; variant < 2; ++variant) {
        bool cpu = variant == 1;
        if (g_mutate && !cpu && mutating("S2-PC-MARKER")) continue;
        if (g_mutate && cpu && mutating("S2-PC-MOVE")) continue;
        auto r = run(mk(cpu));
        pre.check(*r, 1, 3, false, false, false);
        std::string vn = cpu ? "CPU variant" : "Copper variant";
        for (int fi = 2; fi <= 3; ++fi) {
            Frame f = get_frame(*r, fi);
            if (!f.ok) { ok_all = false; continue; }
            const Acc* a0 = first_in(*r, f.t0, f.t1, LA, LA + 0x20);
            const Acc* a1 = first_in(*r, f.t0, f.t1, LB, LB + 0x20);
            const Acc* a = a0;
            if (a1 && (!a || a1->cyc < a->cyc)) a = a1;
            uint32_t want_w = (fi == 2 ? LA : LB) / 2;
            g.note("S2 " + vn + ": F" + dec(fi) + " first read in the list areas at word address 0x" + hexs(a ? a->waddr : 0, 6) + ", expected 0x" + hexs(want_w, 6));
            if (!(a && !a->wr && a->waddr == want_w)) ok_all = false;
            uint32_t intended_list = fi == 2 ? LA : LB_int;
            {
                bool oa = list_in_order(*r, f, LA, 2 + 2 * 8 + 2), ob = list_in_order(*r, f, LB, 2 + 2 * 8 + 2);
                bool na = list_seq(*r, f, LA, 0x100).empty(), nb = list_seq(*r, f, LB, 0x100).empty();
                if (!((oa && nb) || (ob && na))) pc_spr = false;
            }
            if (!cpu) {
                std::vector<Wr> iv;
                push_ptr(iv, R_SPRxPTH(0), R_SPRxPTL(0), intended_list);
                bool m = moves_fetched(*r, f, iv, r->lay.vb_data, 0, 2);
                auto t = first_clread(*r, r->lay.vb_data[1], f.t0, f.t1);
                m = m && t && (!a || *t < a->cyc);
                if (!m) pc_move = false;
            } else {
                uint64_t tm = marker_time(*r, fi);
                if (!(tm > f.t0 && (!a || tm < a->cyc))) pc_marker = false;
            }
        }
    }
    pre.print("the S2 runs");
    g.expect(pc_spr, "S2-PC-SPR: in F2 and in F3 exactly one of the two lists (A or B) is read, once, in order: word 0, word 1 once, the eight data pairs and the end pair");
    g.expect(pc_move, "S2-PC-MOVE: in the Copper variant the SPR0PTH and SPR0PTL data words of F2 and F3 are fetched after t_F(i), before the first list read of that frame, and carry list A (F2) and list B (F3)");
    g.expect(pc_marker, "S2-PC-MARKER: in the CPU variant the marker of F2 and F3 lies after t_F(i) and before the first list read of that frame");
    g.assert_line("S2", pre.setup && pre.frame && pc_spr && pc_move && pc_marker, ok_all,
        "S2: a sprite pointer written by the processor or the coprocessor during vertical blank determines where sprite 0's list is read in the following frame (p.28): in both variants the first read in F2 is at list A's word 0 and the first read in F3 is at list B's word 0");
}

}  // namespace s3
