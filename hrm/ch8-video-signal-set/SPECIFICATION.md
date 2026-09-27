# Specification — `ch8-video-signal-set`

Package: `hrm/ch8-video-signal-set`
Fixture: `minimig`
Claims file: `hrm-testpipeline/work/ch8-s4-build/ch8-video-signal-set.jsonl` (10 claims)
Subject: the analog-RGB output set p.260 names — Red, Green, Blue and Sync —
and whether that four-member set is present at this design's video-output
boundary.

The propositions below are drafted from the `quote` / `restatement` fields
of the 10 claims — they name no port, no line number and no signal width.
The claims file's `classify.evidence` field, on every one of the 10
records, names concrete RTL port names and line numbers
(`minimig.v:239-252`, `agnus_beamcounter.v:421`, `agnus.v:75`,
`minimig.v:459-461`) and states that the design emits three sync outputs
where the page names one; that fact does not shape any proposition's
wording (see the Sync section below). Port names, stimulus values and the
`expect()` assertions are in the test source.

## Claim inventory (all 10, grouped by proposition)

| # | uid | page | role |
|---|---|---|---|
| 1 | `opus:8-display.c8`  | 260 | PROP-COUNT, primary |
| 2 | `sol:8-display.c7`   | 260 | PROP-COUNT, twin of #1 |
| 3 | `opus:8-display.c9`  | 260 | PROP-RED, primary |
| 4 | `sol:8-display.c8`   | 260 | PROP-RED, twin of #3 |
| 5 | `opus:8-display.c10` | 260 | PROP-GREEN, primary |
| 6 | `sol:8-display.c9`   | 260 | PROP-GREEN, twin of #5 |
| 7 | `opus:8-display.c11` | 260 | PROP-BLUE, primary |
| 8 | `sol:8-display.c10`  | 260 | PROP-BLUE, twin of #7 |
| 9 | `opus:8-display.c12` | 260 | PROP-SYNC, primary |
| 10 | `sol:8-display.c11` | 260 | PROP-SYNC, twin of #9 |

All 10 claims are accounted for; none is left unassigned. All
ten cite the same sentence, p.260's first bullet under "Two separate type of
RGB video are available on the connector":

> RGB Monitors ("analog RGB"). Provides four outputs; Red (R), Green (G),
> Blue (B), and Sync (S). They can generate up to 4,096 different colors
> on-screen simultaneously using the circuitry presently available on the
> Amiga.

Two readers extracted this sentence independently. `opus` quoted all four
printed lines of the bullet; `sol` quoted only the "Provides four outputs..."
clause. Neither reader's restatement reaches the "4,096 different colors"
clause — none of the 10 claims asserts a color-depth count, so that clause
is not a proposition this package tests.

**Crossread asymmetry, noted but not re-litigated:** `opus`'s five claims
were each crossread by `sol` and marked `verdict: ok`, `states_behaviour:
yes`. `sol`'s five claims, quoting the identical clause, were each crossread
by `opus` and marked `verdict: objected`, `states_behaviour: no`. Despite the
objection, `classify` marked all ten `status: testable`. This specification
does not resolve why the two readers disagreed on whether the same clause
"states a behaviour" — that adjudication belongs to `classify` — but it is
recorded here since a
reader of the twin table might otherwise wonder why five of ten carry a
dissenting crossread and still stand as graded propositions.

## The split every proposition below shares: the connector versus the signal set

The quoted sentence, read literally, describes four pins on the 23-pin
analog-RGB connector on the back of the machine. NanoMig has no such
connector. That is exactly why 129 of p.260's 143 claims were classified
`not-asserted` elsewhere in this slice, and it is why this package holds
only 10 of that page's claims rather than the whole page.

What survived into this package is not a claim about a connector, because a
connector claim would not be testable here — there is nothing to plug in.
What survived is the set of four named roles the sentence assigns to that
connector's pins: Red, Green, Blue, Sync. Each proposition below is worded
as a claim about that four-member *named signal set* — whatever this
design's video-output boundary does to carry a Red role, a Green role, a
Blue role and a Sync role — not as a claim that a 23-pin jack exists. This
reframing is not an invention of this specification; it is the boundary the
claims file's own classification already drew by putting these ten records
in this package and the connector-existence claims elsewhere as
not-constructible. It is recorded here so each PROP below reads as
testable rather than as a claim this design provably cannot satisfy.

## PROP-COUNT — The analog-RGB interface provides four outputs

Page 260.

> Provides four outputs; Red (R), Green (G), Blue (B), and Sync (S).
— `sol:8-display.c7`

> RGB Monitors ("analog RGB"). Provides four outputs; Red (R), Green (G),
> Blue (B), and Sync (S). They can generate up to 4,096 different colors
> on-screen simultaneously using the circuitry presently available on the
> Amiga.
— `opus:8-display.c8` (twin of `sol:8-display.c7`; longer quotation, same
clause)

