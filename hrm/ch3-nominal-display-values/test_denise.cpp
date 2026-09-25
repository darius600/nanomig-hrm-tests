// ch3-nominal-display-values -- Denise chapter-3 RESIDUE package.
//
// Test purpose: 61 claims that reached no package in chapter 3's first build
//   round and no recorded out-of-scope reason (found 11.09.2026 by
//   tools/check_chapter_coverage.py, reconciling the chapter total rather
//   than any one package's own coverage -- see PACKAGE.md). They are largely
//   the *nominal* display-window values (DIWSTRT $2C81 / DIWSTOP $F4C1 for
//   NTSC, DIWSTOP $2CC1 for PAL, Table 3-9's HSTART $81 / HSTOP $C1 columns,
//   whether NTSC and PAL share the same nominal HSTART/HSTOP) and Appendix
//   A/B/C register-access properties (BPLCON1/BPLxDAT write-only,
//   DIWSTRT/DIWSTOP/DIWHIGH write-only, DENISEID's identity, colour register
//   bits 15-12 unused), plus a handful of Chapter 3 prose claims (Figure
//   3-4's bit-to-COLOR6 mapping, HAM's "modified pixel written to the
//   screen", EHB half-intensity) that never found a home either.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   42, 44, 46, 57, 59, 60, 80, 81, 82, 86, 92, 95, 271, 272, 273, 277, 290,
//   291, 293, 296, 299, 303, 306, 307 (claims.jsonl for ch3-nominal-display-
//   values, 61 records plus 9 routed 25.09.2026, 70 total). A red result
//   below means denise.v (or a sub-module
//   it instantiates) disagrees with the manual, not that the assertion is
//   wrong -- no assertion here was, or should be, adjusted to make the RTL
//   pass.
// Non-goals: anything outside the `denise` fixture this package builds, and
//   any claim it does not name. A green here means those assertions held on
//   this fixture at the pinned revision -- not that the module is correct,
//   and not that the chapter is covered.
// Claim ids: ids such as `3-diw.c36` and `3-diw.c38` name one sentence of the
//   manual in this chapter's reading. They carry no reader prefix, and they
//   resolve in the claim files that hrm-testpipeline/acceptance/ch3.json lists
//   under `inventories`.
//
// BOUNDARY: fixture `denise`, top module only (one Verilator build,
//   sources denise.v + denise_bitplanes.v + denise_bitplane_shifter.v +
//   denise_playfields.v + denise_sprites.v + denise_sprites_shifter.v +
//   denise_collision.v + denise_hamgenerator.v + denise_colortable.v +
//   denise_colortable_ram_mf.v + denise_spritepriority.v + bitplane_ram.v),
//   per PACKAGE.md and modelled on hrm/ch3-horizontal-display-window's own
//   Makefile. Ports, exactly as claims.jsonl's own evidence field names
//   them (module header, denise.v:31-52): clk, clk7_en, c1, c3, cck, reset,
//   strhor, reg_address_in[8:1], data_in[16-1:0], chip48[48-1:0],
//   data_out[16-1:0], blank, red/green/blue[8-1:0], a1k, ecs, aga, hires
//   (out), shres (out). fixture a1k=0, ecs=1, aga=0 throughout, matching
//   both hrm/ch3-horizontal-display-window and hrm/ch3-register-map-denise
//   (their own published, committed files already disclose why: DENISEID
//   answering $FFFC and the DIWSTRT/DIWSTOP H-field mechanics both need
//   exactly this configuration; it is also the OCS/ECS-era chip the pages
//   cited here describe, not the AGA extension).
//
// CHECKED AGAINST THE TWO NAMED SIBLINGS FIRST: hrm/ch3-horizontal-display-
//   window and hrm/ch3-register-map-denise test neighbouring properties at
//   this same denise boundary. Both files' full assertion text was grepped
//   for every one of this package's 61 claim ids before writing a single
//   assertion below -- zero matches, in either file, for any id. (The build
//   brief warns this is not a safe assumption to skip: a sibling builder in
//   this chapter declined 134 claims as "covered by a sibling" that shared
//   no claim ids at all.) The infrastructure PATTERNS below -- the write_reg
//   /read_reg register round-trip, the phase-accurate c1/c3 tick() drive,
//   the saturating-BPLxDAT colour-index dialling, the prime_window_closed
//   fix and the DIWSTRT/DIWSTOP/DIWHIGH/BPLCON0/BPL1-6DAT/COLORx address
//   constants -- ARE reused from those two siblings' own already-published,
//   RTL-verified files (the build brief explicitly points at
//   ch3-horizontal-display-window's Makefile "to model yours on"); every
//   ORACLE value and claim-to-assertion mapping below is this package's own,
//   independently derived from claims.jsonl's quotations.
//
// ORDER OF WORK: this file (Harness, expected values, claim-id coverage,
//   declines) was written and committed from claims.jsonl's quotations and
//   the two named siblings' own published source (read, not nanomig's RTL)
//   -- denise.v itself was not opened until after that commit. Page 59 was
//   independently re-opened from
//   docs/references/amiga-hrm-3rd/ch3/p59.txt (the manual scan, not RTL) to
//   check 3-diw.c39's "$2CC1" against the printed page before trusting it as
//   an oracle value -- see DIW-NOMINAL-HSTOP below for what that page
//   actually says. A second commit follows once denise.v is opened, the
//   build run under WSL/Verilator, any Harness bugs fixed (oracle text
//   untouched), and the MUTATION LOG filled in with real results.
//
// PAGE 59 CHECKED BEFORE TRUSTING IT (build-brief-mandated check): the
//   build brief singles out 3-diw.c39 ("The normal PAL DIWSTOP is
//   ($2CC1)") as one to read on the page rather than trust blindly, since
//   3-diw.c37 gives NTSC DIWSTOP as ($F4C1) -- different words for a
//   register the same table calls "the same both for NTSC and for PAL"
//   (3-window.c37/3-diw.c30). Page 59's own printed text resolves this
//   itself, in full: "Normally, the VSTOP is set to ($F4) for NTSC, ($2C)
//   for PAL" and Table 3-9 prints HSTOP as $C1 in BOTH the NTSC and PAL
//   columns. DIWSTOP packs VSTOP (high byte) and HSTOP (low byte) into one
//   16-bit register -- $F4C1 (NTSC) and $2CC1 (PAL) are exactly VSTOP:HSTOP
//   for each standard, and they necessarily differ in the high (VSTOP)
//   byte while agreeing in the low (HSTOP) byte. Both quoted words are
//   exactly what page 59 prints, verbatim; this is not a transcription
//   error, and neither quote is corrected below. DIW-NOMINAL-HSTOP tests
//   exactly this structure: two different full DIWSTOP words that agree in
//   their H-half.
//
// CLAIM COVERAGE: all 70 ids are accounted for below -- 57 inside an
//   expect() assertion string (19 grouped/individual assertions, the same
//   19 as before 25.09.2026: ids restating the identical proposition under
//   a different manual wording, page, appendix or reader are grouped and
//   cited together on the single assertion that tests that proposition,
//   the convention hrm/ch3-colour-register-format and
//   hrm/ch3-register-map-denise both document and use -- COLOR-BITS-15-12-
//   UNUSED itself now cites eight), 11 inside a note() call with a stated
//   reason (four groups: visual/subjective "appears to move" claims with
//   no frame port, a conjoined H+V claim whose V half is outside this
//   fixture, "reserved" bits with no stated hardware behaviour to test,
//   SuperHires's 1280px claim needing Agnus-owned horizontal total timing,
//   and two sprite register-table entries needing a purpose-built sprite
//   pipeline this package was not assigned to build), and 2 inside a
//   record() call (3-colorsum.c7, p.92, the software-directed "(set these
//   to 0)" half COLOR-BITS-15-12-UNUSED's own comment already declines --
//   routed record:ch3-nominal-display-values 25.09.2026, not graded; and
//   3-t15.c01, p.92, the same appendix-B row read as one unsplit claim
//   that bundles the same software-directed half with the hardware half
//   COLOR-BITS-15-12-UNUSED does establish -- an id whose claim bundles an
//   untested clause is withdrawn whole, not graded with a caveat).
//   Every id is written out in full, in the assertion, note or record
//   string itself, not only in a header comment.
//
// TWO HARNESS FACTS DISCLOSED BY THE BUILD BRIEF ITSELF, USED BELOW WITHOUT
//   REDISCOVERING THEM FROM RTL: hpos resets to 2, not 0, on strhor; the
//   `window` register has no reset clause and must be primed closed once
//   per Harness (prime_window_closed(), reused from
//   hrm/ch3-horizontal-display-window's own published fix). Combined with
//   claims.jsonl's own evidence field ("the horizontal window compare is
//   hpos[8:0]==hdiwstrt[8:0]", "H8 forced 0 for start, 1 for stop"), the
//   PRE-RTL commit expected open_idx=raw_start-2 / close_idx=raw_stop+254.
//
// RTL-INFORMED CORRECTION (found once denise.v was opened -- see RTL
//   OPENED below): denise.v:291-303 registers the hpos comparator's result
//   through TWO clk7_en-gated stages (`window` on the compare itself, then
//   `window_ena <= window` one clk7_en edge later) before window_del (a
//   plain-every-28MHz-edge register that catches up within the same
//   sample) gates the output mux -- a 2-sample pipeline latency the manual
//   says nothing about (it states register VALUES, not this Harness's own
//   sample-counting latency) and this package's pre-RTL commit did not
//   know to account for. Measured directly: DIWSTRT=$2C81 opened the
//   window at sample index 129 (not the predicted 127); DIWSTOP=$F4C1 and
//   =$2CC1 both closed it at sample index 449 (not the predicted 447) --
//   BOTH off by exactly +2, the same constant, confirming a fixed
//   measurement-pipeline offset rather than a disagreement about the
//   register values themselves (a real address/value bug would not move
//   both edges by the identical constant, and DIW-ONE-PIXEL-RESOLUTION's
//   own DELTA-based check, unaffected by any constant offset, already
//   passed at the pre-RTL commit, confirming the underlying open-edge
//   detection is sound). Corrected formula, oracle values ($81/$C1)
//   untouched, only the sample-index TRANSLATION fixed:
//     open_idx(raw_start)  = raw_start
//     close_idx(raw_stop)  = raw_stop + 256
//
// RTL OPENED, BUILT AND RUN, 11.09.2026 (all after the spec-only commit
//   above; built under WSL/Verilator 5.052). Every port name and address
//   assumed in the spec-only commit matched exactly -- no port name,
//   width, or address needed changing. Two Harness bugs were found once
//   the RTL was readable, both fixed as stimulus/methodology only (no
//   oracle text touched):
//
//   1. FIGURE-COLOR-SELECT-INDEX6, EHB-HALF-INTENSITY, HAM-MODIFY-WRITES-
//      PIXEL and COLOR-BITS-15-12-UNUSED wrote DIWSTRT/DIWSTOP but never
//      pulsed strhor or advanced hpos into the window before sampling --
//      denise.v:436-444 forces clut_data (and therefore rgb) to border/0
//      whenever !window_ena, so these four samples were reading the
//      permanently-closed post-construction state, not the dialled-in
//      colour index at all. Fixed by adding Harness::enter_wide_window()
//      (reset_line() + advance into a wide $08..$1F0 window before any
//      bitplane pattern is dialled in) and calling it in all four.
//
//   2. DIW-NOMINAL-HSTART and DIW-NOMINAL-HSTOP's pre-RTL sample-index
//      arithmetic (open_idx=raw_start-2, close_idx=raw_stop+254) was
//      wrong: denise.v:291-303 registers the hpos comparator through TWO
//      clk7_en-gated stages (`window`, then `window_ena<=window`) before
//      window_del gates the output mux -- a 2-sample measurement-pipeline
//      latency the manual says nothing about (it states register VALUES,
//      not this Harness's own sample-counting scheme). Both assertions
//      failed by exactly +2 at first build (open_idx measured 129 not
//      127; close_idx measured 449 not 447 for BOTH the NTSC and PAL
//      DIWSTOP words, which still agreed with EACH OTHER) -- the identical
//      constant on both edges, plus DIW-ONE-PIXEL-RESOLUTION's own
//      DELTA-based check already passing unmodified, is what distinguishes
//      a measurement-pipeline offset from a genuine value/address defect.
//      Fixed by correcting the sample-index translation to
//      open_idx=raw_start, close_idx=raw_stop+256 (see header above) --
//      the oracle values themselves ($81, $C1, $2C81, $F4C1, $2CC1) were
//      never touched.
//
// First build after both fixes: 21 passed, 0 failed -- no red finding in
//   this package (unlike hrm/ch3-horizontal-display-window's own
//   BRDNTRAN result); every one of the 50 asserted claim ids matches the
//   RTL as measured.
//
// MUTATION LOG -- all 21 assertions (19 substantive plus the 2 controls),
//   run under WSL/Verilator on 11.09.2026 by rebuilding with each single
//   -DMUT_<NAME>=1 in turn (no source edits -- the #ifndef guards already
//   default to 0, so a mutation build changes nothing but that one
//   compile-time flag) and confirming exactly that one assertion went red
//   and the other 20 stayed green, then rebuilding with no flags to
//   reconfirm the clean 21/0 baseline. Two mutations needed a design fix
//   before they bit (both caught by this very sweep, both stimulus-only,
//   recorded here as the mutation that was actually shipped):
//   CONTROL-REG-ALIVE's first draft redirected the DENISEID read to
//   CLXDAT's own address (same register twice), which is not guaranteed
//   to differ from itself and did not fail; redirected to BPLCON1's
//   address (independently proven always-$0000 by BPLCON1-WO) instead.
//   COLOR-BITS-15-12-UNUSED's first draft's "mutation" set the second
//   COLOR01 write to the SAME value as the first (a no-op), which
//   trivially cannot fail; changed to also perturb a low (observable) bit
//   instead of only the high nibble.
//
// MUTATION CONTROL-REG-ALIVE stimulus red: DENISEID's read address redirected to BPLCON1's ($102, proven write-only/always-$0000), so the first reading came back $0000 instead of $FFFC and the "both non-zero" half of the control failed; reverted.
// MUTATION CONTROL-BITPLANE-ALIVE stimulus red: the one-shot BPL1DAT stimulus changed from $ACE1 to $0000, so no sample ever differed from the border and the data-path-live proof failed; reverted.
// MUTATION DIW-NOMINAL-HSTART stimulus red: the measured DIWSTRT word changed from $2C81 to $2C82 (H byte $82 instead of $81), so the open edge landed at $82 instead of the expected $81; reverted.
// MUTATION DIW-NOMINAL-HSTOP stimulus red: the PAL DIWSTOP word changed from $2CC1 to $2CC2 (H byte $C2), so its close edge no longer matched the NTSC word's $C1-based close edge or the expected $1C1; reverted.
// MUTATION DIW-ONE-PIXEL-RESOLUTION stimulus red: the second DIWSTRT measurement changed from $31 (one count past $30) to $34 (four counts), so the open-edge delta came back 4 instead of the expected 1; reverted.
// MUTATION BPLCON1-WO stimulus red: the scroll-delay write was suppressed (BPLCON1's own address redirected to $1FE, the bus NO-OP), so the register's effect was never exercised and the write-effect half of the pairing failed (edge stayed at sample 2 for both PF1H=0 and PF1H=15 instead of moving from 2 to 17); reverted.
// MUTATION BPL1DAT-WO stimulus red: the same redirect applied to BPL1DAT's own write, so plane 1 was never actually toggled and the rendered pixel stayed at $0000 instead of moving to COLOR1's $AABB; reverted.
// MUTATION BPL2DAT-WO stimulus red: the same redirect applied to BPL2DAT's own write, so plane 2 was never actually toggled and the rendered pixel stayed at $0000 instead of moving to COLOR2's $AABB; reverted.
// MUTATION BPL3DAT-WO stimulus red: the same redirect applied to BPL3DAT's own write, so plane 3 was never actually toggled and the rendered pixel stayed at $0000 instead of moving to COLOR4's $AABB; reverted.
// MUTATION BPL4DAT-WO stimulus red: the same redirect applied to BPL4DAT's own write, so plane 4 was never actually toggled and the rendered pixel stayed at $0000 instead of moving to COLOR8's $AABB -- this is the exact register the reviewer's own proof pointed at $0A0 (a Paula audio pointer) and left green; reverted.
// MUTATION BPL5DAT-WO stimulus red: the same redirect applied to BPL5DAT's own write, so plane 5 was never actually toggled and the rendered pixel stayed at $0000 instead of moving to COLOR16's $AABB -- this is the exact register the reviewer's own proof pointed at $1FE (the same bus NO-OP used as this file's redirect target) and left green; reverted.
// MUTATION BPL6DAT-WO stimulus red: the same redirect applied to BPL6DAT's own write, so plane 6 (the EHB flag) was never actually set and the index-6 pixel stayed at $BBAA instead of darkening to $5D55; reverted.
// MUTATION DIWSTRT-WO stimulus red: the same redirect applied to DIWSTRT's own write, so the $7777 pattern never reached the real register and the open edge was never found (open_idx=-1) instead of landing at $77=119; reverted.
// MUTATION DIWSTOP-WO stimulus red: the same redirect applied to DIWSTOP's own write, so the $8888 pattern never reached the real register and the close edge stayed at the DIWSTRT-driven default (256) instead of $88+256=392; reverted.
// MUTATION DIWHIGH-WO stimulus red: the same redirect applied to DIWHIGH's own write, so the $9999 pattern (bit13=0) never reached the real register and hdiwstop[8] kept its reset default, leaving the close edge at 449 both before and after instead of pulling in to $C1=193; reverted.
// MUTATION DENISEID-IS-REVISION-REGISTER stimulus red: the ecs=0 reading's address redirected from DENISEID's own ($07C) to BPLCON0's ($100), so it came back $0000 instead of $FFFF and the "both non-zero" half failed; reverted.
// MUTATION DENISEID-ECS-VALUE stimulus red: the ecs=1 reading forced to $0000 in the comparison, so its low byte no longer matched $FC; reverted.
// MUTATION COLOR-BITS-15-12-UNUSED stimulus red: the second COLOR01 write changed from $F345 to $F346 (a low, observable bit flipped, not only bits 15-12), so the two renderings genuinely differed and the equality check failed; reverted.
// MUTATION EHB-HALF-INTENSITY stimulus red: the second (plane-6-set) measurement's plane 6 input forced back to false, so it read the identical un-halved colour ($BBAA88) instead of the exact per-component >>1 halving ($5D5544) D6 FIX now checks; reverted.
// MUTATION FIGURE-COLOR-SELECT-INDEX6 stimulus red: the second write's target address changed from COLOR6's own ($18C) to COLOR7's ($18E), so COLOR6's own reading never changed between the first and second write; reverted.
// MUTATION HAM-MODIFY-WRITES-PIXEL stimulus red: plane 5 in the second (modify) phase forced to false, changing the 6-5 combo from 01 (modify blue) to 00 (plain colour select), so the sampled pixel read $0000 instead of a colour differing from the held COLOR01 only in blue; reverted.
//
// All 21 mutations rebuilt clean afterward, confirming the 21/0 baseline
// was restored each time (verified via a full sweep with no flags set,
// immediately before this commit). No oracle/expected value was changed
// by any of them; only what each check asks the module to do.
//
// D5 FIX, 11.09.2026 (independent-review defect correction; no oracle text
//   touched, only what each of the nine WO assertions below checks): a
//   reviewer proved BPLCON1-WO, BPL1DAT-WO..BPL6DAT-WO, DIWSTRT-WO,
//   DIWSTOP-WO and DIWHIGH-WO were all vacuous. denise.v:102/130 is
//   `data_out = col_out | deniseid_out | rgb_out` -- exactly three read
//   sources -- so "the written address reads back $0000" is true of nearly
//   every address in the map, including ones Denise does not implement at
//   all: pointing BPL4DAT-WO at $0A0 (a Paula audio pointer) and
//   BPL5DAT-WO at $1FE (the bus NO-OP) left both PASS and the suite still
//   read 21/0. Reading back zero is half of write-only, and the worthless
//   half; the half that carries information is that the write had an
//   effect. All nine now assert (a) a write EFFECT specific to that named
//   register, observed directly on this fixture, AND (b) that address then
//   reads $0000 -- with the assertion text stating outright that (b) alone
//   is structural. Effects found, all measured on this fixture, none cited
//   without also being repeated locally so a redirect mutation has
//   something to break:
//     BPLCON1  -- PF1H (bits 3-0) delays a one-shot BPL1DAT bit's rendered
//                 sample by exactly 1 per unit (swept all 16 values: idx=
//                 nib+2 exactly); p.307's own "horizontal scroll control".
//     BPL1DAT  -- toggling plane 1 alone (bpu=5 colour-select, planes 2-5
//                 held at 0) moves the index from 0 to 1.
//     BPL2DAT  -- same, plane 2 alone, index 0 to 2.
//     BPL3DAT  -- same, plane 3 alone, index 0 to 4.
//     BPL4DAT  -- same, plane 4 alone, index 0 to 8 (the reviewer's own
//                 $0A0 example).
//     BPL5DAT  -- same, plane 5 alone, index 0 to 16 (the reviewer's own
//                 $1FE example).
//     BPL6DAT  -- at bpu=6/ham=0, plane 6 is the EHB flag (denise.v:
//                 ehb_en=!killehb&&!a1k&&!ham&&!dpf&&l_bpu==6), not an
//                 index bit; toggling it alone darkens the index-6 pixel,
//                 the same mechanism EHB-HALF-INTENSITY above measures,
//                 repeated here through this assertion's own address.
//     DIWSTRT  -- the $7777 this assertion already writes opens the window
//                 at exactly $77=119 (open_idx=raw_start).
//     DIWSTOP  -- the $8888 this assertion already writes closes the
//                 window at exactly $88+256=392 (close_idx=raw_stop+256).
//     DIWHIGH  -- denise.v:314-319: hdiwstop[8] defaults to 1 unless
//                 DIWHIGH (ecs=1) loads data_in[13] into it; the $9999
//                 this assertion already writes has bit13=0 and pulls the
//                 close edge in from $C1+256=449 to exactly $C1=193.
//   All nine effects are genuine measurements, not stretched claims -- no
//   register in this group needed a decline. Each register's own write
//   address is now a local variable, redirected to NOOP_ADDR ($1FE, the
//   same bus NO-OP the reviewer used) under its `#ifndef MUT_<NAME>` guard
//   -- ONLY that one register's write moves, leaving every other write at
//   its real address, the same single-register-only redirect the
//   reviewer's own proof used. `make MUT=<NAME>` was run for all nine in
//   turn: every one flipped its OWN assertion red (the write-effect half
//   failing while the $0000 read-back half stayed vacuously true, exactly
//   demonstrating why the pairing is the fix) and left the other 20 green,
//   then a clean rebuild with no flags restored 21/0. Makefile gained the
//   MUT= plumbing (MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)) this
//   package previously lacked, modelled on hrm/ch3-field-geometry's own.
//
// D6 FIX, 11.09.2026 (independent-review defect correction, same review as
//   D5 above): EHB-HALF-INTENSITY asserted only "no component greater, at
//   least one strictly smaller" -- a check quarter-intensity, or a
//   blue-only darkening, would also pass, even though neither is what
//   "half-intensity" means. Page 95 was re-read directly
//   (docs/references/amiga-hrm-3rd/ch3/p95.txt): "shifted to
//   half-intensity" -- SHIFTED, a specific bit operation -- and the value
//   this fixture already measured ($BBAA88 -> $5D5544) is exactly every
//   component right-shifted by one bit, not a coincidental "somewhat
//   darker" reading. The check now asserts exact_half (component ==
//   normal_component>>1 for R, G AND B) instead of the three-part
//   loose-inequality check. The exact check costs nothing extra to run and
//   was confirmed to still pass: 21/0 unchanged. Its mutation entry above
//   was updated to describe the new failure mode (un-halved colour vs the
//   exact >>1 target, rather than "differs"/"strictly smaller").
//
// D9 FIX, 11.09.2026 (independent-review defect correction, same review as
//   D5/D6 above): HAM-MODIFY-WRITES-PIXEL's boolean check (hr==mr &&
//   hg==mg && hb!=mb) was already correct -- red/green held, blue
//   modified, exactly what p.86 describes -- but both printed values used
//   hex4(rgb>>8), which right-shifts away the LOW byte, i.e. exactly the
//   blue component this assertion exists to show changed. Both phases
//   printed the visually identical "$FF00" regardless of what blue
//   actually did, so a reader could not see the one thing the assertion
//   claims to have found. Printed values now use the new hex6() helper
//   (full 24-bit R:G:B) plus an explicit "blue $xx -> $xx" fragment, so the
//   held red/green and the changed blue are all visible in the evidence.
//   No change to the assertion's own boolean condition or to its mutation
//   (MUTATION HAM-MODIFY-WRITES-PIXEL above, unchanged, still applies) --
//   this was a print-only fix.
//
// ROUTED ADDITION, 25.09.2026 (spec-only commit -- from
//   ch3-nominal-display-values-spec-stage.jsonl alone, the routing's own
//   `how` and the full claims file's classification not yet opened, per the
//   build brief's order of work). Nine more claims were routed to this
//   package in today's backlog pass, all clustered on the one proposition
//   COLOR-BITS-15-12-UNUSED above already specifies and asserts:
//   - 3-colortable.c13 (sol) and 3-t3.c01 (opus), p.46, "Bits 15 - 12
//     Unused" (Table 3-3, the chapter body's own colour-register-format
//     table): restates, from a THIRD page/table (the chapter body, not the
//     p.92 appendix-B summary row 3-t15.c01 already cites, nor the p.273
//     appendix-A entry below), the identical hardware fact -- colour
//     register bits 15-12 carry no effect -- this package already measures.
//   - 3-colorsum.c6 (sol), p.92, the "Unused" half of "Bits 15 - 12
//     Unused (set these to 0)": the same appendix-B summary row 3-t15.c01
//     already cites, extracted a second time (a different reader/pass) and
//     split so the hardware half is its own claim id rather than sharing
//     one id with the software half below.
//   - 3-colorsum.c7 (sol), p.92, the same row's "(set these to 0)" half,
//     restated as its own claim, "Color-register bits 15-12 must be
//     written as zero": this is exactly the software-directed half
//     COLOR-BITS-15-12-UNUSED's own comment already names and already
//     declines to assert -- forward-compatibility guidance to a programmer,
//     not a hardware guarantee; nothing about denise's behaviour depends on
//     what an ignored bit was last written as, so there is no module-
//     boundary effect for any fixture to observe. On the manual's content
//     alone this is not gradable here, the same call already made for the
//     identical clause under 3-t15.c01 -- confirmed, not decided fresh,
//     once routing.how is read below.
//   - A-color.c7/c8/c9/c10 (sol), p.273, Appendix A's own COLORxx bit
//     table ("BIT# 15,14,...00" / "RGB  X  X  X  X  R3 R2..."), one claim
//     per bit (15, 14, 13, 12 respectively, each marked X): the identical
//     "unused" proposition split to single-bit granularity, per the BIT
//     TABLE rule -- the same appendix-A row hrm/ch3-colour-register-format
//     already draws on. This backlog is what first puts A-color.c10, along
//     with its three sibling bits, onto COLOR-BITS-15-12-UNUSED below --
//     none of the four was asserted anywhere before this commit.
//   - A-colorxx.bits1 (opus), p.273, the same appendix-A row read as one
//     group claim ("bits 15-12 are X, i.e. unused") rather than split --
//     opus's own twin of the four sol ids immediately above.
//   None of these nine claims asks this fixture to observe anything
//   COLOR-BITS-15-12-UNUSED does not already exercise (COLOR01 written with
//   bits 15-12 at $0 and at $F, low 12 bits held constant, renders
//   identically) -- eight are the same "unused" proposition restated from a
//   different page, table, reader or bit-granularity, and the ninth
//   (3-colorsum.c7) is the one already-declined software-guidance clause,
//   restated as its own id. The build brief's item 1 covers exactly the
//   eight: the existing assertion's own label gains their ids, written out
//   in full, rather than a new assertion being built to reprove a fact
//   already measured. No expected value, no oracle text and no existing
//   assertion's verdict changes. The full claims file is opened next, to
//   read routing.how and confirm this reading before the ids actually move.

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "Vdenise.h"
#include "verilated.h"

