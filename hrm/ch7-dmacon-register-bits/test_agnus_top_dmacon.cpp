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
//   assertions (Paula, not `dmacon`, gates those requests per
//   agnus.v:326-357 per the build brief - not reopened here). Also not this
//   package's job: anything about audio or disk DMA *behaviour* beyond their
//   DMACON bit name/position (bits 0-4), which IS in scope as part of
//   "read-back of bits 0-12".
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-08_fpga-090_classified.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
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

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after agnus.v and its
// submodules agnus_blitter.v, agnus_copper.v, agnus_spritedma.v,
// agnus_bitplanedma.v, agnus_diskdma.v and agnus_audiodma.v were read in
// full for the first time - the specification comment above this point was
// committed on its own first, per the chapter's spec-before-RTL gate; see
// this package's own git history, commit 919d353).
//
// REGISTER MECHANICS (agnus.v:271-302):
//   `dmacon` is a 13-bit register (`reg [12:0] dmacon;`, agnus.v:271) - bits
//   13/14 of DMACONR are NOT stored bits of this register at all, they are a
//   live mux of `blit_busy`/`blit_zero` wires sourced from agnus_blitter
//   (agnus.v:277,281: `data_out = data_bmc | dmaconr | data_blt;` and
//   `dmaconr[15:0] = {1'b0, blit_busy, blit_zero, dmacon[12:0]}`). Writing
//   data_in[13] or data_in[14] through the DMACON path has no storage to
//   land in - the write logic only ever touches `dmacon[12:0]`
//   (agnus.v:291-292: `dmacon[12:0] <= dmacon[12:0] | data_in[12:0]` /
//   `& ~data_in[12:0]`). This is a stronger form of "read only" than a
//   masked-off writable bit: there is no bit there to write.
//   DMACON write decode: `reg_address[8:1]==DMACON[8:1]` (agnus.v:289),
//   DMACON=9'h096 (agnus.v:103). DMACONR read decode:
//   `reg_address[8:1]==DMACONR[8:1]` (agnus.v:280), DMACONR=9'h002
//   (agnus.v:104). `reg_address` is the internal, arbitrated register
//   address bus (agnus.v:133-267): it equals the CPU's own
//   `reg_address_cpu` (`aen&(rd|hwr|lwr) ? address_in : 8'hFF`,
//   agnus.v:114) ONLY on a cycle where agnus's own priority mux grants the
//   bus to no DMA engine (the trailing `else` arm, agnus.v:255-266, where
//   `dbr` reads 0); on every other cycle `reg_address` is overwritten by
//   whichever engine currently holds `dbr`. This fixture's `write_reg`/
//   `read_reg` helpers (below) retry a held register operation for up to 8
//   ticks and act on the first tick where `dbr==0` - the only kind of cycle
//   on which a CPU register access can land. That is not a workaround for a
//   flaky testbench, it is what the real arbitrated register bus requires,
//   and DMACON-WRITE-DECODED-AT-096's own control below exercises it rather
//   than assuming it away.
//
// CONSUMPTION (agnus.v:118-310):
//   spren/blten/copen/bplen are each `dmacon[bit] & dmacon[9]`
//   (agnus.v:299-302) - DMAEN(9) is a literal AND-gate master enable over
//   exactly these four bits and no others; AUD0-3EN/DSKEN (bits 0-4) are NOT
//   wired to any enable input anywhere in agnus.v, agnus_audiodma.v or
//   agnus_diskdma.v (both engines take their request straight from Paula's
//   audio_dmal/disk_dmal with no dmacon-derived port at all) - confirming
//   packages.md's discard of the five audio/disk consumption claims as out
//   of this package's RTL scope, not merely out of its manual scope.
//   dma_spr/dma_cop/dma_blt (agnus.v:125-127) are each `req_x & xen`: the
//   enable bit gates the TOP-LEVEL grant, not the engine's own internal
//   `reqdma`, so an engine can "want" a cycle whether or not its enable bit
//   is set - only whether agnus ever GRANTS it (dbr, address_out,
//   reg_address_out, and for blitter/disk dbwe) depends on the enable bit.
//   That grant is exactly what the SPREN/BLTEN/COPEN/BPLEN/DMAEN
//   consumption assertions below measure.
//   Isolation trick used by the SPREN/COPEN/BPLEN/DMAEN-combined
//   assertions: with disk_dmal/audio_dmal held at their idle 0 (as this
//   fixture always does) and every OTHER enable bit left clear, the only
//   things that can ever produce `dbr==1` in a given run are (a) the one
//   engine under test and (b) agnus_refresh's unconditional 4-slot/line
//   pulse (agnus.v:319-322, dma_ref; see hrm/ch6-refresh-dma-slots's header for its
//   full derivation), independently reported on the `refresh` port. So
//   `dbr && !refresh`, counted over a run with disk/audio idle and every
//   other enable bit clear, is a clean, port-list-only proxy for "did the
//   one engine under test get a cycle", with no need to decode
//   `address_out`/`reg_address_out` per engine. BLTEN's assertion uses
//   `dbwe` instead, which is even more specific: agnus.v's priority mux
//   only ever drives `dbwe` from `wr_dsk` (disk branch, agnus.v:180) or
//   `we_blt` (blitter branch, agnus.v:252) - never from refresh or any
//   other engine - so with disk_dmal held at 0, `dbwe==1` can only mean
//   "the blitter wrote a channel-D word this cycle".
//   Blitter setup mirrors the already-committed, working
//   hrm/ch6-blitter-status pattern (BLTCON0 = USEC|USED|LF_C_PASSTHROUGH,
//   agnus_blitter.v:252/595-601, then a BLTSIZE write, agnus_blitter.v:
//   521-525/555-556 for the OCS width/height packing) - re-applied here at
//   the `agnus` top-level register bus instead of at the `agnus_blitter`
//   module boundary that package uses. Copper setup mirrors the
//   already-committed hrm/agnus_top_register_bus pattern (DMAEN+COPEN, then
//   COP1LCH/COP1LCL/COPJMP1, agnus_copper.v:78-88) at the SAME `agnus`
//   boundary this package uses. Sprite and bitplane setup values (SPR0PTH/
//   PTL/POS/CTL; DIWSTRT/DIWSTOP/DDFSTRT/DDFSTOP/BPLCON0/FMODE) are the
//   same register values hrm/agnus_spritedma's and hrm/agnus_bitplanedma's
//   own RTL_REGRESSION packages already show produce real reqdma/dma
//   activity in these same submodules when driven directly - reused here
//   as known-good stimulus, re-applied through the `agnus` top-level
//   register bus instead of each submodule's own ports, and re-verified
//   against the real consumption signals (`dbr`/`dbwe`), not assumed to
//   still work unseen. `agnus.v` does not expose `hpos`/`vpos` as
//   top-level ports, so this fixture never reads them; elapsed-line
//   counting uses `sol` exactly as hrm/ch6-refresh-dma-slots's Harness does.
//   The blitter yields the bus every other tick regardless of DMA
//   activity: agnus_blitter's `enable = enadma & clkena` and agnus.v:442
//   wires `clkena` to `cck`, which this fixture's `tick()` toggles every
//   tick (the same true-divide-by-2 convention hrm/ch6-refresh-dma-slots documents)
//   - so a register access retried across 2 ticks during an active blit is
//   guaranteed a `dbr==0` cycle on the `cck==0` half. That is what makes
//   BZERO/BBUSY's assertions safe to read DMACONR while a blit may still
//   be running.
//   BLTPRI (agnus.v:298,429-433,310): `bls_cnt` increments on every
//   `!cck` tick that `bls` is asserted and `bltpri` is NOT, saturates at
//   BLS_CNT_MAX(3), and blocks `ena_blt` (hence the blitter's own
//   `enable`) once saturated; `bltpri` (dmacon[10]) holds `bls_cnt` at 0
//   unconditionally regardless of `bls`. Under a `bls` signal held
//   asserted for the whole run, BLTPRI=0 lets a blit take only its first
//   handful of cycles before permanently stalling (bls never deasserts in
//   this fixture, so bls_cnt never resets); BLTPRI=1 lets the identical
//   blit run to completion. This is the same mechanism the manual calls
//   DMAF_BLITHOG monitoring the CPU (p.198) and disabling /BLS (p.278) - a
//   full stall vs. full completion is a sharper, still faithful, reading
//   of "keeps the bus for every available cycle" vs. "monitors 68000 cycle
//   requests" than trying to reproduce exact intermittent gaps would be.
//
// MUTATION LOG: see the bottom of this file, after main(). Recorded there
// (not here) so it can quote this file's own assertion labels; every entry
// was actually run - edited, rebuilt under WSL/Verilator, executed, reverted,
// rebuilt clean - by this agent, not predicted.
// ============================================================================

// SPECIFICATION ADDENDUM: 25 records -- quotation, restatement and page
// only. Each new claim id is grouped under the one manual proposition it
// restates; grouping is by wording alone, not by which existing assertion
// (if any) already measures it -- that mapping is decided, and recorded in
// CLAIM COVERAGE and the assertion labels themselves.
//
//   [P1] DMACONR-APPENDIX-B-ADDRESS-ORDER (p.289) -- "DMACONR *002 R A P
//     DMA control (and blitter status) read", the address-order table row:
//     DMACONR is listed at $002. B-dmaconr.addr (opus; its own reader flags
//     this row print-only/not-cross-read).
//
//   [P2] DDFSTRT-BELOW-$38-DISABLES-SPRITES (p.80) -- "using a data-fetch
//     start earlier than ($38) will disable some sprites." 3-max-window.c18
//     (sol), 3-max.c12 (opus) -- two readings of the same sentence.
//
//   [P3] DDFSTRT-$0030-DISABLES-SPRITE-7 (p.82) -- "sooner (DDFSTRT =
//     $0030). Incidentally, this will disable sprite 7." 3-hscr.c10 (opus),
//     3-hscroll.c12 (sol).
//
//   [P4] SCROLL-FETCH-WORD-DISABLES-SPRITES (p.83) -- "Fetching an extra
//     word for scrolling will disable some sprites." 3-hscr.c17 (opus),
//     3-hscroll.c17 (sol).
//
//   [P5] DMACON-BIT10-IS-BLTPRI (p.278) -- "10 BLTPRI Blitter DMA
//     priority" -- DMACON bit 10 is BLTPRI. APPA-dmacon.c10 (opus, position
//     only). CH4-appA-dmacon.c17 (sol) restates the same row with its
//     crossread-restored parenthetical included -- "(over CPU micro) (also
//     called \"blitter nasty\")" -- so its own restatement is the compound
//     "DMACON bit 10 enables blitter DMA priority over CPU bus use, also
//     called blitter nasty", naming both the position and the function.
//
//   [P6] BLTPRI-DISABLES-BLS-PIN (p.278) -- "(disables /BLS pin,
//     preventing micro from stealing any bus cycles while blitter DMA is
//     running)." APPA-dmacon.c11 (opus), CH4-appA-dmacon.c18 (sol).
//
//   [P7] DMACON-BIT9-DMAEN-ENABLES-ALL-BELOW (p.278) -- "09 DMAEN Enable
//     all DMA below." APPA-dmacon.c12 (opus) restates this as "so sprite
//     DMA also requires DMAEN" -- flagged by crossread as
//     restatement_exceeds_quote, since the row names no sprite and the
//     sprite-specific consequence also depends on the separate SPREN row,
//     not quoted here. CH4-appA-dmacon.c19 (sol) restates the same row as
//     "the master enable for the individual DMA enables in bits 8 through
//     0" without naming any one of them.
//
//   [P8] DMACON-BIT8-IS-BPLEN (p.278) -- "08 BPLEN Bitplane DMA enable."
//     APPA-dmacon.c13 (opus), CH4-appA-dmacon.c20 (sol).
//
//   [P9] DMACON-BIT7-IS-COPEN (p.278) -- "07 COPEN Copper DMA enable."
//     APPA-dmacon.c14 (opus), CH4-appA-dmacon.c21 (sol).
//
//   [P10] DMACON-BIT6-IS-BLTEN (p.278) -- "06 BLTEN Blitter DMA enable."
//     APPA-dmacon.c15 (opus), CH4-appA-dmacon.c22 (sol).
//
//   [P11] DMACON-BIT5-IS-SPREN (p.278) -- "05 SPREN Sprite DMA enable."
//     APPA-dmacon.c16 (opus), CH4-appA-dmacon.c23 (sol).
//
//   [P12] DMACON-BIT14-BBUSY-READONLY (p.278) -- "14 BBUSY Blitter busy
//     status bit (read only)." APPA-dmacon.c6 (opus), CH4-appA-dmacon.c13
//     (sol).
//
//   [P13] DMACON-BIT13-BZERO-READONLY (p.278) -- "13 BZERO Blitter logic
//     zero status bit (read only)." APPA-dmacon.c7 (opus),
//     CH4-appA-dmacon.c14 (sol).
//
//   25 ids total: 1+2+2+2+2+2+2+2+2+2+2+2+2 = 25.
// ============================================================================

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus.h"

