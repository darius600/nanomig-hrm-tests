# SPECIFICATION — ch8-appb-chip-column-denise

## Unit

- Fixture: `denise`. The exact RTL file under `$(NANOMIG)/src/minimig-aga/` and every
  module it instantiates will be identified and named in Step 2, after this
  specification is committed.
- Manual page under test: 289 (Appendix B register listing), composed with the
  CHIP-column legend on page 287.
- Claims file: `ch8-appb-chip-column-denise.jsonl` — 6 claims, 3 distinct
  propositions, each claimed independently by two readers (`opus`, `sol`) who
  mark each other as `twin`.

## Source quotations (verbatim, from the claims file)

Page 289 row quotes:

1. `JOY0DAT      *00A   R        D       Joystick-mouse 0 data (vert,horiz)`
   — `opus:8-appB.c10`
   `JOY0DAT      *00A   R        D` — `sol:8-appB.c9` (twin of `opus:8-appB.c10`)
2. `JOY1DAT      *00C   R        D       Joystick-mouse 1 data (vert,horiz)`
   — `opus:8-appB.c15`
   `JOY1DAT      *00C   R        D` — `sol:8-appB.c14` (twin of `opus:8-appB.c15`)
3. `JOYTEST      *036   W        D       Write to all four joystick-mouse counters at once`
   — `opus:8-appB.c55`
   `JOYTEST      *036   W        D` — `sol:8-appB.c54` (twin of `opus:8-appB.c55`)

Page 287 legend quote (embedded verbatim inside each claim's
`classify.source_defines` field, which every one of the 6 claims reproduces
identically):

    A=Agnus chip, D=Denise chip, P=Paula chip.

## Composed propositions (row × legend — the fact that gets graded)

Per this task's carried-forward guidance, a row and its legend state one fact
*between* them; the composed fact is what gets graded, not the row cell alone.

- **P1** — "JOY0DAT is a Denise-chip register": the CHIP column marks JOY0DAT
  (offset `$00A`, access R) with the letter `D`; the legend defines `D` as
  "Denise chip". Composed: the Denise chip answers at register offset `$00A`.
  Claims: `opus:8-appB.c10`, `sol:8-appB.c9`.
- **P2** — "JOY1DAT is a Denise-chip register": CHIP column marks JOY1DAT
  (offset `$00C`, access R) `D` → the Denise chip answers at `$00C`.
  Claims: `opus:8-appB.c15`, `sol:8-appB.c14`.
- **P3** — "JOYTEST is a Denise-chip register": CHIP column marks JOYTEST
  (offset `$036`, access W) `D` → the Denise chip answers at `$036`.
  Claims: `opus:8-appB.c55`, `sol:8-appB.c54`.

A CHIP-column letter is a claim about which chip answers at an address, so
each composed proposition above predicts that the `denise` module — compiled
alone, with no other custom-chip module present in the fixture — claims
(decodes/answers) the stated offset.

## Test approach

Whether P1–P3 are testable *as a stimulus/response check inside this fixture*
depends on two structural facts I do not yet know, because I have not opened
the RTL:

1. whether `denise` exposes an explicit "this address is mine"
   chip-select/acknowledge output distinct from its general data-out bus, or
   whether every input address — recognized or not — reads back through the
   same combined data path with no distinguishing signal; and
2. where `$00A`, `$00C` and `$036` actually decode in this RTL: inside the
   `denise` fixture's own hierarchy, or in a module outside it.

I will not guess a port list to answer these before writing this
specification — that is exactly how an expected value starts life as a fact
about the implementation. Both are Step-2 questions.

Per this task's carried-forward guidance, a sibling package
(`ch8-appb-chip-column-paula`) has already faced the identical structural
situation on this identical manual page: several of its CHIP-column rows name
registers that this RTL decodes in `userio.v`, a module `minimig.v`
instantiates as a *sibling* of the chip module under test, not inside it.
That package's accepted resolution was a **boundary decline**: name the
module that actually owns the address, decline the claim with `record()`,
and do neither of the two alternatives — do not widen the fixture to include
the sibling module, and do not grade the claim `expect()`-red as though this
were a manual-vs-RTL disagreement about a value at an address genuinely
inside the fixture's own boundary.

