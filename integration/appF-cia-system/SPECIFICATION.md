# appF-cia-system: specification

This is the specification of the integration test package `appF-cia-system`. It was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. It covers three things on the two 8520 CIAs: the timer outputs on port B (PB on/off, toggle and pulse), the bit layout of the control registers CRA and CRB, and the φ2 clock input. Page numbers are printed page numbers. "φ2" is the clock the manual prints as "02". A builder implements the package from this document and may not change what it specifies.

---

## 0. Claims that give no proposition, or only part of one

| Claim | Page | What cannot be used, and why |
|---|---|---|
| opus:F-portsig.c47 | 351 | "PC...not used" (CIAB). None of the outputs a test can see shows a PC pin on either CIA, and "not used" describes no behaviour and gives no value. (The text layer puts the FLAG gloss "disk index*" on the PC line. The printed page puts it on "F....INDEX*", and the claim reads it correctly.) |
| opus:F-signals.c1, sol:F-signals.c69: the words "TTL compatible input" | 353 | This is an electrical property of an input pin, and nothing a test sees depends on it. The rest of both claims is used in P5 and P6. |
| sol:F-signals.c69, sol:F-signals.c70: ".715909 Mhz for NTSC or .709379 Mhz for PAL" (in their quotes) | 353 | The test measures time in system clock cycles. Absolute frequencies come from the harness's clock, not from the design. Only the stated ratio, "1/10 of the CPU clock", is checked (P5). |
| opus:F-timers.c6, sol:F-timers.c8: "allow generation of long time delays, variable width pulses, pulse trains, and variable frequency waveforms" | 341 | This sentence states what the modes make possible and names no value. It yields a proposition only through the sentence quoted with it in sol:F-timers.c8, that the latch "is a divider of the input clocking frequency" (P9). The modes it refers to are specified elsewhere: pulse trains in P4, variable width and variable frequency in P9, linked timers (long delays) in P8.65. |
| opus:F-cra.c12, opus:F-crb.c13: the reading that LOAD has a "0 setting" of force load | 349, 350 | The bitmaps print one entry for LOAD, "1=force load (strobe)", split over two lines. LOAD has no 0 setting: "writing a 0 has no effect" (p348, p349). The rest of both claims is used (P1, P3, P4, P7, P8). |

No claim in this package needs data read from or written to a disk surface.

---

## 1. Common setup

### 1.1 Registers, addresses, wiring

> "With future expansion in mind, we have decided on the following addresses: CIAA = BFEr01; CIAB = BFDr00. Software must use byte accesses to these address, and no other." (p353)

`r` is the register number of the Chip Register Map (p340). Every CIA access in every program of this package is a byte access to exactly these addresses.

| r | Name | CIAA | CIAB | Meaning (p340, p341, p343) |
|---|---|---|---|---|
| 1 | PRB | $BFE101 | $BFD100 | Port B data; a read returns the pins |
| 3 | DDRB | $BFE301 | $BFD300 | Port B direction, 1 = output |
| 4 | TALO | $BFE401 | $BFD400 | Timer A: read = counter, write = latch |
| 5 | TAHI | $BFE501 | $BFD500 | |
| 6 | TBLO | $BFE601 | $BFD600 | Timer B |
| 7 | TBHI | $BFE701 | $BFD700 | |
| 8 | TODLO | $BFE801 | $BFD800 | Event LSB |
| 9 | TODMID | $BFE901 | $BFD900 | Event 8-15 |
| A | TODHI | $BFEA01 | $BFDA00 | Event MSB |
| C | SDR | $BFEC01 | $BFDC00 | Serial data register |
| D | ICR | $BFED01 | $BFDD00 | Interrupt control |
| E | CRA | $BFEE01 | $BFDE00 | Control register A |
| F | CRB | $BFEF01 | $BFDF00 | Control register B |

Wiring (p351, printed image; the same on p337):
- CIAA: PB7..PB0 are Centronics data 7..0. CNT is KCLK and SP is KDAT ("keyboard data").
- CIAB: PB7 is MTR*, PB6 SEL3*, PB5 SEL2*, PB4 SEL1*, PB3 SEL0* ("select internal drive"), PB2 SIDE*, PB1 DIR, PB0 STEP*. CNT is POUT and SP is BUSY.
- CIAB TOD counts "Horizontal sync event[s]" (p340). CIAA TOD is a "50/60 Hz event counter ... (VSync or line tick)" (p339).

Control-register values used below (p348-p350):

| Bit | CRA | CRB |
|---|---|---|
| 0 ($01) | START: 1 start, 0 stop | START |
| 1 ($02) | PBON: 1 timer A output on PB6 | PBON: 1 timer B output on PB7 |
| 2 ($04) | OUTMODE: 1 toggle, 0 pulse | OUTMODE |
| 3 ($08) | RUNMODE: 1 one-shot, 0 continuous | RUNMODE |
| 4 ($10) | LOAD: 1 force load (strobe) | LOAD |
| 5 ($20) | INMODE: 1 CNT, 0 φ2 | INMODE low bit (CRB5) |
| 6 ($40) | SPMODE: 1 output, 0 input | INMODE high bit (CRB6) |
| 7 ($80) | unused | ALARM: 1 TOD writes set the alarm |

CRB INMODE (CRB6 CRB5): 00 counts φ2 pulses, 01 positive CNT transitions, 10 timer A underflow pulses, 11 timer A underflow pulses while CNT is held high (p349, p350). Below, "CR" means CRA for timer A and CRB for timer B. Every CRB value used for a timer-B case outside P8.65 and P8.7 has bits 7..5 = 000.

### 1.2 Program environment (every run)

- **E1.** The 68000 enters the test program directly from reset, with no other code executed first. The program sits in Chip memory.
- **E2.** The program's first instructions set SR to $2700 and write $7FFF to DMACON ($DFF096) and to INTENA ($DFF09A). Bit 15 = 0 clears (bit 15 is SET/CLR in both registers, p278 and p280). p352 makes the same DMACON write before its CIA timing example.
- **E3.** For each CIA, the program writes ICR ← $7F and then reads ICR once, discarding the value. The write clears all mask bits: "If bit 7 is a 0, then any bit 6-0 in your own mask byte which is set to a 1 clears the corresponding bit in the MASK register" (p347). The read clears the flags: "When you read the DATA register, its contents are cleared" (p346).
- **E4.** The program never writes CIAA PRA or DDRA, because PA0 is the "memory overlay bit" and PA1 the power LED (p351). It never writes CIAB PRA or DDRA.
- **E5. Markers.** A marker is a word store of a distinct constant to a marker area in Chip memory. The test sees it at the Chip-memory port as a write with a time. A *bracketed* CIA write is a marker, then the CIA write, then a marker, with no other instruction between them.
- **E6. Results.** Every value the program reads for grading goes into a data register and is then stored as a word to a result area in Chip memory. Each store is visible at the Chip-memory port with its time.
- **E7. Parameters.** Anything that differs between runs of the same program (latch values, delay counts, CR values, key timing) is read from a parameter block in Chip memory. Runs that are compared with each other must execute identical code.
- **E8.** The program ends with a final marker and then branches to itself.

