# SPECIFICATION — ch7-playfield-priority-codes

Package: `ch7-playfield-priority-codes`
Fixture: `denise_spritepriority`
Claims file: `ch7-playfield-priority-codes-spec-stage.jsonl` (46 claims)

This specification is written from the quotations in the claims file alone —
the marker-line text quotes on printed pages 207, 209, 210 and 211, and the
two figures they point at, `ch7/figures/p209.png` (Figure 7-2, "Analogy for
Video Priority") and `ch7/figures/p212.png` (Figure 7-3, "Sprite/Playfield
Priority"), both opened and read as drawings. No RTL has been opened and no
module port name is used below; every proposition is stated in the manual's
own vocabulary — a priority code's binary/decimal value, a sprite group's
name, "in front of" / "behind" / "higher priority" — never a signal name.

**Table 7-2 is a truth table and this specification sweeps it exhaustively.**
For each of the two playfields there are 5 defined code values (0–4) and 4
sprite groups to compare against, so each playfield gets a full 5×4 grid of
20 independent front/behind facts (40 in total across PF1 and PF2), not a
sample of adjacent-only or single-code cases. Nothing about the monotonic
"insertion point" shape the manual's prose suggests is assumed or relied on
to skip a cell — a row that violates that shape is exactly the kind of
implementation defect this suite exists to catch, and it is only caught if
every cell is actually measured.

## Vocabulary fixed by the manual

- **"In front of" / "higher priority" / "displayed over"** and **"behind" /
  "lower priority"**: Figure 7-2's vertical axis is labelled, top to bottom,
  "In front (higher priority)" ... "Behind" (p.209), and page 209's prose
  uses "on top of" and "beneath" for the same relation. Below, "front" and
  "behind" are used as short predicate names for exactly this relation:
  object A "front" of object B means A is what is displayed where both would
  otherwise occupy the same point.
- **Sprite groups SP01, SP23, SP45, SP67**: taken as given, named entities,
  exactly as Table 7-2's own column headers use them (`sol:7-video.c23`
  through `c27`, quoting e.g. "000 PF1 SP01 SP23 SP45 SP67"). Which
  individual sprites belong to which group is a claim of the sibling package
  `ch7-sprite-priority-groups`, not of this one; this package only needs the
  four group labels as Table 7-2 already gives them.
