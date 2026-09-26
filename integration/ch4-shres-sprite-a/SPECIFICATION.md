# SuperHires sprite colour encoding: test specification

This document specifies one integration test package for the *Amiga Hardware
Reference Manual*, 3rd edition, Appendix C, printed page 302 ("SuperHires
sprites are encoded in the upper sixteen color registers"). It was written
from the package's claims (`claims-spec-stage.jsonl`, 249 claims, all on
printed page 302) and from the manual's pages alone, by an author who never
saw the implementation or any test. Page numbers are printed page numbers.
The table on page 302 reads the same in the page text and in the rendered
print, and every one of the 249 claims matches it.

The package runs one 68000 program, with one frame layout, on the whole
system. It has four propositions (P1 to P4). Section 1 lists the claims
that give no proposition. Section 2 lists what the pages leave open. Section 3
specifies the program, the observation and the preconditions that all four
propositions share. Section 4 gives the propositions.

---

## 1. Claims not turned into a proposition

### 1.1 Colour 16: nothing on the screen can show it (6 claims)

`APPC-sprkeykey.c16r`, `APPC-sprkeykey.c16g`, `APPC-sprkeykey.c16b`,
`CH4-appC-color.c2`, `CH4-appC-color.c3`, `CH4-appC-color.c4`
(p. 302, `Sprite (Color 16) :   AB-- CD-- EF--`).

No page says which sprite pixel shows colour 16, and chapter 4 says that no
sprite pixel shows it:

- p. 103: "The color value of the first register in each group of four
  registers is ignored by sprites. When the sprite bits select this
  register, the "transparent" value is used."
- p. 108: "Recall that binary 00 always means transparent and never refers
  to a color except background."
- Table 4-6 (p. 130): sprites 0 or 1, value `00`: "Not used \*", where the
  footnote reads "Selects transparent mode".

Page 302 lists colour 16 in the SuperHires key, but it does not say how a
sprite pixel could select that colour. A transparent sprite pixel shows the
playfield, and in SuperHires the playfield colours are in the lower sixteen
registers (p. 301). So the letters A to F cannot reach the output by any
mechanism a page states, and a test has nothing to look at. The program
still loads A to F as the table prescribes (section 3.5), because the table
is one scheme.

### 1.2 The 192 letter cells: stimulus only, no check of any single cell

- `APPC-sprtab.r10b00` to `APPC-sprtab.r17b11`: all 96 claims (registers
  `$10` to `$17`, bits 00 to 11).
- `CH4-appC-color.c18`–`c29` (`$10`), `c34`–`c45` (`$11`), `c50`–`c61`
  (`$12`), `c66`–`c77` (`$13`), `c82`–`c93` (`$14`), `c98`–`c109` (`$15`),
  `c114`–`c125` (`$16`), `c130`–`c141` (`$17`): 96 claims.

Each claim says which key letter one bit of COLOR16 to COLOR23 carries. None
of them can be graded on its own, for two reasons.

1. **The pages do not say which register the display reads for a pixel.**
   They also do not say which bits of that register reach which displayed
   pixel. Page 301 says only: "In order to decode sprite and bitplane color
   information in SuperHires mode, certain multiplexing occurs in the use
   of the registers." A cell has an effect on the output only if the
   display reads it, and no page says when the display reads it.
2. **The table itself makes the key's colours visible whether or not the
   scheme is decoded.** Row `$11` reads `G H A B I J C D K L E F`. So
   COLOR17, read as an ordinary `0x0RGB` register (p. 301; Table 4-6,
   p. 130), has `GH`, `IJ` and `KL` in the upper two bits of its red, green
   and blue nibbles. Those are exactly the two bits per component that the
   key encodes for colour 17. Rows `$12` and `$13` do the same for colours
   18 and 19. A display that ignores the SuperHires scheme and shows
   COLOR17 to COLOR19 as ordinary colours therefore shows the key's colours
   in every bit the key specifies. It differs from a decoding display only
   in bits 1 and 0, which the key leaves open (section 1.4). A stimulus that
   follows the table cannot make the two kinds of display differ in
   anything the pages predict.

These cells are used as stimulus: the program loads every cell exactly as
these claims state. A green result agrees with them but confirms none of
them one by one. Rows `$18` to `$1F` of the same table carry no claim in
this package, but they are loaded too.

### 1.3 The 32 dot cells: no expected value

