# NM-HRM-003 — OCS Copper address uses five high bits instead of three [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should COP1LCH at `ecs = 0` latch three high bits, as pp. 273 and 309 print, instead of the five it latches in both chipsets? Closes when COP1LCH at `ecs = 0` keeps three high bits at the Copper module's own address output, turning `T4`, `LW1`, `LW3` and `LW4` green (option 1 under *Decision*), or when an Amiga specialist rejects the change by declaring the core ECS-and-later for this page, [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md) (option 2), in which case this page is refiled against the classification. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | COP1LCH's high field at ecs = 0 (OCS) and at ecs = 1 (ECS), five bits latched in both; the identical COP2LCH latch (`agnus_copper.v:159-161`) is not among the red assertions; chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 2 — Coprocessor Hardware, printed p. 25; Appendix A, printed pp. 273 and 276; Appendix B, printed p. 290; Appendix C, printed p. 309 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (17)</summary> claims by assertion: `T4`: `COP2-location-registers.c2` (p. 25); `LW1`: `COP2-cop1lch.c5`, `COP2-cop1lch-ecs.c6`, `COP2-location-registers.c2`, `ch2-appa-location-registers.c4`, `ch2-appb-table.c18`, `ch2-appc-ecs.c6`, `ch2-location-registers.c2`, `ch2-location-registers.c7`, `COP2-cop2lch.c5`, `COP2-cop-location-pointers.c7`, `COP2-location-registers.c4` (pp. 25, 273, 276, 290, 309); `LW3`: `ch2-appa-location-registers.c3.s2`, `ch2-appb-table.c17.s2` (pp. 273, 290); `LW4`: `COP2-cop-location-pointers.c4`, `COP2-cop-location-pointers.c7`, `ch2-appa-copins.c140` (p. 276). All four red assertions carry claim ids </details> |
| **Module** | `agnus_copper.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L142-L144) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L142-L144); inherited (MiSTer and TC64 have the same three lines) |
| **Test** | [`hrm/ch2-copper-control`](../../hrm/ch2-copper-control) and [`hrm/ch2-copper-locreg-width`](../../hrm/ch2-copper-locreg-width): `make -C hrm/ch2-copper-control test`; `make -C hrm/ch2-copper-locreg-width test`; an expected red exits nonzero |
| **Assertions** | 4 red. `T4` in `hrm/ch2-copper-control` (27 passed, 1 failed): COP1LCH written with high word `0x001D` at `ecs = 0`, expected first-fetch word address `0x29234`, observed `0xe9234`. In `hrm/ch2-copper-locreg-width` (12 passed, 3 failed): `LW1` expects a 3-bit high field at `ecs = 0`, observed 5 bits; `LW3` expects the two chipsets to differ in width, observed the same width in both; `LW4` expects an 18-bit pointer, observed 20 (15 low + 5 high). `LW3` is the one that matters |
| **Verdict of the four** | **three of the four narrow by chipset**, to 19 bits on OCS: Copperline masks the stored pointer, WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Memory.cpp:1623`) mask the address the DMA drives. Denise does not — it masks to the installed chip RAM, not by Agnus revision. The three do it in different places and the effect on a fetched address is the same. Hardware: not measured |
| **Effort** | M — option 1 under *Decision* puts one revision-derived mask on every assembled DMA pointer: the Copper, blitter and audio pointers of this page, NM-HRM-022 and NM-HRM-024, and the bitplane and disk pointers of NM-HRM-088 and NM-HRM-043; option 2 changes no RTL |
| **See also** | [NM-HRM-022](NM-HRM-022.md) (blitter), [NM-HRM-024](NM-HRM-024.md) (audio) and [NM-HRM-088](NM-HRM-088.md) (bitplane) — the same decision in three other engines |

---

## Source requirement

**Printed p. 273**, the COP1LCH row:

> (high 3 bits, high 5 bits if ECS)

**Printed p. 309**, the Appendix C row:

> (high 5 bits,was 3 bits)

**Printed p. 276:**

> These are 18-bit pointer registers whose contents are used to modify the program counter for initialization or jumps.

---

## RTL and measurement

NanoMig latches five high bits with no chipset condition:

[`src/minimig-aga/agnus_copper.v#L142-L144`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L142-L144)

```verilog
  		cop1lch[20:16] <= 0;
  	else if (reg_address_in[8:1]==COP1LCH[8:1])
  		cop1lch[20:16] <= data_in[4:0];
```

Five high bits in both chipsets; a 20-bit pointer, 15 low plus 5 high. The latch
is unconditional.

`LW3` carries the finding. The appendix gives **two different widths for the two
chipsets**, so a module reporting the same width in both contradicts the manual
whatever that width is; the finding needs only that the widths differ. `T4` shows
the same defect on a fetch address, and `LW4` is not a separate defect: 15 + 5 = 20
against p. 276's 18 follows from `LW1`.

---

## Risks and unknowns

