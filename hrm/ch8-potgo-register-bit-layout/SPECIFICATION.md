# Specification — `ch8-potgo-register-bit-layout`

Package: `hrm/ch8-potgo-register-bit-layout`
Fixture: `userio.v` (compiled alone)
Claims file: `hrm-testpipeline/work/ch8-s1-build/ch8-potgo-register-bit-layout.jsonl` (81 claims)
Subject: POTGO's and POTGOR's bit layout — the eight OUT/DAT cells for the
four pot lines, the START bit, and what each bit position means.

This file is written from the `extract.quote` / `restatement` fields of the
81 claims, cross-read against the primary sources those claims cite —
printed pages 240 and 282, and Figure 8-6 on page 237 — **alone**. No RTL
file for this package has been opened. It fixes the printed propositions to
be tested; port names, concrete stimulus values, and the `expect()`
assertions themselves are added only after this file is committed and step 2
(opening `userio.v`) begins, per the build brief.

`userio.v` has never had a test package before this one: there is no sibling
harness in this suite to copy for this fixture, so the fixture-liveness proof
(§ "On a control case and mutation") has to be built from nothing rather than
adapted from an existing one.

## The three source pages

- **p.240** — Chapter 8, "Digital I/O On The Controller Port." Table 8-4
  (`POTGO ($DFF034) and POTGOR ($DFF016) Registers`) plus the paragraph
  immediately before it and the two paragraphs immediately after it. This is
  the primary source for every bit's name and function.
- **p.282** — Appendix A, register summary, the `POTGO`/`POTGOR` entry. A
  second printing of the same table, in different words, immediately after
  the `POT0DAT`/`POT1DAT` entry.
- **p.237** — Figure 8-6, "Potentiometer Charging Circuit." A schematic, not
  running text (`pdftotext` cannot read it reliably — the figure image itself
  is the source; see "What this file does not contain" for how it was read).
  It draws POTGO as a 16-cell register (BIT 15 left, BIT 0 right) and POTGOR
  (labelled `POTINP` in the figure, its older name per p.240) as a second,
  read-only row of cells underneath it.

## Claim inventory (all 81)

`FRAMING` = folded into PROP-1..8 (see "General framing"); `PROP-n` = scored
proposition n; the four `OUT-OF-SCOPE`/`NOT-TESTABLE`/`COMPOSITE`/`FOLDED`
tags are explained in "Claims not assigned an assertion" below.

