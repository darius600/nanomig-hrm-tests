# SuperHires colour encoding in the Denise colour registers — test specification

This specification was written from the package's claims (`claims-spec-stage.jsonl`, 218 claims) and the printed pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. It says what a 68000 program must set up, what the test must observe at the Chip-memory port and at the video output, which value or relation the pages predict, what must be true first, and which change of the stimulus must turn each check red. Where the pages give no value, none is specified.

All 218 claims sit on printed page 301 (217 claims) and page 302 (1 claim), Appendix C, section "SuperHires Mode and the Denise Color Registers". The key and all sixteen rows of both tables were read on the rendered printed pages 301 and 302 as well as in the text; they agree.

---

## 1. Claims not turned into a proposition

None. Every one of the 218 claims is the basis of at least one proposition below. Three limits set how far some of them can be graded. Each limit is repeated where it applies.

- **L1: the 192 table claims (`APPC-bpltab.*`) can come out "not observable".** Each one gets its own proposition (P4). But the pages never say which of the sixteen registers, or which half of a register, the chip reads for a given pixel. And the video output the test sees hides every second SuperHires pixel. So a register bit whose half never drives a visible pixel cannot be seen by any test with this output. P4 reports such a claim as NOT OBSERVABLE, which is neither a pass nor a failure. How many claims end up that way depends on the chip, not on the pages. For example, if the chip took the two pixels of one hires period from the two halves of one register, which the table's layout suggests but no page states, only one half of each register could ever reach a visible pixel.
- **L2: the dashes in the key give no expected value.** The key writes each component as `ab--`, with two encoded bits and two positions marked `-`. The pages do not say what the video output shows in those two lower bit positions. The test checks only that they carry no information beyond the two encoded bits (P2). It never checks them against a value.
- **L3: `APPC-shr.c9` (sprites) is checked for sprite 0 only.** The sprite key on p.302 names Colors 16–19. Chapter 4 assigns 17–19 to sprites 0 and 1 and says the first register of each group is never shown (p.103). So Color 16, with letters A–F, cannot appear on screen. No page says how sprites 2–7 are coloured in SuperHires mode. Nothing is specified for them.

---

## 2. Pages this specification rests on

| Printed page | What is used |
|---|---|
| 301 | The claims: the SuperHires paragraphs, the bitplane key and the sixteen-row bitplane table |
| 302 | The sprite claim, the sprite key and table, and "loss of lower bits of precision" |
| 296, 297 | SuperHires is provided by the enhanced Denise. "There are new color register interpretations as part of the SuperHires mode." |
| 299 | VPOSR chip identification and DENISEID ($FC) |
| 300 | The 35 ns pixel rate, 2-bitplane maximum, and BPLCON0 bits HIRES, BPU, SHRES |
| 303 | SPRxCTL bits SHSH1/SHSH0 (left 0) |
| 29, 30 | Copper list example (MOVE, WAIT, colour changes mid-display), and the Copper start sequence |
| 42–44 | Colour indirection. Plane 1 is the least significant bit of a pixel's value (Figure 3-4). |
| 46, 47, 92 | Colour register layout (bits 15–12 unused, set to 0; 11–8 red, 7–4 green, 3–0 blue). 4,096 colours. Colour registers are write-only. |
| 48, 49, 90 | BPLCON0 BPU and HIRES |
| 57–63 | Display window (resolution one lores pixel) and data fetch (hires normal $3C/$D4) |
| 61, 270, 272 | Pointers increment and add the modulo. "The MSB is output first, and is, therefore, always on the left." Pointers must be reset every vertical blank. |
| 94 | Table 3-18: a 2-bitplane hires playfield selects registers 0–3 only |
| 101–109 | Sprites: lores-sized pixels in every playfield mode. Data structure. Sprite 0 uses 17–19, and binary 00 is transparent. |
| 210, 211 | BPLCON2 priority codes. "PF2P2 - PF2P0 … are the priority bits for normal (non-dual) playfields." |
| 273–277 | COLORxx bit names R3…B0. Copper MOVE format. COP1LC/COPJMP1. DDFSTRT/DDFSTOP extreme values. DIWSTRT/DIWSTOP. |
| 278 | DMACON bits |
| Appendix B | Register addresses |

---

## 3. Notation

**SuperHires pixel, hires period, visible pixel.** "SuperHires mode provides a 35ns pixel display rate - twice the horizontal resolution of Hires mode, and four times the Lores rates" (p.300). A hires period is the time of one hires pixel, which is two SuperHires pixels. The test's video output shows one pixel per hires period, so every second SuperHires pixel is invisible. A *visible pixel* is one output pixel position x on an output line, counted from that line's horizontal sync at the hires pixel rate.

**Value of a pixel.** v = 2·(bit from plane 2) + (bit from plane 1). "Values in the highest numbered bitplane have the highest significance" (p.43; Figure 3-4 on p.44 marks plane 1 "least significant"). With two bitplanes, values run from 0 to 3.

**Color c.** The key on p.301 names the four bitplane colours "Bitplane (Color 0)" … "(Color 3)". The page introduces them as the colours of "bitplane registers 0-3". Color c is the colour of pixels whose value is c. Each Color has three two-bit components: R_c, G_c and B_c.

**Letters.** The key assigns two letters to each component. The first letter is the upper bit (bit 3 of the 4-bit component) and the second is the lower bit (bit 2). The two `-` positions are bits 1 and 0 (L2).

```
                              R    G    B
                             ---- ---- ----
      Bitplane (Color 0) :   ab-- cd-- ef--
      Bitplane (Color 1) :   gh-- ij-- kl--
      Bitplane (Color 2) :   mn-- op-- qr--
      Bitplane (Color 3) :   st-- uv-- wx--
```

The positions under "R" are R3 R2 R1 R0, as p.273 names the colour register bits (`R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0`). The same names are used for the 4-bit red, green and blue of the video output.

**The table on p.301.** It is reproduced here from the printed page. Bits 15–12 are `.` in every row.

