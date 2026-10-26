# NM-AA-007 — BPLCON0's BYPASS bit has no effect: the red output still follows the colour table, where AA p. 17 puts the colour address on R(7:0) [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `denise.v`, with the AA chip set selected and BPLCON0 bit 5 (BYPASS) set, put each pixel's colour address on the red output instead of the colour table's red, as AA PDF p. 17 prints? Closes when the red output follows the colour address under BYPASS and `P6-II` and `P6-AGG` are green on both cores, or when an Amiga specialist rejects the change; if the rejection means p. 17 is wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | AA chip set (`chipset_config` = $10), BPLCON0 = $0030 against $0010 (eight low-resolution bit-planes scrolled by BPLCON1 = $0033, sprite 0 in front of them on lines $60-$6F); red's top four bits only, green and blue not graded; fx68k and TG68K; chip revision unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 17, BYPASS; PDF p. 4, *Video*; PDF p. 16, the BPLCON0 column of the bit table. The document prints no page numbers: p. N is the PDF page · `P6-I`, recorded not graded: `sol:AA-bplcon0.c19`, p. 17; `P6-II`, `P6-AGG` and the preconditions carry no claim id |
| **Module** | `src/minimig-aga/denise.v:167`, `:190`, `:487-500` and `:536-565` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L167-L190) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L130-L153); inherited (MiSTer leaves `bypass` and its assignment commented out; TC64 assigns `bypass`, as NanoMig does, and nothing reads it) |
| **Test** | [`integration/aa-integ-s1`](../../integration/aa-integ-s1): `make -k -C integration/aa-integ-s1 test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. The AA chip set, graded on both CPU cores |
| **Assertions** | 2 red on fx68k and 2 on TG68K, in a package run of 156 passed, 10 failed, 11 NOT GRADED on each core: `P6-II` expects, with BYPASS set, the same red at every observed pixel under table T_x as under table T_id, and observes 248528 of 248528 pixels differing on each core; `P6-AGG` needs both halves of p. 17's sentence and is red through `P6-II`. `P6-I`, scrolling and priority unchanged under BYPASS, prints NOT GRADED because without a BYPASS that acts it would hold vacuously. `P6-R4`, `P6-FIXTURE`, `P6-F3`, `P6-R1`, `P6-POP`, `P6-TBL`, `P6-RED16`, `P6-SPR` and `P6-SCROLL`, which the comparison rests on, are green on both cores. The run's eight other reds are not this finding's: `P4b-C1`, `P4b-C2` and `P5` grade BPLCON1's reset value ([NM-AA-006](NM-AA-006.md)), `P10-LATCH` the light-pen latch ([NM-HRM-087](NM-HRM-087.md)), and `P14-U1`, `P14-U2`, `P14-REL` and `P14-AGG` HHPOSR with DUAL set ([NM-AA-008](NM-AA-008.md)) |
| **Verdict of the four** | **split, one against two, and Denise casts no vote.** WinUAE, with AGA selected, replaces the colour table by a grey ramp, entry i giving red, green and blue i, so the red byte is the colour address. vAmiga and Copperline model the AGA display and look every pixel up in the colour table; neither reads BPLCON0 bit 5. Denise models no AA machine. MiSTer and TC64, descendants, read the bit nowhere, as NanoMig. Hardware: not measured |
| **Effort** | S — `denise.v` only: qualify `bypass` with `aga`, register `clut_data` by one clock to line up with the table's output, and select it into the red byte of `out_rgb` while `bypass` is set |

---

## Source requirement

PDF p. 17 gives the bit's meaning in the notes under the bit table:

```
BYPASS= bitplanes are scrolled and prioritized normally, but
        bypass color table and 8 bit wide data appear on
        R(7:0).
```

PDF p. 4, in the section *Video*, says the same from the chip's side:

```
        BYPASS bit in BPLCON0 allows prioritized color address to
bypass the color table and exit LISA on R(7:0) bus.
```

PDF p. 16 puts the bit at BPLCON0 bit 5 and marks it `=0`, so RST_ clears it:

```
      05        BYPASS=0    PF2H3       PF2P2       BRDRBLNK=0  ESPRM5=0
```

With BYPASS set, then, the address the priority logic chooses for a pixel, playfield or sprite, reaches R(7:0) in place of the table's red, and scrolling and priority are as without it.

## RTL and measurement

**BPLCON0 bit 5 is stored and named, and nothing reads it.** The register is kept whole and the bit is assigned to the wire `bypass`, which no line of `denise.v` reads — [`src/minimig-aga/denise.v#L167-L190`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L167-L190):

