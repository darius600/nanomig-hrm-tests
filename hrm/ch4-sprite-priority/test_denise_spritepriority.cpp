// ch4-sprite-priority -- denise_spritepriority conformance test
//
// Test purpose: chapter 4's general statements about video priority among
//   sprites and between sprites and playfields -- that each sprite has a
//   fixed priority with respect to all the other sprites, that an object
//   (sprite or playfield) with low priority is composited behind an object
//   with higher priority, that a transparent sprite lets a lower-priority
//   sprite or playfield show through, and that sprite data is fed to a
//   priority circuit that decides sprite-versus-playfield compositing -- all
//   at the denise_spritepriority module boundary. Package
//   ch4-sprite-priority, fixture denise_spritepriority (one Verilator build,
//   this module only, per this package's own claims.jsonl --
//   ch4_packages/ch4-sprite-playfield-priority.jsonl, 8 records).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 103, 124, 125. A red result below means denise_spritepriority.v
//   disagrees with the manual, not that the assertion is wrong -- no
//   assertion here was, or should be, adjusted to make the RTL pass.
// Non-goals: anything outside the `denise_spritepriority` fixture this
//   package builds, and any claim it does not name. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: ids such as `CH4-p103.c20` and `CH4-prio.c1` name one sentence of
//   the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch4.json
//   lists under `inventories`.
//   `FPGA-nnn` is a task number in the private workspace where this work was
//   planned; it resolves nowhere public, and docs/legacy-names.md gives the
//   finding keys that replaced the ones that were findings.
//
// ORDER OF WORK: this file (assertions, expected values, claim-id
// coverage) was written and committed from claims.jsonl's own quotations
// alone -- denise_spritepriority.v was not opened by this pass until
// after that commit, per the build brief's explicit gate. The port names
// (bplcon2[5:0], nplayfield[2:1], nsprite[7:0], sprsel), the
// nplayfield-bit-packing convention (PF1_ON=0x01, PF2_ON=0x02 for
// Verilator's flattening of the non-zero-based range [2:1]), and the
// fact that this module is purely combinational (no clock port) are
// reused verbatim from hrm/denise_spritepriority/test_denise_spritepriority.cpp
// and hrm/ch3-sprite-playfield-priority/test_denise_spritepriority.cpp --
// two already-committed, already-built sibling harnesses for this SAME
// module, which the build brief for this package explicitly named and
// directed this pass to read before writing. Two further facts are
// likewise taken from those sibling packages and from this package's own
// claims.jsonl classify.evidence field (an artifact of an earlier
// classification pass, not of this pass opening the RTL): "nsprite=0
// always gives sprsel=0", and "denise_spritepriority.v:20-40 groups
// nsprite in pairs, encodes sprcode and compares it against
// bplcon2[2:0] (PF1) and bplcon2[5:3] (PF2)" -- i.e. the eight nsprite
// bits reduce, before any bplcon2 comparison, to one of four pair-groups
// (sprites 0/1, 2/3, 4/5, 6/7), each already exercised individually (one
// representative bit per pair) by the legacy package's own committed
// test cases. None of this is a numeric threshold, priority-code value,
// or comparison outcome -- no assertion below depends on knowing any of
// those; see FIXTURE BOUNDARY below for what that rules out.
//
// FIXTURE BOUNDARY:
//   (a) Group granularity only. The four pair-groups (0/1, 2/3, 4/5, 6/7)
//       are each represented below by a single bit (SPR0, SPR2, SPR4,
//       SPR6 -- the same representative bits the legacy denise_spritepriority
//       package already uses). Nothing about which sprite wins AGAINST
//       ITS OWN PAIR PARTNER (e.g. SPR0 vs SPR1) is asserted anywhere in
//       this file: no claim in this package's 8 records states an
//       intra-pair outcome, and no evidence disclosed to this pass before
//       the RTL-open step settles it either way.
//   (b) Sprite-data transparency (SPRxDATA/SPRxDATB == %00) is decoded
//       upstream of this module. This fixture's `nsprite` input is
//       already a resolved per-sprite active/opaque bitmask (0x01=SPR0
//       active, etc., per the sibling packages' own usage) -- there is no
//       raw 2-bit pixel-colour port here for the actual "data == 00"
//       comparison CH4-color.c13 names. That comparison belongs to a
//       wider boundary (the sprite generator, denise_sprites, a different
//       package's fixture) and is not tested here. What IS observable at
//       this boundary is the downstream consequence: when this module is
//       told no sprite is presenting active data (nsprite=0 -- the
//       fixture-level stand-in for "transparent/absent"), does the
//       priority decision correctly fall through to the alternative
//       (lower-priority) content. See ABSENT-SPRITE-CEDES-TO-LOWER-PRIORITY
//       below.
//   (c) No specific priority-code value or bit identity is asserted. This
//       package's own claims (CH4-p103.c20, CH4-prio.c1, CH4-prio.c3,
//       CH4-p103.c22) state the general priority PRINCIPLE in prose, never
//       a specific BPLCON2 bit-to-field mapping or a numeric code-to-
//       outcome table -- that material is Appendix A's and chapter 3's,
//       already covered by hrm/ch3-sprite-playfield-priority/ on this
//       same fixture (BIT0-IS-PF1P0 .. BIT5-IS-PF2P2,
//       PF1P/PF2P-DIGITS-BOUND-TO-BIT-WEIGHT,
//       PF1P/PF2P-GOVERNS-...-INDEPENDENT-OF-...). This package tests the
//       general principle by sweeping every bplcon2 value existentially/
//       universally as needed, never by pinning a single hand-picked code.
//
// OVERLAP WITH THE TWO EXISTING PACKAGES ON THIS FIXTURE (read in full
// before this file was written, per the build brief):
//   hrm/denise_spritepriority/ is a pre-claims legacy suite (FPGA-033,
//     Chapter 7 oracle, no claim ids, no MUTATION discipline). Its last
//     two cases (SPR0+SPR6 both active vs SPR6 alone, same bplcon2) give
//     INCIDENTAL, non-isolated evidence that a lower-numbered group wins
//     when several are active together -- but nothing there is claim-
//     tagged, isolated, generalized across all six group pairs, or
//     mutation-verified, so it earns no coverage credit for this
//     package's CH4-prio.c3 and is not treated as "already measured" for
//     purposes of the build brief's do-not-duplicate instruction; a
//     clean, isolated, exhaustive version is built fresh below instead.
//   hrm/ch3-sprite-playfield-priority/ is this repo's chapter-3/Appendix-A
//     view of the SAME fixture: which BPLCON2 bit is which named field
//     (PF1P0-2, PF2P0-2), and that each field's effect is confined to its
//     own playfield. Its BPLCON2-BITS-0-5-ARE-LIVE-PRIORITY-FIELD
//     assertion (page 90 there) already sweeps bplcon2[5:0] through all
//     64 values and shows sprsel takes both possible values -- i.e. it
//     ALREADY MEASURES, on this exact fixture, the proposition
//     CH4-p103.c22 states in chapter 4's own words: "You can vary the
//     priority between sprites and playfields." Building a second,
//     page-103-cited copy of that same sweep here would be exactly the
//     "second copy of chapter 3's package" the build brief warns against,
//     not a new measurement. CH4-p103.c22 is therefore DECLINED below,
//     not asserted, and its id appears only in this comment, not inside
//     any expect() label -- correctly reading as uncovered by THIS
//     package, because chapter 4's own quoted text (unlike chapter 3's
//     Appendix-A quotations) never names BPLCON2 as the mechanism: to
//     test it at all requires importing which register to vary from
//     outside this package's own 8 claims, which is exactly the kind of
//     chapter-7-deferred, not-chapter-4's-own-quotation dependency the
//     build brief's task description warns about (page 103's own next
//     sentence, excluded from extraction, is "(See Chapter 7, 'System
//     Control Hardware,' for more information about sprite priority.)").
//     What chapter 4's own quotations DO support without borrowing from
//     elsewhere -- the fixed ordering among sprites themselves
//     (CH4-prio.c3's "priority with respect to all the other sprites")
//     and sprite data's structural path to the priority circuit
//     (CH4-hw.c14 / CH4-p124.c10 / CH4-fig4-13.c6) -- is asserted below.
//
// CLAIM COVERAGE (8 records total):
//   [control] FIXTURE-LIVE-DATA-PATH (no ids, no page)
//   SPRITE-PRIORITY-FIXED-BY-GROUP-NUMBER (3 ids, p.103) -- CH4-p103.c20,
//     CH4-prio.c1, CH4-prio.c3
//   ABSENT-SPRITE-CEDES-TO-LOWER-PRIORITY (1 id, p.103) -- CH4-color.c13
//   SPRITE-DATA-REACHES-PRIORITY-CIRCUIT (3 ids, pp.124-125) --
//     CH4-hw.c14, CH4-p124.c10, CH4-fig4-13.c6
//   DECLINED, not duplicated (1 id) -- CH4-p103.c22: already measured by
//     hrm/ch3-sprite-playfield-priority/'s BPLCON2-BITS-0-5-ARE-LIVE-
//     PRIORITY-FIELD on this same fixture, and its mechanism (BPLCON2) is
//     not named by chapter 4's own quoted text; see OVERLAP above.
//   3 + 1 + 3 + 1(declined) = 8. All 8 claim ids of
//   ch4-sprite-playfield-priority.jsonl are accounted for above.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id) sweeps
// nplayfield and nsprite (bplcon2 held at 0) and requires sprsel to take
// both its possible values (0 and 1), not one stuck value -- proving the
// eval()-then-read-sprsel measurement basis every claim-based assertion
// below shares is actually live before any of them depends on it.
//
// RESULTS and MUTATION LOG: see the end of this file, filled in after the
// RTL-open step (this file's own header commentary and assertions above
// were written and committed first, per ORDER OF WORK above).

