# Specification: ch7-vertb-vertical-blank-timing

Fixture: `agnus_beamcounter`. Subject: the VERTB vertical-blank interrupt
(bit 5) at display line 0, and the vertical-blanking line range.

Written from the quotations in `ch7-vertb-vertical-blank-timing.jsonl` alone.
No RTL has been opened to produce this document. Every claim is cited by its
`uid` (`<reader>:<id>`), never by the bare `id`, per BUILD-BRIEF.

## Chipset scope

Chapter 7's body is the OCS account (printed p.224 hands the ECS system
control registers to Appendix C). All five propositions below are therefore
scoped to OCS, asserted with `ecs=0` per the task sheet.

One proposition (P1) is stated by both readers as unchanged under ECS: no
INTENA/INTENAR/INTREQ/INTREQR entry exists anywhere in Appendix C, so the
interrupt-controller behaviour it describes is not chipset-dependent and the
same fixture run answers for OCS and ECS alike.

The other four (P2-P5) are explicitly OCS-only: Appendix C p.305 gives
BEAMCON0 bits HARDDIS (bit 14, "Disable hardwired vertical/horizontal
blank") and VARVBEN (bit 12) the power to make the vertical-blank range
programmable. A measurement of that programmable behaviour is out of scope
here; it belongs to `ch7-beamcon0-ecs-control-bits` (which already measured
VARVBEN moving the range from 21 lines to 5). This package asserts only the
hardwired OCS range and must run with the chip's ECS-control bits at their
OCS default (`ecs=0`), and must say so in its assertion labels.

## Testable propositions

### P1 -- VERTB fires exactly at display line 0

> "Bit 5, VERTB, causes an interrupt at line 0 (start of vertical blank) of
> the video display frame." (printed p.219)

Testable form: the video-line counter reaches the value the manual calls
"line 0" exactly once per frame, and the condition the manual calls VERTB
becomes true at that line and at no other line in the frame.

Applies to OCS and ECS alike (no ECS override exists for this behaviour).

Cited by: `sol:7-interrupts.c34`, `opus:7-int.c31` (explicit twins, same
proposition, both taken).

Co-cited, denotation only: `opus:7-fig7-4.c18` (Figure 7-4, printed p.221)
reads the row whose fourth column is "VERTB" and whose Description cell
reads "vertical blank interval" -- i.e. the figure names VERTB as denoting
the same condition P1 tests. The same figure row also carries an Exec
software priority ("6") and a hardware priority ("3") for VERTB; those two
values are a claim about the CPU interrupt-priority encoder, which sits in
`paula_intcontroller`, not in `agnus_beamcounter`, and are out of this
package's boundary (see "Not turned into assertions" below). Only the
naming/denotation half of `opus:7-fig7-4.c18` is folded into P1.

### P2 -- NTSC minimum vertical blanking is 20 horizontal scan lines

> "The minimum time of vertical blanking is 20 horizontal scan lines for an
> NTSC system and 25 horizontal scan lines for a PAL system." (printed
> p.219, NTSC clause)

Testable form (ratio, not absolute -- a scan-line count is a duration over a
line duration, dimensionless, and countable directly as a number of line
boundaries in an NTSC-timed frame): the hardwired vertical-blanking duration
in an NTSC frame is 20 lines.

OCS only (`ecs=0`).

Cited by: `sol:7-interrupts.c35`, `opus:7-int.c32` (opus's quotation was
extracted short and its own crossread step restored it to the full
sentence above; both name the same NTSC clause).

### P3 -- PAL minimum vertical blanking is 25 horizontal scan lines

> "The minimum time of vertical blanking is 20 horizontal scan lines for an
> NTSC system and 25 horizontal scan lines for a PAL system." (printed
> p.219, PAL clause)

Testable form: the hardwired vertical-blanking duration in a PAL frame is 25
lines.

OCS only (`ecs=0`).

Cited by: `sol:7-interrupts.c36`, `opus:7-int.c33` (same restoration as P2).

### P4 -- The NTSC vertical-blank range runs from line 0 to line 20

> "The range starts at line 0 and ends at line 20 for NTSC or line 25 for
> PAL." (printed p.219, NTSC clause)

Testable form: in an NTSC-timed frame, the condition the manual calls
vertical blanking holds for every line from line 0 through line 20
inclusive, and does not hold for a line clearly outside that range (this
package's control case, drawn directly from the range being stated as
bounded -- "starts... ends..." -- rather than open-ended).

OCS only (`ecs=0`).

Cited by: `sol:7-interrupts.c37`, `opus:7-int.c34` (opus's quotation
restored to the full sentence).

### P5 -- The PAL vertical-blank range runs from line 0 to line 25

