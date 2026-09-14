# ch8-pot-pins-as-digital-io — specification

Unit: `userio` (fixture: `userio.v`, compiled alone)
Printed pages: 240, 241
Claims: `hrm-testpipeline/work/ch8-s1-build/ch8-pot-pins-as-digital-io.jsonl` (36 claims,
readers `opus` and `sol`, uids `opus:8-dio.c3`..`c41` and `sol:8-dio.c1`..`c40`)

Subject: the four POTGO/POTGOR OUT/DAT bit pairs used as a general-purpose digital I/O
port on the pot pins. Not POTGO's bit *names* (OUTRY/DATRY/OUTRX/DATRX and siblings) —
that is `ch8-potgo-register-bit-layout`, already built and green.

This document is written from the 36 quotations only. `userio.v` and every sibling
`Makefile` are still unopened. No port name, bit offset, internal signal name, or
address width appears below — those belong to the implementation, written after this
file is committed.

## Known conflicts and carried-forward guidance touching this package

- **KNOWN CONFLICT 3**: printed p.233 numbers the controller ports 0 and 1 where p.240
  and the rest of the chapter number them 1 and 2. Two propositions below (Group 2)
  quote "two controller ports"; only the undisputed sub-claim is graded — the four I/O
  lines form two pairs, each pair belonging to one connector — independent of which
  numeral labels which connector.
- **Carried-forward flag**: the build brief states that, measured by the classification
  at this fixture, no input reaches the DAT bits and a claim that an OUT/DAT pair
  reports the pin's input state is an absent signal here. Two propositions below
  (Group 5, `..c33`/`..c31`) are exactly that shape. This package resolves the flag by
  direct measurement against `userio.v`, not by assuming it; if the measurement
  disagrees with the flag, that disagreement will be reported plainly rather than
  silently deferred to either source.
- **`ch8-potgo-register-bit-layout`** already grades which POTGO bit is OUTRY, DATRY,
  OUTRX, DATRX and their siblings, and is green. No proposition below needs a specific
  bit identity to be stated; where the manual text would require one, this document
  uses this chapter's own vocabulary ("a pair's OUT bit", "a pair's DAT bit") and
  leaves which physical bit that is to the already-green sibling.
- p.241's CIAAPRA material (`ch8-ciaa-port-direction`, `ch8-ciaapra-address-window`) is
  not quoted by any of this package's 36 claims — noted for completeness, nothing to
  decline here on that account.

## Propositions

Grouped by what one measurement would settle. Every uid still gets its own graded
(`expect`) or declined (`record`) line in the implementation, cited by uid — including
a uid whose proposition is identical to its twin's.

### Group 1 — POTGO redefines pin function; the register is a 4-bit bidirectional port sharing the pot pins

- `opus:8-dio.c3` (p240) — "The control lines built into the POTGO register (address
  $DFF034) can redefine the functions of some of the controller port pins." →
  *the value written to POTGO's control lines changes what a controller-port pin does.*
- `sol:8-dio.c1` (p240, twin of c3) — same quote. Same proposition.
- `opus:8-dio.c7` (p240) — "...the pot-control register controls a four-bit
  bidirectional I/O port that shares the same four pins as the four pot inputs." →
  *the pot control register controls a four-bit bidirectional I/O port.*
- `sol:8-dio.c5` (p240, twin of c7) — same core quote. Same proposition.
- `opus:8-dio.c8` (p240) — same source sentence as c7, restated → *that four-bit
  bidirectional port uses the same four pins as the four pot inputs.*
- `sol:8-dio.c6` (p240, twin of c8) — same proposition.
- `opus:8-dio.c23` (p240) — "Instead of using the pot pins as variable-resistive
  inputs, you can use these pins as a four-bit input/output port." → *the pot pins can
  be used as a four-bit I/O port instead of as variable-resistive (proportional)
  inputs.*
- `sol:8-dio.c21` (p240, twin of c23) — same proposition.

### Group 2 — Two extra general-purpose I/O pins per controller port (Conflict 3 applies)

- `opus:8-dio.c24` (p240) — "...This provides you with two additional pins on each of
  the two controller ports for general purpose I/O." → *(undisputed sub-claim only)
  the four I/O lines form two pairs, each pair belonging to one connector.*
- `sol:8-dio.c22` (p240, twin of c24) — same undisputed sub-claim.

