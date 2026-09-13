# Specification: ch7-collision-model

Fixture: `denise_collision`
Claims file: `ch7-collision-model.jsonl` (9 claims, chapter 7 body, printed p.213)

Written from the claim quotations alone, before the RTL was opened. Per
BUILD-BRIEF, this file names propositions and test intent only; it does not
name module port names or expected bit-level values. Where a measurement
basis below refers to "a sprite group," "a playfield," or "CLXDAT," those are
the manual's own vocabulary (page 213 and, for orientation only, the already
crossread p.213 neighbourhood), not a port list read off the RTL.

## Why this package exists

Page 213 states the collision model in two sentences: which *classes* of
object pairs can collide (sprite-to-sprite, sprite-to-playfield,
playfield-to-playfield, and any combination), and *when* a collision is
registered at all (two or more objects, at the same pixel position, which
then sets a bit in the collision data register). `ch3-collision-registers`
already asserts the appendix-A bit-by-bit CLXCON/CLXDAT table (98 claims,
165 of this chapter's citations routed there). This package's own subject,
per its task sheet, is narrower and does not restate that table: the
same-pixel requirement is the load-bearing part, and it is what distinguishes
this package from the existing coverage. No existing package measures
whether presenting two objects *at different pixels* (never co-resident)
fails to collide, as opposed to measuring which bit lights up when they are
co-resident.

## Relationship to `hrm/denise_collision` (pre-pipeline, proposed for replacement)

That harness runs two checks, both against this same fixture:

1. Sprite 0 + Sprite 2 presented together → CLXDAT bit 9. This is the same
   proposition as this package's A1 (sprite-group-to-sprite-group collision).
   A1 covers its intent, and improves on it with an explicit "each sprite
   alone registers nothing" contrast leg, which the original check does not
   have.
2. Playfield-1 + Sprite 0 presented together, driven with `bpldata=0xFF` →
   CLXDAT bit 1. Per the task sheet, `bpldata=0xFF` makes *both* playfields
   present at once (every bitplane matches), so this check is green for the
   wrong reason: it does not isolate playfield 1, and with a genuinely
   isolating stimulus (`bpldata=0x55`, playfield 1 present, playfield 2
   absent) the same proposition reads bit 1 **clear** in single-playfield
   mode — because oddmatch is gated by `(dblpf | evenmatch)` in the RTL, a
   mode-dependent anomaly already assigned to `ch7-collision-playfield-mode`
   (page 214), not to this package.

   This package's own A2 (sprite-group-to-either-playfield collision) is
   page 213's bare capability statement, not page 214's mode comparison. To
   assert it without reproducing the flawed check's accident, or without
   accidentally re-measuring the page-214 anomaly under a different name, A2
   must be measured in a mode where an isolated single-playfield stimulus is
   unambiguously expected to work — dual-playfield mode (`dblpf=1`) — the
   same choice `ch4-sprite-collision` already made for the identical concern
   at its own page (113). Single-playfield mode's isolated-stimulus result is
   `ch7-collision-playfield-mode`'s own measurement (its A1/A3/A4), not
   restated here. So: **this package's A2 covers the same underlying
   capability the old check aimed at, deliberately not by the same means**,
   and the old check's specific defect is neither repeated nor silently
   re-asserted under this package's name.

## Propositions and assertions

### A1 — SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION (p.213)

Claims: `sol:7-collision.c1`, `opus:7-cd.c1`

Quote: "You can use the hardware to detect collisions between one sprite
group and another sprite group, any sprite group and either of the
playfields, the two playfields, or any combination of these items." (this
package's slice: "one sprite group and another sprite group")

**Proposition:** the hardware detects a collision between one sprite group
and another sprite group.

**Form of the assertion:** two distinct sprite groups presented together at
one pixel → the CLXDAT bit for that sprite-group pair reads set. Each of the
two groups presented alone (not together) → that same bit reads clear.

**Control case:** the alone/together contrast is itself a same-measurement-
basis control: the "alone" legs are expected PASS results (bit clear) on
exactly the apparatus the "together" leg is measured with.

**Mutation:** withhold the second sprite group from the "together" stimulus
(present only the first) → the assertion goes red because the bit that was
expected set no longer is.

### A2 — SPRITE-GROUP-TO-PLAYFIELD-COLLISION (p.213)

Claims: `sol:7-collision.c2`, `opus:7-cd.c1`

Quote: same sentence as A1; this package's slice: "any sprite group and
either of the playfields"

**Proposition:** the hardware detects a collision between a sprite group and
a playfield.

**Form of the assertion:** measured in dual-playfield mode (`dblpf=1`, see
"Relationship to `hrm/denise_collision`" above for why), with a stimulus that
isolates one playfield's presence (its own bitplanes matching, the other
playfield's bitplanes genuinely not matching — not merely CLXCON-disabled)
together with a sprite group at one pixel → the corresponding CLXDAT bit
reads set. The sprite group presented alone, with no playfield content
present → that bit reads clear.

**Non-goal:** this assertion does not probe whether the result changes
between `dblpf=0` and `dblpf=1`, and does not use an un-isolated stimulus
(e.g. all bitplanes matching at once) that would leave both playfields
present. That comparison, and the odd/even-bitplane-to-playfield equations
it depends on, are `ch7-collision-playfield-mode`'s own assigned
propositions.

**Control case:** the alone/together contrast, as in A1.

**Mutation:** withhold the sprite group's presence from the "together"
stimulus (present only the isolated playfield) → the assertion goes red.

### A3 — PLAYFIELD-TO-PLAYFIELD-COLLISION (p.213)

Claims: `sol:7-collision.c3`, `opus:7-cd.c1`

Quote: same sentence as A1; this package's slice: "the two playfields"

**Proposition:** the hardware detects a collision between the two
playfields.

**Form of the assertion:** both playfields genuinely present (their own
bitplanes matching) together at one pixel → the playfield-to-playfield
CLXDAT bit reads set. Only one playfield present → that bit reads clear.

**Control case:** the alone/together contrast, as in A1.

**Mutation:** withhold one playfield's presence from the "together" stimulus
→ the assertion goes red.

### A4 — COMBINATION-OF-CLASSES (p.213)

Claims: `sol:7-collision.c4`, `opus:7-cd.c1`

Quote: same sentence as A1; this package's slice: "or any combination of
these items"

**Proposition:** the hardware can detect a combination of these collision
classes together, not only one class at a time.

**Form of the assertion:** a single pixel staged so that a sprite-to-sprite
pair, a sprite-to-playfield pair, and a playfield-to-playfield pair are all
simultaneously true → one CLXDAT read shows all three corresponding bits set
together.

**Control case:** contrasted against A1/A2/A3's own single-class stimuli,
which each set only their own bit — establishing that the combined read is
not merely one dominant bit masquerading as three.

**Mutation:** withhold one of the three staged classes from the combined
stimulus (the stimulus that produces the other two is left intact) → the
assertion goes red, since it demands all three bits together and now only
two are set.

### A5 — MULTIPLICITY-REQUIRED (p.213)

Claims: `sol:7-collision.c6`, `opus:7-cd.c4`

Quote: "Collisions are detected when two or more objects attempt to overlap
in the same pixel position." (this package's slice: "two or more objects")

**Proposition:** a collision requires at least two objects; one object alone
must not register anything.

**Form of the assertion:** at least two distinct single-object stimuli (a
sprite group alone, and a playfield alone), each presented with nothing else
live, each independently produce an all-clear CLXDAT read.

**Control case:** doubly satisfied by A1/A2/A3's own "together" legs, which
are the identical apparatus showing a bit does set once a second object
genuinely joins the first.

**Mutation:** turn one "alone" leg into a "together" stimulus (add the
second object back) → the "must stay clear" assertion goes red, showing it
is sensitive to multiplicity rather than vacuously true.

### A6 — SAME-PIXEL-REQUIRED (p.213) — load-bearing

Claims: `sol:7-collision.c6`, `opus:7-cd.c4`

Quote: same sentence as A5; this package's slice: "attempt to overlap in the
same pixel position"

**Proposition:** two objects collide only if they overlap at the *same*
pixel position; presence at different pixels/cycles, never co-resident, must
not register a collision.

**Form of the assertion:** the same two objects that A1 (or A4) shows DO
collide when presented together at one pixel are instead presented one at a
time across two consecutive pixels — object A alone at pixel N, object B
alone at pixel N+1, at no point co-present → the CLXDAT read must show no
collision for that pair.

**Control case:** the same-pixel case (A1's "together" leg) is the required
known-good control on the same measurement basis: if the different-pixel
leg ever showed the bit set, that would be the finding this package exists
to catch, per the task sheet's framing of the same-pixel requirement as the
load-bearing, distinguishing part of this package.

**Mutation:** change the different-pixel stimulus so both objects land at
the same pixel after all (the opposite of the assertion's own precondition)
→ the "must stay clear" expectation flips to red, confirming the assertion
is actually sensitive to the same-pixel condition and not a vacuous
always-pass.

### A7 — COLLISION-SETS-CLXDAT-BIT (p.213)

Claims: `sol:7-collision.c7`, `opus:7-cd.c5`

Quote: "This will set a bit in the collision data register."

**Proposition:** a detected collision sets a bit in the collision data
register (CLXDAT) — it is visible by reading that register, not merely an
internal condition.

**Form of the assertion:** CLXDAT read idle (nothing presented) → reads with
no relevant bit set. Stage a genuine same-pixel collision; read CLXDAT again
→ reads with the corresponding bit set, a changed, non-zero value against
the idle read.

**Control case:** the idle/after contrast is itself the control, on the same
apparatus as every other read in this package.

**Mutation:** withhold the collision-causing stimulus from the "after"
measurement (present nothing, identical to idle) → the "idle vs. after must
differ" assertion goes red.

## Harness control (not a claims-file proposition)

FIXTURE-LIVE-DATA-PATH: before A1–A7 are trusted, two distinct, known
collision stimuli must produce two distinct, correctly-positioned CLXDAT
readings through the write/stimulate/read path. This is the chapter-6
lesson named in BUILD-BRIEF (a silently dead data path reports every
data-dependent assertion as a finding about the design) applied here; it
carries no claim id and is not counted against the 9.

**Mutation:** repeat the first stimulus as the second instead of a
genuinely different one → the "must be distinct" assertion goes red.

## Claims accounted for

| Assertion | Claims |
|---|---|
| A1 | `sol:7-collision.c1`, `opus:7-cd.c1` |
| A2 | `sol:7-collision.c2`, `opus:7-cd.c1` |
| A3 | `sol:7-collision.c3`, `opus:7-cd.c1` |
| A4 | `sol:7-collision.c4`, `opus:7-cd.c1` |
| A5 | `sol:7-collision.c6`, `opus:7-cd.c4` |
| A6 | `sol:7-collision.c6`, `opus:7-cd.c4` |
| A7 | `sol:7-collision.c7`, `opus:7-cd.c5` |

`opus:7-cd.c1` is the whole-sentence twin of `sol:7-collision.c1`–`c4` (per
the claims file's own twin note: "the split is the one to cite for
coverage... keep this record as the whole-sentence citation on the same
assertions") and is cited on all four of A1–A4, not spawned as a fifth
assertion. `opus:7-cd.c4` and `opus:7-cd.c5` are likewise identical-quote
twins of `sol:7-collision.c6` and `c7` respectively and are cited alongside
them rather than separately. All 9 claims are accounted for; none is
declined.

## Chipset scope

All 9 claims classify `applies_to: [OCS, ECS]` — appendix C is measured
silent on collision, so nothing here is OCS-only. No chipset-conditional
assertion is required.

## HALT conditions

None encountered while writing this specification. The claims file supplied
enough — three named collision classes, their combination, the two-or-more/
same-pixel condition, and CLXDAT as the register a collision sets a bit in —
to state every proposition testably without reading the RTL or knowing any
port name. If, once the RTL is opened, `denise_collision` does not expose
per-pixel control over sprite-group presence, playfield presence, and a
dual-playfield mode select independently of each other, or does not expose
CLXDAT as a readable multi-bit register, that is a HALT to report, not
something to work around.
