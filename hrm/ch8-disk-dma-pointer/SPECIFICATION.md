# Specification: ch8-disk-dma-pointer

Fixture: `agnus_diskdma`. Subject: DSKPTH ($DFF020) / DSKPTL ($DFF022), the
disk DMA pointer pair, printed pp.241, 246-247, 279 and 287-289.

Written from the quotations in `ch8-disk-dma-pointer.jsonl` alone (55
claims). No RTL has been opened to produce this document; every fact about
port names, bit ranges and measured values below is drawn from the
`classify.evidence` fields the classification stage already recorded on
each claim, not from a fresh reading. Every claim is cited by its
`classify.uid` (`<reader>:<id>`), never by the bare `id`, per BUILD-BRIEF.

Two candidates raised during classification named this package as their
deliverable:

- **C7** -- `$020` loads `data_in[4:0]` into `address_out[20:16]`, five bits,
  not the three the p.279 OCS reading gives.
- **C14** -- the same five-bit load is unconditional: `agnus_diskdma` has no
  `ecs` port, so the appendix's `(E)` mark ("5 bits if ECS", implying a
  chipset-dependent difference) also fails, for the same single cause.

**REPAIRED 2026-09-15**, per independent stage-8 validation
(`hrm-testpipeline/work/2026-09-15_ch8_s2_stage8_validation.md` and
`_sol_validation_A.md`, sections 6/9/10): C7 and C14 are one and the same
underlying fact -- DSKPTH's high field is five bits wide and loads
unconditionally -- and that fact is kept once, properly, at the sibling
package `ch8-disk-pointer-ecs-and-copper`'s `agnus` boundary, where `ecs` is
a real, drivable port (its P4 and P5). `agnus_diskdma` has no `ecs` port at
all, so this boundary can only observe the unconditional width; it cannot
put the OCS/ECS conditional question the sibling's boundary can. P4A, P4B
and P11B (below) are therefore **declined**, each with a `record()` naming
the sibling assertion that carries the fact -- not graded here. **P11A is a
separate, distinct finding** (see below) and remains graded RED: it is the
p.241/p.246 "32 bits" wording against the stored 20 bits, not the OCS/ECS
conditioning fact, and the two are never to be merged.

## Reading the DSKDATR/DSKDAT direction claims: what is graded and what is not

Two of this package's 55 claims (`sol:8-dskpth.c2`, `sol:8-dskpth.c3`) cite
`reg_address_out` landing on DSKDATR ($008) or DSKDAT ($026) as evidence for
which direction (source/destination) the pointer serves. Neither claim's own
proposition is about *exclusivity* of that register to the DMA channel --
that is a different pair of claims (`*:8-app[Bb]-dskdatr.c2`,
`*:8-app[Bb]-dskdat.c2`), assigned to the sibling package
`ch8-disk-dma-data-channel`, not to this one. P10 below grades only the
directional fact this package's own two claims state (which address
`reg_address_out` shows for which `wr`/`dmas` combination); it does not
grade, and does not need to grade, the word "only."

## Propositions

### P1 -- DSKPTH is decoded at chip-register offset $DFF020

> "DSKPTH ($DFF020) DMA pointer (32 bits)" (p.241)
> "The value must be written as a single long word to the DSKPTH register
> ($DFF020)." (p.246)
> "DSKPTH 020 W A( E ) Disk pointer (high 3 bits, high 5 bits if ECS)" (p.279)
> "DSKPTH + *020 W A (E) Disk pointer (high 3 bits, 5 bits if ECS)" (p.289)

Testable form: presenting `reg_address_in` = 9'h020[8:1] together with a
`data_in` value causes that value's low five bits to reach
`address_out[20:16]`; presenting a different address does not.

Cited by: `opus:8-dsk.c32` (p.241), `opus:8-dma.c11` (p.246),
`sol:8-dskpth.c7` (p.246, twin), `opus:8-appA-dskpt.c1` (p.279),
`sol:8-appa-dskpth.c1` (p.279, twin), `opus:8-appB-dskpth.c1` (p.289),
`sol:8-appb-dskpth.c1` (p.289, twin).

