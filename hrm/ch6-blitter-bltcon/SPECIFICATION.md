# BLTCON0 / BLTCON1 Test Specification

Written from this package's own claims and the *Amiga Hardware Reference Manual*'s
pages by an author who never saw the RTL or the test; the package's assertions have
been checked against it.

This document specifies, from the claims in `claims-spec-stage.jsonl` alone, what a
test package for the Amiga blitter's two control registers — BLTCON0 and BLTCON1 —
should check. It groups the 220 claims into 22 propositions, organized by the two
register bit-tables the *Amiga Hardware Reference Manual*, 3rd edition, gives for
these registers: the area-mode table and the line-mode table (both printed across
pages 267-268), plus the register descriptions scattered through chapter 6 and its
appendix A entries. Each proposition states what a port-level test would write,
what it would observe, and the expected value or relation the cited page states.
Where a table cell is named on the page but no claim in the file documents its
behavior, or where two claims disagree, that is said explicitly rather than filled
in.

## Claims not used

- **`6-line-mode.c68`** ("In the line-mode logic function, source C is the
  original destination data.") is not used. Its own quoted fragment ends
  mid-sentence ("The C DMA channel represents the original"), and the words that
  actually complete it on page 191 are "...represents the original **source**,
  the A channel the bit to set in the line, and the B channel the pattern to
  draw." The restatement's "destination data" is not what the page says. No
  proposition rests on this claim.

## Table cells that no claim documents

Three bits that the area-mode table on page 267 names are not given a meaning by
any claim in the file (checked against both the `sol`-read and the `opus`-read
passes, which independently produced the same gap):

- **BLTCON1 bit 7, area mode ("DOFF")** — the table prints the name, but no claim
  states what DOFF does, and no claim on any page read for this task defines it
  either.
- **BLTCON1 bit 3, area mode ("IFE")** — page 267's legend does define "IFE
  Inclusive fill enable", but no claim quotes or restates that legend line (the
  neighboring EFE legend line is claimed; this one is not).
- **BLTCON1 bit 2, area mode ("FCI")** — page 267's legend defines "FCI Fill
  carry input", again with no claim carrying it forward.

These three are named below (in the fields adjacent to them) but get no
proposition of their own: a test must not invent what they do.

A fourth cell, **BLTCON0 bit 12, area mode**, is a softer case: the table prints
"ASA0" for it (the header row calls the field "ASH3-0", and bits 15/14/13 are
individually claimed as ASH3/ASH2/ASH1 — "ASA0" for bit 12 looks like a printing
slip for "ASH0", but no claim resolves it either way). No claim assigns bit 12 a
per-bit name. It is, however, covered collectively: a claim from page 179 states
that "the shift value for the A channel is set with bits 15 through 12 of
BLTCON0", which does place bit 12 inside the A-shift field even without naming
it. See Proposition 2.

## Naming conflicts the manual itself carries

Several bits have two or three different names depending on which part of the
manual is talking about them. The claims file flags three of these explicitly
with `"flags": ["conflict"]`; a fourth pair is visible by comparing pages
directly. A test package should pick one vocabulary and note the others as
synonyms, not treat them as different bits:

- BLTCON0 bits 11/10/9/8: register-summary table (p267) calls them **USEA /
  USEB / USEC / USED**; chapter 6's DMA-channel prose (p170) calls the same four
  **SRCA / SRCB / SRCC / DEST**; the line-mode register summary (p192) uses
  **SRCA / SRCB / SRCC** for the same first three but calls the fourth **SRCD**;
  and the line-mode walkthrough (p191) calls it **DEST**. Bit 8 alone thus has
  three names on file: USED, DEST, SRCD.
- BLTCON1 bit 4 (area mode): **EFE** (p267) vs **FILL_XOR** (p184).
- BLTCON1 bit 6 (line mode): **SIGN** (p268 table and legend) vs **SIGNFLAG**
  (p191, p192).
- BLTCON1 bit 1 (line mode): **SING** (p268 table and legend) vs **ONEDOT**
  (p191, p192).

---

## Overview: two modes, one bit

**Proposition 1.** BLTCON0 and BLTCON1 are used together to control every
blitter operation, in one of two modes — area (also called "normal") and line —
selected by BLTCON1 bit 0. Bit 0 clear selects area mode; bit 0 set selects line
mode.
Rests on: `6-appa-registers.c22`, `6-appa-registers.c23`, `6-appa-registers.c24`,
`ch6-appa-bltcon0.c7`, `ch6-appa-bltcon0.c8`, `ch6-appa-bltcon0.c9` (all page 267,
framing statements the `opus` cross-read judged descriptive rather than
bit-precise — they are used here as the framing they are, not as a bit test);
`6-appa-control.c32`, `ch6-appa-bltcon0.area.b00.con1`, `6-appa-control.c44`,
`ch6-appa-bltcon0.legend.LINE` (BLTCON1 bit 0 = 0, area mode, page 267);
`6-appa-control.c76`, `ch6-appa-bltcon0.line.b00.con1`, `6-appa-control.c86`,
`ch6-appa-lf.legend.LINE` (BLTCON1 bit 0 = 1, line mode, page 268).

