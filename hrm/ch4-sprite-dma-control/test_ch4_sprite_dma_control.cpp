// Test purpose: DMACON ($096) and DMACONR ($002) as chapter 4's
//   sprite-facing view of DMA control - SPREN (sprite DMA enable, bit 5),
//   the master DMAEN (bit 9), the DMACONR read-back register, and which bits
//   a write can and cannot reach (DMACON's write-only address; DMACONR's
//   read-only address; BBUSY/BZERO's read-only bits within it).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 278 (Appendix A custom register table). Every claim below cites
//   page 278; none of this package's claims cite any other page.
// Non-goal: reproducing hrm/ch7-dmacon-register-bits. That package already builds
//   this exact fixture (`agnus`, the full chipset top level) and already
//   measures most of the per-bit and register-mechanics propositions this
//   package's claim set also names - see COVERAGE ALREADY MEASURED BY
//   hrm/ch7-dmacon-register-bits below. This package's own assertions are only the
//   propositions that package does not measure.
// Non-goals: anything outside the `agnus` fixture this package builds, and
//   any claim it does not name. A green here means those assertions held on
//   this fixture at the pinned revision -- not that the module is correct,
//   and not that the chapter is covered.
// Claim ids: ids such as `APPA-dmacon.c1` and `APPA-dmacon.c3` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch4.json lists under `inventories`.
//
// BOUNDARY: `agnus` (top-level chipset module) - the only fixture this
//   package's claims classify as constructible. Per every claim's own
//   classify.evidence: "the agnus fixture is not constructible from this
//   rtl/ slice -- verilator --lint-only --top-module agnus rtl/*.v reports 7
//   MODMISSING (agnus_refresh, agnus_diskdma, agnus_audiodma,
//   agnus_bitplanedma, agnus_copper, agnus_blitter, agnus_beamcounter); the
//   existing package agnus_top_dmacon builds it in the full tree." This
//   package's Makefile is agnus_top_dmacon's Makefile with only TESTBENCH
//   changed, for exactly that reason.
//
// PORTS (from the claims' own evidence fields, handed to the building agent
//   as part of the task inventory - NOT discovered by opening agnus.v):
//   in  - aen, rd, hwr, lwr, address_in, data_in
//   out - data_out (DMACONR read-back path: agnus.v:281 drives dmaconr,
//         agnus.v:277 puts it on data_out)
//   DMACON write path: aen/hwr/lwr/address_in/data_in reaches the `dmacon`
//   register at agnus.v:289-292. dmacon is 13 bits wide (bits 12:0 only) -
//   DMACONR's bits 14/13 (BBUSY/BZERO) are not stored bits of it at all.
//   The enable bits' downstream *function* (dbr/refresh/dbwe/address_out/
//   reg_address_out via the arbiter, agnus.v:171-262) is this package's
//   NON-goal per the coverage note above - every claim whose proposition is
//   a function (not a register-mechanics fact) is already measured in
//   hrm/ch7-dmacon-register-bits.
//
// ============================================================================
// CLAIMS (verbatim quote + restatement, from
// work/ch4_packages/ch4-dmacon-bits.jsonl, package "ch4-dmacon-bits", all 45
// records, classify.status == "testable", classify.observable == "yes, at
// agnus"). Two readers (opus, sol) recorded most of these sentences
// independently - grouped below by proposition; both ids are cited together
// wherever they name the same sentence.
//
// -- DMACON overview: address $096, write-only, chip partition (A D P),
//    "DMA control write (clear or set)" --
//   APPA-dmacon.c1 (opus): quote "DMACON     096      W     A D P   DMA
//     control write (clear or set)" -> "DMACON is at address $096, is
//     write-only, is implemented in Agnus, Denise and Paula, and is the DMA
//     control write register with clear-or-set semantics."
//   CH4-appA-dmacon.c1 (sol): same quote -> "DMACON is at custom-register
//     offset $096."
//   CH4-appA-dmacon.c2 (sol): same quote -> "DMACON is write-only."
//   CH4-appA-dmacon.c3 (sol): same quote -> "DMACON is implemented across
//     the Agnus, Denise, and Paula custom-chip functions." (classify.split:
//     "Agnus third -> ch4-dmacon-bits; Denise and Paula thirds are not
//     constructible in this rtl/ slice and are a chip-partition statement,
//     not a behaviour" - grep of denise.v finds no $096 decode at all, and
//     paula.v is not in this rtl/ slice, so only the Agnus third is
//     assertable here.)
//   CH4-appA-dmacon.c4 (sol): same quote -> "DMACON writes either set or
//     clear selected DMA-control bits."
//   APPA-dmacon.c3 (opus): quote "This register controls all of the DMA
//     channels and" -> "DMACON controls all of the DMA channels and
//     contains blitter DMA status bits." (classify.evidence explicitly
//     grounds the write-only-ness claim on this same sentence: "nothing
//     sources $096 onto data_out (:277), so both the address and the
//     write-only-ness are assertable at the agnus boundary.")
//
// -- DMACONR overview: address $002, read-only, chip partition (A P),
//    "DMA control (and blitter status) read" --
//   APPA-dmacon.c2 (opus): quote "DMACONR    002      R     A   P   DMA
//     control (and blitter status) read" -> "DMACONR is at address $002, is
//     read-only, is implemented in Agnus and Paula, and reads DMA control
//     and blitter status." (classify.evidence: "Existing assertion
//     DMACONR-READ-DECODED-AT-002 in package agnus_top_dmacon" - the
//     address-decode-for-read half; see coverage note below.)
//   CH4-appA-dmacon.c5 (sol): same quote -> "DMACONR is at custom-register
//     offset $002."
//   CH4-appA-dmacon.c6 (sol): same quote -> "DMACONR is read-only."
//   CH4-appA-dmacon.c7 (sol): same quote -> "DMACONR is implemented by
//     Agnus and Paula." (classify.split: "Agnus half -> ch4-dmacon-bits;
//     Paula half outside this slice.")
//   CH4-appA-dmacon.c8 (sol): same quote -> "Reading DMACONR returns
//     DMA-control state and blitter status."
//   CH4-appA-dmacon.c9 (sol): quote "This register controls all of the DMA
//     channels and\n                 contains blitter DMA status bits." ->
//     "The DMACON/DMACONR register node controls all DMA-channel enables."
//   CH4-appA-dmacon.c10 (sol): same quote -> "The DMACON/DMACONR register
//     node also reports blitter DMA status."
//
// -- SET/CLR write semantics (bit 15; "bits written 0 stay unchanged") --
//   APPA-dmacon.c4 (opus): quote "15    SET/CLR     Set/clear control bit.
//     Determines" -> "DMACON bit 15 is SET/CLR, the set/clear control bit,
//     which determines whether bits written with a 1 get set or cleared."
//   APPA-dmacon.c5 (opus): quote "cleared.  Bits written with a zero" ->
//     "In a DMACON write, bits written with a zero are unchanged."
//   CH4-appA-dmacon.c11 (sol): quote "15    SET/CLR     Set/clear control
//     bit. Determines\n if bits written with a 1 get set or\n cleared." ->
//     "DMACON bit 15 selects whether written one bits set or clear their
//     corresponding controls."
//   CH4-appA-dmacon.c12 (sol): quote "Bits written with a zero\n are
//     unchanged." -> "A zero in a DMACON write leaves the corresponding
//     control bit unchanged."
//   Both readers' classify.evidence for all four: "agnus.v:291-292 -- if
//     (data_in[15]) dmacon |= data_in[12:0]; else dmacon &= ~data_in[12:0],
//     which is exactly set/clear with zero bits unchanged, and it is
//     visible through DMACONR at $002 (:281)." No "Existing assertion" tag
//     in these four - but this agent's own reading of
//     hrm/ch7-dmacon-register-bits's WALKING-BIT-* loop (SET from zero, then CLR
//     from an all-13-bits-set baseline, for every bit 0-12) and its
//     SETCLR-MULTIBIT-DIRECTION assertion (an explicit SET-then-partial-CLR
//     of a 3-bit pattern) shows this exact mechanism is already exercised
//     there, generically, across the whole register. See coverage note.
//
// -- Per-bit position/name, bits 0-10 (the six agnus consumes: 5,6,7,8,9,10;
//    the five it only stores/reads back: 0,1,2,3,4) --
//   APPA-dmacon.c16/CH4-appA-dmacon.c23: "05 SPREN Sprite DMA enable" (bit 5)
//   APPA-dmacon.c15/CH4-appA-dmacon.c22: "06 BLTEN Blitter DMA enable" (bit 6)
//   APPA-dmacon.c14/CH4-appA-dmacon.c21: "07 COPEN Copper DMA enable" (bit 7)
//   APPA-dmacon.c13/CH4-appA-dmacon.c20: "08 BPLEN Bitplane DMA enable" (bit 8)
//   APPA-dmacon.c12/CH4-appA-dmacon.c19: "09 DMAEN Enable all DMA below" (bit 9)
//   APPA-dmacon.c10/CH4-appA-dmacon.c17: "10 BLTPRI Blitter DMA priority" (bit 10)
//   APPA-dmacon.c11/CH4-appA-dmacon.c18: BLTPRI disables /BLS (function)
//   APPA-dmacon.c17/CH4-appA-dmacon.c24: "04 DSKEN Disk DMA enable" (bit 4)
//   APPA-dmacon.c18/CH4-appA-dmacon.c25: "03 AUD3EN Audio channel 3 DMA
//     enable" (bit 3)
//   APPA-dmacon.c19/CH4-appA-dmacon.c26: "02 AUD2EN Audio channel 2 DMA
//     enable" (bit 2)
//   APPA-dmacon.c20/CH4-appA-dmacon.c27: "01 AUD1EN Audio channel 1 DMA
//     enable" (bit 1)
//   APPA-dmacon.c21/CH4-appA-dmacon.c28: "00 AUD0EN Audio channel 0 DMA
//     enable" (bit 0)
//   Bits 4-0's own classify.evidence: "The ENABLE half is not in this tree's
//     Agnus at all ... disk and audio requests arrive already gated from
//     Paula on the disk_dmal / audio_dmal inputs ... Do not write an enable
//     assertion for this bit here." Only the storage-and-read-back half of
//     these five is in scope for `agnus` at all - see coverage note, this
//     half is already measured.
//
// -- BBUSY (bit 14) and BZERO (bit 13), read-only --
//   APPA-dmacon.c6/CH4-appA-dmacon.c13: "14 BBUSY Blitter busy status bit
//     (read only)"
//   APPA-dmacon.c7/CH4-appA-dmacon.c14: "13 BZERO Blitter logic zero status
//     bit (read only)"
//   classify.evidence (both): "DMACONR bit 14 is blit_busy and bit 13 is
//     blit_zero (agnus.v:281), while the writable register dmacon is only
//     13 bits wide (agnus.v:271) and takes only data_in[12:0]
//     (agnus.v:291-292), so neither bit has a write path ... Existing
//     assertions BBUSY-READONLY-WITH-POSITIVE-CONTROL and
//     BZERO-READONLY-WITH-POSITIVE-CONTROL in package agnus_top_dmacon."
//
// That is all 45 records in ch4-dmacon-bits.jsonl. Note what is absent: page
// 278's own bit table skips 12 and 11 (its rows run 15,14,13,10,9,...,0) -
// there is no quote for either position, so no claim for either exists in
// this package's inventory. Per this task's own instruction, that silence is
// classified `insufficient`, not asserted. dmacon IS 13 bits wide
// (agnus.v:271, per the evidence above), so bits 11/12 exist and take a
// write/read-back like any other position - hrm/ch7-dmacon-register-bits's
// WALKING-BIT-11 and WALKING-BIT-12 already measure exactly that (SET from
// zero, CLR from all-ones, same as every other bit), labelled "bit11(X)" /
// "bit12(X)" - no function claimed, matching page 278's silence. This
// package adds nothing about bits 11/12: there is no proposition to assert,
// and the generic SET/CLR mechanics they do have are already measured
// elsewhere. Measured, not asserted - reported in this package's own build
// report, not invented here.
//
// ============================================================================
// COVERAGE ALREADY MEASURED BY hrm/ch7-dmacon-register-bits (read in full before
// writing anything below - its Makefile is copied above with only TESTBENCH
// changed, and its Harness/write_reg/read_reg pattern, forced on this
// fixture by the SAME arbitrated-register-bus fact its own IMPLEMENTATION
// NOTES document, is reused below). Per this task's own instruction: say so,
// do not duplicate. Grouped by the existing assertion that already measures
// each claim's proposition:
//
//   SPREN-GATES-SPRITE-DMA (bit 5 function) + WALKING-BIT-5-SPREN (position):
//     APPA-dmacon.c16, CH4-appA-dmacon.c23.
//   BLTEN-GATES-BLITTER-DMA + WALKING-BIT-6-BLTEN:
//     APPA-dmacon.c15, CH4-appA-dmacon.c22.
//   COPEN-GATES-COPPER-DMA + WALKING-BIT-7-COPEN:
//     APPA-dmacon.c14, CH4-appA-dmacon.c21.
//   BPLEN-GATES-BITPLANE-DMA + WALKING-BIT-8-BPLEN:
//     APPA-dmacon.c13, CH4-appA-dmacon.c20.
//   DMAEN-MASTER-GATES-ALL-DMA-BELOW + WALKING-BIT-9-DMAEN:
//     APPA-dmacon.c12, CH4-appA-dmacon.c19.
//   BLTPRI-BUS-SHARE-UNDER-CONTENTION (function) + WALKING-BIT-10-BLTPRI
//   (position):
//     APPA-dmacon.c10, APPA-dmacon.c11, CH4-appA-dmacon.c17,
//     CH4-appA-dmacon.c18.
//   WALKING-BIT-4-DSKEN (storage/read-back only, per this package's own
//   claims' instruction not to assert an enable function for it here):
//     APPA-dmacon.c17, CH4-appA-dmacon.c24.
//   WALKING-BIT-3-AUD3EN: APPA-dmacon.c18, CH4-appA-dmacon.c25.
//   WALKING-BIT-2-AUD2EN: APPA-dmacon.c19, CH4-appA-dmacon.c26.
//   WALKING-BIT-1-AUD1EN: APPA-dmacon.c20, CH4-appA-dmacon.c27.
//   WALKING-BIT-0-AUD0EN: APPA-dmacon.c21, CH4-appA-dmacon.c28.
//   BBUSY-READONLY-WITH-POSITIVE-CONTROL:
//     APPA-dmacon.c6, CH4-appA-dmacon.c13.
//   BZERO-READONLY-WITH-POSITIVE-CONTROL:
//     APPA-dmacon.c7, CH4-appA-dmacon.c14.
//   WALKING-BIT-* (all 13 positions, SET-from-zero/CLR-from-all-ones) +
//   SETCLR-MULTIBIT-DIRECTION (explicit SET-then-partial-CLR) together
//   already exercise the SET/CLR mechanism (bit 15's role; "bits written 0
//   stay unchanged") generically, across the whole register:
//     APPA-dmacon.c4, APPA-dmacon.c5, CH4-appA-dmacon.c11,
//     CH4-appA-dmacon.c12.
//
// 24 of this package's 45 claim ids are covered this way. The remaining 21
// are this package's own job - see PLANNED ASSERTIONS. None of the 24 above
// get a duplicate assertion in this file; this file's own report will list
// them as "not asserted here - already measured by hrm/ch7-dmacon-register-bits:
// <assertion name>", per this task's instruction to say so rather than
// re-test them.
//
// ============================================================================
// PLANNED ASSERTIONS (names, cited-id groups and intent only - no RTL line
// numbers beyond what the claims' own classify.evidence already handed over;
// no fixture code below this comment block, matching hrm/ch7-dmacon-register-bits's
// own spec-before-RTL gate. Added in the next commit, after agnus.v is
// opened to confirm the write_reg/read_reg pattern copied from
// hrm/ch7-dmacon-register-bits still applies unchanged - that package's own
// IMPLEMENTATION NOTES already document why the retry-on-pre-edge-dbr shape
// is required, not merely convenient, for this arbitrated register bus):
//
//   1. DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER: this package's control
//      case, expected to PASS, and its fixture-delivers-a-known-word proof.
//      Three DIFFERENT DMACON patterns are written in sequence (not just
//      the final state) and DMACONR is read back and checked after EACH
//      one, confirming each arrives distinct and in the order it was
//      written - not a stale or latched value from a previous step. Cites
//      the DMACON overview group (APPA-dmacon.c1, APPA-dmacon.c3,
//      CH4-appA-dmacon.c1, CH4-appA-dmacon.c2, CH4-appA-dmacon.c3,
//      CH4-appA-dmacon.c4), the DMACONR overview group (APPA-dmacon.c2,
//      CH4-appA-dmacon.c5, CH4-appA-dmacon.c6, CH4-appA-dmacon.c7,
//      CH4-appA-dmacon.c8, CH4-appA-dmacon.c9, CH4-appA-dmacon.c10) and the
//      SET/CLR group (APPA-dmacon.c4, APPA-dmacon.c5,
//      CH4-appA-dmacon.c11, CH4-appA-dmacon.c12) - the sequence's own
//      SET-then-partial-CLR step is a direct, freshly-measured
//      demonstration of "bits written 0 stay unchanged" under this
//      package's own ids, alongside (not instead of) the coverage already
//      noted above under hrm/ch7-dmacon-register-bits's WALKING-BIT-*/
//      SETCLR-MULTIBIT-DIRECTION.
//
//   2. DMACON-IS-WRITE-ONLY-AT-096: after a write through $096, a READ
//      attempted at $096 itself (aen/rd/address_in=$096) must NOT reflect
//      the pattern just written - "nothing sources $096 onto data_out"
//      (every claim's own classify.evidence) - while the SAME state read
//      through $002 (DMACONR) in the SAME run DOES reflect it, the paired
//      control on the same measurement basis. Cites APPA-dmacon.c1,
//      APPA-dmacon.c3, CH4-appA-dmacon.c1, CH4-appA-dmacon.c2,
//      CH4-appA-dmacon.c3.
//
//   3. DMACONR-IS-READ-ONLY-AT-002: a write ATTEMPTED at $002 (DMACONR's
//      own address, not $096) must NOT change what a later read at $002
//      returns, while a real write through $096 immediately afterward, in
//      the SAME run, DOES change it - the paired control proving the write
//      path itself is live and the $002 attempt's silence is not a dead
//      harness. Cites APPA-dmacon.c2, CH4-appA-dmacon.c5,
//      CH4-appA-dmacon.c6, CH4-appA-dmacon.c7.
//
// Three planned assertions. Together with the 24 ids covered by
// hrm/ch7-dmacon-register-bits (noted above) and these three assertions' own
// citations, every one of the 45 ids in ch4-dmacon-bits.jsonl is either
// covered elsewhere (named above) or cited on one of these three - none are
// being left silently uncovered.
//
// No RTL has been opened yet to write this comment block: the port names
// (aen, rd, hwr, lwr, address_in, data_in, data_out) and the register-
// mechanics facts (dmacon is 13 bits wide, data_in[15] selects set/clear,
// DMACON=$096 write-only, DMACONR=$002 read-only, bits 14/13 are a live mux
// of blit_busy/blit_zero and not stored bits of dmacon at all) come from
// this package's own claims' classify.evidence fields and from
// hrm/ch7-dmacon-register-bits's already-committed file (both handed to or
// available to the building agent as inventory, not discovered by this
// agent opening agnus.v). Exact confirmation of the write_reg/read_reg retry
// shape against this fixture, and the mutation log, are answered in the next
// commit, after agnus.v is read.
// ============================================================================

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after agnus.v was opened for
// the first time - the specification comment above this point was committed
// on its own first, per the chapter's spec-before-RTL gate; see this
// package's own git history, commit 59c9b46).
//
// agnus.v confirms every fact the claims' own classify.evidence already
// handed over, at the exact lines cited there: `localparam DMACON = 9'h096`
// (:103), `localparam DMACONR = 9'h002` (:104), `reg [12:0] dmacon` (:271,
// 13 bits), `assign data_out = data_bmc | dmaconr | data_blt` (:277),
// DMACONR's read mux - `if (reg_address[8:1]==DMACONR[8:1]) dmaconr[15:0] =
// {1'b0, blit_busy, blit_zero, dmacon[12:0]}; else dmaconr = 0` (:281-282),
// and DMACON's write - `else if (reg_address[8:1]==DMACON[8:1]) ... if
// (data_in[15]) dmacon[12:0] <= dmacon[12:0] | data_in[12:0]; else
// dmacon[12:0] <= dmacon[12:0] & ~data_in[12:0]` (:289,291-292). Nothing
// here was a surprise against the inventory; opening the file confirmed the
// spec above rather than revising it.
//
// Two things follow directly from that read-mux shape that the three
// assertions below depend on:
//   - DMACON-IS-WRITE-ONLY-AT-096: `dmaconr` is the ONLY driver of
//     `data_out` that ever depends on `reg_address` matching anything
//     related to `dmacon`'s storage, and its own `if` (:281) only fires for
//     DMACONR[8:1] ($002[8:1]), not DMACON[8:1] ($096[8:1]) - the two
//     addresses differ in bits [8:1] (0x096>>1=0x4B vs 0x002>>1=0x01), so a
//     read at $096 takes dmaconr's `else` arm (:282, dmaconr=0) and neither
//     data_bmc nor data_blt (the other two OR terms, sourced from
//     agnus_bmc/agnus_blitter's own register windows, not DMACON's address)
//     has any reason to decode $096 either - data_out is 0 at that address,
//     not merely "not equal to the pattern written".
//   - DMACONR-IS-READ-ONLY-AT-002: the write's `else if` (:289) tests
//     DMACON[8:1], not DMACONR[8:1] - a write with address_in=$002 (i.e.
//     $002>>1=0x001) fails that comparison against DMACON[8:1]=0x4B and
//     never reaches the `dmacon[12:0] <= ...` assignment at all, regardless
//     of data_in or the SET/CLR bit.
//
// The write_reg/read_reg retry-on-pre-edge-dbr shape (copied unchanged
// below from hrm/ch7-dmacon-register-bits's Harness) is still the correct way to
// drive this fixture: the write at :289 is gated by the SAME `reg_address`
// wire the arbiter overwrites on every cycle some DMA engine holds the bus
// (agnus.v's priority mux, documented in hrm/ch7-dmacon-register-bits's own
// IMPLEMENTATION NOTES), and this package never sets any DMA engine's own
// enable bit as its own subject, but agnus_refresh's unconditional periodic
// pulse still contends for the bus regardless of any DMACON bit - the retry
// loop is what makes a register access land on the first available
// non-refresh cycle instead of assuming one is always immediately free.
//
// MUTATION LOG: see the bottom of this file, after main(). Every entry was
// actually applied - edited into this file, rebuilt under WSL/Verilator
// (`make test`), run, the PASS/FAIL line read from real output, then
// reverted and rebuilt clean before the next entry.
// ============================================================================

