# ch8-disk-controller-overview — SPECIFICATION

The package carries the quotations in
`work/ch8-s2-build/ch8-disk-controller-overview.jsonl` (the 22 claims
assigned to this package). Every proposition and its expected colour is
derived from this claims file (`classify.evidence`, classification-stage
evidence) and the printed pages; the plumbing common to every
`paula_floppy` package (register-address convention, the
`Checks`/`mutating()`/`Rig` shape, the DSKLEN double-write idiom, the
fresh-model-per-trial discipline) follows the sibling packages on the same
fixture, `hrm/ch8-drive-sense-lines`, `hrm/ch8-drive-select-motor-led` and
`hrm/ch8-dsklen-dma-arming`. No expected value below is drawn from those
siblings' RTL findings.

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

## What this package is for

Printed p.241 opens the floppy section: how many drives the built-in
controller supports, what kind, what the controller is made of, and how the
disk subsystem is reached — structural propositions about the controller as
a whole, not about one register. Two claims (p.247) extend this to the
buffering/scheduling picture the chapter builds on p.241's foundation. Not
in scope: form factors, physical sizes, what is "typically" fitted, what is
installed in the basic unit — facts about a *product*, not a module. None of
the 22 claims' quotations names any such product-level fact; all 22 are
propositions about the controller module itself (drive count, DMA capacity,
register presence, port bit counts), so no claim in this package is declined
on that ground.

## Split content, deliberately excluded from this package's assertions

Two qualifiers inside this package's claims' quotations are explicitly split
off by the classify stage as *separate* propositions belonging to *other*
claims not in this package:

- **"MFM-type"** in `opus:8-dsk.c1`/`sol:8-dsk.c1` — the classify evidence
  states the media-format qualifier is carried by `opus:8-dsk.c10-c17` /
  `sol:8-dsk.c10-c20` (not in this package). The drive-count assertion
  grades the **count** only ("up to four ... devices"), not the "MFM-type"
  media qualifier.
- **"raw"** in `opus:8-dsk.c6`/`c7`/`sol:8-dsk.c7` — the classify evidence
  states the "unmediated medium bit cells" sense of "raw" is the same
  media-format family (`opus:8-dsk.c10-c17`), not this package's concern.
  The floppy data path terminates at `sdc_byte_in_data`/`sdc_byte_out_data`
  (an SD card, not a medium), so no port here can ever answer a raw-medium
  claim.

None of the 22 claims carries a `note` field restricting it to half of its
evidence, and none of the quotations mentions DSKDATR/DSKDAT or an
exclusivity word ("only") whose bare grading would overreach.

## Propositions

- **P-DRIVE-COUNT** — `opus:8-dsk.c1`, `sol:8-dsk.c1` (twins). The
  controller supports (up to) four drives — a structural property of the
  module's own select/target width, not of what medium is in a given drive.
  Graded: with `floppy_drives` at its maximum, each of the four `_sel` bits
  independently selects a working drive slot.
