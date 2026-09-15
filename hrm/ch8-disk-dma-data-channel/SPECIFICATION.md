# ch8-disk-dma-data-channel — SPECIFICATION

Written from the quotations in `ch8-disk-dma-data-channel.jsonl` alone (19
claims). No RTL, Makefile, or other package's files have been opened yet.
This file is committed before any of that happens.

Two facts from the build brief and from
`work/2026-09-15_ch8_s2_finding_candidates.md` are used below without being
independently re-derived from RTL, because they were handed to me directly as
already-measured: the fixture must be a rig over `paula_floppy` +
`agnus_diskdma` (compiling `paula_floppy.v` + `paula_floppy_fifo.v` +
`agnus_diskdma.v`, never `paula_floppy_ebr.v`), and `dsktrack`
(`paula_floppy.v:671-680`) has no reset term, so a reused Verilated model
carries head position across trials — every trial below allocates a fresh
model. Neither fact tells me what any assertion's expected value is; both are
rig-construction constraints, not readings of module behaviour. The double
write DSKLEN needs to arm DMA, and its two intermediate values
(`data_out`=0x9000/`fdd_led`=0 after the first write, 0xD000/`fdd_led`=1/
`sdc_rd`=1 after the second) are likewise given, not rediscovered — used only
to arm the fixture, not as an expected value under test.

## Source claims (verbatim quotations)

| uid | page | quote (trimmed) |
|---|---|---|
| `opus:8-appA-dskdat.c9` | 279 | "This register is the disk DMA data buffer. It contains two bytes of data that are either sent (written) to or received (read) from the disk. ... The DMA controller automatically transfers data to or from this register and RAM..." |
| `opus:8-appA-dskdat.c10` | 279 | same paragraph |
| `opus:8-appA-dskdat.c11` | 279 | same paragraph |
| `opus:8-appA-dskdat.c13` | 279 | same paragraph |
| `sol:8-appa-dskdata.c1` | 279 | "This register is the disk DMA data buffer." |
| `sol:8-appa-dskdata.c2` | 279 | "It contains two bytes of data that are either sent (written) to or received (read) from the disk." |
| `sol:8-appa-dskdata.c3` | 279 | same sentence |
| `sol:8-appa-dskdata.c4` | 279 | same sentence |
| `sol:8-appa-dskdata.c6` | 279 | "The DMA controller automatically transfers data to or from this register and RAM," |
| `sol:8-appa-dskdata.c7` | 279 | same clause |
| `opus:8-appB-legend.c8` | 287 | "ER Early read. This is a DMA data transfer to RAM, from either the disk or the blitter. ... These transfers are therefore initiated by Agnus timing, rather than a read address on the destination address bus." |
| `opus:8-appB-dskdatr.c2` | 289 | "DSKDATR & *008 ER P Disk data early read (dummy address)" |
| `opus:8-appB-dskdatr.c4` | 289 | same row |
| `opus:8-appB-dskdat.c2` | 289 | "DSKDAT & *026 W P Disk DMA data write" |
| `sol:8-appb-legend.c15` | 287 | "ER Early read. This is a DMA data transfer to RAM, from either the disk or the blitter." |
| `sol:8-appb-legend.c17` | 287 | "These transfers are therefore initiated by Agnus timing, rather than a read address on the destination address bus." |
| `sol:8-appb-dskdatr.c2` | 289 | same DSKDATR row (twin of `opus:8-appB-dskdatr.c2`) |
| `sol:8-appb-dskdatr.c4` | 289 | same DSKDATR row (twin of `opus:8-appB-dskdatr.c4`) |
| `sol:8-appb-dskdat.c2` | 289 | same DSKDAT row (twin of `opus:8-appB-dskdat.c2`) |

## Propositions, grouped

Nineteen uids reduce to nine distinct testable propositions. Where two or
more uids state the identical proposition from different citing passages,
each still gets its own `expect()` so each uid is individually gradeable by
the coverage ledger — matching the precedent in
`hrm/ch8-ciaapra-address-window` (four uids, four separate `expect()` calls,
one shared measurement).

