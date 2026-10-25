# Specification: AA sprite modes, sprite widths, attachment, sprite colour banks and their reset values (unit aa-s6)

This document specifies one integration test package. The oracle for every
expected value is the pages of Commodore's *Functional Specification for the
Advanced Amiga Chip Set (AA)* (cited as "AA p.N", N being the corpus page
number; the document prints none); a page of the *Amiga Hardware Reference
Manual*, 3rd edition (cited as "HRM p.N", printed page) is cited only for
behaviour the AA page takes over unchanged (the sprite data structure, the
colour-register grouping of sprite pairs, the attached-sprite colour numbers,
standard display-window and data-fetch values).

The package has seventeen propositions: P1 to P12, and P2a, P5a, P6a, P6b and
P6c, each of which grades one cell or one width of P2, P5 or P6 on its own.

---

## 1. Claims not turned into a proposition

None. All 25 claims support one of the propositions below (index in section 7).

Parts of claims that no test grades, and why:

| Claim part | Why it is not graded |
|---|---|
| Each individual 35 ns sprite pixel (AA p.31 "4) 35ns") | At one sample per 70 ns of display (u = 1, 3.7) each sample shows one of each pair of 35 ns pixels. A 35 ns sprite pixel can then be shown only through what the page implies for the visible half: the sprite's extent and the loss of pixel-to-pixel alternation (P5). Which of each pair of 35 ns pixels is visible is a property of the harness, not of the page. |
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
p.126; AA p.29: "DATA bits are LSBs of the pixels"). "Any mode" follows the
width sentence, while its parenthesis and AA p.2 speak of resolutions; P7
grades both: the four sprite resolution modes on the three display
resolutions, and the three wider sprite widths of FMODE (AA p.3, p.24).

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

AA chip set selected (`Config.chipset_config = 0x10`), 2 MB Chip memory (the
harness default), no interlace. The video standard is the one the harness gives
after reset; the program writes no BEAMCON0. The display-window values of 3.3
are the PAL ones of HRM p.59; no check depends on the standard, since every
graded line lies between $2C and $C0.

Every case runs on both CPU builds of `nanomig.mk`, with `cpu_config` = 0
(68000 behaviour), and both builds are graded. The chip set is selected by the
`chipset_config` input of `nanomig.v`, not by the build: bit 4 selects AA on
either core. The build define `ENABLE_AGA`, which `nanomig.mk` gives the TG68K
build only, is read by no file of the chip set or of `nanomig.v`. Each build measures its own u and r (3.7) on its own M-H-00 run.

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
2. BPLCON2 ($104) = $0024, then load the palette (3.2). BPLCON2 is written
   first so that RDRAM (bit 8) is 0 before any colour register write (AA p.5:
   "RDRAM bit in BPLCON2 causes LISA to interpret all color table accesses as
   reads").
3. BPLCON0 ($100) = case value; BPLCON1 ($102) = $0000;
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
4. Prepare the data (3.4, 3.5) and the Copper list (3.6) at the addresses of
   3.9, by program writes or as part of the image.
5. COP1LCH/COP1LCL ($080/$082) = list address, strobe COPJMP1 ($088), and only
   then DMACON = $83A0 (SET, DMAEN, BPLEN, COPEN, SPREN). AA p.21: "It is
   important that one of the jump registers be initalized and it's jump strobe
   address hit, after power up but before Coprocessor DMA is initalized."
6. End with the `DONE` macro of `nanomig.i`: it writes DONE_VALUE ($600D) to
   DONE_ADDR ($0C0FFE), then loops on a branch to itself.

The harness runs each case for at least three complete frames after the frame
in which the DONE write appears.

### 3.4 Bitplane data (calibration rows)

One bitplane, BPL1PT = the 48 KiB bitplane area at $090000 (3.9), all zero
except its first 640 bytes, which hold the word $CCCC. Displayed from the
first line of the display window (line $2C), these 640 bytes fill the first
rows (16 lines at the 20 words per line of standard LORES, 8 at the 40 of
HIRES, fewer at SHRES).
These rows are the **calibration rows**; every sprite in the package starts at
line $40 or lower on the screen, below them.

### 3.5 Sprite data structures

Every structure is at the address 3.9 gives it, a multiple of 8.
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
  row. The scan doubler is always on (`nanomig_harness.h`, `find_lines`), so
  one beam line gives more than one row; r counts them.
- **Sample index.** A sample's index counts from the rising hs edge that starts
  its row. Indices compare across rows only if every row starts at the same
  point of its beam line; C7 asserts that on the calibration rows, whose first
  colour(1) sample lies at the same beam position on every line (the same
  DIWSTRT and DDFSTRT, HRM p.59-60, and the same data).
- **r.** The number of rows per beam line, a harness constant. It is measured
  in the HIRES calibration rows: 640 bytes at the 40 words per line that
  DDFSTRT $3C and DDFSTOP $D4 fetch (HRM p.60: "DDFSTRT = DDFSTOP - (4 * (word
  count - 2)) for high resolution") are 8 beam lines, so the HIRES calibration
  rows number 8r. Runs without a HIRES screen use the r of the package's HIRES
  runs (M-H-*); C7 confirms it in every LORES run (20 words per line, "DDFSTRT
  = DDFSTOP - (8 * (word count - 1)) for low resolution": 16 beam lines, 16r
  rows). The pages give no word count for SHRES, so C7 takes the words per row
  from the SHRES calibration rows themselves and requires the row count to
  follow from it. Beam line m of a band is its rows [m*r, (m+1)*r), counted from the
  band's first row.
