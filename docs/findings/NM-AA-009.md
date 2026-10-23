# NM-AA-009 — with CSCBEN set, VSY* keeps the vertical sync and VBSTRT never reaches it, where AA pp. 12 and 29 put the variable composite blank on that pin [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `agnus_beamcounter.v`, with BEAMCON0 bit 10 (CSCBEN) set and VARVSYEN clear, put the variable composite blank that VBSTRT and VBSTOP set on the VSY* pin, as AA PDF pp. 12 and 29 print, where NanoMig's VSY* carries the vertical sync alone and moving VBSTRT from 150 to 152 changes none of its samples? Closes when the pin carries the blank and `P9-BV`, `P9-B` and `P9-ID` are green on both cores (`P9-BV` may also need the comparator vertical sync to start where p. 12 puts it, see *Risks and unknowns*), or when an Amiga specialist rejects the change; if the rejection means pp. 12 and 29 are wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | AA chip set (`chipset_config` = $10), BEAMCON0 with PAL and VARBEAMEN set, CSCBEN set ($04A0) against clear ($00A0), VARVSYEN clear; HTOTAL $00E2, VTOTAL $0138, VSSTRT 100, VSSTOP 103, VBSTRT 150 against 152, VBSTOP 170; VSY* as `nanomig.v` presents it behind the scan doubler, on fx68k and TG68K; the HSY* half of the same sentence is not observable at this boundary, because the scan doubler regenerates `hs`; chip revision unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 12, the BEAMCON0 entry; PDF p. 29, the VBSTOP/VBSTRT entry. The document prints no page numbers: p. N is the PDF page. *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p. 305 · `P9-B`: `opus:AA-beamcon0.c16`, `sol:AA-beamcon0.c34`, p. 12; `P9-ID`: `opus:AA-beamcon0.b10`, `sol:AA-beamcon0.c11`, p. 12; `P9-A` (not graded): `opus:AA-beamcon0.c15`, `sol:AA-beamcon0.c33`, p. 12, `opus:AA-beamcon0.c38`, p. 13; `P9-BV`, `P9-NV`, `P9-AH`, `P9-AC`, `F5-SV-DIFF`, `P10-VPAIR` and `P10-VSPAIR` carry no claim id |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L399-L406) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L385-L392); inherited (TC64 has `cscben` as a live wire that nothing reads, stores VBSTRT for its VARVBEN blank only, and forms `_vsync` as NanoMig does) |
| **Test** | [`integration/aa-integ-s2`](../../integration/aa-integ-s2): `make -k -C integration/aa-integ-s2 test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. The whole design at `nanomig.v`'s ports, once with each CPU core |
| **Assertions** | 3 red on fx68k and 3 on TG68K, in a package run of 131 passed, 13 failed on each core: `P9-BV` expects that changing VBSTRT from 150 to 152 with CSCBEN set changes the VSY* output only inside one interval of at most two line periods, starting 49 to 51 line periods after the start of V_var, and observes 0 intervals; `P9-B` (`opus:AA-beamcon0.c16`, `sol:AA-beamcon0.c34`) expects the variable composite blank on VSY* following VBSTRT with CSCBEN set, and is red because `P9-BV` is; `P9-ID` (`opus:AA-beamcon0.b10`, `sol:AA-beamcon0.c11`) expects bit 10 to route the blank to VSY*, and is red because `P9-B` is. `P9-A`, `P9-AH` and `P9-AC`, the HSY* half of the same sentence, print NOT GRADED: anchor S-H prints NOT CHECKABLE, because the harness shows `hs` only after the scan doubler. In the same run `F5-SV-DIFF`, `P9-NV`, `P10-VPAIR` and `P10-VSPAIR` are green on the same pin. The run's ten other reds are not this finding's: `F5-SV-INV` grades VSYTRUE on the same pin ([NM-HRM-032](NM-HRM-032.md)), `P11-HHRUN` and `P12-K` HHPOSR ([NM-AA-008](NM-AA-008.md)), `P12-UHRES` the UHRES pointers ([NM-AA-010](NM-AA-010.md)), `P13-0-LINES` and `P13-0-COUNT` the long line under a programmed line length ([NM-AA-005](NM-AA-005.md)), `P14-0-LINES` and `P14` the NTSC long line ([NM-HRM-010](NM-HRM-010.md)), and the two light-pen lines ([NM-HRM-087](NM-HRM-087.md)) |
| **Verdict of the four** | **none of the four puts a composite blank on VSY\***: WinUAE takes its vertical sync signal from the vertical sync alone and reads CSCBEN only to blank its display when a monitor with separate H and V syncs is configured; vAmiga names the bit in a comment; Denise has no constant for it and no VSY* signal; Copperline latches it *"without physical host output"*. Hardware: not measured |
| **Effort** | M — `agnus_beamcounter.v`: decode CSCBEN, store VBSTRT, and drive `_vsync` from the variable vertical blank when CSCBEN is set and the comparator vertical sync does not have priority, with the vertical sync kept for `_csync` |
| **See also** | [NM-HRM-032](NM-HRM-032.md): the polarity bits of the same pins, in the same register |

---

## Source requirement

PDF p. 12 prints BEAMCON0's bits in a table. Three rows bear on this finding:

```
                    10     CSCBEN
                     9     VARVSYEN
                     3     (unused, formerly BLANKEN)
