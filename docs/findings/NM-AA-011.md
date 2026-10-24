# NM-AA-011 — SPRxCTL bits 4 and 3 (SH1, SH0) do not move a sprite: its start keeps the 140 ns step, where AA p. 28 gives them 70 ns and 35 ns [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `denise_sprites_shifter.v`, with the AA chip set selected, keep SPRxCTL bit 4 (SH1) and bit 3 (SH0) and move a sprite's start by 70 ns and 35 ns for them, as AA PDF p. 28 prints, where NanoMig keeps bits 7 and 0 of a SPRxCTL write only, so that setting SH1 alone or SH0 alone leaves the sprite where it was? Closes when the shifter stores both bits and applies them to the start, and `S5-SH1`, `S6`, `J2` and `J3` are green on both cores, or when an Amiga specialist rejects the change; if the rejection means p. 28 is wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | AA chip set selected (`chipset_config` = $10), PAL, a one-plane lores display (BPLCON0 = $1000), FMODE 0; all eight sprites DMA-loaded, each 16 lines, with one start H written over SPRxPOS and SPRxCTL as p. 28 splits it; SH1 alone ($440 against $442) with BPLCON3 = $0C00 (SPRES = 00, 140 ns sprite pixels on this display), SH0 alone ($440 against $441) with BPLCON3 = $0CC0 (SPRES = 11, 35 ns sprite pixels); on fx68k and TG68K. Not measured: other display resolutions, SPRES = 01 and 10, CPU-loaded control words (`S7` prints NOT GRADED), wide sprites. Chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 28, the SPRxPOS and SPRxCTL entries; PDF pp. 2, 5, 17, 18 and 31, quoted as context. The document prints no page numbers: p. N is the PDF page. *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p. 303 (Appendix C), as context · <details><summary>claim ids by assertion (15)</summary> `S5-SH1`: `opus:AA-sprxctl.c10`, `sol:AA-sprxctl.c24`, p. 28; `S6`: `opus:AA-sprxctl.c11`, `sol:AA-sprxctl.c26`, p. 28; `J2`: `opus:AA-sprxpos.c8`, `sol:AA-sprxpos.c14`, p. 28; `J3`: `opus:AA-sprxctl.c17`, `sol:AA-sprxctl.c31`, p. 28; `S5`, green: `opus:AA-sprxpos.c7`, `opus:AA-sprxctl.c14`, `sol:AA-sprxctl.c30`, p. 28; `S7`, not graded: `opus:AA-sprxctl.c15`, `opus:AA-sprxctl.c16`, `sol:AA-sprxctl.c25`, `sol:AA-sprxctl.c27`, p. 28. `S5-SH10-SH3`, `S6-CONTROL`, `S6-PC-WIDTH`, `S6-PC-HALF`, `S7-PC-SH1` and `S7-PC-SH0` carry no claim id</details> |
| **Module** | `denise_sprites_shifter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v#L256-L281) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites_shifter.v#L71-L96); inherited (TC64 has the same lines; NanoMig's `DENISE_EBR` branch of the same file, which the TG68K build compiles, has the same POS and CTL writes) |
| **Test** | [`integration/aa-integ-s3`](../../integration/aa-integ-s3): `make -k -C integration/aa-integ-s3 test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. The whole design at `nanomig.v`'s ports, once with each CPU core |
| **Assertions** | 4 red on fx68k and 4 on TG68K, in a package run of 196 passed, 10 failed on each core: `S5-SH1` expects setting SH1 alone to move the leftmost sprite pixel by 1 output pixel (70 ns) on every band line, and observes 0 on all 128; `S6` expects {V($AAAA, SH0 = 0), V($AAAA, SH0 = 1)} = {0, 8} and V($5555, 1) = V($AAAA, 0) on every band line, and observes 8 and 8 on the first; `J2` and `J3` are red because `S5-SH1` and `S6` are. Green on the same run: `S5` (SH2 to SH10 move the sprite by 2 to 512 output pixels, as p. 28 gives them), `S5-SH10-SH3`, `S6-CONTROL` (the data word moved by one 35 ns pixel flips V), `S6-PC-WIDTH` and `S6-PC-HALF`. `S7-PC-SH1` and `S7-PC-SH0`, preconditions that ask the same two bits to act in CPU-loaded sprites, are red with them, and `S7` prints NOT GRADED. The run's other four reds are not this finding's: `S15-PC-GROUPS`, `S15-PC-LOM` and `S16-PC-ACCESS`, preconditions of the UHRES sprite lines, which find no UHRES access, and the field-length line `LF-FRAME` ([NM-AA-012](NM-AA-012.md)) |
| **Verdict of the four** | **split, one of four**: WinUAE stores bit 3 as a 35 ns step in AGA mode and matches it when its own display runs at SuperHires resolution; vAmiga and Copperline model the AA chip set and read bit 3 nowhere, as NanoMig does; Denise has no AA Denise and cannot answer. Bit 4 is read by all three that model the AA chip set, but none of them moves a 140 ns sprite on a lores display by it, as `S5-SH1` asks, except WinUAE when it draws at Hires or SuperHires resolution: vAmiga applies it to 70 ns and 35 ns sprites, Copperline only with BPLCON0's SHRES bit set. Hardware: not measured |
| **Effort** | M — `denise_sprites_shifter.v` and `denise_sprites.v`: store SPRxCTL bits 4 and 3 with the AA chip set (the CTL write at lines 276–281, and 71–76 in the `DENISE_EBR` branch), bring `aga` into the shifter, delay each sprite's load by up to three 28 MHz clocks after the 7 MHz match, and count its shift strobe from that load |
| **See also** | [NM-HRM-019](NM-HRM-019.md): SPRxCTL bit 4 on the same register line, for the ECS chips in SuperHires; [NM-HRM-013](withdrawn/NM-HRM-013.md), withdrawn: the *Hardware Reference Manual* prints bit 3 `(unimplemented)` for the ECS chips |

