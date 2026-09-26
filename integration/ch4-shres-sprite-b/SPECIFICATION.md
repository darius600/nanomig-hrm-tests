# SuperHires sprite colour encoding, colour registers $18-$1F: test specification

This specification was written from the package's claims (`claims-spec-stage.jsonl`, 224 claims) and from the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who has never seen the implementation or any test. It says what the 68000 test program writes and prepares, what the test observes, what the pages let it expect, and where the pages leave the outcome open. Page numbers are printed page numbers.

It contains four propositions (P1-P4) and five preconditions (PC0-PC4). All four propositions share one stimulus (section 3) and one reference frame (section 4).

---

## 1. Claims not turned into a proposition, and a limit that applies to all of them

### 1.1 No claim can be checked register by register

For every bit of every upper colour register, p. 302 gives the letter to write there. The key above the table defines each letter as one bit of one sprite colour. The page does not say which of the sixteen registers the chip reads to display a given sprite pixel. Each letter of colours 17-19 appears in seven of the sixteen registers. Two or six of its positions (in two or five registers) are in $18-$1F, and the rest are in $10-$17. The page predicts a displayed colour only when every position of a letter holds the same value. For any other register contents it predicts nothing.

A test can therefore change a letter only in all of its positions at once. It cannot make one register, or one bit of one register, decide an observation. Every letter claim of this package is checked jointly with the other positions of the same letter, per letter (P1-P3). **A green result does not show that any particular register among $18-$1F is read at all.**

The table is also built so that COLOR17, COLOR18 and COLOR19 (rows $11, $12, $13), read as ordinary colour words, hold colours 17, 18 and 19 in the two upper bits of every component. The ordinary format is given on p. 92 (Table 3-15): bits 11-8 red, 7-4 green, 3-0 blue. Row $11 holds G H | I J | K L at bits 11-10 | 7-6 | 3-2. Row $12 holds M N | O P | Q R there, and row $13 holds S T | U V | W X. The expected values of P1-P3 therefore also hold for a sprite shown through the ordinary colour selection of Table 4-6 (p. 130). No page gives an expected value that tells the SuperHires decoding apart from the ordinary one: the two would differ only in the two lower bits of each component, and the key leaves those as `--`. PC1 establishes that SuperHires mode is in effect. Nothing in this package can establish that the SuperHires colour decoding is the one being used.

### 1.2 Group 1: bits 15-12 of $18-$1F (32 claims, not used)

CH4-appC-color.c142, c143, c144, c145, c158, c159, c160, c161, c174, c175, c176, c177, c190, c191, c192, c193, c206, c207, c208, c209, c222, c223, c224, c225, c238, c239, c240, c241, c254, c255, c256, c257.

The table marks these bits with a dot and gives them no letter. The page does not say what these bits do. Chapter 3 lists bits 15-12 of every colour register as "Unused (set these to 0)" (p. 92, Table 3-15) and "Unused" (p. 46, Table 3-3), and says "The color registers are write-only" (p. 47; "All color registers are write-only", p. 92), so the bits cannot be read back. These claims give no expected value, and a test must not invent one. The stimulus writes these bits as 0, as p. 92 asks.

### 1.3 Group 2: colour-16 letters A-F in $18 and $1C (24 claims, only partly usable)

APPC-sprtab.r18b11, r18b10, r18b07, r18b06, r18b03, r18b02, r1Cb11, r1Cb10, r1Cb07, r1Cb06, r1Cb03, r1Cb02; CH4-appC-color.c146, c147, c150, c151, c154, c155, c210, c211, c214, c215, c218, c219.

Their positive content cannot be observed: a pixel of colour 16 would be displayed with red AB--, green CD--, blue EF--, but no sprite pixel is ever displayed in colour 16. Chapter 4 says:

> "The color value of the first register in each group of four registers is ignored by sprites. When the sprite bits select this register, the "transparent" value is used." (p. 103)

Sprite value 00 is "Transparent" (p. 108, Table 4-2) or "Not used * ... * Selects transparent mode" (p. 130, Table 4-6). The attached-sprite table begins at colour 17 (p. 131, Table 4-7). No page names a sprite pixel that is displayed in colour 16, yet p. 302 lists "Sprite (Color 16) : AB-- CD-- EF--" (see section 8).

Only one consequence can be checked: colours 17-19 take none of their displayed bits from these positions. That is P4.

### 1.4 Group 3: CH4-appC-color.c268 and c269 (2 claims, not used as stated)

Both quote row $1F correctly, but their `says` reads "carries sprite-color component bit undefined" for bit 1 and bit 0. The quoted row, the text layer and the rendered scan of p. 302 all give **W at bit 1 and X at bit 0**. As stated, the two claims give no expected value and contradict their own quote. The content of the quote is carried by APPC-sprtab.r1Fb01 (W) and APPC-sprtab.r1Fb00 (X), which are in P3.

---

## 2. The table as read

p. 302, under the heading "SuperHires Mode and the Denise Color Registers" (p. 301):

> "In order to decode sprite and bitplane color information in SuperHires mode, certain multiplexing occurs in the use of the registers." (p. 301)
>
> "SuperHires sprites are encoded in the upper sixteen color registers using a similar scheme:" (p. 302)

```
                            R    G    B
      Sprite (Color 16) :   AB-- CD-- EF--
      Sprite (Color 17) :   GH-- IJ-- KL--
      Sprite (Color 18) :   MN-- OP-- QR--
      Sprite (Color 19) :   ST-- UV-- WX--

      BIT  15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
      18   .   .   .   .   A   B   M   N   C   D   O   P   E   F   Q   R
      19   .   .   .   .   G   H   M   N   I   J   O   P   K   L   Q   R
      1A   .   .   .   .   M   N   M   N   O   P   O   P   Q   R   Q   R
      1B   .   .   .   .   S   T   M   N   U   V   O   P   W   X   Q   R
      1C   .   .   .   .   A   B   S   T   C   D   U   V   E   F   W   X
      1D   .   .   .   .   G   H   S   T   I   J   U   V   K   L   W   X
      1E   .   .   .   .   M   N   S   T   O   P   U   V   Q   R   W   X
      1F   .   .   .   .   S   T   S   T   U   V   U   V   W   X   W   X
```

