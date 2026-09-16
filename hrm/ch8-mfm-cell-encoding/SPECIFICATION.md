# ch8-mfm-cell-encoding — SPECIFICATION

Written from the quotations in `ch8-mfm-cell-encoding.jsonl` alone (the 8
claims assigned to this package by `work/ch8-s2-build/ch8-mfm-cell-encoding.jsonl`
— four distinct propositions, each read twice, once by `opus` and once by
`sol`, both readers reaching the identical restatement and the identical
`testable` verdict). No RTL file has been opened by me yet. This document is
committed before any of that happens.

Fixture assigned by the build brief: `paula_floppy` (`paula_floppy.v` +
`paula_floppy_fifo.v`; `paula_floppy_ebr.v` excluded — it declares a second
`module paula_floppy` and is not part of this fixture). I have not opened any
of these three files.

## Why 8 claims reduce to 4 propositions

All 8 claims quote page 250: the "raw MFM data ... twice as large" sentence,
and the three-line cell table (`1 -> 01`, `0 -> 10 ;if following a 0`,
`0 -> 00 ;if following a 1`). Each of the four quotations was read once by
`opus` and once by `sol`; both readings restate the identical proposition and
carry the identical `testable` verdict, so — exactly as this suite's existing
packages do for twin readings (e.g. `hrm/ch8-ciaapra-address-window`,
`hrm/ch8-dsklen-dma-arming`) — each pair cites the *same* assertion. No
proposition is graded by more than one assertion in this package, and no
assertion grades more than one proposition.

## What this package is bounded to, and what it is not

Per the build brief and per finding candidate C2 in
`work/2026-09-15_ch8_s2_finding_candidates.md` (mandatory reading, disclosed
to me before this document, **not yet independently confirmed by me against
RTL**): no port anywhere in this design carries raw medium bit cells. The MFM
layer is reported to be *synthesised* inside `paula_floppy` from SD sector
bytes fed on `sdc_byte_in_data`/`sdc_byte_in_strobe`, and read back at
`data_out`. Every assertion below is written to be observable at exactly that
boundary — known bytes in, the encoded stream word(s) read back out — and none
requires a port this fixture is not reported to have. If, once RTL is opened,
any assertion turns out to need a port this fixture does not carry, that is a
HALT per the build brief, not something to route around.

The build brief's own background (candidate C9, same document) further
discloses — again, not yet independently confirmed by me — that this design's
encoder is *context-free*: it is reported to force the clock cell to a fixed
value for every payload bit, so a 0x00 sector reportedly reads back as 0xAAAA
and a 0xFF sector as 0xFFFF. I record this disclosure here, before opening any
RTL, precisely so the reasoning trail shows it was known in advance rather than
discovered and then used to shape the assertions below — the assertions are
written from the page alone; the disclosure only sets my expectation for which
colour each will grade, exactly as the dsklen sibling package recorded its own
pre-disclosed expectation for its two hardware-bug assertions.

## Assertions — one per proposition, each naming every citing uid and page

### A0 — FIXTURE_LIVENESS (no manual citation)

Not a page claim. Per the build brief: "The fixture proves it can deliver a
known word before anything reads one." Before any encoding assertion is
trusted, drive two distinct, known byte patterns through this fixture's
SD-sector-in -> stream-out data path and confirm each produces a distinct,
known word at `data_out`, unmodified and mutually distinguishable. Required as
its own graded, mutation-recorded item.

### A1 — MFM_DOUBLES_SIZE

| uid | page | quote |
|---|---|---|
| `opus:8-mfm.c1` | 250 | "The raw MFM data that must be presented to the disk controller will be\ntwice as large as the unencoded data." |
| `sol:8-mfm.c1` | 250 | "The raw MFM data that must be presented to the disk controller will be\ntwice as large as the unencoded data." |

Proposition: the encoded (raw MFM) data occupying the disk is exactly twice
the size, in bytes, of the unencoded data it was built from. Graded on the
DATA portion specifically — N unencoded payload bytes fed in produce 2N
encoded bytes read back at `data_out` over the corresponding payload region of
the stream. Scoped this way because the sentence itself names "the unencoded
data" as the comparison baseline, and this fixture's own sync/header/gap
framing is not unencoded application data being doubled — it is fixed control
structure the encoder emits regardless of payload content, and outside what
this one sentence is claiming a ratio about. A ratio measurement, per the
build brief: "twice as large" strips to a dimensionless 2:1 and is measured in
byte counts, not any absolute unit.

### A2 — MFM_ONE_ENCODES_01