1. **DSKDAT/DSKDATR is the disk-DMA data buffer** (existence/identity, not
   width or direction) — `opus:8-appA-dskdat.c9`, `sol:8-appa-dskdata.c1`.
2. **The buffer holds two bytes** (16-bit width, both bytes move as one
   word) — `opus:8-appA-dskdat.c10`, `sol:8-appa-dskdata.c2`.
3. **Writing the buffer sends data toward the disk** —
   `sol:8-appa-dskdata.c3`, and the write half of `opus:8-appA-dskdat.c11`
   (whose quote states both directions in one sentence, so it is graded on
   both this proposition and proposition 4).
4. **Reading the buffer receives data from the disk** —
   `sol:8-appa-dskdata.c4`, and the read half of `opus:8-appA-dskdat.c11`.
5. **The DMA controller automatically moves data from RAM into DSKDAT for a
   disk write**, with no CPU register access driving each transfer —
   `sol:8-appa-dskdata.c6`, and the write half of `opus:8-appA-dskdat.c13`
   (same both-directions sentence as c11, graded on both 5 and 6).
6. **The DMA controller automatically moves data from DSKDATR into RAM for a
   disk read**, same automation property — `sol:8-appa-dskdata.c7`, and the
   read half of `opus:8-appA-dskdat.c13`.
7. **ER ("early read"): the disk-DMA read transfer is initiated by Agnus's
   own timing, not by a read address presented to the register, and the
   register it presents is DSKDATR — disk half only** (blitter half and the
   generic RAM-timing rationale are out of scope; see the attack-constraint
   section below) — `opus:8-appB-legend.c8`, `sol:8-appb-legend.c15`,
   `sol:8-appb-legend.c17`, `opus:8-appB-dskdatr.c4`, `sol:8-appb-dskdatr.c4`.
8. **DSKDATR is used by the disk-DMA channel — "used by", not "used only
   by"** — `opus:8-appB-dskdatr.c2`, `sol:8-appb-dskdatr.c2`. Constrained by
   the attack stage; see below.
9. **DSKDAT is used by the disk-DMA channel — "used by", not "used only
   by"** — `opus:8-appB-dskdat.c2`, `sol:8-appb-dskdat.c2`. Constrained by
   the attack stage; see below.

## The attack constraint: which half of each of the four claims is graded

