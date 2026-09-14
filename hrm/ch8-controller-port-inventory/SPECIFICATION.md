# ch8-controller-port-inventory

Printed page: 227 (Amiga Hardware Reference Manual, chapter 8).
Fixture: `userio.v`, compiled alone.

This specification is written from the quotations in
`ch8-controller-port-inventory.jsonl` alone, before the RTL has been opened.
Where a test strategy below needs a signal or register name, none is given —
signal names belong to step 2 of the build order, not to this document.

## Subject

Two controller ports exist as independent boundaries, and both accept the
device kinds page 227 lists.

## Quotations (all page 227)

| uid | quote |
|---|---|
| opus:8-intro.c1 | "* Two multiple purpose mouse/joystick/light pen control ports" |
| opus:8-intro.c8 | "Each Amiga has two  nine-pin connectors  that can be used for input or" |
| sol:8-overview.c1 | "Each Amiga has two  nine-pin connectors  that can be used for input or\noutput with a variety of controllers." |
| sol:8-overview.c2 | "Each Amiga has two  nine-pin connectors  that can be used for input or\noutput with a variety of controllers." |
| sol:8-overview.c4 | "Usually, the  nine-pin connectors\nare used with a mouse or joystick but they will also accept input from\nlight pens, paddles, trackballs, and other popular input devices." |
| sol:8-overview.c5 | same quotation as sol:8-overview.c4 |
| sol:8-overview.c7 | same quotation as sol:8-overview.c4 |
| opus:8-intro.c13 | same quotation as sol:8-overview.c4 |
| sol:8-overview.c8 | same quotation as sol:8-overview.c4 |
| opus:8-intro.c14 | same quotation as sol:8-overview.c4 |

Ten claims, six distinct testable propositions. Where two or more uids quote
the identical sentence and restate the identical fact (marked `twin` by the
classifier, or plainly so on inspection), this package grades them as one
proposition cited by all of its uids together, rather than as separate,
identically-measured assertions. That is not the forbidden collective "all
cases passed": each group below is one proposition, quoted more than once by
independent readers, not several unrelated propositions folded together.

## Testable propositions

### P1 — Two independent connectors exist
Cites: `opus:8-intro.c1`, `opus:8-intro.c8`, `sol:8-overview.c1`

The text asserts a cardinality of two, not one connector shared or
multiplexed, and not more than two. At a hardware boundary this is only a
meaningful, falsifiable claim if the two connectors are separately
identifiable *and* mutually non-interfering: a stimulus delivered through one
must not appear as a change of state attributable to the other. If both names
funneled into one shared piece of state, "two" would be a documentation
fiction over one channel.

**Test strategy:** drive a distinct, known stimulus at connector A's inputs
only (connector B left at rest) and confirm (a) connector A's own observable
state changes to the value the stimulus implies, and (b) connector B's
observable state is unchanged from its rest value. Repeat with the roles
reversed. Both directions must hold.

Connectors are called "A" and "B" here — arbitrary, order-free labels. Page
227 does not number them. KNOWN CONFLICT 3 (printed p.233 numbers the ports 0
and 1; printed p.240 and the rest of the chapter number them 1 and 2) is not
engaged by this proposition: it asserts cardinality and independence, not a
numbering, and grading it does not require choosing a side in that conflict.

### P2 — A connector is capable of carrying input
Cites: `sol:8-overview.c2`

Distinct from P1: a connector could be an independently addressable piece of
state (satisfying P1) while being permanently tied off and never actually
responding to a stimulus — two dead, independent things are still two
independent things. This proposition requires driving a connector's input and
observing the module's own output move to a value consistent with that
stimulus, not merely "differ from before."

**Test strategy:** drive one connector with a stimulus distinct from its rest
state and confirm the observable output takes on the value that stimulus
implies.

### P3 — Accepts mouse input
Cites: `sol:8-overview.c4`

An Amiga mouse reports motion as two independently-phased pulse trains
(quadrature) per axis, driving an up/down count; it is not a level-based
device like a joystick switch.

**Test strategy:** drive a sequence of quadrature transitions on a connector's
two-phase pins in one phase order for a known number of steps N and confirm
the corresponding count moved by +N; drive the same transitions with the
phase order reversed and confirm the count moved by -N.

### P4 — Accepts joystick input
Cites: `sol:8-overview.c5`

A digital joystick reports discrete direction/fire state as switch closures,
not as a moving count — level-driven, not counted.

**Test strategy:** starting from an all-open rest state, close one
directional switch input at a time and confirm the corresponding status bit
reads asserted while the others read deasserted; release and confirm all
deassert.

### P5 — Accepts paddle input
Cites: `sol:8-overview.c7`, `opus:8-intro.c13`

An Amiga paddle is a potentiometer whose position is measured by timing how
long a capacitor takes to charge through it, relative to a synchronised start
pulse — a proportional (analog-timed) encoding, distinct from both the switch
encoding (P4) and the quadrature encoding (P3).

**Test strategy:** issue the capture/start stimulus, hold the paddle's sense
input at the level meaning "still charging" for a known number of periodic
count-enable pulses, and confirm the corresponding count has advanced by
exactly that number.

### P6 — Accepts trackball input
Cites: `sol:8-overview.c8`, `opus:8-intro.c14`

A trackball is electrically the same interface as a mouse — paired quadrature
phases per axis. The manual lists it as a separate accepted device on the
same connectors; the module cannot distinguish a mouse from a trackball at
the electrical level. This proposition is graded by the same kind of
quadrature-acceptance measurement as P3, run as its own independent stimulus
and its own read-back, not by re-using P3's result.

**Note on light pens:** the same sentence backing P3–P6 also names light pens
as an accepted device kind, but no claim in this package's ten covers light
pens — a light pen is a beam-position-timing device on a different signal
path than the joystick/mouse pins. This package makes and grades no
light-pen proposition.

## Control / precondition requirement

A green P1 (independence) is worthless if the harness cannot first prove it
can deliver a distinct, known word through the same read path used for
grading — a data path that is silently dead on both connectors would also
read as "independent," for the wrong reason. Before P1 is graded, the harness
must drive connector A alone and connector B alone, each with its own
distinct known pattern, and show each connector's own observable state takes
on the expected value. These control checks are not derived from a page-227
quotation; they exist to rule out a dead data path producing a false-clean
independence result, on the same failure mode a chapter-6 package lost a
harness to.

## Absent-signal review

Every proposition above resolves, in principle, to something a controller
port module would need to expose to do its job at all: two input groups, a
proportional-count enable, and read-back state. None of the six needs a
physical connector attribute (pin count, mechanical shell) with no
corresponding module-boundary signal — "two connectors" and "nine-pin" are
kept separate by the classifier (the nine-pin descriptor is a different claim,
`opus:8-intro.c9`, not in this package's ten and not graded here). If RTL
reading in step 2 turns up a proposition among these six that in fact needs a
signal the module does not expose, it will be declined with `record()`,
naming what would carry it, rather than widening the fixture past `userio.v`
alone.

## Assertion count

Six graded propositions (P1–P6) plus at least two ungraded-by-manual-claim
control/precondition checks (known-word delivery on each connector, required
by P1). Ten uids are cited across the six propositions; none is left
unaddressed.
