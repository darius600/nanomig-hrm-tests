# ch8-drive-step-timing — SPECIFICATION

Written from the quotations in `ch8-drive-step-timing.jsonl` alone (30 claims).
No RTL, Makefile, or other package's files have been opened yet. This file is
committed before any of that happens.

Port names and line numbers appearing below (`_step`, `direc`, `_sel`,
`_track0`, `clk7_en`, `step_ena_cnt`, `paula_floppy.v:594-603`, etc.) are not
something I read from the RTL myself at this stage — they are already
disclosed, as part of my claims file's own `classify.evidence` fields and the
build brief handed to me, by the classification work done before this build
started. I am repeating that disclosure here, not independently verifying it;
verification happens in the next step, after this file is committed.

## Source claims (verbatim quotations)

| uid | page | quote |
|---|---|---|
| `opus:8-cia.c46` | 245 | "PB1 DSKDIREC ... Zero implies seek towards the center spindle. Track zero is at the outside of the disk. This line must be set up before the actual step pulse, with a separate write to the register." |
| `sol:8-cia.c40` | 245 | "Zero implies seek towards the center spindle." (twin of `opus:8-cia.c46`) |
| `opus:8-time.c15` | 242 | access-timing diagram, first "18 ms min" label on the STEP row |
| `opus:8-time.c16` | 242 | access-timing diagram, second "18 ms min" label on the STEP row |
| `opus:8-time.c17` | 242 | access-timing diagram, "3ms min" label (split over two lines) |
| `opus:8-time.c21` | 242 | read-timing diagram, "1us min" from DRIVE SELECT active to the STEP pulse |
| `opus:8-fig8-8.c3` | 243 | figure, leftmost `t` arrow: DRIVE SELECT falling edge to first STEP |
| `opus:8-fig8-8.c4` | 243 | figure, first "18ms min" arrow on the STEP row |
| `opus:8-fig8-8.c5` | 243 | figure, second "18ms min" arrow on the STEP row |
| `opus:8-fig8-8.c6` | 243 | figure, "3ms min" arrow between the two step bursts |
| `opus:8-fig8-8.c8` | 243 | figure, DIRECTION SELECT row's several arrows, each labelled only `t` |
| `opus:8-fig8-8.c10` | 243 | figure, read frame, "1 us min" arrow, DRIVE SELECT falling edge to STEP |
| `sol:8-fig8-7.c3` | 242 | Figure 8-7 (write timing), STEP pulse width "1us min" |
| `sol:8-fig8-8-text.c1` | 242 | extracted access diagram, outer STEP spacing "18 ms min" |
| `sol:8-fig8-8-text.c2` | 242 | extracted access diagram, close STEP spacing "3ms min" |
| `sol:8-fig8-8-text.c4` | 242 | extracted access diagram, TRACK 00 depicted active-low |
| `sol:8-fig8-9-text.c1` | 242 | extracted read-timing diagram, STEP pulse width "1us min" |
| `sol:8-fig8-8.c1` | 243 | figure, legend: "t: 1 us min." assigned to every `t`-labelled interval |
| `sol:8-fig8-8.c2` | 243 | figure, bare value 18 on the first separated STEP arrow |
| `sol:8-fig8-8.c3` | 243 | figure, bare value 3 on the close-spaced STEP arrow |
| `sol:8-fig8-8.c4` | 243 | figure, bare value 18 on the second separated STEP arrow |
| `sol:8-fig8-8.c6` | 243 | figure, DRIVE SELECT depicted active-low |
| `sol:8-fig8-8.c7` | 243 | figure, read frame, bare value 1 on the DRIVE-SELECT-to-STEP arrow |
| `sol:8-fig8-8.c17` | 243 | figure, upper-panel note: every `t` interval is a minimum (sense only) |
| `sol:8-fig8-8.c19` | 243 | figure, sense only: first separated STEP arrow is a minimum |
| `sol:8-fig8-8.c21` | 243 | figure, sense only: close-spaced STEP arrow is a minimum |
| `sol:8-fig8-8.c23` | 243 | figure, sense only: second separated STEP arrow is a minimum |
| `sol:8-fig8-8.c26` | 243 | figure, STEP depicted active-low |
| `sol:8-fig8-8.c27` | 243 | figure, TRACK 00 depicted active-low |
| `sol:8-fig8-8.c29` | 243 | figure, read frame, sense only: STEP pulse-width arrow is a minimum |

## Why 30 claims become 8 assertions, not 30

