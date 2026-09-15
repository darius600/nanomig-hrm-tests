# ch8-drive-sense-lines — SPECIFICATION

Written from the quotations in `work/ch8-s2-build/ch8-drive-sense-lines.jsonl`
alone (the 10 claims assigned to this package). No RTL file
(`$(NANOMIG)/src/minimig-aga/*.v`) has been opened by me at this stage. This
document is committed before any RTL is opened.

**Process note, disclosed up front rather than left implicit:** before writing
this file I looked at two already-built sibling packages on the same fixture —
`hrm/ch8-drive-select-motor-led` (`SPECIFICATION.md` and its `.cpp`) and
`hrm/ch8-drive-step-timing` (`SPECIFICATION.md` and its `.cpp`) — to see this
suite's directory layout and file-naming convention before creating my own.
Both are prior HRM packages, not RTL. What I took from them is build/harness
*mechanics* common to every `paula_floppy` package (a `clk`/`clk7_en` tick
pattern, a `Checks` class with `expect`/`note`/`record`, a `mutating(id)`
helper keyed on a `MUTATE` env var, the fixed `// MUTATION` comment form) —
not any conformance fact about the four sense lines this package grades. Every
RTL line number, signal name, and mechanism claim appearing below comes from
my own claims file's `classify.evidence` fields and the build brief handed to
me for this unit, exactly as the sibling packages' own specifications did
before their authors opened RTL. I have not opened `paula_floppy.v` or
`paula_floppy_fifo.v` myself yet.

## Source claims (verbatim quotations, all page 244)

| uid | restatement | quote |
|---|---|---|
| `opus:8-cia.c7` | DSKRDY* is active low. | "PA5 DSKRDY* Disk ready (active low). The drive will pull this line low when the motor is known to be rotating at full speed. This signal is only valid when the motor is ON, at other times configuration information may obscure the meaning of this input." |
| `opus:8-cia.c12` | The drive pulls DSKTRACK0* low when the heads are positioned over track zero. | "PA4 DSKTRACK0* Track zero detect. The drive will pull this line low when the disk heads are positioned over track zero. Software must not attempt to step outwards when this signal is active. Some drives will refuse to step, others will attempt the step, possibly causing alignment damage. All new drives must refuse to step outward in this condition." |
| `sol:8-cia.c10` | The drive pulls DSKTRACK0* low when its heads are over track zero. (twin of `opus:8-cia.c12`) | "The drive will pull this line low when the disk heads are positioned over track zero." |
| `opus:8-cia.c15` | All new drives are required to refuse an outward step while DSKTRACK0* is active. | "Software must not attempt to step outwards when this signal is active. ... All new drives must refuse to step outward in this condition." |
| `sol:8-cia.c15` | New drives are required to refuse outward stepping while at track zero. (twin of `opus:8-cia.c15`) | "All new drives must refuse to step outward in this condition." |
| `opus:8-cia.c17` | DSKPROT* is active low. | "PA3 DSKPROT* Disk is write protected (active low)." |
| `opus:8-cia.c19` | DSKCHANGE* goes low whenever a disk is removed. | "PA2 DSKCHANGE* Disk has been removed from the drive. The signal goes low whenever a disk is removed. It remains low until a disk is inserted AND a step pulse is received." |
| `sol:8-cia.c18` | DSKCHANGE* goes low whenever a disk is removed. (twin of `opus:8-cia.c19`) | "The signal goes low whenever a disk is removed." |
| `opus:8-cia.c20` | DSKCHANGE* stays low until both a disk is inserted and a step pulse is received. | "It remains low until a disk is inserted AND a step pulse is received." |
| `sol:8-cia.c19` | DSKCHANGE* remains low until both disk insertion and receipt of a step pulse have occurred. (twin of `opus:8-cia.c20`) | "It remains low until a disk is inserted AND a step pulse is received." |

All 10 are `disposition: new` and carry `observable: needs a wider boundary` —
each claim's `classify.evidence` already explains that the drive-side half of
these signals is not at `ciaa`/`ciab` (generic 8520 ports, measured by the
classify stage to pass every bit through unaltered and add no polarity, latch,
or interpretation of their own) but at `paula_floppy`, where NanoMig actually
models the drive.

## Reduction to six testable propositions

- **P-READY** — `opus:8-cia.c7`. DSKRDY* polarity and its stated precondition
  (valid, pulled low, only when the motor is at full speed).
- **P-TRACK0-ZERO** — `opus:8-cia.c12`, `sol:8-cia.c10` (twins). DSKTRACK0*
  pulled low when the selected drive's head position is at track zero.
- **P-TRACK0-REFUSE-OUTWARD** — `opus:8-cia.c15`, `sol:8-cia.c15` (twins). An
  outward step is refused while at track zero.