- **Graded frame.** The last complete frame of the run.
- **u.** The number of samples per 70 ns of display, a harness constant. It is
  measured in the HIRES calibration rows, where $CCCC gives runs of two HIRES
  pixels (140 ns) of colour(1) and colour(0): each run is 2u samples. Anchor:
  AA p.17-18 print the pixel widths (LORES 140 ns, HIRES 70 ns, SHRES 35 ns).
  If u = 1 (one sample per 70 ns of display), each sample shows one of every
  two 35 ns pixels; which one is a property of the harness, not of the page.
  Runs without a HIRES screen use the u of the package's HIRES runs (M-H-*). In
  every run the calibration rows must confirm u: runs of 4u (LORES), 2u
  (HIRES), u (SHRES). In SHRES the two 35 ns pixels of $CC pairs make 70 ns
  runs, so the SHRES runs are u samples long whichever of each pair is shown.
- **Why counts are exact.** With u = 1, any 2k consecutive 35 ns pixels contain
  exactly k shown ones, whatever the phase of the dropped pixel; with an even u
  every 35 ns pixel has u/2 samples of its own. Either way a run of sprite
  pixels that starts on a 140 ns boundary and lasts T ns (a multiple of 70)
  gives exactly (T/70)*u samples, and a group of pixels spanning 70 ns gives
  exactly u samples in order. An odd u above 1 gives a 35 ns pixel no whole
  number of samples; the 35 ns checks of (d) are then recorded, not graded.
- **Background.** colour(0), shown in border and in playfield zero pixels
  (BRDRBLNK = 0). A sample is **opaque** if it decodes to an index other than
  0.
- **Band.** A maximal set of consecutive rows below the calibration rows that
  contain opaque samples. Bands are numbered from the top, starting at 0.
- **Segment.** A maximal run of opaque samples within a row. Segments are
  numbered from the left, starting at 0. L denotes a segment's first sample.

### 3.8 Common preconditions

Each run asserts these before grading. A failed precondition makes the run
inconclusive, not green. C7 is a precondition of every run.

The RAM-port log (`nanomig_harness.h`, `RamAccess`) records address, data and
direction, not which unit made the access. A fetch or read named below is a
read at the address of the structure it names (3.9); after the DONE write the
CPU reads only its closing loop.

| Id | Precondition | How the test checks it | Contrasting case (precondition must fail, program still finishes) |
|---|---|---|---|
| C1 | The program reached its end. | The trace holds the CPU write of DONE_VALUE to DONE_ADDR. | CP-1: case M-S-01 without step 6's write. |
| C2 | The display is in steady state. | The graded frame and the frame before it are identical in every calibration row and every band row. | CP-2: case M-S-01 where, after step 6's write and in place of the branch to itself, the program polls INTREQR ($01E) bit 5 (VERTB), clears it via INTREQ ($09C) = $0020, and toggles COLOR00 between colour(0) and colour(2) in every vertical blank. |
| C3 | The display resolution is the case's, and the colour path is direct. | The calibration rows contain only colour(0) and colour(1) between the first and last colour(1) sample, alternating in runs of 4u (LORES), 2u (HIRES) or u (SHRES), ignoring the first and last run of each row. | CP-3: case M-S-01 with BPLCON0 = $9200 (HIRES) instead of $1240: runs of 2u where C3 expects u. |
| C4 | Sprite DMA used the prepared structures. | For every sprite structure of the case, the trace of the graded frame holds a read at the address of its first word. | CP-4: case M-S-01 with DMACON = $8380 (SPREN missing): no such reads. |
| C5 | No sprite is clipped by the display window. | Sample indices compare across rows (C7). The window is observed in the calibration rows: W_L is the index of their first colour(1) sample and W_R the smallest index of their last colour(1) sample. The window's left edge is DIWSTRT's horizontal value and a sprite's left edge is its HSTART, in the same units (HRM p.59, HRM p.99-100: a sprite's X value is counted from the window's left edge, $81); one LORES pixel of 140 ns is 2u samples (3.7). For every one of the eight sprites of the case, whether or not it shows in the output, the programmed left edge W_L + (HSTART - DIWSTRT_H)*2u lies at or right of W_L, and the programmed right edge, that plus the expected length (from the proposition), lies at or left of W_R. The sprites in the output are not read. | CP-5: case M-L-00 with DIWSTRT = $2CB0 (window starts at horizontal position $B0, to the right of the sprites' HSTART $A0): the programmed left edge lies left of W_L. |
| C7 | The row grid holds: rows map to beam lines r at a time, and sample indices compare across rows (3.7). | HIRES runs: the calibration rows number 8r for a whole number r >= 1. LORES runs: they number 16r, with the r of the HIRES runs. SHRES runs: every calibration row holds the same number n of runs between its first and last colour(1) sample, and n + 1 (those runs and the colour(0) pair that follows the last of them) is eight times a whole number w, since each word $CCCC gives eight runs of two SHRES pixels; a row then shows w words, the 320 words of the 640 bytes of 3.4 fill 320/w beam lines, and the calibration rows number (320/w)*r, with the r of the HIRES runs. Every run: the first colour(1) sample has the same index in every calibration row. | CP-7a: case M-L-00 with 1280 bytes of $CCCC (32 beam lines, $2C-$4B, above the first band at $50): 32r calibration rows where C7 expects 16r. CP-7a-S: case M-S-01 with 1280 bytes of $CCCC: twice the rows where C7 expects (320/w)*r. CP-7b: case M-L-00 whose Copper list adds, after the pointer MOVEs, MOVE BPLCON1 ($102) = $0000, WAIT $3007,$FFFE and MOVE $0102,$0033 (PF1H5-2 = PF2H5-2 = 0011, AA p.16 table; AA p.5: the old PF1H0 "now it's called PF1H2", the 140 ns step): from line $30 the playfield is delayed by 3 LORES pixels, so the first colour(1) sample of the later calibration rows lies 6u further right, while C3's runs and the row count stay. |

The CP cases need only to be run once each, on the case named.

### 3.9 Memory layout

Every address the package fixes lies in the Chip memory that `nanomig.i` names
identity-mapped, $080000-$1FFFFF. Below $080000 the ROM overlay answers after
reset (HRM p.223: "The OVL bit in one of the 8520 Chips disables the overlay"),
and `nanomig_harness.h` notes that a CPU write there reaches no memory while
the overlay is set. No program in this package writes CIA-A, and nothing is
placed below $080000.

