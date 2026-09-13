# Specification — `ch7-appb-copper-write-protection`

Package: `hrm/ch7-appb-copper-write-protection`
Fixture: `agnus_copper`
Claims file: `hrm-testpipeline/work/ch7-build/ch7-appb-copper-write-protection-spec-stage.jsonl` (19 claims)
Subject: Appendix B's Copper write-protection marks — which addresses the
Copper may not write, and which it may write freely (no claim in this staged
file speaks to a "may write only with the danger bit set" mark — see the
closing section).

This file is written from the `extract.quote` / `restatement` fields of the
19 claims alone. No RTL file for this package has been opened. It fixes the
printed propositions to be tested; port names, concrete stimulus values, and
the `expect()` assertions themselves are added only after this file is
committed and step 3 (opening `agnus_copper.v`) begins, per `BUILD-BRIEF.md`.

## Claim inventory (all 19)

| # | uid | page | role |
|---|---|---|---|
| 1 | `sol:7-appB-legend.c4` | 287 | LEGEND, primary |
| 2 | `opus:7-appB.c5` | 287 | LEGEND, twin of #1 |
| 3 | `sol:7-appB-dmaconr.c5` | 289 | PROP-1, primary |
| 4 | `sol:7-appB-vposr.c5` | 289 | PROP-2, primary |
| 5 | `sol:7-appB-vhposr.c5` | 289 | PROP-3, primary |
| 6 | `sol:7-appB-clxdat.c5` | 289 | PROP-4, primary |
| 7 | `sol:7-appB-intenar.c5` | 289 | PROP-5, primary |
| 8 | `sol:7-appB-intreqr.c5` | 289 | PROP-6, primary |
| 9 | `sol:7-appB-vposw.c5` | 289 | PROP-7, primary |
| 10 | `sol:7-appB-vhposw.c5` | 289 | PROP-8, primary |
| 11 | `opus:7-appB.c14` | 289 | PROP-1, twin of #3 |
| 12 | `opus:7-appB.c16` | 289 | PROP-2, twin of #4 |
| 13 | `opus:7-appB.c18` | 289 | PROP-3, twin of #5 |
| 14 | `opus:7-appB.c20` | 289 | PROP-4, twin of #6 |
| 15 | `opus:7-appB.c22` | 289 | PROP-5, twin of #7 |
| 16 | `opus:7-appB.c24` | 289 | PROP-6, twin of #8 |
| 17 | `opus:7-appB.c26` | 289 | PROP-7, twin of #9 |
| 18 | `opus:7-appB.c28` | 289 | PROP-8, twin of #10 |
| 19 | `opus:7-appB.c33` | 290 | PROP-9, PROP-10, PROP-11, PROP-12 (one claim, split four ways) |

All 19 claims are accounted for; none is left unassigned at this stage.

## LEGEND — meaning of the `*` mark

Page 287.

> \*    Address not writable by the Copper.
— `sol:7-appB-legend.c4`

> \*    Address not writable by the Copper.
— `opus:7-appB.c5` (twin of `sol:7-appB-legend.c4`; identical quote, both
crossreads passed)

This legend is not itself worded about any one address, so it is not made
into its own scored assertion. Per the task sheet's instruction to "read the
legend as a statement about each marked address, not as page furniture," its
whole content is realized in PROP-1 through PROP-8 below: each of those
propositions cites this legend jointly with its own marked row, because the
row alone (a bare asterisk before an offset) has no meaning without it.

## PROP-1 through PROP-8 — eight addresses the Copper cannot write

Each of these pairs a page-289 table row with the page-287 legend above.
Both readers extracted the same eight rows independently (twins), and both
crossreads passed. The addresses in this file's own order:

