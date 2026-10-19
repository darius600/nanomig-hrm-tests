# Audio DMA at and below the minimum period: specification addendum

This addendum specifies five propositions about audio DMA at, above and below
the minimum sampling period that p.141 gives, in NTSC and in PAL. Its inputs
are the twelve claims in `claims-spec-stage.jsonl` (all printed on p.141) and
these pages of the *Amiga Hardware Reference Manual*, 3rd edition: pp.23, 24,
34 and 40 (beam position and field length), pp.137-145, 147-148, 156-159 and
164-166 (Chapter 5, audio), pp.194-195 (Chapter 6, DMA time slots and
Figure 6-9), pp.265, 266, 278, 280 and 285 (Appendix A), pp.290-291
(Appendix B) and pp.304 and 310 (Appendix C). The addendum is
self-contained: its data, program, reference frames, preconditions and
contrasts are its own, and it changes nothing in the audio package it is
added to. It specifies only what the
claims support. Quotations are the printed text.

## 1. Claims not turned into a proposition

None. Every claim is used:

| Claim | Proposition |
|---|---|
| sol:5-p141.c9, opus:5-rate.c14 | MP1 |
| sol:5-p141.c4, opus:5-rate.c8 | MP1; the MP2 sub-results |
| sol:5-p141.c10, opus:5-rate.c15 | MP2 |
| sol:5-p141.c11, opus:5-rate.c17 | MP3, MP4 |
| sol:5-p141.c12, opus:5-rate.c18 | MP3, MP4 |
| sol:5-p141.c3, opus:5-rate.c6 | MP5; the MP2 sub-results |

Parts of these claims that no check here covers, and why:

- **"In order to save buffers"** (c4, c8): the page's reason for the design
  rate. No output shows a reason.
- **31,469 as a rate that is reached** (c3, c6): the page calls it "a
  theoretical maximum" and says in the next sentence that "the hardware is
  designed to handle 28,867 samples/second". No page predicts that any output
  reaches 31,469 samples/second, so nothing checks that it is reached. Only
  the bound is checked (MP5), and only in NTSC, because the claim's figure is
  built from NTSC numbers (262.5 lines/frame, 59.94 frames/second). The page
  gives no PAL figure.
- **Periods 429 to 65535** inside "at least 124" (c9, c14) and "at least 123"
  (c12, c18): the propositions cover them, but they are not in the required
  set (section 3.2). One run there, with its settling time and a window of
  256 sample periods, costs from about 0.05 s of simulated time (at 429) to
  about 7 s (at 65535). The check establishes "at least" on the tested
  values only.
- **Period 0** inside "below 124" (c10, c15): excluded. The pages do not say
  what the value 0 does. p.140 gives "the maximum is 65535"; p.159 speaks of
  "its full range 124-65536".
- **How often the previous sample is reused** below the minimum (c10): the
  page does not say. Only "at least once in the window" is graded; the rate
  is reported, never graded.
- **What happens to the sample that arrives too late** (c10, c15): the page
  says the previous sample is reused. It does not say whether the late
  sample is then played or lost. Forward jumps ("skip", section 3.6) are
  counted below the minimum but not graded.

## 2. The propositions at a glance

| Id | Standard | Periods (AUD0PER) | Expected in the window |
|---|---|---|---|
| MP1 | NTSC | the set S_hi (124 and up) | every sample period carries the next data sample |
| MP2 | NTSC | every value 1-123 | at least one sample period does not carry the next sample, and at least one reuses a sample already played |
| MP3 | PAL | 123 and the set S_hi | as MP1 |
| MP4 | PAL | every value 1-122 | as MP2 |
| MP5 | NTSC | every value 1-113 | no line holds more than one data fetch of the channel |

## 3. Common setup

### 3.1 Data

One table of 64 words (128 bytes) at a word-aligned address `T` in Chip
memory, with `T` below $80000, so that it is reachable through the 18-bit
location of p.266 as well as the ECS one (p.290: "high 3 bits, 5 if ECS").
Byte `k` (k = 0 ... 127) at byte address `T+k` holds the signed value
`2k - 127`: -127, -125, ..., +125, +127. Every byte value is different from
every other one and none is 0.

- p.138: "The data is stored in byte address order with the first digitized
  amplitude value at the lowest byte address, the second at the next byte
  address, and so on. Also, note that the first byte of data must start at a
  word-address boundary." and "the data address to which AUD0LC points must
  be somewhere in chip RAM."
- Sample values lie in the range p.138 gives: "Sample values can range from
  -128 to +127."

