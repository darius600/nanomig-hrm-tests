# Specification — `ch8-ciabprb-disk-control-bits`

Package: `hrm/ch8-ciabprb-disk-control-bits`
Fixture: `ciab` (compiled alone, with its CIA submodules `cia_int.v`,
`cia_timera.v`, `cia_timerb.v`, `cia_timerd.v`, the way
`hrm/ch8-ciaa-port-direction` compiles `ciaa`).
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
above `ciab` (`minimig.v:650`). A claim that states "bit PBn is named X" is
therefore only partly a claim about `ciab`: the fact that PBn is a distinct,
independently writable bit position is `ciab`'s own behaviour and is
testable here; the fact that position n carries the name X, and therefore
drives a particular physical disk line, is not something this fixture's
compiled boundary can confirm or deny. This package grades the first half
and declines the second, naming the reason, rather than exercising a
walking-bit pattern and letting a passing result stand in for a claim about
drive semantics it did not test. The second half is graded by
`hrm/ch8-cia-disk-signal-bindings` at the `minimig.v:650` join.

The same split applies to "active low", reset defaults, write order and
pulse shape: what `ciab` alone can show is that the register echoes what
software writes and that a PB bit's pull-up/reset default is HIGH, not that
a physically LOW line runs a specific drive's motor, that a drive reacts to
the order of two writes, or that a step pulse moves a head — that is the
downstream consumer's fact (`paula_floppy.v` and the drive, outside this
boundary) and is not re-derived here either, even where a claim's own
`classify.evidence` cites it as corroboration.

## Claim inventory (all 30)

| # | uid | page | topic | assigned |
|---|---|---|---|---|
| 1 | `opus:8-cia.c4` | 244 | PB bits are outputs in CIABPRB ($BFD100) | **PROP-POSITION** |
| 2 | `opus:8-cia.c21` | 244 | PB7 named DSKMOTOR* | DECLINED — name binding (PB7-BINDING) |
| 3 | `opus:8-cia.c22` | 244 | DSKMOTOR* active low | DECLINED — no consumer (PROP-RESET-DEASSERT, measured) |
| 4 | `opus:8-cia.c29` | 244 | all drive motors off after reset | DECLINED — no consumer (PROP-RESET-DEASSERT, measured) |
| 5 | `opus:8-cia.c32` | 245 | PB6 named DSKSEL3* | DECLINED — name binding (PB6-BINDING) |
| 6 | `opus:8-cia.c33` | 245 | DSKSEL3* active low | DECLINED — no consumer (PROP-RESET-DEASSERT, measured) |
| 7 | `opus:8-cia.c34` | 245 | PB5 named DSKSEL2* | DECLINED — name binding (PB5-BINDING) |
| 8 | `opus:8-cia.c35` | 245 | DSKSEL2* active low | DECLINED — no consumer (PROP-RESET-DEASSERT, measured) |
| 9 | `opus:8-cia.c36` | 245 | PB4 named DSKSEL1* | DECLINED — name binding (PB4-BINDING) |
| 10 | `opus:8-cia.c37` | 245 | DSKSEL1* active low | DECLINED — no consumer (PROP-RESET-DEASSERT, measured) |
| 11 | `opus:8-cia.c38` | 245 | PB3 named DSKSEL0* | DECLINED — name binding (PB3-BINDING) |
| 12 | `opus:8-cia.c40` | 245 | DSKSEL0* active low | DECLINED — no consumer (PROP-RESET-DEASSERT, measured) |
| 13 | `opus:8-cia.c41` | 245 | PB2 named DSKSIDE | DECLINED — name binding (PB2-BINDING) |
| 14 | `opus:8-cia.c45` | 245 | PB1 named DSKDIREC | DECLINED — name binding (PB1-BINDING) |
| 15 | `opus:8-cia.c48` | 245 | DSKDIREC set up before STEP, separate write | DECLINED — no consumer (PROP-ORDER, measured) |
| 16 | `opus:8-cia.c49` | 245 | PB0 named DSKSTEP* | DECLINED — name binding (PB0-BINDING) |
| 17 | `opus:8-cia.c50` | 245 | DSKSTEP* quick-pulse shape | DECLINED — no consumer (PROP-STEP-SHAPE, measured) |
| 18 | `sol:8-cia.c3` | 244 | twin of #1 | **PROP-POSITION** |
| 19 | `sol:8-cia.c20` | 244 | PB7 composed: position+name+polarity | DECLINED — name binding (PB7-BINDING) |
| 20 | `sol:8-cia.c27` | 244 | twin of #4 | DECLINED — no consumer (PROP-RESET-DEASSERT, measured) |
| 21 | `sol:8-cia.c30` | 245 | PB6 composed | DECLINED — name binding (PB6-BINDING) |
| 22 | `sol:8-cia.c31` | 245 | PB5 composed | DECLINED — name binding (PB5-BINDING) |
| 23 | `sol:8-cia.c32` | 245 | PB4 composed | DECLINED — name binding (PB4-BINDING) |
| 24 | `sol:8-cia.c33` | 245 | PB3 composed | DECLINED — name binding (PB3-BINDING) |
| 25 | `sol:8-cia.c35` | 245 | PB2 DSKSIDE named, twin of #13 | DECLINED — name binding (PB2-BINDING) |
| 26 | `sol:8-cia.c39` | 245 | PB1 DSKDIREC named, twin of #14 | DECLINED — name binding (PB1-BINDING) |
| 27 | `sol:8-cia.c42` | 245 | DSKDIREC before STEP (ordering half) | DECLINED — no consumer (PROP-ORDER, measured) |
| 28 | `sol:8-cia.c43` | 245 | DSKDIREC separate write (register-structure half) | DECLINED — no consumer (PROP-ORDER, measured) |
| 29 | `sol:8-cia.c44` | 245 | PB0 named DSKSTEP*, twin of #16 | DECLINED — name binding (PB0-BINDING) |
| 30 | `sol:8-cia.c45` | 245 | DSKSTEP* quick-pulse shape, twin of #17 | DECLINED — no consumer (PROP-STEP-SHAPE, measured) |

