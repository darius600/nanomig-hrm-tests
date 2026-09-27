# Specification — `ch8-dskbytr-status-register`

Package: `hrm/ch8-dskbytr-status-register`
Fixture: `paula_floppy` (`paula_floppy.v` + `paula_floppy_fifo.v`;
`paula_floppy_ebr.v` declares a second `module paula_floppy` and must not
be compiled alongside it)
Claims file:
`hrm-testpipeline/work/ch8-s2-build/ch8-dskbytr-status-register.jsonl`
(48 claims)
Subject: DSKBYTR's identity, its bit layout, and the behaviour the manual
attributes to each bit — DSKBYT set per byte and cleared on read, DMAON,
DISKWRITE, WORDEQUAL, the currently-unused bits, the DATA byte, and the
"pollable register" / "sector-searching facilities" framing carried over
from chapter 8's disk-DMA narrative on p.247.

Every proposition below comes from a printed quotation (the
`extract.quote` / `crossread.full_quote` / `restatement` fields of the 48
claims). Finding candidate C6 in
`work/2026-09-15_ch8_s2_finding_candidates.md` records DSKBYTR idle at a
**constant 0x9000** — bit 15 (DSKBYT) fixed at 1 and unchanged by a second
consecutive read, bit 12 (WORDEQUAL) fixed at 1, bits 7-0 (DATA) fixed at
0x00 — found independently by two classifiers; candidate C11 records that
DMACON bit 9 alone leaves bit 14 (DMAON) clear at a wider boundary than this
package's own fixture. Rig hazards: DSKLEN needs two identical writes to
arm DMA, and a Verilated model must not be shared across trials because
`dsktrack` carries no reset term.

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
| `opus:8-dskbytr.c8` | 248 | recorded (DSKLEN half measured by A4; DMACON half declined) |
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
| `sol:8-dskbytr.c9` | 248 | recorded (DSKLEN half measured by A4; DMACON half declined) |
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
| `opus:8-appA-dskbytr.c10` | 278 | recorded (DSKLEN half measured by A4; DMACON half declined) |
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
   half differently, and DMACON is not a port this package's fixture
   (`paula_floppy` alone) carries at all. A4 grades only the half both
   pages state identically (DSKLEN's own bit); the DMACON half is declined
   below, not silently dropped and not asserted on a boundary that cannot
   carry it.
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

Not a proposition from the manual: the fixture proves it can deliver a
known word before anything reads one. Two distinct, known register states
must each arrive at DSKBYTR distinctly before any content-specific
assertion below is trusted, so a silently-dead read/write bus cannot
masquerade as every claim below being a genuine finding. A `note()`, not
graded: it carries no claim uid. Measured: 0xB000 after DSKLEN=0x4000,
0x9000 after DSKLEN=0x0000.

## A2 — DSKBYTR-IS-A-REAL-READABLE-REGISTER

- Page: 247, 248, 278. Uids: `opus:8-dma.c31`, `sol:8-dskdma.c9`,
  `opus:8-dskbytr.c1`, `sol:8-dskbytr.c1`, `opus:8-appA-dskbytr.c5`.
- Quotes: p.247 "There is a register that may be polled to examine the disk
  input stream."; p.248 "This register is the disk-microprocessor data
  buffer."; p.278 "This register is the disk-microprocessor data buffer.
  Data from the disk (in read mode) is loaded into this register one byte
  at a time, and bit 15 (DSKBYT) is set true." (identity clause only —
  the loading/DSKBYT clauses of this same p.278 quote are cited under A5
  and A9).
- Proposition: DSKBYTR at its printed address is a real, decoded register —
  reading it returns a value that differs correctly by state (DISKWRITE,
  bit 13, set after a WRITE-bit DSKLEN write; DMAON, bit 14, set
  mid-transfer), while an address this module does not decode returns
  0x0000. This is the "is a register that may be polled" half of the page;
  what the polled value actually contains is graded by A5-A10 below. The
  decode marker is bits 13 and 14, both page-defined and shown to move by
  A3/A4: p.278 gives DSKBYTR no reset or idle word at all, so idle bit 15
  (DSKBYT) = 1 cannot serve.
- Measured: GREEN (the harness's own control case, on the same measurement
  basis — reading the same register the suspect assertions read — that
  A5-A10 rely on to mean anything).

## A3 — DISKWRITE-BIT13-TRACKS-DSKLENS-WRITE-BIT

- Page: 248, 278. Uids: `opus:8-dskbytr.c10`, `sol:8-dskbytr.c14`,
  `opus:8-appA-dskbytr.c12`, `sol:8-appa-dskbytr.c13`.
- Quotes: p.248 "13 DISKWRITE The disk write bit (in DSKLEN) is enabled.";
  p.278 "13 DISKWRITE Mirror of bit 14 (WRITE) in DSKLEN".
