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

## A5 — DSKBYT-BIT15-IS-NOT-SET-ONLY-ON-BYTE-RECEIPT

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
- Proposition: bit 15 (DSKBYT) is set true specifically as each byte
  arrives, and indicates a valid byte is present — it should therefore
  read 0 at any time no byte has ever been received (a freshly-reset
  module, no transfer ever armed), and only become 1 once a byte has
  actually arrived.
- Expected: RED (per the build brief's own disclosed background,
  candidate C6: bit 15 is a constant 1, unconditionally, including with
  zero bytes ever received).

## A6 — DSKBYT-BIT15-IS-NOT-CLEARED-ON-READ

- Page: 248, 278. Uids: `opus:8-dskbytr.c4`, `opus:8-dskbytr.c7`,
  `sol:8-dskbytr.c8`, `opus:8-appA-dskbytr.c9`, `sol:8-appa-dskbytr.c10`.
- Quotes: p.248 "DSKBYT is cleared when the DSKBYTR register is read.";
  p.248 "(reset by reading this register)" (reset clause of the same table
  row A5 cites for its other half); p.278 "15 DSKBYT Disk byte ready
  (reset on read)" (reset clause).
- Proposition: reading DSKBYTR clears bit 15 — a second, immediately
  consecutive read must show it cleared relative to the first.
- Expected: RED (per the build brief's own disclosed background: two
  successive reads both return the same constant 0x9000, so bit 15 does
  not clear).

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
- Proposition: bit 12 (WORDEQUAL) should read differently depending on
  whether the disk input stream currently matches the programmed DSKSYNC
  value — 0 when it does not, transiently 1 only while it does. This is
  the printed "sector-searching facility": software polls this bit (or the
  interrupt it can raise) to find a sector without buffering a whole
  track.
- Expected: RED (per the build brief's own disclosed background,
  candidate C6: bit 12 is a constant 1, including with no drive selected
  and no transfer running at all — it cannot be distinguishing a match
  from a non-match if it never varies).

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
  time, as it is received — feeding two distinct, known byte streams
  through the module should produce two distinct DATA readings that
  correspond to what was fed.
- Expected: RED (per the build brief's own disclosed background,
  candidate C6: bits 7-0 are a constant 0x00 idle; the claims' own
  evidence records this as a real divergence, not an absent one — the
  byte path exists in NanoMig but was rerouted through a FIFO at a
  different address rather than left wired to this field).

## A10 — DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-TRANSFER-DIRECTION

- Page: 248. Uids: `opus:8-dskbytr.c5`, `sol:8-dskbytr.c5`,
  `sol:8-dskbytr.c6`.
- Quote: "DSKBYTR may be used to synchronize the processor to the disk
  rotation before issuing a read or write under DMA control."
- Proposition, per each claim's own `split` note (the advice half —
  "may be used to synchronize" — is the page's recommendation to
  programmers; the hardware proposition under it is what is graded):
  DSKBYTR's own state should track the incoming byte stream and hence the
  passage of disk rotation, both before a read-direction DMA transfer is
  armed and before a write-direction one is. Sampling it at different
  points relative to rotation, and under both a read-armed and a
  write-armed DSKLEN state, should show it varying.
- Expected: RED (this reduces to the same underlying fact A5, A7 and A9
  already establish — a register that is a hardwired constant cannot vary
  with rotation or with transfer direction either; this assertion measures
  that reduction directly rather than assuming it).

## Claims not turned into a new assertion

| uid(s) | page | proposition | reason |
|---|---|---|---|
| `opus:8-dskbytr.c8` (DMACON clause), `sol:8-dskbytr.c9` (DMACON clause), `opus:8-appA-dskbytr.c10` (DMACON clause) | 248, 278 | DMAON also requires DMACON's own DSKEN and/or DMAEN bits | This package's assigned fixture is `paula_floppy` compiled alone, which has no DMACON port — DMACON reaches this module only pre-combined into a single `enable` input at a wider boundary (`paula`). Grading DMACON's specific bit combination needs that wider fixture, already measured by candidate C11 in the finding-candidates document, and is not something this package's boundary can exercise. Not a red against this RTL — a scope decision, and a boundary this package was not assigned. |

## Totals

- 48 claims read, all 48 accounted for above.
- 10 new graded assertions (A2-A10, plus A1 the mandatory
  fixture-liveness precondition): A1, A2, A3, A4 and A8 are not pre-judged
  by the source claims and are expected to measure PASS — A2/A3/A4 in
  particular share the same register and the same read mechanism as every
  suspect assertion, so they are this package's control cases. A5, A6, A7,
  A9 and A10 are expected RED, per the build brief's own disclosed
  background (candidate C6) — that background is disclosed above, not
  derived from any RTL reading done for this specification.
- 1 claim group (3 uids, the DMACON clause shared across two printed
  quotes) declined as out of this package's assigned boundary, not as
  absent or as a red.

Nothing in this file required opening the RTL, the port list, or any file
under `nanomig-hrm-tests/nanomig/` to write. No HALT condition was
encountered.
