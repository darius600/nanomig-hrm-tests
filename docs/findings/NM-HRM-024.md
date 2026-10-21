# NM-HRM-024 — the audio DMA pointer takes five high bits in both chipsets [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should the audio DMA address at `ecs = 0` take only three high bits of AUDxLCH, as pp. 139 and 266 print, instead of the five it takes in both chipsets? Closes when the central pointer mask under *Decision* is made and `LCH-OCS-HOLDS-ONLY-THREE-BITS-AT-REAL-ECS` and `LCH-ECS-PORT-CHANGES-WHAT-LOADS` are green, or when an Amiga specialist records the core as ECS-and-later for this page, [NM-HRM-003](NM-HRM-003.md) and [NM-HRM-022](NM-HRM-022.md). `LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD` grades `agnus_audiodma` alone, which has no chipset input (*Risks and unknowns*). |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | AUDxLCH's high field: all four channels at `agnus_audiodma` alone, which has no chipset input; channel 0 at the Agnus top with `ecs` at 0 and at 1; five bits latched in both; chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 5 — Audio, printed pp. 139 and 145; Appendix A, printed p. 266; Appendix B, printed p. 290; Appendix C, printed pp. 298 and 309 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (47)</summary> `LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD`: `5-p139.c1`, `5-loc.c1`, p. 139; `5-sum.c1`, p. 145; `5-appA-p266.c5`, `5-appA-p266.c11`, `A-audxlc.c1`, `A-audxlch.w1`, p. 266; `5-appB-p290.c57`, `5-appB-p290.c92`, `5-appB-p290.c127`, `5-appB-p290.c148`, `B-aud0lch.width`, `B-aud1lch.width`, `B-aud2lch.width`, `B-aud3lch.width`, p. 290; `5-appC-p298.c6`, `C-map.lch.width`, p. 298; `5-appC-p309.c6`, p. 309; `LCH-OCS-HOLDS-ONLY-THREE-BITS-AT-REAL-ECS`: `5-p139.c1`, `5-loc.c1`, p. 139; `5-sum.c1`, p. 145; `5-appA-p266.c11`, `A-audxlc.c1`, p. 266; `5-appB-p290.c57`, `5-appB-p290.c92`, `5-appB-p290.c127`, `B-aud1lch.width`, `B-aud2lch.width`, `B-aud3lch.width`, p. 290; `LCH-ECS-PORT-CHANGES-WHAT-LOADS`: `5-appA-p266.c5`, `A-audxlch.w1`, p. 266; `5-appB-p290.c148`, `B-aud0lch.width`, p. 290; `5-appC-p298.c6`, `C-map.lch.width`, p. 298; `5-appC-p309.c6`, p. 309; green `LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS`: `5-appA-p266.c6`, `A-audxlch.w1`, p. 266; `5-appB-p290.c149`, `B-aud0lch.width`, p. 290; `5-appC-p298.c6`, `C-map.lch.width`, p. 298; `5-appC-p309.c1`, `5-appC-p309.c2`, `5-appC-p309.c6`, `C-prose.c1`, `C-prose.lch`, p. 309. `TOP-LEVEL-LIVENESS` and `TEST-DATA-DELIVERY` carry no claim id </details> |
| **Module** | `agnus_audiodma.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_audiodma.v#L80-L82) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L80-L82); inherited (MiSTer and TC64 have the same lines) |
| **Test** | [`hrm/ch5-audio-location-pointer`](../../hrm/ch5-audio-location-pointer), two fixtures: `agnus_audiodma` alone, and the Agnus top, `agnus`, whose `ecs` input the fixture drives to 0 and to 1: `make -C hrm/ch5-audio-location-pointer test`; an expected red exits nonzero |
| **Assertions** | 3 red in `hrm/ch5-audio-location-pointer` (10 passed, 3 failed: `agnus_audiodma` alone 9 passed, 1 failed; Agnus top 1 passed, 2 failed). `LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD` (`agnus_audiodma` alone): bits 3 and 4 of AUDxLCH expected to have no effect, observed `0x40000` and `0x80000`. `LCH-OCS-HOLDS-ONLY-THREE-BITS-AT-REAL-ECS` (Agnus top, `ecs` 0): the same expectation, the same observed values. `LCH-ECS-PORT-CHANGES-WHAT-LOADS`: AUD0LCH = $001F with AUD0LCL = $FFFF expected `0x3FFFF` at `ecs` 0 and `0xFFFFF` at `ecs` 1, observed `0xFFFFF` at both |
| **Verdict of the four** | **three of the four narrow by chipset**: Copperline masks the stored pointer; WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Agnus.cpp:190-194`) mask the address the DMA drives. Denise does not — it masks to the installed chip RAM, not by Agnus revision. Hardware: not measured |
| **Effort** | none; if option 1 under *Decision* is chosen, M: an `ecs` input to the audio engine, connected in `agnus.v`, masking the assembled pointer, and the same pointer assembly in the Copper and blitter ([NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md)); option 2 changes no RTL |
| **See also** | [NM-HRM-003](NM-HRM-003.md) (Copper), [NM-HRM-022](NM-HRM-022.md) (blitter) and [NM-HRM-088](NM-HRM-088.md) (bitplane) — the same decision in three other engines |

