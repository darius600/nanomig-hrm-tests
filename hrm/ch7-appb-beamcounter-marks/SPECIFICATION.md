# Specification — `ch7-appb-beamcounter-marks`

Package: `hrm/ch7-appb-beamcounter-marks`
Fixture: `agnus_beamcounter`
Claims file: `hrm-testpipeline/work/ch7-build/ch7-appb-beamcounter-marks-spec-stage.jsonl` (24 claims)

Subject: Appendix B's per-register marks for the beam registers — **direction**
(R/W), **chip column** (Agnus ownership) and the **"(E)" enhanced-chip-set
mark** — for VPOSR $004, VHPOSR $006, VPOSW $02A, VHPOSW $02C and BEAMCON0
$1DC.

**Address is out of scope.** `hrm/ch3-register-map-agnus-beamcounter` already
asserts the address of all five registers from these same printed rows; this
claims file's own `covered_by`/`split` fields on the five bundled `opus`
records (`opus:7-appB.c15/.c17/.c25/.c27/.c35`) confirm this by measurement
against that package's own `out.txt` ("its 26 result lines are
VPOSR-ADDRESS-004, ... plus ten ADDRESS ... lines"; "There is NO direction and
NO chip-ownership assertion for any of these five" — which is exactly this
package's job). No claim below is turned into an address assertion.

This file is written from the `extract.quote` / `restatement` fields of the 24
claims alone. No RTL file for this package has been opened. It fixes the
printed propositions to be tested; port names, concrete stimulus values, and
the `expect()` assertions themselves are added only after this file is
committed and step 3 (opening the RTL under `$(NANOMIG)/src/minimig-aga/`)
begins, per `BUILD-BRIEF.md`.

## Claim inventory (all 24)

| # | uid | page | role |
|---|---|---|---|
| 1 | `sol:7-appA-legend.c5` | 264 | LEGEND-A ("(E)" defined), primary |
| 2 | `sol:7-appA-legend.c6` | 264 | LEGEND-A2 (two classes of "(E)"), primary |
| 3 | `opus:7-appA-pre.c10` | 263 | UNIVERSAL-EXCLUSIVITY (R xor W), primary |
| 4 | `opus:7-appA-pre.c15` | 264 | LEGEND-A, twin of #1 |
| 5 | `opus:7-appA-pre.c17` | 264 | LEGEND-A2, twin of #2 (crossread override recorded) |
| 6 | `sol:7-appB-legend.c8` | 288 | LEGEND-B ("(E)" defined again, for App. B), primary |
| 7 | `opus:7-appB.c11` | 288 | LEGEND-B, twin of #6 |
| 8 | `sol:7-appB-vposr.c2` | 289 | PROP-D1 (VPOSR read-only), primary |
| 9 | `sol:7-appB-vposr.c3` | 289 | PROP-C1 (VPOSR = Agnus), primary |
| 10 | `sol:7-appB-vposr.c6` | 289 | PROP-E1 (VPOSR "(E)" = extended), primary |
| 11 | `sol:7-appB-vhposr.c2` | 289 | PROP-D2 (VHPOSR read-only), primary |
| 12 | `sol:7-appB-vhposr.c3` | 289 | PROP-C2 (VHPOSR = Agnus), primary; also PROP-CTRL1 evidence |
| 13 | `sol:7-appB-vposw.c2` | 289 | PROP-D3 (VPOSW write-only), primary |
| 14 | `sol:7-appB-vposw.c3` | 289 | PROP-C3 (VPOSW = Agnus), primary; also PROP-CTRL2 evidence |
| 15 | `sol:7-appB-vhposw.c2` | 289 | PROP-D4 (VHPOSW write-only), primary |
| 16 | `sol:7-appB-vhposw.c3` | 289 | PROP-C4 (VHPOSW = Agnus), primary; also PROP-CTRL3 evidence |
| 17 | `sol:7-appB-beamcon0.c2` | 293 | PROP-D5 (BEAMCON0 write-only), primary |
| 18 | `sol:7-appB-beamcon0.c3` | 293 | PROP-C5 (BEAMCON0 = Agnus, enhanced), primary |
| 19 | `sol:7-appB-beamcon0.c5` | 293 | PROP-E2 (BEAMCON0 "(E)" = completely new), primary |
| 20 | `opus:7-appB.c15` | 289 | confirmation twin, VPOSR bundled row (PROP-D1/C1/E1) |
| 21 | `opus:7-appB.c17` | 289 | confirmation twin, VHPOSR bundled row (PROP-D2/C2/CTRL1) |
| 22 | `opus:7-appB.c25` | 289 | confirmation twin, VPOSW bundled row (PROP-D3/C3/CTRL2) |
| 23 | `opus:7-appB.c27` | 289 | confirmation twin, VHPOSW bundled row (PROP-D4/C4/CTRL3) |
| 24 | `opus:7-appB.c35` | 293 | confirmation twin, BEAMCON0 bundled row (PROP-D5/C5/E2) |

