# ch8-drive-select-motor-led — SPECIFICATION

Written from the quotations in `ch8-drive-select-motor-led.jsonl` alone (the
eight claims assigned to this package by
`work/ch8-s2-build/ch8-drive-select-motor-led.jsonl`). No RTL file has been
opened by me yet. This document is committed before that happens.

## Source claims (verbatim quotations, all page 244)

| uid | restatement | quote span |
|---|---|---|
| `opus:8-cia.c24` | Each drive latches the motor signal at the moment its own select signal turns on. | "Each drive will latch the motor signal at the time its select signal turns on." |
| `sol:8-cia.c22` | A drive latches the motor signal when that drive's select signal turns on. (twin of `opus:8-cia.c24`) | "Each drive will latch the motor signal at the time its select signal turns on." |
| `opus:8-cia.c25` | A drive's motor stays in the latched state until the next time that drive's select turns on. | "The disk drive motor will stay in this state until the next time select turns on." |
| `sol:8-cia.c23` | The latched motor state persists until the next assertion of select. (twin of `opus:8-cia.c25`) | "The disk drive motor will stay in this state until the next time select turns on." |
| `opus:8-cia.c26` | DSKMOTOR* also drives the activity light on the front of the drive. | "DSKMOTOR* also controls the activity light on the front of the disk drive." |
| `sol:8-cia.c24` | DSKMOTOR* also controls the drive's front activity light. (twin of `opus:8-cia.c26`) | "DSKMOTOR* also controls the activity light on the front of the disk drive." |
| `opus:8-cia.c28` | A drive "remember[s]" its motor state while it is not selected. | "The drive will \"remember\" the state of its motor when it is not selected." |
| `sol:8-cia.c26` | An unselected drive retains the previously latched motor state (source's own word: "remember"). (twin of `opus:8-cia.c28`) | "The drive will \"remember\" the state of its motor when it is not selected." |

All eight come from the same paragraph (p.244, `PB7 DSKMOTOR*`) and all eight
are marked `disposition: new` for this package — none is a citation onto an
assertion built elsewhere.

Note the id collision that makes `classify.uid` mandatory rather than the bare
id: the extraction pipeline assigned the bare id `c24` twice on this page, once
to `opus` for the latch-edge sentence and once to `sol` for the LED sentence
(`opus:8-cia.c24` ≠ `sol:8-cia.c24` — different propositions entirely). Citing
"c24" without its reader prefix would be ambiguous inside this very package.

## Reduction to four testable propositions

The eight claims collapse into four propositions, each carried by a twin pair
(one `opus` reading, one `sol` reading, cross-checked `ok` by the other
reader):

- **P1 — latch-at-edge.** A drive samples the motor line at the edge where its
  *own* select turns on, and only there. (`opus:8-cia.c24`, `sol:8-cia.c22`)
- **P2 — persists-until-next-select.** Between two turn-on edges of the same
  drive's select, the latched motor state does not change. (`opus:8-cia.c25`,
  `sol:8-cia.c23`)
- **P3 — LED follows the motor.** DSKMOTOR* also drives the drive's front
  activity light. (`opus:8-cia.c26`, `sol:8-cia.c24`)
- **P4 — remembered while deselected.** A drive keeps its latched motor state
  while it is not the selected drive. (`opus:8-cia.c28`, `sol:8-cia.c26`)

P1, P2 and P4 are three facets of one mechanism (a per-drive latch clocked by
that drive's own select edge and written nowhere else); they are still graded
as four independent assertions below (eight, counting twins) because each
citing quotation is a distinct claim under this suite's rules, not because the
mechanism itself has four moving parts.

## P3 carries the package's headline finding, and its own history

P3's two claims were **not** classified as ordinarily testable. Both readers'
independent extractions of this sentence were **discarded together** at
classification, on the reasoning that a physical lamp on the front of a disk
drive's case is outside any chip boundary and has no port — `reason_kind:
not-hardware`. That discard was then **overturned by the attack stage**
(`attacker: sol`, `verdict: overturned`), which is disclosed as an `attack`
block on both records rather than folded silently back into `testable`. The
attack's argument, quoted from the claims file: `fdd_led` is a real,
named output of `paula_floppy` (cited there as `paula_floppy.v:100`), so a
motor→light claim is directly gradeable at this boundary after all — the
nearest analogue to "the front-panel lamp" is not absent, it is a signal this
suite can drive and read. The claims file also discloses, as the attacker's
own supporting evidence (not yet independently verified by me — see below),
that `fdd_led` is driven from disk-DMA activity rather than from the motor
line, which is this package's expected finding if that citation holds up once
I read the RTL myself.

**The prescribed fixture, verbatim from the attack block and from this
package's own build brief:** hold disk DMA idle, drive `_sel` and the
active-low `_motor` through both motor states, and read `fdd_led`; **then**
start disk DMA as the control case that shows what actually does drive the
LED. That second half is not optional — a red P3 reading against a fixture
that never demonstrates `fdd_led` moving *at all* would be indistinguishable
from a dead, stuck-low output, and the control is what rules that out. This
control also doubles as this package's mandatory "at least one control case
expected to PASS on the same measurement basis as anything under suspicion":
it is read through the identical `fdd_led` port, on the identical fixture, as
P3's own two assertions.

Because P3 rides on a signal (`dskstate`, `DISKDMA_IDLE`) outside the plain
motor/select port list P1/P2/P4 use, and because arming disk DMA needs a
register write path into the module (disclosed by the build brief as needing
the DSKLEN double-write, `paula_floppy.v:743`, to arm DMA — not yet verified
by me), P3's assertions are expected to need more of `paula_floppy`'s
interface than P1/P2/P4 do. That is still within the `paula_floppy` boundary
already assigned to this package; no wider module is needed.

## Fixture boundary: why `paula_floppy` and not a wider or narrower one

This package's fixture is `paula_floppy` (assigned by the build brief, not
derived by me from reading RTL). All eight claims are about a disk drive's
own motor/select/light behaviour, which — per the claims file's own disclosed
evidence — is modelled entirely inside `paula_floppy`; ciaa/ciab (the generic
8520 CIA ports that carry DSKMOTOR* as a bit of CIABPRB before it reaches the
floppy module) are explicitly named elsewhere as out of scope for the
drive-side half of this behaviour. I have not opened ciaa.v/ciab.v/minimig.v
to confirm the wiring between them and `paula_floppy` myself; that is not
needed here because the assertions below are entirely about what happens once
`_sel`/`_motor` reach `paula_floppy`'s own ports, not about how a CIA register
bit gets there.

The claims file discloses (evidence already written by the classify stage,
citing exact line numbers) that the per-drive latch is implemented as four
copies of one pattern gated on each drive's own `_sel[n]` turn-on edge, that it
is read back through a port called `index`, and that `fdd_led` is driven from
a DMA state comparison. I treat all of this as **disclosed but not yet
independently verified** — the same posture `hrm/ch8-ciaapra-address-window`
took toward its own disclosed `gary.v` citations before its RTL-reading step.
Verifying it, and reading the exact port directions/widths and the mechanics
of `index`'s other qualifying terms (a rotation counter and a frame strobe,
per the disclosed evidence), happens in the next step, after this file is
committed.

## Rig hazards named by the build brief (not derived by me from RTL)

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680`, per the build
  brief). A Verilated model reused across trials would carry stale head
  position into a later trial. **A fresh `Vpaula_floppy` instance is
  allocated per trial** in this package — no trial reuses another trial's
  model, regardless of whether that trial's own assertions touch `dsktrack`.
