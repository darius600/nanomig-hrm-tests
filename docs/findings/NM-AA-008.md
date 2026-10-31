# NM-AA-008 — HHPOSR reads $0000 and HHPOSW reaches nothing with DUAL set: the secondary beam counter of AA p. 24 is not there [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `agnus_beamcounter.v` give HHPOSR ($1DA) and HHPOSW ($1D8) the second horizontal counter of AA p. 24, which with BEAMCON0 bit 6 (DUAL) set counts colour clocks up to HTOTAL and restarts, and which a write to HHPOSW sets, where NanoMig's HHPOSR reads $0000 and a write to HHPOSW reaches no register? Closes when the counter is added and `P14-U1`, `P14-U2`, `P14-REL` and `P14-AGG` of `aa-integ-s1`, `P11-HHRUN` and `P12-K` of `aa-integ-s2` and `P8-E1`, `P8-E2`, `P8-E3`, `P8-E4` and `P8-AGG` of `aa-integ-s7-rest` are green on both cores, or when an Amiga specialist rejects the counter because the chip has none, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | HHPOSR ($1DA) read with BEAMCON0 DUAL set and VARBEAMEN clear, HTOTAL $0040 and $0070 in `aa-integ-s1`, $0038 in `aa-integ-s2`, and $0071, $0051 and $000A in `aa-integ-s7-rest`, whose runs write HHPOSW ($1D8) with $0010, $0040 or $0005 just before 64 reads and set LPENDIS, so that HHPOSR reads the counter and not a light-pen latch; AA chip set selected (`chipset_config` $10); chip revision unknown (the entries name none); hardware not measured. Isolated: the main counter keeps its line under DUAL (`P14-MAIN` and `PR-9a` green); bits 15-8 of HHPOSR, which no page states, are not graded; `aa-integ-s1` never writes HHPOSW, and `aa-integ-s2`'s HHPOSW line, `P12-HHW`, prints NOT GRADED |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 24 (the HHPOSR/HHPOSW and HTOTAL entries), p. 12 (BEAMCON0: DUAL, VARBEAMEN, LPENDIS), p. 10 (the HHPOSW and HHPOSR rows). The document prints no page numbers: p. N is the PDF page · <details><summary>claim ids by assertion (18)</summary> `P14-AGG` of `aa-integ-s1`: `sol:AA-hhpos.c1`, `sol:AA-hhpos.c5`, p. 24; `P8-AGG` of `aa-integ-s7-rest`: `opus:AA-hhposw.c1`, `opus:AA-hhposw.c2`, `opus:AA-hhposw.c5`, `opus:AA-hhposr.c1`, `opus:AA-hhposr.c2`, `opus:AA-hhposr.c5`, `sol:AA-hhposw.c1`, `sol:AA-hhposw.c2`, `sol:AA-hhposw.c5`, `sol:AA-hhposr.c1`, `sol:AA-hhposr.c2`, `sol:AA-hhposr.c5`, p. 10; `P12` of `aa-integ-s2` (not graded): `opus:AA-beamcon0.c31`, `opus:AA-beamcon0.c32`, `sol:AA-beamcon0.c48`, `sol:AA-beamcon0.c49`, p. 12. `P14-U1`, `P14-U2` and `P14-REL`; `P11-HHRUN`, `P12-K`, `P11-MAXH`, `P11-HHMAX`, `P11`, `P11-PERIOD`, `P12-HHW`, `P11-FIXTURE`, `P11-ANCHOR`, `P12-KFIXTURE` and `P12-KANCHOR` of `aa-integ-s2`; and `P8-E1`, `P8-E2`, `P8-E3`, `P8-E4`, `P8-HD` and `PR-8a` to `PR-8d` of `aa-integ-s7-rest` carry no claim id</details> |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L108-L115) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L113-L121); inherited (MiSTer has the same read path; TC64 has it too and wires bit 6 to its RTG switch) |
| **Test** | [`integration/aa-integ-s1`](../../integration/aa-integ-s1): `make -k -C integration/aa-integ-s1 test`, or `test-fx68k` and `test-tg68k` of the same package; [`integration/aa-integ-s2`](../../integration/aa-integ-s2): `make -k -C integration/aa-integ-s2 test`, or `test-fx68k` and `test-tg68k`; [`integration/aa-integ-s7-rest`](../../integration/aa-integ-s7-rest): `make -k -C integration/aa-integ-s7-rest test`, or `test-fx68k` and `test-tg68k`; an expected red exits nonzero. The AA chip set, graded on both CPU cores |
| **Assertions** | 4 red on fx68k and 4 on TG68K, in a package run of 156 passed, 10 failed, 11 NOT GRADED on each core: `P14-U1`, expected every HHPOSR bits 7-0 value from the first restart at most $40 with maximum $40, observed largest whole HHPOSR word $0000 and no restart; `P14-U2`, expected at most $70, maximum $70, values between $40 and $70, observed $0000; `P14-REL`, expected the h difference of consecutive samples to equal the elapsed colour clocks modulo HTOTAL+1, observed no restart to start from; `P14-AGG`, expected a second counter from 0 to HTOTAL, observed $0000 (it is red through `P14-U1`, `P14-U2` and `P14-REL`; `P14-MAIN` is green). The run's six other reds are not this finding's: `P4b-C1`, `P4b-C2` and `P5` grade BPLCON1's reset value ([NM-AA-006](NM-AA-006.md)), `P6-II` and `P6-AGG` BPLCON0's BYPASS bit ([NM-AA-007](NM-AA-007.md)), and `P10-LATCH` the light-pen latch ([NM-HRM-087](NM-HRM-087.md)). In `aa-integ-s2`, 2 red on fx68k and 2 on TG68K, in a package run of 131 passed, 13 failed on each core: `P11-HHRUN`, expected at least two different HHPOSR values on every main line, observed a maximum of 0 over 313 lines; `P12-K`, expected at least two HHPOSR decreases on every complete main line, observed 0. `P11-MAXH`, `P11-HHMAX`, `P11` and `P11-PERIOD` print NOT GRADED behind `P11-HHRUN`. That run's eleven other reds are not this finding's: `F5-SV-INV` ([NM-HRM-032](NM-HRM-032.md)), `P9-BV`, `P9-B` and `P9-ID` ([NM-AA-009](NM-AA-009.md)), `P12-UHRES` ([NM-AA-010](NM-AA-010.md)), `P13-0-LINES` and `P13-0-COUNT` ([NM-AA-005](NM-AA-005.md)), `P14-0-LINES` and `P14` ([NM-HRM-010](NM-HRM-010.md)), and the two light-pen lines ([NM-HRM-087](NM-HRM-087.md)). In `aa-integ-s7-rest`, 5 red on fx68k and 5 on TG68K, in a package run of 36 passed, 21 failed, 11 NOT GRADED on each core: `P8-E1`, expected the samples after HHPOSW = $0040 to read ($0040 − $0010) mod 114 = $30 more, sample for sample, than after $0010, observed no difference; `P8-E2`, expected every sample at most HTOTAL and, in HW (HTOTAL $0A, HHPOSW $05), the counter to reach $0A and the sample after it to be lower, observed 64 samples of one value, $00, $0A sampled 0 times; `P8-E3`, expected the first sample to follow the written value by at most the measured latency, 20 memory cycles, plus one count, observed $00 in HA, HB and HC; `P8-E4`, expected every count difference between consecutive samples to be at least 1, observed 0 (the counts sum to 0 over 820 memory cycles on fx68k, 759 on TG68K); `P8-AGG`, a counter that HHPOSW sets and that counts to HTOTAL and restarts, is red with them. Green on the same run: `P8-HD` and the preconditions `PR-8a` to `PR-8d`. That run's sixteen other reds are not this finding's: `P1` and `PR-2a` ([NM-AA-013](NM-AA-013.md)) and fourteen UHRES lines ([NM-AA-010](NM-AA-010.md)) |
| **Verdict of the four** | **split: one of four has the counter, none bounds it by HTOTAL alone.** WinUAE reads and writes a second horizontal counter that DUAL detaches from the main one and that restarts its UHRES chain, but it restarts where the main line ends, at HTOTAL only when VARBEAMEN is set as well. vAmiga stores an HHPOSW write and decodes no HHPOSR; Denise decodes neither and lists "UHRES/DUAL stuff" among its todos; Copperline returns the last HHPOSW value from HHPOSR and logs that DUAL is not emulated. Hardware: not measured |
| **Effort** | none; if the counter is accepted, M: a second 8-bit counter in `agnus_beamcounter.v` with a write case at $1D8 that loads it and a read case at $1DA that returns it in bits 7-0, one step per colour clock restarting after the stored `htotal_reg`, and BEAMCON0 bit 6 selecting it |
| **See also** | [NM-AA-005](NM-AA-005.md): the long line of the same HTOTAL entry; [NM-AA-010](NM-AA-010.md): the UHRES chain that DUAL restarts at this counter's reset |

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