**Environment preconditions.** Every run asserts all three.

- **EP1 (reset state).** The program's first reads of CRA and CRB on both CIAs, made before any write to them, return $00. Source: "The timer control registers are set to zero" (p354). *Contrast:* a variant that writes CRA ← $0A before the read gets $0A back.
- **EP2 (DMA quiet).** Between the first and the last marker, the Chip-memory port shows only CPU accesses and memory-refresh accesses. *Contrast:* start Copper DMA in the order p30 prescribes: write COP1LC, strobe COPJMP1, then write DMACON ← $8280 (SET, DMAEN, COPEN). Copper fetches then appear.
- **EP3 (completion).** The final marker appears. A run without it has failed; it has no graded result.

### 1.3 Observation channels

- **O1. PRB read-back.** "When you READ a PR register, you read the actual current state of the I/O pins (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs or outputs." (p341). For CIAA PB6 and PB7, this is the only channel: no output the test sees shows the Centronics lines.
- **O2. Drive LED (CIAB PB7, MTR*, only).** From p244 (checked against the scan):

  > "Disk motor control (active low). [...] Each drive will latch the motor signal at the time its select signal turns on. The disk drive motor will stay in this state until the next time select turns on. DSKMOTOR* also controls the activity light on the front of the disk drive." [...] "All drive motors turn off after system reset."

  *Routine R-SELECT.* CIAB DDRB bit 3 = 1. Write PRB with bit 3 = 1, then with bit 3 = 0 (select turns on and the drive latches MTR*), then with bit 3 = 1. Each write keeps the other PRB bits the case needs.

  *Expected channel behaviour:* if MTR* is 0 when select turns on, the LED is on; if it is 1, the LED is off.

  *Observation interval:* from the marker after the bit-3 = 0 write to the marker before the next R-SELECT. The page gives no settle time, so the graded LED level is the one at the end of this interval. The builder makes the interval at least 2000 E periods long.

  *Calibration precondition LP.* This applies to every run that grades O2, and must hold before any O2 leg is graded:
  1. At program entry the LED is off (p244).
  2. With CIAB CRB = $00 and DDRB = $88, write PRB bit 7 = 0 and run R-SELECT: the LED turns on.
  3. Write PRB bit 7 = 1 and run R-SELECT: the LED turns off.

  If LP fails, every O2 leg is ungraded and reported; the O1 legs of the same run are still graded. *Contrast:* change bit 7 without running R-SELECT, and the LED keeps its previous state ("stay in this state until the next time select turns on").
- **O3. Chip-memory port.** Marker and result stores with their times (E5, E6), and the refresh accesses (F4).
- **O4. ICR flags.** Read ICR bits: D0 TA, D1 TB, D2 ALRM, D3 SP, D4 FLG, D7 IR (p347). "Any interrupt will set the corresponding bit in the DATA register." (p346), whatever the MASK holds. The sources include "Underflow from Timer A (timer counts down past 0)" and "Underflow from Timer B" (p346). A read clears all flags (p346), so each ICR read reports the events since the previous read.

### 1.4 Reference frames

- **F1. CPU clock.** T_CPU is the CPU clock period in system clock cycles, a constant of the harness that the builder states.
  - The CPU clock is 7.09379 MHz on PAL and 7.15909 MHz on NTSC (p2).
  - A memory cycle is "approximately 280 ns" (p194), which is two CPU clock periods.
  - *Anchor check (every run):* accesses the harness reports in consecutive memory slots are 2·T_CPU apart. *Contrast:* the same check evaluated with 1.25·T_CPU fails.
- **F2. E period and E-index.** T_E = 10·T_CPU, from "The "E" clock runs at 1/10 of the CPU clock." (p353), which p352 repeats ("count down at 1/10 the CPU clock").
  - The E-index of a store at time t, taken relative to an earlier store t0 in the same loop, is e = round((t − t0) / T_E).
  - An E-index is defined only in a run that passes P6's phase check for that loop. If the check fails, every grade that needs an E-index is not given for that run, and P6 is red.
- **F3. Underflow period P(L).** This is for a timer running continuously on φ2 with latch L, where every R-SAMPLE iteration includes an ICR read.
  - Let f_j be the E-indices of the ICR stores and q_j their TA or TB flag. P(L) is the integer P for which one phase φ exists such that, for every j ≥ 1, q_j = 1 exactly when φ + kP lies in (f_(j−1), f_j] for some integer k.
  - It requires an iteration spacing of at most P/2 E periods and at least 16 flags, so that exactly one P fits.
  - The pages leave open whether P(L) is L or L+1 (Section 3, item 1).
- **F4. Lines.** Each line has "4 cycles for memory refresh" (p194).
  - The first refresh access of each group of four marks a line. The number of lines between two events is the number of marks between their times.
  - *Anchor:* consecutive marks are 227 or 228 slots apart, averaging between those values ("227.5 "color clocks", or memory access cycles" per line, p194). *Contrast:* taking groups of three misaligns the marks and the anchor fails.

### 1.5 Routines

- **R-LATCH(T, L).** While the timer's CR has START = 0 and RUNMODE = 0, write the timer's LO register ← L & $FF, then its HI register ← L >> 8.
  - The order comes from p352: "Set time (low byte THEN high byte)".
  - Afterwards the counter holds L: "The timer latch is loaded into the timer on any timer underflow, on a force load, or following a write to the high byte of the pre- scalar while the timer is stopped" (p343).
  - RUNMODE must be 0 during the write because of p342: "In one-shot mode, a write to timer-high [...] will transfer the timer latch to the counter and initiate counting regardless of the start bit". Timers in this package are started only by writing START = 1 (P2 depends on what counts as a start).
- **R-SAMPLE(n; X [, Y]).** Run n iterations of: read register X and store it (E6); optionally, read register Y and store it. The loop body is fixed code with no data-dependent branch.
- **R-SELECT.** See O2.
- **R-KEY.** The harness's keyboard sends one key code, $35 ("B" pressed, p358), starting d_k ≥ 100 E periods after the test sees the program's "ready" marker.
  - Per p357 and p358, the eight bits are each "followed by a pulse on KCLK (low then high)". KCLK stays low "about 20 microseconds", at "about 60 microseconds per bit". Before the transmission "both KCLK and KDAT are high".
  - The test records the start and end of each of the 8 KCLK low intervals it drives. Their lengths in E periods are ℓ_1..ℓ_8.
  - No handshake is given. Every case that uses R-KEY ends within 143 ms of the last clock, since the keyboard resyncs only after "143 ms" without a handshake (p359). The harness sends no other key in the run.
  - **KP (keyboard idle).** In the same program without R-KEY, a CIAA timer B running with CRB INMODE 01 (CRB ← $21) is unchanged over a window of the same length. *Contrast:* with R-KEY inside that window, it changes by 8.

### 1.6 Runs

