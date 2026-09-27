// ch4-sprite-vertical-position -- agnus_spritedma conformance test
//
// Test purpose: the vertical half of Amiga sprite positioning as printed in
//   appendix A (p284) and appendix B (pp287,291,292) -- SPRxPOS's 9-bit
//   start-vertical value (SV7-SV0 plus the SV8 high bit), SPRxCTL's 9-bit
//   stop-vertical value (EV7-EV0 plus the EV8 high bit), the word address
//   each of the 8 sprites' POS/CTL/DATA/DATB registers occupies, which
//   chip(s) the printed "chip" column claims for each register, the "%"
//   (DMA-usually/processor-sometimes) mark every register in this range
//   carries, and the general statement that SPRxPOS and SPRxCTL work
//   together as position/size/feature control registers -- all at the
//   agnus_spritedma module boundary. Package ch4-sprite-vertical- position,
//   fixture agnus_spritedma (one Verilator build, this module only, per this
//   package's own claims.jsonl).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 284, 287, 291, 292 (claims.jsonl for ch4-sprite-vertical-position,
//   178 records). A red result below means agnus_spritedma.v disagrees with
//   the manual, not that the assertion is wrong -- no assertion here was, or
//   should be, adjusted to make the RTL pass.
// Non-goals: anything outside the `agnus_spritedma` fixture this package
//   builds, and any claim it does not name. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: ids such as `CH4-appA-sprite.c5` and `CH4-appA-sprite.c1` name
//   one sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch4.json lists under `inventories`.
//
// FIXTURE BOUNDARY: agnus_spritedma's SPRxCTL write keeps only the
//   vertical fields -- measured during this package's own classification
//   stage (claims.jsonl classify.evidence): "the SPRxCTL write at
//   rtl/agnus_spritedma.v:221 keeps only
//   {data_in[15:8],data_in[6],data_in[5],data_in[2],data_in[1]}", and
//   "toggling SPRxCTL bit 7 (ATT) between 0 and 1 leaves the whole
//   reqdma/reg_address_out/address_out trace ... byte-identical". None of
//   this package's 178 claims requires a horizontal or ATTACH observable
//   (every one is about VSTART/VSTOP, the SV8/EV8 high bits, register
//   addresses, chip-column marks, or the "%" DMA/processor mark), so no
//   claim forces a boundary this fixture cannot deliver -- that finer
//   split (ATT, SH0, the horizontal start bits) belongs to denise_sprites,
//   a different package on a different fixture, not tested here.
//
// SPRxCTL BITS 6 AND 5 -- SILENT IN THE PRINTED SOURCE: this package's
//   own classify.evidence measured that "bits 6 and 5 -- which p284
//   calls unused -- carry the ECS ninth vertical bits, gated by the ecs
//   input" (rtl/agnus_spritedma.v:221/225, :242/244): vstart[9]=bit6,
//   vstop[9]=bit5, alongside the PRINTED SV8=bit2=vstart[8] and
//   EV8=bit1=vstop[8]. No claim in this package's 178 records names bits
//   6/5 that way (APPA-sprctl.c6 and CH4-appA-sprite.c21 print "06-04 X
//   Not used" and are themselves a recorded conflict against appendix C
//   p303's different bit assignment for that span -- see SPRXCTL-BITS-6-
//   4-SPAN-IS-DISPUTED below). Where the corpus is silent, this file
//   measures and reports and does not assert -- so bits
//   6/5 are exercised only by a RECORDED/MEASURED note, never inside a
//   checks.expect() pass/fail condition, and are cited by no claim id.
//
// CONFLICTS: 20 of this package's 178 claims carry a
//   classify.conflict_resolution naming an undisputed sub-claim, in two
//   conflicts, and none of the 20 is graded. The 18 "A D(E)"/"A D"
//   chip-column claims for SPRxCTL are declined whole on
//   AGNUS-IS-WRITE-TARGET-DENISE-HALF-UNTESTED: each bundles the (E)
//   qualifier and the Denise ("D") side, both out of this boundary, into
//   the same sentence as the "A". The undisputed Agnus half -- Agnus is a
//   genuine write target -- is graded by
//   AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL, which carries none of
//   the 18. SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED (2 ids) declines
//   entirely, because the disputed part (what bits 6-3 are) is exactly
//   what an "unused" assertion would have to pick a side on, and the
//   undisputed bits in that same row (SV8 at bit 2, EV8 at bit 1) are
//   already separately asserted elsewhere.
//
// Scope, in detail: The "D" (Denise) half of every "A D"/"A D(E)"
//   chip-column
//   claim, and the "(E)" ECS qualifier specifically, are not tested here
//   -- this fixture has no Denise instance, so only the "A" (Agnus) half
//   of any joint chip-mark claim can be shown. The "feature" component of
//   "SPRxPOS and SPRxCTL work together as position, size and feature
//   sprite-control registers" (p284) is likewise untested -- feature
//   (ATTACH) is a horizontal-boundary observable per the FIXTURE BOUNDARY
//   note above; only the position and size components are asserted.
//   SPRxDATA/SPRxDATB's "%" mark and "usually loaded by DMA... may be
//   loaded by processor" claims are classified `observable: yes` at this
//   boundary, but this package's own classify.evidence records that the
//   two halves of that claim are themselves SPLIT across two modules:
//   "agnus_spritedma does NOT decode $144/$146 for writes at all (:205/
//   220 accept only [2:1]==00/01 -- POS/CTL only); ... a processor write
//   to SPRxDATA/SPRxDATB lands in Denise". So SPRXDATA-SPRXDATB-DMA-PATH-
//   ONLY-AT-THIS-BOUNDARY below asserts only the DMA-emission half (this
//   module requests those addresses on its own); the processor-write half
//   is a non-goal here, not silently assumed. Frequency words themselves
//   ("usually", "sometimes", "normally", "at any time", "during
//   horizontal blank") are never asserted anywhere in this file -- this
//   package's own classify.evidence records, for the "%" mark generally,
//   that "no port carries how often a write happens" and that the DMA
//   slot window is "hpos-gated ..., not blank-gated" -- only the
//   structural fact that both a DMA path and (for POS/CTL) a processor
//   path exist is asserted, never their relative frequency or a precise
//   blanking-window correspondence.
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_spritedma.v, per this package's
//   own claims.jsonl classify.evidence field (the classification
//   stage's own measurement of the RTL): register
//   writes decode at reg_address_in[8:6]==$140[8:6] with [2:1] selecting
//   POS(00)/CTL(01) and [5:3] the sprite channel (0-7), "confirmed by
//   measurement I (eight distinct pointer banks)"; observable ports are
//   `reqdma`, `reg_address_out`, `address_out` (the DMA request/address
//   side) and `reg_address_in`/`data_in` (the direct-write side) -- no
//   data_out for the fetched sprite image exists at this boundary.
//   `vstart`/`vstop`/`sprdmastate` are internal wires with no read-back
//   port; every assertion below observes them only indirectly, through
//   the DMA-active line range they produce on reqdma/reg_address_out.
//
// HARNESS AND STIMULUS SOURCES: the classify.evidence field is the
// classification stage's own measurement of the RTL (the same status as
// a sibling package's own PACKAGE.md evidence field -- the "BOUNDARY"
// section of hrm/ch3-sprite-registers/test_denise_sprites.cpp cites
// denise.v line numbers "per PACKAGE.md"). The Harness's port names,
// clk7_en 4-cycle-per-hpos clocking, reg_address_in/data_in write
// sequencing, the reqdma/ackdma same-cycle-sample-then-grant handshake,
// and the word-address map ($140->0xA0 for SPR0POS, +4 words per sprite)
// are those of hrm/agnus_spritedma/test_agnus_spritedma.cpp, the
// pre-pipeline harness for this SAME module (removed). Three numeric stimulus values
// (SPR0POS=$28/VSTART=40, SPR0CTL=$2C/VSTOP=44, and the second
// SPR0CTL=$2E/VSTOP=46) come from this package's own classify.evidence
// field, which reports them as measured:
// "with SPR0POS=$2800 (VSTART=40) and SPR0CTL=$2C00 (VSTOP=44), reqdma is
// granted twice per line with reg_address_out naming SPR0DATA at hpos 53
// and SPR0DATB at hpos 57 on lines 40-43, and naming SPR0POS/SPR0CTL on
// line 44; with SPR0CTL=$2E00 (VSTOP=46) the identical stimulus carries
// the window through line 45 instead."
//
// MEASUREMENT BASIS: every non-declined assertion below writes SPR0POS
// and/or SPR0CTL directly through reg_address_in/data_in (the same single
// write interface every write in this file uses -- there is no separate
// DMA-tagged write path into this module's own POS/CTL registers) on a
// freshly reset Harness, then sweeps vpos from 0 up to a stated maximum,
// and at every line sweeps the full 454-hpos line granting ackdma
// whenever reqdma is seen, recording the first and one-past-last line at
// which this sprite's own DATA/DATB address (0x144/0x146, +8 per sprite)
// appears on reg_address_out -- "Window::start"/"Window::end". A vpos
// sweep always starts at 0 (never mid-frame) so no measurement depends on
// the DMA sequencer having already been armed by an earlier, unobserved
// transition. SPRXPOS-SPRXCTL-ADDRESS-MAP instead sweeps with `vblend`
// held high (the automatic POS/CTL-prefetch trigger, per the sibling
// harness's own "Line 31 (vblend): POS/CTL fetch") and collects the SET
// of addresses this module requests on its own, unprompted by any
// register write -- proving address existence, not per-sprite
// independence under direct write (which the address/bit-field
// assertions below prove for sprite 0 specifically, and which this
// package's own classify.evidence separately reports as "confirmed by
// measurement I (eight distinct pointer banks)" for the decode logic
// itself).
//
// CLAIM COVERAGE: all 178 records of claims.jsonl are cited below, in
// full, inside an expect() or record() label. Ids restating the identical
// proposition under a different manual wording, page, sprite number (0-7)
// or reader are grouped and cited together on the single assertion that
// tests that proposition, as hrm/ch3-sprite-registers/ and
// hrm/ch3-collision-registers/ both do.
//
//   [control] FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER (0 ids)
//   SPRXPOS-SPRXCTL-ADDRESS-MAP (34 ids)
//   SPRXPOS-BYTE-IS-SV70-VSTART (11 ids)
//   SPRXCTL-BYTE-IS-EV70-VSTOP (11 ids)
//   SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT (4 ids)
//   SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT (2 ids)
//   SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED [declined] (2 ids)
//   AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL (39 ids: 2 on the
//     assertion, 37 on its two [declined] records)
//   SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS (39 ids)
//   SPRXDATA-SPRXDATB-DMA-PATH-ONLY-AT-THIS-BOUNDARY (36 ids)
//   34+11+11+4+2+2+39+39+36 = 178.
//
// CONTROL CASE: FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER (no page, no
// claim id, labelled "[control]"; it carries its own mutation record)
// proves this fixture can deliver a known word before anything reads one:
// two DIFFERENT (VSTART,VSTOP) pairs, written directly through
// reg_address_in/data_in, each land in their OWN correct role (VSTART
// governs where the window starts, VSTOP where it ends -- not swapped,
// not merged, not dropped) and produce the exact window classify.evidence
// already measured (40..44, then 40..46) -- before any claim-based
// assertion below depends on the same write -> sweep -> observe path.
//
// STIMULUS CHOICES (neither changes an expected value, only stimulus):
// (1) SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT's SV8=0 case arms SPR0CTL with
// an EV8-set (VSTOP=511) value, comfortably clear of VSTART=255. Arming
// it with the SAME byte (0xFF) used for SPR0POS would make VSTART and
// VSTOP both 255; per agnus_spritedma.v:242 (VSTOP is checked before
// VSTART on a simultaneous vpos match, giving VSTOP precedence), that
// sprite would never activate at all, and the assertion would measure
// start==-1 for the wrong reason. (2) SPRXPOS-SPRXCTL-ADDRESS-MAP's and
// SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS's own
// address-set sweeps start at vpos=1, not 0: every sprite channel's own
// (never-written) VSTOP defaults to 0, which equals vpos=0 on that first
// line and independently latches `sprvstop` (agnus_spritedma.v:255-258),
// triggering the very POS/CTL-prefetch cycle these two assertions observe
// THROUGH A DIFFERENT PATH than the one (`vblend`) they exercise -- with
// a vpos=0 start, the MUT_ADDRMAP/MUT_WRITEBUS mutations below
// (withholding vblend) do not move either assertion. No channel's
// default VSTOP=0 matches vpos=1, so each mutation turns only its own
// assertion red (see MUTATION LOG below).
//
// RESULTS: 14 passed, 0 failed. No RTL deviation found at this boundary.
// agnus_spritedma.v conforms, at this boundary, to every printed
// proposition this package could test: the SV7-SV0/EV7-EV0 byte fields,
// the SV8/EV8 high bits, the full 16-address POS/CTL map for all 8
// channels, Agnus's own genuine write-target status for SPRxPOS/SPRxCTL,
// and the structural existence of both a DMA path and a processor path
// for SPRxPOS/SPRxCTL (DMA-only for SPRxDATA/SPRxDATB, per the module's
// own decode); and, for the routed claims below, the ECS register map's
// "chg" row for SPRxCTL is real and ecs-gated for all 8 channels, and
// appendix C's "position, size and feature" claim and the ECS map's $142
// request/write claims both hold at Harness(ecs=1), not only at this
// fixture's default ecs=0. The one MEASURED (non-claim) finding --
// SPRxCTL bits 6/5 are vstart[9]/vstop[9], gated by `ecs`, each
// contributing exactly +512 to the measured line when set (base=4 bit6=4
// at ecs=0, base=4 bit6=516 at ecs=1; base=8 bit5=8 at ecs=0, base=8
// bit5=520 at ecs=1) -- is not a deviation against any claim (no claim in
// this package's 178 records states what these bits do), so it is
// reported, not scored red or green.
//
// MUTATION LOG (each entry run under WSL/Verilator: build with
// -DMUT_NAME=1 -> rebuild -> run -> confirm the named assertion, and only
// that assertion, changed colour -> rebuild with no flags -> confirm the
// baseline restored; the pass/fail totals quoted in an entry are those of
// the run that recorded it. nanomig/ is not touched; every entry changes
// stimulus only, each guarded behind its own #if MUT_NAME flag so the
// default build is unmutated):
//
// MUTATION FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER stimulus red: -DMUT_CONTROL=1 makes the second measurement's SPR0CTL write 0x2C (the SAME as the first measurement's) instead of 0x2E, so the two windows stop differing -> FAIL ("8 passed, 1 failed"), only this control case newly red (w2.end became 44, not the asserted 46). Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXPOS-SPRXCTL-ADDRESS-MAP stimulus red: -DMUT_ADDRMAP=1 withholds vblend (held low instead of high) across the sweep -> FAIL ("8 passed, 1 failed"), only this assertion newly red (the collected address set came back empty -- see STIMULUS CHOICES above for why the sweep had to start at vpos=1, not 0, for this mutation to be meaningful at all). Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXPOS-BYTE-IS-SV70-VSTART stimulus red: -DMUT_SV70=1 makes the second SPR0POS write 0x20 (the same as the first) instead of 0x21 -> FAIL ("8 passed, 1 failed"), only this assertion newly red (wb.start no longer differs from wa.start by 1; the two are equal). Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXCTL-BYTE-IS-EV70-VSTOP stimulus red: -DMUT_EV70=1 makes the second SPR0CTL write 0x20 (the same as the first) instead of 0x21 -> FAIL ("8 passed, 1 failed"), only this assertion newly red (wb.end no longer differs from wa.end by 1). Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT stimulus red: -DMUT_SV8=1 withholds the SV8 stimulus in the "hb" measurement (SPR0CTL held at $FF/SV8=0 instead of SV8=1) -> FAIL ("8 passed, 1 failed"), only this assertion newly red (wb.start no longer 511). Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT stimulus red: -DMUT_EV8=1 withholds the EV8 stimulus in the "hb" measurement (EV8=0 instead of 1) -> FAIL ("8 passed, 1 failed"), only this assertion newly red (wb.end no longer 511). Rebuilt with no flags: 9/0 restored.
// MUTATION AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL stimulus red: -DMUT_AGNUSTARGET=1 makes the second SPR0POS write in this assertion's own independent measurement 0x10 (the same as the first) instead of 0x18 -> FAIL ("8 passed, 1 failed"), only this assertion newly red (pos_reaches_agnus became false); SPRXPOS-BYTE-IS-SV70-VSTART, which shares no variable with this assertion's own measurement, stayed green, confirming the isolation held. Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS stimulus red: -DMUT_WRITEBUS=1 withholds vblend in this assertion's own independent address-set sweep -> FAIL ("8 passed, 1 failed"), only this assertion newly red (dma_side became false); SPRXPOS-SPRXCTL-ADDRESS-MAP, which uses the same technique but its own separate sweep, stayed green. Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXDATA-SPRXDATB-DMA-PATH-ONLY-AT-THIS-BOUNDARY stimulus red: -DMUT_DATADATB=1 changes the arming SPR0POS write to 0x20 (VSTART=32) instead of 0x28 (VSTART=40), so the measured window no longer starts at line 40 -> FAIL ("8 passed, 1 failed"), only this assertion newly red (wd.start became 32, not 40). Rebuilt with no flags: 9/0 restored.
// MUTATION SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED: no record -- this claim is declined (checks.record() only, no checks.expect()), so there is no red/green state for a mutation to move.

