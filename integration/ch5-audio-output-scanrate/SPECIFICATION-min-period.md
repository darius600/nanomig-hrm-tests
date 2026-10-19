# Audio DMA at and below the minimum period: specification addendum

This addendum specifies six propositions about audio DMA at, above and below
the minimum sampling period that p.141 gives, in NTSC and in PAL. Its inputs
are the twelve claims in `claims-spec-stage.jsonl` (all printed on p.141; MP1-MP5
rest on them, and MP6 rests on pp.139 and 147 and carries no claim id) and
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
| sol:5-p141.c4, opus:5-rate.c8 | MP1 |
| sol:5-p141.c10, opus:5-rate.c15 | MP2 |
| sol:5-p141.c11, opus:5-rate.c17 | MP3, MP4 |
| sol:5-p141.c12, opus:5-rate.c18 | MP3, MP4 |
| sol:5-p141.c3, opus:5-rate.c6 | MP5 |

Parts of these claims that no check here covers, and why:

- **"In order to save buffers"** (c4, c8): the page's reason for the design
  rate. No output shows a reason.
- **The design rate of 28,867 samples/second as a result of the MP2 runs**
  (c4, c8): "the hardware is designed to handle 28,867 samples/second" is not
  what the MP2 sub-result at P = 114-119 measures. A delivered rate above
  28,867 samples/second does not contradict a design rate of 28,867, so a late
  word at these periods does not establish the figure. c4 and c8 are carried
  by MP1 only, which grades that every next sample arrives at 124 and above.
- **"At least one sample period not next" at P <= 113** (c3, c6): any maximum
  below the requested rate predicts it. It does not establish the figure
  31,469. c3 and c6 are carried by MP5 only, which grades the per-line bound
  that the figure is derived from.
- **Periods 120-123 (NTSC) and 120-122 (PAL) inside "below 124" (c10, c15) and
  "below 123" (c11, c17, c12, c18):** reported, not graded: p.141 and p.164
  disagree there (section 5, item 11).
- **31,469 as a rate that is reached** (c3, c6): the page calls it "a
  theoretical maximum" and says in the next sentence that "the hardware is
  designed to handle 28,867 samples/second". No page predicts that any output
  reaches 31,469 samples/second, so nothing checks that it is reached. Only
  the bound is checked (MP5), and only in NTSC, because the claim's figure is
  built from NTSC numbers (262.5 lines/frame, 59.94 frames/second). The page
  gives no PAL figure.
