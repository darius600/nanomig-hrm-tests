# NM-AA-005 — the long-line toggle runs, but no line is a colour clock longer [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should the alternate line last HTOTAL+2 colour clocks (82, with HTOTAL 80) while VPOSR bit 7 alternates, where this core measures 81 and 81? The AA specification's HTOTAL entry says it should. Closes when the `end_of_line` comparison in `agnus_beamcounter.v` lengthens every other line by one colour clock and `LONG-LINES-ARE-ONE-CLOCK-LONGER`, and `P13-0-LINES` and `P13-0-COUNT` of `integration/aa-integ-s2` on both cores, are green, or when an Amiga specialist rejects the long line for NanoMig and the reason is recorded here. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Agnus beam counter under the AA specification's HTOTAL entry; chip revision `unknown` (the HTOTAL entry names none); measured with the PAL bit low, LOLDIS low, VARBEAMEN set and HTOTAL 80, and in `integration/aa-integ-s2` with the same bits (BEAMCON0 $0080), HTOTAL $E2 and VTOTAL $138, AA chip set selected (`chipset_config` $10), at `nanomig.v`'s ports on fx68k and TG68K. Isolated: the toggle itself runs (green), the line length does not follow it |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 24, the HTOTAL entry; p. 29, the STRLONG entry (no claim id) · <details><summary>claim ids by assertion (67)</summary> p. 24 claims: `LONG-LINES-ARE-ONE-CLOCK-LONGER`: `sol:AA-htotal.c8`; `LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS`: `opus:AA-htotal.c17`, `sol:AA-htotal.c7`; `HTOTAL-IS-THE-HIGHEST-COUNT`: `opus:AA-htotal.c1`, `opus:AA-htotal.c4`, `opus:AA-htotal.c16`, `sol:AA-htotal.c1`, `sol:AA-htotal.c5`, `sol:AA-htotal.c6`; `VARBEAMEN-GATES-THE-LIMIT-REGISTERS`: `opus:AA-htotal.c18`, `sol:AA-htotal.c9`; `HTOTAL-IGNORES-THE-X-CELLS`: `opus:AA-hsstop.c6`, `opus:AA-hsstrt.c6`, `sol:AA-hsstop.c1`, `sol:AA-hsstrt.c1`, `sol:AA-htotal.c10`, `sol:AA-htotal.c12`, `sol:AA-htotal.c14`, `sol:AA-htotal.c16`, `sol:AA-htotal.c18`, `sol:AA-htotal.c20`, `sol:AA-htotal.c22`, `sol:AA-htotal.c24`, `sol:AA-htotal.c26`, `sol:AA-htotal.c27`, `sol:AA-htotal.c28`, `sol:AA-htotal.c29`, `sol:AA-htotal.c30`, `sol:AA-htotal.c31`, `sol:AA-htotal.c33`; `HSSTRT-AND-HSSTOP-BOUND-HSYNC`: `opus:AA-hsstop.c1`, `opus:AA-hsstop.c4`, `opus:AA-hsstrt.c1`, `sol:AA-hsstop.c5`, `sol:AA-hsstrt.c6`; `HSSTRT-NEEDS-VARHSYEN-TOO`: `opus:AA-hsstrt.c7`; `HBSTRT-AND-HBSTOP-BOUND-BLANKING`: `opus:AA-hbstop.c1`, `opus:AA-hbstop.c3`, `opus:AA-hbstrt.c2`, `opus:AA-hbstrt.c4`, `opus:AA-hbstrt.c7`, `opus:AA-hbstrt.c9`, `opus:AA-hbstrt.c10`, `opus:AA-hbstrt.c11`, `opus:AA-hbstrt.c13`, `opus:AA-hbstrt.c14`, `opus:AA-hbstrt.c15`, `opus:AA-hbstrt.c16`, `opus:AA-hbstrt.c17`, `opus:AA-hbstrt.c18`, `opus:AA-hbstrt.c19`, `opus:AA-hbstrt.c20`, `sol:AA-hbstop.c1`, `sol:AA-hbstrt.c1`, `sol:AA-horizontal-blanking.c1`, `sol:AA-horizontal-blanking.c2`, `sol:AA-horizontal-blanking.c3`, `sol:AA-horizontal-blanking.c5`, `sol:AA-horizontal-blanking.c9`, `sol:AA-horizontal-blanking.c10`, `sol:AA-horizontal-blanking.c11`, `sol:AA-horizontal-blanking.c12`, `sol:AA-horizontal-blanking.c13`, `sol:AA-horizontal-blanking.c14`, `sol:AA-horizontal-blanking.c15`, `sol:AA-horizontal-blanking.c16` · p. 17: `LACE-IS-BPLCON0-BIT-2-AND-RESET-CLEARS-IT`: `opus:AA-lace.c1` · `FIXTURE-RUNS-A-PROGRAMMED-LINE`, and `P13-0-LINES`, `P13-0-COUNT`, `P13-0-LOL`, `P13-1-LINES`, `P13-1-LOL`, `P13-0-FIXTURE`, `P13-0-F3`, `P13-0-F4`, `P13-VARMODE` and `P13-LOLDIS` of `integration/aa-integ-s2`, carry no claim id </details> |
| **Module** | `src/minimig-aga/agnus_beamcounter.v` — [NanoMig `4315acb` `:105`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L105) (the declaration and its comment), [NanoMig `4315acb` `:250`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L250) (the `end_of_line` comparison), and the toggle at `:293-298`; inherited (MiSTer and TC64 have the same code) |
| **Test** | [`aa/aa-programmable-sync-windows`](../../aa/aa-programmable-sync-windows) · `make -C aa/aa-programmable-sync-windows test`, or `hrm/run_all.sh --in ../aa` (that one runs the whole tree); [`integration/aa-integ-s2`](../../integration/aa-integ-s2) · `make -k -C integration/aa-integ-s2 test`, or `test-fx68k` and `test-tg68k` of the same package; the expected red exits nonzero |
| **Assertions** | `LONG-LINES-ARE-ONE-CLOCK-LONGER`, red: expected 82 colour clocks for the second line, observed 81 and 81; the package has 10 graded assertions, 9 green and this one red. In `integration/aa-integ-s2`, 2 red on fx68k and 2 on TG68K, in a package run of 131 passed, 13 failed on each core: `P13-0-LINES`, expected the port lines of a stopped frame to alternate between HTOTAL+1 = 227 and HTOTAL+2 = 228 colour clocks, observed 227 on every line; `P13-0-COUNT`, expected as many 228-colour-clock lines as lines with LOL = 1, to within 1, observed 0 against 157. `P13-0-LOL`, LOL alternating in the same mode, is green, and so are `P13-1-LINES` and `P13-1-LOL` with the PAL bit set. That run's eleven other reds are not this finding's: `F5-SV-INV` ([NM-HRM-032](NM-HRM-032.md)), `P9-BV`, `P9-B` and `P9-ID` ([NM-AA-009](NM-AA-009.md)), `P11-HHRUN` and `P12-K` ([NM-AA-008](NM-AA-008.md)), `P12-UHRES` ([NM-AA-010](NM-AA-010.md)), `P14-0-LINES` and `P14` ([NM-HRM-010](NM-HRM-010.md)), and the two light-pen lines ([NM-HRM-087](NM-HRM-087.md)) |
| **Verdict of the four** | **two do it, one cannot, one refuses — and none of them does what this RTL does.** WinUAE and Denise lengthen the alternate line under a programmed HTOTAL; vAmiga lengthens it but does not implement HTOTAL; Copperline stops the toggle when HTOTAL is programmed. A toggle that runs and reports while every line stays the same length is produced by none of the four. Hardware: not measured |
| **Effort** | none; if a specialist wants the long line, M: lengthening the alternate line touches the `end_of_line` comparison at `agnus_beamcounter.v:250`, which every other timing in the module hangs off |
| **See also** | [NM-HRM-033](NM-HRM-033.md), where LOLDIS is ignored without VARBEAMEN |

