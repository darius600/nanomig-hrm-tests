# NM-HRM-090 — with VARVBEN set, VBSTRT starts no vertical blank: the line it names still shows playfield data, where pp. 304 and 305 make it the line at which blanking starts [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should vertical blanking start at the line VBSTRT names when BEAMCON0 has VARVBEN set and VARBEAMEN clear, where `agnus_beamcounter.v` never stores VBSTRT, so the line it names ($A0 and $B0 in the package) still shows playfield data? It closes when `agnus_beamcounter.v` stores VBSTRT and starts the programmed blank from it with VARVBEN set and the five reds `P5-MAIN` V1 S1, V1 S2, V2 S1, V2 S2 and the P5 aggregate (`opus:B-vbstrt.addr`) turn green, or when an Amiga specialist rejects the reading, in which case, if that means pp. 293, 304 and 305 are wrong, this page is refiled against the printed page |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | ECS Agnus (the harness is configured ECS, `P5-d`), PAL and NTSC, BEAMCON0 = $1020 (PAL) or $1000 (NTSC): VARVBEN set, HARDDIS and VARBEAMEN clear; VBSTRT naming the lines $A0 and $B0, one low-resolution plane. VARBEAMEN set and lines of $100 and above are not measured. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C — Enhanced Chip Set, printed pp. 298, 304, 305; Appendix B, printed p. 293; 3 — Playfield Hardware, printed p. 79 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims, each with the assertion that carries it: the P5 aggregate: `opus:B-vbstrt.addr`, p. 293 (its message cites pp. 293, 304, 305 and 79). Declined whole, no assertion: `opus:C-vbstrt.c01` and `sol:C-scan.c20`, p. 304 (the quotation also names the register's write-only access and the chip that holds it). `P5-MAIN`, `P5-a`, `P5-b`, `P5-c` and `CAL-VIDEO` carry no claim id; pp. 298 and 305 are quoted as context |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L429-L444) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L415-L430); inherited (TC64 stores VBSTRT and starts a blank from it, but only when VARBEAMEN is set as well) |
| **Test** | [`integration/ch3-playfield-system`](../../integration/ch3-playfield-system): `make -k -C integration/ch3-playfield-system test`, or `make -k -C integration/ch3-playfield-system test-fx68k` and `make -k -C integration/ch3-playfield-system test-tg68k`; an expected red exits nonzero |
| **Assertions** | `integration/ch3-playfield-system`, 5 red of this finding's on fx68k and 5 on TG68K, in a package run of 223 passed, 10 failed on each core: `P5-MAIN` V1 S1, V2 S1, V1 S2 and V2 S2 (VBSTRT for line $A0 in V1, $B0 in V2; ECS/PAL in S1, ECS/NTSC in S2) — expected every beam line showing C2 to be preceded by a line showing C1 and followed by a line showing none of C1, C2 and C3, at least three such lines, one field apart, observed that this does not hold; and the P5 aggregate, `opus:B-vbstrt.addr` — expected the line before the line VBSTRT names to show playfield data and the line it names not to, observed the same failure (pp. 293, 304, 305, 79). The other five reds, `P3-MAIN` in S1 to S4 and the P3 aggregate, are the package's light-pen freeze lines (BPLCON0 bit 3 set), [NM-HRM-087](NM-HRM-087.md)'s subject. Green beside them in all four runs: `P5-a` (data shown in lines N-10 to N-2), `P5-b` (the Copper's colour changes anchored in lines N-1 and N+1 by the beam counter), `P5-c` (field length) and `CAL-VIDEO` |
| **Verdict of the four** | **split**: three of the four start a programmed vertical blank from VBSTRT with VARVBEN set and VARBEAMEN clear, vAmiga does not. Copperline blanks from the line VBSTRT names, as the package reads p. 304; WinUAE and Denise set their blank on that line but send the display chip the blanking strobe only from the next line, so the line VBSTRT names still shows playfield data there. vAmiga stores VBSTRT and acts on nothing, as NanoMig does. MiSTer, a descendant, has NanoMig's commented-out lines; TC64, a descendant, starts a blank from VBSTRT only when VARBEAMEN is set too. Hardware: not measured |
| **Effort** | M — `agnus_beamcounter.v` only: store VBSTRT (register, reset value, write), a flip-flop that starts the blank on it, `vbstop` taken with VARVBEN alone (line 233) and the select between the programmed window and the hardwired blank at `assign vbl` |
| **See also** | [NM-HRM-011](NM-HRM-011.md) — the hardwired end of the same blank |

