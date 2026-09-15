# ch8-dsklen-dma-arming — SPECIFICATION

Written from the quotations in `ch8-dsklen-dma-arming.jsonl` alone (the 54
claims assigned to this package by
`work/ch8-s2-build/ch8-dsklen-dma-arming.jsonl`). No RTL file has been opened
by me yet. This document is committed before any of that happens.

Fixture assigned by the build brief: `paula_floppy` (`paula_floppy.v` +
`paula_floppy_fifo.v`; `paula_floppy_ebr.v` excluded — it declares a second
`module paula_floppy` and is not part of this fixture). I have not opened any
of these three files.

## Why 54 claims reduce to a handful of testable propositions

All 54 claims quote one of three sources: page 246 (the "four items"
paragraph and the DSKLEN bit table), page 247 (the five-step arming sequence,
the decrement/stop paragraph, and the two hardware-bug sentences), or page 279
(Appendix A's DSKLEN and DSKDAT entries, which restate the same bit table and
the same enable/direction/length facts in different words). Across those three
sources the manual repeats the same handful of facts many times — once as a
list item, once as a bit-table row, once as a numbered step, once as an
appendix paragraph. Each repetition is still its own claim (its own reader,
its own uid) and gets graded, but where two or more claims reduce to the
*identical* testable proposition, they cite the *same* assertion, exactly as
this suite's existing packages do for twin readings (e.g.
`hrm/ch8-ciaapra-address-window`, `hrm/ch8-drive-select-motor-led`). No
proposition is graded by more than one assertion in this package, and no
assertion grades more than one proposition.

## Assertions — one per proposition, each naming every citing uid and page

### A0 — FIXTURE_LIVENESS (no manual citation)

Not a page claim. Per the build brief: "The fixture proves it can deliver a
known word before anything reads one." Before any DSKLEN-based assertion is
trusted, drive a distinct, known data value through this fixture's data path
and confirm it arrives unmodified and distinguishable from a second, different
known value. Required as its own graded, mutation-recorded item.

### A1 — DSKLEN_WRITE_ONLY

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c12` | 246 | "All of the control bits relating to this topic are contained in a\nwrite-only register, called DSKLEN:" |

Proposition: DSKLEN is a write-only register — reading back at its own address
does not return the bits just written to it.

### A2 — DSKLEN_TWO_CONTROL_BITS

| uid | page | quote |
|---|---|---|
| `sol:8-appa-dsklen.c6` | 279 | "It also contains two control\n                 bits, a DMA enable bit, and a DMA direction\n                 (read/write) bit." |

Proposition: besides the LENGTH field, DSKLEN carries exactly two further,
independently-settable single-bit controls (enable, direction) — distinct
from each other and from the length count.

### A3 — DSKLEN_ENABLE_ITEM_AND_DMACON_GATE

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c2` | 246 | "The disk DMA is controlled by four items:" |
| `opus:8-dma.c20` | 247 | "   1. Enable disk DMA in the  DMACON  register (See Chapter 7 for more\n      information)" |
| `sol:8-dsklen.c9` | 247 | "   1. Enable disk DMA in the  DMACON  register (See Chapter 7 for more\n      information)" |

Proposition: of the items said to control disk DMA, the DMACON-derived enable
is one — until it is asserted, a fully-programmed and twice-written DSKLEN
does not start a transfer; asserting it (with nothing else changed) does. This
package's fixture boundary (`paula_floppy`) carries the DMACON-derived signal
as a single input port (per this claims file's own `classify.evidence`,
disclosed before this document, not yet independently read from RTL); DMACON's
own bit decode is out of scope (Chapter 7 / a different module) and is not
graded here. `opus:8-dma.c2`'s "four items" also names the DMA pointer, which
lives in a different register pair reached through a different module
(`agnus_diskdma`, not `paula_floppy`); this package does not grade the pointer
half of that count — only the enable-gating fact this claims file's own
evidence measures at this fixture.

### A4 — DSKLEN_FORCE_OFF_FROM_IDLE

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c21` | 247 | "   2. Set DSKLEN to $4000, thereby forcing the DMA for the disk to be\n      turned off." |
| `sol:8-dsklen.c10` | 247 | "   2. Set DSKLEN to $4000, thereby forcing the DMA for the disk to be\n      turned off." |

Proposition: writing $4000 to DSKLEN, from an idle/unarmed state, forces disk
DMA off (leaves it not running).

### A5 — DSKLEN_FIRST_WRITE_LOADS_DOES_NOT_START

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c17` | 247 | "The hardware requires a special sequence in order to start DMA to the\ndisk." |
| `sol:8-dsklen.c6` | 247 | "The hardware requires a special sequence in order to start DMA to the\ndisk." |
| `opus:8-dma.c22` | 247 | "   3. Put the value you want into the DSKLEN register." |
| `sol:8-dsklen.c11` | 247 | "   3. Put the value you want into the DSKLEN register." |

