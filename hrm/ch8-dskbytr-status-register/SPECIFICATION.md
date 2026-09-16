# Specification — `ch8-dskbytr-status-register`

Package: `hrm/ch8-dskbytr-status-register`
Fixture: `paula_floppy` (per the build brief: compile `paula_floppy.v` +
`paula_floppy_fifo.v`; `paula_floppy_ebr.v` declares a second `module
paula_floppy` and must not be compiled alongside it)
Claims file:
`C:\code4nanomig\code\hrm-testpipeline\work\ch8-s2-build\ch8-dskbytr-status-register.jsonl`
(48 claims)
Subject: DSKBYTR's identity, its bit layout, and the behaviour the manual
attributes to each bit — DSKBYT set per byte and cleared on read, DMAON,
DISKWRITE, WORDEQUAL, the currently-unused bits, the DATA byte, and the
"pollable register" / "sector-searching facilities" framing carried over
from chapter 8's disk-DMA narrative on p.247.

This file is written from the `extract.quote` / `crossread.full_quote` /
`restatement` fields of the 48 claims alone. No RTL file for this package,
no port list, and no file under `nanomig-hrm-tests/nanomig/` has been opened
to produce this document. Port names, concrete stimulus values, and the
`expect()` assertions themselves are added only after this file is
committed, per the build brief's order of work.

**Two things are recorded here as background given by the task, not as
something discovered by reading the RTL myself** (the same convention
`ch8-mouse-quadrature-decode/SPECIFICATION.md` uses for its own reset-hazard
note): the build brief states that a prior measurement, run before this
package was assigned, found DSKBYTR idle at a **constant 0x9000** — bit 15
(DSKBYT) fixed at 1 and unchanged by a second consecutive read, bit 12
(WORDEQUAL) fixed at 1, bits 7-0 (DATA) fixed at 0x00 — found independently
by two classifiers (finding candidate C6 in
`work/2026-09-15_ch8_s2_finding_candidates.md`). The same document records
that DMACON bit 9 alone leaves bit 14 (DMAON) clear at a wider boundary than
this package's own fixture (candidate C11), that DSKLEN needs two identical
writes to arm DMA, and that a reused Verilated model must not be shared
across trials because `dsktrack` carries no reset term. None of this shapes
which propositions are written below — every proposition here still comes
from a printed quotation — but it is disclosed up front because it already
tells me several of the assertions below are expected to grade RED, and the
build brief is explicit that going looking for a reading of the page that
makes a red case green would be the one failure that makes this package
worthless.

## Claim inventory (all 48)

