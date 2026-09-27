# SPECIFICATION: ch8-joytest-counter-write

The package carries the quotations in
`hrm-testpipeline/work/ch8-s1-build/ch8-joytest-counter-write.jsonl` (42
claims, readers `opus` and `sol`, printed page 282).

**Declined cells.** The twelve "no test data" propositions below
(`opus .c11 .c12 .c19 .c20 .c27 .c28 .c35 .c36`, `sol .c6 .c8 .c10 .c12`) are
declined as unspecified, not graded. The claims only say the table defines no
test-data value at those positions -- "undefined" is not "must be ignored",
and the page states no rule for what must happen there, so asserting that
toggling an "xx" write bit must leave the counter unchanged would grade an
invented rule. Pages read for a printed rule that defines them: pp.227-241
(chapter body), the Appendix A preamble (pp.263-264), pp.281-282 (this table
and its neighbour), and pp.287-289 (Appendix B). One relevant rule exists,
p.264: it tells the *programmer* to keep an unused write-only bit clear when
writing, and warns that setting such bits "may cause serious future software
incompatibility if those bits ... are implemented in the future" -- i.e. it
explicitly leaves present hardware behaviour on them open, not fixed at
no-op. No rule found requires an implementation to preserve the counter's
value at these twelve positions. They are recorded as **declined,
unspecified** in the built package.

## Scope

- Register under test: **JOYTEST**, address `$036` (as printed in the quoted
  table line itself — no port list needed to know this).
- In scope: JOYTEST's *semantic write effect* — that one write loads data into
  all four joystick-mouse counter halves at once, and the specific mapping from
  write-data bit to counter bit given by the write-test-data table.
- Out of scope, declined here by design, not by omission: JOYTEST's own address
  and its read/write direction (the `W` column). `ch8-register-map-userio`
  grades JOYTEST's ADDRESS and direction and declines JOYTEST's semantic
  content by name to this package. Nothing here re-grades ADDRESS or
  direction.
- Fixture fact, not derived from the quotations and not an expected value:
  `userio.v` exposes no explicit read/write strobe (`test_load` is a bare
  address match, `userio.v:317`), so a write is driven by holding JOYTEST's
  address for the one active cycle that captures it — never asserted from the
  manual's `W` alone. This affects how the stimulus is driven, not what is
  claimed below.

## Source quotations (page 282)

**Block A** — the register's summary line (`extract.quote` of
`opus:8-appa-joytest.c4` / `sol:8-appa-joytest.c4`):

    JOYTEST    036      W       D     Write to all four joystick-mouse counters
                                      at once.

**Block B** — the write test-data table, `0DAT` row only (`extract.quote` of
`opus:8-appa-joytest.c5`..`c20`):

                     BIT#  15,14,13,12,11,10,09,08  07,06,05,04,03,02,01,00
                           -----------------------  -----------------------
                     0DAT  Y7 Y6 Y5 Y4 Y3 Y2 xx xx  X7 X6 X5 X4 X3 X2 xx xx

**Block C** — the same table, `0DAT` and `1DAT` rows together
(`extract.quote` of `opus:8-appa-joytest.c21`..`c36`; `sol:8-appa-joytest.c5`..
`c12` quote the `0DAT` or `1DAT` row alone, same content):

                     BIT#  15,14,13,12,11,10,09,08  07,06,05,04,03,02,01,00
                           -----------------------  -----------------------
                     0DAT  Y7 Y6 Y5 Y4 Y3 Y2 xx xx  X7 X6 X5 X4 X3 X2 xx xx
                     1DAT  Y7 Y6 Y5 Y4 Y3 Y2 xx xx  X7 X6 X5 X4 X3 X2 xx xx

The "BIT#" scale is what fixes bit position for every cell below it; every
proposition that follows cites it plus the cell it fixes.

## Propositions

Every graded proposition below goes through `expect()` with the
**classify.uid as the label, verbatim** — the ledger reads uids off graded
lines, so each of the 30 graded claims gets its own line, even where several
claims restate the same underlying fact at coarser or finer grain; the twelve
"no test data" claims go through `record()` instead (see above). No
proposition is inferred from RTL line numbers; the RTL line numbers in the
claims file's `classify.evidence` are the readers' own measurements, which
this package treats as context.

### G0 — one write reaches all four counters

Quote: Block A. Page 282.

> Write to all four joystick-mouse counters at once.

Claim: a single JOYTEST write changes JOY0DAT's Y-half, JOY0DAT's X-half,
JOY1DAT's Y-half and JOY1DAT's X-half together — not one counter reached by one
write and the others left alone.

