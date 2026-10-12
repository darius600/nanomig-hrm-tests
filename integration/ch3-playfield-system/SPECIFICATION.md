# Test package specification: playfield system bits (COLOR_ON, LPEN, VBSTRT)

This specification was written from the package's claims (`claims-spec-stage.jsonl`)
and from the printed pages of the *Amiga Hardware Reference Manual*, 3rd edition,
by an author who never saw the implementation under test or any existing test. Every
expected value below is taken from a quoted sentence of the manual; where the manual
leaves a value open, this document says so and specifies no value for it. Page
numbers are printed page numbers; every quoted sentence was checked against the
rendered printed page, not only against the text layer.

The package has five propositions:

| id | proposition (short) | claims |
|---|---|---|
| P1 | BPLCON0 bit 9 (COLOR_ON) has no effect on the RGB output | opus:3-a1000.c04, sol:3-composite.c6, opus:3-sum.c21 |
| P2 | the RGB output is the correct picture whether bit 9 is set or clear | opus:3-a1000.c05 (sol:3-composite.c7 is declined, see below) |
| P3 | BPLCON0 bit 3 is LPEN, the light pen enable: setting it latches the beam position read through VPOSR/VHPOSR | opus:A-bpc.b03.c0, sol:A-bplcon.c49, opus:A-leg.lpen1, sol:A-bplcon.c72, opus:3-r0.c09, sol:3-regsum-bplcon0.c6 |
| P4 | LPEN is 0 after power-up | opus:A-leg.lpen2, sol:A-bplcon.c73 |
| P5 | the value written to VBSTRT ($1CC, ECS) is the line at which vertical blanking starts | opus:B-vbstrt.addr (opus:C-vbstrt.c01 and sol:C-scan.c20 are declined, see below) |

---

## Claims not turned into a proposition

Two claims are declined whole and printed as RECORDED lines; no assertion carries
their ids. The following parts of claims give nothing a test can grade, and no test
may assert anything about them:

- **opus:C-vbstrt.c01 and sol:C-scan.c20 (p.304; p.293, where the legend on p.287
  reads "W=write-only").** Each quotation bundles the register's write-only access
  ("W") and the chip that holds it ("A", Agnus) with the vertical-blanking-start
  clause. No page says what a read of $DFF1CC returns, so a test must not read VBSTRT
  and must not expect any value from such a read; which chip holds a register is not
  visible at any output the test has (Chip-memory port, video, audio). Neither
  clause is gradeable, so both ids are declined whole. The VBLANK-start clause is
  graded by P5's aggregate under opus:B-vbstrt.addr (p.293: "VBSTRT 1CC W A(E)
  Vertical line for VBLANK start").
- **"regardless of the output of the composite video signal" in opus:3-a1000.c05
  and sol:3-composite.c7** (p.63). The composite video output is not among the
  outputs a test can see; the only handle a test has on the composite signal is its
  enable bit, BPLCON0 bit 9. The quotation of opus:3-a1000.c05 opens with the
  sentence "The color burst enable does not affect the RGB video signal." and that
  bit 9 handle is what P2 grades: it checks the RGB picture with bit 9 set and with
  bit 9 clear, and nothing about the composite signal itself. The quotation of
  sol:3-composite.c7 is the second sentence alone, "RGB video is correctly generated
  regardless of the output of the composite video signal.", which names no handle;
  that id is declined whole and printed as a RECORDED line.
- **"reset on power up" in opus:A-leg.lpen2 and sol:A-bplcon.c73** (p.271). A run
  contains exactly one power-up, the harness's initial reset. P4 checks LPEN in
  the interval between that power-up and the program's first write to BPLCON0; a
  reset other than power-up is not covered by the page and is not tested.

---

## Common definitions

### System configurations

The harness offers ECS or OCS behaviour and PAL or NTSC. The configurations used
below are:

| name | chip set | video standard |
|---|---|---|
| S1 | ECS | PAL (the harness default) |
| S2 | ECS | NTSC |
| S3 | OCS | PAL |
| S4 | OCS | NTSC |

Chip memory size: the default 2 MB. Every run is made once with each of the two
CPU cores; a proposition passes only if it passes with both cores.

All Copper lists and all bitplane data are placed at even addresses below $40000.
The pages disagree on the width of the bitplane pointer (p.272: "the 18-bit
pointer"; p.54: "Pairs of registers with names ending in PTH and PTL contain 19-bit
addresses"); addresses below $40000 are valid under both.

### Registers used (addresses from Appendix B, pp.289-293)

| register | address | page |
|---|---|---|
| VPOSR | $004 (R) | p.289 |
| VHPOSR | $006 (R) | p.289 |
| INTREQR | $01E (R) | p.289 |
| COP1LCH/L | $080/$082 | p.290 |
| COPJMP1 | $088 (strobe) | p.290 |
| DIWSTRT, DIWSTOP | $08E, $090 | p.290 |
| DDFSTRT, DDFSTOP | $092, $094 | p.290 |
| DMACON | $096 | p.290 |
| INTENA, INTREQ | $09A, $09C | p.290 |
| BPL1PTH ... BPL4PTH | $0E0, $0E4, $0E8, $0EC (PTL at +2) | p.291 |
| BPLCON0, BPLCON1, BPLCON2 | $100, $102, $104 | p.291 |
| BPL1MOD, BPL2MOD | $108, $10A | p.291 |
| COLOR00 ... COLOR15 | $180 ... $19E | p.292 |
| VBSTRT, VBSTOP | $1CC, $1CE (ECS) | p.293 |
| BEAMCON0 | $1DC (ECS) | p.293 |

One page disagrees on BPLCON0's address: the light-pen figure on p.333 labels the
box carrying the "light pen enable" arrow at bit 3 "BPLCON0 write only DFF104".
Appendix A (p.271: "BPLCON0 100"), Appendix B (p.291) and chapter 8 (p.229:
"BPLCON0 ($DFF100) Bit 3 enables the light pen latch") give $100, and $104 is
BPLCON2 on p.271. All pages agree that the light pen enable is bit 3. The test
writes BPLCON0 at $DFF100.

### Rules every program follows

- **BPLCON0 is always written as a whole word.** p.48: "The bits in the BPLCON0
  register cannot be set independently. To set any one bit, you must reload them
  all." p.89: "(Warning: Bits in this register cannot be independently set.)" Every
  value below gives all 16 bits; every bit not named is 0. p.89: "Bits 4-7 not used
  (make 0)".
- **Start-up.** The program first writes DMACON = $7FFF, INTENA = $7FFF and
  INTREQ = $7FFF. p.278 (DMACON) and p.280 (INTENA/INTREQ): bit 15 "SET/CLR ...
  Determines if bits written with a 1 get set or cleared". With bit 15 = 0 these
  writes clear every DMA enable, every interrupt enable and every pending request.
  The 68000's interrupt mask stays at level 7 for the whole program, so no
  interrupt is taken; interrupt requests are only polled.
- **Start word.** Directly after the start-up writes every program stores one word
  to Chip memory address $0C0FF0, above the region where the overlay would lose a
  CPU write, so the store reaches the Chip-memory port: $5A01 in the programs of P1
  and P2, $5A03 in those of P3 and P4, $5A05 in those of P5. The start word is the
  known value the fixture line of a run certifies: the fixture requires the store to
  be seen with exactly that value.
- **Copper start**, where a Copper list is used, follows p.30: "you must initialize
  one of the Copper location registers (COP1LC or COP2LC) and write to its strobe
  address before Copper DMA is turned on." The order is COP1LC (one long-word
  write to COP1LCH), then the COPJMP1 strobe, then the DMACON write that sets
  COPEN (bit 7) and DMAEN (bit 9) (p.278). Every Copper list ends with the WAIT
  p.23 gives for this purpose: `$FFFF,$FFFE`.
- **Copper instruction encoding**, p.21 and p.22: MOVE = first word register
  address (bit 0 = 0), second word data; WAIT = first word `VP<<8 | HP | 1`,
  second word `$FFFE` (bit 15 blitter-finished-disable = 1, all compare-enable bits
  set). Below, `WAIT(v,h)` means the words `(v<<8)|(h&$FE)|1, $FFFE`.
- **Registers whose power-up value no page states**, and which the displays below
  depend on, are written explicitly: BPLCON1 = $0000 and BPLCON2 = $0000 before any
  display is enabled. DIWHIGH (ECS) is never written; p.306: "If it is not written,
  the old scheme for DIWSTRT and DIWSTOP described above holds."
- The program never reads VBSTRT, VBSTOP or BEAMCON0.

### Observing a CPU register read

