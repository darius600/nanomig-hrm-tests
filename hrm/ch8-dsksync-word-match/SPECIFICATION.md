# ch8-dsksync-word-match — SPECIFICATION

The package carries the quotations in
`work/ch8-s2-build/ch8-dsksync-word-match.jsonl` (22 claims); finding
candidate C10 and the rig hazards below are recorded in
`work/2026-09-15_ch8_s2_finding_candidates.md`. The harness/mutation-record
boilerplate follows `hrm/ch8-ciaapra-address-window` and
`hrm/ch8-drive-select-motor-led`; no proposition below comes from them.

## Source claims (verbatim quotations), reduced to eleven propositions

Each proposition names the printed page, its two uids (an opus/sol twin pair
citing the same underlying text, classified independently), and the
restatement each reader reduced the quotation to. Twins get one proposition
and, where graded, two `expect()` calls — each uid graded under its own
label, never merged into one call.

| # | proposition | page | quote | uids |
|---|---|---|---|---|
| P-ALIGN | using DSKSYNC guarantees word alignment | 247 | "Using the DSKSYNC register (described below) will guarantee word alignment of the data." | `opus:8-dma.c29`, `sol:8-dsksync.c1` |
| P-ENABLE | WORDSYNC=1 enables synchronizing on a word during a disk read | 249 | "WORDSYNC Value of 1 enables synchronizing and starting of DMA on disk read of a word. The word on which to synchronize must be written into the DSKSYNC address ($DFF07E). This capability is highly useful." | `opus:8-adkcon.c19`, `sol:8-adkcon.c15` |
| P-DMASTART | WORDSYNC=1 also starts DMA on the word match | 249 | (same quote as P-ENABLE) | `opus:8-adkcon.c20`, `sol:8-adkcon.c16` |
| P-WRITETARGET | the sync word must be written into DSKSYNC ($DFF07E) | 249 | (same quote as P-ENABLE) | `opus:8-adkcon.c21`, `sol:8-adkcon.c17` |
| P-SYNCREG | DSKSYNC is used to synchronize the input stream | 250 | "The DSKSYNC register is used to synchronize the input stream." | `opus:8-dsksync.c1`, `sol:8-dsksync.c2` |
| P-GATE | with WORDSYNC enabled, no data is transferred until a stream word matches DSKSYNC | 250 | "If the WORDSYNC bit is enabled in ADKCON, no data is transferred until a word is found in the input stream that matches the word in the DSKSYNC register." | `opus:8-dsksync.c2`, `sol:8-dsksync.c3` |
| P-ALIGNSTART | on read, DMA starts with the word following the match | 250 | "On read, DMA will start with the following word from the disk." | `opus:8-dsksync.c3`, `sol:8-dsksync.c4` |
| **P-RESYNC** (candidate **C10**) | the controller resyncs every time the word match is found, during read DMA | 250 | "During disk read DMA, the controller will resync every time the word match is found." | `opus:8-dsksync.c4`, `sol:8-dsksync.c5` |
| P-INTREQSET | the DSKSYNC bit in INTREQ is set when the stream matches DSKSYNC | 250 | "In addition, the DSKSYNC bit in INTREQ is set when the input stream matches the DSKSYNC register." | `opus:8-dsksync.c6`, `sol:8-dsksync.c7` |
| P-INTREQINDEP | that INTREQ bit is independent of the WORDSYNC enable | 250 | "The DSKSYNC bit in INTREQ is independent of the WORDSYNC enable." | `opus:8-dsksync.c7`, `sol:8-dsksync.c8` |
| P-APPA | DSKSYNC holds the match code for disk read synchronization | 279 | "holds the match code for disk read synchronization." | `opus:8-appA-dsksync.c5`, `sol:8-appa-dsksync.c4` |

22 uids, 11 propositions, matching the claims file's count exactly.

