# NM-HRM-024 — the audio DMA pointer takes five high bits in both chipsets [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_audiodma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_audiodma.v#L80-L82) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L80-L82) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch5-audio-location-pointer`](../../hrm/ch5-audio-location-pointer) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **three of the four narrow by chipset**: Copperline masks the stored pointer; WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Agnus.cpp:190-194`) mask the address the DMA drives. Denise does not — it masks to the installed chip RAM, not by Agnus revision |
| **See also** | [NM-HRM-003](NM-HRM-003.md) (Copper) and [NM-HRM-022](NM-HRM-022.md) (blitter) — the same decision in two other engines |

---

## What the HRM says

**Chapter 5, printed p. 139**, on the audio location registers (p. 145 repeats it
as "AUDxLCH (high three bits)"):

> they "contain the high three bits"

**Appendix A, printed p. 266**, on `AUDxLCH`:

> "(high 3 bits, 5 bits if ECS)" … "18 bit starting address"

**Appendix C's ECS register map, printed p. 298:**

> `0A0 AUDxLCH chg W A Audio channel x location(high 5 bits was 3 bits)`

Three is the OCS width; five is ECS. The manual is consistent: the width depends
on the chipset.

---

## What NanoMig does

It latches five bits, unconditionally:

[`src/minimig-aga/agnus_audiodma.v#L80-L82`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_audiodma.v#L80-L82)

```verilog
  if (clk7_en) begin
    if (audlcena & ~reg_address_in[1]) // AUDxLCH
      audlch[audlcsel] <= #1 data_in[4:0];
```

**The module has no `ecs` port, no chipset input, and no mention of a chipset in
its 157 lines.** There is nothing here to gate on, even in principle.

### Measured

A walking one through all five bits of `AUDxLCH`, on every one of the four
channels:

| written | `address_out` |
|---|---|
| bit 3 alone | `0x40000` — not zero |
| bit 4 alone | `0x80000` — not zero |
| all five | word address `0xFFFFF` |

Every bit lands at `address_out` bits 16–20. An OCS Agnus should ignore the top
two.

---

## What the others say

Nobody decides this but the manual.

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **narrows the BUS, not the register.** `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear — 19 bits on OCS, on the Agnus-side chip-RAM path disk and every other DMA fetch uses. The register itself stays full width. Two caveats in the same function stop it being a clean vote for the manual: `cs_agnussize` overrides it either way (`:2627-2644`), and with JIT on, `if (currprefs.cachesize) chipmem_full_mask = chipmem_bank.allocated_size - 1` (`:2646-2648`) drops the chipset term entirely |
| **vAmiga** | [`Core/Components/Agnus/AgnusDma.cpp:99-103`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L99-L103)<br>[`Core/Components/Memory/Memory.cpp:1621-1623`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1621-L1623)<br>[`Core/Components/Agnus/Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194) | **narrows the BUS, not the register**, by Agnus revision. The audio fetch is `mem.peek16 <Accessor::AGNUS> (audpt[channel])`, and that accessor begins `addr &= agnus.ptrMask;`, with the mask set per revision: <pre>case AgnusRevision::OCS:     ptrMask = 0x07FFFF; break;<br>case AgnusRevision::ECS_1MB: ptrMask = 0x0FFFFF; break;</pre> The location register itself stays full width |
| **Denise** | [`emulation/libami/agnus/dma.cpp:6-11`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L6-L11)<br>[`emulation/libami/agnus/memory.cpp:695-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L695-L698) | masks the stored pointer, but to the installed chip RAM, not by Agnus revision: `cha.ptrLatch &= dmaChipMemMask;`, with `dmaChipMemMask = chipMemMask`, the chip RAM size less one. Its one chipset condition widens rather than narrows: an ECS Agnus with 512K chip and at least 512K slow RAM gets `dmaChipMemMask = 0xfffff` |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118)<br>[`src/bus.rs:4991-4996`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4991-L4996) | <pre>pub fn dma_addr_capability_mask(self) -> u32 {<br>&nbsp;&nbsp;&nbsp;&nbsp;match self {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ocs =&gt; 0x0007_FFFF,<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ecs8372Rev4 =&gt; 0x000F_FFFF,<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ecs8375 \| Self::AgaAlice =&gt; 0x001F_FFFF,</pre> applied in `configure_chip_dma_masks` to Agnus, Denise, the blitter, Paula and the floppy alike |
| **MiSTer** | [`rtl/agnus_audiodma.v:80-82`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L80-L82) | descendant — byte-identical to NanoMig; the file has no differences from it at all |
| **TC64** | [`rtl/minimig/agnus_audiodma.v:80-82`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L80-L82) | descendant — likewise byte-identical; the file has no differences from NanoMig either. Neither fork's `agnus_audiodma` has an `ecs` port, and neither instantiation in `agnus.v` connects one. A fix here diverges from neither line, in either tree |

**Copperline models it in the register**, and it models it centrally rather than
per engine — one mask derived from the Agnus revision, applied to every DMA
pointer. WinUAE and vAmiga model it at the address every DMA fetch drives; Denise
masks to the installed chip RAM only. Neither
MiSTer's nor TC64's `agnus_audiodma` has an `ecs` port, same as NanoMig — the
absence is inherited, not a second and third omission.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "the audio DMA control logic 53 (four channels), the bitplane DMA control logic 57 (six channels), the disk and refresh DMA control logic 61 (2 channels), and the Blitter DMA control logic 65 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us5103499-copper/p016.txt -->

  The four audio channels' pointers are 18-bit registers, and p017 says the audio backup (location) value is reloaded into them through the '18 bit adder 137' — a 512K reach, the manual's OCS AUDxLCH width. NanoMig's audio address_out reaches word address 0xFFFFF (20 bits). Pre-ECS; the location registers' own width is not stated.

- **US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988**, PDF page 8 — agrees with the manual:

  > "The first custom chip 61 generates a 18 bit dynamic RAM address multiplexed onto a nine bit bus 75"
  <!-- corpus: patent-us4777621-system/p008.txt -->

  The DMA channels' address registers sit on chip 61 (p008), and DMA transfer to the audio generator from memory is one of the bus's listed transfers (p007); the DRAM address chip 61 drives is 18 bits, the manual's OCS reach. Bus-level: it does not name AUDxLCH. The finding's measurement is address_out, the same observable. Filed 1985, before ECS.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 21 — agrees with the manual:

  > "AUD x LCH Audio channel x location (high 3 bits) AUD x LCL Audio channel x location (low 15 bits) This pair of registers contains. the 18 bit starting address (location) of Audio channel x (x=0,1,2,3) DMA data."
  <!-- corpus: agnus-8371-spec/p021.txt -->

  On the 8371, AUDxLCH has three high bits and the pair holds an 18 bit starting address, and Table 2 (p032) gives 'Audio channel 1 location (High 3 bits' and the same for channels 2 and 3. That is the manual's OCS width, against NanoMig's audio address_out using bits 16-20. OCS only: the five-bit ECS width is outside this document.

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## This is the third engine, and that is the point

Copper ([`agnus_copper.v:144`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L144)),
blitter (`BLTxPTH`) and audio all build their pointer as `data_in[4:0]` for the
high bits and `data_in[15:1]` for the low, unconditionally.

Read separately they are three omissions. Read together they are **one decision
not to model OCS pointer narrowing anywhere in Agnus** — and audio is the case
where the module has nothing to gate on even in principle, because no chipset
signal reaches it.

---

## Suggested change

If OCS narrowing is wanted, it belongs where Copperline puts it: once, centrally,
derived from the chipset, rather than three times in three engines. In this RTL
that means an `ecs` input reaching the DMA engines and a mask applied to the
assembled pointer, not to the register write —

```verilog
// OCS Agnus reaches 19 bits; ECS reaches 21.
assign address_out[20:1] = {audptout[20:19] & {2{ecs}}, audptout[18:1]};
```

— sketched rather than written, because the shape depends on how `ecs` is
already threaded through `agnus.v` and that is upstream's call.

**If the intent is an ECS-and-later core, the correct action is no change**, and
the manual's OCS sentences are out of scope. That decision covers all three
findings at once, which is an argument for taking it deliberately rather than
per engine.

---

## What holds the finding up

- **The control is green on the identical methodology.**
  `LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS` uses the same walking one and passes, so
  the red is the OCS claim failing and not a dark fixture.
- **The red assertion demands the OCS reading outright.**
  `LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD` expects bits 3 and 4 of `AUDxLCH` to
  have no effect on the address; its label names what it finds. The module has no
  chipset input to select the OCS alternative by, so the expectation is absolute
  rather than per chipset.

---

## For the specialists

If the intent is an ECS-only core, the manual's OCS sentences are simply out of
scope and [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) and this one are a single scope decision. If
not, three engines need the same narrowing, and Copperline has already shown it
is cheaper to do once.
