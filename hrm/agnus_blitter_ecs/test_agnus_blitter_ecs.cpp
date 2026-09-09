// Test purpose: Verify everything gated on the `ecs` port and observable at
//   the agnus_blitter boundary (packages.md item 13): BLTCON0L ($05A, writes
//   BLTCON0's lower 8 minterm bits, gated on ecs), BLTSIZH ($05E, an 11-bit
//   width field that starts the blit, gated on ecs) and BLTSIZV ($05C, a
//   15-bit height field), and DOFF (BLTCON1 bit 7, documented to disable
//   channel D output). The ECS pointer width (BLTxPTH/BLTxPTL) is explicitly
//   NOT this package's territory - packages.md item 13 says so in as many
//   words ("The ECS pointer width lives in PKG-BLT-ADRGEN, one fixture
//   down") - and is not reached for here.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 172, 189, 198-199, 267, 269-270, 308-310.
// Non-goals: the ECS pointer width, BLTxPTH/BLTxPTL (PKG-BLT-ADRGEN, per
//   packages.md item 13's own instruction not to reach for it here - see
//   "NOT ASSERTED - REDIRECTED" below); the OCS BLTSIZE zero-means-64/1024
//   encoding's own numeric values (6-appc-big-blits.c22's "1024-line
//   maximum") - that IS "zero meaning 64 and 1024", the exact phrase
//   packages.md item 8 assigns to PKG-BLT-SIZE, and is orthogonal to `ecs`
//   (the encoding is the same whichever chipset is selected); pixel-count
//   figures that require the manual's own word-to-pixel (x16) or the
//   width/height-to-area scaling ("32768 pixels", "32K by 32K pixels") -
//   this RTL exposes only a word-count field (width) and a line-count field
//   (height), never a pixel count, so converting either to "pixels" is the
//   programmer's own arithmetic, not something observable at this boundary
//   (see NOT ASSERTED below; the same treatment PKG-BLT-LINE gives the
//   dy/dx modulo-value formulas and PKG-BLT-SIZE's own "width in words
//   versus height in lines" framing already establishes the convention);
//   register read-back / write-only completeness for BLTCON0L, BLTSIZH,
//   BLTSIZV (PKG-BLT-REGDECODE's job - this module has no host-read
//   datapath for any register at all, so nothing here could show a register
//   behaving otherwise); minterm/LF decode correctness beyond using two
//   already-simple LF settings as plumbing to observe which one is active
//   (PKG-BLT-MINTERM); the barrel shifter's own arithmetic beyond reusing
//   its documented ascending-shift formula as a one-bit witness that ash
//   survived a BLTCON0L write (PKG-BLT-SHIFT, 4.2M-case sweep).
//
// NO PRIOR ART IS SUPERSEDED. docs/blitter-prior-art.md was read in full
// before writing this file, per the build brief's explicit instruction to
// say so rather than assume it. Its "eight blitter tests" table lists
// barrelshifter, fill, minterm, modulo, ptr, line_octant, mask and zero -
// none of the eight compiles agnus_blitter with ecs driven both ways, and
// none asserts anything about BLTCON0L, BLTSIZH, BLTSIZV or DOFF. This is a
// genuinely new directory with no predecessor to confirm, extend or
// supersede.
//
// THE INVENTORY'S OWN EVIDENCE TEXT FOR BLTSIZV IS WRONG, AND THE BRIEF
// (BACKED BY THE RTL AND BY docs/findings.md) WINS. Every classified.jsonl
// record whose claim mentions BLTSIZV (6-appa-registers.c70/.c71,
// 6-appc-modifications.c12, ch6-appc-map.bltsizv, ch6-appc-bltsizv.c1/.h,
// 6-appc-big-blits.c65, ch6-appc-bigblits.c8, and every BLTSIZV bit-table
// entry) carries the SAME `evidence` sentence: "BLTSIZV loads height[14:0]
// ... both only when ecs is high" - stated as fact, not as a claim under
// test. Reading agnus_blitter.v directly (done before writing a line of
// this file, per the build brief's order of work) shows this is false:
//
//     agnus_blitter.v:546-550
//       if (reset)
//         height[14:0] <= 0;
//       else if (reg_address_in[8:1]==BLTSIZV[8:1]) // ECS BLTSIZV register
//         height[14:0] <= data_in[14:0];
//
// There is no `&& ecs` on that branch - contrast BLTSIZH's own decode two
// lines below it (agnus_blitter.v:523-524, `... && ecs`), which does have
// one, and which busy's own gate (line 500) also has. This is exactly the
// finding already recorded in docs/findings.md, "Chapter 6 - read from the
// RTL, no test yet" -> "Appendix C's five ECS blitter changes are
// implemented five different ways": "BLTSIZV $05C new - V size, 15-bit |
// line 548 | no - and the comment on that line calls it ECS". The
// classifier's evidence field appears to have copied BLTSIZH's genuinely-
// gated description onto BLTSIZV's claims without rechecking line 548
// itself. BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS below tests what the
// manual's own "new" marking for this register implies (p309/310: BLTSIZV
// is new to ECS, so an ecs=0 write should not behave like an ecs=1 write)
// and is REQUIRED to go red, per findings.md and per this task's own
// instruction not to make it pass.
//
// DOFF is the second required-red finding, same section of findings.md:
// "bltcon1[7] does not appear in agnus_blitter.v. The bits it reads are
// 15-12, 4, 3, 2, 1 and 0." Confirmed again while writing this file:
// agnus_blitter.v:277-283 stores bltcon1[11:0] on every BLTCON1 write with
// no per-bit exception, and no other expression in the file reads
// `bltcon1[7]` for anything. DOFF-NO-OBSERVABLE-EFFECT below is REQUIRED to
// go red for the same reason.
//
// CLAIM COVERAGE. All 135 records of classify.package=="PKG-BLT-ECS" and
// classify.status=="testable" in work/2026-09-08_fpga-090_classified.jsonl
// are covered below, either by an assertion or under NOT ASSERTED with a
// reason - verified by script (a claim-id set built from the same filter,
// diffed against every id cited anywhere in this file) before this header
// was finalized, so the count is not a hand tally. Both readers (sol/opus)
// cover much of the same ground - the appendix-C BLTSIZH/BLTSIZV bit tables
// in particular are stated by both readers, one claim per bit, so many ids
// below are the same proposition stated twice or split across a functional/
// reserved-bit boundary. Every id is written out in full, never a
// shared-prefix shorthand - two packages before this one had to redo their
// citations for exactly that shortcut. Assertion name -> covered ids:
//
//   TEST-DATA-DELIVERY-ECS: fixture check, no claim id - reproduces
//     hrm/agnus_blitter_status's TEST-DATA-DELIVERY through this file's own
//     Harness before anything data-dependent is asserted, per the build
//     brief.
//
//   BLTCON0L-GATED-BY-ECS (p269, p309): 6-appa-registers.c25,
//     6-appa-registers.c26, 6-appa-registers.c27, ch6-appa-bltcon0l.c3,
//     ch6-appc-map.bltcon0l, 6-appc-modifications.c11, ch6-appc-ecs.c9 .
//
//   BLTCON0L-WRITES-LOW-8-BITS-ONLY (p269, p309): 6-appa-registers.c28,
//     6-appc-modifications.c8, ch6-appc-ecs.c7 .
//
//   BLTSIZH-GATED-BY-ECS (p172, p189, p269, p308, p310):
//     6-appa-registers.c73, 6-appa-registers.c74, 6-appc-modifications.c13,
//     ch6-appc-map.bltsizh, ch6-appc-bltsizh.c1, 6-appc-big-blits.c25,
//     ch6-appc-bigblits.c5, 6-dma-channels.c66, ch6-dma.c68, ch6-line.c7 .
//
//   BLTSIZH-STARTS-BLIT (p269, p308): 6-appa-registers.c75,
//     ch6-appa-bltsize.h2, 6-appc-big-blits.c66, ch6-appc-bigblits.c9 .
//
//   BLTSIZH-WIDTH-TOP-BIT-FUNCTIONAL (p269, p308 - the functional half of
//     the bit table, bits 10 down to 0): 6-appc-big-blits.c49,
//     6-appc-big-blits.c50, 6-appc-big-blits.c51, 6-appc-big-blits.c52,
//     6-appc-big-blits.c53, 6-appc-big-blits.c54, 6-appc-big-blits.c55,
//     6-appc-big-blits.c56, 6-appc-big-blits.c57, 6-appc-big-blits.c58,
//     6-appc-big-blits.c59, ch6-appc-bltsizh.bit00, ch6-appc-bltsizh.bit01,
//     ch6-appc-bltsizh.bit02, ch6-appc-bltsizh.bit03, ch6-appc-bltsizh.bit04,
//     ch6-appc-bltsizh.bit05, ch6-appc-bltsizh.bit06, ch6-appc-bltsizh.bit07,
//     ch6-appc-bltsizh.bit08, ch6-appc-bltsizh.bit09, ch6-appc-bltsizh.bit10,
//     6-appc-big-blits.c60, ch6-appc-bltsizh.w (the 11-bit half only - see
//     NOT ASSERTED for the pixel-count half), 6-appa-registers.c75,
//     ch6-appa-bltsize.h2 (the 11-bit-field half of a claim already cited
//     whole under BLTSIZH-STARTS-BLIT for its "starts the blit" half - the
//     source claim states both, so it is cited under both assertions it
//     actually supports, not bundled into one).
//
//   BLTSIZH-WIDTH-UPPER-BITS-IGNORED (p308 - the reserved half of the bit
//     table, bits 15 down to 11): 6-appc-big-blits.c44, 6-appc-big-blits.c45,
//     6-appc-big-blits.c46, 6-appc-big-blits.c47, 6-appc-big-blits.c48,
//     ch6-appc-bltsizh.bit11, ch6-appc-bltsizh.bit12, ch6-appc-bltsizh.bit13,
//     ch6-appc-bltsizh.bit14, ch6-appc-bltsizh.bit15 .
//
//   BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS *** REQUIRED RED, SEE ABOVE ***
//     (p269, p308, p310): 6-appc-modifications.c12, ch6-appc-map.bltsizv,
//     6-appa-registers.c70, 6-appa-registers.c71, ch6-appc-bltsizv.c1,
//     6-appc-big-blits.c65, ch6-appc-bigblits.c8 .
//
//   BLTSIZV-HEIGHT-TOP-BIT-FUNCTIONAL (p269, p308 - the functional half of
//     the bit table, bits 14 down to 0): 6-appc-big-blits.c27,
//     6-appc-big-blits.c28, 6-appc-big-blits.c29, 6-appc-big-blits.c30,
//     6-appc-big-blits.c31, 6-appc-big-blits.c32, 6-appc-big-blits.c33,
//     6-appc-big-blits.c34, 6-appc-big-blits.c35, 6-appc-big-blits.c36,
//     6-appc-big-blits.c37, 6-appc-big-blits.c38, 6-appc-big-blits.c39,
//     6-appc-big-blits.c40, 6-appc-big-blits.c41, ch6-appc-bltsizv.bit00,
//     ch6-appc-bltsizv.bit01, ch6-appc-bltsizv.bit02, ch6-appc-bltsizv.bit03,
//     ch6-appc-bltsizv.bit04, ch6-appc-bltsizv.bit05, ch6-appc-bltsizv.bit06,
//     ch6-appc-bltsizv.bit07, ch6-appc-bltsizv.bit08, ch6-appc-bltsizv.bit09,
//     ch6-appc-bltsizv.bit10, ch6-appc-bltsizv.bit11, ch6-appc-bltsizv.bit12,
//     ch6-appc-bltsizv.bit13, ch6-appc-bltsizv.bit14, 6-appc-big-blits.c42,
//     6-appc-big-blits.c43, ch6-appc-bltsizv.h, 6-appa-registers.c72,
//     ch6-appa-bltsize.v2 .
//
//   BLTSIZV-HEIGHT-UPPER-BIT-IGNORED (p308 - the reserved half, bit 15):
//     6-appc-big-blits.c26, ch6-appc-bltsizv.bit15 .
//
//   BACKWARD-COMPAT-BLTSIZE-WORKS-WITH-ECS (p308): 6-appc-big-blits.c64,
//     ch6-appc-bigblits.c7 .
//
//   DOFF-NO-OBSERVABLE-EFFECT *** REQUIRED RED, SEE ABOVE *** (p198, p199,
//     p267, p309, p310): 6-appc-modifications.c4, 6-appc-modifications.c5,
//     6-appc-modifications.c6, 6-appc-modifications.c7,
//     6-appc-modifications.c9, ch6-appc-ecs.c4, ch6-appc-ecs.c6,
//     ch6-appc-map.bltcon1, 6-appa-control.c18,
//     ch6-appa-bltcon0.area.b07.con1, 6-appa-registers.c20,
//     ch6-appa-bltcon0.c6, ch6-blockdiag.c10, 6-blitter-block-diagram.c11 .
//     (c6/c7's "channels A, B and C can still supply inputs" and "certain
//     address modification without writing D" describe consequences of the
//     SAME single bit that c5/ecs.c4 describe (bltcon1[7], disables D
//     output) - since bltcon1[7] is read nowhere in agnus_blitter.v, no
//     measurement can distinguish "does nothing to D" from "does nothing to
//     D specifically while leaving A/B/C alone", so testing whether ANY
//     aspect of a real blit's behaviour differs with bit 7 set is the
//     complete, decisive test of all of them together - the same "entailed,
//     not separable" reasoning PKG-BLT-STATUS used for Z-NEVER-RESTORED.)
//
// NOT ASSERTED - REDIRECTED (reported, not silently dropped):
//   6-appa-registers.c109 (p270, "BLTxPTL implements the pointer's low 15
//     bits"), ch6-appa-bltxpt.c5 (p270, same claim, opus) - pointer width.
//     packages.md item 13 says outright: "The ECS pointer width lives in
//     PKG-BLT-ADRGEN, one fixture down" - this package does not reach for
//     it. hrm/agnus_blitter_adrgen/test_agnus_blitter_adrgen.cpp's
//     PTR-TWO-WORD-COMPOSITION assertion already measures exactly this (a
//     BLTAPTL write alone leaves the high word - the pointer's upper bits -
//     unchanged), even though it does not cite these two ids by name; this
//     is a redirect to already-built, already-green territory, not a gap.
//   6-dma-channels.c67 (p172, "larger pointer-register range"), ch6-dma.c69
//     (p172, same, opus) - same redirect, general chapter-body statement of
//     the same pointer-width fact.
//   6-appc-big-blits.c22 (p308, "Original-chip-set BLTSIZE height has a
//     1024-line maximum") - this is OCS's own zero-means-1024 height
//     encoding, orthogonal to `ecs` (the encoding is identical regardless of
//     chipset selected) and is literally the phrase packages.md item 8
//     assigns to PKG-BLT-SIZE: "zero meaning 64 and 1024". Per this task's
//     own rule ("if this brief and packages.md or the inventory disagree,
//     the document wins and you say so"): packages.md item 8's description
//     governs this specific numeric claim's content even though
//     classify.package filed it under PKG-BLT-ECS; not reached for here.
//
// NOT ASSERTED - PIXEL/WORD UNIT CONVERSION (absent_signal: a pixel-count
//   port; reason_kind: absent - this RTL exposes only a word-count field
//   (width) and a line-count field (height), never a pixel count; the
//   manual's own arithmetic converts words to pixels at 16 pixels/word for
//   the WIDTH dimension, which is exactly why "11 bit width" (6-appc-big-
//   blits.c60, testable, tested above) sits beside "= 32768 pixels max"
//   (c61, not testable) as two different units for the same fact - the
//   same non-goal PKG-BLT-LINE already established for the dy/dx modulo
//   formulas and PKG-BLT-SIZE's own "width in words versus height in
//   lines" framing):
//   6-appc-big-blits.c1 (p308, "32768 pixels wide"), 6-appc-big-blits.c2
//     (p308, "32768 pixels high" - HEIGHT stated in pixels too, in a joint
//     V+H sentence that does not separate the two dimensions' units),
//   6-appc-big-blits.c24 (p308, "Original-chip-set BLTSIZE width has a
//     1024-pixel maximum" - OCS's own 64-word zero-encoding times 16
//     pixels/word), 6-appc-big-blits.c61 (p308, "32768 pixels of width"),
//   6-appc-big-blits.c62 (p308, "32K pixels wide", joint V+H),
//   6-appc-big-blits.c63 (p308, "32K pixels high", joint V+H),
//   ch6-appc-bigblits.c2 (p308, "32768 x 32768 pixels"), ch6-appc-bigblits.c6
//     (p308, "32K by 32K", joint V+H), 6-dma-channels.c69 (p172, "32K by 32K
//     pixels"), ch6-dma.c70 (p172, same, opus).
//
// NOT ASSERTED - PROSE / NOTATION (absent_signal: none; reason_kind:
//   insufficient - no hardware behaviour, nothing to name a signal for):
//   ch6-appc-ecs.c8 (p309, "expediting the set up of some blits ... speeds
//     up the software" - a programming-convenience justification, not a
//     hardware behaviour); ch6-appc-map.legend (p310, "A=Agnus chip,
//     D=Denise chip, P=Paula chip, W=Write, R=Read, S=Strobe" - the register
//     map's own notation legend, the same treatment packages.md gives
//     "the book's own notation conventions (p175)").
//
// FIXTURE. Started from the working harness in hrm/agnus_blitter_status/
// (the only agnus_blitter fixture in the tree proven to deliver data
// correctly) - TEST-DATA-DELIVERY-ECS below reproduces its
// TEST-DATA-DELIVERY assertion first, before anything data-dependent, per
// the build brief. Harness::cycle's DMA-grant handshake (the settling
// eval() before the clock edge) is the same mechanism as agnus_blitter_
// status, agnus_blitter_masks, agnus_blitter_shift_chain and
// agnus_blitter_line, all read before writing this file. `ecs` is driven
// per-Harness-instance (set at construction, changeable mid-run via
// set_ecs()) so every gated assertion below drives it BOTH ways - a check
// that only ever sees ecs=1 cannot show a gate exists.
//
// Traps from agnus_blitter_status's header that this file's assertions
// actually touch:
//   1. `we`/`data_out` are transient, sampled at the grant (Harness::cycle),
//      never polled after the tick - this file uses the same capture point.
//   2. BLTAFWM/BLTALWM default to 0 after reset and mask channel A
//      unconditionally on first/last word (agnus_blitter.v:311-319) - every
//      block below that drives channel A writes both to 0xFFFF first.
//   3/4/5. busy/int3/zero completion timing traps do not apply here: this
//      file never reads `zero` or `int3`, and every busy-based check either
//      samples busy immediately after the triggering write (no settle
//      window needed) or waits for run_until_done()'s registered fall,
//      which is not subject to the transient-sampling trap the way
//      `we`/`data_out` are.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean, confirmed byte-identical to the baseline by
// `git diff --stat` after every revert) and the PASS/FAIL lines quoted are
// what the run printed, not predictions. Every mutation is a STIMULUS
// mutation to this test file's OWN inputs (a register value, an `ecs`
// setting, a preload) - never to nanomig/, which this task forbids
// touching for any reason.
//
// MUTATION TEST-DATA-DELIVERY-ECS stimulus red: fed C word at index 2
//   changed from 0x9ABC to 0x9ABD (expected constants left untouched) ->
//   FAIL, only this assertion; every other assertion PASS (or, for the two
//   required-red assertions, stays red for its own documented reason,
//   unaffected by this unrelated fixture's data).
// MUTATION BLTCON0L-GATED-BY-ECS stimulus red: h_on's `ecs` changed from
//   true to false at construction (so BOTH harnesses now run with ecs=0) ->
//   FAIL - h_on's BLTCON0L write is now also ignored, so its D word reads
//   back as the untouched C-passthrough value (0x2222) instead of the
//   A-passthrough value (0x1111) the assertion expects; every other
//   assertion PASS.
// MUTATION BLTCON0L-WRITES-LOW-8-BITS-ONLY stimulus red: the ash value
//   preloaded into BLTCON0 before the BLTCON0L write changed from 1 to 0
//   (expected constant 0x4000 left untouched) -> FAIL, got 0x8000 (the
//   unshifted value) instead of 0x4000 - with ash already 0, the BLTCON0L
//   write cannot be shown to have left it alone, because there is nothing
//   left to disturb; every other assertion PASS.
// MUTATION BLTSIZH-GATED-BY-ECS stimulus red: h_on's `ecs` changed from
//   true to false (so BOTH harnesses now run with ecs=0) -> FAIL - busy
//   never sets in h_on either, so `on_busy_immediate` reads false; every
//   other assertion PASS.
// MUTATION BLTSIZH-STARTS-BLIT stimulus red: the width written via BLTSIZH
//   changed from 3 to 5 (expected constant 3 left untouched) -> FAIL, 5
//   writes occur, not 3; every other assertion PASS.
// MUTATION BLTSIZH-WIDTH-TOP-BIT-FUNCTIONAL stimulus red: the width probe
//   changed from 1024 (bit 10) to 512 (expected constant 1024 left
//   untouched) -> FAIL, 512 writes occur, not 1024; every other assertion
//   PASS.
// MUTATION BLTSIZH-WIDTH-UPPER-BITS-IGNORED stimulus red: the written value
//   changed from (0x800|7) to (0x800|9) (expected constant 7 left
//   untouched) -> FAIL, 9 writes occur, not 7; every other assertion PASS.
// MUTATION BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS stimulus, diagnosed rather
//   than simply reported as "stayed red" per rule 3: the height value
//   written in BOTH the ecs=0 and the ecs=1-control branches changed from 7
//   to 9 (same value in both branches, as the original) -> both branches'
//   measured row count become 9 (not 7), CONFIRMING the harness's counting
//   mechanism genuinely tracks the height value written (not a stuck or
//   hardcoded readout) - but the two branches remain EQUAL to each other (9
//   == 9), so the assertion (which checks the two branches DIFFER) still
//   FAILS, for the same reason it failed before the mutation: nothing in
//   agnus_blitter.v reads `ecs` on BLTSIZV's own decode (line 548), so no
//   value of the height stimulus can make the two branches diverge. This is
//   the correct shape for a mutation against a genuinely absent gate, not a
//   vacuous assertion - see BLTSIZH-GATED-BY-ECS above, which drives the
//   IDENTICAL two-branch comparison structure against a register that DOES
//   gate on ecs and DOES go green, proving the comparison mechanism itself
//   is sound.
// MUTATION BLTSIZV-HEIGHT-TOP-BIT-FUNCTIONAL stimulus red: the height probe
//   changed from 16384 (bit 14) to 8192 (expected constant 16384 left
//   untouched) -> FAIL, 8192 writes occur, not 16384; every other assertion
//   PASS.
// MUTATION BLTSIZV-HEIGHT-UPPER-BIT-IGNORED stimulus red: the written value
//   changed from (0x8000|7) to (0x8000|9) (expected constant 7 left
//   untouched) -> FAIL, 9 writes occur, not 7; every other assertion PASS.
// MUTATION BACKWARD-COMPAT-BLTSIZE-WORKS-WITH-ECS stimulus red: the OCS
//   BLTSIZE width changed from 3 to 5 (expected constant 3 left untouched,
//   `ecs` left at 1) -> FAIL, 5 writes occur, not 3; every other assertion
//   PASS. (Toggling `ecs` itself was tried first and rejected as the
//   mutation: BLTSIZE's decode, agnus_blitter.v:500,521-522,555-556, has no
//   `&& ecs` anywhere, so it is unconditional either way and flipping `ecs`
//   changes nothing about this particular assertion - a mutation that
//   provably cannot move is not a stimulus mutation, so the width value is
//   the correct lever instead.)
// MUTATION DOFF-NO-OBSERVABLE-EFFECT stimulus, diagnosed per rule 3: USED
//   (BLTCON0 bit 8) removed from BOTH h_doff0 and h_doff1's BLTCON0 write,
//   with everything else (including BLTCON1 bit 7) held fixed -> the
//   assertion, which checks h_doff1.data_writes.empty(), now PASSES (goes
//   GREEN): with the D channel genuinely disabled by a real, working gate
//   (`used`, agnus_blitter.v:252,490-491), NEITHER harness ever writes, so
//   "no D writes occurred" becomes true for a real reason instead of a
//   coincidence. Reverting to the baseline (USED restored, only BLTCON1 bit
//   7 differs between h_doff0/h_doff1) returns the assertion to FAIL/red -
//   confirming the comparison mechanism is sound and DOES detect a real
//   D-disable condition when one exists; it is specifically insensitive to
//   bit 7, which is the finding.

