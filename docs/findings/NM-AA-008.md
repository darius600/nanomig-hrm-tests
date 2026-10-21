# NM-AA-008 — HHPOSR reads $0000 with DUAL set: the secondary beam counter of AA p. 24 is not there [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `agnus_beamcounter.v` give HHPOSR ($1DA) the second horizontal counter of AA p. 24, which with BEAMCON0 bit 6 (DUAL) set counts colour clocks up to HTOTAL and restarts, where NanoMig's HHPOSR reads $0000? Closes when the counter is added and `P14-U1`, `P14-U2`, `P14-REL` and `P14-AGG` are green on both cores, or when an Amiga specialist rejects the counter because the chip has none, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | HHPOSR ($1DA) read with BEAMCON0 DUAL set and VARBEAMEN clear, HTOTAL $0040 and $0070; AA chip set selected (`chipset_config` $10); chip revision unknown (the entries name none); hardware not measured. Isolated: the main counter keeps its line under DUAL (`P14-MAIN` green); bits 15-8 of HHPOSR, which no page states, are not graded; this package never writes HHPOSW |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 24 (the HHPOSR/HHPOSW and HTOTAL entries), p. 12 (BEAMCON0: DUAL, VARBEAMEN), p. 10 (the HHPOSR row). The document prints no page numbers: p. N is the PDF page · `P14-AGG`: `sol:AA-hhpos.c1`, `sol:AA-hhpos.c5`, p. 24; `P14-U1`, `P14-U2` and `P14-REL` carry no claim id |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L108-L115) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L113-L121); inherited (MiSTer has the same read path; TC64 has it too and wires bit 6 to its RTG switch) |
| **Test** | [`integration/aa-integ-s1`](../../integration/aa-integ-s1): `make -k -C integration/aa-integ-s1 test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. The AA chip set, graded on both CPU cores |
| **Assertions** | 4 red on fx68k and 4 on TG68K, in a package run of 156 passed, 10 failed, 11 NOT GRADED on each core: `P14-U1`, expected every HHPOSR bits 7-0 value from the first restart at most $40 with maximum $40, observed largest whole HHPOSR word $0000 and no restart; `P14-U2`, expected at most $70, maximum $70, values between $40 and $70, observed $0000; `P14-REL`, expected the h difference of consecutive samples to equal the elapsed colour clocks modulo HTOTAL+1, observed no restart to start from; `P14-AGG`, expected a second counter from 0 to HTOTAL, observed $0000 (it is red through `P14-U1`, `P14-U2` and `P14-REL`; `P14-MAIN` is green). The run's six other reds are not this finding's: `P4b-C1`, `P4b-C2` and `P5` grade BPLCON1's reset value ([NM-AA-006](NM-AA-006.md)), `P6-II` and `P6-AGG` BPLCON0's BYPASS bit ([NM-AA-007](NM-AA-007.md)), and `P10-LATCH` the light-pen latch ([NM-HRM-087](NM-HRM-087.md)) |
| **Verdict of the four** | **split: one of four has the counter, none bounds it by HTOTAL alone.** WinUAE reads and writes a second horizontal counter that DUAL detaches from the main one and that restarts its UHRES chain, but it restarts where the main line ends, at HTOTAL only when VARBEAMEN is set as well. vAmiga stores an HHPOSW write and decodes no HHPOSR; Denise decodes neither and lists "UHRES/DUAL stuff" among its todos; Copperline returns the last HHPOSW value from HHPOSR and logs that DUAL is not emulated. Hardware: not measured |
| **Effort** | none; if the counter is accepted, M: a second 8-bit counter in `agnus_beamcounter.v` with a read case at $1DA that returns it in bits 7-0, one step per colour clock restarting after the stored `htotal_reg`, and BEAMCON0 bit 6 selecting it |
| **See also** | [NM-AA-005](NM-AA-005.md): the long line of the same HTOTAL entry |

---

## Source requirement

**PDF p. 24** gives the two registers one entry:

```
HHPOSR   H 1DA  R   A        DUAL mode hires H beam counter read
HHPOSW   H 1D8  W   A        DUAL mode hires H beam counter write
                    This is the secondary beam counter for
                    the faster mode, triggering the UHRES pointers & doing the
                    comparisons for HBSTRT,STOP,HTOTAL,HSSTRT,HSSTOP
                    (See HTOTAL for bits)