Rows $18-$1F of the text layer agree with the printed page. The printed header sets "01 00" closer together than the other columns, but the row entries still fall one under each.

How the key is read: each colour has a 4-bit red, green and blue value. "MN--" means red bit 3 = M and red bit 2 = N, with red bits 1 and 0 given no letter. In each pair, the first letter is the upper bit. This matches the `says` of the APPC-sprtab claims ("the upper of the two encoded bits").

The row numbers are hexadecimal colour register numbers: $18-$1F are COLOR24-COLOR31 at $1B0-$1BE (Appendix B, p. 293). The rows follow a regular pattern, which is useful for checking register words. Row $10 + 4k + j holds colour 16+j in bits 11, 10, 7, 6, 3, 2 and colour 16+k in bits 9, 8, 5, 4, 1, 0. The page gives no meaning to this regularity, and nothing below depends on it.

---

## 3. Common stimulus

### 3.1 Configuration

- Enhanced Chip Set, PAL, 2 MB Chip memory. The display is not interlaced.
- The program uses only 68000 instructions. It runs on each of the two CPU cores, both in their default 68000 behaviour, and every check applies to each run separately. Nothing checked here depends on the CPU model, so the 68010 and 68020 settings are not needed.
- No other input (controller ports, mouse, keyboard, serial port, real-time clock) and no disk is used.

### 3.2 Chip memory contents

The addresses are suggestions. Any word-aligned, non-overlapping Chip-memory addresses will do: p. 104 says "Be sure all of your sprite data structures are word aligned in Chip Memory". The test identifies fetches by these addresses.

| Name | Address | Contents |
|---|---|---|
| COPLIST | $010000 | Copper list (3.5) |
| SPR0DAT | $011000 | sprite 0 data structure |
| SPR1DAT | $011200 | sprite 1 data structure |
| NULLSPR | $011400 | $0000, $0000 |
| MARKER | $011500 | written by the program at the end (3.4 step 8) |
| BPLBUF | $020000-$02FFFF | 32768 words, each $CCCC |

The sprite data structures follow p. 105 (Table 4-1), p. 107 and p. 108. Word 0 is SPRxPOS, word 1 is SPRxCTL. For each source line n = 80 ... 129 in order comes the colour descriptor **low** word and then the **high** word. Two words $0000, $0000 end the structure.

- Sprite 0: SPR0POS = **$5060**, SPR0CTL = **$8200**. VSTART = 80 = $50 goes in POS bits 15-8. HSTART = 192 puts bits 8-1 = $60 in POS bits 7-0 and bit 0 = 0 in CTL bit 0. VSTOP = 130 = $82 goes in CTL bits 15-8. CTL bit 7 (attach) = 0, bits 6-3 = 0 (p. 107 "Unused (make zero)"; on the ECS, bits 4 and 3 are SHSH1/SHSH0, p. 303, left 0), bit 2 VSTART high = 0 and bit 1 VSTOP high = 0.
- Sprite 1: SPR1POS = **$506C**, SPR1CTL = **$8201**. HSTART = 217 puts bits 8-1 = $6C in POS and bit 0 = 1 in CTL. Everything else is as for sprite 0.
- Both sprites carry the same 50 lines of data. Each source line n = 80 + 10k + j (band k = 0..4, line j = 0..9 within the band) has these words:

| j | Type | Pixel values, pixel 0 (left) to 15 | low word | high word |
|---|---|---|---|---|
| 0, 1 | L1 | 1111111111111111 | $FFFF | $0000 |
| 2, 3 | L2 | 2222222222222222 | $0000 | $FFFF |
| 4, 5 | L3 | 3333333333333333 | $FFFF | $FFFF |
| 6, 7 | LA | 1121322331030201 | $D9D1 | $2F94 |
| 8, 9 | gap | 0000000000000000 | $0000 | $0000 |

The pixel-to-word mapping follows chapter 4. Pixel i is bit 15-i of both words ("most significant (leftmost) bit first", p. 124). The low word supplies bit 0 of the pixel value and the high word supplies bit 1 ("The bits in the high order color descriptor word contribute the leftmost digit of the binary color selector number for each pixel; the low order word contributes the rightmost digit.", p. 107; worked example p. 108-109). LA contains every ordered pair of neighbouring values from {1, 2, 3}, and each of 1, 2, 3 next to 0 on both sides. It is coverage for whatever neighbour effects an implementation has. The page predicts no such effect, and the expected value of every pixel stays its own colour.

### 3.3 Colour sets

Each set assigns every colour 16-19 a 2-bit value per component (R, G, B), each 0-3. The upper bit of the value is the component's upper letter and the lower bit is its lower letter. For example, colour 18 with R = 2 (binary 10) gives M = 1, N = 0. A register word is built by placing, at each bit 11-0 of the row, the value of the letter p. 302 puts there. Bits 15-12 are 0.

The rotation sets S0-S3 give colour 16+i the values R = (i+k) mod 4, G = (i+k+1) mod 4, B = (i+k+2) mod 4 in set Sk. Within each set, the four colours differ in every component. Across the four sets, every component of every colour takes all four values, so every letter is observed at 0 and at 1, and "01" and "10" are both observed for every pair. No colour 17-19 has all three components 00 in any set.

| Set | colour 16 (R G B) | colour 17 | colour 18 | colour 19 |
|---|---|---|---|---|
| S0 | 00 01 10 | 01 10 11 | 10 11 00 | 11 00 01 |
| S1 | 01 10 11 | 10 11 00 | 11 00 01 | 00 01 10 |
| S2 | 10 11 00 | 11 00 01 | 00 01 10 | 01 10 11 |
| S3 | 11 00 01 | 00 01 10 | 01 10 11 | 10 11 00 |
| S0x | **11 10 01** | 01 10 11 | 10 11 00 | 11 00 01 |
| S0-c18 (contrast CR3 only) | 00 01 10 | 01 10 11 | **01 00 11** | 11 00 01 |