#include <cstdint>
#include <cstdio>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t BLTCON0  = to_reg(0x040);
constexpr uint16_t BLTCON1  = to_reg(0x042);
constexpr uint16_t BLTAFWM  = to_reg(0x044);
constexpr uint16_t BLTALWM  = to_reg(0x046);
constexpr uint16_t BLTCON0L = to_reg(0x05A);
constexpr uint16_t BLTSIZV  = to_reg(0x05C);
constexpr uint16_t BLTSIZH  = to_reg(0x05E);
constexpr uint16_t BLTCDAT  = to_reg(0x070);
constexpr uint16_t BLTBDAT  = to_reg(0x072);
constexpr uint16_t BLTADAT  = to_reg(0x074);
constexpr uint16_t BLTSIZE  = to_reg(0x058);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252.
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

constexpr uint16_t DOFF_BIT = 1u << 7;  // BLTCON1 bit 7, appendix C p.309

// Minterm LF settings (agnus_blitter_minterm.v), used strictly as plumbing
// to observe which one is active - minterm correctness itself is
// PKG-BLT-MINTERM's job.
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C
constexpr uint16_t LF_A_PASSTHROUGH = 0xF0;  // D = A

// BLTSIZE data word for OCS-style sizing: height in bits[15:6], width in
// bits[5:0] (agnus_blitter.v:521-525,555-556).
constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