### P2 -- DSKPTL is decoded at chip-register offset $DFF022

> "DSKPTL 022 W A Disk pointer (low 15 bits)" (p.279)
> "DSKPTL + *022 W A Disk pointer (low 15 bits)" (p.289)

Testable form: presenting `reg_address_in` = 9'h022[8:1] together with a
`data_in` value causes that value's bits [15:1] to reach `address_out[15:1]`.

Cited by: `opus:8-appA-dskpt.c6` (p.279), `sol:8-appa-dskptl.c1` (p.279,
twin), `opus:8-appB-dskptl.c1` (p.289), `sol:8-appb-dskptl.c1` (p.289, twin).

### P3 -- DSKPTH and DSKPTL are write-only: neither is readable back through this boundary -- DECLINED 2026-09-15 (repair lot D)

**Declined, not asserted.** Independent validation found the previous
`expect(true, ...)` form was unfalsifiable by construction (the literal
condition `true`) -- it restated this boundary's lack of a data-output port
as its own result, proving neither register write-only rather than testing
it. Converted to `record()`; the structural fact itself (no `rd`/`hwr`/`lwr`
strobe, no data-output port of any kind) is unchanged and still printed via
`note()`.

> "DSKPTH 020 W ..." / "DSKPTL 022 W ..." (p.279, the R/W column)
> "DSKPTH + *020 W ..." / "DSKPTL + *022 W ..." (p.289, composed with the
> p.287 legend "W=write-only; R=read-only")

Per the claims' own `boundary_caveat`: `agnus_diskdma` has no `rd`/`hwr`/
`lwr` strobe and, per its evidence-recorded port list (`reg_address_in`,
`data_in`, `address_out`, `reg_address_out`, `dmal`, `dmas`, `dma`, `wr`,
`hpos`, `speed`), **no data-output port of any kind**. Write-only is graded
as the structural fact that follows: a value presented at $020 or $022
reaches state (P1/P2), and no port on this module ever answers with it.
This is unfalsifiable *by stimulus* -- no input configuration can create an
output port the compiled module does not have -- so its mutation record is
`none`/`impossible`, not a stimulus flip.

Cited by: `opus:8-appA-dskpt.c2` (p.279, DSKPTH), `sol:8-appa-dskpth.c2`
(p.279, twin), `opus:8-appA-dskpt.c7` (p.279, DSKPTL),
`sol:8-appa-dskptl.c2` (p.279, twin), `opus:8-appB-dskpth.c4` (p.289,
composed), `sol:8-appb-dskpth.c6` (p.289, twin), `opus:8-appB-dskptl.c4`
(p.289, composed), `sol:8-appb-dskptl.c6` (p.289, twin).

### P4A -- the OCS reading ("high 3 bits") -- DECLINED 2026-09-15 [C7/C14]

> "DSKPTH 020 W A( E ) Disk pointer (high 3 bits, high 5 bits if ECS)" (p.279)

Originally graded here as: a $020 write's low **five** bits of `data_in`
all reach `address_out[20:16]`, falsifying the claim that only the low
three reach anywhere.

**Declined, not asserted.** Independent validation found this is the same
unconditional-five-bit DSKPTH fact carried by the sibling package
`ch8-disk-pointer-ecs-and-copper`'s **P5** ("on OCS, DSKPTH holds only the
high three bits"), which drives the real `ecs`=0 port at the `agnus`
boundary and puts the OCS reading to an actual test. `agnus_diskdma` has no
`ecs` port at all, so this boundary can only observe the unconditional
width (measured and printed via `record()`); it cannot ask the OCS
question. Cited by: `opus:8-appA-dskpt.c4` (p.279), `sol:8-appa-dskpth.c4`
(p.279, twin) -- both declined via `record()`, naming
`ch8-disk-pointer-ecs-and-copper`'s P5.

### P4B -- the "(E)" conditional-difference reading -- DECLINED 2026-09-15 [C7/C14]

> "...high 5 bits if ECS" (p.279); "DSKPTH + *020 ... A (E) ..." (p.289, the
> `(E)` mark)

