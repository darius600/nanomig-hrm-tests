# SPECIFICATION — ch7-inter-sprite-priority

Package: `ch7-inter-sprite-priority`
Fixture: `denise_sprites`
Claims file: `ch7-inter-sprite-priority.jsonl` (23 claims)

This specification is written from the quotations in the claims file alone —
the marker-line text quotes on printed pages 207 and 209, and the two figures
they point at, `ch7/figures/p208.png` (Figure 7-1) and `ch7/figures/p209.png`
(Figure 7-2), both opened and read as drawings. No module port name is used
below. Every proposition is stated in manual
language: sprite numbers, "in front of" / "displayed over", "armed" (made
active), never a signal name.

## What the two figures show, read directly

**Figure 7-1** (p.208, caption "Inter-Sprite Fixed Priorities"): eight square
boxes numbered 0 to 7 in a diagonal cascade, box 0 at the bottom left, box 7
at the top right. Box 0 is drawn whole — a complete, uninterrupted outline.
Each subsequent box (1..7) has its lower-left corner and part of its left and
bottom edge missing exactly where a lower-numbered box overlaps it, while its
own top edge and the rest of its outline are intact. Reading the cascade: for
every adjacent pair (n, n+1), box n is drawn in front of box n+1.

Directly under the figure, the page continues under the heading "HOW SPRITES
ARE GROUPED": "For playfield priority and collision purposes only, sprites
are treated as four groups of two sprites each. The groups of sprites are:
Sprites 0 and 1 / Sprites 2 and 3 / Sprites 4 and 5 / Sprites 6 and 7." This
paragraph defines group *membership*, not group *order*, and group membership
belongs to the sibling package `ch7-sprite-priority-groups` (fixture
`denise_spritepriority`), not to this one.

**Figure 7-2** (p.209, caption "Analogy for Video Priority"): a hand-of-fingers
drawing. The four "Sprite Groups" fingers, from the axis's "In front (higher
priority)" end down to "Behind", read SP01, SP23, SP45, SP67 in that order.
The body text above it states: "Just as you cannot change the sequence of the
four fingers on the one hand, neither can you change the relative priority of
the sprites" — an explicit invariance claim, not merely a drawing.

## Chipset scope

Chapter 7's body is the OCS account; the chapter's closing sentence on
printed p.224 sends ECS system-control register material to Appendix C, and
the claims file records that Appendix C (p.307) adds BPLCON2 fields but none
that bear on sprite-to-sprite priority. Nothing below is a claim about ECS or
AGA behaviour — only about what the OCS-documented chapter states and draws.

## Assertions

Each assertion below is expected to **PASS**. None of them is under
suspicion; as control cases measured on the same basis as any case under
suspicion, `PRI-01` (the simplest, first-drawn adjacency) and `SPR-0` (the
simplest single-sprite liveness check) are the nominated controls — every
other assertion in this package is measured exactly the same way.

Exact stimulus mechanics (how a sprite is made active, what carries its
pixel code) are not stated here — they depend on the module's actual ports
and are in the test source. What this document fixes is: the proposition,
its expected result (PASS), its page, and the claims it is cited from.

### Fixture liveness / sprite identity — `SPR-0` .. `SPR-7`

Proposition (one per sprite N, N = 0..7): sprite N, made active on its own
with a distinct, known pixel code and no other sprite active, is the sprite
whose code is delivered by the fixture. Taken together these eight checks
establish (a) that the fixture actually delivers a distinguishable, known
word for each of the eight sprites before any ordering claim is tested on
top of it — the data-path liveness check — and (b) the
hardware half of the claim that there are eight sprites, numbered 0 to 7.