| COLOR | 11 | 10 | 09 | 08 | 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 00 | a | b | a | b | c | d | c | d | e | f | e | f |
| 01 | g | h | a | b | i | j | c | d | k | l | e | f |
| 02 | m | n | a | b | o | p | c | d | q | r | e | f |
| 03 | s | t | a | b | u | v | c | d | w | x | e | f |
| 04 | a | b | g | h | c | d | i | j | e | f | k | l |
| 05 | g | h | g | h | i | j | i | j | k | l | k | l |
| 06 | m | n | g | h | o | p | i | j | q | r | k | l |
| 07 | s | t | g | h | u | v | i | j | w | x | k | l |
| 08 | a | b | m | n | c | d | o | p | e | f | q | r |
| 09 | g | h | m | n | i | j | o | p | k | l | q | r |
| 0A | m | n | m | n | o | p | o | p | q | r | q | r |
| 0B | s | t | m | n | u | v | o | p | w | x | q | r |
| 0C | a | b | s | t | c | d | u | v | e | f | w | x |
| 0D | g | h | s | t | i | j | u | v | k | l | w | x |
| 0E | m | n | s | t | o | p | u | v | q | r | w | x |
| 0F | s | t | s | t | u | v | u | v | w | x | w | x |

Row labels are hexadecimal register numbers. Row `0A` is COLOR10, at address $194. The address of row n is $180 + 2n.

**Writing the table.** This is a restatement of the printed table, not an extra claim. Row n, with p = n mod 4 and q = n div 4, carries Color p in bits 11–10, 7–6 and 3–2, and Color q in bits 9–8, 5–4 and 1–0, each as (upper letter, lower letter). A program that encodes four Colors therefore writes:

`COLOR[n] = R_p<<10 | R_q<<8 | G_p<<6 | G_q<<4 | B_p<<2 | B_q`,  with bits 15–12 = 0

Bits 15–12 are 0 because p.92 says of those bits "Unused (set these to 0)". The sprite table on p.302 has the same layout at rows 10–1F, which are COLOR16–COLOR31, holding Colors 16–19.

**Palettes.** Each palette gives the three two-bit components of Colors 0–3, followed by the sixteen register values that result from the formula. Q0 makes all four Colors differ in every component. Q1–Q5 set letter number l (a = 0 … x = 23) to bit j−1 of (l + 1), so any two letters differ in some palette. Z and O set every letter to 0 and to 1. Across Q0–Q5, Z and O, every component of every Color takes all four values, and every letter is both 0 and 1. S0–S3 are Q0 with Color 1 set to (s, s, s). Q3x is Q3 with Colors 1 and 2 exchanged; it is used only in contrast V1.

| Palette | Color 0 (R,G,B) | Color 1 | Color 2 | Color 3 |
|---|---|---|---|---|
| Q0 | 0,1,2 | 1,2,3 | 2,3,0 | 3,0,1 |
| Q1 | 2,2,2 | 2,2,2 | 2,2,2 | 2,2,2 |
| Q2 | 1,2,1 | 2,1,2 | 1,2,1 | 2,1,2 |
| Q3 | 0,1,3 | 2,0,1 | 3,2,0 | 1,3,2 |
| Q4 | 0,0,0 | 1,3,3 | 3,2,0 | 0,0,1 |
| Q5 | 0,0,0 | 0,0,0 | 0,1,3 | 3,3,3 |
| Z | 0,0,0 | 0,0,0 | 0,0,0 | 0,0,0 |
| O | 3,3,3 | 3,3,3 | 3,3,3 | 3,3,3 |
| S0 | 0,1,2 | 0,0,0 | 2,3,0 | 3,0,1 |
| S1 | 0,1,2 | 1,1,1 | 2,3,0 | 3,0,1 |
| S2 | 0,1,2 | 2,2,2 | 2,3,0 | 3,0,1 |
| S3 | 0,1,2 | 3,3,3 | 2,3,0 | 3,0,1 |
| Q3x | 0,1,3 | 3,2,0 | 2,0,1 | 1,3,2 |

| Palette | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Q0 | 005A | 049E | 08D2 | 0C16 | 016B | 05AF | 09E3 | 0D27 | 0278 | 06BC | 0AF0 | 0E34 | 0349 | 078D | 0BC1 | 0F05 |
| Q1 | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA | 0AAA |
| Q2 | 05A5 | 0969 | 05A5 | 0969 | 0696 | 0A5A | 0696 | 0A5A | 05A5 | 0969 | 05A5 | 0969 | 0696 | 0A5A | 0696 | 0A5A |
| Q3 | 005F | 0817 | 0C93 | 04DB | 024D | 0A05 | 0E81 | 06C9 | 036C | 0B24 | 0FA0 | 07E8 | 017E | 0936 | 0DB2 | 05FA |
| Q4 | 0000 | 04CC | 0C80 | 0004 | 0133 | 05FF | 0DB3 | 0137 | 0320 | 07EC | 0FA0 | 0324 | 0001 | 04CD | 0C81 | 0005 |
| Q5 | 0000 | 0000 | 004C | 0CCC | 0000 | 0000 | 004C | 0CCC | 0013 | 0013 | 005F | 0CDF | 0333 | 0333 | 037F | 0FFF |
| Z | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 | 0000 |
| O | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF | 0FFF |
| S0 | 005A | 0012 | 08D2 | 0C16 | 0048 | 0000 | 08C0 | 0C04 | 0278 | 0230 | 0AF0 | 0E34 | 0349 | 0301 | 0BC1 | 0F05 |
| S1 | 005A | 0456 | 08D2 | 0C16 | 0159 | 0555 | 09D1 | 0D15 | 0278 | 0674 | 0AF0 | 0E34 | 0349 | 0745 | 0BC1 | 0F05 |
| S2 | 005A | 089A | 08D2 | 0C16 | 026A | 0AAA | 0AE2 | 0E26 | 0278 | 0AB8 | 0AF0 | 0E34 | 0349 | 0B89 | 0BC1 | 0F05 |
| S3 | 005A | 0CDE | 08D2 | 0C16 | 037B | 0FFF | 0BF3 | 0F37 | 0278 | 0EFC | 0AF0 | 0E34 | 0349 | 0FCD | 0BC1 | 0F05 |
| Q3x | 005F | 0C93 | 0817 | 04DB | 036C | 0FA0 | 0B24 | 07E8 | 024D | 0E81 | 0A05 | 06C9 | 017E | 0DB2 | 0936 | 05FA |

The sprite palettes are T0, where Color 16+v = (v, v+1, v+2) mod 4, and its complement T0c. They are written to COLOR16–31 (rows 10–1F):

