# ch8-joydat-counter-layout

Fixture: `userio.v`, compiled alone. Printed pages: 230, 281.

This package tests the **layout** of the JOY0DAT/JOY1DAT mouse/trackball
counter registers: which register exists, how wide it is, which byte holds
the horizontal vs. vertical count, and — at the bit-table on p.281 — which
individual bit holds which named counter bit. It does not test the
quadrature *decode* arithmetic (up/down counting correctness); that is
`ch8-mouse-quadrature-decode`'s job. It does not test the POTGO/POTGOR bit
layout; that is `ch8-potgo-register-bit-layout`'s job. It does not test the
general `userio` register address map; that is `ch8-register-map-userio`'s
job. Where a claim in this package's claims file needs any of that other
scope to be settled, it is declined below rather than asserted here.

Every assertion below is written from its quotation alone. No port, signal,
or register-offset name has been taken from the RTL — only the register
names `JOY0DAT`, `JOY1DAT`, `JOYTEST` (the last named only in the "already
cost" carry-forward measurement quoted in the build brief, not from any
manual quotation in this package's claims, and used only as informal
narration here, never as an asserted proposition) appear, because the
manual itself uses them as its register names on the pages assigned.

## Control case / fixture liveness

**opus:8-joydat.c1** — p.230
> "The mouse/trackball counter contents can be accessed by reading register
> addresses named  JOY0DAT and JOY1DAT .  These registers contain counts for"

Proposition under test: reading at the JOY0DAT and JOY1DAT addresses
returns the counter contents — i.e. these are live, readable registers,
not a dead or always-zero decode. This is this package's control case: the
fixture must be shown to deliver a known, distinct word at each address
before any other assertion in this file is trusted. Pass criterion: with
distinct known values placed in the two registers' underlying counters, a
read at the JOY0DAT address and a read at the JOY1DAT address each return
the value placed there, and the two reads differ from each other when the
values placed differ.

## Register width — p.230

**opus:8-joydat.c4** — p.230
> "The contents of each of these 16-bit registers are as follows:"

Proposition: JOY0DAT and JOY1DAT are each 16 bits wide (collective
statement, both registers).

**sol:8-mouse.c15** — p.230 (same quotation, register-individual restatement)
Proposition: JOY0DAT is 16 bits wide.

**sol:8-mouse.c16** — p.230 (same quotation, register-individual restatement)
Proposition: JOY1DAT is 16 bits wide.

Pass criterion for all three: a full 16-bit-wide pattern is observable at
each register — i.e. a bit placed at position 15 down to position 0 is each
independently readable back, with no position silently masked, tied off,
or aliased to another position within the same register.

## Byte assignment within the register — p.230

**opus:8-joydat.c5** — p.230
> "   Bits 15-8     Mouse/trackball vertical count"

**sol:8-mouse.c17** — p.230 (same quotation, "each JOYxDAT register" restatement)

Proposition: bits 15-8 of JOY0DAT/JOY1DAT hold the vertical count.

**opus:8-joydat.c6** — p.230
> "   Bits  7-0     Mouse/trackball horizontal count"

**sol:8-mouse.c18** — p.230 (same quotation, "each JOYxDAT register" restatement)

Proposition: bits 7-0 of JOY0DAT/JOY1DAT hold the horizontal count.

Pass criterion for all four: driving only the vertical-count input of a
counter changes only the high byte (bits 15-8) of its register while the
low byte (bits 7-0) is unaffected, and driving only the horizontal-count
input changes only the low byte while the high byte is unaffected — for
both JOY0DAT and JOY1DAT.

## Declined — port-number labelling (p.230, disputed against p.233)

**opus:8-joydat.c2** — p.230
> "addresses named  JOY0DAT and JOY1DAT .  These registers contain counts for
> ports 1 and 2 respectively."

Proposition as restated: "JOY0DAT holds the counts for port 1."

**opus:8-joydat.c3** — p.230, same quotation window
Proposition as restated: "JOY1DAT holds the counts for port 2."

**sol:8-mouse.c13** — p.230, fuller quotation window (same sentence)
Proposition as restated: "Reading JOY0DAT accesses the mouse/trackball
counts for controller port 1."

**sol:8-mouse.c14** — p.230, same quotation window
Proposition as restated: "Reading JOY1DAT accesses the mouse/trackball
counts for controller port 2."