and LPENDIS sends the counter, not a light-pen latch, to HHPOSR:

```
                    LPENDIS= When this bit is a low and LPE (BPLCON0,BIT 3) is
                     enabled, the light-pen latched value (beam hit position)
                     will be read by VHPOSR, VPOSR and HHPOSR.  When the bit is
                     a high the light-pen latched value is ignored and the actual
                     beam counter position is read by VHPOSR, VPOSR and HHPOSR.
```

**PDF p. 10**, the address list, gives both rows:

```
HHPOSW     h  1D8  W    A       DUAL mode hires H beam counter write
HHPOSR     h  1DA  R    A       DUAL mode hires H beam counter read
```

Read together: with DUAL set, HHPOSR reads a second horizontal counter, HHPOSW writes it, it counts the 280 ns steps HTOTAL counts, and HTOTAL is its highest count. The main counter keeps its line. None of these pages says what HHPOSR returns with DUAL clear, or in bits 15-8, where HTOTAL's bits are `x`.

## RTL and measurement

**No register at $1D8 or $1DA exists.** The beam counter's register list stops at the ECS registers ([`agnus_beamcounter.v#L60-L76`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L60-L76)) and no file under `src/` decodes $1DA or $1D8, so a write to HHPOSW reaches nothing. The read multiplexer has two cases and a default of 0 ([`#L108-L115`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L108-L115)), so a read of $1DA returns $0000:

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

