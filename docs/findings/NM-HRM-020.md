# NM-HRM-020 — BLTSIZV is not gated on ECS, while its two siblings are

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v#L546-L549) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L566-L569) |
| **Upstream** | inherited — the line is identical in both trees, and unchanged upstream |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/agnus_blitter_ecs`](../../hrm/agnus_blitter_ecs) |
| **Chapter** | 6 — Blitter · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **all four gate it on ECS. NanoMig alone does not.** |

---

## What the HRM says

**Appendix C's ECS register map, printed p. 298**, marks `BLTSIZV $05C` **new for
ECS** — the same status it gives `BLTSIZH $05E` and `BLTCON0L $05A`.

Three registers, one appendix page, one status.

---

## What NanoMig does

It gates two of the three and not the third. `BLTCON0L` is gated:

[`src/minimig-aga/agnus_blitter.v#L257-L260`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v#L257-L260)

```verilog
  	else if (reg_address_in[8:1]==BLTCON0[8:1] || reg_address_in[8:1]==BLTCON0L[8:1] && ecs)
  		bltcon0[7:0] <= data_in[7:0];
```

`BLTSIZV` is not — and **the line carries the contradiction in its own comment**,
which calls the register ECS while the code accepts it unconditionally:

[`src/minimig-aga/agnus_blitter.v#L546-L549`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v#L546-L549)

```verilog
  	if (reset)
  		height[14:0] <= 0;
  	else if (reg_address_in[8:1]==BLTSIZV[8:1]) // ECS BLTSIZV register
  		height[14:0] <= data_in[14:0];
```

### Measured

| | height loaded |
|---|---|
| `BLTSIZV` write with `ecs = 0` | 7 |
| `BLTSIZV` write with `ecs = 1` | 7 |

Identical. An OCS Agnus should ignore the write entirely.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a checkout at
the pinned revision and the quotation verified there.

| | code | |
|---|---|---|
| **WinUAE** | — | gates all three |
| **vAmiga** | — | gates all three |
| **Denise** | [`emulation/libami/agnus/register.cpp:222-224`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L222-L224) | <pre>case 0x5c:<br>&nbsp;&nbsp;&nbsp;&nbsp;if (ecsAndHigher())<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;blitter.setBltSizeV(value);</pre> |
| **Copperline** | [`src/bus/custom_regs.rs:787-792`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L787-L792) | <pre>0x05C =&gt; {<br>&nbsp;&nbsp;&nbsp;&nbsp;if !self.blitter_ecs_registers_enabled() {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return false;<br>&nbsp;&nbsp;&nbsp;&nbsp;}</pre> |

**All four gate `BLTSIZV` on ECS. NanoMig alone does not.** This is one of only
two findings in chapters 3 and 6 where the manual, vAmiga and WinUAE all stand
on the same side with NanoMig the outlier, which makes it the strongest of the
three blitter entries.

---

## Suggested change

The sibling two lines up already shows the shape:

```verilog
  	else if (reg_address_in[8:1]==BLTSIZV[8:1] && ecs) // ECS BLTSIZV register
  		height[14:0] <= data_in[14:0];
```

One `&& ecs`, matching `BLTCON0L` and `BLTSIZH` in the same file, the appendix
page that gives all three the same status, and all four reference
implementations. This is the least contentious change in the register.

---

## What holds the finding up

**The control is the point.** `BLTCON0L-GATED-BY-ECS` is **green** on the same
fixture, so this fixture's `ecs` input demonstrably changes behaviour elsewhere
in the same module. The red is `BLTSIZV` ignoring it, not a dead `ecs` signal.
