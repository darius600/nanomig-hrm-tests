# Specification: AA sprite modes, sprite widths, attachment, sprite colour banks and their reset values (unit aa-s6)

This document specifies one integration test package. It was written from the
package's claims (`claims-spec-stage.jsonl`) and from the pages of Commodore's
*Functional Specification for the Advanced Amiga Chip Set (AA)* (cited as
"AA p.N", N being the corpus page number; the document prints none), with the
*Amiga Hardware
Reference Manual*, 3rd edition (cited as "HRM p.N", printed page) read only
where the AA specification builds on the original chip set. Its author never
saw the implementation or any test. The oracle for every expected value is the
AA specification; a Manual page is cited only for behaviour the AA page takes
over unchanged (the sprite data structure, the colour-register grouping of
sprite pairs, the attached-sprite colour numbers, standard display-window and
data-fetch values).

The package has ten propositions, P1 to P10.

---

## 1. Claims not turned into a proposition

None. All 25 claims support one of the propositions below (index in section 7).

Parts of claims that no test grades, and why:

| Claim part | Why it is not graded |
|---|---|
| "on any display resolution" (AA p.31) read as including the VGA mode of AA p.30 | AA p.30 lists "VGA (640x480 non-interlaced)" beside LORES, HIRES and SUPERHIRES. No page gives the beam-counter register values that produce it, and a test must not invent them. "Any display resolution" is graded over the three resolutions BPLCON0 selects, which AA p.2 calls "all 3 resolutions". |
| "Sprites can be attached in any mode" (AA p.31) read as including the 32- and 64-bit sprite widths | The sentence follows the width sentence, but its parenthesis ("formerly could not attach sprites in the ECS SHRES 35nS resolution mode") and AA p.2 ("Attached Sprites are now available in all resolutions") speak of resolutions only. What the pages agree on is graded: the four sprite resolution modes on the three display resolutions. Attachment at 32 or 64 bits is left open. |
| Each individual 35 ns sprite pixel (AA p.31 "4) 35ns") | The harness drops every second pixel of the fastest resolution. A 35 ns sprite pixel can be shown only through what the page implies for the visible half: the sprite's extent and the loss of pixel-to-pixel alternation (P5). Which of each pair of 35 ns pixels is visible is a property of the harness, not of the page. |
| "init. to 1" as the effect of the RST_ pin (AA p.1, p.16) | AA p.2 and p.16 tie the value to LISA's RST_ input going low. The test cannot see that pin; it takes the harness's system reset as that event (P9, P10, precondition R1). |

No claim needs a disk.

---

## 2. What the pages say, and where they leave things open

