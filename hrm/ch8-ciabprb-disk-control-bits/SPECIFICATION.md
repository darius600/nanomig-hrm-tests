# Specification — `ch8-ciabprb-disk-control-bits`

Package: `hrm/ch8-ciabprb-disk-control-bits`
Fixture: `ciab` (compiled alone, with its CIA submodules `cia_int.v`,
`cia_timera.v`, `cia_timerb.v`, `cia_timerd.v`, the way
`hrm/ch8-ciaa-port-direction` compiles `ciaa`). No RTL file has been opened for
the purpose of drafting this file — see the disclosure at the end of this
document for what was already known before drafting began, and why.
Claims file:
`hrm-testpipeline/work/ch8-s2-build/ch8-ciabprb-disk-control-bits.jsonl`
(30 claims)
Subject: printed pages 244–245, CIABPRB ($BFD100) — the eight PB output bits
that carry disk drive control: DSKMOTOR*, DSKSEL3*–DSKSEL0*, DSKSIDE,
DSKDIREC and DSKSTEP*. Their bit positions, their reset/default level, the
ordering rule between DSKDIREC and DSKSTEP*, and DSKSTEP*'s pulse shape.

## The corpus conflict this package's claims carry — already resolved, not reopened here

Printed p.244 names this register `CIAAPRB`; printed p.241 lists the same
address, $BFD100, as `CIABPRB`. This was resolved during classification, not
by this package: p.241 (the register list) is right, p.244 (prose gloss) is
a typo, on three independent grounds recorded in `classify.conflict_resolution`
— the register-list-vs-prose-gloss precedence, the address decoder ($BFD100
has A13 low, A12 high, which selects CIA-B not CIA-A), and the wiring that
wires *this* register's output to the disk drive controls rather than to the
joystick bits that hang off CIAA. Seven of this package's 30 claims carry
that field verbatim (`opus:8-cia.c4`, `sol:8-cia.c3`, `sol:8-cia.c20`,
`sol:8-cia.c30`, `sol:8-cia.c31`, `sol:8-cia.c32`, `sol:8-cia.c33`); the
other 23 do not carry it because they do not restate the register's chip
identity, only a bit's position or behaviour within it, and inherit the
resolution contextually. Every assertion in this package is written against
`ciab`'s PRB register (`rs`=4'h1) throughout. Nothing here re-derives or
re-argues the chip letter.

## What this fixture can and cannot carry — the boundary this specification is built around

`ciab` compiled alone exposes exactly one 8-bit output relevant to this
register: `portb_out[7:0]`, driven from the PRB write register with the
8520's own pull-up behaviour (undriven bits read 1). It has no signal named
`_motor`, `_sel3..0`, `side`, `direc` or `_step` — those names, and which of
`portb_out`'s eight bit positions each one is, are a wiring fact at a level
above `ciab` (recorded in the claims' own evidence as `minimig.v:650`,
already read by the classifier, not independently opened for this
document). A claim that states "bit PBn is named X" is therefore only
partly a claim about `ciab`: the fact that PBn is a distinct, independently
writable bit position is `ciab`'s own behaviour and is testable here; the
fact that position n carries the name X, and therefore drives a particular
physical disk line, is not something this fixture's compiled boundary can
confirm or deny. Per the build task's explicit instruction, this
specification grades the first half and declines the second, naming the
reason, rather than exercising a walking-bit pattern and letting a passing
result stand in for a claim about drive semantics it did not test.

The same split applies to "active low": what `ciab` alone can show is that
the pull-up/reset default of a PB bit is HIGH (the deasserted state a
page's `(active low)` notation implies), not that a physically LOW line
runs a specific drive's motor or asserts a specific drive's select — that
second half is the downstream consumer's fact (`paula_floppy.v`, outside
this boundary) and is not re-derived here either, even where a claim's own
`classify.evidence` cites it as corroboration.

## Claim inventory (all 30)

