# Specification — `ch8-ciaapra-disk-sense-bits`

Package: `hrm/ch8-ciaapra-disk-sense-bits`
Fixture: `ciaa` (compiled alone, with its CIA submodules `cia_int.v`,
`cia_timera.v`, `cia_timerb.v`, `cia_timerd.v` — the same source list
`hrm/ch8-ciaa-port-direction`'s Makefile uses; confirmed by reading that
Makefile only, per the build task's own instruction to do exactly that). No
RTL content file (`ciaa.v` or any submodule) has been opened for the purpose
of drafting this document. See the disclosure at the end of this file for
exactly what else was read before it was committed, and why.
Claims file: `hrm-testpipeline/work/ch8-s2-build/ch8-ciaapra-disk-sense-bits.jsonl`
(10 claims)
Subject: printed page 244, Table 8-5's four `PA`-labelled rows of CIAAPRA
($BFE001) — PA5 DSKRDY*, PA4 DSKTRACK0*, PA3 DSKPROT*, PA2 DSKCHANGE* — and
the table's own opening sentence that bits labelled PA are inputs. What this
package grades is those four bits' **positions and their direction as
inputs**. It does not grade which drive signal each position is named for,
what a low level on any of them physically means, or DSKCHANGE*'s retention
rule — see below for why each is out of this fixture's reach.

This file is written from the ten claims' `extract.quote` / `classify.restatement`
fields, cross-read against the primary source text for page 244
(`hrm-testpipeline/docs/references/amiga-hrm-3rd/ch8/p244.txt`, matching
`.../text/p244.txt`). Nothing below names a Verilog port, signal, register-
select encoding, or file:line location.

## What this fixture can and cannot carry — the boundary this specification is built around

The build task that assigned this package already states, as prior
measurement rather than something to rediscover: `ciaa` is a **transparent
buffer** for bits 7:2 of a CIAAPRA read — neither a DDRA write nor a PRA
write alters them, and the fixture does **not** latch any of them, including
PA2. It also states that `ciaa`'s ports are generic 8520 port pins with no
signal named `_ready`, `_track0`, `_wprot` or `_change` by name, and that the
binding of a bit **position** to one of those drive-signal **names** is a
fact one level above `ciaa`, at `minimig.v:622` — not exercised here. And it
states that the CIA address decode is not a full compare, so grading where on
the bus CIAAPRA answers is an address-**window** claim that a sibling
package, `ch8-ciaapra-address-window`, already owns; none of this package's
ten claims restate a bus address, so that boundary does not otherwise
constrain anything below, but is noted for completeness.

Given that boundary, a claim of the shape "bit PAn is named X, and reports Y"
splits into parts this fixture can and cannot carry:

- **Bit position and its being an input** — that a CPU read of CIAAPRA at a
  given position reflects an externally-driven pin, and that no write
  through either the direction register or the data register changes what
  that position reports — is `ciaa`'s own, directly measurable behaviour.
- **Which drive signal (DSKRDY*, DSKTRACK0*, DSKPROT*, DSKCHANGE*) sits at
  that position** is the `minimig.v:622` wiring fact above, out of reach at
  this boundary.
- **What a low level physically means for the drive** — motor at speed,
  heads at track zero, write-protect tab, media presence — is downstream of
  `ciaa` entirely (at `paula_floppy`, the drive model, or not modelled at
  all) and is not this fixture's to confirm or deny.
- **DSKCHANGE*'s retention rule** ("[the signal] remains low until a disk is
  inserted AND a step pulse is received") needs a latch. `ciaa` measurably
  has none for this bit; where a latch exists, the build task says it is in
  `paula_floppy`, not here. A claim whose quotation carries this sentence is
  therefore not gradable at this boundary for that half, regardless of
  whether `ciaa`'s answer would be red or green — there is no latch here to
  measure either way.

Per the build task's explicit instruction, this specification grades the
first bullet and declines the rest, naming the reason, rather than letting a
passing position/direction result stand in for a claim this fixture cannot
test.

## Claim inventory (all 10)

| # | uid | reader | page | content | assigned |
|---|---|---|---|---|---|
| 1 | `opus:8-cia.c2` | opus | 244 | "Bits labeled PA are input bits in CIAAPRA ($BFE001)." | **PROP-INPUT** |
| 2 | `sol:8-cia.c1` | sol | 244 | twin of #1 | **PROP-INPUT** |
| 3 | `opus:8-cia.c6` | opus | 244 | PA5 named DSKRDY*, "disk ready", motor-validity clause | DECLINED — name binding |
| 4 | `sol:8-cia.c5` | sol | 244 | PA5, composed: position + name + polarity, twin of #3 | DECLINED — name binding + polarity |
| 5 | `opus:8-cia.c11` | opus | 244 | PA4 named DSKTRACK0*, track-zero detect, step-outward clause | DECLINED — name binding |
| 6 | `sol:8-cia.c9` | sol | 244 | twin of #5 | DECLINED — name binding |
| 7 | `opus:8-cia.c16` | opus | 244 | PA3 named DSKPROT*, "write protected (active low)" | DECLINED — name binding + polarity |
| 8 | `sol:8-cia.c16` | sol | 244 | PA3, composed: position + name + polarity, twin of #7 | DECLINED — name binding + polarity |
| 9 | `opus:8-cia.c18` | opus | 244 | PA2 named DSKCHANGE*, "disk removed" **and** the retention sentence | DECLINED — name binding + retention/latch |
| 10 | `sol:8-cia.c17` | sol | 244 | PA2, twin of #9, shorter quote (no retention sentence) | DECLINED — name binding |

2 claims scored, across one proposition (PROP-INPUT); 8 declined, each for
the boundary reasons above.

## PROP-INPUT — CIAAPRA's PA5-PA2 bits are genuinely input bits, not merely currently so

Page 244 (`opus:8-cia.c2`, twin `sol:8-cia.c1`): "Bits labeled PA are input
bits in CIAAPRA ($BFE001)." Table 8-5's own PA-labelled rows are exactly
PA5, PA4, PA3 and PA2 — no other PA position appears anywhere in this table
— so the sentence's scope, as printed, is those four rows. (CIAAPRA's other
bits — PA7/PA6, the fire lines documented elsewhere and owned by
`ch8-ciaapra-fire-buttons`; PA1/PA0, owned elsewhere again per
`ch8-ciaa-port-direction`'s own specification — are not part of Table 8-5
and are not graded here.)

