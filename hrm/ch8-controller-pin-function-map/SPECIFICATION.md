# ch8-controller-pin-function-map — SPECIFICATION

Source: *Amiga Hardware Reference Manual*, 3rd ed., page 228, Table 8-1 ("9-Pin
D Female Connector Pinouts", chapter 8) and its footnote.

Claims: `hrm-testpipeline/work/ch8-s1-build/ch8-controller-pin-function-map.jsonl`,
42 claims, two independent readers (`opus`, `sol`), 21 twin pairs. Every uid below
is `classify.uid` verbatim, not the bare `id`.

Fixture: `userio.v`, compiled alone. This document is written from the
quotations in the claims file only. No RTL file has been opened yet — no port
name, signal name or bit position appears below. Where the exact device/register
names below (JOY0DAT, POTGOR, POT0DAT/POT1DAT) are used, they are Amiga register
names from the manual's own vocabulary (chapter 6's custom-chip register set),
not RTL signal names; they are used only to say *which documented register* a
behaviour is expected to surface in, not what it is called inside the module.

## Table 8-1 as quoted in the claims

Five device-type columns, read off the restatement fields that accompany each
quote: **Joystick**, **Mouse/Trackball/Driving-Controller**, **Proportional
Controller (Pair)**, **X-Y Proportional Joystick**, **Light Pen**.

| Pin | Joystick | Mouse/TB/DC | Prop. Pair | XY Prop. Joystick | Light Pen |
|---|---|---|---|---|---|
| 1 | Forward | V-pulse | --- | Button 3** | --- |
| 2 | Back | H-pulse | --- | --- | --- |
| 3 | Left | VQ-pulse | Left button | Button 1 | --- |
| 4 | Right | HQ-pulse | Right button | Button 2 | --- |
| 5* | --- | Middle button** | Right POT | POT X | Pen pressed to screen |
| 6* | Button 1 | Left button | --- | --- | Beam trigger |
| 9* | Button 2** | Right button | Left POT | POT Y | Button 2** |

Footnote (quoted): `*  These pins may also be configured as outputs`, printed
against the rows for pins 5, 6 and 9.

Pins 7 and 8 carry no row in Table 8-1 (ground/+5V on the physical connector)
and no claim in the file names them; out of scope by construction, not by
decision.

Only cells with a claim in the 42-line file are addressed below. A `---` cell,
or a populated cell with no corresponding uid (e.g. pin 1's "Button 3**", pin
3/4's Prop-Pair and XY-Joystick button cells, pin 6's "Beam trigger", pin 6's
own asterisk-output footnote), is neither asserted nor declined — it was not
handed to this package as a claim, and this package does not invent claims the
readers did not extract.

## KNOWN CONFLICT 3 — checked, does not bind here

The dispute is whether the controller ports are numbered 0/1 (p.233) or 1/2
(pp.240ff). None of the 42 restatements name a port index — every one reads
"the [Joystick/Mouse/...] column carries X" or "controller pin N carries Y",
about the pin-to-function map that is identical on both physical ports. No
assertion below depends on which numbering is used, so this package takes no
position on it.

## Composed facts, one row per uid, verdict and reason

### Group A — pins 1-4, direct-read vs. quadrature (8 facts, 16 uids, ASSERT)

Each of these four pins carries two independently-structured behaviours
depending on which device type is plugged in: a joystick reports a simple
switch closure (a level), a mouse/trackball/driving-controller reports a
quadrature pulse train. These are not competing names for one signal (unlike
Group C below) — they are two different signal *shapes* that a digital fixture
can tell apart without knowing which device is attached, so both members of
each pair are asserted, not declined.

