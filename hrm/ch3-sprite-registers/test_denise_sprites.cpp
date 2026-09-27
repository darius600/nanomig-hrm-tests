// ch3-sprite-registers -- denise_sprites conformance test
//
// Test purpose: the appendix C sprite-register fields Denise's ECS
//   SuperHires mode adds -- the extra low-order horizontal-start bit (SH0)
//   that SPRxCTL carries as an unconditional 140ns increment of the same
//   9-bit start value whose upper bits (SH8-SH1) SPRxPOS carries, the
//   further 70ns/35ns SuperHires increment bits (SHSH1, SHSH0) the same
//   register adds on ECS, the joint role of SPRxPOS/ SPRxCTL as
//   position-and-feature registers, and that these registers accept a direct
//   (non-DMA) write -- all at the denise_sprites module boundary. Package
//   ch3-sprite-registers, fixture denise_sprites (one Verilator build, this
//   module only, per PACKAGE.md).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 297, 298, 303 (claims.jsonl for ch3-sprite-registers, 20 records,
//   plus 17 more routed here in a later extension pass -- see CLAIM
//   COVERAGE ADDENDUM below -- plus a repair pass fixing 8 under-graded
//   backlog ids and 3 pre-backlog ids with the same defects -- see
//   REPAIR PASS below). A red result below means denise_sprites.v
//   disagrees with the manual, not that the assertion is wrong -- no
//   assertion here was, or should be, adjusted to make the RTL pass.
// CORRECTION (this pass, working from p303.txt -- print page 303 -- read
//   in full against the RTL findings already on record below): two
//   assertions in an earlier version of this file misstated the manual
//   and are replaced here, not weakened. SPRXCTL-BIT0-SH0-ONLY-IN-
//   SUPERHIRES required SH0 to be inert outside SuperHires -- a
//   restriction the manual's own bit-0 row never states ('00      SH0
//   Start horiz. value Low bit 140 ns increment', p.303, no resolution
//   qualifier); replaced below by SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT.
//   SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS required bit 3 to be a live field,
//   when the manual prints that bit's own row's verdict already in the
//   word: '03      SHSH0     Start horizontal (SHR mode) 35ns
//   (unimplemented)' (p.303); replaced below by two assertions,
//   SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE (bit 4, still required live)
//   and SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED (bit 3, declined -- nothing left
//   to require of a field the manual itself marks unimplemented). Both
//   corrected assertions keep every claim id their predecessor cited (see
//   CLAIM COVERAGE below for where each landed); see RESULTS for what
//   each one measured.
// Non-goals: "size" (vertical extent, VSTART/VSTOP) is part of what
//   C-shres-sprite.c4/C-spr.c03 say SPRxPOS/SPRxCTL jointly control, but
//   this module's own port list (below) carries no vertical-position input
//   at all -- only `hpos`. SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING therefore
//   asserts only the two components this boundary can observe, position and
//   feature (attach) control, and says so in its own label rather than
//   silently dropping the "size" half of the claim. C-shres-sprite.c5, whose
//   entire restatement is the "size" clause alone, is withdrawn whole to a
//   record instead of kept there with a caveat (this repair pass) -- see
//   RESULTS below.
//   SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE tests only that bit 4 is live
//   (changes something observable), not the OCS/ECS chip-scoping the
//   manual's NOTE separately states ('Note: bits 3 and 4 are in the ECS
//   chips only.', p.303), because this module exposes a single `aga` input,
//   not a finer OCS-vs-ECS select -- documented in that assertion's own
//   label, not silently assumed. Bit 3 (SHSH0) is declined rather than
//   asserted, by SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED -- the manual's own row
//   already gives its verdict on that bit, so there is nothing left for this
//   boundary to require of it.
// Claim ids: ids such as `C-shres-sprite.c7` and `C-shres-sprite.c8` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch3.json lists under `inventories`.
//
// BOUNDARY: nanomig/src/minimig-aga/denise_sprites.v:55,67 (SPRPOSCTLBASE
//   9'h140), per PACKAGE.md, whose own evidence field additionally cites
//   the sprite resolution select at denise.v:252 (`spres` =
//   `bplcon3[7:6] & aga`) and denise.v:390-391/393-416 (instantiation
//   with `reg_address_in`, `hpos`, `sprena`). That `& aga` term means
//   `spres` can only be non-zero one level up, in denise.v, when `aga` is
//   also set -- a fact about denise.v's wiring, outside this package's
//   boundary and not tested here (denise_sprites itself takes `spres` as
//   a bare input and is driven directly by this harness, independent of
//   its own `aga` input). Worth flagging for whoever owns the wider
//   boundary: the manual frames SuperHires sprite positioning as an ECS
//   Denise feature, and this wiring appears to make it reachable only
//   when AGA is also selected.
//
// ORDER OF WORK: this file (assertions, expected values, claim-id
// coverage, register addresses, bit positions) was written and committed
// from claims.jsonl's quotations and PACKAGE.md's own evidence field
// ONLY -- denise_sprites.v itself was not opened until after that commit,
// per the build brief's explicit gate. The register word-address
// convention ($140->0xA0 for SPR0POS, +4 words per sprite for SPR1's own
// registers), the clk7_en/c1/c3 four-phase clocking, the reg_address_in/
// data_in write sequencing, the SPR0DATA=0x8000 arm-on-write convention,
// and the SPR1CTL bit 7 (ATTACH) -> combined-colour-index convention are
// all reused from hrm/denise_sprites/test_denise_sprites.cpp -- an
// already-committed, already-built sibling harness for the SAME module --
// not learned by opening this package's own forbidden-until-committed
// file. The two numeric predictions this file stakes out before opening
// the RTL -- that a +1 step of SPRxPOS's byte moves the measured start by
// exactly 2 raw units, and that SPRxCTL's bit 0 (SH0) moves it by exactly
// 1 -- follow directly from the two quoted bit-table rows themselves
// ("07-00 SH8-SH1 Start horizontal value. Low bit (SH0) in SPRxCTL.":
// SPRxPOS[7:0] holds the *upper* 8 bits of a 9-bit start value, so its
// LSB is worth 2 raw units; SPRxCTL bit 0 is that missing raw LSB, worth
// 1) -- not from the RTL, and not from the sibling harness's own
// observation that SPR0POS=0x40 puts the sprite near hpos 0x80 (that
// observation is consistent with, but not needed to derive, the doubling
// relationship asserted here).
//
// MEASUREMENT BASIS: every check below arms sprite 0 (and, for the
// ATTACH check, sprite 1) with the same DATB=0x0000/DATA=0x8000 image
// (colour index 1, or 13 when attached to an armed sprite 1 -- both
// values already validated as reachable by the sibling harness) and
// reads `sprdata` while sweeping `hpos` 0..511 (the start value SH8-SH0
// is 9 bits, so 512 covers its whole range) from a freshly reset DUT
// each time, to keep every measurement isolated from any earlier one.
// "Start" means the first hpos at which sprdata is non-zero. No
// assertion below depends on the *absolute* value of that first hpos
// (which carries whatever fixed pipeline latency the comparator has,
// unmeasured here) -- every position-based assertion instead compares
// two starts measured the same way, so a constant latency cancels out.
//
// CLAIM COVERAGE: all 20 records of claims.jsonl are cited below, in
// full, inside an expect() label. Ids restating the identical
// proposition under a different manual wording, page, or reader are
// grouped and cited together on the single assertion that tests that
// proposition, as hrm/ch3-collision-registers/ and
// hrm/ch3-sprite-playfield-priority/ both do.
//
//   PROCESSOR-WRITE-REACHES-SPRXPOS (1 id)         -- C-shres-sprite.c7
//   SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 (2 ids)      -- C-shres-sprite.c8,
//                                                      C-spr.c05
//   SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT (2 ids)      -- C-shres-sprite.c12,
//                                                      C-shres-sprite.c9
//   SH0-IS-HALF-OF-POS-BYTE-STEP (2 ids)           -- C-shres-sprite.c3,
//                                                      C-spr.c02
//   SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING (3 ids)   -- C-shres-sprite.c4,
//     C-shres-sprite.c5, C-spr.c03
//   SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE (9 ids) -- C-library.c2,
//     C-shr.c05, C-shres-sprite.c1, C-shres-sprite.c2, C-spr.c01,
//     C-shres-sprite.c15, C-spr.c10, C-shres-sprite.c14, C-spr.c09
//   SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED (1 id, declined) -- C-shres-sprite.c13
//   1+2+2+2+3+9+1 = 20. 1 claim id declined (C-shres-sprite.c13, on
//   SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED): the manual's own row for bit 3
//   prints its verdict already -- "(unimplemented)" -- so there is
//   nothing left at this boundary for a test to require of it.
//
//   Re-grouping note (this correction pass): the 7 ids formerly grouped
//   under SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES that describe the
//   SuperHires/70ns sprite-positioning capability in general terms
//   (C-library.c2, C-shr.c05, C-shres-sprite.c1, C-shres-sprite.c2,
//   C-spr.c01, C-shres-sprite.c15, C-spr.c10) are moved to
//   SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE -- that is the bit the
//   printed table actually assigns the 70ns/SuperHires-gated increment
//   to (row 04, SHSH1); attaching them to bit 0 (SH0, row 00, a plain
//   140ns increment with no resolution qualifier) was the error being
//   corrected here. No id's wording changed, only which assertion cites
//   it. Bits 3/4's own field mnemonics and function (SHSH0/SHSH1, the
//   "(unimplemented)"/"70ns increment" verdicts) are quoted directly from
//   p303.txt in the two assertions below; no id among this package's 20
//   records independently states them (C-spr.c07/c08 in the wider
//   denise_sprites.jsonl reading do, but were not part of this package's
//   declared 20 and are not cited here for that reason -- the quotes
//   stand on the printed page itself, which is the oracle).
//
// CLAIM COVERAGE ADDENDUM (extension pass, 17 further records from
// ch3-sprite-registers.jsonl / ch3-sprite-registers-spec-stage.jsonl,
// routed after the 20 above were already graded): grouped per
// routing.how, onto the SAME assertions above (their expected values,
// verdicts and pre-existing label text unchanged -- only new ids added,
// per the build brief's one allowed change) except for one new assertion
// and two records:
//
//   PROCESSOR-WRITE-REACHES-SPRXPOS (+2 ids) -- APPC-pos.c6,
//     CH4-appC-position.c9
//   SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 (+2 ids) -- APPC-pos.c8,
//     CH4-appC-position.c13
//   SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT (+4 ids) -- APPC-pos.c8,
//     APPC-pos.c12, CH4-appC-position.c14, CH4-appC-position.c20
//   SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING (+3 ids) -- CH4-appC-position.c17,
//     CH4-appC-position.c5, CH4-appC-position.c7
//   SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE (+3 ids) -- APPC-pos.c1,
//     CH4-appC-position.c1, CH4-appC-position.c2
//   SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES (new assertion, 2 ids) --
//     APPC-pos.c11, CH4-appC-position.c19
//   Recorded, not asserted (2 ids, printed beside SPRXCTL-BIT4-SHSH1-
//     70NS-SUPERHIRES-LIVE) -- APPC-pos.c10, APPC2-SPRxCTL.conf2
//
//   2+2+4+3+3+2+2 = 18 id-citations for 17 distinct new ids (APPC-pos.c8
//   is cited twice: its own sentence states both the SPRxPOS-byte half and
//   the SPRxCTL-bit-0 half, so it is cited on both assertions that
//   together establish it, the same way this package's own APPC-pos.c8
//   twin note describes -- CH4-appC-position.c13/c14 split the identical
//   sentence into two separate ids instead, one per half, and each is
//   cited only where its own half is established).
//
//   Bundled-clause check for the two ids initially routed to two
//   assertions each (CH4-appC-position.c5, "jointly control sprite
//   position"; CH4-appC-position.c7, "jointly control sprite features"):
//   routing.how proposed citing c5 on both SPRXPOS-SPRXCTL-JOINTLY-LOAD-
//   BEARING and SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2. Declined the second
//   half of that: SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING is the assertion
//   this package already built for this exact p303 sentence ("The ECS
//   registers SPRxPOS and SPRxCTL work together as position, size and
//   sprite feature control registers.", carrying C-shres-sprite.c4/c5,
//   C-spr.c03 already), and citing c5/c7 there alone matches how "jointly"
//   is used in the source sentence -- the register PAIR takes on the
//   three roles collectively, not that each register does all three
//   individually. Citing c5 on SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 as well
//   would be redundant, not wrong, but adds nothing that assertion's own
//   label doesn't already establish about SPRxPOS's byte step.
//
//   Deviation from routing.how, and why: CH4-appC-position.c13's own
//   restatement ("On ECS, SPRxPOS bits 7 through 0 hold start-horizontal
//   bits SH8 through SH1.") states nothing about SPRxCTL or SH0 -- its
//   routing.how text ("Cite on ... and SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT")
//   appears to be carried over verbatim from its twin APPC-pos.c8's own
//   routing (which legitimately spans both halves, since APPC-pos.c8's
//   own sentence states both). Citing c13 on SPRXCTL-BIT0-SH0-IS-140NS-
//   LOW-BIT would put an id on an assertion that does not test what that
//   id's own restatement says at all -- not a partial-bundle case, a
//   mismatch. c13 is cited on SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 only, its
//   twin CH4-appC-position.c14 (the SPRxCTL/SH0 half, split out
//   separately, matching the sol/opus split this package's own twin notes
//   already describe) carries the SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT
//   citation instead.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id, labelled
// "[control]" so the mutation gate does not require a mutation record
// for it, matching hrm/ch3-collision-registers/'s convention) proves the
// write -> arm -> hpos-sweep -> read-sprdata path this whole file shares
// is alive -- the sprite becomes visible somewhere and invisible
// elsewhere -- before any claim-based assertion depends on it.
//
// RESULTS: at the pre-correction baseline (both misstated assertions
// still in the file), 5 passed, 2 failed. After this correction pass
// (both assertions rewritten to match p303.txt, one splitting in two):
// 6 passed, 1 failed -- see per-assertion RESULT lines below for which
// one is which. One genuine finding remains; it accounts for the one
// failure. Neither assertion was adjusted to hide it, in either
// direction: SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT below is GREEN because
// the RTL's actual (ungated) behaviour is exactly what the corrected
// claim requires, and SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE stays RED
// because it is not.
//
//   RETRACTED (was FINDING 1) -- "SH0 is never gated by resolution" was
//     recorded as an RTL deviation against SPRXCTL-BIT0-SH0-ONLY-IN-
//     SUPERHIRES's old requirement that SH0 be inert outside SuperHires.
//     That requirement was the test's own error (see CORRECTION above),
//     not a property the manual states -- p.303's bit-0 row carries no
//     resolution qualifier. The RTL fact behind it is unchanged and still
//     recorded, now as the basis for a PASSING assertion instead of a
//     failing one: denise_sprites_shifter.v (non-EBR path) computes the
//     sprite's position match purely as `hpos[7:0] == hstart[7:0]`, where
//     hstart[8:1] comes from SPRxPOS and hstart[0] (SH0) comes from
//     SPRxCTL bit 0 -- this comparison does not read `spres`/`shift` at
//     all, so SH0 moves the start by 1 raw unit at every one of the 4
//     spres encodings, including spres=0, matching '00      SH0
//     Start horiz. value Low bit 140 ns increment' (p.303) with no
//     qualifier attached. SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT's own MEASURED
//     note above records the four per-spres deltas (1,1,1,1) that show it.
//   FINDING (was the bit-4 half of FINDING 2) -- SPRxCTL bit 4 is not
//     implemented at all, on any chip mode. The ONLY bits
//     denise_sprites_shifter.v's CTL-register write ever captures are bit
//     7 (attach) and bit 0 (SH0): `{attach,hstart[0]} <=
//     {data_in[7],data_in[0]};` -- nothing in denise_sprites.v or
//     denise_sprites_shifter.v (non-EBR path) ever reads data_in[4] from a
//     CTL write. This is a stronger deviation than "ECS chips only"
//     (p.303's NOTE) describes: bit 4 is not chip-scoped, it is simply
//     absent from this implementation, at aga=0 and (by the same
//     dead-code reading) at aga=1 too.
//   NOT A FINDING (was the bit-3 half of FINDING 2) -- SPRxCTL bit 3 is
//     likewise never read by that same CTL-register write
//     (`{attach,hstart[0]} <= {data_in[7],data_in[0]};` captures only bits
//     7 and 0). Unlike bit 4, this is not a deviation: the manual's own
//     row for bit 3 already reads '03      SHSH0     Start horizontal
//     (SHR mode) 35ns (unimplemented)' (p.303) -- the RTL not implementing
//     an unimplemented field is conformance, not a finding. Declined by
//     SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED below rather than asserted either
//     way.
//   The bit-4 finding is read directly off the RTL cited above (file,
//   line, the exact expression), not inferred from the test's failure
//   alone.
//
// RESULTS ADDENDUM (extension pass): baseline before this pass (measured,
// same RTL, same build) was 6 passed, 1 failed. After adding the new
// SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES assertion and the id additions
// above: 7 passed, 1 failed. The 7 pre-existing PASS/FAIL/MEASURED lines
// are byte-identical to the pre-extension baseline once claim ids are
// stripped -- no expected value, verdict or measurement changed. The new
// assertion is GREEN: SPR0CTL bit 3 set against clear leaves the full
// sprdata trace identical at every one of the 4 spres values, matching
// the manual's own "(unimplemented)" verdict for this bit's row and the
// same {attach,hstart[0]} <= {data_in[7],data_in[0]} write path already
// read for the bit-4 finding above (bit 3 is likewise never captured).
// This is a genuine measurement, not a restatement of the declined
// SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED assertion below -- that one prints the
// manual's row without measuring anything; this one measures and confirms
// it. Two claims are recorded rather than asserted (APPC-pos.c10,
// APPC2-SPRxCTL.conf2), both beside SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-
// LIVE per their own routing.how.
//
// REPAIR PASS (this session, from validation/ch3-b.md's ch3-sprite-
// registers section and its "Outside scope" note): 4 defects on
// backlog-added ids (8 ids across 4 defect lines) and 3 pre-backlog ids
// with the same defects (C-shres-sprite.c5, c7, c8) were repaired.
// Baseline before this pass (measured, same RTL, same build): 7 passed,
// 1 failed. After: 10 passed, 1 failed -- the one finding is unchanged.
// Every one of the four pre-backlog assertions this pass touched
// (PROCESSOR-WRITE-REACHES-SPRXPOS, SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2,
// SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING, SPRXCTL-BIT4-SHSH1-70NS-
// SUPERHIRES-LIVE) kept its verdict, expected value and label text --
// the only change on any of them was removing an id. No id was added to
// an existing pre-backlog assertion; every id that needed to be
// established was moved onto a brand-new assertion instead, or
// withdrawn to a record.
//
//   PROCESSOR-WRITE-REACHES-SPRXPOS-AND-SPRXCTL (new assertion, 3 ids)
//     -- C-shres-sprite.c7, APPC-pos.c6, CH4-appC-position.c9. Removed
//     from PROCESSOR-WRITE-REACHES-SPRXPOS, which drives SPR0POS only
//     and so never established the SPRxCTL half of "they may be loaded
//     by the processor" (p.303); the new assertion drives SPR0POS AND
//     SPR0CTL independently, through fresh measurements, and requires
//     both to move the measured start.
//   SPRXPOS-EACH-BIT-IS-SH8-SH1-WEIGHTED (new assertion, 3 ids) --
//     C-shres-sprite.c8, APPC-pos.c8 (its SPRxPOS-byte half only -- its
//     SPRxCTL/SH0 half stays on SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT,
//     untouched by this pass), CH4-appC-position.c13. Removed from
//     SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2, which moves only bit 0 and so
//     measures only that one bit's own weight, not the eight the claim
//     states ("07-00  SH8-SH1", p.303); the new assertion measures
//     every one of the eight bits' own weight (2^(k+1) raw units each).
//     C-spr.c05, out of this repair's named scope, stays on SPRXPOS-
//     BYTE-IS-SH8-SH1-STEP-OF-2 unchanged.
//   SPRXCTL-HOLDS-POSITION-AND-CONTROL-DATA (new assertion, 1 id) --
//     CH4-appC-position.c17. Removed from SPRXPOS-SPRXCTL-JOINTLY-LOAD-
//     BEARING, whose position half is measured through SPRxPOS, not
//     SPRxCTL, so it never established that SPRxCTL's OWN register-map
//     row ("SPRxCTL W   A D     Sprite x position and control data",
//     p.303) carries position data; the new assertion measures both
//     halves entirely through SPRxCTL writes, SPRxPOS held fixed.
//   C-shres-sprite.c5 [withdrawn, recorded] -- removed from SPRXPOS-
//     SPRXCTL-JOINTLY-LOAD-BEARING. Its whole restatement is the "size"
//     clause this module cannot observe (no vertical-position input) --
//     unlike C-shres-sprite.c4/C-spr.c03, which state clauses this
//     boundary DOES measure and were left in place, c5 has no unbundled
//     half to keep. Withdrawn whole to the record() beside SPRXPOS-
//     SPRXCTL-JOINTLY-LOAD-BEARING below (see Non-goals above).
//   APPC-pos.c1, CH4-appC-position.c1, CH4-appC-position.c2
//     [withdrawn, recorded] -- removed from SPRXCTL-BIT4-SHSH1-70NS-
//     SUPERHIRES-LIVE. Their claim compares positioning granularity
//     across SuperHires/Hires/Lores; this module's position-match
//     comparator does not read `spres` at all (SH0's own delta is 1 raw
//     unit at every spres value, measured above), and the bits that
//     would carry a finer-than-SH0 step within SuperHires are dead code
//     (the FINDING above), so no stimulus at this boundary could show
//     the comparison. Withdrawn whole to the record() beside SPRXCTL-
//     BIT4-SHSH1-70NS-SUPERHIRES-LIVE below.
//
// Every graded PASS/FAIL/MEASURED line carried over from before this
// pass is byte-identical once claim ids are stripped -- verified by
// tools/check_labels_gained_only_ids.py against this pass's own before/
// after capture ("problems: 0"). tools/check_uid_placement.py against
// the after capture reports "packages asserting AND declining one : 0".
// No assertion this pass removed an id from was added by the backlog
// (816bea1~1..HEAD) -- all four predate it -- so no correction-on-
// purpose exception to the first check applies or is claimed.
//
// MUTATION LOG (the first 5 stimulus entries below were run by an
// earlier pass of this agent under WSL/Verilator on 11.09.2026, against
// the then-current 5-passed/2-failed baseline -- edit -> rebuild -> run
// -> confirm the named assertion, and only that assertion, changed colour
// -> revert -> rebuild -> confirm baseline restored -- before moving to
// the next one. nanomig/ was never touched; no expected value, oracle
// text or comparison logic was ever edited, only stimulus. The first
// attempt at the SPRXPOS-BYTE mutation also moved SPRXPOS-SPRXCTL-
// JOINTLY-LOAD-BEARING, which reused that assertion's pos_delta variable
// -- a coupling bug in the harness, not a finding; it was fixed by giving
// JOINTLY-LOAD-BEARING its own independent measurement before the
// mutation below was run for record. A 6th entry, for the new
// SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT assertion, was added by this
// correction pass the same day, after the oracle fixes moved the
// baseline to 6-passed/1-failed (see RESULTS above); it is guarded behind
// its own `#ifndef MUT_BIT0` flag (`-DMUT_BIT0=1` to replay) rather than
// a hand-edited stimulus, per this pass's own brief:
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: measure() was given a fifth, defaulted arm_data parameter (default 0x8000, unchanged for every other call site) and the control block's own call was changed to pass arm_data=0x0000, so only its SPR0DATA write stopped arming sprite 0 -> FAIL ("4 passed, 3 failed"), only [control] FIXTURE-LIVE-DATA-PATH newly red (sprdata stayed 0 for the whole hpos 0..511 sweep, so took_both_values was false). Reverted both the call site and the measure() signature to their original form (no arm_data parameter, arming hardcoded to 0x8000): 5/2, same two findings.
// MUTATION PROCESSOR-WRITE-REACHES-SPRXPOS stimulus red: the second SPR0POS write's value changed from 0x50 to 0x10 (the same as the first), so the two writes stopped differing -> FAIL ("4 passed, 3 failed"), only PROCESSOR-WRITE-REACHES-SPRXPOS newly red (lo.start no longer < hi.start; they are equal). Reverted to 0x50: 5/2, same two findings.
// MUTATION SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 stimulus red: the second SPR0POS write's value changed from 0x41 to 0x42, a byte step of 2 instead of 1 -> FAIL ("4 passed, 3 failed"), only SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 newly red (measured delta became 4, not the asserted 2). Reverted to 0x41: 5/2, same two findings.
// MUTATION SH0-IS-HALF-OF-POS-BYTE-STEP stimulus red: the "hi" measurement's SPR0CTL value changed from 0x01 (SH0) to 0x02 (bit 1, a bit this RTL never stores anywhere) -> FAIL ("4 passed, 3 failed"), only SH0-IS-HALF-OF-POS-BYTE-STEP newly red (measured delta became 0, not the asserted 1). Reverted to 0x01: 5/2, same two findings.
// MUTATION SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING stimulus red: the attached-case SPR1CTL write changed from 0x80 (ATTACH set) to 0x00, so sprite 1 is armed but never actually attached -> FAIL ("4 passed, 3 failed"), only SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING newly red (the attached measurement returned colour 1, the same as the unattached one, instead of 13). Reverted to 0x80: 5/2, same two findings.
// MUTATION SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT stimulus red: built with -DMUT_BIT0=1, which withholds the SH0 stimulus in this assertion's own independent hi0 measurement (SPR0CTL held at 0x00 instead of 0x01, spres=0) -> FAIL ("5 passed, 2 failed"), only SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT newly red (sh0_delta0 became 0, not the asserted 1); SH0-IS-HALF-OF-POS-BYTE-STEP, which shares this block's earlier delta[]/superhires_spres loop but not this flag, stayed green, confirming the isolation held. Rebuilt with no flags: 6/1, the one finding unchanged.
// MUTATION SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE oracle flip green: built with -DMUT_BIT4_ORACLE=1, which changes only the expected relation to shsh1_delta == 0 (the sh0_delta/shsh1_delta measurement is unchanged) -> PASS ("11 passed, 0 failed"), only SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE newly green, still reading sh0_delta=4, shsh1_delta=0 -- the same two numbers as the unmutated run, now satisfying the mutated relation instead of the page's own one. Confirms the comparator and the 28 MHz measurement are live, and that the unmutated run's red comes from the relation stated in the assertion's own label, not from a stuck or unreachable measurement. Rebuilt with no flags: 10/1, the one finding unchanged.
// MUTATION SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED: no record -- this claim is declined (checks.note only, no checks.expect), so there is no red/green state for a mutation to move.
// MUTATION SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES stimulus red (extension pass, run under WSL/Verilator this session against the then-current 7-passed/1-failed baseline): built with -DMUT_BIT3=1, which substitutes bit 0 (SH0, already proven live above) for bit 3 in this assertion's own per-spres toggle, so the "identical trace" comparator has a live bit to catch -> FAIL ("6 passed, 2 failed"), only SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES newly red (the off/on traces differed at every spres once bit 0 was the one toggled); every other line, including SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE (already red, unrelated), was unchanged. Rebuilt with `make -B test` (no flags): 7/1, the one finding unchanged.
// MUTATION APPC-pos.c10, APPC2-SPRxCTL.conf2: no record -- both are recorded, not asserted (checks.record only, no checks.expect carrying either id), so there is no red/green state of their own for a mutation to move; the assertion they are recorded beside, SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE, already carries its own mutation record above (none, impossible -- already red at baseline).
// MUTATION PROCESSOR-WRITE-REACHES-SPRXPOS-AND-SPRXCTL stimulus red (repair pass, run under WSL/Verilator this session against the then-current 10-passed/1-failed baseline): built with -DMUT_PROC_CTL=1, which withholds the SPR0CTL stimulus in this assertion's own independent ctl_hi measurement (held at 0x00 instead of 0x01) -> FAIL ("9 passed, 2 failed"), only PROCESSOR-WRITE-REACHES-SPRXPOS-AND-SPRXCTL newly red (ctl_reached became false; pos_reached, measured independently, stayed true). Rebuilt with no flags: 10/1, the one finding unchanged.
// MUTATION SPRXPOS-EACH-BIT-IS-SH8-SH1-WEIGHTED stimulus red (repair pass, run under WSL/Verilator this session against the then-current 10-passed/1-failed baseline): built with -DMUT_POSBITS=1, which at k=5 substitutes bit 4's own stimulus for bit 5's -> FAIL ("9 passed, 2 failed"), only SPRXPOS-EACH-BIT-IS-SH8-SH1-WEIGHTED newly red (deltas[5] read 32, the weight of bit 4, not the asserted 64). Rebuilt with no flags: 10/1, the one finding unchanged; the per-bit MEASURED note above the assertion confirms the clean deltas 2,4,8,16,32,64,128,256 at baseline.
// MUTATION SPRXCTL-HOLDS-POSITION-AND-CONTROL-DATA stimulus red (repair pass, run under WSL/Verilator this session against the then-current 10-passed/1-failed baseline): built with -DMUT_CTL_ATTACH=1, which withholds the ATTACH stimulus in this assertion's own independent "attached" measurement (SPR1CTL held at 0x00 instead of 0x80) -> FAIL ("9 passed, 2 failed"), only SPRXCTL-HOLDS-POSITION-AND-CONTROL-DATA newly red (attached read 1, the same as plain, not the asserted 13; position_via_ctl, measured independently, stayed true). Rebuilt with no flags: 10/1, the one finding unchanged.
// MUTATION SH0-IS-HALF-OF-POS-BYTE-STEP stimulus red (at spres=0b10/SuperHires): built with -DMUT_SH0HALF=1, which withholds the SH0 stimulus in this assertion's own hi measurement (ctl held at 0x00 instead of 0x01) -> FAIL ("9 passed, 2 failed"), only SH0-IS-HALF-OF-POS-BYTE-STEP newly red (sh0_delta_at_shres became 0, not the asserted 1); SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE (already red, unrelated) was unchanged. Rebuilt with no flags: 10/1, the one finding unchanged.

