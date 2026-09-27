// Test purpose: Table 7-2's PF1P2-PF1P0 code-to-placement table (printed
//   p.210) and its PF2P2-PF2P0 mirror (p.211, "in exactly the same way"),
//   swept exhaustively -- all 5 defined codes x all 4 sprite groups, for
//   both playfields (40 cells) -- plus the six-object fixture-liveness
//   precondition those 40 cells depend on, plus two aggregate assertions
//   (ids 47/48) over the relationship BETWEEN those 40 cells: that each playfield's outcome is
//   changeable and reaches both front and behind, and that its five defined
//   codes produce five distinct placements.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 207-212 (Table 7-2, its p.209 descriptive-prose twin, the p.211
//   worked example, Figures 7-2 and 7-3).
// Package: ch7-playfield-priority-codes. Specification: SPECIFICATION.md
//   in this directory.
// Non-goals (see SPECIFICATION.md "Out of scope" / "Declined content" for
//   the claim-level accounting): sprite-group-to-sprite-group ordering
//   (denise_sprites, not this fixture); PF1-vs-PF2 relative priority /
//   PF2PRI (no claim in this package's 46 states a proposition about it);
//   on-screen visibility/colour of the PF1/PF2 boundary (this fixture's
//   only output is a 1-bit selection, no colour/RGB); the colour-0
//   clipping mechanism (asserted at ch3-dual-playfield's
//   CODE-ZERO-TRANSPARENT, not here).
// Non-goals: anything outside the `denise_spritepriority` fixture this
//   package builds, and any claim it does not cite. A green here
//   means those assertions held on this fixture at the pinned revision --
//   not that the module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: denise_spritepriority.v:5-11 -- input [5:0] bplcon2 (bits 2:0 =
// PF1P2-PF1P0, bits 5:3 = PF2P2-PF2P0), input [2:1] nplayfield (Verilator
// packs this non-zero-based range to a contiguous LSB-based scalar: bit 0
// of the packed value is Verilog index 1 / PF1, bit 1 is Verilog index 2 /
// PF2 -- tool plumbing, not a claim), input [7:0] nsprite (one
// flag per sprite 0-7, grouped in pairs into SP01/SP23/SP45/SP67), output
// reg sprsel (0 selects playfield data, 1 selects sprite data). Purely
// combinational: no clock port on the module, so every measurement below
// is one dut->eval() after setting inputs.
//
// RTL analysis (l.14-53): an internal sprcode is priority-encoded from
// nsprite's four sprgroup bits -- 1 for SP01, 2 for SP23, 3 for SP45, 4
// for SP67 (lowest-numbered active group wins if more than one group is
// armed), 7 as a sentinel when no sprite group is active at all. pf1front
// = sprcode > bplcon2[2:0]; pf2front = sprcode > bplcon2[5:3] -- exactly
// Table 7-2's own step pattern (a group is "in front of" a playfield code
// C iff the group's fixed rank exceeds C). The top-level select checks
// sprcode==7 first (forces sprsel=0, playfield, regardless of anything
// else -- there is no sprite to be behind), then pf1front&&nplayfield[1],
// then pf2front&&nplayfield[2], else sprsel=1. Driving only one of
// nplayfield's two bits at a time (as every assertion below does) removes
// any PF1-vs-PF2 interaction from the measurement, matching the "Out of
// scope" note in SPECIFICATION.md.
//
// LABEL SHAPE: tools/check_gates.py's per-assertion mutation gate reads an
// assertion's id from the FIRST STRING LITERAL of its expect() call and
// matches it against "// MUTATION <id> <kind> <result>" comments. So both
// sweeps and the liveness section are written out as individual
// checks.expect() calls, each with its own literal id as the leading token
// of its own label string (no snprintf, no std::string label variable, no
// id built by concatenation), and the mutation log below uses those same
// literal ids.
//
// MUTATION MECHANISM: every graded assertion is numbered 1-48 via
// kActiveMutation, matching the literal id in its own label 1:1 (ids 1-46
// in the log below, 47-48 in the SWEEP-SUMMARY records after it).
// `make MUT=<n>` defines MUT_ID=<n> for the build (see Makefile), and the
// one call site whose sequential number matches substitutes its mutated
// stimulus for its normal stimulus while leaving the expected value
// untouched -- a genuine, separately-compiled stimulus change: change what
// the module is asked to do, confirm the assertion goes red, revert.
// `make` (MUT unset) is the baseline / revert build.
//
// MUTATION LOG, ids 1-46 (WSL/Verilator; measured on a loop-bodied form of
// this file that drives the same dut->bplcon2 / ->nplayfield / ->nsprite /
// ->eval() sequence, in the same order, on the same values, as the calls
// below; its tallies count ids 1-46 only: baseline `make` first -- 46
// passed, 0 failed -- then `make MUT=<id>` for id = 1..46 in order, each a
// full `rm -rf obj_dir` rebuild, each log captured; a final plain `make`
// afterward re-confirmed the 46/0 baseline). For every id below the result
// was identical in shape: 45 passed, 1 failed -- the failing line was that
// id alone, all other 45 lines still PASS; no id produced 0 failures (a
// dead mutation) or more than 1 (a collateral flip). The ids in these
// records are identical, character for character, to the leading token of
// the corresponding assertion's own label below.
//
// MUTATION LIVE-PF1 stimulus red: withhold PF1 (nplayfield[1]: 1->0), SP01 stays armed -> 45 passed, 1 failed (LIVE-PF1 only). Reverted.
// MUTATION LIVE-PF2 stimulus red: withhold PF2 (nplayfield[2]: 1->0), SP01 stays armed -> 45 passed, 1 failed (LIVE-PF2 only). Reverted.
// MUTATION LIVE-SP01 stimulus red: withhold SP01 (nsprite: 0x01->0x00) -> 45 passed, 1 failed (LIVE-SP01 only). Reverted.
// MUTATION LIVE-SP23 stimulus red: withhold SP23 (nsprite: 0x04->0x00) -> 45 passed, 1 failed (LIVE-SP23 only). Reverted.
// MUTATION LIVE-SP45 stimulus red: withhold SP45 (nsprite: 0x10->0x00) -> 45 passed, 1 failed (LIVE-SP45 only). Reverted.
// MUTATION LIVE-SP67 stimulus red: withhold SP67 (nsprite: 0x40->0x00) -> 45 passed, 1 failed (LIVE-SP67 only). Reverted.
// MUTATION PF1P-000-SP01 stimulus red: PF1P code 0->1 -> 45 passed, 1 failed (PF1P-000-SP01 only). Reverted.
// MUTATION PF1P-000-SP23 stimulus red: PF1P code 0->2 -> 45 passed, 1 failed (PF1P-000-SP23 only). Reverted.
// MUTATION PF1P-000-SP45 stimulus red: PF1P code 0->3 -> 45 passed, 1 failed (PF1P-000-SP45 only). Reverted.
// MUTATION PF1P-000-SP67 stimulus red: PF1P code 0->4 -> 45 passed, 1 failed (PF1P-000-SP67 only). Reverted.
// MUTATION PF1P-001-SP01 stimulus red: PF1P code 1->0 -> 45 passed, 1 failed (PF1P-001-SP01 only). Reverted.
// MUTATION PF1P-001-SP23 stimulus red: PF1P code 1->2 -> 45 passed, 1 failed (PF1P-001-SP23 only). Reverted.
// MUTATION PF1P-001-SP45 stimulus red: PF1P code 1->3 -> 45 passed, 1 failed (PF1P-001-SP45 only). Reverted.
// MUTATION PF1P-001-SP67 stimulus red: PF1P code 1->4 -> 45 passed, 1 failed (PF1P-001-SP67 only). Reverted.
// MUTATION PF1P-010-SP01 stimulus red: PF1P code 2->0 -> 45 passed, 1 failed (PF1P-010-SP01 only). Reverted.
// MUTATION PF1P-010-SP23 stimulus red: PF1P code 2->1 -> 45 passed, 1 failed (PF1P-010-SP23 only). Reverted.
// MUTATION PF1P-010-SP45 stimulus red: PF1P code 2->3 -> 45 passed, 1 failed (PF1P-010-SP45 only). Reverted.
// MUTATION PF1P-010-SP67 stimulus red: PF1P code 2->4 -> 45 passed, 1 failed (PF1P-010-SP67 only). Reverted.
// MUTATION PF1P-011-SP01 stimulus red: PF1P code 3->0 -> 45 passed, 1 failed (PF1P-011-SP01 only). Reverted.
// MUTATION PF1P-011-SP23 stimulus red: PF1P code 3->1 -> 45 passed, 1 failed (PF1P-011-SP23 only). Reverted.
// MUTATION PF1P-011-SP45 stimulus red: PF1P code 3->2 -> 45 passed, 1 failed (PF1P-011-SP45 only). Reverted.
// MUTATION PF1P-011-SP67 stimulus red: PF1P code 3->4 -> 45 passed, 1 failed (PF1P-011-SP67 only). Reverted.
// MUTATION PF1P-100-SP01 stimulus red: PF1P code 4->0 -> 45 passed, 1 failed (PF1P-100-SP01 only). Reverted.
// MUTATION PF1P-100-SP23 stimulus red: PF1P code 4->1 -> 45 passed, 1 failed (PF1P-100-SP23 only). Reverted.
// MUTATION PF1P-100-SP45 stimulus red: PF1P code 4->2 -> 45 passed, 1 failed (PF1P-100-SP45 only). Reverted.
// MUTATION PF1P-100-SP67 stimulus red: PF1P code 4->3 -> 45 passed, 1 failed (PF1P-100-SP67 only). Reverted.
// MUTATION PF2P-000-SP01 stimulus red: PF2P code 0->1 -> 45 passed, 1 failed (PF2P-000-SP01 only). Reverted.
// MUTATION PF2P-000-SP23 stimulus red: PF2P code 0->2 -> 45 passed, 1 failed (PF2P-000-SP23 only). Reverted.
// MUTATION PF2P-000-SP45 stimulus red: PF2P code 0->3 -> 45 passed, 1 failed (PF2P-000-SP45 only). Reverted.
// MUTATION PF2P-000-SP67 stimulus red: PF2P code 0->4 -> 45 passed, 1 failed (PF2P-000-SP67 only). Reverted.
// MUTATION PF2P-001-SP01 stimulus red: PF2P code 1->0 -> 45 passed, 1 failed (PF2P-001-SP01 only). Reverted.
// MUTATION PF2P-001-SP23 stimulus red: PF2P code 1->2 -> 45 passed, 1 failed (PF2P-001-SP23 only). Reverted.
// MUTATION PF2P-001-SP45 stimulus red: PF2P code 1->3 -> 45 passed, 1 failed (PF2P-001-SP45 only). Reverted.
// MUTATION PF2P-001-SP67 stimulus red: PF2P code 1->4 -> 45 passed, 1 failed (PF2P-001-SP67 only). Reverted.
// MUTATION PF2P-010-SP01 stimulus red: PF2P code 2->0 -> 45 passed, 1 failed (PF2P-010-SP01 only). Reverted.
// MUTATION PF2P-010-SP23 stimulus red: PF2P code 2->1 -> 45 passed, 1 failed (PF2P-010-SP23 only). Reverted.
// MUTATION PF2P-010-SP45 stimulus red: PF2P code 2->3 -> 45 passed, 1 failed (PF2P-010-SP45 only). Reverted.
// MUTATION PF2P-010-SP67 stimulus red: PF2P code 2->4 -> 45 passed, 1 failed (PF2P-010-SP67 only). Reverted.
// MUTATION PF2P-011-SP01 stimulus red: PF2P code 3->0 -> 45 passed, 1 failed (PF2P-011-SP01 only). Reverted.
// MUTATION PF2P-011-SP23 stimulus red: PF2P code 3->1 -> 45 passed, 1 failed (PF2P-011-SP23 only). Reverted.
// MUTATION PF2P-011-SP45 stimulus red: PF2P code 3->2 -> 45 passed, 1 failed (PF2P-011-SP45 only). Reverted.
// MUTATION PF2P-011-SP67 stimulus red: PF2P code 3->4 -> 45 passed, 1 failed (PF2P-011-SP67 only). Reverted.
// MUTATION PF2P-100-SP01 stimulus red: PF2P code 4->0 -> 45 passed, 1 failed (PF2P-100-SP01 only). Reverted.
// MUTATION PF2P-100-SP23 stimulus red: PF2P code 4->1 -> 45 passed, 1 failed (PF2P-100-SP23 only). Reverted.
// MUTATION PF2P-100-SP45 stimulus red: PF2P code 4->2 -> 45 passed, 1 failed (PF2P-100-SP45 only). Reverted.
// MUTATION PF2P-100-SP67 stimulus red: PF2P code 4->3 -> 45 passed, 1 failed (PF2P-100-SP67 only). Reverted.
//
// MUTATION LOG, ids 47-48 (SWEEP-SUMMARY, see its own comment below;
// baseline 48 passed, 0 failed; `make` between each MUT run, final `make`
// confirmed 48 passed, 0 failed restored):
// MUTATION SWEEP-PF1-VARIABILITY-AND-FIVE-POSITIONS stimulus red: built with
//   `make MUT=47`, which substitutes PF1 code 4's stimulus with code 0's
//   inside this summary section's own re-derivation loop only (ids 1-46
//   above do not re-run this loop and are untouched) -> 47 passed, 1
//   failed (id 47 alone; ids 1-46 and id 48 stayed PASS). Reverted,
//   rebuilt: 48/0 restored.
// MUTATION SWEEP-PF2-VARIABILITY-AND-FIVE-POSITIONS stimulus red: built
//   with `make MUT=48`, the same substitution applied to PF2's code 4
//   instead -> 47 passed, 1 failed (id 48 alone; ids 1-46 and id 47 stayed
//   PASS). Reverted, rebuilt: 48/0 restored.

