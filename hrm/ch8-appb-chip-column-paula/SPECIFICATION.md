# Specification: ch8-appb-chip-column-paula

Fixture: `paula` (paula.v), compiled alone with whatever it itself
instantiates.

Subject: printed p.289's Appendix B register-map rows for POT0DAT, POT1DAT,
POTGOR, SERDATR, SERDAT, SERPER and POTGO, each row's CHIP column composed
with printed p.287's chip-letter legend, predicting that the register lives
inside the Paula chip.

The package carries the quotations in `ch8-appb-chip-column-paula.jsonl` (14
claims, 7 register rows x 2 readers each). Every claim is cited by its
`classify.uid` (`<reader>:<id>`), never the bare `id` -- a twin pair that
asserts the identical composed fact is cited together, both uids on the same
graded or declined line.

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

## The composed fact, and why every row is declined

Each row states a CHIP-column letter; printed p.287 states what the letter
means (quoted, per every claim's `classify.source_defines`, as: `A=Agnus
chip, D=Denise chip, P=Paula chip.`). A row and its legend state one fact
BETWEEN them -- the composed fact, "*register* is a Paula register," is what
each claim carries, not the row cell or the legend line read separately.

Printed p.287's CHIP-column legend reads only:

    A,D,P
        A=Agnus chip, D=Denise chip, P=Paula chip.

This names a physical 1985 chip; it does not mention a module hierarchy, an
RTL boundary, or an instantiation tree -- those concepts do not appear on
p.287 at all. `paula`, `userio` and `denise` are this open-source FPGA
clone's own engineering partition of the reimplemented logic; nothing on
p.287 or p.289 establishes that this partition tracks the three original
chips' silicon boundaries. So a decode's presence inside the Verilog module
named `paula` is evidence about this clone's source layout, not evidence
that the physical Paula chip answers there -- exactly as its absence is no
evidence either way.

The rule, applied identically in this package and in sibling
`ch8-appb-chip-column-denise`: presence and absence inside this clone's
module hierarchy are the same kind of structural fact, and both are
DECLINED with `record()`, naming the structural fact found and, where the
address is decoded elsewhere, the module that would decide it -- the fixture
is not widened to force an answer. All seven T1-T7 rows (14 claims) are
declined; none is asserted with `expect()`. This is a legitimate, reportable
outcome rather than a defect in the fixture: this suite, bounded to
RTL-module-hierarchy observation, cannot settle any of these fourteen
claims, because the legend's letters assert physical chip identity, which
this FPGA clone's module partitioning is not shown anywhere to track.

What the fixture finds: POT0DAT, POT1DAT, POTGOR and POTGO are decoded
nowhere in `paula`'s own hierarchy (they are decoded in `userio.v`);
SERDATR, SERDAT and SERPER are decoded inside `paula_uart.v`, instantiated
inside `paula`. Both findings are recorded, with the SERDATR/SERDAT/SERPER
measurements (a working, content-carrying UART), and neither is a
CHIP-column verdict.

### T1 -- POT0DAT, address $012

> "POT0DAT      *012   R           P (E)Pot counter pair 0 data (vert,horiz)"
> (printed p.289, `opus:8-appB.c20`)
> "POT0DAT      *012   R           P (E)" (printed p.289, `sol:8-appB.c19`,
> same row, shorter extract)

Composed with printed p.287's legend: POT0DAT is a Paula register.
Declined by the rule above; the record names where $012 is decoded. The
`(E)` mark is out of scope (see above).

Cited by: `opus:8-appB.c20`, `sol:8-appB.c19` (twin, same composed
proposition, both taken).

### T2 -- POT1DAT, address $014

> "POT1DAT      *014   R           P (E)Pot counter pair 1 data (vertfhoriz)"
> (printed p.289, `opus:8-appB.c25`)
> "POT1DAT      *014   R           P (E)" (printed p.289, `sol:8-appB.c24`,
> same row, shorter extract)

Composed with printed p.287's legend: POT1DAT is a Paula register.
Declined by the rule above; the record names where $014 is decoded. The
`(E)` mark is out of scope (see above).

Cited by: `opus:8-appB.c25`, `sol:8-appB.c24` (twin, same composed
proposition, both taken).

### T3 -- POTGOR, address $016

> "POTGOR       *016   R           P    Pot port data read (formerly
> POTINP)" (printed p.289, `opus:8-appB.c30`)
> "POTGOR       *016   R           P" (printed p.289, `sol:8-appB.c29`, same
> row, shorter extract)

Composed with printed p.287's legend: POTGOR is a Paula register.
Declined by the rule above; the record names where $016 is decoded.

Cited by: `opus:8-appB.c30`, `sol:8-appB.c29` (twin, same composed
proposition, both taken).

### T4 -- SERDATR, address $018

> "SERDATR      *018   R           P    Serial port data and status read"
> (printed p.289, `opus:8-appB.c35`)
> "SERDATR      *018   R           P" (printed p.289, `sol:8-appB.c34`, same
> row, shorter extract)

Composed with printed p.287's legend: SERDATR is a Paula register.
Declined by the rule above; the record names where $018 is decoded.

Cited by: `opus:8-appB.c35`, `sol:8-appB.c34` (twin, same composed
proposition, both taken).

### T5 -- SERDAT, address $030

> "SERDAT       *030   W           P    Serial port data and stop bits
> write" (printed p.289, `opus:8-appB.c40`)
> "SERDAT       *030   W           P" (printed p.289, `sol:8-appB.c39`, same
> row, shorter extract)

Composed with printed p.287's legend: SERDAT is a Paula register.
Declined by the rule above; the record names where $030 is decoded.

Cited by: `opus:8-appB.c40`, `sol:8-appB.c39` (twin, same composed
proposition, both taken).

### T6 -- SERPER, address $032

> "SERPER       *032   W           P    Serial port period and control"
> (printed p.289, `opus:8-appB.c45`)
> "SERPER       *032   W           P" (printed p.289, `sol:8-appB.c44`, same
> row, shorter extract)

Composed with printed p.287's legend: SERPER is a Paula register.
Declined by the rule above; the record names where $032 is decoded.

Cited by: `opus:8-appB.c45`, `sol:8-appB.c44` (twin, same composed
proposition, both taken).

### T7 -- POTGO, address $034

> "POTGO        *034   W           P    Pot port data write and start"
> (printed p.289, `opus:8-appB.c50`)
> "POTGO        *034   W           P" (printed p.289, `sol:8-appB.c49`, same
> row, shorter extract)

Composed with printed p.287's legend: POTGO is a Paula register.
Declined by the rule above; the record names where $034 is decoded.

Cited by: `opus:8-appB.c50`, `sol:8-appB.c49` (twin, same composed
proposition, both taken).

## Fixture liveness

Before any register's content is trusted the fixture is shown to deliver a
known, distinct word through one of its own registers -- no claim id, no
manual page, this package's own precondition: A0 round-trips two distinct
SET/CLR-encoded words through paula.v's own ADKCON ($09E) / ADKCONR ($010)
pair.

## Control case

At least one assertion is expected to PASS, on the same measurement basis
as anything under suspicion, so a red result cannot be dismissed as a units
error. No T1-T7 row is graded with `expect()`, so the A0 fixture-liveness
assertions (ADKCON round-trip) are the control: same address-bus protocol
and round-trip-and-compare method as every declined row, and PASS. A
package that declines every claim-bearing row is a legitimate outcome
provided its liveness control stands -- it does here.
