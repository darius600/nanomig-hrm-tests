# NM-HRM-068 — p. 304 gives a PAL line 227.5 colour clocks, p. 23 gives it 227, and every implementation makes it 227

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. Question: does any PAL configuration alternate its line lengths, so that p. 304's "227.5 colorclocks per scan line" on the PAL row is right and NanoMig's constant 227 is not? Closes when an Amiga specialist records p. 304's PAL row as a misprint for p. 23's 227, or confirms that a PAL configuration alternates line lengths, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendix C of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | NanoMig's `agnus_beamcounter` on its hardwired default (VARBEAMEN=0), PAL, the line length averaged over four consecutive lines; chip revision unknown; PAL hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C, printed p. 304, the V36 graphics library scan-rate list; 2 — Coprocessor, printed p. 23; 6 — Blitter, printed p. 194, quoted for context · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `COLOUR-CLOCKS-PER-LINE-227POINT5 (PAL)`: `opus:C-scan.c04`, p. 304; `COLOUR-CLOCKS-PER-LINE-227POINT5 (PAL bundled with line count)`: `opus:7-appC-beam.c3`, p. 304. The green control carries no claim id, and neither PAL assertion carries one for p. 23 or p. 194 |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L86) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86); TC64 has the same constant one bit wider; inherited |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking): `make -C hrm/ch3-beam-counters-and-blanking test`; an expected red exits nonzero |
| **Assertions** | 2 red, both PAL. `COLOUR-CLOCKS-PER-LINE-227POINT5 (PAL)`: expected an average of 227.5, observed 227.0 over four lines of 227. `COLOUR-CLOCKS-PER-LINE-227POINT5 (PAL bundled with line count)`: expected 625 lines and 227.5, observed 625 and 227.0; its line-count half matches. The control that writes HTOTAL and measures the line it produces is green. The package reports `28 passed, 5 failed`: the other three reds are the NTSC pair, [NM-HRM-010](NM-HRM-010.md)'s, and [NM-HRM-011](NM-HRM-011.md)'s |
| **Verdict of the four** | **unanimous with NanoMig and with p. 23**: WinUAE, vAmiga, Denise and Copperline all end every PAL line at 227 colour clocks and alternate long and short lines only in NTSC. Hardware: not measured |
| **Effort** | none — no RTL change, at most a correction to p. 304's PAL row |
| **See also** | [NM-HRM-010](NM-HRM-010.md) — the NTSC half of the same sentence, where the manual's two pages agree and NanoMig does not |

---

## Source requirement

**Appendix C, printed p. 304**, lists the scan rates the V36 graphics library
supports:

> NTSC (525 lines, 227.5 colorclocks per scan line)
> PAL (625 lines, 227.5 colorclocks per scan line)

**Chapter 2, printed p. 23**, says what the lines are:

> All lines are not the same length in NTSC. Every other line is a long line
> (228 color clocks, 0-$E3), with the others being 227 color clocks long. In
> PAL, they are all 227 long. The display sees all these lines as 227 1/2
> color clocks long, while the copper sees alternating long and short lines.

**The two pages disagree about PAL.** For NTSC they say the same thing twice:
alternating lines of 228 and 227 average 227.5. For PAL, p. 23 says every line
is 227, which averages 227; p. 304 prints 227.5 on the PAL row as on the NTSC
row above it. The last sentence of p. 23 does not reconcile them: *"the copper
sees alternating long and short lines"* describes the NTSC lines, and p. 23 has
just said PAL has no long ones. The finding is filed against p. 304 because p. 23
gives the reason for the half clock and p. 304 repeats the NTSC figure on the
PAL row.

**Chapter 6, printed p. 194**, gives the figure once more and names no video
standard:

> During a horizontal scan line (about 63 microseconds), there are 227.5
> "color clocks", or memory access cycles.

## RTL and measurement

