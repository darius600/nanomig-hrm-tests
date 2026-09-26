# Specification: Copper beam positions and line geometry

This document specifies one integration test package that checks the chip set
against the *Amiga Hardware Reference Manual*, 3rd edition, chapter 2 pages
23, 24, 27 and 34 and Appendix A page 275. It was written from the package's
claims (`claims-spec-stage.jsonl`) and the manual's printed pages alone, by an
author who never saw the implementation or any test. Page numbers are printed
page numbers. Every expected value below is quoted from a page; where the
pages leave a value open or disagree with each other, this document says so
and specifies no value.

## 1. Claims not turned into a proposition

| Claim | Reason |
|---|---|
| `ch2-horizontal-beam-position.c14`, `COP2-horizontal-beam-position.c16` (p.23: "The display sees all these lines as 227 1/2 color clocks long") | The display's line length reaches the test only as the timing of the video output, and that output passes a line doubler. Nothing the test sees separates the doubler's timing from the chip set's: a doubler that re-times its output would show uniform lines whatever the chip set does, so a green result would not be evidence. No page names another observable that follows the 227 1/2 figure (the beam counter and the Copper see 227 or 228, see P2 and P3); inventing one, for example an alternating offset between Copper colour changes and the display window edge, would need an assumption about where the display window is compared that no page states. The sentence is also unclear for PAL (section 7, item 6). |
| `ch2-vertical-beam-position.c21`, `COP2-vertical-beam-position.c24` (p.24: "processor is twice this") | The processor clock is internal to the CPU. The test sees only the CPU's bus accesses at the Chip-memory port, and no page states how many processor clock cycles a bus access or an instruction takes (p.196 says only that "The 68000 uses only the even-numbered memory access cycles"). No observation converts into processor clock cycles. The pages agree on the rate itself (p.24: 3,579,545 Hz times two; p.2: "7.15909 MHz"), but not on what "system clock" names (section 7, item 9). |

Claims used only in part:

| Claim | Part graded | Part not graded, and why |
|---|---|---|
| `COP2-copins-timing.c6` (p.275: "memory cycles are 280 ns") | P4: one horizontal count lasts one memory cycle (p.285 gives the same 280 ns for one step of the horizontal beam position). | The absolute 280 ns: the test sees time only in clock cycles of a simulation whose clock the test bench supplies. Other pages give 279.365 ns (NTSC) and 281.937 ns (PAL) (p.141) and "approximately 280 ns" (p.194). |
| `COP2-putting-together-a-copper-instruction-list.c9`, `COP2-putting-together-a-copper-instruction-list.c10` (p.27: end of display (226,262) NTSC, (226,312) PAL) | P3: the horizontal count ends at 226 = $E2 on PAL lines (and on NTSC short lines). | The y value: p.24 and p.34 give 262 and 312 lines; counted from line 0 (p.27 itself starts at (0,0)) the last line is 261 or 311, not 262 or 312. The x value for NTSC: p.23 says long lines run to $E3 = 227, and no page says whether an NTSC field ends on a long or a short line. See section 7, item 1. |
| `COP2-horizontal-beam-position.c8`, `ch2-horizontal-beam-position.c7` (p.23: unused portion "$04 to $47") | P9: the unused portion shows only background, and it contains the horizontal blanking with background on both sides of it. | The positions $04 and $47 as counter values on screen: no page gives the delay from a counter value to a displayed pixel, the pages place the standard screen's first pixel at different counter values, and $04 to $47 inclusive plus a 320-pixel screen is one count longer than a 227-count line. See section 7, item 5. |
| `COP2-horizontal-beam-position.c6`, `ch2-horizontal-beam-position.c6` (p.23: blanking "$0F to $35") | P10: one blanking interval per line, no longer than $35 - $0F + 1 = 39 counts. | The position of the interval against the counter, for the reason above. The page says blanking "falls in the range of"; that bounds the interval, it does not say it fills the range. |
| `COP2-vertical-beam-position.c9`, `COP2-vertical-beam-position.c10` (p.24: vertical 0 through 5 cover the last six lines; 256 + 6 = 262) | P8, NTSC only. | PAL: the six lines are NTSC's 262 - 256; the page gives no PAL counterpart. The recipe's first step read literally cannot work (section 7, item 7); the stimulus enters line 256 by a route every page supports. Copper execution on the lines of vertical blanking before the Copper restarts is not graded (section 7, item 8). |
| `COP2-vertical-beam-position.c11` (p.24: "There are alternating long and short lines") | P2, NTSC. | PAL: p.23 says "In PAL, they are all 227 long." |

## 2. Claim map

| Proposition | Claims | Pages |
|---|---|---|
| P1 Lines per field; the beam starts at (0,0) | `COP2-vertical-beam-position.c3`, `COP2-vertical-beam-position.c4`; `COP2-using-the-copper-in-interlaced-mode.c2`, `COP2-using-the-copper-in-interlaced-mode.c4`; `COP2-putting-together-a-copper-instruction-list.c7` | 24, 34, 27 |
| P2 Line length as the Copper sees it | `COP2-horizontal-beam-position.c10`, `COP2-horizontal-beam-position.c11`, `COP2-horizontal-beam-position.c12`, `COP2-horizontal-beam-position.c14`, `COP2-horizontal-beam-position.c15`; `COP2-vertical-beam-position.c11` | 23, 24 |
| P3 Range of the horizontal beam count | `COP2-horizontal-beam-position.c13`, `COP2-horizontal-beam-position.c12`, `COP2-horizontal-beam-position.c15`; x part of `COP2-putting-together-a-copper-instruction-list.c9`, `COP2-putting-together-a-copper-instruction-list.c10` | 23, 27 |
| P4 Copper horizontal positions: one count per memory cycle, 113 positions, nothing beyond $E2 | `COP2-horizontal-beam-position.c3`; `COP2-vertical-beam-position.c23`; `COP2-copins-timing.c6` (as a relation) | 23, 24, 275 |
| P5 One Copper position is 4 low-resolution or 8 high-resolution pixels | `COP2-horizontal-beam-position.c4`, `COP2-horizontal-beam-position.c5`; `ch2-horizontal-beam-position.c4`, `ch2-horizontal-beam-position.c5` | 23 |
| P6 WAIT on line $FF: $FF is reached, $FFE2 is the largest value, $FFFE never | `COP2-the-wait-instruction.c26`, `COP2-the-wait-instruction.c27`, `COP2-the-wait-instruction.c28`, `COP2-the-wait-instruction.c29` | 23 |
| P7 Line 256 appears as zero in the comparison; the counter does not wrap | `COP2-the-wait-instruction.c22`, `COP2-the-wait-instruction.c23`; `COP2-vertical-beam-position.c7` | 23, 24 |
| P8 NTSC: vertical 0 through 5 after line 255 are the last six lines | `COP2-vertical-beam-position.c9`, `COP2-vertical-beam-position.c10` | 24 |
| P9 Standard screen: 320 pixels, only background in the unused portion | `COP2-horizontal-beam-position.c7`, `COP2-horizontal-beam-position.c8`, `COP2-horizontal-beam-position.c9`; `ch2-horizontal-beam-position.c7`, `ch2-horizontal-beam-position.c8` | 23 |
| P10 Horizontal blanking | `COP2-horizontal-beam-position.c6`; `ch2-horizontal-beam-position.c6` | 23 |