S0x is S0 with colour 16 complemented. S0-c18 is S0 with colour 18 complemented.

The register words for $10-$1F (COLOR16-COLOR31, written in ascending order) are:

| Set | $10 | $11 | $12 | $13 | $14 | $15 | $16 | $17 | $18 | $19 | $1A | $1B | $1C | $1D | $1E | $1F |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| S0 | $005A | $049E | $08D2 | $0C16 | $016B | $05AF | $09E3 | $0D27 | $0278 | $06BC | $0AF0 | $0E34 | $0349 | $078D | $0BC1 | $0F05 |
| S1 | $05AF | $09E3 | $0D27 | $016B | $06BC | $0AF0 | $0E34 | $0278 | $078D | $0BC1 | $0F05 | $0349 | $049E | $08D2 | $0C16 | $005A |
| S2 | $0AF0 | $0E34 | $0278 | $06BC | $0BC1 | $0F05 | $0349 | $078D | $08D2 | $0C16 | $005A | $049E | $09E3 | $0D27 | $016B | $05AF |
| S3 | $0F05 | $0349 | $078D | $0BC1 | $0C16 | $005A | $049E | $08D2 | $0D27 | $016B | $05AF | $09E3 | $0E34 | $0278 | $06BC | $0AF0 |
| S0x | $0FA5 | $07AD | $0BE1 | $0F25 | $0DA7 | $05AF | $09E3 | $0D27 | $0EB4 | $06BC | $0AF0 | $0E34 | $0F85 | $078D | $0BC1 | $0F05 |
| S0-c18 | $005A | $049E | $041E | $0C16 | $016B | $05AF | $052F | $0D27 | $014B | $058F | $050F | $0D07 | $0349 | $078D | $070D | $0F05 |

The lower sixteen registers (COLOR00-COLOR15) use one of two sets:

- **Pattern set.** It follows the p. 301 bitplane encoding with bitplane colour 0 = 00 00 00, colour 1 = 11 11 11, and colours 2 and 3 = 00 00 00: $00:$0000 $01:$0CCC $02:$0000 $03:$0000 $04:$0333 $05:$0FFF $06:$0333 $07:$0333 $08:$0000 $09:$0CCC $0A:$0000 $0B:$0000 $0C:$0000 $0D:$0CCC $0E:$0000 $0F:$0000.
- **Dark set.** All sixteen are $0000, which is the same encoding with all four bitplane colours 00 00 00.

### 3.4 Program, in order

1. INTENA <- $7FFF and DMACON <- $7FFF. SET/CLR = 0 clears every bit written as 1 (DMACON, p. 278).
2. Fill Chip memory as in 3.2 and write the Copper list of 3.5.
3. BPLCON0 <- **$1040**. This sets bit 6 SHRES = 1 ("SuperHires 35ns pixel enable bit", p. 300), bit 15 HIRES = 0 ("Set it to zero if SHRES enabled", p. 300), and BPU = 001, one bitplane ("Depth of SuperHires mode (1 or 2)", p. 300). HAM, DPF, BPLHWRM, SPRHWRM, LPEN, LACE and ENBPLCN3 (p. 307) are all 0.
4. BPLCON1 <- $0000 and BPLCON2 <- **$0024**. PF1P = 100 and PF2P = 100 place both playfields behind all four sprite groups (p. 210-211, Tables 7-1 and 7-2). Bits 15-7 are 0 ("Not used (keep at 0)").
5. BPL1MOD <- $0000, DIWSTRT <- $2C81, DIWSTOP <- $2CC1 ("The normal PAL DIWSTRT is ($2C81). The normal PAL DIWSTOP is ($2CC1).", p. 59), DDFSTRT <- $003C and DDFSTOP <- $00D4 ("The normal high resolution DDFSTRT is ($003C)", p. 60; DDFSTOP $00D4, p. 60 and p. 80). The ECS pages give no fetch values for SuperHires, so the high-resolution ones are used and PC1 checks the result. DIWHIGH and BPLCON3 are not written ("If it is not written, the old scheme for DIWSTRT and DIWSTOP described above holds", p. 306).
6. COP1LC <- COPLIST as a longword write, then a read of COPJMP1. p. 30: "you must initialize one of the Copper location registers (COP1LC or COP2LC) and write to its strobe address before Copper DMA is turned on", and the listing uses `MOVE.W COPJMP1(a1),d0`.
7. DMACON <- **$83A0**: SET/CLR, DMAEN, BPLEN, COPEN and SPREN (p. 278). The Copper is enabled after its location and strobe (p. 30). The sprite pointers are rewritten by the Copper at the start of every field, as p. 109-110 require ("For each subsequent display field, during the vertical blanking interval, rewrite the sprite pointers"). All eight pointers are set ("all eight sprite pointers must be initialized to either a real sprite or a safe null sprite", p. 114).
8. The program counts six frame starts by polling VPOSR/VHPOSR. A frame start is the vertical position (V8 from VPOSR bit 0, V7-V0 from VHPOSR bits 15-8) dropping below 16 after having been at or above 256. The program then writes $A5A5 to MARKER and loops on itself. After step 7 it writes no custom register.

### 3.5 Copper list

MOVE and WAIT use the formats of p. 21-22. WAIT first word = VP<<8 | HP | 1, second word $FFFE ("Bits 14-1 are normally set to all 1s", p. 24).

```
; section T: runs from the start of every frame (p. 30)
MOVE $0120,$0001  MOVE $0122,$1000        ; SPR0PTH/L <- SPR0DAT
MOVE $0124,$0001  MOVE $0126,$1200        ; SPR1PTH/L <- SPR1DAT
MOVE $0128..$013E <- NULLSPR (high $0001, low $1400) ; SPR2PTH/L .. SPR7PTH/L
MOVE $00E0,$0002  MOVE $00E2,$0000        ; BPL1PTH/L <- BPLBUF
MOVE $0180..$019E <- pattern set          ; COLOR00..COLOR15
MOVE $01A0..$01BE <- S0                   ; COLOR16..COLOR31
WAIT $3A07,$FFFE                          ; line 58
MOVE $0180..$019E <- dark set
WAIT $5807,$FFFE                          ; line 88
MOVE $01A0..$01BE <- S1
WAIT $6207,$FFFE                          ; line 98
MOVE $01A0..$01BE <- S2
WAIT $6C07,$FFFE                          ; line 108
MOVE $01A0..$01BE <- S3
WAIT $7607,$FFFE                          ; line 118
MOVE $01A0..$01BE <- S0x
WAIT $8407,$FFFE                          ; line 132
MOVE $0180..$019E <- pattern set
WAIT $FFFF,$FFFE                          ; never completes (p. 23, p. 30)
```