#include <array>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vdenise_spritepriority.h"

namespace {

// ---- The suite's fixed Checks class, no other macro. ----
class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

#ifdef MUT_ID
constexpr int kActiveMutation = MUT_ID;
#else
constexpr int kActiveMutation = 0; // 0 = no assertion has id 0 -> baseline
#endif

struct Group { const char* name; uint8_t bit; int code; };
const Group GROUPS[4] = {
    {"SP01", 0x01, 1},
    {"SP23", 0x04, 2},
    {"SP45", 0x10, 3},
    {"SP67", 0x40, 4},
};

constexpr uint8_t PF1_ON = 0x01;
constexpr uint8_t PF2_ON = 0x02;

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise_spritepriority* dut = new Vdenise_spritepriority;
    Checks checks;

    auto eval = [&]() { dut->eval(); };

    // measure_pf1/measure_pf2/measure_pf_live/measure_group_live each drive
    // one stimulus through the ports (bplcon2, nplayfield, nsprite, then
    // eval()) and return what the assertions below compare.
    auto measure_pf1 = [&](int code, uint8_t group_bit) -> bool {
        dut->bplcon2 = (uint8_t)code;      // PF1P = code, PF2P = 0
        dut->nplayfield = PF1_ON;
        dut->nsprite = group_bit;
        dut->eval();
        return dut->sprsel == 0;           // true = PF1 front
    };
    auto measure_pf2 = [&](int code, uint8_t group_bit) -> bool {
        dut->bplcon2 = (uint8_t)(code << 3); // PF2P = code, PF1P = 0
        dut->nplayfield = PF2_ON;
        dut->nsprite = group_bit;
        dut->eval();
        return dut->sprsel == 0;           // true = PF2 front
    };
    auto measure_pf_live = [&](uint8_t pf_mask) -> int {
        dut->bplcon2 = 0x00;
        dut->nplayfield = pf_mask;
        dut->nsprite = GROUPS[0].bit;      // SP01 armed, front for PF1P=PF2P=0
        dut->eval();
        return dut->sprsel;
    };
    auto measure_group_live = [&](uint8_t group_mask) -> int {
        dut->bplcon2 = 0x00;
        dut->nplayfield = 0x00;
        dut->nsprite = group_mask;
        dut->eval();
        return dut->sprsel;
    };