| uid | page | quote |
|---|---|---|
| `opus:8-mfm.c2` | 250 | "               1 -> 01" |
| `sol:8-mfm.c2` | 250 | "               1 -> 01" |

Proposition: an unencoded data bit of 1 encodes, unconditionally, as the
two-bit cell pair `01`.

### A3 — MFM_ZERO_AFTER_ZERO_ENCODES_10

| uid | page | quote |
|---|---|---|
| `opus:8-mfm.c3` | 250 | "               0 -> 10   ;if following a 0" |
| `sol:8-mfm.c3` | 250 | "               0 -> 10   ;if following a 0" |

Proposition: an unencoded data bit of 0, whose immediately preceding
unencoded data bit (in original bit-stream order, not raw stream-word order —
see the caveat below) is also 0, encodes as the two-bit cell pair `10`.

### A4 — MFM_ZERO_AFTER_ONE_ENCODES_00

| uid | page | quote |
|---|---|---|
| `opus:8-mfm.c4` | 250 | "               0 -> 00   ;if following a 1" |
| `sol:8-mfm.c4` | 250 | "               0 -> 00   ;if following a 1" |

Proposition: an unencoded data bit of 0, whose immediately preceding
unencoded data bit is 1, encodes as the two-bit cell pair `00`.

### A note shared by A3 and A4: what "following" means at this boundary

This package's own claims file (disclosed to me before this document) flags
that this fixture is reported to split a word's odd- and even-position bits
into two separate stream words roughly 256 words apart, so two cells sitting
next to each other inside one raw stream word are not necessarily two
adjacent bits of the original unencoded data. A3 and A4 must therefore each
construct their stimulus so that the intended predecessor relationship (0
after 0; 0 after 1) holds in the *original, unencoded bit-stream order* the
page is describing — not assumed from position inside one output word. How
that is done mechanically depends on the exact bit-to-cell mapping, which is
RTL detail not yet read; the construction will be disclosed, with its
reasoning, in the test source once the relevant lines are open.

## Control case

A1 (`MFM_DOUBLES_SIZE`) and, if it grades as the pre-disclosure above expects,
A3 (`MFM_ZERO_AFTER_ZERO_ENCODES_10`) are this package's PASS-shaped
measurements. A1 is on a different measurement basis (a byte-count ratio) from
the per-cell content checks (A2-A4), which strengthens rather than substitutes
for a same-basis control: if the fixture's data path can deliver a correct 2:1
count AND (per the pre-disclosure) a correct individual cell reading in one of
the three table rows, two independent mechanisms on this fixture are shown
live and trustworthy before A2 and A4 are read as findings rather than
instrument noise. Per the build brief's own guidance for a green whose
precondition might not occur, A3's own precondition — that the disclosed
context-free behaviour actually produces `10` for a false reason rather than
the page's true reason — is asserted directly, not assumed: the test will
state explicitly, in its own label and comments, that a green A3 does not by
itself certify context-sensitivity, only the cell value on that one row.

## Rig hazards named by the build brief (restated here, not yet acted on)

- `dsktrack` is reported to have no reset term, so a reused Verilated model
  would carry head position between trials. This package will allocate a
  fresh model per trial. Not yet verified from RTL — accepted as a build
  constraint. (This package drives no `_step` edges at all, but the brief's
  mandate is to allocate fresh regardless of whether a given trial happens to
  need it.)
- `paula_floppy_ebr.v` reportedly declares a second `module paula_floppy`;
  the fixture is `paula_floppy.v` + `paula_floppy_fifo.v` only.

## Mutation plan

Each assertion above gets a `// MUTATION <label> <oracle|stimulus|none>
<red|green|impossible> <what changed>` record in the C++ source (not a
separate file), `<label>` being the exact leading token of the string passed
to `expect()`.

A0 (fixture liveness) and A1 (size doubling) are expected, from the page
alone, to hold; each gets a genuine stimulus mutation (drive a different byte
count / a colliding known value) and is confirmed to redden, then reverted.

A3 is pre-disclosed as expected GREEN and, being a genuine PASS, is eligible
for a genuine stimulus mutation too (present a byte pattern where the checked
bit's true predecessor is not 0, while the assertion still asks for the
`0-after-0` reading) — confirmed to redden, then reverted.

