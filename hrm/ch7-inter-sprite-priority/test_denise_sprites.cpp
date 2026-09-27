// ch7-inter-sprite-priority -- denise_sprites conformance test
//
// Test purpose: Figure 7-1's inter-sprite fixed priority (eight overlapping
//   boxes numbered 0 to 7, box 0 in front) and Figure 7-2's sprite-GROUP
//   ordering (SP01 > SP23 > SP45 > SP67), both as observable at the
//   denise_sprites module boundary. Package ch7-inter-sprite-priority,
//   fixture denise_sprites (denise_sprites.v + denise_sprites_shifter.v, the
//   same two-file build hrm/denise_sprites/ and hrm/ch4-attached-sprites/
//   already use).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 207-209 (text) and Figures 7-1 (p.208) and 7-2 (p.209), both opened
//   and read directly from ch7/figures/p208.png and p209.png.
// Non-goals: anything outside the `denise_sprites` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM SOURCE: this package's own claims file is
//   hrm-testpipeline/work/ch7-build/
//   ch7-inter-sprite-priority.jsonl (23 records). All 23 are cited below,
//   in full, inside an expect() or record() label. None are borrowed from,
//   or lent to, any other package. The sibling package
//   ch7-sprite-priority-groups (fixture denise_spritepriority) owns group
//   MEMBERSHIP (which sprites form which pair) -- this package owns only
//   ORDER (which sprite, or which group, wins), which is not observable at
//   that sibling's fixture (classify.evidence for this package's own
//   claims: nsprite=0x01, 0x02 and 0x03 all give the identical sprsel at
//   denise_spritepriority) and IS observable here.
//
// HARNESS BASIS: SPECIFICATION.md is written from claims.jsonl's quotations
//   and the two oracle figures alone. The register word-address map
//   (SPR0POS=0xA0 step 4 per sprite), the clk7_en/c1/c3 four-phase
//   clocking, and the arm-on-write (CTL, then DATB, then DATA -- DATA last,
//   since it both arms the sprite and, if a later CTL write were to follow,
//   would be disarmed by it) are the same as
//   hrm/ch4-attached-sprites/test_denise_sprites.cpp's, a sibling harness
//   for this exact fixture. denise_sprites.v's port list, its priority
//   cascade (the nsprite[1:0] group tested first, then [3:2], then [5:4],
//   then [7:6], each testing the lower sprite of its own pair first) and
//   the sprdata packing ({esprm or osprm, 2-bit group id, 2-bit raw pixel})
//   are at denise_sprites.v:32-64 and :470-512, as the claims file's own
//   classify.evidence field states.
//
// STIMULUS DESIGN: for the twenty-eight pairwise assertions (PRI-*) and the
//   two order-invariance assertions (INV-*), the two competing sprites of a
//   pair are given DISTINGUISHABLE non-zero 2-bit pixel codes -- the
//   lower-numbered sprite's own DATA register gets 0x8000 (pixel code
//   2'b01), the higher-numbered sprite's own DATB register gets 0x8000
//   (pixel code 2'b10) -- so sprdata's own low 2 bits, read at the one
//   hpos where the pair's shared horizontal start fires, say directly
//   which sprite's own pixel reached the output, without needing to
//   decode the esprm/osprm/group tag bits at all. This is the same
//   "distinguishable 2-bit codes" method the claims file's own
///  classify.evidence already describes as MEASURED.
//
// WHAT IS GRADED:
//   Every pair. p.207's own sentence -- "You cannot change the relative
//   priorities of the sprites. They will always appear on the screen with
//   the lower-numbered sprites appearing in front of ... the
//   higher-numbered sprites" -- states the rule for every overlapping pair,
//   C(8,2)=28 of them, not only the 7 adjacent ones; a design where sprite
//   0 lost to sprite 2 must fail. PRI-01..PRI-67 grade the seven ADJACENT
//   pairs and PRI-02..PRI-57 the 21 non-adjacent ones, on the same
//   measurement basis and mutation pattern (see the "PRI-02 .. PRI-57"
//   block below for the full citation reasoning: they use
//   sol:7-video.c1/opus:7-vp.c1/opus:7-vp.c9, the page-207/209 general
//   wording, not opus:7-vp.c2, whose own recorded evidence covers only the
//   seven adjacent boxes).
//
//   What p.208/207/209 actually say, no more. SPR-0..SPR-7 grade liveness
//   plus code identity, not the FULL packed sprdata byte --
//   expected_single_byte()'s ((n/2)<<2) group-id term, taken from
//   denise_sprites.v's own cascade packing -- nor an exact one-sample
//   pulse width; p.208 establishes eight sprites numbered 0-7 and neither
//   of those. INV-01/INV-67 grade the same winner at the same hpos in both
//   arm orders, not fwd.trace==rev.trace, equality of all 512 internal
//   sprdata samples; p.207/209 state an invariance about which sprite wins
//   the overlap, not about internal state at every other hpos. The
//   RTL-derived parts are measured and printed (SPR-N-ENCODING-MEASURED,
//   INV-0N-FULLTRACE-MEASURED) and are not required by expect(), each with
//   its own NOT-GRADED record(), reason_kind source-silent.
//
//   The RTL's own cascade (denise_sprites.v: whichever group -- (0,1), then
//   (2,3), then (4,5), then (6,7) -- is checked first and, within it, the
//   lower sprite wins if it is active at all) implements full transitive
//   lower-wins priority, not only adjacent-pair priority, so every PRI-*
//   assertion passes.
//
// CLAIM COVERAGE -- all 23 records cited, each inside an expect() or
//   record() label. The 21 non-adjacent pairs reuse the universal-claim
//   ids; no further claim record is involved:
//   FIXTURE-LIVE-DATA-PATH [control, no claim id, no page]
//   SPR-0 .. SPR-7 (8 assertions, p.208) -- opus:7-fig7-1.c1 (hardware
//     half only; the RTL-derived packed-byte/pulse-width half is measured
//     and recorded, not graded -- see WHAT IS GRADED above)
//   PRI-01 .. PRI-67 (7 adjacent-pair assertions, p.208, plus p.209
//     group-crossing support on PRI-12/34/56):
//     PRI-01: sol:7-fig7-1.c1, opus:7-fig7-1.c2, opus:7-vp.c2
//     PRI-12: sol:7-fig7-1.c2, opus:7-fig7-1.c3, opus:7-fig7-2.c6, opus:7-vp.c2
//     PRI-23: sol:7-fig7-1.c3, opus:7-fig7-1.c4, opus:7-vp.c2
//     PRI-34: sol:7-fig7-1.c4, opus:7-fig7-1.c5, opus:7-fig7-2.c7, opus:7-vp.c2
//     PRI-45: sol:7-fig7-1.c5, opus:7-fig7-1.c6, opus:7-vp.c2
//     PRI-56: sol:7-fig7-1.c6, opus:7-fig7-1.c7, sol:7-fig7-2.c9,
//             opus:7-fig7-2.c8, opus:7-vp.c2
//     PRI-67: sol:7-fig7-1.c7, opus:7-fig7-1.c8, opus:7-vp.c2
//   PRI-02 .. PRI-57 (21 non-adjacent-pair assertions, p.207+209):
//     sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 on each --
//     the same three ids INV-01/INV-67 already use, because these are the
//     claims that actually state the rule without an adjacency qualifier.
//   INV-01, INV-67 (2 assertions, p.207+209): sol:7-video.c1, opus:7-vp.c1,
//     opus:7-vp.c9
//   Declined [no expect(), record() only]:
//     opus:7-fig7-1.c1's drawing-inventory half (p.208)
//     opus:7-fig7-1.c2 through opus:7-fig7-1.c8's ink-description halves
//       (p.208, 7 ids)
//   23 records = 8 (SPR, sharing opus:7-fig7-1.c1's hardware half) + 7
//   adjacent PRI + 2 INV citation sets + 2 declined records, with
//   opus:7-fig7-1.c1 (both halves), opus:7-vp.c2, the three fig7-2
//   group-crossing ids, and sol:7-video.c1/opus:7-vp.c1/opus:7-vp.c9
//   (21 more times, on PRI-02..PRI-57) each doing multiple duty.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id) proves the
//   write -> arm -> hpos-sweep -> read-sprdata measurement basis every
//   other assertion in this file shares is alive. PRI-01 and SPR-0 are also
//   nominated controls in
//   SPECIFICATION.md: none of this package's assertions are under
//   suspicion, and all thirty-eight other graded assertions share this
//   same basis.
//
// RESULTS (WSL/Verilator 5.052): 39 passed, 0 failed. All thirty-nine
//   graded assertions (FIXTURE-LIVE-DATA-PATH, SPR-0..SPR-7,
//   PRI-01..PRI-67, PRI-02..PRI-57, INV-01, INV-67) are GREEN: the manual's
//   universal priority rule holds under measurement for every one of the
//   28 pairs.
//   Measured hpos values: FIXTURE-LIVE-DATA-PATH MSB hpos=130, mid
//   hpos=138, LSB hpos=145 (distinct, monotonic); every one of SPR-0..
//   SPR-7, and every PRI-*/INV-* pair (all armed at the same horizontal
//   start, pos_byte=0x40), hits at hpos=130, confirming all eight sprites'
//   own register-to-shifter paths are independently live and that the
//   pairwise measurement basis is the same one FIXTURE-LIVE-DATA-PATH
//   proved alive. SPR-N-ENCODING-MEASURED bytes (measured only):
//   SPR-0/1=0x01, SPR-2/3=0x05, SPR-4/5=0x09, SPR-6/7=0x0D, each with
//   exactly 1 active sample -- denise_sprites.v's own packing, which p.208
//   does not require. INV-01/INV-67-FULLTRACE-MEASURED (measured only):
//   both true (fwd.trace==rev.trace over all 512 samples) on the
//   unmutated build.
//
// MUTATION LOG (WSL/Verilator; each
//   entry actually run: `make MUT=<NAME>` -> read the PASS/FAIL lines ->
//   confirm only the named assertion changed colour -> `make` with no flag
//   -> confirm the 39-passed/0-failed baseline was restored). Gate
//   grammar: `// MUTATION <assertion-label> <oracle|stimulus|none>
//   <red|green|impossible> <what changed>` -- the second token is the
//   assertion's OWN label, exactly as passed to expect(), not the -D
//   switch that drives it; the switch name is kept in the description so a
//   reader can still find the build. All thirty-nine below are STIMULUS
//   mutations: each changes what the module is ASKED to do (an arming
//   write withheld) and leaves the expected value untouched: flipping an
//   expected value proves nothing.
//
//   INV-01 and INV-67 are the two where the mutation itself needs a reason,
//   not just a result: even under the narrow same-hpos/same-code
//   grading, the two runs' WINNER is compared to
//   EACH OTHER, not to a constant, so a mutation that changed both sides
//   alike (e.g. a symmetric arm-order swap) would leave them agreeing and
//   would not have failed -- it would only re-demonstrate the same
//   order-invariance the assertion already asserts. Both are built
//   asymmetrically instead: the lower sprite is withheld in the
//   REVERSE-armed run ONLY, so the forward run still shows the lower
//   sprite's code while the reverse run now shows the higher sprite's --
//   the two sides the comparison compares are forced to disagree, and the
//   assertion goes red because ITS OWN comparison is sensitive, not because
//   a hand-picked expected byte changed.
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: MUT_FIXTURE=1 makes the MSB-run stimulus 0x0001, identical to the LSB-run -> FAIL (38 passed, 1 failed), only FIXTURE-LIVE-DATA-PATH newly red (MSB hpos=145 became equal to LSB hpos=145, no longer distinct or monotonic; mid hpos=138 unchanged). Rebuilt with no flag: 39 passed, 0 failed restored.
// MUTATION SPR-0 stimulus red: MUT_SPR0=1 withholds arming for sprite 0 (its own four registers are never written) -> FAIL (38 passed, 1 failed), only SPR-0 newly red (hit hpos=-1 -- nothing on sprdata at all), the other seven SPR-* and all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION SPR-1 stimulus red: MUT_SPR1=1 withholds arming for sprite 1 -> FAIL (38 passed, 1 failed), only SPR-1 newly red (hit hpos=-1), all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION SPR-2 stimulus red: MUT_SPR2=1 withholds arming for sprite 2 -> FAIL (38 passed, 1 failed), only SPR-2 newly red (hit hpos=-1), all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION SPR-3 stimulus red: MUT_SPR3=1 withholds arming for sprite 3 -> FAIL (38 passed, 1 failed), only SPR-3 newly red (hit hpos=-1), all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION SPR-4 stimulus red: MUT_SPR4=1 withholds arming for sprite 4 -> FAIL (38 passed, 1 failed), only SPR-4 newly red (hit hpos=-1), all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION SPR-5 stimulus red: MUT_SPR5=1 withholds arming for sprite 5 -> FAIL (38 passed, 1 failed), only SPR-5 newly red (hit hpos=-1), all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION SPR-6 stimulus red: MUT_SPR6=1 withholds arming for sprite 6 -> FAIL (38 passed, 1 failed), only SPR-6 newly red (hit hpos=-1), all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION SPR-7 stimulus red: MUT_SPR7=1 withholds arming for sprite 7 -> FAIL (38 passed, 1 failed), only SPR-7 newly red (hit hpos=-1), all other assertions unchanged. Rebuilt with no flag: baseline restored.
// MUTATION PRI-01 stimulus red: MUT_PRI01=1 withholds arming for sprite 0 (the lower of the pair); group(0,1) then has only sprite 1 active, so sprdata's low bits read 2'b10 (sprite 1's code) instead of the expected 2'b01 -> FAIL (38 passed, 1 failed), only PRI-01 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-12 stimulus red: MUT_PRI12=1 withholds arming for sprite 1 (the lower of the pair); group(0,1) is then entirely inactive and the cascade falls through to group(2,3), where sprite 2 is the only active sprite, so sprdata reads sprite 2's code (2'b10) instead of sprite 1's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-12 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-23 stimulus red: MUT_PRI23=1 withholds arming for sprite 2 (the lower of the pair); group(2,3) then has only sprite 3 active, so sprdata reads sprite 3's code (2'b10) instead of sprite 2's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-23 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-34 stimulus red: MUT_PRI34=1 withholds arming for sprite 3 (the lower of the pair); group(2,3) is then entirely inactive and the cascade falls through to group(4,5), where sprite 4 is the only active sprite, so sprdata reads sprite 4's code (2'b10) instead of sprite 3's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-34 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-45 stimulus red: MUT_PRI45=1 withholds arming for sprite 4 (the lower of the pair); group(4,5) then has only sprite 5 active, so sprdata reads sprite 5's code (2'b10) instead of sprite 4's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-45 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-56 stimulus red: MUT_PRI56=1 withholds arming for sprite 5 (the lower of the pair); group(4,5) is then entirely inactive and the cascade falls through to group(6,7), where sprite 6 is the only active sprite, so sprdata reads sprite 6's code (2'b10) instead of sprite 5's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-56 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-67 stimulus red: MUT_PRI67=1 withholds arming for sprite 6 (the lower of the pair); group(6,7) then has only sprite 7 active, so sprdata reads sprite 7's code (2'b10) instead of sprite 6's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-67 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-02 stimulus red: MUT_PRI02=1 withholds arming for sprite 0 (the lower of the pair); only sprite 2 is armed, so group(2,3) wins by default and sprdata reads sprite 2's code (2'b10) instead of sprite 0's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-02 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-03 stimulus red: MUT_PRI03=1 withholds arming for sprite 0; only sprite 3 armed, group(2,3) wins by default, sprdata reads sprite 3's code (2'b10) instead of sprite 0's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-03 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-04 stimulus red: MUT_PRI04=1 withholds arming for sprite 0; only sprite 4 armed, group(4,5) wins by default, sprdata reads sprite 4's code (2'b10) instead of sprite 0's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-04 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-05 stimulus red: MUT_PRI05=1 withholds arming for sprite 0; only sprite 5 armed, group(4,5) wins by default, sprdata reads sprite 5's code (2'b10) instead of sprite 0's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-05 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-06 stimulus red: MUT_PRI06=1 withholds arming for sprite 0; only sprite 6 armed, group(6,7) wins by default, sprdata reads sprite 6's code (2'b10) instead of sprite 0's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-06 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-07 stimulus red: MUT_PRI07=1 withholds arming for sprite 0; only sprite 7 armed, group(6,7) wins by default, sprdata reads sprite 7's code (2'b10) instead of sprite 0's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-07 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-13 stimulus red: MUT_PRI13=1 withholds arming for sprite 1; only sprite 3 armed, group(2,3) wins by default, sprdata reads sprite 3's code (2'b10) instead of sprite 1's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-13 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-14 stimulus red: MUT_PRI14=1 withholds arming for sprite 1; only sprite 4 armed, group(4,5) wins by default, sprdata reads sprite 4's code (2'b10) instead of sprite 1's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-14 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-15 stimulus red: MUT_PRI15=1 withholds arming for sprite 1; only sprite 5 armed, group(4,5) wins by default, sprdata reads sprite 5's code (2'b10) instead of sprite 1's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-15 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-16 stimulus red: MUT_PRI16=1 withholds arming for sprite 1; only sprite 6 armed, group(6,7) wins by default, sprdata reads sprite 6's code (2'b10) instead of sprite 1's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-16 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-17 stimulus red: MUT_PRI17=1 withholds arming for sprite 1; only sprite 7 armed, group(6,7) wins by default, sprdata reads sprite 7's code (2'b10) instead of sprite 1's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-17 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-24 stimulus red: MUT_PRI24=1 withholds arming for sprite 2; only sprite 4 armed, group(4,5) wins by default, sprdata reads sprite 4's code (2'b10) instead of sprite 2's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-24 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-25 stimulus red: MUT_PRI25=1 withholds arming for sprite 2; only sprite 5 armed, group(4,5) wins by default, sprdata reads sprite 5's code (2'b10) instead of sprite 2's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-25 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-26 stimulus red: MUT_PRI26=1 withholds arming for sprite 2; only sprite 6 armed, group(6,7) wins by default, sprdata reads sprite 6's code (2'b10) instead of sprite 2's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-26 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-27 stimulus red: MUT_PRI27=1 withholds arming for sprite 2; only sprite 7 armed, group(6,7) wins by default, sprdata reads sprite 7's code (2'b10) instead of sprite 2's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-27 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-35 stimulus red: MUT_PRI35=1 withholds arming for sprite 3; only sprite 5 armed, group(4,5) wins by default, sprdata reads sprite 5's code (2'b10) instead of sprite 3's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-35 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-36 stimulus red: MUT_PRI36=1 withholds arming for sprite 3; only sprite 6 armed, group(6,7) wins by default, sprdata reads sprite 6's code (2'b10) instead of sprite 3's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-36 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-37 stimulus red: MUT_PRI37=1 withholds arming for sprite 3; only sprite 7 armed, group(6,7) wins by default, sprdata reads sprite 7's code (2'b10) instead of sprite 3's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-37 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-46 stimulus red: MUT_PRI46=1 withholds arming for sprite 4; only sprite 6 armed, group(6,7) wins by default, sprdata reads sprite 6's code (2'b10) instead of sprite 4's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-46 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-47 stimulus red: MUT_PRI47=1 withholds arming for sprite 4; only sprite 7 armed, group(6,7) wins by default, sprdata reads sprite 7's code (2'b10) instead of sprite 4's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-47 newly red. Rebuilt with no flag: baseline restored.
// MUTATION PRI-57 stimulus red: MUT_PRI57=1 withholds arming for sprite 5; only sprite 7 armed, group(6,7) wins by default, sprdata reads sprite 7's code (2'b10) instead of sprite 5's (2'b01) -> FAIL (38 passed, 1 failed), only PRI-57 newly red. Rebuilt with no flag: baseline restored.
// MUTATION INV-01 stimulus red: MUT_INV01=1 withholds arming for sprite 0 (the pair's lower sprite) in the REVERSE-armed run only, leaving the forward-armed run untouched; the forward run's winner at hpos=130 is still sprite 0's code (2'b01), but the reverse run's winner at the same hpos is now sprite 1's (2'b10), so same_winner's code-identity check fails on the reverse side even though fwd.hpos==rev.hpos -> FAIL (38 passed, 1 failed), only INV-01 newly red (INV-01-FULLTRACE-MEASURED also flips true->false, corroborating but not required for the red). Rebuilt with no flag: baseline restored.
// MUTATION INV-67 stimulus red: MUT_INV67=1 withholds arming for sprite 6 (the pair's lower sprite) in the REVERSE-armed run only, leaving the forward-armed run untouched; same mechanism as INV-01 at the opposite end of the cascade: forward winner stays sprite 6's code (2'b01), reverse winner becomes sprite 7's (2'b10) at the same hpos -> FAIL (38 passed, 1 failed), only INV-67 newly red. Rebuilt with no flag: baseline restored.
//
// No mutation record for opus:7-fig7-1.c1/c2..c8 [declined]: these two
//   claims.record() calls carry no checks.expect(), so there is no
//   red/green state for a mutation to move.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_sprites.h"