    // -----------------------------------------------------------------
    // Fixture liveness / object identity -- LIVE-PF1, LIVE-PF2, LIVE-SP01,
    // LIVE-SP23, LIVE-SP45, LIVE-SP67 (ids 1-6). These run first and are
    // the precondition for every sweep cell below: the fixture must prove
    // each contending object's own wire actually moves sprsel before any
    // priority relation about it is trusted. Support:
    // opus:7-fig7-2.c9 (the two playfields, PF1 and PF2); Table 7-2's own
    // column headers (sol:7-video.c23..c27, opus:7-vp.c24..c28) naming
    // SP01/SP23/SP45/SP67. No page/claim rules on liveness itself -- this
    // is the suite's methodology, not a manual proposition.
    // -----------------------------------------------------------------
    int liveness_failed_before = checks.failed;

    checks.expect(measure_pf_live((kActiveMutation == 1) ? 0x00 : PF1_ON) == 0,
        "LIVE-PF1: PF1 active, SP01 armed, PF1P=0 -> playfield selected "
        "(precondition for the PF1 sweep below; opus:7-fig7-2.c9)");
    checks.expect(measure_pf_live((kActiveMutation == 2) ? 0x00 : PF2_ON) == 0,
        "LIVE-PF2: PF2 active, SP01 armed, PF2P=0 -> playfield selected "
        "(precondition for the PF2 sweep below; opus:7-fig7-2.c9)");
    checks.expect(measure_group_live((kActiveMutation == 3) ? 0x00 : 0x01) == 1,
        "LIVE-SP01: SP01 armed alone, no playfield active -> sprite "
        "selected (precondition for the SP01 sweep cells below; Table "
        "7-2's own column header)");
    checks.expect(measure_group_live((kActiveMutation == 4) ? 0x00 : 0x04) == 1,
        "LIVE-SP23: SP23 armed alone, no playfield active -> sprite "
        "selected (precondition for the SP23 sweep cells below; Table "
        "7-2's own column header)");
    checks.expect(measure_group_live((kActiveMutation == 5) ? 0x00 : 0x10) == 1,
        "LIVE-SP45: SP45 armed alone, no playfield active -> sprite "
        "selected (precondition for the SP45 sweep cells below; Table "
        "7-2's own column header)");
    checks.expect(measure_group_live((kActiveMutation == 6) ? 0x00 : 0x40) == 1,
        "LIVE-SP67: SP67 armed alone, no playfield active -> sprite "
        "selected (precondition for the SP67 sweep cells below; Table "
        "7-2's own column header)");