---

## Source requirement

**Appendix C, printed p. 304**, introduces the register:

> Programmable blanking is implemented through four new ECS Agnus registers:

> HBSTRT    W   A     Horizontal line position for HBLANK start
> HBSTOP    W   A     Horizontal line position for HBLANK stop
> VBSTRT    W   A     Vertical   line position for VBLANK start
> VBSTOP    W   A     Vertical   line position for VBLANK stop

**Printed p. 305**, BEAMCON0, says which bit puts it to use:

> 14    HARDDIS    Disable hardwired vertical/horizontal blank
> ...
> 12    VARVBEN    Use VBSTRT/STOP disable hard window stop
> ...
> 7     VARBEAMEN  Variable beam counter comparator enable

and warns, about BEAMCON0 itself:

> Other modes, such as VGA (31 KHz programmable mode) disable the hard stops on display sync and blank. Do not write to this register.

**Appendix B, printed p. 293**, and **Appendix C, printed p. 298**, give the address and mark which of the new registers depend on VARBEAMEN. p. 298:

> 1C8  VTOTAL   new   W  A   Highest numbered vertical line  (VARBEAMEN=1)
> 1CA  VSSTOP   new   W  A   Vertical line position for VSYNC stop
> 1CC  VBSTRT   new   W  A   Vertical line for VBLANK start
> 1CE  VBSTOP   new   W  A   Vertical line for VBLANK stop

p. 293 prints the same rows, among them:

> VBSTRT        1CC   W     A (E)    Vertical line for VBLANK start

On both pages HTOTAL and VTOTAL carry *"(VARBEAMEN=1)"* and VBSTRT and VBSTOP do not.

**Chapter 3, printed p. 79**, says what vertical blanking does to a playfield:

> Vertically, the restrictions are simple. No data can be displayed in the vertical blanking area.

## RTL and measurement

**The register is not stored.** The write, the register and the selection of a programmed start are comments — [`src/minimig-aga/agnus_beamcounter.v#L187`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L187), [`#L202`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L202), [`#L215`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L215), [`#L232`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L232): `//reg [10:0] vbstrt_reg;` and `//VBSTRT [8:1] : vbstrt_reg  <= {data_in[10:0]};` among them.

**Vertical blanking always starts at line 0.** The flip-flop that would start it at VBSTRT is commented out with a `// TODO`, and what remains blanks every line up to `vbstop` — [`#L429-L444`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L429-L444):

```verilog
/*
...
  else if (vpos == vbstrt)
    vbl_reg <= 1'b1;
...
assign vbl = vbl_reg; // TODO
*/

assign vbl = (vpos <= vbstop);
```

`vbstop` is VBSTOP only when VARBEAMEN is set as well, `wire [10:0] vbstop = varvben  && varbeamen ? vbstop_reg  : pal ? VBSTOP_PAL_VAL : VBSTOP_NTSC_VAL;` ([`#L233`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L233)), so with the package's BEAMCON0 the blank is the hardwired lines 0 to 25 (PAL) or 0 to 20 (NTSC), [NM-HRM-011](NM-HRM-011.md)'s. Lines $A0 and $B0 lie outside it, so by the code neither line N nor line N+1 is blank on NanoMig. `vblank <= vbl;` at the end of horizontal blanking ([`#L450-L459`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L450-L459)) reaches the RGB output through `minimig.v`'s `vblank` port, where `nanomig.v` zeroes the colour, `wire [3:0] r_in = (hbl||vbl)?4'h0:red[7:4];` ([`src/nanomig.v#L1372-L1376`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/nanomig.v#L1372-L1376), [`#L1577-L1578`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/nanomig.v#L1577-L1578)).

**Of VARVBEN, only the second half is wired.** BEAMCON0 is stored under ECS, and `wire varvben = beamcon0_reg[12];` ([`#L119-L130`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L119-L130)) reaches `assign harddis_out = harddis || varbeamen || varvben;` ([`#L236`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L236)), which lifts the hardwired limits of the display fetch ([`agnus_bitplanedma.v#L410`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L410)), p. 305's *"disable hard window stop"*. Its first half, *"Use VBSTRT/STOP"*, reaches nothing.

