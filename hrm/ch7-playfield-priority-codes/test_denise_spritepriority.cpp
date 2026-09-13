// Test purpose: Table 7-2's PF1P2-PF1P0 code-to-placement table (printed
//   p.210) and its PF2P2-PF2P0 mirror (p.211, "in exactly the same way"),
//   swept exhaustively -- all 5 defined codes x all 4 sprite groups, for
//   both playfields (40 cells) -- plus the six-object fixture-liveness
//   precondition those 40 cells depend on.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 207-212 (Table 7-2, its p.209 descriptive-prose twin, the p.211
//   worked example, Figures 7-2 and 7-3).
// Package: ch7-playfield-priority-codes. Specification: SPECIFICATION.md
//   in this directory, committed as 873db3e before this file was written
//   or the RTL below was opened.
// Non-goals (see SPECIFICATION.md "Out of scope" / "Declined content" for
//   the claim-level accounting): sprite-group-to-sprite-group ordering
//   (denise_sprites, not this fixture); PF1-vs-PF2 relative priority /
//   PF2PRI (no claim in this package's 46 states a proposition about it);
//   on-screen visibility/colour of the PF1/PF2 boundary (this fixture's
//   only output is a 1-bit selection, no colour/RGB); the colour-0
//   clipping mechanism (asserted at ch3-dual-playfield's
//   CODE-ZERO-TRANSPARENT, not here).
//
// BOUNDARY (opened only after SPECIFICATION.md's commit, per the two-step
// gate): denise_spritepriority.v:5-11 -- input [5:0] bplcon2 (bits 2:0 =
// PF1P2-PF1P0, bits 5:3 = PF2P2-PF2P0), input [2:1] nplayfield (Verilator
// packs this non-zero-based range to a contiguous LSB-based scalar: bit 0
// of the packed value is Verilog index 1 / PF1, bit 1 is Verilog index 2 /
// PF2 -- reused from hrm/denise_spritepriority's already-built harness for
// this same module; tool plumbing, not a claim), input [7:0] nsprite (one
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
// MUTATION MECHANISM: every one of the 46 graded assertions (6 liveness +
// 40 sweep cells) is numbered 1-46, in the fixed order the code below
// generates them. `make MUT=<n>` defines MUT_ID=<n> for the build (see
// Makefile) and the one call site whose sequential id matches substitutes
// its mutated stimulus for its normal stimulus while leaving the expected
// value untouched -- a genuine, separately-compiled stimulus change per
// BUILD-BRIEF's "change what the module is asked to do... confirm the
// assertion goes red... revert", not a flipped expectation and not a
// shared rule applied once (a coordinator review asked, after the first
// version of this file described one shared rule for all 40 sweep cells,
// that every one of the 46 actually be rebuilt and confirmed individually
// -- "a derived proposition that is never measured is an assumption with
// a label on it" applies just as much to a mutation that is described but
// never run).
//
// MUTATION LOG (46/46 run, 12.09.2026, this agent, under WSL/Verilator:
// baseline `make` first -- 46 passed, 0 failed -- then `make MUT=<id>` for
// id = 1..46 in order, each a full `rm -rf obj_dir` rebuild, each log
// captured; a final plain `make` afterward re-confirmed the 46/0
// baseline). For every id below the result was identical in shape: 45
// passed, 1 failed -- the failing line was id <n> alone, all other 45
// lines still PASS. No id produced 0 failures (a dead mutation) or more
// than 1 (a collateral flip); no assertion other than the one named went
// red for any other id's build. Column 3 is the stimulus change; every
// liveness mutation withholds the object under test, every sweep
// mutation re-drives the priority code to the other defined Table 7-2 row
// (0-4) that states the opposite relation for that one cell -- both are
// legitimate "what the module is asked to do" changes, never a touched
// comparison or a flipped expected value.
//
//   id  1 LIVE-PF1        withhold PF1 (nplayfield[1]: 1->0)          -> 45 passed, 1 failed (id 1 only). Reverted.
//   id  2 LIVE-PF2        withhold PF2 (nplayfield[2]: 1->0)          -> 45 passed, 1 failed (id 2 only). Reverted.
//   id  3 LIVE-SP01       withhold SP01 (nsprite: 0x01->0x00)         -> 45 passed, 1 failed (id 3 only). Reverted.
//   id  4 LIVE-SP23       withhold SP23 (nsprite: 0x04->0x00)         -> 45 passed, 1 failed (id 4 only). Reverted.
//   id  5 LIVE-SP45       withhold SP45 (nsprite: 0x10->0x00)         -> 45 passed, 1 failed (id 5 only). Reverted.
//   id  6 LIVE-SP67       withhold SP67 (nsprite: 0x40->0x00)         -> 45 passed, 1 failed (id 6 only). Reverted.
//   id  7 PF1P-000-SP01   PF1P code 0->1                              -> 45 passed, 1 failed (id 7 only). Reverted.
//   id  8 PF1P-000-SP23   PF1P code 0->2                              -> 45 passed, 1 failed (id 8 only). Reverted.
//   id  9 PF1P-000-SP45   PF1P code 0->3                              -> 45 passed, 1 failed (id 9 only). Reverted.
//   id 10 PF1P-000-SP67   PF1P code 0->4                              -> 45 passed, 1 failed (id 10 only). Reverted.
//   id 11 PF1P-001-SP01   PF1P code 1->0                              -> 45 passed, 1 failed (id 11 only). Reverted.
//   id 12 PF1P-001-SP23   PF1P code 1->2                              -> 45 passed, 1 failed (id 12 only). Reverted.
//   id 13 PF1P-001-SP45   PF1P code 1->3                              -> 45 passed, 1 failed (id 13 only). Reverted.
//   id 14 PF1P-001-SP67   PF1P code 1->4                              -> 45 passed, 1 failed (id 14 only). Reverted.
//   id 15 PF1P-010-SP01   PF1P code 2->0                              -> 45 passed, 1 failed (id 15 only). Reverted.
//   id 16 PF1P-010-SP23   PF1P code 2->1                              -> 45 passed, 1 failed (id 16 only). Reverted.
//   id 17 PF1P-010-SP45   PF1P code 2->3                              -> 45 passed, 1 failed (id 17 only). Reverted.
//   id 18 PF1P-010-SP67   PF1P code 2->4                              -> 45 passed, 1 failed (id 18 only). Reverted.
//   id 19 PF1P-011-SP01   PF1P code 3->0                              -> 45 passed, 1 failed (id 19 only). Reverted.
//   id 20 PF1P-011-SP23   PF1P code 3->1                              -> 45 passed, 1 failed (id 20 only). Reverted.
//   id 21 PF1P-011-SP45   PF1P code 3->2                              -> 45 passed, 1 failed (id 21 only). Reverted.
//   id 22 PF1P-011-SP67   PF1P code 3->4                              -> 45 passed, 1 failed (id 22 only). Reverted.
//   id 23 PF1P-100-SP01   PF1P code 4->0                              -> 45 passed, 1 failed (id 23 only). Reverted.
//   id 24 PF1P-100-SP23   PF1P code 4->1                              -> 45 passed, 1 failed (id 24 only). Reverted.
//   id 25 PF1P-100-SP45   PF1P code 4->2                              -> 45 passed, 1 failed (id 25 only). Reverted.
//   id 26 PF1P-100-SP67   PF1P code 4->3                              -> 45 passed, 1 failed (id 26 only). Reverted.
//   id 27 PF2P-000-SP01   PF2P code 0->1                              -> 45 passed, 1 failed (id 27 only). Reverted.
//   id 28 PF2P-000-SP23   PF2P code 0->2                              -> 45 passed, 1 failed (id 28 only). Reverted.
//   id 29 PF2P-000-SP45   PF2P code 0->3                              -> 45 passed, 1 failed (id 29 only). Reverted.
//   id 30 PF2P-000-SP67   PF2P code 0->4                              -> 45 passed, 1 failed (id 30 only). Reverted.
//   id 31 PF2P-001-SP01   PF2P code 1->0                              -> 45 passed, 1 failed (id 31 only). Reverted.
//   id 32 PF2P-001-SP23   PF2P code 1->2                              -> 45 passed, 1 failed (id 32 only). Reverted.
//   id 33 PF2P-001-SP45   PF2P code 1->3                              -> 45 passed, 1 failed (id 33 only). Reverted.
//   id 34 PF2P-001-SP67   PF2P code 1->4                              -> 45 passed, 1 failed (id 34 only). Reverted.
//   id 35 PF2P-010-SP01   PF2P code 2->0                              -> 45 passed, 1 failed (id 35 only). Reverted.
//   id 36 PF2P-010-SP23   PF2P code 2->1                              -> 45 passed, 1 failed (id 36 only). Reverted.
//   id 37 PF2P-010-SP45   PF2P code 2->3                              -> 45 passed, 1 failed (id 37 only). Reverted.
//   id 38 PF2P-010-SP67   PF2P code 2->4                              -> 45 passed, 1 failed (id 38 only). Reverted.
//   id 39 PF2P-011-SP01   PF2P code 3->0                              -> 45 passed, 1 failed (id 39 only). Reverted.
//   id 40 PF2P-011-SP23   PF2P code 3->1                              -> 45 passed, 1 failed (id 40 only). Reverted.
//   id 41 PF2P-011-SP45   PF2P code 3->2                              -> 45 passed, 1 failed (id 41 only). Reverted.
//   id 42 PF2P-011-SP67   PF2P code 3->4                              -> 45 passed, 1 failed (id 42 only). Reverted.
//   id 43 PF2P-100-SP01   PF2P code 4->0                              -> 45 passed, 1 failed (id 43 only). Reverted.
//   id 44 PF2P-100-SP23   PF2P code 4->1                              -> 45 passed, 1 failed (id 44 only). Reverted.
//   id 45 PF2P-100-SP45   PF2P code 4->2                              -> 45 passed, 1 failed (id 45 only). Reverted.
//   id 46 PF2P-100-SP67   PF2P code 4->3                              -> 45 passed, 1 failed (id 46 only). Reverted.
//
// An earlier automated pass over this same 46-id loop mis-reported 5 ids
// (17, 24, 29, 35, 44) as producing zero failures; re-running those five
// individually (`make MUT=17` etc., output inspected directly) showed
// each correctly producing "45 passed, 1 failed" with only that id red --
// the mis-report was a bug in the shell comparison used to check the
// log (a leading-zero string mismatch plus one flaky capture), not a
// property of the build. The full 46-id loop was re-run afterward with a
// corrected checker and every one of the 46 came back OK-ISOLATED, which
// is the result recorded in the table above. `make` (MUT unset) is the
// baseline / revert build; it was run before, and again after, the full
// sweep, both times 46 passed, 0 failed.

