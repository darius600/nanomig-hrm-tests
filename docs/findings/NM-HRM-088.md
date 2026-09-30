# NM-HRM-088 — BPLxPTH carries five high bits on OCS, where pp. 272 and 291 give it three [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L220) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L220) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, and **only together with [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md), [NM-HRM-024](NM-HRM-024.md) and [NM-HRM-043](NM-HRM-043.md)** — see *Suggested change* |
| **Test** | [`hrm/ch3-bitplane-pointers-and-modulo`](../../hrm/ch3-bitplane-pointers-and-modulo) · `BPLXPTH-OCS-HIGH-THREE-BITS`, claims `A-bplptr.c3` and `A-bplxpth.fn` (p. 272) and `B-subject.c29` (p. 291), with its control `BPLXPTH-OCS-HIGH-THREE-BITS-CONTROL` |
| **Chapter** | 3 — Playfield; Appendix A, printed p. 272; Appendix B, printed p. 291; 1 — Introduction, printed p. 5 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red, `BPLXPTH-OCS-HIGH-THREE-BITS`: at `ecs` = 0, BPL1PTH = $001F and BPL1PTL = $0000 give a first fetch at 0x1f0000, where the pages give 0x070000; one of the package's `19 passed, 1 failed`. Its control `BPLXPTH-OCS-HIGH-THREE-BITS-CONTROL`, BPL1PTH = $0007 through the same fixture and comparator, is green at 0x070000. The `ecs` = 1 twin, `BPLXPTH-ECS-HIGH-BITS-TWIN`, is a note, not graded |
| **Verdict of the four** | **three of the four narrow by chipset**: Copperline masks the stored bitplane pointer by Agnus revision and names it in a test; WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Agnus.cpp:190-194`) mask the address the bitplane fetch drives. Denise does not — it masks to the installed chip RAM, not by Agnus revision |
| **See also** | [NM-HRM-003](NM-HRM-003.md) (Copper), [NM-HRM-022](NM-HRM-022.md) (blitter), [NM-HRM-024](NM-HRM-024.md) (audio) and [NM-HRM-043](NM-HRM-043.md) (disk) — **this is the fifth sighting of the same cause.** And [NM-HRM-014](NM-HRM-014.md), on the same register under ECS, filed against the printed page |

---

## This is one cause in five engines, not five findings

[NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md), [NM-HRM-024](NM-HRM-024.md) and
[NM-HRM-043](NM-HRM-043.md) establish that the Copper, blitter, audio and disk pointers each
latch five high bits with no chipset condition. **Their reasoning is not repeated
here.** [NM-HRM-043](NM-HRM-043.md) lists six pointer sites in this Agnus, all loaded from
`data_in[4:0]` with no `ecs` term; the bitplane site is one of them. This entry is
that site, measured and red.

It differs from its four siblings in one respect: **it has only an OCS half.** The
Copper, blitter, audio and disk registers are printed with two widths, three bits
and five if ECS. The bitplane pointer is printed with one, three, and no register
table gives it a second width for the ECS. So this finding does not rest on the two
chipsets differing; it rests on the OCS width alone, which every page that speaks
to it prints the same way. Whether the ECS bitplane pointer should reach further,
as chapter 1's larger ECS Chip memory needs, is [NM-HRM-014](NM-HRM-014.md)'s question,
and this finding needs no answer to it.

## What the HRM says

**Appendix A, printed p. 272**, the register and its pair:

> `BPLxPTH    0E0      W       A     Bitplane x pointer (high 3 bits)`

> "This pair of registers contains the 18-bit pointer to the address of bitplane x
> (x=1,2,3,4,5,6) DMA data."

**Appendix B, printed p. 291**, the first of six such rows:

> `BPL1PTH    +   0E0    W   A           Bitplane 1 pointer    (high 3 bits)`

Neither row carries the mark **Appendix B, printed p. 288**, defines for registers
the ECS changed:

> "Registers denoted with an "(E)" in the chip column means that those registers
> have been changed in the Enhanced Chip Set (ECS)."

On the printed p. 291 the chip column of the six BPLxPTH rows reads `A` alone,
while `BPLCON0` a few rows below reads `A D(E)`.

**Chapter 3, printed p. 54**, counts the same pointer in bytes:

> "Pairs of registers with names ending in PTH and PTL contain 19-bit addresses."

Eighteen word-address bits and nineteen byte-address bits are the same pointer
with and without bit 0; either way the high word holds three bits.

**Chapter 1, printed p. 5**, gives the reach of the OCS Agnus:

> "The original custom chip set found in the A1000 could access the first 512K of
> RAM."

> "The Fat Agnus chip has DMA access to 512K of Chip memory, just like the original
> Agnus"

Three high bits above fifteen low word-address bits address exactly 512K. On an
OCS Agnus, BPL1PTH = $001F with BPL1PTL = $0000 names 0x070000: bits 3 and 4 of
the write have no address line to reach.

**Appendix C, printed p. 309**, lists the pointers the ECS widens:

> "The following registers now have two additional bits for addressing larger
> segments of memory, when the Enhanced Chip Set is present:"

and names DSKPTH, BLTxPTH, COP1LCH, COP2LCH and AUDxLCH, the four siblings'
registers, each "(high 5 bits, was 3 bits)". BPLxPTH is not among them. That
silence is [NM-HRM-014](NM-HRM-014.md)'s subject; at OCS nothing is silent.

### What the manual tells software about the bits above the field

**Appendix A, printed p. 264**:

> "If a bit is described as unused in a write-only register, be sure to keep that
> bit clear when writing values to that register."

> "Setting unused bits in a write-only register, reading unused bits from a read
> only register and writing to undocumented registers or addresses may cause
> serious future software incompatibility if those bits or addresses are
> implemented in the future"

A rule for programs, not a description of the chip. Its reason, that such bits
may be implemented in the future, presupposes that the chip described does not
implement them. No page says what an OCS Agnus does with bits 3 and 4 of BPLxPTH
other than that its DMA reaches 512K.

## What NanoMig does

[`src/minimig-aga/agnus_bitplanedma.v#L82`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L82),
[`#L220`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L220),
[`#L230`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L230)

