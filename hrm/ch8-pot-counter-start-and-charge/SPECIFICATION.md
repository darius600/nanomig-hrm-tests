# ch8-pot-counter-start-and-charge — SPECIFICATION

Written from the quotations in
`hrm-testpipeline/work/ch8-s1-build/ch8-pot-counter-start-and-charge.jsonl` (83 claims,
readers `opus` and `sol`) alone. The RTL has not been opened at the time this document is
written. Where a claim needs a port name or signal name to become testable, this document
says so explicitly and leaves the naming to the build step; it does not invent one.

## Oracle and scope

The oracle is the *Amiga Hardware Reference Manual*, printed pages 234, 235, 236 and 282,
as quoted in the claims file. Where a proposition's testable content diverges from what the
fixture actually does, the correct outcome is a **red** assertion, not an adjusted one.

**Module boundary**: `userio.v`, compiled alone. Every claim in the file was classified
`"observable": "yes, at userio"` — nothing here needs a wider boundary.

**Chip set**: every claim in the file carries a `classify.chipset` note. Two shapes occur:

- For the page-234/235/236 claims: the claim's own page names no chip set. Appendix B
  (printed p.289, outside this package's pages, cited only as the already-accepted oracle
  for the `(E)` mark) shows POT0DAT/POT1DAT carrying `(E)` — "changed in the Enhanced Chip
  Set" per the definition on **printed p.288**. `(E)` is a fact about a *different*
  register table than the one on p.234-236 and must not be read back onto this claim's own
  page as if p.234-236 said "ECS" — it does not.
- For the page-282 (Appendix A) claims: the p.282 entry for POT0DAT/POT1DAT itself carries
  a `P( E )` chip-column mark, and p.288 is again the only place `(E)` is defined. p.282
  never states *which* chip set's behaviour it is describing, so this mark is carried as an
  open question, not resolved by silence and not read as "OCS" or "ECS" by assumption.

Both shapes end at the same place for **this package**: the claims file records that the
module has no chip-set-selecting input at all (no `ecs`, `aga`, `a1k`, or `chipset` signal
anywhere at this boundary, contrasted with `denise.v`, which does take such signals). A
fixture instantiating `userio.v` alone cannot select a chip set because there is nothing to
select — the module has exactly one behaviour to grade, and the `(E)` marks on both p.282
and (elsewhere) p.289 do not create a second one to distinguish. This is stated as fact by
the claims file; it is not re-derived from the RTL here, and no `ecs`/`aga` signal name is
asserted because none has been opened yet.

## The port/connector-numbering conflict (claims dated to p.236)

