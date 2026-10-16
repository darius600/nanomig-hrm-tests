# NM-HRM-022 — BLTxPTH carries five high bits in both chipsets [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Question: does an OCS Agnus (`ecs = 0`) latch only three high bits of BLTxPTH, so that writing `$001F` to BLTAPTH should drive `0x38000` where NanoMig drives `0xF8000`, the same as at `ecs = 1`? Closes when the change is made and `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` and `BLTXPTH-3-BIT-OCS-5-BIT-ECS-RANGE` turn green, or when an Amiga specialist declares the core ECS-and-later, which puts the manual's OCS sentences out of scope. Decide it together with [NM-HRM-003](NM-HRM-003.md) and [NM-HRM-024](NM-HRM-024.md), which rest on the same decision. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | `agnus_blitter`, BLTAPTH written with `$001F`, `address_out` compared at `ecs = 0` and `ecs = 1`; BLTBPTH, BLTCPTH and BLTDPTH not measured; real Agnus not measured; chip revision unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Chapter 6, printed p. 172; Appendix A, printed p. 270; Appendix C, printed p. 309 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. `BLTXPTH-WIDTH-REGARDLESS-OF-ECS`: `6-appa-registers.c110`, `ch6-appa-bltxpt.c2`, `ch6-appa-bltxpt.c6`, p. 270. `BLTXPTH-3-BIT-OCS-5-BIT-ECS-RANGE`: `6-dma-channels.c67`, `ch6-dma.c69`, p. 172. The control `BLTCON0L-GATED-BY-ECS`: `6-appa-registers.c25`, `6-appa-registers.c26`, `6-appa-registers.c27`, `ch6-appa-bltcon0l.c3` (p. 269), `ch6-appc-map.bltcon0l`, `6-appc-modifications.c11` (p. 298, the ECS register map; the ledger and the test give p. 309), `ch6-appc-ecs.c9` (p. 309). All three assertions carry claim ids |
| **Module** | `agnus_blitter.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v); inherited |
| **Test** | [`hrm/ch6-blitter-ecs`](../../hrm/ch6-blitter-ecs): `make -C hrm/ch6-blitter-ecs test`; an expected red exits nonzero |
| **Assertions** | 2 red, on one measurement: `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` expected the two chipsets to differ, observed `0xF8000` in both; `BLTXPTH-3-BIT-OCS-5-BIT-ECS-RANGE` expected `0x38000` at `ecs = 0` and `0xF8000` at `ecs = 1`, observed `0xF8000` at `ecs = 0`. The package reports `12 passed, 4 failed`. The control `BLTCON0L-GATED-BY-ECS`, the same write-and-compare against a register the RTL does gate, is green in the same run |
| **Verdict of the four** | **three of the four narrow by chipset**: Copperline masks the stored pointer; WinUAE ([`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623)) and vAmiga ([`Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194)) mask the address the DMA drives. Denise does not — it masks to the installed chip RAM, not by Agnus revision. Hardware: not measured |
| **Effort** | M — option 1 under *Decision*: one mask derived from the Agnus revision on every assembled DMA pointer, the Copper, audio and bitplane pointers of [NM-HRM-003](NM-HRM-003.md), [NM-HRM-024](NM-HRM-024.md) and [NM-HRM-088](NM-HRM-088.md) included; option 2 changes no RTL |
| **See also** | [NM-HRM-003](NM-HRM-003.md) (Copper), [NM-HRM-024](NM-HRM-024.md) (audio) and [NM-HRM-088](NM-HRM-088.md) (bitplane) — the same decision in three other engines |

---

## Source requirement

**Appendix A, printed p. 270**, on the blitter pointers:

> "Blitter pointer to x (high 3 bits, 5 bits if ECS)"

**Appendix C, printed p. 309**, in the list of registers ECS changed:

> BLTxPTH 050 W A Blitter pointer to x (high 5 bits, was 3 bits)

**Chapter 6, printed p. 172**, gives the direction:

> The blitter size and pointer registers have increased range under the
> Enhanced Chip Set (ECS).

Three on OCS, five on ECS. No page gives the original chip set more than three.

## RTL and measurement

NanoMig latches five bits, unconditionally. Every DMA pointer in this design is built the same way, with no chipset condition: [`agnus_blitter_adrgen.v:62`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter_adrgen.v#L62) takes `data_in[4:0]` for the high bits and `data_in[15:1]` for the low. The package measures at `agnus_blitter`, the parent that has the `ecs` port; `agnus_blitter_adrgen` has none.

| | `address_out` |
|---|---|
| `BLTAPTH = $001F` at `ecs = 0` | `0xF8000` |
| `BLTAPTH = $001F` at `ecs = 1` | `0xF8000` |

Identical. An OCS Agnus should drop the top two bits and land `0x38000`. The package grades this one measurement twice: `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` because the two rows are equal, `BLTXPTH-3-BIT-OCS-5-BIT-ECS-RANGE` because the `ecs = 0` row is not `0x38000`. Both are red; the control `BLTCON0L-GATED-BY-ECS` is green.

## Risks and unknowns

- Only BLTAPTH is written; BLTBPTH, BLTCPTH and BLTDPTH are not measured, and the claim for them rests on the shared line above.
- Real Agnus is not measured and no named software depends on the width.
- Denise does not narrow by chipset, and its one chipset condition widens ([`memory.cpp:695-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L695-L698)): an ECS Agnus with 512K chip and at least 512K slow RAM gets `dmaChipMemMask = 0xfffff`.
- WinUAE's mask is not a clean vote for the manual: [`cs_agnussize` overrides it either way (`:2627-2644`), and with JIT on, `chipmem_full_mask = chipmem_bank.allocated_size - 1` (`:2646-2648`) drops the chipset term](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2648).
- The three Commodore documents below predate ECS: they fix the OCS width and none states the five-bit ECS width. The 4,777,621 patent does not name BLTxPTH.

