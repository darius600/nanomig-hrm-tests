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
| 5-p165.c22, 5-sig.napnav | The DMA-request half is recorded, not asserted: no line isolates napnav's own contribution, and it is graded only through the phases that P10-AV, P10-AP and P10-BOTH grade. The claim also says napnav conditions the normal *interrupt* requests. With attach period alone, Figure 5-8 routes the interrupt through a different bracket on 010->011 that no claim of this package records. So the claim gives no interrupt time for that mode. In attach-volume mode it predicts only "as normal", which is the same loop phase that P10-AV measures on the DMA requests. |
| 5-mod.c19, 5-p150.c22 | "Channels are attached by the system in a predetermined order, as shown in Table 5-5." The six fixed pairs are checked (P6). The claim also implies that no other pairing can be selected. That describes a control that does not exist, and nothing can be observed about it beyond the six pairs. |
| 5-p165.c19, 5-sig.audxav, 5-p165.c20, 5-sig.audxap | p.165: "AUDxAV Attach volume. Send data to volume latch of next channel instead of to D->A converter." and the same for AUDxAP, for every channel x. The sentence has two halves (the next channel's latch receives the data; the channel's own converter does not). One session holds both halves for channel 0 (P1-JOINT for AUDxAV, P8-JOINT-AP for AUDxAP). For channel 1 a session holding both is not observable at this boundary: channels 1 and 2 share one output (see *Output sides and silence*), so channel 1's own output and channel 2's modulated output cannot be told apart. For channel 2 the destination half is P6-A (bits 2 and 6), whose modulator is silent at volume 0, and no session holds its converter half. The claims quantify over every x, so the four ids are asserted on no line. |
| 5-mod.c18, 5-p150.c21 | p.150: "The lengths of the data sets of the modulator and the modulated channels are completely independent." P9 observes two length pairs (5 and 3, 2 and 7). Two ratios do not establish "completely", so neither id is asserted. |

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
session runs. The data sets lie at $088000 (channel 0), $089000, $08A000 and
$08B000, one 4 KB region per channel, and the marker and result words at
$0C0000 (ch5_regs.i), all in Chip memory. Data sets start on a word boundary (p.138:
"the first byte of data must start at a word-address boundary"), and each
channel's data set lies in its own region, so every audio fetch at the
Chip-memory port can be attributed to one channel and one word offset.

Register addresses (Appendix B, p.289-291; Appendix A, p.265-266, p.278,
p.280): ADKCON $DFF09E (write), ADKCONR $DFF010 (read), DMACON $DFF096,
DMACONR $DFF002, INTENA $DFF09A, INTREQ $DFF09C, VHPOSR $DFF006. For audio
channel x (x = 0-3) the base is $DFF0A0 + $10·x: AUDxLCH +0, AUDxLCL +2,
AUDxLEN +4, AUDxPER +6, AUDxVOL +8.

### Output sides and silence

p.137 and p.326 disagree on which output carries which channel pair (see
*Ambiguities*, item 12, for both quotes); they agree only on the pairing:
channels 0 and 3 share one output, channels 1 and 2 share the other, and
the two outputs differ. No proposition below may depend on which physical
jack that is. Each channel's own output is the one its own reference
session shows changing: every reference session plays one channel with
all others off (or at volume 0), so exactly one output changes while the
other stays constant, and that is the channel's output for every later
observation of it, in any session. PC5 grades only the pairing itself. A
channel is observed while every other channel sharing its own measured
output is either off (AUDxEN clear, AUDxVOL = 0) or at volume 0. Volume 0
is silence: p.163, Table 5-9, volume 0 = "Minus infinity"; p.266,
"000000=no output".

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

**One line, ℓ.** p.164: "DMA requests and restart requests are transferred to
Agnus once each horizontal line, and the data comes back about 14 clock
cycles later (the duration of a clock cycle is 280 ns)". A request is
therefore served at the next line, and a counter-driven interval between two
fetches of one channel is realised to within less than one line: each fetch
follows its request by less than one line plus a constant delay, and the
constant cancels in the difference. The page gives no number for a line. ℓ is
measured in the same run as everything else: the request made when F_2 arrives
(Figure 5-8, 101->010) is the next request, so F_3 − F_2 of the ordinary R-MOD
is one line (the PC7-CAL frame). Every comparison of two fetch times of one
channel, or of the same two fetches in two sessions, is graded `< ℓ`. No
other time bound enters this package: where the page names two alternatives a
period apart (the phase of the requests, odd or even time-outs) the line is
graded by the alternative it lies nearer to, and the delay from a fetch to
its change is measured and not graded (the page gives the request-to-data
delay only as "about 14 clock cycles").

**Apparatus resolution.** The audio output and the port log resolve time to
one clk_sys cycle (8 per tick). A duration measured in one session is compared
with a reference measured in another (PC6, P1-C, P2-B's run means, P4, P6-B,
P9-D) to within 1 clk, and a measured period is matched to a reference
duration to within 1 clk. It is the resolution of the apparatus, not a
tolerance of any page, and every difference this package grades is at least
one tick (8 clk).

**ε.** A change of the period shows at the first sample that has the new
period (p.164: "The period counter is reloaded at these transitions"), so its
instant is known to the length of one sample. ε is the largest reference duration
any session here modulates channel 1 through, D_1(900), measured in R-PER-1; it is
used for the instants of period changes only (P5). The instant of a volume change
is known to the apparatus resolution.

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
| R-DAC-P4 | $00 | M_P4, 4, 4000, 63 | off | off | off | about 60,000 (minimum: one data-set cycle + 8,000) | P8, PC3 preconditions |
| R-DAC-VP | $00 | M_VP, 8, 4000, 63 | off | off | off | about 125,000 (minimum: one data-set cycle + 8,000) | P8, PC3 preconditions |
| R-DAC3-V / R-DAC3-P | $00 | off | off | off | W_V / W_P, 2, 2000, 63 | 30,000 | P8 preconditions |
| R-VOL-c (c = 1, 2, 3) | $00 | off unless c | | | | 4,000 per value | level references |
| R-PER-c (c = 1, 2, 3) | $00 | off unless c | | | | 20,000 per value | duration references |
| R-RAMP | $00 | off | RAMP, 8, 700, 48 | off | off | see below | P2 references |
| S-AV | $01 | M_V, 8, 4000, 63 | SQ, 4, 200, 40 | off | off | 200,000 | P1, P3, P5, P8, P10 |
| S-AP | $10 | M_P4, 4, 4000, 63 | SQ, 4, 200, 40 | off | off | 110,000 | P5, P8, P10 |
| S-BOTH | $11 | M_VP, 8, 4000, 63 | SQ, 4, 200, 63 | off | off | 110,000 | P4, P5, P8, P10 |
| S-P2 | $10 | M_P, 17, 35000, 63 | RAMP, 8, 700, 48 | off | off | 2,700,000 | P2, P3 |
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

- **Sample boundary**: for an SQ channel, an instant at which the observed
  side changes sign relative to the current reference level pair --
  crosses from the pair's lower (-100) member to its upper (+100) member,
  or back. A change of level pair alone (a modulation word landing inside
  a sample) is not a boundary: the sample it falls inside runs to the next
  sign alternation, and its representative value is the settled one after
  the word lands. For RAMP (no volume modulation is ever concurrent with
  it), a boundary is every change of value. **Duration**: the time between
  consecutive boundaries.
- **Level pair** of an interval: the two settled output values of an SQ
  channel, one for the +100 sample and one for the -100 sample. **Reference
  level pair** L_c(v): the pair measured in R-VOL-c (or R-PER-c for v = 40)
  in the middle of the marker interval for v, excluding the first two samples
  after each marker.
- **Reference duration** D_c(P): the median complete-sample duration of
  channel c in R-PER-c (or R-RAMP) for the value P.
- **Applied volume** of an interval: the v whose reference level pair equals
  the interval's. **Applied period** of a run of equal durations (within the
  apparatus resolution): the P whose reference duration equals the run's mean
  (within the apparatus resolution). The first
  sample after a change of the written period still has the old duration.
  p.164: "The period counter is reloaded at these transitions". So a run
  begins at the first sample with the new duration.
- **Change instant C**: for a volume change, the time of the first output
  sample that shows the new level pair; for a period change, the start of
  the first sample of the new run. Neither is corrected for latency; P5 compares
  the instants of changes with one another, and orders them against the port times
  of fetches. The audio latency **L_a**, the minimum over all
  R-VOL-1 writes of (first output sample at the new level − marker time),
  shifts only the windows that start at a fetch (P1-JOINT's source half,
  P8-ATT).
- **Fetch record** of channel x: the port reads inside x's own 4 KB data
  region between M_on and M_off, each with its time and word offset. **F_n** is
  the page's n-th fetch of the channel after the DMA start (p.164): F_1 is the
  word "thrown away", F_2 the word whose arrival enters state 010, F_3 the word
  requested on the 101->010 transition. The page gives no address for F_1, so the
  record may or may not hold it. PC7-CAL finds in the run which record entry r is
  F_2 by time alone, from channel 0's own output in R-MOD (the entry nearest to the
  instant state 010 was entered), and assumes no address for it. Every grade of a
  fetch of channel 0 by its page index n is of record entry r + n - 2, in every
  session (the start-up is the same in every session). A window "from the
  third fetch" without a page index is from the third entry of the record of the
  channel's own data set; a window "from the modulator's first restart" is from the
  cycle of the first fetch at offset 0 after the first entry of the record of its
  own 4 KB region (the anchor PC3 uses).
- **End of play** of a channel's output: the DMACON clear that stops the channel
  precedes the M_off marker by one instruction and shows on the output as the
  final run (the output idle to the end of the capture). The window of every
  grade of samples, runs or levels of that output ends one clk after the start of
  the run before the final run -- the sample cut short by the stop, or the last
  complete one where that sample and the idle level are the same value -- so the
  cut sample is in progress at the window's end and no complete-sample or
  complete-run population holds it. Nothing depends on a measured latency. An
  output that never goes idle has no cut sample and its window ends at M_off.
  Fetch records and the constant-output checks end at M_off.
- **Cyclically consecutive in S**: a sequence of values A_1 ... A_n where
  every A_i is in the data set S and A_(i+1) is the word that follows A_i in
  S, wrapping from the last word to the first.
- **Population**: the count of intervals, runs or changes a clause grades
  (PC3's complete cycles; a clause's applied-value changes, complete
  samples, word intervals or runs). Two kinds are told apart. An
  **independent population** is one that exists whatever the attach bits
  do: the session's window, the fetches of the modulator, the complete samples
  of the observed channel, PC3's complete cycles. Where an independent population
  is short of what a line needs, the line's precondition is unmet and the line
  prints NOT GRADED (it names the precondition), never PASS or FAIL. An
  **applied-event population** is the changes, word intervals and runs that the
  modulator's words produce on the modulated channel. It is the very thing the
  clause says happens, so it is never a precondition: once the independent
  populations hold, an empty applied-event population is the failure of the
  clause (or of the joint line that contains it), and the line prints FAIL.
  A "change" is counted only
  where a value differs from the one before it. At a window's start P3 and P4
  count from the channel's own written starting value; in P1, P2, P5, P6, P9
  and the destination half of P8-JOINT-AP the first segment or run of the
  window shows the value in force at its edge, applied before the window
  opened, and is not counted. An unchanging window is
  an empty population of changes, not one change.