| Use | Address | Size |
|---|---|---|
| Program image (`PROGRAM_ORG`, `nanomig.i`) | $080400-$087FFF | code; data assembled into the image sits only at the addresses below |
| Copper list | $088000-$0880FF | at most 204 bytes (3.6 with P8's sixteen WAIT/MOVE pairs) |
| Prologue Copper list (RP-1 only) | $088100 | one instruction |
| Sprite structure of sprite k (k = 0..7) | $089000 + $400*k | at most 520 bytes (B-N, B-A); every start a multiple of 8 (3.5) |
| Bitplane area (3.4) | $090000-$09BFFF | 48 KiB |
| RESULTS, DONE_ADDR (`nanomig.i`) | $0C0000-$0C0FFF | the done word at $0C0FFE |
| Stack (`PROGRAM_STACK`, `nanomig.i`) | below $0FFF00 | |

No two uses overlap.

---

## 4. Sprite resolution modes (P1 to P5, P2a, P5a, P11, P12)

### 4.1 Cases M-<res>-<spres> (twelve runs)

For every display resolution res in {L = LORES, H = HIRES, S = SHRES} and every
SPRES1,SPRES0 in {00, 01, 10, 11}: skeleton 3.3 with that BPLCON0, BPLCON3 =
$0C00 | (SPRES << 6), BPLCON4 = $0011, FMODE = $0000 (16-bit sprites).
"Any display resolution" (AA p.31) is graded over the three resolutions
BPLCON0 selects, which AA p.2 calls "all 3 resolutions"; VGA is a display
mode (AA p.30 lists it under "NEW LISA DISPLAY MODES"), not a resolution.

All eight sprites, stacked: sprite k (k = 0..7) has VSTART = $50 + 10k, VSTOP =
VSTART + 6, HSTART = $A0, ATT = 0. Its six lines are:

- lines 0-2 (**solid rows**): DATA = $FFFF, DATB = $FFFF (value 3 in all 16
  pixels);
- lines 3-5 (**alternating rows**): DATA = $FFFF, DATB = $AAAA (values 3,1,3,1,…
  from the left; pixel 0 is value 3).

The Copper list is 3.6 with no case-specific part.

**Observation.** Graded frame, bands 0..7 (band k = sprite k). A band's row
count is 6r (r from 3.7, confirmed by C7); its beam lines 0-2 (rows [0, 3r))
are solid rows, beam lines 3-5 (rows [3r, 6r)) alternating rows.

**Common checks per band and row** (W from the table in 2, in samples:
140 ns → 32u, 70 ns → 16u, 35 ns → 8u for 16 pixels):

- (a) the row has exactly one segment, and its length is W;
- (b) all rows of the band have the same L;
- (c) solid rows: the segment has one colour, c3;
- (d) alternating rows: at 140 ns, the segment is 16 runs of 2u samples,
  alternating c3 and a colour c1 != c3, starting with c3; at 70 ns, 16 runs of
  u samples, same alternation; at 35 ns with u = 1, one colour throughout (c3
  or c1: each sample shows one of each pair of 35 ns pixels, and the page cannot
  say which), and at 35 ns with an even u, 16 runs of u/2 samples, same
  alternation (3.7).

There must be exactly 8 bands. A missing band is a failure of the proposition,
not of a precondition.

### P1. In ECS default mode every sprite has the ECS pixel width on each display resolution

- **Claims:** opus:AA-sprites.c2, sol:AA-sprites.c1; AA p.31.
- **Stimulus:** cases M-L-00, M-H-00, M-S-00.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with pixel width 140 ns on LORES and HIRES and
  70 ns on SHRES (W = 32u, 32u, 16u), for all eight sprites, on every row. From
  AA p.17 "0 | 0 | ECS defaults(LORES,HIRES=140nS,SHRES=70nS" and AA p.29 "In
  lowres mode, 1 sprite pixel is 1 bitplane pixel wide. In HRES and SHRES mode,
  1 sprite pixel is 2 bitplane pixels." Quantifiers: "All sprites" - every
  sprite 0..7; "any display resolution" - all three resolutions; one sprite or
  one resolution does not establish either.
- **Preconditions:** C1-C5, C7.
- **Contrasting case:** the output of M-L-10 graded against P1's LORES
  expectation must fail (W = 16u, not 32u); likewise M-S-01 against P1's SHRES
  expectation (32u, not 16u).
- **Reference frame:** 3.7, u from the HIRES runs.

### P2. In 140 ns mode every sprite has 140 ns pixels on each display resolution

- **Claims:** sol:AA-sprites.c2; AA p.31.
- **Stimulus:** cases M-L-01, M-H-01, M-S-01.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with 140 ns pixels (W = 32u) on all three
  resolutions, all eight sprites, every row. AA p.17: "0 | 1 | LORES(140nS)".
  The cell M-S-01 is the page's own example ("140ns sprites on a superhires
  screen"); P2a grades it on its own.
- **Preconditions:** C1-C5, C7.
- **Contrasting case:** M-S-00 graded against P2's SHRES expectation must fail
  (16u, not 32u).
- **Reference frame:** 3.7.

### P2a. 140 ns sprites on a superhires screen (cell M-S-01)

- **Claims:** sol:AA-sprites.c7; AA p.31: "eg ... 140ns sprites on a superhires
  screen."
- **Stimulus:** case M-S-01.
- **Observation:** 4.1.
- **Expected:** P2's checks on M-S-01 alone: (a)-(d) with 140 ns pixels
  (W = 32u), all eight sprites, every row.
- **Preconditions:** C1-C5, C7.
- **Contrasting case:** M-S-00 graded against this expectation must fail (16u,
  not 32u).
- **Reference frame:** 3.7.

### P3. The 140 ns mode is not the ECS default mode

- **Claims:** sol:AA-sprites.c3; AA p.31: "2) 140ns (this is not ECS mode!)".
  The parenthesis is also part of opus:AA-sprites.c3, which P12 grades with
  P2.
- **Stimulus:** cases M-S-00 and M-S-01.
- **Observation:** 4.1, SHRES runs, each band.
- **Expected:** on a SHRES screen the two settings differ: segment length 16u
  with SPRES = 00, 32u with SPRES = 01, for every sprite. From the AA p.17 table
  (SHRES=70nS under ECS defaults; LORES(140nS) under 01). On LORES and HIRES the
  same table gives 140 ns for both settings, so the page predicts no difference
  there; the claim is graded on SHRES only.
- **Preconditions:** C1-C5, C7 in both runs.
- **Contrasting case:** the same comparison on M-L-00 against M-L-01 (and
  M-H-00 against M-H-01): the page predicts equal lengths (32u), so a check that
  demands a difference must fail there.
- **Reference frame:** 3.7.

### P4. In 70 ns mode every sprite has 70 ns pixels on each display resolution

- **Claims:** opus:AA-sprites.c4, sol:AA-sprites.c4; AA p.31.
- **Stimulus:** cases M-L-10, M-H-10, M-S-10.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with 70 ns pixels (W = 16u), all three
  resolutions, all eight sprites, every row. AA p.17: "1 | 0 | HIRES(70nS)".
- **Preconditions:** C1-C5, C7.
- **Contrasting case:** M-L-01 graded against P4's LORES expectation must fail
  (32u, not 16u).
- **Reference frame:** 3.7.

### P5. In 35 ns mode every sprite has 35 ns pixels on each display resolution

- **Claims:** opus:AA-sprites.c5, sol:AA-sprites.c5; AA p.31.
- **Stimulus:** cases M-L-11, M-H-11, M-S-11.
- **Observation:** 4.1.
- **Expected:** checks (a)-(d) with 35 ns pixels: W = 8u, and on alternating
  rows (d)'s 35 ns form (with u = 1, a single colour across the segment). AA
  p.18: "1 | 1 | SHRES(35nS)". The cell M-L-11 is the page's own example ("35ns
  sprites on a lores screen"); P5a grades it on its own. What cannot be
  established: the value of each individual 35 ns pixel (section 1).
- **Preconditions:** C1-C5, C7.
- **Contrasting case:** M-L-10 graded against P5's LORES expectation must fail
  (16u and alternating runs of u, not 8u and (d)'s 35 ns form).
- **Reference frame:** 3.7. The phase of the dropped 35 ns pixel is not
  assumed; checks (a) and (d) hold for either phase.

### P5a. 35 ns sprites on a lores screen (cell M-L-11)

- **Claims:** sol:AA-sprites.c6; AA p.31: "eg 35ns sprites on a lores screen".
- **Stimulus:** case M-L-11.
- **Observation:** 4.1.
- **Expected:** P5's checks on M-L-11 alone: (a)-(d) with 35 ns pixels
  (W = 8u), all eight sprites, every row.
- **Preconditions:** C1-C5, C7.
- **Contrasting case:** M-L-10 graded against this expectation must fail (16u,
  not 8u).
- **Reference frame:** 3.7.

### P11. Every sprite can be displayed in each of the four modes on each display resolution

- **Claims:** opus:AA-sprites.c1, opus:AA-sprites.c6; AA p.31: "All sprites can
  now be displayed in either: 1) ECS default mode 2) 140ns (this is not ECS
  mode!) 3) 70ns 4) 35ns on any display resolution."