// ============================================================================
// EXTENSION -- twelve new propositions (all classify.status=="testable"),
// written from quotations and restatements alone, without the evidence
// field.
//
// SOURCE PAGES: printed pp.114-115 (Chapter 4, "Creating Additional
// Sprites," its boxed "About sprite DMA." note and its "Warning:" callout)
// and printed p.303 (Appendix C, ECS chip-differences table, the SPRxPOS/
// SPRxCTL row).
//
// -- Group 1 (4 ids): SPREN is a single, chip-wide enable -- enabling
//    sprite DMA for one sprite enables DMA for every sprite, not just the
//    one named, and keeps every channel running without a further step --
//   CH4-more.c4 (opus, p114): quote "When you enable sprite DMA for one
//     sprite, you enable DMA for all the" -> "Enabling sprite DMA for one
//     sprite enables DMA for all the sprites."
//   CH4-more.c5 (opus, p114): quote "sprites and place them all in
//     automatic mode. Thus, you do not need" -> "Enabling sprite DMA places
//     all the sprites in automatic mode."
//   CH4-p114.c1 (sol, p114): quote (restored) "When you enable sprite DMA
//     for one sprite, you enable DMA for all the sprites and place them all
//     in automatic mode." -> "The SPREN control enables sprite DMA globally
//     for all eight sprite channels."
//   CH4-p114.c2 (sol, p114): same restored quote -> "Enabling sprite DMA
//     places every sprite channel in automatic mode."
//   All four read the SAME sentence (p114's "About sprite DMA." box), split
//   the same way by both readers: a "gates every channel" half
//   (CH4-more.c4 / CH4-p114.c1) and an "automatic mode" half (CH4-more.c5 /
//   CH4-p114.c2). hrm/ch7-dmacon-register-bits's own SPREN-GATES-SPRITE-DMA
//   already shows SPREN gates sprite channel 0's DMA cycles (under ITS OWN
//   ids, none of these four) -- planned assertion
//   SPREN-GATES-EVERY-SPRITE-CHANNEL below extends that to a second and a
//   third channel (sprite 1 and sprite 7 -- SPR1PTH/PTL/POS/CTL =
//   $124/$126/$148/$14A, SPR7PTH/PTL/POS/CTL = $13C/$13E/$178/$17A, both
//   from Appendix A/B's own register table, p.291-292, not from agnus.v),
//   each independently held clear then set, to show it is the SAME one bit
//   -- not a per-channel one -- that gates a channel other than sprite 0.
//   "Automatic mode" (printed p.113: a sprite whose data is "reread" from
//   its data structure "for each display field" with no further action) is
//   the channel autonomously continuing to request its own DMA cycles once
//   armed -- the same observable this assertion already measures over a
//   40-line window with no CPU write after the initial pointer/POS/CTL
//   setup. CH4-more.c5 and CH4-p114.c2 are cited on that same assertion,
//   not withdrawn: what they assert (DMA continuing to run the channel
//   without repeating the enable step) is exactly what a sustained,
//   multi-line measurement with a single SPREN write shows.
//
// -- Group 2 (6 ids): additional display DMA demand can leave some sprites
//    unusable --
//   CH4-more.c8 (opus, p114): quote "Remember that some sprites can become
//     unusable when additional DMA cycles" -> "Some sprites can become
//     unusable when additional DMA cycles are allocated to displaying the
//     screen."
//   CH4-more.c9 (opus, p114): quote "are allocated to displaying the
//     screen, for example when an extra" -> "Enabling an extra wide display
//     is one case that allocates additional DMA cycles and can make some
//     sprites unusable."
//   CH4-more.c10 (opus, p114): quote "wide display or horizontal scrolling
//     is enabled (see" -> "Enabling horizontal scrolling is one case that
//     allocates additional DMA cycles and can make some sprites unusable."
//   CH4-p114.c5 (sol, p114): quote (restored) "Remember that some sprites
//     can become unusable when additional DMA cycles are allocated to
//     displaying the screen, for example when an extra wide display or
//     horizontal scrolling is enabled (see Figure 6-9: DMA Time Slot
//     Allocation)." -> "Additional display DMA demand, including
//     extra-wide display or horizontal scrolling, can consume sprite DMA
//     slots and make some channels unavailable."
//   CH4-warn.c1 (opus, p115): quote "Some sprites become unusable when
//     additional DMA cycles are allocated" -> "Some sprites become unusable
//     when additional DMA cycles are allocated to displaying the screen."
//   CH4-warn.c2 (opus, p115): quote "to displaying the screen, e.g. when
//     enabling an extra wide display or" -> "Enabling an extra wide display
//     is one such case." (corpus-truncated after "or"; printed p115
//     continues "horizontal scrolling. (See Figure 6-11: DMA Time Slot
//     Allocation.)", per the classification's own note.)
//   p.114's "Remember that..." paragraph and p.115's boxed "Warning:" are
//   the SAME sentence printed twice (once inline, once as a callout,
//   citing Figure 6-9 and Figure 6-11 respectively for what appears to be
//   the same figure content) -- six ids, one proposition. The manual's own
//   cross-reference names the mechanism as a fixed per-line DMA time-slot
//   map, not a probabilistic one: widening the active display (more
//   bitplane fetch demand, or a horizontal-scroll DDF extension) competes
//   for the same bus this package's own DMACON-gated engines share. Whether
//   that competition is OBSERVABLE at this package's own `agnus` boundary
//   -- by holding SPREN and one sprite channel active, then comparing its
//   own attributable DMA-cycle count with BPLEN/a wide DDF window clear
//   against the same count with it driven wide -- is a question for
//   agnus.v's own arbiter (agnus.v:171-262, already named as this
//   package's downstream-function non-goal for the DMACON-bits reading,
//   never previously tried for a competing-demand reading) and is answered
//   below, after that file is reopened, not assumed here. If no port
//   attributes a granted cycle to "this sprite's own register" distinctly
//   from "the bitplane engine's own fetch," or the arbiter's fixed
//   priority order makes the outcome known without a live measurement
//   either way, this group is recorded, not asserted, and the reason given
//   is the concrete one found in agnus.v, not this paragraph's forecast of
//   it.
//
// -- Group 3 (2 ids): SPRxPOS/SPRxCTL "usually loaded by [DMA] ... during
//    horizontal blank" --
//   APPC-pos.c5 (opus, p303, Appendix C, ECS): quote (restored) "They are
//     usually loaded by the sprite DMA channel, during horizontal blank,
//     however they may be loaded by the processor." -> "SPRxPOS and
//     SPRxCTL are usually loaded by the sprite DMA channel during
//     horizontal blank."
//   CH4-appC-position.c8 (sol, p303, Appendix C, ECS): same quote ->
//     "On ECS automatic mode, sprite DMA normally loads SPRxPOS and
//     SPRxCTL during horizontal blanking."
//   A sibling package on the same underlying module,
//   hrm/ch4-sprite-vertical-position (already committed), read this exact
//   family of claims for SPRxDATA/SPRxDATB and recorded, from its own
//   classify.evidence, that "no port carries how often a write happens"
//   and that the DMA slot window this module produces is "hpos-gated ...,
//   not blank-gated" -- frequency and blanking-correspondence words
//   ("usually," "during horizontal blank") are accordingly never asserted
//   in that file, only the structural fact that a DMA path (and,
//   separately, a processor-write path) exists. Both of these two ids' own
//   restatements bundle that same timing qualifier INTO the one
//   proposition being graded ("usually ... during horizontal blank[ing]"),
//   not as a separate claim beside it -- so even if the structural half
//   were reproved here, the id could not ride on it: an id whose claim
//   bundles a clause this package does not test is not graded. Planned as
//   a record(), citing this
//   already-established fact -- reconfirmed against this package's own
//   fixture, not merely quoted from the sibling, once agnus.v/
//   agnus_spritedma.v are reopened below. If that reading does not hold
//   here, this section says so instead of recording on inherited
//   authority.
//
// Twelve ids total: 4 planned for SPREN-GATES-EVERY-SPRITE-CHANNEL, 6
// pending Group 2's own RTL answer below (asserted or recorded, not both),
// 2 planned as record(). No RTL has been opened for this extension yet --
// the SPR1/SPR7 register addresses above come from Appendix A/B's own
// printed table (p.291-292), not from agnus.v or agnus_spritedma.v.
//
// RESOLVED (after agnus.v/agnus_bitplanedma.v/agnus_spritedma.v were
// opened, see the IMPLEMENTATION section before main() and the assertions
// themselves): Group 2 IS observable at this boundary and is asserted, not
// recorded. Final disposition of all twelve ids, four assertions:
//   - SPREN-GATES-EVERY-SPRITE-CHANNEL (CH4-more.c4, CH4-p114.c1): the
//     GATING half of "enabling sprite DMA for one sprite enables it for
//     all" - a single SPREN write brings every channel's own vblend-
//     triggered POS/CTL fetch onto reg_address_out.
//   - SPREN-PLACES-EVERY-CHANNEL-IN-AUTOMATIC-MODE (CH4-more.c5,
//     CH4-p114.c2): the AUTOMATIC-MODE half - showing a channel actually
//     goes on to fetch its own image data, field after field, with no
//     further CPU step, needs the vblend-triggered POS/CTL reload to carry
//     a REAL VSTART/VSTOP rather than this fixture's idle data_in=0, which
//     SPREN-GATES-EVERY-SPRITE-CHANNEL's own POS/CTL-only measurement does
//     not supply - a separate assertion, feeding real DMA words on every
//     grant and sweeping two full fields.
//   - SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND (CH4-more.c8,
//     CH4-more.c9, CH4-warn.c1, CH4-warn.c2): the extra-wide-display case,
//     graded against a NORMAL-WIDTH, BPLEN-SET control (not BPLEN entirely
//     clear, which cannot distinguish "any bitplane DMA costs sprites" from
//     "only a widened one does"); the BPLEN-clear run remains as an
//     ungraded fixture check. Grades only that at least one of the eight
//     channels drops under half its control rate - neither page says which
//     channel survives or that fewer than all eight are ever affected, so
//     the condition does not either.
//   - SPRITE-DMA-LOSES-CHANNELS-UNDER-HORIZONTAL-SCROLL (CH4-more.c10,
//     CH4-p114.c5): the horizontal-scrolling case, stimulated with p.82's
//     own recipe (DDFSTRT one fetch unit sooner, $0038->$0030, BPLCON1
//     given a non-zero delay) against the same normal-width control.
//
// APPC-pos.c5/CH4-appC-position.c8 are graded, not recorded, on
// SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK below - RED (13 of 16 SPRxPOS/SPRxCTL
// fetch events fall inside hblank; channels 6 and 7 each have at least one
// that does not). Red is the correct, reportable outcome for that
// assertion and is left red.
// ============================================================================