#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vdenise_spritepriority.h"

namespace {

// ---- BUILD-BRIEF's fixed Checks class, verbatim, no other macro. ----
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
    int next_id = 1; // assigned sequentially, matches the MUT_ID legend above

    // -----------------------------------------------------------------
    // Fixture liveness / object identity -- ids 1-6: LIVE-PF1, LIVE-PF2,
    // LIVE-SP01, LIVE-SP23, LIVE-SP45, LIVE-SP67. These run first and are
    // the precondition for every sweep cell below: BUILD-BRIEF requires
    // the fixture prove each contending object's own wire actually moves
    // sprsel before any priority relation about it is trusted. Support:
    // opus:7-fig7-2.c9 (the two playfields, PF1 and PF2); Table 7-2's own
    // column headers (sol:7-video.c23..c27, opus:7-vp.c24..c28) naming
    // SP01/SP23/SP45/SP67. No page/claim rules on liveness itself -- this
    // is BUILD-BRIEF's own methodology, not a manual proposition.
    // -----------------------------------------------------------------
    int liveness_failed_before = checks.failed;

    // id 1: LIVE-PF1 -- PF1 active alone, SP01 armed, PF1P=0 (front for
    // any armed group) -> playfield selected. Mutation: withhold PF1
    // (nplayfield[1]=0); SP01 stays armed (sprcode stays 1, not the
    // no-sprite sentinel), so selection must flip to sprite if PF1's own
    // flag is genuinely wired into the decision.
    {
        int id = next_id++;
        dut->bplcon2 = 0x00;
        dut->nplayfield = (kActiveMutation == id) ? 0x00 : PF1_ON;
        dut->nsprite = GROUPS[0].bit;
        eval();
        char label[220];
        snprintf(label, sizeof label,
            "[%02d] LIVE-PF1: PF1 active, SP01 armed, PF1P=0 -> playfield "
            "selected (precondition for the PF1 sweep below; opus:7-fig7-2.c9)",
            id);
        checks.expect(dut->sprsel == 0, label);
    }

