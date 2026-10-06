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
| P2 | the RGB output is the correct picture whether bit 9 is set or clear | opus:3-a1000.c05, sol:3-composite.c7 |
| P3 | BPLCON0 bit 3 is LPEN, the light pen enable: setting it latches the beam position read through VPOSR/VHPOSR | opus:A-bpc.b03.c0, sol:A-bplcon.c49, opus:A-leg.lpen1, sol:A-bplcon.c72, opus:3-r0.c09, sol:3-regsum-bplcon0.c6 |
| P4 | LPEN is 0 after power-up | opus:A-leg.lpen2, sol:A-bplcon.c73 |
| P5 | the value written to VBSTRT ($1CC, ECS) is the line at which vertical blanking starts | opus:B-vbstrt.addr, opus:C-vbstrt.c01, sol:C-scan.c20 |

---

## Claims not turned into a proposition

No claim is left out entirely. The following parts of claims give nothing a test can
grade, and no test may assert anything about them:

- **"W" (write-only) in opus:C-vbstrt.c01 and sol:C-scan.c20** (p.304, and p.293,
  where the legend on p.287 reads "W=write-only"). No page says what a read of
  $DFF1CC returns. A test must not read VBSTRT and must not expect any value from
  such a read.
- **"A" (Agnus) in opus:C-vbstrt.c01 and sol:C-scan.c20.** Which chip holds a
  register is not visible at any output the test has (Chip-memory port, video,
  audio). Not checkable.
- **"regardless of the output of the composite video signal" in opus:3-a1000.c05
  and sol:3-composite.c7** (p.63). The composite video output is not among the
  outputs a test can see. The only handle a test has on the composite signal is
  its enable bit, BPLCON0 bit 9; P2 therefore checks the RGB picture with bit 9
  set and with bit 9 clear, and nothing about the composite signal itself.
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
- **P1-e (video standard):** the field-length check (vsync to vsync) matches the
  configured standard.

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
- **P1-c:** DMACON = $8280 (BPLEN not set); no bitplane reads occur.
- **P1-d:** in B, fill the bitplane with $0000; no sample shows C1.
- **P1-e:** check the field length against the other standard's values; it fails.

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

- **P2-a:** P1-b and P1-e hold for the run.
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
  fails.
- **Item 1 as a count:** DIWSTOP with VSTOP one line lower ($2BC1 in PAL, $F3C1 in
  NTSC); p.59 predicts H-1 data lines per field, and item 1 fails while items 2 and
  3 still hold for the lines shown.

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
   - store a marker word (the iteration number) into Chip memory; the time of this
     store is t_f;
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

Not graded, because the pages leave it open:

- **the frozen value itself.** "the end of the display field" is not defined as a
  position; p.239's comparison value "$10500 (V=261)" belongs to an NTSC field and
  no PAL value is given; and whether a trigger occurs is not under the test's
  control.
- **the release after position 20.** p.239: "when the light pen latch is enabled,
  the VPOSR register may be latched at any time, and cannot be relied on as a
  counter." A trigger may freeze the value again at any point after the release,
  and the test cannot know whether one occurred.

### 5. Preconditions

- **P3-a (enough reads, spread over the window):** each window holds at least 100
  counted reads; the first lies within 30 cc after the window start, the last
  within 30 cc before its end.
- **P3-b (the anchor):** in L0, every counted read has V (V8..V0) between 1 and 19
  inclusive, and the 17-bit values increase strictly from read to read (p.216:
  VHPOSR reads "the counter that is producing the beam on the screen"). In both L0
  and L1, consecutive t_f satisfy the field-length check.
- **P3-c (ECS only):** the program's BEAMCON0 write carries bit 13 = 0 and bit 5 =
  the configured standard (checked on the program's code and on the fetched
  immediate data word), and the field-length check after it gives the configured
  standard.
- **P3-d:** the program contains no other write to $DFF100, and no Copper
  instruction fetched at the Chip-memory port during the run is a MOVE to $100
  (first word $0100), so nothing else can have written BPLCON0.

