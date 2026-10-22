# Specification — `ch8-disk-index-interrupt-path`

Package: `hrm/ch8-disk-index-interrupt-path`
Fixture: a **RIG over three modules** — `paula_floppy` (+ `paula_floppy_fifo`),
`ciab` (+ its CIA submodules `cia_int.v`, `cia_timera.v`, `cia_timerb.v`,
`cia_timerd.v`), and `paula_intcontroller` — each compiled as its own
Verilated top and wired together in the C++ harness, not by any Verilog
top-level module. `paula_floppy_ebr.v` is left out of the build: it declares
a second `module paula_floppy` and would collide with `paula_floppy.v`.
Claims file: `hrm-testpipeline/work/ch8-s2-build/ch8-disk-index-interrupt-path.jsonl`
(7 claims).
Subject: printed page 250's disk-interrupt summary line for INDEX, and
printed page 245's CIABPRB/FLAG row for DSKINDEX* — the journey of one pulse
from the drive's index signal to a level-6 68000 interrupt request, through
the 8520 (ciab) FLAG pin and the shared `int6` line.

No RTL file has been opened for the purpose of drafting this document. Every
RTL fact used below — port names, expressions, line numbers, and the two
measurements quoted as already taken — was handed over before step
1 began, in the build task and in the claims file's own `classify.evidence`
fields (both required reading per the build task). The line at the end of
this document discloses exactly what that is and is not.

## The RIG's joins — mine to make, stated here so the rig can be checked

Because this fixture is three independently-instantiated Verilated models
with no shared Verilog parent, the wiring between them is done in the C++
harness, once per tick, and each join reproduces one piece of `minimig.v`'s
own top-level wiring:

1. **`paula_floppy`'s `index` output → `ciab`'s `flag` input.** Reproduces
   `minimig.v:657`, `.flag(index)` — the ciab instantiation's FLAG port
   fed from the wire `index` — combined with `minimig.v:536`, `.index(index)`,
   which is where that wire is confirmed to be `paula_floppy`'s own `index`
   output (the paula_floppy instantiation's INDEX port, named `index` at
   both ends of the wire). The harness reads `paula_floppy`'s `index` output
   each tick and drives it onto `ciab`'s `flag` input before evaluating
   `ciab`.
2. **`ciab`'s `irq` output → `paula_intcontroller`'s `int6` input.**
   Reproduces `minimig.v:656`, `.irq(int6)` — the ciab instantiation's IRQ
   port driving the wire `int6` — combined with `minimig.v:521`, which is
   where that same wire is confirmed to be the `paula` instantiation's
   `int6` input. `paula_intcontroller` is one level inside `paula`, and its
   own `int6` port (present on its port list per the claims'
   `classify.evidence`, not independently confirmed by opening `paula.v`,
   which is outside this fixture) is what actually receives the interrupt
   line inside `paula`. The harness reads `ciab`'s `irq` output each tick
   and drives it onto `paula_intcontroller`'s `int6` input before evaluating
   `paula_intcontroller`.

No other signal is joined between the three models. Everything else each
module needs (clocks, resets, the CIA's own bus signals, `paula_intcontroller`'s
other interrupt sources, `INTENA` bits) is driven directly by the harness as
fixed or independently-varied stimulus, not sourced from one of the other two
DUTs.

## What this fixture is for, and what it must decline

Per the build task: this package grades the index pulse's whole downstream
journey — that a pulse on `index` reaches the CIA, raises the interrupt, and
lands at the level the page names. It does **not** grade, and cannot grade,
where the `index` pulse itself comes from physically. Page 250's own quoted
text calls it "index sensor triggered," and the build task states directly
that `index` is *synthesised* inside `paula_floppy` (cited there as
`paula_floppy.v:576`) rather than driven by any modelled sensor pin — there
is no medium, no physical hole, and no sensor input anywhere in this design.
**No claim in this package's own 7 restates that causal sensor-to-signal
step**, so no claim needs to be declined outright for it — but the boundary
is real and is recorded here as a standing non-goal: this package treats
`paula_floppy`'s `index` output as a given, real signal (whatever produces
it inside `paula_floppy` is out of scope for grading) and asserts only what
happens to a pulse on it from that point on. A `record()` line in the test
source names this decision so it is not silently assumed.