    bool liveness_ok = (checks.failed == liveness_failed_before);
    checks.note(std::string("liveness gate: ") +
        (liveness_ok
             ? "6/6 passed -- the 40 sweep cells below are evidence"
             : "at least one of LIVE-PF1..LIVE-SP67 failed -- the 40 sweep "
               "cells below are NOT evidence until this is resolved, per "
               "the liveness precondition"));

    // -----------------------------------------------------------------
    // PF1 exhaustive sweep -- PF1P-<code>-<group> (20 cells, ids 7-26).
    // Table 7-2, printed p.210 (sol:7-video.c23..c27, opus:7-vp.c24..c28),
    // reinforced by the p.209 descriptive prose (sol:7-video.c8..c12) and,
    // for specific cells, the p.211 worked example and Figures 7-2/7-3 --
    // full per-cell citation list in SPECIFICATION.md.
    // -----------------------------------------------------------------
    checks.expect(measure_pf1((kActiveMutation == 7) ? 1 : 0, 0x01),
        "PF1P-000-SP01: PF1P=000 (code 0), SP01 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c8, sol:7-video.c23, opus:7-vp.c24, sol:7-video.c32, opus:7-vp.c34, opus:7-vp.c36, opus:7-vp.c37, sol:7-video.c35, sol:7-video.c36, sol:7-fig7-3.c2)");
    checks.expect(measure_pf1((kActiveMutation == 8) ? 2 : 0, 0x04),
        "PF1P-000-SP23: PF1P=000 (code 0), SP23 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c8, sol:7-video.c23, opus:7-vp.c24)");
    checks.expect(measure_pf1((kActiveMutation == 9) ? 3 : 0, 0x10),
        "PF1P-000-SP45: PF1P=000 (code 0), SP45 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c8, sol:7-video.c23, opus:7-vp.c24)");
    checks.expect(measure_pf1((kActiveMutation == 10) ? 4 : 0, 0x40),
        "PF1P-000-SP67: PF1P=000 (code 0), SP67 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c8, sol:7-video.c23, opus:7-vp.c24)");
    checks.expect(!measure_pf1((kActiveMutation == 11) ? 0 : 1, 0x01),
        "PF1P-001-SP01: PF1P=001 (code 1), SP01 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c9, sol:7-video.c24, opus:7-vp.c25, opus:7-fig7-2.c10, sol:7-fig7-2.c5)");
    checks.expect(measure_pf1((kActiveMutation == 12) ? 2 : 1, 0x04),
        "PF1P-001-SP23: PF1P=001 (code 1), SP23 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c9, sol:7-video.c24, opus:7-vp.c25, opus:7-fig7-2.c10, sol:7-fig7-2.c6)");
    checks.expect(measure_pf1((kActiveMutation == 13) ? 3 : 1, 0x10),
        "PF1P-001-SP45: PF1P=001 (code 1), SP45 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c9, sol:7-video.c24, opus:7-vp.c25)");
    checks.expect(measure_pf1((kActiveMutation == 14) ? 4 : 1, 0x40),
        "PF1P-001-SP67: PF1P=001 (code 1), SP67 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c9, sol:7-video.c24, opus:7-vp.c25)");
    checks.expect(!measure_pf1((kActiveMutation == 15) ? 0 : 2, 0x01),
        "PF1P-010-SP01: PF1P=010 (code 2), SP01 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c10, sol:7-video.c25, opus:7-vp.c26)");
    checks.expect(!measure_pf1((kActiveMutation == 16) ? 1 : 2, 0x04),
        "PF1P-010-SP23: PF1P=010 (code 2), SP23 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c10, sol:7-video.c25, opus:7-vp.c26)");
    checks.expect(measure_pf1((kActiveMutation == 17) ? 3 : 2, 0x10),
        "PF1P-010-SP45: PF1P=010 (code 2), SP45 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c10, sol:7-video.c25, opus:7-vp.c26)");
    checks.expect(measure_pf1((kActiveMutation == 18) ? 4 : 2, 0x40),
        "PF1P-010-SP67: PF1P=010 (code 2), SP67 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c10, sol:7-video.c25, opus:7-vp.c26)");
    checks.expect(!measure_pf1((kActiveMutation == 19) ? 0 : 3, 0x01),
        "PF1P-011-SP01: PF1P=011 (code 3), SP01 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c11, sol:7-video.c26, opus:7-vp.c27)");
    checks.expect(!measure_pf1((kActiveMutation == 20) ? 1 : 3, 0x04),
        "PF1P-011-SP23: PF1P=011 (code 3), SP23 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c11, sol:7-video.c26, opus:7-vp.c27)");
    checks.expect(!measure_pf1((kActiveMutation == 21) ? 2 : 3, 0x10),
        "PF1P-011-SP45: PF1P=011 (code 3), SP45 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c11, sol:7-video.c26, opus:7-vp.c27)");
    checks.expect(measure_pf1((kActiveMutation == 22) ? 4 : 3, 0x40),
        "PF1P-011-SP67: PF1P=011 (code 3), SP67 armed -> PF1 front (p.210 Table 7-2; sol:7-video.c11, sol:7-video.c26, opus:7-vp.c27)");
    checks.expect(!measure_pf1((kActiveMutation == 23) ? 0 : 4, 0x01),
        "PF1P-100-SP01: PF1P=100 (code 4), SP01 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c12, sol:7-video.c27, opus:7-vp.c28)");
    checks.expect(!measure_pf1((kActiveMutation == 24) ? 1 : 4, 0x04),
        "PF1P-100-SP23: PF1P=100 (code 4), SP23 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c12, sol:7-video.c27, opus:7-vp.c28)");
    checks.expect(!measure_pf1((kActiveMutation == 25) ? 2 : 4, 0x10),
        "PF1P-100-SP45: PF1P=100 (code 4), SP45 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c12, sol:7-video.c27, opus:7-vp.c28)");
    checks.expect(!measure_pf1((kActiveMutation == 26) ? 3 : 4, 0x40),
        "PF1P-100-SP67: PF1P=100 (code 4), SP67 armed -> PF1 behind (p.210 Table 7-2; sol:7-video.c12, sol:7-video.c27, opus:7-vp.c28)");