Proposition: the first write of a desired DSKLEN value loads it (length,
direction, enable-latch) but does not by itself start DMA — demonstrating that
a special (multi-step) sequence, not a single write, is required.

### A6 — DSKLEN_SECOND_WRITE_ARMS_DMAEN

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c6` | 246 | "   *  DMA enable" |
| `sol:8-dskdma.c5` | 246 | "   *  DMA enable" |
| `opus:8-dma.c14` | 246 | "       15      DMAEN   Secondary disk DMA enable" |
| `sol:8-dsklen.c3` | 246 | "       15      DMAEN   Secondary disk DMA enable" |
| `opus:8-appA-dsklen.c6` | 279 | "                 This register contains the length (number of words)\n                 of disk DMA data. It also contains two control\n                 bits, a DMA enable bit, and a DMA direction\n                 (read/write) bit." |
| `sol:8-appa-dsklen.c7` | 279 | "It also contains two control\n                 bits, a DMA enable bit, and a DMA direction\n                 (read/write) bit." |
| `opus:8-appA-dsklen.c8` | 279 | "                 15    DMAEN         Disk DMA enable" |
| `sol:8-appa-dsklen.c9` | 279 | "                 15    DMAEN         Disk DMA enable" |
| `opus:8-dma.c19` | 247 | "In short, the DMAEN bit in the DSKLEN register must be turned on twice\nin order to actually enable the disk DMA hardware." |
| `sol:8-dsklen.c8` | 247 | "In short, the DMAEN bit in the DSKLEN register must be turned on twice\nin order to actually enable the disk DMA hardware." |
| `opus:8-dma.c23` | 247 | "   4. Write this value again into the DSKLEN register. This actually\n      starts the DMA." |
| `sol:8-dsklen.c12` | 247 | "   4. Write this value again into the DSKLEN register. This actually\n      starts the DMA." |

Proposition: DSKLEN bit 15 is DMAEN, the DMA-enable control; it is a
*secondary* enable that only takes effect on the second of two consecutive
writes with the bit set — the second write (repeating the same value) is what
actually starts DMA. All twelve quotations above (the enable list-item, the
bit-table row printed twice, the appendix paragraph and bit row, the "must be
turned on twice" sentence, and step 4) describe this one mechanism.

### A7 — DSKLEN_FORCE_OFF_DURING_ACTIVE_TRANSFER

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c24` | 247 | "   5. After the DMA is complete, set the DSKLEN register back to\n      $4000, to prevent accidental writes to the disk." |
| `sol:8-dsklen.c13` | 247 | "   5. After the DMA is complete, set the DSKLEN register back to\n      $4000, to prevent accidental writes to the disk." |

Proposition: writing $4000 to DSKLEN *while a transfer is active* also forces
DMA off, the same as it does from idle (A4) — distinct precondition, same
prescribed value, guarding against accidental writes after a transfer
completes.

