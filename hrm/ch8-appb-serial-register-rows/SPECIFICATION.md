# Specification — `ch8-appb-serial-register-rows`

Package: `hrm/ch8-appb-serial-register-rows`
Fixture: `paula_uart` (module `paula_uart.v`, compiled alone)
Claims file: `hrm-testpipeline/work/ch8-s1-build/ch8-appb-serial-register-rows.jsonl` (18 claims)

Subject: Appendix B's address-order rows for SERDATR ($018), SERDAT ($030) and
SERPER ($032) — address, access direction and function, each as a fact
composed from a row cell and its column/legend partner.

This file is written from the `extract.quote` / `restatement` fields of the 18
claims alone, plus the verbatim text of three composing-partner records that
every one of the 18 cites by uid in its own `classify.composes_with` field but
that are not themselves among the 18: `opus:8-appB.c7` (the column-heading
row, page 289) and `opus:8-appB.c3` / `opus:8-appB.c4` (the R/W legend, page
287). Those three were retrieved from the pipeline's classified corpus
(`2026-09-14_ch8_s1_classified_all.jsonl`) so each composition below can be
quoted in full rather than asserted from memory or paraphrase. No RTL file
has been opened to produce this document. Port names, concrete stimulus
values and `expect()` calls are step-3 material, added only after this file
is committed.

## Claim inventory (all 18)

| # | uid | page | register | cell |
|---|---|---|---|---|
| 1 | `opus:8-appB.c33` | 289 | SERDATR | ADD |
| 2 | `sol:8-appB.c32` | 289 | SERDATR | ADD |
| 3 | `opus:8-appB.c34` | 289 | SERDATR | R/W |
| 4 | `sol:8-appB.c33` | 289 | SERDATR | R/W |
| 5 | `opus:8-appB.c36` | 289 | SERDATR | FUNCTION |
| 6 | `sol:8-appB.c35` | 289 | SERDATR | FUNCTION |
| 7 | `opus:8-appB.c38` | 289 | SERDAT | ADD |
| 8 | `sol:8-appB.c37` | 289 | SERDAT | ADD |
| 9 | `opus:8-appB.c39` | 289 | SERDAT | R/W |
| 10 | `sol:8-appB.c38` | 289 | SERDAT | R/W |
| 11 | `opus:8-appB.c41` | 289 | SERDAT | FUNCTION |
| 12 | `sol:8-appB.c40` | 289 | SERDAT | FUNCTION |
| 13 | `opus:8-appB.c43` | 289 | SERPER | ADD |
| 14 | `sol:8-appB.c42` | 289 | SERPER | ADD |
| 15 | `opus:8-appB.c44` | 289 | SERPER | R/W |
| 16 | `sol:8-appB.c43` | 289 | SERPER | R/W |
| 17 | `opus:8-appB.c46` | 289 | SERPER | FUNCTION |
| 18 | `sol:8-appB.c45` | 289 | SERPER | FUNCTION |

Every uid is an `opus`/`sol` twin pair over the same printed row cell (9
propositions × 2 readers = 18). All 18 are accounted for below; none is left
unassigned.

**Chip column, out of scope by the claims file's own shape.** The task's
subject line names "chip" alongside address, direction and function, and
each claim's `extra.chipset` field does record that the row's CHIP column
prints `P` and that page 287 defines `P` as Paula — but no claim in this
18-record file turns that cell into its own testable `restatement`; it rides
along as metadata on the other three propositions, not as a fourth one. No
chip-ownership proposition is invented to fill that gap; it is reported here
as a gap, not silently produced.

**The "*" mark, out of scope for the same reason.** Every quoted row carries
a leading `*` before its address (`SERDATR      *018 …`). No claim's
`restatement` addresses what that mark means, so nothing here tests it either.

## The composition rule this package is built around

