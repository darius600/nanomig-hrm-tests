# Specification: sprite registers, ECS sprite positioning, and the register-summary codes

This document was written from the package's claims (`claims-spec-stage.jsonl`)
and from the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an
author who never saw the implementation or any test. It states, for every
proposition the claims support, what the test program does, where the test
looks, what it expects and why, what must be true before a result counts, and
which change of the stimulus must turn the check red. The builder implements it
as written.

Page numbers are printed page numbers. Quotations are from the page files;
where a table, figure or disputed line was checked against the printed scan,
this is said. "The port" means the Chip-memory port the test observes. It shows
word addresses. This document gives byte addresses; the port's word address is
the byte address divided by two.

---

## 1. Claims not turned into a proposition

| claim | reason |
|---|---|
| `CH4-fig47.f10` (p.106) | A statement about the corpus, not about the hardware: the page file's ASCII copy of Figure 4-7 repeats "line 1" where the print says "last line". The printed figure was checked and reads "low word of data, last line" / "high word of data, last line". A test cannot observe a transcription. The data-structure layout the tests build follows the printed figure. |
| `CH4-data.c2` (p.129) | "normally used only by the display processor" describes customary use. "Normally" cannot be checked. The behaviour next to it ("They are the holding registers for the data obtained by DMA cycles", p.129) is covered by P6. |
| `APPC2-copcon.c5` (p.310) | The pages disagree (section 2.1). For ECS with CDANG clear, p.310 and p.21 let the Copper write $040–$07E, but p.26, the "~" legend on p.287 and the marks on pp.289–290 do not. No expected value. |
| `APPC2-copcon.c6` (p.310) | This claim compares the reach with CDANG set on the standard chip set against CDANG clear on ECS. The ECS-clear half has no expected value (row above), so the comparison has none either. |
| `APPB2-legend.star`, `CH4-APPB-LEGEND.c006` (p.287) | The "\*" rows are $000–$03E. For $010–$03E, p.21 disagrees with p.287 and p.26 (section 2.1). For ECS with CDANG set, p.310 disagrees too. Every page agrees only on $000–$00E, and only for OCS or for ECS with CDANG clear. Those rows are all read-only or early-read dummy addresses (p.289), so a Copper write to them has no effect anyone could look for. No proposition is left. |
| `APPC2-copcon.conf2` (p.310) | The claim records a disagreement between appendix B and appendix C and says it is "not resolved". Under this document's rule a disagreement yields no expected value (section 2.1). In addition, SPRHDAT's function is never described (row below). |
| `APPC2-BEAMCON0.uhres3` (p.298) | SPRHDAT is "Ext. logic UHRES sprite pointer and data id" (p.290). No page describes its bits or any effect a program, the port, the video or the audio could show. The only reading "ext. logic" allows is a signal to logic outside the chip set, which a test cannot see. The pages also disagree on whether SPRHDAT is an ECS register (section 2.4). |
| `CH4-APPB-LEGEND.c016` (p.287) | "An early-read DMA transfer can originate at the disk" needs data read from a disk surface. The test harness cannot serve a floppy disk yet (no disk image), so this cannot be checked yet. P11 keeps the stimulus for later, marked as deferred. |

Claims used only in part:

| claim | part used | part not used, and why |
|---|---|---|
| `APPC2-copcon.c3` | P12: with CDANG set, the standard chip set's Copper writes $040–$07E | The lower end $03E is disputed (section 2.2). |
| `APPC2-copcon.c4` | P12: with CDANG set, the ECS Copper writes $040–$07E | "all of the Amiga chip registers" below $040 is disputed (section 2.1). |
| `APPB2-legend.tilde`, `CH4-APPB-LEGEND.c007` | P12: with COPCON set true, "~" rows are writable by the Copper | "not writable ... unless" (the ban when the bit is clear) is disputed (section 2.1). |
| `APPC-pos.c3`, `CH4-appC-position.c3` | P1: displacements come in 70 ns steps | The phase of those steps against the SuperHires pixel grid ("on 70ns boundaries") cannot be seen, because the video output drops every second SuperHires pixel. |
| `APPC-pos.c13`, `CH4-appC-position.c21` | P1 (bit 4 acts on ECS), P3 (bits 3 and 4 do nothing on OCS) | Bit 3 on ECS "(unimplemented)" (p.303) has no effect a test could use to show that it exists. |
| `APPB2-legend.er1`, `APPB2-legend.er2`, `CH4-APPB-LEGEND.c014`, `CH4-APPB-LEGEND.c015` | P11: the blitter's destination transfer is a DMA write into Chip memory | The disk half needs a disk image, which the harness does not provide yet (row `c016` above). |

---

## 2. Where the pages disagree or leave something open

### 2.1 Which registers the Copper may write

- p.21 (checked in print): "The Copper can store data into the following
  registers: Any register whose address is $20 or above. Any register whose
  address is between $10 and $20 if the Copper danger bit is a 1. ... The Copper
  cannot write into any register whose address is lower than $10."
- p.26 (checked in print): "The registers that the Copper can always affect are
  numbered $80 through $FF inclusive. ... Those it cannot affect at all are
  numbered $00 to $3E inclusive. ... from $40 to $7E, are protected by a bit in
  the Copper control register." And: "When CDANG is 0, these registers cannot be
  written by the Copper. When CDANG is 1, these registers can be written by the
  Copper."
- p.274: "This is a 1-bit register that when set true, allows the Copper to
  access the blitter hardware. This bit is cleared by power-on reset, so that the
  Copper cannot access the blitter hardware. See Appendix C for ECS operation."
- p.287 (checked in print): "\* Address not writable by the Copper." "~ Address
  not writable by the Copper unless the "copper danger bit", COPCON is set
  true." On pp.289–290 (checked in print), $000–$03E carry "\*" and $040–$07E
  carry "~".
- p.310 (checked in print): "In the standard chip set, if this is set, the
  Copper can access the address range from $DFF03E through $DFF07E. Now, in the
  ECS, if this bit is set, the Copper can access all of the Amiga chip registers.
  If this bit is clear, the Copper can access the address range from $DFF03E
  through $DFF07E, the same range as when the danger bit is set in the standard
  chip set."

What every page agrees on:

- (a) With the danger bit set, the Copper can write $040–$07E, on both chip sets.
- (b) The Copper can write the registers used as ordinary Copper targets in this
  package: $080–$13E and $180. Support: p.21 "$20 or above"; no mark in appendix
  B; the Copper list of the chapter-4 example writes $0E0–$13E (p.112); p.26
  names $80–$FF. No page forbids them. p.26's range stops at $FF, so for
  $100–$1FE it says nothing either way.

Nothing else is agreed. p.21 lets the Copper write $020 and up at all times,
the blitter range included. That contradicts p.26, p.274 and appendix B for the
danger bit clear on either chip set. Appendix C's ECS statements contradict
appendix B's unqualified marks. Appendix B's own note on p.288 refers the reader
to appendix C for registers marked "(E)", and COPCON is marked "A(E)" (p.289).
One reading therefore takes appendix C as the ECS rule. The marks on p.287 carry
no such qualification, so this document does not pick that reading. Whether it
should is a question for the people who own the oracle.

### 2.2 The address $03E

Appendix B marks STRLONG "& \*03E" (p.289), and p.26 includes $3E in "cannot
affect at all". Appendix C starts the danger-bit range at $DFF03E (p.310), and
p.21 gives "$20 or above". No test writes $03E.

### 2.3 Width of the sprite pointer

"SPRxPTH contains the high three bits of the memory address" (p.110); "the 18-bit
sprite pointer register" (p.126); "the 18-bit address of sprite x" (p.285,
checked in print). Against this, p.127 says "This pair of registers contains the
32-bit word address of Sprite 0 DMA data". The ECS list of pointers that gained
two bits (p.309) names DSKPTH, BLTxPTH, COP1LCH, COP2LCH and AUDxLCH, but not
SPRxPTH. Every address in this document is below $80000. Every page's pointer
width reaches that far.

### 2.4 SPRHDAT, UHRES and BEAMCON0

