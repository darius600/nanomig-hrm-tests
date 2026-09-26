// Test purpose: Verify the BLTCON0/BLTCON1 bit maps printed on appendix A
//   p.267 (AREA MODE) and p.268 (LINE MODE), field by field: which bit is
//   which named field, in both registers, in both modes. The AREA-MODE
//   table's bits marked unused ("X") are asserted as "setting this bit
//   changes nothing observable", made falsifiable with an explicit control,
//   because p.264/p.170 state they are not implemented; the LINE-MODE
//   table's fixed-value bits (11,10,9,8,7,5) carry no page-stated
//   consequence for a different value, so that half is recorded, not
//   asserted -- see each assertion's own header for which is which. Extended
//   with one claim from Figure 6-13 (p.199) about which of these same
//   BLTCON0/BLTCON1 fields are the control lines the figure draws entering
//   the datapath from outside -- see "EXTENSION" below.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 170, 174, 179, 184, 191-192, 198-199, 267-269 (appendix A's
//   BLTCON0/BLTCON1 register tables and legends, the chapter prose that
//   names the same fields, and p.198's prose plus Figure 6-13 itself on
//   p.199, a diagram with no independent text layer -- see "EXTENSION"
//   below). Appendix A pp.267-268 verified against the actual
//   PDF page scan via tools/show_page.py, not only against the OCR text
//   corpus -- see "CORPUS DEFECT FOUND" below. p.199's figure was read
//   directly as an image (docs/references/amiga-hrm-3rd/ch6/figures/
//   p199.png); it has no OCR text layer to compare it against.
// Non-goals: exact minterm/truth-table correctness for a given LF byte
//   (PKG-BLT-MINTERM's job -- this file uses the minterm generator only as
//   plumbing to prove which BITS reach it, sampling the two end bits LF7/
//   LF0 in area mode and both of the manual's own worked line-mode byte
//   values, never re-deriving the full truth table); barrel-shifter
//   correctness for a given ASH/BSH value (PKG-BLT-SHIFT/PKG-BLT-SHIFT-
//   CHAIN's job -- this file only shows each shift-field bit is
//   independently live, never what a given shift value produces);
//   fill-mode correctness (PKG-BLT-FILL's job); full line-mode
//   octant/Bresenham/ONEDOT pixel-by-pixel correctness, which octant NUMBER
//   steps in which real-world direction, and the sign flag's role across a
//   multi-pixel line beyond the one flat-line precondition LINE-MODE-SING
//   needs (PKG-BLT-LINE's job -- this file only shows each line-mode
//   control bit is independently live); DOFF and BLTCON0L/BLTSIZH/
//   BLTSIZV's ECS-gated behaviour beyond the one BLTCON0L-aliases-LF fact
//   this package's own claim list names (the ECS width/height claims are
//   PKG-BLT-ECS's job); register offsets and access types
//   (PKG-BLT-REGDECODE's job).
// Claim ids: ids such as `6-appa-control.c1` and `6-appa-control.c3` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch6.json lists under `inventories`. One id,
//   `ch6-fig613.f27`, is a figure claim (`source_kind: "figure"`) rather
//   than a sentence claim -- see "EXTENSION" below for what that changes
//   about how it is grounded and cited.
// Specification: SPECIFICATION.md in this directory, written from this
//   package's own claims and the manual's pages by an author who never saw
//   the RTL or the test; the assertions below are checked against it.
//
// NO PRIOR ART IS SUPERSEDED. docs/evidence/blitter-prior-art.md was read in full
// before writing this file, per the build brief's explicit instruction to
// say so rather than assume it. Its "eight blitter tests" table lists
// barrelshifter, fill, minterm, modulo, ptr, line_octant, mask and zero --
// none of the eight asserts a single fact about which BIT of BLTCON0 or
// BLTCON1 carries which named field, in either mode, or about which bits
// the tables mark unused/reserved. This is a genuinely new directory with
// no predecessor to confirm, extend or supersede.
//
// FIXTURE. Built on the corrected-clkena harness, the same mechanism as
// agnus_blitter_status/_size/_cycles: Harness::cycle's DMA-grant handshake
// settles eval() before the clock edge, captures we/data_out AT the grant
// rather than polling afterwards, samples int3 before the tick's own clock
// edge, and toggles clkena once per cycle() call to match agnus.v's true
// divide-by-two cck drive rather than holding it constantly high. This
// package counts no durations, so the clkena fixture defect that cost
// PKG-BLT-CYCLES eight false findings cannot bite here (nothing below
// reads a tick count) -- the corrected harness is used anyway, because the
// grant-handshake fix it also carries (TEST-DATA-DELIVERY) is unrelated to
// clkena and load-bearing regardless. TEST-DATA-DELIVERY below reproduces
// agnus_blitter_status's/agnus_blitter_cycles's own fixture check first,
// before anything data-dependent: it feeds four distinct C words and
// confirms they arrive at data_out, in order, at the cycles they were
// fetched on -- a harness whose data path is silently dead would make
// every data-dependent assertion below report a finding about the design
// instead of about the fixture.
//
// THE ecs PORT: most of this file ties `ecs` to 0 (the printed AREA/LINE
// tables and their bit meanings are chipset-independent). One assertion,
// BLTCON0L-ALIASES-LF-BYTE-WHEN-ECS, sets ecs=1 deliberately, because that
// is the one claim in this package (p.269, appendix A) that is inherently
// about the ECS-gated register. The 109-record ecs/BLTSIZV defect recorded
// in docs/findings.md (109 records claim `ecs` gates BLTSIZV; it does not)
// does not bear on anything asserted here -- noted only because the
// standing rule requires checking.
//
// CORPUS DEFECT FOUND while grounding against the printed pages (rule:
// ground against the printed corpus, not against extract.restatement).
// docs/references/amiga-hrm-3rd/appx6-source/p267.txt -- the corpus
// directory the chapter-playbook's Phase-(-1) item 7 calls "the print", used
// to check appA-blitter's OCR against page breaks -- has its own OCR defect
// on this specific table: its BLTCON1 AREA-MODE column is shifted down one
// row for bits 7 through 0 (it reads "07 X / 06 DOFF / 05 X / 04 X / 03 EFE
// / 02 IFE / 01 FCI / 00 DESC" with a trailing, bit-number-less "LINE(=0)").
// Rendering the actual PDF page (tools/show_page.py --page 267, verified by
// looking at the image, not just its OCR) shows the true printed table
// reads "07 DOFF / 06 X / 05 X / 04 EFE / 03 IFE / 02 FCI / 01 DESC / 00
// LINE(=0)" -- which is exactly what docs/references/amiga-hrm-3rd/
// appA-blitter/p267.txt (the Developer-CD-derived corpus every claim in
// this package's inventory actually quotes) already has right, and it is
// exactly what agnus_blitter.v:285-288 wires (efe=bltcon1[4], ife=
// bltcon1[3], fci computed from bltcon1[2], desc=bltcon1[1], line=
// bltcon1[0]). So every assertion below follows appA-blitter's (and the
// PDF scan's) bit numbering; appx6-source/p267.txt's shifted OCR is not
// used anywhere and is recorded here as a defect in that one corpus file,
// not as a sixth disputed table -- the PDF settles it without ambiguity,
// unlike the six/seven genuine source conflicts below.
//
// SOURCE CONFLICTS.
//   #4 (p267): the table row labels BLTCON0 bit 12 "ASA0" while the legend
//     on the same page says "ASH3-0". Undisputed: bits 15-13 are ASH3-ASH1
//     and the field is four bits ending at bit 12 -- see
//     AREA-ASH-BITS-15-13-ARE-A-SHIFT-VALUE, which asserts bits 15-13 by
//     name and separately shows bit 12 belongs to the same 4-bit field by
//     the same RTL mechanism, without asserting either disputed name for
//     it. Neither reader created a claim naming bit 12 "ASA0" in this
//     package's inventory (checked by grep over the 219 records), so there
//     is nothing to assert or decline for that name specifically.
//   #5 (p268 vs pp.191-192): START3-0 "0 thru 15 hex" against "x1 modulo
//     15". Undisputed: the field is four bits and holds the line's starting
//     bit position -- see LINE-MODE-START-BITS-15-12-REUSE-THE-ASH-FIELD,
//     which asserts the field's existence, width and identity with `ash`,
//     never a specific numeric range. No run in this file writes START=15
//     anywhere (0, 5 and 14 are used) - 15 is a value p.191/192's "x1
//     modulo 15" cannot produce.
//   FCI/IFE bit numbers (p.185 "FILL_CARRYIN or bit 3" vs p.184 "FILL_OR,
//     or bit 3" vs p.267 bit 3 IFE / bit 2 FCI): see "FILL-CARRY-IN / IFE-
//     BIT3" below, which withdraws the bit-number ids rather than picking
//     a side.
//   #6a (p269 vs pp.191-192): BLTBDAT "FF" against "$FFFF" for a solid
//     line -- and #6b (p270 vs pp.191-192): BLTAPTL "2Y-X" against
//     "4*dy-2*dx". Neither bears on this package's own claim ids (BLTBDAT's/
//     BLTAPTL's numeric claims are PKG-BLT-LINE's and PKG-BLT-ADRGEN's, not
//     a BLTCON0/BLTCON1 bit-map claim). Of this file's own line-mode runs:
//     LINE-MODE-SING and LINE-MODE-START set BOTH BLTBDAT=$FFFF and a
//     hand-computed BLTAPT (a real p.192 dx=2,dy=0 line); LINE-MODE-LF-4A-
//     AND-CA and LINE-MODE-LF-BITS-EACH-ALONE write BLTBDAT ($FFFF, and
//     $0000/$FFFF across the sweep) but not BLTAPT, relying on its reset
//     value of 0 being already correct for a degenerate one-pixel dx=dy=0
//     case; LINE-MODE-TEXTURE writes BLTBDAT=$0001 (its own single test
//     bit, not the page's solid-line value) and not BLTAPT, for the same
//     dx=dy=0 reason; BLTCON1-BIT0, LINE-MODE-BLTCON0-BITS-11-10-9-8 and
//     LINE-MODE-SUD-SUL-AUL write neither, since none of the three asserts
//     a page-derived line at all (the first two are area-vs-line and
//     fetch-gating questions, and SUD-SUL-AUL's own label already says its
//     probe values are RTL-chosen sensitizing vectors, not a page-derived
//     (dx,dy) setup).
//
// CLAIM COVERAGE. All 219 records of classify.package==
// "PKG-BLT-BLTCON-FIELDS" and classify.status=="testable" are covered
// below, either by an assertion, a MEASURED note, or a RECORDED decline
// with a reason. Both readers cover much of the same ground -- the
// appendix A bit-map table in particular is stated by both readers, once
// per bit per register per mode -- so most ids below are the same
// proposition (one bit's identity) stated up to four times (sol's
// table-row phrasing, opus's table-row phrasing, and often a same-field
// legend/prose restatement from each). Every id is written out in full
// inside its assertion's message or its RECORDED note, never a
// shared-prefix shorthand.
//
// EXTENSION (ch6-fig613.f27): one claim from Figure 6-13 itself
// (p.199, classify.package=="PKG-BLT-MINTERM"), graded in THIS package
// rather than ch6-blitter-minterm -- its content is a bit/field-identity
// claim like the rest of this file's scope, not minterm truth-table
// correctness, which stays PKG-BLT-MINTERM's/ch6-blitter-minterm's job per
// this file's own Non-goals above. It is not part of the 219-record count
// in "CLAIM COVERAGE" above, which is scoped to
// classify.package=="PKG-BLT-BLTCON-FIELDS" only.
//
// extract.quote for this id is literally "[FIGURE 6-13 -- figures/p199.png
// -- not text; open the image and read it]"; extract.restatement reads "In
// Figure 6-13 the 'Blitter Control' block supplies SHIFT A VALUE, the
// multi-minterm select bits, Fill Carry In, Fill Enable and Store to
// Destination." Grounded against the actual page image
// (docs/references/amiga-hrm-3rd/ch6/figures/p199.png, opened directly,
// never read from extract.restatement) and against p.198's prose
// introducing the same figure: the diagram does NOT draw an arrow from a
// box literally labelled "Blitter Control" into the five named lines. It
// draws SIX sibling incoming lines entering the datapath from off-page --
// SHIFT A VALUE, Multi-minterm select bits (8), Fill Carry In, Fill Enable,
// Store to Destination, and a sixth, separately boxed line that itself
// reads "Blitter Control" -- distinct from the boxes the SAME figure feeds
// directly off DATA BUS at the top (FW Mask, A new, LW Mask, B new, C
// hold). The only other "(Blitter Control)" annotation on the page sits
// beside the unrelated SHIFT B VALUE box near the top, confirming the
// phrase is used as a descriptive tag for "this line is blitter-control-
// register-sourced," not as the name of an enclosing block with drawn
// output arrows. p.198's own prose bullets introducing Figure 6-13 use
// exactly that generic, unlabelled framing and nowhere name a "Blitter
// Control" block: shift values "based on the A shift or B shift values
// shown in BLTCON0 and BLTCON1" (bullet 2, SHIFT A VALUE); "the minterm
// select bits either allow or inhibit the use of a specific minterm"
// (bullet 3, Multi-minterm select bits); fill operating on the minterm
// output (bullet 4, Fill Carry In / Fill Enable); and "data generated for
// the destination can be prevented from being written to a destination by
// using one of the blitter control bits" (bullet 5, Store to Destination).
// Read this way the claim's substance holds -- all five are
// BLTCON0/BLTCON1-programmable, not hardwired or purely data-derived --
// even though "the 'Blitter Control' block supplies" overstates the
// diagram's own arrow topology; extract.restatement is a paraphrase, not
// the quotation, and what the assertion below actually measures is the
// substance, grounded against the page.
//
// This claim bundles five sub-facts in one id, split at the RTL as follows:
// SHIFT A VALUE = BLTCON0[15:12] (:219-232), minterm select = BLTCON0[7:0]
// (:254-261, :444), Fill Carry In = BLTCON1[2] (:464), Fill Enable =
// BLTCON1[4:3] (:287-288), Store to Destination = USED, BLTCON0[8] (:252).
// Five assertions carry the id, each measuring one sub-fact:
// AREA-ASH-BITS-15-13-ARE-A-SHIFT-VALUE (SHIFT A VALUE),
// AREA-USE-BITS-11-8-SELECT-CHANNELS (Store to Destination, via its
// USED/D-grant result), AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL (the EFE half
// of Fill Enable), BLTCON0-BITS-7-0-EACH-BIT-SELECTS-ONLY-ITS-OWN-MINTERM
// (minterm select -- BLTCON0-BITS-7-0-ARE-THE-LF-BYTE samples only bit 7
// and bit 0, the two ends of the eight select lines Figure 6-13 draws, and
// never shows the other six come from BLTCON0 at all, so EACH-BIT carries
// the id instead) and FILL-CARRY-IN-PRODUCES-P185-WORKED-EXAMPLE (the Fill
// Carry In half: BLTCON0/BLTCON1-programmable and producing p.185's own
// worked value, without resolving which bit number is FCI -- see
// "FILL-CARRY-IN / IFE-BIT3" below for that separate, unresolved question).
// None of the five assertions carrying the id claims to establish the whole
// of what ch6-fig613.f27 says by itself; the set of them, taken together,
// does.
//
// UNUSED-BIT ASSERTIONS ARE MADE FALSIFIABLE WITH A CONTROL, where the
// manual states no effect for a bit. One assertion below claims "setting
// this bit changes nothing observable": AREA-MODE-BLTCON1-BITS-11-10-9-8-
// 6-5-ARE-UNUSED (control: BLTCON1 bit 1/DESC, shown wired by
// AREA-DESC-BIT1 earlier in this file, on the address_out-sequence basis).
// The control is driven through the identical measurement, on the
// identical basis, in the identical run as the sweep it backs. If the
// fixture could not see the control bit's effect, the control itself
// would fail first and the unused-bit half would never be trusted.
//
// The LINE-MODE equivalents of this pattern do NOT survive as graded
// assertions: no page states what line mode does with a different value in
// BLTCON0 bits 11, 10, 9 and 8, or in BLTCON1 bits 11, 10, 9, 8, 7 and 5 --
// every "no effect" claim about them was a fact read off the RTL, not the
// manual (agnus_blitter.v:795-869's line-mode FSM never reads usea, useb or
// used; agnus_blitter.v:277-283 stores BLTCON1[11:0] wholesale with nothing
// reading those six bits). BLTCON0's four are declined together (bit 9's
// own RTL behaviour does not change that: see
// "LINE-MODE-BLTCON0-BITS-11-10-9-8" below for why grading bit 9 alone by
// p.170 while declining its siblings by the same page is not an available
// split). BLTCON1's required-zero half is not built as its own run either:
// LINE-MODE-SING-BIT1's own p.192 flat line already holds all six at 0 in
// both branches, so those ids are carried there instead, and only the
// OVFLAG ids (whose bit position no page states at all) get a RECORDED
// decline of their own. Every one of these bits' "no effect of a different
// value" half is kept only as a MEASURED note (a source observation), never
// asserted. The line-mode table nonetheless prints literal required values
// ("1","0","1","1" for bits 11,10,9,8; "0" for bits 11,10,9,8,7 and "0
// (Reserved)" for bit 5), and the chapter's own register-summary prose
// (pp.191-192) tells the programmer to write them that way - programming
// guidance, not a hardware-enforcement claim, and this package does not
// test that they are enforced.
//
// Every mutation below was actually run against the file as it now
// stands: edit -> rebuild -> run -> confirm exactly the named assertion
// turned red with every other graded assertion still green -> revert ->
// rebuild -> confirm the full green count returns. The final clean
// rebuild after the last revert printed "18 passed, 0 failed", and every
// mutation is a STIMULUS change to this file's own inputs -- expected
// values and oracle text were never touched -- and never to nanomig/,
// which this task forbids touching for any reason.
//
// Each record below is one physical line: the id, then the kind and
// result keywords immediately after it on the SAME line (check_gates.py's
// MUTATION_RE cannot cross a `//` continuation prefix, so wrapping the
// keywords onto the next comment line makes the record unreadable even
// though the mutation was genuinely run). No bracketed tag is put in the
// id position either, for the same reason: it is not part of the
// assertion's own recognised id.
//
// MUTATION LOG (one entry per graded assertion; 18 assertions, 18 records):
//
// MUTATION TEST-DATA-DELIVERY stimulus red: fed C word at index 2 changed
//   from 0x9ABC to 0x9ABD -> FAIL, the third captured data_out no longer
//   matches. Reverted: 18/0.
// MUTATION AREA-ASH-BITS-15-13-ARE-A-SHIFT-VALUE stimulus red: the bit-13
//   probe's stimulus changed from probe_ash(1u<<1) to probe_ash(0), so it
//   now collides with the ash=0 baseline -> FAIL, "bit13 != baseline"
//   fails. Reverted: 18/0.
// MUTATION AREA-BSH-BITS-15-12-ARE-B-SHIFT-VALUE stimulus red: the bit-13
//   probe's stimulus changed from probe_bsh(1u<<1) to probe_bsh(1u<<3),
//   colliding with the bit-15 probe's own stimulus -> FAIL, the newly
//   added "bit15 != bit13" comparison fails (both read the same shifted
//   value). Reverted: 18/0.
// MUTATION AREA-USE-BITS-11-8-SELECT-CHANNELS stimulus red: the "noA" run's
//   use_bits changed from (USEB|USEC|USED) to (USEA|USEB|USEC|USED), so
//   channel A is no longer actually removed -> FAIL, "!noA[0]" fails
//   (A is still granted). Reverted: 18/0.
// MUTATION BLTCON0-BITS-7-0-ARE-THE-LF-BYTE stimulus red: word index 7's C
//   input forced to 0x0000 (was 0x8000, the all-ones selector) right
//   before both probe_lf_bit(0x80) and probe_lf_bit(0x01) consume it ->
//   FAIL, the LF7 probe's word 7 no longer reads 0x8000 against a
//   still-all-ones expectation. Reverted: 18/0.
// MUTATION BLTCON0-BITS-7-0-EACH-BIT-SELECTS-ONLY-ITS-OWN-MINTERM stimulus red:
//   the per-bit probe's own LF stimulus changed from
//   `static_cast<uint16_t>(1u << k)` to `static_cast<uint16_t>(1u <<
//   ((k + 1) % 8))`, so each iteration k now sets a DIFFERENT LF bit than
//   the one its own (A,B,C) comparison expects -> FAIL, every one of the
//   eight per-k comparisons mismatches. Reverted: 18/0.
// MUTATION BLTCON0L-ALIASES-LF-BYTE-WHEN-ECS stimulus red: `h.set_ecs(true)`
//   changed to `h.set_ecs(false)` before the BLTCON0L write -> FAIL, the
//   write is ignored (agnus_blitter.v:259's `&& ecs` guard), so the stored
//   word stays 0x0000 against the mutated call's own unchanged expectation
//   of 0x8000. Reverted: 18/0.
// MUTATION AREA-MODE-BLTCON1-BITS-11-10-9-8-6-5-ARE-UNUSED stimulus red:
//   the sweep array's bit-9 slot changed from 9 to 1 (DESC, the control
//   bit, genuinely wired) -> FAIL, that entry's "identical to baseline"
//   check fails (DESC really does reverse the address_out sequence).
//   Reverted: 18/0.
// MUTATION AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL stimulus red: the BLTCON1
//   write's `(efe ? (1u<<4) : 0)` term changed to an unconditional
//   `(false ? (1u<<4) : 0)`, so the "on" run no longer actually sets EFE ->
//   FAIL, "on == 0x1C08" fails (the "on" run stays at the unfilled 0x2418,
//   same as the "off" run). Reverted: 18/0.
// MUTATION FILL-CARRY-IN-PRODUCES-P185-WORKED-EXAMPLE stimulus red: the
//   `carry_via_bit2` computation's `FCI_BIT2` term dropped, so the carry-in
//   is never actually forced through bit 2 -> FAIL, both disjuncts read
//   0x1C08, neither equals 0xE3F7. Reverted: 18/0.
// MUTATION AREA-DESC-BIT1-SELECTS-DESCENDING-MODE stimulus red: the
//   BLTCON1 write changed from `desc ? (1u<<1) : 0` to an unconditional
//   0x0000, so the "descending" run no longer actually sets DESC -> FAIL,
//   "descending_delta < 0" fails (both runs ascend). Reverted: 18/0.
// MUTATION BLTCON1-BIT0-SELECTS-AREA-OR-LINE-MODE stimulus red: the
//   BLTCON1 write changed from `line_mode ? LINE_BIT : 0` to an
//   unconditional LINE_BIT, so the "area" run is secretly run in line mode
//   too -> FAIL, "area_we == 2" fails (1 write, not 2). Reverted: 18/0.
// MUTATION LINE-MODE-START-BITS-15-12-REUSE-THE-ASH-FIELD stimulus red:
//   the `seq14` run's own start argument changed from 14 to 0, so both
//   runs now use the same starting value -> FAIL, "seq14 == expect14"
//   fails (identical 3-pixel D-write address sequences, no word crossing
//   on either run). Reverted: 18/0.
// MUTATION LINE-MODE-TEXTURE-BITS-15-12-REUSE-THE-BSH-FIELD stimulus red:
//   the "texture15" run's argument changed from 15 to 0, so both runs now
//   use no shift -> FAIL, "texture15 == 0x0000" fails (both reads reach
//   0x8000). Reverted: 18/0.
// MUTATION LINE-MODE-SUD-SUL-AUL-BITS-4-3-2-ARE-LIVE stimulus red: the
//   octant-1 probe's stimulus changed from AUL alone (1u<<2) to AUL|SUD
//   (matching octant 4's stimulus) -> FAIL, "sud_live" (oct1 != oct4) fails
//   (identical 4-pixel D-write address sequences). Reverted: 18/0.
// MUTATION LINE-MODE-SING-BIT1-GATES-D-CHANNEL-WRITES-TO-FIRST-PIXEL stimulus red:
//   the SING-setting line changed from `if (sing) con1 |= SING;` to
//   `if (false) con1 |= SING;`, so the "sing=true" run no longer actually
//   sets bit 1 -> FAIL, "with_sing == 1" fails (all 3 pixels still reach
//   the row, popcount 3). Reverted: 18/0.
// MUTATION LINE-MODE-LF-BITS-EACH-ALONE-MATCH-P174-MAPPING stimulus red:
//   the one-pixel sweep's own C-word feed changed from `c` to `c ^ 0x0002`,
//   decoupling the DUT's actual C input from what `page_expect` computes
//   -> FAIL, every one of the 32 runs mismatches. Reverted: 18/0.
// MUTATION LINE-MODE-LF-4A-AND-CA-SELECT-DOCUMENTED-FUNCTIONS stimulus red:
//   the "ordinary_form" run's LF argument changed from 0xCA to 0x4A, so
//   both runs now select the SAME (exclusive-or) function -> FAIL,
//   "ordinary_form == 0xFFFF" fails (reads 0x7FFF instead). Reverted: 18/0.
//
// Every mutation was confirmed reverted by re-running `make test` (18/0)
// before moving to the next one; the final clean rebuild after the last
// revert also printed "18 passed, 0 failed".