Every `classify` record in this file already carries a `split` (or matching
`evidence`) note saying which sibling claims it composes with — the printed
diagrams repeat the same handful of underlying propositions across two
renderings (an ASCII/text extraction on p.242 and the image figure on p.243)
and across two readers (`opus`, `sol`), each of whom sometimes split one
printed label into a value fragment, a unit fragment and a sense fragment.
Building one assertion per citing quotation, as this package's own sibling
`ch8-ciaapra-address-window` chose to do, would here mean grading the *same
measured fact* nineteen times over for the ratio alone. Grading it once, and
citing every uid that composed into it, is what "one assertion per
proposition" means when the propositions themselves were fragmented across
readers and renderings before I saw them. No claim is discarded by this
grouping; every one of the 30 uids below is named on exactly one assertion.

## The eight propositions

### A1 — RATIO-A: the printed 6:1 step-spacing ratio (RED, expected)

Composes `opus:8-time.c15`, `opus:8-time.c16`, `opus:8-time.c17`,
`opus:8-fig8-8.c4`, `opus:8-fig8-8.c5`, `opus:8-fig8-8.c6`,
`sol:8-fig8-8-text.c1`, `sol:8-fig8-8-text.c2`, `sol:8-fig8-8.c2`,
`sol:8-fig8-8.c3`, `sol:8-fig8-8.c4`, `sol:8-fig8-8.c19`, `sol:8-fig8-8.c21`,
`sol:8-fig8-8.c23` — 14 uids, all reductions (full labels, bare numbers, or
bare "minimum" sense) of the same three arrows in Figure 8-8's access-timing
diagram: two spacings marked 18 ms and one marked 3 ms, in the same frame, a
ratio of 6:1 between the widely-spaced and closely-spaced STEP minima.

**Proposition under test**: the module enforces two *different* minimum
step-to-step intervals, in a 6:1 ratio, depending on where in a step burst the
edge falls. Dimensionless (strip "ms" and 6:1 survives), so measurable in
clk7_en ticks without a clock frequency.

**Expected**: RED. The build brief already discloses that `paula_floppy` gates
every accepted `_step` edge through exactly one counter (`step_ena_cnt`, a
minimum of 256 clk7_en ticks, `paula_floppy.v:594-603`), with nothing that
varies the threshold by burst position. A ratio of 1:1, not 6:1, is what the
assertion will measure — and that is this package's headline finding
(candidate C3), not a defect in the assertion.

### A2 — RATIO-B: the `t`-interval equality (RED, expected)

Composes `sol:8-fig8-8.c1`, `opus:8-fig8-8.c8`, `opus:8-fig8-8.c3`,
`sol:8-fig8-8.c17`, `sol:8-fig8-8.c29` — 5 uids. The figure's own legend reads
"t: 1 us min." and assigns that single value to every interval marked only
`t`: the DIRECTION SELECT row's several arrows, the DRIVE-SELECT-to-first-STEP
arrow, and — read across into the paired read-timing frame — the STEP
pulse-width arrow.

**Proposition under test**: these different-looking intervals are governed by
one common floor. Dimensionless: the claim is an *equality* between two
things the module actually implements — the `direc` setup required before an
accepted `_step` edge, and the minimum accepted `_step` low pulse width.

**Expected**: RED. Already disclosed as measured and true at this fixture:
`direc` setup = 0 ticks, `_step` minimum low width = 1 tick. Not equal, so the
page's single shared "t" floor does not hold.

### A3 — SEL-GATING: DRIVE SELECT gates STEP (GREEN, expected)

Composes `opus:8-time.c21`, `opus:8-fig8-8.c10`, `sol:8-fig8-8.c7` (all: a
"1 us min" gap from DRIVE SELECT active to the STEP pulse, in the read-timing
frame) and `sol:8-fig8-8.c6` (DRIVE SELECT depicted active-low).

**Proposition under test**: DRIVE SELECT is active-low, and precedes STEP —
i.e. a step is only meaningful while the target drive is selected.
Dimensionless: an ordering/gating requirement, not the absolute 1 us value.

**Expected**: GREEN. This is a chip requirement the module is expected to
enforce structurally (accept a step only while the addressed drive is
selected), independent of the Chinon-specific microsecond figure.

### A4 — PULSE-WIDTH-FLOOR: STEP pulse width has a floor (GREEN, expected)

Composes `sol:8-fig8-7.c3` (Figure 8-7, write timing) and
`sol:8-fig8-9-text.c1` (read timing) — both print the STEP pulse width as
"1 us min".

**Proposition under test**: a floor on the accepted STEP low-pulse width
exists (a chip requirement the drive must meet, and the chip enforces on its
input). Dimensionless: existence and sense of the floor, not its microsecond
value.

