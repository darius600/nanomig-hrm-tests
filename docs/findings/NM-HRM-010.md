# NM-HRM-010 — the NTSC line is 227 colour clocks every time, where the manual alternates 228 and 227 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. The RTL names the simplification in its own comments, so whether NanoMig wants the NTSC half clock at all is a project question before it is a code one. |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L86) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86) |
| **Report to** | **NanoMig and MiSTer-devel/Minimig-AGA_MiSTer**, as a question rather than a defect: both lines, and TC64, carry the simplification with the same comment, so there is no fix in either to port |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) · `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)`, claim `C-scan.c02`, and `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC bundled with line count)`, claim `opus:7-appC-beam.c2` (p. 304) |
| **Chapter** | Appendix C — ECS, printed p. 304; 2 — Coprocessor, printed p. 23; 6 — Blitter, printed p. 194 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red, both NTSC. The control that writes HTOTAL and measures the line it produces is green in the same run; the PAL pair beside them is [NM-HRM-068](NM-HRM-068.md)'s |
| **Verdict of the four** | **unanimous against NanoMig**: WinUAE, vAmiga, Denise and Copperline all make every other NTSC line 228 colour clocks. NanoMig toggles the bit that reports a long line and keeps every line at 227 |
| **See also** | [NM-HRM-068](NM-HRM-068.md) — the PAL row of the same sentence, where p. 23 sides with the RTL; [NM-HRM-033](NM-HRM-033.md) — the condition on the same toggle; [NM-AA-005](NM-AA-005.md) — the AA specification's HTOTAL entry, the same missing long line under a programmed line length |

---

## What the HRM says

**Appendix C, printed p. 304**, lists the scan rates the V36 graphics library
supports:

> NTSC (525 lines, 227.5 colorclocks per scan line)

**Chapter 2, printed p. 23**, says how a line of 227.5 colour clocks is made:

> All lines are not the same length in NTSC. Every other line is a long line
> (228 color clocks, 0-$E3), with the others being 227 color clocks long. In
> PAL, they are all 227 long. The display sees all these lines as 227 1/2
> color clocks long, while the copper sees alternating long and short lines.

**Chapter 6, printed p. 194**, gives the same figure for the line as a whole:

> During a horizontal scan line (about 63 microseconds), there are 227.5
> "color clocks", or memory access cycles.

**For NTSC the pages agree.** No line lasts half a colour clock; two
consecutive lines last 228 and 227, and the average is p. 304's 227.5.

---

## What NanoMig does