**Proposition to test:** the named signal set this sentence assigns to the
analog-RGB interface has exactly four members — Red, Green, Blue, Sync — no
fewer. (Whether it has no *more* than four is not this proposition's
question on its own: PROP-SYNC below is where a plurality within the Sync
role specifically would surface, and this package does not grade that
plurality. This proposition is only about the floor the sentence states:
four named roles, present.)

Status: recorded, not separately asserted. The four-member count is a
structural fact of the port declaration that no runtime stimulus can change,
so it can only be measured as the conjunction of
PROP-RED/PROP-GREEN/PROP-BLUE/PROP-SYNC below — and an assertion must not
fail for a reason other than the one it names. The two uids sit on a
`record()` that names the four graded assertions carrying the content.

## PROP-RED — One of the analog-RGB outputs is Red, abbreviated R

Page 260.

> One of the analog RGB outputs is Red, abbreviated R.
— restatement of `opus:8-display.c9` and `sol:8-display.c8` (twins, same
clause as PROP-COUNT, quoted above)

**Proposition to test:** the analog-RGB signal set includes a role the page
calls Red (R). At this fixture's boundary, this is a claim about a
video-output signal whose carried value corresponds to the design's
red-intensity data — not about a specific port name (see "What this
specification deliberately does not contain" below).

Status: graded (RED-OUTPUT-CARRIES-COLOR00), GREEN.

## PROP-GREEN — One of the analog-RGB outputs is Green, abbreviated G

Page 260. Same clause as PROP-COUNT.

> One of the analog RGB outputs is Green, abbreviated G.
— restatement of `opus:8-display.c10` and `sol:8-display.c9` (twins)

**Proposition to test:** the analog-RGB signal set includes a role the page
calls Green (G), by the same reasoning as PROP-RED.

Status: graded (GREEN-OUTPUT-CARRIES-COLOR00), GREEN.

## PROP-BLUE — One of the analog-RGB outputs is Blue, abbreviated B

Page 260. Same clause as PROP-COUNT.

> One of the analog RGB outputs is Blue, abbreviated B.
— restatement of `opus:8-display.c11` and `sol:8-display.c10` (twins)

**Proposition to test:** the analog-RGB signal set includes a role the page
calls Blue (B), by the same reasoning as PROP-RED.

Status: graded (BLUE-OUTPUT-CARRIES-COLOR00), GREEN.

## PROP-SYNC — One of the analog-RGB outputs is Sync, abbreviated S

Page 260. Same clause as PROP-COUNT.

> One of the analog RGB outputs is Sync, abbreviated S.
— restatement of `opus:8-display.c12` and `sol:8-display.c11` (twins)

**Proposition to test:** the analog-RGB signal set includes a role the page
calls Sync (S) — one named role, singular in the sentence. How many
physical signals realize that one role is not a wording question for this
proposition: the sentence names a role, not a wire count, and this
specification does not assert that the role must be realized by exactly one
signal. The proposition is worded so that either a one-signal or a
multi-signal realization of the Sync role can satisfy it; this package does
not grade the count of sync signals.

Status: graded (SYNC-OUTPUT-TOGGLES), GREEN — the check samples the
composite sync output as the page's singular "Sync" and requires it to be
live (low for part of a sampled line, high for the rest); its mutation
freezes it.

## Claims not assigned an assertion

None. All 10 claims are classified `testable` in the claims file and each is
assigned to exactly one of the five propositions above (PROP-COUNT: claims
1, 2; PROP-RED: claims 3, 4; PROP-GREEN: claims 5, 6; PROP-BLUE: claims 7,
8; PROP-SYNC: claims 9, 10 — see the inventory table for the exact
uid-to-proposition mapping). PROP-COUNT's two uids are recorded rather than
graded (see PROP-COUNT above).

## What this specification deliberately does not contain

Port names, signal widths, which minimig-aga submodule assembles which
signal, how many wires realize the Sync role, and concrete stimulus values
belong in the test source, not here.

## Outcome

`make -B test`: 6 passed, 0 failed — the two controls
(CONTROL-RESET-RELEASES, CONTROL-VIDEO-BASELINE) and the four graded
propositions (RED/GREEN/BLUE-OUTPUT-CARRIES-COLOR00, SYNC-OUTPUT-TOGGLES);
PROP-COUNT recorded.

## Lineage note

Checked against the upstream `MiSTer-devel/Minimig-AGA_MiSTer` file rather
than assuming a slice-wide "shared-lineage" statement applies:
`rtl/minimig.v` on that project's `master` branch (fetched
directly, not from a document) declares the identical port set this
design's `minimig.v` declares for this proposition's subject — `_hsync`,
`_vsync`, `_csync` and `red`/`green`/`blue` as 8-bit outputs, at the
equivalent point in the port list, with the same inline comments
("horizontal sync" / "vertical sync" / "composite sync"). This package's
propositions are therefore statements about the Minimig line's video-output
boundary, not about this board alone — as for the other `src/minimig-aga`
packages, confirmed here rather than assumed.
