# SuperHires colour encoding in the Denise colour registers — test specification

This specification was written from the package's claims (`claims-spec-stage.jsonl`, 218 claims) and the printed pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. It says what a 68000 program must set up, what the test must observe at the Chip-memory port and at the video output, which value or relation the pages predict, what must be true first, and which change of the stimulus must turn each check red. Where the pages give no value, none is specified.

All 218 claims sit on printed page 301 (217 claims) and page 302 (1 claim), Appendix C, section "SuperHires Mode and the Denise Color Registers". The key and all sixteen rows of both tables were read on the rendered printed pages 301 and 302 as well as in the text; they agree.

---

## 1. Claims not turned into a proposition

None. Every one of the 218 claims is the basis of at least one proposition below. Three limits set how far some of them can be graded. Each limit is repeated where it applies.

- **L1: the 192 table claims (`APPC-bpltab.*`) can come out "not observable".** Each one gets its own proposition (P4). But the pages never say which of the sixteen registers, or which half of a register, the chip reads for a given pixel. And the test reads one output sample per hires period (R0), so every second SuperHires pixel is hidden from it. So a register bit whose half never drives a visible pixel cannot be seen by any test with this output. P4 reports such a claim as NOT OBSERVABLE, which is neither a pass nor a failure. How many claims end up that way depends on the chip, not on the pages. For example, if the chip took the two pixels of one hires period from the two halves of one register, which the table's layout suggests but no page states, only one half of each register could ever reach a visible pixel.
- **L2: the dashes in the key give no expected value.** The key writes each component as `ab--`, with two encoded bits and two positions marked `-`. The pages do not say what the video output shows in those two lower bit positions. The test checks only that they carry no information beyond the two encoded bits (P2). It never checks them against a value. The dashes state that those two bits are not encoded: no register bit stores them, since every bit 11–0 of COLOR00–0F carries one of the letters a–x, which P4 grades bit by bit and P1K grades per Color and component.
- **L3: `APPC-shr.c9` (sprites) is checked for sprite 0 only.** The sprite key on p.302 names Colors 16–19. Chapter 4 assigns 17–19 to sprites 0 and 1 and says the first register of each group is never shown (p.103). So Color 16, with letters A–F, cannot appear on screen. No page says how sprites 2–7 are coloured in SuperHires mode. Nothing is specified for them.

---

## 2. Pages this specification rests on

| Printed page | What is used |
|---|---|
| 301 | The claims: the SuperHires paragraphs, the bitplane key and the sixteen-row bitplane table |
| 302 | The sprite claim, the sprite key and table, and "loss of lower bits of precision" |
| 296, 297 | SuperHires is provided by the enhanced Denise. "There are new color register interpretations as part of the SuperHires mode." |
| 299 | VPOSR chip identification ("A value of 20 or 30 indicates that the enhanced Hires Agnus is present") and DENISEID ($FC) |
| 300 | The 35 ns pixel rate, 2-bitplane maximum, and BPLCON0 bits HIRES, BPU, SHRES |
| 303 | SPRxCTL bits SHSH1/SHSH0 (left 0) |
| 23, 194 | A Copper horizontal position of two colour clocks "corresponds to 4 pixels in low resolution and 8 pixels in high resolution"; a memory cycle (colour clock) is about 280 ns |
| 29, 30 | Copper list example (MOVE, WAIT, colour changes mid-display), and the Copper start sequence |
| 223, 337, 339 | After reset a copy of the ROM responds from $00000000 and "Software must assume that memory is not available"; "The OVL bit in one of the 8520 Chips disables the overlay": CIA-A PRA ($BFE001) bit 0, DDRA ($BFE201) "set to 0x03" |
| 272, 285, 309 | BPLxPTH and SPRxPTH hold the "high 3 bits" of an 18-bit pointer; the ECS list of pointers with two additional bits names DSKPTH, BLTxPTH, COP1LCH, COP2LCH and AUDxLCH only |
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

**SuperHires pixel, hires period, visible pixel.** "SuperHires mode provides a 35ns pixel display rate - twice the horizontal resolution of Hires mode, and four times the Lores rates" (p.300). A hires period is the time of one hires pixel, which is two SuperHires pixels. The test reads one output sample per hires period (R0), so at most one of the two SuperHires pixels of a period is visible to it. A *visible pixel* is that sample, at position x on an output line, counted in hires periods from that line's horizontal sync (R0).

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

**Palettes.** Each palette gives the three two-bit components of Colors 0–3, followed by the sixteen register values that result from the formula. Q0 makes all four Colors differ in every component. Q1–Q5 set letter number l (a = 0 … x = 23) to bit j−1 of (l + 1), so any two letters differ in some palette. Z and O set every letter to 0 and to 1. Across Q0–Q5, Z and O, every component of every Color takes all four values, and every letter is both 0 and 1. S0–S3 are Q0 with Color 1 set to (s, s, s).

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

The sprite palettes are T0, where Color 16+v = (v, v+1, v+2) mod 4, and its complement T0c. They are written to COLOR16–31 (rows 10–1F):

| Palette | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1A | 1B | 1C | 1D | 1E | 1F |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| T0 | 005A | 049E | 08D2 | 0C16 | 016B | 05AF | 09E3 | 0D27 | 0278 | 06BC | 0AF0 | 0E34 | 0349 | 078D | 0BC1 | 0F05 |
| T0c | 0FA5 | 0B61 | 072D | 03E9 | 0E94 | 0A50 | 061C | 02D8 | 0D87 | 0943 | 050F | 01CB | 0CB6 | 0872 | 043E | 00FA |

---

## 4. Setup common to all runs

### 4.1 System

- Enhanced Chip Set and 2 MB Chip memory, the harness's default `Config` (`nanomig_harness.h`: `chipset_config` bit 3 selects ECS, `memory_config` 2.0 MB).
- The program uses only 68000 instructions and no timing loops. The Copper paces every frame. The program runs unchanged on both CPU cores, each separately, with the second core in its default 68000 behaviour. The claims say nothing about the CPU, so no 68010 or 68020 setting is required.
- Bitplane buffers, sprite structures and Copper lists lie below $80000 (§4.2). BPLxPTH and SPRxPTH hold only the "high 3 bits" of an 18-bit word pointer (p.272, p.285), which reaches byte addresses up to $7FFFE; p.309 extends the Copper, blitter, disk and audio pointers under ECS, not these. The same placement lets the OCS contrast (C1) run the same program. That range is covered by the ROM overlay after reset (p.223), so the program clears the overlay before it writes there (§4.5).
- The program, its stack and its results stay where the harness puts and reads them (`nanomig.i`): the program at `PROGRAM_ORG` ($080400), the stack below `PROGRAM_STACK` ($0FFF00), the results from `RESULTS` ($0C0000), and the completion mark `DONE_VALUE` at `DONE_ADDR` ($0C0FFE). The harness reads stored words only in the identity-mapped range $080000–$1FFFFF (`peek_logical()`).
- The claims name no video standard, and the harness `Config` names no video-standard setting. The test runs in the standard the configuration gives and reports it from VPOSR (p.299: "20 for PAL, 30 for NTSC"). The geometry below is valid in both.

### 4.2 Data in Chip memory

- **Pixel stream.** A plane-1 buffer and a plane-2 buffer, each 32768 words. That is enough for 200 lines at any fetch width the data-fetch registers allow.
  - Words 0–255 of both buffers are $FFFF. This is the *R section*, where every pixel has value 3.
  - Words 256–32767 are the *pattern section*. Pixel values v[0], v[1], … come from a 32-bit xorshift generator: `x ^= x<<13; x ^= x>>17; x ^= x<<5` (all modulo 2^32), with seed x = $2545F491. After each step the pixel value is `x >> 30`.
  - Pattern pixel j of pattern word k goes to bit 15−j of that word: plane 1 takes bit 0 of the value and plane 2 takes bit 1.
  - The first pattern words are plane 1 $AC41 $C956 $EBAD $15F3 and plane 2 $CE9D $9A90 $89DC $A548, so the generator can be checked against them.
  - Nothing in a check depends on these values. The expected values are always taken from the words the port shows were fetched (§5, R2). The stream exists to cover every neighbour context (C8).
  - The stream takes 520,192 generator steps (32512 pattern words, 16 pixels each). Its words may be computed when the program is assembled, carried in the program image, and copied into place by the program.
- **Zero buffer.** 32768 words of $0000. Both plane pointers may point at it.
- **Sprite 0 structure** (p.105–108). The control words are SPR0POS = $5060 and SPR0CTL = $8C00. That gives VSTART $50, VSTOP $8C, lores HSTART $C0, H0 = 0, and SHSH1 = SHSH0 = 0 (p.303).
  - 60 data line pairs follow, each written as low word then high word. Line j with j mod 3 = 0 has value 1 ($FFFF, $0000). j mod 3 = 1 has value 2 ($0000, $FFFF). j mod 3 = 2 has value 3 ($FFFF, $FFFF).
  - The structure ends with $0000, $0000.
- **Null sprite.** $0000, $0000.
- **Copper lists** F0–F11 (§4.4). A contrast variant of a list takes that list's place.

The addresses are fixed as follows. Everything the chips fetch lies in $010000–$04CFFF, below $80000 and clear of the 68000 exception vectors at $000000–$0003FF. Nothing overlaps.

| Item | Addresses |
|---|---|
| Plane-1 stream buffer (32768 words) | $010000–$01FFFF |
| Plane-2 stream buffer (32768 words) | $020000–$02FFFF |
| Zero buffer (32768 words) | $030000–$03FFFF |
| Sprite 0 structure (124 words) | $040000–$0400F7 |
| Null sprite (2 words) | $040100–$040103 |
| Copper list Ff, f = 0…11 (at most 4 KB each) | $041000 + f·$1000, so F11 at $04C000–$04CFFF |
| Program (`PROGRAM_ORG`) | from $080400 |
| Stored fixture, VPOSR, DENISEID | `RESULTS` ($0C0000), `RESULTS`+2, `RESULTS`+4 |
| Completion mark (`DONE`) | `DONE_ADDR` ($0C0FFE) |
| Stack | below `PROGRAM_STACK` ($0FFF00) |

### 4.3 Register values written by each frame's Copper header

Every list begins, before its first WAIT, with MOVEs that set:

