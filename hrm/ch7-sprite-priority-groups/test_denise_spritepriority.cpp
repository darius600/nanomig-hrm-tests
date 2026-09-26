// ch7-sprite-priority-groups -- denise_spritepriority conformance test
//
// Test purpose: chapter 7's statement that, for playfield-priority purposes,
//   the eight sprites are treated as four fixed groups of two -- (0,1),
//   (2,3), (4,5), (6,7) -- at the denise_spritepriority module boundary.
//   Package ch7-sprite-priority-groups, fixture denise_spritepriority (one
//   Verilator build, this module only), per this package's own claims file
//   ch7-sprite-priority-groups.jsonl (18 records).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed pp. 208-209 (body
//   text p.208, Figure 7-2 p.209). A red result below means
//   denise_spritepriority.v disagrees with the manual, not that the
//   assertion is wrong -- no assertion here was, or should be, adjusted to
//   make the RTL pass.
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
// ORDER OF WORK: SPECIFICATION.md in this directory (assertions, uids,
// scope) was written and committed (c9ac428) from the claims file's own
// quotations alone -- denise_spritepriority.v was not opened until after
// that commit, per the build brief's gate. Everything below that depends on
// a port name, bit width, or the internal grouping/priority-encoder
// structure was learned only now, opening the RTL for the first time for
// this package.
//
// WHAT THE RTL SHOWS (read for the first time after the spec-only commit):
//   denise_spritepriority.v:5-11 port list -- input [5:0] bplcon2, input
//   [2:1] nplayfield, input [7:0] nsprite, output reg sprsel; purely
//   combinational, no clock port. Verilator flattens the non-zero-based
//   range [2:1] to a contiguous LSB-based scalar: bit 0 of the packed value
//   is Verilog index 1 (PF1), bit 1 is Verilog index 2 (PF2) -- same
//   convention already committed in hrm/ch4-sprite-priority/ and
//   hrm/denise_spritepriority/'s own harnesses for this same module.
//   Lines 20-23: sprgroup[0..3] are each `nsprite[2n+1:2n] != 2'd0`, i.e.
//   sprgroup[k] is the OR of the two nsprite bits at positions {2k,2k+1} --
//   this is the RTL fact that makes sprites {0,1},{2,3},{4,5},{6,7} the
//   groups, confirming this package's SPECIFICATION.md membership
//   assertions were the right shape sight unseen. Lines 26-36: sprcode is a
//   priority encoder over sprgroup[0..3] in ascending order (lowest active
//   group wins), 3'd7 when none is set. Lines 39-53: sprcode is compared
//   against bplcon2[2:0] (PF1P) and bplcon2[5:3] (PF2P) to decide whether a
//   playfield is in front of the winning sprite group, gated by
//   nplayfield[1]/[2]; sprcode==3'd7 forces sprsel=1'b0 unconditionally,
//   before nplayfield is even read.
//   This reproduces, port-for-port and line-for-line, what the claims
//   file's own classify.evidence field already described for this fixture
//   (evidence written by the reading pipeline before this package existed).
//   It is treated here as another reader's measurement being reproduced,
//   not as an expected value -- and it does reproduce, with no
//   disagreement to report.
//   No `ecs`, `aga`, or chipset-select term appears anywhere on this
//   module's ports or body: this fixture has no input that could make its
//   behaviour depend on OCS vs ECS, so "applies_to: OCS" (every one of the
//   18 claims' own classify.chipset field) is not a live distinction to
//   test at this boundary -- there is nothing here for it to gate on.
//
// FIXTURE BOUNDARY / DESCOPED (see SPECIFICATION.md "Descoped" for the
// citation-bearing version of both of these):
//   (a) Within-pair order. Sprites 0 and 1 (and each other pair) feed the
//       SAME sprgroup bit via a plain OR (l.20-23): the RTL has no way to
//       tell, from sprgroup or anything downstream, which of the two was
//       actually active. Confirmed below by MEASURED, not asserted --
//       belongs to ch7-inter-sprite-priority on denise_sprites.
//   (b) Collision. This module has no collision-related port at all
//       (sprsel is the only output); the collision half of "for playfield
//       priority and collision purposes" is a different signal path
//       entirely (denise_collision), not reachable from here.
//
// CLAIM COVERAGE (18 records total):
//   [control] FIXTURE-LIVE (no ids, no page)
//   SPGRP-01-SAME, SPGRP-23-SAME, SPGRP-45-SAME, SPGRP-67-SAME (p.208/209,
//     4 uids each -- 16 total): sol:7-video.c3/c4/c5/c6, opus:7-vp.c5/c6/c7/c8,
//     sol:7-fig7-2.c1/c2/c3/c4, opus:7-fig7-2.c2/c3/c4/c5
//   SPGRP-B12-DISTINCT, SPGRP-B34-DISTINCT, SPGRP-B56-DISTINCT,
//     SPGRP-B70-DISTINCT (p.209, 2 uids each, opus's figure exhaustiveness
//     clause, reused pairwise across adjacent boundaries): opus:7-fig7-2.c2,
//     c3, c4, c5
//   Overview, realised jointly by the 8 assertions above, not a 9th
//     collective assertion: sol:7-video.c2, opus:7-vp.c4 (playfield-priority
//     half only -- collision half declined, see record() below)
//   16 (membership, 4 uids x 4) + 8 (distinctness, 2 uids x 4, each opus fig
//   uid reused at both its boundaries) - 8 (overlap: each opus fig uid
//   counted once in each list above) ... counted by uid instead: sol has 9
//   uids (c2..c6, fig.c1..c4), opus has 9 uids (vp.c4..c8, fig.c2..c5) = 18.
//   All 18 accounted for: every uid appears in the table above or in the
//   overview line.
//
// CONTROL CASE / MEASUREMENT BASIS: every SAME assertion sweeps ALL 192
// (bplcon2, nplayfield) combinations (64 x 3, nplayfield=0x00 excluded --
// with no playfield valid, sprsel is always 1 for any active group
// regardless of bplcon2, so it carries no group-distinguishing information;
// same exclusion hrm/ch4-sprite-priority/ already made on this fixture) and
// requires EQUALITY at every single one. Every DISTINCT assertion searches
// the SAME 192-point space for at least one point where, simultaneously,
// the lower group's own two members agree with each other, the upper
// group's own two members agree with each other, AND the two groups
// disagree with each other -- i.e. the runs behind a SAME half and the runs
// behind the linked DISTINCT half are evaluated at the literal same
// (bplcon2, nplayfield) point, differing only in which single nsprite bit
// is armed. This is what rules out a module that simply gives every sprite
// its own unique outcome (which would satisfy a DISTINCT check alone but
// fail its linked SAME check, since sprite pairs that are supposedly
// group-mates would then disagree too).
//
// RESULTS and MUTATION LOG: see the end of this file, filled in after this
// RTL-open step (header/assertions above were committed first).

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vdenise_spritepriority.h"