    // id 2: LIVE-PF2 -- symmetric with PF2.
    {
        int id = next_id++;
        dut->bplcon2 = 0x00;
        dut->nplayfield = (kActiveMutation == id) ? 0x00 : PF2_ON;
        dut->nsprite = GROUPS[0].bit;
        eval();
        char label[220];
        snprintf(label, sizeof label,
            "[%02d] LIVE-PF2: PF2 active, SP01 armed, PF2P=0 -> playfield "
            "selected (precondition for the PF2 sweep below; opus:7-fig7-2.c9)",
            id);
        checks.expect(dut->sprsel == 0, label);
    }

    // ids 3-6: LIVE-SP01, LIVE-SP23, LIVE-SP45, LIVE-SP67 -- no playfield
    // active, group G alone armed -> sprite selected (sprcode takes G's
    // own code, not the 7 "no valid sprite data" sentinel). Mutation:
    // withhold the group entirely (nsprite=0); sprcode becomes the
    // sentinel, so selection must flip to playfield even though no
    // playfield is active either -- this is what proves G's own nsprite
    // bits, specifically, are wired into "is any sprite valid" detection.
    for (int gi = 0; gi < 4; gi++) {
        int id = next_id++;
        dut->bplcon2 = 0x00;
        dut->nplayfield = 0x00;
        dut->nsprite = (kActiveMutation == id) ? 0x00 : GROUPS[gi].bit;
        eval();
        char label[220];
        snprintf(label, sizeof label,
            "[%02d] LIVE-%s: %s armed alone, no playfield active -> sprite "
            "selected (precondition for the %s sweep cells below; Table "
            "7-2's own column header)",
            id, GROUPS[gi].name, GROUPS[gi].name, GROUPS[gi].name);
        checks.expect(dut->sprsel == 1, label);
    }