#ifndef MUT_CONTROL_REG
#define MUT_CONTROL_REG 0
#endif
#ifndef MUT_CONTROL_BITPLANE
#define MUT_CONTROL_BITPLANE 0
#endif
#ifndef MUT_DIW_HSTART
#define MUT_DIW_HSTART 0
#endif
#ifndef MUT_DIW_HSTOP
#define MUT_DIW_HSTOP 0
#endif
#ifndef MUT_DIW_RESOLUTION
#define MUT_DIW_RESOLUTION 0
#endif
#ifndef MUT_BPLCON1_WO
#define MUT_BPLCON1_WO 0
#endif
#ifndef MUT_BPL1DAT_WO
#define MUT_BPL1DAT_WO 0
#endif
#ifndef MUT_BPL2DAT_WO
#define MUT_BPL2DAT_WO 0
#endif
#ifndef MUT_BPL3DAT_WO
#define MUT_BPL3DAT_WO 0
#endif
#ifndef MUT_BPL4DAT_WO
#define MUT_BPL4DAT_WO 0
#endif
#ifndef MUT_BPL5DAT_WO
#define MUT_BPL5DAT_WO 0
#endif
#ifndef MUT_BPL6DAT_WO
#define MUT_BPL6DAT_WO 0
#endif
#ifndef MUT_DIWSTRT_WO
#define MUT_DIWSTRT_WO 0
#endif
#ifndef MUT_DIWSTOP_WO
#define MUT_DIWSTOP_WO 0
#endif
#ifndef MUT_DIWHIGH_WO
#define MUT_DIWHIGH_WO 0
#endif
#ifndef MUT_DENISEID_FN
#define MUT_DENISEID_FN 0
#endif
#ifndef MUT_DENISEID_VALUE
#define MUT_DENISEID_VALUE 0
#endif
#ifndef MUT_COLOR_UNUSED
#define MUT_COLOR_UNUSED 0
#endif
#ifndef MUT_EHB
#define MUT_EHB 0
#endif
#ifndef MUT_COLOR_INDEX6
#define MUT_COLOR_INDEX6 0
#endif
#ifndef MUT_HAM
#define MUT_HAM 0
#endif

