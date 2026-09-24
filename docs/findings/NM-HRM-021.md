# NM-HRM-021 — DOFF (BLTCON1 bit 7) has no observable effect [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch6-blitter-ecs`](../../hrm/ch6-blitter-ecs) |
| **Chapter** | 6 — Blitter · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | two implement it, two do not — the emulators cannot break this tie |

---

## What the HRM says

**Appendix C, printed p. 309**, among the ECS register changes:

> Bit 7 (DOFF) of the BLTCON1 register, when set, disables the output of the
> Blitter hardware on channel D.

The page states no line-mode condition. An earlier version of this entry quoted
it as "disable D output in line mode": those words are the source comment of
Denise's implementation below, not the manual's.

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
| **MiSTer** | [`rtl/agnus_blitter.v:283`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L283) | descendant — identical to NanoMig: `bltcon1[11:0] <= data_in[11:0];` captures bits 11-0 only; bit 7 is absent from the module in MiSTer too |
| **TC64** | [`rtl/minimig/agnus_blitter.v:282`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L282) | descendant — the same line, byte for byte (TC64's fourteen changed lines here are whitespace only, far from the `BLTCON1` decode). DOFF is unbuilt in all three; adding it diverges from neither |

**Two of the four independent implementations implement it, two do not.** This is
the only place in the chapter where the third-witness reading cannot break a
tie, so it says nothing in either direction and the manual decides alone — which
it does clearly. DOFF is unbuilt in MiSTer and TC64 too, exactly as it is in
NanoMig; that is inheritance, not a third and fourth vote either way.

*Citation note: the register previously gave this as `blitter.cpp:297` without a
path. There are two files of that name in the source set; the line is Denise's
`agnus/blitter.cpp:297`, and WinUAE's `blitter.cpp:297` is an unrelated closing
brace.*


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
// ECS: DOFF disables the D output in line mode (appendix C, printed p.309)
wire doff = ecs & bltcon1[7];
```

then suppress the D write when `doff` is set. Denise's comment adds "in line
mode"; p.309 does not. `BLTCON1` bit 7 has to
be captured first — the register decode currently drops it.

Being an unbuilt feature, this is an addition rather than a correction, and it is
the kind that needs a maintainer's judgement about whether anything depends on
it. Two of four emulators live without it.

---

## What holds the finding up

The package's other `ecs`-gated assertions pass on the same fixture, so the
fixture's `ecs` input works. The red is the absence of any decode for the bit,
which is also visible by reading the module: the bit is simply never referenced.