The list contains no MOVE to BPLCON0 ($0100).

### 3.6 Frame layout (source lines, PAL)

| Source lines | Content |
|---|---|
| 44-57 | pattern region A: bitplane data $CCCC, pattern set; PC1 checks lines 46-55 |
| 58 | Copper switches COLOR00-15 to the dark set (not graded) |
| 59-79 | dark |
| 80-129 | sprite band: band k (k = 0..4) is lines 80+10k to 80+10k+9; lines +0 to +7 carry L1, L1, L2, L2, L3, L3, LA, LA and lines +8, +9 are gap lines |
| 88, 98, 108, 118 | gap lines in which the Copper writes the next band's set |
| 130-131 | dark |
| 132 | Copper switches COLOR00-15 back to the pattern set (not graded) |
| 133-299 | pattern region B; PC1 checks lines 140-160 |

| Band k | Pattern lines | Set in COLOR16-31 | Written by |
|---|---|---|---|
| 0 | 80-87 | S0 | section T |
| 1 | 90-97 | S1 | group after WAIT line 88 |
| 2 | 100-107 | S2 | group after WAIT line 98 |
| 3 | 110-117 | S3 | group after WAIT line 108 |
| 4 | 120-127 | S0x | group after WAIT line 118 |

---

## 4. Reference frame

### 4.1 Samples and the masked colour

A *sample* is one output pixel. The output passes a line doubler, so every second SuperHires pixel is not visible, and consecutive samples of a line are one 70 ns SuperHires pixel pair apart. Each sample has 4-bit R, G and B values. The key gives only two bits per component, so every comparison in this package uses only bits 3 and 2 of each component: the masked value is m(s) = (R, G, B) & ($C, $C, $C), written as a hexadecimal RGB value. A sample is *dark* if m(s) = $000 and *lit* otherwise. Colour 0 in both lower-register sets is dark. Bitplane colour 1 in the pattern set and colours 17-19 in every set are lit.

### 4.2 Graded frame

The graded frame is the last complete output frame (vertical sync to vertical sync) that ends before the program's CPU write of $A5A5 to MARKER, which appears on the Chip-memory port. The chip-bus time and the video time are related through the harness's common system-clock time base.

### 4.3 Vertical anchor

v0 is the first output line of the graded frame that contains a run of at least 32 consecutive lit samples. Page anchor: VSTART = 80 is "where the topmost line of the sprite will be positioned" (p. 107), and source line 80 is L1 in both sprites (16 pixels of value 01). Nothing else in the frame has lit runs longer than one sample while SuperHires is in effect, because the pattern regions alternate. d is the number of output lines per source line that the line doubler produces, taken from the harness. Source line n corresponds to output lines v0 + d(n-80) to v0 + d(n-80) + d-1. PC2 checks the anchor and d line by line.

### 4.4 Horizontal anchors

On output line v0, a0 is the first sample of the first lit run, and a1 is the first lit sample after a0 + 31. Sprite pixel i (0 = leftmost = bit 15 of the data words) covers samples **a0 + 2i and a0 + 2i + 1** for sprite 0 and **a1 + 2i and a1 + 2i + 1** for sprite 1. The factor 2 comes from three statements:

- a sprite pixel is one low-resolution pixel: "The shift occurs once during each low resolution pixel time" (p. 124), "changing the resolution or interlace mode of the playfield has no effect on the size or resolution of a sprite" (p. 101), "The sprite resolution is independent of the bitplane resolution" (p. 128);
- a low-resolution pixel is four SuperHires pixels: "a 35ns pixel display rate - twice the horizontal resolution of Hires mode, and four times the Lores rates" (p. 300);
- the harness shows every second SuperHires pixel.

The anchors hold for every line of the graded frame, because both sprites keep one HSTART for all their lines. The distance a1 - a0 (50 samples by p. 107) belongs to the sprite-positioning claims and is not graded here. PC2 checks the anchors.

### 4.5 Chip-bus identification

Copper instruction fetches are fetches from COPLIST. The sprite data fetch for source line n is the pair of sprite-DMA fetches of the words at SPR0DAT + 4 + 4(n-80) and SPR0DAT + 6 + 4(n-80) (byte addresses; low word first) for sprite 0, and the same offsets from SPR1DAT for sprite 1 (p. 105, Table 4-1).

---

## 5. Preconditions and their contrasting cases

If a precondition fails, it is reported as a failed precondition. It is never reported as a verdict on a claim.

**PC0 (configuration).** The harness runs ECS, PAL and 2 MB Chip memory.
*Contrast CR7:* select OCS. The pages then predict a low-resolution display instead of SuperHires. SuperHires is an enhanced-Denise feature ("The enhanced Denise chip provides the new SuperHires mode", p. 296). The original chip set's BPLCON0 table marks bit 6 as X, and HIRES = 0 does not select 70 ns pixels (p. 271). The pattern regions therefore show runs of four samples and PC1 goes red. The program still finishes.