```

and, below the table, what bit 10 does:

```
                    CSCBEN= The variable composite sync comes out on the HSY*
                     pin, and the variable composite blank comes out on the VSY*
                     pin. The idea is to allow all the information to come out
                     of the chip for a DUAL mode display.  The normal monitor
                     uses the normal composite sync, and the variable composite
                     sync & blank come out the HSY* & VSY* pins. The bits VARVSYEN
                     & VARHSYEN (below) have priority over this control bit.
```

The same page says where the variable vertical blank comes from:

```
                    VARVBEN= Use the comparator generated Vertical Blank
                     (from VBSTRT,VBSTOP) to run the internal chip stuff-
```

PDF p. 29 prints VBSTOP and VBSTRT as one entry, with one note for both:

```
VBSTOP    H 1CE  W   A       Vertical line for VBLANK stop
VBSTRT    H 1CC  W   A       Vertical line for VBLANK strt
                    (V10-0 <- D10-0) Affects CSY* pin if BLANKEN=1 &
                    VSY* Pin if CSCBEN=1 (see BEAMCON0)
```

Read together: with CSCBEN set and VARVSYEN clear, the VSY* pin carries the variable composite blank, and VBSTRT, the line on which the vertical blank starts, moves it. With VARVSYEN set, the comparator vertical sync has priority and VSY* carries that instead.

The Hardware Reference Manual divides the same work between two bits. Its BEAMCON0 table, printed p. 305, lists

> 10 CSCBEN Composite sync redirection

and

> 3 BLANKEN Composite blank redirection

## RTL and measurement

**CSCBEN is decoded nowhere, and VBSTRT is not stored.** The wire (`//wire cscben       = beamcon0_reg[10];`, [`agnus_beamcounter.v#L132`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L132)), VBSTRT's register ([`#L187`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L187)), its reset value ([`#L202`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L202)), its write ([`#L215`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L215)) and the wire that would select it ([`#L232`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L232)) are all comments. The vertical blank that does exist starts at line 0 whatever is written: the block that compared `vpos` with `vbstrt` is itself inside a comment, and the line after it reads `assign vbl = (vpos <= vbstop);` ([`#L429-L444`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L429-L444)).

**`_vsync`, the VSY* output, is the vertical sync and nothing else.** It is set and reset from VSSTRT, VSSTOP, HSSTRT and HCENTER alone, [`#L399-L406`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L399-L406):