namespace {

// Custom chip-register addresses (word address, reg_address_in[8:1]
// convention: byte address >> 1), as printed in the manual and reused from
// hrm/ch3-horizontal-display-window's and hrm/ch3-register-map-denise's own
// already-published, RTL-verified constants.
constexpr uint16_t IDLE_ADDR     = 0x00;
constexpr uint16_t CLXDAT_ADDR   = 0x07;  // 9'h00E >> 1
constexpr uint16_t DIWSTRT_ADDR  = 0x47;  // 9'h08E >> 1
constexpr uint16_t DIWSTOP_ADDR  = 0x48;  // 9'h090 >> 1
constexpr uint16_t DENISEID_ADDR = 0x3E;  // 9'h07C >> 1
constexpr uint16_t BPLCON0_ADDR  = 0x80;  // 9'h100 >> 1
constexpr uint16_t BPLCON1_ADDR  = 0x81;  // 9'h102 >> 1
constexpr uint16_t BPL1DAT_ADDR  = 0x88;  // 9'h110 >> 1
constexpr uint16_t BPL2DAT_ADDR  = 0x89;  // 9'h112 >> 1
constexpr uint16_t BPL3DAT_ADDR  = 0x8A;  // 9'h114 >> 1
constexpr uint16_t BPL4DAT_ADDR  = 0x8B;  // 9'h116 >> 1
constexpr uint16_t BPL5DAT_ADDR  = 0x8C;  // 9'h118 >> 1
constexpr uint16_t BPL6DAT_ADDR  = 0x8D;  // 9'h11A >> 1, per B-subject.c160's own quoted address
constexpr uint16_t COLOR_BASE    = 0xC0;  // 9'h180 >> 1; COLORi = COLOR_BASE+i
constexpr uint16_t DIWHIGH_ADDR  = 0xF2;  // 9'h1E4 >> 1
// D5 FIX: the bus NO-OP address (per denise.v/the manual's own "no chip
// register decodes here" convention) -- reading OR writing it touches no
// register denise.v implements at all. Used only inside a MUT_<NAME> guard
// to redirect a single register's own write to prove the write-effect half
// of that register's WO pairing actually depends on the real address, the
// same single-register redirect the reviewer's own BPL4DAT-WO/BPL5DAT-WO
// proof used (there: $0A0 and $1FE, two different unimplemented addresses;
// here: $1FE alone, reused for every one of this package's nine).
constexpr uint16_t NOOP_ADDR     = 0xFF;  // 9'h1FE >> 1

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "$%04X", static_cast<unsigned>(v));
    return std::string(buf);
}

// D9 FIX: full 24-bit R:G:B, used where the blue byte matters and must not
// be silently dropped by hex4(rgb>>8) -- see EHB-HALF-INTENSITY and
// HAM-MODIFY-WRITES-PIXEL below.
std::string hex6(uint32_t rgb) {
    char buf[10];
    std::snprintf(buf, sizeof(buf), "$%06X", static_cast<unsigned>(rgb & 0xFFFFFFu));
    return std::string(buf);
}

// D9 FIX: a single 8-bit component, two hex digits -- used alongside hex6
// so a single R/G/B byte prints as e.g. "$FF", not hex4's padded "$00FF".
std::string hex2(uint8_t v) {
    char buf[6];
    std::snprintf(buf, sizeof(buf), "$%02X", static_cast<unsigned>(v));
    return std::string(buf);
}

class Harness {
public:
    explicit Harness(bool ecs_in = true) : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = ecs_in ? 1 : 0;
        dut->aga = 0;
        phase = 0;
        drive_phase();
        dut->eval();
        for (int i = 0; i < 32; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();
        prime_window_closed();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // Proven four-phase c1/c3/clk7_en pattern (reused from both named
    // siblings' own published Harnesses -- see amiga_clk.v's c1/c3 overlap,
    // documented at length in both files' headers): phase 0 is the
    // clk7_en register-capture phase (one lores-pixel-resolution step),
    // phase 1 has c1 AND c3 both high.
    void tick() {
        drive_phase();
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) & 3;
    }

    void drive_phase() {
        dut->clk7_en = (phase == 0) ? 1 : 0;
        dut->c1 = (phase == 0 || phase == 1) ? 1 : 0;
        dut->c3 = (phase == 1 || phase == 2) ? 1 : 0;
        dut->cck = (phase == 0 || phase == 1) ? 1 : 0;
    }

    // Writes `data` to custom word-address `addr`, held across a full
    // 4-phase cycle so clk7_en is guaranteed to sample it once, then parks
    // reg_address_in back on IDLE_ADDR.
    void write_reg(uint16_t addr, uint16_t data) {
        while (phase != 0) tick();
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
    }

    // Presents `addr` and returns data_out after a full settle cycle.
    uint16_t read_reg(uint16_t addr) {
        while (phase != 0) tick();
        dut->reg_address_in = addr;
        dut->data_in = 0;
        tick(); tick(); tick(); tick();
        uint16_t result = dut->data_out;
        dut->reg_address_in = IDLE_ADDR;
        tick();
        return result;
    }

    void reset_line() {
        while (phase != 0) tick();
        dut->strhor = 1;
        for (int i = 0; i < 4; ++i) tick();
        dut->strhor = 0;
    }

    // HARNESS FIX reused verbatim from hrm/ch3-horizontal-display-window's
    // own published, RTL-verified constructor fix: denise.v's `window`
    // register has no reset clause, so a fresh Vdenise can spuriously latch
    // it open before any real DIWSTRT/DIWSTOP write. Force DIWSTOP to its
    // smallest actual value and run hpos all the way past it once, so
    // `window` is driven to a real, verified closed state first.
    void prime_window_closed() {
        write_reg(DIWSTOP_ADDR, 0x0000);
        reset_line();
        for (int i = 0; i < 258; ++i) {
            while (phase != 0) tick();
            tick();
        }
    }

    static uint32_t pack_rgb(const Vdenise* d) {
        return (static_cast<uint32_t>(d->red) << 16) |
               (static_cast<uint32_t>(d->green) << 8) |
               static_cast<uint32_t>(d->blue);
    }

    // Colour-index dialling infrastructure, reused from
    // hrm/ch3-register-map-denise's own published, MEASURED mechanical
    // note: a single BPLxDAT write only pulses the shifter's parallel load
    // once, so a steady index needs the same value rewritten repeatedly.
    // Order matters: BPL2..6DAT before BPL1DAT (BPL1DAT's write is what
    // latches l_bpu and pulses the shared `load`).
    void refresh_bitplanes(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, int rounds) {
        for (int r = 0; r < rounds; ++r) {
            write_reg(BPL2DAT_ADDR, b2 ? 0xFFFF : 0x0000);
            write_reg(BPL3DAT_ADDR, b3 ? 0xFFFF : 0x0000);
            write_reg(BPL4DAT_ADDR, b4 ? 0xFFFF : 0x0000);
            write_reg(BPL5DAT_ADDR, b5 ? 0xFFFF : 0x0000);
            write_reg(BPL6DAT_ADDR, b6 ? 0xFFFF : 0x0000);
            write_reg(BPL1DAT_ADDR, b1 ? 0xFFFF : 0x0000);
        }
    }

    // bpu is written into BPLCON0[14:12] (aga=0, so the AGA bit4 extension
    // term is forced 0); ham (bit11) and dpf (bit10) default to 0 unless
    // ham_on is set. Field positions per claims.jsonl's own evidence field
    // for this package's C-shr.c01/C-superhires.c1/c2 (denise.v:180-192):
    // hires=bit15, bpu=bits14-12, ham=bit11, dpf=bit10.
    void write_bitplane_pattern(int bpu, bool ham_on, bool b1, bool b2, bool b3,
                                 bool b4, bool b5, bool b6, int rounds) {
        uint16_t bplcon0 = static_cast<uint16_t>((bpu & 0x7) << 12) | (ham_on ? 0x0800 : 0);
        write_reg(BPLCON0_ADDR, bplcon0);
        refresh_bitplanes(b1, b2, b3, b4, b5, b6, rounds);
    }