| Palette | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1A | 1B | 1C | 1D | 1E | 1F |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| T0 | 005A | 049E | 08D2 | 0C16 | 016B | 05AF | 09E3 | 0D27 | 0278 | 06BC | 0AF0 | 0E34 | 0349 | 078D | 0BC1 | 0F05 |
| T0c | 0FA5 | 0B61 | 072D | 03E9 | 0E94 | 0A50 | 061C | 02D8 | 0D87 | 0943 | 050F | 01CB | 0CB6 | 0872 | 043E | 00FA |

---

## 4. Setup common to all runs

### 4.1 System

- Enhanced Chip Set, PAL, 2 MB Chip memory.
- The program uses only 68000 instructions and no timing loops. The Copper paces every frame. The program runs unchanged on both CPU cores, each separately, with the second core in its default 68000 behaviour. The claims say nothing about the CPU, so no 68010 or 68020 setting is required.
- All data, Copper lists and results are placed below $80000, so the OCS contrast (C1) can use the same program.
- The claims name no video standard. NTSC is not required. The geometry below is valid under NTSC unchanged.

### 4.2 Data in Chip memory

- **Pixel stream.** A plane-1 buffer and a plane-2 buffer, each 32768 words. That is enough for 200 lines at any fetch width the data-fetch registers allow.
  - Words 0–255 of both buffers are $FFFF. This is the *R section*, where every pixel has value 3.
  - Words 256–32767 are the *pattern section*. Pixel values v[0], v[1], … come from a 32-bit xorshift generator: `x ^= x<<13; x ^= x>>17; x ^= x<<5` (all modulo 2^32), with seed x = $2545F491. After each step the pixel value is `x >> 30`.
  - Pattern pixel j of pattern word k goes to bit 15−j of that word: plane 1 takes bit 0 of the value and plane 2 takes bit 1.
  - The first pattern words are plane 1 $AC41 $C956 $EBAD $15F3 and plane 2 $CE9D $9A90 $89DC $A548, so the builder can check the generator.
  - Nothing in a check depends on these values. The expected values are always taken from the words the port shows were fetched (§5, R2). The stream exists to cover every neighbour context (C8).
- **Zero buffer.** 32768 words of $0000. Both plane pointers may point at it.
- **Sprite 0 structure** (p.105–108). The control words are SPR0POS = $5060 and SPR0CTL = $8C00. That gives VSTART $50, VSTOP $8C, lores HSTART $C0, H0 = 0, and SHSH1 = SHSH0 = 0 (p.303).
  - 60 data line pairs follow, each written as low word then high word. Line j with j mod 3 = 0 has value 1 ($FFFF, $0000). j mod 3 = 1 has value 2 ($0000, $FFFF). j mod 3 = 2 has value 3 ($FFFF, $FFFF).
  - The structure ends with $0000, $0000.
- **Null sprite.** $0000, $0000.
- **Copper lists** F0–F10 (§4.4), and a result area.

### 4.3 Register values written by each frame's Copper header

Every list begins, before its first WAIT, with MOVEs that set:

| Register | Value |
|---|---|
| BPLCON0 ($100) | $2040 in SuperHires frames: BPU = 010, HIRES = 0, SHRES = 1, HAM = 0, DPF = 0, LACE = 0 (p.300: "HIRES … Set it to zero if SHRES enabled", "BPU … Depth of SuperHires mode (1 or 2)", "SHRES … SuperHires 35ns pixel enable bit"). $A000 in hires frames. $2000 in the lores frame. COLOR (bit 9) = 0; p.63: "RGB video is correctly generated regardless". |
| BPLCON1 ($102) | $0000 |
| BPLCON2 ($104) | $0024: PF2P = PF1P = 100, so sprites are in front of both playfield codes (p.210–211) |
| DIWSTRT ($08E) | $2C81 |
| DIWSTOP ($090) | $F4A1. The window covers lines $2C–$F3 (200 lines) and lores positions $81–$1A0 (288 lores pixels = 576 hires periods). |
| DDFSTRT ($092), DDFSTOP ($094) | $003C, $00D4, the hires normal values (p.60). The pages give no SuperHires values. Fallback under C6: $0028 / $00D8, the extreme entries of p.277. |
| BPL1MOD ($108), BPL2MOD ($10A) | $0000, so each line continues where the previous one stopped (p.61) |
| BPL1PTH/L ($0E0/$0E2), BPL2PTH/L ($0E4/$0E6) | the plane-1 and plane-2 stream buffers |
| SPR0PT–SPR7PT ($120–$13E) | null sprite. In F7–F10, SPR0PT points to the sprite 0 structure. |
| COLOR00–COLOR0F | the frame's first palette (§4.6) |
| COLOR10–COLOR1F | T0, except T0c in F8 |

### 4.4 Frames and Copper lists

There is one Copper list per frame. Window lines are numbered y = 0…199 from beam line $2C.

- A band change is a group of MOVEs placed directly after `WAIT (beam line $2C+y, horizontal $00)`. Its first word is ((($2C+y) << 8) | $01) and its second is $FFFE (p.30–31). Colour register MOVEs appear nowhere else.
- Each list ends with MOVEs to COP1LCH/COP1LCL that point at the next frame's list, then `WAIT $FFFF,$FFFE` (p.30). COP1LC "is automatically used at the beginning of each vertical blank time" (p.276).
- F10's list points at itself.
- F0 is a lead-in copy of F1 and is never graded.

A band change that writes a whole palette (sixteen MOVEs, four memory cycles each, p.275) runs past the start of its line's data fetch. The line it starts on is a *spacer line*: its display may be disturbed and it is never graded. The spacer lines are named in the frame plan, and the graded-line rule of §4.7 excludes them by itself. A band change of at most four MOVEs is planned to end before the line's first bitplane fetch, so its line is graded. C7 checks this at the port.

### 4.5 The 68000 program

1. Write DMACON ($096) = $7FFF (all DMA off).
2. Read VPOSR ($004) and DENISEID ($07C) and store both words in the result area.
3. Build every buffer and Copper list of §4.2 and §4.4.
4. Start the Copper exactly as p.30 prescribes: "you must initialize one of the Copper location registers (COP1LC or COP2LC) and write to its strobe address before Copper DMA is turned on". Write COP1LC (longword to $080) = address of F0, strobe COPJMP1 ($088), then write DMACON = $83A0 (SET, DMAEN, BPLEN, COPEN, SPREN; p.278).
5. Wait until at least 12 vertical blanks have passed. Poll VPOSR/VHPOSR for the wrap of the vertical position. The CPU must not write any buffer or list while it waits.
6. Store a completion word in the result area.

