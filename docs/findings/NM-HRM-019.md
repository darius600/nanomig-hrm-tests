# NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should NanoMig's sprite shifter read SPRxCTL bit 4 (SHSH1) so that, in SuperHires, setting it alone moves the sprite's first lit sample two 35 ns samples later (p. 303's "70ns increment", half of SH0's four), where NanoMig moves it by 0? Closes when the RTL change is made and `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` and, in `integration/ch4-sprite-registers`, `P1` are green (on both cores for `P1`), or when a specialist records bit 4 as unused on an ECS Denise, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | ECS Denise (p. 303: "bits 3 and 4 are in the ECS chips only"), SuperHires only; isolated as SHSH1 alone against SH0 alone on sprite 0 in the `denise_sprites` fixture without AGA, sampled every 35 ns; in `integration/ch4-sprite-registers` the same bit on sprite 2, read at the video output against sprite 0 as reference, with BPLCON0 $1240, ECS PAL, on both CPU cores. Lores is recorded, not asserted; Hires is promised nothing. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C, printed p. 303, SPRxCTL bits 4 and 0, the ECS note and the SuperHires Warning (also p. 297, quoted in the assertion's message); quoted below: chapter 4 pp. 107 and 128, Appendix A pp. 264 and 284 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (17)</summary> claims carried by `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` in `hrm/ch3-sprite-registers`: `sol:C-library.c2`, `opus:C-shr.c05`, p. 297; `sol:C-shres-sprite.c1`, `sol:C-shres-sprite.c2`, `opus:C-spr.c01`, `sol:C-shres-sprite.c14`, `sol:C-shres-sprite.c15`, `opus:C-spr.c09`, `opus:C-spr.c10`, p. 303; carried by `P1` in `integration/ch4-sprite-registers`: `APPC-pos.c2`, `CH4-appC-position.c18`, `CH4-appC-position.c4`, p. 303. Recorded beside it, not asserted: `APPC-pos.c10`, `APPC2-SPRxCTL.conf2`; withdrawn whole: `APPC-pos.c1`, `CH4-appC-position.c1`, `CH4-appC-position.c2`. No other assertion of `hrm/ch3-sprite-registers` is named on this page, and the other assertions of `integration/ch4-sprite-registers` it names (`P1-PRE1` to `P1-PRE3`, `P1-CONTROL`, `P2`, `P3`) carry no claim id </details> |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v); inherited (MiSTer and TC64 have the same absence) |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers): `make -C hrm/ch3-sprite-registers test`; an expected red exits nonzero. [`integration/ch4-sprite-registers`](../../integration/ch4-sprite-registers): `make -k -C integration/ch4-sprite-registers test` (both cores, `test-fx68k` and `test-tg68k`); an expected red exits nonzero |
| **Assertions** | 1 red in `hrm/ch3-sprite-registers`: `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` — expected SHSH1 alone to move the first lit sample 2 samples later (half of SH0 alone's 4), observed 0; the package reports `10 passed, 1 failed`. 1 red on each core in `integration/ch4-sprite-registers`: `P1` — expected the edge of sprite 2 to lie d = 80 + k output pixels from the reference sprite's for each of ten starts 70 ns apart, k = 0 to 9; observed, on the first line of each start, d = 80, 80, 82, 82, 84, 84, 86, 86, 88, 88, so every start with SHSH1 set reads the d of the start before it; the package prints `81 passed, 2 failed` on fx68k and on TG68K, and its other red is `P5-NOPOS`. `P1-CONTROL`, the five starts without SHSH1 through `P1`'s comparator, is green |
| **Verdict of the four** | **all four implement it**, and on ECS all four confine its effect on position to SuperHires, as the page's Warning states; outside the emulators, the 8373R4 dissection latches the bit for sprites 0 to 3 and applies it in the sprite's data path, not in the position compare — see *What other Commodore documents say*. Hardware: not measured |
| **Effort** | S — `denise_sprites_shifter.v` and `denise_sprites.v` only: capture bit 4 in the CTL write and apply it to the start in SuperHires |
| **Origin** | the surviving half of the withdrawn [NM-HRM-013](withdrawn/NM-HRM-013.md); bit 3 is settled in the RTL's favour by the manual's `(unimplemented)` on the same page |

---

## Source requirement

**Printed p. 303** gives bit 4 a name and a behaviour, which is what makes this a requirement rather than a mention:

```
04      SHSH1     Start horizontal (SHR mode) 70ns increment
00      SH0       Start horiz. value Low bit 140 ns increment
```

scoped by the page's own note and Warning:

> "Note: bits 3 and 4 are in the ECS chips only."

> "70ns sprite positions are only available in SuperHires mode. Attempting to
> use 70ns sprite positioning with Hires mode under the current system may lead
> to unpredictable results."

So the row asks one thing: in SuperHires, bit 4 moves the sprite's start by 70 ns, half of SH0's 140 ns. Outside SuperHires the page gives it no position, and in Hires it promises nothing at all.

Chapter 4's sprite-control word, printed p. 107:

```
          Bits 6-3        Unused (make zero)
```

its summary on p. 128:

> "Bits 6-3 are reserved for future use (make zero)."

and Appendix A's SPRxCTL table on p. 284, on a row marked `D( E )`:

```
                 06-04    X        Not used
```

Appendix A sends the reader to Appendix C for what the ECS changed (printed p. 264):

> "Registers denoted with an "(E)" in the chip column means that those
> registers have been changed the Enhanced Chip Set(ECS)."

> "See the register map in Appendix C for information on which ECS registers
> are new and which have been modified."

So for an ECS Denise the manual's word on bit 4 is p. 303's.

## RTL and measurement

A write to SPRxCTL keeps bits 7 and 0 and no other, in both builds of the shifter, with and without `DENISE_EBR`
([`denise_sprites_shifter.v#L133-L138`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L133-L138)):

```verilog
// CTL register
always @(posedge clk)
  if (clk7_en) begin
    if (aen && address==CTL)
      {attach,hstart[0]} <= {data_in[7],data_in[0]};
  end
```

In SuperHires the start still moves only in 140 ns steps. `denise.v` hands the sprite module `spres` = `2'b10` whenever BPLCON0's SHRES bit is set and BPLCON3's SPRES field reads 0, which without AGA it always does ([`denise.v#L252`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L252), [`#L390-L391`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L390-L391)). At `2'b10` the sprite's pixels shift out at 70 ns ([`denise_sprites.v#L161-L164`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L161-L164)), but the start is compared once per 7 MHz cycle against the 9-bit `hstart`, whose low bit is SH0 ([`denise_sprites_shifter.v#L113-L116`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L113-L116)). Nothing places the start on the 70 ns position between two SH0 steps.

`SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` in `hrm/ch3-sprite-registers` runs the `denise_sprites` fixture without AGA and with `spres` = `2'b10`. On a fresh DUT each time it arms sprite 0 at SPR0POS `$40` with SPR0DATB `$0000` and SPR0DATA `$8000`, writes SPR0CTL `$00`, `$01` (SH0 alone) or `$10` (SHSH1 alone), and samples `sprdata` on every 28 MHz clock, 35 ns apart, keeping the first sample at which it is non-zero (`test_denise_sprites.cpp:558-576`). The assertion reads p. 303's two rows together: SHSH1 alone must move that sample later by half as many samples as SH0 alone does (`test_denise_sprites.cpp:963-1025`).

```
MEASURED: SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE: at spres=0b10 (SuperHires), 28 MHz (35 ns) sample deltas from the ctl=0x00 baseline -- SH0 alone (ctl=0x01): 4, SHSH1 alone (ctl=0x10): 0 (p.303 predicts 4 and 2).
```

SH0 moves the first lit sample by 4 samples, 140 ns, as its row says; SHSH1 leaves it where it was, where its row predicts 2.

### At the video output, on both CPU cores

[`integration/ch4-sprite-registers`](../../integration/ch4-sprite-registers) makes the measurement through the whole system, from a
68000 program to the video output. On ECS, with BPLCON0 = `$1240` (SHRES), sprite 2 starts ten times, four lines each, at
P = 400 + k in 70 ns units, k = 0 to 9: P >> 1 goes into SH8–SH0 and P & 1 into SHSH1, so SHSH1 is set for every odd k. Sprite 0,
at a fixed position, is the reference. d is the distance from its edge to sprite 2's in output pixels, one per 70 ns, and `P1`
expects d = 80 + k on every line of start k (`test_ch4_sprite_registers.cpp:2480-2483`, with `p1_field` at `:2348-2352`). It is red on fx68k and on TG68K:

```text
MEASURED: SHR SHR-SWEEP ECS: refresh-group spacing 16 clk_sys cycles, 2 graded fields; field 0: ruler periods 8 8 8, SREF block=1 one-column=1 height=80, 10 groups (height:d of the first line) 4:80 4:80 4:82 4:82 4:84 4:84 4:86 4:86 4:88 4:88; field 1: ruler periods 8 8 8, SREF block=1 one-column=1 height=80, 10 groups (height:d of the first line) 4:80 4:80 4:82 4:82 4:84 4:84 4:86 4:86 4:88 4:88
FAIL: P1 (APPC-pos.c2 CH4-appC-position.c18 CH4-appC-position.c4): in SuperHires on ECS, SPRxCTL bit 4 moves the sprite start by 70 ns, two SuperHires pixels, and every 70 ns position in between is reachable (p.303) -- ten groups, d = 80 + k on every line of group k
```

Each step of SH0 moves the edge by 2 output pixels, 140 ns, as SH0's row says, and every odd k reads the d of the k before it:
SHSH1 moves nothing, as the fixture measurement above found. `P1`'s preconditions `P1-PRE1` to `P1-PRE3` are green (SuperHires
is on, the reference sprite shows one edge at one column on each of its 80 lines, the first start is four lines high), and so is
`P1-CONTROL`, which runs `P1`'s comparator on the even k alone, the starts SH8–SH0 reach without SHSH1: d = 80, 82, 84, 86 and 88
(`test_ch4_sprite_registers.cpp:2477-2479`, with `p2pre2_field` at `:2358-2362`). `P3`, bits 3 and 4 written on OCS, is green, as p. 303's note on bits 3 and 4
expects.

