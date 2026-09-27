# Specification — `ch8-keyboard-events`

Package: `hrm/ch8-keyboard-events`
Fixture: `hid` (the module declared in `nanomig/src/misc/hid.v`, with its
own `keymap` instance, `amiga_keymap.v`).
Claims file: `hrm-testpipeline/work/ch8-s3-build/ch8-keyboard-events.jsonl`
(17 claims), plus 4 Caps Lock LED claims from
`2026-09-16_ch8_s3_classified_all.jsonl` (see "The Caps Lock LED claims"
below): opus:8-appG.c54/c55 and sol:8-appG.c66/c67 — 21 claims in total.
Subject: three families of behaviour at the keyboard-event boundary —
printed p.251's up/down flag on the keycode stream, p.359's Caps Lock
press-only behaviour and its LED-driven up/down bit (Appendix G), and
p.361's Ctrl-Amiga-Amiga reset combination (Appendix G).

Every proposition below is argued from the claims' `quote` and
`restatement` fields and the printed page numbers, not from
`classify.evidence` or from the RTL.

## The Caps Lock LED claims

`2026-09-16_ch8_s3_classified_all.jsonl` marks `opus:8-appG.c54`,
`opus:8-appG.c55`, `sol:8-appG.c66` and `sol:8-appG.c67` — p.359's third
sentence about Caps Lock, printed immediately after the two sentences
`PROP-C1`–`PROP-C3` cover — `not-asserted`, with `absent_signal:
caps_lock_led` and the reason "no LED in this tree is keyboard-driven."
**That reasoning answers the wrong question.** The observable these four
claims name is `kbd_mouse_data[7]`, the same real `hid` output
`PROP-B2`/`PROP-B3`/`PROP-C3` already read; the LED is named only in the
sentence's own antecedent, identifying WHICH press is under discussion ("the
press that turns the LED on" / "the press that shuts the LED off"), not as a
signal the test needs to read. Checked against the raw page scan at
`docs/references/amiga-hrm-3rd/appG-keyboard/p359.txt` and `p360.txt`
(quoted below), and against `hid.v`'s complete port list — no `led` port
exists anywhere in the file.

**PROP-C4 and PROP-C5 below grade them.** Grounding "the press that turns
the LED on" and "the press that shuts it off" in something testable needs
one more fact, also on p.360 and also `not-asserted` at this boundary for
the same reason (no LED port exists to read the LED itself turning off) but
cited here as background: the keyboard's own start-up sequence ends
"Finally, the Caps Lock LED is shut off" (`opus:8-appG.c112`,
`sol:8-appG.c133`) — so the LED is in a known-off state immediately after
start-up, which is what licenses calling the FIRST subsequent Caps Lock
press "the press that turns it on" and the SECOND "the press that shuts it
off." `opus:8-appG.c112`, `opus:8-appG.c113` and `sol:8-appG.c133` are not
themselves reclassified or graded by this package — they remain
`not-asserted` at this boundary in their own right — they are cited here
only as the printed fact behind PROP-C4/PROP-C5's stimulus design.

The same sentence fixes how `PROP-C3` reads "the up/down bit is still
used": the bit means the LED's target state, not physical key state (see
`PROP-C3` below).

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

## Claim inventory (all 21, grouped by proposition)

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
| 14 | `opus:8-appG.c54`  | 359 | PROP-C4-CAPS-LED-ON-PRESS, primary |
| 15 | `sol:8-appG.c66`   | 359 | PROP-C4-CAPS-LED-ON-PRESS, twin |
| 16 | `opus:8-appG.c55`  | 359 | PROP-C5-CAPS-LED-OFF-PRESS, primary |
| 17 | `sol:8-appG.c67`   | 359 | PROP-C5-CAPS-LED-OFF-PRESS, twin |
| 18 | `opus:8-appG.c117` | 361 | PROP-A1-RESET-TASK, primary |
| 19 | `sol:8-appG.c142`  | 361 | PROP-A1-RESET-TASK, twin |
| 20 | `opus:8-appG.c118` | 361 | PROP-A2-RESET-COMBO, primary |
| 21 | `sol:8-appG.c143`  | 361 | PROP-A2-RESET-COMBO, twin |

All 21 claims are accounted for. The 17 from `ch8-keyboard-events.jsonl`
carry `classify.status: testable` (a field read only to confirm nothing here
was pre-discarded — its `evidence`, which would name RTL detail, is not);
the 4 Caps Lock LED claims (rows 14–17) carry `classify.status:
not-asserted` in `2026-09-16_ch8_s3_classified_all.jsonl` — see "The Caps
Lock LED claims" above for why that classification does not hold and this
package grades them anyway.

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
two distinct reported events. The package therefore grades two atomic
assertions (down alone; up alone) plus a third that the two compose within
one press-then-release sequence on a third key.

