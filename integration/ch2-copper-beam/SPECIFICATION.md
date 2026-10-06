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
| `COP2-copins-timing.c6` (p.275: "memory cycles are 280 ns") | The claim is a duration in nanoseconds. The test sees time only in `clk_sys` cycles of a simulation whose clock the test bench supplies, and the harness states no period for that clock (`nanomig_harness.h`), so no duration in nanoseconds is observable. Other pages give 279.365 ns (NTSC) and 281.937 ns (PAL) (p.141) and "approximately 280 ns" (p.194). P4 uses p.275's sentence only as the reason why one horizontal count lasts one memory cycle (p.285 gives the same 280 ns for one step of the horizontal beam position); that relation is `COP2-vertical-beam-position.c23`. |
| `COP2-putting-together-a-copper-instruction-list.c9`, `COP2-putting-together-a-copper-instruction-list.c10` (p.27: end of display (226,262) NTSC, (226,312) PAL) | The claim is one coordinate, and neither of its values can be graded as stated. The y value: p.24 and p.34 give 262 and 312 lines; counted from line 0 (p.27 itself starts at (0,0)) the last line is 261 or 311, not 262 or 312. The x value for NTSC: p.23 says long lines run to $E3 = 227, and no page says whether an NTSC field ends on a long or a short line. See section 7, item 1. P3 grades the horizontal count's range on PAL lines, which agrees with x = 226 for PAL, but not the coordinate. |
| `COP2-horizontal-beam-position.c8`, `ch2-horizontal-beam-position.c7` (p.23: unused portion "$04 to $47") | The claim is a pair of positions on the counter. No page gives the delay from a counter value to a displayed pixel, the pages place the standard screen's first pixel at different counter values, and $04 to $47 inclusive plus a 320-pixel screen is one count longer than a 227-count line. See section 7, item 5. P9 grades what the rest of the line shows, not where it lies on the counter. |
| `COP2-horizontal-beam-position.c6`, `ch2-horizontal-beam-position.c6` (p.23: blanking "$0F to $35") | The claim is a range of positions on the counter, for the reason above; the position of the interval against the counter is not observable. No proposition grades a part of it. |
| `COP2-vertical-beam-position.c10` (p.24: "the total of 256 + 6 = 262 lines of video beam travel during which Copper instructions can be executed") | The claim covers every one of the 262 lines. Copper execution on the lines of vertical blanking depends on where the Copper restarts, which the pages state two ways (section 7, item 8): a restart at the end of vertical blanking leaves the first lines of a field without Copper execution. P8 grades the six lines after line 255 and the absence of a line 262, not the whole claim. |

Claims graded for NTSC only, because their page states them for NTSC:

| Claim | Graded by | Why this is the whole claim |
|---|---|---|
| `COP2-vertical-beam-position.c9` (p.24: vertical 0 through 5 cover the last six lines) | P8, NTSC. | The page's numbers are NTSC's: "256 + 6 = 262", while the same page gives 312 lines for PAL. In PAL, vertical 0 through 5 are not the last six lines, and the page states no PAL counterpart. The recipe's first step read literally cannot work (section 7, item 7); the stimulus enters line 256 by a route every page supports. |
| `COP2-vertical-beam-position.c11` (p.24: "There are alternating long and short lines") | P2, NTSC. | p.23 restricts the alternation to NTSC ("All lines are not the same length in NTSC", "In PAL, they are all 227 long"), and the p.24 box that states c11 says "This alternation of lines and fields produces the standard NTSC 4 field repeating pattern". |

## 2. Claim map

| Proposition | Claims | Pages |
|---|---|---|
| P1 Lines per field; the beam starts at (0,0) | `COP2-vertical-beam-position.c3`, `COP2-vertical-beam-position.c4`; `COP2-using-the-copper-in-interlaced-mode.c2`, `COP2-using-the-copper-in-interlaced-mode.c4`; `COP2-putting-together-a-copper-instruction-list.c7` | 24, 34, 27 |
| P2 Line length as the Copper sees it | `COP2-horizontal-beam-position.c10`, `COP2-horizontal-beam-position.c11`, `COP2-horizontal-beam-position.c12`, `COP2-horizontal-beam-position.c14`, `COP2-horizontal-beam-position.c15`; `COP2-vertical-beam-position.c11` | 23, 24 |
| P3 Range of the horizontal beam count | `COP2-horizontal-beam-position.c13` | 23, 27 |
| P4 Copper horizontal positions: one count per memory cycle, 113 positions, nothing beyond $E2 | `COP2-horizontal-beam-position.c3`; `COP2-vertical-beam-position.c23` | 23, 24, 275 |
| P5 One Copper position is 4 low-resolution or 8 high-resolution pixels | `COP2-horizontal-beam-position.c4`, `COP2-horizontal-beam-position.c5`; `ch2-horizontal-beam-position.c4`, `ch2-horizontal-beam-position.c5` | 23 |
| P6 WAIT on line $FF: $FF is reached, $FFE2 is the largest value, $FFFE never | `COP2-the-wait-instruction.c26`, `COP2-the-wait-instruction.c27`, `COP2-the-wait-instruction.c28`, `COP2-the-wait-instruction.c29` | 23 |
| P7 Line 256 appears as zero in the comparison; the counter does not wrap | `COP2-the-wait-instruction.c22`, `COP2-the-wait-instruction.c23`; `COP2-vertical-beam-position.c7` | 23, 24 |
| P8 NTSC: vertical 0 through 5 after line 255 are the last six lines | `COP2-vertical-beam-position.c9` | 24 |
| P9 Standard screen: 320 pixels, only background in the unused portion | `COP2-horizontal-beam-position.c7`, `COP2-horizontal-beam-position.c9`; `ch2-horizontal-beam-position.c8` | 23 |

## 3. Conventions

### 3.1 Configurations

- Every run: 2 MB Chip memory, executed once with each of the two CPU
  cores. Each expectation must hold for both cores.
- The chip set and the video standard are harness configuration. The builder
  takes them from the `Config` values `nanomig_harness.h` documents:
  `chipset_config` bit 3 selects the Enhanced Chip Set ("ECS: bit 3 sets the
  ecs input of Agnus and Denise", set in the default `Config`), bit 4 AGA,
  which stays off; OCS is the same `Config` with bit 3 clear. `Config`
  documents no value that selects PAL or NTSC. A configuration the harness
  cannot make leaves its runs recorded as not made, not graded. This
  includes the contrasting cases below that select the other standard.
- Every run is made with ECS; Runs A, B and C are also made with OCS.
  Chapter 2 states these points without reference to a chip set. Appendix C
  (pp.298-310) lists no change to line lengths, the beam counter's range or
  the Copper's comparison other than through BEAMCON0 (p.305, among its bits
  "LOLDIS Disable long line/short line toggle" and "VARBEAMEN Variable beam
  counter comparator enable") and the VPOSR bits v10 and v9 (p.299), which
  RF2 ignores.
- Which standard a made run has is read from the chip identification in
  VPOSR bits 14-8, which no graded clause depends on. p.299: "8361 (regular
  NTSC) or 8370 (fat NTSC) = 10 for NTSC Agnus", "8367 (regular PAL) or 8371
  (fat PAL) = 00 for PAL Agnus", "8368 (hr) or 8372 (fat-hr) = 20 for PAL,
  30 for NTSC". Runs A, B and C carry it in every trace sample; Runs D and E
  store one VPOSR word at RESULTS (3.4, step 6). The run is graded against
  the values the pages give for that standard. An identification that names
  neither standard leaves the run recorded, not graded. Each run below names
  the standards it is graded for.
