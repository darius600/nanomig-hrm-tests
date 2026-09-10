// Test purpose: Verify, for the `agnus_blitter` module, every blitter
//   register's custom-chip-bus offset and access type (write-only), the
//   absence of ANY read path back to the processor at this boundary, that a
//   write to BLTSIZE behaves as the strobe that starts a blit, and that
//   BLTDDAT -- unlike every other register listed here -- has no write
//   decode at all: nothing at its documented (or, per Appendix A, its
//   entirely absent) address reaches an internal register through the
//   `reg_address_in`/`data_in` write path.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 170-171 (chapter 6 prose, "DMA Channels"), 266-267 (Appendix A,
//   BLTCON0/BLTCON1/BLTAFWM/BLTALWM rows), 269-270 (Appendix A, BLTCON0L/
//   BLTDDAT/BLTSIZE/BLTxDAT/BLTxMOD rows and prose), 287 (Appendix B
//   legend), 289-290 (Appendix B register table), 309 (Appendix C, ECS
//   BLTCON1 row).
// Non-goals (explicitly another package's job, not re-litigated here):
//   minterm/truth-table correctness for a given LF byte, or any BLTCON0
//   field's bit position other than "the LF byte reaches the minterm unit
//   at all" (PKG-BLT-MINTERM); barrel-shifter correctness for a given ASH/
//   BSH value, or any BLTCON1 field's bit position (PKG-BLT-BLTCON-FIELDS,
//   PKG-BLT-SHIFT/-SHIFT-CHAIN); fill-mode correctness (PKG-BLT-FILL);
//   line-mode octant/Bresenham correctness (PKG-BLT-LINE); descending-mode
//   pointer-step correctness beyond "DESC visibly changes something"
//   (PKG-BLT-DESC); BLTSIZE/BLTSIZH/BLTSIZV's exact field widths, maximum
//   values, and ECS size-combination arithmetic (PKG-BLT-SIZE); BLTxMOD's
//   exact numeric role in address stepping beyond "a written modulo visibly
//   changes address stepping" (PKG-BLT-ADRGEN); DMACON's own address decode
//   and the CDANG/Copper-vs-CPU write-arbitration the Appendix B "*"/"~"
//   marks describe, which do not live inside this module at all (see
//   DEFERRED below; PKG-AGNUS-DMACON's job, concurrently in progress in
//   this same repository as this package is built).
//
// BOUNDARY: `agnus_blitter`, per this package's build brief (row 5 of
// hrm-testpipeline/work/2026-09-08_fpga-090_packages.md and
// hrm-testpipeline/work/2026-09-08_fpga-090_build_register.md).
//
// PROVENANCE / SPEC-BEFORE-RTL GATE: this file is committed before
// agnus_blitter.v or any nanomig/ source file has been opened by the agent
// writing it. Every register offset, port name (busy, int3, we, data_out,
// address_out, reg_address_out, reqdma, ackdma, reg_address_in, data_in,
// ecs, clkena, enadma, zero) and internal RTL citation appearing anywhere
// in this header comes from material already handed to this task rather
// than from this agent's own reading of the RTL:
//   (1) the build brief itself (FPGA-090 Prompt 7, sections C and D), which
//       quotes agnus.v/agnus_blitter.v line numbers directly and names the
//       fixture traps found while building the sibling packages;
//   (2) the `classify.evidence` field already present on every one of the
//       109 claims in work/2026-09-08_fpga-090_classified.jsonl -- produced
//       during Prompt 4 (classification), which mandates RTL reading and
//       predates this build task; using it here is using the deliverable
//       this task explicitly says to filter and use, not fresh RTL reading;
//   (3) already-committed sibling test files in this repository
//       (hrm/agnus_blitter_cycles/test_agnus_blitter_cycles.cpp,
//       hrm/agnus_blitter_bltcon/test_agnus_blitter_bltcon.cpp,
//       hrm/agnus_blitter_status/test_agnus_blitter_status.cpp,
//       hrm/agnus_slots/test_agnus_slots.cpp), read because the build brief
//       explicitly directs this package to copy its DMA-grant harness from
//       agnus_blitter_cycles rather than reinvent it, and because the
//       brief names agnus_blitter_bltcon as "a worked example of driving
//       many different registers at this boundary".
// No RTL line number, register offset or port name below was produced by
// this agent opening agnus_blitter.v, agnus_blitter_adrgen.v,
// agnus_blitter_minterm.v, agnus_blitter_barrelshifter.v or
// agnus_blitter_fill.v -- that reading happens only in the next commit.
//
// CLAIMS (verbatim from work/2026-09-08_fpga-090_classified.jsonl, filtered
// on classify.package == "PKG-BLT-REGDECODE" -- 109 records, all
// classify.status == "testable", classify.observable == "yes, at
// `agnus_blitter`" except ch6-appb-dmacon.rw, discussed under DEFERRED).
// Grouped below by the PLANNED ASSERTION each group will support; the
// grouping is this agent's organisation of the inventory, not a field in
// the inventory itself. Every id is written out in full.
//
//   [A1] BLTCON0-DECODES-AT-0x040-WRITE-ONLY
//     id: ch6-appa-bltcon0.c1 (reader: opus)  page 266
//       quote: "BLTCON0    040      W       A      Blitter control register 0"
//       restatement: BLTCON0 is at address 040.
//     id: ch6-appa-bltcon0.c2 (reader: opus)  page 266
//       quote: "BLTCON0    040      W       A      Blitter control register 0"
//       restatement: BLTCON0 is write-only.
//     id: 6-appa-registers.c14 (reader: sol)  page 266
//       quote: "BLTCON0    040      W       A      Blitter control register 0"
//       restatement: BLTCON0 is at custom-register offset $040.
//     id: 6-appa-registers.c15 (reader: sol)  page 266
//       quote: "BLTCON0    040      W       A      Blitter control register 0"
//       restatement: BLTCON0 is writable.
//     id: 6-appa-registers.c17 (reader: sol)  page 266
//       quote: "BLTCON0    040      W       A      Blitter control register 0"
//       restatement: BLTCON0 is blitter control register 0.
//     id: ch6-appb-bltcon0.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTCON0 at address 040 and describes it as "Blitter control register 0".
//     id: ch6-appb-bltcon0.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTCON0 W in the R/W column and A in the CHIP column.
//
//   [A2] BLTCON1-DECODES-AT-0x042-WRITE-ONLY
//     id: ch6-appa-bltcon0.c4 (reader: opus)  page 267
//       quote: "BLTCON1    042      W      A( E )  Blitter control register 1"
//       restatement: BLTCON1 is at address 042.
//     id: ch6-appa-bltcon0.c5 (reader: opus)  page 267
//       quote: "BLTCON1    042      W      A( E )  Blitter control register 1"
//       restatement: BLTCON1 is write-only.
//     id: 6-appa-registers.c18 (reader: sol)  page 267
//       quote: "BLTCON1    042      W      A( E )  Blitter control register 1"
//       restatement: BLTCON1 is at custom-register offset $042.
//     id: 6-appa-registers.c19 (reader: sol)  page 267
//       quote: "BLTCON1    042      W      A( E )  Blitter control register 1"
//       restatement: BLTCON1 is writable.
//     id: 6-appa-registers.c21 (reader: sol)  page 267
//       quote: "BLTCON1    042      W      A( E )  Blitter control register 1"
//       restatement: BLTCON1 is blitter control register 1.
//     id: ch6-appb-bltcon1.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTCON1 at address 042 and describes it as "Blitter control register 1".
//     id: ch6-appb-bltcon1.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTCON1 W in the R/W column and A(E) in the CHIP column.
//     id: ch6-appc-ecs.c5 (reader: opus)  page 309
//       quote: "BLTCON1   042   W   A      Blitter control register 1"
//       restatement: BLTCON1 is at address 042, write-only, in Agnus.
//
//   [A3] BLTCON0L-DECODES-AT-0x05A-ECS-GATED-WRITE-ONLY
//     id: ch6-appa-bltcon0l.c1 (reader: opus)  page 269
//       quote: "BLTCON0L   05A      W     A( E ) Blitter control 0, lower 8 bits (minterms)"
//       restatement: BLTCON0L is at address 05A.
//     id: ch6-appa-bltcon0l.c2 (reader: opus)  page 269
//       quote: "BLTCON0L   05A      W     A( E ) Blitter control 0, lower 8 bits (minterms)"
//       restatement: BLTCON0L is write-only.
//     id: ch6-appb-bltcon0l.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTCON0L at address 05A and describes it as "Blitter control 0, lower 8 bits (minterms)".
//     id: ch6-appb-bltcon0l.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTCON0L W in the R/W column and A(E) in the CHIP column.
//     id: 6-appendix-b.c18 (reader: sol)  page 289
//       quote: "BLTCON0L ~05A W A(E) Blitter control 0, lower 8 bits (minterms)"
//       restatement: In Appendix B's table, ECS BLTCON0L at $05A carries ~; Appendix C p.310 supplies the ECS access interpretation.
//
//   [A4] BLTAFWM-DECODES-AT-0x044-WRITE-ONLY
//     id: ch6-appa-masks.c1 (reader: opus)  page 266
//       quote: "BLTAFWM    044      W       A      Blitter first-word mask for source A"
//       restatement: BLTAFWM is at address 044.
//     id: ch6-appa-masks.c2 (reader: opus)  page 266
//       quote: "BLTAFWM    044      W       A      Blitter first-word mask for source A"
//       restatement: BLTAFWM is write-only.
//     id: ch6-appb-bltafwm.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTAFWM at address 044 and describes it as "Blitter first word mask for source A".
//     id: ch6-appb-bltafwm.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTAFWM W in the R/W column and A in the CHIP column.
//
//   [A5] BLTALWM-DECODES-AT-0x046-WRITE-ONLY
//     id: ch6-appa-masks.c5 (reader: opus)  page 266
//       quote: "BLTALWM    046      W       A      Blitter last-word mask for source A"
//       restatement: BLTALWM is at address 046.
//     id: ch6-appa-masks.c6 (reader: opus)  page 266
//       quote: "BLTALWM    046      W       A      Blitter last-word mask for source A"
//       restatement: BLTALWM is write-only.
//     id: ch6-appb-bltalwm.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTALWM at address 046 and describes it as "Blitter last word mask for source A".
//     id: ch6-appb-bltalwm.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTALWM W in the R/W column and A in the CHIP column.
//
//   [A6] BLTAMOD-DECODES-AT-0x064-WRITE-ONLY
//     id: ch6-appb-bltamod.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTAMOD at address 064 and describes it as "Blitter modulo for source A".
//     id: ch6-appb-bltamod.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTAMOD W in the R/W column and A in the CHIP column.
//
//   [A7] BLTBMOD-DECODES-AT-0x062-WRITE-ONLY
//     id: ch6-appb-bltbmod.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTBMOD at address 062 and describes it as "Blitter modulo for source B".
//     id: ch6-appb-bltbmod.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTBMOD W in the R/W column and A in the CHIP column.
//
//   [A8] BLTCMOD-DECODES-AT-0x060-WRITE-ONLY
//     id: ch6-appb-bltcmod.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTCMOD at address 060 and describes it as "Blitter modulo for source C".
//     id: ch6-appb-bltcmod.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTCMOD W in the R/W column and A in the CHIP column.
//
//   [A9] BLTDMOD-DECODES-AT-0x066-WRITE-ONLY
//     id: ch6-appb-bltdmod.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTDMOD at address 066 and describes it as "Blitter modulo for destination D".
//     id: ch6-appb-bltdmod.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTDMOD W in the R/W column and A in the CHIP column.
//
//   [A10] BLTAPTH-BLTAPTL-DECODE-AT-0x050-0x052-WRITE-ONLY
//     id: ch6-appb-bltapth.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTAPTH at address 050 and describes it as "Blitter pointer to source A (high 3 bits)".
//     id: ch6-appb-bltapth.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTAPTH W in the R/W column and A(E) in the CHIP column.
//     id: ch6-appb-bltaptl.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTAPTL at address 052 and describes it as "Blitter pointer to source A (low 15 bits)".
//     id: ch6-appb-bltaptl.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTAPTL W in the R/W column and A in the CHIP column.
//
//   [A11] BLTBPTH-BLTBPTL-DECODE-AT-0x04C-0x04E-WRITE-ONLY
//     id: ch6-appb-bltbpth.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTBPTH at address 04C and describes it as "Blitter pointer to source B (high 3 bits)".
//     id: ch6-appb-bltbpth.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTBPTH W in the R/W column and A in the CHIP column.
//     id: ch6-appb-bltbptl.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTBPTL at address 04E and describes it as "Blitter pointer to source B (low 15 bits)".
//     id: ch6-appb-bltbptl.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTBPTL W in the R/W column and A in the CHIP column.
//
//   [A12] BLTCPTH-BLTCPTL-DECODE-AT-0x048-0x04A-WRITE-ONLY
//     id: ch6-appb-bltcpth.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTCPTH at address 048 and describes it as "Blitter pointer to source C (high 3 bits)".
//     id: ch6-appb-bltcpth.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTCPTH W in the R/W column and A in the CHIP column.
//     id: ch6-appb-bltcptl.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTCPTL at address 04A and describes it as "Blitter pointer to source C (low 15 bits)".
//     id: ch6-appb-bltcptl.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTCPTL W in the R/W column and A in the CHIP column.
//
//   [A13] BLTDPTH-BLTDPTL-DECODE-AT-0x054-0x056-WRITE-ONLY
//     id: ch6-appb-bltdpth.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTDPTH at address 054 and describes it as "Blitter pointer to destination D (high 3 bits)".
//     id: ch6-appb-bltdpth.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTDPTH W in the R/W column and A in the CHIP column.
//     id: ch6-appb-bltdptl.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTDPTL at address 056 and describes it as "Blitter pointer to destination D (low 15 bits)".
//     id: ch6-appb-bltdptl.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTDPTL W in the R/W column and A in the CHIP column.
//
//   [A14] BLIT-NEEDS-ONLY-SOURCE-POINTER-DEST-POINTER-AND-SIZE
//     id: 6-dma-channels.c37 (reader: sol)  page 171
//       quote: "be told only the starting address (20), the destination address, and the"
//       restatement: A rectangular copy is specified with a starting address.
//     id: 6-dma-channels.c38 (reader: sol)  page 171
//       quote: "be told only the starting address (20), the destination address, and the"
//       restatement: A rectangular copy is specified with a destination address.
//     id: 6-dma-channels.c39 (reader: sol)  page 171
//       quote: "size of the block (height = 6, width = 7). It will then automatically move"
//       restatement: A rectangular copy is specified with a block height and width.
//     id: ch6-dma.c35 (reader: opus)  page 171
//       quote: "be told only the starting address (20), the destination address, and the"
//       restatement: To copy a block the blitter needs to be told only the starting address, the destination address and the size of the block.
//
//   [A15] BLTSIZE-DECODES-AT-0x058-WRITE-ONLY-CONTAINS-WIDTH-HEIGHT
//     id: ch6-appa-bltsize.c1 (reader: opus)  page 269
//       quote: "BLTSIZE    058      W       A      Blitter start and size (window width,"
//       restatement: BLTSIZE is at address 058.
//     id: ch6-appa-bltsize.c2 (reader: opus)  page 269
//       quote: "BLTSIZE    058      W       A      Blitter start and size (window width,"
//       restatement: BLTSIZE is write-only.
//     id: ch6-appa-bltsize.c4 (reader: opus)  page 269
//       quote: "This register contains the width and height of"
//       restatement: BLTSIZE contains the width and height of the blitter operation.
//     id: 6-appa-registers.c36 (reader: sol)  page 269
//       quote: "BLTSIZE    058      W       A      Blitter start and size (window width,"
//       restatement: BLTSIZE is at custom-register offset $058.
//     id: 6-appa-registers.c37 (reader: sol)  page 269
//       quote: "BLTSIZE    058      W       A      Blitter start and size (window width,"
//       restatement: BLTSIZE is writable.
//     id: 6-appa-registers.c39 (reader: sol)  page 269
//       quote: "This register contains the width and height of"
//       restatement: BLTSIZE contains blit width.
//     id: 6-appa-registers.c40 (reader: sol)  page 269
//       quote: "This register contains the width and height of"
//       restatement: BLTSIZE contains blit height.
//     id: ch6-appb-bltsize.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTSIZE at address 058 and describes it as "Blitter start and size (window width, height)".
//     id: ch6-appb-bltsize.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTSIZE W in the R/W column and A in the CHIP column.
//
//   [A16] BLTSIZE-WRITE-IS-A-STROBE-THAT-STARTS-THE-BLIT
//     id: ch6-appb-legend.s (reader: opus)  page 287
//       quote: "[SCAN appendix B legend -- printed page 287 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.287]"
//       restatement: Appendix B's legend defines S as "Strobe (write address with no register bits).  Writing the register causes the effect."
//
//   [A17] BLTSIZV-BLTSIZH-DECODE-AT-0x05C-0x05E-ECS-GATED-WRITE-ONLY
//     id: ch6-appb-bltsizv.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTSIZV at address 05C and describes it as "Blitter V size (for 15 bit vertical size)".
//     id: ch6-appb-bltsizv.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTSIZV W in the R/W column and A(E) in the CHIP column.
//     id: ch6-appb-bltsizh.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTSIZH at address 05E and describes it as "Blitter H size and start (for 11 bit H size)".
//     id: ch6-appb-bltsizh.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTSIZH W in the R/W column and A(E) in the CHIP column.
//     id: 6-appendix-b.c19 (reader: sol)  page 289
//       quote: "BLTSIZV ~05C W A(E) Blitter V size (for 15 bit vertical size)"
//       restatement: In Appendix B's table, ECS BLTSIZV at $05C carries ~; Appendix C p.310 supplies the ECS access interpretation.
//     id: 6-appendix-b.c20 (reader: sol)  page 289
//       quote: "BLTSIZH ~05E W A(E) Blitter H size and start (for 11 bit H size)"
//       restatement: In Appendix B's table, ECS BLTSIZH at $05E carries ~; Appendix C p.310 supplies the ECS access interpretation.
//
//   [A18] BLTADAT-DECODES-AT-0x074-WRITE-ONLY
//     id: ch6-appb-bltadat.addr (reader: opus)  page 290
//       quote: "[SCAN appendix B register table -- printed page 290 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.290]"
//       restatement: Appendix B places BLTADAT at address 074 and describes it as "Blitter source A data register".
//     id: ch6-appb-bltadat.rw (reader: opus)  page 290
//       quote: "[SCAN appendix B register table -- printed page 290 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.290]"
//       restatement: Appendix B marks BLTADAT W in the R/W column and A in the CHIP column.
//     id: 6-appa-registers.c76 (reader: sol)  page 269
//       quote: "BLTxDAT    074      W       A     Blitter source x data register"
//       restatement: The generic BLTxDAT entry uses offset $074 for x=A; Appendix B enumerates B and C at their own offsets.
//     id: 6-appa-registers.c77 (reader: sol)  page 269
//       quote: "BLTxDAT    074      W       A     Blitter source x data register"
//       restatement: BLTxDAT is writable.
//     id: ch6-appa-bltxdat.c1 (reader: opus)  page 269
//       quote: "BLTxDAT    074      W       A     Blitter source x data register"
//       restatement: BLTxDAT is at address 074.
//     id: ch6-appa-bltxdat.c2 (reader: opus)  page 269
//       quote: "BLTxDAT    074      W       A     Blitter source x data register"
//       restatement: BLTxDAT is write-only.
//
//   [A19] BLTBDAT-DECODES-AT-0x072-WRITE-ONLY
//     id: ch6-appb-bltbdat.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTBDAT at address 072 and describes it as "Blitter source B data register".
//     id: ch6-appb-bltbdat.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTBDAT W in the R/W column and A in the CHIP column.
//
//   [A20] BLTCDAT-DECODES-AT-0x070-WRITE-ONLY
//     id: ch6-appb-bltcdat.addr (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B places BLTCDAT at address 070 and describes it as "Blitter source C data register".
//     id: ch6-appb-bltcdat.rw (reader: opus)  page 289
//       quote: "[SCAN appendix B register table -- printed page 289 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.289]"
//       restatement: Appendix B marks BLTCDAT W in the R/W column and A in the CHIP column.
//
//   [A21] EACH-CHANNEL-DATA-REGISTER-IS-SEPARATELY-DECODED-AND-SUPPLIES-THE-BLITTER
//     id: 6-appa-registers.c79 (reader: sol)  page 269
//       quote: "This register holds source x (x=A,B,C) data for"
//       restatement: BLTxDAT holds source data for x=A, B, or C.
//     id: 6-appa-registers.c80 (reader: sol)  page 269
//       quote: "use by the blitter. It is normally loaded by the"
//       restatement: BLTxDAT supplies its value to the blitter.
//     id: ch6-appa-bltxdat.c4 (reader: opus)  page 269
//       quote: "This register holds source x (x=A,B,C) data for"
//       restatement: BLTxDAT holds source x data, x being A, B or C, for use by the blitter.
//     id: 6-dma-channels.c8 (reader: sol)  page 170
//       quote: "separate address pointer, modulo and data registers and an enable bit. Two"
//       restatement: Every blitter DMA channel has a separate data register.
//     id: ch6-dma.c6 (reader: opus)  page 170
//       quote: "separate address pointer, modulo and data registers and an enable bit. Two"
//       restatement: Each blitter DMA channel has a separate data register.
//
//   [A22] BLTDDAT-HAS-NO-PROCESSOR-VISIBLE-ADDRESS-AND-CANNOT-BE-READ
//     id: 6-appa-registers.c29 (reader: sol)  page 269
//       quote: "BLTDDAT    ---      -      ----  Blitter destination data register"
//       restatement: BLTDDAT has no processor-visible register address.
//     id: 6-appa-registers.c30 (reader: sol)  page 269
//       quote: "BLTDDAT    ---      -      ----  Blitter destination data register"
//       restatement: BLTDDAT is not processor-readable or processor-writable.
//     id: 6-appa-registers.c34 (reader: sol)  page 269
//       quote: "cannot be read by the micro.  The transfer is"
//       restatement: The microprocessor cannot read BLTDDAT.
//     id: ch6-appa-bltddat.c1 (reader: opus)  page 269
//       quote: "BLTDDAT    ---      -      ----  Blitter destination data register"
//       restatement: Appendix A gives BLTDDAT no address, printing "---" in the address column.
//     id: ch6-appa-bltddat.c2 (reader: opus)  page 269
//       quote: "BLTDDAT    ---      -      ----  Blitter destination data register"
//       restatement: BLTDDAT is the blitter destination data register.
//     id: ch6-appa-bltddat.c5 (reader: opus)  page 269
//       quote: "cannot be read by the micro.  The transfer is"
//       restatement: BLTDDAT cannot be read by the micro.
//
//   [A23] BLTDDAT-HAS-NO-WRITE-DECODE-TRANSFER-IS-AUTOMATIC
//     id: 6-appa-registers.c31 (reader: sol)  page 269
//       quote: "This register holds the data resulting from each"
//       restatement: BLTDDAT holds the result of each word operation.
//     id: 6-appa-registers.c32 (reader: sol)  page 269
//       quote: "word of blitter operation until it is sent to a"
//       restatement: BLTDDAT holds a result until it is sent to RAM.
//     id: 6-appa-registers.c33 (reader: sol)  page 269
//       quote: "RAM destination.  This is a dummy address and"
//       restatement: BLTDDAT is a dummy address.
//     id: 6-appa-registers.c35 (reader: sol)  page 269
//       quote: "automatic during blitter operation."
//       restatement: Transfer from BLTDDAT to RAM is automatic during blitter operation.
//     id: ch6-appa-bltddat.c3 (reader: opus)  page 269
//       quote: "This register holds the data resulting from each"
//       restatement: BLTDDAT holds the data resulting from each word of blitter operation until it is sent to a RAM destination.
//     id: ch6-appa-bltddat.c4 (reader: opus)  page 269
//       quote: "RAM destination.  This is a dummy address and"
//       restatement: BLTDDAT is a dummy address.
//     id: ch6-appa-bltddat.c6 (reader: opus)  page 269
//       quote: "cannot be read by the micro.  The transfer is"
//       restatement: The transfer through BLTDDAT is automatic during blitter operation.
//     id: 6-appendix-b.c3 (reader: sol)  page 289
//       quote: "BLTDDAT & *000 ER A Blitter destination early read (dummy address)"
//       restatement: The Appendix B BLTDDAT row carries *, so the Copper cannot write address $000.
//
//   [A24] NO-REGISTER-IN-THIS-MODULE-HAS-A-READ-PATH
//     id: ch6-appb-legend.wr (reader: opus)  page 287
//       quote: "[SCAN appendix B legend -- printed page 287 -- no text file in this corpus; read the page at hrm/Amiga_Hardware_Reference_Manual_3rd_edition.pdf printed p.287]"
//       restatement: Appendix B's legend defines W as write-only and R as read-only.
//// DEFERRED -- claims classified into this package that this agent could NOT
// turn into a PKG-BLT-REGDECODE assertion, with the reason for each. Kept
// out rather than forced, per the build brief's HALT-AND-REPORT list ("a
// claim cannot be settled by the port list or by a measurement"). Every id
// below is one of the 109 filtered from classify.package ==
// "PKG-BLT-REGDECODE"; none is invented.
//
//   6-appa-registers.c41 (p.269, quote "the blitter operation (in line
//     mode, width must", restatement "In line mode, BLTSIZE width must
//     equal 2.") -- a field-CONTENT/value-range claim (what a valid line-
//     mode width value is), not an address-decode or access-type claim.
//     PKG-BLT-SIZE's or PKG-BLT-LINE's domain, not this package's four-part
//     scope (offset, access type, read-path absence, BLTSIZE-as-strobe,
//     BLTDDAT-no-write-decode) as stated in packages.md row 5.
//   6-appa-registers.c61 (p.269, quote "h=height=vertical lines (10
//     bits=1024 lines max)", restatement "BLTSIZE height has a maximum of
//     1024 lines.") -- field-content (bit width / max value), not decode.
//     PKG-BLT-SIZE's domain.
//   6-appa-registers.c64 (p.269, quote "words=1024 pixels max)",
//     restatement "BLTSIZE width has a maximum of 1024 pixels.") --
//     field-content (bit width / max value), not decode. PKG-BLT-SIZE's
//     domain.
//   6-appa-registers.c67 (p.269, quote "LINE DRAW   controls the line
//     length (10 bits gives", restatement "The BLTSIZE h field is ten bits
//     wide in original-chip-set line mode.") -- field-content (bit width),
//     not decode. PKG-BLT-SIZE's or PKG-BLT-LINE's domain.
//   6-appa-registers.c93 (p.270, quote "address point to the start of the
//     next line.  Each", restatement "Modulo application advances the
//     address to the next line's start.") -- a field-content/behaviour
//     claim about what a modulo DOES to the address generator, not that
//     BLTxMOD decodes at its offset (which IS asserted, in A6-A9).
//     PKG-BLT-ADRGEN's domain (92 claims, already built).
//   ch6-appa-bltxmod.c13 (p.270, quote "LINE DRAW   being drawn (normally
//     two times the", restatement "That width is normally two times the
//     screen width in words.") -- a typical-value/content claim about what
//     BLTxMOD usually holds, not that it decodes. PKG-BLT-ADRGEN's domain.
//   ch6-appb-dmacon.rw (p.290, quote "[SCAN appendix B register table --
//     printed page 290 ...]", restatement "Appendix B marks DMACON W in
//     the R/W column and A D P in the CHIP column.") -- TENTATIVE pending
//     the next commit's RTL read, recorded here because the doubt exists
//     BEFORE that read, not manufactured after: classify.observable says
//     "yes, at `agnus_blitter`" and classify.evidence reuses this
//     package's general write-only-as-absence-of-a-read-path argument, but
//     every sibling harness already read for this package's fixture
//     (agnus_blitter_cycles, agnus_blitter_bltcon, agnus_blitter_status,
//     agnus_slots) drives DMACON's effect on this module as the
//     pre-decoded `enadma` INPUT PORT, never as a register offset written
//     through `reg_address_in`/`data_in` -- and none of the blitter
//     register offsets this package's own claims name (0x040-0x074) is
//     DMACON's real address (0x096, outside that range). If agnus_blitter.v
//     confirms DMACON has no decode of its own inside this module, this
//     claim needs the wider `agnus` boundary and is PKG-AGNUS-DMACON's job
//     -- the package concurrently being built in this same repository right
//     now, per this task's own concurrency note. Resolved, one way or the
//     other, with a citation to the actual RTL, in the next commit; not
//     asserted here either way.
//
// PLANNED ASSERTIONS (names and intent only -- no RTL line numbers, no
// register-offset arithmetic beyond what Appendix A/B already print, no
// fixture code below this comment block; the harness, the Makefile and the
// assertion bodies are added in the next commit, after agnus_blitter.v and
// its submodules are opened):
//
//   A1-A13, A15, A17-A20 (fourteen assertions): for each control, mask,
//     modulo, pointer, size and data register named in the groups above,
//     drive a short blit through the register-write port with a
//     distinguishing value at that register's documented offset and
//     confirm SOME behaviour the manual already ties to that register
//     changes (minterm output for BLTCON0/BLTCON0L, address stepping for
//     BLTCON1/pointers/modulos, first/last-word content for BLTAFWM/
//     BLTALWM, fetched-word content for BLTxDAT, iteration count for
//     BLTSIZE/BLTSIZH/BLTSIZV) -- and that writing the IDENTICAL value to a
//     neighbouring, undocumented offset changes nothing. That second half
//     is both this group's control and its stimulus mutation: retarget the
//     write to the wrong offset, confirm the assertion goes red, revert.
//     BLTCON0L/BLTSIZH/BLTSIZV additionally assert the write has effect
//     only when `ecs=1` (their Appendix A "(E)" mark / Appendix B "~"
//     mark), with `ecs=0` as the within-assertion control.
//   A14: run one small end-to-end copy using only a source pointer, a
//     destination pointer and BLTSIZE -- nothing else -- and confirm it
//     reproduces the source words at the destination, directly against
//     p.171's "told only the starting address, the destination address,
//     and the size of the block". Mutation: omit one of the three and
//     confirm the copy no longer matches.
//   A16: confirm writing BLTSIZE makes `busy` rise on the same cycle,
//     independent of any other register -- the strobe effect the Appendix
//     B legend's S mark describes in the abstract, demonstrated concretely
//     on the one register in this package most plausibly built that way.
//     Mutation: retarget the write to a neighbouring offset, confirm `busy`
//     never rises, revert. THIS is the package's positive control for every
//     absence claim below (A22-A24): the same busy-rises measurement,
//     applied to a register that IS decoded, is what proves the fixture
//     can detect a write's effect at all.
//   A21: confirm writing BLTADAT, BLTBDAT and BLTCDAT independently changes
//     only their own channel's contribution to the blit result, never the
//     other two. Mutation: swap which channel's write is checked against
//     which channel's expected content, confirm the (now mismatched)
//     assertion goes red, revert.
//   A22: confirm BLTDDAT has no address a CPU write can target -- Appendix
//     A prints "---", not a number -- and that nothing in this module's
//     port list constitutes a processor-side read of it or of any other
//     blitter register (dovetails with A24). Mutation: point the "read"
//     probe at a register that DOES have externally visible content via a
//     DMA grant (a pointer's `address_out`) instead, confirm the assertion
//     -- which expects no such visibility for BLTDDAT specifically --
//     still correctly distinguishes the two, revert.
//   A23: confirm that NO value driven through `reg_address_in`/`data_in`
//     at BLTDDAT's Appendix-B address ($000) reaches the D-channel result
//     register -- unlike every other register in A1-A21, where a write
//     provably does reach something. Uses A16's busy-rise measurement as
//     its named control: the identical write mechanism that starts a blit
//     on BLTSIZE must, applied to BLTDDAT's address, do nothing to the
//     D-channel word. Mutation: apply the same write to a real data
//     register's address (e.g. BLTADAT) instead of BLTDDAT's, confirm the
//     assertion -- which expects no change from BLTDDAT's address
//     specifically -- correctly reports red for the substituted register,
//     revert.
//   A24: a module-wide sweep confirming no register offset in this
//     package's table produces ANY output distinguishable as a "read
//     result" when driven back through `reg_address_in` without a
//     corresponding DMA grant -- the general form of A22/A23, run once
//     across the whole offset table rather than only at BLTDDAT. Uses
//     A16 as its control for the same reason as A23.
//
// A sanity precondition, not tied to a manual page: this package's DMA-
// grant harness must be shown to actually deliver written source words
// through to the observed output before any data-dependent assertion above
// is trusted (TEST-DATA-DELIVERY, per every sibling package's header).
//
int main() {
    // Placeholder -- the real fixture, Makefile and assertion bodies are
    // added in the next commit, after this specification is committed on
    // its own, per the build brief's spec-before-RTL gate.
    return 0;
}