**Proposition to test:** a key's downstroke produces a reported event
(`sol:8-keycode.c3`'s half), a key's upstroke produces a reported event
(`sol:8-keycode.c4`'s half), and a single press-then-release sequence
produces both (`opus:8-keycode.c3`'s compound reading).

Status: graded (EVENT-DOWN-ACCEPTED, EVENT-UP-ACCEPTED,
EVENT-BOTH-IN-SEQUENCE), GREEN.

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

**Wording note:** "the value shown here" refers
to p.252's per-key keycode tables, which are not part of this claims file
(they belong to the sibling `ch8-keycode-table` package, built against
`keymap`). This proposition is not "key X reads as code Y" for any
particular X — that is `ch8-keycode-table`'s question, already exhaustively
covered there by a 128-value sweep of `keymap`'s own two ports. This
proposition is narrower and different in kind: given whatever value a key's
downstroke carries, is that value transmitted as-is (no `$80` added, no
other change), as opposed to `PROP-B3-UP-VALUE` below, where the manual
says an offset is added on the release. The assertion uses one concrete
key ('a') as the vehicle — the specific key does not matter to what this
proposition asserts, since the "value shown" is a placeholder for whichever
code that key's downstroke carries, not a claim about which key maps to
which code.

Status: graded (DOWN-VALUE-UNMODIFIED), GREEN.

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

**Precondition:** a meaningful "plus $80" reading requires the down-value
to be established first, on the same key, in the same run, exactly as
`ch7-overlay-source`'s pattern required "was the overlay set" to be checked
before "was it cleared" meant anything. That precondition is a graded
`expect()` of its own — `PROP-B2-DOWN-VALUE`'s own assertion, not a
duplicate; the press value this trial reads first is printed via `note()`.

Status: graded (UPSTROKE-ADDS-0X80), GREEN.

---

## Family 2 — p.359 (Appendix G), Caps Lock's press-only behaviour and its LED-driven bit

Five propositions from three adjacent sentences about the one key Appendix
G calls out as an exception to the general down/up scheme just specified in
Family 1. The third sentence (`PROP-C4`, `PROP-C5`) also decides how
`PROP-C3` reads the second.

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

Status: graded (CAPS-LOCK-DOWN-ACCEPTED), GREEN.

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
"nothing observed," a legitimate, cite-able proposition rather than an
untestable absence: the manual states the absence affirmatively ("never...
when it is released"), so a bounded, deliberate non-event is exactly what
the page asserts. Operationally, "no reported event" means
`kbd_mouse_level` does not toggle; CONTROL-NO-EVENT-ON-UNMAPPED-CODE is the
same-basis PASS control that this read can and does report "no event".

Status: graded (CAPS-LOCK-RELEASE-NOT-ACCEPTED), RED: `hid.v:101`'s
acceptance condition carries no branch on the key's identity or on
`data_in[7]`, so Caps Lock's release is accepted as a reported event.

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
there is no such event to carry it.

Nor does the bit mean physical keystate on Caps Lock's downstroke, the way
`PROP-B2`/`PROP-B3` establish for an ordinary key: the very next sentence
on p.359 says "When pushing the Caps Lock key turns on the Caps Lock LED,
the up/down bit will be 0; when pushing Caps Lock shuts off the LED, the
up/down bit will be 1." Caps Lock's byte IS shaped differently: this bit
carries the LED's target state after the press, not "is the key currently
down". See `PROP-C4-CAPS-LED-ON-PRESS` and `PROP-C5-CAPS-LED-OFF-PRESS`
below, which grade that rule directly.

The proposition graded here: the one event Caps Lock does produce (its
downstroke) carries a real, populated bit 7, rather than one blanked out or
hardwired — read on the first press since a known-off LED state, that bit
is 0, for the reason `PROP-C4` states, not because it mirrors physical
keystate. On that first press the LED-target-state reading and the
physical-keystate reading agree — both say 0 — so this proposition alone
cannot tell them apart; the SECOND press, which only `PROP-C5` exercises,
does.

Status: graded (CAPS-LOCK-BIT7-CARRIES-LED-STATE), GREEN.

### PROP-C4-CAPS-LED-ON-PRESS — the press that turns the LED on sends bit 0

Page 359, the sentence immediately following `PROP-C3`'s, naming the LED
explicitly for the first time on this page:

> When pushing the Caps
> Lock key turns on the Caps Lock LED, the up/down bit will be 0;
— `opus:8-appG.c54`, restated "When pressing Caps Lock turns the Caps Lock
LED on, the up/down bit is 0."

> When pushing the Caps
> Lock key turns on the Caps Lock LED, the up/down bit will be 0; when pushing Caps Lock shuts
> off the LED, the up/down bit will be 1.
— `sol:8-appG.c66` (twin of `opus:8-appG.c54`, quoting the full two-clause
sentence rather than splitting it; restated for its first clause "A Caps
Lock press that turns the LED on sends up/down bit 0.")

Grounded, for identifying WHICH press this is, by page 360:

> ...Finally, the Caps Lock LED is shut off. This marks the end of the start-up
> sequence, and normal processing commences.
— `opus:8-appG.c112` (quoting "Finally, the Caps Lock LED is shut off."),
`opus:8-appG.c113` (quoting the sentence naming the end of the start-up
sequence), `sol:8-appG.c133` (same LED-off sentence as `c112`). All three
remain `not-asserted` at this boundary in their own right — no LED port
exists anywhere in the pinned tree to read the LED itself turning off —
and are not reclassified by this package; they are cited here only as the
printed fact that the LED is off immediately after start-up.

**Proposition to test:** immediately after the keyboard's own start-up
sequence, the Caps Lock LED is off (p.360). The FIRST Caps Lock press from
that point on is therefore, by p.359's own rule, "the press that turns the
LED on," and the up/down bit reported on that press must be 0.

**Why no LED port is needed:** the LED is named only in the sentence's
antecedent ("when pushing... turns on the... LED"), identifying which press
the consequent ("the up/down bit will be 0") is about. The graded
observable is `kbd_mouse_data[7]`, the same real `hid` output every other
proposition in this document already reads — nothing about grading this
proposition requires observing the LED itself. This is why the
`absent_signal` ground in the classified-all file does not hold (see "The
Caps Lock LED claims" above).

Status: graded (CAPS-LOCK-LED-ON-PRESS-BIT-ZERO), GREEN — also the
same-basis PASS control for `PROP-C5`.

### PROP-C5-CAPS-LED-OFF-PRESS — the press that shuts the LED off sends bit 1

Page 359, same sentence as `PROP-C4`, its second clause:

> when pushing Caps Lock shuts
> off the LED, the up/down bit will be 1.
— `opus:8-appG.c55`, restated "When pressing Caps Lock turns the Caps Lock
LED off, the up/down bit is 1."

> When pushing the Caps
> Lock key turns on the Caps Lock LED, the up/down bit will be 0; when pushing Caps Lock shuts
> off the LED, the up/down bit will be 1.
— `sol:8-appG.c67` (twin of `opus:8-appG.c55`, same full-sentence quote as
`sol:8-appG.c66`; restated for its second clause "A Caps Lock press that
turns the LED off sends up/down bit 1.")

Grounded by the same p.360 citations as `PROP-C4` (`opus:8-appG.c112`,
`opus:8-appG.c113`, `sol:8-appG.c133`, not reclassified, cited only as
background — see `PROP-C4`, not repeated on a second graded line here).

**Proposition to test:** starting from the same known-off LED state as
`PROP-C4`, one press turns the LED on (that is `PROP-C4`'s own proposition,
not re-tested here) and — Caps Lock being a toggle with a persistent LED —
the very next press necessarily turns it back off. That SECOND press must,
by p.359's rule, report the up/down bit as 1.

**Split from `PROP-C4`:** kept as two separate propositions, each with its
own citation, rather than one, because they are testable independently and
a fixture can get one right and the other wrong — as `hid.v` does.

Status: graded (CAPS-LOCK-LED-OFF-PRESS-BIT-ONE), RED: `hid.v` has no
register that counts Caps Lock presses or tracks an LED state; `hid.v:104`
mirrors `data_in[7]` verbatim on every accepted event, so the second press
reads bit 7 == 0, like the first.

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
would under-cite the manual.

Status: graded (RESET-TASK-CAPABLE), GREEN.

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
or any other combination. A control case belongs here (at least one case
expected to PASS on the same measurement basis as the suspect case):
holding fewer than all three keys down must NOT
produce the same result, or this assertion has not shown that the
combination — as opposed to any one of its members alone — is what
matters.

**What this proposition does not cover, and why:** the manual's
surrounding paragraphs (p.361-364) go on to describe a printed reset
*protocol* around this trigger — a `$78` warning code, a KCLK/KDAT
handshake, a 500 ms pulse — none of which is in this claims file. Those
sentences are not silently absorbed into this proposition; if they belong
to `ch8-keyboard-events` at all rather than to a sibling package or to
`not-asserted`, that is a question for those sentences' own
classification, not answered by expanding this proposition's scope beyond
what `ch8-keyboard-events.jsonl`'s own 17 claims license.

Status: graded (RESET-COMBO-FIRES-ON-ALL-THREE, and
RESET-COMBO-PARTIAL-DOES-NOT-FIRE as its control), GREEN.

---

## Claims not assigned an assertion

None. 17 claims are classified `testable` in `ch8-keyboard-events.jsonl`
and each is assigned to exactly one of the first eight propositions above;
the 4 Caps Lock LED claims are classified `not-asserted` in
`2026-09-16_ch8_s3_classified_all.jsonl` on a ground "The Caps Lock LED
claims" above shows does not hold, and are assigned to `PROP-C4` and
`PROP-C5` (see the inventory table for the exact uid-to-proposition
mapping, all 21 rows).

## What this specification deliberately does not contain

Port names, signal names, stimulus encodings (including HID key codes),
concrete sequencing (how a "press" or "release" is driven at this boundary,
or how many clock cycles apart) and the mutation design belong in the test
source, not here. The ten PROPs above are the propositions that become this
package's `expect()` assertions, each with its own control case and
stimulus mutation.

## Outcome

`make -B test`: 13 passed, 2 failed. The two RED assertions are
CAPS-LOCK-RELEASE-NOT-ACCEPTED (`PROP-C2`) and
CAPS-LOCK-LED-OFF-PRESS-BIT-ONE (`PROP-C5`).
