# NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. Read *For the specialists* before deciding: the 8373R4 silicon stores the bit and applies it as p. 303 describes, and Commodore's 1988 ECS Denise specification prints it as not used. |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red: `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` (`10 passed, 1 failed`). In SuperHires, sampled every 35 ns, SH0 alone moves the sprite's first lit sample 4 samples later and SHSH1 alone 0, where p. 303 predicts 2; the oracle-flip mutation, which expects 0, is green on the same two numbers |
| **Verdict of the four** | **all four implement it**, and on ECS all four confine its effect on position to SuperHires, as the page's Warning states; outside the emulators, the 8373R4 dissection latches the bit for sprites 0 to 3 and applies it in the sprite's data path, not in the position compare — see *What other Commodore documents say* |
| **Origin** | the surviving half of the withdrawn [NM-HRM-013](withdrawn/NM-HRM-013.md) |

---

## What the HRM says

**Printed p. 303** gives bit 4 a name *and a behaviour*, which is what makes this
a requirement rather than a mention:

```
04      SHSH1     Start horizontal (SHR mode) 70ns increment
00      SH0       Start horiz. value Low bit 140 ns increment
```

scoped by the page's own note and Warning:

> "Note: bits 3 and 4 are in the ECS chips only."

> "70ns sprite positions are only available in SuperHires mode. Attempting to
> use 70ns sprite positioning with Hires mode under the current system may lead
> to unpredictable results."

So the row asks one thing: in SuperHires, bit 4 moves the sprite's start by 70
ns, half of SH0's 140 ns. Outside SuperHires the page gives it no position, and
in Hires it promises nothing at all.

**Compare bit 3 on the same page**, which the manual prints as `(unimplemented)`.

**Three other pages give the bit no function**, without naming a chip set:
chapter 4's sprite-control word on printed p. 107,

```
          Bits 6-3        Unused (make zero)
```

its summary on p. 128,

> "Bits 6-3 are reserved for future use (make zero)."

and Appendix A's SPRxCTL table on p. 284, on a row marked `D( E )`:

```
                 06-04    X        Not used
```

Appendix A itself sends the reader to Appendix C for what the ECS changed
(printed p. 264):

> "Registers denoted with an "(E)" in the chip column means that those
> registers have been changed the Enhanced Chip Set(ECS)."

> "See the register map in Appendix C for information on which ECS registers
> are new and which have been modified."

So for an ECS Denise the manual's word on bit 4 is p. 303's.

---

## What NanoMig does

Nothing with it. A write to SPRxCTL keeps bits 7 and 0 and no other, in both
builds of the shifter, with and without `DENISE_EBR`:

[`src/minimig-aga/denise_sprites_shifter.v#L133-L138`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L133-L138)

```verilog
// CTL register
always @(posedge clk)
  if (clk7_en) begin
    if (aen && address==CTL)
      {attach,hstart[0]} <= {data_in[7],data_in[0]};
  end
```

In SuperHires the start still moves only in 140 ns steps. `denise.v` hands the
sprite module `spres` = `2'b10` whenever BPLCON0's SHRES bit is set and
BPLCON3's SPRES field reads 0, which without AGA it always does
([`denise.v#L252`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L252)):

[`src/minimig-aga/denise.v#L390-L391`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L390-L391)

```verilog
wire [1:0] spres_d;
assign spres_d = (spres == 2'b00) ? shres ? 2'b10 : 2'b01 : spres;
```

At `2'b10` the sprite's pixels shift out at 70 ns, `2'b10   : shift = ~c1 ^ c3;`
([`denise_sprites.v#L161-L164`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L161-L164)),
but the start is compared once per 7 MHz cycle against the 9-bit `hstart`,
whose low bit is SH0
([`denise_sprites_shifter.v#L113-L116`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L113-L116)).
Nothing places the start on the 70 ns position between two SH0 steps; measured,
SHSH1 does not move it at all (*What the test measures*).

---

## What the test measures

`SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` in `hrm/ch3-sprite-registers` runs
the `denise_sprites` fixture without AGA and with `spres` = `2'b10`, the value
line 391 of `denise.v` supplies when SHRES is set. On a fresh DUT each time it
arms sprite 0 at SPR0POS `$40` with SPR0DATB `$0000` and SPR0DATA `$8000`,
writes SPR0CTL `$00`, `$01` (SH0 alone) or `$10` (SHSH1 alone), and samples
`sprdata` on every 28 MHz clock, 35 ns apart, keeping the first sample at which
it is non-zero (`test_denise_sprites.cpp:671-689`). The assertion reads p. 303's
two rows together, "70ns increment" and "140 ns increment": SHSH1 alone must
move that sample later by half as many samples as SH0 alone does
(`test_denise_sprites.cpp:1079-1126`):