---

## Source requirement

Below its bit table, page 24's HTOTAL entry says what a line lasts, and closes with `Active if VARBEAMEN=1 or DUAL=1.`:

```
Horiz line has this many + 1 280ns increments. If the PAL bit & LOLDIS are
not high, long line/short line toggle will occur, and there will be this
many + 2 every other line.
```

Two claims: a **condition** — the toggle runs unless the PAL bit or LOLDIS is high — and an **effect** — every other line lasts HTOTAL+2 rather than HTOTAL+1. The package grades them separately, because this module answers them differently. The STRLONG entry on p. 29 says the same for NTSC and for a programmed line: *"lines with long counts(228- NTSC, HTOTAL+2- VARBEAMEN=1"*.

## RTL and measurement

The condition is implemented as printed whenever VARBEAMEN is set; without VARBEAMEN the RTL ignores LOLDIS, which is [NM-HRM-033](NM-HRM-033.md):

```verilog
if (end_of_line)
    if (pal || (loldis && varbeamen))
        long_line <= 1'b0;
    else if (!(loldis && varbeamen))
        long_line <= ~long_line;
```

With the PAL bit and LOLDIS low the bit alternates line by line and reaches software as VPOSR bit 7 (`:110`): over four lines it changes 4 times with the PAL bit low and 0 times with it high, `LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS`, green.