### A8 — DSKLEN_WRITE_DIRECTION_BIT_AND_ACCIDENTAL_WRITE_PREVENTION

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c5` | 246 | "   *  Direction of data transfer (read/write)" |
| `sol:8-dskdma.c4` | 246 | "   *  Direction of data transfer (read/write)" |
| `opus:8-dma.c15` | 246 | "       14      WRITE   Disk write (RAM -> disk if 1)" |
| `sol:8-dsklen.c4` | 246 | "       14      WRITE   Disk write (RAM -> disk if 1)" |
| `opus:8-appA-dskdat.c12` | 279 | "                 This register is the disk DMA data buffer. It\n                 contains two bytes of data that are either sent\n                 (written) to or received (read) from the disk.\n                 The write mode is enabled by bit 14 of the LENGTH\n                 register. The DMA controller automatically\n                 transfers data to or from this register and RAM,\n                 and when the DMA data is finished (length=0) it\n                 causes a disk block interrupt. See interrupts below." |
| `sol:8-appa-dskdata.c5` | 279 | "                 The write mode is enabled by bit 14 of the LENGTH\n                 register." |
| `opus:8-appA-dsklen.c7` | 279 | "                 This register contains the length (number of words)\n                 of disk DMA data. It also contains two control\n                 bits, a DMA enable bit, and a DMA direction\n                 (read/write) bit." |
| `sol:8-appa-dsklen.c8` | 279 | "It also contains two control\n                 bits, a DMA enable bit, and a DMA direction\n                 (read/write) bit." |
| `opus:8-appA-dsklen.c9` | 279 | "                 14    WRITE         Disk write (RAM to disk) if 1" |
| `sol:8-appa-dsklen.c10` | 279 | "                 14    WRITE         Disk write (RAM to disk) if 1" |
| `opus:8-dma.c18` | 247 | "This sequence prevents accidental writes to the disk." |
| `sol:8-dsklen.c7` | 247 | "This sequence prevents accidental writes to the disk." |

Proposition: DSKLEN bit 14 is WRITE, selecting RAM-to-disk direction when set
(read-from-disk otherwise), and is one of the two control bits the appendix
describes; and the two-write arming sequence (A5/A6) is specifically what
prevents a single stray write — even one with WRITE and DMAEN both set — from
starting an accidental disk write.

### A9 — DSKLEN_LENGTH_FIELD_COUNTS_WORDS (control case: expected PASS)

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c4` | 246 | "   *  Length of data to be moved by DMA" |
| `sol:8-dskdma.c3` | 246 | "   *  Length of data to be moved by DMA" |
| `opus:8-dma.c16` | 246 | "       13-0    LENGTH  Number of words to transfer" |
| `sol:8-dsklen.c5` | 246 | "       13-0    LENGTH  Number of words to transfer" |
| `opus:8-appA-dsklen.c5` | 279 | "                 This register contains the length (number of words)\n                 of disk DMA data. It also contains two control\n                 bits, a DMA enable bit, and a DMA direction\n                 (read/write) bit." |
| `sol:8-appa-dsklen.c5` | 279 | "                 This register contains the length (number of words)\n                 of disk DMA data." |
| `opus:8-appA-dsklen.c10` | 279 | "                 13-0  LENGTH        Length (# of words) of DMA data." |
| `sol:8-appa-dsklen.c11` | 279 | "                 13-0  LENGTH        Length (# of words) of DMA data." |

Proposition: DSKLEN bits 13-0 are LENGTH, the number of words to transfer —
programming a specific word count and running a transfer delivers exactly that
many words. This is the package's control case: a straightforward, unexcep-
tional reading of the page that is expected to hold, on the same measurement
basis (word-count-in against word-count-out) used by the more contested
assertions.

### A10 — DSKLEN_LENGTH_DECREMENTS_AND_TRANSFER_STOPS_AT_ZERO

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c25` | 247 | "As each data word is transferred, the length value is decremented." |
| `sol:8-dsklen.c14` | 247 | "As each data word is transferred, the length value is decremented." |
| `opus:8-dma.c28` | 247 | "When the length\nvalue counts down to 0, the transfer stops." |
| `sol:8-dsklen.c17` | 247 | "When the length\nvalue counts down to 0, the transfer stops." |
| `opus:8-appA-dskdat.c14` | 279 | "                 This register is the disk DMA data buffer. It\n                 contains two bytes of data that are either sent\n                 (written) to or received (read) from the disk.\n                 The write mode is enabled by bit 14 of the LENGTH\n                 register. The DMA controller automatically\n                 transfers data to or from this register and RAM,\n                 and when the DMA data is finished (length=0) it\n                 causes a disk block interrupt. See interrupts below." |

Proposition: the length counter decrements once per word transferred, and the
transfer stops (raising the disk-block interrupt) exactly when it reaches
zero — demonstrated by programming a length *shorter* than the available
source data and confirming the transfer stops early, at the programmed count,
not at the end of the source data.

### A11 — DSKBUG_LAST_THREE_BITS_LOST_ON_WRITE (documented hardware bug)

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c32` | 247 | "There is a hardware bug that causes the last three bits of data sent to\nthe disk to be lost." |
| `sol:8-dskbug.c1` | 247 | "There is a hardware bug that causes the last three bits of data sent to\nthe disk to be lost." |

Proposition, as printed: a documented Amiga hardware bug causes the last three
bits of data sent to disk during a write transfer to be lost. Graded literally
against NanoMig: does a known write-direction transfer's outgoing byte stream
lose its last three bits versus the source data? If NanoMig's write path
reproduces the bug this assertion is green; if NanoMig carries the full byte
through unmodified (a real possibility flagged by this package's own claims
file, which had not yet run the full transfer when it classified this claim
testable), the assertion is red, and that red is the deliverable, not a defect
in the test.

