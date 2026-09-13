# Specification: ch7-copper-interrupt-source

Fixture: `agnus_copper + paula_intcontroller`. Subject: printed p.219, COPER
bit 4, "reserved for specifically identifying the Copper as the interrupt
source."

Written from the quotations in `ch7-copper-interrupt-source-spec-stage.jsonl`
alone. No RTL has been opened to produce this document. Every claim is cited
by its `uid` (`<reader>:<id>`), never by the bare `id`, per BUILD-BRIEF.

## Reading "reserved": convention vs. discriminator

The manual's verb is "reserved," and it supports two readings that are not
the same proposition:

1. **Convention (software/documentation level).** The manual is telling the
   programmer how to *interpret* a bit that is already set: "if you see bit 4
   up, that means the Copper asked for it." This is guidance about meaning,
   not a claim about what the hardware does on any given cycle. No stimulus
   and no observation makes a convention true or false — a testbench can no
   more grade "this bit means X" than it can grade a variable's name. This
   half is **not assertable** in any fixture, this one included.

2. **Discriminator (hardware level).** For the convention to be honest, the
   hardware has to make something *true*: bit 4 must actually respond to the
   Copper's interrupt condition, and not to anything else. That itself
   splits by scope:
   - **2a. Within this fixture's boundary** (`agnus_copper` +
     `paula_intcontroller`, and nothing else): when the Copper produces the
     condition under which it asks for an interrupt, bit 4 of the
     interrupt-request register responds, and no other bit of that register
     responds to the same stimulus. This is falsifiable with the two modules
     assigned and is what **P1** below tests.
   - **2b. Across the whole chip**: that *no other* interrupt source
     anywhere in the design — none of which are present in this fixture —
     can ever also set bit 4. Confirming that needs every other candidate
     source's RTL, which is a wider module boundary than the two modules
     assigned to this package. **Not assertable here**; recorded below.

So only 2a is testable at the boundary this package was given. P1 is written
to test exactly that, and no more.

## Testable proposition

### P1 — the Copper's interrupt condition sets bit 4, and only bit 4, of the interrupt-request register

> "this bit has been reserved for specifically identifying the Copper as the
> interrupt source." (printed p.219)

Testable form: when the Copper module produces the condition under which it
requests an interrupt, bit 4 (COPER) of the interrupt-request register
becomes set, and no other bit of that register changes state as a result of
that same stimulus. Both halves of the conjunction are required — a check
that only looked for "some bit changed" would not test identification, and a
check that only looked for "no other bit changed" would pass vacuously if
bit 4 never got set at all.

This proposition does not presuppose what, mechanically, causes the Copper
to produce that condition (an instruction outcome, a strobe, a stored flag —
whichever the RTL turns out to use). That mechanism is RTL knowledge and is
deliberately not decided here; step 2 picks the concrete stimulus that
realizes "the Copper's interrupt condition." The pass/fail criterion above
does not change once that mechanism is known.

Cited by: `sol:7-interrupts.c41` (printed p.219).

Co-cited, denotation only: `opus:7-fig7-4.c15` (Figure 7-4, printed p.221)
reads the row whose fourth column is "COPER" and whose Description cell
reads "graphics coprocessor" — i.e., the figure names COPER as denoting the
same condition (the Copper, described there as the graphics coprocessor)
that P1 tests. The same figure row also carries an Exec software priority
("5") and a hardware priority ("3") for COPER, visible via the crossread
note rather than this claim's own restatement; those two values are a claim
about the CPU interrupt-priority encoder in `paula_intcontroller`, a
distinct proposition from naming (which bit answers for the Copper) and out
of this package's boundary. This package's own claims file corroborates the
split directly: `sol:7-interrupts.c41`'s own classify note says "the
level-3 half of the same printed paragraph is claimed separately by
`7-interrupts.c40` and is testable" — i.e., the priority-level content next
to our claim, on the same printed page, was deliberately carved off into a
different claim, not handed to this package. Only the naming/denotation half
of `opus:7-fig7-4.c15` is folded into P1; the priority-encoder half is
recorded, not asserted (see below).

## Control case

BUILD-BRIEF requires at least one control case expected to PASS on the same
measurement basis as the case under suspicion. This package's control is the
same register read P1 uses, under the state where the Copper's interrupt
condition has not been presented: bit 4 reads clear. Presenting the
condition and observing bit 4 set (P1), against not presenting it and
observing bit 4 clear (control), on the same measurement basis, is also the
fixture-integrity proof BUILD-BRIEF asks for: it shows the path between the
two modules is neither stuck at 0 (the positive case would never go red)
nor stuck at 1 (the control would never pass). Neither half alone would rule
out either failure mode.

## Not turned into assertions

- **The convention half of `sol:7-interrupts.c41`** — that a set bit 4
  *means*, to the programmer, "the Copper did it." Not a runtime behaviour;
  no stimulus/observation pair grades it. Recorded, not asserted.
- **Whole-system exclusivity** — that bit 4 can never be set by any source
  other than the Copper anywhere in the full chip. This package's fixture is
  exactly two modules; confirming exclusivity across the whole design needs
  every other candidate interrupt source's RTL, which is a wider module
  boundary than assigned. Recorded, not asserted.
- **The priority-encoder half of `opus:7-fig7-4.c15`** (Exec software
  priority 5, hardware priority 3 for COPER) — a claim about
  `paula_intcontroller`'s priority-encoding logic, not about which bit
  identifies the Copper. Distinct proposition, out of this package's two
  claims. Recorded, not asserted. (Consistent with `7-interrupts.c40`, the
  neighbouring priority claim on the same printed page, being carved off to
  a different package rather than given to this one.)

## Claims accounted for

Both claims in `ch7-copper-interrupt-source-spec-stage.jsonl` are accounted
for: `sol:7-interrupts.c41` → P1 (discriminator half asserted; convention
half and whole-system-exclusivity half recorded, not asserted).
`opus:7-fig7-4.c15` → co-cited into P1 (naming/denotation half asserted;
priority-encoder half recorded, not asserted).

## Mutation plan (to be executed once the RTL is open)

- **P1**: change what the Copper is asked to do so that its interrupt
  condition does *not* occur (a different, or no, operation), while still
  checking for bit 4 set. Expect the assertion to go red. Revert.
- **Control**: change the stimulus from "condition withheld" to "condition
  presented," while still checking for bit 4 clear. Expect the control to go
  red. Revert.

Both mutations touch only what the Copper is asked to do; the expected
value in each assertion is left untouched, per BUILD-BRIEF.
