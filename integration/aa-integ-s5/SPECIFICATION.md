# Specification: audio data, length, volume and attach bits, serial data registers (package aa-s5)

This document specifies an integration test package. It was written from the
package's claims (`claims-spec-stage.jsonl`) and from the pages of
Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)*,
06/07/91, by an author who never saw the implementation under test or any
test. Where the AA specification builds on the original chip set, pages of
the *Amiga Hardware Reference Manual*, 3rd edition (HRM), are cited for
context and for reference frames; every expected value comes from an AA page.
A builder implements the package from this document and does not change what
it specifies.

The package's 33 claims come from AA page 11 (audio registers and the audio
bits of ADKCON; one sentence ends on page 12) and AA page 27 (SERDAT,
SERDATR). They yield 20 propositions: P1 to P15 on audio, P16 to P20 on the
serial port. The system runs with the AA chip set selected (`chipset_config =
0x10`), PAL, 2 MB of Chip memory. Nothing depends on PAL or NTSC because
every time is counted in colour clocks. Every proposition runs on both builds
that `nanomig.mk` makes, each with its core set to behave as a 68000
(`cpu_config = 0`). The package is graded on the TG68K build only, the build
with `ENABLE_AGA`, NanoMig's AGA configuration. The fx68k build is NanoMig's
non-AGA configuration (`nanomig.mk` gives it no `ENABLE_AGA`), so its
results are recorded, not graded. No proposition
depends on the fx68k build having an AA feature. No claim in this package
needs a disk.

---

## 1. Claims not turned into a proposition

| Claim | Page | Reason |
|---|---|---|
| `sol:AA-audxdat.c8` | 11 | "With maximum volume, each byte can drive the audio outputs with 0.8 volts(peak to peak, typ)". This is an analog voltage. A test sees the audio output only as digital samples, never as volts. "typ" is a typical value, not a limit, and "can drive" states a capability. No expected value can be derived. |
| `sol:AA-audxvol.c5` | 11 | "15-07 Not used". The page calls these bits unused, so it gives no expected value for them, and a test must not invent one. Every stimulus writes them as 0 (HRM p.264: "If a bit is described as unused in a write-only register, be sure to keep that bit clear when writing values to that register."). |
| `sol:AA-serdat.c9` | 27 | "A stop bit must be provided as part of the data word." This is an obligation on the program, not a behaviour of the chip. The page states no outcome for a word that has no stop bit. Every stimulus meets the obligation, and P18 relies on it. The HRM states an outcome for an all-zero word (p.259: "If this register is written with all zeros, no data transmission is initiated"), but the oracle is the AA page, so that outcome is not tested. |
| `sol:AA-audxlen.c2` | 11 | "AUDxLEN 0A4 W", which the claim reads as write-only access. The write half is exercised by P1 (a CPU write at 0A4 takes effect, graded under `sol:AA-audxlen.c1` and `sol:AA-audxlen.c4`). The read half has no expected value: the legend (p.6 and p.10) reads "W=Write, R=Read" and says nothing about what a read of a W address returns. A proposition grades a claim whole or not at all, so this claim is declined. |
| `sol:AA-audxvol.c2` | 11 | "AUDxVOL 0A8 W", write-only access. As for `sol:AA-audxlen.c2`: the write half is exercised by P6, the read half has no expected value. |
| `sol:AA-audxdat.c2` | 11 | "AUDxDAT 0AA W", write-only access. As for `sol:AA-audxlen.c2`: the write half is exercised by P5, the read half has no expected value. |

The HRM forbids reads of such registers (p.263 to 264: "if a register is
marked as a write-only register, do not attempt to read from it, as this may
trash the register and crash the system"), so the program never reads 0A4,
0A8, 0AA or their other-channel addresses.

---

## 2. Conventions shared by all propositions

### 2.1 Registers and the program

- Register addresses on the AA pages are offsets. The CPU reaches offset R at
  `0xDFF000 + R` (HRM p.263: "680x0 address = (chip address) + $DFF000").
  The other-channel members of the audio registers are at +0x10 per channel
  (AA p.7 to p.8: AUD1LEN 0B4, AUD2LEN 0C4, AUD3LEN 0D4; AUD1VOL 0B8 ...;
  AUD1DAT 0BA ...; AUD1LCH 0B0 ...).
- The program writes custom registers only with `MOVE.W`, or with `MOVE.L` on
  a pointer pair (HRM p.139, p.263). It never uses `CLR`, `BSET`, `BCLR`, `ADD`
  or any other read-modify-write instruction on a custom register (HRM p.263: "The CLR
  instruction causes a read and a clear (two accesses) on a 68000"). It never
  reads a W register. Several writes store 0 (`AUDxVOL ← 0`, `AUDxDAT ←
  0x0000`), so every program is assembled for the 68000, vasm's default CPU,
  with no `-m68010` or higher option: with a higher CPU selected, vasm
  assembles `MOVE.W #0,<ea>` as `CLR.W <ea>`, which on a 68000 reads the
  register first. The listing is checked for every such write.
- INTENA, INTREQ, DMACON and ADKCON are set/clear registers: "15 SET/CLR
  Set/Clear control bit. Determines if bits written with a 1 get set or
  cleared. Bits written with a zero are always unchanged." (p.10 ADKCON,
  p.22 DMACON, p.25 INTENA/INTREQ).
- **Baseline S0.** The program runs this at the start of every run, in this order:
  `INTENA ← 0x7FFF`, `INTREQ ← 0x7FFF`, `DMACON ← 0x7FFF`,
  `ADKCON ← 0x08FF` (clears UARTBRK and the audio bits 7 to 0),
  `AUD0VOL..AUD3VOL ← 0`. No CPU interrupt is used anywhere. The program
  polls.
- **Timeouts.** Every wait loop has a timeout of twice its expected length.
  On a timeout the program stores a timeout marker and carries on, so the
  program always finishes. A timeout marker turns the affected check red, or
  the affected precondition where one is named.
- **Memory map.** Each audio channel x has a buffer region `BUFx` that is
  word-aligned, in Chip memory and disjoint from the others (suggested:
  0x0D0000, 0x0D2000, 0x0D4000, 0x0D6000). The region from `BUFx − 0x40` to
  `BUFx + 0x1000` is reserved: during a run, only channel x's audio DMA reads
  it. The program writes it only before the run and where a proposition
  names a write. The program and its log lie elsewhere.
  Every memory address the program writes (buffers, log, results, stack)
  lies in the Chip memory that the harness maps one to one, 0x080000 to
  0x1FFFFF (`nanomig.i`, `nanomig_harness.h`
  `kChipIdentityLo`/`kChipIdentityHi`). Below 0x080000 the CPU cannot reach
  Chip memory after reset: "An additional copy of the system ROM responds
  starting at memory location $00000000. The system RAM that would normally
  be located at this address is not available." (HRM p.223), until the
  program clears CIA-A OVL (HRM p.339), which it does not rely on. The
  buffers also lie below 0x100000, so the "20 bit starting address" of
  AUDxLCH/AUDxLCL (p.11) reaches them whether the 20 bits count bytes or
  words (p.6: "Low word uses DB1-DB15, High word DB0-DB4"). The program
  starts at `PROGRAM_ORG` (0x080400) and
  ends below 0x0C0000; `RESULTS` (0x0C0000) to `DONE_ADDR` (0x0C0FFE) hold
  the stored results; the stack grows down from `PROGRAM_STACK` (0x0FFF00);
  the reserved buffer regions span 0x0CFFC0 to 0x0D7000. None of these
  ranges overlaps another. The poll log (2.3) starts at 0x100000 and grows
  upward, clear of all of them. It never covers `DONE_ADDR`:
  a log store of `DONE_VALUE` (0x600D) there would end the run (`nanomig.i`,
  `DONE`).

### 2.2 Colour clock (reference frame RF-C)

Every time the pages give is in colour clocks, and the memory cycle is one:
"(memory cycles are 280 ns)" (p.21); AUDxPER "The minimum period is 124
color clocks" (p.11); SERPER "1 bit every (N+1)*.2794 Microseconds" (p.27);
VHPOSR "RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)" (p.29). HRM p.194 agrees:
"A memory cycle is approximately 280 ns in duration". Let **c** be the number
of system clocks per colour clock, which is the length of one memory slot at
the Chip-memory port.

*Anchor check G-C.* The program reads VHPOSR (offset 006) in a tight loop for
at least two lines and logs each value (see 2.3). Within one line, fit bits
7 to 0 (H8 to H1) against the store time. The slope must be one count per c
system clocks, within 2 %. The slot starts of the DMA reads seen at the port
must also be congruent modulo c; since the harness does not mark which device
made a read, these are the reads that 2.4 identifies, the audio fetches and
the reads at which `refresh` is high. *Contrast:* fit bits 15 to 8 (V7 to V0) instead.
The page makes that field count lines, not colour clocks, so the slope check
goes red.

### 2.3 Poll log and markers (RF-L)

The program records every register value it reads by storing it to a log in
Chip memory. The store's time at the Chip-memory port is the sample time.
The read happened after the previous log store and before this one. **R** is
the largest interval between consecutive log stores in the loop concerned.
Around every register write that a check depends on, the program stores a
marker word before the write and another after it, so the write happened
between the two marker stores.

*Precondition G-R*, used wherever a bound on R is named: R does not exceed
that bound, on each CPU core. *Contrast:* add a delay loop to the poll loop
so that R exceeds the bound. G-R then goes red.

### 2.4 Audio fetches (RF-F)

A **fetch of channel x** is a read at the Chip-memory port whose word
address lies in channel x's reserved region. Each fetch has an address and a
time. The harness does not say which device made a read: a `RamAccess`
entry holds the cycle, the word address, the data, the direction and the
byte lanes, nothing more (`nanomig_harness.h`). The definition therefore
rests on what else can read the region, and nothing else does: the program
never reads it (2.1), S0 turns every DMA channel off, and each proposition
turns on audio DMA only. The one other reader is memory refresh, which the
port list marks with the `refresh` output ("ram refresh cycle"). A read at a
cycle in which `refresh` is high is excluded; the test samples `refresh`
itself, stepping the system with `run_cycles(1)` and matching the cycle
numbers of the log. Every other read in the region counts as a channel-x
fetch, so a fetch outside `[BUFx, BUFx + 2L)` is seen.
HRM p.194 gives the slot budget as "4 cycles for audio DMA (2 bytes per
channel)" per line, that is one slot per channel per line.

