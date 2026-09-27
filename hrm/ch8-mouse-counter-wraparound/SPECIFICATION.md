# ch8-mouse-counter-wraparound

Subject: what the Amiga Hardware Reference Manual, page 231, says about the
mouse/trackball counters at their limits — wraparound in both directions, the
0..255 range, the sign-gives-direction rule, and Table 8-2's four worked
examples with their footnotes. This package does **not** test the counters'
bit layout inside JOY0DAT/JOY1DAT (that is `ch8-joydat-counter-layout`) and
does **not** test which physical mouse motion produces which
quadrature phase order (that is `ch8-mouse-quadrature-decode`).

Register identification: the manual text quoted below never names a register.
That the "counters" on page 231 are JOY0DAT/JOY1DAT (the CIA-adjacent custom
chip registers read by software to sample the mouse) is not derived by this
package from the page; it is taken from `ch8-joydat-counter-layout`, which
grades the counters' layout inside those registers on the same fixture.

All 22 claims in `ch8-mouse-counter-wraparound.jsonl` are page 231. All are
`"status": "testable"`. All are `"observable": "yes, at userio"`.

## Fixture liveness (not a page-231 claim)

Before any proposition below is graded, the harness drives the horizontal and
vertical counters to two distinct, small, chosen values from a known reset
state and reads them back, on both JOY0DAT and JOY1DAT independently, to prove
the stimulus-to-observation path is alive and that the two bytes are not
aliased to each other.

## Propositions, one assertion per proposition

### A1 — wraps in the positive direction
uids: `opus:8-limits.c1`, `sol:8-mouse-limit.c1`
quote (opus, c1): "These counters will \"wrap around\" in either the positive
or negative direction. If you wish to use the mouse to control something that
is"
quote (sol, c1): "These counters will \"wrap around\" in either the positive
or negative direction."
Assertion: driving the counter up past its maximum (255) with one more
incrementing step lands it at 0, not at 256 or any other value.
Measured: GREEN.

### A2 — wraps in the negative direction
uids: `opus:8-limits.c2`, `sol:8-mouse-limit.c2`
Same quote as A1.
Assertion: from 0, one decrementing step lands the counter at 255.
Measured: GREEN.

### A3 — the sign of the difference indicates direction
uids: `opus:8-limits.c11`, `sol:8-mouse-limit.c11`
quote (opus, c11): "value of the difference will represent the speed. The
sign of the difference (positive or negative) lets you determine which
direction the mouse is traveling."
quote (sol, c11): "The sign of the difference (positive or negative) lets you
determine which direction the mouse is traveling."
Scope: this module's ports carry no separate "direction" signal — Table 8-1's
pin map and Figure 8-2's phase ordering (which physical direction is which
sign) belong to `ch8-mouse-quadrature-decode`. What is testable here, with
the counter alone, is only that the sign is a *reliable, reversing* indicator:
driving one fixed phase order gives one sign, reversing the phase order gives
the other sign, on the same counter, without needing to know which sign is
physically "up".
Assertion: after N incrementing steps, (current − previous) > 0; after N
decrementing steps from the same starting point, (current − previous) < 0.
Measured: GREEN.

### A4 — new value minus previous value equals the count of mouse pulses since the last check
uids: `opus:8-limits.c13`, `sol:8-mouse-limit.c12`
quote (opus, c13): "arithmetic. The new value of a counter minus the previous
value will represent the number of mouse counts since the last check. The
example"
quote (sol, c12): "The new value of a counter minus the previous value will
represent the number of mouse counts since the last check."
Note: sol's classification gloss ("8-bit signed arithmetic") is the reader's
own addition, not present in the quoted text; this assertion tests only what
the quote states — the raw difference equals the pulse count — leaving the
signed/unsigned framing to A5.
Assertion: for several distinct pulse counts (no wraparound in range), the
byte-difference (current − previous) equals exactly the number of quadrature
state changes driven.
Measured: GREEN.

### A5 — treated as unsigned, the counters range 0..255
uid: `opus:8-limits.c14`
quote: "shown in Table 8-2 presents an alternate method. It treats both
counts as unsigned values, ranging from 0 to 255. A count of 100 pulses is
measured"
Assertion: the counter is observed to reach 255 and never observed above 255.
Measured: GREEN.
Mutation note: the upper bound of an 8-bit hardware register cannot be
exceeded by any input stimulus — this is **unfalsifiable at this boundary**, not a scope choice.
The wrap-point precision (that 255, not any other value, is where it turns
over) is what A1/A2 already falsify via stimulus.

### A6 — every row of Table 8-2 is a "100 pulses measured" example
uid: `opus:8-limits.c15` (flagged `borderline` by the reader)
quote: "unsigned values, ranging from 0 to 255. A count of 100 pulses is
measured in each case."
Assertion: for all four rows of Table 8-2, the previous→current transition
is realized by exactly 100 quadrature state changes.
Measured: **RED**. Rows 1–2 (200→100, 100→200) involve no wraparound and are
exactly 100 steps. Rows 3–4 (200→45, 45→200) wrap; on this 8-bit (mod-256)
counter reaching the stated current value from the stated previous value
takes 101 state changes, not 100 (see A9/A10) — a genuine hardware/manual
mismatch, not a units error in this harness.

