# ch8-disk-register-map-paula-floppy — SPECIFICATION

Written from the quotations in
`work/ch8-s2-build/ch8-disk-register-map-paula-floppy.jsonl` alone (the 79
claims assigned to this package — the largest in slice ch8-s2). No RTL file
has been opened by me yet. This document is committed before any of that
happens.

Fixture assigned by the build brief: `paula_floppy` (`paula_floppy.v` +
`paula_floppy_fifo.v`; `paula_floppy_ebr.v` excluded — it declares a second
`module paula_floppy` and is not part of this fixture). I have not opened any
of these three files.

## What this package is for

The register-map propositions for five registers: DSKLEN ($024), DSKBYTR
($01A), DSKSYNC ($07E), DSKDAT ($026), DSKDATR ($008) — which address each
answers at, in which direction, and what the function column says, each
composed with the appendix legend where the claim is a bare R/W letter (the
legend text itself is graded by a different package; here it is only used,
per general knowledge of the appendix's own key, to read the letter). A row
saying a register carries `W` and the legend saying what `W` means are two
separate facts; neither alone states "write-only", the composition does.

Two registers named in the build brief's own address list — DSKPTH ($020) and
DSKPTL ($022) — carry **no claims in this package's jsonl** and are not
addressed below. Nothing in `paula_floppy-`space is graded against them here;
the "does not answer elsewhere" half of each address proposition below is
checked against the other four registers in this file's own set plus the two
un-claimed offsets, since the classify evidence already swept exactly that
set (see each ADDRESS table's own uids).

## Why 79 claims reduce to 19 testable propositions

(20 originally; see the 2026-09-15 repair note under E3 for E4's removal.)

The manual states each of these five rows in four to nine different printings
(the page-241 register list, the page-246/249/265 cross-references, Appendix
A p.278-279, Appendix B pp.289-290), and both readers indexed nearly every
printing separately, with two extraction passes each (`opus:8-app*A*` /
`sol:8-appa-` at Appendix A, `opus:8-appB-` / `sol:8-appb-` at Appendix B).
Every printing of the same row reduces to the same handful of testable facts
— an address, a direction, a function — so this package grades each fact
once, citing every uid that names it. No proposition is graded by more than
one assertion; no assertion grades more than one proposition. A handful of
claims split a single printed row into two independently-gradeable halves
per `classify.split`; those are graded as two separate propositions, each
citing only the uids that name that half.

## Assertions

### L0 — FIXTURE_LIVENESS (no manual citation)

Per the build brief: "The fixture proves it can deliver a known word before
anything reads one." Before any assertion below is trusted, drive a distinct,
known value through this fixture's write/read data path and confirm it
arrives unmodified and distinguishable from a second, different known value.

### L1 — DSKLEN_ADDRESS

| uid | page | quote |
|---|---|---|
| `opus:8-dsk.c35` | 241 | "   DSKLEN     ($DFF024)    length of DMA" |
| `opus:8-dma.c13` | 246 | "           Table 8-6: DSKLEN Register ($DFF024)" |
| `opus:8-appA-dsklen.c1` | 279 | "DSKLEN     024      W       P     Disk length" |
| `sol:8-appa-dsklen.c1` | 279 | "DSKLEN     024      W       P     Disk length" |
| `opus:8-appB-dsklen.c1` | 289 | "DSKLEN       *024   W           P    Disk length" |
| `sol:8-appb-dsklen.c1` | 289 | "DSKLEN       *024   W           P    Disk length" |
| `opus:B-dsklen.addr` | 289 | "DSKLEN       *024   W           P    Disk length" |

Proposition: DSKLEN answers (i.e. a write reaches state observable through
this fixture) at $024, and at $024 only — not at $008, $01A, $026 or $07E.
`opus:B-dsklen.addr` is a later Appendix B extraction of this same row and
states the same fact; L1 already establishes it in full.

### L2 — DSKLEN_WRITE_ONLY

| uid | page | quote |
|---|---|---|
| `sol:8-dsklen.c1` | 246 | "All of the control bits relating to this topic are contained in a\nwrite-only register, called DSKLEN:" |
| `opus:8-appA-dsklen.c2` | 279 | "DSKLEN     024      W       P     Disk length" |
| `sol:8-appa-dsklen.c2` | 279 | "DSKLEN     024      W       P     Disk length" |
| `opus:8-appB-dsklen.c3` | 289 | "DSKLEN       *024   W           P    Disk length" |
| `sol:8-appb-dsklen.c3` | 289 | "DSKLEN       *024   W           P    Disk length" |

Proposition (composed: row letter `W` + the appendix legend's "write-only"):
a value presented at $024 reaches state, and reading at $024 returns nothing
written there — no read decode exists for it.

### L3 — DSKLEN_FUNCTION_LENGTH

| uid | page | quote |
|---|---|---|
| `opus:8-dsk.c36` | 241 | "   DSKLEN     ($DFF024)    length of DMA" |
| `sol:8-reglist.c17` | 241 | "   DSKLEN     ($DFF024)    length of DMA" |
| `opus:8-appA-dsklen.c4` | 279 | "DSKLEN     024      W       P     Disk length" |
| `sol:8-appa-dsklen.c4` | 279 | "DSKLEN     024      W       P     Disk length" |
| `opus:8-appB-dsklen.c5` | 289 | "DSKLEN       *024   W           P    Disk length" |
| `sol:8-appb-dsklen.c5` | 289 | "DSKLEN       *024   W           P    Disk length" |

Proposition: DSKLEN's value functions as the length of a DMA transfer — a
value programmed there governs how many words a transfer moves before it
ends.

### B1 — DSKBYTR_ADDRESS

| uid | page | quote |
|---|---|---|
| `opus:8-dsk.c37` | 241 | "   DSKBYTR    ($DFF01A)    Disk data byte and status read" |
| `opus:8-appA-dskbytr.c1` | 278 | "DSKBYTR    01A      R       P     Disk data byte and status read" |
| `sol:8-appa-dskbytr.c1` | 278 | "DSKBYTR    01A      R       P     Disk data byte and status read" |
| `opus:8-appB-dskbytr.c1` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |
| `sol:8-appb-dskbytr.c1` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |
| `opus:B-dskbytr.addr` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |

Proposition: DSKBYTR answers at $01A, and at $01A only. `opus:B-dskbytr.addr`
is a later Appendix B extraction of this same row and states the same fact;
B1 already establishes it in full.

### B2 — DSKBYTR_READ_ONLY

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskbytr.c2` | 278 | "DSKBYTR    01A      R       P     Disk data byte and status read" |
| `sol:8-appa-dskbytr.c2` | 278 | "DSKBYTR    01A      R       P     Disk data byte and status read" |
| `opus:8-appB-dskbytr.c3` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |
| `sol:8-appb-dskbytr.c3` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |

Proposition (composed: row letter `R` + legend "read-only"): $01A answers on
a read, and nothing presented at $01A on a write reaches any state.

### B3 — DSKBYTR_STATUS_HALF

| uid | page | quote |
|---|---|---|
| `sol:8-reglist.c20` | 241 | "   DSKBYTR    ($DFF01A)    Disk data byte and status read" |
| `sol:8-appa-dskbytr.c5` | 278 | "DSKBYTR    01A      R       P     Disk data byte and status read" |
| `sol:8-appb-dskbytr.c6` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |

Proposition (the status half of the function cell): reading DSKBYTR provides
disk status that moves as the module's disk-DMA state moves.

### B4 — DSKBYTR_DATA_BYTE_HALF

| uid | page | quote |
|---|---|---|
| `sol:8-reglist.c19` | 241 | "   DSKBYTR    ($DFF01A)    Disk data byte and status read" |
| `sol:8-appa-dskbytr.c4` | 278 | "DSKBYTR    01A      R       P     Disk data byte and status read" |
| `opus:8-appB-dskbytr.c5` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |
| `sol:8-appb-dskbytr.c5` | 289 | "DSKBYTR      *01A   R           P    Disk data byte and status read" |

Proposition (the data-byte half of the function cell): reading DSKBYTR
provides a disk data byte — DATA sampled at the instant a KNOWN byte is
current should read that byte. Flagged by `finding_candidates.md` C6 as a
likely red; graded here regardless of that expectation.

**Repaired (independent validation, 2026-09-15):** the original form
sampled only idle and a DSKLEN WRITE-bit state and passed if either low
byte was nonzero — zero is a valid disk byte, so neither state could ever
refute the claim; there was no independent ground truth for what the
"right" byte should be. THIS IS THE SAME UNDERLYING FACT as
`ch8-dskbytr-status-register`/A9 and
`ch8-disk-controller-overview`/`opus:8-dsk.c9`+`sol:8-dsk.c9` — one fact,
repaired identically in all three packages that carry it. Repaired to bind
a KNOWN incoming byte (0xFF) to the DSKBYTR sampling instant via the
DSKDATR/FIFO marker technique already established by the sibling
`ch8-mfm-cell-encoding` package: a uniform 0xFF sector's payload word can
only ever arrive as exactly 0xFFFF. `find_ff_marker_pop_index()`
calibrates the pop index at which that marker appears; the real trial
peeks (does not pop) DSKDATR to confirm the known word is present, then
peeks DSKBYTR at the same instant (zero clock edges elapse between the two
peeks). Measured: RED — `known_word=0xFFFF` (bound, confirmed at pop #33),
DSKBYTR's DATA field reads 0x00.

**Repaired again (lot G, 2026-09-16):** the 2026-09-15 form's
`find_ff_marker_pop_index()` SEARCHED for the 0xFFFF marker and used
`bound_ok` (whether the search found it) as a CONJUNCT of the graded
condition (`bound_ok && data_field == known_byte`) — but 0xFFFF is THIS
RTL's own non-conformant encoding of 0xFF (p.250 makes 0xFF → 0x5555), so
against a conforming encoder the search would return -1 and the assertion
would FAIL anyway, reporting a DATA-field defect that would not be there.
Repaired to a FIXED locator (`FIRST_PAYLOAD_POP_INDEX` = 33, taken from
`paula_floppy.v`'s own word-counter framing thresholds, :484-497,
structurally independent of what any cell encodes to) plus a PRECONDITION:
the peek at that fixed instant still confirms the sampled word is the known
byte's own encoding, but confirmation failure now DECLINES via `record()`
rather than folding into a graded FAIL — the shape already accepted in
`ch8-dskbytr-status-register`/A9. Measured values unchanged.

### B5 — DSKBYTR_COMPOUND_BYTE_AND_STATUS

| uid | page | quote |
|---|---|---|
| `opus:8-dsk.c38` | 241 | "   DSKBYTR    ($DFF01A)    Disk data byte and status read" |
| `opus:8-appA-dskbytr.c4` | 278 | "DSKBYTR    01A      R       P     Disk data byte and status read" |
| `sol:8-appa-dskbytr.c6` | 278 | "                 This register is the disk-microprocessor data\n                 buffer." |

Proposition: these three claims restate the printed function cell without
`classify.split` decomposing it into the two halves above — they assert the
conjunction, "DSKBYTR returns a disk data byte together with status" /
"is the disk-to-microprocessor data buffer" (byte content AND status
content, both real). Graded as the conjunction B3 ∧ B4.

**Repaired (independent validation, 2026-09-15):** B5's byte half
inherited B4's original unsupported "nonzero at least once" premise and
was withdrawn for the same reason. Repaired by reusing B4's own repaired,
known-byte-bound measurement for the byte half (see B4 above) instead of
recomputing an unbound premise. Measured: RED, for the same underlying
reason as B4 (`status_half_real=true`, `byte_half_real=false`).

**Repaired again (lot G, 2026-09-16):** the byte half's
`byte_half_real = (known_word == 0xFFFF) && (...)` folded the same
`bound_ok`-as-conjunct fault B4 carried. Repaired identically to B4: fixed
locator, and an unconfirmed sample DECLINES the whole compound claim via
`record()` instead of failing the conjunction for the wrong reason. Measured
values unchanged (`status_half_real=true`, byte binding confirmed,
`data_matches=false`).

### B6 — DSKBYTR_BYTE_FIELD_COMPARATOR_LIVE (substitute, no manual citation)

Not a page claim. B4 and B5 are expected red (see B4). A stimulus mutation
cannot fail an already-failing assertion, so per the build brief this
substitute applies the *same* equality/inequality comparator this package
uses to grade B4/B5 (reading a specific field out of `data_out` at $01A and
comparing it against an expected, stimulus-dependent value) to the status
field, which B3 already shows moves — proving the extraction-and-compare
apparatus is not itself silently broken or vacuous.

### S1 — DSKSYNC_ADDRESS

| uid | page | quote |
|---|---|---|
| `opus:8-dsk.c39` | 241 | "   DSKSYNC    ($DFF07E)    Disk sync finder; holds a match word" |
| `opus:8-adkcon.c22` | 249 | "     10      WORDSYNC   Value of 1 enables synchronizing and starting of\n                        DMA on disk read of a word.  The word on which to\n                        synchronize must be written into the  DSKSYNC\n                        address ($DFF07E).  This capability is highly\n                        useful." |
| `sol:8-adkcon.c18` | 249 | "The word on which to\n                        synchronize must be written into the  DSKSYNC\n                        address ($DFF07E)." |
| `opus:8-appA-dsksync.c1` | 279 | "DSKSYNC    07E      W       P     Disk sync register" |
| `sol:8-appa-dsksync.c1` | 279 | "DSKSYNC    07E      W       P     Disk sync register" |
| `opus:8-appA-adkcon.c21` | 265 | "            10     WORDSYNC       Enables disk read synchronizing on a word\n                              equal to DISK SYNC CODE, located in\n                              address (3F)*2." |
| `sol:8-appa-adkbit.c12` | 265 | "            10     WORDSYNC       Enables disk read synchronizing on a word\n                              equal to DISK SYNC CODE, located in\n                              address (3F)*2." |
| `opus:8-appB-dsksync.c1` | 290 | "    DSKSYNC      ~07E     W               Disk sync pattern register for disk\n                                               read" |
| `sol:8-appb-dsksync.c1` | 290 | "    DSKSYNC      ~07E     W               Disk sync pattern register for disk\n                                               read" |
| `opus:B-dsksync.addr` | 290 | "    DSKSYNC      ~07E     W               Disk sync pattern register for disk" |

Proposition (the p.265/p.249 pair composes the arithmetic "(3F)*2 = $07E"
with the register list's own $07E): DSKSYNC answers at $07E, and at $07E
only. `opus:B-dsksync.addr` is a later Appendix B extraction of this same
row and states the same fact; S1 already establishes it in full. Only the
register-offset half of the p.249/p.265 pair is graded — the
$DFF page base that turns $07E into $DFF07E is decoded above this package's
fixture boundary and belongs to a different package (`ch7-custom-address-
space`, per this claims file's own `classify.split`); WORDSYNC's own
enable/disable semantics are likewise out of scope here.

### S2 — DSKSYNC_WRITE_ONLY

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dsksync.c2` | 279 | "DSKSYNC    07E      W       P     Disk sync register" |
| `sol:8-appa-dsksync.c2` | 279 | "DSKSYNC    07E      W       P     Disk sync register" |
| `opus:8-appB-dsksync.c3` | 290 | "    DSKSYNC      ~07E     W               Disk sync pattern register for disk\n                                               read" |
| `sol:8-appb-dsksync.c3` | 290 | "    DSKSYNC      ~07E     W               Disk sync pattern register for disk\n                                               read" |

Proposition (composed: row letter `W` + legend "write-only"): a value
presented at $07E reaches state, and reading $07E returns nothing written
there.

### S3 — DSKSYNC_FUNCTION_MATCH_WORD

| uid | page | quote |
|---|---|---|
| `opus:8-dsk.c40` | 241 | "   DSKSYNC    ($DFF07E)    Disk sync finder; holds a match word" |
| `opus:8-dsk.c41` | 241 | "   DSKSYNC    ($DFF07E)    Disk sync finder; holds a match word" |
| `sol:8-reglist.c22` | 241 | "   DSKSYNC    ($DFF07E)    Disk sync finder; holds a match word" |
| `sol:8-reglist.c23` | 241 | "   DSKSYNC    ($DFF07E)    Disk sync finder; holds a match word" |
| `opus:8-appA-dsksync.c4` | 279 | "DSKSYNC    07E      W       P     Disk sync register" |
| `opus:8-appB-dsksync.c4` | 290 | "    DSKSYNC      ~07E     W               Disk sync pattern register for disk\n                                               read" |
| `sol:8-appb-dsksync.c4` | 290 | "    DSKSYNC      ~07E     W               Disk sync pattern register for disk\n                                               read" |

Proposition: DSKSYNC holds a word that the module compares against the
incoming disk data stream, and that comparison gates (or does not gate)
delivery/interrupt behaviour on whether the two match.

### D1 — DSKDAT_ADDRESS

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskdat.c1` | 279 | "DSKDAT     026      W       P     Disk DMA data write" |
| `sol:8-appa-dskdat.c1` | 279 | "DSKDAT     026      W       P     Disk DMA data write" |
| `opus:8-appB-dskdat.c1` | 289 | "DSKDAT     & *026   W           P    Disk DMA data write" |
| `sol:8-appb-dskdat.c1` | 289 | "DSKDAT     & *026   W           P    Disk DMA data write" |
| `opus:B-dskdat.addr` | 289 | "DSKDAT     & *026   W           P    Disk DMA data write" |

Proposition: DSKDAT answers at $026, and at $026 only. `opus:B-dskdat.addr`
is a later Appendix B extraction of this same row and states the same fact;
D1 already establishes it in full.

### D2 — DSKDAT_WRITE_ONLY

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskdat.c2` | 279 | "DSKDAT     026      W       P     Disk DMA data write" |
| `sol:8-appa-dskdat.c2` | 279 | "DSKDAT     026      W       P     Disk DMA data write" |
| `opus:8-appB-dskdat.c4` | 289 | "DSKDAT     & *026   W           P    Disk DMA data write" |
| `sol:8-appb-dskdat.c4` | 289 | "DSKDAT     & *026   W           P    Disk DMA data write" |

Proposition (composed: row letter `W` + legend "write-only"): a value
presented at $026 reaches state, and reading $026 returns nothing written
there.

### D3 — DSKDAT_FUNCTION_DMA_WRITE

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskdat.c4` | 279 | "DSKDAT     026      W       P     Disk DMA data write" |
| `sol:8-appa-dskdat.c4` | 279 | "DSKDAT     026      W       P     Disk DMA data write" |
| `opus:8-appB-dskdat.c6` | 289 | "DSKDAT     & *026   W           P    Disk DMA data write" |
| `sol:8-appb-dskdat.c6` | 289 | "DSKDAT     & *026   W           P    Disk DMA data write" |

Proposition: a write at $026 is disk DMA outgoing data — the written value
enters the disk-write data path.

### E1 — DSKDATR_ADDRESS

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskdat.c5` | 279 | "DSKDATR    008      ER      P     Disk DMA data read (early read dummy\n                                      address)" |
| `sol:8-appa-dskdatr.c1` | 279 | "DSKDATR    008      ER      P     Disk DMA data read (early read dummy\n                                      address)" |
| `opus:8-appB-dskdatr.c1` | 289 | "DSKDATR    & *008   ER          P    Disk data early read (dummy address)" |
| `sol:8-appb-dskdatr.c1` | 289 | "DSKDATR    & *008   ER          P    Disk data early read (dummy address)" |
| `opus:B-dskdatr.addr` | 289 | "DSKDATR    & *008   ER          P    Disk data early read (dummy address)" |

Proposition: DSKDATR answers at $008, and at $008 only. `opus:B-dskdatr.addr`
is a later Appendix B extraction of this same row and states the same fact;
E1 already establishes it in full.

### E2 — DSKDATR_EARLY_READ_ACCESS_IS_TRIGGER

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskdat.c6` | 279 | "DSKDATR    008      ER      P     Disk DMA data read (early read dummy\n                                      address)" |
| `sol:8-appa-dskdatr.c2` | 279 | "DSKDATR    008      ER      P     Disk DMA data read (early read dummy\n                                      address)" |

Proposition (the `ER` mark's observable half — per `classify.split`, the
68000 bus-timing sense of "early" has no port anywhere in this fixture and is
not graded): the read access at $008 is itself what advances/produces the
value — there is no separate strobe distinct from the access.

### E3 — DSKDATR_NOT_A_DUMMY_ADDRESS — DECLINED

| uid | page | quote |
|---|---|---|
| `opus:8-appA-dskdat.c8` | 279 | "DSKDATR    008      ER      P     Disk DMA data read (early read dummy\n                                      address)" |
| `sol:8-appa-dskdatr.c4` | 279 | "DSKDATR    008      ER      P     Disk DMA data read (early read dummy\n                                      address)" |
| `sol:8-appa-dskdatr.c5` | 279 | "DSKDATR    008      ER      P     Disk DMA data read (early read dummy\n                                      address)" |
| `opus:8-appB-dskdatr.c6` | 289 | "DSKDATR    & *008   ER          P    Disk data early read (dummy address)" |
| `sol:8-appb-dskdatr.c6` | 289 | "DSKDATR    & *008   ER          P    Disk data early read (dummy address)" |

Original proposition (withdrawn): the manual prints $008 as a "dummy
address" — one with no real register behind it, so no value distinguishable
from a dummy/constant answer, dependent on prior state (FIFO content),
should ever appear at $008. Flagged by `finding_candidates.md` C15 as a
likely red; graded here regardless of that expectation.

**Disposition: DECLINED (`record()`), per independent validation
(2026-09-15).** The decisive text is p.287's `ER` legend: early-read
transfers put data on the bus early for RAM timing and "are therefore
initiated by Agnus timing, rather than a read address on the destination
address bus." "Dummy address" names WHO/WHAT triggers the bus cycle
(Agnus's own DMA timing, not a CPU-issued read-address decode on the
destination bus) — it does not claim there is no real register or content
behind $008, and does not require consecutive reads to be identical. The
withdrawn assertion tested exactly that unstated "no real content" reading.

Restating the assertion to what the page ACTUALLY claims would need to
distinguish "an Agnus-timed access" from "a CPU destination-address-bus
access" — a distinction that exists only at the Agnus↔Paula boundary. This
package's assigned fixture (`paula_floppy` alone) has no such boundary:
`reg_address_in` is presented identically regardless of what upstream
hardware drove it there. Declining rather than either re-testing the
withdrawn "no real content" reading (which E1/E2 already show is false in
an uninteresting way) or overreaching to a boundary this package was not
assigned.

E4 (below) is removed along with E3's grading: its sole purpose was
proving E3's own comparator wasn't vacuously red, and there is no longer a
graded comparator for it to defend — E1 already independently establishes
`busrd`'s live/undecoded discrimination over the same swept set.

## Declined claims

As of the 2026-09-15 lot-B repair, six propositions are declined
(`record()`, not `expect()`), 24 uids total:

- E3 (5): `opus:8-appA-dskdat.c8`, `sol:8-appa-dskdatr.c4`,
  `sol:8-appa-dskdatr.c5`, `opus:8-appB-dskdatr.c6`, `sol:8-appb-dskdatr.c6`
  — the manual's "dummy address" claim is about WHO triggers the bus cycle,
  a distinction this package's fixture cannot make (see E3 above).
- L2 (5), S2 (4), D2 (4) — the three WRITE-ONLY propositions (DSKLEN,
  DSKSYNC, DSKDAT). p.263-264 forbids reading a write-only register outright
  and gives no value for what such a read returns; grading "reads 0x0000"
  checks a forbidden operation against a value the page never gives.
- D3 (4) — blckint/length-completion does not show the written word became
  genuine outgoing disk data; reaching that needs paula_floppy's full
  sync+header+payload write parser, which this claim's own trial does not
  drive.
- E2 (2) — same fault as E3: grades the mechanism p.287's `ER` legend
  explicitly says is NOT what triggers an early-read transfer.

The remaining 55 of the 79 claims are cited on a graded (`expect()`) line.
L0 carries no uid and is a `note()` (fixture liveness, not conformance
evidence).

## Expected outcome, stated before any RTL was opened

Per `work/2026-09-15_ch8_s2_finding_candidates.md`, two of this package's
propositions were flagged as likely red from prior classifier measurement:
**B4** (C6 — DSKBYTR's data-byte half) and **E3** (C15 — DSKDATR's "dummy
address" claim). Every other proposition was expected green. This
expectation was written down before RTL was opened, precisely so it could
not be quietly revised after the fact.

**Outcome as measured after the 2026-09-15 repair:** B4 (and B5, the
conjunction that depends on it) measure RED, as expected, but now on a
sound, known-byte-bound basis. E3 was withdrawn by an independent
validation and is DECLINED rather than graded — see E3 above for why a
sound repair is not available at this package's assigned boundary. Every
other proposition remains GREEN.

**Outcome as measured after the 2026-09-15 lot-B repair:** L1, L3, B1, B2,
B3, S1, S3, D1, E1 all still measure GREEN on their repaired, narrowed
oracles (B1/B2/B3 masked to DISKWRITE/before-after equality rather than
DSKBYTR's hardwired-bit idle word; S1/S3 no longer lean on the RTL's reset
default as their only positive evidence). L2, S2, D2, D3 and E2 are
declined rather than graded, for the reasons above. See the build report
for the captured pass/fail tally.