Appendix B marks SPRHDAT "~078 W A(E)" (p.290, checked in print). Appendix C's
register map "shows the changes and new registers in the Amiga's Enhanced Chip
Set" (p.298, checked in print) and has no SPRHDAT row. BEAMCON0 is described as
"(SHRES,PAL)" on p.266 and p.293 and as "(SHRES,UHRES,PAL)" on p.298. The bit
table on p.305 names neither SHRES nor UHRES: bit 6 is "DUAL Special ultra
resolution mode enable". P14 uses only bit 5, PAL.

### 2.5 SPRxCTL bit 3

- p.107: "Bits 6-3 Unused (make zero)".
- p.128: "Bits 6-3 are reserved for future use (make zero)".
- p.284 (checked in print): "06-04 X Not used". Bit 3 is not listed at all.
- p.303: "03 SHSH0 Start horizontal (SHR mode) 35ns (unimplemented)" and "bits 3
  and 4 are in the ECS chips only".

P3 rests on p.303.

### 2.6 A strobe triggered by a read

p.287 says "S Strobe (write address with no register bits). Writing the register
causes the effect.", and p.26 says "When you write to a Copper strobe address".
On p.30 (checked in print), the prose says "write to its strobe address", but
the code does `MOVE.W COPJMP1(a1),d0 ; Causes copper to load PC from COP1LC`,
which is a read. Every test here strobes with a write, which all these pages
support. No test grades what a read of a strobe address does.

### 2.7 When the Copper restarts

p.25 (checked in print): "At the start of each vertical blanking interval,
COP1LC is automatically used to start the program counter. That is, no matter
what the Copper is doing, when the end of vertical blanking occurs, the Copper
is automatically forced to restart". Start and end are both named. The tests
therefore never rely on the restart time. They anchor on the observed Copper
fetch of the list's first word (R5).

### 2.8 The "(E)" mark on the SPRxCTL rows

Appendix B marks SPR0CTL "A D(E)" but SPR1CTL–SPR7CTL "A D" (pp.291–292,
checked in print). Appendix C lists "142 SPRxCTL chg" for all x (p.298). This
package uses sprites 0, 2 and 4 for position. The positioning proposition is
graded on sprite 2, whose row carries no "(E)", as p.298 and p.303 ("SPRxCTL")
say it applies to every sprite.

### 2.9 Hires with bit 4 set

"Attempting to use 70ns sprite positioning with Hires mode under the current
system may lead to unpredictable results." (p.303). No test sets SPRxCTL bit 4
in Hires mode.

---

## 3. Common setup, conventions and reference frames

### 3.1 Configurations

The default is ECS, PAL, 2 MB Chip memory. A proposition that needs something
else says so. The programs are plain 68000 code, and every run uses both CPU
cores with 68000 behaviour, their default. No proposition here sets 68010 or
68020 behaviour.

### 3.2 Memory map (byte addresses, all even, all below $80000)

| address | content |
|---|---|
| $20000 | Copper list L1 |
| $20800 | Copper list L2 (P10) |
| $21000 | NULL: the null sprite, two words $0000,$0000 (p.111, p.114) |
| $22000 | MARK: marker words, one word per event |
| $22100 | RESULT: values the program read, one word each |
| $22200 | DECOY: one word $A5A5 |
| $25000 | S1: sprite-0 structure (P6–P9) |
| $27000 | S2: sprite-0 structure differing from S1 (P7, P9) |
| $28000 | S4: reference sprite-4 structure (P6–P9) |
| $29000 | SWP: sprite-2 structure (P1–P3) |
| $2A000 | SREF: reference sprite-0 structure (P1–P3) |
| $30000–$303FF | blitter buffers (P11–P13) |
| $30400–$30409 | disk buffer (P11, deferred half only) |
| $40000–$4FFFF | RULER: bitplane buffer, every word $FF00 |
| $50000–$5FFFF | ZERO: bitplane buffer, every word $0000 |

The program's code, data and stack must lie outside $20000–$5FFFF.

### 3.3 Markers, results, quiet windows

- The program marks each event by writing a distinct word to the next free word
  of MARK. The write appears at the port with its time, and that time is the
  event's time.
- Values the program reads go to RESULT and are checked after the run.
- A "quiet window" is an interval in which the program executes no instruction
  that writes Chip memory. It may fetch instructions, read custom registers and
  write custom registers. The program's structure guarantees this; the builder
  documents it in the test source.

### 3.4 Starting the Copper (p.30)

"you must initialize one of the Copper location registers (COP1LC or COP2LC) and
write to its strobe address before Copper DMA is turned on" (p.30). Every run
that uses the Copper does the following, in order:

1. `MOVE.L #list,COP1LCH` ("you write the 18-bit address by moving one long word
   to the register whose name ends in "H."", p.25).
2. A write to COPJMP1 (`MOVE.W D0,COPJMP1`; see 2.6).
3. DMACON with SET/CLR (bit 15), DMAEN (bit 9), COPEN (bit 7) and the other
   enables the run needs (p.278).

A Copper list ends with `$FFFF,$FFFE` ("WAIT for VP = $FF and HP = $FE", p.30).
A WAIT for line V is `(V<<8)|$01, $FFFE`. A MOVE is `register offset, data`
(p.21).

### 3.5 Standard display (the chapter-4 example, pp.111–112)

These values follow the example listing on p.111. That listing is an example,
not a statement about the hardware:

| register | value |
|---|---|
| BPLCON0 | $1200 (1 bitplane, COLOR) |
| BPLCON1 | $0000 |
| BPLCON2 | $0024 ("Sprites have priority over playfields") |
| BPL1MOD | $0000 |
| DDFSTRT | $0038 |
| DDFSTOP | $00D0 |
| DIWSTRT | $2C81 |
| DIWSTOP | $F4C1 |

The Copper writes BPL1PTH/BPL1PTL at the head of every field (p.288: bitplane
pointers are reloaded "vertical blank for bitplane and sprite pointers"). Unless
a proposition says otherwise, the pointer is set to ZERO.

### 3.6 Colour sets

**Plain set** (P4–P9). COLOR00 = COLOR01 = $000, COLOR17 = $F00 (red),
COLOR18 = $0F0 (green), COLOR19 = $00F (blue), COLOR25–COLOR27 = $FFF (white).
All other colour registers are $000. With every playfield colour black, the
bitplane content never shows.

**SuperHires-safe set** (P1–P3). In SuperHires the colour registers are read in
an encoded form: "SuperHires bitplane colors are specially encoded in the sixteen
lower color registers" (p.301), and "SuperHires sprites are encoded in the upper
sixteen color registers using a similar scheme" (p.302). In those tables (checked
in print), register n with Y = n & 3 and X = n >> 2 holds the 2-bit R, G, B of
colour Y in bits 11–10, 7–6, 3–2 and of colour X in bits 9–8, 5–4, 1–0. The same
set is also used in the Lores runs of P2 and P3, where the registers are read as
ordinary 12-bit colours (Table 4-6, p.130). The values below therefore make every
nonzero playfield colour red only and every nonzero sprite colour green only in
both readings:

| registers | values |
|---|---|
| COLOR00–COLOR15 | $000 $C00 $C00 $C00 $300 $F00 $F00 $F00 $300 $F00 $F00 $F00 $300 $F00 $F00 $F00 |
| COLOR16–COLOR31 | $000 $0C0 $0C0 $0C0 $030 $0F0 $0F0 $0F0 $030 $0F0 $0F0 $0F0 $030 $0F0 $0F0 $0F0 |

With this set, an output pixel is a sprite pixel when G ≠ 0 and a nonzero
playfield pixel when R ≠ 0.

### 3.7 Sprite pointers per field

The pointers are set as the manual prescribes: "Set the sprite pointers ... Turn
on sprite direct memory access ... For each subsequent display field, during the
vertical blanking interval, rewrite the sprite pointers." (p.109). This is
"Normally ... done by the Copper" (p.110), and "all eight sprite pointers must be
initialized to either a real sprite or a safe null sprite" (p.114). The Copper
list writes SPRxPTH/SPRxPTL for all eight sprites before any WAIT. Unused
sprites point at NULL. A structure is POS, CTL, then one (low word, high word)
pair per line, then the next usage or $0000,$0000 (Table 4-1, p.105; Figure 4-7
as printed, p.106; p.116). Usages of one channel are separated by at least one
line: "the bottom line of one usage of a sprite must be separated from the top
line of the next usage by at least one horizontal scan line" (p.117).

