# Specification: ch8-appb-chip-column-paula

Fixture: `paula`, compiled alone (module name and file to be located under
`$(NANOMIG)/src/minimig-aga/` in step 2 -- not yet opened while this document
is written).

Subject: printed p.289's Appendix B register-map rows for POT0DAT, POT1DAT,
POTGOR, SERDATR, SERDAT, SERPER and POTGO, each row's CHIP column composed
with printed p.287's chip-letter legend, predicting that the register lives
inside the Paula chip.

Written from the quotations in `ch8-appb-chip-column-paula.jsonl` alone (14
claims, 7 register rows x 2 readers each). No RTL has been opened to produce
this document. Every claim is cited by its `classify.uid` (`<reader>:<id>`),
never the bare `id`, per BUILD-BRIEF -- a twin pair that asserts the identical
composed fact is cited together, both uids on the same graded or declined
line.

## The marker between register name and address

Every one of the seven printed rows carries a marker between the register
name and the printed address, e.g. `POT0DAT      *012` -- consistent with
this chapter's known Appendix B layout trap: a pattern that skips that gap
silently counts too few rows. All seven quotations below, taken verbatim from
the claims file, keep the marker; none is normalised away.

## The (E) marker -- explicitly out of scope on this page

POT0DAT's and POT1DAT's CHIP cell reads `P (E)`, not bare `P`. Printed p.287
-- the legend page these two claims cite for the letter `P` -- does not
define `(E)` (both claims' `classify.conflict_resolution` measure zero
occurrences of the string "(E)" on p.287). `(E)` IS defined on printed
p.288 -- "Registers denoted with an "(E)" in the chip column means that
those registers have been changed in the Enhanced Chip Set (ECS)" -- and that
definition is already graded as testable by chapter 7's accepted
`opus:7-appB.c11` ("yes, page 288"). Nothing here re-derives or re-grades
that p.288 definition, and `(E)` is NOT read as "ECS" by this package. These
two claims assert only the composed `P` half (Paula-chip ownership); the
`(E)` half is not carried into any assertion here, and nothing in this
package compares POT0DAT or POT1DAT between OCS and ECS. This mirrors each
claim's own `classify.split`.

## Testable propositions