#include <cstdint>
#include <iostream>
#include <set>
#include <string>

#include <verilated.h>
#include "Vdenise_spritepriority.h"

// MUT_ID: 0 = no mutation (the committed baseline). Any other value
// perturbs exactly one assertion's own stimulus (see the numbered
// comments beside each `if (MUT_ID == N)` / `(MUT_ID == N) ?` site below)
// so that assertion, and only it, can be driven red and reverted -- the
// mutation gate this package's build brief requires, one id per
// assertion. Build with `-CFLAGS -DMUT_ID=N` (or edit the default below)
// to replay mutation N.
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
    int passed = 0;
    int failed = 0;
};

// input [2:1] nplayfield: Verilator packs a non-zero-based range to a
// contiguous LSB-based scalar -- bit 0 of the packed value is Verilog
// index 1 (PF1), bit 1 is Verilog index 2 (PF2). Reused from
// hrm/denise_spritepriority/test_denise_spritepriority.cpp's already-built
// harness for this same module; tool plumbing, not a claim.
constexpr uint8_t PF1_ON = 0x01;
constexpr uint8_t PF2_ON = 0x02;

// One representative nsprite bit per pair-group (see FIXTURE BOUNDARY(a)
// above): group 0 = SPR0/1, group 1 = SPR2/3, group 2 = SPR4/5,
// group 3 = SPR6/7. Same representative bits hrm/denise_spritepriority/'s
// own committed test cases already use for these same four groups.
constexpr uint8_t GROUP_BIT[4] = {0x01, 0x04, 0x10, 0x40};