---

## Source requirement

**Chapter 5, printed p. 139** (p. 145 repeats it as "AUDxLCH (high three bits)"):

> they "contain the high three bits"

**Appendix A, printed p. 266**, on `AUDxLCH`:

> "(high 3 bits, 5 bits if ECS)" … "18 bit starting address"

**Appendix C, printed p. 298:**

> `0A0 AUDxLCH chg W A Audio channel x location(high 5 bits was 3 bits)`

Three is the OCS width, five is ECS: the width depends on the chipset.

---

## RTL and measurement

NanoMig latches five bits, unconditionally
([`agnus_audiodma.v#L80-L82`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_audiodma.v#L80-L82)):

```verilog
    if (audlcena & ~reg_address_in[1]) // AUDxLCH
      audlch[audlcsel] <= #1 data_in[4:0];
```

The module has no `ecs` port in its 157 lines. The Agnus top declares `ecs` at
[`agnus.v:97`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L97)
and connects it to the bitplane, sprite, Copper, blitter and beam-counter
instances (e.g. [`agnus.v:369`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L369)),
but the audio engine's instantiation,
[`agnus.v:344-357`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L344-L357),
has ten connections and no `ecs`.

The package reads the pointer at `address_out` (word address). `agnus_audiodma`
alone: a walking one through all five bits on every one of the four channels.
Agnus top: AUD0LCH and AUD0LCL written through the CPU register bus, read in
channel 0's audio DMA slot.

| written | alone | top, `ecs` 0 | top, `ecs` 1 |
|---|---|---|---|
| bit 3 of AUDxLCH alone | `0x40000` | `0x40000` | not measured |
| bit 4 of AUDxLCH alone | `0x80000` | `0x80000` | not measured |
| AUD0LCH = $001F, AUD0LCL = $FFFF (alone: all five bits) | `0xFFFFF` | `0xFFFFF`; the page wants `0x3FFFF` | `0xFFFFF`, as p. 266 gives |

The three-bit reading would give `0` for the first two rows. The five-bit
control `LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS` (same walking one: every bit lands at `address_out` bits 16–20) and the two fixtures' liveness checks, `TEST-DATA-DELIVERY` (alone) and `TOP-LEVEL-LIVENESS` (AUD0LCH = $0015, AUD0LCL = $2A2A at `ecs` 1 reads `0xA9515`,
the word address written) are green, so the reds are not a dark fixture.

---

## Risks and unknowns

- Real hardware is not measured; no named software depends on the width.
- `agnus_audiodma` alone has no chipset input, so `LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD` grades the three-bit reading as the only one; the Agnus-top reds are per chipset.
- Denise masks by installed chip RAM, not by Agnus revision, so the vote is three of four.
- WinUAE is not a clean vote for the manual: `cs_agnussize` overrides its mask either way ([`memory.cpp:2627-2644`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2627-L2644)), and with JIT on, `if (currprefs.cachesize) chipmem_full_mask = chipmem_bank.allocated_size - 1` ([`memory.cpp:2646-2648`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2646-L2648)) drops the chipset term.
- The patents and the 8371 specification are pre-ECS or OCS only: the location registers' own width is not stated in the patents, patent 4,777,621 is bus-level and does not name AUDxLCH, and the five-bit ECS width is outside the 8371 document.

---

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **narrows the bus, not the register**: `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear, 19 bits on OCS, on the chip-RAM path every DMA fetch uses |
| **vAmiga** | [`AgnusDma.cpp:99-103`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L99-L103), [`Memory.cpp:1621-1623`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1621-L1623), [`Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194) | **narrows the bus, not the register**, by Agnus revision: the audio fetch is `mem.peek16 <Accessor::AGNUS> (audpt[channel])`, which begins `addr &= agnus.ptrMask;`, and `ptrMask` is `0x07FFFF` on OCS, `0x0FFFFF` on ECS_1MB |
| **Denise** | [`dma.cpp:6-11`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L6-L11), [`memory.cpp:695-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L695-L698) | masks the stored pointer to the installed chip RAM, not by Agnus revision: `cha.ptrLatch &= dmaChipMemMask;`. Its one chipset condition widens: an ECS Agnus with 512K chip and at least 512K slow RAM gets `0xfffff` |
| **Copperline** | [`agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118), [`bus.rs:4991-4996`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4991-L4996) | **masks the stored pointer** by Agnus revision (`0x0007_FFFF` OCS, `0x000F_FFFF` Ecs8372Rev4, `0x001F_FFFF` Ecs8375 and AGA), one mask applied to every DMA pointer |
| **MiSTer** | [`rtl/agnus_audiodma.v:80-82`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L80-L82) | descendant — byte-identical to NanoMig |
| **TC64** | [`rtl/minimig/agnus_audiodma.v:80-82`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L80-L82) | descendant — byte-identical to NanoMig; neither fork's module has an `ecs` port or connects one in `agnus.v`, so a fix here diverges from neither line |

