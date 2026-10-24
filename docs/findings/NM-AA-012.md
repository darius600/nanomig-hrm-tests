# NM-AA-012 — with VARBEAMEN set and LACE clear, a field lasts VTOTAL + 2 lines while the long-frame bit is set, where AA p. 29 gives VTOTAL + 1 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `agnus_beamcounter.v`, with BEAMCON0 bit 7 (VARBEAMEN) set and BPLCON0 bit 2 (LACE) clear, end every field after VTOTAL + 1 lines, as AA PDF p. 29 prints, where NanoMig ends it after VTOTAL + 2 while the long-frame bit (VPOSR bit 15) is set, as it is from reset on, so that VTOTAL = $05FF gives fields of 1537 lines? Closes when the vertical counter wraps after line VTOTAL in that mode whatever the long-frame bit holds, and `LF-FRAME` is green on both cores, or when an Amiga specialist rejects the change; if the rejection means p. 29 is wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | AA chip set selected (`chipset_config` = $10); BEAMCON0 = $08A0 (VARBEAMEN, LOLDIS, PAL), HTOTAL $00E2, VTOTAL $05FF against $04FF, BPLCON0 = $1000 (LACE clear); the long-frame bit at its reset value 1, since the program writes no VPOSW; each field timed between two Copper restarts at `nanomig.v`'s ports, on fx68k and TG68K. With the bit cleared by a VPOSW write, the module alone gives VTOTAL + 1 (`hrm/ch3-beam-counters-and-blanking`, VTOTAL 5). Not measured at the ports: LACE set, the PAL bit clear, the bit cleared. Chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 29, the VTOTAL entry. The document prints no page numbers: p. N is the PDF page. *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p. 304 (Appendix C), as context · `LF-FRAME` and `LF-CONTROL` carry no claim id; `VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (non-interlaced)` of `hrm/ch3-beam-counters-and-blanking`: `sol:C-scan.c9`, `opus:C-vtotal.c02`, `opus:C-vtotal.c03`, `opus:7-appC-beam.c5`, `opus:7-appC-beam.c6`, p. 304 |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L339-L369) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L325-L355); inherited (TC64 has the same counter, and forces the long-frame bit to 0 only while its RTG shadow of BEAMCON0 has DISPLAYDUAL set) |
| **Test** | [`integration/aa-integ-s3`](../../integration/aa-integ-s3): `make -k -C integration/aa-integ-s3 test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. The whole design at `nanomig.v`'s ports, once with each CPU core. [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking): `make -C hrm/ch3-beam-counters-and-blanking test`, `agnus_beamcounter` alone; its five reds are other findings', so it exits nonzero |
| **Assertions** | 1 red on fx68k and 1 on TG68K, in a package run of 196 passed, 10 failed on each core: `LF-FRAME` expects fields F1 and F2 of VTOTAL + 1 = 1536 lines at VTOTAL = $05FF, and observes 1537 and 1537. Green on the same run: `LF-CONTROL` (1281 and 1281 lines at VTOTAL = $04FF, 256 fewer), `LF-PC-SETUP`, `LF-PC-FRAME` and `LF-PC-TAG`. The run's other nine reds are not this finding's: `S5-SH1`, `S6`, `J2`, `J3`, `S7-PC-SH1` and `S7-PC-SH0` ([NM-AA-011](NM-AA-011.md)), and `S15-PC-GROUPS`, `S15-PC-LOM` and `S16-PC-ACCESS`, preconditions of the UHRES sprite lines, which find no UHRES access. In `hrm/ch3-beam-counters-and-blanking`, a run of 28 passed, 5 failed, `VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (non-interlaced)` is green: 6 lines at VTOTAL 5 with the bit cleared first; the five reds are [NM-HRM-010](NM-HRM-010.md)'s, [NM-HRM-068](NM-HRM-068.md)'s and [NM-HRM-011](NM-HRM-011.md)'s |
| **Verdict of the four** | **split, one of three with the page**: Copperline ends a programmed field after VTOTAL + 1 lines whatever the long-frame bit holds, with LACE set as well; WinUAE and Denise end it after VTOTAL + 1 lines plus the bit, which changes by itself only with LACE set, so with the bit set they count NanoMig's VTOTAL + 2 (Denise sets it at every reset, WinUAE clears it at a hard reset); vAmiga stores VTOTAL and acts on nothing. Hardware: not measured |
| **Effort** | S — `agnus_beamcounter.v`: the `last_line` assignment (line 369) takes the extra line only with LACE set or VARBEAMEN clear |
| **See also** | [NM-AA-009](NM-AA-009.md): the same bit puts the vertical sync at its long-field positions in a non-interlaced programmed frame; [NM-HRM-001](withdrawn/NM-HRM-001.md), withdrawn: with the bit set the hardwired PAL field has 313 lines, which p. 216 prints |

---

## Source requirement

**PDF p. 29**, the VTOTAL entry:

```
VTOTAL    H 1C8  W   A       Highest numbered vertical line (VARBEAMEN=1)
                    It's the line number to reset the counter,
                    so there's this many + 1 in a field. The exception is
                    if the LACE bit is set (BPLCON0), in which case every
                    other field is this many +2 & the short field is this
                    many + 1.