- **Periods inside "at least 124" (c9, c14) and "at least 123" (c12, c18)
  that are not in the tested set `S_hi`:** 132-139, 141-159, 161-199,
  201-255, 257-319, 321-427 and 429 to 65535 (and, in PAL, none below 123).
  The propositions cover them, but they are not in the required set
  (section 3.2); `S_hi` is the tested set, chosen for the cost of a run. One run there, with its settling time and a window of
  at least 454 sample periods, costs from about 0.07 s of simulated time (at 429) to
  about 11 s (at 65535). The check establishes "at least" on the tested
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
| MP2 | NTSC | every value 1-119 graded; 120-123 run and reported (section 5, item 11) | at least one sample period does not carry the next sample, and at least one reuses a sample already played |
| MP3 | PAL | 123 and the set S_hi | as MP1 |
| MP4 | PAL | every value 1-119 graded; 120-122 run and reported (section 5, item 11) | at least one sample period does not carry the next sample; reuse is reported, not graded |
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
passes of the same window is `2 * D_ref` to within 0.5 %. Basis: each word
holds two samples (p.138: "the audio DMA retrieves one word (16 bits) at a
time and uses the sample it reads as two bytes of data"), and each word is
fetched once per pass (p.139, p.165: "AUDxDR Audio DMA request to Agnus for
one word of data");
(c) the mean line length of F4, in ticks, over the complete lines of the
port window. NTSC: the mean lies between 226.8 and 228.2 ticks. p.23: "Every
other line is a long line (228 color clocks, 0-$E3), with the others being
227 color clocks long."; p.194: "there are 227.5 "color clocks"". The range
takes in both lengths the page names, with a slack of 0.2 tick. PAL: the
mean lies between 226.8 and 227.7 ticks. The two pages that state it give
different values, and this range takes in both: p.23 says "In PAL, they are
all 227 long", and p.304 lists "PAL (625 lines, 227.5 colorclocks per scan
line)". Only the mean is graded, because the tick calibration needs only
the mean. Whether NTSC lines alternate between 228 and 227 ticks is printed
as a MEASURED line and not graded. The tolerances of this section (0.2 tick,
0.5 %, the PAL band 226.8-227.7) are choices of this specification, not
values a page gives.

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
(F2, F3), or `u` where a sample period is unmapped. Set `m = c_1`. `m` is the
furthest sample reached so far; `c_1` itself is not classified. (If `c_1` is
unmapped, count it as unmapped and start from the first mapped sample
period.) For `j = 2 ... N`:

| Condition (indices mod 128) | Class | `m` after |
|---|---|---|
| `c_j = u` | unmapped | unchanged |
| `c_j = m + 1` | **next** | `c_j` |
| `c_j = m` | **reuse** | unchanged |
| `c_j = m - 1` | **repeat** (reported, not graded) | unchanged |
| `c_j = m + k`, 2 <= k <= 63 | skip | `c_j` |
| anything else | other | unchanged |

The definition of **reuse** is what p.141 says: "the previous sample will be
reused", which is the sample just played (`c_j = m`). p.164 says "If the
sampling rate is set much higher than the normal maximum sampling rate
(approximately 29 KHz), the two samples in the buffer register will be
repeated", which would also allow a sample period that carries the other
sample of the last word (`m - 1`). That is reported as **repeat**,
separately, and never graded. The table has 128 different values (section
3.1), so a repeat cannot be taken for a new sample.

Each run reports `N` and the number of sample periods in each class, repeats included.

### 3.7 Preconditions

Every graded run asserts these first. If one fails, the run is reported as
"precondition failed" with its name, never as green or red for a
proposition. Each precondition has a contrast: a change of the stimulus for
which the pages predict that this precondition fails while the program still
finishes and the other preconditions hold. A contrast run is graded only for
the precondition it is the contrast for.

In the runs below the minimum (MP2, MP4 and MP5) MC6, and the "without a
gap" clause of MC4, are printed as MEASURED results and are not
preconditions there: they encode how the late sample is handled, which is the
clause under test. The other parts of MC4 (the fixed slot offset, no CPU
access, the anchor, at least one wrap) and MC1, MC2, MC3 and MC5 stay
preconditions in those runs. Below the minimum address order is a result, so
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
(MC1). In this variant, MC6 is judged only up to the last table fetch.

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
   next data sample." and "In order to save buffers, the hardware is
   designed to handle 28,867 samples/second." (3,579,545 / 124 = 28,867
   samples/second.)
   Quantifiers: "at least 124" is graded on every value of `S_hi`, each on
   its own; "the next data sample" is graded on every sample period of the
   window, not on one.
5. **Preconditions.** MC1-MC6 (MC3 from the NTSC reference run). If only
   MC4's address-order clause or MC5's table clauses fail, MP1 is graded and
   reported all the same, next to the verdict of that precondition (section
   3.7).
6. **Contrast.** The same run with `P = 119`. p.141 and p.164 both predict a
   late word there (section 5, item 11) and MP2 grades it, so the MP1 check
   goes red. A second contrast is `P = 64`. Both runs are already in the
   required set, and the setup and preconditions are unchanged. (`P` = 120-123
   are not used: the pages disagree there.)
7. **Reference frames.** F1, F2, F3, F5. F1 and F2 are checked by MC3, F3 by
   MC6, the window start by MC4 and MC5.

### MP2. NTSC: below 124, the next sample is not retrieved in time and the previous sample is reused

1. **Proposition.** In NTSC, with any period value from 1 to 119, the audio
   DMA does not always retrieve the next data sample before the count
   reaches 0, and a sample already played is output again. Claims
   sol:5-p141.c10, opus:5-rate.c15; p.141, and p.164 for the mechanism
   (section 5, item 11). The periods 120-123, which p.141 puts below its
   minimum but p.164 does not predict a late word for, are run and reported,
   not graded.
2. **Stimulus.** The program of section 3.3, NTSC, once for each `P` from 1
   to 123.
3. **Observation.** As MP1.
4. **Expected.** For every `P` from 1 to 119, separately:
   (a) at least one classified sample period is not **next**
   (opus:5-rate.c15 and sol:5-p141.c10); and
   (b) at least one is **reuse**, `c_j = m` (sol:5-p141.c10).
   From p.141: "If the period value is below 124, by the time the cycle
   count has reached 0, the audio DMA will not have had enough time to
   retrieve the next data sample and the previous sample will be reused."
   Quantifiers: "below 124" is graded on every value 1-119, and both pages
   predict a late word there (section 5, item 11). The page gives
   no rate, so only "at least one in the window" is graded. The window
   `W = 60,000` ticks is a stimulus choice of this specification (section
   3.3, which says what it covers). A line that is red reads "no reuse within
   W = 60,000 ticks" (or "every sample period carries the next sample within
   W = 60,000 ticks" for (a)), with the window named in its label. The numbers
   of next, reuse, repeat, skip, unmapped and other periods are reported.
   Skips are not graded (section 1).
   For `P` = 120-123 the same numbers are printed as MEASURED, with a
   RECORDED line "p.141 and p.164 disagree at these periods" that names both
   sentences (section 5, item 11); (a) and (b) are not graded there.
   *Sub-results*, reported separately with the same expected value and the
   same claim ids:
   **MP2-lo**, `P` = 1-113: a channel at these periods needs more than the
   "theoretical maximum" of 31,469 samples/second (3,579,545 / 31,469 =
   113.75 ticks per sample). p.164 points the same way for very high rates.
   **MP2-hi**, `P` = 114-119: these lie between that theoretical figure and
   the designed rate of 28,867 samples/second. Here the expected reuse rests
   on the quoted sentence and on p.164's timing alone. c3, c6, c4 and c8 are
   not on these lines (section 1).
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
5. **Preconditions.** MC1-MC6 (MC3 from the PAL reference run); as for MP1,
   MP3 is graded and reported even when only MC4's address-order clause or
   MC5's table clauses fail (section 3.7).
6. **Contrast.** The same run with `P = 119`. MP4 (a) predicts that not every
   next sample is retrieved there (both pages, section 5, item 11), so the MP3
   check goes red. A second contrast is `P = 64`. (`P` = 120-122 are not used:
   the pages disagree there.)
7. **Reference frames.** As MP1. For PAL, F1 (c) uses the range of mean line
   length that takes in both p.23's value and p.304's.

### MP4. PAL: below 123, the next sample is not retrieved in time and the previous sample is reused

1. **Proposition.** In PAL, with any period value from 1 to 119, the audio
   DMA does not always retrieve the next data sample in time, and a sample
   already played is output again. Claims sol:5-p141.c11, opus:5-rate.c17,
   sol:5-p141.c12, opus:5-rate.c18; p.141, and p.164 for the mechanism
   (section 5, item 11). The periods 120-122 are run and reported, not
   graded.
2. **Stimulus.** The program of section 3.3, PAL, once for each `P` from 1
   to 122.
3. **Observation.** As MP2, with the PAL reference run.
4. **Expected.** For every `P` from 1 to 119: (a) at least one classified
   sample period is not **next**, within the window `W = 60,000` ticks
   (a stimulus choice, section 3.3). Basis: p.141 makes 28,867
   samples/second "also the maximum sampling rate for PAL systems" and
   requires "at least 123 ticks/sample" (3,546,895 / 119 = 29,805
   samples/second, above that maximum), so the audio DMA cannot retrieve
   every next sample in time; p.164's timing predicts the same (section 5,
   item 11). For `P` = 120-122 the counts are printed as MEASURED with the
   RECORDED line of section 5, item 11, and (a) is not graded.
   (b) The page prints the consequence "the previous sample will be reused"
   only in the NTSC paragraph, the sentence quoted under MP2; applied
   literally to PAL it would predict reuse at 123 as well, which the same
   page's PAL minimum of 123 excludes. So the number of **reuse** periods in
   PAL is MEASURED and not graded. p.164's statement about rates "much
   higher than the normal maximum sampling rate (approximately 29 KHz)"
   names no standard. No sub-results: the claims give no theoretical PAL
   figure.