#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus.h"

namespace {

// ---- register addresses (9-bit custom-chip offsets, agnus.v:103-104) ----
constexpr uint16_t DMACON_ADDR  = 0x096;
constexpr uint16_t DMACONR_ADDR = 0x002;

// ---- DMACON/DMACONR bit masks used by this package's own assertions ----
constexpr uint16_t SET_CLR = 0x8000;  // bit 15
constexpr uint16_t SPREN   = 1u << 5;
constexpr uint16_t BPLEN   = 1u << 8;
constexpr uint16_t DMAEN   = 1u << 9;

// ---- EXTENSION -- addresses this section's own assertions need, confirmed
// against agnus.v/agnus_bitplanedma.v/agnus_spritedma.v ----
//
// DIWSTRT/DIWSTOP/DDFSTRT/DDFSTOP/BPLCON0: same custom-register addresses
// hrm/ch7-dmacon-register-bits's own Harness already uses (agnus.v's own
// localparams DIWSTRT/DIWSTOP at agnus.v:105-106; DDFSTRT/DDFSTOP/BPLCON0 are
// agnus_bitplanedma.v's own DDFSTRT_REG/DDFSTOP_REG/BPLCON0_REG, 0x092/
// 0x094/0x100).
constexpr uint16_t DIWSTRT_ADDR = 0x08E;
constexpr uint16_t DIWSTOP_ADDR = 0x090;
constexpr uint16_t DDFSTRT_ADDR = 0x092;
constexpr uint16_t DDFSTOP_ADDR = 0x094;
constexpr uint16_t BPLCON0_ADDR = 0x100;
constexpr uint16_t BPLCON1_ADDR = 0x102;  // horizontal-scroll delay register

// Sprite N's own PTH/PTL/POS/CTL addresses, from Appendix A/B's own printed
// register table (p.291-292: SPR0PTH=$120, SPR1PTH=$124, ..., SPR7PTH=$13C,
// stride 4; SPR0POS=$140, SPR1POS=$148, ..., SPR7POS=$178, stride 8; CTL is
// POS+2) - confirmed against agnus_spritedma.v's own
// SPRPTBASE_REG=9'h120/SPRPOSCTLBASE_REG=9'h140 localparams, which use the
// same base and the same per-sprite indexing (reg_address_in[4:2] selects
// the pointer bank, reg_address_in[5:3] the position/control bank).
constexpr uint16_t SprPthAddr(int n) { return static_cast<uint16_t>(0x120 + 4 * n); }
constexpr uint16_t SprPtlAddr(int n) { return static_cast<uint16_t>(0x122 + 4 * n); }
constexpr uint16_t SprPosAddr(int n) { return static_cast<uint16_t>(0x140 + 8 * n); }
constexpr uint16_t SprCtlAddr(int n) { return static_cast<uint16_t>(0x142 + 8 * n); }

// Harness: copied from hrm/ch7-dmacon-register-bits's own Harness class unchanged
// (same fixture, same arbitrated register bus, same reason for the
// pre-edge-dbr retry - see IMPLEMENTATION NOTES above). Only the members
// this package's own assertions actually call are kept: write_reg, read_reg
// and the tick machinery they depend on. The DMA-consumption helpers
// (count_nonrefresh_dbr_over_lines, count_dbwe_over_ticks,
// sync_to_line_boundary, start_blit/copper/sprite/bitplane, set_bls) are
// hrm/ch7-dmacon-register-bits's own job, not copied here - this package does not
// duplicate what that package already measures.
class Harness {
public:
    Harness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->data_in = 0;
        dut->address_in = 0;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->disk_dmal = 0;
        dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0;
        dut->a1k = 0;
        dut->ecs = 0;
        dut->aga = 0;
        dut->floppy_speed = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One CPU register write, held for up to `max_ticks` ticks and applied
    // on the first tick where `dbr==0` (see IMPLEMENTATION NOTES). Returns
    // whether a dbr==0 tick was actually seen via `write_ok`, checked at
    // every call site so a silently-missed write cannot masquerade as a
    // passing assertion.
    void write_reg(uint16_t reg9, uint16_t data, int max_ticks = 8) {
        dut->aen = 1;
        dut->hwr = 1;
        dut->lwr = 1;
        dut->rd = 0;
        dut->address_in = static_cast<uint16_t>(reg9 >> 1);
        dut->data_in = data;
        write_ok = false;
        for (int i = 0; i < max_ticks; ++i) {
            bool pre_dbr = tick_pre_edge_dbr();
            if (!pre_dbr) { write_ok = true; break; }
        }
        dut->aen = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->address_in = 0;
        dut->data_in = 0;
    }

