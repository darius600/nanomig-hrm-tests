# NM-AA-004 — VPOSR reports chip identifier $23, where the AA list gives an Alice $22

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. The AA specification lists the identifier every Agnus reports, and this core answers one bit more than the value it gives for the part it describes. Whether real Alice silicon answers `$22` or `$23` is an Amiga question, not one this repository can settle. |
| **Module** | `src/minimig-aga/agnus_beamcounter.v:110` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L110) at the pinned revision |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** if the line is upstream's; the identifier word is one expression and a change to it is one edit |
| **Test** | [`aa/aa-beam-position-words`](../../aa/aa-beam-position-words) · four graded assertions, three green and this one red. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 29, the VPOSR entry |
| **Assertions** | 4 graded; `make mutate` turns the three green ones red and this one green, which is the established shape for a red baseline |
| **Verdict of the four** | **three of three against the document.** Every one of the four that models an identifier at all returns **$23** for AGA — the value this RTL returns — and none returns the document's $22. Denise casts no vote: its AGA term lands in the wrong bit position. See [What the others say](#what-the-others-say) |
| **See also** | [NM-HRM-034](NM-HRM-034.md), which raises the same word against the Hardware Reference Manual — where the field is printed as unused rather than as an identifier |

## What the AA specification says

Page 29 prints VPOSR as a sixteen-row bit table. Bit 15 is LOF, bit 7 is LOL,
bits 2 to 0 are V10 to V8, and bits 14 to 8 are the seven-bit chip identifier:

```
                    I0-I6 chip identification:
                      8361(regular) or 8370(fat) (agnus-ntsc)  = 10
                      8367(pal) or 8371(fat-pal) (agnus-pal)    = 00
                      8372(fat-hr) (agnushr),thru rev. 4      = 20 PAL,30 NTSC
                      8372(fat-hr) (agnushr),rev. 5           = 21 PAL,31 NTSC
                      8374(alice)                             = 22 PAL,32 NTSC
```

`8374` is Alice — the Agnus of the chip set this document specifies. The list
is a progression: `20`, `21`, `22` for successive revisions, with `$10` added
for NTSC. The `8374(alice)` line is tagged `[3/3] ocr+guide+model` in the
corpus: all three independent readings produced it.

## What NanoMig does

`agnus_beamcounter.v:110` builds the whole word in one expression:

```verilog
data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
```

Bits 14 to 8 are therefore `{1'b0, ecs, ntsc, 2'b00, aga, aga}`. With `aga` and
`ecs` high and `ntsc` low — an Alice on PAL — that is `0100011`, **$23**.

The document's Alice is `0100010`, **$22**. The two differ in one bit: I0,
which is VPOSR bit 8. The RTL takes it from `aga`, the same input it already
uses for I1; the document's Alice leaves it clear.

## The measurement

```
MEASURED: VPOSR bits 14-8 read 0x23 with PAL and 0x33 with NTSC;
          the AA list gives an Alice 0x22 and 0x32
FAIL: VPOSR-REPORTS-THE-CHIP-IDENTIFIER
```

Both halves move together: NTSC adds `$10` in the RTL exactly as the
document's list does, so the `ntsc` term is right and only the duplicated
`aga` is in question.

Three further assertions in the same package are green, which is what makes
this one a reading of the identifier rather than of a broken fixture: a line
number written through VHPOSW comes back in VHPOSR's high byte, the low byte
follows the beam across the line, and the vertical counter's three highest
bits come back at the bottom of VPOSR.

## What the others say

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:2399`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2399) | `csbit |= aga_mode ? 0x2300 : 0;` — bits 14-8 are **$23** for AGA, this RTL's value. The NTSC term is added separately (`csbit |= 0x1000`), so PAL and NTSC move by the document's `+$10` exactly as they do here. A configured `cs_agnusrev` replaces the whole field (`:2396`), so the $23 is the default rather than a constant |
| **vAmiga** | [`Agnus.cpp:272`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L272) | `case AgnusRevision::AGA: return 0x2300;` — **$23**. ECS returns `0x2000` for both the 1 MB and the 2 MB part, one of them marked `TODO: CHECK ON REAL MACHINE`, and there is no NTSC term at all: vAmiga never produces the list's $32 or $33 |
| **Denise** | [`agnus.cpp:745`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L745) | **no vote.** `... \| (1 << 13) \| (aga() ? 3 : 0)` puts the ECS bit at I5 and the AGA term at bits 1:0 — where V9 and V8 are — instead of at I1:I0. Bits 14-8 therefore read $20, the ECS value, in AGA mode as well. The shape of the other terms says a shift is missing; that is Denise's question, not this repository's |
| **Copperline** | [`agnus.rs:98`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L98) | `(Self::AgaAlice, VideoStandard::Pal) => 0x23` — **$23**, NTSC $33, and the comment above it cites *"HRM table: 8374 (Alice) rev 3 thru rev 4 = $23 PAL / $33 NTSC"* |
| **MiSTer** | [`rtl/agnus_beamcounter.v:116`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L116) | descendant — the identical `{2{aga}}` expression, same duplicated bit |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:132`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L132) | descendant — the identical expression |

**This does not settle what Alice answers, and it does move the weight.** Three
independent implementations model the identifier and all three give $23. Two of
them also record a doubt about the neighbouring row of the same list: WinUAE
disables its `$21` case with the comment *"apparently 8372 (Fat-hr) (agnushr),
rev 5 does not exist"* (`custom.cpp:2402`), and Copperline's comment says the
same row is *"a part that apparently never shipped"*.

**Copperline's citation is to a table this corpus does not carry.** The 3rd
edition's own list, on `appC-playfield/p299`, stops at `8368 (hr) or 8372
(fat-hr) = 20 for PAL, 30 for NTSC` — no Alice row at all. So the $23 the three
share comes from somewhere later than both documents this project reads, which
fits the simplest reading of the split: the AA specification is marked
PRELIMINARY and dated 06/07/91, its list is a progression `20, 21, 22`, and the
shipped Alice appears to have been one revision further on.

**It stays a red and the assertion stays as written.** The oracle here is the
printed page, and the page says $22; what three emulators do is evidence for the
reader who decides about the RTL, not a second oracle. That decision is exactly
what this finding asks for.

## Suggested change

If Alice is to report what this document lists, the two duplicated bits become
one:

```verilog
-    data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
+    data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,aga,1'b0,long_line,4'b0000,vpos[10:8]};
```

That is one edit and it changes one bit of one read-only register. It is
**not** proposed as obviously correct: software that probes VPOSR to tell an
AGA machine from an ECS one may be reading either bit, and which value real
silicon returns is the thing to establish first.

## What holds the finding up

The document's line is corroborated by three independent readings. The RTL's
expression is one line and says plainly what it builds. The package's other
three assertions are green, and its mutation run turns them red and this one
green, so the comparison is reading the register rather than a constant.

## For the specialists

Two questions, in order:

1. **What does an 8374 actually return in VPOSR bits 14-8?** If it is `$23`,
   this document's list is wrong and the finding moves to the printed page.
2. **Does any software depend on the bit?** The identifier's whole purpose is
   to let a program tell the parts apart, so a bit that is set where the
   published list says clear is exactly the kind of thing a probe would trip
   over.
