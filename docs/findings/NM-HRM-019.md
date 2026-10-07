# NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should NanoMig's sprite shifter read SPRxCTL bit 4 (SHSH1) so that, in SuperHires, setting it alone moves the sprite's first lit sample two 35 ns samples later (p. 303's "70ns increment", half of SH0's four), where NanoMig moves it by 0? The decision belongs to an Amiga specialist, not to this repository. Closes when the RTL change is made and `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` is green, or when a specialist records bit 4 as unused on an ECS Denise, in which case this finding is withdrawn. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer; its `rtl/denise_sprites_shifter.v` has the same CTL write, keeping bits 7 and 0 only, so a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Scope** | ECS Denise (p. 303: "bits 3 and 4 are in the ECS chips only"), SuperHires only; isolated as SHSH1 alone against SH0 alone on sprite 0 in the `denise_sprites` fixture without AGA, sampled every 35 ns. Lores is recorded, not asserted; Hires is promised nothing. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C, printed p. 303, SPRxCTL bits 4 and 0, the ECS note and the SuperHires Warning (also p. 297, quoted in the assertion's message); quoted below: chapter 4 pp. 107 and 128, Appendix A pp. 264 and 284; test chapter 3 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims, all carried by `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE`: `C-library.c2`, `C-shr.c05`, `C-shres-sprite.c1`, `C-shres-sprite.c2`, `C-spr.c01`, `C-shres-sprite.c14`, `C-shres-sprite.c15`, `C-spr.c09`, `C-spr.c10`. Recorded beside it, not asserted: `APPC-pos.c10`, `APPC2-SPRxCTL.conf2`; withdrawn whole: `APPC-pos.c1`, `CH4-appC-position.c1`, `CH4-appC-position.c2`. The integration package's `P1` carries `APPC-pos.c2`, `CH4-appC-position.c18`, `CH4-appC-position.c4`. No other assertion is named on this page |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v); inherited (MiSTer and TC64 have the same absence) |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers): `make -C hrm/ch3-sprite-registers test`; [`integration/ch4-sprite-registers`](../../integration/ch4-sprite-registers): `make -k -C integration/ch4-sprite-registers test`; an expected red exits nonzero |
| **Assertions** | 1 red: `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` — expected SHSH1 alone to move the first lit sample 2 samples later (half of SH0 alone's 4), observed 0; the package reports `10 passed, 1 failed` |
| **Verdict of the four** | **all four implement it**, and on ECS all four confine its effect on position to SuperHires, as p. 303's Warning states. Hardware: the 8373R4 dissection latches the bit for sprites 0 to 3 and applies it in the sprite's data path, not in the position compare; not measured on a chip |
| **Effort** | S: `denise_sprites_shifter.v` and `denise_sprites.v` only: capture bit 4 in the CTL write and apply it to the start in SuperHires |
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

Three other pages give the bit no function, without naming a chip set. Chapter 4's sprite-control word, printed p. 107:

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

[`integration/ch4-sprite-registers`](../../integration/ch4-sprite-registers) measures the same relation at the video output. Its `P1` sweeps the start in ten 70 ns steps with SHRES set, SHSH1 set on the odd steps, and expects each step to move the sprite's edge by one 70 ns output pixel; it is red on both cores (`70 passed, 1 failed` on each). Its `P2`, which expects only 140 ns multiples in Lores, is not graded because it depends on `P1`.

## Risks and unknowns

- **Two Commodore-side sources disagree.** The 8373R4 dissection latches bit 4 for sprites 0 to 3 and, in SuperHires, moves the sprite one 70 ns pixel to the right: p. 303's row. Commodore's 1988 ECS Denise specification prints SPRxCTL bits 06-04 as not used, with no row for bit 3: NanoMig's register. Neither is the oracle: the red stands on p. 303.
- **The specification's table may be carried over.** It has the shape of an OCS-era SPRxCTL table, without even the EV8 row, as Appendix A prints it on p. 284.
- **The dissection is a reading of one revision, in progress.** The latch and the output selection are traced for sprite 0; sprites 1 to 3 are read from the overview sheet.
- **Conflicting passages.** pp. 107, 128 and 284 give the bit no function and name no chip set; the red rests on p. 303's scoping to ECS.
- **The assertion reads only where the sprite first lights**, so it cannot tell a moved start compare from a delayed output pixel; both pass it.
- **Outside SuperHires nothing is asserted**, since p. 303 gives bit 4 no position there and Hires nothing at all. `hrm/ch3-sprite-registers` records that bit 4 alone leaves the Lores trace, sampled once per 140 ns step, identical (`test_denise_sprites.cpp:1034-1047`).
- **Hardware is not measured.** A measurement on an 8373 Denise would settle what the chip does: a one-line sprite of `$8000` over a SuperHires display, SPR0CTL `$00` and then `$10`, measured in 35 ns pixels. Two pixels later is p. 303's row and the dissection's; no move is the specification's table and NanoMig.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2825-2836`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2825-L2836) | <pre>} else if (ecs_denise) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (s-&gt;ctl & 0x10) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sprxp \|= 2;</pre> bit 4 (`0x10`) from ECS Denise on; bit 3 (`0x08`) only on AGA. Outside SuperHires the match drops it ([`:5318`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5318), `sp->xpos & ~3`); in ECS hires it only clips the sprite's leftmost pixel ([`:5325-5329`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5325-L5329)) |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:171-173`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L171-L173) | <pre>(sprctl[x] & 0x10) &gt;&gt; 4);  // SHSH1 (ECS only)</pre> the lowest bit of the sprite's start; the start is then masked to the sprite pixel's width ([`:968`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L968), [`DeniseRegs.cpp:854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L854)), which on OCS and ECS keeps the bit only in SuperHires |
| **Denise** | [`emulation/libami/video/denise.cpp:299`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L299) | <pre>spr.shsh10 = (value & 0x10) &gt;&gt; 3;</pre> read always, compared only when `_shres` |
| **Copperline** | [`src/video/bitplane/sprite.rs:1857-1859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1857-L1859) | <pre>pub(super) fn sprite_hsub_70ns(ctl: u16) -> bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;ctl & 0x0010 != 0<br>}</pre> added to the start only while SHRES is set ([`:1723`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1723)) |
| **MiSTer** | — | descendant — `rtl/denise_sprites.v` — the same absence: no `sprctl[4]`, `SHSH1` or `shsh1` anywhere in the file |
| **TC64** | — | descendant — `rtl/minimig/denise_sprites.v` — likewise absent. A fix is new work on both lines; neither reads bit 4 for NanoMig to have dropped |

MiSTer and TC64 have never read the bit, which is the same absence NanoMig has, not a vote against it.

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — **agrees with NanoMig**:

  > "07 ATT Sprite attach control bit (odd Sprites) 06-04 X Not used."
  <!-- corpus: denise-spec-252126/p010.txt -->

  In Commodore's ECS Denise specification (it defines SHRES and names the 8373 revisions) the SPRxCTL table gives bits 06-04 as 'Not used' and has no row for bit 3; SHSH1 and 70 ns sprite positioning appear nowhere in its 14 pages. That sides with NanoMig not reading bit 4.

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 6d, 6j1, 6a, 6c and 6k and the second pass of section 6 — agrees with the manual. *Read from the drawing, not quoted:* sheet 6d latches D4in into S0C4 on SPR0CTL's write strobe, and the second-pass sheet draws the same latch for sprites 0 to 3. S0C4 is not an input of sprite 0's position match (sheets 6a and 6c); it steers the sprite's two outputs (sheet 6j1), which reach the display as two 70 ns pixels in SuperHires (sheet 6k; see [NM-HRM-071](NM-HRM-071.md)). Setting bit 4 therefore moves the sprite one 70 ns pixel to the right: p. 303's "70ns increment", made in the data path rather than in the start compare.
  <!-- drawing: denise-8373r4-dissection/sheet_028.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_039.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_034.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_025.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_027.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_036.txt -->

## Decision

Read bit 4 into the sprite's horizontal position as a 70 ns sub-position, and apply it only in SuperHires: Denise's one-liner plus its `_shres` guard is the smallest complete model; Copperline's named predicate is the tidiest. The 8373R4 makes the same move elsewhere: it leaves the start compare alone and, with the bit set, sends each 70 ns pixel out one pixel later, for sprites 0 to 3. Either form moves the first lit sample by 2 and turns the red green. The decision belongs to an Amiga specialist, and closes as the *What is asked of a reader* row says.
