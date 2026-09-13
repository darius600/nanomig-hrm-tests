# SPECIFICATION -- ch7-sprite-priority-groups

Fixture: `denise_spritepriority`
Claims file: `ch7-sprite-priority-groups.jsonl` (18 claims, readers `sol` and `opus`)

Written from the quotations in the claims file alone, before any RTL was opened.
No port name, signal name, or file/line reference appears below; none is known
to this document's author yet.

## Subject and scope

Printed page 208 (Amiga Hardware Reference Manual, chapter 7):

> For playfield priority and collision purposes only, sprites are treated as
> four groups of two sprites each.
>
>      Sprites 0 and 1
>      Sprites 2 and 3
>      Sprites 4 and 5
>      Sprites 6 and 7

(quoted independently by both readers: `sol:7-video.c2`/`sol:7-video.c3..c6`
and `opus:7-vp.c4`/`opus:7-vp.c5..c8`)

Printed page 209, Figure 7-2, described by both readers as a hand with a
"Sprite Groups" finger group carrying exactly four labels -- SP01, SP23, SP45,
SP67 (`sol:7-fig7-2.c1..c4`, `opus:7-fig7-2.c2..c5`). Opus's reading of the
figure adds an exhaustiveness clause each time: those four labels are the
**only** sprite-group labels in the frame, so the eight sprites form four such
pairs "and no other grouping."

Per the task sheet, this package's subject is the four two-sprite groups as
they are treated for **playfield priority**. Two things are explicitly out of
scope here and are not asserted by this package (see "Descoped" below):

- the **collision** half of the "for playfield priority and collision
  purposes" sentence -- flagged in both `sol:7-video.c2` and `opus:7-vp.c4`'s
  own `split` as belonging to a collision-boundary package, not this one;
- **within-pair order** (which of the two sprites in a group wins) -- flagged
  by the task sheet as not observable at this boundary; that belongs to
  `ch7-inter-sprite-priority` on `denise_sprites`.

What IS in scope, and what the quotations above support, is exactly two kinds
of proposition about this boundary's treatment of playfield priority:

1. **Membership**: sprites 0&1, 2&3, 4&5, 6&7 are each treated as one unit.
2. **Exhaustiveness**: no other grouping exists -- in particular the four
   groups are mutually distinct, so a sprite at the edge of one group (1, 3,
   5, 7) is not silently folded into its numeric neighbour's group (2, 4, 6,
   0 respectively).

