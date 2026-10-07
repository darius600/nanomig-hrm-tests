# NM-HRM-021 — DOFF (BLTCON1 bit 7) has no observable effect [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should NanoMig's blitter stop writing channel D when `BLTCON1` bit 7 (DOFF) is set on ECS Agnus, as Appendix C p.309 prints, given that nothing in `agnus_blitter.v` reads that bit? Closes when an Amiga specialist either accepts the addition under *Decision* or records that NanoMig need not build DOFF. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer: its `rtl/agnus_blitter.v` stores the bit and never reads it, exactly as NanoMig's does; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Scope** | ECS Agnus (`ecs` input true); measured on `agnus_blitter` as a one-word area blit using channels A, C and D, with only `BLTCON1` bit 7 varied. Line mode is not measured; no chip revision is named |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 6 — Blitter, Appendix C, printed p. 309 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · the test's `DOFF-NO-OBSERVABLE-EFFECT` carries claims `6-appc-modifications.c4`, `.c5`, `.c6`, `.c7`, `.c9`, `ch6-appc-ecs.c4`, `ch6-appc-ecs.c6`, `ch6-appc-map.bltcon1`, `6-appa-control.c18`, `ch6-appa-bltcon0.area.b07.con1`, `6-appa-registers.c20`, `ch6-appa-bltcon0.c6`, `ch6-blockdiag.c10`, `6-blitter-block-diagram.c11` (the test names pp. 198, 199, 267, 309 and 310 for them); the controls `BLTCON0L-GATED-BY-ECS` and `BLTSIZH-GATED-BY-ECS` carry their own claim ids in the test and are not graded here |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v); inherited, the same code in MiSTer and TC64 |
| **Test** | [`hrm/ch6-blitter-ecs`](../../hrm/ch6-blitter-ecs): `make -C hrm/ch6-blitter-ecs test`; the expected red exits nonzero |
| **Assertions** | 1 red: `DOFF-NO-OBSERVABLE-EFFECT` — expected no D write with DOFF set, observed the same D write as with DOFF clear (`0x2222`). The package totals 12 passed, 4 failed; the other three reds belong to [NM-HRM-020](NM-HRM-020.md) and [NM-HRM-022](NM-HRM-022.md) |
| **Verdict of the four** | three of four implement it, each gated on ECS Agnus; vAmiga does not |
| **Effort** | S: one decode of `bltcon1[7]` gated on `ecs`, and the D write gate in `agnus_blitter.v` |
| **See also** | [NM-HRM-020](NM-HRM-020.md), [NM-HRM-022](NM-HRM-022.md) |

---

## Source requirement

**Appendix C, printed p. 309**, among the ECS register changes:

> Bit 7 (DOFF) of the BLTCON1 register, when set, disables the output of the
> Blitter hardware on channel D.

The page states no line-mode condition.

## RTL and measurement

**`bltcon1[7]` does not occur in `agnus_blitter.v` at all.** The bits the module
reads from `BLTCON1` are 15–12, 4, 3, 2, 1 and 0; bit 7 is stored with the rest
by `bltcon1[11:0] <= data_in[11:0]` and nothing reads it. The D write is gated by
`used` and `chsel` only. This is an unbuilt feature rather than a wrong one:
there is no incorrect logic to correct.

The same one-word D write is run with `BLTCON1` bit 7 clear and with it set.
Setting the bit changes nothing that can be observed. The package's
`BLTCON0L-GATED-BY-ECS` and `BLTSIZH-GATED-BY-ECS` pass on the same fixture, so
its `ecs` input works.

## Risks and unknowns

- **An addition, not a correction.** It needs a maintainer's judgement about
  whether anything depends on DOFF being absent.
- **The page does not say what "disables the output" does to the write.** WinUAE
  and Denise still perform the D write but store the bus value in place of the
  blitter's; Copperline suppresses the store. The suggested change takes the
  second reading.
- **Line mode is not measured**, and the page states no line-mode condition.
- **No hardware measurement** of DOFF is recorded here; vAmiga does not implement it.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`blitter.cpp:1541`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1541), [`:470-471`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L470-L471) | <pre>blit_dof = ecs_agnus && (bltcon1_next & BLTDOFF);</pre> implements it, ECS-gated: every D write then stores the bus latch instead of the blitter's value |
| **vAmiga** | — | absent, a constant only |
| **Denise** | [`emulation/libami/agnus/blitter.cpp:297`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitter.cpp#L297) | <pre>doff = agnus.ecsAndHigher() && (bltcon1 & 0x80);</pre> implements **and** ECS-gates it |
| **Copperline** | [`src/chipset/blitter.rs:1663`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1663), [`src/bus/custom_regs.rs:633-641`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L633-L641) | <pre>write_d: use_d && con1 & BLTCON1_DOFF == 0,</pre> implements it, ECS-gated: no D store in an area blit while it is set, and the bit is masked off on an OCS Agnus |
| **MiSTer** | [`rtl/agnus_blitter.v:283`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L283) | descendant — identical to NanoMig: `bltcon1[11:0] <= data_in[11:0];` stores bit 7 with the rest, and nothing in the module reads it |
| **TC64** | [`rtl/minimig/agnus_blitter.v:282`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L282) | descendant — the same line, byte for byte (TC64's fourteen changed lines here are whitespace only, far from the `BLTCON1` decode); adding DOFF diverges from neither |

Denise's file is `agnus/blitter.cpp:297`; WinUAE's `blitter.cpp:297` is an
unrelated closing brace.

## Decision

Denise's line is the whole of it, and it fits this RTL almost verbatim:

```verilog
// ECS: DOFF disables the D output (appendix C, printed p.309)
wire doff = ecs & bltcon1[7];
```

then suppress the D write when `doff` is set; the register decode already stores
the bit. The manual decides, and it does clearly: three of the four emulators
implement DOFF, each gated on ECS Agnus. The finding closes as stated in *What is
asked of a reader*.