### Measured

[`integration/ch3-playfield-system`](../../integration/ch3-playfield-system) P5 shows one low-resolution plane, every word $FFFF, in a window from line $2C to line $F0, with COLOR01 = C1. The Copper sets COLOR01 to C2 at `WAIT(N-1,$30)` and to C3 at `WAIT(N+1,$30)`, so line N-1 is the only line in C2 if line N is blank; the CPU reads VPOSR/VHPOSR around both Copper MOVEs to place them on lines N-1 and N+1. The test counts beam lines between hsync pulses of the video output and prints, for the first graded field, which of C1, C2 and C3 each of the lines N-2 to N+8 shows (`MEASURED: P5`). The package grades line N; lines N+1 to N+7 are not graded. In every configuration, identical on fx68k and TG68K; V1 S1 shown, the three other `P5-MAIN` lines read the same with their tags:

```text
PASS: P5-a V1 S1: beam lines N-10..N-2 each show C1
PASS: P5-b V1 S1: the VPOSR/VHPOSR reads bracketing the Copper's COLOR01 change show V=N-1 (before/at the C2 change) and V=N+1 (before/at the C3 change)
PASS: P5-c V1 S1: consecutive detection markers and consecutive vsync assertions recur at the configured standard's field length
FAIL: P5-MAIN V1 S1: every beam line showing C2 is immediately preceded by a line showing C1 (not C2) and immediately followed by a line showing none of C1/C2/C3; at least 3 such lines, one field apart
MEASURED: P5 V1 S1: first graded field, lines N-2..N+8 (N = the line VBSTRT names): N-2=C1 N-1=C2 N=C2 N+1=C3 N+2=C3 N+3=C3 N+4=C3 N+5=C3 N+6=C3 N+7=C3 N+8=C3
FAIL: opus:B-vbstrt.addr p293 p304 p305 p79 P5 aggregate: with VARVBEN set the line before the line VBSTRT names shows playfield data and the line it names does not, for VBSTRT=$A0 and $B0, in PAL and NTSC
```

Line N shows C2, so it shows playfield data; every line from N-2 to N+8 shows one of the three colours, so none is blank. `test_ch3_playfield_system.cpp:1861-1864` grades `P5-MAIN`, `:1867-1872` prints the line above and `:1876-1880` grades the aggregate.

## Risks and unknowns

- **Which line is the first blank one?** Copperline blanks the line VBSTRT names; WinUAE and Denise, whose hardwired blank starts on the field's last line and shows from line 0, show a programmed blank from the line after VBSTRT. On an ECS machine: VARVBEN set, VBSTRT = $A0, a Copper colour change on line $9F; whether line $A0 shows the playfield decides between them, and decides whether `P5-MAIN` grades the right line. On NanoMig the red does not depend on it, as no line from $A0 on is blank by the code.
- **Does VARVBEN need VARBEAMEN?** p. 305 says no, and pp. 293 and 298 mark only HTOTAL and VTOTAL with *"(VARBEAMEN=1)"*; NanoMig requires it for VBSTOP and TC64 for both registers. WinUAE, Denise and Copperline do not.
- **Is the hardwired blank still in force with VARVBEN set?** p. 305 gives HARDDIS its own bit, *"Disable hardwired vertical/horizontal blank"*; the three implementations that act on VBSTRT drop the hardwired blank as soon as VARVBEN is set.
- **The pages leave the bit layout open.** p. 304 prints one for HTOTAL and none for VBSTRT or VBSTOP, so the package writes each line in the low bits and in the high byte alike: VBSTRT = $A0A0 and $B0B0, VBSTOP = $A8A8 and $B8B8 ([the P5 program](../../integration/ch3-playfield-system/prog_p5_core.s), lines 21–29, 54–55). Only lines below $100 are used.
- **The condition.** p. 305 ties VBSTRT to VARVBEN; neither p. 293 nor p. 298 ties it to VARBEAMEN. The program writes BEAMCON0 = $1020 in PAL and $1000 in NTSC (lines 70–74).
- **WinUAE's and Denise's line N+1 and TC64's VARBEAMEN condition are read from the code, not run.**

## Independent evidence