- **CPU.** Every run is made on the 68000 core, and on the second core in its default 68000 behaviour. No claim concerns the 68010 or 68020 settings, and p353 speaks of the "680x0 "E" clock" without saying how another CPU derives it, so those settings are not run.
- **Video standard.** PAL. P5 is also run on NTSC.
- **Chip set and memory.** ECS (the default), 2 MB of Chip memory. No claim distinguishes OCS from ECS.
- **Separate runs.** Each leg of a proposition is its own run from reset unless its stimulus says otherwise.

---

## 2. Propositions

### P1: PB on/off puts the timer output on PB6 or PB7, forcing an output over DDRB and the PR data

**Proposition.** With PBON = 1 (CRA bit 1 for timer A, CRB bit 1 for timer B), the timer's output appears on PB6 (timer A) or PB7 (timer B) of the same CIA, and the line is an output whatever DDRB and the PR data bit say. PBON affects only that one line. With PBON = 0 the line is back in normal operation.

**Claims.**
- p342: opus:F-timers.c10, opus:F-timers.c11, sol:F-timers.c12.
- p341: opus:F-ports.c7, sol:F-ports.c7.
- p349, CRA bitmap PBON column: sol:F-cra.c17, opus:F-cra.c12, opus:F-cra.c13.
- p349, CRB text: sol:F-crb.c3, opus:F-crb.c2.
- p350, CRB bitmap: sol:F-crb.c14, opus:F-crb.c13, opus:F-crb.c14.

**Instances.** There are four: (CIAA, timer A, n = 6), (CIAA, timer B, n = 7), (CIAB, timer A, n = 6), (CIAB, timer B, n = 7). In what follows, m = 1 << n, and o is the other one of bits 6 and 7.

**Other-bit pattern.** For bits 0-5, DDRB = 1 and the PR data are:
- CIAA: pattern Q1 = %101010 and pattern Q2 = %010101.
- CIAB: pattern Q1 = %111101 (DIR = 0, SIDE* = 1) and pattern Q2 = %111011 (DIR = 1, SIDE* = 0). SEL0*-SEL3* and STEP* always stay 1, except when R-SELECT drives SEL0*.

Bit o has DDRB = 0 throughout (it is an input). On CIAB, R-SELECT needs DDRB bit 3 = 1, which the pattern already provides.

**Stimulus.** One run per instance, in this order. On CIAB, the value written as "DDRB ← $00" in steps 1 and 5 is $08 instead, after a first write of PRB ← $FF. This drives SEL0* high and keeps bit 3 an output for R-SELECT, while bit n stays an input.
1. Environment (1.2). DDRB ← $00. CR ← $00.
2. **Pull-up reference.** Read PRB → r0.
3. **Leg A2: pulse output, stopped.** CR ← $02 (PBON, pulse, stopped). Read PRB → a2.
4. **Make the toggle output low by exactly one underflow.** CR ← $00. R-LATCH(L = $0010). CR ← $0F (START, PBON, toggle, one-shot). Poll CR until bit 0 reads 0, at most 1000 polls. Read ICR → i1.
5. **Leg A1: DDRB override, line an input.** With DDRB = $00 and CR as the one-shot left it, read PRB → a1.
6. **Leg B: PR-data override.** DDRB ← m | pattern bits. PRB ← m | Q1 (data bit n = 1). Read PRB → b1.
7. **Leg C: timer output high.** PRB ← Q2 (data bit n = 0). CR ← $00. R-LATCH($FFFF). CR ← $07 (START, PBON, toggle, continuous). Within 1000 E periods, read PRB → c1, then read ICR → i2.
8. **Leg D: normal operation.** CR ← $00. Read PRB → d1 (DDRB bit n = 1, data 0). DDRB ← pattern bits without m. Read PRB → d2.
9. **O2 legs (CIAB, timer B only).** First LP, at the start of the run. Then run R-SELECT at the end of step 5 (graded as LED-A1), at the end of step 7 (LED-C), after d1 (LED-D1) and after d2 (LED-D2). In step 5, bit 7 is an input (DDRB = $08), overridden by PBON.

**Observation.** The result stores of r0, a1, a2, b1, c1, d1, d2, i1 and i2 (O1, O4), and the LED level at the end of each O2 interval.

**Expected.**
- **a2 bit n = 0.** In pulse mode the output is "a single positive pulse of one cycle duration" on an underflow (p342), and with the timer stopped there is no underflow. A positive pulse rises from low (see Section 3, item 4). PBON "overrides the DDRB control bit and forces the appropriate PB line to become an output" (p342), so the line reads 0 although DDRB says input and r0 shows the pull-up.
- **a1 bit n = 0.** "The toggle output is set high whenever the timer is started" and "On every timer underflow, the output can either toggle" (p342). One start and one underflow leave it low.
- **b1 bit n = 0**, although DDRB bit n = 1 and the data bit is 1.
- **c1 bit n = 1**, although the data bit is 0 (set high on start, no underflow).
- **d1 bit n = 0** (the data bit) **and d2 bit n = 1** (an input with a pull-up): "0 = PB6 is normal operation" (p348), "0 = PB7 is normal operation" (p349).
- **Other bits.** In a1, a2, b1 and c1, every bit other than n equals what the same DDRB and PR contents give with PBON = 0: the pattern bits, and 1 for the input bit o. The override affects "the appropriate PB line" only.
- **LED (CIAB, timer B).** LED-A1 on, LED-C off, LED-D1 on, LED-D2 off.
- **Quantifier.** The claims name PB6 for timer A and PB7 for timer B generally, so all four instances must pass.

**Preconditions.**
- **Pre1.1.** r0 bit n = 1. The undriven line reads high: "Ports A and B have passive pull-up devices" (p341); "a read of the ports will return all highs because of passive pull-ups" (p354). Without this, legs A1 and A2 cannot tell a driven 0 from an undriven one. If r0 bit n = 0, legs A1 and A2 are not graded and the value is reported; legs B, C and D still are. *Contrast:* set DDRB bit n = 1 and the data bit to 0 before the read, and r0 bit n = 0.
- **Pre1.2.** Exactly one underflow happens before leg A1: CR bit 0 reads 0 within the poll limit, and i1 has TA (timer A) or TB (timer B) set. *Contrast:* with L = $FFFF in step 4, the poll limit runs out with bit 0 still 1 and the i1 flag at 0.
- **Pre1.3.** No underflow happens during leg C: the i2 flag is 0. *Contrast:* with L = $0004 in step 7, the flag is 1, and c1 is then not graded.
- **LP** for the O2 legs (O2).
- **EP1-EP3**, and the F1 anchor.

**Contrasting case.** Repeat each leg with PBON = 0: CR ← $00 in step 3, CR ← $0D in step 4 (so the step-5 state has PBON = 0), CR ← $05 in step 7. The page then predicts the opposite in every leg:
- a1 and a2 bit n = 1 (input, pull-up);
- b1 bit n = 1 (the data);
- c1 bit n = 0 (the data);
- LED-A1 off (MTR* pulled up);
- LED-C on (in leg C, DDRB bit 7 = 1 and the data bit 7 = 0, so MTR* is low).

