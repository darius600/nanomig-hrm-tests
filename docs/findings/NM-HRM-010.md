# NM-HRM-010 — the NTSC line is 227 colour clocks every time, where the manual alternates 228 and 227 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should NanoMig's NTSC line end one colour clock later on every other line (228 and 227 alternating, as p. 23 prints), where `agnus_beamcounter.v` ends every line at 227 and says in its own comment that the 227.5 line is not supported? Whether NanoMig wants the NTSC half clock at all is a project question before it is a code one. Closes when the end-of-line comparison (`agnus_beamcounter.v:250`) lengthens every line the long-line bit marks by one colour clock and `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)`, `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC bundled with line count)`, `P3`, `P2-NTSC`, `P3-NTSC-SET` and `PRE-P3COVER`, and `P14-0-LINES` and `P14` of `integration/aa-integ-s2`, turn green, or when an Amiga specialist records that the simplification stays. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NTSC lines at the hardwired line length (VARBEAMEN = 0), measured on `agnus_beamcounter`; an interlaced ECS/NTSC display (BPLCON0 = $1204), a non-interlaced one without bitplanes (BPLCON0 = $0200), and the non-interlaced ECS/NTSC and OCS/NTSC displays of the light-pen run (BPLCON0 = $0000 while the beam is measured), each on fx68k and on TG68K; and the AA chip set in normal mode with the PAL bit clear (BEAMCON0 = $0000), on fx68k and on TG68K. A programmed line length is [NM-AA-005](NM-AA-005.md)'s. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C — Enhanced Chip Set, printed p. 304; 2 — Coprocessor Hardware, printed pp. 23, 24; 6 — Blitter Hardware, printed p. 194 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · second document: Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY, PDF p. 29 (STRLONG, VPOSR) and p. 12 (PAL); the document prints no page numbers · <details><summary>claim ids by assertion (19)</summary> claims, each with the assertion that carries it: `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)`: `opus:C-scan.c02`, p. 304; `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC bundled with line count)`: `opus:7-appC-beam.c2`, p. 304; `P3`: `ch2-vertical-beam-position.c15`, `ch2-vertical-beam-position.c16`, `ch2-vertical-beam-position.c17`, `ch2-vertical-beam-position.c18`, `ch2-vertical-beam-position.c19`, `COP2-vertical-beam-position.c17`, `COP2-vertical-beam-position.c18`, `COP2-vertical-beam-position.c19`, `COP2-vertical-beam-position.c20`, `COP2-vertical-beam-position.c21`, `COP2-vertical-beam-position.c22`, p. 24; `P2-NTSC`: `COP2-horizontal-beam-position.c10`, p. 23, and, graded over the measured lines only, `COP2-horizontal-beam-position.c11`, `COP2-horizontal-beam-position.c12`, p. 23, `COP2-vertical-beam-position.c11`, p. 24, and, recorded and not asserted, `COP2-horizontal-beam-position.c14`, p. 23; `P3-NTSC-LONGONLY`, recorded and not graded: `COP2-horizontal-beam-position.c13`, p. 23. The control `COLOUR-CLOCKS-PER-LINE-227POINT5 (control, no claim id)`, `PT-1`, `PT-2`, `PT-3`, `PT-5`, `PT-7`, `P3-NTSC-SET`, `PRE-P3COVER` and `LP-A2-LINE-LENGTHS` carry no claim id; `LP-A2-LINE-LENGTHS` cites p. 23. `P14-0-LINES`, `P14`, `P14-0-LOL`, `P14-0-LONGONLY` and `P14-1` of `integration/aa-integ-s2` carry no claim id either. p. 194 is quoted as context and carries none </details> |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L86) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86); inherited |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking): `make -C hrm/ch3-beam-counters-and-blanking test`; [`integration/ch2-interlace-fields`](../../integration/ch2-interlace-fields): `make -k -C integration/ch2-interlace-fields test`; [`integration/ch2-copper-beam`](../../integration/ch2-copper-beam): `make -k -C integration/ch2-copper-beam test`; [`integration/appDE-ports-disk`](../../integration/appDE-ports-disk): `make -k -C integration/appDE-ports-disk test`; [`integration/aa-integ-s2`](../../integration/aa-integ-s2): `make -k -C integration/aa-integ-s2 test`; an expected red exits nonzero; the private chip-set copy of the Decision, not run by `make test`: `NMH_ONLY=A make -C integration/ch2-copper-beam lc-test` |
| **Assertions** | `hrm/ch3-beam-counters-and-blanking`, 2 red of this finding's: `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)` — expected an average of 227.5, observed 227.0 (227, 227, 227, 227); `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC bundled with line count)` — the 525-line half matches, the same 227.0 average does not. The package is 28 passed, 5 failed: the other three reds are the PAL pair, [NM-HRM-068](NM-HRM-068.md)'s, and `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`, [NM-HRM-011](NM-HRM-011.md)'s. Four integration packages are red the same way on each core, listed under *Where else the suite meets it*. |
| **Verdict of the four** | **unanimous against NanoMig**: WinUAE, vAmiga, Denise and Copperline all make every other NTSC line 228 colour clocks. NanoMig toggles the bit that reports a long line and keeps every line at 227. Hardware: not measured |
| **Effort** | M — the end-of-line comparison at `agnus_beamcounter.v:250`, which the rest of the module's horizontal timing hangs off; the long-line toggle itself is not touched |
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

