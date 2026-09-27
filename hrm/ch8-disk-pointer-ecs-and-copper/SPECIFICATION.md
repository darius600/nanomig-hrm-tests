# Specification: ch8-disk-pointer-ecs-and-copper

Fixture: `agnus` (the Agnus top level, not `agnus_diskdma`). Subject: the
Appendix B DSKPTH row's OCS/ECS bit-width split and `(E)` mark (p.289,
composed with p.288), and p.279's claim that the Copper may initialise the
disk pointer before DMA is enabled.

The package grades the six claims in `ch8-disk-pointer-ecs-and-copper.jsonl`.
No fact about a port, a bit range or a measured value below is a fresh
reading of the RTL; each is drawn from the claims' own `classify.evidence`
fields or from a measurement elsewhere in this suite, named where it is used.

## Why `agnus`, not `agnus_diskdma`

The sibling package `ch8-disk-dma-pointer` measures, at the `agnus_diskdma`
boundary, that a `$020` write moves `data_in[4:0]` into `address_out[20:16]`
-- five bits, unconditionally. `agnus_diskdma` has no `ecs` port at all
(`agnus.v:326-340` instantiates it with none), so that boundary cannot put
the OCS "high 3 bits" reading or the `(E)` "5 bits if ECS" reading to the
question; the sibling declines both and names P5 and P4 here.

What `agnus_diskdma`'s boundary cannot do is ask whether a *real*, drivable
`ecs` signal -- one that exists as an actual input somewhere in this
design -- changes anything about the pointer. `ecs` is a real port on
`agnus` itself. This package drives it there and observes whether it makes
any difference to what reaches the pointer, which is the one question the
sibling package structurally cannot put.

The same wider boundary is also the only place a Copper-issued write can be
told apart from a CPU-issued write to the same register: `agnus_diskdma`'s
`reg_address_in`/`data_in` carry an address and a value and nothing that
says who put them there, so a claim about *the Copper specifically* being
able to initialise the pointer cannot be settled one level down.

## Why the Copper is aimed at the pointer only with `ecs`=0

The Copper's write-protect logic (`agnus_copper`) blocks $008-$026 (which
contains $020/DSKPTH and $022/DSKPTL) at OCS/cdang=0, OCS/cdang=1 **and**
ECS/cdang=0, and lets a Copper write reach only at ECS/cdang=1;
`hrm/ch2-copper-move-sweep` measures the Copper's accepted destinations at
all four corners. This package **grades the printed `*` mark with `ecs`=0**,
where Appendix B's `*` on $020 (p.289) stands without p.310's ECS extension.
The ECS/cdang=1 reach case is p.310's ECS sentence, which NM-HRM-053 records and `hrm/ch2-copper-move-sweep`'s `SWEEP ecs=1 cdang=1 vs
Appendix C` grades; p.310 is not among this package's pages. This package
therefore only ever drives `ecs`=0 when a Copper MOVE is aimed at the
protected pointer registers and does not build an ECS/cdang=1 Copper-write
assertion. (T4/T6 in `agnus_copper_move_range` are not usable evidence here
-- they exercise $07E under the printed name BLTSIZE, not a disk register.)

## Propositions

### P1 -- the Copper may initialise DSKPTH before disk DMA is enabled [sol:8-appa-dskptr.c3, p.279]

> "These address registers must be initialized by the processor or Copper
> before disk DMA is enabled." (p.279)

Restatement graded here: the Copper half only -- the processor half of the
same sentence is graded by the sibling package's P9, at the narrower
`agnus_diskdma` boundary (`sol:8-appa-dskptr.c2`).

Testable form: with a known value already resident at the pointer (loaded
by a CPU write, which is not in dispute), a Copper MOVE instruction
targeting $020 (DSKPTH), executed with `ecs`=0, changes the pointer to the
MOVE's operand.

Expected: **RED**. $020 is inside $008-$026, which the Copper's
write-protect logic blocks at OCS regardless of `cdang` (see above); the
Copper cannot reach it at `ecs`=0.

DSKPTL ($022) is not independently exercised through a second, chained
Copper MOVE: an illegal MOVE halts the Copper's own instruction sequencer
(per the classify evidence) rather than letting it continue to a next
instruction, so a list of two MOVEs cannot demonstrate anything about the
second one once the first is blocked. $022 sits in the same $008-$026
range, blocked under the same condition; this is recorded, not separately
asserted.

### P2 -- the high three pointer bits ARE carried by DSKPTH [opus:8-appB-dskpth.c6, p.289]

> "DSKPTH + *020 W A (E) Disk pointer (high 3 bits, 5 bits if ECS)" (p.289)

