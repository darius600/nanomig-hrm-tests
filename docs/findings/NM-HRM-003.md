# NM-HRM-003 — OCS Copper address uses five high bits instead of three

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L142-L144) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L142-L144) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if it is to be reported at all — see *Suggested change* |
| **Test** | [`hrm/ch2-copper-control`](../../hrm/ch2-copper-control), [`hrm/ch2-copper-locreg-width`](../../hrm/ch2-copper-locreg-width) |
| **Chapter** | 2 — Coprocessor · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **two of the four narrow by chipset**: Copperline masks the stored pointer, WinUAE masks the address the DMA drives (`memory.cpp:2619-2623`). vAmiga and Denise do not — to 19 bits on OCS. The two do it in different places and the effect on a fetched address is the same |
| **See also** | [NM-HRM-022](NM-HRM-022.md) (blitter) and [NM-HRM-024](NM-HRM-024.md) (audio) — the same decision in two other engines |

---

## What the HRM says

**Printed pp. 273 and 309:** OCS has three high address bits, ECS five. **Printed
p. 276:** the location registers are 18-bit pointers.

---

## What NanoMig does

It latches five high bits with no chipset condition:

[`src/minimig-aga/agnus_copper.v#L142-L144`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L142-L144)

```verilog
  		cop1lch[20:16] <= 0;
  	else if (reg_address_in[8:1]==COP1LCH[8:1])
  		cop1lch[20:16] <= data_in[4:0];
```

### Measured

Five high bits in both chipsets; a 20-bit pointer, 15 low plus 5 high. The latch
is unconditional.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **narrows the BUS, not the register.** `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear — 19 bits on OCS, on the Agnus-side chip-RAM path disk and every other DMA fetch uses. The register itself stays full width. Two caveats in the same function stop it being a clean vote for the manual: `cs_agnussize` overrides it either way (`:2627-2644`), and with JIT on, `if (currprefs.cachesize) chipmem_full_mask = chipmem_bank.allocated_size - 1` (`:2646-2648`) drops the chipset term entirely |
| **vAmiga** | — | no chipset narrowing |
| **Denise** | [`emulation/libami/agnus/copper.cpp:345-347`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L345-L347) | <pre>auto Copper::setCOP1LCH(uint16_t value) -&gt; void {<br>&nbsp;&nbsp;&nbsp;&nbsp;cop1lc = (cop1lc & 0xffff) \| (value &lt;&lt; 16);</pre> takes the whole write, no mask |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118) | <pre>Self::Ocs =&gt; 0x0007_FFFF,</pre> **narrows to 19 bits on OCS** — the manual's width |
| **MiSTer** | [`rtl/agnus_copper.v:142-144`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L142-L144) | descendant — identical to NanoMig: `cop1lch[20:16] <= data_in[4:0];`, five bits latched unconditionally |
| **TC64** | [`rtl/minimig/agnus_copper.v:142-144`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L142-L144) | descendant — the same three lines, byte for byte. Both forks' only change to this file is elsewhere, lines 234-274, where `copjmp1`/`copjmp2`/`strobe1`/`strobe2`/`strobe` move from non-blocking to blocking assignment — the *same* eighteen lines, the same edit, in each — and neither touches COP1LCH. A fix here diverges from neither line |

**One of the four independent implementations implements what the manual
describes.**


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "Copper 47 (one channel), the audio DMA control logic 53 (four channels), the bitplane DMA control logic 57 (six channels), the disk and refresh DMA control logic 61 (2 channels), and the Blitter DMA control logic 65 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us5103499-copper/p016.txt -->

  The Copper's pointer (its program counter) is one of 25 pointer registers of 18 bits: 15 low plus 3 high word-address bits, 512K bytes (same page: the RAM address bus addresses 'one of 256K words (512K bytes)'). That is the manual's OCS width; NanoMig's pointer is 20 bits (15+5). The patent states the pointer register's width, not COP1LCH's directly; p017 says the location (backup) values are substituted into these pointers through the '18 bit adder 137'. It predates ECS, so it speaks only to the OCS half of the manual's two-width statement. The '19 bits wide' on p023 is about bitplane pointers and reads as the byte-address width of the same 512K range, not a contradiction.

- **US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988**, PDF page 8 — agrees with the manual:

  > "The first custom chip 61 generates a 18 bit dynamic RAM address multiplexed onto a nine bit bus 75"
  <!-- corpus: patent-us4777621-system/p008.txt -->

  Chip 61 houses the co-processor 45 and the address registers for the DMA channels (p008), and the DRAM address it generates is 18 bits — a 512K-byte reach, the manual's OCS width. This is a bus-level statement about the address the chip drives, not about COP1LCH's latch; it is the kind of narrowing the finding credits to WinUAE. Weakest of the agreeing rows; filed 1985, before ECS.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 26 — agrees with the manual:

  > "COPILCH Copper first location register (high 3 bits)"
  <!-- corpus: agnus-8371-spec/p026.txt -->

  The 8371 (OCS, PAL) specification gives COP1LCH three high bits (OCR 'COPILCH'), p027 calls COP1LC/COP2LC '18 bit pointer registers', and Table 2 (p032) repeats 'Co-processor first location register' with High 3 bits: the manual's OCS width, against NanoMig's unconditional five. It is an OCS document and speaks only to the OCS half of the manual's two-width statement; pp. 015 and 017 give every one of the 25 DMA channels an 18 bit pointer reaching 256K words (512K bytes).

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## Which assertion carries the finding

`LW3` is the one that matters. The appendix gives **two different widths for the
two chipsets**, so a module reporting the same width in both contradicts the
manual whatever that width happens to be. The finding does not depend on
agreeing that OCS is three bits — only on the widths differing, which the manual
states plainly.

---

## Suggested change

**Read with [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md), not alone.** The Copper, the blitter
and the audio DMA all build their pointers the same way with no chipset
condition. Two coherent actions:

1. **One central mask**, as Copperline does — derived from the Agnus revision and
   applied to every assembled DMA pointer. Closes all three findings at once.
2. **Declare the core ECS-and-later**, making the manual's OCS sentences out of
   scope, and record all three as one scope decision.

Three separate patches in three engines is the option to avoid.

---

## For the specialists

Three of four implementations do what NanoMig does. Read together with the other
two pointer findings, that points at the manual being read too strictly rather
than at the RTL being wrong — but the manual is this project's oracle and an
implementation never promotes to that. What the agreement earns is a careful
re-reading of pp. 273 and 309 before anyone patches three modules.