- BEAMCON0 is never written (p.305: "Do not write to this register"), and no
  page gives its value after reset. The ECS runs assume a reset state that
  changes nothing this package measures: its variable-timing and sync bits,
  among them LOLDIS (bit 11) and VARBEAMEN (bit 7), are 0, and its PAL bit
  (bit 5) agrees with the chip identification. An ECS run of A, B or C whose
  field lengths (the number of distinct V values per complete field, RF2)
  or, in Run A, line lengths (P2's L(v)) differ from those of the same run
  made with OCS shows otherwise: it is recorded with both sets of values,
  not graded. Where the OCS run is not made, the ECS run is graded under the
  assumption and its record says that the assumption was not checked. Runs
  D and E are made with ECS only; what they grade (P5: a ratio of widths;
  P9: a run of pixels) does not depend on line or field length.

### 3.2 Time

All times are in memory slots. One slot is one memory cycle, the unit that
Figure 6-9 (p.195) numbers along a line and draws 280 ns wide.

The harness logs each Chip-memory access with its `clk_sys` cycle and
states no memory-cycle length: `nanomig_harness.h` samples the RAM port once
per 7 MHz bus period and names no number of clock cycles per memory cycle.
The slot length S, in `clk_sys` cycles, is therefore measured in every run
from an event the pages fix, the memory refresh:

- p.194: per line, "4 cycles for memory refresh"; p.285: "There are four
  refresh time slots".
- Figure 6-9 draws the four refresh slots at the start of every line, each
  separated from the next by one slot not used for refresh.
- The refresh accesses are seen at the ports: the port list has an output
  `refresh` ("ram refresh cycle"), and the Chip-memory port log shows the
  accesses made in those slots.
- S is half the spacing, in `clk_sys` cycles, of consecutive refresh
  accesses of one line.

"slot(x)" is floor((c(x) - c_R) / S), where c(x) is the `clk_sys` cycle the
harness logs for access x and c_R is the cycle of a refresh access of the
run. Only differences of slots are used. The harness logs one entry per
7 MHz bus period, two per slot, and one bus cycle can span several entries;
entries are first merged per bus cycle (`coalesce()` in
`nanomig_harness.h`), and an access logged between two slot boundaries
belongs to the slot that began before it.

Several pages use "system clock" for different clocks (section 7, item 9),
so this document does not use the term.

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
  Copper may write it: Appendix B marks with "*" an "Address not writable
  by the Copper" and with "~" one that needs the Copper danger bit (p.287),
  and NO-OP(NULL) carries neither mark (p.293); nor do COP1LCH, COP1LCL
  (p.290), BPL1PTH, BPL1PTL (p.291) and COLOR00 (p.292), the other registers
  a list here writes. The data word t is unique within its list, so each
  marker is identified by its address and its data.
- `END`: `$FFFF,$FFFE` (p.23 and p.29, "Wait for line 255, H = 254 (ends
  Copper list)").

### 3.4 Program skeleton

Every program is assembled for the 68000, vasm's default CPU. With
`-m68010` or higher vasm assembles `MOVE #0,<ea>` as `CLR <ea>`, which on a
68000 reads the destination before writing it; step 5 depends on a plain
write of COPJMP1, and the program writes, never reads, every register it
sets.

1. Write DMACON = $7FFF and INTENA = $7FFF, which clears every DMA enable
   and interrupt enable (p.222: bit 15 is SET/CLR).
2. Clear the ROM overlay. After reset "An additional copy of the system ROM
   responds starting at memory location $00000000. The system RAM that would
   normally be located at this address is not available", and "The OVL bit
   in one of the 8520 Chips disables the overlay" (p.223). Write $03 to
   CIA-A DDRA at $BFE201 ("Direction for port A (BFE001);1-output (set to
   0x03)", p.339), then write $00 to CIA-A PRA at $BFE001, whose bit 0 is
   OVL (p.339). The Copper lists and the bitplane data lie below $080000
   (3.6), where the overlay covers Chip memory until this step.
3. Prepare the data and Copper lists in Chip memory at the addresses of 3.6
   (p.20: "the Copper can retrieve its instructions only from the chip RAM
   area").
4. Write the display registers the run names. BPLCON0 bit 0 (ENBPLCN3) is
   always 0, so BPLCON3 is not in use (p.307). DIWHIGH is never written, so
   "the old scheme for DIWSTRT and DIWSTOP described above holds" (p.306).
   BEAMCON0 and COPCON are never written.
5. Write the list's address to COP1LCH as one long-word write (p.25), then
   write COPJMP1 (p.26: "When you write to a Copper strobe address, the
   Copper reloads its program counter"). Only then write DMACON = $8280
   (SET, DMAEN, COPEN), or $8380 when bitplanes are shown (adds BPLEN). This
   is the order p.30 prescribes: "you must initialize one of the Copper
   location registers (COP1LC or COP2LC) and write to its strobe address
   before Copper DMA is turned on." (p.30's example code performs a read of
   COPJMP1; the text and p.26 say write. The program writes.)
6. Runs A, B and C: run the beam trace (RF2) until its buffer is full. Runs
   D and E: store the first VPOSR word read at RESULTS ($0C0000), for the
   chip identification (3.1), then wait the number of fields the run names
   by polling VPOSR.
7. Execute `DONE` (`nanomig.i`): it writes the completion word $600D
   (DONE_VALUE) to DONE_ADDR, $0C0FFE, then loops in place.

Sprite, audio, disk and blitter DMA are never enabled.

Precondition of every run (L1): the Copper's first fetches from the list,
after step 5, read the words the program wrote there in step 3, as the
Chip-memory port log shows both. This shows that step 2 opened the memory
below $080000 to the CPU's writes. Contrasting case: leave out step 2.

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

### 3.6 Memory layout

| Addresses | Use | Why there |
|---|---|---|
| $010000 - $01FFFF | Copper lists of every run | Every Copper list pointer reaches it. p.25: COP1LCH holds the "High 3 bits" and "you write the 18-bit address"; p.290: "high 3 bits, high 5 bits if ECS". Runs A, B and C also run with OCS (3.1), where a list at $080000 or above is beyond the pointer. |
| $020000 - $02FFFF | Bitplane data of Runs D and E | BPL1PTH holds the "high 3 bits" (p.291), and p.309 lists no extension of the bitplane pointers under ECS, so plane data at $080000 or above may lie beyond the pointer. The largest use is D-hi: 40 words per line. |
| $080400 upward | the program | PROGRAM_ORG (`nanomig.i`). |
| $0C0000 - $0C0FFF | RESULTS: the VPOSR word of Runs D and E at $0C0000; DONE_ADDR at $0C0FFE | `nanomig.i`. Nothing else is written there. |
| below $0FFF00 | stack | PROGRAM_STACK (`nanomig.i`). |
| $100000 - $1FFFFF | trace buffer (RF2) | Only the CPU reads or writes it, so no DMA pointer has to reach it; `nanomig.i` gives $080000 - $1FFFFF as the Chip memory the harness maps one-to-one. It lies clear of DONE_ADDR: a stored VHPOSR word can have the value $600D (line $60, count $0D), and `run_until_write()` in `nanomig_harness.h` would end the run at such a write to DONE_ADDR. |

The two areas below $080000 are written only after step 2 of 3.4. The test
finds a list's and the plane data's words in the Chip-memory port log at the
addresses where the program's writes of them appear.

## 4. Reference frames

### RF1: Memory slots

Defined in 3.2. Anchor checks (precondition S1), in every run:

- Every line has four refresh accesses, consecutive ones equally spaced,
  and that spacing, 2S, is the same in every line of the run (Figure 6-9,
  p.194, p.285).
- No two logged accesses, merged per bus cycle, occupy the same slot.

The program has no input that changes the refresh; S1 has no contrasting
case at this boundary.

### RF2: Beam trace

**Stimulus.** After the start sequence, the program repeats one step: read
the long word at $DFF004 and store it to the next long word of the trace
buffer (3.6). The high word of that long word is VPOSR and the low
word is VHPOSR (p.216: "the address pairs VPOSR,VHPOSR ... can be read ...
as long words, with the most significant addresses being VPOSR"). The
program stops when the buffer is full. The buffer holds at least the number
of complete fields the run names. BPLCON0 bit 3, LPEN, is 0 in every run
(section 5; p.239: "To enable the light pen input, write a 1 into bit 3
(LPEN) of BPLCON0"), so the read returns the beam position and not a
light-pen position (p.216: "when you are using a light pen, this same
address is used to read the light pen position rather than the beam
position").

**Reading a sample.** Sample i is the pair of words the CPU writes, as
logged at the Chip-memory port. Its time w_i is the slot of the later of
the two writes.

- V8 is bit 0 of the VPOSR word (p.216, p.285).
- V7-V0 are bits 15-8 of the VHPOSR word, and the horizontal count H is
  bits 7-0 (H8-H1; p.216, p.285: "RESOLUTION = 1/160 of screen width (280
  ns)").
- V = 256·V8 + V7-V0. All other VPOSR bits are ignored for V: LOF; the chip
  identification in bits 14-8, which only names the run's standard (3.1);
  under ECS LOL in bit 7, and v10 and v9 in bits 2-1 (p.299).

**Trusted samples.** A sample is trusted when H ≥ 16. Its two words come
from two reads within one loop step, and one step lasts at most 16 slots
(precondition T1). If a line began between the two reads, H is below 16
and V8 may belong to the previous line.

**Parity coverage.** A trace has parity coverage when its samples contain H
values of both parities. p.196 says "The 68000 uses only the even-numbered
memory access cycles", Figure 6-9 numbers the cycles along each line, and no
page says which count a CPU read of VHPOSR returns (section 7, item 15). The
reads may therefore fall on counts of one parity only, the "observed
parity". Parity coverage is not a precondition: P1 and P3 grade one of two
forms according to it.

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
- The video output passes a line doubler. `nanomig_harness.h`
  (`find_lines()`) splits the capture at every rising edge of `hs` and
  states that such a line "is one scan-doubler output line, not one Amiga
  scanline". Each counter line therefore appears in more than one output
  line. Every output line is measured on its own, and every condition below
  on the runs of "a line" applies per output line.
- Runs of equal colour are taken cyclically within the output line: a run
  that touches both the start and the end of the line counts once, and its
  two parts add.
- **P_lo** is the length, in output pixel clocks, of one low-resolution
  pixel, measured in Run D-lo. **P_hi** is the same for a high-resolution
  pixel, measured in Run D-hi.
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
MARK($A0F0) MARK($A0F1) ... MARK($A0FF)          ; 16 moves: padding
for n = 0 .. N:          (N = 6 for NTSC, N = $37 for PAL)
  WAIT(n,$60)  MARK($0100+n)                     ; m_(256+n)
END
```

The 16 padding moves put the next WAIT's fetch well inside line 256, where
RF2 can identify it. RF2 places an event on a line only when a trusted
sample of that line (H ≥ 16) was written before the event; with steps of up
to 16 slots (T1) and the time from a sample's read to its write, the first
such sample can be written more than 32 counts into the line. A MOVE takes
four memory cycle times (p.20: "four memory cycle times are required per
instruction"), so the 16 moves put the fetch of `WAIT(0,$60)` about 64
counts into line 256, still before its position $60.

Variants:

- **A-LACE**: PAL, BPLCON0 = $0204 (LACE set, p.51).
- **A-noprep**: `WAIT($FF,$E2)`, `MARK($A0E2)` and the padding removed.
- **A-slow**: a delay of at least 300 slots between trace samples.
- **A-noCop**: the CPU writes DMACON = $0080 (clears COPEN, p.30: "You can
  also stop the Copper by disabling its ability to use DMA") after the third
  field boundary, then keeps tracing.
- **A-anchor**: every `WAIT(v,$40)` of the first loop becomes
  `WAIT(v+1,$40)`, with v running $30 ... $FE.
- **A-shift**: `WAIT($80,$42)` in place of `WAIT($80,$40)`.
- **A-hold**: the program reads the long word at $DFF004 once, after
  polling VHPOSR until H ≥ $20, and stores that same long word in every
  step.
- **A-short**: PAL, the trace buffer holds 64 samples.

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
- **B-late**: in L_1, `WAIT($FF,$E2) MARK($B400)` followed by 16 padding
  moves `MARK($B410)` ... `MARK($B41F)` is inserted before `WAIT($FF,h_1)`.
  As in List A, the padding puts the WAIT's fetch well inside line 256,
  where RF2 can identify its line.
- **B-254**: in L_2, `WAIT($FF,$40)` becomes `WAIT($FE,$40)`, and
  `WAIT($FF,$30) MARK($B502)` is inserted directly after `MARK($B102)`. a_2
  is then fetched on line 254, and `WAIT($FF,h_2)` is still fetched on line
  255, after count $30.

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
- **C-shift-end**: for k = 106 (h = $D4), `WAIT(u_106,$D6)` instead of
  `WAIT(u_106,$D4)`, the position of k = 107.
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
- **D-flat**: D-lo, and D-hi, with every plane word $FFFF.
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
- **E-widewindow**: E-widefetch with DIWSTRT = $2C71 and DIWSTOP = $2CD1
  (PAL) or $F4D1 (NTSC): the horizontal values widen the window, and the
  vertical stop keeps the normal value for each standard (p.59: "Normally,
  the VSTOP is set to ($F4) for NTSC, ($2C) for PAL").
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
- With parity coverage (RF2): H = 0 occurs in the trace (in any sample).
- Without parity coverage: the lowest value of the observed parity occurs,
  $00 if the H values read are even and $01 if they are odd; p.196 gives
  the 68000 one parity ("The 68000 uses only the even-numbered memory access
  cycles"). If the observed parity is odd, count 0 cannot be read, so
  `COP2-putting-together-a-copper-instruction-list.c7` is not graded whole:
  the assertion is made without that id, and the id goes on a record line
  with this reason.
- p.27: "The video beam traverses the screen from position (0,0) in the
  upper left hand corner".
- Quantifier: every complete field of the trace, at least 6.
- The largest V is not graded (section 7, item 1).

**Preconditions.**

- The completion word is written.
- S1, L1, T1, T2 and R1 hold.
- At least 6 complete fields are traced.

**Contrasting cases.**

- Main: select NTSC for the PAL check, or PAL for the NTSC check. p.24 then
  predicts the other count.
- Main: A-LACE (PAL). p.24: "there are also long and short fields
  (interlace only) ... in PAL, 312, then 313 lines" (p.34 agrees: "625 in
  interlaced mode"). A field with 313 distinct values appears, and "every
  field has 312" goes red.
- First trusted sample at V = 0, and H = 0 (or the lowest value of the
  observed parity) occurs: no input exists for which the pages predict a
  field that starts on another line, or a line without its first count.
  These clauses have no contrasting case at this boundary; the normal run
  exercises them.
- T1: A-slow. Lines are missed and T1 fails.
- T2: A-hold. Every sample is trusted (H ≥ $20), V never changes and H never
  increases, so T2 fails while T1 still holds.
- R1: A-noCop. Fields after the third have no restart and R1 fails.
- Completion word: leave out step 7.

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
- L(v) = time(m_{v+1}) - time(m_v) for v = 256 ... 310 (PAL) or 256 ... 260
  (NTSC): pairs of markers after the wrap, all at $60, on consecutive lines
  of one field. m_262 (NTSC) lies on no line of its field (P8) and is not
  used.
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

- Every `WAIT(v,$40)`, and every `WAIT(n,$60)` for n = 0 ... $37 (PAL) or
  0 ... 5 (NTSC), waited.
- A-anchor: every m_v lies on line v (RF2).
- S1, L1, T1, T2, R1.

**Contrasting cases.**

- Main: select NTSC for the PAL check. L alternates, so "every L = 227" is
  red. Select PAL for the NTSC check. No alternation, so it is red.
- Main, the values 227 and 228: A-shift. m_$80 comes 2 slots later, so
  L($7F) is 229 or 230 and L($80) is 225 or 226 (PAL: 229 and 225), outside
  both expected sets, while m_$80 still lies on line $80 and its WAIT still
  waited.
- "waited": insert `WAIT($7F,$40)` directly after `MARK($7F)`. It is
  fetched after its target has passed, so its delay equals λ_min.
- A-anchor: variant A-anchor. The markers lie on line v + 1.
- T1, T2, R1: as in P1.

**Reference frame.** RF1. RF2 identifies lines, anchored by A-anchor.

---

### P3. The horizontal beam count runs 0 to $E2 on PAL lines; NTSC long lines reach $E3, and only they do

**Claims.** `COP2-horizontal-beam-position.c13` (p.23).

**Not claimed here.** The line lengths of `COP2-horizontal-beam-position.c12`
and `COP2-horizontal-beam-position.c15` are graded, and claimed, by P2. The
PAL range below agrees with p.27's x = 226, whose claims are declined
(section 1).

**Stimulus.** Run A (PAL and NTSC).

**Observation.**

- The H values of all trace samples. The H value is read together with V7-V0
  in one read, so every sample may be used.
- For NTSC, the line type of each sample on a line v whose length L(v) P2
  measured.

**Expected.**

The form graded depends on parity coverage (RF2).

With parity coverage:

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

Without parity coverage, where the H values read have one parity only
(p.196 gives the 68000 one parity: "The 68000 uses only the even-numbered
memory access cycles"; section 7, item 15):

- PAL: every H is at most $E2, and every value of the observed parity is
  present, from the lowest ($00 or $01) to the highest not above $E2 ($E2
  or $E1).
- NTSC: every H is at most $E3, and every value of the observed parity is
  present, from the lowest ($00 or $01) to the highest not above $E3 ($E2
  or $E3). If the observed parity is odd, no sample with H = $E3 lies on a
  line with L(v) = 227. If it is even, $E3 cannot be read and that clause
  is not graded.
- One end of "0-$E3" cannot be read in this form ($00 is even, $E3 is
  odd), so `COP2-horizontal-beam-position.c13` is not graded whole: the
  assertion is made without the id, and the id goes on a record line with
  this reason.

**Preconditions.**

- S1, L1, T1 and T2.
- NTSC coverage, for "no $E3 on a line with L(v) = 227": H ≥ $E0 is
  observed on at least 20 distinct lines with L(v) = 228 and on at least 20
  distinct lines with L(v) = 227. This shows that lines of both types were
  sampled near their end, so "only long lines reach $E3" is not green
  merely because short lines were never sampled there. Samples fall on
  shifting positions because lines are 227 or 228 slots long, and the CPU
  uses the even memory cycles (p.196).
- PAL needs no coverage precondition. The expected set itself requires every
  value up to $E2 (of the observed parity, without parity coverage) to be
  seen, so sampling that never reaches the end of a line turns the check
  red, not green.
- P2's preconditions, for the NTSC line types.

**Contrasting cases.**

- Main, with parity coverage: the PAL check under NTSC ($E3 appears: red).
  The NTSC check under PAL ($E3 absent: red).
- Main, without parity coverage:
  - PAL check under NTSC: red when the observed parity is odd, as $E3
    appears. When it is even, no input exists for which the pages predict
    an even value above $E2; the bound then has no contrasting case at this
    boundary.
  - NTSC check under PAL: red when the observed parity is odd, as $E3 is
    absent. When it is even, both standards give the same even values, and
    the check has no contrasting case at this boundary.
- Every value present, in both forms: A-short (PAL). The trace ends after
  64 samples, about four lines, so most values are never read, while S1,
  L1, T1 and T2 still hold.
- "$E3 only on lines with L(v) = 228": no input exists for which the pages
  predict $E3 on a short line; this clause has no contrasting case at this
  boundary.
- NTSC coverage: select PAL. No line has L(v) = 228, so the coverage
  precondition fails.

**Reference frame.** RF2 values, with P2's line lengths.

---

### P4. The Copper's horizontal comparison advances one count per memory cycle, gives 113 distinct positions $02 ... $E2, and has no position beyond $E2

**Claims.** `COP2-horizontal-beam-position.c3` (p.23);
`COP2-vertical-beam-position.c23` (p.24).

**Not claimed here.** p.275's 280 ns is used below only as a reason; its
claim, `COP2-copins-timing.c6`, is declined (section 1).

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

(b) For h_k in $D2 ... $E2 (k = 105 ... 113): D_k strictly increases with
k.

Near the end of a line the pages leave the Copper's fetch slots open
(section 7, item 14). A 227-count line ends at $E2 and the next begins at
$00, so the slot parity breaks there, and a fetch that would fall past the
end of the line moves by one slot:

- Reading A, p.20 taken literally in count numbering: the Copper "requests
  the bus only during odd-numbered memory cycles", so the two slots $E2 and
  $00 in a row are both closed to it and the fetch comes one slot later:
  D_k - (h_k - $10) is 0 or 1.
- Reading B, Figure 6-9's numbering: refresh, disk, audio and sprite slots
  are drawn on odd counts, and p.20 says the Copper's cycles do not collide
  with them ("This prevents collision with audio, disk, refresh, sprites"),
  so the Copper uses even counts, $E2 and $00 are both open to it, and the
  fetch comes one slot earlier: D_k - (h_k - $10) is -1 or 0.

Neither band is graded. D_k - (h_k - $10) for k = 105 ... 113 is recorded
as a measurement, per core, with both readings named. The pages also do not
say how many slots after the end of a wait the next fetch comes, so they do
not say which of these positions are affected. The strict ordering holds
under both readings, because consecutive positions are 2 counts apart and
the shift is at most one slot. The spacing of 2 counts per position, and
the 113 distinct values $02 ... $E2, are graded exactly through (a) and
through the strict ordering of (b). p.32 confirms the upper end: "Position
$E2 is horizontal position 113 for the Copper and the last real horizontal
position available."

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
- S1, L1, T1, T2, R1.

**Contrasting cases.**

- (a): C-shift. D_50 changes by 2 slots.
- (b): C-shift-end. mark_106 waits for $D6, the position of k = 107, so
  D_106 is not below D_107 and the strict ordering is red.
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
- Quantifier: every position from $36 to $D0 is an edge position, and every
  single step between neighbouring positions in that range is covered.
  Sub-sweep 1 gives w for consecutive n at the second edge; sub-sweep 2 does
  the same at the first edge. Positions in $0F-$35 fall in the horizontal
  blanking (p.23: "Horizontal blanking falls in the range of $0F to $35")
  and cannot be seen. Positions after $D0 are not graded for the reason in
  P4(b). Positions $00-$0E are not graded, because a width that crosses the
  counter's line boundary depends on where the displayed line begins
  relative to the counter line, which no page states.

**Preconditions.**

- Calibration:
  - D-lo: each line inside the window shows 160 COLOR01 runs, and all
    COLOR01 runs and the 159 COLOR00 runs between them have one common
    length.
  - D-hi: 320 COLOR01 runs, likewise of one common length.
- Edges present: A_u appears for every sweep line u, and every output line
  that shows A_u shows exactly one run of it, with B before and after it on
  that output line.
- Every sweep WAIT waited.
- The completion word is written; S1, L1 and R1 hold.

**Contrasting cases.**

- Main: D-shift. w on line $50 becomes 4 · 25 · P_lo while n = 24.
- Calibration: D-flat, for D-lo and for D-hi. There are no alternating runs.
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
- for j = 0, time(x_0) - time(a_0), recorded, not graded.

**Expected.**

- a_j lies on line 255 in every field. p.23: "When waiting for $FFFE, the
  line $FF will be reached".
- j = 0 (h = $E2): x_0 is fetched in the same field, after a_0. p.23: "The
  horizontal position has a maximum value of $E2. This means that the
  largest number that will ever appear in the comparison is $FFE2."
- time(x_0) - time(a_0) is not graded. It is recorded as a measurement, per
  core, against both readings of P4(b): $E2 - $40 = $A2, or $A3, under
  reading A; $A1 or $A2 under reading B (section 7, item 14).
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
- S1, L1, T1, T2, R1.

**Contrasting cases.**

- "a_j lies on line 255": B-254. a_2 lies on line 254, while the
  preconditions still hold for L_2.
- Main: B-FE. `WAIT($FE,$FE)` ends when line $FF begins, so its marker is
  fetched, and time($B301) - time($B300) = $40. p.23: "If the vertical
  position is not $FF, then as soon as the line number becomes higher than
  the entered number, the comparison will evaluate to true and the wait
  will end."
- "$E2 reached": give L_0 h = $E4. x_0 is not fetched.
- "fetched on line 255": B-late. The WAIT for h_1 is fetched on line 256,
  after the padding.
- Chain: point L_3's COP1LC at L_5. The restart after L_3 fetches L_5, not
  L_4.
- "waited", T1, T2, R1: as in P2 and P1.

**Reference frame.** RF2 and RF3. Restarts close the observation interval.

---

### P7. In the Copper's comparison, line 256 appears as zero, and line 256+n as n; the beam counter itself does not wrap

**Claims.** `COP2-the-wait-instruction.c22`, `COP2-the-wait-instruction.c23` (p.23);
`COP2-vertical-beam-position.c7` (p.24).

**Stimulus.** Run A (PAL and NTSC). After `WAIT($FF,$E2)` and 16 padding
moves, `WAIT(n,$60)` with `MARK($0100+n)`, for n = 0 ... N.

**Observation.**

- The line of each marker m_(256+n) (RF2), and the words of the two samples
  that place it (s⁻ and s⁺ of RF2).
- The delay of `WAIT($00,$60)`.
- Every complete field.

**Expected.**

- `WAIT($00,$60)` waited.
- Every m_(256+n) lies on line 256 + n: n = 0 ... 5 in NTSC, n = 0 ... $37
  (lines 256 ... 311) in PAL.
- The samples that place m_(256+n) have V8 = 1 and V7-V0 = n: the beam
  counter reads 256 + n where the comparison sees n.
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
- A-anchor, S1, L1, T1, T2, R1.

**Contrasting cases.**

- Main: A-noprep. `WAIT($00,$60)` is fetched on line 255 right after
  `m_$FF`. It ends at once (p.23, quoted in P6), and m_256 lies on line 255.
- "fetched on line 256": the same variant A-noprep. The WAIT's second word
  is fetched on line 255.
- A-anchor, T1, T2, R1: as in P2 and P1.

**Reference frame.** RF2, anchored by A-anchor.

---

### P8. NTSC: after line 255, vertical positions 0 through 5 are the last six lines of the field before vertical blanking, 256 to 261; there is no line 262

**Claims.** `COP2-vertical-beam-position.c9` (p.24).

**Not claimed here.** `COP2-vertical-beam-position.c10`, Copper execution
on all 262 lines, is not graded whole; it is declined (section 1).

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
- Not graded, and the reason `COP2-vertical-beam-position.c10` is declined
  (section 1):
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

**Claims.** `COP2-horizontal-beam-position.c7`, `COP2-horizontal-beam-position.c9`;
`ch2-horizontal-beam-position.c8` (p.23), all graded by (a).

**Not claimed here.** The claims of the positions "$04 to $47"
(`COP2-horizontal-beam-position.c8`, `ch2-horizontal-beam-position.c7`) are
declined (section 1). (b) grades only the order that p.23's two ranges
imply, and carries no claim.

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
- The completion word is written; L1 and R1 hold.

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
14. **The Copper's slot parity.**
    - p.20: "The Copper is a two-cycle processor that requests the bus only
      during odd-numbered memory cycles. This prevents collision with audio,
      disk, refresh, sprites, and most low resolution display DMA access,
      all of which use only the even-numbered memory cycles."
    - Figure 6-9 (p.195) numbers the slots of a line by count and draws the
      refresh slots at $01, $03 and $05 and the disk slots after them on odd
      counts.
    - Read in count numbering, p.20's "odd-numbered" gives the Copper odd
      counts (reading A); read with Figure 6-9's allocation and p.20's
      "prevents collision", it gives the Copper even counts (reading B).
    - Specified: only what both readings agree on, P4(b)'s ordering. The
      offsets near the end of a line, P4(b) and P6's time(x_0) - time(a_0),
      are recorded against both readings.
15. **The count a CPU read returns.**
    - p.196: "The 68000 uses only the even-numbered memory access cycles",
      and Figure 6-9 numbers the cycles along each line; no page says which
      count a read of VHPOSR returns.
    - If the reads fall on counts of one parity only, half the counter's
      values cannot be read. Specified: P1 and P3 grade the whole range when
      both parities are read, and the values of the one parity read
      otherwise.
16. **BEAMCON0 after reset.** p.305 lists its bits, among them "LOLDIS
    Disable long line/short line toggle", "VARBEAMEN Variable beam counter
    comparator enable" and "PAL Programmable pal mode enable", and says "Do
    not write to this register"; no page gives its value after reset.
    Specified: it is not written, and the ECS runs rest on the assumption
    stated in 3.1, checked against the OCS runs.

## 8. Build

This section records what the build fixes where the specification leaves a
choice, the programs, the final run of each core and the mutations. The
propositions and expected values of sections 1 to 7 are unchanged; where the
build grades less than a section states, 8.4 says so.

### 8.1 Programs

Every program is assembled for the 68000 with vasm's default CPU. The common
prologue (steps 1 and 2 of 3.4, and the fixture constant of 8.2) is the macro
`FRAME_INIT` in `copbeam.i`. A variant wrapper `prog_<run>_<variant>[_ntsc].s`
sets one symbol and includes the run's program; `NMH_MUTATE=<value>` makes the
test run the variant named in 8.6 in place of the normal run. A normal run
(`make test`) runs no variant.

| Source | Run | Contents |
|---|---|---|
| `prog_a.s` | A | List A as written in 5, with the changes of 8.2 items 3 and 4; 50000 copies of `move.l VPOSR(a0),(a6)+` |
| `prog_b.s` | B | Lists L_init, L_FE and L_0 ... L_15; 130 blocks of 1000 copies of the same step, one `dbra` between blocks |
| `prog_c.s` | C | List C; 30000 copies of the same step |
| `prog_d.s` | D, E | The plane data, the list of the run, the display registers; six field boundaries counted by polling VPOSR and VHPOSR |
| `prog_<run>_ntsc.s` | A, B, C, D, E | The same program, run with the NTSC harness configuration |

### 8.2 Choices the specification leaves open

1. **Standard and chip set.** `chipset_config` is $08 (ECS, PAL) and $0A (ECS,
   NTSC); the OCS runs of Runs A, B and C use $00 and $02. The test never
   assumes which standard a run has: `PRE-STD` reads the chip identification
   (VPOSR bits 14-8, 3.1) of every sample, or the word stored at RESULTS in
   Runs D and E, and prints FAIL when it does not name the standard of the
   line. Both cores identify as $20 (PAL) and $30 (NTSC).
2. **Fixture.** The program stores the known word $A5C3 to $0C0002 before
   anything else is read. `FIX-CONST` asserts that the Chip-memory port logs
   that write with that data. `FIX-LOAD` asserts that the program loaded and
   the CPU was released. A failed fixture line makes every dependent line
   NOT GRADED.
3. **Run A: reaching line 256.** The list of Run A waits for ($FF,$E0), one
   of the 113 positions of p.23, where section 5 gives ($FF,$E2): the marker is
   `MARK($A0E0)`, and `PRE-E0` asserts that it is fetched in every field.
   Run B measures the WAIT for ($FF,$E2) itself (P6, j = 0). The 16 padding
   moves follow, and `PRE-P7L256` asserts that the second word of the WAIT
   before m_256 is fetched on line 256 (RF2).
4. **Run A, NTSC: the instruction after the last WAIT.** The marker m_262
   follows one further instruction, `MOVE($1FE,$0000)`, so that the first
   word of the instruction after a pending WAIT, which P6 measures as fetched
   shortly before the next restart on both cores, is not taken for the end of
   the WAIT for line 262.
5. **Samples.** Runs A and C read the long word at $DFF004 in a flat run of
   copies (50000 and 30000). Run B needs about 130000 samples; the copies
   would reach the address of RESULTS, so it runs 130 blocks of 1000 copies
   with one `dbra` between blocks.
6. **Run B.** L_FE, the field of B-FE, is part of every run: L_init names
   L_FE, L_FE names L_0. The markers of L_15 are those of L_14, as section 5 states.
   `L1` of Run B reads the first two words of L_init. B-late waits for
   ($FF,$E0), as item 3.
7. **Run C, C-next.** The wait of q_5 is `WAIT(w_5+1,$50)` where section 5 gives
   `WAIT(w_5+1,$20)`: with $50 the WAIT of r2_5 is fetched after its target,
   which `PRE-C-WAITED` and `P4-C` assert it is not.
8. **Runs D and E.** The CPU writes the colour registers and DIWSTRT,
   DIWSTOP, DDFSTRT, DDFSTOP, BPLCON0, BPL1MOD (3.4 step 4); the list holds
   the two bitplane pointer moves and END. The window lines of a field are the
   output lines that show COLOR01. Output pixel clocks are the video samples
   of the harness (one per `clk_sys` cycle); P_lo and P_hi are measured from
   the calibration runs in the same standard.
9. **Slot length.** S is measured in every capture of both cores from the
   refresh reads (RF1) and is 8 `clk_sys` cycles per slot in all of them. The
   refresh reads are taken from the harness's uncoalesced port log.
10. **A-slow.** The delay between samples is a loop of 100 `dbra`
    iterations, at least 300 slots (3.2); 400 samples are taken.

### 8.3 Preconditions beyond the specification's list

| Id | What it asserts | Why |
|---|---|---|
| `PRE-T1E` | no line identification of this run is bracketed by a step that ended before the event | the part of T1 that RF2's line identification rests on; `PRE-T1` itself, as section 4 states it (every step at most 16 slots), is printed and stays red |
| `PRE-E0`, `PRE-P7L256` | m_E0 is fetched in every field; the WAIT before m_256 is fetched on line 256 | guard the stimulus of 8.2 item 3 |
| `PRE-P3ODDEND`, `PRE-P3COVER` | the odd count $E1 is read; line-end coverage of both NTSC line types | P3 (8.4 item 2) |
| `PRE-C-R1LINE`, `PRE-C-WAITED` | r1_j lies on line w_j; every WAIT under test waited | P4 as section 6 states |
| `PRE-B-CHAIN`, `PRE-B-L255`, `PRE-B-WAITED` | each restart fetches the list the previous one named, at least 3 fields of L_15; the WAIT of each list is fetched on line 255; every WAIT($FF,$40) waited | P6 as section 6 states |
| `PRE-P5-CALLO`, `PRE-P5-CALHI`, `PRE-P5-EDGES`, `PRE-P5-WAITED` | the calibration and edge preconditions of P5 | P5 as section 6 states |
| `PRE-P9-PLO`, `PRE-P9-LINES`, `PRE-P9-ONE0` | P_lo valid; at least one window line per field; exactly one run of 0 per window line | P9 as section 6 states |

The common preconditions are printed once per capture, with the capture's run
named after the standard (`PRE-DONE PAL run A: ...`); they carry the same id in
every run, so one mutation record names the id.

`PRE-T1` is red in every trace capture of both cores. The steps that exceed 16
slots are listed by the test (`T1 ... gaps exceed 16 slots, N of them across a
field boundary`): the step that spans the Copper's restart at a field boundary
lasts about 19 to 20 slots, and Run C and Run B have further long steps while
the Copper fetches. Without Copper DMA no step exceeds 16 slots (8.6, `A-COPOFF-PAL`: `PRE-T1` passes).

### 8.4 What the build grades differently from sections 1 to 7

1. **P1, the number of lines.** Not graded. The pages give two field lengths.
   p.24: "There are actually 262 NTSC (312 PAL) possible vertical positions."
   p.34: "Whereas a normal NTSC display has 262 lines" and "PAL has 312 lines
   normally". p.27 places the end of the display at "(226,262) NTSC (or (226,312)
   PAL)" from "(0,0)", which are 263 and 313 positions. p.216, Table 7-5, gives
   PAL only: V8 "Allows PAL line counts (313) to appear in PAL versions of the
   Amiga." The test prints the measured number of distinct V values per complete
   field on a `MEASURED:` line and the quotations on a `RECORDED: P1-COUNT ...
   NOT GRADED` line that names `COP2-vertical-beam-position.c3`,
   `COP2-vertical-beam-position.c4`, `COP2-using-the-copper-in-interlaced-mode.c2`
   and `COP2-using-the-copper-in-interlaced-mode.c4`. The contrast A-LACE is not
   built: a field of 313 PAL lines is no contrast while the non-interlaced field
   already has 313. `P1-ORIGIN` is graded as section 6 states.
2. **P3, the range of the count.** Section 6 grades the whole set of counts when both
   parities are read. Both parities are read on both cores, but the sampler
   never reads some odd counts near the start of the line (the lists of 8.5);
   every even count is read. The whole set is therefore not something the
   sampler can establish, whatever the chip set does. P3 grades what it can:
   every H at most $E2 (PAL) or $E3 (NTSC), every even value from $00 to $E2,
   and for NTSC the odd value $E3, which is graded only when `PRE-P3ODDEND`
   shows that odd counts at the end of a line are read (the odd value $E1 occurs).
   The odd values the sampler does not read are measured, not graded.
   `COP2-horizontal-beam-position.c13` is not asserted whole and is declined on a
   `RECORDED:` line. "$E3 only on long lines" is graded when `PRE-P3COVER` holds;
   it never holds on either core.
3. **P2, the contrast "select NTSC for the PAL check".** Not built: the lines
   of the NTSC capture are all 227 slots, so `P2-PAL` stays green on it. The
   mutation of `P2-PAL` is A-shift.
4. **P4 (c).** `E_j = $40` is not graded. Where the next fetch falls in the
   first counts of a line is left open by the pages (p.20: the Copper's cycles
   do not collide with refresh, which occupies slots at the start of every line;
   7, item 14), and E_j compares a fetch at the start of a line with a fetch at
   count $40. `P4-C` grades that r2_j lies on line w_j + 1 and that the WAIT of
   r2_j waited, which is what the page predicts: the WAIT for a position beyond
   $E2 on line w_j ended before count $40 of line w_j + 1. E_j is measured per j.
5. **P6.** `time(x_0) - time(a_0)` is recorded and not graded, as section 6 states,
   together with the line of x_0 and, for h_j above $E2, the number of slots
   between the fetch of x_j and the next restart and the number of fetches after
   the second word of the WAIT. The contrast B-FE is a control line,
   `P6-CONTROL`, that grades that $B300 is fetched in the same field and that
   the WAIT($FF,$40) after it waited; `time($B301) - time($B300) = $40` is not
   graded, for the reason of item 4, and is measured.
6. **P7 and P8, the contrasts.** A-noprep puts the WAIT($00,$60) on line 255, so
   `PRE-E0` and `PRE-P7L256` fail and `P7` is NOT GRADED: a precondition never
   depends on the clause under test, and a failed precondition makes its
   dependent line NOT GRADED. `P7` is mutated by A-tailshift, with every
   precondition holding, and `P7-WAITED` by A-longpad. `P8` has no mutation of
   its own, because `P8-AFTER` is red on both cores and no input of this package
   passes it: `P8-LINES` is mutated by A-tailshift on the NTSC list and
   `P8-NO262` by A-tailpast, a green control.
7. **P8 and "six or seven".** p.24 says of the end of the field: "Some minor
   complications can occur if you want something to happen within these last six
   or seven scan lines." P8 grades the box's "256 + 6 = 262 lines": the lines
   256 to 261 and no line 262. The measured NTSC field has a line 262 (the line
   after 261 is 262, 8.5), the same count that P1 leaves ungraded.

### 8.5 The final run of each core

`make -k -B test`: fx68k 224 passed, 16 failed; TG68K 224 passed, 16 failed. The verdicts of every graded line are the same on both cores. `PRE-T1` is the specification's precondition as section 4 states it; the dependent lines are gated by `PRE-T1E` (8.3). The table counts the lines of each label over all runs and standards; a failing line names its standard and run.

| label | fx68k | TG68K |
|---|---|---|
| `FIX-CONST` | 16 PASS | 16 PASS |
| `FIX-LOAD` | 16 PASS | 16 PASS |
| `P1-ORIGIN` | 2 PASS | 2 PASS |
| `P2-NTSC` | 0 PASS, 1 FAIL (NTSC) | 0 PASS, 1 FAIL (NTSC) |
| `P2-PAL` | 1 PASS | 1 PASS |
| `P3-NTSC-SET` | 0 PASS, 1 FAIL (NTSC) | 0 PASS, 1 FAIL (NTSC) |
| `P3-PAL` | 1 PASS | 1 PASS |
| `P4` | 2 PASS | 2 PASS |
| `P4-A` | 2 PASS | 2 PASS |
| `P4-B` | 2 PASS | 2 PASS |
| `P4-C` | 2 PASS | 2 PASS |
| `P5-HI` | 2 PASS | 2 PASS |
| `P5-LO` | 2 PASS | 2 PASS |
| `P6-CONTROL` | 2 PASS | 2 PASS |
| `P6-LARGEST` | 0 PASS, 2 FAIL (PAL; NTSC) | 0 PASS, 2 FAIL (PAL; NTSC) |
| `P6-LINE255` | 2 PASS | 2 PASS |
| `P6-NEVER` | 0 PASS, 2 FAIL (PAL; NTSC) | 0 PASS, 2 FAIL (PAL; NTSC) |
| `P7` | 2 PASS | 2 PASS |
| `P7-WAITED` | 2 PASS | 2 PASS |
| `P8` | 0 PASS, 1 FAIL (NTSC) | 0 PASS, 1 FAIL (NTSC) |
| `P8-AFTER` | 0 PASS, 1 FAIL (NTSC) | 0 PASS, 1 FAIL (NTSC) |
| `P8-LINES` | 1 PASS | 1 PASS |
| `P8-NO262` | 0 PASS, 1 FAIL (NTSC) | 0 PASS, 1 FAIL (NTSC) |
| `P9-A` | 2 PASS | 2 PASS |
| `P9-A-E` | 2 PASS | 2 PASS |
| `P9-A-EW` | 2 PASS | 2 PASS |
| `P9-B` | 2 PASS | 2 PASS |
| `PRE-ANCHOR` | 2 PASS | 2 PASS |
| `PRE-B-CHAIN` | 2 PASS | 2 PASS |
| `PRE-B-L255` | 2 PASS | 2 PASS |
| `PRE-B-WAITED` | 2 PASS | 2 PASS |
| `PRE-C-R1LINE` | 2 PASS | 2 PASS |
| `PRE-C-WAITED` | 2 PASS | 2 PASS |
| `PRE-DONE` | 16 PASS | 16 PASS |
| `PRE-E0` | 2 PASS | 2 PASS |
| `PRE-FIELDS` | 16 PASS | 16 PASS |
| `PRE-L1` | 16 PASS | 16 PASS |
| `PRE-P2WAITED` | 2 PASS | 2 PASS |
| `PRE-P3COVER` | 0 PASS, 1 FAIL (NTSC A) | 0 PASS, 1 FAIL (NTSC A) |
| `PRE-P3ODDEND` | 1 PASS | 1 PASS |
| `PRE-P5-CALHI` | 2 PASS | 2 PASS |
| `PRE-P5-CALLO` | 2 PASS | 2 PASS |
| `PRE-P5-EDGES` | 2 PASS | 2 PASS |
| `PRE-P5-WAITED` | 2 PASS | 2 PASS |
| `PRE-P7L256` | 2 PASS | 2 PASS |
| `PRE-P9-LINES` | 2 PASS | 2 PASS |
| `PRE-P9-ONE0` | 4 PASS | 4 PASS |
| `PRE-P9-PLO` | 2 PASS | 2 PASS |
| `PRE-R1` | 16 PASS | 16 PASS |
| `PRE-S1A` | 16 PASS | 16 PASS |
| `PRE-S1B` | 16 PASS | 16 PASS |
| `PRE-STD` | 16 PASS | 16 PASS |
| `PRE-T1` | 0 PASS, 6 FAIL (PAL A; NTSC A; PAL C; NTSC C; PAL B; NTSC B) | 0 PASS, 6 FAIL (PAL A; NTSC A; PAL C; NTSC C; PAL B; NTSC B) |
| `PRE-T1E` | 6 PASS | 6 PASS |
| `PRE-T2` | 6 PASS | 6 PASS |

```text
CORE fx68k
MEASURED: T1 PAL run A: 17 of 49999 consecutive-sample gaps exceed 16 slots, 10 of them across a field boundary
MEASURED: T2 PAL run A: 0 of 46533 trusted-sample transitions violate T2
MEASURED: R1 PAL run A: 0 of 7 inter-boundary spans have restart count != 1
MEASURED: P1-COUNT PAL run A: distinct V values per complete field {313} over 7 complete fields
MEASURED: P1-ORIGIN PAL run A: parity coverage yes (even seen, odd seen)
MEASURED: P3 PAL run A: H values observed 00..E2, 208 distinct; even values not observed (0): none; odd values up to $E1 not observed (19): 05 07 09 0B 0D 11 15 19 1D 21 25 29 2D 31 35 39 3D 41 45; lines with H>=$E0 and L=228: 0, with L=227: 303
MEASURED: OCS-CHECK PAL run A: ECS field-length set {313} vs OCS {313} -- match, assumption checked
MEASURED: T1 NTSC run A: 20 of 49999 consecutive-sample gaps exceed 16 slots, 10 of them across a field boundary
MEASURED: T2 NTSC run A: 0 of 46566 trusted-sample transitions violate T2
MEASURED: R1 NTSC run A: 0 of 9 inter-boundary spans have restart count != 1
MEASURED: P1-COUNT NTSC run A: distinct V values per complete field {263} over 9 complete fields
MEASURED: P1-ORIGIN NTSC run A: parity coverage yes (even seen, odd seen)
MEASURED: P3 NTSC run A: H values observed 00..E2, 203 distinct; even values not observed (0): none; odd values up to $E1 not observed (24): 05 07 09 0B 0D 0F 11 15 17 19 1D 21 23 25 29 2D 2F 31 35 39 3B 3D 41 45; lines with H>=$E0 and L=228: 0, with L=227: 315
MEASURED: P8 NTSC run A: the trusted sample after the last sample of line 261 has V (count): 262x9 over 9 field boundaries
MEASURED: OCS-CHECK NTSC run A: ECS field-length set {263} vs OCS {263} -- match, assumption checked
MEASURED: T1 PAL run C: 96 of 29999 consecutive-sample gaps exceed 16 slots, 4 of them across a field boundary
MEASURED: T2 PAL run C: 0 of 28053 trusted-sample transitions violate T2
MEASURED: R1 PAL run C: 0 of 4 inter-boundary spans have restart count != 1
MEASURED: P4 PAL run C: D_k - (h_k - $10) for h_k = $D2..$E2 (reading A predicts 0 or 1, reading B -1 or 0): k=105:0 k=106:0 k=107:0 k=108:0 k=109:0 k=110:0 k=111:0 k=112:0 k=113:3
MEASURED: P4-C PAL run C: E_j and the line of r2_j per j: j=0(w=$A4, expected line 165):{E=62 line 165} j=1(w=$A6, expected line 167):{E=62 line 167} j=2(w=$A8, expected line 169):{E=62 line 169} j=3(w=$AA, expected line 171):{E=62 line 171} j=4(w=$AC, expected line 173):{E=62 line 173} j=5(w=$AE, expected line 175):{E=62 line 175} j=6(w=$B0, expected line 177):{E=62 line 177} j=7(w=$B2, expected line 179):{E=62 line 179} j=8(w=$B4, expected line 181):{E=62 line 181} j=9(w=$B6, expected line 183):{E=62 line 183} j=10(w=$B8, expected line 185):{E=62 line 185} j=11(w=$BA, expected line 187):{E=62 line 187} j=12(w=$BC, expected line 189):{E=62 line 189} j=13(w=$BE, expected line 191):{E=62 line 191}
MEASURED: OCS-CHECK PAL run C: ECS field-length set {313} vs OCS {313} -- match, assumption checked
MEASURED: T1 NTSC run C: 115 of 29999 consecutive-sample gaps exceed 16 slots, 6 of them across a field boundary
MEASURED: T2 NTSC run C: 0 of 28037 trusted-sample transitions violate T2
MEASURED: R1 NTSC run C: 0 of 5 inter-boundary spans have restart count != 1
MEASURED: P4 NTSC run C: D_k - (h_k - $10) for h_k = $D2..$E2 (reading A predicts 0 or 1, reading B -1 or 0): k=105:0 k=106:0 k=107:0 k=108:0 k=109:0 k=110:0 k=111:0 k=112:0 k=113:3
MEASURED: P4-C NTSC run C: E_j and the line of r2_j per j: j=0(w=$A4, expected line 165):{E=62 line 165} j=1(w=$A6, expected line 167):{E=62 line 167} j=2(w=$A8, expected line 169):{E=62 line 169} j=3(w=$AA, expected line 171):{E=62 line 171} j=4(w=$AC, expected line 173):{E=62 line 173} j=5(w=$AE, expected line 175):{E=62 line 175} j=6(w=$B0, expected line 177):{E=62 line 177} j=7(w=$B2, expected line 179):{E=62 line 179} j=8(w=$B4, expected line 181):{E=62 line 181} j=9(w=$B6, expected line 183):{E=62 line 183} j=10(w=$B8, expected line 185):{E=62 line 185} j=11(w=$BA, expected line 187):{E=62 line 187} j=12(w=$BC, expected line 189):{E=62 line 189} j=13(w=$BE, expected line 191):{E=62 line 191}
MEASURED: OCS-CHECK NTSC run C: ECS field-length set {263} vs OCS {263} -- match, assumption checked
MEASURED: T1 PAL run B: 151 of 129999 consecutive-sample gaps exceed 16 slots, 18 of them across a field boundary
MEASURED: T2 PAL run B: 0 of 121914 trusted-sample transitions violate T2
MEASURED: R1 PAL run B: 0 of 21 inter-boundary spans have restart count != 1
MEASURED: P6 PAL run B: j=0: x_0 fetched in the same field, time(x_0)-time(a_0) = 13102 slots, on a line the trace does not identify (reading A predicts $A2 or $A3 = 162 or 163, reading B $A1 or $A2 = 161 or 162, p.23 and P4(b)); fields of h_j > $E2: 19 of 19 fields: x_j fetched 2 slots before the next restart, 1 Copper fetches after the WAIT's second word;
MEASURED: OCS-CHECK PAL run B: ECS field-length set {313} vs OCS {313} -- match, assumption checked
MEASURED: T1 NTSC run B: 156 of 129999 consecutive-sample gaps exceed 16 slots, 11 of them across a field boundary
MEASURED: T2 NTSC run B: 0 of 121909 trusted-sample transitions violate T2
MEASURED: R1 NTSC run B: 0 of 25 inter-boundary spans have restart count != 1
MEASURED: P6 NTSC run B: j=0: x_0 fetched in the same field, time(x_0)-time(a_0) = 1752 slots, on a line the trace does not identify (reading A predicts $A2 or $A3 = 162 or 163, reading B $A1 or $A2 = 161 or 162, p.23 and P4(b)); fields of h_j > $E2: 23 of 23 fields: x_j fetched 2 slots before the next restart, 1 Copper fetches after the WAIT's second word;
MEASURED: OCS-CHECK NTSC run B: ECS field-length set {263} vs OCS {263} -- match, assumption checked
MEASURED: P5 PAL D-lo: 2048 window lines, 0 without 160 COLOR01 runs of one common length with the COLOR00 runs between; lengths {2}
MEASURED: P5 PAL D-hi: 2048 window lines, 0 without 320 COLOR01 runs of one common length with the COLOR00 runs between; lengths {1}
MEASURED: P5 PAL D-sweep: 1120 A_u runs measured; P_lo=2, P_hi=1
MEASURED: P9 PAL E: 2048 window lines, COLOR01 run lengths {640}, P_lo=2
MEASURED: P5 NTSC D-lo: 1600 window lines, 0 without 160 COLOR01 runs of one common length with the COLOR00 runs between; lengths {2}
MEASURED: P5 NTSC D-hi: 1600 window lines, 0 without 320 COLOR01 runs of one common length with the COLOR00 runs between; lengths {1}
MEASURED: P5 NTSC D-sweep: 1120 A_u runs measured; P_lo=2, P_hi=1
MEASURED: P9 NTSC E: 1600 window lines, COLOR01 run lengths {640}, P_lo=2
```

```text
CORE TG68K
MEASURED: T1 PAL run A: 16 of 49999 consecutive-sample gaps exceed 16 slots, 11 of them across a field boundary
MEASURED: T2 PAL run A: 0 of 46531 trusted-sample transitions violate T2
MEASURED: R1 PAL run A: 0 of 7 inter-boundary spans have restart count != 1
MEASURED: P1-COUNT PAL run A: distinct V values per complete field {313} over 7 complete fields
MEASURED: P1-ORIGIN PAL run A: parity coverage yes (even seen, odd seen)
MEASURED: P3 PAL run A: H values observed 00..E2, 208 distinct; even values not observed (0): none; odd values up to $E1 not observed (19): 05 07 09 0B 0D 11 15 19 1D 21 25 29 2D 31 35 39 3D 41 45; lines with H>=$E0 and L=228: 0, with L=227: 304
MEASURED: OCS-CHECK PAL run A: ECS field-length set {313} vs OCS {313} -- match, assumption checked
MEASURED: T1 NTSC run A: 20 of 49999 consecutive-sample gaps exceed 16 slots, 10 of them across a field boundary
MEASURED: T2 NTSC run A: 0 of 46557 trusted-sample transitions violate T2
MEASURED: R1 NTSC run A: 0 of 9 inter-boundary spans have restart count != 1
MEASURED: P1-COUNT NTSC run A: distinct V values per complete field {263} over 9 complete fields
MEASURED: P1-ORIGIN NTSC run A: parity coverage yes (even seen, odd seen)
MEASURED: P3 NTSC run A: H values observed 00..E2, 203 distinct; even values not observed (0): none; odd values up to $E1 not observed (24): 05 07 09 0B 0D 0F 13 15 17 1B 1F 21 23 27 2B 2D 2F 33 37 39 3B 3F 43 45; lines with H>=$E0 and L=228: 0, with L=227: 315
MEASURED: P8 NTSC run A: the trusted sample after the last sample of line 261 has V (count): 262x9 over 9 field boundaries
MEASURED: OCS-CHECK NTSC run A: ECS field-length set {263} vs OCS {263} -- match, assumption checked
MEASURED: T1 PAL run C: 96 of 29999 consecutive-sample gaps exceed 16 slots, 4 of them across a field boundary
MEASURED: T2 PAL run C: 0 of 28052 trusted-sample transitions violate T2
MEASURED: R1 PAL run C: 0 of 4 inter-boundary spans have restart count != 1
MEASURED: P4 PAL run C: D_k - (h_k - $10) for h_k = $D2..$E2 (reading A predicts 0 or 1, reading B -1 or 0): k=105:0 k=106:0 k=107:0 k=108:0 k=109:0 k=110:0 k=111:0 k=112:0 k=113:3
MEASURED: P4-C PAL run C: E_j and the line of r2_j per j: j=0(w=$A4, expected line 165):{E=62 line 165} j=1(w=$A6, expected line 167):{E=62 line 167} j=2(w=$A8, expected line 169):{E=62 line 169} j=3(w=$AA, expected line 171):{E=62 line 171} j=4(w=$AC, expected line 173):{E=62 line 173} j=5(w=$AE, expected line 175):{E=62 line 175} j=6(w=$B0, expected line 177):{E=62 line 177} j=7(w=$B2, expected line 179):{E=62 line 179} j=8(w=$B4, expected line 181):{E=62 line 181} j=9(w=$B6, expected line 183):{E=62 line 183} j=10(w=$B8, expected line 185):{E=62 line 185} j=11(w=$BA, expected line 187):{E=62 line 187} j=12(w=$BC, expected line 189):{E=62 line 189} j=13(w=$BE, expected line 191):{E=62 line 191}
MEASURED: OCS-CHECK PAL run C: ECS field-length set {313} vs OCS {313} -- match, assumption checked
MEASURED: T1 NTSC run C: 115 of 29999 consecutive-sample gaps exceed 16 slots, 6 of them across a field boundary
MEASURED: T2 NTSC run C: 0 of 28040 trusted-sample transitions violate T2
MEASURED: R1 NTSC run C: 0 of 5 inter-boundary spans have restart count != 1
MEASURED: P4 NTSC run C: D_k - (h_k - $10) for h_k = $D2..$E2 (reading A predicts 0 or 1, reading B -1 or 0): k=105:0 k=106:0 k=107:0 k=108:0 k=109:0 k=110:0 k=111:0 k=112:0 k=113:3
MEASURED: P4-C NTSC run C: E_j and the line of r2_j per j: j=0(w=$A4, expected line 165):{E=62 line 165} j=1(w=$A6, expected line 167):{E=62 line 167} j=2(w=$A8, expected line 169):{E=62 line 169} j=3(w=$AA, expected line 171):{E=62 line 171} j=4(w=$AC, expected line 173):{E=62 line 173} j=5(w=$AE, expected line 175):{E=62 line 175} j=6(w=$B0, expected line 177):{E=62 line 177} j=7(w=$B2, expected line 179):{E=62 line 179} j=8(w=$B4, expected line 181):{E=62 line 181} j=9(w=$B6, expected line 183):{E=62 line 183} j=10(w=$B8, expected line 185):{E=62 line 185} j=11(w=$BA, expected line 187):{E=62 line 187} j=12(w=$BC, expected line 189):{E=62 line 189} j=13(w=$BE, expected line 191):{E=62 line 191}
MEASURED: OCS-CHECK NTSC run C: ECS field-length set {263} vs OCS {263} -- match, assumption checked
MEASURED: T1 PAL run B: 152 of 129999 consecutive-sample gaps exceed 16 slots, 19 of them across a field boundary
MEASURED: T2 PAL run B: 0 of 121948 trusted-sample transitions violate T2
MEASURED: R1 PAL run B: 0 of 21 inter-boundary spans have restart count != 1
MEASURED: P6 PAL run B: j=0: x_0 fetched in the same field, time(x_0)-time(a_0) = 13102 slots, on a line the trace does not identify (reading A predicts $A2 or $A3 = 162 or 163, reading B $A1 or $A2 = 161 or 162, p.23 and P4(b)); fields of h_j > $E2: 19 of 19 fields: x_j fetched 2 slots before the next restart, 1 Copper fetches after the WAIT's second word;
MEASURED: OCS-CHECK PAL run B: ECS field-length set {313} vs OCS {313} -- match, assumption checked
MEASURED: T1 NTSC run B: 156 of 129999 consecutive-sample gaps exceed 16 slots, 21 of them across a field boundary
MEASURED: T2 NTSC run B: 0 of 121949 trusted-sample transitions violate T2
MEASURED: R1 NTSC run B: 0 of 25 inter-boundary spans have restart count != 1
MEASURED: P6 NTSC run B: j=0: x_0 fetched in the same field, time(x_0)-time(a_0) = 1752 slots, on a line the trace does not identify (reading A predicts $A2 or $A3 = 162 or 163, reading B $A1 or $A2 = 161 or 162, p.23 and P4(b)); fields of h_j > $E2: 23 of 23 fields: x_j fetched 2 slots before the next restart, 1 Copper fetches after the WAIT's second word;
MEASURED: OCS-CHECK NTSC run B: ECS field-length set {263} vs OCS {263} -- match, assumption checked
MEASURED: P5 PAL D-lo: 2048 window lines, 0 without 160 COLOR01 runs of one common length with the COLOR00 runs between; lengths {2}
MEASURED: P5 PAL D-hi: 2048 window lines, 0 without 320 COLOR01 runs of one common length with the COLOR00 runs between; lengths {1}
MEASURED: P5 PAL D-sweep: 1120 A_u runs measured; P_lo=2, P_hi=1
MEASURED: P9 PAL E: 2048 window lines, COLOR01 run lengths {640}, P_lo=2
MEASURED: P5 NTSC D-lo: 1600 window lines, 0 without 160 COLOR01 runs of one common length with the COLOR00 runs between; lengths {2}
MEASURED: P5 NTSC D-hi: 1600 window lines, 0 without 320 COLOR01 runs of one common length with the COLOR00 runs between; lengths {1}
MEASURED: P5 NTSC D-sweep: 1120 A_u runs measured; P_lo=2, P_hi=1
MEASURED: P9 NTSC E: 1600 window lines, COLOR01 run lengths {640}, P_lo=2
```


### 8.6 Mutations

Each value of `NMH_MUTATE` runs the section that owns the mutated line, with every comparator of that section unchanged; the program or the capture graded is the only difference. The verdict columns give the first line printed under the label on each core. The records are in the header of the test source.

| Label | Record | NMH_MUTATE | fx68k | TG68K | What changes |
|---|---|---|---|---|---|
| `FIX-LOAD` | none impossible | - | - | - | apparatus check: it fails only when the harness cannot load the program or release the CPU, which no stimulus of this package varies (not exercised, by a scope decision) |
| `FIX-CONST` | stimulus red | `A-NOFIX-PAL` | FAIL | FAIL | the program leaves out the store of $A5C3 to $0C0002, so the port logs no such write and the dependent lines are NOT GRADED |
| `PRE-DONE` | stimulus red | `A-NODONE-PAL` | FAIL | FAIL | the program starts the Copper and halts before the trace and before DONE, so the completion word is never written |
| `PRE-S1A` | none impossible | - | - | - | unfalsifiable at this boundary: no input of the programs changes the refresh slots (RF1) |
| `PRE-S1B` | none impossible | - | - | - | unfalsifiable at this boundary: no input of the programs changes the refresh slots, so no two accesses can be made to share one (RF1) |
| `PRE-L1` | stimulus red | `A-NOOVL-PAL` | FAIL | FAIL | CIA-A is not written (step 2 left out), the overlay stays on, the CPU's writes of the list never reach Chip memory and the port logs none; the program still reaches DONE |
| `PRE-T1` | stimulus green | `A-COPOFF-PAL` | PASS | PASS | Copper DMA is never enabled (DMACON = $8200), so no restart interrupts the trace and every step lasts 12 or 13 slots; the unchanged comparator passes where the normal run is red |
| `PRE-T1E` | stimulus red | `A-SLOW-PAL` | FAIL | FAIL | a delay of 100 dbra iterations (at least 300 slots) between 400 samples, so the events fall between samples more than 16 slots apart |
| `PRE-T2` | stimulus red | `A-HOLD-PAL` | FAIL | FAIL | one read of $DFF004 after polling H >= $20 is stored in every step, so H never increases between trusted samples |
| `PRE-R1` | stimulus red | `A-NOCOP-PAL` | FAIL | FAIL | DMACON = $0080 (COPEN cleared) is written after the third field boundary the trace sees, so the later fields have no restart |
| `PRE-FIELDS` | stimulus red | `A-SHORT-PAL` | FAIL | FAIL | the trace buffer holds 64 samples, far fewer than 6 complete fields |
| `PRE-STD` | stimulus red | `A-STD-NTSC-LABEL-PALCAP` | FAIL | FAIL | the PAL capture is graded under the NTSC line, and its chip identification names PAL |
| `PRE-ANCHOR` | stimulus red | `A-ANCHOR-PAL` | FAIL | FAIL | every WAIT(v,$40) becomes WAIT(v+1,$40), so m_v lies on line v+1 |
| `PRE-P2WAITED` | stimulus red | `A-P2WAITED-PAL` | FAIL | FAIL | an extra WAIT($7F,$40) after MARK($7F) is fetched after its target has passed, so its delay equals lambda_min |
| `PRE-E0` | stimulus red | `A-NOPREP-PAL` | FAIL | FAIL | WAIT($FF,$E0), MARK($A0E0) and the padding are left out, so m_E0 does not exist |
| `PRE-P7L256` | stimulus red | `A-NOPREP-PAL` | FAIL | FAIL | WAIT($FF,$E0) and the padding are left out, so the second word of WAIT(0,$60) is fetched on line 255 |
| `P1-ORIGIN` | none impossible | - | - | - | not exercised, by a scope decision: a write to VPOSW or VHPOSW (p.216) would move the counter and the pages would then predict the written position, not (0,0); the package never writes them |
| `P2-PAL` | stimulus red | `A-SHIFT-PAL` | FAIL | FAIL | WAIT($80,$42) in place of WAIT($80,$40), so L($7F) and L($80) differ from 227 |
| `P2-NTSC` | none impossible | - | - | - | already red on both cores, and no input of this package gives a line of 228 slots, so no capture can pass the comparator; neither unfalsifiable nor a scope decision |
| `PRE-P3ODDEND` | stimulus red | `A-SHORT-NTSC` | FAIL | FAIL | the trace buffer holds 64 samples, so the odd count $E1 is not read |
| `PRE-P3COVER` | none impossible | - | - | - | already red on both cores, and no capture of this package has a line of 228 slots, so no input passes the comparator; neither unfalsifiable nor a scope decision |
| `P3-PAL` | stimulus red | `A-SHORT-PAL` | FAIL | FAIL | the trace buffer holds 64 samples, so most even counts are never read |
| `P3-NTSC-SET` | none impossible | - | - | - | already red on both cores, and no capture of this package reads $E3, so no input passes the comparator; neither unfalsifiable nor a scope decision |
| `P3-NTSC-LONGONLY` | none impossible | - | - | - | NOT GRADED on both cores because PRE-P3COVER fails, and no input exists for which the pages predict $E3 on a short line |
| `P7` | stimulus red | `A-TAILSHIFT-PAL` | FAIL | FAIL | every tail WAIT(n,$60) becomes WAIT(n+1,$60), so m_(256+n) lies on line 257+n |
| `P7-WAITED` | stimulus red | `A-LONGPAD-PAL` | FAIL | FAIL | 16 further padding moves put the fetch of WAIT(0,$60) after count $60, so it is already satisfied |
| `P8-LINES` | stimulus red | `A-TAILSHIFT-NTSC` | FAIL | FAIL | every tail WAIT(n,$60) becomes WAIT(n+1,$60), so m_(256+n) lies on line 257+n |
| `P8-AFTER` | none impossible | - | - | - | already red on both cores, and no input of this package changes the length of a field, so no capture has line 0 after line 261; neither unfalsifiable nor a scope decision |
| `P8-NO262` | stimulus green | `A-TAILPAST-NTSC` | PASS | PASS | the last tail WAIT is WAIT(7,$60), a line that no field has, so m_262 is not fetched through the unchanged comparator where the normal run is red |
| `P8` | none impossible | - | - | - | already red on both cores through P8-AFTER, which no input of this package passes; neither unfalsifiable nor a scope decision |
| `PRE-C-R1LINE` | stimulus red | `C-CR1LINE-PAL` | FAIL | FAIL | WAIT(w_5,$C0) becomes WAIT(w_5+1,$C0), so r1_5 lies on line w_5+1 |
| `PRE-C-WAITED` | stimulus red | `C-CLATE-PAL` | FAIL | FAIL | the reference of k = 90 is WAIT(u_90,$C4), so the following WAIT(u_90,$B4) is fetched after its target |
| `P4-A` | stimulus red | `C-CSHIFT-PAL` | FAIL | FAIL | WAIT(u_50,$66) in place of WAIT(u_50,$64), so D_50 changes by 2 slots |
| `P4-B` | stimulus red | `C-CSHIFTEND-PAL` | FAIL | FAIL | WAIT(u_106,$D6) in place of WAIT(u_106,$D4), so D_106 is not below D_107 |
| `P4-C` | stimulus red | `C-CNEXT-PAL` | FAIL | FAIL | q_5 is WAIT(w_5+1,$50), so the WAIT of r2_5 is fetched after its target and did not wait |
| `P4` | stimulus red | `C-CSHIFT-PAL` | FAIL | FAIL | WAIT(u_50,$66) in place of WAIT(u_50,$64), so the aggregate fails through P4-A |
| `PRE-B-CHAIN` | stimulus red | `B-BCHAIN-PAL` | FAIL | FAIL | the COP1LC moves of L_3 name L_5, so the restart after L_3 fetches L_5 and not L_4 |
| `PRE-B-L255` | stimulus red | `B-BLATE-PAL` | FAIL | FAIL | in L_1 a WAIT($FF,$E0) and 16 padding moves precede WAIT($FF,h_1), so its second word is fetched on line 256 |
| `PRE-B-WAITED` | stimulus red | `B-BWAITED-PAL` | FAIL | FAIL | in L_4 a WAIT($FF,$80) precedes WAIT($FF,$40), so the latter is fetched after its target |
| `P6-LINE255` | stimulus red | `B-B254-PAL` | FAIL | FAIL | in L_2 the first WAIT is for line $FE and a WAIT($FF,$30) follows it, so a_2 lies on line 254 while the preconditions still hold |
| `P6-CONTROL` | stimulus red | `B-BFEOFF-PAL` | FAIL | FAIL | L_FE waits for ($FF,$FE), so $B300 is not fetched |
| `P6-LARGEST` | none impossible | - | - | - | already red on both cores: the first word after a pending WAIT is fetched just before the next restart in every field, so x_j is fetched for every h_j above $E2 and no input of this package passes the comparator; neither unfalsifiable nor a scope decision |
| `P6-NEVER` | none impossible | - | - | - | already red on both cores for the reason of P6-LARGEST: no field of this package has a WAIT that never ends without a fetch before the next restart; neither unfalsifiable nor a scope decision |
| `PRE-P5-CALLO` | stimulus red | `D-DFLATLO-PAL` | FAIL | FAIL | every plane word of D-lo is $FFFF, so there are no alternating runs |
| `PRE-P5-CALHI` | stimulus red | `D-DFLATHI-PAL` | FAIL | FAIL | every plane word of D-hi is $FFFF, so there are no alternating runs |
| `PRE-P5-EDGES` | stimulus red | `D-DDROP-PAL` | FAIL | FAIL | the second MOVE($180,B) of line $50 is left out, so line $50 has no A to B edge |
| `PRE-P5-WAITED` | stimulus red | `D-DLATE-PAL` | FAIL | FAIL | the second WAIT of line $50 is at $3A, already satisfied when it is fetched |
| `P5-LO` | stimulus red | `D-DSHIFT-PAL` | FAIL | FAIL | the second WAIT of line $50 is at $36+2*24+2, so w_$50 is 4 * 25 * P_lo while n_$50 is 24 |
| `P5-HI` | stimulus red | `D-DSHIFT-PAL` | FAIL | FAIL | the second WAIT of line $50 is at $36+2*24+2, so w_$50 is 8 * 25 * P_hi while n_$50 is 24 |
| `PRE-P9-PLO` | stimulus red | `E-PLOFLAT-PAL` | FAIL | FAIL | the calibration run D-lo has every plane word $FFFF, so P_lo is not valid |
| `PRE-P9-LINES` | stimulus red | `E-ENOBPL-PAL` | FAIL | FAIL | bitplane DMA is left off, so no output line shows COLOR01 |
| `PRE-P9-ONE0` | stimulus red | `E-EBLACKGAP-PAL` | FAIL | FAIL | COLOR00 = $000 and the 10th plane word of every line is $0000, so each window line has a second run of 0 |
| `P9-A-E` | stimulus red | `E-EWIDEWIN-PAL` | FAIL | FAIL | DIWSTRT $2C71 and DIWSTOP $2CD1 with 22 words of $FFFF per line, so the COLOR01 run is 352 * P_lo |
| `P9-A-EW` | stimulus red | `E-EWWIDEWIN-PAL` | FAIL | FAIL | the E-widefetch capture is replaced by E-widewindow, so its COLOR01 run is 352 * P_lo |
| `P9-A` | stimulus red | `E-EWIDEWIN-PAL` | FAIL | FAIL | DIWSTRT $2C71 and DIWSTOP $2CD1 with 22 words of $FFFF per line, so the aggregate fails through P9-A-E |
| `P9-B` | stimulus red | `E-EBLACK-PAL` | FAIL | FAIL | COLOR00 = $000, so no COLOR00 pixel is distinguishable from the blanking on either side |


### 8.7 Claims

The ids on `PASS:` and `FAIL:` lines of the final run are the asserted claims;
the ids on `RECORDED:` lines are the declined claims. No id is on both.

Asserted (verdicts on the lines that carry the id):

- `COP2-horizontal-beam-position.c10`: FAIL
- `COP2-horizontal-beam-position.c11`: FAIL
- `COP2-horizontal-beam-position.c12`: FAIL
- `COP2-horizontal-beam-position.c14`: FAIL
- `COP2-horizontal-beam-position.c15`: PASS
- `COP2-horizontal-beam-position.c3`: PASS
- `COP2-horizontal-beam-position.c4`: PASS
- `COP2-horizontal-beam-position.c5`: PASS
- `COP2-horizontal-beam-position.c7`: PASS
- `COP2-horizontal-beam-position.c9`: PASS
- `COP2-putting-together-a-copper-instruction-list.c7`: PASS
- `COP2-the-wait-instruction.c22`: PASS
- `COP2-the-wait-instruction.c23`: PASS
- `COP2-the-wait-instruction.c26`: FAIL
- `COP2-the-wait-instruction.c27`: PASS
- `COP2-the-wait-instruction.c28`: FAIL
- `COP2-the-wait-instruction.c29`: FAIL
- `COP2-vertical-beam-position.c11`: FAIL
- `COP2-vertical-beam-position.c23`: PASS
- `COP2-vertical-beam-position.c7`: PASS
- `COP2-vertical-beam-position.c9`: FAIL
- `ch2-horizontal-beam-position.c4`: PASS
- `ch2-horizontal-beam-position.c5`: PASS
- `ch2-horizontal-beam-position.c8`: PASS

Declined on `RECORDED:` lines:

- `COP2-copins-timing.c6`
- `COP2-horizontal-beam-position.c13`
- `COP2-horizontal-beam-position.c16`
- `COP2-horizontal-beam-position.c6`
- `COP2-horizontal-beam-position.c8`
- `COP2-putting-together-a-copper-instruction-list.c10`
- `COP2-putting-together-a-copper-instruction-list.c9`
- `COP2-using-the-copper-in-interlaced-mode.c2`
- `COP2-using-the-copper-in-interlaced-mode.c4`
- `COP2-vertical-beam-position.c10`
- `COP2-vertical-beam-position.c24`
- `COP2-vertical-beam-position.c3`
- `COP2-vertical-beam-position.c4`
- `ch2-horizontal-beam-position.c14`
- `ch2-horizontal-beam-position.c6`
- `ch2-horizontal-beam-position.c7`
- `ch2-vertical-beam-position.c21`

In both lists: none