### Common preconditions

Every session asserts these. Each one comes with a stimulus change that
makes it fail, so that the check itself can be shown to go red. A proposition's
lines are gated on the PC1, PC2 and PC3 lines of the session they grade (PC3 only
where it is printed for that session): where one of them fails, or, for PC3, is
unmet because the population it needs is short, the gated lines print NOT GRADED
and name the precondition, never PASS or FAIL. A contrast of a precondition changes the
program of the session it grades; the expected value of the precondition does
not change with it.

| Id | Precondition | Contrast that makes it fail |
|---|---|---|
| PC1 | Bits 7-0 of the stored ADKCONR word equal the session's attach mask (p.265: "ADKCONR 010 R P Audio, disk, control read"). | Omit the step-4 ADKCON write in an attach session (bits read 0); or write $8001 in a reference session. |
| PC2 | The stored DMACONR word has bit 9 (DMAEN) and the session's AUDxEN bits set (p.278). | R-MOD with the enable $8001 (AUD0EN, no DMAEN; prog_rmod_nodma). Bit 9 then reads 0, and no audio fetch occurs (p.144: "For any audio DMA to occur ... the DMAEN bit in DMACON must be set"). |
| PC3 | Every enabled channel has fetches in its own region (its own 4 KB, Appendix B), at offsets stepping 0, 1, ..., LEN-1, 0, ... (p.147: "Each time the length counter reaches zero, both the location and length registers are reloaded"), graded from that channel's first restart (the first fetch at offset 0 after the first entry of its record) on, over [M_on, M_off] of the session -- the first pass is graded in hrm/ch5-audio-dma-first-pass, not here. The modulator has at least two complete cycles after its own first restart (one in S-P2). Where the session holds fewer than the required cycles after the channel's first restart, PC3 is unmet for that channel: it prints RECORDED "precondition not met", never PASS or FAIL, and every line gated on PC3 prints NOT GRADED; a channel whose record holds no restart at all fails PC3, as a broken step does. In S-LEN-A and S-LEN-B the same stepping is P9-A (channel 0) and P9-B (channel 1), graded from the later of the two restarts: PC3 is printed for those two sessions as their ordinary programs build them, and a contrast of P9-A or P9-B replaces the session for P9-A to P9-D only. | R-MOD with AUD0LEN = 7 (prog_rmod_len7), graded with LEN 8: the offsets step 0..6,0,... and the ordinary R-MOD line fails. |
| PC4-VOL | In R-VOL-c the reference level pairs of all VSET_c values are pairwise distinct. | R-VOL-1 with 40 written in the step labelled 60 (prog_rvol1_mut): two labels then have the same pair. R-VOL-2 and R-VOL-3 are graded by the same call and have no contrast of their own. |
| PC4-DUR | In R-PER-c and R-RAMP the reference durations are pairwise distinct and rise strictly with the written value. | R-PER-1 with 500 written in the step labelled 700 (prog_rper1_mut): two labels then have the same duration. R-PER-2, R-PER-3 and R-RAMP are graded by the same call and have no contrast of their own. |
| PC5-VOL, PC5-PER | Pairing anchor: channels 0 and 3 measure on one output, 1 and 2 on the other, and the two outputs differ, each channel's own output taken from its own reference session (see *Output sides and silence*). PC5-VOL is graded from the volume-sweep family and PC5-PER from the period-sweep family. No jack name enters the check. | PC5-VOL: R-VOL-1's capture is graded as R-VOL-3's, so "channel 3's" output is channel 1's. PC5-PER: R-VOL-3's capture is graded as R-PER-1's, so "channel 1's" output is channel 3's. Each pairing then fails against the same comparator. |
| PC6 | Unit anchor (p.140: "the period value represents clock ticks per sample"): in R-MOD, every complete sample duration of channel 0 equals D_mod within the apparatus resolution (1 clk). | CPU writes AUD0PER = 5000 halfway through R-MOD: the spread fails. |
| PC7-CAL | Frame calibration (P10): in R-MOD the record entry r of channel 0 that is the page's F_2 (p.164: "When the data arrives, state 010 is entered"). The lower byte of the first word is output when state 011 is entered, one period D_mod after 010 (p.164: "In the 010 state the upper byte is output, and in the 011 state the lower byte is output"; "Transitions such as 010->011->010 occur whenever the period counter counts down to one"): its instant is the start of the first run of channel 0's output whose value is neither the first run's nor the most frequent one's (the level of byte 0), and the instant 010 was entered is D_mod before it. The entry r is the record entry nearest to that instant, whatever its word offset and its index in the record; the precondition holds when the instant lies nearer to that entry than that entry lies to any other fetch of the record, and a fetch follows it. | R-MOD with AUD0PER = 6000 from the start (prog_rmod_per6000), graded with R-MOD's D_mod of 4000 ticks: the instant 010 is computed 2000 ticks late, it lies farther from the nearest fetch than that fetch lies from its neighbour, and the calibration fails. PC7-LOOP, PC7-N3 and P10-AV, P10-AP and P10-BOTH take the contrast's calibration and print NOT GRADED. |
| PC7-FRAME | No read of channel 0's data region occurs before M_on. | M_on is written late (prog_rmod_pc7frame): a fetch occurs before it. |
| PC7-LOOP | Loop anchor (P10): in R-MOD, every consecutive pair of fetches from the page's F_3 on is D_mod·2 apart, to within less than one line ℓ. Requires PC7-CAL; prints NOT GRADED without it. | CPU writes AUD0PER = 3000 halfway through R-MOD (prog_rmod_pc7): the gap is 6000 ticks, farther than ℓ from 8000. |
| PC7-N3 | 0 < F_3 − F_2 < D_mod (Figure 5-8's 101->010 bracket, "AUDxDR if napnav", with napnav = 1: the request made when F_2 arrives is fetched before the first time-out of the period counter, which comes one period after 010 was entered; no claim id in either claims file names that transition). Requires PC7-CAL; prints NOT GRADED without it. | S-AP's capture graded through the same comparator: with napnav = 0 the figure prints no request on 101->010, so on a page-conforming implementation the next fetch is made on the 010->011 transition, F_3 − F_2 is at least one period, and the line fails. |

---

## P1. ATVOL0: channel 0's words become channel 1's volume

**1. Proposition.** With ADKCON bit 0 (ATVOL0, Appendix A: USE0V1) set,
every word of channel 0's data set is written, in order, into channel 1's
volume, so that channel 1 plays its own waveform at the volume each word
carries in bits 5-0. No word of channel 0 reaches channel 1's period.
Claims asserted on P1-A: 5-hw.c3, 5-p136.c3 (p.136); 5-p149.c5, 5-mod.c3,
5-p149.c7 (p.149); 5-mod.c15, 5-p150.c2, 5-p150.c8,
5-p150.c12, 5-p150.c16, 5-p150.c20, 5-t54.w1.c4, 5-t54.w2.c4, 5-t54.w3.c4,
5-t54.w4.c4 (p.150); 5-p151.c8, 5-t55.b0 (p.151); 5-appA-p265.c29,
A-adkcon.b00 (p.265). 5-mod.c20 and 5-p150.c23 ("To attach a channel as a
modulator, you set its attach bit to 1") are asserted on P6-JOINT, which holds
every attach bit. P1-JOINT carries no id (see *Claims used only in part*, 5-p165.c19
and 5-sig.audxav). Used
only in part, so asserted on no line: 5-mod.c12 and 5-p149.c18 (bits 5-0 only,
V6 withdrawn); 5-mod.c1 (both kinds of modulation, with P2 and P4);
5-mod.c5 and 5-p149.c10 (how the attach bits make every channel's data
interpreted); 5-mod.c7 and 5-p149.c13 (the output ceasing and the data
modulating the next channel, for every attach bit); 5-mod.c10 and 5-p149.c16
belong to P5.

**2. Stimulus.** Session S-AV, with the program sequence above and ADKCON
$8001 before DMACON $8203. The modulator data M_V follows p.149: "Define the
modulator channel's data set as a series of words, each containing volume
information in the following format". Bits 15-7 are 0 (unused, see top) and
bit 6 is 0 (disputed, see top). AUD1VOL = 40 is not in M_V, so an interval
at 40 is recognisably unmodulated.

**3. Observation.** Channel 1's own output (see *Output sides and silence*).
Window: from the modulator's first restart (channel 0's first fetch at offset 0
after the first entry of its record: the first pass is graded in
hrm/ch5-audio-dma-first-pass, not here) until the end of play of channel 1's
output (see *Measurements*). The window does not depend on what channel 1
plays.

**4. Expected.**
- (a) (P1-A) The applied volumes A_1 ... A_n in the window are cyclically
  consecutive in M_V, with n ≥ 17: every word is used, in order, twice or
  more, and no value comes from outside M_V. Sources: p.149, "The data words
  from the modulator channel are written into the corresponding registers of
  the modulated channel"; p.149, "each containing volume information";
  p.150, Table 5-4, column "Modulating Volume Only", which lists Words 1-4 as
  "|volume|".
- (b) (P1-B) The window holds applied volumes, and every level pair in it is the
  reference pair L_1(v) of the current A_i. Channel 1's own waveform is played, only
  scaled: p.149, "its data is used to modulate the sound of the next higher-numbered
  channel"; p.165, "AUDxAV Attach volume. Send data to volume latch of next channel".
- (c) (P1-C) With applied volumes in the window, every channel 1 duration in the
  window equals D_1(200), within the apparatus resolution. No word goes to the
  period (Table 5-4, "Modulating Volume Only").
- P1-JOINT: p.165, "AUDxAV Attach volume. Send data to volume latch of next
  channel instead of to D->A converter." has two halves, and one line holds
  both for S-AV: (a) holds (channel 1 follows channel 0's data) AND channel 0's
  own output holds one value from its third fetch on (P8's source half; the
  window starts at that fetch shifted by L_a). It holds both halves for channel
  0 only and carries no id (see *Claims used only in part*).

"each containing volume information" covers every word, so (a) is checked
for every interval. M_V holds each of bits 0-5 alone and all six together,
so ignoring any one of bits 5-0 makes some A_i differ from its word.

**5. Preconditions.** PC1 (mask $01), PC2, PC3 (channels 0 and 1;
channel 0 has at least 2 cycles after its first restart), PC4-VOL and PC4-DUR
(VSET_1, D_1(200)), PC5. P1-WINDOW: the window exists, that is, channel 0 has a
first restart before the end of play of channel 1's output. Where it does not,
it prints as a precondition not met (RECORDED, not FAIL) and P1-A, P1-B, P1-C
and P1-JOINT print NOT GRADED, as they do when PC1, PC2 or PC3 of S-AV fails;
it is not a red. Inside a window that exists, P1-A and P1-JOINT need n ≥ 17
applied changes, and a channel 1 that never leaves L_1(40) has none and fails
them. (b) and (c) need applied volumes in the window, and (c) complete samples:
a window with no applied volume is one in which no word has reached the
volume, and both lines fail with it (an applied-event population, per
*Measurements*).

**6. Contrasting cases.** Each changes the program of S-AV and nothing else.
- Omit the ADKCON $8001 write (prog_s_av_noattach, selector PC1). PC1 keeps
  expecting mask $01 and fails; P1-A, P1-B, P1-C and P1-JOINT print NOT GRADED.
- The second and third modulator words exchanged (prog_s_av_swap, selector P1-A):
  every value is still a word of M_V, the order is not, and P1-A fails (P1-JOINT
  contains it and fails with it).
- Channel 3 also plays, audibly, on the output channel 0 is observed on
  (prog_s_av_ch3, selector P1-JOINT): channel 0's side no longer holds one value
  and P1-JOINT fails (the source half; P1-A holds).
- Channel 1's DMA stopped after one eighth of the session (prog_s_av_stop,
  selector P1-WINDOW): channel 1's output ends before channel 0 restarts, P1-WINDOW
  is not met and P1-A, P1-B, P1-C and P1-JOINT print NOT GRADED.
- A last modulator word of 33 (prog_s_av_mv33, selector P1-MV33) is a volume with
  no reference level pair: P1-B fails (P1-A and P1-JOINT contain the level
  check and fail with it).
- Channel 1's own AUDxPER rewritten from 200 to 300 halfway through
  (prog_s_av_per1write, selector P1-PER1WRITE): P1-C fails.

The verdicts of every contrast, per core, on the repository RTL and on the
page model, are in *Measured values*.

**7. Reference frame.** Channel 1's own output is measured directly, from
R-VOL-1 (see *Output sides and silence*); PC5 checks only the pairing, not
this channel's side. Volumes are identified by the R-VOL-1 level pairs
(PC4-VOL). Word order is the M_V order, which the channel 0 port offsets
confirm (PC3).

---

## P2. ATPER0: channel 0's words become channel 1's period; all 16 bits count; the waveform is unchanged

**1. Proposition.** With ADKCON bit 4 (ATPER0, Appendix A: USE0P1) set,
every word of channel 0's data set is written, in order, into channel 1's
period, using all sixteen bits. Channel 1 keeps playing its own waveform,
sample for sample and at its own volume; only its pitch changes.
Claims: 5-mod.c4, 5-p149.c8, 5-p149.c9, 5-p149.c4, 5-mod.c13, 5-p149.c19
(p.149); 5-mod.c16, 5-p150.c3, 5-p150.c7, 5-p150.c11, 5-p150.c15,
5-p150.c19, 5-t54.w1.c3, 5-t54.w2.c3, 5-t54.w3.c3, 5-t54.w4.c3 (p.150);
5-p151.c4, 5-t55.b4 (p.151); 5-appA-p265.c25, A-adkcon.b04 (p.265).
5-p149.c9 ("the pitch is increased or decreased by frequency modulation" while
"the basic waveform itself remains the same") says both that the pitch changes and
that the waveform stays: it is asserted on P2-JOINT, which holds (a) and (c) in
S-P2. 5-mod.c10 and 5-p149.c16 belong to P5. 5-p165.c20 and 5-sig.audxap are
withdrawn (see *Claims used only in part*).

**2. Stimulus.** Session S-P2 (ADKCON $8010, DMACON $8203). Channel 0 plays
M_P with AUD0PER = 35000, so each word is in effect for about 70,000 ticks.
The first sample after a change still has the previous word's period. The
M_P order puts a short word before each long one, so every word still gets
at least two complete samples (33168 follows 144), and every word up to 1000
at least 20. Channel 1 plays RAMP at AUD1PER = 700, a value not in M_P, and
AUD1VOL = 48. The references come from R-RAMP.

**3. Observation.** Channel 1's own output. Window: from the modulator's
first restart (the first pass is graded in hrm/ch5-audio-dma-first-pass, not
here) until the end of play of channel 1's output (see *Measurements*). The
window must hold at least 18 runs, which is one full cycle of M_P plus one; the
17 words of M_P last 17 × 70,000 ticks, so S-P2 lasts 2,700,000 ticks.

**4. Expected.**
- (a) (P2-A) The applied periods of the runs are cyclically consecutive in
  the 17 words of M_P (400 ... 2448 in the order the data set holds them: not
  the 700 of R-RAMP's settle interval), from at least 18 runs. Sources: p.149, "Define the modulator channel's data set as a series of
  words, each containing period information"; Table 5-4, column "Modulating
  Period Only", which lists Words 1-4 as "|period|".
- (b) (P2-B) The window holds applied-period runs, and the mean complete-sample
  duration of each run equals D_1(P) of the word the data set holds at that position
  (the first run is aligned to its own word, the data set is cyclic), within the
  apparatus resolution. p.149's format: "15 - 0 Period information,
  P15 - P0". M_P toggles each of the sixteen bits once against 400, so
  dropping any bit turns one run into a different value. A difference of
  one tick (bit 0) is resolved over at least 150 samples.
- (c) (P2-C) Throughout the window, channel 1's output values follow RAMP's
  sixteen bytes in order, wrapping around, with none skipped or repeated,
  and every value is one of R-RAMP's sixteen levels at volume 48. The order is
  the data table's (8, 24 ... 120, -8 ... -120, from $0818 ... $9888); the
  sixteen levels are R-RAMP's own distinct output values at volume 48, and a
  byte's level is the one of the same rank, because volume scales every byte by
  one positive factor (p.139). p.149:
  "Although the basic waveform itself remains the same, the pitch is
  increased or decreased by frequency modulation." Table 5-4, "Modulating
  Period Only": no word goes to the volume.
- P2-JOINT: (a) holds (the pitch of channel 1 follows channel 0's period words) AND
  (c) holds (channel 1's output follows RAMP's sixteen bytes in order): both halves
  of 5-p149.c9, one session.

**5. Preconditions.** PC1 ($10), PC2, PC3 (channel 0 at least one complete
cycle in the window), PC4-DUR (the R-RAMP durations are distinct and rise with
the value, and the sixteen levels are distinct), PC5. (b) needs applied-period
runs in the window: with none, no word has reached the period, and (b) fails (an
applied-event population, per *Measurements*). (c)'s population is the window's
complete runs, which exist whatever channel 1 receives; (c) grades the waveform on
its own, and P2-JOINT fails when (a) does.

**6. Contrasting cases.** Each changes the program of S-P2 and nothing else.
- The eighth and ninth words of M_P (464 and 272) exchanged (prog_s_p2_swap,
  selectors P2-A and P2-B): every value is still a word of M_P, the order is not;
  (a) fails with a full population, and so does (b), since the run at each
  position no longer has the duration of the word the position holds; P2-JOINT
  fails with (a).
- The second and third words of channel 1's own data exchanged (prog_s_p2_ramp,
  selector P2-C): the periods are those of M_P, the waveform order is not RAMP's, and
  (c) fails; P2-JOINT fails with it.

**7. Reference frame.** Runs are delimited by the durations themselves,
anchored on p.164's reload at each sample transition. Periods are
identified by R-RAMP (PC4-DUR).

---

## P3. A modulator word is used whole, not as two bytes

**1. Proposition.** While a modulator word is in effect, the modulated
channel holds the one value of the whole 16-bit word. It does not take the
word's two bytes one after the other.
Claims: 5-mod.c8, 5-mod.c9, 5-p149.c14, 5-p149.c15 (p.149), asserted on
P3-JOINT, which holds the volume mode (P3-WHOLE, S-AV) and the period mode
(P3-SP2, S-P2): the sentence is about a modulator's words, whatever they carry.
P3-WHOLE, P3-SILENT and P3-SP2 grade one consequence each and carry no id.

**2. Stimulus.** S-AV and S-P2, unchanged. In M_V the high byte of every
word is 0, so a byte-wise channel would alternate between volume 0 and the
low byte. Seven words of M_P have a non-zero high byte (16784, 33168, 912,
8592, 1424, 4496, 2448), whose bytes as periods would be (65, 144),
(129, 144), (3, 144), (33, 144), (5, 144), (17, 144) and (9, 144).

**3. Observation.** Channel 1's own output, in the P1 and P2 windows (from the
modulator's first restart). A word interval is the span between two consecutive
applied changes.

**4. Expected.** In S-AV, the P1 window holds word intervals and each has exactly
one level pair, the whole word's L_1(v) (P3-WHOLE), and none is silent -- volume 0
never occurs (P3-SILENT). In S-P2, the window holds applied-period runs and each
run's duration is the whole word's D_1(P), never that of one of its bytes (P3-SP2).
P3-JOINT holds P3-WHOLE and P3-SP2. p.149: "When a channel is used as a modulator,
the words in its data set are no longer treated as two individual bytes. Instead,
they are used as "modulator" words." This holds for every interval in the
window.

**5. Preconditions.** Those of P1 and P2. In addition (P3-PRE), R-MOD shows the
normal case: channel 0, unattached, plays each M_V word as two samples on
its own output (p.138: "uses the sample it reads as two bytes of data"), for
example 0 and then 32 for $0020. Each of P3's graded lines needs word intervals
(two lines) or applied-period runs (one line) in its window: with none, no word
has reached channel 1, and the line fails (an applied-event population, per
*Measurements*).

**6. Contrasting cases.** Each changes the program of S-AV or S-P2 and nothing else.
- Every modulator word 0 (prog_s_av_zero, selector P3-SILENT): channel 1 is silent
  in every interval and P3-SILENT fails.
- Channel 1 plays the first four words of M_V as its own data (prog_s_av_ch1bytes,
  selector P3-WHOLE): its words reach the output as two bytes each and no word
  interval has a whole word's level pair; P3-WHOLE fails, and P3-JOINT with it.
- The modulator word 144 of M_P is 129 instead (prog_s_p2_w129, selector P3-SP2): a
  whole word of 129 ticks, the period a byte-wise reading of $8190 would give, so
  a run of 129 ticks is on the output; P3-SP2 fails, and P3-JOINT with it.
- P3-PRE: R-MOD with AUD0VOL = 0 (prog_rmod_vol0) plays no byte.
Each of P3-SILENT and P3-WHOLE reads its own capture, so the contrast of one does
not reach the other.

**7. Reference frame.** As in P1 and P2.

---

## P4. Both attach bits: the words alternate volume, period, starting with volume

**1. Proposition.** With ADKCON bits 0 and 4 both set, channel 0 modulates
both the volume and the period of channel 1 at the same time. Its data-set
words alternate: word 1 is a volume, word 2 a period, word 3 a volume, and
so on.
Claims asserted on P4-A: 5-mod.c2, 5-p149.c6 (p.149); 5-mod.c15,
5-mod.c16, 5-p150.c2, 5-p150.c3, 5-mod.c17, 5-p150.c4, 5-p150.c6,
5-p150.c10, 5-p150.c14, 5-p150.c18, 5-t54.w1.c2, 5-t54.w2.c2,
5-t54.w3.c2, 5-t54.w4.c2 (p.150); 5-appA-p265.c30, 5-appA-p265.c32,
A-adkcon.note1 (p.265). On P4-JOINT: 5-mod.c14, 5-p150.c1 (p.150), whose
sentence needs both halves ("requires both bits"). Used only in part, asserted
on no line: 5-appA-p265.c31 (V5-V0) and A-adkcon.note2 (V6 withdrawn).

**2. Stimulus.** Session S-BOTH: ADKCON $8011, then DMACON $8203. p.150:
"if channel 0 is used to modulate both the period and frequency [sic;
volume is meant, see *Ambiguities*] of channel 1, you set two attach bits --
bit 0 to modulate the volume and bit 4 to modulate the period." Channel 0
plays M_VP (LEN 8, even) at PER 4000. Channel 1 plays SQ at PER 200 and
VOL 63, neither of which occurs in M_VP.

**3. Observation.** Channel 1's own output. All volume changes
and all period changes of channel 1 are merged in time order. Window: from
the first change after M_on until the end of play (see *Measurements*),
containing at least 16 changes.

**4. Expected.**
- (a) (P4-A) The merged sequence is cyclically consecutive in M_VP: V24, P300,
  V56, P700, V8, P500, V40, P900, V24, and so on. So the changes alternate
  in kind, each volume change carries an odd-position word and each period
  change an even-position word. Sources: p.150, "When period and volume are
  both modulated, words in the modulator channel's data set are defined
  alternately as volume and period information"; Table 5-4, column
  "Modulating Both Period and Volume": Word 1 "volume for other channel",
  Word 2 "period for other channel", Word 3 volume, Word 4 period; p.265:
  "First word xxxxxxxx V6-V0 , Second word P15-P0 (etc)".
- (b) (P4-B) The window holds applied changes, and after the first volume change,
  no applied volume lies outside {24, 56, 8, 40}; after the first period change,
  no applied period lies outside {300, 700, 500, 900}. A window with no change
  of either kind is one in which no word has reached channel 1 (an applied-event
  population, per *Measurements*), and (b) fails with it.
- (c) (P4-C) Both parameters change in the window, each at least 8 times. p.149:
  "or do both types of modulation on a channel at the same time".

Which data-set word the first change after M_on carries is not graded here.
The alternation is anchored on data-set positions, through the distinct values.
P4-A, P4-B, P4-C, P4-JOINT and P5-BOTH depend on Figure 5-8's count of
the first pass (the length counter counts on 001->101 only, not on 101->010, so a
first pass is AUDxLEN words): on an implementation whose first pass is one word
longer or shorter, every later word of a dual-mode data set takes the other of
the two roles for the whole session, and these lines fail against Table 5-4
("Word 1 ... volume for other channel", "Word 2 ... period for other channel") in
every pass.

**5. Preconditions.** PC1 ($11), PC2, PC3 (channel 0 at least 2 cycles in
the window), PC4-VOL and PC4-DUR (VSET_1 including 63, 24, 56, 8, 40; R-PER-1 durations
for 200, 300, 500, 700, 900), PC5.

**6. Contrasting cases.** Each changes the data set of S-BOTH and nothing else;
ADKCON stays $8011, so PC1 keeps expecting $11 and stays green.
- The volume words 56 and 8 exchanged (prog_s_both_swap, selector P4-A): every
  value is still a word of M_VP, the order is not; (a) fails (and P4-JOINT,
  which contains it); (b) and (c) hold.
- The last volume word 32 instead of 40 (prog_s_both_v32, selector P4-B): a
  volume outside the four of M_VP; (b) fails, and (a) and P4-JOINT with it, since
  they contain the sets; (c) holds.
- Every period word 300 (prog_s_both_perconst, selector P4-C): the period changes
  once; (c) fails, and (a) and P4-JOINT with it; (b) holds.
p.150: "If you want to modulate both period and volume on the same channel,
you need to attach the channel as both a period and volume modulator."
Table 5-4, "Modulating Volume Only": every word is a volume. Writing $8001
instead of $8011 (S-BOTH-volonly) is graded on its own terms, below.

S-BOTH-volonly (section 6's own stimulus) is also graded on its own terms,
not only as P4's contrast. P4-VOLONLY: with only bit 0 set, channel 1's period
stays at D_1(200) throughout, graded on complete samples only (the sample the
DMA stop cuts short is excluded). That line establishes only one half of
5-mod.c14's and 5-p150.c1's sentence ("modulating both requires both bits"):
it cannot tell "requires both bits" from "attach does nothing", and it carries
no id. P4-JOINT carries them, with both halves: P4-A holds (both bits modulate
both parameters) AND P4-VOLONLY holds on a non-empty population. Channel 1's own
volume-change count in S-BOTH-volonly is measured for context and is not part
of either line: it shares P1-A's population. Contrast of P4-VOLONLY: channel 1's
own AUDxPER rewritten from 200 to 300 halfway through that session
(prog_s_both_volonly_per, selector P4-VOLONLY), so its durations leave D_1(200).

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
Claims: 5-mod.c10, 5-p149.c16 (p.149; P5-BOTH, the one session in which a word
lands at every time-out); 5-p165.c18, 5-sig.pbufld2 (p.165; P5-AP). P5-AV carries
no id. P5-POP-AV, P5-POP-AP and P5-POP-BOTH are the population preconditions of
the three lines.

**2. Stimulus.** Sessions S-AV (volume), S-AP (period, M_P4) and S-BOTH
(both), all with AUD0PER = 4000. Channel 1 plays at periods of at most 900,
far below 4000.

**3. Observation.** Channel 0's fetches at the port and channel 1's changes
on channel 1's own output. Window: from the third fetch of channel 0's own
data set after M_on until the end of play (see *Measurements*), which leaves
out the sample the DMA stop cuts short. By the third fetch the modulator
is in its main loop. p.164: "State 101 is entered as soon as this word
arrives ... When the data arrives, state 010 is entered".

The time-outs cannot be seen directly, because the attached channel's own
output is silenced (P8). The test places them from the fetches. p.164:
"Transitions such as 010->011->010 occur whenever the period counter counts
down to one", and requests go out at transitions, so every fetch follows a
time-out, and the time-outs lie D_mod apart.

**4. Expected.** For every change of channel 1 in the window, let k be the
data-set word whose value it carries, and F_k the latest fetch of word k's
offset before the change. Then F_k < C (the word is written only after it has
been fetched). The page gives the delay from a request to its data only as one
line plus "about 14 clock cycles", not as a bound, so the offset C − F_k is
measured and not graded. What is graded is the time-out grid itself: a word is
written at an instant when the period counter times out, and the time-outs lie on
the counter's own grid, which a line-quantised request does not move.
- Single attach bit (S-AV, S-AP): consecutive changes are D_mod·2 apart: to the
  apparatus resolution for volume changes (S-AV), to ε for period changes (S-AP),
  whose instant is known to the length of one sample. p.164: "In attach volume,
  requests occur as they do in
  normal operation (on the 011->010 transition). In attach period, a set of
  requests occurs on the 010->011 transition". Each is one request per
  010->011->010 loop. p.165: "pbufld2 Like pbufld1, but only during
  010->011 with attach period" -- graded on S-AP, the one session where
  that transition alone carries the request. In the frame PC7-CAL calibrates
  (F_2 enters state 010, and the period counter times out every D_mod after it),
  the 010->011 transitions are the odd time-outs, so every change of S-AP lies
  nearer to an odd multiple of D_mod after F_2 than to an even one (the two
  alternatives are D_mod apart). A word carried on 011->010 would land an even
  multiple.
- Both bits (S-BOTH): consecutive changes are D_mod apart, to ε (a period change is
  in every pair), alternating volume and period. p.164: "When both attach period and
  attach volume are high, requests occur on both transitions".
- Each line needs at least 2L = 8 changes of channel 1 in the window (L = 4, channel 1's
  own length): fewer is a window in which the words did not take effect.

The sentence graded is p.149: "The data words from the modulator channel
are written into the corresponding registers of the modulated channel each
time the period register of the modulator channel times out." "Each time"
is graded on every change and every consecutive pair in the window. With
one attach bit, the page does not say which of the two time-outs after a
fetch carries the word. That is not graded.

**5. Preconditions.** PC1 ($01, $10, $11), PC2, PC3 of the session a line grades,
PC4-VOL, PC4-DUR, PC6 (D_mod); P5-AP also needs PC7-CAL and the start-up frame of
S-AP (at least three fetches follow the calibrated entry r), and prints NOT GRADED
without them. P5-POP-AV, P5-POP-AP and P5-POP-BOTH: the window, from channel 0's
third fetch to M_off, holds at least 2L = 8 fetches of the modulator's words.
They count channel 0's fetches, which exist whatever channel 1 receives, and gate
the three lines (NOT GRADED without them); the changes of channel 1 are what the
lines grade.

**6. Contrasting cases.** Run the session with AUD0PER = 6000 while keeping
the check's D_mod from R-MOD (4000 ticks): prog_s_av_per6000 (P5-AV),
prog_s_ap_per6000 (P5-AP), prog_s_both_per6000 (P5-BOTH). The page then
predicts changes 12,000 (6000 for both bits) ticks apart, and the spacing check
fails. A shorter session leaves fewer than 2L fetches of the modulator's words in
the window: prog_s_av_short (35,000 ticks, P5-POP-AV), prog_s_ap_short (25,500
ticks, P5-POP-AP) and prog_s_both_short (17,500 ticks, P5-POP-BOTH); each still
holds the modulator's first restart, and the precondition of its own line fails.

**7. Reference frame.** The time-out grid is anchored on channel 0's
fetches (p.164 transfer timing). D_mod comes from R-MOD (PC6).

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
(p.265, with the bit 0 and bit 4 rows already cited in P1 and P2), asserted
on P6-A for the bit each names. 5-mod.c20 and 5-p150.c23 ("To attach a channel as
a modulator, you set its attach bit to 1") are asserted on P6-JOINT, which holds
the six attach bits that attach a channel: P1-A for bit 0, P6-A for bits 1 and 2,
P2-A for bit 4, P6-A for bits 5 and 6 (bits 3 and 7 attach nothing: P7).
5-mod.c19 and 5-p150.c22 are used only in part
(the six fixed pairs are checked; that no other pairing can be selected is
not). 5-mod.c22 and 5-p150.c25 have two halves (the channel's output is
disabled, P8; it is attached to the next higher channel, P6-A) and no single
line here establishes both, so they are asserted on none, and so are 5-mod.c7
and 5-p149.c13 (see P1).

**2. Stimulus.** Six sessions S-PAIR-b, all four channels enabled
(DMACON $820F), ADKCON $8000 | (1 << b):

| b | Modulator | Target | Tripwires | Modulator data |
|---|---|---|---|---|
| 0 | ch0 | ch1 volume | ch2, ch3 | M6V |
| 1 | ch1 | ch2 volume | ch0, ch3 | M6V |
| 2 | ch2 | ch3 volume | ch0, ch1 | M6V |
| 4 | ch0 | ch1 period | ch2, ch3 | M_P4 |
| 5 | ch1 | ch2 period | ch0, ch3 | M_P4 |
| 6 | ch2 | ch3 period | ch0, ch1 | M_P4 |

The target's own output and the other (non-target) channels' own output
are each measured from their own reference sessions (see *Output sides
and silence*), not named as a jack.

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
the port fetch record of every channel. Window: from the modulator's first
restart (the first pass is graded in hrm/ch5-audio-dma-first-pass, not here)
until the end of play of the target's output (see *Measurements*), which excludes
the sample the DMA stop cuts short; the fetch records and the other side's
constant-output check end at M_off. It must contain at least 9 changes of the
target.

**4. Expected.**
- (a) (P6-A) Volume sessions: the target's applied volumes are cyclically
  consecutive in M6V, and every level pair on the target side is a
  reference pair of the target channel (R-VOL-c) at an M6V value.
  Period sessions: the target's applied periods, graded on sign-alternation
  samples (see *Measurements*), are cyclically consecutive in M_P4.
- (b) (P6-B) Volume sessions: the target's durations stay at D_c(200).
  Period sessions: the target's level pair stays at the reference for VOL 40.
- (c) (P6-C) The other side's output is constant for the whole window.
- (d) (P6-D) Every non-target channel's consecutive fetch spacings equal
  2·AUDxPER as written, to within less than one line ℓ: 8000 ticks for the
  modulator, 2·PER for each tripwire. p.164 gives one request per two
  transitions, each after a period count-down.
- P6-JOINT: the six attach bits that attach a channel each make the next channel's
  parameter follow the channel's words: P1-A (bit 0), P6-A for bits 1 and 2, P2-A (bit
  4), P6-A for bits 5 and 6, all in their own sessions. 5-mod.c20 and 5-p150.c23.

Sources: p.149, "its data is used to modulate the sound of the next
higher-numbered channel"; p.150, "the channel will be attached to the next
higher channel, as shown in Table 5-5. Because an attached channel always
modulates the next higher numbered channel, you cannot attach channel 3."
"always" is graded by running all six bits. "the next higher" (a single
target) is graded by (c) and (d) on every other channel.

**5. Preconditions.** PC1 (mask per b), PC2, PC3 (all four channels, the
modulator at least 2 cycles after its first restart), PC4-VOL and PC4-DUR (VSET_c
and R-PER-c of the target channel c), PC5.

**6. Contrasting cases.**
- For every b: the second and third modulator words exchanged
  (prog_s_pair<b>_swap, selector P6-b). Every value is still a word of the
  modulator's data set, the order is not, and (a) fails; PC1 keeps expecting the
  bit's mask and stays green.
- To show that (b) can fail: the target's own AUD1PER rewritten from 200 to
  300 halfway through the volume session b = 0 (prog_s_pair0_tgtper, selector
  P6-TGT-0), and the target's own AUD1VOL rewritten from 40 to 20 halfway through
  the period session b = 4 (prog_s_pair4_tgtvol, selector P6-TGT-4).
- To show that the tripwire checks (c) and (d) can fail, write a second
  attach bit that Table 5-5 makes route another channel's words to a tripwire,
  with the setup of the session intact (the tripwires still start silent and at
  their written periods). PC1 then expects the combined mask. The session keeps
  its own program name with the suffix _attc (for (c), selector P6-b-ATTC) or
  _attd (for (d), selector P6-b-ATTD). The routing is the page's, and the
  contrast is a stimulus only:
  - (c), the added bit sends words as volumes to a tripwire, which becomes
    audible on its own output (on the side of the other channels):
    b = 0, write $8005 (bit 2: ch2, W_V, to ch3); b = 1, write $8006 (bit 2: ch2,
    the target, whose words are SQ's, to ch3); b = 2, write $8005 (bit 0: ch0,
    W_V, to ch1); b = 4, write $8014 (bit 2: ch2, W_P, to ch3); b = 5, write
    $8024 (bit 2: ch2, the target, to ch3); b = 6, write $8041 (bit 0: ch0,
    W_P, to ch1).
  - (d), the added bit sends period words to a tripwire or to the modulator, whose
    fetch spacing leaves 2·PER: b = 4, write $8050 (bit 6: ch2, W_P: 600, 1000, to
    ch3, spacing leaves 4000); b = 5, write $8030 (bit 4: ch0 to ch1, the
    modulator, spacing leaves 8000); b = 6, write $8050 (bit 4: ch0 to ch1,
    spacing leaves 3200).
  - b = 0, 1, 2 have no (d) contrast: their modulator and tripwire data are
    volume words (8 to 60, 48, 16), which are not valid periods (p.141: the
    minimum is 124). (d) is the same check code for every b.
- P6-JOINT's contrasts are those of its halves: the words exchanged in S-AV
  (selector P1-A), in S-P2 (selector P2-A) or in one S-PAIR session (selector
  P6-b).

**7. Reference frame.** Each channel's own output is measured directly
(see *Output sides and silence*); PC5 checks only the pairing. Channels
are identified at the port by their data regions. Spacings are in ticks
from D_mod (PC6).

---

## P7. Channel 3's attach bits attach it to nothing

**1. Proposition.** Setting ADKCON bit 3 (ATVOL3, Appendix A: USE3VN) or
bit 7 (ATPER3, USE3PN) makes channel 3's words modulate nothing. No
channel's volume or period changes, channel 3's own included.
Claims: 5-appA-p265.c26 ("03 USE3VN", bit 3) on P7-CONST of S-CH3-V, and
5-appA-p265.c22 ("07 USE3PN", bit 7) on P7-SPACING of S-CH3-P: the volume bit is
seen by the outputs, which stay constant when no volume is modulated, and the
period bit by the fetch spacings, since all four channels are at volume 0 and a
modulated period moves no output. A-adkcon.b03 and A-adkcon.b07 (p.265, with
p.151's parenthesis on bits 7 and 3) bundle "modulates nothing" with "disables
channel 3's audio output": each sits on a joint line, P7-JOINT-V and P7-JOINT-P,
that holds this proposition's line for the bit and P8-ATT for S-DAC3-V or S-DAC3-P.
5-mod.c23 and 5-p150.c26 (p.150) say that an attached channel always modulates the
next higher channel, so channel 3 cannot be attached, and that its modulate bits
only disable its audio output: the second half is P8's, so neither is asserted.

**2. Stimulus.**
- S-CH3-V: ADKCON $8008, DMACON $820F. ch3 plays W_V (valid volume words
  48 and 16) at LEN 2, PER 2000, VOL 0. ch0, ch1 and ch2 are tripwires
  playing W_V at LEN 2, VOL 0, PER 1400, 1600 and 1800.
- S-CH3-P: ADKCON $8080. The same, with W_P (valid period words 600 and
  1000) for all four channels.

**3. Observation.** Both outputs, and the fetch record of all four channels.
Window: from ch3's third fetch after M_on until M_off.

**4. Expected.** (P7-CONST) Both outputs are constant for the whole window.
(P7-SPACING) Every channel's consecutive fetch spacings equal 2·AUDxPER to within
less than one line ℓ: 2800, 3200, 3600 and 4000 ticks. Sources: p.151 Table 5-5, "7 ATPER3 Use audio channel
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
  and 16 and plays its bytes 0, 48, 0, 16 audibly on its own output, so
  the "constant" check fails.
- S-CH3-P with $8040 (bit 6) instead of $8080: ch3's period takes 600 and
  1000, and its spacing leaves 4000.
- PC1 then expects the new mask.
The programs are prog_s_ch3v_att (selector P7V, P7-CONST) and prog_s_ch3p_att (selector
P7P, P7-SPACING). The joint lines P7-JOINT-V and P7-JOINT-P fail with these (they
contain P7-CONST and P7-SPACING) and with P8-OMIT (they contain P8-ATT).
P7-CONST for bit 7 and P7-SPACING for bit 3 are the same calls as the ones that have
contrasts.

**7. Reference frame.** As in P6.

---

## P8. An attached channel's data does not reach its own converter

**1. Proposition.** While either attach bit of a channel is set, including
channel 3's, the words its DMA delivers are not sent to its own
digital-to-analog converter, so the channel's output does not follow its
data.
Claims: A-adkcon.b03, A-adkcon.b07 (p.265, with p.151's parenthesis on bits 7
and 3), asserted on the joint lines P7-JOINT-V and P7-JOINT-P (see P7), which hold
P8-ATT for channel 3 as their converter half. 5-p165.c19 and 5-sig.audxav
(AUDxAV), 5-p165.c20 and 5-sig.audxap (AUDxAP): each says the data goes to the
next channel's latch instead of to the converter, which is two halves, for every
channel x. The joint lines P1-JOINT (S-AV, in P1) and P8-JOINT-AP (S-AP, below) hold
both halves for channel 0 and carry no id (see *Claims used only in part*); no joint
line exists for S-BOTH. P8-ATT for channel 0 grades the converter half alone and
carries no id.

**2. Stimulus.**
- Channel 0 at VOL 63, alone on its own output, in S-AV (bit 0), S-AP
  (bit 4) and S-BOTH (bits 0 and 4).
- Channel 3 at VOL 63, alone on its own output, in S-DAC3-V (bit 3, W_V)
  and S-DAC3-P (bit 7, W_P).

**3. Observation.** The attached channel's own output (see *Output sides
and silence*). Window: from the attached channel's third fetch after M_on,
shifted by L_a to align audio time with port time, until the end of play (see
*Measurements*). By the third fetch words are flowing (p.164). The destination
half of P8-JOINT-AP counts channel 1's applied-period changes from the modulator's
first restart (the first pass is graded in hrm/ch5-audio-dma-first-pass, not here).

**4. Expected.** (P8-ATT) The channel's own output has one value for the whole
window, with no change. p.165: "AUDxAV Attach volume. Send data to volume
latch of next channel instead of to D->A converter." "AUDxAP Attach
period. Send data to period latch of next channel instead of to the D->A
converter." p.151 on bits 7 and 3: "(disables audio output of channel
3)". Which value the output holds is stated on no page (p.150 says only
that the output "will be disabled"), so the value is not graded.
(P8-JOINT-AP) For S-AP both halves of AUDxAP's sentence in one line: channel 1's
applied periods follow channel 0's words cyclically in M_P4, from at least 8
changes (the destination half, graded as P5-AP's changes are), AND channel 0's
own output holds one value (the source half).

**5. Preconditions.** PC1, PC2, PC3 of the attached session (the attached channel
fetches, so data is flowing). The same channel with the same data, period and
volume, unattached, changes its own output at least twice per data word
(P8-PRE): R-MOD (M_V), R-DAC-P4 (M_P4), R-DAC-VP (M_VP), R-DAC3-V (W_V) and
R-DAC3-P (W_P). P8-ATT of a session prints NOT GRADED where PC1-PC3 of the session
or its P8-PRE fails.

**6. Contrasting cases.** Omit the attach write: the matching unattached session,
the one P8-PRE grades, is graded in the attached one's place (selector P8-OMIT:
R-MOD for S-AV, R-DAC-P4 for S-AP, R-DAC-VP for S-BOTH, R-DAC3-V for S-DAC3-V and
R-DAC3-P for S-DAC3-P). The page predicts the channel's bytes on its own output
(p.138), and the "constant" check fails in all five lines and in P8-JOINT-AP and
P7-JOINT-V and P7-JOINT-P, which contain them. The PC lines of the attached
sessions are graded as in the ordinary run. P8-JOINT-AP's destination half: the
second and third words of M_P4 exchanged in S-AP (prog_s_ap_swap, selector
P8-JOINT-AP), which leaves P5-AP and the source half as they are. P8-PRE: R-MOD
with AUD0VOL = 0 (prog_rmod_vol0, selector P8-PRE) changes nothing on its own
output; P8-ATT of S-AV prints NOT GRADED.

**7. Reference frame.** Each channel's own output is measured directly
(see *Output sides and silence*). The window is anchored on the attached
channel's own fetches.

---

## P9. The modulator's and the modulated channel's data-set lengths are independent

**1. Proposition.** The modulator repeats its data set after its own
AUDxLEN words and the modulated channel after its own, whatever the ratio
of the two. The modulation follows the modulator's cycle across the
modulated channel's restarts.
Claims: 5-mod.c18, 5-p150.c21 (p.150) say the lengths are "completely
independent". P9 observes two length pairs of S-LEN-A and S-LEN-B (5 and 3, 2 and
7), which does not establish "completely": the ids are asserted on no line (see
*Claims used only in part*), and the two ratios are a scoped observation.