- Real hardware is not measured; no named software depends on the width.
- Option 1 under *Decision* would turn the regression pins `LR4` (`test_agnus_copper_locreg_width.cpp:365`, COP1LCH) and, since the mask covers every assembled pointer, `LR3` (`:311`, COP2LCH) in `hrm/ch2-copper-locreg-width` red: both are green now because they record a five-bit field with the harness in OCS (`ecs = 0`).
- Denise masks by installed chip RAM, not by Agnus revision, so the vote is three of four, not four.
- WinUAE is not a clean vote for the manual: `cs_agnussize` overrides its mask either way (`memory.cpp:2627-2644`), and with JIT on, `if (currprefs.cachesize) chipmem_full_mask = chipmem_bank.allocated_size - 1` (`:2646-2648`) drops the chipset term.
- The second documents speak only to the OCS half of the manual's two-width statement: none of the three speaks to ECS, and the patents state the pointer register's width, not COP1LCH's directly. The `19 bits wide` on patent p023 is about bitplane pointers and reads as the byte-address width of the same 512K range, not a contradiction.
- Patent 4,777,621 is a bus-level statement about the address the chip drives, not COP1LCH's latch, and the weakest of the agreeing rows.

---

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **narrows the BUS, not the register.** `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear — 19 bits on OCS, on the Agnus-side chip-RAM path disk and every other DMA fetch uses. The register itself stays full width |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:1623`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1623) | <pre>addr &= agnus.ptrMask;</pre> **narrows the BUS, not the register**, as WinUAE does. Every Agnus DMA read goes through this `peek16 <Accessor::AGNUS>`, the Copper's included ([`AgnusDma.cpp:264`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L264)), and [`Agnus.cpp:190-191`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L191) sets `ptrMask = 0x07FFFF` for an OCS Agnus: 19 bits. The register keeps the whole high word (`cop1lc = REPLACE_HI_WORD(cop1lc, value);`, [`CopperRegs.cpp:82`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperRegs.cpp#L82)) |
| **Denise** | [`emulation/libami/agnus/copper.cpp:345-347`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L345-L347)<br>[`emulation/libami/agnus/dma.cpp:416-424`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L416-L424) | <pre>auto Copper::setCOP1LCH(uint16_t value) -&gt; void {<br>&nbsp;&nbsp;&nbsp;&nbsp;cop1lc = (cop1lc & 0xffff) \| (value &lt;&lt; 16);</pre> takes the whole write, no mask. The fetch masks with `dmaChipMemMask` (`addrBus = adr & dmaChipMemMask;`), the installed chip RAM less one, not the Agnus revision |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118) | <pre>Self::Ocs =&gt; 0x0007_FFFF,</pre> **narrows to 19 bits on OCS** — the manual's width |
| **MiSTer** | [`rtl/agnus_copper.v:142-144`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L142-L144) | descendant — identical to NanoMig: `cop1lch[20:16] <= data_in[4:0];`, five bits latched unconditionally |
| **TC64** | [`rtl/minimig/agnus_copper.v:142-144`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L142-L144) | descendant — the same three lines, byte for byte. The file's only difference from NanoMig is at lines 234-274, where MiSTer and TC64 both assign `copjmp1`/`copjmp2`/`strobe1`/`strobe2`/`strobe` blocking and NanoMig non-blocking; it does not touch COP1LCH, so a fix here diverges from neither line |

### What other Commodore documents say

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "Copper 47 (one channel), the audio DMA control logic 53 (four channels), the bitplane DMA control logic 57 (six channels), the disk and refresh DMA control logic 61 (2 channels), and the Blitter DMA control logic 65 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us5103499-copper/p016.txt -->

  The Copper's pointer (its program counter) is one of 25 pointer registers of 18 bits: 15 low plus 3 high word-address bits, 512K bytes (same page: the RAM address bus addresses 'one of 256K words (512K bytes)'). That is the manual's OCS width; NanoMig's pointer is 20 bits (15+5). p017 says the location (backup) values are substituted into these pointers through the '18 bit adder 137'.

- **US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988**, PDF page 8 — agrees with the manual:

  > "The first custom chip 61 generates a 18 bit dynamic RAM address multiplexed onto a nine bit bus 75"
  <!-- corpus: patent-us4777621-system/p008.txt -->

  Chip 61 houses the co-processor 45 and the address registers for the DMA channels (p008), and the DRAM address it generates is 18 bits — a 512K-byte reach, the manual's OCS width; the kind of narrowing the finding credits to WinUAE.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 26 — agrees with the manual:

  > "COPILCH Copper first location register (high 3 bits)"
  <!-- corpus: agnus-8371-spec/p026.txt -->

  The 8371 (OCS, PAL) specification gives COP1LCH three high bits (OCR 'COPILCH'), p027 calls COP1LC/COP2LC '18 bit pointer registers', and Table 2 (p032) repeats 'Co-processor first location register' with High 3 bits: the manual's OCS width, against NanoMig's unconditional five. pp. 015 and 017 give every one of the 25 DMA channels an 18 bit pointer reaching 256K words (512K bytes).

---

## Decision

The question is in the header. **Read with [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md), not alone.** The Copper, the blitter
and the audio DMA all build their pointers the same way with no chipset
condition. Two coherent actions:

1. **One central mask**, as Copperline does — derived from the Agnus revision and
   applied to every assembled DMA pointer. Closes all three findings at once.
2. **Declare the core ECS-and-later**, making the manual's OCS sentences out of
   scope, and record all three as one scope decision.

Three separate patches in three engines is the option to avoid. What is left to decide is scope, the same decision as in
[NM-HRM-022](NM-HRM-022.md), [NM-HRM-024](NM-HRM-024.md) and
[NM-HRM-043](NM-HRM-043.md): whether this core models an OCS Agnus at all. The
three implementations that narrow side with the manual, not with NanoMig's
unconditional five bits, so the printed page is not the outlier.