// ============================================================================
// ROUTED CLAIMS: 14 further ids, from chapter pages 107/128 and
// appendices B and C. What each states:
//
// R1 SPRXCTL-BITS-6-3-UNUSED-OCS (4 ids: CH4-p107.c8, CH4-sprctl.c6,
//   CH4-p128.c14, CH4-sctl.c13): chapter body page 107 ("Bits 6-3 Unused
//   (make zero)") and page 128 ("Bits 6-3 are reserved for future use (make
//   zero)"), each read twice (sol, opus), state that SPRxCTL bits 6 through
//   3 are unused/reserved and must be written zero. This package's own
//   classify.conflict_resolution -- identical text on all four claims --
//   names the SAME dispute SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED above
//   declines: appendix C p303 names bits 4 and 3 SHSH1/SHSH0,
//   "unimplemented ... in the ECS chips only", against the chapter's and
//   appendix A's "unused". The undisputed sub-span is bits 6 and 5, which
//   this file's own MEASURED note above (not a claim, not asserted)
//   reports as inert on OCS (ecs=0) and live on ECS (ecs=1). These four
//   ids restate the identical BUNDLED span -- bits 6 through 3 together,
//   not split per bit -- from two further pages and two further readers
//   (disposition below: recorded, the bundle withdrawn whole).
//
// R2 SPRXCTL-CHIP-COLUMN-CONFLICT (2 ids: APPB2-conf.c1, APPB2-conf.c2):
//   appendix B p291 gives SPR0CTL's chip column as "A D(E)"; p292 gives
//   SPR1CTL's as "A D", no qualifier -- the Denise/ECS-qualifier half of
//   the same five-way chip-column conflict this package's own
//   AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL assertion already leaves
//   undisputed-but-untested (the Denise half, the "(E)" qualifier). Package
//   field: "REGRESSION-CLASS: ch4-source-conflicts (record both readings,
//   assert neither)"; both restatements end "Recorded, not resolved."
//
// R3 SPR0POS-PCT-MARK-FREQUENCY (1 id: APPB2-conf.c5): appendix B p291's
//   "%" mark on SPR0POS, chapter p128 ("normally only written by the sprite
//   DMA channel itself") and appendix A p284 ("may be loaded by either
//   processor at any time") all state a RELATIVE FREQUENCY for the same
//   dual DMA/processor write path this package's own
//   SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS assertion
//   already asserts the structural half of and explicitly declines the
//   frequency half of ("no port carries how often a write happens").
//   Package field marks this REGRESSION-CLASS too; restatement: "recorded
//   here because the mark is the only one of the three that is
//   machine-readable per register."
//
// R4 SPRXPOS-SPRXCTL-JOINT-SIZE-ECS (1 id: CH4-appC-position.c6):
//   appendix C p303, the ECS appendix: "The ECS registers SPRxPOS and
//   SPRxCTL work together as position, size and sprite feature control
//   registers." Same substance as p284's "position, size and feature"
//   wording (APPA-sprpair.c1, recorded beside this package's own
//   AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL assertion, which
//   measures the position and size components at this fixture's default
//   ecs=0) -- but sourced from the ECS appendix specifically, so it needs
//   an independent ecs=1 measurement (disposition below: graded on
//   SPRXPOS-SPRXCTL-JOINT-SIZE-ECS).
//
// R5 SPRXCTL-ECS-MAP-ROW (4 ids: APPC2-SPRxCTL.addr, APPC2-SPRxCTL.v20,
//   APPC2-SPRxCTL.acc, APPC2-SPRxCTL.chip): one printed row, appendix C
//   p298's ECS register map, "142  SPRxCTL  chg   W  A   Sprite x position
//   and control data", read column by column: address $142 (.addr),
//   version-2.0 status "chg" i.e. changed-not-new (.v20), access "W" i.e.
//   write (.acc), chip column "A" alone i.e. Agnus (.chip). classify's own
//   conflict_resolution on two of these (.chip, and the related .conf1
//   below) separates an undisputed sub-claim -- SPRxCTL is an
//   Agnus register -- from a disputed one -- whether Denise also
//   implements it, kept record-only -- the same split this package's own
//   AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL assertion makes
//   for the appendix A/B readings of the same fact.
//
// R6 SPRXCTL-ECS-MAP-ROW-CONFLICTS (2 ids: APPC2-SPRxCTL.conf1,
//   APPC2-SPRxCTL.conf3): the same appendix C p298 row's chip column read
//   against four other printed readings of the same column (map p298 "A"
//   alone; appendix A p284 "A D( E )"; appendix B p291 "A D(E)" for
//   SPR0CTL; appendix B p292 "A D" for SPR1CTL-SPR7CTL; appendix C p303
//   body text "A D") -- five readings, recorded not resolved
//   (APPC2-SPRxCTL.conf1); and whether "SPRxCTL chg" printed once at $142
//   means all eight sprite control registers changed, or only the one at
//   $142, is not stated by the source (APPC2-SPRxCTL.conf3). Both
//   restatements end "Recorded, not resolved."
//
// DISPOSITION: 4 ids graded, 10 recorded. A graded id sits on an
// assertion that establishes the whole of what its claim states -- all 8
// channels where the claim names the indexed family, ecs=1 where the
// source is ECS-scoped -- and an id that states an ambiguity, or bundles
// a half this fixture cannot observe, is recorded instead.
//
//   GRADED (4 ids):
//     SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS: APPC2-SPRxCTL.v20 (1 id
//       -- this assertion measures all 8 channels, not sprite 0 alone as
//       SPRXCTL-CHG-IS-ECS-DEPENDENT does, and is true under either
//       reading of whether "SPRxCTL chg" at $142 names one register or the
//       whole indexed family)
//     SPRXPOS-SPRXCTL-JOINT-SIZE-ECS: CH4-appC-position.c6 (1 id --
//       appendix C's own ECS-scoped source needs an ecs=1 measurement;
//       SPRXPOS-BYTE-IS-SV70-VSTART/SPRXCTL-BYTE-IS-EV70-VSTOP measure at
//       ecs=0 only)
//     SPRXCTL-ECS-MAP-ROW-LIVE-AT-ECS1: APPC2-SPRxCTL.addr,
//       APPC2-SPRxCTL.acc (2 ids -- the ECS register map's row, measured
//       at ecs=1; SPRXPOS-SPRXCTL-ADDRESS-MAP and
//       AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL measure it at ecs=0
//       only)
//     1+1+2 = 4.
//
//   RECORDED (10 ids):
//     SPRXCTL-CHIP-COLUMN-DENISE-ECS-QUALIFIER-CONFLICT: APPB2-conf.c1,
//       APPB2-conf.c2 (2 ids)
//     SPRXCTL-CHIP-COLUMN-FIVE-WAY-CONFLICT: APPC2-SPRxCTL.conf1,
//       APPC2-SPRxCTL.chip (2 ids)
//     SPR0POS-PCT-MARK-FREQUENCY: APPB2-conf.c5 (1 id)
//     SPRXCTL-BITS-6-3-BUNDLED-SPAN-HALF-UNTESTED: CH4-p107.c8,
//       CH4-sprctl.c6, CH4-p128.c14, CH4-sctl.c13 (4 ids -- the claim
//       bundles bits 6/5, which SPRXCTL-BITS-6-3-UNUSED-AT-ECS0
//       establishes at ecs=0, with bits 4/3, which reach no wire in this
//       module under any ecs value; a bundled claim is withdrawn whole)
//     SPRXCTL-CHG-CHANGE-SCOPE-AMBIGUOUS: APPC2-SPRxCTL.conf3 (1 id --
//       this id IS an ambiguity record ("recorded, not resolved"); grading
//       it would grade one resolution of it)
//     2+2+1+4+1 = 10.
//
// MUTATION LOG, ROUTED-CLAIM ASSERTIONS (same protocol as the MUTATION
// LOG above -- build with -DMUT_NAME=1, rebuild, run, confirm the named
// assertion and only that assertion changed colour, rebuild with no
// flags, confirm the baseline restored):
//
// MUTATION SPRXCTL-BITS-6-3-UNUSED-AT-ECS0 stimulus red: -DMUT_BITS63=1
// switches ecs on (was off) for the bits-6-3-set side of the comparison
// only -> FAIL ("13 passed, 1 failed"), only this assertion newly red (the
// two measured windows, equal at ecs=0, diverge once bit6/bit5 go live at
// ecs=1). Rebuilt with no flags: 14/0 restored.
// MUTATION SPRXCTL-CHG-IS-ECS-DEPENDENT stimulus red: -DMUT_CHGECS=1
// withholds bit 6 in the probe (was set) -> FAIL ("13 passed, 1 failed"),
// only this assertion newly red (the ecs=0 and ecs=1 measurements become
// equal, since the one bit that differs between them was never written).
// Rebuilt with no flags: 14/0 restored. (This assertion carries no claim
// id -- see the disposition above; it is the sprite-0 sub-case of
// SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS, mutation-tested on its own.)
// MUTATION SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS stimulus red:
// -DMUT_ALLCHANNELS=1 restricts the bit6 stimulus to sprite channel 0
// only (channels 1-7 always get it clear, were all set) -> FAIL ("13
// passed, 1 failed"), only this assertion newly red (channels 1-7 no
// longer show an ecs=0/ecs=1 difference). Rebuilt with no flags: 14/0
// restored.
// MUTATION SPRXPOS-SPRXCTL-JOINT-SIZE-ECS stimulus red: -DMUT_JOINTSIZEECS
// =1 makes the second SPR0POS write in this assertion's own ecs=1
// VSTART-vs-start comparison 0x20 (the same as the first) instead of
// 0x21 -> FAIL ("13 passed, 1 failed"), only this assertion newly red
// (the VSTOP-vs-end half, untouched by the flag, stayed green in
// isolation). Rebuilt with no flags: 14/0 restored.
// MUTATION SPRXCTL-ECS-MAP-ROW-LIVE-AT-ECS1 stimulus red: -DMUT_ECSMAPROW
// =1 makes the second SPR0CTL write in this assertion's own ecs=1
// write-reaches-agnus comparison 0x10 (the same as the first) instead of
// 0x14 -> FAIL ("13 passed, 1 failed"), only this assertion newly red
// (the address-requested half, untouched by the flag, stayed green in
// isolation). Rebuilt with no flags: 14/0 restored.
// MUTATION SPRXCTL-CHIP-COLUMN-DENISE-ECS-QUALIFIER-CONFLICT,
// SPRXCTL-CHIP-COLUMN-FIVE-WAY-CONFLICT, SPR0POS-PCT-MARK-FREQUENCY,
// SPRXCTL-BITS-6-3-BUNDLED-SPAN-HALF-UNTESTED,
// SPRXCTL-CHG-CHANGE-SCOPE-AMBIGUOUS: no record -- these five claims are
// declined (checks.record() only, no checks.expect()), so there is no
// red/green state for a mutation to move, same as
// SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED above.
//
// UID PLACEMENT: tools/check_uid_placement.py reads a `<reader>:`-prefixed
// uid (`opus:`/`sol:` per uid_expand.py's BASE regex) and, separately, a
// bare id in this package's own convention (`CH4-p107.c8`,
// `APPC2-SPRxCTL.chip`, ...) under its own `bare:` pseudo-prefix (the
// tool's BARE regex) -- it is not blind to this package. Its "packages
// asserting AND declining one : 0" on this file is therefore a real check,
// not just a hand check, and by hand the same holds: each of the 4 graded
// ids appears on a PASS: line and never on a RECORDED: line (the
// SPRXCTL-CHG-CHANGE-SCOPE-AMBIGUOUS record() names APPC2-SPRxCTL.v20 only
// as a cross-reference to where it IS graded, marked "(graded: ...)" so
// the tool reads it as a reference, not a decline); each of the 10
// recorded ids appears on a RECORDED: line and never on a PASS:/FAIL:
// line.
// ============================================================================