### A12 — DSKBUG_ONE_WORD_SHORT_ON_READ (documented hardware bug)

| uid | page | quote |
|---|---|---|
| `opus:8-dma.c33` | 247 | "Also, the last word in a disk-read DMA operation may\nnot come in (that is, one less word may be read than you asked for)." |
| `sol:8-dskbug.c2` | 247 | "Also, the last word in a disk-read DMA operation may\nnot come in (that is, one less word may be read than you asked for)." |

Proposition, as printed: a documented Amiga hardware bug can cause a
disk-read DMA to deliver one word fewer than programmed.

**REPAIRED 2026-09-15, DECLINED — not graded.** The original plan above
(quoted for the record) graded this literally as `delivered == programmed-1`
and expected RED as the deliverable. Independent validation found that wrong:
the page says the last word **may** not come in — "may" is a possibility, not
a promise, and a red requires the page to demand something a single
deterministic trial can violate. This one demands nothing, so the graded
check tested more than the page states. **Declined via `record()`, quoting
the page's own "may" wording.** The measured fact — this fixture's fresh
trial delivers the FULL programmed count, matching A9/A10's own independent
exact-count measurement — is kept as a `note()`: real and interesting, but a
fact about NanoMig, not a conformance failure against a page that only ever
claimed a possibility.

**A12-SUBSTITUTE-EXACT-COUNT-COMPARATOR-LIVE is removed along with A12's
graded check.** Its only job was proving A12's comparator was not vacuously
red (needed because a stimulus mutation cannot demonstrate anything on an
already-failing check). With A12 declined rather than graded, there is no
graded comparator left to defend, and the substitute duplicated A9/A10's own
already-graded, already-green exact-count measurement anyway. Removed, along
with its mutation record.

## Rig hazards named by the build brief (restated here, not yet acted on)

- `dsktrack` is reported to have no reset term, so a reused Verilated model
  would carry head position between trials. This package will allocate a
  fresh model per trial. Not yet verified from RTL — accepted as a build
  constraint.
- `paula_floppy_ebr.v` reportedly declares a second `module paula_floppy`;
  the fixture is `paula_floppy.v` + `paula_floppy_fifo.v` only.
- DSKLEN reportedly needs the double write to arm DMA, consistent with A5/A6
  above, which this package derives independently from the manual text, not
  from that disclosure.

## Control case

A9 (`DSKLEN_LENGTH_FIELD_COUNTS_WORDS`) is this package's control case: an
unexceptional, uncontested reading of the page, on the same
measurement basis (program a word count, run a transfer, count delivered
words) as A10's decrement/stop assertion and as A12's declined short-read
claim (see A12 above, repaired 2026-09-15). If A9 is red, the fixture's
word-counting mechanism itself cannot be trusted for any of the length-based
readings in this package.

## Mutation plan

Each assertion above gets a `// MUTATION <label> <oracle|stimulus|none>
<red|green|impossible> <what changed>` record in the C++ source (not a
separate file), `<label>` being the exact leading token of the string passed
to `expect()`. The default plan is a stimulus mutation: change what the
fixture is asked to do (a different write value, a different port state, a
shorter/longer source feed) while leaving the expected value untouched, and
confirm the assertion reddens, then revert.

Two claims (A11, A12) are DECLINED via `record()`, not graded through
`expect()`, and carry no mutation record — neither is an assertion that can
fail, so the mutation-record requirement ("if it is graded through expect(),
it needs a record") does not apply to either. A11 declines because reaching a
genuine last-written byte is out of proportion to this package's scope (see
the header comment). **A12 was repaired 2026-09-15**: it was originally
graded literally-red against the page's "may" wording and defended by a
substitute assertion (A12-SUBSTITUTE-EXACT-COUNT-COMPARATOR-LIVE) proving its
comparator live; independent validation found the grading itself wrong ("may"
demands nothing a deterministic trial can violate), so both A12's graded
check and its now-purposeless substitute are removed, and A12 is declined
instead.

A0 (fixture liveness) gets its own mutation: drive the second known value
identical to the first, so the two readings become indistinguishable, and
confirm the liveness check reddens.

## What happens next

Only after this file is committed: read `paula_floppy.v` and
`paula_floppy_fifo.v` for the actual port names, register-address decode, the
FIFO write/read interface, and the `enable`/`dsktrack`/`fdd_led` signals
needed to build the twelve assertions above plus fixture liveness; read an
existing sibling package's Makefile (e.g. `hrm/ch8-ciaapra-address-window` or
`hrm/paula_floppy`) for the build pattern. All of that reading will be
disclosed in the next commit.