| Prop | Register (row's own name) | Offset | Row quote (p.289) | Primary uid | Twin uid |
|---|---|---|---|---|---|
| PROP-1 | DMACONR | $002 | `DMACONR      *002   R     A     P    DMA control (and blitter status) read` | `sol:7-appB-dmaconr.c5` | `opus:7-appB.c14` |
| PROP-2 | VPOSR | $004 | `VPOSR        *004   R     A (E)      Read vert most signif. bit (and frame flop)` | `sol:7-appB-vposr.c5` | `opus:7-appB.c16` |
| PROP-3 | VHPOSR | $006 | `VHPOSR       *006   R     A          Read vert and horiz. position of beam` | `sol:7-appB-vhposr.c5` | `opus:7-appB.c18` |
| PROP-4 | CLXDAT | $00E | `CLXDAT       *00E   R        D       Collision data register (read and clear)` | `sol:7-appB-clxdat.c5` | `opus:7-appB.c20` |
| PROP-5 | INTENAR | $01C | `INTENAR      *01C   R           P    Interrupt enable bits read` | `sol:7-appB-intenar.c5` | `opus:7-appB.c22` |
| PROP-6 | INTREQR | $01E | `INTREQR      *01E   R           P    Interrupt request bits read` | `sol:7-appB-intreqr.c5` | `opus:7-appB.c24` |
| PROP-7 | VPOSW | $02A | `VPOSW        *02A   W     A          Write vert most signif. bit (and frame flop)` | `sol:7-appB-vposw.c5` | `opus:7-appB.c26` |
| PROP-8 | VHPOSW | $02C | `VHPOSW       *02C   W     A          Write vert and horiz position of beam` | `sol:7-appB-vhposw.c5` | `opus:7-appB.c28` |

**Wording note**, carried from the claims' own crossread notes (e.g. the
note on `sol:7-appB-dmaconr.c5`): the mark attaches to the **address**, not
to the register name printed next to it — appendix B is laid out in address
order, and the two starred rows that already name both register and offset
in their own restatement (`VPOSW`/`VHPOSW`, PROP-7/PROP-8) are the form the
crossread said the other six should also take. Accordingly every proposition
below is worded as "the Copper cannot write custom-chip offset **$xxx**,"
naming the row's register only as identification of which printed row that
offset comes from — not as a claim that the restriction is register-specific
rather than address-specific.

**Proposition to test, per row:** a Copper-issued write cycle targeting
custom-chip offset $xxx (as listed above) does not take effect — the
addressed register's contents are unchanged after the attempted write.

**Chipset scope:** OCS only, for all eight. This scope is taken directly
from each claim's own `classify.chipset.applies_to: ["OCS"]` field in the
claims file — not from any RTL reading. The claims file's own stated reason
is that the printed chapter (p.224) scopes this material to OCS and
Appendix C (p.310) is where the ECS-scaled ranges live; that is a fact about
which pages govern which chipset, not a fact about port names or signals,
so using it here does not violate the "no RTL before commit" rule. All
eight of these propositions must be exercised with the fixture configured
for OCS, and not asserted for ECS.

Status: testable, all eight.

## PROP-9 through PROP-12 — four addresses with no restriction mark

Page 290.

> DMACON         096    W      D        DMA control write (clear or set)
> CLXCON         098    W      D        Collision control
> INTENA         09A    W               Interrupt enable bits (clear or
>                                          set bits)
> INTREQ         09C    W               Interrupt request bits (clear or
>                                          set bits)
— `opus:7-appB.c33` (one claim record, four table rows)

The claim's own crossread marked this record `defective`
(`multiple-fields-in-one-claim`): one record combining four rows. Its own
`classify.split` field resolves that defect explicitly: "the package makes
four assertions, not one." This specification follows that instruction and
splits the one claim into four propositions, each citing the same uid
`opus:7-appB.c33` (there is no other claim covering these rows).