    // One CPU register read, same dbr==0 retry as write_reg. `read_ok`
    // reports whether a clean sample was taken.
    uint16_t read_reg(uint16_t reg9, int max_ticks = 8) {
        dut->aen = 1;
        dut->rd = 1;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->address_in = static_cast<uint16_t>(reg9 >> 1);
        uint16_t val = 0xFFFF;
        read_ok = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (!last_dbr) { val = last_data_out; read_ok = true; break; }
        }
        dut->aen = 0;
        dut->rd = 0;
        dut->address_in = 0;
        return val;
    }

    void tick() { tick_pre_edge_dbr(); }

    // Advances one clk7_en tick (cck toggle, then a clk 0->1->0 pulse) and
    // returns the PRE-edge value of `dbr` - the value the RTL's own
    // `always @(posedge clk)` write logic actually saw for this edge.
    bool tick_pre_edge_dbr() {
        dut->cck = cck_phase ? 1 : 0;
        cck_phase = !cck_phase;
        dut->eval();
        bool pre_dbr = dut->dbr;
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        last_dbr = dut->dbr;
        last_data_out = dut->data_out;
        last_refresh = dut->refresh;
        last_sol = dut->sol;
        last_hblank = dut->hblank;
        last_reg_address_out = static_cast<uint16_t>(dut->reg_address_out);
        return pre_dbr;
    }

    // Copied from hrm/ch7-dmacon-register-bits's own Harness (same fixture,
    // same `sol` port): waits for the next start-of-line pulse.
    bool sync_to_line_boundary(int max_ticks = 2000) {
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (last_sol) return true;
        }
        return false;
    }

    // Like tick_pre_edge_dbr(), but calls `feed(pre_dbr,
    // pre_reg_address_out)` AFTER sampling the PRE-edge dbr/reg_address_out
    // and BEFORE the clock edge that latches a DMA-granted register load, so
    // the caller can present a `data_in` word for that exact grant - this
    // fixture supplies no chip-RAM model, and agnus.v wires every submodule's
    // `data_in` straight to its own top-level `data_in` input (agnus.v:338,
    // 355, 379, 401, 422/450, 471 - the same external port for every engine),
    // so nothing else can put a fetched word on the bus. Mirrors the
    // technique hrm/ch4-sprite-dma-sequence's own harness already uses on
    // agnus_spritedma alone (mirror reg_address_out back as the write
    // address, present data_in at the grant); reg_address_in is not a port of
    // this package's own `agnus` top-level fixture (it is wired internally to
    // the arbiter's own `reg_address`, agnus.v:136), so only data_in needs
    // presenting here - the address side is already the arbiter's own doing.
    template <class Fn>
    bool tick_pre_edge_dbr_feeding(Fn&& feed) {
        dut->cck = cck_phase ? 1 : 0;
        cck_phase = !cck_phase;
        dut->eval();
        bool pre_dbr = dut->dbr;
        uint16_t pre_reg_address_out = static_cast<uint16_t>(dut->reg_address_out);
        feed(pre_dbr, pre_reg_address_out);
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        last_dbr = dut->dbr;
        last_data_out = dut->data_out;
        last_refresh = dut->refresh;
        last_sol = dut->sol;
        last_hblank = dut->hblank;
        last_reg_address_out = static_cast<uint16_t>(dut->reg_address_out);
        return pre_dbr;
    }

    bool last_dbr = false;
    uint16_t last_data_out = 0;
    bool write_ok = false;
    bool read_ok = false;
    // Observation-only additions, read by this section's own assertions;
    // every existing call site above is unaffected, and
    // last_dbr/last_data_out/write_ok/read_ok keep exactly the meaning the
    // first three assertions already depend on.
    bool last_refresh = false;
    bool last_sol = false;
    bool last_hblank = false;
    uint16_t last_reg_address_out = 0;

    Vagnus* dut;

private:
    bool cck_phase = false;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // record() form, for a claim this section declines rather than grades.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    // note() form, for a measurement printed without grading it (used by
    // the fixture check below).
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Helpers this section's own assertions share.
//
// Arms sprite `n`: a safe pointer (mirrors hrm/ch7-dmacon-register-bits's
// own start_sprite0 exactly, just at sprite n's own PTH/PTL address) and
// VSTART/VSTOP written into SPRxPOS[15:8]/SPRxCTL[15:8] - the 8-bit,
// non-ECS form both readers' own restatements describe (this Harness's
// `ecs` input is held 0 throughout, as it is in every already-committed
// sibling package's own Harness).
void arm_sprite(Harness& h, int n, uint8_t vstart, uint8_t vstop) {
    h.write_reg(SprPthAddr(n), 0x0000);
    h.write_reg(SprPtlAddr(n), 0x2000);
    h.write_reg(SprPosAddr(n), static_cast<uint16_t>(vstart) << 8);
    h.write_reg(SprCtlAddr(n), static_cast<uint16_t>(vstop) << 8);
}

// Pushes the bitplane data-fetch window to the hardware limits
// (agnus_bitplanedma.v's own hard_start/hard_stop, hpos word $18/$D8 -
// $18 is also exactly where agnus_spritedma.v's own sprite-fetch region
// begins) and arms six bitplanes (the heaviest non-AGA fetch demand this
// harness's own `aga=0` allows) - the widest display fetch this fixture can
// produce, matching the manual's own "extra wide display" wording. DIWSTRT/
// DIWSTOP are hrm/ch7-dmacon-register-bits's own already-proven-live values
// (its BPLEN-GATES-BITPLANE-DMA assertion).
void widen_bitplane_into_sprite_window(Harness& h) {
    h.write_reg(DIWSTRT_ADDR, 0x2C81);
    h.write_reg(DIWSTOP_ADDR, 0x2CC1);
    h.write_reg(DDFSTRT_ADDR, 0x0018);
    h.write_reg(DDFSTOP_ADDR, 0x00D8);
    h.write_reg(BPLCON0_ADDR, 0x6000);  // BPU=6 (data_in[14:12]=110), aga=0
}

// The normal-width, non-scrolled bitplane display used as this package's
// own control for "additional DMA cycles cost some sprites" (same BPU as
// widen_bitplane_into_sprite_window, DDFSTRT at the ordinary p.82 start
// "the normal data-fetch start for non-scrolled displays is ($38)", DDFSTOP
// at a normal, non-maximal stop - hrm/ch7-dmacon-register-bits's own
// start_bitplane/measure_sprite_data_fetch already use this exact
// DDFSTRT/DDFSTOP pair as their own baseline). A design where ANY bitplane
// DMA costs sprites, not only a widened one, must still pass this control.
void normal_bitplane(Harness& h) {
    h.write_reg(DIWSTRT_ADDR, 0x2C81);
    h.write_reg(DIWSTOP_ADDR, 0x2CC1);
    h.write_reg(DDFSTRT_ADDR, 0x0038);
    h.write_reg(DDFSTOP_ADDR, 0x00D0);
    h.write_reg(BPLCON0_ADDR, 0x6000);  // same BPU=6 as the wide case
}