[`integration/aa-integ-s1`](../../integration/aa-integ-s1)'s P14 selects the AA chip set and makes four runs. Each writes BEAMCON0 and HTOTAL, then takes 1024 samples, each an HHPOSR read and a VHPOSR read (`prog_dual.s`; HHPOSR and VHPOSR are read registers, and the program writes nothing else after its register list). Each sample waits until the main counter's VHPOSR bits 7-0 reach a target that moves 38 to 44 counts per sample, so the main beam, not the CPU's speed, sets when it is taken. U1 writes BEAMCON0 = $0860 (LOLDIS, DUAL, PAL) and HTOTAL = $0040, U2 the same with HTOTAL = $0070; U0 ($0820, DUAL clear) and U3 ($08A0, VARBEAMEN, with VTOTAL $0138, sampled back to back by `prog_dual_cpu.s`, whose lines are shorter than the targets) are contrasts for the main counter. The reads in U1 and U2 are all $0000:

```
MEASURED: P14 measured: first restart sample U1 -1, U2 -1; largest whole HHPOSR word U1=$0000 U2=$0000
MEASURED: P14 measured: max h after the first restart U1=$00 U2=$00; max m U0=$E2 U1=$E2 U2=$E2 U3=$2F (U3 min $03)
MEASURED: P14 qualifying pairs: U1 705 (0 not a whole number of CCK; 0 after the first restart, 0 differ), U2 705 (0; 0, 0)
```

(fx68k. TG68K prints 701 qualifying pairs in each of U1 and U2, and `U3=$F8 (U3 min $03)`; the first-restart and largest-word lines are the same.) `P14-MAIN` is green: the main counter's VHPOSR maximum is $E2 in U0, U1 and U2, so DUAL leaves its line alone. The sampling preconditions are green on both cores: `P14-FIXTURE`, `P14-UNIT`, `P14-R2`, `P14-PHASE` (the sample times cover every phase of a counter of HTOTAL+1), `P14-STEP`, `P14-PAIRS`, `P14-POS` and `P14-U3`.

`P14-U1` is graded at `test_aa_integ_s1.cpp:2239`, `P14-U2` at `:2240`, `P14-REL` at `:2246` and `P14-AGG` at `:2253-2254`.