*Precondition G-F*, where named: channel x's fetches occur. *Contrast:*
leave AUDxEN out of the DMACON write. The page makes DMA for the channel
depend on it ("00 AUD0EN Audio channel 0 DMA enable", p.22), so no fetch
occurs and G-F goes red.

### 2.5 Audio output measurement (RF-O)

- **O(t)** is the audio output. The ports give a left and a right output
  (`audio_left`, `audio_right`, port list; `AudioSample` in
  `nanomig_harness.h`), and O is their sum. The AA pages do not say which channel goes to
  which output, so no check depends on the output side. HRM p.137 assigns
  channels 1 and 2 to the left and 0 and 3 to the right; that is not graded.
- **s** is the interval between output samples. **τ** = s + c is the
  tolerance on every measured duration. One **LSB** is the output's smallest
  step.
- **Zero reference Z** is the mean of O over two lines while all four AUDxVOL are 0
  and no audio DMA runs. p.11: "000000=no output". Every level below is
  measured relative to Z.
- **Runs.** At volume 64 the page says "64 ones, no zeros", so a sample's
  output does not vary within the sample. A **run** is a maximal interval
  in which O is constant. Its level is the mean of O − Z over the run,
  excluding its first and last output sample.
- **Levels at volumes below 64.** The page describes those volumes as patterns
  of ones and zeros ("63 0nes, one zero"). If O inside an interval of constant
  data repeats a pattern instead of being constant, the level of the interval
  is the mean over a whole number of pattern repetitions.
- Two levels are **equal** if they differ by at most 1 LSB. The 1 LSB is the
  quantisation of the measurement, the output's smallest step. It is not a
  value from the page, and no check widens it into a band of its own.

*Precondition G-FLAT*, for every check that uses runs: at volume 64, a word
whose two bytes are equal and not 0x00 gives a constant O over its whole
interval, apart from its first and last output sample. If the implementation
filters the audio output, the filter control that HRM p.157 describes (CIA-A
PRA bit 1, the same bit as the power LED, which the test sees) is set to the
state in which G-FLAT holds. The HRM does not say which state that is, so the
builder finds it with this check and reports it. The bit drives its pin only
as an output: HRM p.339 lists "ddra Direction for port A (BFE001);1=output
(set to 0x03)", so the program writes DDRA (byte address 0xBFE201) ← 0x03
before it writes PRA (0xBFE001), both with `MOVE.B`. If G-FLAT holds in neither
state, the run-based checks cannot be graded, and the report says so instead
of reporting red or green. *Contrast:* use a word with two different bytes.
The page predicts two levels ("outputed sequentially", p.11), so O is not
constant over the word and G-FLAT goes red.

### 2.6 Sample period (RF-P)

AUDxPER "contains the Period (rate) of Audio channel x DMA data transfer. The
minimum period is 124 color clocks. ... This corresponds to a maximum sample
frequency of 28.86 khz" (p.11). Since 1/(124 × 279.4 ns) = 28.86 kHz, one
sample, which is one byte, lasts P colour clocks.

*Anchor check G-P*, for every check that measures run lengths or places
windows by samples: with period P and volume 64, every run of a word whose
two bytes differ lasts P·c ± τ, and a word whose two bytes are equal gives one
run of 2P·c ± τ. *Contrast:* double AUDxPER. The runs double and G-P goes red
against the old P.

### 2.7 Audio start sequence

The AA pages prescribe no order for starting an audio channel. The program
follows HRM p.145: write AUDxLCH/AUDxLCL (one `MOVE.L` at offset 0A0 + 0x10x),
then AUDxLEN, AUDxVOL and AUDxPER, then
`DMACON ← 0x8200 | (1 << x)`. p.22: "09 DMAEN Enable all DMA below". To
stop a channel the program writes `DMACON ← (1 << x)`. After starting, the
program reads DMACONR (offset 002) and logs it. *Precondition G-D:* bit 9 and
bit x are 1. *Contrast:* write `0x8000 | (1 << x)` without DMAEN. Bit 9 then
reads 0, G-D goes red, and G-F goes red as well.

### 2.8 Serial line (RF-U)

- **Pins.** TxD is the `uart_tx` output and RxD the `uart_rx` input of the
  port list ("UART/RS232"). The harness holds `uart_rx` at 1 unless a test
  drives it (`nanomig_harness.h`: "idle mark level").
- **Bit time.** T = (N+1)·c for `SERPER = N` (p.27: "If this number is N,
  then the Baud Rate is 1 bit every (N+1)*.2794 Microseconds"). The .2794 µs
  on the page is the NTSC colour clock. HRM p.255 confirms that "All timing
  is done on the basis of a 'color clock'" and gives 281.94 ns on PAL. Every
  serial proposition uses `SERPER ← 0x003F` (N = 63, LONG = 0), so T = 64
  colour clocks.
- **Line format.** The AA page gives the stop bit ("S= stop bit =1", p.27)
  but neither a start bit nor a bit order. These come from the HRM, as a
  reference frame only. p.255: "the receive circuitry expects to see one start bit, eight
  or nine data bits, and at least one stop bit". p.259: "One start bit is
  automatically generated before the first data bit (bit 0) is sent." p.258:
  shifting is "starting with the least significant bit". A frame is a start
  cell, then the data cells, then the stop cell, each one T long. A cell whose
  level equals the stop cell's level is a 1. The idle line is at the stop level.
- **Bit-order independence.** Every data byte used below reads the same in
  both bit orders (0x00, 0xFF, 0x18, 0x24, 0x3C, 0x42, 0x5A, 0x66, 0x7E, 0x81,
  0x99, 0xA5, 0xBD, 0xC3). No check depends on the HRM's LSB-first order.
- *Anchor check G-T*, for every TxD check: every TxD edge lies on a grid of
  pitch T from the frame's first edge, within ±c, and the measured T equals
  (N+1)·c within ±c. *Contrast:* set `SERPER ← 0x007F`. The edges then lie on
  a 128-colour-clock grid and G-T goes red against T = 64.
- *Precondition G-BRK*, for every TxD check: after S0, ADKCONR (offset 010)
  bit 11 reads 0. p.10: "11 UARTBRK Forces a UART break (clears TXD) if
  true". *Contrast:* write `ADKCON ← 0x8800`. Bit 11 reads 1 and G-BRK goes
  red.
- *Anchor check G-RXD*, for every RxD check: SERDATR (offset 018) bit 11 reads
  the level the test drives on RxD: 1 while it drives the idle (stop) level
  and 0 during a 12T low it drives after the observed sequence. p.27: "11 RXD
  RXD pin receives UART serial data for direct bit test by the micro".
  *Contrast:* drive RxD inverted. G-RXD goes red.

### 2.9 Baseline preconditions (G-S0)

After S0, INTENAR (offset 01C) bits 14 to 0 read 0, DMACONR bits 9 to 0 read 0,
and ADKCONR bits 11 and 7 to 0 read 0. *Contrast:* before S0, write
`INTENA ← 0x8080` and `ADKCON ← 0x8001`, then leave out the INTENA and ADKCON
writes of S0. The reads then show those bits, and G-S0 goes red.

### 2.10 Runs and fetches from the second pass on (RF-W)

A **pass** of channel x is one run of its DMA through the data set. Pass 1
begins at the DMA start; pass p ≥ 2 begins with the p-th fetch at `BUFx`
(the blocks of P1). Whether pass 1 outputs every word is open (section 6), so
no output of pass 1 is graded, and runs are paired with fetches only from
pass 2 on:

- **Anchor of pass p ≥ 2.** The first run that starts after the first fetch
  of pass p (its fetch at `BUFx`) is the first run of word 1 of pass p.
  AUDxDAT is a one-word buffer, "the Audio channel x (x=0,1,2,3) DMA data
  buffer. It contains 2 bytes of data" (p.11), so a word is fetched after the
  word before it has begun its output and before its own output begins. Every
  data set anchored this way ends in a word of two equal bytes, which is a
  single run (G-P), so no run of the last word starts between the anchor
  fetch and word 1's run.
- From the anchor on, the runs of the pass belong to its words in RAM order.
  Where a proposition's words each give one run (P2, P3), the j-th run from
  the anchor belongs to the j-th fetch from the anchor fetch. Where words may
  give two runs (P4), word n of the pass occupies the interval of length
  2P·c that starts (n − 1)·2P·c after the anchor run's start (RF-P), and a
  run belongs to the word whose interval holds its midpoint.
- *Precondition G-W*, wherever RF-W is used: no run starts within τ of an
  anchor fetch, so the anchor is unambiguous. It compares times only, never
  levels, so it does not rest on what the runs carry.

---

## 3. Propositions: audio (AA page 11)

### P1. AUDxLEN at 0A4 sets the number of words in each DMA block

**Claims.** `sol:AA-audxlen.c1`, `sol:AA-audxlen.c4`, both on p.11.
(`sol:AA-audxlen.c2` is declined, section 1.)

**Proposition.** A CPU write of L to channel x's AUDxLEN (0A4 for channel 0,
0B4, 0C4, 0D4 for the others) makes the channel's DMA fetch exactly L
consecutive words from its location and then start again at the location,
for every channel.