Each row states a CHIP-column letter; printed p.287 states what the letter
means (quoted, per every claim's `classify.source_defines`, as: `A=Agnus
chip, D=Denise chip, P=Paula chip.`). A row and its legend state one fact
BETWEEN them -- the composed fact, "*register* is a Paula register," is what
this package grades, not the row cell or the legend line read separately.

Whether the `paula` fixture can observe that composed fact for a given row is
a question about the RTL, answered only in step 2, after this document is
committed. Two outcomes are possible per row, and this document commits to
the rule, not to which row lands in which bucket -- that determination has
not been made yet:

- If the row's address is decoded somewhere inside `paula`'s own instantiated
  hierarchy, this file's only DUT is `paula` and whatever it itself
  instantiates, with no other chip's decode logic present anywhere in the
  build. Chip ownership is then structural: a round trip of a distinct, known
  value through the register's own address is simultaneously the register's
  functional-content check and its CHIP=P check, the same structural argument
  chapter 7's accepted `paula_intcontroller` package already uses for
  INTENAR/INTREQR. The composed fact is asserted with `expect()`.
- If the row's address is not decoded anywhere inside `paula`'s own
  instantiated hierarchy, the CHIP-column claim is outside what this fixture
  can observe at all -- a chip-column letter names which chip answers, and a
  fixture that never receives that chip's decode logic cannot return either a
  yes or a no about it. Per BUILD-BRIEF's carried-forward rule for
  CHIP-column claims, this is declined with `record()`, naming the module
  that would decide it -- the fixture is not widened to force an answer.

### T1 -- POT0DAT, address $012

> "POT0DAT      *012   R           P (E)Pot counter pair 0 data (vert,horiz)"
> (printed p.289, `opus:8-appB.c20`)
> "POT0DAT      *012   R           P (E)" (printed p.289, `sol:8-appB.c19`,
> same row, shorter extract)

Composed with printed p.287's legend. Testable form: POT0DAT is a Paula
register, predicting that address $012 is decoded within `paula`'s own
hierarchy. The `(E)` mark is out of scope (see above).

Cited by: `opus:8-appB.c20`, `sol:8-appB.c19` (twin, same composed
proposition, both taken).

### T2 -- POT1DAT, address $014

> "POT1DAT      *014   R           P (E)Pot counter pair 1 data (vertfhoriz)"
> (printed p.289, `opus:8-appB.c25`)
> "POT1DAT      *014   R           P (E)" (printed p.289, `sol:8-appB.c24`,
> same row, shorter extract)

Composed with printed p.287's legend. Testable form: POT1DAT is a Paula
register, predicting that address $014 is decoded within `paula`'s own
hierarchy. The `(E)` mark is out of scope (see above).

Cited by: `opus:8-appB.c25`, `sol:8-appB.c24` (twin, same composed
proposition, both taken).

### T3 -- POTGOR, address $016

> "POTGOR       *016   R           P    Pot port data read (formerly
> POTINP)" (printed p.289, `opus:8-appB.c30`)
> "POTGOR       *016   R           P" (printed p.289, `sol:8-appB.c29`, same
> row, shorter extract)

Composed with printed p.287's legend. Testable form: POTGOR is a Paula
register, predicting that address $016 is decoded within `paula`'s own
hierarchy.

Cited by: `opus:8-appB.c30`, `sol:8-appB.c29` (twin, same composed
proposition, both taken).

### T4 -- SERDATR, address $018

> "SERDATR      *018   R           P    Serial port data and status read"
> (printed p.289, `opus:8-appB.c35`)
> "SERDATR      *018   R           P" (printed p.289, `sol:8-appB.c34`, same
> row, shorter extract)

Composed with printed p.287's legend. Testable form: SERDATR is a Paula
register, predicting that address $018 is decoded within `paula`'s own
hierarchy.

Cited by: `opus:8-appB.c35`, `sol:8-appB.c34` (twin, same composed
proposition, both taken).

### T5 -- SERDAT, address $030

> "SERDAT       *030   W           P    Serial port data and stop bits
> write" (printed p.289, `opus:8-appB.c40`)
> "SERDAT       *030   W           P" (printed p.289, `sol:8-appB.c39`, same
> row, shorter extract)

Composed with printed p.287's legend. Testable form: SERDAT is a Paula
register, predicting that address $030 is decoded within `paula`'s own
hierarchy.

Cited by: `opus:8-appB.c40`, `sol:8-appB.c39` (twin, same composed
proposition, both taken).

### T6 -- SERPER, address $032

> "SERPER       *032   W           P    Serial port period and control"
> (printed p.289, `opus:8-appB.c45`)
> "SERPER       *032   W           P" (printed p.289, `sol:8-appB.c44`, same
> row, shorter extract)

Composed with printed p.287's legend. Testable form: SERPER is a Paula
register, predicting that address $032 is decoded within `paula`'s own
hierarchy.

Cited by: `opus:8-appB.c45`, `sol:8-appB.c44` (twin, same composed
proposition, both taken).

### T7 -- POTGO, address $034

> "POTGO        *034   W           P    Pot port data write and start"
> (printed p.289, `opus:8-appB.c50`)
> "POTGO        *034   W           P" (printed p.289, `sol:8-appB.c49`, same
> row, shorter extract)

Composed with printed p.287's legend. Testable form: POTGO is a Paula
register, predicting that address $034 is decoded within `paula`'s own
hierarchy.

Cited by: `opus:8-appB.c50`, `sol:8-appB.c49` (twin, same composed
proposition, both taken).

## Fixture liveness

Per BUILD-BRIEF, before any register's content is trusted the fixture must be
shown to deliver a known, distinct word through at least one of its own
registers -- no claim id, no manual page, this package's own precondition.
Wherever step 2 confirms the `paula` boundary can observe a row at all, two
distinct patterns are round-tripped through that register before it is used
to compose any CHIP claim.

## Control case

BUILD-BRIEF requires at least one assertion expected to PASS, on the same
measurement basis as anything under suspicion, so a red result cannot be
dismissed as a units error. Every row of T1-T7 that step 2 finds decoded
inside `paula`'s own hierarchy serves this role for the rows that are not:
same fixture, same address-bus protocol, same round-trip-and-compare method,
differing only in which address is driven and what the RTL does with it.

SUPERSEDED by the REPAIR below: after 15.09.2026, no T1-T7 row is graded
with `expect()`, so none of them can serve as this package's PASS control.
The control-case requirement is instead satisfied entirely by the A0
fixture-liveness assertions (ADKCON round-trip), which are on the same
address-bus protocol and round-trip-and-compare method as every declined
row, and remain PASS. A package that declines every claim-bearing row is a
legitimate outcome under BUILD-BRIEF's HALT/decline provisions provided its
liveness control still stands -- it does here.

## What step 2 will determine, not invent

This document names no port, no address width, no clock or reset protocol,
and no submodule `paula` itself instantiates -- BUILD-BRIEF's gate against a
specification anchored to the implementation. Those are read from the RTL
only after this file is committed, and the assert/decline split for T1-T7 is
decided there, not here.

SUPERSEDED by the REPAIR below for the same reason: the "split" language
above assumed a real two-way split existed. After the repair, all seven T1-T7
rows land on the same side.

## REPAIR (15.09.2026) -- correcting an asymmetric rule assumed above

The "Testable propositions" section above committed, before the RTL was
opened, to a two-branch rule: grade a *present* decode with `expect()`,
decline an *absent* one with `record()`. Step 2 found both branches actually
fire in this RTL -- POT0DAT/POT1DAT/POTGOR/POTGO are decoded nowhere in
`paula`'s own hierarchy (declined), while SERDATR/SERDAT/SERPER are decoded
inside paula_uart.v, itself instantiated inside `paula` (originally graded
`expect()`-green). Independent validation
(`2026-09-14_ch8_s1_sol_validation_A.md`, defect 8) found that split
indefensible: the same kind of structural fact -- an address's presence or
absence inside this fixture's own compiled module hierarchy -- was treated
as proof when present and as merely unobservable when absent, in the same
package, argued from the same legend.

Printed p.287 settles which reading is correct. Its CHIP-column legend reads
only:

    A,D,P
        A=Agnus chip, D=Denise chip, P=Paula chip.

This names a physical 1985 chip; it does not mention a module hierarchy, an
RTL boundary, or an instantiation tree -- those concepts do not appear on
p.287 at all. `paula`, `userio` and `denise` are this open-source FPGA
clone's own engineering partition of the reimplemented logic; nothing on
p.287 or p.289 establishes that this partition tracks the three original
chips' silicon boundaries. So a decode's presence inside the Verilog module
named `paula` is evidence about this clone's source layout, not evidence
that the physical Paula chip answers there -- exactly as its absence was
already (correctly) treated as no evidence either way for the POT rows.

The corrected rule, applied identically to both this package and sibling
`ch8-appb-chip-column-denise`: presence and absence inside this clone's
module hierarchy are the same kind of structural fact, and both are
DECLINED with `record()`. All seven T1-T7 rows (14 claims) are declined;
none is asserted with `expect()`. SERDATR/SERDAT/SERPER's former `expect()`
grading and their three MUTATION records are walked back accordingly -- see
`test_paula.cpp`'s file-header REPAIR note for the disposition-level detail,
including why this is a legitimate, reportable outcome rather than a defect
in the fixture: this suite, bounded to RTL-module-hierarchy observation,
cannot settle any of these fourteen claims, because the legend's letters
assert physical chip identity, which this FPGA clone's module partitioning
is not shown anywhere to track.