// The manual's own operational definition of "enabling horizontal
// scrolling" (p.82): "The normal data-fetch start for
// non-scrolled displays is ($38). If horizontal scrolling is desired, then
// the data fetch must start one word sooner (DDFSTRT = $0030)... DDFSTOP
// remains unchanged" - already the established reading elsewhere in this
// corpus (hrm/ch3-ddf-fetch-window's own HSCROLL-EXTRA-WORD-21-FOR-20, and
// hrm/ch7-dmacon-register-bits's own DDFSTRT-0030-SPRITE-7, which cites the
// SAME p.82/83 sentences this package's CH4-more.c10/CH4-p114.c5 also read).
// BPLCON1 (the pixel-delay register the manual also names for scrolling) is
// written too, for fidelity to the manual's own recipe, even though this
// `agnus` fixture is Agnus alone and BPLCON1 has no Denise to be read by -
// only DDFSTRT moves the sprite-DMA needle here.
void horizontal_scroll_bitplane(Harness& h) {
    h.write_reg(DIWSTRT_ADDR, 0x2C81);
    h.write_reg(DIWSTOP_ADDR, 0x2CC1);
    h.write_reg(DDFSTRT_ADDR, 0x0030);  // one fetch unit before $38, p.82
    h.write_reg(DDFSTOP_ADDR, 0x00D0);  // "DDFSTOP remains unchanged" (p.82)
    h.write_reg(BPLCON0_ADDR, 0x6000);  // same BPU=6 as the wide/normal cases
    h.write_reg(BPLCON1_ADDR, 0x0004);  // BPLCON1: non-zero scroll delay
}

// One granted, non-refresh dma cycle whose reg_address_out falls in the
// sprite engine's own 8-bit range (agnus_spritedma.v: {SPRPOSCTLBASE_REG
// [8:6]=3'b101, sprite[2:0], type[1:0]}, i.e. $A0-$BF - never produced by
// any other engine on this fixture, confirmed against every other engine's
// own *_REG localparam in agnus.v/agnus_bitplanedma.v/agnus_copper.v/
// agnus_blitter.v/agnus_diskdma.v, none of which fall in $A0-$BF).
struct SpriteFetch {
    int sprite;   // 0-7
    int type;     // 0=POS 1=CTL 2=DATA 3=DATB (agnus_spritedma.v's own encoding)
    bool hblank;  // agnus.v's own `hblank` output, sampled the same tick
};

std::vector<SpriteFetch> collect_sprite_fetches_over_lines(Harness& h, int n_lines,
                                                            int max_ticks_per_line = 1000) {
    std::vector<SpriteFetch> out;
    for (int line = 0; line < n_lines; ++line) {
        for (int t = 0; t < max_ticks_per_line; ++t) {
            h.tick();
            if (h.last_sol) break;
            if (h.last_dbr && !h.last_refresh) {
                uint16_t a = h.last_reg_address_out;
                if ((a & 0xE0) == 0xA0) {
                    out.push_back(SpriteFetch{(a >> 2) & 0x7, a & 0x3, h.last_hblank});
                }
            }
        }
    }
    return out;
}