The table must not overlap the program, its parameter words or its result
block. No other DMA and no CPU access touches the table after the program
has written it (checked by MC4).

### 3.2 Runs

- One run plays one period value `P` on audio channel 0. The program is the
  same in every run; only the constants `P` and `N_run` (section 3.3) and
  the variant switches of the contrasts change.
- The harness selects the standard (NTSC or PAL). The program does not write
  BEAMCON0. The Enhanced Chip Set stays selected and Chip memory stays at
  2 MB, as by default; the claims make no OCS/ECS distinction and p.310 names
  a different AUDxPER behaviour only "with programmable scan rates", which
  are not used here.
- Each run is made once on each CPU core (the 68000, and the second core in
  its default 68000 mode). The expected values are the same on both. The
  program waits by beam position, not by instruction timing, so it does not
  depend on the core's speed.
- **Required period values:**
  - `S_hi` = {124, 125, 126, 127, 128, 129, 130, 131, 140, 160, 200, 256,
    320, 428}. 124-131 are the values next to the boundary. 256, 320 and 428
    are periods the chapter itself uses (p.157 Table 5-6, p.158).
  - NTSC: every `P` from 1 to 123, every `P` in `S_hi`, and the reference run
    `P_ref = 447`.
  - PAL: every `P` from 1 to 122, `P = 123`, every `P` in `S_hi`, and the
    reference run `P_ref = 447`.
  - `P_ref = 447` is the period of the worked example on p.143. It is far
    above both minimums. It calibrates the measurement (MC3) and is graded by
    no proposition.
- That makes 138 NTSC runs and 138 PAL runs, plus the contrast variants of
  sections 3.7 and 4. The values 1-123 and 1-122 are each required, because
  the claims say "below 124" and "at least 123": one value does not establish
  every value.

### 3.3 Program

Register names and addresses are those of Appendix B (pp.290-291) and
Appendix A.
`CUSTOM` = $DFF000. The steps follow the order of the audio summary on
p.145 (location, length, volume, period, then start the DMA).