**PC1 (SuperHires in effect around the sprite band).** On every output line of source lines 46-55 and 140-160 of the graded frame, samples a0 to a0 + 99 alternate lit and dark with runs of exactly one sample, in either phase. Page: "SuperHires mode provides a 35ns pixel display rate" (p. 300). The data $CCCC is 1100 repeated: two SuperHires pixels on and two off, which is one sample lit and one dark. The same holds whichever pixel of each pair the harness shows. In addition, the Copper fetches of the graded frame contain no MOVE to $0100, and the program writes BPLCON0 only in step 3. SuperHires mode therefore holds from region A through the sprite band to region B. This is the "while SuperHires" condition of the claims ("On ECS SuperHires").
*Contrast CR4:* BPLCON0 <- $9000 (HIRES = 1, SHRES = 0, one bitplane). "HIRES=High-resolution (70 ns pixels)" (p. 271), so runs of two samples appear and PC1 goes red. The sprites still display. P1-P3 are expected to pass in this run as well (section 1.1). That is why PC1 is needed.

**PC2 (sprite frame).** All five parts must hold in the graded frame.
(a) Chip bus: sprite 0 and sprite 1 fetch their control words and all 50 data word pairs in address order, with the data of 3.2.
(b) Output: on every output line of source lines 80-129, the samples of both sprites show lit and dark exactly as the pixel values of that line predict. Value 00 is dark, because p. 103 says it "becomes transparent and shows the color of any other sprite or playfield that has lower video priority", and the playfield here is the dark set. Values 01, 10 and 11 are lit. So L1, L2 and L3 lines are lit over all 32 samples, LA lines are dark exactly at pixels 10, 12 and 14, and gap lines are dark over all 32 samples.
(c) Samples a0-4 to a0-1, a0+32 to a1-1, and a1+32 to a1+35 are dark on those lines, and a1 - a0 >= 33.
(d) On the output lines of source lines 70-79 and 130-131, samples a0-4 to a1+35 are dark.
(e) The number of graded samples equals the counts given in P1-P4.
*Contrast CR5:* DMACON <- $8380 (SPREN not set). No sprite DMA takes place (p. 278; p. 114 "When you enable sprite DMA for one sprite, you enable DMA for all the sprites"), so (a) and (b) go red and no v0 is found. The Copper, bitplane DMA and program run as before.

**PC3 (register contents in place, and as specified).** In the graded frame, the Copper fetches every instruction word of the list in order, and the MOVE data words equal those of 3.3 and 3.5. For band k = 1..4, all 32 fetches of the band's colour group come after the last sprite-0 and sprite-1 data fetch of source line 80 + 10(k-1) + 7 and before the first data fetch of source line 80 + 10k. The two colour groups of section T and the dark-set group after the line-58 WAIT all come before the first sprite data fetch of line 80.
*Contrast CR6:* the band-1 WAIT becomes $5C07 (line 92) instead of $5807. Its colour group is then fetched after the data fetch of line 90, and PC3 goes red. Everything else runs.

**PC4 (run length).** MARKER receives $A5A5, the graded frame is complete, and at least three complete Copper passes precede it. A complete pass is a fetch run from the first word of the list to its last WAIT.
*Contrast CR8:* the program writes MARKER after one frame start instead of six. Fewer than three complete passes then precede the graded frame, and PC4 goes red. The program still finishes.

---

## 6. Propositions

Common to P1-P4:

- **Stimulus:** section 3.
- **Frame:** section 4.
- **Preconditions:** PC0-PC4.
- **Observation interval:** the graded frame, the output lines of the sprite band's pattern lines, and the two samples of every pixel whose value is not 00. The sprites are assigned their colours by p. 130, Table 4-6: "0 or 1 ... 01 ... 17, 10 ... 18, 11 ... 19" (also p. 108, Table 4-2, and p. 115, Table 4-3, "0 and 1 ... 17 - 19"). Only sprites 0 and 1 are used, because they are the only sprites whose colours the pages tie to colours 17-19.
- **Components:** only bits 3 and 2 of each component are graded. Bits 1 and 0 have no expected value. The key gives them no letter ("--"), p. 301 says "There are only two bits of red, green and blue color resolution", and p. 302 speaks of "the loss of lower bits of precision in specifying SuperHires colors". Bits 1 and 0 may be recorded, but they never carry a verdict.
- **Quantifier:** the key speaks of *the* colour of a sprite pixel of colour c, so every graded sample of colour c must match. One mismatching sample fails the letter concerned. The test also asserts the exact number of graded samples, so that no sample is skipped.

The expected masked values per band, with p. 302 as the source for every row, are:

| Band (set) | colour 17 | colour 18 | colour 19 |
|---|---|---|---|
| 0 (S0) | $48C | $8C0 | $C04 |
| 1 (S1) | $8C0 | $C04 | $048 |
| 2 (S2) | $C04 | $048 | $48C |
| 3 (S3) | $048 | $48C | $8C0 |
| 4 (S0x) | $48C | $8C0 | $C04 |

### P1: colour 17 (letters G H I J K L)

1. **Proposition.** In ECS SuperHires mode, with the sixteen upper colour registers written in the encoding of p. 302, every pixel of value 01 that sprite 0 or sprite 1 displays has red bits 3 and 2 = G and H, green bits 3 and 2 = I and J, and blue bits 3 and 2 = K and L. The table places these letters in this package's range at $19 and $1D, bits 11, 10, 7, 6, 3, 2. Page 302. Claims: the 24 ids in rows G-L of Appendix A.
2. **Stimulus.** Section 3. Rows $10-$17, which are outside this package, are written as the same page prescribes, because a letter has a defined value only when all its positions agree (1.1).
3. **Observation.** Bands 0-3 (source lines 80-87, 90-97, 100-107, 110-117). In those bands the value-01 pixels are pixels 0-15 of the L1 lines and pixels 0, 1, 3, 9 and 15 of the LA lines, in both sprites. That is 84 samples per sprite per band per output line copy, and 672 x d samples in total.
4. **Expected value.** m(s) is the colour-17 value of the band's set (table above). Per letter: bit 3 of R = G, bit 2 of R = H, bit 3 of G = I, bit 2 of G = J, bit 3 of B = K, bit 2 of B = L. Each letter is 1 in two of the four bands and 0 in the other two. Source: "Sprite (Color 17) : GH-- IJ-- KL--" together with rows $19 and $1D (and rows $11, $14-$17) of p. 302. The verdict is given per letter: a letter is green only if it matches in every graded sample of every band, and its verdict applies to all of that letter's claim ids jointly.
5. **Preconditions.** PC0-PC4.
6. **Contrasts.** *CR1:* COLOR16-31 hold S2 instead of S0 in band 0 (section T writes S2). The page then predicts colour 17 = $C04 in band 0. Graded against S0 ($48C), bit 3 fails for every component, so G, I and K go red. *CR2:* S1 instead of S0 in band 0. The page predicts $8C0, and bit 2 fails for every component, so H, J and L go red. In both runs PC0-PC4 still hold, with PC3 comparing against the words actually written in that run.
7. **Reference frame.** Section 4.