## 3. Conventions

### 3.1 Configurations

- Every run: Enhanced Chip Set, 2 MB Chip memory, PAL unless the run says
  NTSC, executed once with each of the two CPU cores. Each expectation must
  hold for both cores.
- Runs A, B and C are also executed with OCS selected. Chapter 2 states these
  points without reference to a chip set, and Appendix C (pp.298-310) lists
  no change to line lengths, the beam counter's range or the Copper's
  comparison.
- PAL or NTSC is selected only through the test system. The program never
  writes BEAMCON0 (p.305: "Do not write to this register").

### 3.2 Time

All times are in memory slots. One slot is one memory cycle, the unit that
Figure 6-9 (p.195) numbers along a line. The test converts the harness's
access times to slot numbers with the harness's fixed number of clock cycles
per memory cycle. "slot(x)" is the slot of access x. Several pages use
"system clock" for different clocks (section 7, item 9), so this document
does not use the term.

### 3.3 Copper instruction notation

The encodings are from p.21, p.22 and Table 2-2 (p.36).

- `WAIT(v,h)`: first word `(v << 8) | h | 1`, second word `$FFFE`. Here v
  is the vertical position VP (for a line numbered 256 or more, VP = line -
  256), and h is an even horizontal count; bits 7-1 of the first word are
  HP8-HP2 (Table 2-2), so the low byte is h with bit 0 set. `$FFFE` sets
  the blitter-finished-disable bit and all compare enable bits (p.24:
  "Bits 14-1 are normally set to all 1s"). Example: `WAIT($FF,$E2)` is
  `$FFE3,$FFFE`, the same form as p.33's `$00E3,$80FE ; Wait for
  Horizontal $E2`.
- `MOVE(r,d)`: first word r (register offset, bit 0 = 0), second word d
  (p.21).
- `MARK(t)`: `MOVE($1FE,t)`, a move to NO-OP(NULL) at $1FE (p.293). The
  Copper may write any register at $80 or above (p.26). The data word t is
  unique within its list, so each marker is identified by its address and
  its data.
- `END`: `$FFFF,$FFFE` (p.23 and p.29, "Wait for line 255, H = 254 (ends
  Copper list)").

### 3.4 Program skeleton

1. Write DMACON = $7FFF and INTENA = $7FFF, which clears every DMA enable
   and interrupt enable (p.222: bit 15 is SET/CLR).
2. Prepare the data and Copper lists in Chip memory (p.20: "the Copper can
   retrieve its instructions only from the chip RAM area").
3. Write the display registers the run names. BPLCON0 bit 0 (ENBPLCN3) is
   always 0, so BPLCON3 is not in use (p.307). DIWHIGH is never written, so
   "the old scheme for DIWSTRT and DIWSTOP described above holds" (p.306).
   BEAMCON0 and COPCON are never written.
4. Write the list's address to COP1LCH as one long-word write (p.25), then
   write COPJMP1 (p.26: "When you write to a Copper strobe address, the
   Copper reloads its program counter"). Only then write DMACON = $8280
   (SET, DMAEN, COPEN), or $8380 when bitplanes are shown (adds BPLEN). This
   is the order p.30 prescribes: "you must initialize one of the Copper
   location registers (COP1LC or COP2LC) and write to its strobe address
   before Copper DMA is turned on." (p.30's example code performs a read of
   COPJMP1; the text and p.26 say write. The program writes.)
5. Runs A, B and C: run the beam trace (RF2) until its buffer is full. Runs
   D and E: wait the number of fields the run names by polling VPOSR.
6. Write the completion word $600D to a fixed Chip-memory address, then loop
   in place.

Sprite, audio, disk and blitter DMA are never enabled.

### 3.5 Copper events

- The **fetch** of a Copper instruction word is the Copper's read of that
  word at the Chip-memory port. After DMA is on, the only reads in a list's
  address range are Copper fetches.
- The **time of a marker** is the slot of the fetch of its first word.
- For a WAIT `W`, **delay(W)** is the slot of the fetch of the first word of
  the instruction after W, minus the slot of the fetch of W's second word.
- **λ_min** is delay(`WAIT($00,$00)`) at the head of a list in the same
  field. That WAIT is satisfied whenever it is fetched, because every
  position is equal to or greater than (0,0) (p.22: the Copper waits "until
  the video beam counters are equal to (or greater than) the coordinates").
- **W waited** means delay(W) > λ_min.

## 4. Reference frames

### RF1: Memory slots

Defined in 3.2. Anchor check: no two logged accesses occupy the same slot.

### RF2: Beam trace

**Stimulus.** After the start sequence, the program repeats one step: read
the long word at $DFF004 and store it to the next long word of a trace
buffer in Chip memory. The high word of that long word is VPOSR and the low
word is VHPOSR (p.216: "the address pairs VPOSR,VHPOSR ... can be read ...
as long words, with the most significant addresses being VPOSR"). The
program stops when the buffer is full. The buffer holds at least the number
of complete fields the run names.

**Reading a sample.** Sample i is the pair of words the CPU writes, as
logged at the Chip-memory port. Its time w_i is the slot of the later of
the two writes.

- V8 is bit 0 of the VPOSR word (p.216, p.285).
- V7-V0 are bits 15-8 of the VHPOSR word, and the horizontal count H is
  bits 7-0 (H8-H1; p.216, p.285: "RESOLUTION = 1/160 of screen width (280
  ns)").
- V = 256·V8 + V7-V0. All other VPOSR bits are ignored: LOF; under ECS the
  chip identification in bits 14-8, LOL in bit 7, and v10 and v9 in bits
  2-1 (p.299).

**Trusted samples.** A sample is trusted when H ≥ 16. Its two words come
from two reads within one loop step, and one step lasts at most 16 slots
(precondition T1). If a line began between the two reads, H is below 16
and V8 may belong to the previous line.

**Preconditions of every run that uses RF2**

- T1: w_{i+1} - w_i ≤ 16 for every i.
- T2: the counter is live. Between consecutive trusted samples with equal V,
  H strictly increases. Between consecutive trusted samples, V is unchanged,
  rises by exactly 1, or becomes 0.

**Line of an event.** For an event at slot t, let s⁻ be the last trusted
sample with w ≤ t and s⁺ the first trusted sample with w ≥ t + 16. If
V(s⁻) = V(s⁺), the event lies on line V. Otherwise its line is not
identified. The reason: the read of s⁺ followed the write of the sample
before it, which by T1 is at or after t; the read of s⁻ was at or before
t.

**Anchor check.** In Run A, every marker `m_v` (v = $30 ... $FF) lies on
line v (precondition A-anchor).

### RF3: Fields and Copper restarts

- A **field boundary** is a place where V falls between consecutive trusted
  samples.
- A **complete field** runs from one boundary to the next and begins after
  the first restart.
- A **restart** is a Copper fetch of the word at the address in COP1LC that
  is not the first Copper fetch of the run. p.25: "At the start of each
  vertical blanking interval, COP1LC is automatically used to start the
  program counter".
- Where in the field the restart falls is not used (section 7, item 8).

Anchor check (precondition R1): between consecutive field boundaries there
is exactly one restart.

### RF4: Video output

- An **output line** is the run of pixels between consecutive horizontal
  sync pulses. A pixel's position is its index from the first pixel after
  the sync pulse. A colour is the 12-bit value of 4-bit red, green and blue.
- Runs of equal colour are taken cyclically within the output line: a run
  that touches both the start and the end of the line counts once, and its
  two parts add.
- **P_lo** is the length, in output pixel clocks, of one low-resolution
  pixel, measured in Run D-lo. **P_hi** is the same for a high-resolution
  pixel, measured in Run D-hi.
- **c_px** is output pixel clocks per horizontal count, measured in Run
  D-sweep (P5).
- A field of output lines lies between vertical sync pulses. Only complete
  fields after the first restart are graded.

Anchor checks are the calibration preconditions in P5.

## 5. Runs

Each run follows 3.4. The Copper list of a run is the same in every field
unless the run says otherwise.

### Run A: beam trace and line markers (PAL; NTSC; OCS)

BPLCON0 = $0200 (no bitplanes, COLOR). DMACON = $8280. The trace covers at
least 6 complete fields. List A:

```
WAIT($00,$00)  MARK($A000)                       ; λ_min
for v = $30 .. $FF:
  WAIT(v,$40)  MARK(v)                           ; m_v