    // -----------------------------------------------------------------
    // PF2 exhaustive sweep -- PF2P-<code>-<group> (20 cells, ids 27-46),
    // derived from "Bits PF2P2-PF2P0 let you position playfield 2 among
    // the sprite priorities in exactly the same way" (sol:7-video.c28,
    // opus:7-vp.c30) and "You have the same possibilities for playfield 2"
    // (sol:7-video.c13) -- PF1's grid with PF2 substituted. Each cell is
    // graded independently: a derivation that were never measured would be
    // an assumption with a label on it, not evidence the mirror actually
    // holds in this RTL.
    // -----------------------------------------------------------------
    checks.expect(measure_pf2((kActiveMutation == 27) ? 1 : 0, 0x01),
        "PF2P-000-SP01: PF2P=000 (code 0), SP01 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(measure_pf2((kActiveMutation == 28) ? 2 : 0, 0x04),
        "PF2P-000-SP23: PF2P=000 (code 0), SP23 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(measure_pf2((kActiveMutation == 29) ? 3 : 0, 0x10),
        "PF2P-000-SP45: PF2P=000 (code 0), SP45 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(measure_pf2((kActiveMutation == 30) ? 4 : 0, 0x40),
        "PF2P-000-SP67: PF2P=000 (code 0), SP67 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 31) ? 0 : 1, 0x01),
        "PF2P-001-SP01: PF2P=001 (code 1), SP01 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(measure_pf2((kActiveMutation == 32) ? 2 : 1, 0x04),
        "PF2P-001-SP23: PF2P=001 (code 1), SP23 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(measure_pf2((kActiveMutation == 33) ? 3 : 1, 0x10),
        "PF2P-001-SP45: PF2P=001 (code 1), SP45 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(measure_pf2((kActiveMutation == 34) ? 4 : 1, 0x40),
        "PF2P-001-SP67: PF2P=001 (code 1), SP67 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 35) ? 0 : 2, 0x01),
        "PF2P-010-SP01: PF2P=010 (code 2), SP01 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30, sol:7-video.c33, opus:7-vp.c34, opus:7-vp.c36, opus:7-vp.c37, sol:7-video.c35, sol:7-fig7-3.c1, opus:7-fig7-3.c1)");
    checks.expect(!measure_pf2((kActiveMutation == 36) ? 1 : 2, 0x04),
        "PF2P-010-SP23: PF2P=010 (code 2), SP23 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30, sol:7-video.c33, opus:7-vp.c34, opus:7-vp.c36, opus:7-vp.c37, opus:7-fig7-2.c11, sol:7-fig7-2.c7)");
    checks.expect(measure_pf2((kActiveMutation == 37) ? 3 : 2, 0x10),
        "PF2P-010-SP45: PF2P=010 (code 2), SP45 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30, opus:7-fig7-2.c11, sol:7-fig7-2.c8)");
    checks.expect(measure_pf2((kActiveMutation == 38) ? 4 : 2, 0x40),
        "PF2P-010-SP67: PF2P=010 (code 2), SP67 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 39) ? 0 : 3, 0x01),
        "PF2P-011-SP01: PF2P=011 (code 3), SP01 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 40) ? 1 : 3, 0x04),
        "PF2P-011-SP23: PF2P=011 (code 3), SP23 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 41) ? 2 : 3, 0x10),
        "PF2P-011-SP45: PF2P=011 (code 3), SP45 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(measure_pf2((kActiveMutation == 42) ? 4 : 3, 0x40),
        "PF2P-011-SP67: PF2P=011 (code 3), SP67 armed -> PF2 front (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 43) ? 0 : 4, 0x01),
        "PF2P-100-SP01: PF2P=100 (code 4), SP01 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 44) ? 1 : 4, 0x04),
        "PF2P-100-SP23: PF2P=100 (code 4), SP23 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 45) ? 2 : 4, 0x10),
        "PF2P-100-SP45: PF2P=100 (code 4), SP45 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");
    checks.expect(!measure_pf2((kActiveMutation == 46) ? 3 : 4, 0x40),
        "PF2P-100-SP67: PF2P=100 (code 4), SP67 armed -> PF2 behind (derived from p.211 \"in exactly the same way\" + p.210 Table 7-2; sol:7-video.c13, sol:7-video.c28, opus:7-vp.c30)");

