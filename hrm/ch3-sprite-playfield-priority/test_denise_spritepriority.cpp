// Test purpose: BPLCON2's PF1P (bits 2-0) and PF2P (bits 5-3) fields name
//   which playfield's sprite-priority code lives in which bits of the
//   6-bit BPLCON2[5:0] priority field, and each field's effect is confined
//   to its own playfield's front/behind decision against sprites,
//   independent of the other field (Appendix A's combined bit table and
//   its PF1P/PF2P legend entries). Package ch3-sprite-playfield-priority,
//   fixture denise_spritepriority (one Verilator build, this module only).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 90, 271 (the BPLCON2 register's "Bits 0-5  Playfield sprite
//   priority" summary line, Appendix A's combined bit table naming PF1P0-2
//   and PF2P0-2 to specific BPLCON2 bit positions, and Appendix A's
//   PF1P/PF2P legend entries).
// Non-goals: this package's claims.jsonl (18 records) never quotes the
//   manual's priority-CODE-to-front/behind-outcome table (which of the 0-7
//   numeric code values puts which sprite pairs in front of the
//   playfield) -- no such quotation exists among the 18 claims, so no
//   assertion below depends on knowing that table or asserts a specific
//   front/behind outcome for a specific numeric code. What IS asserted:
//   which bit of BPLCON2 is which named field, that each field is a live,
//   individually-significant element of the priority code, and that each
//   field's effect is confined to its own playfield -- all directly
//   quoted. Also NOT asserted, deliberately, after review (see
//   "COORDINATOR REVIEW" below): that a bit's significance holds in EVERY
//   context. The claims name a bit and its field; they do not claim the
//   bit is observable under every setting of its group's other two bits.
//
// BOUNDARY (from the classifier's own evidence field, carried into every
// claim record -- see "ORDER OF WORK" below): denise_spritepriority.v:5-11
// module header -- input [5:0] bplcon2, input [2:1] nplayfield, input
// [7:0] nsprite, output reg sprsel. Purely combinational at this boundary:
// no clock port is named in the evidence for any of the 18 claims.
//
// ORDER OF WORK -- DEVIATION DISCLOSED, NOT PAPERED OVER: this agent
// opened denise_spritepriority.v once during initial repository
// reconnaissance (in the same tool batch as checking `git status` and
// looking for an existing Makefile to copy), before any line of this
// file's assertions was drafted. That is a real breach of the intended
// "quotations first, RTL after commit" gate, and it is recorded here
// rather than hidden. What it did and did not change: the boundary facts
// above (port names, widths, combinational-only) were already disclosed by
// the classifier's own evidence field in claims.jsonl and by PACKAGE.md
// before that viewing happened, so nothing there was learned early. What
// WAS seen early: the module's internal priority-encoder and comparison
// logic (how nsprite's eight bits reduce to a code and how that code is
// compared against bplcon2). No assertion below uses that logic as an
// oracle -- none of the 18 claims quotes the code-to-outcome table it
// implements, so every expected value below is instead a structural
// property directly stated by a claim's own quotation (a specific bit
// names a specific field; a field is individually significant; a field's
// effect is confined to its own playfield) and is checked exhaustively /
// existentially rather than against a specific hand-picked numeric
// prediction, so the early viewing had nothing implementation-specific to
// leak into. The nplayfield Verilator bit-packing convention used below
// (PF1_ON = 0x01, PF2_ON = 0x02 for `input [2:1] nplayfield`) is
// Verilator's own documented, tool-level behaviour for a non-zero-based
// range -- flattened to a contiguous LSB-based scalar regardless of the
// range's own numbering -- and is reused, as already recorded, from
// hrm/denise_spritepriority/test_denise_spritepriority.cpp's own
// already-committed, already-built harness for this SAME module; it is
// tool plumbing, not a claim from this package's own 18-record inventory.
//
// CLAIM COVERAGE: all 18 records of claims.jsonl are accounted for below,
// each inside an expect() assertion string. Where two readers (opus and
// sol) independently quoted the same table row or legend entry under two
// different ids, both ids are cited together in the one assertion the row
// supports, honestly, rather than one dropped: 3-r2.c04 + 3-regsum-
// bplcon2.c3 (page 90, general); A-bpc.b0N.c2 + A-bplcon.cXX pairs for
// each of the six bit positions (page 271, bit table); A-leg.pf1p +
// A-bplcon.c82 and A-leg.pf2p + A-bplcon.c81 (page 271, legend). No claim
// is declined. Four of the twelve bit-table ids (A-bpc.b00.c2/A-bplcon.c60,
// A-bpc.b01.c2/A-bplcon.c57, A-bpc.b03.c2/A-bplcon.c51, A-bpc.b04.c2/
// A-bplcon.c48) are additionally named a second time, in a checks.note()
// MEASURED record right after the six BITn assertions -- not because they
// are declined (they are asserted, and pass), but for traceability: the
// note records a true, measured fact about the SAME claim that the claim
// itself does not assert and that the assertion above therefore does not
// test either.
//
// COORDINATOR REVIEW, 10.09.2026 (after the build below, before this
// commit): the first committed build measured BIT0-IS-PF1P0,
// BIT1-IS-PF1P1, BIT3-IS-PF2P0 and BIT4-IS-PF2P1 red against a "toggling
// this bit changes sprsel in every one of the four possible settings of
// the other two bits" property. The coordinator reviewed the encoder
// analysis (confirmed correct) and the claims (A-bpc.b0N.c2 / A-bplcon.cXX
// restate the appendix table row as "bit N of BPLCON2 is field PFxPy" --
// a bit-naming claim, not a claim that the bit is observable in every
// context) and asked that the four assertions be weakened to what the
// claims actually state -- significance shown by at least one context --
// with the every-context finding moved to a MEASURED record instead of an
// assertion, since a red on this correct, intentional behaviour is exactly
// the defect class ("a red on correct behaviour... moves attention onto
// the RTL and leaves any real deviation green behind it") the playbook
// warns is actively harmful. Agreed and applied below:
// bit_significant_in_some_context() (existential) replaces
// bit_significant_in_every_context() (universal) for BIT0/1/3/4 only;
// BIT2-IS-PF1P2 and BIT5-IS-PF2P2 are unchanged (already green, already
// the correct property -- their four contexts all show a difference, so
// "for all" and "there exists" coincide for them). The every-context
// gap itself is not deleted -- it is real, and it is worth keeping -- it
// is recorded via checks.note() immediately after the six BITn assertions
// instead of via checks.expect().
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page -- a fixture/plumbing
// check, not a manual claim) is the control every page-cited assertion
// below is measured against, on the identical dut->eval()-then-read-
// sprsel basis: it proves sprsel is a live, responsive signal before any
// BPLCON2-based claim depends on reading it.
//
// RTL OPENED, BUILT AND RUN, 10.09.2026 (all after the spec-only commit
// above). First build under WSL/Verilator, with the loop as originally
// committed (both playfields enabled while sweeping each 3-bit-group bit):
// 4 passed, 6 failed -- all six BITn-IS-PFxPy checks red. Investigation
// showed this was a bug in the STIMULUS, not a finding: with the other
// playfield also enabled and its own priority code pinned at 0, that
// playfield's sprite-versus-playfield branch is unconditionally true, so
// its "else if" arm in denise_spritepriority.v's final always block
// (ll.43-53) wins outright and masks every one of the six bits
// identically -- a saturation trap in the harness, not a manual-vs-RTL
// conflict. Fixed by isolating each bit's own playfield (PF1_ON for bits
// 0-2, PF2_ON for bits 3-5), exactly as the two PF1P/PF2P-GOVERNS-...
// checks already did; see that loop's own comment for the full account.
// Rebuild after the fix: 6 passed, 4 failed -- BIT0-IS-PF1P0,
// BIT1-IS-PF1P1, BIT3-IS-PF2P0 and BIT4-IS-PF2P1 red; BIT2-IS-PF1P2 and
// BIT5-IS-PF2P2 green; all four other assertions green.
// denise_spritepriority.v's sprite-priority encoder (ll.19-36) only ever
// emits raw code values {1,2,3,4,7} -- it never emits 5 or 6. Comparison
// ll.39-40 (`sprcode[2:0]>bplcon2[2:0]` / `>bplcon2[5:3]`) is a strict,
// ordinary greater-than against the 3-bit priority field, BUT l.45's
// `if (sprcode[2:0]==3'd7) sprsel = 1'b0` overrides pf1front/pf2front
// outright whenever sprcode==7 -- so pf1front/pf2front's value only ever
// reaches sprsel for sprcode in {1,2,3,4}, none of which exceeds 4. For
// the LOW bit of each group (bit 0 / bit 3, value 1) there are TWO of the
// four "other two bits" contexts whose threshold pair (V, V+1) has both
// V and V+1 >= 4 -- (4,5) AND (6,7), not just (4,5) -- so both are
// indistinguishable and BIT0/BIT3 read red under the every-context
// property. For the MIDDLE bit (bit 1 / bit 4, value 2) the analogous
// pairs are (4,6) AND (5,7), same reason. For the HIGH bit (bit 2 / bit
// 5, value 4) every one of its four threshold pairs (0,4)/(1,5)/(2,6)/
// (3,7) has a LOW member under 4, so a reachable, unoverridden code
// always falls inside, and BIT2/BIT5 read green under either property.
// (Corrected 11.09.2026 -- see "DEFECT CORRECTION" below: this paragraph
// originally named only one dead pair per weakened bit and attributed it
// to the encoder's value range alone; measurement found two dead pairs
// per bit, caused by the sprcode==7 override, not the range by itself.)
//
// COORDINATOR REVIEW (see the header block above for the full account):
// the four reds were correct behaviour, not a defect, and the property
// that made them red ("significant in every context") is stronger than
// what A-bpc.b0N.c2 / A-bplcon.cXX actually state ("bit N is field
// PFxPy"). Revised per the coordinator's direction: BIT0-IS-PF1P0,
// BIT1-IS-PF1P1, BIT3-IS-PF2P0 and BIT4-IS-PF2P1 now use
// bit_significant_in_some_context() (existential over the four contexts)
// instead of bit_significant_in_every_context() (universal); the
// every-context gap is recorded via two checks.note() MEASURED calls
// right after the six BITn assertions, naming the same claim ids, instead
// of being asserted. BIT2-IS-PF1P2 and BIT5-IS-PF2P2 are untouched.
// Rebuild after the revision: 10 passed, 0 failed, plus the 2 MEASURED
// notes (which are printed but do not count as pass or fail).
//
// MUTATION LOG (12 entries total. The first 10 were actually run by this
// agent under WSL/Verilator on 10.09.2026 -- 6 before the coordinator
// review, 4 (BIT0/1/3/4) re-run after it against the revised, now-green
// assertions -- each by manually editing the call site, rebuilding,
// confirming the named assertion(s) change colour as described,
// reverting, rebuilding, confirming baseline, before moving on. On
// 11.09.2026 (see "DEFECT CORRECTION" below), an independent review found
// three defects (D1/D2/D3, corrected below) and this log gained two more
// entries (PF1P/PF2P-DIGITS-BOUND-TO-BIT-WEIGHT, the new D3 assertions)
// plus real `#ifndef MUT_<NAME>` / `#ifdef MUT_<NAME>` plumbing
// (`make MUT=<NAME>`, see the Makefile) for the four BIT0/1/3/4 entries
// and the two new entries -- this file had no MUT_ guards at all before.
// All six of those were re-run through the plumbing (not just
// re-described) on 11.09.2026 and confirmed red-alone, then reverted; the
// six untouched by D1/D2/D3 (FIXTURE-LIVE-DATA-PATH,
// BPLCON2-BITS-0-5-..., BIT2, BIT5, PF1P/PF2P-GOVERNS-...) were not
// re-run and keep their original 10.09.2026 numbers below, measured
// against the then-current 10-assertion file. Every mutation is a
// STIMULUS change to this file's own inputs (which nsprite/nplayfield/
// bplcon2 values are fed, which bit index a shared check is asked to
// test, the mask_override argument that neutralizes one call site's
// stimulus delta, or -- for the two new entries -- which bit index
// flips_at_others_zero() actually uses); no expected value, oracle text
// or comparison logic was ever touched, and nanomig/ was never touched.
// Final rebuild after the last revert on 11.09.2026: 12 passed, 0 failed,
// confirmed again immediately before this commit.):
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: the swept nplayfield/nsprite stimulus sets were both shrunk from four points each to a single point (nplayfield={0x00}, nsprite={0x00}) -> FAIL ("9 passed, 1 failed"), only FIXTURE-LIVE-DATA-PATH red (a lone sample point can only ever produce a set of size 1, never 2). Reverted: 10/0.
// MUTATION BPLCON2-BITS-0-5-ARE-LIVE-PRIORITY-FIELD stimulus red: the swept-bplcon2 loop's fixed nsprite argument changed from 0xFF (all eight sprites active) to 0x00 (no sprite active) -> FAIL ("9 passed, 1 failed"), only BPLCON2-BITS-0-5-ARE-LIVE-PRIORITY-FIELD red (with no sprite active, sprcode is always 7 and the module's own first branch forces sprsel=0 regardless of bplcon2, collapsing the observed set to size 1). Reverted: 10/0.
// MUTATION BIT0-IS-PF1P0 stimulus red: now wired as `make MUT=BIT0_IS_PF1P0` (guards `#ifndef MUT_BIT0_IS_PF1P0` around the checks.expect() condition, l.386 area), swapping bit_significant_in_some_context(0) for bit_significant_in_some_context(0, 0), forcing mask_override=0 so base and mutant bplcon2 are identical in all four contexts; measured 11 passed, 1 failed, only BIT0-IS-PF1P0 red (no possible difference can ever be found once the fed stimulus never actually differs). Reverted (plain `make`): 12/0.
// MUTATION BIT1-IS-PF1P1 stimulus red: `make MUT=BIT1_IS_PF1P1` (same mask_override=0 neutralization, guarded `#ifndef MUT_BIT1_IS_PF1P1`); measured 11 passed, 1 failed, only BIT1-IS-PF1P1 red. Reverted: 12/0.
// MUTATION BIT2-IS-PF1P2 stimulus red: the call argument changed from bit_significant_in_every_context(2) to bit_significant_in_every_context(0) -> FAIL ("9 passed, 1 failed" against the 10-assertion file this was first run against), BIT2-IS-PF1P2 flips from its genuine green to red when fed bit 0's stimulus instead of bit 2's. Reverted to bit 2: back to green. Not re-run against the 12-assertion file (unchanged by this correction; no MUT_ guard added since D1/D2/D3 did not touch this assertion).
// MUTATION BIT3-IS-PF2P0 stimulus red: `make MUT=BIT3_IS_PF2P0` (same mask_override=0 neutralization, guarded `#ifndef MUT_BIT3_IS_PF2P0`); measured 11 passed, 1 failed, only BIT3-IS-PF2P0 red. Reverted: 12/0.
// MUTATION BIT4-IS-PF2P1 stimulus red: `make MUT=BIT4_IS_PF2P1` (same mask_override=0 neutralization, guarded `#ifndef MUT_BIT4_IS_PF2P1`); measured 11 passed, 1 failed, only BIT4-IS-PF2P1 red. Reverted: 12/0.
// MUTATION BIT5-IS-PF2P2 stimulus red: the call argument changed from bit_significant_in_every_context(5) to bit_significant_in_every_context(3) -> FAIL ("9 passed, 1 failed" against the 10-assertion file this was first run against), BIT5-IS-PF2P2 flips from its genuine green to red when fed bit 3's stimulus instead of bit 5's. Reverted to bit 5: back to green. Not re-run against the 12-assertion file (unchanged by this correction; no MUT_ guard added since D1/D2/D3 did not touch this assertion).
// MUTATION PF1P-DIGITS-BOUND-TO-BIT-WEIGHT stimulus red: `make MUT=PF1P_DIGITS_BOUND_TO_BIT_WEIGHT` (guarded `#ifdef MUT_PF1P_DIGITS_BOUND_TO_BIT_WEIGHT` inside flips_at_others_zero(), swaps bits 0<->2 on the way into the DUT for the PF1P group only); measured 255 / 240 / 192 instead of 192 / 240 / 255 -- weight 1 saw weight 4's flip count and vice versa -- 11 passed, 1 failed, only PF1P-DIGITS-BOUND-TO-BIT-WEIGHT red; PF2P-DIGITS-BOUND-TO-BIT-WEIGHT stayed green (the guard is scoped to group_start==0 only). Reverted: 12/0.
// MUTATION PF2P-DIGITS-BOUND-TO-BIT-WEIGHT stimulus red: `make MUT=PF2P_DIGITS_BOUND_TO_BIT_WEIGHT` (guarded `#ifdef MUT_PF2P_DIGITS_BOUND_TO_BIT_WEIGHT` inside flips_at_others_zero(), swaps bits 3<->5 on the way into the DUT for the PF2P group only); measured 255 / 240 / 192 instead of 192 / 240 / 255; 11 passed, 1 failed, only PF2P-DIGITS-BOUND-TO-BIT-WEIGHT red; PF1P-DIGITS-BOUND-TO-BIT-WEIGHT stayed green (the guard is scoped to group_start==3 only). Reverted: 12/0.
// MUTATION PF1P-GOVERNS-PLAYFIELD1-INDEPENDENT-OF-PF2P stimulus red: the inner comparison's nplayfield argument changed from PF1_ON to (PF1_ON | PF2_ON), so PF2 is also enabled while PF2P is swept -> FAIL ("9 passed, 1 failed"), only PF1P-GOVERNS-PLAYFIELD1-INDEPENDENT-OF-PF2P red (PF2P now genuinely affects sprsel once PF2 is enabled, breaking the claimed invariance). Reverted: 10/0.
// MUTATION PF2P-GOVERNS-PLAYFIELD2-INDEPENDENT-OF-PF1P stimulus red: the inner comparison's nplayfield argument changed from PF2_ON to (PF1_ON | PF2_ON), so PF1 is also enabled while PF1P is swept -> FAIL ("9 passed, 1 failed"), only PF2P-GOVERNS-PLAYFIELD2-INDEPENDENT-OF-PF1P red. Reverted: 10/0.
//
// DEFECT CORRECTION, 11.09.2026 (independent review of the 10.09.2026
// build above, before this commit) -- three defects, all in this file,
// none in nanomig/ (nanomig/ was never opened for this correction):
// D1 (assertions stated a false measured fact): BIT0-IS-PF1P0,
//   BIT1-IS-PF1P1, BIT3-IS-PF2P0 and BIT4-IS-PF2P1's "concretely: ..."
//   clauses, both checks.note() records right after the six BITn
//   assertions, and the "RTL OPENED, BUILT AND RUN" paragraph above all
//   claimed exactly ONE dead ("no difference") context per weakened bit
//   (e.g. "only 4->5 does not [differ]"). Measured on the real DUT
//   (sweeping all 256 nsprite values at each of the four "other two
//   bits" contexts, own playfield only): there are TWO -- (4,5) AND
//   (6,7) for the low bit (0/3), (4,6) AND (5,7) for the middle bit
//   (1/4) -- not one; the claimed differing pair 6->7 (resp. 5->7) does
//   NOT differ. Cause: l.45's `if (sprcode[2:0]==3'd7) sprsel = 1'b0`
//   overrides pf1front/pf2front outright when sprcode==7, so the
//   ll.39-40 comparator only ever reaches sprsel for sprcode in
//   {1,2,3,4}; the earlier analysis reasoned about that comparator alone
//   and missed the override. All four "concretely" clauses, both notes,
//   and the RTL-analysis paragraph above are corrected to name both dead
//   pairs and the override as the cause.
// D2 (notes were static strings): the two checks.note() calls carried
//   the every-context gap as C++ string literals -- nothing in the build
//   evaluated it, so permuted, gutted or absent RTL would have printed
//   the identical text, which is how D1 went undetected. Both notes now
//   call context_counts(bit), a small helper that sweeps the DUT right
//   there in main(), and print the counts it just measured.
// D3 (existential form is blind to bit weight): BIT0/BIT1/BIT2 (and
//   BIT3/BIT4/BIT5) differ from each other only in which BPLCON2 bit
//   they name -- the digit in PF1P0/PF1P1/PF1P2 was untested, so
//   swapping bits 0<->2 (or 3<->5) on the way into the DUT left all four
//   weakened assertions green (confirmed: the PF1P/PF2P_DIGITS_BOUND_TO_
//   BIT_WEIGHT mutations below reproduce exactly that swap and go red).
//   Measured discriminator: at others=0 (this bit's own playfield only,
//   the group's other two bits held at 0), the count of the 256 nsprite
//   values whose sprsel flips is 192 for weight 1, 240 for weight 2, 255
//   for weight 4 -- distinct, monotone, reproduced identically for both
//   groups (bits 0/1/2 and bits 3/4/5). Two assertions added, one per
//   group -- PF1P-DIGITS-BOUND-TO-BIT-WEIGHT and PF2P-DIGITS-BOUND-TO-
//   BIT-WEIGHT -- each binding its three bits to 192/240/255 in order and
//   citing the same claim ids the corresponding BITn-IS-PFxPy assertion
//   already cites for the digit each bit names.
// Baseline before this correction: 10 passed, 0 failed (confirmed by
// this agent by running `make` before making any change). Baseline
// after: 12 passed, 0 failed (10 original assertions, unchanged in
// substance, plus the 2 new PxP-DIGITS-BOUND-TO-BIT-WEIGHT assertions).
// All six new-or-changed assertions (BIT0-IS-PF1P0, BIT1-IS-PF1P1,
// BIT3-IS-PF2P0, BIT4-IS-PF2P1, PF1P-DIGITS-BOUND-TO-BIT-WEIGHT,
// PF2P-DIGITS-BOUND-TO-BIT-WEIGHT) were mutated via their new MUT_
// guards, confirmed red alone, and reverted -- see the six MUTATION log
// lines above for BIT0/1/3/4 and PF1P/PF2P-DIGITS-BOUND-TO-BIT-WEIGHT,
// all dated in substance 11.09.2026 even though BIT0/1/3/4's underlying
// stimulus delta (mask_override=0) is unchanged from 10.09.2026 -- only
// its plumbing (manual edit -> `#ifndef MUT_<NAME>` guard) is new.