// One #define per graded assertion, each replaying that assertion's own
// "// MUTATION" stimulus change when built with -DMUT_<NAME>=1 (via
// `make MUT=<NAME>`). Never combined: exactly one is set per build.
#ifndef MUT_FIXTURE
#define MUT_FIXTURE 0
#endif
#ifndef MUT_SPR0
#define MUT_SPR0 0
#endif
#ifndef MUT_SPR1
#define MUT_SPR1 0
#endif
#ifndef MUT_SPR2
#define MUT_SPR2 0
#endif
#ifndef MUT_SPR3
#define MUT_SPR3 0
#endif
#ifndef MUT_SPR4
#define MUT_SPR4 0
#endif
#ifndef MUT_SPR5
#define MUT_SPR5 0
#endif
#ifndef MUT_SPR6
#define MUT_SPR6 0
#endif
#ifndef MUT_SPR7
#define MUT_SPR7 0
#endif
#ifndef MUT_PRI01
#define MUT_PRI01 0
#endif
#ifndef MUT_PRI12
#define MUT_PRI12 0
#endif
#ifndef MUT_PRI23
#define MUT_PRI23 0
#endif
#ifndef MUT_PRI34
#define MUT_PRI34 0
#endif
#ifndef MUT_PRI45
#define MUT_PRI45 0
#endif
#ifndef MUT_PRI56
#define MUT_PRI56 0
#endif
#ifndef MUT_PRI67
#define MUT_PRI67 0
#endif
// Non-adjacent pairs: the universal p.207 rule -- "the lower-numbered
// sprites appearing in front of ... the higher-numbered sprites" -- covers
// every overlapping pair, not only the seven adjacent ones PRI-01..PRI-67
// grade.
#ifndef MUT_PRI02
#define MUT_PRI02 0
#endif
#ifndef MUT_PRI03
#define MUT_PRI03 0
#endif
#ifndef MUT_PRI04
#define MUT_PRI04 0
#endif
#ifndef MUT_PRI05
#define MUT_PRI05 0
#endif
#ifndef MUT_PRI06
#define MUT_PRI06 0
#endif
#ifndef MUT_PRI07
#define MUT_PRI07 0
#endif
#ifndef MUT_PRI13
#define MUT_PRI13 0
#endif
#ifndef MUT_PRI14
#define MUT_PRI14 0
#endif
#ifndef MUT_PRI15
#define MUT_PRI15 0
#endif
#ifndef MUT_PRI16
#define MUT_PRI16 0
#endif
#ifndef MUT_PRI17
#define MUT_PRI17 0
#endif
#ifndef MUT_PRI24
#define MUT_PRI24 0
#endif
#ifndef MUT_PRI25
#define MUT_PRI25 0
#endif
#ifndef MUT_PRI26
#define MUT_PRI26 0
#endif
#ifndef MUT_PRI27
#define MUT_PRI27 0
#endif
#ifndef MUT_PRI35
#define MUT_PRI35 0
#endif
#ifndef MUT_PRI36
#define MUT_PRI36 0
#endif
#ifndef MUT_PRI37
#define MUT_PRI37 0
#endif
#ifndef MUT_PRI46
#define MUT_PRI46 0
#endif
#ifndef MUT_PRI47
#define MUT_PRI47 0
#endif
#ifndef MUT_PRI57
#define MUT_PRI57 0
#endif
#ifndef MUT_INV01
#define MUT_INV01 0
#endif
#ifndef MUT_INV67
#define MUT_INV67 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map, reused from hrm/ch4-attached-sprites/'s own
// test_denise_sprites.cpp for this same module: SPRPOSCTLBASE byte 0x140 ->
// word 0xA0 for SPR0POS, four consecutive word addresses per sprite (POS,
// CTL, DATA, DATB), eight sprites -> SPR0..SPR7 at 0xA0, 0xA4, ..., 0xBC.
// Confirmed directly against denise_sprites.v's own selspr0..selspr7
// decode and denise_sprites_shifter.v's own POS/CTL/DATA/DATB parameters.
constexpr int SPR_BASE(int n) { return 0xA0 + 4 * n; }  // + 0 POS, +1 CTL, +2 DATA, +3 DATB
constexpr int IDLE_ADDR = 0x00;
constexpr int HPOS_MAX = 512;  // SH8..SH0 is a 9-bit value: 0..511 covers it.