    // -----------------------------------------------------------------
    // SWEEP-SUMMARY -- aggregate assertions (ids 47, 48) for the general/
    // definitional claims that are ABOUT the sweep as a whole, not about
    // any one cell: opus:7-vp.c3 ("priority... changeable", p.207),
    // opus:7-vp.c10 ("either playfield can be positioned in front of or
    // behind the sprites", p.209), sol:7-video.c7 / its twin opus:7-vp.c11
    // ("each playfield has five selectable positions", p.209), and
    // opus:7-vp.c21 ("the binary value in PF1P2-PF1P0... as shown in
    // Table 7-2", p.210 -- PF1 only, per its own text).
    //
    // A record() cannot demonstrate anything; only an expect() can. None
    // of the five is about one cell, so citing them on individual
    // PF1P-*/PF2P-* labels would be a fabricated per-cell citation.
    // Checked against pp.207-211, all five ARE demonstrated by the sweep,
    // but only as an AGGREGATE fact across cells (the outcome actually
    // differs across codes; the five codes give five DISTINCT profiles) --
    // a fact about the relationship BETWEEN cells, not identical to any
    // single cell's own front/behind value, and therefore assertable on its
    // own graded line without duplicating or replacing any of the 40 cells
    // above. The lines below re-derive the same profiles the 40 cells above
    // measure, from the same measure_pf1/measure_pf2 stimulus (no new
    // fixture behaviour introduced).
    // -----------------------------------------------------------------
    auto profile = [&](bool pf1, int code) -> std::array<bool,4> {
        std::array<bool,4> p{};
        for (int gi = 0; gi < 4; gi++)
            p[gi] = pf1 ? measure_pf1(code, GROUPS[gi].bit)
                        : measure_pf2(code, GROUPS[gi].bit);
        return p;
    };
    std::array<std::array<bool,4>,5> pf1_profiles{}, pf2_profiles{};
    for (int code = 0; code <= 4; code++) {
        // MUTATION SWEEP-PF1-VARIABILITY-AND-FIVE-POSITIONS / SWEEP-PF2-
        // VARIABILITY-AND-FIVE-POSITIONS (ids 47/48): substitute code 4's
        // stimulus with code 0's for the named playfield only, collapsing
        // that playfield's five profiles to four distinct ones. Local to
        // this summary re-derivation -- none of PF1P-*/PF2P-* ids 7-46
        // above re-run this loop, so they are untouched by ids 47/48.
        int pf1_code = (kActiveMutation == 47 && code == 4) ? 0 : code;
        int pf2_code = (kActiveMutation == 48 && code == 4) ? 0 : code;
        pf1_profiles[code] = profile(true, pf1_code);
        pf2_profiles[code] = profile(false, pf2_code);
    }
    bool pf1_has_front = false, pf1_has_behind = false, pf1_all_distinct = true;
    bool pf2_has_front = false, pf2_has_behind = false, pf2_all_distinct = true;
    for (int c = 0; c <= 4; c++) {
        for (int g = 0; g < 4; g++) {
            (pf1_profiles[c][g] ? pf1_has_front : pf1_has_behind) = true;
            (pf2_profiles[c][g] ? pf2_has_front : pf2_has_behind) = true;
        }
        for (int c2 = c + 1; c2 <= 4; c2++) {
            if (pf1_profiles[c] == pf1_profiles[c2]) pf1_all_distinct = false;
            if (pf2_profiles[c] == pf2_profiles[c2]) pf2_all_distinct = false;
        }
    }
    checks.expect(pf1_has_front && pf1_has_behind && pf1_all_distinct,
        "SWEEP-PF1-VARIABILITY-AND-FIVE-POSITIONS: across PF1's five defined "
        "codes (0-4) swept against all four sprite groups above, playfield 1 "
        "reaches BOTH front and behind against some group (opus:7-vp.c3 p.207 "
        "'priority... may be changed relative to sprites'; opus:7-vp.c10 p.209 "
        "'you can position the playfields in front of or behind the sprites'), "
        "and the five codes produce five pairwise-DISTINCT front/behind "
        "profiles (sol:7-video.c7, opus:7-vp.c11 p.209 'five possible "
        "positions'; opus:7-vp.c21 p.210 'the binary values... determine "
        "where playfield 1 occurs in the priority chain as shown in Table "
        "7-2' -- PF1 only, per that claim's own text)");
    checks.expect(pf2_has_front && pf2_has_behind && pf2_all_distinct,
        "SWEEP-PF2-VARIABILITY-AND-FIVE-POSITIONS: across PF2's five defined "
        "codes (0-4) swept against all four sprite groups above, playfield 2 "
        "reaches BOTH front and behind against some group (opus:7-vp.c3 p.207; "
        "opus:7-vp.c10 p.209, both stated for 'either playfield'), and the "
        "five codes produce five pairwise-DISTINCT front/behind profiles "
        "(sol:7-video.c7, opus:7-vp.c11 p.209, 'you have the same "
        "possibilities for playfield 2'); opus:7-vp.c21 is PF1-specific by "
        "its own text and is not cited here");