    bool liveness_ok = (checks.failed == liveness_failed_before);
    checks.note(std::string("liveness gate: ") +
        (liveness_ok
             ? "6/6 passed -- the 40 sweep cells below are evidence"
             : "at least one of ids 1-6 failed -- the 40 sweep cells below "
               "are NOT evidence until this is resolved, per BUILD-BRIEF's "
               "liveness precondition"));

    // -----------------------------------------------------------------
    // PF1 exhaustive sweep -- ids 7-26 (5 codes x 4 groups). Table 7-2,
    // printed p.210 (sol:7-video.c23..c27, opus:7-vp.c24..c28), reinforced
    // by the p.209 descriptive prose (sol:7-video.c8..c12) and, for
    // specific cells, the p.211 worked example and Figures 7-2/7-3 -- full
    // per-cell citation list in SPECIFICATION.md.
    // -----------------------------------------------------------------
    for (int code = 0; code <= 4; code++) {
        for (int gi = 0; gi < 4; gi++) {
            int id = next_id++;
            int g = GROUPS[gi].code;
            bool expect_front = (g > code); // Table 7-2's own step pattern

            // MUTATION for this cell: re-drive PF1P to the other defined
            // code (0-4) that Table 7-2 itself states disagrees with this
            // row on this column -- (code<g)?g:(g-1) always lands on a
            // different row whose own printed relation for group g is the
            // opposite of expect_front. Expected value (expect_front)
            // itself is never touched.
            int mcode = (code < g) ? g : (g - 1);
            int actual_code = (kActiveMutation == id) ? mcode : code;

            dut->bplcon2 = (uint8_t)actual_code; // PF1P = actual_code, PF2P = 0
            dut->nplayfield = PF1_ON;
            dut->nsprite = GROUPS[gi].bit;
            eval();

            char bin[4];
            snprintf(bin, sizeof bin, "%d%d%d", (code >> 2) & 1, (code >> 1) & 1, code & 1);
            char label[260];
            snprintf(label, sizeof label,
                "[%02d] PF1P-%s-%s: PF1P=%s (code %d), %s armed -> PF1 %s "
                "(p.210 Table 7-2)",
                id, bin, GROUPS[gi].name, bin, code, GROUPS[gi].name,
                expect_front ? "front" : "behind");
            bool actual_front = (dut->sprsel == 0);
            checks.expect(actual_front == expect_front, label);
        }
    }