struct Meas {
    std::vector<uint8_t> trace;  // sprdata at every swept hpos.
};

// Clocking (clk7_en/c1/c3 four-phase tick, reg_address_in/data_in write
// sequencing) reused from hrm/ch4-attached-sprites/ for this same module,
// not learned from this package's own RTL (see ORDER OF WORK above).
class Harness {
public:
    Harness() : dut(new Vdenise_sprites) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->aga = 0;      // OCS/ECS account -- this package's own chipset scope.
        dut->spres = 0;
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

    // Arms sprite n at pos_byte with the given DATA/DATB words. CTL is
    // always written (no attach, bit7=0) before DATB and DATA -- DATA last,
    // since writing DATA is what sets this sprite's own "armed" flip-flop,
    // and a CTL write after it would clear that flip-flop again
    // (denise_sprites_shifter.v: "aen && address==CTL) armed <= 0").
    void arm_sprite(int n, uint8_t pos_byte, uint16_t data_word, uint16_t datb_word) {
        int base = SPR_BASE(n);
        write_reg(base + 0, pos_byte);   // POS
        write_reg(base + 1, 0x0000);     // CTL: attach=0
        write_reg(base + 3, datb_word);  // DATB
        write_reg(base + 2, data_word);  // DATA -- arms the sprite.
    }