Test: write BLTCON1 with bit 0 = 0, then with bit 0 = 1, holding the rest of
BLTCON0/BLTCON1 at a fixed, valid pattern for each mode, and start a blit both
times (this document does not name the register that starts it, only that
BLTCON0/1 select which of the two behaviors that start produces). Observe which
of the two behaviors described in the manual results: an area operation (the
rectangular copy/logic/fill behavior of the propositions below) or a line-draw
operation (the octant-driven behavior of the line-mode propositions below).

Expected: bit 0 = 0 gives area-mode behavior ("LINE=0 selects area mode" is
literally the legend's own words on page 267); bit 0 = 1 gives line-mode
behavior ("LINE=1 selects line mode", page 268 legend).

---

## Area mode (BLTCON1 bit 0 = 0)

### Field: A-channel shift value — BLTCON0 bits 15-12 ("ASH3-0")

**Proposition 2.** BLTCON0 bits 15, 14 and 13 are named ASH3, ASH2 and ASH1
respectively; together with bit 12 (unnamed by any claim — see "Table cells"
above) they form the A-channel's shift-value field, set by "bits 15 through 12
of BLTCON0" per page 179.
Rests on: `6-appa-control.c1` (bit 15 = ASH3), `ch6-appa-bltcon0.area.b15.con0`;
`6-appa-control.c3` (bit 14 = ASH2), `ch6-appa-bltcon0.area.b14.con0`;
`6-appa-control.c5` (bit 13 = ASH1), `ch6-appa-bltcon0.area.b13.con0`;
`ch6-appa-bltcon0.legend.ASH30` (legend, "ASH3-0 Shift value of A source",
page 267); `ch6-shifts.c8` (page 179, "the shift value for the A channel is
set with bits 15 through 12", which places bit 12 in the field without naming
it). All page 267 except `ch6-shifts.c8` (page 179).

Test: write a 4-bit pattern into BLTCON0 bits 15-12, enable the A channel
(BLTCON0 bit 11, Proposition 4) with a known word of source data, and run a
blit. Observe the A data the blitter uses in its logic-function combination.

Expected: the A-channel data is shifted by the amount held in bits 15-12, per
the field's name (register-summary legend, "ASH3-0 Shift value of A source" —
`ch6-appa-bltcon0.legend.ASH30`, page 267). No claim in the file states the
shift's direction or its numeric range, so this specification does not assert
either as an expected value.

### Field: B-channel shift value — BLTCON1 bits 15-12 ("BSH3-0")

