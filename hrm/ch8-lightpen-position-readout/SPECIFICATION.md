# ch8-lightpen-position-readout

Unit: `agnus_beamcounter` (compiled alone).
Oracle: Amiga Hardware Reference Manual, printed page 239 (light pen position
register resolution table, and the light-pen-trigger detection procedures
that follow it).
Chipset scope: OCS baseline. Page 239 names no chip set anywhere in its
surrounding chapter (pp. 227-241); every claim below carries that as an
unwritten "on OCS". Where the fixture also implements ECS/AGA extensions,
those bits are out of scope for the propositions on this page and are held
at their OCS-quiescent values unless a proposition forces otherwise.

This document is written from the quotations in the claims file alone. It
does not name any RTL port, signal, address decode, or line number: those
belong to step 2 (build), after this file is committed and the RTL is
opened. Register names used below (VPOSR, VHPOSR, BPLCON0, vpos, hpos, LACE)
are the manual's own chip-register vocabulary, established in this book's
register map (already built and graded elsewhere in this suite as
`hrm/ch3-register-map-agnus-beamcounter`), not RTL implementation detail.

## Propositions

### P1 — Vertical resolution, non-interlaced (uid opus:8-lpen.c28, sol:8-pen.c29)

> Vertical      1 scan line in non-interlaced mode

Proposition: with the display in non-interlaced (LACE off) mode, the
vertical beam position (vpos) advances by exactly one count per scan line —
no more, no fewer.

Pass criterion: over a run of N scan lines (N end-of-line events), the
number of vpos increments observed equals N exactly (ratio 1.00).

### P2 — Vertical resolution, interlaced (uid opus:8-lpen.c29, sol:8-pen.c30)

> 2 scan lines in interlaced mode (However, if you know
> which interlaced frame is under display, you can
> determine the correct position)