### Group 3 — Output-enable semantics

- `opus:8-dio.c25` (p240) — "If you set the output enable for any pin to a 1, the
  Amiga disconnects the potentiometer control circuitry from the port..." → *setting a
  pair's output-enable (OUT) bit to 1 disconnects the potentiometer control circuitry
  from that pin.*
- `sol:8-dio.c23` (p240, twin of c25) — same proposition.
- `opus:8-dio.c26` (p240) — "...and configures the pin for output." → *setting the OUT
  bit to 1 configures that pin for output.*
- `sol:8-dio.c24` (p240, twin of c26) — same proposition.
- `opus:8-dio.c27` (p240) — "The state of the data bit controls the logic level on the
  output pin." → *with OUT=1, the pair's DAT bit sets the pin's driven logic level.*
- `sol:8-dio.c25` (p240, twin of c27) — same proposition.

### Group 4 — Register address map

- `opus:8-dio.c28` (p240) — "This register must be written to at the POTGO address..."
  → *the pot control register is written at the POTGO address.*
- `sol:8-dio.c26` (p240, twin of c28) — same proposition.
- `opus:8-dio.c29` (p240) — "...and read from the POTGOR address." → *the pot control
  register is read at the POTGOR address, distinct from POTGO.*
- `sol:8-dio.c27` (p240, twin of c29) — same proposition.

### Group 5 — All-input mode and the proportional counters

- `opus:8-dio.c32` (p240) — "To use the entire register as an input, sensing the
  current state of the pot pins, write all 0s to POTGO." → *writing all 0s to POTGO
  makes every pair an input that senses the pot pins' current state.*
- `sol:8-dio.c30` (p240, twin of c32) — same proposition.
- `opus:8-dio.c33` (p240) — "Thereafter you can read the current state by using
  read-only address POTGOR." → *with POTGO all-0, POTGOR reports the pins' current
  (input) state.* **Carried-forward flag — see above.**
- `sol:8-dio.c31` (p240, twin of c33) — same proposition, same flag.
- `opus:8-dio.c34` (p240) — "Note that bits set as inputs will be connected to the
  proportional counters..." → *a pair left as an input stays connected to its
  proportional counter.*
- `sol:8-dio.c32` (p240, twin of c34) — same proposition.

### Group 6 — Button-input / pull-up recipe

- `opus:8-dio.c35` (p241) — "These lines can also be used for button inputs." → *the
  same lines double as button inputs.*
- `sol:8-dio.c33` (p241, twin of c35) — same proposition.
- `opus:8-dio.c38` (p241) — "...set the proper pin to output, and drive the line high
  (set both OUT... and DAT... to 1)." → *the button-input pull-up is made by setting
  the pin to output **and** driving the line high — the full recipe.*
- `sol:8-dio.c37` (p241, twin of c38, narrower scope) — "To do this, set the proper
  pin to output..." → *configuring the pin as output is the first half of supplying
  the pull-up* (the "drive high" half is c38/sol's own separate claim below; c37 does
  not by itself claim the whole recipe the way opus's c38 does).
- `opus:8-dio.c39` (p241, twin of sol's c38 below) — "...drive the line high (set both
  OUT... and DAT... to 1)." → *"driving the line high" specifically means setting
  both the OUT bit and the DAT bit of that pair to 1.*
- `sol:8-dio.c38` (p241, twin of opus's c39 above) — same proposition.
- `opus:8-dio.c40` (p241) — "Reading POTGOR will produce a 0 if the button is
  pressed..." → *with the pull-up applied, POTGOR's bit reads 0 while the button is
  pressed.*
- `sol:8-dio.c39` (p241, twin of c40) — same proposition.
- `opus:8-dio.c41` (p241) — "...a 1 if it is not [pressed]." → *with the pull-up
  applied, POTGOR's bit reads 1 while the button is not pressed.*
- `sol:8-dio.c40` (p241, twin of c41) — same proposition.

## Testability note

All 36 propositions look testable at the `userio` boundary from an OUT/DAT-pair write,
a POTGOR / proportional-counter read, and an external stimulus on the shared pin — none
of that requires a port name to state. Port names, bit offsets, and the address format
are needed only to *implement* the checks below, and are taken from `userio.v` — opened
only after this file is committed.
