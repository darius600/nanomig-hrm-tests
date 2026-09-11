// Test purpose: DMACON ($096) and DMACONR ($002) as chapter 4's sprite-facing
//   view of DMA control - SPREN (sprite DMA enable, bit 5), the master DMAEN
//   (bit 9), the DMACONR read-back register, and which bits a write can and
//   cannot reach (DMACON's write-only address; DMACONR's read-only address;
//   BBUSY/BZERO's read-only bits within it).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 278 (Appendix A custom register table). Every claim below cites
//   page 278; none of this package's claims cite any other page.
// Non-goal: reproducing hrm/agnus_top_dmacon. That package already builds
//   this exact fixture (`agnus`, the full chipset top level) and already
//   measures most of the per-bit and register-mechanics propositions this
//   package's claim set also names - see COVERAGE ALREADY MEASURED BY
//   hrm/agnus_top_dmacon below. This package's own assertions are only the
//   propositions that package does not measure.
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
//   hrm/agnus_top_dmacon.
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
//     hrm/agnus_top_dmacon's WALKING-BIT-* loop (SET from zero, then CLR
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
// write/read-back like any other position - hrm/agnus_top_dmacon's
// WALKING-BIT-11 and WALKING-BIT-12 already measure exactly that (SET from
// zero, CLR from all-ones, same as every other bit), labelled "bit11(X)" /
// "bit12(X)" - no function claimed, matching page 278's silence. This
// package adds nothing about bits 11/12: there is no proposition to assert,
// and the generic SET/CLR mechanics they do have are already measured
// elsewhere. Measured, not asserted - reported in this package's own build
// report, not invented here.
//
// ============================================================================
// COVERAGE ALREADY MEASURED BY hrm/agnus_top_dmacon (read in full before
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
// them as "not asserted here - already measured by hrm/agnus_top_dmacon:
// <assertion name>", per this task's instruction to say so rather than
// re-test them.
//
// ============================================================================
// PLANNED ASSERTIONS (names, cited-id groups and intent only - no RTL line
// numbers beyond what the claims' own classify.evidence already handed over;
// no fixture code below this comment block, matching hrm/agnus_top_dmacon's
// own spec-before-RTL gate. Added in the next commit, after agnus.v is
// opened to confirm the write_reg/read_reg pattern copied from
// hrm/agnus_top_dmacon still applies unchanged - that package's own
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
//      noted above under hrm/agnus_top_dmacon's WALKING-BIT-*/
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
// hrm/agnus_top_dmacon (noted above) and these three assertions' own
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
// hrm/agnus_top_dmacon's already-committed file (both handed to or
// available to the building agent as inventory, not discovered by this
// agent opening agnus.v). Exact confirmation of the write_reg/read_reg retry
// shape against this fixture, and the mutation log, are answered in the next
// commit, after agnus.v is read.
// ============================================================================