```verilog
reg  [20:16] bplpth [7:0];        // upper 5 bits bitplane pointers
assign bplpth_in = dma ? newpt[20:16] : data_in[4:0];
assign address_out[20:16] = bplpth[plane[2:0]];
```

Five high bits per plane, loaded from the write with no `ecs` term and driven
straight onto the address.

**The module has the chipset input, and uses it elsewhere.** Its port list declares
[`agnus_bitplanedma.v:35`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L35) `input  wire           ecs,`
and
[`agnus.v:362-381`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L362-L381) connects it, `.ecs(ecs),`.
The module reads it for DIWHIGH, for the DDFSTRT and DDFSTOP H2 bit, for
SuperHires and for the OCS fetch start and stop; the pointer bank is the part that
never sees it. Unlike the disk and audio engines of [NM-HRM-043](NM-HRM-043.md) and
[NM-HRM-024](NM-HRM-024.md), there is something here to gate on.

**It holds at the Agnus pins.** The bitplane slot of
[`agnus.v:214`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L214) drives `address_out = address_bpl;`
with no chipset term, and
[`gary.v:131`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L131) passes all twenty bits to memory, `assign ram_address_out  = dbr ? {3'b000, dma_address_in[20:1]}`.

### Measured

In [`hrm/ch3-bitplane-pointers-and-modulo`](../../hrm/ch3-bitplane-pointers-and-modulo),
at the `agnus_bitplanedma` boundary, where `ecs` is a port the fixture drives: one
bitplane, the standard low-resolution fetch window, the pointer written before line
$50, and the first address plane 1 fetches on that line.