## Claim inventory (all 7)

| # | uid | page | quote / restatement | proposition |
|---|---|---|---|---|
| 1 | `opus:8-dskint.c8` | 250 | "The INDEX interrupt is at level 6." | **PROP-LEVEL6** |
| 2 | `opus:8-dskint.c9` | 250 | "...arrives on the 8520 Flag pin, not through an INTREQ bit of its own." (composed, both halves) | **PROP-FLAG-PIN** + **PROP-NOT-OWN-BIT** |
| 3 | `sol:8-int.c8` | 250 | "The INDEX interrupt is level 6." (twin of #1) | **PROP-LEVEL6** |
| 4 | `sol:8-int.c9` | 250 | "...is connected through the 8520 FLAG pin." (positive half only) | **PROP-FLAG-PIN** |
| 5 | `opus:8-cia.c56` | 245 | "The 8520 FLAG input is named DSKINDEX* and carries the disk index pulse." | **PROP-FLAG-PIN** |
| 6 | `opus:8-cia.c58` | 245 | "DSKINDEX* can be used to generate a level 6 interrupt." | **PROP-LEVEL6** |
| 7 | `sol:8-cia.c52` | 245 | "DSKINDEX* can generate a level-6 interrupt." (twin of #6) | **PROP-LEVEL6** |

All 7 claims carry `classify.status: testable` in the claims file; none is
declined. `sol:8-int.c9`'s restatement covers only the FLAG-pin half of its
twin `opus:8-dskint.c9`'s composed claim, so it is cited on PROP-FLAG-PIN
only, not on PROP-NOT-OWN-BIT — citing it there too would grade a half it
does not restate. `opus:8-dskint.c9` itself is cited on both propositions,
because — unlike a declined half — both halves of its composed claim are in
fact graded by this package.

## PROP-FLAG-PIN — `index` is the signal `ciab`'s FLAG pin carries, and a pulse on it sets ICR's FLG bit / raises `irq`

Page 245 (`opus:8-cia.c56`): "FLAG DSKINDEX* Disk index pulse ($BFDD00, bit
4)." Page 250, positive half of `opus:8-dskint.c9` and the whole of
`sol:8-int.c9`: "...arrives on the 8520 Flag pin" / "...connected through
the 8520 FLAG pin."

**Testable proposition, at the join stated above:** a pulse presented on
`paula_floppy`'s real `index` output, carried across join 1 onto `ciab`'s
`flag` input, is what `ciab` treats as the FLAG pin's DSKINDEX* pulse — it
sets ICR bit 4 (FLG) and, when that bit is unmasked in the ICR mask, raises
`ciab`'s `irq` output. The claims file already records this leg as measured
independently at `ciab` alone (`tb_ciab.cpp` M7, cited in
`classify.evidence`): with FLG unmasked, a pulse on `flag` drives `irq` to 1
and a subsequent ICR read (`rs`=0xD) returns `data_out`=0x90 (bit 4 FLG and
bit 7 IR both set); the read itself clears the latch; with no pulse, `irq`
stays 0. This package's own contribution is the **join** — showing that the
pulse on `flag` genuinely originates from `paula_floppy`'s own `index`
output rather than being driven synthetically, since driving `flag` directly
from the harness would not test join 1 at all.

Status: testable, across the paula_floppy→ciab join.

## PROP-LEVEL6 — a pulse on `index`, carried through both joins with the interrupt unmasked, requests 68000 level 6

Page 250 (`opus:8-dskint.c8`, twin `sol:8-int.c8`): "INDEX (level 6, 8520
Flag pin)." Page 245 (`opus:8-cia.c58`, twin `sol:8-cia.c52`): "Can be used
to create a level 6 interrupt."