**2. Stimulus.** Two sessions, both with ADKCON $8001:
- S-LEN-A: modulator 5 words (M_LA), modulated channel 3 words (SQ).
- S-LEN-B: modulator 2 words (M_LB), modulated channel 7 words (SQ).

The lengths are coprime, and the modulator's is once longer and once
shorter than the modulated channel's.

**3. Observation.** The port fetch records of channels 0 and 1, and the
channel 1's own output. Window: from each channel's own first restart (the
same anchor PC3 grades from) until the end of play (see *Measurements*);
the fetch checks end at M_off. Anchoring on a value
departure instead is unsound here: channel 1's own first-pass overrun (its
data region read beyond its own AUDxLEN before its first restart, graded
in hrm/ch5-audio-dma-first-pass, not here) can produce a value that
coincidentally matches a reference pair and opens a window inside that
overrun. It must contain at least 2 modulator cycles.

**4. Expected.**
- (a) (P9-A) Channel 0's offsets step 0 ... L_m−1 and then back to 0, with
  L_m = 5 or 2.
- (b) (P9-B) Channel 1's offsets step 0 ... L_c−1 and then back to 0, with
  L_c = 3 or 7.
- (c) (P9-C) The window holds applied volumes of channel 1, and they are cyclically
  consecutive in M_LA or M_LB. None is skipped or repeated at channel 1's restarts.
