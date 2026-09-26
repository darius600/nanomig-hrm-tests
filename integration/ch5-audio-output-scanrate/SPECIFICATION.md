# Specification: audio byte output, ADKCON disk-rate bits, AUDxPER and scan rate

This document specifies the integration test package `ch5-audio-output-scanrate`.
It was written from the package's claims (`claims-spec-stage.jsonl`) and the
pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who
never saw the implementation or any test. Page numbers are printed page
numbers. Quotations were checked against the printed page where the claim
depends on them (pp. 155, 156, 158, 164, 249, 265, 266, 279, 290, 310).

The package has 16 claims. Six propositions are specified (P1-P6). Ten claims
give no proposition. Five are used only in part. One (5-appA-p265.c42) is
used in full.

---

## 1. Claims not turned into a proposition

### 1.1 Not used

**AUDxPER and the programmable scan rate (p. 310): 5-appC-p310.c5,
5-appC-p310.c6, 5-appC-p310.c7, 5-appC-p310.c8, C-interp.per.c1,
C-interp.per.c2, C-interp.per.c3, C-interp.per.c4.**

The passage reads, in full: "The AUDxPER register is another register value
that varies according to the programmable scan rate. / AUDxPER 0A6 W P Audio
channel x period / With programmable scan rates, the maximum value read from
this register will differ. Generally, the faster the scan rate, the smaller
the maximum period becomes. Adjustments to the scan rate are reflected in
this maximum value."

A test *can* program a scan rate: HTOTAL/VTOTAL on p. 304, BEAMCON0 bit 7
VARBEAMEN on p. 305. None of these claims can still be graded:

- **No value.** The page gives no maximum for any scan rate, and no rule
  linking a scan rate to a maximum. c5/c1 ("varies"), c6/c2 ("will differ")
  and c8/c4 ("are reflected in this maximum value") state only that some
  dependence exists. A test would have to invent the value.
- **Hedged direction.** The one directional statement (c7/c3) begins with
  "Generally". That does not claim the relation holds in every case.
- **Nothing to observe.** The page does not say what happens when a period
  above "the maximum" is written. No output distinguishes a period inside the
  maximum from one beyond it.
- **Nothing to read.** "The maximum value read from this register" cannot be
  read. The same page lists AUDxPER as `W`, and so do p. 266 ("AUDxPER 0A6 W
  P(E)"), p. 290 ("AUD0PER 0A6 W P (E)") and p. 298 ("0A6 AUDxPER chg W P").
  Appendix B defines the code on p. 287: "W=write-only". C-interp.per.c2
  already notes this conflict.
- **C-interp.per.c3 goes beyond its page.** Its "says" adds that p. 140's
  maximum of 65535 "holds for the standard chip set only". That sentence
  appears on neither page. Page 310 speaks of "programmable scan rates", not
  of the ECS as such. Page 140 says of 124 and 65535: "These limits apply to
  both PAL and NTSC machines."

**The 7 kHz filter (p. 158): 5-p158.c37, 5-scale.c13.**
Quote: "...makes the most effective use of the Amiga's 7 KHz cut-off filter to
prevent noise."

- **The pages disagree on the frequency.** p. 156: "This filter becomes active
  around 4 KHz and gradually begins to attenuate (cut off) the signal." The
  label under Figure 5-5 (p. 155): "Filter passes all frequencies below about
  5 kHz." p. 158: "7 KHz cut-off filter". No page gives an attenuation at any
  stated frequency. All three agree only that the filter is a low-pass
  filter somewhere between about 4 and 7 kHz, and there is no number in that
  to grade.
- **The filter is not placed in the chips.** p. 156: "The system includes a
  low-pass filter". p. 157: the bypass "is the same output bit of the 8520
  CIA that controls the brightness of the red "power" LED (CIA A $BFE001 -
  Bit 1: /LED)". Nothing puts it inside the chips whose audio output the test
  samples. The test does see the power LED, but no claim of this package
  concerns that bit.

### 1.2 Used only in part

**MSBSYNC function (p. 265): 5-appA-p265.c20, A-adkcon.b09.**
"09 MSBSYNC Enables disk read synchronizing on the MSB (most significant
bit). Appl type GCR." The function acts on data read from a disk. p. 250:
"MSBSYNC, when a 1, tells the disk controller to look for this sync bit on
every disk byte". The harness serves no disk image, and a test cannot drive
the disk read-data input, so the function **cannot be checked yet**.

What a program can still check through registers: that bit 9 exists at that
position and holds its state (P5).