A CPU read of a custom register does not appear at the Chip-memory port; the
value read does, when the program stores it. Every read in this package is made by
an instruction that stores the value into a buffer in Chip memory at once (for
VPOSR: `MOVE.L $DFF004,(A0)+`, a long-word read of VPOSR and VHPOSR together, which
p.216 permits: "the address pairs VPOSR,VHPOSR and VPOSW,VHPOSW can be read from
and written to as long words"). The read of iteration *i* happened after the store
of iteration *i*-1 and before the store of iteration *i*; the test gives read *i*
the time interval between those two stores, both of which it sees with their
system-clock time. "A read lies inside an interval" means both bounding stores lie
inside it.

### Line and colour-clock lengths

p.24: "One horizontal count takes one cycle of the system clock (processor is
twice this)", NTSC 3,579,545 Hz, PAL 3,546,895 Hz; that cycle is the colour clock
(cc). p.23: "In PAL, they are all 227 long. [...] Every other line is a long line
(228 color clocks, 0-$E3), with the others being 227 color clocks long" (NTSC).
p.24: "In NTSC, the fields are 262, then 263 lines and in PAL, 312, then 313
lines." The builder converts colour clocks to the harness's system-clock cycles
using the harness's clock definition.

**Field-length check** (used as a precondition wherever a program writes BEAMCON0
or relies on the video standard): the time between two consecutive detections of
VERTB (P3, P4, P5) or two consecutive vsync assertions (P1, P2) equals, within one
line (227 cc) plus the period of the detecting loop, one of 70,824 cc or 71,051 cc
(PAL: 312 or 313 lines of 227 cc) or one of 59,605 cc or 59,833 cc (NTSC: 262 or
263 lines alternating 227/228 cc), and it is the value of the standard the harness
was configured for. The pages state the 312/313 and 262/263 alternation for
interlace; which of the two a non-interlaced field has is not stated, so both are
accepted.

### Video output

The test sees 4-bit R, G, B, hsync and vsync per pixel clock. A colour register's
12 bits map onto the three guns as p.46 Table 3-3 gives them: "Bits 11 - 8 Red /
Bits 7 - 4 Green / Bits 3 - 0 Blue". A sample "shows Cn" when its (R, G, B) equals
the red, green and blue fields of the 12-bit value Cn. "vsync assertion" is the
transition of vsync into the level vsync holds for the shorter part of a field.