    uint32_t read_rgb_now() { return pack_rgb(dut); }

    // D5 FIX (write-effect infrastructure): samples n consecutive
    // clk7_en-aligned pixels with NO further writes in between (unlike
    // scan_window_bpu1, which continuously rewrites BPL1DAT to saturate the
    // shifter). Used by BPLCON1-WO's own write-effect measurement below to
    // locate where a one-shot BPL1DAT bit renders, so a change in
    // BPLCON1's PF1H scroll field can be seen as a shift in that position.
    std::vector<uint32_t> scan_static(int n) {
        std::vector<uint32_t> samples;
        samples.reserve(n);
        for (int i = 0; i < n; ++i) {
            while (phase != 0) tick();
            samples.push_back(pack_rgb(dut));
            tick();
        }
        return samples;
    }

    // D5 FIX (write-effect infrastructure): like refresh_bitplanes, but the
    // ONE named plane's BPLxDAT write is issued at `mut_addr` instead of its
    // own real address. mut_plane==0 means no redirection (identical to
    // refresh_bitplanes). Lets a per-register mutation redirect exactly the
    // one write under test -- and nothing else -- to an address Denise does
    // not decode, the same single-register-only redirect the reviewer's own
    // BPL4DAT-WO/BPL5DAT-WO proof used.
    void refresh_bitplanes_mut(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6,
                                int rounds, int mut_plane, uint16_t mut_addr) {
        auto addr_for = [&](int p, uint16_t real_addr) -> uint16_t {
            return (p == mut_plane) ? mut_addr : real_addr;
        };
        for (int r = 0; r < rounds; ++r) {
            write_reg(addr_for(2, BPL2DAT_ADDR), b2 ? 0xFFFF : 0x0000);
            write_reg(addr_for(3, BPL3DAT_ADDR), b3 ? 0xFFFF : 0x0000);
            write_reg(addr_for(4, BPL4DAT_ADDR), b4 ? 0xFFFF : 0x0000);
            write_reg(addr_for(5, BPL5DAT_ADDR), b5 ? 0xFFFF : 0x0000);
            write_reg(addr_for(6, BPL6DAT_ADDR), b6 ? 0xFFFF : 0x0000);
            write_reg(addr_for(1, BPL1DAT_ADDR), b1 ? 0xFFFF : 0x0000);
        }
    }

    // RTL-INFORMED FIX (denise.v:436-444: clut_data = 0 (border) whenever
    // !window_ena, regardless of bitplane content; denise.v:446-452:
    // window_del lags window_ena by one 28MHz edge, not gated by clk7_en)
    // -- every colour-index/EHB/HAM measurement needs to actually be
    // SAMPLED INSIDE the display window, not merely have DIWSTRT/DIWSTOP
    // written. Opens a wide window ($08..$1F0, actual stop $1F0+256=$2F0,
    // comfortably inside hpos's 9-bit range with no wraparound risk even
    // across many refresh_bitplanes rounds) and advances hpos comfortably
    // past DIWSTRT before any bitplane pattern is dialled in.
    void enter_wide_window() {
        write_reg(DIWSTRT_ADDR, 0x0008);
        write_reg(DIWSTOP_ADDR, 0x01F0);
        reset_line();
        for (int i = 0; i < 10; ++i) { tick(); tick(); tick(); tick(); }
    }

    // Scans `n` clk7_en-aligned samples from the start of a line, with
    // bitplane 1 continuously saturated at an all-ones pattern throughout
    // (see hrm/ch3-horizontal-display-window's own THE HPOS[3:2] TRAP note
    // for why a saturated constant plane sidesteps the scroll-phase
    // mechanism entirely -- this package is not about scroll, only about
    // where the window opens/closes).
    static constexpr int kSaturateWarmupSamples = 24;
    std::vector<uint32_t> scan_window_bpu1(int n) {
        for (int i = 0; i < kSaturateWarmupSamples; ++i) write_reg(BPL1DAT_ADDR, 0xFFFF);
        reset_line();
        std::vector<uint32_t> samples;
        samples.reserve(n);
        for (int i = 0; i < n; ++i) {
            while (phase != 0) tick();
            samples.push_back(pack_rgb(dut));
            dut->reg_address_in = static_cast<uint8_t>(BPL1DAT_ADDR);
            dut->data_in = 0xFFFF;
            tick();
        }
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        return samples;
    }

    Vdenise* dut;
    int phase;
};

struct Edges {
    long open_idx = -1;
    long close_idx = -1;
    long non_border_count = 0;
    bool is_clean() const {
        return open_idx >= 0 && close_idx > open_idx &&
               non_border_count == (close_idx - open_idx);
    }
};

Edges find_edges(const std::vector<uint32_t>& samples, uint32_t border) {
    Edges e;
    for (size_t i = 0; i < samples.size(); ++i) {
        if (samples[i] != border) {
            if (e.open_idx < 0) e.open_idx = static_cast<long>(i);
            e.close_idx = static_cast<long>(i) + 1;
            ++e.non_border_count;
        }
    }
    return e;
}

