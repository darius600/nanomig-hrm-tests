# Specification — `ch7-appb-copper-write-protection`

Package: `hrm/ch7-appb-copper-write-protection`
Fixture: `agnus_copper`
Claims file: `hrm-testpipeline/work/ch7-build/ch7-appb-copper-write-protection-spec-stage.jsonl` (19 claims)
Subject: Appendix B's Copper write-protection marks — which addresses the
Copper may not write, and which it may write freely (no claim in this staged
file speaks to a "may write only with the danger bit set" mark — see the
closing section).

This file is written from the `extract.quote` / `restatement` fields of the
claims alone. It fixes the printed propositions to be tested; port names,
concrete stimulus values, and the `expect()` assertions themselves are in the
test source.

A further claim is routed to this package: `ch2-appb-legend.c1`, page 287,
quote `"    *    Address not writable by the Copper."` — the identical
printed legend sentence already covered by `sol:7-appB-legend.c4` and
`opus:7-appB.c5` below, read independently a third time. It joins the
LEGEND entry as a third twin: it does not name an address, so it is not its
own scored proposition here. None of the three twins is graded in this
package: `hrm/ch2-copper-move-sweep`'s APPBSTAR LEGEND line sweeps all 32
of the legend's *-marked rows, while this package's own PROP-1 through
PROP-8 probe only 8 of the 32, and T2's unmarked-address reading does not
establish the *-marked side either (see "LEGEND" below).

## Claim inventory (all 20)

| # | uid | page | role |
|---|---|---|---|
| 1 | `sol:7-appB-legend.c4` | 287 | LEGEND, primary |
| 2 | `opus:7-appB.c5` | 287 | LEGEND, twin of #1 |
| 3 | `ch2-appb-legend.c1` | 287 | LEGEND, twin of #1 |
| 4 | `sol:7-appB-dmaconr.c5` | 289 | PROP-1, primary |
| 5 | `sol:7-appB-vposr.c5` | 289 | PROP-2, primary |
| 6 | `sol:7-appB-vhposr.c5` | 289 | PROP-3, primary |
| 7 | `sol:7-appB-clxdat.c5` | 289 | PROP-4, primary |
| 8 | `sol:7-appB-intenar.c5` | 289 | PROP-5, primary |
| 9 | `sol:7-appB-intreqr.c5` | 289 | PROP-6, primary |
| 10 | `sol:7-appB-vposw.c5` | 289 | PROP-7, primary |
| 11 | `sol:7-appB-vhposw.c5` | 289 | PROP-8, primary |
| 12 | `opus:7-appB.c14` | 289 | PROP-1, twin of #4 |
| 13 | `opus:7-appB.c16` | 289 | PROP-2, twin of #5 |
| 14 | `opus:7-appB.c18` | 289 | PROP-3, twin of #6 |
| 15 | `opus:7-appB.c20` | 289 | PROP-4, twin of #7 |
| 16 | `opus:7-appB.c22` | 289 | PROP-5, twin of #8 |
| 17 | `opus:7-appB.c24` | 289 | PROP-6, twin of #9 |
| 18 | `opus:7-appB.c26` | 289 | PROP-7, twin of #10 |
| 19 | `opus:7-appB.c28` | 289 | PROP-8, twin of #11 |
| 20 | `opus:7-appB.c33` | 290 | PROP-9, PROP-10, PROP-11, PROP-12 (one claim, split four ways) |

All 20 claims are accounted for; none is left unassigned.

## LEGEND — meaning of the `*` mark

Page 287.

> \*    Address not writable by the Copper.
— `sol:7-appB-legend.c4`

> \*    Address not writable by the Copper.
— `opus:7-appB.c5` (twin of `sol:7-appB-legend.c4`; identical quote, both
crossreads passed)

> \*    Address not writable by the Copper.
— `ch2-appb-legend.c1` (twin of `sol:7-appB-legend.c4` and
`opus:7-appB.c5`; identical quote, same page)

This legend is not itself worded about any one address, so it is not made
into its own scored assertion. p.287's legend covers all 32 *-marked rows of
Appendix B ($000-$03E), and this package's PROP-1 through PROP-8 probe only
8 of the 32 -- citing the legend jointly on those eight lines would grade a
universal claim on one case of eight-of-thirty-two. T2's unmarked-address
reading (DMACON, page 290) does not establish the three legend ids either:
it grades only the freely-writable side, never the *-marked side the legend
states. None of the three legend ids (`sol:7-appB-legend.c4`,
`opus:7-appB.c5`, `ch2-appb-legend.c1`) is cited on PROP-1 through PROP-8 or
on T2 below; all three are recorded instead (see "Claims not assigned an
assertion"). All three are graded on the whole band by
`hrm/ch2-copper-move-sweep`'s APPBSTAR LEGEND line, which sweeps all 32 *
rows at ecs=0 with CDANG clear and set.

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
which pages govern which chipset, not a fact about port names or signals.
All eight of these propositions must be exercised with the fixture
configured for OCS, and not asserted for ECS.

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
configured for OCS and once for ECS — two graded checks per proposition, not
a doubling of the proposition count.

Status: testable, all four (one claim, split four ways as directed).

## On a control case and on mutation

Every case under suspicion needs at least one control case expected to PASS
on the same measurement basis, and every assertion a stimulus mutation.
Concrete stimulus values and port names are not fixed here, but the shape
of both follows from the claims alone:

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
lines are in the test source.

## Claims not assigned an assertion

None outright unassigned. Three claims (`sol:7-appB-legend.c4`,
`opus:7-appB.c5`, `ch2-appb-legend.c1`) are not made into their own scored
proposition because none names an address, and none is cited on PROP-1
through PROP-8 or on T2 either: the legend covers all 32 *-marked rows of
Appendix B and this package's eight starred propositions probe only 8 of
the 32, while T2 grades only the freely-writable side (DMACON), never the
*-marked side the legend states. All three are `record()`ed instead, and
all three are graded on the whole band by `hrm/ch2-copper-move-sweep`'s
APPBSTAR LEGEND line. All other 17 claims map onto exactly one of PROP-1
through PROP-12 as shown in the inventory table.

## What this specification deliberately does not contain

Port names, signal names, and concrete stimulus values are absent by
design — they are in the test source. Nothing in this file asserts a "may
write only with the danger bit set" mark: no claim in
`ch7-appb-copper-write-protection-spec-stage.jsonl` quotes such a mark (some
claims' own analysis fields mention, in passing, that a second mark exists
in the source material — that is not the same as this staged file
containing a claim about it), so this package's specification cannot rest
an assertion on it, and does not guess what the mark says or where it
falls. The test source records where that mark is covered instead.