A2 and A4 are pre-disclosed as expected RED. Per the build brief, a stimulus
mutation on an assertion already red cannot demonstrate anything (it cannot
fail harder). The substitute rule applies: a different mechanism, already
proven live elsewhere in this same run through the identical cell-extraction
comparator — A3's own live, green `0-after-0 -> 10` reading — is what the
`// MUTATION` record for A2 and for A4 each point to, recorded against their
own assertion id with outcome `green`, exactly as this suite's established
precedent for a red-baseline substitute
(`hrm/ch8-dskbytr-status-register`, its own file header quoting the same
build-brief rule) already does. No new, uid-less "apparatus" assertion is
introduced to carry this: A3 is itself a real, page-cited proposition, and
reusing its already-graded result is the "different mechanism, proven live
elsewhere in the same run" the rule asks for.

## What happens next

Only after this file is committed: read `paula_floppy.v` and
`paula_floppy_fifo.v` in full for the actual port names, the register-address
decode (if any is needed — this package does not expect to need one, since it
drives no register writes, only the SD-sector ingest path and `data_out`
streaming reads), the exact odd/even bit-to-stream-word mapping (needed to
construct A3's and A4's stimulus correctly, per the note above), and the
`dsktrack` reset situation. Read an existing sibling package's Makefile (e.g.
`hrm/ch8-dsklen-dma-arming` or `hrm/ch8-dskbytr-status-register`, both against
the same fixture) for the build pattern. All of that reading will be disclosed
in the next commit.

## Repair addendum (Lot F, 2026-09-15)

An independent validation found A0 (FIXTURE_LIVENESS, no claim uid) and A1
(MFM_DOUBLES_SIZE) both bound their measurement to DSKDATR word `0xFFFF`,
which is this RTL's own non-conformant encoder output for a uniform 0xFF
byte — p.250 makes 0xFF page-legally encode to `0x5555`, so against a
conforming encoder both would count/search for a word that never appears.

- **A0** withdrawn to `note()` (it carries no uid; was never conformance
  evidence).
- **A1** repaired: switched to a uniform 0x00 sector, whose page-legal
  encoding (`0xAAAA`, p.250's own "0 -> 10 if following a 0" steady state)
  this RTL also produces. Counting is scoped to the independently-confirmed
  payload index range (`fifo_word_counter` 32-543, read directly off the
  generator's own word-counter threshold logic, paula_floppy.v:484-497 —
  structurally unrelated to the per-bit encoder logic at :467-477 that is
  the actual site of candidate C9) rather than a whole-stream content
  search, which also avoids the payload checksum words (30/31) that DO
  depend on fed content. Stays GREEN (512/512).

A2 and A4 (both RED, candidate C9 itself) were left untouched — they are
the finding, not the guard.

## Repair addendum (Lot G, 2026-09-16)

Lot F above repaired A1's own locator but left `find_ff_marker_pop_index()`
itself in place, and A0, A2, A3 and A4 all still called it: fed a uniform
0xFF sector on a throwaway rig and searched up to 60 DSKDATR pops for the
first exact `0xFFFF` match. `0xFFFF` is this RTL's own non-conformant
encoder output for a uniform 0xFF byte, not a page-legal one (p.250 makes
0xFF encode to `0x5555`) — against a conforming encoder the search finds
nothing and returns -1, and each caller's own read loop,
`for (int i = 1; i <= idx; i++) w = rig.dskdatr_pop();`, never executes
with `idx = -1`. `w` stays its initializer 0, and A2/A3/A4 would each grade
a cell extracted from a word that was never actually popped — a verdict
for a reason other than the one it names.

Repaired by removing `find_ff_marker_pop_index()` entirely and replacing
every one of its four call sites with a fixed constant,
`FIRST_PAYLOAD_POP_INDEX` (= `FRAMING_WORDS` + 1 = 33), taken from the same
independently-confirmed `fifo_word_counter` threshold logic
(paula_floppy.v:484-497) A1's own Lot F repair already relies on. Every
trial in this file feeds a UNIFORM 512-byte sector, so every payload word
encodes the identical byte — which payload word is read carries no
information A0/A2/A3/A4 need, so the payload's fixed first word
(fifo_word_counter=32, DSKDATR pop #33) is as good a sample as the one the
withdrawn search used to find, and this locator holds against a conforming
encoder exactly as it does against this one.

Measured values are unchanged: against this exact RTL the withdrawn search
always found the marker at pop #33 anyway, so A2 (payload word `0xFFFF`,
first cell `11`, RED), A3 (payload word `0xAAAA`, second cell `10`, GREEN)
and A4 (payload word `0xEAEA`, second cell `10`, RED) grade identically
before and after this repair — 2 passed, 2 failed, both before and after.
A2 and A4 remain untouched in what they assert; only the locator that
finds the word they read was replaced.