The claim's `extract.flags` marks it `borderline` and its own note records
that the two readers "disagree in principle about whether a missing
restriction mark is a statement" — but `classify.status` is `testable`, and
the `split` field gives the reasoning the disagreement was resolved by: the
legend column (page 287) enumerates Copper restrictions, so a row inside
that enumeration carrying no mark is inside the enumeration's scope, unlike
a blank cell in an unrelated column. This package proceeds on that
resolution.

**Wording note**, by the same reasoning applied to PROP-B in a sibling
package's specification: "no mark" is being read here as an observable
about what the hardware does, not as an endorsement of programmer practice.
Each proposition below is worded as an observable, not as a permission.

| Prop | Register | Offset |
|---|---|---|
| PROP-9 | DMACON | $096 |
| PROP-10 | CLXCON | $098 |
| PROP-11 | INTENA | $09A |
| PROP-12 | INTREQ | $09C |

**Proposition to test, per row:** a Copper-issued write cycle targeting
custom-chip offset $xxx (as listed above) takes effect — the addressed
register's contents change to reflect the written value.

**Chipset scope:** both OCS and ECS. The claim's own
`classify.chipset.applies_to` lists `["OCS", "ECS"]` and its `stated_at`
field says explicitly this proposition "must be RUN at both, or it only
half tests the proposition." This specification carries that instruction
forward: each of PROP-9 through PROP-12 is exercised once with the fixture
configured for OCS and once for ECS at step 3 — two graded checks per
proposition, not a doubling of the proposition count.

Status: testable, all four (one claim, split four ways as directed).

## On a control case and on mutation

`BUILD-BRIEF.md` requires at least one control case expected to PASS on the
same measurement basis as any case under suspicion, and a stimulus mutation
per assertion. Concrete stimulus values and port names are step-3 material
and are deliberately not fixed here, but the shape of both is already
visible from the claims alone and is recorded so step 3 does not have to
re-derive it:

- PROP-9 through PROP-12 (freely writable) are the natural control cases
  for PROP-1 through PROP-8 (protected): both groups are exercised through
  the same Copper write mechanism, on the same fixture, differing only in
  target address — so a PROP-9..12 PASS demonstrates the harness's write
  path is live on the very same basis the protected-address checks rely on
  to mean anything.
- A mutation that needs no port names: for a protected-address proposition
  (PROP-1..8), redirect the stimulus's target address to one of the
  freely-writable offsets (PROP-9..12) while leaving the expected outcome
  ("unchanged") alone — the check must go red, because that address is
  writable. Symmetrically, a freely-writable proposition's (PROP-9..12)
  mutation redirects its target address to one of the protected offsets
  (PROP-1..8) while leaving its expected outcome ("changed to written
  value") alone — the check must go red, because that address is
  protected. This uses only facts already established by this claims file
  (which offsets are marked and which are not), no RTL fact.

This section states a strategy, not the assertions themselves; the actual
`expect()` calls, their exact stimulus values, and the `// MUTATION` header
lines are written in step 3.

## Claims not assigned an assertion

None outright unassigned. Two claims (`sol:7-appB-legend.c4`,
`opus:7-appB.c5`) are not made into their own scored proposition because
neither names an address — both are cited jointly with every one of
PROP-1 through PROP-8 instead, per the task sheet's own instruction on how
to read the legend. All other 17 claims map onto exactly one of PROP-1
through PROP-12 as shown in the inventory table.

## What this specification deliberately does not contain

Port names, signal names, and concrete stimulus values are absent by
design, per `BUILD-BRIEF.md`'s order of work — they are added only after
this file is committed and `agnus_copper.v` is read for step 3. Nothing in
this file asserts a "may write only with the danger bit set" mark: no claim
in `ch7-appb-copper-write-protection-spec-stage.jsonl` quotes such a mark
(some claims' own analysis fields mention, in passing, that a second mark
exists in the source material — that is not the same as this staged file
containing a claim about it), so this package's specification cannot rest
an assertion on it. That gap is reported to the lead rather than filled in
by guessing what the mark says or where it falls.