The effect is not implemented. The line ends where `end_of_line` fires, and that comparison never consults `long_line`:

```verilog
if (hpos[8:0]=={htotal[8:1],1'b0})
    end_of_line <= 1'b1;
```

The module says so at the declaration (`:105`, "actually long lines are not supported yet") and at `:86` ("NTSC line length of 227.5 CCKs is not supported"). `LONG-LINES-ARE-ONE-CLOCK-LONGER` counts cck ticks between wraps of the horizontal counter over two consecutive lines with HTOTAL 80: 81 and 81, where HTOTAL+2 is 82; red, one of the package's `9 passed, 1 failed`. The other nine assertions are green, which makes this a reading of the line length rather than of a broken fixture.

[`integration/aa-integ-s2`](../../integration/aa-integ-s2) shows the same at the ports of the whole design. Its P13 writes HTOTAL $E2, VTOTAL $138 and BEAMCON0 $00A0 (VARBEAMEN and the PAL bit) in one run and $0080 (VARBEAMEN alone) in the other, samples VPOSR:VHPOSR over one frame for LOL, and stops the CPU with every DMA channel off over another, so that each access at the chip-RAM port is a refresh, four to a line, and the spacing of those groups is the line's length. With the PAL bit set every line is HTOTAL+1 = 227 colour clocks and LOL stays 0; with it clear LOL alternates, 157 lines of the frame read LOL = 1, and every line is still 227:

```
MEASURED: P13 measured, PAL clear: port line lengths in colour clocks 227,227,227,227...; lines of 228 in frame B = 0, lines with LOL = 1 in frame A = 157
FAIL: P13-0-LINES: with PAL clear (VARBEAMEN set, LOLDIS clear) the port lines of frame B alternate between HTOTAL + 1 = 227 and HTOTAL + 2 = 228 colour clocks
PASS: P13-0-LOL: with PAL clear (VARBEAMEN set, LOLDIS clear) LOL alternates between consecutive sampled lines of frame A
FAIL: P13-0-COUNT: with PAL clear the number of 228-colour-clock port lines in frame B equals the number of LOL = 1 lines in frame A to within 1
```

The same lines on fx68k and TG68K. The PAL-clear run's own anchors, `P13-0-FIXTURE`, `P13-0-F3` (groups of four, every line a whole number of colour clocks) and `P13-0-F4`, are green on both cores, as are `P13-VARMODE` (HTOTAL $C0 shortens every line by 34 colour clocks, so the length follows HTOTAL) and `P13-LOLDIS`. `P13-0-LINES` is graded at `test_aa_integ_s2.cpp:1728`, `P13-0-LOL` at `:1729` and `P13-0-COUNT` at `:1730`.

## Risks and unknowns

