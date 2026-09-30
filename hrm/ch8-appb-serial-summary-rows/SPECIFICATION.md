# Specification — `ch8-appb-serial-summary-rows`

Package: `hrm/ch8-appb-serial-summary-rows`
Fixture: `paula`, compiled alone (module and file to be located under
`$(NANOMIG)/src/minimig-aga/` only in step 2 — not yet opened while this
document is written).
Claims file: `hrm-testpipeline/work/ch8-s4-build/ch8-appb-serial-summary-rows.jsonl`
(24 claims)

Subject: Appendix B's address-order register-summary rows for the serial
registers — SERDATR ($018), SERDAT ($030), SERPER ($032) and ADKCONR ($010)
— each row's NAME/ADD/R-W/FUNCTION cells (printed p.289), plus ADKCON bit 11
(UARTBRK), named on both printed p.257 (chapter 8's own bit table) and
printed p.265 (Appendix A's register-bit table).

Written from the `extract.quote` / `restatement` fields of the 24 claims
alone, plus the verbatim text of the composing-partner lines they each imply
(the p.289 column heading and the p.287 R/W legend, both quoted below in
full). No RTL file has been opened to produce this document. Port names,
module names beyond `paula` (already given by the package list, not derived
by reading RTL), concrete stimulus values and `expect()` calls are step-3
material, added only after this file is committed. Every claim is cited by
its `classify.uid` (`<reader>:<id>`), never the bare `id`.

## Claim inventory (all 24)

| # | uid | page | subject | cell |
|---|---|---|---|---|
| 1 | `opus:8-regsum.c2` | 289 | SERDATR | row (reduces to ADD, see below) |
| 2 | `opus:8-regsum.c3` | 289 | SERDATR | ADD |
| 3 | `opus:8-regsum.c4` | 289 | SERDATR | R/W |
| 4 | `opus:8-regsum.c6` | 289 | SERDATR | FUNCTION |
| 5 | `opus:8-regsum.c8` | 289 | SERDAT | row (reduces to ADD, see below) |
| 6 | `opus:8-regsum.c9` | 289 | SERDAT | ADD |
| 7 | `opus:8-regsum.c10` | 289 | SERDAT | R/W |
| 8 | `opus:8-regsum.c12` | 289 | SERDAT | FUNCTION |
| 9 | `opus:8-regsum.c14` | 289 | SERPER | row (reduces to ADD, see below) |
| 10 | `opus:8-regsum.c15` | 289 | SERPER | ADD |
| 11 | `opus:8-regsum.c16` | 289 | SERPER | R/W |
| 12 | `opus:8-regsum.c18` | 289 | SERPER | FUNCTION |
| 13 | `opus:8-regsum.c20` | 289 | ADKCONR | row — **already graded, slice s2** |
| 14 | `opus:8-regsum.c21` | 289 | ADKCONR | ADD — **already graded, slice s2** |
| 15 | `opus:8-regsum.c22` | 289 | ADKCONR | R/W — **already graded, slice s2** |
| 16 | `opus:8-regsum.c24` | 289 | ADKCONR | FUNCTION — **already graded, slice s2** |
| 17 | `opus:8-adkcon.c25` | 265 | ADKCON b11 UARTBRK | location |
| 18 | `opus:8-adkcon.c26` | 265 | ADKCON b11 UARTBRK | behaviour (clears TXD) |
| 19 | `sol:8-adkcon.c6` | 257 | ADKCON b11 UARTBRK | location |
| 20 | `sol:8-adkcon.c8` | 257 | ADKCON b11 UARTBRK | behaviour (transmit pin to 0) |
| 21 | `sol:8-adkcon.c22` | 265 | ADKCON b11 UARTBRK | location |
| 22 | `sol:8-adkcon.c24` | 265 | ADKCON b11 UARTBRK | behaviour ("forces a UART break") |
| 23 | `sol:8-adkcon.c25` | 265 | ADKCON b11 UARTBRK | behaviour (clears TXD) |
| 24 | `sol:8-adkcon.c23` | 265 | ADKCON b11 UARTBRK | location — restored exact duplicate of #17 |

All 24 are accounted for below; none is left unassigned.

## Four claims are already graded elsewhere — not re-graded here

`opus:8-regsum.c20`, `c21`, `c22` and `c24` each carry `classify.note:
"subject of slice s2"` and quote, in their own `classify.evidence`, the four
slice-s2 claims that grade this identical printed row
(`opus:8-appB-adkconr.c1` … `c4`, from
`work/2026-09-15_ch8_s2_classified_all.jsonl`). Tracing each of those four
s2 uids to the accepted package that actually carries it on a graded
`expect()` line (searched across `nanomig-hrm-tests/hrm`, not asserted from
memory):

- **`opus:8-appB-adkconr.c1`** ("ADKCONR sits at offset 010", composing the
  ADD cell with the column heading) and **`opus:8-appB-adkconr.c3`**
  ("ADKCONR is read-only", composing the R/W cell with the p.287 legend) are
  both graded together on ONE `expect()` line, label
  `ADKCONR-ADDRESS-READONLY-AND-CHIP`, in `hrm/ch5-audio-dmacon-adkcon`
  (`test_paula.cpp`).
- **`opus:8-appB-adkconr.c5`** ("ADKCONR's function is the audio, disk
  control register read", composing the FUNCTION cell with the column
  heading) is graded on the `expect()` line labelled `A2-ADKCONR-DISK-READ`
  in `hrm/ch8-adkcon-disk-control-bits` (`test_ch8-adkcon-disk-control-bits.cpp`).

Mapping onto this package's own four claims, by which s2 proposition each
one's own `classify.evidence` reduces to:

| this package's uid | cell | reduces to (s2) | graded in | label |
|---|---|---|---|---|
| `opus:8-regsum.c20` | row | `opus:8-appB-adkconr.c1` (ADD) — c20's own evidence says the gradable content of "there is a row" is that the register exists at the row's address, the same content as the ADD cell | `hrm/ch5-audio-dmacon-adkcon` | `ADKCONR-ADDRESS-READONLY-AND-CHIP` |
| `opus:8-regsum.c21` | ADD | `opus:8-appB-adkconr.c1` | `hrm/ch5-audio-dmacon-adkcon` | `ADKCONR-ADDRESS-READONLY-AND-CHIP` |
| `opus:8-regsum.c22` | R/W | `opus:8-appB-adkconr.c3` | `hrm/ch5-audio-dmacon-adkcon` | `ADKCONR-ADDRESS-READONLY-AND-CHIP` |
| `opus:8-regsum.c24` | FUNCTION | `opus:8-appB-adkconr.c5` | `hrm/ch8-adkcon-disk-control-bits` | `A2-ADKCONR-DISK-READ` |

Each is put on its own `record()` line in the test file, citing the package
and the specific accepted uid, per this slice's own rule against reaching a
different verdict from an accepted one on the same printed row. **None of
the four is re-measured by this package.**

## The composition rule this package is built around

A register-table row and its legend state one fact between them. Neither a
row cell nor its column/legend partner states a complete proposition alone;
the composition of the two does, and the composition is what is graded —
the same practice the two sibling packages named in this slice's task
(`hrm/ch8-appb-serial-register-rows`, `hrm/ch8-appb-chip-column-paula`) both
follow. Each proposition below cites the row uid(s) and its composing
partner.

### The composing partners, quoted in full

**Column heading** (composes with every ADD and every FUNCTION proposition):

> NAME        ADD     R/W   CHIP    FUNCTION
— printed p.289. Names ADD as the address column and FUNCTION as the fifth
column.

**R/W legend** (composes with every direction proposition):

> W=write-only; R=read-only,
— printed p.287. Composed with a row's R/W cell: "\<register\> is
read-only" (cell reads `R`) or "\<register\> is write-only" (cell reads
`W`).

### Two claims per row reduce to one proposition (SERDATR/SERDAT/SERPER)

Each of SERDATR, SERDAT and SERPER carries two claims in this file with
restatements "There is a row for the register X" (`c2`/`c8`/`c14`) and "X's
ADD column gives 0XX" (`c3`/`c9`/`c15`). The crossread on the "row exists"
claims objected that they do not state a behaviour
(`"states_behaviour": "no"`), and classify's own answer — printed verbatim
in each one's `classify.evidence` — is: *"The gradable content is that the
register exists at the row's address, not that the table has a row."* That
is exactly the ADD claim's own content. So `c2`/`c8`/`c14` are graded
**together with** `c3`/`c9`/`c15` on the same assertion, as co-assertors of
one composed fact ("X is at chip-register offset $0XX"), not as a second,
independent proposition. This mirrors how a twin pair (two readers, one
proposition) is cited together elsewhere in this suite — here it is one
reader's own two claim ids collapsing onto one content, and the collapse is
classify's own statement, not this document's invention.

## The "*" mark on every row in this lot — read correctly, not assumed

Every quoted row below carries `*` between the register name and its
address, e.g. `SERDATR      *018`. Printed p.287 defines two marks, and they
are **not interchangeable**:

> `*`    Address not writable by the Copper.
>
> `~`     Address not writable by the Copper unless the "copper danger bit",
> COPCON is set true.

`*` (this lot's mark, at p.287:22) is **unconditional** — no chipset or
COPCON qualifier is printed with it. `~` (p.287:24) is the **conditional**
one. This package's four rows (SERDATR, SERDAT, SERPER, ADKCONR) are all
marked `*`, never `~`. **No claim in this 24-record file restates what the
mark means** (none of the 24 `restatement` fields addresses the `*` at
all — the pattern already established by sibling
`ch8-appb-serial-register-rows` for the same three rows), so nothing here
tests the mark either, and nothing here treats it as conditional. Recorded
here only so the correct reading is on file before any test code is
written, per this package's own task instructions.

## The CHIP column — out of scope, and already handled by a sibling

None of the 24 claims' restatements touch the CHIP column (no claim reads
"SERDATR/SERDAT/SERPER/ADKCONR is in the Paula chip"). That column, for
these same four rows (among others), is `hrm/ch8-appb-chip-column-paula`'s
own subject, and that package declines all of its
CHIP-column claims with `record()`, reasoning that this clone's module
partition (`paula`/`userio`/`denise`) is not shown anywhere to track the
original silicon chip boundaries the p.287 legend actually names. Nothing
in that reasoning is re-opened or re-litigated here; it is simply not this
package's claim to make, since no claim in this file asserts it.

## Testable propositions — SERDATR, SERDAT, SERPER (printed p.289)

### R1 — SERDATR is at chip-register offset $018

> SERDATR      *018   R           P    Serial port data and status read
— `opus:8-regsum.c2` (row), `opus:8-regsum.c3` (ADD cell)

Composed with the column heading: "SERDATR is at chip-register offset
$018." **Testable form:** the fixture answers/decodes address $018 as
SERDATR — the same read-path measurement used for R2's positive half.

Cited by: `opus:8-regsum.c2`, `opus:8-regsum.c3`.

### R2 — SERDATR is read-only

> SERDATR      *018   R           P    Serial port data and status read
— `opus:8-regsum.c4` (R/W cell)

Composed with the R/W legend (`R`=read-only): "SERDATR is read-only."
**Testable form**, per the boundary caveat below:
1. reading $018 returns register content (not a fixed/dead value) — shared
   basis with R1;
2. presenting distinct, known values while addressing $018 changes no
   later read of $018 (nor of any other decoded address).

Cited by: `opus:8-regsum.c4`.

### R3 — SERDATR's function is serial port data and status read

> SERDATR      *018   R           P    Serial port data and status read
— `opus:8-regsum.c6` (FUNCTION cell)

Composed with the column heading: the fifth column names SERDATR's
function as reading serial port data and status together in one word.
**Testable form:** a read at $018 returns a word that carries both a data
component and a status component, and the status component changes with
the UART's observable state (at minimum: distinguishable values before and
after an event the manual elsewhere ties to status, e.g. a transmit).
Concrete bit assignments are not fixed here.

