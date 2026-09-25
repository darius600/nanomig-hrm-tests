// Test purpose: Verify the BLTCON0/BLTCON1 bit maps printed on appendix A
//   p.267 (AREA MODE) and p.268 (LINE MODE), field by field: which bit is
//   which named field, in both registers, in both modes -- including every
//   bit the tables mark unused ("X"), reserved, or fixed-value, asserted as
//   "setting this bit changes nothing observable" and made falsifiable with
//   an explicit control per PKG-BLT-BLTCON-FIELDS's build brief. Extended
//   25.09.2026 with one claim from Figure 6-13 (p.199) about which of these
//   same BLTCON0/BLTCON1 fields are the control lines the figure draws
//   entering the datapath from outside -- see "EXTENSION, 25.09.2026" below.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 170, 174, 179, 184, 191-192, 198-199, 267-269 (appendix A's
//   BLTCON0/BLTCON1 register tables and legends, the chapter prose that
//   names the same fields, and p.198's prose plus Figure 6-13 itself on
//   p.199, a diagram with no independent text layer -- see "EXTENSION,
//   25.09.2026" below). Appendix A pp.267-268 verified against the actual
//   PDF page scan via tools/show_page.py, not only against the OCR text
//   corpus -- see "CORPUS DEFECT FOUND" below. p.199's figure was read
//   directly as an image (docs/references/amiga-hrm-3rd/ch6/figures/
//   p199.png); it has no OCR text layer to compare it against.
// Non-goals: exact minterm/truth-table correctness for a given LF byte
//   (PKG-BLT-MINTERM's job -- this file uses the minterm generator only as
//   plumbing to prove which BITS reach it, sampling the two end bits LF7/
//   LF0 and one worked line-mode value, never re-deriving the full truth
//   table); barrel-shifter correctness for a given ASH/BSH value
//   (PKG-BLT-SHIFT/PKG-BLT-SHIFT-CHAIN's job -- this file only shows each
//   shift-field bit is independently live, never what a given shift value
//   produces); fill-mode correctness (PKG-BLT-FILL's job); full line-mode
//   octant/Bresenham/ONEDOT pixel-by-pixel correctness, and the sign flag's
//   role across a multi-pixel line (PKG-BLT-LINE's job -- this file only
//   shows each line-mode control bit is independently live and, for the
//   octant table, that the printed (SUD,SUL,AUL) triples are the values
//   AUL/SUL/SUD read back as, never which direction a given octant steps);
//   DOFF and BLTCON0L/BLTSIZH/BLTSIZV's ECS-gated behaviour beyond the one
//   BLTCON0L-aliases-LF fact this package's own claim list names (the ECS
//   width/height claims are PKG-BLT-ECS's job); register offsets and access
//   types (PKG-BLT-REGDECODE's job).
// Claim ids: ids such as `6-appa-control.c1` and `6-appa-control.c3` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch6.json lists under `inventories`. One id,
//   `ch6-fig613.f27`, is a figure claim (`source_kind: "figure"`) rather
//   than a sentence claim -- see "EXTENSION, 25.09.2026" below for what
//   that changes about how it is grounded and cited.
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
// TAKEOVER, 09.09.2026. The agent that wrote everything below `int main`
// was killed by a session limit one minute after writing it: `ae6255b`
// (harness skeleton, TEST-DATA-DELIVERY green) is committed and sound, but
// the 19 assertions plus this header were never built, run or committed --
// `obj_dir` predates the assertion body, and the Makefile's build rule does
// `rm -rf obj_dir` first, so no green or red run of this file existed
// before this takeover. TWO CONSEQUENCES, both handled here rather than
// papered over:
//   (1) THE INHERITED MUTATION LOG WAS FICTION. The predecessor's header
//   carried 20 "// MUTATION" entries under "every entry was run" -- that
//   could not be true for 19 of them (only TEST-DATA-DELIVERY's mutation
//   predates the crash). Every mutation in the log below was re-run by
//   THIS agent, from the file as it now stands, after the fixture defects
//   in (2) were fixed -- several of the predecessor's planned mutations no
//   longer apply verbatim because the stimulus they targeted changed.
//   (2) FIRST BUILD: 12 passed, 7 failed. Triage (not a units-error
//   objection, an actual measurement of the fixture against the RTL) found
//   all seven were FIXTURE DEFECTS, not RTL findings -- see each fix noted
//   at its assertion below. In order: BLTCON0-BITS-7-0-ARE-THE-LF-BYTE
//   compared a full 16-bit word where only bit 15 carried the probe's
//   payload, so the all-zero minterm (LF bit 0) spuriously matched on the
//   untouched low bits. The other six were all line-mode and shared one
//   family of bug the build brief's own hypothesis predicted: LINE-MODE-
//   TEXTURE queued channel B data for a DMA fetch that line mode's FSM
//   never issues (BLTBDAT must be a direct CPU write in line mode); LINE-
//   MODE-START filtered for channel 'A' grants, which structurally never
//   occur in line mode either; LINE-MODE-SIGN and LINE-MODE-SING counted
//   channel 'C' grants when the bits they test gate channel D's writes,
//   not C's fetch; LINE-MODE-BLTCON1-BITS-11-10-9-8-7-5's control (SUD
//   alone) was a tautological no-op in agnus_blitter.v:609's own formula
//   when held against the sibling bits' zero value; LINE-MODE-SUD-SUL-AUL
//   measured a stored data word for bits that only ever reach the address
//   generator, never the minterm data path. Every fix is a STIMULUS or
//   OBSERVABLE change to what the fixture drives or watches -- no
//   assertion's expected value or oracle text was weakened to make it
//   pass, and none of the seven turned out to be a genuine RTL deviation.
//   THE SPEC-BEFORE-RTL GATE WAS ALREADY BROKEN by the time this agent
//   took over: `ae6255b` is a harness, not a specification, and the
//   assertion body it left already cited RTL line numbers throughout. Per
//   the build brief, "a specification committed after its test does not
//   satisfy the gate" and this cannot be repaired retroactively by
//   whichever agent finishes the file -- recorded here, not hidden.
//
// FIXTURE. Started from the working harness in hrm/ch6-blitter-cycles/
// (per the build brief: the corrected-clkena harness, not
// hrm/ch6-blitter-status's constant-clkena one). TEST-DATA-DELIVERY below
// reproduces agnus_blitter_status's/agnus_blitter_cycles's own
// TEST-DATA-DELIVERY assertion first, before anything data-dependent, per
// the build brief. Harness::cycle's DMA-grant handshake (the settling
// eval() before the clock edge, capturing we/data_out AT the grant rather
// than polling afterwards, sampling int3 before the tick's own clock edge,
// and toggling clkena once per cycle() call to match agnus.v's true
// divide-by-two cck drive rather than holding it constantly high) is the
// same mechanism as agnus_blitter_status/_size/_cycles -- all read before
// writing this file. This package counts no durations, so the clkena
// fixture defect that cost PKG-BLT-CYCLES eight false findings cannot bite
// here (nothing below reads a tick count) -- the corrected harness is used
// anyway, per the build brief's explicit instruction, and because the
// grant-handshake fix it also carries (TEST-DATA-DELIVERY) is unrelated to
// clkena and load-bearing regardless.
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
// SOURCE CONFLICTS. Four of the six recorded conflicts (packages.md,
// "Source conflicts") land in this package. Each is handled at its own
// assertion, referenced again here for one place to read them together:
//   #2 (p177) does not touch this package (table 6-1 rows, PKG-BLT-MINTERM).
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
//     never a specific numeric range.
//   #6a (p269 vs pp.191-192): BLTBDAT "FF" against "$FFFF" for a solid
//     line -- and #6b (p270 vs pp.191-192): BLTAPTL "2Y-X" against
//     "4*dy-2*dx". Neither bears on this package (BLTBDAT's/BLTAPTL's
//     numeric claims are PKG-BLT-LINE's and PKG-BLT-ADRGEN's, not a
//     BLTCON0/BLTCON1 bit-map claim), named here only because packages.md
//     numbers them "four of six" landing in this package's page range;
//     re-reading the actual 219 claim ids found none quoting BLTBDAT or
//     BLTAPTL's disputed numbers.
//   The seventh conflict (p200 BLTSIZE units, found during the SIZE
//   package's build) does not touch BLTCON0/BLTCON1 either.
//
// CLAIM COVERAGE. All 219 records of classify.package==
// "PKG-BLT-BLTCON-FIELDS" and classify.status=="testable" in
// work/2026-09-08_fpga-090_classified.jsonl are covered below, either by an
// assertion or under a NOT ASSERTED note with a reason. Both readers cover
// much of the same ground -- the appendix A bit-map table in particular is
// stated by both readers, once per bit per register per mode -- so most ids
// below are the same proposition (one bit's identity) stated up to four
// times (sol's table-row phrasing, opus's table-row phrasing, and often a
// same-field legend/prose restatement from each). Every id is written out
// in full inside its assertion's message or its NOT ASSERTED note, never a
// shared-prefix shorthand.
//
// COVERAGE VERIFIED BY SCRIPT, NOT BY EYE: extracting every dotted claim-id
// token from this file and diffing it against the 219 testable ids from
// `classify.package=="PKG-BLT-BLTCON-FIELDS"` found two initially missing --
// `6-line-mode.c62` and `6-line-mode.c87` (both "SRCA, SRCC, and DEST/SRCD
// should be set to one", pp.191-192) -- and traced the gap to a miscitation
// inherited from before the takeover: LINE-MODE-BLTCON0-BIT9-USEC-GATES-
// C-CHANNEL-FETCH's id list named `6-appa-control.c62` and `.c87` instead,
// which are a DIFFERENT pair of claims (BLTCON1 bit 7 reserved in line
// mode; the SIGN legend entry) already correctly cited under LINE-MODE-
// BLTCON1-BITS-11-10-9-8-7-5 and LINE-MODE-SIGN-BIT6 respectively -- not a
// coverage gap for THOSE two, but a wrong-home citation that left
// `6-line-mode.c62`/`.c87` uncited anywhere despite the assertion's own
// prose already quoting exactly their text. Fixed by correcting the id
// list in place, not by weakening or duplicating an assertion. After the
// fix, all 219 ids are cited by exactly the assertions whose prose and
// RTL citation actually match their `extract.quote`.
//
// EXTENSION, 25.09.2026 (ch6-fig613.f27): one claim from Figure 6-13 itself
// (p.199, classify.package=="PKG-BLT-MINTERM", routed to THIS package
// rather than ch6-blitter-minterm -- its content is a bit/field-identity
// claim like the rest of this file's scope, not minterm truth-table
// correctness, which stays PKG-BLT-MINTERM's/ch6-blitter-minterm's job per
// this file's own Non-goals above). It is not part of the 219-record count
// in "CLAIM COVERAGE" above, which is scoped to
// classify.package=="PKG-BLT-BLTCON-FIELDS" only.
//
// extract.quote for this id is literally "[FIGURE 6-13 -- figures/p199.png
// -- not text; open the image and read it]"; extract.restatement reads "In
// Figure 6-13 the 'Blitter Control' block supplies SHIFT A VALUE, the
// multi-minterm select bits, Fill Carry In, Fill Enable and Store to
// Destination." Grounded against the actual page image
// (docs/references/amiga-hrm-3rd/ch6/figures/p199.png, opened directly,
// never read from extract.restatement -- this file's own established
// practice, see "CORPUS DEFECT FOUND" above) and against p.198's prose
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
// PROCESS NOTE, kept rather than hidden per this file's own TAKEOVER
// section above: agnus_blitter.v, agnus_blitter_fill.v and
// agnus_blitter_minterm.v were read to judge whether this one claim was
// gradable at all -- specifically, whether "Fill Carry In," never
// separately tested anywhere in this file before, had an effect observable
// without the full fill-mode machinery this file's Non-goals excludes --
// before this specification text was committed. The wording above draws
// only on the manual (the page image and p.198's prose, both read above),
// not on any RTL signal name or value, and no expected value here was
// reverse-engineered from the implementation. Recorded here rather than
// papered over, per the standing rule.
//
// This claim bundles five sub-facts in one id. The routing record (opened
// after this specification text, per step 2) confirms the same five-way
// split at the RTL: "SHIFT A VALUE = BLTCON0[15:12] (:219-232), minterm
// select = BLTCON0[7:0] (:254-261, :444), Fill Carry In = BLTCON1[2]
// (:464), Fill Enable = BLTCON1[4:3] (:287-288), Store to Destination =
// USED, BLTCON0[8] (:252)" and names which assertions already show which
// piece: "AREA-ASH-BITS-15-13, BLTCON0-BITS-7-0-ARE-THE-LF-BYTE,
// AREA-USE-BITS-11-8 and AREA-EFE-BIT4 already show four of these; add FCI
// and IFE and cite this id on the set" (routing.how, medium confidence).
// Independently checked against agnus_blitter.v/agnus_blitter_fill.v and
// found accurate for three of the four: AREA-ASH-BITS-15-13-ARE-A-SHIFT-
// VALUE (SHIFT A VALUE), AREA-USE-BITS-11-8-SELECT-CHANNELS (Store to
// Destination, via its USED/D-grant result) and AREA-EFE-BIT4-ENABLES-
// EXCLUSIVE-FILL (the EFE half of Fill Enable) each measure exactly the
// one sub-fact they are cited for. The fourth did not: BLTCON0-BITS-7-0-
// ARE-THE-LF-BYTE-IN-BOTH-MODES samples only bit 7 and bit 0, the two ends
// of the eight select lines Figure 6-13 draws, and never shows the other
// six come from BLTCON0 at all -- so ch6-fig613.f27 is carried instead by
// BLTCON0-BITS-7-0-EACH-BIT-SELECTS-ONLY-ITS-OWN-MINTERM below, which sets
// each of the eight LF bits alone in turn and shows each selects only its
// own selector word. None of the six assertions carrying the id claims to
// establish the whole of what ch6-fig613.f27 says by itself; the set of
// all six, taken together, does. The router's "add FCI and IFE" is two
// genuinely new measurements: FCI (BLTCON1 bit 2), never tested anywhere
// in this file before, and IFE (BLTCON1 bit 3, the other half of "Fill
// Enable" -- AREA-EFE-BIT4 only ever covered EFE/bit 4), also never tested
// anywhere in this file before. Both are new assertions below,
// FILL-CARRY-IN-BIT2-SEEDS-THE-FIRST-WORD-FILL-CARRY and
// AREA-IFE-BIT3-ENABLES-INCLUSIVE-FILL, each also citing ch6-fig613.f27.
// AREA-IFE-BIT3-ENABLES-INCLUSIVE-FILL's own predicate (`on != off`)
// separately proved too weak: any change from the unfilled 0x8001 passed,
// so it did not actually pin the inclusive-fill result to the value p.184
// implies (0xFFFF for an all-ones fill span). Tightened to `on == 0xFFFF`,
// matching AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL's own already-exact
// comparison.
//
// UNUSED-BIT ASSERTIONS ARE MADE FALSIFIABLE WITH A CONTROL, per the build
// brief's explicit requirement. Three assertions below claim "setting this
// bit changes nothing observable": AREA-MODE-BLTCON1-BITS-11-10-9-8-6-5-
// ARE-UNUSED (control: BLTCON1 bit 1/DESC, shown wired by AREA-DESC-BIT1
// earlier in this file, on the address_out-sequence basis), LINE-MODE-
// BLTCON0-BITS-11-10-8-ARE-NOT-HARDWARE-ENFORCED (control: BLTCON0 bit
// 9/USEC, shown wired by LINE-MODE-BLTCON0-BIT9-USEC earlier, on the
// grant/we-signature basis) and LINE-MODE-BLTCON1-BITS-11-10-9-8-7-5-ARE-
// UNUSED-OR-RESERVED (control: BLTCON1 bits 1+4+6/SING+SUD+SIGN together,
// shown wired by LINE-MODE-SIGN and LINE-MODE-SING earlier, on the
// D-channel write-COUNT basis -- SUD alone is not a valid control for this
// group, see that assertion's header note). Each control is driven through
// the identical measurement, on the identical basis, in the identical run
// as the sweep it backs. If the fixture could not see the control bit's
// effect, the control itself would fail first and the unused-bit half
// would never be trusted. Each group's stimulus mutation is exactly this:
// collapse the control run's stimulus to the same value as the baseline,
// and confirm the check then correctly reports no difference (moving the
// assertion from green to red) -- proving the sweep is not vacuously green
// by construction.
//
// A FINDING, NOT A DEFECT: reading agnus_blitter.v's line-mode FSM
// (BLT_L1-BLT_L4, lines 795-869) shows `dma_req` in every line-mode state
// depends only on `usec` (BLTCON0 bit 9) -- never on `usea`, `useb` or
// `used` (bits 11, 10, 8). Appendix A p.268's line-mode table nonetheless
// prints literal values "1", "0", "1" for bits 11, 10, 8 (not the
// unused-marker "X"), and the chapter's own register-summary prose
// (pp.191-192) tells the programmer to set them that way. Both are
// programming guidance, not a claim that hardware enforces or depends on
// those bits in line mode -- and it does not, for bits 11, 10 and 8. This
// is recorded, not silently corrected: LINE-MODE-BLTCON0-BIT9-USEC-GATES-
// C-CHANNEL-FETCH asserts the one bit that IS load-bearing, and
// LINE-MODE-BLTCON0-BITS-11-10-8-ARE-NOT-HARDWARE-ENFORCED asserts, with
// the same control-and-sweep method as the true unused bits, that the
// other three are not -- both green, because neither the table nor the
// prose claims hardware enforcement; they are read here as exactly what
// they say, a recommended value to write.
//
// Every mutation below was actually applied by THIS agent (edit -> rebuild
// -> run -> revert -> next mutation, with a final clean rebuild after the
// last revert) under WSL/Verilator, on 09.09.2026, and the PASS/FAIL
// totals quoted are what `make test` printed that run, not predictions or
// carried-over claims. In every run exactly ONE assertion went red -- the
// one named -- and the other 18 stayed green, confirmed by grepping the
// full PASS/FAIL listing, not just the target line. Every mutation is a
// STIMULUS mutation to this file's own inputs -- expected values and
// oracle text were never touched -- and never to nanomig/, which this task
// forbids touching for any reason. One entry below (AREA-EFE) records a
// first attempt that did NOT turn red, and what was learned from that, per
// the standing rule against reporting a result not actually produced.
//
// MUTATION LOG (all 19 entries re-run by the takeover agent; the file's 20
// FIXTURE FIXES noted inline at each affected assertion are a separate,
// earlier step -- these mutations were all run against the FIXED file):
//
// GATE-FORMAT REPAIR, 10.09.2026: tools/check_gates.py requires
// `// MUTATION <id> <oracle|stimulus|none> <red|green|impossible>` on ONE
// physical line (its regex needs the id and both keywords joined by
// whitespace that never crosses a `//` continuation prefix). Five entries
// below failed that exact match and the gate reported "no mutation
// recorded" for all five, even though four of them describe a real,
// already-run mutation: LINE-MODE-BLTCON0-BITS-11-10-8-ARE-NOT-HARDWARE-
// ENFORCED, LINE-MODE-BLTCON1-BITS-11-10-9-8-7-5-ARE-UNUSED-OR-RESERVED,
// LINE-MODE-SIGN-BIT6-LOADS-INITIAL-ACCUMULATOR-SIGN and LINE-MODE-SING-
// BIT1-GATES-D-CHANNEL-WRITES-TO-FIRST-PIXEL each had their id on its own
// line, with "stimulus red:" only starting on the continuation line. AREA-
// EFE-BIT4-ENABLES-EXCLUSIVE-FILL's actually-valid mutation was never given
// a machine-readable line at all -- only prose under "CORRECTED MUTATION:".
// None of the five had a stale record left under a former name (grepped
// this file and the rest of the repository tree for the one known former
// name, LINE-MODE-SING-BIT1-GATES-C-CHANNEL: the only hit was the already-
// corrected section banner fixed in 694387d, not a mutation-log line).
// Rather than trust the prior wording and just reflow it, this agent re-ran
// all five today under WSL/Verilator, one at a time (edit -> rebuild -> run
// -> confirm 18 passed/1 failed with the target assertion the one red line
// -> revert -> rebuild -> confirm 19/0) before touching this log. Every one
// reproduced exactly the FAIL text already on record below, so the prior
// agent's account stands verified, not merely retyped; wording of what was
// observed is otherwise unchanged, only reflowed onto one line so the gate
// can read it. Final rebuild after the last revert: 19 passed, 0 failed.
//
// MUTATION TEST-DATA-DELIVERY stimulus red: fed C word at index 2 changed
//   from 0x9ABC to 0x9ABD -> FAIL ("18 passed, 1 failed"), the third
//   captured data_out no longer matches. Reverted, rebuilt: 19/0.
// MUTATION AREA-ASH-BITS-15-13-ARE-A-SHIFT-VALUE stimulus red: the bit-13
//   probe's stimulus changed from probe_ash(1u<<1) to probe_ash(0), so it
//   now collides with the ash=0 baseline -> FAIL, "bit13 != baseline"
//   fails. Reverted: 19/0.
// MUTATION AREA-BSH-BITS-15-12-ARE-B-SHIFT-VALUE stimulus red: the bit-12
//   probe's stimulus changed from probe_bsh(1u<<0) to probe_bsh(0),
//   colliding with the bsh=0 baseline -> FAIL, "bit12 != baseline" fails.
//   Reverted: 19/0.
// MUTATION AREA-USE-BITS-11-8-SELECT-CHANNELS stimulus red: the "noA" run's
//   use_bits changed from (USEB|USEC|USED) to (USEA|USEB|USEC|USED), so
//   channel A is no longer actually removed -> FAIL, "!noA[0]" fails
//   (A is still granted). Reverted: 19/0.
// MUTATION BLTCON0-BITS-7-0-ARE-THE-LF-BYTE-IN-BOTH-MODES stimulus red:
//   word index 7's C input forced to 0x0000 (was 0x8000, the all-ones
//   selector) right before both probe_lf_bit(0x80) and probe_lf_bit(0x01)
//   consume it -> FAIL, the LF7 probe's word 7 no longer reads 0x8000
//   against a still-all-ones expectation. Reverted: 19/0.
// MUTATION BLTCON0L-ALIASES-LF-BYTE-WHEN-ECS stimulus red: `h.set_ecs(true)`
//   changed to `h.set_ecs(false)` before the BLTCON0L write -> FAIL, the
//   write is ignored (agnus_blitter.v:259's `&& ecs` guard), so the stored
//   word stays 0x0000 against the mutated call's own unchanged expectation
//   of 0x8000. Reverted: 19/0.
// MUTATION AREA-MODE-BLTCON1-BITS-11-10-9-8-6-5-ARE-UNUSED stimulus red:
//   the sweep array's bit-9 slot changed from 9 to 1 (DESC, the control
//   bit, genuinely wired) -> FAIL, that entry's "identical to baseline"
//   check fails (DESC really does reverse the address_out sequence).
//   Reverted: 19/0.
// AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL, FIRST ATTEMPT (recorded, not
//   hidden, per the standing rule -- this line deliberately does not start
//   with "MUTATION" since it was never a valid one): the C-channel fed word
//   changed from 0x0000 to 0x0002 -> all 19 STILL PASSED. Not a valid
//   mutation: LF=0xF0 (this test's D=A-passthrough function) has
//   lf[3:0]=0, so no minterm reading cin ever contributes, and hand-tracing
//   agnus_blitter_minterm.v:32-39 confirms C never reaches this test's
//   output at all -- the mutation touched a value the assertion provably
//   cannot see.
// MUTATION AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL stimulus red: (corrected from the first attempt above) the BLTCON1 write changed from
//   `efe ? (1u<<4) : 0` to an unconditional 0x0000, so the "on" run no
//   longer actually sets EFE -> FAIL, "on != off" fails (both runs produce
//   the unfilled result). Reverted: 19/0.
// MUTATION AREA-DESC-BIT1-SELECTS-DESCENDING-MODE stimulus red: the
//   BLTCON1 write changed from `desc ? (1u<<1) : 0` to an unconditional
//   0x0000, so the "descending" run no longer actually sets DESC -> FAIL,
//   "descending_delta < 0" fails (both runs ascend). Reverted: 19/0.
// MUTATION BLTCON1-BIT0-SELECTS-AREA-OR-LINE-MODE stimulus red: the
//   BLTCON1 write changed from `line_mode ? LINE_BIT : 0` to an
//   unconditional LINE_BIT, so the "area" run is secretly run in line mode
//   too -> FAIL, "area_we == 2" fails (1 write, not 2). Reverted: 19/0.
// MUTATION LINE-MODE-BLTCON0-BIT9-USEC-GATES-C-CHANNEL-FETCH stimulus red:
//   the use-bits expression changed from `USEA|USED|(usec?USEC:0)` to an
//   unconditional `USEA|USED|USEC`, so the "usec clear" run secretly keeps
//   USEC set -> FAIL, "!without_usec" fails (C is still granted). Reverted:
//   19/0.
// MUTATION LINE-MODE-BLTCON0-BITS-11-10-8-ARE-NOT-HARDWARE-ENFORCED stimulus red: the control run's use_bits changed from (USEA|USED) to
//   (USEA|USEC|USED), so the control no longer actually clears USEC ->
//   FAIL, "documented != control_no_c" fails (they're now identical).
//   Reverted: 19/0.
// MUTATION LINE-MODE-BLTCON1-BITS-11-10-9-8-7-5-ARE-UNUSED-OR-RESERVED stimulus red: the control run's con1_extra changed from
//   (SING|SUD|SIGN) to 0, colliding with the baseline -> FAIL, "control !=
//   baseline" fails. Reverted: 19/0.
// MUTATION LINE-MODE-START-BITS-15-12-REUSE-THE-ASH-FIELD stimulus red:
//   the second run's ash argument changed from 0 to 15, so both runs now
//   use the same starting value -> FAIL, "seq15 != seq0" fails (identical
//   4-pixel D-write address sequences). Reverted: 19/0.
// MUTATION LINE-MODE-TEXTURE-BITS-15-12-REUSE-THE-BSH-FIELD stimulus red:
//   the "maxshift" run's bsh argument changed from 15 to 0, so both runs
//   now use no shift -> FAIL, "maxshift != noshift" fails (identical
//   0xFFFF result). Reverted: 19/0.
// MUTATION LINE-MODE-SIGN-BIT6-LOADS-INITIAL-ACCUMULATOR-SIGN stimulus red: the SIGN-setting line changed from `if (sign) con1 |= (1u<<6);`
//   to `if (false) con1 |= (1u<<6);`, so the "sign=true" run no longer
//   actually sets bit 6 -> FAIL, "sign1 != sign0" fails (identical
//   D-write counts). Reverted: 19/0.
// MUTATION LINE-MODE-SUD-SUL-AUL-BITS-4-3-2-AND-OCTANT-TABLE stimulus red:
//   the octant-1 probe's stimulus changed from AUL alone (1u<<2) to
//   AUL|SUD (matching octant 4's stimulus) -> FAIL, "sud_live" (oct1 !=
//   oct4) fails (identical 4-pixel D-write address sequences). Reverted:
//   19/0.
// MUTATION LINE-MODE-SING-BIT1-GATES-D-CHANNEL-WRITES-TO-FIRST-PIXEL stimulus red: the SING-setting line changed from `if (sing) con1 |=
//   (1u<<1);` to `if (false) con1 |= (1u<<1);`, so the "sing=true" run no
//   longer actually sets bit 1 -> FAIL, "sing_on < normal" fails (identical
//   D-write counts). Reverted: 19/0.
// MUTATION LINE-MODE-LF-VALUE-0x4A-SELECTS-DOCUMENTED-FUNCTION stimulus red:
//   word index 3's C-input bit flipped (`c[3] ^= 0x8000`) after the
//   expected-output vector was already computed from the unmutated (A,B,C)
//   -> FAIL, that word's computed function no longer matches its own
//   expectation. Reverted: 19/0.
//
// Every mutation was confirmed reverted by re-running `make test` (all 19
// assertions green again) before moving to the next one; the final clean
// rebuild after the last revert also printed "19 passed, 0 failed".
//
// EXTENSION, 25.09.2026 (ch6-fig613.f27's two new assertions): both
// mutations below were run by THIS agent under WSL/Verilator on
// 25.09.2026, the same edit -> rebuild -> run -> revert -> rebuild cycle as
// above, against the file as extended (21 assertions). Both confirmed
// exactly ONE assertion red -- the one named -- with the other 20 staying
// green, and a final clean rebuild after the last revert printed "21
// passed, 0 failed".
// MUTATION FILL-CARRY-IN-BIT2-SEEDS-THE-FIRST-WORD-FILL-CARRY stimulus red:
//   the BLTCON1 write's `(fci ? (1u << 2) : 0)` term changed to an
//   unconditional `0`, so the "fci=true" run no longer actually sets bit 2
//   -> FAIL, "fci1 == 0xFFFF" fails (the stored word stays 0x0000, same as
//   the fci=false run). Reverted: 21/0.
// MUTATION AREA-IFE-BIT3-ENABLES-INCLUSIVE-FILL stimulus red: the BLTCON1
//   write changed from `ife ? (1u << 3) : 0x0000` to an unconditional
//   `0x0000`, so the "on" run no longer actually sets bit 3 -> FAIL,
//   "on == 0xFFFF" fails (the "on" run stays at the unfilled 0x8001, same
//   as the "off" run). Re-run against the tightened predicate (`on ==
//   0xFFFF` in place of the earlier `on != off`, which passed under any
//   value change and did not actually pin the inclusive-fill result to
//   0xFFFF): reverted, 22/0.
// MUTATION BLTCON0-BITS-7-0-EACH-BIT-SELECTS-ONLY-ITS-OWN-MINTERM stimulus
//   red: the per-bit probe's own LF stimulus changed from
//   `static_cast<uint16_t>(1u << k)` to `static_cast<uint16_t>(1u <<
//   ((k + 1) % 8))`, so each iteration k now sets a DIFFERENT LF bit than
//   the one its own (A,B,C) comparison expects -> FAIL, every one of the
//   eight per-k comparisons mismatches (the selected word is always one
//   position away from where the check looks). Reverted: 22/0.

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
            bit15 != bit14 && bit14 != bit13 && bit13 != bit12,
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
        // matching agnus_blitter_minterm.v's own convention). This is the
        // SAME physical wire in both modes (agnus_blitter.v:444's
        // `.lf(bltcon0[7:0])` is unconditional on `line`), so measuring it
        // once here legitimately covers both the area-mode and line-mode
        // table rows' claims about this byte's identity.
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
            "BLTCON0-BITS-7-0-ARE-THE-LF-BYTE-IN-BOTH-MODES: with only LF bit 7 "
            "set (0x80), only the all-(A,B,C)-ones selector word produces a 1 in "
            "the output; with only LF bit 0 set (0x01), only the all-zeros "
            "selector word does - pinning bits 7 and 0 as the two ends of the "
            "8-bit LF field, in both the AREA-MODE table ('07 LF7' through '00 "
            "LF0', p.267) and the LINE-MODE table (same rows, p.268, and "
            "'LF7-0 Logic function minterm select lines should be preloaded "
            "with 4A...', p.268). agnus_blitter.v:255-261 `bltcon0[7:0] <= "
            "data_in[7:0]` (unconditional on `line`), :444 `.lf(bltcon0[7:0])`. "
            "6-appa-control.c17, 6-appa-control.c19, 6-appa-control.c21, "
            "6-appa-control.c23, 6-appa-control.c25, 6-appa-control.c27, "
            "6-appa-control.c29, 6-appa-control.c31, 6-appa-control.c39, "
            "ch6-appa-bltcon0.area.b07.con0, ch6-appa-bltcon0.area.b06.con0, "
            "ch6-appa-bltcon0.area.b05.con0, ch6-appa-bltcon0.area.b04.con0, "
            "ch6-appa-bltcon0.area.b03.con0, ch6-appa-bltcon0.area.b02.con0, "
            "ch6-appa-bltcon0.area.b01.con0, ch6-appa-bltcon0.area.b00.con0, "
            "ch6-appa-bltcon0.legend.LF70, "
            "6-function-generator.c6, ch6-funcgen.c10, ch6-funcgen.c4, "
            "6-appa-control.c61, 6-appa-control.c63, 6-appa-control.c65, "
            "6-appa-control.c67, 6-appa-control.c69, 6-appa-control.c71, "
            "6-appa-control.c73, 6-appa-control.c75, 6-appa-control.c79, "
            "ch6-appa-bltcon0.line.b07.con0, ch6-appa-bltcon0.line.b06.con0, "
            "ch6-appa-bltcon0.line.b05.con0, ch6-appa-bltcon0.line.b04.con0, "
            "ch6-appa-bltcon0.line.b03.con0, ch6-appa-bltcon0.line.b02.con0, "
            "ch6-appa-bltcon0.line.b01.con0, ch6-appa-bltcon0.line.b00.con0 .");
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
            "AREA-MODE and LINE-MODE table rows '07 LF7' through '00 LF0' "
            "(pp.267-268). agnus_blitter_minterm.v:29-40 ties each lf[k] to "
            "its own AND gate before the final OR at :44 - eight independent "
            "gates, not one shared enable. "
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
        auto efe_output = [](bool efe) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.a_words = {0x8001};
            h.c_words = {0x0000};
            write_bltcon0(h, 0, USEA | USEC | USED, 0xF0);  // D=A passthrough into fill
            h.write_reg(BLTCON1, efe ? static_cast<uint16_t>(1u << 4) : 0x0000);
            h.write_reg(BLTSIZE, bltsize_word(1, 1));
            h.run_until_writes(1, 60);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        const uint16_t off = efe_output(false);
        const uint16_t on = efe_output(true);

        checks.expect(off == 0x8001 && on != off,
            "AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL: with BLTCON1 bit 4 clear the "
            "minterm output (0x8001, two isolated bits) passes through "
            "unfilled; with it set, the SAME minterm output is exclusive-filled "
            "into a different word - 'bit (FILL_XOR, or bit 4) in BLTCON1. The "
            "inclusive fill mode fills' (p.184), table row '04 LF4 EFE' (p.267, "
            "no dispute: p.184 and p.267 agree, and p.185's disputed bit is "
            "BLTCON1 bit 3, not bit 4). agnus_blitter.v:287 `assign efe = ~line "
            "& bltcon1[4]`, :455 feeds the fill block's efe input. "
            "6-appa-control.c24, 6-appa-control.c40, "
            "ch6-appa-bltcon0.area.b04.con1, ch6-appa-bltcon0.legend.EFE, "
            "6-area-fill.c9, ch6-fill.c8, ch6-fig613.f27 .");
    }

    // ---------------- FILL-CARRY-IN-BIT2-SEEDS-THE-FIRST-WORD-FILL-CARRY ----------------
    {
        // LF=0x00 makes the minterm output constant 0 regardless of A/B/C
        // (agnus_blitter_minterm.v:29-40: all eight lf bits clear means
        // every mt[k]=0, so out=0), isolating the fill carry chain
        // (agnus_blitter_fill.v: `carry[0]=fci^in[0]`, then
        // `carry[j]=carry[j-1]^in[j]` for j=1..15) from the minterm logic
        // PKG-BLT-MINTERM's own package tests (Non-goals above). With
        // in=0 throughout, carry[15:0] is FCI replicated across all 16
        // bits. EFE is held set so the fill block's output is the carry
        // itself (`out=carry` when efe, agnus_blitter_fill.v), making
        // FCI's effect visible on a single word without any multi-word/
        // 16-BLOCKS fill machinery -- the same single-word-blit technique
        // AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL above already uses.
        auto fci_output = [](bool fci) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.a_words = {0x8001};
            h.c_words = {0x0000};
            write_bltcon0(h, 0, USEA | USEC | USED, 0x00);  // LF=0: minterm output constant 0
            h.write_reg(BLTCON1, static_cast<uint16_t>((1u << 4) | (fci ? (1u << 2) : 0)));  // EFE=1 fixed, FCI (bit 2) toggled
            h.write_reg(BLTSIZE, bltsize_word(1, 1));
            h.run_until_writes(1, 60);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        const uint16_t fci0 = fci_output(false);
        const uint16_t fci1 = fci_output(true);

        checks.expect(fci0 == 0x0000 && fci1 == 0xFFFF,
            "FILL-CARRY-IN-BIT2-SEEDS-THE-FIRST-WORD-FILL-CARRY: with the minterm "
            "output held at a constant 0 (LF=0x00) and exclusive fill enabled "
            "(BLTCON1 bit 4), a single-word blit's stored word is 0x0000 with "
            "BLTCON1 bit 2 clear and 0xFFFF with it set - the fill carry chain "
            "seeded by, and only by, this one bit on a word with nothing else to "
            "carry. Figure 6-13 (p.199): the 'Fill Carry In' line into the fill "
            "logic, one of the five control lines ch6-fig613.f27 names; 'If the "
            "FCI bit is a 1 instead of a 0, the area outside the lines is filled' "
            "(p.185, Figure 6-6, the worked FCI example). agnus_blitter.v:464 "
            "`assign fci = first_word_del ? bltcon1[2] : fcy`, "
            "agnus_blitter_fill.v `carry[0] = fci ^ in[0]` chained through "
            "`carry[j] = carry[j-1] ^ in[j]` for j=1..15, `out = carry` when efe. "
            "ch6-fig613.f27 .");
    }

    // ---------------- AREA-IFE-BIT3-ENABLES-INCLUSIVE-FILL ----------------
    {
        // Same D=A-passthrough minterm output as AREA-EFE-BIT4 above
        // (LF=0xF0, A=0x8001), so `off` reproduces that assertion's own
        // unfilled baseline. BLTCON1 bit 2 (FCI) is left clear in both
        // runs here, so this run's fill-carry seed is 0 -- a different,
        // narrower probe than FILL-CARRY-IN-BIT2 above, which holds LF=0x00
        // and toggles bit 2 instead.
        auto ife_output = [](bool ife) {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.a_words = {0x8001};
            h.c_words = {0x0000};
            write_bltcon0(h, 0, USEA | USEC | USED, 0xF0);  // D=A passthrough into fill
            h.write_reg(BLTCON1, ife ? static_cast<uint16_t>(1u << 3) : 0x0000);
            h.write_reg(BLTSIZE, bltsize_word(1, 1));
            h.run_until_writes(1, 60);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        const uint16_t off = ife_output(false);
        const uint16_t on = ife_output(true);

        checks.expect(off == 0x8001 && on == 0xFFFF,
            "AREA-IFE-BIT3-ENABLES-INCLUSIVE-FILL: with BLTCON1 bit 3 clear the "
            "minterm output (0x8001, two isolated bits) passes through unfilled; "
            "with it set, the SAME minterm output is inclusive-filled into "
            "0xFFFF - 'The inclusive fill mode fills between lines, "
            "leaving the lines intact' (p.184), and appendix A p.267's bit-3 "
            "table row (the corpus's OCR of this exact row is defective -- see "
            "'CORPUS DEFECT FOUND' above -- but the PDF scan and "
            "appA-blitter/p267.txt agree bit 3 is IFE, matching "
            "agnus_blitter.v:288 below). Figure 6-13 (p.199): the 'Fill Enable' "
            "line into the fill logic, one of the five control lines "
            "ch6-fig613.f27 names, drawn as one line covering both this bit and "
            "BLTCON1 bit 4 (AREA-EFE-BIT4-ENABLES-EXCLUSIVE-FILL above, which "
            "already carries ch6-fig613.f27 for the EFE half); this assertion is "
            "new coverage for the IFE half, not previously asserted anywhere in "
            "this file. agnus_blitter.v:288 `assign ife = ~line & bltcon1[3]`, "
            ":455 feeds the fill block's ife input; agnus_blitter_fill.v "
            "`out = carry | in` when ife (and not efe). "
            "ch6-fig613.f27 .");
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
        // The SAME BLTCON0 (USEA|USEC|USED, ash=15, LF=0) and the SAME
        // BLTSIZE(width=2,height=1) word behave as two entirely different
        // FSMs depending on this one bit: as an ordinary 2-word rectangle
        // (2 destination writes) with LINE=0, or as a single-pixel line
        // draw (1 destination write, the width field no longer meaning
        // "words per row") with LINE=1.
        auto we_count_for = [](bool line_mode) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            write_bltcon0(h, 15, USEA | USEC | USED, 0x00);
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
            "'the two basic modes, area and line, which are selected by bit 0 of "
            "BLTCON1' (p.267), table rows '00 LF0 LINE(=0)' (p.267) and '00 LF0 "
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

    // ---------------- LINE-MODE-BLTCON0-BIT9-USEC-GATES-C-CHANNEL-FETCH ----------------
    {
        auto c_granted = [](bool usec) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            const uint16_t use = static_cast<uint16_t>(USEA | USED | (usec ? USEC : 0));
            write_bltcon0(h, 15, use, 0x00);
            h.write_reg(BLTCON1, LINE_BIT);
            h.write_reg(BLTSIZE, bltsize_word(2, 2));
            h.run_cycles(60);
            for (auto& g : h.grants) if (g.channel == 'C') return true;
            return false;
        };
        const bool with_usec = c_granted(true);
        const bool without_usec = c_granted(false);

        checks.expect(with_usec && !without_usec,
            "LINE-MODE-BLTCON0-BIT9-USEC-GATES-C-CHANNEL-FETCH: with BLTCON0 bit "
            "9 (USEC) set, channel C is granted during a line-mode blit; with it "
            "clear (bits 11 and 8 held identical in both runs), C is never "
            "granted - 'The SRCA, SRCC, and DEST bits of BLTCON0 should be set "
            "to one' (p.191), 'The C DMA channel represents the original [...]' "
            "(p.191), table row '09 1 0' (appendix A p.268, SRCC/USEC). "
            "agnus_blitter.v:823 `dma_req = usec;` inside BLT_L2, the line "
            "mode's C-channel fetch state. "
            "6-appa-control.c57, 6-line-mode.c62, 6-line-mode.c87, "
            "6-line-mode.c68, ch6-appa-bltcon0.line.b09.con0 .");
    }

    // ---------------- LINE-MODE-BLTCON0-BITS-11-10-8-ARE-NOT-HARDWARE-ENFORCED ----------------
    {
        // BLTCON0 bits 11 (USEA), 10 (USEB) and 8 (USED) are read the SAME
        // way in line mode as in area mode (agnus_blitter.v:252 is
        // unconditional on `line`), but no line-mode FSM state
        // (BLT_L1-BLT_L4, agnus_blitter.v:795-869) ever tests usea, useb or
        // used - only usec (previous assertion). CONTROL: clearing usec on
        // this exact measurement basis DOES change the signature (already
        // shown above with a boolean; re-demonstrated here on the same
        // string-signature basis the sweep below uses).
        auto line_signature = [](uint16_t use_bits) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            write_bltcon0(h, 15, use_bits, 0x00);
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
        const std::string control_no_c = line_signature(USEA | USED);
        const std::string no_a = line_signature(USEC | USED);
        const std::string b_set = line_signature(USEA | USEB | USEC | USED);
        const std::string no_d = line_signature(USEA | USEC);

        checks.expect(
            documented != control_no_c &&
            no_a == documented && b_set == documented && no_d == documented,
            "LINE-MODE-BLTCON0-BITS-11-10-8-ARE-NOT-HARDWARE-ENFORCED [control: "
            "clearing USEC (bit 9, shown load-bearing above) on the same "
            "measurement DOES change the grant/we signature]: clearing USEA "
            "(bit 11), setting USEB (bit 10) against its documented zero, or "
            "clearing USED (bit 8) each leaves the grant/we signature over a "
            "2-pixel line blit IDENTICAL to the documented-pattern baseline - "
            "'The SRCA, SRCC, and DEST bits of BLTCON0 should be set to one, and "
            "the SRCB flag should be set to zero' (p.191), matching table rows "
            "'11 1 0', '10 0 0', '08 1 0' (appendix A p.268). "
            "agnus_blitter.v:795-869 (BLT_L1-BLT_L4) never reads usea, useb or "
            "used. "
            "6-line-mode.c61, 6-line-mode.c63, 6-line-mode.c64, 6-line-mode.c86, "
            "6-line-mode.c88, 6-line-mode.c89, ch6-line.c32, ch6-line.c33, "
            "ch6-linesum.c13, ch6-linesum.c14, 6-appa-control.c53, "
            "6-appa-control.c55, 6-appa-control.c59, "
            "ch6-appa-bltcon0.line.b11.con0, ch6-appa-bltcon0.line.b10.con0, "
            "ch6-appa-bltcon0.line.b08.con0 .");
    }

    // ---------------- LINE-MODE-BLTCON1-BITS-11-10-9-8-7-5-ARE-UNUSED-OR-RESERVED ----------------
    {
        // FIXED (was: SUD alone as control, comparing a flat "every grant's
        // address, any channel" signature). Two problems in the original:
        // (1) it mixed C-bank grant addresses (BLT_L2) and D/C-bank grant
        // addresses (BLT_L4) into one undifferentiated string; (2) SUD alone,
        // held against SUL=AUL=0, is a tautological no-op in
        // agnus_blitter.v:609-612's own formula (`lineinc = (SUD&&!AUL) ||
        // (!SUD&&!SUL&&!sign_del)`: with SUL=AUL=0 both branches evaluate to
        // 1 regardless of SUD), so the control could never have shown a
        // difference and this assertion could not pass regardless of the
        // "unused" bits. CONTROL, corrected: SING (bit 1) + SUD (bit 4)
        // together are shown genuinely wired by the fixed LINE-MODE-SIGN and
        // LINE-MODE-SING assertions above (both now green), gating the
        // D-channel write count itself (agnus_blitter.v:860) - a simpler,
        // more robust observable than an address sequence for a control.
        auto line_write_count = [](uint16_t con1_extra) {
            Harness h;
            for (int i = 0; i < 6; ++i) h.c_words.push_back(0x0000);
            write_bltcon0(h, 15, USEA | USEC | USED, 0x00);
            h.write_reg(BLTCON1, static_cast<uint16_t>(LINE_BIT | con1_extra));
            h.write_reg(BLTSIZE, bltsize_word(2, 3));
            h.run_cycles(90);
            h.settle();
            return h.data_writes.size();
        };
        const size_t baseline = line_write_count(0);
        const size_t control = line_write_count(static_cast<uint16_t>((1u << 1) | (1u << 4) | (1u << 6)));  // SING+SUD+SIGN
        const uint8_t unused_bits[] = {11, 10, 9, 8, 7, 5};
        bool all_unchanged = true;
        for (uint8_t b : unused_bits)
            if (line_write_count(static_cast<uint16_t>(1u << b)) != baseline) all_unchanged = false;

        checks.expect(control != baseline && all_unchanged,
            "LINE-MODE-BLTCON1-BITS-11-10-9-8-7-5-ARE-UNUSED-OR-RESERVED [control: "
            "setting bits 1+4+6 (SING+SUD+SIGN) together, on the identical "
            "measurement, DOES change the D-channel write count - SIGN=1 is "
            "needed alongside SING+SUD to make `first_pixel` ever drop to 0, "
            "see LINE-MODE-SING's header note]: setting BLTCON1 bit 11, 10, "
            "9, 8, 7 or 5 alone (LINE still set) leaves the number of D-channel "
            "writes over a 3-pixel line blit IDENTICAL to the all-clear baseline "
            "- table row values '0','0','0','0','0','0 (Reserved)' at bits "
            "11,10,9,8,7,5 (appendix A p.268), and 'The OVFLAG should be "
            "cleared' (p.191, p.192 register summary). agnus_blitter.v:277-283 "
            "stores BLTCON1[11:0] wholesale; no expression in the file reads "
            "bits 11, 10, 9, 8, 7 or 5. "
            "6-appa-control.c54, 6-appa-control.c56, 6-appa-control.c58, "
            "6-appa-control.c60, 6-appa-control.c62, 6-appa-control.c66, "
            "6-appa-control.c88, "
            "ch6-appa-bltcon0.line.b11.con1, ch6-appa-bltcon0.line.b10.con1, "
            "ch6-appa-bltcon0.line.b09.con1, ch6-appa-bltcon0.line.b08.con1, "
            "ch6-appa-bltcon0.line.b07.con1, ch6-appa-bltcon0.line.b05.con1, "
            "ch6-appa-lf.legend.reserved, 6-line-mode.c65, 6-line-mode.c93, "
            "ch6-line.c34, ch6-linesum.c18 .");
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
        // The previous version of this assertion filtered h.grants for
        // channel=='A' to find that pointer's step - but line mode's FSM
        // (BLT_L1-L4, agnus_blitter.v:795-869) never sets chsel==CHA at a
        // state where dma_req is asserted (chsel is CHA only at BLT_L1 and
        // BLT_L3, both dma_req=0 there), so no 'A' grant is EVER recorded
        // in line mode and first/second stayed -1 for every run,
        // structurally forcing delta15==delta0==0 regardless of ash or any
        // other RTL behavior. FIXED: observe the D-channel write ADDRESS
        // SEQUENCE (agnus_blitter.v:491's `we`) over 4 pixels instead -
        // ash=15 makes lineinc fire on pixel 1 (ash==1111), stepping the
        // pointer bank one word early relative to ash=0, which propagates
        // into a different address trajectory for every later pixel too.
        auto d_addr_sequence = [](uint16_t ash) {
            Harness h;
            for (int i = 0; i < 8; ++i) h.c_words.push_back(0x0000);
            h.write_reg(BLTAPTH, 0);
            h.write_reg(BLTAPTL, 0);       // accumulator stays 0 all run (BLTAMOD/BLTBMOD also left 0)
            h.write_reg(BLTDPTH, 0);
            h.write_reg(BLTDPTL, 0x1000);
            write_bltcon0(h, ash, USEA | USEC | USED, 0x00);
            h.write_reg(BLTCON1, LINE_BIT);  // octant 6: SUD=SUL=AUL=0
            h.write_reg(BLTSIZE, bltsize_word(2, 4));
            h.run_cycles(120);
            h.settle();
            std::vector<uint32_t> addrs;
            for (auto& g : h.grants) if (g.channel == 'D') addrs.push_back(g.address);
            return addrs;
        };
        const auto seq15 = d_addr_sequence(15);
        const auto seq0 = d_addr_sequence(0);

        checks.expect(seq15.size() == 4 && seq0.size() == 4 && seq15 != seq0,
            "LINE-MODE-START-BITS-15-12-REUSE-THE-ASH-FIELD: starting a line "
            "blit (octant 6) with BLTCON0[15:12]=15 versus =0 - all else "
            "identical - changes the D-channel write-address sequence over a "
            "4-pixel line, confirming line mode's START3-0 field is the SAME "
            "physical bits as area mode's ASH3-0, genuinely consumed (not "
            "merely stored) as the line's starting bit-position value - table "
            "rows '15 START3 ...' through '12 START0 ...' (appendix A p.268), "
            "'START3-0 Starting point of line' (p.268 legend), 'The A bit is "
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
        // `bsh` (BLTCON1[15:12]) is the identical unconditional wire
        // AREA-BSH-BITS-15-12-ARE-B-SHIFT-VALUE already measured
        // (agnus_blitter.v:274 has no `line` condition); this assertion
        // adds the one line-mode-specific fact: line mode reads bit 0 of
        // the SHIFTED result, not the raw register (agnus_blitter.v:410
        // `bltbhold <= {16{shiftbout[0]}}` when `line`). BLTBDAT=0x0001
        // (bit 0 only) with bsh=0 (no shift) should leave that bit at
        // position 0; with bsh=15 (maximum shift) it should not.
        auto texture_bit = [](uint16_t bsh) {
            Harness h;
            for (int i = 0; i < 2; ++i) h.c_words.push_back(0x0000);
            // FIXED (was h.b_words = {...}): line mode's FSM (BLT_L1-L4,
            // agnus_blitter.v:795-869) never sets chsel==CHB and never
            // requests a DMA grant for channel B -- b_words, which is only
            // consumed on a CHB grant, is dead stimulus here and BLTBDAT
            // stayed at its reset value of 0 regardless of bsh. BLTBDAT is
            // the same register either way (reg_address_in/data_in path,
            // agnus_blitter.v:359-366); in line mode the programmer loads it
            // with an ordinary CPU register write instead of via DMA fetch,
            // which is what write_reg does.
            write_bltcon0(h, 0, USEC | USED, 0xCC);  // D=B passthrough
            h.write_reg(BLTCON1, static_cast<uint16_t>(LINE_BIT | (bsh << 12)));
            h.write_reg(BLTBDAT, 0x0001);
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_cycles(50);
            h.settle();
            return h.data_writes.empty() ? uint16_t{0xDEAD} : h.data_writes[0];
        };
        const uint16_t noshift = texture_bit(0);
        const uint16_t maxshift = texture_bit(15);

        checks.expect(noshift == 0xFFFF && maxshift != noshift,
            "LINE-MODE-TEXTURE-BITS-15-12-REUSE-THE-BSH-FIELD: with BLTBDAT's "
            "single set bit at position 0 and BLTCON1[15:12]=0 (no shift), the "
            "line's texture replication reads that bit as 1 (D=0xFFFF, since "
            "line mode replicates shiftbout[0] across all 16 output bits); with "
            "BLTCON1[15:12]=15 (maximum shift) the SAME BLTBDAT no longer reads "
            "as 1 there - confirming BLTCON1[15:12] is the identical BSH3-0 "
            "field, genuinely consumed as line mode's texture-bit selector, not "
            "a separately-named but inert field - table rows '15 ... TEXTURE3' "
            "through '12 ... TEXTURE0' (appendix A p.268), 'The \"B\" source is "
            "used for texturing the drawn lines' (p.268). "
            "6-appa-control.c46, 6-appa-control.c48, 6-appa-control.c50, "
            "6-appa-control.c52, "
            "ch6-appa-bltcon0.line.b15.con1, ch6-appa-bltcon0.line.b14.con1, "
            "ch6-appa-bltcon0.line.b13.con1, ch6-appa-bltcon0.line.b12.con1, "
            "ch6-appa-lf.c8 .");
    }

    // ---------------- LINE-MODE-SIGN-BIT6-LOADS-INITIAL-ACCUMULATOR-SIGN ----------------
    {
        // SING=1 and SUD=1 (ONEDOT-style setup) make `first_pixel` gate
        // channel C's DMA request (agnus_blitter.v:860); `first_pixel`
        // itself, from the second pixel on, is `~sign_del`
        // (agnus_blitter.v:913), and `sign_del` is last pixel's `sign`
        // (agnus_blitter.v:940) - which on the very first BLTCON1 write is
        // loaded directly from bit 6 (agnus_blitter.v:932 `sign <=
        // data_in[6]`). So the INITIAL bit-6 value should change how many
        // of 3 pixels request channel C.
        // FIXED (was: count grants with channel=='C'): channel C's own DMA
        // request (BLT_L2, agnus_blitter.v:823 `dma_req = usec;`) is
        // unconditional -- it never reads bltcon1 at all, so it cannot
        // possibly show SIGN's effect and a 'C'-grant count is identical
        // for both runs by construction. SING/SUD gate the D-CHANNEL store
        // instead (BLT_L4, agnus_blitter.v:860, chsel==CHD, `we` asserted -
        // agnus_blitter.v:491 `we = ackdma && chsel==CHD`), which is exactly
        // what h.data_writes (populated only when `we` fires) already
        // counts. Using it directly is simpler and more direct than
        // filtering h.grants by channel letter.
        auto d_write_count = [](bool sign) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            write_bltcon0(h, 15, USEA | USEC | USED, 0x00);
            uint16_t con1 = static_cast<uint16_t>(LINE_BIT | (1u << 1) /*SING*/ | (1u << 4) /*SUD*/);
            if (sign) con1 |= (1u << 6);
            h.write_reg(BLTCON1, con1);
            h.write_reg(BLTSIZE, bltsize_word(2, 3));
            h.run_cycles(90);
            h.settle();
            return h.data_writes.size();
        };
        const size_t sign1 = d_write_count(true);
        const size_t sign0 = d_write_count(false);

        checks.expect(sign1 != sign0,
            "LINE-MODE-SIGN-BIT6-LOADS-INITIAL-ACCUMULATOR-SIGN: with SING and "
            "SUD set (ONEDOT-style), the number of D-channel writes (of 3 "
            "pixels) differs between BLTCON1 bit 6 initially 1 versus "
            "initially 0, all else identical - table row '06 LF6 SIGN' "
            "(appendix A p.268), 'SIGN Sign flag' (p.268 legend). "
            "agnus_blitter.v:932 `sign <= data_in[6];  // initial sign value`, "
            "loaded on every BLTCON1 write regardless of `line`. "
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
            write_bltcon0(h, 15, USEA | USEC | USED, 0x00);
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

        checks.expect(all_distinct && complete && sud_live && sul_live && aul_live,
            "LINE-MODE-SUD-SUL-AUL-BITS-4-3-2-AND-OCTANT-TABLE: the eight printed "
            "(SUD,SUL,AUL) triples for octants 0-7 are pairwise distinct (verified "
            "against the PDF page image, tools/show_page.py --page 268), and each "
            "of BLTCON1 bit 4 (SUD), bit 3 (SUL) and bit 2 (AUL) independently "
            "changes the D-channel write-address sequence of a 4-pixel line when "
            "probed against the adjacent octant-table row that isolates it - "
            "table rows '04 LF4 SUD','03 LF3 SUL','02 LF2 AUL' and the octant "
            "table (appendix A p.268), 'SUD Sometimes up or down', 'SUL "
            "Sometimes up or left', 'AUL Always up or left' (p.268 legend). "
            "agnus_blitter.v:609-612 read bltcon1[4],[3],[2] directly in the "
            "lineinc/linedec/lineadd/linesub formulas. "
            "6-appa-control.c68, 6-appa-control.c70, 6-appa-control.c72, "
            "ch6-appa-bltcon0.line.b04.con1, ch6-appa-bltcon0.line.b03.con1, "
            "ch6-appa-bltcon0.line.b02.con1, "
            "ch6-appa-lf.legend.SUD, ch6-appa-lf.legend.SUL, ch6-appa-lf.legend.AUL, "
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
        // FIXED (was: count grants with channel=='C', name said "GATES
        // C-CHANNEL"): agnus_blitter.v:860's `dma_req = usec & (~bltcon1[1]
        // | ~bltcon1[4] | first_pixel);` is BLT_L4's dma_req -- the
        // D-CHANNEL store (chsel==CHD there, unconditionally), not the C
        // fetch. Channel C's own request (BLT_L2, agnus_blitter.v:823
        // `dma_req = usec;`) never reads bltcon1 and is unconditional, so a
        // 'C'-grant count is identical whether SING is set or not, by
        // construction -- it cannot show what this claim is about. SING (with
        // SUD) actually suppresses repeat D-channel WRITES within the
        // horizontal run (the manual's own "single bit per horizontal line",
        // matching agnus_blitter_line's LINE-ONEDOT-SUPPRESSES-REPEAT-WRITES
        // finding for the same bit). h.data_writes.size() -- populated only
        // when `we` fires (agnus_blitter.v:491, chsel==CHD) -- is the direct
        // count of D-channel writes and needs no channel filtering.
        // ALSO NEEDED: SIGN (bit 6) set to 1. `first_pixel` (agnus_blitter.v
        // :907-914) only ever drops to 0 via `first_pixel <= ~sign_del`, and
        // sign_del (:937-941) only becomes 1 by inheriting the INITIALLY
        // WRITTEN SIGN bit at pixel 1's own BLT_L1 (`sign_del<=sign`, using
        // sign's pre-edge value, i.e. the value the BLTCON1 write itself set
        // - agnus_blitter.v:929-935) - because BLTAPTL/BLTAMOD/BLTBMOD are
        // all left at their reset value of 0 here (this file never gives the
        // real Bresenham accumulator a nonzero modulo to move it, which is
        // out of scope - see the LINE-MODE-START/-SUD-SUL-AUL header notes),
        // `sign` itself is overwritten to sign_out=0 starting pixel 1's own
        // BLT_L1 and stays 0 for every LATER pixel regardless of the initial
        // SIGN bit. So with SIGN left at its default 0 (as the original,
        // now-fixed draft of this assertion left it), first_pixel never
        // drops and SING's gate never fires for either run - both measure
        // 3/3, and the assertion cannot distinguish SING on/off no matter
        // how the D-writes are counted. Setting SIGN=1 makes sign_del=1 at
        // pixel 1, dropping first_pixel to 0 for pixel 2 specifically (and
        // back to 1 from pixel 3 on, since `sign` has already reverted to
        // its sign_out-driven 0 by then) - exactly the one-pixel gap SING
        // is supposed to produce.
        auto d_writes_over_pixels = [](bool sing) {
            Harness h;
            for (int i = 0; i < 4; ++i) h.c_words.push_back(0x0000);
            write_bltcon0(h, 15, USEA | USEC | USED, 0x00);
            uint16_t con1 = static_cast<uint16_t>(LINE_BIT | (1u << 4) | (1u << 6));  // SUD=1, SIGN=1
            if (sing) con1 |= (1u << 1);
            h.write_reg(BLTCON1, con1);
            h.write_reg(BLTSIZE, bltsize_word(2, 3));
            h.run_cycles(90);
            h.settle();
            return h.data_writes.size();
        };
        const size_t normal = d_writes_over_pixels(false);
        const size_t sing_on = d_writes_over_pixels(true);

        checks.expect(sing_on < normal && sing_on >= 1,
            "LINE-MODE-SING-BIT1-GATES-D-CHANNEL-WRITES-TO-FIRST-PIXEL: over a "
            "3-pixel line blit with SUD and SIGN set, the D channel writes "
            "fewer times with SING (BLTCON1 bit 1) set than with it clear - "
            "table row '01 LF1 SING' (appendix A p.268), 'SING Single bit per "
            "horizontal line for use with subsequent area fill' (p.268 "
            "legend). "
            "agnus_blitter.v:860 `dma_req = usec & (~bltcon1[1] | ~bltcon1[4] "
            "| first_pixel);` inside BLT_L4 (chsel==CHD, the D-channel store "
            "state, not the C fetch - see agnus_blitter.v:491 `we = ackdma && "
            "chsel==CHD` and :813-823's BLT_L2, whose own dma_req never reads "
            "bltcon1 at all). "
            "6-appa-control.c74, 6-appa-control.c89, "
            "ch6-appa-bltcon0.line.b01.con1, ch6-appa-lf.legend.SING .");
    }

    // ---------------- LINE-MODE-LF-VALUE-0x4A-SELECTS-DOCUMENTED-FUNCTION ----------------
    {
        // Eight one-word blits enumerate all 8 (A,B,C) selector
        // combinations with LF=$4A: the manual's own worked line-mode
        // function, quoted at appendix A p.268 as "D=(AC+ABC)" (overbars
        // lost in the OCR corpus; 6-line-mode.c90/c91 restate it, with the
        // negations, as (A&B&~C) OR (~A&C)). Uses the same area-mode
        // combinational measurement as BLTCON0-BITS-7-0-ARE-THE-LF-BYTE-
        // IN-BOTH-MODES, for the same reason: `lf` is the unconditional
        // wire bltcon0[7:0] regardless of `line` (agnus_blitter.v:444), so
        // measuring what 0x4A produces in area mode is equally valid
        // evidence for line mode's use of it.
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        std::vector<uint16_t> a, b, c, expected;
        for (int k = 0; k < 8; ++k) {
            const bool A = (k & 4) != 0, B = (k & 2) != 0, C = (k & 1) != 0;
            a.push_back(A ? 0x8000 : 0x0000);
            b.push_back(B ? 0x8000 : 0x0000);
            c.push_back(C ? 0x8000 : 0x0000);
            const bool out = (A && B && !C) || (!A && C);
            expected.push_back(out ? 0x8000 : 0x0000);
        }
        h.a_words.assign(a.begin(), a.end());
        h.b_words.assign(b.begin(), b.end());
        h.c_words.assign(c.begin(), c.end());
        start_blit(h, USEA | USEB | USEC | USED, 0x4A, 8, 1);
        h.run_until_writes(8, 200);
        h.settle();

        checks.expect(h.data_writes == expected,
            "LINE-MODE-LF-VALUE-0x4A-SELECTS-DOCUMENTED-FUNCTION: LF=0x4A "
            "computes (A AND B AND NOT C) OR (NOT A AND C) across all eight "
            "(A,B,C) selector combinations, exactly as the worked example "
            "describes - 'LF7-0 [...] should be preloaded with 4A to select the "
            "equation D=(AC+ABC). Since A contains a single bit true (8000), "
            "most bits will pass the C field unchanged (not A and C), but one "
            "bit will invert the C field and combine it with texture (A and B "
            "and not C)' (appendix A p.268). agnus_blitter_minterm.v:29-40 "
            "(lf[6] selects A&B&~C, lf[1]/lf[3] together select ~A&C). "
            "6-line-mode.c90, 6-line-mode.c91 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
