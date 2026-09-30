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

Pass criterion (field-length parity alone would test only the
*mechanism* behind the resolution loss, not the resolution-loss claim
itself): with LACE on, at least one vpos value that lies within both
field lengths is observed at both field-identity states (field1=0 and
field1=1) across two field passes — i.e. the same digital position reading
recurs while the field alternates, so vpos alone does not distinguish the
two occurrences. With LACE off this does not occur (field identity does not
alternate at all, so nothing recurs across two distinguishable states).

Superseded criterion, kept for the record: "with LACE on, successive
fields' total line counts differ by exactly one line (one field N lines,
the next N+1, alternating); with LACE off, every field has the same line
count." (Chapter 7 previously measured N=311/312 on this same fixture;
this package re-measured its own build rather than reusing that number.)
This fact is true and still measured by the built package (as a note, not
a graded claim): it is the textbook mechanism by which interlacing
achieves its half-line raster offset, but showing field lengths differ is
not the same as showing a *reading* is ambiguous, so it cannot carry
opus:8-lpen.c29/sol:8-pen.c30 on its own.

### P3 — Frame identity recovers the position (uid opus:8-lpen.c30, sol:8-pen.c31)

> 2 scan lines in interlaced mode (However, if you know
> which interlaced frame is under display, you can
> determine the correct position)

Proposition: the frame/field identity the manual says you must "know" to
disambiguate is itself something this chip exposes and updates once per
field, in step with the field alternation of P2 — i.e. the disambiguating
information is available at this boundary, not merely asserted to exist
elsewhere.

Pass criterion (merely showing the bit toggles is not
yet "you can determine the correct position"): across four consecutive
field boundaries with LACE on, the field-identity bit is not merely
alternating but is a reliable, checkable predictor of which field-length
regime (the 311- or 312-line field measured under P2) is active — every
311-line field carries the same field-identity value and every 312-line
field carries the other, with both regimes actually observed. That is what
makes the bit decodable, checkable information a program can use, rather
than an arbitrary flag: it is the literal content of "if you know which
interlaced frame is under display, you can determine the correct
position." This does not reconstruct the physical CRT line itself — see
"still open" below; it establishes that field identity is genuine,
verifiable information, which is the assertable half at this boundary.

Superseded criterion, kept for the record: "a field-identity bit is
readable, and it changes value (alternates) once per field boundary when
LACE is on, in lock-step with the long/short field alternation measured
under P2. Under LACE off it does not alternate." Alternation alone does not
establish that the bit *recovers* anything; it only shows the bit is not
stuck.

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

Disposition: opus:8-lpen.c45 and sol:8-pen.c42 are declined, not carried by this
proposition's assertion. Both uids quote the manual's read-twice / mask /
compare procedure verbatim — a sequence performed by software on values
already read from VPOSR/VHPOSR — and reading that procedural text as a
register-layout claim would turn a software operation into a
register-layout assertion, which it is not. Declined instead. The
layout fact itself (the masked remainder still discriminates distinct
positions) remains real and is still measured, but it now carries no uid:
what discriminates positions is not what either uid's own words describe.
Additionally, agnus_beamcounter has no LPEN input and no freeze-on-trigger
logic anywhere in this pinned RTL tree (confirmed by reading the module and
grepping the tree: only a commented-out `lpendis` wire here and a
commented-out `lpen`/BPLCON0[3] wire in denise.v), so the "read twice and
require them equal" precondition that gives the mask-and-compare step its
meaning cannot be exercised at this or, as built, any module boundary in
this suite. What would carry it: a CPU/firmware-level test running the
actual detection routine against a system model that implements the LPEN
freeze — no such model exists in this submodule.

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

12
of the 14 uids in the claims file are asserted by P1-P6 (via the
P2/P3 criteria above). Two are declined, not asserted:

- opus:8-lpen.c45 and sol:8-pen.c42 quote the manual's software read-twice
  / mask / compare procedure, not a hardware layout fact. `agnus_beamcounter`
  has no LPEN input and no freeze-on-trigger logic anywhere in this pinned
  RTL tree, so the "two reads equal" precondition the procedure depends on
  cannot be exercised at this or any module boundary as built. See P5's
  disposition note above for the full reasoning. What would carry it: a
  CPU/firmware-level test running the actual routine against a system model
  that implements the LPEN freeze — no such model exists in this submodule.

This may change again if a future build carries an LPEN/freeze model — that
would be a re-open, not something to resolve on this package's own
authority.

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