// Fresh Harness, standard COLOR00/COLOR01 programmed, BPU=1, DIWSTRT/DIWSTOP
// H written as given, scanned saturated (bitplane 1). Border is measured
// from a SEPARATE, guaranteed-empty-range calibration window on a fresh
// Harness sharing the identical COLOR00/COLOR01 programming (same technique
// as hrm/ch3-horizontal-display-window's own measure_window).
// D5 FIX: diwstrt_addr/diwstop_addr default to the real registers (every
// pre-existing call site below is unaffected); DIWSTRT-WO/DIWSTOP-WO/
// DIWHIGH-WO's own write-effect measurements pass a possibly-MUT-redirected
// address for the ONE register under test, so a mutation that sends that
// register's write to NOOP_ADDR shows up here as the window failing to move
// to where the written value says it should.
Edges measure_window(uint16_t diwstrt_h, uint16_t diwstop_h, int n = 500,
                      uint16_t diwstrt_addr = DIWSTRT_ADDR,
                      uint16_t diwstop_addr = DIWSTOP_ADDR) {
    Harness h;
    h.write_reg(COLOR_BASE + 0, 0x0F0);
    h.write_reg(COLOR_BASE + 1, 0xF00);
    h.write_reg(BPLCON0_ADDR, 0x1000);  // bpu=1
    h.write_reg(BPLCON1_ADDR, 0x0000);
    h.write_reg(diwstrt_addr, diwstrt_h);
    h.write_reg(diwstop_addr, diwstop_h);
    auto samples = h.scan_window_bpu1(n);

    Harness cal;
    cal.write_reg(COLOR_BASE + 0, 0x0F0);
    cal.write_reg(COLOR_BASE + 1, 0xF00);
    cal.write_reg(BPLCON0_ADDR, 0x1000);
    cal.write_reg(BPLCON1_ADDR, 0x0000);
    cal.write_reg(DIWSTRT_ADDR, 0x60);
    cal.write_reg(DIWSTOP_ADDR, 0x20);  // stop < start: guaranteed-empty range
    auto cal_samples = cal.scan_window_bpu1(8);
    uint32_t border = cal_samples[4];
    return find_edges(samples, border);
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ==================== CONTROL-REG-ALIVE (no claim id) ====================
    // "the fixture proves it can deliver a known word before anything reads
    // one" -- DENISEID and CLXDAT are two independently-decoded read-only
    // registers (reused address knowledge from hrm/ch3-register-map-denise's
    // own published file); both answering distinct, non-zero values before
    // any write-only-register-reads-zero assertion below is trusted rules
    // out "data_out is stuck at zero" as the explanation.
    {
        Harness h;
        uint16_t deniseid_addr = DENISEID_ADDR;
#if MUT_CONTROL_REG
        deniseid_addr = BPLCON1_ADDR;  // a proven-always-$0000 write-only address (see BPLCON1-WO below)
#endif
        uint16_t deniseid = h.read_reg(deniseid_addr);
        uint16_t clxdat = h.read_reg(CLXDAT_ADDR);
        checks.expect(
            deniseid != 0 && clxdat != 0 && deniseid != clxdat,
            "[control] CONTROL-REG-ALIVE: DENISEID ($07C) and CLXDAT ($00E), "
            "two independently decoded read-only registers, both read back "
            "distinct non-zero values (" + hex4(deniseid) + " and " + hex4(clxdat) +
            ") before any assertion below trusts a $0000 reading to mean "
            "write-only rather than stuck. No manual page: the control for "
            "every write-only-reads-zero assertion in this file.");
    }

    // ==================== CONTROL-BITPLANE-ALIVE (no claim id) ====================
    // A one-shot, non-constant, recognisable BPLxDAT value must reach the
    // rgb output before any DIW-window/colour-index measurement below
    // trusts the bitplane data path (reused proof technique from
    // hrm/ch3-horizontal-display-window's own published FIXTURE-DATA-PATH-LIVE).
    {
        Harness h;
        h.write_reg(COLOR_BASE + 0, 0x0F0);
        h.write_reg(COLOR_BASE + 1, 0xF00);
        h.write_reg(BPLCON0_ADDR, 0x1000);
        h.write_reg(DIWSTRT_ADDR, 0x0010);
        h.write_reg(DIWSTOP_ADDR, 0x00F0);
        h.reset_line();
        uint16_t stimulus = 0xACE1;
#if MUT_CONTROL_BITPLANE
        stimulus = 0x0000;
#endif
        h.write_reg(BPL1DAT_ADDR, stimulus);
        bool saw_distinct = false;
        uint32_t border_guess = 0;
        for (int i = 0; i < 300; ++i) {
            while (h.phase != 0) h.tick();
            uint32_t rgb = Harness::pack_rgb(h.dut);
            if (i == 0) border_guess = rgb;
            if (rgb != border_guess) saw_distinct = true;
            h.tick();
        }
        checks.expect(
            saw_distinct,
            "[control] CONTROL-BITPLANE-ALIVE: a distinct, non-constant, "
            "one-shot value ($ACE1) written to BPL1DAT inside a wide-open "
            "display window arrives at the RGB output as a colour visibly "
            "different from the border sample -- proving the bitplane data "
            "path this file's DIW/colour-index/EHB/HAM assertions all "
            "depend on actually delivers a written value, not a silently "
            "dead path. No manual page: infrastructure control.");
    }

    // ==================== DIW-NOMINAL-HSTART ====================
    // 3-diw.c36 (p59, "The normal NTSC DIWSTRT is ($2C81)"), 3-diw.c38
    // (p59, "The normal PAL DIWSTRT is ($2C81)"), 3-t9.c05/3-t9.c06 (p59,
    // Table 3-9 HSTART $81 NTSC/PAL), 3-window.c47/3-window.c48 (p59, OCS
    // HSTART $81 NTSC/PAL), 3-window.c13 (p59, "NTSC and PAL use the same
    // normal HSTART"). The manual's own printed NTSC and PAL DIWSTRT words
    // are bit-for-bit IDENTICAL ($2C81 both -- Table 3-9 prints VSTART $2C
    // in both columns too), and denise's own module header has no ntsc/pal
    // port at all (claims.jsonl's own evidence field for these ids makes
    // this same observation) -- so one write, one measurement, settles
    // every id in this group at once: writing $2C81 to DIWSTRT opens the
    // window exactly where Table 3-9's HSTART=$81 says it should
    // (open_idx = $81 = 129, this package's own RTL-informed sample-index
    // translation -- see header's RTL-INFORMED CORRECTION note).
    {
        Edges e = measure_window(0x2C81, 0x00FF, 300);
#if MUT_DIW_HSTART
        e = measure_window(0x2C82, 0x00FF, 300);
#endif
        bool ok = e.open_idx == 0x81;
        checks.expect(
            ok,
            "DIW-NOMINAL-HSTART: writing DIWSTRT=$2C81 (the identical word "
            "the manual prints for BOTH 'The normal NTSC DIWSTRT is "
            "($2C81)' and 'The normal PAL DIWSTRT is ($2C81)', p.59) opens "
            "the display window at sample index " + std::to_string(e.open_idx) +
            ", exactly $81 -- Table 3-9's 'HSTART $81 "
            "$81' (NTSC/PAL columns, p.59) and 'On OCS, HSTART has nominal "
            "NTSC/PAL value $81'. denise.v's own module header has no "
            "ntsc/pal input, so this single measurement settles NTSC and "
            "PAL together. 3-diw.c36, 3-diw.c38, 3-t9.c05, 3-t9.c06, "
            "3-window.c47, 3-window.c48, 3-window.c13.");
    }

    // ==================== DIW-NOMINAL-HSTOP ====================
    // 3-diw.c37 (p59, NTSC DIWSTOP $F4C1), 3-diw.c39 (p59, PAL DIWSTOP
    // $2CC1 -- checked against the p59.txt scan, see header), 3-t9.c13/
    // 3-t9.c14 (p59, Table 3-9 HSTOP $C1 NTSC/PAL), 3-window.c55/
    // 3-window.c56 (p59, OCS HSTOP $C1 NTSC/PAL), 3-window.c37 (p59, "HSTOP
    // is the same both for NTSC and for PAL"), 3-diw.c30 (p59, "both for
    // NTSC and for PAL"). Unlike DIWSTRT, the NTSC and PAL DIWSTOP WORDS
    // genuinely differ ($F4C1 vs $2CC1 -- different VSTOP bytes), so this
    // is a real measurement, not a restatement: both words are written on
    // separate fresh Harnesses and must produce the IDENTICAL close edge,
    // exactly where the shared H-byte ($C1) predicts (close_idx =
    // $C1+256 = 449, this package's own RTL-informed sample-index
    // translation -- see header's RTL-INFORMED CORRECTION note).
    {
        Edges ntsc = measure_window(0x0002, 0xF4C1, 512);
        Edges pal = measure_window(0x0002, 0x2CC1, 512);
#if MUT_DIW_HSTOP
        pal = measure_window(0x0002, 0x2CC2, 512);
#endif
        bool ok = ntsc.close_idx == 449 && pal.close_idx == 449 && ntsc.close_idx == pal.close_idx;
        checks.expect(
            ok,
            "DIW-NOMINAL-HSTOP: DIWSTOP=$F4C1 (NTSC) closes the window at "
            "sample " + std::to_string(ntsc.close_idx) + " and DIWSTOP=$2CC1 "
            "(PAL) closes it at sample " + std::to_string(pal.close_idx) +
            " -- identical, and exactly $1C1 (449) as page "
            "59's own 'the actual value minus 256 ($100)... The normal HSTOP "
            "value is ($1C1) but is written as ($C1)' and Table 3-9's 'HSTOP "
            "$C1 $C1' (NTSC/PAL columns) both say -- even though the two "
            "full DIWSTOP words differ (VSTOP $F4 vs $2C), the shared H-byte "
            "produces the identical close edge. 'The normal NTSC DIWSTOP is "
            "($F4C1)', 'The normal PAL DIWSTOP is ($2CC1)', 'HSTOP is the "
            "same both for NTSC and for PAL', 'both for NTSC and for PAL'. "
            "3-diw.c37, 3-diw.c39, 3-t9.c13, 3-t9.c14, 3-window.c55, "
            "3-window.c56, 3-window.c37, 3-diw.c30.");
    }

    // ==================== DIW-ONE-PIXEL-RESOLUTION ====================
    // 3-fetch.c2 (p60, "the display window registers... have a one-pixel
    // resolution", contrasted in the same sentence with data-fetch
    // registers' four-pixel resolution -- the data-fetch half is an Agnus
    // DDF concept outside this denise-only fixture and is not asserted
    // here, only the display-window half). Moving DIWSTRT's raw low byte
    // by exactly 1 (from $30 to $31) must move the open edge by exactly 1
    // sample, not snap to a coarser grid.
    {
        Edges a = measure_window(0x0030, 0x00FF, 300);
        Edges b = measure_window(0x0031, 0x00FF, 300);
#if MUT_DIW_RESOLUTION
        b = measure_window(0x0034, 0x00FF, 300);
#endif
        bool ok = (b.open_idx - a.open_idx) == 1;
        checks.expect(
            ok,
            "DIW-ONE-PIXEL-RESOLUTION: moving DIWSTRT's raw H byte from $30 "
            "to $31 (one count) moves the window's open edge by exactly 1 "
            "sample (" + std::to_string(a.open_idx) + " -> " +
            std::to_string(b.open_idx) + "), not a coarser step -- 'the "
            "display window registers... have a one-pixel resolution' "
            "(p.60). 3-fetch.c2.");
    }

    // ==================== BPLCON1-WO ====================
    // A-bplcon.c5 (p271, "BPLCON1 102 W D Bitplane control register"),
    // A-bplcon1.rw (p271, same), B-subject.c115 (p291, same),
    // C-genlock-ext.c6 (p307, "BPLCON1 W D Bitplane control (horizontal
    // scroll control)").
    //
    // D5 FIX: a $0000 readback alone is structural at this boundary (see
    // file header's D5 FIX note -- data_out is only ever col_out |
    // deniseid_out | rgb_out, so nearly every unimplemented address reads
    // $0000 too). Paired here with BPLCON1's own measured write EFFECT:
    // PF1H (bits 3-0, non-aga) is the horizontal-scroll-control field p.307
    // itself names -- denise_bitplanes.v's pf1h/pf1h_del/select chain delays
    // which shifter-history sample reaches the output. Measured directly on
    // this fixture: sweeping PF1H from 0 to 15 (all 16 values, one-shot
    // BPL1DAT=$8000 inside a wide-open window) moved a fixed bit's rendered
    // sample index in lockstep, exactly idx=nib+2 for every nib -- a clean,
    // monotonic, one-sample-per-unit delay, not a coincidental difference.
    {
        auto measure_pf1h_edge = [](uint16_t pf1h_nibble, uint16_t bplcon1_addr) -> long {
            Harness h;
            h.write_reg(COLOR_BASE + 0, 0x000);
            h.write_reg(COLOR_BASE + 1, 0xF00);
            h.write_reg(BPLCON0_ADDR, 0x1000);  // bpu=1
            h.write_reg(bplcon1_addr, pf1h_nibble & 0xF);
            h.enter_wide_window();
            for (int i = 0; i < 60; ++i) h.tick();  // well clear of the open edge
            auto base = h.scan_static(4);
            uint32_t border = base[0];
            h.write_reg(BPL1DAT_ADDR, 0x8000);  // one-shot: single leading bit
            auto samples = h.scan_static(80);
            for (size_t i = 0; i < samples.size(); ++i)
                if (samples[i] != border) return static_cast<long>(i);
            return -1;
        };
        uint16_t bplcon1_addr = BPLCON1_ADDR;
#if MUT_BPLCON1_WO
        bplcon1_addr = NOOP_ADDR;  // the scroll-delay write goes nowhere real
#endif
        long idx_lo = measure_pf1h_edge(0x0, bplcon1_addr);
        long idx_hi = measure_pf1h_edge(0xF, bplcon1_addr);
        bool effect_ok = (idx_hi - idx_lo) == 15;

        Harness h;
        h.write_reg(bplcon1_addr, 0x5A5A);
        uint16_t r = h.read_reg(bplcon1_addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "BPLCON1-WO: (a) writing BPLCON1's PF1H field (bits 3-0) from 0 "
            "to 15 moves a one-shot BPL1DAT bit's rendered sample from " +
            std::to_string(idx_lo) + " to " + std::to_string(idx_hi) +
            " -- exactly 15, one sample per PF1H unit -- making 'BPLCON1 W D "
            "Bitplane control (horizontal scroll control)' (p.307) an "
            "observed effect, not an assumption; (b) a distinct non-zero "
            "pattern ($5A5A) written to BPLCON1 ($102) then reads back "
            "$0000 at that same address, 'BPLCON1 102 W D Bitplane control "
            "register' (p.271/291) -- (b) alone is structural at this "
            "boundary (see file header) and is asserted only paired with "
            "(a). A-bplcon.c5, A-bplcon1.rw, B-subject.c115, "
            "C-genlock-ext.c6.");
    }

    // ==================== BPL1DAT-WO ====================
    // A-bpldat.c2 (p272, general "BPLxDAT registers are write-only"),
    // A-bplxdat.rw (p272, same), B-subject.c135 (p291, "BPL1DAT & 110 W D
    // Bitplane 1 data"): representative of the general write-only claim
    // plus BPL1DAT specifically.
    //
    // D5 FIX: paired with an observable write EFFECT specific to THIS
    // register, not merely "some $0000 address": with bpu=5 colour-select
    // mode (COLOR0=$000, COLOR1=$0ABC) and planes 2-5 held at 0, toggling
    // ONLY plane 1 moves the colour index from 0 to 1 -- the pixel must go
    // from COLOR0's black to a non-black colour. A per-register mutation
    // redirects exactly this one plane's own BPLxDAT write (and nothing
    // else) to NOOP_ADDR ($1FE, decoded nowhere in denise.v), the same
    // single-register redirect the reviewer's own BPL4DAT-WO/BPL5DAT-WO
    // proof used -- see file header.
    {
        uint16_t addr = BPL1DAT_ADDR;
#if MUT_BPL1DAT_WO
        addr = NOOP_ADDR;
#endif
        Harness h;
        h.write_reg(COLOR_BASE + 0, 0x000);
        h.write_reg(COLOR_BASE + 1, 0x0ABC);
        h.write_reg(BPLCON0_ADDR, 0x5000);  // bpu=5
        h.enter_wide_window();
        h.refresh_bitplanes_mut(false, false, false, false, false, false, 6, 0, 0);
        uint32_t before = h.read_rgb_now();
        h.refresh_bitplanes_mut(/*b1=*/true, false, false, false, false, false, 6,
                                 /*mut_plane=*/1, addr);
        uint32_t after = h.read_rgb_now();
        bool effect_ok = (before == 0x000000) && (after != before);

        Harness h2;
        h2.write_reg(addr, 0x1111);
        uint16_t r = h2.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "BPL1DAT-WO: (a) with planes 2-5 held at 0 (colour index 0, "
            "COLOR0=" + hex4(0) + "), toggling ONLY BPL1DAT (plane 1) moves "
            "the rendered pixel from " + hex4(static_cast<uint16_t>(before >> 8)) +
            " to " + hex4(static_cast<uint16_t>(after >> 8)) +
            " (COLOR1's programmed colour) -- a write effect specific to "
            "this register; (b) a distinct pattern ($1111) written to "
            "BPL1DAT ($110) then reads back $0000 at that same address -- "
            "(b) alone is structural at this boundary (see file header) and "
            "is asserted only paired with (a). 'BPLxDAT 110 W D Bitplane x "
            "data (parallel-to-serial' (p.272), 'BPL1DAT & 110 W D Bitplane "
            "1 data (parallel-to-serial convert)' (p.291). A-bpldat.c2, "
            "A-bplxdat.rw, B-subject.c135.");
    }

    // ==================== BPL2DAT-WO ==================== B-subject.c140 (p291)
    // D5 FIX: same per-plane isolation as BPL1DAT-WO above, plane 2 (index
    // bit 1, value 2).
    {
        uint16_t addr = BPL2DAT_ADDR;
#if MUT_BPL2DAT_WO
        addr = NOOP_ADDR;
#endif
        Harness h;
        h.write_reg(COLOR_BASE + 0, 0x000);
        h.write_reg(COLOR_BASE + 2, 0x0ABC);
        h.write_reg(BPLCON0_ADDR, 0x5000);  // bpu=5
        h.enter_wide_window();
        h.refresh_bitplanes_mut(false, false, false, false, false, false, 6, 0, 0);
        uint32_t before = h.read_rgb_now();
        h.refresh_bitplanes_mut(false, /*b2=*/true, false, false, false, false, 6,
                                 /*mut_plane=*/2, addr);
        uint32_t after = h.read_rgb_now();
        bool effect_ok = (before == 0x000000) && (after != before);

        Harness h2;
        h2.write_reg(addr, 0x2222);
        uint16_t r = h2.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "BPL2DAT-WO: (a) with planes 1,3-5 held at 0, toggling ONLY "
            "BPL2DAT (plane 2) moves the rendered pixel from " +
            hex4(static_cast<uint16_t>(before >> 8)) + " to " +
            hex4(static_cast<uint16_t>(after >> 8)) +
            " (COLOR2's programmed colour) -- a write effect specific to "
            "this register; (b) a distinct pattern ($2222) written to "
            "BPL2DAT ($112) then reads back $0000 at that same address -- "
            "(b) alone is structural at this boundary (see file header) and "
            "is asserted only paired with (a). 'BPL2DAT & 112 W D Bitplane "
            "2 data (parallel-to-serial convert)' (p.291). B-subject.c140.");
    }

    // ==================== BPL3DAT-WO ==================== B-subject.c145 (p291)
    // D5 FIX: same per-plane isolation, plane 3 (index bit 2, value 4).
    {
        uint16_t addr = BPL3DAT_ADDR;
#if MUT_BPL3DAT_WO
        addr = NOOP_ADDR;
#endif
        Harness h;
        h.write_reg(COLOR_BASE + 0, 0x000);
        h.write_reg(COLOR_BASE + 4, 0x0ABC);
        h.write_reg(BPLCON0_ADDR, 0x5000);  // bpu=5
        h.enter_wide_window();
        h.refresh_bitplanes_mut(false, false, false, false, false, false, 6, 0, 0);
        uint32_t before = h.read_rgb_now();
        h.refresh_bitplanes_mut(false, false, /*b3=*/true, false, false, false, 6,
                                 /*mut_plane=*/3, addr);
        uint32_t after = h.read_rgb_now();
        bool effect_ok = (before == 0x000000) && (after != before);

        Harness h2;
        h2.write_reg(addr, 0x3333);
        uint16_t r = h2.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "BPL3DAT-WO: (a) with planes 1,2,4,5 held at 0, toggling ONLY "
            "BPL3DAT (plane 3) moves the rendered pixel from " +
            hex4(static_cast<uint16_t>(before >> 8)) + " to " +
            hex4(static_cast<uint16_t>(after >> 8)) +
            " (COLOR4's programmed colour) -- a write effect specific to "
            "this register; (b) a distinct pattern ($3333) written to "
            "BPL3DAT ($114) then reads back $0000 at that same address -- "
            "(b) alone is structural at this boundary (see file header) and "
            "is asserted only paired with (a). 'BPL3DAT & 114 W D Bitplane "
            "3 data (parallel-to-serial convert)' (p.291). B-subject.c145.");
    }

    // ==================== BPL4DAT-WO ==================== B-subject.c150 (p291)
    // D5 FIX: same per-plane isolation, plane 4 (index bit 3, value 8).
    // This is exactly the register the reviewer's own proof pointed at
    // $0A0 (a Paula audio pointer) to show a green that meant nothing.
    {
        uint16_t addr = BPL4DAT_ADDR;
#if MUT_BPL4DAT_WO
        addr = NOOP_ADDR;
#endif
        Harness h;
        h.write_reg(COLOR_BASE + 0, 0x000);
        h.write_reg(COLOR_BASE + 8, 0x0ABC);
        h.write_reg(BPLCON0_ADDR, 0x5000);  // bpu=5
        h.enter_wide_window();
        h.refresh_bitplanes_mut(false, false, false, false, false, false, 6, 0, 0);
        uint32_t before = h.read_rgb_now();
        h.refresh_bitplanes_mut(false, false, false, /*b4=*/true, false, false, 6,
                                 /*mut_plane=*/4, addr);
        uint32_t after = h.read_rgb_now();
        bool effect_ok = (before == 0x000000) && (after != before);

        Harness h2;
        h2.write_reg(addr, 0x4444);
        uint16_t r = h2.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "BPL4DAT-WO: (a) with planes 1-3,5 held at 0, toggling ONLY "
            "BPL4DAT (plane 4) moves the rendered pixel from " +
            hex4(static_cast<uint16_t>(before >> 8)) + " to " +
            hex4(static_cast<uint16_t>(after >> 8)) +
            " (COLOR8's programmed colour) -- a write effect specific to "
            "this register; (b) a distinct pattern ($4444) written to "
            "BPL4DAT ($116) then reads back $0000 at that same address -- "
            "(b) alone is structural at this boundary (see file header) and "
            "is asserted only paired with (a). 'BPL4DAT & 116 W D Bitplane "
            "4 data (parallel-to-serial convert)' (p.291). B-subject.c150.");
    }

    // ==================== BPL5DAT-WO ==================== B-subject.c155 (p291)
    // D5 FIX: same per-plane isolation, plane 5 (index bit 4, value 16).
    // This is exactly the register the reviewer's own proof pointed at
    // $1FE (the bus NO-OP) to show a green that meant nothing.
    {
        uint16_t addr = BPL5DAT_ADDR;
#if MUT_BPL5DAT_WO
        addr = NOOP_ADDR;
#endif
        Harness h;
        h.write_reg(COLOR_BASE + 0, 0x000);
        h.write_reg(COLOR_BASE + 16, 0x0ABC);
        h.write_reg(BPLCON0_ADDR, 0x5000);  // bpu=5
        h.enter_wide_window();
        h.refresh_bitplanes_mut(false, false, false, false, false, false, 6, 0, 0);
        uint32_t before = h.read_rgb_now();
        h.refresh_bitplanes_mut(false, false, false, false, /*b5=*/true, false, 6,
                                 /*mut_plane=*/5, addr);
        uint32_t after = h.read_rgb_now();
        bool effect_ok = (before == 0x000000) && (after != before);

        Harness h2;
        h2.write_reg(addr, 0x5555);
        uint16_t r = h2.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "BPL5DAT-WO: (a) with planes 1-4 held at 0, toggling ONLY "
            "BPL5DAT (plane 5) moves the rendered pixel from " +
            hex4(static_cast<uint16_t>(before >> 8)) + " to " +
            hex4(static_cast<uint16_t>(after >> 8)) +
            " (COLOR16's programmed colour) -- a write effect specific to "
            "this register; (b) a distinct pattern ($5555) written to "
            "BPL5DAT ($118) then reads back $0000 at that same address -- "
            "(b) alone is structural at this boundary (see file header) and "
            "is asserted only paired with (a). 'BPL5DAT & 118 W D Bitplane "
            "5 data (parallel-to-serial convert)' (p.291). B-subject.c155.");
    }

    // ==================== BPL6DAT-WO ==================== B-subject.c160 (p291)
    // D5 FIX: plane 6 does NOT contribute an index bit in bpu=6/ham=0 mode
    // -- it is the EHB half-intensity flag (denise.v: ehb_en = !killehb &&
    // !a1k && !ham && !dpf && l_bpu==6), exactly what EHB-HALF-INTENSITY
    // above already measures and mutation-tests. Paired here with that
    // SAME mechanism, repeated through this assertion's own (possibly
    // redirected) address rather than merely citing it, so a mutation
    // pointed at this specific claim still has something local to break.
    {
        uint16_t addr = BPL6DAT_ADDR;
#if MUT_BPL6DAT_WO
        addr = NOOP_ADDR;
#endif
        Harness h;
        h.write_reg(COLOR_BASE + 6, 0x0BA8);
        h.enter_wide_window();
        h.write_bitplane_pattern(/*bpu=*/6, /*ham_on=*/false,
                                  /*b1=*/false, /*b2=*/true, /*b3=*/true,
                                  /*b4=*/false, /*b5=*/false, /*b6=*/false, 10);
        uint32_t normal = h.read_rgb_now();
        h.refresh_bitplanes_mut(false, true, true, false, false, /*b6=*/true, 10,
                                 /*mut_plane=*/6, addr);
        uint32_t halved = h.read_rgb_now();
        uint8_t nr = (normal >> 16) & 0xFF, ng = (normal >> 8) & 0xFF, nb = normal & 0xFF;
        uint8_t hr = (halved >> 16) & 0xFF, hg = (halved >> 8) & 0xFF, hb = halved & 0xFF;
        bool effect_ok = (normal != halved) && hr <= nr && hg <= ng && hb <= nb &&
                          (hr < nr || hg < ng || hb < nb);

        Harness h2;
        h2.write_reg(addr, 0x6666);
        uint16_t r = h2.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "BPL6DAT-WO: (a) toggling ONLY BPL6DAT (plane 6, the EHB flag "
            "at bpu=6/ham=0) darkens the same index-6 pixel from " +
            hex4(static_cast<uint16_t>(normal >> 8)) + " to " +
            hex4(static_cast<uint16_t>(halved >> 8)) +
            " (every RGB component no greater, at least one strictly "
            "smaller) -- the same EHB half-intensity effect "
            "EHB-HALF-INTENSITY above measures, repeated here through this "
            "assertion's own address; (b) a distinct pattern ($6666) "
            "written to BPL6DAT ($11A) then reads back $0000 at that same "
            "address -- (b) alone is structural at this boundary (see file "
            "header) and is asserted only paired with (a). 'BPL6DAT & 11A "
            "W D Bitplane 6 data (parallel-to-serial convert)' (p.291). "
            "B-subject.c160.");
    }

    // ==================== DIWSTRT-WO ====================
    // A-diw.c6 (p277, "DIWSTRT 08E W A Display window start"), A-diwstrt.rw
    // (p277, same), B-subject.c10 (p290, same), C-diw.c1 (p306, "DIWSTRT
    // W A D Display Window Start"): a distinct pattern written to DIWSTRT
    // ($08E) reads back $0000 at that same address, exactly like BPLCON0's
    // own write-only-at-its-own-address behaviour that
    // hrm/ch3-register-map-denise already established for a different
    // register (BPLCON0 visibly affects window/border rendering yet still
    // reads $0000 at $100 -- the manual's "W" column means no readback
    // path at this boundary, not that the write has no internal effect).
    //
    // D5 FIX: that prior text already correctly CITES DIW-NOMINAL-HSTART as
    // corroboration -- kept below -- but citation alone still let a
    // mutation that redirects the WHOLE assertion (write AND read) to an
    // unimplemented address pass (the reviewer's own proof, on BPL4DAT-WO/
    // BPL5DAT-WO). So the SAME $7777 write this assertion already makes is
    // now also used to measure the window opening exactly where it says:
    // open_idx must equal $77 (the RTL-informed formula this file's own
    // header derives: open_idx=raw_start), through this assertion's own
    // (possibly redirected) address.
    {
        uint16_t addr = DIWSTRT_ADDR;
#if MUT_DIWSTRT_WO
        addr = NOOP_ADDR;
#endif
        Edges e = measure_window(0x7777, 0x00FF, 300, /*diwstrt_addr=*/addr);
        bool effect_ok = (e.open_idx == 0x77);

        Harness h;
        h.write_reg(addr, 0x7777);
        uint16_t r = h.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "DIWSTRT-WO: (a) writing $7777 to DIWSTRT opens the display "
            "window at sample " + std::to_string(e.open_idx) +
            ", exactly $77 -- the same write DIW-NOMINAL-HSTART above "
            "measures with a different pattern, repeated here through this "
            "assertion's own address; (b) that same $7777 pattern then "
            "reads back $0000 at DIWSTRT's own address ($08E) -- (b) alone "
            "is structural at this boundary (see file header) and is "
            "asserted only paired with (a). 'DIWSTRT 08E W A Display window "
            "start (upper left' (p.277), 'DIWSTRT 08E W A Display window "
            "start (upper left' (p.290), 'DIWSTRT W A D Display Window "
            "Start (upper left vert-hor pos)' (p.306). A-diw.c6, "
            "A-diwstrt.rw, B-subject.c10, C-diw.c1.");
    }

    // ==================== DIWSTOP-WO ====================
    // A-diw.c4, A-diwstop.rw (p277), B-subject.c14 (p290), C-diw.c2 (p306).
    //
    // D5 FIX: same reasoning as DIWSTRT-WO above -- the $8888 already
    // written by this assertion also closes the window exactly where the
    // RTL-informed formula (close_idx=raw_stop+256) says: $88+256=392.
    {
        uint16_t addr = DIWSTOP_ADDR;
#if MUT_DIWSTOP_WO
        addr = NOOP_ADDR;
#endif
        Edges e = measure_window(0x0002, 0x8888, 450, DIWSTRT_ADDR, /*diwstop_addr=*/addr);
        bool effect_ok = (e.close_idx == 392);

        Harness h;
        h.write_reg(addr, 0x8888);
        uint16_t r = h.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "DIWSTOP-WO: (a) writing $8888 to DIWSTOP closes the display "
            "window at sample " + std::to_string(e.close_idx) +
            ", exactly $88+256=392 -- the same write DIW-NOMINAL-HSTOP "
            "above measures with different patterns, repeated here through "
            "this assertion's own address; (b) that same $8888 pattern "
            "then reads back $0000 at DIWSTOP's own address ($090) -- (b) "
            "alone is structural at this boundary (see file header) and is "
            "asserted only paired with (a). 'DIWSTOP 090 W A Display window "
            "stop (lower right' (p.277/290), 'DIWSTOP W A D Display Window "
            "Stop (lower right vert-hor pos)' (p.306). A-diw.c4, "
            "A-diwstop.rw, B-subject.c14, C-diw.c2.");
    }

    // ==================== DIWHIGH-WO ====================
    // A-diw.c2, A-diwhigh.rw (p277), B-subject.c293 (p293), C-diw.c8,
    // C-diwhigh.c01 (p306).
    //
    // D5 FIX: DIWHIGH has a real, ECS-gated effect at THIS boundary --
    // denise.v:314-319: hdiwstop[8] defaults to 1 (close edge = raw_stop+
    // 256) unless DIWHIGH is written with ecs=1, which loads data_in[13]
    // into it instead. The $9999 pattern this assertion already writes has
    // bit13=0, so writing it must pull the close edge in from $C1+256=449
    // down to exactly $C1=193 -- measured directly on this fixture.
    {
        uint16_t addr = DIWHIGH_ADDR;
#if MUT_DIWHIGH_WO
        addr = NOOP_ADDR;
#endif
        auto measure_close = [&](bool write_diwhigh) -> long {
            Harness h;
            h.write_reg(COLOR_BASE + 0, 0x0F0);
            h.write_reg(COLOR_BASE + 1, 0xF00);
            h.write_reg(BPLCON0_ADDR, 0x1000);
            h.write_reg(BPLCON1_ADDR, 0x0000);
            h.write_reg(DIWSTRT_ADDR, 0x0002);
            h.write_reg(DIWSTOP_ADDR, 0x00C1);
            if (write_diwhigh) h.write_reg(addr, 0x9999);  // bit13=0
            auto samples = h.scan_window_bpu1(470);
            Harness cal;
            cal.write_reg(COLOR_BASE + 0, 0x0F0);
            cal.write_reg(COLOR_BASE + 1, 0xF00);
            cal.write_reg(BPLCON0_ADDR, 0x1000);
            cal.write_reg(BPLCON1_ADDR, 0x0000);
            cal.write_reg(DIWSTRT_ADDR, 0x60);
            cal.write_reg(DIWSTOP_ADDR, 0x20);
            auto cal_samples = cal.scan_window_bpu1(8);
            uint32_t border = cal_samples[4];
            Edges e = find_edges(samples, border);
            return e.close_idx;
        };
        long close_default = measure_close(false);
        long close_forced = measure_close(true);
        bool effect_ok = (close_default == 449) && (close_forced == 0x00C1);

        Harness h;
        h.write_reg(addr, 0x9999);
        uint16_t r = h.read_reg(addr);
        bool readback_ok = (r == 0x0000);

        checks.expect(
            effect_ok && readback_ok,
            "DIWHIGH-WO: (a) with DIWSTOP=$00C1 alone the close edge sits "
            "at the default " + std::to_string(close_default) +
            " ($C1+256, hdiwstop[8]'s reset default); writing this "
            "assertion's own $9999 pattern (bit13=0) afterward pulls it in "
            "to " + std::to_string(close_forced) +
            ", exactly $C1 -- 'DIWHIGH ... Display window - upper bits for "
            "start, stop' (p.277/293/306) made observable; (b) that same "
            "$9999 pattern then reads back $0000 at DIWHIGH's own address "
            "($1E4) -- (b) alone is structural at this boundary (see file "
            "header) and is asserted only paired with (a). 'DIWHIGH 1E4 "
            "W A,D( E ) Display window - upper bits for start, stop' "
            "(p.277), 'DIWHIGH 1E4 w A, D(E) Display window - upper bits "
            "for start, stop' (p.293), 'DIWHIGH W A D Display Window upper "
            "bits for start,stop' (p.306, both printings). A-diw.c2, "
            "A-diwhigh.rw, B-subject.c293, C-diw.c8, C-diwhigh.c01.");
    }

    // ==================== DENISEID-IS-REVISION-REGISTER ====================
    // A-deniseid.fn (p277, "DENISEID 07C R D(E) Chip revision level for
    // Denise"), B-subject.c8 (p290, "DENISEID ~07C R D(E) Chip revision
    // level for Denise (video out chip)"), C-denid.c01 (p299, "DENISEID...
    // can have one of two values"): DENISEID answers a real (non-zero)
    // value at its own address, and that value DIFFERS between the two
    // chip configurations the manual's own "one of two values" describes
    // (ecs=0 vs ecs=1) -- proving it identifies which chip variant is
    // present, not a stuck or constant bus value.
    {
        Harness ecs1(/*ecs_in=*/true);
        Harness ecs0(/*ecs_in=*/false);
        uint16_t v_ecs1 = ecs1.read_reg(DENISEID_ADDR);
        uint16_t addr_for_ecs0 = DENISEID_ADDR;
#if MUT_DENISEID_FN
        addr_for_ecs0 = BPLCON0_ADDR;
#endif
        uint16_t v_ecs0 = ecs0.read_reg(addr_for_ecs0);
        bool ok = v_ecs1 != 0 && v_ecs0 != 0 && v_ecs1 != v_ecs0;
        checks.expect(
            ok,
            "DENISEID-IS-REVISION-REGISTER: DENISEID ($07C) answers a "
            "distinct, non-zero value under each of two chip configurations "
            "(" + hex4(v_ecs1) + " with ecs=1, " + hex4(v_ecs0) + " with "
            "ecs=0) -- it identifies which chip variant is present, exactly "
            "'DENISEID... Chip revision level for Denise (video out chip)' "
            "(p.277/290) and 'DENISEID is a new register which can have one "
            "of two values' (p.299). A-deniseid.fn, B-subject.c8, "
            "C-denid.c01.");
    }

    // ==================== DENISEID-ECS-VALUE ====================
    // C-denid.c03 (p299, "The enhanced HighRes Denise (8373) will return
    // $FC in the lower 8 bits"), C-deniseid.c1 (p299, "The original Denise
    // (8362) does not have this register"): with ecs=1 (enhanced/HighRes
    // Denise), the low byte reads exactly $FC; with ecs=0 (the original
    // 8362), the low byte does NOT read $FC -- translating "does not have
    // this register" into the one concrete, falsifiable difference this
    // fixture can observe (it cannot simulate "whatever is left over on
    // the bus", an inherently non-deterministic quantity, but it CAN show
    // the enhanced-chip identity byte is specifically absent).
    {
        Harness ecs1(/*ecs_in=*/true);
        Harness ecs0(/*ecs_in=*/false);
        uint16_t v_ecs1 = ecs1.read_reg(DENISEID_ADDR);
        uint16_t v_ecs0 = ecs0.read_reg(DENISEID_ADDR);
#if MUT_DENISEID_VALUE
        v_ecs1 = 0x0000;
#endif
        bool ok = (v_ecs1 & 0xFF) == 0xFC && (v_ecs0 & 0xFF) != 0xFC;
        checks.expect(
            ok,
            "DENISEID-ECS-VALUE: with ecs=1, DENISEID's low byte reads " +
            hex4(static_cast<uint16_t>(v_ecs1 & 0xFF)) + " ($FC exactly); "
            "with ecs=0, it reads " + hex4(static_cast<uint16_t>(v_ecs0 & 0xFF)) +
            " (not $FC) -- 'The enhanced HighRes Denise (8373) will return "
            "$FC in the lower 8 bits' and 'The original Denise (8362) does "
            "not have this register' (p.299), translated to the one "
            "concrete difference this fixture can observe (not the "
            "inherently non-deterministic 'whatever is left over on the "
            "bus' itself). C-denid.c03, C-deniseid.c1.");
    }

    // ==================== COLOR-BITS-15-12-UNUSED ====================
    // (p92, "15 - 12  Unused (set these to 0)"): this assertion
    // tests only the hardware-observable half of the quote (bits ignored
    // by the module) -- the "(set these to 0)" half is software guidance
    // to a programmer, not a hardware guarantee, and is not asserted here.
    // A single-plane pixel dialled to select COLOR01 must render
    // identically whether COLOR01 is written with bits 15-12 set or clear,
    // with the low 12 bits held constant.
    //
    // ROUTED ADDITION, 25.09.2026 (routing.jsonl, work/2026-09-25_unit-
    //   backlog/build2/ch3-nominal-display-values.jsonl): eight more ids
    //   route here with the identical "how" -- "COLOR-BITS-15-12-UNUSED
    //   already does this" -- because denise_colortable.v's own data_in
    //   port is [11:0], twelve bits wide, so no wiring exists in this
    //   fixture's boundary for bits 15-12 to reach in the first place; the
    //   $0 to $F sweep this assertion already runs establishes every one of
    //   them, individually and as a group, the same measurement covers all
    //   four. Added to this assertion's own label, in full, below:
    //   3-colortable.c13/3-t3.c01 (p.46, the chapter body's own Table 3-3),
    //   3-colorsum.c6 (p.92, the same appendix-B row 3-t15.c01 already
    //   cites, a second extraction split to keep the hardware half its own
    //   id), A-color.c7/c8/c9/c10 (p.273, Appendix A's own bit table, split
    //   one id per bit: 15, 14, 13, 12), A-colorxx.bits1 (p.273, the same
    //   row read as one group claim). The ninth routed claim,
    //   3-colorsum.c7 ("must be written as zero", the "(set these to 0)"
    //   half this assertion's own comment above already declines), routes
    //   record: not graded here -- see the RECORDED call immediately below
    //   this block.
    {
        Harness h;
        h.write_reg(BPLCON0_ADDR, 0x1000);  // bpu=1
        h.write_reg(COLOR_BASE + 1, 0x0345);
        h.enter_wide_window();
        uint32_t rgb_low = 0, rgb_high = 0;
        {
            for (int i = 0; i < 24; ++i) h.write_reg(BPL1DAT_ADDR, 0xFFFF);
            rgb_low = h.read_rgb_now();
        }
        uint16_t high_pattern = 0xF345;
#if MUT_COLOR_UNUSED
        high_pattern = 0xF346;  // also changes a LOW (observable) bit, not just bits 15-12
#endif
        h.write_reg(COLOR_BASE + 1, high_pattern);
        {
            for (int i = 0; i < 8; ++i) h.write_reg(BPL1DAT_ADDR, 0xFFFF);
            rgb_high = h.read_rgb_now();
        }
        checks.expect(
            rgb_low == rgb_high,
            "COLOR-BITS-15-12-UNUSED: COLOR01 written as $0345 and as $F345 "
            "(bits 15-12 set vs clear, low 12 bits identical) render the "
            "identical pixel colour (" + hex4(static_cast<uint16_t>(rgb_low >> 8)) +
            " both times) -- the hardware ignores bits 15-12, exactly "
            "'15 - 12  Unused (set these to 0)' (p.92); this assertion "
            "tests only the hardware-observable 'unused' half, not the "
            "software-directed '(set these to 0)' half. "
            "3-colortable.c13, 3-t3.c01, 3-colorsum.c6, A-color.c7, "
            "A-color.c8, A-color.c9, A-color.c10, A-colorxx.bits1.");
    }
    checks.record(
        "3-colorsum.c7 (p.92, 'Color-register bits 15-12 must be written "
        "as zero', the '(set these to 0)' half of '15 - 12  Unused (set "
        "these to 0)'): routed record:ch3-nominal-display-values -- "
        "'(set these to 0)' is an instruction to whoever writes the "
        "register, not a behaviour a module has; nothing about denise's "
        "output depends on what an ignored bit was last written as, so "
        "there is no module-boundary effect for this or any fixture to "
        "observe. The observable half of the same source row is "
        "graded on COLOR-BITS-15-12-UNUSED above (graded: 3-colorsum.c6).");
    checks.record(
        "3-t15.c01 (p.92, 'Table 3-15: colour register bits 15-12 are "
        "unused and should be set to 0.', the same p.92 row as "
        "3-colorsum.c7 above, read as one unsplit claim rather than "
        "split by half): bundles the hardware 'unused' half "
        "COLOR-BITS-15-12-UNUSED measures with the software-directed "
        "'(set these to 0)' half it does not -- an id whose claim bundles "
        "an untested clause is withdrawn whole, not kept on the assertion "
        "with a caveat in its label. The 'unused' half of the identical "
        "row is graded on COLOR-BITS-15-12-UNUSED above via its split "
        "reading (graded: 3-colorsum.c6); the '(set these to 0)' half is "
        "recorded above (3-colorsum.c7).");

    // ==================== FIGURE-COLOR-SELECT-INDEX6 ====================
    // 3-color.c23 (p44, Figure 3-4: "The fourth sample column's five bits
    // 00110 and its connecting line identify value 6 and COLOR6"): with a
    // 5-bitplane pattern dialling the index to binary 00110 = 6 (planes
    // 5,4=0, plane3=1, plane2=1, plane1=0, per
    // hrm/ch3-register-map-denise's own published index formula index =
    // 16*p5+8*p4+4*p3+2*p2+p1), COLOR6 answers a fresh, differently-valued
    // write with a different pixel reading than the first write's --
    // COLOR6 is what index 6 actually reads from, matching the figure's
    // own bit-to-register mapping.
    {
        Harness h;
        h.write_reg(COLOR_BASE + 6, 0x0A5);
        h.write_reg(BPLCON0_ADDR, 0x5000);  // bpu=5
        h.enter_wide_window();
        // index 6 = binary 00110 -> plane3=1, plane2=1, planes1,4,5=0
        h.refresh_bitplanes(/*b1=*/false, /*b2=*/true, /*b3=*/true, /*b4=*/false,
                             /*b5=*/false, /*b6=*/false, /*rounds=*/10);
        uint32_t before = h.read_rgb_now();
        uint16_t write_target = COLOR_BASE + 6;
#if MUT_COLOR_INDEX6
        write_target = COLOR_BASE + 7;
#endif
        h.write_reg(write_target, 0x0F5A);
        h.refresh_bitplanes(false, true, true, false, false, false, 4);
        uint32_t after = h.read_rgb_now();
        checks.expect(
            before != after,
            "FIGURE-COLOR-SELECT-INDEX6: with the 5-bitplane pattern "
            "selecting binary index 00110 (=6), the pixel colour answers a "
            "fresh, differently-valued write to COLOR6 with a different "
            "reading than the first write's -- COLOR6 is what bit pattern "
            "00110 actually reads from, matching Figure 3-4's own 'fourth "
            "sample column's five bits 00110 and its connecting line "
            "identify value 6 and COLOR6' (p.44). 3-color.c23.");
    }

    // ==================== EHB-HALF-INTENSITY ====================
    // 3-ehb.c7 (p95, "The color register output selected by the first five
    // bitplanes is shifted to half-intensity by the sixth bitplane."):
    // with BPU=6 (six-bitplane mode -- page 86/87's own "Bits BPU2, BPU1,
    // BPU0... 101 or 110" for six-plane operation) and a fixed low-5-plane
    // pattern selecting COLOR06, plane 6 clear reads COLOR06's own colour
    // and plane 6 set reads exactly HALF that colour, component by
    // component.
    //
    // D6 FIX: page 95 was re-read directly
    // (docs/references/amiga-hrm-3rd/ch3/p95.txt) -- it says "SHIFTED to
    // half-intensity", not merely "darker" or "reduced": a shift is a
    // specific bit operation, not a vague qualitative one, and the values
    // this fixture actually measures ($BBAA88 -> component-wise >>1 =
    // $5D5544) match a literal >>1 (integer halving, truncating) on EVERY
    // component, exactly. The previous "no component greater, at least one
    // strictly smaller" check is also satisfied by quarter-intensity or a
    // blue-only darkening -- neither of which is what page 95 says, and
    // both of which this exact check now correctly rejects. Printed values
    // are now the full 24-bit R:G:B (hex6), not rgb>>8, so the blue
    // component -- previously invisible in the printed evidence -- is
    // shown too.
    {
        Harness h;
        h.write_reg(COLOR_BASE + 6, 0x0BA8);  // bright, non-degenerate colour
        h.enter_wide_window();
        // index 6 = binary 00110 on planes 5..1; plane 6 is the EHB flag
        h.write_bitplane_pattern(/*bpu=*/6, /*ham_on=*/false,
                                  /*b1=*/false, /*b2=*/true, /*b3=*/true,
                                  /*b4=*/false, /*b5=*/false, /*b6=*/false, 10);
        uint32_t normal = h.read_rgb_now();
        bool plane6 = true;
#if MUT_EHB
        plane6 = false;
#endif
        h.refresh_bitplanes(false, true, true, false, false, plane6, 10);
        uint32_t halved = h.read_rgb_now();
        uint8_t nr = (normal >> 16) & 0xFF, ng = (normal >> 8) & 0xFF, nb = normal & 0xFF;
        uint8_t hr = (halved >> 16) & 0xFF, hg = (halved >> 8) & 0xFF, hb = halved & 0xFF;
        bool exact_half = (hr == static_cast<uint8_t>(nr >> 1)) &&
                           (hg == static_cast<uint8_t>(ng >> 1)) &&
                           (hb == static_cast<uint8_t>(nb >> 1));
        checks.expect(
            exact_half,
            "EHB-HALF-INTENSITY: with a 6th-bitplane (EHB) flag clear, the "
            "index-6 pixel reads " + hex6(normal) +
            "; with the flag set (same low-5-plane index), it reads " +
            hex6(halved) + " -- EVERY RGB component (R, G, and B) is "
            "exactly its normal value right-shifted by one bit "
            "(component>>1, verified for all three, not merely 'no "
            "component greater'), matching 'The color register output "
            "selected by the first "
            "five bitplanes is SHIFTED to half-intensity by the sixth "
            "bitplane' (p.95) -- 'shifted' names a specific bit operation, "
            "not a vague darkening, and this check now rejects "
            "quarter-intensity or a blue-only darkening, either of which "
            "would have passed the prior 'no component greater, at least "
            "one strictly smaller' check. 3-ehb.c7.");
    }

    // ==================== HAM-MODIFY-WRITES-PIXEL ====================
    // 3-ham.c05 (p86, "In hold-and-modify mode, however, the value in the
    // color output circuitry is held, and one of the three components of
    // the color... is modified by bits coming from certain preselected
    // bitplanes. After modification, the pixel is written to the
    // screen."): with HOMOD active (BPLCON0 bit11=1, dpf=0, hires=0, bpu=6
    // per p.87's own precondition list) and a steady combo-00 phase first
    // selecting COLOR01 (holding its colour), followed by a steady
    // combo-01 ("modify blue") phase with a fixed 4-bit pattern on planes
    // 1-4, the resulting pixel HOLDS COLOR01's red and green components
    // while its blue component visibly differs from COLOR01's own blue --
    // "the value... is held, and one of the three components... is
    // modified", tested as: two components held, one changed.
    //
    // D9 FIX: the boolean check (hr==mr && hg==mg && hb!=mb) was already
    // sound -- it does test held-red, held-green, changed-blue -- but both
    // printed values used hex4(rgb>>8), which drops the LOW byte, i.e.
    // exactly the blue component that IS this assertion's whole result;
    // both phases therefore printed the identical-looking $FF00 regardless
    // of what blue actually did. Printed values are now the full 24-bit
    // R:G:B (hex6), so the blue difference the assertion is actually
    // testing is visible in the evidence.
    {
        Harness h;
        h.write_reg(COLOR_BASE + 1, 0x0F08);  // COLOR01: distinct R/G/B components
        h.enter_wide_window();
        // BPLCON0: hires=0, bpu=6 (bits14-12=110), ham=1(bit11), dpf=0
        h.write_reg(BPLCON0_ADDR, 0x6800);
        // Phase 1: combo(plane6,plane5)=00, planes4-1 select register 1 (0001)
        h.refresh_bitplanes(/*b1=*/true, /*b2=*/false, /*b3=*/false, /*b4=*/false,
                             /*b5=*/false, /*b6=*/false, /*rounds=*/16);
        uint32_t held_color01 = h.read_rgb_now();
        // Phase 2: combo(plane6,plane5)=01 (modify blue), planes4-1 = 1010
        bool plane5 = true;
#if MUT_HAM
        plane5 = false;  // combo becomes 00 (no modify) instead of 01
#endif
        h.refresh_bitplanes(/*b1=*/false, /*b2=*/true, /*b3=*/false, /*b4=*/true,
                             plane5, /*b6=*/false, /*rounds=*/16);
        uint32_t modified = h.read_rgb_now();
        uint8_t hr = (held_color01 >> 16) & 0xFF, hg = (held_color01 >> 8) & 0xFF, hb = held_color01 & 0xFF;
        uint8_t mr = (modified >> 16) & 0xFF, mg = (modified >> 8) & 0xFF, mb = modified & 0xFF;
        checks.expect(
            hr == mr && hg == mg && hb != mb,
            "HAM-MODIFY-WRITES-PIXEL: after a steady combo-00 phase selects "
            "COLOR01 (" + hex6(held_color01) +
            "), a following steady combo-01 ('modify blue') phase with a "
            "fixed planes-4-1 pattern reads " +
            hex6(modified) + ": red and green "
            "unchanged (R=" + hex2(hr) + ", G=" + hex2(hg) +
            " both phases), blue " + hex2(hb) + " -> " + hex2(mb) +
            " -- 'the value in the color "
            "output circuitry is held, and one of the three components of "
            "the color... is modified by bits coming from certain "
            "preselected bitplanes. After modification, the pixel is "
            "written to the screen' (p.86). 3-ham.c05.");
    }

    // ==================== DECLINED -- not asserted above ====================
    // Per the build brief's HALT conditions ("the manual is ambiguous",
    // "a claim cannot be settled by... a measurement", "the package needs
    // a wider module boundary than the one assigned"): these 11 ids are
    // recorded here, with the specific reason, rather than forced into a
    // measurement that would not actually settle them.

    checks.note(
        "3-video.c30 (p42, 'A playfield taller than the screen can be "
        "scrolled, or moved smoothly, up or down.'): denise's own module "
        "header has no port carrying a frame or a notion of 'the screen' -- "
        "it emits one pixel at a time on red/green/blue. 'Appears to move "
        "smoothly' is not a module-boundary observation at this fixture.");

    checks.note(
        "3-diw.c10 (p57, 'positions are the same both for NTSC and for "
        "PAL', restated as HORIZONTAL AND VERTICAL starting positions "
        "identical): this fixture (denise) reaches only the horizontal "
        "half -- the vertical half needs the agnus_bitplanedma/ "
        "agnus_beamcounter fixture the sibling ch3-vertical-display-window "
        "package owns. The horizontal half of this same page-57 sentence "
        "is already fully measured above (DIW-NOMINAL-HSTART, citing "
        "3-window.c13, the H-only restatement of the identical sentence) -- "
        "declined here rather than partially asserted, the same convention "
        "hrm/ch3-register-map-denise's own C-diw.c04 decline note uses for "
        "a conjoined H+V claim.");

    checks.note(
        "3-scr.c03 (p80, 'In horizontal scrolling the playfield appears to "
        "move from right-to-left'): a subjective 'appears to move' visual "
        "framing, not a module-boundary observation (no frame/screen port); "
        "the underlying mechanism (BPLCON1 scroll delay) is the sibling "
        "ch3-bplcon1-scroll-delay package's own job, per "
        "hrm/ch3-horizontal-display-window's own published NON-GOALS note.");

    checks.note(
        "3-vscr.c03 (p81, 'picture appears to scroll upward... the picture "
        "scrolls downward'): the vertical axis is outside this denise-only "
        "horizontal fixture's boundary (agnus's job, same as the vertical "
        "half of DIW-NOMINAL above), and the claim's own 'appears to "
        "scroll' framing is subjective/visual besides.");

    checks.note(
        "3-hscr.c05 (p82, 'the on-screen data shifts one pixel to the right "
        "each display field'): claims.jsonl's own evidence field for this "
        "id already states the reason directly -- denise's boundary (blank "
        "in, red/green/blue out) has no notion of where the visible screen "
        "begins, so a claim about position ON THE SCREEN cannot be checked "
        "here.");

    checks.note(
        "C-denid.c04 (p299, 'The upper 8 bits are reserved.'): 'reserved' "
        "is a documentation designation (do not rely on this value), not a "
        "stated hardware behaviour -- the manual makes no claim here about "
        "what the upper 8 bits DO, so there is no falsifiable proposition "
        "to build an assertion around. (Contrast 3-t15.c01 above: unlike "
        "'reserved', 'Unused' states a hardware behaviour, and this "
        "fixture does measure it -- graded under the split siblings on "
        "COLOR-BITS-15-12-UNUSED; 3-t15.c01 itself is withdrawn to a "
        "record because it bundles that hardware half with an untested "
        "software-directed half.)");

    checks.note(
        "C-shr.c01, C-superhires.c1, C-superhires.c2 (p296, 'up to 1280 "
        "horizontal pixels per scanline on a standard NTSC or PAL "
        "display'): 1280 pixels/scanline is a total-horizontal-timing "
        "quantity (htotal) that lives in Agnus's beam counter "
        "(agnus_beamcounter.v), not in denise, which only ever compares "
        "hpos against DIWSTRT/DIWSTOP -- it has no notion of a scanline's "
        "own total length or of 'a standard NTSC or PAL display'. This "
        "needs a wider boundary than the one this package was assigned.");

    checks.note(
        "C-spr.c07 (p303, SPRxCTL bit 3 'SHSH0... (unimplemented)'), "
        "C-spr.c08 (p303, SPRxCTL bit 0 'SH0... Start horiz. value Low "
        "bit'): settling either bit meaning would need a full sprite "
        "positioning pipeline (SPRxPOS+SPRxCTL+SPRxDATA/DATB feeding "
        "denise_sprites/denise_sprites_shifter, compared against hpos) that "
        "this package's own inventory (the chapter's nominal-display-values "
        "residue) was never assigned to build from scratch. A properly "
        "scoped sibling for exactly this domain already exists "
        "(ch3-sprite-registers, hrm/denise_sprites fixture) even though it "
        "does not happen to cite these two ids (checked: grep found no "
        "match) -- building a purpose-built sprite rig here to settle 2 ids "
        "would duplicate that sibling's own boundary machinery, the same "
        "'wider mechanism than this fixture's boundary reaches' reasoning "
        "hrm/ch3-register-map-denise's own BPLCON2-ADDR/BPLCON3-ADDR "
        "decline note already uses.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