| BPL1PTH, BPL1PTL | `ecs` | first fetch | the pages | |
|---|---|---|---|---|
| $0007, $0000 | 0 | 0x070000 | 0x070000 | green, `BPLXPTH-OCS-HIGH-THREE-BITS-CONTROL` |
| $001F, $0000 | 0 | **0x1f0000** | 0x070000 | **red**, `BPLXPTH-OCS-HIGH-THREE-BITS` |
| $001F, $0000 | 1 | 0x1f0000 | disputed, [NM-HRM-014](NM-HRM-014.md) | recorded, `BPLXPTH-ECS-HIGH-BITS-TWIN` |

Bits 3 and 4 of the write land on address bits 19 and 20, and the two chipset
settings read alike. The package's run ends `19 passed, 1 failed`; the one
failure is `BPLXPTH-OCS-HIGH-THREE-BITS`.

The control is `BPLXPTH-OCS-HIGH-THREE-BITS-CONTROL` (`test_agnus_bitplanedma.cpp:1253-1282`),
the red is `BPLXPTH-OCS-HIGH-THREE-BITS` (`:1284-1315`),
and the recorded twin is `BPLXPTH-ECS-HIGH-BITS-TWIN` (`:1317-1362`).

**The red is the width and nothing else.** With the one line above changed so that
the high field keeps three bits at `ecs` = 0,

```verilog
assign bplpth_in = (dma ? newpt[20:16] : data_in[4:0]) & {ecs, ecs, 3'b111};
```