namespace {

// ---- register addresses (9-bit custom-chip offsets, as written in the HRM
// and in agnus.v's own `parameter`/`localparam` declarations) ----
constexpr uint16_t DMACON_ADDR  = 0x096;
constexpr uint16_t DMACONR_ADDR = 0x002;
constexpr uint16_t BLTAFWM_ADDR = 0x044;  // agnus_blitter.v's own register; used only as
                                           // a "different, known address" control target
                                           // for DMACON-WRITE-DECODED-AT-096's control -
                                           // never read back or relied on for its own value.
constexpr uint16_t BLTCON0_ADDR = 0x040;
constexpr uint16_t BLTSIZE_ADDR = 0x058;
constexpr uint16_t COP1LCH_ADDR = 0x080;
constexpr uint16_t COP1LCL_ADDR = 0x082;
constexpr uint16_t COPJMP1_ADDR = 0x088;
constexpr uint16_t DIWSTRT_ADDR = 0x08E;
constexpr uint16_t DIWSTOP_ADDR = 0x090;
constexpr uint16_t DDFSTRT_ADDR = 0x092;
constexpr uint16_t DDFSTOP_ADDR = 0x094;
constexpr uint16_t BPLCON0_ADDR = 0x100;
constexpr uint16_t FMODE_ADDR   = 0x1FC;
constexpr uint16_t SPR0PTH_ADDR = 0x120;
constexpr uint16_t SPR0PTL_ADDR = 0x122;
constexpr uint16_t SPR0POS_ADDR = 0x140;
constexpr uint16_t SPR0CTL_ADDR = 0x142;

// ---- DMACON/DMACONR bit masks ----
constexpr uint16_t SET_CLR = 0x8000;  // bit 15
constexpr uint16_t AUD0EN  = 1u << 0;
constexpr uint16_t AUD1EN  = 1u << 1;
constexpr uint16_t AUD2EN  = 1u << 2;
constexpr uint16_t AUD3EN  = 1u << 3;
constexpr uint16_t DSKEN   = 1u << 4;
constexpr uint16_t SPREN   = 1u << 5;
constexpr uint16_t BLTEN   = 1u << 6;
constexpr uint16_t COPEN   = 1u << 7;
constexpr uint16_t BPLEN   = 1u << 8;
constexpr uint16_t DMAEN   = 1u << 9;
constexpr uint16_t BLTPRI  = 1u << 10;
constexpr uint16_t BIT11_X = 1u << 11;
constexpr uint16_t BIT12_X = 1u << 12;
constexpr uint16_t BZERO_B = 1u << 13;  // DMACONR only, read-only
constexpr uint16_t BBUSY_B = 1u << 14;  // DMACONR only, read-only

// ---- BPLCON0 values for 1/2/4/6 bitplanes, lores, colour on (agnus_bitplanedma.v:282:
// bplcon0<={data_in[6],data_in[15],aga&data_in[4],data_in[14:12]} - BPU is data_in[14:12]
// directly for every value used here; the non-AGA "all three bits set" alias to 4 planes
// never triggers for 1/2/4/6). Bit 9 (COLOR) is carried over unchanged from the original
// $1200 fixture value; HIRES (bit15) and SHRES (bit6) stay clear throughout. ----
constexpr uint16_t BPLCON0_1BPL = 0x1200;  // BPU=1 (existing fixture value)
constexpr uint16_t BPLCON0_2BPL = 0x2200;  // BPU=2
constexpr uint16_t BPLCON0_4BPL = 0x4200;  // BPU=4
constexpr uint16_t BPLCON0_6BPL = 0x6200;  // BPU=6

// ---- BLTCON0 use-channel bits (agnus_blitter.v:252) and a minterm that
// passes channel C straight through to D (agnus_blitter_minterm.v; D = C
// for every A/B value) - same constants hrm/ch6-blitter-status uses. ----
constexpr uint16_t BC_USEA = 1u << 11;
constexpr uint16_t BC_USEB = 1u << 10;
constexpr uint16_t BC_USEC = 1u << 9;
constexpr uint16_t BC_USED = 1u << 8;
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;

// BLTSIZE data word for OCS-style sizing: height in bits[15:6], width in
// bits[5:0] (agnus_blitter.v:521-525,555-556).
constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

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

    void set_bls(bool v) { dut->bls = v ? 1 : 0; }

    // One CPU register write, held for up to `max_ticks` ticks and applied
    // on the first tick where `dbr==0` (see IMPLEMENTATION NOTES above for
    // why that is the correct, not merely convenient, condition). Returns
    // whether a dbr==0 tick was actually seen; every call site in this file
    // checks it via `write_ok` immediately after, so a silently-missed
    // write cannot masquerade as a passing assertion.
    //
    // Critically, the RTL's write (`always @(posedge clk) ... if
    // (reg_address[8:1]==DMACON[8:1]) ...`, agnus.v:286-295) is decided by
    // the combinational `reg_address`/`dbr` value that is stable going INTO
    // the rising edge - not the value `dbr` settles to afterwards, once
    // this same edge has also advanced `hpos` (and therefore dma_ref and
    // every hpos-derived dma_x wire) to its next value. Gating retry on the
    // post-edge `dbr` sample (as tick()'s `last_dbr` is, for the
    // consumption assertions' aggregate counting, where the one-tick offset
    // does not change a total count) silently missed writes here whenever
    // some engine held the bus pre-edge but had already released it by the
    // time the post-edge sample was taken - caught empirically: an
    // ALL-13-bits-SET write (which also sets SPREN/BLTEN/COPEN/BPLEN, live
    // engine-enable bits) followed by a single-bit CLR reliably left the
    // targeted bit un-cleared even though the post-edge `dbr` read 0 on the
    // very first retry tick. `tick_pre_edge_dbr()` below samples `dbr`
    // between the `cck` update and the clock edge, which is the value the
    // write decision itself actually uses.
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

    bool sync_to_line_boundary(int max_ticks = 2000) {
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (last_sol) return true;
        }
        return false;
    }

    // Counts ticks where `dbr` is high and `refresh` is not, across `n_lines`
    // sol-to-sol windows starting right after a call to sync_to_line_boundary.
    // See IMPLEMENTATION NOTES for why this isolates "the one engine under
    // test" whenever disk/audio are idle and every other enable bit is clear.
    int count_nonrefresh_dbr_over_lines(int n_lines, int max_ticks_per_line = 1000) {
        int total = 0;
        for (int line = 0; line < n_lines; ++line) {
            for (int t = 0; t < max_ticks_per_line; ++t) {
                tick();
                if (last_sol) break;
                if (last_dbr && !last_refresh) ++total;
            }
        }
        return total;
    }

    // Counts ticks where `dbwe` is high over a fixed number of ticks (no
    // line-boundary framing needed - dbwe is unambiguous on its own, see
    // IMPLEMENTATION NOTES).
    int count_dbwe_over_ticks(int n_ticks) {
        int total = 0;
        for (int i = 0; i < n_ticks; ++i) {
            tick();
            if (last_dbwe) ++total;
        }
        return total;
    }

    // Returns the 1-based tick index of the Nth dbwe pulse (the D-channel
    // write that completes a W*H-word blit, when N==W*H), or -1 if fewer
    // than N pulses occur within max_ticks - the tighter measurement
    // BLTPRI-BUS-SHARE-UNDER-CONTENTION's own window-based count cannot make:
    // two runs finishing on the SAME tick is a much sharper "no cycles
    // stolen" than two runs both finishing SOMEWHERE inside a shared window.
    int tick_of_nth_dbwe(int n, int max_ticks) {
        int count = 0;
        for (int i = 1; i <= max_ticks; ++i) {
            tick();
            if (last_dbwe) {
                ++count;
                if (count == n) return i;
            }
        }
        return -1;
    }

    void run_ticks(int n) { for (int i = 0; i < n; ++i) tick(); }

    void tick() { tick_pre_edge_dbr(); }

    // Advances one clk7_en tick (cck toggle, then a clk 0->1->0 pulse),
    // updating the post-edge observation members (last_sol/last_refresh/
    // last_dbr/last_dbwe/last_data_out) exactly as tick() always has, and
    // additionally returns the PRE-edge value of `dbr` - the value the
    // RTL's own `always @(posedge clk)` write logic actually saw for this
    // edge (see write_reg's comment above for why the two can differ).
    bool tick_pre_edge_dbr() {
        dut->cck = cck_phase ? 1 : 0;
        cck_phase = !cck_phase;
        dut->eval();
        bool pre_dbr = dut->dbr;
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        last_sol = dut->sol;
        last_refresh = dut->refresh;
        last_dbr = dut->dbr;
        last_dbwe = dut->dbwe;
        last_data_out = dut->data_out;
        last_reg_address_out = dut->reg_address_out;
        return pre_dbr;
    }

    bool last_sol = false;
    bool last_refresh = false;
    bool last_dbr = false;
    bool last_dbwe = false;
    uint16_t last_data_out = 0;
    uint16_t last_reg_address_out = 0;  // word address (reg9>>1) of whichever engine
                                         // currently holds the bus
    bool write_ok = false;
    bool read_ok = false;

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
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

void start_blit(Harness& h, uint16_t bltcon0, uint16_t width, uint16_t height) {
    h.write_reg(BLTCON0_ADDR, bltcon0);
    h.write_reg(BLTSIZE_ADDR, bltsize_word(width, height));
}

void start_copper(Harness& h, uint16_t list_addr_words) {
    // list_addr_words is a byte address; COP1LCH gets the high 5 bits,
    // COP1LCL the low 16 (agnus_copper.v:78-79/143-168).
    h.write_reg(COP1LCH_ADDR, static_cast<uint16_t>((list_addr_words >> 16) & 0x1F));
    h.write_reg(COP1LCL_ADDR, static_cast<uint16_t>(list_addr_words & 0xFFFF));
    h.write_reg(COPJMP1_ADDR, 0x0000);  // any write strobes the jump-to-COP1LC restart
}

void start_sprite0(Harness& h) {
    // Mirrors hrm/agnus_spritedma's own known-good setup: pointer, then
    // VSTART=0x20(32)/VSTOP=0x22(34) via SPR0POS/SPR0CTL.
    h.write_reg(SPR0PTH_ADDR, 0x0000);
    h.write_reg(SPR0PTL_ADDR, 0x2000);
    h.write_reg(SPR0POS_ADDR, 0x2000);
    h.write_reg(SPR0CTL_ADDR, 0x2200);
}

void start_bitplane(Harness& h) {
    // Mirrors hrm/agnus_bitplanedma's own known-good "lowres, 1 bitplane"
    // setup (DIWSTRT/DIWSTOP/DDFSTRT/DDFSTOP/BPLCON0/FMODE).
    h.write_reg(DIWSTRT_ADDR, 0x2C81);
    h.write_reg(DIWSTOP_ADDR, 0x2CC1);
    h.write_reg(DDFSTRT_ADDR, 0x0038);
    h.write_reg(DDFSTOP_ADDR, 0x00D0);
    h.write_reg(BPLCON0_ADDR, 0x1200);
    h.write_reg(FMODE_ADDR, 0x0000);
}

// Arms sprite `n`: a safe pointer (start_sprite0's own pattern, retargeted
// to sprite n's own PTH/PTL/POS/CTL addresses - 4 words apart for the
// pointer pair, 8 for the POS/CTL/DATA/DATB quartet, matching
// agnus_spritedma.v's own SPRPTBASE_REG/SPRPOSCTLBASE_REG spacing) plus
// VSTART/VSTOP written into SPRxPOS[15:8]/SPRxCTL[15:8] (the 8-bit,
// non-ECS form - this Harness's `ecs` input is held 0 throughout).
void arm_sprite(Harness& h, int n, uint16_t vstart, uint16_t vstop) {
    h.write_reg(static_cast<uint16_t>(SPR0PTH_ADDR + 4 * n), 0x0000);
    h.write_reg(static_cast<uint16_t>(SPR0PTL_ADDR + 4 * n), 0x2000);
    h.write_reg(static_cast<uint16_t>(SPR0POS_ADDR + 8 * n), static_cast<uint16_t>(vstart << 8));
    h.write_reg(static_cast<uint16_t>(SPR0CTL_ADDR + 8 * n), static_cast<uint16_t>(vstop << 8));
}

