# NM-HRM-010 — the NTSC line is 227 colour clocks every time, where the manual alternates 228 and 227 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should NanoMig's NTSC line end one colour clock later on every other line (228 and 227 alternating, as p. 23 prints), where `agnus_beamcounter.v` ends every line at 227 and says in its own comment that the 227.5 line is not supported? The decision belongs to an Amiga specialist; whether NanoMig wants the NTSC half clock at all is a project question before it is a code one. Closes when a specialist accepts the change, in which case `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)` and `P3` are the reds that should turn green, or records that the simplification stays. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer, as a question rather than a defect: MiSTer and TC64 carry the same simplification with the same comment, so there is no fix in either to port |
| **Scope** | NTSC lines at the hardwired line length (VARBEAMEN = 0), measured on `agnus_beamcounter`; and an interlaced ECS/NTSC display (BPLCON0 = $1204) on fx68k and on TG68K. A programmed line length is [NM-AA-005](NM-AA-005.md)'s. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C — ECS, printed p. 304; 2 — Coprocessor, printed pp. 23, 24; 6 — Blitter, printed p. 194 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims, each with the assertion that carries it: `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)`: `C-scan.c02`, p. 304; `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC bundled with line count)`: `opus:7-appC-beam.c2`, p. 304; `P3`: `ch2-vertical-beam-position.c15` to `.c19` and `COP2-vertical-beam-position.c17` to `.c22`, p. 24. The control `COLOUR-CLOCKS-PER-LINE-227POINT5 (control, no claim id)` and `PT-1`, `PT-2`, `PT-3`, `PT-5` and `PT-7` carry no claim id. p. 23 and p. 194 are quoted as context and carry none |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L86) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86); inherited |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking): `make -C hrm/ch3-beam-counters-and-blanking test`; [`integration/ch2-interlace-fields`](../../integration/ch2-interlace-fields): `make -k -C integration/ch2-interlace-fields test`; an expected red exits nonzero |
| **Assertions** | `hrm/ch3-beam-counters-and-blanking`, 2 red of this finding's: `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)` — expected an average of 227.5, observed 227.0 (227, 227, 227, 227); `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC bundled with line count)` — the 525-line half matches, the same 227.0 average does not. The package is 28 passed, 5 failed: the other three reds are the PAL pair, [NM-HRM-068](NM-HRM-068.md)'s, and `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`. `integration/ch2-interlace-fields`, 1 red on each core, `P3` — expected a 228-colour-clock last line in two fields of every four at one fixed phase, observed 3150 of 3150 line lengths at 227 (31 passed, 1 failed on fx68k and on TG68K) |
| **Verdict of the four** | **unanimous against NanoMig**: WinUAE, vAmiga, Denise and Copperline all make every other NTSC line 228 colour clocks. NanoMig toggles the bit that reports a long line and keeps every line at 227. Hardware status: not measured |
| **Effort** | M: the end-of-line comparison at `agnus_beamcounter.v:250`, which the rest of the module's horizontal timing hangs off; the long-line toggle itself is not touched |
| **See also** | [NM-HRM-068](NM-HRM-068.md) — the PAL row of the same sentence, where p. 23 sides with the RTL; [NM-HRM-033](NM-HRM-033.md) — the condition on the same toggle; [NM-AA-005](NM-AA-005.md) — the AA specification's HTOTAL entry, the same missing long line under a programmed line length |

---

## Source requirement

**Appendix C, printed p. 304**:

> NTSC (525 lines, 227.5 colorclocks per scan line)

**Chapter 2, printed p. 23**, says how a line of 227.5 colour clocks is made:

> All lines are not the same length in NTSC. Every other line is a long line
> (228 color clocks, 0-$E3), with the others being 227 color clocks long. In
> PAL, they are all 227 long. The display sees all these lines as 227 1/2
> color clocks long, while the copper sees alternating long and short lines.