| # | uid | page | assigned to | # | uid | page | assigned to |
|---|---|---|---|---|---|---|---|
| 1 | `opus:8-dio.c9` | 240 | PROP-1 | 42 | `sol:8-appa-pot.c40` | 282 | PROP-1 |
| 2 | `opus:8-dio.c10` | 240 | PROP-1 | 43 | `sol:8-appa-pot.c41` | 282 | PROP-2 |
| 3 | `opus:8-dio.c11` | 240 | PROP-2 | 44 | `sol:8-appa-pot.c42` | 282 | PROP-3 |
| 4 | `opus:8-dio.c12` | 240 | PROP-3 | 45 | `sol:8-appa-pot.c43` | 282 | PROP-4 |
| 5 | `opus:8-dio.c13` | 240 | PROP-4 | 46 | `sol:8-appa-pot.c44` | 282 | PROP-5 |
| 6 | `opus:8-dio.c14` | 240 | PROP-5 | 47 | `sol:8-appa-pot.c45` | 282 | PROP-6 |
| 7 | `opus:8-dio.c15` | 240 | PROP-6 | 48 | `sol:8-appa-pot.c46` | 282 | PROP-7 |
| 8 | `opus:8-dio.c16` | 240 | PROP-11 | 49 | `sol:8-appa-pot.c47` | 282 | PROP-8 |
| 9 | `opus:8-dio.c17` | 240 | PROP-7 | 50 | `sol:8-appa-pot.c48` | 282 | PROP-9 |
| 10 | `opus:8-dio.c18` | 240 | PROP-8 | 51 | `sol:8-appa-pot.c49` | 282 | PROP-9 |
| 11 | `opus:8-dio.c19` | 240 | PROP-12 | 52 | `sol:8-appa-pot.c51` | 282 | PROP-10 |
| 12 | `opus:8-dio.c20` | 240 | PROP-9 | 53 | `opus:8-fig8-6r.c7` | 237 | OUT-OF-SCOPE-COUNTER |
| 13 | `opus:8-dio.c22` | 240 | PROP-10 | 54 | `opus:8-fig8-6r.c8` | 237 | OUT-OF-SCOPE-ADDR |
| 14 | `sol:8-dio.c7` | 240 | PROP-1 | 55 | `opus:8-fig8-6r.c9` | 237 | OUT-OF-SCOPE-ADDR |
| 15 | `sol:8-dio.c8` | 240 | PROP-1 | 56 | `opus:8-fig8-6r.c10` | 237 | COMPOSITE |
| 16 | `sol:8-dio.c9` | 240 | PROP-2 | 57 | `opus:8-fig8-6r.c11` | 237 | PROP-10 |
| 17 | `sol:8-dio.c10` | 240 | PROP-3 | 58 | `opus:8-fig8-6r.c13` | 237 | NOT-TESTABLE-NAME |
| 18 | `sol:8-dio.c11` | 240 | PROP-4 | 59 | `opus:8-fig8-6r.c14` | 237 | PROP-13 |
| 19 | `sol:8-dio.c12` | 240 | PROP-5 | 60 | `opus:8-fig8-6r.c15` | 237 | PROP-2 |
| 20 | `sol:8-dio.c13` | 240 | PROP-6 | 61 | `opus:8-fig8-6r.c16` | 237 | FOLDED-PAIRING |
| 21 | `sol:8-dio.c14` | 240 | PROP-11 | 62 | `opus:8-fig8-6r.c17` | 237 | FOLDED-TOPOLOGY |
| 22 | `sol:8-dio.c15` | 240 | PROP-7 | 63 | `sol:8-fig8-6r.c9` | 237 | OUT-OF-SCOPE-COUNTER |
| 23 | `sol:8-dio.c16` | 240 | PROP-8 | 64 | `sol:8-fig8-6r.c10` | 237 | OUT-OF-SCOPE-ADDR |
| 24 | `sol:8-dio.c17` | 240 | PROP-12 | 65 | `sol:8-fig8-6r.c11` | 237 | OUT-OF-SCOPE-ADDR |
| 25 | `sol:8-dio.c18` | 240 | PROP-9 | 66 | `sol:8-fig8-6r.c12` | 237 | PROP-1 |
| 26 | `sol:8-dio.c20` | 240 | PROP-10 | 67 | `sol:8-fig8-6r.c13` | 237 | PROP-2 |
| 27 | `opus:8-appa-potgo.c10` | 282 | FRAMING | 68 | `sol:8-fig8-6r.c14` | 237 | PROP-3 |
| 28 | `opus:8-appa-potgo.c11` | 282 | FRAMING | 69 | `sol:8-fig8-6r.c15` | 237 | PROP-4 |
| 29 | `opus:8-appa-potgo.c12` | 282 | PROP-1 | 70 | `sol:8-fig8-6r.c16` | 237 | PROP-5 |
| 30 | `opus:8-appa-potgo.c13` | 282 | PROP-2 | 71 | `sol:8-fig8-6r.c17` | 237 | PROP-6 |
| 31 | `opus:8-appa-potgo.c14` | 282 | PROP-3 | 72 | `sol:8-fig8-6r.c18` | 237 | PROP-7 |
| 32 | `opus:8-appa-potgo.c15` | 282 | PROP-4 | 73 | `sol:8-fig8-6r.c19` | 237 | PROP-8 |
| 33 | `opus:8-appa-potgo.c16` | 282 | PROP-5 | 74 | `sol:8-fig8-6r.c21` | 237 | PROP-10 |
| 34 | `opus:8-appa-potgo.c17` | 282 | PROP-6 | 75 | `sol:8-fig8-6r.c22` | 237 | FOLDED-TOPOLOGY |
| 35 | `opus:8-appa-potgo.c18` | 282 | PROP-7 | 76 | `sol:8-fig8-6r.c23` | 237 | NOT-TESTABLE-NAME |
| 36 | `opus:8-appa-potgo.c19` | 282 | PROP-8 | 77 | `sol:8-fig8-6r.c24` | 237 | PROP-2 |
| 37 | `opus:8-appa-potgo.c20` | 282 | PROP-9 | 78 | `sol:8-fig8-6r.c25` | 237 | PROP-4 |
| 38 | `opus:8-appa-potgo.c21` | 282 | PROP-9 | 79 | `sol:8-fig8-6r.c26` | 237 | PROP-6 |
| 39 | `opus:8-appa-potgo.c23` | 282 | PROP-10 | 80 | `sol:8-fig8-6r.c27` | 237 | PROP-8 |
| 40 | `sol:8-appa-pot.c38` | 282 | FRAMING | 81 | `sol:8-fig8-6r.c28` | 237 | PROP-13 |
| 41 | `sol:8-appa-pot.c39` | 282 | FRAMING | | | | |