// Copied in mechanism from hrm/agnus_blitter_status (the DMA-grant
// second-eval() fix), read in full before writing this file. `ecs` is a
// per-instance constructor parameter, changeable mid-run via set_ecs() -
// needed by BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS, which must write
// BLTSIZV under one ecs value and BLTSIZH (the only register that can
// start a blit) under another within the same run.
class Harness {
public:
    explicit Harness(bool ecs_initial) : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = ecs_initial ? 1 : 0;
        dut->clkena = 1;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        data_writes.clear();
        tick_count = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void set_ecs(bool v) { dut->ecs = v ? 1 : 0; }

    void write_reg(uint16_t addr, uint16_t data) { cycle(true, addr, data); tick_bookkeeping(); }

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    bool run_until_writes(size_t n, int max_ticks) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    // Runs until busy falls from 1 to 0 (a completed blit), or max_ticks is
    // exhausted. Returns true if a completed fall was observed.
    bool run_until_done(int max_ticks) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            const bool busy_now = static_cast<bool>(dut->busy);
            if (busy_now) seen_busy = true;
            if (seen_busy && !busy_now) return true;
        }
        return false;
    }

    // Idle margin: busy/int3 signal completion BEFORE the pipeline has
    // flushed the last D-channel word (agnus_blitter_status's header,
    // "Fourth" trap - measured again here: for a 3-word blit, busy falls at
    // tick 16 but the 3rd write does not land until tick 18). Run a few
    // idle cycles before trusting data_writes.size() as final.
    void settle(int n = 6) { run_cycles(n); }

    bool busy() const { return dut->busy; }

    std::deque<uint16_t> a_words, c_words;
    std::vector<uint16_t> data_writes;
    int tick_count = 0;