- (d) (P9-D) Channel 1's durations stay at D_1(200).

p.150: "The lengths of the data sets of the modulator and the modulated
channels are completely independent." "completely" is graded for the two length
pairs and orderings above, and by (c) holding across every restart of either
channel; other pairs are not run.

**5. Preconditions.** PC1 ($01), PC2, PC3, PC4-VOL (VSET_1 including 8, 48, 20, 60,
32), PC4-DUR, of the ordinary program of the session (PC3's stepping is the check
P9-A and P9-B make, so a contrast of those lines does not reach it). P9-WINDOW: the window opens after each channel's own first restart,
with fetches recorded for both channels; where it does not, it prints as a
precondition not met (RECORDED, not FAIL) and P9-A to P9-D print NOT GRADED, as they
do when PC1, PC2 or PC3 of the session's ordinary program fails. (c) needs channel 1's
applied-volume changes in the window, not counting the value in force at the window's
edge: with none, no word has reached the volume, and (c) fails (an applied-event
population, per *Measurements*). (d)'s population is the window's complete samples,
which exist whatever channel 1 receives. The offsets of each channel are read in its own 4 KB region, so
that a fetch beyond the channel's own length is seen.

**6. Contrasting cases.** Every line grades with the page's own lengths
(L_m = 5, L_c = 3) and the page's data set whichever program runs. Each changes
the program of S-LEN-A and nothing else.
- AUD0LEN = 10 over a table that holds M_LA twice, 260,000 ticks
  (prog_s_lena_len10, selector P9-A). p.147 and p.145 ("Set the length register,
  AUDxLEN, to the number of data words to be retrieved before starting at the
  address currently in AUDxLC") predict a 10-word cycle: the volumes still cycle
  through M_LA, the offsets do not wrap at 5, and (a) with L_m = 5 fails.
- AUD1LEN = 2 (prog_s_lena_ch1len2, selector P9-B): channel 1 cycles two words,
  and (b) with L_c = 3 fails.
- The third and fourth words of M_LA exchanged (prog_s_lena_swap, selector P9-C):
  the offsets are right, the order of the volumes is not, and (c) fails.
- Channel 1's own AUDxPER rewritten from 200 to 300 halfway through
  (prog_s_lena_per1write, selector P9-D): (d) fails.
- AUD1LEN = 1000 (prog_s_lena_ch1len1000, selector P9-WINDOW): channel 1 does not
  restart in the session, P9-WINDOW is not met and P9-A to P9-D print NOT GRADED.

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
5-sm.c35 (p.164); F58.loop.attach (p.166, Figure 5-8). 5-p165.c22 and
5-sig.napnav (p.165, DMA part) are recorded, not asserted (see *Claims used only in
part*): P10's phases are the only grade of the DMA requests they condition.

**2. Stimulus.** R-MOD (normal), S-AV ($01), S-AP ($10) and S-BOTH ($11),
all with AUD0PER = 4000.

**3. Observation.** Channel 0's fetch times at the port, in the frame PC7-CAL
calibrates in R-MOD: the page's F_2 is record entry r, and the page's F_n is
record entry r + n - 2 in every session (the start-up is the same in every
session). p.164: "Because of
pipelining in Agnus, the first data word must be thrown away. State 101 is
entered as soon as this word arrives; a request for the next data word has
already gone out. When the data arrives, state 010 is entered and the main
loop continues". Time-outs then follow every D_mod. The page gives no address
for F_1, so nothing here depends on whether the record holds it. Only the
fetches from the page's F_3 on are graded. A line whose frame is not
established (PC7-CAL not met, or too few fetches) prints NOT GRADED.

**4. Expected.**
- P10-AV (S-AV): the rate, o_n = F_n − F_3 − 2(n−3)·D_mod, is less than one line ℓ
  from 0 for every n ≥ 4 (the requests from F_3 on lie on the period counter's
  grid, 2·D_mod apart), AND the phase of every fetch, (F_n − F_2) mod 2·D_mod for
  n ≥ 3, is R-MOD's rather than the attach-period phase: it lies nearer to R-MOD's
  phase than to R-MOD's plus D_mod (the two alternatives are D_mod apart). p.164:
  "In attach volume, requests occur as they
  do in normal operation (on the 011->010 transition)." p.165: "napnav
  /AUDxAV * /AUDxAP + AUDxAV -- no attach stuff or else attach volume.
  Condition for normal DMA and interrupt requests."
- P10-AP (S-AP): the phase of the fetches, (F_n − F_2) mod 2·D_mod for n ≥ 3, is
  the attach-period phase: every fetch lies nearer to R-MOD's phase plus D_mod
  (mod 2·D_mod) than to R-MOD's own; R-MOD's phase is
  the median of its own phases over n ≥ 3. p.164: "In attach period, a set of
  requests occurs on the 010->011 transition." Figure 5-8, arrow 010->011:
  "[pbufld2 [printed pubfld2] if AUDxAP, AUDxDR if (AUDxAP & AUDxON),
  percntrld]". The 011->010 request "AUDxDR if (AUDxON & napnav)" is off,
  because napnav is false, and so is the 101->010 request "AUDxDR if napnav".
  Grading the phase against R-MOD's needs no assumption about which request
  produced which fetch.