WAIT($FF,$E2)  MARK($A0E2)                       ; m_E2
MARK($A0F0) MARK($A0F1) ... MARK($A0F7)          ; 8 moves: padding
for n = 0 .. N:          (N = 6 for NTSC, N = $37 for PAL)
  WAIT(n,$60)  MARK($0100+n)                     ; m_(256+n)
END
```

The 8 padding moves put the next WAIT's fetch well inside line 256, where
RF2 can identify it.

Variants:

- **A-LACE**: PAL, BPLCON0 = $0204 (LACE set, p.51).
- **A-noprep**: `WAIT($FF,$E2)`, `MARK($A0E2)` and the padding removed.
- **A-slow**: a delay of at least 300 slots between trace samples.
- **A-noCop**: the CPU writes DMACON = $0080 (clears COPEN, p.30: "You can
  also stop the Copper by disabling its ability to use DMA") after the third
  field boundary, then keeps tracing.
- **A-anchor**: every `WAIT(v,$40)` of the first loop becomes
  `WAIT(v+1,$40)`, with v running $30 ... $FE.

### Run B: WAIT on line $FF beyond $E2 (PAL; NTSC; OCS)

BPLCON0 = $0200, DMACON = $8280. The program loads `L_init`. Each list
points COP1LC at its successor, so each list runs one complete field (p.32:
"You can change the value in the location registers at any time"; p.25: the
restart uses COP1LC).

```
L_init:  MOVE($080,hi(L_0)) MOVE($082,lo(L_0)) END
L_j, j = 0 .. 14, h_j = $E2 + 2j   (j = 0: $E2; j = 1..14: $E4 .. $FE)
  WAIT($00,$00)  MARK($B000+j)                   ; λ_min
  MOVE($080,hi(L_{j+1})) MOVE($082,lo(L_{j+1}))
  WAIT($FF,$40)  MARK($B100+j)                   ; a_j
  WAIT($FF,h_j)  MARK($B200+j)                   ; x_j
  END
L_15: identical to L_14, with COP1LC pointing to L_15 itself
```

For j = 14 the WAIT under test is `$FFFF,$FFFE`, the instruction on p.23.
The trace covers L_0 to L_14 and at least 3 fields of L_15.

Variants:

- **B-FE**: one extra field whose list is `WAIT($00,$00) MARK(λ)`, the
  COP1LC moves, `WAIT($FE,$FE)` (first word `$FEFF`) `MARK($B300)`,
  `WAIT($FF,$40)` `MARK($B301)`, END.
- **B-late**: in L_1, `WAIT($FF,$E2) MARK($B400)` is inserted before
  `WAIT($FF,h_1)`.

### Run C: horizontal position sweep (PAL; NTSC; OCS)

BPLCON0 = $0200, DMACON = $8280, trace for at least 3 complete fields.
List C:

```
WAIT($00,$00)  MARK($C000)                                ; λ_min
for k = 1 .. 113:  line u_k = $30 + k,  h_k = 2k   (h_k = $02 .. $E2)
  if h_k ≤ $80:  WAIT(u_k,h_k) MARK($C100+k)   WAIT(u_k,$C0) MARK($C200+k)
  else:          WAIT(u_k,$10) MARK($C200+k)   WAIT(u_k,h_k) MARK($C100+k)
for j = 0 .. 13:  line w_j = $A4 + 2j,  g_j = $E4 + 2j   (g_j = $E4 .. $FE)
  WAIT(w_j,$C0)   MARK($C300+j)                           ; r1_j
  WAIT(w_j,g_j)   MARK($C400+j)                           ; q_j
  WAIT(w_j+1,$40) MARK($C500+j)                           ; r2_j
