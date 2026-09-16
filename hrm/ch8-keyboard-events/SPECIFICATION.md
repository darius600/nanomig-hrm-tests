# Specification — `ch8-keyboard-events`

Package: `hrm/ch8-keyboard-events`
Fixture: `hid` (the module declared in `nanomig/src/misc/hid.v` — module name
and location as given by this package's task assignment; see the disclaimer
below for exactly what that does and does not mean for this document).
Claims file: `hrm-testpipeline/work/ch8-s3-build/ch8-keyboard-events.jsonl`
(17 claims)
Subject: three families of behaviour at the keyboard-event boundary —
printed p.251's up/down flag on the keycode stream, p.359's Caps Lock
press-only behaviour (Appendix G), and p.361's Ctrl-Amiga-Amiga reset
combination (Appendix G).

## Order of work and what this document does and does not know

This file is written from the `quote` and `restatement` fields of the 17
claims in `ch8-keyboard-events.jsonl` alone. Every proposition below, and
every judgment about what is or is not testable, is derived only from those
two fields plus the printed page numbers — never from `classify.evidence`,
which is present in the claims file but is not read for this document.

**Full transparency on one point, because the build brief asks for it rather
than a silence that could be mistaken for the stronger claim:** `hid.v`
itself has not been opened to write this document. But this package's task
assignment and `2026-09-16_ch8_s3_package_list.md` — both read before this
file, as the build brief requires — already state several line-level facts
about `hid.v` in the course of describing the package: that `hid` is a
module in `nanomig/src/misc/hid.v`; that lines 107-109 latch three
modifier keys, line 163 conjoins them, and line 170 pulses an output port
`kbd_reset` declared at line 30; that line 104 reads
`kbd_mouse_data <= { data_in[7], amiga_keycode }`; that line 101's
acceptance condition carries no per-key or `data_in[7]` branch; and that
`hid`'s own `reset` input (line 9) is wired to power-on reset rather than to
`kbd_reset`, so the modifier latches survive the reset the module itself
causes. Those facts arrived as background in the assignment, not as
something opened here to write this document.

**None of them is used below.** No proposition, no wording, no status
judgment in this document depends on any of that RTL detail — each is
argued from the manual quotation alone, exactly as the equivalent facts
would have to be argued if `hid.v` did not exist yet. Port names, the
specific HID stimulus codes, the exact form of the stimulus sequences, and
how (or whether) the reset-survives-its-own-cause fact changes the shape of
the Ctrl-Amiga-Amiga test are step-3 questions, answered only after this
file is committed, per the build brief's order of work. If, once `hid.v` is
actually opened for step 3, any of those pre-supplied facts turns out to be
wrong, that is reported as a finding about the assignment, not quietly
folded into this document after the fact.

## Scope: this package's findings are about NanoMig, not about Minimig

**Every proposition below is tested at `hid`, and `hid` does not exist
upstream.** In `MiSTer-devel/Minimig-AGA_MiSTer` at `da7632e` — the project
this NanoMig core derives from — there is no `amiga_keymap.v`, no `keymap`
and no `hid.v`. `rtl/minimig.v:207` takes the already-decoded
`kbd_mouse_data` as a top-level input, and the translation from a physical
keyboard's own protocol into that byte happens in software above the
chipset, on the host side, not in any RTL this suite builds against. Other
Minimig boards solve the same problem differently again.

**A red result from any assertion in this package is therefore a statement
about NanoMig's `hid.v`, and does not generalise to the Minimig family.**
This is not a hedge added out of caution; it is a measured fact about what
does and does not exist in the upstream tree, carried into this document
because a finding printed without it would read as a claim about Minimig
and be wrong.

## Claim inventory (all 17, grouped by proposition)

