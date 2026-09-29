# NM-HRM-071 — in SuperHires a sprite pixel lasts 70 ns, where p. 101 keeps it one low-resolution pixel, and Commodore's AA specification and every implementation give 70 ns

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. Chapter 4 makes a sprite pixel one low-resolution pixel whatever the playfield's resolution (pp. 101, 124, 128); p. 131 sends SuperHires sprites to Appendix C, which gives them 70 ns positioning and no width, and Commodore's AA specification lists the ECS default sprite resolution in SuperHires as 70 ns. NanoMig shows a 70 ns sprite pixel in SuperHires, and so do all four implementations and both Minimig lines. The question is which document describes the ECS Denise; the entry exists so the next reader does not take the red for an implementation fault. |
| **Module** | `denise.v`, `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L391) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L361) |
| **Report to** | **the manual, not the implementation.** NanoMig, both Minimig lines, all four implementations and Commodore's AA specification give an ECS sprite 70 ns pixels in SuperHires |
| **Test** | [`integration/ch4-shres-sprite-b`](../../integration/ch4-shres-sprite-b) · `PC2w`, a precondition that carries no claim id, run on both CPU cores, red on fx68k and on TG68K |
| **Chapter** | 4 — Sprite Hardware, printed pp. 101, 124, 128 and 131; Appendix C, printed pp. 297, 300 and 303; 1 — Introduction, printed p. 3 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red on each core: `PC2w` (27 passed, 1 failed on fx68k and on TG68K). The package's colour propositions `P1`–`P4` are green on both cores; the claims that state the width are graded nowhere (see *What grades it*) |
| **Verdict of the four** | **unanimous with NanoMig and with the AA specification**: WinUAE, vAmiga, Denise and Copperline all show an ECS sprite pixel for 70 ns in SuperHires and for 140 ns in the other resolutions. None keeps p. 101's low-resolution pixel in SuperHires; outside the emulators, the 8373R4 dissection shifts a sprite out two pixels per 140 ns in SuperHires, 70 ns each — see *What other Commodore documents say* |
| **See also** | [NM-HRM-016](NM-HRM-016.md) (SuperHires sprite colour encoding), [NM-HRM-019](NM-HRM-019.md) (SHSH1, SuperHires sprite positioning) |

---

## What the HRM says

### Chapter 4: a sprite pixel is a low-resolution pixel in every playfield mode

**Printed p. 101**, *Size of Sprites*:

> Sprites are 16 pixels wide and can be almost any height you wish -- as short as one line or taller than the screen.

> Sprite size is based on a pixel that is 1/320th of a screen's width,
> 1/200th of a NTSC screen's height, or 1/256 of a PAL screen's height. This
> pixel size corresponds to the low resolution and non-interlaced modes of
> the normal full-size playfield. Sprites, however, are independent of
> playfield modes of display, so changing the resolution or interlace mode
> of the playfield has no effect on the size or resolution of a sprite.

**Printed p. 124**, on the parallel-to-serial converters:

> Immediately after the data is transferred from the sprite data
> registers, each parallel-to-serial converter begins shifting the bits
> out of the converter, most significant (leftmost) bit first. The
> shift occurs once during each low resolution pixel time and continues
> until all 16 bits have been transferred to the display circuitry.

**Printed p. 128**, under *Sprite placement resolution*:

> The sprites have a placement resolution on a full screen of 320 by
> 200 NTSC (320 by 256 PAL). The sprite resolution is independent of
> the bitplane resolution.

Chapter 1 says it once more, on printed p. 3: "A sprite is 16 low resolution
pixels wide". None of these sentences names a chip set.

**Printed p. 131**, the last paragraph of the chapter, hands the enhanced chip
set's sprites to the appendix:

> For information relating to sprites in the Enhanced Chip Set (ECS),
> such as SuperHires sprites and SuperHires sprite positioning,
> see Appendix C.

### Appendix C: SuperHires sprites get 70 ns positioning, and no width

**Printed p. 300** fixes the pixel rates:

> SuperHires mode provides a 35ns pixel display rate - twice the horizontal
> resolution of Hires mode, and four times the Lores rates.

**Printed p. 297**:

> The new SuperHires mode provides 35ns pixel rates and sprite
> positioning at 70ns rates.

**Printed p. 303**, *SuperHires 70ns Sprite Positioning*:

> SuperHires mode has a finer granularity of sprite positioning than either
> Hires or Lores modes. This allows for positioning the sprite every other
> SuperHires pixel on 70ns boundaries. The ECS registers SPRxPOS and
> SPRxCTL work together as position, size and sprite feature control
> registers.

and in its SPRxCTL table and warning:

> 04      SHSH1     Start horizontal (SHR mode) 70ns increment

> 00      SH0       Start horiz. value Low bit 140 ns increment

> 70ns sprite positions are only available in SuperHires mode.

"Position, size and sprite feature control registers" is Appendix A's
description of the same two registers for the original chip set (p. 284,
"These two registers work together as position, size and feature
sprite-control registers"); neither page gives a width. Appendix C's other
SuperHires sprite page, p. 302, gives the colour encoding and nothing else.

**Read together, the manual says two things about a SuperHires sprite and is
silent on the third.** Its width is chapter 4's low-resolution pixel, by a rule
written without a chip set. Its position is not independent of the playfield's
resolution: p. 303 gives it a 70 ns step that exists "only" in SuperHires,
which is the resolution half of p. 101's "no effect on the size or resolution of
a sprite" and of p. 128's "independent of the bitplane resolution" revoked for
that mode. The width of a SuperHires sprite pixel is stated nowhere in
Appendix C, the part of the manual p. 131 sends the reader to.

---

## What NanoMig does

**In SuperHires it shifts a sprite out twice as fast as in the other
resolutions: one sprite pixel per 70 ns.** Denise feeds the sprite unit a
resolution code built from BPLCON3's SPRES field, which is forced to 0 outside
AGA, and BPLCON0's SHRES bit —
[`src/minimig-aga/denise.v#L390-L391`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L390-L391),
[`#L252`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L252):