The `(E)` mark and the "if ECS" clause both assert that this field's width
is *conditioned* on a chipset selection. `agnus_diskdma`'s evidence-recorded
port list carries no `ecs` (or equivalent chipset-select) input at all, and
`agnus.v:326-340` wires no `ecs` signal into this module.

**Declined, not asserted.** Independent validation found that varying
`dmas`/`speed`/`hpos` (the only other real inputs this boundary has) is not
a valid proxy for chipset identity -- the page requires none of those
signals to stand in for it. This is the same fact carried by the sibling
package `ch8-disk-pointer-ecs-and-copper`'s **P4** ("the (E) mark: ECS
changes DSKPTH"), which drives the real `ecs` port (0 vs 1) at the `agnus`
boundary -- the genuine test of the conditioning -- and finds no
difference. Cited by: `opus:8-appA-dskpt.c5` (p.279), `sol:8-appa-dskpth.c5`
(p.279, twin) -- both declined via `record()`, naming
`ch8-disk-pointer-ecs-and-copper`'s P4.

### P5 -- DSKPTL loads fifteen bits, `data_in[15:1]`, into `address_out[15:1]`

> "DSKPTL 022 W A Disk pointer (low 15 bits)" (p.279)
> "DSKPTL + *022 W A Disk pointer (low 15 bits)" (p.289)

Testable form: a $022 write's `data_in[15:1]` (fifteen bits) all reach
`address_out[15:1]`.

Cited by: `opus:8-appA-dskpt.c9` (p.279), `sol:8-appa-dskptl.c4` (p.279,
twin), `opus:8-appB-dskptl.c6` (p.289), `sol:8-appb-dskptl.c8` (p.289,
twin).

Expected: **GREEN**. This is this package's required control on the same
measurement basis as the P4A/P4B measurements (declined above, but measured
by the identical mechanism: a register write, then `address_out` inspected
for which written bits arrived), same fixture, same kind of bit-count
comparison -- but here the printed bit count and the measured bit count
agree, which is what shows the comparator discriminates rather than always
reading red.

### P6 -- the pointer's lowest bit is not representable: a DSKPTL write's bit 0 changes nothing -- DECLINED 2026-09-15 (repair lot D)

**Declined, not asserted.** Independent validation found the even-address
rule is a precondition on software (what a programmer must write), not a
specified hardware masking behaviour for a misaligned write -- neither
citing page says what happens if bit 0 is set. Grading "does the RTL
discard bit 0" made the RTL its own oracle. The only fact this boundary can
measure (no storage exists for bit 0 at all) is already this package's own
green P5 control, restated under an "evenness" label. Converted to
`record()`.

> "The lowest bit of the address must be zero, and the buffer must be in
> Chip memory." (p.246, first clause only -- "Chip memory" is a `gary`-level
> fact, out of this boundary, see "Not turned into assertions")
> "+ Address register pair. Must be an even address pointing to chip
> memory." (p.287, "even address" clause only, composed with the p.289 `+`
> mark on both rows)

Testable form: `address_out` is `[20:1]` -- a word address with no bit-0
storage at all. Loading DSKPTL with bit 0 set produces the identical
`address_out` as loading it with bit 0 clear (all other bits equal).

Cited by: `opus:8-dma.c8` (p.246), `sol:8-dskpth.c4` (p.246, twin),
`sol:8-appb-legend.c5` (p.287, composed with the `+` mark),
`sol:8-appb-dskpth.c3` (p.289, composed), `sol:8-appb-dskptl.c3` (p.289,
composed). The evenness half of `opus:8-appB-dskpth.c2` and
`opus:8-appB-dskptl.c2` (p.289, composed `+` mark) is also graded here; the
pair-membership half of those same two records is graded under P7 and their
chip-memory half is out of this boundary (see "Not turned into assertions").

Expected: **GREEN** -- this is a case where NanoMig's structure matches the
manual's stated requirement, not a divergence.

### P7 -- DSKPTH and DSKPTL are one address-register pair: both halves must be written to establish a full pointer value -- DECLINED 2026-09-15 (repair lot D)