Cited by: `opus:8-regsum.c6`.

### R4 — SERDAT is at chip-register offset $030

> SERDAT       *030   W           P    Serial port data and stop bits write
— `opus:8-regsum.c8` (row), `opus:8-regsum.c9` (ADD cell)

Composed with the column heading: "SERDAT is at chip-register offset $030."
**Testable form:** the fixture accepts a write directed at address $030 and
that write has an observable effect distinguishable from a write to any
other decoded address — the same write-path measurement used for R5's
positive half.

Cited by: `opus:8-regsum.c8`, `opus:8-regsum.c9`.

### R5 — SERDAT is write-only

> SERDAT       *030   W           P    Serial port data and stop bits write
— `opus:8-regsum.c10` (R/W cell)

Composed with the R/W legend (`W`=write-only): "SERDAT is write-only."
**Testable form:**
1. a value written to $030 reaches state, with an observable effect (shared
   basis with R4);
2. reading $030 does not return the value most recently written there.

Cited by: `opus:8-regsum.c10`.

### R6 — SERDAT's function is serial port data and stop bits write

> SERDAT       *030   W           P    Serial port data and stop bits write
— `opus:8-regsum.c12` (FUNCTION cell)

Composed with the column heading: the fifth column names SERDAT's function
as writing both a data payload and stop-bit control in one word.
**Testable form:** distinct words written to $030 produce distinguishably
different transmitted output, and the written word's role includes both a
data payload and a stop-bit component (at minimum: two distinct written
words differing only in the bit the manual elsewhere assigns to stop-bit
control produce a measurably different transmitted frame).

