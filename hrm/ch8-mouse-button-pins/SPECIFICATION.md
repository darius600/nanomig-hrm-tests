# ch8-mouse-button-pins

Source: claims file `ch8-mouse-button-pins.jsonl` (11 claims), quoting the
Commodore Amiga Hardware Reference Manual, printed page 232.

Subject: the Amiga mouse's second- and third-button paths at the controller
port (pins 9 and 5), and the module's support for up to three mouse buttons
overall.

Fixture: `userio.v`, compiled alone. No other module is part of this
package's boundary.

This document is written from the claims file's `extract.quote` /
`extract.page` / `extract.restatement` fields only. No RTL file has been
opened yet; port names, bit widths and the exact stimulus/read mechanism are
deliberately not decided here.

## Boundary, decided before any RTL is read

`hrm/ch8-ciaapra-fire-buttons` is already built and green on the `ciaa`
fixture; it grades CIAAPRA bit 6 and bit 7 (the two controller ports'
PRIMARY fire buttons) tracking their own pin, independently, both
directions. Nothing in this package's 11 claims names CIAAPRA, a CIA
register, or the primary/first fire button's own routing -- all 11 are about
button 2 / button 3 (the mouse's second and third buttons) or about the
module's overall button-count support. This package repeats or re-derives no
fact about a primary fire button reaching CIAAPRA; where the "three buttons"
proposition (P1 below) needs to show that even the first button-position bit
has *some* distinct effect inside `userio.v`, it grades only that the effect
is distinct from the other two bits' effects -- it grades no CIAAPRA-facing
semantic about that bit (not "independent of the joystick," not
"active-low," not "means switch closed"). That content is exclusively
`hrm/ch8-ciaapra-fire-buttons`'s, on a different fixture.

That sibling's build also declined two uids, `opus:8-joy2.c3` and
`sol:8-joy.c19` ("a 0 [in the CIAAPRA fire-button bit] indicates the switch
is closed"), because whether a closed switch pulls a CIA input pin low is
controller-port wiring, outside `ciaa`'s boundary. Checked: neither uid
appears in this package's claims file (`ch8-mouse-button-pins.jsonl`; its 11
ids are `8-mbtn.c2`, `8-mbtn.c9`, `8-mbtn.c10`, `8-mbtn.c11`, `8-mbtn.c12`,
`8-mbtn.c13`, `8-button.c2`, `8-button.c8`, `8-button.c9`, `8-button.c10`,
`8-button.c11`). They are not this package's to decide on.

## Propositions

### P1 -- the control circuitry and software support as many as three mouse buttons, not just two (p.232)

Quote (opus, p.232): *"There are two buttons on the standard Amiga mouse.
However, the control circuitry and software support up to three buttons."*
Quote (sol, p.232): *"However, the control circuitry and software support up
to three buttons."*

- `opus:8-mbtn.c2` -- "The control circuitry and the software support as
  many as three mouse buttons."
- `sol:8-button.c2` -- "The control circuitry supports as many as three
  mouse buttons."

Testable at this fixture as: the module accepts (at least) three separate
button-input lines, and each of the three has its own distinct,
non-overlapping observable effect elsewhere in the module -- as opposed to
only two of the three doing anything. This grades the *count* alone; it does
not characterize what any one bit's effect represents (a fire button, a pot
bit, or anything else), and in particular makes no claim about what the
first bit's effect means at any register outside this module -- see
Boundary above.

### P2 -- button 2 reaches the pin-9 path, one of the two "proportional" pins (p.232)

Quote (p.232): *"Button 2 (right button on Amiga mouse) is connected to pin
9 of the controller ports, one of the proportional pins."*

- `opus:8-mbtn.c9` -- "Button 2 is the right button on the Amiga mouse."
- `opus:8-mbtn.c10` -- "Button 2 is connected to pin 9 of the controller
  ports."
- `opus:8-mbtn.c11` -- "Pin 9 of a controller port is one of the
  proportional pins."
- `sol:8-button.c8` -- "The right Amiga mouse button is connected to
  controller-port pin 9."
- `sol:8-button.c9` -- "Controller-port pin 9 is a proportional pin."