```verilog
wire          bypass;     // bypass color table; 8-bit wide data appears on r[7:0]
...
      bplcon0[15:0] <= #1 data_in[15:0];
...
assign bypass   = bplcon0[5];
```

`:182-194` assign every bit of BPLCON0 to a wire; the wires that Denise reads are those of bits 15, 14-12, 11, 10, 6, 4 and 0, and those of bits 9, 8, 7, 5, 3, 2 and 1 are read by nothing. The priority logic's choice, playfield or sprite, is `clut_data` ([`#L536-L545`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L536-L545)), which is the colour table's `.select` input (`:494`); the video output is the table's colour or the HAM generator's, and black while `t_blank` (`:562`) is set, [`#L556-L565`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L556-L565):

```verilog
wire  [24-1:0] out_rgb  = ham_sel && window_del && !sprsel_del ? ham_rgb : clut_rgb;
...
assign {red[7:0],green[7:0],blue[7:0]} = t_blank ? 24'h000000 : out_rgb;
```

`nanomig.v` passes the top four bits of the red byte to its video output, `wire [3:0] r_in = (hbl||vbl)?4'h0:red[7:4];` ([`src/nanomig.v#L1374`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/nanomig.v#L1374)). None of these lines is inside `ifdef DENISE_EBR`, so the TG68K build, which defines it, and the fx68k build have the same path.

[`integration/aa-integ-s1`](../../integration/aa-integ-s1)'s P6 shows eight low-resolution bit-planes, scrolled by BPLCON1 = $0033, with sprite 0 in front of them on lines $60-$6F. Each line holds twenty groups of sixteen pixels, and every pixel's colour address is non-zero. Two colour tables are loaded: T_id gives entry i the red nibble i>>4, T_x the red nibble $F - (i>>4). Four runs combine BPLCON0 = $0010 (eight planes, B0) or $0030 (the same with BYPASS, B1) with T_id or T_x, and the red output is compared pixel by pixel over the window of F3. The port shows four bits a colour, and `P6-R4` shows they are R7-R4: a pixel whose colour holds red $A5 shows `$A`.

- **Without BYPASS the table decides every pixel.** B0/T_x and B0/T_id differ in red at every observed pixel (`P6-TBL`, green).
- **With BYPASS set the table still decides every pixel.** B1/T_x and B1/T_id differ at the same pixels, where the page's reading makes them equal. `P6-II` is red on both cores.
- **Setting BYPASS changes no pixel.** B1/T_id and B0/T_id differ at 0 pixels. `P6-I` would compare exactly these two runs to show that scrolling and priority survive BYPASS; with a BYPASS that does nothing the comparison holds without showing it, so the line prints NOT GRADED, and `P6-AGG`, which needs both halves, is red.

```
MEASURED: P6 population: 392 lines, 634 window columns on a line
MEASURED: P6 table: 248528 of 248528 pixels differ between B0/T_x and B0/T_id
MEASURED: P6 sprite: 32 output lines carry sprite columns
MEASURED: P6 scroll: 19 group boundaries without scroll, 19 with BPLCON1=$0033, line index 78
MEASURED: P6 (ii): 248528 of 248528 pixels differ between B1/T_x and B1/T_id
MEASURED: P6 (i): 0 of 248528 pixels differ between B1/T_id and B0/T_id
```

(fx68k; TG68K prints the same six lines.) The package grades red's top four bits only; green and blue under BYPASS are not stated on the page and not graded.

`P6-II` is graded at `test_aa_integ_s1.cpp:1584`, `P6-I` recorded at `:1588` and `P6-AGG` graded at `:1590`.

## Risks and unknowns

- **What green and blue show under BYPASS.** The pages name R(7:0) only. WinUAE shows the address on all three colours; leaving green and blue to the table is equally consistent with the pages.
- **Is BYPASS wanted on NanoMig at all?** Its red byte reaches the video output (`nanomig.v:1374`), so with BYPASS the screen would show colour addresses as red.
- **Two of the three implementations that model an AGA display agree with NanoMig.** vAmiga and Copperline have no BYPASS either; WinUAE alone has one. The finding rests on pp. 4 and 17, not on any implementation.
- **The reading of the page.** The first line of the p. 17 entry is tagged `[3/3]` in the corpus; its two continuation lines are tagged `[1/3]` because they carry no identifier to corroborate, and the Tesseract reading has the second line word for word (`ocr/p17_300dpi_psm6.txt`, line 7); the 1993 `AGA.guide` retyping reads `bypass color table and 8 bit wide data appear on R(7:0).` The p. 4 lines are tagged `[2/3]`. The document is marked PRELIMINARY.
- **`sol:AA-bplcon0.c19` is not graded.** Its claim, that scrolling and priority are unchanged under BYPASS, sits on `P6-I`, which prints NOT GRADED while BYPASS does nothing; the red rests on the table clause alone.
- **Real hardware is not measured.**

## Independent evidence

The question put to each: with an AA chip set and BPLCON0 bit 5 set, what reaches the red output?

| | where | result |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2432-2441`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2432-L2441), [`:2852-2876`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2852-L2876) | **the colour address**: with AGA selected and the bit set it draws from `direct_colors_for_drawing_bypass`, a grey ramp whose entry i is red, green and blue i, and turns HAM and EHB off |
| **vAmiga** | [`DeniseRegs.cpp:172`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L172), [`PixelEngine.cpp:197-241`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L197-L241), [`DeniseTypes.h:24`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseTypes.h#L24) | **the table's colour**: nothing in its Denise names BYPASS; a BPLCON0 write reaches the pixel engine only if it changes a bit of `0xFC50`, which does not hold bit 5, and the pixel engine reads BPLCON0 for HAM, SHRES and EHB only. Its AGA Denise is marked `(partially supported)` |
| **Denise** | [`system.cpp:444-459`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L444-L459), [`denise.h:22`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.h#L22) | **no vote**: `setModel` offers OCS and ECS chip sets only, and the display chip's models end at `ECS = 8`; BPLCON0 bit 5 is read nowhere |
| **Copperline** | [`output.rs:429-482`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/output.rs#L429-L482), [`bitplane.rs:1112-1118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L1112-L1118), [`:1185-1213`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L1185-L1213) | **the table's colour**: its Lisa path, `denise_aga_playfield_output`, takes every non-HAM pixel from the banked palette; the BPLCON0 bits the renderer reads are HIRES, the plane count, HAM, DPF, SHRES, LACE and ECSENA, and bit 5 is not one of them |
| **MiSTer** | [`rtl/denise.v:130`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L130), [`:153`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L153), [`:468-477`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L468-L477) | descendant: the wire and its assignment are comments, `//wire          bypass;` and `//assign bypass   = bplcon0[5];`; the `out_rgb` multiplexer is NanoMig's |
| **TC64** | [`rtl/minimig/denise.v:137`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise.v#L137), [`:160`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise.v#L160), [`:483-492`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise.v#L483-L492) | descendant: declares and assigns the bit, `assign bypass   = bplcon0[5];`, and nothing reads `bypass`; the output multiplexer is NanoMig's |

### What practitioners report

Reports from the forum thread [EAB 19676](https://eab.abime.net/showthread.php?t=19676): witnesses, never an oracle ([practitioner reports](../evidence/practitioner-reports.md)).

**SUPPORTS.** [EAB post 1070412](https://eab.abime.net/showpost.php?p=1070412), Toni Wilen, 2016-02-14. Chip set: AGA. Known from: not stated in the post.

> "Output is grayscale, output value is copied to all color component bits, not just red."

Under BYPASS the colour index reaches the output, red included, which is what AA p. 17 prints and what the page's reds grade.

## Decision

**One option: when the AA chip set is selected and BPLCON0 bit 5 is set, drive the red byte from the colour address instead of the table.** In `denise.v`, qualify the `bypass` of `:190` with the chip set, `bypass = bplcon0[5] & aga`, keep a copy of `clut_data` registered on `clk` so that it lines up with the table's output, which the instance's own comment says is `delayed by one clk28m clock cycle` (`:499`), and select it into the red byte of `out_rgb`, in place of the table's or the HAM generator's, while `bypass` is set; WinUAE turns HAM and EHB off under BYPASS. The page says nothing about green and blue; WinUAE shows the address on all three, and leaving them to the table is the other choice. This option has not been built or run here. On NanoMig the red byte is the video output, so the change puts colour addresses on the screen as red whenever software sets the bit.