```

With VARBEAMEN set and LACE clear a field has VTOTAL + 1 lines, 1536 for VTOTAL = $05FF; only with LACE set is every other field VTOTAL + 2.

**The *Hardware Reference Manual* prints the same rule** for the ECS register, printed p. 304, Appendix C:

> VTOTAL contains the line number at which to reset the vertical position counter. This value represents the number of lines in a field(+1). The exception is if the INTERLACE bit is set (BPLCON0). In this case this value represents the number of lines in the long field (+2) and the number of lines in the short field (+1).

## RTL and measurement

**With VARBEAMEN set the counter's limit is VTOTAL**, `varbeamen ? vtotal_reg  : pal ? VTOTAL_PAL_VAL : VTOTAL_NTSC_VAL` ([`agnus_beamcounter.v#L229`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L229)). **While the long-frame bit is set, the field ends one line after it** ([`#L353-L369`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L353-L369)):

```verilog
wire vpos_equ_vtotal = (vpos==vtotal);
...
if (long_frame && vpos_equ_vtotal)
	extra_line <= 1'b1;
...
//in non-interlaced display the last line is equal to vtotal or vtotal+1 (depends on long_frame)
assign last_line = long_frame ? extra_line : vpos_equ_vtotal;
```

`extra_line` is set on the line step that leaves line VTOTAL, so `last_line` holds on line VTOTAL + 1, and `vpos` runs from 0 to VTOTAL + 1 before it returns to 0 ([`#L331-L335`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L331-L335)): VTOTAL + 2 lines. The bit is 1 from reset, `long_frame <= 1'b1;`, and changes only on a VPOSW write or at the end of a field with LACE set, `else if (end_of_frame && lace) // interlace` ([`#L341-L350`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L341-L350)).

**The package sees the field at the Copper.** The field's end, `assign eof = end_of_frame;` ([`#L374-L377`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L374-L377)), leaves the beam counter as `.eof(sof),` ([`agnus.v#L485`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L485)) and restarts the Copper at COP1LC, `if (reg_address_in[8:1]==COPJMP1[8:1] || sof)` ([`agnus_copper.v#L236`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L236)). [`integration/aa-integ-s3`](../../integration/aa-integ-s3) writes HTOTAL $00E2, VTOTAL $05FF and BEAMCON0 $08A0 and then BPLCON0 $1000 (`s3_std.h:41-52`), and no VPOSW. It times the Copper's fetches of the first word at COP1LC, one per field, and counts each field in line periods, the spacing of the Copper's per-line tag MOVEs (`s3_props_lf.h:31-39`); a second run at VTOTAL $04FF is the control.