All 24 claims are accounted for; none is left unassigned at this stage.

## LEGEND-A — Appendix A's own legend for "(E)"

Page 264.

> Registers denoted with an "(E)" in the chip column means that those
> registers have been changed the Enhanced Chip Set(ECS).
— `sol:7-appA-legend.c5`

> Registers denoted with an "(E)" in the chip column means that those
> registers have been changed the Enhanced Chip Set(ECS).
— `opus:7-appA-pre.c15` (twin of `sol:7-appA-legend.c5`; opus's own extract
was shortened mid-sentence, its crossread restored it to the identical
wording sol already quoted in full)

This legend is not itself worded about any one register, so — as in the
sibling `ch7-appb-copper-write-protection` package's treatment of its own
Copper-mark legend — it is not scored as its own proposition. Its content is
realized jointly in PROP-E1 and PROP-E2 below, cited alongside each
(E)-marked row.

**Scope note:** the claims' own `split` field records that Appendix A's
"(E)" legend, in this printed slice, actually names *three* rows: VPOSR,
BEAMCON0, and BPLCON2 $104. BPLCON2 is a Denise register and therefore a
different fixture; per that same `split` field it is "covered by
PKG-CH7-DENISE-ECS-MARK," not this package. Only VPOSR and BEAMCON0 belong
here.

## LEGEND-A2 — the two classes an "(E)" mark can mean

Page 264.

> Certain ECS registers are completely new, others have been extended
> in their functionality.
— `sol:7-appA-legend.c6`

> Certain ECS registers are completely new, others have been extended
> in their functionality.
— `opus:7-appA-pre.c17` (twin of `sol:7-appA-legend.c6`, restored quote; this
record's own crossread marked it `excluded-non-behaviour`, but the claims
file's own `classify.crossread_use` field overrules that verdict as a
measurement, not a preference: "BOTH classes are instantiated on one fixture
in this slice ... BEAMCON0 is the 'completely new' case (writes are ignored
at ecs=0) and VPOSR is the 'extended' case (same address, one extra bit at
ecs=1)." This specification follows that resolution.)

This sentence is what makes PROP-E1 and PROP-E2 two different propositions
rather than one: VPOSR's "(E)" means "extended" (same address, added ECS
behavior); BEAMCON0's "(E)" means "completely new" (the write path exists
only under ECS).

## LEGEND-B — Appendix B restates the same legend for its own table

Page 288.

> About the ECS registers. Registers denoted with an "(E)" in the chip column means
> that those registers have been changed in the Enhanced Chip Set (ECS). The ECS is
— `opus:7-appB.c11` (full corpus extract, includes the heading sentence and a
trailing fragment; the claim's own note records that Appendix B exists in
this corpus only as a printed scan)

> Registers denoted with an "(E)" in the chip column means
> that those registers have been changed in the Enhanced Chip Set (ECS).
— `sol:7-appB-legend.c8` (twin of `opus:7-appB.c11`, without the heading
sentence and trailing fragment)

Cited jointly with PROP-E1 and PROP-E2, alongside LEGEND-A: Appendix A
defines the mark once for the whole register map (p.264), Appendix B repeats
it for its own table (p.288); both name the same rule.

## UNIVERSAL-EXCLUSIVITY — every register is read-only or write-only, never both

Page 263.

> Registers are either read-only or write-only.
— `opus:7-appA-pre.c10`