Per this package's build brief: "A register-table row and its legend state
one fact between them." Neither a row cell nor its column/legend partner
states a complete proposition alone; the composition of the two does, and the
composition is what is graded — matching the accepted `BPLCON2-WO` practice
in `hrm/ch3-register-map-denise`. Each proposition below cites both the row
uid and its composing-partner uid.

### The composing partners, quoted in full

**Column heading** (composes with every ADD and every FUNCTION proposition):

> NAME        ADD     R/W   CHIP    FUNCTION
— `opus:8-appB.c7`, printed p.289. Classified `status: not-asserted`,
`reason_kind: not-hardware` — "the proposition is about the shape of a table
on a printed page… the testable content sits in the row records." Composed
with a row's ADD cell: "\<register\> is at chip-register offset $0XX."
Composed with a row's FUNCTION cell: the fifth column is what the row's
prose names as the register's function.

**R/W legend** (composes with every direction proposition):

> W=write-only; R=read-only,
— `opus:8-appB.c3` (as `W`) / `opus:8-appB.c4` (as `R`), printed p.287.
Both classified `status: not-asserted`, `reason_kind: not-hardware` — "a
gloss on appendix B's own notation… the hardware propositions live in the
p.289 rows that carry the mark." Composed with a row's R/W cell: "\<register\>
is read-only" (cell reads `R`) or "\<register\> is write-only" (cell reads
`W`).

## Boundary caveat — how direction is graded at this fixture

`paula_uart.v` has no `rd`/`hwr`/`lwr` strobe in its interface, so (per this
package's own task) a read cycle and a write cycle are the same stimulus at
this boundary. Every direction proposition (P2, P5, P8 below) is therefore
graded as the conjunction actually observable, not as the manual's word
asserted directly:

- **read-only**: the address answers on the data output, **and** no value
  presented on the data input at that address reaches any state;
- **write-only**: a value presented on the data input at that address reaches
  state (an observable effect), **and** the address does not answer with
  that value — or with any written value — on the data output.

Both halves are measured for every direction proposition; the assertion text
states what was measured, not "the manual says R" / "the manual says W"
verbatim.

A classification's own scratchpad measurement (recorded in this file's own
`classify.evidence` field, not run by this package) found $018 reading
`$3800` out of reset and $030/$032 reading `$0000`. That is its measurement,
made on a throwaway, uncommitted harness — not this package's. This
package's own fixture must independently show the same shape (or discover
otherwise) before any direction proposition is graded; the number is not
carried over as a given.

## P1 — SERDATR is at chip-register offset $018

> SERDATR      *018
— `sol:8-appB.c32`, printed p.289 (ADD cell)

> SERDATR      *018   R           P    Serial port data and status read
— `opus:8-appB.c33`, printed p.289 (full row, ADD cell read)

Composed with the column heading (`opus:8-appB.c7`): "SERDATR is at
chip-register offset $018."

**Testable form:** the fixture answers/decodes address $018 as SERDATR — the
same read-path measurement used for P2's positive half and P3.

Cited by: `opus:8-appB.c33`, `sol:8-appB.c32`.

## P2 — SERDATR is read-only

> SERDATR      *018   R
— `sol:8-appB.c33`, printed p.289 (R/W cell)

> SERDATR      *018   R           P    Serial port data and status read
— `opus:8-appB.c34`, printed p.289 (full row, R/W cell read)

Composed with the R/W legend (`opus:8-appB.c4`, `R`=read-only): "SERDATR is
read-only." Graded per the Boundary caveat above:

**Testable form:**
1. reading $018 returns register content (not a fixed/dead value) — shared
   basis with P1 and P3;
2. presenting distinct, known values on the data input while addressing
   $018 changes no later read of $018 (nor of any other decoded address).

Cited by: `opus:8-appB.c34`, `sol:8-appB.c33`.

## P3 — SERDATR's function is serial port data and status read

