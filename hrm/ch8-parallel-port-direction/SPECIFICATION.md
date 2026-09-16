# Specification — `ch8-parallel-port-direction`

Package: `hrm/ch8-parallel-port-direction`
Fixture: `ciaa` (compiled alone), top per the build task and the package
list (`work/2026-09-16_ch8_s4_package_list.md`). The exact RTL filename and
its register-select encoding are not named here — per the build brief, that
is step-2 material, opened only after this document is committed.
Claims file: `hrm-testpipeline/work/ch8-s4-build/ch8-parallel-port-direction.jsonl`
(2 claims)
Subject: printed page 259's Interface Hardware paragraph on the parallel
port — the single sentence stating that the general-purpose parallel
interface is bi-directional.

## Order of work — disclosed, not papered over

The build brief requires this document to be written "from the quotations
alone" and committed before the RTL is opened, and says: **"If your package
list or task sheet has done the same [pre-exposed RTL facts] to you, say so
in your report; do not pretend the order held."**

It did not hold cleanly here, for two independent reasons, both outside this
builder's control:

1. **The task instructions that assigned this package already named RTL
   detail** — specific line numbers in `ciaa.v` (`:267`, `:270-279`, `:268`,
   `:282-288`, `:296`, `:114`) and what each line does, before any
   quotation-only document existed.
2. **Both records in the claims file carry a `classify.evidence` field that
   itself quotes RTL.** `extract.quote` (what step 1 is supposed to use) and
   `classify.evidence` (RTL-derived) live in the same JSON record for each
   claim, so reading the file for the former could not avoid seeing the
   latter. Both `opus:8-par.c1` and `sol:8-par.c1`'s `classify.evidence`
   field quotes `ciaa.v`'s port list (`:80-104`), the PRB/DDRB register-select
   assignments (`:137`, `:139`), the commented-out output register (`:267`,
   `:270-279`) and the never-assigned `portb_out` wire (`:114`).

This is the same situation the build brief itself names for slice s3 ("a
package list handed a builder its module's line numbers before the spec was
written, which compromised this rule through no fault of the builder").

**Despite that prior exposure, the proposition below is stated, and its
testability is argued, using only `extract.quote` / `classify.restatement`
text and the primary source page (p.259).** No RTL fact shapes what is
claimed or why it is testable — the RTL is not opened for that purpose until
after this document is committed, and what it was found to do is recorded
separately, in the committed test source, as the step that follows.

## Claim inventory (both)

| # | uid | page | assigned |
|---|---|---|---|
| 1 | `opus:8-par.c1` | 259 | **PROP-1** |
| 2 | `sol:8-par.c1` | 259 | **PROP-1** (twin of #1) |

Both claims' `extract.quote` fields quote the same clause of the same
sentence on p.259; they differ only in how much of the sentence's tail each
reader carried into the quote (`opus` continues into "This connector is
generally used for a parallel printer.", `sol` stops at the first sentence).
Same proposition, same page — graded together as one testable proposition,
carrying both uids on its assertion, per the pattern the sibling package
`hrm/ch8-ciaa-port-direction` already uses for its own twin uids (e.g. its
PROP-2, carrying `opus:8-dio.c46` and `sol:8-dio.c43` on one line).

## PROP-1 — the general-purpose parallel interface is bi-directional

Page 259 (`opus:8-par.c1`):

> The general-purpose bi-directional parallel interface is a 25-pin
> connector on the back panel of the computer. This connector is generally
> used for a parallel printer.

Page 259 (`sol:8-par.c1`, the same clause):

> The general-purpose bi-directional parallel interface is a 25-pin
> connector on the back panel of the computer.

Restatements: "The parallel interface is bi-directional." (opus) / "The
general-purpose parallel interface is bidirectional." (sol)

**Testable proposition, address and pinout stripped out:** at this
fixture's boundary, "bi-directional" means the interface can carry data in a
direction chosen by software — when configured for output, a value written
by software must be what a subsequent read of the same register returns, not
a fixed echo of something outside software's control regardless of what was
written. This is not tested as a claim about signal levels on the physical
25-pin connector (outside this fixture's boundary, and outside the RTL
entirely) but as the boundary-level fact the manual's sentence requires to be
true of *some* register this interface exposes: that it has a write side
that a read side can observe, under a direction setting that says it should.

The manual does not, on this page, name a register or a module. Per the
sibling package's own precedent, concrete register-select values and bit
positions are step-2 material and are not fixed here.

**Status: testable, contingent on step 2 confirming this fixture (`ciaa`
alone) exposes both a data register and a direction register for this
interface at all** — if it does not, that is a HALT to be reported, not
assumed here.

## On a control case and mutation

This package's task instructions name `hrm/ch8-ciaa-port-direction` — chapter
8's sibling package that already grades the identical shape (a
direction-configured data register that must show write-follows-read
behaviour) for the *other* CIA-A port — as this package's control pattern.
That package found the mechanism works for at least one bit position of its
port. Reusing the identical measurement (same fixture, same bus read/write
protocol), pointed instead at that already-working port, is this package's
control case: a red here, measured on the same basis as a demonstrated pass,
rules out "the fixture cannot deliver a known value" as the explanation for
red, per the build brief's requirement for a control on the same measurement
basis as anything under suspicion.

- **Fixture liveness.** Before PROP-1 is graded, the harness must drive at
  least two distinct, known values into a direction register on this
  interface's own side and confirm both arrived and read back distinctly —
  proof this exact build of the fixture can deliver a known value at all,
  before anything data-dependent is trusted.
- **Control case.** The identical write/read-back comparator PROP-1 uses,
  pointed instead at the sibling package's already-working port, expected to
  PASS. Not itself a page claim — the measurement-basis control.
- **Mutation, PROP-1 (stimulus).** Per the build brief, "stimulus green at a
  passing assertion proves nothing" — it is only meaningful at a red
  baseline. This package's task instructions state PROP-1's baseline is
  expected red. Its required mutation is therefore the substitution proof a
  red baseline needs: the identical comparator, retargeted by register-select
  alone (no other code change) at the control's already-working port, built
  as a replayable `-D` variant, must go green — proving the comparator itself
  is not vacuously false.
- **Mutation, control case (stimulus, red).** Omitting the direction-bit
  write the control depends on must turn the control red — proving the
  control is not vacuously true either.
- **Mutation, fixture-liveness (stimulus, red).** Repeating the same stimulus
  word instead of writing a distinct second word must turn liveness red.

Per the build brief: every mutation above is committed as a `MUT ?=` /
`-D$(MUT)=1` Makefile variant with `#if MUT_n / #else / #endif` at its site,
replayable as `make MUT=<NAME> test` — not applied by editing and reverting.

Concrete port names, register-select values, bit positions and the `expect()`
calls themselves are step-2 material, fixed only once the RTL is opened for
that step.

## What this specification deliberately does not contain

Port names, register-select values, bit positions, and concrete stimulus
values are absent by design, per the build brief's order of work — they are
added only after this document is committed and the RTL is read for step 2.

Nothing above states a fact this document learned from RTL. The prior
exposure disclosed at the top of this document (the task instructions'
line-number citations, and the `classify.evidence` field co-resident with
`extract.quote` in the claims file) was not used to shape PROP-1's wording,
its testability argument, or its scope — both rest solely on `extract.quote`,
`extract.page`, the restatements, and the primary p.259 source text. This is
disclosed rather than concealed, following the same practice the sibling
package `hrm/ch8-ciaa-port-direction` recorded for itself, and per the build
brief's own instruction not to pretend the order held when it did not.
