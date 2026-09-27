# NM-HRM-021 — DOFF (BLTCON1 bit 7) has no observable effect [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch6-blitter-ecs`](../../hrm/ch6-blitter-ecs) |
| **Chapter** | 6 — Blitter · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red: `DOFF-NO-OBSERVABLE-EFFECT` in `hrm/ch6-blitter-ecs` |
| **Verdict of the four** | three of four implement it, each gated on ECS Agnus; vAmiga does not |

---

## What the HRM says

**Appendix C, printed p. 309**, among the ECS register changes:

> Bit 7 (DOFF) of the BLTCON1 register, when set, disables the output of the
> Blitter hardware on channel D.

The page states no line-mode condition.

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
| **WinUAE** | [`blitter.cpp:1541`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1541), [`:470-471`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L470-L471) | <pre>blit_dof = ecs_agnus && (bltcon1_next & BLTDOFF);</pre> implements it, ECS-gated: every D write then stores the bus latch instead of the blitter's value |
| **vAmiga** | — | absent, a constant only |
| **Denise** | [`emulation/libami/agnus/blitter.cpp:297`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitter.cpp#L297) | <pre>doff = agnus.ecsAndHigher() && (bltcon1 & 0x80);</pre> implements **and** ECS-gates it |
| **Copperline** | [`src/chipset/blitter.rs:1663`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1663), [`src/bus/custom_regs.rs:633-641`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L633-L641) | <pre>write_d: use_d && con1 & BLTCON1_DOFF == 0,</pre> implements it, ECS-gated: no D store in an area blit while it is set, and the bit is masked off on an OCS Agnus |
| **MiSTer** | [`rtl/agnus_blitter.v:283`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L283) | descendant — identical to NanoMig: `bltcon1[11:0] <= data_in[11:0];` stores bit 7 with the rest, and nothing in the module reads it in MiSTer either |
| **TC64** | [`rtl/minimig/agnus_blitter.v:282`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L282) | descendant — the same line, byte for byte (TC64's fourteen changed lines here are whitespace only, far from the `BLTCON1` decode). DOFF is unbuilt in all three; adding it diverges from neither |

**Three of the four independent implementations implement it, each gated on
ECS Agnus; vAmiga alone does not.** WinUAE and Denise still perform the D write
but store the bus value in place of the blitter's; Copperline suppresses the
store. The manual decides, and it does clearly. DOFF is unbuilt in MiSTer and
TC64 too, exactly as it is in NanoMig; that is inheritance, not a vote either
way.

*Citation note: there are two files named `blitter.cpp` in the source set; the
line is Denise's `agnus/blitter.cpp:297`, and WinUAE's `blitter.cpp:297` is an
unrelated closing brace.*


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988; Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

---

## Suggested change

Denise's line is the whole of it, and it fits this RTL almost verbatim:

```verilog
// ECS: DOFF disables the D output (appendix C, printed p.309)
wire doff = ecs & bltcon1[7];
```

then suppress the D write when `doff` is set. `BLTCON1` bit 7 is already stored
by the register decode (`bltcon1[11:0] <= data_in[11:0]`); nothing reads it.

Being an unbuilt feature, this is an addition rather than a correction, and it is
the kind that needs a maintainer's judgement about whether anything depends on
it. One of four emulators lives without it.

---

## What holds the finding up

The package's `BLTCON0L-GATED-BY-ECS` and `BLTSIZH-GATED-BY-ECS` pass on the
same fixture, so the fixture's `ecs` input works. The red is the absence of any decode for the bit,
which is also visible by reading the module: the bit is simply never referenced.
