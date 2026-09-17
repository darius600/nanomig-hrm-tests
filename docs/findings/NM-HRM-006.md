# NM-HRM-006 — DDFSTRT/DDFSTOP bit H3 is gated by the chipset, not by resolution

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L339) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L339) |
| **Upstream** | **MiSTer** `rtl/agnus_bitplanedma.v:339` — identical to NanoMig: `if (hpos[8:1]=={ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0})`, H3 gated on the chipset strap, not resolution. **TC64** `rtl/minimig/agnus_bitplanedma.v:313` — the same statement, byte for byte (the line number moves because an earlier, unrelated rewrite of the `hde`/`hdiwstrt` display-enable logic removes 26 lines above it). A fix here diverges from neither line |
| **Report to** | **nowhere, probably** — see *Suggested change* |
| **Test** | [`hrm/ch3-ddf-fetch-window`](../../hrm/ch3-ddf-fetch-window) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **all four gate on the chipset. None gates on resolution.** |

---

## What the HRM says

**Printed p. 91**, on `DDFSTRT` and `DDFSTOP`:

> Bits 7-2 — pixel position H8-H3 (**bit H3 only respected in Hires Mode.**)

---

## What NanoMig does

It gates H3 on the **chipset strap**, never on resolution:

[`src/minimig-aga/agnus_bitplanedma.v#L339`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L339) and [`#L349`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L349)

```verilog
      if (hpos[8:1]=={ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0})
      if (hpos[8:1] == {ddfstop[8:3], ddfstop[2] & ecs, 1'b0})
```

`ddfstrt[2]` is H3, and it is ANDed with **`ecs`** — the chipset strap, declared
on line 35 as *"ddfstrt/ddfstop ECS bits enable"*. Resolution never enters the
expression.

So H3 is honoured on an ECS machine in **lores** as well, and ignored on an OCS
machine in **hires** — the opposite of the printed sentence in both directions.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:1210`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1210) | <pre>ddf_mask = ecs_agnus ? 0xfe : 0xfc;</pre> applied at `:3663` / `:3674` |
| **vAmiga** | — | respects H3 in lores too |
| **Denise** | [`emulation/libami/agnus/register.cpp:293`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L293) | <pre>ddfStart = value & (ecsAndHigher() ? 0xfe : 0xfc);</pre> |
| **Copperline** | [`src/chipset/agnus.rs:490-496`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L490-L496), [`:498-499`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L498-L499) | <pre>pub(crate) fn ddf_register_mask(revision: AgnusRevision) -&gt; u16 {<br>&nbsp;&nbsp;&nbsp;&nbsp;if matches!(revision, AgnusRevision::Ocs) { 0x00FC } else { 0x00FE }</pre> and then <pre>pub fn effective_bitplane_ddf_start_hpos(revision: AgnusRevision, bplcon0: u16, raw: u16) -&gt; u16 {<br>&nbsp;&nbsp;&nbsp;&nbsp;let _ = bplcon0;</pre> |

**All four gate the DDF granularity on the chipset. None gates it on
resolution.** Copperline is the sharpest statement of it: its function *takes*
the resolution register as a parameter and then discards it with
`let _ = bplcon0;`. Page 91's "only respected in Hires Mode" is the outlier.

---

## Suggested change

**Probably none.** Four independent implementations agree with NanoMig against
the printed sentence, and one of them discards the resolution input explicitly
enough to look like a decision rather than an oversight.

The likely reading is that p. 91's sentence is an **OCS-era statement never
revised for ECS**: on an OCS machine H3 is masked off entirely, and the only
place the bit could be seen to matter was hires. If that is right, the manual is
incomplete rather than NanoMig wrong.

Settling it needs someone who knows what ECS Agnus does with H3 in lores — not a
patch.

---

## What holds the finding up

The test asserts what page 91 says, and the page says it plainly. The question
this finding raises is not whether the test is right but whether the sentence
still is.