#include <cstdint>
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
constexpr uint16_t BLTCPTH  = to_reg(0x048);
constexpr uint16_t BLTCPTL  = to_reg(0x04a);
constexpr uint16_t BLTBPTH  = to_reg(0x04c);
constexpr uint16_t BLTBPTL  = to_reg(0x04e);
constexpr uint16_t BLTAPTH  = to_reg(0x050);
constexpr uint16_t BLTAPTL  = to_reg(0x052);
constexpr uint16_t BLTDPTH  = to_reg(0x054);
constexpr uint16_t BLTDPTL  = to_reg(0x056);
constexpr uint16_t BLTSIZE  = to_reg(0x058);
constexpr uint16_t BLTCON0L = to_reg(0x05a);
constexpr uint16_t BLTCMOD  = to_reg(0x060);
constexpr uint16_t BLTBMOD  = to_reg(0x062);
constexpr uint16_t BLTAMOD  = to_reg(0x064);
constexpr uint16_t BLTDMOD  = to_reg(0x066);
constexpr uint16_t BLTCDAT  = to_reg(0x070);
constexpr uint16_t BLTBDAT  = to_reg(0x072);
constexpr uint16_t BLTADAT  = to_reg(0x074);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252.
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEB = 1u << 10;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;
constexpr uint16_t LINE_BIT = 1u << 0;  // BLTCON1 bit 0

struct Sample {
    int tick;
    bool busy;
    bool zero;
    bool int3;
    bool we;
    uint16_t data_out;
    bool reqdma;
};

struct Grant {
    int tick;
    char channel;  // 'A','B','C','D'
    uint32_t address;
};