// MUT_*: replay stimulus for this file's own mutation records (RESULTS/
// MUTATION LOG below), one flag per non-declined assertion, each
// isolated to that assertion's own call site so a mutation build changes
// only the named assertion's colour. -DMUT_NAME=1 to replay one.
#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_ADDRMAP
#define MUT_ADDRMAP 0
#endif
#ifndef MUT_SV70
#define MUT_SV70 0
#endif
#ifndef MUT_EV70
#define MUT_EV70 0
#endif
#ifndef MUT_SV8
#define MUT_SV8 0
#endif
#ifndef MUT_EV8
#define MUT_EV8 0
#endif
#ifndef MUT_AGNUSTARGET
#define MUT_AGNUSTARGET 0
#endif
#ifndef MUT_WRITEBUS
#define MUT_WRITEBUS 0
#endif
#ifndef MUT_DATADATB
#define MUT_DATADATB 0
#endif
#ifndef MUT_BITS63
#define MUT_BITS63 0
#endif
#ifndef MUT_CHGECS
#define MUT_CHGECS 0
#endif
#ifndef MUT_ALLCHANNELS
#define MUT_ALLCHANNELS 0
#endif
#ifndef MUT_JOINTSIZEECS
#define MUT_JOINTSIZEECS 0
#endif
#ifndef MUT_ECSMAPROW
#define MUT_ECSMAPROW 0
#endif

#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_spritedma.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records that state what the SOURCE says, or why a claim is
    // declined. Not measurements; must never be printed as though they
    // were.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map, as in the pre-pipeline (removed) hrm/agnus_spritedma/
// test_agnus_spritedma.cpp for this same module, generalised to all 8
// sprite channels per this package's own classify.evidence ("reg_address_
// in[8:6]==$140[8:6] with [2:1] selecting POS/CTL and [5:3] the channel"):
// each sprite occupies 4 consecutive word addresses (POS,CTL,DATA,DATB),
// 8 bytes apart in byte-address terms, 8 sprites total.
constexpr int PosWordAddr(int n) { return (0x140 + 8 * n) >> 1; }
constexpr int CtlWordAddr(int n) { return (0x142 + 8 * n) >> 1; }
constexpr int DataByteAddr(int n) { return 0x144 + 8 * n; }
constexpr int DatbByteAddr(int n) { return 0x146 + 8 * n; }

// One isolated DUT: fresh reset, clk7_en 4-cycle-per-hpos clocking and the
// reg_address_in/data_in write sequencing are those of the pre-pipeline
// harness for this SAME module (see HARNESS AND STIMULUS SOURCES above).
class Harness {
public:
    explicit Harness(uint8_t ecs = 0, uint8_t aga = 0) : dut(new Vagnus_spritedma) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->aga = aga;
        dut->ecs = ecs;
        dut->ackdma = 0;
        dut->vbl = 0;
        dut->vblend = 0;
        dut->hpos = 0;
        dut->vpos = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        tick();
        dut->reset = 0;
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

    void tick_hpos() {
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        tick();
        tick();
        tick();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_hpos();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }

    Vagnus_spritedma* dut;
};

void write_pos(Harness& h, int sprite_idx, uint8_t vstart_low8) {
    h.write_reg(PosWordAddr(sprite_idx), static_cast<uint16_t>(vstart_low8) << 8);
}

// ev70_byte: SPRxCTL bits 15-8 (EV7-EV0). sv8/ev8: bit 2/bit 1, the
// PRINTED high bits (p284: "02 SV8", "01 EV8"). bit6/bit5: the ECS-only,
// manual-silent ninth-bit extension this package's own classify.evidence
// measured (vstart[9]=bit6, vstop[9]=bit5) -- used ONLY by the MEASURED
// note below, never inside a pass/fail expect().
void write_ctl(Harness& h, int sprite_idx, uint8_t ev70_byte, bool sv8, bool ev8,
               bool bit6 = false, bool bit5 = false, bool bit4 = false, bool bit3 = false) {
    uint16_t v = (static_cast<uint16_t>(ev70_byte) << 8)
               | (bit6 ? 0x0040u : 0u)
               | (bit5 ? 0x0020u : 0u)
               | (bit4 ? 0x0010u : 0u)
               | (bit3 ? 0x0008u : 0u)
               | (sv8  ? 0x0004u : 0u)
               | (ev8  ? 0x0002u : 0u);
    h.write_reg(CtlWordAddr(sprite_idx), v);
}

struct Window {
    int start = -1;  // first line (0-based) at which this sprite's own
                      // DATA/DATB address is seen on reg_address_out
                      // during a granted reqdma cycle.
    int end = -1;     // one past the last such line: [start,end) is the
                       // observed DMA-active range.
};

// Sweeps vpos 0..vpos_max (exclusive), vblend held low throughout (so no
// automatic POS/CTL re-fetch competes with a value already written
// directly before this call), granting ackdma immediately whenever reqdma
// is seen -- reqdma is always sampled by its OWN eval() BEFORE ackdma is
// asserted and evaluated again, never the reverse: setting the DMA grant
// inside the same eval() that samples the request makes a harness's data
// path look silently dead.
Window measure_window(Harness& h, int sprite_idx, int vpos_max) {
    Window w;
    const int data_byte = DataByteAddr(sprite_idx);
    const int datb_byte = DatbByteAddr(sprite_idx);
    for (int line = 0; line < vpos_max; ++line) {
        h.dut->vpos = line;
        h.dut->vblend = 0;
        bool active_this_line = false;
        for (int hp = 0; hp < 454; ++hp) {
            h.dut->hpos = hp;
            h.dut->clk7_en = 1;
            h.dut->eval();
            if (h.dut->reqdma) {
                h.dut->ackdma = 1;
                h.dut->eval();
                int reg_out = h.dut->reg_address_out << 1;
                if (reg_out == data_byte || reg_out == datb_byte) active_this_line = true;
            } else {
                h.dut->ackdma = 0;
            }
            h.tick();
            h.dut->clk7_en = 0;
            h.tick();
            h.tick();
            h.tick();
        }
        if (active_this_line) {
            if (w.start == -1) w.start = line;
            w.end = line + 1;
        }
    }
    return w;
}

