# NM-HRM-022 — BLTxPTH carries five high bits in both chipsets

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if it is to be reported at all — see *Suggested change* |
| **Test** | [`hrm/ch6-blitter-ecs`](../../hrm/ch6-blitter-ecs) |
| **Chapter** | 6 — Blitter · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **two of the four narrow by chipset**: Copperline masks the stored pointer, WinUAE masks the address the DMA drives (`memory.cpp:2619-2623`). vAmiga and Denise do not — by Agnus revision |
| **See also** | [NM-HRM-003](NM-HRM-003.md) (Copper) and [NM-HRM-024](NM-HRM-024.md) (audio) — the same decision in two other engines |

---

## What the HRM says

**Appendix A, printed p. 270**, on the blitter pointers:

> "Blitter pointer to x (high 3 bits, 5 bits if ECS)"

Three on OCS, five on ECS.

---

## What NanoMig does

It latches five, unconditionally. Every DMA pointer in this design is built the
same way — `data_in[4:0]` for the high bits and `data_in[15:1]` for the low —
with no chipset condition anywhere.

### Measured

| | `address_out` |
|---|---|
| `BLTAPTH = $001F` at `ecs = 0` | `0xF8000` |
| `BLTAPTH = $001F` at `ecs = 1` | `0xF8000` |

Identical. An OCS Agnus should drop the top two bits.

---

## What the others say

| | | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **narrows the BUS, not the register.** `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear — 19 bits on OCS, on the Agnus-side chip-RAM path disk and every other DMA fetch uses. The register itself stays full width. Two caveats in the same function stop it being a clean vote for the manual: `cs_agnussize` overrides it either way (`:2627-2644`), and with JIT on, `if (currprefs.cachesize) chipmem_full_mask = chipmem_bank.allocated_size - 1` (`:2646-2648`) drops the chipset term entirely |
| **vAmiga** | — | no chipset narrowing |
| **Denise** | — | no mask on the high word |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118) | <pre>pub fn dma_addr_capability_mask(self) -&gt; u32 {<br>&nbsp;&nbsp;&nbsp;&nbsp;match self {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ocs =&gt; 0x0007_FFFF,</pre> applied centrally to every DMA pointer, blitter included |
| **MiSTer** | [`rtl/agnus_blitter_adrgen.v:62`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter_adrgen.v#L62) | descendant — identical to NanoMig: `assign bltptr_in[20:1] = enaptr ? newptr[20:1] : {data_in[4:0], data_in[15:1]};`. The instantiation at `agnus_blitter.v:596` connects no `.ecs()` |
| **TC64** | [`rtl/minimig/agnus_blitter_adrgen.v:62`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter_adrgen.v#L62) | descendant — the same line, byte for byte; the file is unchanged in both forks. The instantiation at `agnus_blitter.v:596` (MiSTer) / `agnus_blitter.v:576` (TC64, same line as NanoMig) connects no `.ecs()` in either tree. A fix here diverges from neither line |

**Three of the four independent implementations side with NanoMig.** Copperline
narrows in the register, and it does so in one place for all engines rather than per
engine. MiSTer and TC64 carry the same unconditional five-bit latch, inherited
rather than independently arrived at.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "and the Blitter DMA control logic 65 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us5103499-copper/p016.txt -->

  The four Blitter channels' pointers are among the 25 pointer registers of 18 bits — 15 low plus 3 high word-address bits, the manual's OCS width. NanoMig takes five high bits from BLTxPTH (address_out 0xF8000, a 20-bit word address). Pre-ECS; says nothing of the five-bit ECS width.

- **US patent 4,777,621, Commodore-Amiga**, PDF page 8 — agrees with the manual:

  > "The first custom chip 61 generates a 18 bit dynamic RAM address multiplexed onto a nine bit bus 75"
  <!-- corpus: patent-us4777621-system/p008.txt -->

  The bit map image manipulation circuitry 47 (the blitter) and the DMA address registers sit on chip 61 (p008), whose DRAM address is 18 bits — the manual's OCS reach. The finding measures address_out, the address the blitter DMA drives, so this bus-level statement bears on the same observable; it does not name BLTxPTH. Filed 1985, before ECS.

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; Commodore, *Denise* specification, part 252126-01 (ECS); Commodore, *A500/A2000 Gary Specification*; US patent 4,780,844, Commodore-Amiga; US patent 4,829,473, Commodore-Amiga; 6502.org dissection of Paula 8364R7; techtravels, *Inside Paula's Floppy Controller*, part I.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## Suggested change

**Take this together with [NM-HRM-003](NM-HRM-003.md) and [NM-HRM-024](NM-HRM-024.md), not on its own.** The
Copper, the blitter and the audio DMA all build their pointers identically with
no chipset condition. Read separately they are three omissions; read together
they are one decision not to model OCS pointer narrowing anywhere in Agnus.

So there are two coherent actions and no good third one:

1. **Model it once, centrally**, the way Copperline does — a mask derived from
   the Agnus revision, applied to every assembled DMA pointer. That closes all
   three findings with one change.
2. **Decide the core is ECS-and-later**, in which case the manual's OCS sentences
   are out of scope and all three findings are one scope note rather than three
   defects.

Three separate patches in three engines is the option to avoid.

---

## What holds the finding up

**Measured at `agnus_blitter`, the parent that has the `ecs` port** — after an
independent validation rejected handling it one fixture lower, where that port
does not exist. A test cannot demonstrate that a chipset input is ignored from
inside a module the input never reaches.

Every statement on this page was checked by reading the RTL directly rather than
taken from the classifier's report.

The measurement, its control and its mutation are recorded in the package. The
finding is kept separate from [NM-HRM-003](NM-HRM-003.md) because the two rest on separate
measurements at separate module boundaries — the same rule that keeps
[NM-HRM-016](NM-HRM-016.md) and [NM-HRM-017](NM-HRM-017.md) apart — even though they very likely have one
answer.

---

## For the specialists

Three of four implementations do what NanoMig does. Read with [NM-HRM-003](NM-HRM-003.md), where
the same is true, that points at the manual rather than at the RTL.

It does not close either one. The manual is this project's oracle and an
implementation never promotes to that — but where three independent
implementations and the design under test all disagree with a printed sentence,
the sentence is worth re-reading before the RTL is.