### 4.6 Frame plan

| Frame | Mode | Content (window lines) |
|---|---|---|
| F0 | SuperHires | Copy of F1. Lead-in, never graded. |
| F1 | SuperHires | Lines 0–23: Q0. Then eleven bands, each one spacer line plus 12 lines, in the order Q1, Q2, Q3, Q4, Q5, Z, O, S0, S1, S2, S3 (lines 24–166). Then a spacer line (167) and Q0 on lines 168–199. |
| F2, F3, F4 | SuperHires | Lines 0–23: Q0. Then perturbation bands of 2 lines each: band k changes one bit of one register of Q0, with the band change restoring the previous band's register and changing the next. Then Q0 on the last 16 lines. F2 and F3 carry 80 bands each (lines 24–183) and F4 carries 32. Together they perturb each of the 192 bits (n = 00…0F, b = 00…11) exactly once. |
| F5 | Hires ($A000) | Lines 0–23: Q0. Then 16 two-line bands with COLOR01 = $0kkk (k = 0…15, all other registers Q0). Then 16 two-line bands, each inverting one register n (Q0[n] XOR $0FFF), n = 00…0F, one band each. Then Q0 on the remaining lines. |
| F6 | Lores ($2000) | Lines 0–23: Q0. Then 16 two-line bands with COLOR01 = $0kkk. Then Q0. |
| F7 | SuperHires | Lines 0–29: Q0 on the stream. At line 30 a band change of four MOVEs reloads BPL1PT/BPL2PT with the zero buffer; line 30 is graded. Sprite 0 lines are beam lines $50–$8B (window lines 36–95), in 20 blocks of three lines (values 1, 2, 3). Blocks 0–1 and 15–19 are the baseline. Blocks 2–14 each invert one register r = COLOR16, 20, 21, …, 31 (XOR $0FFF), restored at the next block. |
| F8 | SuperHires | As F7 without inversions, with COLOR10–1F = T0c |
| F9 | SuperHires | As F7 without inversions, with COLOR00–0F = Q1 from line 30 on. The band change at line 30 carries the palette, so line 30 is a spacer. |
| F10 | Hires ($A000) | As F7, with blocks 2–14 inverting 16, 20–31, blocks 15–17 inverting 17, 18 and 19, and blocks 18–19 as baseline |

Contrast variants (§6, §7) change one item of this plan and nothing else.

### 4.7 Graded lines and graded pixels

A *source line* is one beam line's burst of bitplane fetches at the Chip-memory port. Source line y of frame f is *graded* when all of the following hold:

- (a) f's Copper fetches come from f's list (C4).
- (b) Line y fetched the same number F of words from each plane buffer, from consecutive addresses, and F is the same for every line of the frame (C3).
- (c) No Copper MOVE to a colour register, to BPLCON0–2, or to a bitplane pointer is fetched between line y's first and last bitplane fetch (C7).

The palette in effect on a graded line is the frame plan's palette for its band. Its values are also rebuilt from the Copper MOVEs fetched before the line's first bitplane fetch: IR1 is the register address and IR2 the data (p.275). The rebuilt values are reported next to every result. A difference from the plan is reported, and is not a precondition.

A *graded pixel* is a visible pixel x on an output copy of a graded line, with x inside the check interval (R3). If the line doubler shows a source line more than once, every copy is graded and the copies must agree.

---

## 5. Reference frames and how the test checks their anchors

**R1: source line to output line.** Output lines are delimited by horizontal sync and frames by vertical sync. Source line k of a frame is paired with output line k + Δ. Δ is the one shift for which the output lines that show the Color-3 run of R3 are exactly the copies of the *pure R lines*, the source lines whose fetched words all lie in the R section. Check: Δ exists, is unique, and is the same in every frame.

**R2: pixel index.** On a graded line with F fetched word pairs, pixel i (0 ≤ i < 16F) is bit 15 − (i mod 16) of the ⌊i/16⌋-th fetched word of each plane.

- The MSB is first: "The MSB is output first, and is, therefore, always on the left" (p.272).
- Its value v[i] is taken from those fetched words as the port shows them.
- Output pixel x shows pixel i as follows:
  - SuperHires frames: i = 2x − s (two SuperHires pixels per hires period, p.300)
  - hires frames: i = x − s
  - lores frame: i = ⌊(x − s)/2⌋

s is a constant per frame. It is found on the frame's top Q0 lines (lines 0–23; in F7–F10, lines 0–29) as the single s for which every graded pixel of those lines matches:

- SuperHires frames: bits 3 and 2 of R, G and B equal Q0's letters of Color v[i] (P1)
- hires and lores frames: all twelve bits equal register v[i] of Q0 (p.94, p.273)

Check: exactly one s fits, and in F1–F6 the same s fits the bottom Q0 lines. If none fits, that is a failure of P1 (SuperHires) or of the hires/lores baseline on those lines. If more than one fits, the stimulus is ambiguous and nothing is graded in that frame. In SuperHires frames, s mod 2 tells which SuperHires pixel of a hires period the output shows. The test reports it and no proposition depends on its value.

**R3: check interval.** On every pure R line of the frame, the output shows a single run [xL, xR] of Color 3, meaning bits 3–2 of R, G and B equal 3, 0 and 1, which is Q0's Color 3. In hires and lores frames the run is $C16, the Q0 value of COLOR03. The run is the display window where fetched data is shown. The check interval is x from xL + 8 to xR − 8 that also satisfies 16 ≤ i(x) ≤ 16F − 17. That excludes one data word at each end of the fetch and 8 visible pixels at each edge of the run. Check: C6.

**R4: letters to output bits.** A component's upper letter is output bit 3 and its lower letter is output bit 2 (§3). Bits 1–0 carry no letter (L2).

**R5: frame and band boundaries.** These come from the port. The Copper fetch addresses identify the list, and hence the frame. The position of each band-change MOVE among the bitplane fetch bursts identifies the line at which a band begins.