```
MEASURED: LF: VTOTAL = 0x05FF: the field of F1 lasts 1537 lines, the field of F2 lasts 1537 lines (LF-FRAME expects 1536)
MEASURED: LF-CONTROL: VTOTAL = 0x04FF: the field of F1 lasts 1281 lines, the field of F2 lasts 1281 lines
PASS: LF-PC-FRAME: the Copper fetches at the COP1LC address exist for every frame used in the LF runs and are equally spaced
FAIL: LF-FRAME: with VARBEAMEN set, VTOTAL = 0x05FF and LACE clear a field has VTOTAL + 1 = 1536 lines (p.29, VTOTAL: 'It's the line number to reset the counter, so there's this many + 1 in a field'): the fields F1 and F2 of the long-frame run last exactly VTOTAL + 1 lines
PASS: LF-CONTROL: the field length follows VTOTAL line for line (p.29: 'this many + 1 in a field'): with VTOTAL = 0x05FF the fields F1 and F2 last exactly 256 lines more than with VTOTAL = 0x04FF
```

The same lines on fx68k and on TG68K: both fields are VTOTAL + 2 lines at both values. `LF-FRAME` is graded at `s3_props_lf.h:61-63` and `LF-CONTROL` at `:64-66`. In [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) the module alone, with the bit cleared by a VPOSW write, gives a field of 6 lines at VTOTAL 5, VTOTAL + 1: `VTOTAL-PLUS-ONE-PLUS-TWO-INTERLACE-FORMULA (non-interlaced)`, green (`test_agnus_beamcounter.cpp:1369-1388`).

## Risks and unknowns

- **The witnesses split, two to one.** WinUAE and Denise add the bit's line under VARBEAMEN, as NanoMig does; Copperline does not, and ignores the bit with LACE set too, where p. 29 gives the long field VTOTAL + 2. vAmiga does not use VTOTAL.
- **The run rests on the bit's reset value.** The program writes no VPOSW; NanoMig and Denise set the bit at reset, WinUAE clears it at a hard reset. p. 29 names LACE as the only exception and does not mention the bit.
- **Not measured on hardware**, and the document is marked PRELIMINARY. Probe: on an AA machine, with LACE clear, write VPOSW $8000, VTOTAL $05FF and BEAMCON0 $08A0 and count the lines between two vertical blanks: 1536 is p. 29's row, 1537 is NanoMig, WinUAE and Denise.
- **The readings.** The register row and the LACE line are `[3/3]` in the corpus; the other four lines of the entry, on which the finding rests, are `[1/3]`, lines with no identifier to corroborate. The Tesseract reading has each of them, up to spacing and apostrophes. The p. 304 sentence is the Developer CD reading; the scan's text layer has it with `+l` for `+1` and `BPLCONO` for `BPLCON0`.

## Independent evidence