2 claims graded (PROP-POSITION); 28 declined: 16 name bindings, graded in
`hrm/ch8-cia-disk-signal-bindings` by the PBn-BINDING assertion named in
the table, and 12 whose reset, order or pulse-shape content has no consumer
at this boundary (measured and printed under the PROP named in the table).
None is declined for doubt about the chip letter, which is settled.

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

Status: graded, at `ciab`'s own boundary (no address decoder or minimig
instantiation needed for this half).

## PROP-RESET-DEASSERT — every PB bit defaults / resets to its deasserted (HIGH) level

Page 244 (`opus:8-cia.c22`, "DSKMOTOR* ... active low"; `opus:8-cia.c29`
and twin `sol:8-cia.c27`, "All drive motors turn off after system reset")
and page 245 (`opus:8-cia.c33`/`c35`/`c37`/`c40`, "DSKSEL3*/SEL2*/SEL1*/SEL0*
... active low").

**Measurement at this boundary:** for each of these five positions (PB7
motor, PB6–PB3 select 3–0), the register's own reset behaviour and 8520
pull-up default (undriven bit reads 1) leave that bit HIGH — the deasserted
level an active-low signal's default state must be, and the state that must
hold immediately after `reset` for "all drive motors turn off after system
reset" to be true of the CIA side of that claim. This is the whole byte at
once (`portb_out`=0xFF after reset, with PRB written all-low just before
the reset pulse), since all five positions share the identical mechanism
(`ciab.v`'s `regportb`/`ddrportb` reset clears, and the pull-up OR). Not
measured: what a drive does with a LOW level once selected, or that each
drive individually latches and remembers the motor state at its own select
edge — both are downstream of `ciab`.

Status: measured and printed via `note()`, not graded — with no drive or
motor consumer at this boundary, the reset default of a named signal is not
shown; the seven uids are declined via `record()`.

## PROP-ORDER — DSKDIREC, once written, survives an independent STEP-only write

Page 245 (`opus:8-cia.c48`; twin halves `sol:8-cia.c42` — the ordering half
— and `sol:8-cia.c43` — the register-structure half): "This line [DSKDIREC]
must be set up before the actual step pulse, with a separate write to the
register."

**Measurement at this boundary:** DSKDIREC (PB1) and DSKSTEP* (PB0) are two
bits of the same PRB byte, so no single write can present one before the
other — the manual's "separate write" requirement is exactly the fact that a
second, independent write is what changes DSKSTEP* while leaving DSKDIREC's
value from the first write undisturbed. Measured by writing DSKDIREC once,
then two further writes that only toggle DSKSTEP* (the shape
PROP-STEP-SHAPE also uses), and confirming DSKDIREC's bit position never
moves across either of those two writes.

Status: measured and printed via `note()`, not graded — with no head or
drive consumer at this boundary, the write order is not observable beyond
the stimulus, and that the register holds what was written is already
established by PROP-POSITION; the three uids are declined via `record()`.

## PROP-STEP-SHAPE — DSKSTEP* observably pulses high, momentarily low, then high

Page 245 (`opus:8-cia.c50`; twin `sol:8-cia.c45`): "This signal must always
be used as a quick pulse (high, momentarily low, then high)."

**Measurement at this boundary:** the shape — high, low, high — across three
successive PRB writes is observable on `portb_out`[0]. How short
"momentarily" is — a duration with no companion interval on this page to
form a ratio with — is out of scope for this package.

Status: measured and printed via `note()`, not graded — with no head
consumer at this boundary the measurement only shows `portb_out` mirroring
the three writes, the stimulus itself; the two uids are declined via
`record()`.

## On a control case, fixture liveness and mutation

- **Fixture liveness.** Two distinct, known bytes written to PRB (with DDRB
  set to all-outputs) must both arrive on `portb_out`, and differ from each
  other, before PROP-POSITION or anything after it is trusted. Printed via
  `note()`: it carries no uid, and this suite never grades a no-uid check.
- **Control.** The same per-bit comparator PROP-POSITION sweeps across all
  eight positions, instantiated once more at a single bit position alone,
  carries no uid, and is printed via `note()` for the same reason.
- **Mutation.** PROP-POSITION, the one graded assertion, is GREEN at
  baseline and carries a stimulus mutation, in the source, in the fixed
  `// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what
  changed>` form: its stimulus withholds the clearing write for one bit,
  leaving the expected value alone. A `note()` or `record()` carries no
  mutation.

## Claims not assigned an assertion (28 of 30)

**The 16 name bindings.** `ciab` compiled alone has no signal named
DSKMOTOR*, DSKSEL3*–DSKSEL0*, DSKSIDE, DSKDIREC or DSKSTEP* — only
`portb_out[7:0]`, a generic 8520 port. The correspondence between a bit
position and one of these names is a wiring fact one level above this
fixture (`minimig.v:650`), graded in `hrm/ch8-cia-disk-signal-bindings` by
the PBn-BINDING assertion for each signal: `opus:8-cia.c21`, `c32`, `c34`,
`c36`, `c38`, `c41`, `c45`, `c49`, and composed twins `sol:8-cia.c20`,
`c30`, `c31`, `c32`, `c33`, `c35`, `c39`, `c44`. The five composed `sol`
claims that also assert active-low polarity (`c20`, `c30`, `c31`, `c32`,
`c33`) are not cited on PROP-POSITION's graded line, because a uid cited on
a graded line is counted asserted for the *whole* claim it names, including
the name-binding half this package does not test.

**The 12 reset, order and pulse-shape claims.** `opus:8-cia.c22`, `c29`,
`c33`, `c35`, `c37`, `c40` and `sol:8-cia.c27` (PROP-RESET-DEASSERT);
`opus:8-cia.c48`, `sol:8-cia.c42`, `sol:8-cia.c43` (PROP-ORDER);
`opus:8-cia.c50`, `sol:8-cia.c45` (PROP-STEP-SHAPE). Each is measured and
printed at this boundary, and none is independent evidence beyond the
stimulus and PROP-POSITION: no drive, motor or head consumer is present to
show the reset default, the write order or the pulse shape matters. Each
`record()` says what the claim would need.