## Independent evidence

| | | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **narrows the BUS, not the register.** `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear — 19 bits on OCS, on the Agnus-side chip-RAM path disk and every other DMA fetch uses. The register itself stays full width |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:1621-1623`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1621-L1623)<br>[`Core/Components/Agnus/Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194) | **narrows the BUS, not the register**, by Agnus revision. Every Agnus DMA read goes through `Memory::peek16 <Accessor::AGNUS>`, which begins `addr &= agnus.ptrMask;`, and the mask is `0x07FFFF` on OCS and `0x0FFFFF` on ECS_1MB. The blitter's pointer register stays full width |
| **Denise** | [`emulation/libami/agnus/blitter.cpp:340-344`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitter.cpp#L340-L344)<br>[`emulation/libami/agnus/memory.cpp:695-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L695-L698) | masks the stored pointer, to the installed chip RAM, not by Agnus revision: `bltApt &= agnus.dmaChipMemMask;`, with `dmaChipMemMask = chipMemMask`, the chip RAM size less one |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118) | `dma_addr_capability_mask` returns `0x0007_FFFF` for `Self::Ocs`, applied centrally to every DMA pointer, blitter included |
| **MiSTer** | [`rtl/agnus_blitter_adrgen.v:62`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter_adrgen.v#L62) | descendant — identical to NanoMig: `assign bltptr_in[20:1] = enaptr ? newptr[20:1] : {data_in[4:0], data_in[15:1]};`. The instantiation at [`agnus_blitter.v:596`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L596) connects no `.ecs()` |
| **TC64** | [`rtl/minimig/agnus_blitter_adrgen.v:62`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter_adrgen.v#L62) | descendant — the same line, byte for byte. The instantiation at [`agnus_blitter.v:576`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L576) connects no `.ecs()`. A fix here diverges from neither line |

MiSTer and TC64 carry the same unconditional five-bit latch, inherited rather than independently arrived at.

### What other Commodore documents say

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "and the Blitter DMA control logic 65 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us5103499-copper/p016.txt -->

  The four Blitter channels' pointers are among the 25 pointer registers of 18 bits — 15 low plus 3 high word-address bits, the manual's OCS width. NanoMig takes five high bits from BLTxPTH (address_out 0xF8000, a 20-bit word address).

- **US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988**, PDF page 8 — agrees with the manual:

  > "The first custom chip 61 generates a 18 bit dynamic RAM address multiplexed onto a nine bit bus 75"
  <!-- corpus: patent-us4777621-system/p008.txt -->

  The bit map image manipulation circuitry 47 (the blitter) and the DMA address registers sit on chip 61 (p008), whose DRAM address is 18 bits — the manual's OCS reach. The finding measures address_out, the address the blitter DMA drives, so this bus-level statement bears on the same observable.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 21 — agrees with the manual:

  > "BLT x PTH Blitter pointer to x (high 3 bits) BLT x PTL a Blitter pointer to x (low 15 bits) This pair of registers contains the 18 bit address of Blitter source"
  <!-- corpus: agnus-8371-spec/p021.txt -->

  On the 8371 the blitter pointers have three high bits and an 18 bit address, and Table 2 (p031) repeats 'Blitter pointer to source C' with '(High 3 bits)'. That is the manual's OCS width, against NanoMig's unconditional five (address_out 0xF8000 at ecs=0).

## Decision

**Take this together with [NM-HRM-003](NM-HRM-003.md) and [NM-HRM-024](NM-HRM-024.md), not on its own.** The Copper, the blitter and the audio DMA all build their pointers identically with no chipset condition. Read separately they are three omissions; read together they are one decision not to model OCS pointer narrowing anywhere in Agnus. There are two coherent actions and no good third one:

1. **Model it once, centrally**, the way Copperline does — a mask derived from the Agnus revision, applied to every assembled DMA pointer. That closes all three findings with one change.
2. **Decide the core is ECS-and-later**, in which case the manual's OCS sentences are out of scope and all three findings are one scope note rather than three defects.

Three separate patches in three engines is the option to avoid. Under option 1 the two red assertions turn green when `ecs = 0` lands `0x38000` and `ecs = 1` lands `0xF8000`.