### P2: colour 18 (letters M N O P Q R)

1. **Proposition.** In ECS SuperHires mode, with the encoding of p. 302, every pixel of value 10 that sprite 0 or sprite 1 displays has red bits 3 and 2 = M and N, green bits 3 and 2 = O and P, and blue bits 3 and 2 = Q and R. The table places these letters in this package's range at $18 bits 9, 8, 5, 4, 1, 0; $19 bits 9, 8, 5, 4, 1, 0; $1A all twelve letter bits; $1B bits 9, 8, 5, 4, 1, 0; and $1E bits 11, 10, 7, 6, 3, 2. Page 302. Claims: the 72 ids in rows M-R of Appendix A.
2. **Stimulus.** Section 3.
3. **Observation.** Bands 0-3. The value-10 pixels are pixels 0-15 of the L2 lines and pixels 2, 5, 6 and 13 of the LA lines, in both sprites. That is 80 samples per sprite per band per copy, and 640 x d in total.
4. **Expected value.** m(s) is the colour-18 value of the band's set. Per letter: M, N are R bits 3, 2; O, P are G bits 3, 2; Q, R are B bits 3, 2. Source: "Sprite (Color 18) : MN-- OP-- QR--" and rows $18-$1B and $1E (and $12, $16) of p. 302. The verdict is given per letter.
5. **Preconditions.** PC0-PC4.
6. **Contrasts.** CR1 predicts colour 18 = $048 against S0's $8C0 in band 0, so bit 3 fails for every component (M, O, Q). CR2 predicts $C04, so bit 2 fails for every component (N, P, R).
7. **Reference frame.** Section 4.

### P3: colour 19 (letters S T U V W X)

1. **Proposition.** In ECS SuperHires mode, with the encoding of p. 302, every pixel of value 11 that sprite 0 or sprite 1 displays has red bits 3 and 2 = S and T, green bits 3 and 2 = U and V, and blue bits 3 and 2 = W and X. The table places these letters in this package's range at $1B bits 11, 10, 7, 6, 3, 2; $1C bits 9, 8, 5, 4, 1, 0; $1D bits 9, 8, 5, 4, 1, 0; $1E bits 9, 8, 5, 4, 1, 0; and $1F all twelve letter bits. Page 302. Claims: the 70 ids in rows S-X of Appendix A. CH4-appC-color.c268 and c269 are excluded (1.4); APPC-sprtab.r1Fb01 and r1Fb00 carry the same positions.
2. **Stimulus.** Section 3.
3. **Observation.** Bands 0-3. The value-11 pixels are pixels 0-15 of the L3 lines and pixels 4, 7, 8 and 11 of the LA lines, in both sprites. That is 80 samples per sprite per band per copy, and 640 x d in total.
4. **Expected value.** m(s) is the colour-19 value of the band's set. Per letter: S, T are R bits 3, 2; U, V are G bits 3, 2; W, X are B bits 3, 2. Source: "Sprite (Color 19) : ST-- UV-- WX--" and rows $1B-$1F (and $13, $17) of p. 302. The verdict is given per letter.
5. **Preconditions.** PC0-PC4.
6. **Contrasts.** CR1 predicts $48C against S0's $C04 in band 0, so bit 3 fails for every component (S, U, W). CR2 predicts $048, so bit 2 fails for every component (T, V, X).
7. **Reference frame.** Section 4.

### P4: colour 16's letters (A-F) do not show in colours 17-19

1. **Proposition.** In ECS SuperHires mode, when colour 16 is changed in every position the table gives its letters, including $18 and $1C bits 11, 10, 7, 6, 3, 2, and colours 17-19 are unchanged, the displayed bits 3 and 2 of every pixel of colours 17-19 are unchanged. Page 302. Claims: the 24 ids of section 1.3 (rows A-F of Appendix A). Only this consequence of those claims can be checked (1.3).
2. **Stimulus.** Band 4 holds S0x, which is S0 with all six letters A-F inverted in all their positions (rows $10-$14, $18, $1C) and G-X unchanged.
3. **Observation.** Band 4 (source lines 120-127): every sample of every pixel of value 01, 10 or 11 in both sprites. That is 244 samples per sprite per copy, and 488 x d in total.
4. **Expected value.** m(s) equals S0's value for the pixel's colour: $48C, $8C0 and $C04 for colours 17, 18 and 19. Source: the key defines the displayed colours 17-19 through G-X alone ("Sprite (Color 17) : GH-- IJ-- KL--", "Sprite (Color 18) : MN-- OP-- QR--", "Sprite (Color 19) : ST-- UV-- WX--"), while A-F belong to "Sprite (Color 16) : AB-- CD-- EF--". There is one joint verdict for all 24 ids.
5. **Preconditions.** PC0-PC4. PC3 includes the check that band 4's colour group carries the S0x words.
6. **Contrast.** *CR3:* band 4 holds S0-c18 instead of S0x, that is, colour 18 complemented instead of colour 16. The page predicts colour 18 = $40C in band 4, which differs from $8C0 in every graded bit, so P4 goes red on the colour-18 samples. Colours 17 and 19 stay at $48C and $C04.
7. **Reference frame.** Section 4.

---

## 7. What the pages leave open