---

## Source requirement

**PDF p. 28** gives the horizontal start as one value spread over two registers, SPRxPOS with the high eight bits:

```
SPRxPOS    140  W   A D      Sprite x Vert-Horiz start position data
...
                    07-00    SH10-SH3  Sprite horizontal start value.  Low-order
                                       3 bits are in SPRxCTL register below. If
```

and SPRxCTL with the low three, each with its own step:

```
SPRxCTL  p 142  W   A D      Sprite x position and control data
...
                    04       SH1=0       start horiz. value, 70nS increment
                    03       SH0=0       start horiz. value, 35nS increment
...
                    00       SH2       start horiz. value, 140nS increment
                    These 2 registers work together as position, size and
                    feature Sprite control registers. They are usually loaded
```

**The step holds in every resolution.** PDF p. 2:

```
    Resolution of bitplane scroll, display window, and horizontal
    sprite position has been improved to 35ns in all resolutions.
```

PDF p. 5, *Horizontal Comparators*:

```
        All programmable comparators with the exception of VHPOSW have
35nSec resolution.:DIWHIGH,HBSTRT,HBSTOP,SPRCTL,BPLCON1. BPLCON1 has
additional high-order bits as well. Note that horizontal bit position
representing 140nSec resolution has been changed to 3rd least
significant bit,where before it used to be a field's LSB. For example,
```

BPLCON3's SPRES table, PDF pp. 17 and 18, sets the sprite pixel apart from the display's:

```
       0   |   0    | ECS defaults(LORES,HIRES=140nS,SHRES=70nS
       0   |   1    | LORES(140nS)
       1   |   0    | HIRES(70nS)
   1     |    1     | SHRES(35nS)
```

and PDF p. 31:

```
All sprites can now be displayed in either:
...
4) 35ns
on any display resolution. eg 35ns sprites on a lores screen, or 140ns sprites
```

Read together: with the AA chip set the start is one value SH10–SH0 in 35 ns units, SH1 its 70 ns bit and SH0 its 35 ns bit, on any display resolution.

**The *Hardware Reference Manual* describes the chip set before this one**, and prints less for the same two bits, printed p. 303, Appendix C:

> 04      SHSH1     Start horizontal (SHR mode) 70ns increment

> 03      SHSH0     Start horizontal (SHR mode) 35ns (unimplemented)

## RTL and measurement