private:
    Vagnus_blitter* dut;

    static uint16_t pop_or_zero(std::deque<uint16_t>& q) {
        if (q.empty()) return 0;
        uint16_t v = q.front();
        q.pop_front();
        return v;
    }

    bool last_we = false;
    uint16_t last_data_out = 0;

    void tick_bookkeeping() {
        if (last_we) data_writes.push_back(last_data_out);
        ++tick_count;
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        last_we = false;
        last_data_out = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            if (reg == BLTADAT) v = pop_or_zero(a_words);
            else if (reg == BLTCDAT) v = pop_or_zero(c_words);
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the combinational
            // network BEFORE the clock edge - the step the salvaged
            // diag.cpp harness skipped (see hrm/agnus_blitter_status's
            // header). we/data_out are transient and only valid here.
            dut->eval();
            if (dut->we) {
                last_we = true;
                last_data_out = dut->data_out;
            }
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = IDLE_REG;
            dut->data_in = 0;
        }
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY-ECS (fixture check, no page) ----------------
    {
        Harness h(false);
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(4, 1));
        h.run_until_writes(4, 60);

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY-ECS: four distinct C words fed into the "
            "fixture arrive at data_out, in order, at the we cycles they were fetched on - "
            "proves the DMA grant handshake actually delivers data, which every "
            "data-dependent assertion below depends on. Reproduced from "
            "hrm/agnus_blitter_status's TEST-DATA-DELIVERY first, per the build brief, "
            "before anything data-dependent.");
    }

    // ---------------- BLTCON0L-GATED-BY-ECS (p.269, p.309) ----------------
    {
        Harness h_off(false);
        h_off.a_words = {0x1111};
        h_off.c_words = {0x2222};
        h_off.write_reg(BLTAFWM, 0xFFFF);
        h_off.write_reg(BLTALWM, 0xFFFF);
        h_off.write_reg(BLTCON0, USEA | USEC | USED | LF_C_PASSTHROUGH);
        h_off.write_reg(BLTCON1, 0x0000);
        h_off.write_reg(BLTCON0L, LF_A_PASSTHROUGH);  // ecs=0: attempt to switch LF to A-passthrough
        h_off.write_reg(BLTSIZE, bltsize_word(1, 1));
        h_off.run_until_writes(1, 60);

        Harness h_on(true);
        h_on.a_words = {0x1111};
        h_on.c_words = {0x2222};
        h_on.write_reg(BLTAFWM, 0xFFFF);
        h_on.write_reg(BLTALWM, 0xFFFF);
        h_on.write_reg(BLTCON0, USEA | USEC | USED | LF_C_PASSTHROUGH);
        h_on.write_reg(BLTCON1, 0x0000);
        h_on.write_reg(BLTCON0L, LF_A_PASSTHROUGH);  // ecs=1: same write, should take effect
        h_on.write_reg(BLTSIZE, bltsize_word(1, 1));
        h_on.run_until_writes(1, 60);

        checks.expect(
            h_off.data_writes.size() == 1 && h_off.data_writes[0] == 0x2222 &&
            h_on.data_writes.size() == 1 && h_on.data_writes[0] == 0x1111,
            "BLTCON0L-GATED-BY-ECS: with BLTCON0's LF field left at C-passthrough (0xAA) by "
            "the initial BLTCON0 write, a subsequent BLTCON0L write of 0xF0 (A-passthrough) "
            "has NO effect when ecs=0 (D reads back the untouched C value, 0x2222) and DOES "
            "take effect when ecs=1 (D reads back the A value, 0x1111) - "
            "reg_address_in[8:1]==BLTCON0L[8:1] && ecs, agnus_blitter.v:259-260. "
            "6-appa-registers.c25, 6-appa-registers.c26, 6-appa-registers.c27, "
            "ch6-appa-bltcon0l.c3, ch6-appc-map.bltcon0l, 6-appc-modifications.c11, "
            "ch6-appc-ecs.c9 .");
    }

    // ---------------- BLTCON0L-WRITES-LOW-8-BITS-ONLY (p.269, p.309) ----------------
    {
        Harness h(true);
        h.a_words = {0x8000};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        // ash=1 (bltcon0[15:12]) preloaded via the ordinary BLTCON0 write.
        h.write_reg(BLTCON0, static_cast<uint16_t>((1u << 12) | USEA | USEC | USED | LF_C_PASSTHROUGH));
        h.write_reg(BLTCON1, 0x0000);
        // Change ONLY the low byte via BLTCON0L; ash (bits 15:12) should survive untouched.
        h.write_reg(BLTCON0L, LF_A_PASSTHROUGH);
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_until_writes(1, 60);

        // Barrel shifter, ascending mode, shift=1, previous=0 (first word), current=0x8000:
        // shiftaout = (0 << 15) | (0x8000 >> 1) = 0x4000. If ash had been reset to 0 by the
        // BLTCON0L write, shiftaout would instead equal the unshifted 0x8000.
        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x4000,
            "BLTCON0L-WRITES-LOW-8-BITS-ONLY: with ash preloaded to 1 via BLTCON0, a "
            "BLTCON0L write that changes the LF field to A-passthrough still shows A "
            "shifted by 1 (D=0x4000, not the unshifted 0x8000) - bltcon0[7:0] alone is "
            "written by BLTCON0L (agnus_blitter.v:259-260), bltcon0[15:12] (ash) is untouched "
            "by that decode and is written only by the ordinary BLTCON0 path "
            "(agnus_blitter.v:219-230). 6-appa-registers.c28, 6-appc-modifications.c8, "
            "ch6-appc-ecs.c7 .");
    }

    // ---------------- BLTSIZH-GATED-BY-ECS (p.172, p.189, p.269, p.308, p.310) ----------------
    {
        Harness h_off(false);
        h_off.write_reg(BLTSIZV, 1);  // height=1; loads regardless of ecs (see header note)
        h_off.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h_off.write_reg(BLTCON1, 0x0000);
        h_off.write_reg(BLTSIZH, 3);  // ecs=0: should have no effect
        h_off.run_cycles(30);
        const bool off_busy = h_off.busy();
        const bool off_any_writes = !h_off.data_writes.empty();

        Harness h_on(true);
        h_on.write_reg(BLTSIZV, 1);
        h_on.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h_on.write_reg(BLTCON1, 0x0000);
        h_on.write_reg(BLTSIZH, 3);  // ecs=1: should set busy and start the blit
        h_on.run_cycles(1);
        const bool on_busy_immediate = h_on.busy();
        h_on.run_until_done(60);
        h_on.settle();  // busy falls before the last D-word lands - see Harness::settle
        const bool on_completed = h_on.data_writes.size() == 3;

        checks.expect(!off_busy && !off_any_writes && on_busy_immediate && on_completed,
            "BLTSIZH-GATED-BY-ECS: writing BLTSIZH (with BLTSIZV already holding height=1) "
            "sets busy and produces a 3-word blit when ecs=1, and does neither - busy stays "
            "0, no D writes ever occur - when ecs=0. reg_address_in[8:1]==BLTSIZH[8:1] && "
            "ecs, agnus_blitter.v:500 (busy), :523-524 (width). 6-appa-registers.c73, "
            "6-appa-registers.c74, 6-appc-modifications.c13, ch6-appc-map.bltsizh, "
            "ch6-appc-bltsizh.c1, 6-appc-big-blits.c25, ch6-appc-bigblits.c5, "
            "6-dma-channels.c66, ch6-dma.c68, ch6-line.c7 .");
    }

    // ---------------- BLTSIZH-STARTS-BLIT (p.269, p.308) ----------------
    {
        Harness h(true);
        h.write_reg(BLTSIZV, 1);
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZH, 3);
        h.run_until_done(60);
        h.settle();  // busy falls before the last D-word lands - see Harness::settle

        checks.expect(h.data_writes.size() == 3,
            "BLTSIZH-STARTS-BLIT: writing BLTSIZH (width=3, ecs=1, height already 1 via "
            "BLTSIZV) runs the FSM to completion and produces exactly 3 destination writes - "
            "BLTSIZH genuinely starts the blitter, the same role BLTSIZE plays for OCS. "
            "6-appa-registers.c75, ch6-appa-bltsize.h2, 6-appc-big-blits.c66, "
            "ch6-appc-bigblits.c9 .");
    }

    // ---------------- BLTSIZH-WIDTH-TOP-BIT-FUNCTIONAL (p.269, p.308) ----------------
    {
        Harness h(true);
        h.write_reg(BLTSIZV, 1);
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZH, 1024);  // bit 10, the field's top functional bit
        const bool done = h.run_until_done(6000);
        h.settle();  // busy falls before the last D-word lands - see Harness::settle

        checks.expect(done && h.data_writes.size() == 1024,
            "BLTSIZH-WIDTH-TOP-BIT-FUNCTIONAL: writing width=1024 (bit 10 of BLTSIZH's "
            "width[10:0]) produces exactly 1024 destination writes - bit 10, the top of the "
            "11-bit field, is functional, distinctly beyond OCS BLTSIZE's own 6-bit/"
            "zero-means-64 ceiling. agnus_blitter.v:523-524 "
            "(`width[10:0] <= data_in[10:0]`). 6-appc-big-blits.c49, 6-appc-big-blits.c50, "
            "6-appc-big-blits.c51, 6-appc-big-blits.c52, 6-appc-big-blits.c53, "
            "6-appc-big-blits.c54, 6-appc-big-blits.c55, 6-appc-big-blits.c56, "
            "6-appc-big-blits.c57, 6-appc-big-blits.c58, 6-appc-big-blits.c59, "
            "ch6-appc-bltsizh.bit00, ch6-appc-bltsizh.bit01, ch6-appc-bltsizh.bit02, "
            "ch6-appc-bltsizh.bit03, ch6-appc-bltsizh.bit04, ch6-appc-bltsizh.bit05, "
            "ch6-appc-bltsizh.bit06, ch6-appc-bltsizh.bit07, ch6-appc-bltsizh.bit08, "
            "ch6-appc-bltsizh.bit09, ch6-appc-bltsizh.bit10, 6-appc-big-blits.c60, "
            "ch6-appc-bltsizh.w, 6-appa-registers.c75, ch6-appa-bltsize.h2 .");
    }

    // ---------------- BLTSIZH-WIDTH-UPPER-BITS-IGNORED (p.308) ----------------
    {
        Harness h(true);
        h.write_reg(BLTSIZV, 1);
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZH, static_cast<uint16_t>(0x0800 | 7));  // bit 11 set, field value 7
        const bool done = h.run_until_done(200);
        h.settle();  // busy falls before the last D-word lands - see Harness::settle

        checks.expect(done && h.data_writes.size() == 7,
            "BLTSIZH-WIDTH-UPPER-BITS-IGNORED: writing 0x0807 (bit 11 set, low 11 bits = 7) "
            "produces exactly 7 destination writes, not 2055 - bits 15:11 are outside "
            "width[10:0] and have no effect. agnus_blitter.v:523-524 "
            "(`width[10:0] <= data_in[10:0]`, an 11-bit slice of a 16-bit data_in). "
            "6-appc-big-blits.c44, 6-appc-big-blits.c45, 6-appc-big-blits.c46, "
            "6-appc-big-blits.c47, 6-appc-big-blits.c48, ch6-appc-bltsizh.bit11, "
            "ch6-appc-bltsizh.bit12, ch6-appc-bltsizh.bit13, ch6-appc-bltsizh.bit14, "
            "ch6-appc-bltsizh.bit15 .");
    }

    // ---------------- BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS (p.269, p.308, p.310) ----------------
    // *** REQUIRED RED - see file header. Do not adjust this assertion to pass. ***
    {
        // Branch 1: BLTSIZV written while ecs=0, then ecs raised only to write BLTSIZH
        // (the only register that can start a blit and copy height into height_cnt).
        Harness h1(false);
        h1.write_reg(BLTSIZV, 7);          // height=7, written at ecs=0
        h1.set_ecs(true);
        h1.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h1.write_reg(BLTCON1, 0x0000);
        h1.write_reg(BLTSIZH, 1);          // width=1, starts the blit (ecs=1 here, unavoidably)
        h1.run_until_done(300);
        h1.settle();  // busy falls before the last D-word lands - see Harness::settle
        const int r1 = static_cast<int>(h1.data_writes.size());

        // Branch 2 [control]: identical height value, written at ecs=1 throughout.
        Harness h2(true);
        h2.write_reg(BLTSIZV, 7);          // height=7, written at ecs=1
        h2.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h2.write_reg(BLTCON1, 0x0000);
        h2.write_reg(BLTSIZH, 1);
        h2.run_until_done(300);
        h2.settle();
        const int r2 = static_cast<int>(h2.data_writes.size());

        checks.expect(r1 != r2,
            "BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS: appendix C marks BLTSIZV 'new' for ECS "
            "(p.309/310), the same status BLTSIZH and BLTCON0L carry and are correctly "
            "gated for (see BLTSIZH-GATED-BY-ECS and BLTCON0L-GATED-BY-ECS above) - so a "
            "BLTSIZV write made while ecs=0 is expected to behave differently from one made "
            "while ecs=1 (r1 != r2). MEASURED: r1=" + std::to_string(r1) +
            ", r2=" + std::to_string(r2) + " - identical. agnus_blitter.v:546-550 has no "
            "`&& ecs` on BLTSIZV's decode (contrast BLTSIZH two lines below it, "
            "agnus_blitter.v:523-524, which does), so the write takes effect whether or not "
            "ecs is set. THIS ASSERTION IS REQUIRED TO FAIL - see docs/findings.md, "
            "'Chapter 6 - read from the RTL, no test yet', and the file header above. "
            "6-appc-modifications.c12, ch6-appc-map.bltsizv, 6-appa-registers.c70, "
            "6-appa-registers.c71, ch6-appc-bltsizv.c1, 6-appc-big-blits.c65, "
            "ch6-appc-bigblits.c8 .");
    }

    // ---------------- BLTSIZV-HEIGHT-TOP-BIT-FUNCTIONAL (p.269, p.308) ----------------
    {
        Harness h(true);
        h.write_reg(BLTSIZV, 16384);  // bit 14, the field's top functional bit
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZH, 1);      // width=1, starts the blit
        const bool done = h.run_until_done(200000);
        h.settle();  // busy falls before the last D-word lands - see Harness::settle

        checks.expect(done && h.data_writes.size() == 16384,
            "BLTSIZV-HEIGHT-TOP-BIT-FUNCTIONAL: writing height=16384 (bit 14 of BLTSIZV's "
            "height[14:0]) produces exactly 16384 destination writes - bit 14, the top of "
            "the 15-bit field, is functional, distinctly beyond OCS BLTSIZE's own 10-bit/"
            "zero-means-1024 ceiling. agnus_blitter.v:548-549 "
            "(`height[14:0] <= data_in[14:0]`), :557-558 (BLTSIZH copies it into height_cnt). "
            "6-appc-big-blits.c27, 6-appc-big-blits.c28, 6-appc-big-blits.c29, "
            "6-appc-big-blits.c30, 6-appc-big-blits.c31, 6-appc-big-blits.c32, "
            "6-appc-big-blits.c33, 6-appc-big-blits.c34, 6-appc-big-blits.c35, "
            "6-appc-big-blits.c36, 6-appc-big-blits.c37, 6-appc-big-blits.c38, "
            "6-appc-big-blits.c39, 6-appc-big-blits.c40, 6-appc-big-blits.c41, "
            "ch6-appc-bltsizv.bit00, ch6-appc-bltsizv.bit01, ch6-appc-bltsizv.bit02, "
            "ch6-appc-bltsizv.bit03, ch6-appc-bltsizv.bit04, ch6-appc-bltsizv.bit05, "
            "ch6-appc-bltsizv.bit06, ch6-appc-bltsizv.bit07, ch6-appc-bltsizv.bit08, "
            "ch6-appc-bltsizv.bit09, ch6-appc-bltsizv.bit10, ch6-appc-bltsizv.bit11, "
            "ch6-appc-bltsizv.bit12, ch6-appc-bltsizv.bit13, ch6-appc-bltsizv.bit14, "
            "6-appc-big-blits.c42, 6-appc-big-blits.c43, ch6-appc-bltsizv.h, "
            "6-appa-registers.c72, ch6-appa-bltsize.v2 .");
    }

    // ---------------- BLTSIZV-HEIGHT-UPPER-BIT-IGNORED (p.308) ----------------
    {
        Harness h(true);
        h.write_reg(BLTSIZV, static_cast<uint16_t>(0x8000 | 7));  // bit 15 set, field value 7
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZH, 1);
        const bool done = h.run_until_done(200);
        h.settle();  // busy falls before the last D-word lands - see Harness::settle

        checks.expect(done && h.data_writes.size() == 7,
            "BLTSIZV-HEIGHT-UPPER-BIT-IGNORED: writing 0x8007 to BLTSIZV (bit 15 set, low 15 "
            "bits = 7) produces exactly 7 destination writes, not 32775 - bit 15 is outside "
            "height[14:0] and has no effect. agnus_blitter.v:548-549 "
            "(`height[14:0] <= data_in[14:0]`, a 15-bit slice of a 16-bit data_in). "
            "6-appc-big-blits.c26, ch6-appc-bltsizv.bit15 .");
    }

    // ---------------- BACKWARD-COMPAT-BLTSIZE-WORKS-WITH-ECS (p.308) ----------------
    {
        Harness h(true);  // ecs=1: does the OLD OCS register still work?
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(3, 1));  // OCS-style width=3, height=1
        const bool done = h.run_until_done(60);
        h.settle();  // busy falls before the last D-word lands - see Harness::settle

        checks.expect(done && h.data_writes.size() == 3,
            "BACKWARD-COMPAT-BLTSIZE-WORKS-WITH-ECS: the original OCS BLTSIZE register "
            "($058) still starts a blit and sizes it correctly (3 destination writes for "
            "width=3) even with ecs=1 - agnus_blitter.v:500,521-522,555-556 have no `&& "
            "ecs` anywhere on the BLTSIZE decode, so it is unconditional. 6-appc-big-"
            "blits.c64, ch6-appc-bigblits.c7 .");
    }

    // ---------------- DOFF-NO-OBSERVABLE-EFFECT (p.198, p.199, p.267, p.309, p.310) ----------------
    // *** REQUIRED RED - see file header. Do not adjust this assertion to pass. ***
    {
        Harness h_doff0(true);
        h_doff0.a_words = {0x1111};
        h_doff0.c_words = {0x2222};
        h_doff0.write_reg(BLTAFWM, 0xFFFF);
        h_doff0.write_reg(BLTALWM, 0xFFFF);
        h_doff0.write_reg(BLTCON0, USEA | USEC | USED | LF_C_PASSTHROUGH);
        h_doff0.write_reg(BLTCON1, 0x0000);  // DOFF (bit 7) clear
        h_doff0.write_reg(BLTSIZE, bltsize_word(1, 1));
        h_doff0.run_until_writes(1, 60);

        Harness h_doff1(true);
        h_doff1.a_words = {0x1111};
        h_doff1.c_words = {0x2222};
        h_doff1.write_reg(BLTAFWM, 0xFFFF);
        h_doff1.write_reg(BLTALWM, 0xFFFF);
        h_doff1.write_reg(BLTCON0, USEA | USEC | USED | LF_C_PASSTHROUGH);
        h_doff1.write_reg(BLTCON1, DOFF_BIT);  // DOFF (bit 7) set - identical stimulus otherwise
        h_doff1.write_reg(BLTSIZE, bltsize_word(1, 1));
        h_doff1.run_cycles(30);

        char doff0_word[8];
        std::snprintf(doff0_word, sizeof(doff0_word), "%04X",
                      h_doff0.data_writes.empty() ? 0u : static_cast<unsigned>(h_doff0.data_writes[0]));

        checks.expect(h_doff1.data_writes.empty(),
            "DOFF-NO-OBSERVABLE-EFFECT: with BLTCON1 bit 7 (DOFF) set, appendix C p.309 says "
            "the blitter's output on channel D is disabled ('when set, disables the output "
            "of the Blitter hardware on channel D', 'this allows inputs to channels A, B and "
            "C and certain address modification ... without the Blitter outputting over "
            "channel D'). MEASURED: with identical stimulus otherwise (the same run that "
            "produces one D write with DOFF clear, h_doff0.data_writes={0x" +
            std::string(doff0_word) +
            "}), setting DOFF still produces the SAME D write. bltcon1[11:0] is stored on "
            "every BLTCON1 write with no per-bit exception (agnus_blitter.v:277-283) and no "
            "expression anywhere in the file reads bltcon1[7]; the D write gate is `used` "
            "and `chsel`, agnus_blitter.v:252,490-491 - DOFF genuinely has no wire into "
            "either. THIS ASSERTION IS REQUIRED TO FAIL - see docs/findings.md, 'Chapter 6 - "
            "read from the RTL, no test yet', and the file header above. "
            "6-appc-modifications.c4, 6-appc-modifications.c5, 6-appc-modifications.c6, "
            "6-appc-modifications.c7, 6-appc-modifications.c9, ch6-appc-ecs.c4, "
            "ch6-appc-ecs.c6, ch6-appc-map.bltcon1, 6-appa-control.c18, "
            "ch6-appa-bltcon0.area.b07.con1, 6-appa-registers.c20, ch6-appa-bltcon0.c6, "
            "ch6-blockdiag.c10, 6-blitter-block-diagram.c11 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
