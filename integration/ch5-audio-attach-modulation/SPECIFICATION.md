# Audio channel attachment and modulation: test specification

This specification was written from this package's claims
(`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference
Manual*, 3rd edition, by an author who never saw the implementation or any
test. It turns the claims into ten propositions about attaching one audio
channel to another through the attach bits of ADKCON (bits 7-0). For each
proposition it gives the 68000 program's stimulus, what the test observes,
the value or relation the page predicts, the preconditions, and a
contrasting case. Page numbers are printed page numbers.

## Claims not turned into a proposition

| Claim | Reason |
|---|---|
| 5-mod.c11, 5-p149.c17 | p.149, volume-modulator word format: "15 - 7 Not used". The page gives no value for these bits and does not say what happens when they are set, so the claim gives no expected value. Every volume word in this package has bits 15-7 = 0, so no check depends on them. |
| 5-p165.c4, 5-sig.intreq1 | "intreq1 Interrupt request that combines with intreq2 to form AUDxIR" (p.165). intreq1 and intreq2 are local signals (p.164: "Upper-case names indicate external signals; lower-case names indicate local signals"). The claim says that the two combine, but not how or when. No port access, audio sample or register value tells one way of combining from another. |

These claims are used only in part:

| Claim | Part used, and why the rest is not |
|---|---|
| 5-mod.c12, 5-p149.c18, 5-appA-p265.c31, A-adkcon.note2 | Volume bits V5-V0 are used (P1, P4). V6 is not, because the pages disagree about the width of a volume value. p.139: "There is a six-bit volume register for each audio channel ... You can specify values from 64 to 0." p.140: "For any volume setting from 64 to 0, you write the value into bits 5-0 of AUD0VOL." Against them, p.149: "6 - 0 Volume information, V6 - V0"; p.265: "First word xxxxxxxx V6-V0"; and p.266 (AUDxVOL): "06 Forces volume to max (64 ones, no zeros)", "05-00 Sets one of 64 levels". Every page agrees that values 0-63 are volumes carried in bits 5-0. All volume words here are 0-63, with bit 6 = 0. |
| 5-p165.c22, 5-sig.napnav | The DMA-request half is used (P10). The claim also says napnav conditions the normal *interrupt* requests. With attach period alone, Figure 5-8 routes the interrupt through a different bracket on 010->011 that no claim of this package records. So the claim gives no interrupt time for that mode. In attach-volume mode it predicts only "as normal", which is the same loop phase that P10 already measures on the DMA requests. |
| 5-mod.c19, 5-p150.c22 | "Channels are attached by the system in a predetermined order, as shown in Table 5-5." The six fixed pairs are checked (P6). The claim also implies that no other pairing can be selected. That describes a control that does not exist, and nothing can be observed about it beyond the six pairs. |

## Conventions

### System

PAL, Enhanced Chip Set, 2 MB of Chip memory. Nothing in these claims
depends on OCS/ECS or PAL/NTSC. For ECS, p.166 points to Appendix C, whose
audio changes (p.298, p.310) are the wider AUDxLCH and the maximum of
AUDxPER "with programmable scan rates". The program never writes BEAMCON0,
so the fixed scan rate and the p.140 maximum of 65535 apply. Every check is
run with each CPU core, each in its default 68000 behaviour. Nothing in
these claims depends on the CPU model, and none of them involves the disk.

The program runs with all interrupts disabled (INTENA = $7FFF at start;
p.280, bit 15 = 0 clears). All DMA other than audio stays off (DMACON =
$7FFF at start). The CPU never reads or writes an audio data region while a
session runs. All data sets and the marker and result words lie in the
first 256 KB of Chip memory. Data sets start on a word boundary (p.138:
"the first byte of data must start at a word-address boundary"), and each
channel's data set lies in its own region, so every audio fetch at the
Chip-memory port can be attributed to one channel and one word offset.

Register addresses (Appendix B, p.289-291; Appendix A, p.265-266, p.278,
p.280): ADKCON $DFF09E (write), ADKCONR $DFF010 (read), DMACON $DFF096,
DMACONR $DFF002, INTENA $DFF09A, INTREQ $DFF09C, VHPOSR $DFF006. For audio
channel x (x = 0-3) the base is $DFF0A0 + $10·x: AUDxLCH +0, AUDxLCL +2,
AUDxLEN +4, AUDxPER +6, AUDxVOL +8.

### Output sides and silence

p.137: "Channels 1 and 2 are connected to the left-side stereo output jack.
Channels 0 and 3 are connected to the right-side output jack." A channel is
observed on its side while every other channel on that side is either off
(AUDxEN clear, AUDxVOL = 0) or at volume 0. Volume 0 is silence: p.163,
Table 5-9, volume 0 = "Minus infinity"; p.266, "000000=no output". If the
harness offers the four channel outputs separately, the builder may observe
the channel's own output instead. Every expected value below holds there as
well.

This package needs the audio output to show every change of a channel's
output value. If the harness delivers audio more coarsely than one sample
per tick, the builder averages each duration over all complete samples of
the interval it belongs to (see *Measurements*) and states the resulting
resolution. The builder may not change a stimulus value to compensate.

### Units

A **tick** is one period of the clock constant. p.140: "In units, the
period value represents clock ticks per sample." p.141: 3,546,895 ticks per
second on PAL. The test does not rely on the harness's clock ratio.
Instead, it takes **D_mod**, the median sample duration of channel 0 in
session R-MOD (AUD0PER = 4000), as the length of one modulator period, and
defines 1 tick = D_mod / 4000.

**λ = 300 ticks** is the bound on the time from a DMA request to the fetch
it causes. p.164: "DMA requests and restart requests are transferred to
Agnus once each horizontal line, and the data comes back about 14 clock
cycles later (the duration of a clock cycle is 280 ns)". One line is about
227.5 ticks (p.141: 3,579,545 ticks/s over 262.5 lines/frame × 59.94
frames/s), so one line plus 14 ticks plus margin gives 300. Every use of λ
separates events at least 4000 ticks apart, so its exact value is not
critical.

### Markers and results

Right after every register write that starts or ends something
(DMACON enable and disable, each reference-sweep write), the CPU writes a
distinct word to a fixed marker address in Chip memory. The port shows that
write with its time, and it follows the register write by one instruction.
**M_on** and **M_off** are the markers after a session's DMACON enable and
disable writes. The CPU also stores the words it reads from ADKCONR and
DMACONR into a result area.

### Data sets

All values are 16-bit words. "Bytes" are the two samples a normal channel
plays from a word, high byte first (p.138: "uses the sample it reads as two
bytes of data"; p.164: "In the 010 state the upper byte is output, and in
the 011 state the lower byte is output").

| Name | Words | Use |
|---|---|---|
| SQ | $649C repeated LEN times (bytes +100, -100) | modulated channel: every sample differs from the one before, so every sample boundary is visible, and each volume gives one level pair |
| RAMP | $0818 $2838 $4858 $6878 $F8E8 $D8C8 $B8A8 $9888 (16 distinct bytes: 8, 24 ... 120, -8 ... -120) | modulated channel in S-P2: each sample identifies its position in the waveform |
| M_V | $0020 $0001 $0002 $0004 $0008 $0010 $003F $0015 (32, 1, 2, 4, 8, 16, 63, 21) | volume words: each of bits 0-5 alone, all six, and a mixed value; bits 15-6 = 0 |
| M_P | $0190 $0191 $0192 $0194 $0198 $0180 $01B0 $01D0 $0110 $4190 $0090 $8190 $0390 $2190 $0590 $1190 $0990 (400, 401, 402, 404, 408, 384, 432, 464, 272, 16784, 144, 33168, 912, 8592, 1424, 4496, 2448) | period words: the base 400 and each of the 16 bits toggled against it once; all ≥ 144 (p.141 minimum 124). The order keeps each word's interval long enough in S-P2 |
| M_P4 | $012C $02BC $01F4 $0384 (300, 700, 500, 900) | period words for timing and pairing |
| M_VP | $0018 $012C $0038 $02BC $0008 $01F4 $0028 $0384 (V24, P300, V56, P700, V8, P500, V40, P900) | dual mode: odd positions are volumes, even positions periods |
| M6V | $0008 $0030 $0014 $003C (8, 48, 20, 60) | volume words for pairing |
| M_LA | $0008 $0030 $0014 $003C $0020 (8, 48, 20, 60, 32) | length independence, 5 words |
| M_LB | $0008 $0030 (8, 48) | length independence, 2 words |
| W_V | $0030 $0010 (bytes 0, 48, 0, 16; as volume words 48, 16) | tripwire and channel-3 data, volume sessions |
| W_P | $0258 $03E8 (bytes 2, 88, 3, 232; as period words 600, 1000) | tripwire and channel-3 data, period sessions |

The values in each modulator data set are distinct, so every value the
modulated channel takes identifies the word it came from. Words at the port
are identified by their offset from AUDxLC.

### Program sequence for one session

The sequence follows the order p.145 gives in "Audio Summary", steps 3-7.

1. All AUDxEN clear (DMACON = $000F, since the SET/CLR bit 15 = 0 clears;
   p.145: "You can stop the channel by writing a 0 into the AUDxEN bit").
   The CPU waits at least twice the longest period of the previous session,
   plus 1000 ticks. p.145: "If the channel is disabled for a very short time
   (less than two sampling periods) it may stay on and thus continue from
   where it left off."
2. ADKCON = $00FF. This clears attach bits 7-0 and leaves bits 14-8 alone
   (p.265, bit 15: "Bits written with a zero are always unchanged").
3. For each channel the session uses: the location as a long word to AUDxLCH
   (p.139: "writing a long word into AUDxLCH moves the audio data address
   into both locations"), then AUDxLEN, AUDxVOL and AUDxPER (p.145 steps 3-6).
   Unused channels: AUDxVOL = 0 and AUDxEN clear.
4. ADKCON = $8000 | attach mask (p.150: "To attach a channel as a modulator,
   you set its attach bit to 1"). No page orders the attach write against
   the DMA start. The attach bits are set while all channels are off, so no
   check depends on the channel state at the moment of attaching.
5. Read ADKCONR and store the word.
6. DMACON = $8200 | channel mask (p.144: "When both DMAEN and AUDxEN are
   set, the DMA will start for channel x"; p.145 step 7). Write M_on. Read
   DMACONR and store the word.
7. Wait for the session's duration, for example by counting lines through
   VHPOSR.
8. DMACON = channel mask (clear). Write M_off. ADKCON = $00FF.

A reference sweep (R-VOL, R-PER, R-RAMP) inserts register writes during
step 7, each followed by a marker.

### Sessions

Columns: channel entries are data, LEN, PER, VOL. "off" means AUDxEN clear
and AUDxVOL = 0. Durations are minimum ticks from M_on to M_off.

| Session | ADKCON mask | ch0 | ch1 | ch2 | ch3 | Duration | Serves |
|---|---|---|---|---|---|---|---|
| R-MOD | $00 | M_V, 8, 4000, 63 | off | off | off | 200,000 | D_mod, P3, P8, P10 anchor |
| R-DAC-P4 / R-DAC-VP | $00 | M_P4, 4 / M_VP, 8; 4000, 63 | off | off | off | one data-set cycle + 8,000 | P8 preconditions |
| R-DAC3-V / R-DAC3-P | $00 | off | off | off | W_V / W_P, 2, 2000, 63 | 30,000 | P8 preconditions |
| R-VOL-c (c = 1, 2, 3) | $00 | off unless c | | | | 4,000 per value | level references |
| R-PER-c (c = 1, 2, 3) | $00 | off unless c | | | | 20,000 per value | duration references |
| R-RAMP | $00 | off | RAMP, 8, 700, 48 | off | off | see below | P2 references |
| S-AV | $01 | M_V, 8, 4000, 63 | SQ, 4, 200, 40 | off | off | 200,000 | P1, P3, P5, P8, P10 |
| S-AP | $10 | M_P4, 4, 4000, 63 | SQ, 4, 200, 40 | off | off | 110,000 | P5, P8, P10 |
| S-BOTH | $11 | M_VP, 8, 4000, 63 | SQ, 4, 200, 63 | off | off | 110,000 | P4, P5, P8, P10 |
| S-P2 | $10 | M_P, 17, 35000, 63 | RAMP, 8, 700, 48 | off | off | 1,400,000 | P2, P3 |
| S-PAIR-b | see P6 | | | | | 110,000 | P6 |
| S-CH3-V / S-CH3-P | $08 / $80 | see P7 | | | | 60,000 | P7 |
| S-DAC3-V / S-DAC3-P | $08 / $80 | off | off | off | W_V / W_P, 2, 2000, 63 | 30,000 | P8 |
| S-LEN-A | $01 | M_LA, 5, 4000, 63 | SQ, 3, 200, 40 | off | off | 150,000 | P9 |
| S-LEN-B | $01 | M_LB, 2, 4000, 63 | SQ, 7, 200, 40 | off | off | 60,000 | P9 |

Reference sweeps. Each channel runs alone, with ADKCON attach bits clear.

- **R-VOL-c**: channel c plays SQ (LEN 4, PER 200). The CPU writes AUDcVOL
  to each value of VSET_c in turn and holds each for at least 4,000 ticks,
  with a marker after each write. VSET_1 = {40, 1, 2, 4, 8, 16, 20, 21, 24,
  32, 48, 56, 60, 63}; VSET_2 = VSET_3 = {40, 8, 20, 48, 60}.
- **R-PER-c**: channel c plays SQ at VOL 40. The CPU writes AUDcPER to 200,
  300, 500, 700 and 900 in turn and holds each for at least 20,000 ticks.
- **R-RAMP**: channel 1 plays RAMP at VOL 48. The CPU writes AUD1PER to 700
  and then to each M_P value in M_P order. Each value is held for (previous
  value + 21 × value) ticks when it is at most 4000, otherwise for (previous
  value + 3 × value) ticks.

The reference sessions may run in the same program run as the checks or in
a separate run of the same build and configuration.

### Measurements

- **Sample boundary**: an instant at which the output of the observed side
  changes value. With SQ and RAMP every boundary is visible at any volume
  above 0. **Duration**: the time between consecutive boundaries.
- **Level pair** of an interval: the two settled output values of an SQ
  channel, one for the +100 sample and one for the -100 sample. **Reference
  level pair** L_c(v): the pair measured in R-VOL-c (or R-PER-c for v = 40)
  in the middle of the marker interval for v, excluding the first two samples
  after each marker.
- **Reference duration** D_c(P): the median complete-sample duration of
  channel c in R-PER-c (or R-RAMP) for the value P.
- **Applied volume** of an interval: the v whose reference level pair equals
  the interval's. **Applied period** of a run of equal durations (within 0.5
  tick): the P whose reference duration equals the run's mean. The first
  sample after a change of the written period still has the old duration.
  p.164: "The period counter is reloaded at these transitions". So a run
  begins at the first sample with the new duration.
- **Change instant C**: for a volume change, the time of the first output
  sample that shows the new level pair; for a period change, the start of
  the first sample of the new run. Both are corrected by the audio latency
  **L_a**, the minimum over all R-VOL-1 writes of (first output sample at
  the new level − marker time).
- **Fetch record** of channel x: port reads inside x's data region, each with
  time and word offset. **F_n** is the n-th fetch of channel x after M_on.
- **Cyclically consecutive in S**: a sequence of values A_1 ... A_n where
  every A_i is in the data set S and A_(i+1) is the word that follows A_i in
  S, wrapping from the last word to the first.

### Common preconditions

Every session asserts these. Each one comes with a stimulus change that
makes it fail, so that the check itself can be shown to go red.

| Id | Precondition | Contrast that makes it fail |
|---|---|---|
| PC1 | Bits 7-0 of the stored ADKCONR word equal the session's attach mask (p.265: "ADKCONR 010 R P Audio, disk, control read"). | Omit the step-4 ADKCON write in an attach session (bits read 0); or write $8001 in a reference session. |
| PC2 | The stored DMACONR word has bit 9 (DMAEN) and the session's AUDxEN bits set (p.278). | Enable with $8000 \| channel mask, without DMAEN. Bit 9 then reads 0, and no audio fetch occurs (p.144: "For any audio DMA to occur ... the DMAEN bit in DMACON must be set"). |
| PC3 | Every enabled channel has fetches in its own region, at offsets stepping 0, 1, ..., LEN-1, 0, ... (p.147: "Each time the length counter reaches zero, both the location and length registers are reloaded"). The modulator has at least two complete cycles inside the proposition's window. | Leave that channel's AUDxEN bit out of the enable write: the channel makes no fetches. |
| PC4 | In R-VOL-c the reference level pairs of all VSET_c values are pairwise distinct. In R-PER-c and R-RAMP the reference durations are pairwise distinct and rise strictly with the written value. | In the sweep, write 40 in the step labelled 60: two labels then have the same pair. |
| PC5 | Side anchor: in every R-VOL-c and R-PER-c the channel's output appears on the side p.137 names, and the other side stays constant. | In R-VOL-1, also enable channel 0 playing SQ at VOL 40: the right side is no longer constant. |
| PC6 | Unit anchor: in R-MOD, channel 0's sample durations differ from D_mod by at most 1 part in 4000. | CPU writes AUD0PER = 5000 halfway through R-MOD: the spread fails. |
| PC7 | Loop anchor (P10): in R-MOD, for every n ≥ 4, F_n − F_2 − 2(n−3)·D_mod lies in [−100, +600] ticks. | CPU writes AUD0PER = 3000 halfway through R-MOD: the offsets leave the window. |

---

## P1. ATVOL0: channel 0's words become channel 1's volume

**1. Proposition.** With ADKCON bit 0 (ATVOL0, Appendix A: USE0V1) set,
every word of channel 0's data set is written, in order, into channel 1's
volume, so that channel 1 plays its own waveform at the volume each word
carries in bits 5-0. No word of channel 0 reaches channel 1's period.
Claims: 5-hw.c3, 5-p136.c3 (p.136); 5-mod.c1, 5-p149.c5, 5-mod.c3,
5-p149.c7, 5-mod.c5, 5-p149.c10, 5-mod.c7, 5-p149.c13, 5-mod.c10,
5-p149.c16, 5-mod.c12 and 5-p149.c18 (bits 5-0 only) (p.149); 5-mod.c20,
5-p150.c23, 5-mod.c15, 5-p150.c2, 5-p150.c8, 5-p150.c12, 5-p150.c16,
5-p150.c20, 5-t54.w1.c4, 5-t54.w2.c4, 5-t54.w3.c4, 5-t54.w4.c4 (p.150);
5-p151.c8, 5-t55.b0 (p.151); 5-p165.c19, 5-sig.audxav (p.165);
5-appA-p265.c29, A-adkcon.b00 (p.265).

**2. Stimulus.** Session S-AV, with the program sequence above and ADKCON
$8001 before DMACON $8203. The modulator data M_V follows p.149: "Define the
modulator channel's data set as a series of words, each containing volume
information in the following format". Bits 15-7 are 0 (unused, see top) and
bit 6 is 0 (disputed, see top). AUD1VOL = 40 is not in M_V, so an interval
at 40 is recognisably unmodulated.

**3. Observation.** The left output, where channel 1 is alone (p.137).
Window: from the first change of channel 1's applied volume away from 40
until M_off + L_a.

**4. Expected.**
- (a) The applied volumes A_1 ... A_n in the window are cyclically
  consecutive in M_V, with n ≥ 17: every word is used, in order, twice or
  more, and no value comes from outside M_V. Sources: p.149, "The data words
  from the modulator channel are written into the corresponding registers of
  the modulated channel"; p.149, "each containing volume information";
  p.150, Table 5-4, column "Modulating Volume Only", which lists Words 1-4 as
  "|volume|".
- (b) Every level pair in the window is the reference pair L_1(v) of the
  current A_i. Channel 1's own waveform is played, only scaled: p.149, "its
  data is used to modulate the sound of the next higher-numbered channel";
  p.165, "AUDxAV Attach volume. Send data to volume latch of next channel".
- (c) Every channel 1 duration in the window equals D_1(200). No word goes
  to the period (Table 5-4, "Modulating Volume Only").

"each containing volume information" covers every word, so (a) is checked
for every interval. M_V holds each of bits 0-5 alone and all six together,
so ignoring any one of bits 5-0 makes some A_i differ from its word.

**5. Preconditions.** PC1 (mask $01), PC2, PC3 (channels 0 and 1;
channel 0 has at least 2 cycles in the window), PC4 (VSET_1, D_1(200)), PC5.

**6. Contrasting case.** Omit the ADKCON $8001 write. p.149: "Normally,
each channel produces sound when it is enabled." Nothing then modulates
channel 1, which plays at AUD1VOL = 40 throughout, and (a) fails. The
program still finishes; PC1 then expects mask $00.

**7. Reference frame.** The side comes from p.137 and is checked by PC5.
Volumes are identified by the R-VOL-1 level pairs (PC4). Word order is the
M_V order, which the channel 0 port offsets confirm (PC3).

---

## P2. ATPER0: channel 0's words become channel 1's period; all 16 bits count; the waveform is unchanged

**1. Proposition.** With ADKCON bit 4 (ATPER0, Appendix A: USE0P1) set,
every word of channel 0's data set is written, in order, into channel 1's
period, using all sixteen bits. Channel 1 keeps playing its own waveform,
sample for sample and at its own volume; only its pitch changes.
Claims: 5-mod.c4, 5-p149.c8, 5-p149.c9, 5-p149.c4, 5-mod.c13, 5-p149.c19,
5-mod.c10, 5-p149.c16 (p.149); 5-mod.c16, 5-p150.c3, 5-p150.c7,
5-p150.c11, 5-p150.c15, 5-p150.c19, 5-t54.w1.c3, 5-t54.w2.c3,
5-t54.w3.c3, 5-t54.w4.c3 (p.150); 5-p151.c4, 5-t55.b4 (p.151);
5-p165.c20, 5-sig.audxap (p.165); 5-appA-p265.c25, A-adkcon.b04 (p.265).

**2. Stimulus.** Session S-P2 (ADKCON $8010, DMACON $8203). Channel 0 plays
M_P with AUD0PER = 35000, so each word is in effect for about 70,000 ticks.
The first sample after a change still has the previous word's period. The
M_P order puts a short word before each long one, so every word still gets
at least two complete samples (33168 follows 144), and every word up to 1000
at least 20. Channel 1 plays RAMP at AUD1PER = 700, a value not in M_P, and
AUD1VOL = 48. The references come from R-RAMP.

**3. Observation.** The left output (channel 1 alone). Window: from the
start of the first run whose applied period differs from 700 until
M_off + L_a. The window must hold at least 18 runs, which is one full
cycle of M_P plus one.

**4. Expected.**
- (a) The applied periods of the runs are cyclically consecutive in M_P.
  Sources: p.149, "Define the modulator channel's data set as a series of
  words, each containing period information"; Table 5-4, column "Modulating
  Period Only", which lists Words 1-4 as "|period|".
- (b) The mean complete-sample duration of each run equals D_1(P) of that
  run's word, within 0.5 tick. p.149's format: "15 - 0 Period information,
  P15 - P0". M_P toggles each of the sixteen bits once against 400, so
  dropping any bit turns one run into a different value. A difference of
  one tick (bit 0) is resolved over at least 150 samples.
- (c) Throughout the window, channel 1's output values follow RAMP's
  sixteen bytes in order, wrapping around, with none skipped or repeated,
  and every value is one of R-RAMP's sixteen levels at volume 48. p.149:
  "Although the basic waveform itself remains the same, the pitch is
  increased or decreased by frequency modulation." Table 5-4, "Modulating
  Period Only": no word goes to the volume.

**5. Preconditions.** PC1 ($10), PC2, PC3 (channel 0 at least one complete
cycle in the window), PC4 (the R-RAMP durations are distinct and rise with
the value, and the sixteen levels are distinct), PC5.

**6. Contrasting case.** Omit the ADKCON $8010 write. Channel 1 then plays
at 700 throughout, and (a) and (b) fail. For (c): with mask $11 and M_VP
(session S-BOTH), the odd words become volumes (P4), so channel 1's levels
change and (c) fails.

**7. Reference frame.** Runs are delimited by the durations themselves,
anchored on p.164's reload at each sample transition. Periods are
identified by R-RAMP (PC4).

---

## P3. A modulator word is used whole, not as two bytes

**1. Proposition.** While a modulator word is in effect, the modulated
channel holds the one value of the whole 16-bit word. It does not take the
word's two bytes one after the other.
Claims: 5-mod.c8, 5-mod.c9, 5-p149.c14, 5-p149.c15 (p.149).

**2. Stimulus.** S-AV and S-P2, unchanged. In M_V the high byte of every
word is 0, so a byte-wise channel would alternate between volume 0 and the
low byte. Seven words of M_P have a non-zero high byte (16784, 33168, 912,
8592, 1424, 4496, 2448), whose bytes as periods would be (65, 144),
(129, 144), (3, 144), (33, 144), (5, 144), (17, 144) and (9, 144).

**3. Observation.** The left output, in the P1 and P2 windows. A word
interval is the span between two consecutive applied changes.

**4. Expected.** In S-AV, each word interval has exactly one level pair,
the whole word's L_1(v), and no interval is silent (volume 0 never occurs).
In S-P2, each run's duration is the whole word's D_1(P), never that of one
of its bytes. p.149: "When a channel is used as a modulator, the words in
its data set are no longer treated as two individual bytes. Instead, they
are used as "modulator" words." This holds for every interval in the
window.

**5. Preconditions.** Those of P1 and P2. In addition, R-MOD shows the
normal case: channel 0, unattached, plays each M_V word as two samples on
the right (p.138: "uses the sample it reads as two bytes of data"), for
example 0 and then 32 for $0020.

**6. Contrasting case.** Omit the attach write (session R-MOD's setting).
Channel 0 then plays its words as two bytes each, and channel 1 is not
modulated. The P3 check finds no word intervals and fails.

**7. Reference frame.** As in P1 and P2.

---

## P4. Both attach bits: the words alternate volume, period, starting with volume

**1. Proposition.** With ADKCON bits 0 and 4 both set, channel 0 modulates
both the volume and the period of channel 1 at the same time. Its data-set
words alternate: word 1 is a volume, word 2 a period, word 3 a volume, and
so on.
Claims: 5-mod.c2, 5-p149.c6 (p.149); 5-mod.c14, 5-p150.c1, 5-mod.c15,
5-mod.c16, 5-p150.c2, 5-p150.c3, 5-mod.c17, 5-p150.c4, 5-p150.c6,
5-p150.c10, 5-p150.c14, 5-p150.c18, 5-t54.w1.c2, 5-t54.w2.c2,
5-t54.w3.c2, 5-t54.w4.c2 (p.150); 5-appA-p265.c30, 5-appA-p265.c31
(V5-V0), 5-appA-p265.c32, A-adkcon.note1, A-adkcon.note2 (p.265).

**2. Stimulus.** Session S-BOTH: ADKCON $8011, then DMACON $8203. p.150:
"if channel 0 is used to modulate both the period and frequency [sic;
volume is meant, see *Ambiguities*] of channel 1, you set two attach bits --
bit 0 to modulate the volume and bit 4 to modulate the period." Channel 0
plays M_VP (LEN 8, even) at PER 4000. Channel 1 plays SQ at PER 200 and
VOL 63, neither of which occurs in M_VP.

**3. Observation.** The left output (channel 1 alone). All volume changes
and all period changes of channel 1 are merged in time order. Window: from
the first change after M_on until M_off + L_a, containing at least 16
changes.

**4. Expected.**
- (a) The merged sequence is cyclically consecutive in M_VP: V24, P300,
  V56, P700, V8, P500, V40, P900, V24, and so on. So the changes alternate
  in kind, each volume change carries an odd-position word and each period
  change an even-position word. Sources: p.150, "When period and volume are
  both modulated, words in the modulator channel's data set are defined
  alternately as volume and period information"; Table 5-4, column
  "Modulating Both Period and Volume": Word 1 "volume for other channel",
  Word 2 "period for other channel", Word 3 volume, Word 4 period; p.265:
  "First word xxxxxxxx V6-V0 , Second word P15-P0 (etc)".
- (b) After the first volume change, no applied volume lies outside
  {24, 56, 8, 40}. After the first period change, no applied period lies
  outside {300, 700, 500, 900}.
- (c) Both parameters change in the window, each at least 8 times. p.149:
  "or do both types of modulation on a channel at the same time".

Which data-set word the first change after M_on carries is not graded.
p.164 says the first word fetched after the DMA start "must be thrown
away", but not whether that is data-set word 1. The alternation itself is
anchored on data-set positions, through the distinct values.

**5. Preconditions.** PC1 ($11), PC2, PC3 (channel 0 at least 2 cycles in
the window), PC4 (VSET_1 including 63, 24, 56, 8, 40; R-PER-1 durations
for 200, 300, 500, 700, 900), PC5.

**6. Contrasting case.** Write $8001 instead of $8011, so only bit 0 is set.
p.150: "If you want to modulate both period and volume on the same channel,
you need to attach the channel as both a period and volume modulator."
Table 5-4, "Modulating Volume Only": every word is a volume. Channel 1's
period then stays 200, and (a) and (c) fail. The program still finishes, and
PC1 then expects $01. (The volumes that words 2, 4, 6, 8 would give are not
predicted, because they have bits 15-7 set; the contrast relies only on the
period staying 200.)

**7. Reference frame.** Data-set positions are identified by value and
confirmed by the channel 0 port offsets. Levels and durations are
identified by R-VOL-1 and R-PER-1.

---

## P5. A word reaches the modulated channel at a time-out of the modulator's period

**1. Proposition.** The modulated channel takes a modulator word's value
only at an instant when the modulator's period counter times out, never
before the first time-out after the word was fetched. With one attach bit,
successive words take effect two modulator periods apart. With both bits,
a word takes effect at every time-out.
Claims: 5-mod.c10, 5-p149.c16 (p.149); 5-p165.c18, 5-sig.pbufld2 (p.165).

**2. Stimulus.** Sessions S-AV (volume), S-AP (period, M_P4) and S-BOTH
(both), all with AUD0PER = 4000. Channel 1 plays at periods of at most 900,
far below 4000.

**3. Observation.** Channel 0's fetches at the port and channel 1's changes
on the left output. Window: from channel 0's third fetch after M_on until
M_off. By the third fetch the modulator is in its main loop. p.164: "State
101 is entered as soon as this word arrives ... When the data arrives,
state 010 is entered".

The time-outs cannot be seen directly, because the attached channel's own
output is silenced (P8). The test places them from the fetches. p.164:
"Transitions such as 010->011->010 occur whenever the period counter counts
down to one", and requests go out at transitions, so every fetch lies
within λ after a time-out, and the time-outs lie D_mod apart.

**4. Expected.** For every change of channel 1 in the window, let k be the
data-set word whose value it carries, and F_k the latest fetch of word k's
offset before the change. Then Δ = C − F_k satisfies
m·D_mod − λ ≤ Δ ≤ m·D_mod + ε for some integer m ≥ 1. Here ε is the delay
before a change can show: for a volume change, D_1 of channel 1's current
period plus 50 ticks; for a period change, D_1 of the old period plus 50
ticks (p.164: "The period counter is reloaded at these transitions").
- Single attach bit (S-AV, S-AP): consecutive changes are D_mod·2 apart,
  within λ + ε. p.164: "In attach volume, requests occur as they do in
  normal operation (on the 011->010 transition). In attach period, a set of
  requests occurs on the 010->011 transition". Each is one request per
  010->011->010 loop.
- Both bits (S-BOTH): consecutive changes are D_mod apart, within λ + ε,
  alternating volume and period. p.164: "When both attach period and attach
  volume are high, requests occur on both transitions". p.165: "pbufld2
  Like pbufld1, but only during 010->011 with attach period".

The sentence graded is p.149: "The data words from the modulator channel
are written into the corresponding registers of the modulated channel each
time the period register of the modulator channel times out." "Each time"
is graded on every change and every consecutive pair in the window. With
one attach bit, the page does not say which of the two time-outs after a
fetch carries the word (m = 1 or m = 2). That is not graded.

**5. Preconditions.** PC1 ($01, $10, $11), PC2, PC3, PC4, PC6 (D_mod).
At least 2L changes of channel 1 in the window. L_a comes from R-VOL-1.

**6. Contrasting case.** Run S-AV with AUD0PER = 6000 while keeping the
check's D_mod from R-MOD (4000 ticks). The page then predicts changes
12,000 ticks apart, on a 6000-tick grid, and both the grid check and the
spacing check fail. Grading S-BOTH with the single-bit spacing, or S-AV
with the dual-bit spacing, also fails.

**7. Reference frame.** The time-out grid is anchored on channel 0's
fetches (p.164 transfer timing). D_mod comes from R-MOD (PC6), and L_a
aligns audio time with port time.

---

## P6. Each attach bit pairs channel x with channel x+1, and with no other channel

**1. Proposition.** Each of ADKCON bits 0, 1, 2 (volume) and 4, 5, 6
(period) makes channel x = (bit mod 4) modulate the named parameter of
channel x+1, and of no other channel. The modulator's own period and every
bystander's volume and period stay as written.
Claims: 5-p151.c2, 5-p151.c3, 5-p151.c6, 5-p151.c7, 5-p151.c4,
5-p151.c8, 5-t55.b0, 5-t55.b1, 5-t55.b2, 5-t55.b4, 5-t55.b5, 5-t55.b6
(p.151); 5-appA-p265.c23, 5-appA-p265.c24, 5-appA-p265.c27,
5-appA-p265.c28, A-adkcon.b01, A-adkcon.b02, A-adkcon.b05, A-adkcon.b06
(p.265, with the bit 0 and bit 4 rows already cited in P1 and P2);
5-mod.c7, 5-p149.c13 (p.149); 5-mod.c19, 5-p150.c22, 5-mod.c22,
5-p150.c25 (p.150).

**2. Stimulus.** Six sessions S-PAIR-b, all four channels enabled
(DMACON $820F), ADKCON $8000 | (1 << b):

| b | Modulator | Target | Tripwires | Modulator data | Target side | Other side |
|---|---|---|---|---|---|---|
| 0 | ch0 | ch1 volume | ch2, ch3 | M6V | left | right |
| 1 | ch1 | ch2 volume | ch0, ch3 | M6V | left | right |
| 2 | ch2 | ch3 volume | ch0, ch1 | M6V | right | left |
| 4 | ch0 | ch1 period | ch2, ch3 | M_P4 | left | right |
| 5 | ch1 | ch2 period | ch0, ch3 | M_P4 | left | right |
| 6 | ch2 | ch3 period | ch0, ch1 | M_P4 | right | left |

- The modulator runs at LEN 4, PER 4000 and VOL 0. Volume 0 keeps it silent
  whatever P8 finds.
- The target plays SQ at LEN 4, PER 200 and VOL 40.
- The tripwires play W_V (volume sessions) or W_P (period sessions) at
  LEN 2 and VOL 0, with PER 1400 for ch0, 1600 for ch1, 1800 for ch2 and
  2000 for ch3. A tripwire is silent, and its fetches come every 2·PER. A
  volume word delivered to it would make it audible, and a period word would
  change its fetch spacing to at most 1800 ticks.

This is the table the proposition rests on, p.151 Table 5-5: "6 ATPER2 Use
audio channel 2 to modulate period of channel 3"; "5 ATPER1 ... period of
channel 2"; "4 ATPER0 ... period of channel 1"; "2 ATVOL2 ... volume of
channel 3"; "1 ATVOL1 ... volume of channel 2"; "0 ATVOL0 ... volume of
channel 1". p.265 gives the same six rows (USE2P3 ... USE0V1).

**3. Observation.** The target side's output; the other side's output; and
the port fetch record of every channel. Window: from the modulator's third
fetch after M_on until M_off. It must contain at least 9 changes of the
target.

**4. Expected.**
- (a) Volume sessions: the target's applied volumes are cyclically
  consecutive in M6V, and every level pair on the target side is a
  reference pair of the target channel (R-VOL-c) at an M6V value.
  Period sessions: the target's applied periods are cyclically consecutive
  in M_P4, and its level pair stays at the reference for VOL 40.
- (b) Volume sessions: the target's durations stay at D_c(200).
- (c) The other side's output is constant for the whole window.
- (d) Every non-target channel's consecutive fetch spacings equal 2·AUDxPER
  as written, within ±λ: 8000 ticks for the modulator, 2·PER for each
  tripwire. p.164 gives one request per two transitions, each after a
  period count-down.

Sources: p.149, "its data is used to modulate the sound of the next
higher-numbered channel"; p.150, "the channel will be attached to the next
higher channel, as shown in Table 5-5. Because an attached channel always
modulates the next higher numbered channel, you cannot attach channel 3."
"always" is graded by running all six bits. "the next higher" (a single
target) is graded by (c) and (d) on every other channel.

**5. Preconditions.** PC1 (mask per b), PC2, PC3 (all four channels, the
modulator at least 2 cycles in the window), PC4 (VSET_c and R-PER-c of the
target channel c), PC5.

**6. Contrasting cases.**
- For every b: omit the attach write. The target then keeps VOL 40 and
  PER 200, and (a) fails.
- To show that the tripwire checks (c) and (d) can fail, add a second
  attach bit that p.151 says turns a tripwire into a target of valid words:
  - b = 0, write $8005: bit 2 makes ch2 (W_V: volumes 48 and 16) modulate
    ch3's volume, ch3 becomes audible on the right, and (c) fails.
  - b = 2, write $8005: bit 0 makes ch0 modulate ch1, and the left side
    becomes audible.
  - b = 4, write $8050: bit 6 makes ch2 (W_P: 600, 1000) modulate ch3's
    period, ch3's spacing leaves 4000, and (d) fails.
  - b = 6, write $8050: bit 4 makes ch0 modulate ch1's period, and ch1's
    spacing leaves 3200.
  - b = 5, write $8030: bit 4 makes ch0 modulate the period of ch1, the
    modulator, and its spacing leaves 8000.
  - b = 1: no added bit makes a tripwire change without making it a
    modulator whose own output is silenced. (c) and (d) are the same check
    code as for b = 0, whose contrast covers them.

**7. Reference frame.** Sides come from p.137 (PC5). Channels are
identified at the port by their data regions. Spacings are in ticks from
D_mod (PC6).

---

## P7. Channel 3's attach bits attach it to nothing

**1. Proposition.** Setting ADKCON bit 3 (ATVOL3, Appendix A: USE3VN) or
bit 7 (ATPER3, USE3PN) makes channel 3's words modulate nothing. No
channel's volume or period changes, channel 3's own included.
Claims: 5-mod.c23, 5-p150.c26 (p.150); 5-appA-p265.c22, 5-appA-p265.c26,
A-adkcon.b03, A-adkcon.b07 (p.265).

**2. Stimulus.**
- S-CH3-V: ADKCON $8008, DMACON $820F. ch3 plays W_V (valid volume words
  48 and 16) at LEN 2, PER 2000, VOL 0. ch0, ch1 and ch2 are tripwires
  playing W_V at LEN 2, VOL 0, PER 1400, 1600 and 1800.
- S-CH3-P: ADKCON $8080. The same, with W_P (valid period words 600 and
  1000) for all four channels.

**3. Observation.** Both outputs, and the fetch record of all four channels.
Window: from ch3's third fetch after M_on until M_off.

**4. Expected.** Both outputs are constant for the whole window. Every
channel's consecutive fetch spacings equal 2·AUDxPER within ±λ: 2800, 3200,
3600 and 4000 ticks. Sources: p.151 Table 5-5, "7 ATPER3 Use audio channel
3 to modulate nothing (disables audio output of channel 3)" and the same
for "3 ATVOL3"; p.265, "07 USE3PN Use audio channel 3 to modulate nothing."
and "03 USE3VN ..."; p.150, "Because an attached channel always modulates
the next higher numbered channel, you cannot attach channel 3. Writing a 1
into channel 3's modulate bits only disables its audio output." The
quantifiers "nothing" and "only" are graded on all four channels, both
parameters, and the whole window.

**5. Preconditions.** PC1 ($08 / $80), PC2, PC3 (all four channels
including ch3: p.150's "only disables its audio output" leaves its DMA
running). By construction, ch3's words are valid values for the parameter
its bit names.

**6. Contrasting case.**
- S-CH3-V with $8004 (bit 2) instead of $8008. p.151, "ATVOL2 Use audio
  channel 2 to modulate volume of channel 3": ch3 takes ch2's volumes 48
  and 16 and plays its bytes 0, 48, 0, 16 audibly on the right, so the
  "constant" check fails.
- S-CH3-P with $8040 (bit 6) instead of $8080: ch3's period takes 600 and
  1000, and its spacing leaves 4000.
- PC1 then expects the new mask.

**7. Reference frame.** As in P6.

---

## P8. An attached channel's data does not reach its own converter

**1. Proposition.** While either attach bit of a channel is set, including
channel 3's, the words its DMA delivers are not sent to its own
digital-to-analog converter, so the channel's output does not follow its
data.
Claims: 5-p165.c19, 5-sig.audxav, 5-p165.c20, 5-sig.audxap (p.165);
A-adkcon.b03, A-adkcon.b07 (p.265, with p.151's parenthesis on bits 7 and
3).

**2. Stimulus.**
- Channel 0 at VOL 63, alone on the right, in S-AV (bit 0), S-AP (bit 4)
  and S-BOTH (bits 0 and 4).
- Channel 3 at VOL 63, alone on the right, in S-DAC3-V (bit 3, W_V) and
  S-DAC3-P (bit 7, W_P).

**3. Observation.** The right output. Window: from the attached channel's
third fetch after M_on until M_off, both shifted by L_a. By the third fetch
words are flowing (p.164).

**4. Expected.** The right output has one value for the whole window, with
no change. p.165: "AUDxAV Attach volume. Send data to volume latch of next
channel instead of to D->A converter." "AUDxAP Attach period. Send data to
period latch of next channel instead of to the D->A converter." p.151 on
bits 7 and 3: "(disables audio output of channel 3)". Which value the
output holds is stated on no page (p.150 says only that the output "will be
disabled"), so the value is not graded.

**5. Preconditions.** PC1, PC2, PC3 (the attached channel fetches, so data
is flowing). The same channel with the same data, period and volume,
unattached, changes the right output at least twice per data word: R-MOD
(M_V), R-DAC-P4 (M_P4), R-DAC-VP (M_VP), R-DAC3-V (W_V) and R-DAC3-P (W_P).

**6. Contrasting case.** Omit the attach write, which gives the matching
R- session. The page predicts the channel's bytes on the right (p.138), and
the "constant" check fails.

**7. Reference frame.** Sides come from p.137. The window is anchored on
the attached channel's own fetches.

---

## P9. The modulator's and the modulated channel's data-set lengths are independent

**1. Proposition.** The modulator repeats its data set after its own
AUDxLEN words and the modulated channel after its own, whatever the ratio
of the two. The modulation follows the modulator's cycle across the
modulated channel's restarts.
Claims: 5-mod.c18, 5-p150.c21 (p.150).

**2. Stimulus.** Two sessions, both with ADKCON $8001:
- S-LEN-A: modulator 5 words (M_LA), modulated channel 3 words (SQ).
- S-LEN-B: modulator 2 words (M_LB), modulated channel 7 words (SQ).

The lengths are coprime, and the modulator's is once longer and once
shorter than the modulated channel's.

**3. Observation.** The port fetch records of channels 0 and 1, and the
left output. Window: from the first change of channel 1's applied volume
until M_off + L_a. It must contain at least 2 modulator cycles.

**4. Expected.**
- (a) Channel 0's offsets step 0 ... L_m−1 and then back to 0, with
  L_m = 5 or 2.
- (b) Channel 1's offsets step 0 ... L_c−1 and then back to 0, with
  L_c = 3 or 7.
- (c) Channel 1's applied volumes are cyclically consecutive in M_LA or
  M_LB. None is skipped or repeated at channel 1's restarts.
- (d) Channel 1's durations stay at D_1(200).

p.150: "The lengths of the data sets of the modulator and the modulated
channels are completely independent." "completely" is graded by both
orderings of the lengths and by (c) holding across every restart of either
channel.

**5. Preconditions.** PC1 ($01), PC2, PC3, PC4 (VSET_1 including 8, 48, 20,
60, 32).

**6. Contrasting case.** S-LEN-A with AUD0LEN = 4. p.147 and p.145
("Set the length register, AUDxLEN, to the number of data words to be
retrieved before starting at the address currently in AUDxLC") predict a
4-word cycle, 8, 48, 20, 60, and (a) and (c) with L_m = 5 fail.

**7. Reference frame.** Word offsets at the port are measured from each
channel's own AUDxLC.

---

## P10. When the modulator requests its data: as normal with attach volume, one period later with attach period, twice as often with both

**1. Proposition.** With attach volume only, the modulator channel's DMA
requests, seen as its fetches, keep the phase they have in normal operation:
one per loop, on the 011->010 transition. With attach period only, they
come on the 010->011 transition, one modulator period later in the loop.
With both bits, they come on both transitions, one per time-out.
Claims: 5-p164.c33, 5-p164.c34, 5-p164.c35, 5-sm.c33, 5-sm.c34,
5-sm.c35 (p.164); 5-p165.c22, 5-sig.napnav (p.165, DMA part);
F58.loop.attach (p.166, Figure 5-8).

**2. Stimulus.** R-MOD (normal), S-AV ($01), S-AP ($10) and S-BOTH ($11),
all with AUD0PER = 4000.

**3. Observation.** Channel 0's fetch times at the port. The anchor is
t0 = F_2, channel 0's second fetch after M_on. p.164: "Because of
pipelining in Agnus, the first data word must be thrown away. State 101 is
entered as soon as this word arrives; a request for the next data word has
already gone out. When the data arrives, state 010 is entered and the main
loop continues". Time-outs then follow every D_mod. For every n ≥ 4 in the
window (until M_off), o_n = F_n − t0 − k(n)·D_mod.

**4. Expected.** o_n lies in [−100, +600] ticks for every n ≥ 4, with:
- R-MOD and S-AV: k(n) = 2(n−3). p.164: "In attach volume, requests occur
  as they do in normal operation (on the 011->010 transition)." p.165:
  "napnav /AUDxAV * /AUDxAP + AUDxAV -- no attach stuff or else attach
  volume. Condition for normal DMA and interrupt requests."
- S-AP: k(n) = 2(n−3)+1. p.164: "In attach period, a set of requests occurs
  on the 010->011 transition." Figure 5-8, arrow 010->011: "[pbufld2 [printed
  pubfld2] if AUDxAP, AUDxDR if (AUDxAP & AUDxON), percntrld]". The 011->010
  request "AUDxDR if (AUDxON & napnav)" is off, because napnav is false.
- S-BOTH: k(n) = n−3. p.164: "When both attach period and attach volume are
  high, requests occur on both transitions."

The window [−100, +600] covers the arrival of the anchor word and λ. Windows
for neighbouring k are 4000 ticks apart. Every fetch n ≥ 4 is graded. The
third fetch is not, because its timing comes from Figure 5-8's 101->010
bracket ("AUDxDR if napnav"), which no claim here records.

**5. Preconditions.** PC1, PC2, PC3, PC6, and PC7, which checks the anchor:
normal operation must show k(n) = 2(n−3).

**6. Contrasting case.** Change S-AV's attach write from $8001 to $8010 and
give channel 0 the period words M_P4, which makes it S-AP. The page predicts
the attach-period phase, so S-AV's check (k = 2(n−3)) fails by D_mod on
every fetch. Likewise $8011 with M_VP (S-BOTH) doubles the fetch rate and
fails the single-bit checks. The channel 0 data is changed along with the
bits so that channel 1 only ever receives valid values (period ≥ 124,
p.141).

**7. Reference frame.** The loop entry t0 = F_2 comes from p.164's start-up
description and is checked by PC7 on normal operation. D_mod comes from
R-MOD (PC6).

---

## Ambiguities and disagreements on the pages

1. **Width of a volume value.** p.139 ("six-bit volume register", "values
   from 64 to 0") and p.140 ("For any volume setting from 64 to 0, you write
   the value into bits 5-0") disagree with p.149 ("6 - 0 Volume information,
   V6 - V0"), p.265 ("V6-V0") and p.266 (bit 6 "Forces volume to max").
   p.140 cannot hold for 64. Only volumes 0-63 are used.
2. **Placeholder width on p.265.** "First word xxxxxxxx V6-V0" shows eight
   x before seven bits, fifteen positions in all, while p.149 marks bits
   15-7, nine bits, as unused. Both put the volume in the low bits, so
   nothing depends on it.
3. **p.150's example.** It reads "if channel 0 is used to modulate both the
   period and frequency of channel 1". The same sentence then sets "bit 0 to
   modulate the volume and bit 4 to modulate the period", so "frequency"
   stands for volume.
4. **Figure 5-8** prints "pubfld2" where p.165's list defines "pbufld2".
5. **One word per time-out, or one per two?** p.149 says words are written
   "each time the period register of the modulator channel times out".
   p.164 and Figure 5-8 give one data request per 010->011->010 loop, that
   is per two time-outs, with a single attach bit. The pages do not say at
   which of the two time-outs a new word lands, or whether the other
   time-out rewrites the same word. P5 grades only what both allow.
6. **The first word after DMA start.** p.164: "the first data word must be
   thrown away". Whether data-set word 1 is lost at start-up is not stated,
   so no check depends on which word comes first after M_on.
7. **Both attach bits with an odd AUDxLEN.** The alternation is defined on
   data-set positions (p.150, p.265), and the data set wraps (p.147). With an
   odd length, word L and word 1 would both be volumes. No page says what
   happens then, so every dual-mode data set here has an even length.
8. **Names.** Chapter 5 calls the bits ATVOL0-3 and ATPER0-3 (p.151).
   Appendix A calls them USE0V1, USE1V2, USE2V3, USE3VN, USE0P1, USE1P2,
   USE2P3 and USE3PN (p.265). The bit numbers agree.
9. **Attach period and the normal request.** p.164 says a set of requests
   occurs on 010->011 with attach period, but not that the 011->010 request
   stops. Only Figure 5-8's napnav gate (p.165) says that it does. The two
   do not conflict.
10. **Level of a disabled output.** p.150 ("audio output will be disabled"),
    p.149 ("ceases to produce sound") and p.165 ("instead of to D->A
    converter") do not say what level the output then holds. P8 checks only
    that it does not change.
11. **Changing attach bits while DMA runs** is not described on any page.
    The program changes them only while all channels are off.