All 81 claims accounted for: 65 claims scored across 13 propositions
(PROP-1 through PROP-13; several claims — opus/sol twins and cross-page
corroborations — land on the same proposition), 4 folded into the
general-framing note below, and 12 not assigned an assertion, with reasons
given in "Claims not assigned an assertion" (2 out-of-scope pot-counter
claims, 4 out-of-scope address/width claims, 2 not-testable register-name
claims, 1 composite claim resolved by the propositions it composes, and 3
folded claims that state no fact beyond an already-scored proposition).

## General framing (both pages agree)

> The pot-control register controls a four-bit bidirectional I/O port that
> shares the same four pins as the four pot inputs.
— p.240

> This register controls a 4-bit bi-directional I/O port that shares the
> same four pins as the four pot counters above.
— p.282 (`opus:8-appa-potgo.c10`/`c11`, `sol:8-appa-pot.c38`/`c39`)

And the mechanism, stated only on p.240, immediately after Table 8-4:

> If you set the output enable for any pin to a 1, the Amiga disconnects the
> potentiometer control circuitry from the port, and configures the pin for
> output. The state of the data bit controls the logic level on the output
> pin. This register must be written to at the POTGO address, and read from
> the POTGOR address. [...] To use the entire register as an input, sensing
> the current state of the pot pins, write all 0s to POTGO. Thereafter you
> can read the current state by using read-only address POTGOR.
— p.240

These two framing claims (`opus:8-appa-potgo.c10`, `c11`, `sol:8-appa-pot.c38`,
`c39`) name no bit position of their own, so — by the same reasoning a sibling
chapter-7 package applied to its own page-287 legend — they are not scored as
an independent proposition. Their content is realized jointly by PROP-1
through PROP-8 below: each of those pairs an output-enable bit with its data
bit and is tested by exactly this "OUT=1 drives, OUT=0 senses" mechanism,
which is what "bidirectional I/O port" means operationally.

## PROP-1 through PROP-8 — the eight OUT/DAT bit positions

Table 8-4, p.240 (primary; the reserved chip-revision/chip-ID row 07-01 is
handled separately as PROP-9, and the START row as PROP-10):

```
       Bit
       Number  Name    Function
       ------  ----    --------
       15      OUTRY   Output enable for bit 14 (1=output)
       14      DATRY   data for port 2, pin 9
       13      OUTRX   Output enable for bit 12
       12      DATRX   data for port 2, pin 5
       11      OUTLY   Output enable for bit 10
       10      DATLY   data for port 1, pin 9 (right  mouse button )
       09      OUTLX   Output enable for bit 8
       08      DATLX   data for port 1, pin 5 (middle  mouse button )
```

Corroborated on p.282 (same eight bits, physical-pin wording instead of
port/pin wording):

```
                 BIT#  FUNCT   DESCRIPTION
                 ----  ---------------------------------------
                 15    OUTRY   Output enable for Paula pin 36
                 14    DATRY   I/O data Paula pin 36
                 13    OUTRX   Output enable for Paula pin 35
                 12    DATRX   I/O data Paula pin 35
                 11    OUTLY   Output enable for Paula pin 33
                 10    DATLY   I/O data Paula pin 33
                 09    OUTLX   Output enable for Paula pin 32
                 08    DATLX   I/O data Paula pin 32
```