**Sprite resolution modes.** AA p.31: "All sprites can now be displayed in
either: 1) ECS default mode 2) 140ns (this is not ECS mode!) 3) 70ns 4) 35ns on
any display resolution. eg 35ns sprites on a lores screen, or 140ns sprites on
a superhires screen." The register is BPLCON3 ($106), bits 7 (SPRES1) and 6
(SPRES0) (AA p.16 table). AA p.17-18:

    SPRESx= determine resolution of all 8 sprites (x=0,1):
    SPRES1 | SPRES0 | SPRITE RESOLUTION
       0   |   0    | ECS defaults(LORES,HIRES=140nS,SHRES=70nS
       0   |   1    | LORES(140nS)
       1   |   0    | HIRES(70nS)
       1   |   1    | SHRES(35nS)

The ECS-default row is corroborated independently by AA p.28-29 (SPRxDATA/B):
"In lowres mode, 1 sprite pixel is 1 bitplane pixel wide. In HRES and SHRES
mode, 1 sprite pixel is 2 bitplane pixels." With AA p.17 "SHRES= super-hi-res
mode (35nS pixel width)" and HRM p.300 ("35ns pixel display rate - twice the
horizontal resolution of Hires mode, and four times the Lores rates"), the
sprite pixel width is:

| SPRES1,SPRES0 | LORES screen | HIRES screen | SHRES screen |
|---|---|---|---|
| 00 (ECS default) | 140 ns | 140 ns | 70 ns |
| 01 | 140 ns | 140 ns | 140 ns |
| 10 | 70 ns | 70 ns | 70 ns |
| 11 | 35 ns | 35 ns | 35 ns |

AA p.2 and p.5 say the same in words ("Sprite resolution can be set to
LORES,HIRES,SHRES,independent of bitplane resolution"; "LISA can globally set
sprite resolution to LORES,HIRES, or SHRES").

**Sprite width.** AA p.31: "Sprites are either 16, 32, or 64 bits wide." AA
p.3: "Bits SPAGEM and SPR32 in FMODE [control] whether size of sprite load data
in SPR0DATA(B) thru SPR7DATA(B) is 16,32, or 64 bits, analagous to bitplanes."
FMODE is $1FC, SPAGEM bit 3, SPR32 bit 2 (AA p.23). AA p.24:

    SPAGEM SPR32    Sprite Fetch Increment  Memory Cycle    Bus Width
       0      0     by 2 bytes  (as before)  normal CAS        16
       0      1     by 4 bytes                normal CAS       32
       1      0     by 4 bytes                double CAS       16
       1      1     by 8 bytes                double CAS       32

The page does not print the width per setting. This specification reads the
increment as the size of each sprite data load, as AA p.15 does for bitplanes
("the completion of all bit planes for that word (16/32/64 pixels)"): 00 gives
16 pixels, 01 and 10 give 32, 11 gives 64.

Open: the pages do not say how the two control words (SPRxPOS, SPRxCTL) sit in
a 4- or 8-byte fetch, nor which 16-bit word of a wide data fetch appears
leftmost. The data layout below repeats each control word across the whole
fetch, so no expectation depends on which word the chip takes. Where a check
needs distinct words (P6 pattern rows), it leaves the position open.

**Attachment.** AA p.31: "Sprites can be attached in any mode (formerly could
not attach sprites in the ECS SHRES 35nS resolution mode)." AA p.2: "Attached
Sprites are now available in all resolutions." The attach bit is SPRxCTL bit 7,
odd sprites only (AA p.28). Colour numbers of an attached pair are the
Manual's (HRM p.121-122, Table 4-5): the odd sprite's high-order word gives
bit 3, its low-order word bit 2, the even sprite's high-order word bit 1, its
low-order word bit 0; value v selects register 16+v, and 0 is transparent.
Low-order word = SPRxDATA = first word of each line in memory (HRM p.105,
p.126; AA p.29: "DATA bits are LSBs of the pixels").

**Colour banks.** AA p.31: "Can use any bank of 16 colours from the 256 colour
palette for the sprite colours." BPLCON4 ($10C; AA p.8, p.16) bits 7-4 ESPRM7-4
and bits 3-0 OSPRM7-4. AA p.18: "ESPRMx= 4 bit field provides the 4 high order
color table address bits for even sprites:SPR0,SPR2,SPR4,SPR6. Default value is
0001 binary.(x=7-4)"; OSPRMx the same for SPR1,3,5,7. AA p.4: "In the case of
attached sprites OSPRM bits are used." The low four bits are the Manual's
grouping (HRM p.103, Figure 4-6: sprites 2p and 2p+1 use registers
16+4p+1..3). So:

- unattached sprite k, pixel value v (1..3): colour index = BANK*16 + (k>>1)*4 + v,
  BANK = ESPRM for even k, OSPRM for odd k;
- attached pair, 4-bit value v (1..15): colour index = OSPRM*16 + v.

With both fields 0001 this reproduces the Manual's registers 17..31. AA p.2
calls the two fields "4-bit mask fields" that "do the same" as the bitplane
mask, which AA p.3 says is XOR'ed. Read as an XOR on the Manual's register
numbers 16..31, the default 0001 would move sprites to registers 0..15, which
contradicts p.18's "provides the 4 high order color table address bits" and
the default's purpose; read as an XOR on a 4-bit sprite colour number it gives
the same index as p.18. This specification uses p.18.

**Reset values.** AA p.1 (revision list): "6/07/91 corrected ESPRM4 & OSPRM4 to
be init. to 1". AA p.16 table, BPLCON4 column: "ESPRM4=1", "OSPRM4=1", all other
BPLCON4 bits "=0", legend "=0/=1 bit values initialized by RST_ pin going low."
AA p.18: "Default value is 0001 binary." Three pages agree. AA p.2 and p.4 say
RST_ "resets all the bits" of BPLCON3, BPLCON4, CLXCON2, DIWHIGH and FMODE; they
give no values, and p.16's legend defines the reset state as the printed =0/=1
values (it also prints PF2OF1=1, PF2OF0=1 in BPLCON3, the offset p.17 calls
"(default)"). This is read as agreement, not as a conflict.

Open: no page says how soon a write to BPLCON4 takes effect on the display.
P8 changes BPLCON4 only on a line where no sprite pixel is shown.

**Addresses.** BPLCON4 is $10C on AA p.8 and p.16. AA p.18 prints the heading
"CLXCON2 P 10C", while AA p.8 gives CLXCON2 at $10E. All pages agree on $10C
for BPLCON4; this package never writes CLXCON2.

---

## 3. Common setup

### 3.1 System

AA chip set selected (`Config.chipset_config = 0x10`), PAL, 2 MB Chip memory,
no interlace. Every case runs on both CPU cores (the 68000, and the second core
in its default 68000 behaviour); no expectation depends on the CPU, and both
must give the same verdicts.

### 3.2 Palette

Every case loads all 256 colour registers with distinct values that survive
the 4-bit output:

    colour(i) = $0RGB with R = i>>4, G = i & $F, B = $A        (i = 0..255)

written as a COLORxx word with bit 15 (T) = 0. With LOCT = 0 a write to the
high-order palette also loads the low-order one (AA p.5: "Loading the MSB always
loads the LSB as well"; AA p.19: "the 4 bit values are automatically extended
to 8 bits"), so each 8-bit gun holds its nibble twice and any 4 bits of it the
output shows give R, G, B back. An output sample (r, g, b) with b = $A decodes
to palette index 16*r + g. Samples with b != $A are not palette colours
(blanking, sync).

Loading order (AA p.5 BANK table): for n = 0..7, write BPLCON3 = (n<<13) | $0C00
(BANK = n, PF2OF = 011 as reset, LOCT = 0, all else 0), then COLOR00..COLOR31
($180..$1BE) = colour(32n + j), j = 0..31.

### 3.3 Program skeleton

The program is loaded into Chip memory and runs from reset. Unless a case says
otherwise:

1. INTENA ($09A) = $7FFF, DMACON ($096) = $7FFF (bit 15 = 0: clear every
   enable; AA p.22, p.25).
2. Load the palette (3.2).
3. BPLCON0 ($100) = case value; BPLCON1 ($102) = $0000; BPLCON2 ($104) = $0024;
   BPLCON3 ($106) = $0C00 | (SPRES << 6); BPLCON4 ($10C) = case value (never
   written in case R); FMODE ($1FC) = case value (bits 15-4 and 1-0 always 0);
   BPL1MOD ($108) = BPL2MOD ($10A) = 0; DIWSTRT ($08E) = $2C81, DIWSTOP ($090) =
   $2CC1 (PAL standard, HRM p.59); DDFSTRT ($092) / DDFSTOP ($094) = $0038 /
   $00D0 on LORES, $003C / $00D4 on HIRES and SHRES (HRM p.60 standard values;
   the Manual gives none for SHRES, and no check depends on the exact fetch
   window). DIWHIGH is not written; the DIWSTRT/DIWSTOP writes reset it (AA
   p.4). All "x" bits are written 0 (AA p.16: "drive to 0 for upward
   compatibility").
   BPLCON0: LORES $1200, HIRES $9200, SHRES $1240 (one bitplane; HIRES bit 15 is
   0 with SHRES, HRM p.300; BPU3 = 0; ECSENA = 0).
   BPLCON2 = $0024 keeps RDRAM (bit 8) and KILLEHB at 0; no sprite overlaps a
   non-zero playfield pixel, so no check depends on the priority code.
4. Prepare the data (3.4, 3.5) and the Copper list (3.6) in Chip memory, or
   load them as part of the image.
5. COP1LCH/COP1LCL ($080/$082) = list address, strobe COPJMP1 ($088), and only
   then DMACON = $83A0 (SET, DMAEN, BPLEN, COPEN, SPREN). AA p.21: "It is
   important that one of the jump registers be initalized and it's jump strobe
   address hit, after power up but before Coprocessor DMA is initalized."
6. Write the done marker (a fixed word, e.g. $600D) to the address DONE, then
   loop on a branch to itself.

The harness runs each case for at least three complete frames after the frame
in which the DONE write appears.

### 3.4 Bitplane data (calibration rows)

One bitplane, BPL1PT = a word-aligned area of 48 KiB, all zero except its first
640 bytes, which hold the word $CCCC. Displayed from the first line of the
display window (line $2C), these 640 bytes fill the first rows (16 lines at the
20 words per line of standard LORES, 8 at the 40 of HIRES, fewer at SHRES).
These rows are the **calibration rows**; every sprite in the package starts at
line $40 or lower on the screen, below them.

### 3.5 Sprite data structures

Every structure is in Chip memory at an address that is a multiple of 8.
Positions: SPRxPOS = (VSTART & $FF) << 8 | (HSTART >> 1); SPRxCTL =
(VSTOP & $FF) << 8 | ATT << 7 | (HSTART & 1) (AA p.28, HRM p.107). All VSTART
and VSTOP values in this package are below 256 and of equal parity (AA p.4's
note on vertical start and stop parity is thereby met, though scan-doubling
stays off: FMODE bit 15 = 0). HSTART is in low-resolution pixels; the new 70 ns
and 35 ns position bits SH1, SH0 (SPRxCTL bits 4, 3) are always 0, so every
sprite starts on a 140 ns boundary.

Layouts, per the fetch increment of AA p.24:

- 16-bit (SPAGEM,SPR32 = 00): POS, CTL; per line DATA, DATB; end $0000,$0000
  (HRM p.105, Table 4-1).
- 32-bit (01 or 10): [POS,POS] [CTL,CTL]; per line [DATA w0,w1] [DATB w0,w1];
  end [0,0] [0,0].
- 64-bit (11): [POS x4] [CTL x4]; per line [DATA w0..w3] [DATB w0..w3]; end
  [0 x4] [0 x4].

Sprite and bitplane pointers are written by the Copper at the top of every
frame (AA p.28: "These address registers must be initialized by the processor
or coprocessor every vertical blank time"; AA p.15 for BPLxPT; HRM p.110).

### 3.6 Copper list

    MOVE BPL1PTH ($0E0), MOVE BPL1PTL ($0E2)
    MOVE SPRxPTH ($120+4x), MOVE SPRxPTL ($122+4x)       x = 0..7
    [case-specific WAIT/MOVE pairs]
    WAIT $FFFF,$FFFE                                     (end of list)

MOVE: first word = register offset, bit 0 = 0; second word = data. WAIT: first
word = VP7-0 in bits 15-8, HP8-2 in bits 7-1, bit 0 = 1; second word $FFFE
(BFD set, all compare bits enabled, bit 0 = 0) (AA p.20). Every destination is
above $7E, so COPCON's CDANG is not needed (AA p.19). COP1LC restarts the list
at every vertical blank (AA p.21).

### 3.7 Reference frame

- **Frame, row, sample.** A frame of the video output lies between two vertical
  syncs, a row between two horizontal syncs, a sample is one recorded pixel of a
  row. With the line doubler, one beam line may give more than one row; no
  check depends on how many.
- **Graded frame.** The last complete frame of the run.
- **u.** The number of samples per 70 ns of display, a harness constant. It is
  measured in the HIRES calibration rows, where $CCCC gives runs of two HIRES
  pixels (140 ns) of colour(1) and colour(0): each run is 2u samples. Anchor:
  AA p.17-18 print the pixel widths (LORES 140 ns, HIRES 70 ns, SHRES 35 ns);
  the harness shows one of every two 35 ns pixels, i.e. one visible pixel per
  70 ns. Runs without a HIRES screen use the u of the package's HIRES runs
  (M-H-*). In every run the calibration rows must confirm u: runs of 4u (LORES),
  2u (HIRES), u (SHRES). In SHRES the two 35 ns pixels of $CC pairs make 70 ns
  runs, so consecutive visible pixels alternate whichever phase is dropped.
- **Why counts are exact.** Any 2k consecutive 35 ns pixels contain exactly k
  visible ones, whatever the phase of the dropped pixel. So a run of sprite
  pixels of total length T ns gives exactly (T/70)*u samples, and a group of
  pixels spanning 70 ns gives exactly u samples in order.
- **Background.** colour(0), shown in border and in playfield zero pixels
  (BRDRBLNK = 0). A sample is **opaque** if it decodes to an index other than
  0.
- **Band.** A maximal set of consecutive rows below the calibration rows that
  contain opaque samples. Bands are numbered from the top, starting at 0.
- **Segment.** A maximal run of opaque samples within a row. Segments are
  numbered from the left, starting at 0. L denotes a segment's first sample.

### 3.8 Common preconditions

Each run asserts these before grading. A failed precondition makes the run
inconclusive, not green.

| Id | Precondition | How the test checks it | Contrasting case (precondition must fail, program still finishes) |
|---|---|---|---|
| C1 | The program reached its end. | The trace holds the CPU write of the done word to DONE. | CP-1: case M-S-01 without step 6's write. |
| C2 | The display is in steady state. | The graded frame and the frame before it are identical in every calibration row and every band row. | CP-2: case M-S-01 where, after step 6, the program polls INTREQR ($01E) bit 5 (VERTB), clears it via INTREQ ($09C) = $0020, and toggles COLOR00 between colour(0) and colour(2) in every vertical blank. |
| C3 | The display resolution is the case's, and the colour path is direct. | The calibration rows contain only colour(0) and colour(1) between the first and last colour(1) sample, alternating in runs of 4u (LORES), 2u (HIRES) or u (SHRES), ignoring the first and last run of each row. | CP-3: case M-S-01 with BPLCON0 = $9200 (HIRES) instead of $1240: runs of 2u where C3 expects u. |
| C4 | Sprite DMA used the prepared structures. | For every sprite structure of the case, the trace of the graded frame holds a DMA read of its first word. | CP-4: case M-S-01 with DMACON = $8380 (SPREN missing): no such reads. |
| C5 | No sprite is clipped by the display window. | Rows are aligned at horizontal sync, so sample indices compare across rows. In the calibration rows, the first colour(1) sample lies left of the leftmost sprite segment's L, and the last colour(1) sample lies at or right of the rightmost segment's L plus its expected length (from the proposition). | CP-5: case M-L-00 with DIWSTRT = $2CB0 (window starts at horizontal position $B0, inside the sprites that start at $A0): the calibration pattern starts right of the sprites' L. |

The CP cases need only to be run once each, on the case named.

---

## 4. Sprite resolution modes (P1 to P5)

### 4.1 Cases M-<res>-<spres> (twelve runs)

For every display resolution res in {L = LORES, H = HIRES, S = SHRES} and every
SPRES1,SPRES0 in {00, 01, 10, 11}: skeleton 3.3 with that BPLCON0, BPLCON3 =
$0C00 | (SPRES << 6), BPLCON4 = $0011, FMODE = $0000 (16-bit sprites).

All eight sprites, stacked: sprite k (k = 0..7) has VSTART = $50 + 10k, VSTOP =
VSTART + 6, HSTART = $A0, ATT = 0. Its six lines are:

- lines 0-2 (**solid rows**): DATA = $FFFF, DATB = $FFFF (value 3 in all 16
  pixels);
- lines 3-5 (**alternating rows**): DATA = $FFFF, DATB = $AAAA (values 3,1,3,1,…
  from the left; pixel 0 is value 3).

The Copper list is 3.6 with no case-specific part.

**Observation.** Graded frame, bands 0..7 (band k = sprite k). The band's rows
split into a first half (solid) and second half (alternating) by beam line; a
band's row count is 6r, r being the rows per beam line.

**Common checks per band and row** (W from the table in 2, in samples:
140 ns → 32u, 70 ns → 16u, 35 ns → 8u for 16 pixels):

- (a) the row has exactly one segment, and its length is W;
- (b) all rows of the band have the same L;
- (c) solid rows: the segment has one colour, c3;
- (d) alternating rows: at 140 ns, the segment is 16 runs of 2u samples,
  alternating c3 and a colour c1 != c3, starting with c3; at 70 ns, 16 runs of
  u samples, same alternation; at 35 ns, one colour throughout (c3 or c1; the
  page cannot say which, since the harness shows only one of each pair of
  35 ns pixels).

There must be exactly 8 bands. A missing band is a failure of the proposition,
not of a precondition.

### P1. In ECS default mode every sprite has the ECS pixel width on each display resolution

- **Claims:** opus:AA-sprites.c1, opus:AA-sprites.c2, opus:AA-sprites.c6,
  sol:AA-sprites.c1; AA p.31.
- **Stimulus:** cases M-L-00, M-H-00, M-S-00.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with pixel width 140 ns on LORES and HIRES and
  70 ns on SHRES (W = 32u, 32u, 16u), for all eight sprites, on every row. From
  AA p.17 "0 | 0 | ECS defaults(LORES,HIRES=140nS,SHRES=70nS" and AA p.29 "In
  lowres mode, 1 sprite pixel is 1 bitplane pixel wide. In HRES and SHRES mode,
  1 sprite pixel is 2 bitplane pixels." Quantifiers: "All sprites" - every
  sprite 0..7; "any display resolution" - all three resolutions; one sprite or
  one resolution does not establish either.
- **Preconditions:** C1-C5.
- **Contrasting case:** the output of M-L-10 graded against P1's LORES
  expectation must fail (W = 16u, not 32u); likewise M-S-01 against P1's SHRES
  expectation (32u, not 16u).
- **Reference frame:** 3.7, u from the HIRES runs.

### P2. In 140 ns mode every sprite has 140 ns pixels on each display resolution

- **Claims:** opus:AA-sprites.c1, opus:AA-sprites.c3, opus:AA-sprites.c6,
  sol:AA-sprites.c2, sol:AA-sprites.c7; AA p.31.
- **Stimulus:** cases M-L-01, M-H-01, M-S-01.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with 140 ns pixels (W = 32u) on all three
  resolutions, all eight sprites, every row. AA p.17: "0 | 1 | LORES(140nS)".
  The cell M-S-01 is the page's own example ("140ns sprites on a superhires
  screen", sol:AA-sprites.c7) and must pass on its own.
- **Preconditions:** C1-C5.
- **Contrasting case:** M-S-00 graded against P2's SHRES expectation must fail
  (16u, not 32u).
- **Reference frame:** 3.7.

### P3. The 140 ns mode is not the ECS default mode

- **Claims:** sol:AA-sprites.c3 (and the parenthesis quoted in
  opus:AA-sprites.c3); AA p.31: "2) 140ns (this is not ECS mode!)".
- **Stimulus:** cases M-S-00 and M-S-01.
- **Observation:** 4.1, SHRES runs, each band.
- **Expected:** on a SHRES screen the two settings differ: segment length 16u
  with SPRES = 00, 32u with SPRES = 01, for every sprite. From the AA p.17 table
  (SHRES=70nS under ECS defaults; LORES(140nS) under 01). On LORES and HIRES the
  same table gives 140 ns for both settings, so the page predicts no difference
  there; the claim is graded on SHRES only.
- **Preconditions:** C1-C5 in both runs.
- **Contrasting case:** the same comparison on M-L-00 against M-L-01 (and
  M-H-00 against M-H-01): the page predicts equal lengths (32u), so a check that
  demands a difference must fail there.
- **Reference frame:** 3.7.

### P4. In 70 ns mode every sprite has 70 ns pixels on each display resolution

- **Claims:** opus:AA-sprites.c1, opus:AA-sprites.c4, opus:AA-sprites.c6,
  sol:AA-sprites.c4; AA p.31.
- **Stimulus:** cases M-L-10, M-H-10, M-S-10.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with 70 ns pixels (W = 16u), all three
  resolutions, all eight sprites, every row. AA p.17: "1 | 0 | HIRES(70nS)".
- **Preconditions:** C1-C5.
- **Contrasting case:** M-L-01 graded against P4's LORES expectation must fail
  (32u, not 16u).
- **Reference frame:** 3.7.

### P5. In 35 ns mode every sprite has 35 ns pixels on each display resolution

- **Claims:** opus:AA-sprites.c1, opus:AA-sprites.c5, opus:AA-sprites.c6,
  sol:AA-sprites.c5, sol:AA-sprites.c6; AA p.31.
- **Stimulus:** cases M-L-11, M-H-11, M-S-11.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with 35 ns pixels: W = 8u, and on alternating
  rows a single colour across the segment. AA p.18: "1 | 1 | SHRES(35nS)". The
  cell M-L-11 is the page's own example ("35ns sprites on a lores screen",
  sol:AA-sprites.c6) and must pass on its own. What cannot be established: the
  value of each individual 35 ns pixel (section 1).
- **Preconditions:** C1-C5.
- **Contrasting case:** M-L-10 graded against P5's LORES expectation must fail
  (16u and alternating runs of u, not 8u and one colour).
- **Reference frame:** 3.7. The phase of the dropped 35 ns pixel is not
  assumed; checks (a) and (d) hold for either phase.

---

## 5. Sprite width (P6)

### P6. A sprite is 16, 32 or 64 pixels wide, selected by FMODE SPAGEM and SPR32

- **Claims:** opus:AA-sprites.c7, sol:AA-sprites.c8, sol:AA-sprites.c9,
  sol:AA-sprites.c10; AA p.31, with AA p.3 and the table on AA p.24.
- **Stimulus:** cases W-00, W-01, W-10, W-11 (SPAGEM,SPR32). Skeleton 3.3 on
  LORES (BPLCON0 = $1200), SPRES = 00 (140 ns pixels by P1's table), BPLCON4 =
  $0011, FMODE = $0000, $0004, $0008, $000C. Eight sprites stacked as in 4.1
  (VSTART = $50 + 10k, VSTOP = VSTART + 6, HSTART = $A0), each in the layout of
  3.5 for its FMODE (16-bit for W-00, 32-bit for W-01 and W-10, 64-bit for
  W-11). Lines 0-2 (**solid rows**): every DATA and DATB word $FFFF. Lines 3-5
  (**pattern rows**): in each DATA and in each DATB fetch, the first word
  (lowest address) $FFFF and every other word $0000 (for W-00 this equals a
  solid row).
- **Observation:** graded frame, bands 0..7 (band k = sprite k).
- **Expected:**
  - solid rows: exactly one segment, one colour, length 32u (W-00), 64u (W-01),
    64u (W-10), 128u (W-11): 16, 32, 32, 64 pixels of 140 ns;
  - pattern rows of W-01, W-10, W-11: exactly one segment of length 32u (one
    16-bit word's pixels), starting at L + q*32u, where L is the solid rows'
    start and q is an integer with 0 <= q < 2 (32-bit) or 0 <= q < 4 (64-bit).
    The page does not say which word is leftmost, so q is not fixed; DATA and
    DATB are taken to be read the same way, since AA p.3 treats SPRxDATA and
    SPRxDATB alike. This row type shows that the wider sprite is made of
    distinct data bits, not of one 16-bit word stretched (which would give 64u
    or no opaque samples);
  - over the four cases the widths are exactly the set {16, 32, 64}, as "either
    16, 32, or 64" requires, with the mapping from AA p.24's increments (2, 4,
    4, 8 bytes). All eight sprites in every case.
- **Preconditions:** C1-C5 (C5 with the widest expected sprite, 128u from L).
- **Contrasting case:** W-00's output graded against W-01's expectation must
  fail (32u, not 64u); W-01's against W-11's (64u, not 128u).
- **Reference frame:** 3.7.

---

## 6. Attachment, colour banks, reset values (P7 to P10)

### P7. Sprites can be attached in every sprite mode on every display resolution

- **Claims:** opus:AA-sprites.c8, sol:AA-sprites.c11; AA p.31: "Sprites can be
  attached in any mode (formerly could not attach sprites in the ECS SHRES 35nS
  resolution mode)." AA p.2: "Attached Sprites are now available in all
  resolutions."
- **Stimulus:** cases A-<res>-<spres>, twelve runs as in 4.1 (same BPLCON0,
  BPLCON3, FMODE = $0000), with BPLCON4 = $0011 so that ESPRM = OSPRM = 0001
  and the result does not depend on which field an attached pair uses. Four
  attached pairs, stacked: pair p (sprites 2p and 2p+1) at VSTART = $50 + 12p,
  VSTOP = VSTART + 8, HSTART = $A0, both sprites at the same position (HRM
  p.120); ATT = 1 in sprite 2p+1's SPRxCTL (AA p.28). Eight lines, types A and
  B alternating (A first):

  | Line type | even sprite DATA | even DATB | odd DATA | odd DATB | 4-bit values of the eight 2-pixel groups |
  |---|---|---|---|---|---|
  | A | $CCCC | $3C3C | $03FC | $0003 | 1 2 3 4 5 6 7 8 |
  | B | $CCCC | $3C3C | $03FC | $FFFC | 9 10 11 12 13 14 15 0 |

  (Bit order from HRM p.121: odd high-order word = bit 3, odd low-order = bit
  2, even high-order = bit 1, even low-order = bit 0; DATA = low-order.)
- **Observation:** graded frame, bands 0..3 (band p = pair p); in each row, L =
  first opaque sample; group g (g = 0..7) is the samples [L + g*G, L + (g+1)*G),
  G = 2 sprite pixels: 4u at 140 ns, 2u at 70 ns, u at 35 ns (widths from the
  table in 2; ECS default: 4u on LORES and HIRES, 2u on SHRES). Row type by
  beam line within the band: even lines A, odd lines B.
- **Expected:** every group of every row shows colour(16 + v), v the group's
  value from the table (type B group 7: background). HRM p.122 Table 4-5 maps
  value v to register 16 + v; AA p.4 makes the bank OSPRM, which is 0001 here.
  On SHRES the palette entry is shown directly (AA p.5: "There is no longer any
  need to 'scramble' SHRES color table entries"; AA p.30: "as ECS, but with no
  colour fudging"). Quantifier "any mode": all twelve cells, each on its own,
  with all four pairs; the cell A-S-00 is the one the parenthesis says was
  formerly impossible.
- **Preconditions:** C1-C5; 4 bands present.
- **Contrasting case:** runs AX-S-00 and AX-L-11: the same as A-S-00 and A-L-11
  with ATT = 0. The sprites are then unattached; each pixel shows the lower
  numbered sprite if its 2-bit value is non-zero (HRM p.118: lowest number has
  highest priority), else the higher; colours 16 + 4p + (2-bit value) (HRM
  p.103). For every pair at least the groups with values 4..15 then differ
  from colour(16 + v), so P7's check must fail on both runs.
- **Reference frame:** 3.7; groups located from L as there.

### P8. Each of the 16 colour banks can be selected for the even sprites, the odd sprites and attached pairs

- **Claims:** opus:AA-sprites.c10, sol:AA-sprites.c13; AA p.31: "Can use any
  bank of 16 colours from the 256 colour palette for the sprite colours." AA
  p.18 (ESPRM, OSPRM definitions), AA p.4 ("In the case of attached sprites
  OSPRM bits are used").
- **Stimulus, case B-N (unattached):** skeleton 3.3 on LORES, SPRES = 00,
  FMODE = $0000, BPLCON4 = $0011 initially. Eight sprites side by side: sprite k
  at HSTART = $90 + 24k, VSTART = $40, VSTOP = $C0 (128 lines), ATT = 0. Data
  line j (j = 0..127): if j mod 8 = 0, DATA = DATB = $0000 (**gap line**), else
  DATA = $F0F0, DATB = $0FF0 (values 1, 2, 3, 0 in four 4-pixel groups). The
  Copper list's case-specific part, for b = 0..15:

      WAIT (($40 + 8b) << 8) | $07, $FFFE
      MOVE $010C, (b << 4) | (15 - b)          ESPRM = b, OSPRM = 15 - b, BPLAM = 0

  Each MOVE executes on the gap line that opens band b, so every sprite pixel
  of band b is displayed at least one line after the write (the page does not
  say how soon a write takes effect; see 2).
- **Stimulus, case B-A (attached):** as B-N, but four attached pairs: pair p
  (sprites 2p, 2p+1) at HSTART = $90 + 40p, ATT = 1 on the odd sprite; non-gap
  lines use P7's words, types A and B alternating starting with A on the first
  line after each gap line.
- **Observation:** graded frame. Bands 0..15 (band b = the seven non-gap lines
  after the b-th MOVE). B-N: 8 segments per row, segment k = sprite k, groups
  g = 0,1,2 of 8u samples from L (value v = g + 1). B-A: 4 segments per row,
  segment p = pair p, groups of 4u (2 pixels of 140 ns) as in P7.
- **Expected:**
  - B-N: every group shows colour(BANK*16 + (k>>1)*4 + v), BANK = b for even k
    and 15 - b for odd k (AA p.18 "provides the 4 high order color table address
    bits"; HRM p.103 for the low four bits);
  - B-A: every group shows colour((15 - b)*16 + v), v from P7's table (AA p.4
    OSPRM for attached; HRM p.122); a pair that used ESPRM would show
    colour(b*16 + v), which differs for every b;
  - quantifier "any bank": all 16 banks for the even field, all 16 for the odd
    field, all 16 for attached pairs, in all rows of every band.
- **Preconditions:** C1-C5; exactly 16 bands; 8 (B-N) or 4 (B-A) segments in
  every graded row; and **C6**: the trace of the graded frame holds the Copper
  fetches of all sixteen MOVE instructions of the list, in list order.
- **Contrasting case:** BX-N and BX-A: as B-N and B-A with band 5's MOVE data
  replaced by band 4's value ($004B). The page predicts band 5 in bank 4 / 11
  instead of 5 / 10, so P8 fails on band 5 and passes on the others; C6 still
  holds. Contrast for C6: BP, case B-N with the end-of-list WAIT placed after
  band 9's MOVE: the fetches of MOVEs 10..15 are absent and C6 fails.
- **Reference frame:** 3.7; bands found by the gap rows between them.

### P9. ESPRM4 is 1 after reset

- **Claims:** opus:AA-revlist.c1, sol:AA-revision.c1; AA p.1: "6/07/91
  corrected ESPRM4 & OSPRM4 to be init. to 1". Corroborated by AA p.16
  ("ESPRM4=1"; "=0/=1 bit values initialized by RST_ pin going low") and AA
  p.18 ("Default value is 0001 binary").
- **Stimulus, case R:** skeleton 3.3 on LORES, SPRES = 00, FMODE = $0000,
  **BPLCON4 never written** by the program or the Copper. Eight sprites side
  by side: sprite k at HSTART = $90 + 24k, VSTART = $50, VSTOP = $58, ATT = 0,
  every line DATA = $F0F0, DATB = $0FF0. Copper list 3.6 without a
  case-specific part.
- **Observation:** graded frame, the one band, 8 segments per row; groups g =
  0,1,2 of 8u samples from L, value v = g + 1. Decode each group's palette
  index.
- **Expected:** for the even sprites 0, 2, 4, 6, bit 4 of the decoded index is
  1 in every group of every row. This is the graded bit: the index's bit 4 is
  ESPRM4 (AA p.18). The full index the pages give for the whole field (ESPRM7-4
  = 0,0,0,1, AA p.16) is 16 + (k>>1)*4 + v; the test records any mismatch in
  bits 7-5 as a separate observation, not as a failure of this claim.
  "Initialised" is graded at the only point the test can see: the display
  after reset, before any write to BPLCON4.
- **Preconditions:** C1-C5, and:
  - **R1** No code ran before the test program: the first CPU access in the
    trace is the reset-vector read or the first instruction fetch of the
    program at its entry address, and no Copper, bitplane or sprite fetch
    precedes it. Contrast RP-1: a prologue at another address, executed first,
    that enables Copper DMA on a one-instruction list and then jumps to the
    entry: the first instruction fetch is no longer at the entry, and Copper
    fetches precede it; R1 fails.
  - **R2** Nothing wrote BPLCON4: the program has no write to $DFF10C (by
    construction), the Copper list has no MOVE whose first word is $010C, and
    every Copper fetch in the run's trace is from the list's own addresses.
    Contrast RP-2: add `MOVE $010C,$0011` to the list: R2 fails although the
    value written equals the default, which shows R2 checks the write, not the
    outcome.
  - 8 segments in every row of the band.
- **Contrasting case:** RX-E, case R with BPLCON4 = $0001 written in step 3
  (ESPRM = 0000, OSPRM = 0001): the even sprites' indices become (k>>1)*4 + v,
  bit 4 = 0, and P9 must fail; the odd sprites are unchanged, so P10 still
  passes in RX-E.
- **Reference frame:** 3.7.

### P10. OSPRM4 is 1 after reset

- **Claims:** opus:AA-revlist.c2, sol:AA-revision.c2; AA p.1 (same line as P9),
  AA p.16 ("OSPRM4=1"), AA p.18 ("OSPRMx … Default value is 0001 binary").
- **Stimulus:** case R (P9).
- **Observation:** as P9, odd sprites 1, 3, 5, 7.
- **Expected:** bit 4 of the decoded index is 1 in every group of every row of
  every odd sprite (bit 4 is OSPRM4, AA p.18). Full index per AA p.16: 16 +
  (k>>1)*4 + v; bits 7-5 recorded separately as in P9. Graded separately from
  P9, because each field can be wrong alone.
- **Preconditions:** as P9 (C1-C5, R1, R2, 8 segments).
- **Contrasting case:** RX-O, case R with BPLCON4 = $0010 (ESPRM = 0001, OSPRM
  = 0000): the odd sprites' bit 4 becomes 0 and P10 must fail; P9 still passes
  in RX-O.
- **Reference frame:** 3.7.

---

## 7. Claim index

| Claim uid | Proposition(s) |
|---|---|
| opus:AA-sprites.c1 | P1, P2, P4, P5 (the "all sprites" quantifier) |
| opus:AA-sprites.c2 | P1 |
| opus:AA-sprites.c3 | P2, P3 |
| opus:AA-sprites.c4 | P4 |
| opus:AA-sprites.c5 | P5 |
| opus:AA-sprites.c6 | P1, P2, P4, P5 (the "any display resolution" quantifier) |
| opus:AA-sprites.c7 | P6 |
| opus:AA-sprites.c8 | P7 |
| opus:AA-sprites.c10 | P8 |
| sol:AA-sprites.c1 | P1 |
| sol:AA-sprites.c2 | P2 |
| sol:AA-sprites.c3 | P3 |
| sol:AA-sprites.c4 | P4 |
| sol:AA-sprites.c5 | P5 |
| sol:AA-sprites.c6 | P5 (cell M-L-11) |
| sol:AA-sprites.c7 | P2 (cell M-S-01) |
| sol:AA-sprites.c8 | P6 (16 bits) |
| sol:AA-sprites.c9 | P6 (32 bits) |
| sol:AA-sprites.c10 | P6 (64 bits) |
| sol:AA-sprites.c11 | P7 |
| sol:AA-sprites.c13 | P8 |
| opus:AA-revlist.c1 | P9 |
| sol:AA-revision.c1 | P9 |
| opus:AA-revlist.c2 | P10 |
| sol:AA-revision.c2 | P10 |

## 8. Run list

| Run | Used by |
|---|---|
| M-L-00, M-H-00, M-S-00 | P1, P3 (S), contrasts of P2, P3 |
| M-L-01, M-H-01, M-S-01 | P2, P3 (S), contrasts of P1, P3, P4; CP-3 base |
| M-L-10, M-H-10, M-S-10 | P4, contrasts of P1, P5 |
| M-L-11, M-H-11, M-S-11 | P5 |
| W-00, W-01, W-10, W-11 | P6 and its contrasts |
| A-<res>-<spres> (12) | P7 |
| AX-S-00, AX-L-11 | P7 contrast |
| B-N, B-A | P8 |
| BX-N, BX-A, BP | P8 contrasts, C6 contrast |
| R | P9, P10 |
| RX-E, RX-O | P9, P10 contrasts |
| RP-1, RP-2 | R1, R2 contrasts |
| CP-1 … CP-5 | C1 … C5 contrasts |