// Number of DISTINCT sprite indices (0-7) with at least one fetch of the
// given type(s) in `fetches`. type_mask bit i set means type i is counted;
// e.g. 0x3 = POS or CTL, 0xC = DATA or DATB.
int distinct_sprites(const std::vector<SpriteFetch>& fetches, int type_mask) {
    std::set<int> seen;
    for (const auto& f : fetches) {
        if (type_mask & (1 << f.type)) seen.insert(f.sprite);
    }
    return static_cast<int>(seen.size());
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER (p.278) ----------------
    // The fixture-integrity proof this chapter's own build brief requires:
    // three DIFFERENT DMACON patterns are written in sequence and DMACONR is
    // sampled after EACH one (not just the final state), confirming each
    // arrives distinct and in the order it was written - the same harness
    // cannot be silently returning a stale or zero value and still pass all
    // three. This is also this package's control case, expected to PASS on
    // the same read_reg/write_reg measurement basis as the two suspicion
    // cases below.
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t after_a = h.read_reg(DMACONR_ADDR);
        bool ok_a = h.read_ok && h.write_ok;

        h.write_reg(DMACON_ADDR, SET_CLR | BPLEN);
        uint16_t after_b = h.read_reg(DMACONR_ADDR);
        bool ok_b = h.read_ok && h.write_ok;

        h.write_reg(DMACON_ADDR, 0x0000 | SPREN);  // CLR only SPREN
        uint16_t after_c = h.read_reg(DMACONR_ADDR);
        bool ok_c = h.read_ok && h.write_ok;

        checks.expect(
            ok_a && ok_b && ok_c &&
                after_a == (SPREN | DMAEN) &&
                after_b == (SPREN | DMAEN | BPLEN) &&
                after_c == (DMAEN | BPLEN),
            "DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER: three distinct "
            "DMACON writes, sampled through DMACONR after each one, arrive "
            "in the order written - SET SPREN|DMAEN (0x0220), then SET BPLEN "
            "additionally (0x0320), then CLR only SPREN, leaving DMAEN and "
            "BPLEN untouched (0x0300) - proving the harness delivers a known, "
            "freshly-fetched word at each step rather than a stale or "
            "zero-flag one, and demonstrating the SET-then-partial-CLR "
            "direction ('bits written with a zero are unchanged') under this "
            "package's own measurement. 'DMACON 096 W ... DMA control write "
            "(clear or set)' / 'DMACONR 002 R ... DMA control (and blitter "
            "status) read' / '15 SET/CLR ... Bits written with a zero ... "
            "unchanged' (p.278). APPA-dmacon.c1, APPA-dmacon.c3, "
            "APPA-dmacon.c4, APPA-dmacon.c5, CH4-appA-dmacon.c1, "
            "CH4-appA-dmacon.c2, CH4-appA-dmacon.c3, CH4-appA-dmacon.c4, "
            "CH4-appA-dmacon.c11, CH4-appA-dmacon.c12, APPA-dmacon.c2, "
            "CH4-appA-dmacon.c5, CH4-appA-dmacon.c6, CH4-appA-dmacon.c7, "
            "CH4-appA-dmacon.c8, CH4-appA-dmacon.c9, CH4-appA-dmacon.c10.");
    }

    // ---------------- DMACON-IS-WRITE-ONLY-AT-096 (p.278) ----------------
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t at_002 = h.read_reg(DMACONR_ADDR);   // positive control: same run, other address
        bool ok_002 = h.read_ok;
        uint16_t at_096 = h.read_reg(DMACON_ADDR);    // the address under test
        bool ok_096 = h.read_ok;

        checks.expect(
            ok_002 && ok_096 && at_002 == (SPREN | DMAEN) && at_096 == 0x0000,
            "DMACON-IS-WRITE-ONLY-AT-096: after a write of SPREN|DMAEN "
            "through $096, a read through the SAME aen/rd/address_in path "
            "AIMED AT $096 itself returns 0x0000, not the pattern just "
            "written, while the SAME state read at $002 (DMACONR) in the "
            "SAME run - the paired control on the same measurement basis - "
            "DOES return it (0x0220) - 'DMACON 096 W ... DMA control write "
            "(clear or set)' (p.278), the R/W column showing DMACON as write "
            "only. APPA-dmacon.c1, APPA-dmacon.c3, CH4-appA-dmacon.c1, "
            "CH4-appA-dmacon.c2, CH4-appA-dmacon.c3.");
    }

    // ---------------- DMACONR-IS-READ-ONLY-AT-002 (p.278) ----------------
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BPLEN);  // establish a known baseline
        uint16_t baseline = h.read_reg(DMACONR_ADDR);
        bool ok_baseline = h.read_ok && h.write_ok;

        h.write_reg(DMACONR_ADDR, SET_CLR | SPREN);  // attempt a write AT $002 itself
        uint16_t after_attempt = h.read_reg(DMACONR_ADDR);
        bool ok_attempt = h.read_ok && h.write_ok;   // write_ok only reports a dbr==0 cycle was seen, not that the write landed in dmacon

        h.write_reg(DMACON_ADDR, SET_CLR | SPREN);   // the paired control: a REAL write, through $096, in the SAME run
        uint16_t after_real_write = h.read_reg(DMACONR_ADDR);
        bool ok_real = h.read_ok && h.write_ok;

        checks.expect(
            ok_baseline && ok_attempt && ok_real &&
                baseline == (DMAEN | BPLEN) &&
                after_attempt == (DMAEN | BPLEN) &&
                after_real_write == (DMAEN | BPLEN | SPREN),
            "DMACONR-IS-READ-ONLY-AT-002: a write attempted AT $002 itself "
            "(DMACONR's own address) leaves DMACONR's later read unchanged "
            "(0x0300 before and after the attempt) even though the attempt's "
            "own data pattern (SET_CLR|SPREN) would have changed it had it "
            "landed, while a REAL write of the SAME pattern through $096, in "
            "the SAME run - the paired control on the same measurement basis "
            "- DOES change it (0x0320) - proving the write path itself is "
            "live and the $002 attempt's silence is not a dead harness. "
            "'DMACONR 002 R ... DMA control (and blitter status) read' "
            "(p.278), the R/W column showing DMACONR as read only. "
            "APPA-dmacon.c2, CH4-appA-dmacon.c5, CH4-appA-dmacon.c6, "
            "CH4-appA-dmacon.c7.");
    }

    // ============================================================================
    // EXTENSION -- three new assertions for the twelve ids the header's own
    // EXTENSION section plans.
    // ============================================================================

    // ---------------- SPREN-GATES-EVERY-SPRITE-CHANNEL (p.114) ----------------
    // All eight sprites are armed identically. Their shared, once-per-field
    // SPRxPOS/SPRxCTL fetch (agnus_spritedma.v's own `vblend` trigger - the
    // last line of vertical blanking, the same event for every sprite, not
    // tied to any per-sprite VSTART/VSTOP) is swept over enough lines to
    // reach it, once with SPREN set and once with SPREN left clear
    // [control] - same DMAEN, same sprite setup, same sweep.
    {
        Harness h_en;
        for (int n = 0; n < 8; ++n) arm_sprite(h_en, n, 200, 220);
        h_en.write_reg(DMACON_ADDR, SET_CLR | DMAEN | SPREN);
        auto fetches_en = collect_sprite_fetches_over_lines(h_en, 35);
        int channels_en = distinct_sprites(fetches_en, 0x3);  // POS(1)|CTL(2)

        Harness h_dis;
        for (int n = 0; n < 8; ++n) arm_sprite(h_dis, n, 200, 220);
        h_dis.write_reg(DMACON_ADDR, SET_CLR | DMAEN);  // control: SPREN left clear
        auto fetches_dis = collect_sprite_fetches_over_lines(h_dis, 35);
        int channels_dis = distinct_sprites(fetches_dis, 0x3);

        char en_str[4];
        snprintf(en_str, sizeof en_str, "%d", channels_en);
        char dis_str[4];
        snprintf(dis_str, sizeof dis_str, "%d", channels_dis);

        checks.expect(
            channels_en == 8 && channels_dis == 0,
            "SPREN-GATES-EVERY-SPRITE-CHANNEL: with all eight sprites armed "
            "identically (pointer plus VSTART/VSTOP on every SPRxPOS/SPRxCTL) "
            "and DMAEN held set throughout, a single SPREN write brings the "
            "vblend-triggered SPRxPOS/SPRxCTL fetch of every one of the eight "
            "sprite channels onto reg_address_out within a 35-line sweep (" +
                std::string(en_str) +
                "/8 distinct channels), while the SAME setup with SPREN left "
                "clear [control] shows none (" + std::string(dis_str) +
                "/8) - one enable, not eight, and no further write is made "
                "between the enable and the fetch. 'When you enable sprite "
                "DMA for one sprite, you enable DMA for all the sprites and "
                "place them all in automatic mode.' (p.114) - the GATING half "
                "only; 'automatic mode' itself (no further step needed, field "
                "after field) is graded separately by "
                "SPREN-PLACES-EVERY-CHANNEL-IN-AUTOMATIC-MODE below, which is "
                "where CH4-more.c5/CH4-p114.c2 are cited -- this assertion's "
                "own 35-line, POS/CTL-only sweep never reaches VSTART=200 and "
                "this fixture returns data_in=0 on every grant it does not "
                "feed, so it could not have shown a channel going on to "
                "display. CH4-more.c4 CH4-p114.c1.");
    }

    // ---------------- SPREN-PLACES-EVERY-CHANNEL-IN-AUTOMATIC-MODE (p.114) ----------------
    // Separate from SPREN-GATES-EVERY-SPRITE-CHANNEL above: showing a
    // channel actually goes on to fetch its own image data, field after
    // field, with no further CPU step ("automatic mode"), needs the
    // vblend-triggered POS/CTL reload to carry a REAL VSTART/VSTOP - this
    // fixture's data_in otherwise reads back 0 on any grant the file does
    // not itself feed, which would silently zero VSTART/VSTOP and leave no
    // channel ever reaching its own display window.
    //
    // Fed by presenting every DMA-granted SPRxPOS/SPRxCTL load with the SAME
    // real VSTART/VSTOP word every time it is fetched (tick_pre_edge_dbr_
    // feeding(), added to Harness above) -- exactly the technique hrm/
    // ch4-sprite-dma-sequence's own harness already uses on this same
    // module (mirror the granted register address, present data_in at the
    // grant). Only the sprite pointer (PTH/PTL) is CPU-written, once, before
    // SPREN is set; SPRxPOS/SPRxCTL themselves are NEVER CPU-written at
    // all -- every load of them, first and every later one, is the DMA
    // engine's own automatic reread, "not need[ing] a further step" per the
    // claim. The sweep runs 660 lines (this fixture's own PAL beam counter,
    // agnus_beamcounter.v's VTOTAL_PAL_VAL=311/VBSTOP_PAL_VAL=25 with
    // ntsc=0, wraps at line 311 -- 660 comfortably covers TWO consecutive
    // fields, split at line 330 for the per-field count below), and each of
    // the eight channels' own SPRxDATA/SPRxDATB fetch (active for
    // VSTART<=vpos<VSTOP, 40..220, well inside either half) is counted
    // separately in each half.
    {
        constexpr uint8_t kVstart = 40, kVstop = 220;
        Harness h;
        for (int n = 0; n < 8; ++n) {
            h.write_reg(SprPthAddr(n), 0x0000);
            h.write_reg(SprPtlAddr(n), 0x2000);
        }
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | SPREN);

        auto feed_real_words = [&](bool pre_dbr, uint16_t pre_addr) {
            if (!pre_dbr || (pre_addr & 0xE0) != 0xA0) return;
            int type = pre_addr & 0x3;
            if (type == 0) h.dut->data_in = static_cast<uint16_t>(kVstart) << 8;       // POS
            else if (type == 1) h.dut->data_in = static_cast<uint16_t>(kVstop) << 8;   // CTL
        };

        constexpr int kFieldSplitLine = 330;   // > VBSTOP(25), < VTOTAL(311) into field 2
        constexpr int kTotalLines = 660;       // > 2 * (VTOTAL_PAL_VAL+1) = 624
        int field1[8] = {0}, field2[8] = {0};
        for (int line = 0; line < kTotalLines; ++line) {
            for (int t = 0; t < 1000; ++t) {
                h.tick_pre_edge_dbr_feeding(feed_real_words);
                if (h.last_sol) break;
                if (h.last_dbr && !h.last_refresh) {
                    uint16_t a = h.last_reg_address_out;
                    if ((a & 0xE0) == 0xA0 && (a & 0x3) >= 2) {
                        int sprite = (a >> 2) & 0x7;
                        (line < kFieldSplitLine ? field1 : field2)[sprite]++;
                    }
                }
            }
        }

        bool field1_all = true, field2_all = true;
        int min1 = field1[0], min2 = field2[0];
        for (int n = 0; n < 8; ++n) {
            if (field1[n] == 0) field1_all = false;
            if (field2[n] == 0) field2_all = false;
            if (field1[n] < min1) min1 = field1[n];
            if (field2[n] < min2) min2 = field2[n];
        }
        char min1_str[8], min2_str[8];
        snprintf(min1_str, sizeof min1_str, "%d", min1);
        snprintf(min2_str, sizeof min2_str, "%d", min2);

        checks.expect(
            field1_all && field2_all,
            "SPREN-PLACES-EVERY-CHANNEL-IN-AUTOMATIC-MODE: all eight sprites' "
            "pointers (PTH/PTL only) are CPU-written once and SPREN is set "
            "once; SPRxPOS/SPRxCTL are NEVER CPU-written, only fed as DMA "
            "words (VSTART=40/VSTOP=220) exactly when the fixture's own "
            "reg_address_out shows them granted -- yet every one of the "
            "eight channels' own SPRxDATA/SPRxDATB fetch reaches "
            "reg_address_out in BOTH of two consecutive PAL fields (field 1: "
            "min " +
                std::string(min1_str) +
                " events/channel; field 2: min " + std::string(min2_str) +
                " events/channel, over a 660-line, field-split sweep) -- the "
                "vblend-triggered reread repeats on its own, field after "
                "field, without a further CPU step. 'you do not need' [a "
                "further step] / 'place them all in automatic mode' (p.114). "
                "CH4-more.c5, CH4-p114.c2.");
    }

    // ---------------- SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND (p.114-115) ----------------
    // The control is a NORMAL-WIDTH display (same BPU, DDFSTRT=$38, BPLEN
    // SET - normal_bitplane() above), not BPLEN entirely clear: a design
    // where ANY bitplane DMA costs sprites, not only a widened one, must
    // still pass this control, so BPLEN-clear alone cannot tell the two
    // apart. The BPLEN-clear run is kept only as an ungraded fixture check
    // (a MEASURED note below).
    //
    // The condition grades only "at least one of the eight channels' rate
    // drops to under half the control's" - exactly what "some sprites can
    // become unusable" states. Neither p.114 nor p.115 says WHICH channel
    // survives or that fewer than all eight are ever affected, so the
    // condition does not claim either (those would be facts about this
    // fixture's own slot timing, not about the page).
    //
    // Same eight sprites, same SPREN|DMAEN, measuring their per-line
    // SPRxDATA/SPRxDATB fetch (active once vpos reaches each sprite's own
    // VSTART) over a line window - once at the normal-width control, once
    // with BPLEN set and DDFSTRT/DDFSTOP pushed to the hardware fetch
    // limits ($18-$D8), which starts the display fetch inside the sprite
    // engine's own $18-$38 slot window. agnus.v's own fixed-priority mux
    // tests dma_bpl before dma_spr (agnus.v:207 before :219), so a granted
    // bitplane cycle on one of a sprite's two ticks silently costs that
    // sprite the tick - no retry.
    //
    // Each run first lets vblend (vpos==25, agnus_beamcounter.v's own
    // vbstop) pass with SPREN CLEAR - agnus_spritedma.v's own header
    // comment says vblend is "the only chance for DMA to fetch new values
    // of SPRxPOS/SPRxCTL" each field, and this harness supplies no chip-RAM
    // model, so a vblend fetch with SPREN set would silently overwrite the
    // VSTART/VSTOP this run is about to write with data_in's idle value (0)
    // - caught empirically: the first version of this assertion armed the
    // sprites and set SPREN before vblend, and measured 16 POS/CTL events
    // (the vblend refetch itself) but zero DATA/DATB events in either run,
    // because that refetch had already zeroed every VSTART/VSTOP. Arming
    // AFTER vblend, with SPREN still clear until the arm is done, means the
    // CPU-written VSTART/VSTOP are the last word in sprpos/sprctl - the
    // next automatic refetch is a full field away, well outside this
    // assertion's own window.
    {
        int wide_per_sprite[8] = {0};
        // 35 lines: the same budget SPREN-GATES-EVERY-SPRITE-CHANNEL's own
        // sweep already proved sufficient to reach vblend (its 8/8 result
        // is exactly the vblend-triggered fetch landing inside 35 lines).
        // No agnus.v port carries vpos or vblend directly - both are
        // internal to agnus_beamcounter.v - so this counts sol pulses
        // rather than reading either.
        Harness h_none;
        h_none.write_reg(DMACON_ADDR, SET_CLR | DMAEN);  // SPREN, BPLEN both clear
        for (int i = 0; i < 35; ++i) h_none.sync_to_line_boundary();
        for (int n = 0; n < 8; ++n) arm_sprite(h_none, n, 40, 220);
        h_none.write_reg(DMACON_ADDR, SET_CLR | SPREN);  // fixture check: BPLEN left clear
        auto fetches_none = collect_sprite_fetches_over_lines(h_none, 90);

        Harness h_normal;
        h_normal.write_reg(DMACON_ADDR, SET_CLR | DMAEN);
        for (int i = 0; i < 35; ++i) h_normal.sync_to_line_boundary();
        for (int n = 0; n < 8; ++n) arm_sprite(h_normal, n, 40, 220);
        normal_bitplane(h_normal);
        h_normal.write_reg(DMACON_ADDR, SET_CLR | SPREN | BPLEN);  // control: normal-width, BPLEN SET
        auto fetches_normal = collect_sprite_fetches_over_lines(h_normal, 90);

        Harness h_wide;
        h_wide.write_reg(DMACON_ADDR, SET_CLR | DMAEN);
        for (int i = 0; i < 35; ++i) h_wide.sync_to_line_boundary();
        for (int n = 0; n < 8; ++n) arm_sprite(h_wide, n, 40, 220);
        widen_bitplane_into_sprite_window(h_wide);
        h_wide.write_reg(DMACON_ADDR, SET_CLR | SPREN | BPLEN);
        auto fetches_wide = collect_sprite_fetches_over_lines(h_wide, 90);

        // distinct_sprites (>=1 event, ever) is too weak here: a sprite
        // that loses most, not all, of its per-line DATA/DATB cycles still
        // shows up at least once across 90 lines. The manual's own
        // "unusable" is a per-sprite RATE claim, so this counts each
        // sprite's own total DATA+DATB events in both runs and compares
        // them sprite by sprite - the same "what would this look like if
        // the claim were true, and does it differ from false" witness this
        // package's own DMACON-IS-WRITE-ONLY-AT-096 already uses, applied
        // per channel instead of per address.
        int none_per_sprite[8] = {0}, normal_per_sprite[8] = {0};
        for (const auto& f : fetches_none) {
            if (f.type == 2 || f.type == 3) ++none_per_sprite[f.sprite];
        }
        for (const auto& f : fetches_normal) {
            if (f.type == 2 || f.type == 3) ++normal_per_sprite[f.sprite];
        }
        for (const auto& f : fetches_wide) {
            if (f.type == 2 || f.type == 3) ++wide_per_sprite[f.sprite];
        }
        int none_uniform = none_per_sprite[0];
        bool none_all_equal = true;
        int normal_uniform = normal_per_sprite[0];
        bool normal_all_equal = true;
        int sprites_halved = 0;
        for (int n = 0; n < 8; ++n) {
            if (none_per_sprite[n] != none_uniform) none_all_equal = false;
            if (normal_per_sprite[n] != normal_uniform) normal_all_equal = false;
            if (wide_per_sprite[n] * 2 < normal_per_sprite[n]) ++sprites_halved;
        }

        checks.note(
            "SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND fixture "
            "check (not graded): with BPLEN entirely clear (no bitplane "
            "engine running at all), all eight channels' own SPRxDATA/"
            "SPRxDATB fetch reaches reg_address_out at the SAME rate (" +
                std::to_string(none_uniform) + " events/channel, uniform=" +
                (none_all_equal ? std::string("yes") : std::string("no")) +
                ") - the fixture delivers a known, non-zero rate before any "
                "bitplane demand is added.");

        char normal_rate_str[8];
        snprintf(normal_rate_str, sizeof normal_rate_str, "%d", normal_uniform);
        char worst_wide_str[8];
        int worst_sprite = 0;
        for (int n = 1; n < 8; ++n) if (wide_per_sprite[n] < wide_per_sprite[worst_sprite]) worst_sprite = n;
        snprintf(worst_wide_str, sizeof worst_wide_str, "%d", wide_per_sprite[worst_sprite]);
        char halved_str[4];
        snprintf(halved_str, sizeof halved_str, "%d", sprites_halved);

        checks.expect(
            normal_all_equal && normal_uniform > 0 && sprites_halved >= 1,
            "SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND: with all "
            "eight sprites armed identically (VSTART=40/VSTOP=220) and "
            "SPREN|DMAEN held set throughout, every one of the eight "
            "channels' own SPRxDATA/SPRxDATB fetch reaches reg_address_out "
            "at the SAME rate (" +
                std::string(normal_rate_str) +
                " events/channel over a 90-line window) at the CONTROL - a "
                "NORMAL-WIDTH display, same BPU, DDFSTRT=$38, BPLEN SET (not "
                "BPLEN clear: a design where any bitplane DMA costs sprites "
                "must still pass this control) - the SAME setup, with "
                "DDFSTRT/DDFSTOP pushed to the hardware limits ($18-$D8, the "
                "widest display fetch this fixture can produce, matching the "
                "manual's 'extra wide display' wording) in the SAME run, "
                "drops at least one channel to under half the control's rate "
                "(channel " +
                std::to_string(worst_sprite) + ": " + std::string(worst_wide_str) +
                " events; " + std::string(halved_str) +
                " of 8 channels halved or worse) - SOME sprites "
                "become unusable when additional DMA cycles are allocated to "
                "displaying the screen, exactly the manual's own wording (no "
                "claim is made here about WHICH channel survives or that "
                "fewer than all eight are ever affected - neither page says "
                "either). 'Remember that some sprites can become unusable "
                "when additional DMA cycles are allocated to displaying the "
                "screen, for example when an extra wide display ... is "
                "enabled' (p.114) / 'Some sprites become unusable when "
                "additional DMA cycles are allocated ... e.g. when enabling "
                "an extra wide display' (p.115, Warning). CH4-more.c8 "
                "CH4-more.c9 CH4-warn.c1 CH4-warn.c2.");
    }

    // ---------------- SPRITE-DMA-LOSES-CHANNELS-UNDER-HORIZONTAL-SCROLL (p.82-83, p.114-115) ----------------
    // Separate from SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND
    // above, which only ever widens BPU/DDFSTRT and never moves DDFSTRT the
    // ONE fetch unit a scrolled display actually needs, never touches
    // BPLCON1, and so never stimulates the horizontal-scrolling case
    // CH4-more.c10/CH4-p114.c5 name. Same normal-width control as above
    // (recomputed fresh on an isolated Harness), this time compared against
    // horizontal_scroll_bitplane() - DDFSTRT moved from $38 to $30 (one
    // fetch unit sooner) and BPLCON1 given a non-zero delay, the manual's
    // own p.82 recipe for "horizontal scrolling is enabled", already the
    // established reading of this exact sentence elsewhere in this corpus
    // (hrm/ch3-ddf-fetch-window's HSCROLL-EXTRA-WORD-21-FOR-20; hrm/
    // ch7-dmacon-register-bits's DDFSTRT-0030-SPRITE-7, which cites the
    // SAME p.82/83 "will disable sprite 7" sentence CH4-more.c10/
    // CH4-p114.c5 also read).
    {
        Harness h_normal;
        h_normal.write_reg(DMACON_ADDR, SET_CLR | DMAEN);
        for (int i = 0; i < 35; ++i) h_normal.sync_to_line_boundary();
        for (int n = 0; n < 8; ++n) arm_sprite(h_normal, n, 40, 220);
        normal_bitplane(h_normal);
        h_normal.write_reg(DMACON_ADDR, SET_CLR | SPREN | BPLEN);
        auto fetches_normal = collect_sprite_fetches_over_lines(h_normal, 90);

        Harness h_scroll;
        h_scroll.write_reg(DMACON_ADDR, SET_CLR | DMAEN);
        for (int i = 0; i < 35; ++i) h_scroll.sync_to_line_boundary();
        for (int n = 0; n < 8; ++n) arm_sprite(h_scroll, n, 40, 220);
        horizontal_scroll_bitplane(h_scroll);
        h_scroll.write_reg(DMACON_ADDR, SET_CLR | SPREN | BPLEN);
        auto fetches_scroll = collect_sprite_fetches_over_lines(h_scroll, 90);

        int normal_per_sprite[8] = {0}, scroll_per_sprite[8] = {0};
        for (const auto& f : fetches_normal) {
            if (f.type == 2 || f.type == 3) ++normal_per_sprite[f.sprite];
        }
        for (const auto& f : fetches_scroll) {
            if (f.type == 2 || f.type == 3) ++scroll_per_sprite[f.sprite];
        }
        int normal_uniform = normal_per_sprite[0];
        bool normal_all_equal = true;
        int sprites_halved = 0;
        int worst_sprite = 0;
        for (int n = 0; n < 8; ++n) {
            if (normal_per_sprite[n] != normal_uniform) normal_all_equal = false;
            if (scroll_per_sprite[n] * 2 < normal_per_sprite[n]) ++sprites_halved;
            if (scroll_per_sprite[n] < scroll_per_sprite[worst_sprite]) worst_sprite = n;
        }

        checks.expect(
            normal_all_equal && normal_uniform > 0 && sprites_halved >= 1,
            "SPRITE-DMA-LOSES-CHANNELS-UNDER-HORIZONTAL-SCROLL: with all "
            "eight sprites armed identically (VSTART=40/VSTOP=220) and "
            "SPREN|DMAEN held set throughout, every one of the eight "
            "channels' own SPRxDATA/SPRxDATB fetch reaches reg_address_out "
            "at the SAME rate (" +
                std::to_string(normal_uniform) +
                " events/channel over a 90-line window) at the normal-width "
                "control (DDFSTRT=$38, BPLCON1=0) - the SAME setup with "
                "DDFSTRT moved to $0030 (one fetch unit sooner, 'the data "
                "fetch must start one word sooner (DDFSTRT = $0030)', p.82) "
                "and BPLCON1 given a non-zero scroll delay in the SAME run "
                "drops channel " +
                std::to_string(worst_sprite) + " to " +
                std::to_string(scroll_per_sprite[worst_sprite]) +
                " events (" + std::to_string(sprites_halved) +
                " of 8 channels halved or worse) - enabling horizontal "
                "scrolling is one case that allocates additional DMA cycles "
                "and can make some sprites unusable, exactly the manual's "
                "own wording ('Incidentally, this will disable sprite 7', "
                "p.82; 'Fetching an extra word for scrolling will disable "
                "some sprites', p.83). 'Enabling horizontal scrolling is "
                "one case that allocates additional DMA cycles and can make "
                "some sprites unusable' / 'Additional display DMA demand, "
                "including extra-wide display or horizontal scrolling, can "
                "consume sprite DMA slots and make some channels "
                "unavailable' (p.114). CH4-more.c10 CH4-p114.c5.");
    }

    // ---------------- SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK (p.303, Appendix C) ----------------
    // "Usually" contrasts DMA loading against processor loading (not a
    // frequency-of-DMA-events claim), and the DMA loads are placed "during
    // horizontal blank" with no qualifier - a plain, per-fetch structural
    // claim this fixture's own `hblank` output can grade directly, exactly
    // as the id's own text reads. Graded, not recorded: the same
    // vblend-triggered SPRxPOS/
    // SPRxCTL fetch SPREN-GATES-EVERY-SPRITE-CHANNEL already swept, checked
    // against agnus.v's own `hblank` output (agnus_beamcounter.v:
    // HBSTRT_VAL=25, HBSTOP_VAL=102, fixed with varbeamen left at its reset
    // value - hblank high for hpos in [25,102)) at the SAME tick as each
    // fetch.
    //
    // MEASURED (per-event, not assumed): agnus_spritedma.v's own fetch
    // region is hpos[8:1] in [$18,$38), i.e. REAL hpos in [49,113) (hpos[8:1]
    // is a WORD address - real hpos is twice that plus the LSB set on the
    // triggering edge, agnus_spritedma.v:267-271). That range starts safely
    // inside hblank (49 > 25) but EXTENDS PAST hblank's own end at hpos 102
    // (113 > 102) - the LAST ~1.5 of the fetch window's eight per-sprite
    // slots (sprite 6's second half, all of sprite 7) fall after hpos 102,
    // outside hblank, by construction: of 16 SPRxPOS/SPRxCTL events across
    // 8 channels, 13 fall inside hblank (sprites 0-5 entirely, sprite 6's
    // POS only) and 3 fall outside (sprite 6's CTL, sprite 7's POS and
    // CTL) - confirmed per-event, not merely counted.
    //
    // This IS the router's grading ("assert every such fetch falls while
    // hblank is high"), and it comes out RED: read literally, with no
    // "usually" qualifier softening it, the manual's sentence is wrong for
    // sprites 6/7 on this RTL. Red is the correct, reportable outcome, not
    // an inverted oracle - matching this package's own DDFSTRT-0030-SPRITE-7
    // sibling in hrm/ch7-dmacon-register-bits, which is red for the SAME
    // reason (a page statement this RTL's own timing does not fully bear
    // out).
    {
        Harness h;
        for (int n = 0; n < 8; ++n) arm_sprite(h, n, 200, 220);
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | SPREN);
        auto fetches = collect_sprite_fetches_over_lines(h, 35);

        int pos_ctl_events = 0, pos_ctl_events_in_hblank = 0;
        std::set<int> sprites_seen, sprites_outside_hblank;
        for (const auto& f : fetches) {
            if (f.type == 0 || f.type == 1) {
                ++pos_ctl_events;
                sprites_seen.insert(f.sprite);
                if (f.hblank) {
                    ++pos_ctl_events_in_hblank;
                } else {
                    sprites_outside_hblank.insert(f.sprite);
                }
            }
        }

        char events_str[8];
        snprintf(events_str, sizeof events_str, "%d", pos_ctl_events);
        char in_hblank_str[8];
        snprintf(in_hblank_str, sizeof in_hblank_str, "%d", pos_ctl_events_in_hblank);
        char sprites_seen_str[4];
        snprintf(sprites_seen_str, sizeof sprites_seen_str, "%d",
                  static_cast<int>(sprites_seen.size()));
        char outside_str[4];
        snprintf(outside_str, sizeof outside_str, "%d",
                  static_cast<int>(sprites_outside_hblank.size()));

        checks.expect(
            sprites_outside_hblank.empty() && pos_ctl_events == pos_ctl_events_in_hblank &&
                pos_ctl_events > 0,
            "SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK: \"They are usually loaded "
            "by the sprite DMA channel, during horizontal blank, however "
            "they may be loaded by the processor\" / \"sprite DMA normally "
            "loads SPRxPOS and SPRxCTL during horizontal blanking\" (p.303) "
            "- 'usually' contrasts DMA loading against processor loading, "
            "and the DMA loads themselves are placed 'during horizontal "
            "blank' with no frequency qualifier, so this fixture's own "
            "hblank output grades it directly, per fetch: of " +
                std::string(events_str) +
                " SPRxPOS/SPRxCTL fetch events across " +
                std::string(sprites_seen_str) +
                " sprite channels swept, " + std::string(in_hblank_str) +
                " occurred while hblank was high; " + std::string(outside_str) +
                " channels (6 and 7, the LAST two in the fetch window) had "
                "at least one fetch outside hblank - agnus_spritedma.v's own "
                "fetch region (real hpos 49-113, twice hpos[8:1]'s own "
                "$18-$38 word range) starts inside hblank (hpos 25-102) but "
                "runs past its own end at hpos 102, catching the window's "
                "last ~1.5 of eight per-sprite slots outside it, by "
                "construction. RED is the correct, reportable outcome: the "
                "manual's own 'during horizontal blank' does not hold for "
                "every channel on this RTL. APPC-pos.c5, "
                "CH4-appC-position.c8.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// ============================================================================
// MUTATION LOG (11-12.09.2026). Every entry below was actually applied by
// this agent - edited into this file, rebuilt under WSL/Verilator
// (`make test`), run, the PASS/FAIL line read from real output (quoted
// verbatim below, not predicted), then reverted and rebuilt clean before the
// next entry. Baseline before this log's first mutation and final clean run
// after the last revert were both "3 passed, 0 failed".
//
// 1. DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER: the first write's target
//    address changed from DMACON_ADDR (0x096) to 0x098, a nearby but
//    non-decoded address (agnus.v's write decode is
//    reg_address[8:1]==DMACON[8:1], and 0x098[8:1] != 0x096[8:1]). Result:
//    "2 passed, 1 failed" - DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER went
//    red (after_a read back 0x0000, not SPREN|DMAEN, since the write landed
//    nowhere DMACONR reads from). Reverted to DMACON_ADDR; rebuilt clean (3
//    passed, 0 failed).
//
// 2. DMACON-IS-WRITE-ONLY-AT-096: the second read's target address changed
//    from DMACON_ADDR (the address under test) to DMACONR_ADDR, so both
//    reads in the assertion aimed at $002. Result: "2 passed, 1 failed" -
//    DMACON-IS-WRITE-ONLY-AT-096 went red (at_096 read back 0x0220, equal
//    to at_002, instead of the expected 0x0000 - the assertion's own
//    "returns 0x0000, not the pattern just written" check failed). Reverted
//    to DMACON_ADDR; rebuilt clean (3 passed, 0 failed).
//
// 3. DMACONR-IS-READ-ONLY-AT-002: the write-attempt's target address
//    changed from DMACONR_ADDR (0x002, the address under test) to
//    DMACON_ADDR (0x096), turning the "attempt" into a real write. Result:
//    "2 passed, 1 failed" - DMACONR-IS-READ-ONLY-AT-002 went red
//    (after_attempt read back 0x0320 = DMAEN|BPLEN|SPREN, not the expected
//    0x0300 = DMAEN|BPLEN unchanged - the write landed because it now used
//    DMACON's own address). Reverted to DMACONR_ADDR; rebuilt clean (3
//    passed, 0 failed).
//
// Machine-readable records, one per line:
// MUTATION DMACON-SEQUENTIAL-WRITES-READ-BACK-IN-ORDER stimulus red: first write's target address moved from $096 to $098 (not decoded by agnus.v's DMACON path); after_a read back 0x0000, not SPREN|DMAEN.
// MUTATION DMACON-IS-WRITE-ONLY-AT-096 stimulus red: the read aimed at $096 was retargeted to $002, so both reads returned the same value (0x0220) instead of 0x0000 vs 0x0220.
// MUTATION DMACONR-IS-READ-ONLY-AT-002 stimulus red: the write attempt's target address moved from $002 to $096, turning the attempt into a real write; after_attempt came back 0x0320 instead of the unchanged 0x0300.
// ============================================================================

// ============================================================================
// MUTATION LOG (EXTENSION). Both entries below were actually
// applied - edited into this file, rebuilt under WSL/Verilator (`make -B
// test`), run, the PASS/FAIL line read from real output (quoted verbatim
// below), then reverted and rebuilt clean before the next entry. Baseline
// before this log's first mutation and final clean run after the last
// revert were both "5 passed, 0 failed".
//
// 1. SPREN-GATES-EVERY-SPRITE-CHANNEL: h_en's DMACON write changed from
//    SET_CLR|DMAEN|SPREN to SET_CLR|DMAEN (SPREN left clear, matching the
//    already-clear [control] run exactly). Result: "4 passed, 1 failed" -
//    SPREN-GATES-EVERY-SPRITE-CHANNEL went red (0/8 distinct channels with
//    SPREN "set" to this mutated value, not 8/8). Reverted to
//    SET_CLR|DMAEN|SPREN; rebuilt clean (5 passed, 0 failed).
//
// 2. SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND:
//    widen_bitplane_into_sprite_window's DDFSTRT write changed from
//    0x0018 (the hardware-minimum start, overlapping the sprite engine's
//    own $18-$38 slot window) to 0x0038 (the normal, non-overlapping
//    start hrm/ch7-dmacon-register-bits's own start_bitplane already
//    uses). Result: "4 passed, 1 failed" -
//    SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND went red (0 of 8
//    channels halved or worse, not >=1 - every channel's DATA/DATB rate
//    stayed at the 172-event baseline once the display fetch no longer
//    reached into the sprite window). Reverted to 0x0018; rebuilt clean (5
//    passed, 0 failed).
//
// Machine-readable records, one per line:
// MUTATION SPREN-GATES-EVERY-SPRITE-CHANNEL stimulus red: h_en's DMACON write dropped SPREN (SET_CLR|DMAEN only); 0/8 distinct channels seen, not 8/8.
// MUTATION SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND stimulus red: DDFSTRT moved from $18 (sprite-window overlap) to $38 (normal, non-overlapping); 0 of 8 channels halved, not >=1.
// ============================================================================

// ============================================================================
// MUTATION LOG (continued). Every entry below was actually applied - edited
// into this file, rebuilt under WSL/Verilator (`make -B test`), run, the
// PASS/FAIL line read from real output, then reverted and rebuilt clean
// before the next entry. Baseline before this log's first mutation and
// final clean run after the last revert were both "7 passed, 1 failed"
// (the one failure is SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK's own genuine
// finding, unaffected by the other mutations below).
//
// 1. SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND was re-verified
//    against its own corrected control (the machine-readable record above
//    still describes it correctly: DDFSTRT $18->$38 on
//    widen_bitplane_into_sprite_window turns it red, unaffected by the
//    control-basis change).
//
// 2. SPREN-PLACES-EVERY-CHANNEL-IN-AUTOMATIC-MODE: feed_real_words's body
//    was emptied (data_in never fed on any grant, matching the pre-repair
//    state). Result: "6 passed, 2 failed" - SPREN-PLACES-EVERY-CHANNEL-IN-
//    AUTOMATIC-MODE went red (field 1 and field 2 both read 0 events/
//    channel for every one of the eight channels, not >=1 in each).
//    Reverted; rebuilt clean (7 passed, 1 failed).
//
// 3. SPRITE-DMA-LOSES-CHANNELS-UNDER-HORIZONTAL-SCROLL:
//    horizontal_scroll_bitplane's DDFSTRT write changed from 0x0030 to
//    0x0038 (the normal, non-scrolled start). Result: "6 passed, 2 failed"
//    - SPRITE-DMA-LOSES-CHANNELS-UNDER-HORIZONTAL-SCROLL went red (0 of 8
//    channels halved or worse, not >=1 - every channel's rate stayed at
//    the 172-event control once DDFSTRT no longer moved). Reverted to
//    0x0030; rebuilt clean (7 passed, 1 failed).
//
// 4. SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK is RED at baseline (the genuine
//    finding: channels 6 and 7 each have a fetch outside hblank) - a
//    failing assertion cannot be made to fail harder, so this substitutes
//    a different, proven-live population through the SAME comparator
//    instead: the counting loop's own `f.sprite < 6` filter, restricting
//    it to sprites 0-5 (shown inside hblank in this SAME run). Result: "8
//    passed, 0 failed" - SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK flipped GREEN
//    (12 of 12 events in hblank for that subset), proving the hblank
//    comparator itself is live and not vacuously red. Reverted to the real,
//    all-eight-channel comparison, which stays red.
//
// Machine-readable records, one per line:
// MUTATION SPREN-PLACES-EVERY-CHANNEL-IN-AUTOMATIC-MODE stimulus red: feed_real_words left data_in unfed on every grant; field 1 and field 2 both read 0 events/channel for all eight channels, not >=1 in each.
// MUTATION SPRITE-DMA-LOSES-CHANNELS-UNDER-HORIZONTAL-SCROLL stimulus red: horizontal_scroll_bitplane's DDFSTRT moved from $0030 to $0038 (no scroll); 0 of 8 channels halved, not >=1.
// MUTATION SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK stimulus green: already red at baseline (channels 6/7 fall outside hblank, the genuine finding) - substituted the proven-live sprite 0-5 subset (all inside hblank in the same run) through the same per-fetch hblank comparator in place of all eight; PASS (12 of 12 in hblank). A failing assertion cannot be made to fail harder, so this proves the comparator is not vacuous. Reverted to the real, all-eight-channel comparison, which stays red.
// ============================================================================