- **Stimulus:** the twelve runs M-<res>-<spres>.
- **Observation:** 4.1.
- **Expected:** P1, P2, P4 and P5 all pass. Each of them grades one mode for all
  eight sprites on the three resolutions; these two claims state their
  quantifier ("All sprites"; "on any display resolution") over all four modes
  at once, so only the conjunction grades either whole.
- **Preconditions:** those of P1, P2, P4 and P5.
- **Contrasting case:** one contrast of P1, P2, P4 or P5 in place of its cell,
  e.g. M-L-10's output in place of M-L-00's: P1 fails, so P11 must fail.
- **Reference frame:** 3.7.

### P12. The 140 ns mode is available to every sprite on each display resolution and is not the ECS default mode

- **Claims:** opus:AA-sprites.c3; AA p.31: "2) 140ns (this is not ECS mode!)".
- **Stimulus:** cases M-L-01, M-H-01, M-S-01 and M-S-00.
- **Observation:** 4.1.
- **Expected:** P2 and P3 both pass. The claim states the mode (graded by P2)
  and its parenthesis (graded by P3).
- **Preconditions:** those of P2 and P3.
- **Contrasting case:** M-S-00's output in place of M-S-01's: P2's SHRES cell
  and P3 fail, so P12 must fail.
- **Reference frame:** 3.7.

---

## 5. Sprite width (P6, P6a, P6b, P6c)

### P6. A sprite is 16, 32 or 64 pixels wide, selected by FMODE SPAGEM and SPR32

- **Claims:** opus:AA-sprites.c7; AA p.31, with AA p.3 and the table on AA
  p.24. P6a, P6b and P6c grade each width on its own.
