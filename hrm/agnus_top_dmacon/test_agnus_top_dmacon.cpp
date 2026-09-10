// Test purpose: DMACON at $096 and DMACONR at $002 - the SET/CLR write rule,
//   read-back of bits 0-12, BZERO (13) and BBUSY (14) as read-only blitter
//   status, and the enable/priority bits `agnus` actually consumes (SPREN 5,
//   BLTEN 6, COPEN 7, BPLEN 8, DMAEN 9, BLTPRI 10).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 186-187 (blitter status flag names), 198 (DMAF_BLITHOG / bus-hog
//   behaviour), 278 (Appendix A custom register table), 289-290 (Appendix B
//   register descriptions).
// Non-goals: the five agnus_audiodma / agnus_diskdma claims that packages.md
//   discards with `covered_by` pointing at this package's bit-position
//   assertions (Paula, not `dmacon`, gates those requests per agnus.v:326-357
//   per the build brief - not reopened here). Also not this package's job:
//   anything about audio or disk DMA *behaviour* beyond their DMACON bit
//   name/position (bits 0-4), which IS in scope as part of "read-back of
//   bits 0-12".
//
// BOUNDARY: `agnus` (top-level chipset module), NOT `agnus_blitter`.
//
// CLAIMS (verbatim quote + restatement from
// work/2026-09-08_fpga-090_classified.jsonl, filtered on classify.package ==
// "PKG-AGNUS-DMACON"; all 72 records have classify.status == "testable",
// classify.observable == "yes, at `agnus`"). Grouped by page and by the
// proposition each supports; ids that are the same proposition read
// independently by two readers (sol, opus) or under two manual wordings are
// grouped together and will be cited together on the assertion(s) that test
// that proposition.
//
// -- page 186: blitter-done status flag, alternate ("software") name --
//   6-blitter-done-flag.c5 (sol): quote "DMAF_BLTDONE (bit 14) in  DMACONR .
//     This flag is set when a blit is in" -> "DMAF_BLTDONE is DMACONR bit 14."
//   ch6-done.c5 (opus): same quote -> "The blitter done flag is provided as
//     DMAF_BLTDONE in DMACONR."
//   ch6-done.c6 (opus): same quote -> "DMAF_BLTDONE is bit 14."
//
// -- page 187: blitter-zero status flag, alternate ("software") name --
//   6-zero-flag.c5 (sol): quote "and can be read from bit (13) DMAF_BLTNZERO
//     of the  DMACONR  register." -> "DMAF_BLTNZERO is read from DMACONR bit
//     13."
//   ch6-zero.c5 (opus): same quote -> "The zero flag is read from
//     DMAF_BLTNZERO of the DMACONR register."
//   ch6-zero.c6 (opus): same quote -> "DMAF_BLTNZERO is bit 13."
//
// -- page 198: DMAF_BLITHOG (= BLTPRI) bus-hog-vs-share behaviour --
//   6-blitter-priority.c12 (sol): quote "If DMAF_BLITHOG is a 0, the DMA
//     manager will monitor the 68000 cycle" -> "When DMAF_BLITHOG=0, the DMA
//     manager monitors unsatisfied 68000 cycle requests."
//   6-blitter-priority.c7 (sol): quote "This bit is called DMAF_BLITHOG (bit
//     10) and is" -> "DMAF_BLITHOG is bit 10."
//   6-blitter-priority.c8 (sol): quote "in register  DMACON ." ->
//     "DMAF_BLITHOG resides in DMACON."
//   6-blitter-priority.c9 (sol): quote "If DMAF_BLITHOG is a 1, the blitter
//     will keep the bus for every available" -> "When DMAF_BLITHOG=1, the
//     blitter keeps the bus for every available Chip-memory cycle."
//   ch6-sysdma.c57 (opus): quote "blocked from bus access.  This bit is
//     called DMAF_BLITHOG (bit 10) and is" -> "The blitter DMA mode bit is
//     called DMAF_BLITHOG and is in register DMACON."
//   ch6-sysdma.c58 (opus): same quote -> "DMAF_BLITHOG is bit 10."
//   ch6-sysdma.c59 (opus): quote "If DMAF_BLITHOG is a 1, the blitter will
//     keep the bus for every available" -> "If DMAF_BLITHOG is a 1 the
//     blitter keeps the bus for every available Chip memory cycle."
//   ch6-sysdma.c61 (opus): quote "If DMAF_BLITHOG is a 0, the DMA manager
//     will monitor the 68000 cycle" -> "If DMAF_BLITHOG is a 0 the DMA
//     manager monitors the 68000 cycle requests."
//
// -- page 278 (Appendix A), DMACON write decode / overall function --
//   6-appa-dmacon.c1 (sol): quote "DMACON     096      W     A D P   DMA
//     control write (clear or set)" -> "DMACON is at custom-register offset
//     $096."
//   6-appa-dmacon.c2 (sol): same quote -> "DMACON is writable."
//   6-appa-dmacon.c3 (sol): same quote -> "DMACON controls DMA by clearing or
//     setting selected bits."
//   ch6-appa-dmacon.c1 (opus): same quote -> "DMACON is at address 096."
//   ch6-appa-dmacon.c2 (opus): same quote -> "DMACON is write-only."
//   ch6-appa-dmacon.c4 (opus): same quote -> "DMACON is the DMA control write
//     register, clear or set."
//   ch6-appa-dmacon.c9 (opus): quote "This register controls all of the DMA
//     channels and" -> "This register controls all of the DMA channels."
//
// -- page 278 (Appendix A), DMACONR read decode / overall function --
//   6-appa-dmacon.c4 (sol): quote "DMACONR    002      R     A   P   DMA
//     control (and blitter status) read" -> "DMACONR is at custom-register
//     offset $002."
//   6-appa-dmacon.c5 (sol): same quote -> "DMACONR is read-only."
//   6-appa-dmacon.c6 (sol): same quote -> "DMACONR exposes DMA control
//     state."
//   6-appa-dmacon.c7 (sol): same quote -> "DMACONR exposes blitter status."
//   ch6-appa-dmacon.c5 (opus): same quote -> "DMACONR is at address 002."
//   ch6-appa-dmacon.c6 (opus): same quote -> "DMACONR is read-only."
//   ch6-appa-dmacon.c8 (opus): same quote -> "DMACONR is the DMA control and
//     blitter status read register."
//   ch6-appa-dmacon.c10 (opus): quote "contains blitter DMA status bits." ->
//     "This register contains blitter DMA status bits."
//
// -- page 278 (Appendix A), SET/CLR (bit 15) write semantics --
//   6-appa-dmacon.c8 (sol): quote "15    SET/CLR     Set/clear control bit.
//     Determines" -> "DMACON bit 15 is SET/CLR."
//   6-appa-dmacon.c9 (sol): quote "if bits written with a 1 get set or" ->
//     "SET/CLR determines whether other bits written as 1 are set or
//     cleared."
//   6-appa-dmacon.c10 (sol): quote "cleared.  Bits written with a zero" ->
//     "Bits written as 0 to DMACON remain unchanged."
//   ch6-appa-dmacon.bit15 (opus): quote "15    SET/CLR     Set/clear control
//     bit. Determines" -> "Bit 15 of DMACON/DMACONR is SET/CLR."
//   ch6-appa-dmacon.setclr (opus): quote "if bits written with a 1 get set
//     or" -> "SET/CLR determines whether bits written with a 1 get set or
//     cleared."
//   ch6-appa-dmacon.setclr2 (opus): quote "cleared.  Bits written with a
//     zero" -> "Bits written with a zero are unchanged."
//
// -- page 278 (Appendix A), per-bit position/name table, bits 0-12 --
//   ch6-appa-dmacon.bit00 (opus): "00    AUD0EN      Audio channel 0 DMA
//     enable" -> "Bit 0 of DMACON/DMACONR is AUD0EN."
//   ch6-appa-dmacon.bit01 (opus): "01    AUD1EN      Audio channel 1 DMA
//     enable" -> "Bit 1 of DMACON/DMACONR is AUD1EN."
//   ch6-appa-dmacon.bit02 (opus): "02    AUD2EN      Audio channel 2 DMA
//     enable" -> "Bit 2 of DMACON/DMACONR is AUD2EN."
//   ch6-appa-dmacon.bit03 (opus): "03    AUD3EN      Audio channel 3 DMA
//     enable" -> "Bit 3 of DMACON/DMACONR is AUD3EN."
//   ch6-appa-dmacon.bit04 (opus): "04    DSKEN       Disk DMA enable" ->
//     "Bit 4 of DMACON/DMACONR is DSKEN."
//   6-appa-dmacon.c26 (sol): "05    SPREN       Sprite DMA enable" -> "DMACON
//     bit 5, SPREN, enables sprite DMA."
//   ch6-appa-dmacon.bit05 (opus): same quote -> "Bit 5 of DMACON/DMACONR is
//     SPREN."
//   6-appa-dmacon.c25 (sol): "06    BLTEN       Blitter DMA enable" ->
//     "DMACON bit 6, BLTEN, enables blitter DMA."
//   ch6-appa-dmacon.bit06 (opus): same quote -> "Bit 6 of DMACON/DMACONR is
//     BLTEN."
//   6-appa-dmacon.c24 (sol): "07    COPEN       Copper DMA enable" ->
//     "DMACON bit 7, COPEN, enables Copper DMA."
//   ch6-appa-dmacon.bit07 (opus): same quote -> "Bit 7 of DMACON/DMACONR is
//     COPEN."
//   6-appa-dmacon.c23 (sol): "08    BPLEN       Bitplane DMA enable" ->
//     "DMACON bit 8, BPLEN, enables bitplane DMA."
//   ch6-appa-dmacon.bit08 (opus): same quote -> "Bit 8 of DMACON/DMACONR is
//     BPLEN."
//   6-appa-dmacon.c22 (sol): "09    DMAEN       Enable all DMA below" ->
//     "DMACON bit 9, DMAEN, enables all lower-numbered DMA classes."
//   ch6-appa-dmacon.bit09 (opus): same quote -> "Bit 9 of DMACON/DMACONR is
//     DMAEN."
//   ch6-appa-dmacon.dmaen2 (opus): same quote -> "DMAEN enables all the DMA
//     channels listed below it in the table."
//   6-appa-dmacon.c17 (sol): "10    BLTPRI      Blitter DMA priority" ->
//     "DMACON bit 10 is BLTPRI."
//   ch6-appa-dmacon.bit10 (opus): same quote -> "Bit 10 of DMACON/DMACONR is
//     BLTPRI."
//   ch6-appa-dmacon.bltpri1 (opus): same quote -> "BLTPRI is the blitter DMA
//     priority bit."
//   6-appa-dmacon.c16 (sol): "11    X" -> "DMACON bit 11 is unused."
//   ch6-appa-dmacon.bit11 (opus): same quote -> "Bit 11 of DMACON/DMACONR is
//     marked X, that is unused."
//   6-appa-dmacon.c15 (sol): "12    X" -> "DMACON bit 12 is unused."
//   ch6-appa-dmacon.bit12 (opus): same quote -> "Bit 12 of DMACON/DMACONR is
//     marked X, that is unused."
//
// -- page 278 (Appendix A), BZERO (bit 13) read-only --
//   6-appa-dmacon.c13 (sol): "13    BZERO       Blitter logic  zero status
//     bit" -> "DMACONR bit 13 is BZERO."
//   6-appa-dmacon.c14 (sol): "(read only)." -> "BZERO is the read-only
//     blitter logic-zero status bit."
//   ch6-appa-dmacon.bit13 (opus): "13    BZERO       Blitter logic  zero
//     status bit" -> "Bit 13 of DMACON/DMACONR is BZERO."
//   ch6-appa-dmacon.bzero2 (opus): "(read only)." -> "BZERO, the blitter
//     logic zero status bit, is read only."
//
// -- page 278 (Appendix A), BBUSY (bit 14) read-only --
//   6-appa-dmacon.c11 (sol): "14    BBUSY       Blitter busy status bit
//     (read only)" -> "DMACONR bit 14 is BBUSY."
//   6-appa-dmacon.c12 (sol): same quote -> "BBUSY is the read-only
//     blitter-busy status bit."
//   ch6-appa-dmacon.bit14 (opus): same quote -> "Bit 14 of DMACON/DMACONR is
//     BBUSY."
//   ch6-appa-dmacon.bbusy2 (opus): same quote -> "BBUSY, the blitter busy
//     status bit, is read only."
//
// -- page 278 (Appendix A), BLTPRI / BLS pin --
//   6-appa-dmacon.c20 (sol): quote "pin, preventing micro from" ->
//     "BLTPRI disables the /BLS pin."
//   6-appa-dmacon.c21 (sol): quote "stealing any bus cycles while" ->
//     "With BLTPRI active, the microprocessor cannot steal bus cycles while
//     blitter DMA runs."
//   ch6-appa-dmacon.bltpri4 (opus): quote "\"blitter nasty\") (disables /BLS"
//     -> "BLTPRI disables the /BLS pin."
//   ch6-appa-dmacon.bltpri5 (opus): quote "pin, preventing micro from" ->
//     "Disabling the /BLS pin prevents the micro from stealing any bus
//     cycles while blitter DMA is running."
//
// -- page 289 (Appendix B), DMACONR address (scan) --
//   ch6-appb-dmaconr.addr (opus, source_kind scan): "Appendix B places
//     DMACONR at address 002 and describes it as \"DMA control (and blitter
//     status) read\"."
//
// -- page 290 (Appendix B), DMACON address (scan) --
//   ch6-appb-dmacon.addr (opus, source_kind scan): "Appendix B places DMACON
//     at address 096 and describes it as \"DMA control write (clear or
//     set)\"."
//
// That is all 72 records filtered on classify.package == "PKG-AGNUS-DMACON"
// (grep -c on work/2026-09-08_fpga-090_classified.jsonl confirmed 72 before
// this list was written).
//
// PLANNED ASSERTIONS (names, cited-id groups and intent only; no RTL line
// numbers, no exact register-bit-width facts beyond what the build brief's
// own section D handed over - `aen`/`rd`/`hwr`/`lwr`/`address_in`/`data_in`
// in, `data_out` out, `dbr`/`dbwe`/`address_out` for cycle-taking, `bls` for
// BLTPRI - no fixture code below this comment block. Those are added in the
// next commit, after agnus.v and its submodules are opened):
//
//  Register-mechanics group (observable on data_out alone):
//   1. DMACON-WRITE-DECODED-AT-096: a write through aen/hwr/address_in=$096/
//      data_in changes what DMACONR later reads back. Cites the "DMACON
//      write decode / overall function" group above (7 ids) plus the
//      Appendix B corroboration ch6-appb-dmacon.addr.
//   2. A control (no page) on the same write path: the identical data_in
//      pattern aimed at a different address must NOT change what DMACONR
//      reads back - rules out a fixture that reports "written" regardless
//      of address.
//   3. DMACONR-READ-DECODED-AT-002: a read through aen/rd/address_in=$002
//      reflects two DIFFERENT known dmacon states differently (not a stuck
//      constant). Cites the "DMACONR read decode / overall function" group
//      (8 ids) plus ch6-appb-dmaconr.addr.
//   4-16. Thirteen WALKING-BIT assertions, one per DMACON/DMACONR bit
//      position 0..12: write that bit alone (SET), confirm DMACONR reads
//      back exactly that bit and no other; write it alone again (CLR),
//      confirm it and only it clears. Each of the 13 cites its own
//      bit-position ids from the per-bit table above (AUD0EN..DSKEN,
//      SPREN..BLTPRI, the two unused positions) - 23 ids across the 13.
//      Because each iteration checks that the OTHER twelve bits are
//      unaffected, this group's evidence also carries the "bits written
//      with 0 remain unchanged" half of the SET/CLR proposition.
//   17. SETCLR-MULTIBIT-DIRECTION: one SET write sets a multi-bit pattern,
//      one CLR write clears a subset of it while leaving the rest of that
//      same pattern untouched - the OR/AND-NOT distinction the single-bit
//      walk above does not exercise on its own. Cites the "SET/CLR write
//      semantics" group (6 ids).
//   18. BZERO-READONLY-WITH-POSITIVE-CONTROL: an attempt to write bit 13
//      through the normal DMACON write path, compared before/after against
//      a writable control bit (DMAEN, bit 9) written in the SAME operation -
//      the control bit must change and bit 13 must not, proving the write
//      path is live while bit 13 still does not move. Cites the BZERO
//      read-only group (page 278, 4 ids) plus the page 187 DMAF_BLTNZERO
//      alternate-name group (3 ids) - both name the same bit.
//   19. BBUSY-READONLY-WITH-POSITIVE-CONTROL: identical shape for bit 14.
//      Cites the BBUSY read-only group (page 278, 4 ids) plus the page 186
//      DMAF_BLTDONE alternate-name group (3 ids).
//
//  Consumption group (observable via dbr/dbwe/address_out; each holds DMAEN
//  set throughout per the build brief's own gating fact for these bits, and
//  each assertion's "bit set" measurement is simultaneously the control the
//  build brief requires for that same assertion's "bit clear" measurement -
//  same fixture, same window, both states run and compared):
//   20. SPREN-GATES-SPRITE-DMA (bit 5).
//   21. BLTEN-GATES-BLITTER-DMA (bit 6).
//   22. COPEN-GATES-COPPER-DMA (bit 7).
//   23. BPLEN-GATES-BITPLANE-DMA (bit 8).
//   24. DMAEN-MASTER-GATES-ALL-DMA-BELOW (bit 9): with every one of
//      SPREN/BLTEN/COPEN/BPLEN held set, toggling DMAEN alone toggles all
//      four engines' cycles together - the "enable all DMA below" claim,
//      distinct from any single engine's own bit.
//   25. BLTPRI-BUS-SHARE-UNDER-CONTENTION (bit 10): under a sustained /BLS
//      request, BLTPRI=0 yields periodic gaps in blitter bus activity
//      (DMAF_BLITHOG=0 wording, page 198) and BLTPRI=1 shows none / takes
//      every available cycle (DMAF_BLITHOG=1 wording, page 198, and the
//      Appendix A "/BLS pin disabled" wording, page 278). Cites the page 198
//      group (8 ids) and the "BLTPRI / BLS pin" group (4 ids); the bit's own
//      position/name ids (6-appa-dmacon.c17, ch6-appa-dmacon.bit10,
//      ch6-appa-dmacon.bltpri1) are cited on assertion 4-16's bit-10
//      iteration and not repeated as a separate structural claim here -
//      this assertion is the FUNCTIONAL claim about the same bit.
//
//   20-24 each also re-cite their bit's own "enables X DMA" restatement
//   (6-appa-dmacon.c22/c23/c24/c25/c26, ch6-appa-dmacon.dmaen2) alongside the
//   position ids already cited on the matching walking-bit iteration, since
//   those restatements are compound (name+position AND function) and belong
//   on both the structural and the functional assertion.
//
// Twenty-six planned assertions. Every one of the 72 ids listed above is
// cited on at least one of them; none are being turned into an assertion by
// recollection - all 72 came from a grep filter on classify.package, quoted
// in full above.
//
// No RTL has been opened yet to write this comment block: the port names
// (`aen`, `rd`, `hwr`, `lwr`, `address_in`, `data_in`, `data_out`, `dbr`,
// `dbwe`, `address_out`, `bls`) and the six consumed-bit numbers come from
// the build brief's own section B/D and from classify.evidence's prose
// (both handed to the building agent as part of this task's inventory, not
// discovered by this agent opening agnus.v). Exact address-decode widths,
// per-engine "wants a cycle" preconditions, and the BLTPRI bus-share
// threshold are RTL questions and are answered in the next commit, after
// agnus.v and its submodules are read.

int main() {
    // Placeholder - the real fixture, fixture helpers and assertion bodies
    // are added in the next commit, after this specification is committed
    // on its own.
    return 0;
}