**FAST disk data clock rate (p. 265): A-adkcon.b08, 5-appA-p265.c21.**
"08 FAST Disk data clock rate control 1=fast(2us) 0=slow(4us)."

- **Cannot be checked yet on the disk data itself.** The harness serves no
  disk image, and the drive's data lines are not among the outputs a test
  sees.
- **What can still be checked:**
  - through registers, the bit's position and state (P5);
  - at the Chip-memory port, the rate at which disk *write* DMA takes words
    from memory (P6). The pages tie this rate to the bit-cell time.
- **P6 is conditional.** It is gradable only if disk write DMA runs with no
  disk image. Its precondition B3 decides this, and a failure there means
  "not checkable yet", not red.
- **Not graded:** the parenthesis "(fast for MFM, slow for MFM or GCR)". It
  is advice on use.

**AUDxDAT LSB size (p. 265): A-audxdat.c5.**
"(LSB = 3 MV)"

- **Not observable:** the magnitude, 3 mV at the output pins. The test sees
  samples, not volts.
- **Used:** what the single LSB value implies for the samples, namely that
  every one-LSB step of a byte moves the output by the same amount (P4). The
  words its quote shares with 5-appA-p265.c42 are also used (P3).
- The print reads "MV" (checked on the scan), which literally means
  megavolts. p. 140 supports millivolts: "For a typical output at volume 64,
  with maximum data values of -128 to 127, the voltage output is between +.4
  volts and -.4 volts" (128 x 3 mV = 0.384 V).

---

## 2. Setup common to all propositions

### 2.1 Configurations

Every proposition runs on both CPU cores, each separately, in these
configurations:

| Proposition | Configurations |
|---|---|
| P1-P4 | ECS/PAL (default), OCS/PAL |
| P5 | ECS/PAL, OCS/PAL |
| P6 | ECS/PAL, OCS/PAL, ECS/NTSC |

Why OCS as well: AUDxDAT and ADKCON are Paula registers, and Appendix A
describes them for the standard chip set. p. 297 says the Enhanced Chip Set
"consists of compatible revisions to the Agnus and Denise custom chips", so
no page separates OCS from ECS for these registers.

Why NTSC for P6: pp. 249 and 265 give one bit-cell time for both video
standards.