This is a universal claim over every custom-chip register on all three
chips, and the claims file's own `split` field records it is split three
ways across three fixtures/packages: `hrm/ch3-register-map-denise` and
`hrm/ch5-audio-interrupt` already assert its Denise and Paula instances at
their own printed rows (citing pp. 289–291 in their own headers). This
package is where its Agnus-beamcounter instance is asserted: VPOSR, VHPOSR,
VPOSW, VHPOSW and BEAMCON0. It names no single register, so it is not scored
as its own proposition; it is cited jointly with PROP-D1 through PROP-D5,
where its content is realized — a register found to answer in its documented
direction is also checked not to answer in the other one.

## PROP-D1 through PROP-D5 — direction (read-only / write-only)

| Prop | Register | Offset | Direction | Row quote (printed, p.289/293) | Primary uid | Confirming uid |
|---|---|---|---|---|---|---|
| PROP-D1 | VPOSR | $004 | read-only | `VPOSR        *004   R     A (E)      Read vert most signif. bit (and frame flop)` | `sol:7-appB-vposr.c2` | `opus:7-appB.c15` |
| PROP-D2 | VHPOSR | $006 | read-only | `VHPOSR       *006   R     A          Read vert and horiz. position of beam` | `sol:7-appB-vhposr.c2` | `opus:7-appB.c17` |
| PROP-D3 | VPOSW | $02A | write-only | `VPOSW        *02A   W     A          Write vert most signif. bit (and frame flop)` | `sol:7-appB-vposw.c2` | `opus:7-appB.c25` |
| PROP-D4 | VHPOSW | $02C | write-only | `VHPOSW       *02C   W     A          Write vert and horiz position of beam` | `sol:7-appB-vhposw.c2` | `opus:7-appB.c27` |
| PROP-D5 | BEAMCON0 | $1DC | write-only | `BEAMCON0      1DC   W      A(E)   Beam counter control register (SHRES,PAL)` (restored quote — the corpus row itself carries a lower-case "w", which the claim's own crossread records as unrepaired OCR residue against both the legend's letter and the rendered page's "W") | `sol:7-appB-beamcon0.c2` | `opus:7-appB.c35` |

The five bundled `opus` rows are cited as the second reader's confirmation
per their own `split` field ("Cite this uid as the second reader's
confirmation, not as the assertion's subject"), not as an independent
proposition; sol's per-field claims are the assertion's subject.

**Proposition to test, per row:** for the read-only pair (PROP-D1, D2), the
register answers reads with data, and a write directed at that same address
does not change what is subsequently read there. For the write-only trio
(PROP-D3, D4, D5), a write to the register's address changes observable
state, and a read of that same address does not return the value most
recently written to it. Both halves are jointly required by
UNIVERSAL-EXCLUSIVITY: never both directions at once.

**Chipset scope:** PROP-D1 and PROP-D5 carry an "(E)" mark, so their
*existence/behavior* is chipset-dependent (see PROP-E1/E2), but no claim
here says VPOSR stops being read-only, or BEAMCON0 stops being write-only,
at ecs=0 — only that BEAMCON0's write has no observable effect there. PROP-D2,
D3, D4 carry no chipset field in their own claims: both OCS and ECS.

**Precondition:** PROP-D5 (BEAMCON0 write-only) can only be shown green on
the ecs=1 half of the fixture run — per `classify.chipset.stated_at` on
`sol:7-appB-beamcon0.c2/.c3/.c5`, "the write decode is gated on `ecs`, so at
ecs=0 the register does not exist as far as this boundary can tell." Per
`BUILD-BRIEF.md` — "where a green depends on a precondition that might never
occur, assert the precondition too" — that precondition is exactly PROP-E2
below, which this specification already carries.

## PROP-C1 through PROP-C5 — chip column (Agnus ownership)

| Prop | Register | Offset | Primary uid | Confirming uid |
|---|---|---|---|---|
| PROP-C1 | VPOSR | $004 | `sol:7-appB-vposr.c3` | `opus:7-appB.c15` |
| PROP-C2 | VHPOSR | $006 | `sol:7-appB-vhposr.c3` | `opus:7-appB.c17` |
| PROP-C3 | VPOSW | $02A | `sol:7-appB-vposw.c3` | `opus:7-appB.c25` |
| PROP-C4 | VHPOSW | $02C | `sol:7-appB-vhposw.c3` | `opus:7-appB.c27` |
| PROP-C5 | BEAMCON0 | $1DC | `sol:7-appB-beamcon0.c3` | `opus:7-appB.c35` |

Row quotes are the same as in the PROP-D table above (each claim's `extract`
is the identical printed row; sol's per-field records simply restate a
different field of it — the "A" / "A (E)" chip column rather than the R/W
column).

**Proposition to test, per row:** the fixture decodes and answers at this
address as one of Agnus's own registers — distinguishable from an address
the fixture does not claim at all. Every row quoted anywhere in this claims
file for this fixture already carries the "A" chip-column letter (none is
marked for Denise or Paula), so there is no in-file address of a different
chip to compare against directly; the negative half of this proposition (an
unclaimed address is *not* answered) is carried by the mutation described
below, using an address outside these five rows rather than a
differently-owned one.

**Chipset scope:** PROP-C5 (BEAMCON0) is additionally marked "(E)" — Appendix
C p.305 is cited by the claims file as identifying it with the *enhanced*
Agnus specifically (`sol:7-appB-beamcon0.c3`'s own `chipset.stated_at`). The
other four carry no such qualifier.

## PROP-E1, PROP-E2 — the "(E)" mark itself

| Prop | Register | Class (per LEGEND-A2) | Primary uid | Confirming uid |
|---|---|---|---|---|
| PROP-E1 | VPOSR $004 | "extended" — same address, added ECS behavior | `sol:7-appB-vposr.c6` | `opus:7-appB.c15` |
| PROP-E2 | BEAMCON0 $1DC | "completely new" — exists only under ECS | `sol:7-appB-beamcon0.c5` | `opus:7-appB.c35` |

**Proposition to test — PROP-E1:** VPOSR's value differs measurably between
ecs=0 and ecs=1. `sol:7-appB-vposr.c6`'s own `chipset.stated_at` field:
"Appendix B p.288 defines (E); Appendix C p.299 gives the changed layout."
The claims file's own classification separately records a concrete instance
of this on the bundled confirming row (`opus:7-appB.c15`): reading $004
returns $8000 at ecs=0 and $A000 at ecs=1 — an added bit, not a changed one,
consistent with the "extended" class from LEGEND-A2.

**Proposition to test — PROP-E2:** BEAMCON0's write path is observable only
under ECS. `sol:7-appB-beamcon0.c5`'s own `chipset.stated_at` field: "the
write decode is gated on `ecs`, so at ecs=0 the register does not exist as
far as this boundary can tell" (repeated verbatim on `sol:7-appB-beamcon0.c2`
and `.c3`). A write to $1DC changes observable state at ecs=1 and has no
effect at ecs=0 — consistent with the "completely new" class from
LEGEND-A2.

**Chipset scope, both:** OCS and ECS, run as one fixture exercised twice —
`classify.chipset.stated_at` on the LEGEND-A/B claims states this directly:
"the assertion runs the fixture twice, at ecs=0 and ecs=1."

## PROP-CTRL1 through PROP-CTRL3 — controls: registers without the "(E)" mark

| Prop | Register | Offset | Absence of mark shown by (same row quoted in PROP-D/C above) |
|---|---|---|---|
| PROP-CTRL1 | VHPOSR | $006 | `sol:7-appB-vhposr.c2` / `.c3` — row prints "R A", no "(E)" |
| PROP-CTRL2 | VPOSW | $02A | `sol:7-appB-vposw.c2` / `.c3` — row prints "W A", no "(E)" |
| PROP-CTRL3 | VHPOSW | $02C | `sol:7-appB-vhposw.c2` / `.c3` — row prints "W A", no "(E)" |

**Proposition to test, per row:** this register's observable behavior is
identical at ecs=0 and ecs=1 — no measurable difference, unlike PROP-E1/E2.

These are `BUILD-BRIEF.md`'s required "at least one control case expected to
PASS on the same measurement basis as any case under suspicion" for
PROP-E1/E2: the same fixture, the same ecs=0-vs-ecs=1 comparison mechanism,
differing only in which address is compared. Without them, a red or green
PROP-E1/E2 would not be distinguishable from a harness that always reports
"differs" or always reports "identical" regardless of what it is fed.

## On mutation (per proposition group)

Concrete stimulus values and port names are step-3 material and are
deliberately not fixed here, but the shape of each mutation is already
visible from the claims alone:

- **PROP-D1..D5:** redirect the stimulus's target address, within this same
  five-row table, to one whose documented direction is the opposite of the
  one under test (R-group {VPOSR $004, VHPOSR $006} vs. W-group {VPOSW $02A,
  VHPOSW $02C, BEAMCON0 $1DC}), leaving the expected direction-outcome text
  unchanged. The check must go red, because the swapped-in address does not
  have that direction.
- **PROP-C1..C5:** redirect the stimulus's target address to a value outside
  these five documented rows, leaving the expected "this fixture answers as
  Agnus's own register" outcome unchanged. The check must go red, because
  none of this claims file's rows cover that address. (Whether the fixture's
  own boundary makes such an address observably distinct — silence rather
  than a decoded response — is confirmed once the RTL is read in step 3; if
  it does not, that is reported as a halt on this one proposition group
  rather than silently weakened.)
- **PROP-E1/PROP-E2 and PROP-CTRL1..3:** swap the register under comparison
  between the "(E)"-marked pair (VPOSR, BEAMCON0) and the unmarked trio
  (VHPOSR, VPOSW, VHPOSW), leaving the expected outcome text ("differs
  across ecs" for E1/E2, "identical across ecs" for CTRL1..3) unchanged.
  Each check must go red under the swap, because the two groups' actual
  ecs-dependence is exchanged.

This states the mutation strategy, not the literal `// MUTATION` header
lines or `expect()` calls; those are written in step 3.

**Fixture liveness**, per `BUILD-BRIEF.md` (applies to the harness as a
whole, not to any one printed claim): before any of the above propositions
are graded, the fixture must be shown to deliver distinct, known values on
the beam-position read path at the cycle they were fetched on — the chapter
6 lesson about a data path that is silently dead. This is a build-step
requirement carried forward, not a page citation.

## Claims not assigned an assertion

None outright unassigned; all 24 map onto the inventory table above. Four
claims/pairs — LEGEND-A (`sol:7-appA-legend.c5` / `opus:7-appA-pre.c15`),
LEGEND-A2 (`sol:7-appA-legend.c6` / `opus:7-appA-pre.c17`), LEGEND-B
(`sol:7-appB-legend.c8` / `opus:7-appB.c11`), and UNIVERSAL-EXCLUSIVITY
(`opus:7-appA-pre.c10`) — name no single register and are not scored as
their own proposition; each is cited jointly with the propositions above.
The five bundled `opus` records (`opus:7-appB.c15/.c17/.c25/.c27/.c35`) are
cited as the second reader's confirmation on each row per their own `split`
field, not as an independent proposition — their content is already carried
by sol's per-field primary claims for the same rows.

## What this specification deliberately does not contain

Port names, signal names and concrete stimulus values, per
`BUILD-BRIEF.md`'s order of work — added only once this file is committed
and the RTL is read for step 3.

Address assertions are absent by design: `hrm/ch3-register-map-agnus-beamcounter`
already asserts all five addresses from these same printed rows (measured
against its own `out.txt`, per this claims file's own `covered_by` fields),
and duplicating that here would be the same "one package must not compile
three fixtures" problem `opus:7-appA-pre.c10`'s own `split` field warns
against, applied within one fixture instead of across three.

Nothing here asserts a Copper-write mark either. The printed rows quoted in
this file for VPOSR, VHPOSR, VPOSW and VHPOSW each carry a leading "*" — a
mark this file never restates or explains, but which is visibly a different
column from the "(E)" chip-column mark this file's claims are actually
about. The BEAMCON0 row quoted here carries no such leading mark. No claim
in this 24-record staged file makes a restatement about that mark, for any
of the five registers, so none of it is turned into a proposition. This
package's own task sheet describes its subject, in part, as covering
"Copper-write marks" — that phrase is not met by any claim actually present
in `ch7-appb-beamcounter-marks-spec-stage.jsonl`; it is reported to the lead
as a gap rather than filled in by inventing a claim for it.