| # | uid | page | role |
|---|---|---:|---|
| 1 | `opus:8-keycode.c3` | 251 | PROP-B1-EVENT-BOTH, primary (compound) |
| 2 | `sol:8-keycode.c3`  | 251 | PROP-B1-EVENT-BOTH, split half (down) |
| 3 | `sol:8-keycode.c4`  | 251 | PROP-B1-EVENT-BOTH, split half (up) |
| 4 | `opus:8-keycode.c5` | 251 | PROP-B2-DOWN-VALUE, primary |
| 5 | `sol:8-keycode.c5`  | 251 | PROP-B2-DOWN-VALUE, twin |
| 6 | `opus:8-keycode.c6` | 251 | PROP-B3-UP-VALUE, primary |
| 7 | `sol:8-keycode.c6`  | 251 | PROP-B3-UP-VALUE, twin |
| 8 | `opus:8-appG.c51`   | 359 | PROP-C1-CAPS-DOWN, primary |
| 9 | `sol:8-appG.c63`    | 359 | PROP-C1-CAPS-DOWN, twin |
| 10 | `opus:8-appG.c52`  | 359 | PROP-C2-CAPS-NO-RELEASE, primary |
| 11 | `sol:8-appG.c64`   | 359 | PROP-C2-CAPS-NO-RELEASE, twin |
| 12 | `opus:8-appG.c53`  | 359 | PROP-C3-CAPS-UPDOWN-BIT, primary |
| 13 | `sol:8-appG.c65`   | 359 | PROP-C3-CAPS-UPDOWN-BIT, twin |
| 14 | `opus:8-appG.c117` | 361 | PROP-A1-RESET-TASK, primary |
| 15 | `sol:8-appG.c142`  | 361 | PROP-A1-RESET-TASK, twin |
| 16 | `opus:8-appG.c118` | 361 | PROP-A2-RESET-COMBO, primary |
| 17 | `sol:8-appG.c143`  | 361 | PROP-A2-RESET-COMBO, twin |

All 17 claims are accounted for; none is left unassigned at this stage.
Every claim in this file carries `classify.status: testable` already (a
field read only to confirm nothing here was pre-discarded — its
`evidence`, which would name RTL detail, is not).

---

## Family 1 — p.251, the up/down flag on the keycode stream

Three propositions, all from the same paragraph, all bearing on whether a
key's press and release are both reported, and how.

### PROP-B1-EVENT-BOTH — keycodes are reported for both key transitions

Page 251.

> These codes include both the down and up transitions of the keys.
— `opus:8-keycode.c3`, restated by that reader as the compound claim "The
keycode stream reports both the down transition and the up transition of a
key."

The same sentence is read by the other reader as two atomic halves rather
than one compound claim:

> These codes include both the down and up transitions of the keys.
— `sol:8-keycode.c3`, restated narrowly as "Keycodes report key-down
transitions."

> These codes include both the down and up transitions of the keys.
— `sol:8-keycode.c4`, restated narrowly as "Keycodes report key-up
transitions."