**Stimulus.** Run S0. For each channel x = 0..3 alone, and for each
L ∈ {1, 2, 3, 7, 16}:
1. Fill `BUFx` with 16 words of any value.
2. Following 2.7: set AUDxLC ← BUFx, AUDxLEN ← L, AUDxVOL ← 0 and AUDxPER ← 200,
   then start the channel.
3. Wait at least 5·L·2·200 colour clocks, then stop the channel.

Then run all four channels at once with L = 3, 5, 7 and 11 on channels 0 to 3,
started with one write `DMACON ← 0x820F`.

**Observation.** The fetches of each channel (RF-F), from the start marker to
the stop marker.

**Expected.** p.11: "This register contains the length (number of words) of
Audio Channel x DMA data." p.6: "LCL,LCH=20 bit Location (starting
address) of DMA data. Used to automaticall[y] restart pointers, such as ...
the Audio sample counter (whenever the audio length count is finished)".
The fetch addresses form the sequence BUFx, BUFx+2, …, BUFx+2(L−1), BUFx,
BUFx+2, … (HRM Table 5-1, p.138, shows consecutive words at +2). This holds
for every fetch in the interval, over at least four complete blocks:
- exactly L distinct addresses are fetched;
- a fetch at BUFx+2(L−1) is always followed by a fetch at BUFx;
- no fetch lies outside `[BUFx, BUFx+2L)`.

In the four-channel run, each channel follows its own L.

**Preconditions.** G-S0, G-C, G-D, G-F. *Anchor:* the first fetch after the
start is at BUFx. p.11 calls AUDxLCH/LCL "the 20 bit starting address
(location)". If the first fetch is elsewhere, the block frame is not
established and P1 is red. The contrast for this anchor: set AUDxLC ← BUFx+4.
The page predicts the first fetch at BUFx+4, and the check against BUFx goes
red.

**Contrasting cases.**
- Write L = 5 instead of 7. The page predicts blocks of 5 words, so the check
  for 7 goes red.
- Address contrast: with channel 0 observed, first write AUD0LEN ← 3, then
  write 7 to 0B4 instead of 0A4. The page predicts that channel 0 keeps
  3-word blocks, so the check for 7 goes red.

**Left open.** The meaning of AUDxLEN = 0 is not stated on the page, so it
is not tested. What a read of 0A4 returns is not stated either (section 1).

---

### P2. AUDxDAT is channel x's DMA data buffer, filled from RAM

**Claims.** `sol:AA-audxdat.c5`, `sol:AA-audxdat.c9`, both on p.11.

**Proposition.** For each channel x = 0..3, the audio DMA transfers the words
it fetches from RAM into channel x's data buffer, so the channel's output
follows the RAM words in fetch order. A word changed in RAM is output
changed at its next fetch.

**Stimulus.** Run S0. For each channel x alone:
1. Fill `BUFx` with 8 words, each made of two equal bytes, in the order
   A, B, A, C, B, D, C, D, where A = 0x6060, B = 0xE0E0, C = 0x2020 and
   D = 0xA0A0. Consecutive words differ, including across the block wrap.
2. Set AUDxLEN ← 8, AUDxPER ← 300 and AUDxVOL ← 0x0040 (64), then start the
   channel (2.7).
3. After about 2.5 blocks (12 000 colour clocks), the CPU writes word 6
   (address BUFx+10, until now D) ← 0x6060 (A). The write is visible at the
   port.
4. Run until 6 blocks have been fetched, then stop.

**Observation.** The fetches of channel x (RF-F) and the output runs (RF-O),
from the first fetch of pass 2 until two words before the stop marker. Runs
are paired with fetches by RF-W; pass 1 is not graded (section 6).

**Expected.** p.11: "This register is the Audio channel x (x=0,1,2,3) DMA data
buffer." "The audio DMA channel controller automatically transfers data to
this register from RAM."
- (a) From the anchor of pass 2 on, the runs pair one to one with the
  fetches, in order: the j-th run from the anchor belongs to the j-th fetch
  from the anchor fetch (RF-W). Two runs are equal in level if and only if
  the words of their fetches are equal. The four word values give four
  distinct levels.
- (b) Each run begins after the time of its fetch.
- (c) Every fetch of BUFx+10 from pass 2 on that follows the CPU's write
  there gives a run at A's level. Every such fetch before that write gives
  D's level. This holds for every such fetch in the interval.

**Preconditions.** G-S0, G-C, G-D, G-F, G-FLAT, G-W, and G-P (each word one
run of 600·c ± τ). The fetches show 8-word blocks (the P1 check). At least
one fetch of BUFx+10 from pass 2 on falls on each side of the RAM write.
*Contrast for the last
precondition:* write RAM before starting the DMA. Then no fetch precedes the
write, and the precondition goes red.

**Contrasting cases.** Step 3 is the contrast built into the run: the page
predicts that the changed word is output from its next fetch on, so a check
that expected D at every fetch of BUFx+10 goes red. As a second contrast,
swap words 3 and 4 in RAM before the start (A, B, C, A, B, D, C, D). The
level pattern then changes, and check (a) against the original pattern goes
red. Neighbouring words still differ, including across the wrap, so each
word is still one run and G-P holds.

**Left open.** Whether the CPU can write the buffer while DMA fills it is not
tested, because the page gives no outcome for that.

---

### P3. Each byte of AUDxDAT is a two's-complement signed integer

**Claim.** `sol:AA-audxdat.c6`, p.11.

**Proposition.** The output level of a byte increases strictly with the
byte's two's-complement value, in both byte positions of the word.

**Stimulus.** Run S0 and use channel 0. `BUF0` holds 9 words, each made of two
equal bytes, with the bytes in this order: 0x00, 0x7F, 0x80, 0x01, 0xFF,
0x60, 0xA0, 0xE0, 0x20. Set AUD0LEN ← 9, AUD0PER ← 300 and AUD0VOL ← 64.
Play four blocks, so that passes 2 and 3 are output whole whether or not
pass 1 is one word short.

**Observation.** The runs of passes 2 and 3, from the anchor of each pass
(RF-W): the j-th run from the anchor belongs to word j. Pass 1 is not graded
(section 6).

**Expected.** p.11: "It contains 2 bytes of data (each byte is a twos
complement signed integer)".
- Each word gives exactly one run, of 600·c ± τ. Both byte positions of that
  word therefore produce the same level.
- The levels are strictly ordered as
  L(0x80) < L(0xA0) < L(0xE0) < L(0xFF) < L(0x00) < L(0x01) < L(0x20) <
  L(0x60) < L(0x7F), each step by at least 1 LSB.
- The same byte gives the same level in both passes.

An unsigned reading would put 0x80 above 0x7F. A sign-magnitude reading would
put 0xE0 below 0xA0. A ones'-complement reading would make 0xFF equal to 0x00.

**Preconditions.** G-S0, G-C, G-D, G-F, G-FLAT, G-P, G-W. The output format has
at least one LSB per data unit at volume 64. The harness documents only the
width of the outputs ("15 bits significant", `AudioSample` in
`nanomig_harness.h`), not their scale, so the scale is measured, on two bytes
that every reading of a byte (unsigned, sign-magnitude, ones' or two's
complement) places at 1 and at 127: |L(0x7F) − L(0x01)| ≥ 126 LSB. The
measurement therefore does not depend on the clause under test. Without it,
the pairs one unit apart (0xFF/0x00/0x01) are not graded and the report says
so.

**Contrasting case.** Swap the words holding 0x7F and 0x80 in RAM. The page
predicts the lowest level at word 2 and the highest at word 3. The order
check, applied by word position, goes red.

**Left open.** Whether L(0x00) equals Z is not stated on the page, so it is
not graded.

---

### P4. The two bytes of AUDxDAT are output one after the other

**Claim.** `sol:AA-audxdat.c7`, p.11.

**Proposition.** Each data word is output as two consecutive samples, one per
byte. Each sample lasts one period, and the pair has the levels of the two
bytes.

**Stimulus.** Run S0 and use channel 0 with volume 64 and period 300. Let
a = 0x20, b = 0x60, c = 0xA0, d = 0xE0. `BUF0` holds these 10 words:
(a,a), (b,c), (d,d), (c,b), (a,a), (b,d), (c,c), (d,b), (a,a), (b,b),
where the high byte is written first. No two neighbouring words, including
across the block wrap, share a byte value, so runs never merge, whichever
byte comes out first. The equal-byte words calibrate L(a), L(b), L(c) and
L(d) within the same pass. Play four blocks, so that passes 2 and 3 are
output whole whether or not pass 1 is one word short.