---

## 6. Preconditions common to the propositions

Each precondition is asserted before any proposition is graded. It is followed by the change that must make it fail while the program still finishes.

| | Precondition | How the test knows | Contrast that must make it fail |
|---|---|---|---|
| C1 | The Enhanced Chip Set is present, because SuperHires "is provided by" the enhanced Denise (p.296) | The stored DENISEID has low byte $FC ("The enhanced HighRes Denise (8373) will return $FC in the lower 8 bits", p.299). The stored VPOSR bits 14–8 equal $20 ("20 for PAL", p.299). | Select OCS. p.299: the original Denise returns "whatever value is left over on the bus", and the PAL Agnus reports $00. |
| C2 | The program ran to its end | The completion word is present in the result area | Omit step 6 |
| C3 | Bitplane DMA fetched two planes on every graded line | At the port, each graded line has F ≥ 16 fetches from each plane buffer, the same count per plane, consecutive addresses inside the buffers, no other bitplane addresses, and data equal to what the program wrote | BPLCON0 = $1040 (one plane). Only plane-1 addresses are fetched. |
| C4 | Each frame ran its own list with the planned header | The Copper fetch addresses per frame follow F0→F1→…→F10. The header MOVEs (BPLCON0–2, DIW, DDF, modulos, pointers) carry the §4.3 data and are fetched before the frame's first bitplane fetch. | Make F5's list point at itself instead of F6. The frame order fails. |
| C5 | The claims' condition "In SuperHires mode" holds while pixels are counted | In SuperHires frames a unique s fits with the mapping i = 2x − s (R2), on the top and on the bottom Q0 lines. Only a 35 ns pixel rate makes two consecutive fetched pixels per hires period line up with the output. | BPLCON0 = $A000 in F1. No s fits the SuperHires mapping and one fits the hires mapping. |
| C6 | The check interval exists and lies inside the window | The Color-3 run has identical ends on all pure R lines. Its width is at least 256 and at most 576 hires periods: the window is 288 lores pixels, and a lores pixel is two hires periods (p.57 "resolution … one low resolution pixel", p.300). The check interval is at least 256 wide. If it is narrower with DDF $3C/$D4, the whole run is repeated with $28/$D8. If it is still narrower, the setup is reported as unable to display the pattern, which is not a finding about any claim. | DIWSTRT = $2CC1, DIWSTOP = $F401, a window of 64 lores pixels. The run can be at most 128 periods wide ("You cannot display objects outside of the defined display window", p.57). |
| C7 | No colour or control register changes while a graded line is shown | Every Copper MOVE to a colour register, BPLCON0–2 or a bitplane pointer follows a WAIT with horizontal position $00. Each is fetched after the previous line's last bitplane fetch and, except on the planned spacer lines, before its own line's first bitplane fetch. Every line of the frame plan that is not a spacer line is graded. | Put one perturbation band's MOVEs behind `WAIT (line, $80)`. That line is not graded, and C7 reports a planned line that is missing. |
| C8 | Every neighbour context occurs visibly in every band of F1–F4 | On the graded pixels of every band (i = 2x − s), each of the 16 ordered pairs (p, q) occurs both as (v[i], v[i+1]) and as (v[i−1], v[i]). So every value is seen as the left and as the right member of every pair, whichever pixel of a hires period the output shows. | Restrict the pattern section to values 0 and 3 |
| C9 | No sprite pixels in F1–F6 | All sprite fetches in F1–F6 read only $0000 control words | Point SPR0PT at the sprite 0 structure in F1 |

---

## 7. Propositions

### P1: every visible SuperHires pixel shows its Color's encoded bits (12 propositions)

**1. Proposition.** In SuperHires mode, with COLOR00–COLOR0F written as the p.301 table prescribes for four Colors, every visible pixel of value c shows the two letters of Color c for component X in bits 3 and 2 of X, with the upper letter in bit 3.

| Id | Claim | Color | Component | Bit 3, bit 2 |
|---|---|---|---|---|
| P1.0R | APPC-bplkeykey.c0r | 0 | red | a, b |
| P1.0G | APPC-bplkeykey.c0g | 0 | green | c, d |
| P1.0B | APPC-bplkeykey.c0b | 0 | blue | e, f |
| P1.1R | APPC-bplkeykey.c1r | 1 | red | g, h |
| P1.1G | APPC-bplkeykey.c1g | 1 | green | i, j |
| P1.1B | APPC-bplkeykey.c1b | 1 | blue | k, l |
| P1.2R | APPC-bplkeykey.c2r | 2 | red | m, n |
| P1.2G | APPC-bplkeykey.c2g | 2 | green | o, p |
| P1.2B | APPC-bplkeykey.c2b | 2 | blue | q, r |
| P1.3R | APPC-bplkeykey.c3r | 3 | red | s, t |
| P1.3G | APPC-bplkeykey.c3g | 3 | green | u, v |
| P1.3B | APPC-bplkeykey.c3b | 3 | blue | w, x |

All twelve also rest on APPC-shr.c8. Page 301.

**2. Stimulus.** Setup §4 and frame F1: palettes Q0, Q1–Q5, Z, O and S0–S3, each written as the table prescribes. Further instances are the Q0 lines of F2–F4 and lines 0–29 of F7–F9. F5, F6 and F10 are not SuperHires frames and are not P1 instances.

**3. Observation.** The video output, at every graded pixel (§4.7) of every band of F1, of the Q0 lines of F2–F4, and of lines 0–29 of F7–F9.

**4. Expected value.** Bit 3 of component X equals the upper letter and bit 2 the lower letter of X of Color v[i] in the band's palette, with i = 2x − s.

The source is the key and the sentence "SuperHires bitplane colors are specially encoded in the sixteen lower color registers:" (p.301). The key assigns letters to Colors, and the table places them in registers.

Quantifiers:
- The claims print no quantifier, and the encoding is stated without condition. So it must hold at every graded pixel, one mismatch fails, and it is graded for:
  - every pixel value 0–3
  - every neighbour on either side (C8)
  - every letter at 0 and at 1
  - every pair of letters set to different values in some palette, so an exchanged letter is caught (§3)
- Bits 1–0 are not compared (L2; P2 covers them).

**5. Preconditions.** C1–C9.

