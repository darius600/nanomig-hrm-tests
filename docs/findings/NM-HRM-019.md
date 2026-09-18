# NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented

| | |
|---|---|
| **Status** | open · red |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_sprites.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **three of four implement it**, and Denise scopes it exactly as the page's Warning states |
| **Origin** | the surviving half of the withdrawn `NM-HRM-013` |

---

## What the HRM says

**Printed p. 303** gives bit 4 a name *and a behaviour*, which is what makes this
a requirement rather than a mention:

```
04      SHSH1     Start horizontal (SHR mode) 70ns increment
```

scoped by the page's own Warning — *"70ns sprite positions are only available in
SuperHires mode"* — and by the ECS note.

**Compare bit 3 on the same page**, which the manual prints as `(unimplemented)`.
That contrast is the whole reason this half survived and the other did not.

---

## What NanoMig does

Nothing with it. Bit 4 is not read in the sprite path.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:1199-1203`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1199-L1203) | <pre>sprite_sprctlmask = 0x01 \| 0x08 \| 0x10;<br>} else if (ecs_denise) {<br>&nbsp;&nbsp;&nbsp;&nbsp;sprite_sprctlmask = 0x01 \| 0x10;<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;sprite_sprctlmask = 0x01;</pre> bit 4 (`0x10`) from ECS Denise on; bit 3 (`0x08`) only on AGA |
| **vAmiga** | — | does not implement it |
| **Denise** | [`emulation/libami/video/denise.cpp:299`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L299) | <pre>spr.shsh10 = (value & 0x10) &gt;&gt; 3;</pre> compared only when `_shres` |
| **Copperline** | [`src/video/bitplane/sprite.rs:1857-1859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1857-L1859) | <pre>pub(super) fn sprite_hsub_70ns(ctl: u16) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;ctl & 0x0010 != 0<br>}</pre> |
| **MiSTer** | — | descendant — `rtl/denise_sprites.v` — the same absence: no `sprctl[4]`, `SHSH1` or `shsh1` anywhere in the file |
| **TC64** | — | descendant — `rtl/minimig/denise_sprites.v` — likewise absent. A fix is new work on both lines; neither reads bit 4 for NanoMig to have dropped |

**Three of the four independent implementations implement it**, and Denise
scopes it exactly as p. 303's Warning states — read always, compared only in
SuperHires. MiSTer and TC64 have never read the bit either, which is the same
absence NanoMig has, not a fourth vote against it.

WinUAE's mask is the clearest statement of the ECS-versus-AGA split: `0x10` is
present from ECS Denise onwards, `0x08` only on AGA. That is the split the
withdrawn `NM-HRM-013` was broken into.

---

## Suggested change

Read bit 4 into the sprite's horizontal position as a 70 ns sub-position, and
apply it only in SuperHires. Denise's one-liner plus its `_shres` guard is the
smallest complete model; Copperline's named predicate is the tidiest.

```verilog
wire sprite_hsub_70ns = sprctl[4];   // SHSH1, appendix p.303 -- SuperHires only
```

with the position arithmetic taking it only when the sprite resolution is
SuperHires.

---

## Why this entry exists, and what it replaced

`NM-HRM-013` asserted that `SPRxCTL` bits 3 **and** 4 are live fields, resting on
the note *"bits 3 and 4 are in the ECS chips only"* — a statement about **which
chips have them**, not about what they do. That is not a requirement, and the
finding was withdrawn.

The two bits then part company:

| bit | what the manual prints | outcome |
|---|---|---|
| 3 | `(unimplemented)` | the manual settles it in the RTL's favour |
| 4 | `SHSH1  Start horizontal (SHR mode) 70ns increment` | a behaviour — this finding |

The withdrawal was right and the split is what it produced.