`sol:8-dsksync.c7` and `sol:8-dsksync.c8` carry a `flags: ["out_of_slice"]`
mark from the reader's own classification pass, but both crossread `verdict:
"ok"` and both are named in this package's own `.jsonl` under
`package.disposition: "new"`, so this package accounts for them (see the
INTREQ boundary below) rather than dropping them.

## Fixture and rig hazards

Fixture: `paula_floppy.v` + `paula_floppy_fifo.v` only. `paula_floppy_ebr.v`
declares a second `module paula_floppy` and is left out of the build entirely
(also noted by finding candidates C2/C6's sibling note and the "One more rig
hazard" section of the finding-candidates file).

`dsktrack` has no reset term (`paula_floppy.v:671-680`). A reused Verilated
model carries head position across trials, so every trial allocates a fresh
model.

DSKLEN needs the double write to arm DMA (`paula_floppy.v:743`); every trial
uses it (A0's mutation deliberately omits the second write).

## Module boundary

Every proposition's own `classify.evidence` names `paula_floppy` observables
only: `reg_address_in`, `data_in`, `data_out`, `wordsync`, `dmal`, `syncint`,
`blckint`, `enable`, and the SD-side `sdc_byte_in_data`/`sdc_byte_in_strobe`/
`sdc_byte_addr`. `wordsync` arrives at this boundary as a plain one-bit input
(`paula_floppy.v:98`) — ADKCON's decode and the `paula.v:283` wire that
carries bit 10 into it belong to `paula`, and no proposition here needs to
drive ADKCON through that decode: the testbench drives `wordsync` directly.

P-INTREQSET and P-INTREQINDEP are about a bit landing in **INTREQ**, which is
decoded and held in `paula_intcontroller`, not in `paula_floppy` — the
claims' own `classify.evidence` says so explicitly: *"Composed fact across
the two modules this lot owns: paula_floppy supplies 'a stream match pulses
`syncint`' and paula_intcontroller supplies '`syncint` sets INTREQ bit 12'.
Neither module states the whole sentence alone."* A uid on a graded line
counts as asserted, and a local `syncint` pulse does not establish INTREQ's
bit being set, so both pairs are **declined via `record()`** here: they are
graded in full by `hrm/ch8-disk-interrupt-path` (PROP-DSKSYNC-FULLJOIN and
PROP-DSKSYNC-INTREQINDEP), which observes the real INTREQ bit 12 and
`minimig.v`'s `_cpu_ipl` in the same run as the stimulus. This package's own
`syncint` pulse counts (with `wordsync` 0 and 1) are printed via `note()`.

## What the fixture's generator can and cannot present

`floppy_data` (`paula_floppy.v:479-500`) is a purely word-indexed
combinational multiplexer on `fifo_word_counter`: word0,1 are the fixed
0xAAAA preamble, word2,3 the fixed 0x4489 sync pair, then header/gap words
and the MFM-"encoded" payload. There is no shift register and no sub-word or
bit-rotated view of the stream anywhere in the module, so `sync_match`
(`:914-915`) only ever compares whole, word-aligned words. This decides two
propositions:

- **P-ALIGN — DECLINED.** "Guarantee word alignment" is a guarantee against
  bit-misaligned delivery; nothing this generator presents can be
  misaligned, so a "stays word-aligned" measurement cannot distinguish the
  mechanism the sentence describes from its absence — unfalsifiable here.
  The word-for-word-shifted measurement is kept as a `note()`.
- **P-RESYNC — DECLINED.** "The controller will resync every time the word
  match is found" does not say a later, already-word-aligned matching word
  is OMITTED from delivery — a controller can re-align its word boundary at
  a later match while still delivering that already-aligned word — so
  grading "omitted" would test more than p.250 states. A sound version needs
  a deliberately BIT-MISALIGNED recurring match, which this generator cannot
  present. The fixture's own (necessarily word-aligned) recurring-match
  behaviour — an engineered payload byte producing 0xAAAA again, delivered as
  ordinary data — is kept as a `note()`, not a finding against a sentence
  that never rules it out.

## Checks as built

Every check reads the delivered-word sequence via DSKDATR (`$008`) from fresh
trials and compares sequences (gated against ungated, one DSKSYNC value
against another), never a single raw value against a hand-derived constant.
The FIFO's look-ahead prefetch hides the very first delivered word; comparing
whole shifted sequences cancels that offset.

- **A0 — fixture liveness**, no citation. With `wordsync=0`, the fixture's own
  two fixed structural words — the 0xAAAA preamble (the steady-state all-zero
  encoding p.250's rule produces) and the 0x4489 sync mark (the value the
  manual names as the usual MFM sync mark) — arrive distinctly via DSKDATR.
  Structural words rather than engineered payload bytes, because an
  engineered 0xFF byte arrives as 0xFFFF, this RTL's own non-conformant
  encoder output (p.250 makes 0xFF → 0x5555).
- **P-ENABLE** (2 uids): with WORDSYNC=1 and a DSKSYNC value the stream
  contains, the first delivered word differs from the ungated reading.
- **P-DMASTART** (2 uids): with WORDSYNC=1 and a DSKSYNC value absent from the
  stream, no word is ever delivered; with a present value, delivery starts.
- **P-WRITETARGET**, **P-SYNCREG**, **P-APPA** (2 uids each): writing 0xAAAA
  moves the match to the preamble, writing 0x4489 moves it to the sync mark —
  the match point is exactly where the written value sends it.
- **P-GATE** (2 uids): with a DSKSYNC value absent from the stream, every one
  of the consecutive DSKDATR reads returns the same unchanged value.
- **P-ALIGNSTART** (2 uids): the first word delivered once WORDSYNC opens is
  the word immediately after the matched word, not the match word itself.
- **P-ALIGN, P-RESYNC, P-INTREQSET, P-INTREQINDEP** (8 uids): declined via
  `record()`, with their local measurements printed via `note()` (see above).

## The mutation plan

Every graded `expect()` (A0 and the 14 uid-carrying assertions) has its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what
changed>` record in the C++ source, `<label>` the exact leading token of that
`expect()`'s own string literal — for the uid-carrying assertions, the
claim's own `classify.uid`, unbroken in one string literal, matching this
suite's chapter-8 convention (`hrm/ch8-drive-select-motor-led`,
`hrm/ch8-controller-port-inventory`). Each is an ordinary **stimulus**
mutation: change what the rig is asked to do (redirect which value is written
to DSKSYNC, flip `wordsync`, compare against the match word instead of its
successor, write DSKLEN only once) so the assertion goes red, confirm only
that label moves, then revert. The eight declined uids carry no `expect()`
and no mutation record.

## Outcome

`make -B test`: 15 passed, 0 failed (A0 plus 14 uid-carrying assertions).