> SERDATR      *018   R           P    Serial port data and status read
— `sol:8-appB.c35` / `opus:8-appB.c36`, printed p.289 (FUNCTION cell)

Composed with the column heading (`opus:8-appB.c7`): the fifth column is
what this prose names as SERDATR's function — reading serial port data and
status together in one word.

**Testable form:** a read at $018 returns a word that carries both a data
component and a status component, and the status component changes with the
UART's observable state (at minimum: distinguishable values before and after
an event that the manual elsewhere ties to status, e.g. a transmit).
Concrete bit assignments are not fixed here — the p.289 row names the
function, not the bit layout — but the composed fact is that this address
returns *this kind* of combined word, not a data-only or status-only one.

Cited by: `opus:8-appB.c36`, `sol:8-appB.c35`.

## P4 — SERDAT is at chip-register offset $030

> SERDAT       *030
— `sol:8-appB.c37`, printed p.289 (ADD cell)

> SERDAT       *030   W           P    Serial port data and stop bits write
— `opus:8-appB.c38`, printed p.289 (full row, ADD cell read)

Composed with the column heading (`opus:8-appB.c7`): "SERDAT is at
chip-register offset $030."

**Testable form:** the fixture accepts a write directed at address $030 and
that write has an observable effect distinguishable from a write to any
other decoded address — the same write-path measurement used for P5's
positive half and P6.

Cited by: `opus:8-appB.c38`, `sol:8-appB.c37`.

## P5 — SERDAT is write-only

> SERDAT       *030   W
— `sol:8-appB.c38`, printed p.289 (R/W cell)

> SERDAT       *030   W           P    Serial port data and stop bits write
— `opus:8-appB.c39`, printed p.289 (full row, R/W cell read)

Composed with the R/W legend (`opus:8-appB.c3`, `W`=write-only): "SERDAT is
write-only." Graded per the Boundary caveat above:

**Testable form:**
1. a value written to $030 reaches state, with an observable effect (shared
   basis with P4 and P6);
2. reading $030 does not return the value most recently written there.

Cited by: `opus:8-appB.c39`, `sol:8-appB.c38`.

## P6 — SERDAT's function is serial port data and stop bits write

> SERDAT       *030   W           P    Serial port data and stop bits write
— `sol:8-appB.c40` / `opus:8-appB.c41`, printed p.289 (FUNCTION cell)

Composed with the column heading (`opus:8-appB.c7`): the fifth column names
SERDAT's function as writing both a data payload and stop-bit control in one
word.

**Testable form:** distinct words written to $030 produce distinguishably
different transmitted output, and the written word's role includes both a
data payload and a stop-bit component (at minimum: two distinct written
words that differ only in the bits the manual elsewhere assigns to "stop
bits" produce a measurably different transmitted frame, not merely a
different data payload).

Cited by: `opus:8-appB.c41`, `sol:8-appB.c40`.

## P7 — SERPER is at chip-register offset $032

> SERPER       *032
— `sol:8-appB.c42`, printed p.289 (ADD cell)

> SERPER       *032   W           P    Serial port period and control
— `opus:8-appB.c43`, printed p.289 (full row, ADD cell read)

Composed with the column heading (`opus:8-appB.c7`): "SERPER is at
chip-register offset $032."

**Testable form:** the fixture accepts a write directed at address $032 and
that write has an observable effect distinguishable from a write to any
other decoded address — the same write-path measurement used for P8's
positive half and P9.

Cited by: `opus:8-appB.c43`, `sol:8-appB.c42`.

## P8 — SERPER is write-only

> SERPER       *032   W
— `sol:8-appB.c43`, printed p.289 (R/W cell)

> SERPER       *032   W           P    Serial port period and control
— `opus:8-appB.c44`, printed p.289 (full row, R/W cell read)

Composed with the R/W legend (`opus:8-appB.c3`, `W`=write-only): "SERPER is
write-only." Graded per the Boundary caveat above:

**Testable form:**
1. a value written to $032 reaches state, with an observable effect (shared
   basis with P7 and P9);
2. reading $032 does not return the value most recently written there.

Cited by: `opus:8-appB.c44`, `sol:8-appB.c43`.

## P9 — SERPER's function is serial port period and control

> SERPER       *032   W           P    Serial port period and control
— `sol:8-appB.c45` / `opus:8-appB.c46`, printed p.289 (FUNCTION cell)

Composed with the column heading (`opus:8-appB.c7`): the fifth column names
SERPER's function as controlling the serial port's period.

**Testable form:** distinct values written to $032 change the timing of
transmitted bits on a ratio basis (cycles-to-cycles, dimensionless — no
absolute time unit is asserted, matching the claims file's own
`ratio-or-absolute` gate on this cell). At least two distinct SERPER values
must produce two distinct, measurably different bit widths.

Cited by: `opus:8-appB.c46`, `sol:8-appB.c45`.

## Precondition to assert (fixture liveness)

Not sourced from a manual quotation — the fixture-delivers-a-known-word rule
applied to this fixture. Before any of P1–P9 is trusted, the fixture must be
shown to deliver distinct, known words at the addresses and cycles they were
driven: at minimum, that a read at a decoded address returns *something*
other than a stuck value, and that a write at a decoded address produces
*some* distinguishable observable effect. Without this, P2's and P5/P8's
"no effect" / "no answer" halves would be indistinguishable from a dead data
path — the chapter-6 lesson this build brief names directly.

## Control case

P1/P2's read-path measurement (SERDATR answers reads, is unaffected by
writes) and P4/P5's + P7/P8's write-path measurement (SERDAT/SERPER accept
writes with an effect, do not answer reads) are run on the identical
measurement basis — "does this address answer a read" / "does a write at
this address change state" — applied to a register with a known-opposite
documented direction. SERDATR's positive read-answer is the control that
shows the rig's read path is not stuck (so SERDAT's/SERPER's failure to
answer at their own addresses is a meaningful "no," not a dead bus); SERDAT's
and SERPER's positive write-effect is the control that shows the rig's write
path is not stuck (so SERDATR's failure to change under a write is a
meaningful "no," not a dead input). No fourth, throwaway register is needed
for this — the three documented rows already span both directions.

## On mutation (per proposition group)

Concrete stimulus values, port names and the literal `// MUTATION` lines are
step-3 material and are not fixed here, but the shape of each mutation is
already visible from the claims alone:

- **P1, P4, P7 (address):** redirect the stimulus's target address to one of
  the other two rows' addresses, leaving the expected register-specific
  outcome unchanged. The check must go red, because that address does not
  carry the row under test.
- **P2, P5, P8 (direction):** redirect the stimulus's target address to a
  row whose documented direction is the opposite one (SERDATR vs.
  {SERDAT, SERPER}), leaving the expected direction-outcome text unchanged.
  The check must go red, because the swapped-in address does not have that
  direction.
- **P3, P6, P9 (function):** change the data/period value presented at the
  correct address (a stimulus mutation on the same address), leaving the
  expected qualitative outcome text unchanged where possible, or — where the
  proposition is itself about distinguishing two stimuli (P6, P9) — invert
  which of two stimulus values is expected to produce which outcome. The
  check must go red because the fixture's actual behavior no longer matches
  the swapped expectation.

## Claims not assigned an assertion

None. All 18 claims map onto P1–P9 above (2 uids each, 9 × 2 = 18).

## What this specification deliberately does not contain

Port names, signal names and concrete stimulus/expected values, per the
build brief's order of work — added only once this file is committed and the
RTL is read for step 3. A chip-ownership (CHIP=Paula) proposition and a
proposition about the leading "*" mark are also absent by design: no claim
in this 18-record file restates either as its own testable fact (see the
"out of scope" notes under Claim inventory above).
