# NM-HRM-011 — Table 3-13 blanks 29 PAL lines, p. 219 ends the blank at line 25, and no implementation reaches 29

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. Is Table 3-13's PAL *Vertical Blank Stop* `$1D` (29) a misprint for p. 219's PAL range, lines 0 to 25, which is the range NanoMig blanks? Closes when an Amiga specialist records that Table 3-13's PAL row is the misprint, or confirms that the hardwired PAL blank ends at line 28, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: an issue on this repository whose title begins with NM-HRM-011 |
| **Scope** | Agnus's hardwired vertical blank (the OCS fixtures, `ecs=0`), PAL and NTSC; the ECS programmable blank (VARVBEN with VARBEAMEN) is not graded. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 3 — Playfield, printed p. 79, Table 3-13; 7 — System Control, printed p. 219; 8 — Interface Hardware, printed p. 239 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (30)</summary> `VBLANK-DURATION-NTSC-MATCHES-MANUAL` (p. 79) `sol:3-max-window.c1`, `sol:3-max-window.c2`, `sol:3-max-window.c4`, `sol:3-max-window.c6`, `sol:3-max-window.c7`, `opus:3-t13.c01`, `opus:3-t13.c03`, `opus:3-t13.c05`, `opus:3-t13.c06`, `opus:3-t13.c09`, `opus:3-max.c02`, and (p. 239) `opus:8-lpen.c40`, `opus:8-lpen.c44`; `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` (p. 79) `sol:3-max-window.c3`, `sol:3-max-window.c5`, `sol:3-max-window.c8`, `sol:3-max-window.c9`, `opus:3-t13.c02`, `opus:3-t13.c04`, `opus:3-t13.c07`, `opus:3-t13.c08`, `opus:3-t13.c10`; `NTSC-VBLANK-DURATION-20-LINES` (p. 219) `sol:7-interrupts.c35`, `opus:7-int.c32`; `PAL-VBLANK-DURATION-25-LINES` (p. 219) `sol:7-interrupts.c36`, `opus:7-int.c33`; `NTSC-VBLANK-ENDS-BY-LINE-21` (p. 219) `sol:7-interrupts.c37`, `opus:7-int.c34`; `PAL-VBLANK-ENDS-BY-LINE-26` (p. 219) `sol:7-interrupts.c38`, `opus:7-int.c35`. Every assertion this page names carries a claim id </details> |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L89-L90) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L89-L90); inherited (MiSTer has the identical lines; TC64 the same two values with the blank held in a register) |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking): `make -C hrm/ch3-beam-counters-and-blanking test`; [`hrm/ch7-vertb-vertical-blank-timing`](../../hrm/ch7-vertb-vertical-blank-timing): `make -C hrm/ch7-vertb-vertical-blank-timing test`; the expected reds exit nonzero |
| **Assertions** | 3 red. `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` (Table 3-13) — expected 29 blanked PAL lines, observed 26; the package reads `28 passed, 5 failed`, the other four reds are [NM-HRM-010](NM-HRM-010.md)'s and [NM-HRM-068](NM-HRM-068.md)'s. `NTSC-VBLANK-DURATION-20-LINES` (p. 219) — expected 20, observed 21; `PAL-VBLANK-DURATION-25-LINES` (p. 219) — expected 25, observed 26; that package reads `8 passed, 2 failed` |
| **Verdict of the four** | **unanimous against Table 3-13's PAL row**: WinUAE and vAmiga end the hardwired PAL blank after line 25, as NanoMig does, Copperline after line 24 and Denise after line 23; none blanks to the $1D the table prints. In NTSC WinUAE ends it after line 20, as NanoMig does, vAmiga and Copperline after line 19, Denise after line 18. Hardware: not measured |
| **Effort** | none; if the hardwired PAL blank is confirmed to end at line 28, S: `VBSTOP_PAL_VAL` at `agnus_beamcounter.v:89` from 25 to 28 |

---

## Source requirement

**Chapter 3, printed p. 79**:

> Vertically, the restrictions are simple. No data can be displayed in the
> vertical blanking area. The following table shows the allowable vertical
> display area.