Five claims on p.236 (`opus:8-potreg.c1`, `opus:8-potreg.c2`, `sol:8-pot.c26`,
`sol:8-pot.c27`, `sol:8-pot.c28`, `sol:8-pot.c29` — six, in fact) assert that POT0DAT
carries port-1 data and POT1DAT carries port-2 data. The claims file records that Figure
8-6 (printed p.237, **outside this package's pages**) draws the wiring the other way for
one of the two registers, and that appendix A (printed p.282, **inside** this package,
worded in `LEFT`/`RIGHT` rather than port numbers) is silent on which number goes with
which side. This package does not re-adjudicate that conflict: the claims file already
carries a `conflict_resolution` field resolving it in favour of the p.236 reading, by three
independent measurements recorded against the claims themselves. That resolution is treated
here as part of "your claims" — the evidentiary material handed to this package, not
something derived by this builder opening the RTL before the gate. The assertions below for
p.236 grade the p.236 reading (POT0DAT = port 1 = the "LEFT"/mouse side, POT1DAT = port 2 =
the "RIGHT" side), consistent with how the claims file already resolved it.

## Method

Each group below is one proposition. Every member claim gets its own `expect()` call in the
built package, labelled with its full `classify.uid` — never the bare `id`, never a `-D`
name. A card that is only a byte- or bit-position instance of a table already graded in
full still gets its own call: the coverage ledger counts a uid ASSERTED only if it appears,
whole, on a graded line.

Where a claim's own `classify.split` field marks part of its content not constructible at
this boundary (an absolute physical quantity — ohms, volts, "vertical blanking" as a frame
phase — none of which the fixture has an input for), that half is named and left out of the
assertion; the part that survives unit-stripping is what is graded. That reasoning is
already in the claims file; it is not reopened here.

---

## G1 — Two proportional inputs per port (p.234)
- `opus:8-pot.c1`, `sol:8-pot.c1`
- Quote: "Each of the game controller ports can handle two variable-resistance input
  devices, also known as proportional input devices."
- Assert: each controller port's two counters advance from two distinct, independent
  inputs (the count "two" is graded; that the device is resistive is discarded, ohms have
  no home at this boundary).

## G2 — POTGO write starts a proportional reading (p.234)
- `opus:8-pot.c11`, `sol:8-pot.c12`
- Quote: "During vertical blanking, you write a value into an address called POTGO... this
  register starts the operation of some special hardware that reads the potentiometer
  values..." (split across two adjacent sentences in the source)
- Assert: a write to the POTGO register, and not any other write, is what starts the
  reading. Discarded: "during vertical blanking" — the fixture has no frame/vertical-blank
  phase input, so that half is not constructible here.

## G3 — POTGO=$0001 is the standard-joystick start value (p.234)
- `opus:8-pot.c12`, `sol:8-pot.c13`
- Quote: "For a standard X-Y joystick, this value is hex 0001."
- **Repaired 2026-09-15** after independent validation found the original assertion
  stronger than the claim: it also required $0001 and $00FF to behave identically (not
  stated; p.282 calls bits 7-1 reserved, not don't-care) and required all four counters to
  track an external pin (pot1x has none at all in this design — that is the four-pin
  topology, a different claim's business: p.234's "two per port" sentence, graded in G1, and
  p.282's connector table, graded in G22).
- Assert, narrowed to what the claim says: writing $0001 leaves the counters that do have an
  external gating pin in this design (pot0x, pot0y, pot1y) tracking that pin. Not asserted
  here: $00FF equivalence, or that pot1x has a pin.

## G4 — POTGO start reads the potentiometer values (p.234)
- `opus:8-pot.c13`, `sol:8-pot.c14`
- Quote: "Writing to this register starts the operation of some special hardware that reads
  the potentiometer values..."
- **Repaired 2026-09-15** after independent validation found the original assertion
  stronger than the claim: it also required all four counters to have their own
  independently-drivable external pin, which this sentence does not say — that four-pin
  topology belongs to different claims (p.234's "two per port" sentence, graded in G1, and
  p.282's connector table, graded in G22).
- Assert, narrowed to what the claim says: after the start, each counter that this design
  does gate from an external pin (pot0x, pot0y, pot1y) advances only when that pin is read
  as released, and nothing moves while every gate is held. Not asserted here: that pot1x
  must have a pin of its own.

## G5 — POTGO start zeroes the POT registers (p.234, loose form)
- `opus:8-pot.c14`, `sol:8-pot.c15`
- Quote: "...and sets the values contained in the POT registers (described below) to zero."
- Assert: the same precise condition as G18 below (it is bit 0 of the POTGO write, not the
  write as such, that zeroes all four counters). Graded against the bit-0 form so it cannot
  pass by coincidence of every test POTGO write happening to carry bit 0.

## G6 — Reset interval is seven or eight scan lines (p.235)
- `opus:8-potchg.c1`, `sol:8-pot.c16`
- Quote: "The read circuitry stays in a reset state for the first seven or eight horizontal
  video scan lines."
- Assert: after a POTGO start, the counters do not advance on the first seven or eight
  ticks of the one-count-per-scan-line input, and do advance on the following one. (Count
  of scan lines is dimensionless and survives unit-stripping; graded as a count.)
- **Repaired 2026-09-15**: the built package's original predicate compared the raw
  1-indexed pulse number of the first nonzero read directly against 7-or-8. That pulse
  number is not the quantity the claim names — a first increment at pulse 1 means *zero*
  completed reset lines preceded it, not one. The correct quantity is
  `first_increment_at - 1` (completed reset lines), which is what is now compared against
  7-or-8. The assertion still measures genuinely 0 completed reset lines in this design (no
  separate reset state exists at all — see `test_userio.cpp`'s file header note), so it
  remains red for the same underlying reason, now correctly encoded.

## G7 — Comparison happens once per scan line thereafter (p.235)
- `opus:8-potchg.c4`, `sol:8-pot.c19`
- Quote: "For each horizontal scan line thereafter, the circuit compares the charge on the
  timing capacitor to a preset value."
- Assert: once counting, a counter advances by exactly one count per tick of the
  scan-line-cadence input, never zero and never two, for as long as it is running.
  Discarded: the comparator itself (charge vs. preset) is not at this boundary — the pin
  that gates the count arrives already thresholded.

## G8 — Below-preset charge increments the counter (p.235)
- `opus:8-potchg.c5`, `sol:8-pot.c20`
- Quote: "If the charge is below the preset, the POT counter is incremented."
- Assert: while a counter's gating pin reads "below preset," each tick of the cadence input
  increments that counter by one.

## G9 — Above-preset charge holds the counter until the next POTGO (p.235)
- `opus:8-potchg.c6`, `sol:8-pot.c21`
- Quote: "If the charge is above the preset, the counter value will be held until the next
  POTGO is issued."
- **Repaired 2026-09-15** after independent validation: the original assertion drove the
  counter's digital gate back to "below preset" without an intervening POTGO and demanded
  the counter stay latched anyway. A real RC timing capacitor's charge cannot fall back
  below the preset on its own — nothing lowers it short of a POTGO dump — so "held until
  the next POTGO is issued" is simply the description of a charge that cannot decrease any
  other way, not an independent requirement covering an artificial comparator reversal. The
  page states nothing about that reversal case.
- Assert, narrowed to what's testable: once a counter's gating pin reads "above preset," its
  value does not change on any further tick for as long as the pin keeps reading "above
  preset." (The design's behaviour on an artificial reversal — it resumes counting as soon
  as the gate reopens, POTGO or not — is still measured and printed via `note()` for
  transparency, but is not part of what gates this claim's `expect()` call.)

## G10 — Counters are free to overflow past 255 (p.235)
- `opus:8-potchg.c8`, `sol:8-pot.c24`
- Quote: "Nothing in the system prevents the counters from overflowing (wrapping past a
  count of 255)."
- Assert: driven past 255 counts with no intervening POTGO, a counter wraps back through
  zero with no flag, no saturation and no stop. The counter width itself is measured by
  this package (not assumed from any other package's prior finding) by continuing to pulse
  past the wrap and observing the modulus.

## G11 — Figure 8-5: charging starts eight lines after POTGO (p.235, figure)
- `opus:8-fig8-5.c1`, `sol:8-fig8-5.c2`
- Content (figure, extracted): the callout at the foot of the ramp in Figure 8-5 reads
  "starts eight horizontal lines after POTGO is written" — the precise case of G6's "seven
  or eight" on the same printed page.
- Assert: identical measurement basis to G6 (same reset-interval count), graded against the
  figure's precise "eight" rather than the body text's "seven or eight" — a tighter form of
  the same claim, not a fourth conflicting source.
- **Repaired 2026-09-15**: the built package originally shared G6's exact 7-or-8 predicate
  with this pair too, so it wrongly accepted 7 where the figure specifically says eight.
  Now graded against exactly `completed_reset_lines == 8` (see G6's repair note for the same
  off-by-one fix to the underlying quantity).

## G12 — Figure 8-5: every counter stops at the same threshold (p.235, figure)
- `opus:8-fig8-5.c2`, `sol:8-fig8-5.c3`
- Content (figure, extracted): a single dashed threshold line is drawn for all curves,
  captioned "each pot counter stops when voltage reaches this value."
- Assert, as an equality between four counters' behaviour (dimensionless, the volt is
  discarded): all four counters are governed by the identical stop rule — none has a rule
  of its own.

## G13 — Figure 8-5: higher resistance means a larger count (p.235, figure)
- `opus:8-fig8-5.c4`
- Content (figure, extracted): the lower curve, labelled "for higher resistance," crosses
  the threshold later than the other curve.
- Assert, as a crossing-time-to-count relationship (the resistance value itself is
  discarded — no ohms input at this boundary): holding a counter's gating pin in the
  "above preset" state for part of an equal-length run yields a strictly smaller final
  count than a run where the pin never leaves "below preset" — i.e., a later crossing
  produces a smaller total than an earlier (or no) crossing over the same span.

## G14 — POT0DAT is port 1 data (p.236)
- `opus:8-potreg.c1`, `sol:8-pot.c26` (restated "vertical"), `sol:8-pot.c27` (restated
  "horizontal")
- Quote: "POT0DAT — port 1 data (vertical/horizontal)"
- Assert (per the conflict resolution above): POT0DAT is the register fed by the
  mouse/"LEFT" connector, which appendix A's own figures elsewhere in the chapter label
  port 1.

## G15 — POT1DAT is port 2 data (p.236)
- `opus:8-potreg.c2`, `sol:8-pot.c28` (restated "vertical"), `sol:8-pot.c29` (restated
  "horizontal")
- Quote: "POT1DAT — port 2 data (vertical/horizontal)"
- Assert: POT1DAT is the register fed by the non-mouse/"RIGHT" connector, i.e. port 2.

## G16 — Bits 15-8 hold the Y value (p.236)
- `opus:8-potreg.c3`, `sol:8-pot.c30`
- Quote: "Bits 15-8   POT0Y value or POT1Y value"
- Assert: for both POT0DAT and POT1DAT, bits 15-8 of a read hold that port's Y counter and
  nothing else — a write/change confined to the Y input moves only the high byte.

## G17 — Bits 7-0 hold the X value (p.236)
- `opus:8-potreg.c4`, `sol:8-pot.c31`
- Quote: "Bits 7-0    POT0X value or POT1X value"
- Assert: for both POT0DAT and POT1DAT, bits 7-0 of a read hold that port's X counter and
  nothing else — a change confined to the X input moves only the low byte.

## G18 — Bit-0 POTGO write resets all counts to zero (p.236, precise form)
- `opus:8-potreg.c5`, `sol:8-pot.c32`
- Quote: "All counts are reset to zero when POTGO is written with bit zero high."
- Assert: a POTGO write with bit 0 set zeroes all four counters (not just the register that
  would otherwise be read next); a POTGO write with bit 0 clear leaves all four unchanged.

## G19 — POT0DAT/POT1DAT each read a pair of 8-bit counters (p.282)
- `opus:8-appa-pot.c9`, `sol:8-appa-pot.c11` (POT0DAT), `sol:8-appa-pot.c12` (POT1DAT)
- Quote: "These addresses each read a pair of 8-bit pot counters."
- Assert: reading POT0DAT returns two counters' worth of data, and reading POT1DAT does
  too — four counters' worth of state across the two registers, not shared or aliased.

## G20 — Each pot counter is 8 bits wide (p.282)
- `opus:8-appa-pot.c10`
- Quote: "These addresses each read a pair of 8-bit pot counters." (same sentence as G19,
  a different proposition: the *width*, not the *pairing*)
- Assert: each individual counter's modulus is 2^8 (a count of binary places, dimensionless
  — the "8-bit" claim is not discarded as a physical unit; it survives as a ratio).

## G21 — Four counters total (p.282)
- `opus:8-appa-pot.c11`, `sol:8-appa-pot.c13`
- Quote: "(Four counters total.)"
- Assert: exactly four independently-addressable counters exist across POT0DAT/POT1DAT,
  and they respond independently of one another.

## G22 — The four counters are stopped by two connectors, two pins each (p.282)
- `opus:8-appa-pot.c12`, `opus:8-appa-pot.c13`, `sol:8-appa-pot.c14` (left connector),
  `sol:8-appa-pot.c15` (right connector)
- Quote: "The counters are stopped by signals from two controller connectors (left-right)
  with two pins each."
- Assert: the left connector's two pins each stop one of POT0DAT's two counters, and the
  right connector's two pins each stop one of POT1DAT's two counters — matching the p.236
  and Figure-8-5 pictures of a per-pin stop rule, now tied to specific connectors.

## G23 — Bit assignment table for the pot counters (p.282, table)
- 32 individual-bit claims from `opus` (`opus:8-appa-pot.c14` through `opus:8-appa-pot.c45`,
  one per bit position 15 down to 0, once for the table's "RIGHT" row and once for its
  "LEFT" row) plus 4 byte-level claims from `sol` (`sol:8-appa-pot.c16` RIGHT-Y,
  `sol:8-appa-pot.c17` RIGHT-X, `sol:8-appa-pot.c18` LEFT-Y, `sol:8-appa-pot.c19` LEFT-X) —
  36 claims in total.
- Quote:

      BIT#  15,14,13,12,11,10,09,08  07,06,05,04,03,02,01,00
      ----- ------------------------ -----------------------
      RIGHT Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0
      LEFT  Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0

- Assert, per bit: reading POT1DAT ("RIGHT", per the resolved mapping in G14/G15) at bit N
  for N in 8..15 returns Y-counter bit (N-8), and at bit N for N in 0..7 returns X-counter
  bit N — and correspondingly for POT0DAT ("LEFT"). This is a strictly stronger claim than
  G16/G17 (which only fix which *byte* holds which counter): it fixes bit order within the
  byte too, so each of the 32 individual-bit claims and each of the 4 byte-level claims is
  graded on its own line.

---

## Fixture design notes (not from the manual — carried forward, to be re-measured, not assumed)

- A prior builder on this same fixture measured that the pot counters are declared
  `reg [7:0]` in the RTL and therefore wrap at 256. This package does not import that
  number; G10's assertion is graded by this package's own pulse-and-observe measurement,
  taken independently after the RTL is opened.
- A prior builder measured that JOY0DAT/JOY1DAT idle at $0202, not $0000, on this fixture.
  Nothing in this package's 83 claims concerns JOY0DAT/JOY1DAT directly, but if the harness
  uses either register as part of its "fixture can deliver a known, distinct word" liveness
  proof, it must use the correct idle value and not assume $0000.
- No RTL port, signal or file name is asserted anywhere above; none has been opened yet.

## What happens next

This document is committed before `userio.v` (or any sibling package's files) is opened.
Step 2 opens the RTL to learn the port list needed to instantiate `userio.v` under
Verilator, wires the stimuli described above, and writes one `expect()` call per uid named
in groups G1-G23 (83 calls total), plus the liveness proof and mutation records the brief
requires. Any claim above that turns out not to be constructible once the port list is
known will be declined on a `record()` line with the reason, not silently dropped.