### A7 — Table 8-2 row 1: previous 200, current 100
uids: `opus:8-limits.c16`, `sol:8-mouse-limit.c13`
quote: "                  200         100      Up (Left)"
Scope: "Up (Left)" is a direction label with no corresponding output port on
this module (see A3's scope note); what's falsifiable here is the row's
numeric content.
Assertion: 100 decrementing state changes from 200 land the counter at 100.
Measured: GREEN. Serves as **control case**: same measurement basis (edges
driven vs. byte delta) as the RED A6/A9/A10, on a transition with no
wraparound.

### A8 — Table 8-2 row 2: previous 100, current 200
uids: `opus:8-limits.c17`, `sol:8-mouse-limit.c14`
quote: "                  100         200     Down (Right)"
Assertion: 100 incrementing state changes from 100 land the counter at 200.
Measured: GREEN. Second control case, opposite direction of edges from A7.

### A9a — Table 8-2 row 3 + footnote *, count half: previous 200, current 45
uids: `opus:8-limits.c20`, `sol:8-mouse-limit.c17`
quote (footnote, opus c20): "   *  Because 200-45 = 155, which is more than
127, the true count must be 255 - ( 200-45) = 100; the direction is down."
The footnote is what turns the bare row into a concrete, falsifiable number:
it claims the "true count" — the number of state changes needed to realize
this transition — is 100. That is the proposition graded here.
Assertion: the number of quadrature state changes actually required to move
the counter from 200 to 45 equals the manual's stated true count, 100.
Measured: **RED**. On a mod-256 counter, reaching 45 from 200 takes 101 state
changes (200 + 101 = 301 ≡ 45 mod 256), not 100 — the manual's formula uses
255 where the hardware's modulus is 256. Whether the printed page is wrong is
for Amiga specialists, not for this suite; the discrepancy is what is
gradeable and is reported red, unadjusted.

### A9b — Table 8-2 row 3, direction half: previous 200, current 45, "Down"
uids: `opus:8-limits.c18`, `sol:8-mouse-limit.c15`
quote (row, opus c18): "                  200          45       Down *"
The bare row's own content — beyond the numeric footnote graded in A9a — is
the direction label "Down". Scope, same boundary as A3: this module carries
no direction port, and which physical mouse motion is "up" vs "down" is
Table 8-1's pin map / Figure 8-2's waveform, out of scope for this package
(`ch8-mouse-quadrature-decode`'s job). What is testable at this boundary: A7
realized row 1 ("Up (Left)") by driving decrementing edges; A8 realized row 2
("Down (Right)") by driving incrementing edges. Row 3 carries row 2's label
("Down"), so its true (short, wraparound) path should be realized by the same
edge sense as row 2: incrementing.
Assertion: of the two edge senses (incrementing, decrementing) that can walk
the counter from 200 to 45 on this 8-bit wrapping register, the one that
does so in the fewer state changes is the incrementing sense — the same sense
A8 used for row 2's "Down" label.
Measured: GREEN.

### A10a — Table 8-2 row 4 + footnote **, count half: previous 45, current 200
uids: `opus:8-limits.c21`, `sol:8-mouse-limit.c18`
quote (footnote, opus c21): "   ** 45 - 200 = -155.  Because the absolute
value of -155 exceeds 127, the true count must be  255 + (-155) = 100; the
direction is up."
Assertion: the number of quadrature state changes actually required to move
the counter from 45 to 200 equals the manual's stated true count, 100.
Measured: **RED**, by the same mod-256 arithmetic as A9a: 45 − 101 ≡ 200
(mod 256) (45 − 101 = −56, and −56 + 256 = 200) — 101 state changes are
required, not 100.

### A10b — Table 8-2 row 4, direction half: previous 45, current 200, "Up"
uids: `opus:8-limits.c19`, `sol:8-mouse-limit.c16`
quote (row, opus c19, full table as extracted): "                Previous
Current\n                 Count       Count     Direction\n                --------
-------    ---------\n                  200         100      Up (Left)\n
100         200     Down (Right)\n                  200          45       Down
*\n                   45         200        Up **"
Same boundary as A9b, mirrored. Row 4 carries row 1's label ("Up"), so its
true (short) path should be realized by the same edge sense A7 used for row
1: decrementing.
Assertion: of the two edge senses that can walk the counter from 45 to 200,
the one that does so in the fewer state changes is the decrementing sense —
the same sense A7 used for row 1's "Up" label.
Measured: GREEN.

## Mutations

- A1, A2, A3, A4, A7, A8, A9b, A10b are GREEN under nominal stimulus:
  each gets a **stimulus mutation** (change the driven edge count, edge
  direction, or target value so the module is asked to do something
  different; expected value untouched; confirm the assertion goes red;
  revert). For A9b/A10b the driven quantity is the target value the counter
  is walked to, since that is what determines which edge sense comes out
  shorter.
- A5 is GREEN and structurally cannot be driven red by any stimulus (8-bit
  register width) — recorded as `impossible`, unfalsifiable at this boundary.
- A6, A9a, A10a are RED already. A red assertion is not mutated further with
  stimulus (it cannot fail harder); instead each is
  **substituted**: the same comparator (required-edges == observed-edges) is
  run against a known-live mechanism proven elsewhere in this same package —
  the exact-100, no-wrap transition of A7/A8 — and shown to go GREEN, proving
  the comparator is not vacuously always-false.

## Coverage and outcome

All 22 uids resolve to one of the twelve assertions above. `make -B test`:
15 passed, 3 failed — A6, A9a and A10a are RED (Table 8-2's wrapped rows
take 101 state changes on this mod-256 counter, not 100); the other nine
assertions, the three fixture-liveness checks and the three substitute
checks are GREEN.