5. **Preconditions.** MC1-MC6 (PAL reference run).
6. **Contrast.** The same run with `P = 124`. Every page puts PAL 124 at or
   above the minimum (p.140, p.141, p.266), so the MP4 (a) check goes red. 123
   is not used as the contrast, because it is the disputed value (MP3-123).
   The MP3 contrast at `P` = 119 is the same measurement seen from the other
   side.
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
   2 * (reads in the complete lines) / (duration of those complete lines in
   seconds), not graded on its own. The duration is that of the complete lines
   the reads were counted in, never the length of the whole window. With
   lines of 227 ticks the figure is 2 * 3,579,545 / 227 = 31,538
   samples/second, 0.2 % above p.141's 31,469, which rests on 227.5-tick lines
   (262.5 lines/frame); the record says that the figure follows from the
   227-tick lines of the NTSC runs.
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
   its own sequence in address order. Channel 1's output is on the other output, at
   volume 0, so it is "no output" (p.266). This contrast shows that the
   per-line count works: it counts two reads when two channels fetch in a
   line. It does not show that one channel can exceed its slot; no page
   stimulus makes one channel take two slots in a line.
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
   sample just played (`c_j = m`). A period that carries the other sample of
   the last word (`m - 1`) is reported as a repeat (section 3.6).