- Proposition: bit 13 (DISKWRITE) mirrors DSKLEN's own WRITE bit — writing
  DSKLEN with that bit set makes DISKWRITE read 1; writing it clear makes
  DISKWRITE read 0.
- Measured: GREEN (the package's second control case, on the same
  register, same measurement basis as every DSKBYTR bit under suspicion
  below).

## A4 — DMAON-BIT14-TRACKS-DSKLENS-OWN-DMA-ENABLE-BIT (DSKLEN half only)

- Page: 248, 278. Uids: none on the graded line. `opus:8-dskbytr.c8`,
  `sol:8-dskbytr.c9` and `opus:8-appA-dskbytr.c10` each state the whole
  compound DSKLEN-and-DMACON condition, and the coverage ledger reads uids,
  not a "DSKLEN clause" qualifier beside them; so the three uids sit once,
  on the DMAON-DMACON-HALF `record()` line, which names this assertion as
  where their DSKLEN half is measured.
- Quotes: p.248 "14 DMAON Indicates when DMA is actually enabled. All the
  various DMA bits must be true. This means the DMAEN bit in DSKLEN, and
  the DSKEN & DMAEN bits in DMACON." (DSKLEN clause graded here); p.278
  "14 DMAON Mirror of bit 15 (DMAEN) in DSKLEN, ANDed with Bit09 (DMAEN) in
  DMACON" (DSKLEN clause graded here).
- Proposition: bit 14 (DMAON) requires DSKLEN's own DMA-enable bit to be
  armed before it can read 1 — with the double write DSKLEN needs
  (paula_floppy.v:735-743): the first DSKLEN write alone leaves DMAON
  clear, and only the second identical write brings it up.
- Measured: GREEN (this package's third control case — the one bit of the
  three under DMAON's own printed description that this fixture, compiled
  alone, can actually carry).