### What other Commodore documents say

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "the audio DMA control logic 53 (four channels), the bitplane DMA control logic 57 (six channels), the disk and refresh DMA control logic 61 (2 channels), and the Blitter DMA control logic 65 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us5103499-copper/p016.txt -->

  The four audio channels' pointers are 18-bit registers, and p017 says the audio backup (location) value is reloaded into them through the '18 bit adder 137' — a 512K reach, the manual's OCS AUDxLCH width. NanoMig's audio address_out reaches word address 0xFFFFF (20 bits).

- **US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988**, PDF page 8 — agrees with the manual:

  > "The first custom chip 61 generates a 18 bit dynamic RAM address multiplexed onto a nine bit bus 75"
  <!-- corpus: patent-us4777621-system/p008.txt -->

  The DMA channels' address registers sit on chip 61 (p008), and DMA transfer to the audio generator from memory is one of the bus's listed transfers (p007); the DRAM address chip 61 drives is 18 bits, the manual's OCS reach. The finding's measurement is address_out, the same observable.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 21 — agrees with the manual:

  > "AUD x LCH Audio channel x location (high 3 bits) AUD x LCL Audio channel x location (low 15 bits) This pair of registers contains. the 18 bit starting address (location) of Audio channel x (x=0,1,2,3) DMA data."
  <!-- corpus: agnus-8371-spec/p021.txt -->

  On the 8371, AUDxLCH has three high bits and the pair holds an 18 bit starting address, and Table 2 (p032) gives 'Audio channel 1 location (High 3 bits' and the same for channels 2 and 3. That is the manual's OCS width, against NanoMig's audio address_out using bits 16-20.

---

## Decision

The question and its closure are in the header. Read with
[NM-HRM-003](NM-HRM-003.md) and [NM-HRM-022](NM-HRM-022.md): the Copper
([`agnus_copper.v:144`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L144)),
the blitter (`BLTxPTH`) and audio all build the pointer from `data_in[4:0]` and
`data_in[15:1]` with no chipset condition, so this is one decision not to model
OCS pointer narrowing anywhere in Agnus. Two coherent actions:

1. **One central mask**, as Copperline does: derived from the chipset, applied to
   the assembled pointer rather than the register write.
   In this RTL that needs an `ecs` input reaching the DMA engines. Sketched, not
   written, because the shape depends on how `ecs` is threaded through `agnus.v`:

   ```verilog
   // OCS Agnus reaches 19 bits; ECS reaches 21.
   assign address_out[20:1] = {audptout[20:19] & {2{ecs}}, audptout[18:1]};
   ```

2. **Declare the core ECS-and-later**: no change, the manual's OCS sentences are
   out of scope, and all three findings are recorded as one scope decision.

Three separate patches in three engines is the option to avoid.
