# Specification — `ch7-overlay-source`

Package: `hrm/ch7-overlay-source`
Fixture: `minimig`
Claims file: `hrm-testpipeline/work/ch7-build/ch7-overlay-source-spec-stage.jsonl` (5 claims)
Subject: where the overlay signal comes from — printed p.223's account of it
being entered by reset and turned off by a CIA write.

This file fixes the printed propositions under test, from the `quote` /
`restatement` fields of the 5 claims in `ch7-overlay-source.jsonl` alone.
Concrete port names, signal names, stimulus values and the `expect()`
assertions themselves live in `test_minimig.cpp`.

## Claim inventory (all 5, grouped by proposition)

| # | uid | page | role |
|---|---|---|---|
| 1 | `sol:7-reset.c3`  | 223 | PROP-A, sole claim |
| 2 | `sol:7-reset.c8`  | 223 | PROP-B, primary |
| 3 | `opus:7-reset.c4` | 223 | PROP-B, twin of #2 |
| 4 | `sol:7-reset.c12` | 223 | PROP-C, primary |
| 5 | `opus:7-reset.c9` | 223 | PROP-C, twin of #4 |

All 5 claims are accounted for; none is left unassigned.

## PROP-A — Power-on / external reset enters the overlay memory-map state

Page 223.

> When the Amiga is turned on or externally reset, the memory map is in a
> special state.
— `sol:7-reset.c3`

**Proposition to test:** on power-on or external reset, the memory map is
in the special (overlaid) state described by the chapter — i.e. reset is
what *sets* the overlay condition. No twin claim reached this sentence from
the other reader in this claims file; it stands on its own single citation.

Status: testable.

## PROP-B — The OVL bit in an 8520 CIA disables (clears) the overlay

Page 223.

> The OVL bit in one of the 8520 Chips
> disables the overlay
— `sol:7-reset.c8`

> The OVL bit in one of the 8520 Chips
> disables the overlay (See  Appendix F  for the bit location).
— `opus:7-reset.c4` (twin of `sol:7-reset.c8`; opus's own crossread flagged
its shorter quotation as `shortened-quotation` and the restored quote above
adds the Appendix-F cross-reference. Same proposition, not double-counted.)

**Split:** the restored parenthetical "(See Appendix F for the bit
location)" is a cross-reference to a different appendix for the bit's
position within the CIA, not part of the behavioural claim itself. Appendix
F is not in this claims file and is excluded from the assertion; the
proposition under test is the behaviour ("the OVL bit disables/clears the
overlay"), not the bit's numeric position within the chip.

**Wording note:** both quotations name "one of the 8520 Chips" without
saying which — neither says CIA-A. This specification does not assert
CIA-A; that identification is not supported by either quotation in this
claims file. Which CIA instance carries the OVL bit, and its port/signal
name, is identified in the fixture (`test_minimig.cpp`'s `CIAA_PRA` /
`CIAA_DDRA`), not settled by this claims-derived specification.

**Precondition:** a write that "disables" the overlay is only a
meaningful, checkable transition if the overlay was previously enabled
(set) — that precondition is exactly PROP-A's (and PROP-C's) proposition.
The test asserts that the overlay was set before the CIA write, not only
that it reads clear afterward.

Status: testable (the specific CIA instance and port are identified in the
fixture; the CIA-A identity is not asserted by this specification).

## PROP-C — Invoking the 68000 RESET (line) has the same effect as external reset or power-on

Page 223.

> The 68000 RESET instruction works much like external reset or power on.
— `sol:7-reset.c12`

> The 68000 RESET instruction works much like external reset or power on.
— `opus:7-reset.c9` (twin of `sol:7-reset.c12`; identical quote, crossread
`ok`)

**Split**, carried from both claims' own `split` field, worded identically
by both readers: nothing in any fixture decodes a 680x0 RESET opcode, so
"the RESET instruction" as distinct from "an assertion of the reset line"
is not-hardware. The proposition must be worded as the reset **line**, not
the CPU instruction that (outside this fixture) would assert it. Wording
the assertion as "the instruction" would be exactly the oracle-stretch
`BUILD-BRIEF.md` warns against, and it would happen here, in the
specification, not in the test.

**Proposition to test:** asserting the reset line produces the same
overlay-related effect as external reset or power-on (PROP-A) — i.e. the
mechanism that sets the overlay is uniform across reset sources, at the
reset-line level. This package does not test CPU instruction decode.

Status: testable, worded as the reset line rather than the CPU instruction.

## Scope note: PROP-A vs PROP-C

PROP-A and PROP-C are kept as two separate assertions with two separate
citations, even though they may end up measured the same way at the
fixture boundary (a single reset-line stimulus): PROP-A is grounded in the
power-on/external-reset sentence, PROP-C in the
RESET-instruction-works-like-external-reset-or-power-on sentence.
Collapsing them into one assertion would under-cite the manual. In the
fixture both end up driving the same pin (`_cpu_reset_in`), but they are
not interchangeable measurements: PROP-A's transition is confounded with
the internal power-on reset chain and is measured, not graded (see
`test_minimig.cpp`'s CLAIM COVERAGE note); PROP-C isolates the same pin
after that chain has already released, and is graded as
RESET-LINE-RESTORES-OVERLAY.

## Claims not assigned an assertion

None. All 5 claims are classified `testable` in the claims file and each is
assigned to exactly one of the three propositions above (PROP-A: claim 1;
PROP-B: claims 2, 3; PROP-C: claims 4, 5 — see the inventory table for the
exact uid-to-proposition mapping).

## What this specification deliberately does not contain

Port names, signal names, which of the two 8520 CIAs carries the OVL bit,
and concrete stimulus values are absent by design: this file fixes the
printed propositions the claims license, independent of the RTL's own
naming. PROP-A, PROP-B and PROP-C above are the three propositions that
become `test_minimig.cpp`'s `expect()` assertions, each with its own
control case and stimulus mutation.
