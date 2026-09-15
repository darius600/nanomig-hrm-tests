# ch8-dsksync-word-match — SPECIFICATION

Written from `work/ch8-s2-build/ch8-dsksync-word-match.jsonl` (22 claims) and
`work/2026-09-15_ch8_s2_finding_candidates.md` alone. `paula_floppy.v` and
`paula_floppy_fifo.v` — the assigned fixture — have not been opened. This file
is committed before either is.

**Process disclosure, honestly recorded rather than papered over.** Before
writing this file I read, in full, two sibling packages' already-built test
sources for this suite's harness/mutation-record boilerplate shape:
`hrm/ch8-ciaapra-address-window/test_ch8-ciaapra-address-window.cpp` and
`hrm/ch8-drive-select-motor-led/test_ch8-drive-select-motor-led.cpp`. Neither
is `paula_floppy.v` or `paula_floppy_fifo.v`, and neither is this package's own
RTL boundary, but the second one's header discloses `paula_floppy` RTL facts
about `motor_on`, `fdd_led`, `dskstate` and DMA arming — the mechanism behind
*that* package's own propositions (drive-select/motor/LED), unrelated to
DSKSYNC/WORDSYNC/`trackrdok`. None of those facts appear in, or shaped, any
proposition or test plan below; everything RTL-specific used here (port names,
line numbers, register offsets) is quoted from `classify.evidence` in this
package's own `.jsonl`, which is not RTL I opened myself, exactly as the
`ch8-ciaapra-address-window` precedent disclosed doing with `gary.v`
citations. I am recording the deviation rather than asserting a purer process
than actually happened.

## Source claims (verbatim quotations), reduced to eleven propositions

Each proposition names the printed page, its two uids (an opus/sol twin pair
citing the same underlying text, classified independently), and the
restatement each reader reduced the quotation to. Twins get one proposition
and two `expect()` calls in the built package — each uid graded and recorded
under its own label, never merged into one call.

