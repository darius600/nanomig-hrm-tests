# ch8-disk-interrupt-requests — SPECIFICATION

Written from the quotations in
`hrm-testpipeline/work/ch8-s2-build/ch8-disk-interrupt-requests.jsonl` alone
(the 8 claims assigned to this package). No RTL file
(`$(NANOMIG)/src/minimig-aga/*.v`) has been opened by me at this stage. This
document is committed before any RTL is opened.

**Process note, disclosed up front rather than left implicit:** before writing
this file I looked at three already-built or already-specified sibling
packages, read-only, none of them modified:

- `hrm/ch8-drive-sense-lines` (`SPECIFICATION.md` and its `.cpp`, both
  committed) — for this suite's directory layout, the `Checks` class
  (`expect`/`note`/`record`), the `mutating(id)` / `MUTATE` env-var
  convention, the fixed `// MUTATION` comment form, and the twin-claim
  pattern (two readers citing the same printed sentence get two independent
  `expect()` calls under their own `classify.uid`, not one assertion citing
  both).
- `hrm/ch8-dsklen-dma-arming` (`.cpp`, committed) — for the DSKLEN
  double-write arming sequence and the `drain_until_blckint` /
  `feed_sector_read` harness technique on the same fixture, since three of my
  eight claims need a completed DMA transfer to observe `blckint`.
- `hrm/ch8-disk-index-interrupt-path` (`SPECIFICATION.md` only — its `.cpp`
  does not exist yet) — read only to confirm the boundary split stated below:
  it is a three-module rig (`paula_floppy` + `ciab` + `paula_intcontroller`)
  built specifically for the INDEX interrupt's journey to the CPU (page 250's
  INDEX line, page 245's CIABPRB/FLAG row). None of my 8 claims quotes the
  INDEX bullet, so there is no overlap to resolve — this was read to confirm
  the division of labour, not to borrow any fact used below.

None of the three is RTL, and no `paula_floppy`-specific conformance fact
below came from any of them — only build/harness mechanics common to this
suite's `paula_floppy` packages. Every RTL line number, signal name, and
mechanism claim appearing below comes from my own claims file's
`classify.evidence` fields and the build task handed to me for this unit. I
have not opened `paula_floppy.v` or `paula_floppy_fifo.v` myself yet.

## Source claims (verbatim quotations)

| uid | page | restatement | quote |
|---|---|---|---|
| `opus:8-dskint.c1` | 250 | The disk controller can raise exactly three kinds of interrupt. | "The disk controller can issue three kinds of interrupts:" |
| `sol:8-int.c1` | 250 | The disk controller has three interrupt types. (twin of `opus:8-dskint.c1`) | "The disk controller can issue three kinds of interrupts:" |
| `opus:8-dskint.c4` | 250 | The DSKSYNC interrupt fires when the input stream matches the DSKSYNC register. | "   *   DSKSYNC  (level 5,  INTREQ  bit 12) -- input stream matches the\n                                             DSKSYNC register." |
| `sol:8-int.c4` | 250 | The DSKSYNC interrupt is requested when the input stream matches DSKSYNC. (twin of `opus:8-dskint.c4`) | "   *   DSKSYNC  (level 5,  INTREQ  bit 12) -- input stream matches the\n                                             DSKSYNC register." |
| `opus:8-dskint.c7` | 250 | The DSKBLK interrupt fires when disk DMA has completed. | "   *   DSKBLK  (level 1,  INTREQ  bit 1) -- disk DMA has completed." |
| `sol:8-int.c7` | 250 | DSKBLK is requested when disk DMA completes. (twin of `opus:8-dskint.c7`) | "   *   DSKBLK  (level 1,  INTREQ  bit 1) -- disk DMA has completed." |
| `opus:8-appA-dskdat.c15` | 279 | Reaching the end of DMA causes a disk block interrupt. | "This register is the disk DMA data buffer. It contains two bytes of data that are either sent (written) to or received (read) from the disk. The write mode is enabled by bit 14 of the LENGTH register. The DMA controller automatically transfers data to or from this register and RAM, and when the DMA data is finished (length=0) it causes a disk block interrupt. See interrupts below." |
| `sol:8-appa-dskdata.c8` | 279 | When disk-DMA length reaches zero, the controller requests a disk-block interrupt. (twin of `opus:8-appA-dskdat.c15`, narrower quote) | "and when the DMA data is finished (length=0) it\n                 causes a disk block interrupt." |