**6. Contrast (V1).** In F1, write Q3x instead of Q3 for the Q3 band and keep the expectation of Q3. Colors 1 and 2 of Q3 differ in an encoded bit of every component: (2,0,1) against (3,2,0). The pages therefore predict that value-1 and value-2 pixels of that band show the other Color, and P1 fails there. Every precondition still holds.

**7. Reference frame.** R2 gives i and s, R3 the check interval, R4 the output bits. All three are checked by C5, C6 and R2's bottom-line check.

### P2: only two bits of red, green and blue per pixel (3 propositions)

**1. Proposition.** In SuperHires mode, the 4-bit value of component X shown by a visible pixel of value c depends only on the two encoded letters of X of Color c. Pixels of value c whose Color c has the same two letters for X show the same 4-bit X, whatever the neighbours, the other Colors, the line or the frame.

| Id | Claims | Component |
|---|---|---|
| P2.R | APPC-shr.c3, CH4-appC-context.c3; the `--` half of APPC-bplkeykey.c{0..3}r | red |
| P2.G | APPC-shr.c4, CH4-appC-context.c4; the `--` half of APPC-bplkeykey.c{0..3}g | green |
| P2.B | APPC-shr.c5, CH4-appC-context.c5; the `--` half of APPC-bplkeykey.c{0..3}b | blue |

Page 301.

**2. Stimulus.** As for P1: all bands of F1, the Q0 lines of F2–F4, and lines 0–29 of F7–F9.