`opus:8-appB-dskdatr.c2`, `sol:8-appb-dskdatr.c2`, `opus:8-appB-dskdat.c2`
and `sol:8-appb-dskdat.c2` each cite the same printed row mark: an `&`
between the register's name and its address, glossed on p.287 as "Register
used by DMA channel only." Each carries `attack.original_verdict:
partly-overturned` and a note that the assertion "must grade only the half
the evidence names."

**Graded half — the positive claim, "used by the disk DMA channel":**
`agnus_diskdma` selects `DSKDATR`/`DSKDAT` as `reg_address_out` for disk
transfers, automatically, as part of the same granted-DMA-slot mechanism
graded under propositions 5–7 above. This half has a concrete, named
observable: `reg_address_out` at a granted slot.

**Not graded — the word "only", i.e. exclusivity:** that DSKDATR/DSKDAT are
used *exclusively* by the disk DMA channel, to the exclusion of any other bus
master. This half is not graded because there is nothing in either module's
interface that could ever falsify it. Per the attack's own evidence
(reproduced in the claims file's `classify.evidence`), `paula_floppy`'s
`busrd`/`buswr` decode is a plain function of `reg_address_in` with no
separate chip-select or requester-identity input in its port list — so a bus
cycle at this boundary carries no information about *who* placed the address
on the bus. There is no signal at this rig's boundary that distinguishes "the
DMA channel drove $008" from "something else drove $008"; asserting
exclusivity would therefore not be testing the register, it would be testing
whether I built a rig that happens to be the only thing wired to
`reg_address_in`, which is a fact about the rig, not about the chip. That is
why the word is ungradeable here rather than merely unchecked: no stimulus in
either module's input space can make the exclusivity claim fail, so there is
no way to construct a discriminating test for it at this boundary — this is
the "unfalsifiable at this boundary" reading of an ungraded half, not a scope
choice.

The same reasoning applies to `opus:8-appB-legend.c8`'s exclusion of the
blitter half and the generic RAM-timing rationale: `agnus_blitter` is a
different module outside this package's assigned boundary, and "RAM timing
requires data to be on the bus earlier than microprocessor read cycles" names
a hardware timing margin with no corresponding observable signal in either
`paula_floppy` or `agnus_diskdma` — there is nothing to compare a measured
number against. Only the disk-DMA mechanics actually wired between these two
modules are graded under proposition 7.

## Fixture

A rig over `paula_floppy` + `agnus_diskdma`, instantiating both (source:
`paula_floppy.v`, `paula_floppy_fifo.v`, `agnus_diskdma.v` — `paula_floppy_ebr.v`
excluded, per the rig hazard already given: it declares a second `module
paula_floppy`). The exact port names and the wiring between the two modules
(how `agnus_diskdma`'s slot-grant and `reg_address_out`/`wr` reach
`paula_floppy`'s bus-facing ports) are read from the RTL and from
`minimig.v`'s own instantiation of both, after this file is committed, the
same order `hrm/ch8-ciaapra-address-window` used for its `ciaa`+`gary` rig.

**Rig hazard, disclosed before the RTL is read:** `dsktrack` has no reset
term, so every trial below constructs its own fresh `Vtop` instance (`new`
before, `delete` after) rather than reusing one object across `do_reset()`
calls — a plain reset is not sufficient at this boundary.

**Arming disk DMA:** per the build brief, `DSKLEN` needs two writes to arm a
transfer (`paula_floppy.v:743`); the first write alone leaves `data_out`
=0x9000 and `fdd_led`=0, the second gives 0xD000, `fdd_led`=1, `sdc_rd`=1.
The fixture drives this double write as setup for any trial that needs an
armed, running disk DMA (propositions 3–7); it is not itself under test.

## Assertions — one per proposition, most citing more than one uid, matching precedent's one-`expect()`-per-citing-uid rule

| id | uid(s) | page | proposition | direction expected |
|---|---|---|---|---|
| L1 | — (fixture liveness, read side) | — | two distinct SD-sourced words, driven into the read-DMA path, arrive distinct and unmodified at `data_out` when read at DSKDATR | must PASS or every read-direction/DSKDATR assertion below is unproven |
| L2 | — (fixture liveness, write side) | — | two distinct words, written at DSKDAT, arrive distinct and unmodified at the disk-facing side (`sdc_byte_out_data`) | must PASS or every write-direction/DSKDAT assertion below is unproven |
| CTRL1 | — (control case) | — | reading a different, known-constant register (DSKBYTR, $01A) through the same rig returns *that* register's own value, distinct from the DSKDAT/DSKDATR FIFO content — proves the address decode discriminates rather than always answering one register | PASS, same measurement basis as B1/B2 |
| B1 | `opus:8-appA-dskdat.c9` | 279 | DSKDATR answers with the FIFO's own (non-idle-constant) content, not a dummy/unimplemented value | PASS expected |
| B2 | `sol:8-appa-dskdata.c1` | 279 | twin of B1 | PASS expected |
| W1 | `opus:8-appA-dskdat.c10` | 279 | a full 16-bit, two-byte word passes through the buffer unmodified | PASS expected |
| W2 | `sol:8-appa-dskdata.c2` | 279 | twin of W1 | PASS expected |
| DW1 | `sol:8-appa-dskdata.c3` | 279 | a word written at DSKDAT reaches the disk-facing (`sdc_*`) side | PASS expected |
| DW2 | `opus:8-appA-dskdat.c11` (write half) | 279 | same fact, second citing passage | PASS expected |
| DR1 | `sol:8-appa-dskdata.c4` | 279 | a word supplied on the disk-facing side reaches `data_out` when read at DSKDATR | PASS expected |
| DR2 | `opus:8-appA-dskdat.c11` (read half) | 279 | same fact, second citing passage | PASS expected |
| AW1 | `sol:8-appa-dskdata.c6` | 279 | during a granted slot with the write direction selected, `reg_address_out` automatically presents DSKDAT and the address pointer advances, with no CPU register access per transfer | PASS expected |
| AW2 | `opus:8-appA-dskdat.c13` (write half) | 279 | same fact, second citing passage | PASS expected |
| AR1 | `sol:8-appa-dskdata.c7` | 279 | same automation, read direction, DSKDATR | PASS expected |
| AR2 | `opus:8-appA-dskdat.c13` (read half) | 279 | same fact, second citing passage | PASS expected |
| ER1 | `opus:8-appB-legend.c8` | 287 | disk half of ER: transfer initiated by Agnus's own slot timing, register presented is DSKDATR, no read address involved | PASS expected |
| ER2 | `sol:8-appb-legend.c15` | 287 | ER moves data to RAM from disk (disk half) | PASS expected |
| ER3 | `sol:8-appb-legend.c17` | 287 | ER initiated by Agnus timing, not a read address | PASS expected |
| ER4 | `opus:8-appB-dskdatr.c4` | 289 | DSKDATR's row R/W cell (ER) composed with the legend | PASS expected |
| ER5 | `sol:8-appb-dskdatr.c4` | 289 | twin of ER4 | PASS expected |
| CHDR1 | `opus:8-appB-dskdatr.c2` | 289 | DSKDATR used by disk DMA channel — positive half only | PASS expected |
| CHDR2 | `sol:8-appb-dskdatr.c2` | 289 | twin of CHDR1 | PASS expected |
| CHDW1 | `opus:8-appB-dskdat.c2` | 289 | DSKDAT used by disk DMA channel — positive half only | PASS expected |
| CHDW2 | `sol:8-appb-dskdat.c2` | 289 | twin of CHDW1 | PASS expected |

24 `expect()` calls, all 19 uids each cited at least once (`opus:8-appA-dskdat.c11`
and `opus:8-appA-dskdat.c13` each cited twice, once per direction, because
each one's own quotation states both directions in a single sentence).

None of these nine propositions appear among the sixteen red-expected
candidates in `work/2026-09-15_ch8_s2_finding_candidates.md` (C1–C16); every
one of that file's disk-DMA-adjacent entries (C15 in particular — "DSKDATR
$008 is not a dummy address here") supports these claims rather than
contradicting them. Every assertion above is expected to PASS against the
RTL. If reading the RTL shows otherwise, the assertion still gets built and
its red result reported as-is — this table records what is *expected* before
the RTL is opened, not a promise about what will be found.

## Mutation plan

Because every assertion above is expected to PASS at baseline (none are
`partly-overturned`-and-still-red in this claims file — the four
attack-constrained ones were promoted specifically because their graded half
is positive), every mutation below is the standard `stimulus`/`red` form:
change what the fixture asks the module to do, leave the expected value
alone, confirm the targeted assertion alone goes red, revert. Each gets its
own `// MUTATION <label> stimulus red: ...` line in the C++ source, `<label>`
being the exact leading token of that assertion's `expect()` label. L1/L2
(liveness) and CTRL1 (control) get mutation records too, per the mandate that
mutation coverage includes the liveness proof and the control case, not just
manual-citing assertions.

## What happens next

Only after this file is committed: read `paula_floppy.v`, `paula_floppy_fifo.v`
and `agnus_diskdma.v` in full for port names and the internal facts already
disclosed in the build brief (`dma`, `dmaslot`, `wr`, `reg_address_out`
equations; `busrd`/`buswr` decode; the DSKLEN arming sequence); read
`minimig.v` for how these two modules are wired to each other in the real
chip (the `dmal`/`dmas`/`reg_address_out` handoff), the same way
`hrm/ch8-ciaapra-address-window` read `minimig.v` for its `ciaa`+`gary`
wiring; and read an existing package's test harness (candidate:
`hrm/ch8-ciaapra-address-window/test_ch8-ciaapra-address-window.cpp`) to match
this suite's established `Checks`/`mutating()`/`tick()`/`settle()`
boilerplate. All of that reading, and anything else opened beyond what is
named here, will be disclosed in the next commit's test-file header.