All 8 are `disposition: new`, all route to this package (`fixture:
paula_floppy`), and `package_purpose` for every one of them reads: "The three
interrupt kinds the controller can raise, and the conditions that raise
DSKSYNC and DSKBLK."

## Reduction to four testable propositions

- **P-THREE-KINDS** — `opus:8-dskint.c1`, `sol:8-int.c1` (twins, p.250). The
  disk controller's interrupt-raising outputs number exactly three.
- **P-DSKSYNC-MATCH** — `opus:8-dskint.c4`, `sol:8-int.c4` (twins, p.250). A
  stream match against the DSKSYNC register raises the DSKSYNC request,
  printed together with "(level 5, INTREQ bit 12)".
- **P-DSKBLK-COMPLETE** — `opus:8-dskint.c7`, `sol:8-int.c7` (twins, p.250).
  Disk DMA completion raises the DSKBLK request, printed together with
  "(level 1, INTREQ bit 1)".
- **P-LENZERO-BLCKINT** — `opus:8-appA-dskdat.c15`, `sol:8-appa-dskdata.c8`
  (twins, p.279, Appendix A DSKDATR/DSKDAT register description). DMA length
  reaching zero causes a disk block interrupt. No level or INTREQ-bit
  language in this quote.

`opus:8-dskint.c7`/`sol:8-int.c7` (P-DSKBLK-COMPLETE) and
`opus:8-appA-dskdat.c15`/`sol:8-appa-dskdata.c8` (P-LENZERO-BLCKINT) describe
what is disclosed to be the same underlying mechanism (`blckint` on the DMA
length counter reaching zero) from two different printed pages with two
different exact sentences. Per this package's own rule of one assertion per
*citing quotation*, they get four independent assertions, not two — each
demonstrated on its own trial, against its own page, under its own uid.

## The boundary: INTREQ and `_ipl` are not ports of `paula_floppy`

Disclosed by the build task, and independently consistent with
`classify.evidence`'s own module attribution (`paula_intcontroller.v:108`
`intreq[1]<=tmp[1]|blckint`; `:130` `intreq[12]<=tmp[12]|syncint`;
`:152-170` the priority encoder producing `_ipl`): the INTREQ register and
the `_ipl` priority output live at `paula_intcontroller`, one module up from
this package's fixture. `paula_floppy` only drives the two request-line
outputs `blckint` and `syncint`; it has no INTREQ port and no `_ipl` port.

Two of my four propositions print the interrupt number and priority level in
the same sentence as the request-line condition:

- P-DSKSYNC-MATCH's quote is "(level 5, INTREQ bit 12) -- input stream
  matches the DSKSYNC register."
- P-DSKBLK-COMPLETE's quote is "(level 1, INTREQ bit 1) -- disk DMA has
  completed."

Per this suite's rule for a claim that packages a fixture-visible half
together with a half that needs a wider boundary: **each assertion grades
only the request-line half** (the match condition raises `syncint`; DMA
completion raises `blckint`) **and declines the level/INTREQ-bit half**,
naming the boundary explicitly via `record()` rather than silently dropping
it or silently grading it as if `paula_floppy` could see it. P-LENZERO-
BLCKINT's quote carries no such language and needs no such split.