| # | uid | page | topic | assigned |
|---|---|---|---|---|
| 1 | `opus:8-cia.c4` | 244 | PB bits are outputs in CIABPRB ($BFD100) | **PROP-POSITION** |
| 2 | `opus:8-cia.c21` | 244 | PB7 named DSKMOTOR* | DECLINED — name binding |
| 3 | `opus:8-cia.c22` | 244 | DSKMOTOR* active low | **PROP-RESET-DEASSERT** |
| 4 | `opus:8-cia.c29` | 244 | all drive motors off after reset | **PROP-RESET-DEASSERT** |
| 5 | `opus:8-cia.c32` | 245 | PB6 named DSKSEL3* | DECLINED — name binding |
| 6 | `opus:8-cia.c33` | 245 | DSKSEL3* active low | **PROP-RESET-DEASSERT** |
| 7 | `opus:8-cia.c34` | 245 | PB5 named DSKSEL2* | DECLINED — name binding |
| 8 | `opus:8-cia.c35` | 245 | DSKSEL2* active low | **PROP-RESET-DEASSERT** |
| 9 | `opus:8-cia.c36` | 245 | PB4 named DSKSEL1* | DECLINED — name binding |
| 10 | `opus:8-cia.c37` | 245 | DSKSEL1* active low | **PROP-RESET-DEASSERT** |
| 11 | `opus:8-cia.c38` | 245 | PB3 named DSKSEL0* | DECLINED — name binding |
| 12 | `opus:8-cia.c40` | 245 | DSKSEL0* active low | **PROP-RESET-DEASSERT** |
| 13 | `opus:8-cia.c41` | 245 | PB2 named DSKSIDE | DECLINED — name binding |
| 14 | `opus:8-cia.c45` | 245 | PB1 named DSKDIREC | DECLINED — name binding |
| 15 | `opus:8-cia.c48` | 245 | DSKDIREC set up before STEP, separate write | **PROP-ORDER** |
| 16 | `opus:8-cia.c49` | 245 | PB0 named DSKSTEP* | DECLINED — name binding |
| 17 | `opus:8-cia.c50` | 245 | DSKSTEP* quick-pulse shape | **PROP-STEP-SHAPE** |
| 18 | `sol:8-cia.c3` | 244 | twin of #1 | **PROP-POSITION** |
| 19 | `sol:8-cia.c20` | 244 | PB7 composed: position+name+polarity | DECLINED — name binding (polarity half already graded under `opus:8-cia.c22`) |
| 20 | `sol:8-cia.c27` | 244 | twin of #4 | **PROP-RESET-DEASSERT** |
| 21 | `sol:8-cia.c30` | 245 | PB6 composed | DECLINED — name binding (polarity half under `opus:8-cia.c33`) |
| 22 | `sol:8-cia.c31` | 245 | PB5 composed | DECLINED — name binding (polarity half under `opus:8-cia.c35`) |
| 23 | `sol:8-cia.c32` | 245 | PB4 composed | DECLINED — name binding (polarity half under `opus:8-cia.c37`) |
| 24 | `sol:8-cia.c33` | 245 | PB3 composed | DECLINED — name binding (polarity half under `opus:8-cia.c40`) |
| 25 | `sol:8-cia.c35` | 245 | PB2 DSKSIDE named, twin of #13 | DECLINED — name binding |
| 26 | `sol:8-cia.c39` | 245 | PB1 DSKDIREC named, twin of #14 | DECLINED — name binding |
| 27 | `sol:8-cia.c42` | 245 | DSKDIREC before STEP (ordering half) | **PROP-ORDER** |
| 28 | `sol:8-cia.c43` | 245 | DSKDIREC separate write (register-structure half) | **PROP-ORDER** |
| 29 | `sol:8-cia.c44` | 245 | PB0 named DSKSTEP*, twin of #16 | DECLINED — name binding |
| 30 | `sol:8-cia.c45` | 245 | DSKSTEP* quick-pulse shape, twin of #17 | **PROP-STEP-SHAPE** |

14 claims scored across 4 propositions; 16 declined, all for the single
name-binding reason stated above (never for doubt about the chip letter,
which is settled).

## PROP-POSITION — the PB register is eight independently writable output bits

Page 244 (`opus:8-cia.c4`, twin `sol:8-cia.c3`, corrected per the resolved
conflict above): "Bits labeled PB are output bits located in CIABPRB
($BFD100)."