- **The DMACON clause of this same printed sentence — "the DSKEN & DMAEN
  bits in DMACON" (p.248) / "ANDed with Bit09 (DMAEN) in DMACON" (p.278) —
  is declined, not asserted.** `paula_floppy` compiled alone (this
  package's fixture) has no DMACON port; DMACON's bits reach this
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
- **Disposition: DECLINED (`record()`).** Neither cited page defines
  DSKBYT's reset value — "receipt sets it" is not "reset clears it", and
  grading "bit 15 reads 0 until a byte has arrived" from a freshly-reset
  module would silently require both. A sound receipt test needs a
  genuinely cleared starting state, reached through the page's own
  documented clearing action (the read A6 grades), before a delivered
  known byte and an observed 0→1 transition can mean anything. A6's
  methodology (a real, clk7_en-qualified read cycle, not a bare
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
- Method: both reads use `read_reg_clocked()`, which holds the address
  across a genuine, clk7_en-qualified rising edge — the same bus-cycle
  shape every register WRITE in this module uses. The plain `read_reg()`
  spans no clock edge at all (two `eval()`-only combinational peeks of the
  read mux), so it could never exercise a synchronous read-side-effect
  regardless of what the RTL implements. A6-CONTROL (no claim uid, a
  `note()`) uses that exact cycle shape to observe DMAON and DISKWRITE,
  already known to move (A3/A4), before A6 applies it to bit 15; since
  those two bits are combinational, it shows the read samples correctly,
  not that a real bus cycle reaches a synchronous side effect.
- Measured: RED. Two genuine, clk7_en-qualified read cycles both return
  0x9000 — bit 15 (DSKBYT) is the literal constant `1'b1`
  (paula_floppy.v:699) and cannot clear regardless of how genuine the read
  cycle is.

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
- Method: p.248 requires WORDEQUAL false on a non-match; it never requires
  an IDLE state and an ACTIVE-BUT-MISMATCHING state to differ from each
  other. So bit 12 is sampled at a PROVED match and across a PROVED,
  sustained mismatch, using `syncint` — a real output port — as the
  oracle: with an armed transfer, `dmaen` is latched 1 throughout,
  which zeroes `syncint`'s other disjunct permanently, so `syncint`
  becomes exactly `sync_match` (paula_floppy.v:915,917) for the whole run.
- Measured: RED. Both trials PROVED their state (`syncint` pulsed at the
  match instant; `syncint` never pulsed across the sustained mismatch
  poll). WORDEQUAL reads 1 at the proved match (page-compliant) but ALSO
  reads 1 throughout the proved mismatch (should be 0) — bit 12 is the
  literal constant `1'b1` (paula_floppy.v:699) and cannot be distinguishing
  the two.

## A8 — UNUSED-BITS-11-8-CARRY-NO-DEFINED-FUNCTION — DECLINED

- Page: 248, 278. Uids: `opus:8-dskbytr.c13`, `sol:8-dskbytr.c18`,
  `opus:8-appA-dskbytr.c14`, `sol:8-appa-dskbytr.c15`.
- Quotes: p.248 "11-8 Currently unused; don't depend on read value."
  (unused clause only — the "don't depend on" clause is programmer advice,
  not a hardware proposition, and is recorded under a different uid not in
  this package's 48); p.278 "11-08 X Not used".
- **Disposition: DECLINED (`record()`).** p.264 says outright: "do not
  rely on the values of unused bits when reading from a read only
  register." Any value on bits 11-8 is compatible with the page; requiring
  zero (or constancy across states) grades something the manual never
  states, and treating it as unfalsifiable because this RTL has a literal
  `4'b0000` there (paula_floppy.v:699) would make the implementation the
  oracle. The measured value — 0 idle, 0 after a DISKWRITE-set write, 0
  during an armed transfer — is printed via `note()`, not graded.

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
- Method: two arbitrary DSKBYTR snapshots (say after uniform 0x11 and 0xEE
  feeds) compared for inequality would have no independent ground truth
  for either sample — both reading 0x00 is compatible with a conforming
  implementation whose DATA field simply had not yet caught up. This is
  THE SAME UNDERLYING FACT as `ch8-disk-register-map-paula-floppy`/B4 and
  `ch8-disk-controller-overview`/`opus:8-dsk.c9`+`sol:8-dsk.c9` — one
  fact, graded against a known, bound byte in all three packages (those
  two locate it at a fixed payload-start index; this one calibrates the
  index by search). A KNOWN incoming byte is bound to the DSKBYTR sampling
  instant via the DSKDATR/FIFO marker technique `ch8-mfm-cell-encoding`
  also uses: this RTL delivers a uniform 0xFF sector's payload word as
  0xFFFF. Calibrate the DSKDATR pop index at which that marker appears,
  then on the real trial peek (not pop) DSKDATR to confirm the known word
  is present, and peek DSKBYTR at the same instant (zero clock edges
  elapse between the two peeks).
- The binding is a precondition, not a conjunct: 0xFFFF is this RTL's own
  non-conformant encoder output for a uniform 0xFF byte, not a page-legal
  value (p.250 makes 0xFF → 0x5555), so a conforming encoder would never
  produce the marker. When the marker cannot be located the assertion
  declines (`record()`); when it can, the DATA field is graded.
- Measured: RED. `known_word=0xFFFF` (bound=true, confirmed at pop #33),
  DSKBYTR at that same instant reads 0xD000 — DATA field 0x00. Bits 7:0
  are the literal constant `8'h00` (paula_floppy.v:699) and cannot ever
  reflect a known incoming byte.

## A10 — DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-TRANSFER-DIRECTION — DECLINED

- Page: 248. Uids: `opus:8-dskbytr.c5`, `sol:8-dskbytr.c5`,
  `sol:8-dskbytr.c6`.
- Quote: "DSKBYTR may be used to synchronize the processor to the disk
  rotation before issuing a read or write under DMA control."
- **Disposition: DECLINED (`record()`).** The cited sentence is programmer ADVICE about a USE of
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
| `opus:8-dskbytr.c8` (DMACON clause), `sol:8-dskbytr.c9` (DMACON clause), `opus:8-appA-dskbytr.c10` (DMACON clause) | 248, 278 | DMAON also requires DMACON's own DSKEN and/or DMAEN bits | This package's fixture is `paula_floppy` compiled alone, which has no DMACON port — DMACON reaches this module only pre-combined into a single `enable` input at a wider boundary (`paula`). Grading DMACON's specific bit combination needs that wider fixture, already measured by candidate C11 in the finding-candidates document, and is not something this package's boundary can exercise. Not a red against this RTL — a scope decision. The three uids sit on one `record()` line, which also names A4 as where their DSKLEN half is measured. |

## Totals

- 48 claims read, all 48 accounted for above.
- 6 graded assertions. A2, A3 and A4 are GREEN (the control cases). A6,
  A7 and A9 are RED — measurements of DSKBYTR's hardwired constant bits
  (paula_floppy.v:699).
- 3 propositions DECLINED via `record()`: A5 (no page defines DSKBYT's
  reset value, and A6 proves the only candidate clearing precondition is
  unreachable), A8 (p.264: any value on unused bits is page-compatible)
  and A10 (the cited sentence is programmer advice, not a falsifiable
  two-sample inequality).
- 1 claim group (3 uids, the DMACON clause shared across two printed
  quotes) recorded as outside this package's boundary, not as absent or as
  a red.
- A1 (fixture liveness) and A6-CONTROL (read-cycle liveness) carry no
  claim uid and are `note()`s, not graded.
- `make -B test`: 3 passed, 3 failed.
