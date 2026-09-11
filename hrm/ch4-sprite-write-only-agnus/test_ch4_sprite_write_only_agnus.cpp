// Test purpose: the sixteen sprite pointer registers Agnus decodes -
//   SPR0PTH/SPR0PTL through SPR7PTH/SPR7PTL - are write-only: a read aimed
//   at one of their own addresses does not return what was written there.
//   Package ch4-sprite-registers-write-only-agnus, 34 claim records, all
//   citing this one proposition repeated per register (plus, for SPR0PTH
//   and SPR0PTL only, the generic "SPRxPTH"/"SPRxPTL" family entry on
//   p.285, which names the same two addresses).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition. p.285
//   (Appendix A, the generic SPRxPTH/SPRxPTL family row) and p.291
//   (Appendix B, the sixteen per-sprite rows SPR0PTH..SPR7PTL). A red
//   result below means agnus.v disagrees with the manual, not that the
//   assertion is wrong - no assertion here is, or will be, adjusted to
//   make the RTL pass.
//
// THE OCR DAMAGE ON p.291 (read, not solved, here): all sixteen p.291 rows
//   this package cites carry a lower-case `w` in the access column where
//   the printed page sets upper-case `W` - one instance of a
//   corpus-wide, already-documented defect (hrm/appB/ocr-repairs.md
//   records exactly 21 lower-case-`w` rows on p.291; every one of this
//   package's sixteen rows is inside that count). This is not a defect in
//   the claims: each one quotes the damaged text verbatim (`quote_verbatim:
//   yes`) and states the correction in its own restatement, which is the
//   corpus's documented reading rule (`ocr-repairs.md`, "the second
//   reading quoted the damage as it stands and put the correction in the
//   restatement"). The letter being read is `w` or `W` does not change
//   what is being asserted: p.287's legend defines only `W=write-only`,
//   and both readers' restatements settle on that meaning for every row
//   cited here. Nothing about the OCR damage changes any assertion below.
//
// A MISSING COLUMN IS NOT A STATEMENT: every row this package cites - the
//   p.285 generic entry and all sixteen p.291 per-sprite rows - carries a
//   non-empty chip column (`A`, Agnus) and a non-empty access column (`W`/
//   `w`, corrected to `W`). Nothing here is asserted from a blank cell.
//
// Non-goal: the pointer registers' bit WIDTH, byte-vs-word addressing, and
//   the high/low split composing a chip-memory address are
//   hrm/ch4-sprite-pointer-registers' job (fixture agnus_spritedma,
//   already built) and are not reproduced here - this package's 34 claims
//   are exclusively about write-only-ness, never about what the written
//   bits do once stored.
//
// BOUNDARY: `agnus`, the Agnus top-level module - not agnus_spritedma.
//   Every one of this package's 34 claims carries the identical
//   classify.observable field: "needs a wider boundary: agnus --
//   agnus_spritedma has no data_out port (rtl/agnus_spritedma.v:86-102);
//   the read path is agnus.v:277/281" and the identical classify.evidence:
//   "port list rtl/agnus_spritedma.v:86-102 -- the module's outputs are
//   reqdma, reg_address_out and address_out; there is no data_out.
//   rtl/agnus.v:277 sources data_out from data_bmc|dmaconr|data_blt only,
//   so no sprite pointer address is ever read back". Write-only-ness is a
//   fact about what reaches the CPU-visible data bus, and agnus_spritedma
//   alone has no such bus to observe - only the top-level module carries
//   the mux that (per this evidence) never includes a sprite-pointer term.
//   This matches the build brief's own instruction: hrm/agnus_top_dmacon
//   and hrm/ch4-sprite-dma-control (committed tonight, read in full before
//   writing this file) both already build exactly this fixture in this
//   same rtl/ tree.
//
// HOW TO MEASURE IT (read from hrm/ch4-sprite-dma-control, built tonight,
//   before writing this file, per the build brief's own instruction): a
//   read that returns something other than the pattern just written proves
//   nothing on its own - it could equally be a harness whose read path
//   never samples anything live. That package's own DMACON-IS-WRITE-ONLY-
//   AT-096 assertion pairs the suspect read (at DMACON's own address) with
//   a control read, in the SAME run, at DMACONR - a genuinely readable
//   register that reflects the very same write. This package's claims
//   name no readable twin of their own (classify.twin says so explicitly:
//   "none in this lot" for the p.285 pair; the p.291 pairs' twin field
//   only cross-references the OTHER reader's record of the same sentence,
//   not a readable register), so the paired control used below is the
//   same DMACON/DMACONR pair hrm/ch4-sprite-dma-control already
//   established on this identical fixture: each write-only check below
//   writes a known pattern to the sprite address under test AND writes a
//   known pattern to DMACON, then reads both addresses back in the same
//   run. DMACONR reflecting its pattern is what proves the read path is
//   live; the sprite address's read then not reflecting ITS pattern is
//   what the proposition claims.
//
// PORTS (from every claim's own classify.evidence field, and from
//   hrm/ch4-sprite-dma-control's and hrm/agnus_top_dmacon's already-
//   committed Harness for this identical fixture - NOT discovered by this
//   package opening agnus.v, which has not happened yet):
//   in  - clk, clk7_en, cck, reset, aen, rd, hwr, lwr, address_in, data_in,
//         audio_dmal, audio_dmas, disk_dmal, disk_dmas, bls, ntsc, a1k,
//         ecs, aga, floppy_speed
//   out - data_out (the read-back bus every write-only check samples;
//         agnus.v:277 sources it from data_bmc|dmaconr|data_blt only, per
//         every claim's own evidence), dbr (bus-busy/arbiter-grant signal
//         the write_reg/read_reg retry loop polls, same as the sibling
//         packages' Harness)
//   Register-bus addressing: a 9-bit word address (byte address >> 1), the
//   same convention DMACON=$096/DMACONR=$002 use in
//   hrm/ch4-sprite-dma-control.
//
// ============================================================================
// CLAIMS (verbatim quote + restatement, from
// work/ch4_packages/ch4-sprite-registers-write-only-agnus.jsonl, package
// "ch4-sprite-registers-write-only-agnus", all 34 records, classify.status
// == "testable"). Grouped by register address; the p.291 rows were each
// read independently by two readers (opus, sol) and both ids are cited
// together wherever they name the same sentence - classify.twin on each
// pair records "both records of this sentence classify identically, so
// the sentence carries no reader disagreement".
//
//   $120 SPR0PTH (p.285 generic "SPRxPTH 120 W A Sprite x pointer (high 3
//     bits)" + p.291 "SPR0PTH + 120 w A Sprite 0 pointer (high 3 bits)"):
//     CH4-appA-sprite.c42 (sol, p.285) -> "SPRxPTH is write-only."
//     APPB2-SPR0PTH.acc (opus, p.291) -> "SPR0PTH is write-only. [...]"
//     CH4-APPB-SPR0PTH.access (sol, p.291) -> "The access field reads
//       lowercase 'w'. [...] this visibly damaged cell is corrected here
//       to 'W', meaning write-only."
//   $122 SPR0PTL (p.285 generic "SPRxPTL 122 W A Sprite x pointer (low 15
//     bits)" + p.291 "SPR0PTL + 122 w A Sprite 0 pointer (low 15 bits)"):
//     CH4-appA-sprite.c46 (sol, p.285), APPB2-SPR0PTL.acc (opus, p.291),
//     CH4-APPB-SPR0PTL.access (sol, p.291).
//   $124 SPR1PTH (p.291 only): APPB2-SPR1PTH.acc, CH4-APPB-SPR1PTH.access.
//   $126 SPR1PTL (p.291 only): APPB2-SPR1PTL.acc, CH4-APPB-SPR1PTL.access.
//   $128 SPR2PTH (p.291 only): APPB2-SPR2PTH.acc, CH4-APPB-SPR2PTH.access.
//   $12A SPR2PTL (p.291 only): APPB2-SPR2PTL.acc, CH4-APPB-SPR2PTL.access.
//   $12C SPR3PTH (p.291 only): APPB2-SPR3PTH.acc, CH4-APPB-SPR3PTH.access.
//   $12E SPR3PTL (p.291 only): APPB2-SPR3PTL.acc, CH4-APPB-SPR3PTL.access.
//   $130 SPR4PTH (p.291 only): APPB2-SPR4PTH.acc, CH4-APPB-SPR4PTH.access.
//   $132 SPR4PTL (p.291 only): APPB2-SPR4PTL.acc, CH4-APPB-SPR4PTL.access.
//   $134 SPR5PTH (p.291 only): APPB2-SPR5PTH.acc, CH4-APPB-SPR5PTH.access.
//   $136 SPR5PTL (p.291 only): APPB2-SPR5PTL.acc, CH4-APPB-SPR5PTL.access.
//   $138 SPR6PTH (p.291 only): APPB2-SPR6PTH.acc, CH4-APPB-SPR6PTH.access.
//   $13A SPR6PTL (p.291 only): APPB2-SPR6PTL.acc, CH4-APPB-SPR6PTL.access.
//   $13C SPR7PTH (p.291 only): APPB2-SPR7PTH.acc, CH4-APPB-SPR7PTH.access.
//   $13E SPR7PTL (p.291 only): APPB2-SPR7PTL.acc, CH4-APPB-SPR7PTL.access.
//
// That is all 34 records: 2 on p.285 (SPR0PTH/SPR0PTL's generic family
// entry only - the family entry names one representative address, the
// same $120/$122 p.291 already gives sprite 0, so it is cited alongside
// sprite 0's own rows rather than as a seventeenth register) + 16 opus
// p.291 records + 16 sol p.291 records = 34. Every id above is cited on
// exactly one planned assertion below; none are left uncovered.
//
// COVERAGE ALREADY MEASURED ELSEWHERE: none. hrm/ch4-sprite-pointer-
// registers builds agnus_spritedma alone, which (per every claim's own
// evidence, quoted above) has no data_out port at all and so cannot
// observe write-only-ness - it measures the composed address, not the
// register bus. No other package in hrm/ builds `agnus` and reads these
// sixteen addresses. All 34 ids are this package's own job.
//
// ============================================================================
// PLANNED ASSERTIONS (names, cited-id groups and intent only - no RTL line
// numbers beyond what the claims' own classify.evidence already handed
// over; no fixture code below this comment block, matching hrm/ch4-
// sprite-dma-control's own spec-before-RTL gate. Implementation added in
// the next commit, after agnus.v is opened to confirm the write_reg/
// read_reg pattern copied from hrm/ch4-sprite-dma-control still applies
// unchanged):
//
//   0. FIXTURE-DELIVERS-KNOWN-WORD-VIA-DMACONR: this package's own control
//      case, expected to PASS, and its fixture-delivers-a-known-word
//      proof, run fresh on this package's own Harness instance (the fix
//      does not travel with a copied file - see hrm/ch4-sprite-dma-
//      control's own IMPLEMENTATION NOTES on exactly this point). Three
//      different DMACON patterns are written in sequence and DMACONR is
//      read back after EACH one, confirming each arrives distinct and in
//      order. DMACON/DMACONR are not this package's subject - no claim id
//      is cited - this assertion exists purely to show the harness's read
//      path is alive before any write-only check below leans on that read
//      path returning "not what was written" as evidence of anything.
//
//   1-16. <REGISTER>-IS-WRITE-ONLY-AT-<ADDR>, one per address in the table
//      above (SPR0PTH-IS-WRITE-ONLY-AT-120 through SPR7PTL-IS-WRITE-ONLY-
//      AT-13E): write a distinct non-zero pattern to the sprite address
//      under test; in the SAME run, write a distinct non-zero pattern to
//      DMACON; read back DMACONR (expected to reflect its own pattern -
//      the paired control, on the same measurement basis, proving the
//      read path is live) and read back the sprite address under test
//      itself (expected NOT to reflect the pattern written to it - the
//      proposition under test). Each assertion cites the claim id group
//      for its own address from the table above.
//
// Seventeen planned assertions (one control + sixteen write-only checks).
// Together they cite all 34 ids in ch4-sprite-registers-write-only-
// agnus.jsonl.
//
// No RTL has been opened yet to write this comment block: the port names
// (listed above) and the register-bus mechanics (word-addressed CPU bus,
// arbitrated by `dbr`, an unselected address reading back 0 through an
// OR-mux) come from this package's own claims' classify.evidence fields
// and from hrm/ch4-sprite-dma-control's and hrm/agnus_top_dmacon's
// already-committed files (both handed to or available to the building
// agent as inventory, not discovered by this agent opening agnus.v). The
// sixteen register addresses come directly from each claim's own quoted
// text, not from the RTL. Exact confirmation that the write_reg/read_reg
// retry shape still applies unchanged against this fixture, and the
// mutation log, are answered in the next commit, after agnus.v is read.
// ============================================================================