**Every PAL line is 227 colour clocks.** The hardwired line length is one
constant for both standards —
[`src/minimig-aga/agnus_beamcounter.v#L86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L86):

```verilog
parameter HTOTAL_VAL      = 8'd227 - 8'd1;   // line length of 227 CCKs in PAL mode (NTSC line length of 227.5 CCKs is not supported)
```

and in PAL the long-line bit is held clear —
[`#L291-L300`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L291-L300):

```verilog
if (pal || (loldis && varbeamen))
    long_line <= 1'b0;
```

That bit reaches only VPOSR's LOL bit
([`#L110`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L110));
the line ends where the counter meets the constant
([`#L250`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L250)),
in either standard.

[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking)
drives the colour-clock input itself and counts its periods between
consecutive end-of-line pulses, over four consecutive lines of the hardwired
default:

| standard | four lines | average | p. 304 | p. 23 |
|---|---|---:|---:|---:|
| PAL | 227, 227, 227, 227 | 227.0 | 227.5 | 227 |

`COLOUR-CLOCKS-PER-LINE-227POINT5 (PAL)` is red, and so is its bundled form.

## Risks and unknowns

- **PAL hardware is not measured.** Only a PAL machine, or the PAL Agnus's
  documentation, says whether any PAL configuration alternates line lengths.
- **Could p. 304 mean something other than the line?** It introduces the list as
  *"scan rates supported in the V36 graphics.library"*, a software statement,
  beside a VGA row of 114.0. A figure the library uses for its own timing
  calculations and the length of a line on the wire could differ; the page does
  not say which it gives.
- **The line count on the same row is right.** 625 lines over a long and a short
  field is measured green in the same run, so the row is not wrong as a whole.

## Independent evidence

The question put to each: how long is a PAL line?

| | code | |
|---|---|---|
| **WinUAE** | [`include/custom.h:116-117`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L116-L117), [`custom.cpp:10970-10975`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10970-L10975), [`:2328-2333`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2328-L2333) | <pre>#define MAXHPOS_PAL 227<br>#define MAXHPOS_NTSC 227</pre> The line in use is `maxhpos = maxhpos_short + lol;`, and at the start of each line `lol` flips only without the PAL bit, `if (!(new_beamcon0 & BEAMCON0_PAL) && !(new_beamcon0 & BEAMCON0_LOLDIS)) {` / `lol = lol ? false : true;`, and is cleared otherwise. **Every PAL line is 227** |
| **vAmiga** | [`Core/Components/Agnus/Beam.h:80`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L80), [`Beam.cpp:283-301`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.cpp#L283-L301) | <pre>isize hCnt() const { return lol ? 228 : 227; }</pre> and `switchMode` sets `lolToggle = false;` for PAL and `lolToggle = true;` for NTSC. **Every PAL line is 227** |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:217-219`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L217-L219), [`:1028-1032`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1028-L1032), [`events.cpp:479-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L479-L480) | <pre>if (lolToggle) lol ^= 1;<br>updateEvent&lt;EVENT_HTOTAL&gt;((beamCon & VARBEAMEN) ? (hTotal + lol) : (0xe2 + lol));</pre> with `lolToggle = ntsc;` at reset, and the frame rate computed as `fps = (double)frequency() / (227.0 * 313.0);` for PAL against `227.5 * 263.0` for NTSC. **Every PAL line is 227** |
| **Copperline** | [`src/chipset/agnus.rs:9`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L9), [`:18`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L18), [`:841-859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L841-L859) | <pre>if matches!(self.video_standard, VideoStandard::Ntsc) && lol {<br>&nbsp;&nbsp;&nbsp;&nbsp;NTSC_LONG_COLORCLOCKS_PER_LINE<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;COLORCLOCKS_PER_LINE<br>}</pre> with `pub const COLORCLOCKS_PER_LINE: u32 = 227;`, and `long_line_toggles()` is true only for NTSC. **Every PAL line is 227** |
| **MiSTer** | [`rtl/agnus_beamcounter.v:86`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86), [`:281-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L281-L285) | descendant — identical to NanoMig: `parameter HTOTAL_VAL      = 8'd227 - 8'd1;`, and the long-line bit held clear in PAL |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:102`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L102) | descendant — the same constant, one bit wider, `parameter HTOTAL_VAL      = 9'd227 - 9'd1;` |

## Decision

Not an RTL change. If no PAL configuration alternates line lengths, p. 304's PAL row is a misprint and the two assertions grade a number
the manual itself contradicts on p. 23.