Decline reason (all four): these claims' own classification already flags
the fact as disputed — p.230 numbers the two ports "1" and "2", p.233
numbers what is evidently the same pair of registers differently (per this
claims file's own `conflict_resolution` field). The undisputed part —
that JOY0DAT and JOY1DAT are driven by two distinct, independent physical
ports — is not what these four propositions assert; they assert a specific
numeral label ("port 1", "port 2") attached to a specific register. p.233
is outside the pages assigned to this package (230, 281), so this
package cannot read the page that would resolve which numbering the manual
intends, and cannot adjudicate a dispute the manual has with itself from
one side of it alone. This is the manual being ambiguous at the point
being tested — declined, not asserted with either numbering.

## Register composition, p.281 (Appendix A / mouse counter table preamble)

**opus:8-appa-joy.c9** — p.281
**sol:8-appa-joy.c11** — p.281 (same fact, JOY0DAT only)
**sol:8-appa-joy.c12** — p.281 (same fact, JOY1DAT only)
> "                 These addresses each read a pair of 8-bit mouse
>                  counters.  0=left controller pair, 1=right"

Proposition (c9): each of JOY0DAT and JOY1DAT reads a pair of 8-bit mouse
counters. (c11/c12: the same, stated once per register.)

**opus:8-appa-joy.c10** — p.281, same quotation
Proposition: each of those mouse counters is 8 bits wide.

Pass criterion (c9-c12, c10): each register is composed of two
independently-readable 8-bit fields (already covered functionally by the
byte-assignment assertions above); additionally, each 8-bit field spans
exactly 8 bits — a value written to fill all 8 bits of one field is read
back intact and does not overflow into, or get truncated by, the
neighbouring field.

**opus:8-appa-joy.c13** — p.281
**sol:8-appa-joy.c15** — p.281 (twin claim, same quotation window, same
proposition)
> "                 counters.  0=left controller pair, 1=right
>                  controller pair (four counters total).  The bit"

Proposition: JOY0DAT and JOY1DAT expose four mouse counters in total (i.e.
the two bytes of JOY0DAT plus the two bytes of JOY1DAT are four
independently-addressable-by-content counters, not fewer via aliasing).

Pass criterion: four distinct values placed in the four byte-fields (JOY0DAT
high, JOY0DAT low, JOY1DAT high, JOY1DAT low) are all four read back
distinctly and without cross-talk between any pair of them.

**opus:8-appa-joy.c14** — p.281
**sol:8-appa-joy.c16** — p.281 (twin claim, same proposition)
> "                 below.  Each counter is clocked by signals from
>                  two controller pins.  Bits 1 and 0 of each counter"

Proposition: each mouse counter is clocked by signals coming from two
controller pins (a cardinality claim about the number of input lines
feeding one counter — not a claim about how those two lines are decoded
into a direction, which is `ch8-mouse-quadrature-decode`'s scope). Pass
criterion: for one counter, driving only one of its two associated input
pins (holding the other fixed) changes that counter's value, and driving
only the other pin (holding the first fixed) also changes that same
counter's value — establishing that two, not one, independent input lines
each affect the counter.

## Bit-position table, p.281 — JOY0DAT row ("0DAT")

> "                 BIT# 15,14,13,12,11,10,09,08  07,06,05,04,03,02,01,00
>                       -----------------------  -----------------------
>                  0DAT Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0"

Each of the following claims composes the `BIT#` legend row above with the
`0DAT` value row beneath it — the row alone names no bit position, and the
legend alone names no register; the fact graded is the pairing of the two,
per column. One assertion per bit position, each an independent
proposition on the same table:

| uid | page | proposition |
|---|---|---|
| opus:8-appa-joy.c19 | 281 | JOY0DAT bit 15 = Y7 |
| opus:8-appa-joy.c20 | 281 | JOY0DAT bit 14 = Y6 |
| opus:8-appa-joy.c21 | 281 | JOY0DAT bit 13 = Y5 |
| opus:8-appa-joy.c22 | 281 | JOY0DAT bit 12 = Y4 |
| opus:8-appa-joy.c23 | 281 | JOY0DAT bit 11 = Y3 |
| opus:8-appa-joy.c24 | 281 | JOY0DAT bit 10 = Y2 |
| opus:8-appa-joy.c25 | 281 | JOY0DAT bit 09 = Y1 |
| opus:8-appa-joy.c26 | 281 | JOY0DAT bit 08 = Y0 |
| opus:8-appa-joy.c27 | 281 | JOY0DAT bit 07 = X7 |
| opus:8-appa-joy.c28 | 281 | JOY0DAT bit 06 = X6 |
| opus:8-appa-joy.c29 | 281 | JOY0DAT bit 05 = X5 |
| opus:8-appa-joy.c30 | 281 | JOY0DAT bit 04 = X4 |
| opus:8-appa-joy.c31 | 281 | JOY0DAT bit 03 = X3 |
| opus:8-appa-joy.c32 | 281 | JOY0DAT bit 02 = X2 |
| opus:8-appa-joy.c33 | 281 | JOY0DAT bit 01 = X1 |
| opus:8-appa-joy.c34 | 281 | JOY0DAT bit 00 = X0 |

Pass criterion, each row: a value that makes the named counter-bit (Y7..Y0
or X7..X0) a known, distinct level is placed so that only that bit is
expected to read 1 (the rest 0) at the claimed position; the read of
JOY0DAT shows a 1 at exactly that bit index and nowhere else attributable
to this stimulus.

## Bit-position table, p.281 — JOY1DAT row ("1DAT")

> "                 BIT# 15,14,13,12,11,10,09,08  07,06,05,04,03,02,01,00
>                       -----------------------  -----------------------
>                  0DAT Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0
>                  1DAT Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0"

Same composition as above (legend row + this claim's own quoted row window
extends down through the `1DAT` row, since — per each claim's own
`evidence`/quote — the `0DAT` and `1DAT` rows are identical apart from
their row label):

| uid | page | proposition |
|---|---|---|
| opus:8-appa-joy.c35 | 281 | JOY1DAT bit 15 = Y7 |
| opus:8-appa-joy.c36 | 281 | JOY1DAT bit 14 = Y6 |
| opus:8-appa-joy.c37 | 281 | JOY1DAT bit 13 = Y5 |
| opus:8-appa-joy.c38 | 281 | JOY1DAT bit 12 = Y4 |
| opus:8-appa-joy.c39 | 281 | JOY1DAT bit 11 = Y3 |
| opus:8-appa-joy.c40 | 281 | JOY1DAT bit 10 = Y2 |
| opus:8-appa-joy.c41 | 281 | JOY1DAT bit 09 = Y1 |
| opus:8-appa-joy.c42 | 281 | JOY1DAT bit 08 = Y0 |
| opus:8-appa-joy.c43 | 281 | JOY1DAT bit 07 = X7 |
| opus:8-appa-joy.c44 | 281 | JOY1DAT bit 06 = X6 |
| opus:8-appa-joy.c45 | 281 | JOY1DAT bit 05 = X5 |
| opus:8-appa-joy.c46 | 281 | JOY1DAT bit 04 = X4 |
| opus:8-appa-joy.c47 | 281 | JOY1DAT bit 03 = X3 |
| opus:8-appa-joy.c48 | 281 | JOY1DAT bit 02 = X2 |
| opus:8-appa-joy.c49 | 281 | JOY1DAT bit 01 = X1 |
| opus:8-appa-joy.c50 | 281 | JOY1DAT bit 00 = X0 |

Pass criterion, each row: as above, against JOY1DAT.

## Bit-position table, p.281 — aggregate byte-range restatements

These four claims restate ranges of the same table rows used above, as a
single per-byte proposition each (sol's reading), rather than one
proposition per bit (opus's reading). Both readings are graded; they are
not merged into the per-bit assertions above.

**sol:8-appa-joy.c21** — p.281
> "                 0DAT Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0"

Proposition: JOY0DAT bits 15-8 are mouse-counter bits Y7-Y0, as a group.

**sol:8-appa-joy.c22** — p.281, same quotation
Proposition: JOY0DAT bits 7-0 are mouse-counter bits X7-X0, as a group.

**sol:8-appa-joy.c23** — p.281
> "                 1DAT Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0"

Proposition: JOY1DAT bits 15-8 are mouse-counter bits Y7-Y0, as a group.

**sol:8-appa-joy.c24** — p.281, same quotation
Proposition: JOY1DAT bits 7-0 are mouse-counter bits X7-X0, as a group.

Pass criterion, each: a single 8-bit pattern placed across the named four
(or eight, counting both nibble halves) counter bits is read back as one
intact byte at the claimed register half, composing the same legend/row
pairing as the per-bit table above but graded as one byte-wide comparison
per claim rather than eight single-bit comparisons.

## Summary

56 claims read from this package's claims file. 52 become assertions
(`expect()`), listed above by uid and page. 4 are declined (`record()`):
opus:8-joydat.c2, opus:8-joydat.c3, sol:8-mouse.c13, sol:8-mouse.c14 — the
port-number-label dispute against p.233, out of this package's assigned
pages.

Every assertion above needs the fixture's control case (opus:8-joydat.c1)
to hold first: known, distinct values must be shown to arrive at both
register addresses before any byte- or bit-position claim built on top of
that delivery is meaningful.