// SPECIFICATION ADDENDUM (extension pass, from ch3-sprite-registers-
// spec-stage.jsonl's 17 records -- quotation, restatement and page only;
// this block was written and committed before ch3-sprite-registers.jsonl
// or the RTL was opened again, per the build brief's gate for extending
// an accepted package). Each new claim id is grouped under the one manual
// proposition it restates; grouping is by wording alone at this stage,
// not by which existing assertion (if any) already measures it -- that
// mapping is decided, and recorded in CLAIM COVERAGE above and RESULTS
// below, only after this block was committed.
//
//   [P1] SUPERHIRES-FINER-GRANULARITY-THAN-HIRES-AND-LORES (p.303) --
//     "SuperHires mode has a finer granularity of sprite positioning than
//     either Hires or Lores modes." Three readings of the same sentence:
//     APPC-pos.c1 keeps both comparators ("either Hires or Lores");
//     CH4-appC-position.c1 restates the Hires half alone;
//     CH4-appC-position.c2 restates the Lores half alone.
//
//   [P2] SPRXCTL-BIT-TABLE-ROW-04-SHSH1 (p.303) -- "04      SHSH1
//     Start horizontal (SHR mode) 70ns increment" -- SPRxCTL bit 4 is
//     SHSH1, a 70ns SuperHires horizontal-start increment. APPC-pos.c10.
//     source_defines records tension with chapter 4 pp.107/128 and
//     appendix A p.284, which call the same bit range unused/reserved
//     without naming a chipset -- the existing NM-HRM-019 regression.
//
//   [P3] SPRXCTL-BIT-TABLE-ROW-03-SHSH0 (p.303) -- "03      SHSH0
//     Start horizontal (SHR mode) 35ns (unimplemented)" -- SPRxCTL bit 3
//     is SHSH0, a 35ns SuperHires increment the manual's own row marks
//     unimplemented. APPC-pos.c11, CH4-appC-position.c19.
//
//   [P4] SPRXCTL-BIT-TABLE-ROW-00-SH0 (p.303) -- "00      SH0
//     Start horiz. value Low bit 140 ns increment" -- SPRxCTL bit 0 is
//     SH0, a 140ns increment, the low bit of the horizontal start value.
//     APPC-pos.c12, CH4-appC-position.c20.
//
//   [P5] SPRXPOS-SPRXCTL-PROCESSOR-LOADABLE (p.303) -- "They are usually
//     loaded by the sprite DMA channel, during horizontal blank, however
//     they may be loaded by the processor." SPRxPOS and SPRxCTL may be
//     loaded by the processor, not only by DMA. APPC-pos.c6,
//     CH4-appC-position.c9.
//
//   [P6] SPRXPOS-BIT-TABLE-ROW-07-00-SH8-SH1 (p.303) -- "07-00  SH8-SH1
//     Start horizontal value. Low bit (SH0) in SPRxCTL." SPRxPOS bits
//     07-00 are SH8-SH1, the upper 8 bits of the 9-bit horizontal start
//     value; its low bit (SH0) lives in SPRxCTL. APPC-pos.c8,
//     CH4-appC-position.c13, CH4-appC-position.c14.
//
//   [P7] SPRXCTL-REGISTER-MAP-ROW-POSITION-AND-CONTROL (p.303) --
//     "SPRxCTL W   A D     Sprite x position and control data" --
//     SPRxCTL's own register-map row describes it as holding sprite
//     position and control data. CH4-appC-position.c17.
//
//   [P8] SPRXPOS-SPRXCTL-JOINT-POSITION-AND-FEATURES (p.303) -- "The ECS
//     registers SPRxPOS and SPRxCTL work together as position, size and
//     sprite feature control registers." CH4-appC-position.c5 restates
//     the position half, CH4-appC-position.c7 the feature half.
//
//   [P9] SPRXCTL-CHIPSET-SCOPING-CONFLICT (p.298, vs pp.107/128/284) --
//     "142  SPRxCTL  chg   W  A   Sprite x position and control data" --
//     the appendix B register-summary map marks SPRxCTL "chg" (changed
//     between chipsets), which appendix C p.303 explains as bits 3 and 4
//     being ECS-only, against chapter 4 pp.107/128 and appendix A p.284,
//     which call the same bits unused/reserved without naming a chipset.
//     APPC2-SPRxCTL.conf2. Recorded as a conflict, not resolved, by the
//     classifier.
//
//   17 ids total: 3+1+2+2+2+3+1+2+1 = 17.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_sprites.h"

