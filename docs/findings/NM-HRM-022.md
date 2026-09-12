# NM-HRM-022 — BLTxPTH carries five high bits in both chipsets

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v) |
| **Upstream** | inherited from Minimig-AGA, unchanged there |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if it is to be reported at all — see *Suggested change* |
| **Test** | [`hrm/agnus_blitter_ecs`](../../hrm/agnus_blitter_ecs) |
| **Chapter** | 6 — Blitter · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | three do the same; Copperline alone narrows by Agnus revision |
| **See also** | `NM-HRM-003` (Copper) and `NM-HRM-024` (audio) — the same decision in two other engines |

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
| **WinUAE** | — | no chipset narrowing of blitter pointers |
| **vAmiga** | — | no chipset narrowing |
| **Denise** | — | no mask on the high word |
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118) | <pre>pub fn dma_addr_capability_mask(self) -&gt; u32 {<br>&nbsp;&nbsp;&nbsp;&nbsp;match self {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Self::Ocs =&gt; 0x0007_FFFF,</pre> applied centrally to every DMA pointer, blitter included |

**Three of four side with NanoMig.** Copperline alone narrows, and it does so in
one place for all engines rather than per engine.

---

## Suggested change

**Take this together with `NM-HRM-003` and `NM-HRM-024`, not on its own.** The
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
finding is kept separate from `NM-HRM-003` because the two rest on separate
measurements at separate module boundaries — the same rule that keeps
`NM-HRM-016` and `NM-HRM-017` apart — even though they very likely have one
answer.

---

## For the specialists

Three of four implementations do what NanoMig does. Read with `NM-HRM-003`, where
the same is true, that points at the manual rather than at the RTL.

It does not close either one. The manual is this project's oracle and an
implementation never promotes to that — but where three independent
implementations and the design under test all disagree with a printed sentence,
the sentence is worth re-reading before the RTL is.
