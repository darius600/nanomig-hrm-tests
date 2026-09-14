# Specification: ch8-mouse-quadrature-decode

Fixture: `userio.v`, compiled alone. Subject: page 229's mouse/trackball
quadrature-input text, Figure 8-2 (printed p.230, "Mouse Quadrature" — the
Case 1/Case 2 waveforms and the V/VQ:D1/D0 truth table), Figure 8-3 (printed
p.233, "Joystick to Counter Connections" — only the parts of it that describe
how the quadrature pair for each axis reaches the Y/X counters, not the DE-9
connector or the register-address material it also draws), and the Appendix A
p.281 sentence on sampling and clocking the mouse counters.

Written from `ch8-mouse-quadrature-decode.jsonl` alone (61 claims). No RTL, no
port list, and no other file under `nanomig-hrm-tests/nanomig/` have been
opened to produce this document. The two figure images
(`docs/references/amiga-hrm-3rd/ch8/figures/p230.png` and `.../p233.png`) were
opened and read, as the task instructs, to judge the restatements that stand
in for a figure quotation — the images are printed pages of the manual, not
RTL, and reading them is exactly what "the quotations alone" requires when the
quotation is a figure marker.

**userio.v has never had a test package before this one.** There is no
sibling harness on this exact top module to reuse. The mandatory
fixture-liveness proof (A1 below) is written from scratch for that reason:
before any content-specific assertion is graded, the harness must show it can
deliver two *distinct, known* pin-transition sequences and see each arrive
independently at its own counter half.

**A recorded RTL observation, given as part of this task rather than
discovered by reading the RTL myself:** `userio.v` has no reset branch for its
port counters, so driving `reset` alone does not clear them. Nothing in this
specification assumes a reset-cleared starting count; every assertion below
that needs a known starting count must establish it by driving a deliberate,
counted sequence of transitions from whatever the counter's actual power-on
value turns out to be — a step-2/3 mechanism decision, not a specification
detail.

## How the 61 claims were organised

1. **Twins.** `opus` and `sol` independently read the same sentences, the same
   figure, and the same table rows, and produced separate claim records for
   the same proposition (e.g. `opus:8-mouse.c1` and `sol:8-mouse.c1`+
   `sol:8-mouse.c2` both state that mouse pulses become separate horizontal
   and vertical counts — `sol` split the one `opus` sentence into two,
   one per axis). Each assertion below cites every uid that supports it.