**Declined, not asserted.** Independent validation found the testable form
(a DSKPTH-alone write leaves the low field untouched) shows only that the
two registers decode and load independently -- already established by this
package's own green P1/P2 -- not that they are specifically the manual's
"+  Address register pair". Any two unrelated write-only registers feeding
independent bit ranges would pass this identical measurement. The "single
long word" claims' remaining half is the CPU-bus-width fact already
excluded below as a non-goal. Converted to `record()`.

> "You specify the 32-bit wide address..." / "The value must be written as a
> single long word to the DSKPTH register ($DFF020)." (p.246, two-register
> half only -- see "Not turned into assertions" for the CPU-access-size half)
> "DSKPTH ($DFF020) DMA pointer (32 bits)" composed with a longword CPU
> access (p.241, two-register half only)
> "+ Address register pair." (p.287, pair-membership clause, composed with
> the p.289 `+` marks on both rows)

Testable form: after DSKPTH alone is written, `address_out[15:1]` still
holds its prior (unwritten) value; only after DSKPTL is also written does
`address_out` reflect the full intended pointer. Both registers are
necessary; neither alone is sufficient.

Cited by: `opus:8-dma.c10` (p.246), `sol:8-dskpth.c6` (p.246, twin),
`opus:8-reglist.c15` (p.241), `opus:8-appB-dskpth.c2` (p.289,
pair-membership half only), `opus:8-appB-dskptl.c2` (p.289,
pair-membership half only), `sol:8-appb-dskpth.c2` (p.289,
pair-membership half only), `sol:8-appb-dskptl.c2` (p.289, pair-membership
half only).

Expected: **GREEN**.

### P8 -- the pointer is `address_out` itself: DSKPTH/DSKPTL feed it, and it advances by one word on every granted DMA cycle, holding the address of the next word to move

> "Pointer to the area into which or from which the data is to be moved"
> (p.246); "DMA pointer" (p.241); "the disk DMA channel is controlled by a
> pointer identifying the data area" (p.246, restated)
> "After each transfer occurs, the value of the pointer is incremented."
> (p.247)
> "The pointer points to the [sic] next word of data to [be] written or
> read." (p.247, `corpus_damage` flagged on both citing claims -- read as
> "the next word of data to be written or read")

Testable form: across successive granted disk-DMA cycles (`dma`=1), the
value visible at `address_out` (a) is the address about to be used for that
transfer, not the one just used, and (b) increases by exactly one word
after each granted cycle.

Cited by (increment/next-word half): `opus:8-dma.c26` (p.247),
`sol:8-dsklen.c15` (p.247, twin), `opus:8-dma.c27` (p.247),
`sol:8-dsklen.c16` (p.247, twin). Cited by (role/identity half, established
by the same measurement -- `address_out` is the port that is both loaded by
DSKPTH/DSKPTL and consumed/advanced by DMA grants): `opus:8-dsk.c33`
(p.241), `opus:8-dma.c3` (p.246), `sol:8-dskdma.c2` (p.246, twin).

Expected: **GREEN**. **NARROWED 2026-09-15 (repair lot D):** the graded
condition no longer requires exactly 3 grants at this implementation's own
hpos slots (0x19/0x1D/0x21) -- a correct implementation scheduling
differently would have failed that. It now requires only that at least one
grant occurs and that every granted cycle observed satisfies the page's own
requirement (next-word address, advance by one). Still green.

### P9 -- nothing in this module restores the pointer; it must be reloaded before each new transfer -- REPAIRED 2026-09-15 (repair lot D)

**Repaired, not declined.** Independent validation found the previous
testable form graded mere *holding* (address_out unchanged while dmal is
withdrawn), which the cited claims do not describe -- "must be reloaded
before use" is a cause-and-effect claim. The repaired testable form: after
DMA stops, DSKPTH/DSKPTL are written to a fresh, distinct pointer value and
DMA is re-enabled; the assertion now requires the FIRST subsequent granted
cycle to use exactly that reloaded value, not the address where the
previous transfer stopped. This is reachable at this boundary and remains
green.