**Expected**: GREEN. Already disclosed: the module's own minimum accepted
`_step` low width is 1 clk7_en tick.

### A5 — TRACK00-POLARITY (GREEN, expected)

Composes `sol:8-fig8-8-text.c4` and `sol:8-fig8-8.c27` (twins): both diagrams
depict TRACK 00 as active-low, asserted while the head is at track zero.

**Proposition under test**: the polarity of the TRACK 00 read-back — not a
timing value at all, and the one class of claim in this package that survives
the "with a Chinon drive" caveat entirely, since a chip-side polarity is not a
drive obligation.

**Expected**: GREEN.

### A6 — STEP-POLARITY (GREEN, expected)

`sol:8-fig8-8.c26`: the diagram depicts STEP as an active-low pulse.

**Proposition under test**: the module's actionable event is the *rising*
edge of `_step` (the end of the active-low pulse), not its falling edge or its
low level alone — a step held low and never released produces no motion.

**Expected**: GREEN.

### A7 — DIREC-PROTOCOL (GREEN, expected — this package's control case)

Composes `opus:8-cia.c46` and `sol:8-cia.c40` (twins): "Zero implies seek
towards the center spindle. Track zero is at the outside of the disk." —
composed, the page asserts `direc`=0 moves the head *away* from track zero,
i.e. increases the track count, and `direc`=1 reverses it.

**Proposition under test**: `direc`, sampled at an accepted `_step` edge,
selects increment (0) vs. decrement (1) of the internal track position,
observable through `_track0`.

**Expected**: GREEN. This is the package's control case: same rig, same
`_step`/`_track0` mechanism as A1/A2 above, on the same measurement basis, but
grading a proposition the module is expected to implement rather than one it
is expected to fail.

### A8 — FIXTURE-LIVE (GREEN, expected — fixture-liveness proof)

Not tied to a manual claim. Required by the build brief before anything else
here is trusted: the rig must be shown to deliver a known, *distinct* word
before it reads one. Two distinct `_step`/`direc` stimulus sequences must
produce two distinct, correct `_track0` read-backs — proof the stimulus-to-
observable path is not silently dead (which would let every assertion above
pass or fail for the wrong reason).

## Chipset

Pages 242–245 name no chipset marker and no `(E)` register; the classify
records agree the 8520-CIA/disk interface these pages describe is unchanged
across OCS, ECS and AGA. Nothing here is chipset-conditional.

## What this package does NOT grade

Every one of these pages is printed "with a Chinon drive" and warns the
figures change with other vendors' drives. The absolute microsecond/
millisecond *values* (1 us, 18 ms, 3 ms) are the Chinon's obligation, not
Paula's, and are not asserted here — only the dimensionless content each
`classify` record already reduced them to (a ratio, an equality, a sense, or a
polarity) is. This package does not claim the chip drives any particular wall-
clock timing; it claims the chip's own accept/reject mechanism does or does
not reproduce the *shape* the manual prints.

## Rig hazards (already disclosed, not rediscovered here)

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680` per the build
  brief). A reused Verilated model carries head position between trials.
  Every trial below allocates a fresh `Vpaula_floppy` instance.
- **`paula_floppy_ebr.v` declares a second `module paula_floppy`.** The build
  compiles `paula_floppy.v` + `paula_floppy_fifo.v` only.

## Mutation plan

Each of A1–A8 gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
line in the test source, `<label>` being the exact leading token of the
string literal passed to `expect()`.

- **A1, A2** (RED at baseline): an `oracle green` record — the expected ratio
  (6) / expected equality is changed to what the RTL actually produces (1 /
  "0 ticks vs 1 tick"), confirming the assertion is sensitive and the red is
  a genuine measurement rather than a broken comparator, then reverted. A
  stimulus mutation on an assertion already red cannot demonstrate anything
  further — it cannot fail harder.
- **A3–A8** (GREEN at baseline): a `stimulus red` record each — change what
  the rig asks the module to do (deselect the drive throughout; never toggle
  `_step`; never step away from track 0; never release `_step` high; drive
  `direc` the wrong way; never step at all for the liveness proof), leave the
  expected value alone, confirm the assertion goes red, then revert.

## What happens next

Only after this file is committed: open `paula_floppy.v` and
`paula_floppy_fifo.v` for the exact module port list, clocking scheme and the
line numbers cited above, read the existing `hrm/paula_floppy` and
`hrm/ch8-ciaapra-address-window` packages for the build/harness conventions
this suite uses, and write the Verilator test harness. All of that reading —
and anything else opened beyond what is named above — will be disclosed in
the next commit.
