# ch8-disk-interrupt-requests — SPECIFICATION

The package carries the quotations in
`hrm-testpipeline/work/ch8-s2-build/ch8-disk-interrupt-requests.jsonl` (the
8 claims assigned to this package). Its harness mechanics (the `Checks`
class, the `mutating(id)` / `MUTATE` convention, the fixed `// MUTATION`
form, the twin-claim pattern, the DSKLEN double-write arming and the
SD-sector ingest) follow `hrm/ch8-drive-sense-lines` and
`hrm/ch8-dsklen-dma-arming` on the same fixture; no conformance fact below
comes from them.

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

## Reduction to four propositions

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
  language in this quote, but "causes a disk block interrupt" names the same
  CPU-visible DSKBLK interrupt.

## The boundary: INTREQ and `_ipl` are not ports of `paula_floppy`

The INTREQ register and the `_ipl` priority output live at
`paula_intcontroller`, one module up from this package's fixture
(`classify.evidence`: `paula_intcontroller.v:108`
`intreq[1]<=tmp[1]|blckint`; `:130` `intreq[12]<=tmp[12]|syncint`;
`:152-170` the priority encoder producing `_ipl`). `paula_floppy` only
drives the local request-line outputs `blckint`, `syncint` and `index`; it
has no INTREQ port and no `_ipl` port.

Every one of this package's claims names a CPU-visible interrupt: "three
kinds of interrupts" for all three ports, "level 5, INTREQ bit 12" on
DSKSYNC, "level 1, INTREQ bit 1" on DSKBLK, and "causes a disk block
interrupt" in Appendix A. A uid printed on a graded line counts as fully
asserted by this suite's coverage ledger, whatever a `record()` beside it
says, so a local pin measurement cannot carry any of these uids:

- **P-DSKSYNC-MATCH, P-DSKBLK-COMPLETE, P-LENZERO-BLCKINT** (six uids) are
  declined via `record()`. Each is graded in full by
  `hrm/ch8-disk-interrupt-path` at `TOP_MODULE=minimig`, which observes the
  real INTREQ bit and `minimig.v`'s top-level `_cpu_ipl` port in the same
  cycle as the stimulus (PROP-DSKSYNC-FULLJOIN, PROP-DSKBLK-FULLJOIN). This
  package's own local `syncint`/`blckint` measurements are printed via
  `note()`.
- **P-THREE-KINDS** — `paula_floppy` declares exactly three output ports
  documented as disk-controller interrupt pulses (`classify.evidence`:
  `paula_floppy.v:96-97` `blckint`, `syncint`; `paula_floppy.v:93` `index`,
  which `minimig.v:657` ties to `ciab`'s `flag` input). That `index` is a
  real, independently-triggerable third pulse, distinct from the other two,
  is graded here under labels carrying no uid
  (P-THREE-KINDS-INDEX-PULSE-A/-B). The two uids appear only on a
  `record()`: `hrm/ch8-disk-interrupt-path` closes two of the three
  CPU-visible paths (DSKSYNC, DSKBLK) in one run;
  `hrm/ch8-disk-index-interrupt-path` closes INDEX's 8520 Flag-pin journey
  at `paula_intcontroller`'s own boundary but does not reach `_cpu_ipl` in
  the same run; so INDEX's own local-pin-to-`_cpu_ipl` join is not graded
  anywhere in this suite.

## Fixture boundary: `paula_floppy`

`paula_floppy.v` + `paula_floppy_fifo.v` only. `paula_floppy_ebr.v`
declares a **second** `module paula_floppy` and is not compiled alongside
it — the same hazard `hrm/ch8-drive-sense-lines` and
`hrm/ch8-dsklen-dma-arming` record.

## Rig hazards

- **`dsktrack` has no reset term** (`paula_floppy.v:671-680`). None of this
  package's trials steps the drive, but **a fresh `Vpaula_floppy` instance
  is allocated per trial** regardless, so nothing relies on trials never
  needing head position.
- **DSKLEN needs a double write to arm DMA** (`paula_floppy.v:743`): the
  first write leaves `data_out`=0x9000 and `fdd_led`=0; the second gives
  `data_out`=0xD000, `fdd_led`=1, `sdc_rd`=1; a single write (e.g. 0xC00A)
  leaves `dmas`=0. The DSKBLK and length-zero measurements need a completed
  DMA transfer, so both arm DSKLEN with this exact double-write sequence.
- **The DSKSYNC match uses the structural sync word.** `floppy_data`, the
  stream `sync_match` compares against, carries the fixed sync mark 0x4489
  at words 2/3 of every sector, and DSKSYNC's reset default is also 0x4489,
  so an ordinary DMA read with DSKSYNC left untouched produces a genuine
  match; 0x1234 is used where a trial needs no match (see the test source's
  header for the mechanism).

## Checks

- **FIXTURE-LIVE** — the register write/read-back path delivers two
  distinct, known, correct values (the DSKBYTR WRITE-bit mirror). A
  `note()`: it carries no claim uid.
- **INTERRUPT-LINES-IDLE-CONTROL** — this package's control case, on the
  same two ports (`blckint`, `syncint`) the local measurements read: with
  the fixture freshly reset and nothing armed or matched, both lines read
  deasserted (not stuck high) — proof that a "pulses when triggered"
  reading is a real transition, not a line that is simply always asserted.
  Graded, GREEN.
- **P-THREE-KINDS-INDEX-PULSE-A/-B** — `index` asserts on a `sof` pulse at
  `rpm_pulse_cnt`==0 with a drive selected and its motor commanded on,
  while `blckint` and `syncint` stay deasserted. Graded, GREEN, no uid on
  the label.
- **P-DSKSYNC-MATCH, P-DSKBLK-COMPLETE, P-LENZERO-BLCKINT** — local
  measurement printed via `note()`, uids declined via `record()` naming the
  `hrm/ch8-disk-interrupt-path` trial that grades them.

Three chapter-7 uids (`opus:7-int.c54`, `opus:7-fig7-4.c6`,
`opus:7-fig7-4.c39`, "Bit 1, DSKBLK, indicates 'disk block finished'" and
Figure 7-4) are also declined here via `record()`: they are CPU-visible
statements about INTREQ bit 1, and no package in the suite grades them.

## Mutation plan

Each graded check has its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
record in the C++ source, `<label>` being the exact leading token of the
string literal passed to `expect()`. All three are GREEN at baseline, so
each gets an ordinary `stimulus red` mutation: change what the rig *asks*
the module to do (exercise `syncint`'s separate idle-`sof` term instead of
leaving the fixture idle; never pulse `sof`), leave the expected value
alone, confirm the targeted check alone goes red, then revert. A `note()`
or `record()` carries no mutation record.