```
                              NTSC                 PAL
     Vertical Blank Start     0                    0
      Vertical Blank Stop     $15 (21)             $1D (29)

                              NTSC     NTSC        PAL      PAL
                              Normal   Interlaced  Normal   Interlaced
        Displayable lines
          of screen video     241      483         283      567
                                       =525-(21*2)          =625-(29*2)

        Table 3-13: Maximum Allowable Vertical Screen Video
```

The table counts the stop value as blanked lines: 262 − 21 = 241 and
312 − 29 = 283 per field, so lines 0 to 20 are blanked in NTSC and lines 0 to 28
in PAL.

**Chapter 7, printed p. 219**, in the section on the vertical blanking
interrupt:

> The minimum time of vertical blanking is 20 horizontal scan lines for an
> NTSC system and 25 horizontal scan lines for a PAL system. The range starts
> at line 0 and ends at line 20 for NTSC or line 25 for PAL. After the minimum
> vertical blanking range, you can control where the display actually starts
> by using the DIWSTRT (display window start) register to extend the effective
> vertical blanking time.

**Chapter 8, printed p. 239**, names the end once more, in NTSC's numbers, and gives no PAL figure:

> This freeze is released at the end of internal vertical blanking (vertical
> position 20).

The pages disagree about PAL. Read as a count, p. 219's first sentence gives 25
PAL lines; read inclusively, its second gives 26, lines 0 to 25. Table 3-13
blanks 29. For NTSC the table's 21 is p. 219's second sentence read
inclusively, and only the count sentence gives 20. For PAL no reading of p. 219
reaches 29: p. 219 hands the display start to DIWSTRT after line 25, and
Table 3-13 allows no data before line 29.

## RTL and measurement

**Lines 0 to 25 are blanked in PAL, 0 to 20 in NTSC**, p. 219 read inclusively
in both standards:
[`agnus_beamcounter.v#L89-L90`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L89-L90)

```verilog
parameter VBSTOP_PAL_VAL  = 9'd25;           // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
parameter VBSTOP_NTSC_VAL = 9'd20;           // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
```

Unless ECS's VARVBEN and VARBEAMEN are both set, `vbstop` is one of these two
constants — [`#L233`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L233): `varvben  && varbeamen ? vbstop_reg  : pal ? VBSTOP_PAL_VAL : VBSTOP_NTSC_VAL` — and the blank holds through that line inclusive — [`#L444`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L444): `assign vbl = (vpos <= vbstop);`. The module's own comment names the first line after the blank — [`#L99`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L99): `//first visible line $1A (PAL) or $15 (NTSC)`: the NTSC one is Table 3-13's `$15`, the PAL one is three lines before its `$1D`.

| blanked lines | NanoMig | p. 219 as a count | p. 219 read inclusively | Table 3-13 |
|---|---|---|---|---|
| NTSC | 21 | 20 | 21 | 21 (stop `$15`) |
| PAL | **26** | 25 | 26 | **29** (stop `$1D`) |

[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking)
reads `vbl` and counts consecutive blanked lines from `vpos=0`: 21 under NTSC,
matching Table 3-13 — `VBLANK-DURATION-NTSC-MATCHES-MANUAL`, green — and 26
under PAL, three short of the table's 29 —
`VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`, red. The two run on the same
fixture, counter and counting convention, so a units error would show in NTSC
too.

[`hrm/ch7-vertb-vertical-blank-timing`](../../hrm/ch7-vertb-vertical-blank-timing)
measures the same counts against p. 219's count sentence: 21 lines where it
gives 20 — `NTSC-VBLANK-DURATION-20-LINES`, red — and 26 where it gives 25 —
`PAL-VBLANK-DURATION-25-LINES`, red. What both readings of p. 219 agree on is
green: blanking holds at line 0 and has ended by line 21 in NTSC and by line 26
in PAL — `NTSC-VBLANK-ENDS-BY-LINE-21`, `PAL-VBLANK-ENDS-BY-LINE-26`. Lines 20
and 25 themselves are recorded, `vbl=1` on both, and not graded.

## Risks and unknowns

- **Not measured:** a real PAL Agnus; the ECS programmable blank; the chip
  revision.