**Split:** one sentence, one reader keeping it as a single compound
proposition ("both directions are reported") and the other splitting it
into its two atomic halves ("down is reported" and, separately, "up is
reported"). All three are kept as three claims rather than collapsed to
one: the compound claim is not the same test as either half alone — a
fixture could in principle report down-transitions only, up-transitions
only, both, or neither, and the compound proposition is the "both"
quadrant specifically, not implied by asserting the two halves as two
independent single-direction checks unless a third check actually
establishes that a single continuous press-then-release sequence produces
two distinct reported events. Step 3 must therefore provide, at minimum,
two atomic assertions (down alone; up alone) plus a third that the two
compose within one sequence, or explain in its own terms why the third is
redundant on this fixture.

**Proposition to test:** a key's downstroke produces a reported event
(`sol:8-keycode.c3`'s half), a key's upstroke produces a reported event
(`sol:8-keycode.c4`'s half), and a single press-then-release sequence
produces both (`opus:8-keycode.c3`'s compound reading).

Status: testable.

### PROP-B2-DOWN-VALUE — a downstroke transmits the printed keycode value, unmodified

Page 251.

> A downstroke of the key transmits the value shown here.
— `opus:8-keycode.c5`

> A downstroke of the key transmits the value shown here.
— `sol:8-keycode.c5` (twin of `opus:8-keycode.c5`; same quote, restated as
"Pressing a key transmits its listed raw keycode value.")

**Proposition to test:** the value reported when a key goes down is the
keycode value printed for that key on p.252's tables — no offset, no
transformation beyond what p.252 itself already encodes.

**Wording note carried forward for step 3:** "the value shown here" refers
to p.252's per-key keycode tables, which are not part of this claims file
(they belong to the sibling `ch8-keycode-table` package, built against
`keymap`). This proposition is not "key X reads as code Y" for any
particular X — that is `ch8-keycode-table`'s question, already exhaustively
covered there by a 128-value sweep of `keymap`'s own two ports. This
proposition is narrower and different in kind: given whatever value a key's
downstroke carries, is that value transmitted as-is (no `$80` added, no
other change), as opposed to `PROP-B3-UP-VALUE` below, where the manual
says an offset is added on the release. Step 3 should pick any one
concrete key as the vehicle for this assertion — the specific key does not
matter to what this proposition asserts, since the "value shown" is a
placeholder for whichever code that key's downstroke carries, not a claim
about which key maps to which code.

Status: testable.

### PROP-B3-UP-VALUE — an upstroke transmits the same value plus $80

Page 251.

> An
> upstroke of the key transmits this value plus $80.
— `opus:8-keycode.c6`

> An
> upstroke of the key transmits this value plus $80.
— `sol:8-keycode.c6` (twin of `opus:8-keycode.c6`; same quote, restated as
"Releasing a key transmits its listed raw keycode plus $80.")

**Proposition to test:** for the same key whose downstroke carries some
value V (per `PROP-B2-DOWN-VALUE`), that key's upstroke carries V + $80 —
an arithmetic relationship between the two readings of the same key, not
two independent facts about two different values.

**Precondition, carried forward for step 3:** a meaningful "plus $80"
reading requires the down-value to be established first, on the same key,
in the same run, exactly as `ch7-overlay-source`'s pattern required "was
the overlay set" to be checked before "was it cleared" meant anything. Per
the build brief's precondition rule this is a graded `expect()` of its own,
not a comment, and it is `PROP-B2-DOWN-VALUE`'s own assertion reused, not a
duplicate written independently.

Status: testable.

---

## Family 2 — p.359 (Appendix G), Caps Lock's press-only behaviour

Three propositions from two adjacent sentences about the one key Appendix G
calls out as an exception to the general down/up scheme just specified in
Family 1.

### PROP-C1-CAPS-DOWN — Caps Lock generates a keycode when pressed

Page 359.

> This key is different from all the others in that it generates a keycode
> only when it is pushed
> down, never when it is released.
— `opus:8-appG.c51`, restated as "Caps Lock generates a keycode when it is
pushed down."

> This key is different from all the others in that it generates a keycode
> only when it is pushed
> down, never when it is released.
— `sol:8-appG.c63` (twin of `opus:8-appG.c51`; same quote, restated
"Caps Lock generates a keycode when pressed.")

**Proposition to test:** pressing Caps Lock produces a reported event, the
same as any other key's downstroke under `PROP-B1-EVENT-BOTH`'s down half —
this half of the sentence states no exception, only the release half does.

Status: testable.

### PROP-C2-CAPS-NO-RELEASE — Caps Lock generates no keycode when released

Page 359. Same sentence as `PROP-C1-CAPS-DOWN`, its other half:

> This key is different from all the others in that it generates a keycode
> only when it is pushed
> down, never when it is released.
— `opus:8-appG.c52`, restated "Caps Lock never generates a keycode when it
is released."

> This key is different from all the others in that it generates a keycode
> only when it is pushed
> down, never when it is released.
— `sol:8-appG.c64` (twin of `opus:8-appG.c52`; restated "Caps Lock
generates no keycode when released.")

**Split, matching Family 1's precedent:** one sentence naming two
behaviours (pressed: yes; released: no), and here both readers split it
identically into two claims each — unlike `PROP-B1-EVENT-BOTH`, there is no
compound "both halves at once" reading of this sentence to also carry,
because the sentence's whole point is that the two halves are NOT
symmetric. `PROP-C1-CAPS-DOWN` and `PROP-C2-CAPS-NO-RELEASE` are the
complete pair.

**Proposition to test:** releasing Caps Lock produces no reported event —
the opposite of every other key's upstroke under `PROP-B1-EVENT-BOTH`'s up
half. This is the one place in this package where an expected value is
"nothing observed," which the build brief's own framing treats as a
legitimate, cite-able proposition rather than an untestable absence: the
manual states the absence affirmatively ("never... when it is released"),
so a bounded, deliberate non-event is exactly what the page asserts. Step 3
must define what "no reported event" means operationally at this
boundary — the manual's own affirmative statement is what makes that a
testable question rather than a hedge.

Status: testable.

### PROP-C3-CAPS-UPDOWN-BIT — the up/down bit is still carried on Caps Lock's own code

Page 359, the sentence immediately following the one above.

> However, the up/down bit is still used.
— `opus:8-appG.c53`, restated "The up/down bit is still carried in the
Caps Lock keycode."

> However, the up/down bit is still used.
— `sol:8-appG.c65` (twin of `opus:8-appG.c53`; restated "Caps Lock still
uses the up/down flag bit.")

**Proposition to test:** this narrows Family 1's general up/down-bit
mechanism (`PROP-B2-DOWN-VALUE` / `PROP-B3-UP-VALUE`'s shared bit) to Caps
Lock specifically. Because `PROP-C2-CAPS-NO-RELEASE` says Caps Lock's
upstroke produces no event at all, "the bit is still used" cannot mean "an
upstroke event carries the bit set" the way it would for an ordinary key —
there is no such event to carry it. Read plainly against the surrounding
two sentences, "however" signals a qualification of the press-only rule
just stated, not a suspension of it: **the one event Caps Lock does
produce (its downstroke) still carries the same bit in the same position
as every other key's downstroke does**, rather than Caps Lock's single byte
being shaped differently from the general p.251 format merely because the
key behaves specially about when it fires. That is the reading this
specification adopts, on the printed sentence pairing alone; step 3 should
report explicitly if the fixture makes that reading untestable or
contested, rather than silently substituting a different one.

Status: testable, under the reading above.

---

## Family 3 — p.361 (Appendix G), the Ctrl-Amiga-Amiga reset combination

Two propositions from two adjacent sentences describing the keyboard's
reset-request role.

### PROP-A1-RESET-TASK — resetting the computer on the user's command is a task the keyboard performs

Page 361.

> The keyboard has the additional task of resetting the computer on the
> command of the user.
— `opus:8-appG.c117`, restated "Resetting the computer on the user's
command is a task of the keyboard."

> The keyboard has the additional task of resetting the computer on the
> command of the user.
— `sol:8-appG.c142` (twin of `opus:8-appG.c117`; the source claims file
notes an id collision — a different, unrelated `opus:8-appG.c142` exists
elsewhere in the wider p.364 material and is not this claim — restated here
"The keyboard can reset the computer at the user's command.")

**Proposition to test:** some mechanism reachable from the keyboard side of
this boundary is capable of producing a system reset — the existence of the
capability, not yet the specific combination that triggers it (that is
`PROP-A2-RESET-COMBO`, next). This is the coarser of the two claims and is
logically implied by demonstrating `PROP-A2-RESET-COMBO`, but is kept as
its own assertion with its own citation rather than folded silently into
the other one, following `ch7-overlay-source`'s precedent for two
propositions that may reduce to one observable mechanism: collapsing them
now, before step 3 has looked, would under-cite the manual.

Status: testable.

### PROP-A2-RESET-COMBO — Reset Warning is initiated by pressing Ctrl and both Amiga keys simultaneously

Page 361.

> The
> user initiates Reset Warning by simultaneously pressing the Ctrl key and
> the two Amiga keys.
— `opus:8-appG.c118`, restated "Reset Warning is initiated by pressing
Ctrl and both Amiga keys simultaneously."

> The
> user initiates Reset Warning by simultaneously pressing the Ctrl key and
> the two Amiga keys.
— `sol:8-appG.c143` (twin of `opus:8-appG.c118`; restated "Simultaneously
pressing Ctrl and both Amiga keys initiates Reset Warning.")

**Proposition to test:** the specific three-key combination — Ctrl, left
Amiga and right Amiga, all three, at once — is what produces the reset
signal named in `PROP-A1-RESET-TASK`, as opposed to any two of the three,
or any other combination. A control case belongs here by the build brief's
own rule (at least one case expected to PASS on the same measurement basis
as the suspect case): holding fewer than all three keys down must NOT
produce the same result, or this assertion has not shown that the
combination — as opposed to any one of its members alone — is what
matters.

**What this proposition does not cover, and why:** the manual's
surrounding paragraphs (p.361-364) go on to describe a printed reset
*protocol* around this trigger — a `$78` warning code, a KCLK/KDAT
handshake, a 500 ms pulse — none of which is in this claims file. Those
sentences are not silently absorbed into this proposition; if they belong
to `ch8-keyboard-events` at all rather than to a sibling package or to
`not-asserted`, that is a step-3 question to be raised explicitly, not
answered by expanding this proposition's scope beyond what
`ch8-keyboard-events.jsonl`'s own 17 claims license.

Status: testable.

---

## Claims not assigned an assertion

None. All 17 claims are classified `testable` in the claims file and each
is assigned to exactly one of the eight propositions above (see the
inventory table for the exact uid-to-proposition mapping). If step 3 finds
that any claim cannot actually be graded at this boundary without changing
RTL, adding a port, or inventing a requirement the page does not state,
that is a HALT to report — per the build brief — not a silent re-scoping of
this document after the fact.

## What this specification deliberately does not contain

Port names, signal names, stimulus encodings (including any HID key
codes), concrete sequencing (how a "press" or "release" is driven at this
boundary, or how many clock cycles apart), the mutation design, and any
resolution of how the reset-survives-its-own-cause fact named in this
package's task assignment affects `PROP-A2-RESET-COMBO`'s test sequence,
are all absent by design. Per the build brief's order of work, they are
added only after this file is committed and `hid.v` (and, where needed,
`amiga_keymap.v` for translating a stimulus source code into the p.252
value a proposition needs) are read for step 3. The eight PROPs above are
the propositions that become this package's `expect()` assertions, each
with its own control case and stimulus mutation, once that step opens.