**Chapter 6, printed p. 194**, gives the same figure: "227.5 'color clocks'". For NTSC the pages agree: no line lasts half a colour clock, two consecutive lines last 228 and 227, and the average is p. 304's 227.5.

## RTL and measurement

**Every line is 227 colour clocks, in NTSC as in PAL.** The length is one constant, [`agnus_beamcounter.v#L86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L86), and the line ends where the horizontal counter reaches it, with no long-line term, at [`#L250`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L250):

```verilog
parameter HTOTAL_VAL      = 8'd227 - 8'd1;   // line length of 227 CCKs in PAL mode (NTSC line length of 227.5 CCKs is not supported)
```

The long-line bit exists, alternates in NTSC and is reported in VPOSR, but its own comment calls it "not used, only for better NTSC compatibility" and nothing reads it for the line length ([`#L291-L300`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L291-L300)).

[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) counts colour-clock periods between consecutive end-of-line pulses over four lines of the hardwired default, and compares an average because a single line cannot end on half a clock. A control that writes HTOTAL = 100 under VARBEAMEN reads 101 colour clocks per line on the same comparator, so an alternating line would show.

| standard | four lines | average | p. 304 | p. 23 |
|---|---|---:|---:|---:|
| NTSC | 227, 227, 227, 227 | 227.0 | 227.5 | 228 and 227 alternating |

[`integration/ch2-interlace-fields`](../../integration/ch2-interlace-fields) shows the same at the Copper: an interlaced NTSC display whose Copper list marks each line with a `WAIT (v, $E0)` and a MOVE, timed at the chip-RAM port. p. 24's four-field pattern needs a 228-colour-clock last line in two fields of every four. `P3` grades twelve consecutive fields at any phase and is gated by `PT-4` and `PT-6`, both open in the normal run: the fields alternate 263 and 262 lines, but every field's last line is 227 colour clocks (1816 cycles at 8 per colour clock). `PT-5` stays green because 227 is one of the two page values.

The `MEASURED` lines are identical on both cores ([`test_ch2_interlace_fields.cpp:1453-1472`](../../integration/ch2-interlace-fields/test_ch2_interlace_fields.cpp) prints them):

```text
MEASURED: P3 measured: Delta = 1288 clk_sys (161 colour clocks)
MEASURED: P3 measured: (f_k, x_k in colour clocks) for k = 0..11 = (long,227) (short,227) (long,227) (short,227) (long,227) (short,227) (long,227) (short,227) (long,227) (short,227) (long,227) (short,227)
MEASURED: P3 measured: of 3150 line lengths in fields 0..11, 3150 are 227 colour clocks, 0 are 228 and 0 are neither
```

## Risks and unknowns

- **Not measured:** hardware, and a programmed line length ([NM-AA-005](NM-AA-005.md)).
- **The RTL names the simplification itself**, so whether NanoMig wants the half clock is a project question before a code one.
- **Does the long-line bit mislead?** VPOSR bit 7 alternates in NTSC and reports long lines that are not longer, so a program that reads it is told something the beam does not do.
- **Is the half clock visible above the beam counter?** p. 23 says the Copper sees it: on a long line the horizontal position reaches $E3. On NanoMig it never passes $E2.

## Independent evidence

The question put to each: how long is an NTSC line?

| | code | |
|---|---|---|
| **WinUAE** | [`include/custom.h:116-117`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L116-L117), [`custom.cpp:2054`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2054), [`:2328-2333`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2328-L2333), [`:6771`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6771) | <pre>maxhpos = maxhpos_short + lol;<br>maxhpos_lol = lol;<br>maxhpos_long = linetoggle ? maxhpos_short + 1 : -1;</pre> on a short line of `#define MAXHPOS_NTSC 227`, with `linetoggle = !(new_beamcon0 & BEAMCON0_PAL) && !(new_beamcon0 & BEAMCON0_LOLDIS);` and BEAMCON0's reset value `currprefs.ntscmode ? 0x00 : BEAMCON0_PAL`. **NTSC lines alternate 228 and 227** |
| **vAmiga** | [`Core/Components/Agnus/Beam.h:80`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L80), [`Beam.cpp:263-264`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.cpp#L263-L264), [`:294-298`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.cpp#L294-L298) | <pre>isize hCnt() const { return lol ? 228 : 227; }</pre> with the toggle at every end of line, `if (lolToggle) lol = !lol;`, and `lolToggle = true;` when the machine is switched to NTSC. **NTSC lines alternate 228 and 227** |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:1030`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1030), [`:217-219`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L217-L219), [`events.cpp:479-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L479-L480) | <pre>fps = (double)frequency() / (227.5 * 263.0); // start with lol toggling</pre> NTSC `227.5 × 263`, PAL `227.0 × 313`. The next line is scheduled as `updateEvent<EVENT_HTOTAL>((beamCon & VARBEAMEN) ? (hTotal + lol) : (0xe2 + lol));` after `if (lolToggle) lol ^= 1;`, with `lolToggle = ntsc;` at reset. **NTSC lines alternate 228 and 227** |
| **Copperline** | [`src/chipset/agnus.rs:9`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L9), [`:18`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L18), [`:841-859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L841-L859) | <pre>if matches!(self.video_standard, VideoStandard::Ntsc) && lol {<br>&nbsp;&nbsp;&nbsp;&nbsp;NTSC_LONG_COLORCLOCKS_PER_LINE<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;COLORCLOCKS_PER_LINE<br>}</pre> with `pub const NTSC_LONG_COLORCLOCKS_PER_LINE: u32 = 228;`, and `long_line_toggles()` true for NTSC unless the beam is programmed or LOLDIS is set. **NTSC lines alternate 228 and 227** |
| **MiSTer** | [`rtl/agnus_beamcounter.v:86`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86) | descendant — identical to NanoMig: `parameter HTOTAL_VAL = 8'd227 - 8'd1;` with the same "NTSC line length of 227.5 CCKs is not supported" comment |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:102`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L102) | descendant — the same simplification and the same comment, the constant only wider (`9'd227 - 9'd1` versus `8'd227 - 8'd1`) inside a file whose port list and internals are otherwise substantially rewritten (371 lines differ with line endings and whitespace normalised, in a file of 461). Both forks decline the half clock exactly as NanoMig does |

**All four make the half clock the way p. 23 describes it**: whole lines, one colour clock longer every other time. Denise's comment names the mechanism, *lol toggling*. MiSTer and TC64 are the same simplification inherited, not a third reading.

**Other Commodore documents** that speak to this:

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989**, PDF page 25 — agrees with the manual:

  > "During scan of a horizon tal line, there are typically 227.5 memory access cycles, each of which is approximately 280 nanoseconds in duration."
  <!-- corpus: patent-us4874164-agnus-denise/p025.txt -->

  It says 'typically' and names no video standard. The same sentence is on patent-us5103499-copper p025.

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 11 — agrees with the manual:

  > "strobe shown above is used during the second refresh time slot of every other line, to identify lines with long counts (228)."
  <!-- corpus: denise-spec-252126/p011.txt -->

  Every other line is a long line of 228 counts: the alternation NanoMig does not generate.

## Decision

**Should the long-line bit lengthen the line it marks?** As WinUAE (`maxhpos_short + lol`) and Denise (`0xe2 + lol`) do: end a line one colour clock later when `long_line` is set, so that in NTSC consecutive lines last 228 and 227. The bit already alternates in the right standard, so the change is to the end-of-line comparison at `:250`, not to the toggle.

That comparison is the one every other horizontal timing in the module hangs off, and the RTL's own comments call the half clock unsupported rather than broken. Decide it together with [NM-HRM-033](NM-HRM-033.md), which is about when the same bit may toggle, and [NM-AA-005](NM-AA-005.md), which asks the same question under a programmed line length. It closes when a specialist accepts the change or records that the simplification stays.