2. **Splits.** Several claim records bundle more than one printed fact in one
   quotation, or flag part of their own restatement as not measurable at this
   module's boundary (`classify.split`). I follow the split already recorded
   rather than re-deriving one: e.g. "a mechanical wheel inside the mouse
   produces two pulse trains" is split into the testable half (each axis is
   carried by exactly two lines) and the non-testable half (there is no
   mechanical wheel at a Verilog module's boundary for any assertion to reach).
3. **Figure 8-2's truth table is graded per cell**, per the task's own
   instruction — 4 rows × 2 columns (D1, D0) = 8 separate assertions, not one
   assertion for the table. Each cell is supported by both a page-229 running
   text quotation (the OCR stream captured the table as plain text, whole rows
   at a time) and a page-230 figure quotation (the same table, split per cell
   by the `sol` reader) — both are cited.
4. **Register-address and word-layout content is out of this package's
   subject.** Figure 8-3 also draws JOY0DAT's address ($DFF00A), its
   read-only caption, and the fact its two counter bytes form one 16-bit word.
   That is not quadrature *decoding* — it's the register's address and width,
   the subject of a differently-scoped claims file in this same claims
   directory (`ch8-joydat-counter-layout.jsonl`). I decline that half of the
   Figure 8-3 claims here, not because it is unmeasurable, but because it is
   not this package's assigned subject.
5. **The DE-9 pin number is not a signal of this module.** Figure 8-3's four
   direction-line labels each name a physical connector contact (e.g. "pin 1")
   together with a joystick function name (FORWARD/BACK/LEFT/RIGHT) and the
   mouse-quadrature name sharing that same wire (MOUSE V/H/VQ/HQ). A
   Verilog module compiled alone has no connector and no pin numbers — only
   whatever logical ports it declares. Composing a DE-9 contact number with a
   *joystick* function name additionally requires Table 8-3 (printed p.232),
   which is not in this claims file. What *is* testable at this module's
   boundary, and squarely part of quadrature decoding, is the half Figure 8-3
   states unambiguously on its own: which **axis** (Y/vertical or X/
   horizontal) each of the two mouse-quadrature signal pairs feeds. That half
   is asserted (A4); the pin-number and joystick-function half is declined.
6. **JOY1DAT is explicitly in this package's subject** (per the task: "into
   the JOY0DAT/JOY1DAT counters"). Figure 8-3's own caption for port 2 says
   its wiring "is wired similarly" to port 1's — that is a quadrature-decode
   proposition about port 2, not a register-layout one, and gets its own
   assertion (A18) rather than being folded into the declined address/layout
   material.

Every one of the 61 uids is accounted for below, either as support for a new
assertion or in the "claims not turned into a new assertion" table with a
reason.

## Chipset scope

Page 229's body and Figure 8-2 (p.230) are unqualified: the claims file's own
`classify.chipset` notes record that the chapter names no chip set anywhere in
this range and is silent on ECS/AGA, carrying an implicit "OCS-era, as
written" scope, same as this chapter's other packages have already recorded.
Figure 8-3 (p.233) carries the same silence. Nothing in the 61 claims disputes
the quadrature-decode mechanism, the truth table, or the direction sense
across chipsets, so every new assertion below is unconditional — it is not
measured under two different chipset configurations the way some chapter-7
packages had to.

## New assertions

### A1 — MOUSE-QUADRATURE-DELIVERS-KNOWN-DISTINCT-AXIS-COUNTS
- Pages: 229, 281. Uids: `opus:8-mouse.c1`, `opus:8-mouse.c4`,
  `sol:8-mouse.c1`, `sol:8-mouse.c2`, `opus:8-appa-joy.c86`,
  `sol:8-appa-joy.c51`.
- Quotes: p.229 "Pulses entering the mouse inputs are converted to separate
  horizontal and vertical counts."; p.229 "The mouse uses quadrature
  inputs."; p.281 "After being sampled, these connector pin signals are used
  in quadrature to clock the mouse counters."
- Proposition, and the mandatory fixture-liveness proof at once: a distinct,
  known number of quadrature transitions driven on the horizontal pair alone
  arrives at the horizontal count and leaves the vertical count exactly where
  it started; a different, distinct known number driven on the vertical pair
  alone arrives at the vertical count and leaves the horizontal count exactly
  where it started. Before any later, content-specific assertion in this file
  is graded, this must hold — it is the proof that userio.v's data path here
  is actually live, not silently dead, on this fixture's first-ever use.
- Expected: PASS (harness precondition; every later assertion in this package
  depends on it).

### A2 — MOUSE-COUNTERS-ARE-8-BIT-AND-WRAP
- Page: 229, 230. Uids: `opus:8-mouse.c2`, `sol:8-mouse.c3`, `sol:8-mouse.c4`,
  `opus:8-fig8-2.c3` (the figure's D2/"etc" trace, showing the decode's carry
  continuing into the counter's higher-order bits — the same fact this
  assertion's wraparound measures, not a separate one).
- Quote: p.229 "The 8 bit wide horizontal and vertical counter registers can
  track mouse movement without processor intervention."
- Proposition: the horizontal counter and the vertical counter are each 8 bits
  wide — driven far enough past 255 (or below 0) in one direction, each wraps
  rather than saturating or stopping, and the carry the figure draws from D1
  into D2 is what makes that wraparound possible in the first place.
- Expected: not stated by the claims file; to be measured.

### A3 — MOUSE-COUNTERS-ADVANCE-WITHOUT-REGISTER-ACCESS
- Page: 229. Uids: `opus:8-mouse.c3`, `sol:8-mouse.c5`.
- Quote: "The 8 bit wide horizontal and vertical counter registers can track
  mouse movement without processor intervention."
- Proposition: the counters advance purely from quadrature-pin transitions —
  driving a run of transitions with no register address ever read or written
  during the run, then reading the result afterward, shows the same count as
  interleaving reads throughout. This is the "no processor intervention"
  half of the sentence; the width half is A2.
- Expected: not stated by the claims file; to be measured.

### A4 — EXACTLY-TWO-LINES-PER-AXIS-MATCH-VERTICAL-HORIZONTAL-LABELING
- Pages: 229, 233. Uids: `opus:8-mouse.c5` (two-pulse-trains half only — the
  mechanical-wheel half is discarded, see below), `sol:8-mouse.c6`,
  `opus:8-fig8-3.c1`, `opus:8-fig8-3.c2`, `opus:8-fig8-3.c3`,
  `opus:8-fig8-3.c4` (axis-routing half of each pin label only — the DE-9
  pin-number/joystick-function half is discarded, see organisation point 5
  above), `opus:8-fig8-3.c5`, `opus:8-fig8-3.c6`, `opus:8-fig8-3.c7`,
  `opus:8-fig8-3.c8`, `sol:8-fig8-3.c1`, `sol:8-fig8-3.c2`,
  `sol:8-fig8-3.c3`, `sol:8-fig8-3.c4` (same axis-routing-only halves as
  their opus twins).
- Quotes: p.229 "For each direction, a mechanical wheel inside the mouse will
  produce two pulse trains, one 90 degrees out of phase with the other (see
  Figure 8-2 for details)."; p.233 figure — the two direction-line pairs
  drawn running into "MOUSE 0 Y counter" (captioned "vertical") and
  "MOUSE 0 X counter" (captioned "horizontal").
- Proposition: of the four mouse-quadrature direction lines, exactly two
  (V, VQ) move the vertical/Y count and no others do, and exactly two (H, HQ)
  move the horizontal/X count and no others do — each of the four lines
  individually toggled, one at a time, moves exactly one axis's count and
  leaves the other fixed.
- Expected: not stated by the claims file; to be measured.

### A5 — ONE-COUNT-PER-QUADRATURE-STATE-CHANGE
- Pages: 229, 281. Uids: `opus:8-mouse.c6` (unit-stripped: the claims file's
  own note strips "degrees" and keeps the underlying dimensionless
  quarter-cycle offset, measurable in state changes, not degrees),
  `sol:8-mouse.c7`, `opus:8-appa-joy.c86`, `sol:8-appa-joy.c51`.
- Quotes: p.229 "...one 90 degrees out of phase with the other (see Figure
  8-2 for details)."; p.281 "...these connector pin signals are used in
  quadrature to clock the mouse counters."
- Proposition: each single accepted quadrature-state transition ("clock[s]
  the mouse counters") advances or retreats its axis's count by exactly one —
  not more, not fewer, not skipped — across a run of many transitions taken
  in the same rotational sense, on both axes independently. A previous
  classification pass reports having measured this itself (±24 counts for 24
  state changes, and 10/20/30/40/50 state changes giving exact matching
  deltas); that is its measurement, not this package's — it is not adopted
  here, and this assertion is designed to reproduce the finding with its own
  chosen transition counts rather than repeat the other one's numbers.
- Expected: not stated by the claims file; to be measured.

### A6 — VERTICAL-CASE1-V-LEADS-VQ-COUNTS-UP
- Pages: 229, 230. Uids: `opus:8-fig8-2.c1`, `sol:8-fig8-2.c1`,
  `opus:8-mouse.c7` (general phase-determines-direction statement, primary
  citation — also underlies A7-A9 on the same basis, not repeated on each),
  `sol:8-mouse.c8`, `opus:8-mouse.c13`, `sol:8-mouse.c10`.
- Quotes: p.230 figure, waveform block labelled "Case 1: Count Up:" — the V
  trace rises one quarter-period before the VQ trace; p.229 "The phase
  relationship determines direction." and "The counters increment when the
  mouse is moved to the right or 'down' (toward you)."
- Proposition: driving the vertical pair through the Case-1 phase order (V
  leading VQ, the four-state ring 00→10→11→01 the figure draws) increases the
  vertical count, matching both the figure's own "Count Up" label and the
  text's "down... increment[s]" sentence (vertical axis, positive sense).
- Expected: not stated by the claims file; to be measured.

### A7 — VERTICAL-CASE2-VQ-LEADS-V-COUNTS-DOWN
- Pages: 229, 230. Uids: `opus:8-fig8-2.c2`, `sol:8-fig8-2.c2`,
  `opus:8-mouse.c15`, `sol:8-mouse.c12`.
- Quotes: p.230 figure, waveform block labelled "Case 2: Count Down:" — the
  VQ trace rises one quarter-period before the V trace; p.229 "The counters
  decrement when the mouse is moved to the left or 'up' (away from you)."
- Proposition: driving the vertical pair through the Case-2 phase order (VQ
  leading V) decreases the vertical count, matching the figure's "Count Down"
  label and the text's "up... decrement[s]" sentence.
- Control case: same measurement basis as A6 (one control-PASS-shaped
  assertion, one under-suspicion-shaped assertion, both against the same
  ring-driving harness) — together A6/A7 are this package's PASS/PASS pair
  proving the harness can show *either* outcome, not just the one it happens
  to produce.
- Expected: not stated by the claims file; to be measured.

### A8 — HORIZONTAL-RIGHTWARD-SENSE-INCREMENTS
- Page: 229. Uids: `opus:8-mouse.c12`, `sol:8-mouse.c9`.
- Quote: "The counters increment when the mouse is moved to the right or
  'down' (toward you)."
- Proposition: driving the horizontal pair through the same phase order
  Figure 8-2 labels "Count Up" for the vertical pair (H leading HQ) increases
  the horizontal count — the "right" half of the sentence, tested on the
  horizontal pair by the ordinary reading of "right" as the horizontal sense
  (paired with A6's "down" on the vertical pair). The figure itself only
  draws V/VQ traces; this assertion applies the general phase-determines-
  direction sentence (A6's `opus:8-mouse.c7`/`sol:8-mouse.c8`) to the other
  axis, not a figure specific to H/HQ.
- Expected: not stated by the claims file; to be measured.

### A9 — HORIZONTAL-LEFTWARD-SENSE-DECREMENTS
- Page: 229. Uids: `opus:8-mouse.c14`, `sol:8-mouse.c11`.
- Quote: "The counters decrement when the mouse is moved to the left or 'up'
  (away from you)."
- Proposition: driving the horizontal pair through the "Count Down" phase
  order (HQ leading H) decreases the horizontal count — same reasoning as A8,
  the "left" half paired with A7's "up" on the vertical pair.
- Expected: not stated by the claims file; to be measured.

### A10 — FIG8-2-TABLE-V0-VQ0-D1-IS-1
- Pages: 229, 230. Uids: `opus:8-mouse.c8` (whole-row text quotation, page
  229's OCR stream), `sol:8-fig8-2.c3` (D1-only half of the same cell, page
  230's figure).
- Quote: p.229 text "...V VQ : D1 D0 --------- 0 0 : 1 0"; p.230 figure, same
  table, row 1.
- Proposition: at quadrature state V=0, VQ=0, the decode's D1 output is 1.
- Expected: not stated by the claims file; to be measured.

### A11 — FIG8-2-TABLE-V0-VQ0-D0-IS-0
- Pages: 229, 230. Uids: `opus:8-mouse.c8`, `sol:8-fig8-2.c4`.
- Same quotes as A10.
- Proposition: at V=0, VQ=0, the decode's D0 output is 0.
- Expected: not stated by the claims file; to be measured.

### A12 — FIG8-2-TABLE-V0-VQ1-D1-IS-0
- Pages: 229, 230. Uids: `opus:8-mouse.c9`, `sol:8-fig8-2.c5`.
- Quote: p.229 text row "0 1 : 0 1"; p.230 figure, same table, row 2.
- Proposition: at V=0, VQ=1, the decode's D1 output is 0.
- Expected: not stated by the claims file; to be measured.

### A13 — FIG8-2-TABLE-V0-VQ1-D0-IS-1
- Pages: 229, 230. Uids: `opus:8-mouse.c9`, `sol:8-fig8-2.c6`.
- Same quotes as A12.
- Proposition: at V=0, VQ=1, the decode's D0 output is 1.
- Expected: not stated by the claims file; to be measured.

### A14 — FIG8-2-TABLE-V1-VQ0-D1-IS-1
- Pages: 229, 230. Uids: `opus:8-mouse.c10`, `sol:8-fig8-2.c7`.
- Quote: p.229 text row "1 0 : 1 1"; p.230 figure, same table, row 3.
- Proposition: at V=1, VQ=0, the decode's D1 output is 1.
- Expected: not stated by the claims file; to be measured.

### A15 — FIG8-2-TABLE-V1-VQ0-D0-IS-1
- Pages: 229, 230. Uids: `opus:8-mouse.c10`, `sol:8-fig8-2.c8`.
- Same quotes as A14.
- Proposition: at V=1, VQ=0, the decode's D0 output is 1.
- Expected: not stated by the claims file; to be measured.

### A16 — FIG8-2-TABLE-V1-VQ1-D1-IS-0
- Pages: 229, 230. Uids: `opus:8-mouse.c11`, `sol:8-fig8-2.c9`.
- Quote: p.229 text row "1 1 : 0 0"; p.230 figure, same table, row 4.
- Proposition: at V=1, VQ=1, the decode's D1 output is 0.
- Expected: not stated by the claims file; to be measured.

### A17 — FIG8-2-TABLE-V1-VQ1-D0-IS-0
- Pages: 229, 230. Uids: `opus:8-mouse.c11`, `sol:8-fig8-2.c10`.
- Same quotes as A16.
- Proposition: at V=1, VQ=1, the decode's D0 output is 0.
- Expected: not stated by the claims file; to be measured.

### A18 — JOY1DAT-PORT-MIRRORS-JOY0DAT-QUADRATURE-DECODE
- Page: 233. Uids: `opus:8-fig8-3.c11`, `sol:8-fig8-3.c6`.
- Quote: p.233 figure, port 2 connector caption "JOY1DAT / DFF00C / is wired
  similarly" (to port 1).
- Proposition: port 2's quadrature pair reproduces the same direction sense
  as port 1's — driving port 2's vertical-equivalent pair through the
  Case-1/Case-2 phase orders moves its count up/down exactly as A6/A7 showed
  for port 1, independently of whatever port 1 is doing at the same time.
  This is the quadrature-decode half of "wired similarly"; the address
  ($DFF00C) and read-only-word-layout half is out of this package's subject
  (organisation point 4 above).
- Expected: not stated by the claims file; to be measured.

## Claims not turned into a new assertion

| uid(s) | page | proposition | reason |
|---|---|---|---|
| `opus:8-fig8-2.c4` | 230 | image annotates one D0 pulse and the following D1 pulse with digits "1" and "0" | Subsumed — this is one instant of the same V/VQ:D1/D0 table A10-A17 already grade exhaustively, all four rows, both columns. The claims file itself flags this record `borderline`. |
| `opus:8-fig8-2.c5` | 230 | the in-figure table matches the page-229 text table | Subsumed — this is exactly what pairing a page-229-text uid with a page-230-figure uid under each of A10-A17 already establishes, cell by cell. The claims file itself flags this record `borderline`. |
| `opus:8-fig8-3.c9` | 233 | JOY0DAT is captioned "$DFF00A, read only" | Out of subject (organisation point 4) — register address and access mode, not quadrature decoding. |
| `sol:8-fig8-3.c5` | 233 | same JOY0DAT address/read-only caption | Same reason as `opus:8-fig8-3.c9` (its twin). |
| `opus:8-fig8-3.c10` | 233 | the two counter boxes are drawn as 8+8=16 bit cells, one register | Out of subject (organisation point 4) — register width, not quadrature decoding. |

## Totals

- 61 claims read, all 61 accounted for above.
- 18 new graded assertions (A1-A18): none pre-judged by the source claims: A1
  is the mandatory fixture-liveness precondition; A6/A7 are a same-basis
  PASS/PASS direction pair; A10-A17 are Figure 8-2's truth table, one
  assertion per cell as the task requires.
- 5 claims declined, each with its own reason: 2 subsumed into the per-cell
  table assertions, 3 out of this package's subject (register address/word
  layout, not quadrature decoding).

Nothing in this package required opening the RTL, the port list, or any file
under `nanomig-hrm-tests/nanomig/` to write. No HALT condition was
encountered.