1. `INTENA` ($09A) <- $7FFF and `INTREQ` ($09C) <- $7FFF: clears every
   interrupt enable and request (p.280: bit 15 SET/CLR "Determines if bits
   written with a 1 get set or cleared").
2. `DMACON` ($096) <- $7FFF: clears every DMA enable (p.278, same SET/CLR
   rule). Copper, bitplane, sprite, blitter and disk DMA stay off for the
   whole run.
3. `ADKCON` ($09E) <- $00FF: clears bits 7-0, the USExVy and USExPy attach
   bits (p.265), so that no channel modulates another.
4. `AUD1VOL` ($0B8), `AUD2VOL` ($0C8), `AUD3VOL` ($0D8) <- 0. p.266: bits
   05-00 "000000=no output". Channels 1-3 stay disabled.
5. Write the table of section 3.1 at `T`.
6. `AUD0LCH` ($0A0) / `AUD0LCL` ($0A2) <- `T`, with one `MOVE.L` to
   `AUD0LCH`. p.139: "Since these two register addresses are contiguous,
   writing a long word into AUDxLCH moves the audio data address into both
   locations."
7. `AUD0LEN` ($0A4) <- 64. p.139: "The length of the data is the number of
   samples in your waveform divided by 2, or the number of words in the data
   set."
8. `AUD0VOL` ($0A8) <- 64 ($40). p.266: bit 06 "Forces volume to max".
9. `AUD0PER` ($0A6) <- `P`. p.143: "To set the period register, you must
   write the period value into the register AUDxPER".
10. `DMACON` <- $8201 (SET/CLR, DMAEN, AUD0EN). p.144: "When both DMAEN and
    AUDxEN are set, the DMA will start for channel x."; p.145 step 7.
11. Read `DMACONR` ($002) and store it at `R+0`; read `ADKCONR` ($010) and
    store it at `R+2`. `R` is a result block in Chip memory outside the
    table.
12. Wait `N_run` lines. The program polls `VPOSR` ($004) and `VHPOSR`
    ($006): the vertical position is V8 = `VPOSR` bit 0 and V7-V0 =
    `VHPOSR` bits 15-8 (p.285). It reads `VPOSR`, `VHPOSR`, `VPOSR` and
    repeats the triple if the two V8 bits differ. Each change of the
    vertical position counts one line. It keeps the largest vertical
    position seen. It stores that maximum at `R+4` and the number of lines
    counted at `R+6`.
13. `DMACON` <- $0001: stops channel 0 (p.145: "You can stop the channel by
    writing a 0 into the AUDxEN bit at any time.").
14. Store the done marker $A5C3 at `R+8`. Then loop on itself.

Run length. With `W(P) = max(60 000, 256 * P)` ticks (the window, section
3.5) and `T_settle(P) = max(128 * P, 14 600)` ticks:

`N_run = max(330, ceil((T_settle(P) + W(P)) / 227) + 16)` lines.

`128 * P` ticks is one pass through the table at period `P`. 14 600 ticks
(64 lines) is the length of one pass if no more than one word arrives per
line. 330 lines is more than the longest field (313 lines, p.24), so the wait
always crosses at least one field end (MC1). These numbers only size the run.
If a run turns out too short, MC5 reports it as incomplete; it is never
graded on a shorter window.

### 3.4 What the test records

- **Chip-memory port:** every access with its time in system clock cycles,
  its word address, read or write, and whether it was a DMA fetch (refresh
  told apart from the other DMA) or a CPU access.
- **Audio output:** the right output, which carries channels 0 and 3 (p.137:
  "Channels 0 and 3 are connected to the right-side output jack."), and the
  left output, which carries channels 1 and 2. The test needs every output
  sample's time on the same system-clock time base as the Chip-memory port:
  either as a time stamp or as a fixed number of system clock cycles per
  output sample, counted from a known start. If the harness gives one mixed
  output instead of two, the same rules apply to it, because channels 1-3
  are at volume 0. The test reads the levels as the harness delivers them.
  They must not have passed a low-pass filter (p.156: "The system includes
  a low-pass filter"). MC3 shows whether that holds.
- **Result block** `R+0` ... `R+9`, read after the run.
- **Output resolution `s`:** the spacing of output samples, in ticks
  (section 3.5, F1). A period `P` can be graded at the output only if
  `s <= P/2`. If it is not, MP1-MP4 report that value as "not checkable:
  output resolution", never as green. MP5 does not use the output.

### 3.5 Reference frames

**F1. Tick.** One tick is one count of the period register, which is one
color clock.

- p.266: "The minimum period is 124 color clocks."
- p.140: "In units, the period value represents clock ticks per sample."
- p.141: "In all of these calculations "ticks" and "timing intervals" refer
  to the same thing." Clock interval 0.279365 microseconds NTSC, 0.281937
  PAL (p.141 table "Clock Values").
- p.194: "there are 227.5 "color clocks", or memory access cycles. A memory
  cycle is approximately 280 ns in duration."

*Measurement.* In the reference run (`P_ref = 447`) of each standard, `D_ref`
is the mean duration of the right output's level runs over 256 consecutive
runs inside the window (section 3.5, F5). One tick = `D_ref / 447` system
clock cycles.

*Anchor checks* (part of MC3):
(a) every one of those 256 level runs lasts `D_ref` to within one output
sample spacing;
(b) at the port, the mean spacing of consecutive table fetches over whole
passes of the same window is `2 * D_ref` to within 0.5 %. Basis: each word
holds two samples (p.138: "the audio DMA retrieves one word (16 bits) at a
time and uses the sample it reads as two bytes of data"), and each word is
fetched once per pass (p.139, p.165: "AUDxDR Audio DMA request to Agnus for
one word of data");
(c) the line length of F4, in ticks: NTSC lines alternate between 228 and
227 ticks to within 0.2 tick, and their mean is 227.5 to within 0.2 tick
(p.23: "Every other line is a long line (228 color clocks, 0-$E3), with the
others being 227 color clocks long."; p.194: 227.5). PAL: the mean line
length lies between 226.8 and 227.7 ticks. The two pages that state it
give different values, and this range takes in both: p.23 says "In PAL,
they are all 227 long", and p.304 lists "PAL (625 lines, 227.5 colorclocks
per scan line)".

**F2. Level cycle.** In the reference run, the right output shows the table
played in order. The 128 distinct levels that appear, in the order in which
they follow one another, form the level cycle. Give the first level of the
reference window index 0 and number the others along the cycle (0 ... 127).
`succ(i) = (i + 1) mod 128`. The test does not need to know which level is
byte 0, nor whether a higher byte value gives a higher or a lower level:
only the order along the cycle is used. In every other run, a level is
mapped to the index it has in the cycle of the same standard. A level that
is not in the cycle is "unmapped".

**F3. Sample-period grid.** p.140: "each time the register counts down to 0,
another sample is retrieved from the waveform data set for output". p.164:
"Transitions such as 010->011->010 occur whenever the period counter counts
down to one. The period counter is reloaded at these transitions." So in a
run at period `P`, the right output can change level only on a grid of
spacing `P` ticks. The grid is `t0 + k * P` (k = 0, 1, 2, ...), with `t0` =
`t_start`, the first level change of the window (F5). MC6 requires every
level change inside the window to lie within one output-sample spacing `s`
of a grid point. The window is cut into consecutive sample periods of `P`
ticks starting at `t0`. Each sample period carries the index of the level
the output shows in it.

**F4. Line.** Figure 6-9 (p.195) draws each horizontal line beginning with
the memory refresh slots, followed by the disk, audio and sprite slots, with
one audio slot per channel in a fixed position. p.194: "4 cycles for memory
refresh" per line. At the port, a refresh cluster is a run of refresh
accesses with gaps of no more than 4 ticks. The start of a line is the first
access of a cluster. A line runs from one cluster start to the next.
*Anchor checks* (part of MC4): every cluster holds 4 refresh accesses; every
channel-0 table fetch lies at the same offset, in ticks, from the start of
its line (Figure 6-9: a fixed slot per channel).

**F5. Windows.**

- *Output window:* `t_first` is the time of the first right-output sample
  whose level is in the level cycle. In the reference run, which defines
  the level cycle, `t_first` is the first change of the right output after
  step 10. `t_start` is the first right-output
  level change at or after `t_first + T_settle(P)`. The output window is
  `[t_start, t_start + W(P))`, which holds `N = floor(W(P) / P)` sample
  periods, at least 256. Waiting `T_settle` keeps the start of the DMA out
  of the window. p.164 says that at the start "Because of pipelining in
  Agnus, the first data word must be thrown away"; none of these claims
  speaks to the start.
- *Port window:* from the first table fetch at or after (the first table
  fetch of the run) `+ T_settle(P)`, for `W(P)` ticks. MP5 counts the
  complete lines (F4) inside it.
- *Table pass at the port:* a fetch of `T` that follows a fetch of `T+126`
  is a wrap. p.139: "Each time the system retrieves the last audio word from
  the data area, it uses the contents of these location registers to again
  find the start of the data."

### 3.6 Sample-period classification

Let `c_1 ... c_N` be the indices that the window's sample periods carry
(F2, F3), or `u` where a sample period is unmapped. Set `m = c_1`. `m` is the
furthest sample reached so far; `c_1` itself is not classified. (If `c_1` is
unmapped, count it as unmapped and start from the first mapped sample
period.) For `j = 2 ... N`:

| Condition (indices mod 128) | Class | `m` after |
|---|---|---|
| `c_j = u` | unmapped | unchanged |
| `c_j = m + 1` | **next** | `c_j` |
| `c_j = m` or `c_j = m - 1` | **reuse** | unchanged |
| `c_j = m + k`, 2 <= k <= 63 | skip | `c_j` |
| anything else | other | unchanged |

The definition of **reuse** is what both pages that describe the effect
agree on. p.141 says "the previous sample will be reused", which is the
sample just played (`c_j = m`). p.164 says "If the sampling rate is set much
higher than the normal maximum sampling rate (approximately 29 KHz), the two
samples in the buffer register will be repeated", which are the two samples
of the last word (`m - 1`, `m`). Both describe a sample period that carries
one of the two samples played last instead of the next one. The table has
128 different values (section 3.1), so such a repeat cannot be taken for a
new sample.

Each run reports `N` and the number of sample periods in each class.

### 3.7 Preconditions

Every graded run asserts these first. If one fails, the run is reported as
"precondition failed" with its name, never as green or red for a
proposition. Each precondition has a contrast: a change of the stimulus for
which the pages predict that this precondition fails while the program still
finishes and the other preconditions hold. A contrast run is graded only for
the precondition it is the contrast for.

**MC1. Standard.** The largest vertical position the program recorded
(`R+4`) is 261 or 262 in NTSC runs and 311 or 312 in PAL runs. p.24: "There
are actually 262 NTSC (312 PAL) possible vertical positions." and "In NTSC,
the fields are 262, then 263 lines and in PAL, 312, then 313 lines."; p.40:
"Each complete set of lines (262/NTSC or 312/PAL) is called a display
field."; p.34: "PAL has 312 lines normally".
*Contrast:* make an NTSC-declared run with the harness set to PAL, and a
PAL-declared run with the harness set to NTSC. `R+4` falls into the other
set, so MC1 fails.

**MC2. Configuration read back.** `DMACONR` (`R+0`) bits 9-0 equal the DMA
enables the run declares; in every main run that is DMAEN (bit 9) and AUD0EN
(bit 0) only (p.278 bit table). `ADKCONR` (`R+2`) bits 7-0 are 0 (p.265).
Bits 15-10 of `DMACONR` (blitter status, BLTPRI, unused) are not compared.
*Contrast (DMACONR):* a variant also sets up channel 3 on a second 64-word
table `T3`, which does not overlap `T`: `AUD3LCH/AUD3LCL` <- `T3`,
`AUD3LEN` <- 64, `AUD3PER` <- 447, `AUD3VOL` <- 0, and in step 10 writes
`DMACON` <- $8209, while the declaration stays DMAEN + AUD0EN. Bit 3 reads
1, so MC2 fails. Channel 0 keeps its own slot (Figure 6-9). Channel 3's
output is "no output" (p.266). Its fetches lie outside `T`.
*Contrast (ADKCONR):* a variant writes `ADKCON` <- $8080 after step 3, which
sets USE3PN, "Use audio channel 3 to modulate nothing" (p.265). Bit 7 reads
1, so MC2 fails. Channel 0 is not involved.

**MC3. Measurement calibrated (reference run, per standard).** In the
`P_ref = 447` run:

1. the left output is constant throughout the window;
2. the right output shows exactly 128 distinct levels, which repeat with a
   period of 128 level runs, each level once per period (F2);
3. the anchor checks F1 (a), (b) and (c) hold;
4. `s` is known (F1).

This is what p.140 defines a period to be ("The sampling period specifies
the number of system clock ticks, or timing intervals, that should elapse
between each sample (byte of audio data) fed to the digital-to-analog
converter"). It sits at a period well above both minimums, where no claim of
this addendum is at stake.
*Contrast (levels):* the reference run with `AUD0VOL` <- 0. p.266:
"000000=no output". No level cycle appears, so MC3 fails.
*Contrast (time scale):* the reference run with `AUD0PER` <- 894 and the
declaration left at 447. The tick comes out twice as long, so F1 (c) measures
lines of about 113.75 ticks (NTSC) or 113.5 ticks (PAL), and MC3 fails.

**MC4. The channel plays the table, and nothing else touches it.** Inside
the port window:

- the DMA reads at channel 0's slot offset (F4) all lie in the range
  `T ... T+126` and follow the address order `T`, `T+2`, ..., `T+126`, `T`,
  ... without a gap, and the window holds at least one wrap (F5). p.148 b:
  the back-up location register "will be used as a pointer showing current
  data word to fetch"; p.147: "Each time the length
  counter reaches zero, both the location and length registers are reloaded
  with the same values to continue the audio output."
- there is no CPU access to that range after step 5;
- the anchor checks of F4 hold.

Channel 0's slot offset is the constant offset of the reads in the range
`T ... T+126`. The test assigns a fetch to channel 0 by its address. That is
sound because only channel 0 is set up to read `T` (MC2 shows that only
AUD0EN is on).
*Contrast (address order):* a variant writes `AUD0LEN` <- 65 while the
declared table stays 64 words, and stores the bytes 0 and 2 (not table
values) in the word at `T+128`. p.145 step 4: the length register holds "the
number of data words to be retrieved before starting at the address
currently in AUDxLC". So each pass now also fetches `T+128` at channel 0's
offset, between `T+126` and `T`, and MC4 fails.
*Contrast (CPU access):* a variant reads the word at `T+64` once (`MOVE.W`)
in the middle of the wait. A CPU access appears in the range, and MC4 fails.
The audio DMA is not delayed by it. p.194: "Disk DMA, audio DMA, display
DMA, and sprite DMA all have the highest priority level."

**MC5. Complete run.** `R+8` holds $A5C3. `R+6` equals `N_run`. Channel-0
table fetches occur in the first and in the last tenth of the port window's
lines. The last table fetch comes after the end of the output window.
*Contrast (marker):* a variant leaves out step 14's store. MC5 fails.
*Contrast (channel stopped early):* a variant writes `DMACON` <- $0001
after 100 lines of the wait and then completes the full wait of `N_run`
lines. No table fetch occurs in the last tenth of the port window, so MC5
fails. The wait still crosses a field end (MC1). In this variant, MC6 is
judged only up to the last table fetch.

**MC6. Period in effect, and output resolved.** `s <= P/2`, and every level
change inside the window lies within `s` of the grid of F3.
*Contrast:* the `P = 128` run with `AUD0PER` <- 129 written while 128 is
declared. The level changes drift by one tick per sample period against the
128-tick grid, so MC6 fails.

## 4. Propositions

### MP1. NTSC: at the minimum period of 124 and above, every next sample is retrieved in time

1. **Proposition.** In NTSC, with any period value of at least 124 in the
   period register, the audio DMA retrieves every next data sample before it
   is due, so every sample period carries the next sample of the table.
   Claims sol:5-p141.c9, opus:5-rate.c14, sol:5-p141.c4, opus:5-rate.c8;
   p.141.
2. **Stimulus.** The program of section 3.3, NTSC, once for each `P` in
   `S_hi`.
3. **Observation.** The right output in the output window (F5), cut into
   sample periods (F3) and classified (section 3.6). The claim concerns a
   channel that is playing its data at period `P`. The test knows that this
   holds in the window from MC4 (the fetches run through the table), MC6
   (the output changes on the `P` grid) and the window start after
   `T_settle`.
4. **Expected.** For every `P` in `S_hi`, all `N - 1` classified sample
   periods are **next**: no reuse, skip, unmapped or other. From p.141:
   "Therefore, a value of at least 124 must be written into the period
   register to assure that the audio system DMA will be able to retrieve the
   next data sample." and "In order to save buffers, the hardware is
   designed to handle 28,867 samples/second." (3,579,545 / 124 = 28,867
   samples/second.)
   Quantifiers: "at least 124" is graded on every value of `S_hi`, each on
   its own; "the next data sample" is graded on every sample period of the
   window, not on one.
5. **Preconditions.** MC1-MC6 (MC3 from the NTSC reference run).
6. **Contrast.** The same run with `P = 123`. p.141 predicts reuse there
   (MP2), so the MP1 check goes red. A second contrast is `P = 64`. Both runs
   are already in the required set, and the setup and preconditions are
   unchanged.
7. **Reference frames.** F1, F2, F3, F5. F1 and F2 are checked by MC3, F3 by
   MC6, the window start by MC4 and MC5.

### MP2. NTSC: below 124, the next sample is not retrieved in time and the previous sample is reused

1. **Proposition.** In NTSC, with any period value from 1 to 123, the audio
   DMA does not always retrieve the next data sample before the count
   reaches 0, and a sample already played is output again. Claims
   sol:5-p141.c10, opus:5-rate.c15; p.141.
2. **Stimulus.** The program of section 3.3, NTSC, once for each `P` from 1
   to 123.
3. **Observation.** As MP1.
4. **Expected.** For every `P` from 1 to 123, separately:
   (a) at least one classified sample period is not **next**
   (opus:5-rate.c15 and sol:5-p141.c10); and
   (b) at least one is **reuse** (sol:5-p141.c10).
   From p.141: "If the period value is below 124, by the time the cycle
   count has reached 0, the audio DMA will not have had enough time to
   retrieve the next data sample and the previous sample will be reused."
   Quantifiers: "below 124" is graded on every value 1-123. The page gives
   no rate, so only "at least one in the window" is graded. The numbers of
   next, reuse, skip, unmapped and other periods are reported. Skips are not
   graded (section 1).
   *Sub-results*, reported separately with the same expected value:
   **MP2-lo**, `P` = 1-113: a channel at these periods needs more than the
   "theoretical maximum" of 31,469 samples/second (sol:5-p141.c3,
   opus:5-rate.c6; 3,579,545 / 31,469 = 113.75 ticks per sample). p.164
   points the same way for very high rates.
   **MP2-hi**, `P` = 114-123: these lie between that theoretical figure and
   the designed rate of 28,867 samples/second (sol:5-p141.c4,
   opus:5-rate.c8). Here the expected reuse rests on the quoted sentence
   alone. A red result only in MP2-hi touches "the hardware is designed to
   handle 28,867 samples/second" specifically.
5. **Preconditions.** MC1-MC6. MC6 matters most here. If the written period
   does not take effect, the sample periods are not those of `P`, and a
   channel running slower could look like reuse. MC6 catches that whenever
   the slower period's level changes leave the `P` grid. If they stay on the
   grid (a whole multiple of `P`), the output shows a sample output again
   for the next period, which is what the page calls reuse, and it is graded
   as reuse. Periods with `s > P/2` are reported as not checkable
   (section 3.4).
6. **Contrast.** The same run with `P = 124`. p.141 predicts every next
   sample retrieved (MP1), so the MP2 check goes red.
7. **Reference frames.** As MP1.

### MP3. PAL: at the minimum period of 123 and above, every next sample is retrieved in time

1. **Proposition.** In PAL, with any period value of at least 123, the audio
   DMA retrieves every next data sample before it is due, so every sample
   period carries the next sample of the table. Claims sol:5-p141.c12,
   opus:5-rate.c18, sol:5-p141.c11, opus:5-rate.c17; p.141 (also p.140).
2. **Stimulus.** The program of section 3.3, PAL, once for `P = 123` and
   once for each `P` in `S_hi`.
3. **Observation.** As MP1, with the PAL reference run's level cycle and
   tick.
4. **Expected.** As MP1: all classified sample periods are **next**, for
   every tested value, each on its own. From p.141: "28,867 samples/second
   is also the maximum sampling rate for PAL systems. Thus, for PAL systems,
   a value of at least 123 ticks/sample must be written into the period
   register." Read with the NTSC sentence the "Thus" refers back to: "to
   assure that the audio system DMA will be able to retrieve the next data
   sample". p.140 agrees: "The minimum period value you should use is 124
   ticks per sample NTSC (123 PAL)". (3,546,895 / 123 = 28,837
   samples/second, below 28,867.)
   *Sub-result* **MP3-123**, reported separately: Appendix A (p.266) gives
   the minimum without naming a standard: "The minimum period is 124 color
   clocks. This means that the smallest number that should be placed in this
   register is 124 decimal. This corresponds to a maximum sample frequency
   of 28.86 khz." p.266 predicts no outcome at 123; it only advises against
   the value. So no page predicts reuse at PAL 123, and the expected value
   stands. But a red result at 123 alone falls on the one value where the
   pages state different minimums (section 5).
5. **Preconditions.** MC1-MC6 (MC3 from the PAL reference run).
6. **Contrast.** The same run with `P = 122`. MP4 predicts reuse there, so
   the MP3 check goes red.
7. **Reference frames.** As MP1. For PAL, F1 (c) uses the range of mean line
   length that takes in both p.23's value and p.304's.

### MP4. PAL: below 123, the next sample is not retrieved in time and the previous sample is reused

1. **Proposition.** In PAL, with any period value from 1 to 122, the audio
   DMA does not always retrieve the next data sample in time, and a sample
   already played is output again. Claims sol:5-p141.c11, opus:5-rate.c17,
   sol:5-p141.c12, opus:5-rate.c18; p.141.
2. **Stimulus.** The program of section 3.3, PAL, once for each `P` from 1
   to 122.
3. **Observation.** As MP2, with the PAL reference run.
4. **Expected.** For every `P` from 1 to 122, separately: (a) at least one
   classified sample period is not **next**; (b) at least one is **reuse**.
   Basis: p.141 makes 28,867 samples/second "also the maximum sampling rate
   for PAL systems" and requires "at least 123 ticks/sample" (3,546,895 /
   122 = 29,073 samples/second, above that maximum). The page prints the
   consequence of a lower value only in the NTSC paragraph, the sentence
   quoted under MP2. It carries that consequence to PAL by "also" and
   "Thus". p.164's statement about rates "much higher than the normal
   maximum sampling rate (approximately 29 KHz)" names no standard.
   Quantifiers and reporting as MP2. No sub-results: the claims give no
   theoretical PAL figure.
5. **Preconditions.** MC1-MC6 (PAL reference run).
6. **Contrast.** The same run with `P = 124`. Every page puts PAL 124 at or
   above the minimum (p.140, p.141, p.266), so the MP4 check goes red. 123
   is not used as the contrast, because it is the disputed value (MP3-123).
7. **Reference frames.** As MP3.

### MP5. NTSC: no channel retrieves more than two samples per line (theoretical maximum 31,469 samples/second)

1. **Proposition.** In NTSC, audio channel 0 never receives more than one
   data word (two samples) in one horizontal line, however short the
   period, so it never receives new data at more than 31,469 samples per
   second. Claims sol:5-p141.c3, opus:5-rate.c6; p.141.
2. **Stimulus.** The NTSC runs of MP2-lo (`P` = 1-113). At these periods
   the channel would need more than one word per line.
3. **Observation.** The Chip-memory port, in the port window (F5). For each
   complete line (F4) in the window, the number of DMA reads in the range
   `T ... T+126`. The output is not used, so periods that the output cannot
   resolve are graded here too.
4. **Expected.** In every complete line of the window: at most 1. From
   p.141: "2 samples/line * 262.5 lines/frame * 59.94 frames/second = 31,469
   samples/second" and "The figure of 31,469 is a theoretical maximum."
   The sentences just before the formula state the per-line form of the
   bound:
   "Each audio channel is allocated one DMA slot per horizontal scan line of
   the screen display. An audio channel can retrieve two data samples during
   each horizontal scan line." Quantifier: a maximum holds in every line, so
   every line is graded. The per-second figure follows from the per-line
   bound (262.5 * 59.94 = 15,734 lines/second) and is reported as
   2 * (fetches) / (window length in seconds), not graded on its own.
5. **Preconditions.** MC1-MC5. MC5's requirement that fetches appear in the
   first and last tenth of the window keeps a channel that stopped fetching
   from passing as "at most one per line".
6. **Contrast.** A variant run at `P = 64` that also sets up channel 1 on
   the same table: `AUD1LCH/AUD1LCL` <- `T`, `AUD1LEN` <- 64,
   `AUD1PER` <- 64, `AUD1VOL` <- 0, and step 10 writes `DMACON` <- $8203,
   which is the declaration for this run (MC2). Each channel has its own
   slot in every line (p.141 above; p.194: "4 cycles for audio DMA (2 bytes
   per channel)"; Figure 6-9: audio slots 0-3). p.164: "DMA requests and
   restart requests are transferred to Agnus once each horizontal line". So
   at this period both channels fetch from `T` in the same lines, those
   lines hold two reads in the range, and the MP5 check goes red. In this
   variant MC4 is judged per slot offset: two constant offsets, each with
   its own sequence in address order. Channel 1's output is on the left at
   volume 0, so it is "no output" (p.266).
7. **Reference frames.** F4 (line), checked by MC4's anchor checks and by
   F1 (c). F5 (port window), checked by MC5.

## 5. What the pages leave open or state differently

1. **PAL minimum: 123 or 124.** p.140: "The minimum period value you should
   use is 124 ticks per sample NTSC (123 PAL)". p.141: "for PAL systems, a
   value of at least 123 ticks/sample must be written into the period
   register." p.266 (Appendix A): "The minimum period is 124 color clocks.
   This means that the smallest number that should be placed in this
   register is 124 decimal. This corresponds to a maximum sample frequency
   of 28.86 khz." p.266 names no standard. Its frequency is the NTSC figure:
   at PAL, 124 gives 28.60 kHz. Handling: PAL 123 is its own sub-result
   (MP3-123), and MP4's contrast uses 124.
2. **"These limits apply to both PAL and NTSC machines"** (p.140) directly
   follows two different minimums for the two standards. Which limits it
   means is left open. No expected value rests on it.
3. **What is reused.** p.141 says "the previous sample will be reused".
   p.164 says that well above about 29 kHz "the two samples in the buffer
   register will be repeated". Only what both agree on is graded: a repeat
   of one of the two samples played last (section 3.6).
4. **Theoretical against designed rate.** p.141 derives 31,469
   samples/second from the slot allocation (a period of 113.75 ticks), yet
   puts the minimum at 124 because "In order to save buffers, the hardware
   is designed to handle 28,867 samples/second". It does not describe the
   mechanism that makes 114-123 fail. Handling: the MP2-hi and MP2-lo
   sub-results.
5. **How often, and what happens to the late sample.** Below the minimum the
   page gives neither the rate of reuse nor what happens to the data that
   arrives late. Only "at least once" is graded; skips are recorded, not
   graded.
6. **PAL consequence.** The consequence of a period below the minimum is
   printed only in the NTSC paragraph. For PAL it is carried by "also" and
   "Thus" (MP4).
7. **Period 0.** p.140 "the maximum is 65535" against p.159 "its full range
   124-65536". What the value 0 does is not stated. It is excluded.
8. **PAL line length.** p.23: "In PAL, they are all 227 long." p.304: "PAL
   (625 lines, 227.5 colorclocks per scan line)". It affects only the
   anchor check F1 (c), which accepts the range between the two.
9. **"Each allocated DMA time slot".** p.148 d: "Start retrieving audio data
   each allocated DMA time slot." Read literally, this would fetch in every
   line whatever the period. p.165 describes one request per word ("AUDxDR
   Audio DMA request to Agnus for one word of data"), which is the only
   reading under which a table plays in order at long periods. F1 (b) uses
   the per-word reading, and only as a cross-check of the time base.
10. **Figure 5-8** (p.166, the audio state diagram) is not used for any
    expected value. The claims do not rest on it.