// MUT_BIT0: replay stimulus for SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT's own
// mutation record above (-DMUT_BIT0=1). Withholds the SH0 toggle in that
// assertion's independent hi0 measurement so the assertion, and only it,
// goes red.
#ifndef MUT_BIT0
#define MUT_BIT0 0
#endif

// MUT_BIT3: replay stimulus for SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES's
// own mutation record below (-DMUT_BIT3=1). Substitutes bit 0 (SH0,
// already proven live) for bit 3 in that assertion's own toggle, so the
// "identical trace" comparator has something it should catch.
#ifndef MUT_BIT3
#define MUT_BIT3 0
#endif

// MUT_PROC_CTL: replay stimulus for PROCESSOR-WRITE-REACHES-SPRXPOS-AND-
// SPRXCTL's own mutation record below (-DMUT_PROC_CTL=1). Withholds the
// SPR0CTL stimulus in that assertion's own independent ctl_hi
// measurement, so only its SPRxCTL half goes red.
#ifndef MUT_PROC_CTL
#define MUT_PROC_CTL 0
#endif

// MUT_POSBITS: replay stimulus for SPRXPOS-EACH-BIT-IS-SH8-SH1-WEIGHTED's
// own mutation record below (-DMUT_POSBITS=1). At k=5, substitutes bit
// 4's stimulus for bit 5's own, so the per-bit weight check has a wrong
// delta to catch at exactly one index.
#ifndef MUT_POSBITS
#define MUT_POSBITS 0
#endif