| uid | pin | column | quote (p.228) | claim |
|---|---|---|---|---|
| `opus:8-tbl8-1.c1` | 1 | Joystick | "1 Forward V-pulse --- Button 3** ---" | pin 1 = joystick forward |
| `sol:8-pin.c1` | 1 | Joystick | same | pin 1 = joystick forward |
| `opus:8-tbl8-1.c2` | 1 | Mouse/TB/DC | same | pin 1 = mouse/TB/DC V-pulse |
| `sol:8-pin.c2` | 1 | Mouse/TB/DC | same | pin 1 = mouse/TB/DC V-pulse |
| `opus:8-tbl8-1.c4` | 2 | Joystick | "2 Back H-pulse --- --- ---" | pin 2 = joystick back |
| `sol:8-pin.c5` | 2 | Joystick | same | pin 2 = joystick back |
| `opus:8-tbl8-1.c5` | 2 | Mouse/TB/DC | same | pin 2 = mouse/TB/DC H-pulse |
| `sol:8-pin.c6` | 2 | Mouse/TB/DC | same | pin 2 = mouse/TB/DC H-pulse |
| `opus:8-tbl8-1.c6` | 3 | Joystick | "3 Left VQ-pulse Left button Button 1 ---" | pin 3 = joystick left |
| `sol:8-pin.c7` | 3 | Joystick | same | pin 3 = joystick left |
| `opus:8-tbl8-1.c7` | 3 | Mouse/TB/DC | same | pin 3 = mouse/TB/DC VQ-pulse |
| `sol:8-pin.c8` | 3 | Mouse/TB/DC | same | pin 3 = mouse/TB/DC VQ-pulse |
| `opus:8-tbl8-1.c10` | 4 | Joystick | "4 Right HQ-pulse Right button Button 2 ---" | pin 4 = joystick right |
| `sol:8-pin.c11` | 4 | Joystick | same | pin 4 = joystick right |
| `opus:8-tbl8-1.c11` | 4 | Mouse/TB/DC | same | pin 4 = mouse/TB/DC HQ-pulse |
| `sol:8-pin.c12` | 4 | Mouse/TB/DC | same | pin 4 = mouse/TB/DC HQ-pulse |

**What will be measured, functionally (bit/port names filled in after the RTL
is opened, per the order of work):**

- *Joystick-mode direct read* (pins 1/2/3/4 = forward/back/left/right): with
  the fixture presented as a joystick port, asserting the electrical line the
  connector routes to pin *N* makes exactly the direction indication that
  Table 8-1 names for pin *N* become true in the port's decoded state, while
  the indications named for the other three pins of the same axis pair stay
  false; releasing it returns all four to false. Four pins share one
  experiment shape, run once per pin.
- *Mouse-mode quadrature* (pins 1&3 = vertical pair carrying V/VQ, pins 2&4 =
  horizontal pair carrying H/HQ): with the fixture presented as a
  mouse/trackball/driving-controller port, driving a two-phase sequence on one
  pair advances that axis's counter in the port's mouse-motion register by the
  number of transitions driven, while the other axis's counter is unchanged;
  the sign of the advance is set by which of the pair's two lines leads.

### Group B — pins 5/9, proportional-controller pot gating (2 facts, 8 uids, ASSERT)

For these four cells, the two column names given for the same pin are not two
different signal shapes — they are two conventions for naming the *same*
potentiometer-timing byte (a proportional-controller pair calls pin 5's byte
"Right POT" and pin 9's "Left POT"; an X-Y proportional joystick calls the same
two bytes "POT X" and "POT Y"). Which byte gets gated is a structural fact
independent of which naming convention a caller uses, so both names for each
pin are asserted, not declined — unlike Group C, where the two names describe
mutually exclusive external devices with no such shared structural fact.

| uid | pin | column | claim |
|---|---|---|---|
| `opus:8-tbl8-1.c15` | 5 | Prop. Pair | pin 5 = Right POT |
| `sol:8-pin.c17` | 5 | Prop. Pair | pin 5 = Right POT |
| `opus:8-tbl8-1.c16` | 5 | XY Prop. Joystick | pin 5 = POT X |
| `sol:8-pin.c18` | 5 | XY Prop. Joystick | pin 5 = POT X |
| `opus:8-tbl8-1.c32` | 9 | Prop. Pair | pin 9 = Left POT |
| `sol:8-pin.c35` | 9 | Prop. Pair | pin 9 = Left POT |
| `opus:8-tbl8-1.c33` | 9 | XY Prop. Joystick | pin 9 = POT Y |
| `sol:8-pin.c36` | 9 | XY Prop. Joystick | pin 9 = POT Y |