- **P-WPROT** — `opus:8-cia.c17`. DSKPROT* polarity (active low, driven by
  the write-protect condition).
- **P-CHANGE-ON-REMOVE** — `opus:8-cia.c19`, `sol:8-cia.c18` (twins).
  DSKCHANGE* asserted whenever the disk is removed.
- **P-CHANGE-LATCH** — `opus:8-cia.c20`, `sol:8-cia.c19` (twins). DSKCHANGE*
  stays asserted until *both* a disk is reinserted *and* a step pulse is
  received — a compound condition, not insertion alone.

## This package's subject, named by the build brief: C4, C5, C17

Three finding candidates from `work/2026-09-15_ch8_s2_finding_candidates.md`
are this package's specific subject, disclosed to me (not derived by me from
RTL) as follows:

- **C4** (grades `opus:8-cia.c7` / P-READY): the build brief states
  `paula_floppy.v:688-694` computes `_ready` from `_sel` and the configured
  drive count only, and the module's own comment marks the motor/disk
  condition "(not implemented)".
  **REPAIRED 2026-09-15.** The original revision of this document (and the
  package built from it) graded P-READY as expected RED. Independent
  validation found that wrong: p.244, three lines below the sentence this
  claim cites, reads "This signal is only valid when the motor is ON, at
  other times configuration information may obscure the meaning of this
  input." The page itself declines to say what `_ready` means with the
  motor off, so a check requiring a particular value there tests a case the
  page expressly refuses to define — more than the page states. **P-READY
  is now DECLINED via `record()`, not graded through `expect()`.** The RTL
  fact this claim was built on (the motor term is genuinely absent from
  `_ready`'s cone, confirmed by reading `paula_floppy.v:688-694`, and the
  module's own 2008-09-24 changelog documents this as a deliberate
  compatibility choice) is kept as a measured `note()`, not a finding
  against this page.
- **C5** (no claim in this package's 10 cites the timing figure this
  candidate is about): the build brief states `_track0` is purely
  combinational (`paula_floppy.v:666,683`), so its latency is 0 ticks and
  nothing in the module bounds the figure's printed "4 ms max" to TRACK 00.
  None of my 10 claims quotes that figure, so there is no `expect()` to grade
  against it — but I will **measure** the tick-latency of `_track0` responding
  to a step that crosses the track-zero boundary and report it via `note()`
  (ungraded), since the build brief names C5 as part of this package's
  subject even though no citable assertion exists for it in my claims file.
- **C17** (a rig hazard for P-TRACK0-ZERO and P-TRACK0-REFUSE-OUTWARD,
  disclosed by the build brief, not by any of my 10 claims): `_track0` is
  **deasserted whenever the drive is deselected, regardless of head
  position** (`paula_floppy.v:666`). Every read of `_track0` below selects the
  drive first — reading it deselected reads the mask, not the track-zero
  state. Because this masking is a real, measured property of the port that
  the page does not describe, it gets its own assertion
  (`SEL-MASKS-TRACK0` below) even though it carries no page citation.

## Also disclosed, out of scope for this package's 10 claims

`index` is synthesised from `sof` (`paula_floppy.v:576` per the build brief)
— there is no sensor input, so a claim that the index pulse is caused by a
physical hole would have no observable here. None of this package's 10 claims
is about `index`; nothing is being declined here because nothing was asked.
Noted only so the absence is not mistaken for an oversight.

## Fixture boundary: `paula_floppy`, assigned not derived

The build brief assigns `paula_floppy` as this package's fixture, compiling
`paula_floppy.v` + `paula_floppy_fifo.v` only — `paula_floppy_ebr.v` declares
a second `module paula_floppy` and must not be compiled alongside it. All 10
claims' `classify.evidence` independently confirms the same boundary: `ciaa`
is measured (by the classify stage, not by me) to be a transparent buffer
that adds no polarity, latch, or interpretation of its own to any of these
four sense bits, so the entire testable content of each claim lives inside
`paula_floppy`.