And corroborated on p.237 (Figure 8-6): the sixteen POTGO cells, read left
(BIT 15) to right (BIT 0), are labelled, in order: `OUTRY DATRY OUTRX DATRX
OUTLY DATLY OUTLX DATLX` followed by seven cells marked `X` (bits 7 down to
1) and then `START` (bit 0). Directly below, POTGOR's own row of cells
(labelled `POTINP` in the figure) shows `0` under OUTRY's column, `RY` under
DATRY's (with the digit "14" printed beneath it, anchoring that column to bit
14), `0` under OUTRX's, `RX` under DATRX's, `0` under OUTLY's, `LY` under
DATLY's, `0` under OUTLX's, `LX` under DATLX's, and one wide `0` cell
spanning the remaining low-order bits (7 down to 0). Dotted vertical lines
connect each `DATxx` cell in the POTGO row to the matching `xy`/`xx` cell in
the POTGOR row directly beneath it.

Two pages name the same physical pin two different ways (p.240: "port 2, pin
9"; p.282: "Paula pin 36") — this is not a conflict, it is the same pin
identified by connector position versus by package pin number, and both
namings corroborate the same bit-to-pin association. The Paula package-pin
number itself (36, 35, 33, 32) is not something a test at this fixture's
boundary can observe — a port carries no package-pin identifier — so it is
not part of what is asserted; what is asserted is that the bit occupies its
position and plays its role (enable, or data for the pin its partner
enables), consistent across p.240, p.282, and the figure.

| Prop | Bit | Name | Role | p.240 wording | p.282 wording |
|---|---|---|---|---|---|
| PROP-1 | 15 | OUTRY | output enable, paired with bit 14 | "Output enable for bit 14 (1=output)" | "Output enable for Paula pin 36" |
| PROP-2 | 14 | DATRY | data bit, paired with bit 15's enable | "data for port 2, pin 9" | "I/O data Paula pin 36" |
| PROP-3 | 13 | OUTRX | output enable, paired with bit 12 | "Output enable for bit 12" | "Output enable for Paula pin 35" |
| PROP-4 | 12 | DATRX | data bit, paired with bit 13's enable | "data for port 2, pin 5" | "I/O data Paula pin 35" |
| PROP-5 | 11 | OUTLY | output enable, paired with bit 10 | "Output enable for bit 10" | "Output enable for Paula pin 33" |
| PROP-6 | 10 | DATLY | data bit, paired with bit 11's enable | "data for port 1, pin 9 (right mouse button)" | "I/O data Paula pin 33" |
| PROP-7 | 9 | OUTLX | output enable, paired with bit 8 | "Output enable for bit 8" | "Output enable for Paula pin 32" |
| PROP-8 | 8 | DATLX | data bit, paired with bit 9's enable | "data for port 1, pin 5 (middle mouse button)" | "I/O data Paula pin 32" |

**Proposition to test, per pair (PROP-1/2, PROP-3/4, PROP-5/6, PROP-7/8):**
with the enable bit (OUTxx) written 1, the paired data bit's (DATxx) write
value controls the driven state of that pair's one physical line; with the
enable bit written 0, that same physical line is left as an input and
reading the data bit's position on POTGOR reflects the line's external
state. Each enable bit (PROP-1, 3, 5, 7) is tested by observing that
*only* its own paired line responds to it (an enable set alone must not
move any of the other three lines' readback). Each data bit (PROP-2, 4, 6,
8) is tested by observing both halves of the mechanism above for its own
line, and that writing it does not move any other line's readback.

Status: testable, all eight — each independently, per the build brief's "one
assertion per bit position, never one per table": a decoder that mis-wires
one bit fails exactly one of these eight, not all of them together.

## PROP-9 — bits 7 through 1 (the p.240/p.282 conflict)

p.240 Table 8-4:

> 07-01   X       chip revision identification number

p.282 Appendix A:

> 07-01   0     Reserved for chip ID code (presently 0)

**The conflict, and its resolution.** The two pages disagree about what
these seven bits *are*: p.240 calls them a populated field carrying a "chip
revision identification number," p.282 calls them an unpopulated field
"reserved... presently 0." Both pages agree on the one thing that matters for
a test: neither page assigns these bits any pot-port I/O function (no pin, no
output enable, no data bit lives there, unlike bits 15-8) — the row exists in
the same table only to account for the bit positions between the I/O nibble
and START.

**Why the assertion is written in the p.282 form.** A test at this boundary
can only observe a value: whatever bits 7-1 of POTGOR read back as, under
whatever stimulus is applied to POTGO. A constant 0 is consistent with
*both* readings — "reserved, presently 0" (p.282) and "revision 0" (p.240,
if revision 0 happens to be the chip being modelled) — because a revision
register that always reads 0 and a reserved field that always reads 0
produce the identical waveform. No stimulus in POTGO's input space can turn
one reading into the other; the disagreement is about the field's *name and
purpose*, not about a difference in observable behaviour, so it cannot be
adjudicated by a digital test. The assertion is therefore written in the
p.282 form, as an observable: **bits 7 through 1 of POTGOR read 0, in every
configuration exercised, regardless of what is written to POTGO.** This
tests the fact both pages agree on (no I/O function lives there; the
constant is 0) without asserting either page's disputed characterization of
*why* it is 0.

Status: testable (as the observable-0 proposition above). Whether a stimulus
mutation exists for it depends on whether any write path reaches these seven
bits at all — undecidable before opening the RTL; see the mutation section
below.

## PROP-10 — bit 0, START

p.240:

> 00      START   Start pots (dump capacitors, start counters)

p.282, word-for-word the same function text:

> 00    START   Start pots (dump capacitors, start
>                                  counters)

Figure 8-6 (p.237) corroborates the bit position: the rightmost POTGO cell,
under the "BIT 0" label, is `START`.

**Proposition to test:** bit 0 of POTGO is named START and its write value
triggers the "start pots" function described identically on both pages —
setting it dumps the pot capacitors and starts the counters associated with
the four pot lines. The manual does not give the digital detail of what
"dump capacitors" means in a synthesizable model; the testable content is
only what both pages state in words — that writing this bit is what starts
(or restarts) the counting, as opposed to the counting running
unconditionally or not at all. The concrete counter behaviour to observe
(which counters, what "restart" reads as at this module's boundary) is
fixed once the module's ports are known, in step 2 — not before, per the
"no port names before the specification is committed" rule.

Status: testable.

## PROP-11 / PROP-12 — the two bits with a stated input identity

p.240 gives bits 10 and 8 an extra clause the other six rows do not carry:

> 10      DATLY   data for port 1, pin 9 (right  mouse button )
> 08      DATLX   data for port 1, pin 5 (middle  mouse button )

This parenthetical is a distinct testable proposition from PROP-6/PROP-8
(which test only that the bit *is* DATLY/DATLX and behaves as a data bit): it
additionally names *what drives that line when it is left as an input* — the
right and middle mouse buttons, respectively. A wiring bug could get the
bit-position/data-bit role right while wiring the wrong physical signal to
it (or vice versa), so these are graded separately.

| Prop | Bit | Claim |
|---|---|---|
| PROP-11 | 10 (DATLY) | with bit 11 (OUTLY) = 0 (pin left as input), reading bit 10 of POTGOR reflects the right mouse button's state |
| PROP-12 | 8 (DATLX) | with bit 9 (OUTLX) = 0 (pin left as input), reading bit 8 of POTGOR reflects the middle mouse button's state |

Status: testable, both — contingent on `userio.v` exposing a mouse-button
input at all (to be confirmed in step 2; if it does not, this is a HALT for
these two propositions specifically, not for the package).

## PROP-13 — POTGOR's unpopulated bit positions read 0

Figure 8-6 (p.237) shows POTGOR's own row of cells (labelled `POTINP`,
p.240's note that POTGOR "formerly called POTINP" identifies this as the
same register): under the four output-enable columns (OUTRY, OUTRX, OUTLY,
OUTLX — bits 15, 13, 11, 9) the figure prints `0`, not a label, and only the
four data columns (DATRY→RY, DATRX→RX, DATLY→LY, DATLX→LX, i.e. bits 14, 12,
10, 8) carry a live value. Bit 0 (under START) falls inside the same wide
"0" cell as bits 7-1.

**Proposition to test:** reading POTGOR, bits 15, 13, 11, 9, and 0 are
always 0, regardless of what is written to POTGO — only bits 14, 12, 10, and
8 vary with input. This is a distinct proposition from PROP-9 (which covers
bits 7-1 specifically, and carries the p.240/p.282 naming conflict); PROP-13
covers the four write-only control bits (the enables and START) that the
table text alone does not say read back as 0 — only the figure shows it.

Status: testable.

## On a control case and mutation

The build brief requires (a) a fixture-liveness proof preceding any
data-dependent grading, (b) at least one control case expected to PASS on
the same basis as anything under suspicion, and (c) a stimulus mutation per
assertion, recorded in the required `// MUTATION <label> <oracle|stimulus|
none> <red|green|impossible> <what changed>` form. Concrete stimulus values
and port names are step-2 material and are deliberately not fixed here, but
the shape of each is already visible from the claims alone:

- **Fixture liveness.** `userio.v` has never been measured before. Before
  any of PROP-1 through PROP-13 is graded, the harness must write at least
  two distinct, known 16-bit words to POTGO and confirm — by some
  POTGO-write-dependent effect visible at the boundary (most simply: two
  different OUTxx/DATxx patterns driving two different, observably distinct
  states) — that both distinct words actually reached the module and did not
  silently collapse to one value or to nothing. This is infrastructure, not
  a manual proposition, and is written up front in the test file before any
  `expect()` that depends on it.
- **Control case.** One representative OUT/DAT pair (the plan is bits
  9/8 — OUTLX/DATLX, the pair PROP-7/PROP-8/PROP-12 already cover) exercised
  through the same write/read mechanism as every other pair is the control:
  if the harness's own write-then-read path is not proven live on this
  basis, no protected-or-not finding about any other bit means anything.
  PROP-7/PROP-8's own PASS is that control, on the same measurement basis as
  every other pair — no separate, unscored control harness is needed.
- **Mutation, PROP-1 through PROP-8.** For an enable/data pair, the natural
  stimulus mutation redirects the *bit position* under test to a different
  bit belonging to a different pair while leaving the expected value and the
  assertion's label alone — e.g. PROP-1 (bit 15 governs bit 14) mutated by
  driving bit 13 instead of bit 15 and checking bit 14's readback: this must
  go red, because bit 13 does not govern bit 14. This uses only facts this
  file already establishes (which bit belongs to which pair) — no RTL fact.
- **Mutation, PROP-11/PROP-12.** Redirect the stimulus to the *other*
  button (drive the middle button and check PROP-11's bit, or the right
  button and check PROP-12's bit) — must go red, distinct buttons must not
  cross-affect each other's bit.
- **Mutation, PROP-9 and PROP-13.** Both assert a constant readback (0)
  regardless of stimulus. Whether a stimulus mutation is *possible* for
  either is undecided until step 2: if no write path in `userio.v` reaches
  these bit positions at all (which p.282's own "presently 0" wording
  suggests, for PROP-9 at least), no stimulus can move them and the correct
  record is `// MUTATION PROP-9 none impossible <reason>` — and per the
  build brief, that reason must say *which* kind of impossible it is
  ("unfalsifiable at this boundary" vs. "not exercised, by a scope
  decision"), decided against what step 2 actually shows, not assumed here.
- **Mutation, PROP-10 (START).** The natural stimulus mutation is to omit
  writing the START bit and confirm the "start" side effect does not occur
  (or occurs only relative to whatever baseline step 2 establishes) — exact
  form fixed once the counter-observing ports are known.

This section states a strategy, not the assertions themselves; the actual
`expect()` calls, their concrete stimulus values, and the `// MUTATION`
header lines are written in step 2, after `userio.v` is opened.

## Claims not assigned an assertion

Sixteen of the 81 claims are not made into their own scored proposition: the
4 general-framing claims already addressed above, and the 12 detailed here.
None is discarded silently; each reason is stated here so step 2 does not
have to re-derive it.

**Framing, folded into PROP-1 through PROP-8 (4 claims):**
`opus:8-appa-potgo.c10`, `c11`, `sol:8-appa-pot.c38`, `c39` — the general
"4-bit bidirectional I/O port" statement. See "General framing" above: it
names no bit position of its own; its content is realized by PROP-1..8
jointly.

**Out of scope — pot-counter clocking behaviour, not POTGO/POTGOR bit
layout (2 claims):** `opus:8-fig8-6r.c7`, `sol:8-fig8-6r.c9` — Figure 8-6's
POT1Y line feeding the POT1Y counter through a Schmitt-triggered buffer. This
describes how the pot-counter module (a different subject) uses one of these
four lines once it is configured as an input; it says nothing about what a
POTGO/POTGOR bit *is* or *means*, which is this package's assigned subject.
Additionally, the analogue hysteresis the figure draws is not something a
digital fixture models at all.

**Out of scope — a different register's or the same register's own
address/width identity, not a bit-field meaning (4 claims):**
`opus:8-fig8-6r.c8` (POT1DAT at $DFF014 — a different register entirely),
`sol:8-fig8-6r.c10` (twin), `opus:8-fig8-6r.c9` (POTGO at $DFF034, write-only,
16 bits wide), `sol:8-fig8-6r.c11` (twin). "Which address a register lives
at" and "how many bits wide it is" are not "what a bit position means" — the
subject line's own scope ("what each bit position means") is about the
sixteen individual bit positions' functions, not the register's own
location. A single combinational module tested standalone, without the
containing chip's address decoder, is also unlikely to make an address fact
observable at all — to be confirmed, not assumed, in step 2.

**Not testable — a register's own name is a document fact with no signal
counterpart (2 claims):** `opus:8-fig8-6r.c13`, `sol:8-fig8-6r.c23` — that
the figure labels the $DFF016 register `POTINP`. No port carries a register
name; this is settled by comparing the manual's own words (p.240: "POTGOR
(formerly POTINP)") against each other, not by any measurement this fixture
could make.

**Composite claim, resolved by PROP-1 through PROP-8 individually (1
claim):** `opus:8-fig8-6r.c10` — one claim recording all eight POTGO bit
labels (15 down to 8) at once. Per the build brief's "one assertion per bit
position, never one per table," this composite is not itself turned into one
assertion; it is exactly what PROP-1 through PROP-8 already cover, one bit at
a time.

**Folded — DAT-bit/readback pairing, not a new fact beyond PROP-2/4/6/8 (1
claim):** `opus:8-fig8-6r.c16` — the dotted lines in Figure 8-6 connecting
each POTGO DATxx cell to the POTGOR cell at the same bit position. PROP-2,
PROP-4, PROP-6, and PROP-8 each already test writing a DATxx bit and reading
it back at that same bit position; this claim states no fact beyond what
those four already require to be true.

**Folded — enable+data jointly drive one line; only the topology portion is
excluded (2 claims):** `opus:8-fig8-6r.c17`, `sol:8-fig8-6r.c22` — Figure
8-6 draws OUTRY and DATRY feeding an unlabelled group of three triangular
buffers that in turn drives the POT1Y line. The testable half — bits 15 and
14 jointly control that line — is exactly PROP-1 and PROP-2 combined; no
separate assertion is needed for it. The topology half — which of the three
drawn triangles does what — could not be resolved from the image even by
the claim's own prior reading (per each claim's own `split` field), so no
page-only proposition can be written for it regardless of what `userio.v`
turns out to expose; that portion is excluded as unresolvable from the
source material, not merely unscored. (Each claim's own `split` field also
notes, from its own earlier RTL reading, that this module has no pot output
port for a driver network to exist against — that is their finding, not
verified here, and not what this exclusion rests on.)

## What this specification deliberately does not contain

Port names, signal names, and concrete stimulus values are absent by design,
per the build brief's order of work — they are added only after this file is
committed and `userio.v` is read for step 2.

Figure 8-6 (p.237) is a scanned schematic; `pdftotext`'s reading of it
(`docs/references/amiga-hrm-3rd/ch8/p237.txt`) states plainly that it is
unreliable for a figure and directs the reader to the rendered image
(`docs/references/amiga-hrm-3rd/ch8/figures/p237.png`). Every claim drawn
from p.237 in this file was checked directly against that rendered image, not
against the unreliable OCR text layer, before being written up above.

Nothing in this file names or infers a Verilog port, wire, or line number.
Where a claim's own evidence field (in the 81-claim source file) cited
`userio.v` line numbers or signal names as part of how an earlier classifier
established the claim was testable, that RTL-derived detail was read (the
claims file could not be parsed for uid/page/quote without encountering it)
but is deliberately not reproduced or relied upon anywhere in this
specification — every proposition above is stated, and can be tested,
without it. This is disclosed rather than concealed: the claims file handed
to this package already carries that content; nothing here re-derives an
expected value from it.