```cpp
bool relation = sh0_delta != 0 && sh0_delta % 2 == 0
              && shsh1_delta == sh0_delta / 2;
```

It is red, one of the package's `10 passed, 1 failed`:

```
MEASURED: SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE: at spres=0b10 (SuperHires), 28 MHz (35 ns) sample deltas from the ctl=0x00 baseline -- SH0 alone (ctl=0x01): 4, SHSH1 alone (ctl=0x10): 0 (p.303 predicts 4 and 2).
```

SH0 moves the first lit sample by 4 samples, 140 ns, as its row says; SHSH1
leaves it where it was, where its row predicts 2.

**The red is the relation's, not the measurement's.** Built with
`MUT_BIT4_ORACLE`, which changes only the expected relation, to
`shsh1_delta == 0`, the package reads the same two numbers and is green,
`11 passed, 0 failed`
(the mutation record, `test_denise_sprites.cpp:385`). An
implementation that does what p. 303 describes moves the first lit sample by 2
and passes, whether it moves the start compare, as the four implementations
do, or delays the sprite's output by one 70 ns pixel, as the 8373R4 does
(*What other Commodore documents say*): the assertion reads only where the
sprite first lights.

Outside SuperHires the package asserts nothing about bit 4, since p. 303 gives
it no position there and Hires nothing at all. It records that bit 4 alone
leaves the Lores trace, sampled once per 140 ns step, identical
(`test_denise_sprites.cpp:1128-1148`).