    Vdenise_sprites* dut;
    int phase = 0;
};

// Sweeps the full hpos range on an already-armed harness and returns the
// sprdata trace.
Meas sweep(Harness& h) {
    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        m.trace.push_back(static_cast<uint8_t>(h.dut->sprdata));
    }
    return m;
}

// Arms sprite n alone (no other sprite is ever written, so every other
// sprite stays in its post-reset, un-armed state) with the given DATA word
// (DATB held at 0), unless `arm` is false, in which case sprite n's own
// registers are never written at all (used by the MUT_SPR* mutations).
// Used by FIXTURE-LIVE-DATA-PATH and SPR-0..SPR-7.
Meas measure_single(int n, uint8_t pos_byte, uint16_t data_word, bool arm = true) {
    Harness h;
    if (arm) h.arm_sprite(n, pos_byte, data_word, 0x0000);
    return sweep(h);
}

// Arms sprite `lo` (the lower-numbered of an adjacent pair) with pixel
// code 2'b01 (DATA=0x8000) and sprite `hi` (the higher-numbered) with
// pixel code 2'b10 (DATB=0x8000), both at the SAME horizontal start
// (pos_byte), so the pair genuinely overlaps. `lo_first` selects which
// sprite's own four registers are written first in real time -- the two
// sprites' final register CONTENTS are identical either way, only the
// write order differs. `arm_lo`/`arm_hi` (default true) can withhold one
// sprite's own arming entirely -- used by the MUT_PRI*/MUT_INV* mutations.
// Used by PRI-01..PRI-67 and INV-01/INV-67.
Meas measure_pair(int lo, int hi, uint8_t pos_byte, bool lo_first,
                   bool arm_lo = true, bool arm_hi = true) {
    Harness h;
    auto do_lo = [&]() { if (arm_lo) h.arm_sprite(lo, pos_byte, 0x8000, 0x0000); };
    auto do_hi = [&]() { if (arm_hi) h.arm_sprite(hi, pos_byte, 0x0000, 0x8000); };
    if (lo_first) { do_lo(); do_hi(); } else { do_hi(); do_lo(); }
    return sweep(h);
}