the package reads `20 passed, 0 failed`: the red reads 0x070000 and passes, the
control and the other eighteen assertions stay green, and the `ecs` = 1 twin still
reads 0x1f0000.

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **on the bus, not in the register.** `BPLxPTH` keeps the whole high word (`bplpt[num] = (bplpt[num] & 0x0000ffff) \| ((uae_u32)v << 16);`, [`custom.cpp:3497`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3497)). Outside AGA the bitplane slot reads with `fetch16(r)` ([`custom.cpp:11777`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11777)), which is `v = chipmem_wget_indirect(p);` ([`custom.cpp:8822`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L8822)); in the default configuration that is `chipmem_agnus_wget` ([`memory.cpp:998`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L998)), which begins `addr &= chipmem_full_mask;` ([`memory.cpp:812`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L812)), and the mask is `0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear. Three caveats: `cs_agnussize` overrides the mask either way ([`memory.cpp:2627-2644`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2627-L2644)); with JIT on, `chipmem_full_mask = chipmem_bank.mask = chipmem_bank.allocated_size - 1;` ([`memory.cpp:2646-2648`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2646-L2648)) drops the chipset term; and, for bitplanes alone, with the display optimisation set to full a line whose parameters repeat the previous field's is drawn from the pointer masked with `pt &= chipmem_bank.mask;`, the allocated chip RAM ([`custom.cpp:10333-10336`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10333-L10336)) |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:1621-1623`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1621-L1623)<br>[`Core/Components/Agnus/Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194) | **on the bus, not in the register**, by Agnus revision. `setBPLxPTH` keeps the whole high word (`bplpt[x - 1] = REPLACE_HI_WORD(bplpt[x - 1], value);`, [`AgnusRegs.cpp:776`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L776)), and `ptrMask` there only feeds an `xfiles` log line ([`AgnusRegs.cpp:778-780`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L778-L780)). The bitplane fetch is `mem.peek16<Accessor::AGNUS>(bplpt[bitplane])` ([`AgnusDma.cpp:126`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L126)), that accessor begins `addr &= agnus.ptrMask;`, and the mask is set per revision, `ptrMask = 0x07FFFF` for an OCS Agnus |
| **Denise** | [`emulation/libami/agnus/dma.cpp:19-23`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L19-L23)<br>[`emulation/libami/agnus/memory.cpp:693-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L693-L698) | masks the bitplane pointer at the write, `bpl1pt &= dmaChipMemMask;`, and again at the fetch, `addrBus = bpl1pt & dmaChipMemMask;` ([`dma.cpp:109`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L109)), but with the installed chip-RAM mask, `dmaChipMemMask = chipMemMask;`, which the chip RAM option sets up to 2 MB whatever the Agnus ([`memory.cpp:614-627`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L614-L627)). Its one `ecs()` term widens the mask, for the Slow-RAM mirror. No chipset narrowing: this write fetches from 0x070000 with 512K fitted and from 0x1f0000 with 2 MB |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118)<br>[`src/chipset/denise.rs:445-449`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/denise.rs#L445-L449)<br>[`src/bus.rs:4991-4999`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4991-L4999) | **narrows the register by Agnus revision.** Its own Denise module stores the high word as `((cur & 0x0000_FFFF) \| ((val as u32 & 0x001F) << 16)) & self.dma_ptr_mask()`, and `configure_chip_dma_masks` sets that mask to `chip_dma_addr_mask(self.mem.chip_ram.len()) & self.agnus.revision().dma_addr_capability_mask();`, with `Self::Ocs => 0x0007_FFFF,`. Its test `dma_pointer_high_bits_follow_agnus_revision` ([`src/bus/tests.rs:2785-2840`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L2785-L2840)) fits `empty_bus_with_chip_ram(2 * 1024 * 1024)`, writes $001F and $FFFE into each `BPL{plane}PT`, and expects `(AgnusRevision::Ocs, 0x0007_FFFEu32),` on OCS: this package's stimulus, with the manual's answer |
| **MiSTer** | [`rtl/agnus_bitplanedma.v:82`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L82), [`:220`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L220), [`:230`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L230) | descendant — `agnus_bitplanedma.v` is identical to NanoMig's once line endings are normalised: `reg  [20:16] bplpth [7:0];`, `assign bplpth_in = dma ? newpt[20:16] : data_in[4:0];` and `assign address_out[20:16] = bplpth[plane[2:0]];`, no `ecs` term. [`rtl/agnus.v:325-343`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L325-L343) connects `.ecs(ecs)` to the module, and [`rtl/agnus.v:177`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L177) drives `address_out = address_bpl;` unchanged |
| **TC64** | [`rtl/minimig/agnus_bitplanedma.v:82`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L82), [`:194`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L194), [`:204`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L204) | descendant — the same three statements, byte for byte, `assign bplpth_in = dma ? newpt[20:16] : data_in[4:0];` among them, no `ecs` term; the file differs from NanoMig's in the display-window registers and an exported `ddf`, not in the pointers. [`rtl/minimig/agnus.v:364-382`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L364-L382) connects `.ecs(ecs)`, and [`rtl/minimig/agnus.v:232`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L232) drives `address_out = address_bpl;` unchanged. A fix at this site diverges from neither line |

**Three of the four independent implementations narrow the OCS bitplane fetch to
512K; only Denise does not.** Copperline narrows the register, with the same mask it
applies to every other DMA pointer, and names the bitplane pointer in the test that
walks them all; WinUAE and vAmiga narrow the address every Agnus fetch drives, which
has the same effect on the fetched address. Denise masks to the installed chip RAM.
MiSTer and TC64 carry NanoMig's five bits, inherited rather than independently
arrived at.

## Suggested change

**The same one as for the four siblings, and it is one change, not five.**
[NM-HRM-003](NM-HRM-003.md) and [NM-HRM-022](NM-HRM-022.md) set out the two coherent actions, and
[NM-HRM-043](NM-HRM-043.md) counts the six pointer sites they cover; this site is one of them:

1. **Model it once, centrally**, the way Copperline does: a mask derived from the
   Agnus revision and applied to every assembled DMA pointer. At this site the
   module already has `ecs` in scope, so nothing needs routing; the one-line change
   under *Measured* is what the mask amounts to here, and it moves this red and
   nothing else in the package.
2. **Declare the core ECS-and-later**, making the manual's OCS pointer sentences out
   of scope, and record all five findings as one scope decision.

**Five separate patches in five engines is the option to avoid.** Either action
leaves what NanoMig does at `ecs` = 1 as it is, so [NM-HRM-014](NM-HRM-014.md)'s question
is neither settled nor changed by it.

## What holds the finding up

- **One width, printed without dissent.** Appendices A and B give BPLxPTH three
  high bits with no `(E)` mark, p. 272 calls the pair an 18-bit pointer, p. 54 a
  19-bit address, and p. 5 gives the OCS Agnus 512K. No page gives the OCS
  bitplane pointer more.
- **Measured at the module that has the port.** `agnus_bitplanedma` takes `ecs`
  as an input and the fixture drives it, so the test asks the question where the
  chipset input reaches, the rule that moved [NM-HRM-022](NM-HRM-022.md)'s measurement to
  `agnus_blitter`.
- **A green control on the identical fixture.** BPL1PTH = $0007 reads 0x070000
  through the same comparator; the red is the two bits above the field, not a
  fixture that cannot see an address.
- **A three-bit field passes it and moves nothing else**, the `ecs` = 1 reading
  included (*Measured*).

## What weakens it

Everything that weakens [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md),
[NM-HRM-024](NM-HRM-024.md) and [NM-HRM-043](NM-HRM-043.md) weakens this one identically.

- **One of the four fetches as NanoMig does.** Denise drives the stored pointer
  with no chipset term; with more than 512K of chip RAM fitted it fetches past
  512K on OCS. That WinUAE and vAmiga keep all the bits in the register weakens
  nothing: BPLxPTH is write-only, so the stored value shows only in the address
  the DMA drives.
- **The stimulus sets bits p. 264 tells software to keep clear.** The rule is
  about programs and gives its reason as bits that may be implemented in the
  future; it does not describe the OCS chip as using them, and p. 5 bounds every
  OCS DMA address to 512K whatever was written. Writing bits above the field is
  the only way a test can see the width of a write-only register.

## How this differs from NM-HRM-014

[NM-HRM-014](NM-HRM-014.md) is filed against the printed page, about the same register
under ECS. There the pages disagree: Appendix C's list of pointers the ECS widens
leaves BPLxPTH out, and Appendices A and B print "(high 3 bits)" without the `(E)`
mark, while chapter 1 gives the ECS Agnus one or two megabytes and puts bitplanes
anywhere in Chip memory. No assertion can grade that without choosing a page, and
no RTL change settles it; it asks for none.

This finding is the OCS half, where the pages agree. An assertion grades it, and
it is red. The package keeps the two apart the same way: `BPLXPTH-OCS-HIGH-THREE-BITS`
grades `ecs` = 0, and `BPLXPTH-ECS-HIGH-BITS-TWIN`, the identical write at `ecs` = 1,
is recorded without a verdict because the page it would be graded against is
[NM-HRM-014](NM-HRM-014.md)'s open question. NanoMig's five bits under ECS are the reach
chapter 1 needs; its five bits at OCS are this finding.

## For the specialists

There is nothing here that the first four sightings do not already put before you.
What the fifth adds is that the bitplane engine, which has the chipset input and
uses it beside the pointer bank for its display window and fetch timing, still
takes the pointer's high bits without it. [NM-HRM-043](NM-HRM-043.md)'s sixth site, the sprite
pointer, is built the same way and has no graded test; [NM-HRM-014](NM-HRM-014.md) records that
its OCS width is less settled on the page, p. 127 calling the pair a "32-bit word
address".

The decision is the one [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md),
[NM-HRM-024](NM-HRM-024.md) and [NM-HRM-043](NM-HRM-043.md) already ask for: whether this core models an
OCS Agnus at all. Take it once, for all five.