`CH4-appC-color.c14`–`c17` (`$10`), `c30`–`c33` (`$11`), `c46`–`c49`
(`$12`), `c62`–`c65` (`$13`), `c78`–`c81` (`$14`), `c94`–`c97` (`$15`),
`c110`–`c113` (`$16`), `c126`–`c129` (`$17`).

The table marks bits 15 to 12 of each register with "." and gives them no
letter. No page states a value for these bits or an effect of writing them.
These claims give no expected value. The program writes 0 to these bits,
because the colour registers hold `0x0RGB` (p. 301), and no check reads them.

### 1.4 The "--" half of every key row: no expected value

Each key claim (`APPC-sprkeykey.*`, `CH4-appC-color.c2`–`c13`) also says
that the lower two bits of each four-bit component are not encoded. The
only other thing page 302 says about them is: "Keep in mind that because
of the loss of lower bits of precision in specifying SuperHires colors,
pastel colors in a closely graduated color scheme may be visually
difficult to distinguish from each other." No page states what bits 1 and
0 of a displayed component show. They have no expected value. The checks
mask them, record them in the log, and never grade them. For colours 17,
18 and 19, the letter half of these claims is propositions P2 to P4.

---

## 2. What the pages leave open, and the readings used

- **Colour numbering in SuperHires.** Page 302 names colours 16 to 19 but
  does not say again which sprite pixel selects each of them. Chapter 4
  says so, and gives no mode restriction: for sprites 0 and 1, values
  `01`, `10` and `11` select colour registers 17, 18 and 19 (Table 4-2,
  p. 108; Table 4-3, p. 115; Table 4-6, p. 130). This specification reads
  "Sprite (Color 17)" as the colour those tables give to value `01` of
  sprites 0 and 1, and likewise for 18 and 19. Of everything the checks
  rely on, this reading alone is not printed on page 302.
- **Sprites 2 to 7.** Table 4-3 gives them colours 21–23, 25–27 and 29–31.
  In SuperHires those registers hold encoded bits of colours 16 to 19. No
  page says what sprites 2 to 7 show in SuperHires, so they are kept idle.
- **Attached sprites.** No page covers attached sprites in SuperHires, so
  the package does not use them.
- **Pixel pairs.** No page states which two SuperHires pixel times share a
  register, or what appears where such a pair straddles a sprite edge.
  Pixels at run edges are not graded (reference frame R5).
- **Bits 1 and 0 of each component** are open (section 1.4). So are the dot
  bits (section 1.3).
- **Wording.** Page 301 says "There are only two bits of red, green and
  blue color resolution per hires pixel" in a section about SuperHires
  pixels. The two-letter fields of both keys agree with "two bits". This
  has no effect on the checks.
- **No conflict between pages.** Pages 103 and 108 ("ignored by sprites",
  "never refers to a color except background") sit uneasily with page 302
  listing a "Sprite (Color 16)", but the pages do not contradict each
  other: page 302 never says that a sprite shows colour 16. The only
  consequence is section 1.1.
- **SuperHires data fetch.** No page gives DDFSTRT or DDFSTOP values for
  SuperHires. The design does not depend on them (section 3.3).

---

## 3. The common test

### 3.1 Configuration

- Enhanced Chip Set, PAL, 2 MB of Chip memory (the defaults).
- The package runs once on each CPU core. All checks apply to each run.

### 3.2 Memory the program prepares