**Observation.** The runs of passes 2 and 3. There are 14 per pass. Each run
belongs to a word by RF-W: word n of a pass occupies the 600·c interval that
starts (n − 1)·600·c after that pass's anchor run begins. Pass 1 is not
graded (section 6).

**Expected.** p.11: "that are outputed sequentially (with digital to analog
conversion) to the audio output pins." For each word whose two bytes differ,
(b1, b2):
- its interval holds exactly two runs;
- each run lasts 300·c ± τ;
- one run's level equals L(b1) and the other's equals L(b2), with those
  levels calibrated in the same pass;
- no run has any other level, such as a sum or an average of the two.

**Preconditions.** G-S0, G-C, G-D, G-F, G-FLAT, G-P (from the equal-byte
words), G-W. The four calibration levels are pairwise distinct.

**Contrasting case.** Replace (b,c) with (b,b). The page predicts a single
run of 600·c at L(b), so the two-run check goes red at that word.

**Left open.** The AA page does not say which byte comes first. The test
records the order it observes but does not grade it. (HRM p.138 puts the
first sample at "the lowest byte address", which is bits 15 to 8 on a 68000,
but the oracle is the AA page.)

---

### P5. The CPU can write AUDxDAT directly at 0AA

**Claims.** `sol:AA-audxdat.c1`, `sol:AA-audxdat.c10`, both on p.11.
(`sol:AA-audxdat.c2` is declined, section 1.)

**Proposition.** With the channel's DMA off, a CPU write at channel x's
AUDxDAT (0AA for channel 0, then 0BA, 0CA, 0DA) makes the channel output the
two bytes written.

**Stimulus.** Run S0. For each channel x alone, with AUDxEN clear, set
AUDxVOL ← 64 and AUDxPER ← 400. The CPU writes AUDxDAT with each of these
values in turn, waiting 10·P after each write (with markers):
0x0000, 0x7F7F, 0x8080, 0x0000, 0x7F80, 0x0000, 0x807F, 0x0000.
Immediately before each AUDxDAT write the program writes
`INTREQ ← 1 << (7+x)`, so the channel's request is clear when the word
arrives. The HRM makes direct output depend on it: "As long as the interrupt
is cleared by the processor in time, the machine remains in the main loop.
Otherwise, it enters the idle state." (p.164), and Figure 5-8 (p.166) leaves
the idle state on a processor write only under "AUDxDAT · AUDxON‾ ·
AUDxIP‾", that is with the request not pending. The AA page names no
condition, so the stimulus meets the HRM's.

**Observation.** O from each write marker to the next write marker.

**Expected.** p.11: "The Processor can also write directly to this register."
together with "outputed sequentially".
- After 0x7F7F, O settles at one new level H. After 0x8080, it settles at one
  level Lo. After 0x0000, it settles at a level Z0.
- H > Z0 > Lo. This is P3's signed ordering, which gives three distinct
  levels.
- After 0x7F80, and again after 0x807F, the first change of O goes to H or Lo,
  and the next change goes to the other of the two. The window contains a
  run at H and a run at Lo that are adjacent. Between them, and after them,
  O takes no level other than H or Lo.

**Preconditions.** G-S0, G-C, G-FLAT. DMACONR bit x reads 0 and channel x
makes no fetch, so the channel's DMA is off. *Contrast:* start the channel's
DMA. DMACONR bit x then reads 1 and the precondition goes red.

**Contrasting cases.**
- Set AUDxVOL ← 0 for the 0x7F80 write. The page predicts no output
  ("000000=no output"), so O stays at Z and the H/Lo check goes red.
- Address contrast, with channel 0 observed: set AUD1VOL ← 0 and write
  0x7F80 at 0BA instead of 0AA. The page predicts no change in channel 0's
  output, so the check goes red.