[`integration/aa-integ-s2`](../../integration/aa-integ-s2) reads the counter with a lower HTOTAL. Its P11 writes HTOTAL = $0038 and BEAMCON0 = $0060 (DUAL, PAL; VARBEAMEN clear) and samples VPOSR:VHPOSR and HHPOSR over one frame (`prog_beam.s`); P12 runs the same program again as the DUAL run whose alternate-counter rate and wraps it needs for the UHRES chain. HHPOSR reads 0 on every sample of P11's run, while the main counter runs to 226 and the port lines are 227 colour clocks, and it neither rises nor wraps within a line of P12's:

```
MEASURED: P11 measured: median port line of a stopped frame = 227 colour clocks in the DUAL run (anchor 1)
MEASURED: P11 measured: HTOTAL=0038: maximum H = 226, maximum HHPOSR (bits 7-0) = 0, HHPOSR decreases per main line = 0.000 (0 over 313 lines)
FAIL: P11-HHRUN: on every main line of the frame HHPOSR takes at least two different values, so the alternate counter runs
MEASURED: P12 measured: colour clock T_C = 8 system clocks; HHPOSR rate per colour clock = 0.000, alternate counter period = 0.000 colour clocks, fewest HHPOSR decreases on a main line = 0
FAIL: P12-K: in the DUAL run of P11 HHPOSR decreases at least twice on every complete main line, so the alternate counter wraps at least twice per main line
```

The same lines on fx68k and TG68K, which take 1963 and 2078 samples in the frame. The runs' own anchors are green on both cores: `P11-FIXTURE`, `P11-ANCHOR`, `P12-KFIXTURE` and `P12-KANCHOR`. `P11-HHRUN` is graded at `test_aa_integ_s2.cpp:1550` and `P12-K` at `:2098`.

[`integration/aa-integ-s7-rest`](../../integration/aa-integ-s7-rest)'s P8 writes the counter. Each run writes HTOTAL = N, BEAMCON0 = $2860 (LPENDIS, LOLDIS, DUAL, PAL; VARBEAMEN clear) and HHPOSW = X, then reads HHPOSR 64 times at once (`prog_samp.s`): HA (N = $71, X = $10), HB ($71, $40), HC ($51, $10), HD (HA again) and HW ($0A, $05), whose line of 11 counts lets 64 samples, at most 14 memory cycles apart on fx68k and 13 on TG68K, pass every count. Every sample reads $00:

```
MEASURED: P8: first samples (bits 7-0) HA: 00 00 00 00 00 00 HB: 00 00 00 00 00 00 HC: 00 00 00 00 00 00 HD: 00 00 00 00 00 00 HW: 00 00 00 00 00 00
MEASURED: P8-E2: HW writes HTOTAL = $0A and X = $05: its 64 samples show 1 distinct values; the line has N+1 = 11 counts, the highest sample is $00, N is sampled 0 times (fx68k)
FAIL: P8-E1: reading HHPOSR after HHPOSW = $0040 gives, sample for sample, ($0040 - $0010) mod 114 more than after HHPOSW = $0010 (AA p.24: HHPOSW 'DUAL mode hires H beam counter write', the counter counts up to HTOTAL and starts again)
FAIL: P8-E2: the counter counts up to HTOTAL and then starts again lower: every sample of HA, HB, HC and HW (bits 7-0) is at most the run's HTOTAL, N = $71 for HA and HB, $51 for HC and $0A for HW; within the 64 samples of HW the counter takes its highest value N = HTOTAL, and the sample after each sample at N is lower (AA p.24: HTOTAL 'Highest color clock count in horiz line', 'Horiz line has this many + 1 280ns increments', 'Active if VARBEAMEN=1 or DUAL=1')
MEASURED: P8-E4 rate (not graded): HA: sum d 0 counts, elapsed 820.000 memory cycles HB: sum d 0 counts, elapsed 820.000 memory cycles HC: sum d 0 counts, elapsed 820.000 memory cycles (fx68k)
FAIL: P8-E4: in each of HA, HB and HC the count advances by at least 1 between consecutive samples (AA p.24 HTOTAL 'this many + 1 280ns increments'; the rate is measured and not graded)
```

The same lines on TG68K, with 759.000 elapsed memory cycles and `(tg68k)`. The preconditions are green on both cores: HA and HB read the counter at the same moments (`PR-8a`), the samples are close enough for each count difference to be unambiguous (`PR-8b`), HA, HB, HC and HW end with 64 sample stores (`PR-8c`), and the first sample store follows the fetch of the HHPOSW instruction's last word by 20 memory cycles on both cores (`PR-8d`). `P8-E1` is graded at `s7r_p8.h:167-168`, `P8-E2` at `:187-188`, `P8-E3` at `:196-197`, `P8-E4` at `:216-217` and `P8-AGG` at `:230-231`.