**P-THREE-KINDS gets the same treatment, for a related but distinct reason.**
`classify.evidence` (present in my own claims file, on `opus:8-dskint.c1` /
`sol:8-int.c1`) discloses that `paula_floppy` declares exactly three output
ports documented as disk-controller interrupt pulses:
`paula_floppy.v:96-97` `blckint` ("disk dma has finished interrupt") and
`syncint` ("disk syncword found"); `paula_floppy.v:93` `index` ("the disk
index pulse"), which the same evidence says `minimig.v:647` ties to `ciab`'s
`flag` input. That a fourth output does not exist is a fact about
`paula_floppy`'s own port list, gradeable at this fixture. That these three
—and only these three— actually *reach* the CPU as interrupts is not: for
`blckint`/`syncint` it needs `paula_intcontroller` (the same boundary as
above); for `index` it needs `paula_intcontroller` **and** `ciab`, which is
exactly the journey `hrm/ch8-disk-index-interrupt-path` is a dedicated
three-module rig for. So P-THREE-KINDS is graded here as: `paula_floppy`
exposes exactly three distinct, independently-triggerable pulse-type outputs
matching the page's count, and the CPU-visible-interrupt half of all three
(not only the INDEX one) is declined and named, not graded.

I do not yet know the RTL condition that produces a pulse on `index` — no
claim in this package's 8 is about it and neither the build task nor my
claims file's `classify.evidence` discloses its driving expression to me.
Discovering it (or discovering that it cannot be independently triggered
within this fixture, in which case P-THREE-KINDS will grade only two of the
three ports dynamically and say so) is deferred to the RTL-reading stage
below, per the order-of-work rule against reading ahead to shape a
specification.

## Fixture boundary: `paula_floppy`, assigned not derived

The build task assigns `paula_floppy` as this package's fixture, compiling
`paula_floppy.v` + `paula_floppy_fifo.v` only. `paula_floppy_ebr.v` declares
a **second** `module paula_floppy` and must not be compiled alongside it —
disclosed as a rig hazard by the build task, matching the same hazard
recorded independently in `hrm/ch8-drive-sense-lines` and
`hrm/ch8-dsklen-dma-arming`'s own specifications.

## Rig hazards (disclosed by the build task, not derived by me from RTL)

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680`, per the build
  task). Not directly relevant to this package's propositions (none of them
  step the drive), but the build task's mandate is unconditional: **a fresh
  `Vpaula_floppy` instance is allocated per trial**, matching the literal rig
  hazard mandate rather than relying on my own trials never happening to need
  head position.
- **DSKLEN needs a double write to arm DMA** (`paula_floppy.v:743`, per the
  build task). Measured and disclosed to me already: the first write leaves
  `data_out`=0x9000 and `fdd_led`=0; the second gives `data_out`=0xD000,
  `fdd_led`=1, `sdc_rd`=1; a single write (e.g. 0xC00A) leaves `dmas`=0.
  P-DSKBLK-COMPLETE and P-LENZERO-BLCKINT both need a completed DMA transfer,
  so both trials arm DSKLEN with this exact double-write sequence.
- **The MFM encoder is reported context-free** (finding candidate C9 in
  `work/2026-09-15_ch8_s2_finding_candidates.md`, a measured fact from a
  different lot's classifier, not one of my own claims): a sector fed as all
  `0x00` bytes reads back as `0xAAAA` on the internal data stream, all `0xFF`
  as `0xFFFF`. This is the mechanism P-DSKSYNC-MATCH's stimulus is planned
  around: feed a sector of one known fill byte, program DSKSYNC to the
  correspondingly disclosed encoded word for the match trial and to a
  deliberately different word (e.g. the classify stage's own `0x1234`, used
  in exactly this role in `classify.evidence`) for the no-match trial. This
  is a plan stated before RTL is opened, not a confirmed mechanism — the
  encoding, and whether it is visible to `sync_match`'s `floppy_data` input
  in the way I expect, is verified once `paula_floppy.v` is read, and this
  file's account will be corrected in the `.cpp` header if reading the RTL
  finds it working differently.

## Assertions — one per citing quotation, plus what this suite's own rules require

All page citations below repeat the source table above.

- **A1** — `opus:8-dskint.c1` (P-THREE-KINDS, p.250). Grades the port-count
  half only; declines the CPU-visible-interrupt half for all three ports.
  Expected GREEN.
- **A2** — `sol:8-int.c1` (P-THREE-KINDS, twin of A1, p.250). Expected GREEN.
- **A3** — `opus:8-dskint.c4` (P-DSKSYNC-MATCH, p.250). Grades the
  request-line half only; declines "level 5, INTREQ bit 12". Expected GREEN.
- **A4** — `sol:8-int.c4` (P-DSKSYNC-MATCH, twin of A3, p.250). Expected
  GREEN.
- **A5** — `opus:8-dskint.c7` (P-DSKBLK-COMPLETE, p.250). Grades the
  request-line half only; declines "level 1, INTREQ bit 1". Expected GREEN.
- **A6** — `sol:8-int.c7` (P-DSKBLK-COMPLETE, twin of A5, p.250). Expected
  GREEN.
- **A7** — `opus:8-appA-dskdat.c15` (P-LENZERO-BLCKINT, p.279). No boundary
  split needed. Expected GREEN.
- **A8** — `sol:8-appa-dskdata.c8` (P-LENZERO-BLCKINT, twin of A7, p.279).
  Expected GREEN.

None of my 8 claims are named among the sixteen finding candidates in
`work/2026-09-15_ch8_s2_finding_candidates.md`, and the build task states
this package's subject matter has already been measured green on a
throwaway rig — this is a **confirming** package, not a hunt for a red. If
RTL reading below turns up a disagreement with any of the eight quotes
above, that disagreement is reported as found, per this suite's rule that a
red is a valid, finished outcome — but none is predicted here.

Plus, ungraded by any manual citation but required by this suite's own
rules:

- **FIXTURE-LIVE** — the register write/read-back path must be shown to
  deliver two distinct, known, correct values before any claim-based read
  above is trusted.
- **INTERRUPT-LINES-IDLE-CONTROL** — this package's control case, on the
  same two ports (`blckint`, `syncint`) the claims above grade: with the
  fixture freshly reset and nothing armed or matched, both lines are
  expected to read deasserted (not stuck high) — proof that a "pulses when
  triggered" reading in A3-A8 is a real transition, not a line that is
  simply always asserted.

## Mutation plan

Each assertion above gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
record in the C++ source, `<label>` being the exact leading token of the
string literal passed to `expect()` — the claim's own `classify.uid` for
A1-A8, a plain uppercase id for FIXTURE-LIVE and
INTERRUPT-LINES-IDLE-CONTROL.

All ten are expected **GREEN at baseline** (this is a confirming package, no
red predicted per the section above), so all ten get an ordinary `stimulus
red` mutation: change what the rig *asks* the module to do (e.g. feed a
mismatching DSKSYNC in the match trial, skip the second DSKLEN write, drain
one word short, never actually deselect/reset before the idle-control
reading), leave the expected value alone, confirm the targeted assertion
alone goes red, then revert. If RTL reading turns up that any assertion is
in fact red at baseline, this section is corrected in the `.cpp` header and
the substitute-mechanism rule is used for that one instead, exactly as
`hrm/ch8-drive-sense-lines` did for its own single red.

Which exact stimulus each mutation changes, and the measured before/after
pass/fail tally, is recorded in the C++ source once the package is built and
run — this section states the plan, not a claim that any run has happened.

## What happens next

Only after this file is committed: open `paula_floppy.v` in full (port list;
the `blckint`/`syncint`/`index` declarations and their driving expressions;
the DSKLEN double-write and DMA state machine; the `sync_match` expression
and what `floppy_data`/`fifo_reading_sector`/`trackrd` actually are) and
`paula_floppy_fifo.v` in full; then write this package's own Makefile and
test source. All of that reading, and anything else opened beyond what is
named here, will be disclosed in the next commit's file header.