Chipset scope: every one of the 18 claims marks `applies_to: OCS`, reasoned
from printed page 224 ("For information on the system control registers in
the Enhanced Chip Set (ECS), see Appendix C") -- i.e. the manual body read
here is the OCS account, and nothing in the quotations above says whether ECS
groups sprites differently. This package therefore asserts OCS behaviour.
Whether the same RTL path also serves ECS is a question for step 3 (it is not
answered by any quotation), not asserted here.

## Assertions

Each assertion compares the boundary's playfield-priority outcome for one
stimulus against another, holding everything else constant. "Outcome" here
means whatever this boundary produces to indicate which sprite (or group) it
currently favours for playfield priority -- deliberately not named further
until the RTL is open.

| # | ID | Page | Proposition | uids |
|---|----|------|--------------|------|
| 1 | `SPGRP-01-SAME` | 208/209 | Presenting sprite 0 alone as the priority-seeking sprite and presenting sprite 1 alone must yield the identical outcome -- they are one group. | `sol:7-video.c3`, `opus:7-vp.c5`, `sol:7-fig7-2.c1`, `opus:7-fig7-2.c2` |
| 2 | `SPGRP-23-SAME` | 208/209 | Sprite 2 alone and sprite 3 alone must yield the identical outcome. | `sol:7-video.c4`, `opus:7-vp.c6`, `sol:7-fig7-2.c2`, `opus:7-fig7-2.c3` |
| 3 | `SPGRP-45-SAME` | 208/209 | Sprite 4 alone and sprite 5 alone must yield the identical outcome. | `sol:7-video.c5`, `opus:7-vp.c7`, `sol:7-fig7-2.c3`, `opus:7-fig7-2.c4` |
| 4 | `SPGRP-67-SAME` | 208/209 | Sprite 6 alone and sprite 7 alone must yield the identical outcome. | `sol:7-video.c6`, `opus:7-vp.c8`, `sol:7-fig7-2.c4`, `opus:7-fig7-2.c5` |
| 5 | `SPGRP-B12-DISTINCT` | 209 | Sprite 1 alone and sprite 2 alone must yield **different** outcomes -- the {0,1} group does not extend to include 2. | `opus:7-fig7-2.c2`, `opus:7-fig7-2.c3` |
| 6 | `SPGRP-B34-DISTINCT` | 209 | Sprite 3 alone and sprite 4 alone must yield different outcomes. | `opus:7-fig7-2.c3`, `opus:7-fig7-2.c4` |
| 7 | `SPGRP-B56-DISTINCT` | 209 | Sprite 5 alone and sprite 6 alone must yield different outcomes. | `opus:7-fig7-2.c4`, `opus:7-fig7-2.c5` |
| 8 | `SPGRP-B70-DISTINCT` | 209 | Sprite 7 alone and sprite 0 alone must yield different outcomes (the grouping does not wrap). | `opus:7-fig7-2.c5`, `opus:7-fig7-2.c2` |

The overview sentence (`sol:7-video.c2`, `opus:7-vp.c4`, playfield-priority
half only) is not given a ninth, collective assertion of its own -- the brief
is explicit that a collective "all cases passed" is not a proposition. Its
content is exactly the conjunction of #1-#8: four groups of two (membership,
#1-#4), and no other grouping (exhaustiveness, #5-#8). Both uids are cited
above through the assertions that realise them.

### Built-in control

#1-#4 and #5-#8 sit on the same measurement basis (compare the outcome for
sprite X alone against sprite Y alone) but expect opposite verdicts (equal vs.
unequal). This is itself the control the brief asks for: a harness that always
reports "equal" (e.g. an output stuck at a constant) would pass #1-#4 for the
wrong reason and fail #5-#8 visibly; a harness that always reports "different"
would fail #1-#4 visibly. Only a harness that is actually reading a
group-dependent signal can pass all eight, so the eight together certify each
other rather than needing a separate, unrelated control case.

### Fixture liveness (harness requirement, not a manual claim -- no uid)

Before any of the above is trusted, the test proves the fixture itself is
alive: drive a "no sprite requests priority" baseline and sprite 0 alone, and
assert the two produce different, known outcomes. This has no printed page --
it is the "fixture proves it can deliver a known word before anything reads
one" requirement from `BUILD-BRIEF.md`, guarding against a data path that is
silently dead (as happened with chapter 6's first salvaged harness).

## Descoped (recorded, not asserted)

- The **collision** half of "for playfield priority and collision purposes
  only, sprites are treated as four groups of two" (`sol:7-video.c2`,
  `opus:7-vp.c4`) -- both records' own `split` field routes this to a
  collision-boundary package (`denise_collision`), not `denise_spritepriority`.
  Recorded with that reason, not asserted here.
- **Within-pair order** (whether sprite 0 or sprite 1 wins inside the {0,1}
  group, etc.) -- per the task sheet, not observable at this boundary; the
  same 8 claims are silent on order and only ever name the pair as a unit.
  Recorded with that reason, not asserted here; belongs to
  `ch7-inter-sprite-priority` on `denise_sprites`.

## Claims not turned into an assertion

None. All 18 claims are accounted for above: 16 as direct support for
assertions #1-#8, and the remaining 2 (`sol:7-video.c2`, `opus:7-vp.c4`) as
the overview statement whose playfield-priority content assertions #1-#8
jointly realise, and whose collision-purposes content is explicitly descoped
above.

## Halt condition check

This specification did not require any module port name, signal name, or
file/line reference to state testably. No halt is called for.