// MUT_CTL_ATTACH: replay stimulus for SPRXCTL-HOLDS-POSITION-AND-CONTROL-
// DATA's own mutation record below (-DMUT_CTL_ATTACH=1). Withholds the
// ATTACH stimulus in that assertion's own independent "attached"
// measurement, so only its control half goes red.
#ifndef MUT_CTL_ATTACH
#define MUT_CTL_ATTACH 0
#endif

// MUT_SH0HALF: replay stimulus for SH0-IS-HALF-OF-POS-BYTE-STEP's own
// mutation record below (-DMUT_SH0HALF=1). Withholds the SH0 toggle in
// that assertion's own hi measurement (ctl held at 0x00 instead of
// 0x01), so the assertion, and only it, goes red.
#ifndef MUT_SH0HALF
#define MUT_SH0HALF 0
#endif

// MUT_BIT4_ORACLE: replay for SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE's
// own mutation record below (-DMUT_BIT4_ORACLE=1). Changes only the
// expected relation between the two measured deltas, to shsh1_delta ==
// 0 instead of shsh1_delta == sh0_delta/2 -- the measurement itself is
// unchanged.
#ifndef MUT_BIT4_ORACLE
#define MUT_BIT4_ORACLE 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records that state what the SOURCE says, or why a claim is declined.
    // They are not measurements and must not be printed as though they were:
    // a constant string under a MEASURED label is a record no run can
    // contradict, which is how a false "measured" fact survived a day in
    // ch3-sprite-playfield-priority.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map, reused from hrm/denise_sprites/