- **P-WHOLETRACK-DMA** — `opus:8-dsk.c6`, `opus:8-dsk.c7`, `sol:8-dsk.c7`
  (p.241, "It can DMA an entire track of raw MFM data into memory in a
  single disk revolution") and `sol:8-dskdma.c6` (p.247, "you need to read
  from the disk only once for the entire track"). Measured and printed, not
  graded — declined via `record()` for two independent reasons, either
  alone sufficient: the whole-track word count (5984) is derived from
  `paula_floppy.v`'s own sector-counter structure, not a figure the manual
  states; and this fixture never drives `sof` or observes an index-to-index
  interval, so a transfer in any number of revolutions would pass. The
  manual states neither a rotation speed nor a clock frequency (neither the
  s2 slice, pp.241-250, nor the full 687-page corpus does), so no
  real-time comparison replaces it. A sound assertion would drive `sof` on
  a real timebase and bracket the transfer between two consecutive `index`
  pulses.
- **P-DSKSYNC-MATCH** — `opus:8-dsk.c8`, `sol:8-dsk.c8` (twins). The
  dedicated sync register gates delivery on a specific programmed data
  pattern actually being matched, not merely on wordsync being enabled.
  Graded.
- **P-DSKBYTR-BYTE-READ** — `opus:8-dsk.c9`, `sol:8-dsk.c9` (twins). The
  dedicated byte-read register should present, at the instant a KNOWN
  incoming byte is current, that same byte. A known byte (0xFF) is bound to
  the DSKBYTR sampling instant through the DSKDATR/FIFO, the technique
  `ch8-mfm-cell-encoding` uses: the locator is the fixed
  `FIRST_PAYLOAD_POP_INDEX` (= 33), taken from `paula_floppy.v`'s own
  word-counter framing thresholds (:484-497), structurally independent of
  what any cell encodes to, so it cannot break when the encoder is fixed.
  The peek at that instant confirms the sampled word is the known byte's
  own encoding (0xFFFF on this RTL); a confirmation failure declines via
  `record()` rather than grading — 0xFFFF is not itself a page-legal MFM
  encoding of 0xFF (p.250 makes 0xFF → 0x5555), so a search for it would
  report a DATA-field defect against a conforming encoder that would not be
  there. THIS IS THE SAME UNDERLYING FACT as
  `ch8-dskbytr-status-register`/A9 and
  `ch8-disk-register-map-paula-floppy`/B4, graded against a known, bound
  byte in all three packages (A9 finds the payload index by search; B4 and
  this package use the fixed index). RED: measured `known_word=0xFFFF` (bound, at pop #33), DSKBYTR's
  DATA field reads 0x00 (`paula_floppy.v:699` hardwires bits 7:0 to 8'h00).
- **P-CIAAPRA-FOUR-BITS** — `opus:8-dsk.c21`, `sol:8-reglist.c4` (twins),
  and **P-CIAAPRA-SENSING** — `opus:8-dsk.c22`. Four independently-
  controllable sense signals, each driven by its own disk-sensing condition,
  reach paula_floppy's own ports. Measured and printed, not graded —
  declined via `record()`: this fixture never reaches CIAAPRA itself (the
  `minimig.v:622` join), so a severed or swapped join would pass.
  `hrm/ch8-cia-disk-signal-bindings`'s PA5/PA4/PA3/PA2-BINDING assertions
  grade the fact through a real CPU read of CIAAPRA ($BFE001).
- **P-CIABPRB-EIGHT-BITS** — `opus:8-dsk.c24`, `sol:8-reglist.c6`;
  **P-CIABPRB-SELECTION** — `opus:8-dsk.c25`, `sol:8-reglist.c7`;
  **P-CIABPRB-CONTROL** — `opus:8-dsk.c26`, `sol:8-reglist.c8`;
  **P-CIABPRB-STEPPING** — `opus:8-dsk.c27`, `sol:8-reglist.c9` (twins
  each). Eight independently-drivable control inputs (`_sel[3:0]`, `_step`,
  `direc`, `side`, `_motor`) reach paula_floppy, with selection, control and
  stepping functions. Measured and printed, not graded — declined via
  `record()`: this fixture never reaches CIABPRB itself (the `minimig.v:650`
  join). `hrm/ch8-cia-disk-signal-bindings`'s PB7-BINDING through
  PB0-BINDING assertions grade the fact through a real CPU write to CIABPRB
  ($BFD100).
- **P-STEP-DURING-DMA** — `sol:8-dskdma.c7` (p.247). Head stepping is
  accepted while a DMA transfer is in progress, not gated on the transfer
  being idle. Graded.

## Fixture boundary: `paula_floppy`

`paula_floppy.v` + `paula_floppy_fifo.v` only. `paula_floppy_ebr.v`
declares a **second** `module paula_floppy` and is not compiled alongside
it.

Every one of the eight CIAAPRA/CIABPRB claims' `classify.observable` field
says `"yes, at paula_floppy"`, and their `observable_detail` fields note
that the *specific CIA bit-position* mapping needs `ciaa`/`ciab`
(`minimig.v`), outside this package's module boundary. At this boundary the
count, independence and functional category of the floppy-side signals can
be measured, but not that they land at the named CIA bits; that join is
graded in `hrm/ch8-cia-disk-signal-bindings`.

## Rig hazards

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680`). A reused
  Verilated model would carry stale head position between trials. **A
  fresh `Vpaula_floppy` instance is allocated per trial** in this package —
  no instance is shared across two assertions or two mutation runs.
- **`paula_floppy_ebr.v` declares a second `module paula_floppy`.** Left out
  of the source list entirely, rather than compiled with `-Wno-fatal`.
- **DSKLEN needs the double write** (`paula_floppy.v:743`) to arm DMA:
  first write leaves `data_out`=0x9000, `fdd_led`=0; second gives
  `data_out`=0xD000, `fdd_led`=1, `sdc_rd`=1. A single 0xC00A leaves
  `dmas`=0. Every trial in this package that needs disk DMA armed performs
  the double write.

## Assertions and verdicts

All page 241 except `sol:8-dskdma.c6` and `sol:8-dskdma.c7` (page 247).

| check | uids | verdict |
|---|---|---|
| FIXTURE-LIVE | (none, apparatus) | `note()`, not graded |
| P-DRIVE-COUNT | `opus:8-dsk.c1`, `sol:8-dsk.c1` | GREEN |
| P-WHOLETRACK-DMA | `opus:8-dsk.c6`, `opus:8-dsk.c7`, `sol:8-dsk.c7`, `sol:8-dskdma.c6` | declined (`record()`) |
| P-DSKSYNC-MATCH | `opus:8-dsk.c8`, `sol:8-dsk.c8` | GREEN |
| P-DSKBYTR-BYTE-READ | `opus:8-dsk.c9`, `sol:8-dsk.c9` | **RED** (C6: DATA field hardwired 0x00) |
| DSKBYTR-CONTROL | (none, control case for the red above) | `note()`, not graded |
| P-CIAAPRA-FOUR-BITS | `opus:8-dsk.c21`, `sol:8-reglist.c4` | declined (`record()`) |
| P-CIAAPRA-SENSING | `opus:8-dsk.c22` | declined (`record()`) |
| P-CIABPRB-EIGHT-BITS | `opus:8-dsk.c24`, `sol:8-reglist.c6` | declined (`record()`) |
| P-CIABPRB-SELECTION | `opus:8-dsk.c25`, `sol:8-reglist.c7` | declined (`record()`) |
| P-CIABPRB-CONTROL | `opus:8-dsk.c26`, `sol:8-reglist.c8` | declined (`record()`) |
| P-CIABPRB-STEPPING | `opus:8-dsk.c27`, `sol:8-reglist.c9` | declined (`record()`) |
| P-STEP-DURING-DMA | `sol:8-dskdma.c7` | GREEN |

FIXTURE-LIVE and DSKBYTR-CONTROL carry no claim uid, so they are printed
via `note()` and never graded. DSKBYTR-CONTROL compares two readings for
inequality only, never the whole DSKBYTR word, whose p.278 layout marks
bits "X Not used" with no stated reset/idle value.

`opus:8-dsk.c9`'s own `classify.evidence` reports the measured result
P-DSKBYTR-BYTE-READ reproduces: "two consecutive reads of \$DFF01A both
return `data_out`=0x9000 ... the proposition is posed and answered at
`data_out`, and the answer is a divergence to record, not a reason to call
it untestable."

## Mutation plan

Each graded assertion has its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
record in the C++ source, `<label>` being the exact leading token of the
string literal passed to `expect()` — the claim's own `classify.uid`.

- Every assertion GREEN at baseline: a `stimulus red` record — change what
  the rig *asks* the module to do (a drive is never actually selected, the
  "should not match" DSKSYNC probe is programmed with the matching value,
  DMA is never armed), leave
  the expected value alone, confirm the targeted assertion alone goes red,
  then revert.
- P-DSKBYTR-BYTE-READ, RED at baseline: a stimulus mutation cannot
  demonstrate anything on an already-failing check — the mutation run
  substitutes DSKBYTR-CONTROL's mechanism (the WRITE-bit mirror, proven
  live in the same run) through the identical data_out-varies comparator,
  and flips GREEN.
- A `note()` or `record()` carries no mutation record.