**Chapter 2, printed p. 24**:

> There are alternating long and short lines, there are also long and short fields (interlace only). In NTSC, the fields are 262, then 263 lines and in PAL, 312, then 313 lines.

**Chapter 6, printed p. 194**, gives the same figure for the line as a whole:

> During a horizontal scan line (about 63 microseconds), there are 227.5
> "color clocks", or memory access cycles.

For NTSC the pages agree: no line lasts half a colour clock, two consecutive lines last 228 and 227, and the average is p. 304's 227.5.

## RTL and measurement

**Every line is 227 colour clocks, in NTSC as in PAL.** The length is one constant, [`agnus_beamcounter.v#L86`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L86), and the line ends where the horizontal counter reaches it, with no long-line term, at [`#L250`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L250):

```verilog
parameter HTOTAL_VAL      = 8'd227 - 8'd1;   // line length of 227 CCKs in PAL mode (NTSC line length of 227.5 CCKs is not supported)
```

The long-line bit exists, alternates in NTSC and is reported in VPOSR ([`#L110`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L110)), but its own comment calls it "not used, only for better NTSC compatibility" and nothing reads it for the line length ([`#L291-L300`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L291-L300)).

[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) counts colour-clock periods between consecutive end-of-line pulses over four lines of the hardwired default, and compares an average because a single line cannot end on half a clock. A control that writes HTOTAL = 100 under VARBEAMEN reads 101 colour clocks per line on the same comparator, so an alternating line would show.

| standard | four lines | average | p. 304 | p. 23 |
|---|---|---:|---:|---:|
| NTSC | 227, 227, 227, 227 | 227.0 | 227.5 | 228 and 227 alternating |

### Where else the suite meets it

