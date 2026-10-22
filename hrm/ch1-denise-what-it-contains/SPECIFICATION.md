# SPECIFICATION -- ch1-denise-what-it-contains

Package: `hrm/ch1-denise-what-it-contains`
Fixture: `denise` (top module -- one Verilator build, `denise.v` plus every
module it itself instantiates).
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered
to `classify.package == "ch1-denise-what-it-contains"` -- 37 records.
Subject: chapter 1's "what Denise contains" material -- printed pages 1-4
(the custom-chip introduction, the display-mode/colour-count list, the
sprite and priority/collision bullets) and page 8 (Figure 1-1, the DENISE
block).

## Sources

This file rests on the `extract.quote` / `classify.evidence` /
`classify.observable` content of the 37 claims, whose `classify.evidence`
fields already carry `file:line` citations into `denise.v` and its
submodules (port names, bit assignments, instantiation line numbers), and
on the sibling packages under `hrm/` that grade overlapping propositions.
Port names and bit positions are confirmed against `denise.v` and its
submodules.

## This chapter is an introduction: grade the sentence, not the mechanism

This package's 37 claims are structural sentences.
The assertion is that a thing is inside Denise's boundary, or that a port
carries a stated width -- not that the mechanism behaves correctly. Chapters
3 (playfields) and 4 (sprites) are accepted chapters that already grade the
detailed mechanism behind much of what chapter 1 only names. For every
proposition, the `hrm/` tree is searched for an assertion on the same
printed proposition; where one exists, the claim is declined on a
`record()` line citing that package, rather than re-graded here.

## The 37 claims, grouped into 19 propositions

Two readers (`sol`, `opus`) independently claimed nearly every sentence in
this material; their bare ids diverge by a small constant from
`1-components.c45`/`c46` onward (one reader split an earlier sentence
differently), so pairing is by page + restatement content, not by bare id
equality -- `(reader, id)` is the uid, never the bare id alone. One claim,
`sol:1-blockdiag.c14`
(Figure 1-1, page 8), has no twin in this package.

### Group A -- graded in this package (4 propositions, 9 claims)

**A1. `sol:1-components.c2` + `opus:1-components.c2`** (p.1): "The Amiga does
this through the use of custom silicon hardware that yields advanced
graphics and sound features." This bundles two domains (video, audio) and
this package's fixture is `denise` alone -- it cannot exercise Paula's audio
path. Under the "no claim may lose its home" rule: where only
part of a claim is gradeable at this boundary, the measurement is kept
under an `expect()` label carrying **no uid**, and both uids are cited on a
`record()` line telling the whole story. Graded here: the video half only
(custom-silicon-to-video-output, no CPU in the path). Declined, same
`record()`: the audio half (Paula's domain, outside this single-module
fixture).

**A2. `sol:1-components.c36` + `opus:1-components.c36`** (p.2): "These
three custom chips, named Paula, Agnus, and Denise, each contain the logic
to handle a specific set of tasks." Same boundary problem: proving all
three chips are mutually disjoint needs all three fixtures. What this
single-module fixture CAN show is denise's own half of the partition: its
decode is bounded to its own address table and does not answer addresses
that belong to a sibling chip. Graded (no uid): denise's own bounded
decode. Declined on `record()` with both uids: the full three-way
disjointness claim, which needs Paula's and Agnus's own fixtures too.