END
```

In the first loop, `$C100+k` is mark_k and `$C200+k` is ref_k. Every WAIT
under test is at least 16 counts after the Copper's previous event, so it is
fetched before its target.

Variants:

- **C-shift**: for k = 50 (h = $64), `WAIT(u_50,$66)` instead of
  `WAIT(u_50,$64)`.
- **C-late**: for k = 90 (h = $B4), the reference is
  `WAIT(u_90,$C4) MARK($C290)`, placed before `WAIT(u_90,$B4)`.
- **C-next**: for j = 5, `WAIT(w_5+1,$20)` instead of `WAIT(w_5,g_5)`.

### Run D: pixel calibration and Copper edge sweep (PAL; NTSC; ECS)

Common settings:

- DIWSTRT = $2C81.
- DIWSTOP = $2CC1 (PAL) or $F4C1 (NTSC). These are the normal values
  (p.59).
- BPLCON1 = 0, BPLCON2 = 0, BPL1MOD = 0.

Each sub-run lasts at least 3 complete fields. The list starts with
`MOVE($0E0,hi(plane)) MOVE($0E2,lo(plane))`, because bitplane pointers must
be rewritten every field (p.27). Then:

- **D-lo**:
  - BPLCON0 = $1200: one low-resolution plane, COLOR.
  - DDFSTRT = $0038, DDFSTOP = $00D0 ("The normal low resolution DDFSTRT is
    ($0038)", "DDFSTOP is ($00D0)", p.60).
  - Every plane word $AAAA, 20 words per line.
  - COLOR00 = $00F, COLOR01 = $FFF.
  - DMACON = $8380. The list continues with END.
- **D-hi**:
  - BPLCON0 = $9200: HIRES, one plane.
  - DDFSTRT = $003C, DDFSTOP = $00D4 (p.60).
  - Every word $AAAA, 40 words per line. Colours as in D-lo.
- **D-sweep**:
  - BPLCON0 = $0200, DMACON = $8280 (no bitplanes).
  - B = $0F0, and A_u = $F00 + (u - $40) for line u.

The D-sweep list:

```
WAIT($00,$00)  MARK($D000)                  ; λ_min
MOVE($180,B)
sub-sweep 1: for n = 8 .. 77, line u = $40 + (n - 8)            (u = $40 .. $85)
  WAIT(u,$36)  MOVE($180,A_u)  WAIT(u,$36+2n)  MOVE($180,B)
sub-sweep 2: for m = 0 .. 69, line u = $86 + m, n = 77 - m      (u = $86 .. $CB)
  WAIT(u,$36+2m)  MOVE($180,A_u)  WAIT(u,$D0)  MOVE($180,B)