```

and the HTOTAL entry on the same page gives the bits, the count and when HTOTAL acts:

```
HTOTAL   H 1C0  W   A        Highest color clock count in horiz line
                    Bit# 15 14 13 12 11 10 09 08 07 06 05 04 03 02 10 00
                          x  x  x  x  x  x  x  x h8 h7 h6 h5 h4 h3 h2 h1
                    (x's should be driven to 0 for upward compatibility)
                    Horiz line has this many + 1  280ns increments. If the
                    PAL bit & LOLDIS are not high, long line/short line toggle will
                    occur, and there will be this many + 2 every other line.
                    Active if VARBEAMEN=1 or DUAL=1.
```

**PDF p. 12**, BEAMCON0, gives DUAL the horizontal comparators and VARBEAMEN the main counter:

```
                    DUAL= Run the horizontal comparators with the alternate
                     horizontal beam counter, and starts the UHRES pointer chain
                     with the reset of this counter rather than the normal one.
                     This allows the UHRES pointers to come out more than once
                     in a horizontal line, assuming there is some memory
                     bandwidth left (It doesn't work in 640*400*4 interlace mode)
                     Also, to keep the 2 displays synced, the horizontal line
                     lengths should be multiples of each other. If you are
                     amazingly clever, you might not need to do this.
```

```
                    VARBEAMEN= Enables the variable beam counter comparators
                     to operate (allowing different beam counter total values)
                     on the main horiz counter. It also disables hard display
                     stops on both horizontal & vertical.
```

**PDF p. 10**, the address list, gives the read row:

```
HHPOSR     h  1DA  R    A       DUAL mode hires H beam counter read
```

Read together: with DUAL set, HHPOSR reads a second horizontal counter, it counts the 280 ns steps HTOTAL counts, and HTOTAL is its highest count. The main counter keeps its line. None of these pages says what HHPOSR returns with DUAL clear, or in bits 15-8, where HTOTAL's bits are `x`.

## RTL and measurement

**No register at $1DA exists.** The beam counter's register list stops at the ECS registers ([`agnus_beamcounter.v#L60-L76`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L60-L76)) and no file under `src/` decodes $1DA or $1D8. The read multiplexer has two cases and a default of 0 ([`#L108-L115`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L108-L115)), so a read of $1DA returns $0000:

```verilog
always @(*) begin
	if (reg_address_in[8:1]==VPOSR[8:1] || reg_address_in[8:1]==VPOSW[8:1])
		data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
	else if (reg_address_in[8:1]==VHPOSR[8:1] || reg_address_in[8:1]==VHPOSW[8:1])
		data_out[15:0] = {vpos[7:0],hpos[8:1]};
	else
		data_out[15:0] = 0;
end
```

**DUAL is stored and read nowhere.** BEAMCON0 is kept whole in `beamcon0_reg` ([`#L118-L126`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L118-L126)), and the wire that would name bit 6 is a comment ([`#L136`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L136), `//wire displaydual  = beamcon0_reg[ 6];`). There is one horizontal counter, `hpos` ([`#L256-L289`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L256-L289)), and a written HTOTAL reaches it only through VARBEAMEN ([`#L223`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L223), `varbeamen ? htotal_reg : HTOTAL_VAL << 1`). With DUAL set and VARBEAMEN clear, HTOTAL is stored and acts on nothing.

[`integration/aa-integ-s1`](../../integration/aa-integ-s1)'s P14 selects the AA chip set and makes four runs. Each writes BEAMCON0 and HTOTAL, then takes 1024 samples, each an HHPOSR read and a VHPOSR read (`prog_dual.s`; HHPOSR and VHPOSR are read registers, and the program writes nothing else after its register list). U1 writes BEAMCON0 = $0860 (LOLDIS, DUAL, PAL) and HTOTAL = $0040, U2 the same with HTOTAL = $0070; U0 ($0820, DUAL clear) and U3 ($08A0, VARBEAMEN, with VTOTAL $0138) are contrasts for the main counter. The reads in U1 and U2 are all $0000:

```
MEASURED: P14 measured: first restart sample U1 -1, U2 -1; largest whole HHPOSR word U1=$0000 U2=$0000
MEASURED: P14 measured: max h after the first restart U1=$00 U2=$00; max m U0=$E2 U1=$E2 U2=$E2 U3=$2F (U3 min $03)
MEASURED: P14 qualifying pairs: U1 844 (0 not a whole number of CCK; 0 after the first restart, 0 differ), U2 844 (0; 0, 0)
```

(fx68k. TG68K prints 831 qualifying pairs in each of U1 and U2, and `U3=$FC (U3 min $01)`; the first-restart and largest-word lines are the same.) `P14-MAIN` is green: the main counter's VHPOSR maximum is $E2 in U0, U1 and U2, so DUAL leaves its line alone. The sampling preconditions are green on both cores: `P14-FIXTURE`, `P14-UNIT`, `P14-R2`, `P14-PHASE` (the sample times cover every phase of a counter of HTOTAL+1), `P14-STEP`, `P14-PAIRS`, `P14-POS` and `P14-U3`.

`P14-U1` is graded at `test_aa_integ_s1.cpp:2231`, `P14-U2` at `:2232`, `P14-REL` at `:2238` and `P14-AGG` at `:2244-2245`.

## Risks and unknowns

- **Hardware is not measured**, the AA specification is marked PRELIMINARY and the entries name no chip revision. Does Alice have the counter? With DUAL set, write HHPOSW and read HHPOSR twice a few colour clocks apart: a count that moves from the written value settles it; $0000, or the written value unchanged, says the page describes a mode the part does not have.
- **Which bit decides its period, and at what rate it counts.** p. 24 makes HTOTAL "Active if VARBEAMEN=1 or DUAL=1" and p. 12 gives VARBEAMEN to "the main horiz counter"; the package reads that as HTOTAL ending the second counter's line with DUAL alone, and WinUAE ends it at the main line unless VARBEAMEN is set. The name says "hires H beam counter" while its bits are HTOTAL's, which counts 280 ns steps; `P14-REL` grades one count per colour clock.
- **HHPOSR bits 15-8** are stated by no page and graded by no assertion; the whole HHPOSR word reads $0000 in `P14`'s runs.
- **The line length is measured, not stated.** `P14-R2` and `P14-UNIT` accept lines of 227 or 228 colour clocks; the unit run measures 227 on every one of its 1401 line spacings.
- **DUAL's other users are not graded.** `P15-V1`, the UHRES pointer under DUAL, prints NOT GRADED because `P15-V0` measures no UHRES access with DUAL clear; the horizontal comparators p. 12 gives DUAL are not graded either.
- **The witnesses split.** Three of the four have no such counter, the one that has it ties the period to VARBEAMEN, TC64 has taken bit 6 for its RTG switch, and a comment there calls the mode never implemented on real hardware, without a source. The finding rests on pp. 10, 12 and 24, not on any implementation.
- **`aa-integ-s1` has not been run on a copy of the RTL with the counter**, so that its four `P14` reds turn green is a reading of the page, not a result.

## Independent evidence

The question put to each: with DUAL set and VARBEAMEN clear, what does HHPOSR read, and where does that counter restart?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7511`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7511), [`:2540-2543`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2540-L2543), [`:7074-7078`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7074-L7078), [`:2528-2538`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2528-L2538), [`:11127`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11127), [`:11148-11154`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11148-L11154), [`:1893-1900`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1893-L1900), [`:9266-9272`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9266-L9272), [`:11217`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11217) | <pre>if (beamcon0_dual) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (hhpos == maxhpos \|\| hhpos == maxhpos_long) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;hhpos = 0;<br>&nbsp;&nbsp;&nbsp;&nbsp;}<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;hhpos = agnus_hpos;<br>}</pre> **a second counter, restarting where the main line ends.** A write to 1D8 sets it, `hhpos = v & (MAXHPOS_ROWS - 1);`, and a read of 1DA returns its low byte unless the Agnus is an OCS one, `if (!ecs_agnus)` / `goto writeonly;` / `v = HHPOSR();`. It advances once per colour clock beside the main counter, `hhpos++;`; with DUAL clear it is the main counter, with DUAL set it runs on its own and restarts at `maxhpos`, the main line's length. HTOTAL sets that length, `maxhpos = htotal + 1;`, only under VARBEAMEN, which shortens the main line too. DUAL moves the hardwired sync comparisons onto it, `int hhp = beamcon0_dual ? hhpos : agnus_hpos;`, and restarts the UHRES chain from it, `} else if (hhpos == 0 && beamcon0_dual) {` / `// DUAL: trigger is HHPOS reset` |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:3088-3089`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L3088-L3089), [`Core/Components/Agnus/AgnusRegs.cpp:570-572`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L570-L572), [`:651-656`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L651-L656), [`Memory.cpp:2508-2509`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2508-L2509), [`AgnusRegs.cpp:549-568`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L549-L568) | <pre>void<br>Agnus::pokeHHPOSW(u16 value)<br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;xfiles("pokeHHPOSW(%04x)\n", value);<br>&nbsp;&nbsp;&nbsp;&nbsp;hhposw = value;<br>}</pre> **no counter.** A write to 1D8 is stored, under the comment *"The following registers are recognized and stored, but have no effect yet."*, and nothing reads `hhposw`. `peekCustom16` has no case for 1DA, so a read takes the default, `result = peekCustomFaulty16(addr);`, which returns the last value on the data bus. `pokeBEAMCON0` acts on PAL and LOLDIS; DUAL appears only in its comment, `// 14: HARDDIS      10: CSCBEN      6: DUAL         2: CSYTRUE` |
| **Denise** | [`emulation/libami/agnus/agnus.h:15-21`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L15-L21), [`emulation/libami/agnus/register.cpp:112-125`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L112-L125), [`:707-709`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L707-L709), [`:550-555`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L550-L555) | <pre>/**<br> * todos:<br> * AGA<br> *<br> * variable vsync, vblank, hsync, hblank, hcenter<br> * UHRES/DUAL stuff<br> */</pre> **no counter.** `readCustom` and `writeCustom` have no case for 1DA or 1D8: a read takes the default and returns the data bus, `return dataBus;`, and a write takes the default, which reads the address instead. BEAMCON0 is stored whole, `beamCon = value;`, and no line reads its DUAL bit |
| **Copperline** | [`src/chipset/agnus.rs:232-235`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L232-L235), [`:1305-1312`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1305-L1312), [`src/bus/custom_regs.rs:109-111`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L109-L111), [`:950-955`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L950-L955), test [`src/chipset/agnus.rs:2451-2459`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L2451-L2459) | <pre>// HHPOSR (ECS Agnus): UHRES dual-mode H counter readback. The<br>// counter is not emulated, so this reads the HHPOSW latch.<br>0x1DA if self.agnus.revision().is_ecs() =&gt; self.agnus.hhpos(),</pre> **a latch, not a counter.** A write to 1D8 on an ECS or AA Agnus stores nine bits, `self.hhpos = val & 0x01FF;`, and HHPOSR returns them unchanged; its own test asserts the readback, `assert_eq!(ecs.hhpos(), 0x0123, "9-bit horizontal latch");`. Setting DUAL logs `"BEAMCON0 DUAL set: A2024/Productivity (UHRES dual-monitor) display is not emulated"` once |
| **MiSTer** | [`rtl/agnus_beamcounter.v:113-121`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L113-L121), [`:142`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L142) | descendant — the same two-case read multiplexer with a default of 0, and `//wire displaydual  = beamcon0_reg[ 6];` as a comment. No counter |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:129-136`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L129-L136), [`:167`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L167), [`:224`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L224), [`:515-516`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L515-L516) | descendant — the same read multiplexer and no counter. The DUAL bit is wired, `wire displaydual  = beamcon0_sh[ 6];`, but for another purpose: `// Abuse the DUAL bit in BEAMCON0 to enable the RTG mode` / `assign rtg_ena = displaydual;`. The comment over the register it reuses calls the mode `// AMR - Register for never-implemented-on-real-hardware Dual mode - we use as line compare` |

Copperline has the register as a latch that reads back what was written and says outright that the counter is not emulated; vAmiga and Denise do not decode HHPOSR at all and return the data bus. None of the four counts HHPOSR up to HTOTAL with VARBEAMEN clear.

## Decision

One option, a second horizontal counter in `agnus_beamcounter.v`:

- **The register.** A read of $1DA returns an 8-bit counter in bits 7-0, with bits 15-8 zero, which no page states.
- **The count.** One step per colour clock, restarting after the stored `htotal_reg` (p. 24's "Active if VARBEAMEN=1 or DUAL=1"), while `htotal` (line 223) keeps feeding the main counter only under VARBEAMEN.
- **DUAL.** Bit 6, the comment at line 136, selects that counter. p. 12 also gives DUAL the horizontal comparators and the UHRES pointer chain, which `aa-integ-s1` does not grade.

A program that reads HHPOSR gets $0000 now; it would get a count. This option has not been built or run here.