```verilog
if ((vpos==vsstrt+1 && hpos==hsstrt && long_frame) || (vpos==vsstrt && hpos==hcenter && !long_frame))
	_vsync <= 1'b0;
else if ((vpos==vsstop && hpos==hcenter && long_frame) || (vpos==vsstop && hpos==hsstrt && !long_frame))
	_vsync <= 1'b1;
```

with `vsstrt` and `vsstop` the programmed values only when VARVSYEN and VARBEAMEN are both set ([`#L230-L231`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L230-L231)). No term of it names BEAMCON0 bit 10 or a blank. The package sees that output: `_vsync` leaves the module on port [`#L43`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L43), passes `agnus` and `minimig` unchanged ([`agnus.v#L476`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L476), [`minimig.v#L465`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L465)), becomes `vs_in` in `nanomig.v` ([`#L1570`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/nanomig.v#L1570)) and reaches the `vs` port through the scan doubler, which `nanomig.v` enables ([`#L1626`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/nanomig.v#L1626)) and which passes the vertical sync through one clock later ([`Amber.v#L183`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig/Amber.v#L183)). The horizontal sync reaches the harness only as the scan doubler regenerates it ([`Amber.v#L182`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig/Amber.v#L182)), so this page is about VSY*.

[`integration/aa-integ-s2`](../../integration/aa-integ-s2) samples `nanomig.v`'s `vs` once per system clock over a whole graded frame of 313 lines and compares two runs that differ in one register, sample by sample. In every run of this part BEAMCON0 has PAL and VARBEAMEN set, HTOTAL is `$00E2`, VTOTAL `$0138`, VSSTRT 100, VSSTOP 103, VBSTRT 150 and VBSTOP 170 (`test_aa_integ_s2.cpp:2189-2191`).

- **The pin is live.** With VARVSYEN set ($02A0), moving VSSTOP from 103 to 105 changes `vs` in exactly one interval of two line periods (`F5-SV-DIFF`, green).
- **With CSCBEN set it does not move.** With CSCBEN set and VARVSYEN clear ($04A0), moving VBSTRT from 150 to 152 changes no sample. `P9-BV` is red because the pair differs in no sample at all, so no choice of window, polarity or horizontal content would turn it green. `P9-NV`, the same pair with CSCBEN clear, is green whatever CSCBEN does, because VBSTRT reaches nothing.
- **VSY* is still the vertical sync with CSCBEN set.** With CSCBEN and VARHSYEN set and VARVSYEN clear, a change of HCENTER moves `vs` by 64 system clocks, 8 colour clocks, at the line where the hardwired vertical sync ends: [line 403](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L403) ends the vertical sync of a long frame at `hpos==hcenter`, and VARHSYEN makes `hcenter` the programmed value ([`#L226`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L226)). The package prints that pair as a MEASURED line and does not grade it.

```
MEASURED: F5 measured: S-V1 against S-V2, vs: 1 intervals [line 102, length 3632]
MEASURED: F5 measured: V_var length 4540 system clocks starting in line 100; V_norm length 4540 starting in line 2
MEASURED: P9 measured: CSCBEN set, VBSTRT 150 against 152, vs: 0 intervals
FAIL: P9-BV: with CSCBEN set, changing VBSTRT from 150 to 152 changes the VSY* output only inside one interval of at most two line periods that starts between 49 and 51 line periods after the start of V_var, each of its two line periods holding differing samples
PASS: P9-NV: with CSCBEN clear, changing VBSTRT from 150 to 152 does not change the VSY* output at any sample (the HSY* half is not observable at this boundary)
MEASURED: P10 measured: CSCBEN and VARHSYEN, HCENTER $91 against $99, hs: 0 intervals; same pair, vs: 1 intervals [line 4, length 64]
```

The same lines, in the same order, on fx68k and on TG68K. `P9-BV` is graded at `test_aa_integ_s2.cpp:2434`, `P9-B` at `:2439-2440` and `P9-ID` at `:2441-2442`; `F5-SV-DIFF` at `:2340`, `P9-NV` at `:2435`, `P10-VPAIR` at `:2474` and `P10-VSPAIR` at `:2475`.

## Risks and unknowns

- **Closing `P9-BV` may need a second change.** `P9-BV` counts its window from the start of the comparator vertical sync, `V_var`, which the package expects at HSSTRT of line VSSTRT (p. 12: *"the horizontal position for set & reset HSSTRT on short fields (all fields are short if LACE=0)"*). In this RTL a non-interlaced frame keeps `long_frame` at 1 (reset to 1, toggled only with LACE, [`#L341-L350`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L341-L350)), so the vertical sync takes the long-field positions of [`#L401`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L401): it starts on line VSSTRT+1 at HSSTRT. A blank that starts at the beginning of line 150 would then begin less than 49 line periods after `V_var`, outside the window; that is a reading of the code, not run. The package measures `V_var` at 4540 system clocks, two and a half line periods, and grades neither its length nor its start.
- **What an Alice puts on VSY\* with CSCBEN set and VARVSYEN clear is not measured.** Write VBSTRT and VBSTOP, set CSCBEN alone, and scope VSY*: a pulse that follows VBSTRT settles the finding; the vertical sync unchanged would move it to the printed page.
- **Polarity and content of the composite blank.** The page gives neither whether VSY* is low or high inside the blank, nor whether the horizontal blank (HBSTRT, HBSTOP) is part of it; the package grades neither. The option below drives the pin low inside the blank, as the pin's `*` name suggests, and uses the vertical part only.
- **The HRM and AA divide the work differently.** The HRM gives the composite blank redirection to BLANKEN (bit 3); the AA document marks bit 3 *"(unused, formerly BLANKEN)"* and gives the blank to CSCBEN, while its own p. 29 note still names BLANKEN for the CSY* pin. This finding rests on the CSCBEN half of that note and on the p. 12 paragraph, which agree.
- **No implementation models the pin**, so there is none to compare against; the document is marked PRELIMINARY; only the VSY* half of the p. 12 sentence is measured. The bit-table rows are tagged `[3/3]` in the corpus. The CSCBEN paragraph rests on two or three readings except its third and fifth lines, which rest on the model's reading alone; the VARVBEN lines rest on two or three readings and the two p. 29 note lines on two. The band images of the printed pages, `p12_band1.png`, `p12_band2.png` and `p29_band1.png`, show every line quoted here word for word.

## Independent evidence

The question put to each: with CSCBEN set and VARVSYEN clear, what comes out on VSY*, and does VBSTRT move it?

| | where | result |
|---|---|---|
| **WinUAE** | [`custom.cpp:9799-9803`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9799-L9803), [`:5198-5200`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L5198-L5200), [`cfgfile.cpp:261`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L261), [`:8691`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L8691), [`custom.cpp:6886`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6886), [`:8780`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L8780) | **no composite blank on the vertical sync signal**: `check_vidsyncs` takes its VSYNC flag from the programmed vertical sync with VARVSYEN and from the hardwired one otherwise, with no CSCBEN term. CSCBEN is read in `check_no_signal`: when the configuration's sync option is `hvsync` or `hvsync_s`, a monitor on separate H and V syncs, setting the bit blanks the emulated display; in the default, `hvcsync`, it does nothing there. It is also one of the bits that mark a programmed mode. WinUAE models what the bit does to such a monitor, not the signal on the pin |
| **vAmiga** | [`AgnusRegs.cpp:550-568`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L550-L568), [`:617-621`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L617-L621), [`Agnus.h:136-139`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L136-L139) | **does not model it**: `pokeBEAMCON0` names the bit in its comment and acts on PAL and LOLDIS only; `pokeVBSTRT` stores the value under the field comment *"ECS registers that are recognized and stored, but not yet acted upon"*. There are no sync pins |
| **Denise** | [`agnus.cpp:1-13`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1-L13), [`register.cpp:661-666`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L661-L666), [`agnus.cpp:586-589`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L586-L589), [`:557-558`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L557-L558) | **does not model it**: the BEAMCON0 bit names it defines run from `HARDDIS` to `CSYTRUE` and leave out bit 10. VBSTRT is stored and used for the VARVBEN blank. The one sync line the core drives towards the display is the composite sync into Denise; the vertical sync appears only as a CIA time-of-day tick at its hardwired end, `// hardwired vsync end`. There is no VSY* signal |
| **Copperline** | [`agnus.rs:21-25`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L21-L25), [`:35-36`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L35-L36), [`:916-928`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L916-L928) | **latches it and does nothing with it**, and says so: *"Composite-sync routing (VARCSYEN/CSCBEN) and the xSYTRUE pin polarities are latched without physical host output."* The constant carries `#[allow(dead_code)]`. VBSTRT is used for the VARVBEN blank |
| **MiSTer** | [`rtl/agnus_beamcounter.v:138`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L138), [`:193`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L193), [`:221`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L221), [`:385-392`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L385-L392), [`:415-430`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L415-L430) | descendant: NanoMig's code, `cscben` and `vbstrt_reg` as comments, the same `_vsync` block, and the VBSTRT comparison inside a comment above `assign vbl = (vpos <= vbstop);` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:163`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L163), [`:253`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L253), [`:301`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L301), [`:439-445`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L439-L445), [`:468-479`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L468-L479) | descendant: `wire cscben       = beamcon0_reg[10];` is declared and read nowhere in the file; VBSTRT is stored and starts the vertical blank under VARVBEN, which NanoMig does not do, but `_vsync` is NanoMig's block, so VSY* carries the vertical sync with CSCBEN set, as here |

## Decision

**One option: give the VSY* pin its own value, chosen as p. 12 orders it.** Decode CSCBEN, store VBSTRT, and drive the pin from the variable vertical blank when CSCBEN is set and the comparator vertical sync does not have priority; the vertical sync itself stays as it is and still forms `_csync`. In `agnus_beamcounter.v`:

```verilog
-	output reg        _vsync,         // vertical sync
+	output            _vsync,         // vertical sync
...
-//wire cscben       = beamcon0_reg[10];
+wire cscben       = beamcon0_reg[10];
...
-//reg [10:0] vbstrt_reg;
+reg [10:0] vbstrt_reg;
 ...                                          // and its reset value and its write, uncommented
+reg _vsync_r; // the vertical sync itself
 always @(posedge clk) begin
 	if (clk7_en) begin
 		if ((vpos==vsstrt+1 && hpos==hsstrt && long_frame) || (vpos==vsstrt && hpos==hcenter && !long_frame))
-			_vsync <= 1'b0;
+			_vsync_r <= 1'b0;
 		else if ((vpos==vsstop && hpos==hcenter && long_frame) || (vpos==vsstop && hpos==hsstrt && !long_frame))
-			_vsync <= 1'b1;
+			_vsync_r <= 1'b1;
 	end
 end
+
+wire vcblank   = (vpos >= vbstrt_reg) && (vpos < vbstop_reg);
+assign _vsync  = (cscben && !varvsyen) ? ~vcblank : _vsync_r;
...
-assign _csync = _hsync & _vsync | vser; //composite sync with serration pulses
+assign _csync = _hsync & _vsync_r | vser; //composite sync with serration pulses
```

p. 12 gives VARVSYEN priority over CSCBEN without naming VARBEAMEN, so the selector reads VARVSYEN alone. [NM-HRM-032](NM-HRM-032.md)'s VSYTRUE inversion, if it is made, applies to the same output after this selector. The change touches `agnus_beamcounter.v` alone, so every user of `_vsync` sees the pin's value, CIA-A's time-of-day tick included (`.tick(_vsync)`, [`minimig.v#L629`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L629)); a separate output for the pin would keep CIA-A on the vertical sync. This option has not been run by `make test`.