| Register | Value |
|---|---|
| BPLCON0 ($100) | $2040 in SuperHires frames: BPU = 010, HIRES = 0, SHRES = 1, HAM = 0, DPF = 0, LACE = 0 (p.300: "HIRES … Set it to zero if SHRES enabled", "BPU … Depth of SuperHires mode (1 or 2)", "SHRES … SuperHires 35ns pixel enable bit"). $A000 in hires frames. $2000 in the lores frame. COLOR (bit 9) = 0; p.63: "RGB video is correctly generated regardless". |
| BPLCON1 ($102) | $0000 |
| BPLCON2 ($104) | $0024: PF2P = PF1P = 100, so sprites are in front of both playfield codes (p.210–211) |
| DIWSTRT ($08E) | $2C81 |
| DIWSTOP ($090) | $F4A1. The window covers lines $2C–$F3 (200 lines) and lores positions $81–$1A0 (288 lores pixels = 576 hires periods). |
| DDFSTRT ($092), DDFSTOP ($094) | $003C, $00D4, the hires normal values (p.60). The pages give no SuperHires values. |
| BPL1MOD ($108), BPL2MOD ($10A) | $0000, so each line continues where the previous one stopped (p.61) |
| BPL1PTH/L ($0E0/$0E2), BPL2PTH/L ($0E4/$0E6) | the plane-1 and plane-2 stream buffers |
| SPR0PT–SPR7PT ($120–$13E) | null sprite. In F7–F10, SPR0PT points to the sprite 0 structure. |
| COLOR00–COLOR0F | the frame's first palette (§4.6) |
| COLOR10–COLOR1F | T0, except T0c in F8 |

### 4.4 Frames and Copper lists

There is one Copper list per frame. Window lines are numbered y = 0…199 from beam line $2C.

- A band change is a group of MOVEs placed directly after `WAIT (beam line $2C+y, horizontal $00)`. Its first word is ((($2C+y) << 8) | $01) and its second is $FFFE (p.30–31). Colour register MOVEs appear nowhere else.
- Each list ends with MOVEs to COP1LCH/COP1LCL that point at the next frame's list, then `WAIT $FFFF,$FFFE` (p.30). COP1LC "is automatically used at the beginning of each vertical blank time" (p.276).
- F10's list points at F11's, and F11's list points at itself.
- F0 is a lead-in copy of F1 and is never graded.

A band change that writes a whole palette (sixteen MOVEs, four memory cycles each, p.275) runs past the start of its line's data fetch. The line it starts on is a *spacer line*: its display may be disturbed and it is never graded. The spacer lines are named in the frame plan, and the graded-line rule of §4.7 excludes them by itself. A band change of at most four MOVEs is planned to end before the line's first bitplane fetch, so its line is graded. C7 checks this at the port: it takes a band of more than four MOVEs for a spacer line and does not time its MOVEs against its own line. The null band of V4c is the one line graded whatever its size: V4c's contrast gives that band more than four MOVEs on purpose (§7, P4), and V4c grades its lines all the same.

### 4.5 The 68000 program

1. Store the fixture: write FIXTUREVAL ($BEEF) to `RESULTS`. It is read back before anything else is graded, so a run that never reaches this point is told apart from one that reaches it and writes the wrong value.
2. Write DMACON ($096) = $7FFF (all DMA off).
3. Read VPOSR ($004) and DENISEID ($07C) and store them at `RESULTS`+2 and `RESULTS`+4.
4. Clear the ROM overlay before any write below $80000 (p.223): write CIA-A DDRA ($BFE201) = $03 ("set to 0x03", p.339), then PRA ($BFE001) = $00, which makes OVL (PRA bit 0, the "ROM/RAM overlay bit", p.337) an output at 0. That the writes below $80000 landed is shown at the port by C3 (fetched data equal to what the program wrote).
5. Build every buffer and Copper list of §4.2 and §4.4 at the addresses of §4.2.
6. Start the Copper exactly as p.30 prescribes: "you must initialize one of the Copper location registers (COP1LC or COP2LC) and write to its strobe address before Copper DMA is turned on". Write COP1LC (longword to $080) = address of F0, strobe COPJMP1 ($088), then write DMACON = $83A0 (SET, DMAEN, BPLEN, COPEN, SPREN; p.278).
7. Wait until at least 13 vertical blanks have passed. Poll VPOSR/VHPOSR for the wrap of the vertical position. The CPU must not write any buffer or list while it waits. The count is 13 and not 12 because F0 runs twice when the Copper is started in the last lines of a frame (a WAIT compares eight bits of the vertical position): F1 then starts at the second vertical blank, F11 at the twelfth, and the thirteenth ends F11's frame.
8. End with `DONE` (`nanomig.i`), which writes `DONE_VALUE` to `DONE_ADDR`.

### 4.6 Frame plan