Proposition (assertable half only — see P3 for the frame-identity half; the
manual's own parenthetical is the second half, tested separately): with LACE
on, an interlaced frame is made of two fields, and the two fields are not
identical in length — one field is one scan line longer than the other. A
single vpos value read without field identity therefore denotes either of
two distinct physical lines, i.e. one field's set of lines is offset by one
line from the other field's set. This is the mechanism that halves the
resolution obtainable from vpos alone. The interleaving of those lines on a
physical CRT is outside the chip and is not asserted.

Pass criterion: with LACE on, successive fields' total line counts differ
by exactly one line (one field N lines, the next N+1, alternating); with
LACE off, every field has the same line count. (Chapter 7 previously
measured N=311/312 on this same fixture; this package re-measures its own
build rather than reusing that number — see build notes.)

### P3 — Frame identity recovers the position (uid opus:8-lpen.c30, sol:8-pen.c31)

> 2 scan lines in interlaced mode (However, if you know
> which interlaced frame is under display, you can
> determine the correct position)

Proposition: the frame/field identity the manual says you must "know" to
disambiguate is itself something this chip exposes and updates once per
field, in step with the field alternation of P2 — i.e. the disambiguating
information is available at this boundary, not merely asserted to exist
elsewhere.

Pass criterion: a field-identity bit is readable, and it changes value
(alternates) once per field boundary when LACE is on, in lock-step with the
long/short field alternation measured under P2. Under LACE off it does not
alternate.

### P4a — Horizontal resolution, low resolution display (uid opus:8-lpen.c31 [shared with P4b], sol:8-pen.c32)

> Horizontal    2 low resolution pixels in either high or low resolution

Proposition (low-resolution half): in low-resolution display mode, the
horizontal beam position register resolves position in steps of 2
low-resolution-pixel units — i.e., two adjacent low-resolution-pixel
positions of the beam read back as one and the same horizontal register
value, and the next pair reads back as the next value.

Pass criterion: forcing hpos through a run of consecutive low-resolution-
pixel positions, the horizontal position readout changes at half that rate
(2:1).

### P4b — Horizontal resolution, high resolution display (uid opus:8-lpen.c31 [shared with P4a], sol:8-pen.c33)

> Horizontal    2 low resolution pixels in either high or low resolution

Proposition (high-resolution half): the same 2-low-resolution-pixel
granularity holds when the display is in high-resolution mode — the
horizontal position register's step size does not shrink to match the
finer high-resolution pixel.

Pass criterion: with the display's resolution control set to high
resolution, the horizontal position readout still changes at the 2:1 rate
measured under P4a, for the same underlying beam movement.

### P5 — Masked long-word layout carries position, not the whole word (uid opus:8-lpen.c45, opus:8-lpen.c51 [shared with P6], sol:8-pen.c42)

> 3. If both values are the same, mask off the upper 15 bits of the
>    32-bit word and compare it with the hex value of $10500 (V = 261).

(and, restated at the same page under the simplified method:)

> 2. Mask off the upper 15 bits of the 32-bit word and compare it
>    with the hex value of $10500 (V = 261).

Proposition (layout half only — the masking-as-a-CPU-operation and the
comparison itself are procedural steps performed by software, not
assertable at this boundary): the manual's 32-bit word — the vertical and
horizontal position registers read and combined into one long word — still
carries position-discriminating information after its upper 15 bits are
discarded. That is, the masked remainder is not constant; it still
distinguishes different beam positions from one another.

Pass criterion: for at least three distinct, known vertical positions
(adjacent to V=261, see P6), the masked remainder of the composed long word
takes three distinct values, ordered the same way the underlying positions
are ordered.

### P6 — $10500 encodes V=261 (uid opus:8-lpen.c46, opus:8-lpen.c51 [shared with P5], sol:8-pen.c43)

> 3. If both values are the same, mask off the upper 15 bits of the
>    32-bit word and compare it with the hex value of $10500 (V = 261).

(and, restated at the same page under the simplified method:)

> 2. Mask off the upper 15 bits of the 32-bit word and compare it
>    with the hex value of $10500 (V = 261).

Proposition (encoding half): at vertical position V=261 (with horizontal
position at its origin, per the manual's own worked example), the masked
32-bit word equals exactly $10500.

Pass criterion: with vpos forced to 261 and hpos forced to its origin, the
composed-and-masked long word reads exactly 0x10500 — neither more nor
less.

Control basis for P5/P6: the same measurement at the two positions
immediately adjacent to V=261 (V=260, V=262) is expected to read something
other than 0x10500 (less than, and greater than, respectively), on the same
measurement basis, so a positive P6 result cannot be produced by a
comparator that accepts anything.

## Claims not turned into propositions

None. All 14 uids in the claims file (opus:8-lpen.c28, c29, c30, c31, c45,
c46, c51; sol:8-pen.c29, c30, c31, c32, c33, c42, c43) are covered by P1-P6
above. This may change at step 2 if the RTL does not carry a signal one of
these propositions needs at this module's boundary — that would be a HALT
and a decline naming the missing signal and the module that would carry it,
not a wider fixture chosen on this package's own authority.

## Still open — resolved only after the RTL is read (step 2)

- Which register/signal at the `agnus_beamcounter` boundary is VPOSR,
  which is VHPOSR, and how BPLCON0's LACE and resolution bits reach the
  module (write-decoded internally, or ports) — needed to drive/observe
  the above, not to state the above.
- Whether a field-identity signal (P3) is actually present at this
  module's boundary. If it is not, P3 is a HALT/decline, not a wider
  fixture.
- Exact bit positions of vpos/hpos within VPOSR/VHPOSR at this boundary,
  and how the two combine into the manual's "32-bit word" (P5/P6) — the
  manual's own text on this page states the $10500/V=261 correspondence
  directly (quoted above); it does not, in what is quoted here, state the
  composition step itself, so composition is test-harness plumbing, not an
  additional oracle claim.
- Mutation design per assertion (stimulus mutation, or a stated
  impossible/unfalsifiable-vs-scope reason), and the harness's own
  liveness check (distinct forced vpos/hpos values must read back as
  distinct values before any proposition above is graded) — both are
  build-time concerns, recorded in the built package's file header per the
  suite's convention, not this document.