### 6. Contrasting cases

- **Main check:** run L0. The pages predict counting values (p.216); the reads in
  each window increase strictly, so the "all equal" check fails. Only bit 3 differs
  between L1 and L0, so a pass of L1 and a fail of L0 locate the enable at bit 3.
- **P3-a:** reduce the burst to 20 reads; fewer than 100 reads fall into the window.
- **P3-b:** evaluate the anchor with the window end moved to t_f + 30 * 227 cc;
  reads with V above 19 appear in L0.
- **P3-c (ECS):** BEAMCON0 = $2020 (PAL) / $2000 (NTSC), LPENDIS = 1. p.305 predicts
  that the latched value is ignored on a read; the L1 windows then show strictly
  increasing values and the main check fails, while the program finishes and
  P3-a and P3-b still hold. Separately, BEAMCON0 with bit 5 set opposite to the
  configured standard makes the field-length check report the other standard
  (p.304: "NTSC (525 lines ...) PAL (625 lines ...)").
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

In every window, the 17-bit values increase strictly from read to read, and each
has V between 1 and 19. From p.271, "reset on power up": LPEN = 0, so p.239's
latch is not enabled, and VHPOSR reads "the counter that is producing the beam on
the screen" (p.216).

Quantifier: the page states one event, power-up; the test has one per run, graded
in four system configurations and with both cores.

### 5. Preconditions