// First (and, for a single-source stimulus, only) hpos at which sprdata is
// non-zero, or -1. Also reports whether more than one sample was
// non-zero, so a "exactly one hit" requirement can be checked.
struct FirstActive {
    int hpos = -1;
    int active_count = 0;
};
FirstActive first_active(const Meas& m) {
    FirstActive r;
    for (size_t i = 0; i < m.trace.size(); ++i) {
        if (m.trace[i] != 0) {
            if (r.hpos == -1) r.hpos = static_cast<int>(i);
            ++r.active_count;
        }
    }
    return r;
}

// Expected sprdata byte when sprite n alone (esprm=osprm=0 throughout this
// file) is armed with DATA=0x8000/DATB=0x0000 (pixel code 2'b01): denise_
// sprites.v's own cascade packs {esprm-or-osprm(4 bits), group id (2
// bits), raw pixel (2 bits)}; with esprm=osprm=0 the top 4 bits are always
// 0 regardless of which half of the pair n is, so only the group id
// (n/2) and the pixel code (01) show.
uint8_t expected_single_byte(int n) {
    return static_cast<uint8_t>(((n / 2) & 0x3) << 2 | 0x1);
}

// Formats a byte as "0xNN" for MEASURED lines -- used only to print what was
// actually read back, never to compute a graded condition.
std::string hex2(uint8_t v) {
    static const char digits[] = "0123456789ABCDEF";
    std::string s = "0x";
    s += digits[(v >> 4) & 0xF];
    s += digits[v & 0xF];
    return s;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- FIXTURE-LIVE-DATA-PATH [control] ----------------
    // Proves the write -> arm -> hpos-sweep -> read-sprdata path is alive
    // and delivers DISTINCT fetched values in order, before any claim-
    // based assertion depends on it (a
    // harness whose data path is silently dead reports every data-
    // dependent assertion as a finding about the design, not the harness).
    // Sprite 0 alone, armed three times from a fresh reset, DATA holding a
    // single 1-bit at three different bit positions (MSB, mid, LSB).
    {
        uint16_t lsb_word = 0x0001;
#if MUT_FIXTURE
        uint16_t msb_word = 0x0001;  // MUTATION: identical to lsb_word.
#else
        uint16_t msb_word = 0x8000;
#endif
        Meas msb = measure_single(0, 0x40, msb_word);
        Meas mid = measure_single(0, 0x40, 0x0080);
        Meas lsb = measure_single(0, 0x40, lsb_word);
        FirstActive fa_msb = first_active(msb);
        FirstActive fa_mid = first_active(mid);
        FirstActive fa_lsb = first_active(lsb);
        bool each_single = fa_msb.active_count == 1 && fa_mid.active_count == 1 &&
                            fa_lsb.active_count == 1;
        bool all_found = fa_msb.hpos != -1 && fa_mid.hpos != -1 && fa_lsb.hpos != -1;
        bool distinct = fa_msb.hpos != fa_mid.hpos && fa_mid.hpos != fa_lsb.hpos &&
                         fa_msb.hpos != fa_lsb.hpos;
        bool monotonic = distinct && all_found &&
            ((fa_msb.hpos < fa_mid.hpos && fa_mid.hpos < fa_lsb.hpos) ||
             (fa_msb.hpos > fa_mid.hpos && fa_mid.hpos > fa_lsb.hpos));
        checks.expect(each_single && all_found && distinct && monotonic,
            "[control] FIXTURE-LIVE-DATA-PATH: arming sprite 0 alone (POS "
            "byte 0x40, CTL=0x00, DATB=0x0000) three times from a fresh "
            "reset, each time with a single distinct 1-bit DATA word "
            "(0x8000, 0x0080, 0x0001 -- bit 15, bit 7, bit 0) and sweeping "
            "hpos 0..511, makes exactly one sprdata sample go non-zero per "
            "run, at three distinct hpos values monotonic in the same "
            "order as the three bit positions -- proving the measurement "
            "basis every assertion below shares actually delivers "
            "distinct fetched values, in order, not a single stuck value. "
            "Fixture check, no claim id, no page. MSB hpos=" +
            std::to_string(fa_msb.hpos) + " mid hpos=" + std::to_string(fa_mid.hpos) +
            " LSB hpos=" + std::to_string(fa_lsb.hpos) + ".");
    }

    // ---------------- SPR-0 .. SPR-7 ----------------
    // Each sprite, armed alone (all seven others left untouched since
    // reset -- so any non-zero sample can only have come from this one),
    // delivers its own known code. Together these ground the hardware
    // half of opus:7-fig7-1.c1 -- "there are eight sprites and they are
    // numbered 0 to 7" -- as eight independently-failing checks, not one
    // check with eight labels: each run is its own fresh Harness, touching
    // only sprite n's own four registers.
    //
    // The graded condition is liveness (some sample went non-zero) plus
    // code fidelity (its low 2 bits match the 2'b01 pixel code this file
    // wrote) -- what p.208 grounds. p.208 establishes eight sprites
    // numbered 0-7, not the FULL packed byte (expected_single_byte()'s
    // ((n/2)<<2) group-id term, read out of denise_sprites.v's own cascade
    // packing) nor an exact one-sample pulse width. Those are measured and
    // printed on every run (SPR-N-ENCODING-MEASURED below), not required by
    // expect() -- see the combined SPR-*-ENCODING-NOT-GRADED record after
    // SPR-7.
    {
        bool arm = true;
#if MUT_SPR0
        arm = false;  // MUTATION: sprite 0 never armed.
#endif
        Meas m = measure_single(0, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-0: sprite 0, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-0-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(0)) + ", one sample).");
    }
    {
        bool arm = true;
#if MUT_SPR1
        arm = false;  // MUTATION: sprite 1 never armed.
#endif
        Meas m = measure_single(1, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-1: sprite 1, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-1-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(1)) + ", one sample).");
    }
    {
        bool arm = true;
#if MUT_SPR2
        arm = false;  // MUTATION: sprite 2 never armed.
#endif
        Meas m = measure_single(2, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-2: sprite 2, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-2-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(2)) + ", one sample).");
    }
    {
        bool arm = true;
#if MUT_SPR3
        arm = false;  // MUTATION: sprite 3 never armed.
#endif
        Meas m = measure_single(3, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-3: sprite 3, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-3-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(3)) + ", one sample).");
    }
    {
        bool arm = true;
#if MUT_SPR4
        arm = false;  // MUTATION: sprite 4 never armed.
#endif
        Meas m = measure_single(4, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-4: sprite 4, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-4-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(4)) + ", one sample).");
    }
    {
        bool arm = true;
#if MUT_SPR5
        arm = false;  // MUTATION: sprite 5 never armed.
#endif
        Meas m = measure_single(5, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-5: sprite 5, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-5-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(5)) + ", one sample).");
    }
    {
        bool arm = true;
#if MUT_SPR6
        arm = false;  // MUTATION: sprite 6 never armed.
#endif
        Meas m = measure_single(6, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-6: sprite 6, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-6-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(6)) + ", one sample).");
    }
    {
        bool arm = true;
#if MUT_SPR7
        arm = false;  // MUTATION: sprite 7 never armed.
#endif
        Meas m = measure_single(7, 0x40, 0x8000, arm);
        FirstActive fa = first_active(m);
        bool ok = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(ok, "SPR-7: sprite 7, armed alone with a known non-zero code, "
            "delivers that code (hit hpos=" + std::to_string(fa.hpos) +
            "). Grounds 'there are eight sprites and they are numbered 0 to 7' "
            "(opus:7-fig7-1.c1, hardware half, p.208).");
        checks.note("SPR-7-ENCODING-MEASURED: full sprdata byte=" +
            hex2(fa.hpos != -1 ? m.trace[fa.hpos] : 0) + ", " +
            std::to_string(fa.active_count) + " active sample(s) (denise_sprites.v's own "
            "packing predicts " + hex2(expected_single_byte(7)) + ", one sample).");
    }
    checks.record(
        "SPR-0-ENCODING-NOT-GRADED through SPR-7-ENCODING-NOT-GRADED (p.208): "
        "the full packed byte (denise_sprites.v's own {esprm/osprm=0, group-id=(n/2), "
        "pixel} packing) and the 'exactly one active sample' pulse width are "
        "denise_sprites.v's own internal encoding. p.208 establishes eight sprites "
        "numbered 0-7 and defines neither the packing nor a pulse width. Both are "
        "measured and printed above (SPR-N-ENCODING-MEASURED); neither is required by "
        "expect(). reason_kind source-silent.");

    // ---------------- PRI-01 .. PRI-67 ----------------
    // Each adjacent pair, armed together at the same horizontal start with
    // distinguishable non-zero codes (lower -> 2'b01, higher -> 2'b10):
    // the lower-numbered sprite's code is the one that reaches sprdata.
    {
        bool arm_lo = true;
#if MUT_PRI01
        arm_lo = false;  // MUTATION: sprite 0 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(0, 1, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.active_count == 1 && fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-01: sprites 0 and 1 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 0's code (2'b01) is the one that "
            "reaches sprdata, not sprite 1's (2'b10) -- box 0 is drawn whole in Figure "
            "7-1, its outline unbroken where box 1 overlaps it. sol:7-fig7-1.c1, "
            "opus:7-fig7-1.c2, opus:7-vp.c2 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI12
        arm_lo = false;  // MUTATION: sprite 1 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(1, 2, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.active_count == 1 && fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-12: sprites 1 and 2 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 1's code is the one that reaches "
            "sprdata, not sprite 2's -- box 1 in front of box 2 in Figure 7-1, and this "
            "adjacency also crosses the SP01/SP23 group boundary of Figure 7-2 (SP01 "
            "finger above SP23 finger on the 'in front' axis). sol:7-fig7-1.c2, "
            "opus:7-fig7-1.c3, opus:7-fig7-2.c6, opus:7-vp.c2 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI23
        arm_lo = false;  // MUTATION: sprite 2 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(2, 3, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.active_count == 1 && fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-23: sprites 2 and 3 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 2's code is the one that reaches "
            "sprdata, not sprite 3's -- box 2 in front of box 3 in Figure 7-1. "
            "sol:7-fig7-1.c3, opus:7-fig7-1.c4, opus:7-vp.c2 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI34
        arm_lo = false;  // MUTATION: sprite 3 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(3, 4, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.active_count == 1 && fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-34: sprites 3 and 4 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 3's code is the one that reaches "
            "sprdata, not sprite 4's -- box 3 in front of box 4 in Figure 7-1, and this "
            "adjacency also crosses the SP23/SP45 group boundary of Figure 7-2. "
            "sol:7-fig7-1.c4, opus:7-fig7-1.c5, opus:7-fig7-2.c7, opus:7-vp.c2 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI45
        arm_lo = false;  // MUTATION: sprite 4 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(4, 5, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.active_count == 1 && fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-45: sprites 4 and 5 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 4's code is the one that reaches "
            "sprdata, not sprite 5's -- box 4 in front of box 5 in Figure 7-1. "
            "sol:7-fig7-1.c5, opus:7-fig7-1.c6, opus:7-vp.c2 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI56
        arm_lo = false;  // MUTATION: sprite 5 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(5, 6, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.active_count == 1 && fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-56: sprites 5 and 6 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 5's code is the one that reaches "
            "sprdata, not sprite 6's -- box 5 in front of box 6 in Figure 7-1, and this "
            "adjacency also crosses the SP45/SP67 group boundary of Figure 7-2 (SP45 "
            "finger above SP67 finger). sol:7-fig7-1.c6, opus:7-fig7-1.c7, "
            "sol:7-fig7-2.c9, opus:7-fig7-2.c8, opus:7-vp.c2 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI67
        arm_lo = false;  // MUTATION: sprite 6 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(6, 7, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.active_count == 1 && fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-67: sprites 6 and 7 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 6's code is the one that reaches "
            "sprdata, not sprite 7's -- box 6 in front of box 7 in Figure 7-1. "
            "sol:7-fig7-1.c7, opus:7-fig7-1.c8, opus:7-vp.c2 .");
    }

    // ---------------- PRI-02 .. PRI-57 (non-adjacent pairs) ----------------
    // p.207 states the rule for EVERY overlapping pair -- "You cannot
    // change the relative priorities of the sprites. They will always
    // appear on the screen with the lower-numbered sprites appearing in
    // front of ... the higher-numbered sprites" -- not only the seven
    // adjacent pairs PRI-01..PRI-67 above grade. Of C(8,2)=28 total pairs,
    // 7 are adjacent (above); these 21 cover the rest. opus:7-vp.c2 is NOT
    // cited here: its own classify.evidence in the claims file is written
    // strictly over the seven adjacent boxes of Figure 7-1's cascade, so
    // citing it on a non-adjacent pair would over-generalise it.
    // sol:7-video.c1 / opus:7-vp.c1 (p.207 body text) and
    // opus:7-vp.c9 (p.209 hand analogy) carry the actual universal wording
    // and are cited instead -- the same claim ids INV-01/INV-67 already
    // use for the same reason. Same measurement basis, same mutation
    // pattern (withhold the lower sprite of the pair) as PRI-01..PRI-67.
    {
        bool arm_lo = true;
#if MUT_PRI02
        arm_lo = false;  // MUTATION: sprite 0 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(0, 2, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-02: sprites 0 and 2 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 0's code is the one that reaches "
            "sprdata, not sprite 2's -- p.207's rule applies to every overlapping pair, "
            "not only adjacent ones. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI03
        arm_lo = false;  // MUTATION: sprite 0 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(0, 3, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-03: sprites 0 and 3 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 0's code is the one that reaches "
            "sprdata, not sprite 3's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI04
        arm_lo = false;  // MUTATION: sprite 0 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(0, 4, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-04: sprites 0 and 4 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 0's code is the one that reaches "
            "sprdata, not sprite 4's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI05
        arm_lo = false;  // MUTATION: sprite 0 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(0, 5, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-05: sprites 0 and 5 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 0's code is the one that reaches "
            "sprdata, not sprite 5's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI06
        arm_lo = false;  // MUTATION: sprite 0 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(0, 6, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-06: sprites 0 and 6 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 0's code is the one that reaches "
            "sprdata, not sprite 6's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI07
        arm_lo = false;  // MUTATION: sprite 0 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(0, 7, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-07: sprites 0 and 7 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 0's code is the one that reaches "
            "sprdata, not sprite 7's -- the two ends of the cascade. sol:7-video.c1, "
            "opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI13
        arm_lo = false;  // MUTATION: sprite 1 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(1, 3, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-13: sprites 1 and 3 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 1's code is the one that reaches "
            "sprdata, not sprite 3's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI14
        arm_lo = false;  // MUTATION: sprite 1 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(1, 4, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-14: sprites 1 and 4 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 1's code is the one that reaches "
            "sprdata, not sprite 4's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI15
        arm_lo = false;  // MUTATION: sprite 1 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(1, 5, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-15: sprites 1 and 5 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 1's code is the one that reaches "
            "sprdata, not sprite 5's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI16
        arm_lo = false;  // MUTATION: sprite 1 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(1, 6, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-16: sprites 1 and 6 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 1's code is the one that reaches "
            "sprdata, not sprite 6's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI17
        arm_lo = false;  // MUTATION: sprite 1 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(1, 7, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-17: sprites 1 and 7 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 1's code is the one that reaches "
            "sprdata, not sprite 7's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI24
        arm_lo = false;  // MUTATION: sprite 2 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(2, 4, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-24: sprites 2 and 4 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 2's code is the one that reaches "
            "sprdata, not sprite 4's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI25
        arm_lo = false;  // MUTATION: sprite 2 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(2, 5, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-25: sprites 2 and 5 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 2's code is the one that reaches "
            "sprdata, not sprite 5's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI26
        arm_lo = false;  // MUTATION: sprite 2 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(2, 6, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-26: sprites 2 and 6 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 2's code is the one that reaches "
            "sprdata, not sprite 6's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI27
        arm_lo = false;  // MUTATION: sprite 2 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(2, 7, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-27: sprites 2 and 7 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 2's code is the one that reaches "
            "sprdata, not sprite 7's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI35
        arm_lo = false;  // MUTATION: sprite 3 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(3, 5, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-35: sprites 3 and 5 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 3's code is the one that reaches "
            "sprdata, not sprite 5's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI36
        arm_lo = false;  // MUTATION: sprite 3 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(3, 6, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-36: sprites 3 and 6 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 3's code is the one that reaches "
            "sprdata, not sprite 6's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI37
        arm_lo = false;  // MUTATION: sprite 3 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(3, 7, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-37: sprites 3 and 7 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 3's code is the one that reaches "
            "sprdata, not sprite 7's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI46
        arm_lo = false;  // MUTATION: sprite 4 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(4, 6, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-46: sprites 4 and 6 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 4's code is the one that reaches "
            "sprdata, not sprite 6's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI47
        arm_lo = false;  // MUTATION: sprite 4 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(4, 7, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-47: sprites 4 and 7 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 4's code is the one that reaches "
            "sprdata, not sprite 7's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }
    {
        bool arm_lo = true;
#if MUT_PRI57
        arm_lo = false;  // MUTATION: sprite 5 (the lower of the pair) withheld.
#endif
        Meas m = measure_pair(5, 7, 0x40, /*lo_first=*/true, arm_lo, true);
        FirstActive fa = first_active(m);
        bool lower_wins = fa.hpos != -1 && (m.trace[fa.hpos] & 0x3) == 0x1;
        checks.expect(lower_wins,
            "PRI-57: sprites 5 and 7 armed together at the same horizontal start with "
            "distinguishable non-zero codes; sprite 5's code is the one that reaches "
            "sprdata, not sprite 7's. sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
    }

    // ---------------- INV-01, INV-67 ----------------
    // "You cannot change the relative priorities of the sprites" (p.207)
    // read as an invariance claim distinct from the static picture: not
    // just that the lower sprite wins, but that this does not depend on
    // WHICH of the two sprites' registers were written first. Two runs
    // with IDENTICAL final register contents, differing only in write
    // order, are compared to EACH OTHER -- not to a constant -- so a
    // hypothetical order-dependent bug (whichever sprite is armed LAST
    // wins, say) would show up as the two traces disagreeing, not as
    // either one failing to match a hand-picked expected byte.
    //
    // The graded condition is not fwd.trace == rev.trace, full equality of
    // all 512 internal sprdata samples across the sweep: p.207/209 state an
    // invariance about WHICH sprite wins the overlap, not about
    // denise_sprites.v's internal sample-for-sample state outside that
    // overlap. It checks the one thing the page claims: the same sprite
    // (the lower-numbered one) wins, at the same hpos, in both arm orders.
    // Full 512-sample equality is measured and printed (not required) --
    // see the record after INV-67 below.
    {
        Meas fwd = measure_pair(0, 1, 0x40, /*lo_first=*/true, true, true);
        bool arm_lo_rev = true;
#if MUT_INV01
        arm_lo_rev = false;  // MUTATION: sprite 0 withheld in the reverse-armed run only.
#endif
        Meas rev = measure_pair(0, 1, 0x40, /*lo_first=*/false, arm_lo_rev, true);
        FirstActive fa_fwd = first_active(fwd);
        FirstActive fa_rev = first_active(rev);
        bool same_winner = fa_fwd.hpos != -1 && fa_rev.hpos != -1 &&
                            fa_fwd.hpos == fa_rev.hpos &&
                            (fwd.trace[fa_fwd.hpos] & 0x3) == 0x1 &&
                            (rev.trace[fa_rev.hpos] & 0x3) == 0x1;
        checks.expect(same_winner,
            "INV-01: sprites 0 and 1 armed with IDENTICAL final register contents in "
            "two runs that differ only in which sprite's own four registers are "
            "WRITTEN first (sprite-0-then-1 vs sprite-1-then-0); sprite 0's code wins "
            "at the same hpos (fwd=" + std::to_string(fa_fwd.hpos) + ", rev=" +
            std::to_string(fa_rev.hpos) + ") in BOTH arm orders. 'You cannot change "
            "the relative priorities of the sprites. They will always appear on the "
            "screen with the lower-numbered sprites appearing in front of...the "
            "higher-numbered sprites' (p.207); 'Just as you cannot change the "
            "sequence of the four fingers on the one hand, neither can you change the "
            "relative priority of the sprites' (p.209). sol:7-video.c1, opus:7-vp.c1, "
            "opus:7-vp.c9 .");
        checks.note("INV-01-FULLTRACE-MEASURED: fwd.trace == rev.trace across all 512 "
            "hpos samples is " + std::string(fwd.trace == rev.trace ? "true" : "false") +
            ".");
    }
    {
        Meas fwd = measure_pair(6, 7, 0x40, /*lo_first=*/true, true, true);
        bool arm_lo_rev = true;
#if MUT_INV67
        arm_lo_rev = false;  // MUTATION: sprite 6 withheld in the reverse-armed run only.
#endif
        Meas rev = measure_pair(6, 7, 0x40, /*lo_first=*/false, arm_lo_rev, true);
        FirstActive fa_fwd = first_active(fwd);
        FirstActive fa_rev = first_active(rev);
        bool same_winner = fa_fwd.hpos != -1 && fa_rev.hpos != -1 &&
                            fa_fwd.hpos == fa_rev.hpos &&
                            (fwd.trace[fa_fwd.hpos] & 0x3) == 0x1 &&
                            (rev.trace[fa_rev.hpos] & 0x3) == 0x1;
        checks.expect(same_winner,
            "INV-67: sprites 6 and 7 armed with IDENTICAL final register contents in "
            "two runs that differ only in which sprite's own four registers are "
            "WRITTEN first (sprite-6-then-7 vs sprite-7-then-6); sprite 6's code wins "
            "at the same hpos (fwd=" + std::to_string(fa_fwd.hpos) + ", rev=" +
            std::to_string(fa_rev.hpos) + ") in BOTH arm orders. Same two "
            "quotations as INV-01, the opposite end of the cascade: 'You cannot "
            "change the relative priorities of the sprites...' (p.207); 'Just as you "
            "cannot change the sequence of the four fingers on the one hand, neither "
            "can you change the relative priority of the sprites' (p.209). "
            "sol:7-video.c1, opus:7-vp.c1, opus:7-vp.c9 .");
        checks.note("INV-67-FULLTRACE-MEASURED: fwd.trace == rev.trace across all 512 "
            "hpos samples is " + std::string(fwd.trace == rev.trace ? "true" : "false") +
            ".");
    }
    checks.record(
        "INV-01-FULLTRACE-NOT-GRADED, INV-67-FULLTRACE-NOT-GRADED (p.207, p.209): "
        "full 512-sample sprdata-trace equality between the two arm orders is not the "
        "graded condition; p.207/209 state an invariance about which sprite wins the "
        "overlap, not about denise_sprites.v's internal state at every other hpos. "
        "Measured and printed above (INV-0N-FULLTRACE-MEASURED); not required by "
        "expect(). reason_kind source-silent.");

    // --------- Declined: drawing-ink descriptions [no expect()] ---------
    // A drawing's ink is not a hardware signal (where the manual is
    // silent, measure and report -- do not assert).
    checks.record(
        "(half graded: opus:7-fig7-1.c1) [declined, drawing-inventory half]: 'the drawing...is a "
        "cascade of exactly eight identical rectangles...no ninth box and no other "
        "digit appears in the frame' counts and describes the printed drawing on "
        "p.208; it names no signal. (Its hardware half -- 'there are eight sprites "
        "and they are numbered 0 to 7' -- is asserted above as SPR-0..SPR-7.)");
    checks.record(
        "(half graded: opus:7-fig7-1.c2 through opus:7-fig7-1.c8) [declined, ink-description "
        "halves, 7 ids, p.208]: each describes which corners and edges of the rear "
        "box in Figure 7-1 are inked ('box N's lower-left corner and part of its "
        "left and bottom edges are not drawn, so only a strip at its right...remains "
        "visible'); this names no signal. Read directly from p208.png, the "
        "description is also inaccurate as a reading of the drawing: "
        "each rear box keeps its top band and top edge visible as well as the "
        "right-hand strip, matching the crossread's own figure-description-mismatch "
        "finding against all seven of these ids. (Each record's priority half is "
        "asserted above as one of PRI-01..PRI-67.)");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