**Testable proposition, restricted to what `ciaa` alone can show:** for each
of PA5, PA4, PA3 and PA2: (a) a CPU read of CIAAPRA at that bit position
reflects the value currently driven at the corresponding external input pin,
distinctly from the other three positions; and (b) that reflection is not
conditional on writes that leave the position configured as an input — a
DIFFERENT position's direction and data registers, written to output, must
not move it; and this position's own data register, written while its own
direction bit stays clear (p.341: a clear DDR bit defines an input, and a PR
read is always the actual pin), must not move it either. "Input bit" is a
meaningful, falsifiable claim — distinct from "an output bit that happens to
currently echo the external value" — only if an attempt to override it
through the write path, without also reconfiguring the position itself as an
output, provably fails to move it. Switching the position's own direction
bit to output as well, with the same external line still held, is a
different case: p.341 says a read is always the actual pin but does not say
which of an external hold and an enabled output then sets that pin, so it is
not graded here. Part (b) is therefore the necessary other half of part (a),
not an independent claim invented here; together they are what "is an input
bit" means for an 8520-style port, short of that undecided case.

Status: testable, at `ciaa`'s own boundary — no address decoder or `minimig`
instantiation is needed for either half.

## On a control case, fixture liveness and mutation

Per this suite's rules: a fixture-liveness proof precedes any data-dependent
grading; at least one control case is expected to PASS on the same
measurement basis; each assertion (fixture-liveness and control included)
carries a recorded stimulus mutation, in the source, in the fixed
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what
changed>` form.

- **Fixture liveness.** Two distinct, known patterns driven onto the input
  pins covering PA5-PA2 must both arrive at a CIAAPRA read and read back
  correctly and distinctly, before PROP-INPUT is graded.
- **Control.** The same per-bit comparator PROP-INPUT sweeps across PA5-PA2
  is instantiated once more at a single position alone, carries no uid, and
  is the measurement-basis control every other assertion in this package is
  implicitly compared against.
- **Mutation, PROP-INPUT.** PROP-INPUT is expected GREEN at baseline (the
  build task's prior measurement already says this fixture is a transparent,
  non-configurable buffer for these bits, and that is exactly what the claim
  states). So the mutation here is the ordinary form: change what is driven
  onto the input pin at one graded position from the value the assertion's
  fixed expectation names to its opposite, leave that expectation alone,
  confirm the assertion goes RED, then revert — not a red-baseline
  substitution proof. Concrete pin names and bit indices are step-2
  material, fixed once the RTL is opened; the shape of the mutation is already visible from the claim.
- **Mutation, control.** The identical bug shape, applied to the control's
  own single position instead of within the four-way sweep.

## Claims not assigned an assertion (8 of 10)

All eight are declined for boundary reasons already stated above, grouped
here rather than repeated eight times.

**Name binding (all eight).** `ciaa` compiled alone has no signal named
`_ready`, `_track0`, `_wprot` or `_change` — only generic 8520 port pins.
The correspondence between a bit position and one of DSKRDY*/DSKTRACK0*/
DSKPROT*/DSKCHANGE* is a wiring fact one level above this fixture
(`minimig.v:622`, per the build task, not independently opened to draft this
document). Testing "PAn is named X" as a single claim would require either
instantiating `minimig` or asserting that one wiring line as a trusted
contract; this package's fixture is `ciaa` alone, so neither is available.
Affects: `opus:8-cia.c6`, `sol:8-cia.c5`, `opus:8-cia.c11`, `sol:8-cia.c9`,
`opus:8-cia.c16`, `sol:8-cia.c16`, `opus:8-cia.c18`, `sol:8-cia.c17`. Each
position's bit-transfer/input-ness is still exercised as one of PROP-INPUT's
four data points — that is the different, byte-position claim
`opus:8-cia.c2`/`sol:8-cia.c1` state, and grading it does not re-grade any of
these eight claims' name-binding content under a different label.

**Polarity ("active low"), where the claim's own quotation carries it.**
`opus:8-cia.c6` ("Disk ready (active low)..."), `sol:8-cia.c5` (composed),
`opus:8-cia.c16` ("...write protected (active low)"), and `sol:8-cia.c16`
(composed) each state or compose a polarity fact — what a LOW level on the
named signal physically means for the drive. `ciaa` is measured to be a
transparent buffer here: it adds no polarity, latch, or interpretation of
its own to these bits. Whatever a low level means is entirely downstream
(named by these claims' own evidence as `paula_floppy.v`), not something
`ciaa` contributes to or could confirm. Not graded here.

**Retention / latch.** `opus:8-cia.c18`'s quotation carries DSKCHANGE*'s
full retention sentence: "It remains low until a disk is inserted AND a step
pulse is received." That needs a latch. `ciaa` is measured to hold none for
this bit — the transparent-buffer finding applies to PA2 specifically, by
name, in the build task. Where a latch for this behaviour exists, the build
task says it is in `paula_floppy`, a different package's boundary. This
package neither has nor needs that latch to grade PA2's position/input-ness,
which is covered as one of PROP-INPUT's four data points; the retention
sentence itself is not graded by any assertion in this file.
`sol:8-cia.c17`'s quotation is shorter and does not carry the retention
sentence, but is declined for the same name-binding reason as the other
seven.

## Disclosure — what was already known, or read, before this file was drafted

This file is written from the ten claims' `extract.quote` / restatement
content, the primary page-244 source text, and the build task's own text
(read first, as instructed). The build task itself already states several
facts that are properly RTL-derived — that `ciaa` is a transparent,
non-latching buffer for bits 7:2, that its ports carry no drive-signal names,
that the name binding is at `minimig.v:622`, and that the CIA address decode
is not a full compare. None of that was discovered by this document; it was
handed over before step 1 began, in material the build task named as
required reading (including a finding-candidates file), and it shapes
only which claims are in scope, not any expected value graded above (every
expected value above — that a position tracks its pin, that no write moves
it — restates the claims' own "input bits" wording, not a number read off
any RTL file).

Separately, and not excused by the above: before drafting this file, and
beyond the Makefile the build task explicitly directs reading, two sibling
packages were opened in full — `hrm/ch8-ciaa-port-direction`'s and
`hrm/ch8-ciabprb-disk-control-bits`'s `SPECIFICATION.md` and test `.cpp` —
while confirming this suite's own build pattern, coding convention, and how
a directly comparable prior package (`ch8-ciabprb-disk-control-bits`,
CIABPRB's disk-**control** bits, the sibling register this same table's PB
row belongs to) handled an identical name-binding boundary decision. That
reading exposed `ciaa.v`- and `ciab.v`-specific RTL facts beyond the
Makefile alone: register-select encoding (`rs` values for PRA/DDRA), a
Harness class's bus-timing protocol, and — already independently stated by
the build task itself, so not first learned here — that PA5-PA2 are never
captured by a write. Disclosed rather than papered over, per the build
task's own instruction for exactly this situation, and following the same
practice `ch8-ciabprb-disk-control-bits` recorded for itself. Two things
limit what that reading could have contaminated: first, every proposition
and expected value in this document was already fixed by the ten claims'
own quotations and the build task's explicit scope statements before that
reading happened, so nothing here was first learned from `ciaa.v` or
`ciab.v`; second, no proposition was narrowed or widened because of anything
either file showed that the build task had not already said. The concrete
port names, bit indices, and mutation implementations are step-2 material
and are written only after this file is committed, as the order of work
requires.