**Testable proposition, across both joins:** a pulse on `paula_floppy`'s
`index`, with `ciab`'s ICR FLG bit unmasked (so `irq` rises per
PROP-FLAG-PIN) and with `paula_intcontroller`'s INTENA configured to accept
bit 13, ends with `paula_intcontroller`'s interrupt-request register
reading bit 13 set and its `_ipl` output at the code the claims file already
records as measured (INTREQR = 0x2000 with `int6` alone driven; `_ipl`
driven to the level-6 code once INTENA and bit 13 are both enabled). Level
6 is the composed claim: FLAG pin → ICR/`irq` (measured at `ciab`) → `int6`
→ INTREQ bit 13 → `_ipl` (measured at `paula_intcontroller`) — this package
adds both joins that connect those two already-measured legs into one
chain driven by a real `index` pulse.

Status: testable, across the full three-module chain.

## PROP-NOT-OWN-BIT — INDEX has no INTREQ bit of its own; it can only arrive on the shared `int6` line

Page 250, negative half of `opus:8-dskint.c9`: "...not through an INTREQ
bit of its own."

**Testable proposition, at `paula_intcontroller`'s own port list:** the
claims file's own evidence already establishes that `paula_intcontroller`
declares `rxint, txint, vblint, int2, int3, int6, blckint, syncint` and
`audint`, and nothing named for the disk index — so no input on this module
can carry an INDEX-specific request; INTREQ bit 13 can only ever be raised
through the shared `int6` line, which `ciab`'s `irq` output (itself shared
by every unmasked ICR source, not just FLG) also drives. Graded by showing
that a non-FLG interrupt source inside `ciab` (any other unmasked ICR bit,
driven without ever pulsing `flag`) reaches `paula_intcontroller` and sets
the identical INTREQ bit 13 that a `flag` pulse does — i.e., that bit 13 is
`int6`'s bit, not `index`'s bit, and `index` has no separate bit to call its
own. This is also the package's negative control (see below): a different,
already-live mechanism (a non-FLG `ciab` interrupt source) put through the
same join-2 comparator as PROP-LEVEL6, showing the same INTREQ bit 13
without ever touching `flag`/`index`.

Status: testable, at `paula_intcontroller`'s port list plus the join-2
boundary.

## On fixture liveness, the control case, and mutation

Per this suite's rules, in the fixed order: fixture liveness precedes any
data-dependent grading; at least one control case is on the same
measurement basis as the graded claims; every graded line (liveness and
control included) carries a stimulus mutation recorded in the source in the
fixed `// MUTATION <label> <oracle|stimulus|none> <red|green|impossible>
<what changed>` form.

- **Fixture liveness.** Two distinct, known conditions must produce two
  distinct, correctly-differing readings at `paula_intcontroller`'s
  boundary before anything else is graded: (a) `paula_floppy` driven so
  that it never pulses `index`, with everything else held as in the graded
  case, must leave INTREQ bit 13 and `_ipl` at their non-requesting values;
  (b) `paula_floppy` driven so that it does pulse `index` must move both.
  This exercises the full rig (both joins, all three DUTs) with the
  simplest possible distinct inputs, ahead of PROP-LEVEL6's own grading.
  The exact stimulus that makes `paula_floppy` pulse `index` (its port
  names for drive select / motor state, and the timing of its internal
  rotation counter) is step-2 material, fixed once the RTL is read.
- **Control.** PROP-NOT-OWN-BIT's own comparator — an unmasked non-FLG
  `ciab` interrupt source reaching `paula_intcontroller` via join 2 alone,
  without any `flag` pulse — is the package's control case: it is expected
  to PASS on the identical join-2 measurement basis as PROP-LEVEL6, proving
  the `irq`→`int6`→bit-13→`_ipl` leg is live and correctly wired
  independently of whether `paula_floppy`/join 1 ever fires at all.