### 3.8 Waiting on the beam from the CPU

- **Vertical blank.** The program polls INTREQR ($DFF01E) bit 5, VERTB "Start of
  vertical blank" (p.280), and clears it by writing $0020 to INTREQ ($DFF09C).
- **Beam line.** The program reads VHPOSR ($DFF006) bits 15–8 (V7–V0, p.285) and
  VPOSR ($DFF004) bit 0 (V8).

### 3.9 Reference frames

- **R1, column.** On each output line, a column is the number of pixel clocks
  since HSYNC became active on that line. Positions are compared only on the
  same line, as the distance between two edges. Anchor check: each line of the
  graded interval has exactly one HSYNC activation.
- **R2, pixel size.** An output pixel is 70 ns. It equals two SuperHires pixels
  ("SuperHires mode provides a 35ns pixel display rate", p.300; the video output
  shows every second pixel of the fastest resolution). It is half a lores pixel
  ("The shift occurs once during each low resolution pixel time", p.124; SH0 is
  a "140 ns increment", p.303). Anchor check on the ruler lines (R3):
  - SuperHires: the period of the R ≠ 0 pattern is 8 output pixels.
  - Lores: the period is 32 output pixels.
  A run in which the check fails grades nothing that uses R2.
- **R3, ruler lines.** In P1–P3 the Copper sets BPL1PT to RULER at the head of
  the field, waits for line $30 and sets BPL1PT to ZERO. The ruler lines are the
  first three output lines on which R ≠ 0 pixels appear. RULER's words are
  $FF00, so the ruler pattern has a period of 16 playfield pixels.
- **R4, edge.** A sprite edge is the first column of a maximal run of sprite
  pixels (G ≠ 0 with the SuperHires-safe set, or a sprite colour with the plain
  set). On one line, e_A is the first such edge and e_B the first edge after e_A
  that follows at least one non-sprite pixel.
- **R5, field.** A field is the interval from one VSYNC activation to the next.
  For Copper-timed checks, a field is the interval from one Copper fetch of L1's
  first word to the next (see 2.7). Line counts within a field are HSYNC
  activations counted from the field's VSYNC activation. Only differences of
  line counts are graded.
- **R6, chip identification (p.299).** VPOSR bits 14–8 are "the chip
  identification": $00 PAL, $10 NTSC for the original Agnus, and "20 for PAL, 30
  for NTSC" for "the enhanced HighRes Agnus". Where a proposition needs to know
  the chip set, the program reads VPOSR into RESULT, and the test asserts the
  identification that matches the selected configuration.

---

## 4. Propositions

### P1 — In SuperHires on ECS, SPRxCTL bit 4 moves the sprite start by 70 ns, two SuperHires pixels, and every 70 ns position is reachable

**1. Proposition.** On ECS with SuperHires active, setting SPRxCTL bit 4 (SHSH1)
starts the sprite one 70 ns step, two SuperHires pixels, later than with the bit
clear. With SH0 (140 ns) and SH8–SH1, it reaches every 70 ns position in
between. Claims: `APPC-pos.c2`, `APPC-pos.c3` (step only),
`CH4-appC-position.c18`, `CH4-appC-position.c3` (step only),
`CH4-appC-position.c4`, and the ECS half of `APPC-pos.c13` and
`CH4-appC-position.c21`. Page 303.

**2. Stimulus** (run SHR-SWEEP, ECS PAL).

