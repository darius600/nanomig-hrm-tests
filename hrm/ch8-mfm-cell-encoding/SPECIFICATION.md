# ch8-mfm-cell-encoding — SPECIFICATION

The package carries the quotations in
`work/ch8-s2-build/ch8-mfm-cell-encoding.jsonl` (the 8 claims assigned to
this package — four distinct propositions, each read twice, once by `opus`
and once by `sol`, both readers reaching the identical restatement and the
identical `testable` verdict).

Fixture: `paula_floppy` (`paula_floppy.v` + `paula_floppy_fifo.v`;
`paula_floppy_ebr.v` excluded — it declares a second `module paula_floppy`
and is not part of this fixture).

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

No port anywhere in this design carries raw medium bit cells (finding
candidate C2 in `work/2026-09-15_ch8_s2_finding_candidates.md`). The MFM
layer is *synthesised* inside `paula_floppy` from SD sector bytes fed on
`sdc_byte_in_data`/`sdc_byte_in_strobe`, and read back at `data_out` through
DSKDATR. Every assertion below is observable at exactly that boundary —
known bytes in, the encoded stream word(s) read back out.

The encoder is *context-free* (candidate C9, `paula_floppy.v:467-477`): it
emits a constant `1` clock cell before every payload data bit, so a 0x00
sector reads back as 0xAAAA and a 0xFF sector as 0xFFFF. The assertions are
written from the page alone; C9 is why A2 and A4 grade red.

## Assertions — one per proposition, each naming every citing uid and page

### A0 — FIXTURE_LIVENESS (no manual citation)

Not a page claim: the fixture proves it can deliver a known word before
anything reads one. Two distinct, known byte patterns (uniform 0xFF, uniform
0x88) are driven through this fixture's SD-sector-in -> stream-out data path
and each produces a distinct, known word at `data_out`. A `note()`, not
graded: it carries no uid, and one of its two words (0xFF -> 0xFFFF) is this
RTL's own non-conformant encoder output, not a page-legal value.

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
this one sentence is claiming a ratio about. A ratio measurement: "twice as
large" strips to a dimensionless 2:1 and is measured in byte counts, not any
absolute unit.