    // -----------------------------------------------------------------
    // PF2 exhaustive sweep -- ids 27-46 (5 codes x 4 groups), derived from
    // "Bits PF2P2-PF2P0 let you position playfield 2 among the sprite
    // priorities in exactly the same way" (sol:7-video.c28, opus:7-vp.
    // c30) and "You have the same possibilities for playfield 2" (sol:
    // 7-video.c13) -- PF1's grid with PF2 substituted. Each cell is graded
    // independently: a derivation that were never measured would be an
    // assumption with a label on it, not evidence the mirror actually
    // holds in this RTL.
    // -----------------------------------------------------------------
    for (int code = 0; code <= 4; code++) {
        for (int gi = 0; gi < 4; gi++) {
            int id = next_id++;
            int g = GROUPS[gi].code;
            bool expect_front = (g > code);

            int mcode = (code < g) ? g : (g - 1);
            int actual_code = (kActiveMutation == id) ? mcode : code;

            dut->bplcon2 = (uint8_t)(actual_code << 3); // PF2P = actual_code, PF1P = 0
            dut->nplayfield = PF2_ON;
            dut->nsprite = GROUPS[gi].bit;
            eval();

            char bin[4];
            snprintf(bin, sizeof bin, "%d%d%d", (code >> 2) & 1, (code >> 1) & 1, code & 1);
            char label[260];
            snprintf(label, sizeof label,
                "[%02d] PF2P-%s-%s: PF2P=%s (code %d), %s armed -> PF2 %s "
                "(derived from p.211 \"in exactly the same way\" + p.210 "
                "Table 7-2)",
                id, bin, GROUPS[gi].name, bin, code, GROUPS[gi].name,
                expect_front ? "front" : "behind");
            bool actual_front = (dut->sprsel == 0);
            checks.expect(actual_front == expect_front, label);
        }
    }

    // -----------------------------------------------------------------
    // Measured, not graded -- undefined codes 5-7. Page 209: "The numbers
    // 0 through 4 shown in parentheses ... are the actual values you use
    // to select the playfield priority positions" (sol:7-video.c14, twin
    // opus:7-vp.c12) -- both fields are 3 bits wide (0-7) but only 0-4 are
    // given a meaning. Per BUILD-BRIEF ("where the manual is silent,
    // measure and report -- do not assert"), no expect() here, and no
    // MUT_ID id is spent on these six -- there is nothing in the manual to
    // mutate against.
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
            char header[64];
            snprintf(header, sizeof header, "PF%dP=%s (%d): ", pf, bin, code);
            checks.note(std::string(header) + profile +
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
        "sol:7-video.c23..c27 / opus:7-vp.c24..c28's own split field");
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