**Every line is 227 colour clocks, in NTSC as in PAL.** The hardwired length
is one constant —
[`src/minimig-aga/agnus_beamcounter.v#L86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L86):

```verilog
parameter HTOTAL_VAL      = 8'd227 - 8'd1;   // line length of 227 CCKs in PAL mode (NTSC line length of 227.5 CCKs is not supported)
```

and the line ends where the horizontal counter reaches it, with no long-line
term —
[`#L250`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L250):

```verilog
if (hpos[8:0]=={htotal[8:1],1'b0})
```

The long-line bit exists, alternates in NTSC and is reported in VPOSR
(`:110`), but nothing reads it for the line length —
[`#L291-L300`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L291-L300):

```verilog
//long line signal (not used, only for better NTSC compatibility)
...
if (pal || (loldis && varbeamen))
    long_line <= 1'b0;
else if (!(loldis && varbeamen))
    long_line <= ~long_line;
```

### Measured

[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking)
drives the colour-clock input itself and counts its periods between
consecutive end-of-line pulses, over four consecutive lines of the hardwired
default:

| standard | four lines | average | p. 304 | p. 23 |
|---|---|---:|---:|---:|
| NTSC | 227, 227, 227, 227 | 227.0 | 227.5 | 228 and 227 alternating |

`COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)` is red, and so is the bundled form,
whose line-count half — 525 lines over a long and a short field — matches. Two
of the package's `28 passed, 5 failed`.

---

## What the others say

The question put to each: how long is an NTSC line?

| | code | |
|---|---|---|
| **WinUAE** | [`include/custom.h:116-117`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L116-L117), [`custom.cpp:2054`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2054), [`:2328-2333`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2328-L2333), [`:6771`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6771) | <pre>maxhpos = maxhpos_short + lol;<br>maxhpos_lol = lol;<br>maxhpos_long = linetoggle ? maxhpos_short + 1 : -1;</pre> on a short line of `#define MAXHPOS_NTSC 227`, with `linetoggle = !(new_beamcon0 & BEAMCON0_PAL) && !(new_beamcon0 & BEAMCON0_LOLDIS);` and BEAMCON0's reset value `currprefs.ntscmode ? 0x00 : BEAMCON0_PAL`. **NTSC lines alternate 228 and 227** |
| **vAmiga** | [`Core/Components/Agnus/Beam.h:80`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L80), [`Beam.cpp:263-264`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.cpp#L263-L264), [`:294-298`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.cpp#L294-L298) | <pre>isize hCnt() const { return lol ? 228 : 227; }</pre> with the toggle at every end of line, `if (lolToggle) lol = !lol;`, and `lolToggle = true;` when the machine is switched to NTSC. **NTSC lines alternate 228 and 227** |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:1030`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1030), [`:217-219`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L217-L219), [`events.cpp:479-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L479-L480) | <pre>fps = (double)frequency() / (227.5 * 263.0); // start with lol toggling</pre> NTSC `227.5 × 263`, PAL `227.0 × 313`. The next line is scheduled as `updateEvent<EVENT_HTOTAL>((beamCon & VARBEAMEN) ? (hTotal + lol) : (0xe2 + lol));` after `if (lolToggle) lol ^= 1;`, with `lolToggle = ntsc;` at reset. **NTSC lines alternate 228 and 227** |
| **Copperline** | [`src/chipset/agnus.rs:9`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L9), [`:18`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L18), [`:841-859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L841-L859) | <pre>if matches!(self.video_standard, VideoStandard::Ntsc) && lol {<br>&nbsp;&nbsp;&nbsp;&nbsp;NTSC_LONG_COLORCLOCKS_PER_LINE<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;COLORCLOCKS_PER_LINE<br>}</pre> with `pub const NTSC_LONG_COLORCLOCKS_PER_LINE: u32 = 228;`, and `long_line_toggles()` true for NTSC unless the beam is programmed or LOLDIS is set. **NTSC lines alternate 228 and 227** |
| **MiSTer** | [`rtl/agnus_beamcounter.v:86`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86) | descendant — identical to NanoMig: `parameter HTOTAL_VAL = 8'd227 - 8'd1;` with the same "NTSC line length of 227.5 CCKs is not supported" comment |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:102`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L102) | descendant — the same simplification and the same comment, the constant only wider (`9'd227 - 9'd1` versus `8'd227 - 8'd1`) inside a file whose port list and internals are otherwise substantially rewritten (643 of 461 lines differ). Both forks decline the half clock exactly as NanoMig does; there is no line to diverge from |

**All four make the half clock the way p. 23 describes it**: whole lines, one
colour clock longer every other time, so that two lines average 227.5. Denise's
comment names the mechanism, *lol toggling*, long-line toggling. MiSTer and
TC64 decline the half clock exactly as NanoMig does — as descendants that is
not a third reading, only the same simplification inherited unchanged.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989**, PDF page 25 — agrees with the manual:

  > "During scan of a horizon tal line, there are typically 227.5 memory access cycles, each of which is approximately 280 nanoseconds in duration."
  <!-- corpus: patent-us4874164-agnus-denise/p025.txt -->

  Same figure as the manual, per horizontal line. It says 'typically' and names no video standard. The same sentence is on patent-us5103499-copper p025.

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 11 — agrees with the manual:

  > "strobe shown above is used during the second refresh time slot of every other line, to identify lines with long counts (228)."
  <!-- corpus: denise-spec-252126/p011.txt -->

  The ECS Denise specification says every other line is a long line of 228 counts. This is the alternation NanoMig does not generate.

Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988.

---

## Suggested change

**Let the long-line bit lengthen the line it marks**, as WinUAE
(`maxhpos_short + lol`) and Denise (`0xe2 + lol`) do: end a line one colour
clock later when `long_line` is set, so that in NTSC consecutive lines last 228
and 227. The bit already alternates in the right standard, so the change is to
the end-of-line comparison at `:250`, not to the toggle.

That comparison is the one every other horizontal timing in the module hangs
off, and the RTL's own comments call the half clock unsupported rather than
broken. Decide it together with [NM-HRM-033](NM-HRM-033.md), which is about
when the same bit may toggle, and [NM-AA-005](NM-AA-005.md), which asks the
same question under a programmed line length.

---

## What holds the finding up

- **The comparator reads a written line length back.** In the same package, a
  control writes HTOTAL = 100 under VARBEAMEN and measures 101 colour clocks
  per line, the same written-plus-one relation the hardwired default shows as
  226 and 227. So the count moves with the line, and an alternating line would
  show.
- **No conversion factor comes from the RTL.** The harness drives the
  colour-clock input itself and counts its periods between two end-of-line
  pulses; the expected value is p. 304's own number, compared as an average
  because a single line cannot end on half a clock.
- **The RTL says the same thing in its own words**, twice: *"NTSC line length
  of 227.5 CCKs is not supported"* at `:86`, and *"not used, only for better
  NTSC compatibility"* on the long-line bit.

---

## For the specialists

1. **Is the half clock visible above the beam counter?** p. 23 says the Copper
   sees the alternation: on a long line the horizontal position reaches $E3.
   On NanoMig it never passes $E2, so a Copper list or a program that waits on
   the last horizontal position of an NTSC line sees a different line from the
   one the page describes.
2. **Does the long-line bit mislead?** VPOSR's bit 7 alternates line by line
   in NTSC and reports long lines that are not longer. A program that reads it
   is told something the beam does not do.
