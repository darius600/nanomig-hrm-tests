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
| **Verdict of the four** | **not consulted.** WinUAE, vAmiga, Denise and Copperline were not read for this claim. What each returns in VPOSR bits 14-8 for an AGA configuration is exactly the evidence that would settle it |
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