> "The range starts at line 0 and ends at line 20 for NTSC or line 25 for
> PAL." (printed p.219, PAL clause)

Testable form: in a PAL-timed frame, the condition the manual calls vertical
blanking holds for every line from line 0 through line 25 inclusive, and
does not hold for a line clearly outside that range.

OCS only (`ecs=0`).

Cited by: `sol:7-interrupts.c38`, `opus:7-int.c35` (same restoration as P4).

## Control case

BUILD-BRIEF requires at least one control case expected to PASS on the same
measurement basis as any case under suspicion. P4 and P5 each supply their
own: the same line-counting measurement that checks the range's endpoint
also checks a line well outside the range (both below the range is
impossible, since it starts at 0; and clear of the upper end) reads as NOT
blanking. Because NTSC (20) and PAL (25) give different expected boundary
values from the same fixture and the same measurement, P2/P4 running
against P3/P5 on the same code, expecting different numbers, is itself a
cross-check against a hard-coded constant that ignores the PAL/NTSC
selection.

## Precondition to assert

Per BUILD-BRIEF's fixture-integrity rule, a green on P1-P5 depends on this
fixture's video-line counter actually counting lines in order and reaching
the specific values the propositions name (0, 20, 25) rather than, say,
being stuck or wrapping early. Where that has never been shown, it does not
get to stay an assumption: the package must assert, as its own precondition
proposition, that the counter reaches a sequence of distinct, known line
numbers in order over the course of a frame, before any P1-P5 assertion is
trusted. This precondition is not sourced from a manual quotation -- it is
the fixture-delivers-a-known-word rule applied to a line counter instead of
a data word -- so it is asserted as an engineering precondition, not cited
to a page.

## Not turned into assertions

- The part of `sol:7-interrupts.c34` / `opus:7-int.c31` describing "the
  vblint input lands on request bit 5" (i.e. that the VERTB condition is
  wired to bit 5 of the interrupt request register): this needs
  `paula_intcontroller`, where `vblint` is a free input with no
  beam-position port of its own, so the line-0 timing cannot be read
  there. That signal belongs to a package built against
  `paula_intcontroller` (or the interrupt-register/priority packages), not
  to `agnus_beamcounter`. Recorded, not asserted, in this package.
- The Exec-software-priority-6 / hardware-priority-3 half of
  `opus:7-fig7-4.c18`'s Figure 7-4 row: this is a claim about the CPU
  interrupt-priority encoder in `paula_intcontroller`, out of this
  fixture's boundary for the same reason as above. Recorded, not asserted.

## Addendum (written during step 3, before any assertion was coded): resolving the P2/P4 and P3/P5 off-by-one

Page 219 states, in the same paragraph: "The minimum time of vertical
blanking is 20 horizontal scan lines for an NTSC system and 25 horizontal
scan lines for a PAL system. The range starts at line 0 and ends at line 20
for NTSC or line 25 for PAL. After the minimum vertical blanking range, you
can control where the display actually starts..."

Read inclusively, "starts at line 0 and ends at line 20" spans 21 lines,
one more than "is 20 horizontal scan lines" states two sentences earlier.
The page does not otherwise say which of the two governs whether line 20
(NTSC) / line 25 (PAL) itself still counts as blanking -- both readings are
grammatically available and the third sentence ("after the ... range you
can control...") is consistent with either.

This is decided per-proposition rather than punted whole:

- P2 and P3 ("minimum time ... is N horizontal scan lines") state a plain
  count, with no dependency on which specific line is the boundary. They
  are asserted as counts: the number of lines for which blanking holds, in
  a frame, equals 20 (NTSC) / 25 (PAL). Unambiguous, fully testable.
- P4 and P5 ("the range starts at line 0 and ends at line N") are asserted
  only for what both readings of "ends at" agree on: blanking holds at
  line 0 (the unambiguous start), and blanking has ended by one line past
  the disputed boundary (line 21 for NTSC, line 26 for PAL -- outside the
  range under either reading, and the control case BUILD-BRIEF requires,
  on the same measurement basis as the assertion above it).
- The disputed line itself (20 for NTSC, 25 for PAL) is measured and
  reported (`record()`), not graded -- see the test's two boundary records.

## Claims accounted for

All 11 claims in `ch7-vertb-vertical-blank-timing.jsonl` are accounted for
above: 5 are folded into P1 (2 claims) through P5 (2 claims each, 8 claims
total across P2-P5) plus the denotational half of the 11th
(`opus:7-fig7-4.c18`, folded into P1), whose priority-encoder half, together
with the request-bit-5 half of claims 1 and 6, is recorded rather than
asserted.