**Left open.** How long each byte is held without DMA, what the output does
after both bytes, and when the request is set in this mode are not stated on
the AA page. (HRM p.157 says "the audio output will cease changing. The last
value remains" and that "the interrupt occurs after one data word has been
output".) None of these is graded.

---

### P6. AUDxVOL at 0A8 gives 65 linear levels, and 000000 gives no output

**Claims.** `sol:AA-audxvol.c1`, `sol:AA-audxvol.c4`, `sol:AA-audxvol.c7`,
all on p.11. (`sol:AA-audxvol.c2` is declined, section 1.)

**Proposition.** For every channel, the output amplitude of constant data is
proportional to the volume value v written at AUDxVOL, for v = 0..64. Here
0..63 are written in bits 5 to 0 and 64 is written as bit 6. At v = 0 the
channel gives no output.

**Stimulus.** Run S0. For each channel x alone:
1. `BUFx` holds one word, 0x7F7F. Set AUDxLEN ← 1, AUDxPER ← 200 and
   AUDxVOL ← 0x0040, then start the channel.
2. For v = 0, 1, …, 63, then 0x0040: write AUDxVOL ← v (with markers), wait two
   lines, and measure for the next two lines.
3. Repeat the sweep with the word 0x8080.

**Observation.** For each v, the level A(v) = mean of O − Z over the
measuring window (RF-O, over whole pattern repetitions if O is not constant).

**Expected.** p.11: "Bits 6,5,4,3,2,1,0 specify 65 linear volume levels";
"05-00 Sets one of 64 levels (000000=no output (111111=63 0nes, one zero)".
- For each data word and every v in 0..63, and for 0x0040 (= 64):
  |A(v) − v·A(64)/64| < 1 + v/64 LSB. The bound is the integer quantisation
  of the two measured levels, not a value from the page. The output is an
  integer, so each level is the integer part, or the rounding, of an exact
  linear value: A(v) = v·a + e₁ and A(64) = 64·a + e₂ with |e₁| < 1 and
  |e₂| < 1 LSB. Then A(v) − v·A(64)/64 = e₁ − (v/64)·e₂, whose magnitude is
  below 1 + v/64 LSB. The level for 111111 is 63/64 of the maximum.
  Adjacent levels are |A(64)|/64 apart; where that is less than twice the
  bound, a one-step error is within the quantisation and not seen. The
  contrasting cases change the level by 16 steps or more.
- At v = 0, O equals Z at every output sample of the window, for both data
  words.
- A(64) has opposite signs for 0x7F7F and 0x8080.

(HRM p.140 agrees with linear amplitude: 48 → −2.5 dB, 32 → −6.0 dB,
16 → −12.0 dB.)

**Preconditions.** G-S0, G-C, G-D, G-F. For data 0x7F7F, the larger of
|A(63)| and |A(64)| is at least 126 LSB, so the sweep spans many times the
quantisation bound of the check. The larger of
the two is taken because a level written in bits 5 to 0 alone (63) and the
level written as bit 6 (64) rest on different parts of the clause under test:
an implementation that ignored bit 6 would leave A(64) near 0, and a
precondition on A(64) alone would then report P6 and P7 as not graded
instead of red. Each window starts at least two samples after its write
marker. *Contrast for the resolution precondition:* use the word 0x0101
(data 1). The page predicts |A(63)| and |A(64)| as small as one data unit,
and the precondition goes red.

**Contrasting cases.**
- At the step for v = 32, write 16. The page predicts half the level, and the
  check for 32 goes red.
- Address contrast, with channel 0 observed: write the sweep values at 0B8
  instead of 0A8. The page predicts channel 0's level stays at the last value
  written at 0A8, and the check goes red.

---

### P7. AUDxVOL bit 6 forces the maximum volume

**Claim.** `sol:AA-audxvol.c6`, p.11. (`sol:AA-audxvol.c4`, the 65 linear
levels, is graded whole by P6, which covers bit 6 as the level 64; P7 grades
only its bit-6 part and so does not carry it.)

**Proposition.** With bit 6 set, the level is the maximum level, whatever
bits 5 to 0 hold.

**Stimulus.** The same as P6, for each channel and both data words. After the
P6 sweep, write AUDxVOL ← 0x0040 | k for k = 0..63, with the same wait and
measuring windows.

**Observation.** A(0x40|k) as in P6.

**Expected.** p.11: "06 Forces volume to max (64 ones, no zeros)". For every
k in 0..63, A(0x40|k) equals A(64) (RF-O: within 1 LSB, the quantisation of
the measurement), where A(64) is the level of 0x0040 from the same sweep. The
level for bit 6 therefore exceeds the level for 111111 by A(64)/64.

**Preconditions.** Those of P6.

**Contrasting case.** Write k without bit 6. The page predicts k/64 of the
maximum. For every k ≤ 63 that differs from the maximum by at least
|A(64)|/64, about 2 LSB or more under P6's resolution precondition, which is
more than the 1 LSB of equality, so the check goes red.

---

### P8. An audio interrupt request is set when a DMA block has been output

**Claim.** `sol:AA-audxdat.c11`. The sentence begins on p.11 and ends on p.12.

**Proposition.** For each channel x, each time the channel's DMA has output
its Length words and the last word in AUDxDAT has been used, the
channel's interrupt request bit (INTREQ bit 7 + x) is set.

**Stimulus.** Run S0 (INTENA all clear). For each channel x alone:
0. Calibration: `BUFx+0x100` holds one word, 0x9090. Following 2.7, set
   AUDxLC ← BUFx+0x100, AUDxLEN ← 1, AUDxPER ← 400 and AUDxVOL ← 64, play
   for 8 lines, then stop the channel. The level of its runs is **L9**, the
   level of word L below. A one-word data set outputs the same word whether
   or not its first pass is short, so L9 does not rest on the open question
   of section 6.
1. `BUFx` holds 6 words, each made of two equal bytes: 0x1010, 0x3030, 0x5050,
   0x7070, 0xD0D0, 0x9090. Set AUDxLC ← BUFx, AUDxLEN ← 6, AUDxPER ← 400 and
   AUDxVOL ← 64.
2. Write `INTREQ ← 1 << (7+x)` to clear the request.
3. Start the channel.
4. Poll INTREQR (offset 01E) and log every read. When bit 7+x reads 1, write
   `INTREQ ← 1 << (7+x)` with markers and keep polling.
5. Stop after 7 blocks. p.24: "These bits may be polled by the processor".

**Observation.** The poll log (RF-L), the fetches (RF-F) and the runs (RF-O).
An **event** is a read of 1 in bit 7+x whose previous read in the log was 0.
The event's time is the store time of the read of 1. Block k is the k-th
sequence of fetches that starts at BUFx, counted from the start marker of
step 3 (the calibration's fetches at BUFx+0x100 lie before it). Word L is
the word at BUFx+2(L−1) with L = 6, the stimulus value, which the grader
keeps even in the contrasting case below. **Word L's run** in
block k is found by its level, not by counting runs: it is the first run
that starts after F_k and whose level equals L9 (RF-O).

For each block k = 2..6 there is a window **W_k** = [F_k, E_k + τ + 2R]:
- F_k is the fetch time of word L (BUFx+10) in block k;
- E_k is the start of the run that follows word L's run in block k. At E_k
  the output of word L is over: the data "has been used".

**Expected.** p.11 to 12: "When the DMA data is finished (words outputted=Length)
and the data in this register has been used, an audio channel interrupt
request is set." p.25: "07 AUD0 4 Audio channel 0 block finished",
"08 AUD1", "09 AUD2", "10 AUD3". Every window W_2 … W_6 contains at least
one event.

Why the window has these ends: the pages word the instant differently (see
section 5.1), and the window covers every reading and nothing more. It opens
at the fetch of word L: HRM p.220 puts the request "when the last word in an
audio data stream has been accessed", the earliest of the readings, and HRM
p.164 puts it later, "just as the last word of the waveform starts its
output". It closes where the AA page puts it, when word L "has been used",
at E_k. No latency is added: τ is the resolution of a run start (RF-O), and
2R that of the poll, since a read of 1 follows the store before it and
stores are at most R apart (RF-L). Block 1 is not graded, because it is
pass 1, whose output the pages leave open (section 6). Events outside the
windows are not graded, including the one HRM p.148 predicts at DMA start:
the claim says a request is set at block end, not only then.

**Preconditions.** G-S0, G-C, G-D, G-F, G-FLAT, G-P. G-R with R ≤ P·c/8.
In each block k = 2..6 exactly one run at level L9 starts between F_k and
the fetch of word 2 of block k+1, so word L's run is found without doubt.
INTENAR reads 0 after S0. The fetches show blocks of as many words as the
program wrote to AUDxLEN (the P1 check). The windows, however, are always
placed with L = 6, so the contrasting case below keeps this precondition
green and goes red through the windows. Outside every window, the first poll
after each clear marker reads 0, so the clear took effect. *Contrasts:*
- Leave out the clear write in step 4. The next poll reads 1, and the
  precondition goes red.
- Write `INTENA ← 0xC000 | 1 << (7+x)` after S0. INTENAR reads non-zero, and
  the precondition goes red.

**Contrasting case.** Extend the buffer to 9 words (add 0xB0B0, 0xF0F0,
0x4040) and set AUDxLEN ← 9, while the grader keeps L = 6. The page predicts
requests at the ends of 9-word blocks. The windows placed around word 6
(0x9090, still found by L9) then contain no event, and the check goes red;
0x9090 still occurs once per block, so the L9 precondition holds.

---

### P9 to P11. ADKCON bits 0, 1, 2: channel m modulates the volume of channel m+1

**Claims.** P9: `sol:AA-adkcon.c30` (bit 00, USE0V1). P10:
`sol:AA-adkcon.c29` (bit 01, USE1V2). P11: `sol:AA-adkcon.c28` (bit 02,
USE2V3). All on p.11.

**Proposition.** With ADKCON bit m set, which ADKCONR reads back as 1,
the data words of channel m's DMA set the volume of channel m+1. Channel
m+1's level then takes, cyclically in RAM order, the volumes the modulator's
words hold, in place of the volume the CPU wrote to it.

**Stimulus.** Run S0. For (m, n) = (0, 1), (1, 2), (2, 3):
1. Channel n: `BUFn` holds one word, 0x7F7F. Set AUDnLEN ← 1, AUDnPER ← 200
   and AUDnVOL ← 32.
2. Channel m: `BUFm` holds 0x0040, 0x0010, 0x0028 (volumes 64, 16, 40, with
   bits 15 to 7 zero). Set AUDmLEN ← 3, AUDmPER ← 1500 and AUDmVOL ← 0.
3. Phase 1 (calibration): start channel n only. Write AUDnVOL ← 64, 16, 40,
   32 in turn, with markers, holding each for three lines. Measure L64, L16,
   L40 and L32 over the last two lines of each.
4. Phase 2: write `ADKCON ← 0x8000 | 1 << m`. Read ADKCONR and log it. Start
   channel m. Run for 24 modulator periods, 24 × 1500 = 36 000 colour clocks
   (at least 160 lines), which is at least 12 modulator words however long a
   word governs: HRM p.149 moves a word "each time the period register of
   the modulator channel times out", while HRM p.164 has attach-volume
   requests "on the 011->010 transition", one word per two periods, and the
   AA page states neither.
5. Phase 3: write `ADKCON ← 1 << m`. Read ADKCONR and log it. Stop both
   channels.

**Observation.** Channel n's output in phase 2 (channel m's volume is 0, so
channel m's own output is nothing, p.11). Split it into **segments**: maximal
intervals of constant level (RF-O, over whole pattern repetitions). The
observation starts at the first segment whose level equals L64, L16 or L40
and ends at the phase-3 marker.

**Expected.** p.11: "00 USE0V1 Use audio channel 0 to modulate volume of
channel 1" (and "01 USE1V2 …", "02 USE2V3 …" for P10 and P11). For the
volume field of a modulator word, the same page's NOTE reads "First AUDxDAT
word is used for V6-V0 of AUDxVOL". The words here have bits 15 to 7 at
zero, so the result does not depend on which bits the single-modulation mode
takes. Then:
- every segment in the observation has a level equal to L64, L16 or L40
  (RF-O: within 1 LSB, the quantisation of the measurement);
- no segment has level L32;
- the segments cycle 64 → 16 → 40 → 64 …, starting anywhere;
- each value occurs at least three times.

In phase 2 ADKCONR bit m reads 1 and the other bits 7 to 0 read 0. In phase 3
bit m reads 0 ("Bits written with a zero are always unchanged", p.10).

**Preconditions.** G-S0, G-C, G-D. G-F for both channels: channel m makes at
least 12 fetches in phase 2. The calibration levels L64, L16, L40 and L32
differ pairwise by more than 2 LSB, so no level can equal two of them.
*Contrast:* leave channel
m's DMA off. There are no modulator fetches, and G-F goes red.

**Contrasting cases.**
- Leave out the ADKCON write in phase 2. The page predicts no modulation, so
  channel n stays at L32 and the check goes red.
- Write `ADKCON ← 1 << m` in phase 2, which has SET/CLR = 0 ("Determines if
  bits written with a 1 get set or cleared", p.10). ADKCONR bit m reads 0,
  so the phase-2 readback check goes red, and so does the level check.
- Store the modulator words in the order 0x0028, 0x0010, 0x0040. The page
  predicts the reverse cycle, and the forward-cycle check goes red.

**Left open.** The AA page does not say:
- whether channel m still produces sound. (HRM p.149: "that channel ceases to
  produce sound".) Channel m's volume is 0, so this is not observed;
- when each word takes effect, or for how long. (HRM p.149: "each time the
  period register of the modulator channel times out".) Not graded;
- what channel n's volume is after the bit is cleared. Not graded.

---

### P12 to P14. ADKCON bits 4, 5, 6: channel m modulates the period of channel m+1

**Claims.** P12: `sol:AA-adkcon.c26` (bit 04, USE0P1). P13:
`sol:AA-adkcon.c25` (bit 05, USE1P2). P14: `sol:AA-adkcon.c24` (bit 06,
USE2P3). All on p.11.

**Proposition.** With ADKCON bit 4+m set, which ADKCONR reads back as 1,
the data words of channel m's DMA set the period of channel m+1. Channel
m+1's samples then last, cyclically in RAM order, the periods the
modulator's words hold, in place of the period the CPU wrote to it.

**Stimulus.** Run S0. For (m, n) = (0, 1), (1, 2), (2, 3):
1. Channel n: `BUFn` holds one word, 0x7F80, so consecutive samples alternate
   in level and every sample is a separate run. Set AUDnLEN ← 1,
   AUDnPER ← 400 and AUDnVOL ← 64.
2. Channel m: `BUFm` holds 0x00C8, 0x0258, 0x015E (periods 200, 600, 350,
   all at least the 124 of p.11). Set AUDmLEN ← 3, AUDmPER ← 3000 and
   AUDmVOL ← 0.
3. Phase 1: start channel n only, for 10 lines.
4. Phase 2: write `ADKCON ← 0x8000 | 1 << (4+m)`. Read ADKCONR and log it.
   Start channel m. Run for 24 modulator periods, 24 × 3000 = 72 000 colour
   clocks (at least 320 lines), which is at least 12 modulator words however
   long a word governs: HRM p.149 moves a word "each time the period register
   of the modulator channel times out", while HRM p.164 has attach-period
   requests "on the 010->011 transition", one word per two periods, and the
   AA page states neither.
5. Phase 3: write `ADKCON ← 1 << (4+m)`. Read ADKCONR and log it. Stop.

**Observation.** Channel n's runs (RF-O) in phase 2, from the first run whose
length is classified (below) until the phase-3 marker. A run is classified
as 200, 600 or 350 if its length is within ±τ of that many colour clocks
times c. Otherwise it is transitional. A segment is a maximal sequence of
runs with the same class.

**Expected.** p.11: "04 USE0P1 Use audio channel 0 to modulate period of
channel 1" (and "05 USE1P2 …", "06 USE2P3 …"). For the period field, the
page's NOTE reads "Second AUDxDAT word is used for P15-P0 of AUDxPER", and
here every word is a whole 16-bit period. Then:
- every run in the observation is classified, apart from at most one
  transitional run at each segment boundary;
- the segments cycle 200 → 600 → 350 → 200 …, starting anywhere;
- each value occurs at least three times;
- no run lasts 400·c ± τ except as a transitional run.

ADKCONR bit 4+m reads 1 in phase 2 and 0 in phase 3, and bits 7 to 0 other
than 4+m read 0 throughout.

**Preconditions.** G-S0, G-C, G-D, G-F for both channels, G-FLAT. G-P in
phase 1: all runs last 400·c ± τ. *Contrast:* as in P9 to P11, modulator
DMA left off.

**Contrasting cases.**
- Leave out the ADKCON write. The page predicts unmodulated 400-colour-clock
  runs, and the check goes red.
- Write `ADKCON ← 1 << (4+m)` in phase 2 (SET/CLR = 0), as in P9 to P11.
  The readback check and the run check go red.
- Store the modulator words in reverse order. The page predicts the reverse
  cycle, and the check goes red.

**Left open.** As in P9 to P11: channel m's own output, when a new period
takes effect, and how many samples each modulator word governs. The AA page
also says nothing about the length of a sample during which the period
changes. That is why one transitional run per boundary is allowed.

---

### P15. ADKCON bit 3 (USE3VN): channel 3 modulates nothing

**Claim.** `sol:AA-adkcon.c27`, p.11.

**Proposition.** With ADKCON bit 3 set and channel 3's DMA running, no
channel's volume or period changes. Each of channels 0, 1 and 2 plays exactly
as it does with bit 3 clear.

**Stimulus.** For k = 0, 1, 2, make two runs, A and B. Each run begins with
S0 (2.1), so ADKCON bit 3 is clear at the start of run B.
- Channel k: `BUFk` holds one word, 0x7F80. Set AUDkLEN ← 1, AUDkPER ← 300 and
  AUDkVOL ← 64. The volume is 64 because the check measures run lengths:
  the page describes every lower volume as a pattern of ones and zeros
  ("111111=63 0nes, one zero", p.11), so below 64 the output inside one
  sample need not be constant and a run (RF-O) is not defined; at 64 it is
  ("64 ones, no zeros").
- Channel 3: `BUF3` holds 0x0010, 0x0020. Set AUD3LEN ← 2, AUD3PER ← 700 and
  AUD3VOL ← 0.
- Run A: write `ADKCON ← 0x8008`, read ADKCONR and log it, then start
  channels 3 and k together (`DMACON ← 0x8200 | 0x8 | 1 << k`). Run for 60
  lines.
- Run B: the same without the ADKCON write.

**Observation.** Channel k's runs, from two lines after the start until the
stop.

**Expected.** p.11: "03 USE3VN Use audio channel 3 to modulate nothing". In
run A, every run of channel k lasts 300·c ± τ and has one of the two levels
of channel k in run B (the levels of 0x7F and 0x80 at volume 64), equal
under RF-O (within 1 LSB, the quantisation of the measurement). ADKCONR bit 3
reads 1 in run A and 0 in run B.

**Preconditions.** G-S0, G-C, G-D, G-FLAT, G-F for channels 3 and k in run A.
*Contrast:* leave channel 3's DMA off. There are no channel-3 fetches, and
the precondition goes red.

**Contrasting cases.** The page predicts a changed outcome here:
- for k = 1: set bit 0 (USE0V1) in addition to bit 3 (`ADKCON ← 0x8009`),
  with channel 0 carrying 0x0010, 0x0020 (AUD0LEN ← 2, AUD0PER ← 700,
  AUD0VOL ← 0) and started with channels 3 and 1. Channel 1's level changes,
  so the check against run B goes red, while ADKCONR bit 3 still reads 1.
- for k = 2: set bit 1 in addition to bit 3, with channel 1 carrying the
  words. The check goes red.
- for k = 0: no bit on the page modulates channel 0. The contrast is a CPU
  write AUD0VOL ← 16 mid-run. By P6 the page predicts 16/64 of the level
  from then on, so the check against run B goes red.

**Left open.** Whether channel 3 still produces sound is not stated. (HRM
p.150: "Writing a 1 into channel 3's modulate bits only disables its audio
output"; Table 5-5, p.151: "(disables audio output of channel 3)".) Channel
3's volume is 0, so this is not observed.

---

## 4. Propositions: serial port (AA page 27)

All serial runs use S0, then `SERPER ← 0x003F`, and then check G-BRK. The
program logs every register read (RF-L). The data bytes are bit palindromes
(2.8).

### P16. A word written to SERDAT is moved into the shift register whenever the shift register is empty, and is transmitted

**Claims.** `sol:AA-serdat.c6`, `sol:AA-serdat.c7`, both on p.27.

**Proposition.** Every word written to SERDAT goes to the transmit buffer and
is transmitted on TxD once, in the order written. When a word waits in the
buffer, it moves into the shift register as soon as the shift register is
empty, so its frame follows the previous frame with no idle cell.

**Stimulus.**
1. Wait 20T with TxD idle, polling INTREQR and logging every read (P17).
2. Write `SERDAT ← 0x0100 | d1` (stop bit at bit 8, 8 data bits; "S= stop bit
   =1", p.27).
3. For k = 2..8: poll INTREQR until bit 0 (TBE) reads 1, then write
   `INTREQ ← 0x0001`, then write `SERDAT ← 0x0100 | dk`, with markers around
   each write. Each word is written as soon as TBE reads 1, and nothing else
   delays it. The move of word k−1 sets TBE ("This sets the Interrupt
   Request TBE", p.27) when frame k−1 has not yet begun, so word k waits in
   the buffer while frame k−1 is sent. Check (d) rests on this stimulus fact;
   no precondition measures it.
4. Wait 30T and stop.

The data bytes d1..d8 are 0x81, 0x3C, 0x5A, 0x00, 0xFF, 0x66, 0x18, 0xA5.

**Observation.** TxD, from 20T before the first write until 30T after the
last write. Frames are decoded with RF-U.

**Expected.** p.27: "This address writes data to a Transmit data buffer.
Data from this buffer is moved into a serial shift register for output
transmission, whenever it is empty." Over the whole interval:
- (a) Before the first write, TxD is constant.
- (b) Exactly eight frames appear. Frame k carries dk: the 8 cells after the
  start cell read dk. Each frame ends with a stop cell at the stop level.
  No frame is missing, repeated or extra.
- (c) Frame 1's start cell begins after the first write marker. The page
  names no latency for the move, so only this order is graded.
- (d) For k = 2..8, frame k's start cell begins no later than T after the
  end of frame k−1's stop cell. That is, there is no idle cell.
- (e) After frame 8's stop cell, TxD stays idle until the end.

**Preconditions.** G-S0, G-C, G-BRK, G-T.

**Contrasting cases.**
- Write word 5 only after 3 frame times of idle. The page predicts no move
  when frame 4 ends, because the buffer is empty, and the move at the
  delayed write instead. Check (d) for k = 5 goes red through its own
  clause; no precondition refers to the time of the writes, so all of them
  still hold.
- Change d3 to 0x24. The decoded frame 3 differs, and check (b) goes red.

---

### P17. Moving the buffer into the shift register sets the TBE request

**Claim.** `sol:AA-serdat.c8`, p.27.

**Proposition.** Every move of the transmit buffer into the shift register
sets INTREQ bit 0 (TBE), and TBE stays clear while a written word still
waits in the buffer.

**Stimulus.** The P16 run, with its poll log of INTREQR, including the reads
of the 20T wait before write 1.

**Observation.** The INTREQR bit-0 reads in the log, placed against the
writes and the frames on TxD. Move k can happen no earlier than the **earliest
instant** e_k and has happened by the start of frame k's start cell:
- e_1 is write 1's marker: before it no word has been written;
- e_k (k ≥ 2) is the later of word k's write marker and the start of frame
  k−1's stop cell. Before the stop cell the shift register still holds data
  cells, so it is not empty and no move can happen.

**Expected.** p.27: "This sets the Interrupt Request TBE (transmit buffer
empty)." p.25: "00 TBE 1 Serial port Transmit Buffer Empty". For every move
k = 1..8:
- every read stored after the clear before word k (S0's for k = 1, the clear
  marker c_k for k ≥ 2) and before e_k reads 0;
- the first read stored after the start of frame k's start cell plus R reads
  1: the word is moved "for output transmission", so its move precedes its
  frame.
- Reads between e_k and the start of frame k's start cell plus R are not
  graded. The page names no latency for the move or for the request, so only
  this order is graded.

**Preconditions.** G-S0, G-C, G-BRK, G-T. G-R with R ≤ T/2 (32 colour
clocks). A bound of T/8 or T/4 leaves the loop no room: "The 68000 uses only the
even-numbered memory access cycles" (HRM p.196, Figure 6-10), so each CPU
access takes two colour clocks, and with `turbo_config = 0` every access,
instruction fetches included, goes over the Chip-memory port
(`nanomig_harness.h`, `Config`). A loop that reads INTREQR, stores the value,
tests bit 0, counts its timeout and branches makes at least eight accesses,
16 colour clocks, between two log stores. No expectation above needs R
small: R enters only as the time between a read and its store.
INTENAR reads 0. The first read after each clear marker reads 0.
*Contrast:* leave out the clear. That read is 1, and the precondition goes
red.

**Contrasting case.** At the start of the 20T wait, write
`SERDAT ← 0x0181` (a frame X, with markers) and go on polling without a
clear. The page predicts that X moves into the empty shift register and sets
TBE, so reads stored before write 1's marker read 1, and the check for k = 1
goes red. X's frame lasts 10T and ends before write 1, so the rest of the run
proceeds as without it, and the first read after each clear marker still
reads 0.

---

### P18. The position of the stop bit sets the length of the data word

**Claim.** `sol:AA-serdat.c10`, p.27. `sol:AA-serdat.c9` is followed as a
stimulus rule.

**Proposition.** Moving the stop bit up by one position lengthens the
transmitted data word by one bit time.

**Stimulus.** For p in the order 8, 9, 7, 4, 8, 9:
1. Wait until TxD has been idle for 12T.
2. Write `SERDAT ← 1 << p` (data bits all zero, stop bit at position p), with
   markers.

The page draws the stop bit at bit 9 ("USE 0 0 0 0 0 0 S D8 D7 … D0") and
states the rule for any position: "The length of the data word is set by the
position of the stop bit." Positions above 9 lie in the bits the diagram
draws as 0, and are not used.

**Observation.** For each frame, w(p): the length of the low interval that
begins at the frame's first falling edge. It spans the start cell and the p
data cells, all low.

**Expected.**
- w(p) − w(8) = (p − 8)·T ± c for p = 9, 7, 4.
- Both repeats of p = 8 and p = 9 agree within c.
- After each low interval, TxD is at the stop level and stays there until the
  next write.

(With the HRM's single start bit, w(p) = (p+1)·T. That absolute value comes
from the HRM and is recorded but not graded.)

**Preconditions.** G-S0, G-C, G-BRK, G-T. TxD is constant for 12T before each
write. *Contrast:* write the next word 2T after the previous frame's stop
cell. The idle precondition goes red.

**Contrasting case.** Write p = 9 where the grader expects p = 8. The page
predicts a word one bit longer. w exceeds w(8) by T, and the check for p = 8
goes red.

---

### P19. SERDATR reads the receive buffer, which is loaded whenever the receive shift register is full

**Claims.** `sol:AA-serdatr.c6`, `sol:AA-serdatr.c7`, both on p.27.

**Proposition.** Each frame received on RxD is loaded into the receive buffer
when the shift register is full, and SERDATR bits 7 to 0 then read its data
byte until the next frame is loaded. This holds for every frame, whether or
not the program has read or acknowledged the previous one.

**Stimulus.** The program writes SERPER (LONG = 0, the 8-bit receive of p.27
"15 LONG Defines Serial Receive as 9 bit word"). It then polls SERDATR in a
tight loop and logs every read, without clearing RBF, for the whole sequence.
The test drives RxD as follows:
1. Idle for 20T.
2. Frames 1 to 8: a start cell, 8 data cells and a stop cell, each exactly T.
   The data bytes e1..e8 are 0x24, 0xC3, 0x7E, 0x99, 0x00, 0xFF, 0x42, 0xBD.
3. Frames 1 to 6 are each followed by 5T of idle before the next frame
   starts. Frames 7 and 8 are back to back: frame 8's start cell directly
   follows frame 7's stop cell.
4. Idle for 20T, then 12T low (for G-RXD), then idle.

**Observation.** SERDATR bits 7 to 0 in the log, placed against the frame
times, which the test knows from its own drive schedule. For frame k:
- λ_k is the start of its last data cell;
- σ_k is the start of the next frame's start cell. For frame 8, σ_8 is the end
  of its stop cell plus 5T.

**Expected.** p.27: "This address reads data from a Receive data buffer. Data
in this buffer is loaded from a receiving shift register whenever it is
full." The table reads "07 DB7 Data bit … 00 DB0 Data bit". For every frame
k:
- every read stored between σ_k + R and λ_{k+1} reads e_k in bits 7 to 0;
  for frame 8 the interval runs until σ_8. For frame 7, whose successor
  follows back to back, the interval starts at σ_7 + T + R instead. The
  interval runs up to λ_{k+1} because the buffer is not loaded before the
  shift register can be full: until then it still holds e_k, not e_{k+1}.
- Reads stored before σ_1 + R, and for k ≥ 2 between λ_k and σ_k + R
  (σ_7 + T + R for k = 7), fall where a load may happen and are not graded.

Why frame 7 starts one cell later: the pages do not fix where in a cell the
receiver samples the pin, only that "N+1 color clocks occur between samples
of the state of the input pin (for receive)" (HRM p.255) and that the
transfer comes "When the proper number of bit times have elapsed" (HRM
p.256). A receiver that samples once per cell can see the start edge up to
one cell late and load at the end of its own stop cell, up to T after σ_7.
Frames 1 to 6 and 8 are followed by 5T of idle, so σ_k already lies well
past that point.

For the back-to-back pair, e7 is read at least once between σ_7 + T + R and
λ_8.

**Preconditions.** G-S0, G-C, G-RXD, and G-R with R ≤ T/4. SERPER is
write-only, so its LONG bit is a stimulus fact and not an observed
precondition. The rate that the test drives, T = 64 colour clocks, is the
rate that G-T establishes on TxD for the same SERPER value in P16.

**Contrasting cases.**
- Leave out frame 4 (idle instead). The page predicts no load, so SERDATR
  keeps e3 through frame 4's slot, and "e4 after σ_4" goes red.
- Change e4 to 0x5A. The check for e4 goes red.

**Left open.** SERDATR bits 9 and 8 are not graded, because the pages
contradict each other about them (section 5.3). OVRUN is not graded, because
no claim of this package states it.

---

### P20. SERDATR also reads the RBF and TBE request bits

**Claim.** `sol:AA-serdatr.c8`, p.27.

**Proposition.** SERDATR bit 14 reads the same as INTREQR bit 11 (RBF)
at all times. SERDATR bit 13 reads the same as INTREQR bit 0 (TBE) in the
two cases on which the pages agree.

**Stimulus, run A (RBF).**
1. The test drives one frame with data 0x5A on RxD. The program polls INTREQR
   until bit 11 reads 1, then waits 5T.
2. **Triple 1:** read SERDATR, then INTREQR, then SERDATR, and log all three.
3. Write `INTREQ ← 0x0800`, then take triple 2.
4. Write `INTREQ ← 0x8800`, then take triple 3. p.24: "Both a set and clear
   operation are required to load arbitary data into this register".
5. Write `INTREQ ← 0x0800`, then take triple 4.

**Stimulus, run B (TBE).**
1. Wait until TxD has been idle for 12T.
2. Write `INTREQ ← 0x0001`, then `SERDAT ← 0x0181`. Poll INTREQR until bit 0
   reads 1, meaning the word has moved, then take a triple. This is case (a).
3. Write `INTREQ ← 0x0001`, then `SERDAT ← 0x01FF`, then take a triple at
   once. This is case (b): the second word waits in the buffer while frame 1
   is sent. The triple follows the SERDAT write directly, a few instructions
   into a frame of 10T; this is a stimulus fact, and no precondition
   measures it.
4. Poll INTREQR until bit 0 reads 1, that is until the second word has moved,
   then take a triple. This is case (a) again.

**Observation.** The logged triples. A triple is **valid** if its two SERDATR
reads agree in the bit concerned.

**Expected.** p.27: "Several interrupt request bits are also read at this
address, along with the data,as shown below." "14 RBF Serial port Receive
Buffer Full(mirror)". "13 TBE Serial port Transmit Buffer Empty(mirror)".
- Run A: in every valid triple, SERDATR bit 14 equals INTREQR bit 11. Triples
  1 and 3 read (1, 1) and triples 2 and 4 read (0, 0).
- Run B: in every valid triple, SERDATR bit 13 equals INTREQR bit 0. The
  step-2 and step-4 triples read (1, 1). The step-3 triple reads (0, 0),
  unless the move of the second word falls between its reads, in which case
  it is not valid and not graded.

**Preconditions.**
- G-S0, G-BRK, G-RXD. INTENAR reads 0.
- In run A, triples 2 and 3 are valid, and INTREQR bit 11 takes both values
  across the four triples, which confirms that the set and clear took effect.

*Contrast:* in run A, leave out the INTREQ writes of steps 3, 4 and 5.
INTREQR bit 11 then reads 1 in all four triples and never 0, and the
precondition goes red. (Changing only step 4 would not do: triples 1 and 2
would still show both values.)

**Contrasting cases.**
- Leave out the software set in run A's step 4. The page predicts that bit 14
  reads 0 at triple 3, so the expectation "(1, 1) at triple 3" goes red.
- In run B, take the step-3 triple after the second word has moved. The page
  predicts (1, 1), so the expectation (0, 0) goes red through its own
  clause; no precondition refers to the time of the triple, so all of them
  still hold.

**Not tested, because the pages disagree** (section 5.2): SERDATR bit 13 while
INTREQ TBE is cleared with the buffer empty, and while INTREQ TBE is set by
software with the buffer full.

---

## 5. What the pages leave open or state inconsistently

### 5.1 The instant of the audio request (P8)

- AA p.11 to 12, and HRM Appendix A p.265 in the same words: "When the DMA
  data is finished (words outputted=Length) and the data in this register has
  been used, an audio channel interrupt request is set."
- HRM p.220: "When the audio DMA is operating in automatic mode, this
  interrupt occurs when the last word in an audio data stream has been
  accessed."
- HRM p.147: "Interrupts occur immediately after the audio DMA channel has
  read the location and length registers and stored their values in the
  back-up registers." HRM p.148 adds a request at DMA start: "c. Create an
  interrupt for the 680x0 saying that it has completed retrieving working
  copies of length and location registers."

All readings put one request per block between the fetch of the block's
last word (the earliest, HRM p.220) and the moment that word has been used,
when the next word's output begins (the AA page, the latest). P8 grades a
request inside that span, from block 2 on, and nothing outside it: no event
at DMA start, and nothing about direct (non-DMA) mode.

### 5.2 SERDATR bit 13, TBE: mirror or not (P20)

- AA p.27: "13 TBE Serial port Transmit Buffer Empty(mirror)". HRM Appendix A
  p.283 says the same: "13 TBE Serial port transmit buffer empty (mirror)."
- HRM p.257: "13 TBE TRANSMIT BUFFER EMPTY (Not a mirror -- interrupt occurs
  when the buffer becomes empty.) When bit 14 is a 1, the data in the output
  data register ( SERDAT ) has been transferred to to accept another output
  word. This is also true when the buffer is empty." The sentence is garbled
  there, and "bit 14" presumably means bit 13.

The readings agree only in two cases: just after a move with the request not
yet cleared (both 1), and with a word waiting and the request cleared (both
0). P20 tests only those.

### 5.3 SERDATR bit 8 (not graded)

- AA p.27: "08 STP-DB8 Stop bit if LONG, Data bit if not." The same page defines
  "15 LONG Defines Serial Receive as 9 bit word". With a 9-bit word, bit 8
  would be the ninth data bit, not a stop bit, so page 27 contradicts itself.
  HRM Appendix A p.283 prints the same line.
- HRM p.257 gives the other assignment: "8 STP Stop bit if 8 data bits are
  specified for receive. OR DB8 9th data bit if 9 bits are specified for
  receive."

No reading agrees on bits 8 and 9 in either mode, so P19 grades only bits 7 to
0.

### 5.4 The volume register's width (no effect on the tests)

- HRM p.139: "There is a six-bit volume register for each audio channel ...
  You can specify values from 64 to 0". HRM p.140: "you write the value into
  bits 5-0". Neither 64 nor 65 levels fit in six bits.
- AA p.11 states seven bits and 65 levels, with bit 6 forcing the maximum.

The pages agree that writing 64 gives the maximum, and P6 and P7 follow AA.

### 5.5 Left open on the AA pages

- **AUDxDAT.** Which byte comes out first (P4). Which output side each channel
  reaches (RF-O). What the output does in direct mode after both bytes, and
  when the request comes in that mode (P5). Whether the first pass of a data
  set outputs every word (section 6).
- **Attach bits.** Whether an attached modulator still sounds. When a
  modulator word takes effect, and for how many samples. The length of a
  sample whose period is changed (P9 to P15). Channel 3's own output under
  bit 3 or bit 7.
- **AUDxLEN.** The meaning of AUDxLEN = 0.
- **UART line.** The start bit, the bit order and the idle level on TxD and
  RxD are taken from the HRM and used only as a reference frame (RF-U). The
  checks use palindromic bytes and differences of lengths, so no expected
  value depends on them.
- **Latencies.** The AA pages name none, so no check adds one. Where the page
  gives only an order, only the order is graded: P16 (c) and P17. P8's
  window ends where the AA page puts the request. P16 (d) uses the cell T of
  the frame to define "no idle cell", and P19 starts its intervals at the
  next frame's start, one cell later after a back-to-back frame, because the
  pages do not fix where in a cell the receiver samples.

### 5.6 Wording checked against the print

Several quoted lines of AA p.11 and p.27 rest on a single reading of the
merged text layer. Every line quoted in this document from p.11, p.12 and
p.27 was compared with the scanned page images, and each reads as quoted:
- the AUDxLEN, AUDxPER, AUDxVOL and AUDxDAT entries;
- ADKCON bits 07 to 00 and the NOTE;
- "is set." at the top of p.12;
- the SERDAT and SERDATR entries, including "08 STP-DB8 Stop bit if LONG,
  Data bit if not." and "13 TBE Serial port Transmit Buffer Empty(mirror)".

The contradiction in 5.3 is therefore printed on the page, not an artefact of
reading it. HRM Appendix A carries the same wording on p.265 (AUDxDAT and the
ADKCON attach bits), p.266 (AUDxLEN and AUDxVOL, the latter printed as
"(111111=63 1s, one 0)") and p.283 (SERDAT, SERDATR and SERPER).

---

## 6. Claim index

| Claim | Page | Proposition |
|---|---|---|
| `sol:AA-audxlen.c1` | 11 | P1 |
| `sol:AA-audxlen.c4` | 11 | P1 |
| `sol:AA-audxdat.c5` | 11 | P2 |
| `sol:AA-audxdat.c9` | 11 | P2 |
| `sol:AA-audxdat.c6` | 11 | P3 |
| `sol:AA-audxdat.c7` | 11 | P4 |
| `sol:AA-audxdat.c1` | 11 | P5 |
| `sol:AA-audxdat.c10` | 11 | P5 |
| `sol:AA-audxvol.c1` | 11 | P6 |
| `sol:AA-audxvol.c4` | 11 | P6 |
| `sol:AA-audxvol.c7` | 11 | P6 |
| `sol:AA-audxvol.c6` | 11 | P7 |
| `sol:AA-audxdat.c11` | 11 (ends on 12) | P8 |
| `sol:AA-adkcon.c30` | 11 | P9 |
| `sol:AA-adkcon.c29` | 11 | P10 |
| `sol:AA-adkcon.c28` | 11 | P11 |
| `sol:AA-adkcon.c26` | 11 | P12 |
| `sol:AA-adkcon.c25` | 11 | P13 |
| `sol:AA-adkcon.c24` | 11 | P14 |
| `sol:AA-adkcon.c27` | 11 | P15 |
| `sol:AA-serdat.c6` | 27 | P16 |
| `sol:AA-serdat.c7` | 27 | P16 |
| `sol:AA-serdat.c8` | 27 | P17 |
| `sol:AA-serdat.c10` | 27 | P18 |
| `sol:AA-serdat.c9` | 27 | none (section 1; stimulus rule in P16 to P20) |
| `sol:AA-serdatr.c6` | 27 | P19 |
| `sol:AA-serdatr.c7` | 27 | P19 |
| `sol:AA-serdatr.c8` | 27 | P20 |
| `sol:AA-audxdat.c8` | 11 | none (section 1) |
| `sol:AA-audxvol.c5` | 11 | none (section 1) |
| `sol:AA-audxlen.c2` | 11 | none (section 1) |
| `sol:AA-audxvol.c2` | 11 | none (section 1) |
| `sol:AA-audxdat.c2` | 11 | none (section 1) |

**The first pass of a data set is not graded here.** The pages disagree on
whether it outputs every word. HRM p.145 step 4 sets AUDxLEN to "the number
of data words to be retrieved before starting at the address currently in
AUDxLC", which makes every pass L words long. HRM p.164 says "Because of
pipelining in Agnus, the first data word must be thrown away", and the
length counter "counts down once with each word that comes in", which makes
the first pass one word short in its output. The AA page says neither, and
the package takes no side. Both readings agree on the fetches, which P1
grades from the first fetch on (the thrown-away word is still fetched), and
on every pass after the first. So every check that pairs output with
fetches starts at pass 2 (RF-W): P2 (a) to (c) grade pass 2 onward, P3 and
P4 passes 2 and 3. P8 grades blocks 2 to 6 and finds word L's run by its level,
calibrated on a one-word data set that looks the same under both readings.
P5 to P7 and P9 to P15 do not pair output with particular fetches.