4. **Theoretical against designed rate.** p.141 derives 31,469
   samples/second from the slot allocation (a period of 113.75 ticks), yet
   puts the minimum at 124 because "In order to save buffers, the hardware
   is designed to handle 28,867 samples/second". It does not describe the
   mechanism that makes 114-123 fail; p.164 does (item 11). Handling: the
   MP2-hi and MP2-lo sub-results, graded to 119 (item 11).
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
   anchor check F1 (c), which accepts the range between the two; for NTSC the mean line length is
   graded against the range between the two lengths p.23 names.
9. **"Each allocated DMA time slot".** p.148 d: "Start retrieving audio data
   each allocated DMA time slot." Read literally, this would fetch in every
   line whatever the period. p.165 describes one request per word ("AUDxDR
   Audio DMA request to Agnus for one word of data"), which is the only
   reading under which a table plays in order at long periods. F1 (b) uses
   the per-word reading, and only as a cross-check of the time base.
10. **Figure 5-8** (p.166, the audio state diagram) is not used for any
    expected value beyond item 11. The claims do not rest on it.
11. **Where p.141 and p.164 predict a late word.** p.141: "If the period
    value is below 124, by the time the cycle count has reached 0, the audio
    DMA will not have had enough time to retrieve the next data sample and
    the previous sample will be reused." (PAL: "a value of at least 123
    ticks/sample must be written into the period register.") So p.141
    predicts a late word at every period below 124 (NTSC) and below 123
    (PAL). p.164 gives the mechanism: "DMA requests and restart requests are
    transferred to Agnus once each horizontal line, and the data comes back
    about 14 clock cycles later (the duration of a clock cycle is 280 ns)."
    The request goes out on the 011->010 transition (p.164: "requests occur
    as they do in normal operation (on the 011->010 transition)"; Figure 5-8,
    p.166), and the next 011->010 transition comes `2 * P` ticks later
    (p.164: "Transitions such as 010->011->010 occur whenever the period
    counter counts down to one"). A word requested just after a line's
    transfer waits one line, `L` ticks, and then about 14 ticks for the
    data, so it can be late when `2 * P < L + 14`. `L` is 227 ticks, or 228
    on a long NTSC line (p.23: "Every other line is a long line (228 color
    clocks, 0-$E3), with the others being 227 color clocks long. In PAL, they
    are all 227 long."). The page says "about 14", and this specification
    allows two ticks for it, a choice of the specification: both pages
    predict a late word where `2 * P < L + 12`. With `L` = 228 (NTSC) that is
    `P` <= 119, and with `L` = 227 (PAL) it is also `P` <= 119. Handling:
    MP2 (NTSC) and MP4 (PAL) are graded at `P` = 1-119. At `P` = 120-123
    (NTSC) and 120-122 (PAL) p.141 predicts reuse and p.164 predicts none, or
    none for certain, so these periods are run, their counts are printed as
    MEASURED, and a RECORDED line says "p.141 and p.164 disagree at these
    periods" and names both sentences. They are not graded.

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
  `prog_minper_mc2adk.s`, `prog_minper_mc3vol.s`, `prog_minper_mc4len.s`,
  `prog_minper_mc4cpu.s`, `prog_minper_mc5mark.s`, `prog_minper_mc5stop.s`,
  `prog_minper_mp5ch1.s` and `prog_minper_fixmp.s`. The period contrasts
  (MP1-MP4, MC3 time scale, MC6) change only the poked parameter, the
  standard contrast (MC1) only the harness standard. The vasm listings of
  the normal program and of the MC2, MC4 and MP5 variants show
  `MOVE.W #$0002,(A1)` for the word at `T+128` and `MOVE.W T+64,D0` for the
  CPU read; the program is assembled for the 68000.
- One test binary runs `SPECIFICATION.md`'s test, then NTSC and PAL of this
  addendum at the same time in two processes, and prints one summary line.
- The standard is selected by the harness's `chipset_config` (ECS, bit 1
  set for NTSC). The harness runs 2 MB of Chip memory.

### 6.2 Reference frames, measured

Reference run `P_ref = 447`, NTSC and PAL, fx68k and TG68K:

- The channel-0 output is `audio_left`; `audio_right` does not change (394
  changes after the first fetch on TG68K, 393 on fx68k). Not graded.
- `D_ref` = 3576 cycles in both standards on both cores, the largest
  deviation of a level run from it 0, so one tick = 8.0000 cycles and `s` =
  0.125 tick.
- The mean spacing of table fetches over 64 fetches is 7150.5 cycles, 0.021
  % from `2 * D_ref`.
- The 503 complete lines of the port window are all 227.000 ticks long in
  NTSC and in PAL; none is 228 ticks and none differs from its predecessor.
  The NTSC alternation of p.23 is therefore not present at the port; it is
  printed and not graded (section 3.5, F1 (c)).
- 770 refresh clusters were checked in each reference run; every one holds
  four edges of `refresh` two slots apart. The cross-check "reads below word
  $1000" counts 3084 on fx68k and 3088 on TG68K against 4 * 770 = 3080.
- Channel 0's fetch lies 114 cycles (14.25 ticks) after the first refresh
  edge of a line, in every run, with the channel on its own.

### 6.3 Results

Period values below the minimum, fx68k and TG68K identical:

| Standard | P | Next-sample periods in the window | Verdict |
|---|---|---|---|
| NTSC | 1 | 529 of 59999 classified | MP2 (a), (b) green; MP5 green (1 read per line) |
| NTSC | 114-119 | 1 to 6 reuse periods in the window | MP2 green, except 116 (below) |
| NTSC | 120-123 | all next, no reuse | MP2 (a) and (b) red: "no reuse within W = 60000 ticks" |
| PAL | 1-119 | as NTSC | MP4 (a) green, except 116 (below) |
| PAL | 120-122 | all next | MP4 (a) red |

- `P` = 116 in both standards: MC5 fails (the last table fetch is not after
  the end of the output window; 5 fetches off the address order, 6 skips and
  7 unmapped periods are measured in the window), so MP2 and MP4 are not
  graded at 116.
- MP1: 14 of 14 values of `S_hi` green. MP3: 15 of 15 (`P` = 123 and `S_hi`)
  green, MP3-123 included. MP5: `P` = 1-113 green, at most one table read in
  a line, 263 reads in 263 lines (31,381 samples/second by the reported
  formula).
- `P` = 114-119 are above the page's theoretical maximum period of 113.75
  ticks and the reuse there is present but rare (1 to 6 periods in a window).
- Totals, `make -k -B test`: both cores print `2309 passed, 13 failed`, of
  which the earlier test contributes 126 passed, 0 failed. The 13 failures
  are the MC5 line of NTSC 116 and PAL 116, the eight MP2 lines (a) and (b)
  for `P` = 120-123, and the three MP4 (a) lines for `P` = 120-122.
- Wall time of a normal run, the two standards in two processes at once
  while other simulations ran on the machine: fx68k 2196 s, TG68K 3702 s.

### 6.4 Mutations

Each selector runs only the periods its contrast needs and runs the package's
own comparator.

| `NMH_MUTATE` | Assertion or precondition | Stimulus | Result on both cores |
|---|---|---|---|
| `MP1` | MP1 | `P` = 123, `P` = 64 | 123 green, 64 red |
| `MP2` | MP2 | `P` = 124, `P` = 64 | 124 red ((a), (b)), 64 green |
| `MP3` | MP3 | `P` = 122, `P` = 64 | 122 green, 64 red |
| `MP4` | MP4 | `P` = 124, `P` = 64 | 124 red ((a)), 64 green |
| `MP5CH1` | MP5 | NTSC 64, channel 1 on `T` | red, 2 reads in a line |
| `MC1STD` | MC1 | declared and harness standard swapped | red in both |
| `MC2DMA`, `MC2ADK` | MC2 | channel 3 enabled, USE3PN set | red, other preconditions green |
| `MC3VOL`, `MC3TIME` | MC3 | volume 0, `AUD0PER` 894 declared 447 | item 2 red, anchor F1 (c) red |
| `MC4LEN`, `MC4CPU` | MC4 | length 65, CPU read at `T+64` | red, other preconditions green |
| `MC5MARK`, `MC5STOP` | MC5 | marker left out, stop after 100 lines at `P` = 64 | red |
| `MC6PER` | MC6 | `AUD0PER` 129 declared 128 | red, 64 ticks from the grid |
| `FIXMP` | FIXTURE-CONST | constant $DEAD | red |

- The specification's contrast for MP3 (`P` = 122) does not turn MP3 red,
  because the pages' PAL consequence (MP4 (a)) does not occur at 122 on this
  implementation (every period is next at 120-122). `P` = 64 is added as the
  input for which the page predicts the failure by the same argument (3,546,895
  / 64 exceeds the PAL maximum); it goes red. The same two inputs are run
  through MP4 so that its comparator is shown live at 64 and red at 124.
- `MC5STOP`: at `P` = 64 the port window starts after the stop and holds no
  wrap, so MC4 also fails there, besides MC5.
- The contrast runs reuse the reference runs of the standards they need.