    // -----------------------------------------------------------------
    // Measured, not graded -- undefined codes 5-7. Page 209: "The numbers
    // 0 through 4 shown in parentheses ... are the actual values you use
    // to select the playfield priority positions" (sol:7-video.c14, twin
    // opus:7-vp.c12) -- both fields are 3 bits wide (0-7) but only 0-4 are
    // given a meaning. Where the manual is silent, measure and report --
    // do not assert: no expect() here.
    // -----------------------------------------------------------------
    for (int pf = 1; pf <= 2; pf++) {
        for (int code = 5; code <= 7; code++) {
            std::string profile;
            for (int gi = 0; gi < 4; gi++) {
                dut->bplcon2 = (pf == 1) ? (uint8_t)code : (uint8_t)(code << 3);
                dut->nplayfield = (pf == 1) ? PF1_ON : PF2_ON;
                dut->nsprite = GROUPS[gi].bit;
                eval();
                profile += std::string(GROUPS[gi].name) + "=" +
                           ((dut->sprsel == 0) ? "front " : "behind ");
            }
            char bin[4];
            snprintf(bin, sizeof bin, "%d%d%d", (code >> 2) & 1, (code >> 1) & 1, code & 1);
            char headerbuf[64];
            snprintf(headerbuf, sizeof headerbuf, "PF%dP=%s (%d): ", pf, bin, code);
            checks.note(std::string(headerbuf) + profile +
                "-- manual does not define this code; not asserted (sol:"
                "7-video.c14, opus:7-vp.c12)");
        }
    }