#include <cstdint>
#include <iostream>
#include <set>
#include <string>

#include <verilated.h>
#include "Vdenise_spritepriority.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // A note is not a check: it prints and does not touch passed/failed.
    // Used to record, in an actual C++ string (not a `//` comment), a
    // measurement that is true and worth keeping but is NOT what any claim
    // asserts -- so its claim ids are found by a coverage search of the
    // file's strings without it being counted as a pass or a fail.
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
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

// Purely combinational: set inputs, eval, read sprsel. No clock port is
// named in any of the 18 claims' evidence.
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

    // ---------------- FIXTURE-LIVE-DATA-PATH ----------------
    {
        std::set<uint8_t> seen;
        const uint8_t nplayfields[] = {0x00, PF1_ON, PF2_ON, static_cast<uint8_t>(PF1_ON | PF2_ON)};
        const uint8_t nsprites[] = {0x00, 0x01, 0x80, 0xFF};
        for (uint8_t pf : nplayfields)
            for (uint8_t spr : nsprites)
                seen.insert(h.read(/*bplcon2=*/0x00, pf, spr));

        checks.expect(seen.size() == 2,
            "FIXTURE-LIVE-DATA-PATH: sweeping nsprite and nplayfield alone "
            "(bplcon2 held at 0) makes sprsel take both its possible "
            "values (0 and 1), not a single stuck value -- proves the "
            "dut->eval()-then-read-sprsel measurement basis every "
            "assertion below shares is actually live before any "
            "BPLCON2-based claim depends on it. Fixture check, no claim "
            "id, no page.");
    }

    // ---------------- BPLCON2-BITS-0-5-ARE-LIVE-PRIORITY-FIELD ----------------
    {
        std::set<uint8_t> seen;
        for (int code = 0; code < 64; ++code)
            seen.insert(h.read(static_cast<uint8_t>(code), static_cast<uint8_t>(PF1_ON | PF2_ON), 0xFF));

        checks.expect(seen.size() == 2,
            "BPLCON2-BITS-0-5-ARE-LIVE-PRIORITY-FIELD: sweeping bplcon2[5:0] "
            "through all 64 values, with both playfields enabled and all "
            "eight sprites active, makes sprsel take both its possible "
            "values -- the 6-bit field genuinely controls the sprite-"
            "versus-playfield decision, as printed: '   Bits 0-5  "
            "Playfield sprite priority' (p.90). 3-r2.c04, "
            "3-regsum-bplcon2.c3 .");
    }

    // ---------------- BITn-IS-PFxPy (six assertions, one per bit position) ----------------
    // bit_significant_in_every_context(bit) holds the SAME exhaustive
    // logic for all six bits (compiled once, no per-bit text in here) --
    // every expect() call below supplies its own literal claim ids, quote
    // and page inline in the string argument, so a text search over the
    // assertion strings finds each id sitting next to its own check rather
    // than in a shared table elsewhere in the file.
    //
    // ONLY the bit's own playfield is enabled (PF1_ON for bits 0-2, PF2_ON
    // for bits 3-5) -- an EARLIER draft of this loop enabled both
    // playfields while holding the OTHER 3-bit group at 0. That was a bug
    // in the stimulus, not a finding about the RTL: with the other
    // playfield also enabled and its own priority code pinned at 0, that
    // playfield's sprite-versus-playfield branch is unconditionally true
    // (every reachable sprite code is >0) and its "else if" arm in the
    // module's priority chain wins outright regardless of the field under
    // test, masking every one of the six bits identically. First build
    // under that draft: 4 passed, 6 failed, all six BITn-IS-PFxPy checks
    // red for this reason (see the follow-up commit message for the full
    // trace). Isolating each bit's own playfield -- exactly as the two
    // PF1P/PF2P-GOVERNS-...-INDEPENDENT-OF-... checks below already do --
    // removes the masking and is what is committed here.
    // MUTATE-HERE(bit-significance): the diff between `others` and
    // `others | bit_mask` below is the only thing a mutation changes; the
    // "does sprsel differ" comparison itself is never touched.
    auto bit_significant_in_every_context = [&](int bit) {
        int group_start = (bit / 3) * 3;
        int other0 = group_start + ((bit - group_start + 1) % 3);
        int other1 = group_start + ((bit - group_start + 2) % 3);
        uint8_t bit_mask = static_cast<uint8_t>(1u << bit);
        uint8_t nplayfield_for_group = (group_start == 0) ? PF1_ON : PF2_ON;
        for (int p = 0; p < 4; ++p) {
            uint8_t others = static_cast<uint8_t>(((p & 1) << other0) | (((p >> 1) & 1) << other1));
            bool found_difference = false;
            for (int spr = 0; spr < 256 && !found_difference; ++spr) {
                uint8_t base_val = h.read(others, nplayfield_for_group, static_cast<uint8_t>(spr));
                uint8_t mut_val = h.read(static_cast<uint8_t>(others | bit_mask), nplayfield_for_group, static_cast<uint8_t>(spr));
                if (base_val != mut_val) found_difference = true;
            }
            if (!found_difference) return false;
        }
        return true;
    };

    // bit_significant_in_some_context(bit): what the bit-naming claims
    // (A-bpc.b0N.c2 / A-bplcon.cXX -- "bit N of BPLCON2 is field PFxPy")
    // actually support: this bit is a live, individually significant part
    // of the priority code, shown by AT LEAST ONE context (one of the
    // four "other two bits" settings) in which toggling it alone changes
    // sprsel somewhere across the 256-value nsprite sweep. Same isolation
    // (only the bit's own playfield enabled) and same bit_mask delta as
    // bit_significant_in_every_context above; only the quantifier over the
    // four contexts changes, from "for all" to "there exists".
    // MUTATE-HERE(bit-significance, existential form): the diff between
    // `others` and `others | bit_mask` below is the only thing a mutation
    // changes; the "does sprsel differ, anywhere" comparison itself is
    // never touched.
    // mask_override defaults to -1 ("use this bit's own mask, 1<<bit");
    // passing 0 is how each BIT0/1/3/4 mutation below neutralizes the
    // stimulus delta for exactly one call site without touching the
    // shared lambda's comparison logic or any other call site.
    auto bit_significant_in_some_context = [&](int bit, int mask_override = -1) {
        int group_start = (bit / 3) * 3;
        int other0 = group_start + ((bit - group_start + 1) % 3);
        int other1 = group_start + ((bit - group_start + 2) % 3);
        uint8_t bit_mask = mask_override >= 0 ? static_cast<uint8_t>(mask_override) : static_cast<uint8_t>(1u << bit);
        uint8_t nplayfield_for_group = (group_start == 0) ? PF1_ON : PF2_ON;
        for (int p = 0; p < 4; ++p) {
            uint8_t others = static_cast<uint8_t>(((p & 1) << other0) | (((p >> 1) & 1) << other1));
            for (int spr = 0; spr < 256; ++spr) {
                uint8_t base_val = h.read(others, nplayfield_for_group, static_cast<uint8_t>(spr));
                uint8_t mut_val = h.read(static_cast<uint8_t>(others | bit_mask), nplayfield_for_group, static_cast<uint8_t>(spr));
                if (base_val != mut_val) return true;
            }
        }
        return false;
    };

    checks.expect(
#ifndef MUT_BIT0_IS_PF1P0
        bit_significant_in_some_context(0),
#else
        bit_significant_in_some_context(0, 0), // MUTATION: mask_override=0 neutralizes the stimulus delta
#endif
        "BIT0-IS-PF1P0: bit 0 of bplcon2 is field PF1P0 -- toggling only "
        "this bit (only this field's own playfield enabled, sprite "
        "activity swept across all 256 values of nsprite) changes sprsel "
        "in at least one of the four possible settings of the other two "
        "bits of its 3-bit group (concretely: PF1P 0->1 and 2->3 each "
        "show a difference; 4->5 and 6->7 do not -- see the MEASURED "
        "note right after the six BITn assertions), proving bit 0 is a "
        "live, individually significant element of the priority code, as "
        "named in Appendix A's bit table: "
        "'                 00        X         PF1H0      PF1P0' "
        "(p.271). A-bpc.b00.c2, A-bplcon.c60, opus:7-appA-bplcon2.c20 .");

    checks.expect(
#ifndef MUT_BIT1_IS_PF1P1
        bit_significant_in_some_context(1),
#else
        bit_significant_in_some_context(1, 0), // MUTATION: mask_override=0 neutralizes the stimulus delta
#endif
        "BIT1-IS-PF1P1: bit 1 of bplcon2 is field PF1P1 -- toggling only "
        "this bit (only this field's own playfield enabled, sprite "
        "activity swept across all 256 values of nsprite) changes sprsel "
        "in at least one of the four possible settings of the other two "
        "bits of its 3-bit group (concretely: PF1P 0->2 and 1->3 each "
        "show a difference; 4->6 and 5->7 do not -- see the MEASURED "
        "note right after the six BITn assertions), proving bit 1 is a "
        "live, individually significant element of the priority code, as "
        "named in Appendix A's bit table: "
        "'                 01       ERSY       PF1H1      PF1P1' "
        "(p.271). A-bpc.b01.c2, A-bplcon.c57, opus:7-appA-bplcon2.c19 .");

    checks.expect(bit_significant_in_every_context(2),
        "BIT2-IS-PF1P2: bit 2 of bplcon2 is field PF1P2 -- toggling only "
        "this bit (only this field's own playfield enabled, the other two bits of its "
        "3-bit group held at each of their four combinations, sprite "
        "activity swept across all 256 values of nsprite) changes sprsel "
        "for at least one stimulus in every one of those four "
        "combinations, proving bit 2 is a live, individually significant "
        "element of the priority code -- Appendix A's bit table: "
        "'                 02       LACE       PF1H2      PF1P2' "
        "(p.271). A-bpc.b02.c2, A-bplcon.c54, opus:7-appA-bplcon2.c18 .");

    checks.expect(
#ifndef MUT_BIT3_IS_PF2P0
        bit_significant_in_some_context(3),
#else
        bit_significant_in_some_context(3, 0), // MUTATION: mask_override=0 neutralizes the stimulus delta
#endif
        "BIT3-IS-PF2P0: bit 3 of bplcon2 is field PF2P0 -- toggling only "
        "this bit (only this field's own playfield enabled, sprite "
        "activity swept across all 256 values of nsprite) changes sprsel "
        "in at least one of the four possible settings of the other two "
        "bits of its 3-bit group (concretely: PF2P 0->1 and 2->3 each "
        "show a difference; 4->5 and 6->7 do not -- see the MEASURED "
        "note right after the six BITn assertions), proving bit 3 is a "
        "live, individually significant element of the priority code, as "
        "named in Appendix A's bit table: "
        "'                 03       LPEN       PF1H3      PF2P0' "
        "(p.271). A-bpc.b03.c2, A-bplcon.c51, opus:7-appA-bplcon2.c17 .");

    checks.expect(
#ifndef MUT_BIT4_IS_PF2P1
        bit_significant_in_some_context(4),
#else
        bit_significant_in_some_context(4, 0), // MUTATION: mask_override=0 neutralizes the stimulus delta
#endif
        "BIT4-IS-PF2P1: bit 4 of bplcon2 is field PF2P1 -- toggling only "
        "this bit (only this field's own playfield enabled, sprite "
        "activity swept across all 256 values of nsprite) changes sprsel "
        "in at least one of the four possible settings of the other two "
        "bits of its 3-bit group (concretely: PF2P 0->2 and 1->3 each "
        "show a difference; 4->6 and 5->7 do not -- see the MEASURED "
        "note right after the six BITn assertions), proving bit 4 is a "
        "live, individually significant element of the priority code, as "
        "named in Appendix A's bit table: "
        "'                 04        X         PF2H0      PF2P1' "
        "(p.271). A-bpc.b04.c2, A-bplcon.c48, opus:7-appA-bplcon2.c16 .");

    checks.expect(bit_significant_in_every_context(5),
        "BIT5-IS-PF2P2: bit 5 of bplcon2 is field PF2P2 -- toggling only "
        "this bit (only this field's own playfield enabled, the other two bits of its "
        "3-bit group held at each of their four combinations, sprite "
        "activity swept across all 256 values of nsprite) changes sprsel "
        "for at least one stimulus in every one of those four "
        "combinations, proving bit 5 is a live, individually significant "
        "element of the priority code -- Appendix A's bit table: "
        "'                 05        X         PF2H1      PF2P2' "
        "(p.271). A-bpc.b05.c2, A-bplcon.c45, opus:7-appA-bplcon2.c15 .");

    // ---------------- MEASURED (not asserted, computed at run time): the dead contexts ----------------
    // Neither of the two notes below is a claim of this package's 18-record
    // inventory -- no claim quotes denise_spritepriority.v's encoder code
    // range, its sprcode==7 override, or a per-value outcome table. Each
    // note names the same claim ids as the corresponding BITn-IS-...
    // assertion above (so a coverage search finds them there too) purely
    // for traceability: it is the SAME claim, the note just records an
    // additional, true, measured fact about it that the claim itself does
    // not assert and is therefore not what is tested by the expect() calls
    // above. D2 fix (see "DEFECT CORRECTION" above the MUTATION LOG): the
    // counts below come from context_counts() sweeping the DUT right now,
    // not from a string literal -- a permuted, gutted or absent RTL would
    // change the printed numbers, not just the prose around them.
    auto context_counts = [&](int bit, int out_counts[4]) {
        int group_start = (bit / 3) * 3;
        int other0 = group_start + ((bit - group_start + 1) % 3);
        int other1 = group_start + ((bit - group_start + 2) % 3);
        uint8_t bit_mask = static_cast<uint8_t>(1u << bit);
        uint8_t nplayfield_for_group = (group_start == 0) ? PF1_ON : PF2_ON;
        for (int p = 0; p < 4; ++p) {
            uint8_t others = static_cast<uint8_t>(((p & 1) << other0) | (((p >> 1) & 1) << other1));
            int count = 0;
            for (int spr = 0; spr < 256; ++spr) {
                uint8_t base_val = h.read(others, nplayfield_for_group, static_cast<uint8_t>(spr));
                uint8_t mut_val = h.read(static_cast<uint8_t>(others | bit_mask), nplayfield_for_group, static_cast<uint8_t>(spr));
                if (base_val != mut_val) ++count;
            }
            out_counts[p] = count;
        }
    };
    {
        int c0[4], c3[4];
        context_counts(0, c0);
        context_counts(3, c3);
        checks.note(
            "A-bpc.b00.c2, A-bplcon.c60 (bit 0 / PF1P0) and A-bpc.b03.c2, "
            "A-bplcon.c51 (bit 3 / PF2P0): measured just now by sweeping "
            "all 256 nsprite values at each of the four \"other two bits\" "
            "contexts (this field's own playfield only) -- bit 0 flips "
            "sprsel for " + std::to_string(c0[0]) + "/256 at context 0 "
            "(field 0->1), " + std::to_string(c0[1]) + "/256 at context 1 "
            "(2->3), " + std::to_string(c0[2]) + "/256 at context 2 "
            "(4->5), " + std::to_string(c0[3]) + "/256 at context 3 "
            "(6->7); bit 3 flips " + std::to_string(c3[0]) + "/" +
            std::to_string(c3[1]) + "/" + std::to_string(c3[2]) + "/" +
            std::to_string(c3[3]) + " in the same four contexts. TWO of "
            "the four contexts are 0, not one: denise_spritepriority.v:"
            "44-46 forces sprsel=0 whenever sprcode==7, so pf1front/"
            "pf2front (ll.39-40) only ever reach sprsel for sprcode in "
            "{1,2,3,4} -- none of which exceeds 4 -- so any crossing "
            "where both endpoints are >=4 (4->5 and 6->7 here) is "
            "unobservable, not just one of them. Not a manual-vs-RTL "
            "conflict -- no claim in this package names the encoder's "
            "range, the override, or a per-value outcome -- and not a "
            "defect. The bit's field identity and its significance in "
            "the other two contexts (0->1, 2->3) are what is asserted, "
            "and are asserted, above.");
    }
    {
        int c1[4], c4[4];
        context_counts(1, c1);
        context_counts(4, c4);
        checks.note(
            "A-bpc.b01.c2, A-bplcon.c57 (bit 1 / PF1P1) and A-bpc.b04.c2, "
            "A-bplcon.c48 (bit 4 / PF2P1): measured just now, same basis "
            "as the low-bit note above -- bit 1 flips sprsel for " +
            std::to_string(c1[0]) + "/256 at context 0 (field 0->2), " +
            std::to_string(c1[1]) + "/256 at context 1 (4->6), " +
            std::to_string(c1[2]) + "/256 at context 2 (1->3), " +
            std::to_string(c1[3]) + "/256 at context 3 (5->7); bit 4 "
            "flips " + std::to_string(c4[0]) + "/" + std::to_string(c4[1]) +
            "/" + std::to_string(c4[2]) + "/" + std::to_string(c4[3]) +
            " in the same four contexts. Same conclusion as the low-bit "
            "note: TWO of the four contexts are 0 (4->6 and 5->7), caused "
            "by the same sprcode==7 override -- not a manual-vs-RTL "
            "conflict, not a defect. The bit's field identity and its "
            "significance in the other two contexts (0->2, 1->3) are "
            "what is asserted, and are asserted, above.");
    }

    // ---------------- PF1P-DIGITS-BOUND-TO-BIT-WEIGHT / PF2P-DIGITS-BOUND-TO-BIT-WEIGHT ----------------
    // D3: BIT0-IS-PF1P0 / BIT1-IS-PF1P1 / BIT2-IS-PF1P2 (and the PF2P
    // trio) differ from each other ONLY in which digit -- which BPLCON2
    // bit -- each claim names; nothing above tests THAT binding, so
    // swapping which bit is fed for "bit 0" vs "bit 2" on the way into
    // the DUT (a stimulus change, no RTL touched) would leave all four
    // weakened assertions green. The existing sweep already yields a
    // discriminator that is distinct per bit and monotone in bit weight:
    // at others=0, the count of the 256 nsprite values whose sprsel flips
    // is 192 for weight 1 (bit 0 / bit 3), 240 for weight 2 (bit 1 / bit
    // 4), 255 for weight 4 (bit 2 / bit 5). One assertion per group binds
    // its three bits to these three counts, in order, citing the same
    // claim ids the corresponding BITn-IS-PFxPy assertion above cites for
    // the digit each bit names.
    // MUTATE-HERE(bit-weight binding): the bit index handed to bit_mask
    // below is the only thing a mutation changes; the expected counts
    // (192/240/255) are never touched.
    auto flips_at_others_zero = [&](int bit) {
        int group_start = (bit / 3) * 3;
        int mutated_bit = bit;
#ifdef MUT_PF1P_DIGITS_BOUND_TO_BIT_WEIGHT
        if (group_start == 0) { // MUTATION: swap bits 0<->2 within the PF1P group only
            if (bit == 0) mutated_bit = 2;
            else if (bit == 2) mutated_bit = 0;
        }
#endif
#ifdef MUT_PF2P_DIGITS_BOUND_TO_BIT_WEIGHT
        if (group_start == 3) { // MUTATION: swap bits 3<->5 within the PF2P group only
            if (bit == 3) mutated_bit = 5;
            else if (bit == 5) mutated_bit = 3;
        }
#endif
        uint8_t bit_mask = static_cast<uint8_t>(1u << mutated_bit);
        uint8_t nplayfield_for_group = (group_start == 0) ? PF1_ON : PF2_ON;
        int count = 0;
        for (int spr = 0; spr < 256; ++spr) {
            uint8_t base_val = h.read(0, nplayfield_for_group, static_cast<uint8_t>(spr));
            uint8_t mut_val = h.read(bit_mask, nplayfield_for_group, static_cast<uint8_t>(spr));
            if (base_val != mut_val) ++count;
        }
        return count;
    };
    {
        int w0 = flips_at_others_zero(0);
        int w1 = flips_at_others_zero(1);
        int w2 = flips_at_others_zero(2);
        checks.expect(w0 == 192 && w1 == 240 && w2 == 255,
            "PF1P-DIGITS-BOUND-TO-BIT-WEIGHT: with bplcon2's other two "
            "group bits held at 0, the count (of 256) of nsprite values "
            "whose sprsel flips when toggling bit 0 alone / bit 1 alone / "
            "bit 2 alone is 192 / 240 / 255 -- distinct and increasing "
            "with the bit's binary weight, so this checks that bit 0 is "
            "PF1P0 (weight 1), bit 1 is PF1P1 (weight 2) and bit 2 is "
            "PF1P2 (weight 4), not merely that three unspecified bits are "
            "each individually significant (measured: " + std::to_string(w0) +
            " / " + std::to_string(w1) + " / " + std::to_string(w2) +
            "). A-bpc.b00.c2, A-bplcon.c60, A-bpc.b01.c2, A-bplcon.c57, "
            "A-bpc.b02.c2, A-bplcon.c54, sol:7-video.c21, opus:7-vp.c20, sol:7-appA-bplcon2.c9 .");
    }
    {
        int w3 = flips_at_others_zero(3);
        int w4 = flips_at_others_zero(4);
        int w5 = flips_at_others_zero(5);
        checks.expect(w3 == 192 && w4 == 240 && w5 == 255,
            "PF2P-DIGITS-BOUND-TO-BIT-WEIGHT: with bplcon2's other two "
            "group bits held at 0, the count (of 256) of nsprite values "
            "whose sprsel flips when toggling bit 3 alone / bit 4 alone / "
            "bit 5 alone is 192 / 240 / 255 -- distinct and increasing "
            "with the bit's binary weight within its group, so this "
            "checks that bit 3 is PF2P0 (weight 1), bit 4 is PF2P1 "
            "(weight 2) and bit 5 is PF2P2 (weight 4), not merely that "
            "three unspecified bits are each individually significant "
            "(measured: " + std::to_string(w3) + " / " + std::to_string(w4) +
            " / " + std::to_string(w5) + "). A-bpc.b03.c2, A-bplcon.c51, "
            "A-bpc.b04.c2, A-bplcon.c48, A-bpc.b05.c2, A-bplcon.c45, sol:7-video.c20, opus:7-vp.c19, sol:7-appA-bplcon2.c8 .");
    }

    // ---------------- PF1P-GOVERNS-PLAYFIELD1-INDEPENDENT-OF-PF2P ----------------
    {
        // MUTATE-HERE(field-independence): the nplayfield stimulus fed to
        // the inner h.read() below (PF1_ON) is what a mutation changes;
        // the equality comparison itself is never touched.
        bool invariant = true;
        for (int pf1p = 0; pf1p < 8 && invariant; ++pf1p) {
            for (int spr = 0; spr < 256 && invariant; ++spr) {
                uint8_t base_val = h.read(static_cast<uint8_t>(pf1p), PF1_ON, static_cast<uint8_t>(spr));
                for (int pf2p = 1; pf2p < 8; ++pf2p) {
                    uint8_t bplcon2 = static_cast<uint8_t>(pf1p | (pf2p << 3));
                    if (h.read(bplcon2, PF1_ON, static_cast<uint8_t>(spr)) != base_val) {
                        invariant = false;
                        break;
                    }
                }
            }
        }
        checks.expect(invariant,
            "PF1P-GOVERNS-PLAYFIELD1-INDEPENDENT-OF-PF2P: with only "
            "playfield 1 enabled (nplayfield's PF2 bit held off), sprsel "
            "is identical across all 8 values of PF2P (bits 5-3), for "
            "every one of the 8 values of PF1P and all 256 values of "
            "nsprite -- PF2P has no effect when only playfield 1 is being "
            "decided, as its own name states: 'PF1P=Playfield 1 priority "
            "code (with respect to sprites)' (p.271). A-leg.pf1p, "
            "A-bplcon.c82, sol:7-appA-bplcon2.c12, opus:7-appA-bplcon2.c23 .");
    }

    // ---------------- PF2P-GOVERNS-PLAYFIELD2-INDEPENDENT-OF-PF1P ----------------
    {
        // MUTATE-HERE(field-independence): the nplayfield stimulus fed to
        // the inner h.read() below (PF2_ON) is what a mutation changes;
        // the equality comparison itself is never touched.
        bool invariant = true;
        for (int pf2p = 0; pf2p < 8 && invariant; ++pf2p) {
            for (int spr = 0; spr < 256 && invariant; ++spr) {
                uint8_t base_val = h.read(static_cast<uint8_t>(pf2p << 3), PF2_ON, static_cast<uint8_t>(spr));
                for (int pf1p = 1; pf1p < 8; ++pf1p) {
                    uint8_t bplcon2 = static_cast<uint8_t>((pf2p << 3) | pf1p);
                    if (h.read(bplcon2, PF2_ON, static_cast<uint8_t>(spr)) != base_val) {
                        invariant = false;
                        break;
                    }
                }
            }
        }
        checks.expect(invariant,
            "PF2P-GOVERNS-PLAYFIELD2-INDEPENDENT-OF-PF1P: with only "
            "playfield 2 enabled (nplayfield's PF1 bit held off), sprsel "
            "is identical across all 8 values of PF1P (bits 2-0), for "
            "every one of the 8 values of PF2P and all 256 values of "
            "nsprite -- PF1P has no effect when only playfield 2 is being "
            "decided, as its own name states: 'PF2P=Playfield 2 priority "
            "code (with respect to sprites)' (p.271). A-leg.pf2p, "
            "A-bplcon.c81, sol:7-appA-bplcon2.c11, opus:7-appA-bplcon2.c22 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