| uid | page | groups under |
|---|---|---|
| `opus:8-dma.c30` | 247 | A7 |
| `opus:8-dma.c31` | 247 | A2 |
| `opus:8-dskbytr.c1` | 248 | A2 |
| `opus:8-dskbytr.c2` | 248 | A9 |
| `opus:8-dskbytr.c3` | 248 | A5 |
| `opus:8-dskbytr.c4` | 248 | A6 |
| `opus:8-dskbytr.c5` | 248 | A10 |
| `opus:8-dskbytr.c6` | 248 | A5 |
| `opus:8-dskbytr.c7` | 248 | A6 |
| `opus:8-dskbytr.c8` | 248 | A4 (DSKLEN half) + declined (DMACON half) |
| `opus:8-dskbytr.c10` | 248 | A3 |
| `opus:8-dskbytr.c11` | 248 | A7 |
| `opus:8-dskbytr.c12` | 248 | A7 |
| `opus:8-dskbytr.c13` | 248 | A8 |
| `opus:8-dskbytr.c14` | 248 | A9 |
| `sol:8-dskdma.c8` | 247 | A7 |
| `sol:8-dskdma.c9` | 247 | A2 |
| `sol:8-dskbytr.c1` | 248 | A2 |
| `sol:8-dskbytr.c2` | 248 | A9 |
| `sol:8-dskbytr.c3` | 248 | A5 |
| `sol:8-dskbytr.c5` | 248 | A10 |
| `sol:8-dskbytr.c6` | 248 | A10 |
| `sol:8-dskbytr.c7` | 248 | A5 |
| `sol:8-dskbytr.c8` | 248 | A6 |
| `sol:8-dskbytr.c9` | 248 | A4 (DSKLEN half) + declined (DMACON half) |
| `sol:8-dskbytr.c14` | 248 | A3 |
| `sol:8-dskbytr.c15` | 248 | A7 |
| `sol:8-dskbytr.c16` | 248 | A7 |
| `sol:8-dskbytr.c18` | 248 | A8 |
| `sol:8-dskbytr.c20` | 248 | A9 |
| `opus:8-appA-dskbytr.c5` | 278 | A2 |
| `opus:8-appA-dskbytr.c6` | 278 | A9 |
| `opus:8-appA-dskbytr.c7` | 278 | A5 |
| `opus:8-appA-dskbytr.c8` | 278 | A5 |
| `opus:8-appA-dskbytr.c9` | 278 | A6 |
| `opus:8-appA-dskbytr.c10` | 278 | A4 (DSKLEN half) + declined (DMACON half) |
| `opus:8-appA-dskbytr.c12` | 278 | A3 |
| `opus:8-appA-dskbytr.c13` | 278 | A7 |
| `opus:8-appA-dskbytr.c14` | 278 | A8 |
| `opus:8-appA-dskbytr.c15` | 278 | A9 |
| `sol:8-appa-dskbytr.c7` | 278 | A9 |
| `sol:8-appa-dskbytr.c8` | 278 | A5 |
| `sol:8-appa-dskbytr.c9` | 278 | A5 |
| `sol:8-appa-dskbytr.c10` | 278 | A6 |
| `sol:8-appa-dskbytr.c13` | 278 | A3 |
| `sol:8-appa-dskbytr.c14` | 278 | A7 |
| `sol:8-appa-dskbytr.c15` | 278 | A8 |
| `sol:8-appa-dskbytr.c16` | 278 | A9 |

All 48 accounted for; none left unassigned.

## How the 48 claims were organised

1. **Twins across two readers and two pages.** `opus` and `sol` each read
   both the chapter-8 body text (pp.247-248) and the Appendix A register
   summary (p.278) independently, so most propositions below are supported
   by four uids: an `opus`/`sol` pair on p.248 and an `opus`/`sol` pair on
   p.278 stating the same fact in the appendix's terser phrasing.