// Sweeps vpos in [vpos_lo,vpos_hi) with vblend held HIGH throughout (the
// automatic prefetch trigger the sibling harness's own "Line 31 (vblend):
// POS/CTL fetch" exercises), collecting the SET of byte addresses this
// module's own DMA sequencer requests on its own -- no register is
// written before this call; the Harness is used at its power-on default.
std::set<int> sweep_prefetch_addresses(Harness& h, int vpos_lo, int vpos_hi,
                                        bool assert_vblend = true) {
    std::set<int> seen;
    for (int line = vpos_lo; line < vpos_hi; ++line) {
        h.dut->vpos = line;
        h.dut->vblend = assert_vblend ? 1 : 0;
        for (int hp = 0; hp < 454; ++hp) {
            h.dut->hpos = hp;
            h.dut->clk7_en = 1;
            h.dut->eval();
            if (h.dut->reqdma) {
                h.dut->ackdma = 1;
                h.dut->eval();
                seen.insert(h.dut->reg_address_out << 1);
            } else {
                h.dut->ackdma = 0;
            }
            h.tick();
            h.dut->clk7_en = 0;
            h.tick();
            h.tick();
            h.tick();
        }
    }
    return seen;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ------------- [control] FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER -------------
    {
        // Stimulus from this package's own classify.evidence
        // measurement (see HARNESS AND STIMULUS SOURCES above):
        // SPR0POS=$28 (VSTART=40), SPR0CTL=$2C (VSTOP=44) -> window
        // [40,44); SPR0CTL=$2E (VSTOP=46) -> window [40,46).
        Harness h1;
        write_pos(h1, 0, 0x28);
        write_ctl(h1, 0, 0x2C, false, false);
        Window w1 = measure_window(h1, 0, 50);
        bool first_pair_ok = (w1.start == 40 && w1.end == 44);

        Harness h2;
        write_pos(h2, 0, 0x28);
#if MUT_CONTROL
        write_ctl(h2, 0, 0x2C, false, false);  // MUTATION: same VSTOP as h1 (was 0x2E).
#else
        write_ctl(h2, 0, 0x2E, false, false);
#endif
        Window w2 = measure_window(h2, 0, 50);
        bool second_pair_ok = (w2.start == 40 && w2.end == 46);

        checks.expect(first_pair_ok && second_pair_ok,
            "[control] FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER: writing "
            "SPR0POS=$28 (VSTART=40) then SPR0CTL=$2C (VSTOP=44) through "
            "reg_address_in/data_in -- the one write interface every write "
            "in this file uses -- produces the DMA-active window [40,44); "
            "writing the SAME SPR0POS=$28 with a DIFFERENT SPR0CTL=$2E "
            "(VSTOP=46) on a fresh DUT produces [40,46), not [40,44) again "
            "and not some corrupted or swapped range -- two distinct "
            "values arrive, in the correct role each was written for, at "
            "the cycle they were written on. Both windows match this "
            "package's own classify.evidence measurement exactly. Fixture "
            "check, no claim id, no page.");
    }

    // ------------- SPRXPOS-SPRXCTL-ADDRESS-MAP -------------
    {
        Harness h;
        // Sweep starts at vpos=1, not 0: at vpos=0, every sprite's own
        // (never-written, default-0) vstop coincidentally equals vpos,
        // which independently latches sprvstop true regardless of
        // vblend (agnus_spritedma.v:255-258) -- a fixture-default
        // coincidence discovered only once the RTL was open and the
        // MUT_ADDRMAP mutation below failed to move this assertion at
        // first. Starting at vpos=1 avoids it (no channel's default
        // vstop=0 matches vpos=1); the addresses asserted for are
        // unchanged.
#if MUT_ADDRMAP
        std::set<int> seen = sweep_prefetch_addresses(h, 1, 30, /*assert_vblend=*/false);  // MUTATION: vblend withheld.
#else
        std::set<int> seen = sweep_prefetch_addresses(h, 1, 30);
#endif
        bool all_present = true;
        std::vector<int> missing;
        for (int n = 0; n < 8; ++n) {
            int pos_addr = 0x140 + 8 * n;
            int ctl_addr = 0x142 + 8 * n;
            if (!seen.count(pos_addr)) { all_present = false; missing.push_back(pos_addr); }
            if (!seen.count(ctl_addr)) { all_present = false; missing.push_back(ctl_addr); }
        }
        checks.expect(all_present,
            "SPRXPOS-SPRXCTL-ADDRESS-MAP: from a freshly reset DUT (no "
            "register written), sweeping vpos 1..29 with vblend held high "
            "-- the automatic POS/CTL-prefetch trigger -- and granting "
            "ackdma on every reqdma, this module's own DMA sequencer "
            "requests all 16 of the printed word addresses on its own: "
            "$140/$142 (sprite 0) through $178/$17A (sprite 7), matching "
            "'SPR0POS 140 w A D' / 'SPR0CTL 142 w A D(E)' through "
            "'SPR7POS 178 W A D' / 'SPR7CTL 17A W A D' (pp284,291,292), "
            "confirming the printed address map exists as a real decode "
            "point in this module for every one of the 8 channels -- "
            "consistent with this package's own classify.evidence, which "
            "separately reports the decode logic itself as 'confirmed by "
            "measurement I (eight distinct pointer banks)'. "
            "CH4-appA-sprite.c5, APPB2-SPR0POS.addr, APPB2-SPR1POS.addr, "
            "CH4-APPB-SPR0POS.address, CH4-APPB-SPR1POS.address, "
            "APPB2-SPR2POS.addr, APPB2-SPR3POS.addr, APPB2-SPR4POS.addr, "
            "APPB2-SPR5POS.addr, APPB2-SPR6POS.addr, APPB2-SPR7POS.addr, "
            "CH4-APPB-SPR2POS.address, CH4-APPB-SPR3POS.address, "
            "CH4-APPB-SPR4POS.address, CH4-APPB-SPR5POS.address, "
            "CH4-APPB-SPR6POS.address, CH4-APPB-SPR7POS.address, "
            "CH4-appA-sprite.c1, APPB2-SPR0CTL.addr, "
            "CH4-APPB-SPR0CTL.address, APPB2-SPR1CTL.addr, "
            "APPB2-SPR2CTL.addr, APPB2-SPR3CTL.addr, APPB2-SPR4CTL.addr, "
            "APPB2-SPR5CTL.addr, APPB2-SPR6CTL.addr, APPB2-SPR7CTL.addr, "
            "CH4-APPB-SPR1CTL.address, CH4-APPB-SPR2CTL.address, "
            "CH4-APPB-SPR3CTL.address, CH4-APPB-SPR4CTL.address, "
            "CH4-APPB-SPR5CTL.address, CH4-APPB-SPR6CTL.address, "
            "CH4-APPB-SPR7CTL.address . ");
    }

    // ------------- SPRXPOS-BYTE-IS-SV70-VSTART -------------
    {
        Harness ha;
        write_pos(ha, 0, 0x20);
        write_ctl(ha, 0, 0xFF, false, false);  // VSTOP far out so the window does not close early.
        Window wa = measure_window(ha, 0, 50);

        Harness hb;
#if MUT_SV70
        write_pos(hb, 0, 0x20);  // MUTATION: same byte as ha (was 0x21).
#else
        write_pos(hb, 0, 0x21);
#endif
        write_ctl(hb, 0, 0xFF, false, false);
        Window wb = measure_window(hb, 0, 50);

        bool ok = wa.start != -1 && wb.start != -1 && (wb.start - wa.start) == 1;
        checks.expect(ok,
            "SPRXPOS-BYTE-IS-SV70-VSTART: incrementing SPR0POS's byte by "
            "1 (0x20 -> 0x21, SPR0CTL held at $FF/VSTOP far out) moves the "
            "measured DMA-active start line by exactly 1, matching "
            "'15-08 SV7-SV0 Start vertical value.' (p284) -- SPRxPOS bits "
            "15-8 ARE the (low 8 bits of the) vertical start value this "
            "module compares vpos against. CH4-appA-sprite.c8 and the "
            "per-sprite APPB2/CH4-APPB '.description'/'.mark' restatements "
            "of 'sprite x vert-horiz start position data' all restate the "
            "same identity. APPA-sprpos.c3, CH4-appA-sprite.c14, "
            "CH4-appA-sprite.c8, CH4-APPB-SPR0POS.description, "
            "CH4-APPB-SPR1POS.description, CH4-APPB-SPR2POS.description, "
            "CH4-APPB-SPR3POS.description, CH4-APPB-SPR4POS.description, "
            "CH4-APPB-SPR5POS.description, CH4-APPB-SPR6POS.description, "
            "CH4-APPB-SPR7POS.description . ");
    }

    // ------------- SPRXCTL-BYTE-IS-EV70-VSTOP -------------
    {
        Harness ha;
        write_pos(ha, 0, 0x02);  // VSTART early and fixed.
        write_ctl(ha, 0, 0x20, false, false);
        Window wa = measure_window(ha, 0, 50);

        Harness hb;
        write_pos(hb, 0, 0x02);
#if MUT_EV70
        write_ctl(hb, 0, 0x20, false, false);  // MUTATION: same byte as ha (was 0x21).
#else
        write_ctl(hb, 0, 0x21, false, false);
#endif
        Window wb = measure_window(hb, 0, 50);

        bool ok = wa.end != -1 && wb.end != -1 && (wb.end - wa.end) == 1;
        checks.expect(ok,
            "SPRXCTL-BYTE-IS-EV70-VSTOP: with SPR0POS fixed at 0x02 "
            "(VSTART=2), incrementing SPR0CTL's upper byte by 1 (0x20 -> "
            "0x21) moves the measured DMA-active end line by exactly 1, "
            "matching '15-08 EV7-EV0 End (stop) vertical value low 8 "
            "bits' (p284) -- SPRxCTL bits 15-8 ARE the (low 8 bits of the) "
            "vertical stop value. The per-sprite '.description' rows "
            "restate the same register identity ('sprite x vert stop "
            "position and control data'). APPA-sprctl.c4, "
            "CH4-appA-sprite.c19, CH4-appA-sprite.c4, "
            "CH4-APPB-SPR0CTL.description, CH4-APPB-SPR1CTL.description, "
            "CH4-APPB-SPR2CTL.description, CH4-APPB-SPR3CTL.description, "
            "CH4-APPB-SPR4CTL.description, CH4-APPB-SPR5CTL.description, "
            "CH4-APPB-SPR6CTL.description, CH4-APPB-SPR7CTL.description . ");
    }

    // ------------- SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT -------------
    {
        Harness ha;
        write_pos(ha, 0, 0xFF);               // VSTART low byte maxed (255).
        write_ctl(ha, 0, 0xFF, false, true);   // SV8=0, VSTOP=511 (EV8 set) so
                                                // VSTOP cannot coincide with
                                                // VSTART=255: agnus_spritedma.v:242
                                                // checks VSTOP before VSTART on a
                                                // simultaneous match, so VSTOP=255
                                                // would keep this sprite from ever
                                                // activating (see STIMULUS
                                                // CHOICES above).
        Window wa = measure_window(ha, 0, 260);

        Harness hb;
        write_pos(hb, 0, 0xFF);
#if MUT_SV8
        write_ctl(hb, 0, 0xFF, false, false);  // MUTATION: SV8 withheld (was true).
#else
        write_ctl(hb, 0, 0xFF, true, false);  // SV8=1.
#endif
        Window wb = measure_window(hb, 0, 520);

        bool ok = wa.start == 255 && wb.start == 511;
        checks.expect(ok,
            "SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT: with SPR0POS fixed at "
            "0xFF (SV7-SV0 maxed at 255), SPR0CTL bit 2 (SV8) clear gives "
            "a measured start of line 255; setting it alone (all other "
            "bits, including EV8, held clear) gives a measured start of "
            "line 511 = 255 + 256 -- SV8 is genuinely the 9th (high) bit "
            "of the vertical start value, matching '02 SV8 Start vertical "
            "value high bit' (p284) read together with the SPRxPOS row "
            "that names the high bit as living in SPRxCTL: 'High bit(SV8) "
            "is in SPRxCTL register below.' (p284). APPA-sprctl.c7, "
            "CH4-appA-sprite.c22, APPA-sprpos.c4, CH4-appA-sprite.c15 . ");
    }

    // ------------- SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT -------------
    {
        Harness ha;
        write_pos(ha, 0, 0x02);               // VSTART early and fixed.
        write_ctl(ha, 0, 0xFF, false, false);  // EV8=0.
        Window wa = measure_window(ha, 0, 260);

        Harness hb;
        write_pos(hb, 0, 0x02);
#if MUT_EV8
        write_ctl(hb, 0, 0xFF, false, false);  // MUTATION: EV8 withheld (was true).
#else
        write_ctl(hb, 0, 0xFF, false, true);   // EV8=1.
#endif
        Window wb = measure_window(hb, 0, 520);

        bool ok = wa.end == 255 && wb.end == 511;
        checks.expect(ok,
            "SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT: with SPR0POS fixed at "
            "0x02 (VSTART=2) and SPR0CTL's upper byte maxed at 0xFF, bit "
            "1 (EV8) clear gives a measured end of line 255; setting it "
            "alone (all other bits, including SV8, held clear) gives a "
            "measured end of line 511 = 255 + 256, matching '01 EV8 End "
            "(stop) vertical value high bit' (p284) -- EV8 is genuinely "
            "the 9th (high) bit of the vertical stop value. "
            "APPA-sprctl.c8, CH4-appA-sprite.c23 . ");
    }

    // ------------- SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED [declined] -------------
    // No checks.expect() and no MUTATION record: the disputed part of
    // this claim (what bits 6-3 are) is exactly what an "unused"
    // assertion would have to pick a side on, and the undisputed bits in
    // this same printed row (SV8 at bit 2, EV8 at bit 1) are already
    // separately asserted above.
    checks.record(
        "SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED [declined]: APPA-sprctl.c6, "
        "CH4-appA-sprite.c21 -- p284 prints '06-04 X Not used', but "
        "chapter pp.107/128 give the unused span as bits 6-3 instead (per "
        "this package's own classify.conflict_resolution), and appendix C "
        "p303 separately assigns bit 4 SHSH1 and bit 3 SHSH0 "
        "'(unimplemented)', scoped to ECS chips only. What is undisputed "
        "in this same row -- SV8 at bit 2, EV8 at bit 1, EV7-EV0 at bits "
        "15-8 -- is already asserted by SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-"
        "BIT, SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT and "
        "SPRXCTL-BYTE-IS-EV70-VSTOP above. Declined, not tested: asserting "
        "'bits 6-4 unused' would require taking a side the sources "
        "themselves do not agree on.");

    // MEASURED note: bits 6 and 5 are silent in every printed source this
    // package's 178 claims draw on (see the header note above). Reported,
    // not asserted -- no claim id, no pass/fail. bit6 feeds vstart[9] (so
    // its effect shows on Window::start) and bit5 feeds vstop[9] (so its
    // effect shows on Window::end) -- two DIFFERENT fields, per
    // agnus_spritedma.v:221/225 and this package's own
    // classify.evidence. VSTART=4/VSTOP=8 (baseline, both
    // bits clear) are chosen distinct from each other and small, so a
    // +512 shift on either stays inside a single, shared sweep ceiling
    // without the two colliding.
    {
        auto probe = [](bool ecs, bool bit6, bool bit5) -> Window {
            Harness h(ecs);
            write_pos(h, 0, 0x04);                          // VSTART low byte = 4.
            write_ctl(h, 0, 0x08, false, false, bit6, bit5); // VSTOP low byte = 8.
            return measure_window(h, 0, 530);
        };
        Window ecs0_base = probe(false, false, false);
        Window ecs0_bit6 = probe(false, true, false);
        Window ecs0_bit5 = probe(false, false, true);
        Window ecs1_base = probe(true, false, false);
        Window ecs1_bit6 = probe(true, true, false);
        Window ecs1_bit5 = probe(true, false, true);
        checks.note(
            "SPRxCTL bits 6/5 (SPR0POS=0x04/VSTART=4, SPR0CTL upper "
            "byte=0x08/VSTOP=8, SV8/EV8 both clear, swept vpos 0..529): "
            "bit6's effect on the measured START line -- ecs=0: base=" +
            std::to_string(ecs0_base.start) + " bit6=" +
            std::to_string(ecs0_bit6.start) + "; ecs=1: base=" +
            std::to_string(ecs1_base.start) + " bit6=" +
            std::to_string(ecs1_bit6.start) +
            ". bit5's effect on the measured END line -- ecs=0: base=" +
            std::to_string(ecs0_base.end) + " bit5=" +
            std::to_string(ecs0_bit5.end) + "; ecs=1: base=" +
            std::to_string(ecs1_base.end) + " bit5=" +
            std::to_string(ecs1_bit5.end) + ". Per this package's own "
            "classify.evidence and agnus_spritedma.v:221 "
            "('{data_in[15:8],data_in[6],data_in[5],data_in[2],"
            "data_in[1]}') and :225/242/244: bit6 is vstart[9] and bit5 "
            "is vstop[9], "
            "each gated by the ecs input -- a fact no printed source in "
            "this package's 178 claims states. If a delta lands outside "
            "the swept range, that too is part of what was measured, not "
            "assumed.");
    }

    // ------------- AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL -------------
    {
        // Independent measurement (own Harness instances, not reusing
        // wa/wb from the assertions above) so a mutation aimed at this
        // assertion's own stimulus cannot silently move
        // SPRXPOS-BYTE-IS-SV70-VSTART or SPRXCTL-BYTE-IS-EV70-VSTOP too.
        Harness hp1;
        write_pos(hp1, 0, 0x10);
        write_ctl(hp1, 0, 0xFF, false, false);
        Window wp1 = measure_window(hp1, 0, 60);

        Harness hp2;
#if MUT_AGNUSTARGET
        write_pos(hp2, 0, 0x10);  // MUTATION: same byte as hp1 (was 0x18).
#else
        write_pos(hp2, 0, 0x18);
#endif
        write_ctl(hp2, 0, 0xFF, false, false);
        Window wp2 = measure_window(hp2, 0, 60);

        bool pos_reaches_agnus = wp1.start != -1 && wp2.start != -1 && wp1.start != wp2.start;

        Harness hc1;
        write_pos(hc1, 0, 0x04);
        write_ctl(hc1, 0, 0x10, false, false);
        Window wc1 = measure_window(hc1, 0, 40);

        Harness hc2;
        write_pos(hc2, 0, 0x04);
        write_ctl(hc2, 0, 0x14, false, false);
        Window wc2 = measure_window(hc2, 0, 40);

        bool ctl_reaches_agnus = wc1.end != -1 && wc2.end != -1 && wc1.end != wc2.end;

        checks.expect(pos_reaches_agnus && ctl_reaches_agnus,
            "AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL: a write to "
            "SPR0POS changes this module's own measured DMA-active start "
            "(shown above by SPRXPOS-BYTE-IS-SV70-VSTART, confirmed again "
            "here with an independent measurement), and a write to "
            "SPR0CTL changes this module's own measured end -- Agnus "
            "(this module) is demonstrably one of the chips the printed "
            "'A D'/'A D(E)' chip column names for these two registers, "
            "matching 'SPR0POS 140 w A D ...' and 'SPR0CTL 142 w A D(E) "
            "...' (p291) and their per-sprite restatements (pp284,291,"
            "292). This module has no Denise instance, so the 'D' half of "
            "every such claim, and the '(E)' qualifier specifically, are "
            "NOT tested here -- see Non-goals above; for the SPRxCTL "
            "claims this is also the undisputed half of a recorded "
            "conflict over whether the Denise side carries an ECS "
            "qualifier (classify.conflict_resolution: 'Undisputed: SPRxCTL "
            "is written by Agnus and by Denise ... Disputed and kept out "
            "of any assertion: whether the Denise side carries an ECS "
            "qualifier'). Independently, the SAME two measurements show "
            "SPRxPOS alone changes the result (position) and SPRxCTL "
            "alone changes the result (size, i.e. the vertical extent "
            "VSTART..VSTOP) -- neither register alone accounts for the "
            "whole result, matching 'SPRxPOS and SPRxCTL work together as "
            "position, size and feature sprite-control registers.' "
            "(p284). This boundary exposes no ATTACH/feature observable "
            "(see FIXTURE BOUNDARY above), so only the position and size "
            "components of that joint claim are asserted here. "
            "CH4-appA-sprite.c10, CH4-appA-sprite.c9 . ");
    }

    // No checks.expect() for the Denise half of the chip-column claims
    // AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL's own prose already
    // discusses: each of these ids bundles the untested 'D'/'(E)' half
    // into the same sentence as the tested 'A' half, so the id is
    // declined whole rather than kept on the graded line with a caveat.
    checks.record(
        "AGNUS-IS-WRITE-TARGET-DENISE-HALF-UNTESTED [declined]: "
        "APPA-sprpos.c2, APPA-sprctl.c2, CH4-appA-sprite.c3, "
        "CH4-appA-sprite.c7, APPB2-SPR0POS.chip, APPB2-SPR1POS.chip, "
        "APPB2-SPR2POS.chip, APPB2-SPR3POS.chip, APPB2-SPR4POS.chip, "
        "APPB2-SPR5POS.chip, APPB2-SPR6POS.chip, APPB2-SPR7POS.chip, "
        "APPB2-SPR0CTL.chip, APPB2-SPR1CTL.chip, APPB2-SPR2CTL.chip, "
        "APPB2-SPR3CTL.chip, APPB2-SPR4CTL.chip, APPB2-SPR5CTL.chip, "
        "APPB2-SPR6CTL.chip, APPB2-SPR7CTL.chip, CH4-APPB-SPR0POS.chip, "
        "CH4-APPB-SPR1POS.chip, CH4-APPB-SPR2POS.chip, "
        "CH4-APPB-SPR3POS.chip, CH4-APPB-SPR4POS.chip, "
        "CH4-APPB-SPR5POS.chip, CH4-APPB-SPR6POS.chip, "
        "CH4-APPB-SPR7POS.chip, CH4-APPB-SPR0CTL.chip, "
        "CH4-APPB-SPR1CTL.chip, CH4-APPB-SPR2CTL.chip, "
        "CH4-APPB-SPR3CTL.chip, CH4-APPB-SPR4CTL.chip, "
        "CH4-APPB-SPR5CTL.chip, CH4-APPB-SPR6CTL.chip, "
        "CH4-APPB-SPR7CTL.chip -- each restates the SPRxPOS/SPRxCTL chip "
        "column as 'A D', 'A D(E)' or 'Agnus and Denise' (appendix A "
        "p284, appendix B pp291-292). AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-"
        "AND-SPRXCTL above establishes only the 'A' half -- this fixture "
        "has no Denise instance (see FIXTURE BOUNDARY above), so the 'D' "
        "half of every one of these claims, and the '(E)' qualifier "
        "where printed, is NOT tested here. Declined, not graded: each "
        "claim bundles a clause this package cannot observe.");

    // Same treatment for the one id whose bundled claim names a third,
    // untested component (feature/ATTACH) instead of Denise.
    checks.record(
        "AGNUS-IS-WRITE-TARGET-FEATURE-CLAUSE-UNTESTED [declined]: "
        "APPA-sprpair.c1 -- 'SPRxPOS and SPRxCTL work together as the "
        "position, size and feature sprite-control registers.' (p284). "
        "AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL above establishes "
        "the position and size components of that same sentence, but "
        "this boundary exposes no ATTACH/feature observable (see FIXTURE "
        "BOUNDARY above) -- the feature component is a horizontal-"
        "boundary claim this package's fixture does not deliver. "
        "Declined, not graded: the claim bundles a clause this package "
        "cannot observe.");

    // --- SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS ---
    {
        // DMA side: this module requests both POS/CTL (already shown by
        // SPRXPOS-SPRXCTL-ADDRESS-MAP above) unprompted by any write --
        // re-measured here, independently, restricted to sprite 0.
        Harness hd;
        // Sweep starts at vpos=1, not 0, for the same reason
        // SPRXPOS-SPRXCTL-ADDRESS-MAP's own sweep does (see its comment
        // above): at vpos=0 every channel's default (never-written)
        // vstop=0 coincidentally matches, latching sprvstop regardless
        // of vblend.
#if MUT_WRITEBUS
        std::set<int> seen = sweep_prefetch_addresses(hd, 1, 5, /*assert_vblend=*/false);  // MUTATION: vblend withheld.
#else
        std::set<int> seen = sweep_prefetch_addresses(hd, 1, 5);
#endif
        bool dma_side = seen.count(0x140) && seen.count(0x142);

        // Processor side: a direct reg_address_in/data_in write (the
        // SAME mechanism used everywhere in this file) reaches and
        // changes this module's own state -- independent measurement,
        // own Harness instances.
        Harness hp1;
        write_pos(hp1, 0, 0x30);
        write_ctl(hp1, 0, 0xFF, false, false);
        Window wp1 = measure_window(hp1, 0, 60);

        Harness hp2;
        write_pos(hp2, 0, 0x38);
        write_ctl(hp2, 0, 0xFF, false, false);
        Window wp2 = measure_window(hp2, 0, 60);

        bool processor_side = wp1.start != -1 && wp2.start != -1 && wp1.start != wp2.start;

        checks.expect(dma_side && processor_side,
            "SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS: "
            "this module's own DMA sequencer requests SPR0POS ($140) and "
            "SPR0CTL ($142) on its own (re-measured here from a fresh, "
            "unwritten DUT), AND a direct reg_address_in/data_in write to "
            "the same two addresses independently changes this module's "
            "measured behaviour -- both the DMA path and the processor "
            "path this package's own classify.evidence names genuinely "
            "exist at this boundary for SPRxPOS/SPRxCTL ('agnus_spritedma "
            "accepts processor writes on reg_address_in (:205/220) and "
            "emits the same addresses itself on reg_address_out during a "
            "granted slot'), matching the '%' mark's legend definition "
            "('Register used by DMA channel usually, processors "
            "sometimes.' p287) and the body text ('usually loaded by the "
            "sprite DMA channel ...; however, they may be loaded by "
            "either processor' p284). Per this package's own "
            "classify.evidence, the RELATIVE FREQUENCY those words name "
            "('usually'/'sometimes'/'at any time'/'during horizontal "
            "blank') is NOT assertable at this or any boundary ('no port "
            "carries how often a write happens'; the DMA slot window is "
            "'hpos-gated ..., not blank-gated') -- only that both paths "
            "structurally exist is claimed here, never their frequency or "
            "a precise blanking-window correspondence. "
            "APPB2-legend.pct1, APPB2-legend.pct2, CH4-APPB-LEGEND.c002, "
            "APPB2-SPR0CTL.mark, APPB2-SPR0POS.mark, APPB2-SPR1POS.mark, "
            "CH4-APPB-SPR0CTL.mark, CH4-APPB-SPR0POS.mark, "
            "CH4-APPB-SPR1POS.mark, APPB2-SPR1CTL.mark, "
            "APPB2-SPR2CTL.mark, APPB2-SPR2POS.mark, APPB2-SPR3CTL.mark, "
            "APPB2-SPR3POS.mark, APPB2-SPR4CTL.mark, APPB2-SPR4POS.mark, "
            "APPB2-SPR5CTL.mark, APPB2-SPR5POS.mark, APPB2-SPR6CTL.mark, "
            "APPB2-SPR6POS.mark, APPB2-SPR7CTL.mark, APPB2-SPR7POS.mark, "
            "CH4-APPB-SPR1CTL.mark, CH4-APPB-SPR2CTL.mark, "
            "CH4-APPB-SPR2POS.mark, CH4-APPB-SPR3CTL.mark, "
            "CH4-APPB-SPR3POS.mark, CH4-APPB-SPR4CTL.mark, "
            "CH4-APPB-SPR4POS.mark, CH4-APPB-SPR5CTL.mark, "
            "CH4-APPB-SPR5POS.mark, CH4-APPB-SPR6CTL.mark, "
            "CH4-APPB-SPR6POS.mark, CH4-APPB-SPR7CTL.mark, "
            "CH4-APPB-SPR7POS.mark, APPA-sprpair.c2, APPA-sprpair.c3, "
            "CH4-appA-sprite.c12, CH4-appA-sprite.c13 . ");
    }

    // ------- SPRXDATA-SPRXDATB-DMA-PATH-ONLY-AT-THIS-BOUNDARY -------
    {
        // SPR0DATA/SPR0DATB: this module's DMA sequencer requests them
        // on its own during the sprite's active window (VSTART=40,
        // VSTOP=44 -- the same classify.evidence-sourced stimulus the
        // control case uses); it does NOT decode them for direct writes
        // at all, per classify.evidence ("agnus_spritedma does NOT decode
        // $144/$146 for writes at all (:205/220 accept only [2:1]==00/
        // 01)") -- so no processor-write stimulus is attempted here for
        // these two addresses; doing so would test nothing this module
        // can answer.
        Harness hd;
#if MUT_DATADATB
        write_pos(hd, 0, 0x20);  // MUTATION: VSTART=32 instead of 40.
#else
        write_pos(hd, 0, 0x28);
#endif
        write_ctl(hd, 0, 0x2C, false, false);
        Window wd = measure_window(hd, 0, 50);
        bool dma_side = wd.start == 40 && wd.end == 44;

        checks.expect(dma_side,
            "SPRXDATA-SPRXDATB-DMA-PATH-ONLY-AT-THIS-BOUNDARY: arming "
            "SPR0POS/SPR0CTL for VSTART=40/VSTOP=44 makes this module's "
            "own DMA sequencer request SPR0DATA ($144) and SPR0DATB "
            "($146) automatically on lines 40-43 (measured here as the "
            "DMA-active window [40,44), the same window the control case "
            "above validates) -- the DMA half of the '%' mark and of "
            "'usually loaded by the sprite image DMA channel' (p284) IS "
            "observable at this boundary and IS asserted here. This "
            "package's own classify.evidence records that the OTHER half "
            "of the same claim -- 'may be loaded by either processor' -- "
            "is a genuine SPLIT across two modules, not merely unobserved: "
            "'agnus_spritedma does NOT decode $144/$146 for writes at all "
            "... a processor write to SPRxDATA/SPRxDATB lands in Denise'. "
            "That half belongs to a different package on a different "
            "fixture (denise_sprites) and is a non-goal here, not "
            "silently assumed true or false. Frequency words "
            "('usually'/'at any time') are not asserted, per the same "
            "reasoning as SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-"
            "PROCESSOR-PATHS above. APPB2-SPR0DATA.mark, "
            "APPB2-SPR0DATB.mark, CH4-APPB-SPR0DATA.mark, "
            "CH4-APPB-SPR0DATB.mark, APPB2-SPR1DATA.mark, "
            "APPB2-SPR1DATB.mark, APPB2-SPR2DATA.mark, "
            "APPB2-SPR2DATB.mark, APPB2-SPR3DATA.mark, "
            "APPB2-SPR3DATB.mark, APPB2-SPR4DATA.mark, "
            "APPB2-SPR4DATB.mark, APPB2-SPR5DATA.mark, "
            "APPB2-SPR5DATB.mark, APPB2-SPR6DATA.mark, "
            "APPB2-SPR6DATB.mark, APPB2-SPR7DATA.mark, "
            "APPB2-SPR7DATB.mark, CH4-APPB-SPR1DATA.mark, "
            "CH4-APPB-SPR1DATB.mark, CH4-APPB-SPR2DATA.mark, "
            "CH4-APPB-SPR2DATB.mark, CH4-APPB-SPR3DATA.mark, "
            "CH4-APPB-SPR3DATB.mark, CH4-APPB-SPR4DATA.mark, "
            "CH4-APPB-SPR4DATB.mark, CH4-APPB-SPR5DATA.mark, "
            "CH4-APPB-SPR5DATB.mark, CH4-APPB-SPR6DATA.mark, "
            "CH4-APPB-SPR6DATB.mark, CH4-APPB-SPR7DATA.mark, "
            "CH4-APPB-SPR7DATB.mark, APPA-sprdat.c4, APPA-sprdat.c5, "
            "CH4-appA-sprite.c34, CH4-appA-sprite.c35 . ");
    }

    // ============================================================
    // ROUTED CLAIMS: the 14 ids described in the header's ROUTED CLAIMS
    // section -- 4 graded below, 10 recorded (see DISPOSITION there).
    // ============================================================

    // ------------- SPRXCTL-BITS-6-3-UNUSED-AT-ECS0 -------------
    {
        // Routing of CH4-p107.c8, CH4-sprctl.c6, CH4-p128.c14,
        // CH4-sctl.c13 (identical text on all four; the ids themselves are
        // recorded below): "Hold ecs=0 (this
        // RTL uses bits 6/5 as ECS V9 bits) and assert SPRxCTL with bits
        // 6..3 set leaves the reqdma/reg_address_out window identical to
        // bits clear; the package's probe already measures bits 6/5 at
        // ecs=0." Uses the same probe technique as the MEASURED note
        // above, graded here instead of reported.
        Harness h0(/*ecs=*/false);
        write_pos(h0, 0, 0x04);
        write_ctl(h0, 0, 0x08, false, false, false, false, false, false);
        Window w_clear = measure_window(h0, 0, 530);

#if MUT_BITS63
        Harness h1(/*ecs=*/true);  // MUTATION: ecs on for the bits-set side (was false).
#else
        Harness h1(/*ecs=*/false);
#endif
        write_pos(h1, 0, 0x04);
        write_ctl(h1, 0, 0x08, false, false, /*bit6=*/true, /*bit5=*/true, /*bit4=*/true, /*bit3=*/true);
        Window w_set = measure_window(h1, 0, 530);

        bool ok = w_clear.start != -1 && w_set.start != -1
               && w_clear.start == w_set.start && w_clear.end == w_set.end;
        checks.expect(ok,
            "SPRXCTL-BITS-6-3-UNUSED-AT-ECS0: with ecs held at 0 (this "
            "fixture's default), setting SPRxCTL bits 6-3 all at once "
            "(VSTART=4/VSTOP=8 fixed on the low byte) leaves the measured "
            "DMA-active window identical to bits 6-3 clear, matching "
            "'Bits 6-3 Unused (make zero)' (p107) and 'Bits 6-3 are "
            "reserved for future use (make zero)' (p128). This is NOT "
            "unfalsifiable across the whole span: bits 6 and 5 of it are "
            "genuinely live when ecs=1 (this package's own MEASURED note "
            "above), and the MUTATION below reproduces that by switching "
            "ecs on for the bits-set side alone, which turns this "
            "assertion red -- proving the comparator is not vacuous for "
            "that half. Bits 4 and 3 of the same span ARE unfalsifiable "
            "at this boundary: agnus_spritedma.v's SPRxCTL write reads "
            "only data_in[15:8],[6],[5],[2],[1] into sprctl -- bits 4 and "
            "3 reach no wire in this module under any ecs value, so no "
            "stimulus in this module's whole input space can make that "
            "half fail. Which bits 4/3 ARE -- appendix C's disputed "
            "SHSH1/SHSH0, ECS-only per its own note -- is the separate "
            "question SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED above declines; "
            "this assertion only answers whether they change this "
            "module's behaviour on OCS, which they provably do not. No "
            "claim id here -- see the record() below for the four ids "
            "naming the bundled bits-6-3 span, and why they are withdrawn "
            "rather than kept on this half-establishing line. ");
    }

    // The chapter-body ids naming "Bits 6-3 Unused" as one span bundle a
    // half this assertion cannot settle (bits 4/3) with a half it just
    // did (bits 6/5). A bundled claim is withdrawn whole, not kept on a
    // half-establishing assertion with a caveat in the prose.
    checks.record(
        "SPRXCTL-BITS-6-3-BUNDLED-SPAN-HALF-UNTESTED [declined]: "
        "CH4-p107.c8, CH4-sprctl.c6, CH4-p128.c14, CH4-sctl.c13 -- these "
        "four restate 'Bits 6-3 Unused (make zero)' (p107) and 'Bits 6-3 "
        "are reserved for future use (make zero)' (p128) as one bundled "
        "four-bit span. SPRXCTL-BITS-6-3-UNUSED-AT-ECS0 above establishes "
        "only bits 6 and 5 of that span -- the half this module's own "
        "SPRxCTL write actually reads (data_in[6],data_in[5], per "
        "agnus_spritedma.v:221, cited there). Bits 4 and 3 reach no wire "
        "in this module under any ecs value, so no stimulus at this "
        "boundary can make that half fail either way, and SPRxCTL is "
        "also a Denise register, where bits 4/3 (SHSH) would be "
        "observable -- this fixture has no Denise instance (see FIXTURE "
        "BOUNDARY above). Declined, not graded: asserting the whole "
        "span would credit a half this fixture cannot observe.");

    // ------------- SPRXCTL-CHG-IS-ECS-DEPENDENT -------------
    {
        // Routing of APPC2-SPRxCTL.v20 (the id itself sits on
        // SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS below): "The ecs input
        // is the one chipset switch here, and it changes what SPRxCTL
        // does ... promote the package's bits-6/5 probe to an assertion
        // that the same SPRxCTL value behaves differently at ecs=0 and
        // ecs=1, without claiming the SHSH change p303 names." Same probe
        // as the MEASURED note above (VSTART=4, SPR0CTL upper byte=8, bit
        // 6 set), graded here.
        auto probe = [](bool ecs) -> Window {
            Harness h(ecs);
            write_pos(h, 0, 0x04);
#if MUT_CHGECS
            write_ctl(h, 0, 0x08, false, false, /*bit6=*/false, false);  // MUTATION: bit6 withheld (was true).
#else
            write_ctl(h, 0, 0x08, false, false, /*bit6=*/true, false);
#endif
            return measure_window(h, 0, 530);
        };
        Window w_ecs0 = probe(false);
        Window w_ecs1 = probe(true);
        bool ok = w_ecs0.start != -1 && w_ecs1.start != -1 && w_ecs0.start != w_ecs1.start;
        checks.expect(ok,
            "SPRXCTL-CHG-IS-ECS-DEPENDENT: writing the SAME SPRxCTL value "
            "(bit 6 set, VSTART=4/VSTOP=8) measures a DIFFERENT DMA-active "
            "start line depending on this module's own ecs input (start=4 "
            "at ecs=0, start=516 at ecs=1, per the MEASURED note above) -- "
            "SPRxCTL demonstrably behaves differently under the Enhanced "
            "Chip Set, matching the ECS register map's own 'chg' "
            "classification for this row rather than 'new' (p298, '142  "
            "SPRxCTL  chg   W  A'). This asserts only that the behaviour "
            "changes with ecs, not that the change IS the SHSH1/SHSH0 "
            "reading appendix C p303 gives bits 4/3 -- that identification "
            "is exactly what SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED above "
            "declines to take a side on. Sprite 0 only, no claim id here "
            "-- the printed row this measures names 'SPRxCTL', the whole "
            "8-channel family, not sprite 0 alone, so its id lives on "
            "SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS below, which "
            "repeats this same ecs=0-vs-ecs=1 comparison for all 8 "
            "channels and is true under either reading of the row. ");
    }

    // ------- SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS -------
    {
        // Routing of APPC2-SPRxCTL.conf3 (the id itself is recorded
        // below): "sprctl is one bank of eight
        // under one ecs gate; write each of $142, $14A, ..., $17A with the
        // same ecs-sensitive value and assert every channel's window
        // changes with ecs, not sprite 0's only." Independent measurement,
        // own Harness instances, sprite indices 0-7.
        bool all_channels_changed = true;
        for (int n = 0; n < 8; ++n) {
            auto probe = [n](bool ecs) -> Window {
                Harness h(ecs);
                write_pos(h, n, 0x04);
#if MUT_ALLCHANNELS
                // MUTATION: only channel 0 ever gets bit6 set; channels
                // 1-7 always get it clear, so only channel 0 should show a
                // measured difference between ecs=0 and ecs=1.
                bool bit6 = (n == 0);
#else
                bool bit6 = true;
#endif
                write_ctl(h, n, 0x08, false, false, bit6, false);
                return measure_window(h, n, 530);
            };
            Window w0 = probe(false);
            Window w1 = probe(true);
            bool changed = w0.start != -1 && w1.start != -1 && w0.start != w1.start;
            if (!changed) all_channels_changed = false;
        }
        checks.expect(all_channels_changed,
            "SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS: writing the SAME "
            "ecs-sensitive SPRxCTL value (bit 6 set) to each of the 8 "
            "sprite channels' own SPRxCTL register in turn -- $142, $14A, "
            "$152, $15A, $162, $16A, $172, $17A -- and comparing its "
            "measured DMA-active start at ecs=0 against ecs=1 shows every "
            "one of the 8 channels' start line move, not sprite 0's "
            "($142's) alone: the printed map's single row 'chg' "
            "classification (p298, '142  SPRxCTL  chg   W  A') describes "
            "the whole indexed family sprctl[0..7] behind one shared ecs "
            "gate, matching the table's own 'x' wildcard convention used "
            "elsewhere on the same page for other indexed families (e.g. "
            "'050 BLTxPTH chg', '0A0 AUDxLCH chg') rather than a claim "
            "about the one printed address alone; the same all-8-channel "
            "evidence settles 'SPRxCTL ... changed' under either reading "
            "of whether 'chg' at $142 names one register or the whole "
            "family (see the record() below for why that reading "
            "question itself is not what this assertion grades). "
            "APPC2-SPRxCTL.v20 . ");
    }

    // The ambiguity claim itself, not a reading of it, is what
    // APPC2-SPRxCTL.conf3 states -- grading a resolution of an explicit
    // "recorded, not resolved" would pick a side the source declines to
    // pick.
    checks.record(
        "SPRXCTL-CHG-CHANGE-SCOPE-AMBIGUOUS [declined]: "
        "APPC2-SPRxCTL.conf3 -- \"Whether 'SPRxCTL chg' at one address "
        "means all eight control registers changed or only the one at "
        "$142 is not stated. Recorded, not resolved.\" This id IS the "
        "ambiguity record. SPRXCTL-CHG-APPLIES-TO-ALL-EIGHT-CHANNELS "
        "above already asserted the unqualified 'SPRxCTL ... changed' "
        "reading (graded: APPC2-SPRxCTL.v20), which is true under BOTH "
        "readings the claim declines to choose between -- not this id, "
        "which would mean grading one resolution of a stated ambiguity.");

    // ------------- SPRXPOS-SPRXCTL-JOINT-SIZE-ECS -------------
    // SPRXPOS-BYTE-IS-SV70-VSTART and SPRXCTL-BYTE-IS-EV70-VSTOP each run
    // at this fixture's default ecs=0, establishing only the OCS half of
    // "The ECS registers SPRxPOS and SPRxCTL work together as position,
    // size and sprite feature control registers." (appendix C p303, an
    // ECS-scoped source). This repeats both comparisons at Harness(ecs=1).
    {
        Harness ha(/*ecs=*/1);
        write_pos(ha, 0, 0x20);
        write_ctl(ha, 0, 0xFF, false, false);  // VSTOP far out, bits 6/5 clear.
        Window wa = measure_window(ha, 0, 50);

        Harness hb(/*ecs=*/1);
#if MUT_JOINTSIZEECS
        write_pos(hb, 0, 0x20);  // MUTATION: same byte as ha (was 0x21).
#else
        write_pos(hb, 0, 0x21);
#endif
        write_ctl(hb, 0, 0xFF, false, false);
        Window wb = measure_window(hb, 0, 50);
        bool vstart_governs_start_ecs1 =
            wa.start != -1 && wb.start != -1 && (wb.start - wa.start) == 1;

        Harness hc(/*ecs=*/1);
        write_pos(hc, 0, 0x02);  // VSTART early and fixed.
        write_ctl(hc, 0, 0x20, false, false);
        Window wc = measure_window(hc, 0, 50);

        Harness hd(/*ecs=*/1);
        write_pos(hd, 0, 0x02);
        write_ctl(hd, 0, 0x21, false, false);
        Window wd = measure_window(hd, 0, 50);
        bool vstop_governs_end_ecs1 =
            wc.end != -1 && wd.end != -1 && (wd.end - wc.end) == 1;

        checks.expect(vstart_governs_start_ecs1 && vstop_governs_end_ecs1,
            "SPRXPOS-SPRXCTL-JOINT-SIZE-ECS: at Harness(ecs=1) -- the ECS "
            "chip set, with SPRxCTL bits 6/5 held clear so the ECS-only "
            "high bits this package's own MEASURED note reports do not "
            "confound the comparison -- incrementing SPR0POS's byte by 1 "
            "moves the measured DMA-active start line by exactly 1, and "
            "independently incrementing SPR0CTL's byte by 1 moves the "
            "measured end line by exactly 1: SPRxPOS bounds the window's "
            "start and SPRxCTL bounds its end under ECS too, not only at "
            "ecs=0. Matches 'The ECS registers SPRxPOS and SPRxCTL work "
            "together as position, size and sprite feature control "
            "registers.' (appendix C p303). Position/size half only -- "
            "this boundary exposes no ATTACH/feature observable at either "
            "ecs value (see FIXTURE BOUNDARY above). CH4-appC-position.c6 . ");
    }

    // ------------- SPRXCTL-ECS-MAP-ROW-LIVE-AT-ECS1 -------------
    // SPRXPOS-SPRXCTL-ADDRESS-MAP and AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-
    // AND-SPRXCTL each measure $142's request and write-target status only
    // at this fixture's default ecs=0, but appendix C p298 is the ECS
    // register map -- an ECS-scoped source -- and this module's ecs input
    // changes what a $142 write does (bits 6/5). Repeat both the request
    // side and the write side at Harness(ecs=1).
    {
        Harness he(/*ecs=*/1);
        std::set<int> seen_ecs1 = sweep_prefetch_addresses(he, 1, 30);
        bool addr_requested_ecs1 = seen_ecs1.count(0x142) != 0;

        Harness hw1(/*ecs=*/1);
        write_pos(hw1, 0, 0x04);
        write_ctl(hw1, 0, 0x10, false, false);
        Window ww1 = measure_window(hw1, 0, 40);

        Harness hw2(/*ecs=*/1);
        write_pos(hw2, 0, 0x04);
#if MUT_ECSMAPROW
        write_ctl(hw2, 0, 0x10, false, false);  // MUTATION: same byte as hw1 (was 0x14).
#else
        write_ctl(hw2, 0, 0x14, false, false);
#endif
        Window ww2 = measure_window(hw2, 0, 40);
        bool write_reaches_agnus_ecs1 = ww1.end != -1 && ww2.end != -1 && ww1.end != ww2.end;

        checks.expect(addr_requested_ecs1 && write_reaches_agnus_ecs1,
            "SPRXCTL-ECS-MAP-ROW-LIVE-AT-ECS1: at Harness(ecs=1), this "
            "module's own DMA sequencer still requests $142 on its own "
            "(re-swept here from a fresh, unwritten DUT), and a direct "
            "reg_address_in/data_in write to $142 still changes this "
            "module's measured behaviour (re-measured here, independent "
            "instances) -- the ECS register map's row '142  SPRxCTL  chg "
            "  W  A  Sprite x position and control data' (appendix C "
            "p298) is real at the chip set the map itself is scoped to, "
            "not only at this fixture's default ecs=0; the ecs=0 "
            "measurements of SPRXPOS-SPRXCTL-ADDRESS-MAP and AGNUS-IS-"
            "WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL above are not ECS "
            "measurements of this ECS-scoped row. "
            "APPC2-SPRxCTL.addr, APPC2-SPRxCTL.acc . ");
    }

    checks.record(
        "SPRXCTL-CHIP-COLUMN-DENISE-ECS-QUALIFIER-CONFLICT [declined]: "
        "APPB2-conf.c1, APPB2-conf.c2 -- SPR0CTL's chip column reads "
        "'A D(E)' (p291) while SPR1CTL-SPR7CTL read plain 'A D' (p292), "
        "appendix A page 284 gives the generic SPRxCTL as 'A D( E )', and "
        "appendix C page 303 gives it as 'A D' -- four readings of one "
        "column, the Denise/ECS-qualifier half of the same conflict "
        "AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL above already "
        "leaves undisputed-but-untested for the undisputed Agnus half. "
        "This fixture has no Denise instance (see FIXTURE BOUNDARY above), "
        "so neither reading can be graded here; both are recorded per "
        "classify.package's own instruction ('record both readings, "
        "assert neither').");

    checks.record(
        "SPRXCTL-CHIP-COLUMN-FIVE-WAY-CONFLICT [declined]: "
        "APPC2-SPRxCTL.conf1, APPC2-SPRxCTL.chip -- the ECS register "
        "map's chip column for SPRxCTL (p298, 'A' alone) against "
        "appendix A p284 ('A D( E )'), appendix B p291 ('A D(E)' for "
        "SPR0CTL), appendix B p292 ('A D' for SPR1CTL-SPR7CTL) and "
        "appendix C p303 body text ('A D') -- five readings of the same "
        "column, recorded not resolved. AGNUS-IS-WRITE-TARGET-FOR-"
        "SPRXPOS-AND-SPRXCTL above establishes only that Agnus reaches "
        "$142 and changes the vertical compare -- the undisputed 'A' -- "
        "not that it reaches $142 alone: 'alone' is exactly the disputed "
        "side of this five-way conflict, so APPC2-SPRxCTL.chip's own "
        "claim ('SPRxCTL's chip column in the ECS register map reads A "
        "alone') is declined here in full, not graded. The Denise-side "
        "decode is reported in this package's own classify.evidence "
        "(denise_sprites.v:104, denise_sprites_shifter.v:138-141 decode "
        "$142 too) -- not something this fixture, which builds "
        "agnus_spritedma alone with no Denise instance, can itself "
        "measure.");

    checks.record(
        "SPR0POS-PCT-MARK-FREQUENCY [declined]: APPB2-conf.c5 -- the '%' "
        "mark on SPR0POS (p291), chapter page 128 ('normally only written "
        "by the sprite DMA channel itself') and appendix A page 284 ('may "
        "be loaded by either processor at any time') all name a RELATIVE "
        "FREQUENCY for the same dual DMA/processor write path "
        "SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS above "
        "already asserts the structural half of. Per this package's own "
        "classify.evidence, no port at this boundary carries how often a "
        "write happens; the three sources agree in substance and none is "
        "assertable, so all three are recorded rather than graded, per "
        "classify.conflict_resolution's own instruction.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
