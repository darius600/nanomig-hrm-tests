# NM-HRM-011 — Table 3-13 blanks 29 PAL lines, p. 219 ends the blank at line 25, and no implementation reaches 29

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The manual ends the PAL vertical blank in two places: Table 3-13 (p. 79) stops it at $1D (29) and computes 283 displayable lines from 29, while p. 219 ends the range at line 25. NanoMig blanks PAL lines 0 to 25, p. 219 read inclusively, the reading under which p. 219 also gives Table 3-13's NTSC figure; no independent implementation blanks a PAL line past 25. The entry exists so the next reader does not take the red for an implementation fault; whether Table 3-13's PAL row is a misprint is for an Amiga specialist to say. |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L89-L90) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L89-L90) |
| **Report to** | **nobody, as an RTL defect.** The question is Table 3-13's PAL row; the RTL's PAL blank ends on the line p. 219 prints, and both Minimig lines carry the same two constants |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) · [`hrm/ch7-vertb-vertical-blank-timing`](../../hrm/ch7-vertb-vertical-blank-timing) |
| **Chapter** | 3 — Playfield, and 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 3 red: `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` against Table 3-13 in `hrm/ch3-beam-counters-and-blanking` (`28 passed, 5 failed`; the other four reds are NM-HRM-010's and NM-HRM-068's), and `NTSC-VBLANK-DURATION-20-LINES` and `PAL-VBLANK-DURATION-25-LINES` against p. 219 in `hrm/ch7-vertb-vertical-blank-timing` (`8 passed, 2 failed`) |
| **Verdict of the four** | **unanimous against Table 3-13's PAL row**: WinUAE and vAmiga end the hardwired PAL blank after line 25, as NanoMig does, Copperline after line 24 and Denise after line 23; none blanks to the $1D the table prints. In NTSC WinUAE ends it after line 20, as NanoMig does, vAmiga and Copperline after line 19, Denise after line 18 |

---

## What the HRM says

**Chapter 3, printed p. 79**, introduces Table 3-13:

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
312 − 29 = 283 per field, and twice the stop value per interlaced frame.
Lines 0 to 20 are blanked in NTSC and lines 0 to 28 in PAL.

**Chapter 7, printed p. 219**, gives the range again, in the section on the
vertical blanking interrupt:

> The minimum time of vertical blanking is 20 horizontal scan lines for an
> NTSC system and 25 horizontal scan lines for a PAL system. The range starts
> at line 0 and ends at line 20 for NTSC or line 25 for PAL. After the minimum
> vertical blanking range, you can control where the display actually starts
> by using the DIWSTRT (display window start) register to extend the effective
> vertical blanking time.

**Chapter 8, printed p. 239**, names the end once more, in NTSC's numbers,
and gives no PAL figure:

> This freeze is released at the end of internal vertical blanking (vertical
> position 20).

**The pages disagree about PAL.** Read as a count, p. 219's first sentence
gives 25 PAL lines; read inclusively, its second gives 26, lines 0 to 25.
Table 3-13 blanks 29. For NTSC the table's 21 is p. 219's second sentence read
inclusively, lines 0 to 20, and only the count sentence gives 20. For PAL no
reading of p. 219 reaches 29: p. 219 hands the display start to DIWSTRT after
line 25, and Table 3-13 allows no data before line 29.

---

## What NanoMig does

**Lines 0 to 25 are blanked in PAL, 0 to 20 in NTSC**: p. 219 read
inclusively, in both standards.

[`src/minimig-aga/agnus_beamcounter.v#L89-L90`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L89-L90)

```verilog
parameter VBSTOP_PAL_VAL  = 9'd25;           // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
parameter VBSTOP_NTSC_VAL = 9'd20;           // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
```

Unless ECS's VARVBEN and VARBEAMEN are both set, `vbstop` is one of these two
constants — [`#L233`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L233): `varvben  && varbeamen ? vbstop_reg  : pal ? VBSTOP_PAL_VAL : VBSTOP_NTSC_VAL` — and the blank holds through that line inclusive — [`#L444`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L444): `assign vbl = (vpos <= vbstop);`

The module's own comment names the first line after the blank in each
standard — [`#L99`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L99): `//first visible line $1A (PAL) or $15 (NTSC)`. The NTSC
one is the $15 Table 3-13 prints as the NTSC stop; the PAL one is three lines
before the table's $1D.

| | NanoMig | p. 219 as a count | p. 219 read inclusively | Table 3-13 |
|---|---|---|---|---|
| NTSC | 21 | 20 | 21 | 21 (stop `$15`) |
| PAL | **26** | 25 | 26 | **29** (stop `$1D`) |