- **Hardware is not measured.** The specification is marked PRELIMINARY and the HTOTAL entry names no chip revision.
- **The LOLDIS term is ungraded.** The package never sets LOLDIS; the condition assertion is green for its PAL-bit term only. `integration/aa-integ-s2` sets it only in a precondition, `P13-LOLDIS` (BEAMCON0 $0880: neither the line lengths nor LOL alternate), green on both cores.
- **The document ties the toggle to the PAL bit and LOLDIS, not to VARBEAMEN.** Copperline's suppression under VARBEAMEN answers a question about real silicon that the entry does not ask.
- **Software impact is unknown.** A program that reads VPOSR bit 7 to place a sprite on an alternate line would be told the line is long and find it is not; no such program is named.
- **The long line is a project question.** No line lasts half a colour clock: the NTSC 227.5 is the average of 228 and 227. Whether NanoMig wants the alternation at all comes before any edit.
- **The witnesses split** two to one to one, with vAmiga unable to answer for a programmed HTOTAL.

## Independent evidence

The question put to each: with VARBEAMEN set and HTOTAL written, does the alternate line last one colour clock longer?

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:1899`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1899), [`:2330`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2330), [`:10969-10975`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10969-L10975) | **both halves.** With VARBEAMEN, `maxhpos = htotal + 1`; the line length in use is then `maxhpos = maxhpos_short + lol`, so every other line is one colour clock longer than the programmed value. The condition is the document's sentence: `if (!(new_beamcon0 & BEAMCON0_PAL) && !(new_beamcon0 & BEAMCON0_LOLDIS)) { lol = lol ? false : true; linetoggle = true; } else { lol = false; linetoggle = false; }` |
| **vAmiga** | [`Beam.h:80`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L80), [`AgnusRegs.cpp:567`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L567), [`:570`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L570) | **lengthens the line, and cannot answer for a programmed HTOTAL.** `hCnt() const { return lol ? 228 : 227; }` is the whole model of the long line, and the toggle is keyed on NTSC and LOLDIS. HTOTAL is stored under a comment saying these registers *"are recognized and stored, but have no effect yet"* |
| **Denise** | [`events.cpp:479-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L479-L480), [`register.cpp:565`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L565) | **both halves.** The next line is scheduled as `updateEvent<EVENT_HTOTAL>((beamCon & VARBEAMEN) ? (hTotal + lol) : (0xe2 + lol))` — the programmed total plus the toggle — and the condition is `lolToggle = !(value & LOLDIS) && !(value & BEAM_PAL)`, the document's two terms |
| **Copperline** | [`agnus.rs:855-858`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L855-L858), [`:899-904`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L899-L904) | **deliberately neither.** `long_line_toggles()` requires `programmable_line_cck().is_none()`, under a comment that says *"only when the beam is not programmable (VARBEAMEN fixes the line length)"*. With HTOTAL programmed nothing toggles, nothing reports, and every line is HTOTAL+1 |
| **MiSTer** | [`rtl/agnus_beamcounter.v:256`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L256), [`:282-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L282-L285) | descendant — the identical toggle and the identical `hpos[8:0]=={htotal[8:1],1'b0}` comparison with no `long_line` term |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:318`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L318), [`:343-346`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L343-L346) | descendant — the same two places, the same absence |

The four agree that the two halves travel together: where the toggle runs the line grows (WinUAE, Denise, and vAmiga on its fixed NTSC line); where the line cannot grow the toggle is stopped (Copperline, for exactly this mode). No implementation reports an alternating long-line bit over lines that are all the same length, which is what this core does.

## Decision

No change is proposed. Lengthening the alternate line touches the one comparison every other timing in the module hangs off, and the feature models an NTSC line of 227.5 colour clocks on average — 228 and 227 alternately — which this core does not claim to support anywhere. The document describes a behaviour; the bit that selects it is implemented and visible to software, and the behaviour itself is not.

The cheap half of a fix is Copperline's: suppressing the toggle under VARBEAMEN is a condition, where lengthening the line is a change to the comparison. The entry does not license it (see *Risks and unknowns*).

