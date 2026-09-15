# ch8-disk-controller-overview — SPECIFICATION

Written from the quotations in
`work/ch8-s2-build/ch8-disk-controller-overview.jsonl` alone (the 22 claims
assigned to this package). This document is committed before any RTL file
(`$(NANOMIG)/src/minimig-aga/*.v`) is opened by me.

**Process note, disclosed up front rather than left implicit:** before writing
this file I read three already-built sibling packages on the same
`paula_floppy` fixture in full — `hrm/ch8-drive-sense-lines`,
`hrm/ch8-drive-select-motor-led` and `hrm/ch8-dsklen-dma-arming`
(`SPECIFICATION.md` and `.cpp` for each) — to see this suite's directory
layout, file-naming convention, and harness mechanics before creating my own.
Unlike the lighter precedent those packages themselves describe (reading only
a sibling's harness boilerplate), these three are fully built, and their
`.cpp` files carry `MECHANISM, confirmed by reading paula_floppy.v myself`
comment blocks with genuine RTL derivations — register address encoding
(`reg_address_in = addr>>1`, with `DSKLEN=9'h024`, `DSKBYTR=9'h01a`,
`DSKSYNC=9'h07e`, `DSKDATR=9'h008`), the `DSKBYTR` bit composition, the
`dskstate`/`fdd_led`/`dmaen` double-write mechanism, and the `motor_on`/
`index` latch. I did read that content, not only the harness boilerplate
around it, before writing this file — recorded exactly so it is not
mistaken for a clean read. What I am using from it below is **only**
plumbing common to every `paula_floppy` package (register-address
convention, the `Checks`/`mutating()`/`Rig` shape, the DSKLEN double-write
idiom, the fresh-model-per-trial discipline) that the build brief already
discloses independently (the DSKLEN double-write sequence and its measured
0x9000/0xD000/0xB000/0xF000 values are given verbatim in the build brief
itself, not discovered by me in a sibling's RTL trace). **No expected value
below for any of my own 22 claims is drawn from those siblings' RTL
findings** — every proposition and its expected colour is derived from this
package's own claims file (`classify.evidence`, which is itself
classification-stage-disclosed evidence, exactly as the sibling packages'
own specifications treat their own claims files) and from the build brief
and finding-candidates document handed to me for this unit. Where a sibling
package's RTL trace happens to cover the same signal my own claim's evidence
already names (e.g. `fdd_led`, `_ready`), I have used only my own claim's
evidence for the proposition — the sibling material is disclosed for
completeness, not cited as a source for any oracle value.

## Source claims (verbatim quotations)

All 22 claims are `disposition: new`, page 241 except the last two (p.247).

| uid | page | restatement | quote |
|---|---|---|---|
| `opus:8-dsk.c1` | 241 | The built-in disk controller supports a maximum of four MFM-type drives. | "The built-in disk controller in the system can handle up to four MFM-type\ndevices." |
| `sol:8-dsk.c1` | 241 | (twin of `opus:8-dsk.c1`, OCS-scoped) | same quote |
| `opus:8-dsk.c6` | 241 | The controller can DMA a whole track of raw MFM data into memory. | "It can DMA an entire track of raw\nMFM data into memory in a single disk revolution." |
| `opus:8-dsk.c7` | 241 | That whole-track transfer completes within a single disk revolution. | same quote as c6 |
| `sol:8-dsk.c7` | 241 | (twin of `opus:8-dsk.c6` + `opus:8-dsk.c7` combined — sol keeps them as one claim) | same quote |
| `opus:8-dsk.c8` | 241 | Dedicated registers let the CPU synchronize to specific data in the disk stream. | "Special registers allow\nthe CPU to synchronize with specific data, or read input a byte at a time." |
| `sol:8-dsk.c8` | 241 | (twin of `opus:8-dsk.c8`) | same quote |
| `opus:8-dsk.c9` | 241 | Dedicated registers let the CPU read the disk input one byte at a time. | same quote as c8 |
| `sol:8-dsk.c9` | 241 | (twin of `opus:8-dsk.c9`) | same quote |
| `opus:8-dsk.c21` | 241 | CIAAPRA contributes four bits to the disk subsystem and they are input bits. | "   CIAAPRA    (\$BFE001)    four input bits for disk sensing" |
| `sol:8-reglist.c4` | 241 | (twin of `opus:8-dsk.c21`) | same quote |
| `opus:8-dsk.c22` | 241 | Those four CIAAPRA input bits are used for disk sensing. | same quote as c21 |
| `opus:8-dsk.c24` | 241 | CIABPRB contributes eight bits to the disk subsystem and they are output bits. | "   CIABPRB    (\$BFD100)    eight output bits for disk selection, control\n                             and stepping" |
| `sol:8-reglist.c6` | 241 | (twin of `opus:8-dsk.c24`) | same quote |
| `opus:8-dsk.c25` | 241 | Those CIABPRB output bits serve disk (drive) selection. | same quote as c24 |
| `sol:8-reglist.c7` | 241 | (twin of `opus:8-dsk.c25`) | same quote |
| `opus:8-dsk.c26` | 241 | Those CIABPRB output bits serve disk control. | same quote as c24 |
| `sol:8-reglist.c8` | 241 | (twin of `opus:8-dsk.c26`) | same quote |
| `opus:8-dsk.c27` | 241 | Those CIABPRB output bits serve head stepping. | same quote as c24 |
| `sol:8-reglist.c9` | 241 | (twin of `opus:8-dsk.c27`) | same quote |
| `sol:8-dskdma.c6` | 247 | Whole-track buffering with DSKSYNC requires only one disk read for the track. | "With this process you need to read from the disk only once for the\nentire track." |
| `sol:8-dskdma.c7` | 247 | The drive can step to the next head while software processes and checksums the preceding track. | "In a high speed loader, the step to the next head can occur\nwhile the previous track is processed and checksummed." |

## What this package is for (build brief)

Printed p.241 opens the floppy section: how many drives the built-in
controller supports, what kind, what the controller is made of, and how the
disk subsystem is reached — structural propositions about the controller as
a whole, not about one register. Two claims (p.247) extend this to the
buffering/scheduling picture the chapter builds on p.241's foundation. What
is **not** in scope, per the build brief: form factors, physical sizes, what
is "typically" fitted, what is installed in the basic unit — facts about a
*product*, not a module. None of my 22 claims' quotations name any such
product-level fact; all 22 are propositions about the controller module
itself (drive count, DMA capacity, register presence, port bit counts), so
no claim in this package is declined on that ground. I checked, rather than
assumed, this for every one of the 22 quotes above.

## Split content, deliberately excluded from this package's assertions

Two qualifiers inside my own claims' quotations are explicitly split off by
the classify stage as *separate* propositions belonging to *other* claims
not in this package:

- **"MFM-type"** in `opus:8-dsk.c1`/`sol:8-dsk.c1` — the classify evidence
  states the media-format qualifier is carried by `opus:8-dsk.c10-c17` /
  `sol:8-dsk.c10-c20` (not in this package). My drive-count assertion grades
  the **count** only ("up to four ... devices"), not the "MFM-type" media
  qualifier.
- **"raw"** in `opus:8-dsk.c6`/`c7`/`sol:8-dsk.c7` — the classify evidence
  states the "unmediated medium bit cells" sense of "raw" is the same
  media-format family (`opus:8-dsk.c10-c17`), not this package's concern.
  My whole-track assertions grade an MFM **word stream** delivered by DMA,
  not any claim about medium-level bit cells. This lines up with finding
  candidates C1/C2 in `work/2026-09-15_ch8_s2_finding_candidates.md`: the
  floppy data path terminates at `sdc_byte_in_data`/`sdc_byte_out_data` (an
  SD card, not a medium), so no port here can ever answer a raw-medium
  claim — consistent with, not derived from, C1/C2.

I checked all 8 constraint-carrying claims named in the build brief
("partly-overturned", DSKDATR/DSKDAT rows, grade-only-the-half-the-evidence-
names) against my own 22 claims: **none of my 22 carries such a `note`
field**, and none of my quotations mentions DSKDATR/DSKDAT or an
exclusivity word ("only") whose bare grading would overreach. That
constraint therefore does not apply to any assertion in this package.

## Reduction to thirteen testable propositions

- **P-DRIVE-COUNT** — `opus:8-dsk.c1`, `sol:8-dsk.c1` (twins). The
  controller supports (up to) four drives — a structural property of the
  module's own select/target width, not of what medium is in a given drive.
- **P-WHOLETRACK-CAPACITY** — `opus:8-dsk.c6`, `sol:8-dsk.c7` (partial
  twin). A single DMA programming can move a whole-track-sized run of MFM
  words (larger than what one SD-sector-sized fetch alone would hold),
  completing as one continuous operation.
- **P-REVOLUTION-RATIO** — `opus:8-dsk.c7`, `sol:8-dsk.c7` (partial twin).
  That whole-track transfer's duration, measured in the module's own
  revolution-counting units, does not exceed one revolution's worth — a
  dimensionless ratio, not a millisecond figure.
- **P-DSKSYNC-MATCH** — `opus:8-dsk.c8`, `sol:8-dsk.c8` (twins). The
  dedicated sync register gates delivery on a specific programmed data
  pattern actually being matched, not merely on wordsync being enabled.
- **P-DSKBYTR-BYTE-READ** — `opus:8-dsk.c9`, `sol:8-dsk.c9` (twins). The
  dedicated byte-read register should present varying byte content as the
  disk stream varies.
- **P-CIAAPRA-FOUR-BITS** — `opus:8-dsk.c21`, `sol:8-reglist.c4` (twins).
  Exactly four independently-controllable sense signals reach the CIAAPRA
  side of the boundary from the floppy side (not fewer, not aliased).
- **P-CIAAPRA-SENSING** — `opus:8-dsk.c22`. Those four signals are each
  driven by a genuinely distinct disk-sensing condition (track position,
  media presence, selection/readiness, write-protect), not four copies of
  one condition under different names.
- **P-CIABPRB-EIGHT-BITS** — `opus:8-dsk.c24`, `sol:8-reglist.c6` (twins).
  Exactly eight independently-drivable control inputs reach paula_floppy
  from the CIABPRB side.
- **P-CIABPRB-SELECTION** — `opus:8-dsk.c25`, `sol:8-reglist.c7` (twins).
  Those bits include a drive-selection function: driving a different select
  pattern targets a different drive.
- **P-CIABPRB-CONTROL** — `opus:8-dsk.c26`, `sol:8-reglist.c8` (twins).
  Those bits include drive-control functions (motor being the case this
  package exercises).
- **P-CIABPRB-STEPPING** — `opus:8-dsk.c27`, `sol:8-reglist.c9` (twins).
  Those bits include a head-stepping function.
- **P-ONE-READ-PER-TRACK** — `sol:8-dskdma.c6` (p.247). A transfer spanning
  more data than a single SD-sector-sized fetch delivers is requested from
  the storage side as one continuous read operation, not as a series of
  separate reads.
- **P-STEP-DURING-DMA** — `sol:8-dskdma.c7` (p.247). Head stepping is
  accepted while a DMA transfer is in progress, not gated on the transfer
  being idle.

## Fixture boundary: `paula_floppy`, assigned not derived

The build brief assigns `paula_floppy` as this package's fixture:
`paula_floppy.v` + `paula_floppy_fifo.v` only. `paula_floppy_ebr.v` declares
a **second** `module paula_floppy` and must not be compiled alongside it —
disclosed by the build brief and independently by the finding-candidates
document (C1's rig-hazard section, "one more rig hazard").

The CIAAPRA/CIABPRB propositions (P-CIAAPRA-*, P-CIABPRB-*) are graded **at
paula_floppy's own ports**, not at `ciaa`/`ciab`/`minimig.v`'s wiring —
every one of the eight relevant claims' `classify.observable` field already
says `"yes, at paula_floppy"`, and their `observable_detail` fields note
that the *specific CIA bit-position* mapping needs `ciaa`/`ciab`
(`minimig.v`), which is outside this package's assigned module boundary.
This package therefore grades the **count, independence and functional
category** of the floppy-side signals that occupy those CIA bits (four
sense outputs, eight control inputs) — not their exact landing bit position
inside a physical CIAAPRA/CIABPRB register, which this fixture cannot
reach. This matches the package purpose stated in every one of my claims'
`package.package_purpose` field: "the CIA port-bit counts seen from the
floppy side."

## Rig hazards (disclosed by the build brief and finding-candidates
document, not derived by me from RTL)

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680`). A reused
  Verilated model would carry stale head position between trials. **A
  fresh `Vpaula_floppy` instance is allocated per trial** in this package —
  no instance is shared across two assertions or two mutation runs.
- **`paula_floppy_ebr.v` declares a second `module paula_floppy`.** Left out
  of the source list entirely, per the build brief's stated preference over
  `-Wno-fatal`.
- **DSKLEN needs the double write** (`paula_floppy.v:743`) to arm DMA,
  measured end to end by the build brief: first write leaves
  `data_out`=0x9000, `fdd_led`=0; second gives `data_out`=0xD000,
  `fdd_led`=1, `sdc_rd`=1. A single 0xC00A leaves `dmas`=0. Every assertion
  in this package that needs disk DMA armed performs the double write.

## Assertions — one per proposition, each naming its page

All page 241 except P-ONE-READ-PER-TRACK and P-STEP-DURING-DMA (page 247).

| assertion | uids | expected |
|---|---|---|
| FIXTURE-LIVE | (none, apparatus) | GREEN |
| P-DRIVE-COUNT | `opus:8-dsk.c1`, `sol:8-dsk.c1` | GREEN |
| P-WHOLETRACK-CAPACITY | `opus:8-dsk.c6`, `sol:8-dsk.c7` | GREEN (to be measured) |
| P-REVOLUTION-RATIO | `opus:8-dsk.c7`, `sol:8-dsk.c7` | GREEN (to be measured) |
| P-DSKSYNC-MATCH | `opus:8-dsk.c8`, `sol:8-dsk.c8` | GREEN |
| P-DSKBYTR-BYTE-READ | `opus:8-dsk.c9`, `sol:8-dsk.c9` | **RED** (C6: DATA field measured hardwired 0x00 in this package's own claims-file evidence) |
| DSKBYTR-CONTROL | (none, control case for the red above) | GREEN |
| P-CIAAPRA-FOUR-BITS | `opus:8-dsk.c21`, `sol:8-reglist.c4` | GREEN |
| P-CIAAPRA-SENSING | `opus:8-dsk.c22` | GREEN |
| P-CIABPRB-EIGHT-BITS | `opus:8-dsk.c24`, `sol:8-reglist.c6` | GREEN |
| P-CIABPRB-SELECTION | `opus:8-dsk.c25`, `sol:8-reglist.c7` | GREEN |
| P-CIABPRB-CONTROL | `opus:8-dsk.c26`, `sol:8-reglist.c8` | GREEN |
| P-CIABPRB-STEPPING | `opus:8-dsk.c27`, `sol:8-reglist.c9` | GREEN |
| P-ONE-READ-PER-TRACK | `sol:8-dskdma.c6` | to be measured (RTL's `sdc_rd` protocol across a multi-sector transfer is not yet known to me) |
| P-STEP-DURING-DMA | `sol:8-dskdma.c7` | GREEN |

`opus:8-dsk.c9`'s own `classify.evidence` already reports the measured
result this package's P-DSKBYTR-BYTE-READ assertion is expected to
reproduce: "two consecutive reads of \$DFF01A both return `data_out`=0x9000
... the proposition is posed and answered at `data_out`, and the answer is
a divergence to record, not a reason to call it untestable." That measured
divergence is why P-DSKBYTR-BYTE-READ is listed RED above, before any RTL
of my own has been opened — it comes from my claims file's own disclosed
evidence, exactly as C4 was disclosed to `ch8-drive-sense-lines` before that
package's author opened RTL.

P-WHOLETRACK-CAPACITY, P-REVOLUTION-RATIO and P-ONE-READ-PER-TRACK are
marked "to be measured": nothing in my 22 claims' own evidence, the build
brief, or the finding-candidates document states a definite expected colour
for these three, unlike P-DSKBYTR-BYTE-READ. I am not guessing a colour for
them here — the RTL is opened next specifically to find out, and each will
be reported with whatever colour the run actually produces.

## Mutation plan

Each assertion above gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
record in the C++ source, `<label>` being the exact leading token of the
string literal passed to `expect()` — the claim's own `classify.uid` where
one exists, a plain uppercase id for FIXTURE-LIVE and DSKBYTR-CONTROL.

- Every assertion expected **GREEN** at baseline: a `stimulus red` record —
  change what the rig *asks* the module to do (a different drive stays
  selected than the one the loop is checking, the double write is reduced
  to a single write, the sync value fed no longer matches DSKSYNC, the
  qualifying step edge is skipped, etc.), leave the expected value alone,
  confirm the targeted assertion alone goes red, then revert.
- P-DSKBYTR-BYTE-READ, expected **RED**: a stimulus mutation cannot
  demonstrate anything on an already-failing check (the substitute rule
  applies) — the mutation run substitutes DSKBYTR-CONTROL's mechanism (the
  WRITE-bit mirror, proven live and green in the same run) through the
  identical data_out-varies comparator, expected to flip GREEN.
- P-WHOLETRACK-CAPACITY / P-REVOLUTION-RATIO / P-ONE-READ-PER-TRACK: mutation
  form fixed once their baseline colour is known from the run (ordinary
  stimulus-red if green at baseline; substitute if red at baseline, per the
  same rule).

Which exact stimulus each mutation changes, and the measured before/after
pass/fail tally, is recorded in the C++ source once the package is built and
run — this section states the plan, not a claim that any run has happened.

## What happens next

Only after this file is committed: open `paula_floppy.v` in full (register
address parameters, the `_sel`/drive-count decode, DSKSYNC/`wordsync`
gating, DSKBYTR's construction, the CIAAPRA/CIABPRB-side port list, the
`sdc_rd` protocol across a multi-word/multi-sector transfer, the revolution
counter) and `paula_floppy_fifo.v` in full; then write this package's own
Makefile and test source, resolving the three "to be measured" rows above
to whatever the RTL and a run of it actually show. All of that reading, and
anything else opened beyond what is named here, will be disclosed in the
next commit's file header.