1. Colours: the SuperHires-safe set (3.6).
2. Display: the standard display (3.5), except BPLCON0 = $1240 (bit 6 SHRES
   "SuperHires 35ns pixel enable bit"; bit 15 HIRES "Set it to zero if SHRES
   enabled", p.300).
3. SREF (sprite 0, reference). POS $4050, CTL $9000 (VSTART $40, VSTOP $90,
   H = $0A0). Then 80 pairs (low $FFFF, high $0000), then $0000,$0000.
4. SWP (sprite 2). Ten usages, k = 0..9, each followed by 4 pairs (low $FFFF,
   high $0000), then $0000,$0000. For usage k:
   - VSTART = $40 + 8k and VSTOP = VSTART + 4.
   - P_k = 400 + k, the start in 70 ns units.
   - H_k = P_k >> 1 and SHSH1_k = P_k & 1.
   - POS = (VSTART << 8) | (H_k >> 1).
   - CTL = (VSTOP << 8) | (SHSH1_k << 4) | (H_k & 1).

   | k | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
   |---|---|---|---|---|---|---|---|---|---|---|
   | POS | $4064 | $4864 | $5064 | $5864 | $6065 | $6865 | $7065 | $7865 | $8066 | $8866 |
   | CTL | $4400 | $4C10 | $5401 | $5C11 | $6400 | $6C10 | $7401 | $7C11 | $8400 | $8C10 |

   Bit layout: SPRxPOS "07-00 SH8-SH1"; SPRxCTL "04 SHSH1", "00 SH0" (p.303);
   bits 15–8 VSTART and VSTOP (p.107).
5. Copper list L1:
   - BPL1PT = RULER ($0004,$0000).
   - SPR0PT = $2A000, SPR2PT = $29000, all other sprite pointers = NULL.
   - WAIT line $30.
   - BPL1PT = ZERO ($0005,$0000).
   - End.
6. Start the Copper as in 3.4, with DMACON = $83A0 (SET, DMAEN, BPLEN, COPEN,
   SPREN, as on p.112).
7. Wait for 3 VERTB, write M1, wait for 3 VERTB, write M2. The program writes
   no custom register after step 6.

**3. Observation.** Video output of every field that begins after M1 and ends
before M2. On each line where the SWP usages appear, measure the edges e_A
(sprite 0) and e_B (sprite 2) as in R4, and compute d = e_B − e_A in output
pixels. Usage k is the k-th group of consecutive lines, counted from the top,
that shows an e_B.

**4. Expected.** On every line of group k, d = 80 + k, for all k = 0..9. So each
step of P changes d by exactly +1 output pixel, which is 70 ns and two
SuperHires pixels.

- "04 SHSH1 Start horizontal (SHR mode) 70ns increment" (p.303).
- "00 SH0 Start horiz. value Low bit 140 ns increment" (p.303).
- "This allows for positioning the sprite every other SuperHires pixel on 70ns
  boundaries" (p.303).

Quantifier "every other SuperHires pixel": the sweep covers ten consecutive
positions. It crosses changes of SH0, SH1 and SH2, and grades every one of them.
It does not establish positions outside 400–409.

"On 70ns boundaries" is graded only as "steps of 70 ns". Where a step lies
against the 35 ns grid is invisible (R2).

**5. Preconditions.**

1. R2's anchor holds with period 8 on the ruler lines. This shows SuperHires is
   active in this field. BPLCON0 is written once, before DMA starts, so the mode
   holds for the whole field.
2. Every line of the ten groups shows both an e_A and an e_B.
3. Each group has exactly 4 lines. "The value (VSTOP - VSTART) defines how many
   scan lines high the sprite will be" (p.107).
4. d is the same on all lines of one group.

**6. Contrasting cases.**

- Proposition: SWP with SHSH1 cleared in every usage (CTL of odd k minus $0010).
  The page predicts d_{2j+1} = d_{2j}, and the "+1 per step" check goes red.
- Precondition 1: BPLCON0 = $1200. The ruler period becomes 32, so the check
  fails. This is run SHR-LORES, P2.
- Precondition 2: DMACON without SPREN ($8380). No e_A or e_B appears.
- Precondition 3: usage 0 with VSTOP = VSTART + 5 (CTL $4500). Group 0 has 5
  lines.
- Precondition 4: none is needed. Every stimulus change the pages speak to moves
  all lines of a group together.

**7. Reference frame.** R1 (same-line distance, so no line-start anchor enters
d), R2, R3, R4. The groups are identified by order (R5).

**Open.** Sprite pixel width in SuperHires is not stated for this purpose and is
not graded. Only edges are.

---

### P2 — On ECS, 70 ns sprite positions are available only in SuperHires: in Lores, bit 4 produces no 70 ns offset

**1. Proposition.** On ECS in Lores mode, a sprite with SPRxCTL bit 4 set starts
at a whole multiple of 140 ns from the same sprite with the bit clear, never at
an odd multiple of 70 ns. Claims: `APPC-pos.c14`, `CH4-appC-position.c22`.
Page 303.

**2. Stimulus.** Run SHR-LORES: identical to SHR-SWEEP (P1) except BPLCON0 =
$1200. Hires is never run with bit 4 set (see 2.9).

**3. Observation.** As in P1: d on every line of the ten groups, in the fields
between M1 and M2.

**4. Expected.** d is even on every line of every group, and in particular for
odd k, where SHSH1 = 1.

- "Warning: 70ns sprite positions are only available in SuperHires mode."
  (p.303).

Quantifier "only": the Lores run grades all five SHSH1 = 1 usages, at five
different SH8–SH0 values. Hires is excluded by the page. The page does not say
which even value an odd-k usage takes, whether that of SHSH1 = 0 or another, so
no value is expected beyond parity.

**5. Preconditions.**

1. R2's anchor holds with period 32. This shows Lores is active.
2. For even k, d = 80 + k. This shows the measurement resolves 140 ns steps in
   Lores.
3. The paired run SHR-SWEEP, under the same configuration, passes P1. This shows
   that bit 4 is live in this configuration, so that a parity pass cannot come
   from a bit that is ignored everywhere.
4. P1's preconditions 2–4 hold here too.

**6. Contrasting cases.**

- Proposition: the same program with BPLCON0 = $1240, which is P1's run. The
  page predicts d_1 = 81, which is odd, so the parity check goes red.
- Precondition 1: BPLCON0 = $1240 (period 8).
- Precondition 2: usage 2 with CTL $5400 (SH0 cleared). d_2 = 80, not 82.
- Precondition 3: SHR-SWEEP with SHSH1 cleared everywhere. P1's step check
  fails.
- Precondition 4: as in P1.

**7. Reference frame.** As in P1.

---

### P3 — SPRxCTL bits 3 and 4 are not in the original chips: on OCS, setting them changes nothing

**1. Proposition.** On OCS, setting SPRxCTL bit 3, bit 4 or both leaves the
sprite's position and image exactly as with both bits clear. Claims:
`APPC-pos.c13`, `CH4-appC-position.c21` (OCS half). Page 303.

**2. Stimulus.** Run BITS-OCS, OCS PAL. As SHR-LORES (P2), except that SWP has
five usages k = 0..4 (VSTART = $40 + 8k, VSTOP = VSTART + 4, 4 pairs of $FFFF,
$0000 each):

| k | CTL bits | POS | CTL |
|---|---|---|---|
| 0 | none | $4064 | $4400 |
| 1 | 4 | $4864 | $4C10 |
| 2 | 3 | $5064 | $5408 |
| 3 | 3 and 4 | $5864 | $5C18 |
| 4 | SH0 (H = 201) | $6064 | $6401 |

The program reads VPOSR into RESULT+0 before DMA starts.

**3. Observation.** d on every line of groups 0–4, and the length of the sprite-2
run on each of those lines, in the fields between M1 and M2.

**4. Expected.** d = 80 on every line of groups 0, 1, 2 and 3. The sprite-2 run
length is the same in groups 0–3. The expected value rests on "Note: bits 3 and 4
are in the ECS chips only." (p.303): a bit that the original chips do not have
cannot act. p.107 ("Unused (make zero)"), p.128 ("reserved for future use") and
p.284 ("Not used") give the bits no function on the original chips (see 2.5).

**5. Preconditions.**

1. VPOSR bits 14–8 = $00, which identifies the original PAL Agnus (R6).
2. R2's anchor holds with period 32.
3. Group 4 has d = 82. This shows a 140 ns change is visible in this run.
4. Each group has exactly 4 lines.

**6. Contrasting cases.**

- Proposition: usage 1 with CTL $4C01 (SH0 instead of bit 4). The page predicts
  d_1 = 82 ("SH0 ... 140 ns increment", p.303), so the equality check goes red.
- Precondition 1: the ECS configuration. VPOSR gives $20.
- Precondition 2: BPLCON0 = $1240 on ECS (period 8).
- Precondition 3: usage 4 with CTL $6400. d_4 = 80.
- Precondition 4: as in P1.

**7. Reference frame.** R1–R4, R6.

**Open.** Bit 3 on ECS is "(unimplemented)" (p.303). No test grades it.

---

### P4 — On ECS, SPRxPOS and SPRxCTL are write-only: a 68000 write takes effect, and a 68000 read does not return what was written

**1. Proposition.** On ECS, a 68000 write to SPR0POS or SPR0CTL loads the
register. A 68000 read of the same address does not deliver the register's
content. Claims: `CH4-appC-position.c10`, `CH4-appC-position.c15`. Page 303.

**2. Stimulus.** Run MANUAL, ECS PAL. It is shared with P5; see P5 for the
writes. After the display part of field A has passed (the program waits until
V8 = 0 and V7–V0 ≥ $F8, after DIWSTOP's line $F4), the program does the
following:

1. Read DECOY.
2. Read SPR0POS ($DFF140) and store the value at RESULT+4.
3. Read DECOY.
4. Read SPR0CTL ($DFF142) and store the value at RESULT+6.
5. Write marker RA.

It does the same after field B, storing at RESULT+8 and RESULT+10, then writes
marker RB. Values written before the reads:

| field | SPR0POS | SPR0CTL |
|---|---|---|
| A | X1 = $0060 | C1 = $0001 |
| B | X2 = $0061 | C2 = $0000 |

**3. Observation.** RESULT+4..+10 after the run. The port during the reads.

**4. Expected.**

- NOT (RESULT+4 = $0060 and RESULT+8 = $0061).
- NOT (RESULT+6 = $0001 and RESULT+10 = $0000).

That is, the values read do not follow the values written. Sources: "SPRxPOS W
... SPRxCTL W" (p.303), and "W=write-only; R=read-only" (p.287). No page states
what a read of a write-only address returns. p.299 says of a missing register,
"whatever value is left over on the bus from the last cycle will be there". So no
value is expected, only the absence of tracking. The decoy reads keep a
left-over bus value from matching the written value in both fields by chance.

**5. Preconditions.**

1. P5's field-A and field-B display checks pass, with edges and patterns up to
   the last display-window line. So at the time of each read, the register held
   the value written.
2. At the port, a CPU read of DECOY appears before each register read and after
   the preceding one.
3. The reads fall after the last display-window line of their field and before
   the next vertical blank.

**6. Contrasting cases.**

- Proposition: read back, instead of the register, a Chip-memory word into which
  the program wrote X1 (field A) and X2 (field B) just before the read. Memory
  returns what was written, so the "does not follow" check goes red.
- Precondition 1: omit the field-B write of SPR0POS. Field B then has d = 64
  (POS $0060 with CTL $0000) instead of 68, so P5's field-B check fails.
- Precondition 2: omit the DECOY reads.
- Precondition 3: read at V7–V0 = $80 instead. The read falls inside the display
  window.

**7. Reference frame.** Field A and field B as delimited by markers MA/RA and
MB/RB (P5). Line numbers from VHPOSR/VPOSR (3.8).

**Open.** What a read returns. Whether a read has any side effect. Neither is
graded.

---

### P5 — The 68000 can load each of SPRxPOS, SPRxCTL, SPRxDATA and SPRxDATB (manual mode)

**1. Proposition.** Each of the four load decodes in Figure 4-13 accepts a load
from the 68000. SPRxDATA and SPRxDATB, although normally loaded by DMA, lie in the
processor's address space, and a processor write to them is displayed. Claims:
`CH4-fig4-13.c9`, `CH4-fig413.f6` (68000 half), `CH4-data.c1`. Pages 125
(figure, checked in print), 129 and 123.

**2. Stimulus.** Run MANUAL, ECS PAL.

1. Plain colour set. Standard display. Copper list: BPL1PT = ZERO, then end.
2. Start the Copper with DMACON = $8380 (SET, DMAEN, BPLEN, COPEN). SPREN stays
   clear: "Sprites can be activated manually even when the sprite DMA is turned
   off." (p.123).
3. The program reads DMACONR into RESULT+2.
4. After the first VERTB:
   - Reference, sprite 4: SPR4POS = $0050, SPR4CTL = $0000, SPR4DATB = $0000,
     SPR4DATA = $FFFF.
   - Field A, sprite 0: SPR0POS = $0060, SPR0CTL = $0001, SPR0DATB = $F0F0,
     SPR0DATA = $FF00.
   - Write marker MA.

   The order is the one p.123 prescribes: "You display sprites manually by
   writing to the sprite data registers SPRxDATB and SPRxDATA, in that order. You
   write to SPRxDATA last because that address "arms" the sprite".
5. At the end of field A, the reads of P4 and marker RA.
6. After the next VERTB, field B: SPR0POS = $0061, SPR0CTL = $0000,
   SPR0DATB = $FF00, SPR0DATA = $F0F0, then marker MB.
7. The reads of P4 and marker RB.

**3. Observation.** Video output on every line of the display window (the lines
from DIWSTRT's $2C to DIWSTOP's $F4) in field A (between MA and RA) and field B
(between MB and RB). On each line:

- e_A is the sprite-4 edge (white).
- e_B is the sprite-0 edge (the first red, green or blue pixel after sprite 4's
  run).
- d = e_B − e_A.
- The colours of the 24 output pixels from e_B.

**4. Expected.** On every display-window line:

| field | d | output pixels 0–7 | 8–15 | 16–23 |
|---|---|---|---|---|
| A | 66 | COLOR19 ($00F) | COLOR17 ($F00) | COLOR18 ($0F0) |
| B | 68 | COLOR19 | COLOR18 | COLOR17 |

Derivation:

- d = 2 × (H − $A0), with H = (POS bits 7–0) << 1 | CTL bit 0 ("Bits 7-0 specify
  the horizontal start position, bits H8 - H1" and "Bit 0 is bit H0 of horizontal
  start", p.128). A lores pixel is two output pixels (R2).
- Each sprite pixel's colour number takes the SPRxDATB bit as its left digit and
  the SPRxDATA bit as its right digit. p.107 says "The bits in the high order
  color descriptor word contribute the leftmost digit"; p.126 says "The first
  word is stored in SPRxDATA and the second word in SPRxDATB"; the first word of
  a pair is the low word (Table 4-1, p.105). Colour numbers 1–3 select
  COLOR17–19 (Table 4-2, p.108).
- "The data written will then be displayed on every line, at the horizontal
  position given in the "H" portion of the position registers SPRxPOS and
  SPRxCTL." (p.123).

Each register's field-B write changes the outcome in its own way:

- A lost POS write gives d = 64.
- A lost CTL write gives d = 70.
- A lost DATB write gives colours 19, black, 19.
- A lost DATA write leaves the sprite disarmed, since the CTL write disarms it
  (p.123), and nothing is shown.

So a pass shows all four loads from the 68000. Quantifier "every line": graded
on every display-window line of both fields.

**5. Preconditions.**

1. RESULT+2 (DMACONR) has bit 5 = 0 and bit 9 = 1, so no sprite DMA can have
   loaded these registers.
2. The reference (sprite 4) edge is present, and at the same column, on every
   display-window line of both fields.
3. Markers MA and MB precede the display window of their field.

**6. Contrasting cases.**

- Proposition: field B with SPR0CTL = $0001. The page predicts d = 70 in field B,
  and the check goes red.
- Precondition 1: DMACON = $83A0 (SPREN set). Bit 5 = 1.
- Precondition 2: omit the SPR4DATA write. Sprite 4 is not armed, and no
  reference appears.
- Precondition 3: delay the field-B writes until V7–V0 = $80. MB then falls
  inside the display window.

**7. Reference frame.** R1, R4. The display-window lines are counted from the
first line on which the sprite-4 run appears. Fields are delimited by markers.

---

### P6 — Sprite DMA loads each of SPRxPOS, SPRxCTL, SPRxDATA and SPRxDATB

**1. Proposition.** Each of the four load decodes in Figure 4-13 accepts a load
from DMA. A second usage of one sprite channel, fetched by DMA, replaces all four
registers, and the display follows the new contents. Claims: `CH4-fig4-13.c9`,
`CH4-fig413.f6` (DMA half). Pages 125 (figure), 126 and 108.

**2. Stimulus.** Run AUTO, ECS PAL.

1. Plain colour set. Standard display.
2. S1 at $25000:
   - Usage 1: POS $5060, CTL $5401 (VSTART $50, VSTOP $54, H $C1), then 4 pairs
     (low $FF00, high $F0F0).
   - Usage 2: POS $5861, CTL $5E00 (VSTART $58, VSTOP $5E, H $C2), then 6 pairs
     (low $F0F0, high $FF00).
   - $0000,$0000.

   That is 26 words, $25000–$25032.
3. S4 at $28000: POS $4850, CTL $6800 (VSTART $48, VSTOP $68, H $A0), then 32
   pairs (low $FFFF, high $0000), then $0000,$0000.
4. Copper list L1:
   - BPL1PT = ZERO.
   - SPR0PT = $25000.
   - SPR4PT = $28000.
   - All other sprite pointers = NULL.
   - End.
5. Start the Copper with DMACON = $83A0. Read DMACONR into RESULT+2.
6. Wait for 2 VERTB, write M1, wait for 3 VERTB, write M2.

The program never writes SPR0POS, SPR0CTL, SPR0DATA or SPR0DATB.

**3. Observation.** Video output of every field between M1 and M2. On each line
where sprite 4 appears, measure e_A (sprite 4), e_B (sprite 0), d, and the colours
of the 24 pixels from e_B. Also measure the line counts (R5) of the first line of
sprite 4, of each sprite-0 group, and the group heights.

**4. Expected.**

- Sprite 0 appears as two groups of consecutive lines.
- Group 1 has 4 lines, and its first line is 8 lines after sprite 4's first
  line.
- Group 2 has 6 lines, and its first line is 8 lines after group 1's first line.
- On group-1 lines, d = 66 and the colours are 19, 17, 18 (as field A in P5).
- On group-2 lines, d = 68 and the colours are 19, 18, 17 (as field B).

Sources:

- "When the vertical position of the beam counter is equal to the VSTOP value in
  the sprite control words, the next two words fetched from the sprite data
  structure are written into the sprite control registers ... These two words are
  interpreted by the hardware in the same manner as the original words" (p.108).
- "the pointers write these words into SPRxPOS and SPRxCTL" (p.126).
- "Two words are fetched per line of sprite height, and these words are written
  into the sprite data registers. The first word is stored in SPRxDATA and the
  second word in SPRxDATB." (p.126).
- "The value (VSTOP - VSTART) defines how many scan lines high" (p.107).

The derivation of d and colours is as in P5. Each of the four words differs
between the usages, so a pass shows all four loads by DMA.

**5. Preconditions.**

1. RESULT+2 has bits 9 and 5 set.
2. In each graded field, the Copper's fetch of the data word of its SPR0PTL MOVE
   precedes the first port read in $25000–$25032. "During the vertical blanking
   interval before the first display of the sprite, you need to write the
   sprite's memory address into these pointers." (p.110).
3. Sprite 4 appears on exactly 32 consecutive lines, at one column.

**6. Contrasting cases.**

- Proposition: usage 2 with CTL $5E01. The page predicts d = 70 on group-2
  lines, and the check goes red.
- Precondition 1: DMACON = $8380. SPREN is clear, and no sprite appears.
- Precondition 2: move the SPR0PT MOVEs behind a WAIT for line $60. The first S1
  read then precedes them, or S1 is not read in that field.
- Precondition 3: SPR4PT = NULL. No reference appears.

**7. Reference frame.** R1, R4, R5 (line counts relative to sprite 4's first
line).

---

### P7 — The sprite DMA channel takes the whole sprite data structure from Chip memory, at the pointer, in order

**1. Proposition.** The custom chips read a sprite's data structure from Chip
memory. Every word of S1 appears as a read at the Chip-memory port, at
consecutive word addresses starting from the pointer, in the order of Table 4-1.
Claims: `CH4-build.c11`, `CH4-p104.c6`, `CH4-mem.c5`, `CH4-p105.c12`. Pages 104
and 105.

**2. Stimulus.** Run AUTO (P6).

**3. Observation.** The port, in each field between M1 and M2. The interval runs
from the Copper's fetch of the data word of the SPR0PTL MOVE to the same fetch in
the next field. Record the reads in $25000–$25FFF.

**4. Expected.** In each interval, the first 26 reads in $25000–$25FFF are at
$25000, $25002, …, $25032, in ascending order, each once. Sources:

- "Sprite data, like all other data accessed by the custom chips, must be loaded
  into Chip RAM." (p.104).
- "Because this data structure must be accessible by the special-purpose chips,
  you must ensure that this data is located within chip memory." (p.105).
- Table 4-1 (p.105): N control word 1, N+1 control word 2, then the low and high
  words per line.
- "These pointers are dynamic; they are incremented by the sprite DMA channel to
  point first to the control words, then to the data words, and finally to the
  end-of-data words." (p.110).

Quantifier: all 26 words of the structure are graded. "all other data accessed by
the custom chips" goes beyond this package and is not graded. Reads after the
end-of-data words are not graded, because the pages do not say whether the
channel fetches after them.

**5. Preconditions.** P6's preconditions 1 and 2.

**6. Contrasting cases.**

- Proposition: the Copper list sets SPR0PT = $27000 (S2). The page predicts the
  reads at $27000–$27032 and none in $25000–$25FFF, so the check goes red.
- Preconditions: as in P6.

**7. Reference frame.** The interval anchored on the Copper fetch named in 3.
Addresses at the port.

**Open.** What happens when the pointer names an address outside the selected
Chip-memory size. The pages give no expected value, and no test tries it.

---

### P8 — The least significant bit of SPRxPTL is ignored: sprite data is fetched from the word address

**1. Proposition.** Sprite data structures are word aligned. An odd value written
to SPRxPTL fetches the structure from the even address below it, exactly as the
even value would. Claims: `CH4-build.c12`, `CH4-p104.c7`. Pages 104 and 110.

**2. Stimulus.** Run AUTO-ODD: run AUTO with the Copper's SPR0PTL MOVE carrying
$5001 instead of $5000.

**3. Observation.** As in P7 and P6.

**4. Expected.** The same 26 reads as in P7, starting at $25000, and the same
display as in P6. Sources:

- "Be sure all of your sprite data structures are word aligned in Chip Memory."
  (p.104).
- "The least significant bit of SPRxPTL is ignored, as sprite data must be word
  aligned. Thus, only fifteen bits of SPRxPTL are used." (p.110, checked in
  print).
- "+ Address register pair. Must be an even address pointing to chip memory."
  (p.287).

**5. Preconditions.**

1. P6's preconditions.
2. The Copper's fetch of the SPR0PTL MOVE's data word returns $5001 at the port.
   This shows the odd value was the one written.

**6. Contrasting cases.**

- Proposition: the MOVE carries $5002. The page predicts the first read at
  $25002, and the check goes red.
- Precondition 2: the MOVE carries $5000. The data word read is $5000.
- Other preconditions: as in P6.

**7. Reference frame.** As in P7.

---

### P9 — A 68000 long-word write to SPRxPTH loads SPRxPTH and SPRxPTL

**1. Proposition.** One 68000 long-word write to SPR0PTH sets the whole pointer:
the high word goes to SPR0PTH and the low word to SPR0PTL. Claims:
`CH4-p110.c7`, `CH4-ptr.c10`. Page 110, with p.25.

**2. Stimulus.** Run CPU-PTR, ECS PAL.

1. As AUTO, except:
   - The Copper list has no SPR0PT MOVEs.
   - S2 at $27000 is S1 with usage 1's POS = $5070 (H = $E1).
   - Before DMA starts, the program writes SPR0PT = NULL once, so no
     uninitialized pointer runs (p.114).
2. In each of 4 fields, after VERTB (3.8):
   - `MOVE.W #$7000,SPR0PTL`
   - `MOVE.L #$00025000,SPR0PTH`
   - marker P_f.

**3. Observation.** The port and the video output of fields 2–4. For each field,
the interval runs from P_f to the next VERTB.

**4. Expected.** In each graded field:

- The reads of sprite 0 are at $25000–$25032, as in P7.
- There is no read in $27000–$27FFF.
- Group 1 has d = 66.

Sources:

- "As usual, you can write a long word into SPRxPTH." (p.110).
- "you write the 18-bit address by moving one long word to the register whose
  name ends in "H." This is because when you write long words with the 680x0,
  the most significant word goes in the lower addressed word." (p.25).

**5. Preconditions.**

1. P_f precedes the first sprite-0 read of the field at the port.
2. DMACONR bits 9 and 5 set.
3. Sprite 4 appears as in P6.

**6. Contrasting cases.**

- Proposition: replace the long-word write with `MOVE.W #$0002,SPR0PTH`. The
  page predicts that SPR0PTL keeps $7000, so the reads fall at $27000–$27032 and
  group 1 has d = 2 × ($E1 − $A0) = 130. The check goes red.
- Precondition 1: do the writes at V7–V0 = $60 instead of at VERTB. P_f then
  follows the first sprite read.
- Preconditions 2 and 3: as in P6.

**7. Reference frame.** The interval from P_f. R1, R4.

---

### P10 — A strobe register takes effect by being written, whatever data is written

**1. Proposition.** Writing a strobe address causes its effect, and the data
written plays no part. A 68000 write to COPJMP2 with any data restarts the Copper
at COP2LC. Claims: `APPB2-legend.s1`, `APPB2-legend.s2`, `CH4-APPB-LEGEND.c022`,
`CH4-APPB-LEGEND.c023`, `CH4-APPB-LEGEND.c024`. Page 287, with pp.25–26.

**2. Stimulus.** Run JUMP, ECS PAL.

1. Copper lists:
   - L1 at $20000: WAIT line $F0 ($F001,$FFFE); MOVE COLOR00,$0000; end.
   - L2 at $20800: MOVE COLOR00,$0F00; end.
2. The program writes `MOVE.L #$20000,COP1LCH` and `MOVE.L #$20800,COP2LCH`,
   then starts the Copper as in 3.4 with DMACON = $8280 (SET, DMAEN, COPEN).
3. For fields f = 1..6: wait for VERTB, clear it, wait until V7–V0 = $80, write
   marker M_f.
   - In fields 2, 4 and 6 only, the program then writes COPJMP2 ($DFF08A):
     `MOVE.W #D,COPJMP2`, with D = $0000 in field 2, $FFFF in field 4 and $5A5A
     in field 6.
   - Fields 1, 3 and 5 write nothing further. They are the in-run contrast.

   COPJMP2 is marked "S" on p.290.

**3. Observation.** The port. Field f runs from the Copper's read of L1's first
word ($20000) that precedes M_f to its next read (R5). Record reads in
$20000–$2000B and $20800–$20807.

**4. Expected.**

- In fields 2, 4 and 6, the first read in either list after M_f is at $20800.
- In fields 1, 3 and 5, the first read in either list after M_f is at $20004 (L1's
  MOVE, after the WAIT completes), and there is no read of $20800–$20807 in the
  field.
- In every field, there is no read of L2 before M_f.

Sources:

- "S Strobe (write address with no register bits). Writing the register causes
  the effect." (p.287).
- "COPJMP2 Restart Copper from address contained in COP2LC." (p.26).
- "When a jump address strobe is written, the corresponding location register is
  loaded into the Copper program counter. This causes the Copper to jump to a new
  location, from which its next instruction will be fetched." (p.25).

"No register bits" is graded as sameness across three data values, including
all zeros and all ones. The time from the write to the fetch is not stated, and
it is not graded.

**5. Preconditions.**

1. In every field, the Copper reads $20000 and $20002 once, at the field's head.
2. Every M_f appears at the port.
3. M_f falls before the Copper's read of $20004 in its field, which is when the
   WAIT for line $F0 completes.

**6. Contrasting cases.**

- Proposition: in field 4, write COLOR01 ($DFF182, not a strobe) instead of
  COPJMP2. The page predicts no restart, so field 4 shows no L2 read, and the
  check goes red.
- Precondition 1: DMACON without COPEN ($8200). There are no reads of L1.
- Precondition 2: omit the marker write in field 2.
- Precondition 3: wait until V7–V0 = $F4 instead of $80. M_f then falls after the
  WAIT for $F0 has completed.

**7. Reference frame.** R5 (field by L1 fetch; see 2.7). Marker times.

---

### P11 — An "early read" from the blitter is a DMA transfer into Chip memory

**1. Proposition.** The blitter's destination output reaches Chip memory by DMA.
Each result word appears at the port as a write, not as a read, at the address
the destination pointer names, with no CPU write involved. Claims:
`APPB2-legend.er1`, `APPB2-legend.er2`, `CH4-APPB-LEGEND.c014` and
`CH4-APPB-LEGEND.c015` (blitter half each), `CH4-APPB-LEGEND.c017`. Page 287,
with pp.269 and 289. The disk half cannot be checked yet; see "Deferred" at the
end of this proposition.

**2. Stimulus** (run BLIT, ECS PAL).

1. Buffers:
   - SRCA at $30000 = $1234, $5678, $9ABC, $DEF0.
   - DST1 at $30200–$30207 = $EEEE.
   - A guard word at $30208 = $EEEE.
2. DMACON = $8240 (SET, DMAEN, BLTEN). Read DMACONR into RESULT+2.
3. Blitter registers: BLTCON0 = $09F0 (USEA, USED, minterm $F0 "D = A",
   p.177), BLTCON1 = $0000, BLTAFWM = BLTALWM = $FFFF,
   `MOVE.L #$30000,BLTAPTH`, `MOVE.L #$30200,BLTDPTH`, BLTAMOD = BLTDMOD = 0.
4. Write marker M1.
5. BLTSIZE = $0044 (height 1, width 4). "Writing to this register will start the
   blitter, and should be done last" (p.269).
6. Wait for 2 VERTB, then write M2.

M1–M2 is a quiet window.

**3. Observation.** The port between M1 and M2: reads and writes in
$30000–$303FF.

**4. Expected.** Exactly four writes in $30200–$30209, at $30200, $30202, $30204
and $30206 in this order, with data $1234, $5678, $9ABC and $DEF0. No write at
$30208. Sources:

- "ER Early read. This is a DMA data transfer to RAM, from either the disk or the
  blitter." (p.287).
- BLTDDAT "Blitter destination early read (dummy address)", marked ER (p.289).
- BLTDDAT "holds the data resulting from each word of blitter operation until it
  is sent to a RAM destination ... The transfer is automatic during blitter
  operation." (p.269).

Quantifier "either the disk or the blitter": only the blitter disjunct is graded
now. The page does not say that no other channel writes RAM, and this is not
graded.

**5. Preconditions.**

1. Four reads at $30000–$30006 in M1–M2, which shows the blit ran.
2. RESULT+2 has bits 9 and 6 set.
3. M1–M2 is a quiet window (3.3).

**6. Contrasting cases.**

- Proposition: BLTCON0 = $08F0 (USED cleared). The page predicts no writes in
  $30200–$30209 while the A reads still occur ("USED Mode control bit to use
  destination D", p.267). The check goes red.
- Precondition 1: DMACON without BLTEN ($8200). There are no A reads.
- Precondition 2: the same change. Bit 6 = 0.
- Precondition 3: add a CPU write to $30200 inside the window. The quiet-window
  property fails, and the check flags the window as not quiet.

**7. Reference frame.** The interval M1–M2. Address ranges at the port.

**Deferred: the disk half** (`CH4-APPB-LEGEND.c016`, and the disk half of
`APPB2-legend.er1`, `APPB2-legend.er2`, `CH4-APPB-LEGEND.c014` and
`CH4-APPB-LEGEND.c015`). This needs data read from a disk surface, and the
harness serves no disk image yet. It is not built now. The recorded stimulus
follows p.247's sequence (run DISK, ECS PAL):

1. DBUF at $30400–$30407 = $EEEE, and a guard word at $30408.
2. ADKCON = $0400 (clear WORDSYNC, p.265).
3. `MOVE.L #$30400,DSKPTH` ("written as a single long word", p.246).
4. DMACON = $8210.
5. DSKLEN = $4000.
6. Write marker M1.
7. DSKLEN = $8004, and DSKLEN = $8004 again ("Write this value again into the
   DSKLEN register. This actually starts the DMA.", p.247).
8. Poll INTREQR bit 1 (DSKBLK) until it is set, or 4 VERTB have passed.
9. DSKLEN = $4000.
10. Write marker M2.

Expected: 4 or 3 writes at $30400 upward, in ascending order ("one less word may
be read than you asked for", p.247). No write at $30408. No read of DBUF.

Precondition: DSKBLK was set.

Contrast: DSKLEN = $C004 twice ("WRITE Disk write (RAM -> disk if 1)", p.246).
Reads replace the writes.

---

### P12 — With the danger bit set, the Copper can write the blitter registers ($040–$07E), on both chip sets

**1. Proposition.** With COPCON bit 1 (CDANG) set, Copper MOVEs to the blitter
registers in $040–$07E take effect. A blit set up and started entirely by the
Copper runs exactly as the Copper's values specify, on OCS and on ECS. Claims:
`APPC2-copcon.c2`, `APPC2-copcon.c3` (range $040–$07E), `APPC2-copcon.c4`
($040–$07E only), `APPB2-legend.tilde` and `CH4-APPB-LEGEND.c007` (the "set"
half). Pages 310, 287, 289–290, 26, 274 and 21. All of them agree on this part
(2.1).

**2. Stimulus.** Run COPBLT-v with v = $0002, on OCS PAL and on ECS PAL.

1. Buffers:
   - SRCA $30000 = $1234, $5678, $9ABC, $DEF0.
   - SRCB $30010 = $F0F0, $0FF0, $FF00, $00FF.
   - SRCC $30020 = $FF0F, $F0FF, $0FFF, $FFF0.
   - DST1 $30200–$30207 and DST2 $30210–$30217 = $EEEE.
   - Decoy sources $30100–$301FF = $3333.
   - Decoy destinations $30300–$303FF = $EEEE.
2. Decoys, written by the CPU. The CPU may always write these registers.
   - BLTCON0 = $0100, BLTCON1 = $0002.
   - BLTAFWM = BLTALWM = $0000.
   - BLTAPT = $30100, BLTBPT = $30140, BLTCPT = $30180, BLTDPT = $30380 (long
     words).
   - BLTAMOD = BLTBMOD = BLTCMOD = BLTDMOD = $0002.
   - BLTADAT = BLTBDAT = BLTCDAT = $0000.

   The CPU does not write BLTSIZE.
3. COPCON ($DFF02E) = v.
4. Copper list L1:

   ```
   BLTCON0 $0980   BLTCON1 $0000   BLTAFWM $FFFF   BLTALWM $FFFF
   BLTAPTH $0003   BLTAPTL $0000   BLTDPTH $0003   BLTDPTL $0200
   BLTAMOD $0000   BLTDMOD $0000   BLTBDAT $FFFF   BLTCDAT $FFFF
   BLTSIZE $0082
   WAIT line $50
   BLTCON0 $0780   BLTADAT $FFFF
   BLTBPTH $0003   BLTBPTL $0010   BLTCPTH $0003   BLTCPTL $0020
   BLTBMOD $0000   BLTCMOD $0000   BLTDPTH $0003   BLTDPTL $0210
   BLTSIZE $0082
   end
   ```

   The two blits are:
   - Blit 1: D = A·B·C, minterm $80 (p.174: "ABC = 1, bit 7"), with B and C
     constant $FFFF, over 2 rows of 2 words.
   - Blit 2: D = A·B·C with A constant $FFFF and B, C by DMA.

   The offsets are those of pp.289–290: $040, $042, $044, $046, $048/$04A,
   $04C/$04E, $050/$052, $054/$056, $058, $060, $062, $064, $066, $070, $072
   and $074.
5. Start the Copper with DMACON = $82C0 (SET, DMAEN, COPEN, BLTEN). Read DMACONR
   into RESULT+2.
6. Write M1, wait for 3 VERTB, write M2. M1–M2 is a quiet window.

**3. Observation.** The port, in each field (R5) between M1 and M2: accesses in
$30000–$303FF.

**4. Expected.** In every graded field:

- Writes at $30200, $30202, $30204, $30206 with data $1234, $5678, $9ABC,
  $DEF0.
- Writes at $30210, $30212, $30214, $30216 with data $F000, $00F0, $0F00,
  $00F0.
- Reads of SRCA, SRCB and SRCC.
- No access in $30100–$301FF or $30300–$303FF.

Sources:

- "This 1-bit register, the danger bit (CDANG), when set allows the Coprocessor
  to write to the Blitter hardware." (p.310).
- "When CDANG is 1, these registers can be written by the Copper." (p.26).
- "~ Address not writable by the Copper unless the "copper danger bit", COPCON
  is set true." (p.287), read for the case "set true".

Each Copper write that did not take effect would leave its decoy in force and
change the result: zero data, descending addresses, masked words, decoy regions,
or gaps between rows. So a pass shows every listed register was written by the
Copper. Not covered: $05A–$05E (ECS only), $078, $07C (read-only) and $07E, whose
effect needs a synchronized disk read.

**5. Preconditions.**

1. In each graded field, the Copper reads every word of L1 up to and including
   the second BLTSIZE MOVE.
2. RESULT+2 has bits 9, 7 and 6 set.
3. The CPU never writes BLTSIZE in this run (program structure). M1–M2 is quiet.

**6. Contrasting cases.**

- Proposition: the first BLTSIZE MOVE carries $0081 (2 rows × 1 word). The page
  predicts only two writes at DST1, $1234 at $30200 and $5678 at $30202 ("h=height
  ... w=width", p.269). The check goes red.
- Precondition 1: DMACON without COPEN. There are no list reads.
- Precondition 2: DMACON without BLTEN. Bit 6 = 0.
- Precondition 3: add a CPU write BLTSIZE = $0082 before M1. The structural check
  flags it.

**7. Reference frame.** R5 (field by L1 fetch). Address ranges at the port.

**Open.** What happens with CDANG clear (2.1). This run is never used to grade
it.

---

### P13 — COPCON is a 1-bit register: only bit 1 matters

**1. Proposition.** The Copper's access to the blitter registers depends only on
COPCON bit 1. Values that differ only in the other fifteen bits give identical
outcomes. Claim: `APPC2-copcon.c1`. Page 310, with p.26 and p.274.

**2. Stimulus.** Run COPBLT-v (P12) for v = $0002, $FFFF, $0000 and $FFFD, each on
OCS PAL and on ECS PAL.

**3. Observation.** For each run and each graded field, the outcome: the ordered
list of (address, read/write, data) of port accesses in $30000–$303FF, without
times.

**4. Expected.** For each chip set:

- outcome($FFFF) = outcome($0002), which is P12's expected outcome.
- outcome($FFFD) = outcome($0000).

The value of the second pair is not expected, because it is disputed (2.1). Only
the equality is graded. Sources:

- "This 1-bit register, the danger bit (CDANG)" (p.310).
- "In the Copper control register, called COPCON, only bit 1 is currently in use
  by the system." (p.26).
- "This is a 1-bit register ... 01 CDANG" (p.274).

**5. Preconditions.** In every run:

1. P12's preconditions 1 and 2.
2. The CPU writes BLTSIZE only where a contrasting case below says so.
3. M1–M2 is a quiet window.

**6. Contrasting cases.**

- First pair: in the $FFFF run, the first BLTSIZE MOVE carries $0081. The page
  predicts a different blit (P12), so the equality check goes red.
- Second pair: in the $FFFD run, the CPU writes BLTSIZE = $0082 once at V7–V0 =
  $80 of the first graded field. The CPU can always start the blitter ("Writing
  to this register will start the blitter", p.269), so this run gains writes the
  $0000 run lacks, whichever way the disputed case goes. The equality check goes
  red.
- Precondition 1: as in P12.
- Precondition 2: a CPU write of BLTSIZE in the $0000 run. The structural check
  flags it.
- Precondition 3: add a CPU write to $30200 inside the window.

**7. Reference frame.** As in P12. Outcomes are compared field by field in order.

---

### P14 — BEAMCON0 is new with ECS: writing it changes the beam on ECS and nothing on OCS

**1. Proposition.** BEAMCON0 ($1DC) exists in the enhanced Agnus and not in the
standard chip set. Writing its PAL bit turns an NTSC ECS system into PAL line
timing. The same write on OCS changes nothing. Claim: `APPC2-BEAMCON0.uhres1`.
Page 298, with pp.304, 305 and 299.

**2. Stimulus.** Run BEAM, on ECS NTSC and on OCS NTSC.

1. BPLCON0 = $0204 (COLOR, LACE: "LACE =Interlace enable", p.271), so that a
   frame of two fields has the line count p.304 names.
2. DMACON = $7FFF (all DMA off).
3. Read VPOSR into RESULT+0.
4. Wait for 4 VERTB, write M1.
5. Write BEAMCON0 ($DFF1DC) = $0020 (bit 5 "PAL Programmable pal mode enable",
   p.305; all other bits 0).
6. Wait for 8 VERTB, write M2.

**3. Observation.** The video sync outputs. For each VSYNC activation V_i,
N_i = the number of HSYNC activations in [V_i, V_{i+2}), one frame. Graded:

- The N_i with V_{i+2} before M1.
- The four N_i starting at the third VSYNC activation after M1.

**4. Expected.**

- Before M1, N = 525 on both chip sets.
- After, N = 625 on ECS and N = 525 on OCS.

Sources:

- "1DC BEAMCON0 new W A Beam counter control register (SHRES,UHRES,PAL)" (p.298,
  checked in print), in the map that "shows the changes and new registers in the
  Amiga's Enhanced Chip Set" (p.298).
- "A new register in the enhanced Agnus, BEAMCON0, provides a programmable signal
  generator." (p.305).
- "NTSC (525 lines, 227.5 colorclocks per scan line)" and "PAL (625 lines, 227.5
  colorclocks per scan line)" (p.304).
- "Programmable changes between PAL and NTSC modes are new for V2.0." (p.305).

The first two fields after the write are not graded, because the pages do not say
when the change takes effect. p.305 warns "Do not write to this register" and of
damage to multisync monitors. That advice concerns real monitors, and the test
writes only the PAL bit.

**5. Preconditions.**

1. VPOSR bits 14–8 = $30 on the ECS run and $10 on the OCS run: the NTSC Agnus of
   each kind (R6).
2. N = 525 for the graded frames before M1.
3. Each frame contains two VSYNC activations, so the fields alternate as
   interlace requires.

**6. Contrasting cases.**

- Proposition (ECS): write BEAMCON0 = $0000. The page predicts that PAL is not
  enabled and N stays 525, so the "625 after" check goes red.
- Precondition 1: the other chip set, or PAL selected. The identification
  differs.
- Precondition 2: PAL selected. N = 625 before M1.
- Precondition 3: BPLCON0 = $0200 (LACE clear). The frame count is no longer
  fixed by p.304, and the check reports the precondition unmet.

**7. Reference frame.** VSYNC and HSYNC activations on the output (R1, R5).
Markers M1 and M2 for the timing of the write.

**Open.** What the other BEAMCON0 bits do, and what "UHRES" names (2.4).