The question put to each: with BEAMCON0 = $1020 or $1000 (VARVBEN set, VARBEAMEN and HARDDIS clear) and VBSTRT naming line N, which is the first line that shows no playfield data?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7465-7472`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7465-L7472), [`:10024-10051`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10024-L10051), [`:1224-1249`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1224-L1249), [`:9746-9751`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9746-L9751), [`:3346-3351`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3346-L3351), [`:482-485`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L482-L485), [`:499`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L499) | <pre>bool vb = (beamcon0 &amp; BEAMCON0_VARVBEN) ? (agnus_pvb \|\| agnus_pvb_end_line) &amp;&amp; !agnus_pvb_start_line : (agnus_vb == 1 \|\| agnus_vb_end_line);</pre> **N+1.** A write stores the line, `vbstrt = value & (MAXVPOS_LINES_ECS - 1);`, and marks the programmed registers in use. With VARVBEN set, Agnus's vertical blank is the programmed one, `agnus_vb_active = agnus_pvb;`, and it starts on the line VBSTRT names, `if (vpos == vbstrt) {` / `agnus_pvb = true;` / `agnus_pvb_start_line = true;`, without VARBEAMEN. The strobe Agnus sends the display chip in the first refresh slot of each line, `uae_u16 reg = get_strobe_reg(0);`, is the blanking strobe only where the expression above is true, which excludes the start line; that line is sent `strobe = 0x03c;`, the next `strobe = 0x03a;`. So line N still shows playfield data, and the blanking strobe starts with line N+1. The same shape gives the hardwired blank: WinUAE's own table puts the hardwired VBSTRT on the field's last line, `261  VC261 ([VBSTRT][VTOTAL] LOF=0, NTSC short frame)`, *"Vertical blank = VC312 + HC1 to RVB + HC1"*, and blanks from line 0 |
| **vAmiga** | [`Core/Components/Agnus/AgnusRegs.cpp:549-568`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L549-L568), [`:616-621`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L616-L621), [`Core/Components/Agnus/Agnus.h:136-146`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L136-L146), [`:429`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L429) | <pre>/* ECS registers that are recognized and stored, but not yet acted upon.<br> * Real behavior (programmable horizontal/vertical blanking and sync) is<br> * not implemented; a write only updates the shadow value below.<br> */</pre> **none, as in NanoMig.** `pokeVBSTRT` stores the value, `vbstrt = value;`, and nothing reads it; `pokeBEAMCON0` acts on PAL and LOLDIS only, VARVBEN appearing only in its comment, `// 12: VARVBEN`. Its vertical blank test is fixed, `bool inVBlankArea(isize posv) const { return posv < (isPAL() ? 26 : 20); }` |
| **Denise** | [`emulation/libami/agnus/register.cpp:661-667`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L661-L667), [`agnus.cpp:401-403`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L401-L403), [`:438-458`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L438-L458), [`events.cpp:381-389`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L381-L389), [`blanking.cpp:133-148`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blanking.cpp#L133-L148) | <pre>if (beamCon &amp; VARVBEN) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (vPos == vBStrt) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;vBlank = true;<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;vBlankStart = true;</pre> **N+1.** An ECS write stores the line, `vBStrt = value & 0x7ff;`, and with VARVBEN, without VARBEAMEN, the blank starts on the line it names. That line carries `vBlankStart`, which lasts one line, `if (vBlankStart)` / `vBlankStart = false;`, and both the strobe, `} else if (vBlank && !vBlankStart ) {` / `denise.strvbl();`, and the frame's line accounting, `bool _vblank = vBlank && !vBlankStart;`, leave it out. Line N shows playfield data, and the blanking strobe starts with line N+1. The hardwired blank has the same shape: it starts on the field's last line, `if ( (vPos == (lines + lof)) && (model != OCS_A1000) ) {`, and blanks from line 0 |
| **Copperline** | [`src/chipset/agnus.rs:915-928`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L915-L928), [`:1427-1431`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1427-L1431), [`src/video/bitplane.rs:5841-5850`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L5841-L5850), [`:5918-5936`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L5918-L5936), [`src/bus.rs:8655-8667`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L8655-L8667), test [`src/video/bitplane/tests.rs:220-260`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L220-L260) | <pre>fn blank_window_contains(pos: u32, strt: u32, stop: u32) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;if strt &lt; stop {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;pos &gt;= strt &amp;&amp; pos &lt; stop</pre> **N, as the package reads p. 304.** An ECS write stores the line, `self.vbstrt = val & 0x07FF;`, and with VARVBEN, without VARBEAMEN, the blank window is (VBSTRT, VBSTOP), under the comment *"blank asserts on the VBSTRT line and clears on the VBSTOP line"*; every row in it is painted black, and the beam's vertical-blank state takes that window in place of the fixed one, `if let Some((start, stop)) = self.agnus.programmable_vertical_blank() {`. Its own test `programmable_blanking_blanks_vbstrt_vbstop_rows_under_varvben` writes VBSTRT = $50 and VBSTOP = $58 and asserts *"line before VBSTRT untouched"*, *"VBSTRT line blanked"* and *"VBSTOP line shows again"* |
| **MiSTer** | [`rtl/agnus_beamcounter.v:193`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L193), [`:221`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L221), [`:238-239`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L238-L239), [`:415-430`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L415-L430) | descendant — the same commented-out register, write and flip-flop, `//VBSTRT [8:1] : vbstrt_reg  <= {data_in[10:0]};` and `assign vbl = vbl_reg; // TODO` inside a comment, and the same `assign vbl = (vpos <= vbstop);` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:253`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L253), [`:301-302`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L301-L302), [`:467-479`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L467-L479) | descendant, **and changed**: VBSTRT is stored, `VBSTRT [8:1] : vbstrt_reg  <= #1 {data_in[10:0]};`, and a flip-flop starts the blank on it, `else if (vpos == vbstrt)` / `vbl_reg <= #1 1'b1;`, `assign vbl = vbl_reg; // TODO`. But the register is selected only with VARBEAMEN, `assign vbstrt = varvben && varbeamen ? (displaydual ? vbstrt_sh : vbstrt_reg) : (track_vsync && lace) ? vtotal : VBSTRT_VAL;`, so with the package's BEAMCON0 the blank still starts at the hardwired line |

## Decision

**One option: store VBSTRT, and with VARVBEN set let the blank run from VBSTRT to VBSTOP.** In `agnus_beamcounter.v`:

```verilog
-//reg [10:0] vbstrt_reg;
+reg [10:0] vbstrt_reg;
 ...
-			//vbstrt_reg  <= VBSTRT_VAL[10:0];
+			vbstrt_reg  <= 11'h7FF;
 ...
-				//VBSTRT [8:1] : vbstrt_reg  <= {data_in[10:0]};
+				VBSTRT [8:1] : vbstrt_reg  <= {data_in[10:0]};
 ...
-wire [10:0] vbstop  = varvben  && varbeamen ? vbstop_reg  : pal ? VBSTOP_PAL_VAL : VBSTOP_NTSC_VAL;
+wire [10:0] vbstop  = varvben ? vbstop_reg  : pal ? VBSTOP_PAL_VAL : VBSTOP_NTSC_VAL;
 ...
-assign vbl = (vpos <= vbstop);
+reg vbl_ff;
+always @(posedge clk) begin
+	if (clk7_en) begin
+		if (reset) vbl_ff <= 1'b0;
+		else if (vpos == vbstrt_reg) vbl_ff <= 1'b1;
+		else if (vpos == vbstop) vbl_ff <= 1'b0;
+	end
+end
+assign vbl = varvben ? vbl_ff : (vpos <= vbstop);
```

The reset value $7FF names no line, so nothing changes until software writes VBSTRT; with VARVBEN clear nothing changes at all. By the code, not run by `make test`:

- with VARVBEN set, the programmed window replaces the hardwired blank of lines 0 to 25 (PAL) or 0 to 20 (NTSC), as it does in WinUAE, Denise and Copperline, and a program that sets VARVBEN without writing VBSTRT and VBSTOP gets no vertical blank at all;
- dropping `varbeamen` from line 233 makes VBSTOP take effect with VARVBEN alone, as p. 305 has it; line 233 also selects the hardwired stop value [NM-HRM-011](NM-HRM-011.md) is about;
- `vbl` also stops sprite DMA, `if (vbl || ({ecs&vstop[9],vstop[8:0]}==vpos[9:0]))` / `dmastate_in = 0;` ([`agnus_spritedma.v#L242-L243`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_spritedma.v#L242-L243)), so sprites would stop inside the programmed window too;
- whether the first blank line is N, as here and in Copperline, or N+1, as in WinUAE and Denise, is the first question under *Risks and unknowns*; moving it is a one-line change to the flip-flop.
