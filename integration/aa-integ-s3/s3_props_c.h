// s3_props_c.h -- propositions C1-C5 (collision control), the collision frame procedure CF of 2.10

#pragma once

#include "s3_props_s89.h"

namespace s3 {

// ---- the collision frame procedure ----------------------------------------------
struct CfPlan {
    std::vector<std::vector<Wr>> per_frame;     // CPU writes at the start of each observed frame (may be empty)
    std::vector<std::pair<int, int>> sprites;   // (sprite, data rows) whose reads bound the CF window
    int line1 = 0x40, line2 = 0x90, h = 0x08;
    int first_frame = 2;
    // filled by cf_events
    std::vector<int> ev_cf1, ev_cf2;
};

// adds the events of CF for every observed frame to s: [CPU writes at VERTB], CF1 (line1), CF2 (line2)
inline void cf_events(RunSpec& s, CfPlan& c) {
    EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
    for (size_t j = 0; j < c.per_frame.size(); ++j) {
        bool wr = !c.per_frame[j].empty();
        int nvb = j == 0 ? c.first_frame : 1;
        if (wr) {
            EventSpec w; w.nvb = nvb; w.wr = c.per_frame[j]; s.ev.push_back(w);
            nvb = 0;
        }
        EventSpec a; a.nvb = nvb; a.line = c.line1; a.h = c.h; a.rd.push_back(R_CLXDAT);
        c.ev_cf1.push_back(int(s.ev.size())); s.ev.push_back(a);
        EventSpec b; b.line = c.line2; b.h = c.h; b.rd.push_back(R_CLXDAT);
        c.ev_cf2.push_back(int(s.ev.size())); s.ev.push_back(b);
    }
    EventSpec fin; fin.nvb = 1; s.ev.push_back(fin);
}

struct CfResult {
    std::vector<uint16_t> val;      // the value read by CF2 of each observed frame
    bool beam_ok = true;            // every beam marker's re-read shows its target line, inside its frame
    bool window_ok = true;          // CF1 before every sprite's first data read, CF2 after every end-pair read, both markers inside the frame
    int beam_n = 0, beam_off = 0;   // beam markers, and those whose re-read does not show the target line
};

inline CfResult cf_collect(const RunData& r, const CfPlan& c, int line1_expect, int line2_expect) {
    CfResult o;
    for (size_t j = 0; j < c.ev_cf1.size(); ++j) {
        Frame f = get_frame(r, c.first_frame + int(j));
        if (!f.ok) { o.beam_ok = o.window_ok = false; o.val.push_back(0xFFFF); continue; }
        int e1 = c.ev_cf1[j], e2 = c.ev_cf2[j];
        int want_line[2] = {line1_expect, line2_expect};
        int evs[2] = {e1, e2};
        for (int q = 0; q < 2; ++q) {
            uint16_t vp = r.ev_word(evs[q], 2), vh = r.ev_word(evs[q], 4);
            int line = ((vp & 7) << 8) | (vh >> 8);
            uint64_t tm = marker_time(r, evs[q]);
            ++o.beam_n;
            if (line != want_line[q]) ++o.beam_off;
            if (line != want_line[q] || !(tm >= f.t0 && tm < f.t1)) {
                o.beam_ok = false;
                if (std::getenv("NMH_DBG")) std::cout << "DBG cf beam: frame " << (c.first_frame + int(j)) << " event " << evs[q] << " want line 0x" << std::hex << want_line[q] << " got 0x" << line
                                                       << " h 0x" << (vh & 0xFF) << std::dec << " marker rel " << (long(tm) - long(f.t0)) << " of " << (long(f.t1) - long(f.t0)) << std::endl;
            }
        }
        o.val.push_back(r.ev_word(e2, 6));
        uint64_t t1 = marker_time(r, e1), t2 = marker_time(r, e2);
        for (const auto& sp : c.sprites) {
            auto a = first_read_at(r, L_x(sp.first) + 4, f.t0, f.t1);
            auto b = first_read_at(r, L_x(sp.first) + 4 + 4 * uint32_t(sp.second), f.t0, f.t1);
            if (!a || !b || !(t1 < *a) || !(t2 > *b) || !(t1 >= f.t0) || !(t2 < f.t1)) o.window_ok = false;
        }
    }
    return o;
}

inline std::vector<uint16_t> fill_words(uint16_t w) { return std::vector<uint16_t>(0x2000, w); }

// a DMA sprite list of 16 rows DATA, DATB = 0 from `start` at lores X
inline std::vector<uint16_t> col_list(int start, int X, uint16_t data = 0xFFFF) {
    std::vector<SprRow> rows(16, SprRow{data, 0x0000});
    return spr_list(start, start + 16, sh_of_lores(X), false, rows);
}

inline uint16_t clx_bit(int bit) { return uint16_t(1 << bit); }

// ---- C1 -----------------------------------------------------------------------
inline void prop_C1() {
    if (!want("C1")) return;
    Pre pre("C1");
    bool pc_spr = true, pc_window = true, pc_beam = true, pc_ctrl = true, pc_union = true, ok = true;
    const int line2 = mutating("C1-PC-WINDOW") ? 0x68 : 0x90;
    const int hb = mutating("C1-PC-BEAM") ? 0xE0 : 0x08;
    // a: the sprite over b (a at X = 0xA0, b at X = 0xA8); or_alone: the sprite alone at X = 0xD0 (OR runs)
    struct Run1 { std::string name; int a, b, or_alone, bit; uint16_t clxcon; bool want; };
    std::vector<Run1> runs;
    struct Sc { const char* nm; int a, b, bit, ensp; };
    const Sc scs[4] = {{"ENSP1", 1, 2, 9, 12}, {"ENSP3", 3, 0, 9, 13}, {"ENSP5", 5, 0, 10, 14}, {"ENSP7", 7, 0, 11, 15}};
    for (const auto& sc : scs) {
        runs.push_back({std::string(sc.nm) + " set", sc.a, sc.b, -1, sc.bit, uint16_t(1 << sc.ensp), true});
        // the clear run: the other three ENSP bits set and this one clear; the contrast sets the bit in it as well
        runs.push_back({std::string(sc.nm) + " clear", sc.a, sc.b, -1, sc.bit, uint16_t(mutating("C1") ? (1 << sc.ensp) : (0xF000 & ~(1 << sc.ensp))), false});
        if (g_mutate) break;
    }
    if (!g_mutate) {
        runs.push_back({"OR(i) ENSP1 set", 1, 2, 0, 9, 0x1000, true});
        runs.push_back({"OR(i) ENSP1 clear", 1, 2, 0, 9, 0x0000, false});
        runs.push_back({"OR(ii) ENSP1 set", 0, 2, 1, 9, 0x1000, true});
        runs.push_back({"OR(ii) ENSP1 clear", 0, 2, 1, 9, 0x0000, true});
    }
    for (const auto& rn : runs) {
        Std o; o.bpu = 1; o.pal = Pal::Spr; o.dmacon = mutating("C1-PC-SPR") ? 0x8380 : 0x83A0;
        o.extra.push_back({R_CLXCON, rn.clxcon});
        RunSpec s = base_spec(o, "C1");
        int used[8] = {0};
        auto put = [&](int x, int start, int X, uint16_t data = 0xFFFF) { s.mem.push_back({L_x(x), col_list(start, X, data)}); used[x] = 1; };
        if (rn.or_alone >= 0) put(rn.or_alone == 0 ? 0 : 1, 0x60, 0xD0);
        put(rn.a, 0x60, 0xA0, mutating("C1-PC-UNION") ? uint16_t(0x0000) : uint16_t(0xFFFF));
        put(rn.b, 0x60, 0xA8);
        put(4, 0x78, 0xA0);
        put(6, 0x78, mutating("C1-PC-CTRL") ? 0xD0 : 0xA8);
        s.vb = spr_ptrs(own_lists());
        for (const auto& w : bpl_ptrs(all_z(), 1)) s.vb.push_back(w);
        CfPlan plan; plan.line2 = line2; plan.h = hb;
        plan.per_frame = {{}};
        for (int x = 0; x < 8; ++x) if (used[x]) plan.sprites.push_back({x, 16});
        cf_events(s, plan);
        auto r = run(s);
        pre.check(*r, 1, 2, false, true, true);
        CfResult cr = cf_collect(*r, plan, 0x40, line2);
        Frame f = get_frame(*r, 2);
        if (!cr.beam_ok) pc_beam = false;
        if (!cr.window_ok) pc_window = false;
        bool lists_ok = f.ok;
        for (int x = 0; x < 8 && lists_ok; ++x) if (used[x] && !list_in_order(*r, f, L_x(x), 2 + 2 * 16 + 2)) lists_ok = false;
        if (!lists_ok) pc_spr = false;
        uint16_t v = cr.val.empty() ? 0 : cr.val[0];
        if (!((v >> 14) & 1)) pc_ctrl = false;
        // the union of the overlapping pair is a 24-lores-pixel (48 output pixels) cS span on every line 0x60-0x6F
        FrameView fv(*r, 2);
        if (!fv.ok) pc_union = false;
        else for (int n = 0x60; n <= 0x6F; ++n) {
            bool has48 = false;
            for (const auto& q : runs_of(fv.cls(n, kColS))) if (q.second == 48) has48 = true;
            if (!has48) pc_union = false;
        }
        bool bit = (v >> rn.bit) & 1;
        g.note("C1: " + rn.name + ": CLXDAT = 0x" + hexs(v) + ", graded bit " + dec(rn.bit) + " = " + dec(bit) + " (page: " + dec(rn.want) + ")");
        ok = ok && bit == rn.want;
    }
    pre.print("the C1 runs");
    g.expect(pc_spr, "C1-PC-SPR: in F2 of every run the list of every displayed sprite is read in order up to its end pair");
    g.expect(pc_beam, "C1-PC-BEAM: in every run both beam markers' re-reads show their target lines, inside F2");
    g.expect(pc_window, "C1-PC-WINDOW: in every run the first CF read lies inside F2 and before every involved sprite's first data read and the result read lies inside F2 and after every end-pair read");
    g.expect(pc_ctrl, "C1-PC-CTRL: in every run CLXDAT bit 14 (sprite 4 to sprite 6, both even and always included) is 1");
    g.expect(pc_union, "C1-PC-UNION: in every run the union of each overlapping pair is visible as a 48-output-pixel cS span on every line 0x60-0x6F");
    g.assert_line("C1", pre.setup && pre.frame && pre.tag && pre.pixel && pc_spr && pc_beam && pc_window && pc_ctrl && pc_union, ok,
        "C1: opus:AA-clxcon.c5 opus:AA-clxcon.c7 opus:AA-clxcon.c8 opus:AA-clxcon.c9 opus:AA-clxcon.c10 sol:AA-clxcon.c5 sol:AA-clxcon.c7 sol:AA-clxcon.c8 sol:AA-clxcon.c9 sol:AA-clxcon.c10: "
        "CLXCON bits 15-12 (ENSP7, ENSP5, ENSP3, ENSP1) each include one odd sprite in collision detection, ORed with its even partner, and each bit includes only its own sprite (p.18): "
        "the graded CLXDAT bit is 1 when the scenario's ENSP bit is set and 0 when it is clear while the other three are set, and in the OR runs bit 9 follows ENSP1 for sprite 1 and is 1 for both values when the even sprite 0 overlaps");
}

// ---- C2 -----------------------------------------------------------------------
// sprite 0 on lines 0x60-0x6F at X = 0xA0, DATA = 0xFFFF; the planes of the scenario point to P_k or Z
inline RunSpec collision_spec(const std::string& pfx, const std::array<uint32_t, 9>& planes, bool bplcon4,
                              const std::vector<std::pair<int, uint16_t>>& fills, const std::vector<Wr>& extra, CfPlan& plan,
                              uint16_t spr_data = 0xFFFF, bool spren = true) {
    Std o; o.bpu = 8; o.dmacon = spren ? 0x83A0 : 0x8380; o.write_bplcon4 = bplcon4;
    for (const auto& w : extra) o.extra.push_back(w);
    RunSpec s = base_spec(o, pfx);
    s.mem.push_back({L_x(0), col_list(0x60, 0xA0, spr_data)});
    for (const auto& f : fills) s.mem.push_back({P_k(f.first), fill_words(f.second)});
    s.vb = spr_ptrs(own_lists());
    for (const auto& w : bpl_ptrs(planes, 8)) s.vb.push_back(w);
    plan.sprites = {{0, 16}};
    cf_events(s, plan);
    return s;
}

struct C2Row { int enbp, mvbp, want; };

// the table of C2 (and C3): per frame the enable and match value and the expected group bit
inline std::vector<C2Row> c2_rows(int data) {
    if (data == 1) return {{1, 1, 1}, {1, 0, 0}, {0, 0, 1}, {0, 1, 1}};
    return {{1, 0, 1}, {1, 1, 0}};
}

// the preconditions every C2-like run contributes to
struct CPc {
    bool beam = true, window = true, spr = true, other = true;
    int beam_n = 0, beam_off = 0;
    // req[j]: the other group's bit must be 1 in observed frame j (default: in every frame)
    void absorb(const RunData& r, const CfPlan& plan, const CfResult& cr, int own_bit, int other_bit, const std::vector<char>* req = nullptr) {
        if (!cr.beam_ok) beam = false;
        if (!cr.window_ok) window = false;
        beam_n += cr.beam_n;
        beam_off += cr.beam_off;
        for (size_t j = 0; j < plan.ev_cf1.size(); ++j) {
            Frame f = get_frame(r, plan.first_frame + int(j));
            if (!f.ok || !list_in_order(r, f, L_x(0), 2 + 2 * 16 + 2)) spr = false;
            uint16_t val = j < cr.val.size() ? cr.val[j] : 0;
            bool needed = !req || (j < req->size() ? (*req)[j] != 0 : true);
            if (other_bit >= 0 && needed && !((val >> other_bit) & 1)) other = false;
        }
        (void)own_bit;
    }
    // with_beam: the re-read line is a graded precondition (C1, C4); otherwise it is a MEASURED line and the window is the precondition
    void print(const std::string& p, const std::string& where, bool with_other = true, bool with_beam = true) {
        g.expect(spr, p + "-PC-SPR: in every observed frame of " + where + " sprite 0's list is read in order up to its end pair");
        if (with_beam)
            g.expect(beam, p + "-PC-BEAM: in every observed frame of " + where + " both beam markers' re-reads show their target lines, inside the frame");
        else
            g.note(p + ": " + dec(beam_n - beam_off) + " of " + dec(beam_n) + " beam markers of " + where + " re-read the target line (the re-read line is measured, not graded; " + p + "-PC-WINDOW grades the position of the two reads)");
        g.expect(window, p + "-PC-WINDOW: in every observed frame of " + where + " the first CF read lies inside the frame and before sprite 0's first data read, and the result read lies inside the frame and after its end-pair read");
        if (with_other) g.expect(other, p + "-PC-OTHER: in every observed frame of " + where + " the other group's CLXDAT bit (5 for odd planes, 1 for even planes) is 1, where the plane under test is odd, disabled or matching");
    }
    bool all(bool with_beam = true) const { return (!with_beam || beam) && window && spr && other; }
};

inline void prop_C2() {
    if (!want("C2")) return;
    Pre pre("C2");
    CPc pc;
    bool ok = true;
    for (int k = 1; k <= 6; ++k) {
        if (g_mutate && k != 3) continue;
        for (int data = 1; data >= 0; --data) {
            auto rows = c2_rows(data);
            CfPlan plan;
            if (mutating("C2-PC-WINDOW")) plan.line2 = 0x68;
            for (size_t j = 0; j < rows.size(); ++j) {
                int enbp = rows[j].enbp, mvbp = rows[j].mvbp;
                if (mutating("C2") && data == 1 && j == 1) mvbp = 1;                 // F3: MVBP = 1 instead of 0
                uint16_t v = 0;
                if (enbp) v |= clx_bit(5 + k);
                if (mvbp) v |= clx_bit(k - 1);
                plan.per_frame.push_back({{R_CLXCON, v}});
            }
            std::array<uint32_t, 9> planes = all_z();
            std::vector<std::pair<int, uint16_t>> fills;
            if (data == 1) { planes[k] = P_k(k); fills.push_back({k, 0xFFFF}); }
            RunSpec s = collision_spec("C2", planes, true, fills, {}, plan, mutating("C2-PC-OTHER") ? uint16_t(0x0000) : uint16_t(0xFFFF), !mutating("C2-PC-SPR"));
            auto r = run(s);
            pre.check(*r, 2, 1 + int(rows.size()), false, false, false);
            CfResult cr = cf_collect(*r, plan, 0x40, plan.line2);
            int own = (k & 1) ? 1 : 5, other = (k & 1) ? 5 : 1;
            // for an even plane the other (odd) group's bit is demanded in the frames where the plane is disabled or matches; the pages do not say whether it may fall with a mismatching enabled even plane
            std::vector<char> req;
            for (const auto& rw : rows) req.push_back((k & 1) || !(rw.enbp && data != rw.mvbp));
            pc.absorb(*r, plan, cr, own, other, &req);
            for (size_t j = 0; j < rows.size(); ++j) {
                uint16_t val = j < cr.val.size() ? cr.val[j] : 0;
                bool bit = (val >> own) & 1;
                if (bit != (rows[j].want == 1)) ok = false;
                g.note("C2: plane " + dec(k) + ", data " + dec(data) + ", F" + dec(2 + int(j)) + ": ENBP " + dec(rows[j].enbp) + " MVBP " + dec(rows[j].mvbp) + ": CLXDAT = 0x" + hexs(val) +
                       ", group bit " + dec(own) + " = " + dec(bit) + " (page: " + dec(rows[j].want) + ")");
            }
        }
    }
    // AND runs: planes 1 and 3 enabled, both data 1, MVBP1 = 1
    if (!g_mutate) {
        for (int mv3 = 0; mv3 < 2; ++mv3) {
            CfPlan plan;
            uint16_t v = uint16_t(clx_bit(6) | clx_bit(8) | clx_bit(0) | (mv3 ? clx_bit(2) : 0));
            plan.per_frame = {{{R_CLXCON, v}}};
            std::array<uint32_t, 9> planes = all_z();
            planes[1] = P_k(1); planes[3] = P_k(3);
            RunSpec s = collision_spec("C2", planes, true, {{1, 0xFFFF}, {3, 0xFFFF}}, {}, plan);
            auto r = run(s);
            pre.check(*r, 2, 2, false, false, false);
            CfResult cr = cf_collect(*r, plan, 0x40, plan.line2);
            pc.absorb(*r, plan, cr, 1, 5);
            uint16_t val = cr.val.empty() ? 0 : cr.val[0];
            bool bit = (val >> 1) & 1;
            if (bit != (mv3 == 1)) ok = false;
            g.note("C2: AND run, MVBP3 = " + dec(mv3) + ": CLXDAT = 0x" + hexs(val) + ", bit 1 = " + dec(bit) + " (page: " + dec(mv3) + ")");
        }
    }
    pre.print("the C2 runs");
    pc.print("C2", "the C2 runs", true, false);
    g.assert_line("C2", pre.setup && pre.frame && pc.all(false), ok,
        "C2: opus:AA-clxcon.c4 opus:AA-clxcon.c11 opus:AA-clxcon.c12 opus:AA-clxcon.c13 opus:AA-clxcon.c14 opus:AA-clxcon.c15 opus:AA-clxcon.c16 opus:AA-clxcon.c17 opus:AA-clxcon.c18 opus:AA-clxcon.c19 opus:AA-clxcon.c20 opus:AA-clxcon.c21 opus:AA-clxcon.c22 "
        "sol:AA-clxcon.c4 sol:AA-clxcon.c11 sol:AA-clxcon.c12 sol:AA-clxcon.c13 sol:AA-clxcon.c14 sol:AA-clxcon.c15 sol:AA-clxcon.c16 sol:AA-clxcon.c17 sol:AA-clxcon.c18 sol:AA-clxcon.c19 sol:AA-clxcon.c20 sol:AA-clxcon.c21 sol:AA-clxcon.c22: "
        "CLXCON bit (5 + k) (ENBPk) includes bit plane k in collision detection and bit (k - 1) (MVBPk) is the value the plane must have, and an included plane that does not match prevents the collision (p.18, p.19), for k = 1..6: "
        "the group bit takes the values of the table for every combination of enable and match value and both data polarities, and the AND runs agree");
}

// ---- C3 -----------------------------------------------------------------------
struct C3Addr { bool ok = false; bool computed = false; CPc pc; };

constexpr uint16_t kC2Addr[2] = {R_CLXCON2_P8, R_CLXCON2_P18};

// the whole table of C2 for planes 7 and 8 with CLXCON2 written at kC2Addr[ai]
inline C3Addr c3_address(int ai, Pre& pre, bool omit_clxcon2, bool full) {
    C3Addr res;
    res.ok = true;
    res.computed = true;
    for (int k = 7; k <= 8; ++k)
        for (int data = 1; data >= 0; --data) {
            if (g_mutate && !full && !(k == 7 && data == 1)) continue;
            auto rows = c2_rows(data);
            CfPlan plan;
            if (mutating("C3-PC-WINDOW")) plan.line2 = 0x68;
            for (const auto& rw : rows) {
                uint16_t en = k == 7 ? 0x0040 : 0x0080, mv = k == 7 ? 0x0001 : 0x0002;
                uint16_t v2 = uint16_t((rw.enbp ? en : 0) | (rw.mvbp ? mv : 0));
                std::vector<Wr> w{{R_CLXCON, 0x0000}};
                if (!omit_clxcon2) w.push_back({kC2Addr[ai], v2});
                plan.per_frame.push_back(w);
            }
            std::array<uint32_t, 9> planes = all_z();
            std::vector<std::pair<int, uint16_t>> fills;
            if (data == 1) { planes[k] = P_k(k); fills.push_back({k, 0xFFFF}); }
            RunSpec s = collision_spec("C3", planes, true, fills, {}, plan, mutating("C3-PC-OTHER") ? uint16_t(0x0000) : uint16_t(0xFFFF), !mutating("C3-PC-SPR"));
            auto r = run(s);
            pre.check(*r, 2, 1 + int(rows.size()), false, false, false);
            CfResult cr = cf_collect(*r, plan, 0x40, plan.line2);
            int own = (k & 1) ? 1 : 5, other = (k & 1) ? 5 : 1;
            std::vector<char> req;
            for (const auto& rw : rows) req.push_back((k & 1) || !(rw.enbp && data != rw.mvbp));
            res.pc.absorb(*r, plan, cr, own, other, &req);
            for (size_t j = 0; j < rows.size(); ++j) {
                uint16_t val = j < cr.val.size() ? cr.val[j] : 0;
                bool bit = (val >> own) & 1;
                if (bit != (rows[j].want == 1)) res.ok = false;
                g.note("C3 (CLXCON2 at 0x" + hexs(kC2Addr[ai], 3) + "): plane " + dec(k) + ", data " + dec(data) + ", F" + dec(2 + int(j)) + ": ENBP " + dec(rows[j].enbp) + " MVBP " + dec(rows[j].mvbp) +
                       ": group bit " + dec(own) + " = " + dec(bit) + " (page: " + dec(rows[j].want) + ")");
            }
        }
    return res;
}

// whether CLXCON2 is in effect at address ai (the whole table holds there); C5 uses it
inline bool c3_effect(int ai) {
    static int cached[2] = {-1, -1};
    if (cached[ai] >= 0) return cached[ai] == 1;
    Pre dummy("C3");
    bool ok = c3_address(ai, dummy, false, true).ok;
    cached[ai] = ok ? 1 : 0;
    return ok;
}

inline void prop_C3() {
    if (!want("C3")) return;
    Pre pre("C3");
    C3Addr a[2];
    bool omit = mutating("C3");
    for (int ai = 0; ai < 2; ++ai) a[ai] = c3_address(ai, pre, omit, false);
    CPc pc;
    for (int ai = 0; ai < 2; ++ai) {
        pc.beam = pc.beam && a[ai].pc.beam; pc.window = pc.window && a[ai].pc.window;
        pc.spr = pc.spr && a[ai].pc.spr; pc.other = pc.other && a[ai].pc.other;
        pc.beam_n += a[ai].pc.beam_n; pc.beam_off += a[ai].pc.beam_off;
    }
    pre.print("the C3 runs");
    pc.print("C3", "the C3 runs", true, false);
    bool ok = a[0].ok || a[1].ok;
    std::string which = a[0].ok && a[1].ok ? "both addresses" : a[0].ok ? "0x10E" : a[1].ok ? "0x10C" : "neither address";
    g.note("C3: the whole table is satisfied with CLXCON2 written at: " + which);
    g.assert_line("C3", pre.setup && pre.frame && pc.all(false), ok,
        "C3: opus:AA-clxcon2.c5 opus:AA-clxcon2.c9 opus:AA-clxcon2.c10 opus:AA-clxcon2.c12 sol:AA-clxcon2.c5 sol:AA-clxcon2.c10 sol:AA-clxcon2.c12 sol:AA-clxcon2.c16: "
        "CLXCON2 bit 7 (ENBP8) and bit 6 (ENBP7) include planes 8 and 7 in collision detection and bit 1 (MVBP8) and bit 0 (MVBP7) are their required values (p.18, p.19): "
        "the whole table of C2 holds for planes 7 and 8 with CLXCON2 written at one of the two addresses the pages give (0x10E p.8, 0x10C p.18)");
}

// ---- C4 -----------------------------------------------------------------------
inline void prop_C4() {
    if (!want("C4")) return;
    Pre pre("C4");
    CPc pc;
    bool in_effect_any = false, ok = true;
    for (int ai = 0; ai < 2; ++ai) {
        CfPlan plan;
        if (mutating("C4-PC-WINDOW")) plan.line2 = 0x68;
        if (mutating("C4-PC-BEAM")) plan.h = 0xE0;
        const uint16_t a2 = kC2Addr[ai];
        std::vector<Wr> f2{{R_CLXCON, 0x0000}, {a2, 0x00C0}};
        if (mutating("C4-PC-EFFECT")) f2 = {{R_CLXCON, 0x0000}};
        std::vector<Wr> f3{{R_CLXCON, 0x0000}}, f4{{a2, 0x00C0}}, f5{{R_CLXCON, 0xF000}};
        if (mutating("C4")) { f3 = {{R_COLOR00, 0x0000}}; f5 = {{R_COLOR00, 0x0000}}; }       // the CLXCON writes of F3 and F5 left out
        plan.per_frame = {f2, f3, f4, f5};
        std::array<uint32_t, 9> planes = all_z();
        planes[7] = P_k(7); planes[8] = P_k(8);
        RunSpec s = collision_spec("C4", planes, true, {{7, 0xFFFF}, {8, 0xFFFF}}, {}, plan, 0xFFFF, !mutating("C4-PC-SPR"));
        auto r = run(s);
        pre.check(*r, 2, 5, false, false, false);
        CfResult cr = cf_collect(*r, plan, 0x40, plan.line2);
        pc.absorb(*r, plan, cr, 1, -1);
        auto b1 = [&](size_t j) { return cr.val.size() > j && ((cr.val[j] >> 1) & 1); };
        auto b5 = [&](size_t j) { return cr.val.size() > j && ((cr.val[j] >> 5) & 1); };
        bool eff = !b1(0) && !b5(0) && !b1(2) && !b5(2);
        bool reset = b1(1) && b5(1) && b1(3) && b5(3);
        g.note("C4 (CLXCON2 at 0x" + hexs(a2, 3) + "): bits 1/5 in F2..F5: " + dec(b1(0)) + dec(b5(0)) + " " + dec(b1(1)) + dec(b5(1)) + " " + dec(b1(2)) + dec(b5(2)) + " " + dec(b1(3)) + dec(b5(3)) +
               "; CLXCON2 in effect: " + dec(eff));
        if (eff) { in_effect_any = true; if (!reset) ok = false; }
    }
    pre.print("the C4 runs");
    pc.print("C4", "the C4 runs", false);
    g.expect(in_effect_any, "C4-PC-EFFECT: at one of the two addresses at least CLXCON2 is in effect: bits 1 and 5 of CLXDAT are 0 in F2 and in F4");
    g.expect_if(pre.setup && pre.frame && pc.all() && in_effect_any, ok,
        "C4: a write to CLXCON clears the bits of CLXCON2, so that planes 7 and 8 no longer take part in collision detection (p.18): "
        "after CLXCON = 0x0000 (F3) and after CLXCON = 0xF000 (F5) CLXDAT bits 1 and 5 are 1, at every address where CLXCON2 is in effect");
}

// ---- C5 -----------------------------------------------------------------------
inline void prop_C5() {
    if (!want("C5")) return;
    Pre pre("C5");
    CPc pc;
    bool pc_static = true, ok_base = true, ok_contrast = true;
    const uint16_t pat[3] = {0x3333, 0x0F0F, 0x00FF};
    for (int run_ab = 0; run_ab < 2; ++run_ab) {                        // run A: planes 7 and 8 = 0xFFFF; run B: 0x0000
        auto mk = [&](const std::vector<Wr>& extra, CfPlan& plan) {
            std::array<uint32_t, 9> planes = all_z();
            std::vector<std::pair<int, uint16_t>> fills;
            for (int k = 1; k <= 6; ++k) { planes[k] = P_k(k); fills.push_back({k, pat[(k - 1) / 2]}); }
            planes[7] = P_k(7); planes[8] = P_k(8);
            fills.push_back({7, run_ab == 0 ? uint16_t(0xFFFF) : uint16_t(0x0000)});
            fills.push_back({8, run_ab == 0 ? uint16_t(0xFFFF) : uint16_t(0x0000)});
            std::vector<Wr> ex = extra;
            if (mutating("C5-PC-STATIC")) ex.push_back({R_CLXCON, 0x0000});
            if (mutating("C5") && extra.empty()) {
                int ae = c3_effect(0) ? 0 : 1;
                ex.push_back({kC2Addr[ae], run_ab == 0 ? uint16_t(0x00C0) : uint16_t(0x00C3)});     // planes 7 and 8 included, as in the contrast
            }
            plan.per_frame = {{}};
            RunSpec s = collision_spec("C5", planes, false, fills, ex, plan, 0xFFFF, !mutating("C5-PC-SPR"));
            return s;
        };
        {
            CfPlan plan;
            if (mutating("C5-PC-WINDOW")) plan.line2 = 0x68;
            RunSpec s = mk({}, plan);
            auto r = run(s);
            pre.check(*r, 1, 2, false, false, false);
            if (!static_no_write(r->spec, 0x098, 0x098) || !static_no_write(r->spec, 0x10C, 0x10E)) pc_static = false;
            CfResult cr = cf_collect(*r, plan, 0x40, plan.line2);
            pc.absorb(*r, plan, cr, 1, -1);
            uint16_t val = cr.val.empty() ? 0 : cr.val[0];
            bool b = ((val >> 1) & 1) && ((val >> 5) & 1);
            if (!b) ok_base = false;
            g.note("C5: run " + std::string(run_ab == 0 ? "A" : "B") + ": CLXDAT = 0x" + hexs(val) + " (bits 1 and 5 expected 1)");
        }
        if (g_mutate && !mutating("C5-CONTROL")) continue;
        for (int ai = 0; ai < 2; ++ai) {
            if (!c3_effect(ai)) continue;
            CfPlan plan;
            uint16_t v2 = run_ab == 0 ? 0x00C0 : 0x00C3;
            if (mutating("C5-CONTROL")) v2 = 0x0000;
            RunSpec s = mk({{kC2Addr[ai], v2}}, plan);
            auto r = run(s);
            pre.check(*r, 1, 2, false, false, false);
            CfResult cr = cf_collect(*r, plan, 0x40, plan.line2);
            pc.absorb(*r, plan, cr, 1, -1);
            uint16_t val = cr.val.empty() ? 0 : cr.val[0];
            bool b = !((val >> 1) & 1) && !((val >> 5) & 1);
            g.note("C5 contrast: run " + std::string(run_ab == 0 ? "A" : "B") + ", CLXCON2 = 0x" + hexs(v2) + " at 0x" + hexs(kC2Addr[ai], 3) + ": CLXDAT = 0x" + hexs(val) + " (bits 1 and 5 expected 0)");
            if (!b) ok_contrast = false;
        }
    }
    pre.print("the C5 runs");
    pc.print("C5", "the C5 runs", false, false);
    g.expect(pc_static, "C5-PC-STATIC: the program writes neither CLXCON (0x098) nor CLXCON2 or BPLCON4 (0x10C, 0x10E) in the C5 runs");
    g.assert_line("C5", pre.setup && pre.frame && pc.all(false) && pc_static, ok_base,
        "C5: sol:AA-clxcon2.c11 sol:AA-clxcon2.c13: after reset, with neither CLXCON nor CLXCON2 written, planes 7 and 8 are not included in collision detection (p.18, p.4): "
        "CLXDAT bits 1 and 5 are 1 in F2 of run A (planes 7 and 8 all ones) and of run B (all zeros)");
    g.expect_if(pre.setup && pre.frame && pc.all(false) && pc_static, ok_contrast,
        "C5-CONTROL: the same runs with CLXCON2 written (0x00C0 in run A, 0x00C3 in run B, no CLXCON write) at every address where C3 found CLXCON2 in effect: CLXDAT bits 1 and 5 are 0");
}

}  // namespace s3