uids graded: `opus:8-appa-joytest.c4`, `sol:8-appa-joytest.c4` (twins — same
proposition, independently graded per reader, per the fixed citation rule).

Test: one JOYTEST write carrying distinct nonzero data in both the Y-field and
the X-field; confirm both JOY0DAT and JOY1DAT differ from their pre-write value
after that *one* write.

### Per-bit data propositions — 0DAT / Y-half, bits 15..10 → Y7..Y2

Quote: Block B / Block C, `0DAT` row, `BIT#` scale. Page 282.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 15 | Y7 | `opus:8-appa-joytest.c5` |
| 14 | Y6 | `opus:8-appa-joytest.c6` |
| 13 | Y5 | `opus:8-appa-joytest.c7` |
| 12 | Y4 | `opus:8-appa-joytest.c8` |
| 11 | Y3 | `opus:8-appa-joytest.c9` |
| 10 | Y2 | `opus:8-appa-joytest.c10` |

Composed range uid: `sol:8-appa-joytest.c5` — "JOYTEST bits 15-10 provide
Y7-Y2 test data for the JOY0DAT Y counter", the same six cells as one
proposition.

Test (per opus bit): one-hot write of that single write-data bit (all other
written bits 0); the corresponding output bit of JOY0DAT's Y-half must reflect
it, and a one-hot write of a *different* bit must leave this output bit
unaffected — i.e. the output bit tracks exactly its mapped input bit and no
other.

Test (sol range): one write carrying a non-repeating pattern across bits 15:10
(so no single stuck-together-field reading could pass by accident); JOY0DAT's
Y-half must read back exactly that pattern.

### Per-bit "no test data" propositions — 0DAT / Y-half, bits 9,8

Quote: Block B / Block C, `0DAT` row, cells under BIT# 09 and 08 (`xx`).
Page 282.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 9 | xx | `opus:8-appa-joytest.c11` |
| 8 | xx | `opus:8-appa-joytest.c12` |

Composed range uid: `sol:8-appa-joytest.c6` — flagged `borderline` by that
reader — "bits 9-8 for the JOY0DAT Y counter as 'xx', without defining
test-data values for them."

`xx` marks a write-data bit the table defines no test datum for. The page
states no required behaviour at such a position, so these claims are
**declined as unspecified** (see the top of this file). The differential
readback — write $0000, then the one-hot bit (for the sol pair, both bits) —
is printed in each `record()` line for the record only; it grades nothing.

### Per-bit data propositions — 0DAT / X-half, bits 7..2 → X7..X2

Quote: Block B / Block C, `0DAT` row, `BIT#` scale. Page 282. Same structure as
the Y-half above.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 7 | X7 | `opus:8-appa-joytest.c13` |
| 6 | X6 | `opus:8-appa-joytest.c14` |
| 5 | X5 | `opus:8-appa-joytest.c15` |
| 4 | X4 | `opus:8-appa-joytest.c16` |
| 3 | X3 | `opus:8-appa-joytest.c17` |
| 2 | X2 | `opus:8-appa-joytest.c18` |

Composed range uid: `sol:8-appa-joytest.c7`.

Test: identical form to the Y-half data propositions, applied to the X-half.

### Per-bit "no test data" propositions — 0DAT / X-half, bits 1,0

Quote: Block B / Block C, `0DAT` row, cells under BIT# 01 and 00 (`xx`).
Page 282.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 1 | xx | `opus:8-appa-joytest.c19` |
| 0 | xx | `opus:8-appa-joytest.c20` |

Composed range uid: `sol:8-appa-joytest.c8` (flagged `borderline`).

Declined as unspecified, as the 0DAT Y-half "no test data" propositions.

### Per-bit data propositions — 1DAT / Y-half, bits 15..10 → Y7..Y2

Quote: Block C, `1DAT` row, `BIT#` scale. Page 282.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 15 | Y7 | `opus:8-appa-joytest.c21` |
| 14 | Y6 | `opus:8-appa-joytest.c22` |
| 13 | Y5 | `opus:8-appa-joytest.c23` |
| 12 | Y4 | `opus:8-appa-joytest.c24` |
| 11 | Y3 | `opus:8-appa-joytest.c25` |
| 10 | Y2 | `opus:8-appa-joytest.c26` |

Composed range uid: `sol:8-appa-joytest.c9`.

Test: same form as the 0DAT Y-half data propositions, applied to JOY1DAT and
graded independently of it — 0DAT and 1DAT results are never assumed to agree.