Cited by: `opus:8-regsum.c12`.

### R7 — SERPER is at chip-register offset $032

> SERPER       *032   W           P    Serial port period and control
— `opus:8-regsum.c14` (row), `opus:8-regsum.c15` (ADD cell)

Composed with the column heading: "SERPER is at chip-register offset $032."
**Testable form:** the fixture accepts a write directed at address $032 and
that write has an observable effect distinguishable from a write to any
other decoded address — the same write-path measurement used for R8's
positive half.

Cited by: `opus:8-regsum.c14`, `opus:8-regsum.c15`.

### R8 — SERPER is write-only

> SERPER       *032   W           P    Serial port period and control
— `opus:8-regsum.c16` (R/W cell)

Composed with the R/W legend (`W`=write-only): "SERPER is write-only."
**Testable form:**
1. a value written to $032 reaches state, with an observable effect (shared
   basis with R7);
2. reading $032 does not return the value most recently written there.

Cited by: `opus:8-regsum.c16`.

### R9 — SERPER's function is serial port period and control

> SERPER       *032   W           P    Serial port period and control
— `opus:8-regsum.c18` (FUNCTION cell)

Composed with the column heading: the fifth column names SERPER's function
as controlling the serial port's period. **Testable form:** distinct values
written to $032 change the timing of transmitted bits on a ratio basis
(cycles-to-cycles, dimensionless — no absolute time unit asserted, per
finding candidate S4 fixture fact 2 and this slice's own `not
constructible` rule for absolute frequency/period). At least two distinct
SERPER values must produce two distinct, measurably different bit widths.

Cited by: `opus:8-regsum.c18`.

## Boundary caveat — how direction is graded

`paula` (and, inside it, `paula_uart`, per the neighbouring packages' own
disclosed boundary facts) has no documented read/write strobe distinct from
the address itself for this register set — this package's own task states
the same fixture hazard directly: *"reading a write-only address is a
write"*. Every direction proposition (R2, R5, R8) is therefore graded as
the conjunction actually observable, not the manual's letter asserted
directly, exactly as `ch8-appb-serial-register-rows`' own Boundary caveat
states it:

- **read-only**: the address answers on the data output, **and** no value
  presented while addressing it reaches any state;
- **write-only**: a value presented at the address reaches state (an
  observable effect), **and** the address does not answer with that value —
  or with any written value — on the data output.

Step 2 must confirm that this package's own probe of an address never
pulses the write-latching clock edge unless a write is actually intended
(the technique `hrm/ch8-adkcon-disk-control-bits`'s `Rig::read_reg` already
uses at this same `paula` boundary — read only after this document is
committed, for the build pattern, not for its assertions' content).

## Testable propositions — ADKCON bit 11, UARTBRK (printed p.257 and p.265)

Two pages name this bit, in different words, and this package's own task
instructions require each proposition to name its page rather than
collapsing the two printed sentences into one assertion:

> p.257: `11      UARTBRK  Force the transmit pin to zero.`

> p.265: `11     UARTBRK        Forces a UART break (clears TXD) if true.`

### U1 — ADKCON bit 11 is named/located as UARTBRK (p.265)

> 11     UARTBRK        Forces a UART break (clears TXD) if true.
— `opus:8-adkcon.c25`, `sol:8-adkcon.c22`, `sol:8-adkcon.c23`

`opus:8-adkcon.c25` and `sol:8-adkcon.c22` are the cross-read twin pair for
this proposition (reader/crossread_by mirror each other:
opus-written/sol-checked and sol-written/opus-checked, same page, same
quote, same content — "is named UARTBRK" / "is located at bit 11").
`sol:8-adkcon.c23` is documented in the claims file itself as a **restored
exact duplicate** of `opus:8-adkcon.c25` — same restatement text, same
page, same quote — dropped once by `build_union.py` as an exact duplicate
and restored because a dropped uid has no home. Its own
classify note says its classification is "the one measured for
opus:8-adkcon.c25 ... carried across unchanged." All three therefore belong
on the same line. **Testable form:** ADKCON bit 11 is the bit this
document's other UARTBRK propositions (U2a, U2b) exercise — same bit,
demonstrated by using it and observing the effect the same two pages
describe.

Cited by: `opus:8-adkcon.c25`, `sol:8-adkcon.c22`, `sol:8-adkcon.c23`.

### U2a — UARTBRK, true, forces a UART break (p.265)

> 11     UARTBRK        Forces a UART break (clears TXD) if true.
— `sol:8-adkcon.c24`

**Testable form:** setting ADKCON bit 11 produces an observable effect on
the transmit line consistent with "a UART break" being forced; clearing it
releases that effect.

Cited by: `sol:8-adkcon.c24`.

### U2b — UARTBRK, true, clears TXD (p.265)

> 11     UARTBRK        Forces a UART break (clears TXD) if true.
— `sol:8-adkcon.c25`, `opus:8-adkcon.c26`

`opus:8-adkcon.c26`'s own restatement names both "forces a UART break" and
the p.257 wording as an aside, but its own `classify.evidence` narrows what
is graded explicitly: *"GRADED AGAINST p.265 ('clears TXD'); the chapter's
Table 8-9 wording on p.257 ... is a separate claim against a separate page
and is not resolved here."* That graded content is identical to
`sol:8-adkcon.c25`'s own restatement ("When true, UARTBRK clears TXD."), so
the two are cited together. **Testable form:** setting ADKCON bit 11 drives
the transmit line (TXD) to its cleared level; clearing bit 11 releases it.

Cited by: `sol:8-adkcon.c25`, `opus:8-adkcon.c26`.

### U3 — the UARTBRK field is located at ADKCON bit 11 (p.257)

> 11      UARTBRK  Force the transmit pin to zero.
— `sol:8-adkcon.c6`

**Testable form:** the same bit position U1 established for p.265 is named
again, independently, by p.257's own bit table.

Cited by: `sol:8-adkcon.c6`.

### U4 — UARTBRK forces the transmit pin to zero (p.257)

> 11      UARTBRK  Force the transmit pin to zero.
— `sol:8-adkcon.c8`

**Testable form:** setting ADKCON bit 11 drives the transmit pin to 0;
releasing it lets the pin return to its idle level. Graded against p.257's
own wording, independently of U2a/U2b's p.265 wording.

Cited by: `sol:8-adkcon.c8`.

## Precondition to assert (fixture liveness)

Not sourced from a manual quotation. Before any proposition above is
trusted: (1) the `paula` fixture must be shown to deliver a known, distinct
word through its own ADKCON/ADKCONR pair (a non-claim register this
package does not otherwise use for a graded assertion), proving the
address-in → internal register → data-out path is alive at all; (2) a
SERDAT write must be shown to produce two distinct, correctly-ordered bit
sequences on TXD for two distinct payloads, proving the write-then-transmit
path this package's R3/R6/R9/U2a/U2b assertions depend on actually
delivers a known, distinct value before anything reads one.

## Control case

R1's positive read-answer at $018 and R4's/R7's positive write-effect at
$030/$032 are each other's control on the same measurement basis: R1 shows
the read path is not stuck-at-zero (so R5's/R8's "$030/$032 answer nothing"
is a meaningful no, not a dead bus); R4/R7 show the write path is not
stuck-at-no-effect (so R2's "poison at $018 changes nothing" is a
meaningful no, not a dead input). U1/U3 (two independent pages naming the
same bit) and U2a/U2b/U4 (two independent pages describing the same
observable effect) are each other's control in the same sense — agreement
across two independently-worded pages on the same fixture is itself
evidence the measurement is not a units error.

## On mutation (per proposition group)

Concrete stimulus values, port names and the literal `// MUTATION` lines
are step-3 material, but the shape of each mutation is already visible:

- **R1, R4, R7 (address):** redirect the stimulus's target address to one
  of the other rows' addresses, leaving the expected register-specific
  outcome unchanged. The check must go red.
- **R2, R5, R8 (direction):** redirect the stimulus's target address to a
  row of the opposite documented direction, leaving the expected
  direction-outcome text unchanged. The check must go red.
- **R3, R6, R9 (function):** change the data/period value presented at the
  correct address, leaving the expected qualitative outcome text unchanged
  where possible, or invert which of two stimulus values is expected to
  produce which outcome. The check must go red.
- **U1, U3 (location):** write to a different ADKCON bit than 11, leaving
  the expectation "bit 11 changed" unchanged. The check must go red.
- **U2a, U2b, U4 (behaviour):** same shape as U1/U3 — the wrong bit is
  written while the assertion still expects TXD's forced-low/released
  effect.

## Claims not assigned an assertion

None left unassigned. Twenty of the 24 claims map onto R1–R9 and U1–U4
above (a full accounting: R1=2, R2=1, R3=1, R4=2, R5=1, R6=1, R7=2, R8=1,
R9=1 → 12; U1=3, U2a=1, U2b=2, U3=1, U4=1 → 8; 12+8=20). The remaining four
(`opus:8-regsum.c20`, `c21`, `c22`, `c24`) are declined with `record()`,
citing the accepted package and uid that already grades each — see "Four
claims are already graded elsewhere" above.

## What this specification deliberately does not contain

Port names, signal names and concrete stimulus/expected values —
added only once this file is committed and
the RTL is read for step 2/3. No chip-ownership (CHIP=Paula) proposition
and no proposition about the leading `*` mark are invented to fill a gap;
both are reported above as genuinely out of scope for this claims file, not
silently produced.