The reds of the four integration packages: `integration/ch2-interlace-fields`, 1 red on each core, `P3` — expected a 228-colour-clock last line in two fields of every four at one fixed phase, observed 3150 of 3150 line lengths at 227 (31 passed, 1 failed on fx68k and on TG68K); its preconditions `PT-1`, `PT-2`, `PT-3`, `PT-5` and `PT-7` are green on both, and the gates `PT-4` (the line 0 and line 1 marks are 227 colour clocks apart) and `PT-6` print open. `integration/ch2-copper-beam`, 3 red on each core: `P2-NTSC` — expected consecutive NTSC lines alternating 228 and 227 colour clocks, observed 227 on 1908 of 1908 pairs; `PRE-P3COVER` — expected $E0 or later read on at least 20 lines of each length, observed 0 lines of 228 and 315 of 227; `P3-NTSC-SET` — expected $E3 read as well as every even count from $00 to $E2, observed $00 to $E2 and never $E3 (226 passed, 5 failed on fx68k and on TG68K; the other two, `P6-LARGEST` in PAL and in NTSC, are [NM-HRM-089](NM-HRM-089.md)'s). `P2-PAL` and `P3-PAL` are green in the same run, and `P3-NTSC-LONGONLY` is recorded NOT GRADED behind `PRE-P3COVER`. `integration/appDE-ports-disk`, 4 red on each core: `LP-A2-LINE-LENGTHS` in ECS/NTSC and OCS/NTSC on both connectors — expected lines alternating 227 and 228 colour clocks with H at most $E3, observed 263 lines per field, all 227 colour clocks, and the largest H read 225 on fx68k and 226 on TG68K (230 passed, 56 failed on fx68k and on TG68K; the other 52 are [NM-HRM-087](NM-HRM-087.md)'s). The four PAL `LP-A2-LINE-LENGTHS` lines, ECS and OCS on both connectors, are green in the same run. `integration/aa-integ-s2`, 2 red on each core: `P14-0-LINES` — expected the port lines of a stopped frame with the PAL bit clear in normal mode to alternate, the longer ones 228 colour clocks, observed 227 as the longest; and the aggregate `P14`, red through it alone (131 passed, 13 failed on fx68k and on TG68K; the other eleven are not this finding's: `F5-SV-INV` [NM-HRM-032](NM-HRM-032.md)'s, `P9-BV`, `P9-B` and `P9-ID` [NM-AA-009](NM-AA-009.md)'s, `P11-HHRUN` and `P12-K` [NM-AA-008](NM-AA-008.md)'s, `P12-UHRES` [NM-AA-010](NM-AA-010.md)'s, `P13-0-LINES` and `P13-0-COUNT` [NM-AA-005](NM-AA-005.md)'s, and the two light-pen lines ([NM-HRM-087](NM-HRM-087.md))). `P14-0-LOL` (LOL alternates) and `P14-1` (313-line frames with the PAL bit set against 263 with it clear) are green in the same run, and `P14-0-LONGONLY` prints NOT GRADED because no line is 228

[`integration/ch2-interlace-fields`](../../integration/ch2-interlace-fields) shows the same at the Copper: an interlaced NTSC display whose Copper list marks each line with a `WAIT (v, $E0)` and a MOVE, timed at the chip-RAM port, with each line's first bitplane fetch standing in from line 254 on, where the marks stop because a WAIT compares only eight bits of the line. p. 24's four-field pattern needs a 228-colour-clock last line in two fields of every four. `P3` grades twelve consecutive fields at any phase and is gated by `PT-4` and `PT-6`, both open in the normal run: the fields alternate 263 and 262 lines, but every field's last line is 227 colour clocks (1816 cycles at 8 per colour clock). `PT-5` stays green because 227 is one of the two page values.

The `MEASURED` lines are identical on both cores ([`test_ch2_interlace_fields.cpp:1867-1887`](../../integration/ch2-interlace-fields/test_ch2_interlace_fields.cpp) prints them):

```text
MEASURED: P3 measured: Delta = 1288 clk_sys (161 colour clocks)
MEASURED: P3 measured: (f_k, x_k in colour clocks) for k = 0..11 = (long,227) (short,227) (long,227) (short,227) (long,227) (short,227) (long,227) (short,227) (long,227) (short,227) (long,227) (short,227)
MEASURED: P3 measured: of 3150 line lengths in fields 0..11, 3150 are 227 colour clocks, 0 are 228 and 0 are neither
```

[`integration/ch2-copper-beam`](../../integration/ch2-copper-beam) shows it without the interlace and with the CPU's own reads. A non-interlaced ECS/NTSC display without bitplanes (BPLCON0 = $0200) runs a Copper list that puts a MOVE behind `WAIT(v,$40)` on every line from $30 to $FF and behind `WAIT(n,$60)` on the lines after the wrap; each MOVE is timed at the chip-RAM port, so two consecutive markers are one line apart, L(v), counted in memory cycles, which p. 194 calls colour clocks. Meanwhile the CPU reads VPOSR and VHPOSR as one long word 50,000 times in a row and stores every sample. `P2-NTSC` grades p. 23's alternation over L(v) (`test_ch2_copper_beam.cpp:1032-1035`); `P3-NTSC-SET` grades the range p. 23 gives a long line, $00 to $E3, over the horizontal counts read; `PRE-P3COVER` gates the clause that only long lines reach $E3 and needs samples near the end of at least 20 lines of each length. All 1908 pairs of consecutive lines are 227 colour clocks apart, and the count reaches $E2 and never $E3, though 315 lines are sampled at $E0 or later. The same capture made with the original chip set gives the same 227s, so the red does not rest on BEAMCON0's reset value; the PAL run is green on the same comparators (`P2-PAL`, 227 on every measured line; `P3-PAL`, $00 to $E2). The NTSC verdicts and counts below are identical on fx68k and TG68K; the `MEASURED: P3` line is fx68k's, and TG68K's lists a different set of 24 missing odd counts.