**Reference frame.** None beyond F1. E-indices are not needed.

**Left open.** DDRB's own read-back while PBON = 1 is not stated; it is read and reported, not graded.

---

### P2: The toggle output is low after RES and set high whenever the timer is started

**Proposition.** Before any start after reset, the toggle output is low. Every start of the timer sets it high, whether it was low or high before.

**Claims.** opus:F-timers.c14, sol:F-timers.c13 (p342).

**Instances.** The four of P1.

**Stimulus.** One run per instance.
- **Leg R** (the first actions on this CIA's timer after reset). DDRB ← $00; on CIAB, DDRB ← $08 for R-SELECT, with PRB ← $FF. CR ← $06 (PBON, toggle, not started). Read PRB → r1. On CIAB timer B, run R-SELECT and read the LED (LED-R).
- **Leg S** (start from low, four times). CR ← $00. R-LATCH($0020). For k = 1..4:
  1. Bracketed write CR ← $0F (one-shot, toggle, PBON, START).
  2. R-SAMPLE(64; PRB) → s_k[0..63].
  3. Poll CR until bit 0 = 0 (at most 1000 polls).
  4. Read PRB → z_k. Read ICR → i_k.

  After the one-shot the counter again holds $0020: "reload the latched value, then stop" (p342).
- **Leg H** (start from high). CR ← $06. R-LATCH($FFFF). CR ← $07; read PRB → h1. CR ← $06 (stop); read PRB → h2. CR ← $07 (start again); read PRB → h3. Read ICR → ih. All within 2000 E periods.

**Observation.** The O1 values. The E-indices (F2) of the s_k samples relative to the marker after the start write. The LED (O2).

**Expected.**
- **Leg R.** r1 bit n = 0: "set low by RES" (p342), with no start since reset. LED-R is on (MTR* low at the select).
- **Leg S, for every k.**
  - The level before the start is 0: r1 for k = 1, z_(k−1) otherwise.
  - Every sample of s_k whose E-index lies in [L/4, 3L/4] = [8, 24] has bit n = 1: set high on start ("whenever the timer is started", p342). This window lies well inside the countdown, so no latency at either end can matter.
  - Every sample with E-index ≥ 5L/4 = 40 has bit n = 0. The single one-shot underflow toggled the output once, and a one-shot does not underflow again.
  - z_k bit n = 0.
- **Leg H.** h1 = 1 and h3 = 1 (bit n). h2 is reported, not graded (see Left open). h3 separates "set high" from "toggle" only when h2 = 1: a design that toggles on start would then give h3 = 0. If h2 = 0, h3 = 1 is still required, and the report says the leg did not separate the two.
- **Quantifier.** "Whenever" means all four starts of leg S, plus leg H.

**Preconditions.**
- **Pre2.1 (leg R).** E1 holds, and EP1 read CR = $00. The program's first write to this CR is the one in leg R. If the harness runs any code before the test program, leg R cannot be graded, and that is reported. *Contrast:* insert R-LATCH($FFFF) and CR ← $07 before leg R's write. The page then predicts r1 = 1 (started, no underflow) and LED-R off.
- **Pre2.2 (leg S).** Before each restart, the previous one-shot has ended: CR bit 0 read 0 within the poll limit and i_k has the flag. *Contrast:* with R-LATCH($FFFF), the poll limit runs out with bit 0 still 1.
- **Pre2.3.** The E-index frame is valid for the R-SAMPLE loop (P6's phase check). *Contrast:* see P6.
- **Pre2.4 (leg H).** The ih flag is 0 (no underflow in leg H). *Contrast:* with R-LATCH($0008), the flag is 1.

**Contrasting case.** In leg S, replace the start write with CR ← $0E (START = 0). The page predicts no start and no underflow, so every sample of s_k reads 0, including the [8, 24] window. For leg R, see Pre2.1's contrast.

**Reference frame.** F2. Leg S counts E-indices from the store of the marker right after the bracketed start write.

**Left open.** These are not graded:
- What stopping the timer does to the toggle output.
- Whether a START = 1 write to a timer that is already running is a "start".
- Whether the one-shot start caused by a timer-high write (p342) is a "start".

---

### P3: In toggle mode the output inverts on every underflow, and at no other time

**Proposition.** With OUTMODE = 1 (bit 2) and PBON = 1, the PB line changes level once at each underflow and never otherwise.

**Claims.**
- p342: opus:F-timers.c12, opus:F-timers.c13, sol:F-timers.c13.
- p349: OUTMODE "1=toggle" in sol:F-cra.c16, opus:F-cra.c12, opus:F-cra.c13, opus:F-crb.c3, sol:F-crb.c4.
- p350: sol:F-crb.c13, opus:F-crb.c13, opus:F-crb.c14.

**Instances.** The four of P1.

**Stimulus.** DDRB ← $00. CR ← $00. R-LATCH(L = 60). Read ICR (discard). CR ← $07 (START, PBON, toggle, continuous). R-SAMPLE(600; PRB, ICR) → pairs (p_i, q_i). CR ← $00.

**Observation.** Bit n of p_i. The TA or TB flag of q_i.

**Expected.** Source: "On every timer underflow, the output can either toggle or generate a single positive pulse" (p342), with bit 2 = 1 selecting toggle (p348-p350).
- Let T be the set of i with p_i bit n ≠ p_(i+1) bit n (a transition), and Q the set of j with q_j flag = 1 (an underflow).
- There is a one-to-one pairing of T and Q in which paired i and j satisfy |i − j| ≤ 1 (within one loop iteration). Every underflow has its transition, and every transition has its underflow.
- The level before the first flag is 1 (set high on start, P2).

**Preconditions.**
- **Pre3.1.** Underflows are separated: consecutive flags are at least 3 iterations apart, so no ICR read merges two underflows and both levels get sampled. *Contrast:* with L = 2, flags appear in consecutive reads and the precondition fails.
- **Pre3.2.** |Q| ≥ 16, so "every" is tested on many underflows. *Contrast:* CR ← $06 (not started) gives |Q| = 0.

**Contrasting case.** Two changes:
- OUTMODE = 0 (CR ← $03) with the same latch. The page predicts a one-cycle pulse per underflow instead of a level change. Samples are 0 except for at most one sample near a flag, so transitions come in adjacent pairs or not at all, and the pairing fails.
- PBON = 0 (CR ← $05). The line is an input and reads 1 throughout, while flags continue: the pairing fails.

**Reference frame.** Loop iteration indices only.

---

### P4: In pulse mode the output is low except for one φ2 cycle at every underflow

**Proposition.** With OUTMODE = 0 and PBON = 1, the PB line is low, except that at every underflow it goes high for exactly one φ2 cycle.

**Claims.** The OUTMODE "0=pulse" readings in the P3 claim list, together with opus:F-timers.c12, opus:F-timers.c13 and sol:F-timers.c13 (p342).

**Instances.** The four of P1.

**Stimulus.** For each L in {20, 21, 22, 23, 24, 25} (one run each): DDRB ← $00. CR ← $00. R-LATCH(L). Read ICR (discard). CR ← $03 (START, PBON, pulse, continuous). R-SAMPLE(400; PRB, ICR). CR ← $00.

**Observation.** Bit n of each PRB sample at its E-index e_i (F2, computed from the PRB stores alone). P(L) from the ICR stores (F3).

**Expected.**
- **The period.** P(L) is L or L+1 (F3; Section 3, item 1). The offset P(L) − L is the same for every L (P9).
- **The pulse.** There is one residue c such that:
  - every PRB sample with e_i ≡ c (mod P(L)) has bit n = 1;
  - every other PRB sample has bit n = 0;
  - at least 3 samples have bit n = 1.
- **Grading of the quantifiers** in "On every timer underflow, ... a single positive pulse of one cycle duration" (p342):
  - "every underflow": every sample in residue class c is high;
  - "single ... one cycle": no other residue is ever high, including c − 1 and c + 1.

**Preconditions.**
- **Pre4.1.** The E-index frame is valid for the PRB stores (P6's phase check). *Contrast:* see P6.
- **Pre4.2 (coverage).** Every residue mod P(L) is hit by at least 2 PRB samples. A run that fails coverage is not graded and is reported. At least 2 of the 6 latch values must be graded (consecutive latches make it likely that the loop's E-spacing is coprime to some P). *Contrast:* a latch whose P(L) shares a factor with the loop's measured E-spacing (the builder picks it from a run) leaves residues uncovered, and the precondition fails.
- **Pre4.3.** P(L) is uniquely determined (F3: iteration spacing ≤ P/2, at least 16 flags). *Contrast:* with L = 3, the flags merge and no unique P fits.

**Contrasting case.**
- OUTMODE = 1 (CR ← $07) with the same L: about half of all residues read high, and the single-residue check fails.
- START = 0 (CR ← $02) with coverage from a run with START = 1: no sample is high, and the "every underflow" check fails.

**Reference frame.** F2 (E-index) and F3 (P(L)). "One cycle" is taken as one φ2 cycle, the clock "used for internal device operation" (p353); see Section 3, item 2.

**Left open.** These are not graded:
- The E-offset between the underflow (the ICR flag) and the pulse.
- The pulse width when the timer counts CNT or timer A underflows.

---

### P5: φ2 is the E clock, one tenth of the CPU clock

**Proposition.** A timer counting φ2 pulses (CRA INMODE = 0, CRB INMODE = 00) decrements once per E cycle, that is once per 10 CPU clock periods, on PAL and on NTSC.

**Claims.**
- p353: opus:F-signals.c1, sol:F-signals.c69, opus:F-signals.c2, sol:F-signals.c70.
- The φ2 input-mode settings: sol:F-cra.c13 and opus:F-cra.c12 (p349); sol:F-crb.c7 (p349); sol:F-crb.c10 and opus:F-crb.c13 (p350).

**Instances.** Four timers (CIAA A and B, CIAB A and B) × {PAL, NTSC}.

**Stimulus.**
- **Leg (a), differential.** CR ← $00. R-LATCH($FFFF). Bracketed write CR ← $01 (START, φ2, continuous). A delay loop of D iterations. Bracketed write CR ← $00. Read HI, read LO → V. Read ICR → i.
  - There are two runs, D = D1 and D = D2. The builder chooses them so that τ1 ≈ 50 000·T_CPU and τ2 ≈ 550 000·T_CPU, where τ is the time from the marker after the start write to the marker after the stop write.
- **Leg (b), per sample.** CR ← $00. R-LATCH($FFFF). CR ← $01. R-SAMPLE(256; LO register) → v_i. CR ← $00.

**Observation.** Δ = $FFFF − V and τ from the markers (O3, F1). For leg (b), the low bytes v_i at their E-indices e_i.

**Expected.** Source: "On the Amiga, this is connected to the 680x0 "E" clock. The "E" clock runs at 1/10 of the CPU clock." (p353), and "0 = Timer A counts 02 pulses" (p348) and "Timer B counts 02 pulses" (p349).
- **(a)** Δ2 − Δ1 = round((τ2 − τ1) / (10·T_CPU)) ± 1. The code between the markers is identical in both runs, so start and stop latencies cancel.
- **(b)** For every i, (v_i − v_(i+1)) mod 256 = e_(i+1) − e_i.

**Preconditions.**
- **Pre5.1.** There is no underflow in the window: the i flag is 0. *Contrast:* with R-LATCH($1000) and D2, the flag is 1.
- **Pre5.2.** The counter is read while stopped: a second read of HI/LO gives V again. *Contrast:* leave out the stop write, and the two reads differ.
- **Pre5.3 (leg b).** The E-index frame is valid for the LO-read loop (P6). *Contrast:* see P6.
- **EP2 and the F1 anchor.**

**Contrasting case.** Count CNT instead of φ2 (CRA ← $21, CRB ← $21) with no key sent. The page predicts Δ1 = Δ2 = 0 and v_i constant: the timer "counts positive CNT transitions" (p348, p349), and there are none. CIAA's CNT is KCLK, high while idle (p357). CIAB's CNT is POUT (p351), which nothing drives. *Precondition of the contrast:* KP (1.5).

**Reference frame.** F1 and F2.

**Left open.** Absolute frequencies (Section 0).

---

### P6: φ2 is the timing reference for bus transfers to a CIA

**Proposition.** CPU transfers to and from a CIA happen in step with φ2 (E). In a loop whose only element not paced by Chip-memory cycles is a CIA access, every iteration starts at the same phase of the E cycle.

**Claims.** opus:F-signals.c1, sol:F-signals.c69 (p353). The context is the CS paragraph on p353 and the DB7-DB0 paragraph on p354.

**Stimulus.** Environment (1.2), then three loops of 200 iterations, each iteration reading one byte and storing it (E6):
- **K1:** the byte is read from CIAA PRB.
- **K2:** the byte is read from CIAB PRB.
- **K3 (contrast):** the same instruction reads a Chip-memory byte instead. The builder pads the loop body so that the iteration time I3 is not a multiple of 10·T_CPU.

The R-SAMPLE loops used in P2, P4, P5(b) and P9 are checked the same way, once for each store instruction in the body.

**Observation.** The store times t_i. Their phases φ_i = (t_i − t_0) mod T_E, folded into [−T_E/2, T_E/2).

**Expected.** For K1, K2 and every R-SAMPLE loop, all φ_i of one store instruction lie within a band no wider than 4·T_CPU.

Why: "A low level on CS while 02 is high causes the device to respond to signals on the R/W and address (RS) lines" (p353), and the data pins drive "unless CS is low and R/W and 02 are high" (p354). So each transfer occupies the φ2-high part of an E cycle, and the instruction after it starts at a fixed phase of E. The only other timing influence is Chip-memory slot allocation, whose unit is a memory cycle of 2·T_CPU (p194). With DMA off, only the "4 cycles for memory refresh" per line (p194) can delay a CPU access, and the band allows two memory cycles for that.

**Preconditions.**
- **EP2 and the F1 anchor.**
- **Pre6.1 (K3).** The measured I3 mod T_E is not 0. *Contrast:* padding that makes I3 a multiple of 10·T_CPU fails this precondition.

**Contrasting case.** K3. Its iterations are paced by memory cycles only (a multiple of 2·T_CPU) and not by E. With I3 a non-zero multiple of 2·T_CPU mod 10·T_CPU, the phases step through all five even values mod 10·T_CPU. They span 8·T_CPU, more than the band, so the check fails.

**Reference frame.** F1 and F2.

**Left open.** The page does not state the E duty cycle, the number of E cycles one access takes, or how many accesses one E cycle can serve. None of these is graded.

---

### P7: The CRA bit layout (bits 0, 3, 4, 5, 6, 7)

Bits 1 (PBON) and 2 (OUTMODE) of CRA are graded in P1, P3 and P4.

**Claims (all sub-propositions).** opus:F-cra.c12, opus:F-cra.c13 (p349, the CRA bitmap: "E CRA unused 0=input 0=02 1=force 0=cont. 0=pulse 0=PB6OFF 0=stop / unused 1=output 1=CNT load 1=one- 1=toggle 1=PB6ON 1=start / (strobe) shot"), and sol:F-cra.c13 (p349, INMODE). The bitmap is read from the printed image, because the text layer prints "=" as "*" and "1" as "l". Each sub-proposition applies to CIAA and CIAB unless stated: "There are two control registers in the 8520, CRA and CRB" (p348), and the system has two 8520s (p339).

#### P7.0: Bit 0 is START (1 start, 0 stop)

- **Stimulus.** CR ← $00. R-LATCH($8000). Read TAHI, TALO → u1. A delay of about 1000 E periods between markers. Read → u2. Bracketed write CR ← $01. A delay of about 1000 E periods. Bracketed write CR ← $00. Read → u3.
- **Expected.** "1 = start Timer A, 0 = stop Timer A." (p348).
  - u1 = u2 = $8000: stopped, the counter holds, and the latch write loaded it (p343).
  - u3 < u2. The size of u2 − u3 is reported against τ/T_E (P5 grades the rate).
- **Precondition.** No underflow (ICR TA = 0 at the end). *Contrast:* R-LATCH($0100) makes TA = 1.
- **Contrasting case.** CR ← $01 during the first delay makes u2 ≠ u1.

#### P7.3: Bit 3 is RUNMODE (1 one-shot, 0 continuous)

- **Stimulus.** CR ← $00. R-LATCH(40). Read ICR (discard). CR ← $09 (START, one-shot). R-SAMPLE(400; ICR). Read CRA → c. Read TAHI, TALO → u.
- **Expected.** Source: "In one-shot mode, the timer will count down from the latched value to zero, generate an interrupt, reload the latched value, then stop." (p342), and "This bit is automatically reset (= 0) when underflow occurs during one-shot mode." (p348).
  - Exactly one ICR sample has TA = 1.
  - c bit 0 = 0.
  - u = 40.
- **Precondition.** The iteration spacing is ≤ P/2 E periods (the continuous run below shows no two flags in adjacent reads). *Contrast:* with L = 2, flags appear in adjacent reads.
- **Contrasting case.** CR ← $01 (bit 3 = 0, continuous), which "repeat[s] the procedure continuously" (p342): at least 5 samples have TA = 1, one per P(40), and c bit 0 = 1.

#### P7.4: Bit 4 is LOAD (force load, strobe, reads 0)

- **Stimulus, stopped leg.** CR ← $00. R-LATCH($4000). CR ← $01. A delay of about 2000 E periods. CR ← $00. Read → v1. CR ← $10 (LOAD, START = 0). Read → v2. Read CRA → c.
- **Stimulus, running leg.** CR ← $00. R-LATCH($FFFF). CR ← $01. A delay of about 5000 E periods. Bracketed write CR ← $11 (START, LOAD). A delay of about 100 E periods. Bracketed write CR ← $00. Read → v3.
- **Expected.** Source: "A strobe bit allows the timer latch to be loaded into the timer counter at any time, whether the timer is running or not." (p342), and "bit 4 will always read back a zero and writing a 0 has no effect." (p348).
  - v2 = $4000.
  - c bit 4 = 0.
  - $FFFF − v3 ≤ τ_LS/T_E + 2, where τ_LS runs from the marker before the LOAD write to the marker after the stop write. The +2 allows for load and stop latencies the page does not state. Without the load, $FFFF − v3 would be about 5100.
- **Precondition.** v1 ≠ $4000 (the counter has moved). *Contrast:* with no start, v1 = $4000.
- **Contrasting case.**
  - CR ← $00 instead of $10 gives v2 = v1 ("writing a 0 has no effect").
  - CR ← $01 instead of $11 gives $FFFF − v3 ≥ τ_total/T_E − 2, where τ_total runs from the marker after the start write to the marker before the stop write.

#### P7.5: Bit 5 is INMODE (1 counts positive CNT transitions, 0 counts φ2)

- **Stimulus, CIAA.** CR ← $00. R-LATCH($1000). Read ICR (discard). CR ← $21 (START, CNT). "Ready" marker. R-KEY. The program waits W E periods, with 1500 ≤ W ≤ 3000 (the 8 bits take about 8 × 60 µs, about 340 E periods, per p358; the upper bound keeps the φ2 contrast below the latch). Bracketed write CR ← $00. Read TAHI, TALO → v. Read ICR → i.
- **Stimulus, CIAB.** The same without R-KEY (CIAB CNT is POUT, which nothing drives).
- **Expected.** Source: "1 = Timer A counts positive CNT transitions" (p348), with CNT = KCLK (p351) and "a pulse on KCLK (low then high)" per bit, "until all eight data bits have been sent" (p357).
  - CIAA: $1000 − v = 8, and i has SP = 1. The serial port was in input mode (bit 6 = 0): "After eight CNT pulses, the data in the shift register is dumped into the serial data register and an interrupt is generated." (p345).
  - CIAB: v = $1000.
- **Preconditions.**
  - KP. *Contrast:* KP's own contrast.
  - The whole transmission lies between the start and stop writes, judged from the harness's KCLK drive times against the markers. *Contrast:* send the key after the stop write, and v = $1000.
- **Contrasting case.** CR ← $01 (bit 5 = 0, φ2): $1000 − v = round(W/T_E) ± 2, on both CIAs.
- **Left open.** The SDR value is reported, not graded. What it should hold depends on the keyboard's bit order and polarity (p357-p358), which are claims outside this package.

#### P7.6: Bit 6 is SPMODE (1 serial port output, 0 input)

- **Stimulus, CIAB.** CR ← $00. R-LATCH(10). Read ICR (discard). CR ← $41 (START, SPMODE output, continuous). SDR ← $A5. R-SAMPLE(300; ICR). CR ← $00.
- **Expected, CIAB.** Source: "1 = Serial port=output (CNT is the source of the shift clock)" (p348), and p345: "Transmission will start following a write to the serial data register. [...] After eight CNT pulses, an interrupt is generated". At least one sample has SP = 1. The number of TA flags before the first SP flag is reported, not graded: its value, 16 by "Data is shifted out on the SP pin at 1/2 the underflow rate of Timer A" (p345), belongs to claims outside this package.
- **CIAA.** Only the input setting is exercised. The SP = 1 in P7.5's run with CR ← $21 (bit 6 = 0) is this leg's positive result. Its contrast is the same run without R-KEY: SP = 0. CIAA is not put into output mode, because its SP and CNT are the keyboard's KDAT and KCLK (p351), and the host drives KDAT as the keyboard handshake (p357).
- **Precondition (CIAB).** Timer A underflows during the window: at least 20 samples have TA = 1. *Contrast:* CR ← $40 (not started) gives no TA and no SP.
- **Contrasting case (CIAB).** CR ← $01 (bit 6 = 0, input): SP = 0 in every sample. In input mode, data shift only "on the rising edge of the signal applied to the CNT pin" (p345), and CIAB's CNT (POUT) is not driven.

#### P7.7: Bit 7 is unused (unaffected by a write, 0 on a read)

- **Stimulus.** CR ← $80. Read CRA → c1. R-LATCH($8000). Bracketed write CR ← $81. Read CRA → c2. A delay of about 1000 E periods. Bracketed write CR ← $00. Read → w81. A second run is identical except that it writes $01 instead of $81, giving c2' and w01.
- **Expected.** Source: "All unused register bits are unaffected by a write and forced to 0 on a read." (p349).
  - c1 = $00.
  - c2 = $01.
  - |w81 − w01| ≤ 1: the timer behaves as it does with $01.
- **Precondition.** Both runs have the same bracket-to-bracket time within 1 E period. *Contrast:* a different delay count D breaks this.
- **Contrasting case.** c2' bit 0 = 1: a used bit reads back set while the timer runs. p348 describes START being cleared only by a one-shot underflow; see Section 3, item 7.

---

### P8: The CRB bit layout (bits 0, 3, 4, 6-5, 7)

Bits 1 (PBON) and 2 (OUTMODE) of CRB are graded in P1, P3 and P4.

**Claims (all sub-propositions).** opus:F-crb.c13, opus:F-crb.c14, sol:F-crb.c10 (p350, the CRB bitmap, read from the printed image: "F CRB 0=TOD 00=02 1=force 0=cont. 0=pulse 0=PB7OFF 0=stop / 1=Alarm 01=CNT load 1=one- 1=toggle 1=PB7ON 1=start / 10=Timer A (strobe) shot / 11=CNT+ Timer A"), and sol:F-crb.c7 (p349, the INMODE table). Both CIAs unless stated.

#### P8.0, P8.3, P8.4: START, RUNMODE, LOAD

These are specified exactly as P7.0, P7.3 and P7.4, with these substitutions: CRB for CRA, TBLO and TBHI for TALO and TAHI, and ICR bit 1 (TB) for bit 0 (TA). CRB bits 6-5 = 00 (φ2) and bit 7 = 0 in every written value. The expected values, preconditions and contrasting cases carry over unchanged, with the CRB text on p349 as the source: "1 = start Timer B, 0 = stop Timer B. This bit is automatically reset (= 0) when underflow occurs during one-shot mode.", "1 = one-shot mode, 0 = continuous mode.", and "1 = force load (this is a strobe input, there is no data storage; bit 4 will always read back a zero and writing a 0 has no effect.)".

#### P8.65: Bits 6-5 are INMODE (00 φ2, 01 CNT, 10 timer A underflows, 11 timer A underflows while CNT is high)

Source: "CRB6 CRB5 Mode Selected: 0 0 Timer B counts 02 pulses; 0 1 Timer B counts positive CNT transitions; 1 0 Timer B counts Timer A underflow pulses; 1 1 Timer B counts Timer A underflow pulses while CNT pin is held high." (p349). p343 agrees.

- **Mode 00.** P5, the timer-B instances.
- **Mode 01.**
  - *CIAA:* CRB ← $00. R-LATCH_B($1000). CRB ← $21. "Ready" marker. R-KEY. Wait W E periods, with 1500 ≤ W ≤ 3000. CRB ← $00. Read → v. Expected: $1000 − v = 8.
  - *CIAB:* the same without R-KEY. Expected: v = $1000.
  - Preconditions as in P7.5. *Contrasting case:* CRB ← $01 gives $1000 − v = round(W/T_E) ± 2.
- **Mode 10 (both CIAs).**
  - *Stimulus:* CRA ← $00; R-LATCH_A(30); CRB ← $00; R-LATCH_B($FFFF). Read ICR (discard). CRB ← $41 (START, INMODE 10). CRA ← $01. R-SAMPLE(300; ICR). CRA ← $00. Read ICR (final). CRB ← $00. Read TBHI, TBLO → v.
  - *Expected:* $FFFF − v = N_A, the number of ICR reads after CRA's start write, including the final one, that have TA = 1.
  - *Precondition:* the iteration spacing is ≤ P(30)/2, so each TA flag is exactly one underflow. *Contrast:* with R-LATCH_A(2), flags appear in consecutive reads.
  - *Contrasting case:* CRB ← $01 (mode 00) gives $FFFF − v ≥ (window/T_E) − 2, far more than N_A. A second contrast: with CRA left at $00 (timer A stopped), v = $FFFF.
- **Mode 11 (CIAA only; CIAB's CNT has no driver).**
  - *Stimulus:* two runs, X and Y, identical except for the CRB value: X writes $41 (mode 10), Y writes $61 (mode 11). In both: CRA ← $00; R-LATCH_A(3); CRB ← $00; R-LATCH_B($FFFF); CRB ← value; CRA ← $01; "ready" marker; R-KEY at the same d_k; wait W ≥ 1500 E periods; CRA ← $00; CRB ← $00; read TBHI, TBLO. The results are D_X and D_Y (the decrements).
  - *Expected:* D_Y < D_X. With P_A = 3 + δ (δ from P9), the difference D_X − D_Y lies in [Σ_j (⌊ℓ_j/P_A⌋ − 1), Σ_j (⌈ℓ_j/P_A⌉ + 1)], summed over the 8 KCLK low intervals. The ±1 per interval allows for the page not saying when "held high" is sampled relative to an underflow.
  - *Preconditions:* KP. The transmission lies inside the window. P9 has determined δ. *Contrast for KP:* its own. *Contrast for the window:* send the key after CRA's stop write, and D_Y = D_X.
  - *Contrasting case:* the same pair of runs without R-KEY. Then D_Y = D_X, because CNT stays high throughout ("Before the transmission starts, both KCLK and KDAT are high", p357).

#### P8.7: Bit 7 is ALARM (1: TOD writes set the alarm; 0: they set the TOD clock)

Source: "1 = writing to TOD registers sets Alarm, 0 = writing to TOD registers sets TOD clock. Reading TOD registers always reads TOD clock, regardless of the state of the Alarm bit." (p349). The sequence rules are on p344: "TOD is automatically stopped whenever a write to the register occurs. The clock will not start again until after a write to the LSB event register." and "All TOD registers latch on a read of MSB event and remain latched until after a read of LSB event." Writes are made as TODHI, TODMID, TODLO. Reads are made as TODHI, TODMID, TODLO.

- **Stimulus, CIAB.**
  - *Setup:* CRB ← $00. Write TOD ← $000000. Wait at least 20 lines. Read TOD → T0 (bracketed by markers). Read ICR (discard).
  - *Run X:* CRB ← $80. Write TOD registers ← A = T0 + 12. CRB ← $00. Read TOD → T1. R-SAMPLE(n; TODLO, ICR), with n chosen by the builder so that the window covers at least 30 lines (F4).
  - *Run Y:* the same with CRB kept at $00 during the writes, and A = $001000.
- **Expected, CIAB.**
  - *Run X:* T1 − T0 = the number of lines (F4) between the T0 and T1 read markers, ± 1. The writes did not change TOD.
    - The ALRM flag (ICR bit 2) appears. No ICR sample before the first TODLO sample equal to A & $FF has ALRM. The first ALRM sample comes within one iteration after that TODLO sample.
    - The last two points rest on "A programmable alarm is provided for generating an interrupt at a desired time" (p344).
  - *Run Y:* T1 − A = the number of lines between the marker after the TODLO write and the T1 read, ± 1.
    - No ALRM flag appears in the window. The alarm was never written in this run, and "All other registers are reset to zero" (p354) makes it 0. TOD counts up from $001000 and does not reach 0 within the window.
- **Stimulus, CIAA.** The same setup, with no wait needed. Run X uses A = T0 + $1000; run Y uses A = $001000. The T0 to T1 interval is kept under one frame. The sampler part is omitted.
- **Expected, CIAA.** Run X: T1 − T0 ∈ [0, 2]. Run Y: T1 − A ∈ [0, 2]. These hold whichever of the two sources on p339 ("VSync or line tick") drives the counter.
- **Precondition (CIAB).** TOD counts lines. Before run X or Y, two reads 20 lines apart differ by 20 ± 1. *Contrast:* with a 40-line wait, they differ by 40 ± 1.
- **Contrasting case.** Flipping bit 7 turns run X into run Y and back. The page predicts the opposite outcome for T1 and for the ALRM flag.

---

### P9: The latch is the divider of the input clock

**Proposition.** When the timer counts φ2, the underflow period is P(L) = L + δ E periods, with one constant δ for every latch value L. So the toggle waveform's frequency, and the width of a one-shot's high output, follow the latch.

**Claims.** opus:F-timers.c6 and sol:F-timers.c8 (p341): "This latch (prescalar) value is a divider of the input clocking frequency." [...] "Various timer operating modes allow generation of long time delays, variable width pulses, pulse trains, and variable frequency waveforms."

**Instances.** CIAA timer A and CIAB timer B are required; the other two are optional.

**Stimulus.**
- **Leg F (frequency).** P3's run, with L ∈ {40, 41, 64, 100}.
- **Leg W (width).** P2 leg S, with L ∈ {40, 100} and R-SAMPLE long enough to pass the underflow.

**Observation.**
- P(L) from the ICR stores (F3).
- The toggle half-period: the E-index difference between consecutive PRB transitions.
- W(L): the E-index of the first bit-n = 0 sample after the start marker, minus the E-index of that marker.

**Expected.**
- δ = P(L) − L is the same for all four L, and δ ∈ {0, 1}.
- Consecutive PRB transitions are P(L) ± s E periods apart, where s is the PRB sampling spacing in E periods.
- W(100) − W(40) = 60 ± s.

Why δ is 0 or 1: p342 says the timer will "count down from the latched value to zero, generate an interrupt, reload"; p346 calls an underflow "timer counts down past 0". The pages leave open which of L or L+1 the period is, so only δ's constancy and range are graded.

**Preconditions.** Those of P3 (Pre3.1, Pre3.2) and of F3.

**Contrasting case.**
- Changing L by Δ changes the period by exactly Δ: every pair of latch values in leg F is a contrast to the others.
- With CNT counting and no input (CR ← $27, or CRB ← $27), the page predicts no underflow and no transitions at all.

**Reference frame.** F2 and F3.

---

## 3. What the pages leave open, and where they disagree

1. **Underflow period: L or L+1.**
   - p342: "count down from the latched value to zero, generate an interrupt, reload the latched value".
   - p346: "Underflow from Timer A (timer counts down past 0)".
   - p352's example divides the wanted time by the count period directly ("3000 / 1.3968255 = 2148") and calls the result "(exactly) 3 milisecond intervals".

   The spec grades only what all three agree on: P(L) − L is a constant 0 or 1 (P4, P9).
2. **"One cycle duration"** (p342) does not name the clock. This spec reads it as one φ2 cycle, the clock "used for internal device operation" (p353), and grades the width only in φ2 mode.
3. **"Set high whenever the timer is started"** (p342) does not say whether a START = 1 write to a running timer, or a one-shot start by a timer-high write (p342), counts as a start. Nor does any page say what stopping does to the toggle output. None of this is graded (P2).
4. **Pulse idle level.** No page states that the pulse output is low between pulses; "positive pulse" implies it. P1 leg A2 and P4 rely on this reading. P1 legs A1, B and C do not.
5. **CIA address bits, p353.** The binary patterns "CIAA ... 101x xxxx xx01 rrrr xxxx xxx0" and "CIAB ... 101x xxxx xx10 rrrr xxxx xxx1" give A13 = 0, A12 = 1, A0 = 0 for CIAA. That contradicts the same page's "CIAA is selected when A12 is low, A13 high" and its named addresses (BFEr01, BFDr00), which p339, p340, p351 and Appendix D p314 also give. The two patterns appear swapped. This spec uses only the named addresses, on which every page agrees.
6. **p244** says "Bits labeled "PB" are output bits located in CIAAPRB ($BFD100)". The address is CIAB's, and the page's own heading reads "CIAAPRA/CIABPRB". This spec uses CIAB $BFD100.
7. **Read-back of used control-register bits** is not stated directly. p340 ("16 registers that you may read or write") and p348 (START "automatically reset (= 0)" on a one-shot underflow; LOAD "will always read back a zero") imply that used bits read back. Only P7.7's contrast depends on this.
8. **The CIAA TOD source** is "VSync or line tick" (p339). P8.7 grades CIAA only in a way that holds for either source.
9. **p352's example** sets one-shot mode before writing TAHI, so its TAHI write already starts the timer (p342). Its DMACON write carries the comment "Kill all chip interrupts". This spec takes from p352 only the latch write order.
10. **The claims' text readings of the bitmaps on p349 and p350** print "=" as "*" or "-" and "1" as "l". The printed images confirm the readings used here.