// MUT_ID: 0 = no mutation (the committed baseline). Any other value
// perturbs exactly one assertion's own stimulus -- which nsprite bit(s) are
// armed, never the expected outcome -- so that assertion, and only it, can
// be driven red and reverted. Build with `-CFLAGS -DMUT_ID=N` to replay
// mutation N; see the numbered comments beside each `MUT_ID == N` site.
#ifndef MUT_ID
#define MUT_ID 0
#endif

namespace {

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

// input [2:1] nplayfield: Verilator packs bit 0 = Verilog index 1 (PF1),
// bit 1 = Verilog index 2 (PF2). Same convention already committed in
// hrm/ch4-sprite-priority/ and hrm/denise_spritepriority/ for this module.
constexpr uint8_t PF1_ON = 0x01;
constexpr uint8_t PF2_ON = 0x02;
const uint8_t NPLAYFIELDS[3] = {PF1_ON, PF2_ON, static_cast<uint8_t>(PF1_ON | PF2_ON)};

// One nsprite bit per sprite, 0-based, matching the port width directly.
constexpr uint8_t SPR[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// The four groups the manual names, as (A, B) member-bit pairs.
constexpr int GROUP[4][2] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}};

// Purely combinational: set inputs, eval, read sprsel. No clock port is
// named on this module (denise_spritepriority.v:5-11).
class Harness {
public:
    Harness() : dut(new Vdenise_spritepriority) {
        dut->bplcon2 = 0;
        dut->nplayfield = 0;
        dut->nsprite = 0;
        dut->eval();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    uint8_t read(uint8_t bplcon2, uint8_t nplayfield, uint8_t nsprite) {
        dut->bplcon2 = static_cast<uint8_t>(bplcon2 & 0x3F);
        dut->nplayfield = static_cast<uint8_t>(nplayfield & 0x03);
        dut->nsprite = nsprite;
        dut->eval();
        return static_cast<uint8_t>(dut->sprsel & 0x1);
    }

    Vdenise_spritepriority* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    Harness h;

    // ---------------------- [control] FIXTURE-LIVE ----------------------
    // The fixture proves it can deliver a known word before anything reads
    // one: nsprite=0x00 (no sprite active) must differ from at least one
    // active pattern, at some (bplcon2, nplayfield) point. Per the build
    // brief, this is an `expect`, not a `note` -- a silently dead data path
    // would otherwise report every assertion below as a finding about the
    // design, not about the harness.
    {
        uint8_t active_candidates[8] = {SPR[0], SPR[1], SPR[2], SPR[3],
                                         SPR[4], SPR[5], SPR[6], SPR[7]};
        if (MUT_ID == 1) {  // MUTATION 1: "active" stimulus neutralized to 0x00 -- no difference can ever be found.
            for (uint8_t& c : active_candidates) c = 0x00;
        }
        bool found_effect = false;
        for (int code = 0; code < 64 && !found_effect; ++code) {
            for (uint8_t pf : NPLAYFIELDS) {
                uint8_t base = h.read(static_cast<uint8_t>(code), pf, 0x00);
                for (uint8_t act : active_candidates) {
                    if (h.read(static_cast<uint8_t>(code), pf, act) != base) {
                        found_effect = true;
                        break;
                    }
                }
                if (found_effect) break;
            }
        }
        checks.expect(found_effect,
            "[control] FIXTURE-LIVE: at least one (bplcon2, nplayfield) "
            "point exists where changing nsprite from 0x00 (no sprite "
            "active) to some active sprite changes sprsel -- the "
            "dut->eval()-then-read-sprsel measurement basis every "
            "assertion below shares is actually live. Fixture check, no "
            "claim id, no page.");
    }

    // ------------------- SPGRP-xx-SAME (4 assertions) --------------------
    // "Sprites 0 and 1" / "Sprites 2 and 3" / "Sprites 4 and 5" / "Sprites
    // 6 and 7" (p.208), each also one labelled finger of Figure 7-2's
    // "Sprite Groups" hand (p.209): presenting either member of a group
    // alone must give the identical outcome, at EVERY one of the 192
    // (bplcon2, nplayfield) points swept -- not just one hand-picked point.
    bool same_result[4] = {false, false, false, false};
    for (int g = 0; g < 4; ++g) {
        uint8_t bit_a = SPR[GROUP[g][0]];
        uint8_t bit_b = SPR[GROUP[g][1]];
        // MUTATION 2-5 (g+2): arm a sprite from the NEXT group instead of
        // this group's true second member -- a stimulus change, not a
        // flipped expected value. This must go red: the next group is
        // shown distinct from this one by the paired DISTINCT assertion.
        if (MUT_ID == g + 2) {
            bit_b = SPR[GROUP[(g + 1) % 4][0]];
        }
        bool all_same = true;
        for (int code = 0; code < 64 && all_same; ++code) {
            for (uint8_t pf : NPLAYFIELDS) {
                if (h.read(static_cast<uint8_t>(code), pf, bit_a) !=
                    h.read(static_cast<uint8_t>(code), pf, bit_b)) {
                    all_same = false;
                    break;
                }
            }
        }
        same_result[g] = all_same;
    }
    checks.expect(same_result[0],
        "SPGRP-01-SAME: sprite 0 alone and sprite 1 alone give the "
        "identical sprsel at every one of the 192 (bplcon2, nplayfield) "
        "points swept -- 'Sprites 0 and 1' (p.208) / Figure 7-2's SP01 "
        "finger (p.209). sol:7-video.c3, opus:7-vp.c5, sol:7-fig7-2.c1, "
        "opus:7-fig7-2.c2 .");
    checks.expect(same_result[1],
        "SPGRP-23-SAME: sprite 2 alone and sprite 3 alone give the "
        "identical sprsel at every one of the 192 (bplcon2, nplayfield) "
        "points swept -- 'Sprites 2 and 3' (p.208) / Figure 7-2's SP23 "
        "finger (p.209). sol:7-video.c4, opus:7-vp.c6, sol:7-fig7-2.c2, "
        "opus:7-fig7-2.c3 .");
    checks.expect(same_result[2],
        "SPGRP-45-SAME: sprite 4 alone and sprite 5 alone give the "
        "identical sprsel at every one of the 192 (bplcon2, nplayfield) "
        "points swept -- 'Sprites 4 and 5' (p.208) / Figure 7-2's SP45 "
        "finger (p.209). sol:7-video.c5, opus:7-vp.c7, sol:7-fig7-2.c3, "
        "opus:7-fig7-2.c4 .");
    checks.expect(same_result[3],
        "SPGRP-67-SAME: sprite 6 alone and sprite 7 alone give the "
        "identical sprsel at every one of the 192 (bplcon2, nplayfield) "
        "points swept -- 'Sprites 6 and 7' (p.208) / Figure 7-2's SP67 "
        "finger (p.209). sol:7-video.c6, opus:7-vp.c8, sol:7-fig7-2.c4, "
        "opus:7-fig7-2.c5 .");

    // ---------------- SPGRP-Bxx-DISTINCT (4 assertions) ------------------
    // Figure 7-2's exhaustiveness clause (p.209): SP01, SP23, SP45, SP67
    // are the ONLY sprite-group labels, so sprite 1 is not folded into
    // sprite 2's group, sprite 3 not into sprite 4's, and so on. Tested as:
    // does there exist at least one (bplcon2, nplayfield) point where,
    // simultaneously, this group's own two members agree with each other,
    // the next group's own two members agree with each other, AND the two
    // groups disagree -- the same point, differing only in which single
    // sprite is armed, so a "every sprite is unique" module (no real
    // grouping) cannot satisfy this: it would fail the own-group agreement
    // half.
    bool distinct_result[4] = {false, false, false, false};
    for (int g = 0; g < 4; ++g) {
        int hi = (g + 1) % 4;
        uint8_t lo_a = SPR[GROUP[g][0]];
        uint8_t lo_b = SPR[GROUP[g][1]];
        uint8_t hi_a = SPR[GROUP[hi][0]];
        uint8_t hi_b = SPR[GROUP[hi][1]];
        // MUTATION 6-9 (g+6): the "next group" stimulus is the SAME group
        // again instead of the true neighbour -- a stimulus change (which
        // nsprite bits stand in for "the other side"), not a flipped
        // expected value. This must go red: no point can show a group
        // disagreeing with itself.
        if (MUT_ID == g + 6) {
            hi_a = lo_a;
            hi_b = lo_b;
        }
        bool found = false;
        for (int code = 0; code < 64 && !found; ++code) {
            for (uint8_t pf : NPLAYFIELDS) {
                uint8_t v_lo_a = h.read(static_cast<uint8_t>(code), pf, lo_a);
                uint8_t v_lo_b = h.read(static_cast<uint8_t>(code), pf, lo_b);
                uint8_t v_hi_a = h.read(static_cast<uint8_t>(code), pf, hi_a);
                uint8_t v_hi_b = h.read(static_cast<uint8_t>(code), pf, hi_b);
                if (v_lo_a == v_lo_b && v_hi_a == v_hi_b && v_lo_a != v_hi_a) {
                    found = true;
                    break;
                }
            }
        }
        distinct_result[g] = found;
    }
    checks.expect(distinct_result[0],
        "SPGRP-B12-DISTINCT: at least one (bplcon2, nplayfield) point "
        "exists where sprites 0 and 1 agree with each other, sprites 2 "
        "and 3 agree with each other, and the two groups disagree -- "
        "group {0,1} does not extend to include sprite 2, as Figure "
        "7-2's SP01/SP23 labels being the only ones say (p.209). "
        "opus:7-fig7-2.c2, opus:7-fig7-2.c3 .");
    checks.expect(distinct_result[1],
        "SPGRP-B34-DISTINCT: at least one (bplcon2, nplayfield) point "
        "exists where sprites 2 and 3 agree with each other, sprites 4 "
        "and 5 agree with each other, and the two groups disagree -- "
        "group {2,3} does not extend to include sprite 4, as Figure "
        "7-2's SP23/SP45 labels being the only ones say (p.209). "
        "opus:7-fig7-2.c3, opus:7-fig7-2.c4 .");
    checks.expect(distinct_result[2],
        "SPGRP-B56-DISTINCT: at least one (bplcon2, nplayfield) point "
        "exists where sprites 4 and 5 agree with each other, sprites 6 "
        "and 7 agree with each other, and the two groups disagree -- "
        "group {4,5} does not extend to include sprite 6, as Figure "
        "7-2's SP45/SP67 labels being the only ones say (p.209). "
        "opus:7-fig7-2.c4, opus:7-fig7-2.c5 .");
    checks.expect(distinct_result[3],
        "SPGRP-B70-DISTINCT: at least one (bplcon2, nplayfield) point "
        "exists where sprites 6 and 7 agree with each other, sprites 0 "
        "and 1 agree with each other, and the two groups disagree -- "
        "group {6,7} does not wrap into sprite 0's group, as Figure "
        "7-2's SP67/SP01 labels being the only ones say (p.209). "
        "opus:7-fig7-2.c5, opus:7-fig7-2.c2 .");

    // ------------------------- MEASURED / RECORDED ------------------------
    // Within-pair order: measured directly, not assumed from the claims
    // file's own evidence field -- reproduced here as this pass's own
    // reading of the RTL.
    {
        bool order_invisible = true;
        for (int code = 0; code < 64 && order_invisible; ++code) {
            for (uint8_t pf : NPLAYFIELDS) {
                uint8_t v01 = h.read(static_cast<uint8_t>(code), pf, 0x01);
                uint8_t v02 = h.read(static_cast<uint8_t>(code), pf, 0x02);
                uint8_t v03 = h.read(static_cast<uint8_t>(code), pf, 0x03);
                if (!(v01 == v02 && v02 == v03)) { order_invisible = false; break; }
            }
        }
        checks.note(order_invisible ?
            "nsprite=0x01 (sprite 0 alone), 0x02 (sprite 1 alone) and 0x03 "
            "(both) give identical sprsel at every one of the 192 "
            "(bplcon2, nplayfield) points swept -- within-pair order is "
            "not observable at this boundary. Reproduces the claims "
            "file's own classify.evidence measurement for this fixture; "
            "not asserted here (see FIXTURE BOUNDARY(a) above)." :
            "nsprite=0x01, 0x02 and 0x03 do NOT give identical sprsel "
            "everywhere swept -- disagrees with the claims file's own "
            "classify.evidence measurement for this fixture; this is a "
            "finding about that measurement, reported to the lead.");
    }
    checks.record(
        "The collision half of 'for playfield priority and collision "
        "purposes only, sprites are treated as four groups of two' "
        "(sol:7-video.c2, opus:7-vp.c4) is out of scope for this module: "
        "denise_spritepriority has no collision-related port at all (its "
        "only output is sprsel). Belongs to a denise_collision-boundary "
        "package, not tested here.");
    checks.record(
        "Within-pair order (which of the two group members wins, e.g. "
        "sprite 0 vs sprite 1) is not observable at this boundary -- "
        "measured immediately above. Belongs to ch7-inter-sprite-priority "
        "on denise_sprites, not asserted here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// RESULTS: built and run under WSL/Verilator 5.052 against the pinned
// nanomig submodule, after this file's own header/assertions/expected
// values were written following the committed SPECIFICATION.md (c9ac428).
// 9 passed, 0 failed. All 8 claim-bearing assertions plus the fixture-live
// control hold at the denise_spritepriority boundary, as tested here. No
// red findings. The MEASURED line reproduces, without disagreement, the
// claims file's own classify.evidence for this fixture (within-pair order
// invisible at this boundary).
//
// MUTATION LOG: one stimulus mutation per assertion, each built with
// `-CFLAGS -DMUT_ID=N` into its own --Mdir (N = the numbered `MUT_ID == N`
// site inside that assertion, in source order), run, confirmed to turn
// ONLY that assertion red (8 passed, 1 failed every time -- checked
// against the other eight assertions' own PASS lines in each run's full
// output, not spot-checked), then reverted (plain `make`, no MUT_ID).
// Baseline re-verified clean (9 passed, 0 failed) after all nine. N
// assignments, in source order:
//   1 = [control] FIXTURE-LIVE
//   2 = SPGRP-01-SAME   3 = SPGRP-23-SAME   4 = SPGRP-45-SAME   5 = SPGRP-67-SAME
//   6 = SPGRP-B12-DISTINCT   7 = SPGRP-B34-DISTINCT
//   8 = SPGRP-B56-DISTINCT   9 = SPGRP-B70-DISTINCT
//
// MUTATION FIXTURE-LIVE stimulus red ([control], MUT_ID=1: active-sprite
//   stimulus neutralized to 0x00) -- confirmed red, reverted.
// MUTATION SPGRP-01-SAME stimulus red (MUT_ID=2: second run armed sprite 2
//   instead of sprite 1) -- confirmed red, reverted.
// MUTATION SPGRP-23-SAME stimulus red (MUT_ID=3: second run armed sprite 4
//   instead of sprite 3) -- confirmed red, reverted.
// MUTATION SPGRP-45-SAME stimulus red (MUT_ID=4: second run armed sprite 6
//   instead of sprite 5) -- confirmed red, reverted.
// MUTATION SPGRP-67-SAME stimulus red (MUT_ID=5: second run armed sprite 0
//   instead of sprite 7) -- confirmed red, reverted.
// MUTATION SPGRP-B12-DISTINCT stimulus red (MUT_ID=6: "next group" side
//   armed with group {0,1}'s own members again, not group {2,3}'s) --
//   confirmed red, reverted.
// MUTATION SPGRP-B34-DISTINCT stimulus red (MUT_ID=7: "next group" side
//   armed with group {2,3}'s own members again, not group {4,5}'s) --
//   confirmed red, reverted.
// MUTATION SPGRP-B56-DISTINCT stimulus red (MUT_ID=8: "next group" side
//   armed with group {4,5}'s own members again, not group {6,7}'s) --
//   confirmed red, reverted.
// MUTATION SPGRP-B70-DISTINCT stimulus red (MUT_ID=9: "next group" side
//   armed with group {6,7}'s own members again, not group {0,1}'s) --
//   confirmed red, reverted.