- P10-BOTH (S-BOTH): consecutive fetches from the page's F_3 on are D_mod
  apart, to within less than one line ℓ. p.164: "When both attach period and attach volume are high,
  requests occur on both transitions." (F_3 is the request of the 101->010
  transition, which napnav = 1 allows, and F_4 that of the first 010->011.)

Windows for neighbouring phases are 4000 ticks apart. n = 3 for normal
operation is graded by PC7-N3, which carries no claim id (no claim in either
claims file of this package names the 101->010 transition).

**5. Preconditions.** PC1, PC2 and PC3 of the session a line grades, PC6, and
PC7-CAL with the start-up frame of that session (at least three entries follow its
record entry r); a P10 line prints NOT GRADED where one of
them fails. PC7-LOOP and PC7-N3 are printed beside them and check the anchor
(normal operation shows F_n − F_{n−1} = 2·D_mod to within ℓ from F_3 on, and
0 < F_3 − F_2 < D_mod); they are not gates of P10.

**6. Contrasting cases.** P10-AV: S-AV with AUD0PER = 6000 (prog_s_av_per6000,
selector P10-AV), graded with R-MOD's D_mod: the fetches are 12,000 ticks apart,
not 8000, and the rate check fails. S-AP's capture graded through P10-AV's comparator
(selector P10-AV-PHASE): the spacing is the same as in normal operation, the phase is
one D_mod later, so the line fails by the phase alone. P10-AP and P10-BOTH: S-AV's
capture (attach write $8001, M_V) in place of S-AP's or S-BOTH's. The page predicts
the attach-period phase for S-AP and the doubled fetch rate for S-BOTH, so S-AV's
capture fails each comparator.