### Per-bit "no test data" propositions — 1DAT / Y-half, bits 9,8

Quote: Block C, `1DAT` row, cells under BIT# 09 and 08 (`xx`). Page 282.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 9 | xx | `opus:8-appa-joytest.c27` |
| 8 | xx | `opus:8-appa-joytest.c28` |

Composed range uid: `sol:8-appa-joytest.c10` (flagged `borderline`).

Declined as unspecified, as the 0DAT "no test data" propositions; the
JOY1DAT readback is printed independently.

### Per-bit data propositions — 1DAT / X-half, bits 7..2 → X7..X2

Quote: Block C, `1DAT` row, `BIT#` scale. Page 282.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 7 | X7 | `opus:8-appa-joytest.c29` |
| 6 | X6 | `opus:8-appa-joytest.c30` |
| 5 | X5 | `opus:8-appa-joytest.c31` |
| 4 | X4 | `opus:8-appa-joytest.c32` |
| 3 | X3 | `opus:8-appa-joytest.c33` |
| 2 | X2 | `opus:8-appa-joytest.c34` |

Composed range uid: `sol:8-appa-joytest.c11`.

Test: same form as the 1DAT Y-half data propositions.

### Per-bit "no test data" propositions — 1DAT / X-half, bits 1,0

Quote: Block C, `1DAT` row, cells under BIT# 01 and 00 (`xx`). Page 282.

| write bit | table cell | uid (opus, per-bit) |
|---|---|---|
| 1 | xx | `opus:8-appa-joytest.c35` |
| 0 | xx | `opus:8-appa-joytest.c36` |

Composed range uid: `sol:8-appa-joytest.c12` (flagged `borderline`).

Declined as unspecified, as the other "no test data" propositions.

## Claim count check

33 `opus` claims (`c4`..`c36`) + 9 `sol` claims (`c4`..`c12`) = 42, matching the
claims file. Every one is accounted for by exactly one proposition group above:
30 are graded (asserted) and 12 are declined as unspecified (see the
"Declined claims" section). The claims file marks all 42 `status:
"testable"`; the twelve are measurable, but the page states no required
outcome for the measurement.

## Measurement basis, controls, and fixture proof

- **Fixture-delivery proof**, required before any data-dependent proposition is
  trusted: write two different, discriminable data words to JOYTEST and confirm
  the corresponding fields of JOY0DAT and JOY1DAT visibly differ between the two
  writes. This is infrastructure, not a manual claim, and is graded before
  anything else runs (label `fixture-delivers-distinct-words`, no uid).
- **Control basis**: every "carries data" proposition (the Y7..Y2 / X7..X2
  bit and range propositions, all GREEN) is measured on exactly the same
  write-then-read basis as the "no test data" readbacks printed with the
  declines.
- **No assumed symmetry**: 0DAT and 1DAT are graded by entirely separate
  propositions throughout (separate uids, separate table rows). Nothing here
  infers a 1DAT result from a 0DAT measurement or vice versa.

## Declined claims

The twelve "no test data" propositions are declined as unspecified — see the
top of this file. The claim is testable as a measurement (differential
clear-vs-set readback), but the manual states no required outcome for that
measurement, so grading it would assert a requirement the page does not make.
30 of the 42 claims are graded (`G0` x2, the data-bit propositions x24, the
composed data-range propositions x4); the twelve are `RECORDED` in the built
package instead.

## Mutation policy

Every graded proposition gets one stimulus mutation — the write is changed, the
expected value is not. Four `MUT_*` flags each guard a family:

- per-bit data propositions (`MUT_BITSHIFT`): move the one-hot write bit to a
  neighbouring position within its 6-bit field (the assertion must go red,
  since it now checks the wrong bit).
- range propositions (`MUT_RANGEINV`): invert the chosen 6-bit pattern.
- `G0` (`MUT_ADDR`): send the write to a different address (0x00), so the
  single-write-reaches-all-four claim is falsified by construction.
- fixture delivery (`MUT_FIXTURE`): the second write repeats the first word.
- per-bit "no test data" propositions: declined, so no mutation record.
  Rerouting the probe to a neighbouring data bit would only show the
  comparator can fail somewhere, not that a "must stay unchanged" rule is
  real.

The `// MUTATION <uid> ...` lines in the test file's header list every one,
each `stimulus red`.

## Outcome

`make -B test`: 31 passed, 0 failed (30 uid-carrying assertions plus the
fixture-delivery check); 12 `RECORDED` declines.