// test_denise_sprites.cpp for this same module: SPRPOSCTLBASE is byte
// address 0x140 (PACKAGE.md's own evidence field), so word address
// (reg_address_in) is 0x140>>1 = 0xA0 for SPR0POS; each sprite occupies
// 4 consecutive word addresses (POS, CTL, DATA, DATB), 8 sprites total.
constexpr int SPR0POS = 0xA0;
constexpr int SPR0CTL = 0xA1;
constexpr int SPR0DATA = 0xA2;
constexpr int SPR0DATB = 0xA3;
constexpr int SPR1POS = 0xA4;
constexpr int SPR1CTL = 0xA5;
constexpr int SPR1DATA = 0xA6;
constexpr int SPR1DATB = 0xA7;
constexpr int IDLE_ADDR = 0x00;

constexpr int HPOS_MAX = 512;  // SH8..SH0 is a 9-bit value: 0..511 covers it.

// This fixture's own `spres` encoding for SuperHires. Distinct from
// `superhires_spres` below, which is not a SuperHires encoding -- see
// that block's own comment.
constexpr uint8_t SPRES_SUPERHIRES = 0b10;

struct Meas {
    int start = -1;                 // first hpos at which sprdata != 0, or -1.
    std::vector<uint8_t> trace;      // sprdata at every swept hpos.
};

// One isolated measurement: fresh DUT, reset, arm sprite 0 (and,
// optionally, sprite 1 for the ATTACH case) with a known image, sweep
// hpos, record when/what becomes visible. Clocking (clk7_en/c1/c3
// four-phase tick, reg_address_in/data_in write sequencing) and the
// DATB=0/DATA=0x8000 arm-on-write convention are reused from the sibling
// harness for this same module (see ORDER OF WORK above), not learned
// from this package's own denise_sprites.v.
class Harness {
public:
    Harness(uint8_t aga, uint8_t spres) : dut(new Vdenise_sprites) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->aga = aga;
        dut->spres = spres;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->hpos = 0;
        dut->sprena = 1;
        dut->esprm = 0;
        dut->osprm = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // c1=0,c3=0 -> shift/load; c1=1,c3=0; c1=1,c3=1; c1=0,c3=1.
    void tick_phase() {
        phase = (phase + 1) % 4;
        switch (phase) {
            case 0: dut->c1 = 0; dut->c3 = 0; dut->clk7_en = 1; break;
            case 1: dut->c1 = 1; dut->c3 = 0; dut->clk7_en = 0; break;
            case 2: dut->c1 = 1; dut->c3 = 1; dut->clk7_en = 0; break;
            case 3: dut->c1 = 0; dut->c3 = 1; dut->clk7_en = 0; break;
        }
        tick();
    }

    void tick_7mhz() {
        tick_phase();
        tick_phase();
        tick_phase();
        tick_phase();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick_7mhz();
    }

    Vdenise_sprites* dut;
    int phase = 0;
};

// Arms sprite 0 at the given SPRxPOS byte / SPRxCTL byte and sweeps hpos,
// recording the resulting sprdata trace and first-active hpos.
Meas measure(uint8_t pos_byte, uint8_t ctl_byte, uint8_t spres, uint8_t aga) {
    Harness h(aga, spres);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, ctl_byte);
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, 0x8000);  // arms sprite 0.

    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        uint8_t v = static_cast<uint8_t>(h.dut->sprdata);
        m.trace.push_back(v);
        if (v != 0 && m.start == -1) m.start = hp;
    }
    return m;
}

// Arms sprite 0 exactly as measure() above (SPR0POS=$40, SPR0DATB=$0000,
// SPR0DATA=$8000) at spres=SPRES_SUPERHIRES, aga=0, on a fresh DUT, then
// writes SPR0CTL=ctl_byte and samples `sprdata` on every 28 MHz clock
// (tick_phase(), 35 ns apart), not once per 140 ns hpos step as
// measure() above does. Returns the 28 MHz sample index (0-based, 35 ns
// apart) at which sprdata first goes non-zero across the full hpos
// 0..511 sweep, or -1 if it never does.
int measure_superhires_start(uint8_t ctl_byte) {
    Harness h(/*aga=*/0, SPRES_SUPERHIRES);
    h.write_reg(SPR0POS, 0x40);
    h.write_reg(SPR0CTL, ctl_byte);
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, 0x8000);  // arms sprite 0.

    int sample = 0;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        for (int ph = 0; ph < 4; ++ph) {
            h.tick_phase();
            uint8_t v = static_cast<uint8_t>(h.dut->sprdata);
            if (v != 0) return sample;
            ++sample;
        }
    }
    return -1;
}