The trial feeds a uniform 0x00 sector, whose page-legal encoding is 0xAAAA
(p.250's "0 -> 10 if following a 0" steady state) — true of a conforming
encoder and of this one alike — and counts 0xAAAA words within the payload
index range (`fifo_word_counter` 32-543, read off the generator's own
word-counter threshold logic, `paula_floppy.v:484-497`, structurally
unrelated to the per-bit encoder at `:467-477`). A uniform 0xFF sector would
not do: its payload word here is 0xFFFF, which p.250 would make 0x5555, so a
count of it would read zero against a conforming encoder. The fixed range
also excludes the payload checksum words (30/31), which do depend on fed
content. Measured: GREEN (512 of 512).

### A2 — MFM_ONE_ENCODES_01

| uid | page | quote |
|---|---|---|
| `opus:8-mfm.c2` | 250 | "               1 -> 01" |
| `sol:8-mfm.c2` | 250 | "               1 -> 01" |

Proposition: an unencoded data bit of 1 encodes, unconditionally, as the
two-bit cell pair `01`. Measured: RED — payload word 0xFFFF, first cell `11`.

### A3 — MFM_ZERO_AFTER_ZERO_ENCODES_10

| uid | page | quote |
|---|---|---|
| `opus:8-mfm.c3` | 250 | "               0 -> 10   ;if following a 0" |
| `sol:8-mfm.c3` | 250 | "               0 -> 10   ;if following a 0" |

Proposition: an unencoded data bit of 0, whose immediately preceding
unencoded data bit (in original bit-stream order, not raw stream-word order —
see the note below) is also 0, encodes as the two-bit cell pair `10`.
Measured: GREEN — payload word 0xAAAA, second cell `10`.

### A4 — MFM_ZERO_AFTER_ONE_ENCODES_00

| uid | page | quote |
|---|---|---|
| `opus:8-mfm.c4` | 250 | "               0 -> 00   ;if following a 1" |
| `sol:8-mfm.c4` | 250 | "               0 -> 00   ;if following a 1" |

Proposition: an unencoded data bit of 0, whose immediately preceding
unencoded data bit is 1, encodes as the two-bit cell pair `00`. Measured:
RED — payload word 0xEAEA, second cell `10`.

### What "following" means at this boundary, and which word is read

This fixture splits a byte pair's odd- and even-position bits into two
separate stream words (`data_odd`, `data_even`, `paula_floppy.v:467-477`), so
two cells sitting next to each other inside one raw stream word are not
necessarily two adjacent bits of the original unencoded data. The first two
cells of a `data_odd` word are, in true bit-stream order, the cells for bit 7
and bit 5 of the SAME fed byte, so their predecessor relationship is known
from the byte value alone. A2 reads the first cell (bits [15:14]) of that
word; A3 and A4 read the second cell (bits [13:12]), fed from a byte whose
bits 7 and 5 are 0 and 0 (A3, byte 0x00) or 1 and 0 (A4, byte 0x80).

The word read is the payload's first word, `fifo_word_counter` 32, i.e.
DSKDATR pop #33 (`FIRST_PAYLOAD_POP_INDEX`), taken from the same word-counter
threshold logic A1 relies on. Every trial feeds a uniform sector, so every
payload word encodes the same byte and this one fixed index is as good a
sample as any. A content search for the 0xFFFF marker instead would find
nothing against a conforming encoder, and its read loop would then never pop
a word — A2/A3/A4 would grade a cell of an unread, zero-initialised word.

## Control case

A1 (`MFM_DOUBLES_SIZE`) and A3 (`MFM_ZERO_AFTER_ZERO_ENCODES_10`) are this
package's PASS-shaped measurements. A1 is on a different measurement basis (a
byte-count ratio) from the per-cell content checks (A2-A4), which strengthens
rather than substitutes for a same-basis control: the fixture's data path
delivers a correct 2:1 count AND a correct individual cell reading in one of
the three table rows, so two independent mechanisms on this fixture are shown
live before A2 and A4 are read as findings rather than instrument noise. A3
is green for a false reason — the context-free clock cell happens to produce
`10` on this row — and its own label says so: a green A3 does not by itself
certify context-sensitivity, only the cell value on that one row.

## Rig hazards

- `dsktrack` has no reset term (`paula_floppy.v:129,671-680`), so a reused
  Verilated model would carry head position between trials. This package
  drives no `_step` edges at all, but allocates a fresh model per trial
  anyway.
- `paula_floppy_ebr.v` declares a second `module paula_floppy`; the fixture
  is `paula_floppy.v` + `paula_floppy_fifo.v` only.
- DSKLEN needs the double write to arm DMA (`paula_floppy.v:735-744`); every
  trial writes its arming value twice.

## Mutation plan

Each graded assertion has a `// MUTATION <label> <oracle|stimulus|none>
<red|green|impossible> <what changed>` record in the C++ source (not a
separate file), `<label>` being the exact leading token of the string passed
to `expect()`.

A1 gets a genuine stimulus mutation (a shorter armed LENGTH, so fewer than
512 payload words are delivered) and reddens. A3 gets a genuine stimulus
mutation too (a uniform 0xFF sector, whose checked cell reads `11`, while the
assertion still asks for the `0-after-0` reading) and reddens.

A2 and A4 are RED at baseline. A stimulus mutation on an assertion already
red cannot demonstrate anything (it cannot fail harder). The substitute rule
applies: a different mechanism, proven live elsewhere in this same run
through the identical cell-extraction comparator — A3's own live, green
`0-after-0 -> 10` reading — is what the `// MUTATION` record for A2 and for
A4 each point to, recorded against their own assertion id with outcome
`green`, as `hrm/ch8-dskbytr-status-register` does for its red-baseline
assertions. No uid-less "apparatus" assertion is introduced to carry this:
A3 is itself a real, page-cited proposition.

A0 is a `note()` and carries no mutation record.

## Outcome

`make -B test`: 2 passed, 2 failed — A1 and A3 GREEN, A2 and A4 RED
(candidate C9); A0 measured via `note()`.