- **The two packages grade the two pages, and no implementation can be green in
  both.** `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` needs line 26 blanked,
  `PAL-VBLANK-ENDS-BY-LINE-26` needs it clear; an implementation that blanks
  Table 3-13's 29 PAL lines turns the second red. In NTSC the first package's 21
  and the second's 20 exclude each other the same way.
- **Could Table 3-13 mean something other than Agnus's hardwired blank?** It is
  titled *Maximum Allowable Vertical Screen Video*, introduced with *"No data
  can be displayed in the vertical blanking area"*, and calls `$1D` the
  *Vertical Blank Stop*. The page gives no second PAL figure and does not say
  where its 29 comes from.
- **In NTSC the pages differ by one line, in PAL by three or four.** p. 219's
  count sentence gives 20, and p. 239's *"vertical position 20"* does not say
  whether line 20 is still blanked. Table 3-13's 29 PAL lines are three more
  than p. 219 read inclusively, four more than its count.
- **The witnesses split in NTSC**, where the last blanked line is 18, 19 or 20;
  only PAL is unanimous on 25 or earlier.

## Independent evidence

The question put to each: on which line does the hardwired vertical blank end?

| | where | last blanked PAL / NTSC |
|---|---|---|
| **WinUAE** | [`custom.cpp:10087-10090`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10087-L10090) (PAL), [`:10111-10114`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10111-L10114) (NTSC): the blank ends on `if (vpos == 25) {` and `if (vpos == 20) {`, which set `agnus_vb = 0;`; [`include/custom.h:121-123`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L121-L123) names the next line `// following endlines = first visible line`: `#define VBLANK_ENDLINE_PAL 26`, `#define VBLANK_ENDLINE_NTSC 21`. NanoMig's lines in both standards | 25 / 20 |
| **vAmiga** | [`Core/Components/Agnus/Agnus.h:429`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L429): `bool inVBlankArea(isize posv) const { return posv < (isPAL() ? 26 : 20); }`; Denise translates no bitplane data inside it ([`Denise.cpp:1644-1645`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1644-L1645)) and Agnus arms no sprite DMA ([`Agnus.cpp:853-854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L853-L854)) | 25 / 19 |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:449-453`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L449-L453), [`:509-515`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L509-L515), [`events.cpp:381-397`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L381-L397): with the hardwired blank selected, `if (vPos == (ntsc ? 19 : 24)) {` clears `vBlank = false;` one cycle before the refresh slot that issues the line's strobe | 23 / 18 |
| **Copperline** | [`src/bus.rs:8655-8666`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L8655-L8666): *"The fixed blank begins on the final field line, wraps through line zero, and clears at the first display-DMA line (PAL $19, NTSC $14)"*, with `VideoStandard::Pal => 25` and `VideoStandard::Ntsc => 20` as the stop of `vpos < stop`; the bus trace marks those lines `BUS_EVENT_VB` ([`:8631-8633`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L8631-L8633)), and its own test asserts PAL line 25 carries none ([`src/bus/tests.rs:204`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L204)) | 24 / 19 |
| **MiSTer** | descendant — [`rtl/agnus_beamcounter.v:89-90`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L89-L90), [`:430`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L430): identical to NanoMig, `VBSTOP_PAL_VAL = 9'd25`, `VBSTOP_NTSC_VAL = 9'd20`, `assign vbl = (vpos <= vbstop);` | 25 / 20 |
| **TC64** | descendant — [`rtl/minimig/agnus_beamcounter.v:105-106`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L105-L106), [`:472-479`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L472-L479): the same two values (as `11'd25`, `11'd20`), the blank a register cleared by `else if (vpos == vbstop+1)`, a clock later into line 26 and line 21 | 25 / 20 |

The manual's PAL−NTSC gap is **eight** lines, 29 − 21; p. 219's is five, as
NanoMig's, WinUAE's, Copperline's and Denise's; vAmiga's is six.

## Decision

**No RTL change.** The question is Table 3-13's PAL row: somebody with a PAL
machine, or with the PAL Agnus's documentation, saying on which line the
hardwired vertical blank ends. If it is line 25 or earlier, Table 3-13's PAL row
is a misprint and `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` grades a number
p. 219 contradicts; if it is line 28, p. 219's PAL figure is the misprint and
NanoMig, both Minimig lines and all four implementations end the blank three or
more lines early.
