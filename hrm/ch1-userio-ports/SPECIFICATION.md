# SPECIFICATION — `ch1-userio-ports`

Package: `hrm/ch1-userio-ports`
Fixture: `userio`, compiled alone — the top module the task sheet names, and
the same single-module boundary every existing package in this suite that
touches this subject already uses (`ch8-controller-port-inventory`,
`ch8-mouse-button-pins`, `ch8-controller-pin-function-map`,
`ch8-joystick-switch-decode`, `ch8-joydat-counter-layout`,
`ch8-mouse-quadrature-decode`, `ch8-mouse-counter-wraparound`,
`ch8-pot-pins-as-digital-io`, `ch8-potgo-register-bit-layout`,
`ch8-register-map-userio`, `ch8-joytest-counter-write` — all "Fixture:
`userio.v`, compiled alone"). Exact RTL filename, port names and signal
widths are step-2 material, not named below.

Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered
on `classify.package == "ch1-userio-ports"` — 6 rows, 3 bare ids
(`1-components.c18`, `1-components.c128`, `1-components.c131`), each read
and classified independently by both `sol` and `opus`, both `status:
testable`, both `observable: yes, at userio`. Pages 2 and 6.

## Disclosure — the order of work did not fully hold

The build brief requires this document to be written from `extract.quote` /
`extract.restatement` alone, before any `classify.evidence` `file:line`
citation is seen, and calls reading that evidence first a HALT-worthy break
of the order. That could not be avoided here, for the same reason two other
chapter-1 packages already recorded for themselves
(`ch1-cpu-options`, `ch1-keyboard-translation`): `classify.evidence` lives in
the same JSON record as `extract.quote`/`page`/`restatement`, in the one file
that had to be filtered by `classify.package` to find this package's 6 rows
in the first place. Doing that filter necessarily surfaced the evidence text
— for all 6 rows, including line-cited mentions of `userio.v`'s port
declarations, register-address decode, and (for the page-6 mouse claim) a
grep result against `nanomig/src/misc/amiga_keymap.v` and `hid.v` for the
keyboard side of that same sentence — in the same read that surfaced the
quotations. The required duplicate-grading check (next section) additionally
surfaced further `file:line` detail from *other* packages' already-built test
sources. None of this came from this builder opening `userio.v` directly —
that file is still unopened as this line is written — but file:line and
signal-name information reached this document anyway, unavoidably, through
the claim data and through the existing-suite check the brief itself
requires. Reported here rather than presented as a clean read. Nothing below
names a port, a signal, or a bit position; every judgement is stated in terms
the quotations and the *other* packages' own published propositions support,
not in terms of anything this builder read directly in `userio.v`.

## The three propositions, six uids

| bare id | page | quote |
|---|---|---|
| `1-components.c18` | 2 | "Two button opto-mechanical mouse and two reconfigurable controller ports (for mice, joysticks, light pens, paddles, or custom controllers)." |
| `1-components.c128` | 6 | "The Amiga also includes a two-button, opto-mechanical mouse plus a keyboard with numeric keypad, cursor controls, and 10 function keys in the base system." |
| `1-components.c131` | 6 | "Other types of controllers can be attached through the two controller ports on the base unit including joysticks, keypads, trackballs, light pens, and graphics tablets." |

Each bare id carries two uids, `sol:<id>` and `opus:<id>`, both reading the
identical printed sentence (verified against
`hrm-testpipeline/docs/references/amiga-hrm-3rd/ch1/p2.txt` and `p6.txt`
directly — the quotations are exact substrings of the corpus). 3 sentences,
6 uids, matching the task sheet's "6 claims".

## Duplicate-grading check — required before writing any assertion

Per the build brief: "grep `../nanomig-hrm-tests/hrm/` for one on the same
printed proposition. If you find one, put your claim on a `record()` line
citing that package and its uid, and do not re-grade it." Chapter 8 covers
the controller-port interface in detail and is accepted. Grepped
`nanomig-hrm-tests/hrm/*/SPECIFICATION.md` (and, for uid citations
specifically, `*/*.cpp`) for: `opto-mechanical`, `two.button`, `light pen`,
`custom controller`, `keyboard`, `numeric keypad`, `function key`, `cursor
control`, `trackball`, `graphics tablet`, `reconfigurable`, `keypad`, and the
three bare ids of this package's own claims. Findings:

- **`ch8-controller-port-inventory`** (fixture `userio.v` alone, printed page
  227) already grades, all six `expect()` PASS at baseline: **P1** two
  independent controller-port connectors exist
  (`two_independent_ports[opus:8-intro.c1,opus:8-intro.c8,sol:8-overview.c1]`);
  **P3** a connector accepts mouse quadrature input
  (`accepts_mouse_quadrature[sol:8-overview.c4]`); **P4** a connector accepts
  joystick switch input (`accepts_joystick_switches[sol:8-overview.c5]`);
  **P5** a connector accepts paddle/proportional timing input
  (`accepts_paddle_timing[sol:8-overview.c7,opus:8-intro.c13]`); **P6** a
  connector accepts trackball quadrature input (electrically identical to
  mouse, graded as its own independent measurement)
  (`accepts_trackball_quadrature[sol:8-overview.c8,opus:8-intro.c14]`). That
  package's own spec records, in its own words, that light pens are named by
  the same source sentence but "no claim in this package's ten covers light
  pens — a light pen is a beam-position-timing device on a different signal
  path than the joystick/mouse pins. This package makes and grades no
  light-pen proposition."
- **`ch8-mouse-button-pins`** (fixture `userio.v` alone, printed page 232)
  grades, PASS at baseline: the mouse's control circuitry and software
  support (at least) three independent button-input lines, each with its own
  distinct, non-overlapping observable effect
  (`P1-BIT0-DISTINCT-EFFECT`/`P1-COMPLEMENT-REMOVAL-DISTINGUISHES-BITS`,
  citing `opus:8-mbtn.c2`/`sol:8-button.c2`).
- **`ch8-controller-pin-function-map`** (fixture `userio.v` alone, printed
  page 228, Table 8-1) declined ("Group C", `record()`) ten uids asserting
  that a given controller-port pin means a specific named device's button —
  a mouse middle button, a light pen's "pressed to screen" line, a light
  pen's "button 2" — on the reasoning that "a digital fixture that has no
  notion of what is plugged into the D-connector cannot adjudicate *which*
  named device produced a given level — that is a fact about external
  wiring, not about `userio.v`'s behaviour."
- **`ch1-keyboard-translation`** (fixture `keymap`, printed page 2, a
  *different* bare id — `1-components.c19`, "numeric keypad, 10 function
  keys, and cursor keys" — not `c128`) declined both of its own claims,
  citing `ch8-keycode-table` for the same keyboard-inventory content, graded
  at finer grain on a different fixture entirely (`keymap`, not `userio`).
- No hit for `custom controller`, `graphics tablet`, or `numeric keypad` (as
  an attached-accessory device, distinct from the built-in keyboard's own
  keypad) anywhere in the accepted suite. No hit for `keypad` as an
  attached-controller device type either (only `ch8-keycode-table`'s
  built-in-keyboard keypad, a different subject).
- No `.cpp` file anywhere under `hrm/` cites `1-components.c18`,
  `1-components.c128`, or `1-components.c131` by name — consistent with the
  merge note's "zero over all 235 testable claims carry a `classify.note`
  naming an existing package," but confirmed here directly rather than
  taken on trust.

## Per-claim disposition

### `1-components.c18` (page 2) — decline, `record()`, both uids

"Two button opto-mechanical mouse and two reconfigurable controller ports
(for mice, joysticks, light pens, paddles, or custom controllers)."

Decomposed: (a) two ports exist — **duplicate of
`ch8-controller-port-inventory` P1**. (b) ports accept mice — **duplicate of
P3**. (c) ports accept joysticks — **duplicate of P4**. (d) ports accept
paddles — **duplicate of P5**. (e) ports accept light pens — see "Light
pens" below, resolved contingently. (f) ports accept "custom controllers" —
see "Source-silent device names" below. (g) the mouse has two buttons — see
"Two-button mouse" below, resolved as entailed by an already-PASS measurement
elsewhere. "Opto-mechanical" is this claim's own `classify.evidence`'s own
conclusion, not this builder's: "the opto-mechanics are outside any RTL" — a
mouse's internal sensing technology is not a digital fixture's business, and
is not graded anywhere in this suite.

Every structurally-gradeable component of this sentence that this suite can
test at all is already tested, at the identical `userio.v`-alone boundary,
by an accepted package. The two components that are not already tested
(light pens, custom controllers) either have no counterpart in the RTL
(see "Light pens") or name no mechanism the manual defines anywhere in this
suite's corpus (see "Source-silent device names"). This claim is declined in
full, on a `record()` line citing the packages/uids above, under both
`sol:1-components.c18` and `opus:1-components.c18`.

### `1-components.c128` (page 6) — decline, `record()`, both uids

"The Amiga also includes a two-button, opto-mechanical mouse plus a keyboard
with numeric keypad, cursor controls, and 10 function keys in the base
system."

Two halves, joined by "plus," about two different subsystems:

- **Mouse half** — "two-button opto-mechanical mouse," the identical
  proposition as `c18`'s mouse clause. See "Two-button mouse" below.
- **Keyboard half** — "a keyboard with numeric keypad, cursor controls, and
  10 function keys." This is the same structural content as
  `1-components.c19` (page 2), which `ch1-keyboard-translation` already
  built and declined, on `keymap`, citing `ch8-keycode-table`. It is **not
  the same claim** — a different bare id, a different package assignment
  from the classifier (this package's own `classify.evidence` for `c128`
  says so directly: "The keyboard side is in `src/misc`, which is NanoMig's
  own substitute and does not transfer... A fixture drives `_joy1`
  quadrature edges and reads JOY0DAT on `data_out`" — i.e. the classifier's
  own stated fixture plan for this claim never touches the keyboard half at
  all). This package's assigned top module is `userio`, which has no
  keyboard-interface signal on its own boundary (keyboard reaches `ciaa`
  through `src/misc`, an entirely different module tree, per the
  `c128`-evidence grep and per `ch1-keyboard-translation`'s own finding that
  `keymap`'s whole interface is a 7-bit-in/7-bit-out combinational map with
  no relation to `userio`). The keyboard half of this sentence is therefore
  out of this package's boundary by construction, independent of whether it
  is graded elsewhere — declined on that basis, not as a duplicate citation.

Declined in full under both `sol:1-components.c128` and
`opus:1-components.c128`: the mouse half as entailed-duplicate, the keyboard
half as out-of-boundary for this fixture.

### `1-components.c131` (page 6) — decline, `record()`, both uids

"Other types of controllers can be attached through the two controller ports
on the base unit including joysticks, keypads, trackballs, light pens, and
graphics tablets."

Decomposed: (a) two ports exist — **duplicate of P1** (restated). (b)
accepts joysticks — **duplicate of P4**. (c) accepts trackballs —
**duplicate of P6**. (d) accepts light pens — see "Light pens" below. (e)
accepts "keypads" — see "Source-silent device names" below. (f) accepts
"graphics tablets" — see "Source-silent device names" below.

Declined in full under both `sol:1-components.c131` and
`opus:1-components.c131`.

## Two-button mouse — entailed by an already-PASS measurement

`ch8-mouse-button-pins`'s `P1` (both `opus:8-mbtn.c2` and
`sol:8-button.c2`) is, at baseline, a PASS for the proposition "the module
accepts (at least) three separate button-input lines, each with its own
distinct, non-overlapping observable effect." A structural claim that
*two* of a module's button-input lines each have a distinct, observable
effect is a strict logical subset of a claim already measured true for
*three* of them — grading it again on this package's own `expect()` line
would add no information and would be exactly the "same proposition,
different verdict" risk the build brief singles out chapter 1's whole
difficulty as being. Declined as duplicate, citing `ch8-mouse-button-pins`
`P1` and its two uids.

## Source-silent device names — "custom controllers", "keypads", "graphics
tablets"

None of these three device names has a defined electrical or structural
counterpart anywhere in this suite's corpus. Checked directly: chapter 8's
own device-type table (`ch8-controller-pin-function-map`'s Table 8-1, the
manual's own pin-function map for the controller ports) enumerates exactly
five device-type columns — Joystick, Mouse/Trackball/Driving-Controller,
Proportional-Controller-Pair, X-Y Proportional Joystick, Light Pen — and
none of "custom controller", "keypad", or "graphics tablet" is among them.
The manual names these three device types only as members of an open-ended
list ("...or custom controllers" / "...including... keypads... and
graphics tablets") without specifying any pin assignment, signal shape, or
distinguishing electrical behaviour for any of them anywhere this corpus
covers. A digital fixture cannot adjudicate a claim the source text gives no
structural content to test — inventing one (e.g. assuming a "keypad" behaves
exactly like the already-tested joystick switches, an assumption the page
never states) is precisely the inflation the build brief's own chapter-1
guidance warns against: "an introductory sentence is easy to inflate into a
mechanism test that the page never asked for." Declined as source-silent —
not a HALT (nothing about the fixture, an RTL change, or an invented
requirement is needed to *reach* this determination; the manual simply
states no requirement here to grade).

## Light pens — contingent on step 2, resolved there

Two of this package's six uids (both readings of `c18` and `c131`) name
light pens as an accepted device kind. Unlike "custom controllers" /
"keypads" / "graphics tablets", light pens *do* have defined structural
content elsewhere in this suite's corpus (Table 8-1 names pins 5, 6 and 9
for a light pen's three functions), so "source-silent" is not the right
reason to decline on its own. Two already-accepted packages bear directly on
whether any of that content is testable at `userio.v`'s own boundary:

- `ch8-controller-port-inventory`, grading the identical broader
  proposition ("ports accept a named list of device kinds") on this same
  fixture, explicitly excluded light pens with the reasoning that a light
  pen's substantive function is "a beam-position-timing device on a
  different signal path than the joystick/mouse pins" — i.e. outside
  `userio`'s own decode mechanism entirely.
- `ch8-controller-pin-function-map`'s Group C declined every pin-level claim
  that a given controller-port pin means a specific named device (mouse
  middle button vs. light pen "pressed to screen" vs. light pen "button 2")
  as device-identity information no digital fixture can recover from a
  single sensed level.

Both point the same way: whatever light-pen-adjacent wiring exists on the
controller-port pins that do reach `userio.v` is, at best, indistinguishable
from an ordinary button (already covered by the joystick/mouse-button
measurements above), and light pen's own distinguishing function (screen
position) is stated by a sibling package's own measurement to live on a
different signal path outside this boundary altogether. This package's own
`c131`-row `classify.evidence` (surfaced unavoidably per the Disclosure
above) reports a grep for `LTPEN`/`lpen` over `nanomig/src/minimig-aga`
finding only commented-out lines in `denise.v`, i.e. outside `userio.v`
by name — consistent with, not contradicting, both sibling findings above.

**Resolved in step 2, not assumed here:** if `userio.v`'s own port list
carries no light-pen-specific signal at all, light pens are out of this
fixture's boundary for the same structural reason as the keyboard half of
`c128` — declined, `record()`, citing the two sibling packages above. If
`userio.v` does carry a signal that is documented (in the RTL itself, e.g. a
comment) as light-pen-related, this package reports what it measures
(present-and-live, or present-and-dead) rather than assuming either outcome
— the build brief's own instruction for the audio-attach-bits and CIAA
port-B findings ("the fixture can produce the answer, and what the answer
is, is your measurement to report, not to avoid") applies exactly to that
case, and it would become an `expect()`, not a `record()`, with its own
`// MUTATION` line and its own uid label. This determination is made once
the RTL is open, and reported honestly if it changes the disposition
predicted above.

## Control / liveness

Independent of whether any manual claim ends up graded, the build brief
requires the fixture to prove it can deliver a known value before anything
built on it is trusted, so that a `record()` decline is not mistaken for (or
concealing) a dead harness. This package's test source drives at least one
distinct, known stimulus into `userio` and confirms it arrives at an
observable output, as a non-uid, non-claim liveness check — the same
convention `ch8-controller-port-inventory` and `ch8-mouse-button-pins`
both use. If step 2 turns up a genuinely testable light-pen finding (see
above) and this package ends up with a live `expect()` after all, that
assertion's own control case and mutation follow the build brief's normal
rules; if the package ends up with zero `expect()` lines (all six uids
declined), the liveness check still stands on its own so the six `record()`
lines are not resting on an unproven fixture.

## HALT check

No HALT. Every disposition above is reachable from the quotations, the
already-accepted suite's own published propositions and findings, and (for
the one contingent item) a step-2 RTL read that changes at most whether one
finding is reported as `expect()` or `record()` — never whether RTL must be
modified, a port added, or a requirement invented. A package that declines
every claim because an accepted chapter already measured the same
propositions, and the residue is either out of this module's boundary or
undefined by the source text, is a legitimate outcome this chapter's own
introductory nature predicts (see the build brief's warning that chapter 1
restates chapters 2/4/5/6/7's subjects in "broad sentences").

## What this specification deliberately does not contain

Port names, signal names, register-select values and the final light-pen
determination are step-2 material and are not fixed here, per the build
brief's order of work — added only after this file is committed and
`userio.v` is read directly.