- **P4-a:** P3-a and P3-c.
- **P4-b:** the program contains no write to $DFF100; no Copper instruction
  fetched at the Chip-memory port from power-up to the end of the last window is a
  MOVE to $100 (no page states DMACON's power-up value, so the Copper may run
  before the program's first DMACON write); the first instructions the CPU
  fetches after power-up are the test program's.
- **P4-c:** consecutive t_f satisfy the field-length check.

### 6. Contrasting cases

- **Main check:** insert BPLCON0 = $0008 before step 4 (the P3 L1 program). The
  windows then hold one frozen value (P3's expectation), so the strictly-increasing
  check fails; the program finishes and P4-a, P4-c hold. This shows the check would
  see an LPEN of 1 left over from power-up.
- **P4-a, P4-c:** as in P3.
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
playfield data is shown from that line on. Claims opus:B-vbstrt.addr (p.293:
"VBSTRT 1CC W A(E) Vertical line for VBLANK start"), opus:C-vbstrt.c01 and
sol:C-scan.c20 (p.304: "VBSTRT W A Vertical line position for VBLANK start").

What makes VBSTRT take effect: p.305, "12 VARVBEN Use VBSTRT/STOP disable hard
window stop". What vertical blanking does to the picture is not stated on p.304 or
p.305; the only page that states it is p.79: "No data can be displayed in the
vertical blanking area." The expected value below uses exactly that and nothing
more: it grades the absence of playfield colours, not what the output shows
instead.

### 2. Stimulus

N = $A0 (run "V1") and N = $B0 (run "V2"). The page gives no bit layout for VBSTRT
(p.304 gives one for HTOTAL only); "vertical line position" is read as the line
number in the register's low bits, and only values below $100 are used so that no
bit above bit 7 is involved.

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
6. VBSTRT = N, VBSTOP = N + 8.
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
they are blank depends on VBSTOP, which this package has no claim for. Values of
VBSTRT at or above $100 are not tested, because the bit layout is not stated.

### 5. Preconditions

- **P5-a (data is displayed around N):** beam lines N-10 to N-2 each contain a
  sample showing C1.
- **P5-b (the anchor):** in each of VERTB detections 2 to 4, the Copper's fetch of
  the MOVE to COLOR01 that follows WAIT(N-1,$30) has time t_c2; the last VPOSR read
  whose store precedes t_c2 and the first whose store follows it both have
  V = N-1. Likewise for the MOVE after WAIT(N+1,$30): V = N+1. (The Copper
  instruction fetches and the VPOSR stores are both seen at the Chip-memory port.)
- **P5-c (video standard):** the field-length check on consecutive VERTB detections
  gives the configured standard (the BEAMCON0 write kept bit 5 right).
- **P5-d (configuration):** the harness is configured ECS. p.298 lists VBSTRT and
  BEAMCON0 as "new" registers of the Enhanced Chip Set.

### 6. Contrasting cases

- **Main check, VARVBEN:** BEAMCON0 = $0020 (PAL) / $0000 (NTSC). p.305 makes
  VBSTRT depend on VARVBEN, so line N shows data in C2: line N-1 is followed by a
  line containing C2, and line N is preceded by a line containing C2; item 1 fails.
  P5-a, P5-b, P5-c still hold.
- **Main check, value:** VBSTRT = N + 16 with the Copper markers unchanged. Blanking
  then starts at line N + 16, line N shows C2, and item 1 fails.
- **P5-d:** select OCS (S3/S4). With the registers absent (p.298, "new"), line N
  shows C2 and item 1 fails, while P5-a, P5-b and P5-c hold.
- **P5-a:** DMACON = $8280 (no BPLEN); no line contains C1.
- **P5-b:** replace WAIT(N-1,$30) by WAIT(N-2,$30); the reads around t_c2 show
  V = N-2.
- **P5-c:** BEAMCON0 with bit 5 opposite to the configured standard; the field
  length is the other standard's.

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
- **VBSTRT's bit layout** is not given on any page (p.293, p.298, p.304).
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

### Lines and gating

Every proposition prints a fixture line per run, one line per precondition, one
component line per system configuration (per display configuration and system
configuration for P1) and one aggregate line. The aggregate carries the claim ids; no
component line does. It prints FAIL as soon as one graded component failed, PASS when
every expected component was graded and passed (P1: 16, P2: 4, P3: 4, P4: 4, P5: 4),
and is otherwise not graded. A line whose precondition failed prints
`RECORDED ... NOT GRADED`, never PASS or FAIL. The preconditions each line waits for:

| line | waits for |
|---|---|
| `P1-MAIN` | `P1-a`, `P1-b`, `P1-c`, `P1-d`, `P1-e` |
| `P2-I1`, `P2-I3` | `CAL-VIDEO`, `P2-a` |
| `P2-I2` | `CAL-VIDEO`, `P2-a`, `P2-b` |
| `P3-MAIN` | the fixture, `P3-a`, `P3-L`, `P3-c`, `P3-d`, and the V-range and field-length parts of `P3-b` |
| `P4-MAIN` | the fixture, `P4-a`, `P4-L`, `P4-c`, `P4-B-PROGRAM`, `P4-B-COPPER` |
| `P5-MAIN` | `CAL-VIDEO`, `P5-a`, `P5-b`, `P5-c`, `P5-d` |

`P2-I1` is `P2`'s item 1 (at least three complete blocks, every one of exactly H beam
lines), `P2-I2` its item 2 (every pixel of every data line), `P2-I3` its item 3 (the two
border samples). `P2-I1` and `P2-I3` do not wait for `P2-b`: they grade the number of
lines and the border, not which memory words were displayed.

The counting part of `P3-b` (the reads of L0 increase strictly) is the L0 control of
`P3-MAIN`; it is graded in `P3-b` and `P3-MAIN` does not wait for it. The V-range and
field-length parts of `P3-b` are what `P3-MAIN` needs from the anchor, and
`P3-b` prints their results beside the counting part (`P3-b S<n> parts`).

### Units and tolerances

- `CAL-CC` measures the period of the 7 MHz clock enable at the port `clk7_en` (4
  clk_sys cycles) and takes one colour clock as two of its periods (p.24: "One
  horizontal count takes one cycle of the system clock (processor is twice this)").
  Every time in colour clocks that the package uses (the windows of P3 and P4, the
  line of 227 cc, the 30 cc spread) is converted with this value.
- `CAL-VIDEO` checks the units of the scan doubler against the pages in each P2 and P5
  run: between consecutive vsync assertions the output holds 2 hs periods per line of
  the page's line count (312 or 313 lines PAL, 262 or 263 NTSC, p.24); each hs period
  is half a beam line of 227 or 228 cc (p.23); a low-resolution pixel is half a colour
  clock (p.239: "2 low resolution pixels"), so the doubler line length divided by the
  2*227 pixels of a line gives k, the samples per low-resolution pixel (k = 2).
- The field-length check of P3 and P4 adds to its tolerance of one line (227 cc) the
  period of the detecting loop. `P3-L` and `P4-L` measure that period from the
  instruction fetches of the `MOVE.W INTREQR(A0),D0` loop (a CPU read of a custom
  register does not reach the port, the loop's own fetches do) and require at least
  100 iterations in each run; the periods they measure are 136 clk_sys cycles on fx68k
  and 120 on TG68K. A
  detection of vsync (P1-e, P2-a, P5-c) has no loop and uses the tolerance of one line.
  The package holds no tuned margin.

### P3-b and P4-MAIN: the counting lines

On the repository's chipset `P3-b` and `P4-MAIN` print FAIL in S1 to S4 and
`P3-b S<n> parts` / `P4-MAIN S<n>` print why. In every window the reads lie in lines 1
to 19 and the detections recur at the field length; the reads that do not increase are
pairs in which V8..V0 is the one of the read before and H8..H1 is lower: the first read
in a line that falls on H = 0 to 2 still shows the previous line's V, and the next read
shows V+1. Per window 0 to 6 such pairs on fx68k and 1 to 2 on TG68K, and no pair of any other kind. V8..V0 and
H8..H1 are the two bytes of one 16-bit register (VHPOSR, p.238), so these pairs are not
a long-word read torn between two bus cycles. A private chipset whose vertical position counts at the clock
edge at which the horizontal position wraps prints PASS for `P3-b` and `P4-MAIN` in S1
to S4 on both cores. The pages state that VHPOSR is the beam counter and that the V and H
fields are those of p.238; no page states at which colour clock after the wrap of H the
vertical count changes. The two lines stay graded as specified; a chipset that counts V
late by two colour clocks fails them, and the page sentences above do not say whether
that is a defect.

### Contrasts that differ from the text of section 6

- **P1 main.** The contrast keeps the change of bit 9 and adds a change the pages call
  visible: run "1"'s Copper list also sets COLOR00 (A) or COLOR01 (B, C, E) to the
  value one blue step away half way down the picture (after WAIT line $80). The upper
  half still shows the original colours, so `P1-d` holds, the BPLCON0 words are
  unchanged, so `P1-b` holds, and `P1-MAIN` is graded and prints FAIL. Replacing the
  change of bit 9 would have made `P1-b` fail and left `P1-MAIN` not graded.
- **P4 main.** The contrast that inserts a BPLCON0 write is the contrast of
  `P4-B-PROGRAM`. A contrast of `P4-MAIN` has to leave every precondition true, so it is
  an implementation record: a second private chipset in which LPEN is 1 after reset.
- **P2 item 3.** Section 6 has no contrast for the border samples; the contrast widens
  the display window by 32 pixels and fetches one more word per line, so the sample
  after the 320th pixel shows data.
- **P5-d.** The harness configuration the run was started with is the ECS configuration
  (the harness's `chipset_config` bit 3); the contrast starts the run with the OCS one.

### Controls on a chipset that follows the pages

On the repository's chipset the lines `P3-MAIN`, `P3-b`, `P4-MAIN` and `P5-MAIN` are
red, so a stimulus change cannot make them redder. Each has a control that runs its
comparator on a live input from a different mechanism: a private copy of the chipset
sources, outside the repository, with three changes. (1) BPLCON0 bit 3 enables a latch
of the beam position at the end of the field, the latch holds until the start of line
20, and BEAMCON0 bit 13 makes a read ignore it (pp.239, 305). (2) The vertical position
counts at the clock edge at which the horizontal position wraps. (3) With BEAMCON0 bit
12 set, VBSTRT starts and VBSTOP ends vertical blanking (pp.293, 304, 305). A run of
P3, P4 and P5 on that copy prints 84 passed, 0 failed on both cores: every line of the
four propositions passes, and the stimulus contrasts `P3MAIN` (the L0 program stands in
for L1), `P3B` and `P5MAIN` (no VARVBEN) print FAIL through the unchanged comparators.
A second private copy, in which the latch is enabled after reset, runs P4 and prints
23 passed, 5 failed on both cores: every fixture and precondition passes, and `P4-MAIN`
in S1 to S4 and the P4 aggregate print FAIL.

### Measured, per core

| Line | Page(s) | fx68k | TG68K |
|---|---|---|---|
| CAL-CC | p24 | PASS (1/1) | PASS (1/1) |
| CAL-VIDEO (P2 and P5 runs) | p23, p24, p239 | PASS (8/8) | PASS (8/8) |
| P3 fixture | (spec) | PASS (4/4) | PASS (4/4) |
| P3-a (>=100 reads/window, spread) | (spec) | PASS (4/4) | PASS (4/4) |
| P3-L (polling loop measured) | (spec) | PASS (4/4) | PASS (4/4) |
| P3-b (L0 strictly increasing, V in 1-19, field length) | p216, p238, p239 | FAIL (4/4) | FAIL (4/4) |
| P3-c (LPENDIS=0, bit 5, field length), S1, S2 | p304, p305 | PASS (2/2) | PASS (2/2) |
| P3-d (BPLCON0 written once, no Copper write) | (spec) | PASS (4/4) | PASS (4/4) |
| P3-MAIN (LPEN=1: every counted read in a window is the same 17-bit value) | p271, p89, p229, p239 | FAIL (4/4) | FAIL (4/4) |
| P3 aggregate | p271, p89, p229, p239 | FAIL (1/1) | FAIL (1/1) |
| P4 fixture | (spec) | PASS (4/4) | PASS (4/4) |
| P4-a (>=100 reads/window, spread) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-L (polling loop measured) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-c (field length), S1, S2 | (spec) | PASS (2/2) | PASS (2/2) |
| P4-B-PROGRAM (no write to $DFF100) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-B-COPPER (no Copper MOVE to $100) | (spec) | PASS (4/4) | PASS (4/4) |
| P4-MAIN (power-up: strictly increasing, V in 1-19) | p271, p216 | FAIL (4/4) | FAIL (4/4) |
| P4 aggregate | p271 | FAIL (1/1) | FAIL (1/1) |
| P1 fixture, configs A, B, C, E x S1-S4 | p59-p62 | PASS (16/16) | PASS (16/16) |
| P1-a (sync samples identical) | p63 | PASS (16/16) | PASS (16/16) |
| P1-b (BPLCON0 words fetched as tabled) | p63, p64 | PASS (16/16) | PASS (16/16) |
| P1-c (bitplane reads per DMA field) | p61, p62 | PASS (16/16) | PASS (16/16) |
| P1-d (picture not uniform) | p46, p55 | PASS (16/16) | PASS (16/16) |
| P1-e (field length) | p24 | PASS (16/16) | PASS (16/16) |
| P1-MAIN (bit 9 has no effect on RGB) | p63, p64 | PASS (16/16) | PASS (16/16) |
| P1 aggregate | p63, p64 | PASS (1/1) | PASS (1/1) |
| P2 fixture, config B x S1-S4 | p61, p62 | PASS (4/4) | PASS (4/4) |
| P2-a (P1-b and P1-e) | p63 | PASS (4/4) | PASS (4/4) |
| P2-b (buffer read once per word, in order) | p61, p62 | PASS (4/4) | PASS (4/4) |
| P2-I1 (three complete blocks of H lines) | p49, p59, p63 | PASS (4/4) | PASS (4/4) |
| P2-I2 (every pixel of every data line) | p46, p55, p61, p272 | PASS (4/4) | PASS (4/4) |
| P2-I3 (border samples show C0) | p46, p55 | PASS (4/4) | PASS (4/4) |
| P2 aggregate | p63 | PASS (1/1) | PASS (1/1) |
| P5 fixture, V1 V2 x S1 S2 | (spec) | PASS (4/4) | PASS (4/4) |
| P5-a (data in lines N-10 to N-2) | p79 | PASS (4/4) | PASS (4/4) |
| P5-b (anchor: V = N-1 / N+1) | p22 | PASS (4/4) | PASS (4/4) |
| P5-c (field length) | p24 | PASS (4/4) | PASS (4/4) |
| P5-d (harness configured ECS) | p298 | PASS (4/4) | PASS (4/4) |
| P5-MAIN (VBSTRT sets the blanking line) | p293, p304, p305, p79 | FAIL (4/4) | FAIL (4/4) |
| P5 aggregate | p293, p304, p305, p79 | FAIL (1/1) | FAIL (1/1) |

fx68k: 207 passed, 19 failed. TG68K: 207 passed, 19 failed.

### Mutations, per core

Every mutation is run from `obj_dir_fx68k`/`obj_dir_tg68k` directly (fx68k needs its own
microcode there), with `NMH_MUTATE` set; none runs during a plain `make test`. Each is
verified to print FAIL through the clause it targets, on both cores. Selectors marked
`private` run on the private chipset of the previous section.

| `NMH_MUTATE` | Targets | fx68k | TG68K |
|---|---|---|---|
| `CALCC` | CAL-CC | FAIL | FAIL |
| `CALVID` | CAL-VIDEO | FAIL | FAIL |
| `P1FIX` | P1 | FAIL | FAIL |
| `P1A` | P1-a | FAIL | FAIL |
| `P1B` | P1-b | FAIL | FAIL |
| `P1C` | P1-c | FAIL | FAIL |
| `P1D` | P1-d | FAIL | FAIL |
| `P1E` | P1-e | FAIL | FAIL |
| `P1MAIN` | P1-MAIN, opus:3-a1000.c04 | FAIL | FAIL |
| `P2FIX` | P2 | FAIL | FAIL |
| `P2A` | P2-a | FAIL | FAIL |
| `P2B` | P2-b | FAIL | FAIL |
| `P2ITEM1` | P2-I1 | FAIL | FAIL |
| `P2FLIP` | P2-I2, opus:3-a1000.c05 | FAIL | FAIL |
| `P2EDGE` | P2-I3 | FAIL | FAIL |
| `P3FIX` | P3 | FAIL | FAIL |
| `P3A` | P3-a | FAIL | FAIL |
| `P3L` | P3-L | FAIL | FAIL |
| `P3B` | P3-b (private) | FAIL | FAIL |
| `P3C_LPENDIS` | P3-c | FAIL | FAIL |
| `P3C_WRONGSTD` | P3-c | FAIL | FAIL |
| `P3D` | P3-d | FAIL | FAIL |
| `P3MAIN` | P3-MAIN, opus:A-bpc.b03.c0 (private) | FAIL | FAIL |
| `P4FIX` | P4 | FAIL | FAIL |
| `P4A` | P4-a | FAIL | FAIL |
| `P4L` | P4-L | FAIL | FAIL |
| `P4C` | P4-c | FAIL | FAIL |
| `P4B_PROG` | P4-B-PROGRAM | FAIL | FAIL |
| `P4B_COP` | P4-B-COPPER | FAIL | FAIL |
| `P5FIX` | P5 | FAIL | FAIL |
| `P5A` | P5-a | FAIL | FAIL |
| `P5B` | P5-b | FAIL | FAIL |
| `P5C` | P5-c | FAIL | FAIL |
| `P5D` | P5-d | FAIL | FAIL |
| `P5MAIN` | P5-MAIN, opus:B-vbstrt.addr (private) | FAIL | FAIL |
| private chipset, plain run (`NMH_ONLY=P3P4P5`) | every line of P3, P4, P5 | 84 passed, 0 failed | 84 passed, 0 failed |
| second private chipset, LPEN 1 after reset (plain run, `NMH_ONLY=P4`) | P4-MAIN, opus:A-leg.lpen2 | FAIL, every precondition holds | FAIL, every precondition holds |