**3. Observation.** All four bits of R, G and B at every graded pixel of those lines, grouped by (value c, component X, the two letters of X of Color c in the band's palette).

**4. Expected relation.** Within each group, one single 4-bit value.

The source is "There are only two bits of red, green and blue color resolution per hires pixel" (p.301), and the key's `--`. The pages give no value for bits 1–0 (L2), so the test checks only that they are a function of the two letters, never what they are.

The quantifier "only":
- Within SuperHires mode, no program that writes the registers as the table prescribes can make a value-c pixel show more than four distinct values of X, one per pair of letters.
- It is graded over every context (C8), over palettes in which the other Colors' letters change while Color c's stay fixed, and over every line and frame.
- The test reports, per group, how many palettes contributed. For every (c, X), at least one group must contain pixels from two palettes that differ in some other Color's letters of X. The palettes of §3 meet this for all twelve (c, X).
- Across Q0–Q5, Z and O, every (c, X) takes all four letter pairs (§3). So "two bits" is also graded from below: bits 3–2 take all four values (P1).

**5. Preconditions.** C1–C9.

**6. Contrast (V2).** No stimulus inside SuperHires mode is predicted by the pages to break P2, so the contrast leaves SuperHires mode on purpose. Run F1 with BPLCON0 = $A000. C5 is then evaluated in its hires form (i = x − s) and holds, and the other preconditions hold unchanged.

Applied to the hires pixels, P2 fails. In hires a value-c pixel shows all of register c (p.94, Table 3-18; p.273). Under the table, bits 9–8, 5–4 and 1–0 of registers 0–3 hold Color 0's letters (row q = 0). So for c = 1, 2, 3 the lower two output bits follow Color 0's letters. In Q1 and Q3, for example, Color 1's red is 2 in both, while Color 0's red is 2 and 0.

**7. Reference frame.** As P1.

### P3: SuperHires colour control is coarser than Hires and than Lores (2 propositions)

**1. Proposition.** A program controls each component of a pixel's colour in 16 steps in Hires and in Lores, but only in 4 steps in SuperHires.

| Id | Claims | Comparison |
|---|---|---|
| P3.H | APPC-shr.c1, CH4-appC-context.c1 | SuperHires coarser than Hires |
| P3.L | APPC-shr.c1, CH4-appC-context.c2 | SuperHires coarser than Lores |

Page 301.

**2. Stimulus.**
- SuperHires: the S0–S3 bands of F1. Color 1 = (s, s, s), every register written by the table.
- Hires: F5's sixteen bands with COLOR01 = $0kkk.
- Lores: F6's sixteen bands with COLOR01 = $0kkk.

**3. Observation.** The 4-bit R, G and B at graded pixels with v[i] = 1 in those bands. In hires i = x − s and in lores i = ⌊(x − s)/2⌋.

**4. Expected value.**
- Hires and Lores: in band k, each of R, G and B equals k in all four bits, so each component takes 16 distinct values. The sources are "Twelve bits of color selection allow you to define, for each of the 32 registers, one of 4,096 possible colors" (p.46), "each one has 4 bits of red, 4 bits of green, and 4 bits of blue" (p.92), and Table 3-18 (p.94: value 0001 selects register 1).
- SuperHires: in band Ss, bits 3–2 of each component equal s (P1). Across S0–S3 each component takes exactly four distinct 4-bit values (P2).
- The relation 4 < 16 holds for R, G and B, for each comparison.

The source for the comparison is "SuperHires mode has a coarser granularity of color control than either Hires or Lores modes" (p.301). The quantifier "either" is graded as two separate comparisons.

**5. Preconditions.** C1–C9. In F5 and F6 they are evaluated in their hires and lores forms: s fits the mapping of that mode, and the pure R lines show COLOR03 = $C16.

**6. Contrasts.**
- V3a (SuperHires half): run F1 with BPLCON0 = $A000 and replace the S0–S3 bands by sixteen bands with COLOR01 = $0kkk. The pages predict sixteen levels, so "exactly four" fails.
- V3b (Hires half): in F5, write COLOR01 = ($0kkk AND $0CCC). The pages predict four levels, so "sixteen" fails.

Preconditions hold in both.

**7. Reference frame.** R2 in each mode's form, and R3.

### P4: each bit of each lower colour register carries the letter the table puts there (192 propositions)

**1. Proposition P4.n.b.** In SuperHires mode, bit b of COLOR n carries the letter L in row n, column b of the p.301 table. L is component X of Color c, upper or lower, as the key names it (§3). Inverting that single bit, with every other register bit as the table prescribes for Q0, can change the display in only one place: output bit 3 of X (if L is an upper letter) or bit 2 (if lower), on pixels of value c. Bits 1–0 of X on value-c pixels are not compared (L2). The change, where it appears, is the inversion.

The claim is APPC-bpltab.r{n}b{b}, with n = 00…0F (hexadecimal, as the table's rows) and b = 00…11. For example, APPC-bpltab.r05b00 says bit 00 of COLOR05 carries `l`, the lower blue letter of Color 1. The letter of every (n, b) is the table in §3. Page 301.

**2. Stimulus.** F2–F4. Each perturbation band holds Q0 with the single bit b of register n inverted. The band change that starts band k+1 restores band k's register. Bits 15–12 are never touched.

**3. Observation.**
- Every graded pixel of the band's two lines is compared with its *baseline*.
- For bits 3–2 of each component, the baseline is P1's expectation under Q0.
- For bits 1–0, the baseline is the value shown on the same frame's Q0 lines by visible pixels with the same (v[i−1], v[i], v[i+1]).
- A pixel whose triple never occurs visibly on those Q0 lines is excluded from the bits 1–0 comparison, and the count is reported.

**4. Expected result and verdict.**
- **FAIL** if any graded pixel differs from its baseline anywhere other than the allowed places: bit 3 or 2 of X (per L) and bits 1–0 of X, on pixels of value c. A change on a pixel of another value in any of its twelve bits, in another component, or in the other encoded bit of X fails.
- **OBSERVED** if at least one graded pixel of value c shows the allowed bit inverted.
- The verdict is PASS when OBSERVED and not FAIL. It is **NOT OBSERVABLE** when neither (L1).

The source is the table row and the key (p.301). A register bit that carries one letter of one Color cannot be where the chip gets any other part of any Color. Which pixels read it is not stated, so no pixel is required to change.

The report lists, for every OBSERVED instance, the triples of the pixels that changed. No expectation is attached to them.

**5. Preconditions.**
- C1–C9.
- The frame's Q0 lines pass P1, since they are the baseline.
- The band has at least one graded line.

**6. Contrasts.**
- V4a: in one band of F2 whose bit was reported OBSERVED, also invert a bit that carries a letter of another Color and was OBSERVED in its own band. The pages predict a change on pixels of that other value, so P4 fails for the first band.
- V4b: run F2 with BPLCON0 = $A000 (hires). Inverting bit 09 of COLOR01, which carries `a` of Color 0, then changes R1 of value-1 pixels (p.94, p.273), so P4.01.09 fails. Preconditions hold in hires form.
- V4c, the null case: a band that restores Q0 without a new inversion is predicted to change nothing. It must come out not OBSERVED.

**7. Reference frame.** R2, R3, and R5 for band boundaries.

### P5: the bitplane colours use more of the lower registers than the four a normal display uses (1 proposition)

**1. Proposition.** In SuperHires mode, some visible playfield pixel depends on a register outside COLOR00–COLOR03. In Hires, with the same two bitplanes, none does.

Claims: APPC-shr.c6 (bitplane part), CH4-appC-context.c7, APPC-shr.c8. Pages 301, 94.

**2. Stimulus.** The perturbation bands of F2–F4 with n = 04…0F, and F5's inversion bands.

**3. Observation.** Graded pixels of those bands against their baseline, as in P4. In F5 the baseline is register v[i] of Q0 in all twelve bits.

**4. Expected relation.**
- SuperHires: at least one of the 144 bands with n = 04…0F changes at least one graded pixel. Any change counts, whether P4 grades it PASS or FAIL.
- Hires: the bands inverting n = 04…0F change no graded pixel. The bands inverting n = 00…03 invert all twelve bits of exactly the pixels with v[i] = n.

The sources:
- "In order to decode sprite and bitplane color information in SuperHires mode, certain multiplexing occurs in the use of the registers. Instead of 4 bits of red, green, and blue for bitplane registers 0-3 stored as 0x0RGB in four color registers, SuperHires bitplane colors are specially encoded in the sixteen lower color registers" (p.301).
- Table 3-18 (p.94): two bitplanes select registers 0–3.
- p.297: "There are new color register interpretations as part of the SuperHires mode."

The quantifier: "certain multiplexing occurs" is existential. The report lists, for each register 04–0F, whether any of its bits changed a pixel. No register is required individually (L1).

**5. Preconditions.** C1–C9, in hires form for F5.

**6. Contrast (V5).** Run F2–F4 with BPLCON0 = $A000. Table 3-18 predicts that no register 04–0F changes a pixel, so the SuperHires half fails. F5 carries this case in the plan.

**7. Reference frame.** As P4.

### P6: SuperHires sprite colours are encoded in the upper sixteen registers (2 propositions)

**1. Propositions.**
- **P6.1** (APPC-shr.c9, p.302): In SuperHires mode, with COLOR16–COLOR31 written as the p.302 table prescribes for Colors 16–19, the inner pixels of a sprite-0 line of value v show the encoded letters of Color 16+v in bits 3–2 of R, G and B. They do not depend on COLOR00–COLOR15.
- **P6.2** (APPC-shr.c6 sprite part, CH4-appC-context.c6, p.301): In SuperHires mode, some inner sprite-0 pixel depends on an upper register other than COLOR17–19. In Hires none does.

**2. Stimulus.** F7–F10: sprite 0 over a value-0 SuperHires playfield from line 30, with BPLCON2 = $0024 and SPREN set. Each block of three lines carries values 1, 2 and 3. F7 has T0 above and Q0 below, with inversions of COLOR16 and 20–31. F8 has T0c above. F9 has Q1 below. F10 is Hires.

**3. Observation.**
- Sprite lines are the source lines on which the port shows sprite 0 fetching a data pair. The value v of the line is taken from that pair (p.107: "the high order word … contribute[s] the leftmost digit").
- The *run* on a sprite line is the maximal set of consecutive visible pixels whose bits 3–2 differ from Q0's Color 0 in at least one component. It is found on F7's baseline lines.
- The run must have the same ends [xa, xb] on all of them, and a length of 32 ± 1 hires periods (C11).
- The *inner pixels* are xa+1 … xb−1. They are graded at the same positions in F8 and F9. F10 uses its own run (item 7).
- The pixels at xa and xb are not graded, because the pages do not say how a SuperHires pixel pair made of one sprite and one playfield pixel is coloured.

**4. Expected values.**
- **P6.1**
  - F7 baseline blocks: bits 3–2 of R, G and B equal the letters of Color 16+v under T0, which is (v, v+1, v+2) mod 4. For example, v = 1 gives Color 17, `GH-- IJ-- KL--`, equal to 1, 2, 3.
  - F8: the same under T0c.
  - F9: all twelve bits of every inner pixel equal those of the same position and value in F7's baseline blocks, although COLOR00–0F changed from Q0 to Q1.
  - F7: no upper-register inversion changes any graded pixel outside the run.
  - Sources: "SuperHires sprites are encoded in the upper sixteen color registers using a similar scheme:" and the key and table that follow (p.302); "SuperHires bitplane colors are specially encoded in the sixteen lower color registers" (p.301).
- **P6.2**
  - F7: at least one of the thirteen inversion blocks (16, 20–31) changes at least one inner pixel.
  - F10: none of those thirteen changes any inner pixel. The blocks inverting 17, 18 and 19 invert all twelve bits of the inner pixels of the lines with v = 1, 2 and 3 respectively. Baseline inner pixels show all twelve bits of register 16+v.
  - Sources: the multiplexing sentence (p.301); "Sprite channel 0, for example, takes its colors from registers 17 - 19" (p.108, Table 4-2); "The color value of the first register in each group of four registers is ignored by sprites" (p.103).
  - The quantifier is existential, as in P5.

**5. Preconditions.**
- C1, C2, C4, and C5 on lines 0–29.
- C7, including the block changes, which follow WAIT (line, $00).
- **C10:** in each of F7–F10, the port shows sprite 0 fetching its two control words and 60 data pairs, and sprites 1–7 fetching only $0000 control words.
- **C11:** the run is found and is 32 ± 1 hires periods long. p.101 says sprites are 16 pixels wide and "changing the resolution or interlace mode of the playfield has no effect on the size or resolution of a sprite". A lores pixel is two hires periods.

Contrasts for C10 and C11: point SPR0PT at the null sprite. No sprite data is fetched and no run is found.

**6. Contrasts.**
- V6a: in F7, write COLOR10–1F = T0c instead of T0 and keep F7's expectation (T0). The pages predict T0c's letters, so P6.1 fails. The run is still found, because each T0c Color 17–19 differs from Q0's Color 0 in at least one component.
- V6b: in F9, also write COLOR10–1F = T0c. The pages predict that the sprite changes, so "unchanged by the lower sixteen" fails.
- V6c: run F7 with BPLCON0 = $A000. p.108 and p.103 predict that none of the thirteen inversions changes sprite 0, so P6.2's SuperHires half fails. F10 is this case in the plan.

**7. Reference frame.** R1, R5, and sprite lines identified at the port. The run's ends in F7 anchor the inner positions for F8 and F9. In F8 the run found the same way must have the same ends. In F10 (Hires) the run is found anew on its baseline lines with the same rule, and must meet C11. Hires COLOR00 = $05A has the same bits 3–2 as Q0's Color 0, so the rule applies unchanged.

---

## 8. Claims and propositions

| Claims | Proposition(s) |
|---|---|
| APPC-bplkeykey.c0r … c3b (12) | P1.cX, one each. The `--` half goes to P2.X. |
| APPC-bpltab.r00b00 … r0Fb11 (192) | P4.n.b, one each |
| APPC-shr.c1 | P3.H, P3.L |
| CH4-appC-context.c1 | P3.H |
| CH4-appC-context.c2 | P3.L |
| APPC-shr.c3, CH4-appC-context.c3 | P2.R |
| APPC-shr.c4, CH4-appC-context.c4 | P2.G |
| APPC-shr.c5, CH4-appC-context.c5 | P2.B |
| APPC-shr.c6 | P5 (bitplanes), P6.2 (sprites) |
| CH4-appC-context.c6 | P6.2 |
| CH4-appC-context.c7 | P5 |
| APPC-shr.c8 | P1 (all), P5 |
| APPC-shr.c9 | P6.1 |

That makes 212 propositions: 12 + 3 + 2 + 192 + 1 + 2.

---

## 9. What the pages leave open, and where they read differently

- **Which register and which half a pixel is drawn from.** The table says where each letter is stored, never when the chip reads it. No test may assume it (L1, P4). The table's layout, with each row holding one Color in its upper halves and one in its lower halves, suggests that two neighbouring pixels select a register together. The pages do not say so. Nothing here depends on it.
- **The lower two output bits.** The key writes `ab--`, and p.302 speaks of "the loss of lower bits of precision". No page says what the output shows there (L2).
- **"per hires pixel".** p.301 says "only two bits of red, green and blue color resolution per hires pixel" in a paragraph about SuperHires pixels, which are half a hires pixel. P2 reads it as per displayed pixel. Under the literal reading, per hires period, the test's output shows one value per hires period anyway, so P2's check is the same either way.
- **p.273 (Appendix A) against p.301.** p.273 says of COLORxx: "One of these registers at a time is selected (by the BPLxDAT serialized video code) for presentation at the RGB video output pins". Read into SuperHires, that would mean register c shown as 0x0RGB, which p.301 replaces ("Instead of 4 bits … stored as 0x0RGB in four color registers"). p.297 states that SuperHires brings "new color register interpretations". The specification treats Appendix A as describing the other modes and specifies SuperHires from Appendix C. The hires contrasts (V2, V4b, V5, V6c) use p.273 and p.94 only in Hires, where both pages agree.
- **One bitplane.** p.300 allows a SuperHires depth of "1 or 2". The key and table cover four Colors. No page says which registers a one-bitplane SuperHires display uses. Only two bitplanes are used here.
- **HIRES and SHRES both set.** p.300 says only "Set it to zero if SHRES enabled". The result of setting both is not stated and is not tested.
- **Data fetch in SuperHires.** DDFSTRT and DDFSTOP values are given for Lores and Hires only (p.60). p.91 says bit H3 is "only respected in Hires Mode" and is silent on SuperHires. The test therefore derives every position from what the port fetched and the output showed (R2, R3), not from the fetch registers.
- **Sprites.** The sprite key names Color 16, but p.103 says the first register of each group is never shown. The pages do not say how sprites 2–7, or attached sprites, are coloured in SuperHires, nor how a hires period made of one sprite and one playfield pixel is coloured (L3; P6 grades inner pixels only).
- **HAM and dual playfield.** p.300 marks HAM "Incompatible" and DPF "Compatible" with SuperHires, but gives no encoding for either. Neither is used.