## Rig hazards (disclosed by the build brief, not derived by me from RTL)

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680`). A reused
  Verilated model would carry stale head position between trials. **A fresh
  `Vpaula_floppy` instance is allocated per trial** in this package.
- **`paula_floppy_ebr.v` declares a second `module paula_floppy`.** Left out
  of the source list entirely (not warning-suppressed).
- **C17, above**: `_track0` reads the deselected mask, not head position,
  whenever no drive is selected. Every probe selects first.

## Assertions — one per citing quotation, plus what this suite's own rules require

All 10 claim-based assertions are page 244.

- **A1** — `opus:8-cia.c7` (P-READY). **DECLINED via `record()` (repaired
  2026-09-15, was C4)** — see the repair note above; not graded through
  `expect()`, no mutation record.
- **A2** — `opus:8-cia.c12` (P-TRACK0-ZERO). **Expected GREEN.**
- **A3** — `sol:8-cia.c10` (P-TRACK0-ZERO, twin of A2). **Expected GREEN.**
- **A4** — `opus:8-cia.c15` (P-TRACK0-REFUSE-OUTWARD). **Expected GREEN.**
- **A5** — `sol:8-cia.c15` (P-TRACK0-REFUSE-OUTWARD, twin of A4). **Expected
  GREEN.**
- **A6** — `opus:8-cia.c17` (P-WPROT). **Expected GREEN.**
- **A7** — `opus:8-cia.c19` (P-CHANGE-ON-REMOVE). **Expected GREEN.**
- **A8** — `sol:8-cia.c18` (P-CHANGE-ON-REMOVE, twin of A7). **Expected
  GREEN.**
- **A9** — `opus:8-cia.c20` (P-CHANGE-LATCH). **Expected GREEN.**
- **A10** — `sol:8-cia.c19` (P-CHANGE-LATCH, twin of A9). **Expected GREEN.**

Plus, ungraded by any manual citation but required by this suite's own rules
(per the build brief: "per assertion" includes the fixture-liveness proof and
the control case; a real property the page does not describe, like C17's
masking, still gets graded if it is asserted through `expect()`):

- **FIXTURE-LIVE** — the sense-line output path (the shared per-signal
  `&(_sel-term | internal-condition)` output mux that `classify.evidence`
  describes for these ports) must be shown to deliver two distinct, known,
  correct values before any claim-based read above is trusted. Measured on
  `_wprot` (the simplest single-cycle case): `floppy_wrprot` deasserted then
  asserted, drive selected throughout, two distinct correct `_wprot` readings.
- **READY-SEL-CONTROL** — this package's mandatory control case, on the
  *same* `_ready` port/comparator as A1 (the suspect C4 finding): deselecting
  every drive is expected to deassert `_ready`, and selecting a configured
  drive is expected to assert it. Expected GREEN — proof that `_ready` is a
  live signal that does respond to something real (selection), so A1's red
  is a meaningful finding about the motor condition specifically, not an
  artifact of a stuck or dead port.
- **SEL-MASKS-TRACK0** — C17's masking property, real but not printed on the
  page: with the selected drive's head parked at track zero (so the digital
  "at zero" condition A2–A5 grade is true), deselecting that drive is
  expected to read `_track0` deasserted anyway, and reselecting it is
  expected to read it reasserted. Expected GREEN (this is what the RTL does;
  nothing on the page contradicts it, so it is a measured structural fact,
  not a finding).

## Mutation plan

Each assertion above gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
record in the C++ source, `<label>` being the exact leading token of the
string literal passed to `expect()` — the claim's own `classify.uid` for
A1–A10, a plain uppercase id for FIXTURE-LIVE / READY-SEL-CONTROL /
SEL-MASKS-TRACK0.

- **A2–A10, FIXTURE-LIVE, READY-SEL-CONTROL, SEL-MASKS-TRACK0** (all expected
  GREEN at baseline): a `stimulus red` record each — change what the rig
  *asks* the module to do (skip the step that crosses track zero, never
  assert `floppy_wrprot`, never drop `sdc_image_size`, never send the step
  pulse after reinsertion, never deselect, etc.), leave the expected value
  alone, confirm the targeted assertion alone goes red, then revert.
- **A1** — **repaired 2026-09-15: no longer an `expect()` assertion, so no
  mutation record.** The original plan treated this as expected-RED and
  applied the substitute rule (a different live mechanism through the same
  comparator). That plan is withdrawn along with the graded check itself:
  P-READY is a decline (the page names no value for the motor-off case), and
  `record()` lines carry no mutation record per this suite's own rule.
  READY-SEL-CONTROL (above) remains, unchanged, as the live proof that the
  `_ready` port itself is not dead or stuck.

Which exact stimulus each mutation changes, and the measured before/after
pass/fail tally, is recorded in the C++ source once the package is built and
run — this section states the plan, not a claim that any run has happened.

## What happens next

Only after this file is committed: open `paula_floppy.v` (the ports block;
the exact `_ready`, `_track0`, `_wprot`, `_change` equations at the line
ranges disclosed above; the `dsktrack` update logic; the `_disk_change` latch
equation) and `paula_floppy_fifo.v` in full; then write this package's own
Makefile and test source. All of that reading, and anything else opened
beyond what is named here, will be disclosed in the next commit's file
header.
