# NM-HRM-014 — the manual applies its own ECS pointer extension inconsistently

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | none |
| **What is asked of a reader** | No RTL change follows from this. Appendix C p. 309 lists the pointer registers that gain two high bits under ECS and leaves out BPLxPTH and SPRxPTH, and Appendices A and B print both "(high 3 bits)" without the `(E)` mark; chapter 1 gives the ECS Agnus one or two megabytes of Chip memory and puts bitplanes and sprite lists anywhere in it. NanoMig's five high bits follow chapter 1, and all four implementations give these two pointers the reach they give every other DMA pointer. The question is whether Appendix C's list is complete. At OCS every page gives the bitplane pointer three high bits; NanoMig's five there are the construction [NM-HRM-043](NM-HRM-043.md) records, and no assertion grades them. |
| **Module** | — |
| **Report to** | **nobody.** There is no repository that can fix a disagreement between printed pages |
| **Test** | [`hrm/ch3-register-map-agnus`](../../hrm/ch3-register-map-agnus) — recorded as a `note()`, not an assertion |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | none. Two `note()`s record the width, `RTL-VS-MANUAL-BPLXPTH-FIELD-WIDTH` and `POINTER-WIDTH-DISPUTED`; no assertion grades it |
| **Verdict of the four** | **unanimous with NanoMig and with chapter 1**: WinUAE, vAmiga, Denise and Copperline give the bitplane and sprite pointers the reach they give every other DMA pointer, past 512K under ECS, and none holds them to three bits there; Copperline asserts it for `BPLxPT` and `SPRxPT` by name in a test. At OCS three of the four narrow all of them to 512K, as [NM-HRM-043](NM-HRM-043.md) records for the disk pointer |

---

## What this is

A disagreement **inside the manual**, about how widely its own ECS pointer
extension applies. Appendix C names the pointer registers that gain two high
bits under ECS, and the bitplane and sprite pointers are not among them;
chapter 1 gives the ECS Agnus up to two megabytes of Chip memory and lets
bitplanes and sprite lists sit anywhere in it.

It is recorded in the package as `checks.note(...)`, whose own text says
`Not folded into any PASS/FAIL above`.

---

## What the HRM says

**Appendix C, printed p. 309**, names the pointers the ECS extends:

> "The following registers now have two additional bits for addressing larger
> segments of memory, when the Enhanced Chip Set is present:"

and lists DSKPTH, BLTxPTH, COP1LCH, COP2LCH and AUDxLCH, each "(high 5 bits,
was 3 bits)". The ECS register map on printed p. 298 marks the same five
changed. Neither page lists BPLxPTH or SPRxPTH.

**Appendices A and B** apply the extension the same way, and less evenly. In
Appendix A the five extended registers carry the `(E)` mark and "5 bits if ECS"
(AUDxLCH p. 266, BLTxPTH p. 270, COP1LCH and COP2LCH p. 273, DSKPTH p. 279);
the bitplane and sprite pointers carry neither:

> `BPLxPTH    0E0      W       A     Bitplane x pointer (high 3 bits)`

> `SPRxPTH    120      W       A     Sprite x pointer (high 3 bits)`

(printed pp. 272 and 285). Appendix B gives "5 bits if ECS" to DSKPTH, COP1LCH,
COP2LCH and AUD0LCH, the `(E)` mark alone to BLTAPTH, and neither to the other
three blitter pointers, the other three audio pointers, or any of the fourteen
bitplane and sprite pointer rows on printed p. 291:

> `BPL1PTH    +   0E0    W   A           Bitplane 1 pointer    (high 3 bits)`

**Chapter 1, printed p. 5**, says how far the ECS Agnus reaches:

> "ECS Fat Agnus (8372A) can access up to one megabyte of Chip memory."

> "In the A3000, the Enhanced Chip Set can access up to two megabytes of Chip
> memory."

and **printed p. 6** where the DMA data may be:

> "the system allows display bitplanes, sprite processor control lists,
> coprocessor instruction lists, or audio channel control lists to be located
> anywhere within Chip memory."

**So the manual says two things about the ECS bitplane and sprite pointers.**
Chapter 1 needs them to reach past 512K; Appendices A, B and C keep them at
three high bits, the reach of an OCS Agnus.