- **Which register is read for which pixel.** The pages do not say (1.1). A register-by-register check would need contents in which a letter differs between its positions, and the page predicts nothing for such contents. None is specified.
- **Bits 1 and 0 of each displayed component.** The pages give no value.
- **Bits 15-12 of $18-$1F.** These are unused, and the page states no effect (1.2).
- **Sprites 2-7 in SuperHires.** The page names four SuperHires sprite colours (16-19). In other modes these sprites take colours 21-23, 25-27 and 29-31 (p. 115, Table 4-3; p. 130, Table 4-6). No page says which of colours 16-19, if any, a pixel of sprites 2-7 takes in SuperHires. They are not used, and no expected value exists.
- **Attached sprites in SuperHires.** These are not described, and they are not used.
- **SHSH1 (70 ns) and SHSH0 (35 ns) positioning (p. 303).** They are not used. They belong to other claims, and SHSH0 is called "(unimplemented)".
- **The table's rows that combine two different colours.** The page does not say under what display condition such a row applies.
- **BPLCON0 bits 5 and 4 (BPLHWRM, SPRHWRM, p. 300).** They are named without a description and written as 0.
- **Bitplane fetch values for SuperHires.** These are not given; the high-resolution values are used (3.4 step 5).

## 8. Pages that disagree

- **Colour 16 as a sprite colour.** p. 302 lists "Sprite (Color 16) : AB-- CD-- EF--", while p. 103 says "The color value of the first register in each group of four registers is ignored by sprites. When the sprite bits select this register, the "transparent" value is used." Both pages agree on what is specified here: sprite value 00 is transparent, and the letters A-F are what the program writes in their positions. Nothing is expected of a displayed colour 16 (P4 tests only the absence of A-F from colours 17-19).
- **The horizontal origin of the display window.** Chapter 4 uses "a window with horizontal positions from pixel 64 to pixel 383" (p. 99), while chapter 3 gives "The normal PAL DIWSTRT is ($2C81)" (p. 59), a horizontal start of $81. The sprites are placed at HSTART 192-232, which is inside the window under both readings. No check depends on the origin.
- **Sprite placement resolution.** p. 128: "The sprites have a placement resolution on a full screen of 320 by 200 NTSC (320 by 256 PAL)". p. 303: "SuperHires mode has a finer granularity of sprite positioning than either Hires or Lores modes." This package uses only low-resolution positions (SH0 through SH8), which both pages allow.

---

## Appendix A: claims per letter (rows $18-$1F)