> "Must be reloaded by a processor before use..." (p.288, processor-identity
> half not graded, see "Not turned into assertions")
> "These address registers must be initialized by the processor or Copper
> before disk DMA is enabled." (p.279, processor/Copper-identity half not
> graded)

Testable form: after DMA is granted and `address_out` has advanced away
from its loaded value, withdrawing the DMA request (`dmal`=0) does not
restore or reset `address_out` to anything -- it simply holds at wherever it
stopped. Nothing in the port list offers a restart/reload path other than a
fresh DSKPTH/DSKPTL write.

Cited by: `sol:8-appb-ptrlegend.c2` (p.288), `sol:8-appa-dskptr.c2` (p.279).

Expected: **GREEN**.

### P10 -- the pointer's direction: which of DSKDATR/DSKDAT the channel addresses tracks `wr`/`dmas` -- DECLINED 2026-09-15 (repair lot D)

**Declined, not asserted.** Independent validation found this only observes
which Paula register address `dmas` selects; agnus_diskdma's port list
carries no RAM data bus at all (only `reg_address_out`, `wr`,
`address_out`), so the actual transfer the source/destination claims
describe -- data moving to/from RAM -- is not observable here. A reversed
or entirely broken RAM transfer direction would pass this identical
measurement. Converted to `record()`.

> "You specify the 32-bit wide address from which or to which the data is to
> be transferred." (p.246)

Testable form, source direction: with `dmas`=0 and `wr`=1 (a disk-to-memory
transfer, `wr` for "write to memory"), `reg_address_out` reads $008
(DSKDATR). Testable form, destination direction: with `dmas`=1 and `wr`=0
(a memory-to-disk transfer), `reg_address_out` reads $026 (DSKDAT). Neither
half asserts that DSKDATR/DSKDAT are used *exclusively* by this channel --
see this document's opening section.

Cited by: `sol:8-dskpth.c2` (p.246, source direction), `sol:8-dskpth.c3`
(p.246, destination direction).

Expected: **GREEN**.

### P11A -- the "32-bit" width reading fails: the pointer carries 20 word-address bits, not 32

> "DSKPTH ($DFF020) DMA pointer (32 bits)" (p.241)
> "You specify the 32-bit wide address from which or to which the data is to
> be transferred." (p.246)

Testable form: `address_out` is 20 bits wide (`[20:1]`); no combination of
DSKPTH/DSKPTL writes produces a 32-bit-wide observable anywhere on this
boundary.