Restatement graded here (per the claim's own `split`): the weak/existence
reading -- the three bits ARE among what DSKPTH carries. The stronger
"only three" reading is a different claim, graded under P5.

Testable form: a $020 write's low three bits (`data_in[2:0]`) reach
`address_out[18:16]`.

Expected: **GREEN**.

### P3 -- under ECS, DSKPTH holds five bits [opus:8-appB-dskpth.c7 + sol:8-appb-dskpth.c10, p.289]

> "DSKPTH + *020 W A (E) Disk pointer (high 3 bits, 5 bits if ECS)" (p.289)

These two claims are recorded as mutual twins in the classification and
state the identical graded proposition (the ECS half of the same row, from
two readers): under ECS the register holds five bits. One assertion, both
uids cited. (Per each claim's own `split`, the "instead of 3" delta is not
graded here -- that is P5's territory, at `ecs`=0.)

Testable form: with `ecs`=1, a $020 write's low five bits (`data_in[4:0]`)
reach `address_out[20:16]`.

Expected: **GREEN**.

### P4 -- the `(E)` mark: ECS changes DSKPTH [sol:8-appb-dskpth.c8, p.289 composed with p.288]

> "DSKPTH + *020 W A (E) Disk pointer (high 3 bits, 5 bits if ECS)" (p.289),
> composed with the p.288 definition of the `(E)` mark.

Testable form, and the one this package exists to put: the *same* $020
write, presented once with a real `ecs`=0 and once with a real `ecs`=1 at
the `agnus` boundary, produces two different pointer results.

Expected: **RED**. `agnus_diskdma` (where the pointer bits are actually
decoded) has no `ecs` input and `agnus.v:326-340` wires none in, so driving
the real `ecs` port at `agnus` cannot reach the mechanism that would need
to see it -- this is the question the sibling package's boundary could not
ask, asked here and answered.

### P5 -- on OCS, DSKPTH holds *only* the high three bits [sol:8-appb-dskpth.c9, p.289]

> "DSKPTH + *020 W A (E) Disk pointer (high 3 bits, 5 bits if ECS)" (p.289)

Testable form: with `ecs`=0, a $020 write's bits 3 and 4 do **not** reach
`address_out[19]`/`address_out[20]`.

Expected: **RED**, for the same single cause as P4 above: the five-bit load
is unconditional, so bits 3 and 4 land regardless of `ecs`.

## Fixture liveness and control

Two checks are not tied to one printed claim but are required before any
proposition above is trusted:

- **CPU-path liveness**: DSKPTH and DSKPTL are each loaded, via an ordinary
  CPU register write, with a distinct, recognisable bit pattern, and the
  combined pointer is read back and checked against the exact expected
  value. This is the same measurement basis P2/P3/P5 use (a register
  write, then `address_out` inspected for which bits arrived), so it also
  serves as this package's required control on that basis, alongside
  P2/P3.
- **Copper-path liveness**: since P1 depends on a Copper MOVE actually
  being able to reach *some* register through this harness, a MOVE to an
  always-unprotected register (DMACON, $096, outside $008-$026 at any
  `cdang`/`ecs`) is issued and its effect confirmed by a CPU read of
  DMACONR. Without this, a red P1 would be indistinguishable from a harness
  that cannot make the Copper write anything at all.

## Not turned into assertions

- **DSKPTL's half of P1** -- see P1's own note: the Copper halts on the
  first blocked MOVE, so a second instruction in the same list is never
  reached, and $022 is in the same measured-blocked range as $020. Recorded
  under P1, not separately graded.
- **The ECS/cdang=1 reach case for the Copper write-protect mark** -- this
  is p.310's ECS sentence, which NM-HRM-053 records and
  `hrm/ch2-copper-move-sweep` grades; it is not graded here. The run prints
  a `MEASURED` line stating that no Copper MOVE in this package reaches it.

## Claims accounted for

All six claims in `ch8-disk-pointer-ecs-and-copper.jsonl` are accounted
for: P1 (1: `sol:8-appa-dskptr.c3`), P2 (1: `opus:8-appB-dskpth.c6`), P3 (2,
merged: `opus:8-appB-dskpth.c7`, `sol:8-appb-dskpth.c10`), P4 (1:
`sol:8-appb-dskpth.c8`), P5 (1: `sol:8-appb-dskpth.c9`).

## Mutation plan

Every graded proposition gets one mutation, recorded in
`test_ch8_disk_pointer_ecs_and_copper.cpp`'s header, with the baseline
colour named beside the result:

- LIVENESS (CPU path, green baseline): stimulus -- feed a different DSKPTH
  value than the one the hardcoded expected constant was built from; expect
  red. Revert.
- LIVENESS (Copper path, green baseline): stimulus -- retarget the MOVE
  from DMACON to a protected register (DSKPTH) under the graded `ecs`=0
  condition; expect red (the write no longer reaches, so DMACONR's probed
  bit never sets). Revert.
- P2 (green baseline): stimulus -- change the written low-three-bit
  pattern from what the hardcoded expected constant matches; expect red.
  Revert.
- P3 (green baseline): stimulus -- change the written five-bit pattern from
  what the hardcoded expected constant matches; expect red. Revert.
- P1 (red baseline): a mutation cannot fail this harder. Substitute -- the
  same comparator (does the pointer end up holding the operand value,
  observed through the disk-slot readback), driven by a CPU write instead
  of a Copper MOVE to the same register; expect green (this proves the
  comparator and the readback path work, not that the finding is fake).
  Revert.
- P4 (red baseline): substitute -- the same inequality comparator between
  two captured pointer reads, but vary the *written data* between the two
  reads instead of `ecs`; expect green (proves the comparator can detect a
  real difference, ruling out a comparator that always reports "no
  change"). Revert.
- P5 (red baseline): substitute -- the same "does this input bit fail to
  reach this output position" comparator, applied to DSKPTL bit 0 against
  `address_out` (which is `[20:1]`, so no storage exists for a word's bit
  0, a structurally different and independently true non-propagation);
  expect green. Revert.

All mutation results, with the baseline colour beside each, are recorded in
the test source's header comment.