| Letter | Meaning (p. 302 key) | Positions in $18-$1F | Claim ids | Proposition |
|---|---|---|---|---|
| A | colour 16, red bit 3 | $18 b11, $1C b11 | APPC-sprtab.r18b11, CH4-appC-color.c146, APPC-sprtab.r1Cb11, CH4-appC-color.c210 | P4 |
| B | colour 16, red bit 2 | $18 b10, $1C b10 | APPC-sprtab.r18b10, CH4-appC-color.c147, APPC-sprtab.r1Cb10, CH4-appC-color.c211 | P4 |
| C | colour 16, green bit 3 | $18 b7, $1C b7 | APPC-sprtab.r18b07, CH4-appC-color.c150, APPC-sprtab.r1Cb07, CH4-appC-color.c214 | P4 |
| D | colour 16, green bit 2 | $18 b6, $1C b6 | APPC-sprtab.r18b06, CH4-appC-color.c151, APPC-sprtab.r1Cb06, CH4-appC-color.c215 | P4 |
| E | colour 16, blue bit 3 | $18 b3, $1C b3 | APPC-sprtab.r18b03, CH4-appC-color.c154, APPC-sprtab.r1Cb03, CH4-appC-color.c218 | P4 |
| F | colour 16, blue bit 2 | $18 b2, $1C b2 | APPC-sprtab.r18b02, CH4-appC-color.c155, APPC-sprtab.r1Cb02, CH4-appC-color.c219 | P4 |
| G | colour 17, red bit 3 | $19 b11, $1D b11 | APPC-sprtab.r19b11, CH4-appC-color.c162, APPC-sprtab.r1Db11, CH4-appC-color.c226 | P1 |
| H | colour 17, red bit 2 | $19 b10, $1D b10 | APPC-sprtab.r19b10, CH4-appC-color.c163, APPC-sprtab.r1Db10, CH4-appC-color.c227 | P1 |
| I | colour 17, green bit 3 | $19 b7, $1D b7 | APPC-sprtab.r19b07, CH4-appC-color.c166, APPC-sprtab.r1Db07, CH4-appC-color.c230 | P1 |
| J | colour 17, green bit 2 | $19 b6, $1D b6 | APPC-sprtab.r19b06, CH4-appC-color.c167, APPC-sprtab.r1Db06, CH4-appC-color.c231 | P1 |
| K | colour 17, blue bit 3 | $19 b3, $1D b3 | APPC-sprtab.r19b03, CH4-appC-color.c170, APPC-sprtab.r1Db03, CH4-appC-color.c234 | P1 |
| L | colour 17, blue bit 2 | $19 b2, $1D b2 | APPC-sprtab.r19b02, CH4-appC-color.c171, APPC-sprtab.r1Db02, CH4-appC-color.c235 | P1 |
| M | colour 18, red bit 3 | $18 b9, $19 b9, $1A b11, $1A b9, $1B b9, $1E b11 | APPC-sprtab.r18b09, CH4-appC-color.c148, APPC-sprtab.r19b09, CH4-appC-color.c164, APPC-sprtab.r1Ab11, CH4-appC-color.c178, APPC-sprtab.r1Ab09, CH4-appC-color.c180, APPC-sprtab.r1Bb09, CH4-appC-color.c196, APPC-sprtab.r1Eb11, CH4-appC-color.c242 | P2 |
| N | colour 18, red bit 2 | $18 b8, $19 b8, $1A b10, $1A b8, $1B b8, $1E b10 | APPC-sprtab.r18b08, CH4-appC-color.c149, APPC-sprtab.r19b08, CH4-appC-color.c165, APPC-sprtab.r1Ab10, CH4-appC-color.c179, APPC-sprtab.r1Ab08, CH4-appC-color.c181, APPC-sprtab.r1Bb08, CH4-appC-color.c197, APPC-sprtab.r1Eb10, CH4-appC-color.c243 | P2 |
| O | colour 18, green bit 3 | $18 b5, $19 b5, $1A b7, $1A b5, $1B b5, $1E b7 | APPC-sprtab.r18b05, CH4-appC-color.c152, APPC-sprtab.r19b05, CH4-appC-color.c168, APPC-sprtab.r1Ab07, CH4-appC-color.c182, APPC-sprtab.r1Ab05, CH4-appC-color.c184, APPC-sprtab.r1Bb05, CH4-appC-color.c200, APPC-sprtab.r1Eb07, CH4-appC-color.c246 | P2 |
| P | colour 18, green bit 2 | $18 b4, $19 b4, $1A b6, $1A b4, $1B b4, $1E b6 | APPC-sprtab.r18b04, CH4-appC-color.c153, APPC-sprtab.r19b04, CH4-appC-color.c169, APPC-sprtab.r1Ab06, CH4-appC-color.c183, APPC-sprtab.r1Ab04, CH4-appC-color.c185, APPC-sprtab.r1Bb04, CH4-appC-color.c201, APPC-sprtab.r1Eb06, CH4-appC-color.c247 | P2 |
| Q | colour 18, blue bit 3 | $18 b1, $19 b1, $1A b3, $1A b1, $1B b1, $1E b3 | APPC-sprtab.r18b01, CH4-appC-color.c156, APPC-sprtab.r19b01, CH4-appC-color.c172, APPC-sprtab.r1Ab03, CH4-appC-color.c186, APPC-sprtab.r1Ab01, CH4-appC-color.c188, APPC-sprtab.r1Bb01, CH4-appC-color.c204, APPC-sprtab.r1Eb03, CH4-appC-color.c250 | P2 |
| R | colour 18, blue bit 2 | $18 b0, $19 b0, $1A b2, $1A b0, $1B b0, $1E b2 | APPC-sprtab.r18b00, CH4-appC-color.c157, APPC-sprtab.r19b00, CH4-appC-color.c173, APPC-sprtab.r1Ab02, CH4-appC-color.c187, APPC-sprtab.r1Ab00, CH4-appC-color.c189, APPC-sprtab.r1Bb00, CH4-appC-color.c205, APPC-sprtab.r1Eb02, CH4-appC-color.c251 | P2 |
| S | colour 19, red bit 3 | $1B b11, $1C b9, $1D b9, $1E b9, $1F b11, $1F b9 | APPC-sprtab.r1Bb11, CH4-appC-color.c194, APPC-sprtab.r1Cb09, CH4-appC-color.c212, APPC-sprtab.r1Db09, CH4-appC-color.c228, APPC-sprtab.r1Eb09, CH4-appC-color.c244, APPC-sprtab.r1Fb11, CH4-appC-color.c258, APPC-sprtab.r1Fb09, CH4-appC-color.c260 | P3 |
| T | colour 19, red bit 2 | $1B b10, $1C b8, $1D b8, $1E b8, $1F b10, $1F b8 | APPC-sprtab.r1Bb10, CH4-appC-color.c195, APPC-sprtab.r1Cb08, CH4-appC-color.c213, APPC-sprtab.r1Db08, CH4-appC-color.c229, APPC-sprtab.r1Eb08, CH4-appC-color.c245, APPC-sprtab.r1Fb10, CH4-appC-color.c259, APPC-sprtab.r1Fb08, CH4-appC-color.c261 | P3 |
| U | colour 19, green bit 3 | $1B b7, $1C b5, $1D b5, $1E b5, $1F b7, $1F b5 | APPC-sprtab.r1Bb07, CH4-appC-color.c198, APPC-sprtab.r1Cb05, CH4-appC-color.c216, APPC-sprtab.r1Db05, CH4-appC-color.c232, APPC-sprtab.r1Eb05, CH4-appC-color.c248, APPC-sprtab.r1Fb07, CH4-appC-color.c262, APPC-sprtab.r1Fb05, CH4-appC-color.c264 | P3 |
| V | colour 19, green bit 2 | $1B b6, $1C b4, $1D b4, $1E b4, $1F b6, $1F b4 | APPC-sprtab.r1Bb06, CH4-appC-color.c199, APPC-sprtab.r1Cb04, CH4-appC-color.c217, APPC-sprtab.r1Db04, CH4-appC-color.c233, APPC-sprtab.r1Eb04, CH4-appC-color.c249, APPC-sprtab.r1Fb06, CH4-appC-color.c263, APPC-sprtab.r1Fb04, CH4-appC-color.c265 | P3 |
| W | colour 19, blue bit 3 | $1B b3, $1C b1, $1D b1, $1E b1, $1F b3, $1F b1 | APPC-sprtab.r1Bb03, CH4-appC-color.c202, APPC-sprtab.r1Cb01, CH4-appC-color.c220, APPC-sprtab.r1Db01, CH4-appC-color.c236, APPC-sprtab.r1Eb01, CH4-appC-color.c252, APPC-sprtab.r1Fb03, CH4-appC-color.c266, APPC-sprtab.r1Fb01 (CH4-appC-color.c268 for $1F b1 is not used, 1.4) | P3 |
| X | colour 19, blue bit 2 | $1B b2, $1C b0, $1D b0, $1E b0, $1F b2, $1F b0 | APPC-sprtab.r1Bb02, CH4-appC-color.c203, APPC-sprtab.r1Cb00, CH4-appC-color.c221, APPC-sprtab.r1Db00, CH4-appC-color.c237, APPC-sprtab.r1Eb00, CH4-appC-color.c253, APPC-sprtab.r1Fb02, CH4-appC-color.c267, APPC-sprtab.r1Fb00 (CH4-appC-color.c269 for $1F b0 is not used, 1.4) | P3 |

Each letter position carries two claims that say the same thing: an APPC-sprtab id and a CH4-appC-color id.

The ids break down as follows:

| Group | Claims |
|---|---|
| P1 (G-L) | 24 |
| P2 (M-R) | 72 |
| P3 (S-X) | 70 |
| P4 (A-F) | 24 |
| Group 1, bits 15-12, not used | 32 |
| Group 3, c268 and c269, not used as stated | 2 |
| **Total** | **224** |