**Attribution, sharpened 2026-09-15 per independent stage-8 validation:**
this is the p.241/p.246 "32 bits" wording against the stored 20 word-address
bits, and only that -- it is **not** the OCS-18/ECS-20 conditioning fact
(P4A/P4B/P11B, declined above/below, carried by the sibling package
`ch8-disk-pointer-ecs-and-copper`'s P4/P5). These are two different printed
inconsistencies and must not be merged into one finding.

Cited by: `opus:8-dsk.c34` (p.241), `opus:8-dma.c7` (p.246), `sol:8-dskpth.c1`
(p.246, twin).

Expected: **RED**. (Unchanged by the 2026-09-15 repair -- this proposition's
measurement and colour are untouched; only its attribution wording above was
sharpened.)

### P11B -- the "18-bit" width reading -- DECLINED 2026-09-15

> "This pair of registers contains the 18-bit address of disk DMA data."
> (p.279)

Originally graded here as: the five bits P4A established plus the fifteen
bits P5 establishes compose to 20 bits at `address_out`, not 18.

**Declined, not asserted.** The composed total depends on the same
unconditional-five-bit DSKPTH high field P4A/P4B decline above, plus the
uncontested fifteen-bit low field already kept as this package's own green
P5 control. Read in its table context (p.289), 18 is the base/OCS width and
the ECS row explicitly extends it; the fact that decides which width
applies is graded at the sibling package `ch8-disk-pointer-ecs-and-copper`'s
P4/P5, where a real, drivable `ecs` port exists. This boundary's measured
total (`0xFFFFF`, 20 bits) is not an OCS result and must not be confused
with P11A above, which is the distinct p.241/p.246 "32 bits" wording, not
the OCS/ECS conditioning question. Cited by: `opus:8-appA-dskpt.c10`
(p.279), `sol:8-appa-dskptr.c1` (p.279, twin, "on OCS") -- both declined via
`record()`, naming `ch8-disk-pointer-ecs-and-copper`'s P4/P5.

## Fixture liveness

Before any proposition above is graded, the fixture must be shown to
deliver two *distinct* known words to two *distinct* fields and have both
arrive -- not stuck at 0, not stuck at 1, not swapped. DSKPTH is loaded with
a value whose low five bits are `10101` and DSKPTL with a value whose bits
[15:1] are a different, recognisable pattern; both are then read back
(indirectly, through `address_out`, since neither register has a read
port -- P3) and confirmed to have landed in their own, and only their own,
field.

## Control case

P5 (DSKPTL's correct fifteen-bit load) is this package's control on the
same measurement basis as the P4A/P4B measurements (declined 2026-09-15,
see above; still measured by the same mechanism -- a register write,
followed by inspecting which bits of `data_in` reached `address_out`): it
uses the identical comparator shape and passes, showing the harness is not
simply wired to always report red.

## Not turned into assertions

- **The CPU-access-size half of "a single long word"** (`opus:8-dma.c10`,
  `sol:8-dskpth.c6`, `opus:8-reglist.c15`, page 246/241) -- that the 68000/
  Copper issues one 32-bit-wide bus cycle rather than two 16-bit ones. Per
  these claims' own `split` notes, this is a fact about the CPU instruction
  encoding, not observable at `reg_address_in`/`data_in`, where a longword
  write arrives as two ordinary 16-bit register writes regardless of which
  instruction produced them. Not a runtime behaviour of this module; no
  stimulus/observation pair grades it. Recorded, not asserted. (The
  two-register-split half of the same claims -- that both registers must be
  written -- was graded as P7 until the 2026-09-15 repair lot D; P7 is now
  also declined -- see its own section above. Both halves of these three
  claims are declined in this package now.)
- **The "by a processor" / "by the processor or Copper" requester-identity
  half of `sol:8-appb-ptrlegend.c2` (p.288) and `sol:8-appa-dskptr.c2`
  (p.279)** -- `reg_address_in`/`data_in` carry an address and a value, and
  no requester identity. The module cannot distinguish a CPU write from a
  Copper write to the same address; nothing at this boundary discriminates
  "who" wrote it. Recorded, not asserted. (The "must be reloaded before
  use" / "must be initialized before DMA is enabled" half of the same two
  claims *is* graded, as P9, via an explicit reload-and-use measurement
  since the 2026-09-15 repair lot D.)
- **The "pointing to chip memory" half of the `+` mark**
  (`opus:8-appB-dskpth.c2`, `opus:8-appB-dskptl.c2`, p.289, composed with
  the p.287 legend) -- whether the loaded address actually lands in chip
  RAM is a fact about the memory map, observable at `gary`, not at
  `agnus_diskdma`. A wider module boundary than the one assigned to this
  package. Per the claims' own `split` notes this half is graded elsewhere
  (`sol:8-appb-legend.c6`, `sol:8-appb-dskpth.c4`), outside this package's
  55 claims. Recorded, not asserted here.

## Claims accounted for

All 55 claims in `ch8-disk-dma-pointer.jsonl` are accounted for: P1 (7),
P2 (4), P3 (8), P4A (2), P4B (2), P5 (4), P6 (5 direct + 2 partial halves of
P7's pair claims), P7 (7, 2 of which contribute only their pair-membership
half to this count and their evenness half to P6), P8 (7), P9 (2), P10 (2),
P11A (3), P11B (2). The CPU-access-size, requester-identity and
chip-memory halves noted above belong to claims already fully cited under
P6/P7/P9 for their other, graded half.

**2026-09-15 repair:** P4A's 2, P4B's 2 and P11B's 2 claims (6 of the 55)
are now **declined**, not graded -- see their sections above. They remain
accounted for, via `record()` naming the sibling package
`ch8-disk-pointer-ecs-and-copper`'s P4/P5, which carries the fact. 49 of 55
claims remain graded through `expect()`.

**2026-09-15 repair lot D (LOS-D.md):** P3's 8, P6's 5-plus-2-partial, P7's
7 (2 partial) and P10's 2 claims are now also **declined** -- see their
sections above, each with a `record()` explaining why the claim cannot be
graded at this boundary. P8 and P9 remain **graded**, narrowed to what the
pages actually require rather than the implementation's own scheduling
detail (P8) or mere holding (P9). This repair also cleared this package's
`tools/check_uid_placement.py` offenses: 7 uids previously sat on both a
graded line and a `record()` line (P7's and P9's own citations); declining
P7 and stripping P9's label of its literal uid tokens (kept only on the
`record()` lines that already name P7/P9 as the assertion carrying the
graded half) resolved all 7. Remaining graded through `expect()`: LIVENESS,
P1, P2, P5, P8, P9, P11A.

## Mutation plan

Every graded proposition gets one stimulus mutation (for a green baseline:
change what the module is asked to do, expected value untouched, confirm
red, revert) or one oracle-substitution mutation (for a red baseline: since
an already-red check cannot be shown to fail harder, substitute the
compared quantity for one a live mechanism elsewhere in this same run
proves correct, through the same comparator, and show green), per
BUILD-BRIEF. P3's is `none`/`impossible`: the write-only property follows
from a fixed port list with no data-output signal at all, so no reachable
stimulus can ever manufacture a read-back path to falsify against.

- LIVENESS: stimulus -- feed all-zero words instead of the two distinct
  patterns; expect red (both fields read back as zero, indistinguishable
  from a dead/stuck data path). Revert.
- P1: stimulus -- present the value at $022 instead of $020; expect red
  (bits do not land in `address_out[20:16]`). Revert.
- P2: stimulus -- present the value at $020 instead of $022; expect red.
  Revert.
- P3: **removed 2026-09-15 (repair lot D).** Declined via `record()`; the
  original `none`/`impossible` reasoning (no data-output port exists to
  read back through, at any stimulus) is unchanged, but it no longer backs
  an `expect()`, so no mutation record remains.
- P4A, P4B: **removed 2026-09-15 (repair).** Both are declined via
  `record()`, not graded through `expect()`; per BUILD-BRIEF a mutation
  record is removed with its assertion, so neither carries one any more.
- P5: stimulus -- corrupt one bit of the DSKPTL value fed in; expect red.
  Revert.
- P6: **removed 2026-09-15 (repair lot D).** Declined via `record()`; the
  measurement (DSKPTL<=0x2468 vs 0x2469) is now a `note()` only, no
  mutation record remains.
- P7: **removed 2026-09-15 (repair lot D).** Declined via `record()`; the
  measurement (writing DSKPTH alone) is now a `note()` only, no mutation
  record remains.
- P8: stimulus -- withhold `dmal` so no cycle is ever granted; expect red
  (grant_count stays 0, failing `grant_count>=1`). **NARROWED 2026-09-15
  (repair lot D):** the graded condition changed from `grant_count==3` to
  `grant_count>=1`, but this same mutation (0 grants) still fails it.
  Revert.
- P9: **REPAIRED 2026-09-15 (repair lot D)** -- stimulus retargeted to the
  new reload-and-use mechanism: skip the post-stop DSKPTH/DSKPTL reload
  writes instead of performing them, so the pointer is never actually
  reloaded before DMA restarts; expect red (the first subsequent grant's
  address_out equals `stopped`, not `reloaded_ptr`). Revert.
- P10: **removed 2026-09-15 (repair lot D).** Declined via `record()`; the
  measurement (dmas=0/1 vs reg_address_out) is now a `note()` only, no
  mutation record remains.
- P11A: oracle -- substitute the compared width from 32 to 20; expect
  green.
- P11B: **removed 2026-09-15 (repair).** Declined via `record()`, not
  graded through `expect()`; no mutation record remains with its assertion.

All mutation results, with the baseline colour beside each, are recorded in
`test_ch8_disk_dma_pointer.cpp`'s header comment and in the build report,
per BUILD-BRIEF ("in the C++ source, not in a separate file").