// Arms sprite 0 alone (attach=false), or sprite 0 together with sprite 1
// at the SAME start position with SPR1CTL bit 7 (ATTACH) set (attach=
// true), and returns the first non-zero colour index seen. Reuses the
// sibling harness's own already-validated recipe (SPR1 DATB=DATA=0x8000
// under attach), not derived from this package's own RTL.
uint8_t measure_attach_color(uint8_t pos_byte, bool attach) {
    Harness h(/*aga=*/0, /*spres=*/0);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, 0x00);
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, 0x8000);
    if (attach) {
        h.write_reg(SPR1POS, pos_byte);
        h.write_reg(SPR1CTL, 0x80);
        h.write_reg(SPR1DATB, 0x8000);
        h.write_reg(SPR1DATA, 0x8000);
    }
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        uint8_t v = static_cast<uint8_t>(h.dut->sprdata);
        if (v != 0) return v;
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- FIXTURE-LIVE-DATA-PATH [control] ----------------
    {
        Meas m = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool took_both_values = m.start != -1;
        bool went_quiet_again = false;
        if (m.start != -1) {
            for (size_t i = static_cast<size_t>(m.start) + 1; i < m.trace.size(); ++i) {
                if (m.trace[i] == 0) { went_quiet_again = true; break; }
            }
        }
        checks.expect(took_both_values && went_quiet_again,
            "[control] FIXTURE-LIVE-DATA-PATH: arming sprite 0 (SPR0POS=0x40, "
            "SPR0CTL=0x00, SPR0DATB=0x0000, SPR0DATA=0x8000) and sweeping hpos "
            "0..511 makes sprdata take both a non-zero value and, afterwards, "
            "zero again -- not a single stuck value -- proving the write -> "
            "arm -> hpos-sweep -> read-sprdata measurement basis every "
            "assertion below shares is actually live before any claim-based "
            "assertion depends on it. Fixture check, no claim id, no page.");
    }

    // ---------------- PROCESSOR-WRITE-REACHES-SPRXPOS ----------------
    {
        Meas lo = measure(/*pos=*/0x10, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas hi = measure(/*pos=*/0x50, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool both_armed = lo.start != -1 && hi.start != -1;
        bool monotonic = both_armed && hi.start > lo.start;
        checks.expect(monotonic,
            "PROCESSOR-WRITE-REACHES-SPRXPOS: writing two different values "
            "(0x10, then separately 0x50) to SPR0POS through reg_address_in/"
            "data_in -- this module's only register-write interface, the "
            "same one every write in this file uses, with no separate "
            "DMA-tagged path to distinguish a processor write from a "
            "DMA-sourced one -- moves the sprite's measured start position "
            "forward as the written value increases, showing a value placed "
            "on this interface reaches SPRxPOS and takes effect: 'They are "
            "usually loaded by the sprite DMA channel, during horizontal "
            "blank, however they may be loaded by the processor.' (p.303). "
            ".");
    }

    // ------ PROCESSOR-WRITE-REACHES-SPRXPOS-AND-SPRXCTL (new) ------
    // New for this repair pass. The claim above ("They are usually loaded
    // by the sprite DMA channel ... however they may be loaded by the
    // processor.", p.303) names SPRxPOS AND SPRxCTL together, but
    // PROCESSOR-WRITE-REACHES-SPRXPOS only drives SPR0POS -- it does not
    // by itself show a processor write reaches SPRxCTL too. Independent
    // measurements here (fresh Harness per call, no shared state with the
    // block above) so this assertion's own mutation cannot move that one
    // or vice versa.
    {
        Meas pos_lo = measure(/*pos=*/0x10, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas pos_hi = measure(/*pos=*/0x50, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas ctl_lo = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
#if MUT_PROC_CTL
        Meas ctl_hi = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);  // MUTATION: SPR0CTL stimulus withheld (held at 0x00 instead of 0x01).
#else
        Meas ctl_hi = measure(/*pos=*/0x40, /*ctl=*/0x01, /*spres=*/0, /*aga=*/0);
#endif
        bool pos_reached = pos_lo.start != -1 && pos_hi.start != -1 && pos_hi.start != pos_lo.start;
        bool ctl_reached = ctl_lo.start != -1 && ctl_hi.start != -1 && ctl_hi.start != ctl_lo.start;
        checks.expect(pos_reached && ctl_reached,
            "PROCESSOR-WRITE-REACHES-SPRXPOS-AND-SPRXCTL: writing distinct "
            "values to SPR0POS (0x10 vs 0x50) through reg_address_in/"
            "data_in changes the measured start position, and, "
            "independently, writing distinct values to SPR0CTL (0x00 vs "
            "0x01, bit 0) through the same interface also changes it -- "
            "this module's only register-write interface, with no "
            "separate DMA-tagged path to distinguish a processor write "
            "from a DMA-sourced one -- so a value placed on this "
            "interface reaches BOTH SPRxPOS and SPRxCTL and takes effect: "
            "'They are usually loaded by the sprite DMA channel, during "
            "horizontal blank, however they may be loaded by the "
            "processor.' (p.303). C-shres-sprite.c7, APPC-pos.c6, "
            "CH4-appC-position.c9 .");
    }

    // ---------------- SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 ----------------
    int pos_delta = 0;
    {
        Meas a = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas b = measure(/*pos=*/0x41, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool both_armed = a.start != -1 && b.start != -1;
        pos_delta = both_armed ? (b.start - a.start) : -1000;
        checks.expect(both_armed && pos_delta == 2,
            "SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2: incrementing SPR0POS's byte "
            "by 1 (0x40 -> 0x41, SPR0CTL held at 0) moves the measured start "
            "position by exactly 2 raw hpos units, not 1 -- because SPRxPOS "
            "holds the UPPER 8 bits of the 9-bit start value (SH8-SH1), so "
            "its own least-significant bit is worth 2 raw units, with the "
            "true bit-0 (SH0) living elsewhere: "
            "'07-00  SH8-SH1  Start horizontal value. Low bit (SH0) in "
            "SPRxCTL.' (p.303). C-spr.c05 "
            ".");
    }

    // ------------- SPRXPOS-EACH-BIT-IS-SH8-SH1-WEIGHTED (new) -------------
    // New for this repair pass. SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 above
    // moves only SPR0POS's bit 0 (0x40 -> 0x41) and so measures only that
    // one bit's own weight (SH1, worth 2 raw units) -- the claim
    // "07-00  SH8-SH1  Start horizontal value. Low bit (SH0) in SPRxCTL."
    // (p.303) is about all eight bits. Independent per-bit measurement:
    // base = SPR0POS carrying a fixed anchor bit (SPR0CTL held at 0),
    // toggled = the same anchor plus bit k; delta must be 2^(k+1) at
    // every k, matching SPRxPOS's role as the upper 8 bits (SH8-SH1) of
    // the 9-bit start value whose true bit 0 (SH0) lives in SPRxCTL, not
    // SPRxPOS.
    {
        // base/toggled both carry a fixed ANCHOR bit (a bit other than k)
        // so hstart is never 0 in either measurement: with hstart==0 and
        // this Harness's hpos held at its post-reset value (0) for the
        // whole write_reg() sequence that PRECEDES the sweep, the POS/CTL/
        // DATA writes themselves satisfy the position-match comparator a
        // beat early, and "start" reads a fixture artifact (measured: a
        // uniform +1 on every delta, k=0..7, not a per-bit RTL fact).
        // Anchor eliminates the collision; base and toggled differ by bit
        // k alone, same as SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2's 0x40/0x41
        // pair above.
        bool all_weighted = true;
        int deltas[8];
        for (int k = 0; k < 8; ++k) {
            int anchor = (k + 1) % 8;
            uint8_t base_byte = static_cast<uint8_t>(1u << anchor);
#if MUT_POSBITS
            uint8_t bit_k = static_cast<uint8_t>(1u << (k == 5 ? 4 : k));  // MUTATION: at k=5, bit 4's stimulus substituted for bit 5's own.
#else
            uint8_t bit_k = static_cast<uint8_t>(1u << k);
#endif
            Meas base = measure(base_byte, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
            Meas toggled = measure(static_cast<uint8_t>(base_byte | bit_k), /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
            bool armed = base.start != -1 && toggled.start != -1;
            deltas[k] = armed ? (toggled.start - base.start) : -1000;
            if (!armed || deltas[k] != (1 << (k + 1))) all_weighted = false;
        }
        checks.note("SPRXPOS-EACH-BIT-IS-SH8-SH1-WEIGHTED: per-bit deltas "
            "d0=" + std::to_string(deltas[0]) + " d1=" + std::to_string(deltas[1]) +
            " d2=" + std::to_string(deltas[2]) + " d3=" + std::to_string(deltas[3]) +
            " d4=" + std::to_string(deltas[4]) + " d5=" + std::to_string(deltas[5]) +
            " d6=" + std::to_string(deltas[6]) + " d7=" + std::to_string(deltas[7]) +
            " (expected 2,4,8,16,32,64,128,256).");
        checks.expect(all_weighted,
            "SPRXPOS-EACH-BIT-IS-SH8-SH1-WEIGHTED: adding SPR0POS bit k "
            "alone to a fixed anchor bit (SPR0CTL held at 0), for every k "
            "0..7, moves the measured start position by exactly 2^(k+1) "
            "raw hpos units (2,4,8,16,32,64,128,256) -- matching SPRxPOS's "
            "own row, where bits 07-00 are the UPPER 8 bits (SH8-SH1) of "
            "the 9-bit start value: "
            "'07-00  SH8-SH1  Start horizontal value. Low bit (SH0) in "
            "SPRxCTL.' (p.303). C-shres-sprite.c8, APPC-pos.c8, "
            "CH4-appC-position.c13 .");
    }

    // ---------------- SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT ----------------
    // Replaces SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES (see CORRECTION above):
    // the manual's own bit-0 row carries no resolution qualifier, so this
    // assertion requires only what that row states -- SH0 is the 140ns low
    // bit of the horizontal start value -- with no SuperHires gate.
    int superhires_spres = -1;
    {
        // sh0_delta(spres) = start(SH0=1) - start(SH0=0), SPR0POS fixed at
        // 0x40, for each of the 4 values spres (a 2-bit input, per
        // PACKAGE.md's own evidence "spres = bplcon3[7:6] & aga") can take.
        // Unchanged from the predecessor assertion and NOT gated by
        // MUT_BIT0 below. `superhires_spres` itself is NOT a SuperHires
        // encoding: SH0 moves the start at every spres value, so the loop
        // below, which starts searching at sp=1, always lands on sp=1
        // regardless of chip mode. Kept here as the source for the
        // per-spres MEASURED note just below. SH0-IS-HALF-OF-POS-BYTE-STEP
        // measures its own SuperHires spres value directly, at
        // SPRES_SUPERHIRES, not this variable.
        int delta[4];
        bool armed[4];
        for (int sp = 0; sp < 4; ++sp) {
            Meas lo = measure(/*pos=*/0x40, /*ctl=*/0x00, static_cast<uint8_t>(sp), /*aga=*/0);
            Meas hi = measure(/*pos=*/0x40, /*ctl=*/0x01, static_cast<uint8_t>(sp), /*aga=*/0);
            armed[sp] = lo.start != -1 && hi.start != -1;
            delta[sp] = armed[sp] ? (hi.start - lo.start) : -1000;
        }
        int found = -1;
        for (int sp = 1; sp < 4 && found == -1; ++sp) {
            if (armed[sp] && delta[sp] != 0) found = sp;
        }
        superhires_spres = found;
        checks.note("SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT: per-spres SH0 "
            "deltas, measured for SH0-IS-HALF-OF-POS-BYTE-STEP's own use "
            "below (spres0=" + std::to_string(delta[0]) +
            " spres1=" + std::to_string(delta[1]) + " spres2=" + std::to_string(delta[2]) +
            " spres3=" + std::to_string(delta[3]) + ") -- SH0 is live at "
            "every one, not only at a SuperHires-like encoding; this "
            "assertion no longer claims otherwise (see CORRECTION above).");

        // Independent measurement for THIS assertion's own check, at
        // spres=0 (this module's own OCS/default baseline: "dut->spres =
        // 0; // OCS sprites" in the sibling harness for this same module)
        // only -- kept apart from the delta[]/superhires_spres computation
        // above, the same isolation SH0-IS-HALF-OF-POS-BYTE-STEP and
        // SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING already use for the same
        // reason, so the MUT_BIT0 mutation below cannot silently move
        // SH0-IS-HALF-OF-POS-BYTE-STEP's own result too.
        Meas lo0 = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
#if MUT_BIT0
        Meas hi0 = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);  // MUTATION: SH0 stimulus withheld (ctl held at 0x00 instead of 0x01).
#else
        Meas hi0 = measure(/*pos=*/0x40, /*ctl=*/0x01, /*spres=*/0, /*aga=*/0);
#endif
        bool both_armed0 = lo0.start != -1 && hi0.start != -1;
        int sh0_delta0 = both_armed0 ? (hi0.start - lo0.start) : -1000;
        checks.expect(both_armed0 && sh0_delta0 == 1,
            "SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT: with SPR0POS fixed at "
            "0x40 and spres=0 (this module's own OCS/default baseline -- "
            "no SuperHires resolution selected), toggling SPR0CTL bit 0 "
            "alone moves the measured start position by exactly 1 raw "
            "unit, matching the bit's own row, which carries no "
            "resolution qualifier: "
            "'00      SH0       Start horiz. value Low bit 140 ns "
            "increment' (p.303); read together with the SPRxPOS row that "
            "names SH0 as the missing low bit of the same 9-bit value: "
            "'07-00  SH8-SH1  Start horizontal value. Low bit (SH0) in "
            "SPRxCTL.' (p.303). The page's SuperHires/70ns statements "
            "describe the FURTHER increment bits the SAME table places at "
            "bits 4 and 3 (SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE below), "
            "not this one -- SH0's own row carries no such qualifier, and "
            "this assertion requires none. C-shres-sprite.c12, "
            "C-shres-sprite.c9, APPC-pos.c8, APPC-pos.c12, "
            "CH4-appC-position.c14, CH4-appC-position.c20 .");
    }

    // ---------------- SH0-IS-HALF-OF-POS-BYTE-STEP ----------------
    // This assertion's own label names SuperHires. `superhires_spres`
    // above is not a SuperHires encoding -- SH0 moves the start at every
    // spres value, so that loop always lands on sp=1 regardless of chip
    // mode. This body measures at spres=SPRES_SUPERHIRES (0b10) directly,
    // so the label and the spres value it runs at agree.
    {
        Meas lo = measure(/*pos=*/0x40, /*ctl=*/0x00, SPRES_SUPERHIRES, /*aga=*/0);
#if MUT_SH0HALF
        Meas hi = measure(/*pos=*/0x40, /*ctl=*/0x00, SPRES_SUPERHIRES, /*aga=*/0);  // MUTATION: SH0 stimulus withheld (ctl held at 0x00 instead of 0x01).
#else
        Meas hi = measure(/*pos=*/0x40, /*ctl=*/0x01, SPRES_SUPERHIRES, /*aga=*/0);
#endif
        bool ok = false;
        int sh0_delta_at_shres = -1000;
        if (lo.start != -1 && hi.start != -1) {
            sh0_delta_at_shres = hi.start - lo.start;
            // Fixed, quote-derived target (2 / 2 == 1), independent of
            // whatever SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2 measured above,
            // so this assertion cannot pass merely by chaining a
            // possibly-wrong runtime value into itself.
            ok = sh0_delta_at_shres == 1;
        }
        checks.expect(ok,
            "SH0-IS-HALF-OF-POS-BYTE-STEP: at spres=0b10 (SuperHires), "
            "toggling SPR0CTL bit 0 (SH0) alone moves the measured start "
            "by exactly 1 raw unit -- half of the 2-unit step a +1 change "
            "to SPRxPOS's own byte produces -- i.e. SH0 lands the sprite "
            "on the position exactly between two SPRxPOS-addressable "
            "points, matching 'This allows for positioning the sprite "
            "every other SuperHires pixel on 70ns boundaries.' (p.303). "
            "C-shres-sprite.c3, C-spr.c02 .");
    }

    // ---------------- SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING ----------------
    {
        // Measured independently here (not reusing the outer pos_delta) so
        // this assertion's own result cannot be moved by a mutation aimed at
        // SPRXPOS-BYTE-IS-SH8-SH1-STEP-OF-2's stimulus above.
        Meas pa = measure(/*pos=*/0x20, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas pb = measure(/*pos=*/0x30, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        bool position_side = pa.start != -1 && pb.start != -1 && pb.start != pa.start;
        uint8_t plain = measure_attach_color(/*pos=*/0x40, /*attach=*/false);
        uint8_t attached = measure_attach_color(/*pos=*/0x40, /*attach=*/true);
        bool feature_side = plain == 1 && attached == 13 && plain != attached;
        checks.expect(position_side && feature_side,
            "SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING: SPRxPOS alone changes "
            "the measured output (its own byte moves the start position, "
            "shown above), and, independently, SPRxCTL alone -- its bit 7 "
            "(ATTACH), with SPR0POS/SPR1POS held identical -- also changes "
            "the measured output (colour index 1 when sprite 1 is not "
            "attached, 13 when it is) -- neither register alone accounts "
            "for the whole result, as: 'The ECS registers SPRxPOS and "
            "SPRxCTL work together as position, size and sprite feature "
            "control registers.' (p.303). This boundary exposes no "
            "vertical-position input (only hpos), so only the position and "
            "feature-control components are measured here; the size "
            "(vertical extent) component of this claim is outside what can "
            "be observed at this boundary -- see Non-goals above. "
            "C-shres-sprite.c4, C-spr.c03, "
            "CH4-appC-position.c5, "
            "CH4-appC-position.c7 .");
    }

    // -------- C-shres-sprite.c5 [withdrawn, recorded] --------
    // Withdrawn from SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING above (this
    // repair pass): its own restatement is only the "size" third of the
    // same p.303 sentence, and, unlike C-shres-sprite.c4/C-spr.c03 (which
    // state position/feature clauses this boundary DOES measure, and stay
    // above), c5's whole content is the clause this boundary cannot reach
    // at all -- see Non-goals above. Withdrawn whole, not kept with a
    // caveat.
    checks.record(
        "C-shres-sprite.c5 [withdrawn from SPRXPOS-SPRXCTL-JOINTLY-LOAD-"
        "BEARING]: 'ECS SPRxPOS and SPRxCTL jointly control sprite size.' "
        "-- restates only the 'size' third of 'The ECS registers SPRxPOS "
        "and SPRxCTL work together as position, size and sprite feature "
        "control registers.' (p.303). This module's own port list carries "
        "no vertical-position input at all (only hpos) -- see Non-goals "
        "above -- so 'size' (vertical extent, VSTART/VSTOP) cannot be "
        "observed by any stimulus at this boundary. Withdrawn whole.");

    // ------- SPRXCTL-HOLDS-POSITION-AND-CONTROL-DATA (new) -------
    // New for this repair pass. CH4-appC-position.c17 restates SPRxCTL's
    // OWN register-map row -- 'SPRxCTL W   A D     Sprite x position and
    // control data' (p.303) -- a claim about SPRxCTL alone, not the
    // SPRxPOS/SPRxCTL pair. SPRXPOS-SPRXCTL-JOINTLY-LOAD-BEARING above
    // shows SPRxCTL's control half (ATTACH) but measures the position
    // half through SPRxPOS, not SPRxCTL. Independent measurement here,
    // entirely through SPRxCTL writes (SPRxPOS held fixed): bit 0 (SH0)
    // alone moves the measured start position (SPRxCTL carries position
    // data), and bit 7 (ATTACH) alone changes the measured colour
    // (SPRxCTL carries control data) -- distinct stimulus values (0x60,
    // not 0x40) from the assertions above, so this one's own mutation
    // cannot move theirs or vice versa.
    {
        Meas ctl_pos_lo = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas ctl_pos_hi = measure(/*pos=*/0x40, /*ctl=*/0x01, /*spres=*/0, /*aga=*/0);
        bool position_via_ctl = ctl_pos_lo.start != -1 && ctl_pos_hi.start != -1
                              && ctl_pos_hi.start != ctl_pos_lo.start;
        uint8_t plain = measure_attach_color(/*pos=*/0x60, /*attach=*/false);
#if MUT_CTL_ATTACH
        uint8_t attached = measure_attach_color(/*pos=*/0x60, /*attach=*/false);  // MUTATION: ATTACH stimulus withheld.
#else
        uint8_t attached = measure_attach_color(/*pos=*/0x60, /*attach=*/true);
#endif
        bool control_via_ctl = plain == 1 && attached == 13 && plain != attached;
        checks.expect(position_via_ctl && control_via_ctl,
            "SPRXCTL-HOLDS-POSITION-AND-CONTROL-DATA: through SPRxCTL "
            "writes alone (SPRxPOS held fixed at 0x40 for the position "
            "half, 0x60 for the control half), toggling bit 0 (SH0) "
            "moves the measured start position, and, independently, "
            "toggling bit 7 (ATTACH) changes the measured colour (1 "
            "unattached, 13 attached) -- SPRxCTL itself carries both "
            "position and control data, matching its own register-map "
            "row: 'SPRxCTL W   A D     Sprite x position and control "
            "data' (p.303). CH4-appC-position.c17 .");
    }

    // ---------------- SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE ----------------
    // Replaces (its bit-4 half of) SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS (see
    // CORRECTION above). Bit 3's half of that old assertion is declined
    // just below instead, not asserted -- the manual prints bit 3's own
    // row's verdict already, "(unimplemented)".
    //
    // Measures at spres=SPRES_SUPERHIRES (0b10), sampling sprdata on
    // every 28 MHz clock (35 ns apart, not once per 140 ns hpos step),
    // and compares three fresh-DUT measurements against p.303's own two
    // rows together: bit 0 (SH0) alone a 140 ns increment, bit 4 (SHSH1)
    // alone a 70 ns increment -- half as many 35 ns samples.
    {
        int base = measure_superhires_start(0x00);
        int sh0 = measure_superhires_start(0x01);
        int shsh1 = measure_superhires_start(0x10);
        bool armed = base != -1 && sh0 != -1 && shsh1 != -1;
        int sh0_delta = armed ? (sh0 - base) : -1000;
        int shsh1_delta = armed ? (shsh1 - base) : -1000;
        checks.note("SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE: at "
            "spres=0b10 (SuperHires), 28 MHz (35 ns) sample deltas from "
            "the ctl=0x00 baseline -- SH0 alone (ctl=0x01): " +
            std::to_string(sh0_delta) + ", SHSH1 alone (ctl=0x10): " +
            std::to_string(shsh1_delta) + " (p.303 predicts 4 and 2).");
#if MUT_BIT4_ORACLE
        // MUTATION: expected relation changed to shsh1_delta == 0
        // (an oracle flip -- the two measurements above are unchanged).
        bool relation = shsh1_delta == 0;
#else
        bool relation = sh0_delta != 0 && sh0_delta % 2 == 0
                      && shsh1_delta == sh0_delta / 2;
#endif
        checks.expect(armed && relation,
            "SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE: at spres=0b10 "
            "(SuperHires), with sprdata sampled on every 28 MHz clock "
            "(35 ns apart, not once per 140 ns hpos step): sh0_delta=" +
            std::to_string(sh0_delta) + " samples, shsh1_delta=" +
            std::to_string(shsh1_delta) + " samples -- the start "
            "measured with SPR0CTL bit 4 (SHSH1) alone set must move from "
            "the ctl=0x00 baseline by exactly half as many samples as bit "
            "0 (SH0) alone does -- matching the two rows together: "
            "'04      SHSH1     Start horizontal (SHR mode) 70ns "
            "increment' and '00      SH0       Start horiz. value Low bit "
            "140 ns increment' (p.303), 70ns being half of 140ns, scoped "
            "to SuperHires resolution ('70ns sprite positions are only "
            "available in SuperHires mode.' p.303; 'This allows for "
            "positioning the sprite every other SuperHires pixel on 70ns "
            "boundaries.' p.303; 'The new SuperHires mode provides 35ns "
            "pixel rates and sprite positioning at 70ns rates.' p.297; "
            "'SuperHires mode has a finer granularity of sprite "
            "positioning than either Hires or Lores modes.' p.303) and, "
            "separately, to ECS chips ('Note: bits 3 and  4 are in the "
            "ECS chips only.' p.303) -- the nearest chip-mode signal this "
            "boundary exposes is its own `aga`=0 input (it has no "
            "separate OCS-vs-ECS select to test the chip-scoping itself "
            "against). C-library.c2, C-shr.c05, C-shres-sprite.c1, "
            "C-shres-sprite.c2, C-spr.c01, C-shres-sprite.c15, C-spr.c10, "
            "C-shres-sprite.c14, C-spr.c09 "
            ".");
    }

    // ---- Outside SuperHires: recorded, not asserted ----
    // p.303 gives bit 4 no position outside SuperHires ('70ns sprite
    // positions are only available in SuperHires mode.') and Hires is
    // promised nothing at all -- this boundary asserts nothing about bit
    // 4 there. Recorded only, at spres=0 (Lores), sampled once per 140 ns
    // hpos step (measure() above): whether bit 4 alone moves the trace
    // at all.
    {
        Meas lores_base = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
        Meas lores_bit4 = measure(/*pos=*/0x40, static_cast<uint8_t>(1u << 4), /*spres=*/0, /*aga=*/0);
        bool lores_identical = lores_base.trace == lores_bit4.trace;
        checks.record(
            std::string("Outside SuperHires (spres=0, Lores) [recorded, "
            "not asserted]: setting SPR0CTL bit 4 alone leaves the "
            "sprdata trace, sampled once per 140 ns hpos step, ") +
            (lores_identical ? "identical to" : "different from") +
            " the ctl=0x00 baseline. p.303 promises bit 4 no position "
            "outside SuperHires, and Hires nothing at all -- this is "
            "silence in the manual, not a requirement, so it is recorded "
            "rather than asserted.");
    }

    // ---- APPC-pos.c1, CH4-appC-position.c1/c2 [withdrawn, recorded] ----
    // Withdrawn from SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE above (this
    // repair pass): their claim compares three resolution settings, and
    // the assertion above sets bit 4 at spres=0 only, with no SuperHires
    // setting and no Hires/Lores comparison.
    checks.record(
        "APPC-pos.c1, CH4-appC-position.c1, CH4-appC-position.c2 "
        "[withdrawn from SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE]: "
        "'SuperHires mode has a finer granularity of sprite positioning "
        "than either Hires or Lores modes.' (p.303) is a comparison across "
        "three resolution settings. This module's position-match "
        "comparator ('hpos[7:0] == hstart[7:0]' plus the hpos[8] term, "
        "denise_sprites_shifter.v) does not read `spres` at all -- SH0's "
        "own delta stayed 1 raw unit at every one of the 4 spres "
        "encodings, measured above under SPRXCTL-BIT0-SH0-IS-140NS-LOW-"
        "BIT -- and the bits that would carry a finer-than-SH0 step "
        "within SuperHires (SHSH1/SHSH0, bits 4/3) are dead code in this "
        "RTL (the FINDING above: only bits 7 and 0 of a CTL write are "
        "ever captured). There is no stimulus at this boundary through "
        "which SuperHires positioning could be shown finer than Hires/"
        "Lores. Withdrawn whole, not built as a vacuous comparison.");

    // ---- APPC-pos.c10, APPC2-SPRxCTL.conf2 [recorded, not asserted] ----
    // Both routed record:ch3-sprite-registers, "Print RECORDED beside
    // SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE" (routing.how). Neither id
    // goes on the expect() above -- printing them there would count them
    // ASSERTED on a line that also carries plainly-asserting ids, which is
    // not what their own restatement says of itself.
    checks.record(
        "APPC-pos.c10 [recorded, not asserted]: 'SPRxCTL bit 04 is SHSH1, "
        "the start horizontal 70ns increment in SuperHires mode ... "
        "Recorded as a conflict, not resolved.' (source_defines: page 303 "
        "versus pages 107/128 and appendix A page 284). The disputed half "
        "-- whether bit 4 is a working 70ns increment on ECS -- is sent by "
        "the claim's own classification to the NM-HRM-019 regression "
        "record, not to an assertion; SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-"
        "LIVE above already grades that same underlying RTL fact (RED, "
        "bit 4 captured by no register in this RTL at all) for the ids "
        "that assert it plainly. Recorded beside that assertion, per "
        "routing.how, rather than asserted on it.");
    checks.record(
        "APPC2-SPRxCTL.conf2 [recorded, not asserted]: the appendix B "
        "register-summary row '142  SPRxCTL  chg   W  A   Sprite x "
        "position and control data' (p.298) marks SPRxCTL 'chg', and "
        "appendix C p.303 explains that as bits 3 and 4 being ECS-only, "
        "against chapter 4 pp.107/128 and appendix A p.284, which call "
        "the same bits unused/reserved without naming a chipset -- "
        "'Recorded, not resolved.' A second, independent source for the "
        "same NM-HRM-019 regression SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-"
        "LIVE above already grades (RED: bit 4 is not chip-scoped in this "
        "RTL, it is simply absent from the implementation, a stronger "
        "deviation than 'ECS chips only' chip-scoping describes). "
        "Recorded beside that assertion, per routing.how, rather than "
        "asserted on it.");

    // ------- SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES (new assertion) -------
    // New for this extension pass. Distinct from SPRXCTL-BIT3-SHSH0-
    // UNIMPLEMENTED [declined] below, which declines C-shres-sprite.c13
    // because the manual's own row already states the field's verdict --
    // a scope decision made for THAT id. The router for this pass's two
    // new bit-3 ids (APPC-pos.c11, CH4-appC-position.c19) asks for a
    // measurement instead: "grade it as an inert bit, SPR0CTL bit 3 set
    // against clear leaving the sprdata/nsprite trace identical at every
    // spres" (routing.how). That is observable at this boundary via
    // sprdata, so it is graded here, not declined -- a different
    // disposition for a different pair of ids, not a change to the
    // existing declined assertion's own scope.
    {
        bool inert_everywhere = true;
        for (int sp = 0; sp < 4; ++sp) {
#if MUT_BIT3
            // MUTATION: bit 0 (SH0, already proven live above) substituted
            // for bit 3, so the "identical trace" comparator has something
            // it should catch.
            uint8_t toggle_bit = 0x01;
#else
            uint8_t toggle_bit = 0x08;  // bit 3 (SHSH0).
#endif
            Meas off = measure(/*pos=*/0x40, /*ctl=*/0x00, static_cast<uint8_t>(sp), /*aga=*/0);
            Meas on  = measure(/*pos=*/0x40, toggle_bit, static_cast<uint8_t>(sp), /*aga=*/0);
            if (off.trace != on.trace) inert_everywhere = false;
        }
        checks.expect(inert_everywhere,
            "SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES: setting SPR0CTL bit "
            "3 alone (all other bits, including SH0, held at 0) leaves the "
            "full sprdata trace measured across hpos 0..511 identical to "
            "the same sweep with bit 3 clear, at every one of the 4 spres "
            "values this module's `spres` input can take -- matching the "
            "manual's own verdict on this bit's row: "
            "'03      SHSH0     Start horizontal (SHR mode) 35ns "
            "(unimplemented)' (p.303). APPC-pos.c11, "
            "CH4-appC-position.c19 .");
    }

    // ------------- SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED [declined] -------------
    // Replaces (its bit-3 half of) SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS (see
    // CORRECTION above). No checks.expect() and no MUTATION record: there
    // is no assertion here for a stimulus to turn red.
    checks.record(
        "SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED [declined]: C-shres-sprite.c13 "
        "-- the manual prints bit 3's own row with its verdict already in "
        "it: '03      SHSH0     Start horizontal (SHR mode) 35ns "
        "(unimplemented)' (p.303). 'Note: bits 3 and  4 are in the ECS "
        "chips only.' (p.303) names which chip carries the bit; it does "
        "not say the bit is live, and does not contradict the row's own "
        "'(unimplemented)'. Nothing is left to require of this bit at "
        "this boundary: declined, not tested -- matching the manual's own "
        "word for the field, not silently dropping it.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