**What will be measured:** holding pin 5's line low for a timed interval, with
the port's pot-start/charge cycle running, advances (gates) one specific byte
of that port's two-byte pot data register — the byte Table 8-1 calls "Right
POT"/"POT X" — while pin 9's line gates the other byte ("Left POT"/"POT Y"),
distinguishably: driving pin 5 alone must move one byte and not the other, and
driving pin 9 alone the reverse. All four uids per pin are graded against the
same measured byte-selection fact.

### Group C — pins 5/9, device-identity button/pen claims (10 uids, DECLINE)

| uid | pin | column | quote | claim |
|---|---|---|---|---|
| `opus:8-tbl8-1.c14` | 5 | Mouse/TB/DC | "5* --- Middle button** Right POT POT X Pen pressed to screen" | pin 5 = mouse/TB/DC middle button |
| `sol:8-pin.c15` | 5 | Mouse/TB/DC | same | pin 5 = mouse/TB/DC middle button |
| `opus:8-tbl8-1.c17` | 5 | Light Pen | same | pin 5 = light-pen "pen pressed to screen" |
| `sol:8-pin.c19` | 5 | Light Pen | same | pin 5 = light-pen "pen pressed to screen" |
| `opus:8-tbl8-1.c30` | 9 | Joystick | "9* Button 2** Right button Left POT POT Y Button 2**" | pin 9 = joystick button 2 |
| `sol:8-pin.c32` | 9 | Joystick | same | pin 9 = joystick button 2 |
| `opus:8-tbl8-1.c31` | 9 | Mouse/TB/DC | same | pin 9 = mouse/TB/DC right button |
| `sol:8-pin.c34` | 9 | Mouse/TB/DC | same | pin 9 = mouse/TB/DC right button |
| `opus:8-tbl8-1.c34` | 9 | Light Pen | same | pin 9 = light-pen button 2 |
| `sol:8-pin.c37` | 9 | Light Pen | same | pin 9 = light-pen button 2 |

**Decline reason (`record()`):** for these cells, every column printed against
the pin is a plain digital button/switch reading — Table 8-1 gives the same
pin four or five *mutually exclusive device names* for what is, on the wire, a
single sensed level with no structural difference between "it was a mouse
middle-button" and "it was a light pen's pressed-to-screen line": both are just
"this pin read low". A digital fixture that has no notion of what is plugged
into the D-connector cannot adjudicate *which* named device produced a given
level — that is a fact about external wiring, not about `userio.v`'s behaviour.
This is the mirror case of Group B: there, the two names shared a structural
fact (which byte) that IS observable; here they do not. Declined, one `record()`
line per uid, naming the pin and the reason above. (Contrast with pin 6 in
Group D below: pin 6's two button names are declined for a *different* reason —
scope, not observability.)

### Group D — pin 6, fire-button function (4 uids, DECLINE — wrong fixture)

| uid | pin | column | quote | claim |
|---|---|---|---|---|
| `opus:8-tbl8-1.c18` | 6 | Joystick | "6* Button 1 Left button --- --- Beam trigger" | pin 6 = joystick button 1 |
| `sol:8-pin.c20` | 6 | Joystick | same | pin 6 = joystick button 1 |
| `opus:8-tbl8-1.c19` | 6 | Mouse/TB/DC | same | pin 6 = mouse/TB/DC left button |
| `sol:8-pin.c21` | 6 | Mouse/TB/DC | same | pin 6 = mouse/TB/DC left button |