[`integration/ch4-sprite-registers`](../../integration/ch4-sprite-registers)
specifies the same measurement at the video output: P1 sweeps the start in 70
ns steps with SHRES set and expects each step to move the sprite's edge by one
70 ns output pixel, and P2 expects only 140 ns multiples in Lores. It has no
test source yet.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2825-2836`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2825-L2836) | <pre>} else if (ecs_denise) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (s-&gt;ctl & 0x10) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sprxp \|= 2;</pre> bit 4 (`0x10`) from ECS Denise on; bit 3 (`0x08`) only on AGA. Outside SuperHires the match drops it ([`:5318`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5318), `sp->xpos & ~3`); in ECS hires it only clips the sprite's leftmost pixel ([`:5325-5329`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5325-L5329)) |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:171-173`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L171-L173) | <pre>(sprctl[x] & 0x10) &gt;&gt; 4);  // SHSH1 (ECS only)</pre> the lowest bit of the sprite's start; the start is then masked to the sprite pixel's width ([`:968`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L968), [`DeniseRegs.cpp:854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L854)), which on OCS and ECS keeps the bit only in SuperHires |
| **Denise** | [`emulation/libami/video/denise.cpp:299`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L299) | <pre>spr.shsh10 = (value & 0x10) &gt;&gt; 3;</pre> compared only when `_shres` |
| **Copperline** | [`src/video/bitplane/sprite.rs:1857-1859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1857-L1859) | <pre>pub(super) fn sprite_hsub_70ns(ctl: u16) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;ctl & 0x0010 != 0<br>}</pre> |
| **MiSTer** | — | descendant — `rtl/denise_sprites.v` — the same absence: no `sprctl[4]`, `SHSH1` or `shsh1` anywhere in the file |
| **TC64** | — | descendant — `rtl/minimig/denise_sprites.v` — likewise absent. A fix is new work on both lines; neither reads bit 4 for NanoMig to have dropped |

**All four independent implementations implement it**, and on ECS all four
confine its effect on position to SuperHires, as p. 303's Warning states: Denise reads it
always and compares it only in SuperHires; vAmiga and WinUAE mask it out of the
start position outside SuperHires; Copperline adds it only while SHRES is
set.
MiSTer and TC64 have never read the bit either, which is the same absence
NanoMig has, not a vote against it.

WinUAE's decode is the clearest statement of the ECS-versus-AGA split: `0x10` is
read from ECS Denise onwards, `0x08` only on AGA. That is the split the
withdrawn [NM-HRM-013](withdrawn/NM-HRM-013.md) was broken into.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — **agrees with NanoMig**:

  > "07 ATT Sprite attach control bit (odd Sprites) 06-04 X Not used."
  <!-- corpus: denise-spec-252126/p010.txt -->

  In Commodore's ECS Denise specification (it defines SHRES and names the 8373 revisions) the SPRxCTL table gives bits 06-04 as 'Not used' and has no row for bit 3; SHSH1 and 70 ns sprite positioning appear nowhere in its 14 pages. That sides with NanoMig not reading bit 4. Caveat: the table has the shape of an OCS-era SPRxCTL table, without even the EV8 row, so it may have been carried over rather than written for ECS.

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 6d, 6j1, 6a, 6c and 6k and the second pass of section 6 — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 6d latches D4in into S0C4 on SPR0CTL's write strobe, cSPR0CTL; the second-pass sheet draws the same latch for sprites 0 to 3, S0C4 to S3C4, and none for sprites 4 to 7. S0C4 is not an input of sprite 0's position match, the line isSPR0, which sheets 6a and 6c drive from H3 to H10 against SPR0POS and from H2 against SH0. It steers the sprite's two outputs instead (sheet 6j1): with S0C4 clear, S0_AF carries bit 15 of the shift register and S0_AG bit 14; with S0C4 set, S0_AG carries bit 15 and S0_AF carries bit 14 as it was one 140 ns cycle earlier. S0_AG reaches the display only in SuperHires, where it takes the channel of sprite 4 (sheet 6k).
  <!-- drawing: denise-8373r4-dissection/sheet_028.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_039.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_034.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_025.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_027.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_036.txt -->

  In SuperHires the shift register advances two bits per 140 ns cycle and S0_AF and S0_AG are the sprite's two 70 ns pixels (see [NM-HRM-071](NM-HRM-071.md)), so setting bit 4 moves the sprite one 70 ns pixel to the right: p. 303's "70ns increment", made in the data path rather than in the start compare. Outside SuperHires the register advances one bit per cycle, and bit 14 one cycle earlier is the current bit 15 except in the cycle a new word is loaded. So bit 4 is stored and live on the 8373R4, which NanoMig does not model. The latch, the output selection and the channel switch are traced in the netlist for sprite 0; sprites 1 to 3 are read from the overview sheet. The dissection is a work in progress, and of one revision.

Silent on this point: US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989; US patent 5,103,499, Commodore-Amiga, filed 1989 as a division of the 1986 application, granted 1992.

---

## Suggested change

Read bit 4 into the sprite's horizontal position as a 70 ns sub-position, and
apply it only in SuperHires. Denise's one-liner plus its `_shres` guard is the
smallest complete model; Copperline's named predicate is the tidiest.

```verilog
wire sprite_hsub_70ns = sprctl[4];   // SHSH1, appendix p.303 -- SuperHires only
```

with the position arithmetic taking it only when the sprite resolution is
SuperHires.

The 8373R4 makes the same move elsewhere: it leaves the start compare alone and,
with the bit set, sends each 70 ns pixel out one pixel later, for sprites 0 to
3 (*What other Commodore documents say*). Either form moves the first lit
sample by 2 and turns the red green.

---

## For the specialists

Two Commodore-side sources speak to the bit, and they disagree (*What other
Commodore documents say*). The 8373R4 dissection, a third-party reading of an
ECS Denise's silicon, latches SPRxCTL bit 4 for sprites 0 to 3 and, in
SuperHires, moves the sprite one 70 ns pixel to the right: p. 303's row.
Commodore's 1988 ECS Denise specification prints SPRxCTL bits 06-04 as not
used, with no row for bit 3: NanoMig's register. The dissection traces the
latch and the output selection transistor by transistor for sprite 0 and is
of one revision; the specification's SPRxCTL table is the one Appendix A prints
on p. 284 without its EV8 row, and the manual itself gives the ECS bits on
p. 303. Neither is the oracle: the red stands on p. 303.

A measurement on an 8373 Denise would settle what the chip does: a one-line
sprite of `$8000` over a SuperHires display, SPR0CTL `$00` and then `$10`,
measured in 35 ns pixels. Two pixels later is p. 303's row and the silicon's;
no move is the specification's table and NanoMig.

---

## Why this entry exists, and what it replaced

[NM-HRM-013](withdrawn/NM-HRM-013.md) asserted that `SPRxCTL` bits 3 **and** 4 are live fields, resting on
the note *"bits 3 and 4 are in the ECS chips only"* — a statement about **which
chips have them**, not about what they do. That is not a requirement, and the
finding was withdrawn.

The two bits then part company:

| bit | what the manual prints | outcome |
|---|---|---|
| 3 | `(unimplemented)` | the manual settles it in the RTL's favour |
| 4 | `SHSH1  Start horizontal (SHR mode) 70ns increment` | a behaviour — this finding |

The withdrawal was right and the split is what it produced.
