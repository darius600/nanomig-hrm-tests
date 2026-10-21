# Audio DMA at and below the minimum period: specification addendum

This addendum specifies six propositions about audio DMA at, above and below
the minimum sampling period that p.141 gives, in NTSC and in PAL. Its inputs
are the twelve claims in `claims-spec-stage.jsonl` (all printed on p.141; section
1 says which of them a graded line establishes, and MP6 rests on pp.139 and 147
and carries no claim id) and
these pages of the *Amiga Hardware Reference Manual*, 3rd edition: pp.23, 24,
34 and 40 (beam position and field length), pp.137-145, 147-148, 156-159 and
164-166 (Chapter 5, audio), pp.194-195 (Chapter 6, DMA time slots and
Figure 6-9), pp.265, 266, 278, 280 and 285 (Appendix A), pp.290-291
(Appendix B) and pp.304 and 310 (Appendix C). The addendum is
self-contained: its data, program, reference frames, preconditions and
contrasts are its own, and it changes nothing in the audio package it is
added to. It specifies only what the
claims support. Quotations are the printed text.

## 1. Claims and what establishes them

A claim id is printed on a graded line only if the whole `says` and `quote` of
the claim is established by that line. Otherwise the id is printed on a
`RECORDED` line only, with the clause that no check here covers. An id is never
on both.

| Claim | Where |
|---|---|
| sol:5-p141.c10, opus:5-rate.c15 | MP2: the one line of each period 1-123 that grades both of its clauses |
| sol:5-p141.c9, opus:5-rate.c14 | withdrawn: MP1 covers a sample of the values of "at least 124" |
| sol:5-p141.c12, opus:5-rate.c18 | withdrawn: MP3 covers a sample of the values of "at least 123" |
| sol:5-p141.c11, opus:5-rate.c17 | withdrawn: the PAL maximum, graded on one side exhaustively and on the other side on a sample |
| sol:5-p141.c4, opus:5-rate.c8 | withdrawn: the reason "In order to save buffers" and the design maximum |
| sol:5-p141.c3, opus:5-rate.c6 | withdrawn: the figure 31,469 |

Why each is withdrawn, and what stays graded without its id:

- **sol:5-p141.c9, opus:5-rate.c14** ("a value of at least 124 must be written
  into the period register to assure that the audio system DMA will be able to
  retrieve the next data sample"): the claim is about every value of at least
  124. MP1 grades the fourteen values of `S_hi` and nothing else. The values
  132-139, 141-159, 161-199, 201-255, 257-319, 321-427 and 429-65535 are not
  tested. One run there, with its settling time and a window of at least 454
  sample periods, costs from about 0.07 s of simulated time (at 429) to about
  11 s (at 65535); `S_hi` is the tested set, chosen for that cost. The values
  are listed in the scope record. MP1 stays graded, on the fourteen values,
  as the control of MP2 on the same comparator above the minimum, without
  these ids.
- **sol:5-p141.c12, opus:5-rate.c18** ("for PAL systems, a value of at least
  123 ticks/sample must be written into the period register"): the same, for
  PAL. MP4 grades every period 1-122; MP3 grades 123 and `S_hi` and leaves the
  same gaps (132-139 ... 429-65535). MP3 stays graded without these ids.
- **sol:5-p141.c11, opus:5-rate.c17** ("28,867 samples/second is also the
  maximum sampling rate for PAL systems"): the rates above the maximum
  (periods 1-122) are graded at every period by MP4; the rates at and below it
  only at 123 and `S_hi` by MP3. The claim has the same two-sided structure as
  the NTSC design maximum (c4, c8), and is withdrawn with it. MP3 and MP4 stay
  graded without these ids.
- **sol:5-p141.c4, opus:5-rate.c8** ("In order to save buffers, the hardware is
  designed to handle 28,867 samples/second"): the reason is not an
  observation, and no one assertion grades the design maximum on both sides of
  the threshold: MP1 grades the periods at and above 124 on a sample, MP2 the
  periods below it. MP1 and MP2 are graded without these ids.
- **sol:5-p141.c3, opus:5-rate.c6** ("The figure of 31,469 is a theoretical
  maximum", "2 samples/line * 262.5 lines/frame * 59.94 frames/second = 31,469
  samples/second"): MP5 grades the per-line bound that the figure is derived
  from, without these ids. The numeric figure is not graded. It is built from
  262.5 lines per frame, that is lines that alternate between 228 and 227
  colour clocks (p.23, p.194); the NTSC runs here show lines of 227 ticks only
  and 31,538 samples/second (section 6.3), the 228-tick lines do not occur
  (`NM-HRM-010`). The figure's per-line form, "Each audio channel is allocated
  one DMA slot per horizontal scan line", is what MP5 grades.

Other parts of the claims that no check here covers, and why:

- **31,469 as a rate that is reached:** the page calls it "a theoretical
  maximum" and says in the next sentence that "the hardware is designed to
  handle 28,867 samples/second". No page predicts that any output reaches
  31,469 samples/second, so nothing checks that it is reached. Only the bound
  is checked (MP5), and only in NTSC, because the figure is built from NTSC
  numbers (262.5 lines/frame, 59.94 frames/second). The page gives no PAL
  figure.
- **Period 0** inside "below 124": excluded. The pages do not say what the
  value 0 does. p.140 gives "the maximum is 65535"; p.159 speaks of "its full
  range 124-65536".
- **How often the previous sample is reused** below the minimum: the page does
  not say. Only "at least once in the window" is graded; the rate is reported,
  never graded.
- **What happens to the sample that arrives too late:** the page says the
  previous sample is reused. It does not say whether the late sample is then
  played or lost. Forward jumps ("skip", section 3.6) are counted below the
  minimum but not graded.
- **The PAL consequence:** p.141 prints "the previous sample will be reused"
  in its NTSC paragraph only. Its PAL paragraph gives two sentences:
  "28,867 samples/second is also the maximum sampling rate for PAL systems."
  and "Thus, for PAL systems, a value of at least 123 ticks/sample must be
  written into the period register." MP4 grades what those two sentences
  state, that not every next sample is retrieved in time below 123 (section
  4, MP4); the PAL reuse count is measured.

## 2. The propositions at a glance

| Id | Standard | Periods (AUD0PER) | Expected in the window |
|---|---|---|---|
| MP1 | NTSC | the set S_hi (124 and up) | every sample period carries the next data sample |
| MP2 | NTSC | every value 1-123 | at least one sample period does not carry the next sample, and at least one reuses the sample just played |
| MP3 | PAL | 123 and the set S_hi | as MP1 |
| MP4 | PAL | every value 1-122 | at least one sample period does not carry the next sample; reuse is measured, not graded |
| MP5 | NTSC | every value 1-113 | no line holds more than one data fetch of the channel |
| MP6 | both | every run of MP1-MP5 | channel 0's reads at its slot offset form passes of exactly 64 words, T to T+126 in order, each followed by T |

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

0. Boot overlay. `DDRA` ($BFE201) <- $03, then `PRA` ($BFE001) <- $02 (byte
   writes). p.223: "An additional copy of the system ROM responds starting
   at memory location $00000000. The system RAM that would normally be
   located at this address is not available." and "Software must assume that
   memory is not available. The OVL bit in one of the 8520 Chips disables
   the overlay (See Appendix F for the bit location)." p.339 (Appendix F,
   CIAA address map) gives `BFE001 pra ... /LED OVL` and `BFE201 ddra
   Direction for port A (BFE001);1-output (set to 0x03)`: bit 0 is OVL, bit
   1 is /LED. `PRA` <- $02 clears OVL and sets /LED. p.157: "there is a
   control bit that allows the audio output to bypass the low pass filter.
   This control bit is the same output bit of the 8520 CIA that controls the
   brightness of the red "power" LED (CIA A $BFE001 - Bit 1: /LED)."; MC3
   stays the check that the output is unfiltered. `T` stays below $80000
   (3.1); the result block `R` and every parameter or data word of the
   program that the test reads lie at $80000 or above.
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

Run length. The window `W(P) = max(60 000, 454 * P)` ticks (section 3.5) is a
stimulus choice that no page gives: 60 000 ticks is a length chosen so that
the window holds many sample periods at every tested `P`, and `454 * P` ticks
holds 227 words at period `P` (a word every `2 * P` ticks). With it and `T_settle(P) = max(128 * P, 14 600)` ticks:

`N_run = max(330, ceil((T_settle(P) + W(P)) / 227) + 16)` lines.

`128 * P` ticks is one pass through the table at period `P`. 14 600 ticks
(64 lines) is the length of one pass if no more than one word arrives per
line. 330 lines is more than the longest field (313 lines, p.24), so the wait
always crosses at least one field end (MC1). These numbers only size the run.
If a run turns out too short, MC5 reports it as incomplete; it is never
graded on a shorter window.

What W covers. The window holds at least 227 words of the channel at every
tested `P` (`W / (2 * P) >= 227`), and 227 is the number of different phases a
word can have against a line of 227 ticks. So every line phase of an ordinary
word falls in the window. W does not cover every phase at which the restart of
the table can meet the line: that phase steps by `64 * 2 * P` ticks modulo
227 from one pass to the next, and because 227 is prime all 227 restart phases
need 227 table passes, that is `227 * 128 * P` ticks, which is 3.49 million
ticks at `P` = 120. At `P` = 114-123 the window holds 3 to 4 passes. A
proposition that asks for "at least one" event in the window (MP2 (a) and (b),
MP4 (a)) is therefore graded on the words and the restart phases that the
window holds, and a red line says so in its label ("within W"). The window is
not widened to cover every restart phase.

### 3.4 What the test records

- **Chip-memory port:** every access with its time in system clock cycles,
  its word address, and read or write.
- **Refresh:** the `refresh` output of `nanomig.v` ("current bus cycle is
  refresh"), a system-boundary port, sampled once per system clock cycle on
  the same time base as the Chip-memory port. A refresh access is a rising
  edge of that output. The criterion "reads below word address $1000" is an
  observation of the implementation, not of a page; the test prints it as a
  MEASURED cross-check and never grades with it.
- **Audio output:** the channel-0 output is whichever of the two outputs
  carries channel 0; the other output carries the other pair of channels.
  The pages disagree on which named
  jack that is: p.137 says "Channels 0 and 3 are connected to the
  right-side output jack.", and Appendix E (p.326) pairs "left (channels 0,
  3)". The side is measured at runtime, recorded per core and standard, and
  never graded. The test needs every output sample's time on the same
  system-clock time base as the Chip-memory port: either as a time stamp or as a fixed number of system clock cycles per
  output sample, counted from a known start. If the harness gives one mixed
  output instead of two, the same rules apply to it, because channels 1-3
  are at volume 0. The test reads the levels as the harness delivers them.
  They must not have passed a low-pass filter (p.156: "The system includes
  a low-pass filter"). MC3 shows whether that holds.
- **Result block** `R+0` ... `R+9`, read after the run.
- **Output resolution `s`:** the spacing of output samples, in ticks
  (section 3.5, F1). The harness delivers one audio sample per system clock
  cycle, so `s` is one system clock cycle expressed in ticks: `s` = 1 /
  (cycles per tick), with the cycles per tick measured in the reference run
  (F1, `D_ref / 447`). A period `P` can be graded at the output only if
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
is the mean duration of the channel-0 output's level runs over 256 consecutive
runs inside the window (section 3.5, F5). One tick = `D_ref / 447` system
clock cycles.

*Anchor checks* (part of MC3):
(a) every one of those 256 level runs lasts `D_ref` to within one output
sample spacing;
(b) at the port, the mean spacing of consecutive table fetches over whole
passes of the same window is `2 * D_ref` to within the resolution of the
measurement. Basis: each word holds two samples (p.138: "the audio DMA
retrieves one word (16 bits) at a time and uses the sample it reads as two
bytes of data"), and each word is fetched once per pass (p.139, p.165: "AUDxDR
Audio DMA request to Agnus for one word of data"). Resolution: a fetch lies in
the one DMA slot per line that its channel owns (p.141: "Each audio channel is
allocated one DMA slot per horizontal scan line of the screen display"), so
its time is known to one line; the time from the first wrap to the last wrap
is then known to one line, and over the `n` fetches between them the mean
spacing to one line / `n`. `D_ref` is the span of 256 level runs, each end
known to one system clock cycle, so `2 * D_ref` is known to 2 / 256 cycle. The
tolerance is the sum, one longest complete line of the window over `n`
plus 2 / 256 cycle. It is not a percentage;
(c) every complete line of F4 in the port window is as long as the pages say,
in ticks, to within `s`, the output resolution (one system clock cycle). NTSC:
every line is between 227 and 228 ticks long. p.23: "Every other line is a
long line (228 color clocks, 0-$E3), with the others being 227 color clocks
long." PAL: every line is between 227 and 227.5 ticks long. The two pages that
state it give different values, and this range takes in both: p.23 says "In
PAL, they are all 227 long", and p.304 lists "PAL (625 lines, 227.5 colorclocks
per scan line)". The calibration needs the length of each line, so it is the
line that is graded, not a mean. Whether NTSC lines alternate between 228 and
227 ticks, and the 227.5-clock mean of p.194 ("there are 227.5 "color
clocks""), are printed as a MEASURED line and not graded here: `NM-HRM-010`
grades them. The only tolerance of this anchor is `s`, which the measurement
fixes.

**F2. Level cycle.** In the reference run, the channel-0 output shows the table
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
run at period `P`, the channel-0 output can change level only on a grid of
spacing `P` ticks. The grid is `t0 + k * P` (k = 0, 1, 2, ...), with `t0` =
`t_start`, the first level change of the window (F5). MC6 requires every
level change inside the window to lie within one output-sample spacing `s`
of a grid point. The window is cut into consecutive sample periods of `P`
ticks starting at `t0`. Each sample period carries the index of the level
the output shows in it.

**F4. Line.** Figure 6-9 (p.195) draws each horizontal line beginning with
the memory refresh slots, followed by the disk, audio and sprite slots, with
one audio slot per channel in a fixed position. p.194: "4 cycles for memory
refresh" per line. A refresh cluster is a run of refresh
accesses (rising edges of `refresh`, section 3.4) with gaps of no more than
4 ticks. The start of a line is the first access of a cluster. A line runs from one cluster start to the next.
*Anchor checks* (part of MC4, and asserted in each reference run before F1 (c),
which measures lines with this frame): every cluster holds 4 refresh accesses;
every channel-0 table fetch lies at the same offset, in ticks, from the start
of its line (Figure 6-9: a fixed slot per channel).

**F5. Windows.**

- *Output window:* `t_first` is the time of the first channel-0-output sample
  whose level is in the level cycle. In the reference run, which defines
  the level cycle, `t_first` is the first change of the channel-0 output after
  step 10. `t_start` is the first channel-0-output
  level change at or after `t_first + T_settle(P)`. The output window is
  `[t_start, t_start + W(P))`, which holds `N = floor(W(P) / P)` sample
  periods, at least 454. Waiting `T_settle` keeps the start of the DMA out
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
(F2, F3), or `u` where a sample period is unmapped. `c_1` itself is not
classified. (If `c_1` is unmapped, count it as unmapped and start from the
first mapped sample period.) Each later sample period `j` is compared with
the sample period immediately before it, `c_{j-1}`, indices mod 128:

| Condition | Class |
|---|---|
| `c_j = u` | unmapped |
| `c_{j-1} = u`, `c_j` mapped | after an unmapped period (reported, not graded; the sample just played is not known) |
| `c_j = c_{j-1} + 1` | **next** |
| `c_j = c_{j-1}` | **reuse** |
| `c_j = c_{j-1} - 1` | **repeat** (reported, not graded) |
| `c_j = c_{j-1} + k`, 2 <= k <= 63 | skip |
| anything else | other |

The definition of **reuse** is what p.141 says: "the previous sample will be
reused", which is the sample just played, the one in the sample period
immediately before (`c_j = c_{j-1}`). A sample period that returns to a sample
after a step back (`m-1`, then `m`) is a **next**, not a reuse: it is not the
sample just played. p.164 says "If the sampling rate is set much higher than
the normal maximum sampling rate (approximately 29 KHz), the two samples in
the buffer register will be repeated", which would also allow a sample period
that carries the other sample of the last word (`c_{j-1} - 1`). That is
reported as **repeat**, separately, and never graded. The table has 128
different values (section 3.1), so a repeat cannot be taken for a new sample.

Each run reports `N` and the number of sample periods in each class, repeats
included.

### 3.7 Preconditions

Every graded run asserts these first. If one fails, the run is reported as
"precondition failed" with its name, never as green or red for a
proposition. Each precondition has a contrast: a change of the stimulus for
which the pages predict that this precondition fails while the program still
finishes and the other preconditions hold. A contrast run is graded only for
the precondition it is the contrast for.

In the runs below the minimum (MP2, MP4 and MP5) the "without a gap" clause
of MC4 is printed as a MEASURED result and is not a precondition there: it
encodes how the late sample is handled, which is the clause under test. The
other parts of MC4 (the fixed slot offset, no CPU access, the anchor, at least
one wrap) and MC1, MC2, MC3, MC5 and MC6 stay preconditions in those runs.
MC6 is the period in effect, asserted in every run, below the minimum
included: the page fixes when the output may change (p.140, p.164) and what
the late sample is does not change when. Below the minimum address order is a result, so
MC5's clause "table fetches in the first and in the last tenth" and its clause
"the last table fetch comes after the end of the output window" use channel
0's DMA reads at its slot offset (F4), at any address, in those runs.

Above the minimum MP1 and MP3 are graded and reported even when only MC4's
address-order clause ("in address order without a gap") or MC5's table clauses
(the tenths and the last fetch) fail, with the line of the failing
precondition printed next to the verdict: a late word can come with a break in
the address order, as it does below the minimum, and the precondition must not
hide that red.

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

1. the output that does not carry channel 0 is constant throughout the window;
2. the channel-0 output shows exactly 128 distinct levels, which repeat with a
   period of 128 level runs, each level once per period (F2);
3. the anchor checks F1 (a), (b) and (c) hold, and the anchor check F4 holds.

The output resolution `s` (F1) follows from `D_ref` once item 2 holds; it is
no assertion of its own.

This is what p.140 defines a period to be ("The sampling period specifies
the number of system clock ticks, or timing intervals, that should elapse
between each sample (byte of audio data) fed to the digital-to-analog
converter"). It sits at a period well above both minimums, where no claim of
this addendum is at stake.

Each assertion of MC3 has a contrast of its own, a change of the stimulus for
which the pages predict that this assertion fails while the rest of MC3 holds:

*Contrast (item 1, the other output):* the reference run with channel 1
playing a copy of the table at `T3` (not overlapping `T`) at volume 64, period
1000, and `DMACON` <- $8203. p.137 gives the channels their outputs, channel 1
on the other one than channel 0: its levels change, so the output that does
not carry channel 0 is not constant. Channel 0's output, its level cycle, the
fetches of `T` and the lines are unchanged.
*Contrast (item 2, the level cycle):* the reference run with `AUD0LEN` <- 32.
p.145 step 4: the length register holds "the number of data words to be
retrieved before starting at the address currently in AUDxLC", so the channel
plays 32 words (64 levels) and starts again: the output changes, at nonzero
volume, but shows 64 distinct levels, not 128.
*Contrast (F1 (a)):* the reference run with `AUD0PER` <- 448 written, late in
the span of the 256 level runs, so that the last of them last one tick longer
than `D_ref`. (p.143: "To set the period register, you must write the period
value into the register AUDxPER"; p.140: a level lasts as many ticks as the
period.) The line number of the write is a stimulus choice of the build
(section 6.1).
*Contrast (F1 (b)):* the reference run with `AUD0PER` <- 224 written after the
span of the 256 level runs and inside the port window. The level runs of the
span are those of period 447, so `D_ref` and the lines are unchanged; the
fetches of the port window come at twice the rate for the rest of it, so
their mean spacing is no longer `2 * D_ref`.
*Contrast (F1 (c), time scale):* the reference run with `AUD0PER` <- 448 and
the declaration left at 447. The tick comes out 448/447 times as long, so the
lines measure about 227 * 447 / 448 = 226.5 ticks, below the lower limit of
227 minus `s` in both standards, and F1 (c) fails through its own clause. The
mismatch is small enough that the other anchors hold: the 0.5-cycle tolerance
of the F4 cluster check (spacings of `2 * k` ticks, k up to 3) is not
exceeded, so F4 stays green and the line set is not empty; anchors (a) and (b)
are ratios of the same measurement and hold too. 448 is the smallest written
period that leaves the limit: 446 gives 227.5 ticks, inside both ranges.
*Contrast (F4):* the reference run with one CPU read of the word at `T+64`
at the end of the port window, after its last wrap. An access to `T` appears
at an offset from its line start that is not channel 0's slot offset (Figure
6-9: a fixed slot per channel), so the fetches of the port window no longer
lie at one offset. The refresh clusters are the same. The anchor's other
clause, that every refresh cluster holds four accesses two slots apart, has no
contrast: the refresh slots of Figure 6-9 and p.194 are fixed for every
stimulus the pages permit (unfalsifiable at this boundary).

**MC4. The channel plays the table, and nothing else touches it.** Inside
the port window:

- the DMA reads at channel 0's slot offset (F4) all lie in the range
  `T ... T+126` and follow the address order `T`, `T+2`, ..., `T+126`, `T`,
  ... without a gap, and the window holds at least one wrap (F5). p.148 b:
  the back-up location register "will be used as a pointer showing current
  data word to fetch"; p.147: "Each time the length
  counter reaches zero, both the location and length registers are reloaded
  with the same values to continue the audio output."
- there is no CPU access to that range after step 5. A CPU access inside
  `T` is an access off channel 0's slot offset, or one out of address order;
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
lines. The last table fetch comes after the end of the output window. (Below
the minimum the last two clauses use channel 0's DMA reads at its slot
offset, at any address; section 3.7.)
*Contrast (marker):* a variant leaves out step 14's store. MC5 fails.
*Contrast (channel stopped early):* a variant, run at a small period (for
example `P = 64`; at `P = 428`, `T_settle` is 241 lines, so a stop at 200
lines would leave no port window), writes `DMACON` <- $0001 after 200 lines
of the wait and then completes the full wait of `N_run` lines. `T_settle(P)`
is at least 14 600 ticks (64.3 lines) and the channel gets at most one word
per line, so at most 36 words lie between the port window's start and line
100, fewer than the 64 words of one pass, and MC4's "at least one wrap" would
fail as well. A stop after 200 lines leaves more than 130 lines of the port
window, which hold more than one pass at one word per line. No table fetch occurs in the last
tenth of the port window, so MC5 fails. The wait still crosses a field end
(MC1).

**MC6. Period in effect, and output resolved.** `s <= P/2`, and every level
change inside the window lies within `s` of the grid of F3. After the last
fetch of channel 0's data in the run the channel has no data to play, so the
level changes after it are not judged; in a complete run MC5 puts that fetch
after the end of the window, and the clause covers the whole window. MC6 is
asserted in every run, below the minimum as well as above it: the pages fix
when the output may change (p.140: "each time the register counts down to 0,
another sample is retrieved"; p.164), and what the late sample is does not
move that time.
*Contrast:* the `P = 128` run with `AUD0PER` <- 129 written while 128 is
declared. The level changes drift by one tick per sample period against the
128-tick grid, so MC6 fails.
*Contrast (below the minimum):* a PAL run with `AUD0PER` <- 101 written while
100 is declared. The level changes drift by one tick per sample period
against the 100-tick grid, so MC6 fails, and MP4 is reported as "precondition
failed", not graded.

## 4. Propositions

### MP1. NTSC: at the minimum period of 124 and above, every next sample is retrieved in time

1. **Proposition.** In NTSC, with any period value of at least 124 in the
   period register, the audio DMA retrieves every next data sample before it
   is due, so every sample period carries the next sample of the table.
   p.141. No claim id is carried: the values of "at least 124" are graded on
   a sample only (section 1).
2. **Stimulus.** The program of section 3.3, NTSC, once for each `P` in
   `S_hi`.
3. **Observation.** The channel-0 output in the output window (F5), cut into
   sample periods (F3) and classified (section 3.6). The claim concerns a
   channel that is playing its data at period `P`. The test knows that this
   holds in the window from MC4 (the fetches run through the table), MC6
   (the output changes on the `P` grid) and the window start after
   `T_settle`.
4. **Expected.** For every `P` in `S_hi`, all `N - 1` classified sample
   periods are **next**: no reuse, skip, unmapped or other. From p.141:
   "Therefore, a value of at least 124 must be written into the period
   register to assure that the audio system DMA will be able to retrieve the
   next data sample." (3,579,545 / 124 = 28,867 samples/second, the rate
   p.141 designs for.) Quantifiers: "at least 124" is graded on every value of
   `S_hi`, each on its own, and on no other value (section 1); "the next data
   sample" is graded on every sample period of the window, not on one.
5. **Preconditions.** MC1-MC6 (MC3 from the NTSC reference run). If only
   MC4's address-order clause or MC5's table clauses fail, MP1 is graded and
   reported all the same, next to the verdict of that precondition (section
   3.7).
6. **Contrast.** The same run with `P = 119`. p.141 predicts a late word there
   (section 5, item 11) and MP2 grades it, so the MP1 check goes red. A second
   contrast is `P = 64`. Both runs are already in the required set, and the
   setup and preconditions are unchanged.
7. **Reference frames.** F1, F2, F3, F5. F1 and F2 are checked by MC3, F3 by
   MC6, the window start by MC4 and MC5.

### MP2. NTSC: below 124, the next sample is not retrieved in time and the previous sample is reused

1. **Proposition.** In NTSC, with any period value from 1 to 123, the audio
   DMA does not always retrieve the next data sample before the count
   reaches 0, and the previous sample is output again. Claims
   sol:5-p141.c10, opus:5-rate.c15; p.141. p.164 gives the mechanism and is
   not graded (section 5, item 11).
2. **Stimulus.** The program of section 3.3, NTSC, once for each `P` from 1
   to 123.
3. **Observation.** As MP1.
4. **Expected.** For every `P` from 1 to 123, three lines, each on its own:
   (a) at least one classified sample period is not **next**;
   (b) at least one is **reuse**, `c_j = c_{j-1}`, the sample just played
   (section 3.6); and
   (a) and (b) together, the line that carries the claim ids, because it is
   the one that establishes all of what the two claims say. (a) and (b) carry
   no id.
   From p.141: "If the period value is below 124, by the time the cycle
   count has reached 0, the audio DMA will not have had enough time to
   retrieve the next data sample and the previous sample will be reused."
   Quantifiers: "below 124" is graded on every value 1-123, each on its own.
   The page gives no rate, so only "at least one in the window" is graded for
   each clause. The window `W = 60,000` ticks is a stimulus choice of this
   specification (section 3.3, which says what it covers); it is named in the
   label ("within W = 60,000 ticks"). The numbers of next, reuse, repeat,
   skip, unmapped, after-unmapped and other periods are reported. Skips are
   not graded (section 1).
   *Sub-results*, reported separately with the same expected value and the
   same claim ids:
   **MP2-lo**, `P` = 1-113: a channel at these periods needs more than the
   "theoretical maximum" of 31,469 samples/second (3,579,545 / 31,469 =
   113.75 ticks per sample).
   **MP2-hi**, `P` = 114-123: these lie between that theoretical figure and
   the designed rate of 28,867 samples/second. p.141 states the late word and
   the reuse for every period below 124.
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
   period carries the next sample of the table. p.141 (also p.140). No claim
   id is carried: the values of "at least 123" are graded on a sample only
   (section 1).
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
5. **Preconditions.** MC1-MC6 (MC3 from the PAL reference run); as for MP1,
   MP3 is graded and reported even when only MC4's address-order clause or
   MC5's table clauses fail (section 3.7).
6. **Contrast.** The same run with `P = 119`. MP4 predicts that not every
   next sample is retrieved there (p.141, section 5, item 11), so the MP3
   check goes red. A second contrast is `P = 64`.
7. **Reference frames.** As MP1. For PAL, F1 (c) uses the range of line
   length that takes in both p.23's value and p.304's.

### MP4. PAL: below 123, not every next sample is retrieved in time

1. **Proposition.** In PAL, with any period value from 1 to 122, the audio
   DMA does not always retrieve the next data sample in time. p.141, and p.164
   for the mechanism (not graded, section 5, item 11). No claim id is carried
   (section 1).
2. **Stimulus.** The program of section 3.3, PAL, once for each `P` from 1
   to 122.
3. **Observation.** As MP2, with the PAL reference run.
4. **Expected.** For every `P` from 1 to 122: at least one classified sample
   period is not **next**, within the window `W = 60,000` ticks (a stimulus
   choice, section 3.3). Basis, the two sentences of p.141's PAL paragraph:
   "28,867 samples/second is also the maximum sampling rate for PAL systems."
   and "Thus, for PAL systems, a value of at least 123 ticks/sample must be
   written into the period register." (3,546,895 / 122 = 29,073
   samples/second, above that maximum.) The maximum rate is what the DMA can
   retrieve in time, so at a period below 123 it cannot retrieve every next
   sample in time. The reuse count is MEASURED, not graded: the page prints
   the consequence "the previous sample will be reused" in the NTSC paragraph
   only; applied literally to PAL it would predict reuse at 123 as well,
   which the same page's PAL minimum of 123 excludes. p.164's statement about
   rates "much higher than the normal maximum sampling rate (approximately 29
   KHz)" names no standard. No sub-results: the claims give no theoretical PAL
   figure.
5. **Preconditions.** MC1-MC6 (PAL reference run).
6. **Contrast.** The same run with `P = 124`. Every page puts PAL 124 at or
   above the minimum (p.140, p.141, p.266), so the MP4 (a) check goes red. 123
   is not used as the contrast, because it is the disputed value (MP3-123).
   The MP3 contrast at `P` = 119 is the same measurement seen from the other
   side.
7. **Reference frames.** As MP3.

### MP5. NTSC: no channel retrieves more than one data word (two samples) per line

1. **Proposition.** In NTSC, audio channel 0 never receives more than one
   data word (two samples) in one horizontal line, however short the
   period. p.141. No claim id is carried (section 1: the figure 31,469 is not
   graded).
2. **Stimulus.** The NTSC runs of MP2-lo (`P` = 1-113). At these periods
   the channel would need more than one word per line.
3. **Observation.** The Chip-memory port, in the port window (F5). The
   channel is the slot offset of MC4 (F4); for each complete line (F4) in the
   window, the number of DMA reads of the range `T ... T+126` at that offset is
   counted, per channel. With one channel set up, a read of the range at any
   other offset in the line is counted against that channel too. The output
   is not used, so periods that the output cannot resolve are graded here too.
4. **Expected.** In every complete line of the window, for every channel: at
   most 1. From p.141: "Each audio channel is allocated one DMA slot per
   horizontal scan line of the screen display. An audio channel can retrieve
   two data samples during each horizontal scan line." Quantifier: a maximum
   holds in every line, so every line is graded. The per-second figure that
   p.141 derives from it ("2 samples/line * 262.5 lines/frame * 59.94
   frames/second = 31,469 samples/second", "a theoretical maximum") is
   reported as 2 * (reads in the complete lines) / (duration of those complete
   lines in seconds), not graded. The duration is that of the complete lines
   the reads were counted in, never the length of the whole window. With
   lines of 227 ticks the figure is 2 * 3,579,545 / 227 = 31,538
   samples/second, 0.2 % above p.141's 31,469, which rests on 227.5-tick lines
   (262.5 lines/frame); the record says that the figure follows from the
   227-tick lines of the NTSC runs (`NM-HRM-010`).
5. **Preconditions.** MC1-MC5. MC5's requirement that fetches appear in the
   first and last tenth of the window keeps a channel that stopped fetching
   from passing as "at most one per line".
6. **Contrast.** None that can turn it red: MP5 is unfalsifiable at this
   boundary. p.141 allocates one DMA slot per line to each channel, and no
   stimulus the pages permit makes one channel take a second slot in a line;
   a run with two channels on one table has two reads of the range in a
   line, but each channel still has one, and the page predicts exactly that.
   A control run shows that the count is per channel: `P = 64` with channel 1
   also set up on the same table: `AUD1LCH/AUD1LCL` <- `T`, `AUD1LEN` <- 64,
   `AUD1PER` <- 64, `AUD1VOL` <- 0, and step 10 writes `DMACON` <- $8203,
   which is the declaration for this run (MC2). Each channel has its own
   slot in every line (p.141 above; p.194: "4 cycles for audio DMA (2 bytes
   per channel)"; Figure 6-9: audio slots 0-3). In this variant MC4 is judged
   per slot offset: two constant offsets, each with its own sequence in
   address order. Channel 1's output is on the other output, at volume 0, so
   it is "no output" (p.266). Expected, and the same on a correct and on a
   wrong implementation: green, one read per channel in every line.
7. **Reference frames.** F4 (line), checked by MC4's anchor checks and by
   F1 (c). F5 (port window), checked by MC5.

### MP6. Both standards: every pass restarts at the start of the data

1. **Proposition.** In both standards, at every period value of a run of
   MP1-MP5, audio channel 0's DMA reads at its slot offset form passes of
   exactly 64 words: `T`, `T+2`, ..., `T+126` in order, each pass followed by
   a pass that starts at `T`. No read lies outside `T ... T+126`, no word is
   fetched twice in a row (in particular none at a restart), and no pass is
   shorter or longer than 64 words. Pages p.139 and p.147; no claim id (the
   reload claims belong to other packages).
   p.139: "Each time the system retrieves the last audio word from the data
   area, it uses the contents of these location registers to again find the
   start of the data." p.147: "Each time the length counter reaches zero,
   both the location and length registers are reloaded with the same values
   to continue the audio output." The pages give no exception for the period
   value, so the proposition is graded at every period, above and below the
   minimum.
2. **Stimulus.** None of its own: the runs of MP1-MP5 (both standards, every
   `P` of section 3.2), graded for MP6 as they run.
3. **Observation.** The Chip-memory port, in the port window (F5). Channel 0's
   reads are the DMA reads at the slot offset of MC4 (F4), at any address,
   in the order of their time; the CPU's own accesses are not among them. In
   a run with two channels on `T` (the MP5 contrast) each slot offset is read
   on its own.
4. **Expected.** In each run, for the reads of the window:
   (a) no read lies outside `T ... T+126`;
   (b) no read repeats the address of the read before it;
   (c) every read after the first follows the one before it: `T+2i` is
   followed by `T+2(i+1)`, and `T+126` by `T`. A `T` that follows a word other
   than `T+126` is a short pass, a word other than `T` after `T+126` is a long
   pass, any other step is a gap.
   The first and the last pass of the window are partial, and only their
   order is checked. MP6 is graded per run and printed as one line per
   standard that names the periods whose run fails it. For each failing
   period the evidence is printed as MEASURED: the first off-table read, the
   first double fetch, the first short pass (address, and time in ticks after
   the start of the port window). Quantifier: "each time" and "every pass"
   are graded on every pass of the window, and "all periods" on every run of
   MP1-MP5 of both standards.
5. **Preconditions.** MC1, MC2, MC3, and MC4's clauses on the slot offset, on
   no CPU access to `T` and on the anchor; MC5's marker and line count. Not
   MC4's address-order and wrap clauses and not MC5's table clauses: they
   test the same passes as MP6 and would hide its red. MC6 is not a
   precondition either: the output is not used.
6. **Contrast.** NTSC and PAL, `P = 64` (a period at which both standards run
   with the preconditions of MP5 holding), with `AUD0LEN` <- 32 written once
   after `N_run / 2` lines of the wait, while the channel runs. p.147: "Once
   the original registers have been read by the DMA channel, you can change
   their values without disturbing the operation you started with the
   original register contents" and "Each time the length counter reaches
   zero, both the location and length registers are reloaded"; p.145 step 4:
   the length register holds "the number of data words to be retrieved before
   starting at the address currently in AUDxLC". So the pass in progress
   finishes with 64 words and every later pass has 32: `T+62` is followed by
   `T`, a short pass in (c), and MP6 fails. MC1, MC2, MC4's slot-offset, CPU
   and anchor clauses and MC5's marker and line count still hold.
   MP6 is red in the unmutated run (section 6.3), so this contrast alone does
   not show that the comparator can pass. The runs of `P` = 64 and `P` = 124,
   in both standards, are graded through the same MP6 comparator on their own:
   both are green in the unmutated run, and the `P` = 64 run is the one the
   contrast changes from green to red.
7. **Reference frames.** As MP1 (F4, F5).

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
   register will be repeated". Only what p.141 says is graded as reuse: the
   sample just played, the one in the sample period immediately before
   (`c_j = c_{j-1}`). A period that carries the other sample of the last word
   (`c_{j-1} - 1`) is reported as a repeat (section 3.6).
4. **Theoretical against designed rate.** p.141 derives 31,469
   samples/second from the slot allocation (a period of 113.75 ticks), yet
   puts the minimum at 124 because "In order to save buffers, the hardware
   is designed to handle 28,867 samples/second". It does not describe the
   mechanism that makes 114-123 fail; p.164 does (item 11). Handling: the
   MP2-hi and MP2-lo sub-results, graded at every period 1-123.
5. **How often, and what happens to the late sample.** Below the minimum the
   page gives neither the rate of reuse nor what happens to the data that
   arrives late. Only "at least once" is graded; skips are recorded, not
   graded.
6. **PAL consequence.** The consequence of a period below the minimum is
   printed only in the NTSC paragraph. Applied literally to PAL it would
   predict reuse at 123 as well, which the page's PAL minimum of 123
   excludes. So MP4 grades only that not every next sample is retrieved
   (from "also the maximum sampling rate for PAL systems"), and the PAL
   reuse count is MEASURED.
7. **Period 0.** p.140 "the maximum is 65535" against p.159 "its full range
   124-65536". What the value 0 does is not stated. It is excluded.
8. **PAL line length.** p.23: "In PAL, they are all 227 long." p.304: "PAL
   (625 lines, 227.5 colorclocks per scan line)". It affects only the
   anchor check F1 (c), which accepts every line length between the two; for
   NTSC every line is graded against the two lengths p.23 names.
9. **"Each allocated DMA time slot".** p.148 d: "Start retrieving audio data
   each allocated DMA time slot." Read literally, this would fetch in every
   line whatever the period. p.165 describes one request per word ("AUDxDR
   Audio DMA request to Agnus for one word of data"), which is the only
   reading under which a table plays in order at long periods. F1 (b) uses
   the per-word reading, and only as a cross-check of the time base.
10. **Figure 5-8** (p.166, the audio state diagram) is not used for any
    expected value beyond item 11. The claims do not rest on it.
11. **What p.141 prints, and what p.164 explains.** p.141: "If the period
    value is below 124, by the time the cycle count has reached 0, the audio
    DMA will not have had enough time to retrieve the next data sample and
    the previous sample will be reused." (PAL: "a value of at least 123
    ticks/sample must be written into the period register.") So p.141 states
    a late word at every period below 124 (NTSC) and below 123 (PAL), and
    MP2 and MP4 grade p.141's statement at every such period, `P` = 120-123
    (NTSC) and 120-122 (PAL) included. p.164 gives the mechanism:
    "DMA requests and restart requests are transferred to Agnus once each
    horizontal line, and the data comes back about 14 clock cycles later (the
    duration of a clock cycle is 280 ns)." The request goes out on the
    011->010 transition (p.164: "requests occur as they do in normal operation
    (on the 011->010 transition)"; Figure 5-8, p.166), and the next 011->010
    transition comes `2 * P` ticks later (p.164: "Transitions such as
    010->011->010 occur whenever the period counter counts down to one"). A
    word requested just after a line's transfer waits one line, `L` ticks,
    and then about 14 ticks for the data; `L` is 227 ticks, or 228 on a long
    NTSC line (p.23). This explains what the runs measure below the minimum.
    It is not graded: "about 14" prints no bound, so p.164 gives no printed
    prediction that a word is in time at `P` = 120-123 (NTSC) or 120-122
    (PAL), and it is not a statement that contradicts p.141 there. It is
    the reason the window of 60,000 ticks may hold no late word at some of
    these periods, and a red line there reads "within W".

## 6. What the build adds

Everything in this section is a program, a stimulus placement, a mutation or a
measured value of the build; none of it changes a proposition or an expected
value above. Measured values are from `make -k -B test` on fx68k and on
TG68K; where the two cores differ the section says so, and where it does not,
both cores printed the same.

### 6.1 Programs and placement

- `prog_minper.s` is the program of section 3.3. `T` = $002000, `T3` =
  $002200, the result block `R` = $0C0100 (`R+0` ... `R+8`), the fixture
  constant $BEEF at $0C0120, and the two parameter words (`P` at $0C0200,
  `N_run` at $0C0202) that the test pokes before the boot. Step 0 writes
  `DDRA` <- $03 and `PRA` <- $02 with byte writes. The fixture constant is
  stored after step 4 and read back first (`FIXTURE-CONST`).
- The contrasts are assembly-time symbols, each set by a source of its own
  that includes `prog_minper.s`: `prog_minper_mc2dma.s`,
  `prog_minper_mc2adk.s`, `prog_minper_mc3len.s` (`AUD0LEN` <- 32),
  `prog_minper_mc3oth.s` (channel 1 on a copy of the table at `T3`),
  `prog_minper_mc3a.s` (`AUD0PER` <- 448 after 753 lines of the wait),
  `prog_minper_mc3b.s` (`AUD0PER` <- 224 after 800 lines),
  `prog_minper_mc3f4.s` (the CPU read of `T+64` after 1140 lines),
  `prog_minper_mc4len.s`, `prog_minper_mc4cpu.s`, `prog_minper_mc5mark.s`,
  `prog_minper_mc5stop.s` (the stop after 200 lines of the wait and a delay of
  30 loop passes), `prog_minper_mp5ch1.s`, `prog_minper_mp6len.s`
  (`AUD0LEN` <- 32 after `N_run / 2` lines) and `prog_minper_fixmp.s`. The
  contrast of the done marker (`FIXDONE`) runs `prog_nodone.s` of the earlier
  part of the package. The period contrasts (MP1-MP4, MC3 time scale, MC6)
  change only the poked parameter, the standard contrast (MC1) only the
  harness standard. The vasm listings of the normal program and of the MC2,
  MC4 and MP5 variants show `MOVE.W #$0002,(A1)` for the word at `T+128` and
  `MOVE.W T+64,D0` for the CPU read; the program is assembled for the 68000.
  The `MOVE.W #32,AUD0LEN(A0)` of the MP6 variant writes a non-zero constant.
- Values tuned on this RTL, each guarded by the lines of the run that show it
  took effect without disturbing the rest: the line numbers 753, 800 and 1140
  of the MC3 contrasts, and the delay of 30 loop passes of the MC5 stop. The
  line number 753 is the one at which the write falls in the last runs of the
  256 level runs of the reference window: F1 (a) goes red and F1 (b), F1 (c)
  and F4 stay green on both cores (section 6.4). The line number 800 falls
  after those 256 runs and inside the port window. The line number 1140 falls
  after the last wrap of the port window: F4 goes red and F1 (b) stays green.
  The delay of 30 is the stop that leaves MC6 green on both cores: with a
  delay of 20, 25, 35, 40 or 45 loop passes the output changes caused by the
  stop lie off the grid on one core or the other (up to 28 ticks on fx68k, up
  to 25 ticks on TG68K) and MC6 goes red there.
- One test binary runs `SPECIFICATION.md`'s test, then NTSC and PAL of this
  addendum at the same time in two processes, and prints one summary line.
- The standard is selected by the harness's `chipset_config` (ECS, bit 1
  set for NTSC). The harness runs 2 MB of Chip memory.
- `window(P)` = `max(60000, 454 * P)` ticks in every run, the reference runs
  included.

### 6.2 Reference frames, measured

Reference run `P_ref = 447`, NTSC and PAL, fx68k and TG68K:

- The channel-0 output is `audio_left`; `audio_right` does not change after
  the first fetch (0 changes). `audio_left` changes 591 times on fx68k and
  592 times on TG68K in that span. Not graded.
- `D_ref` = 3576 cycles in both standards on both cores, the largest
  deviation of a level run from it 0, so one tick = 8.0000 cycles and `s` =
  0.125 tick. The 256 level runs show 128 distinct levels.
- The mean spacing of table fetches over 128 fetches is 7150.5 cycles,
  1.5 cycles from `2 * D_ref` = 7152; the tolerance is 14.195 cycles (the
  longest complete line, 1816 cycles, over 128 fetches, plus 2 / 256 cycle).
  The fetch time is known to one line, which is why the tolerance is a line
  over the number of fetches and not one cycle.
- The 893 complete lines of the port window are all 227.000 ticks long in
  NTSC and in PAL, none outside 226.875 ... 228.125 ticks (NTSC) and
  226.875 ... 227.625 ticks (PAL), so F1 (c) holds in both. None is 228
  ticks and none differs from its predecessor: the NTSC alternation of p.23
  and the 227.5-clock mean of p.194 are not present at the port; they are
  printed and not graded (`NM-HRM-010`).
- The F4 anchor is asserted in each reference run before F1 (c): fx68k
  checked 1160 refresh clusters in each reference run and TG68K 1161; every
  one holds four edges of `refresh` two slots apart, and the 227 table
  fetches of the port window lie at one offset from the line start. The
  cross-check "reads below word $1000" counts 4644 on fx68k (4 * 1160 =
  4640) and 4648 on TG68K (4 * 1161 = 4644).
- Channel 0's fetch lies 114 cycles (14.25 ticks) after the first refresh
  edge of a line, in every run, with the channel on its own.
- Line phases of the window's words (measured by a separate driver over the
  package's own capture, line and output-window code, outside the normal
  run, on both cores and in both standards): the output word boundaries of
  the window, one every `2 * P` ticks, fall on 227 different phases against
  the line start at `P` = 110, 116 and 120 (273, 259 and 250 word
  boundaries), and every line of the window is 227.000 ticks long.

### 6.3 Results

Period values below the minimum, fx68k and TG68K identical. The reuse count
(`c_j = c_{j-1}`) is 0 at every period of both standards. The runs show the
other classes: **repeat** (`c_j = c_{j-1} - 1`) and, from the repeat, a
**next** that returns to the sample before it.

| Standard | P | Periods in the window that are not next | MP2 / MP4 verdicts |
|---|---|---|---|
| NTSC | 1 | repeat 29735 of 59999 classified; next 30264; reuse 0 | (a) green, (b) red, (a) and (b) red |
| NTSC | 2-113 | repeat 14735 at P = 2, 204 at P = 64, 36 at P = 100, 4 at P = 112, 1 at P = 113; reuse 0 | (a) green, (b) red, (a) and (b) red |
| NTSC | 114-119 | repeat 1, 4, 6, 6, 5 and 2 (114 to 119 in turn); skip 1, 4, 6, 7, 5 and 2 | (a) green, (b) red, (a) and (b) red, 116 included |
| NTSC | 120-123 | all next, no repeat, no reuse | (a) red, (b) red, (a) and (b) red |
| PAL | 1-119 | as NTSC | (a) green |
| PAL | 120-122 | all next | (a) red |

- `P` = 116 in both standards: MC5's last-fetch and tenth clauses use channel
  0's reads at its slot offset, at any address, and are green. The window
  holds 5 reads off the table (the first of them T+128 at 59,020 ticks after
  the start of the port window), 6 skips and 7 unmapped periods.
- `P` = 110 in both standards: 130 unmapped periods, 7 repeats and one period
  after an unmapped period.
- MP1: 14 of 14 values of `S_hi` green. MP3: 15 of 15 (`P` = 123 and `S_hi`)
  green, MP3-123 included. At `P` = 428 the window holds 454 sample periods.
- MP5: `P` = 1-113 green, at most one table read in a line, 263 reads in the
  263 complete lines, 31,538 samples/second over the duration of those
  lines. The figure follows from the length of the NTSC lines of these runs,
  227.000 ticks: one word per line is 2 * 3,579,545 / 227 = 31,538
  samples/second, 0.2 % above p.141's 31,469, which rests on 227.5-tick
  lines.
- MP6: NTSC 137 runs and PAL 137 runs graded, 525 complete passes in the
  windows of each standard. The line of each standard is red, and names `P` =
  110, 116 and 118; every other period is green. Evidence (identical on both
  cores and in both standards):
  - `P` = 110: 63 reads off the table, the first T+128 at 28,829 ticks after
    the start of the port window; 1 double fetch (T+0 twice in a row at
    14,301 ticks); 130 unmapped periods;
  - `P` = 116: 5 reads off the table, the first T+128 at 59,020 ticks;
  - `P` = 118: 1 double fetch (T+0 twice in a row at 14,982 ticks) and 1 short
    pass (T follows T+124 at 29,737 ticks).
- `P` = 119-123 in the window `W = 60,000`: no off-table read, no double
  fetch, no short pass.
- Totals, `make -k -B test`: both cores print `2440 passed, 255 failed`, of
  which the earlier test contributes 126 passed, 0 failed. The 255 failures
  are the MP2 lines (123 of (b), 123 of (a) and (b) together, 4 of (a) at
  P = 120-123), the MP4 lines at P = 120-122 (3) and the MP6 lines of NTSC and
  PAL. The PASS and FAIL lines are identical on both cores.
- Wall time of a normal run, the two standards in two processes at once:
  fx68k about 13 minutes, TG68K about 21 minutes (the build of each core
  included).

### 6.4 Mutations

Each selector runs only the periods its contrast needs and runs the package's
own comparator. Every selector was run on both cores; the PASS and FAIL lines
are identical on both.

| `NMH_MUTATE` | Assertion or precondition | Stimulus | Result on both cores |
|---|---|---|---|
| `MP1` | MP1 | `P` = 119, `P` = 64 | both red (next 499 of 503; next 732 of 936) |
| `MP2` | MP2 | `P` = 124, `P` = 64 | 124 red in (a), (b) and (a) and (b); 64: (a) green, (b) red |
| `MP3` | MP3 | `P` = 119, `P` = 64 | both red |
| `MP4` | MP4 | `P` = 124, `P` = 64 | 124 red, 64 green |
| `MP5CH1` | MP5 (control, not a mutation) | NTSC 64, channel 1 on `T` | green, one read per channel in every line |
| `MP6GREEN` | MP6 | NTSC and PAL, `P` = 64 and 124 | green (the normal run is red at 110, 116, 118) |
| `MP6LEN` | MP6 | NTSC and PAL 64, `AUD0LEN` <- 32 after `N_run / 2` lines | MP6 red in both (4 short passes: T follows T+62); MC1, MC2, MC4, MC5 green |
| `MC1STD` | MC1 | declared and harness standard swapped | red in both |
| `MC2DMA`, `MC2ADK` | MC2 | channel 3 enabled, USE3PN set | red, other preconditions green |
| `MC3LEN` | MC3 item 2 | `AUD0LEN` <- 32 | red (64 distinct levels, the output live), item 1 green |
| `MC3OTH` | MC3 item 1 | channel 1 plays a copy of the table, volume 64 | red, item 2 and all anchors green |
| `MC3A` | MC3 anchor F1 (a) | `AUD0PER` <- 448 after 753 lines | red (deviation 7.938 cycles fx68k, 7.969 TG68K), the other lines green |
| `MC3B` | MC3 anchor F1 (b) | `AUD0PER` <- 224 after 800 lines | red (5012.917 against 7152.000 cycles), the other lines green |
| `MC3F4` | MC3 anchor F4 | CPU read of `T+64` after 1140 lines | red (2 offsets), the other lines green |
| `MC3TIME` | MC3 anchor F1 (c) | `AUD0PER` 448 declared 447 | red, F4 and the other anchors green |
| `MC4LEN`, `MC4CPU` | MC4 | length 65, CPU read at `T+64` | red, other preconditions green |
| `MC5MARK`, `MC5STOP` | MC5 | marker left out, stop after 200 lines and 30 loop passes at `P` = 64 | red; MC5STOP: both clauses red, MC1, MC2, MC4, MC6 green |
| `MC6PER`, `MC6BELOW` | MC6 | `AUD0PER` 129 declared 128, 101 declared 100 | red, 64 and 50 ticks from the grid |
| `FIXMP`, `FIXDONE` | FIXTURE-CONST, FIXTURE | constant $DEAD, `prog_nodone` | red |

- The contrasts of MP1 and MP3 are `P` = 119 and `P` = 64: p.141 predicts a
  late word at 119 (section 5, item 11), and it goes red through the MP1 and
  MP3 comparators. `P` = 123 (NTSC) and `P` = 122 (PAL) are no contrast of
  MP1 and MP3: p.141 puts them below the minimum, but no late word occurs
  there on this implementation. The same inputs are run through MP2 and MP4
  only at `P` = 124 and `P` = 64, so that their comparators are shown red at
  124 and live at 64.
- MP2 (b) and the (a) and (b) line are red at every period of the normal run,
  so no run shows them green: no stimulus at this boundary produces a sample
  period that reuses the sample just played (section 3.6), because the
  periods that are not next are repeats, a step back.
- `MC5STOP`: at `P` = 64 the port window holds 2 wraps and no read out of
  order up to the stop, so MC4 is green; MC5 is red through both clauses (no
  read at the channel offset in the first and last tenth of the window, the
  last such read before the output window ends); MC6 is green (largest
  distance from the grid 0.0000 ticks).
- `MC3TIME`: with `AUD0PER` 448 the tick is measured as 8.0179 cycles. The
  port window holds 895 complete lines of mean length 226.493 ticks, all
  outside 226.875 ... 228.125 ticks (NTSC) and 226.875 ... 227.625 ticks
  (PAL), in both standards on both cores, so F1 (c) is red through its own
  clause. The F4 anchor is green (0 of 1162 clusters on fx68k, 0 of 1163 on
  TG68K fail the shape check), and items 1 and 2 and anchors (a) and (b) are
  green.
- `MC4LEN` and `MC4CPU` print no proposition line: MP3 is not graded behind
  a failing MC4 clause that section 3.7 does not release (MC4LEN: no wrap in
  the window; MC4CPU: a read off the channel offset).
- `MC6BELOW` prints the MP4 line as "precondition failed", not graded.
- The contrast runs reuse the reference runs of the standards they need.