## Risks and unknowns

- **Hardware is not measured**, the AA specification is marked PRELIMINARY and the entries name no chip revision. Does Alice have the counter? With DUAL set, write HHPOSW and read HHPOSR twice a few colour clocks apart: a count that moves from the written value settles it; $0000, or the written value unchanged, says the page describes a mode the part does not have.
- **Which bit decides its period, and at what rate it counts.** p. 24 makes HTOTAL "Active if VARBEAMEN=1 or DUAL=1" and p. 12 gives VARBEAMEN to "the main horiz counter"; all three packages read that as HTOTAL ending the second counter's line with DUAL alone, and WinUAE ends it at the main line unless VARBEAMEN is set. The name says "hires H beam counter" while its bits are HTOTAL's, which counts 280 ns steps; `P14-REL` grades one count per colour clock, while `P8-E4` grades only that the count advances between samples and prints the rate, for which no page gives a tolerance.
- **`P8-E2` is red because no sample moves.** Its first clause, every sample at most HTOTAL, holds for a counter stuck at $00; the clause that fails is that HW's samples reach HTOTAL and fall after it, and they show one value. With a counter, that clause rests on 64 samples at most 14 memory cycles apart visiting all 11 counts of HW's line, a spacing the program is built for and p. 24 does not state.
- **HHPOSR bits 15-8** are stated by no page and graded by no assertion; the whole HHPOSR word reads $0000 in `P14`'s runs, and bits 15-8 read $00 in `aa-integ-s7-rest`'s HA.
- **Green lines that say nothing here:** `P8-HD`, HD reading what HA reads, a predicted difference of 0, is green on a counter that never moves.
- **The line length is measured, not stated.** `P14-R2` and `P14-UNIT` accept lines of 227 or 228 colour clocks; the unit run measures 227 on every one of its 1401 line spacings.
- **DUAL's other users are not graded.** `P15-V1`, the UHRES pointer under DUAL, prints NOT GRADED because `P15-V0` measures no UHRES access with DUAL clear; the horizontal comparators p. 12 gives DUAL are not graded either. In `aa-integ-s2`, `P12`, the UHRES chain restarted at this counter's reset, prints NOT GRADED behind `P12-K` and `P12-UHRES` ([NM-AA-010](NM-AA-010.md)), and the comparators are declined there too. In `aa-integ-s7-rest`, `P9-E1a` and `P9-E1b`, the chain coming out more than once in a line from this counter's resets, print RECORDED: p. 12 qualifies it "assuming there is some memory bandwidth left", and no precondition establishes that bandwidth.
- **The witnesses split.** Three of the four have no such counter, the one that has it ties the period to VARBEAMEN, TC64 has taken bit 6 for its RTG switch, and a comment there calls the mode never implemented on real hardware, without a source. The finding rests on pp. 10, 12 and 24, not on any implementation.
- **`aa-integ-s1` has not been run on a copy of the RTL with the counter**, so that its four `P14` reds turn green is a reading of the page, not a result. That `P11-HHRUN` and `P12-K` of `aa-integ-s2` and the five `P8` reds of `aa-integ-s7-rest` turn green with the counter is likewise a reading of the code, not a `make test` result.

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

- **The register.** A write to $1D8 loads an 8-bit counter, and a read of $1DA returns it in bits 7-0, with bits 15-8 zero, which no page states.
- **The count.** One step per colour clock, restarting after the stored `htotal_reg` (p. 24's "Active if VARBEAMEN=1 or DUAL=1"), while `htotal` (line 223) keeps feeding the main counter only under VARBEAMEN.
- **DUAL.** Bit 6, the comment at line 136, selects that counter. p. 12 also gives DUAL the horizontal comparators and the UHRES pointer chain, which `aa-integ-s1` does not grade; `aa-integ-s2`'s `P12` grades the chain once this counter and [NM-AA-010](NM-AA-010.md)'s chain are there.

A program that reads HHPOSR gets $0000 now, whatever it wrote to HHPOSW; it would get a count that starts from the written value. This is a reading of the code, not run by `make test`.