```text
RECORDED: P2 NTSC run A: L(v) values measured {227} over 1908 of 1908 pairs
MEASURED: P3 NTSC run A: H values observed 00..E2, 203 distinct; even values not observed (0): none; odd values up to $E1 not observed (24): 05 07 09 0B 0D 0F 13 15 17 1B 1F 21 23 27 2B 2D 2F 33 37 39 3B 3F 43 45; lines with H>=$E0 and L=228: 0, with L=227: 315
FAIL: PRE-P3COVER NTSC run A: H >= $E0 is observed on at least 20 distinct lines with L(v)=228 and at least 20 with L(v)=227 (0 and 315)
MEASURED: OCS-CHECK NTSC run A: ECS field-length set {263} vs OCS {263}; line-length L(v) set {227} over 1908 pairs vs OCS {227} over 1908 pairs -- match, assumption checked
```

[`integration/appDE-ports-disk`](../../integration/appDE-ports-disk) reads the same counter in its light-pen runs. In the reference phase of each run, with the light pen disabled, the CPU reads VPOSR and VHPOSR 20,000 times, and the test fits a model of the beam to the reads: lines per field, which lines are long, how long a line is. `LP-A2-LINE-LENGTHS` grades that model against p. 23 (`test_appDE_ports_disk.cpp:1199`), in ECS and OCS, PAL and NTSC, on both connectors. The four PAL runs fit lines of 227 colour clocks and pass. The four NTSC runs fit 263 lines per field, every line 227 colour clocks and no alternating pattern, and fail; the largest H read is 225 on fx68k and 226 on TG68K. fx68k prints, for ECS/NTSC on the first connector:

```text
MEASURED: ECS/NTSC J11 model: 263 lines per field, all lines 227 colour clocks, the vertical count trails the horizontal wrap by 1 colour clock(s), anchor uncertainty u=7 colour clocks (19998 of 20000 usable reference reads bound it), F0=11, largest H read 225, reference field lengths in clk_sys: 477608 477608 477608 477608 477608
FAIL: LP-A2-LINE-LENGTHS ECS/NTSC J11: line lengths are 227 and 228 alternating (NTSC) or all 227 (PAL), and H never exceeds $E3 (NTSC) or $E2 (PAL) (measured: all lines 227 colour clocks, largest H read 225; expected alternating 227 and 228, H at most $E3) [p.23]
```

[`integration/aa-integ-s2`](../../integration/aa-integ-s2) reaches the same counter from the AA specification. It runs the whole design with the AA chip set selected, twice: with BEAMCON0 = $0000 (PAL, VARBEAMEN and LOLDIS clear) and with $0020 (PAL set). In one frame of each run the CPU is stopped and every DMA channel is off, so each access at the chip-RAM port is a memory refresh, four to a line, and the spacing of those groups is the line's length. With the PAL bit clear the frame has 263 lines and LOL alternates, but no line is longer than 227 colour clocks; with it set the frame has 313 lines. A BEAMCON0 write sets the beam counter's PAL decode from bit 5 whenever the chip set is enhanced ([`agnus_beamcounter.v#L167-L175`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L167-L175), `pal <= data_in[5];`), and selecting AA sets that input too ([`minimig.v#L488`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L488), `.ecs(|chipset_config[4:3]),`), so the counter measured above is the one measured here. fx68k and TG68K print these lines identically:

```text
MEASURED: P14 measured: frame periods in units of T_L: PAL set 313 and 313, PAL clear 263 and 263
MEASURED: P14 measured, PAL clear: port line lengths in colour clocks 227,227,227,227..., longest = 227
PASS: P14-0-LOL: with PAL clear in normal mode LOL alternates between consecutive sampled lines
FAIL: P14-0-LINES: with PAL clear in normal mode the port lines alternate and the longer ones are 228 colour clocks
FAIL: P14: in normal mode the PAL bit selects decodes that differ from those with PAL clear, which has every other line a long line of 228 colour clocks
```

`P14-0-LINES` is graded at `test_aa_integ_s2.cpp:1817` and `P14` at `:1822-1823`.

## Risks and unknowns

- **Not measured:** hardware. The alternation under a programmed line length is [NM-AA-005](NM-AA-005.md)'s.
- **The RTL names the simplification itself**, so whether NanoMig wants the half clock is a project question before a code one.
- **Does the long-line bit mislead?** VPOSR bit 7 alternates in NTSC and reports long lines that are not longer, so a program that reads it is told something the beam does not do.
- **Is the half clock visible above the beam counter?** p. 23 says the Copper sees it: on a long line the horizontal position reaches $E3. On NanoMig it never passes $E2 (`integration/ch2-copper-beam` reads VHPOSR on 315 NTSC lines at $E0 or later and never sees $E3), so a Copper list or a program that waits on the last horizontal position of an NTSC line sees a different line from the one the page describes.
- **One clause is not graded.** `P3-NTSC-LONGONLY`, that only a long line reaches $E3 (`COP2-horizontal-beam-position.c13`), is recorded NOT GRADED because `PRE-P3COVER` fails: every one of the 1908 measured pairs is 227 colour clocks, so no 228-colour-clock line exists to read it on.
- **`P2-NTSC` grades the measured lines only.** It compares L(v) for v = $30 to $FE and the pairs after the wrap; lines 0 to $2F, line $FF and the end of the field are not graded, so a green `P2-NTSC` establishes p. 23's alternation over those lines only.
- **`LP-A2-LINE-LENGTHS` has no green control for its alternating branch.** The all-227 branch passes in the four PAL runs; none of the chip sets the package runs makes NTSC lines alternate 228 and 227, so its NTSC red measures 227 against p. 23's alternation without a passing counterpart, and the closure in *What is asked of a reader* does not name it.
- **The AA specification's STRLONG row rests on one reading.** In its corpus page (PDF page 29) the row `STRLONG h 03E ... (228 CC)` is the model reading of the page image alone, the OCR rendering its address as `O03E`; the line `lines with long counts(228- NTSC, HTOTAL+2- VARBEAMEN=1` is read by all three readings. `integration/aa-integ-s2` grades 228 from that row; no PAL-set decode is graded there, because the AA pages give none.

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