Everything is in Chip memory below `$80000` and word aligned (p. 104:
"Be sure all of your sprite data structures are word aligned in Chip
Memory").

- **Bitplane buffer:** 32768 words, each `$FF00`. Every fetched word is the
  same, so the displayed pattern does not depend on how many words the
  hardware fetches per line, or on the modulo.
- **Sprite 0 structure** (Table 4-1, p. 105; control words p. 107):
  `POS = $5050` (VSTART `$50`, HSTART `$A0`), `CTL = $E100` (VSTOP `$E1`;
  bits 7 to 0 all zero: no attach, SHSH1 and SHSH0 zero, V8 bits and H0
  zero). Then one pair of words per line for lines `$50` to `$E0` (145
  pairs, low word first; p. 107: "The bits in the high order color
  descriptor word contribute the leftmost digit"). Then `$0000,$0000`
  (p. 108: "the value 0 should be used for both words when ending the usage
  of a sprite").
- **Sprite 1 structure:** the same as sprite 0, except `POS = $5060`
  (HSTART `$C0`). Sprite 1 therefore starts 32 low-resolution pixels to the
  right of sprite 0, which leaves 16 pixels of background between the two.
- **Null structure:** `$0000,$0000`, used by sprites 2 to 7 (p. 114: "all
  eight sprite pointers must be initialized to either a real sprite or a
  safe null sprite").
- **Copper list:** see section 3.4.
- **Result area:** two words.

Sprite line layout, which is the same for sprites 0 and 1. Band *b* (0 to 4)
starts at line s_b = `$50` + 29·b.

| lines (relative to s_b) | low word | high word | pixel value | meaning |
|---|---|---|---|---|
| +0, +1, +2 | `$0000` | `$0000` | 00 | gap (transparent) |
| +3 … +10 | `$FFFF` | `$0000` | 01 | colour 17, 8 lines |
| +11 | `$0000` | `$0000` | 00 | gap |
| +12 … +19 | `$0000` | `$FFFF` | 10 | colour 18, 8 lines |
| +20 | `$0000` | `$0000` | 00 | gap |
| +21 … +28 | `$FFFF` | `$FFFF` | 11 | colour 19, 8 lines |

| band | s_b | colour 17 | colour 18 | colour 19 | register reload line |
|---|---|---|---|---|---|
| 0 | `$50` | `$53`–`$5A` | `$5C`–`$63` | `$65`–`$6C` | top of list |
| 1 | `$6D` | `$70`–`$77` | `$79`–`$80` | `$82`–`$89` | `$6E` |
| 2 | `$8A` | `$8D`–`$94` | `$96`–`$9D` | `$9F`–`$A6` | `$8B` |
| 3 | `$A7` | `$AA`–`$B1` | `$B3`–`$BA` | `$BC`–`$C3` | `$A8` |
| 4 | `$C4` | `$C7`–`$CE` | `$D0`–`$D7` | `$D9`–`$E0` | `$C5` |

### 3.3 Register writes by the CPU, in this order

Register addresses are from Appendix B.

1. `INTENA` ← `$7FFF` and `DMACON` ← `$7FFF`. This clears all interrupt and
   DMA enables (p. 278: bit 15 clear means "clear").
2. Prepare the memory of section 3.2.
3. Wait for vertical blank. Read the vertical position from `VPOSR` (`$004`,
   bit 0 = V8) and `VHPOSR` (`$006`, bits 15 to 8 = V7 to V0) (p. 285).
   Wait until it is at least `$100`, then until it is below `$020`.
4. `BPLCON0` ← `$1040` sets BPU = 001 (one bitplane), SHRES = 1 and
   HIRES = 0 (p. 300: HIRES "Set it to zero if SHRES enabled"), and leaves
   bit 0 = 0, so BPLCON3 stays off (p. 307).
   Then write the other display registers:
   - `BPLCON1` ← `$0000`.
   - `BPLCON2` ← `$0024`. This sets PF2P = PF1P = 100, which places all
     sprites in front of the playfield (Table 7-2, p. 210; p. 210: "PF2P2 -
     PF2P0, bits 5-3, are the priority bits for normal (non-dual)
     playfields").
   - `DIWSTRT` ← `$2C81` and `DIWSTOP` ← `$2CC1`, the PAL values (p. 59).
   - `DDFSTRT` ← `$003C` and `DDFSTOP` ← `$00D4`, the normal
     high-resolution values (p. 60).
   - `BPL1MOD` ← `$0000`.

   Do not write DIWHIGH (p. 306: "If it is not written, the old scheme for
   DIWSTRT and DIWSTOP described above holds"). Do not write BPLCON3,
   BEAMCON0 (p. 305: "Do not write to this register") or COPCON.
5. Write the pointers, each with a long write (p. 25, p. 110):
   - `BPL1PTH` ← the bitplane buffer.
   - `SPR0PTH` ← the sprite-0 structure, and `SPR1PTH` ← the sprite-1
     structure.
   - `SPR2PTH` to `SPR7PTH` ← the null structure.

   p. 110: "During the vertical blanking interval before the first display
   of the sprite, you need to write the sprite's memory address into these
   pointers."
6. `COP1LCH` ← the Copper list (long write), then write the strobe
   `COPJMP1`. p. 30: "you must initialize one of the Copper location
   registers (COP1LC or COP2LC) and write to its strobe address before
   Copper DMA is turned on."
7. `DMACON` ← `$83A0`: SET/CLR, DMAEN, BPLEN, COPEN and SPREN (p. 278).
   Steps 4 to 7 must finish while the vertical position is still below
   `$02C`.
8. Count four frames. Four times: wait for a vertical position of at least
   `$100`, then wait for one below `$020`.
9. Read `DENISEID` (`$07C`) and store the value at result+2. This value is
   recorded and not graded. p. 299: "The enhanced HighRes Denise (8373)
   will return $FC in the lower 8 bits".
10. Store the completion marker `$600D` at result+0, then halt.

### 3.4 Copper list

All MOVE destinations are at `$20` or above, so COPCON is not needed
(p. 21). A WAIT's second word is `$FFFE` (p. 24: "Bits 14-1 are normally
set to all 1s"; bit 15 is normally 1). Colour registers are written in
ascending address order: COLOR00 is `$180` and COLOR31 is `$1BE`.

```
; top: runs after every restart (p. 25, "About Copper restart")
MOVE  BPL1PTH, BPL1PTL          <- bitplane buffer
MOVE  SPR0PTH/L ... SPR7PTH/L   <- as in step 5 (p. 109 step 4, p. 127)
MOVE  COLOR00 ... COLOR15       <- REF      (section 3.5)
MOVE  COLOR16 ... COLOR31       <- band 0
WAIT  $4041,$FFFE               ; line $40, horizontal $40
MOVE  COLOR00 ... COLOR15       <- B0 ($0000 each)
WAIT  $6E41,$FFFE
MOVE  COLOR16 ... COLOR31       <- band 1
WAIT  $8B41,$FFFE
MOVE  COLOR16 ... COLOR31       <- band 2
WAIT  $A841,$FFFE
MOVE  COLOR16 ... COLOR31       <- band 3
WAIT  $C541,$FFFE
MOVE  COLOR00 ... COLOR15       <- B4 ($0AAA each)
MOVE  COLOR16 ... COLOR31       <- band 4 (same values as band 3)
WAIT  $FFFF,$FFFE               ; end of list (p. 23, p. 30)
```

Each reload WAITs for the second of the band's three gap lines, at a
horizontal position after horizontal blanking (p. 23: "Horizontal blanking
falls in the range of $0F to $35"). That is after the line's sprite fetch,
which happens "during a horizontal blanking interval" (p. 126).

### 3.5 Values

**Letters.** For each colour 16 to 19, each component (R, G, B) is a two-bit
value *v*. The first letter of the pair is bit 1 of *v*, and the second
letter is bit 0. For bands 0 to 3 the values are:

- colour 16: v = (b+1, b+2, b+3) mod 4
- colour 17: v = (b, b+1, b+2) mod 4
- colour 18: v = (b+3, b+4, b+5) mod 4
- colour 19: v = (b+6, b+7, b+8) mod 4

Band 4 uses the values of band 3. With these values, each two-bit field of
colours 17 to 19 takes all four values over bands 0 to 3. Within a colour,
the three components always differ from each other, so a swap of channels
is caught. In every component, colour 16 differs from colours 17, 18 and 19.

| band | A | B | C | D | E | F | G | H | I | J | K | L | M | N | O | P | Q | R | S | T | U | V | W | X |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 |
| 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 |
| 2 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 |
| 3, 4 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 |

**Upper registers (COLOR16 to COLOR31).** These values are computed cell by
cell from the table on page 302. Each "." bit is 0, and each letter bit is
the letter's value from the table above. The builder uses exactly these
values.

| band | `$10` | `$11` | `$12` | `$13` | `$14` | `$15` | `$16` | `$17` | `$18` | `$19` | `$1A` | `$1B` | `$1C` | `$1D` | `$1E` | `$1F` |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `$05AF` | `$016B` | `$0D27` | `$09E3` | `$049E` | `$005A` | `$0C16` | `$08D2` | `$078D` | `$0349` | `$0F05` | `$0BC1` | `$06BC` | `$0278` | `$0E34` | `$0AF0` |
| 1 | `$0AF0` | `$06BC` | `$0278` | `$0E34` | `$09E3` | `$05AF` | `$016B` | `$0D27` | `$08D2` | `$049E` | `$005A` | `$0C16` | `$0BC1` | `$078D` | `$0349` | `$0F05` |
| 2 | `$0F05` | `$0BC1` | `$078D` | `$0349` | `$0E34` | `$0AF0` | `$06BC` | `$0278` | `$0D27` | `$09E3` | `$05AF` | `$016B` | `$0C16` | `$08D2` | `$049E` | `$005A` |
| 3, 4 | `$005A` | `$0C16` | `$08D2` | `$049E` | `$0349` | `$0F05` | `$0BC1` | `$078D` | `$0278` | `$0E34` | `$0AF0` | `$06BC` | `$016B` | `$0D27` | `$09E3` | `$05AF` |

**Lower registers (COLOR00 to COLOR15).** These are computed cell by cell
from the bitplane table on page 301.

- **REF** is used on the pattern lines above the sprites. Bitplane colour 0
  has all letters 0, bitplane colour 1 has all letters 1, and colours 2
  and 3 have all letters 0 (they are unused with one bitplane):
  `$0000 $0CCC $0000 $0000 $0333 $0FFF $0333 $0333 $0000 $0CCC $0000 $0000 $0000 $0CCC $0000 $0000`.
- **B0** is `$0000` in all sixteen registers. All four bitplane colours are
  the same, so the playfield is uniformly U = (00,00,00), whichever
  register the display uses.
- **B4** is `$0AAA` in all sixteen registers, so the playfield is
  uniformly U = (10,10,10).

**Expected upper bits.** U = (R3R2, G3G2, B3B2) is defined in reference
frame R1.

| band | colour 17 | colour 18 | colour 19 | background U |
|---|---|---|---|---|
| 0 | (00,01,10) | (11,00,01) | (10,11,00) | (00,00,00) |
| 1 | (01,10,11) | (00,01,10) | (11,00,01) | (00,00,00) |
| 2 | (10,11,00) | (01,10,11) | (00,01,10) | (00,00,00) |
| 3 | (11,00,01) | (10,11,00) | (01,10,11) | (00,00,00) |
| 4 | (11,00,01) | (10,11,00) | (01,10,11) | (10,10,10) |

No expected sprite colour equals (00,00,00), (10,10,10) or (11,11,11).

### 3.6 Reference frames and how the test checks each anchor

- **R1: output bits.** The key prints each component as four positions
  under "R G B / ---- ---- ----", with the first letter leftmost, as in
  "GH--". So the first letter is bit 3 of the displayed four-bit component
  and the second letter is bit 2. For a visible pixel, U = (R bits 3–2,
  G bits 3–2, B bits 3–2).
  *Anchor check:* in the calibration run C1 (section 3.8), COLOR17 to
  COLOR19 act as ordinary colour registers, "stored as 0x0RGB" (p. 301;
  Table 4-6). The same U values as in section 3.5 must appear there. If
  they do not, the test's channel or bit order is not established, and no
  verdict is given.
- **R2: visible pixels and widths.** The harness shows one sample per two
  SuperHires pixel times, that is, one per high-resolution pixel. From
  p. 300 ("a 35ns pixel display rate - twice the horizontal resolution of
  Hires mode, and four times the Lores rates") and pp. 101 and 128 (sprite
  size and resolution are independent of the playfield), it follows that:
  - the `$FF00` pattern repeats every 16 SuperHires pixels, which is 8
    visible pixels;
  - a 16-pixel sprite is 64 SuperHires pixel times, which is 32 visible
    pixels.

  *Anchor check:* in the calibration run C1, the pattern repeats every 16
  visible pixels and each sprite run is 32 visible pixels.
- **R3: frames.** A Copper restart is a Copper fetch from the address
  written to COP1LC (p. 25). A *graded frame* is an output frame whose
  sprite lines (`$50` to `$E0`) are displayed after the second restart and
  before the CPU's write of `$600D` to result+0, both in simulation time.
  Those lines are more than 60 lines away from either frame boundary, so
  the latency of the line doubler does not matter.
- **R4: sprite lines on the bus.** For line *n*, the event is the fetch of
  the two data words that Table 4-1 assigns to line *n*. That is the
  (*n* − `$50`)-th pair after POS and CTL in that sprite's structure.
  p. 126: "The fetch and store for each horizontal scan line occurs during
  a horizontal blanking interval, far to the left of the start of the
  screen display."
- **R5: groups and runs on the output.** These definitions all apply to
  visible pixels only.
  - *Rising edge:* a pixel with U ≠ (00,00,00) right after a pixel with
    U = (00,00,00).
  - *Pattern line:* an output line with at least nine rising edges at
    equal spacing. Its *spacing* is the spacing of its longest such
    sequence. This definition does not presuppose the spacing that PC1
    checks.
  - *Background-like pixel:* a pixel with U = (00,00,00) or (10,10,10).
  - *Sprite region:* after the first pattern line, find the first block of
    at least four consecutive output lines in which every pixel is
    background-like. The sprite region runs from the first line of that
    block to the end of the frame. The block is source lines `$41` to
    `$52`. Line `$40` may show part of the pattern, because the Copper
    switches the lower registers during that line, so it is excluded.
  - *Group line:* a line of the sprite region with at least one pixel that
    is not background-like.
  - *Group:* a maximal block of consecutive group lines. Groups are counted
    from the top, starting at 0. Group 3b + j belongs to band b and colour
    17 + j.
  - *Run:* a maximal horizontal sequence of pixels that are not
    background-like.
  - *Interior:* a run without its first and its last pixel.

  The edge pixels are left out because no page says what is shown where a
  pixel pair straddles the sprite's edge (section 2). This is why a run of
  31 to 33 pixels is accepted for the 32 expected.
  *Anchor check:* PC2 establishes on the bus that the sprites carry
  exactly 15 non-transparent blocks of lines, separated by transparent
  lines, in the order of section 3.2. The colour check's requirement of
  exactly 15 groups verifies that the output shows the same partition. If
  the display drew anything on a gap line, or dropped a block, the count
  would differ.

### 3.7 The check common to P1–P4 (“the colour check”)

The check is applied to every graded frame, on each CPU core. It passes if
all of the following hold:

- The sprite region contains exactly 15 groups.
- Every line of every group has exactly two runs, the sprite-0 run on the
  left and the sprite-1 run on the right.
- Each run is 31 to 33 visible pixels long (R2, R5).
- Every interior pixel of every run has the U of its group's band and
  colour (section 3.5).

Bits 1 and 0 of every graded pixel are recorded in the log for each group
(the smallest and largest value per channel) and never graded
(section 1.4).

### 3.8 Preconditions, each with its contrast

The test asserts these before it grades anything. A contrast is a separate
run of a changed program, graded against the unchanged expectations. In
every contrast run the program still finishes, and every precondition other
than the one named still holds.

| id | precondition | how the test knows | contrast (the change) | what then happens |
|---|---|---|---|---|
| PC1 | SuperHires is in effect. | Every graded frame has at least one pattern line (R5) above the sprite region, and every such pattern line has a spacing of 8 visible pixels. p. 300: "35ns pixel display rate - twice the horizontal resolution of Hires mode". Sixteen pattern pixels per period at 35 ns make 8 visible pixels (R2). | **C1:** `BPLCON0` ← `$9000` (HIRES, one bitplane, SHRES = 0). | The spacing is 16 visible pixels, so PC1 goes red. By design, the colour check still passes in C1: this is the calibration of R1 and R2. It also shows why PC1 is indispensable: without SuperHires, COLOR17 to COLOR19 already show the key's bits (section 1.2). |
| PC2 | Sprites 0 and 1 fetched exactly the prepared data. | In each graded frame, each sprite fetches POS, CTL and 145 data pairs with the prepared values, one pair per line from `$50` to `$E0` (R4), then the two end words. | **C2:** SPR1PT points to the null structure, both in step 5 and in the Copper list. | Sprite 1 fetches no data, so PC2 goes red. |
| PC3 | Sprites 2 to 7 are idle. | In each graded frame, their DMA reads only the two words of the null structure. | **C3:** SPR2PT points to the sprite-0 structure. | Sprite 2 fetches data, so PC3 goes red. The output is unchanged, because sprite 0 hides sprite 2 (p. 115: "The lowest numbered sprite has the highest priority and appears in front of all other sprites"). |
| PC4 | Colour loads fall between displayed sprite pixels. | (a) Every MOVE fetch at the top of the list comes before the frame's first bitplane fetch. (b) The MOVE fetches after `WAIT $4041` come before sprite 0's pair fetch for line `$50`. (c) For b = 1 to 4, all MOVE fetches of band b's reload come after sprite 0's pair fetch for line s_b, and before its pair fetch for line s_b+2 (R4). Sprite 1 fetches in the same horizontal blanking interval (p. 126), so sprite 0's fetches are enough to mark the lines. | **C4:** band 2's WAIT becomes `$9141` (the fifth colour-17 line of band 2). | Part (c) fails for band 2, so PC4 goes red. |
| PC5 (P1 only) | Band 4's load of the lower registers took effect. | On every line of groups 12 to 14, every visible pixel outside the two runs, apart from the pixel next to each end of a run, has U = (10,10,10). On groups 0 to 11 it has U = (00,00,00). | **C5:** leave out the MOVEs to COLOR00–COLOR15 after `WAIT $C541`. | Band 4 keeps U = (00,00,00), so PC5 goes red. The colour check still passes. |
| PC6 | There are graded frames. | At least two graded frames (R3). | **C6:** step 10 comes right after step 7, with no frame count. | There is no graded frame, so PC6 goes red. |

The run-length requirement of section 3.7 is part of the check, not a
precondition. It stops a run of a few pixels from passing on an almost
empty interior. Its contrast is **C7**: the colour-17 lines of band 0
(`$53` to `$5A`) of both sprites get the low word `$FF00` and the high word
`$0000`. Those runs become 16 visible pixels long, so the colour check goes
red for group 0 only.

---

## 4. Propositions

### P1: SuperHires sprite colours are held in COLOR16 to COLOR31, not in the lower sixteen registers

1. **Proposition.** In SuperHires mode, reloading COLOR16 to COLOR31
   changes the displayed sprite colours as the scheme predicts, and
   reloading COLOR00 to COLOR15 does not change them.
   Claim: `CH4-appC-color.c1`, p. 302.
2. **Stimulus.** Sections 3.2 to 3.5. Bands 0 to 3 differ from each other
   only in COLOR16 to COLOR31. Band 4 differs from band 3 only in COLOR00
   to COLOR15 (B0 becomes B4).
3. **Observation.** The video output, on the group lines of groups 0 to 14
   of every graded frame (R3, R5), at the interior pixels of each run.
4. **Expected.** p. 302: "SuperHires sprites are encoded in the upper
   sixteen color registers using a similar scheme". Every interior pixel
   of groups 0 to 11 shows the U of its band (section 3.5), which changes
   from band to band. Every interior pixel of groups 12 to 14 (band 4)
   shows the same U as the corresponding group of band 3. "Upper sixteen"
   means all sixteen: all of them are loaded in every band.
5. **Preconditions.** PC1 to PC6, including PC5, which shows that the
   lower registers really changed in band 4.
6. **Contrast.** **C8:** after `WAIT $C541`, load COLOR16 to COLOR31 with
   the band-0 values of section 3.5 instead of the band-3 values. The page
   then predicts band-0 colours for groups 12 to 14. The check "band 4
   equals band 3" goes red for all three groups, and PC5 still holds.
7. **Reference frames.** R1, R3, R4 and R5, and the bus timing of PC4.

### P2: colour 17 is displayed with red GH, green IJ, blue KL

1. **Proposition.** In SuperHires mode, with COLOR16 to COLOR31 loaded as
   the table prescribes, every pixel of colour 17 (value `01` of sprite 0
   or sprite 1) is displayed with red bits 3–2 = G,H, green bits 3–2 = I,J
   and blue bits 3–2 = K,L.
   Claims: `APPC-sprkeykey.c17r`, `APPC-sprkeykey.c17g`,
   `APPC-sprkeykey.c17b`, `CH4-appC-color.c5`, `CH4-appC-color.c6`,
   `CH4-appC-color.c7` (the letter half), p. 302.
2. **Stimulus.** Sections 3.2 to 3.5. Over bands 0 to 3, (G,H), (I,J) and
   (K,L) each take all four values, and within each band the three differ
   from each other. The upper-register values are those of section 3.5,
   derived from the table on p. 302 cell by cell.
3. **Observation.** The video output, at the interior pixels of both runs
   on every line of groups 0, 3, 6, 9 and 12, in every graded frame, on
   each CPU core. Section 3.7 covers the group count, the run count and the
   run length for these groups.
4. **Expected.** p. 302: "Sprite (Color 17) :   GH-- IJ-- KL--". U equals
   the colour-17 column of section 3.5 for the group's band. The
   quantifier is every pixel, of both sprites, with all four values of
   every field. Bits 1–0 are not graded (section 1.4).
5. **Preconditions.** PC1 to PC4 and PC6.
6. **Contrast.** **C9:** in band 1, swap G and H (from G=0, H=1 to G=1,
   H=0). All registers that carry G or H change. Band 1's COLOR16 to
   COLOR31 become:
   `$0AF0 $0ABC $0278 $0E34 $0AE3 $0AAF $026B $0E27 $08D2 $089E $005A $0C16 $0BC1 $0B8D $0349 $0F05`.
   The page predicts U = (10,10,11) for group 3. The check expects
   (01,10,11), so it goes red for group 3 alone. Every other group stays
   green, and every precondition holds.
7. **Reference frames.** R1 (bit order: G is bit 3, H is bit 2), R2, R3,
   R4, R5.

### P3: colour 18 is displayed with red MN, green OP, blue QR

1. **Proposition.** Under the same conditions as P2, every pixel of colour
   18 (value `10` of sprite 0 or 1) is displayed with red bits 3–2 = M,N,
   green bits 3–2 = O,P and blue bits 3–2 = Q,R.
   Claims: `APPC-sprkeykey.c18r`, `APPC-sprkeykey.c18g`,
   `APPC-sprkeykey.c18b`, `CH4-appC-color.c8`, `CH4-appC-color.c9`,
   `CH4-appC-color.c10` (the letter half), p. 302.
2. **Stimulus.** The same as P2.
3. **Observation.** As in P2, on groups 1, 4, 7, 10 and 13.
4. **Expected.** p. 302: "Sprite (Color 18) :   MN-- OP-- QR--". U equals
   the colour-18 column of section 3.5, with the same quantifiers as P2.
5. **Preconditions.** PC1 to PC4 and PC6.
6. **Contrast.** **C10:** in band 2, swap O and P (from O=1, P=0 to O=0,
   P=1). Band 2's COLOR16 to COLOR31 become:
   `$0F05 $0BC1 $074D $0349 $0E34 $0AF0 $067C $0278 $0D17 $09D3 $055F $015B $0C16 $08D2 $045E $005A`.
   The page predicts U = (01,01,11) for group 7. The check goes red for
   group 7 alone.
7. **Reference frames.** R1 to R5.

### P4: colour 19 is displayed with red ST, green UV, blue WX

1. **Proposition.** Under the same conditions as P2, every pixel of colour
   19 (value `11` of sprite 0 or 1) is displayed with red bits 3–2 = S,T,
   green bits 3–2 = U,V and blue bits 3–2 = W,X.
   Claims: `APPC-sprkeykey.c19r`, `APPC-sprkeykey.c19g`,
   `APPC-sprkeykey.c19b`, `CH4-appC-color.c11`, `CH4-appC-color.c12`,
   `CH4-appC-color.c13` (the letter half), p. 302.
2. **Stimulus.** The same as P2.
3. **Observation.** As in P2, on groups 2, 5, 8, 11 and 14.
4. **Expected.** p. 302: "Sprite (Color 19) :   ST-- UV-- WX--". U equals
   the colour-19 column of section 3.5, with the same quantifiers as P2.
5. **Preconditions.** PC1 to PC4 and PC6.
6. **Contrast.** **C11:** in band 2, swap W and X (from W=1, X=0 to W=0,
   X=1). Band 2's COLOR16 to COLOR31 become:
   `$0F05 $0BC1 $078D $0345 $0E34 $0AF0 $06BC $0274 $0D27 $09E3 $05AF $0167 $0C15 $08D1 $049D $0055`.
   The page predicts U = (00,01,01) for group 8. The check goes red for
   group 8 alone.
7. **Reference frames.** R1 to R5.

### What a green result does and does not show

When all four propositions are green, the sprite colours in SuperHires
follow the key in bits 3–2, come from COLOR16 to COLOR31, and do not come
from COLOR00 to COLOR15. A green result does **not** show that the display
decodes the multiplexed registers. Section 1.2 explains why no test built
from these pages can show that.

---

## 5. Claim map

| claims | count | use |
|---|---|---|
| `CH4-appC-color.c1` | 1 | P1 |
| `APPC-sprkeykey.c17r/g/b`, `CH4-appC-color.c5`–`c7` | 6 | P2 (letter half); the "--" half has no expected value (section 1.4) |
| `APPC-sprkeykey.c18r/g/b`, `CH4-appC-color.c8`–`c10` | 6 | P3 (as above) |
| `APPC-sprkeykey.c19r/g/b`, `CH4-appC-color.c11`–`c13` | 6 | P4 (as above) |
| `APPC-sprkeykey.c16r/g/b`, `CH4-appC-color.c2`–`c4` | 6 | none: colour 16 is never displayed (section 1.1) |
| `APPC-sprtab.*`, `CH4-appC-color.c18`–`c29`, `c34`–`c45`, `c50`–`c61`, `c66`–`c77`, `c82`–`c93`, `c98`–`c109`, `c114`–`c125`, `c130`–`c141` | 192 | stimulus only; no single cell can be graded (section 1.2) |
| `CH4-appC-color.c14`–`c17`, `c30`–`c33`, `c46`–`c49`, `c62`–`c65`, `c78`–`c81`, `c94`–`c97`, `c110`–`c113`, `c126`–`c129` | 32 | none: dot bits, no expected value (section 1.3) |
| **total** | **249** | |