- **PF1P2-PF1P0 / PF2P2-PF2P0**: the two 3-bit priority codes named on pages
  210–211 (register `BPLCON2` per the claims' own `register` field). Binary
  and decimal notation are both used in the source (Table 7-2 uses binary
  "000".."100"; the p.209 prose uses decimal "(0)".."(4)"); this
  specification gives both at each row.

## What the two figures show, read directly

**Figure 7-2** (p.209, "Analogy for Video Priority"): a two-hand drawing. The
right hand ("Sprite Groups") shows four fingers labelled, top to bottom,
SP01, SP23, SP45, SP67. The left hand ("Playfields") shows two fingers,
labelled PF1 and PF2, interleaved between the right hand's fingers so the
combined top-to-bottom order reads SP01, PF1, SP23, PF2, SP45, SP67. A
vertical arrow at the top is labelled "In front (higher priority)"; one at
the bottom is labelled "Behind". Read as an invariant this would say PF1 is
fixed between SP01 and SP23 and PF2 fixed between SP23 and SP45 — but the
text immediately below the figure gives five selectable positions for each
playfield finger, so the drawing shows **one** selectable arrangement, not a
fixed rule (this is the reading the claims file's own crossread notes settle
on for every Figure 7-2 record: `illustrated-configuration-not-invariant`).
The arrangement drawn corresponds to PF1P = 001 and PF2P = 010, both inside
the 5×4 grids below, and is cited there rather than asserted separately.

**Figure 7-3** (p.212, "Sprite/Playfield Priority"): three small reference
panels — "Playfield 1" (a black frame with a white rectangular window cut
into it), "Playfield 2" (a white frame with a grey window in the same
position), "Sprite 0" (a spinning-top shape) — followed by one larger
composite panel with a dashed overlay and an in-frame caption block: "When
everything is displayed together, sprite 0 is more important than playfield
2 but less important than playfield 1. So even though you can't see the
boundary, the sprite disappears \"behind\" the invisible PF1 boundary." In
the composite, sprite 0's shape is drawn whole and visible over playfield
2's grey region, but is cut off flat along a vertical line coincident with
playfield 1's window edge — the half of the sprite outside that window is
simply absent. This is the same worked configuration as the page-211 prose
(PF1P = 000, PF2P = 010): its two priority relations (sprite 0, i.e. group
SP01, vs. PF1; and vs. PF2) are cells already in the grids below and are
cited there. The figure's second half — that the PF1 boundary itself
produces no visible edge in the composite — has no colour or RGB observable
at this fixture and is declined below, not asserted.

## Chipset scope

Chapter 7 names no chipset explicitly. Printed p.224 closes the chapter with
"For information on the system control registers in the Enhanced Chip Set
(ECS), see Appendix C" — so the chapter body, including Table 7-2 and both
figures, is the original (OCS) account, and nothing below is a claim about
ECS or AGA behaviour.

## How PF2's grid is derived

Table 7-2 (p.210) is printed once, headed for PF1P2-PF1P0 only. Page 211
states: "Bits PF2P2-PF2P0 let you position playfield 2 among the sprite
priorities in exactly the same way" (`sol:7-video.c28`, `opus:7-vp.c30`), and
page 209 already states "You have the same possibilities for playfield 2"
(`sol:7-video.c13`). PF2's grid below is therefore PF1's, with PF2 written in
place of PF1 at every code value — not a second printed table, but the same
encoding rule applied to the second playfield, exactly as the source states
it. Every PF2 cell is graded independently in the harness regardless of this
derivation: if the RTL's PF2 encoding does not actually mirror PF1's, the
individual cell assertions catch that directly, so no separate "mirroring
holds" assertion is needed on top of the 20 PF2 cells themselves.

## Assertions

Every assertion below is expected to **PASS** — nothing in this package's 46
claims describes rows 5–7 or a boundary condition under specific suspicion.
Per BUILD-BRIEF's requirement for a control case measured on the same basis
as anything that might turn out red, two are nominated: `LIVE-PF1` (simplest
liveness check) and `PF1P-000-SP01` (simplest behaviour cell — it is also
exactly the page-211 worked example's own case). Every other assertion here
is measured exactly the same way as these two.

Exact stimulus mechanics — how an object is made "active" at this fixture,
what carries its identifying code, whether the two 3-bit priority fields are
literal ports or packed into a wider bus — are not decided here; they depend
on the module's actual ports, opened only in step 3. What is fixed now, and
must not change once the RTL is open, is: the proposition, its expected
result, its page(s), and the claims it is cited from.

### Fixture liveness / object identity — `LIVE-PF1`, `LIVE-PF2`, `LIVE-SP01`, `LIVE-SP23`, `LIVE-SP45`, `LIVE-SP67`

Per BUILD-BRIEF, before any priority relation is meaningful the fixture must
first be shown able to deliver a known, distinguishable result for each of
the six contending objects on its own. This fixture's only observable, per
this package's own claims notes (the `absent_signal` field recorded against
`sol:7-video.c37`, `sol:7-fig7-3.c3` and `opus:7-fig7-3.c4`: "denise_
spritepriority's only output is the 1-bit sprsel"), is a single selection
result rather than a data word — so the liveness precondition is realized
here as: with exactly one object active and given a distinct, known
identifying stimulus, and every other object held inactive, the fixture's
selection identifies that one object. This is required by BUILD-BRIEF's
methodology, not itself a proposition drawn from a page quote; where a claim
does support the object's existence, it is cited alongside.

- Proposition (one per object O in {PF1, PF2, SP01, SP23, SP45, SP67}): O
  made active alone, with every other object inactive, is the object the
  fixture's selection identifies, regardless of either priority code's
  value.
- Page/claims: `opus:7-fig7-2.c9` (two playfields, PF1 and PF2, both
  established); Table 7-2's own column headers naming SP01/SP23/SP45/SP67 as
  the four group entities (`sol:7-video.c23`–`c27`, `opus:7-vp.c24`–`c28`).
- Expected: PASS, all six.
- Mutation (per object O): leave O inactive instead of activating it (every
  other object stays as in the base case, i.e. also inactive here); the
  identity the assertion expects is then not produced by anything, so the
  check must go red; then activate O again to revert.

### PF1 exhaustive sweep — `PF1P-<code>-<group>` (20 cells)

Proposition (one per (PF1P code, sprite group) pair, 5 codes × 4 groups):
with PF1P2-PF1P0 set to the given code and playfield 1 and the named group
both active and contending, the fixture's selection front/behind relation is
as given.

| PF1P (bin/dec) | vs SP01 | vs SP23 | vs SP45 | vs SP67 | primary claims |
|---|---|---|---|---|---|
| 000 / 0 | **front** | front | front | front | `sol:7-video.c8`, `c23`; `opus:7-vp.c24` |
| 001 / 1 | behind | **front** | front | front | `sol:7-video.c9`, `c24`; `opus:7-vp.c25` |
| 010 / 2 | behind | behind | **front** | front | `sol:7-video.c10`, `c25`; `opus:7-vp.c26` |
| 011 / 3 | behind | behind | behind | **front** | `sol:7-video.c11`, `c26`; `opus:7-vp.c27` |
| 100 / 4 | behind | behind | behind | behind | `sol:7-video.c12`, `c27`; `opus:7-vp.c28` |

Cell values in **bold** are the newly-front group at each row, shown only to
make the step-function shape legible; every cell in the row, bold or not, is
graded as its own assertion — `PF1P-000-SP01` through `PF1P-100-SP67`, 20 in
all.

Additional citations on specific cells, all consistent with the table above
(the worked example and both figures each pin one or two cells, not a new
value):

- `PF1P-000-SP01` (PF1 front of SP01 at code 0): also cited by the p.211
  worked example `sol:7-video.c32` ("playfield 1 is in front of sprite 0 or
  1") and its bundled twin `opus:7-vp.c34`/`opus:7-vp.c36`/`opus:7-vp.c37`;
  by Figure 7-3's callout `sol:7-video.c35`/`c36`/`c37` and its composite-
  drawing twins `sol:7-fig7-3.c2`/`c3`, `opus:7-fig7-3.c2` (testable half)
  and `opus:7-fig7-3.c4` (testable half). This is the most heavily attested
  single cell in the package — it is the worked example's own case.
- `PF1P-001-SP01` and `PF1P-001-SP23` (PF1 behind SP01, front of SP23 at
  code 1): also cited by Figure 7-2's illustrated arrangement
  `sol:7-fig7-2.c5`, `sol:7-fig7-2.c6`, and its twin existential-phrased
  record `opus:7-fig7-2.c10`.

Rows 010, 011 and 100 carry no additional figure or worked-example citation
beyond the table row itself and its p.209 descriptive-prose twin.

### PF2 exhaustive sweep — `PF2P-<code>-<group>` (20 cells)

Proposition (one per (PF2P code, sprite group) pair, 5 codes × 4 groups):
with PF2P2-PF2P0 set to the given code and playfield 2 and the named group
both active and contending, the fixture's selection front/behind relation is
as given. Values are PF1's grid with PF2 substituted, per "How PF2's grid is
derived" above; there is no second printed table to quote row-by-row, so the
derivation itself, not a table quote, is each row's primary basis except
where a worked example or figure independently pins the same cell.

| PF2P (bin/dec) | vs SP01 | vs SP23 | vs SP45 | vs SP67 | basis |
|---|---|---|---|---|---|
| 000 / 0 | **front** | front | front | front | derived (`sol:7-video.c13`, `c28`; `opus:7-vp.c30`) |
| 001 / 1 | behind | **front** | front | front | derived (same basis) |
| 010 / 2 | behind | behind | **front** | front | derived + worked example + figures (below) |
| 011 / 3 | behind | behind | behind | **front** | derived (same basis) |
| 100 / 4 | behind | behind | behind | behind | derived (same basis) |

Additional citations on specific cells:

- `PF2P-010-SP01` and `PF2P-010-SP23` (PF2 behind SP01 and behind SP23 at
  code 2 — "sprites 0 through 3 are in front of playfield 2"): cited by the
  p.211 worked example `sol:7-video.c33` and its bundled twin
  `opus:7-vp.c34`/`opus:7-vp.c36`/`opus:7-vp.c37`; by Figure 7-3's callout
  `sol:7-video.c35` and composite-drawing twins `sol:7-fig7-3.c1`,
  `opus:7-fig7-3.c1` (the `PF2P-010-SP01` half only — sprite 0 is in group
  SP01).
- `PF2P-010-SP23` and `PF2P-010-SP45` (PF2 behind SP23, front of SP45 at
  code 2): also cited by Figure 7-2's illustrated arrangement
  `sol:7-fig7-2.c7`, `sol:7-fig7-2.c8`, and twin `opus:7-fig7-2.c11`.

Rows 000, 001, 011 and 100 carry no worked-example or figure citation; their
basis is the derivation alone, which is exactly why each of their 16 cells
is graded independently rather than assumed.

### Mutation rule for both sweeps

One rule covers all 40 cells, realized per-cell in the harness: for cell
(playfield, code C, group G), the mutation changes the priority code from C
to the other defined code (0–4) that this same grid states flips the
front/behind relation for group G specifically, while leaving both objects'
"active" stimulus and the assertion's hard-coded expected relation
unchanged. Concretely, for `PF1P-000-SP01` (expects **front**): re-drive the
code to 001 (row 001 states PF1 is **behind** SP01), confirm the still-
"expects front" assertion goes red, then restore the code to 000. The same
substitution — pick any other row that the table already says disagrees on
that column — applies uniformly to every one of the 40 cells; it is a
genuine stimulus change (what the module is asked to place PF1/PF2 at), not
a flipped expectation.

### Measured, not graded — undefined codes 5–7

Page 209: "The numbers 0 through 4 shown in parentheses ... are the actual
values you use to select the playfield priority positions" (`sol:7-video.
c14`, twin `opus:7-vp.c12`). Both codes are 3 bits wide (0–7) but only 0–4
are given a meaning by the manual; codes 5, 6 and 7 are not ruled on for
either playfield. Per BUILD-BRIEF ("where the manual is silent, measure and
report — do not assert"), these are `note()`s, not `expect()`s:

- `MEASURE-PF1P-101`, `MEASURE-PF1P-110`, `MEASURE-PF1P-111`: with PF1P set
  to 101/110/111 respectively (SP01/SP23/SP45/SP67 all active), record which
  front/behind profile the fixture actually produces. No pass/fail — there
  is nothing in the manual to grade it against.
- `MEASURE-PF2P-101`, `MEASURE-PF2P-110`, `MEASURE-PF2P-111`: the same,
  substituting PF2 for PF1.

A crossread note against `opus:7-vp.c12` speculates the profile at 5–7 will
be "identical to value 4" — that speculation is not in the manual and is not
what is measured; the note records whatever the fixture actually does,
including if it disagrees with that guess.

## Out of scope for this package

The worked example's crossread note reconstructs the *full* five-way merged
order (PF1, SP01, SP23, PF2, SP45, SP67) from three register fields —
PF1P2-PF1P0, PF2P2-PF2P0, and a third bit the note calls PF2PRI that decides
playfield 1's priority relative to playfield 2 itself. No claim in this
package's 46 states or restates a proposition about PF2PRI or about PF1-
versus-PF2 relative priority — every claim here is PF1-vs-sprite-groups or
PF2-vs-sprite-groups only, which is exactly what Table 7-2 and its p.211
mirror state. The two worked-example claims used above (`sol:7-video.c32`,
`c33`) each need only one playfield's code, not the merge, and are asserted
that way. Relative PF1/PF2 priority is therefore not asserted, mutated, or
even stimulated as a controlled variable here; it belongs to whichever
package's inventory actually carries a claim about it, not this one.

## Declined content — not asserted

- The **sprite-group-to-sprite-group** half of every Table 7-2 row
  (`sol:7-video.c23`–`c27`'s and `opus:7-vp.c24`–`c28`'s own `split` field:
  "the three sprite-group-to-sprite-group relations of row NNN" — e.g. SP01
  vs SP23 with neither playfield in contention): observable at `denise_
  sprites`, not at this fixture, per the claims' own classification. Not
  asserted here.
- The **visibility / colour** half of the Figure 7-3 boundary claims
  (`sol:7-video.c37`'s split, `sol:7-fig7-3.c3`'s split, `opus:7-fig7-3.c4`'s
  split: "that boundary is invisible on screen" / "the two sides of that
  boundary render the same colour"): this fixture's only output is a 1-bit
  selection with no colour or RGB value, per the claims' own `absent_signal`
  note — "needs a wider boundary" is the claims file's own verdict, not a
  gap discovered here. Declined; would need a `denise`/`denise_colortable`
  boundary.
- The **colour-0 mechanism** half of `opus:7-fig7-3.c2` ("the clipping edge
  is the pixel where playfield 1's data stops being colour 0"): the claim's
  own note says this belongs to chapter 3's colour-0 claims and is already
  asserted at `ch3-dual-playfield`'s `CODE-ZERO-TRANSPARENT`, not here.
- `opus:7-vp.c11` (the five-position umbrella) and `opus:7-vp.c21`
  (PF1P2-PF1P0 bits determine Table 7-2 position): general/definitional,
  folded into the PF1 sweep section as supporting citations rather than
  given a separate assertion — their content is exactly the conjunction of
  the 20 PF1 cells.
- `opus:7-vp.c3` (priority is changeable, p.207) and `opus:7-vp.c10` (either
  playfield can be in front of or behind the sprites, p.209): general
  package-level support, cited here rather than asserted on their own — they
  are the premise the whole sweep exists to test, not an additional fact
  beyond it.

No claim in this package's 46 was left with no disposition: each is either a
primary or additional citation on a graded assertion above (`LIVE-*`,
`PF1P-*`, `PF2P-*`), a basis for a measured-not-graded note, folded in as
general support, or declined above with a reason tied to the claims file's
own classification. No HALT condition was met — a testable specification,
with all 40 truth-table cells swept exhaustively rather than sampled, was
possible without any module port name.

## Assertion accounting

- 6 liveness assertions (`LIVE-PF1`, `LIVE-PF2`, `LIVE-SP01`, `LIVE-SP23`,
  `LIVE-SP45`, `LIVE-SP67`)
- 20 PF1 sweep assertions (`PF1P-000-SP01` .. `PF1P-100-SP67`)
- 20 PF2 sweep assertions (`PF2P-000-SP01` .. `PF2P-100-SP67`)
- 46 graded assertions in total, plus 6 measured-not-graded notes
  (`MEASURE-PF1P-101/110/111`, `MEASURE-PF2P-101/110/111`)
- 2 nominated controls: `LIVE-PF1`, `PF1P-000-SP01`