The harness description says the output passes a line doubler, so that every
second pixel of the fastest resolution is not visible. The fastest resolution is
SuperHires (p.300: "SuperHires mode provides a 35ns pixel display rate - twice the
horizontal resolution of Hires mode, and four times the Lores rates"), so the
output shows one sample per 70 ns and a low-resolution pixel lasts k = 2 samples.
The builder takes k from the harness's definition, never from the output under
test. Where the doubler emits a beam line more than once, each copy must satisfy
every per-line check, and the copies count as one beam line.

### Colour values

| name | value |
|---|---|
| C0 | $0359 |
| C1 | $0EA2 |
| C2 | $0F3C |
| C3 | $04E7 |

Palette for the 16-colour configuration (COLOR00 ... COLOR15): $0359, $0EA2,
$0F3C, $04E7, $0A0F, $00F5, $0FF0, $0888, $0C63, $036C, $09E1, $0E19, $05A5, $0D7B,
$01BD, $0B42. All values are pairwise distinct and none is $000 ("Black", p.47
Table 3-4), so a colour from the table cannot be confused with a black blanking
level.

---

## P1 - BPLCON0 bit 9 (COLOR_ON) has no effect on the RGB output

### 1. Proposition

Changing only bit 9 of BPLCON0 leaves every R, G and B sample of the video output
unchanged. Claims opus:3-a1000.c04 and sol:3-composite.c6 (p.63), opus:3-sum.c21
(p.64).

### 2. Stimulus

Four display configurations, each run twice: once with bit 9 = 0 (run "0") and
once with bit 9 = 1 (run "1"). The two runs of a pair are identical in every
instruction and every data word except the BPLCON0 data words named below.

Common set-up, in this order (it follows the steps of the basic playfield summary
on pp.63-64):

1. Start-up writes (Common definitions).
2. Colour registers: COLOR00 = C0, COLOR01 = C1 (configurations A, B, E); COLOR00 ...
   COLOR15 = the 16-colour palette (configuration C).
3. Bitplane data in Chip memory (below).
4. BPLCON1 = $0000, BPLCON2 = $0000.
5. DIWSTRT = $2C81; DIWSTOP = $2CC1 in PAL, $F4C1 in NTSC. p.59: "The normal NTSC
   DIWSTRT is ($2C81). The normal NTSC DIWSTOP is ($F4C1). The normal PAL DIWSTRT
   is ($2C81). The normal PAL DIWSTOP is ($2CC1)."
6. DDFSTRT/DDFSTOP = $0038/$00D0 in low resolution, $003C/$00D4 in high resolution
   (p.60: "The normal low resolution DDFSTRT is ($0038). The normal high resolution
   DDFSTRT is ($003C)." ... "The normal low resolution DDFSTOP is ($00D0). The normal
   high resolution DDFSTOP is ($00D4).").
7. BPL1MOD = BPL2MOD = $0000 (p.64 step 5: "Set modulo to 0 for non-interlaced").
8. The Copper list (below), then Copper start as in Common definitions, with the
   DMACON write = $8380 (SET/CLR, DMAEN, BPLEN, COPEN; p.278). p.62: "You start
   playfield display by making certain that the bitplane pointers are set and
   bitplane DMA is turned on."
9. The CPU then idles in a loop that performs no custom-register access until the
   test ends.

The Copper list of configurations A, B and C, executed every field (p.63: "Each
time the playfield is redisplayed, you have to reset the bitplane pointers. [...]
You write Copper instructions to handle the redisplay"):

    MOVE BPLCON0, <value>
    MOVE BPLxPTH, <high word of plane x start>   ; for every plane x in use
    MOVE BPLxPTL, <low word of plane x start>
    WAIT $FFFF,$FFFE

| config | display | BPLCON0 run "0" | BPLCON0 run "1" | bitplane data |
|---|---|---|---|---|
| A | background only (BPU = 000) | $0000 | $0200 | none |
| B | 1 plane, low resolution | $1000 | $1200 | 20 words per line, H lines |
| C | 4 planes, high resolution | $C000 | $C200 | 4 planes x 40 words per line, H lines |
| E | as B, bit 9 changed by the Copper in the middle of every line | see below | see below | as B |

H = 256 lines in PAL, 200 in NTSC (p.49 Table 3-6: "Non-interlaced 200 [NTSC] 256
[PAL]"). High resolution fetches 40 words per plane per line (p.62: "When you are
using high resolution mode to display the basic playfield, you need to fetch 80
bytes for each line, instead of 40.").

Bitplane data (words, most significant bit first):

- B and E: line L (0 <= L < H), word k (0 <= k < 20):
  `w(L,k) = $8001 OR ((($9E37 * (20*L + k + 1)) mod $10000) AND $7FFE)`.
  (Every word starts and ends with a 1 bit.)
- C: plane p (1..4), line L, word k (0 <= k < 40):
  `w_p(L,k) = ($9E37 * (40*L + k + 1 + 10007*p)) mod $10000`.

Configuration E: the Copper list is

    MOVE BPLCON0, $1000
    MOVE BPL1PTH, ... ; MOVE BPL1PTL, ...
    for each line L from $2C to $FE:
        WAIT(L, $80)
        MOVE BPLCON0, <value(L)>
    WAIT $FFFF,$FFFE

with value(L) = $1000 in run "0" for every L, and in run "1" value(L) = $1200 for
odd L and $1000 for even L. Horizontal position $80 lies inside the displayed part
of the line (p.23: "The standard screen (320 pixels wide) has an unused horizontal
portion of $04 to $47"), so bit 9 changes while pixels are being shown. Both runs
have the same number of Copper instructions at the same positions.

Every configuration is run in S1, S2, S3 and S4.

### 3. Observation

The video output samples (R, G, B, hsync, vsync) of both runs, over two complete
fields: from the 2nd to the 4th vsync assertion after the first Copper instruction
fetch seen at the Chip-memory port. Both runs are aligned on those vsync
assertions, sample index by sample index.

### 4. Expected value

Every R, G and B sample of run "1" equals the sample at the same index of run "0";
zero differing samples over both fields.

From p.63: "NOTE: The color burst enable does not affect the RGB video signal." and
p.64, step 7: "For the A1000: set bit 9 in BPLCON0 to enable the color display on a
composite video monitor. RGB video is not affected." (p.90 names bit 9 "COLOR_ON
(color enable) 1 = composite video color-burst enabled"; p.271 names it COLOR,
"Composite video COLOR enable".)

Quantifier: "does not affect" is universal. It is graded over every sample of two
fields, in four display configurations (background only, low resolution, high
resolution with 16 colours, and bit 9 changing mid-line on every second line),
in all four system configurations, with both CPU cores. These cases do not
establish the statement for every possible display; they cover the display kinds
the chapter's basic playfield summary (pp.63-64) sets up, plus a change of bit 9
during the display.

### 5. Preconditions

Each is checked in both runs of a pair; a pair whose precondition fails is a
failure, never a pass.

- **P1-a (alignment):** the hsync and vsync samples of the two runs are identical
  over the compared span.
- **P1-b (the bit-9 value really reached BPLCON0):** the Copper's fetched MOVE data
  words with destination $100 (seen at the Chip-memory port) carry the table's
  values: in configurations A-C one per field with bit 9 = 0 in run "0" and 1 in
  run "1"; in E, one at the top of the list ($1000) and then one after each of the
  211 WAITs for lines $2C-$FE, in run "1" alternating $1000 (even lines) and $1200
  (odd lines), in run "0" all $1000.
- **P1-c (the display ran):** a "DMA field" is the span between two consecutive
  Copper fetches of the list's first word; the first such fetch after the COPJMP1
  strobe is the start of DMA field 1 (the Copper restart happens in vertical
  blanking, p.25 and p.30, so no field's bitplane fetches are split). In DMA
  fields 2 and 3, B and E show exactly 20 * H reads of the bitplane buffer's
  addresses and C exactly 4 * 40 * H, all within the range the data was written to
  (p.61: "the pointers are continuously incremented to point to the next word to
  be fetched"). In A, no read of any bitplane address.
- **P1-d (the picture is not trivially uniform):** over the compared span, in A
  samples showing C0 occur; in B and E, samples showing C0 and samples showing C1
  both occur; in C, each of the 16 palette colours occurs.
- **P1-e (video standard):** in both runs, the field-length check (vsync to vsync)
  matches the configured standard.

### 6. Contrasting cases

- **Main check:** replace the change of bit 9 by a change the pages say is visible.
  In A, run "1" instead writes COLOR00 = $0358 (C0 with blue bit 0 cleared); in B
  and C, COLOR01 = $0EA3 (C1 with blue bit 0 set); p.46 Table 3-3 puts blue in bits
  3-0, so every sample showing that colour changes its blue value. In E, run "1"
  writes $0000 instead of $1200 on odd lines; p.48 Table 3-5 says of BPU = 000:
  "Shows only a background color; no playfield is visible", so the rest of every
  odd line shows C0 where run "0" shows C1. Each must yield differing samples; the
  program finishes and P1-a, P1-c (for C/B, and E's fetches) and P1-e still hold.
- **P1-a:** select PAL for one run and NTSC for the other; the sync samples differ.
- **P1-b:** apply the run-"1" check to the run-"0" fetch trace; it fails.
- **P1-c:** in B, C and E, DDFSTOP is 8 colour clocks lower than the value of step 6
  with bitplane DMA still on (p.60: DDFSTOP names the last fetch); the bitplane reads
  of a DMA field are fewer than N, and still a nonempty set. In A the Copper list
  carries a one-plane BPLCON0 ($1000, p.89), so reads occur where none are expected.
- **P1-d:** in B, fill the bitplane with $0000; no sample shows C1.
- **P1-e:** in configuration B at S1 the program writes BEAMCON0 with bit 5 opposite
  to the configured standard (p.304: "NTSC (525 lines ...) PAL (625 lines ...)"), and
  the field length is the other standard's.
- **Fixture:** in run "1" of every configuration at S1 the program stores a start word
  other than $5A01; the run completes and the fixture line reports it.

### 7. Reference frame

The vsync assertion. Its detection is checked by P1-e (one assertion per field, at
the configured field length).

---

## P2 - The RGB output is the correct picture whether bit 9 is set or clear

### 1. Proposition

With BPLCON0 bit 9 set, and equally with it clear, the RGB output shows exactly
the picture the playfield registers and bitplane data define. Claims
opus:3-a1000.c05 and sol:3-composite.c7 (p.63).

### 2. Stimulus

Configuration B of P1, both runs ("0": BPLCON0 = $1000; "1": BPLCON0 = $1200), in
S1, S2, S3 and S4.

### 3. Observation

The video samples from the 2nd to the 6th vsync assertion after the first Copper
instruction fetch, in each run separately, grouped into beam lines by hsync. No
page says at which line vsync occurs, so fields are not delimited by vsync here:
the beam lines that contain a sample showing C1 form blocks of consecutive lines,
and a block that touches neither end of the span is one field's data lines
("complete block").

### 4. Expected value

The source sentence: p.63, "RGB video is correctly generated regardless of the
output of the composite video signal." What "correctly" means is taken from the
pages that define the playfield:

- p.55: "For a two-color playfield, you define a bitplane that has "0"s where you
  want the background color and "1"s where you want the color in register 1."
- p.272 (BPLxDAT): "The MSB is output first, and is, therefore, always on the
  left."
- p.61: "The data for the next line begins at memory location START+40" (modulo 0).
- p.49 Table 3-6 and p.59: H = 200 (NTSC) or 256 (PAL) lines, 320 low-resolution
  pixels per line (p.63: "320 pixels in each horizontal line").
- p.46: COLOR00 "is also displayed outside the defined display window, in the
  border area."

In each run:

1. There are at least three complete blocks, and every complete block has exactly
   H beam lines.
2. In the j-th line of a complete block (j = 0 for its topmost line), let s0 be the
   first sample showing C1. For i = 0 ... 319, the k = 2 samples starting at
   s0 + 2i all show C1 if bit (15 - (i mod 16)) of word w(j, floor(i/16)) is 1, and
   all show C0 if it is 0.
3. In the same line, sample s0 - 1 and sample s0 + 640 show C0.

Quantifier: "correctly generated regardless of" the composite output is graded
with the composite colour enable in both states, every pixel of every data line,
at least three fields each, four system configurations, both CPU cores.

What the output shows during horizontal and vertical blanking is not stated on any
page; samples outside the data lines' 320-pixel spans are not graded, except the
two border samples in item 3.

### 5. Preconditions

- **P2-a:** P1-b and P1-e hold for both runs.
- **P2-b (the data displayed is the data prepared):** in DMA fields 2 to 5 (as
  defined in P1-c), the bitplane buffer is read exactly once per word, at START,
  START+2, ..., START + 40*H - 2, in increasing address order (pp.61-62, Figures
  3-10 and 3-11: line 1 at START ... START+38, line 2 at START+40 ...). The buffer
  is followed by 2*H spare bytes of $00 so that the contrast below reads no
  undefined memory.

### 6. Contrasting cases

- **Main check:** in both runs, flip bit 14 of word w(100, 5) in memory while the
  expectation is still computed from the unmodified formula. p.55 predicts the
  pixel for that bit now shows the other of C0 and C1, so item 2 fails for line
  100 of every block only; everything else, including P2-b, still holds.
- **P2-a:** as in P1.
- **P2-b:** BPL1MOD = $0002; p.61 ("the modulo is added to the bitplane pointers")
  predicts every line after the first starts 2 bytes later, and the address check
  fails. Item 1 fails too and item 2 is not graded.
- **Item 1 as a count:** DIWSTOP with VSTOP one line lower ($2BC1 in PAL, $F3C1 in
  NTSC); p.59 predicts H-1 data lines per field, and item 1 fails. The buffer is then
  read for H-1 lines only, so P2-b fails too and item 2 is not graded; item 3 holds
  for the lines shown.
- **Fixture:** in run "1" of configuration B at S1 the program stores a start word other
  than $5A01; the run completes and the fixture line reports it.

### 7. Reference frame

Beam lines are counted on the output by hsync; the left edge of a data line is its
first sample showing C1, which is pixel 0 because every line's first word starts
with a 1 bit. The anchor is checked by items 2 and 3 together: a shifted edge
breaks the bit-for-bit match or the C0 border sample.

---

## P3 - BPLCON0 bit 3 (LPEN) enables the light-pen latch

### 1. Proposition

Writing 1 into bit 3 of BPLCON0, the light pen enable, makes the beam position read
through VPOSR/VHPOSR stay frozen from the end of each field until vertical
position 20 of the next. Claims opus:A-bpc.b03.c0, sol:A-bplcon.c49,
opus:A-leg.lpen1, sol:A-bplcon.c72 (p.271), opus:3-r0.c09,
sol:3-regsum-bplcon0.c6 (p.89).

The claims name bit 3 of BPLCON0 LPEN and call it the light pen enable (p.271
table: "03 LPEN"; legend "LPEN =Light pen enable (reset on power up)"; p.89: "Bit 3
- LPEN (light pen enable)"). What enabling does, which a test can see, is stated in
chapter 8, which these pages refer to (p.216: "when you are using a light pen, this
same address is used to read the light pen position rather than the beam position.
This is described fully in Chapter 8"):

- p.229: "BPLCON0 ($DFF100) Bit 3 enables the light pen latch"
- p.239: "To enable the light pen input, write a 1 into bit 3 of BPLCON0. Once the
  light pen input is enabled and the light pen issues a trigger signal, the value
  in VPOSR is frozen. If no trigger is seen, the counters latch at the end of the
  display field. It is impossible to read the current beam location while the
  VPOSR register is latched. This freeze is released at the end of internal
  vertical blanking (vertical position 20)."

A test cannot drive the light pen trigger input. The observation is chosen so that
the expected result is the same whether or not a trigger occurs: with or without a
trigger, the value is frozen by the end of the field and stays frozen until
vertical position 20.

### 2. Stimulus

Run "L1" (LPEN = 1) and run "L0" (LPEN = 0), identical except for one data word:

1. Start-up writes.
2. ECS configurations only: BEAMCON0 = $0020 in PAL, $0000 in NTSC. This makes
   bit 13 LPENDIS 0 (p.305: "13 LPENDIS Ignore latched pen value on vertical pos
   read"), whose power-up value no page states, and keeps bit 5 "PAL Programmable
   pal mode enable" at the configured standard. p.305 also says of BEAMCON0 "Do not
   write to this register"; that is advice to application software, and the test
   writes it only to put LPENDIS into a known state.
3. BPLCON0 = $0008 in L1, $0000 in L0 (BPU = 000, so no playfield; every other bit
   0).
4. No DMA and no interrupt is enabled. VERTB is polled, not taken as an
   interrupt; p.280 says of the request bits: "These bits may be polled by the
   processor; if enabled by the bits listed in the next register, they may cause
   processor interrupts."
5. Five times:
   - poll INTREQR until bit 5 (VERTB) is 1. p.219: "Bit 5, VERTB, causes an
     interrupt at line 0 (start of vertical blank) of the video display frame."
   - write INTREQ = $0020 (clear VERTB);
   - store a marker word (the iteration number 0..4) into Chip memory; the time of
     this store is t_f;
   - execute 600 times `MOVE.L $DFF004,(A0)+`.
6. Idle loop without custom-register access.

Run in S1, S2, S3, S4.

### 3. Observation

The long words stored by the read bursts after VERTB detections 2, 3, 4 and 5
(detection 1 is skipped: the BPLCON0 write may have come after that field's end).
For detection f, the window is

    [ t_f + 228 cc , t_f + 18 * 227 cc ]

Only reads lying inside the window (Common definitions) count. Of each long word,
only the 17 low bits are compared: V8 (VPOSR bit 0), V7-V0 (VHPOSR bits 15-8) and
H8-H1 (VHPOSR bits 7-0) (p.238 table; p.239: "mask off the upper 15 bits of the
32-bit word").

Why this interval: VERTB is set in line 0 and t_f follows it by less than one
poll-loop period plus one register write and one store, together far less than a
line; so the window starts no earlier than line 1 and ends before line 20 (within
line 18, or line 19 if VERTB is set late in line 0). p.239 twice names the period
"0<V20" (so printed; read as 0 < V < 20) as the one in which a read is
meaningful. The pages disagree on where vertical blanking ends (last section); the
window lies inside every page's version of it and before position 20.

### 4. Expected value

In L1, in each of the four windows, all counted reads have the same 17-bit value.

Source: p.239, "It is impossible to read the current beam location while the VPOSR
register is latched. This freeze is released at the end of internal vertical
blanking (vertical position 20)."

Quantifier: "latched ... released at position 20" is a statement about every read
before position 20; it is graded over every read in the window (at least 100, see
preconditions), in four consecutive fields, four system configurations, two cores.

The aggregate (the line that carries the ids) requires, in every system
configuration, this check on L1 (`P3-MAIN`) together with the L0 line `P3-LIVE`
(below): the claim says that bit 3 makes the value freeze, and a chipset whose
counters freeze whatever bit 3 holds would pass `P3-MAIN` alone. "Only bit 3 differs
between L1 and L0", so a pass of `P3-MAIN` and `P3-LIVE` together locates the enable
at bit 3.

Not graded, because the pages leave it open:

- **the frozen value itself.** "the end of the display field" is not defined as a
  position; p.239's comparison value "$10500 (V=261)" belongs to an NTSC field and
  no PAL value is given; and whether a trigger occurs is not under the test's
  control.
- **the release after position 20.** p.239: "when the light pen latch is enabled,
  the VPOSR register may be latched at any time, and cannot be relied on as a
  counter." A trigger may freeze the value again at any point after the release,
  and the test cannot know whether one occurred.
- **the colour clock at which V steps after H wraps.** No page fixes it (see
  "P3-LIVE and P4-MAIN: the counting lines" in the build notes).

### 5. Preconditions

- **P3-a (enough reads, spread over the window):** each window holds at least 100
  counted reads; the first lies within 30 cc after the window start, the last
  within 30 cc before its end.
- **P3-b (the anchor):** in L0, every counted read has V (V8..V0) between 1 and 19
  inclusive. In both L0 and L1, consecutive t_f satisfy the field-length check.
- **P3-M (the blocks are the program's own):** in both runs the stores to the
  measurement buffer from $0C1000 up are exactly five blocks; block k (k = 0..4) is the
  marker word k followed by 600 long words (20 in the P3-a contrast), each stored high
  half then low half at consecutive word addresses. Every word the grading takes for a
  marker or for a read is then one the program stored as such. P3-a, P3-b, P3-c, P3-LIVE
  and P3-MAIN wait for it.
- **P3-L (the detecting loop is measured):** the `INTREQR` polling loop is seen
  spinning in the instruction fetches of both runs, at least 100 iterations each;
  its period is what the field-length check adds to its tolerance. P3-b and P3-c
  wait for it.
- **P3-c (ECS only):** the program's BEAMCON0 write carries bit 13 = 0 and bit 5 =
  the configured standard, checked on the program's code and on the immediate data
  word the CPU fetched at the Chip-memory port, and the field-length check after it
  gives the configured standard.
- **P3-d:** the program contains no other write to $DFF100, and no Copper
  instruction fetched from the Copper list's address ($020000) at the Chip-memory
  port during the run is a MOVE to $100 (first word $0100), so nothing else can have
  written BPLCON0 from a list the test started. A Copper running from a COP1LC value
  the test did not write is not observed.

Every precondition is printed before the line that waits for it. When the fixture
(both programs loaded, reset released, DONE reached) or a precondition it depends
on fails, the dependent line prints `NOT GRADED`.

### 5a. The L0 line P3-LIVE

In L0, in each window, the counted reads are a running counter (p.216: VHPOSR reads
"the counter that is producing the beam on the screen"; p.239: "Read (long) VPOSR
twice. If both values are not the same ..."). Three criteria, graded per window, over
consecutive counted reads:

1. no two consecutive reads are equal;
2. V (V8..V0) never decreases and steps by at most 1;
3. within one V value, H (H8..H1) increases except for at most one decrease (the wrap
   of H, p.23: "$0 to $E2").

MEASURED, not graded: each pair of consecutive reads with V unchanged and H lower is
printed with its H values. No page fixes the colour clock at which V steps after H
wraps; a read at the wrap may show the previous V. The three criteria hold on a
chipset that steps V at the wrap and on one that steps V a few colour clocks later,
and fail on a counter that is frozen (criterion 1).

### 6. Contrasting cases

- **Main check (P3-MAIN):** run the L0 program in place of the L1 program. The pages
  predict counting values (p.216); the reads in each window are no longer equal and
  the "all equal" check fails.
- **P3-LIVE:** run the L1 program in place of the L0 program on a chipset in which
  bit 3 latches; the L0 windows then hold one frozen value and criterion 1 fails.
- **P3-a:** reduce the burst to 20 reads; fewer than 100 reads fall into the window.
- **P3-b:** delay the marker store by about 26 lines after the VERTB detection in
  L0; the reads then have V above 19.
- **P3-M:** the L1 program stores every marker with bit 8 set ($0100 + k); the block
  shape is not the program's own marker sequence, the line reports it and the lines
  that wait for it are not graded.
- **P3 fixture:** the L1 program stores a start word other than $5A03; both runs
  complete and the fixture line reports it.
- **P3-L:** replace the polling loop by a fixed delay; no loop is seen in the
  instruction fetches.
- **P3-c (ECS):** BEAMCON0 = $2020 (PAL) / $2000 (NTSC), LPENDIS = 1. p.305 predicts
  that the latched value is ignored on a read; the L1 windows then show counting
  values and the main check fails, while the program finishes and P3-a and P3-b still
  hold. Separately, BEAMCON0 with bit 5 set opposite to the configured standard
  makes the field-length check report the other standard (p.304: "NTSC (525 lines
  ...) PAL (625 lines ...)").
- **P3-d:** add a write BPLCON0 = $0000 after the first detection in L1; the program
  check reports it.

### 7. Reference frame

Line 0 is anchored on the VERTB request (p.219), detected by polling; its time is
the marker store. The anchor is checked by P3-b: in L0 the beam counter itself
confirms that every counted read lies in lines 1-19, and in both runs the
detections recur at the field length. Nothing on the pages says the light-pen latch
affects VERTB.

---

## P4 - LPEN is 0 after power-up

### 1. Proposition

After power-up, before the program writes BPLCON0, the light-pen latch is off: the
beam position read through VPOSR/VHPOSR counts in the interval where an enabled
latch would hold it. Claims opus:A-leg.lpen2 and sol:A-bplcon.c73 (p.271: "LPEN
=Light pen enable (reset on power up)").

### 2. Stimulus

The program of P3 with step 3 removed: the program never writes BPLCON0. The run
starts from the harness's power-up reset, and the test program is the first code
the CPU executes after it. Run in S1, S2, S3, S4.

In ECS configurations the BEAMCON0 write of P3 step 2 stays: the power-up value of
LPENDIS is not stated on any page, and with LPENDIS = 1 a set LPEN would be
invisible.

### 3. Observation

As P3: windows after VERTB detections 2 to 5, the 17-bit values of the counted
reads.

### 4. Expected value

In every window the counted reads are a running counter, by the three criteria of
P3-LIVE, and each has V between 1 and 19. From p.271, "reset on power up": LPEN = 0,
so p.239's latch is not enabled, and VHPOSR reads "the counter that is producing the
beam on the screen" (p.216).

The line is graded only in a system configuration in which `P3-MAIN` passed. "LPEN
is 0" is observable only where an LPEN of 1 would latch; where it would not, an LPEN
of 0 and of 1 look alike and a pass would credit the ids vacuously. Where `P3-MAIN`
did not pass, or was not graded, `P4-MAIN` prints `NOT GRADED`.

Quantifier: the page states one event, power-up; the test has one per run, graded
in four system configurations and with both cores.

### 5. Preconditions

- **P4-a:** the P3-a check on the one program: each window holds at least 100 counted
  reads, spread over it.
- **P4-M:** as P3-M, for the one program: five blocks of marker k and 600 long words
  (20 in the P4-a contrast). P4-a, P4-c and P4-MAIN wait for it.
- **P4-L:** as P3-L, for the one program. P4-c waits for it.
- **P4-b:** the program contains no write to $DFF100; no Copper instruction
  fetched from the Copper list's address ($020000) at the Chip-memory port is a MOVE
  to $100; the first instructions the CPU fetches after power-up are the test
  program's. A Copper running from a COP1LC value the test did not write (no page
  states DMACON's power-up value, so a Copper could run before the program's first
  DMACON write) is not observed.
- **P4-c (ECS):** consecutive t_f satisfy the field-length check.
- **P3-MAIN** passed in the same system configuration (above).

Every precondition waits for the fixture and prints `NOT GRADED` when it fails.

### 6. Contrasting cases

- **Main check:** the P3 L1 program is the contrast of `P4-B-PROGRAM`: it inserts
  BPLCON0 = $0008 before step 4. A contrast of `P4-MAIN` has to leave every
  precondition true, so it is an implementation record: a second private chipset in
  which LPEN is 1 after reset (build notes). On it `P3-MAIN` passes, the windows hold
  one frozen value and criterion 1 fails.
- **P4-a, P4-L, P4-c:** as in P3.
- **P4-M:** as in P3: the program stores every marker with bit 8 set.
- **P4 fixture:** the program stores a start word other than $5A03; the run completes
  and the fixture line reports it.
- **P4-b:** (program part) add the BPLCON0 write of the main contrast; the
  program check reports it. (Copper part) before step 5, start a Copper list
  `MOVE BPLCON0,$0008 / WAIT $FFFF,$FFFE` as in Common definitions (DMACON =
  $8280); the fetch-trace check finds the MOVE to $100, and the program still
  finishes.

### 7. Reference frame

As P3.

---

## P5 - VBSTRT ($1CC) sets the line at which vertical blanking starts (ECS)

### 1. Proposition

In the Enhanced Chip Set, with BEAMCON0 bit 12 (VARVBEN) set, the value written to
VBSTRT at $DFF1CC is the vertical line at which vertical blanking starts, so no
playfield data is shown from that line on. Claim opus:B-vbstrt.addr (p.293:
"VBSTRT 1CC W A(E) Vertical line for VBLANK start"). The quotations of
opus:C-vbstrt.c01 and sol:C-scan.c20 (p.304: "VBSTRT W A Vertical line position for
VBLANK start") carry the same clause and add the register's write-only access and its
chip; they are declined whole (see the start of this document), and the clause itself
is graded here under opus:B-vbstrt.addr.

What makes VBSTRT take effect: p.305, "12 VARVBEN Use VBSTRT/STOP disable hard
window stop". What vertical blanking does to the picture is not stated on p.304 or
p.305; the only page that states it is p.79: "No data can be displayed in the
vertical blanking area." The expected value below uses exactly that and nothing
more: it grades the absence of playfield colours, not what the output shows
instead.

### 2. Stimulus

N = $A0 (run "V1") and N = $B0 (run "V2"). No page gives a bit layout for VBSTRT or
VBSTOP (p.304 gives one for HTOTAL only). The line is therefore written in the low
bits and in the high byte alike: VBSTRT = $A0A0 (V1) or $B0B0 (V2), the same line under
a low-bit field of 8 to 11 bits and under a high-byte field (as DIWSTRT/DIWSTOP and the
Copper's VP carry a line in the high byte). Only lines below $100 are used.

1. Start-up writes.
2. COLOR00 = C0, COLOR01 = C1.
3. Bitplane data: 196 lines of 20 words, every word $FFFF.
4. BPLCON1 = $0000, BPLCON2 = $0000, BPLCON0 = $1000.
5. DIWSTRT = $2C81, DIWSTOP = $F0C1, DDFSTRT = $0038, DDFSTOP = $00D0, BPL1MOD =
   $0000. VSTOP = $F0 is used in PAL as well as NTSC: under the forced-MSB rule of
   p.59 ("forcing the MSB of the stop position to be the complement of the next
   MSB") $F0 means line $0F0, and without that rule it means line $F0 too, so the
   window ends at line 240 whether or not VARVBEN's "disable hard window stop"
   changes that rule.
6. VBSTRT = N * $101 ($A0A0, $B0B0), VBSTOP = (N + 8) * $101 ($A8A8, $B8B8).
7. BEAMCON0 = $1020 in PAL, $1000 in NTSC (VARVBEN = 1, bit 5 PAL per configured
   standard, HARDDIS = 0, every other bit 0). The p.305 warning "Do not write to
   this register" is advice to application software; there is no other way to make
   VBSTRT take effect.
8. Copper list:

        MOVE BPL1PTH, ... ; MOVE BPL1PTL, ...
        MOVE COLOR01, C1
        WAIT(N-1, $30)
        MOVE COLOR01, C2
        WAIT(N+1, $30)
        MOVE COLOR01, C3
        WAIT $FFFF,$FFFE

   Horizontal position $30 lies before the displayed part of the line (p.23: "The
   standard screen (320 pixels wide) has an unused horizontal portion of $04 to $47
   (during which only the background color is displayed)"), so line N-1 shows data
   in C2 and every line from N+1 on shows data in C3, wherever data is shown. $30
   is also far enough from the line start that the VPOSR reads bracketing the
   Copper's MOVE (P5-b) fall into the same line.
9. Copper start; DMACON = $8380.
10. Four times: poll INTREQR for VERTB (as in P3), clear it (INTREQ = $0020),
    then poll VHPOSR until its V7-V0 field is at least N-3, then execute 200 times
    `MOVE.L $DFF004,(A0)+`.
11. Idle loop.

Run in S1 and S2.

### 3. Observation

The video output from the 2nd to the 6th vsync assertion after the first Copper
instruction fetch, in beam lines (by hsync, one per beam line as described under
Video output). Per beam line the test records which of C1, C2, C3 occur in any
sample of the line. No page says at which line vsync occurs, so the checks below
do not use vsync to delimit fields.

### 4. Expected value

In V1 and in V2:

1. Every beam line that contains a sample showing C2 is immediately preceded by a
   beam line that contains C1 and not C2, and immediately followed by a beam line
   that contains none of C1, C2 and C3.
2. The span contains at least three beam lines with C2, and consecutive ones are
   exactly one field apart (312 or 313 lines in PAL, 262 or 263 in NTSC), i.e. there
   is exactly one such line per field.

The C2 line is beam line N-1 (reference frame). Together: line N-1 shows data and
line N does not, so vertical blanking starts at line N. Sources: p.304, "VBSTRT W
A Vertical line position for VBLANK start"; p.293, the address $1CC; p.305,
VARVBEN "Use VBSTRT/STOP"; p.79, "No data can be displayed in the vertical
blanking area."

Quantifier: two values of VBSTRT, at least three fields each, PAL and NTSC, both
cores. Only the start line is graded. Lines N+1 to N+7 are not graded: whether
they are blank depends on VBSTOP, which this package has no claim for. Lines at or
above $100 are not tested, because the bit layout is not stated.

### 5. Preconditions

- **P5-a (data is displayed around N):** beam lines N-10 to N-2 each contain a
  sample showing C1.
- **P5-b (the anchor):** in each of VERTB detections 2 to 4, the Copper's fetch of
  the MOVE to COLOR01 that follows WAIT(N-1,$30) has time t_c2; the last VPOSR read
  whose store precedes t_c2 and the first whose store follows it both have
  V = N-1. Likewise for the MOVE after WAIT(N+1,$30): V = N+1. (The Copper
  instruction fetches and the VPOSR stores are both seen at the Chip-memory port.)
- **P5-M (the blocks are the program's own):** the stores to the measurement buffer
  from $0C1000 up are exactly four blocks; block k (k = 0..3) is the marker word k
  followed by 200 long words, each stored high half then low half at consecutive word
  addresses. P5-b, P5-c and P5-MAIN wait for it.
- **P5-c (video standard):** the field-length check on consecutive VERTB detections
  gives the configured standard (the BEAMCON0 write kept bit 5 right). It is applied to
  the times of the consecutive detection markers, each of which follows a VERTB
  detection and the VHPOSR poll, with the tolerance of one line plus the period of the
  longer of the program's two polling loops (the `INTREQR` loop and the `VHPOSR` loop),
  measured from their instruction fetches; and to the consecutive vsync assertions
  with the tolerance of one line.
- **P5-d (configuration):** the harness is configured ECS. p.298 lists VBSTRT and
  BEAMCON0 as "new" registers of the Enhanced Chip Set. This is a setting of the host
  that the test itself chooses, not a measurement of the design: it is printed as a
  RECORDED line and is not graded, and `P5-MAIN` does not wait for it.

### 6. Contrasting cases

- **Main check, VARVBEN:** BEAMCON0 = $0020 (PAL) / $0000 (NTSC). p.305 makes
  VBSTRT depend on VARVBEN, so line N shows data in C2: line N-1 is followed by a
  line containing C2, and line N is preceded by a line containing C2; item 1 fails.
  P5-a, P5-b, P5-c still hold.
- **Main check, value:** VBSTRT = N + 16 with the Copper markers unchanged. Blanking
  then starts at line N + 16, line N shows C2, and item 1 fails.
- **P5-a:** DMACON = $8280 (no BPLEN); no line contains C1.
- **P5-b:** replace WAIT(N-1,$30) by WAIT(N-2,$30); the reads around t_c2 show
  V = N-2.
- **P5-c:** BEAMCON0 with bit 5 opposite to the configured standard; the field
  length is the other standard's.
- **P5-M:** the program stores every marker with bit 8 set ($0100 + k); the line
  reports it and the lines that wait for it are not graded.
- **P5 fixture:** the program stores a start word other than $5A05; the run completes
  and the fixture line reports it.

### 7. Reference frame

Line numbers are beam-counter lines. The line N-1 is anchored on the Copper's
colour change: the Copper waits until "the video beam counters are equal to (or
greater than) the coordinates specified" (p.22), and P5-b confirms with the beam
counter itself (VPOSR/VHPOSR, LPEN = 0) that the change happens in line N-1. The
output line that first shows C2 is therefore the output of beam line N-1, and the
next beam line is N. Without P5-b, a Copper change one line late and a blank one
line late would together pass item 1 and item 2; P5-b excludes that.

---

## Where the pages disagree or leave things open

- **End of vertical blanking.** p.239: "released at the end of internal vertical
  blanking (vertical position 20)". p.219: "The range starts at line 0 and ends at
  line 20 for NTSC or line 25 for PAL." p.79 Table: "Vertical Blank Stop $15 (21)
  [NTSC] $1D (29) [PAL]". P3/P4 count only lines 1-19, inside every version.
- **Copper restart.** p.25 says in one note both "At the start of each vertical
  blanking interval, COP1LC is automatically used" and "when the end of vertical
  blanking occurs, the Copper is automatically forced to restart"; p.30 says "When
  the screen display ends and vertical blanking starts". No check here depends on
  when in vertical blanking the restart happens.
- **"0<V20"** on p.239 is printed so, twice; read as 0 < V < 20.
- **p.239's comparison value** "$10500 (V=261)" fits an NTSC field only; no PAL
  value is given.
- **Chip identification.** p.238: "Chip ID code. Do not depend on value!"; p.299
  lists the values ("20 for PAL, 30 for NTSC" for the ECS Agnus). No check uses
  them.
- **BPLCON0's address** on p.333 ($DFF104) against $100 everywhere else (see Common
  definitions).
- **Bit 9 in ECS.** The ECS BPLCON0 table on p.300 leaves bits 9, 8, 7, 1 and 0
  without a name; p.271 names bit 9 COLOR, p.90 COLOR_ON. The blank on p.300 is
  treated as silence, not as a change. (The text layer of p.63 writes "bit 9
  (COLOR_ON)"; the printed p.63 writes only "bit 9".)
- **Power-up values** of BEAMCON0, BPLCON1, BPLCON2 and all BPLCON0 bits other than
  LPEN, LACE and ERSY are not stated; the programs write the registers they depend
  on.
- **The colour clock at which V steps after H wraps** is not stated on any page; the
  counting lines grade only what the pages fix and print the pairs with V unchanged and H
  lower as MEASURED lines.
- **VBSTRT's bit layout** is not given on any page (p.293, p.298, p.304); P5 writes the
  line in the low bits and in the high byte alike.
- **What vertical blanking shows on the RGB output** is not stated; only p.79's "No
  data can be displayed" is used.
- **VARVBEN's "disable hard window stop"** is not explained on any page; P5's
  display window is chosen so that its end does not depend on it.

---

## Build notes

### Programs

`Makefile` (`TEST_CPP`/`PROGS`), `test_ch3_playfield_system.cpp`, `playfield.i`, and
the `.s` programs the Makefile lists: `prog_lpen_core.s` and its one-line shims for
P3/P4 (base and contrast variants), `prog_playfield_core.s` and its shims for P1/P2
(2-colour low resolution and 16-colour high resolution, PAL and NTSC, and contrast
variants), `prog_p5_core.s` and its shims for P5 (base and contrast variants). The
Copper lists and the bitplane data of P1, P2 and P5 are preloaded by the test, because
a CPU write below $80000 is lost to the overlay.
Every program that starts the Copper writes COP1LC as one long word to COP1LCH, then the
COPJMP1 strobe, then DMACON.

### Lines and gating

Every proposition prints a fixture line per run, one line per precondition, one
component line per system configuration (per display configuration and system
configuration for P1) and one aggregate line. The aggregate carries the claim ids; no
component line does. It prints FAIL as soon as one graded component failed, PASS when
every expected component was graded and passed (P1: 16, P2: 4, P3: 8, the L0 line
`P3-LIVE` and `P3-MAIN` in each of the four system configurations, P4: 4, P5: 4), and is
otherwise not graded; a not graded aggregate prints a RECORDED line that names its ids.
A line whose precondition failed prints `RECORDED ... NOT GRADED`, never PASS or FAIL.
The fixture gates every line that reads its run: when it fails, every precondition and
component line of that run prints NOT GRADED, including the lines that read the
program image or the Copper-fetch log (an empty image or an empty log never passes).
The preconditions each line waits for:

| line | waits for |
|---|---|
| `P1-MAIN` | `P1-a`, `P1-b`, `P1-c`, `P1-d`, `P1-e` |
| `P2-I1`, `P2-I3` | `CAL-VIDEO`, `P2-a` |
| `P2-I2` | `CAL-VIDEO`, `P2-a`, `P2-b` |
| `P3-a`, `P3-L`, `P3-d` | the fixture |
| `P3-b`, `P3-c` | the fixture, `P3-L` |
| `P3-LIVE` | the fixture, `P3-a` |
| `P3-MAIN` | the fixture, `P3-a`, `P3-L`, `P3-b`, `P3-c`, `P3-d` |
| `P4-a`, `P4-L`, `P4-B-PROGRAM`, `P4-B-COPPER` | the fixture |
| `P4-c` | the fixture, `P4-L` |
| `P4-MAIN` | the fixture, `P4-a`, `P4-L`, `P4-c`, `P4-B-PROGRAM`, `P4-B-COPPER`, and `P3-MAIN` passed in the same system configuration |
| `P5-MAIN` | `CAL-VIDEO`, `P5-a`, `P5-b`, `P5-c` |

`P2-I1` is `P2`'s item 1 (at least three complete blocks, every one of exactly H beam
lines), `P2-I2` its item 2 (every pixel of every data line), `P2-I3` its item 3 (the two
border samples). `P2-I1` and `P2-I3` do not wait for `P2-b`: they grade the number of
lines and the border, not which memory words were displayed.

`P3-LIVE` (the L0 line) is a component of the P3 aggregate and is not a precondition of
`P3-MAIN`. `P4-MAIN` runs in the plain run, after P3, and prints NOT GRADED in a run
that does not execute group P3. `P5-d` is a RECORDED line (the harness configuration
the run was started with) and is not a precondition of anything.

Two claims are declined and printed as RECORDED lines that name their ids:
`sol:3-composite.c7` (P2) and `opus:C-vbstrt.c01`, `sol:C-scan.c20` (P5).

### Units and tolerances

- `CAL-CC` measures the period of the 7 MHz clock enable at the port `clk7_en` (4
  clk_sys cycles) and takes one colour clock as two of its periods (p.24: "One
  horizontal count takes one cycle of the system clock (processor is twice this)").
  Every time in colour clocks that the package uses (the windows of P3 and P4, the
  line of 227 cc, the 30 cc spread) is converted with this value.
- `CAL-VIDEO` checks the units of the scan doubler against the pages in each P2 and P5
  run: between consecutive vsync assertions the output holds 2 hs periods per line of
  the page's line count (312 or 313 lines PAL, 262 or 263 NTSC, p.24); each hs period
  is half a beam line, so twice its length is exactly 227 or 228 cc (p.23); a
  low-resolution pixel is half a colour clock (p.239: "2 low resolution pixels"), so the
  doubler line length divided by the 2*227 pixels of a line gives k, the samples per
  low-resolution pixel (k = 2). The factor 2 is a fact of the harness's scan doubler, not
  of any page; the run asserts it against the pages' line counts.
- The field-length check of P3 and P4 adds to its tolerance of one line (227 cc) the
  period of the detecting loop. `P3-L` and `P4-L` measure that period from the
  instruction fetches of the `MOVE.W INTREQR(A0),D0` loop (a CPU read of a custom
  register does not reach the port, the loop's own fetches do) and require at least
  100 iterations in each run; `P3-b`, `P3-c` and `P4-c` wait for them. The periods they
  measure are 136 clk_sys cycles on fx68k and 120 on TG68K. A
  detection of vsync (P1-e, P2-a, P5-c) has no loop and uses the tolerance of one line.
  The package holds no tuned margin.
- A read lies inside a window when the two stores that bound it, the one before it and
  the one after it, both lie inside it (Common definitions). A read is placed by the
  system-clock time of the low-half store of its own iteration and by that of the
  iteration before it (the marker store for the first read).

### P3-LIVE and P4-MAIN: the counting lines

Each window of L0 (`P3-LIVE`) and of the power-up program (`P4-MAIN`) is graded by three
criteria on consecutive counted reads: (i) no two consecutive reads are equal (p.239:
"Read (long) VPOSR twice. If both values are not the same ..."); (ii) V never decreases and
steps by at most 1 between consecutive reads (reads are less than one line apart); (iii)
within one V value, H increases except for at most one decrease, the wrap of H (p.23). No
page fixes the colour clock at which V steps after H wraps, so a pair with V unchanged and
H lower is not graded: it is printed as a MEASURED line with its H values (`P3-LIVE S<n>
pairs with V unchanged and H lower`, `P4-MAIN S<n> pairs ...`), and each window's counts
(reads, equal pairs, V-bad pairs, H decreases beyond the first, V above 19) are printed
beside the verdict.

Measured on the repository's chipset, in every window of every system configuration: no
equal pair, no V step other than 0 or 1, at most one H decrease per V value. The pairs
with V unchanged and H lower are all H 210 -> 0 on fx68k (the read at H = 0 still shows the
previous line's V) and all H 210 -> 1 on TG68K (the read at H = 1 still shows it). V8..V0 and
H8..H1 are the two bytes of one 16-bit register (VHPOSR, p.238), so these pairs are not a
long-word read torn between two bus cycles. A private chipset whose vertical position
counts at the clock edge at which the horizontal position wraps shows no such pair and
meets the criteria as well; a frozen counter (the L1 program on the private chipset with
the latch) fails criterion (i). The criteria hold on both counter phases and fail on a
frozen latch.

### Contrasts that differ from the text of section 6

Every mutation record in the test header names its kind: `stimulus` changes what the
design is asked (a program, a Copper list, memory contents), `oracle` changes an interval
or a value the comparator is fed, `implementation` runs a private copy of the chipset,
`none` is an assertion no stimulus can break. A selector that moves more than its own
line says so in its record; where a line waits for the one a selector breaks, it prints
NOT GRADED.

- **P1 main.** The contrast keeps the change of bit 9 and adds a change the pages call
  visible: run "1"'s Copper list also sets COLOR00 (A) or COLOR01 (B, C, E) to the
  value one blue step away half way down the picture (after WAIT line $80). The upper
  half still shows the original colours, so `P1-d` holds, the BPLCON0 words are
  unchanged, so `P1-b` holds, and `P1-MAIN` is graded and prints FAIL. Replacing the
  change of bit 9 would have made `P1-b` fail and left `P1-MAIN` not graded.
- **P1-a, P1-e.** The dedicated contrasts `P1A` (a genuine PAL run's sync samples
  against a genuine NTSC run's) and `P1E` (a genuine PAL run's field length graded
  against NTSC's values, in both runs) print their own fixture line first and grade only
  when it passes. `P1E` changes the values the comparator is fed, so its record is an
  `oracle` record. The stimulus route for P1-e is `P1ESTD`: the program of configuration
  B at S1 writes BEAMCON0 with bit 5 opposite the configured standard, so the field length
  is the other standard's; P1-c B prints FAIL too.
- **P1-c.** `P1C` turns bitplane DMA off in B, C and E and gives A a one-plane Copper
  list; the one-plane list carries other BPLCON0 words, so P1-b A prints FAIL too, and
  without bitplane data P1-d prints FAIL in B, C and E.
- **P2 contrasts.** `P2B` (BPL1MOD = $0002) turns P2-b and P2-I1 red and leaves P2-I2 not
  graded; `P2ITEM1` (VSTOP one line lower) turns P2-I1 and P2-b red (the buffer is read
  for H-1 lines) and leaves P2-I2 not graded, P2-I3 holds; `P2EDGE` (window 32 pixels
  wider) turns P2-I3 and P2-b red and leaves P2-I2 not graded; `P2A` turns P2-a red and
  leaves P2-I1, P2-I2 and P2-I3 not graded.
- **P3-b.** `P3B` moves the window end of the comparator and is an `oracle` record;
  `P3BLATE`, the L0 program storing its marker about 26 lines after the VERTB detection,
  is its stimulus route to reads with V above 19.
- **P3-LIVE.** The contrast `P3LIVE` runs on the private chipset: the L1 program stands in
  for the L0 program, the L0 windows hold one frozen value and criterion (i) fails; the
  frozen value has V above 19, so P3-b prints FAIL too and P3-MAIN is not graded.
- **P3-L, P3-c, P4-L.** `P3L` leaves P3-b, P3-c and P3-MAIN not graded; `P3C_WRONGSTD`
  also fails P3-b through its field-length part; `P4L` leaves P4-c not graded.
- **P4 main.** The contrast that inserts a BPLCON0 write is the contrast of
  `P4-B-PROGRAM`. A contrast of `P4-MAIN` has to leave every precondition true, so it is
  an implementation record: a second private chipset in which LPEN is 1 after reset.
- **P2 item 3.** Section 6 has no contrast for the border samples; the contrast widens
  the display window by 32 pixels and fetches one more word per line, so the sample
  after the 320th pixel shows data.
- **CAL-VIDEO.** `CALVID` grades the S1 run's line counts against NTSC's and is an
  `oracle` record; its stimulus route is `P5C`, the P5 program that writes BEAMCON0 with
  bit 5 opposite the configured standard (CAL-VIDEO V1 S1 and P5-c V1 S1 print FAIL).
- **CAL-CC.** No stimulus in the design's input space makes the 7 MHz clock enable
  irregular: the line is unfalsifiable at this boundary. `CALCC` reads the port on an
  irregular subset of the cycles, which shows the comparator is live and asks the design
  nothing different.

### Controls on a chipset that follows the pages

On the repository's chipset the lines `P3-MAIN` and `P5-MAIN` are red, and `P4-MAIN` is
not graded because `P3-MAIN` does not pass there, so a stimulus change cannot make them
redder. Each has a control that runs its comparator on a live input from a different
mechanism: a private copy of the chipset sources, outside the repository, with three
changes. (1) BPLCON0 bit 3 enables a latch of the beam position at the end of the field,
the latch holds until the start of line 20, and BEAMCON0 bit 13 makes a read ignore it
(pp.239, 305). (2) The vertical position counts at the clock edge at which the horizontal
position wraps. (3) With BEAMCON0 bit 12 set, VBSTRT starts and VBSTOP ends vertical
blanking (pp.293, 304, 305); the register holds the low 11 bits of the value written, so
$A0A0 and $B0B0 name the lines $A0 and $B0 there. A run of P3, P4 and P5 on that copy
(`NMH_ONLY=P3P4P5`) prints 84 passed, 0 failed on both cores: every line of the three propositions
passes, and the stimulus contrasts `P3MAIN` (the L0 program stands in for L1), `P3LIVE`,
`P3B`, `P3BLATE` and `P5MAIN` (no VARVBEN) print FAIL through the unchanged comparators.
A second private copy, in which the latch is enabled after reset, runs P3 and P4
(`NMH_ONLY=P3P4`) and prints 54 passed, 5 failed on both cores: every fixture and precondition
passes, `P3-MAIN` passes, and `P4-MAIN` in S1 to S4 and the P4 aggregate print FAIL.

### Measured, per core

| Line | Page(s) | fx68k | TG68K |
|---|---|---|---|
| CAL-CC | p24 | PASS (1/1) | PASS (1/1) |
| CAL-VIDEO (P2 and P5 runs) | p23, p24, p239 | PASS (8/8) | PASS (8/8) |
| P3 fixture | (spec) | PASS (4/4) | PASS (4/4) |
| P3-a (>=100 reads/window, spread) | (spec) | PASS (4/4) | PASS (4/4) |
| P3-L (polling loop measured) | (spec) | PASS (4/4) | PASS (4/4) |
| P3-b (L0: V in 1-19; field length) | p23, p24, p239 | PASS (4/4) | PASS (4/4) |
| P3-LIVE (L0: a running counter, three criteria) | p23, p216, p239 | PASS (4/4) | PASS (4/4) |
| P3-c (LPENDIS=0, bit 5 in code and fetch, field length), S1, S2 | p304, p305 | PASS (2/2) | PASS (2/2) |
| P3-d (BPLCON0 written once, no Copper write) | (spec) | PASS (4/4) | PASS (4/4) |
| P3-MAIN (LPEN=1: every counted read in a window is the same 17-bit value) | p271, p89, p229, p239 | FAIL (4/4) | FAIL (4/4) |
| P3 aggregate (8 components) | p271, p89, p229, p239 | FAIL (1/1) | FAIL (1/1) |
| P4 fixture | (spec) | PASS (4/4) | PASS (4/4) |
| P4-a (>=100 reads/window, spread) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-L (polling loop measured) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-c (field length), S1, S2 | (spec) | PASS (2/2) | PASS (2/2) |
| P4-B-PROGRAM (no write to $DFF100) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-B-COPPER (no Copper MOVE to $100 in the list at $020000) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-MAIN (power-up: a running counter, V in 1-19; waits for P3-MAIN) | p271, p216, p239 | NOT GRADED (4/4) | NOT GRADED (4/4) |
| P4 aggregate | p271 | NOT GRADED (1/1) | NOT GRADED (1/1) |
| P1 fixture, configs A, B, C, E x S1-S4 | p59-p62 | PASS (16/16) | PASS (16/16) |
| P1-a (sync samples identical) | p63 | PASS (16/16) | PASS (16/16) |
| P1-b (BPLCON0 words fetched as tabled) | p63, p64 | PASS (16/16) | PASS (16/16) |
| P1-c (bitplane reads per DMA field) | p61, p62 | PASS (16/16) | PASS (16/16) |
| P1-d (picture not uniform) | p46, p55 | PASS (16/16) | PASS (16/16) |
| P1-e (field length, both runs) | p24 | PASS (16/16) | PASS (16/16) |
| P1-MAIN (bit 9 has no effect on RGB) | p63, p64 | PASS (16/16) | PASS (16/16) |
| P1 aggregate | p63, p64 | PASS (1/1) | PASS (1/1) |
| P2 fixture, config B x S1-S4 | p61, p62 | PASS (4/4) | PASS (4/4) |
| P2-a (P1-b and P1-e, both runs) | p63 | PASS (4/4) | PASS (4/4) |
| P2-b (buffer read once per word, in order) | p61, p62 | PASS (4/4) | PASS (4/4) |
| P2-I1 (three complete blocks of H lines) | p49, p59, p63 | PASS (4/4) | PASS (4/4) |
| P2-I2 (every pixel of every data line) | p46, p55, p61, p272 | PASS (4/4) | PASS (4/4) |
| P2-I3 (border samples show C0) | p46, p55 | PASS (4/4) | PASS (4/4) |
| P2 aggregate | p63 | PASS (1/1) | PASS (1/1) |
| P5 fixture, V1 V2 x S1 S2 | (spec) | PASS (4/4) | PASS (4/4) |
| P5-a (data in lines N-10 to N-2) | p79 | PASS (4/4) | PASS (4/4) |
| P5-b (anchor: V = N-1 / N+1) | p22 | PASS (4/4) | PASS (4/4) |
| P5-c (field length) | p24 | PASS (4/4) | PASS (4/4) |
| P5-d (harness configured ECS; a host setting) | p298 | RECORDED (4/4) | RECORDED (4/4) |
| P5-MAIN (VBSTRT sets the blanking line) | p293, p304, p305, p79 | FAIL (4/4) | FAIL (4/4) |
| P5 aggregate | p293, p304, p305, p79 | FAIL (1/1) | FAIL (1/1) |

fx68k: 211 passed, 10 failed. TG68K: 211 passed, 10 failed.

`P4-MAIN` and the P4 aggregate are NOT GRADED on the repository's chipset because `P3-MAIN`
does not pass there; they are graded on the private chipsets (below). The RECORDED lines
that decline claims are `sol:3-composite.c7` (P2), `opus:C-vbstrt.c01` and
`sol:C-scan.c20` (P5); `P5-d` prints one RECORDED line per run.

### Mutations, per core

Every mutation is run from `obj_dir_fx68k`/`obj_dir_tg68k` directly (fx68k needs its own
microcode there), with `NMH_MUTATE` set; none runs during a plain `make test`. Each is
verified to print FAIL through the clause it targets, on both cores; where a record in the
test header names further lines that move with it (a line that waits for the targeted one
prints NOT GRADED, a line the stimulus also changes prints FAIL), the run showed them on
both cores too. Selectors marked `private` run on the private chipset of the previous
section, where the lines they target are live; `P3B` and `P3BLATE` are run on both chipsets.

| `NMH_MUTATE` | Chipset | Targets | fx68k | TG68K |
|---|---|---|---|---|
| `CALCC` | repository | CAL-CC | FAIL | FAIL |
| `CALVID` | repository | CAL-VIDEO | FAIL | FAIL |
| `P1FIX` | repository | P1 | FAIL | FAIL |
| `P1A` | repository | P1-a | FAIL | FAIL |
| `P1B` | repository | P1-b | FAIL | FAIL |
| `P1C` | repository | P1-c | FAIL | FAIL |
| `P1D` | repository | P1-d | FAIL | FAIL |
| `P1E` | repository | P1-e | FAIL | FAIL |
| `P1ESTD` | repository | P1-e | FAIL | FAIL |
| `P1MAIN` | repository | P1-MAIN, opus:3-a1000.c04 | FAIL | FAIL |
| `P2FIX` | repository | P2 | FAIL | FAIL |
| `P2A` | repository | P2-a | FAIL | FAIL |
| `P2B` | repository | P2-b | FAIL | FAIL |
| `P2ITEM1` | repository | P2-I1 | FAIL | FAIL |
| `P2FLIP` | repository | P2-I2, opus:3-a1000.c05 | FAIL | FAIL |
| `P2EDGE` | repository | P2-I3 | FAIL | FAIL |
| `P3FIX` | repository | P3 | FAIL | FAIL |
| `P3A` | repository | P3-a | FAIL | FAIL |
| `P3L` | repository | P3-L | FAIL | FAIL |
| `P3B` | repository | P3-b | FAIL | FAIL |
| `P3BLATE` | repository | P3-b | FAIL | FAIL |
| `P3C_LPENDIS` | repository | P3-c | FAIL | FAIL |
| `P3C_WRONGSTD` | repository | P3-c | FAIL | FAIL |
| `P3D` | repository | P3-d | FAIL | FAIL |
| `P3MAIN` | private | P3-MAIN, opus:A-bpc.b03.c0 | FAIL | FAIL |
| `P3LIVE` | private | P3-LIVE, opus:A-bpc.b03.c0 | FAIL | FAIL |
| `P3B` | private | P3-b | FAIL | FAIL |
| `P3BLATE` | private | P3-b | FAIL | FAIL |
| `P4FIX` | repository | P4 | FAIL | FAIL |
| `P4A` | repository | P4-a | FAIL | FAIL |
| `P4L` | repository | P4-L | FAIL | FAIL |
| `P4C` | repository | P4-c | FAIL | FAIL |
| `P4B_PROG` | repository | P4-B-PROGRAM | FAIL | FAIL |
| `P4B_COP` | repository | P4-B-COPPER | FAIL | FAIL |
| `P5FIX` | repository | P5 | FAIL | FAIL |
| `P5A` | repository | P5-a | FAIL | FAIL |
| `P5B` | repository | P5-b | FAIL | FAIL |
| `P5C` | repository | P5-c | FAIL | FAIL |
| `P5MAIN` | private | P5-MAIN, opus:B-vbstrt.addr | FAIL | FAIL |
| private chipset, plain run (`NMH_ONLY=P3P4P5`) | private | every line of P3, P4, P5 | 84 passed, 0 failed | 84 passed, 0 failed |
| second private chipset, LPEN 1 after reset (plain run, `NMH_ONLY=P3P4`) | private | P4-MAIN, opus:A-leg.lpen2 | FAIL, every precondition holds (54 passed, 5 failed) | FAIL, every precondition holds (54 passed, 5 failed) |