I will apply the same criterion here in Step 2, verified independently
against the RTL myself — not assumed from the claims file's
`classify.evidence` field, which is a prior reader's classification argument
for where a claim belongs, not settled fact for this build:

- If independent inspection shows `denise.v` (or a module it itself
  instantiates) contains `$00A` / `$00C` / `$036` in its own address-decode
  table → grade the corresponding proposition with `expect()`, oracle = the
  composed proposition above, pages 289 and 287.
- If an address is not decoded anywhere in the `denise` fixture's own
  hierarchy, and is instead owned by a module `minimig.v` instantiates
  beside it (e.g. `userio.v`) → decline the corresponding claim(s) with
  `record()`, naming the module that actually owns it, following the
  accepted `ch8-appb-chip-column-paula` shape. This reports a boundary fact
  about the fixture's scope, not a finding about the manual or about the
  RTL's correctness.
- Any other structural picture (partial decode, ownership not determinable
  from the fixture's own hierarchy either way) is a HALT, not a forced choice
  between the two cases above.

## Control / liveness

Independent of P1–P3, the fixture must prove its own data path is alive
before anything built on it is trusted. I will drive at least two distinct,
self-chosen stimulus values into a register that Step-2 structural inspection
confirms *is* decoded inside the `denise` fixture, and assert the two values
arrive as distinguishable observations. This is an infrastructure self-test,
not a manual claim: it carries no uid and no page citation. It doubles as
this run's control case (exercised on the same bus/measurement mechanism as
everything else in the package) and as the unconditional "known word
delivered" proof this task requires. The exact register and observable
signal are named in Step 2, once the port list is known.

## Mutation plan

Every `expect()`-graded assertion in the finished package — the
liveness/control check(s), and any of P1–P3 that end up graded rather than
declined — gets one stimulus mutation recorded in the file header: change
what the fixture is *asked to do*, leave the expected value alone, confirm
the assertion goes red, then revert. `record()` declines are not assertions
and get no mutation entry. Exact mutation mechanics are named in Step 2.

## Symmetric rule for presence and absence

The "Test approach" section above committed, before the RTL was opened, to a
two-branch rule: grade a *present* decode with `expect()`, decline an
*absent* one with `record()`. In practice only the absent branch ever fired
in this package — JOY0DAT/JOY1DAT/JOYTEST are decoded nowhere in `denise`'s
own hierarchy, confirmed independently in Step 2 — so this package's own
dispositions (all three declined) were never wrong. But the *rule itself*,
as stated above, was asymmetric: sibling
package `ch8-appb-chip-column-paula` did hit the present branch and, under
the identical rule, graded three of its own rows `expect()`-green on
structural presence alone while declining the rest on structural absence —
treating the same kind of evidence as proof one way and as inconclusive the
other.

Printed p.287 settles which reading is correct. Its CHIP-column legend reads
only:

    A,D,P
        A=Agnus chip, D=Denise chip, P=Paula chip.

This names a physical 1985 chip; it does not mention a module hierarchy, an
RTL boundary, or an instantiation tree — those concepts do not appear on
p.287 at all. Nothing on p.287 or p.289 establishes that this FPGA clone's
`denise`/`paula`/`userio` module partition tracks the three original chips'
silicon boundaries. So a decode's presence inside the Verilog module sharing
a chip's name is not, by the legend's own words, evidence that the physical
chip answers there — the same non-evidentiary status this package already
gave (correctly in disposition, though not yet stated as a general rule) to
absence.

The rule, applied identically to both packages: presence and
absence inside this clone's module hierarchy are the same kind of structural
fact, and both are DECLINED with `record()`. The two-branch rule in "Test
approach" above is superseded by this single rule; its first branch (grade
presence with `expect()`) is not entered by either package. See `test_denise.cpp`'s file-header note for the
disposition-level detail, and `ch8-appb-chip-column-paula`'s own note
for where the superseded branch actually fired and was walked
back.