**At OCS the pages agree on the bitplane pointer.** p. 5 gives the Fat Agnus
*"DMA access to 512K of Chip memory"*. p. 54 says the pointer pairs *"contain
19-bit addresses"* and p. 272 calls the bitplane pair *"the 18-bit pointer"*;
the two count the same pointer with and without bit 0, as p. 25 does twice in
one paragraph for the Copper's pair:

> "COP1LCL Low 16 bits of first Copper list address."

> "low 15 bits of the address. Therefore, you write the 18-bit address"

Either count leaves the high word three bits. The sprite pointer has one more
reading: beside p. 110's "high three bits" and p. 126's "18-bit sprite pointer",
printed p. 127 says

> "This pair of registers contains the 32-bit word address of Sprite 0 DMA
> data."

---

## What NanoMig does

[`src/minimig-aga/agnus_bitplanedma.v#L82`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L82),
[`#L220`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L220)

```verilog
reg  [20:16] bplpth [7:0];        // upper 5 bits bitplane pointers
assign bplpth_in = dma ? newpt[20:16] : data_in[4:0];
```

Five high bits, loaded from `data_in[4:0]` with no `ecs` term. The sprite pointer
is built the same way,
[`agnus_spritedma.v#L111`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_spritedma.v#L111)
and
[`#L179`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_spritedma.v#L179):
`reg   [20:16] sprpth [7:0];`, `assign sprpth_in = ackdma ? newptr[20:16] : data_in[4:0];`.

Under ECS that is the reach chapter 1 needs. At OCS it is two bits more than
every page gives the bitplane pointer, the construction [NM-HRM-043](NM-HRM-043.md) measures on the
disk pointer and lists, from the source, for these two pointers as well.

### Measured

No assertion grades the width of either pointer. What the suite does record:

- [`hrm/ch3-register-map-agnus`](../../hrm/ch3-register-map-agnus) records it as
  the `RTL-VS-MANUAL-BPLXPTH-FIELD-WIDTH` `note()`, whose own text reads "Not
  folded into any PASS/FAIL above" — the neighbouring twelve pointer-address
  claims it sits beside are graded, and that is what this package's address
  assertions carry instead. [`hrm/ch3-bitplane-pointers-and-modulo`](../../hrm/ch3-bitplane-pointers-and-modulo)
  records the same width as its `POINTER-WIDTH-DISPUTED` `note()`.
- [`hrm/ch4-sprite-pointer-registers`](../../hrm/ch4-sprite-pointer-registers)'
  `MEASURED-WIDTH`, at `ecs` = 0, writes `$FFFF` to SPR0PTH and SPR0PTL and reads
  the fetch address back as `0xfffff`: all twenty bits of `address_out[20:1]`,
  five of them high. Recorded, not graded.
- `POINTER-PAIR-IS-ONE-ADDRESS` in `hrm/ch3-bitplane-pointers-and-modulo`,
  graded and green at `ecs` = 1, writes `0x00123456` to BPL1PTH/BPL1PTL and
  fetches from exactly that address, bit 20 included. It is graded against
  p. 54's "one 32-bit address", not against a width, but a bitplane pointer held
  to three high bits under ECS would fail it.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **on the bus, as for every other pointer.** `BPLxPTH` keeps the whole high word (`bplpt[num] = (bplpt[num] & 0x0000ffff) \| ((uae_u32)v << 16);`, [`custom.cpp:3497`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3497)). The sprite and bitplane slots fetch with `fetch16(r)` ([`custom.cpp:11705`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11705), [`:11777`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11777)), which outside AGA reads `v = chipmem_wget_indirect(p);` ([`:8822`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L8822)); in the default configuration that is `chipmem_agnus_wget` ([`memory.cpp:998`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L998)), which begins `addr &= chipmem_full_mask;` ([`memory.cpp:812`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L812)). The mask is `0x80000 - 1` without `CSMASK_ECS_AGNUS` and, unless its Agnus-size option narrows it, at least `0x100000 - 1` with it |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194) | **on the bus, as for every other pointer.** `setBPLxPTH` and `setSPRxPTH` keep the whole high word (`bplpt[x - 1] = REPLACE_HI_WORD(bplpt[x - 1], value);`, [`AgnusRegs.cpp:776`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L776), and `sprpt[x] = REPLACE_HI_WORD(sprpt[x], value);`, [`:824`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L824)). The fetches read `mem.peek16<Accessor::AGNUS>(bplpt[bitplane])` and `mem.peek16<Accessor::AGNUS>(sprpt[channel])` ([`AgnusDma.cpp:126`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L126), [`:194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L194)), and that accessor begins `addr &= agnus.ptrMask;` ([`Memory.cpp:1623`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1623)), with `ptrMask = 0x07FFFF` for OCS, `0x0FFFFF` for `ECS_1MB` and `0x1FFFFF` for `ECS_2MB` |
| **Denise** | [`emulation/libami/agnus/dma.cpp:19-23`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L19-L23), [`:85-89`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L85-L89) | masks the bitplane and sprite pointers at the write, `bpl1pt &= dmaChipMemMask;` and `sprites[num].ptr &= dmaChipMemMask;`, with the installed chip-RAM mask ([`memory.cpp:693-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L693-L698)), whose one `ecs()` term widens it. No chipset narrowing, as for the other pointers |
| **Copperline** | [`src/chipset/agnus.rs:109-119`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L109-L119) | **narrows the register by Agnus revision**, with the central `dma_addr_capability_mask()` every other pointer uses (`Self::Ocs => 0x0007_FFFF,`, `Self::Ecs8372Rev4 => 0x000F_FFFF,`, `Self::Ecs8375 \| Self::AgaAlice => 0x001F_FFFF,`), applied in `set_bplpt_high` and `set_sprpt_high` ([`src/chipset/denise.rs:445-449`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/denise.rs#L445-L449), [`:455-459`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/denise.rs#L455-L459)) through `configure_chip_dma_masks` ([`src/bus.rs:4991-4999`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4991-L4999)). Its test `dma_pointer_high_bits_follow_agnus_revision` ([`src/bus/tests.rs:2786-2850`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L2786-L2850)) writes `$001F`/`$FFFE` into DSKPT, BLTxPT, COPxLC, AUDxLC, `BPL{plane}PT` and `SPR{sprite}PT` at each of three revisions and expects the same mask from all of them |
| **MiSTer** | [`rtl/agnus_bitplanedma.v:82`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L82), [`:220`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L220) | descendant — `agnus_bitplanedma.v` and `agnus_spritedma.v` are identical to NanoMig's once line endings are normalised: `reg  [20:16] bplpth [7:0];`, loaded from `data_in[4:0]` with no `ecs` term, and `sprpth` the same at [`rtl/agnus_spritedma.v:111`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_spritedma.v#L111) and `:179` |
| **TC64** | [`rtl/minimig/agnus_bitplanedma.v:82`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L82), [`:194`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L194) | descendant — the same five-bit `bplpth`, loaded with `assign bplpth_in = dma ? newpt[20:16] : data_in[4:0];` and no `ecs` term, and `sprpth` the same at [`rtl/minimig/agnus_spritedma.v:111`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_spritedma.v#L111) and `:179`; both files differ from NanoMig's elsewhere, not in the pointers |

**All four give the bitplane and sprite pointers the reach they give every other
DMA pointer.** None reads Appendix C's list as leaving these two at three bits
under ECS, and Copperline's test names them beside the five the list does name.
At OCS, three of the four narrow all of them to 512K, WinUAE and vAmiga on the
bus and Copperline in the register; Denise masks to the installed chip RAM.

---

## What a test can grade, and what it cannot

**The ECS half cannot be graded against one page.** There the disagreement is
between printed pages: an assertion would have to pick Appendix C's list or
chapter 1 as the oracle. The suite already holds chapter 1's side green in
passing, since `POINTER-PAIR-IS-ONE-ADDRESS` fetches bit 20 at `ecs` = 1, so an
assertion of three high bits at `ecs` = 1 would contradict it, and no
implementation could make both green.

**The OCS half can, for the bitplane pointer, and nothing grades it.** At
`ecs` = 0 every page gives its high word three bits, and NanoMig keeps five.
That is the cause [NM-HRM-043](NM-HRM-043.md) records, not this entry's
question. The sprite pointer's OCS width is not settled the same way: p. 127's
"32-bit word address" stands against the three bits of pp. 110, 285 and 291.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

---

## Why this entry is kept

Because the alternative is worse. A manual conflict that is noticed and not
recorded gets re-discovered by the next reader, who has no way to tell whether
anyone looked.

And it is the reason [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md),
[NM-HRM-024](NM-HRM-024.md) and [NM-HRM-043](NM-HRM-043.md) have to be read
together: if the manual applies its own extension inconsistently, then four
findings that each quote one of its sentences are four views of one unsettled
question.