END
```

Sub-sweep 1 fixes the first edge at $36 and moves the second edge through
every position from $46 to $D0. Sub-sweep 2 fixes the second edge at $D0 and
moves the first edge through every position from $36 to $C0. Each line u has
a known n_u, the number of Copper positions between its two WAITs. No
position after $D0 is used, for the reason given in P4.

Variants:

- **D-shift**: on line $50 (sub-sweep 1, n = 24), the second WAIT is at
  $36 + 2·24 + 2.
- **D-drop**: on line $50 the second `MOVE($180,B)` is left out.
- **D-flat**: D-lo with every plane word $FFFF.
- **D-late**: on line $50 the second WAIT is at $3A.

### Run E: standard screen (PAL; NTSC; ECS)

Settings:

- BPLCON0 = $1200. DIWSTRT = $2C81; DIWSTOP = $2CC1 (PAL) or $F4C1 (NTSC).
- DDFSTRT = $0038, DDFSTOP = $00D0, BPL1MOD = 0.
- Every plane word $FFFF.
- COLOR00 = $00F, COLOR01 = $FF0.
- DMACON = $8380.
- List: `MOVE($0E0,..) MOVE($0E2,..) END`.
- At least 3 complete fields.

Variants:

- **E-widefetch**: DDFSTRT = $0030, DDFSTOP = $00D8, 22 words of $FFFF per
  line (p.60: "DDFSTRT = DDFSTOP - (8 * (word count - 1))"; p.80: $D8 is the
  hardware limit), window unchanged.
- **E-widewindow**: E-widefetch with DIWSTRT = $2C71 and DIWSTOP = $2CD1.
- **E-black**: COLOR00 = $000.

## 6. Propositions

---

### P1. A non-interlaced field has 262 lines (NTSC) or 312 lines (PAL), and the beam starts each field at (0,0)

**Claims.** `COP2-vertical-beam-position.c3`, `COP2-vertical-beam-position.c4` (p.24);
`COP2-using-the-copper-in-interlaced-mode.c2`, `COP2-using-the-copper-in-interlaced-mode.c4` (p.34);
`COP2-putting-together-a-copper-instruction-list.c7` (p.27).

**Stimulus.** Run A (PAL and NTSC).

**Observation.** The beam trace (RF2) in each complete field (RF3).
Trusted samples only.

**Expected.**

- For every complete field, the number of distinct V values is 312 (PAL) or
  262 (NTSC).
  - p.24: "There are actually 262 NTSC (312 PAL) possible vertical
    positions."
  - p.34: "Whereas a normal NTSC display has 262 lines ... PAL has 312 lines
    normally".
  - The same figures are on p.40.
- In every complete field, the first trusted sample has V = 0.
- H = 0 occurs in the trace (in any sample).
- p.27: "The video beam traverses the screen from position (0,0) in the
  upper left hand corner".
- Quantifier: every complete field of the trace, at least 6.
- The largest V is not graded (section 7, item 1).

**Preconditions.**

- The completion word is written.
- T1, T2 and R1 hold.
- At least 6 complete fields are traced.

**Contrasting cases.**

- Main: select NTSC for the PAL check, or PAL for the NTSC check. p.24 then
  predicts the other count.
- Main: A-LACE (PAL). p.24: "there are also long and short fields
  (interlace only) ... in PAL, 312, then 313 lines" (p.34 agrees: "625 in
  interlaced mode"). A field with 313 distinct values appears, and "every
  field has 312" goes red.
- T1: A-slow. Lines are missed and T1 fails.
- T2: read $DFF002 (DMACONR) instead of $DFF004. H no longer counts and T2
  fails.
- R1: A-noCop. Fields after the third have no restart and R1 fails.
- Completion word: leave out step 6.

**Reference frame.** RF2 and RF3. The field boundary is checked against the
restart by R1.

---

### P2. The Copper sees every PAL line as 227 counts; in NTSC, lines alternate between 228 and 227

**Claims.** `COP2-horizontal-beam-position.c10`, `COP2-horizontal-beam-position.c11`, `COP2-horizontal-beam-position.c12`, `COP2-horizontal-beam-position.c14`,
`COP2-horizontal-beam-position.c15` (p.23); `COP2-vertical-beam-position.c11` (p.24).

**Stimulus.** Run A (PAL and NTSC). The markers `m_v` come from
`WAIT(v,$40)` on lines v = $30 ... $FF, and `m_(256+n)` from `WAIT(n,$60)`.

**Observation.**

- L(v) = time(m_{v+1}) - time(m_v), in slots, for v = $30 ... $FE.
- L(v) = time(m_{v+1}) - time(m_v) for v = 256 ... 256+N-1 (the markers
  after the wrap all use $60).
- Every complete field.

**Expected.**

- PAL: L(v) = 227 for every measured v.
- NTSC: L(v) is 227 or 228 for every measured v, and L(v) ≠ L(v+1) for
  every consecutive pair in each run of consecutive measured lines.
- Sources:
  - p.23: "All lines are not the same length in NTSC. Every other line is a
    long line (228 color clocks, 0-$E3), with the others being 227 color
    clocks long. In PAL, they are all 227 long. ... the Copper sees
    alternating long and short lines."
  - p.24: "One horizontal count takes one cycle of the system clock".
  - p.194 equates "color clocks" with "memory access cycles".
  - p.285 says of STRLONG: "used during the second refresh time slot of
    every other line to identify lines with long counts (228)".
- The difference of two markers after WAITs at the same count on
  consecutive lines is one line's length in counts, because the Copper's
  wait ends when "the video beam counters are equal to (or greater than)"
  the position (p.22).
- Quantifiers: "every other line" and "they are all" are graded over every
  measured line in every complete field.

**Preconditions.**

- Every `WAIT(v,$40)` and `WAIT(n,$60)` waited.
- A-anchor: every m_v lies on line v (RF2).
- T1, T2, R1.

**Contrasting cases.**

- Main: select NTSC for the PAL check. L alternates, so "every L = 227" is
  red. Select PAL for the NTSC check. No alternation, so it is red.
- "waited": insert `WAIT($7F,$40)` directly after `MARK($7F)`. It is
  fetched after its target has passed, so its delay equals λ_min.
- A-anchor: variant A-anchor. The markers lie on line v + 1.
- T1, T2, R1: as in P1.

**Reference frame.** RF1. RF2 identifies lines, anchored by A-anchor.

---

### P3. The horizontal beam count runs 0 to $E2 on PAL lines; NTSC long lines reach $E3, and only they do

**Claims.** `COP2-horizontal-beam-position.c13`, `COP2-horizontal-beam-position.c12`, `COP2-horizontal-beam-position.c15` (p.23); x
part of `COP2-putting-together-a-copper-instruction-list.c9`, `COP2-putting-together-a-copper-instruction-list.c10`
(p.27).

**Stimulus.** Run A (PAL and NTSC).

**Observation.**

- The H values of all trace samples. The H value is read together with V7-V0
  in one read, so every sample may be used.
- For NTSC, the line type of each sample on a line v whose length L(v) P2
  measured.

**Expected.**

- PAL: the set of H values observed is exactly {$00, ..., $E2}.
  - p.23: "The horizontal beam position has a value of $0 to $E2".
  - p.27: the end of the display has x = 226 = $E2.
- NTSC:
  - The set of H values observed is exactly {$00, ..., $E3}.
  - No sample with H = $E3 lies on a line with L(v) = 227.
  - p.23: "Every other line is a long line (228 color clocks, 0-$E3), with
    the others being 227 color clocks long."
- Quantifier: "0-$E3" is graded as the full set of values and as the
  restriction of $E3 to long lines.

**Preconditions.**

- T1 and T2.
- NTSC coverage: H ≥ $E0 is observed on at least 20 distinct lines with
  L(v) = 228 and on at least 20 distinct lines with L(v) = 227. This shows
  that lines of both types were sampled near their end, so "only long lines
  reach $E3" is not green merely because short lines were never sampled
  there. Samples fall on shifting positions because lines are 227 or 228
  slots long, and the CPU uses the even memory cycles (p.196).
- PAL needs no coverage precondition. The expected set itself requires every
  value up to $E2 to be seen, so sampling that never reaches the end of a
  line turns the check red, not green.
- P2's preconditions, for the NTSC line types.

**Contrasting cases.**

- Main: the PAL check under NTSC ($E3 appears: red). The NTSC check under
  PAL ($E3 absent: red).
- NTSC coverage: select PAL. No line has L(v) = 228, so the coverage
  precondition fails.

**Reference frame.** RF2 values, with P2's line lengths.

---

### P4. The Copper's horizontal comparison advances one count per memory cycle, gives 113 distinct positions $02 ... $E2, and has no position beyond $E2

**Claims.** `COP2-horizontal-beam-position.c3` (p.23);
`COP2-vertical-beam-position.c23` (p.24); `COP2-copins-timing.c6` (p.275,
as a relation).

**Stimulus.** Run C (PAL and NTSC).

**Observation.**

- For each k in 1 ... 113: D_k = time(mark_k) - time(ref_k), in slots.
- For each j in 0 ... 13: E_j = time(r2_j) - time(q_j), and the line of
  r2_j (RF2).
- Every complete field.

**Expected.**

(a) D_k = h_k - H_ref for every k with h_k ≤ $D0, where H_ref = $C0 for
h_k ≤ $80 and $10 otherwise. For $C0 the difference is negative.

- p.22: the wait lasts "until the video beam counters are equal to (or
  greater than) the coordinates".
- p.23: "The least significant bit is not used in the comparison, so there
  are 113 positions available for Copper operations."
- p.24: "One horizontal count takes one cycle of the system clock".
- p.275 and p.285: both a memory cycle and one step of the beam count last
  280 ns.
- Both WAITs of a line end at even counts in the same line, so the fixed
  wake-up of p.20 ("it takes one extra memory cycle to wake up") cancels.

(b) For h_k in $D2 ... $E2 (k = 105 ... 113):

- D_k - (h_k - $10) is 0 or 1.
- D_k strictly increases with k.

The Copper "requests the bus only during odd-numbered memory cycles" (p.20).
A 227-count line ends at $E2 and the next begins at $00, so two even cycles
follow each other there. A fetch that would fall past the end of the line
moves one slot later. The pages do not say how many slots after the end of a
wait the next fetch comes, so they do not say which of these positions are
affected. The spacing of 2 counts per position, and the 113 distinct values
$02 ... $E2, are graded exactly through (a) and through the strict ordering
of (b). p.32 confirms the upper end: "Position $E2 is horizontal position
113 for the Copper and the last real horizontal position available."

(c) For every j: E_j = $40, and r2_j lies on line w_j + 1.

- A WAIT for g_j > $E2 on line w_j ends when line w_j + 1 begins, the same
  moment a WAIT for ($w_j+1, $00) would end.
- p.23: "The horizontal position has a maximum value of $E2", and "as soon
  as the line number becomes higher than the entered number, the comparison
  will evaluate to true and the wait will end".
- p.30: "An HP of greater than $E2 is not possible."

Quantifiers: "113 positions" is graded over all 113 values in every complete
field; "maximum value" over all 14 values above $E2. Position $00 is not
graded as one of the 113. It is the start of the line, and a wait for it ends
at the same moment as any wait for a position beyond $E2 on the line before
(section 7, item 3).

**Preconditions.**

- Every WAIT under test waited: the first-word WAITs of each line, the
  second-word WAITs, and `WAIT(w_j,g_j)`.
- For every j, r1_j lies on line w_j. The WAIT for g_j was then fetched
  during line w_j and was pending when that line ended.
- T1, T2, R1.

**Contrasting cases.**

- (a): C-shift. D_50 changes by 2 slots.
- (c): C-next. The wait ends at count $20 of line w_5 + 1, so E_5 = $20.
- "waited": C-late. The WAIT for $B4 is fetched after count $C4, so its
  delay equals λ_min.
- "r1_j on line w_j": move `WAIT(w_j,$C0)` to `WAIT(w_j+1,$C0)`. r1_j lies
  on line w_j + 1.
- T1, T2, R1: as in P1.

**Reference frame.** RF1. RF2 identifies lines for r1 and r2.

---

### P5. One Copper horizontal position step is 4 low-resolution pixels and 8 high-resolution pixels on screen

**Claims.** `COP2-horizontal-beam-position.c4`, `COP2-horizontal-beam-position.c5`;
`ch2-horizontal-beam-position.c4`, `ch2-horizontal-beam-position.c5` (p.23).

**Stimulus.** Run D: D-lo, D-hi and D-sweep in the same configuration (PAL;
NTSC in addition).

**Observation.**

- D-lo: in every output line that shows COLOR01, the lengths of the COLOR01
  runs and of the COLOR00 runs between them. Their common length is P_lo.
- D-hi: the same, giving P_hi.
- D-sweep: in every output line, the run of colour A_u identifies the line
  u. Its length, from the B→A_u edge to the A_u→B edge, is w_u in output
  pixel clocks.

**Expected.**

- For every sweep line u: w_u = 4 · n_u · P_lo, and w_u = 8 · n_u · P_hi.
  - p.23: "This corresponds to 4 pixels in low resolution and 8 pixels in
    high resolution."
  - p.216: "Horizontal resolution is 1/160th of the screen width", with a
    320-pixel screen: one count is 2 low-resolution pixels.
  - p.271: "HIRES=High-resolution (70 ns pixels)", with p.275's 280 ns
    memory cycle: one count is 4 high-resolution pixels.
  - One position is 2 counts, because the least significant bit is not
    compared (p.23).
- Both edges are made by the same WAIT-MOVE pair on the same line. The data
  of a MOVE goes "at IR2 time directly from RAM to the address given by the
  DA field" (p.275), so the delay from wait to colour change is the same for
  both edges and cancels.
- c_px = w_u / (2 n_u) must then be the same for every u. It is used by
  P10.
- Quantifier: every position from $36 to $D0 is an edge position, and every
  single step between neighbouring positions in that range is covered.
  Sub-sweep 1 gives w for consecutive n at the second edge; sub-sweep 2 does
  the same at the first edge. Positions in $0F-$35 are blanked (P10) and
  cannot be seen. Positions after $D0 are not graded for the reason in
  P4(b). Positions $00-$0E are not graded, because a width that crosses the
  counter's line boundary depends on where the displayed line begins
  relative to the counter line, which no page states.

**Preconditions.**

- Calibration:
  - D-lo: each line inside the window shows 160 COLOR01 runs, and all
    COLOR01 runs and the 159 COLOR00 runs between them have one common
    length.
  - D-hi: 320 COLOR01 runs, likewise of one common length.
- Edges present: every sweep line u shows exactly one run of A_u, with B
  before and after it on the same output line.
- Every sweep WAIT waited.
- The completion word is written; R1 holds.

**Contrasting cases.**

- Main: D-shift. w on line $50 becomes 4 · 25 · P_lo while n = 24.
- Calibration: D-flat. There are no alternating runs.
- Edges present: D-drop. Line $50 has no A→B edge.
- "waited": D-late. The second WAIT of line $50 is already satisfied when it
  is fetched, so its delay equals λ_min.

**Reference frame.** RF4 pixel units, anchored by the calibration runs.

---

### P6. A WAIT for vertical $FF reaches line $FF and horizontal $E2, and never a horizontal position beyond $E2; `$FFFF,$FFFE` never ends and the Copper stops until the next restart

**Claims.** `COP2-the-wait-instruction.c26`, `COP2-the-wait-instruction.c27`, `COP2-the-wait-instruction.c28`, `COP2-the-wait-instruction.c29`
(p.23).

**Stimulus.** Run B (PAL and NTSC). The positive $FF case is also in Run A
(`m_$FF`, `m_E2`).

**Observation.** For each list L_j, from the fetch of the second word of
`WAIT($FF,h_j)` to the next restart:

- whether x_j is fetched;
- whether any Copper fetch occurs.

Also:

- the line of a_j (RF2);
- for j = 0, time(x_0) - time(a_0).

**Expected.**

- a_j lies on line 255 in every field. p.23: "When waiting for $FFFE, the
  line $FF will be reached".
- j = 0 (h = $E2): x_0 is fetched in the same field, and time(x_0) -
  time(a_0) is $E2 - $40 = $A2, or $A3 (P4(b)). p.23: "The horizontal
  position has a maximum value of $E2. This means that the largest number
  that will ever appear in the comparison is $FFE2."
- j = 1 ... 14 (h = $E4 ... $FE), and every field of L_15: x_j is never
  fetched, and no Copper fetch at all occurs between the second word of
  `WAIT($FF,h_j)` and the next restart.
  - p.23: "the horizontal position $FE will never happen. Thus, the position
    will never reach $FFFE", and "This event will never occur, so the Copper
    stops until the next vertical blanking interval begins."
  - p.22: "While waiting, the Copper is off the bus and not using memory
    cycles."
- Quantifiers: "never" is graded over all 14 values above $E2 and, for $FE,
  over at least 4 fields. "Largest number" is graded by $E2 reached and
  every larger value not reached.

**Preconditions.**

- For every j, the second word of `WAIT($FF,h_j)` is fetched on line 255
  (RF2). The WAIT was then pending during line 255, after count $40.
- Each restart fetches the list L_{j+1}, or L_15, that the previous list
  named. This shows the chain works and each field ran the intended list.
- Every `WAIT($FF,$40)` waited.
- T1, T2, R1.

**Contrasting cases.**

- Main: B-FE. `WAIT($FE,$FE)` ends when line $FF begins, so its marker is
  fetched, and time($B301) - time($B300) = $40. p.23: "If the vertical
  position is not $FF, then as soon as the line number becomes higher than
  the entered number, the comparison will evaluate to true and the wait
  will end."
- "$E2 reached": give L_0 h = $E4. x_0 is not fetched.
- "fetched on line 255": B-late. The WAIT for h_1 is fetched on line 256.
- Chain: point L_3's COP1LC at L_5. The restart after L_3 fetches L_5, not
  L_4.
- "waited", T1, T2, R1: as in P2 and P1.

**Reference frame.** RF2 and RF3. Restarts close the observation interval.

---

### P7. In the Copper's comparison, line 256 appears as zero, and line 256+n as n; the beam counter itself does not wrap

**Claims.** `COP2-the-wait-instruction.c22`, `COP2-the-wait-instruction.c23` (p.23);
`COP2-vertical-beam-position.c7` (p.24).

**Stimulus.** Run A (PAL and NTSC). After `WAIT($FF,$E2)` and 8 padding
moves, `WAIT(n,$60)` with `MARK($0100+n)`, for n = 0 ... N.

**Observation.**

- The line of each marker m_(256+n) (RF2), and V8 of the samples on that
  line.
- The delay of `WAIT($00,$60)`.
- Every complete field.

**Expected.**

- `WAIT($00,$60)` waited.
- Every m_(256+n) lies on line 256 + n: n = 0 ... 5 in NTSC, n = 0 ... $37
  (lines 256 ... 311) in PAL.
- Every trace sample on those lines has V8 = 1.
- Sources:
  - p.23: "Since 255 is the maximum number, the next line will wrap to zero
    (line 256 will appear as a zero in the comparison.)"
  - p.24: "the vertical counter appears to wrap to 0 because the comparison
    works on the least significant bits of the vertical count".
  - p.216: V8 "Allows PAL line counts (313) to appear".
- The WAIT for ($00,$60) is fetched during line 256 and did not end at once.
  The comparison therefore saw line 256 as below vertical 1, and the marker
  on line 256 shows it saw it as vertical 0. A comparison that saw line 256
  as a number above $FF would end the wait at once.
- Quantifier: every line from 256 to the field's last line that the list
  addresses, in every complete field.

**Preconditions.**

- The second word of `WAIT($00,$60)` is fetched on line 256 (RF2).
- m_E2 is fetched.
- A-anchor, T1, T2, R1.

**Contrasting cases.**

- Main: A-noprep. `WAIT($00,$60)` is fetched on line 255 right after
  `m_$FF`. It ends at once (p.23, quoted in P6), and m_256 lies on line 255.
- "fetched on line 256": the same variant A-noprep. The WAIT's second word
  is fetched on line 255.
- A-anchor, T1, T2, R1: as in P2 and P1.

**Reference frame.** RF2, anchored by A-anchor.

---

### P8. NTSC: after line 255, vertical positions 0 through 5 are the last six lines of the field before vertical blanking, 256 to 261; there is no line 262

**Claims.** `COP2-vertical-beam-position.c9`, `COP2-vertical-beam-position.c10` (p.24).

**Stimulus.** Run A, NTSC. The markers m_256 ... m_261 are from
`WAIT(0,$60)` ... `WAIT(5,$60)`; m_262 is from `WAIT(6,$60)`.

**Observation.**

- The lines of m_256 ... m_261.
- The line that follows 261 in the trace.
- Whether m_262 is fetched before the next field boundary.

**Expected.**

- m_(256+n) lies on line 256 + n for n = 0 ... 5.
- In every complete field, the trusted sample after line 261 is on line 0,
  which is the start of vertical blanking. p.219: "Bit 5, VERTB, causes an
  interrupt at line 0 (start of vertical blank)"; p.79: "Vertical Blank
  Start 0".
- m_262 is not fetched between m_261 and the next field boundary.
- p.24: "WAIT for any horizontal position with vertical position 0 through
  5, covering the last 6 lines of the scan before vertical blanking occurs.
  ... Thus the total of 256 + 6 = 262 lines of video beam travel during
  which Copper instructions can be executed."
- Quantifier: every complete field.
- Not graded:
  - Whether m_262 is fetched after the boundary. That depends on where the
    Copper restarts, which the pages disagree on (section 7, item 8).
  - Copper execution on the lines of vertical blanking before the restart,
    for the same reason.

**Preconditions.** As in P7.

**Contrasting cases.**

- Main: select PAL. m_262 lies on line 262 before the next field boundary
  (PAL has 312 lines, P1), and "not fetched" is red.
- Preconditions: as in P7.

**Reference frame.** RF2 and RF3.

---

### P9. The standard screen is 320 low-resolution pixels wide; in the rest of each of its lines only the background colour is displayed, and the horizontal blanking lies inside that rest with background on both sides

**Claims.** `COP2-horizontal-beam-position.c7`, `COP2-horizontal-beam-position.c8`, `COP2-horizontal-beam-position.c9`;
`ch2-horizontal-beam-position.c7`, `ch2-horizontal-beam-position.c8` (p.23).

**Stimulus.**

- Run E in PAL and in NTSC: the normal DIWSTRT and DIWSTOP of p.59 and the
  normal DDFSTRT and DDFSTOP of p.60, one plane of all 1 bits.
- Also E-widefetch. Data is fetched beyond the window, so something other
  than background could appear in the unused portion.

**Observation.** Every output line that shows at least one COLOR01 pixel
(the window lines), in every complete field.

**Expected.**

(a) The line consists of exactly one run of COLOR01, of length 320 · P_lo,
and apart from that run only of COLOR00 and 0 (blanking) pixels.

- p.23: "The standard screen (320 pixels wide) has an unused horizontal
  portion ... (during which only the background color is displayed)".
- p.59: HSTART $81, HSTOP $1C1.
- p.57: "You cannot display objects outside of the defined display
  window."

The width and the only-background statement are one check, because outside
the playfield run the only colour other than background that this stimulus
can produce is the playfield colour itself.

(b) Going cyclically from the end of the COLOR01 run to its start, the line
shows at least one COLOR00 pixel, then one run of 0, then at least one
COLOR00 pixel, and nothing else. p.23 gives the unused portion as $04 to
$47 and the blanking as $0F to $35: the blanking begins after the unused
portion begins and ends before it ends.

Quantifier: "only" is graded on every pixel of every window line, in both
the standard stimulus and E-widefetch. The positions $04 and $47 are not
graded (section 1).

**Preconditions.**

- P_lo is valid: the calibration precondition of P5 in the same
  configuration.
- Each window line has exactly one run of pixels of value 0.
- The number of window lines per field is at least 1. The vertical extent
  is not graded.
- The completion word is written; R1 holds.

**Contrasting cases.**

- (a): E-widewindow. Pixels in the former unused portion show COLOR01, and
  the COLOR01 run becomes 352 · P_lo (p.59: the window spans HSTART to
  HSTOP).
- (b): E-black. The background is 0, so no background pixel is
  distinguishable on either side of the blanking.
- "one run of 0": E-black with the 10th plane word of every line set to
  $0000. The gap in the playfield shows COLOR00 = 0, a second run of 0 on
  each window line.
- Calibration: D-flat.

**Reference frame.** RF4.

---

### P10. Each line has one horizontal blanking interval, no longer than the range $0F to $35 (39 counts)

**Claims.** `COP2-horizontal-beam-position.c6`;
`ch2-horizontal-beam-position.c6` (p.23).

**Stimulus.** Run E, PAL.

**Observation.** In every window line (as in P9), the length of the run of
pixels of value 0 (RGB all zero), in output pixel clocks, divided by c_px.
This is the blanking length in counts.

**Expected.**

- Exactly one such run per line, of at most 39 counts.
- p.23: "Horizontal blanking falls in the range of $0F to $35." From $0F to
  $35 inclusive is 39 counts.
- The sentence bounds where the blanking lies; it does not say that
  blanking fills the whole range, so no lower bound is graded.
- p.80 is consistent: "horizontal blanking actually limits the displayable
  video to 368 low resolution pixels (23 words)", which fits within the 188
  counts a 39-count blanking leaves.
- Quantifier: every window line of every complete field.
- NTSC is not graded. The page says the display sees NTSC lines differently
  from the counter (227 1/2 against 227 or 228) and does not say in which
  of the two the blanking is placed.

**Preconditions.**

- c_px is valid: P5's D-sweep precondition holds and c_px is the same on
  every sweep line, in the same configuration.
- The output line carries the whole counter line: the length of every window
  output line is 227 · c_px pixel clocks. Only then does a pixel run measure
  counts of the chip set's line and not the line doubler's.
- COLOR00 ≠ 0 and COLOR01 ≠ 0, so the only zero pixels are blanking.
- R1 holds.

**Contrasting cases.**

- Main: E-black. The zero run now includes the background of the unused
  portion, and its length exceeds 39 counts.
- "whole counter line": select NTSC. Lines are no longer 227 counts each
  (P2), and the PAL-valued check fails.
- "COLOR00 ≠ 0": E-black.
- c_px: D-shift makes c_px differ on line $50.

**Reference frame.** RF4, with c_px from P5.

## 7. Where the pages disagree or leave things open

1. **End of the display.**
   - p.27: "to the end of the display (226,262) NTSC (or (226,312) PAL)",
     starting at (0,0).
   - p.24 and p.34 give 262 and 312 lines. Counted from 0, the last line is
     261 or 311. p.100 allows "any position from line 0 to line 262" for
     sprites.
   - p.23 gives NTSC long lines "0-$E3", beyond p.27's x = 226.
   - Specified: the line count, the start at 0, and H up to $E2 on PAL
     lines. The last line number and the NTSC last x are not specified.
2. **Interlaced NTSC.**
   - p.34: "an interlaced NTSC display has 524 lines" and "displaying, in
     the case of NTSC, 262 lines at a time".
   - p.24: "In NTSC, the fields are 262, then 263 lines". p.79: 483 =
     "525-(21*2)". p.304: "NTSC (525 lines ...)".
   - The interlace contrast is therefore run in PAL only, where p.24 (312
     then 313) and p.34 (625) agree.
3. **113 positions.**
   - p.23 gives the range "$0 to $E2" with the lowest bit unused. That is
     114 even values, 0 to $E2, but the page says "113 positions".
   - p.32: "Position $E2 is horizontal position 113".
   - Both hold if position 0, the line start, is not counted. P4 grades
     $02 ... $E2.
4. **Maximum horizontal value.**
   - p.23: "The horizontal position has a maximum value of $E2", and in the
     same paragraph "long line (228 color clocks, 0-$E3)".
   - The two agree once the lowest bit is dropped: $E3 compares as $E2. The
     beam counter read through VHPOSR can show $E3 (P3); the Copper cannot
     wait for it.
5. **Unused portion and blanking positions.**
   - p.23: "an unused horizontal portion of $04 to $47". Inclusive, that is
     68 counts; with a 320-pixel (160-count) screen it makes 228 counts,
     while p.23 gives 227 for a PAL line. The page does not say whether $47
     is included.
   - No page gives the delay from a counter value to a displayed pixel.
   - The pages place the standard screen's first displayed pixel at
     different counter values: p.60 (window start "$81 / 2", 8.5 counts
     after DDFSTRT $38), Figure 6-9 on p.195 ("if data fetch start is $38,
     data will not be available for display until clock number $45"), and
     p.23 (unused portion up to $47).
6. **Display line length in PAL.**
   - p.23: "In PAL, they are all 227 long. The display sees all these lines
     as 227 1/2 color clocks long, while the Copper sees alternating long
     and short lines."
   - The last clause is true only of NTSC, so it is unclear whether "these
     lines" includes PAL.
   - p.194 gives "227.5 'color clocks'" per line without qualification.
     p.304 gives "PAL (625 lines, 227.5 colorclocks per scan line)".
7. **Recipe for the last lines (p.24).**
   - The recipe begins "WAIT for position (0,255)" and then "WAIT for any
     horizontal position with vertical position 0 through 5".
   - With the greater-or-equal rule (p.22), and p.23's "as soon as the line
     number becomes higher than the entered number, the comparison will
     evaluate to true", a WAIT for vertical 0 to 5 fetched during line 255
     ends at once.
   - The recipe works only once the Copper is past line 255. Runs A and B
     get there with `WAIT($FF,$E2)`, $FFE2 being "the largest number that
     will ever appear in the comparison" (p.23), followed by padding.
8. **When the Copper restarts.**
   - p.25, within one box: "At the start of each vertical blanking interval,
     COP1LC is automatically used", and "when the end of vertical blanking
     occurs, the Copper is automatically forced to restart".
   - p.30, p.32 and p.276 say start. p.33 says COP1LC is written by the
     operating system "at the end of the vertical blanking period".
   - Specified: one restart per field; nothing that depends on its
     position.
9. **"System clock".**
   - p.24: one horizontal count is one cycle of the system clock, "NTSC-
     3,579,545 Hz".
   - p.2: "The 68000's system clock speed is 7.15909 MHz". p.193: "The
     system clock speed for NTSC Amigas is 7.16 megahertz ... The clock for
     the blitter is the system clock."
   - The rates agree (one is twice the other); the name does not. This
     document counts memory slots.
10. **Memory cycle duration.** 280 ns (p.275, p.285); 279.365 ns NTSC and
    281.937 ns PAL (p.141); "approximately 280 ns" (p.194); 279.36 ns
    (p.255). Not graded.
11. **Strobe access.** p.30's text says "write to its strobe address"; its
    example code reads COPJMP1 (`MOVE.W COPJMP1(a1),d0`); p.276 says the
    strobe address is "hit". The program writes.
12. **Vertical blanking end.** p.79 gives stop lines $15 (NTSC) and $1D
    (PAL). p.219: "ends at line 20 for NTSC or line 25 for PAL". Every
    measured Copper event in this package is at line $30 or later, beyond
    all of these.
13. **VPOSR bit 7 under ECS.** p.299 names it "LOL" without describing it.
    It is not used.