Splitting, as the `ciaapra` sibling did for its port-numbering conflict:
"right button" is a housing/marketing label for a button position -- no
signal in a digital module carries handedness, and the page names no test
for it. What is gradeable is the connectivity ("button 2 -> pin 9") and the
pin's distinguishing behavior ("proportional," operationalized below).
`opus:8-mbtn.c9`'s entire restatement is the label alone, with no remaining
testable content once the label is set aside -- declined.
`sol:8-button.c8`'s restatement bundles the label with the connectivity;
this package grades the connectivity half of it ("[a button labelled] is
connected to controller-port pin 9" -> "the module's second button-input
line reaches the pin-9 path") and grades nothing about which housing
position that button occupies.

"Proportional" is not defined by this page's quoted text. Operationalized
from context (paired with P3's "the other proportional controller input,"
implying there are exactly two, and this page's chapter is about the
mouse/joystick controller-port interface): a pin qualifies as this module's
idea of "proportional," for grading purposes, if holding it at its active
level across successive counter-enable pulses advances an accumulating
position value, as opposed to only ever presenting a single static status
bit. This predicate is checked after the RTL is read; if `userio.v` has no
such accumulating mechanism at all, that is a HALT (the manual's term has no
operational referent at this boundary), not a forced pass or fail.

### P3 -- button 3, where present, reaches the pin-5 path, the other proportional pin (p.232)

Quote (p.232): *"Button 3, when used, is connected to pin 5, the other
proportional controller input."*

- `opus:8-mbtn.c12` -- "Button 3, where a mouse provides one, is connected
  to pin 5 of the controller port."
- `opus:8-mbtn.c13` -- "Pin 5 is the other proportional controller input,
  the second of the two."
- `sol:8-button.c10` -- "A third mouse button, when present, is connected
  to controller-port pin 5."
- `sol:8-button.c11` -- "Controller-port pin 5 is the other
  proportional-controller input."

Same shape as P2, one bit position over: connectivity (`c12`, `c10` -- the
module's third button-input line reaches the pin-5 path) and the pin's own
"proportional" behavior (`c13`, `c11`), using the same operational
definition as P2, with the added content that pin 5 is "the other" one --
i.e. its accumulating value is a second, distinct counter from pin 9's, not
the same counter observed twice. No claim in this package's file asserts a
housing label for button 3 (the quote itself calls it only "button 3," not
"left" or any other name), so P3 has no label to split off.

## Declined claims

- `opus:8-mbtn.c9` -- the entire restatement is the "right button" housing
  label; no digital fixture observes handedness, and P2's remaining content
  (connectivity to pin 9, the pin being "proportional") is already graded
  under `opus:8-mbtn.c10` and `opus:8-mbtn.c11`. Declined, not re-measured
  with a weakened restatement.

## Carried forward from `hrm/ch8-ciaapra-fire-buttons` (already built, green, different fixture)

- That package grades CIAAPRA bit 6 / bit 7 (the primary fire buttons)
  tracking their own `porta_in` pin, independently, both directions, on the
  `ciaa` fixture. Nothing here repeats that; P1 above touches at most the
  `userio`-internal effect of the first mouse-button-input bit, and grades
  only that the effect is distinct from the other two bits', never what it
  means at CIAAPRA.
- That package declined `opus:8-joy2.c3` / `sol:8-joy.c19` ("a 0 [in the
  CIAAPRA bit] indicates the switch is closed") as outside `ciaa`'s boundary
  (needs controller-port-to-chip wiring). Checked against this package's own
  claims file: neither uid appears in `ch8-mouse-button-pins.jsonl`. Nothing
  to decide here.

## Assertions planned (uid -> verdict), pending the RTL read

| uid | page | verdict | what is graded |
|---|---|---|---|
| `opus:8-mbtn.c2` | 232 | expect | 3 button-input bits, each with its own distinct, non-overlapping observable effect |
| `sol:8-button.c2` | 232 | expect | twin of `8-mbtn.c2`, graded independently under its own uid |
| `opus:8-mbtn.c9` | 232 | record (decline) | "right button" housing label; not RTL-observable |
| `opus:8-mbtn.c10` | 232 | expect | the second button-input bit reaches the pin-9 path |
| `opus:8-mbtn.c11` | 232 | expect | the pin-9 path is "proportional" (drives an accumulating counter, not just a status bit) |
| `sol:8-button.c8` | 232 | expect | connectivity core of the twin of `8-mbtn.c10`; label half not graded |
| `sol:8-button.c9` | 232 | expect | twin of `8-mbtn.c11` |
| `opus:8-mbtn.c12` | 232 | expect | the third button-input bit reaches the pin-5 path |
| `opus:8-mbtn.c13` | 232 | expect | the pin-5 path is the other, distinct "proportional" counter from pin 9's |
| `sol:8-button.c10` | 232 | expect | twin of `8-mbtn.c12` |
| `sol:8-button.c11` | 232 | expect | twin of `8-mbtn.c13` |

Before any of the above are graded, the fixture must be shown to deliver a
known, distinguishable, non-default word on its relevant input pins and read
it back correctly on each of the two measurement bases used below (a direct
digital read, and a counter-advance) -- a control case, proving the
harness's data path is live rather than silently stuck, and (per the build
brief) every idle bit driven explicitly high, since Verilator defaults an
undriven pin to 0, which on an active-low interface is the pressed/asserted
level.

Port names, bit widths, the exact active level (this page's quotes never
state whether these pins are active-high or active-low; that is read from
the RTL, not assumed), the counter-enable mechanism, and which of the
module's own signals the pin-9/pin-5 paths land on are unknown at this point
and are read from `userio.v` (and whatever sibling files Verilator needs to
elaborate it) only after this document is committed.
