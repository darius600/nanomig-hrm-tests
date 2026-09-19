# NM-HRM-024 — the audio DMA pointer takes five high bits in both chipsets

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_audiodma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_audiodma.v#L80-L82) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L80-L82) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch5-audio-location-pointer`](../../hrm/ch5-audio-location-pointer) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **two of the four narrow by chipset**: Copperline masks the stored pointer, WinUAE masks the address the DMA drives (`memory.cpp:2619-2623`). vAmiga and Denise do not — by Agnus revision |
| **See also** | `NM-HRM-003` (Copper) and `NM-HRM-022` (blitter) — the same decision in two other engines |

---

## What the HRM says

**Chapter 5, printed pp. 139 and 145**, on the audio location registers:

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

[`src/minimig-aga/agnus_audiodma.v#L80-L82`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_audiodma.v#L80-L82)

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

Nobody decides this but the manual. A dash means the source has not been read
for this finding.

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2623`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623) | **narrows the BUS, not the register.** `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear — 19 bits on OCS, on the Agnus-side chip-RAM path disk and every other DMA fetch uses. The register itself stays full width. Two caveats in the same function stop it being a clean vote for the manual: `cs_agnussize` overrides it either way (`:2627-2644`), and with JIT on, `if (currprefs.cachesize) chipmem_full_mask = chipmem_bank.allocated_size - 1` (`:2646-2648`) drops the chipset term entirely |
| **vAmiga** | — | no chipset narrowing |
| **Denise** | — | no mask on the high word at all |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118)<br>[`src/bus.rs:4991-4996`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4991-L4996) | <pre>pub fn dma_addr_capability_mask(self) -> u32 {<br>&nbsp;&nbsp;&nbsp;&nbsp;match self {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ocs =&gt; 0x0007_FFFF,<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ecs8372Rev4 =&gt; 0x000F_FFFF,<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ecs8375 \| Self::AgaAlice =&gt; 0x001F_FFFF,</pre> applied in `configure_chip_dma_masks` to Agnus, Denise, the blitter, Paula and the floppy alike |
| **MiSTer** | [`rtl/agnus_audiodma.v:80-82`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L80-L82) | descendant — byte-identical to NanoMig; the file has no differences from it at all |
| **TC64** | [`rtl/minimig/agnus_audiodma.v:80-82`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L80-L82) | descendant — likewise byte-identical; the file has no differences from NanoMig either. Neither fork's `agnus_audiodma` has an `ecs` port, and neither instantiation in `agnus.v` connects one. A fix here diverges from neither line, in either tree |

**Only Copperline models it**, and it models it centrally rather than per engine
— one mask derived from the Agnus revision, applied to every DMA pointer. Neither
MiSTer's nor TC64's `agnus_audiodma` has an `ecs` port, same as NanoMig — the
absence is inherited, not a second and third omission.

---

## This is the third engine, and that is the point

Copper ([`agnus_copper.v:144`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L144)),
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
assign address_out = {audlc[audlcsel][19:16] & {4{ecs}} | ..., ...};
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
- **The assertion's wording was chosen deliberately.** It asserts that the OCS
  three-bit reading does **not** hold — not that "AUDxLCH is five bits". NanoMig
  implements the ECS width unconditionally, so the OCS alternative cannot be
  selected for; a test demanding it would be demanding a mode that does not
  exist.

---

## For the specialists

If the intent is an ECS-only core, the manual's OCS sentences are simply out of
scope and `NM-HRM-003`, `NM-HRM-022` and this one are a single scope decision. If
not, three engines need the same narrowing, and Copperline has already shown it
is cheaper to do once.