### Measured

[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking)
reads `vbl` and counts consecutive blanked lines from `vpos=0`: 21 under
NTSC, matching Table 3-13 — `VBLANK-DURATION-NTSC-MATCHES-MANUAL`, green —
and 26 under PAL, three short of the table's 29 —
`VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`, red. The package reads
`28 passed, 5 failed`; the other four reds are [NM-HRM-010](NM-HRM-010.md)'s
and [NM-HRM-068](NM-HRM-068.md)'s.

[`hrm/ch7-vertb-vertical-blank-timing`](../../hrm/ch7-vertb-vertical-blank-timing)
measures the same counts against p. 219's count sentence: 21 lines where it
gives 20 — `NTSC-VBLANK-DURATION-20-LINES`, red — and 26 where it gives 25 —
`PAL-VBLANK-DURATION-25-LINES`, red. What both readings of p. 219 agree on is
green: blanking holds at line 0 and has ended by line 21 in NTSC and by line
26 in PAL — `NTSC-VBLANK-ENDS-BY-LINE-21`, `PAL-VBLANK-ENDS-BY-LINE-26`.
Lines 20 and 25 themselves are recorded, `vbl=1` on both, and not graded. The
package reads `8 passed, 2 failed`.

**The two packages grade the two pages, and no implementation can be green in
both.** `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` needs line 26 blanked,
`PAL-VBLANK-ENDS-BY-LINE-26` needs it clear; an implementation that blanks
Table 3-13's 29 PAL lines turns the second red. In NTSC the first package's
21 and the second's 20 exclude each other the same way.

---

## What the others say

The question put to each: on which line does the hardwired vertical blank end?