- Page: 208. Claim: `opus:7-fig7-1.c1` (hardware half only — see "Declined
  content" below for the half of this claim that is not asserted).
- Expected: PASS, all eight.
- Mutation (per sprite N): leave sprite N inactive instead of arming it; the
  code the assertion expects is then simply not produced, so the check must
  go red; then arm it again to revert.
- Graded condition: liveness (some `hpos` sample went non-zero) plus code
  fidelity (its low 2 bits match the 2'b01 pixel code that was written) —
  not the full packed `sprdata` byte (`denise_sprites.v`'s own
  `{esprm/osprm, group-id, pixel}` packing) and not an exact one-sample
  pulse count, since p.208 requires neither. Both are measured and printed,
  not graded, each with its own `record()` and `reason_kind source-silent`.

### Pairwise fixed priority — `PRI-01` .. `PRI-67`, `PRI-02` .. `PRI-57`

Proposition, ADJACENT pairs (one per pair (n, n+1), n = 0..6): when sprite n
and sprite n+1 are both active with distinct, known pixel codes at the same
overlapping position, the code delivered is sprite n's code — the
lower-numbered sprite is displayed in front. Sprite n is armed before sprite
n+1 in this variant (the reversed-order check is `INV-01` / `INV-67` below).

Proposition, ALL OTHER pairs (`PRI-02` .. `PRI-57`): the same proposition,
for the 21 remaining pairs (n, m), n < m, m > n+1, of the 28 total
overlapping pairs across eight sprites — p.207's "the lower-numbered sprites
appearing in front of ... the higher-numbered sprites" names no adjacency
restriction, so the claim is graded on every pair, not only the seven the
drawing's occlusion directly illustrates: a design where sprite 0 lost to
sprite 2 must fail.

| id | pair | page(s) | claims |
|---|---|---|---|
| PRI-01 | (0,1) | 208 | `sol:7-fig7-1.c1`, `opus:7-fig7-1.c2` (priority half), `opus:7-vp.c2` (whole-cascade drawing, general support) |
| PRI-12 | (1,2) | 208, 209 | `sol:7-fig7-1.c2`, `opus:7-fig7-1.c3` (priority half), `opus:7-fig7-2.c6` (SP01 over SP23 crosses this boundary), `opus:7-vp.c2` |
| PRI-23 | (2,3) | 208 | `sol:7-fig7-1.c3`, `opus:7-fig7-1.c4` (priority half), `opus:7-vp.c2` |
| PRI-34 | (3,4) | 208, 209 | `sol:7-fig7-1.c4`, `opus:7-fig7-1.c5` (priority half), `opus:7-fig7-2.c7` (SP23 over SP45 crosses this boundary), `opus:7-vp.c2` |
| PRI-45 | (4,5) | 208 | `sol:7-fig7-1.c5`, `opus:7-fig7-1.c6` (priority half), `opus:7-vp.c2` |
| PRI-56 | (5,6) | 208, 209 | `sol:7-fig7-1.c6`, `opus:7-fig7-1.c7` (priority half), `sol:7-fig7-2.c9`, `opus:7-fig7-2.c8` (SP45 over SP67 crosses this boundary), `opus:7-vp.c2` |
| PRI-67 | (6,7) | 208 | `sol:7-fig7-1.c7`, `opus:7-fig7-1.c8` (priority half), `opus:7-vp.c2` |
| PRI-02 | (0,2) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-03 | (0,3) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-04 | (0,4) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-05 | (0,5) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-06 | (0,6) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-07 | (0,7) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-13 | (1,3) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-14 | (1,4) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-15 | (1,5) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-16 | (1,6) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-17 | (1,7) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-24 | (2,4) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-25 | (2,5) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-26 | (2,6) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-27 | (2,7) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-35 | (3,5) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-36 | (3,6) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-37 | (3,7) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-46 | (4,6) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-47 | (4,7) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |
| PRI-57 | (5,7) | 207, 209 | `sol:7-video.c1`, `opus:7-vp.c1`, `opus:7-vp.c9` |

`opus:7-vp.c2` restates the whole eight-box cascade of Figure 7-1 in one
record (page 208 — the CD text file embeds the drawing under a p.207
heading, but the printed figure is on p.208). It is not given its own
assertion: its content is exactly the conjunction of `PRI-01`..`PRI-67`
above, so it is cited on those seven rather than duplicated as an eighth.
It is deliberately NOT cited on `PRI-02`..`PRI-57`: its own recorded
`classify.evidence` in the claims file is written strictly over the seven
adjacent boxes of Figure 7-1's cascade, so citing it on a non-adjacent pair
would over-generalise it. `sol:7-video.c1`/`opus:7-vp.c1`/`opus:7-vp.c9` —
p.207's body sentence and p.209's hand analogy, both stated without an
adjacency qualifier — are cited on all 28 pairs instead (the citation
`INV-01`/`INV-67` use below).

The three group-crossing claims (`opus:7-fig7-2.c6/c7/c8`, `sol:7-fig7-2.c9`)
are about Figure 7-2's group fingers (e.g. "sprites 0,1 in front of sprites
2,3"), not individual sprites. At this fixture the only thing that can be
measured is the single adjacent pair straddling each group boundary — (1,2),
(3,4), (5,6) — so that is what they are attached to; they are not asked to
prove the full four-way group comparison, only the boundary this fixture can
see.

- Expected: PASS, all twenty-eight.
- Mutation (per pair): withhold activation from sprite n (the lower-numbered
  one of the pair), leaving sprite m active on its own; the code the
  assertion expects (sprite n's) is then absent — sprite m's own group wins
  by default in the cascade instead — so the check must go red; then arm
  sprite n again to revert.

### Order does not decide it — `INV-01`, `INV-67`

Proposition: the two general-rule quotations ("You cannot change the
relative priorities of the sprites... the lower-numbered sprites
[appear]... in front of... the higher-numbered sprites", p.207; "Just as you
cannot change the sequence of the four fingers on the one hand, neither can
you change the relative priority of the sprites", p.209) are read as an
invariance claim distinct from the static picture in Figure 7-1: not merely
that sprite n is in front of sprite n+1, but that this is fixed regardless of
which of the two sprites is made active first. `INV-01` repeats `PRI-01`'s
measurement with sprite 1 armed before sprite 0; `INV-67` repeats `PRI-67`'s
measurement with sprite 7 armed before sprite 6. Two representative pairs,
one at each end of the cascade, stand in for testing this on every pair —
the claim is general and does not single out any one adjacency.

- Page: 207, 209 (both). Claims: `sol:7-video.c1`, `opus:7-vp.c1`,
  `opus:7-vp.c9`.
- Expected: PASS, both.
- Mutation: same as `PRI-01` / `PRI-67` above — withhold the lower-numbered
  sprite of the pair and confirm the check goes red, then revert. A
  mutation that instead reversed the *arm order itself* would not produce a
  red result if the claim is true, and so would not be a valid mutation for
  this assertion — it would only demonstrate the same invariance the
  assertion already asserts.
- Graded condition: the same sprite wins, at the same `hpos`, in both arm
  orders — not full 512-sample trace equality between the two runs.
  p.207/209 state an invariance about which sprite wins the overlap, not
  about `denise_sprites.v`'s internal state at every other `hpos` the two
  runs happen to share. Full-trace equality is measured and printed, not
  graded.

## Declined content — not asserted

The following claims, or halves of claims, describe how the Figure 7-1
drawing is inked rather than any behaviour of the chipset. Where the manual
is silent, the rule is measure and report, do not assert; and a drawing's
ink is not a hardware signal, so none of these is graded. They are recorded
with `record()`, not `expect()`.

- `opus:7-fig7-1.c1`, drawing-inventory half ("the drawing... is a cascade of
  exactly eight identical rectangles... no ninth box and no other digit
  appears in the frame"): counts and describes the printed drawing; names no
  signal. (Its hardware half — "there are eight sprites and they are
  numbered 0 to 7" — is asserted as `SPR-0`..`SPR-7` above.)
- `opus:7-fig7-1.c2` through `opus:7-fig7-1.c8`, each one's ink-description
  half ("box N's lower-left corner and part of its left and bottom edges are
  not drawn, so only a strip at its right... remains visible"): describes
  which corners and edges of the rear box are inked; names no signal. Read
  directly from `p208.png`, this description is also inaccurate as a
  reading of the drawing — each rear box keeps its top band and top edge
  visible as well as the right-hand strip, matching the crossread's own
  finding (`figure-description-mismatch`) recorded against every one of
  these seven claims. (Each record's priority half — "sprite N is displayed
  in front of sprite N+1 wherever they overlap" — is asserted as one of
  `PRI-01`..`PRI-67` above.)

Every one of this package's 23 claims has a disposition: each is either
cited on an assertion above (in whole or in the stated half), or declined
here with a reason. A testable specification is possible without the
module's port names.