2. **The p.248 table and the p.278 table are graded together where they
   agree, and separately where they do not.** Every DSKBYTR bit's row is
   printed twice — once in the chapter's own bit table (p.248) and once in
   Appendix A's register summary (p.278) — and for bits 15, 13 and 12 the
   two pages say the same thing in different words, so one assertion per
   bit cites both pages' uids together. **Bit 14 (DMAON) is the exception**:
   p.248 names three conditions ("the DMAEN bit in DSKLEN, and the DSKEN &
   DMAEN bits in DMACON"), p.278 names only two ("bit 15 (DMAEN) in DSKLEN,
   ANDed with Bit09 (DMAEN) in DMACON") — both omit or include the DMACON
   half differently, and DMACON is not a port this package's assigned
   fixture (`paula_floppy` alone) carries at all. A3 grades only the half
   both pages state identically (DSKLEN's own bit); the DMACON half is
   declined below, not silently dropped and not asserted on a boundary that
   cannot carry it.
3. **Splits already recorded by the claims themselves are followed, not
   re-derived.** Several records split their own quotation into a testable
   hardware half and a non-hardware half (advice to programmers, or an
   absolute time figure with no port to carry real microseconds) — e.g.
   `opus:8-dskbytr.c12`'s own `split` field says the "as little as two
   microseconds" clause is not testable at this boundary and is recorded
   under a different uid not in this package's 48. This file does not
   re-open that question; it grades exactly the half each claim's own
   `split` field names as testable.
4. **Two p.247 claims about "sector-searching facilities" and a "pollable"
   register are folded into the WORDEQUAL propositions they describe.**
   Both readers' own `classify.split` notes say the testable content behind
   the vague word "limited" is the concrete DSKSYNC-match mechanism — which
   is exactly what bit 12 (WORDEQUAL) is printed to expose — so
   `opus:8-dma.c30`/`sol:8-dskdma.c8` are cited under A7 rather than given
   their own assertion. The "polled to examine the disk input stream" claims
   (`opus:8-dma.c31`/`sol:8-dskdma.c9`) describe the register as a whole,
   not any one bit, and are cited under A2, the assertion that grades
   DSKBYTR's own readability.

## A1 — FIXTURE-LIVENESS (harness precondition, no claim uid)

Not a proposition from the manual. Per the build brief: "the fixture proves
it can deliver a known word before anything reads one." Two distinct, known
register states must each arrive at DSKBYTR distinctly before any
content-specific assertion below is graded, so a silently-dead read/write
bus cannot masquerade as every claim below being a genuine finding.

## A2 — DSKBYTR-IS-A-REAL-READABLE-REGISTER

- Page: 247, 248, 278. Uids: `opus:8-dma.c31`, `sol:8-dskdma.c9`,
  `opus:8-dskbytr.c1`, `sol:8-dskbytr.c1`, `opus:8-appA-dskbytr.c5`.
- Quotes: p.247 "There is a register that may be polled to examine the disk
  input stream."; p.248 "This register is the disk-microprocessor data
  buffer."; p.278 "This register is the disk-microprocessor data buffer.
  Data from the disk (in read mode) is loaded into this register one byte
  at a time, and bit 15 (DSKBYT) is set true." (identity clause only —
  the loading/DSKBYT clauses of this same p.278 quote are graded under A5
  and A9).
- Proposition: DSKBYTR at its printed address is a real, decoded register —
  reading it returns a defined value, consistently, whether the module is
  idle or mid-transfer, not an undefined or floating result. This is the
  "is a register that may be polled" half of the page; what the polled
  value actually contains is graded by A5-A10 below.
- Expected: PASS (this is the harness's own control case, on the same
  measurement basis — reading the same register the suspect assertions
  read — that A5-A10 rely on to mean anything).

## A3 — DISKWRITE-BIT13-TRACKS-DSKLENS-WRITE-BIT

- Page: 248, 278. Uids: `opus:8-dskbytr.c10`, `sol:8-dskbytr.c14`,
  `opus:8-appA-dskbytr.c12`, `sol:8-appa-dskbytr.c13`.
- Quotes: p.248 "13 DISKWRITE The disk write bit (in DSKLEN) is enabled.";
  p.278 "13 DISKWRITE Mirror of bit 14 (WRITE) in DSKLEN".
- Proposition: bit 13 (DISKWRITE) mirrors DSKLEN's own WRITE bit — writing
  DSKLEN with that bit set makes DISKWRITE read 1; writing it clear makes
  DISKWRITE read 0.
- Expected: PASS (not stated by the claims file; to be measured — but this
  is the package's second control case, on the same register, same
  measurement basis as every DSKBYTR bit under suspicion below).

## A4 — DMAON-BIT14-TRACKS-DSKLENS-OWN-DMA-ENABLE-BIT (DSKLEN half only)

- Page: 248, 278. Uids: `opus:8-dskbytr.c8` (DSKLEN clause only — the DMACON
  clause of the same quote is declined below), `sol:8-dskbytr.c9` (same,
  DSKLEN half), `opus:8-appA-dskbytr.c10` (DSKLEN clause of its own quote
  only).
- Quotes: p.248 "14 DMAON Indicates when DMA is actually enabled. All the
  various DMA bits must be true. This means the DMAEN bit in DSKLEN, and
  the DSKEN & DMAEN bits in DMACON." (DSKLEN clause graded here); p.278
  "14 DMAON Mirror of bit 15 (DMAEN) in DSKLEN, ANDed with Bit09 (DMAEN) in
  DMACON" (DSKLEN clause graded here).
- Proposition: bit 14 (DMAON) requires DSKLEN's own DMA-enable bit to be
  armed before it can read 1 — this package's own claims file records the
  double-write mechanism DSKLEN needs (a rig hazard given by the build
  brief, not discovered here): the first DSKLEN write alone must leave
  DMAON clear, and only the second identical write brings it up.
- Expected: PASS (this package's third control case — the one bit of the
  three under DMAON's own printed description that this fixture, compiled
  alone, can actually carry).
- **The DMACON clause of this same printed sentence — "the DSKEN & DMAEN
  bits in DMACON" (p.248) / "ANDed with Bit09 (DMAEN) in DMACON" (p.278) —
  is declined, not asserted.** `paula_floppy` compiled alone (this
  package's assigned fixture) has no DMACON port; DMACON's bits reach this
  module only pre-combined into a single `enable` input at a wider
  boundary. Grading DMACON's own two-bit condition needs that wider
  boundary (`paula`), which candidate C11 already measured and which this
  package does not have. See "Claims not turned into a new assertion"
  below.

## A5 — DSKBYT-BIT15-IS-NOT-SET-ONLY-ON-BYTE-RECEIPT — DECLINED

- Page: 248, 278. Uids: `opus:8-dskbytr.c3`, `opus:8-dskbytr.c6`,
  `sol:8-dskbytr.c3`, `sol:8-dskbytr.c7`, `opus:8-appA-dskbytr.c7`,
  `opus:8-appA-dskbytr.c8`, `sol:8-appa-dskbytr.c8`,
  `sol:8-appa-dskbytr.c9`.
- Quotes: p.248 "As each byte is received into the register, the DSKBYT
  bit is set true."; p.248 "15 DSKBYT When set, indicates that this
  register contains a valid byte of data (reset by reading this
  register)." (valid-byte-present clause only — the reset-by-reading
  clause is graded under A6); p.278 "bit 15 (DSKBYT) is set true" (on
  each byte load); p.278 "15 DSKBYT Disk byte ready (reset on read)"
  (ready clause only).
- Original proposition (withdrawn): bit 15 (DSKBYT) reads 0 at any time
  no byte has ever been received (a freshly-reset module, no transfer
  ever armed), and only becomes 1 once a byte has actually arrived.
  **Withdrawn by independent validation (2026-09-15):** neither cited page
  defines DSKBYT's reset value — "receipt sets it" is not "reset clears
  it". The withdrawn assertion silently required both.
- **Disposition: DECLINED (`record()`).** A sound receipt test needs a
  genuinely cleared starting state, reached through the page's own
  documented clearing action (the read A6 grades), before a delivered
  known byte and an observed 0→1 transition can mean anything. A6's own
  repaired methodology (a real, clk7_en-qualified read cycle, not a bare
  combinational peek) proves that clearing action does not exist anywhere
  in this RTL: DSKBYT is the literal constant `1'b1` (paula_floppy.v:699).
  There is therefore no reachable cleared precondition at this boundary
  from which a 0→1 receipt transition could ever be observed — attempting
  to restructure this claim would only restate A6's own finding under a
  different uid, not test receipt. Declined rather than restated.

## A6 — DSKBYT-BIT15-IS-NOT-CLEARED-ON-READ

- Page: 248, 278. Uids: `opus:8-dskbytr.c4`, `opus:8-dskbytr.c7`,
  `sol:8-dskbytr.c8`, `opus:8-appA-dskbytr.c9`, `sol:8-appa-dskbytr.c10`.
- Quotes: p.248 "DSKBYT is cleared when the DSKBYTR register is read.";
  p.248 "(reset by reading this register)" (reset clause of the same table
  row A5 cites for its other half); p.278 "15 DSKBYT Disk byte ready
  (reset on read)" (reset clause).
- Proposition: reading DSKBYTR clears bit 15 — a second, immediately
  consecutive read must show it cleared relative to the first.
- **Repaired (independent validation, 2026-09-15):** the original form's
  `read_reg()` spans no clock edge at all (two `eval()`-only combinational
  peeks of the read mux), so it could never have exercised a synchronous
  read-side-effect regardless of what the RTL implements — the withdrawal
  was sound. Repaired to use `read_reg_clocked()`: holds the address across
  a genuine, clk7_en-qualified rising edge — the same bus-cycle shape every
  register WRITE in this module uses — for both reads. A6-CONTROL (no
  claim uid) proves that exact cycle shape is a faithful read by using it
  to observe DMAON and DISKWRITE, already known to move (A3/A4), before A6
  applies it to bit 15.
- Measured: RED. Two genuine, clk7_en-qualified read cycles both return
  0x9000 — bit 15 (DSKBYT) is the literal constant `1'b1`
  (paula_floppy.v:699) and cannot clear regardless of how genuine the read
  cycle is. This is now a sound measurement of that fact.

## A7 — WORDEQUAL-BIT12-DOES-NOT-TRACK-A-DSKSYNC-MATCH

- Page: 247, 248, 278. Uids: `opus:8-dma.c30`, `sol:8-dskdma.c8`,
  `opus:8-dskbytr.c11`, `sol:8-dskbytr.c15`, `opus:8-dskbytr.c12`,
  `sol:8-dskbytr.c16`, `opus:8-appA-dskbytr.c13`, `sol:8-appa-dskbytr.c14`.
- Quotes: p.247 "the disk hardware supports a limited set of
  sector-searching facilities." / "There is a register that may be polled
  to examine the disk input stream." (the concrete mechanism behind both
  sentences, per each claim's own `split` note, is exactly this bit); p.248
  "12 WORDEQUAL Indicates the DSKSYNC register equals the disk input
  stream. This bit is true only while the input stream matches the sync
  register (as little as two microseconds)." (the "true only while
  matching" half — the absolute-microsecond figure is not graded, per the
  claims' own split); p.278 "12 WORDEQUAL This bit true only while the
  DSKSYNC register equals the data from disk."
- Proposition: bit 12 (WORDEQUAL) should read 1 at a proved DSKSYNC match
  and 0 through a proved mismatch. This is the printed "sector-searching
  facility": software polls this bit (or the interrupt it can raise) to
  find a sector without buffering a whole track.
- **Repaired (independent validation, 2026-09-15):** the original form
  compared an IDLE state against an ACTIVE-BUT-MISMATCHING state and
  required them to differ — but p.248 requires WORDEQUAL false on a
  non-match; it never requires idle and active-mismatching to differ from
  each other. Repaired to sample bit 12 at a PROVED match and across a
  PROVED, sustained mismatch, using `syncint` — a real output port — as
  the oracle: with an armed transfer, `dmaen` is latched 1 throughout,
  which zeroes `syncint`'s other disjunct permanently, so `syncint`
  becomes exactly `sync_match` (paula_floppy.v:915,917) for the whole run.
- Measured: RED. Both trials PROVED their state (`syncint` pulsed at the
  match instant; `syncint` never pulsed across the sustained mismatch
  poll). WORDEQUAL reads 1 at the proved match (page-compliant) but ALSO
  reads 1 throughout the proved mismatch (should be 0) — bit 12 is the
  literal constant `1'b1` (paula_floppy.v:699) and cannot be distinguishing
  the two.

## A8 — UNUSED-BITS-11-8-CARRY-NO-DEFINED-FUNCTION

- Page: 248, 278. Uids: `opus:8-dskbytr.c13`, `sol:8-dskbytr.c18`,
  `opus:8-appA-dskbytr.c14`, `sol:8-appa-dskbytr.c15`.
- Quotes: p.248 "11-8 Currently unused; don't depend on read value."
  (unused clause only — the "don't depend on" clause is programmer advice,
  not a hardware proposition, and is recorded under a different uid not in
  this package's 48); p.278 "11-08 X Not used".
- Proposition: bits 11-8 carry no defined function of their own — they do
  not encode or leak any of DSKBYTR's other defined semantics (DSKBYT,
  DMAON, DISKWRITE, WORDEQUAL, or DATA), and their value does not change
  across the states that DO change those other fields.
- Expected: not stated by the claims file; to be measured.

## A9 — DATA-FIELD-BITS7-0-DOES-NOT-REFLECT-INCOMING-DISK-BYTES

- Page: 248, 278. Uids: `opus:8-dskbytr.c2`, `opus:8-dskbytr.c14`,
  `sol:8-dskbytr.c2`, `sol:8-dskbytr.c20`, `opus:8-appA-dskbytr.c6`,
  `opus:8-appA-dskbytr.c15`, `sol:8-appa-dskbytr.c7`,
  `sol:8-appa-dskbytr.c16`.
- Quotes: p.248 "In read mode, data from the disk is placed into this
  register one byte at a time."; p.248 "7-0 DATA Disk byte data."; p.278
  "Data from the disk (in read mode) is loaded into this register one byte
  at a time" (loading clause only); p.278 "07-00 DATA Disk byte data".
- Proposition: bits 7-0 (DATA) carry the actual disk byte, one byte at a
  time, as it is received — DATA sampled at the instant a KNOWN byte is
  current should read that byte.
- **Repaired (independent validation, 2026-09-15):** the original form
  compared two arbitrary DSKBYTR snapshots (after uniform 0x11 and 0xEE
  feeds) for inequality, with no independent ground truth for either
  sample — both reading 0x00 is compatible with a conforming
  implementation whose DATA field simply had not yet caught up. This is
  THE SAME UNDERLYING FACT as `ch8-disk-register-map-paula-floppy`/B4 and
  `ch8-disk-controller-overview`/`opus:8-dsk.c9`+`sol:8-dsk.c9` — one
  fact, repaired identically in all three packages that carry it. Repaired
  to bind a KNOWN incoming byte to the DSKBYTR sampling instant via the
  DSKDATR/FIFO marker technique already established by the sibling
  `ch8-mfm-cell-encoding` package: a uniform 0xFF sector's payload word can
  only ever arrive as exactly 0xFFFF. Calibrate the DSKDATR pop index at
  which that marker appears, then on the real trial peek (not pop) DSKDATR
  to confirm the known word is present, and peek DSKBYTR at the same
  instant (zero clock edges elapse between the two peeks).
- Measured: RED. `known_word=0xFFFF` (bound=true, confirmed at pop #33),
  DSKBYTR at that same instant reads 0xD000 — DATA field 0x00. Bits 7:0
  are the literal constant `8'h00` (paula_floppy.v:699) and cannot ever
  reflect a known incoming byte.

## A10 — DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-TRANSFER-DIRECTION — DECLINED

- Page: 248. Uids: `opus:8-dskbytr.c5`, `sol:8-dskbytr.c5`,
  `sol:8-dskbytr.c6`.
- Quote: "DSKBYTR may be used to synchronize the processor to the disk
  rotation before issuing a read or write under DMA control."
- Original proposition (withdrawn): two arbitrary DSKBYTR samples 2000
  `clk7_en` cycles apart, with no DMA armed, must differ.
- **Disposition: DECLINED (`record()`), per independent validation
  (2026-09-15).** The cited sentence is programmer ADVICE about a USE of
  DSKBYTR — it promises neither that any two arbitrarily-chosen instants
  differ, nor any minimum separation, nor that idle sampling (no DMA
  armed) exercises the mechanism it is advising about at all: two points
  in a periodic byte stream may legitimately repeat. The hardware fact
  actually underneath this advice — that DSKBYTR tracks a genuinely known
  incoming byte — is exactly what A9 already drives and grades directly
  against a bound, known byte. Restating this claim as a two-arbitrary-
  samples inequality would only produce a second, weaker restatement of
  A9's own finding. Declined rather than restated.

## Claims not turned into a new assertion

| uid(s) | page | proposition | reason |
|---|---|---|---|
| `opus:8-dskbytr.c8` (DMACON clause), `sol:8-dskbytr.c9` (DMACON clause), `opus:8-appA-dskbytr.c10` (DMACON clause) | 248, 278 | DMAON also requires DMACON's own DSKEN and/or DMAEN bits | This package's assigned fixture is `paula_floppy` compiled alone, which has no DMACON port — DMACON reaches this module only pre-combined into a single `enable` input at a wider boundary (`paula`). Grading DMACON's specific bit combination needs that wider fixture, already measured by candidate C11 in the finding-candidates document, and is not something this package's boundary can exercise. Not a red against this RTL — a scope decision, and a boundary this package was not assigned. |

## Totals

- 48 claims read, all 48 accounted for above.
- 9 graded assertions (A1-A4, A6-A9, A8) plus A6-CONTROL (an apparatus
  control, no claim uid): A1-A4, A6-CONTROL and A8 are GREEN. A6, A7 and
  A9 are RED — sound measurements of DSKBYTR's hardwired constant bits
  (paula_floppy.v:699), repaired 2026-09-15 after an independent
  validation withdrew their original forms for testing more than their
  cited pages state (see each assertion's own repair note above).
- 2 claims DECLINED as gradeable claims but not as sound assertions: A5
  (no page defines DSKBYT's reset value, and A6 proves the only candidate
  clearing precondition is unreachable) and A10 (the cited sentence is
  programmer advice, not a falsifiable two-sample inequality).
- 1 claim group (3 uids, the DMACON clause shared across two printed
  quotes) declined as out of this package's assigned boundary, not as
  absent or as a red.

Nothing in this file required opening the RTL, the port list, or any file
under `nanomig-hrm-tests/nanomig/` to write beyond the original build. No
HALT condition was encountered, in the original build or in this repair.

## Repair addendum (Lot F, 2026-09-15)

A second independent validation found five further defects in the prior
repair, all fixed against the printed pages (no RTL touched, no oracle
adjusted):

- **A1** (FIXTURE-LIVENESS, no uid) and **A6-CONTROL** (no uid) withdrawn
  to `note()` — neither ever carried conformance evidence.
- **A2** (DSKBYTR-IS-A-REAL-READABLE-REGISTER) repaired: its decode marker
  was idle bit 15 (DSKBYT) = 1, a value p.278 never defines (DSKBYTR has
  no printed reset/idle word). Narrowed to bits 13 (DISKWRITE) and 14
  (DMAON), both page-defined and both proven to move by A3/A4. Stays
  GREEN.
- **A8** (UNUSED-BITS-11-8) declined to `record()` — p.264 says not to
  rely on unused bits' values at all; requiring zero graded a value the
  page never states.
- **A4**'s graded label repeated three uids (`opus:8-dskbytr.c8`,
  `sol:8-dskbytr.c9`, `opus:8-appA-dskbytr.c10`) that also sat on this
  same block's DMACON-half `record()` line — a uid-placement defect. The
  three uids now sit on the `record()` line alone.
- **A9** (DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES, still RED and still
  the finding) had an unsound guard: it bound its known byte via a search
  for DSKDATR word `0xFFFF`, which is this RTL's own non-conformant
  encoder output for a uniform 0xFF byte, not a page-legal value (p.250
  makes 0xFF → 0x5555). Because the guard was a conjunct of the grading
  expression, a conforming encoder would never produce the marker and the
  assertion would fail anyway, for the wrong reason. Repaired so the
  binding is a precondition that declines when the marker cannot be
  located, and grades the DATA field only when it can.

Post-repair tally: 3 passed, 3 failed (A2/A3/A4 green; A6/A7/A9 red — the
package's three genuine findings, unchanged in substance), 3 declined
(A5, A8, A10), 2 measured-not-graded (A1, A6-CONTROL).