**7. Reference frame.** The loop entry F_2 comes from p.164's start-up
description and is calibrated by PC7-CAL on normal operation. D_mod comes from
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
   thrown away". The page gives no address for it, so the fetch record may or
   may not hold it, and no check depends on where it was read from. Which
   data-set word is the one whose arrival enters state 010 (F_2) is not said
   by that sentence, nor by any other single sentence (Table 5-4 and p.265 make the
   first word of a data set the volume, and Figure 5-8 loads the word that arrives
   on 101->010 with pbufld1, but the page does not say whether the word thrown
   away was word 1), and nothing here depends on it. PC7-CAL finds in the run which
   record entry is the page's F_2 by time alone (see *Measurements*, Fetch record).
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
12. **Which output is which.** p.137: "Channels 1 and 2 are connected to
    the left-side stereo output jack. Channels 0 and 3 are connected to
    the right-side output jack." p.326 (Appendix E, connector J6): AUDO
    "Audio output from left (channels 0, 3) port, intended to send audio
    to the modem."; AUDI "... this input is mixed with the analog output
    of the right (channels 1, 2)." The two pages name opposite jacks for
    the same two channel pairs. They agree only on the pairing itself --
    0 with 3 on one output, 1 with 2 on the other, the two outputs
    different -- and every proposition and PC5 grade only that pairing
    (see *Output sides and silence*).

---

## Mutation scope

Every assertion and every precondition of this package has `// MUTATION` records in
the header of `test_ch5_audio_attach_modulation.cpp`, one per contrast, and the tables
under *Measured values* give one row per record and per core. A record is a
**stimulus** mutation: the program changes (or the capture of another session is
graded in its place); the comparator, the expected value, the window, the observed
channel and the populations stay, and the line of that id prints its verdict through
the unchanged comparator. No selector inverts a verdict, and no selector reaches a gate
in code: each `NMH_MUTATE` branch changes only the stimulus (the program a session
runs, or the capture a line grades). Result **red**: the line prints FAIL. Result
**not graded**: the contrast of a precondition makes the precondition fail and the
lines gated on it print NOT GRADED. Every mutated run reaches DONE (a MEASURED line says
so). A contrast that adds an attach bit makes PC1 expect the combined mask.

