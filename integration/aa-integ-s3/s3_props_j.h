// s3_props_j.h -- the fixture and the joint assertions J1-J4

#pragma once

#include "s3_check.h"
#include "s3_pre.h"

namespace s3 {

// ---- FIXTURE -------------------------------------------------------------------
// The fixture delivers known words before anything reads one: the program stores the setup marker and
// the event markers, the harness reads them back and the port shows the writes; the Copper's first
// fetch of the list returns the first word the test placed there.
inline void fixture() {
    if (!want("FIXTURE")) return;
    Std o; o.bpu = 1; o.dmacon = 0x8380;
    RunSpec s = base_spec(o, "FIXTURE");
    if (mutating("FIXTURE")) s.no_marker = true;
    s.vb = bpl_ptrs(all_z(), 1);
    EventSpec flush; flush.nvb = 1; s.ev.push_back(flush);
    EventSpec fin; fin.nvb = 2; s.ev.push_back(fin);
    auto r = run(s);
    bool seen = r->marker_seen;
    bool marker_ok = r->word(kResults) == kSetupMarker;
    bool ev_ok = r->ev_word(0, 0) == 0xE000 && r->ev_word(1, 0) == 0xE001 && marker_time(*r, 1) != 0;
    bool fetch_ok = false;
    auto it = r->clread.find(kCL);
    if (it != r->clread.end()) for (const auto& p : it->second) if (p.second == R_COLOR00) fetch_ok = true;
    g.note("FIXTURE: RES word 0 = 0x" + hexs(r->word(kResults)) + ", event markers 0x" + hexs(r->ev_word(0, 0)) + " 0x" + hexs(r->ev_word(1, 0)) +
           ", Copper first word at the COP1LC address read as 0x" + hexs(it != r->clread.end() && !it->second.empty() ? it->second.front().second : 0xFFFF));
    g.expect(r->loaded && r->released && r->done && seen && marker_ok && ev_ok && fetch_ok,
        "FIXTURE: the program-stored constants arrive: the setup marker 0x5E7F stored by the program is seen at the port and read back from RES, the event markers 0xE000 and 0xE001 are read back, "
        "and the Copper's fetch at the COP1LC address returns the first word of the list (0x0180, MOVE COLOR00)");
}

// ---- joint assertions -------------------------------------------------------------
inline std::string vs(const char* id) {
    auto it = g_verdict.find(id);
    if (it == g_verdict.end()) return std::string(id) + " not run";
    if (!it->second.graded) return std::string(id) + " NOT GRADED";
    return std::string(id) + (it->second.passed ? " PASS" : " FAIL");
}

inline bool part_ok(const char* id) {
    auto it = g_verdict.find(id);
    return it != g_verdict.end() && it->second.graded && it->second.passed;
}

inline void joint_assertions() {
    if (g_mutate) return;
    if (std::getenv("NMH_ONLY")) return;
    const std::string v1 = vs("S1"), v2 = vs("S2"), v3 = vs("S3"), v4 = vs("S4"), v5 = vs("S5"), v5s1 = vs("S5-SH1"), v5p = vs("S5-SH10-SH3"),
                      v6 = vs("S6"), v8 = vs("S8"), v9 = vs("S9"), v10 = vs("S10"), v12 = vs("S12"), v14b = vs("S14-B");
    g.expect(part_ok("S5-SH10-SH3") && part_ok("S8"),
        "J1: sol:AA-sprxpos.c4: SPRxPOS is the 'Sprite x Vert-Horiz start position data' (p.28); it passes only if S5-SH10-SH3 (horizontal start, SPRxPOS bits 7-0 = SH10-SH3) and S8 (vertical start, SPRxPOS bits 15-8 = SV7-SV0) pass; parts: " + v5p + "; " + v8);
    g.expect(part_ok("S5-SH1") && part_ok("S5") && part_ok("S6"),
        "J2: opus:AA-sprxpos.c8 sol:AA-sprxpos.c14: SPRxPOS bits 7-0 are SH10-SH3 and the low-order 3 bits are in SPRxCTL (p.28); it passes only if S5 (SH10-SH2), S5-SH1 (SH1) and S6 (SH0) pass; parts: " + v5 + "; " + v5s1 + "; " + v6);
    g.expect(part_ok("S5-SH1") && part_ok("S5") && part_ok("S6") && part_ok("S8") && part_ok("S9") && part_ok("S10") && part_ok("S12"),
        "J3: opus:AA-sprxctl.c17 sol:AA-sprxctl.c31: the two registers work together as position, size and feature sprite control registers (p.28); it passes only if S5, S5-SH1, S6, S8 (position), S9 (size), S10 and S12 (feature) pass; parts: " +
        v5 + "; " + v5s1 + "; " + v6 + "; " + v8 + "; " + v9 + "; " + v10 + "; " + v12);
    g.expect(part_ok("S1") && part_ok("S14-B"),
        "J4: opus:AA-sprxptl.c6: the pointer pair contains the 20 bit address of sprite x's DMA data (p.28); it passes only if S1 (the 20-bit address each pair holds) and S14-B (the list read through pointer x belongs to sprite x) pass; parts: " + v1 + "; " + v14b);
    g.expect(part_ok("B3") && part_ok("B5-32") && part_ok("B5-64"),
        "J5: opus:AA-bplxdat.c10 sol:AA-bplxdat.c26: a bit-plane word is 16/32/64 pixels and its MSB is output first (p.15); it passes only if B3 (16-pixel words, FMODE = 0), B5-32 (32-pixel words, FMODE = 0x0001) and B5-64 (64-pixel words, FMODE = 0x0003) pass; parts: " +
        vs("B3") + "; " + vs("B5-32") + "; " + vs("B5-64"));
    g.expect(part_ok("S1") && part_ok("S2") && part_ok("S3") && part_ok("S4"),
        "J-CONTROL: the comparator of J1-J4 (every named part passed on this core) applied to the parts S1, S2, S3 and S4, which are graded and green: " + v1 + "; " + v2 + "; " + v3 + "; " + v4);
}

}  // namespace s3