| # | proposition | page | quote | uids |
|---|---|---|---|---|
| P-ALIGN | using DSKSYNC guarantees word alignment | 247 | "Using the DSKSYNC register (described below) will guarantee word alignment of the data." | `opus:8-dma.c29`, `sol:8-dsksync.c1` |
| P-ENABLE | WORDSYNC=1 enables synchronizing on a word during a disk read | 249 | "WORDSYNC Value of 1 enables synchronizing and starting of DMA on disk read of a word. The word on which to synchronize must be written into the DSKSYNC address ($DFF07E). This capability is highly useful." | `opus:8-adkcon.c19`, `sol:8-adkcon.c15` |
| P-DMASTART | WORDSYNC=1 also starts DMA on the word match | 249 | (same quote as P-ENABLE) | `opus:8-adkcon.c20`, `sol:8-adkcon.c16` |
| P-WRITETARGET | the sync word must be written into DSKSYNC ($DFF07E) | 249 | (same quote as P-ENABLE) | `opus:8-adkcon.c21`, `sol:8-adkcon.c17` |
| P-SYNCREG | DSKSYNC is used to synchronize the input stream | 250 | "The DSKSYNC register is used to synchronize the input stream." | `opus:8-dsksync.c1`, `sol:8-dsksync.c2` |
| P-GATE | with WORDSYNC enabled, no data is transferred until a stream word matches DSKSYNC | 250 | "If the WORDSYNC bit is enabled in ADKCON, no data is transferred until a word is found in the input stream that matches the word in the DSKSYNC register." | `opus:8-dsksync.c2`, `sol:8-dsksync.c3` |
| P-ALIGNSTART | on read, DMA starts with the word following the match | 250 | "On read, DMA will start with the following word from the disk." | `opus:8-dsksync.c3`, `sol:8-dsksync.c4` |
| **P-RESYNC** (this package's headline, candidate **C10**) | the controller resyncs every time the word match is found, during read DMA | 250 | "During disk read DMA, the controller will resync every time the word match is found." | `opus:8-dsksync.c4`, `sol:8-dsksync.c5` |
| P-INTREQSET | the DSKSYNC bit in INTREQ is set when the stream matches DSKSYNC | 250 | "In addition, the DSKSYNC bit in INTREQ is set when the input stream matches the DSKSYNC register." | `opus:8-dsksync.c6`, `sol:8-dsksync.c7` |
| P-INTREQINDEP | that INTREQ bit is independent of the WORDSYNC enable | 250 | "The DSKSYNC bit in INTREQ is independent of the WORDSYNC enable." | `opus:8-dsksync.c7`, `sol:8-dsksync.c8` |
| P-APPA | DSKSYNC holds the match code for disk read synchronization | 279 | "holds the match code for disk read synchronization." | `opus:8-appA-dsksync.c5`, `sol:8-appa-dsksync.c4` |

22 uids, 11 propositions, matching the task's claim count exactly.

`sol:8-dsksync.c7` and `sol:8-dsksync.c8` carry a `flags: ["out_of_slice"]`
mark from the reader's own classification pass, but both crossread `verdict:
"ok"` and both are named in this package's own `.jsonl` under
`package.disposition: "new"`. They are mine to build, not to silently drop
because of a flag raised at an earlier stage; see the INTREQ boundary caution
below for how they are scoped instead.

## Fixture and rig hazards

Fixture: `paula_floppy.v` + `paula_floppy_fifo.v` only. `paula_floppy_ebr.v`
declares a second `module paula_floppy` and is left out of the build entirely
(the task's own instruction; corroborated independently by finding candidates
C2/C6's sibling note and the "One more rig hazard" section of
`2026-09-15_ch8_s2_finding_candidates.md`).

`dsktrack` has no reset term (`paula_floppy.v:671-680`, disclosed by the task
and by candidate note "Rig hazards for the build stage"). A reused Verilated
model carries head position across trials. Every trial in the built package
allocates a fresh model.

DSKLEN needs the double write to arm DMA (`paula_floppy.v:743`, disclosed by
both the task and the candidates file's rig-hazard section, with measured
before/after register values already recorded there). The built package will
use the double write.

## Module boundary: `paula_floppy` alone is sufficient for ten of eleven propositions, and partial for one

Every proposition's own `classify.evidence` names `paula_floppy` observables
only: `reg_address_in`, `data_in`, `data_out`, `wordsync`, `dmal`, `syncint`,
`blckint`, `enable`, and the SD-side `sdc_byte_in_data`/`sdc_byte_in_strobe`/
`sdc_byte_addr`. `wordsync` itself arrives at this boundary as a plain
one-bit input (`paula_floppy.v:98`, per the evidence) — ADKCON's decode and
the `paula.v:269` wire that carries bit 10 into it belong to `paula`, not to
this package's assigned rig, and no proposition here needs to drive ADKCON
through that decode: the task hands `wordsync` to us directly as a testbench
signal. No proposition needs a wider boundary than `paula_floppy` for that
reason either.

P-INTREQSET and P-INTREQINDEP are the one partial case. The full sentence on
p.250 is about a bit landing in **INTREQ**, which is decoded and held in
`paula_intcontroller`, not in `paula_floppy` — the claims' own
`classify.evidence` says so explicitly: *"Composed fact across the two modules
this lot owns: paula_floppy supplies 'a stream match pulses `syncint`' and
paula_intcontroller supplies '`syncint` sets INTREQ bit 12'. Neither module
states the whole sentence alone."* `paula_intcontroller` is not this
package's assigned fixture, and opening a second module here would be
widening the boundary beyond the one assigned — the brief's own HALT
condition. So this package grades **only the `paula_floppy` half of the
composed claim**: that `syncint` pulses when the input stream matches
DSKSYNC (P-INTREQSET), and that it does so whether or not `wordsync` is set
(P-INTREQINDEP) — not that INTREQ's bit 12 is literally set, which this
fixture cannot observe. This mirrors the brief's instruction for the six
promoted `partly-overturned` claims elsewhere in this slice: grade only the
half the evidence names, and say so, rather than overreach into a claim about
a register this rig cannot see.

## Test plan (general; exact stimulus bytes finalized after RTL is read)

**A0 — fixture liveness**, no citation. With `wordsync=0` (no match-gating in
effect), feed two distinct known SD-sector bytes and confirm both arrive,
distinctly, via DSKDATR — the mandated proof that the data path is not
silently dead before anything else is trusted.

**Canonical trials**, each on a fresh Verilated model, each varying
`wordsync`, the value written to DSKSYNC, and the SD-fed byte payload:

- **T-MATCH**: `wordsync=1`, DSKSYNC set to a value the fed stream is known
  (from A0-style groundwork) to produce at a controlled position. Captures
  the delivered word sequence via repeated DSKDATR reads.
- **T-NOMATCH**: `wordsync=1`, DSKSYNC set to a value the fed stream never
  produces. Already measured once at classification (0 words delivered, no
  `syncint`); this package re-measures it itself rather than citing that
  number as its own.
- **T-UNGATED**: `wordsync=0`, otherwise the same stream as T-MATCH — delivery
  is expected to start immediately, unfiltered.
- **T-WRITETARGET**: same stream as T-MATCH, DSKSYNC written to a *different*
  value that the stream also produces at a different position — showing the
  match point moves with whatever was written, not with a fixed constant.
- **T-RESYNC**: **REPAIRED 2026-09-15, DECLINED — see below.** The original
  plan (quoted for the record, not followed): engineer a stream producing the
  DSKSYNC value twice and apply P-ALIGNSTART's "excluded, delivery continues
  with the successor" comparator to the second occurrence, expecting RED as
  the deliverable. Independent validation found this tests MORE than p.250
  states: "the controller will resync every time the word match is found"
  does not say the later, already-word-aligned matching word is OMITTED from
  delivery — a controller can re-align its word boundary at a later match
  while still delivering that already-aligned word. A sound version needs a
  deliberately BIT-MISALIGNED recurring match with an observable showing the
  subsequent word boundary re-aligns, not merely a repeated whole-word value.
  Checked directly against `paula_floppy.v:479-500` (`floppy_sector_data`):
  it is a purely word-indexed combinational multiplexer on
  `fifo_word_counter`, presenting one of a small fixed set of whole 16-bit
  values per counter position — no shift register, no sub-word or
  bit-rotated view of the MFM stream exists anywhere in the module, so no
  stimulus this fixture's input space (SD-sector bytes) can present a match
  at a bit-shifted, non-word-aligned position. The case a repaired assertion
  would need to test does not exist at this boundary. **`opus:8-dsksync.c4`
  and `sol:8-dsksync.c5` are DECLINED via `record()`, quoting this exact
  reasoning; neither is graded through `expect()`, and neither carries a
  mutation record.** The fixture's own (necessarily word-aligned) recurring-
  match behaviour is kept as a `note()` — real and measured, not a finding
  against a sentence that never rules it out.
- **T-SYNCINT**: `wordsync=1` vs `wordsync=0`, same matching stream both
  times — count `syncint` pulses across the full run in both cases.

**Control case, on the same measurement basis as P-RESYNC.** P-ALIGNSTART
(`opus:8-dsksync.c3`, `sol:8-dsksync.c4`) grades the identical "matched word
excluded, delivery continues with its successor" comparator that P-RESYNC
uses, applied to the *first* match instead of a later one, and is expected
GREEN. That is the control this package leans on: it establishes the
comparator can and does detect exclusion when exclusion genuinely happens,
so P-RESYNC's red is a property of the *second* match specifically, not of a
comparator that can never fire true. No separate bespoke control assertion is
added beyond this — P-ALIGNSTART already occupies that role, on the same
measurement basis, as the brief requires.

## The mutation plan

Every `expect()` below (A0, all 22 named-uid assertions) gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what
changed>` record in the C++ source, `<label>` the exact leading token of that
`expect()`'s own string literal — for the 22 named assertions, the claim's
own `classify.uid`, unbroken in one string literal, matching this suite's
established chapter-8 convention (`hrm/ch8-drive-select-motor-led`,
`hrm/ch8-controller-port-inventory`).

For 21 of the 22, the plan is an ordinary **stimulus** mutation: change what
the rig is asked to do (redirect which value is written to DSKSYNC, flip
`wordsync`, corrupt a payload byte, or otherwise ask a different question) so
the assertion goes red, confirm only that label moves, then revert.

**`opus:8-dsksync.c4` and `sol:8-dsksync.c5` (P-RESYNC) — repaired 2026-09-15:
DECLINED, not graded, so neither carries a mutation record.** The plan above
(a substitute mutation through the first-match-exclusion comparator, keyed on
an expected-RED baseline) is withdrawn along with the graded `expect()` it
would have defended. See the T-RESYNC entry above for the reasoning: this
fixture's generator cannot present the bit-misaligned recurring match a sound
version of this assertion needs, so there is no comparator here left to
defend against a vacuous-red objection — the citation is declined instead.

## What happens next

Only after this file is committed: read `paula_floppy.v` and
`paula_floppy_fifo.v` in full for the actual port list, the encoder's exact
byte-to-word mapping (to pin down T-RESYNC's stimulus bytes), the DSKDATR
read path, and the clocking convention; confirm the boundary and mutation
plan above still hold once the real signals are in hand; and if anything
here turns out wrong (a proposition unobservable at this boundary after all,
a control that does not hold, a mutation that cannot be built as planned),
say so rather than silently rewriting this file to fit what RTL turns out to
require. Anything opened beyond `paula_floppy.v` and `paula_floppy_fifo.v`
will be disclosed in the next commit.