    // -----------------------------------------------------------------
    // Declined content -- audit trail matching SPECIFICATION.md's
    // "Declined content" / "Out of scope" sections. Not measurements:
    // these are structural boundary facts, recorded once, not per-cell.
    // -----------------------------------------------------------------
    checks.record("the sprite-group-to-sprite-group half of every Table "
        "7-2 row (e.g. SP01 vs SP23 with neither playfield contending) is "
        "observable at denise_sprites, not at denise_spritepriority -- "
        // Written out rather than as `c23..c27`: uid_expand reads the first id
        // of a `..` range and silently drops the rest, so a marked form using
        // one would claim nine fewer uids than it names.
        "(half graded: sol:7-video.c23, sol:7-video.c24, sol:7-video.c25, "
        "sol:7-video.c26, sol:7-video.c27, opus:7-vp.c24, opus:7-vp.c25, "
        "opus:7-vp.c26, opus:7-vp.c27, opus:7-vp.c28) own split field");
    checks.record("visibility of the PF1/PF2 boundary on screen is out of "
        "scope: this fixture's only output is the 1-bit sprsel, no colour "
        "or RGB -- sol:7-video.c37, sol:7-fig7-3.c3, opus:7-fig7-3.c4 "
        "splits, each with reason_kind=insufficient in the claims file");
    checks.record("the colour-0 clipping mechanism (opus:7-fig7-3.c2 "
        "split) is out of scope here -- already asserted at "
        "ch3-dual-playfield's CODE-ZERO-TRANSPARENT, per that claim's own "
        "note");
    checks.record("PF1-vs-PF2 relative priority (the worked-example "
        "crossread's PF2PRI) is out of scope -- no claim in this "
        "package's 46 states a proposition about it; see SPECIFICATION.md "
        "\"Out of scope for this package\"");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    delete dut;
    return checks.failed == 0 ? 0 : 1;
}