```verilog
wire [1:0] spres_d;
assign spres_d = (spres == 2'b00) ? shres ? 2'b10 : 2'b01 : spres;
```

```verilog
assign spres    = bplcon3[7:6] & {2{aga}};
```

and the sprite unit turns the code into a shift enable —
[`src/minimig-aga/denise_sprites.v#L158-L165`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L158-L165):

```verilog
// shift value
reg shift;
always @ (*) begin
  case (spres)
    2'b11   : shift = 1'b1;
    2'b10   : shift = ~c1 ^ c3;
    default : shift = ~c1 & ~c3;
```

`c1` and `c3` are the 28 MHz-domain phases of the 7 MHz clock
([`src/minimig-aga/amiga_clk.v#L57-L61`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/amiga_clk.v#L57-L61)),
so `~c1 & ~c3` is true in one 28 MHz clock of four, a 140 ns sprite pixel, and
`~c1 ^ c3` in two of four, a 70 ns one. Each sprite shifter advances on it, in
both builds of the shifter the package runs: without `DENISE_EBR` (the fx68k
build),
[`src/minimig-aga/denise_sprites_shifter.v#L280-L283`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L280-L283),
`else if (shift)` / `shifta[63:0] <= {shifta[62:0],1'b0};`, and with it (the
TG68K build),
[`#L237-L241`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L237-L241),
`else if (shift) begin`. With SPRES at 0, SHRES selects code `2'b10`, the
encoding the AA specification's SPRES table gives to `HIRES(70nS)` (see
below).

`shres` is BPLCON0 bit 6 with no chip-set term,
[`#L187`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L187),
`assign shres    = bplcon0[6];`, while the bitplane unit receives it gated on
ECS,
[`#L362`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L362),
`.shres(shres & ecs),`. That difference shows under the original chip set
below.

### Measured

[`integration/ch4-shres-sprite-b`](../../integration/ch4-shres-sprite-b) runs
the whole design on each CPU core with an enhanced chip set and a one-plane
SuperHires display (BPLCON0 = $1040) and puts sprites 0 and 1 over it at
HSTART 192 and 217, 50 lines tall, their uniform lines all sixteen pixels of
one value. A *sample* is one pixel of the harness's line-doubled video output,
70 ns of the source line; the sprites' own positions check that, since 25
low-resolution pixels between the two HSTARTs (p. 303's SH0, "140 ns
increment") measure 50 samples. p. 101 with p. 300 then predicts a 16-pixel
sprite 32 samples wide.

`PC2w` reads the 32 samples from each sprite's first lit sample on every uniform
line: 5 bands × 6 lines × 2 output lines × 2 sprites × 32 samples = 3840, and
the sample either side of each run of 32, 240 more, which must be dark.

| run | BPLCON0 | chip set | `PC2w`: of 3840 samples | `a1 − a0` | fx68k | TG68K |
|---|---|---|---|---:|---|---|
| graded run | $1040, SuperHires | ECS | **1920 lit, 1920 dark** | 50 | **red** | **red** |
| contrast `CR4` | $9000, Hires | ECS | 3840 lit | 50 | green | green |

Each sprite is lit over exactly 16 samples of every uniform line: the run at
the first sprite's anchor is followed by dark samples up to the second sprite
50 samples on, and `PC2(e)`, green, finds the same set of lit offsets on all
120 uniform sprite lines. So a 16-pixel sprite is 16 samples wide, 70 ns a
pixel, in SuperHires, and 32 samples, 140 ns a pixel, in Hires, where the same
comparator is green. The position does not change with the width: `a1 − a0` is
50 in both runs. In each run the two cores print identical lines but for the
cycle at which the program's end marker arrives.

In the graded run `PC1` is green, 6138 transitions of the $CCCC pattern in runs
of exactly one sample, so SuperHires is in effect around the sprites; `CR4`
turns `PC1` red, as that contrast is built to. The sprites'
colours are p. 302's in every graded sample: `P1`–`P4` are green on both cores.

**Under the original chip set** the source has the playfield leave SuperHires
but not the sprite: bit 6 still reaches the sprite unit through the ungated
`shres` at line 391. No run of the package selects the original chip set, so
nothing measures or grades this; see *For the specialists*.

### What grades it

`PC2w` is a precondition of the package's own: it gates the pixel-exact grading
of the sprites' mixed-value lines and carries no claim id. The claims that state
the rule it grades are classified testable, observable at `denise_sprites`, and
are graded nowhere in the suite: `CH4-p101.c7`, `CH4-p101.c8`, `CH4-p101.c9`,
`CH4-size.c6`, `CH4-size.c7`, `CH4-size.c8`, `CH4-size.c9` (p. 101),
`CH4-hw.c10`, `CH4-p124.c6` (p. 124), `CH4-p128.c3`, `CH4-res.c3` (p. 128).
[`hrm/ch4-sprite-colour-registers`](../../hrm/ch4-sprite-colour-registers)
records them as outside its subject (`OUT-OF-SUBJECT-SIZE-RESOLUTION`,
`OUT-OF-SUBJECT-SHIFT-TIMING`) and no other package reads them. The second of
those records also lists `CH4-hw.c17`, whose own restatement is "There is one
SPRxPOS register for each of the eight sprites"; it is graded nowhere either.

---

## What the others say

The question put to each: with BPLCON3's SPRES at 0, how long is one sprite
pixel on an ECS Denise in SuperHires, and does BPLCON0 bit 6 change it without
an ECS Denise?

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:3081-3093`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3081-L3093), [`drawing.cpp:3075-3078`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3075-L3078), [`drawing.cpp:6470`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L6470), [`linetoscr_ecs_shres.cpp:46-71`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/linetoscr_ecs_shres.cpp#L46-L71), [`drawing.cpp:4772-4792`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L4772-L4792), [`include/custom.h:203-209`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L203-L209) | <pre>case 0:<br>default:<br>denise_sprres = denise_res == RES_SUPERHIRES ? RES_HIRES : RES_LORES;</pre> outside AGA the SPRES field is forced to that case, `if (!aga_mode) {` / `sr = 0;`, so SuperHires gives a **hires sprite resolution**. The shift size follows from it, `denise_spr_shiftsize = 1 << (RES_SUPERHIRES - denise_sprres);`, 2 for a hires sprite. An ECS SuperHires line is drawn by the renderer chosen under `} else if (ecs_denise && denise_res == RES_SUPERHIRES) {`, which calls `svt = denise_render_sprites_ecs_shres();` twice for every four SuperHires pixels, and each call does `sp->shift -= 2;` — a new sprite pixel every two SuperHires pixels, **70 ns**. Without an ECS Denise the bit is removed first, `con0 &= ~0x40; // SUPERHIRES`, and the sprite stays low resolution |
| **vAmiga** | [`Core/Components/Denise/DeniseRegs.cpp:818-854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L818-L854), [`Core/Components/Denise/Denise.cpp:923-931`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L923-L931), [`Core/Components/Denise/Denise.h:865-868`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L865-L868), [`Core/Components/Denise/Denise.cpp:1256-1258`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1256-L1258), [`Core/Components/Denise/Denise.h:1004`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L1004), [`Core/Components/Denise/DeniseRegs.cpp:733-736`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L733-L736) | <pre>return shres(spriteBplcon0) ? 2 : 4;</pre> `sprPixelWidth()` gives the width of a sprite pixel *"in buffer entries, i.e. in super-hires pixels"*: **2 in SuperHires, 70 ns**, 4 otherwise; SPRES is read only `if (isAGA()) {`, and its 0 case is commented `// Same as OCS/ECS`. The width picks the drawing routine, `case 2:  drawSprites<Resolution::HIRES>(); break;`, and each sprite pixel fills `constexpr Pixel width = sprPixelSize<R>();` entries, `return R == Resolution::SHRES ? 1 : R == Resolution::HIRES ? 2 : 4;`. `static bool shres(u16 v) { return GET_BIT(v, 6); }` has no chip-set term, while the bitplane resolution has one, `if (GET_BIT(v,6) && !isOCS()) {` — so on an OCS Denise the bit halves the sprite and not the playfield, as in NanoMig |
| **Denise** | [`emulation/libami/video/denise.cpp:489-490`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L489-L490), [`emulation/libami/video/denise.cpp:516-529`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L516-L529), [`emulation/libami/video/denise.cpp:318-320`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L318-L320) | <pre>constexpr int _pix = _shres ? 8 : (_hires ? 4 : 2);</pre> the loop runs `_pix` pixels per colour clock, `for(int p = 0; p < _pix; p++) {`, and the sprite shifters advance `if (_lores \|\| ((p & 1) == 0)) {`, `spr0.shift = (spr0.shift << 1) & ~(0x10000);`: two shifts per colour clock in lores and in hires, 140 ns, and **four in SuperHires, 70 ns**. Below ECS the bit never arrives, `if (model < Model::ECS)` / `value &= ~0x40;`, so an OCS sprite stays low resolution |
| **Copperline** | [`src/video/bitplane.rs:1009-1036`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L1009-L1036), [`src/video/bitplane.rs:1116-1118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L1116-L1118), test [`src/video/bitplane/tests.rs:2800-2852`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L2800-L2852), [`src/bus/custom_regs.rs:1080`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L1080) | <pre>0 =&gt; {<br>&nbsp;&nbsp;&nbsp;&nbsp;if self.shres() {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2<br>&nbsp;&nbsp;&nbsp;&nbsp;} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;4</pre> the width is counted *"in 35 ns (SuperHires) samples"*, so SPRES at 0 gives **70 ns in SuperHires** and 140 ns otherwise; of SPRES = 11 on a non-AGA chip set the code says *"ECS Super Denise accepts the SPRES encoding but retains its 70 ns sprite serializer"*. Its own test `bplcon3_spres_default_upgrades_to_70ns_when_shres_is_set` draws a sprite of `data: 0x8000,` over `bplcon0: BPLCON0_SHRES,` and asserts the lit pixel covers one 70 ns column, `assert_eq!(fb[1], rgb12_to_rgba8(0));`. `self.bplcon0 & BPLCON0_SHRES != 0` has no chip-set term and a write stores the whole word, `self.denise.bplcon0 = val;`, so on an OCS configuration the bit halves the sprite, as in NanoMig |
| **MiSTer** | [`rtl/denise.v:360-361`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L360-L361), [`rtl/denise.v:152`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L152), [`rtl/denise_sprites.v:107-114`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v#L107-L114) | descendant — identical to NanoMig: `assign spres_d = (spres == 2'b00) ? shres ? 2'b10 : 2'b01 : spres;`, the same ungated `assign shres    = bplcon0[6];`, and the same shift table, `2'b10   : shift = ~c1 ^ c3;` |
| **TC64** | [`rtl/minimig/denise.v:375-376`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise.v#L375-L376), [`rtl/minimig/denise.v:159`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise.v#L159), [`rtl/minimig/denise_sprites.v:107-114`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_sprites.v#L107-L114) | descendant — the same three statements, `assign spres_d = (spres == 2'b00) ? shres ? 2'b10 : 2'b01 : spres;`, `assign shres    = bplcon0[6];` and `2'b10   : shift = ~c1 ^ c3;`. Neither line keeps a SuperHires sprite at 140 ns, and neither gates the sprite's `shres` on ECS |

**All four give an ECS sprite a 70 ns pixel in SuperHires**, and none keeps
p. 101's low-resolution pixel there: WinUAE and vAmiga by a resolution code,
Denise by its shift cadence, Copperline by a width it asserts in a test. WinUAE,
vAmiga and Copperline model AGA's SPRES field too, and with it at 0 they use
the same rule, which vAmiga's comment calls *"Same as OCS/ECS"*. On the
original chip set they split two and
two: WinUAE and Denise drop BPLCON0 bit 6 without an ECS Denise and keep a
140 ns sprite; vAmiga and Copperline read the bit with no chip-set term and
halve the sprite, as NanoMig does.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 6f, 6g/6h, 6j1, 6k, 8c, 7j and 6l — **agrees with NanoMig**:

  *Read from the drawing, not quoted:* sprite 0's shift register advances under S0NSFT or S0HSFT, which sheet 6f forms as NOR(is_SPR0, S_HRES) and NOR(is_SPR0, S_HRES#). Under S0NSFT bit 15 takes bit 14, under S0HSFT it takes bit 13 (sheets 6g and 6h), on the same C7M1/C7M2 cycle: one bit per 140 ns outside SuperHires, two in it. Bits 15 and 14 leave as S0_AF and S0_AG (sheet 6j1). In SuperHires S0_AG to S3_AG take the channels of sprites 4 to 7, SA4 to SA7 and SB4 to SB7 (sheet 6k), and there the colour-index PLA on sheet 8c takes index bits CNA1 and CNA0 from channels 0 to 3 and CNA3 and CNA2 from channels 4 to 7, so both pixels reach the display. S_HRES is BPLCON0 bit 6, latched from D6in on sheet 7j and again on CAS1 on sheet 6l.
  <!-- drawing: denise-8373r4-dissection/sheet_030.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_031.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_034.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_036.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_059.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_049.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_037.txt -->

  Two sprite pixels per 140 ns cycle is a 70 ns sprite pixel in SuperHires and a 140 ns one in the other resolutions: NanoMig's width and the AA specification's `SHRES=70nS`, against p. 101's low-resolution pixel. The shift, output and channel paths are traced in the netlist for sprite 0; the PLA of sheet 8c is read from its drawn crosspoints. As drawn, sprites 4 to 7 have no channel of their own in SuperHires. The 8373R4 is an ECS Denise, so the dissection says nothing about the original chip set's configuration under *For the specialists*. It is a work in progress, and of one revision.

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989** — **agrees with the manual**, for the original chip set:

  > "The shift occurs once during each low resolution pixel time and continues until all 16 bits have been transferred to the sprite bus 349."
  <!-- corpus: patent-us4874164-agnus-denise/p028.txt -->

  The patent describes the chip set of its filing, which has no SuperHires mode; the sentence is p. 124's.

Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS), 1988, which gives SHRES a 35 ns pixel and SPRxCTL no SHSH bits and says nothing about the sprite serialiser's rate; Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988.

## The AA specification says what NanoMig does

**Commodore's AA functional specification** (06/07/91, PRELIMINARY) specifies
the AA chip set, and its BPLCON3 entry names what the ECS Denise does with
SPRES at 0, PDF page 17:

```
SPRESx= determine resolution of all 8 sprites (x=0,1):

SPRES1 | SPRES0 | SPRITE RESOLUTION
--------------------------------------------------------
   0   |   0    | ECS defaults(LORES,HIRES=140nS,SHRES=70nS
   0   |   1    | LORES(140nS)
   1   |   0    | HIRES(70nS)
```

The fourth row, `1 | 1 | SHRES(35nS)`, opens PDF page 18. In lores and hires
playfields a sprite pixel is 140 ns; in SuperHires it is 70 ns. Among the new
features of the AA chip set, PDF page 2 lists

```
- Sprite resolution can be set to LORES,HIRES,SHRES,independent
  of bitplane resolution.
```

— the independence p. 101 and p. 128 state for every sprite, listed as new in
AA. NanoMig's encoding is this table's: SPRES 0 becomes `2'b10`, `HIRES(70nS)`,
in SuperHires and `2'b01`, `LORES(140nS)`, otherwise.

It specifies a different chip set and does not decide what the ECS Denise does.
Beside the HRM it is the only Commodore document on this page that states a
SuperHires sprite's pixel, and it states NanoMig's.

---

## Why this is filed against the page

Chapter 4's rule and Commodore's later document disagree, and NanoMig takes the
later document's side. p. 101, p. 124 and p. 128 make a sprite pixel one
low-resolution pixel in every playfield mode; the AA specification gives the
ECS default in SuperHires as 70 ns and lists sprite resolution independent of
the bitplanes as a new AA feature. Between them the manual itself sends
SuperHires sprites to Appendix C (p. 131), and Appendix C gives them a
positioning step that depends on the playfield's resolution (pp. 297, 303) and
no width. NanoMig, both Minimig lines and all four independent implementations
show a 70 ns sprite pixel in SuperHires. Nothing read here, but chapter 4's
general rule and a patent of the original chip set, keeps it at 140 ns.

The finding key names the pair that disagrees, NanoMig and p. 101, and does not
prejudge which side is wrong.

---

## Suggested change

**Not an RTL change.** NanoMig already does what the AA specification states
for the ECS Denise and what every implementation does. The question is for
someone who knows the ECS Denise: whether p. 101's rule holds in SuperHires. If
it does not, `PC2w` grades a sentence the manual itself hands to an appendix
that does not repeat it.

---

## What holds the finding up

- **The width is measured, with its own control.** On both cores a 16-pixel
  sprite is lit over 16 samples in SuperHires and 32 in Hires, through the same
  comparator; the positions, 50 samples apart in both, calibrate the sample to
  70 ns, so the red is the pixel's length and not the sampling.
- **SuperHires is in effect where it is measured.** `PC1` finds runs of exactly
  one sample in the pattern regions above and below the sprites.
- **Both documents are quoted above**, and they disagree over a named quantity,
  the sprite pixel in SuperHires: p. 101's low-resolution pixel against the AA
  specification's `SHRES=70nS` for the ECS default.
- **Every implementation read here** gives 70 ns, WinUAE, vAmiga and Denise by
  their own resolution logic and Copperline in an asserted test.

## What weakens it

- **p. 101 is categorical and no page of the HRM contradicts it on width.**
  Appendix C is silent on the width, not opposed to it, and a silence can be
  read as the general rule carrying over.
- **The AA specification is PRELIMINARY and describes another chip set.** Its
  "ECS defaults" row is the only sentence on either side that is about the ECS
  Denise in SuperHires, and it stands in the description of an AA register.
- **The implementations may not be independent on this point.** Their code
  gives its own reasoning only in vAmiga's *"Same as OCS/ECS"* and Copperline's
  *"retains its 70 ns sprite serializer"*; none cites the hardware behind it.
  The silicon now speaks to it: the 8373R4 dissection shifts a sprite out two
  bits per 140 ns cycle in SuperHires, 70 ns a pixel (*What other Commodore
  documents say*). It is a third-party reading of one revision, and a work in
  progress.

---

## For the specialists

1. **How wide is a sprite on a real ECS Denise in SuperHires?** A one-line
   sprite of $8000, then $FFFF, over a one-plane SuperHires display on an 8373
   Denise, measured in 35 ns pixels: 2 per sprite pixel is the AA
   specification's `SHRES=70nS`, 4 is p. 101's low-resolution pixel.
2. **The original chip set, which this finding does not grade.** With an OCS
   configuration and BPLCON0 bit 6 set, NanoMig's source keeps the playfield low
   resolution and halves the sprite, because
   [`denise.v#L391`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L391)
   reads `shres` without the `ecs` term that
   [`#L362`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L362)
   applies to the bitplanes. There p. 101 has no counter-voice: the original
   chip set's BPLCON0 table marks bit 6 `X` (p. 271), p. 296 gives SuperHires to
   the enhanced Denise, and the AA table's row is the *ECS* default. WinUAE and
   Denise keep a 140 ns sprite there; vAmiga and Copperline halve it, as NanoMig
   does. If the OCS configuration should follow p. 101, `shres & ecs` in line
   391's condition is the one-term option; it has not been run here, and no
   assertion measures it.
3. **Eleven claims state p. 101's rule and none is graded.** They are listed
   under *What grades it*; `PC2w` measures their proposition without carrying
   them.