**Proposition 3.** BLTCON1 bits 15, 14, 13 and 12 are named BSH3, BSH2, BSH1 and
BSH0 respectively — the B-channel's shift-value field, "Shift value of B
source" per the register-summary legend, and set by "bits 15 through 12 of
BLTCON1" per page 179.
Rests on: `6-appa-control.c2` (bit 15 = BSH3), `ch6-appa-bltcon0.area.b15.con1`;
`6-appa-control.c4` (bit 14 = BSH2), `ch6-appa-bltcon0.area.b14.con1`;
`6-appa-control.c6` (bit 13 = BSH1), `ch6-appa-bltcon0.area.b13.con1`;
`6-appa-control.c8` (bit 12 = BSH0), `ch6-appa-bltcon0.area.b12.con1`;
`6-appa-control.c34`, `ch6-appa-bltcon0.legend.BSH30` (legend, "BSH3-0 Shift
value of B source");
`6-shifts-and-masks.c9`, `ch6-shifts.c9` (page 179, "the B shift value is set
with bits 15 through 12 of BLTCON1"). All page 267 except the last two (page
179).

Test: write a 4-bit pattern into BLTCON1 bits 15-12, enable the B channel
(BLTCON0 bit 10, Proposition 5) with known source data, run a blit, and
observe the B-channel data used in the logic function.

Expected: the B-channel data is shifted by the value held in bits 15-12. As
with the A shift, no claim states direction or numeric range, so none is
asserted here.

### Fields: per-channel enable bits — BLTCON0 bits 11-8

**Proposition 4 (bit 11, USEA / SRCA).** BLTCON0 bit 11 is USEA, "Mode control
bit to use source A" — it enables the A DMA channel.
Rests on: `6-appa-control.c9` (bit 11 = USEA, page 267),
`ch6-appa-bltcon0.area.b11.con0`; `6-appa-control.c35`,
`ch6-appa-bltcon0.legend.USEA` (legend, "USEA Mode
control bit to use source A", page 267); `6-dma-channels.c25`,
`6-dma-channels.c26`, `6-dma-channels.c9`, `ch6-dma.c25` (flagged conflict —
names this bit "SRCA" instead, page 170), `ch6-dma.c26`, `ch6-dma.c7` (the
general "each channel has an enable bit" statement, page 170).

Test: write BLTCON0 bit 11 = 1 with A-channel data available, then = 0, holding
everything else fixed, and observe whether the A-channel's data participates in
the logic-function result (Proposition 9).

Expected: set (1) means source A is used; the manual gives no claimed statement
of what value substitutes for A's data when the bit is clear, so this
specification asserts only the enable/disable meaning of the bit itself, not a
specific substituted value.

**Proposition 5 (bit 10, USEB / SRCB).** BLTCON0 bit 10 is USEB, "Mode control
bit to use source B" — it enables the B DMA channel.
Rests on: `6-appa-control.c11` (bit 10 = USEB, page 267),
`ch6-appa-bltcon0.area.b10.con0`; `6-appa-control.c36`,
`ch6-appa-bltcon0.legend.USEB` (legend, page 267); the
same DMA-channel group as Proposition 4 (`6-dma-channels.c25/c26/c9`,
`ch6-dma.c25/c26/c7`) since it names all four enable bits together.

Test/expected: as Proposition 4, for the B channel.

**Proposition 6 (bit 9, USEC / SRCC).** BLTCON0 bit 9 is USEC, "Mode control bit
to use source C" — it enables the C DMA channel.
Rests on: `6-appa-control.c13` (bit 9 = USEC, page 267),
`ch6-appa-bltcon0.area.b09.con0`; `6-appa-control.c37`,
`ch6-appa-bltcon0.legend.USEC` (legend, page 267); same
DMA-channel group as above.

Test/expected: as Proposition 4, for the C channel.

**Proposition 7 (bit 8, USED / DEST / SRCD).** BLTCON0 bit 8 is USED, "Mode
control bit to use destination D" — it enables the destination channel. This is
the bit with three names on file (see "Naming conflicts" above).
Rests on: `6-appa-control.c15` (bit 8 = USED, page 267),
`ch6-appa-bltcon0.area.b08.con0`; `6-appa-control.c38`,
`ch6-appa-bltcon0.legend.USED` (legend, page 267); same
DMA-channel group (`6-dma-channels.c25/c26/c9`, `ch6-dma.c25/c26/c7`, page 170,
naming it DEST); the line-mode uses of the same physical bit, which call it DEST
(`6-line-mode.c63`, page 191) and SRCD (`6-line-mode.c88`, `ch6-linesum.c13`
flagged conflict, page 192), are cited again under Proposition 15 since they
describe the required value in line mode, not the area-mode meaning.

Test: write BLTCON0 bit 8 = 1 to enable the destination, = 0 to disable it, and
observe whether the blit's result is written back to memory (Figure 6-13, cited
under Proposition 9, shows a "Store to Destination" control line coming out of
the same decode block that also produces the shift and minterm-select signals —
`ch6-fig613.f27`, page 199 — consistent with, though not itself naming, this
bit).

Expected: set (1) means the destination is used, i.e. written; the manual gives
no claimed statement of blitter behavior with this bit clear beyond "not used".

**Proposition 8 (BLTCON1 bits 11-8, unused in area mode).** All four of these
bits are marked "X" (unused) in the area-mode table.
Rests on: `6-appa-control.c10` (bit 11), `ch6-appa-bltcon0.area.b11.con1`;
`6-appa-control.c12` (bit 10), `ch6-appa-bltcon0.area.b10.con1`;
`6-appa-control.c14` (bit 9), `ch6-appa-bltcon0.area.b09.con1`;
`6-appa-control.c16` (bit 8), `ch6-appa-bltcon0.area.b08.con1`. All page 267.

This is exactly the case the task brief anticipates: the page marks these bits
unused rather than stating an effect for them. No expected value is given, and
a test must not invent one — in particular, a test must not assert that writing
1 or 0 to these bits has any documented effect in area mode.

### Field: logic function / minterm select — BLTCON0 bits 7-0 ("LF7-0")

**Proposition 9.** BLTCON0 bits 7 through 0 form the LF (Logic Function)
control byte: eight independent bits, each one a minterm-select line that gives
the destination value for one of the eight possible combinations of a single
bit from source A, source B and source C. The combination-to-bit-position
mapping, from the truth table on page 174, is: position = 4·A + 2·B + C (A=0,B=0,C=0
-> position 0; A=1,B=1,C=1 -> position 7); LF bit *n* holds the destination
value for the combination whose position is *n*, and the byte is read "from the
bottom of the table up" — i.e., bit 7 corresponds to A=B=C=1 and bit 0 to
A=B=C=0.
Rests on: bit-identity claims `6-appa-control.c17` (bit 7=LF7),
`ch6-appa-bltcon0.area.b07.con0`; `6-appa-control.c19` (bit 6=LF6),
`ch6-appa-bltcon0.area.b06.con0`; `6-appa-control.c21` (bit 5=LF5),
`ch6-appa-bltcon0.area.b05.con0`; `6-appa-control.c23` (bit 4=LF4),
`ch6-appa-bltcon0.area.b04.con0`; `6-appa-control.c25` (bit 3=LF3),
`ch6-appa-bltcon0.area.b03.con0`; `6-appa-control.c27` (bit 2=LF2),
`ch6-appa-bltcon0.area.b02.con0`; `6-appa-control.c29` (bit 1=LF1),
`ch6-appa-bltcon0.area.b01.con0`; `6-appa-control.c31` (bit 0=LF0),
`ch6-appa-bltcon0.area.b00.con0`; the legend claims `6-appa-control.c39` and
`ch6-appa-bltcon0.legend.LF70` ("LF7-0
Logic function minterm select lines"); the mechanism claims
`6-function-generator.c6` ("the LF control byte specifies a destination bit for
each source-bit combination", page 174), `ch6-funcgen.c4` ("the LF control byte
is in the BLTCON0 register", page 174), `ch6-funcgen.c10` ("bits 6 through 0 of
the LF control byte specify the D output for the remaining source
combinations", page 174); and the figure claim `ch6-fig613.f27` (Figure 6-13
shows the "Blitter Control" decode block supplying the "multi-minterm select
bits" alongside the shift values and fill controls, page 199). All bit-identity
and legend claims are page 267.

Test, with the two worked examples the manual itself gives on page 174: for
each of the eight combinations of one A bit, one B bit and one C bit (with
USEA/USEB/USEC enabled and each channel supplied the corresponding constant
data, and USED enabled to observe the result):
- write LF (BLTCON0 bits 7-0) = $FC (binary 11111100) and observe that the
  destination bit is 1 whenever the A bit is 1 or the B bit is 1, and 0 only
  when both A and B bits are 0 — the manual's own derivation of $FC is "set
  bits... where the corresponding A source bit is 1 or the corresponding B
  source bit is 1" (page 174, `6-function-generator.c6` and
  `ch6-funcgen.c10` support the general mechanism this example instantiates);
- write LF = $80 (binary 10000000) and observe that the destination bit is 1
  only for the single combination A=1,B=1,C=1, and 0 for the other seven — "an
  LF control byte of $80... turns on bits only for those points... where the
  corresponding bits of A, B, and C sources were all on" (page 174).

**Proposition 9a — BLTCON0L, page 269.** BLTCON0L is a separate write-only
register address ("blitter control 0, lower 8 bits (minterms)") that lets a
program write just BLTCON0 bits 7-0 (the LF byte) without touching bits 15-8.
Rests on: `ch6-appa-bltcon0l.c4` (page 269).

Test: write the LF byte through BLTCON0L instead of through BLTCON0, leaving
BLTCON0 bits 15-8 at a previously-established value, and observe that the
logic-function result (Proposition 9) follows the byte written through
BLTCON0L while the shift/enable bits are unaffected. No claim states what
happens to BLTCON0 bits 15-8 if BLTCON0L is written before BLTCON0's upper
byte has ever been set, so this specification does not assert a reset or
power-up value for them.

### Field: BLTCON1 bit 7, area mode ("DOFF") — no claim

No claim identifies what DOFF does. See "Table cells that no claim documents."
This specification states no proposition and no expected value for this bit.

### Field: BLTCON1 bits 6 and 5, area mode (unused)

**Proposition 10.** Both bits are marked "X" (unused) in the area-mode table.
Rests on: `6-appa-control.c20` (bit 6), `ch6-appa-bltcon0.area.b06.con1`;
`6-appa-control.c22` (bit 5), `ch6-appa-bltcon0.area.b05.con1`. Both page 267.

As with Proposition 8, no expected value is given and none should be invented.

### Field: BLTCON1 bit 4, area mode ("EFE" / "FILL_XOR")

**Proposition 11.** BLTCON1 bit 4 is EFE, "Exclusive fill enable" — also called
FILL_XOR on page 184. Setting it selects exclusive fill during an area-fill
operation.
Rests on: `6-appa-control.c24` (bit 4 = EFE, page 267),
`ch6-appa-bltcon0.area.b04.con1`; `6-appa-control.c40`,
`ch6-appa-bltcon0.legend.EFE` (legend, "EFE Exclusive
fill enable", page 267); `6-area-fill.c9`, `ch6-fill.c8` (page 184, naming it
FILL_XOR and describing exclusive fill); the figure claim `ch6-fig613.f27`
shows a "Fill Enable" line from the same decode block (page 199), consistent
with (though not itself distinguishing inclusive from exclusive fill).

Test, using the manual's own worked example (page 184): drive the pre-fill data
pattern `00100100 00011000` through the fill logic with EFE set (and, per the
same page, descending mode selected and BLTCON1 bit 3 clear — see the next
entry) and observe the destination word.

Expected: `00011100 00001000` — "with exclusive fill, the result would be
00011100-00001000" (page 184). The manual also states, as context rather than
as a bit this specification can name, that "exclusive fill yields filled shapes
one pixel narrower than the same pattern filled with inclusive fill" — the
inclusive-fill result for the same input (`00111100 00011000`) is on the same
page but its enabling bit (BLTCON1 bit 3) is one of the undocumented cells
below, so this specification records the inclusive-fill *value* only as
context for the exclusive-fill test, not as a proposition of its own.

### Field: BLTCON1 bit 3, area mode ("IFE") — no claim

Named "Inclusive fill enable" in the page 267 legend, but no claim carries that
meaning forward (see "Table cells that no claim documents"). No proposition.

### Field: BLTCON1 bit 2, area mode ("FCI") — no claim

Named "Fill carry input" in the page 267 legend, but no claim carries that
meaning forward. Figure 6-13 (`ch6-fig613.f27`, page 199) does show a "Fill
Carry In" line into the fill logic from the same decode block that produces the
shift and minterm-select signals, consistent with FCI's legend name — but no
claim ties that figure line to bit 2 specifically or states what value it
should hold, so this remains an open cell rather than a proposition.

### Field: BLTCON1 bit 1, area mode ("DESC")

**Proposition 12.** BLTCON1 bit 1 is DESC, "Descending (decreasing address)
control bit" — it selects decreasing-address (descending) operation for the
blit.
Rests on: `6-appa-control.c30` (bit 1 = DESC, page 267),
`ch6-appa-bltcon0.area.b01.con1`; `6-appa-control.c43`,
`ch6-appa-bltcon0.legend.DESC` (legend, "DESC
Descending (decreasing address) control bit", page 267).

Test: write DESC = 1 and DESC = 0 for an otherwise identical copy blit, and
observe whether the address pointers advance or retreat across successive
words of the blit.

Expected: DESC selects decreasing addresses; the manual as claimed does not
give the specific address-stepping arithmetic here (that is covered, if at
all, by the pointer/modulo registers, out of scope for this document), so this
specification asserts only the direction the bit selects.

---

## Line mode (BLTCON1 bit 0 = 1)

### Field: line start position — BLTCON0 bits 15-12 ("START3-0")

**Proposition 13.** BLTCON0 bits 15, 14, 13 and 12 are START3, START2, START1
and START0 — together "the starting point of the line" (0 through 15 hex), and
the hardware automatically moves this starting bit across the word as the line
is drawn.
Rests on: `6-appa-control.c45` (bit 15=START3, page 268),
`ch6-appa-bltcon0.line.b15.con0`; `6-appa-control.c47` (bit 14=START2),
`ch6-appa-bltcon0.line.b14.con0`; `6-appa-control.c49` (bit 13=START1),
`ch6-appa-bltcon0.line.b13.con0`; `6-appa-control.c51` (bit 12=START0),
`ch6-appa-bltcon0.line.b12.con0`; `6-appa-control.c77` ("START3-0 encodes the
starting position of the line", page 268), `ch6-appa-bltcon0.line.start`;
`6-appa-control.c85` ("line hardware automatically moves the A bit across the
word", page 268).

Test: write START3-0 to a chosen starting bit position, start a line draw with
the A-channel data preloaded as the manual's line-mode setup requires
(Proposition 15), and observe where within the destination word the first "on"
bit of the line appears, and that it moves to the next word as the line
progresses.

Expected: the line's first pixel is positioned at the START3-0 value (0-15);
no claim gives the exact arithmetic beyond "starting point ... 0 thru 15 hex",
so this specification asserts only that relation.

### Field: line texture start bit — BLTCON1 bits 15-12 ("TEXTURE3-0")

**Proposition 14.** BLTCON1 bits 15, 14, 13 and 12 are TEXTURE3, TEXTURE2,
TEXTURE1 and TEXTURE0 — the bit number, within the B-channel texture pattern,
at which to start; the B source itself is used for texturing the drawn line.
Rests on: `6-appa-control.c46` (bit 15=TEXTURE3, page 268),
`ch6-appa-bltcon0.line.b15.con1`; `6-appa-control.c48` (bit 14=TEXTURE2),
`ch6-appa-bltcon0.line.b14.con1`; `6-appa-control.c50` (bit 13=TEXTURE1),
`ch6-appa-bltcon0.line.b13.con1`; `6-appa-control.c52` (bit 12=TEXTURE0),
`ch6-appa-bltcon0.line.b12.con1`; `ch6-appa-lf.c8` ("the B source is used for
texturing the drawn lines", page 268).

Test: preload the B data register with a texture pattern (or $FFFF for a solid
line, per Proposition 15's register summary), write TEXTURE3-0 to a chosen
start bit, draw a line, and observe which bit of the texture pattern appears at
the line's first pixel.

Expected: the texture pattern is read starting at the bit number TEXTURE3-0
names, with bit 0 of that field meaning "the last significant bit" per page
191's B-shift description (cited again under Proposition 15). No claim gives
further arithmetic for how the texture advances along the line, so none is
asserted.

### Fields: channel-enable configuration required for line mode — BLTCON0 bits 11, 10, 9, 8

**Proposition 15.** In line mode, the same four bits that are USEA/USEB/USEC/
USED in area mode are required to hold fixed values: bit 11 = 1, bit 10 = 0,
bit 9 = 1, bit 8 = 1 (the line-mode table prints these as plain constants, "1",
"0", "1", "1"); the descriptive text elsewhere in the manual gives the same
requirement by field name — "the SRCA, SRCC, and DEST bits of BLTCON0 should be
set to one, and the SRCB flag should be set to zero" (page 191), and the
line-mode register summary states it again as "BLTCON0 bits SRCA, SRCC, and
SRCD = 1" and "BLTCON0 bit SRCB = 0" (page 192) — see "Naming conflicts" above
for the DEST/SRCD/USED naming.
Rests on: `6-appa-control.c53` (bit 11=1), `ch6-appa-bltcon0.line.b11.con0`;
`6-appa-control.c57` (bit 9=1), `ch6-appa-bltcon0.line.b09.con0`;
`6-appa-control.c59` (bit 8=1), `ch6-appa-bltcon0.line.b08.con0`;
`6-appa-control.c55` (bit 10=0), `ch6-appa-bltcon0.line.b10.con0` (all page
268); `6-line-mode.c61` (enables source A), `6-line-mode.c62` (enables source
C), `6-line-mode.c63` (enables destination D), `6-line-mode.c64` (disables
source B), `ch6-line.c32` (flagged conflict, same three-bits-to-one statement),
`ch6-line.c33` (SRCB=0) (all page 191); `6-line-mode.c86` (SRCA=1),
`6-line-mode.c87` (SRCC=1), `6-line-mode.c88` (SRCD/DEST=1),
`6-line-mode.c89` (SRCB=0), `ch6-linesum.c13` (flagged conflict, same),
`ch6-linesum.c14` (SRCB=0) (all page 192).

Test: attempt a line draw with BLTCON0 bits 11/10/9/8 set to the required
1/0/1/1 pattern, and separately with a different pattern in those bits, and
observe that only the documented pattern is described as correct for line
drawing. The manual does not state, in any claim on file, what the blitter
does if these bits are set differently in line mode, so this specification
does not assert a specific misbehavior — only that 1/0/1/1 is what the manual
requires.

### Fields: BLTCON1 bits 11, 10, 9, 8, 7 — constants required in line mode

**Proposition 16.** In line mode, BLTCON1 bits 11, 10, 9, 8 and 7 are all
required to be 0 (the table prints "0" for each), and bit 7's counterpart in
BLTCON0 is LF7 (part of the logic-function field, Proposition 18).
Rests on: `6-appa-control.c54` (bit 11=0), `ch6-appa-bltcon0.line.b11.con1`;
`6-appa-control.c56` (bit 10=0), `ch6-appa-bltcon0.line.b10.con1`;
`6-appa-control.c58` (bit 9=0), `ch6-appa-bltcon0.line.b09.con1`;
`6-appa-control.c60` (bit 8=0), `ch6-appa-bltcon0.line.b08.con1`;
`6-appa-control.c62` (bit 7=0), `ch6-appa-bltcon0.line.b07.con1`. All page 268.

This specification separately records that the line-mode register summary also
names a bit "OVFLAG" and requires it to be 0 ("BLTCON1 bit OVFLAG = 0", page
192, `6-line-mode.c93`, `ch6-linesum.c18`; "the OVFLAG should be cleared", page
191, `6-line-mode.c65`, `ch6-line.c34`) — but no claim, and no page read for
this task, states which of BLTCON1's bit positions OVFLAG occupies. It is not
one of the named bits in the line-mode table (which are START/TEXTURE, the
constants above, LF7/0, SIGN, the reserved bit, SUD/SUL/AUL, SING, and LINE).
This specification treats "OVFLAG = 0" as consistent with, but not adding
bit-position information beyond, the constant-0 requirement on bits 11-8 and 7
above; a test must not pick one of those bits and call it OVFLAG without
further evidence.

### Field: logic function — BLTCON0 bits 7-0, line mode ("LF7-0")

**Proposition 17.** As in area mode, BLTCON0 bits 7 through 0 are the LF
control byte (bit 7 = LF7 down to bit 0 = LF0), and in line mode the register
summary gives two specific required values for it, depending on drawing mode:
if exclusive-or line mode, LF = A·B·C̄ + Ā·C (the page prints this with
overbars on the C of the first term and the A of the second — verified
directly against the page image/text: the underscores in
`ABC + AC` sit over the C in "ABC" and the A in "AC"); otherwise (the ordinary,
"cookie-cut" case), LF = A·B + Ā·C (same verification: the single overbar sits
over the A in the second term).
Rests on: `6-appa-control.c61` (bit 7=LF7), `ch6-appa-bltcon0.line.b07.con0`;
`6-appa-control.c63` (bit 6=LF6), `ch6-appa-bltcon0.line.b06.con0`;
`6-appa-control.c65` (bit 5=LF5), `ch6-appa-bltcon0.line.b05.con0`;
`6-appa-control.c67` (bit 4=LF4), `ch6-appa-bltcon0.line.b04.con0`;
`6-appa-control.c69` (bit 3=LF3), `ch6-appa-bltcon0.line.b03.con0`;
`6-appa-control.c71` (bit 2=LF2), `ch6-appa-bltcon0.line.b02.con0`;
`6-appa-control.c73` (bit 1=LF1), `ch6-appa-bltcon0.line.b01.con0`;
`6-appa-control.c75` (bit 0=LF0), `ch6-appa-bltcon0.line.b00.con0`;
`6-appa-control.c79` ("LF7-0 are logic-function minterm selects", page 268);
`6-line-mode.c90` (exclusive-or formula, page 192), `6-line-mode.c91`
(ordinary formula, page 192).

Test: set up a line draw per Proposition 15's channel configuration, write LF
to the exclusive-or formula's byte value and observe that a second draw of the
same line erases the first (the manual's own stated reason for this formula:
"to draw the line using exclusive-or mode, so it can be easily erased by
drawing it again", page 191); write LF to the ordinary formula's byte value and
observe a normal (non-erasing) line.

Open point found on the page but not carried into a proposition: page 191
prints the ordinary-mode formula as "AB + Ā" (only two variables — the
overbar sits over a lone second "A", with no C at all in that term), which
does not match page 192's three-variable "AB + ĀC" for the same case. No claim
in the file quotes the page 191 two-variable form, so it supports no
proposition here, but a reader of this specification should know the manual's
own two mentions of the "ordinary" line formula disagree with each other on
whether C appears in it.

### Field: BLTCON1 bit 6, line mode ("SIGN" / "SIGNFLAG")

**Proposition 18.** BLTCON1 bit 6 is SIGN, the line-mode sign flag, set when
the line's initial error term (4·dy - 2·dx) is negative and cleared otherwise.
Rests on: `6-appa-control.c64` (bit 6=SIGN, page 268),
`ch6-appa-bltcon0.line.b06.con1`; `6-appa-control.c87`,
`ch6-appa-lf.legend.SIGN` (legend, "SIGN is the
line-mode sign flag", page 268); the setup rule naming it SIGNFLAG: "If
(((4*dy) - (2*dx)) < 0): then BLTCON1 bit SIGNFLAG = 1, else ... = 0" (page
192) is part of the register-summary claims already cited for the A pointer
register's initial value, but no claim in this file separately quotes the
SIGNFLAG conditional itself, so this specification records the bit's name and
meaning from the table/legend claims only, and does not assert a proposition
about the conditional's arithmetic.

Test: write BLTCON1 bit 6 = 1 and = 0 for two otherwise-identical line draws
and observe the difference in how the line's Bresenham-style stepping
proceeds. No claim states the stepping algorithm in enough detail to give a
port-level expected sequence, so this specification asserts only the bit's
role (a sign flag for the line's error term), not the stepping it produces.

### Field: BLTCON1 bit 5, line mode ("0 (Reserved)")

**Proposition 19.** BLTCON1 bit 5 is reserved and must be 0 in line mode; the
line-mode legend's "0 Reserved for new mode" line is tied, by the claims on
file, specifically to this bit.
Rests on: `6-appa-control.c66` (bit 5 reserved, must be 0, page 268),
`ch6-appa-bltcon0.line.b05.con1`; `6-appa-control.c88` and
`ch6-appa-lf.legend.reserved` (the legend line "0 Reserved for new mode", page
268).

No claim states whether this "reserved for new mode" characterization also
applies to the other constant-0 bits in the line-mode table (11, 10, 9, 8, 7);
this specification applies it only to bit 5, where the table itself prints the
word "(Reserved)".

Test: write bit 5 = 0 (as required) and confirm normal line-mode operation;
writing bit 5 = 1 has no documented, claimed behavior, so a test must not
assert what it does — only that the manual requires 0.

### Fields: octant selection — BLTCON1 bits 4, 3, 2 ("SUD", "SUL", "AUL")

**Proposition 20.** BLTCON1 bit 4 is SUD ("Sometimes up or down"), bit 3 is SUL
("Sometimes up or left"), and bit 2 is AUL ("Always up or left"); together these
three bits select one of eight octants for line drawing, per the table below
(from page 268):

| Octant | SUD | SUL | AUL |
|---|---|---|---|
| 0 | 1 | 1 | 0 |
| 1 | 0 | 0 | 1 |
| 2 | 0 | 1 | 1 |
| 3 | 1 | 1 | 1 |
| 4 | 1 | 0 | 1 |
| 5 | 0 | 1 | 0 |
| 6 | 0 | 0 | 0 |
| 7 | 1 | 0 | 0 |

Rests on, for the bit identities: `6-appa-control.c68` (bit 4=SUD, page 268),
`ch6-appa-bltcon0.line.b04.con1`; `6-appa-control.c70` (bit 3=SUL, page 268),
`ch6-appa-bltcon0.line.b03.con1`; `6-appa-control.c72` (bit 2=AUL, page 268),
`ch6-appa-bltcon0.line.b02.con1`. For the legend descriptions of the three
names: `ch6-appa-lf.legend.SUD` ("SUD means 'Sometimes up or down
(=AUD*)'"), `ch6-appa-lf.legend.SUL` ("SUL means 'Sometimes up or
left'"), `ch6-appa-lf.legend.AUL` ("AUL means 'Always up or left'") (all
page 268). For the octant table itself:
`ch6-appa-oct0.sud`, `ch6-appa-oct0.sul`, `ch6-appa-oct0.aul`;
`ch6-appa-oct1.sud`, `ch6-appa-oct1.sul`, `ch6-appa-oct1.aul`;
`ch6-appa-oct2.sud`, `ch6-appa-oct2.sul`, `ch6-appa-oct2.aul`;
`ch6-appa-oct3.sud`, `ch6-appa-oct3.sul`, `ch6-appa-oct3.aul`;
`ch6-appa-oct4.sud`, `ch6-appa-oct4.sul`, `ch6-appa-oct4.aul`;
`ch6-appa-oct5.sud`, `ch6-appa-oct5.sul`, `ch6-appa-oct5.aul`;
`ch6-appa-oct6.sud`, `ch6-appa-oct6.sul`, `ch6-appa-oct6.aul`;
`ch6-appa-oct7.sud`, `ch6-appa-oct7.sul`, `ch6-appa-oct7.aul` (all page 268).
Page 192 also states that octant is set via "BLTCON1 bits 4-2 = octant number
from table", but no claim on file quotes that summary line directly, so this
specification relies on the page-268 table claims above for the SUD/SUL/AUL
values themselves.

Test: for each of the eight octants (0-7), set BLTCON1 bits 4/3/2 to the
SUD/SUL/AUL pattern the table gives, draw a line whose (dx, dy) the "normalizing
into octant 0" procedure (page 191) maps to that octant, and observe that the
line is drawn in the correct real-world direction for that octant.

Expected: the SUD/SUL/AUL bit pattern for each octant is exactly the table
above; the legend claims cited give no more geometric meaning of "up", "down",
"left" than the field names themselves already carry ("sometimes up or down",
"sometimes up or left", "always up or left"), so this specification does not
assert a coordinate-system convention beyond those names.

### Field: BLTCON1 bit 1, line mode ("SING" / "ONEDOT")

**Proposition 21.** BLTCON1 bit 1 is SING, "Single bit per horizontal line for
use with subsequent area fill" — also called ONEDOT in the setup prose ("If
only a single bit per horizontal row is desired, the ONEDOT bit of BLTCON1
should be set; otherwise it should be cleared", page 191).
Rests on: `6-appa-control.c74` (bit 1=SING, page 268),
`ch6-appa-bltcon0.line.b01.con1`; `6-appa-control.c89`,
`ch6-appa-lf.legend.SING` (legend, "SING limits
output to a single bit per horizontal line", page 268). The ONEDOT synonym is
read directly from page 191 ("If only a single bit per horizontal row is
desired, the ONEDOT bit of BLTCON1 should be set; otherwise it should be
cleared"), but no claim in the file separately quotes that sentence, so this
specification records the SING name and meaning from the table/legend claims
above and notes ONEDOT only as the synonym found on the page, not as a claim of
its own.

Test: set SING = 1 and draw a line, then set SING = 0 and draw the same line;
observe that with SING = 1 only a single bit per horizontal row is produced
(intended as later input to an area fill, per Proposition 11's fill mechanism),
and with SING = 0 the ordinary (possibly multi-bit-per-row) line is drawn.

### Field: BLTCON1 bit 0, line mode ("LINE(=1)")

Covered under Proposition 1 (mode select). BLTCON0 bit 0 in line mode is LF0
(Proposition 17).

---

## Numbering note

This document contains 22 propositions: numbered 1 through 21, plus 9a as a
named sub-proposition of 9. Three table cells (BLTCON1 bits 2, 3 and 7 in area
mode) are deliberately given no proposition, for the reasons stated where they
occur.