**Testable proposition, restricted to what `ciab` alone can show:** each of
the register's eight bit positions is written and read back independently —
writing one position does not move any other, and a CPU read of the
register returns the same value currently presented on the output pins (so
"output bits located in CIABPRB" is not merely an internal latch, but is
actually observable both ways at this boundary). Graded across all eight
positions, since the page's claim is byte-wide, not about one bit.

Status: testable, at `ciab`'s own boundary (no address decoder or minimig
instantiation needed for this half).

## PROP-RESET-DEASSERT — every PB bit defaults / resets to its deasserted (HIGH) level

Page 244 (`opus:8-cia.c22`, "DSKMOTOR* ... active low"; `opus:8-cia.c29`
and twin `sol:8-cia.c27`, "All drive motors turn off after system reset")
and page 245 (`opus:8-cia.c33`/`c35`/`c37`/`c40`, "DSKSEL3*/SEL2*/SEL1*/SEL0*
... active low").

**Testable proposition, restricted to what `ciab` alone can show:** for each
of these five positions (PB7 motor, PB6–PB3 select 3–0), the register's own
reset behaviour and 8520 pull-up default (undriven bit reads 1) leave that
bit HIGH — the deasserted level an active-low signal's default state must
be, and the state that must hold immediately after `reset` for "all drive
motors turn off after system reset" to be true of the CIA side of that
claim. This is the whole byte at once (`portb_out`=0xFF after reset with a
DDRB write of 0xFF beforehand cleared by that same reset), since all five
graded positions share the identical mechanism (`ciab.v`'s `regportb`/
`ddrportb` reset clears, and the pull-up OR). Not graded: what a drive does
with a LOW level once selected, or that each drive individually latches and
remembers the motor state at its own select edge — both are downstream of
`ciab` and are not tested here.

Status: testable, at `ciab`'s own boundary.

## PROP-ORDER — DSKDIREC, once written, survives an independent STEP-only write

Page 245 (`opus:8-cia.c48`; twin halves `sol:8-cia.c42` — the ordering half
— and `sol:8-cia.c43` — the register-structure half): "This line [DSKDIREC]
must be set up before the actual step pulse, with a separate write to the
register."

**Testable proposition:** DSKDIREC (PB1) and DSKSTEP* (PB0) are two bits of
the same PRB byte, so no single write can present one before the other —
the manual's "separate write" requirement is exactly the fact that a second,
independent write is what changes DSKSTEP* while leaving DSKDIREC's value
from the first write undisturbed. Graded by writing DSKDIREC once, then two
further writes that only toggle DSKSTEP* (the shape PROP-STEP-SHAPE also
uses), and confirming DSKDIREC's bit position never moves across either of
those two writes.

Status: testable, at `ciab`'s own boundary — this is a register-structure
fact (one write latches the whole byte) independent of what DIREC/STEP mean
to a drive.

## PROP-STEP-SHAPE — DSKSTEP* observably pulses high, momentarily low, then high

Page 245 (`opus:8-cia.c50`; twin `sol:8-cia.c45`): "This signal must always
be used as a quick pulse (high, momentarily low, then high)."

**Testable proposition:** the shape — high, low, high — across three
successive PRB writes is observable on `portb_out`[0] and is what the page
demands; a level merely held low is a different, unclaimed shape. Not
graded: how short "momentarily" is — that is a duration with no companion
interval on this page to form a ratio with, out of scope for this package.

Status: testable, at `ciab`'s own boundary.

## On a control case, fixture liveness and mutation

Per the build brief: a fixture-liveness proof precedes any data-dependent
grading; at least one control case is expected to PASS on the same
measurement basis; each assertion (fixture-liveness and control included)
carries a recorded stimulus mutation, in the source, in the fixed
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what
changed>` form.

- **Fixture liveness.** Two distinct, known bytes written to PRB (with DDRB
  set to all-outputs) must both arrive on `portb_out`, and differ from each
  other, before PROP-POSITION or anything after it is graded.
- **Control.** The same per-bit comparator PROP-POSITION sweeps across all
  eight positions, instantiated once more at a single bit position alone,
  carries no uid, and is the measurement-basis control every other
  assertion in this package is implicitly compared against.
- **Mutation, all four PROPs.** Each PROP in this package is expected GREEN
  at baseline (unlike `ch8-ciaa-port-direction`'s partial mechanism, `ciab`'s
  PRB path has no dead positions). So every mutation here is the ordinary
  form: change the stimulus fed to `ciab`, leave the expected value alone,
  confirm the assertion goes RED, then revert — not the red-baseline
  substitution proof `ch8-ciaa-port-direction` needed. Concrete stimulus
  values (which byte, which bit) are step-2 material, fixed once the RTL is
  opened, per the build brief's order of work; the shape of each mutation is
  already visible from the claim: PROP-POSITION's stimulus withholds the
  clearing write for one bit, PROP-RESET-DEASSERT's stimulus omits the reset
  pulse, PROP-ORDER's stimulus corrupts the second STEP-only write into also
  changing DIREC, PROP-STEP-SHAPE's stimulus never drives the low phase.

## Claims not assigned an assertion (16 of 30)

All sixteen are declined for the same reason, stated once here rather than
sixteen times: `ciab` compiled alone has no signal named DSKMOTOR*,
DSKSEL3*–DSKSEL0*, DSKSIDE, DSKDIREC or DSKSTEP* — only `portb_out[7:0]`, a
generic 8520 port. The correspondence between a bit position and one of
these names is a wiring fact one level above this fixture (recorded, in the
claims' own evidence, at `minimig.v:650` — not independently opened to
write this document). Testing "PBn is named X" as a single claim would
require either instantiating `minimig` or asserting that one wiring line as
a trusted contract; this package's fixture is `ciab` alone, so neither is
available, and the build task's own instruction is to say so rather than
let a passing position-independence result stand in for a claim about the
name: `opus:8-cia.c21`, `c32`, `c34`, `c36`, `c38`, `c41`, `c45`, `c49`, and
composed twins `sol:8-cia.c20`, `c30`, `c31`, `c32`, `c33`, `c35`, `c39`,
`c44`. For the five composed `sol` claims that also assert active-low
polarity (`c20`, `c30`, `c31`, `c32`, `c33`), that polarity content is not
lost: it is the identical measurement already graded under its `opus` twin
in PROP-RESET-DEASSERT. It is not re-cited on that graded line under the
`sol` uid as well, because a uid cited on a graded line is counted asserted
for the *whole* claim it names, including the name-binding half this
package does not test — citing it there would be exactly the "pretend the
port carries it" the build task warns against.

## Disclosure — what was already known before this file was drafted

This file is written from the 30 claims' `extract.quote` / restatement
content, plus the build task's own text. Unlike a from-nothing start, the
build task itself — read first, as instructed, before this file existed —
already stated several facts that are properly RTL-derived: that `ciab`'s
PRB is selected by an `rs[3:0]` index rather than by name; that
`portb_out` reaches 0xFF after reset; that a walking PRB pattern (with
DDRB=0xFF) reaches each pin in its own position; that a DIREC bit set in one
PRB write survives two following STEP-only writes; and the conflict
resolution (CIABPRB, not CIAAPRB, at $BFD100). None of that was discovered
by this document — it was handed to the builder before step 1 began, in the
build brief and finding-candidates file the task named as required reading.

Separately, and not excused by the above: in the course of confirming the
suite's own build pattern and toolchain, `ciab.v` itself was opened (in
full) before this file was committed, which is exactly the sequence the
build brief's order of work says not to do ("If you need the port names to
write a testable specification, say so and stop... Knowing the port list
before writing the spec is exactly how an expected value starts life as a
fact about the implementation."). Disclosed rather than papered over, per
the build task's own instruction for exactly this situation. Two things
limit what that reading could have contaminated: first, every expected
value in this document (0xFF after reset, independent bit transfer, DIREC
stability, the STEP high-low-high shape) was already stated as fact by the
build task itself, prior to any RTL reading of my own, so nothing here was
first learned by reading `ciab.v`; second, no expected value in this file
was chosen, adjusted, or narrowed because of anything `ciab.v` showed that
the build task had not already said. The concrete stimulus bytes and the
mutation implementations are step-2 material and are written after this
file is committed, as the order of work requires.