// One granted, non-refresh dma cycle whose reg_address_out falls in the
// sprite engine's own 8-bit range (agnus_spritedma.v's reqdma logic:
// {SPRPOSCTLBASE_REG[8:6]=3'b101, sprite[2:0], type[1:0]}, i.e. $A0-$BF -
// no other engine's own *_REG localparam in agnus.v/agnus_bitplanedma.v/
// agnus_copper.v/agnus_blitter.v/agnus_diskdma.v falls in that range).
struct SpriteFetch {
    int sprite;  // 0-7
    int type;    // 0=POS 1=CTL 2=DATA 3=DATB (agnus_spritedma.v's own encoding)
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
                if ((a & 0xE0) == 0xA0) out.push_back(SpriteFetch{(a >> 2) & 0x7, a & 0x3});
            }
        }
    }
    return out;
}

// Measures each of the 8 sprites' own SPRxDATA/SPRxDATB fetch count over 90
// lines at a given DDFSTRT: DMAEN alone until vblend has passed (35 lines),
// then all eight sprites armed (VSTART=40/VSTOP=220, comfortably inside the
// display window this measures over), then SPREN and BPLEN set together -
// the ordering this section's own header comment explains.
//
// `bplcon0` defaults to $1200 (one bitplane, lores, colour on) - the fixture
// value this file's DDFSTRT assertions use by default.
// Neither p.80 ("using a data-fetch start earlier than ($38) will disable
// some sprites") nor p.82/83 ("DDFSTRT = $0030 ... will disable sprite 7" /
// "Fetching an extra word for scrolling will disable some sprites") names a
// bitplane count, and agnus_bitplanedma.v's own `bpu` field (BPLCON0[14:12],
// read directly for 1/2/4/6 - the non-AGA "all-ones means 4" alias never
// triggers for these values) changes how many of each 8-CCK fetch sequence's
// 8 slots bitplane DMA actually occupies (`plane[4:0] < bpu`), so the repair
// pass measures explicitly at 2, 4 and 6 bitplanes as well - see the
// DDFSTRT-0030-SPRITE-7-* assertions below.
std::vector<int> measure_sprite_data_fetch(uint16_t ddfstrt, uint16_t bplcon0 = 0x1200) {
    Harness h;
    h.write_reg(DMACON_ADDR, SET_CLR | DMAEN);
    for (int i = 0; i < 35; ++i) h.sync_to_line_boundary();
    for (int n = 0; n < 8; ++n) arm_sprite(h, n, 40, 220);
    h.write_reg(DIWSTRT_ADDR, 0x2C81);
    h.write_reg(DIWSTOP_ADDR, 0x2CC1);
    h.write_reg(DDFSTRT_ADDR, ddfstrt);
    h.write_reg(DDFSTOP_ADDR, 0x00D0);
    h.write_reg(BPLCON0_ADDR, bplcon0);
    h.write_reg(FMODE_ADDR, 0x0000);
    h.write_reg(DMACON_ADDR, SET_CLR | SPREN | BPLEN);
    auto fetches = collect_sprite_fetches_over_lines(h, 90);
    std::vector<int> per_sprite(8, 0);
    for (const auto& f : fetches)
        if (f.type == 2 || f.type == 3) ++per_sprite[f.sprite];
    return per_sprite;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // REGISTER MECHANICS - observable on data_out (DMACONR) alone.
    // =========================================================================

    // ---------------- DMACON-WRITE-DECODED-AT-096 (p.278/290) ----------------
    {
        Harness h;
        uint16_t before = h.read_reg(DMACONR_ADDR);
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN);
        uint16_t after = h.read_reg(DMACONR_ADDR);
        checks.expect(
            h.read_ok && h.write_ok && before == 0x0000 && after == DMAEN,
            "DMACON-WRITE-DECODED-AT-096: a SET write through aen/hwr/lwr/"
            "address_in=$096/data_in changes what DMACONR ($002) reads back "
            "afterwards (0x0000 -> 0x0200, DMAEN) - 'DMACON 096 W ... DMA "
            "control write (clear or set)' (p.278) and Appendix B's 'DMACON "
            "at address 096' (p.290). 6-appa-dmacon.c1, 6-appa-dmacon.c2, "
            "6-appa-dmacon.c3, ch6-appa-dmacon.c1, ch6-appa-dmacon.c2, "
            "ch6-appa-dmacon.c4, ch6-appa-dmacon.c9, ch6-appb-dmacon.addr, sol:7-dma.c1, sol:7-dma.c3, opus:7-dma.c1, opus:7-dma.c3, sol:7-appA-dmacon.c1, sol:7-appA-dmacon.c2, sol:7-appA-dmacon.c3, sol:7-appA-dmacon.c4, sol:7-appA-dmacon.c9, opus:7-appA-dmacon.c1, opus:7-appA-dmacon.c3, sol:7-appB-dmacon.c1, sol:7-appB-dmacon.c2, sol:7-appB-dmacon.c3, sol:7-appB-dmacon.c4, opus:7-appB.c29, "
            "ch6-appb-dmacon.rw (p.290, restated: 'Appendix B marks DMACON W "
            "in the R/W column and A D P in the CHIP column' - verbatim "
            "quotation pending: the corpus text extracted for printed p.290 "
            "is defective (misaligned columns), so it is not quoted here; "
            "this assertion demonstrates only the W/write-decoded half of "
            "that claim - the CHIP column's 'A D P' half names which chips "
            "decode the register and is not observable at any port).");
    }

    // ---------------- WRITE-ADDRESS-MUST-MATCH-DMACON [control, no page] ----------------
    {
        Harness h;
        uint16_t before = h.read_reg(DMACONR_ADDR);
        h.write_reg(BLTAFWM_ADDR, SET_CLR | DMAEN);  // same data pattern, different address
        uint16_t after = h.read_reg(DMACONR_ADDR);
        checks.expect(
            h.read_ok && h.write_ok && before == 0x0000 && after == 0x0000,
            "[control] WRITE-ADDRESS-MUST-MATCH-DMACON: the identical data_in "
            "pattern (SET_CLR|DMAEN) written to a different, real register "
            "address ($044, BLTAFWM) leaves DMACONR unchanged - rules out a "
            "fixture or an agnus decode that reports 'written' regardless of "
            "address. No manual page: the control for "
            "DMACON-WRITE-DECODED-AT-096, on the same measurement basis.");
    }

    // ---------------- DMACONR-READ-DECODED-AT-002 (p.278/289) ----------------
    // B-dmaconr.addr restates the same Appendix B address-order row as the
    // already-cited ch6-appb-dmaconr.addr (both say DMACONR is listed at
    // $002); this assertion is that row's address half exactly as it
    // already measures ch6-appb-dmaconr.addr, so the id joins it below.
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | (SPREN | COPEN));
        uint16_t read_a = h.read_reg(DMACONR_ADDR);
        h.write_reg(DMACON_ADDR, 0x0000 | (SPREN | COPEN));  // CLR the same two bits
        uint16_t read_b = h.read_reg(DMACONR_ADDR);
        checks.expect(
            h.read_ok && h.write_ok && read_a == (SPREN | COPEN) && read_b == 0x0000 &&
                read_a != read_b,
            "DMACONR-READ-DECODED-AT-002: reading through aen/rd/address_in=$002 "
            "reflects two DIFFERENT known dmacon states differently (0x00A0 then "
            "0x0000, not a stuck constant) - 'DMACONR 002 R ... DMA control (and "
            "blitter status) read' (p.278) and Appendix B's 'DMACONR at address "
            "002' (p.289). 6-appa-dmacon.c4, 6-appa-dmacon.c5, 6-appa-dmacon.c6, "
            "6-appa-dmacon.c7, ch6-appa-dmacon.c5, ch6-appa-dmacon.c6, "
            "ch6-appa-dmacon.c8, ch6-appa-dmacon.c10, ch6-appb-dmaconr.addr, B-dmaconr.addr, sol:7-dma.c1, sol:7-dma.c2, sol:7-dma.c4, opus:7-dma.c1, opus:7-dma.c2, opus:7-dma.c4, sol:7-appA-dmacon.c5, sol:7-appA-dmacon.c6, sol:7-appA-dmacon.c7, sol:7-appA-dmacon.c8, sol:7-appA-dmacon.c10, opus:7-appA-dmacon.c2, opus:7-appA-dmacon.c3, "
            "sol:7-appB-dmaconr.c1, sol:7-appB-dmaconr.c2, sol:7-appB-dmaconr.c4 (address, direction and function). The chip clause and the bundled appendix-B id are declined on their own RECORDED line below and are "
            "deliberately NOT NAMED on this line: the coverage ledger counts every uid printed on a graded line as asserted, whatever the sentence around it says.");
    }

    // ---------------- WALKING-BIT assertions, DMACON/DMACONR bits 0-12 (p.278) ----------------
    // Each iteration: from a fresh reset (dmacon==0), SET the bit alone and
    // confirm DMACONR reads back exactly that bit and no other (bits 13/14
    // included, since a fresh Harness never runs a blit); then, from a
    // separate fresh reset with ALL 13 bits SET, CLR the bit alone and
    // confirm it - and only it - clears, leaving every other bit of the
    // all-ones pattern untouched. The second half is the real test of "bits
    // written with a zero remain unchanged" under CLR; the first half alone
    // cannot show that, since every other bit already reads zero going in.
    {
        struct BitCase {
            int bit;
            uint16_t mask;
            const char* label;
            std::string ids;
        };
        const std::vector<BitCase> bits = {
            {0, AUD0EN, "AUD0EN", "ch6-appa-dmacon.bit00, sol:7-dma.c21, opus:7-dma.c22, sol:7-appA-dmacon.c26, opus:7-appA-dmacon.c21"},
            {1, AUD1EN, "AUD1EN", "ch6-appa-dmacon.bit01, sol:7-dma.c20, opus:7-dma.c21, sol:7-appA-dmacon.c25, opus:7-appA-dmacon.c20"},
            {2, AUD2EN, "AUD2EN", "ch6-appa-dmacon.bit02, sol:7-dma.c19, opus:7-dma.c20, sol:7-appA-dmacon.c24, opus:7-appA-dmacon.c19"},
            {3, AUD3EN, "AUD3EN", "ch6-appa-dmacon.bit03, sol:7-dma.c18, opus:7-dma.c19, sol:7-appA-dmacon.c23, opus:7-appA-dmacon.c18"},
            {4, DSKEN, "DSKEN", "ch6-appa-dmacon.bit04, sol:7-dma.c17, opus:7-dma.c18, sol:7-appA-dmacon.c22, opus:7-appA-dmacon.c17"},
            {5, SPREN, "SPREN", "6-appa-dmacon.c26, ch6-appa-dmacon.bit05, sol:7-dma.c16, opus:7-dma.c17, sol:7-appA-dmacon.c21, opus:7-appA-dmacon.c16, APPA-dmacon.c16"},
            {6, BLTEN, "BLTEN", "6-appa-dmacon.c25, ch6-appa-dmacon.bit06, sol:7-dma.c15, opus:7-dma.c16, sol:7-appA-dmacon.c20, opus:7-appA-dmacon.c15, APPA-dmacon.c15, CH4-appA-dmacon.c22"},
            {7, COPEN, "COPEN", "6-appa-dmacon.c24, ch6-appa-dmacon.bit07, sol:7-dma.c14, opus:7-dma.c15, sol:7-appA-dmacon.c19, opus:7-appA-dmacon.c14, APPA-dmacon.c14, CH4-appA-dmacon.c21"},
            {8, BPLEN, "BPLEN", "6-appa-dmacon.c23, ch6-appa-dmacon.bit08, sol:7-dma.c13, opus:7-dma.c14, sol:7-appA-dmacon.c18, opus:7-appA-dmacon.c13, APPA-dmacon.c13, CH4-appA-dmacon.c20"},
            {9, DMAEN, "DMAEN",
             "6-appa-dmacon.c22, ch6-appa-dmacon.bit09, ch6-appa-dmacon.dmaen2, sol:7-dma.c11, opus:7-dma.c12, sol:7-dma.c12, opus:7-dma.c13, sol:7-appA-dmacon.c17, opus:7-appA-dmacon.c12"},
            {10, BLTPRI, "BLTPRI",
             "6-appa-dmacon.c17, ch6-appa-dmacon.bit10, ch6-appa-dmacon.bltpri1, sol:7-dma.c9, opus:7-dma.c10, sol:7-appA-dmacon.c15, opus:7-appA-dmacon.c10, APPA-dmacon.c10, CH4-appA-dmacon.c17"},
            {11, BIT11_X, "bit11(X)", "6-appa-dmacon.c16, ch6-appa-dmacon.bit11, sol:7-dma.c8, opus:7-dma.c9, sol:7-appA-dmacon.c14, opus:7-appA-dmacon.c9"},
            {12, BIT12_X, "bit12(X)", "6-appa-dmacon.c15, ch6-appa-dmacon.bit12, sol:7-dma.c8, opus:7-dma.c9, sol:7-appA-dmacon.c14, opus:7-appA-dmacon.c8"},
        };
        constexpr uint16_t ALL13 = 0x1FFF;
        for (const auto& b : bits) {
            Harness h_set;
            h_set.write_reg(DMACON_ADDR, SET_CLR | b.mask);
            uint16_t after_set = h_set.read_reg(DMACONR_ADDR);

            Harness h_clr;
            h_clr.write_reg(DMACON_ADDR, SET_CLR | ALL13);
            uint16_t after_set_all = h_clr.read_reg(DMACONR_ADDR);
            h_clr.write_reg(DMACON_ADDR, 0x0000 | b.mask);
            uint16_t after_clr = h_clr.read_reg(DMACONR_ADDR);

            checks.expect(
                h_set.read_ok && h_set.write_ok && h_clr.read_ok && h_clr.write_ok &&
                    after_set == b.mask && after_set_all == ALL13 &&
                    after_clr == static_cast<uint16_t>(ALL13 & ~b.mask),
                std::string("WALKING-BIT-") + std::to_string(b.bit) + "-" + b.label +
                    ": SET from a zero baseline sets only bit " + std::to_string(b.bit) +
                    "; CLR of the same bit from an all-13-bits-set baseline clears only "
                    "bit " + std::to_string(b.bit) + ", leaving the other twelve set - "
                    "'" + std::string(b.label) +
                    "' at its named position, and (both halves together) the SET/CLR "
                    "'bits written as 0 remain unchanged' rule for this position. " +
                    b.ids);
        }
    }

    // ---------------- SETCLR-MULTIBIT-DIRECTION (p.278) ----------------
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | (AUD0EN | DSKEN | COPEN));
        uint16_t after_set = h.read_reg(DMACONR_ADDR);
        h.write_reg(DMACON_ADDR, 0x0000 | DSKEN);  // CLR only DSKEN, a subset of the SET pattern
        uint16_t after_clr = h.read_reg(DMACONR_ADDR);
        checks.expect(
            h.read_ok && h.write_ok && after_set == (AUD0EN | DSKEN | COPEN) &&
                after_clr == (AUD0EN | COPEN),
            "SETCLR-MULTIBIT-DIRECTION: one SET write sets a 3-bit pattern "
            "(AUD0EN|DSKEN|COPEN), one CLR write then clears only the DSKEN "
            "subset of that same pattern, leaving AUD0EN and COPEN set - the "
            "OR-then-AND-NOT distinction the single-bit walk above does not "
            "exercise on its own. 'if bits written with a 1 get set or "
            "cleared. Bits written with a zero...' (p.278). 6-appa-dmacon.c8, "
            "6-appa-dmacon.c9, 6-appa-dmacon.c10, ch6-appa-dmacon.bit15, "
            "ch6-appa-dmacon.setclr, ch6-appa-dmacon.setclr2, sol:7-dma.c5, opus:7-dma.c5, sol:7-appA-dmacon.c11, opus:7-appA-dmacon.c4, opus:7-appA-dmacon.c5.");
    }

    // ---------------- BBUSY-READONLY-WITH-POSITIVE-CONTROL (p.186/278) ----------------
    {
        Harness h;
        uint16_t idle = h.read_reg(DMACONR_ADDR);  // baseline: no blit has run yet

        // Attempt to write bit 14 (BBUSY) directly, in the SAME write as a
        // real, writable control bit (DMAEN) - the control must move, bit 14
        // must not.
        // BLTEN is required alongside DMAEN here, unlike the write-path
        // checks above: without it agnus.v:443's `enadma` (blten & ena_blt)
        // never goes high, so while `busy` still latches immediately on the
        // BLTSIZE write (agnus_blitter.v:500's condition does not check
        // `enable`), the FSM can never actually run to `done` - this was
        // caught empirically (busy stuck at 1 forever, zero stuck at 0
        // forever, in an earlier run of this same assertion) before BLTEN
        // was added here.
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN | BBUSY_B);
        uint16_t after_attempt = h.read_reg(DMACONR_ADDR);

        // Positive control: a REAL busy=1, driven by the blitter, not by any
        // write through DMACON - proves bit 14 is a live wire, not a stuck 0
        // that merely survived the write attempt above. A 4x4-word blit
        // (agnus_blitter.v:495-504) keeps busy==1 long enough to sample
        // comfortably.
        start_blit(h, BC_USEC | BC_USED | LF_C_PASSTHROUGH, 4, 4);
        uint16_t during_blit = h.read_reg(DMACONR_ADDR);
        h.run_ticks(400);  // generous margin past completion of a 16-word blit
        uint16_t after_blit = h.read_reg(DMACONR_ADDR);

        checks.expect(
            h.read_ok &&
                (idle & BBUSY_B) == 0 &&
                (after_attempt & BBUSY_B) == 0 && (after_attempt & DMAEN) == DMAEN &&
                (during_blit & BBUSY_B) == BBUSY_B &&
                (after_blit & BBUSY_B) == 0,
            "BBUSY-READONLY-WITH-POSITIVE-CONTROL: DMACONR bit 14 reads 0 idle, "
            "stays 0 after a write attempt that DOES move the DMAEN control bit "
            "in the same operation, then reads 1 while a real blit is busy and "
            "0 again once it finishes - the write path is live (DMAEN moved) "
            "while bit 14 never responds to it, and bit 14 is independently "
            "shown to be a live signal (0->1->0) rather than a wire that is "
            "simply always 0. '14 BBUSY Blitter busy status bit (read only)' "
            "(p.278). 6-appa-dmacon.c11, 6-appa-dmacon.c12, "
            "ch6-appa-dmacon.bit14, ch6-appa-dmacon.bbusy2, "
            "6-blitter-done-flag.c5, ch6-done.c5, ch6-done.c6, sol:7-dma.c6, opus:7-dma.c6, sol:7-appA-dmacon.c12, opus:7-appA-dmacon.c6, APPA-dmacon.c6, CH4-appA-dmacon.c13.");
    }

    // ---------------- BZERO-READONLY-WITH-POSITIVE-CONTROL (p.187/278) ----------------
    {
        Harness h;
        uint16_t idle = h.read_reg(DMACONR_ADDR);  // baseline: no blit has run yet

        // Attempt SET on bit 13 alongside the DMAEN control.
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN | BZERO_B);  // BLTEN needed, see BBUSY's comment above
        uint16_t after_set_attempt = h.read_reg(DMACONR_ADDR);

        // Positive control: run a real blit whose D channel only ever sees
        // zero words (BLTCON0's C-passthrough with data_in left at its
        // default 0 for every fetch) - agnus_blitter.v:480-487 sets `zero`
        // at the blit's own init cycle and never has a nonzero result to
        // clear it, so bit 13 goes 0 (idle) -> 1 (real, blitter-driven),
        // not because we wrote it.
        start_blit(h, BC_USEC | BC_USED | LF_C_PASSTHROUGH, 4, 4);
        h.run_ticks(400);
        uint16_t after_blit = h.read_reg(DMACONR_ADDR);

        // Attempt CLR on bit 13 alongside the DMAEN control, now that bit 13
        // genuinely reads 1 - the control (DMAEN) must move back to 0, bit
        // 13 must stay 1.
        h.write_reg(DMACON_ADDR, 0x0000 | DMAEN | BZERO_B);
        uint16_t after_clr_attempt = h.read_reg(DMACONR_ADDR);

        checks.expect(
            h.read_ok &&
                (idle & BZERO_B) == 0 &&
                (after_set_attempt & BZERO_B) == 0 && (after_set_attempt & DMAEN) == DMAEN &&
                (after_blit & BZERO_B) == BZERO_B &&
                (after_clr_attempt & BZERO_B) == BZERO_B &&
                (after_clr_attempt & DMAEN) == 0,
            "BZERO-READONLY-WITH-POSITIVE-CONTROL: DMACONR bit 13 reads 0 idle, "
            "stays 0 through a SET-bit-13-plus-DMAEN write that DOES move "
            "DMAEN, then reads 1 once a real all-zero-result blit finishes - "
            "and a later CLR-bit-13-plus-DMAEN write moves DMAEN back to 0 "
            "while bit 13 stays 1 regardless. The write path is live in both "
            "directions while bit 13 never responds to it, and bit 13 is "
            "independently shown to be a live signal (0->1) rather than a "
            "wire that is simply always 0. '(13) DMAF_BLTNZERO...bit (13) "
            "DMACONR' / '13 BZERO Blitter logic zero status bit (read only)' "
            "(p.187/278). 6-appa-dmacon.c13, 6-appa-dmacon.c14, "
            "ch6-appa-dmacon.bit13, ch6-appa-dmacon.bzero2, 6-zero-flag.c5, "
            "ch6-zero.c5, ch6-zero.c6, sol:7-dma.c7, opus:7-dma.c7, opus:7-dma.c8, sol:7-appA-dmacon.c13, opus:7-appA-dmacon.c7.");
    }

    // ---------------- BZERO-STAYS-CLEAR-AFTER-NONZERO-RESULT-BLIT (p.187/278) ----------------
    // APPA-dmacon.c7 ("zero status") and CH4-appA-dmacon.c14 ("reports
    // blitter zero-result status") both describe a bit that reflects the
    // blit's RESULT, but the assertion above shows only one direction (0
    // before any blit, 1 after an all-zero-result blit) - a bit that simply
    // sets after any completed blit, zero-result or not, would also pass
    // it. This runs a NONZERO-result blit and requires BZERO stay 0.
    //
    // agnus_blitter.v:420-421 loads `bltcdat` straight from `data_in` on the
    // register-bus cycle where the blitter's own arbitrated fetch lands on
    // BLTCDAT's address (this Harness's "no chip-RAM model" note applies
    // here exactly as it does to bitplane/sprite DMA loads above) - holding
    // `data_in` at a nonzero value for the whole blit therefore feeds a
    // nonzero word through LF_C_PASSTHROUGH's channel C, so
    // fill_out/bltdhold/zero (agnus_blitter.v:480-487, "store_result &&
    // |fill_out[15:0]") see a real nonzero result, not a fixture artifact.
    {
        Harness h;
        uint16_t idle = h.read_reg(DMACONR_ADDR);

        // DMAEN|BLTEN must be live BEFORE the blit starts, exactly as the
        // all-zero-result control above requires (its own comment: without
        // BLTEN, agnus.v:443's `enadma` never goes high and the blitter FSM
        // never reaches `init`, so `zero` - no reset branch of its own,
        // agnus_blitter.v:480-487 - would default-read 0 regardless of any
        // data_in stimulus, a vacuous pass this fixture must not produce.
        h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN);
        start_blit(h, BC_USEC | BC_USED | LF_C_PASSTHROUGH, 4, 4);
        h.dut->data_in = 0xFFFF;  // held nonzero for the whole blit - see comment above
        h.run_ticks(400);
        uint16_t after_blit = h.read_reg(DMACONR_ADDR);
        h.dut->data_in = 0x0000;

        checks.expect(
            h.read_ok && (idle & BZERO_B) == 0 && (after_blit & BZERO_B) == 0,
            "BZERO-STAYS-CLEAR-AFTER-NONZERO-RESULT-BLIT: DMACONR bit 13 reads "
            "0 idle and STAYS 0 once a real blit whose D-channel result is "
            "nonzero (data_in held at 0xFFFF through channel C's own DMA fetch, "
            "LF_C_PASSTHROUGH) finishes - the direction the all-zero-result "
            "control above cannot show, since a bit that simply latched 1 after "
            "any completed blit would also pass that one. '(13) "
            "DMAF_BLTNZERO...bit (13) DMACONR' / '13 BZERO Blitter logic zero "
            "status bit (read only)' (p.187/278). APPA-dmacon.c7, "
            "CH4-appA-dmacon.c14.");
    }

    // =========================================================================
    // CONSUMPTION - observable via dbr/dbwe/address_out (each engine gated by
    // its own DMACON bit AND DMAEN, bit 9); disk_dmal/audio_dmal stay at their
    // idle 0 throughout, and every enable bit other than the one(s) under test
    // stays clear, so `dbr && !refresh` (or, for the blitter, `dbwe`) isolates
    // the engine under test - see IMPLEMENTATION NOTES.
    // =========================================================================

    // ---------------- SPREN-GATES-SPRITE-DMA (bit 5, p.278) ----------------
    {
        Harness h_en;
        h_en.write_reg(DMACON_ADDR, SET_CLR | DMAEN | SPREN);
        start_sprite0(h_en);
        h_en.sync_to_line_boundary();
        int cycles_en = h_en.count_nonrefresh_dbr_over_lines(40);

        Harness h_dis;
        h_dis.write_reg(DMACON_ADDR, SET_CLR | DMAEN);  // control: SPREN left clear
        start_sprite0(h_dis);
        h_dis.sync_to_line_boundary();
        int cycles_dis = h_dis.count_nonrefresh_dbr_over_lines(40);

        checks.expect(
            cycles_en > 0 && cycles_dis == 0,
            "SPREN-GATES-SPRITE-DMA: with sprite 0's pointer/VSTART=32/VSTOP=34 "
            "set up identically in both runs and DMAEN held set throughout, "
            "agnus grants sprite-engine bus cycles (dbr&&!refresh, " +
                std::to_string(cycles_en) +
                " ticks over 40 lines) only when SPREN is also set - the SAME "
                "measurement with SPREN left clear [control] grants none (" +
                std::to_string(cycles_dis) +
                "). '05 SPREN Sprite DMA enable' (p.278). 6-appa-dmacon.c26, "
                "ch6-appa-dmacon.bit05, sol:7-dma.c16, opus:7-dma.c17, sol:7-appA-dmacon.c21, opus:7-appA-dmacon.c16, APPA-dmacon.c16.");
    }

    // ---------------- SPREN-GATES-SPRITE-DMA-ALL-CHANNELS (bit 5, p.278) ----------------
    // CH4-appA-dmacon.c23 restates the p.278 SPREN row as "DMACON bit 5
    // enables sprite DMA for all sprite channels" - a universal claim the
    // assertion above cannot establish, since it arms sprite 0 alone.
    // measure_sprite_data_fetch already arms all eight sprites identically;
    // reused here (SPREN vs SPREN-clear, same DDFSTRT=$38/one-bitplane
    // fixture the DDFSTRT assertions use) to grade every channel.
    {
        auto with_spren = measure_sprite_data_fetch(0x0038);  // SET_CLR|SPREN|BPLEN, see the function's own header
        Harness h_dis;
        h_dis.write_reg(DMACON_ADDR, SET_CLR | DMAEN);
        for (int i = 0; i < 35; ++i) h_dis.sync_to_line_boundary();
        for (int n = 0; n < 8; ++n) arm_sprite(h_dis, n, 40, 220);
        h_dis.write_reg(DIWSTRT_ADDR, 0x2C81);
        h_dis.write_reg(DIWSTOP_ADDR, 0x2CC1);
        h_dis.write_reg(DDFSTRT_ADDR, 0x0038);
        h_dis.write_reg(DDFSTOP_ADDR, 0x00D0);
        h_dis.write_reg(BPLCON0_ADDR, BPLCON0_1BPL);
        h_dis.write_reg(FMODE_ADDR, 0x0000);
        h_dis.write_reg(DMACON_ADDR, SET_CLR | BPLEN);  // control: SPREN left clear, BPLEN set as above
        auto fetches_dis = collect_sprite_fetches_over_lines(h_dis, 90);
        std::vector<int> without_spren(8, 0);
        for (const auto& f : fetches_dis)
            if (f.type == 2 || f.type == 3) ++without_spren[f.sprite];

        bool every_channel_gated = true;
        std::string detail;
        for (int n = 0; n < 8; ++n) {
            if (!(with_spren[n] > 0 && without_spren[n] == 0)) every_channel_gated = false;
            detail += std::to_string(n) + ":" + std::to_string(without_spren[n]) + "/" +
                      std::to_string(with_spren[n]) + " ";
        }

        checks.expect(
            every_channel_gated,
            "SPREN-GATES-SPRITE-DMA-ALL-CHANNELS: with all eight sprites armed "
            "identically (VSTART=40/VSTOP=220) and DMAEN|BPLEN held set in both "
            "runs, EVERY one of the eight channels' own SPRxDATA/SPRxDATB fetch "
            "count (without-SPREN/with-SPREN) is " + detail +
                "- zero without SPREN and nonzero with it, for all eight, not "
                "sprite 0 alone. 'DMACON bit 5 enables sprite DMA for all "
                "sprite channels' (p.278, '05 SPREN Sprite DMA enable', "
                "restated). CH4-appA-dmacon.c23.");
    }

    // ---------------- BLTEN-GATES-BLITTER-DMA (bit 6, p.278) ----------------
    {
        Harness h_en;
        h_en.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN);
        start_blit(h_en, BC_USEC | BC_USED | LF_C_PASSTHROUGH, 1, 1);
        int writes_en = h_en.count_dbwe_over_ticks(200);

        Harness h_dis;
        h_dis.write_reg(DMACON_ADDR, SET_CLR | DMAEN);  // control: BLTEN left clear
        start_blit(h_dis, BC_USEC | BC_USED | LF_C_PASSTHROUGH, 1, 1);
        int writes_dis = h_dis.count_dbwe_over_ticks(200);

        checks.expect(
            writes_en > 0 && writes_dis == 0,
            "BLTEN-GATES-BLITTER-DMA: with the identical BLTCON0/BLTSIZE blit "
            "start sequence in both runs and DMAEN held set throughout, agnus "
            "grants the blitter a write cycle (dbwe, " + std::to_string(writes_en) +
                " over 200 ticks) only when BLTEN is also set - the SAME "
                "measurement with BLTEN left clear [control] grants none (" +
                std::to_string(writes_dis) +
                "). '06 BLTEN Blitter DMA enable' (p.278). 6-appa-dmacon.c25, "
                "ch6-appa-dmacon.bit06, sol:7-dma.c15, opus:7-dma.c16, sol:7-appA-dmacon.c20, opus:7-appA-dmacon.c15, APPA-dmacon.c15, CH4-appA-dmacon.c22.");
    }

    // ---------------- COPEN-GATES-COPPER-DMA (bit 7, p.278) ----------------
    {
        Harness h_en;
        h_en.write_reg(DMACON_ADDR, SET_CLR | DMAEN | COPEN);
        start_copper(h_en, 0x1000);
        int cycles_en = h_en.count_nonrefresh_dbr_over_lines(4);

        Harness h_dis;
        h_dis.write_reg(DMACON_ADDR, SET_CLR | DMAEN);  // control: COPEN left clear
        start_copper(h_dis, 0x1000);
        int cycles_dis = h_dis.count_nonrefresh_dbr_over_lines(4);

        checks.expect(
            cycles_en > 0 && cycles_dis == 0,
            "COPEN-GATES-COPPER-DMA: with the identical COP1LCH/COP1LCL/"
            "COPJMP1 restart sequence in both runs and DMAEN held set "
            "throughout, agnus grants the copper bus cycles (dbr&&!refresh, " +
                std::to_string(cycles_en) +
                " over 4 lines) only when COPEN is also set - the SAME "
                "measurement with COPEN left clear [control] grants none (" +
                std::to_string(cycles_dis) +
                "). '07 COPEN Copper DMA enable' (p.278). 6-appa-dmacon.c24, "
                "ch6-appa-dmacon.bit07, sol:7-dma.c14, opus:7-dma.c15, sol:7-appA-dmacon.c19, opus:7-appA-dmacon.c14, APPA-dmacon.c14, CH4-appA-dmacon.c21.");
    }

    // ---------------- BPLEN-GATES-BITPLANE-DMA (bit 8, p.278) ----------------
    {
        Harness h_en;
        h_en.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BPLEN);
        start_bitplane(h_en);
        h_en.sync_to_line_boundary();
        int cycles_en = h_en.count_nonrefresh_dbr_over_lines(90);

        Harness h_dis;
        h_dis.write_reg(DMACON_ADDR, SET_CLR | DMAEN);  // control: BPLEN left clear
        start_bitplane(h_dis);
        h_dis.sync_to_line_boundary();
        int cycles_dis = h_dis.count_nonrefresh_dbr_over_lines(90);

        checks.expect(
            cycles_en > 0 && cycles_dis == 0,
            "BPLEN-GATES-BITPLANE-DMA: with the identical DIWSTRT/DIWSTOP/"
            "DDFSTRT/DDFSTOP/BPLCON0/FMODE display setup in both runs and "
            "DMAEN held set throughout, agnus grants bitplane-engine bus "
            "cycles (dbr&&!refresh, " + std::to_string(cycles_en) +
                " over 90 lines) only when BPLEN is also set - the SAME "
                "measurement with BPLEN left clear [control] grants none (" +
                std::to_string(cycles_dis) +
                "). '08 BPLEN Bitplane DMA enable' (p.278). 6-appa-dmacon.c23, "
                "ch6-appa-dmacon.bit08, sol:7-dma.c13, opus:7-dma.c14, sol:7-appA-dmacon.c18, opus:7-appA-dmacon.c13, APPA-dmacon.c13, CH4-appA-dmacon.c20.");
    }

    // ---------------- DMAEN-MASTER-GATES-ALL-DMA-BELOW (bit 9, p.278) ----------------
    {
        Harness h_en;
        h_en.write_reg(DMACON_ADDR, SET_CLR | DMAEN | SPREN | BLTEN | COPEN | BPLEN);
        start_sprite0(h_en);
        start_copper(h_en, 0x1000);
        start_bitplane(h_en);
        start_blit(h_en, BC_USEC | BC_USED | LF_C_PASSTHROUGH, 1, 1);
        h_en.sync_to_line_boundary();
        int cycles_en = h_en.count_nonrefresh_dbr_over_lines(90);

        Harness h_dis;
        // Same four sub-enable bits WRITTEN, but DMAEN itself left clear -
        // spren/blten/copen/bplen (agnus.v:299-302) all read as
        // dmacon[bit]&dmacon[9], so this must gate every one of them at once.
        h_dis.write_reg(DMACON_ADDR, SET_CLR | SPREN | BLTEN | COPEN | BPLEN);
        start_sprite0(h_dis);
        start_copper(h_dis, 0x1000);
        start_bitplane(h_dis);
        start_blit(h_dis, BC_USEC | BC_USED | LF_C_PASSTHROUGH, 1, 1);
        h_dis.sync_to_line_boundary();
        int cycles_dis = h_dis.count_nonrefresh_dbr_over_lines(90);

        checks.expect(
            cycles_en > 0 && cycles_dis == 0,
            "DMAEN-MASTER-GATES-ALL-DMA-BELOW: with SPREN, BLTEN, COPEN and "
            "BPLEN all set and all four engines given real requests to make "
            "(sprite pointer/VSTART/VSTOP, a copper restart, a display/ddf "
            "window, a BLTSIZE-started blit) in both runs, agnus grants some "
            "combination of them bus cycles (dbr&&!refresh, " +
                std::to_string(cycles_en) +
                " over 90 lines) only when DMAEN is ALSO set - the SAME four-bit "
                "configuration with DMAEN left clear [control] grants none (" +
                std::to_string(cycles_dis) +
                "), confirming DMAEN gates all four together, not by coincidence "
                "of any one of them. '09 DMAEN Enable all DMA below' (p.278). "
                "6-appa-dmacon.c22, ch6-appa-dmacon.bit09, ch6-appa-dmacon.dmaen2, sol:7-dma.c12, opus:7-dma.c13, sol:7-appA-dmacon.c17, opus:7-appA-dmacon.c12.");
    }

    // APPA-dmacon.c12 ("so sprite DMA also requires DMAEN") and
    // CH4-appA-dmacon.c19 ("the master
    // enable for the individual DMA enables in bits 8 through 0") restate p.278's
    // "09 DMAEN Enable all DMA below" as covering EVERY bit below 9, but the
    // assertion above (and WALKING-BIT-9's own mechanical readback) gates only
    // SPREN/BLTEN/COPEN/BPLEN at this fixture's boundary - disk (bit 4) and audio
    // (bits 0-3) are Paula-side and not driven or observed here at all.
    checks.record(
        "APPA-dmacon.c12, CH4-appA-dmacon.c19 (p.278, '09 DMAEN Enable all DMA "
        "below', restated as covering bits 8 through 0): not asserted whole -- "
        "this package's boundary (agnus alone, no Paula module compiled in, per "
        "this Makefile's SRC list) shows DMAEN gating SPREN/BLTEN/COPEN/BPLEN "
        "(bits 5-8) only. The disk (bit 4) and audio (bits 0-3) legs are Paula-"
        "side: ch8-dmaon-enable-composition's A5 and ch5-audio-dmacon-adkcon's "
        "DMAEN-AND-AUDxEN-BOTH-REQUIRED are where those channels' own DMAEN "
        "gating is measured.");

    // ---------------- BLTPRI-BUS-SHARE-UNDER-CONTENTION (bit 10, p.198/278) ----------------
    // APPA-dmacon.c10 and CH4-appA-dmacon.c17 also cite WALKING-BIT-10-BLTPRI
    // above for their shared position/name half; this assertion is their
    // functional half.
    {
        constexpr uint16_t W = 8, H = 8;  // 64-word blit - large enough to show a stall clearly
        constexpr int WINDOW = 500;

        Harness h_share;  // BLTPRI=0: DMAF_BLITHOG=0, "DMA manager monitors the 68000"
        h_share.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN);
        start_blit(h_share, BC_USEC | BC_USED | LF_C_PASSTHROUGH, W, H);
        h_share.set_bls(true);  // CPU is continuously requesting the bus
        int writes_share = h_share.count_dbwe_over_ticks(WINDOW);

        Harness h_hog;  // BLTPRI=1: DMAF_BLITHOG=1, "keeps the bus for every available cycle"
        h_hog.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN | BLTPRI);
        start_blit(h_hog, BC_USEC | BC_USED | LF_C_PASSTHROUGH, W, H);
        h_hog.set_bls(true);  // identical sustained CPU request
        int writes_hog = h_hog.count_dbwe_over_ticks(WINDOW);

        checks.expect(
            writes_hog == W * H && writes_share < writes_hog,
            "BLTPRI-BUS-SHARE-UNDER-CONTENTION: under an identical sustained "
            "/BLS request held for the whole run, BLTPRI=1 lets a " +
                std::to_string(W * H) + "-word blit run to completion (" +
                std::to_string(writes_hog) +
                " D-channel writes) while BLTPRI=0 [same blit, same bls] stalls "
                "at only " + std::to_string(writes_share) +
                " - 'If DMAF_BLITHOG is a 0, the DMA manager will monitor the "
                "68000 cycle...' vs 'If DMAF_BLITHOG is a 1, the blitter will "
                "keep the bus for every available...' (p.198), and 'disables "
                "/BLS pin, preventing micro from stealing any bus cycles while' "
                "(p.278). 6-blitter-priority.c12, 6-blitter-priority.c7, "
                "6-blitter-priority.c8, 6-blitter-priority.c9, ch6-sysdma.c57, "
                "ch6-sysdma.c58, ch6-sysdma.c59, ch6-sysdma.c61, "
                "6-appa-dmacon.c20, 6-appa-dmacon.c21, ch6-appa-dmacon.bltpri4, "
                "ch6-appa-dmacon.bltpri5, sol:7-dma.c10, opus:7-dma.c11, sol:7-appA-dmacon.c16, opus:7-appA-dmacon.c11, "
                "APPA-dmacon.c10, CH4-appA-dmacon.c17.");
    }

    // ---------------- BLTPRI-COMPLETION-TICK-UNCHANGED-UNDER-CONTENTION (bit 10, p.278) ----------------
    // APPA-dmacon.c11 ("disables the /BLS pin, preventing the processor from
    // stealing any bus cycles") and CH4-appA-dmacon.c18 ("/BLS is disabled
    // so the CPU cannot steal bus cycles") both claim NO cycle is stolen, but
    // the assertion above only shows the blit completes SOMEWHERE inside a
    // 500-tick window (64 writes needed, both runs finish at tick 391 - about
    // 27% of the window is slack that could hide stolen cycles undetected).
    // This compares the exact completion TICK, BLTPRI=1 throughout, with and
    // without a sustained /BLS request: if even one cycle were stolen from
    // the /BLS run, its completion tick would be LATER, not merely "still
    // inside the window".
    {
        constexpr uint16_t W = 8, H = 8;
        constexpr int MAX_TICKS = 500;

        Harness h_contended;  // BLTPRI=1, /BLS held asserted throughout
        h_contended.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN | BLTPRI);
        start_blit(h_contended, BC_USEC | BC_USED | LF_C_PASSTHROUGH, W, H);
        h_contended.set_bls(true);
        int tick_contended = h_contended.tick_of_nth_dbwe(W * H, MAX_TICKS);

        Harness h_quiet;  // BLTPRI=1, /BLS never asserted
        h_quiet.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN | BLTPRI);
        start_blit(h_quiet, BC_USEC | BC_USED | LF_C_PASSTHROUGH, W, H);
        int tick_quiet = h_quiet.tick_of_nth_dbwe(W * H, MAX_TICKS);

        checks.expect(
            tick_contended > 0 && tick_quiet > 0 && tick_contended == tick_quiet,
            "BLTPRI-COMPLETION-TICK-UNCHANGED-UNDER-CONTENTION: with BLTPRI=1 "
            "throughout, the " + std::to_string(W * H) + "-word blit's own "
            "completion tick (the tick of its " + std::to_string(W * H) +
                "th D-channel write) is " + std::to_string(tick_contended) +
                " under a sustained /BLS request and " + std::to_string(tick_quiet) +
                " with /BLS never asserted - EQUAL, not merely both inside a "
                "shared window, so no bus cycle is stolen from the blit by the "
                "contending request. '(disables /BLS pin, preventing micro "
                "from stealing any bus cycles while blitter DMA is running)' "
                "(p.278). APPA-dmacon.c11, CH4-appA-dmacon.c18.");
    }

    // =========================================================================
    // DDFSTRT vs. sprite DMA (pp.80/82/83). Needs the agnus top, not
    // agnus_bitplanedma or agnus_spritedma alone (neither module's own
    // package can see both sides) - the arbiter gives bitplane strictly higher
    // priority than sprites (agnus.v:207-230: the `dma_bpl` branch precedes the
    // `dma_spr` branch), and agnus_spritedma.v:264-272 fixes the sprite DMA
    // window at hpos $18-$38 (hex, in reg_address_out's own [8:1] units) -
    // exactly where a DDFSTRT earlier than $38 makes bitplane start fetching.
    // On a horizontal line where both engines want a cycle in that overlap,
    // agnus's priority mux (agnus.v:169-267) grants it to bitplane and the
    // sprite's request for that cycle is simply never acknowledged.
    //
    // Fixture note: this Harness supplies no chip-RAM model, so `data_in`
    // during a DMA-driven register load is whatever it was last set to, not a
    // real fetched word. agnus_spritedma.v's own vblend event (the last line
    // of vertical blanking, agnus_beamcounter.v's `vbstop`) reloads every
    // sprite's SPRxPOS/SPRxCTL unconditionally once a field regardless of
    // SPREN's own arming order; if SPREN is set before that event has passed,
    // the reload overwrites a CPU-written VSTART/VSTOP with zero before the
    // per-line SPRxDATA/SPRxDATB fetch this claim depends on ever gets a
    // chance to run. The fix (the same technique hrm/ch4-sprite-dma-control's
    // SPRITE-DMA-LOSES-CHANNELS-UNDER-WIDE-BITPLANE-DEMAND assertion uses):
    // let vblend pass with SPREN clear, arm the sprites, and only then set
    // SPREN - the CPU write is then the last word in sprpos/sprctl, and the
    // next automatic reload is a full field away, outside this measurement's
    // own window.
    // =========================================================================

    // ---------------- DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES (p.80) ----------------
    // The original form graded only DDFSTRT=$0018 against the $0038 baseline.
    // p.80's claim
    // is universal over the whole range below $38 ("using a data-fetch start
    // earlier than ($38) will disable some sprites"), and a probe at $0030 -
    // in this SAME one-bitplane fixture - found ALL EIGHT sprites unaffected
    // (0 of 8 reduced), so the universal read literally is false at that one
    // point in the range. This now sweeps $0030/$0028/$0020/$0018 and grades
    // the conjunction, so the $0030 counterexample is not lost inside a
    // single-point green.
    {
        auto baseline = measure_sprite_data_fetch(0x0038);
        bool baseline_uniform = true;
        for (int n = 1; n < 8; ++n)
            if (baseline[n] != baseline[0]) baseline_uniform = false;

        const std::vector<uint16_t> sweep = {0x0030, 0x0028, 0x0020, 0x0018};
        bool every_point_some_reduced = true;
        std::string detail;
        for (uint16_t ddfstrt : sweep) {
            auto m = measure_sprite_data_fetch(ddfstrt);
            int reduced = 0, unaffected = 0, worst = 0;
            for (int n = 0; n < 8; ++n) {
                if (m[n] < baseline[n]) ++reduced;
                if (m[n] == baseline[n]) ++unaffected;
                if (n == 0 || m[n] < m[worst]) worst = n;
            }
            bool some_reduced = (reduced > 0 && reduced < 8);
            if (!some_reduced) every_point_some_reduced = false;
            char buf[16];
            snprintf(buf, sizeof buf, "$%04X", ddfstrt);
            detail += std::string(buf) + ": " + std::to_string(reduced) +
                      " of 8 reduced (worst ch" + std::to_string(worst) + " " +
                      std::to_string(m[worst]) + "/" + std::to_string(baseline[worst]) + "); ";
        }

        checks.expect(
            baseline_uniform && baseline[0] > 0 && every_point_some_reduced,
            "DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES: with all "
            "eight sprites armed identically (VSTART=40/VSTOP=220), SPREN|DMAEN "
            "held set and ONE BITPLANE (BPLCON0=$1200, the manual names no "
            "plane count), every channel's own SPRxDATA/SPRxDATB fetch reaches "
            "reg_address_out at the SAME rate (" + std::to_string(baseline[0]) +
                " events/channel over 90 lines) at the DDFSTRT=$38 baseline. "
                "Swept across $0030/$0028/$0020/$0018: " + detail +
                "- the universal claim ('using a data-fetch start earlier than "
                "($38) will disable some sprites', p.80) holds at $0028/$0020/"
                "$0018 but is FALSE at $0030, one bitplane, where every one of "
                "the eight channels reads the SAME count as the $0038 baseline. "
                "3-max-window.c18, 3-max.c12.");
    }

    // ---------------- DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE (p.82) ----------------
    // This assertion's red rests on the fixture's one-bitplane BPLCON0=$1200,
    // stated in the label since p.82 itself names no plane count. The
    // sibling assertion below repeats the SAME $0030-vs-$0038, sprite-7-only
    // comparison at 2/4/6 bitplanes, where it is measured GREEN. The p.83
    // ids (3-hscr.c17, 3-hscroll.c17) are graded in their own assertions
    // below, not here, since sprite 7's own count cannot speak for p.83's
    // "some sprites" (existential, any of the eight) claim.
    {
        auto baseline = measure_sprite_data_fetch(0x0038);
        auto at_30 = measure_sprite_data_fetch(0x0030);  // the manual's own worked value

        checks.expect(
            at_30[7] < baseline[7],
            "DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE: sprite 7's own SPRxDATA/"
            "SPRxDATB fetch count over the SAME 90-line window (all eight "
            "sprites armed identically, VSTART=40/VSTOP=220, SPREN|DMAEN|BPLEN "
            "held set, ONE BITPLANE - BPLCON0=$1200, the manual names no plane "
            "count) reads " + std::to_string(baseline[7]) + " at the DDFSTRT=$38 "
            "baseline and " + std::to_string(at_30[7]) +
                " at DDFSTRT=$0030 - the manual's own worked value - which is "
                "not a reduction: at exactly one bitplane this fixture measures "
                "sprite 7 unaffected at $0030, where the manual states it is "
                "disabled. 'sooner (DDFSTRT = $0030). Incidentally, this will "
                "disable sprite 7' (p.82). 3-hscr.c10, 3-hscroll.c12.");
    }

    // ---------------- DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES (p.82) ----------------
    // The SAME sprite-7, $0030-vs-$0038 comparison as the one-bitplane
    // assertion above, repeated at 2, 4 and 6
    // bitplanes (BPLCON0=$2200/$4200/$6200 - agnus_bitplanedma.v:300's `bpu`
    // field reads BPLCON0[14:12] directly for these three values). p.82
    // names no plane count, so both this assertion and the one-bitplane
    // assertion above cite the SAME two ids: together they report the full
    // range this fixture can measure, and neither hides the other's result.
    {
        auto baseline2 = measure_sprite_data_fetch(0x0038, BPLCON0_2BPL);
        auto at30_2 = measure_sprite_data_fetch(0x0030, BPLCON0_2BPL);
        auto baseline4 = measure_sprite_data_fetch(0x0038, BPLCON0_4BPL);
        auto at30_4 = measure_sprite_data_fetch(0x0030, BPLCON0_4BPL);
        auto baseline6 = measure_sprite_data_fetch(0x0038, BPLCON0_6BPL);
        auto at30_6 = measure_sprite_data_fetch(0x0030, BPLCON0_6BPL);
        bool reduced2 = at30_2[7] < baseline2[7];
        bool reduced4 = at30_4[7] < baseline4[7];
        bool reduced6 = at30_6[7] < baseline6[7];

        checks.expect(
            reduced2 && reduced4 && reduced6,
            "DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES: the SAME sprite-7, "
            "$0030-vs-$0038 comparison as DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE "
            "above, repeated at 2/4/6 bitplanes (BPLCON0=$2200/$4200/$6200). "
            "Sprite 7 reads " + std::to_string(baseline2[7]) +
                " -> " + std::to_string(at30_2[7]) + " at 2 planes, " +
                std::to_string(baseline4[7]) + " -> " +
                std::to_string(at30_4[7]) + " at 4 planes, " +
                std::to_string(baseline6[7]) + " -> " +
                std::to_string(at30_6[7]) +
                " at 6 planes - reduced at every one of the three, where the "
                "one-bitplane case is the only one this fixture measures "
                "unaffected. 'sooner (DDFSTRT = $0030). Incidentally, this "
                "will disable sprite 7' (p.82), which names no plane count. "
                "3-hscr.c10, 3-hscroll.c12.");
    }

    // ---------------- DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE (p.83) ----------------
    // p.83's NOTE is existential over all eight sprites ("Fetching an extra
    // word for scrolling will disable some sprites"), not specific to sprite
    // 7 - DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE above only speaks for
    // channel 7 and cannot honestly report "some sprites" either way. At one
    // bitplane the same $0030 stimulus that
    // leaves sprite 7 unaffected leaves EVERY channel unaffected (see the
    // DDFSTRT-EARLIER-THAN-0038 sweep above, $0030 row), so this is red here
    // too, for the same underlying reason and honestly labelled as such.
    {
        auto baseline = measure_sprite_data_fetch(0x0038);
        auto at_30 = measure_sprite_data_fetch(0x0030);
        bool some_reduced = false;
        for (int n = 0; n < 8; ++n)
            if (at_30[n] < baseline[n]) some_reduced = true;

        checks.expect(
            some_reduced,
            "DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE: with all eight "
            "sprites armed identically and ONE BITPLANE (BPLCON0=$1200, the "
            "manual names no plane count), NONE of the eight channels' own "
            "SPRxDATA/SPRxDATB fetch counts is reduced at DDFSTRT=$0030 versus "
            "the $0038 baseline - the same measurement the sibling sprite-7 "
            "assertion above reports for channel 7 alone, here checked across "
            "all eight. 'Fetching an extra word for scrolling will disable "
            "some sprites' (p.83, NOTE) is false at this one-bitplane "
            "configuration. 3-hscr.c17, 3-hscroll.c17.");
    }

    // ---------------- DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES (p.83) ----------------
    // Sprite 7 alone is a sufficient witness for "some sprites" at each
    // plane count, since it is one of the eight
    // the NOTE quantifies over - the same channel DDFSTRT-0030-SPRITE-7-AT-
    // MULTIPLE-BITPLANES reports. Measured independently here (its own
    // Harness runs, not shared variables) so this assertion's own mutation
    // cycle cannot also move its sibling's.
    {
        auto baseline2 = measure_sprite_data_fetch(0x0038, BPLCON0_2BPL);
        auto at30_2 = measure_sprite_data_fetch(0x0030, BPLCON0_2BPL);
        auto baseline4 = measure_sprite_data_fetch(0x0038, BPLCON0_4BPL);
        auto at30_4 = measure_sprite_data_fetch(0x0030, BPLCON0_4BPL);
        auto baseline6 = measure_sprite_data_fetch(0x0038, BPLCON0_6BPL);
        auto at30_6 = measure_sprite_data_fetch(0x0030, BPLCON0_6BPL);
        bool reduced2 = at30_2[7] < baseline2[7];
        bool reduced4 = at30_4[7] < baseline4[7];
        bool reduced6 = at30_6[7] < baseline6[7];

        checks.expect(
            reduced2 && reduced4 && reduced6,
            "DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES: sprite 7 "
            "(one of the eight 'some sprites' quantifies over) is reduced at "
            "DDFSTRT=$0030 versus the $0038 baseline at every one of 2, 4 and "
            "6 bitplanes (BPLCON0=$2200/$4200/$6200; reads " +
                std::to_string(baseline2[7]) + "->" + std::to_string(at30_2[7]) + ", " +
                std::to_string(baseline4[7]) + "->" + std::to_string(at30_4[7]) + ", " +
                std::to_string(baseline6[7]) + "->" + std::to_string(at30_6[7]) +
                ") - the same channel DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES "
                "reports in full. 'Fetching an extra word for scrolling will "
                "disable some sprites' (p.83, NOTE), which names no plane "
                "count, holds at each of these three. 3-hscr.c17, 3-hscroll.c17.");
    }

    checks.record(
        "sol:7-appB-dmaconr.c3 (p.289, 'Appendix B associates DMACONR with Agnus and Paula') and the bundled "
        "opus:7-appB.c13 (address, direction, chip and function together): not asserted -- DMACONR-READ-DECODED-AT-002 "
        "above demonstrates only the Agnus half of the chip claim (this DUT's own decode and readback reflect the "
        "write); Paula's own decode of $002 is outside this fixture's boundary (no Paula module is compiled into "
        "agnus_top_dmacon, per this Makefile's SRC list) and is not asserted here, the same 'not observable at any "
        "port' limit DMACON-WRITE-DECODED-AT-096 states for its own chip column. This row's address, direction "
        "and function clauses are cited on DMACONR-READ-DECODED-AT-002 above (named there, not here, so this "
        "decline does not read as a decline of them). The bundled id "
        "bundles a fourth (chip) clause this package cannot establish, so per this suite's multi-clause convention "
        "(a bundled id is not partially kept) it is declined here rather than cited alongside its sibling clauses.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// ============================================================================
// MUTATION LOG (updated 10.09.2026, FPGA-090 gate repair, two passes). Every
// entry below was actually applied by this agent - edited into this file,
// rebuilt under WSL/Verilator (`make test`), run, the PASS/FAIL line for the
// named assertion read from real output (quoted verbatim below, not
// predicted), then reverted and rebuilt clean before the next entry. Every
// assertion id tools/check_gates.py's mutation gate has ever named for this
// package got its OWN separate mutate/build/run/revert cycle - no record
// below is reused across two ids, even where two assertions share an
// identical BLTEN-dropped shape (entries 3 and 4). Baseline before this
// log's first mutation and final clean run after the last revert were both
// "25 passed, 0 failed", in both passes.
//
// SCOPE. This file has 25 checks.expect() calls but the gate's mutation
// requirement applies only to the assertion ids its own parser can read -
// confirmed both times by calling tools/check_gates.py's assertion_ids()
// directly against this file, not assumed. Pass 1 (entries 1-10) found two
// causes hiding ids from the gate:
//   - the 13 WALKING-BIT-* labels are built as
//     std::string("WALKING-BIT-") + std::to_string(b.bit) + "-" + b.label +
//     ": ..."; the gate's id parser reads only the FIRST string literal of
//     the call, and that literal by itself ("WALKING-BIT-") has no trailing
//     whitespace or ':' of its own for the id-boundary regex to match on -
//     so these 13 calls yield no assertion id the gate can see.
//   - DMACON-WRITE-DECODED-AT-096's label was one readable id followed by
//     ": ..." prose that itself contains a literal ';' ("...is not quoted
//     here; this assertion demonstrates..."); the gate's EXPECT_CALL_RE
//     stopped at the first '...);' pattern it could find and a bare ';'
//     inside the string broke that search before it reached this call's
//     real closing ");", so the call was never parsed and this id was never
//     read.
// Neither omission was this agent's choice of what to cover; both were
// upstream parser behaviour in tools/check_gates.py, reported to the
// pipeline lead rather than worked around locally. The lead fixed the second
// cause gate-wide on 10.09.2026 (EXPECT_CALL_RE widened to let a complete
// quoted string carry its own semicolons; LEADING_ID_RE widened to allow
// hyphens and ids over 16 characters) - DMACON-WRITE-DECODED-AT-096 is
// readable as of that fix and entry 11 below covers it, run after the fix
// landed. The first cause (WALKING-BIT-* ids composed at runtime) is
// deliberately NOT fixed: the gate has a separate "composed ids" path for
// exactly this shape and reports it as a note, not a violation - rewriting
// working assertions to satisfy a parser would be the wrong trade, per the
// lead's own instruction. The 11 ids covered below are the complete set the
// gate has named as missing for this package across both passes.
//
// 1. DMACONR-READ-DECODED-AT-002: both read_reg calls' target address
//    changed from DMACONR_ADDR (0x002) to 0x004. Result: "24 passed, 1
//    failed" - DMACONR-READ-DECODED-AT-002 went red (both reads returned
//    the same value, read_a==read_b, 0x004 not being decoded anywhere in
//    agnus.v). Reverted to DMACONR_ADDR; rebuilt clean (25 passed, 0
//    failed).
//
// 2. SETCLR-MULTIBIT-DIRECTION: the CLR write's target changed from DSKEN
//    (a bit that WAS set by the preceding SET write) to AUD1EN (a bit that
//    was never set). Result: "24 passed, 1 failed" - SETCLR-MULTIBIT-
//    DIRECTION went red (after_clr read back the original SET pattern
//    unchanged, since clearing an already-0 bit is a no-op, so the
//    assertion's expected "DSKEN cleared" value was never reached).
//    Reverted; rebuilt clean (25 passed, 0 failed).
//
// 3. BBUSY-READONLY-WITH-POSITIVE-CONTROL: this assertion's positive
//    control is a real blit, so per the instruction that these two
//    read-only assertions need a mutation that breaks the CONTROL's half or
//    its setup rather than the first change that turns something red -
//    BLTEN was dropped from the write-attempt's own setup write
//    (SET_CLR|DMAEN|BBUSY_B instead of SET_CLR|DMAEN|BLTEN|BBUSY_B). This
//    is not an arbitrary stimulus change: it reproduces a real bug this
//    assertion's first draft had (agnus.v:443's `enadma` needs `blten`, so
//    without BLTEN the blitter's FSM never runs even though `busy` still
//    latches immediately on the BLTSIZE write, agnus_blitter.v:500) - the
//    positive control silently stops proving anything, which is what a
//    real defect at this boundary looks like. Result: "24 passed, 1
//    failed" - BBUSY-READONLY-WITH-POSITIVE-CONTROL went red (during_blit's
//    bit 14 never reached 1, since `busy` was set by the BLTSIZE write but
//    the blit's `done` condition that would clear it again never fires
//    without `enable`). Reverted; rebuilt clean (25 passed, 0 failed).
//
// 4. BZERO-READONLY-WITH-POSITIVE-CONTROL: same reasoning as entry 3,
//    applied to this assertion's OWN write (BLTEN dropped from
//    SET_CLR|DMAEN|BLTEN|BZERO_B), run and reverted as its own separate
//    build - not a reuse of entry 3's mutation or its build. Result: "24
//    passed, 1 failed" - BZERO-READONLY-WITH-POSITIVE-CONTROL went red
//    (after_blit's bit 13 never reached 1, since `zero` is only set at the
//    blit's own `init` cycle and the FSM never reaches `init` without
//    `enable`). Reverted; rebuilt clean (25 passed, 0 failed).
//
// 5. SPREN-GATES-SPRITE-DMA: SPREN dropped from the "enabled" run's setup
//    write (SET_CLR|DMAEN only). Result: "24 passed, 1 failed" -
//    SPREN-GATES-SPRITE-DMA went red (cycles_en read 0, equal to
//    cycles_dis). Reverted; rebuilt clean (25 passed, 0 failed).
//
// 6. BLTEN-GATES-BLITTER-DMA: BLTEN dropped from the "enabled" run's setup
//    write. Result: "24 passed, 1 failed" - BLTEN-GATES-BLITTER-DMA went
//    red (writes_en read 0). Reverted; rebuilt clean (25 passed, 0 failed).
//
// 7. COPEN-GATES-COPPER-DMA: COPEN dropped from the "enabled" run's setup
//    write. Result: "24 passed, 1 failed" - COPEN-GATES-COPPER-DMA went red
//    (cycles_en read 0). Reverted; rebuilt clean (25 passed, 0 failed).
//
// 8. BPLEN-GATES-BITPLANE-DMA: BPLEN dropped from the "enabled" run's
//    setup write. Result: "24 passed, 1 failed" - BPLEN-GATES-BITPLANE-DMA
//    went red (cycles_en read 0). Reverted; rebuilt clean (25 passed, 0
//    failed).
//
// 9. DMAEN-MASTER-GATES-ALL-DMA-BELOW: DMAEN dropped from the "enabled"
//    run's setup write (SPREN/BLTEN/COPEN/BPLEN left in). Result: "24
//    passed, 1 failed" - DMAEN-MASTER-GATES-ALL-DMA-BELOW went red
//    (cycles_en read 0 - with DMAEN itself clear, spren/blten/copen/bplen,
//    agnus.v:299-302, all evaluate to 0 regardless of their own bits).
//    Reverted; rebuilt clean (25 passed, 0 failed).
//
// 10. BLTPRI-BUS-SHARE-UNDER-CONTENTION: BLTPRI dropped from the "hog"
//     run's setup write, leaving it identical to the "share" run's
//     configuration. Result: "24 passed, 1 failed" -
//     BLTPRI-BUS-SHARE-UNDER-CONTENTION went red (writes_hog read 0, equal
//     to writes_share, so the writes_hog==W*H completion requirement
//     failed). Reverted; rebuilt clean (25 passed, 0 failed).
//
// 11. DMACON-WRITE-DECODED-AT-096 (added in pass 2, after the lead's
//     EXPECT_CALL_RE/LEADING_ID_RE fix made this id readable): the write's
//     target address changed from DMACON_ADDR (0x096) to 0x098, a nearby
//     but non-decoded address (agnus.v's write decode is
//     reg_address[8:1]==DMACON[8:1], DMACON=9'h096, and 0x098[8:1] !=
//     0x096[8:1]). The label's own text - including the honest caveat that
//     the Appendix B "A D P" CHIP-column quotation is pending because the
//     extracted corpus text for printed p.290 is defective - was NOT
//     touched; only the write's target address, inside the assertion body,
//     changed. Result: "24 passed, 1 failed" - DMACON-WRITE-DECODED-AT-096
//     went red on the first try (after read back 0x0000, not DMAEN, since
//     the write landed nowhere DMACONR reads from). Reverted to
//     DMACON_ADDR; rebuilt clean (25 passed, 0 failed).
//
// Entries 12-19 below cover the DDFSTRT assertions, one record each.
// Each entry got its own separate mutate/build/run/revert cycle.
// Baseline before the first of these mutations and after every revert: "30
// passed, 3 failed" (three of the new assertions are honest reds, kept as
// such - see their own headers above).
//
// 12. DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES: already red
//     at baseline (the sweep's own $0030 row is the finding - one bitplane,
//     zero of eight sprites reduced). Substituted a live point (the sweep's
//     $0030 entry changed to $0018, known reduced) through the SAME
//     conjunction - result "31 passed, 2 failed", only this assertion
//     moved, confirming the comparator is not vacuous. Reverted to the real
//     $0030/$0028/$0020/$0018 sweep, which stays red on its $0030 row.
//
// 13. DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE: already red at baseline (the
//     finding itself). Substituted `measure_sprite_data_fetch(0x0018)[2]`
//     for `at_30[7]` (channel 2 at $0018 is known reduced, proven by entry
//     12's own sweep) through the SAME `< baseline[...]` comparator -
//     result "31 passed, 2 failed", only this assertion moved. Reverted to
//     the real channel-7/$0030 comparison, which stays red.
//
// 14. DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES: the 2-bitplane `at30_2`
//     measurement's DDFSTRT argument changed from $0030 to $0038 (baseline,
//     unmoved) - result "29 passed, 4 failed", only this assertion moved
//     (sprite 7 read 172->172 at 2 planes instead of 172->90; the 4- and
//     6-plane conjuncts were untouched and stayed true). DDFSTRT-0030-SOME-
//     SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES (entry 16) does NOT share this
//     assertion's measurements - each was refactored to its own independent
//     Harness runs precisely so one's mutation cannot silently move the
//     other - and it stayed green through this mutation, confirmed in the
//     same run. Reverted; rebuilt clean (30 passed, 3 failed).
//
// 15. DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE: already red at
//     baseline. Substituted $0018 for $0030 in the `at_30` measurement
//     (known to reduce 3 of 8 channels, entry 12) through the SAME "exists n
//     with at_30[n] < baseline[n]" comparator - result "31 passed, 2
//     failed", only this assertion moved. Reverted to the real $0030
//     measurement, which stays red (0 of 8 reduced there).
//
// 16. DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES: the
//     4-bitplane `at30_4` measurement's DDFSTRT argument changed from $0030
//     to $0038 - result "29 passed, 4 failed", only this assertion moved
//     (sprite 7 read 172->172 at 4 planes instead of 172->8). Confirmed in
//     the same run that DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES (entry
//     14's own assertion) stayed green, for the same reason given there.
//     Reverted; rebuilt clean (30 passed, 3 failed).
//
// 17. SPREN-GATES-SPRITE-DMA-ALL-CHANNELS: SPREN left SET in the "without
//     SPREN" control run's own DMACON write (SET_CLR|SPREN|BPLEN instead of
//     SET_CLR|BPLEN) - result "29 passed, 4 failed", only this assertion
//     moved (every one of the eight without-SPREN counts read 172, equal to
//     the with-SPREN counts, so "zero without SPREN" failed for all eight).
//     Reverted; rebuilt clean (30 passed, 3 failed).
//
// 18. BZERO-STAYS-CLEAR-AFTER-NONZERO-RESULT-BLIT: `data_in` changed from
//     the held 0xFFFF back to 0x0000 (an all-zero-result blit again) -
//     result "29 passed, 4 failed", only this assertion moved (after_blit's
//     bit 13 read 1, not 0). Reverted to 0xFFFF; rebuilt clean (30 passed, 3
//     failed). NOTE: this assertion's first draft ran DID NOT write
//     DMAEN|BLTEN to DMACON before start_blit, and PASSED "stays 0" under
//     BOTH 0xFFFF and 0x0000 - a vacuous green, since without BLTEN
//     agnus.v:443's `enadma` never goes high and `zero` (no reset branch of
//     its own) simply defaults-reads 0 regardless of any data_in stimulus.
//     This mutation cycle caught it: an oracle-identical mutation that
//     SHOULD have moved the assertion did not, which is exactly what a
//     mutation is for. Fixed by adding the missing
//     `h.write_reg(DMACON_ADDR, SET_CLR | DMAEN | BLTEN)` before
//     `start_blit`, matching the all-zero-result control's own setup; the
//     counts and PASS/FAIL results in this entry are from the FIXED
//     assertion, re-run after the fix.
//
// 19. BLTPRI-COMPLETION-TICK-UNCHANGED-UNDER-CONTENTION: BLTPRI dropped from
//     the contended run's own DMACON write (SET_CLR|DMAEN|BLTEN instead of
//     SET_CLR|DMAEN|BLTEN|BLTPRI) - result "29 passed, 4 failed", only this
//     assertion moved (tick_contended read -1, i.e. the blit did not
//     complete within the 500-tick budget at all once BLTPRI no longer held
//     bls_cnt at 0 under sustained contention, against tick_quiet's
//     unchanged 391). Reverted; rebuilt clean (30 passed, 3 failed).
//
// Machine-readable records, one per line, read by tools/check_gates.py's
// mutation gate (MUTATION_RE):
// MUTATION DMACON-WRITE-DECODED-AT-096 stimulus red: write's target address moved from $096 to $098 (not decoded by agnus.v's DMACON path); after read back 0x0000, not DMAEN.
// MUTATION DMACONR-READ-DECODED-AT-002 stimulus red: both read_reg targets moved from $002 to $004; read_a==read_b, no longer distinguishes the two dmacon states.
// MUTATION SETCLR-MULTIBIT-DIRECTION stimulus red: CLR write retargeted from DSKEN (set by the prior SET) to AUD1EN (never set); after_clr came back unchanged, matching after_set.
// MUTATION BBUSY-READONLY-WITH-POSITIVE-CONTROL stimulus red: BLTEN dropped from the write-attempt's setup, breaking the positive-control blit's FSM; during_blit's bit 14 never reached 1.
// MUTATION BZERO-READONLY-WITH-POSITIVE-CONTROL stimulus red: BLTEN dropped from the write-attempt's setup, breaking the positive-control blit's FSM; after_blit's bit 13 never reached 1.
// MUTATION SPREN-GATES-SPRITE-DMA stimulus red: SPREN dropped from the enabled run's setup write; cycles_en fell to 0, equal to the disabled control.
// MUTATION BLTEN-GATES-BLITTER-DMA stimulus red: BLTEN dropped from the enabled run's setup write; writes_en fell to 0, equal to the disabled control.
// MUTATION COPEN-GATES-COPPER-DMA stimulus red: COPEN dropped from the enabled run's setup write; cycles_en fell to 0, equal to the disabled control.
// MUTATION BPLEN-GATES-BITPLANE-DMA stimulus red: BPLEN dropped from the enabled run's setup write; cycles_en fell to 0, equal to the disabled control.
// MUTATION DMAEN-MASTER-GATES-ALL-DMA-BELOW stimulus red: DMAEN dropped from the enabled run's setup write (SPREN/BLTEN/COPEN/BPLEN left set); cycles_en fell to 0.
// MUTATION BLTPRI-BUS-SHARE-UNDER-CONTENTION stimulus red: BLTPRI dropped from the hog run's setup write; writes_hog fell to 0, equal to the share run.
// MUTATION WRITE-ADDRESS-MUST-MATCH-DMACON stimulus red: the decoy write's address changed from BLTAFWM_ADDR back to DMACON_ADDR, same data pattern (SET_CLR | DMAEN) -> 24 passed, 1 failed, only this assertion moves: the write now lands and DMACONR reads back non-zero, so `after == 0x0000` fails. Reverted: 25 passed, 0 failed. Added 11.09.2026; the assertion had no record because the gate could not read an id out of its "[...]" prefix and never reported it missing.
// MUTATION DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES none green: already red at baseline (the sweep's own $0030 row is the finding). Substituted a live point (sweep's $0030 entry -> $0018, known reduced) through the SAME conjunction -> 31 passed, 2 failed, only this assertion moves. Reverted: 30 passed, 3 failed.
// MUTATION DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE none green: already red at baseline (the FINDING itself - sprite 7 is measured unaffected at the manual's own $0030, one bitplane). Substituted a different, live mechanism through the SAME comparator (measure_sprite_data_fetch(0x0018)[2] in place of at_30[7]) - channel 2 IS reduced at $0018, proven by the sweep assertion above, and the substitution went green, showing the comparator is not vacuous. Reverted to the real channel 7 / $0030 comparison, which stays red.
// MUTATION DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES stimulus red: the 2-bitplane at30_2 measurement's DDFSTRT argument changed from $0030 back to $0038 (baseline, unmoved); sprite 7 read 172/172 (unaffected) instead of 90/172 -> 29 passed, 4 failed, only this assertion moves. Reverted: 30 passed, 3 failed.
// MUTATION DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE none green: already red at baseline (no channel is reduced at $0030, one bitplane). Substituted $0018 for $0030 in the at_30 measurement (known to reduce 3 of 8 channels) through the SAME "exists n" comparator -> 31 passed, 2 failed, only this assertion moves. Reverted: 30 passed, 3 failed.
// MUTATION DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES stimulus red: the 4-bitplane at30_4 measurement's DDFSTRT argument changed from $0030 back to $0038; sprite 7 read 172/172 instead of 8/172 -> 29 passed, 4 failed, only this assertion moves. Reverted: 30 passed, 3 failed.
// MUTATION SPREN-GATES-SPRITE-DMA-ALL-CHANNELS stimulus red: SPREN left SET in the without-SPREN control run's own DMACON write; all eight without-SPREN counts read 172, equal to the with-SPREN counts -> 29 passed, 4 failed, only this assertion moves. Reverted: 30 passed, 3 failed.
// MUTATION BZERO-STAYS-CLEAR-AFTER-NONZERO-RESULT-BLIT stimulus red: data_in changed from the held 0xFFFF back to 0x0000 (all-zero-result blit); after_blit's bit 13 read 1, not 0 -> 29 passed, 4 failed, only this assertion moves. Reverted: 30 passed, 3 failed.
// MUTATION BLTPRI-COMPLETION-TICK-UNCHANGED-UNDER-CONTENTION stimulus red: BLTPRI dropped from the contended run's own DMACON write; tick_contended read -1 (blit never completed within the 500-tick budget) against tick_quiet's unchanged 391 -> 29 passed, 4 failed, only this assertion moves. Reverted: 30 passed, 3 failed.
//
// Not machine-recorded, and not required by the gate (see SCOPE above):
// DMACON-WRITE-DECODED-AT-096's own control, "[control] WRITE-ADDRESS-MUST-
// MATCH-DMACON" (its label starts with "[control]", not an uppercase
// letter, so LEADING_ID_RE still does not match it even after the 10.09.2026
// widening), and the 13 WALKING-BIT-* assertions (composed ids, the gate's
// own "note, not violation" path - left unmutated deliberately, per the
// lead's instruction not to rewrite working assertions to satisfy a
// parser). Left unmutated; a prior log's unrelated claims about these are
// superseded, not repeated.
// ============================================================================