- **`paula_floppy_ebr.v` declares a second `module paula_floppy`.** The build
  is `paula_floppy.v` + `paula_floppy_fifo.v` only; `paula_floppy_ebr.v` is
  left out of the source list entirely (not merely warning-suppressed — the
  build brief calls that the cleaner route, and the existing
  `hrm/paula_floppy` package's `-Wno-fatal` workaround is not repeated here).
- **DSKLEN needs a double write to arm DMA** (`paula_floppy.v:743`, per the
  build brief) — needed for P3's DMA-start control case, not for P1/P2/P4.

## Assertions — one per citing quotation, each naming its page

All eight claims are page 244. Working labels below (finalised, with exact
line citations, once the RTL is open and the test is written):

- **A1** — `opus:8-cia.c24` (P1, latch-at-edge)
- **A2** — `sol:8-cia.c22` (P1, twin of A1)
- **A3** — `opus:8-cia.c25` (P2, persists-until-next-select)
- **A4** — `sol:8-cia.c23` (P2, twin of A3)
- **A5** — `opus:8-cia.c26` (P3, LED follows motor — attack-recovered, expected
  finding)
- **A6** — `sol:8-cia.c24` (P3, twin of A5)
- **A7** — `opus:8-cia.c28` (P4, remembered while deselected)
- **A8** — `sol:8-cia.c26` (P4, twin of A7)

Plus, ungraded by any manual citation but required by this suite's own rules:

- **A0 (fixture liveness)** — the motor-latch/`index` data path used by
  P1/P2/P4 can deliver a known, distinct word before any of A1–A4/A7–A8 is
  trusted to have read one.
- **CTRL-DMA (control case, doubles as P3's mandatory control)** — starting
  disk DMA, on the same `fdd_led` readout as A5/A6, is what actually moves the
  LED — the prescribed second half of the attack's fixture.

## Mutation plan

Each of A1–A8 gets its own
`// MUTATION <label> stimulus <red|green> <what changed>` record in the C++
source, `<label>` being the exact leading token of the string literal passed
to `expect()` for that assertion (this suite's chapter-8 convention, per
`tools/check_mutation_labels.py`'s own comment, is that leading token may be
the claim's `classify.uid` itself). The planned stimulus mutation for each is:
change what the rig *asks* the module to do — which drive is selected when,
or which motor level is driven, or whether DMA is armed — while leaving the
expected value alone, and confirm the targeted assertion alone goes red, then
revert.

If, once built, any assertion is itself red against the RTL at baseline (the
oracle is the manual; a disagreeing RTL is a valid, finished red — this is the
expected outcome for A5/A6 per the attack's own evidence), the substitute rule
applies instead of a stimulus mutation on an already-red check: a different
mechanism, proven live elsewhere in the same run, is put through the same
comparator and shown to go green. Which of these two paths applies to which
assertion is decided from the measured baseline, not guessed in advance — this
section states the *plan*, not a claim that any run has happened yet.

## What happens next

Only after this file is committed: read `paula_floppy.v` in full (the ports
block, the four-copy `motor_on` latch, the `index` computation and its other
operands `rpm_pulse_cnt`/`sof`, the `fdd_led`/`dskstate`/`DISKDMA_IDLE`
assignment, the DSKLEN double-write path, the `dsktrack` reset gap) and
`paula_floppy_fifo.v`; read the existing `hrm/paula_floppy/Makefile` and
`hrm/paula_floppy/test_paula_floppy.cpp` for this suite's build pattern and
default port init values (disclosed there as an existing precedent, not HRM
conformance evidence itself); then write this package's own Makefile and
test source. All of that reading, and anything else opened beyond what is
named here, will be disclosed in the next commit's file header.
