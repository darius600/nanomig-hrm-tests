// test_aa_integ_s6.cpp
//
// aa-integ-s6: see SPECIFICATION.md. AA sprite resolution modes, sprite widths, attachment,
// sprite colour banks and the reset values of ESPRM4 and OSPRM4, graded at the ports of nanomig.v.
//
// Every run selects the AA chip set with chipset_config = $10; both builds, fx68k and TG68K,
// grade every proposition and every precondition.
//
// One program (prog_main.s and its variants, s6.i) writes the register list that the test places in
// Chip memory; the Copper list, the sprite structures and the bit-plane area are placed by the test
// as well. s6_cases.h builds the cases, s6_grade.h holds the comparators.
//
// NMH_MUTATE=<selector> runs one contrast in place of the normal run: only what the selector names
// runs, with the one stimulus the record describes, through the unchanged comparator. `make test`
// leaves NMH_MUTATE unset and runs no mutant.
//
// MUTATION_RECORDS_BEGIN
// MUTATION_RECORDS_END

#include <set>
#include <functional>
#include "s6_grade.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

using namespace s6;

namespace {

Checks* g_k = nullptr;

// ---- the runs of this process --------------------------------------------------------------------------
std::map<std::string, std::shared_ptr<Run>> g_runs;
std::map<std::string, Pre> g_pre;
std::set<char> g_printed_groups;
Geom g_geom;
bool g_geom_done = false;
bool g_geom_supplied = false;

char group_of(const std::string& n) {
    if (n.rfind("M-", 0) == 0) return 'M';
    if (n.rfind("W-", 0) == 0) return 'W';
    if (n.rfind("A", 0) == 0) return 'A';
    if (n.rfind("B", 0) == 0) return 'B';
    return 'R';
}

// a selector of the form <kind>-<group>: a contrast of one common precondition of that group
struct GroupSel { bool on = false; std::string kind; char group = 0; };
GroupSel group_sel() {
    GroupSel s;
    if (!g_mutate) return s;
    std::string m = g_mutate;
    auto p = m.find('-');
    if (p == std::string::npos || m.size() != p + 2) return s;
    std::string kind = m.substr(0, p);
    char grp = m[p + 1];
    static const std::set<std::string> kinds = {"FIX", "C1", "C2", "C3", "C4", "C5", "C7", "C7B", "BANDS", "SEGS", "R1", "R2", "C6"};
    if (!kinds.count(kind) || std::string("MWABR").find(grp) == std::string::npos) return s;
    s.on = true; s.kind = kind; s.group = grp;
    return s;
}

std::string designated(const GroupSel& s) {
    switch (s.group) {
    case 'M': return (s.kind == "C5" || s.kind == "C7" || s.kind == "C7B") ? "M-L-00" : "M-S-01";
    case 'W': return "W-00";
    case 'A': return (s.kind == "C5" || s.kind == "C7" || s.kind == "C7B") ? "A-L-00" : "A-S-00";
    case 'B': return "B-N";
    default:  return "R";
    }
}

void clear_sprite(Case& c, int k) { c.spr[k].lines.clear(); c.spr[k].vstop = c.spr[k].vstart; }

void apply_group_sel(Case& c) {
    GroupSel s = group_sel();
    if (!s.on || c.name != designated(s)) return;
    if (s.kind == "FIX") { c.prog = "prog_nofix.bin"; }
    else if (s.kind == "C1") apply_cp(c, Cp::C1);
    else if (s.kind == "C2") apply_cp(c, Cp::C2);
    else if (s.kind == "C3") apply_cp(c, Cp::C3);
    else if (s.kind == "C4") apply_cp(c, Cp::C4);
    else if (s.kind == "C5") apply_cp(c, Cp::C5);
    else if (s.kind == "C7") apply_cp(c, Cp::C7a);
    else if (s.kind == "C7B") apply_cp(c, Cp::C7b);
    else if (s.kind == "BANDS" && s.group == 'A') { clear_sprite(c, 6); clear_sprite(c, 7); }
    else if (s.kind == "BANDS" && s.group == 'B') { clear_sprite(c, 7); }
    else if (s.kind == "SEGS" && s.group == 'R') { clear_sprite(c, 7); }
    else if (s.kind == "C6" && s.group == 'B') { c.cop_extra.resize(20); }                 // the end-of-list WAIT follows band 9's MOVE
    else if (s.kind == "R1" && s.group == 'R') { c.prog = "prog_pro.bin"; c.prologue_list = true; c.entry = kProEntry; }
    else if (s.kind == "R2" && s.group == 'R') { append(c.cop_extra, cop_move(R_BPLCON4, 0x0011)); }
}

std::shared_ptr<Run> get(Case c) {
    apply_group_sel(c);
    auto it = g_runs.find(c.name);
    if (it != g_runs.end()) return it->second;
    auto r = execute(c);
    g_runs[c.name] = r;
    return r;
}

const Geom& geom() {
    if (!g_geom_done) {
        const char* sup = g_mutate ? std::getenv("NMH_GEOM") : nullptr;   // a mutation run may take u,r from the normal run
        int su = 0, sr = 0;
        if (sup && std::sscanf(sup, "%d,%d", &su, &sr) == 2 && su > 0 && sr > 0) {
            g_geom.ok = true; g_geom.u = su; g_geom.r = sr;
            g_geom_supplied = true;
        } else {
            auto r = get(make_m('H', 0));
            g_geom = measure_geom(*r);
        }
        g_geom_done = true;
    }
    return g_geom;
}

const Pre& pre_of(const Run& r) {
    auto it = g_pre.find(r.name);
    if (it != g_pre.end()) return it->second;
    return g_pre[r.name] = compute_pre(r, geom());
}

bool a_bands_ok(const Run& r) { return r.frames_ok && bands_of(r).size() == 4; }

bool bands_segs_ok(const Run& r, size_t nbands, size_t nseg) {
    if (!r.frames_ok) return false;
    auto bs = bands_of(r);
    if (bs.size() != nbands) return false;
    for (auto& b : bs)
        for (size_t i = 0; i < b.count; ++i)
            if (segs_of(r.g[b.first + i]).size() != nseg) return false;
    return true;
}
bool b_bands_ok(const Run& r) { return bands_segs_ok(r, 16, r.name.find('A') != std::string::npos ? 4 : 8); }
bool r_segs_ok(const Run& r) { return bands_segs_ok(r, 1, 8); }

// ---- group lines (SPECIFICATION.md 3.8): printed before the line that depends on them ---------------------
void group_line(const std::string& label, const char* text, char grp, const std::function<bool(const Run&)>& pred) {
    bool all = true, any = false;
    std::string bad;
    for (auto& kv : g_runs) {
        if (group_of(kv.first) != grp) continue;
        any = true;
        if (!pred(*kv.second)) { all = false; bad += " " + kv.first; }
    }
    if (!any) return;
    g_k->expect(all, label + " " + text);
    if (!all) {
        std::string why;
        for (auto& kv : g_runs) {
            if (group_of(kv.first) != grp) continue;
            const Pre& p = pre_of(*kv.second);
            if (label.rfind("C3-", 0) == 0 && !p.c3 && !p.c3why.empty()) why += " [" + kv.first + ": " + p.c3why + "]";
            if (label.rfind("C5-", 0) == 0 && !p.c5 && !p.c5why.empty()) why += " [" + kv.first + ": " + p.c5why + "]";
            if (label.rfind("C7-", 0) == 0 && !p.c7 && !p.c7why.empty()) why += " [" + kv.first + ": " + p.c7why + "]";
        }
        g_k->note(label + " fails in:" + bad + why);
    }
}

void print_group_lines() {
    const Geom& G = geom();
    if (!G.ok) g_k->note("u and r could not be measured on the HIRES calibration rows: " + G.why);
    else if (g_printed_groups.empty()) g_k->note("u = " + dec(G.u) + " samples per 70 ns of display, r = " + dec(G.r) + " rows per beam line (" + (g_geom_supplied ? "taken from the normal run of this core" : "M-H-00 calibration rows") + ")");
    for (char grp : std::string("MWABR")) {
        bool have = false;
        for (auto& kv : g_runs) if (group_of(kv.first) == grp) have = true;
        if (!have || g_printed_groups.count(grp)) continue;
        g_printed_groups.insert(grp);
        std::string t(1, grp);
        group_line("FIX-" + t, "fixture: the program loaded, the CPU was released, the program's $BEEF constant was read back and a graded frame was captured", grp, [](const Run& r) { return pre_of(r).fix; });
        group_line("C1-" + t, "the program reached its end: the trace holds the CPU write of $600D to $0C0FFE (SPECIFICATION.md 3.8 C1)", grp, [](const Run& r) { return pre_of(r).c1; });
        group_line("C2-" + t, "the display is in steady state: the graded frame and the frame before it are identical in every calibration row and every band row (C2)", grp, [](const Run& r) { return pre_of(r).c2; });
        group_line("C3-" + t, "the display resolution is the case's and the colour path is direct: the calibration rows alternate colour(0) and colour(1) in runs of 4u, 2u or u samples (C3)", grp, [](const Run& r) { return pre_of(r).c3; });
        group_line("C4-" + t, "sprite DMA used the prepared structures: the graded frame holds a read at the address of every sprite structure (C4)", grp, [](const Run& r) { return pre_of(r).c4; });
        group_line("C5-" + t, "no sprite is clipped by the display window: the first colour(1) sample lies left of the leftmost segment and the last at or right of the rightmost segment's end (C5)", grp, [](const Run& r) { return pre_of(r).c5; });
        group_line("C7-" + t, "the row grid holds: the calibration rows number 8r (HIRES) or 16r (LORES) and the first colour(1) sample has the same index in every calibration row (C7)", grp, [](const Run& r) { return pre_of(r).c7; });
        if (grp == 'A') group_line("BANDS-A", "four bands (one per attached pair) lie below the calibration rows (P7 precondition)", grp, a_bands_ok);
        if (grp == 'B') {
            group_line("BANDS-B", "sixteen bands lie below the calibration rows and every graded row has 8 (B-N) or 4 (B-A) segments (P8 precondition)", grp, b_bands_ok);
            group_line("C6-B", "the graded frame holds the Copper fetches of all sixteen MOVE instructions of the list, in list order (C6)", grp, c6_ok);
        }
        if (grp == 'R') {
            group_line("SEGS-R", "one band lies below the calibration rows and every one of its rows has 8 segments (P9 and P10 precondition)", grp, r_segs_ok);
            group_line("R1-R", "no code ran before the test program: the reset vectors are followed by an instruction fetch at the program's entry and no data of the case was read first (R1)", grp, r1_ok);
            group_line("R2-R", "nothing wrote BPLCON4 other than the case's own register-list entry, COP1LC holds only the list's address, COP2LC and COPJMP2 are not written, and the Copper list has no MOVE to BPLCON4 (R2)", grp, r2_ok);
        }
    }
}

bool deps_ok(const std::vector<std::shared_ptr<Run>>& runs, std::string* unmet = nullptr) {
    bool ok = true;
    for (auto& r : runs) {
        const Pre& p = pre_of(*r);
        if (!p.all()) { ok = false; if (unmet) *unmet += " " + r->name; }
    }
    return ok;
}

void not_graded(const std::string& id, const std::string& unmet) {
    g_k->record("NOT GRADED " + id + ": precondition(s) not met in:" + unmet);
}

void cell_note(const std::string& prop, const Run& r, const CellRes& c) {
    g_k->note(prop + " cell " + r.name + ": " + (c.ok ? std::string("every check holds") : c.why));
}

// ---- cells, with the contrasts the selectors name -------------------------------------------------------------
struct MSub { char res; int spres; bool changed; };

MSub msub(char res, int spres, const std::string& who) {
    MSub s{res, spres, false};
    if (g_mutate && who == g_mutate) {
        if (who == "P1" && res == 'L' && spres == 0) s.spres = 2;
        if (who == "P2" && res == 'S' && spres == 1) s.spres = 0;
        if (who == "P2a" && res == 'S' && spres == 1) s.spres = 0;
        if (who == "P3" && res == 'S') s.res = 'L';
        if (who == "P4" && res == 'L' && spres == 2) s.spres = 1;
        if (who == "P5" && res == 'L' && spres == 3) s.spres = 2;
        if (who == "P5a" && res == 'L' && spres == 3) s.spres = 2;
        if (who == "P11" && res == 'L' && spres == 0) s.spres = 2;
        if (who == "P12" && res == 'S' && spres == 1) s.spres = 0;
    }
    s.changed = s.res != res || s.spres != spres;
    return s;
}

std::shared_ptr<Run> cellM(char res, int spres, const std::string& who) {
    MSub s = msub(res, spres, who);
    return get(make_m(s.res, s.spres));
}

int wsub(int idx, const std::string& who) {
    if (g_mutate && who == g_mutate) {
        if (who == "P6" && idx == 1) return 0;
        if (who == "P6a" && idx == 0) return 1;
        if (who == "P6b" && idx == 1) return 0;
        if (who == "P6c" && idx == 3) return 1;
    }
    return idx;
}
std::shared_ptr<Run> cellW(int idx, const std::string& who) { return get(make_w(wsub(idx, who))); }

bool asub(char res, int spres, const std::string& who) {
    if (!g_mutate || who != g_mutate) return false;
    return (who == "P7-ctl" && res == 'S' && spres == 0) || (who == "P7-L11" && res == 'L' && spres == 3);
}
bool awsub(int idx, const std::string& who) {
    if (!g_mutate || who != g_mutate) return false;
    return (who == "P7-W01" && idx == 1) || (who == "P7-W10" && idx == 2) || (who == "P7-W11" && idx == 3);
}
std::shared_ptr<Run> cellA(char res, int spres, const std::string& who) { return get(make_a(res, spres, !asub(res, spres, who))); }
std::shared_ptr<Run> cellAW(int idx, const std::string& who) { return get(make_aw(idx, !awsub(idx, who))); }

std::shared_ptr<Run> cellB(bool att, const std::string& who) {
    Case c = make_b(att);
    if (g_mutate && ((who == "P8" && !att) || (who == "P8-A" && att)) && who == g_mutate) {
        c.cop_extra[4 * 5 + 3] = 0x004B;     // band 5's MOVE writes band 4's value
        c.name = att ? "BX-A" : "BX-N";
    }
    return get(c);
}

std::shared_ptr<Run> cellR(const std::string& who) {
    Case c = make_r();
    if (g_mutate && who == g_mutate) {
        if (who == "P9") { c.bplcon4 = uint16_t(0x0001); c.name = "RX-E"; }
        if (who == "P10") { c.bplcon4 = uint16_t(0x0010); c.name = "RX-O"; }
    }
    return get(c);
}

// ---------------------------------------------------------------------------------------------------------------
// the propositions
// ---------------------------------------------------------------------------------------------------------------
struct MCell { char res; int spres; };

struct WidthEval {
    std::vector<std::shared_ptr<Run>> runs;
    bool all_ok = true, graded = true;
    std::string unmet;
};

// Evaluate a width proposition over its cells: the expected pixel width of each cell comes from the page (SPECIFICATION.md 2).
WidthEval eval_width(const std::string& id, const std::vector<MCell>& cells, const Geom& G) {
    WidthEval e;
    for (auto c : cells) e.runs.push_back(cellM(c.res, c.spres, id));
    print_group_lines();
    e.graded = deps_ok(e.runs, &e.unmet);
    if (!e.graded) return e;
    for (size_t i = 0; i < cells.size(); ++i) {
        std::string rec;
        CellRes cr = grade_m(*e.runs[i], pixel_ns(cells[i].res, cells[i].spres), G, &rec);
        cell_note(id, *e.runs[i], cr);
        if (!rec.empty()) g_k->record(id + " " + e.runs[i]->name + ": " + rec);
        e.all_ok = e.all_ok && cr.ok;
    }
    return e;
}

void prop_p1() {
    WidthEval e = eval_width("P1", {{'L', 0}, {'H', 0}, {'S', 0}}, geom());
    if (!e.graded) { not_graded("P1", e.unmet); return; }
    g_k->expect(e.all_ok, "P1 opus:AA-sprites.c2 sol:AA-sprites.c1: in ECS default mode (SPRES1,SPRES0 = 00) every sprite has the ECS pixel width on each display resolution: 140 ns on LORES and HIRES, 70 ns on SHRES, all eight sprites on every row (AA p.31, AA p.17)");
}

void prop_p2() {
    WidthEval e = eval_width("P2", {{'L', 1}, {'H', 1}, {'S', 1}}, geom());
    if (!e.graded) { not_graded("P2", e.unmet); return; }
    g_k->expect(e.all_ok, "P2 sol:AA-sprites.c2: in 140 ns mode (SPRES1,SPRES0 = 01) every sprite has 140 ns pixels on LORES, HIRES and SHRES, all eight sprites on every row (AA p.31, AA p.17)");
}

void prop_p2a() {
    WidthEval e = eval_width("P2a", {{'S', 1}}, geom());
    if (!e.graded) { not_graded("P2a", e.unmet); return; }
    g_k->expect(e.all_ok, "P2a sol:AA-sprites.c7: 140 ns sprites on a superhires screen (SPRES1,SPRES0 = 01 with BPLCON0 SHRES), all eight sprites on every row (AA p.31)");
}

void prop_p4() {
    WidthEval e = eval_width("P4", {{'L', 2}, {'H', 2}, {'S', 2}}, geom());
    if (!e.graded) { not_graded("P4", e.unmet); return; }
    g_k->expect(e.all_ok, "P4 opus:AA-sprites.c4 sol:AA-sprites.c4: in 70 ns mode (SPRES1,SPRES0 = 10) every sprite has 70 ns pixels on LORES, HIRES and SHRES, all eight sprites on every row (AA p.31, AA p.17)");
}

void prop_p5() {
    WidthEval e = eval_width("P5", {{'L', 3}, {'H', 3}, {'S', 3}}, geom());
    if (!e.graded) { not_graded("P5", e.unmet); return; }
    g_k->expect(e.all_ok, "P5 opus:AA-sprites.c5 sol:AA-sprites.c5: in 35 ns mode (SPRES1,SPRES0 = 11) every sprite has 35 ns pixels on LORES, HIRES and SHRES, all eight sprites on every row (AA p.31, AA p.18)");
}

void prop_p5a() {
    WidthEval e = eval_width("P5a", {{'L', 3}}, geom());
    if (!e.graded) { not_graded("P5a", e.unmet); return; }
    g_k->expect(e.all_ok, "P5a sol:AA-sprites.c6: 35 ns sprites on a lores screen (SPRES1,SPRES0 = 11 with BPLCON0 LORES), all eight sprites on every row (AA p.31)");
}

void prop_p3() {
    const Geom& G = geom();
    auto a = cellM('S', 0, "P3"), b = cellM('S', 1, "P3");
    print_group_lines();
    std::string unmet;
    if (!deps_ok({a, b}, &unmet)) { not_graded("P3", unmet); return; }
    CellRes ra = grade_len(*a, 16 * G.u, G), rb = grade_len(*b, 32 * G.u, G);
    cell_note("P3", *a, ra);
    cell_note("P3", *b, rb);
    g_k->expect(ra.ok && rb.ok, "P3 sol:AA-sprites.c3: on a SHRES screen 140 ns mode is not the ECS default mode: every sprite's segment is 16u samples long with SPRES = 00 and 32u with SPRES = 01 (AA p.31 '2) 140ns (this is not ECS mode!)', AA p.17)");
}

void prop_p11() {
    const Geom& G = geom();
    std::vector<std::shared_ptr<Run>> runs;
    std::vector<MCell> mc;
    for (char r : std::string("LHS"))
        for (int s = 0; s < 4; ++s) {
            if (g_mutate && !msub(r, s, "P11").changed) continue;     // a mutated process runs the replaced cell only
            runs.push_back(cellM(r, s, "P11"));
            mc.push_back({r, s});
        }
    print_group_lines();
    std::string unmet;
    if (!deps_ok(runs, &unmet)) { not_graded("P11", unmet); return; }
    bool all = true;
    for (size_t i = 0; i < runs.size(); ++i) {
        CellRes cr = grade_m(*runs[i], pixel_ns(mc[i].res, mc[i].spres), G);
        cell_note("P11", *runs[i], cr);
        all = all && cr.ok;
    }
    g_k->expect(all, "P11 opus:AA-sprites.c1 opus:AA-sprites.c6: every sprite can be displayed in each of the four modes (ECS default, 140 ns, 70 ns, 35 ns) on each display resolution: P1, P2, P4 and P5 all hold (AA p.31 'All sprites can now be displayed in either ... on any display resolution')");
}

void prop_p12() {
    const Geom& G = geom();
    auto l = cellM('L', 1, "P12"), h = cellM('H', 1, "P12"), s1 = cellM('S', 1, "P12"), s0 = cellM('S', 0, "P12");
    print_group_lines();
    std::string unmet;
    if (!deps_ok({l, h, s1, s0}, &unmet)) { not_graded("P12", unmet); return; }
    bool p2 = grade_m(*l, 140, G).ok && grade_m(*h, 140, G).ok && grade_m(*s1, 140, G).ok;
    bool p3 = grade_len(*s0, 16 * G.u, G).ok && grade_len(*s1, 32 * G.u, G).ok;
    g_k->note(std::string("P12 P2 part: ") + (p2 ? "holds" : "fails") + "; P3 part: " + (p3 ? "holds" : "fails"));
    g_k->expect(p2 && p3, "P12 opus:AA-sprites.c3: the 140 ns mode is available to every sprite on each display resolution (P2) and is not the ECS default mode (P3) (AA p.31 '2) 140ns (this is not ECS mode!)')");
}

void prop_p6() {
    const Geom& G = geom();
    std::vector<std::shared_ptr<Run>> runs;
    for (int i = 0; i < 4; ++i) runs.push_back(cellW(i, "P6"));
    print_group_lines();
    std::string unmet;
    if (!deps_ok(runs, &unmet)) { not_graded("P6", unmet); return; }
    static const int want_px[4] = {16, 32, 32, 64};
    bool all = true;
    int measured[4] = {0, 0, 0, 0};
    for (int i = 0; i < 4; ++i) {
        CellRes cr = grade_w(*runs[i], i, G, &measured[i]);
        cell_note("P6", *runs[i], cr);
        all = all && cr.ok;
    }
    std::set<int> widths(measured, measured + 4);
    bool set_ok = widths == std::set<int>({16, 32, 64});
    bool map_ok = true;
    for (int i = 0; i < 4; ++i) if (measured[i] != want_px[i]) map_ok = false;
    g_k->note("P6 solid-row widths in pixels of 140 ns: W-00 " + dec(measured[0]) + ", W-01 " + dec(measured[1]) + ", W-10 " + dec(measured[2]) + ", W-11 " + dec(measured[3]));
    g_k->expect(all && set_ok && map_ok, "P6 opus:AA-sprites.c7: a sprite is 16, 32 or 64 pixels wide, selected by FMODE SPAGEM and SPR32: 16 (00), 32 (01), 32 (10), 64 (11), with the wider sprites made of distinct data words (AA p.31, AA p.3, AA p.24)");
}

void width_one(const std::string& id, const std::vector<int>& idxs, const char* text) {
    const Geom& G = geom();
    std::vector<std::shared_ptr<Run>> runs;
    for (int i : idxs) runs.push_back(cellW(i, id));
    print_group_lines();
    std::string unmet;
    if (!deps_ok(runs, &unmet)) { not_graded(id, unmet); return; }
    bool all = true;
    for (size_t k = 0; k < idxs.size(); ++k) {
        CellRes cr = grade_w(*runs[k], idxs[k], G);
        cell_note(id, *runs[k], cr);
        all = all && cr.ok;
    }
    g_k->expect(all, text);
}

void prop_p6a() { width_one("P6a", {0}, "P6a sol:AA-sprites.c8: sprites are 16 bits wide with FMODE SPAGEM,SPR32 = 00 (AA p.31 'Sprites are either 16, 32, or 64 bits wide.', AA p.24)"); }
void prop_p6b() { width_one("P6b", {1, 2}, "P6b sol:AA-sprites.c9: sprites are 32 bits wide with FMODE SPAGEM,SPR32 = 01 and with 10, the two settings of a 4-byte fetch increment (AA p.31, AA p.24)"); }
void prop_p6c() { width_one("P6c", {3}, "P6c sol:AA-sprites.c10: sprites are 64 bits wide with FMODE SPAGEM,SPR32 = 11 (AA p.31, AA p.24)"); }

void prop_p7(const std::string& who) {
    const Geom& G = geom();
    std::vector<std::shared_ptr<Run>> runs;
    std::vector<int> gsamp, parts;
    std::vector<bool> in_ctl;     // the cells of the control: every cell but the two 32-bit fetch widths
    for (char r : std::string("LHS"))
        for (int s = 0; s < 4; ++s) {
            if (g_mutate && !asub(r, s, who)) continue;
            runs.push_back(cellA(r, s, who));
            gsamp.push_back(2 * pixel_ns(r, s) * G.u / 70);
            parts.push_back(1);
            in_ctl.push_back(true);
        }
    static const int wparts[4] = {1, 2, 2, 4};
    for (int i = 1; i < 4; ++i) {
        if (g_mutate && !awsub(i, who)) continue;
        runs.push_back(cellAW(i, who));
        gsamp.push_back(4 * G.u);
        parts.push_back(wparts[i]);
        in_ctl.push_back(i == 3);
    }
    print_group_lines();
    std::string unmet;
    bool deps = deps_ok(runs, &unmet);
    for (auto& r : runs) if (!a_bands_ok(*r)) { deps = false; unmet += " " + r->name; }
    const bool ctl_only = who == "P7-ctl" || who == "P7-L11" || who == "P7-W11";
    const bool contrast_only = who == "P7-W01" || who == "P7-W10";
    if (!deps) { not_graded(ctl_only ? "P7-ctl" : "P7", unmet); return; }
    bool all = true, ctl = true;
    for (size_t i = 0; i < runs.size(); ++i) {
        CellRes cr = grade_a(*runs[i], gsamp[i], parts[i], G);
        cell_note("P7", *runs[i], cr);
        all = all && cr.ok;
        if (in_ctl[i]) ctl = ctl && cr.ok;
    }
    if (contrast_only) { g_k->note(std::string("P7 contrast ") + runs[0]->name + " through the comparator of P7: " + (all ? "holds" : "fails")); return; }
    if (!ctl_only)
        g_k->expect(all, "P7 opus:AA-sprites.c8 sol:AA-sprites.c11: sprites can be attached in every sprite resolution mode on every display resolution and at every sprite width: each group of an attached pair shows colour(16 + v) of Table 4-5 (AA p.31 'Sprites can be attached in any mode', AA p.2, AA p.28, HRM p.122)");
    if (!contrast_only && (!g_mutate || ctl_only))
        g_k->expect(ctl, "P7-ctl control of P7: the same comparator on the twelve resolution cells and on the 64-bit width run (AW-11), each attached pair showing colour(16 + v)");
}

void prop_p8(const std::string& who) {
    const Geom& G = geom();
    auto n = cellB(false, who), a = cellB(true, who);
    print_group_lines();
    std::string unmet;
    bool deps = deps_ok({n, a}, &unmet);
    for (auto& r : {n, a}) if (!b_bands_ok(*r) || !c6_ok(*r)) { deps = false; unmet += " " + r->name; }
    if (!deps) { not_graded("P8", unmet); return; }
    CellRes rn = grade_b(*n, false, G), ra = grade_b(*a, true, G);
    cell_note("P8", *n, rn);
    cell_note("P8", *a, ra);
    g_k->expect(rn.ok && ra.ok, "P8 opus:AA-sprites.c10 sol:AA-sprites.c13: each of the 16 colour banks can be selected for the even sprites (ESPRM), the odd sprites (OSPRM) and attached pairs (OSPRM): colour index BANK*16 + (k>>1)*4 + v unattached, OSPRM*16 + v attached (AA p.31, AA p.18, AA p.4, HRM p.103, HRM p.122)");
}

void prop_p9_p10(bool p10) {
    const std::string who = p10 ? "P10" : "P9";
    const Geom& G = geom();
    auto r = cellR(who);
    print_group_lines();
    std::string unmet;
    bool deps = deps_ok({r}, &unmet) && r_segs_ok(*r) && r1_ok(*r) && r2_ok(*r);
    if (!deps) { not_graded(who, unmet + " (or R1, R2, segments)"); return; }
    std::string bits;
    CellRes cr = grade_r(*r, p10, G, &bits);
    cell_note(who, *r, cr);
    g_k->note(who + " bits 7-5 of the index (recorded, not graded): " + bits);
    if (!p10)
        g_k->expect(cr.ok, "P9 opus:AA-revlist.c1 sol:AA-revision.c1: ESPRM4 is 1 after reset: with BPLCON4 never written, bit 4 of the colour index of every group of the even sprites 0, 2, 4, 6 is 1 (AA p.1 '6/07/91 corrected ESPRM4 & OSPRM4 to be init. to 1', AA p.16, AA p.18)");
    else
        g_k->expect(cr.ok, "P10 opus:AA-revlist.c2 sol:AA-revision.c2: OSPRM4 is 1 after reset: with BPLCON4 never written, bit 4 of the colour index of every group of the odd sprites 1, 3, 5, 7 is 1 (AA p.1 '6/07/91 corrected ESPRM4 & OSPRM4 to be init. to 1', AA p.16, AA p.18)");
}

// ---- the normal run ---------------------------------------------------------------------------------------
void normal_run() {
    geom();
    for (char r : std::string("LHS")) for (int s = 0; s < 4; ++s) cellM(r, s, "");
    for (int i = 0; i < 4; ++i) cellW(i, "");
    for (char r : std::string("LHS")) for (int s = 0; s < 4; ++s) cellA(r, s, "");
    for (int i = 1; i < 4; ++i) cellAW(i, "");
    cellB(false, ""); cellB(true, "");
    cellR("");
    print_group_lines();
    prop_p1(); prop_p2(); prop_p2a(); prop_p3(); prop_p4(); prop_p5(); prop_p5a(); prop_p11(); prop_p12();
    prop_p6(); prop_p6a(); prop_p6b(); prop_p6c();
    prop_p7("");
    prop_p8("");
    prop_p9_p10(false);
    prop_p9_p10(true);
}

void mutated_run() {
    std::string m = g_mutate;
    GroupSel gs = group_sel();
    if (gs.on) {
        // a contrast of a common precondition of one group: the group's variant runs, beside the geometry run
        geom();
        Case c;
        std::string d = designated(gs);
        switch (gs.group) {
        case 'M': c = make_m(d[2], (d[4] - '0') * 2 + (d[5] - '0')); break;
        case 'W': c = make_w(0); break;
        case 'A': c = d == "A-S-00" ? make_a('S', 0) : make_a('L', 0); break;
        case 'B': c = make_b(false); break;
        default:  c = make_r(); break;
        }
        get(c);
        print_group_lines();
        return;
    }
    if (m == "P1") prop_p1();
    else if (m == "P2") prop_p2();
    else if (m == "P2a") prop_p2a();
    else if (m == "P3") prop_p3();
    else if (m == "P4") prop_p4();
    else if (m == "P5") prop_p5();
    else if (m == "P5a") prop_p5a();
    else if (m == "P11") prop_p11();
    else if (m == "P12") prop_p12();
    else if (m == "P6") prop_p6();
    else if (m == "P6a") prop_p6a();
    else if (m == "P6b") prop_p6b();
    else if (m == "P6c") prop_p6c();
    else if (m == "P7-ctl" || m == "P7-L11" || m == "P7-W01" || m == "P7-W10" || m == "P7-W11") prop_p7(m);
    else if (m == "P8" || m == "P8-A") prop_p8(m);
    else if (m == "P9") prop_p9_p10(false);
    else if (m == "P10") prop_p9_p10(true);
    else std::cout << "NMH_MUTATE=" << m << " names no selector\n";
}

// ---- NMH_DUMP=<case name>: prints what one case showed, for looking at a result; grades nothing ------------
Case case_by_name(const std::string& n) {
    auto sp = [&](size_t i) { return (n[i] - '0') * 2 + (n[i + 1] - '0'); };
    if (n.rfind("M-", 0) == 0) return make_m(n[2], sp(4));
    if (n.rfind("W-", 0) == 0) return make_w(sp(2));
    if (n.rfind("AW-", 0) == 0) return make_aw(sp(3));
    if (n.rfind("A-", 0) == 0) return make_a(n[2], sp(4));
    if (n == "B-N") return make_b(false);
    if (n == "B-A") return make_b(true);
    return make_r();
}

void dump_run(const std::string& n) {
    auto r = get(case_by_name(n));
    std::printf("DUMP %s: loaded=%d released=%d fixture=%d done=%d frames_ok=%d rows=%zu\n", n.c_str(), r->loaded, r->released, r->fixture, r->done_seen, r->frames_ok, r->g.size());
    const Geom& G = geom();
    std::printf("DUMP u=%d r=%d\n", G.u, G.r);
    auto gs = groups_of(r->g);
    for (size_t b = 0; b < gs.size(); ++b) {
        std::printf("DUMP group %zu first=%zu rows=%zu\n", b, gs[b].first, gs[b].count);
        if (b == 0 || b > 2) continue;
        for (size_t i = 0; i < gs[b].count; i += (G.r ? G.r : 1)) {
            const Row& row = r->g[gs[b].first + i];
            std::printf("DUMP  row %zu:", i);
            int last = -2, run = 0;
            for (size_t x = 0; x < row.size(); ++x) {
                int id = idx_of(row[x]);
                if (id < 0) { if (run) std::printf(" [%d x%d]", last, run); last = -2; run = 0; continue; }
                if (id == last) ++run; else { if (run) std::printf(" [%d x%d]", last, run); last = id; run = 1; }
            }
            if (run) std::printf(" [%d x%d]", last, run);
            std::printf("\n");
        }
    }
    { const Pre& p = pre_of(*r); std::printf("DUMP pre: fix=%d c1=%d c2=%d c3=%d c4=%d c5=%d c7=%d r1=%d r2=%d c6=%d %s %s %s\n", p.fix, p.c1, p.c2, p.c3, p.c4, p.c5, p.c7, r1_ok(*r), r2_ok(*r), c6_ok(*r), p.c3why.c_str(), p.c5why.c_str(), p.c7why.c_str()); }
    std::printf("DUMP early:");
    for (size_t i = 0; i < r->early.size() && i < 40; ++i) std::printf(" %c%06X", r->early[i].is_write ? 'w' : 'r', r->early[i].word_addr * 2);
    std::printf("\n");
}

}  // namespace

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);
    Checks checks;
    g_k = &checks;
    if (const char* d = std::getenv("NMH_DUMP")) { std::string n = d; if (n == "-") return 0; size_t p; while ((p = n.find(',')) != std::string::npos) { dump_run(n.substr(0, p)); n = n.substr(p + 1); } dump_run(n); return 0; }
    if (g_mutate) mutated_run(); else normal_run();
    {
        std::map<uint16_t, std::string> ids;
        for (auto& kv : g_runs) ids[kv.second->lisa_id] += (ids[kv.second->lisa_id].empty() ? "" : " ") + kv.first;
        for (auto& kv : ids) checks.note("the CPU read of LISAID ($DFF07C) at the end of the register list returned $" + hexs(kv.first) + " in the runs " + kv.second);
    }
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