**A write to SPRxCTL keeps bits 7 and 0 and no other.** The fx68k build compiles the shifter without `DENISE_EBR` ([`denise_sprites_shifter.v#L270-L281`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v#L270-L281)):

```verilog
if (aen && address==POS)
  hstart[8:1] <= data_in[7:0];
...
if (aen && address==CTL)
  {attach,hstart[0]} <= {data_in[7],data_in[0]};
```

The TG68K build defines `DENISE_EBR` and compiles the file's other branch, whose POS and CTL writes are the same lines ([`#L64-L76`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v#L64-L76)). `hstart` is nine bits, SH10 to SH2 in p. 28's names, and neither branch has an `aga` input, so selecting the AA chip set changes nothing here.

**The start is compared once per lores pixel**, against Denise's nine-bit horizontal counter, which counts lores pixels and advances once per 7 MHz enable ([`denise.v#L132-L142`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L132-L142)), and the shifter loads on that match: `load <= armed && (hpos[7:0] == hstart[7:0]) && ...` ([`denise_sprites_shifter.v#L257-L260`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v#L257-L260); in the `DENISE_EBR` branch [`#L142`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v#L142) and [`#L150-L153`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v#L150-L153)).

**The pixels can be 35 ns; the start cannot.** SPRES reaches the sprites with the AA chip set, `assign spres    = bplcon3[7:6] & {2{aga}};` ([`denise.v#L252`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L252)), and SPRES = 11 shifts a sprite on every 28 MHz clock, `2'b11   : shift = 1'b1;` ([`denise_sprites.v#L429-L437`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites.v#L429-L437); in the `DENISE_EBR` branch [`#L126-L134`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites.v#L126-L134)). So a sprite of 35 ns pixels starts on a 140 ns boundary, whatever bits 4 and 3 of SPRxCTL hold.

[`integration/aa-integ-s3`](../../integration/aa-integ-s3) runs one program per setting with the AA chip set selected and a one-plane lores display, and builds the TG68K core with `DENISE_EBR` and the fx68k core without it (the harness's nanomig.mk). Sprite *x* is a DMA sprite of 16 lines from line $40 + $18*x*; all eight start at the same H, counted in 35 ns units and split over the two control words as p. 28 prints them: SPRxPOS bits 7–0 = H >> 3, SPRxCTL bit 0 = bit 2 of H, bit 4 = bit 1, bit 3 = bit 0 (`s3_std.h:106-115`). One output pixel is 70 ns of the line, which the package measures rather than assumes (`S5-PC-PIXEL`, `S6-PC-PIXEL`, *u* = 1).

- **SH1.** With SPRES = 00 the sprite pixel on this display is 140 ns, two output pixels. A sprite of DATA = $8000 starts at H = $440 and, in a second run, at $442, SH1 alone; p. 28 moves its leftmost pixel by 1 output pixel. The same comparator, applied to SH2 to SH10 (H = $444 to $640), is `S5` (`s3_props_s567.h:85-133`).
- **SH0.** With SPRES = 11 a 16-pixel sprite of 35 ns pixels covers 8 output pixels, and one of each pair of its pixels reaches the output (`S6-PC-HALF`). V(Π, SH0) counts the lit output pixels in a 12-pixel window placed on the $FFFF run with the same SH0. A 35 ns move of the start carries the lit pixels of $AAAA onto the hidden half of each pair, so p. 28 predicts {V($AAAA, 0), V($AAAA, 1)} = {0, 8} and V($5555, 1) = V($AAAA, 0) (`s3_props_s567.h:140-193`).

```
MEASURED: S5: k = 1: H = 0x440 -> 0x442: leftmost cS pixel moved by 0 output pixels (page: 1) on 128 lines, 128 lines differ
MEASURED: S5: k = 2: H = 0x440 -> 0x444: leftmost cS pixel moved by 2 output pixels (page: 2) on 128 lines, 0 lines differ
FAIL: S5-SH1: opus:AA-sprxctl.c10 sol:AA-sprxctl.c24: SPRxCTL bit 4 is SH1 and moves the sprite by 70 ns (p.28 '04 SH1=0 start horiz. value, 70nS increment'): changing SH1 alone moves the leftmost cS pixel by 1 output pixel, for every sprite and on every line of its band
MEASURED: S6: 128 band lines; first band line: V(0xAAAA, SH0 = 0) = 8, V(0xAAAA, SH0 = 1) = 8
PASS: S6-PC-HALF: V(0xAAAA, s) + V(0x5555, s) = 8 for s = 0 and s = 1 on every band line: exactly one of each pair of 35-ns pixels reaches the output
FAIL: S6: opus:AA-sprxctl.c11 sol:AA-sprxctl.c26: SPRxCTL bit 3 (SH0) moves the sprite by 35 ns (p.28): with 35-ns sprites {V(0xAAAA, 0), V(0xAAAA, 1)} = {0, 8} and V(0x5555, 1) = V(0xAAAA, 0) on every band line
PASS: S6-CONTROL: the comparator of S6 ({a, b} = {0, 8}) applied to the data words instead of SH0: {V(0xAAAA, 0), V(0x5555, 0)} = {0, 8} on every band line, the 0x5555 word being the 0xAAAA word shifted by one 35-ns sprite pixel
FAIL: J2: opus:AA-sprxpos.c8 sol:AA-sprxpos.c14: SPRxPOS bits 7-0 are SH10-SH3 and the low-order 3 bits are in SPRxCTL (p.28); it passes only if S5 (SH10-SH2), S5-SH1 (SH1) and S6 (SH0) pass; parts: S5 PASS; S5-SH1 FAIL; S6 FAIL
```

The same lines on fx68k and on TG68K, the two builds running the two branches of the shifter. Setting SH1 leaves every band line where it was, while SH2 moves it by the 2 output pixels p. 28 gives; setting SH0 leaves the shown half of each pixel pair where it was, while moving the data word by one 35 ns pixel moves it (`S6-CONTROL`). `S5-SH1` is graded at `s3_props_s567.h:127-129`, `S5` at `:131-133`, `S6` at `:192-193` and `S6-CONTROL` at `:194-195`; `J2` at `s3_props_j.h:56-57` and `J3`, which also needs `S8`, `S9`, `S10` and `S12` (all green), at `:58-60`.

## Risks and unknowns

- **The AA specification is marked PRELIMINARY**, and the *Hardware Reference Manual* prints bit 3 `(unimplemented)`, and bit 4 for SuperHires only, for the chip set before it.
- **Not measured on hardware:** where a 35 ns or 70 ns start falls within a 7 MHz cycle on AA silicon, and whether SH1 and SH0 act for every sprite resolution. p. 2 and p. 5 give the 35 ns comparator to every resolution; if a chip drops SH0, or SH1, for coarser sprites, the change under *Decision* needs the same condition, and `S5-SH1`, which asks SH1 to move a 140 ns sprite, asks more than that chip does.
- **Probe that would settle it:** on an AA machine with a lores display, a sprite of $8000 with SPRES = 00 and SPR0CTL bit 4 clear and then set, and one of $AAAA with SPRES = 11 and bit 3 clear and then set, measured in 35 ns steps: a move of 70 ns and of 35 ns is p. 28's row; no move is NanoMig, vAmiga and Copperline.
- **The implementations split.** Two of the three that model the AA chip set read bit 3 nowhere and drop bit 4 for the 140 ns sprite on a lores display that `S5-SH1` uses; the one that reads both applies them according to its own display setting, which is not an Amiga register.
- **This finding is graded on a lores display, with SPRES = 00 and 11, DMA-loaded sprites and FMODE 0 only.**
- **The readings.** The SPRxCTL rows, the SH10-SH3 row and the SPRES rows are `[3/3]` in the corpus; the continuation `3 bits are in SPRxCTL register below.`, on which `J2` rests, and the two lines from `These 2 registers work together`, on which `J3` rests, are `[1/3]`, as are p. 2's sentence, p. 5's note on the 140 ns bit and p. 31's list, lines with no identifier to corroborate. The Tesseract reading of pp. 2, 5 and 31 has each of their quoted lines, up to spacing.

## Independent evidence

The question put to each: with the AA chip set, do SPRxCTL bits 4 and 3 move a sprite by 70 ns and 35 ns?

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2822-2837`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2822-L2837), [`:3394-3399`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3394-L3399), [`:5380-5386`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5380-L5386), [`:3270-3272`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3270-L3272) | **yes, at the precision of its own display.** The start is kept in 35 ns units, `int sprxp = (s->pos & 0xff) * 2 + (s->ctl & 1);` shifted left by two, and in AGA mode bit 4 adds 70 ns, `sprxp \|= 2;`, and bit 3 adds 35 ns, `sprxp \|= 1;`; on ECS only bit 4 is read. The AGA comparison masks both positions, `if ((cnt & denise_xposmask) == (sp->xpos & denise_xposmask)) {`, with `denise_xposmask \|= hresolution == RES_SUPERHIRES ? 3 : (hresolution == RES_HIRES ? 2 : 0);`, and `hresolution = currprefs.gfx_resolution;` is the emulator's display setting, not an Amiga register: bit 3 acts in every display resolution when WinUAE draws at SuperHires resolution, and is dropped when it draws at Hires or Lores; bit 4 acts when it draws at Hires or SuperHires |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:168-177`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L168-L177), [`:961-969`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L961-L969), [`Core/Components/Denise/Denise.h:865-872`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L865-L872), [`Core/Components/Denise/Denise.cpp:924-931`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L924-L931), [`Core/Components/Denise/DeniseRegs.cpp:843-854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L843-L854), [`:132-134`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L132-L134) | **bit 3 no; bit 4 for 70 ns and 35 ns sprites.** vAmiga models an AA Denise, `u16 result = isAGA() ? 0x00F8 : isECS() ? 0xFFFC : 0xFFFF;`, and builds the sprite's start from SPRxPOS, SPRxCTL bit 0 and bit 4 in 70 ns units, `(sprctl[x] & 0x10) >> 4);  // SHSH1 (ECS only)` (its comments use the HRM's names: its "SH0" is SPRxCTL bit 0, the AA list's SH2); the buffer position doubles it, `sprhppos[x] = 2 * sprhpos[x] + 4 - 8 * HBLANK_MIN;`, so no start falls on an odd 35 ns position. Bit 3 is read nowhere. The start is then masked to the sprite's pixel width, `Pixel strt1 = sprStrt(sprite1) & hposMask;` with `return ~(sprPixelSize<R>() - 1);`, four buffer entries for a lores sprite, which SPRES = 00 gives on a lores display (`return shres(spriteBplcon0) ? 2 : 4;`, `default: drawSprites<Resolution::LORES>(); break;`), so bit 4 is dropped for a 140 ns sprite |
| **Denise** | [`emulation/libami/video/denise.h:22`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.h#L22), [`emulation/libami/video/denise.cpp:294-301`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L294-L301) | **cannot answer.** Its Denise models the OCS and ECS parts and no AA one, `enum Model : uint8_t { OCS_A1000_NO_EHB = 1, OCS_A1000 = 2, OCS = 4, ECS = 8 } model = OCS;`. `setSprCtl` reads bits 0, 4 and 7 of SPRxCTL, `spr.x \|= value & 1;`, `spr.shsh10 = (value & 0x10) >> 3;`, `spr.attached = (nr & 1) && (value & 0x80);`, and not bit 3 |
| **Copperline** | [`src/video/bitplane/sprite.rs:1853-1859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1853-L1859), [`:1712`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1712), [`:177`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L177), [`:245`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L245) | **bit 3 no; bit 4 only in SuperHires.** Copperline models Lisa, `aga: matches!(state.agnus_revision, AgnusRevision::AgaAlice),`, with comments such as *"Lisa only: FMODE SPR32/SPAGEM widen manual sprites too."* The start is SPRxPOS and SPRxCTL bit 0 in 140 ns units, `(((pos & 0x00FF) << 1) \| (ctl & 0x0001)) as i32`, plus bit 4 as one 70 ns step only while BPLCON0's SHRES bit is set, `base_x + i32::from(hsub_70ns && control.shres())`. Bit 3 is read nowhere |
| **MiSTer** | [`rtl/denise_sprites_shifter.v:71-96`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites_shifter.v#L71-L96) | descendant — the identical line, `{attach,hstart[0]} <= {data_in[7],data_in[0]};`, and the same once-per-7 MHz compare, `load <= armed && (hpos[7:0] == hstart[7:0]) && (fmode[15] \|\| (hpos[8] == hstart[8])) ? 1'b1 : 1'b0;` |
| **TC64** | [`rtl/minimig/denise_sprites_shifter.v:67-92`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_sprites_shifter.v#L67-L92) | descendant — the identical lines. Neither line stores bit 4 or bit 3 for NanoMig to have dropped |

WinUAE alone moves a sprite by bit 3, and then only at its SuperHires display setting. By bit 4 WinUAE moves one at its Hires and SuperHires settings, vAmiga one of 70 ns or 35 ns pixels and Copperline one on a SuperHires display, so only WinUAE, at those two settings, moves `S5-SH1`'s 140 ns sprite on a lores display. MiSTer and TC64, descendants, have NanoMig's register line.

## Decision

**One option: keep SPRxCTL bits 4 and 3 with the AA chip set and use them as the two low bits of the start, in 35 ns units.** The register line would take them,

```verilog
-      {attach,hstart[0]} <= {data_in[7],data_in[0]};
+      {attach,hstart[0],hsub} <= {data_in[7],data_in[0],data_in[4:3] & {2{aga}}};
```

in both branches, with `aga` brought into the shifter, and the load would move by `hsub` 28 MHz clocks after the 7 MHz match, 35 ns each; the shifter already runs on the 28 MHz clock. A sprite of 35 ns pixels shifts on every clock, so the later load moves all of it. For 70 ns and 140 ns pixels [`denise_sprites.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites.v#L429-L437) derives one `shift` strobe for all eight sprites from the clock phases, `default : shift = ~c1 & ~c3;`, so a later load alone would shorten the first pixel and leave the rest in place; each sprite's strobe would have to count from its own load. Bit 4 is [NM-HRM-019](NM-HRM-019.md)'s, whose suggestion confines it to SuperHires for the ECS chips; with the AA chip set p. 2 gives both bits in every resolution. WinUAE's position word ([`drawing.cpp:2822-2837`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2822-L2837)), `(pos & 0xff) * 2 + (ctl & 1)` shifted left by two with bit 4 adding 2 and bit 3 adding 1, is the smallest complete model. This is a reading of the code, not run by `make test`.