- **Mutation, all three PROPs.** Nothing in this slice's finding candidates
  names this package or these claim ids as expected red, so, like
  `ch8-ciabprb-disk-control-bits` and `ch8-adkcon-disk-control-bits`, every
  proposition here is expected GREEN at baseline, and every mutation is the
  ordinary stimulus form: change what one DUT is fed (never the expected
  value), rebuild, confirm the assertion goes RED, revert. PROP-FLAG-PIN's
  stimulus mutation withholds the FLG unmask (or breaks join 1 so no pulse
  crosses); PROP-LEVEL6's stimulus mutation withholds INTENA/bit-13 enable
  (or breaks join 2); PROP-NOT-OWN-BIT's stimulus mutation swaps its
  non-FLG source for one that is masked, so nothing reaches `int6` at all.
  Concrete stimulus values and which `ciab` ICR bit stands in as the
  "non-FLG source" for the control/PROP-NOT-OWN-BIT are step-2 material.

## Claims not assigned an assertion

None. All 7 claims in this package's claims file carry `classify.status:
testable` and are each cited on at least one graded `expect()` line above.

## Rig hazards carried into the build, already paid for once elsewhere in this slice

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680`, per the build
  task). Not directly exercised by this package's own claims, but since
  the fixture instantiates a live `paula_floppy`, a fresh Verilated model
  will be allocated per trial regardless, as this suite requires for
  every package touching this module.
- **`paula_floppy_ebr.v` declares a second `module paula_floppy`.** Left out
  of the build entirely, per the build task and the finding-candidates
  file's "one more rig hazard" note — not worked around with `-Wno-fatal`.

## Disclosure — what was already known before this file was drafted

This document is written from the 7 claims' `extract.quote` /
`classify.evidence` content and the build task's own text, both required
reading before step 1. Facts used above that are properly RTL-derived —
`paula_floppy.v:93` declaring the `index` output, `paula_floppy.v:576`'s
synthesis expression for `index`, `minimig.v:536/657/656/521`'s four join
lines, `cia_int.v:57` and `:62-66`'s FLG-latch/`irq` terms, the ICR=0x90
and INTREQR=0x2000/`_ipl` measurements already taken on `ciab` and
`paula_intcontroller`, and `paula_intcontroller`'s port list at
`paula_intcontroller.v:4-24/132` — were all handed to the builder in the
claims file's `classify.evidence` fields or the build task's own text, not
discovered by independently opening any RTL file for the purpose of writing
this specification. No RTL file assigned to this fixture (`paula_floppy.v`,
`paula_floppy_fifo.v`, `ciab.v` and its CIA submodules, or
`paula_intcontroller.v`) has been opened by this builder as of this
commit. The concrete stimulus needed to make `paula_floppy` produce a real
`index` pulse in a Verilated simulation at reasonable cost — its exact input
port names, and whether its internal rotation counter needs to be forced
rather than counted down in real simulated time — is explicitly left as
step-2 material throughout this document, to be resolved only after this
file is committed.

## PROP-NOT-OWN-BIT/CONTROL's second condition

PROP-NOT-OWN-BIT/CONTROL (`opus:8-dskint.c9`, the negative half — "not
through an INTREQ bit of its own") requires two conditions. Driving Timer A
(a different, non-FLG source) to the same bit13/level6 outcome shows bit
13 is shared, but does not rule out an ADDITIONAL, coexisting
index-specific INTREQ bit — both could exist and a check limited to the
Timer A half alone would still
pass, since that half never looks at what a real index pulse does to the rest
of INTREQR. So a second, always-run, falsifiable
condition runs alongside the Timer A one: a real index pulse must change
EXACTLY bit 13 of INTREQR (full-word before/after comparison) and no
other bit — a hypothesis where index also drives some other bit would
break this condition, which the Timer A half alone cannot do. Both
conditions must hold. Baseline: 4 passed, 0 failed;
`MUTATE=opus:8-dskint.c9` gives exactly 3 passed / 1
failed, only this label affected.