**All four take the standard from BEAMCON0's PAL bit on an enhanced Agnus**, which is how `integration/aa-integ-s2` selects NTSC, and toggle the long line from it. WinUAE derives the toggle from the written value, `linetoggle = !(new_beamcon0 & BEAMCON0_PAL) && !(new_beamcon0 & BEAMCON0_LOLDIS);` ([`custom.cpp:2054`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2054)), and takes the write only `if (ecs_agnus) {` ([`:3385-3387`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3385-L3387)). vAmiga's `pokeBEAMCON0` returns on OCS and otherwise switches the video format, `auto format = GET_BIT(value, 5) ? TV::PAL : TV::NTSC;` ([`Core/Components/Agnus/AgnusRegs.cpp:554-563`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L554-L563)), which for NTSC sets `lolToggle = true;` ([`Core/Components/Agnus/Beam.cpp:294-298`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.cpp#L294-L298)). Denise, on ECS and later, sets `lolToggle = !(value & LOLDIS) && !(value & BEAM_PAL);` ([`emulation/libami/agnus/register.cpp:550-565`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L550-L565)). Copperline's `write_beamcon0` selects `VideoStandard::Ntsc` when bit 5 is clear, on every Agnus but the original one ([`src/chipset/agnus.rs:1322-1336`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1322-L1336)). Each would therefore alternate 228 and 227 in that package's run with the PAL bit clear; that is read from the code, not run.

The AA specification, Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY, agrees with the manual for the AA chip set. Its entry for the strobe Agnus puts on the register bus to mark a long line, PDF page 29:

```text
STRLONG   h 03E  S      D    Strobe for identification of long horz. line(228 CC)
```

continues:

```text
                    The 4th strobe shown above is used during the second
                    refresh time slot of every other line, to identify
                    lines with long counts(228- NTSC, HTOTAL+2- VARBEAMEN=1
                    hires chips only).
```

and, on the same page, VPOSR's `LOL=Long line bit. When low, it indicates short raster line.` Its BEAMCON0 entry, PDF page 12, has the PAL bit select the standard: `PAL= Set appropriate decodes (in normal mode) for PAL.` NTSC's long line is 228 counts on every other line, and `integration/aa-integ-s2` measures NanoMig against it and finds no line longer than 227. The long line under a programmed line length, which the same STRLONG entry names `HTOTAL+2- VARBEAMEN=1`, is [NM-AA-005](NM-AA-005.md)'s.

### What other Commodore documents say

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989**, PDF page 25 — agrees with the manual:

  > "During scan of a horizon tal line, there are typically 227.5 memory access cycles, each of which is approximately 280 nanoseconds in duration."
  <!-- corpus: patent-us4874164-agnus-denise/p025.txt -->

  Same figure as the manual, per horizontal line. It says 'typically' and names no video standard. The same sentence is on patent-us5103499-copper p025.

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 11 — agrees with the manual:

  > "strobe shown above is used during the second refresh time slot of every other line, to identify lines with long counts (228)."
  <!-- corpus: denise-spec-252126/p011.txt -->

  The ECS Denise specification says every other line is a long line of 228 counts. This is the alternation NanoMig does not generate.

### What practitioners report

Reports from the forum thread [EAB 19676](https://eab.abime.net/showthread.php?t=19676): witnesses, never an oracle ([practitioner reports](../evidence/practitioner-reports.md)).

**SUPPORTS.** [EAB post 1718467](https://eab.abime.net/showpost.php?p=1718467), Toni Wilen, 2024-12-02. Chip set: Agnus and Alice, NTSC. Known from: not stated; the post says the behaviour took reverse-engineering.

> "NTSC has 227.5 CCK scan line length. To simplify logic, NTSC mode Agnus generates alternating 227 and 228 scan line lengths."

A statement about the real chips that backs p. 23's alternation, which the page otherwise finds only in the four implementations.

**SUPPORTS.** [EAB post 601811](https://eab.abime.net/showpost.php?p=601811), yaqube, 2009-10-06. Chip set: not stated; NTSC mode. Known from: "I have made some tests in the NTSC mode"; the set-up is not stated.

> "It seems that during long lines (228 CCK long) there is no "unusable" or "blocked" cycles."

Independent bus-level evidence that a 228-colour-clock line exists in NTSC.

## Decision

**Let the long-line bit lengthen the line it marks**, as WinUAE (`maxhpos_short + lol`) and Denise (`0xe2 + lol`) do: end a line one colour clock later when `long_line` is set, so that in NTSC consecutive lines last 228 and 227. The bit already alternates in the right standard, so the change is to the end-of-line comparison at `:250`, not to the toggle. A private copy of the chip set whose NTSC lines alternate 228 and 227 colour clocks (the package's `ntsc_long.patch`, not run by `make test`) prints, for `integration/ch2-copper-beam` run A, 46 passed, 0 failed on fx68k and on TG68K: `P2-NTSC`, `PRE-P3COVER` and `P3-NTSC-SET` pass, L(v) is {227,228} over 1908 pairs, and `PRE-P3COVER` reads (320 and 140).

That comparison is the one every other horizontal timing in the module hangs off, and the RTL's own comments call the half clock unsupported rather than broken. Decide it together with [NM-HRM-033](NM-HRM-033.md), which is about when the same bit may toggle, and [NM-AA-005](NM-AA-005.md), which asks the same question under a programmed line length.
