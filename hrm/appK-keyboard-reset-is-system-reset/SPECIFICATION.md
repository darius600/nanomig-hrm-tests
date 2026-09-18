# appK-keyboard-reset-is-system-reset

Amiga Hardware Reference Manual, 3rd edition, Appendix K, p.411 ("System
Halt (/HLT)" / "System Control Signals"). Claims file:
`hrm-testpipeline/work/appK-build/appK-keyboard-reset-is-system-reset.jsonl`,
2 claims (sol:K-0435, opus:K-0441), both `status: testable`,
`boundary: hid`.

**Order of work, stated plainly.** The task assignment already named the
top module (`hid`) and gave a specific RTL citation (`hid.v:30,159-171`,
the LCTRL+LAMIGA+RAMIGA combo raising `kbd_reset` for one cycle) before
this file was written. That context could not be un-seen. What follows is
nonetheless argued from each claim's own `quote`/`restatement` fields; the
byte-serial protocol, HID codes and pass/fail conditions below are step-3
work, decided only after this section was drafted.

## STANDING QUALIFIER -- read this before the results

**`hid` lives in `nanomig/src/misc`, NanoMig's own peripheral-substitute
layer, not the shared Minimig chipset lineage that `paula`, `agnus` and
`denise` belong to.** Upstream Minimig has no counterpart to this file at
all -- it is NanoMig's own USB/MCU keyboard-and-mouse front end. A result
graded here is a fact about *this* substitute peripheral, on *this*
board. It does not transfer to "the chipset" the way a `paula` or `agnus`
result would, and should not be read or cited as if it did. This
qualifier applies to both claims below regardless of how green or red
they read.

## What the two claims assert

- **sol:K-0435** -- quote "asserted on a powerup reset or a keyboard
  reset", restatement "Power-up and keyboard resets are full-system
  resets."
- **opus:K-0441** -- quote "asserted on a powerup reset or a keyboard
  reset; any PIC that needs to differentiate between", restatement "A full
  reset is asserted on a powerup reset or a keyboard reset."

Both are the same compound proposition: (a) a *keyboard* reset mechanism
exists and (b) it produces a full-system reset, the same kind a power-up
produces.

## Boundary

`hid` alone can only speak to half of this compound claim: whether
pressing the printed key combination produces a reset-shaped pulse
(`kbd_reset`, hid.v:30). Whether that pulse then becomes *the same signal*
as the power-up reset is downstream wiring in `minimig_syscontrol.v`
(outside this one-top-module fixture, and outside `src/misc` entirely --
squarely the chipset-adjacent lineage the STANDING QUALIFIER above says
this package's own boundary cannot speak for). That half is measured as
not-exercised by this fixture (a `note()`, not a `record()` decline --
the uid's own restatement is about the reset mechanism existing and
firing, which the fixture does grade; the "merges into one global signal"
elaboration is in `classify.evidence`, not in either quote).

## What will be driven and observed

Fixture: `hid` alone. Keyboard events are delivered through its MCU-facing
byte-serial protocol (`data_in_strobe`/`data_in_start`/`data_in[7:0]`),
CMD 1 ("keyboard data"): a start byte selecting command 1, then one data
byte per key event (`data_in[7]` = release flag, `data_in[6:0]` = the raw
code `keymap` (amiga_keymap.v) translates to hid.v's own Amiga keycode).
`hid`'s own `reset` input (its power-up-side reset, distinct from its
`kbd_reset` output) is pulsed once at the top of the run.

The three keys the combo names -- Left Ctrl, Left Amiga, Right Amiga --
are pressed in sequence (not released), and `kbd_reset` is read for a
one-cycle pulse immediately after the third press. A second run presses
only two of the three and confirms no pulse, the built-in negative control
the word "combo" (three keys together, not any two) requires.

A liveness check with no claim uid (labelled `[not graded -- apparatus
check, no claim uid]`) confirms the byte-serial protocol can deliver a
distinguishable, known value (an ordinary key's keycode) before any graded
read is trusted.

## What would make each assertion fail

Pressing all three of Left Ctrl, Left Amiga and Right Amiga and not
observing a `kbd_reset` pulse would mean the RTL's combo does not match
the page's printed keyboard-reset mechanism.

## Evidence not used until step 3

`classify.evidence` for both claims cites `hid.v:30` (the `kbd_reset`
output) and `:159-171` (the combo-detection block: `kbd_combo_now =
lctrl_down & lamiga_down & ramiga_down`, one-cycle pulse); those exact
lines, plus `amiga_keymap.v`'s HID-to-Amiga-keycode table (needed to know
which raw byte values correspond to Left Ctrl / Left Amiga / Right Amiga),
were opened only after the proposition above was fixed, to build the
fixture.