**Decline reason (`record()`):** pin 6 is Table 8-1's fire line ("Button 1" /
"Left button" are the same fire contact under two device names). Per the
carried-forward chapter guidance, fire-line claims belong to
`ch8-ciaapra-fire-buttons` on the `ciaa` fixture, not to `userio.v`. Declined by
name, one `record()` line per uid, citing `ch8-ciaapra-fire-buttons` as the
package that owns this claim.

### Group E — pins 5/9, output-configurability footnote (2 facts, 4 uids, ASSERT)

| uid | pin | quote | claim |
|---|---|---|---|
| `opus:8-tbl8-1.c35` | 5 | "* These pins may also be configured as outputs" | pin 5 can be configured as an output |
| `sol:8-pin.c39` | 5 | same | pin 5 can be configured as an output |
| `opus:8-tbl8-1.c37` | 9 | same | pin 9 can be configured as an output |
| `sol:8-pin.c41` | 9 | same | pin 9 can be configured as an output |

**What will be measured:** for each of pin 5 and pin 9, the fixture can drive
that same electrical line to a chosen logic level under its own control (not
only sense an externally-driven level) — i.e., the line is bidirectional, not
input-only. Two facts (pin 5, pin 9), each graded against both readers' uids.

## Claim accounting

28 ASSERT (Groups A, B, E) + 14 DECLINE (Groups C, D) = 42. Every uid in the
42-line claims file appears exactly once above.

## Test plan

- **Fixture liveness, before any claim is graded.** Two distinct known words
  are driven into `userio.v` on unrelated ports and their arrival at two
  distinct, unrelated outputs is confirmed, so a stuck-at data path cannot
  masquerade as a passing pin-function assertion. Graded under its own
  non-claim label (`fixture-liveness`), not a `classify.uid`.
- **Control case.** One of the Group A joystick-mode direct-read assertions
  (the simplest, least interpretive shape in this file: a level in, a level
  decoded out) is the control case — same fixture, same measurement basis as
  the quadrature and pot-gating assertions it sits beside. Which specific pin
  serves as control is recorded in the built test's file header once the RTL
  is open and the assertion is confirmed to hold; if none of the four holds,
  that is itself reported rather than silently substituted.
- **Mutation policy.** Every ASSERT uid gets its own `// MUTATION <uid> ...`
  line, the uid used verbatim as the `expect()` label. RTL is never touched;
  a mutation changes what the *testbench* drives — e.g., asserting the
  neighbouring pin's line instead of the one under test, reversing a
  quadrature pair's lead/lag order, or driving the other port's counterpart
  line — confirming the assertion goes red, then reverting. Where two uids
  (an opus/sol twin, or all four uids in a Group B pot-gating fact) grade the
  identical measured boolean, one physical mutation that flips that boolean
  is run once and recorded once per affected label, since it genuinely does
  make every one of those labels go red together.
  - If a stimulus mutation cannot be built for a given assertion because no
    input in `userio.v`'s boundary can affect it, the assertion is
    unfalsifiable at this boundary — labelled `impossible` with that phrase.
  - If an assertion is already red against the manual (an RTL deviation), its
    mutation record substitutes a different, already-proven-live mechanism
    through the same comparator to show the comparator itself is not vacuous,
    per the brief.
- **Declined claims.** Every Group C and Group D uid gets a `record()` line
  naming the pin, the column, and the reason (device-identity not observable
  at this boundary, or wrong fixture — `ch8-ciaapra-fire-buttons`).

## HALT check

No manual ambiguity was found at the point under test; no claim needs the port
list or a measurement to be settled at the specification stage (Groups B and C
were separated on the quotations' own structure — shared byte-naming vs.
mutually-exclusive device-naming — not on anything RTL-derived); the assigned
boundary (`userio.v` alone) is sufficient for every ASSERT group above; no file
outside this unit directory has been touched. No HALT.

---
*Written before any RTL file was opened. Port names, bit positions and exact
register field widths are filled in only after this specification is
committed.*