class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->clkena = 1;  // cycle() drives the true alternation from here on
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        log.clear();
        data_writes.clear();
        grants.clear();
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

    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    int run_until_done(int max_ticks = 2000) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            const Sample& s = log.back();
            if (s.busy) seen_busy = true;
            if (seen_busy && !s.busy) return s.tick;
        }
        return -1;
    }

    void settle(int n = 10) { run_cycles(n); }

    bool zero() const { return dut->zero; }
    bool busy() const { return dut->busy; }
    bool int3() const { return dut->int3; }

    std::deque<uint16_t> a_words, b_words, c_words;
    std::vector<Sample> log;
    std::vector<uint16_t> data_writes;
    std::vector<Grant> grants;
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
    bool last_int3 = false;
    bool last_granted = false;
    char last_grant_channel = 0;
    uint32_t last_grant_address = 0;
    // agnus.v:437 wires the blitter's `clkena` to `cck`, a true divide-by-2
    // of clk7_en (amiga_clk.v:19-38). Toggled once per cycle() call to
    // match cck's own toggle-once-per-tick behaviour, starting high
    // (amiga_clk.v:28's `cck <= 1` reset value). See hrm/agnus_blitter_
    // cycles's header for the full diagnosis; reused verbatim here per the
    // build brief.
    bool clkena_phase = true;

    void tick_bookkeeping() {
        Sample s{tick_count++, static_cast<bool>(dut->busy), static_cast<bool>(dut->zero),
                  last_int3, last_we, last_data_out, static_cast<bool>(dut->reqdma)};
        log.push_back(s);
        if (s.we) data_writes.push_back(s.data_out);
        if (last_granted) grants.push_back(Grant{s.tick, last_grant_channel, last_grant_address});
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->clkena = clkena_phase ? 1 : 0;
        clkena_phase = !clkena_phase;
        dut->eval();
        last_int3 = dut->int3;
        last_we = false;
        last_data_out = 0;
        last_granted = false;
        last_grant_channel = 0;
        last_grant_address = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            char ch = 0;
            if (reg == BLTADAT) { v = pop_or_zero(a_words); ch = 'A'; }
            else if (reg == BLTBDAT) { v = pop_or_zero(b_words); ch = 'B'; }
            else if (reg == BLTCDAT) { v = pop_or_zero(c_words); ch = 'C'; }
            else if (reg == 0 /* BLTDDAT */) { ch = 'D'; }
            dut->reg_address_in = reg;
            dut->data_in = v;
            dut->eval();
            last_granted = true;
            last_grant_channel = ch;
            last_grant_address = dut->address_out;
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
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

void write_bltcon0(Harness& h, uint16_t ash, uint16_t use_bits, uint16_t lf) {
    h.write_reg(BLTCON0, static_cast<uint16_t>((ash << 12) | use_bits | lf));
}

// Starts a plain (non-line, ascending, no-fill) blit: BLTCON0 with the
// given ash/use/lf, BLTCON1=0, BLTSIZE last (starts the blit).
void start_blit(Harness& h, uint16_t use_bits, uint16_t lf, uint16_t width, uint16_t height, uint16_t ash = 0) {
    write_bltcon0(h, ash, use_bits, lf);
    h.write_reg(BLTCON1, 0x0000);
    h.write_reg(BLTSIZE, bltsize_word(width, height));
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture check, no page) ----------------
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        start_blit(h, USEC | USED, 0xAA /* LF: D=C passthrough */, 4, 1);
        h.run_until_writes(4, 60);
        h.settle();

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY: four distinct C words fed into the "
            "fixture arrive at data_out, in order, at the we cycles they were fetched on "
            "- proves the DMA grant handshake actually delivers data, which every "
            "data-dependent assertion below depends on. Reproduced from "
            "hrm/ch6-blitter-cycles's TEST-DATA-DELIVERY before anything data-"
            "dependent, per the build brief.");
    }

    // ============================================================
    // AREA MODE (appendix A p.267)
    // ============================================================

    // ---------------- AREA-ASH-BITS-15-13-ARE-A-SHIFT-VALUE ----------------
    {
        // Two-word A-passthrough blit so the barrel shifter has a real,
        // non-zero "old" word to combine with (a one-word blit's old word
        // is always zero and can hide a shift). Reads the SECOND word's
        // output. Exact shift arithmetic is PKG-BLT-SHIFT/-SHIFT-CHAIN's
        // job (Non-goals); this only shows each probed bit is
        // independently live and inside the field.
        auto probe_ash = [](uint16_t ash) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.a_words = {0xFFFF, 0x0000};
            start_blit(h, USEA | USED, 0xF0 /* LF: D=A passthrough */, 2, 1, ash);
            h.run_until_writes(2, 80);
            h.settle();
            return h.data_writes.size() >= 2 ? h.data_writes[1] : uint16_t{0xDEAD};
        };
        const uint16_t baseline = probe_ash(0);
        const uint16_t bit15 = probe_ash(1u << 3);  // ash=1000
        const uint16_t bit14 = probe_ash(1u << 2);  // ash=0100
        const uint16_t bit13 = probe_ash(1u << 1);  // ash=0010

        checks.expect(
            bit15 != baseline && bit14 != baseline && bit13 != baseline &&
            bit15 != bit14 && bit14 != bit13 && bit15 != bit13,
            "AREA-ASH-BITS-15-13-ARE-A-SHIFT-VALUE: setting BLTCON0 bit 15 alone, "
            "bit 14 alone, or bit 13 alone (all else zero) each produces a shifted "
            "A-channel output distinct from the ash=0 baseline AND from each other, "
            "confirming bits 15, 14 and 13 are three independently-live bits of the "
            "A-shift field - '15 ASH3 ...', '14 ASH2 ...', '13 ASH1 ...' (appendix A "
            "p.267), 'ASH3-0  Shift value of A source' (p.267 legend), 'The shift "
            "value for the A channel is set with bits 15 through 12 of BLTCON0' "
            "(p.179). agnus_blitter.v:232 `assign ash[3:0] = bltcon0[15:12]`, :344 "
            "wires it to shifter A. Bit 12 is part of the SAME 4-bit field by this "
            "same RTL mechanism, but is not named here: the table row labels it "
            "ASA0 while the p.267 legend calls the field ASH3-0 (source conflict "
            "#4, packages.md item 4); neither reader created a claim naming bit "
            "12 in this package's inventory, so there is nothing to assert or "
            "decline for that name specifically. "
            "6-appa-control.c1, 6-appa-control.c3, 6-appa-control.c5, "
            "ch6-appa-bltcon0.area.b15.con0, ch6-appa-bltcon0.area.b14.con0, "
            "ch6-appa-bltcon0.area.b13.con0, ch6-appa-bltcon0.legend.ASH30, "
            "ch6-shifts.c8, ch6-fig613.f27 .");
    }

    // ---------------- AREA-BSH-BITS-15-12-ARE-B-SHIFT-VALUE ----------------
    {
        auto probe_bsh = [](uint16_t bsh) {
            Harness h;
            h.b_words = {0xFFFF, 0x0000};
            write_bltcon0(h, 0, USEB | USED, 0xCC /* LF: D=B passthrough */);
            h.write_reg(BLTCON1, static_cast<uint16_t>(bsh << 12));
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_until_writes(2, 80);
            h.settle();
            return h.data_writes.size() >= 2 ? h.data_writes[1] : uint16_t{0xDEAD};
        };
        const uint16_t baseline = probe_bsh(0);
        const uint16_t bit15 = probe_bsh(1u << 3);
        const uint16_t bit14 = probe_bsh(1u << 2);
        const uint16_t bit13 = probe_bsh(1u << 1);
        const uint16_t bit12 = probe_bsh(1u << 0);

        checks.expect(
            bit15 != baseline && bit14 != baseline && bit13 != baseline && bit12 != baseline &&
            bit15 != bit14 && bit14 != bit13 && bit13 != bit12 &&
            bit15 != bit13 && bit15 != bit12 && bit14 != bit12,
            "AREA-BSH-BITS-15-12-ARE-B-SHIFT-VALUE: setting BLTCON1 bit 15, 14, 13 "
            "or 12 alone each produces a shifted B-channel output distinct from the "
            "bsh=0 baseline AND from each other, confirming all four are "
            "independently-live bits of the B-shift field - table row '15 ... "
            "BSH3' through '12 ... BSH0' (appendix A p.267, no naming dispute on "
            "this column: BSH3-0 in both the row and the legend), 'the B shift "
            "value is set with bits 15 through 12 of BLTCON1' (p.179). "
            "agnus_blitter.v:274 `assign bsh[3:0] = bltcon1[15:12]`, :400 wires it "
            "to shifter B. "
            "6-appa-control.c2, 6-appa-control.c4, 6-appa-control.c6, "
            "6-appa-control.c8, 6-appa-control.c34, "
            "ch6-appa-bltcon0.area.b15.con1, ch6-appa-bltcon0.area.b14.con1, "
            "ch6-appa-bltcon0.area.b13.con1, ch6-appa-bltcon0.area.b12.con1, "
            "ch6-appa-bltcon0.legend.BSH30, 6-shifts-and-masks.c9, ch6-shifts.c9 .");
    }

    // ---------------- AREA-USE-BITS-11-8-SELECT-CHANNELS ----------------
    {
        // Clearing one USE bit alone (holding the other three set) removes
        // exactly that channel from the granted set on a one-word blit,
        // while the other three are still granted.
        auto grants_seen = [](uint16_t use_bits) {
            Harness h;
            h.a_words = {0x1111};
            h.b_words = {0x2222};
            h.c_words = {0x3333};
            start_blit(h, use_bits, 0xF0 /* LF: D=A passthrough */, 1, 1);
            h.run_until_writes(1, 80);
            h.settle();
            bool a = false, b = false, c = false, d = false;
            for (auto& g : h.grants) {
                if (g.channel == 'A') a = true;
                else if (g.channel == 'B') b = true;
                else if (g.channel == 'C') c = true;
                else if (g.channel == 'D') d = true;
            }
            return std::vector<bool>{a, b, c, d};
        };
        const auto all4 = grants_seen(USEA | USEB | USEC | USED);
        const auto noA = grants_seen(USEB | USEC | USED);
        const auto noB = grants_seen(USEA | USEC | USED);
        const auto noC = grants_seen(USEA | USEB | USED);
        const auto noD = grants_seen(USEA | USEB | USEC);

        checks.expect(
            all4[0] && all4[1] && all4[2] && all4[3] &&
            !noA[0] && noA[1] && noA[2] && noA[3] &&
            noB[0] && !noB[1] && noB[2] && noB[3] &&
            noC[0] && noC[1] && !noC[2] && noC[3] &&
            noD[0] && noD[1] && noD[2] && !noD[3],
            "AREA-USE-BITS-11-8-SELECT-CHANNELS: clearing BLTCON0 bit 11 (USEA), "
            "10 (USEB), 9 (USEC) or 8 (USED) alone removes exactly that channel "
            "from the set of channels granted on a one-word blit, while the other "
            "three channels are still granted - 'enable bits are bits SRCA, SRCB, "
            "SRCC, and DEST in control register zero' (p.170), 'USEA Mode control "
            "bit to use source A' etc (p.267 legend), table rows '11 USEA' through "
            "'08 USED' (p.267). agnus_blitter.v:252 `assign {usea,useb,usec,used} "
            "= {bltcon0[11:9], bltcon0[8]&enad}`, :678,701,722,745 gate each "
            "channel's own dma_req on its own bit; :724-726 (BLT_C only reaches "
            "BLT_D, D's only entry point, when `used`). "
            "6-appa-control.c9, 6-appa-control.c11, 6-appa-control.c13, "
            "6-appa-control.c15, 6-appa-control.c35, 6-appa-control.c36, "
            "6-appa-control.c37, 6-appa-control.c38, "
            "ch6-appa-bltcon0.area.b11.con0, ch6-appa-bltcon0.area.b10.con0, "
            "ch6-appa-bltcon0.area.b09.con0, ch6-appa-bltcon0.area.b08.con0, "
            "ch6-appa-bltcon0.legend.USEA, ch6-appa-bltcon0.legend.USEB, "
            "ch6-appa-bltcon0.legend.USEC, ch6-appa-bltcon0.legend.USED, "
            "6-dma-channels.c25, 6-dma-channels.c26, 6-dma-channels.c9, "
            "ch6-dma.c25, ch6-dma.c26, ch6-dma.c7, ch6-fig613.f27 .");
    }

    // ---------------- BLTCON0-BITS-7-0-ARE-THE-LF-BYTE-IN-BOTH-MODES ----------------
    {
        // LF7 (bit7) and LF0 (bit0) are the two ends of the LF byte
        // BLTCON0[7:0]; bits 6-1 in between share the identical per-bit
        // mechanism (agnus_blitter_minterm.v's `for` loop ties each lf[k]
        // directly to its own minterm block, :29-40), so sampling the two
        // ends pins the byte's position without re-deriving the full truth
        // table (PKG-BLT-MINTERM's job, Non-goals). Eight one-word blits
        // enumerate every (A,B,C) selector combination 0..7 (MSB-first,
        // matching agnus_blitter_minterm.v's own convention). This IS the
        // SAME physical wire in both modes (agnus_blitter.v:444's
        // `.lf(bltcon0[7:0])` is unconditional on `line`) - but that wiring
        // fact alone does not entitle an AREA-MODE run to credit the
        // LINE-MODE table's own bit-identity ids: those 17 ids
        // (6-appa-control.c61 through c79 and ch6-appa-bltcon0.line.b07.con0
        // through b00.con0) belong instead to
        // LINE-MODE-LF-BITS-EACH-ALONE-MATCH-P174-MAPPING below, which runs
        // actual line-mode blits and varies B and C so every LF bit is read
        // at some position. This assertion keeps only the
        // AREA-MODE table's own ids.
        auto probe_lf_bit = [](uint16_t lf_bit) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            std::vector<uint16_t> a, b, c;
            for (int k = 0; k < 8; ++k) {
                a.push_back((k & 4) ? 0x8000 : 0x0000);
                b.push_back((k & 2) ? 0x8000 : 0x0000);
                c.push_back((k & 1) ? 0x8000 : 0x0000);
            }
            h.a_words.assign(a.begin(), a.end());
            h.b_words.assign(b.begin(), b.end());
            h.c_words.assign(c.begin(), c.end());
            start_blit(h, USEA | USEB | USEC | USED, static_cast<uint16_t>(lf_bit), 8, 1);
            h.run_until_writes(8, 200);
            h.settle();
            return h.data_writes;
        };
        const auto lf7 = probe_lf_bit(0x80);
        const auto lf0 = probe_lf_bit(0x01);
        bool lf7_ok = lf7.size() == 8;
        bool lf0_ok = lf0.size() == 8;
        // Only bit 15 carries the per-word (A,B,C) payload -- every word's
        // bits 14..0 are 0 for A, B AND C on every probe word (the stimulus
        // only ever sets bit 15), so those low bits ALWAYS satisfy the
        // all-zero minterm (mt0) whenever LF bit 0 is set, regardless of k.
        // A full 16-bit equality check against 0x8000/0x0000 therefore
        // spuriously fails the LF0 probe (every word reads back 0x7FFF or
        // 0xFFFF, not 0x0000/0x8000) even though bit 15 -- the only bit this
        // probe actually controls -- is exactly right. Masking to bit 15
        // is the fix, not a relaxed oracle: it is what the stimulus can
        // actually prove, and was the original agent's own stated intent
        // ("sampling the two end bits LF7/LF0", non-goals above).
        for (int k = 0; k < 8 && lf7_ok; ++k) lf7_ok = lf7_ok && ((lf7[k] & 0x8000) == (k == 7 ? 0x8000 : 0x0000));
        for (int k = 0; k < 8 && lf0_ok; ++k) lf0_ok = lf0_ok && ((lf0[k] & 0x8000) == (k == 0 ? 0x8000 : 0x0000));

        checks.expect(lf7_ok && lf0_ok,
            "BLTCON0-BITS-7-0-ARE-THE-LF-BYTE: with only LF bit 7 "
            "set (0x80), only the all-(A,B,C)-ones selector word produces a 1 in "
            "the output; with only LF bit 0 set (0x01), only the all-zeros "
            "selector word does - pinning bits 7 and 0 as the two ends of the "
            "8-bit LF field, in the AREA-MODE table ('07 LF7' and '00 "
            "LF0', p.267). This run measures LF7 and LF0 only, so it does "
            "not carry the ids for bits 6-1 or the whole-field ids - see "
            "BLTCON0-BITS-7-0-EACH-BIT-SELECTS-ONLY-ITS-OWN-MINTERM below, "
            "which measures all eight. The physical wire is unconditional "
            "on `line` (agnus_blitter.v:255-261, :444), but the LINE-MODE "
            "table's own bit-identity ids are graded by an actual line-mode "
            "run instead - see LINE-MODE-LF-BITS-EACH-ALONE-MATCH-P174-"
            "MAPPING below. "
            "6-appa-control.c17, 6-appa-control.c31, "
            "ch6-appa-bltcon0.area.b07.con0, ch6-appa-bltcon0.area.b00.con0 .");
    }

    // ---------------- BLTCON0-BITS-7-0-EACH-BIT-SELECTS-ONLY-ITS-OWN-MINTERM ----------------
    {
        // Figure 6-13 (p.199, ch6-fig613.f27): "Multi-minterm select bits
        // (8)" draws eight separate lines from the control block to eight
        // separate AND gates, one per LF bit. BLTCON0-BITS-7-0-ARE-THE-LF-
        // BYTE-IN-BOTH-MODES above samples only bit 7 and bit 0, the two
        // ends of the byte; it does not show the other six select lines
        // come from BLTCON0 at all. This assertion carries the id instead:
        // the same probe function, run for every one of the eight LF bits
        // k=0..7 set alone, showing each selects ONLY its own selector word
        // k and no other -- the whole of what the figure's eight lines
        // state, not just the two ends.
        auto probe_lf_bit = [](uint16_t lf_bit) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            std::vector<uint16_t> a, b, c;
            for (int k = 0; k < 8; ++k) {
                a.push_back((k & 4) ? 0x8000 : 0x0000);
                b.push_back((k & 2) ? 0x8000 : 0x0000);
                c.push_back((k & 1) ? 0x8000 : 0x0000);
            }
            h.a_words.assign(a.begin(), a.end());
            h.b_words.assign(b.begin(), b.end());
            h.c_words.assign(c.begin(), c.end());
            start_blit(h, USEA | USEB | USEC | USED, static_cast<uint16_t>(lf_bit), 8, 1);
            h.run_until_writes(8, 200);
            h.settle();
            return h.data_writes;
        };
        bool all_ok = true;
        for (int k = 0; k < 8 && all_ok; ++k) {
            const auto out = probe_lf_bit(static_cast<uint16_t>(1u << k));
            if (out.size() != 8) { all_ok = false; break; }
            for (int j = 0; j < 8; ++j) {
                const uint16_t expect_bit = (j == k) ? 0x8000 : 0x0000;
                if ((out[j] & 0x8000) != expect_bit) { all_ok = false; break; }
            }
        }

        checks.expect(all_ok,
            "BLTCON0-BITS-7-0-EACH-BIT-SELECTS-ONLY-ITS-OWN-MINTERM: for each "
            "of the eight LF bits k=0..7 set alone (all seven others clear), "
            "ONLY the selector word whose (A,B,C) triple equals k produces a "
            "1 in the output, and every other selector word of the eight "
            "produces a 0 - Figure 6-13's 'Multi-minterm select bits (8)' "
            "(p.199), drawn as eight separate lines from the control block "
            "to eight separate AND gates, one per LF bit, matching the "
            "AREA-MODE table rows '07 LF7' through '00 LF0' (p.267) and the "
            "legend and mechanism sentences for the byte as a whole - 'LF7-0 "
            "Logic function minterm select lines' (p.267 legend), 'There are "
            "eight possible combinations of values of the three bits, for "
            "each of which we need to specify the corresponding destination "
            "bit as a zero or one' (p.174), 'the LF (Logic Function) control "
            "byte in the BLTCON0 register' (p.174), 'bits 6 through 0 of the "
            "LF control byte, which specify the D output for these "
            "situations, are set to 0' (p.174). This run measures all eight "
            "bits individually, so it -- not BLTCON0-BITS-7-0-ARE-THE-LF-"
            "BYTE, which measures only bits 7 and 0 -- carries the ids for "
            "bits 6-1 and the whole-field ids. agnus_blitter_minterm.v:29-40 ties "
            "each lf[k] to its own AND gate before the final OR at :44 - "
            "eight independent gates, not one shared enable. "
            "6-appa-control.c19, 6-appa-control.c21, 6-appa-control.c23, "
            "6-appa-control.c25, 6-appa-control.c27, 6-appa-control.c29, "
            "ch6-appa-bltcon0.area.b06.con0, ch6-appa-bltcon0.area.b05.con0, "
            "ch6-appa-bltcon0.area.b04.con0, ch6-appa-bltcon0.area.b03.con0, "
            "ch6-appa-bltcon0.area.b02.con0, ch6-appa-bltcon0.area.b01.con0, "
            "6-appa-control.c39, ch6-appa-bltcon0.legend.LF70, "
            "6-function-generator.c6, ch6-funcgen.c10, ch6-funcgen.c4, "
            "ch6-fig613.f27 .");
    }

    // ---------------- BLTCON0L-ALIASES-LF-BYTE-WHEN-ECS ----------------
    {
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0x8000};
        h.b_words = {0x8000};
        h.c_words = {0x0000};
        write_bltcon0(h, 0, USEA | USEB | USEC | USED, 0x00);  // LF=0 via BLTCON0: always-zero
        h.write_reg(BLTCON1, 0x0000);
        h.set_ecs(true);
        h.write_reg(BLTCON0L, 0xCC);  // LF: D=B passthrough, via the ECS-only alias register
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_until_writes(1, 60);
        h.settle();

        checks.expect(!h.data_writes.empty() && h.data_writes[0] == 0x8000,
            "BLTCON0L-ALIASES-LF-BYTE-WHEN-ECS: BLTCON0 first sets LF=0x00 "
            "(always-zero function); with ecs=1, writing BLTCON0L=0xCC (D=B "
            "passthrough) changes the SAME LF byte, and the stored word becomes "
            "the fed B word (0x8000) instead of 0x0000 - 'BLTCON0L 05A W A(E) "
            "Blitter control 0, lower 8 bits (minterms)' (appendix A p.269). "
            "agnus_blitter.v:259 `reg_address_in[8:1]==BLTCON0L[8:1] && ecs` "
            "loads bltcon0[7:0] from a second register address, gated on ecs. "
            "ch6-appa-bltcon0l.c4 .");
    }

    // ---------------- AREA-MODE-BLTCON1-BITS-11-10-9-8-6-5-ARE-UNUSED ----------------
    {
        // CONTROL: BLTCON1 bit 1 (DESC) is genuinely wired
        // (agnus_blitter.v:286) - reversing it changes the A-channel
        // address_out direction over a 2-word blit. If this fixture could
        // not see THAT, it could not be trusted to see anything below.
        auto area_signature = [](uint16_t con1_extra) {
            Harness h;
            h.write_reg(BLTAPTH, 0);
            h.write_reg(BLTAPTL, 0x0100);
            h.write_reg(BLTDPTH, 0);
            h.write_reg(BLTDPTL, 0x0100);
            h.write_reg(BLTAMOD, 0);
            h.write_reg(BLTDMOD, 0);
            h.a_words = {0x1111, 0x2222};
            write_bltcon0(h, 0, USEA | USED, 0xF0);
            h.write_reg(BLTCON1, con1_extra);
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_until_done(200);
            h.settle();
            std::string sig;
            for (auto& g : h.grants) sig += std::to_string(g.address) + ",";
            return sig;
        };
        const std::string baseline = area_signature(0);
        const std::string control_desc = area_signature(1u << 1);
        const uint8_t unused_bits[] = {11, 10, 9, 8, 6, 5};
        bool all_unchanged = true;
        for (uint8_t b : unused_bits)
            if (area_signature(static_cast<uint16_t>(1u << b)) != baseline) all_unchanged = false;

        checks.expect(control_desc != baseline && all_unchanged,
            "AREA-MODE-BLTCON1-BITS-11-10-9-8-6-5-ARE-UNUSED [control: setting bit "
            "1 (DESC), on the identical measurement, DOES change the address_out "
            "sequence]: setting BLTCON1 bit 11, 10, 9, 8, 6 or 5 alone leaves the "
            "address_out sequence over a 2-word blit IDENTICAL to the all-clear "
            "baseline - table rows '11 X' through '08 X' and '06 X'/'05 X' "
            "(appendix A p.267). agnus_blitter.v:277-283 stores BLTCON1[11:0] "
            "wholesale; no expression in the file reads bits 11, 10, 9, 8, 6 or 5. "
            "6-appa-control.c10, 6-appa-control.c12, 6-appa-control.c14, "
            "6-appa-control.c16, 6-appa-control.c20, 6-appa-control.c22, "
            "ch6-appa-bltcon0.area.b11.con1, ch6-appa-bltcon0.area.b10.con1, "
            "ch6-appa-bltcon0.area.b09.con1, ch6-appa-bltcon0.area.b08.con1, "
            "ch6-appa-bltcon0.area.b06.con1, ch6-appa-bltcon0.area.b05.con1 .");
    }

    // ---------------- AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL ----------------
    {
        // This predicate pins the manual's own worked example verbatim
        // (p.184: "00100100-00011000" filled exclusively becomes
        // "00011100-00001000", i.e. 0x2418 -> 0x1C08), not merely "any
        // change from the unfilled baseline". DESC is set in the fill run,
        // since p.184 also says "Descending mode must be used" for the fill
        // examples -- every fill run in this file sets DESC for that
        // reason.
        auto efe_output = [](bool efe) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.a_words = {0x2418};
            h.c_words = {0x0000};
            write_bltcon0(h, 0, USEA | USEC | USED, 0xF0);  // D=A passthrough into fill
            const uint16_t DESC = 1u << 1;
            h.write_reg(BLTCON1, static_cast<uint16_t>(DESC | (efe ? (1u << 4) : 0)));
            h.write_reg(BLTSIZE, bltsize_word(1, 1));
            h.run_until_writes(1, 60);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        const uint16_t off = efe_output(false);
        const uint16_t on = efe_output(true);

        checks.expect(off == 0x2418 && on == 0x1C08,
            "AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL: with BLTCON1 bit 4 clear the "
            "minterm output (0x2418, DESC set) passes through unfilled; with "
            "it set, the SAME input is exclusive-filled into 0x1C08 - the "
            "manual's own worked example: 'with exclusive fill, the result "
            "would be 00011100-00001000' for an input of "
            "'00100100-00011000' (p.184), table row '04 LF4 EFE' (p.267, no "
            "dispute: p.184 and p.267 agree, and p.185's disputed bit is "
            "BLTCON1 bit 3, not bit 4). agnus_blitter.v:287 `assign efe = ~line "
            "& bltcon1[4]`, :455 feeds the fill block's efe input. "
            "6-appa-control.c24, 6-appa-control.c40, "
            "ch6-appa-bltcon0.area.b04.con1, ch6-appa-bltcon0.legend.EFE, "
            "6-area-fill.c9, ch6-fill.c8, ch6-fig613.f27 .");
    }

    // ---------------- FILL-CARRY-IN / IFE-BIT3: WITHDRAWN, PAGES DISAGREE ----------------
    {
        // The page conflict, verified on the scans:
        //   p.267 (appendix A legend and table): bit 3 = IFE, bit 2 = FCI.
        //   p.184: "inclusive-fill-enable bit (FILL_OR, or bit 3)" - agrees
        //     with p.267 on bit 3.
        //   p.185: "another bit (FILL_CARRYIN or bit 3 in BLTCON1)" - names
        //     bit 3 for the CARRY-IN, disagreeing with both p.267 and p.184.
        // The pipeline's own classifier already resolved every claim naming
        // FCI's or bit 3's bit number to not-asserted (6-appa-control.c26/
        // c28/c41/c42, ch6-appa-bltcon0.area.b02.con1/b03.con1,
        // legend.FCI/IFE) - none of those ids are cited by this package.
        // ch6-fig613.f27 (Figure 6-13's "Fill Carry In" / "Fill Enable"
        // lines, which name no bit numbers at all) is already carried by
        // AREA-ASH, AREA-USE, BLTCON0-BITS-7-0-EACH-BIT and AREA-EFE-BIT4
        // above, and by the neutral assertion below: none of the five
        // grounds a bit-3- or bit-2-specific claim, so f27 does not settle
        // the bit-number question either.
        //
        // The measurement below (DESC set, per p.184's "Descending mode
        // must be used") is printed as a MEASURED note first; this package
        // takes no side on which page's bit numbering is right. The
        // assertion that follows it grades a narrower, bit-number-neutral
        // claim instead (see its own comment).
        auto fill_output = [](uint16_t a_word, uint16_t con1_bits) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.a_words = {a_word};
            h.c_words = {0x0000};
            write_bltcon0(h, 0, USEA | USEC | USED, 0xF0);
            const uint16_t DESC = 1u << 1;
            h.write_reg(BLTCON1, static_cast<uint16_t>(DESC | con1_bits));
            h.write_reg(BLTSIZE, bltsize_word(1, 1));
            h.run_until_writes(1, 60);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        auto hex4 = [](uint16_t v) {
            char buf[8];
            std::snprintf(buf, sizeof buf, "0x%04X", v);
            return std::string(buf);
        };
        const uint16_t EFE = 1u << 4, IFE_BIT3 = 1u << 3, FCI_BIT2 = 1u << 2;
        const uint16_t carry_via_bit2 = fill_output(0x2418, static_cast<uint16_t>(EFE | FCI_BIT2));
        const uint16_t carry_via_bit3 = fill_output(0x2418, static_cast<uint16_t>(EFE | IFE_BIT3));
        const uint16_t plain_exclusive = fill_output(0x2418, EFE);
        const uint16_t inclusive = fill_output(0x2418, IFE_BIT3);
        const bool bit2_matches_page = (carry_via_bit2 == 0xE3F7);
        const bool bit3_matches_plain = (carry_via_bit3 == plain_exclusive);

        checks.note("with DESC set and an exclusive fill (EFE) of 0x2418, "
            "forcing the fill carry-in through BLTCON1 bit 2 gives " +
            hex4(carry_via_bit2) + " (" +
            (bit2_matches_page ? "matches" : "does NOT match") +
            " p.185's own worked FCI-exclusive value 0xE3F7), while forcing "
            "it through bit 3 instead gives " + hex4(carry_via_bit3) + " (" +
            (bit3_matches_plain ? "identical to" : "differs from") +
            " the plain-exclusive baseline " + hex4(plain_exclusive) +
            ", measured here with neither bit set). agnus_blitter.v:288 "
            "`assign ife = ~line & bltcon1[3]`, :464 `assign fci = "
            "first_word_del ? bltcon1[2] : fcy`. A source observation, not "
            "a conformance finding: the page conflict is not resolved by "
            "running the RTL.");

        // Figure 6-13 (p.199, ch6-fig613.f27) draws "Fill Carry In" as one
        // of the five control lines the figure names. This assertion
        // establishes that sub-fact's substance without taking a side on
        // the bit-number conflict above: p.185's own worked FCI-exclusive
        // example, $E3F7, is
        // produced by forcing the carry-in through BLTCON1 bit 2 OR by
        // forcing it through bit 3 - true whichever page's numbering this
        // RTL happens to follow, so the assertion does not depend on the
        // still-open question above.
        checks.expect(carry_via_bit2 == 0xE3F7 || carry_via_bit3 == 0xE3F7,
            "FILL-CARRY-IN-PRODUCES-P185-WORKED-EXAMPLE: with DESC set and "
            "an exclusive fill of 0x2418, forcing the fill carry-in through "
            "EITHER BLTCON1 bit 2 or bit 3 produces p.185's own worked "
            "exclusive-fill result, $E3F7 - 'for the above example, ... "
            "with exclusive fill, the output would be 11100011-11110111' "
            "(p.185); the same sentence begins 'There is another bit "
            "(FILL_CARRYIN or bit 3 in BLTCON1) that forces the area "
            "\"outside\" the lines be filled', naming bit 3, the disputed "
            "number. Figure 6-13's 'Fill Carry In' line (p.199, "
            "ch6-fig613.f27) is BLTCON0/BLTCON1-programmable either way, "
            "which is the substance the figure claim needs; which specific "
            "bit number is FCI is the separate, unresolved question recorded "
            "below. "
            "ch6-fig613.f27 .");

        checks.record(
            "FILL-CARRY-IN-BIT2-AND-AREA-IFE-BIT3: BLTCON1 bit 2 (FCI, 'Fill "
            "carry input') and bit 3 (IFE, 'Inclusive fill enable') per "
            "appendix A p.267's table and legend - but p.185 reads 'another "
            "bit (FILL_CARRYIN or bit 3 in BLTCON1) that forces the area "
            "\"outside\" the lines be filled', naming bit 3 for the carry-in "
            "and disagreeing with p.267 (and with p.184's 'FILL_OR, or bit "
            "3' for IFE, which agrees with p.267 on bit 3). Two pages "
            "against one on the same bit number; whether p.185 is a misprint "
            "is a question for Amiga experts, not for this package, so "
            "neither bit number is graded until one rules. The inclusive-"
            "fill VALUE for a bit-3 IFE reading is recorded, not asserted: "
            "with BLTCON1 bit 3 set, the input 0x2418 fills to " +
            hex4(inclusive) + " (the page's own worked example, p.184, "
            "prints '00111100-00011000' = 0x3C18) - only the BIT "
            "NUMBER is withdrawn, not the fill behaviour.");
    }

    // ---------------- AREA-DESC-BIT1-SELECTS-DESCENDING-MODE ----------------
    {
        auto direction = [](bool desc) {
            Harness h;
            h.write_reg(BLTAPTH, 0);
            h.write_reg(BLTAPTL, 0x0100);
            h.write_reg(BLTDPTH, 0);
            h.write_reg(BLTDPTL, 0x0100);
            h.a_words = {0x1111, 0x2222};
            write_bltcon0(h, 0, USEA | USED, 0xF0);
            h.write_reg(BLTCON1, desc ? static_cast<uint16_t>(1u << 1) : 0x0000);
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_until_done(200);
            h.settle();
            long first = -1, second = -1;
            for (auto& g : h.grants)
                if (g.channel == 'A') { if (first < 0) first = g.address; else if (second < 0) second = g.address; }
            return second - first;
        };
        const long ascending_delta = direction(false);
        const long descending_delta = direction(true);

        checks.expect(ascending_delta > 0 && descending_delta < 0,
            "AREA-DESC-BIT1-SELECTS-DESCENDING-MODE: with BLTCON1 bit 1 clear the "
            "A-channel address increases word to word; with it set, the SAME blit "
            "decreases - table row '01 LF1 DESC' (p.267), 'DESC Descending "
            "(decreasing address) control bit' (p.267 legend). agnus_blitter.v:286 "
            "`assign desc = ~line & bltcon1[1]`, :674-675 `incptr=~desc`/"
            "`decptr=desc`. "
            "6-appa-control.c30, 6-appa-control.c43, "
            "ch6-appa-bltcon0.area.b01.con1, ch6-appa-bltcon0.legend.DESC .");
    }

    // ---------------- BLTCON1-BIT0-SELECTS-AREA-OR-LINE-MODE ----------------
    {
        // The SAME BLTCON0 (USEA|USEC|USED, ash=14, LF=0) and the SAME
        // BLTSIZE(width=2,height=1) word behave as two entirely different
        // FSMs depending on this one bit: as an ordinary 2-word rectangle
        // (2 destination writes) with LINE=0, or as a single-pixel line
        // draw (1 destination write, the width field no longer meaning
        // "words per row") with LINE=1.
        auto we_count_for = [](bool line_mode) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            write_bltcon0(h, 14, USEA | USEC | USED, 0x00);  // any non-zero shift; not this claim's subject
            h.write_reg(BLTCON1, line_mode ? LINE_BIT : uint16_t{0x0000});
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_cycles(70);
            h.settle();
            int we = 0;
            for (auto& s : h.log) if (s.we) ++we;
            return we;
        };
        const int area_we = we_count_for(false);
        const int line_we = we_count_for(true);

        checks.expect(area_we == 2 && line_we == 1,
            "BLTCON1-BIT0-SELECTS-AREA-OR-LINE-MODE: the identical BLTCON0/"
            "BLTSIZE register values produce 2 destination writes (an ordinary "
            "2-word-wide, 1-row rectangle) with BLTCON1 bit 0 clear, and exactly "
            "1 destination write (a single-pixel line draw, the same BLTSIZE "
            "word now read as a pixel count, not a rectangle) with it set - "
            "'There are two basic modes, area and line, which are selected by "
            "bit 0 of BLTCON1' (p.267), table rows '00 LF0 LINE(=0)' (p.267) and '00 LF0 "
            "LINE(=1)' (p.268). agnus_blitter.v:285 `assign line = bltcon1[0]`, "
            ":659-663 `if (line) blt_next=BLT_L1; else blt_next=BLT_A;` is the "
            "only branch point between the two FSMs. "
            "6-appa-control.c32, 6-appa-control.c76, 6-appa-control.c44, "
            "6-appa-control.c86, "
            "ch6-appa-bltcon0.area.b00.con1, ch6-appa-bltcon0.line.b00.con1, "
            "ch6-appa-bltcon0.legend.LINE, ch6-appa-lf.legend.LINE, "
            "ch6-appa-bltcon0.c7, ch6-appa-bltcon0.c8, ch6-appa-bltcon0.c9, "
            "6-appa-registers.c22, 6-appa-registers.c23, 6-appa-registers.c24 .");
    }

    // ============================================================
    // LINE MODE (appendix A p.268)
    // ============================================================

    // ---------------- LINE-MODE-BLTCON0-BITS-11-10-9-8: DECLINED, SOURCE OBSERVATIONS KEPT ----------------
    {
        // Grading bit 9 (USEC) as load-bearing by p.170's general "disabled
        // channel, no memory cycles" rule while declining its siblings 11,
        // 10 and 8 by the SAME page for lack of a page-derived expectation
        // is not an available split: either p.170 governs these four bits
        // in line mode, or it does not.
        //   - If it does: USED=0 should predict no D writes and USEB=1
        //     should predict B fetches. Neither happens in this RTL (see
        //     the MEASURED note below) -- so grading bit 9 by p.170 while
        //     declining 11/10/8 by the same page is inconsistent.
        //   - If it does not (p.189: line mode "changes the meaning of some
        //     other bits in BLTCON0 and BLTCON1"; p.268 prints the line-mode
        //     constants "1 0 1 1" where p.267 prints USEA..USED as area-mode
        //     enables): then bit 9 has no page-derived expectation either.
        // Either way, none of the four bits is graded here. The manual
        // prescribes the fixed line-mode pattern and states no consequence
        // for a different one; silence is not a requirement.
        auto c_granted = [](bool usec) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            const uint16_t use = static_cast<uint16_t>(USEA | USED | (usec ? USEC : 0));
            write_bltcon0(h, 14, use, 0x00);
            h.write_reg(BLTCON1, LINE_BIT);
            h.write_reg(BLTSIZE, bltsize_word(2, 2));
            h.run_cycles(60);
            for (auto& g : h.grants) if (g.channel == 'C') return true;
            return false;
        };
        const bool with_usec = c_granted(true);
        const bool without_usec = c_granted(false);

        auto line_signature = [](uint16_t use_bits) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            write_bltcon0(h, 14, use_bits, 0x00);
            h.write_reg(BLTCON1, LINE_BIT);
            h.write_reg(BLTSIZE, bltsize_word(2, 2));
            h.run_cycles(60);
            h.settle();
            std::string sig;
            for (auto& g : h.grants) sig += g.channel;
            for (auto& s : h.log) sig += s.we ? '1' : '0';
            return sig;
        };
        const std::string documented = line_signature(USEA | USEC | USED);
        const std::string no_a = line_signature(USEC | USED);
        const std::string b_set = line_signature(USEA | USEB | USEC | USED);
        const std::string no_d = line_signature(USEA | USEC);
        const bool usec_differs = (with_usec != without_usec);
        const bool no_a_equal = (no_a == documented);
        const bool b_set_equal = (b_set == documented);
        const bool no_d_equal = (no_d == documented);

        // Source observations, not conformance findings (this package's own
        // "reading inside the module" standard: would this red still be red
        // at the pins, and does any page ask the question at all? Here no
        // page asks it, so there is nothing for a red OR a green to confirm
        // or contradict).
        checks.note(std::string("agnus_blitter.v:823 `dma_req = usec;` inside "
            "BLT_L2: with USEC set, channel C granted = ") +
            (with_usec ? "true" : "false") +
            "; with USEC clear, channel C granted = " +
            (without_usec ? "true" : "false") + "; the two values differ = " +
            (usec_differs ? "true" : "false") + ".");
        checks.note("agnus_blitter.v:795-869 (BLT_L1-BLT_L4) never reads usea, "
            "useb or used: the grant/we signature of a 2-pixel line blit, "
            "documented pattern (USEA|USEC|USED) = \"" + documented + "\", "
            "USEA cleared = \"" + no_a + "\" (equal to documented = " +
            (no_a_equal ? "true" : "false") + "), USEB set = \"" + b_set +
            "\" (equal to documented = " + (b_set_equal ? "true" : "false") +
            "), USED cleared = \"" + no_d + "\" (equal to documented = " +
            (no_d_equal ? "true" : "false") + ").");

        checks.record(
            "LINE-MODE-BLTCON0-BITS-11-10-9-8-REQUIRED-PATTERN-NOT-GRADED-HERE: "
            "the line-mode table requires BLTCON0 bits 11, 10, 9 and 8 to hold "
            "1, 0, 1 and 1 - 'The SRCA, SRCC, and DEST bits of BLTCON0 should be "
            "set to one, and the SRCB flag should be set to zero' (p.191), "
            "table rows '11 1 0', '10 0 0', '09 1 0', '08 1 0' (appendix A "
            "p.268) - but no page states what the blitter does with a "
            "different pattern in any of the four. Two readings are "
            "available and neither settles it: p.170's general 'Each of the "
            "DMA channels can be independently enabled or disabled ... When "
            "disabled, no memory cycles will be executed for that channel' "
            "would predict specific misbehaviour for USEB/USED that "
            "this RTL does not show (see the MEASURED note above), and p.189 "
            "says line mode 'changes the meaning of some other bits in "
            "BLTCON0 and BLTCON1' in the first place, which would mean p.170 "
            "simply does not apply to any of these four bits in line mode - "
            "in which case bit 9 has no page-derived expectation either, "
            "despite the RTL happening to gate channel C's fetch on it. This "
            "package does not assert full line-mode pixel-by-pixel "
            "correctness (that is PKG-BLT-LINE's job, per this file's own "
            "Non-goals), so there is no in-scope observable left to grade "
            "any of the four ids by. Withdrawn rather than graded by the "
            "RTL's own behaviour either way. "
            "6-appa-control.c57, 6-line-mode.c62, 6-line-mode.c87, "
            "ch6-appa-bltcon0.line.b09.con0, "
            "6-line-mode.c61, 6-line-mode.c63, 6-line-mode.c64, 6-line-mode.c86, "
            "6-line-mode.c88, 6-line-mode.c89, ch6-line.c32, ch6-line.c33, "
            "ch6-linesum.c13, ch6-linesum.c14, 6-appa-control.c53, "
            "6-appa-control.c55, 6-appa-control.c59, "
            "ch6-appa-bltcon0.line.b11.con0, ch6-appa-bltcon0.line.b10.con0, "
            "ch6-appa-bltcon0.line.b08.con0 .");

        // 6-line-mode.c68 is about the C channel's role in the line-mode
        // logic function, a different subject from bit 9's fetch-gating
        // question above, so it is recorded on its own.
        checks.record(
            "LINE-MODE-USEC-C68-NOT-USED: 6-line-mode.c68's own restatement "
            "reads 'The C DMA channel represents the original destination "
            "data'; the page (p.191) reads 'The C DMA channel represents "
            "the original source, the A channel the bit to set in the "
            "line, and the B channel the pattern to draw' - the "
            "restatement does not match the page (source, not destination "
            "data), and no proposition in this package's specification "
            "rests on this id. "
            "6-line-mode.c68 .");
    }

    // ---------------- LINE-MODE-BLTCON1 REQUIRED-ZERO IDS: CARRIED ON SING'S OWN RUN ----------------
    {
        // An ad-hoc line (all-zero pointers/moduli, START=15 -- a value
        // p.191/192's "x1 modulo 15" cannot produce) showing that the
        // documented zero pattern for BLTCON1 bits 11,10,9,8,7,5 "draws a
        // correctly-counted line" via a raw D-channel write COUNT is not
        // what any page states (p.269 measures line length in DOTS, and
        // this package's own SING assertion shows the RTL does not write
        // once per dot when SING is set), and would not be a real p.192
        // line to begin with.
        //
        // No separate run is needed: LINE-MODE-SING-BIT1 below already runs
        // a genuine p.192 flat line (dx=2, dy=0, BLTAPT=-4, BLTAMOD=-8,
        // BLTBMOD=0, SIGN=1, octant 7) with BLTCON1 bits 11,10,9,8,7,5 ALL
        // clear in both its runs -- its own SING=0 branch IS the documented
        // zero pattern, drawing a real, page-derived line. These 13 ids are
        // carried there instead of on a separate, weaker run. The "no
        // effect of setting any of them to 1" half is not asserted anywhere
        // (no page states it); OVFLAG is declined separately below, since
        // no page gives its bit position at all.
        checks.record(
            "LINE-MODE-BLTCON1-OVFLAG-BIT-POSITION-UNSTATED: p.191 says 'The "
            "OVFLAG should be cleared' and the p.192 register summary says "
            "'BLTCON1 bit OVFLAG = 0', but no page read for this task states "
            "which of BLTCON1's bit positions OVFLAG occupies - it is not one "
            "of the named bits in the line-mode table (p.268), which lists "
            "START/TEXTURE, the constants above, LF7-0, SIGN, the reserved "
            "bit, SUD/SUL/AUL, SING and LINE. This package does not assign "
            "OVFLAG to bit 11, 10, 9, 8, 7 or 5 without further evidence "
            "(SPECIFICATION.md Proposition 16). "
            "6-line-mode.c65, 6-line-mode.c93, ch6-line.c34, ch6-linesum.c18 .");
    }

    // ---------------- LINE-MODE-START-BITS-15-12-REUSE-THE-ASH-FIELD ----------------
    {
        // The initial value written into BLTCON0[15:12] on a BLTCON0 write
        // is the SAME `ash` field AREA-ASH-BITS-15-13-ARE-A-SHIFT-VALUE
        // measures (agnus_blitter.v:220-230's load is unconditional on
        // `line`); in line mode it is the fractional bit-position tracker
        // that incash/decash step each pixel (agnus_blitter.v:943-944,
        // gated on the SAME octant formula as lineinc/linedec, without the
        // ash==boundary term) and lineinc/linedec (:609-610) fire, moving
        // the actual C/D pointer bank by one word, exactly when ash is AT
        // its 1111/0000 boundary - matching 'The A bit is automatically
        // moved across the word by the hardware' (p.268). CORRECTION: this
        // is NOT the Bresenham error accumulator (that is BLTAPTL/BLTAPTH,
        // a separate 20-bit pointer register read as `bltptr_out` at
        // BLT_L1 and fed the "2Y-X" formula per source conflict #6a,
        // packages.md item 6 - see agnus_blitter_adrgen.v:104-122); ash is
        // the START3-0 bit-position field the table names, nothing more.
        // A fixture with no BLTAPT/modulo (an ad-hoc line, not a page one)
        // using ash=15 would not describe any line the manual's own
        // arithmetic allows: 15 is a START value p.191/192's "x1 modulo 15"
        // cannot produce (modulo 15 gives 0-14 only).
        //
        // This run instead uses the same genuine p.192 flat line
        // LINE-MODE-SING uses (dx=2, dy=0: BLTAPT=-4, BLTAMOD=-8, BLTBMOD=0,
        // SIGN=1, octant 7,
        // LF=$CA, BLTBDAT=$FFFF solid, C=$0000), varying only START3-0
        // (x1=14 versus x1=0 - both within "0 thru 15 hex" AND within "x1
        // modulo 15"). The page's own expectation is that the line's first
        // pixel sits at the START bit position and the hardware "moves the
        // A bit across the word" (p.268) as the line proceeds: starting at
        // bit 14 (one pixel before the word boundary), the third of three
        // pixels crosses into the next word; starting at bit 0, none of the
        // three do.
        auto flat_line_d_addrs = [](uint16_t start) {
            Harness h;
            for (int i = 0; i < 3; ++i) h.c_words.push_back(0x0000);
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0x8000);
            h.write_reg(BLTBDAT, 0xFFFF);
            h.write_reg(BLTAPTH, 0xFFFF);
            h.write_reg(BLTAPTL, 0xFFFC);        // BLTAPT = 4*dy-2*dx = -4 (dx=2,dy=0)
            h.write_reg(BLTAMOD, 0xFFF8);         // 4*(dy-dx) = -8
            h.write_reg(BLTBMOD, 0x0000);         // 4*dy = 0
            h.write_reg(BLTCMOD, 40);
            h.write_reg(BLTDMOD, 40);
            h.write_reg(BLTCPTH, 0);
            h.write_reg(BLTCPTL, 0x1000);
            h.write_reg(BLTDPTH, 0);
            h.write_reg(BLTDPTL, 0x1000);
            write_bltcon0(h, start, USEA | USEC | USED, 0xCA);
            const uint16_t SIGN = 1u << 6, SUD = 1u << 4;  // SIGN=1 per p.192; octant 7
            h.write_reg(BLTCON1, static_cast<uint16_t>(LINE_BIT | SIGN | SUD));
            h.write_reg(BLTSIZE, bltsize_word(2, 3));  // width=2 fixed, height=dx+1=3 (p.191/192)
            h.run_cycles(90);
            h.settle();
            std::vector<uint32_t> addrs;
            for (auto& g : h.grants) if (g.channel == 'D') addrs.push_back(g.address);
            return addrs;
        };
        const std::vector<uint32_t> expect14 = {0x800, 0x800, 0x801};
        const std::vector<uint32_t> expect0  = {0x800, 0x800, 0x800};
        const auto seq14 = flat_line_d_addrs(14);
        const auto seq0 = flat_line_d_addrs(0);

        checks.expect(seq14 == expect14 && seq0 == expect0,
            "LINE-MODE-START-BITS-15-12-REUSE-THE-ASH-FIELD: on p.192's own "
            "flat line (dx=2, dy=0), starting at BLTCON0[15:12]=14 crosses "
            "into the next word on the third pixel (D addresses 0x800, "
            "0x800, 0x801); starting at 0, all three pixels stay in the "
            "first word (0x800, 0x800, 0x800) - confirming line mode's "
            "START3-0 field is the SAME physical bits as area mode's "
            "ASH3-0, genuinely consumed (not merely stored) as the line's "
            "starting bit-position value - table rows '15 START3 ...' "
            "through '12 START0 ...' (appendix A p.268), 'START3-0 "
            "Starting point of line' (p.268 legend), 'The A bit is "
            "automatically moved across the word by the hardware' (p.268). "
            "agnus_blitter.v:609 `wire lineinc = (...) && ash==4'b1111`. "
            "6-appa-control.c45, 6-appa-control.c47, 6-appa-control.c49, "
            "6-appa-control.c51, 6-appa-control.c77, 6-appa-control.c85, "
            "ch6-appa-bltcon0.line.b15.con0, ch6-appa-bltcon0.line.b14.con0, "
            "ch6-appa-bltcon0.line.b13.con0, ch6-appa-bltcon0.line.b12.con0, "
            "ch6-appa-bltcon0.line.start .");
    }

    // ---------------- LINE-MODE-TEXTURE-BITS-15-12-REUSE-THE-BSH-FIELD ----------------
    {
        // A predicate over the ENTIRE 16-bit output word would claim it
        // becomes uniform - that line mode replicates the texture bit
        // across all 16 positions is agnus_blitter.v:410 (`bltbhold <=
        // {16{shiftbout[0]}}`), not anything the page states; p.268/p.192
        // say only which BIT of BLTBDAT the texture read starts at ("0 =
        // last significant bit").
        //
        // This assertion uses p.192's own setup and asserts only the bit at
        // the A position: BLTADAT=$8000 (A's one set bit, per p.192's own
        // register summary), C held at $0000 throughout, LF=$4A (the
        // exclusive-or line function, also p.192's own value). At the bit
        // position where A=1, ABC=A.B.NOT(C) + NOT(A).C reduces (C=0) to
        // just B - i.e. the output AT THAT ONE BIT equals the texture bit
        // the page describes, whatever the other 15 bits do (they are 0
        // here because C=0 and A=0 forces NOT(A).C=0 at every other
        // position - a consequence of this probe's own chosen C, not an
        // assumption about replication).
        auto texture_at_a_bit = [](uint16_t texture) {
            Harness h;
            for (int i = 0; i < 2; ++i) h.c_words.push_back(0x0000);  // C held at 0 (this probe's own choice, not a page value)
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0x8000);  // p.192: "BLTADAT = $8000"
            write_bltcon0(h, 0 /* START, not this claim's subject */, USEA | USEC | USED, 0x4A);
            h.write_reg(BLTCON1, static_cast<uint16_t>(LINE_BIT | (texture << 12)));
            h.write_reg(BLTBDAT, 0x0001);
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_cycles(50);
            h.settle();
            const uint16_t d = h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
            return static_cast<uint16_t>(d & 0x8000);  // only the A-position bit (15)
        };
        const uint16_t texture0 = texture_at_a_bit(0);
        const uint16_t texture15 = texture_at_a_bit(15);

        checks.expect(texture0 == 0x8000 && texture15 == 0x0000,
            "LINE-MODE-TEXTURE-BITS-15-12-REUSE-THE-BSH-FIELD: with BLTADAT="
            "$8000, BLTBDAT's single set bit at position 0, C held at $0000 "
            "(this probe's own choice, isolating the one bit under test), "
            "and LF=$4A (p.192's own exclusive-or byte value), the bit at "
            "the A position reads set with BLTCON1[15:12]=0 (TEXTURE=0, 'start bit "
            "for line texture (0 = last significant bit)', p.192) and clear "
            "with BLTCON1[15:12]=15 (TEXTURE=15) - confirming BLTCON1[15:12] "
            "is the identical BSH3-0 field, genuinely consumed as line "
            "mode's texture-bit selector - table rows '15 ... TEXTURE3' "
            "through '12 ... TEXTURE0' (appendix A p.268), 'The \"B\" source "
            "is used for texturing the drawn lines' (p.268). Only the one "
            "bit the page names is asserted; that line mode also replicates "
            "it across the rest of the word is agnus_blitter.v:410, not "
            "graded here. "
            "6-appa-control.c46, 6-appa-control.c48, 6-appa-control.c50, "
            "6-appa-control.c52, "
            "ch6-appa-bltcon0.line.b15.con1, ch6-appa-bltcon0.line.b14.con1, "
            "ch6-appa-bltcon0.line.b13.con1, ch6-appa-bltcon0.line.b12.con1, "
            "ch6-appa-lf.c8 .");
    }

    // ---------------- LINE-MODE-SIGN-BIT6: WITHDRAWN, OUT OF THIS PACKAGE'S SCOPE ----------------
    {
        // SPECIFICATION.md Proposition 18: the manual states only SIGN's
        // ROLE (a sign flag for the line's initial error term, set when
        // (4*dy-2*dx) < 0) and gives no port-level expected sequence a test
        // could grade against - "this specification asserts only the bit's
        // role..., not the stepping it produces." This package's own header
        // already excludes exactly that: "the sign flag's role across a
        // multi-pixel line" is named as PKG-BLT-LINE's job, not this
        // package's (Non-goals above).
        //
        // Counting D-channel writes with BLTAPT/BLTAMOD/BLTBMOD left at
        // their reset value of 0 would not be a real line (dx=0,dy=0 is
        // degenerate), so SIGN=1 would contradict p.192's own SIGNFLAG rule
        // for any real line the manual describes. LINE-MODE-SING below
        // instead uses a genuinely page-derived flat-line setup (p.191/192:
        // dx=2, dy=0) that DOES need SIGN set correctly as a precondition -
        // SING's own run always
        // sets SIGN=1 and never varies it, so that precondition is assumed
        // there, not independently measured; SIGN's own graded behaviour is
        // declined here, matching P18.
        checks.record(
            "LINE-MODE-SIGN-BIT6: BLTCON1 bit 6 is SIGN - 'SIGN   Sign flag' "
            "(p.268 table/legend), set 'if (((4*dy)-(2*dx)) < 0)' (p.192). "
            "The manual gives no port-level observable for the stepping "
            "SIGN produces, and this package's own Non-goals reserve 'the "
            "sign flag's role across a multi-pixel line' for PKG-BLT-LINE. "
            "Not graded independently here; LINE-MODE-SING below assumes "
            "SIGN=1 as a fixed setup precondition (p.192's own SIGNFLAG rule "
            "for a flat, dy=0 line) and never varies it, so that precondition "
            "is not independently measured, only relied upon. "
            "6-appa-control.c64, 6-appa-control.c87, "
            "ch6-appa-bltcon0.line.b06.con1, ch6-appa-lf.legend.SIGN .");
    }

    // ---------------- LINE-MODE-SUD-SUL-AUL-BITS-4-3-2-AND-OCTANT-TABLE ----------------
    {
        // Table integrity, grounded against the actual PDF scan (see
        // header): appendix A p.268's octant table gives eight (SUD,SUL,
        // AUL) triples, one per octant 0-7 - they must be eight PAIRWISE
        // DISTINCT patterns of three bits, or the table would not define
        // eight octants at all. Separately: SUD (bit 4), SUL (bit 3) and
        // AUL (bit 2) are three independently-live bits, shown the same
        // way as AREA-ASH-BITS-15-13, using the LF=$4A line-mode
        // function's output as the observable. Which octant NUMBER
        // corresponds to which stepping direction is PKG-BLT-LINE's job
        // (Non-goals).
        struct Oct { int n; uint8_t sud, sul, aul; };
        const Oct table[8] = {
            {0, 1, 1, 0}, {1, 0, 0, 1}, {2, 0, 1, 1}, {3, 1, 1, 1},
            {4, 1, 0, 1}, {5, 0, 1, 0}, {6, 0, 0, 0}, {7, 1, 0, 0},
        };
        bool all_distinct = true;
        for (int i = 0; i < 8 && all_distinct; ++i)
            for (int j = i + 1; j < 8; ++j)
                if (table[i].sud == table[j].sud && table[i].sul == table[j].sul && table[i].aul == table[j].aul)
                    all_distinct = false;

        // FIXED (was: data_writes[0], the stored MINTERM word). SUD/SUL/AUL
        // (agnus_blitter.v:609-612) feed ONLY the line-mode address-stepping
        // formulas (incptr/decptr/addmod/submod for the C/D pointer banks) -
        // they never reach agnus_blitter_minterm's ain/bin/cin/lf inputs, so
        // no value of these three bits can ever change a STORED WORD; the
        // chosen observable could not show this claim under any RTL
        // behavior, correct or not.
        //
        // Also FIXED: an all-clear baseline is the wrong control for SUD and
        // AUL specifically. Hand-expanding agnus_blitter.v:609's `lineinc =
        // (SUD&&!AUL) || (!SUD&&!SUL&&!sign_del)` with sign_del held at 0
        // throughout (BLTAPTL/BLTAMOD/BLTBMOD all left at their reset value
        // of 0, so the BLT_L1 accumulator - a SEPARATE register from ash,
        // see LINE-MODE-START's header note above - never moves and
        // sign_out=newptr[15]=0 forever) shows: with SUL=AUL=0 (the all-clear
        // baseline), BOTH `SUD=0` and `SUD=1` make lineinc evaluate to 1 -
        // it is a tautology in this formula, not a fixture bug, and no
        // baseline choice with SUL=AUL=0 could ever show SUD as live. Each
        // bit's own term is gated by a SPECIFIC other bit: SUD's clause is
        // masked by `!AUL`, so SUD only shows through when AUL=1; AUL's
        // clause is masked by `SUD&&`, so AUL only shows through when SUD=1;
        // SUL's clause is masked by `!SUD&&`, so SUL only shows through when
        // SUD=0. Each bit below is therefore probed against the ADJACENT
        // table row that differs by exactly that one bit AND holds the
        // other two at the value the formula needs: octant1(0,0,1)->
        // octant4(1,0,1) isolates SUD; octant6(0,0,0)->octant5(0,1,0)
        // isolates SUL; octant7(1,0,0)->octant4(1,0,1) isolates AUL. This is
        // the same "this bit alone, all else held fixed" method as every
        // other assertion in this file, with "all else" fixed at the value
        // the RTL formula actually needs to make the bit observable, rather
        // than an arbitrary all-zero baseline that happens to make two of
        // the three bits provably invisible. Observed via the D-channel
        // write-address SEQUENCE over 4 pixels (agnus_blitter.v:491's `we`),
        // the same fixed observable LINE-MODE-START now uses, for the same
        // reason (a genuine per-pixel stepping difference need not appear at
        // pixel 1's own address, since that grant is captured BEFORE the
        // pixel's own step registers - it appears from pixel 2 onward).
        auto d_addr_seq = [](uint16_t con1_bits) {
            Harness h;
            for (int i = 0; i < 8; ++i) h.c_words.push_back(0x0000);
            h.write_reg(BLTAPTH, 0);
            h.write_reg(BLTAPTL, 0);
            h.write_reg(BLTDPTH, 0);
            h.write_reg(BLTDPTL, 0x1000);
            write_bltcon0(h, 14, USEA | USEC | USED, 0x00);
            h.write_reg(BLTCON1, static_cast<uint16_t>(LINE_BIT | con1_bits));
            h.write_reg(BLTSIZE, bltsize_word(2, 4));
            h.run_cycles(120);
            h.settle();
            std::vector<uint32_t> addrs;
            for (auto& g : h.grants) if (g.channel == 'D') addrs.push_back(g.address);
            return addrs;
        };
        const auto oct1 = d_addr_seq(static_cast<uint16_t>(1u << 2));                      // AUL=1: octant 1 (0,0,1)
        const auto oct4 = d_addr_seq(static_cast<uint16_t>((1u << 4) | (1u << 2)));        // SUD=1,AUL=1: octant 4 (1,0,1)
        const auto oct6 = d_addr_seq(0);                                                    // octant 6 (0,0,0)
        const auto oct5 = d_addr_seq(static_cast<uint16_t>(1u << 3));                      // SUL=1: octant 5 (0,1,0)
        const auto oct7 = d_addr_seq(static_cast<uint16_t>(1u << 4));                      // SUD=1: octant 7 (1,0,0)

        const bool complete = oct1.size() == 4 && oct4.size() == 4 && oct6.size() == 4 &&
                               oct5.size() == 4 && oct7.size() == 4;
        const bool sud_live = oct1 != oct4;  // SUD toggled, AUL held at 1
        const bool sul_live = oct6 != oct5;  // SUL toggled, SUD held at 0
        const bool aul_live = oct7 != oct4;  // AUL toggled, SUD held at 1

        // None of the eight octants' (SUD,SUL,AUL) table cells is
        // exercised as a table cell by this file: `all_distinct`
        // checks only this test's OWN hardcoded array against the PDF scan
        // (a transcription self-check, not a DUT measurement, and BLTCON1
        // is write-only per p.267 "W" so nothing can "read back" a wrong
        // transcription either). The probes for octants 1, 4, 5, 6 and 7
        // drive BLTCON1 bits 4/3/2 as sensitizing vectors chosen by
        // hand-expanding agnus_blitter.v:609-612's formula, not by page
        // text - they ground "this bit is read and live", never "octant N
        // has this triple" or "octant N steps in its documented direction"
        // (the latter is PKG-BLT-LINE's job either way, per this file's own
        // Non-goals). So the graded assertion below keeps only the three
        // BIT-IDENTITY id groups (SUD=bit4, SUL=bit3, AUL=bit2, and their
        // legend definitions); all 24 octant-table-cell ids move to the
        // RECORDED line.
        checks.expect(all_distinct && complete && sud_live && sul_live && aul_live,
            "LINE-MODE-SUD-SUL-AUL-BITS-4-3-2-ARE-LIVE: BLTCON1 bit 4 (SUD), "
            "bit 3 (SUL) and bit 2 (AUL) each independently changes the "
            "D-channel write-address sequence of a 4-pixel line when probed "
            "with a sensitizing BLTCON1 value chosen from the octant table "
            "(appendix A p.268) - table rows '04 LF4 SUD','03 LF3 SUL','02 "
            "LF2 AUL', 'SUD Sometimes up or down', 'SUL Sometimes up or "
            "left', 'AUL Always up or left' (p.268 legend). Which octant "
            "NUMBER has which triple, or steps in which real-world "
            "direction, is not asserted here (PKG-BLT-LINE's job, per this "
            "file's Non-goals); the probe values are RTL-derived "
            "sensitizing vectors (agnus_blitter.v:609-612's lineinc/"
            "linedec/lineadd/linesub formulas), not a page-derived (dx,dy) "
            "setup. "
            "6-appa-control.c68, 6-appa-control.c70, 6-appa-control.c72, "
            "ch6-appa-bltcon0.line.b04.con1, ch6-appa-bltcon0.line.b03.con1, "
            "ch6-appa-bltcon0.line.b02.con1, "
            "ch6-appa-lf.legend.SUD, ch6-appa-lf.legend.SUL, ch6-appa-lf.legend.AUL .");

        checks.record(
            "LINE-MODE-OCTANT-TABLE-CELLS-NOT-GRADED-HERE: none of the "
            "eight (SUD,SUL,AUL) table cells (appendix A p.268, Table 6-3 "
            "p.190) is graded as a table cell by this package - octants 0, "
            "2 and 3 are never used as stimulus at all, and octants 1, 4, "
            "5, 6 and 7 are driven only as RTL-chosen sensitizing vectors "
            "for the bit-identity assertion above, which does not establish "
            "any specific octant's triple. BLTCON1 is write-only (p.267 "
            "'W'), so there is no port at which a wrong transcription of "
            "any row would read back differently either. As far as this "
            "suite's own cross-references show, these 24 ids are not "
            "graded elsewhere in the suite: ch6-blitter-line cites only "
            "(graded: ch6-appa-oct.select), not the individual per-octant "
            "sud/sul/aul ids. Left to PKG-BLT-LINE, which would draw a real "
            "line per octant against Figure 6-8 (p.190). "
            "ch6-appa-oct0.aul, ch6-appa-oct0.sud, ch6-appa-oct0.sul, "
            "ch6-appa-oct1.aul, ch6-appa-oct1.sud, ch6-appa-oct1.sul, "
            "ch6-appa-oct2.aul, ch6-appa-oct2.sud, ch6-appa-oct2.sul, "
            "ch6-appa-oct3.aul, ch6-appa-oct3.sud, ch6-appa-oct3.sul, "
            "ch6-appa-oct4.aul, ch6-appa-oct4.sud, ch6-appa-oct4.sul, "
            "ch6-appa-oct5.aul, ch6-appa-oct5.sud, ch6-appa-oct5.sul, "
            "ch6-appa-oct6.aul, ch6-appa-oct6.sud, ch6-appa-oct6.sul, "
            "ch6-appa-oct7.aul, ch6-appa-oct7.sud, ch6-appa-oct7.sul .");
    }

    // ---------------- LINE-MODE-SING-BIT1-GATES-D-CHANNEL-WRITES-TO-FIRST-PIXEL ----------------
    {
        // Counting D-writes with BLTAPT/BLTAMOD/BLTBMOD left at 0 would
        // not be a real line by p.192's own arithmetic (dx=dy=0 is
        // degenerate), and a write COUNT alone is not "a single bit per
        // horizontal line" (p.268 legend), which is what the page actually
        // states.
        //
        // This run uses a genuinely page-derived flat line: p.191/192's own
        // worked setup for dx=2, dy=0 (a 3-pixel horizontal run, "we set
        // the blit height to the length of the line, which is dx+1"):
        //   BLTAPT  = 4*dy - 2*dx = -4          (negative -> SIGN=1, p.192)
        //   BLTAMOD = 4*(dy-dx)   = -8
        //   BLTBMOD = 4*dy        =  0
        //   octant: SUD=1,SUL=0,AUL=0 (table row for octant 7, p.268/
        //     Table 6-3). Figure 6-8 (p.190) draws octant 7 as the
        //     right-down shallow ray; octant 0 (SUD=1,SUL=1,AUL=0) is
        //     equally a dy=0 ray on the same figure. The choice between the
        //     two is inconsequential for this run: with SUD=1, SUL does not
        //     appear in any of lineinc/linedec/lineadd/linesub
        //     (agnus_blitter.v:609-612), so octants 0 and 7 drive identical
        //     stepping here. With SUD=1 and AUL=0, linedec and lineadd/
        //     linesub are structurally 0 (AUL=0 kills their non-sign_del
        //     halves, SUD=1 kills the sign_del-gated halves once sign_del
        //     settles to 1, which it does from this run's first pixel
        //     onward since SIGN=1 is written before the blit starts). lineinc
        //     is NOT structurally 0: its SUD&&!AUL term is unconditional on
        //     ash, so it fires whenever ash==4'b1111 regardless of octant or
        //     sign_del. START=5 keeps ash at 5, 6 and 7 across these 3
        //     pixels - never 1111 - which is why they land in the same
        //     destination word; a different START can cross a word boundary
        //     on the identical octant and formula (LINE-MODE-START above,
        //     same SUD=1/AUL=0, START=14, does exactly that).
        //   LF = $CA, the ordinary line function AB+AC (p.191/192): with
        //     BLTBDAT=$FFFF (solid line, p.192) and C held at $0000, this
        //     reduces at every bit position to D=A - i.e. the current
        //     line-bit alone, so each pixel's OWN write can be examined
        //     without needing a real memory model to fold successive
        //     writes into one word (this package has none): the pixels of
        //     a flat line all target the SAME word in real hardware, so the
        //     UNION of what each pixel's write would have contributed is
        //     exactly what "the row" would end up containing.
        //   BLTADAT=$8000, START=5: the A-bit walks positions 15-START=10,
        //     9, 8 across the 3 pixels (agnus_blitter_barrelshifter's own
        //     right-shift-by-`ash` behaviour, already established by
        //     LINE-MODE-START above) - three DISTINCT bits, so the union's
        //     popcount is exactly the pixel count when every pixel writes,
        //     and less when SING suppresses some.
        // SING's own gate (agnus_blitter.v:860, BLT_L4, chsel==CHD) fires
        // dma_req = usec & (~SING | ~SUD | first_pixel): with SUD=1, SING=1
        // suppresses every D-write but the first, so only pixel 1's bit
        // (position 10) ever reaches the row - one set bit. With SING=0,
        // all 3 pixels write - three set bits, one per pixel.
        auto row_bits_set = [](bool sing) {
            Harness h;
            for (int i = 0; i < 3; ++i) h.c_words.push_back(0x0000);  // C held at 0 (this probe's own choice)
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0x8000);       // p.192: "BLTADAT = $8000"
            h.write_reg(BLTBDAT, 0xFFFF);       // p.192: "$FFFF for a solid line"
            h.write_reg(BLTAPTH, 0xFFFF);
            h.write_reg(BLTAPTL, 0xFFFC);       // BLTAPT = 4*dy-2*dx = -4 (dx=2,dy=0)
            h.write_reg(BLTAMOD, 0xFFF8);        // 4*(dy-dx) = -8
            h.write_reg(BLTBMOD, 0x0000);        // 4*dy = 0
            h.write_reg(BLTCMOD, 40);
            h.write_reg(BLTDMOD, 40);
            h.write_reg(BLTCPTH, 0);
            h.write_reg(BLTCPTL, 0x1000);
            h.write_reg(BLTDPTH, 0);
            h.write_reg(BLTDPTL, 0x1000);
            write_bltcon0(h, 5 /* START = x1 modulo 15 */, USEA | USEC | USED, 0xCA);  // LF: ordinary line function AB+AC
            const uint16_t SIGN = 1u << 6, SUD = 1u << 4, SING = 1u << 1;
            uint16_t con1 = static_cast<uint16_t>(LINE_BIT | SIGN | SUD);  // octant 7 (1,0,0); SIGN=1 per p.192's rule
            if (sing) con1 |= SING;
            h.write_reg(BLTCON1, con1);
            h.write_reg(BLTSIZE, bltsize_word(2, 3));  // width=2 fixed, height=dx+1=3 (p.191/192)
            h.run_cycles(90);
            h.settle();
            uint16_t row = 0;
            for (auto v : h.data_writes) row |= v;  // the row's content: the union every pixel would fold into the same word
            int bits = 0;
            for (int i = 0; i < 16; ++i) if (row & (1u << i)) ++bits;
            return bits;
        };
        const int without_sing = row_bits_set(false);
        const int with_sing = row_bits_set(true);

        checks.expect(with_sing == 1 && without_sing == 3,
            "LINE-MODE-SING-BIT1-GATES-D-CHANNEL-WRITES-TO-FIRST-PIXEL: a flat "
            "3-pixel line (p.191/192's own dx=2,dy=0 setup: BLTAPT=-4, "
            "BLTAMOD=-8, BLTBMOD=0, SIGN=1, octant 7) draws 1 set bit in the "
            "row with SING (BLTCON1 bit 1) set, and 3 with it clear - 'SING "
            "Single bit per horizontal line for use with subsequent area "
            "fill' (p.268 legend), table row '01 LF1 SING' (p.268). "
            "agnus_blitter.v:860 `dma_req = usec & (~bltcon1[1] | "
            "~bltcon1[4] | first_pixel);` inside BLT_L4 (chsel==CHD, the "
            "D-channel store state - agnus_blitter.v:491 `we = ackdma && "
            "chsel==CHD`). This run's SING=0 branch also carries the "
            "line-mode table's required-zero ids for BLTCON1 bits 11, 10, "
            "9, 8, 7 and 5 (table row values '0','0','0','0','0','0 "
            "(Reserved)', appendix A p.268): all six are clear in both "
            "branches of this run, which draws a genuine p.192 flat line "
            "('BLTSIZE bits 15-6 = dx+1', p.192) - the documented pattern, "
            "not an ad-hoc one. "
            "6-appa-control.c74, 6-appa-control.c89, "
            "ch6-appa-bltcon0.line.b01.con1, ch6-appa-lf.legend.SING, "
            "6-appa-control.c54, 6-appa-control.c56, 6-appa-control.c58, "
            "6-appa-control.c60, 6-appa-control.c62, 6-appa-control.c66, "
            "6-appa-control.c88, "
            "ch6-appa-bltcon0.line.b11.con1, ch6-appa-bltcon0.line.b10.con1, "
            "ch6-appa-bltcon0.line.b09.con1, ch6-appa-bltcon0.line.b08.con1, "
            "ch6-appa-bltcon0.line.b07.con1, ch6-appa-bltcon0.line.b05.con1, "
            "ch6-appa-lf.legend.reserved .");
    }

    // ---------------- LINE-MODE-LF-BITS-EACH-ALONE-MATCH-P174-MAPPING ----------------
    {
        // A one-pixel run using B=$FFFF and C=$FFFF for both of p.192's
        // worked byte values makes A's combination (1,1,1)=position 7 and
        // every OTHER bit's combination (0,1,1)=position 3 (p.174: position
        // = 4A+2B+C) - no other LF bit position is ever read by such a run,
        // so it cannot ground all 17 line-mode bit-identity ids by itself
        // (masking LF to keep only bits 7 and 3 leaves the two worked-value
        // checks below green, confirming no other bit is read).
        //
        // This run instead varies B and C so every LF bit gets its own,
        // distinct (A,B,C) combination at some bit of the word: for each
        // LF bit k
        // alone, with BLTBDAT in {$0000,$FFFF} and C in {$5555,$AAAA} (4
        // runs per bit, 32 total), the expected output bit i is LF bit
        // (4*[i==15] + 2*B_i + C_i) - p.174's own mapping, since BLTADAT=
        // $8000 makes A's bit 1 only at position 15.
        auto one_pixel_lf = [](uint16_t lf, uint16_t b, uint16_t c) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(c);
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0x8000);
            h.write_reg(BLTBDAT, b);
            write_bltcon0(h, 0 /* START=0 */, USEA | USEC | USED, lf);
            h.write_reg(BLTCON1, LINE_BIT);
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_cycles(60);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        auto page_expect = [](uint16_t lf, uint16_t b, uint16_t c) {
            uint16_t d = 0;
            for (int i = 0; i < 16; ++i) {
                const int a_i = (i == 15) ? 1 : 0;
                const int b_i = (b >> i) & 1;
                const int c_i = (c >> i) & 1;
                const int pos = 4 * a_i + 2 * b_i + c_i;
                if ((lf >> pos) & 1) d = static_cast<uint16_t>(d | (1u << i));
            }
            return d;
        };
        const uint16_t bs[] = {0x0000, 0xFFFF};
        const uint16_t cs[] = {0x5555, 0xAAAA};
        bool all_match = true;
        for (int k = 0; k < 8 && all_match; ++k)
            for (uint16_t b : bs)
                for (uint16_t c : cs) {
                    const uint16_t lf = static_cast<uint16_t>(1u << k);
                    if (one_pixel_lf(lf, b, c) != page_expect(lf, b, c)) all_match = false;
                }

        checks.expect(all_match,
            "LINE-MODE-LF-BITS-EACH-ALONE-MATCH-P174-MAPPING: for each of "
            "the eight LF bits set alone, with BLTBDAT in {$0000,$FFFF} and "
            "C in {$5555,$AAAA} (32 runs), the one-pixel line-mode output "
            "matches p.174's own position = 4A+2B+C mapping bit-for-bit - "
            "'There are eight possible combinations of values of the three "
            "bits, for each of which we need to specify the corresponding "
            "destination bit as a zero or one' (p.174), matching table rows "
            "'07 LF7' through '00 LF0' in the LINE-MODE table (p.268). "
            "agnus_blitter.v:444 `.lf(bltcon0[7:0])` (unconditional on "
            "`line`), agnus_blitter_minterm.v:29-40. "
            "6-appa-control.c61, 6-appa-control.c63, 6-appa-control.c65, "
            "6-appa-control.c67, 6-appa-control.c69, 6-appa-control.c71, "
            "6-appa-control.c73, 6-appa-control.c75, 6-appa-control.c79, "
            "ch6-appa-bltcon0.line.b07.con0, ch6-appa-bltcon0.line.b06.con0, "
            "ch6-appa-bltcon0.line.b05.con0, ch6-appa-bltcon0.line.b04.con0, "
            "ch6-appa-bltcon0.line.b03.con0, ch6-appa-bltcon0.line.b02.con0, "
            "ch6-appa-bltcon0.line.b01.con0, ch6-appa-bltcon0.line.b00.con0 .");
    }

    // ---------------- LINE-MODE-LF-4A-AND-CA-SELECT-DOCUMENTED-FUNCTIONS ----------------
    {
        // The manual's own two worked LF byte values, run in an ACTUAL
        // line-mode blit (LINE_BIT=1) per p.192's own setup: BLTADAT=$8000,
        // BLTBDAT=$FFFF (solid line), C=$FFFF, START=0.
        auto line_lf_output = [](uint16_t lf) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0xFFFF);  // p.192's own worked C = $FFFF
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0x8000);   // p.192: "BLTADAT = $8000"
            h.write_reg(BLTBDAT, 0xFFFF);   // p.192: "$FFFF for a solid line"
            write_bltcon0(h, 0 /* START=0 */, USEA | USEC | USED, lf);
            h.write_reg(BLTCON1, LINE_BIT);
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_cycles(60);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        const uint16_t xor_form = line_lf_output(0x4A);
        const uint16_t ordinary_form = line_lf_output(0xCA);

        checks.expect(xor_form == 0x7FFF && ordinary_form == 0xFFFF,
            "LINE-MODE-LF-4A-AND-CA-SELECT-DOCUMENTED-FUNCTIONS: a "
            "one-pixel line-mode blit (BLTADAT=$8000, BLTBDAT=$FFFF solid, "
            "C=$FFFF, START=0) with LF=$4A (exclusive-or line mode) stores "
            "$7FFF, and the SAME setup with LF=$CA (ordinary line mode) "
            "stores $FFFF - 'LF7-0 [...] should be preloaded with 4A to "
            "select the equation D=(AC+ABC)' (appendix A p.268), 'then "
            "BLTCON0 LF control byte = ABC + AC' (exclusive-or) / 'else "
            "BLTCON0 LF control byte = AB + AC' (ordinary) [overbars over "
            "the C of the first term and the A of the second] (p.192). "
            "agnus_blitter.v:444 `.lf(bltcon0[7:0])` (unconditional on "
            "`line`), agnus_blitter_minterm.v:29-40. "
            "6-line-mode.c90, 6-line-mode.c91 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