Program and data placement: all program code, tables and buffers lie in Chip
memory below $80000, at even addresses (p. 138: audio data "must start at a
word-address boundary"; p. 246: the disk buffer's "lowest bit of the address
must be zero, and the buffer must be in Chip memory"). This keeps the high
pointer bits within the 3 bits OCS has (p. 266, p. 289).

### 2.2 Registers used

Custom chip base $DFF000 (p. 241: "ADKCON ($DFF09E)"). Offsets from Appendix
B (pp. 289-293) and Appendix A:

| Register | Offset | R/W | Bits used |
|---|---|---|---|
| ADKCONR | 010 | R | 9 MSBSYNC, 8 FAST, 7-0 audio attach (p. 265) |
| DSKBYTR | 01A | R | 14 DMAON, 13 DISKWRITE (p. 248) |
| INTREQR | 01E | R | 5 VERTB, 1 DSKBLK (p. 280) |
| DSKPTH/DSKPTL | 020/022 | W | written as one long word (p. 246) |
| DSKLEN | 024 | W | 15 DMAEN, 14 WRITE, 13-0 LENGTH (p. 246) |
| DMACON | 096 | W | 15 SET/CLR, 9 DMAEN, 4 DSKEN, 0 AUD0EN (p. 278) |
| INTENA | 09A | W | 15 SET/CLR (p. 280) |
| INTREQ | 09C | W | 15 SET/CLR (p. 280) |
| ADKCON | 09E | W | 15 SET/CLR, 9, 8, 7-0 (p. 265) |
| AUD0LCH/AUD0LCL | 0A0/0A2 | W | written as one long word (p. 139) |
| AUD0LEN | 0A4 | W | length in words (p. 266) |
| AUD0PER | 0A6 | W | period in color clocks (p. 266) |
| AUD0VOL | 0A8 | W | volume, 64 = maximum (p. 140, p. 266) |
| AUD1VOL, AUD2VOL, AUD3VOL | 0B8, 0C8, 0D8 | W | 0 = "no output" (p. 266) |
| HTOTAL, VTOTAL, BEAMCON0 | 1C0, 1C8, 1DC | W | ECS only; one contrasting case (F1) |

### 2.3 Program conventions

- **Preamble.** Every program starts with:
  - INTENA = $7FFF (all interrupt enables cleared);
  - INTREQ = $7FFF (all requests cleared);
  - DMACON = $7FFF (all DMA enables cleared; bit 15 = 0 clears, p. 278);
  - ADKCON = $7FFF (bits 14-0 cleared, p. 265: "Bits written with a zero are
    always unchanged", bit 15 = 0 clears the others).
- **Result block.** Values read back go to a result block at a fixed Chip
  address, one word per slot. The test sees each store as a CPU write at the
  Chip-memory port.
- **Bounded waits.** Every wait is bounded, so that every variant finishes.
  The program counts vertical-blank requests (poll INTREQR bit 5, clear it
  with INTREQ = $0020). A CPU loop counter serves as a fallback bound, so the
  wait ends even if no request comes. How the program waits is not graded.
- **Done marker.** The program's last action writes a done marker to a fixed
  Chip address. Every run, including every contrasting case, must show that
  write. Otherwise the run is broken, not graded.

### 2.4 Reference frames

**F1: time base, used by P6 only.**
p. 194: "During a horizontal scan line (about 63 microseconds), there are
227.5 "color clocks", or memory access cycles", and the per-line allocation
includes "4 cycles for memory refresh".

- **Anchor.** Memory-refresh accesses at the Chip-memory port come in one
  group per line. Take the first refresh access of each group over 100
  consecutive lines. Convert the mean spacing to color clocks using the
  harness's declared system-clock-cycles-per-color-clock ratio. It must lie
  between 227 and 228 color clocks (the page's 227.5, to half a color clock).
- **Conversion to microseconds.** Microseconds = color clocks x the clock
  interval on p. 141: 0.281937 us for PAL, 0.279365 us for NTSC.
- **Contrasting case** (ECS only). Before the measured runs, write VTOTAL =
  524, then HTOTAL = 113, then BEAMCON0 = $00A0 in PAL or $0080 in NTSC
  (bit 7 VARBEAMEN, bit 5 PAL; p. 304, p. 305).
  - Effect: p. 304 describes HTOTAL as "the number of 280ns increments on the
    horizontal line", and lists VGA at "114.0 colorclocks per scan line". The
    refresh groups then recur about 114 color clocks apart, and the anchor
    must go red.
  - The program still finishes.
  - p. 305's warning ("Do not write to this register") concerns attached
    monitors. The write is used here only as this contrasting case.

**F2: audio pass, used by P1-P4.**
p. 139: "Each time the system retrieves the last audio word from the data
area, it uses the contents of these location registers to again find the
start of the data."

- **Boundaries.** A pass begins with each audio DMA read of the table's first
  word, the word at the address written to AUD0LC. Number these reads
  t_1, t_2, ... in time order.
- **Anchor check.** Each read of the first word except t_1 is immediately
  preceded, among the channel's reads, by a read of the table's last word.
  Within a pass, the read addresses rise by 2.
- **Analysis window.** W_T = [t_2, t_(2+N)), N passes long. The first pass is
  left out because p. 164 says of DMA start: "Because of pipelining in Agnus,
  the first data word must be thrown away". The page leaves open whether the
  first word of the first pass is output.

**F3: disk word index, used by P6.**
p. 247: "As each data word is transferred, the length value is decremented.
After each transfer occurs, the value of the pointer is incremented."

- **Index.** Disk DMA read number k is the read of word address (buffer
  start + 2k).
- **Anchor check.** The reads come in the order k = 0, 1, 2, ..., one per k.

### 2.5 Audio analysis procedures (P1-P4)

The observed output is the right-side audio output. p. 137: "Channels 0 and 3
are connected to the right-side output jack." If the harness offers only a
mono mix, use the mix: the other channels are silenced (2.3, and step 3 of
the audio program).

**LEVELS(W): the list of held levels in window W.**
1. Take the output samples whose time lies in W.
2. Split them into runs of identical consecutive values, and note each run's
   duration.
3. Let m be the median run duration. Drop runs shorter than m/2. These are
   transitions, not held levels.
4. Merge neighbouring remaining runs with equal values. The values of the
   merged runs, in time order, form the list.
5. Drop the first and the last entry, which the window edges may cut.

**FIT(L, C): does the level list match a code sequence?**
C is the sequence of byte codes the page predicts for one pass, in output
order, with consecutive equal codes merged. Its length is K. s_j is the
two's-complement value of byte C[j] (p. 265: "each 2's complement"; p. 138:
"Sample values can range from -128 to +127").

1. For each rotation r = 0 ... K-1, fit L[i] ~ a + b * s_((i+r) mod K) by
   least squares over all i.
2. L matches C when, for some r, every residual |L[i] - a - b * s| is at most
   |b|/4 (a quarter LSB).
3. b is the output change per LSB. Its sign is not graded: the page ties the
   LSB to a voltage, and the sign convention of the harness's samples is the
   harness's own.

Because every table below has distinct codes within a pass, a match also
fixes the list's period at exactly K.

### 2.6 The audio program and its tables (P1-P4)

**Program:**
1. Preamble (2.3).
2. Read ADKCONR and store it (precondition A2).
3. AUD1VOL = AUD2VOL = AUD3VOL = 0. p. 266: "000000=no output". Channels 1-3
   keep DMA off.
4. For each table T in the order T1, T2, T3, follow the steps on p. 137 ("3.
   Set registers telling the system where to find the data and the length of
   the data. 4. Select the volume ... 5. Select the sampling period ... 6.
   Select an audio channel and start up the DMA"):
   1. AUD0LCH/AUD0LCL = address of T, as one long word (p. 139).
   2. AUD0LEN = number of words in T (p. 139).
   3. AUD0VOL = 64 (p. 140: "64 ... (maximum volume)").
   4. AUD0PER = P (p. 143).
   5. DMACON = $8201 (SET, DMAEN, AUD0EN; p. 144: "When both DMAEN and AUDxEN
      are set, the DMA will start for channel x").
   6. Wait at least N_T + 3 passes. One pass lasts 2 x (words in T) x P color
      clocks.
   7. DMACON = $0001, which clears AUD0EN (p. 144: "Once started, the DMA
      continues until you specifically stop it").
5. Done marker.

**Period P.** The smallest of 128, 256, 512 and 1024 for which the harness's
audio output delivers at least 4 samples in P color clocks. Every choice is
above the minimum on p. 140 ("124 ticks per sample NTSC (123 PAL)") and
p. 266 ("124 color clocks"). Every choice is also well below the rate at
which p. 164 says samples repeat ("If the sampling rate is set much higher
than the normal maximum sampling rate (approximately 29 KHz), the two samples
in the buffer register will be repeated").

**Tables.** The code sequence C is written as byte codes in output order.

| Table | Words | Code sequence C | K | N |
|---|---|---|---|---|
| T1 | $0010, $2030, $4050, $6070 | C_up = 00 10 20 30 40 50 60 70 (upper byte first) | 8 | 8 |
| T2 | $0000, $0101, $7F7F, $8080 | C2 = 00 01 7F 80 (equal bytes merge; s = 0, 1, 127, -128) | 4 | 8 |
| T3 | 256 words; word k = b_k * $0101 with b_k = (k + $80) mod $100, i.e. $8080, $8181, ..., $FFFF, $0000, ..., $7F7F | C3 = 80 81 ... FF 00 ... 7F (s = -128 ... 127) | 256 | 2 |

For T1 the lower-first order is C_low = 10 00 30 20 50 40 70 60.

T2 and T3 use words whose two bytes are equal, so their results do not depend
on byte order (P2). T1 contains only codes $00-$70, so its result does not
depend on how signs are read (P3).

### 2.7 Preconditions shared by P1-P4

Each is asserted for the table under test before its proposition is graded.
A failing precondition means "not graded", never green.

| Id | Precondition | How the test checks it | Contrasting case (precondition goes red; the program still finishes) |
|---|---|---|---|
| A1 | Audio DMA for the table ran, pass after pass | F2's anchor check holds, with at least N + 2 reads of the table's first word | DMACON = $8200 instead of $8201 (AUD0EN not set): no audio reads |
| A2 | No attach mode: channel 0's data goes to its D/A converter | The stored ADKCONR has bits 7-0 = 0 (p. 265, bits 7-0) | Add ADKCON = $8001 (USE0V1) between the preamble and step 2, before the read. Bit 0 then reads 1. p. 165 predicts channel 0's data goes elsewhere: "AUDxAV Attach volume. Send data to volume latch of next channel instead of to D->A converter" |
| A3 | Held levels are long enough to resolve | The median run in W_T spans at least 4 output samples | P = 124 (p. 140's minimum), which fails A3 when the harness delivers fewer than about 114,000 samples per second. If the harness is faster, no legal period fails A3; the test states that this case does not apply |
| A4 | The window holds enough levels, and one LSB is resolved | The list L from LEVELS(W_T) has at least 2K entries for T1 and T2, and at least K + 1 for T3; the fitted \|b\| is at least 4 steps of the harness's own sample resolution | AUD0VOL = 0 (p. 266: "000000=no output"): the output stays flat, L has at most one entry |

---

## 3. Propositions

### P1: Both bytes of every audio data word reach the output, one after the other

1. **Proposition.** Every data word delivered to AUD0DAT is output as its two
   bytes, one after the other. Claim: 5-appA-p265.c42 (p. 265).
2. **Stimulus.** The audio program (2.6) with table T1.
3. **Observation.** The right-side audio output inside W_T1 (F2), read as
   L = LEVELS(W_T1).
4. **Expected relation.** p. 265: "It contains 2 bytes of data that are each
   2's complement and are outputted sequentially (with digital-to-analog
   conversion) to the audio output pins."
   - Graded: FIT(L, C_up) or FIT(L, C_low) matches. Either order counts here;
     the order is P2.
   - Quantifier: "2 bytes ... each ... sequentially" means both bytes of
     every word. The fit covers all 4 words in each of the 8 passes, 32 word
     loads, and fails if either byte is missing. Outputting only the upper
     bytes, for example, gives 00 20 40 60, which matches neither sequence.
5. **Preconditions.** A1-A4 for T1.
6. **Contrasting case.** Replace T1's last word $6070 by $7060. The page
   predicts output 00 10 20 30 40 50 70 60, which matches neither C_up nor
   C_low. P1 goes red. A1-A4 still hold.
7. **Reference frame.** F2 (pass boundaries at the reads of the table's first
   word, checked as stated there).

### P2: The upper byte is output first

1. **Proposition.** Of the two bytes of an AUD0DAT word, the upper byte
   (bits 15-8) is output first and the lower byte second.
   - Claim: 5-appA-p265.c42 (p. 265, "outputted sequentially").
   - The order comes from p. 164, a page that speaks to the same point; it is
     not in this package's claims.
2. **Stimulus.** As for P1 (table T1). The same run serves both propositions.
3. **Observation.** As for P1. Upper and lower byte are identified from the
   16-bit data word that the Chip-memory port shows for each audio read.
4. **Expected relation.**
   - p. 164: "In the 010 state the upper byte is output, and in the 011 state
     the lower byte is output. Transitions such as 010->011->010 occur
     whenever the period counter counts down to one."
   - For DMA-driven operation, p. 164: "When the data arrives, state 010 is
     entered and the main loop continues until the DMA is turned off."
   - p. 138 agrees: "The data is stored in byte address order with the first
     digitized amplitude value at the lowest byte address, the second at the
     next byte address".
   - Graded: FIT(L, C_up) matches. Quantifier: every word of every pass in
     W_T1.
5. **Preconditions.** A1-A4 for T1, and P1 green. If P1 is red, P2 is not
   graded.
6. **Contrasting case.** Swap the bytes of every word: $1000, $3020, $5040,
   $7060. The page predicts 10 00 30 20 50 40 70 60. C_up does not match,
   so P2 goes red, while P1 stays green (C_low matches). A1-A4 still hold.
7. **Reference frame.** F2, as for P1.

### P3: Each byte is output as a two's-complement value

1. **Proposition.** The output level of each byte follows the byte's
   two's-complement value, so $80 is the most negative and $7F the most
   positive code. Claims: 5-appA-p265.c42 and A-audxdat.c5, whose quotes both
   carry "2 bytes of data that are each 2's complement" (p. 265).
2. **Stimulus.** The audio program (2.6) with table T2.
3. **Observation.** L = LEVELS(W_T2).
4. **Expected relation.**
   - p. 265: "It contains 2 bytes of data that are each 2's complement". p.
     138: "Sample values can range from -128 to +127."
   - Graded: FIT(L, C2) matches, with s = 0, 1, 127, -128.
   - Why T2 separates the readings, independent of phase and sign convention:
     under two's complement the cyclic level steps are proportional to
     (+1, +126, -255, +128). Under an unsigned or offset reading of the same
     bytes they would be proportional to (+1, +126, +1, -128). No rotation or
     scale turns one into the other.
   - Quantifier: "each" means both bytes of every word. Every code in T2
     occupies both byte positions.
5. **Preconditions.** A1-A4 for T2.
6. **Contrasting case.** Flip bit 7 of every byte: $8080, $8181, $FFFF,
   $0000. The page predicts s = -128, -127, -1, 0, with steps proportional to
   (+1, +126, +1, -128). That is exactly the output an unsigned reading of T2
   would give, and FIT(L, C2) fails, so P3 goes red. A1-A4 still hold.
7. **Reference frame.** F2.

### P4: Every one-LSB step moves the output by the same amount

1. **Proposition.** At a fixed volume, every one-LSB step of a data byte
   moves the audio output by the same amount. Claim: A-audxdat.c5 (p. 265).
   The 3 mV magnitude is not observable (1.2).
2. **Stimulus.** The audio program (2.6) with table T3 (all 256 codes, in
   ascending signed order, each word with equal bytes). Volume 64.
   - Why 64: p. 265 names no volume. p. 140 ties the full-range output to
     volume 64 ("For a typical output at volume 64, with maximum data values
     of -128 to 127, the voltage output is between +.4 volts and -.4 volts"),
     which agrees with 128 x 3 mV.
   - No other volume is graded.
3. **Observation.** L = LEVELS(W_T3), covering two full passes (about 510
   levels).
4. **Expected relation.**
   - p. 265: "(LSB = 3 MV)". A single value for one LSB means every step
     between adjacent codes has the same size.
   - Graded: FIT(L, C3) matches, with s = -128 ... 127. All 255 steps between
     adjacent codes must equal b, to within a quarter LSB.
   - Quantifier: every one of the 256 codes and every one of the 255 adjacent
     steps, in both passes.
   - Note: an unsigned reading of the bytes also matches this fit (rotated by
     128). P4 grades the step size only; the sign reading is P3.
5. **Preconditions.** A1-A4 for T3.
6. **Contrasting case.** Replace the word for code +5 ($0505, k = 133) by
   $0707. The page predicts the codes ... 04 07 06 07 08 ..., steps of +3 and
   -1 where +1 is required, and FIT(L, C3) fails, so P4 goes red. A1-A4 still
   hold.
7. **Reference frame.** F2.

### P5: ADKCON bits 8 (FAST) and 9 (MSBSYNC) are separate bits that ADKCONR reads back

1. **Proposition.** Setting or clearing bit 8 or bit 9 through ADKCON shows
   in the same bit of ADKCONR, bit 8 and bit 9 independently.
   - Claims: A-adkcon.b08, 5-appA-p265.c21 (bit 8 is FAST), A-adkcon.b09,
     5-appA-p265.c20 (bit 9 is MSBSYNC); p. 265.
   - This checks the register part of those claims only: the bits' positions
     and that they hold their state. What the bits *do* to disk data cannot
     be checked yet (1.2).
2. **Stimulus.** Preamble (2.3). Then five steps, each an ADKCON write
   followed at once by an ADKCONR read that is stored in its own result slot
   R0-R4:

   | Step | ADKCON write | Effect (p. 265) | Result slot |
   |---|---|---|---|
   | 1 | $0300 | clear bits 9 and 8 | R0 |
   | 2 | $8100 | set bit 8 | R1 |
   | 3 | $8200 | set bit 9 | R2 |
   | 4 | $0100 | clear bit 8 | R3 |
   | 5 | $0200 | clear bit 9 | R4 |

   p. 265, bit 15: "Set/clear control bit. Determines if bits written with a
   1 get set or cleared. Bits written with a zero are always unchanged."
   Then the done marker.
3. **Observation.** The five result slots, bits 9 and 8 only. The other bits
   are not graded here.
4. **Expected values.**
   - p. 265 lists ADKCON (W) and ADKCONR (R) over one bit table: "09 MSBSYNC",
     "08 FAST". p. 249: "ADKCON is the write-only address and ADKCONR is the
     read-only address for this register."
   - Expected (bit 9, bit 8): R0 = (0,0), R1 = (0,1), R2 = (1,1),
     R3 = (1,0), R4 = (0,0).
   - Quantifier: "1=fast ... 0=slow" and "Enables". Both states of each bit
     are graded, each in both states of the other bit.
5. **Preconditions.**
   - C1: the Chip-memory port shows one CPU write to each of R0-R4, in step
     order. This proves every readback was stored, and none is left over
     from memory initialisation.
6. **Contrasting cases.**
   - Proposition: write $8200 instead of $8100 in step 2. The page predicts
     R1 = (1,0), so P5 goes red. C1 still holds.
   - C1: leave out the store of R2. The port shows no write to R2, so C1
     goes red. The program still finishes.
7. **Reference frame.** None beyond the step order.

### P6: Disk write DMA takes a word per 16 bit cells of about 2 us with FAST = 1 and about 4 us with FAST = 0

1. **Proposition.** With ADKCON bit 8 (FAST) = 1, disk write DMA takes one
   word from memory per 16 bit cells of about 2 us. With FAST = 0, the bit
   cells are about 4 us, twice as long.
   - Claims: A-adkcon.b08, 5-appA-p265.c21 (p. 265). Wording on p. 249 (same
     point, same values).
   - This is the only part of the rate claim observable without a disk
     surface. **It is gradable only if disk write DMA runs without a disk
     image (precondition B3).**
2. **Stimulus.**
   - **Buffers.** Two buffers, A and B, of 544 words each, in Chip memory
     below $80000 at even addresses. Every word is $AAAA. p. 249 for FAST = 1:
     "Data must be valid raw MFM". By the table on p. 250 ("0 -> 10 ;if
     following a 0"), a run of data zeros encodes as $AAAA.
   - **Order of the runs.** Preamble (2.3). Then run F (FAST = 1, buffer A),
     then run S (FAST = 0, buffer B), then the done marker.
   - **Steps of each run.** Steps 3 and 5-7 follow the sequence on p. 247:
     1. ADKCON = $8100 in run F, $0100 in run S. Read ADKCONR and store it
        (B1).
     2. DMACON = $8210: SET, DMAEN, DSKEN (p. 247 step 1, "Enable disk DMA in
        the DMACON register"; p. 278).
     3. DSKPTH/DSKPTL = buffer address, as one long word (p. 246: "The value
        must be written as a single long word to the DSKPTH register").
     4. DSKLEN = $4000 (p. 247 step 2).
     5. DSKLEN = $C220, which is DMAEN, WRITE and LENGTH = 544 (p. 247 step 3;
        Table 8-6 on p. 246: "15 DMAEN", "14 WRITE Disk write (RAM -> disk if
        1)", "13-0 LENGTH Number of words to transfer").
     6. DSKLEN = $C220 again (p. 247 step 4: "Write this value again into the
        DSKLEN register. This actually starts the DMA.").
     7. Read DSKBYTR and store it (B2).
     8. Poll INTREQR bit 1 (DSKBLK, p. 250: "disk DMA has completed") until it
        is set, bounded by 8 vertical-blank requests (2.3). Store whether it
        was seen. It is not graded.
     9. DSKLEN = $4000 (p. 247 step 5). INTREQ = $0002.
   - **No drive is selected.** The program makes no CIA writes. p. 248 makes
     the DMA depend only on "the DMAEN bit in DSKLEN, and the DSKEN & DMAEN
     bits in DMACON".
3. **Observation.** Disk DMA reads at the Chip-memory port, numbered by F3:
   t_A(k) for buffer A and t_B(k) for buffer B.
   - **Interval.** From read k = 16 to read k = 528 (M = 512 words). The
     first 16 reads are left out as the controller's start-up. The interval
     ends before the length runs out at k = 543.
   - **Durations.** D_F = t_A(528) - t_A(16) and D_S = t_B(528) - t_B(16),
     converted to microseconds by F1.
   - **Bit-cell times.** T_F = D_F / (16 x 512) and T_S = D_S / (16 x 512).
   - **16 bit cells per word.**
     - A word is 16 raw bits. p. 279: DSKDAT "contains two bytes of data that
       are either sent (written) to or received (read) from the disk".
     - Memory holds raw bits, one per bit cell. p. 250: "The raw MFM data
       that must be presented to the disk controller will be twice as large
       as the unencoded data". p. 249: "two microseconds per bit cell ... Data
       must be valid raw MFM".
     - The disk has "3 cycles for disk DMA" in every line (p. 194). At 2 us
       per bit cell the controller needs about 2 words per line, which those
       slots allow, so over a long interval the read rate equals the rate at
       which bits are sent.
4. **Expected values.** p. 265: "08 FAST Disk data clock rate control
   1=fast(2us) 0=slow(4us)." p. 249: "Value of 1 selects two microseconds per
   bit cell (usually MFM). ... 0 selects four microseconds per bit (usually
   GCR)."
   - **G1:** 1.5 us <= T_F < 2.5 us. The page prints the value to one
     significant figure, and the test grades it at that precision.
   - **G2:** 3.5 us <= T_S < 4.5 us, for the same reason.
   - **G3:** |D_S - 2 x D_F| < 3 lines (3 x 228 color clocks).
     - The page gives the two settings as 2 and 4 for one control bit. The
       test reads this as a factor of two between them.
     - Where the band comes from: a read occurs in the next disk slot after
       the controller needs the word. That is less than one line later
       (p. 194, disk slots in every line), so each duration is off by less
       than one line. D_S - 2 x D_F is therefore off by less than 3 lines.
       The band is the measurement's resolution, not a tolerance on the
       page's value.
   - **Quantifier.** Both settings, each over 512 consecutive words.
5. **Preconditions.**
   - **B1:** the stored ADKCONR bit 8 equals the run's FAST setting.
   - **B2:** the stored DSKBYTR has bit 14 (DMAON) = 1 and bit 13
     (DISKWRITE) = 1 in each run. p. 248: "DMAON Indicates when DMA is
     actually enabled"; "DISKWRITE The disk write bit (in DSKLEN) is
     enabled".
   - **B3:** in each run, the port shows disk DMA reads of the buffer for
     k = 0 ... 528, one per k, in order (F3).
     - If B1 and B2 hold but B3 fails, record P6 as **not checkable yet**:
       disk write DMA did not run with no disk image in the harness. Do not
       record it as red.
   - **B4:** the F1 anchor holds.
6. **Contrasting cases.**
   - **G1:** run F with FAST = 0 (ADKCON = $0100). The page predicts about
     4 us, so G1 goes red. B1 still holds, because it compares ADKCONR with
     the run's own setting.
   - **G2:** run S with FAST = 1 (ADKCON = $8100). The page predicts about
     2 us, so G2 goes red. B1 still holds.
   - **G3:** both runs with FAST = 1. The page predicts D_S about equal to
     D_F, so G3 goes red. B1 still holds.
   - **B1:** in run F, write $0100 instead of $8100. Bit 15 = 0 clears
     instead of sets, bit 8 reads 0, and B1 goes red.
   - **B2:** in run F, leave out step 6, the second DSKLEN write. p. 247: "the
     DMAEN bit in the DSKLEN register must be turned on twice in order to
     actually enable the disk DMA hardware". DMAON reads 0 and B2 goes red.
     The DSKBLK poll times out, and the program finishes.
   - **B3:** LENGTH = 100 (DSKLEN = $C064 in steps 5 and 6). Only 100 reads
     occur, fewer than 529, so B3 goes red.
   - **B4:** the F1 contrasting case (ECS configurations only).
7. **Reference frames.** F1 (time base, anchored on refresh groups and
   checked there) and F3 (word index, anchored on the pointer increment and
   checked by B3).

---

## 4. What the pages leave open or state differently

- **p. 310 asks for a read of a write-only register.** "the maximum value
  read from this register" is said of AUDxPER, which p. 310 itself lists as
  `W`, as do p. 266, p. 290 and p. 298 (p. 287: "W=write-only"). No page
  gives the maximum for any programmable scan rate. Page 140 gives one
  maximum, 65535, "for both PAL and NTSC machines". The only link the chapter
  draws between period and line rate is for the *minimum* (p. 141: "The
  sampling period is limited by the number of DMA cycles allocated to an
  audio channel. Each audio channel is allocated one DMA slot per horizontal
  scan line"). Page 310 speaks of the *maximum*. No proposition is specified.
- **The (E) marks differ.** p. 290 marks "(E)" only on AUD0LCH and AUD0PER.
  AUD1-AUD3 LCH and PER carry none, and AUD1LCH-AUD3LCH read "(high 3 bits)".
  p. 266 marks AUDxLCH and AUDxPER "(E)" for every channel, and p. 298 lists
  AUDxLCH and AUDxPER as "chg". Checked on the scan.
- **Filter frequency.** About 4 kHz (p. 156, "becomes active around 4
  KHz"), about 5 kHz (Figure 5-5 on p. 155, "passes all frequencies below
  about 5 kHz"), 7 kHz (p. 158, "7 KHz cut-off filter"). Only "low-pass" is
  common to all three.
- **"LSB = 3 MV"** (p. 265) literally reads megavolts. p. 140's +/-0.4 V for
  -128 ... 127 at volume 64 supports millivolts.
- **FAST.** p. 265 says "(fast for MFM, slow for MFM or GCR)". p. 249 says
  "(usually MFM)" for fast and "(usually GCR)" for slow, and calls the unit
  "per bit cell" for fast and "per bit" for slow. Not a contradiction in the
  values: both give 2 us and 4 us.
- **DMA start.** p. 164's "the first data word must be thrown away" leaves
  open whether the table's first word is output in the first pass after DMA
  starts. P1-P4 avoid the first pass (F2).
- **Precision of the disk values.** The pages print the bit-cell times as
  "2us"/"4us" and "two"/"four microseconds" with no further precision. P6
  grades the absolute values only to one significant figure. The factor of
  two between them is the sharper check.