| | |
|---|---|
| **WinUAE** | [`custom.cpp:10087-10090`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10087-L10090) (PAL), [`:10111-10114`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10111-L10114) (NTSC), [`:1229-1246`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1229-L1246), [`include/custom.h:121-123`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L121-L123): the blank ends on `if (vpos == 25) {` in PAL and `if (vpos == 20) {` in NTSC, which set `agnus_vb = 0;` and `agnus_vb_end_line = true;`. That line still sends Denise the vertical-blank strobe, `(agnus_vb == 1 \|\| agnus_vb_end_line)` selecting `strobe = 0x03a;`, and the header names the next line `// following endlines = first visible line`: `#define VBLANK_ENDLINE_PAL 26`, `#define VBLANK_ENDLINE_NTSC 21`. Last blanked line 25 in PAL and 20 in NTSC — **the same lines as NanoMig in both standards**. PAL−NTSC gap **five lines** |
| **vAmiga** | [`Core/Components/Agnus/Agnus.h:429`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L429), [`Core/Components/Denise/Denise.cpp:1644-1645`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1644-L1645), [`Core/Components/Agnus/Agnus.cpp:853-854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L853-L854): `bool inVBlankArea(isize posv) const { return posv < (isPAL() ? 26 : 20); }`. Denise translates no bitplane data inside it, `if (!agnus.inVBlankArea(vpos) && !frameSkips) {`, and Agnus arms no sprite DMA, `if (!inVBlankArea(v)) {`. Last blanked line 25 in PAL, as NanoMig, and 19 in NTSC. PAL−NTSC gap **six lines** |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:449-453`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L449-L453), [`:509-515`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L509-L515), [`events.cpp:381-397`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L381-L397): with the hardwired blank selected, `if (vPos == (ntsc ? 19 : 24)) {` clears `vBlank = false;` in the line counter's update at `case 2:`, `updateVCounter();`, one cycle before the refresh slot that issues the line's strobe, `case 3: refreshCycle<0>(); break;`; the strobe sends `denise.strvbl();` only while `vBlank && !vBlankStart`, and `denise.strhor();` otherwise. Last blanked line 23 in PAL and 18 in NTSC. PAL−NTSC gap **five lines, like NanoMig's** |
| **Copperline** | [`src/bus.rs:8655-8666`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L8655-L8666), [`:8631-8633`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L8631-L8633), [`:263-264`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L263-L264), [`:10518-10525`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L10518-L10525), [`src/bus/frame_capture.rs:652-654`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/frame_capture.rs#L652-L654), [`src/bus/tests.rs:204`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L204): the fixed blank beside the programmable one, *"The fixed blank begins on the final field line, wraps through line zero, and clears at the first display-DMA line (PAL $19, NTSC $14)"*, as `VideoStandard::Pal => 25` and `VideoStandard::Ntsc => 20` in `vpos < stop \|\| vpos.saturating_add(1) >= self.agnus.current_frame_lines()`. That function sets the per-line marker of Copperline's bus trace, `events \|= BUS_EVENT_VB;`, and its own test asserts PAL line 25 carries none: `assert_eq!(pal.beam_trace_events_at(25, 1) & BUS_EVENT_VB, 0);`. Sprite DMA is inhibited on the same lines, `vpos < sprite_dma_first_active_vpos(self.agnus.video_standard())` with `const PAL_SPRITE_DMA_FIRST_ACTIVE_VPOS: u32 = 0x19;`: *"Hard vertical blank inhibits sprite DMA near the top of a standard field. The first line after that blank is one line earlier than bitplane DMA: PAL line $19 and NTSC line $14."* Last blanked line 24 in PAL and 19 in NTSC. PAL−NTSC gap **five lines, like NanoMig's** |
| **MiSTer** | descendant — [`rtl/agnus_beamcounter.v:89-90`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L89-L90), [`:430`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L430) — identical to NanoMig: `VBSTOP_PAL_VAL = 9'd25`, `VBSTOP_NTSC_VAL = 9'd20`, and `assign vbl = (vpos <= vbstop);` |
| **TC64** | descendant — [`rtl/minimig/agnus_beamcounter.v:105-106`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L105-L106), [`:472-479`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L472-L479) — the same two values (as `11'd25`, `11'd20`; the constant is wider, not the number), the same comment. The blank is a register, cleared by `else if (vpos == vbstop+1)` one clock into line 26 in PAL and line 21 in NTSC: the same lines, a clock later. If Table 3-13's 29 is upheld, a fix diverges from both lines equally |

| | last blanked PAL line | last blanked NTSC line | PAL − NTSC |
|---|---:|---:|---:|
| Table 3-13 | 28 | 20 | 8 |
| p. 219, *"ends at line"* | 25 | 20 | 5 |
| NanoMig | 25 | 20 | 5 |
| WinUAE | 25 | 20 | 5 |
| vAmiga | 25 | 19 | 6 |
| Copperline | 24 | 19 | 5 |
| Denise | 23 | 18 | 5 |

**None of the four blanks a PAL line past 25, the line p. 219 names, and none
reaches Table 3-13's $1D.** WinUAE ends the blank on NanoMig's lines in both
standards; vAmiga on NanoMig's PAL line and one NTSC line earlier; Copperline
and Denise one and two lines earlier in both. The manual's PAL−NTSC gap is
**eight** lines, 29 − 21; the implementations' is five, or six in vAmiga, and
p. 219's is five.

---

## Why this is filed against the page

The manual gives the end of the PAL vertical blank twice, and the two
disagree. Table 3-13's NTSC row agrees with p. 219 read inclusively, lines 0 to
20; its PAL row agrees with no reading of p. 219. NanoMig takes p. 219's
inclusive reading in both standards, so it matches Table 3-13 in NTSC and
p. 219 in PAL, and all four independent implementations end the PAL blank on
or before the line p. 219 names. Nothing read here blanks a PAL line to
Table 3-13's $1D.

The finding key names the pair that disagrees, NanoMig and Table 3-13, and
does not prejudge which side is wrong.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

---

## Suggested change

Not an RTL change. Somebody with a PAL machine, or with the PAL Agnus's
documentation, saying on which line the hardwired vertical blank ends. If it
is line 25 or earlier, Table 3-13's PAL row is a misprint and
`VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` grades a number p. 219
contradicts; if it is line 28, p. 219's PAL figure is the misprint and
NanoMig, both Minimig lines and all four implementations end the blank three
or more lines early.

---

## What holds the finding up

**The NTSC twin passes on the same measurement basis.** `VBLANK-DURATION-NTSC-MATCHES-MANUAL`
matches Table 3-13's 21 exactly, on the same fixture, the same counter and the
same counting convention. That is what makes the PAL red survive the units-error
objection: if the test were counting lines wrongly, NTSC would be wrong too.

---

## For the specialists

1. **Could Table 3-13 mean something other than Agnus's hardwired blank?** It
   is titled *Maximum Allowable Vertical Screen Video*, introduced with *"No
   data can be displayed in the vertical blanking area"*, and calls $1D the
   *Vertical Blank Stop*. The page gives no second PAL figure and does not say
   where its 29 comes from.
2. **In NTSC the pages differ by one line, in PAL by three or four.** Table
   3-13's $15 and p. 219's line 20 read inclusively both give NanoMig's 21
   NTSC lines; p. 219's count sentence gives 20, and p. 239's *"vertical
   position 20"* does not say whether line 20 is still blanked. Table 3-13's
   29 PAL lines are three more than p. 219 read inclusively, four more than
   its count.