The question put to each: with VARBEAMEN set, LACE clear and the long-frame bit set, does a field last VTOTAL + 1 lines?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:1893-1899`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1893-L1899), [`:2024`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2024), [`:10924-10938`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10924-L10938), [`:6686`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6686), [`:6739`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6739) | **no: VTOTAL + 1 plus the bit.** Under VARBEAMEN `maxvpos = vtotal + 1;`, and the field ends at `if ((vpos == maxvpos + lof_store) \|\| (vpos == maxvpos_long)) {`, where `maxvpos_long = (beamcon0 & BEAMCON0_VARBEAMEN) ? -1 : maxvpos + 1;` takes the second term out. The bit toggles there only with BPLCON0 bit 2, `if (bplcon0 & 4) {`, so with it set every field is VTOTAL + 2 lines, NanoMig's count. A hard reset clears it, `if (hardreset) {` ... `lof_store = lof_display = 0;` |
| **vAmiga** | [`Core/Components/Agnus/Agnus.h:136-146`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L136-L146), [`Core/Components/Agnus/AgnusRegs.cpp:603-607`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L603-L607), [`Core/Components/Agnus/Beam.h:86`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L86) | **cannot answer.** `pokeVTOTAL` stores the value, `vtotal = value;`, in a field under the comment *"ECS registers that are recognized and stored, but not yet acted upon."*; a PAL field is `isize vCntPal() const { return lof ? 313 : 312; }` whatever VTOTAL holds |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:1035-1037`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1035-L1037), [`emulation/libami/agnus/events.cpp:448-457`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L448-L457), [`emulation/libami/agnus/agnus.cpp:1000-1003`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1000-L1003), [`:218`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L218), [`:901`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L901) | **no: VTOTAL + 1 plus the bit.** `lines = (beamCon & VARBEAMEN) ? vTotal : (ntsc ? 261 : 311);`, and the counter restarts after `if (vPos == (lines + lof) ) {`, which toggles the bit only under `if (lace()) {`; its frame-rate estimate says the same, `linesPerField = lines + (lof ? 2.0 : 1.0);`. Every reset sets the bit, `lof = true;`, so a field is VTOTAL + 2 lines until a VPOSW write clears it, `lof = value & 0x8000;` |
| **Copperline** | [`src/chipset/agnus.rs:795-806`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L795-L806), [`:906-913`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L906-L913), [`:1098-1099`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1098-L1099), [`:2142-2151`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L2142-L2151), [`:105-107`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L105-L107) | **yes, and with LACE set too.** `current_frame_lines` returns the programmed total before it looks at the bit, under the comment *"VARBEAMEN programmable totals (VTOTAL+1) drive the beam entirely and override the fixed PAL/NTSC short/long-frame selection."*, `Some(u32::from(self.vtotal).saturating_add(1).max(1))` for every Agnus but the OCS one (`!matches!(self, Self::Ocs)`), and the counter wraps at `if self.vpos >= self.current_frame_lines() {`. Its own test asserts a 150-line frame at VTOTAL 149 with LACE set and the long frame selected, `assert_eq!(a.current_frame_lines(), 150);` |
| **MiSTer** | [`rtl/agnus_beamcounter.v:325-355`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L325-L355) | descendant — NanoMig's lines: `long_frame <= 1'b1;` at reset, toggled only under `else if (end_of_frame && lace) // interlace`, and `wire last_line = long_frame ? extra_line : vpos_equ_vtotal;` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:382-410`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L382-L410), [`:147-152`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L147-L152), [`:167`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L167) | descendant — the same counter, `assign last_line = long_frame ? extra_line : vpos_equ_vtotal;`, and one difference, `assign long_frame = displaydual ? 1'b0 : long_frame_r;  // Don't want long frames in RTG mode`, with `wire displaydual  = beamcon0_sh[ 6];` from a shadow register that a BEAMCON0 write fills only when bit 13 or bit 6 is set, `if(!rd && (data_in[13] \|\| data_in[6]))`. $08A0 sets neither, so TC64 counts VTOTAL + 2 here |

Copperline alone of the four ends this field after VTOTAL + 1 lines, and it does so by ignoring the bit whenever VARBEAMEN is set. WinUAE and Denise add the bit's line under VARBEAMEN, as NanoMig does; vAmiga does not use VTOTAL. MiSTer has NanoMig's lines; TC64 drops the extra line only in its RTG mode.

## Decision

**One option: take the extra line only with LACE set or VARBEAMEN clear.**

```verilog
-assign last_line = long_frame ? extra_line : vpos_equ_vtotal;
+assign last_line = (long_frame && (lace || !varbeamen)) ? extra_line : vpos_equ_vtotal;
```

With VARBEAMEN set and LACE clear the counter then wraps after line VTOTAL whatever the bit holds, 1536 lines at $05FF and 1280 at $04FF, so `LF-FRAME` would turn green and `LF-CONTROL` stay green. With LACE set the long field keeps VTOTAL + 2 and the short one VTOTAL + 1, as p. 29 gives them. With VARBEAMEN clear nothing changes, so the hardwired PAL field keeps the 313 lines of [NM-HRM-001](withdrawn/NM-HRM-001.md). The bit itself, VPOSR bit 15, is not touched, nor are the vertical sync positions that read it ([NM-AA-009](NM-AA-009.md)). Every other field run with VARBEAMEN set, LACE clear and the bit set, in any package, would be one line shorter. This is a reading of the code, not run by `make test`.
