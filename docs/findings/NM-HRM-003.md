# NM-HRM-003 — OCS Copper address uses five high bits instead of three

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L142-L144) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L142-L144) |
| **Upstream** | **MiSTer** `rtl/agnus_copper.v:142-144` — identical to NanoMig: `cop1lch[20:16] <= data_in[4:0];`, five bits latched unconditionally. **TC64** `rtl/minimig/agnus_copper.v:142-144` — the same three lines, byte for byte. Both forks' only change to this file is elsewhere, lines 234-274, where `copjmp1`/`copjmp2`/`strobe1`/`strobe2`/`strobe` move from non-blocking to blocking assignment — the *same* eighteen lines, the same edit, in each — and neither touches COP1LCH. A fix here diverges from neither line |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if it is to be reported at all — see *Suggested change* |
| **Test** | [`hrm/agnus_copper_control`](../../hrm/agnus_copper_control), [`hrm/agnus_copper_locreg_width`](../../hrm/agnus_copper_locreg_width) |
| **Chapter** | 2 — Coprocessor · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | three do the same; Copperline alone narrows to 19 bits on OCS |
| **See also** | `NM-HRM-022` (blitter) and `NM-HRM-024` (audio) — the same decision in two other engines |

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
| **TC64** | [`rtl/minimig/agnus_copper.v:142-144`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L142-L144) | descendant — the same three lines, byte for byte |

**One of the four independent implementations implements what the manual
describes.**

---

## Which assertion carries the finding

`LW3` is the one that matters. The appendix gives **two different widths for the
two chipsets**, so a module reporting the same width in both contradicts the
manual whatever that width happens to be. The finding does not depend on
agreeing that OCS is three bits — only on the widths differing, which the manual
states plainly.

---

## Suggested change

**Read with `NM-HRM-022` and `NM-HRM-024`, not alone.** The Copper, the blitter
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
