# NM-HRM-021 — DOFF (BLTCON1 bit 7) has no observable effect

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v) |
| **Upstream** | **MiSTer** `rtl/agnus_blitter.v:283` — identical to NanoMig: `bltcon1[11:0] <= data_in[11:0];` captures bits 11-0 only; bit 7 is absent from the module in MiSTer too. **TC64** `rtl/minimig/agnus_blitter.v:282` — the same line, byte for byte (TC64's fourteen changed lines here are whitespace only, far from the `BLTCON1` decode). DOFF is unbuilt in all three; adding it diverges from neither |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/agnus_blitter_ecs`](../../hrm/agnus_blitter_ecs) |
| **Chapter** | 6 — Blitter · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | two implement it, two do not — the emulators cannot break this tie |

---

## What the HRM says

**Appendix C, printed p. 309** lists `DOFF` among the five ECS blitter changes:

> disable D output in line mode

---

## What NanoMig does

Nothing. **`bltcon1[7]` does not occur in `agnus_blitter.v` at all.** The bits
that module reads from `BLTCON1` are 15–12, 4, 3, 2, 1 and 0.

This is an **unbuilt feature rather than a wrong one**, which matters for how it
should be handled: there is no incorrect logic to correct.

### Measured

The same D write appears with `BLTCON1` bit 7 clear and with it set. Setting the
bit changes nothing that can be observed.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`blitter.cpp:1176-1178`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1176-L1178), [`:1541`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1541) | <pre>blit_dof = ecs_agnus && (bltcon1_next & BLTDOFF);</pre> implements it, ECS-gated |
| **vAmiga** | — | absent, a constant only |
| **Denise** | [`emulation/libami/agnus/blitter.cpp:297`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitter.cpp#L297) | <pre>doff = agnus.ecsAndHigher() && (bltcon1 & 0x80);</pre> implements **and** ECS-gates it |
| **Copperline** | — | no decode found for `BLTCON1` bit 7 |

**Two of four implement it, two do not.** This is the only place in the chapter
where the third-witness reading cannot break a tie, so it says nothing in either
direction and the manual decides alone — which it does clearly.

*Citation note: the register previously gave this as `blitter.cpp:297` without a
path. There are two files of that name in the source set; the line is Denise's
`agnus/blitter.cpp:297`, and WinUAE's `blitter.cpp:297` is an unrelated closing
brace.*

---

## Suggested change

Denise's line is the whole of it, and it fits this RTL almost verbatim:

```verilog
// ECS: DOFF disables the D output in line mode (appendix C, printed p.309)
wire doff = ecs & bltcon1[7];
```

then suppress the D write in line mode when `doff` is set. `BLTCON1` bit 7 has to
be captured first — the register decode currently drops it.

Being an unbuilt feature, this is an addition rather than a correction, and it is
the kind that needs a maintainer's judgement about whether anything depends on
it. Two of four emulators live without it.

---

## What holds the finding up

The package's other `ecs`-gated assertions pass on the same fixture, so the
fixture's `ecs` input works. The red is the absence of any decode for the bit,
which is also visible by reading the module: the bit is simply never referenced.