**A3. `sol:1-components.c48` + `opus:1-components.c47`** (p.3, NTSC hires:
"640 by 200 non-interlaced or 640 by 400 interlaced ... in 16 colors") and
**`sol:1-components.c50` + `opus:1-components.c49`** (p.3, PAL hires: "640
by 256 non-interlaced or 640 by 512 interlaced ... in 16 colors"): four
uids, two propositions that reduce, at this fixture, to the identical
measurement. `denise` has no PAL/NTSC input at all -- that selection is
Agnus's `ntsc` port into `agnus_beamcounter.v`, which decides line counts
and field height; a `denise`-only fixture cannot distinguish "NTSC hires"
from "PAL hires" and would produce identical evidence for both. What IS
inside this boundary and directly observable is the mode-selecting
mechanism itself: BPLCON0's HIRES bit (denise.v:182, bit 15) and SHRES bit
(denise.v:189, bit 6) drive the `hires`/`shres` output ports directly and
combinationally, selecting among the resolution *modes* the four bullets
describe. Graded (no uid, one shared assertion for both propositions): the
HIRES/SHRES mode-selection mechanism exists and is directly
register-controlled. Declined on `record()` with all 4 uids: the specific
pixel counts (640x200 vs 640x256 etc.), the interlace line-doubling, and
the PAL-vs-NTSC selection itself -- all Agnus's beam-counter, outside this
fixture.

**A4. `sol:1-blockdiag.c14`** (p.8, Figure 1-1): the DENISE block sits under
PAULA inside the dashed CUSTOM CHIP SECTION; its only incoming arrowhead in
the drawing is from the REG ADR BUS; no separate chip-data arrow enters it.
Testable directly at this boundary and not a restatement of A1/A2's page-1
prose (a different oracle -- the image, not text; its own page, its own
uid): write a register through `reg_address_in`/`data_in` (the one
documented input path the figure shows) and confirm it reaches
`red`/`green`/`blue` (or the `hires`/`shres` ports the write also drives),
with no other input class needed. Graded, full uid, own assertion.

### Group B -- declined, citing existing coverage (15 propositions, 28 claims)

**B1. `sol:1-components.c46` + `opus:1-components.c45`** (p.3): "Bitplane
generated, high resolution graphics capable of supporting both PAL and
NTSC video standards." PAL/NTSC: Agnus's `ntsc` input and
`agnus_beamcounter.v`'s separate line-count constants (312 PAL / 262 NTSC)
-- not a `denise` port, outside this fixture, same reasoning as A3.
"Bitplane generated": `denise_bitplanes.v` is instantiated inside `denise`
(denise.v:379) and its containment is already the citation basis for A3's
own HIRES/SHRES measurement (the same bitplane-fed pipeline). Actually
measuring bitplane data reaching a *visible* pixel needs the phase-accurate
window/hpos state machine `hrm/ch3-bplcon0-display-modes/` and
`hrm/ch3-horizontal-display-window/` already built and already exercise at
this exact `denise` fixture (their own headers disclose the c1/c3-overlap
clock trap and the hpos-mirroring bookkeeping this would otherwise have to
reinvent). Building a second copy of that machinery here, for a chapter-1
restatement, would inflate a structural sentence into a mechanism test.
Declined, citing both
packages for ownership of the bitplane-to-pixel mechanism, and the Agnus
beam counter (outside any `denise`-only fixture) for the PAL/NTSC half.

**B2. `sol:1-components.c52` + `opus:1-components.c51`** (p.3): "Hold-and-
modify (HAM) mode allows for the display of up to 4,096 colors on screen
simultaneously." `denise_hamgenerator` is instantiated inside `denise`
(denise.v:506) -- containment is not in question. But actually measuring
"4,096 simultaneously" needs HAM mode enabled, and `hrm/ch3-ham-control-
field/`'s own header records that the manual **disagrees with itself** about which BPLCON0 bit enables HAM (p.95
says bit 10; pp.87/90/271 say bit 11) and explicitly declines to resolve
that dispute or pick a side, at the narrower `denise_hamgenerator` fixture
where the question is at least directly askable. Driving HAM mode through
this package's own wider `denise` fixture inherits the identical
unresolved dispute one level further removed, with no additional
information to resolve it. Declined, citing `hrm/ch3-ham-control-field/`'s
own unresolved-dispute finding as the reason a "4,096 simultaneously"
measurement is not honestly reachable at any current boundary, not just
this one.