// Purely combinational: set inputs, eval, read sprsel. No clock port is
// named in any of this package's 8 claims' evidence, nor in either
// sibling package's own already-disclosed boundary note for this module.
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

    // ---------------- [control] FIXTURE-LIVE-DATA-PATH ----------------
    {
        std::set<uint8_t> seen;
        uint8_t nplayfields[] = {0x00, PF1_ON, PF2_ON, static_cast<uint8_t>(PF1_ON | PF2_ON)};
        uint8_t nsprites[] = {0x00, 0x01, 0x80, 0xFF};
        size_t n_pf = 4, n_spr = 4;
        if (MUT_ID == 1) { n_pf = 1; n_spr = 1; }  // MUTATION 1: shrink both swept sets to a single point each.
        for (size_t p = 0; p < n_pf; ++p)
            for (size_t s = 0; s < n_spr; ++s)
                seen.insert(h.read(/*bplcon2=*/0x00, nplayfields[p], nsprites[s]));

        checks.expect(seen.size() == 2,
            "[control] FIXTURE-LIVE-DATA-PATH: sweeping nsprite and "
            "nplayfield alone (bplcon2 held at 0) makes sprsel take both "
            "its possible values (0 and 1), not a single stuck value -- "
            "proving the dut->eval()-then-read-sprsel measurement basis "
            "every assertion below shares is actually live before any "
            "claim-based assertion depends on it. Fixture check, no claim "
            "id, no page.");
    }

    // ------------- SPRITE-PRIORITY-FIXED-BY-GROUP-NUMBER -------------
    // "Each sprite has a fixed priority with respect to all the other
    // sprites." For every one of the six ordered pairs of the four
    // pair-groups (i<j), two things are checked together: (precondition)
    // groups i and j are actually distinguishable -- there exists at
    // least one (bplcon2, nplayfield) context where group i alone and
    // group j alone produce different sprsel values, so the "lower wins"
    // check below is not vacuous; and (the claim itself) whenever BOTH
    // groups are signalled active together, the outcome always matches
    // group i (the lower-numbered one) alone, across all 64 bplcon2
    // values and all three playfield-enable combinations -- never group
    // j's outcome instead.
    {
        bool all_pairs_ok = true;
        for (int i = 0; i < 4 && all_pairs_ok; ++i) {
            for (int j = i + 1; j < 4 && all_pairs_ok; ++j) {
                bool found_distinguishing = false;
                bool lower_always_wins = true;
                const uint8_t nplayfields[] = {PF1_ON, PF2_ON, static_cast<uint8_t>(PF1_ON | PF2_ON)};
                for (int code = 0; code < 64; ++code) {
                    for (uint8_t pf : nplayfields) {
                        uint8_t bplcon2 = static_cast<uint8_t>(code);
                        uint8_t v_i = h.read(bplcon2, pf, GROUP_BIT[i]);
                        uint8_t v_j = h.read(bplcon2, pf, GROUP_BIT[j]);
                        if (v_i != v_j) found_distinguishing = true;

                        // MUTATION 2: the baseline this check compares
                        // "both active" against is fed from the WRONG,
                        // higher-numbered group's own stimulus instead of
                        // the lower-numbered group's -- a real stimulus
                        // change (which nsprite pattern produces the
                        // comparison target), not a hand-flipped
                        // expected value.
                        int base_idx = (MUT_ID == 2) ? j : i;
                        uint8_t v_base = h.read(bplcon2, pf, GROUP_BIT[base_idx]);
                        uint8_t v_both = h.read(bplcon2, pf, static_cast<uint8_t>(GROUP_BIT[i] | GROUP_BIT[j]));
                        if (v_both != v_base) lower_always_wins = false;
                    }
                }
                if (!(found_distinguishing && lower_always_wins)) all_pairs_ok = false;
            }
        }
        checks.expect(all_pairs_ok,
            "SPRITE-PRIORITY-FIXED-BY-GROUP-NUMBER: for every one of the "
            "six ordered pairs among the four sprite pair-groups (0/1, "
            "2/3, 4/5, 6/7), the groups are shown distinguishable (some "
            "bplcon2/nplayfield context makes the lower group's solo "
            "outcome differ from the higher group's solo outcome -- the "
            "precondition a green here depends on), and whenever both "
            "groups are signalled active together, sprsel always matches "
            "the LOWER-numbered group's own solo outcome, across all 64 "
            "bplcon2 values and all three playfield-enable combinations, "
            "as printed: 'priority with respect to all the other "
            "sprites.' / 'field that has lower video priority. An object "
            "with low priority appears' / '\"behind\" an object with "
            "higher priority.' (p.103). CH4-p103.c20, CH4-prio.c1, "
            "CH4-prio.c3 .");
    }

    // ------------- ABSENT-SPRITE-CEDES-TO-LOWER-PRIORITY -------------
    // CH4-color.c13's fall-through consequence, at this fixture's own
    // boundary (see FIXTURE BOUNDARY(b) above): when this module is told
    // no sprite pair-group is presenting active data (nsprite=0, the
    // fixture-level stand-in for "transparent/absent"), the priority
    // decision must fall through to the only content left -- the
    // playfield -- for every bplcon2 value and every playfield-enable
    // combination, never getting stuck favouring the absent sprite.
    {
        bool all_zero = true;
        uint8_t nsprite_stim = 0x00;
        if (MUT_ID == 3) nsprite_stim = 0xFF;  // MUTATION 3: a sprite IS present now.
        const uint8_t nplayfields[] = {PF1_ON, PF2_ON, static_cast<uint8_t>(PF1_ON | PF2_ON)};
        for (int code = 0; code < 64; ++code) {
            for (uint8_t pf : nplayfields) {
                if (h.read(static_cast<uint8_t>(code), pf, nsprite_stim) != 0) all_zero = false;
            }
        }
        checks.expect(all_zero,
            "ABSENT-SPRITE-CEDES-TO-LOWER-PRIORITY: with nsprite=0x00 "
            "(no sprite pair-group signalled active) and a playfield "
            "enabled, sprsel reads 0 (playfield shown) for every one of "
            "the 64 bplcon2 values -- an absent/transparent sprite never "
            "blocks the lower-priority content behind it, matching "
            "'[data] is 00 becomes transparent and shows the color of "
            "any other sprite or play-[field]' (p.103; the raw "
            "SPRxDATA/DATB==00 comparison itself is decoded upstream of "
            "this fixture -- see FIXTURE BOUNDARY(b) above -- only the "
            "fall-through consequence is asserted here). CH4-color.c13 .");
    }

    // ------------- SPRITE-DATA-REACHES-PRIORITY-CIRCUIT -------------
    // Sprite data (this fixture's nsprite input) is a live input to the
    // priority circuit (this module): there exists at least one
    // (bplcon2, nplayfield) context in which changing nsprite from
    // "nothing active" to some active pair-group changes sprsel.
    {
        bool found_effect = false;
        const uint8_t nplayfields[] = {PF1_ON, PF2_ON, static_cast<uint8_t>(PF1_ON | PF2_ON)};
        uint8_t active_candidates[] = {GROUP_BIT[0], GROUP_BIT[1], GROUP_BIT[2], GROUP_BIT[3], 0xFF};
        if (MUT_ID == 4) {  // MUTATION 4: "active" stimulus neutralized to 0x00 -- no difference can ever be found.
            for (uint8_t& c : active_candidates) c = 0x00;
        }
        for (int code = 0; code < 64 && !found_effect; ++code) {
            for (uint8_t pf : nplayfields) {
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
            "SPRITE-DATA-REACHES-PRIORITY-CIRCUIT: at least one bplcon2/"
            "nplayfield context exists where changing nsprite from "
            "0x00 (no sprite active) to an active pair-group changes "
            "sprsel -- sprite data is a live input to this priority "
            "decision, not a dead-ended or ignored one, matching "
            "'Sprite data goes to the  priority circuit to establish the "
            "priority' / 'between sprites and playfields.' (p.124) and "
            "Figure 4-13's own label, verified to read 'output to video "
            "priority logic', for the sprite serial video-data block's "
            "destination (p.125). CH4-hw.c14, CH4-p124.c10, "
            "CH4-fig4-13.c6 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// RESULTS: built and run under WSL/Verilator 5.052 against the pinned
// nanomig submodule, after this file's own header/assertions/expected
// values were committed (see ORDER OF WORK above). 4 passed, 0 failed --
// all 7 asserted claims hold at the denise_spritepriority boundary, as
// tested here. No red findings.
//
// denise_spritepriority.v, read for the first time after the spec-only
// commit: sprgroup[0..3] are each `nsprite[2n+1:2n] != 2'd0` (an OR of
// the pair's two bits, ll.20-23) -- confirms FIXTURE BOUNDARY(a): the
// module genuinely never distinguishes which sprite within a pair is
// active, only that the pair is. sprcode is a priority encoder over
// sprgroup[0..3] in ascending order, defaulting to 3'd7 when none are set
// (ll.26-36) -- computed from nsprite alone, before bplcon2 is consulted
// at all, confirming SPRITE-PRIORITY-FIXED-BY-GROUP-NUMBER's design
// needed no numeric threshold knowledge to be correct: with groups i<j
// both active, sprgroup[i] is tested first in the if/else-if chain
// regardless of sprgroup[j], so sprcode is always i's code, matching "i
// alone"'s sprcode exactly, for every bplcon2/nplayfield combination --
// exactly what the assertion measured. sprcode==3'd7 forces sprsel=1'b0
// unconditionally (l.45-46), before nplayfield is even read -- confirms
// ABSENT-SPRITE-CEDES-TO-LOWER-PRIORITY holds for every bplcon2 AND would
// have held for nplayfield=0x00 too (not swept here, since this
// assertion's own claim -- CH4-color.c13 -- is about a playfield actually
// being shown, so the sweep was restricted to playfield-enabled contexts
// on purpose, not by necessity). All three pre-RTL boundary assumptions
// (group-pair granularity, group-encoding independent of bplcon2/
// nplayfield, nsprite=0 forcing sprsel=0 unconditionally) held exactly;
// nothing in this file was corrected after opening the RTL.
//
// MUTATION LOG: one stimulus mutation per assertion, each built with
// `-CFLAGS -DMUT_ID=N` (N = the numbered `MUT_ID == N` site inside that
// assertion, in source order: 1=control, 2=SPRITE-PRIORITY-FIXED-BY-
// GROUP-NUMBER, 3=ABSENT-SPRITE-CEDES-TO-LOWER-PRIORITY, 4=SPRITE-DATA-
// REACHES-PRIORITY-CIRCUIT), run, confirmed to turn ONLY that assertion
// red (the other three stayed green -- checked directly in each run's
// full output, not spot-checked), then reverted (plain `make`, no
// MUT_ID). Baseline re-verified clean (4 passed, 0 failed) after all
// four.
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red
// MUTATION SPRITE-PRIORITY-FIXED-BY-GROUP-NUMBER stimulus red
// MUTATION ABSENT-SPRITE-CEDES-TO-LOWER-PRIORITY stimulus red
// MUTATION SPRITE-DATA-REACHES-PRIORITY-CIRCUIT stimulus red