| Frame | Mode | Content (window lines) |
|---|---|---|
| F0 | SuperHires | Copy of F1. Lead-in, never graded. |
| F1 | SuperHires | Lines 0–23: Q0. Then eleven bands, each one spacer line plus 12 lines, in the order Q1, Q2, Q3, Q4, Q5, Z, O, S0, S1, S2, S3 (lines 24–166). Then a spacer line (167) and Q0 on lines 168–199. |
| F2, F3, F4 | SuperHires | Lines 0–23: Q0. Then perturbation bands of 2 lines each: band k changes one bit of one register of Q0, with the band change restoring the previous band's register and changing the next. F2 and F3 carry 80 bands each (lines 24–183) and F4 carries 32 (lines 24–87). Together they perturb each of the 192 bits (n = 00…0F, b = 00…11) exactly once. Then a null band of two lines that only restores the last band's register (see V4c): in F2 and F3 it is lines 184–185, in F4 it is lines 90–91 after a restoring band on lines 88–89. Q0 fills the rest of the frame: lines 186–199 of F2 and F3, lines 92–199 of F4. |
| F5 | Hires ($A000) | Lines 0–23: Q0. Then 16 two-line bands with COLOR01 = $0kkk (k = 0…15, all other registers Q0). Then 16 two-line bands, each inverting one register n (Q0[n] XOR $0FFF), n = 00…0F, one band each. Then Q0 on the remaining lines. |
| F6 | Lores ($2000) | Lines 0–23: Q0. Then 16 two-line bands with COLOR01 = $0kkk. Then Q0. |
| F7 | SuperHires | Lines 0–29: Q0 on the stream. At line 30 a band change of four MOVEs reloads BPL1PT/BPL2PT with the zero buffer; line 30 is graded. Sprite 0 lines are beam lines $50–$8B (window lines 36–95), in 20 blocks of three lines (values 1, 2, 3). Blocks 0–1 and 15–19 are the baseline. Blocks 2–14 each invert one register r = COLOR16, 20, 21, …, 31 (XOR $0FFF), restored at the next block. |
| F8 | SuperHires | As F7 without inversions, with COLOR10–1F = T0c |
| F9 | SuperHires | As F7 without inversions, with COLOR00–0F = Q1 from line 30 on. The band change at line 30 carries the palette, so line 30 is a spacer. |
| F10 | Hires ($A000) | As F7, with blocks 2–14 inverting 16, 20–31, blocks 15–17 inverting 17, 18 and 19, and blocks 18–19 as baseline |
| F11 | SuperHires | Lines 0–23: Q0. Then sixteen sweep bands j = 0…15, each one spacer line plus two lines (band j starts on line 24+3j): the band change writes the twelve registers that hold a letter of Color 0 or of Color 1 (n = 00…09, 0C and 0D, §3's formula), giving Color 1 the letters (a, a, a) and Color 0 the letters (b, b, b) with j = 4a + b, and Colors 2 and 3 Q0's. A closing band (a spacer line, line 72) restores those registers, and Q0 fills lines 73–199. In every component COLOR01 then holds the nibble 4a + b (bits 11–10 are Color 1's letters, bits 9–8 Color 0's, §3), so a display that showed 4-bit levels at value-1 pixels would show sixteen values of each component over the sweep, and one with a coarser granularity (p.301) fewer. The sweep is the SuperHires population of P3 beside F1. |

Contrast variants (§6, §7) change one item of this plan and nothing else.

### 4.7 Graded lines and graded pixels

A *source line* is one beam line's burst of bitplane fetches at the Chip-memory port. Source line y of frame f is *graded* when all of the following hold:

- (a) f's Copper fetches come from f's list (C4).
- (b) Line y fetched the same number F of words from each plane buffer, from consecutive addresses, and F is the same for every line of the frame (C3).
- (c) No Copper MOVE to a colour register, to BPLCON0–2, or to a bitplane pointer is fetched between line y's first and last bitplane fetch (C7).

The palette in effect on a graded line is the frame plan's palette for its band. Its values are also rebuilt from the Copper MOVEs fetched before the line's first bitplane fetch: IR1 is the register address and IR2 the data (p.275). The rebuilt values are reported next to every result (one MEASURED line gives, per frame, how many bands' rebuilt COLOR00–0F equal the plan). A difference from the plan is reported, and is not a precondition. It does decide one thing: P5.L counts a perturbation band only when its rebuilt palette is Q0 with the planned register's planned bit inverted and nothing else (§7, P5).

Lines 0–29 of F7–F9 are graded lines in the sense of this section when their clauses hold: (a) C4-F7F10, (b) C3-F7F9 (the clauses of C3 on each of the 30 lines of each of the three frames, and the same count of words on all of them) and (c) C7-F7F10, with C5-F7F10 fixing their pixel mapping. P1 and P2 read them (§7).

A *graded pixel* is a visible pixel x on an output copy of a graded line, with x inside the check interval (R3). If the line doubler shows a source line more than once, every copy is graded and the copies must agree.

---

## 5. Reference frames and how the test checks their anchors

**R0: output samples to visible pixels.** The pages fix pixel times against each other, not against `clk_sys`: a SuperHires pixel is 35 ns, half a hires pixel (p.300), and a colour clock of about 280 ns is four hires pixels (p.23, p.194). How many output samples one hires pixel lasts is measured:

- On the graded lines of F5's top Q0 lines, a hires frame in which each fetched bit is one pixel (p.272; Table 3-18, p.94) and Q0's COLOR00–03 all differ, every run of constant r, g, b between two changes is a whole multiple of one sample count w, and the shortest of them is w samples long. Only runs strictly inside the stretch of the line where the colour keeps changing are used; runs that touch blanking or the border are not.
- Their boundaries fall on samples t_hs + φ + k·w, where t_hs is the output line's hs rising edge and φ (0 ≤ φ < w) is one phase for every line.
- Visible pixel x of any output line is the sample t_hs + φ + x·w. The same w and φ serve every frame, because a hires period lasts the same time in every mode (p.300).

Check: w and φ exist and are unique. Contrast (R0): the pattern section of both stream buffers repeats one period of 40 words (640 pixels) made of runs of 4 or 6 pixels of one value, the value changing at every run boundary. Every boundary is a colour change on every pattern line, and the run lengths (4, 6 and 8 samples, 8 where the period repeats) fit more than one sample width (one sample per pixel and two alike), so w and φ are not unique; the measurement, which takes the shortest run as one period, finds runs that are not a multiple of it and reports none.

**R1: source line to output lines.** Output lines are delimited by rising edges of horizontal sync and frames by rising edges of vertical sync, as `find_lines()` and `find_frames()` in `nanomig_harness.h` split them. The harness notes there that its scan doubler shows each source line as two output lines; the test does not assume the number. A source line is shown on the output lines whose samples overlap the cycles of its burst of bitplane fetches: the test takes the last of the output lines grouped under line k as the first copy of line k and the first of those grouped under line k+1 as its second copy (§10.2), and does not compute a line multiplier or an offset. Check: every graded pixel of the two copies agrees (§4.7); this is the precondition R1, printed once for F1–F6 and F11 and once for F7–F10.

**R2: pixel index.** On a graded line with F fetched word pairs, pixel i (0 ≤ i < 16F) is bit 15 − (i mod 16) of the ⌊i/16⌋-th fetched word of each plane.

- The MSB is first: "The MSB is output first, and is, therefore, always on the left" (p.272).
- Its value v[i] is taken from those fetched words as the port shows them.
- Output pixel x shows pixel i as follows:
  - SuperHires frames: i = 2x − s (two SuperHires pixels per hires period, p.300)
  - hires frames: i = x − s
  - lores frame: i = ⌊(x − s)/2⌋

s is a constant per frame. It is found on the frame's top Q0 lines (lines 0–23; in F7–F10, lines 0–29) as the single s for which, over every graded pixel of those lines, the shown value is a function of v[i], one value per pixel value:

- SuperHires frames: bits 3 and 2 of R, G and B. The pages predict Q0's letters of Color v[i] there (P1), and Q0's four Colors differ in every component (§3).
- hires and lores frames: all twelve bits. The pages predict register v[i] of Q0 (p.94, p.273), and Q0's COLOR00–03 all differ.

No expected colour enters the fit. A wrong letter or register therefore fails only the proposition that grades it, on lines whose pixel positions are still known.

Check: exactly one s fits, and in F1–F6 and F11 the same s fits the bottom Q0 lines. In a SuperHires frame, if no s fits i = 2x − s but one fits the hires mapping i = x − s, the frame did not run in SuperHires mode and C5 fails. Otherwise, if none fits, or more than one fits because all four values are shown alike, the pages' prediction of four different appearances fails: that is a failure of P1 (SuperHires) or of the hires/lores baseline on those lines, and nothing else is graded in that frame. In SuperHires frames, s mod 2 tells which SuperHires pixel of a hires period the output shows. The test reports it and no proposition depends on its value.

**R3: check interval.** On every pure R line of the frame, the output shows a single run [xL, xR] of visible pixels with one constant value, bounded on both sides by pixels of one and the same other value, the border colour. The border shows left and right of the display window; blanking lies beyond it and differs from it, so the border stretch outside the window is not bounded alike on both sides and is not the run. The value compared is bits 3–2 of R, G and B in SuperHires frames and all twelve bits in hires and lores frames. Every pixel fetched on a pure R line has value 3 (C3 shows the words), so the pages predict one appearance along the run. Which colour it is does not enter the anchor: in SuperHires frames P1 grades it on the same lines (Q0's Color 3, letters 3, 0, 1), and in hires and lores frames the baseline expects $C16, the Q0 value of COLOR03. The run is the display window where fetched data is shown. The check interval is x from xL + 8 to xR − 8 that also satisfies 16 ≤ i(x) ≤ 16F − 17. That excludes one data word at each end of the fetch and 8 visible pixels at each edge of the run. Check: C6.

**R4: letters to output bits.** A component's upper letter is output bit 3 and its lower letter is output bit 2 (§3). Bits 1–0 carry no letter (L2).

**R5: frame and band boundaries.** These come from the port. The Copper fetch addresses identify the list, and hence the frame. The position of each band-change MOVE among the bitplane fetch bursts identifies the line at which a band begins.

---

## 6. Preconditions common to the propositions

Each precondition is asserted before any proposition is graded. It is followed by the change that must make it fail while the program still finishes.

A precondition is graded only when everything it depends on holds and its population is not empty. Otherwise it is printed as NOT GRADED, without a verdict, and names what did not hold. C3-F7F9, C5-F7F10, C7-F7F10, C10 and R1-F7F10 depend, like C3 and C7 of the other frames, on F7–F10 having run their lists. The dependencies are these. C1 gates every line after it. R0 gates every line that reads a visible pixel (C5/R2, C6, C8, R1, P2-DIVERSITY, and C5-F7F10 and R1-F7F10 of F7–F10). C6 of every graded frame gates R1, which reads its check interval. A frame whose list did not run in order (the order clause of C4) gates every other line that reads that frame. C6 and C3 of a frame gate its pixel mapping (C5/R2), and C3-F7F9 gates C5-F7F10 in the same way. The mappings of all graded frames gate R2's bottom lines; those of F1–F4 and F11 gate C8's pair coverage; those of F1–F4, with C7 of F1–F4 (the band changes that carry the palettes it names), gate P2-DIVERSITY; R2's bottom line of F1 gates C8's diversity of F1. C4's header values gate nothing but the propositions that read the frame (§7): a contrast that has to change a value C4 compares with the plan (BPLCON0, DIW, SPRxPT) makes C4 fail with it, and its record says so.

| | Precondition | How the test knows | Contrast that must make it fail |
|---|---|---|---|
| C1 | The Enhanced Chip Set is present, because SuperHires "is provided by" the enhanced Denise (p.296) | The stored DENISEID has low byte $FC ("The enhanced HighRes Denise (8373) will return $FC in the lower 8 bits", p.299). The stored VPOSR bits 14–8 equal $20 or $30 ("A value of 20 or 30 indicates that the enhanced Hires Agnus is present", p.299); the test reports which ("20 for PAL, 30 for NTSC"). | Select OCS. p.299: the original Denise returns "whatever value is left over on the bus", and the original Agnus reports $00 (PAL) or $10 (NTSC). |
| C2 | The program ran to its end | `DONE_VALUE` is written to `DONE_ADDR` (`DONE`, `nanomig.i`) | A program that never writes DONE: the run ends at the cycle budget, C2 fails, and nothing further is graded. |
| C3 | Bitplane DMA fetched two planes on every graded line | At the port, each graded line has F ≥ 16 fetches from each plane buffer, the same count per plane, consecutive addresses inside the buffers for each plane, data equal to what the program wrote, and no other bitplane address: inside the burst of the line's own bitplane fetches every read of Chip memory below $80000 is a word of a plane buffer, of a Copper list or of the null sprite, or the word 0 that the port shows four times in every line (the memory-refresh slots; C9 counts them) | BPLCON0 = $1040 (one plane): only plane-1 addresses are fetched; C4 fails with it, the header carries the changed BPLCON0. BPLCON0 = $B000 (three bitplanes): plane 3 is fetched from the unprogrammed BPL3PT; C4 fails with it for the same reason, and C9 fails with it, since a read outside the buffers is a sprite DMA read by C9's definition. One word of plane 1's pattern differs from §4.2's (C3D): the data clause fails; C3-F7F9 fails with it, lines 0–29 of F7–F9 reading the same word. |
| C4 | Each frame ran its own list with the planned header | The Copper fetch addresses per frame follow F0→F1→…→F11 (the test prints the order of F0–F6 as C4, that of F7–F10 as C4-F7F10 and that of F11 as C4-F11). The header MOVEs (BPLCON0–2, DIW, DDF, modulos, bitplane pointers, SPR0PT–SPR7PT) carry the §4.3 data and are fetched before the frame's first bitplane fetch. In F1–F6 SPR0PT–SPR7PT all hold the null sprite. | Make F5's list point at itself instead of F6: the frame order fails (C4). Make F7's list point at itself (C4C) or F10's (C4D): the order of F7–F10 (C4-F7F10) or of F11 (C4-F11) fails. Omit the SPR1PT MOVEs from F2's header: the pointer values fail. |
| C5 | The claims' condition "In SuperHires mode" holds while pixels are counted | In SuperHires frames a unique s fits with the mapping i = 2x − s (R2), on the top and on the bottom Q0 lines. Only a 35 ns pixel rate makes two consecutive fetched pixels per hires period line up with the output. | BPLCON0 = $A000 in F1. No s fits the SuperHires mapping and one fits the hires mapping; C4 fails with it, the header carries the changed BPLCON0. |
| C6 | The check interval exists and lies inside the window | Every pure R line shows a run (R3) with identical ends. Each run's width is at least 256 and at most 576 hires periods, asserted on every line: the window is 288 lores pixels, and a lores pixel is two hires periods (p.57 "resolution … one low resolution pixel", p.300). The check interval is at least 256 wide. | DIWSTRT = $2CC1, DIWSTOP = $F401, a window of 64 lores pixels, in F1 (C6) or in F2–F6 and F11 (C6B); C4 fails with it, the header carries the changed DIW. The run can be at most 128 periods wide ("You cannot display objects outside of the defined display window", p.57). |
| C7 | No colour or control register changes while a graded line is shown | Every Copper MOVE to a colour register, BPLCON0–2 or a bitplane pointer follows a WAIT with horizontal position $00. Each is fetched after the previous line's last bitplane fetch and, except on the planned spacer lines, before its own line's first bitplane fetch. Every line of the frame plan that is not a spacer line is graded. | Put one perturbation band's MOVEs behind `WAIT (line, $80)`. That line is not graded, and C7 reports a planned line that is missing. |
| C8 | Every neighbour context occurs visibly in every band of F1–F4 and F11 | On the graded pixels of every band (i = 2x − s), each of the 16 ordered pairs (p, q) occurs both as (v[i], v[i+1]) and as (v[i−1], v[i]). So every value is seen as the left and as the right member of every pair, whichever pixel of a hires period the output shows. F1's own Q0 lines show at least half of the 64 neighbour triples (v[i−1], v[i], v[i+1]). | Restrict the pattern section to values 0 and 3 (C8): both lines fail. Restrict it over the words of lines 24–167 only (C8B): the pair coverage fails and F1's triples do not, because F1's Q0 lines lie outside those words. No contrast was tried that fails F1's diversity alone: the words of F1's Q0 lines are also read by F2's and F3's bands 72–79 and by the Q0 lines from which F2–F4 take their P4 baselines. |
| C9 | No sprite pixels in F1–F6 and F11 | In each of F1–F6 and F11 every sprite DMA read is a word of the null sprite (p.291) and returns $0000. The sprite DMA reads of a frame are its reads of Chip memory below $80000 that are neither bitplane words, Copper-list words nor the word 0 of the refresh slots; the frame has such reads and none comes from another address. The reads of word 0 come four at a time: they are grouped by a gap of more than 300 clk_sys cycles, and every group that lies wholly inside the frame's window holds four. | Point SPR0PT at the sprite 0 structure in F1; C4 fails with it, the header holds SPR0PT. Omit the SPR1PT MOVEs from F2's header: sprite 1 then fetches from the address its pointer reached in F1's frame, past the null sprite; C4 fails with it. Without touching a header: write a non-zero control word into the null sprite (C9N), or control words that start a one-line sprite (C9S). C10 fails with these two, because sprites 1–7 of F7–F10 read the same null sprite. |

---

## 7. Propositions

### P1: every visible SuperHires pixel shows its Color's encoded bits (12 propositions)

**1. Proposition.** In SuperHires mode, with COLOR00–COLOR0F written as the p.301 table prescribes for four Colors, every visible pixel of value c shows the two letters of Color c for component X in bits 3 and 2 of X, with the upper letter in bit 3.

| Id | Claim (carried by P1J.cX) | Color | Component | Bit 3, bit 2 |
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

Page 301. The P1.cX lines carry no claim id: each key claim is graded as a whole by the joint P1J.cX of item 8, which carries its id. The sentence of APPC-shr.c8 introduces the key and is the source of the expectation (item 4), but no P1 proposition carries that id. A display that showed register c as 0x0RGB for pixel value c would pass P1: rows 00–03 of the table hold Color c's own letters in bits 11–10, 7–6 and 3–2 (§3), which are bits 3–2 of R, G and B read as 0x0RGB. So "rather than held in four" cannot fail through P1; P5.L grades it, and P7.3 carries the id.

Each key claim also says that the two dashes after its letters mark bits that are not encoded. That means no register bit stores them: every bit 11–0 of COLOR00–0F carries one of the letters a–x (p.301 table), which the 192 P4 propositions grade bit by bit and P1K grades per Color and component. P1 asserts the two letters.

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

**5. Preconditions.** C1–C9 of F1–F4. Lines 0–29 of F7–F9 join the population only when C3-F7F9, C4-F7F10, C5-F7F10 and C7-F7F10 hold (§4.7); otherwise they are left out and a MEASURED line says so.

**6. Contrast (V1, twelve selectors V1C0R … V1C3B, one for each P1.cX).** In F1 the Q3 band inverts the two letters of component X of Color c (each letter l becomes 3 − l) and nothing else: the registers that hold those letters (§3's formula) take the values of Q3 with those two letters inverted, and the expectation of Q3 is kept. The pages predict that value-c pixels show, in bits 3 and 2 of X, the opposite of both letters Q3 gives them, so P1.cX fails there through both of its letters, and with it P1J.cX. Every precondition still holds: the anchors lie on the top Q0 lines and the pure R lines.

**7. Reference frame.** R0 and R1 place output samples and lines, R2 gives i and s, R3 the check interval, R4 the output bits. R0 and R1 carry their own checks; the others are checked by C5, C6 and R2's bottom-line check.

**8. The full key claim (12 propositions, joint with P1K).** Each key claim's own sentence asserts two things at once: the two letters in bits 3-2, graded above by P1.cX, and, through its two dashes, that no register bit stores anything else of that component for that Color (§1 L2). The second part is graded by P1K.cX over the 192 perturbation bands of F2–F4, the bands P4 grades one bit at a time. The joint proposition P1J.cX is P1.cX together with P1K.cX, PASS when both pass, FAIL when both are graded and either fails, not graded when either is not graded: unlike P7, one failing half does not grade P1J, so a claim id stands only on a line where both halves were graded. P1 and P1K carry no claim id; P1J carries it.

| Id | Claim | Joint of |
|---|---|---|
| P1J.0R | APPC-bplkeykey.c0r | P1.0R, P1K.0R |
| P1J.0G | APPC-bplkeykey.c0g | P1.0G, P1K.0G |
| P1J.0B | APPC-bplkeykey.c0b | P1.0B, P1K.0B |
| P1J.1R | APPC-bplkeykey.c1r | P1.1R, P1K.1R |
| P1J.1G | APPC-bplkeykey.c1g | P1.1G, P1K.1G |
| P1J.1B | APPC-bplkeykey.c1b | P1.1B, P1K.1B |
| P1J.2R | APPC-bplkeykey.c2r | P1.2R, P1K.2R |
| P1J.2G | APPC-bplkeykey.c2g | P1.2G, P1K.2G |
| P1J.2B | APPC-bplkeykey.c2b | P1.2B, P1K.2B |
| P1J.3R | APPC-bplkeykey.c3r | P1.3R, P1K.3R |
| P1J.3G | APPC-bplkeykey.c3g | P1.3G, P1K.3G |
| P1J.3B | APPC-bplkeykey.c3b | P1.3B, P1K.3B |

**P1K.cX (12 propositions, no claim id).** In SuperHires mode the two letters of component X of Color c are the only register bits that carry X of Color c. The p.301 table places each of the two letters in eight register positions (§3); these sixteen bits are the *keyed* bits of (c, X), and each of the other 176 of the 192 register bits is *off-key* for (c, X). The pages do not say which register or half a value-c pixel reads, so no register is assumed: the line states what the key and the table say, that X of Color c is held in its two letters and nowhere else. For every band of F2–F4, compared on its graded pixels of value c with the baseline of P4 item 3 (a pixel whose triple has no baseline is excluded):
- **Off-key bands:** none of the four bits of X changes, bits 1–0 included. This is "no other register bit carries that information".
- **Keyed bands:** of bits 3 and 2 of X only the output bit of the inverted letter changes (upper letter: bit 3, lower letter: bit 2). Bits 1–0 may move (L2).
- **Observed:** some keyed band of the upper letter changes bit 3 and some keyed band of the lower letter changes bit 2. A line that has no violation and lacks one of them is NOT OBSERVABLE (L1) and does not grade P1J.
- **Population:** all 192 bands are graded (the preconditions of F2–F4 and V4c hold) and every band shows at least one graded pixel of value c; otherwise the line is not graded.

The source is the key `ab--` and the table (p.301), and "There are only two bits of red, green and blue color resolution per hires pixel" (p.301). The line prints the number of off-key bands that change X (of the 176), the pixels checked, the keyed bands that change the other output bit (of the 16), and whether each letter was observed.

Contrasts. VP1JG (live input for Colors 1–3): each q-half band of COLOR01–03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes X of a value-c pixel and P1K.1X–3X pass. VKEYAR, VKEYAG and VKEYAB (Color 0, one component each): band 79 of F2, which is off-key for Color 0 in all three components, also inverts bit 11 (red), 7 (green) or 3 (blue) of COLOR00, the upper letter of Color 0 in that component, and the closing band restores it; that component of value-0 pixels then changes in an off-key band and P1K.0R, P1K.0G or P1K.0B fails. V4D, one selector for each of the 24 p-half bits (V4DN0B2 … V4DN3B11): the band of that bit also inverts its partner bit, so the keyed band changes both output bits and the P1K line of that bit's Color and component fails through its keyed band. A P1K contrast cannot leave the P4 line of the band it changes alone: the 192 bands that P1K reads are the bands of P4, so VKEYAR/G/B move P4.n6b7 and V4D moves the P4 line of its bit, and each moves the P1J line of the same Color and component.

### P2: only two bits of red, green and blue per pixel (3 propositions)

**1. Proposition.** In SuperHires mode, the 4-bit value of component X shown by a visible pixel of value c depends only on the two encoded letters of X of Color c. Pixels of value c whose Color c has the same two letters for X show the same 4-bit X, whatever the neighbours, the other Colors, the line or the frame.

| Id | Claims | Component |
|---|---|---|
| P2.R | APPC-shr.c3, CH4-appC-context.c3 | red |
| P2.G | APPC-shr.c4, CH4-appC-context.c4 | green |
| P2.B | APPC-shr.c5, CH4-appC-context.c5 | blue |

Page 301.

**2. Stimulus.** As for P1: all bands of F1, the Q0 lines of F2–F4, and lines 0–29 of F7–F9.

**3. Observation.** All four bits of R, G and B at every graded pixel of those lines, grouped by (value c, component X, the two letters of X of Color c in the band's palette).

**4. Expected relation.** Within each group, one single 4-bit value.

The source is "There are only two bits of red, green and blue color resolution per hires pixel" (p.301). The key's `--` is not a source here: it states that no register bit stores bits 1–0, which P4 grades bit by bit and P1K per Color and component (L2). The pages give no value for bits 1–0 (L2), so the test checks only that they are a function of the two letters, never what they are.

The quantifier "only":
- Within SuperHires mode, no program that writes the registers as the table prescribes can make a value-c pixel show more than four distinct values of X, one per pair of letters.
- It is graded over every context (C8), over palettes in which the other Colors' letters change while Color c's stay fixed, and over every line and frame.
- The test reports, per group, how many palettes contributed. For every (c, X), at least one group must contain pixels from two palettes that differ in some other Color's letters of X. The palettes of §3 meet this for all twelve (c, X). The test grades this as the precondition P2-DIVERSITY, printed before P2 and carrying no claim id; P2 is not graded while it fails.
- Across Q0–Q5, Z and O, every (c, X) takes all four letter pairs (§3). So "two bits" is also graded from below: bits 3–2 take all four values (P1).

**5. Preconditions.** C1–C9 of F1–F4. Lines 0–29 of F7–F9 join the population only when C3-F7F9, C4-F7F10, C5-F7F10 and C7-F7F10 hold (§4.7); otherwise they are left out and a MEASURED line says so.

**6. Contrast (V2).** No stimulus inside SuperHires mode is predicted by the pages to break P2, so the contrast leaves SuperHires mode on purpose. Run F1 with BPLCON0 = $A000. C5 is then evaluated in its hires form (i = x − s) and holds for F1 in that form, and F1 is not a SuperHires frame: C5/R2 fails, P2-DIVERSITY (which reads F1's palette bands) and P2 are not graded, and C4 fails because the header of F1 carries the changed BPLCON0. The test prints the verdict of P2's comparator on the frames that are usable as a MEASURED line.

Contrast for P2-DIVERSITY: C8B (§6) writes values 0 and 3 only over the words that the bands read, so the groups of values 1 and 2 are populated by the Q0 lines alone, and P2-DIVERSITY fails; C8's pair coverage fails with it, since both read the same pixels.

Live input (P2G): F1 stays a SuperHires frame, with the palette bands Q1–Q5, Z and O written with Color 0 as Q0's Color 0 in every register and Colors 1–3 as before. The preconditions and P2-DIVERSITY hold as at baseline, and P2's comparator is the same. P1.0R, P1.0G, P1.0B and P1J.0R, P1J.0G, P1J.0B fail with it: the palettes it writes hold Color 0 fixed where P1 expects Color 0's letters of the plan, and P2 can only pass if Color 0 does not change between the palettes.

Applied to the hires pixels, P2 fails. In hires a value-c pixel shows all of register c (p.94, Table 3-18; p.273). Under the table, bits 9–8, 5–4 and 1–0 of registers 0–3 hold Color 0's letters (row q = 0). So for c = 1, 2, 3 the lower two output bits follow Color 0's letters. In Q1 and Q3, for example, Color 1's red is 2 in both, while Color 0's red is 2 and 0.

**7. Reference frame.** As P1.

### P3: SuperHires colour control is coarser than Hires and than Lores (2 propositions)

**1. Proposition.** A program controls each component of a pixel's colour in 16 steps in Hires and in Lores, and in fewer steps in SuperHires.

| Id | Claims | Comparison |
|---|---|---|
| P3.H | CH4-appC-context.c1 | SuperHires coarser than Hires: fewer distinct values of each component |
| P3.L | CH4-appC-context.c2 | SuperHires coarser than Lores: fewer distinct values of each component |

Page 301. APPC-shr.c1 names both comparisons ("than either Hires or Lores modes"), so neither half carries it; P7.1 does.

**2. Stimulus.**
- SuperHires: every band of F1 -- the top and bottom Q0 lines, and the Q1–Q5, Z, O and S0–S3 bands -- the whole population F1's own stimulus plan covers, not the S0–S3 bands alone, and the sixteen sweep bands of F11 (§4.6), in which the letters of Color 1 and of Color 0 run through all sixteen pairs. F1's twelve palettes alone show at most twelve values of a component at value-1 pixels, below the sixteen of Hires and Lores whatever the granularity of SuperHires; the sweep is what lets the relation fail. Every register written by the table (§3).
- Hires: F5's sixteen bands with COLOR01 = $0kkk.
- Lores: F6's sixteen bands with COLOR01 = $0kkk.

**3. Observation.** The 4-bit R, G and B at graded pixels with v[i] = 1 in those bands. In hires i = x − s and in lores i = ⌊(x − s)/2⌋.

**4. Expected value.**
- Hires and Lores: in band k, each of R, G and B equals k in all four bits, so each component takes 16 distinct values. The sources are "Twelve bits of color selection allow you to define, for each of the 32 registers, one of 4,096 possible colors" (p.46), "each one has 4 bits of red, 4 bits of green, and 4 bits of blue" (p.92), and Table 3-18 (p.94: value 0001 selects register 1).
- SuperHires: at every value-1 pixel of every band of F1 and F11, the number of distinct 4-bit values each component takes over the population above, whatever that number is. The number four belongs to P2 ("only two bits of red, green and blue color resolution", p.301); this proposition does not assume it.
- The relation holds for R, G and B, for each comparison, when the SuperHires count of the component is smaller than the Hires (Lores) count of the same component. A SuperHires count that reaches the Hires (Lores) count fails it.

The source for the comparison is "SuperHires mode has a coarser granularity of color control than either Hires or Lores modes" (p.301), the sentence of CH4-appC-context.c1, CH4-appC-context.c2 and APPC-shr.c1. The quantifier "either" is graded as two separate comparisons.

**5. Preconditions.** C1–C9 of the frames the proposition reads: F1, F11 and F5 for P3.H, F1, F11 and F6 for P3.L. In F5 and F6 they are evaluated in their hires and lores forms: s fits the mapping of that mode, and the pure R lines show COLOR03 = $C16.

**6. Contrasts.**
- V3a: run F1 with BPLCON0 = $A000 and replace the S0–S3 bands by sixteen bands with COLOR01 = $0kkk. F1 is then not a SuperHires frame: C5 (F1's SuperHires pixel mapping) and C7 (F1's planned band count, sixteen bands instead of twelve) fail, C4 fails with them (F1's header carries the changed BPLCON0), and P3.H, P3.L and P7.1 are not graded.
- V3g (live input): F11's sweep bands keep Color 0 as Q0's and run only Color 1's letters, through four values. A value-1 pixel then takes at most 4 values of a component in F11, and together with F1's palettes fewer than sixteen: the unchanged comparator passes P3.H, P3.L and P7.1. Its bands are F1's S0–S3 palettes four times over, so its population is F1's twelve palettes: V3g shows that the comparator can pass, not that a display passes a population that can fail.
- V3b (Hires half of P3.H, on V3g's SuperHires side): V3g, and in F5 write COLOR01 = ($0kkk AND $0CCC). The masked sweep takes four distinct values per component in Hires (p.46, p.92; Table 3-18 on p.94), so a SuperHires count of four or more does not lie below the Hires count and the relation fails.
- V3c (Lores half of P3.L, on V3g's SuperHires side): V3g, and in F6 write COLOR01 = ($0kkk AND $0CCC). The masked sweep takes four distinct values per component in Lores (p.46, p.92; Table 3-17 on p.93 selects register 1 for value 00001 in low resolution), so the relation fails for Lores.

V3b and V3c leave every precondition of F5 and F6 unchanged; each fails P7.1 through its own half. They are run on V3g because P3.H, P3.L and P7.1 fail without any contrast when the sweep is as planned.

**7. Reference frame.** R2 in each mode's form, and R3.

### P4: each bit of each lower colour register carries the letter the table puts there (192 propositions)

**1. Proposition P4.n.b.** In SuperHires mode, bit b of COLOR n carries the letter L in row n, column b of the p.301 table. L is component X of Color c, upper or lower, as the key names it (§3). Inverting that single bit, with every other register bit as the table prescribes for Q0, can change the display in only one place: output bit 3 of X (if L is an upper letter) or bit 2 (if lower), on pixels of value c. Bits 1–0 of X on value-c pixels are not compared (L2). The change, where it appears, is the inversion.

The claim is APPC-bpltab.r{n}b{b}, with n = 00…0F (hexadecimal, as the table's rows) and b = 00…11. For example, APPC-bpltab.r05b00 says bit 00 of COLOR05 carries `l`, the lower blue letter of Color 1. The letter of every (n, b) is the table in §3. Page 301.

**2. Stimulus.** F2–F4. Each perturbation band holds Q0 with the single bit b of register n inverted. The band change that starts band k+1 restores band k's register. Bits 15–12 are never touched.

**3. Observation.**
- Every graded pixel of the band's two lines is compared with its *baseline*.
- The baseline, in all twelve bits, is the value shown on the same frame's Q0 lines (top and bottom) by visible pixels with the same (v[i−1], v[i], v[i+1]).
- A pixel whose triple never occurs visibly on those Q0 lines, or occurs there with more than one value, is excluded, and the count is reported.
- The baseline is observed, not predicted, so P4 does not depend on P1: a table defect that already shows on the Q0 lines, and fails P1 there, is still caught where its bit is inverted.

**4. Expected result and verdict.**
- **FAIL** if any graded pixel differs from its baseline anywhere other than the allowed places: bit 3 or 2 of X (per L) and bits 1–0 of X, on pixels of value c. A change on a pixel of another value in any of its twelve bits, in another component, or in the other encoded bit of X fails.
- **OBSERVED** if at least one graded pixel of value c shows the allowed bit inverted.
- The verdict is PASS when OBSERVED and not FAIL. It is **NOT OBSERVABLE** when neither (L1).

The source is the table row and the key (p.301). A register bit that carries one letter of one Color cannot be where the chip gets any other part of any Color. Which pixels read it is not stated, so no pixel is required to change.

The report lists, for every OBSERVED instance, the triples of the pixels that changed. No expectation is attached to them.

**5. Preconditions.**
- C1–C9 of the frame that carries the band, and the null case of that frame (V4c).
- The band has at least one graded line.

A bit whose frame fails a precondition is not graded; the test records each such bit.

**6. Contrasts.**
- V4a: in one band of F2 whose bit was reported OBSERVED, also invert a bit that carries a letter of another Color and was OBSERVED in its own band. The pages predict a change on pixels of that other value, so P4 fails for the first band.
- V4b: run F2–F4 with BPLCON0 = $A000 (hires); the plan does not fix which of the three frames carries a given bit. Inverting bit 09 of COLOR01, which carries `a` of Color 0, then changes R1 of value-1 pixels (p.94, p.273), so P4.01.09 fails. Preconditions hold in hires form.
- V4c, the null case: a band that restores Q0 without a new inversion is predicted to change nothing. It must come out not OBSERVED. It is printed before the 192 propositions as a precondition of the frame's bits. Its stimulus contrast makes the null band of F2, F3 and F4 (the closing restore band in F2 and F3, band 33 in F4) also invert bit 11 of COLOR01 for part of its own first line: after the band's MOVE it writes the inverted value repeatedly and then restores COLOR01, so the pixels of that line differ from the baseline of their neighbour triples while the triples keep their baselines. The contrast has one selector for each frame (V4C2, V4C3, V4C4), so that each of the three V4c lines can be failed alone. It gives the null band more than four MOVEs, a spacer line by §4.4; V4c grades its lines all the same.
- V4d, a contrast for every P4.n.b whose partner is observed (the 24 p-half bits; one selector each, V4DN0B2 … V4DN3B11): the band of bit b of register n inverts, besides its own bit, bit b XOR 1 of the same register. By the table that bit carries the other letter of the same component of the same Color (bits 11/10, 9/8, 7/6, 5/4, 3/2 and 1/0 each hold the upper and lower letter of one component, §3). The pages predict a change of the other encoded bit of X, which P4.n.b does not allow, so P4.n.b fails (the P1K and P1J lines of that bit's Color and component fail with it, since they read the same band, §7 P1 item 8). For a bit whose partner is never OBSERVED no input can show the clause failing at this output (L1); the test lists those bits.

**7. Reference frame.** R2, R3, and R5 for band boundaries.

### P5: the bitplane colours are held in the sixteen lower registers, in more of them than the four a normal display uses (2 propositions)

**1. Propositions.**
- **P5.L** (CH4-appC-context.c7; pages 301, 94): In SuperHires mode, some visible playfield pixel depends on a register outside COLOR00–COLOR03. In Hires, with the same two bitplanes, no playfield pixel depends on a register outside COLOR00–COLOR03. The bitplane half of APPC-shr.c6 is graded here; that id names sprites as well, so P7.2 carries it.
- **P5.U** (no id of its own; pages 301, 302): In SuperHires mode, no visible playfield pixel depends on an upper register (COLOR16–COLOR31).

APPC-shr.c8 says the bitplane colours are "specially encoded in the sixteen lower color registers" instead of being "stored as 0x0RGB in four color registers". That names both clauses: more than the four registers (P5.L), and the lower sixteen rather than the upper (P5.U). Neither carries it alone; P7.3 does.

**2. Stimulus.**
- P5.L: the perturbation bands of F2–F4 with n = 04…0F, and F5's inversion bands.
- P5.U: F7's inversion blocks 2–14 (COLOR16, 20–31, each XOR $0FFF) over the value-0 SuperHires playfield of the zero buffer (§4.6), and F8, whose COLOR10–1F hold T0c: all sixteen upper registers differ from F7's baseline T0 at once.

**3. Observation.**
- P5.L: graded pixels of the F2–F5 bands against their baseline, as in P4. In F5 the baseline is taken the same way from F5's own Q0 lines.
- P5.U: in F7, the graded pixels outside the sprite run [xa, xb] of P6 on the lines of blocks 2–14, against the pixel at the same x on F7's baseline sprite lines of the same sprite value; in F8, the graded pixels outside the run on every sprite line, against the same baseline pixels. On a line where F7 shows no run, every graded pixel of the line is outside it.

**4. Expected relation.**
- P5.L, SuperHires: at least one of the 144 bands with n = 04…0F changes at least one graded pixel. Any change counts, whether P4 grades it PASS or FAIL, and whether or not P4 can attribute it to the band's own bit, in a band whose rebuilt palette (§4.7) is Q0 with the planned bit of the planned register inverted and nothing else. A band whose rebuilt palette differs from the plan is reported and not counted. The test prints the bands graded, the bands that match the plan and the pixels checked; "no band changes a pixel" needs all 144 bands graded and matching the plan, otherwise P5.L is not graded.
- P5.L, Hires: the bands inverting n = 04…0F change no graded pixel. The bands inverting n = 00…03 invert all twelve bits of exactly the pixels with v[i] = n.
- P5.U: in F7, no inversion block changes any graded pixel outside the run, in any of its twelve bits; in F8, no graded pixel outside the run differs from F7's baseline. F7 reaches COLOR16 and COLOR20–31, F8 all sixteen upper registers.

The sources:
- P5.L: "In order to decode sprite and bitplane color information in SuperHires mode, certain multiplexing occurs in the use of the registers. Instead of 4 bits of red, green, and blue for bitplane registers 0-3 stored as 0x0RGB in four color registers, SuperHires bitplane colors are specially encoded in the sixteen lower color registers" (p.301); Table 3-18 (p.94): two bitplanes select registers 0–3; p.297: "There are new color register interpretations as part of the SuperHires mode."
- P5.U: "SuperHires bitplane colors are specially encoded in the sixteen lower color registers" (p.301) and "SuperHires sprites are encoded in the upper sixteen color registers" (p.302): the upper sixteen are the sprites', the lower sixteen the bitplanes'.

The quantifiers:
- P5.L: "certain multiplexing occurs" is existential. The report lists, for each register 04–0F, whether any of its bits changed a pixel. No register is required individually (L1).
- P5.U: "in the sixteen lower color registers" is universal for the upper registers: one changed playfield pixel fails it.

**5. Preconditions.**
- P5.L: C1–C9 of F2–F4 (SuperHires half) and of F5 (Hires half, in hires form), and the null case V4c of F2–F4.
- P5.U: C1, C2, C4 and C5 on lines 0–29 of F7, C7 including the block changes, C10 (P6) and the run (RUN-ENDS). C11 does not condition it.

**6. Contrasts.**
- V5 (P5.L, SuperHires): run F2–F4 with BPLCON0 = $A000. Table 3-18 predicts that no register 04–0F changes a pixel, so the SuperHires clause of P5.L, if it were graded, fails; the preconditions of F2–F4 fail instead (R2), and P5.L is not graded. F5 carries the Hires case in the plan.
- VLIVE (liveness of P5.L's change counter, no contrast): F2's band 48 (n=04, b=00) also inverts bit 2 of COLOR01, which carries the letter `l` of Color 1, as bit 0 of COLOR04 does, and band 49 restores it. The counter sees the changed pixels of band 48 (288), and does not count them: the band's rebuilt palette differs from the plan. P5.L and P7.3 are then not graded, and P4.n4b0 turns from NOT OBSERVABLE to PASS. VLIVES adds V6S for P7.2.
- V5b (P5.L, Hires): in F5, the band planned to invert COLOR05 inverts COLOR01 instead, and the expectation of the COLOR05 band (no graded pixel changes) is kept. Table 3-18 and p.273 predict that the value-1 pixels change in all twelve bits, so the Hires clause of P5.L fails.
- V5c (P5.U): in F7, the block change of block 2 also writes Q0 with Color 0 set to (3, 2, 1), every letter of Color 0 inverted, into the seven lower registers that hold Color 0's letters: COLOR00 = $0FA5, COLOR01 = $07AD, COLOR02 = $0BE1, COLOR03 = $0F25, COLOR04 = $0DA7, COLOR08 = $0EB4, COLOR0C = $0F85 (§3 formula). Block 3's change restores them. The first lines of blocks 2 and 3 are spacer lines of the variant. The expectation of block 2 (no pixel outside the run changes) is kept. The pages predict that the value-0 playfield pixels of block 2's graded lines show Color 0's inverted letters in bits 3–2 (P1), so P5.U fails. C10 holds, since the baseline blocks are unchanged.
- V5d (P5.L, Hires): in F5, the band planned to invert COLOR02 inverts COLOR03 instead, and the expectation of the COLOR02 band (the value-2 pixels invert in all twelve bits, no other pixel changes) is kept. Table 3-18 and p.273 predict that the value-3 pixels change, so the second Hires clause of P5.L fails.
- V5e (P5.U, F8): in F8, the line-30 band also writes the seven lower registers that hold Color 0's letters, inverted, as V5c does. The expectation (no playfield pixel of F8 outside the run differs from F7's baseline) is kept. The pages predict that the value-0 playfield pixels of F8 show Color 0's inverted letters (P1), so P5.U fails through F8.

**7. Reference frame.** P5.L as P4. P5.U as P6: R0, R1, R5 and the run.

### P6: SuperHires sprite colours are encoded in the upper sixteen registers (2 propositions)

**1. Propositions.**
- **P6.1** (p.302; its sentence is APPC-shr.c9, which P7.4 carries): In SuperHires mode, with COLOR16–COLOR31 written as the p.302 table prescribes for Colors 16–19, the inner pixels of a sprite-0 line of value v show the encoded letters of Color 16+v in bits 3–2 of R, G and B. They do not depend on COLOR00–COLOR15.
- **P6.2** (CH4-appC-context.c6, p.301; the sprite half of APPC-shr.c6 is graded here, and P7.2 carries that id): In SuperHires mode, some inner sprite-0 pixel depends on an upper register other than COLOR17–19. In Hires none does.

**2. Stimulus.** F7–F10: sprite 0 over a value-0 SuperHires playfield from line 30, with BPLCON2 = $0024 and SPREN set. Each block of three lines carries values 1, 2 and 3. F7 has T0 above and Q0 below, with inversions of COLOR16 and 20–31. F8 has T0c above. F9 has Q1 below. F10 is Hires.

**3. Observation.**
- Sprite lines are the source lines on which the port shows sprite 0 fetching a data pair. The value v of the line is taken from that pair (p.107: "the high order word … contribute[s] the leftmost digit").
- The *run* on a sprite line is the maximal set of consecutive visible pixels whose twelve bits differ from those at the same x on window lines 31–35 of the same frame, which show the same value-0 playfield without sprite 0 (sprite 0 starts at window line 36). It is found on F7's baseline lines. No expected colour enters it.
- The run must have the same ends [xa, xb] on all of them that show one (RUN-ENDS). Its length is graded by C11, which conditions none of P5.U, P6.1 and P6.2.
- A baseline sprite line of F7 that shows no run although C10 holds is a failure of P6.1 on that line, not a missing anchor: under T0 each of Colors 17–19 differs from Q0's Color 0 in bits 3–2 of at least one component (§3), so the pages predict a run on every sprite line. The ends [xa, xb] are then taken from the lines that do show one. If no baseline line shows a run, P6.1 fails and P6.2 is not graded, having no inner pixels.
- The *inner pixels* are xa+1 … xb−1. They are graded at the same positions in F8 and F9. F10 uses its own run (item 7).
- The pixels at xa and xb are not graded, because the pages do not say how a SuperHires pixel pair made of one sprite and one playfield pixel is coloured.

**4. Expected values.**
- **P6.1**
  - F7 baseline blocks: bits 3–2 of R, G and B equal the letters of Color 16+v under T0, which is (v, v+1, v+2) mod 4. For example, v = 1 gives Color 17, `GH-- IJ-- KL--`, equal to 1, 2, 3.
  - F8: the same under T0c.
  - F9: all twelve bits of every inner pixel equal those of the same position and value in F7's baseline blocks, although COLOR00–0F changed from Q0 to Q1.
  - Sources: "SuperHires sprites are encoded in the upper sixteen color registers using a similar scheme:" and the key and table that follow (p.302).
  - Whether the playfield outside the run depends on the upper registers is a question about the bitplane colours; P5.U grades it.
- **P6.2**
  - F7: at least one of the thirteen inversion blocks (16, 20–31) changes at least one inner pixel.
  - F10: none of those thirteen changes any inner pixel. The blocks inverting 17, 18 and 19 invert all twelve bits of the inner pixels of the lines with v = 1, 2 and 3 respectively. Baseline inner pixels show all twelve bits of register 16+v.
  - Sources: the multiplexing sentence (p.301); "Sprite channel 0, for example, takes its colors from registers 17 - 19" (p.108, Table 4-2); "The color value of the first register in each group of four registers is ignored by sprites" (p.103).
  - The quantifier is existential, as in P5.L.

**5. Preconditions.**
- C1, C2, C4, and C5 on lines 0–29.
- C7, including the block changes, which follow WAIT (line, $00).
- **Run:** a run is present on the baseline sprite lines, with the same ends on every line that shows one and in F8 as in F7 (RUN-ENDS), and the playfield outside the run differs between F7 and F9 (P6.1-F9PF).
- **C10:** in each of F7–F10, the port shows sprite 0 fetching its two control words and 60 data pairs, and sprites 1–7 fetching only $0000 control words: 14 reads of the null sprite, every one $0000, and, from the frame's first Copper fetch to its last bitplane fetch, no read below $80000 from any address other than the plane buffers, the zero buffer, the Copper lists, sprite 0's structure, the null sprite and word 0.

**C11** is a graded line of its own and conditions neither P5.U nor P6.1 nor P6.2: the run, on every baseline line of F7, F8, F9 and F10 that shows one, is 32 ± 1 hires periods long. It is graded only when C4-F7F10, C5-F7F10, C7-F7F10 and C10 hold. p.101 says sprites are 16 pixels wide and "changing the resolution or interlace mode of the playfield has no effect on the size or resolution of a sprite". A lores pixel is two hires periods. The ±1 is not a tolerance: the sprite covers 64 SuperHires pixels starting on a 140 ns boundary (SH0, p.303, with SHSH1 = SHSH0 = 0), and the pages fix neither where the output's hires periods fall against that boundary nor how a period of one sprite and one playfield pixel is shown. So the run is 32 whole periods, or 31 whole periods and up to two mixed ones.

Contrasts for C10: point SPR0PT at the null sprite (no sprite data is fetched); point SPR1PT of F7's header two words past the null sprite (C10S: sprite 1 fetches from an address that is not the null sprite's; C4-F7F10 does not read the sprite pointers); the null sprite of C9N and C9S (§6). Measure for C11: write sprite 0's data words as $FF00 where they are $FFFF (8 pixels wide). C10 still holds, and the run is 16 ± 1 periods long. C11 has no green control that keeps its gates: the run is 32 periods only in a Hires frame, and F7 or F8 in Hires fail C4-F7F10 and C5-F7F10. F10's own 32 periods, in the same population, show that the comparator sees 32.

**6. Contrasts.**
- V6a: in F7, write COLOR10–1F = T0c instead of T0 and keep F7's expectation (T0). The pages predict T0c's letters, so P6.1 fails. The run is still found, because each T0c Color 17–19 differs from Q0's Color 0 in at least one component.
- V6b: in F9, also write COLOR10–1F = T0c. The pages predict that the sprite changes, so "unchanged by the lower sixteen" fails.
- V6c: run F7 with BPLCON0 = $A000. F7 is then a hires frame: C4-F7F10 and C5-F7F10 fail, and P1, P2, P5.U, P6.1 and P6.2 (which read lines of F7) are not graded. F10 is the Hires case of the plan; p.108 and p.103 predict that none of the thirteen inversions changes sprite 0 there.
- V6d (P6.2's Hires half): in F10, the block planned to invert COLOR20 inverts COLOR17 instead, and the expectation of the COLOR20 block (no inner pixel changes) is kept. p.108 predicts that the inner pixels of that block's value-1 line change in all twelve bits, so the Hires half fails.

**7. Reference frame.** R0, R1, R5, and sprite lines identified at the port. The run's ends in F7 anchor the inner positions for F8 and F9. In F8 the run found the same way must have the same ends. In F10 (Hires) the run is found anew on its baseline lines with the same rule, and its length is graded by C11. The rule uses no colour, so it applies unchanged in Hires.

### P7: claims that name the clauses of two propositions at once (4 propositions)

**1. Propositions.**

| Id | Claim | Graded as |
|---|---|---|
| P7.1 | APPC-shr.c1 | P3.H and P3.L together |
| P7.2 | APPC-shr.c6 | P5.L and P6.2 together |
| P7.3 | APPC-shr.c8 | P5.L and P5.U together |
| P7.4 | APPC-shr.c9 | P6.1 and the SuperHires half of P6.2 together |

APPC-shr.c1 says SuperHires is coarser "than either Hires or Lores modes"; APPC-shr.c6 says multiplexing occurs "to decode sprite and bitplane color information"; APPC-shr.c8 says the bitplane colours are "specially encoded in the sixteen lower color registers" instead of "in four color registers"; APPC-shr.c9 says "SuperHires sprites are encoded in the upper sixteen color registers using a similar scheme" (p.302), the scheme of the bitplane registers. P6.1 grades the letters of the sprite key and the independence of the lower registers, and the SuperHires half of P6.2 grades the multiplexing "using a similar scheme", so neither carries APPC-shr.c9 alone; P7.4 does. Each half is graded by its own proposition. P3.H, P3.L, P5.L and P6.2 carry their own context claims; P5.U carries none. Pages 301 and 302.

**2.–5. Stimulus, observation, expected value, preconditions.** Those of the two halves, unchanged. The verdict is PASS when both halves pass, FAIL when either fails, and not graded when either is not graded.

**6. Contrasts.** Those of the halves: V3b fails P7.1 through P3.H, V3c through P3.L; V5, V5b and V5d fail P7.2 through P5.L, V6c and V6d through P6.2; V5, V5b and V5d fail P7.3 through P5.L, V5c and V5e through P5.U; V6a fails P7.4 through P6.1. V5c on VLIVE's side fails P7.3 through P5.U, and V6d on VLIVES's side fails P7.2 through P6.2, where P5.L is not graded. A joint proposition is graded when both halves are graded; a half that is not graded leaves it not graded unless the other half fails.

**7. Reference frame.** Those of the halves.

---

## 8. Claims and propositions

| Claims | Proposition(s) |
|---|---|
| APPC-bplkeykey.c0r … c3b (12) | P1J.cX, one each: the joint of P1.cX with P1K.cX. P1 grades the two letters and P1K grades the `--` (no other register bit carries the component), over all 192 register bits, per Color and component; neither carries an id. |
| APPC-bpltab.r00b00 … r0Fb11 (192) | P4.n.b, one each |
| APPC-shr.c1 | P7.1 (P3.H and P3.L together) |
| CH4-appC-context.c1 | P3.H |
| CH4-appC-context.c2 | P3.L |
| APPC-shr.c3, CH4-appC-context.c3 | P2.R |
| APPC-shr.c4, CH4-appC-context.c4 | P2.G |
| APPC-shr.c5, CH4-appC-context.c5 | P2.B |
| APPC-shr.c6 | P7.2 (P5.L for bitplanes and P6.2 for sprites, together) |
| CH4-appC-context.c6 | P6.2 |
| CH4-appC-context.c7 | P5.L |
| APPC-shr.c8 | P7.3 (P5.L and P5.U together); its sentence is also the source of P1's expectation (§7 P1) |
| APPC-shr.c9 | P7.4 (P6.1 and the SuperHires half of P6.2 together) |
| (none) | P6.1 (carries no id; P7.4 carries its sentence), P5.U (graded for P7.3) |

That makes 241 propositions: 12 (P1) + 12 (P1K) + 3 + 2 + 192 + 2 + 2 + 4 + 12 (P1J).

---

## 9. What the pages leave open, and where they read differently

- **Which register and which half a pixel is drawn from.** The table says where each letter is stored, never when the chip reads it. No test may assume it (L1, P4). The table's layout, with each row holding one Color in its upper halves and one in its lower halves, suggests that two neighbouring pixels select a register together. The pages do not say so. Nothing here depends on it.
- **The lower two output bits.** The key writes `ab--`, and p.302 speaks of "the loss of lower bits of precision". No page says what the output shows there (L2).
- **"per hires pixel".** p.301 says "only two bits of red, green and blue color resolution per hires pixel" in a paragraph about SuperHires pixels, which are half a hires pixel. P2 reads it as per displayed pixel. Under the literal reading, per hires period, the test reads one value per hires period anyway (R0), so P2's check is the same either way.
- **p.273 (Appendix A) against p.301.** p.273 says of COLORxx: "One of these registers at a time is selected (by the BPLxDAT serialized video code) for presentation at the RGB video output pins". Read into SuperHires, that would mean register c shown as 0x0RGB, which p.301 replaces ("Instead of 4 bits … stored as 0x0RGB in four color registers"). p.297 states that SuperHires brings "new color register interpretations". The specification treats Appendix A as describing the other modes and specifies SuperHires from Appendix C. The hires and lores contrasts (V2, V3a, V3b, V3c, V4b, V5, V5b, V6c, V6d) use p.273, p.93 and p.94 only in Hires and Lores, where the pages agree.
- **One bitplane.** p.300 allows a SuperHires depth of "1 or 2". The key and table cover four Colors. No page says which registers a one-bitplane SuperHires display uses. Only two bitplanes are used here.
- **HIRES and SHRES both set.** p.300 says only "Set it to zero if SHRES enabled". The result of setting both is not stated and is not tested.
- **Data fetch in SuperHires.** DDFSTRT and DDFSTOP values are given for Lores and Hires only (p.60). p.91 says bit H3 is "only respected in Hires Mode" and is silent on SuperHires. The test therefore derives every position from what the port fetched and the output showed (R0–R3), not from the fetch registers.
- **Sprites.** The sprite key names Color 16, but p.103 says the first register of each group is never shown. The pages do not say how sprites 2–7, or attached sprites, are coloured in SuperHires, nor how a hires period made of one sprite and one playfield pixel is coloured (L3; P6 grades inner pixels only).
- **HAM and dual playfield.** p.300 marks HAM "Incompatible" and DPF "Compatible" with SuperHires, but gives no encoding for either. Neither is used.

---

## 10. Implementation and measured results

### 10.1 Programs and data

`prog_main.s` implements section 4.5's eight steps. Every register write besides the CPU's own DMACON, COP1LC and its strobe is a Copper MOVE inside the per-frame tables (`bp_cop_f0_base.i` … `bp_cop_f11_base.i`), copied into place by the program from tables carried in its own image, as section 4.2 allows. In F0–F6 and F11 the header writes SPR0PT–SPR7PT at $120–$13E, all at the null sprite. `bp_data_pattern1.i`/`bp_data_pattern2.i` carry the 256-word R section and the 32512-word xorshift pattern of section 4.2; their first four words per plane equal the section's own four example words. `bp_data_sprite0.i` carries the 124-word sprite 0 structure of section 4.2; F7–F10 are graded in section 10.4. Each contrast is its own `prog_<id>.s`: an `equ` of one or two `MUT_<ID>` symbols followed by `include "prog_main.s"`. Every frame's own table is chosen by an `ifd`/`else` cascade on that symbol, so the same copy loop runs regardless of which table it copies. Five contrasts change the program itself under their symbol: C2 (the program ends in a loop instead of `DONE`), C8B and C3D (words of plane 1's pattern are overwritten after the copy), C9N and C9S (words of the null sprite are overwritten); R0 and C8 change the pattern tables.

### 10.2 Reference frames, as measured

- **R0.** w = 1, phi = 0: one video sample is one hires period. Measured on F5's own pattern lines (a hires frame, full twelve bits compared, per section 5) and reused for every frame, per section 5's rule that w and phi are frame-invariant.
- **R1.** The harness's scan doubler shows two output lines per source line (`nanomig_harness.h`'s own note). Source line k's own two copies are not the two spans that a per-line grouping by fetch-burst cycle overlap assigns to line k: one of those is line k+1's fetch overlapping the same output line. Line k's own pair is the last span grouped under line k (its first copy) and the first span grouped under line k+1 (its second copy). Both copies of every graded line agree: 767,344 sample pairs across F1–F6 and F11, 0 differing.
- **R2.** s is frame-specific and not small: F1's own s is 266. Window line 0, the line immediately after each frame's own Copper header, shows the fetched bitplane data like every other line once R1's pairing is used; every population this specification calls "the top Q0 lines" is graded as window lines 0–23. The Q0 tail of F2 and F3 is graded from window line 186, that of F4 from line 92: each starts two lines after the first line of the frame's null band.
- **R3.** On a pure R line of F1 the output shows, in samples, 65 of blanking, 76 of border colour, the run of 576 hires periods, 102 of border colour and 89 of blanking. The run is bounded by the border colour on both sides, which is what separates it from the border stretch beyond the window. The check interval of the SuperHires frames is [149, 709); F10's, which also satisfies 16 ≤ i(x) with s = 141, is [157, 709).
- **Chip-memory reads.** In each of F1–F6 and F11 the port shows 16 reads of the null sprite, all returning $0000, and no read below $80000 from any other address apart from the plane buffers, the Copper lists and word 0. Word 0 is read in groups of four, one group in every line (312 complete groups in the window of each of F1–F6): the memory-refresh slots, which no pointer of these frames addresses. The window of a frame runs from its list's first fetch to the next list's; F11's is one frame long, the gap between the first fetches of F10's list and of F11's.
- **Words per line.** The port shows 80 words per plane and line in F1–F4 and F11, 40 in F5 and 20 in F6.

### 10.3 Measured verdicts, both cores agreeing throughout

- P1 (all twelve): PASS, 64,868, 64,611, 64,392 and 78,289 graded pixels for Colors 0 to 3. The pixels are those of F1, of the Q0 lines of F2–F4 and of lines 0–29 of F7, F8 and F9; the verdicts are the same without the lines of F7–F9 (measured).
- P3.H, P3.L, P7.1: FAIL. Over F1's palettes and the sixteen sweep bands of F11, a value-1 pixel's red, green and blue each take 16 distinct 4-bit values, against 16 in Hires and in Lores: SuperHires does not lie below either (p.301: "SuperHires mode has a coarser granularity of color control than either Hires or Lores modes"). F1's palettes alone show 7, 9 and 9 values, which is why the sweep is needed: twelve palettes cannot show sixteen values. The count of four belongs to P2.
- P2 (R, G and B): FAIL. At least one (value, letters) group's graded pixels (272,160 pixels in 16 groups) do not all show the same full four-bit value. P2-DIVERSITY: PASS, 12 of 12 (c, X); 11, 11 and 12 of the 16 groups of R, G and B are populated from two or more palettes.
- P4: the p-half bits (2, 3, 6, 7, 10, 11) of COLOR00–03, 24 propositions: PASS, with the triples of the pixels whose allowed bit changed printed for each. The q-half bits (0, 1, 4, 5, 8, 9) of COLOR01–03, 18 propositions: FAIL; the allowed bit itself never changes a graded pixel of the band (changed=0 of 1120 graded pixels) while pixels change outside the allowed places. The q-half bits of COLOR00 (6) and the whole of COLOR04–0F (144), 150 propositions: NOT OBSERVABLE. V4c: PASS in F2, F3 and F4 (checked=1120, diffs=0).
- P1K: P1K.0R, P1K.0G and P1K.0B PASS: none of the 176 off-key bands changes X of a value-0 pixel, none of the 16 keyed bands changes the other output bit, and both letters are observed. P1K.1R … P1K.3B (nine lines) FAIL: 2 of the 176 off-key bands change X of the value-c pixels.
- P1J: the three ids for Color 0 (`c0r`, `c0g`, `c0b`): PASS, with P1 and P1K. The nine ids for Colors 1–3 (`c1r` … `c3b`): FAIL, through P1K; P1 holds.
- P5.L's SuperHires half: FAIL, by the any-bit count as well as the allowed-bit count: all 144 bands n = 04…0F graded and matching the plan (their rebuilt palettes equal Q0 with the planned bit of the planned register inverted), 0 of 161,280 graded pixels change. Its Hires half: PASS, both clauses (17,664 graded pixels).
- The rebuilt palettes (section 4.7) equal the plan in every band of F1–F11.
- The preconditions C1–C9 (C3 over F1–F6 and F11 with both planes and the stray-read clause, C4's header values including SPR0PT–SPR7PT, C6 per frame and per line, C7's count and timing, C8's coverage over 219 bands and F1 diversity, C9's null-sprite reads and refresh groups), R0, R1 and R2's bottom lines: PASS.

Every contrast of the package (the header of `test_ch4_shres_bitplane.cpp` lists each one, with the lines that move under it) was run on both cores; fx68k and TG68K print the same PASS, FAIL, RECORDED and MEASURED lines on every one. A line whose precondition fails, or whose population is empty, is NOT GRADED; the records say which lines each contrast fails, and, where a contrast has to fail a second line, which and why. Lines that are FAIL at baseline turn PASS under their live inputs: V3G (P3.H, P3.L, P7.1), V6S (P6.2-SHR, P6.2, P7.4) and VP1JG (the eighteen q-half P4 lines, the nine P1K lines of Colors 1–3 and the nine P1J lines of Colors 1–3). P2G turns P2.R, P2.G and P2.B PASS on 272,160 pixels in 16 groups, with P2-DIVERSITY unchanged at 12 of 12; it fails P1.0R, P1.0G, P1.0B and P1J.0R, P1J.0G, P1J.0B, which grade the Color 0 letters of the plan. VLIVE and VLIVES show that the change counter of P5.L sees a changed pixel; they do not turn P5.L, P7.2 or P7.3 PASS, since the band that changes it is not a band of the plan. No live input exists for P5.L's SuperHires half.

### 10.4 F7-F10 (sprite colour frames), as measured

F7-F10 run after F6 and are decoded like F1-F6; from line 30 (line 31 in F9) their bitplane pointers address the zero buffer, so a source line there is a burst of zero words. fx68k and TG68K print the same PASS, FAIL, RECORDED and MEASURED lines on every one of them.

- **Header.** SPR0PT of F7-F10 holds the sprite 0 structure and SPR1PT-SPR7PT the null sprite (SPR0PT-SPR7PT are $120-$13E, four bytes apart). C4-F7F10 holds: the frames run in the order F6, F7, F8, F9, F10, each with the section 4.3 header values for BPLCON0-2, DIW, DDF, the modulos and the bitplane pointers. The colour registers and the sprite pointers are the stimuli under test and are not read by C4-F7F10.
- **C10: PASS.** On each of F7-F10 the port shows sprite 0 fetching its control words $5060 and $8C00 and its 60 data pairs, each pair read once, on 60 consecutive source lines starting at window line 36, with the planned values (line j mod 3 = 0: value 1, 1: value 2, 2: value 3). The null sprite is read 14 times per frame (two control words for each of sprites 1-7), every read $0000, and the frame has no read from any other address than the plane buffers, the zero buffer, the Copper lists, sprite 0's structure, the null sprite and word 0.
- **C3-F7F9: PASS.** Lines 0-29 of F7, F8 and F9 each fetch 80 consecutive words per plane, data as written, with no read of another address inside a line's fetches (bad lines = 0; the same count of words, 80, on every line of each of the three frames).
- **C5-F7F10: PASS.** s = 266 on lines 0-29 of F7, F8 and F9 (i = 2x - s) and s = 141 on F10 (i = x - s). One check interval for F7-F9, [149, 709); F10's is [157, 709).
- **C7-F7F10: PASS.** 15, 1, 1 and 18 band changes in F7, F8, F9 and F10; every WAIT at h=$00; no MOVE lands on a graded line (bad_h = bad_after = bad_before = 0). The band change at line 30 of F9 (20 MOVEs) is its one spacer line.
- **Run.** On the 21 baseline sprite lines of F7 the run is [269, 284], 16 hires periods long on every line. F8 and F9 show the same ends on all 60 sprite lines. F10's 12 baseline lines show [269, 300], 32 periods on every line (RUN-ENDS: PASS).
- **C11: FAIL.** The page's width is 32 +- 1 periods (p.101); F7, F8 and F9, the SuperHires frames, show 16, F10 shows 32. C11 is a graded line of its own, graded only when C4-F7F10, C5-F7F10, C7-F7F10 and C10 hold, and conditions none of the lines below. F10's 32 periods, in the population of the line, show that its comparator sees 32.
- **P6.1: PASS.** 882 component values of F7's baseline inner pixels carry the letters of T0, 2520 of F8's the letters of T0c, and 840 inner pixels of F9 equal F7's baseline pixel (P6.1-F9PF: PASS, 544 of 544 playfield positions outside the run differ between F7 and F9).
- **P6.2: FAIL** through the SuperHires half: none of the thirteen inversion blocks (COLOR16, COLOR20-31) changes any of 546 inner pixels of F7. The Hires half holds (0 of 1170 inner pixels of F10 change) and its comparator is live (baseline inner pixels of F10 show register 16+v; blocks 15-17 invert the lines of value 1, 2 and 3 in all twelve bits).
- **P5.U: PASS.** None of 21,216 playfield pixels of F7 outside the run changes under the thirteen inversion blocks (39 lines), and none of 32,640 playfield pixels of F8, whose COLOR10-1F hold T0c in all sixteen registers, differs from F7's baseline (60 lines).
- **P7.2, P7.3 and P7.4: FAIL.** P7.2 through both halves (P5.L and P6.2 fail), P7.3 through P5.L's SuperHires half (P5.U holds), P7.4 through the SuperHires half of P6.2 (P6.1 holds).
- **R1-F7F10: PASS.** 444,168 pairs of output copies of graded source lines of F7-F10, 0 differing.

Every contrast of this section has its own `prog_<id>.s` and its own records in the header of the test source: C4B, C4C, V6C (the specification's V6c), C7B, C10, C10S, C11, RUNENDS, V5C (V5c), V5E (V5e), V6A (V6a), V6B (V6b), V6D (V6d), V6E, V6L, V6Q and V6S (the live input for P6.2's SuperHires half).