**B3. `sol:1-components.c53` + `opus:1-components.c52`** (p.3, overscan:
"higher resolution displays specially suited for video and film") and
**B4. `sol:1-components.c54` + `opus:1-components.c53`** (p.3, "Displays of
arbitrary size, larger than the visible viewing area"): both bullets
describe the same mechanism -- `denise`'s own DIWSTRT/DIWSTOP window
registers (denise.v:57-58, write path denise.v:303-329) accepting values
outside the nominal edges, plus (for B4 specifically, per its own
`classify.evidence`) Agnus's vertical window pair, outside this fixture
regardless. DIWSTRT/DIWSTOP are write-only (no readback via `data_out`,
confirmed by `hrm/ch3-nominal-display-values/`'s own finding); the only way
to observe an out-of-nominal window's *effect* is through `blank`/
`red`/`green`/`blue` gated by the hpos-vs-window comparator, which needs
the same phase-accurate state machine as B1. `hrm/ch3-horizontal-display-
window/` (174 claims) already owns this register's mechanics in full at
this exact `denise` fixture. Declined, citing that package's ownership of
the window-comparator mechanism for the horizontal half, and Agnus's
vertical window (outside every `denise`-only fixture, including
`ch3-horizontal-display-window`'s own) for B4's vertical half.

**B5. `sol:1-components.c55` + `opus:1-components.c54`** (p.3): "Amigas
which contain the Enhanced Chip Set (ECS) support Productivity mode giving
displays of 640 by 480, non-interlaced with 4 colors from a pallette of
64." The 640x480 geometry is Agnus's beam counter
(`agnus_beamcounter.v:206/212/223/229`, `varbeamen_out`), outside this
fixture, same reasoning as A3/B1. The colour half ("4 colors from a
palette of 64") is genuinely ambiguous at this remove: it may be EHB
(extra-half-brite, `denise_colortable`'s own select[5] mechanism, already
measured by `hrm/ch3-colour-register-format/`'s `EHB-HALVES-INTENSITY`) or
a distinct ECS-specific wide-select path; the page does not say which, and
this package does not settle it from the RTL. Declined in full: geometry
cited to the Agnus boundary, colour
mechanism cited to `hrm/ch3-colour-register-format/`'s `EHB-HALVES-
INTENSITY` as the closest already-measured "beyond 32 base colours" fact
at the shared `denise_colortable` mechanism, with the ambiguity disclosed
rather than resolved by assumption.

**B6. `sol:1-components.c60` + `opus:1-components.c59`** (p.3): "32 system
color registers, each of which contains a 12-bit number as four bits of
red, four bits of green, and four bits of blue." Already measured, on this
exact mechanism (`denise_colortable`), by `hrm/ch3-colour-register-format/`:
`ADDR-32-DISTINCT` (32 distinct 12-bit codes on the 32 registers read back
as 32 pairwise-distinct values; cites, among others, 3-colortable.c1-c6,
3-t2.c01-c05, A-color.c3/c5/c6) and `RED-FIELD-BITS-11-8` /
`GREEN-FIELD-BITS-7-4` / `BLUE-FIELD-BITS-3-0` (each nibble moves only its
own channel byte; cites 3-colortable.c14-c16, 3-t3.c02-c04, A-color.c11-
c22). Declined, citing that package and those four assertions.

**B7. `sol:1-components.c61` + `opus:1-components.c60`** (p.3): "This
allows a system color palette of 4,096 different choices of color for each
register." Already measured by `hrm/ch3-colour-register-format/`'s
`SWEEP-12BIT-DISTINCT` (9 distinct 12-bit codes on one register read back
pairwise distinct; cites 3-bpl.c13, 3-color.c14, 3-colortable.c11/c12,
3-ct.c07/c08, A-color.c4, A-colorxx.c02) -- the identical register, the
identical numeric fact, on a different page (p.46 there, p.3 here) of the
same manual describing the same hardware. Declined, citing that package
and assertion.

**B8. `sol:1-components.c62` + `opus:1-components.c61`** (p.3): "Eight
reusable 16-bit wide sprites with up to 15 color choices per sprite pixel
(when sprites are paired)." Three components, all already measured
elsewhere at the `denise_sprites`/`denise_sprites_shifter` fixtures: eight
sprites exist (`hrm/ch3-sprite-registers/`'s own "8 sprites total" register
map; `hrm/ch7-inter-sprite-priority/`'s own "there are eight sprites and
they are numbered 0 to 7", grounded across 8 assertions); 16-bit width
(`hrm/ch4-sprite-shifter/`'s `SPRITE-RUN-IS-EXACTLY-16-PIXELS`, citing
CH4-size.c1 p.101 "Sprites are 16 pixels wide"); 15 colours when paired
(`hrm/ch4-sprite-colour-registers/`, citing CH4-attach.c1/c2 p.120 "you can
create sprites that have fifteen possible color choices (plus transparent)
... by attaching two sprites"). "Reusable" carries no separate testable
threshold (an adjective, like p.1's "advanced" in A1, not a port or a
number). Declined in full, citing all three packages.

**B9. `sol:1-components.c63` + `opus:1-components.c62`** (p.3): "A sprite
is an easily movable graphics object whose display is entirely independent
of the background (called a playfield); sprites can be displayed over or
under this background." The over/under half is BPLCON2 playfield-sprite
priority, already measured by `hrm/ch3-sprite-playfield-priority/`'s
`BPLCON2-BITS-0-5-ARE-LIVE-PRIORITY-FIELD` (sweeps bplcon2[5:0] through all
64 values, shows `sprsel` takes both possible values) -- the same
assertion `hrm/ch4-sprite-priority/` itself already cites when declining
its OWN page-103 restatement of this identical proposition
(`CH4-p103.c22`, "You can vary the priority between sprites and
playfields"), rather than building a second copy. "Movable, independent of
the playfield": sprite position comes from the sprite's own SPRxPOS
register and the beam position (`denise_sprites_shifter.v`'s own `hpos`
port), not from bitplane data -- structural containment already
established by this claim's own `classify.evidence` and by
`hrm/ch4-sprite-horizontal-position/`'s and `hrm/ch4-sprite-vertical-
position/`'s own register-independence measurements at the
`denise_sprites`/`agnus_spritedma` fixtures. Declined in full, citing all
of the above.

**B10. `sol:1-components.c64` + `opus:1-components.c63`** (p.3-4: "A
sprite is 16 low resolution pixels wide and an arbitrary number of lines
tall" -- the sentence crosses the printed page break, both readers'
`crossread` note this and it is not a defect). Width: already measured,
`hrm/ch4-sprite-shifter/`'s `SPRITE-RUN-IS-EXACTLY-16-PIXELS` (same
citation as B8). Height ("an arbitrary number of lines"): NOT a duplicate
decline -- `hrm/ch4-sprite-shifter/` itself explicitly declined this exact
half of the same sentence, on its OWN `record()`
(`VERTICAL-EXTENT-NOT-AT-THIS-BOUNDARY`, citing CH4-p101.c2/c3,
CH4-size.c2), because sprite height is set entirely by
`agnus_spritedma.v`'s own DMA-fetched position/control words -- not
visible at `denise_sprites_shifter`, and not visible anywhere in `denise`
either (confirmed: no vertical line-count port exists on `denise`'s own
top-level interface). No package in `hrm/` measures "arbitrary number of
lines" as its own proposition; it remains ungraded across the suite, for a
genuine boundary reason. Declined here for the same boundary reason
(Agnus's DMA engine, outside `denise` entirely), citing
`hrm/ch4-sprite-shifter/`'s own finding of the identical gap rather than
claiming false coverage.

**B11. `sol:1-components.c67` + `opus:1-components.c66`** (p.4:
"Dynamically controllable inter-object priority, with collision
detection.") A two-part bullet heading; both parts are covered by B12/B13
below and by `hrm/ch4-sprite-priority/`'s general priority statements
(`SPRITE-PRIORITY-FIXED-BY-GROUP-NUMBER`, `SPRITE-DATA-REACHES-PRIORITY-
CIRCUIT`, citing CH4-p103.c20, CH4-prio.c1/c3, CH4-hw.c14, CH4-p124.c10,
CH4-fig4-13.c6) and `hrm/ch4-sprite-collision/`'s page-113 collision
statements (CH4-coll.c1/c2/c3). Declined, citing all four.

**B12. `sol:1-components.c68` + `opus:1-components.c67`** (p.4: "This
means that the system can dynamically control the video priority between
the sprite objects and the bitplane backgrounds (playfields).") The same
proposition as B9's over/under half and as `hrm/ch4-sprite-priority/`'s
own declined `CH4-p103.c22`. Declined, citing `hrm/ch3-sprite-playfield-
priority/`'s `BPLCON2-BITS-0-5-ARE-LIVE-PRIORITY-FIELD` directly, per that
same established precedent.

**B13. `sol:1-components.c69` + `opus:1-components.c68`** (p.4: "You can
control which object or objects appear over or under the background at any
time.") Same mechanism and same citation as B12.

**B14. `sol:1-components.c70` + `opus:1-components.c69`** (p.4:
"Additionally, you can use system hardware to detect collisions between
objects and have your program react to such collisions.") Already measured
almost verbatim by `hrm/ch4-sprite-collision/`'s three page-113 assertions
(CH4-coll.c1: sprites collide with each other; CH4-coll.c2: sprites
collide with either playfield; CH4-coll.c3, `COLLISION-DETECTABLE-VIA-
REGISTER-READ`: collision is readable via CLXDAT) and by `hrm/ch3-
collision-registers/`'s 98-claim CLXCON/CLXDAT inventory. "Have your
program react" is software, not hardware, and is not graded by either
sibling package either (their own non-goals say so explicitly). Declined
in full, citing both packages.

## What this leaves to build

Four assertions carrying claim-bearing evidence (A1-A4 above, 9 of the 37
claims, on 4 `expect()` labels -- three of them shared across more than one
proposition per the "no claim may lose its home" partial-grading rule) plus
one infrastructure liveness control. Fourteen propositions (28 claims) are
declined on `record()` lines, each citing the specific existing package and
assertion (with uids where available) that already grades the same
proposition, or the specific boundary reason (Agnus's beam counter / DMA
engine, outside every `denise`-only fixture) that makes the claim
ungradeable here without a wider fixture outside this package's scope.

## Mutation plan

Every `expect()`-graded assertion (the four claim-bearing ones and the
`[control]` liveness check: the mutation gate reads the id after a
bracketed tag, so a control is an ordinary assertion to it) gets one
stimulus mutation, replayable as `make MUT=MUT_n test`, confirmed red in
isolation and reverted, under the suite's `MUT_n` / `-D` convention.