## Risks and unknowns

- **Two Commodore-side sources disagree** (*Independent evidence*): the 8373R4 dissection sides with p. 303's row, Commodore's 1988 ECS Denise specification with NanoMig's register.
- **The specification's table may be carried over.** It has the shape of the OCS-era SPRxCTL table Appendix A prints on p. 284, without even that table's EV8 row.
- **The dissection is a reading of one revision, in progress.** The latch and the output selection are traced for sprite 0; sprites 1 to 3 are read from the overview sheet.
- **Conflicting passages.** pp. 107, 128 and 284 give the bit no function and name no chip set; the red rests on p. 303's scoping to ECS.
- **The assertion reads only where the sprite first lights**, so it cannot tell a moved start compare from a delayed output pixel; both pass it.
- **Outside SuperHires nothing is asserted**, since p. 303 gives bit 4 no position there and Hires nothing at all. `hrm/ch3-sprite-registers` records that bit 4 alone leaves the Lores trace, sampled once per 140 ns step, identical (`test_denise_sprites.cpp:1034-1047`).
- **The video output drops every second SuperHires pixel.** `integration/ch4-sprite-registers`' `P1` grades whole 70 ns steps,
  d = 80 + k, and records where those steps lie on the 35 ns grid as not asserted (`APPC-pos.c3`, `CH4-appC-position.c3`). Its
  Lores sweep `P2` is recorded not graded while `P1` is red (`RECORDED: P2: NOT GRADED, depends on P1`):
  an even d in Lores says nothing while bit 4 acts nowhere.