- **Stimulus:** cases W-00, W-01, W-10, W-11 (SPAGEM,SPR32). Skeleton 3.3 on
  LORES (BPLCON0 = $1200), SPRES = 00 (140 ns pixels by P1's table), BPLCON4 =
  $0011, FMODE = $0000, $0004, $0008, $000C. Eight sprites stacked as in 4.1
  (VSTART = $50 + 10k, VSTOP = VSTART + 6, HSTART = $A0), each in the layout of
  3.5 for its FMODE (16-bit for W-00, 32-bit for W-01 and W-10, 64-bit for
  W-11). Lines 0-2 (**solid rows**): every DATA and DATB word $FFFF. Lines 3-5
  (**pattern rows**): in each DATA fetch the first word (lowest address) $FFFF
  and every other word $0000; every DATB word $0000 (for W-00 a row of value 1
  over the sprite's 16 pixels, checked as its solid rows are: one segment, one
  colour, 32u).
- **Observation:** graded frame, bands 0..7 (band k = sprite k); beam lines 0-2
  of a band (rows [0, 3r), 3.7) are solid rows, beam lines 3-5 (rows [3r, 6r))
  pattern rows.
- **Expected:**
  - solid rows: exactly one segment, one colour, length 32u (W-00), 64u (W-01),
    64u (W-10), 128u (W-11): 16, 32, 32, 64 pixels of 140 ns;
  - pattern rows of W-01, W-10, W-11: exactly one segment of length 32u (one
    16-bit word's pixels), starting at L + q*32u, where L is the solid rows'
    start and q is an integer with 0 <= q < 2 (32-bit) or 0 <= q < 4 (64-bit).
    The page does not say which word is leftmost, so q is not fixed; nor does
    it say that DATA and DATB are read in the same word order, and with DATB
    all zero no expectation depends on it. This row type shows that the wider
    sprite is made of distinct data bits, not of one 16-bit word stretched
    (which would give 64u or no opaque samples);
  - over the four cases the widths are exactly the set {16, 32, 64}, as "either
    16, 32, or 64" requires, with the mapping from AA p.24's increments (2, 4,
    4, 8 bytes). All eight sprites in every case.
- **Preconditions:** C1-C5, C7 (C5 with the widest expected sprite, 128u from L).
- **Contrasting case:** W-00's output graded against W-01's expectation must
  fail (32u, not 64u); W-01's against W-11's (64u, not 128u).
- **Reference frame:** 3.7.

### P6a, P6b, P6c. Sprites 16, 32 and 64 bits wide, each width on its own

Three propositions, one assertion each:

- **Claims:** P6a: sol:AA-sprites.c8 (16 bits); P6b: sol:AA-sprites.c9 (32
  bits); P6c: sol:AA-sprites.c10 (64 bits). AA p.31: "Sprites are either 16,
  32, or 64 bits wide."
- **Stimulus:** P6a: W-00; P6b: W-01 and W-10, the two settings AA p.24 gives a
  4-byte increment; P6c: W-11. Each as in P6.
- **Observation:** as P6.
- **Expected:** P6's expectations for those cases alone: solid rows 32u (P6a),
  64u (P6b), 128u (P6c); pattern rows as in P6.
- **Preconditions:** as P6.
- **Contrasting case:** P6a: W-01's output graded against W-00's expectation
  must fail (64u, not 32u); P6b: W-00's against W-01's (32u, not 64u); P6c:
  W-01's against W-11's (64u, not 128u).
- **Reference frame:** 3.7.

---

## 6. Attachment, colour banks, reset values (P7 to P10)

### P7. Sprites can be attached in every sprite resolution mode on every display resolution, and at every sprite width

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
- **Stimulus, width runs AW-01, AW-10, AW-11:** skeleton 3.3 on LORES (BPLCON0
  = $1200), SPRES = 00 (140 ns pixels), BPLCON4 = $0011, FMODE = $0004, $0008,
  $000C (SPAGEM,SPR32 = 01, 10, 11; AA p.23-24). The same four pairs, lines and
  table, each structure in the layout of 3.5 for its FMODE (32-bit for AW-01
  and AW-10, 64-bit for AW-11), with every word of a DATA or DATB fetch equal
  to the table's word for that line type, sprite and register. Whichever word
  of a fetch appears leftmost (section 2 leaves it open), each 16-pixel part of
  the sprite then shows the table's 16 pixels, so no expectation depends on the
  word order.
- **Observation:** graded frame, bands 0..3 (band p = pair p); in each row, L =
  first opaque sample; group g is the samples [L + g*G, L + (g+1)*G), G = 2
  sprite pixels: 4u at 140 ns, 2u at 70 ns, u at 35 ns (widths from the table
  in 2; ECS default: 4u on LORES and HIRES, 2u on SHRES). g = 0..7 in the
  A-<res>-<spres> runs; in the width runs g = 0..15 (32-bit) or 0..31 (64-bit),
  and group g has the table's value for group g mod 8. Row type by beam line m
  of the band (rows [m*r, (m+1)*r), 3.7): even m type A, odd m type B.
- **Expected:** every group of every row shows colour(16 + v), v the group's
  value from the table (type B groups 7, and in the width runs every group
  with g mod 8 = 7: background, so a type B row of a width run has 2 or 4
  segments). HRM p.122 Table 4-5 maps value v to register 16 + v; AA p.4 makes
  the bank OSPRM, which is 0001 here. On SHRES the palette entry is shown
  directly (AA p.5: "There is no longer any need to 'scramble' SHRES color
  table entries"; AA p.30: "as ECS, but with no colour fudging"). The widths of
  the width runs, 32 and 64 pixels, are P6's reading of AA p.24. Quantifier
  "any mode": all twelve resolution cells and all three width runs, each on its
  own, with all four pairs; the cell A-S-00 is the one the parenthesis says was
  formerly impossible.
- **Preconditions:** C1-C5, C7 (in the width runs C5 with the expected length
  64u for AW-01 and AW-10, 128u for AW-11). A missing pair is a failure of the
  proposition, not of a precondition: the comparator requires four bands.
- **Contrasting case:** runs AX-S-00 and AX-L-11: the same as A-S-00 and A-L-11
  with ATT = 0. The sprites are then unattached; each pixel shows the lower
  numbered sprite if its 2-bit value is non-zero (HRM p.118: lowest number has
  highest priority), else the higher; colours 16 + 4p + (2-bit value) (HRM
  p.103). In every pair the groups of values 4, 8 and 12, where the even
  sprite's two bits are 0, then show the odd sprite's colour 16 + 4p + v/4,
  which differs from colour(16 + v) for every p; so P7's check must fail on
  both runs, in every pair and in both row types. Other groups may coincide
  (for p = 1 the even sprite's colours 21-23 are those P7 expects for values
  5-7). Width runs AWX-01, AWX-10, AWX-11: the same as AW-01, AW-10, AW-11
  with ATT = 0; in every 16-pixel part the groups of values 4, 8 and 12 then
  show colour(16 + 4p + v/4) in place of colour(16 + v), so P7's check must
  fail on each of the three runs.
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
  after the b-th MOVE, 7r rows). B-N: 8 segments per row, segment k = sprite k,
  groups g = 0,1,2 of 8u samples from L (value v = g + 1). B-A: 4 segments per
  row, segment p = pair p, groups of 4u (2 pixels of 140 ns) as in P7; beam
  line m of the band (rows [m*r, (m+1)*r), 3.7) is type A for even m, type B
  for odd m.
- **Expected:**
  - B-N: every group shows colour(BANK*16 + (k>>1)*4 + v), BANK = b for even k
    and 15 - b for odd k (AA p.18 "provides the 4 high order color table address
    bits"; HRM p.103 for the low four bits);
  - B-A: every group shows colour((15 - b)*16 + v), v from P7's table (type B
    group 7: background, as in P7) (AA p.4 OSPRM for attached; HRM p.122); a
    pair that used ESPRM would show colour(b*16 + v), which differs for every
    b;
  - quantifier "any bank": all 16 banks for the even field, all 16 for the odd
    field, all 16 for attached pairs, in all rows of every band.
- **Preconditions:** C1-C5, C7, and **C6**: the trace of the graded frame holds
  the Copper fetches of all sixteen MOVE instructions of the list, in list
  order. Sixteen bands, and 8 (B-N) or 4 (B-A) segments in every graded row, are
  part of the proposition: a missing band or segment is a failure of P8, not of
  a precondition.
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
- **Preconditions:** C1-C5, C7, and:
  - **R1** No code ran before the test program: in the RAM-port log the reads
    of the reset vectors (logical $000000-$000007, which the overlay maps to
    physical $780000-$780007, `kKickShadowByteBase` in `nanomig_harness.h`)
    are followed by an instruction fetch at the program's entry, the address
    of its first instruction, and no read at the address of a Copper list, a
    sprite structure or the bitplane area (3.9) precedes that fetch. Contrast
    RP-1: a prologue at PROGRAM_ORG, executed first, that enables Copper DMA
    on a one-instruction list at $088100 (3.9) and then jumps to the entry,
    placed after it: the first instruction fetch is no longer at the entry,
    and reads of that list precede it; R1 fails, and so does R2, because the
    Copper has read a list other than the case's.
  - **R2** Nothing wrote BPLCON4: the program has no write to $DFF10C (its
    listing); the Copper list has no MOVE whose first word is $010C (its words
    in memory at the end of the run); and the Copper runs no list other than
    the case's (AA p.21: COP1LC "is automatically used at the beginning of each
    vertical blank time"). The register writes are not visible at the ports of
    `nanomig.v`; the lists the Copper runs are, as its reads in the RAM-port
    log. R2 therefore grades both the register list the program carries out
    (COP1LCH and COP1LCL written only with the list's address, COP2LCH, COP2LCL
    ($084, $086) and COPJMP2 ($08A) never written) and the log of the whole
    run: no read at an address of the Copper area ($088000-$0881FF) beyond the
    words of the case's own list. Contrast RP-2: add `MOVE $010C,$0011` to the
    list: R2 fails although the value written equals the default, which shows
    R2 checks the write, not the outcome.
  The proposition itself requires one band and 8 segments in every row of it; a
  missing band or segment is a failure of P9, not of a precondition.
- **Contrasting case:** RX-E, case R with BPLCON4 = $0001 written in step 3
  (ESPRM = 0000, OSPRM = 0001): the even sprites' indices become (k>>1)*4 + v,
  bit 4 = 0, and P9 must fail; the odd sprites are unchanged, so P10 still
  passes in RX-E. In RX-E, R2's first clause reads "the program's only write to
  $DFF10C is this one"; C1-C5, C7, R1, the rest of R2 and the 8 segments still
  hold, so P9 goes red through bit 4 and not through a precondition.
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
- **Preconditions:** as P9 (C1-C5, C7, R1, R2). The band and its 8 segments per
  row are graded by the proposition, as in P9.
- **Contrasting case:** RX-O, case R with BPLCON4 = $0010 (ESPRM = 0001, OSPRM
  = 0000): the odd sprites' bit 4 becomes 0 and P10 must fail; P9 still passes
  in RX-O. R2 reads in RX-O as in RX-E (P9), and every other precondition
  still holds.
- **Reference frame:** 3.7.

---

## 7. Claim index

Each claim id is carried by one proposition, which grades it whole.

| Claim uid | Proposition |
|---|---|
| opus:AA-sprites.c1 | P11 (the "all sprites" quantifier, over the four modes) |
| opus:AA-sprites.c2 | P1 |
| opus:AA-sprites.c3 | P12 (the 140 ns mode, P2, and its parenthesis, P3) |
| opus:AA-sprites.c4 | P4 |
| opus:AA-sprites.c5 | P5 |
| opus:AA-sprites.c6 | P11 (the "any display resolution" quantifier, over the four modes) |
| opus:AA-sprites.c7 | P6 |
| opus:AA-sprites.c8 | P7 (every resolution mode and every width) |
| opus:AA-sprites.c10 | P8 |
| sol:AA-sprites.c1 | P1 |
| sol:AA-sprites.c2 | P2 |
| sol:AA-sprites.c3 | P3 |
| sol:AA-sprites.c4 | P4 |
| sol:AA-sprites.c5 | P5 |
| sol:AA-sprites.c6 | P5a (cell M-L-11) |
| sol:AA-sprites.c7 | P2a (cell M-S-01) |
| sol:AA-sprites.c8 | P6a (16 bits) |
| sol:AA-sprites.c9 | P6b (32 bits) |
| sol:AA-sprites.c10 | P6c (64 bits) |
| sol:AA-sprites.c11 | P7 (every resolution mode and every width) |
| sol:AA-sprites.c13 | P8 |
| opus:AA-revlist.c1 | P9 |
| sol:AA-revision.c1 | P9 |
| opus:AA-revlist.c2 | P10 |
| sol:AA-revision.c2 | P10 |

## 8. Run list

| Run | Used by |
|---|---|
| M-L-00, M-H-00, M-S-00 | P1, P11, P3 (S), P12 (S), contrasts of P2, P2a, P3, P12; CP-5, CP-7a, CP-7b base |
| M-L-01, M-H-01, M-S-01 | P2, P11, P12, P2a (S), P3 (S), contrasts of P1, P3, P4, P5a; CP-1 ... CP-4 base (S) |
| M-L-10, M-H-10, M-S-10 | P4, P11, contrasts of P1, P5, P5a |
| M-L-11, M-H-11, M-S-11 | P5, P11, P5a (L) |
| W-00, W-01, W-10, W-11 | P6, P6a, P6b, P6c and their contrasts |
| A-<res>-<spres> (12), AW-01, AW-10, AW-11 | P7 |
| AX-S-00, AX-L-11, AWX-01, AWX-10, AWX-11 | P7 contrasts |
| B-N, B-A | P8 |
| BX-N, BX-A, BP | P8 contrasts, C6 contrast |
| R | P9, P10 |
| RX-E, RX-O | P9, P10 contrasts |
| RP-1, RP-2 | R1, R2 contrasts |
| CP-1 … CP-5, CP-7a, CP-7b | C1 … C5, C7 contrasts |


---

## 9 What the build adds

### 9.1 Programs and placement

`s6.i` is the one program of the package; `prog_main.s` assembles it as it
stands, and the other four programs set one symbol before including it:
`prog_nofix.s` leaves out the fixture store (the contrast of the FIX lines),
`prog_nodone.s` leaves out step 6's write of DONE_VALUE (CP-1),
`prog_vbl.s` polls INTREQR bit 5 after the DONE write, clears it through INTREQ
and toggles COLOR00 between colour(0) and colour(2) in every vertical blank
(CP-2), and `prog_pro.s` places the prologue of RP-1 at PROGRAM_ORG: it writes
COP1LC = $088100, strobes COPJMP1, writes DMACON = $8280 (SET, DMAEN, COPEN)
and jumps to the entry at PROGRAM_ORG + $40, where the program of
`prog_main.s` starts. The programs are assembled for the 68000.

The program stores the fixture constant $BEEF at RESULTS ($0C0000) first, then
reads the register list from `RLIST` = $0B8000 and writes each value to its
register with one MOVE.W, in list order, then stores the CPU read of LISAID
($DFF07C) at RESULTS + 4 (reported as a MEASURED line, graded nowhere) and
executes `DONE`. The list holds, in the order of 3.3: INTENA and DMACON
cleared; BPLCON2 = $0024; the palette (3.2); BPLCON0, BPLCON1, BPLCON3,
BPLCON4 (omitted in case R), FMODE, BPL1MOD, BPL2MOD, DIWSTRT, DIWSTOP, DDFSTRT,
DDFSTOP; COP1LCH, COP1LCL, COPJMP1; DMACON = $83A0 last. `RLIST` is the one
address 3.9 does not fix; it lies in the Chip memory of 3.9, apart from every
region of that table (the list is under 1.2 KiB). The Copper list, the sprite
structures and the bit-plane area are placed in Chip memory by the test before
the program starts, at the addresses of 3.9.

`s6_base.h` builds the register list, the Copper list and the sprite structures
of a case and runs one case on one core (boot, run until the DONE write, then
until four vertical syncs have followed it; the graded frame is the last
complete frame, the frame before it the one C2 compares). A case that never
writes DONE_VALUE runs for a fixed span. `s6_cases.h` defines the cases of 4 to
6 and the contrasting variants of 3.8 (`apply_cp`). `s6_grade.h` holds the
comparators and the common preconditions. `test_aa_integ_s6.cpp` holds the
propositions, the group lines and the mutation selectors.

### 9.2 Labels

The first token of an expect label is the token `NMH_MUTATE` selects for it.
Propositions: `P1` ... `P12`, `P6a`, `P6b`, `P6c`, `P2a`, `P5a`; `P7-ctl` is the
control of P7 (9.3, item 7). The common preconditions of 3.8 are printed once
per group of runs, ahead of the propositions that use the group: `FIX-<g>` (the
program loaded, the CPU was released, the $BEEF constant was read back and a
frame was captured), `C1-<g>`, `C2-<g>`, `C3-<g>`, `C4-<g>`, `C5-<g>`,
`C7-<g>`, for the groups g = M (the twelve M runs), W (the four W runs), A (the
twelve A runs and AW-01, AW-10, AW-11), B (B-N, B-A) and R (R). Each line is
PASS when the check holds in every run of the group that the process ran; a
FAIL line is followed by a MEASURED line naming the runs. The other
preconditions: `C6-B`, `R1-R`, `R2-R`. The bands and segments of P7 to P10 are
graded by the comparators of the propositions and have no precondition line.
A proposition is graded when every run
it uses holds all common preconditions and its own; otherwise its line is a
`RECORDED: NOT GRADED` line, never PASS or FAIL. The comparator of a
proposition reports its first deviation per run in a MEASURED line.

`NMH_MUTATE=<label>` runs the contrast of the label in place of the normal
run; for the group lines the selector is the label itself, `C7B-<g>` is the
second contrast of `C7-<g>`. `NMH_GEOM=<u>,<r>` lets a mutation run take u and r
from the normal run of the same core instead of measuring them on its own
M-H-00 run; the normal run ignores it.

### 9.3 Where the build had to choose

1. **u and r** are measured on the M-H-00 calibration rows of each core (3.7):
   r = (number of calibration rows) / 8, u = (interior run of $CCCC) / 2.
2. **Reading the rows.** An output sample is a palette colour when its blue
   gun is $A, and then decodes to index 16*r + g (3.2); a sample is opaque when
   its index is not 0. A row is part of a group of rows when it holds an
   opaque sample; the first group below the vertical blanking is the
   calibration rows, the groups after it are the bands.
3. **c3 and c1.** Check (c) and (d) of 4.1 call the colour of the solid rows
   c3: it is the colour of the first solid row of the band, and every solid
   row and the first run of every alternating row must show it; c1 is the
   colour of the second run of an alternating row, equal in all odd runs and
   different from c3.
4. **Band rows.** A band must have exactly the row count of its table (6r in
   4.1 and 5, 8r in P7, 7r in P8, 8r in case R), the number of bands must
   be the one the proposition names, and the segments of a row those of the
   table (4 or 8 in P8, 8 in case R); a band with another count, a missing
   band or a missing segment is a failure of the proposition.
5. **P6.** The measured width of a case is the length of the solid-row
   segment divided by 2u; the assertion also asks that the four widths are
   16, 32, 32 and 64 and so the set {16, 32, 64}.
6. **P7 rows.** Besides the colour of every group, the comparator counts the
   segments of a row: one for type A rows, one per 16-pixel part for type B
   rows (group 7 of each part is background).
7. **P7-ctl.** The control runs the comparator of P7 on the twelve cells
   A-<res>-<spres> and on AW-11 and prints PASS; its contrasts are AX-S-00,
   AX-L-11 and AWX-11, graded through the same comparator. The contrasts
   AWX-01 and AWX-10 (ATT = 0 in the AW-01 and AW-10 runs) are the contrasts of
   P7 itself: they are graded through the comparator of P7 and print its FAIL
   line.
8. **R1.** While the CPU is held in reset the RAM port reads word address 0;
   the check starts at the first read of a reset vector and takes the first
   Chip memory read after the vector reads. **R2** is evaluated on the
   register list, on the Copper list as the run ended, and on the RAM-port log
   of the whole run (the Copper area $088000-$0881FF read only within the
   words of the case's own list). The register list is the only path by which
   prog_main.s writes a custom register; the prologue of RP-1 (prog_pro.s)
   writes COP1LC in code, and the Copper then reads its list, which the log
   shows: the R1-R contrast prints R2-R FAIL.
9. **C5.** The expected length of a sprite is the page's: pixels times
   nanoseconds per pixel of the proposition's cell; for the W runs it is the
   widest sprite, 64 pixels of 140 ns.
10. **The contrasting variants of the common preconditions** are applied to the
    runs the group line names: M-S-01 (CP-1 to CP-4) and M-L-00 (CP-5, CP-7a,
    CP-7b) as in 3.8, and by the same change to W-00, A-S-00 (CP-1 to CP-4) and
    A-L-00 (CP-5, CP-7a, CP-7b), B-N and R. CP-3 writes BPLCON0 = $9200 (HIRES)
    in a run that is not HIRES and $1200 (LORES) in a HIRES run. CP-7a places
    1280 bytes (32 lines) of $CCCC where the first sprite starts at line $50; the
    first band of B-N starts at line $41, so there it places 800 bytes (20
    lines), which end above the band. CP-7a-S places 1280 bytes where the
    SHRES run M-S-01 is graded. The contrasts of `BANDS-A`, `BANDS-B` and
    `SEGS-R` remove the sprites 6 and 7 (A), 7 (B) and 7 (R) from the structures:
    their lines hold no data, every precondition stays met, and the comparator of
    P7, of P8, and of P9 and P10 prints FAIL. The contrast of `C6-B` ends the
    Copper list with its end-of-list WAIT after band 9's MOVE. A contrast of a
    common precondition may move a neighbouring precondition too; the records of
    the selectors list the lines under "Also printing FAIL", and the line under
    test fails through its own clause in each of them.
11. **C6.** The addresses of the sixteen MOVE instructions are those of the
    case's own list (the first band's WAIT is $4007,$FFFE; band b's MOVE lies
    4b + 2 words after it); the check reads the fetches of the graded frame, in
    list order.

### 9.4 Measured values

The final run of each core (`make -k -B test`, one target per process;
out_tg68k.txt and out_fx68k.txt). Both builds print the same lines apart from
the CORE line.

```text
u = 1 samples per 70 ns of display, r = 2 rows per beam line (M-H-00 calibration rows)
P6 solid-row widths in pixels of 140 ns: W-00 16, W-01 32, W-10 32, W-11 64
P7 cell AW-01: every check holds
P7 cell AW-10: every check holds
P7 cell AW-11: every check holds
P9 bits 7-5 of the index (recorded, not graded): 0 of 1536 samples differ from index 16 + (k>>1)*4 + v
P10 bits 7-5 of the index (recorded, not graded): 0 of 1536 samples differ from index 16 + (k>>1)*4 + v
the CPU read of LISAID ($DFF07C) at the end of the register list returned $00F8 in every run
```

Graded counts: both builds print 59 passed, 0 failed.

P7. The twelve cells A-<res>-<spres> and the three width runs AW-01, AW-10 and
AW-11 show every group of every row in the colours of the table (P7 and P7-ctl
print PASS).

### 9.5 Mutations

Every assertion, control and precondition has one record in the header of
`test_aa_integ_s6.cpp`, selected by `NMH_MUTATE=<label>` (9.2). A mutation
changes the stimulus and prints FAIL through the unchanged comparator on both
builds, with every run reaching DONE. Where the mutated stimulus also changes a
neighbouring precondition, the record lists it as "Also printing FAIL". The
records of P7 are NMH_MUTATE=P7-W01 and P7-W10 (ATT = 0 in the AW-01 and AW-10
runs). The control P7-ctl has three records of its own (P7-ctl, P7-L11,
P7-W11).
