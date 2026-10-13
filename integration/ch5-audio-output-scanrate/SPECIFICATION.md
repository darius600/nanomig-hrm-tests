# Specification: audio byte output, ADKCON disk-rate bits, AUDxPER and scan rate

This document specifies the integration test package `ch5-audio-output-scanrate`.
It was written from the package's claims (`claims-spec-stage.jsonl`) and the
pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who
never saw the implementation or any test. Page numbers are printed page
numbers. Quotations were checked against the printed page where the claim
depends on them (pp. 155, 156, 158, 164, 249, 265, 266, 279, 290, 310).

The package has 16 claims. Seven propositions are specified (P1-P6, plus
P3B alongside P3). Ten claims give no proposition. Six are used only in
part. None are used in full.

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

A program can still check through registers that bit 9 exists at that
position and holds its state; P5 (3, P5) runs that register check, but the
check is a proxy for a different clause (whether the bit synchronizes disk
reads), so this id is not asserted there. It stays declined.

**FAST disk data clock rate (p. 265): A-adkcon.b08, 5-appA-p265.c21.**
"08 FAST Disk data clock rate control 1=fast(2us) 0=slow(4us)."

- **Cannot be checked at this boundary.** The page's quantity is the disk
  data clock on Paula's write-data pin (p. 380: `38 DKWD* Disk write data
  O`). The harness serves no disk image, and `nanomig.v` has no such pin;
  its disk side is the SD-card sector interface.
- **The Chip-memory read rate is a different quantity.** G1, G2 and G3 in
  P6, 3, time the rate at which disk write DMA reads words at the
  Chip-memory port. No page ties that rate to the bit-cell time -- it would
  need an unprinted premise (that DSKDAT is the whole of the storage between
  memory and the pin, or that Paula requests a word only when the previous
  one has been sent; p. 194 lets a device leave its slots unused, and
  pp. 380-381 name Paula's `DMAL`, "DMA request line", and no more). G1, G2
  and G3 decline the claim for this reason; the durations they measure stay
  printed as MEASURED lines.
  - through registers, the bit's position and state alone -- P5 runs that
    check too, but a register readback is a proxy for the rate the bit
    controls, not the rate itself, so this id is not asserted on P5.
- **Not graded:** the parenthesis "(fast for MFM, slow for MFM or GCR)". It
  is advice on use.

**AUDxDAT LSB size (p. 265): A-audxdat.c5.**
"(LSB = 3 MV)"

- **Declined in full.** The magnitude, 3 mV at the output pins, is not
  observable: the test sees samples, not volts. Its quote's two's-complement
  words overlap 5-appA-p265.c42's own quote only so the citation is long
  enough to anchor; P3 and P3B grade two's complement from the page
  directly, not under this id. P4 grades the implied uniform one-LSB step,
  but only as a proxy (equal step size, not the literal 3 mV magnitude), so
  this id is not asserted on P4 either.
- The print reads "MV" (checked on the scan), which literally means
  megavolts. p. 140 supports millivolts: "For a typical output at volume 64,
  with maximum data values of -128 to 127, the voltage output is between +.4
  volts and -.4 volts" (128 x 3 mV = 0.384 V).

**Audio byte output, D/A conversion and the output pins (p. 265):
5-appA-p265.c42.**
"It contains 2 bytes of data that are each 2's complement and are outputted
sequentially (with digital-to-analog conversion) to the audio output pins."

- **Withdrawn in full.** `nanomig.v`'s ports carry the digital sample bus
  (`audio_left`/`audio_right`), not a D/A-converted analog level or a
  physical output pin; the quote's "with digital-to-analog conversion ...
  to the audio output pins" clause is not observable at this boundary, so
  no assertion establishes the id's own full claim.
- The narrower parts the quote also states -- both bytes output in order
  (P1), the upper byte first (P2, from p. 164), each byte two's-complement
  (P3, P3B) -- are graded directly from those pages, without this id.

---

## 2. Setup common to all propositions

### 2.1 Configurations

Every proposition runs on both CPU cores, each separately, in these
configurations:

| Proposition | Configurations |
|---|---|
| P1-P4, P3B | ECS/PAL (default), OCS/PAL |
| P5 | ECS/PAL, OCS/PAL |
| P6 | ECS/PAL, OCS/PAL, ECS/NTSC |

Why OCS as well: AUDxDAT and ADKCON are Paula registers, and Appendix A
describes them for the standard chip set. p. 297 says the Enhanced Chip Set
"consists of compatible revisions to the Agnus and Denise custom chips", so
no page separates OCS from ECS for these registers.

Why NTSC for P6: pp. 249 and 265 give one bit-cell time for both video
standards.

Data placement: the audio tables and disk buffers -- the data the DMA
pointer registers target -- lie in Chip memory below $80000, at even
addresses (p. 138: audio data "must start at a word-address boundary";
p. 246: the disk buffer's "lowest bit of the address must be zero, and the
buffer must be in Chip memory"). This keeps the high pointer bits within
the 3 bits OCS has (p. 266, p. 289); it does not bear on where the program
code itself or its result block sit, since neither is read by a
pointer-width-limited DMA channel.

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
| VHPOSR | 006 | R | bits 15-8: vertical beam position, low byte; read only by the last-level contrast of P1 (Appendix B) |
| HTOTAL, VTOTAL, BEAMCON0 | 1C0, 1C8, 1DC | W | ECS only; contrasting cases of F1 and A6 |

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
- **Fixture constant.** Every program also stores one known constant to its
  own fixed slot, checked by the test before it relies on anything else
  that program stores. Stored right before the done marker, not first,
  so it cannot disturb the timing any earlier, DMA-dependent measurement
  in the same program depends on. Printed as FIXTURE-CONST; the lines that
  read a stored result slot (A2, C1, P5, B1, B2) are graded behind it. The
  test reads that constant's slot and compares it before it reads any other
  result slot of the program; a program whose constant does not read back
  has none of its other slots read.
- **Table content on the port.** Every poked table and buffer is checked on
  the Chip-memory port before a line relies on it (FIXTURE-DATA): every read
  of a table's words returns the content poked there, as changed by the CPU
  writes to the table that the port shows before that read (none, except in
  the last-level contrast of P1, where exactly one), and the number of such
  writes is the stimulus's own; every word of the table is read; for a disk buffer, whose length one case shortens, every read
  returns the poked content and at least one read happens. A table's A4, A5
  and propositions are graded behind its own FIXTURE-DATA line. No graded
  line of the disk program reads buffer content, so none is graded behind
  those two.
- **Result-slot writes.** Every result slot a program stores is written by
  the program (FIXTURE-WRITE: one CPU write to it on the port); the line
  that reads the slot (A2, B1 or B2 of a run) is graded behind that slot's
  line.
- **Done marker.** Every program ends by writing it (FIXTURE); a run that
  does not show it grades nothing further.

### 2.4 Reference frames

**F1: time base, used by P6 and by A6 (2.7).**
p. 194: "During a horizontal scan line (about 63 microseconds), there are
227.5 "color clocks", or memory access cycles", and the per-line allocation
includes "4 cycles for memory refresh". p. 304 gives the PAL figure again:
"PAL (625 lines, 227.5 colorclocks per scan line)".

- **Anchor.** Memory-refresh accesses at the Chip-memory port come in one
  group per line. Take the first refresh access of each group over 100
  consecutive lines (A6, 2.7, takes every group of its interval). Convert the
  mean spacing to color clocks using 8 `clk_sys` cycles per color clock,
  taken from the design (`nanomig_harness.h` declares no such ratio; section
  5). It must lie between 227 and 228 color clocks (the page's 227.5, to half
  a color clock).
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
  t_1, t_2, ... in time order. t_2, the first read of the first word after
  t_1, is the table's first restart.
- **Anchor check, from the first restart on.** Every read of the table's
  words from t_2 on is the next word after the one read before it, the read
  addresses rising by 2 within a pass, and every later read of the first word
  (t_3, t_4, ...) is immediately preceded, among the channel's reads, by a
  read of the table's last word. What precedes t_2 is not examined: that is
  the first pass.
- **The first pass is not graded.** It is counted and printed as a MEASURED
  line that names NM-HRM-084, which grades it in
  `hrm/ch5-audio-dma-first-pass`. No graded line of this package depends on
  its length or on whether a table switch went through the start-up
  sequence. p. 164 says of DMA start: "Because of pipelining in Agnus, the
  first data word must be thrown away", and the page leaves open whether the
  first word of the first pass is output. p. 145 leaves open whether a
  channel that is disabled and enabled again begins with the start-up
  sequence: "If the channel is disabled for a very short time (less than two
  sampling periods) it may stay on and thus continue from where it left
  off." Which of the two a table switch does depends on how long the CPU
  keeps the channel disabled.
- **Analysis window.** W_T = [s, e), N passes of the table long, entirely
  after the first restart, built from the programmed period and not from the
  DMA reads (2.5, step 1). p. 139: "Each time the system retrieves the last audio
  word from the data area, it uses the contents of these location registers
  to again find the start of the data." The anchor check holds that sentence
  on every restart inside the window.

**F3: disk word index, used by P6.**
p. 247: "As each data word is transferred, the length value is decremented.
After each transfer occurs, the value of the pointer is incremented."

- **Index.** Disk DMA read number k is the read of word address (buffer
  start + 2k).
- **Anchor check.** The reads come in the order k = 0, 1, 2, ..., one per k.

### 2.5 Audio analysis procedures (P1-P4)

The observed output is whichever audio output carries channel 0, not a
fixed named side. The pages disagree on which named jack that is: p. 137,
"Channels 0 and 3 are connected to the right-side output jack", against
Appendix E's serial-connector pin list (p. 326), which pairs "Audio output
from left (channels 0, 3) port" with the same two channels. No proposition
of this package grades which named jack carries channel 0; the side
actually used is measured and recorded per core (section 5, "what the
build adds"). If the harness offers only a mono mix, use the mix: the other
channels are silenced (2.3, and step 3 of the audio program).

The port sample itself is read as a raw magnitude, bits 14-0
(`nanomig_harness.h`'s `AudioSample` documents only "15 bits significant",
not a sign or a scale): run detection below needs only exact equality,
which does not depend on how the value is interpreted, and FIT(L, C)
recovers a continuous, comparable sequence from these raw magnitudes by
unwrapping (below), using only the domain size the harness contract
states, never a chosen sign bit.

**LEVELS(W): the list of held levels in window W.**
1. Start the window on an output transition: its first sample, s, is the
   first sample at or after t_2 whose raw value differs from the sample
   before it. If the output does not change within four held levels of t_2
   (a flat output), s is t_2.
   - **The held level.** One output byte lasts one sampling period (p. 140:
     the period is the time per sample; p. 266: AUD0PER is "period in color
     clocks"), and consecutive bytes with one code form one level. A level of
     table T lasts r_T = u_T x P x c samples of the harness's audio capture,
     which takes one sample per `clk_sys` cycle: P the programmed period
     (128, 2.6), u_T the number of consecutive output bytes that carry one
     code in T's code sequence (the table in 2.6), and c the number of
     `clk_sys` cycles per color clock, 8. A6 (2.7) checks c in the run.
   - **The end.** e = s + (2 N K - 1) x r_T / 2, in the middle of level
     number N x K: N passes of K held levels (p. 139 and p. 145: after
     AUDxLEN words the data is fetched again from AUDxLC), the last level cut
     by e. The pages fix no phase between an audio DMA read and the output it
     feeds (p. 141 gives each channel one DMA slot per line), so the window
     does not end on a read; and it ends in the middle of a level so that
     that phase does not move it across an output transition. Half a level
     is 1/(2 N K) of the window: about 0.8 % for T1 and 1.6 % for T2 and
     T2S, more than the 0.2 % A6's band leaves c, but 0.1 % for T3; T3's
     window holds because its levels measure exactly 2048 samples (section
     5), not because of A6.
   - Take the output samples in [s, e) as the port's raw 15-bit magnitude.
2. Split them into runs of identical consecutive values, and note each
   run's value and duration. Consecutive runs never share a value, by this
   construction.
3. Keep every run, the last one included. It is cut by e, but the value it
   holds is the value of its level and is graded like the others. A5 (2.7)
   grades that the number of runs is the programmed N x K.
4. Unwrap the remaining runs' raw values into a continuous sequence: for
   each step from one run's value to the next, add or subtract whole
   multiples of 32768 (the raw magnitude's own domain) so that the step
   taken is the smallest one available. The values of the result, in time
   order, form the list L.

**FIT(L, C): does the level list match a code sequence?**
C is the sequence of byte codes the page predicts for one pass, in output
order, with consecutive equal codes merged. Its length is K. s_j is the
two's-complement value of byte C[j] (p. 265: "each 2's complement"; p. 138:
"Sample values can range from -128 to +127").

1. For each rotation r = 0 ... K-1, fit L[i] ~ a + b * s_((i+r) mod K) by
   least squares over all i.
2. L matches C when, for some r, every residual |L[i] - a - b * s| is at
   most 1e-6 (an exact digital relation, to floating-point precision --
   not a band the page gives: measured on this RTL, section 5, the best
   rotation's own residual is exactly 0 wherever the level list is
   genuinely the expected code sequence).
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

| Table | Words | Code sequence C | K | N | u (bytes per level) |
|---|---|---|---|---|---|
| T1 | $0010, $2030, $4050, $6070 | C_up = 00 10 20 30 40 50 60 70 (upper byte first) | 8 | 8 | 1 |
| T2 | $0000, $0101, $7F7F, $8080 | C2 = 00 01 7F 80 (equal bytes merge; s = 0, 1, 127, -128) | 4 | 8 | 2 |
| T3 | 256 words; word k = b_k * $0101 with b_k = (k + $80) mod $100, i.e. $8080, $8181, ..., $FFFF, $0000, ..., $7F7F | C3 = 80 81 ... FF 00 ... 7F (s = -128 ... 127) | 256 | 2 | 2 |
| T2S | $8001, $7F00 (distinct bytes: word 1 upper=$80 lower=$01, word 2 upper=$7F lower=$00) | C_sign = 80 01 7F 00 (upper byte first; s = -128, 1, 127, 0) | 4 | 8 | 1 |

For T1 the lower-first order is C_low = 10 00 30 20 50 40 70 60.

T2 and T3 use words whose two bytes are equal, so their results do not depend
on byte order (P2). T1 contains only codes $00-$70, so its result does not
depend on how signs are read (P3). T2S's own words have distinct upper and
lower bytes, both sign extremes ($80, $7F) each appearing once in a fixed
byte position, so P3B (section 3) can tell whether a byte position was
dropped or swapped.

### 2.7 Preconditions shared by P1-P4 and P3B

Each is asserted for the table under test before its proposition is graded.
A failing precondition means "not graded", never green.

| Id | Precondition | How the test checks it | Contrasting case (precondition goes red; the program still finishes) |
|---|---|---|---|
| A1 | Audio DMA for the table ran, pass after pass, from its first restart on, long enough to cover the window | F2's anchor check holds from t_2 on, with at least N + 3 reads of the table's first word (t_1 ... t_(N+3)), and the last of them lies at or after the window's end e (2.5). The length of the first pass is not examined | DMACON = $8200 instead of $8201 (AUD0EN not set): no audio reads |
| A2 | No attach mode: channel 0's data goes to its D/A converter | The stored ADKCONR has bits 7-0 = 0 (p. 265, bits 7-0). Graded once per program that reads the output: the audio program (T1, T2, T3) and T2S's own program | Add ADKCON = $8001 (USE0V1) between the preamble and step 2, before the read. Bit 0 then reads 1. p. 165 predicts channel 0's data goes elsewhere: "AUDxAV Attach volume. Send data to volume latch of next channel instead of to D->A converter" |
| A3 | Held levels are long enough to resolve | The median run in W_T spans at least 4 output samples and equals the programmed held level r_T (2.5) | (1) AUD0PER = 129 for T1 only (the other tables keep 128): T1's held levels last 1032 samples against the programmed 1024, so the median run is not r_T and A3 goes red for T1. (2) A systematic merge (T1 with every pair of adjacent codes equal: $0000, $2020, $4040, $6060): every level lasts twice as long, the median run is 2048 against the programmed 1024, and A3 goes red for T1. A4, A5 and the propositions of T1 are then not graded |
| A4 | The window holds enough levels | The list L from LEVELS(W_T) has at least 2K entries for T1, T2 and T2S, and at least K + 1 for T3. The fitted \|b\|, the output change per LSB in raw port counts, is printed as a MEASURED line and not graded: p. 265 gives the step only as 3 mV at the analog output, and the harness gives only the port's width, so no page or harness statement supplies a minimum number of raw counts per LSB | T1's AUD0VOL is set to 0 seven lines after its DMA starts (p. 266: "000000=no output"), inside the window: the output is flat from there on, the runs before it keep their length (A3 holds) and L has fewer than 2K entries |
| A5 | The window holds the programmed population of held levels, nothing dropped or added | L has exactly N x K entries: N passes of K held levels, the last level kept (p. 139 and p. 145: the table repeats after the programmed word count; p. 140: one period per sample). N and K come from the table (2.6), not from the output | (1) A table with two adjacent equal codes within a pass (T2's own A5 case: word 2 repeats word 1's code) merges what should be two held levels into one; L falls short by one per pass |
| A6 | The color-clock time unit holds in the run | F1's anchor (2.4) holds in the run itself: over the interval from the program's first to its last read of an audio table, the mean spacing of all refresh groups in it is 227-228 color clocks, using 8 `clk_sys` cycles per color clock. The window of 2.5 is built with that unit, so A1 (its last read against the window's end), A3-A5 and the propositions are graded behind A6 | ECS only: before the table is started, VTOTAL = 524, HTOTAL = 113 and BEAMCON0 = $00A0, F1's contrasting case (2.4). Lines then last 114 color clocks and A6 goes red. The OCS configuration has no such registers, and A6 stays green there |

Why A5 counts N x K and builds its window from the programmed period. The
count a window holds follows from its length and the length of a held level.
Taking both from the observed output (the span of the DMA reads and the
median run) leaves the count unable to see anything that changes the output
uniformly: a systematic merge doubles the observed level and halves both the
population found and the population expected. Here the level comes from the
programmed period (2.5, step 1), the window from that level, and the expected
population from the table's N and K, so the output is compared with the
programming and not with itself; a systematic merge already fails A3, which
compares the median run with the programmed level. The window's end is not a DMA read (2.5), so
the unspecified phase between a read and the output transition it feeds does
not enter the count, and the last level is graded like the rest.

---

## 3. Propositions

### P1: Both bytes of every audio data word reach the output, one after the other

1. **Proposition.** Every data word delivered to AUD0DAT is output as its two
   bytes, one after the other, on the digital sample bus this package
   observes. No claim id: 5-appA-p265.c42 (p. 265) bundles this with
   digital-to-analog conversion and delivery to the physical output pins,
   neither of which `nanomig.v`'s ports show; the id is withdrawn (1.2) and
   this proposition grades only its narrower, page-derived part.
2. **Stimulus.** The audio program (2.6) with table T1.
3. **Observation.** The audio output that carries channel 0 (2.5) inside W_T1
   (F2), read as L = LEVELS(W_T1).
4. **Expected relation.** p. 265: "It contains 2 bytes of data that are each
   2's complement and are outputted sequentially (with digital-to-analog
   conversion) to the audio output pins."
   - Graded: FIT(L, C_up) or FIT(L, C_low) matches. Either order counts here;
     the order is P2.
   - Quantifier: "2 bytes ... each ... sequentially" means both bytes of
     every word. The fit covers all 4 words in each of the 8 passes, 32 word
     loads, and fails if either byte is missing. Outputting only the upper
     bytes, for example, gives 00 20 40 60, which matches neither sequence.
5. **Preconditions.** A1-A6 for T1.
6. **Contrasting cases.**
   - Replace T1's last word $6070 by $7060. The page predicts output 00 10 20
     30 40 50 70 60, which matches neither C_up nor C_low. P1 goes red.
     A1-A6 still hold.
   - A wrong value in the last level of the window only. The program waits a
     whole number of lines after it starts the DMA, then writes one byte of
     T1 in memory with the CPU: the byte that the window's last level comes
     from, at an instant between the DMA's read of that word in the
     second-to-last pass and its read in the last pass before e. The test
     measures that byte and instant on a first run of the same program that
     writes the byte's own value. The page predicts that the last level
     follows the changed byte, so L differs from C_up and C_low in its last
     entry alone, and P1 goes red. Every other entry of L, and A1-A6, are
     unchanged.
7. **Reference frame.** F2 (pass boundaries at the reads of the table's first
   word, checked as stated there).

### P2: The upper byte is output first

1. **Proposition.** Of the two bytes of an AUD0DAT word, the upper byte
   (bits 15-8) is output first and the lower byte second.
   - No claim id of this package (5-appA-p265.c42's "outputted sequentially"
     does not itself give an order; P1 already withdraws the id, 1.2). The
     order comes from p. 164, a page that speaks to the same point; it is
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
5. **Preconditions.** A1-A6 for T1, and P1 green. If P1 is red, P2 is not
   graded.
6. **Contrasting case.** Swap the bytes of every word: $1000, $3020, $5040,
   $7060. The page predicts 10 00 30 20 50 40 70 60. C_up does not match,
   so P2 goes red, while P1 stays green (C_low matches). A1-A6 still hold.
7. **Reference frame.** F2, as for P1.

### P3: Each byte is output as a two's-complement value

1. **Proposition.** The output level of each byte follows the byte's
   two's-complement value, so $80 is the most negative and $7F the most
   positive code, at four representative codes (0, 1, 127, -128) with equal
   upper/lower bytes in every word. No claim id: 5-appA-p265.c42 bundles
   digital-to-analog conversion and the output pins, which this package
   cannot observe (1.2, P1); A-audxdat.c5 is declined in full (1.2). This
   proposition, and P3B below, grade only the narrower two's-complement
   relation the pages state.
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
   - Quantifier: "each" means both bytes of every word, but T2's own words
     hold the same code in both byte positions (2.6), so this alone cannot
     tell a genuine two's-complement reading from one where a byte position
     was dropped or swapped; P3B (below) grades that.
5. **Preconditions.** A1-A6 for T2.
6. **Contrasting case.** Flip bit 7 of every byte: $8080, $8181, $FFFF,
   $0000. The page predicts s = -128, -127, -1, 0, with steps proportional to
   (+1, +126, +1, -128). That is exactly the output an unsigned reading of T2
   would give, and FIT(L, C2) fails, so P3 goes red. A1-A6 still hold.
7. **Reference frame.** F2.

### P3B: Each byte is output as a two's-complement value, independent of byte position

1. **Proposition.** $80 reads as the most negative code and $7F as the most
   positive, whichever of the two byte positions carries them, so dropping
   or swapping a byte position changes what is read. No claim id, for the
   same reason as P3.
2. **Stimulus.** Table T2S (2.6): two words, $8001 and $7F00, each with
   distinct upper and lower bytes, run by its own program (`prog_t2s.s`)
   independent of `prog_audio.s` and its own mutations. That program
   also reads ADKCONR and stores it before it starts the DMA (2.6, step 2).
3. **Observation.** L = LEVELS(W_T2S).
4. **Expected relation.**
   - p. 265: "It contains 2 bytes of data that are each 2's complement".
   - Graded: FIT(L, C_sign) matches, with C_sign = 80 01 7F 00 (upper byte
     first, P2's own established order), s = -128, 1, 127, 0.
   - Quantifier: "each" graded at both byte positions with distinct codes:
     unlike T2 (P3), dropping or swapping a byte position here changes which
     code is read from which position, so a match establishes the relation
     holds at both positions, not only when they happen to agree.
5. **Preconditions.** A1-A6 for T2S (2.7's own definitions, applied to
   T2S). A2 applies to T2S's own program: it reads ADKCONR and stores it
   (2.6, step 2), because p. 165 says that with an attach bit set channel
   0's data goes to the volume latch of the next channel "instead of to
   D->A converter", so the output this proposition reads would not carry
   T2S's bytes.
6. **Contrasting cases.**
   - Proposition: swap the bytes of both words: $0180, $007F. The
     page predicts 01 80 00 7F (s = 1, -128, 0, 127); FIT(L, C_sign) fails
     (no rotation of 80 01 7F 00 matches), so P3B goes red. A1-A6 still
     hold.
   - A2: add ADKCON = $8001 (USE0V1) between the preamble and the ADKCONR
     read, as in 2.7. A2 goes red for T2S and P3B is not graded.
7. **Reference frame.** F2, checked on T2S's own window.

### P4: Every one-LSB step moves the output by the same amount

1. **Proposition.** At a fixed volume, every one-LSB step of a data byte
   moves the audio output by the same amount. No claim id: A-audxdat.c5
   (p. 265) is declined in full (1.2); the 3 mV magnitude is not observable.
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
     adjacent codes must equal b, exactly (an exact digital relation, 2.5).
   - Quantifier: every one of the 256 codes and every one of the 255 adjacent
     steps, in both passes; A5 (2.7) grades that L holds the complete
     population (2 x 256 = 512 entries), nothing dropped.
   - Note: an unsigned reading of the bytes also matches this fit (rotated by
     128). P4 grades the step size only; the sign reading is P3/P3B.
5. **Preconditions.** A1-A6 for T3.
6. **Contrasting case.** Replace the word for code +5 ($0505, k = 133) by
   $0707. The page predicts the codes ... 04 07 06 07 08 ..., steps of +3 and
   -1 where +1 is required, and FIT(L, C3) fails, so P4 goes red. A1-A6 still
   hold.
7. **Reference frame.** F2.

### P5: ADKCON bits 8 (FAST) and 9 (MSBSYNC) are separate bits that ADKCONR reads back

1. **Proposition.** Setting or clearing bit 8 or bit 9 through ADKCON shows
   in the same bit of ADKCONR, bit 8 and bit 9 independently. p. 265.
   - No claim id of this package is established here: A-adkcon.b08 and
     5-appA-p265.c21 (bit 8 is FAST) stay declined (1.2) -- G1/G2/G3 in P6
     time the Chip-memory read rate, not the bit-cell time the claim states,
     and no page ties the one to the other; A-adkcon.b09 and 5-appA-p265.c20
     (bit 9 is MSBSYNC) stay declined (1.2) as well, since this package
     cannot drive a disk read. A register readback is a proxy for either
     claim, not the claim itself.
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
   - C1: store R2 twice. The port shows two writes to R2, so C1 goes red.
     The program still finishes.
7. **Reference frame.** None beyond the step order.

### P6: Disk write DMA takes a word per 16 bit cells of about 2 us with FAST = 1 and about 4 us with FAST = 0

1. **Proposition.** With ADKCON bit 8 (FAST) = 1, disk write DMA takes one
   word from memory per 16 bit cells of about 2 us. With FAST = 0, the bit
   cells are about 4 us, twice as long.
   - Claims: A-adkcon.b08, 5-appA-p265.c21 (p. 265). Wording on p. 249 (same
     point, same values). Both stay declined (1.2): no page ties the
     Chip-memory read rate G1, G2 and G3 measure to the bit-cell time, so
     they decline the comparison rather than grade it.
   - The durations below are measurable only if disk write DMA runs without
     a disk image (precondition B3); B3's failure means "not measured", not
     red.
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
   - **No drive is selected.** The program's one CIA write is CLEAR_OVL, a
     single write to CIA-A port A ($BFE001); drive selection is entirely on
     CIA-B port B (p. 241: "CIABPRB ($BFD100) eight output bits for disk
     selection, control and stepping"; p. 244-245: the PB bits, DSKSEL0-3
     and DSKMOTOR; p. 339-340: BFE001's own bits are /FIR1, /FIR0, /RDY,
     /TK0, /WPRO, /CHNG, /LED, OVL, none of them a drive-select line, against
     BFD100's /MTR, /SEL3-0, /SIDE, DIR, /STEP). p. 248 makes the DMA depend
     only on "the DMAEN bit in DSKLEN, and the DSKEN & DMAEN bits in
     DMACON".
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
     - **This gives the time of a word on the disk, not the rate it leaves
       memory.** p. 194 lists "3 cycles for disk DMA" in every line, and lets
       a device leave a slot unused ("If a device does not request one of
       its allocated time slots, the slot is open for other uses"). That the
       slots allow room for about 2 words a line at a 2 us cell bounds the
       memory-read rate from below; it does not make the two equal, and no
       page says when Paula requests a disk word from memory (pp. 380-381
       name only `DMAL`, "DMA request line"). T_F and T_S below are measured
       and printed (MEASURED); G1, G2 and G3 decline to grade them against
       the bit-cell time for this reason.
4. **G1, G2 and G3 are declined, not graded.** p. 265: "08 FAST Disk data
   clock rate control 1=fast(2us) 0=slow(4us)." p. 249: "Value of 1 selects
   two microseconds per bit cell (usually MFM). ... 0 selects four
   microseconds per bit (usually GCR)." Both pages give the bit-cell time of
   the disk data clock on Paula's write-data pin (p. 380), not a
   Chip-memory read rate; `nanomig.v` has no such pin, only the SD-card
   sector interface. G1, G2 and G3 measure the rate at which disk write DMA
   reads words at the Chip-memory port and decline to grade it against the
   bit-cell time, because no page ties the one to the other (1.2, and "16
   bit cells per word" above).
   - **G1, G2:** T_F and T_S are printed as MEASURED lines, not graded
     against a band.
   - **G3:** D_F and D_S are printed as MEASURED lines, not graded against a
     factor of two.
   - **Quantifier.** Both settings, each over 512 consecutive words, in
     every configuration.
5. **Preconditions.** Each grades a page sentence of its own, independent of
   the Chip-memory-read-rate premise G1-G3 decline, and stays graded.
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
   - **G1, G2, G3:** none. Each is a RECORDED decline, not a graded
     comparison, so no stimulus can turn it red or green (1.2, and "16 bit
     cells per word" above).
   - **B1:** in run F, write $0100 instead of $8100. Bit 15 = 0 clears
     instead of sets, bit 8 reads 0, and B1 goes red.
   - **B2:** in run F, write DMACON = $8010 instead of $8210 (DMAEN, bit 9,
     clear; DSKEN, bit 4, stays set). p. 247's step 1 already asks to
     "enable disk DMA in the DMACON register"; p. 248 makes DMAON depend on
     "the DSKEN & DMAEN bits in DMACON" and p. 278 mirrors "Bit09 (DMAEN) in
     DMACON" into the same bit -- both pages agree that clearing DMACON's
     own DMAEN makes DMAON read 0, so B2 goes red. B3 for run F goes red as
     well (no reads), since no disk DMA runs at all with the master enable
     clear; that is this contrasting case's own necessary side effect, not
     a separate defect. The DSKBLK poll times out, and the program finishes.
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
  starts. P1-P4 and P3B avoid the first pass (F2).
- **Precision of the disk values.** The pages print the bit-cell times as
  "2us"/"4us" and "two"/"four microseconds" with no further precision. P6's
  G1-G3 do not grade these values at any precision: no page ties the
  measured Chip-memory read rate to the bit-cell time, so there is nothing
  of the page's own to compare it against (1.2, 3).

## 5. What the build adds

Registers this package's programs use beyond `integration/harness/nanomig.i`
(`ADKCONR`, `DSKBYTR`, `DSKPTH`/`DSKPTL`, `DSKLEN`, `ADKCON`, `AUD0LCH`/
`AUD0LCL`/`AUD0LEN`/`AUD0PER`/`AUD0VOL`, `AUD1VOL`-`AUD3VOL`, `HTOTAL`,
`VTOTAL`, `BEAMCON0`) are defined in the package's own `ch5regs.i`, offsets
from Appendix B. Table/buffer content (T1, T2, T3 and the two disk buffers)
is poked into chip RAM directly by the C++ test before `boot()`, the same
mechanism `load_program()` itself uses; no program instruction writes it.

Every item below is a labelled measurement, on both cores unless stated
otherwise.

- **Chip bank 0.** T1/T2/T3 and the two disk buffers sit below $080000
  (2.1), inside the range the boot-time Kickstart overlay otherwise
  redirects. Both `prog_audio.s` and `prog_diskrate.s` start with a single
  write of $00 to $BFE001 (CIA-A port A, `CLEAR_OVL` in `ch5regs.i`).
  Measured: this makes that range real memory for both the CPU's own
  accesses and the audio/disk DMA engines'. CIA-A port A carries no
  drive-select line (those are CIA-B port B, p.241, p.244-245, p.339-340),
  so P6's "no drive is selected" condition does not depend on this write.
- **Time base.** F1's anchor, measured inside the interval it is used in
  (P6's own graded interval, read k=16 to k=528 of each run), comes out at
  227.00 color clocks per line on both cores, every configuration, inside
  the page's 227-228 window. 8 `clk_sys` cycles per color clock (`kCck`) is
  taken from the design, since `nanomig_harness.h` declares no such ratio;
  B4, measured in that same interval, guards it. In the audio runs A6 does
  the same over the interval of each program's own audio reads, averaging
  every group of it: 227.00 color clocks, ECS/PAL and OCS/PAL, on both cores
  (prog_audio: 1786 groups on fx68k, 1681 on TG68K; prog_t2s: 254 and 237).
- **Audio port scale.** `nanomig_harness.h` documents only the width of
  `audio_left`/`audio_right` ("15 bits significant"), not their sign or
  scale. LEVELS(W) (2.5) reads the raw magnitude and recovers a continuous
  sequence by an unwrap (add/subtract multiples of 32768, the raw domain,
  choosing the smallest step at each point) rather than a chosen sign bit,
  so FIT(L, C) does not depend on which raw bit, if any, is a sign bit.
  Measured: this unwrap and a fixed 15-bit two's-complement reading (bit 14
  as sign) agree exactly on this RTL, on every table and every core, since
  no held level's raw magnitude ever approaches the domain's own wraparound
  point.
- **Audio output side.** Measured per core and per config: channel 0's own
  byte sequence appears on `audio_left`, with `audio_right` staying flat
  throughout. The pages disagree on which named jack that is (2.5); this
  package observes whichever side actually carries channel 0 (measured at
  runtime, not fixed in source) and does not grade which named jack it is.
- **F2's anchor and the first pass (2.4).** The anchor is checked from t_2,
  each table's first restart, on; what precedes t_2 is the first pass. Per
  table and per core the test prints the first pass's length as a MEASURED
  line that names NM-HRM-084. It is not graded and not counted in
  passed or failed, so P1-P4, P3B and A1, A3-A5 do not depend on it.
  Measured, ECS/PAL and OCS/PAL alike: reads of the table's words before
  t_2, against the table's length. T1: 3 of 4 on both cores. T2: 4 of 4 on
  both. T3: 255 of 256 on fx68k, 256 of 256 on TG68K. T2S (its own program,
  `prog_t2s.s`, run from a clean boot independent of T1/T2/T3): 2 of 2 on
  both. With the anchor taken from t_2, A1 holds for every table on both
  cores, and P1-P4 and P3B are graded on every run.
- **The window and A5's population (2.5, 2.7), measured per table.** The
  window starts on the first output transition after t_2 and ends in the
  middle of level number N x K. On both cores, ECS/PAL and OCS/PAL alike:
  T1: 65024 samples and 64 runs, all 1024 samples long but the last (512);
  T2: 64512 samples and 32 runs, all 2048 long but the last (1024); T3:
  1047552 samples and 512 runs, all 2048 long but the last (1024); T2S:
  32256 samples and 32 runs, all 1024 long but the last (512). Each window
  holds the programmed N x K levels (64, 32, 512, 32), and the held level
  of the programmed period (u x 128 x 8: 1024 or 2048 samples) is the
  median run.
- **G1/G2/G3 (P6): declined, not a page disagreement.** ADKCON bit 8 (FAST)
  reads back correctly (B1 holds on both runs, every configuration, both
  cores). Measured D_F equals D_S exactly (310520 `clk_sys` cycles for 512
  words, both runs, every configuration, both cores), giving T_F = T_S ~=
  1.336 us (PAL): the measured Chip-memory read rate does not change with
  the FAST bit. This is printed as a MEASURED line, not graded: G1, G2 and
  G3 decline to compare it with the bit-cell time, because no page ties a
  Chip-memory read rate to that time (1.2, 3).
- **P1's last-level contrast (3, P1).** The first run of prog_audio under
  that selector writes the byte's own value; from it the test takes the
  window's last level (code 7 of C_up, the lower byte of T1's word 3), the
  DMA's last two reads of that word (cycles 2337113 and 2346193 on both cores
  and both configurations) and the line count to wait (37). The CPU write of
  the second run comes at cycle 2341125 on fx68k and 2340965 on TG68K, between
  the two reads. P1 goes red and exactly one entry of L, the last, differs
  from the level one pass earlier.
- **A3's contrasting cases.** With AUD0PER = 129 for T1 only, T1's median run
  is 1032 samples against the programmed 1024 and A3 goes red for T1, on both
  cores in ECS/PAL and OCS/PAL; A4, A5, P1 and P2 of T1 are not graded, and T2
  and T3 are unchanged. The systematic merge (T1 with every pair of adjacent
  codes equal) gives a median run of 2048 and also fails A3.
- **A4's contrasting case.** With T1's AUD0VOL set to 0 seven lines after its
  DMA starts, the window holds 5 runs on fx68k and 4 on TG68K in both
  configurations, the median run is still 1024 (A3 holds) and A4 goes red for
  T1.
- **A6's contrasting case.** With VTOTAL, HTOTAL and BEAMCON0 rewritten before
  the tables start (ECS only), the refresh groups recur 114.00 color clocks
  apart in prog_audio's and prog_t2s's runs on both cores, and A6 goes red in
  ECS/PAL; OCS/PAL has no such registers and A6 stays green.
- **Program placement.** The harness's own `PROGRAM_ORG` ($080400) and
  `RESULTS` ($0C0000) sit above $080000, outside the range 2.1 asks the
  DMA-referenced tables and buffers to stay under. Harmless for the stated
  reason: neither the program's own code nor its result block is read by a
  pointer-width-limited DMA channel, only the CPU itself.
- **Mutation scope.** Every mutation is replayed on both cores in every
  configuration its own assertion or precondition is graded in (2.1); the
  F1/B4 contrasting case is ECS only, per 2.4. The normal run never runs a
  mutant.
- **Fixture lines (2.3).** Four kinds of line carry no claim: FIXTURE (the program
  reached the done marker), FIXTURE-CONST (its known constant reads back),
  FIXTURE-DATA (every read of a table or buffer on the Chip-memory port
  returns the poked content) and FIXTURE-WRITE (each stored result slot
  shows one CPU write). Measured, both cores: every one of them passes on
  the normal run, 126 passed and 0 failed. Each has a selector that makes
  exactly its own lines print FAIL through the unchanged comparator and
  moves to RECORDED "not graded" the lines graded behind it. NODONE
  replaces `prog_audio` by a program that never writes the done marker:
  FIXTURE red in ECS/PAL and OCS/PAL on both cores. FIXCONST makes every
  program store a different constant: the nine FIXTURE-CONST lines red;
  A2 (prog_audio and T2S), and through it A3-A5 and P1-P4 of T1, T2 and T3,
  A3-A5 and P3B of T2S, C1, P5, and B1 and B2 of both disk runs, not graded;
  A6 of both programs, which reads no result slot, stays graded and green.
  FIXDATA pokes one word of each table and
  both buffers other than the expected content: 14 FIXTURE-DATA lines red
  (T1, T2, T3, T2S, both buffers); A4, A5 and the propositions of the four
  tables not graded, A1, A2, A3 and A6 still graded and green. FIXWRITE leaves out
  the ADKCONR store into A2's slot of prog_audio and of prog_t2s and run F's
  store into B1's slot: the seven FIXTURE-WRITE lines of those slots red,
  A2 (both programs) and B1 for run F (and what A2 gates) not graded.
- **F1/B4's own contrasting case, measured inside the P6 interval.**
  Rewriting VTOTAL, HTOTAL and BEAMCON0 (2.4) halves the refresh-group
  spacing measured there to 114.00 color clocks on both cores, both ECS
  configurations, so B4 goes red as the page predicts. Disk write DMA's own
  rate halves under the same stimulus (both runs, both cores): D_F and D_S
  each fall from 310520 to 155936 `clk_sys` cycles.