- **Hardware is not measured.** A measurement on an 8373 Denise would settle what the chip does: a one-line sprite of `$8000` over a SuperHires display, SPR0CTL `$00` and then `$10`, measured in 35 ns pixels. Two pixels later is p. 303's row and the dissection's; no move is the specification's table and NanoMig.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2825-2836`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2825-L2836) | <pre>} else if (ecs_denise) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (s-&gt;ctl & 0x10) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sprxp \|= 2;</pre> bit 4 (`0x10`) from ECS Denise on; bit 3 (`0x08`) only on AGA. Outside SuperHires the match drops it ([`:5318`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5318), `sp->xpos & ~3`); in ECS hires it only clips the sprite's leftmost pixel ([`:5325-5329`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5325-L5329)) |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:171-173`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L171-L173) | <pre>(sprctl[x] & 0x10) &gt;&gt; 4);  // SHSH1 (ECS only)</pre> the lowest bit of the sprite's start; the start is then masked to the sprite pixel's width ([`:968`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L968), [`DeniseRegs.cpp:854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L854)), which on OCS and ECS keeps the bit only in SuperHires |
| **Denise** | [`emulation/libami/video/denise.cpp:299`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L299) | <pre>spr.shsh10 = (value & 0x10) &gt;&gt; 3;</pre> read always, compared only when `_shres` |
| **Copperline** | [`src/video/bitplane/sprite.rs:1857-1859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1857-L1859) | <pre>pub(super) fn sprite_hsub_70ns(ctl: u16) -> bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;ctl & 0x0010 != 0<br>}</pre> added to the start only while SHRES is set ([`:1723`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1723)) |
| **MiSTer** | — | descendant — `rtl/denise_sprites.v` — the same absence: no `sprctl[4]`, `SHSH1` or `shsh1` anywhere in the file; its [`rtl/denise_sprites_shifter.v:91-96`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites_shifter.v#L91-L96) has the same CTL write, keeping bits 7 and 0 only |
| **TC64** | — | descendant — `rtl/minimig/denise_sprites.v` — likewise absent. A fix is new work on both lines; neither reads bit 4 for NanoMig to have dropped |

### What other Commodore documents say

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — **agrees with NanoMig**:

  > "07 ATT Sprite attach control bit (odd Sprites) 06-04 X Not used."
  <!-- corpus: denise-spec-252126/p010.txt -->

  In Commodore's ECS Denise specification (it defines SHRES and names the 8373 revisions) the SPRxCTL table gives bits 06-04 as 'Not used' and has no row for bit 3; SHSH1 and 70 ns sprite positioning appear nowhere in its 14 pages. That sides with NanoMig not reading bit 4.

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 6d, 6j1, 6a, 6c and 6k and the second pass of section 6 — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 6d latches D4in into S0C4 on SPR0CTL's write strobe, cSPR0CTL; the second-pass sheet draws the same latch for sprites 0 to 3, S0C4 to S3C4, and none for sprites 4 to 7. S0C4 is not an input of sprite 0's position match, the line isSPR0, which sheets 6a and 6c drive from H3 to H10 against SPR0POS and from H2 against SH0. It steers the sprite's two outputs instead (sheet 6j1): with S0C4 clear, S0_AF carries bit 15 of the shift register and S0_AG bit 14; with S0C4 set, S0_AG carries bit 15 and S0_AF carries bit 14 as it was one 140 ns cycle earlier. S0_AG reaches the display only in SuperHires, where it takes the channel of sprite 4 (sheet 6k).
  <!-- drawing: denise-8373r4-dissection/sheet_028.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_039.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_034.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_025.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_027.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_036.txt -->

  In SuperHires the shift register advances two bits per 140 ns cycle and S0_AF and S0_AG are the sprite's two 70 ns pixels (see [NM-HRM-071](NM-HRM-071.md)), so setting bit 4 moves the sprite one 70 ns pixel to the right: p. 303's "70ns increment", made in the data path rather than in the start compare. Outside SuperHires the register advances one bit per cycle, and bit 14 one cycle earlier is the current bit 15 except in the cycle a new word is loaded. So bit 4 is stored and live on the 8373R4, which NanoMig does not model.

## Decision

Read bit 4 into the sprite's horizontal position as a 70 ns sub-position, and apply it only in SuperHires: Denise's one-liner plus its `_shres` guard is the smallest complete model; Copperline's named predicate is the tidiest. The 8373R4 makes the same move elsewhere: it leaves the start compare alone and, with the bit set, sends each 70 ns pixel out one pixel later, for sprites 0 to 3. Either form moves the first lit sample by 2 and turns the red green. By a reading of the change it also gives d = 80 + k in `integration/ch4-sprite-registers`, which turns `P1` green. It closes as the *What is asked of a reader* row says.