**Page model.** `page_model.patch` changes the audio RTL of `src/minimig-aga`.
`make pm-test-fx68k` and `make pm-test-tg68k` apply it to a private copy under
`obj_dir_pm/` and run the package against that copy in `obj_dir_pm_<core>/`,
writing `out_pm_<core>.txt`, so the ordinary run's `obj_dir_<core>/` and
`out_<core>.txt` are never replaced; the pinned submodule is never modified. It
implements, as pages 149-151 and 164-165 and Figure 5-8 describe them: ADKCON bit x
is AUDxAV and bit 4+x is AUDxAP of channel x; napnav = /AUDxAV * /AUDxAP + AUDxAV;
the request "AUDxDR if napnav" on 101->010 and on 011->010, "AUDxDR if (AUDxAP &
AUDxON)" on 010->011; pbufld1 sends the holding latch to the next channel's volume
latch when AUDxAV is set, pbufld2 (010->011) to its period latch when AUDxAP is set;
an attached channel's own output is disabled; and the length counter counts on
001->101 only, not on 101->010 (the figure's lencount), so a first pass is AUDxLEN
words. The windows of P1, P2, P6 and
the destination half of P8-JOINT-AP, and P9-C, start at the modulator's first
restart and do not depend on the last item; P4-A, P4-B, P4-C, P4-JOINT and P5-BOTH do
(see P4). A run against the page model is a positive control: it shows that every
line can pass. It is never a mutation record, and no expected value was taken from
it. `PM_PATCH` and `PM_NAME` build the same targets against another patch file in
directories of their own.

**Lines without a contrast of their own, and why.** A line graded by the same call as one
that has a record has none of its own.
- PC1, PC2 and PC3 are one function each; their records run it on one session.
  PC4-VOL for R-VOL-2 and R-VOL-3, PC4-DUR for R-PER-2, R-PER-3 and R-RAMP, P6-B for
  bits 1, 2, 5 and 6, P6-D for bits 0, 1 and 2, and P9-A to P9-D for S-LEN-B are the same
  calls as the ones that have records. P6-D has no contrast for bits 0, 1 and 2: their
  data are volume words, which are not valid periods.
- P7-CONST for bit 7 and P7-SPACING for bit 3 are the same calls as the ones that have
  records (P7-CONST for bit 3, P7-SPACING for bit 7).
- P5-POP-AV, P5-POP-AP, P5-POP-BOTH, P1-JOINT, P2-JOINT, P3-JOINT, P4-JOINT, P6-JOINT,
  P7-JOINT-V, P7-JOINT-P and P8-JOINT-AP are joint lines: each records the contrast of each
  half it contains.
- P2-C, P2-JOINT and P3-SP2 replay the 2.7 M tick S-P2 capture with another data set
  (prog_s_p2_ramp, prog_s_p2_w129); P3-SP2's contrast is a whole word of 129 ticks, the
  signature of a byte-wise reading of $8190, not a byte-wise reading itself, which no
  stimulus can make a page-conforming channel perform.

## Measured values

Everything below is printed by `make -k -B test` (the MEASURED lines, the counts) or by
one `NMH_MUTATE=<value>` run (the records). RTL is the pinned submodule's audio RTL
(4315acba); PM is the page model (`make pm-test-fx68k`, `make pm-test-tg68k`). Every
bound is one line (Units), the apparatus resolution (Units), ε (Units), or a reference
measured in the same run.

**Counts of the ordinary run** (`make -k -B test`; the test exits 1 while a line is red,
so make exits 2): RTL 177 passed, 37 failed on fx68k and on TG68K, every verdict equal on
the two cores; PM 214 passed, 0 failed on fx68k and on TG68K. The 37 reds of the RTL are
P1-A, P1-B, P1-C, P1-JOINT, P2-A, P2-B, P2-JOINT, P3-SILENT, P3-WHOLE, P3-SP2, P3-JOINT,
P4-A, P4-B, P4-C, P4-JOINT, P5-AV, P5-AP, P5-BOTH, P6-A for bits 0, 1, 2, 4, 5 and 6,
P6-JOINT, P8-ATT for S-AV, S-AP, S-BOTH, S-DAC3-V and S-DAC3-P, P8-JOINT-AP, P7-JOINT-V,
P7-JOINT-P, P9-C for S-LEN-A and S-LEN-B, P10-AP and P10-BOTH.

**Verdicts that moved against the previous population gating** (identical on fx68k and
TG68K): P1-B, P1-C, P2-B, P3-SILENT, P3-WHOLE, P3-SP2, P4-B, P5-AV, P5-AP, P5-BOTH and P9-C
(both sessions) NOT EVALUATED -> FAIL; P2-C NOT EVALUATED -> PASS (the waveform is graded on
its own); P5-POP-AV, P5-POP-AP, P5-POP-BOTH NOT EVALUATED -> PASS (they count the
modulator's fetches); P2-JOINT, P3-JOINT and P6-JOINT are new lines and FAIL.

**Apparatus constants.**

| Constant | RTL/fx68k | RTL/TG68K | PM/fx68k | PM/TG68K |
|---|---|---|---|---|
| D_mod (R-MOD, AUD0PER=4000), clk_sys cycles | 32000 | 32000 | 32000 | 32000 |
| 1 tick = D_mod / 4000, clk_sys cycles | 8 | 8 | 8 | 8 |
| one line (F_3 − F_2 of R-MOD), clk_sys cycles | 1816 | 1816 | 1816 | 1816 |
| PC5 measured sides (0=left, 1=right) | ch0=0 ch1=1 ch2=1 ch3=0 | ch0=0 ch1=1 ch2=1 ch3=0 | ch0=0 ch1=1 ch2=1 ch3=0 | ch0=0 ch1=1 ch2=1 ch3=0 |
| L_a (R-VOL-1, channel 1), clk_sys cycles | -77 | -77 | -77 | -77 |
| PC7-CAL: state 010 entered at M_on+, clk | 2647 | 2271 | 2647 | 2271 |
| PC7-CAL: nearest record entry / entries, offset, distance | 0 of 46, offset 0, 11 clk | 0 of 46, offset 0, 11 clk | 0 of 46, offset 0, 11 clk | 0 of 46, offset 0, 11 clk |
| ε = D_1(900), clk_sys cycles | 7200 | 7200 | 7200 | 7200 |
| S-AV: the window of channel 1 ends before M_off by, clk | 80 | 1528 | 80 | 1528 |
| P5: applied changes in S-AV / S-AP / S-BOTH | 0 / 0 / 0 | 0 / 0 / 0 | 43 / 22 / 47 | 43 / 22 / 47 |
| P5: largest deviation of a gap from 2·D_mod (S-AV, volume) / S-AP / from D_mod (S-BOTH), clk | - | - | 0 / 4000 / 4816 | 0 / 4000 / 4816 |
| P5: offset of an S-AP change from the nearest multiple of D_mod after F_2, clk | - | - | 27 .. 4827 | 27 .. 4827 |
| P5: C − F_k over the three sessions, clk (measured, not graded) | - | - | 30107 .. 68523 | 30107 .. 68523 |

The audio output of a channel goes idle before M_off when the DMA stop is written (the
window of S-AV's channel 1 ends that many clk before M_off, inside the sample the stop cuts
short; the sample differs between the cores).

**Records.** One row per record, line and run; "A -> B" is the line's verdict without and
with the contrast (a "#n" names the n-th line printed with that id: for P6 the n-th
S-PAIR session in the order bits 0, 1, 2, 4, 5, 6; for P8-ATT the sessions S-AV, S-AP,
S-BOTH, S-DAC3-V, S-DAC3-P; for PC1 the S-AV session). A contrast of a precondition ends
in NOT GRADED or FAIL; a contrast of a window precondition ends in PRECONDITION NOT MET.
A verdict that stays FAIL on the RTL is a line that is red with and without the contrast.

| Line | NMH_MUTATE | Change | RTL/fx68k | RTL/TG68K | PM/fx68k | PM/TG68K |
|---|---|---|---|---|---|---|
| FIXTURE-STORE | FIXTURE-STORE | the program stores $DEAD in place of $BEEF (prog_fixture_mut) | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| FIXTURE-ACTIVITY | FIXTURE-ACTIVITY | AUD0EN is left out of the enable, so channel 0 never plays (prog_fixture_noplay) | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| PC1 | PC1 | the step-4 ADKCON write is omitted in S-AV (prog_s_av_noattach); the line keeps expecting the session's $01 mask | PASS -> FAIL #13 | PASS -> FAIL #13 | PASS -> FAIL #13 | PASS -> FAIL #13 |
| PC2 | PC2 | R-MOD with the enable $8001: AUD0EN without DMAEN (prog_rmod_nodma) replaces the capture of the R-MOD line | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| PC3 | PC3 | R-MOD with AUD0LEN = 7 (prog_rmod_len7) replaces the capture of the R-MOD line, graded with LEN 8 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| PC4-VOL | PC4-VOL | R-VOL-1 writes 40 in the step labelled 60 (prog_rvol1_mut) | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| PC4-DUR | PC4-DUR | R-PER-1 writes 500 in the step labelled 700 (prog_rper1_mut) | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| PC5-VOL | PC5-VOL | R-VOL-1's capture is graded as R-VOL-3's, so channel 3's output is channel 1's | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| PC5-PER | PC5-PER | R-VOL-3's capture is graded as R-PER-1's, so channel 1's output is channel 3's | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| PC6 | PC6 | AUD0PER = 5000 is written halfway through R-MOD (prog_rmod_pc6) | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| PC7-CAL | PC7-CAL | R-MOD with AUD0PER = 6000 from the start (prog_rmod_per6000), graded with R-MOD's D_mod of 4000 ticks | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| PC7-FRAME | PC7-FRAME | M_on is written late, after a delay following the DMACON enable (prog_rmod_pc7frame), so a fetch occurs before it | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| PC7-LOOP | PC7-LOOP | AUD0PER = 3000 is written halfway through R-MOD (prog_rmod_pc7) | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| PC7-N3 | PC7-N3 | S-AP's capture is graded through the same comparator | PASS -> PASS | PASS -> PASS | PASS -> FAIL | PASS -> FAIL |
| P1-WINDOW | P1-WINDOW | channel 1's DMA is stopped after one eighth of the session (prog_s_av_stop): channel 1's output ends before channel 0 restarts | PASS -> PRECONDITION NOT MET | PASS -> PRECONDITION NOT MET | PASS -> PRECONDITION NOT MET | PASS -> PRECONDITION NOT MET |
| P1-A | P1-WINDOW | channel 1's DMA is stopped after one eighth of the session (prog_s_av_stop): channel 1's output ends before channel 0 restarts | FAIL -> NOT GRADED | FAIL -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED |
| P1-A | P1-A | the second and third modulator words are exchanged (prog_s_av_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P1-JOINT | P1-A | the second and third modulator words are exchanged (prog_s_av_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P6-JOINT | P1-A | the second and third modulator words are exchanged (prog_s_av_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P1-B | P1-MV33 | the last modulator word is 33, a volume with no reference level pair (prog_s_av_mv33) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P1-A | P1-MV33 | the last modulator word is 33, a volume with no reference level pair (prog_s_av_mv33) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P1-C | P1-PER1WRITE | channel 1's own AUDxPER is rewritten from 200 to 300 halfway through (prog_s_av_per1write) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P1-JOINT | P1-JOINT | ch3 also plays, audibly, on the output channel 0 is observed on (prog_s_av_ch3): the source half | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P2-A | P2-A | the eighth and ninth modulator words are exchanged (prog_s_p2_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P2-JOINT | P2-A | the eighth and ninth modulator words are exchanged (prog_s_p2_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P6-JOINT | P2-A | the eighth and ninth modulator words are exchanged (prog_s_p2_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P2-B | P2-B | the eighth and ninth modulator words are exchanged (prog_s_p2_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P2-C | P2-C | the second and third words of channel 1's own data (RAMP) are exchanged (prog_s_p2_ramp) | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P2-JOINT | P2-C | the second and third words of channel 1's own data (RAMP) are exchanged (prog_s_p2_ramp) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P3-PRE | P3-PRE | R-MOD with AUD0VOL = 0 (prog_rmod_vol0) plays no byte | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P3-SILENT | P3-SILENT | every modulator word is 0 (prog_s_av_zero) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P3-WHOLE | P3-WHOLE | channel 1 plays the first four words of M_V as its own data (prog_s_av_ch1bytes) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P3-JOINT | P3-WHOLE | channel 1 plays the first four words of M_V as its own data (prog_s_av_ch1bytes) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P3-SP2 | P3-SP2 | the modulator word 144 is 129 (prog_s_p2_w129) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P3-JOINT | P3-SP2 | the modulator word 144 is 129 (prog_s_p2_w129) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-VOLONLY | P4-VOLONLY | channel 1's own AUDxPER is rewritten from 200 to 300 halfway through S-BOTH-volonly (prog_s_both_volonly_per) | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-JOINT | P4-VOLONLY | channel 1's own AUDxPER is rewritten from 200 to 300 halfway through S-BOTH-volonly (prog_s_both_volonly_per) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-A | P4-A | the volume words 56 and 8 are exchanged (prog_s_both_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-JOINT | P4-A | the volume words 56 and 8 are exchanged (prog_s_both_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-B | P4-B | the last volume word is 32 instead of 40 (prog_s_both_v32) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-A | P4-B | the last volume word is 32 instead of 40 (prog_s_both_v32) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-C | P4-C | every period word is 300 (prog_s_both_perconst) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P4-A | P4-C | every period word is 300 (prog_s_both_perconst) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P5-POP-AV | P5-POP-AV | the session lasts 35000 ticks (prog_s_av_short) | PASS -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED |
| P5-AV | P5-POP-AV | the session lasts 35000 ticks (prog_s_av_short) | FAIL -> NOT GRADED | FAIL -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED |
| P5-POP-AP | P5-POP-AP | the session lasts 25500 ticks (prog_s_ap_short) | PASS -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED |
| P5-AP | P5-POP-AP | the session lasts 25500 ticks (prog_s_ap_short) | FAIL -> NOT GRADED | FAIL -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED |
| P5-POP-BOTH | P5-POP-BOTH | the session lasts 17500 ticks (prog_s_both_short) | PASS -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED |
| P5-BOTH | P5-POP-BOTH | the session lasts 17500 ticks (prog_s_both_short) | FAIL -> NOT GRADED | FAIL -> NOT GRADED | PASS -> NOT GRADED | PASS -> NOT GRADED |
| P5-AV | P5-AV | AUD0PER = 6000 (prog_s_av_per6000), graded with R-MOD's D_mod | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P5-AP | P5-AP | AUD0PER = 6000 (prog_s_ap_per6000), graded with R-MOD's D_mod | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P5-BOTH | P5-BOTH | AUD0PER = 6000 (prog_s_both_per6000), graded with R-MOD's D_mod | FAIL -> NOT GRADED | FAIL -> NOT GRADED | PASS -> FAIL | PASS -> FAIL |
| P6-A | P6-0 | bit 0, the second and third modulator words are exchanged (prog_s_pair0_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P6-A | P6-1 | bit 1, the second and third modulator words are exchanged (prog_s_pair1_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #2 | PASS -> FAIL #2 |
| P6-JOINT | P6-1 | bit 1, the second and third modulator words are exchanged (prog_s_pair1_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P6-A | P6-2 | bit 2, the second and third modulator words are exchanged (prog_s_pair2_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #3 | PASS -> FAIL #3 |
| P6-JOINT | P6-2 | bit 2, the second and third modulator words are exchanged (prog_s_pair2_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P6-A | P6-4 | bit 4, the second and third modulator words are exchanged (prog_s_pair4_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #4 | PASS -> FAIL #4 |
| P6-A | P6-5 | bit 5, the second and third modulator words are exchanged (prog_s_pair5_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #5 | PASS -> FAIL #5 |
| P6-JOINT | P6-5 | bit 5, the second and third modulator words are exchanged (prog_s_pair5_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P6-A | P6-6 | bit 6, the second and third modulator words are exchanged (prog_s_pair6_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #6 | PASS -> FAIL #6 |
| P6-JOINT | P6-6 | bit 6, the second and third modulator words are exchanged (prog_s_pair6_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P6-B | P6-TGT-0 | bit 0: the target's own AUDxPER is rewritten from 200 to 300 halfway through (prog_s_pair0_tgtper) | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P6-B | P6-TGT-4 | bit 4: the target's own AUDxVOL is rewritten from 40 to 20 halfway through (prog_s_pair4_tgtvol) | PASS -> FAIL #4 | PASS -> FAIL #4 | PASS -> FAIL #4 | PASS -> FAIL #4 |
| P6-C | P6-0-ATTC | bit 0 with bit 2 added: ch2 routes its words to the volume of ch3, a tripwire ($8005, prog_s_pair0_attc) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P6-C | P6-1-ATTC | bit 1 with bit 2 added: ch2, the target, routes its words to the volume of ch3, a tripwire ($8006, prog_s_pair1_attc) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #2 | PASS -> FAIL #2 |
| P6-C | P6-2-ATTC | bit 2 with bit 0 added: ch0 routes its words to the volume of ch1, a tripwire ($8005, prog_s_pair2_attc) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #3 | PASS -> FAIL #3 |
| P6-C | P6-4-ATTC | bit 4 with bit 2 added: ch2 routes its words to the volume of ch3, a tripwire ($8014, prog_s_pair4_attc) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #4 | PASS -> FAIL #4 |
| P6-C | P6-5-ATTC | bit 5 with bit 2 added: ch2, the target, routes its words to the volume of ch3, a tripwire ($8024, prog_s_pair5_attc) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #5 | PASS -> FAIL #5 |
| P6-C | P6-6-ATTC | bit 6 with bit 0 added: ch0 routes its words to the volume of ch1, a tripwire ($8041, prog_s_pair6_attc) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #6 | PASS -> FAIL #6 |
| P6-D | P6-4-ATTD | bit 4 with bit 6 added: ch2 routes its words to the period of ch3, a tripwire ($8050, prog_s_pair4_attd) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #4 | PASS -> FAIL #4 |
| P6-D | P6-5-ATTD | bit 5 with bit 4 added: ch0 routes its words to the period of ch1, the modulator ($8030, prog_s_pair5_attd) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #5 | PASS -> FAIL #5 |
| P6-D | P6-6-ATTD | bit 6 with bit 4 added: ch0 routes its words to the period of ch1, a tripwire ($8050, prog_s_pair6_attd) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #6 | PASS -> FAIL #6 |
| P7-CONST | P7V | S-CH3-V with bit 2 in place of bit 3: ch2 routes its words to the volume of ch3 ($8004, prog_s_ch3v_att) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P7-JOINT-V | P7V | S-CH3-V with bit 2 in place of bit 3: ch2 routes its words to the volume of ch3 ($8004, prog_s_ch3v_att) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P7-SPACING | P7P | S-CH3-P with bit 6 in place of bit 7: ch2 routes its words to the period of ch3 ($8040, prog_s_ch3p_att) | PASS -> PASS | PASS -> PASS | PASS -> FAIL #2 | PASS -> FAIL #2 |
| P7-JOINT-P | P7P | S-CH3-P with bit 6 in place of bit 7: ch2 routes its words to the period of ch3 ($8040, prog_s_ch3p_att) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P8-ATT | P8-OMIT | the attach write is omitted: the matching unattached session takes the attached one's place | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #1; PASS -> FAIL #2; PASS -> FAIL #3; PASS -> FAIL #4; PASS -> FAIL #5 | PASS -> FAIL #1; PASS -> FAIL #2; PASS -> FAIL #3; PASS -> FAIL #4; PASS -> FAIL #5 |
| P8-JOINT-AP | P8-OMIT | the attach write is omitted: the matching unattached session takes the attached one's place | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P7-JOINT-V | P8-OMIT | the attach write is omitted: the matching unattached session takes the attached one's place | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P7-JOINT-P | P8-OMIT | the attach write is omitted: the matching unattached session takes the attached one's place | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P8-PRE | P8-PRE | R-MOD with AUD0VOL = 0 (prog_rmod_vol0) | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P8-JOINT-AP | P8-JOINT-AP | the second and third words of M_P4 are exchanged in S-AP (prog_s_ap_swap): the destination half | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P9-WINDOW | P9-WINDOW | AUD1LEN = 1000 in S-LEN-A (prog_s_lena_ch1len1000): channel 1 does not restart | PASS -> PRECONDITION NOT MET #2 | PASS -> PRECONDITION NOT MET #2 | PASS -> PRECONDITION NOT MET #2 | PASS -> PRECONDITION NOT MET #2 |
| P9-A | P9-WINDOW | AUD1LEN = 1000 in S-LEN-A (prog_s_lena_ch1len1000): channel 1 does not restart | PASS -> NOT GRADED #1 | PASS -> NOT GRADED #1 | PASS -> NOT GRADED #1 | PASS -> NOT GRADED #1 |
| P9-C | P9-WINDOW | AUD1LEN = 1000 in S-LEN-A (prog_s_lena_ch1len1000): channel 1 does not restart | FAIL -> NOT GRADED #1 | FAIL -> NOT GRADED #1 | PASS -> NOT GRADED #1 | PASS -> NOT GRADED #1 |
| P9-A | P9-A | AUD0LEN = 10 over a table that holds M_LA twice, 260000 ticks (prog_s_lena_len10), graded with L_m = 5 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P9-B | P9-B | AUD1LEN = 2 in S-LEN-A (prog_s_lena_ch1len2), graded with L_c = 3 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P9-C | P9-C | the third and fourth words of M_LA are exchanged (prog_s_lena_swap) | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P9-D | P9-D | channel 1's own AUDxPER is rewritten from 200 to 300 halfway through S-LEN-A (prog_s_lena_per1write) | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 | PASS -> FAIL #1 |
| P10-AV | P10-AV | AUD0PER = 6000 (prog_s_av_per6000), graded with R-MOD's D_mod | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P10-AV | P10-AV-PHASE | S-AP's capture in place of S-AV's: the phase alone | PASS -> PASS | PASS -> PASS | PASS -> FAIL | PASS -> FAIL |
| P10-AP | P10-AP | S-AV's capture in place of S-AP's | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
| P10-BOTH | P10-BOTH | S-AV's capture in place of S-BOTH's | FAIL -> FAIL | FAIL -> FAIL | PASS -> FAIL | PASS -> FAIL |
