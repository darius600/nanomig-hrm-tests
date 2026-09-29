# appF-cia-system: specification

This is the specification of the integration test package `appF-cia-system`. It was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. It covers three things on the two 8520 CIAs: the timer outputs on port B (PB on/off, toggle and pulse), the bit layout of the control registers CRA and CRB, and the φ2 clock input. Page numbers are printed page numbers. "φ2" is the clock the manual prints as "02". A builder implements the package from this document and may not change what it specifies.

---

## 0. Claims that give no proposition, or only part of one

| Claim | Page | What cannot be used, and why |
|---|---|---|
| opus:F-portsig.c47 | 351 | "PC...not used" (CIAB). None of the outputs a test can see shows a PC pin on either CIA, and "not used" describes no behaviour and gives no value. (The text layer puts the FLAG gloss "disk index*" on the PC line. The printed page puts it on "F....INDEX*", and the claim reads it correctly.) |
| opus:F-signals.c1, sol:F-signals.c69 (both whole) | 353 | Both claims say that φ2 is a "TTL compatible input". That is an electrical property of an input pin, and nothing a test sees depends on it. A claim with a clause the package does not grade is not claimed, so both ids are declined whole. What else they say (φ2 is used for internal device operation and as the timing reference for the bus) is still checked, by P5 and P6, without these ids. |
| sol:F-signals.c70: ".715909 Mhz for NTSC or .709379 Mhz for PAL" (in its quote; the claim itself says only that φ2 is the E clock) | 353 | The test measures time in system clock cycles. Absolute frequencies come from the harness's clock, not from the design. Only the stated ratio, "1/10 of the CPU clock", is checked (P5). |
| opus:F-cra.c12, opus:F-crb.c13: the reading that LOAD has a "0 setting" of force load | 349, 350 | The bitmaps print one entry for LOAD, "1=force load (strobe)", split over two lines. LOAD has no 0 setting: "writing a 0 has no effect" (p348, p349). The rest of both claims is used (P10). |

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

- **E1.** The 68000 enters the test program directly from reset, with no other code executed first. The program sits in Chip memory at PROGRAM_ORG ($080400), where the harness loads it and points the reset vectors (`nanomig.i`).
- **E2.** The program's first instructions set SR to $2700 and write $7FFF to DMACON ($DFF096) and to INTENA ($DFF09A). Bit 15 = 0 clears (bit 15 is SET/CLR in both registers, p278 and p280). p352 makes the same DMACON write before its CIA timing example.
- **E3.** For each CIA, the program writes ICR ← $7F and then reads ICR once, discarding the value. The write clears all mask bits: "If bit 7 is a 0, then any bit 6-0 in your own mask byte which is set to a 1 clears the corresponding bit in the MASK register" (p347). The read clears the flags: "When you read the DATA register, its contents are cleared" (p346).
- **E4.** The program never writes CIAA PRA or DDRA, because PA0 is the "memory overlay bit" and PA1 the power LED (p351). It never writes CIAB PRA or DDRA.
- **E5. Markers.** A marker is a word store of a distinct constant to a marker area in Chip memory. The test sees it at the Chip-memory port as a write with a time. A *bracketed* CIA write is a marker, then the CIA write, then a marker, with no other instruction between them.
- **E6. Results.** Every value the program reads for grading goes into a data register and is then stored as a word to a result area in Chip memory. Each store is visible at the Chip-memory port with its time.
- **E7. Parameters.** Anything that differs between runs of the same program (latch values, delay counts, CR values, key timing) is read from a parameter block in Chip memory. Runs that are compared with each other must execute identical code.
- **E8.** The program ends with a final marker and then branches to itself. This is the `DONE` macro of `nanomig.i`: a store of DONE_VALUE to DONE_ADDR ($0C0FFE), then a branch to itself.
- **E9. Layout.** After reset the ROM overlay covers low Chip memory ("The system RAM that would normally be located at this address is not available", p223), and E4 never clears it. So everything a program writes, and every Chip-memory byte it reads as data, lies in $080000-$1FFFFF, which the harness maps one-to-one (`nanomig.i`), at these places, none of which overlap:
  - the program, from PROGRAM_ORG ($080400) up; the stack, below PROGRAM_STACK ($0FFF00);
  - the result area (E6), from RESULTS ($0C0000) up and below DONE_ADDR: at most 2047 words per run. The largest run, P3's sampler of 600 iterations with two stores each, stores 1200;
  - the final marker (E8), at DONE_ADDR;
  - the other markers (E5), in $0C1000-$0C1FFF;
  - the parameter block (E7), from $0C2000;
  - the Copper list of EP2's contrast, at $0C3000;
  - the Chip-memory byte that P6's loop K3 reads, at $0C4000.
- **E10. Plain writes, 68000 code.** Every CIA register write is one byte MOVE of the whole value (for PRB, the value the case needs in every bit). No CLR, BSET, BCLR, BCHG, AND, OR or other instruction that reads its operand before writing it is used on a CIA register. A read of PRB returns the pins, not the data register (p341), so a read-modify-write would copy a pulled-up input or an overridden PB6/PB7 level into the data register; a read of ICR clears it (p346); a read of TODHI latches the TOD registers until TODLO is read (p344). The programs are assembled for the 68000 (vasm's default): with `-m68010` or higher, vasm assembles `MOVE #0,<ea>` as `CLR <ea>`, which on the 68000 reads its operand first.

**Environment preconditions.** Every run asserts all three.

- **EP1 (reset state).** The program's first reads of CRA and CRB on both CIAs, made before any write to them, return $00. Source: "The timer control registers are set to zero" (p354). *Contrast:* a variant that writes CRA ← $0A before the read gets $0A back.
- **EP2 (DMA quiet).** Between the first and the last marker, the Chip-memory port shows only CPU accesses and memory-refresh accesses. *Contrast:* start Copper DMA in the order p30 prescribes: write COP1LC, strobe COPJMP1, then write DMACON ← $8280 (SET, DMAEN, COPEN). COP1LC points at the list of E9, a single WAIT that never completes, $FFFF,$FFFE ("ends Copper list", p23). Copper fetches from $0C3000 then appear.
- **EP3 (completion).** The final marker appears. A run without it has failed; it has no graded result.

### 1.3 Observation channels

- **O1. PRB read-back.** "When you READ a PR register, you read the actual current state of the I/O pins (PA0-PA7, PB0-PB7, regardless of whether you have set them to be inputs or outputs." (p341). For CIAA PB6 and PB7, this is the only channel: no output the test sees shows the Centronics lines.
- **O2. Drive LED (CIAB PB7, MTR*, only).** From p244 (checked against the scan):

  > "Disk motor control (active low). [...] Each drive will latch the motor signal at the time its select signal turns on. The disk drive motor will stay in this state until the next time select turns on. DSKMOTOR* also controls the activity light on the front of the disk drive." [...] "All drive motors turn off after system reset."

  *Drive.* The latch and the light belong to a drive ("Each drive will latch the motor signal at the time its select signal turns on"; "the activity light on the front of the disk drive", p244), and SEL0* is "select internal drive" (p351). The harness's default `floppy_config` is 0, one drive, DF0, write-enabled, at normal speed (`nanomig_harness.h`). No disk and no disk image is needed: nothing used here from p244 depends on one. The light is read at the `fdd_led` output (port list).

  *Not observable at this port.* `fdd_led` tracks disk DMA activity, not MTR* latched at a drive's own select (NM-HRM-046: the light follows the disk-transfer state machine, not the per-drive motor latch the same module implements correctly for `index` and `syncint`). O2's own legs, and LP below, are therefore recorded as measurements, not graded.

  *Routine R-SELECT.* CIAB DDRB bit 3 = 1. Write PRB with bit 3 = 1, then with bit 3 = 0 (select turns on and the drive latches MTR*), then with bit 3 = 1. Each write is a plain byte write of the whole PRB value (E10) that keeps the other data bits the case needs.

  *Expected channel behaviour:* if MTR* is 0 when select turns on, the LED is on; if it is 1, the LED is off.

  *Observation interval:* from the marker after the bit-3 = 0 write to the marker before the next R-SELECT. The page gives no settle time, so the graded LED level is the one at the end of this interval. The builder makes the interval at least 2000 E periods long.

  *Calibration precondition LP.* This applies to every run that grades O2, and must hold before any O2 leg is graded:
  1. At program entry the LED is off (p244).
  2. With CIAB CRB at its reset value $00 (EP1; LP writes no control register, so P2's leg R keeps the first CRB write) and DDRB = $88, write PRB bit 7 = 0 and run R-SELECT: the LED turns on.
  3. Write PRB bit 7 = 1 and run R-SELECT: the LED turns off.

  LP itself is recorded, not graded (O2 is not observable at this port); the O1 legs of the same run are still graded. *Contrast:* change bit 7 without running R-SELECT, and the LED keeps its previous state ("stay in this state until the next time select turns on") -- also recorded, since no proven-live input through R-SELECT turns this build's own reading green.
- **O3. Chip-memory port.** Marker and result stores with their times (E5, E6), and the refresh accesses (F4).
- **O4. ICR flags.** Read ICR bits: D0 TA, D1 TB, D2 ALRM, D3 SP, D4 FLG, D7 IR (p347). "Any interrupt will set the corresponding bit in the DATA register." (p346), whatever the MASK holds. The sources include "Underflow from Timer A (timer counts down past 0)" and "Underflow from Timer B" (p346). A read clears all flags (p346), so each ICR read reports the events since the previous read.

### 1.4 Reference frames

- **F1. CPU clock.** T_CPU is the CPU clock period in system clock cycles. It is measured at the ports, not assumed.
  - A memory cycle is a colour clock ("227.5 "color clocks", or memory access cycles", p194). A colour clock is two CPU clock periods: it is 281.94 ns on PAL and 279.36 ns on NTSC (p255), and the CPU clock is 7.09379 MHz on PAL and 7.15909 MHz on NTSC (p2).
  - Figure 6-9 (p195) draws the four refresh slots of a line in alternate memory cycles. So T_CPU is one quarter of the spacing between consecutive refresh accesses of one F4 group.
  - *Anchor check (every run):* consecutive F4 line marks are 454·T_CPU apart throughout ("In PAL, they are all 227 long", p23), or alternately 456·T_CPU and 454·T_CPU ("Every other line is a long line (228 color clocks ...), with the others being 227 color clocks long", NTSC, p23). *Contrast:* the same check evaluated with 1.25·T_CPU fails.
- **F2. E period and E-index.** T_E = 10·T_CPU, from "The "E" clock runs at 1/10 of the CPU clock." (p353), which p352 repeats ("count down at 1/10 the CPU clock").
  - The E-index of a store at time t, taken relative to an earlier store t0 in the same loop, is e = round((t − t0) / T_E).
  - An E-index is defined only in a run that passes P6's phase check for that loop. If the check fails, every grade that needs an E-index is not given for that run, and P6 is red.
- **F3. Underflow period P(L).** This is for a timer running continuously on φ2 with latch L, where every R-SAMPLE iteration includes an ICR read.
  - Let f_j be the E-indices of the ICR stores and q_j their TA or TB flag. P(L) is the integer P for which one phase φ exists such that, for every j ≥ 1, q_j = 1 exactly when φ + kP lies in (f_(j−1), f_j] for some integer k.
  - It requires an iteration spacing of at most P/2 E periods and at least 16 flags, so that exactly one P fits.
  - The pages leave open whether P(L) is L or L+1 (Section 3, item 1).
- **F4. Lines.** Each line has "4 cycles for memory refresh" (p194).
  - The first refresh access of each group of four marks a line. The number of lines between two events is the number of marks between their times.
  - *Anchor:* each group is four refresh accesses two memory cycles apart (Figure 6-9, p195), and the marks pass F1's anchor check: 227 memory cycles apart on PAL, or alternately 228 and 227 on NTSC (p23; p194's 227.5 is the NTSC average). *Contrast:* taking groups of three misaligns the marks and the anchor fails.

### 1.5 Routines

- **R-LATCH(T, L).** While the timer's CR has START = 0 and RUNMODE = 0, write the timer's LO register ← L & $FF, then its HI register ← L >> 8.
  - The order comes from p352: "Set time (low byte THEN high byte)".
  - Afterwards the counter holds L: "The timer latch is loaded into the timer on any timer underflow, on a force load, or following a write to the high byte of the pre- scalar while the timer is stopped" (p343).
  - RUNMODE must be 0 during the write because of p342: "In one-shot mode, a write to timer-high [...] will transfer the timer latch to the counter and initiate counting regardless of the start bit". Timers in this package are started only by writing START = 1 (P2 depends on what counts as a start).
- **R-SAMPLE(n; X [, Y]).** Run n iterations of: read register X and store it (E6); optionally, read register Y and store it. The loop body is fixed code with no data-dependent branch.
- **R-SELECT.** See O2.
- **R-KEY.** The test sends one key code, $35 ("B" pressed, p358), on the keyboard inputs of the port list (`kbd_mouse_type`, `kbd_mouse_data`, `kbd_mouse_level`, listed under "mouse and keyboard"), in the form those inputs take a key code, d_k ≥ 100 E periods after it sees the program's "ready" marker. A code sent there is the keyboard of p357: its KCLK is CIAA's CNT and its KDAT is CIAA's SP (p351).
  - Per p357 and p358, the keyboard puts out each of the eight bits "followed by a pulse on KCLK (low then high)". KCLK stays low "about 20 microseconds", at "about 60 microseconds per bit". Before the transmission "both KCLK and KDAT are high".
  - No port carries KCLK or KDAT, so the test sees neither, and no check uses the time or the length of a KCLK pulse. The page's timing only sizes the waits (P7.5) and gives the direction graded in P8.65 mode 11. Where a case needs the transmission inside its window, the window precondition of P7.5 places it: the code is sent after the ready marker, and the SP flag is read set before the stop write.
  - No handshake is given. Every case that uses R-KEY ends within 143 ms of sending the code, and so within 143 ms of the last clock, since the keyboard resyncs only after "143 ms" without a handshake (p359). The test sends no other code in the run.
  - **KP (keyboard idle).** A run of the case's program without R-KEY, in which the CIAA timer that the case does not grade counts CNT: CRB ← $21 when timer A is graded, CRA ← $21 when timer B is graded. Where the case uses that timer otherwise, this write replaces its start write. The probe timer is unchanged over a window of the same length. The probe is always the other timer, so that a wrong INMODE on the timer under test cannot fail KP in place of its own check. *Contrast:* with R-KEY inside that window, it changes by 8.

### 1.6 Runs

- **CPU.** Every run is made on the 68000 core, and on the second core in its default 68000 behaviour. No claim concerns the 68010 or 68020 settings, and p353 speaks of the "680x0 "E" clock" without saying how another CPU derives it, so those settings are not run.
- **Video standard.** Whatever the harness's configuration gives. Its `Config` has no field for choosing PAL or NTSC (`nanomig_harness.h` documents `chipset_config` bit 3, ECS, and bit 4, AGA, only), and the harness drives a single `clk_sys`, so a run cannot be moved to the other standard's crystal (p352: "NTSC 28.63636 MHz", "PAL 28.37516 MHz"). No second run on another standard is made. Nothing graded here needs a particular standard: T_CPU and the line length are measured (F1, F4), and "The "E" clock runs at 1/10 of the CPU clock" (p353) is stated for both.
- **Chip set and memory.** ECS (the default), 2 MB of Chip memory. No claim distinguishes OCS from ECS.
- **Separate runs.** Each leg of a proposition is its own run from reset unless its stimulus says otherwise.

---

## 2. Propositions

### P1: PB on/off puts the timer output on PB6 or PB7, forcing an output over DDRB and the PR data

**Proposition.** With PBON = 1 (CRA bit 1 for timer A, CRB bit 1 for timer B), the timer's output appears on PB6 (timer A) or PB7 (timer B) of the same CIA, and the line is an output whatever DDRB and the PR data bit say. PBON affects only that one line. With PBON = 0 the line is back in normal operation.

**Claims.** Graded whole here:
- p342: opus:F-timers.c10, opus:F-timers.c11, sol:F-timers.c12.
- p341: opus:F-ports.c7, sol:F-ports.c7.
- p349, CRA bitmap PBON column: sol:F-cra.c17.
- p349, CRB text: sol:F-crb.c3, opus:F-crb.c2.
- p350, CRB bitmap PBON column: sol:F-crb.c14.

P1 also grades the PBON columns of the whole-bitmap claims opus:F-cra.c12, opus:F-cra.c13, opus:F-crb.c13 and opus:F-crb.c14. Those ids are carried by P10.

**Instances.** There are four: (CIAA, timer A, n = 6), (CIAA, timer B, n = 7), (CIAB, timer A, n = 6), (CIAB, timer B, n = 7). In what follows, m = 1 << n, and o is the other one of bits 6 and 7.

**Other-bit pattern.** For bits 0-5, DDRB = 1 and the PR data are:
- CIAA: pattern Q1 = %101010 and pattern Q2 = %010101.
- CIAB: pattern Q1 = %111101 (DIR = 0, SIDE* = 1) and pattern Q2 = %111011 (DIR = 1, SIDE* = 0). SEL0*-SEL3* and STEP* are always written as 1. Two lines leave 1 all the same: SEL0* when R-SELECT drives it, and on the timer-A instance PB6 (SEL3*), which is line n.

Bit o has DDRB = 0 throughout (it is an input). On CIAB, R-SELECT needs DDRB bit 3 = 1, which the pattern already provides.

**Stimulus.** One run per instance, in this order. On CIAB, the value written as "DDRB ← $00" in steps 1 and 5 is $08 instead, after a first write of PRB ← $FF. This drives SEL0* high and keeps bit 3 an output for R-SELECT, while bit n stays an input.
1. Environment (1.2). DDRB ← $00. CR ← $00.
2. **Pull-up reference.** Read PRB → r0.
3. **Leg A2: pulse output, stopped.** CR ← $02 (PBON, pulse, stopped). Read PRB → a2.
4. **Make the toggle output low by exactly one underflow.** CR ← $00. R-LATCH(L = $0010). CR ← $0F (START, PBON, toggle, one-shot). Poll CR until bit 0 reads 0, at most 1000 polls. Read ICR → i1.
5. **Leg A1: DDRB override, line an input.** With DDRB = $00 and CR as the one-shot left it, read PRB → a1.
6. **Leg B: PR-data override.** DDRB ← m | pattern bits. PRB ← m | Q1 (data bit n = 1). Read PRB → b1.
7. **Leg C: timer output high.** PRB ← Q2 (data bit n = 0). CR ← $00. R-LATCH($FFFF). CR ← $07 (START, PBON, toggle, continuous). Within 1000 E periods, read PRB → c1, then read ICR → i2.
8. **Leg D: normal operation.** CR ← $00. Read PRB → d1 (DDRB bit n = 1, data 0). Pre1.4's own contrast: PRB ← m | Q2 (data bit n = 1, DDRB bit n still 1, CR still $00). Read PRB → o1c. DDRB ← pattern bits without m. Read PRB → d2.
9. **O2 legs (CIAB, timer B only).** First LP, at the start of the run. Then run R-SELECT at the end of step 5 (graded as LED-A1), at the end of step 7 (LED-C), after d1 (LED-D1) and after d2 (LED-D2). In step 5, bit 7 is an input (DDRB = $08), overridden by PBON.

**Observation.** The result stores of r0, a1, a2, b1, c1, d1, o1c, d2, i1 and i2 (O1, O4), and the LED level at the end of each O2 interval.

**Expected.**
- **a2 bit n = 0.** In pulse mode the output is "a single positive pulse of one cycle duration" on an underflow (p342), and with the timer stopped there is no underflow. A positive pulse rises from low (see Section 3, item 4). PBON "overrides the DDRB control bit and forces the appropriate PB line to become an output" (p342), so the line reads 0 although DDRB says input and r0 shows the pull-up.
- **a1 bit n = 0.** "The toggle output is set high whenever the timer is started" and "On every timer underflow, the output can either toggle" (p342). One start and one underflow leave it low.
- **b1 bit n = 0**, although DDRB bit n = 1 and the data bit is 1.
- **c1 bit n = 1**, although the data bit is 0 (set high on start, no underflow).
- **d1 bit n = 0** (the data bit) **and d2 bit n = 1** (an input with a pull-up): "0 = PB6 is normal operation" (p348), "0 = PB7 is normal operation" (p349).
- **o1c bit n = 1** (Pre1.4's own contrast: the same output, now driven to 1).
- **Other bits.** In a1, a2, b1 and c1, every bit other than n equals what the same DDRB and PR contents give with PBON = 0. An output bit reads its PR data bit. An input bit reads the same bit of r0, the level that pin had undriven in step 2 ("you read the actual current state of the I/O pins", p341). In a1 and a2 this is r0 apart from bit n; in b1 and c1 it is the pattern bits, and r0's bit o. The override affects "the appropriate PB line" only.
- **LED (CIAB, timer B).** LED-A1 on, LED-C off, LED-D1 on, LED-D2 off.
- **Quantifier.** The claims name PB6 for timer A and PB7 for timer B generally, so all four instances must pass.

**Preconditions.**
- **Pre1.1.** r0 bit n = 1. The undriven line reads high: "Ports A and B have passive pull-up devices" (p341); "a read of the ports will return all highs because of passive pull-ups" (p354). Without this, legs A1 and A2 cannot tell a driven 0 from an undriven one, and d2 and LED-D2 cannot tell an input from an output driving 0. If r0 bit n = 0, legs A1 and A2, d2 and LED-D2 are not graded and the value is reported; legs B and C, d1 and LED-D1 still are. *Contrast:* set DDRB bit n = 1 and the data bit to 0 before the read, and r0 bit n = 0.
- **Pre1.2.** Exactly one underflow happens before leg A1: CR bit 0 reads 0 within the poll limit, and i1 has TA (timer A) or TB (timer B) set. *Contrast:* with L = $FFFF in step 4, the poll limit runs out with bit 0 still 1 and the i1 flag at 0.
- **Pre1.3.** No underflow happens during leg C: the i2 flag is 0. *Contrast:* with L = $0004 in step 7, the flag is 1, and c1 is then not graded.
- **Pre1.4 (O1, per instance).** DDRB bit n = 1, data 0, PBON = 0 reads 0 (this is leg D's own d1); the contrast, data 1 (DDRB bit n still 1, PBON still 0), reads 1 (an extra leg, read right after d1, before DDRB is relaxed for d2). Together these establish whether a PRB read on this instance can show a driven level at all: "When you READ a PR register, you read the actual current state of the I/O pins" (p341). Where it does not, P1's PBON legs (a2, a1, b1, c1, d1, d2) and P1-OTHERBITS' pattern-bit clause are not graded on that instance, and the instance's own read-back is reported instead.
- **LP** for the O2 legs (O2).
- **EP1-EP3**, and the F1 anchor.

**P1's control.** Repeat each leg with PBON = 0: CR ← $00 in step 3, CR ← $0D in step 4 (so the step-5 state has PBON = 0), CR ← $05 in step 7. The page then predicts, and this is graded in the normal run against the page's own PBON = 0 values, one per instance whose Pre1.4 holds:
- a1 and a2 bit n = 1 (input, pull-up);
- b1 bit n = 1 (the data);
- c1 bit n = 0 (the data);
- d1 bit n = 0 and d2 bit n = 1 (unchanged from the PBON = 1 run: CR = $00 at both points, so PBON's own setting does not reach them);
- LED-A1 off (MTR* pulled up);
- LED-C on (in leg C, DDRB bit 7 = 1 and the data bit 7 = 0, so MTR* is low).

**Reference frame.** None beyond F1. E-indices are not needed.

**Left open.** DDRB's own read-back while PBON = 1 is not stated; it is read and reported, not graded.

---

### P2: The toggle output is low after RES and set high whenever the timer is started

**Proposition.** Before any start after reset, the toggle output is low. Every start of the timer sets it high, whether it was low or high before.

**Claims.** opus:F-timers.c14 (p342), graded whole here, on one assertion over legs R, S and H together (each leg is also its own component line, without the id, since each grades only part of the claim). P2 also grades the start and RES clauses of sol:F-timers.c13 (p342), which P10 carries.

**Instances.** The four of P1.

**Stimulus.** One run per instance.
- **Leg R** (the first actions on this CIA's timer after reset). DDRB ← $00; on CIAB, DDRB ← $08 for R-SELECT, with PRB ← $FF. CR ← $06 (PBON, toggle, not started). Read PRB → r1. On CIAB timer B, run R-SELECT and read the LED (LED-R).
- **Leg S** (start from low, four times). **DDRB bit n = 1, PR bit n = 0** (a specification change from p342's own wording, "This function overrides the DDRB control bit and forces the appropriate PB line to become an output": without a driven line to start from, Pre2.3 cannot hold on this build; see the commit message). Read PRB → s0 (the level right before start 1). CR ← $00. R-LATCH($0020). For k = 1..4:
  1. Bracketed write CR ← $0F (one-shot, toggle, PBON, START).
  2. R-SAMPLE(64; PRB) → s_k[0..63].
  3. Poll CR until bit 0 = 0 (at most 1000 polls); the poll count is stored (Pre2.2).
  4. Read PRB → z_k. Read ICR → i_k.

  After the one-shot the counter again holds $0020: "reload the latched value, then stop" (p342).
- **Leg H** (start from high). Same DDRB bit n = 1 / PR bit n = 0 override as leg S (restated; still holds from leg S). CR ← $06. R-LATCH($FFFF). CR ← $07; read PRB → h1. CR ← $06 (stop); read PRB → h2. CR ← $07 (start again); read PRB → h3. Read ICR → ih. All within 2000 E periods.

**Observation.** The O1 values, s0, and the order of the s_k samples. The LED (O2).

**Expected.**
- **Leg R.** r1 bit n = 0: "set low by RES" (p342), with no start since reset. LED-R is on (MTR* low at the select).
- **Leg S, for every start k whose Pre2.3 holds.** In bit n, the level before the start (0, Pre2.3) followed by s_k[0..63] reads 0…0 1…1 0…0: at least one 1, and exactly one rise. The rise is the start setting the output high ("The toggle output is set high whenever the timer is started", p342).
  - Only this order is graded. No page states how soon after the start write the output rises, or when the underflow comes, so no E-index window is graded.
  - The trailing 0s, from the one-shot's underflow toggling the output (p342), are allowed and not required: the toggle is P3's clause.
- **Leg H.** h1 = 1 and h3 = 1 (bit n). h2 is reported, not graded (see Left open). h3 separates "set high" from "toggle" only when h2 = 1: a design that toggles on start would then give h3 = 0. If h2 = 0, h3 = 1 is still required, and the report says the leg did not separate the two.
- **Quantifier.** "Whenever" means all four starts of leg S, plus leg H.

**Preconditions.**
- **Pre2.1 (leg R).** E1 holds, and EP1 read CR = $00. The program's first write to this CR is the one in leg R. If the harness runs any code before the test program, leg R cannot be graded, and that is reported. *Contrast:* insert R-LATCH($FFFF) and CR ← $07 before leg R's write. The page then predicts r1 = 1 (started, no underflow) and LED-R off.
- **Pre2.2 (leg S).** Before each restart, the previous one-shot has ended: CR bit 0 read 0 within the poll limit and i_k has the flag. *Contrast:* with R-LATCH($FFFF), the poll limit runs out with bit 0 still 1.
- **Pre2.3 (leg S, start k).** The level before start k is 0: s0 bit n for k = 1 (the driven level leg S's own override wrote before any start), z_(k−1) bit n otherwise (the previous one-shot's underflow toggled the output low, p342). A start whose level before is not 0 is not graded, and the value is reported. *Contrast:* write start k−1 as CR ← $0D (PBON = 0) and leave DDRB bit n = 0 (an input) for that start. That one-shot still ends, so Pre2.2 holds, but with DDRB bit n = 0 the line reads its pull-up: z_(k−1) = 1, and start k is not graded.
- **Pre2.4 (leg H).** The ih flag is 0 (no underflow in leg H). *Contrast:* with R-LATCH($0008), the flag is 1.
- **Pre1.4/O1 (legs R and H, per instance).** The same precondition P1 states: a fact of the instance's own read-back channel, not of which program runs it, so it is measured once from P1's own runs and reused here. Where it does not hold (CIA-A), legs R and H are not graded on that instance. Leg S needs no separate gate: Pre2.3 already excludes CIA-A on its own (s0 never reads 0 there).

**Contrasting case.** In leg S, write start 4 as CR ← $0E (START = 0). The page predicts no start, so s_4 reads 0 throughout, with no rise, and the check fails for k = 4. Every precondition of start 4 still holds (start 3's one-shot ended, and z_3 = 0), and no later start depends on start 4's underflow. The pulse mode (CR ← $0B) is not a contrast for an order-only check: its one-cycle pulse at the underflow (p342) can fall on a sample and read as a single rise. For leg R, see Pre2.1's contrast.

**Reference frame.** The order of the samples only; P2 needs no E-index. P9 leg W reuses leg S with E-indices (F2), counted from the store of the marker right after the bracketed start write.

**Left open.** These are not graded:
- What stopping the timer does to the toggle output.
- Whether a START = 1 write to a timer that is already running is a "start".
- Whether the one-shot start caused by a timer-high write (p342) is a "start".

---

### P3: In toggle mode the output inverts on every underflow, and at no other time

**Proposition.** With OUTMODE = 1 (bit 2) and PBON = 1, the PB line changes level once at each underflow and never otherwise.

**Claims.** None graded whole here. P3 grades the toggle half of these claims; P4 grades their pulse half, and P10 carries the ids:
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

**Claims.** None graded whole here. P4 grades the OUTMODE "0=pulse" half of the claims P3 lists, and the pulse half of opus:F-timers.c12, opus:F-timers.c13 and sol:F-timers.c13 (p342). It also shows the "pulse trains" of opus:F-timers.c6 and sol:F-timers.c8 (p341). P10 carries all these ids.

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

**Proposition.** A timer counting φ2 pulses (CRA INMODE = 0, CRB INMODE = 00) decrements once per E cycle, that is once per 10 CPU clock periods.

**Claims.**
- Graded whole here, p353: opus:F-signals.c2, sol:F-signals.c70.
- P5 also grades the φ2 input-mode settings of sol:F-cra.c13 and opus:F-cra.c12 (p349), sol:F-crb.c7 (p349), and sol:F-crb.c10 and opus:F-crb.c13 (p350). P10 carries those ids.
- opus:F-signals.c1 and sol:F-signals.c69 are declined whole (Section 0).

**Instances.** Four timers (CIAA A and B, CIAB A and B), on the harness's one clock (1.6).

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

**Claims.** None. The two claims whose bus-timing clause it checks, opus:F-signals.c1 and sol:F-signals.c69 (p353), are declined whole (Section 0). P6 is run and graded all the same: it is the check that F2's E-index frame holds for a loop. Its sources are the Clock Input and CS paragraphs on p353 and the DB7-DB0 paragraph on p354.

**Stimulus.** Environment (1.2), then three loops of 200 iterations, each iteration reading one byte and storing it (E6):
- **K1:** the byte is read from CIAA PRB.
- **K2:** the byte is read from CIAB PRB.
- **K3 (contrast, graded in the same run):** the same instruction reads a Chip-memory byte instead ($0C4000, E9). The builder pads the loop body so that the iteration time I3 is not a multiple of 10·T_CPU.

The R-SAMPLE loops used in P4, P5(b) and P9 (including P2 leg S's loop, which P9 leg W reuses) are checked the same way, once for each store instruction in the body.

**Observation.** The store times t_i. Their phases φ_i = (t_i − t_0) mod T_E, folded into [−T_E/2, T_E/2).

**Expected.**
- **K1, K2 and every R-SAMPLE loop:** the φ_i of one store instruction lie within a band no wider than 2·T_CPU. They take at most two values, 2·T_CPU apart.
- **K3:** its φ_i take all five values 0, 2, 4, 6 and 8·T_CPU mod T_E, a spread of 8·T_CPU, so the 2·T_CPU band does not hold for it. This is a bound derived from the pages below, not an order.

Why the band is 2·T_CPU:
- "A low level on CS while 02 is high causes the device to respond to signals on the R/W and address (RS) lines" (p353), and the data pins drive "unless CS is low and R/W and 02 are high" (p354). So each transfer occupies the φ2-high part of an E cycle and ends at a fixed phase of E.
- "The 68000 uses only the even-numbered memory access cycles" (p196), and Figure 6-10 (p196) marks the odd cycle "assigned to other devices", the even cycle "available to the 68000". A memory cycle is 2·T_CPU (F1), so the 68000's cycles come every 4·T_CPU.
- T_E is 10·T_CPU, five memory cycles, an odd number. The 68000's cycles therefore fall at one of two offsets from the E cycle, alternating from one E cycle to the next, one memory cycle (2·T_CPU) apart.
- So the first Chip-memory access after a CIA transfer lands at one of two phases 2·T_CPU apart. The fixed code after it runs on the same 4·T_CPU grid, and keeps that offset up to the store.
- Nothing else moves it: refresh and the other devices take the odd cycles (Figure 6-10), and with DMA off (EP2) no device takes an even one.

Why K3 spreads: every access of K3's loop, which contains no CIA access, is on the 68000's 4·T_CPU grid (p196), so I3 is a whole multiple of 4·T_CPU. With I3 mod T_E not 0 (Pre6.1), I3 mod T_E is 2, 4, 6 or 8·T_CPU. Each shares only the factor 2 with 10, so five consecutive stores already take all five phases 0, 2, 4, 6 and 8·T_CPU.

**Preconditions.**
- **EP2 and the F1 anchor.**
- **Pre6.1 (K3).** The measured I3 mod T_E is not 0. *Contrast:* padding that makes I3 a multiple of 10·T_CPU fails this precondition.

**Contrasting case.** K3 is the contrast to K1 and K2: the same loop with the CIA read replaced by a Chip-memory read, which no page ties to E. It is graded in the same run, and the band that K1 and K2 must meet does not hold for it.

**Reference frame.** F1 and F2.

**Left open.** The page does not state the E duty cycle, the number of E cycles one access takes, or how many accesses one E cycle can serve. None of these is graded.

---

### P7: The CRA bit layout (bits 0, 3, 4, 5, 6, 7)

Bits 1 (PBON) and 2 (OUTMODE) of CRA are graded in P1, P3 and P4.

**Claims.** None is graded whole by one sub-proposition. Each sub-proposition grades one field of the CRA bitmap claims opus:F-cra.c12 and opus:F-cra.c13 (p349: "E CRA unused 0=input 0=02 1=force 0=cont. 0=pulse 0=PB6OFF 0=stop / unused 1=output 1=CNT load 1=one- 1=toggle 1=PB6ON 1=start / (strobe) shot"); P7.5 also grades the CNT half of sol:F-cra.c13 (p349, INMODE). P10 carries these ids. The bitmap is read from the printed image, because the text layer prints "=" as "*" and "1" as "l". Each sub-proposition applies to CIAA and CIAB unless stated: "There are two control registers in the 8520, CRA and CRB" (p348), and the system has two 8520s (p339).

#### P7.0: Bit 0 is START (1 start, 0 stop)

- **Stimulus.** CR ← $00. R-LATCH($8000). Read TAHI, TALO → u1. A delay of about 1000 E periods between markers. Read → u2. Bracketed write CR ← $01. A delay of about 1000 E periods. Bracketed write CR ← $00. Read → u3.
- **Expected.** "1 = start Timer A, 0 = stop Timer A." (p348).
  - u1 = u2 = $8000: stopped, the counter holds, and the latch write loaded it (p343).
  - u3 < u2. The size of u2 − u3 is reported against τ/T_E (P5 grades the rate).
- **Precondition.** No underflow (ICR TA = 0 at the end). *Contrast:* R-LATCH($0100) makes TA = 1.
- **Contrasting case.** CR ← $01 during the first delay makes u2 ≠ u1.

#### P7.3: Bit 3 is RUNMODE (1 one-shot, 0 continuous)

- **Stimulus.** Two legs, one run each, identical except for the start value, which comes from the parameter block (E7).
  - *Leg O (one-shot):* CR ← $00. R-LATCH(40). Read ICR (discard). CR ← $09 (START, one-shot). R-SAMPLE(400; ICR). Read CRA → c. Read TAHI, TALO → u.
  - *Leg C (continuous):* the same with CR ← $01 (START, continuous) as the start write.
- **Expected.** Source: "1 = one-shot mode, 0 = continuous mode." (p348); "In one-shot mode, the timer will count down from the latched value to zero, generate an interrupt, reload the latched value, then stop. In continuous mode, the timer will count down from the latched value to zero, generate an interrupt, reload the latched value, and repeat the procedure continuously." (p342); and "This bit is automatically reset (= 0) when underflow occurs during one-shot mode." (p348).
  - Leg O: exactly one ICR sample has TA = 1; c bit 0 = 0; u = 40.
  - Leg C: more than one ICR sample has TA = 1, and c bit 0 = 1 (START is reset only by an underflow "during one-shot mode"). u is reported, not graded: the counter is running when it is read.
- **Precondition.** Consecutive ICR stores are at most 20 E periods apart (P/2 for any P ≥ 40, F3), from their store times (F1, F2), so that a timer that repeats shows more than one flag. *Contrast:* a loop body padded beyond 20 E periods fails it.
- **Contrasting case.** Each leg is the other's. CR ← $01 in leg O gives more than one TA sample, so "exactly one" fails; CR ← $09 in leg C gives exactly one, so "more than one" fails.

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

- **Stimulus, CIAA.** CR ← $00. R-LATCH($1000). Read ICR (discard). CR ← $21 (START, CNT). "Ready" marker. R-KEY. The program waits W E periods, with 1500 ≤ W ≤ 3000 (the 8 bits take about 8 × 60 µs, about 340 E periods, per p358; the upper bound keeps the φ2 contrast below the latch). Read ICR → i. Bracketed write CR ← $00. Read TAHI, TALO → v.
- **Stimulus, CIAB.** The same without R-KEY (CIAB CNT is POUT, which nothing drives).
- **Expected.** Source: "1 = Timer A counts positive CNT transitions" (p348), with CNT = KCLK (p351) and "a pulse on KCLK (low then high)" per bit, "until all eight data bits have been sent" (p357).
  - CIAA: $1000 − v = 8.
  - CIAB: v = $1000.
- **Preconditions.**
  - KP (its probe is timer B, CRB ← $21). *Contrast:* KP's own contrast.
  - **Window (CIAA).** The whole transmission lies between the start write and the stop write.
    - It starts after the start write: the test sends the code d_k ≥ 100 E periods after the ready marker, which follows the start write, as the test's own send time against the marker's store time shows.
    - It has ended before the stop write: i, read before the stop write, has SP = 1. The serial port is in input mode (CRA bit 6 = 0), and "After eight CNT pulses, the data in the shift register is dumped into the serial data register and an interrupt is generated." (p345).
    - *Contrast:* send the key after the stop write. i then has SP = 0.
- **Contrasting case.** CR ← $01 (bit 5 = 0, φ2): $1000 − v = round(W/T_E) ± 2, on both CIAs.
- **Left open.** The SDR value is reported, not graded. What it should hold depends on the keyboard's bit order and polarity (p357-p358), which are claims outside this package.

#### P7.6: Bit 6 is SPMODE (1 serial port output, 0 input)

- **Stimulus, CIAB.** CR ← $00. R-LATCH(10). Read ICR (discard). CR ← $41 (START, SPMODE output, continuous). SDR ← $A5. R-SAMPLE(300; ICR). CR ← $00.
- **Expected, CIAB.** Source: "1 = Serial port=output (CNT is the source of the shift clock)" (p348), and p345: "Transmission will start following a write to the serial data register. [...] After eight CNT pulses, an interrupt is generated". At least one sample has SP = 1. The number of TA flags before the first SP flag is reported, not graded: its value, 16 by "Data is shifted out on the SP pin at 1/2 the underflow rate of Timer A" (p345), belongs to claims outside this package.
- **CIAA.** Only the input setting is exercised. SP = 1 in i, the ICR read that P7.5's CIAA run (CR ← $21, bit 6 = 0) makes before its stop write, is this leg's positive result; P7.5 uses the same value as its window precondition. Its contrast is the same run without R-KEY: SP = 0. CIAA is not put into output mode, because its SP and CNT are the keyboard's KDAT and KCLK (p351), and the host drives KDAT as the keyboard handshake (p357).
- **Precondition (CIAB).** Timer A underflows during the window: at least 20 samples have TA = 1. *Contrast:* CR ← $40 (not started) gives no TA and no SP.
- **Contrasting case (CIAB).** CR ← $01 (bit 6 = 0, input): SP = 0 in every sample. In input mode, data shift only "on the rising edge of the signal applied to the CNT pin" (p345), and CIAB's CNT (POUT) is not driven.

#### P7.7: Bit 7 is unused (unaffected by a write, 0 on a read)

- **Stimulus.** CR ← $80. Read CRA → c1. R-LATCH($8000). Bracketed write CR ← $81. Read CRA → c2. A delay of about 1000 E periods. Bracketed write CR ← $00. Read → w81. A second run is identical except that it writes $01 instead of $81, giving c2' and w01.
- **Expected.** Source: "All unused register bits are unaffected by a write and forced to 0 on a read." (p349).
  - c1 = $00.
  - c2 = $01.
  - |w81 − w01| ≤ 1: the timer behaves as it does with $01.
- **Precondition.** Both runs have the same bracket-to-bracket time within 1 E period. *Contrast:* a different delay count D breaks this.
- **Contrasting case.** One for each half of the page's sentence.
  - *Forced to 0 on a read:* CR ← $02 (PBON, a used bit; the timer stays stopped, so R-LATCH still applies) in place of CR ← $80. A used bit reads back (Section 3, item 7), so the page predicts c1 = $02, and c1 = $00 fails.
  - *Unaffected by a write:* CR ← $21 (START and INMODE, a used bit) in place of CR ← $81. The timer then "counts positive CNT transitions" (p348), and there are none (no key is sent: KP on CIAA; CIAB's CNT is not driven, P7.5), so w21 = $8000, far from w01, and |w − w01| ≤ 1 fails.
- **Control.** c2' = $01 in the second run: a used bit reads back set while the timer runs. p348 describes START being cleared only by a one-shot underflow.

---

### P8: The CRB bit layout (bits 0, 3, 4, 6-5, 7)

Bits 1 (PBON) and 2 (OUTMODE) of CRB are graded in P1, P3 and P4.

**Claims.** None is graded whole by one sub-proposition. Each sub-proposition grades one field of the CRB bitmap claims opus:F-crb.c13 and opus:F-crb.c14 (p350, read from the printed image: "F CRB 0=TOD 00=02 1=force 0=cont. 0=pulse 0=PB7OFF 0=stop / 1=Alarm 01=CNT load 1=one- 1=toggle 1=PB7ON 1=start / 10=Timer A (strobe) shot / 11=CNT+ Timer A"); P8.65 also grades modes 01, 10 and 11 of sol:F-crb.c10 (p350) and sol:F-crb.c7 (p349, the INMODE table). P10 carries these ids. Both CIAs unless stated.

#### P8.0, P8.3, P8.4: START, RUNMODE, LOAD

These are specified exactly as P7.0, P7.3 and P7.4, with these substitutions: CRB for CRA, TBLO and TBHI for TALO and TAHI, and ICR bit 1 (TB) for bit 0 (TA). CRB bits 6-5 = 00 (φ2) and bit 7 = 0 in every written value. The expected values, preconditions and contrasting cases carry over unchanged, with the CRB text on p349 as the source: "1 = start Timer B, 0 = stop Timer B. This bit is automatically reset (= 0) when underflow occurs during one-shot mode.", "1 = one-shot mode, 0 = continuous mode.", and "1 = force load (this is a strobe input, there is no data storage; bit 4 will always read back a zero and writing a 0 has no effect.)".

#### P8.65: Bits 6-5 are INMODE (00 φ2, 01 CNT, 10 timer A underflows, 11 timer A underflows while CNT is high)

Source: "CRB6 CRB5 Mode Selected: 0 0 Timer B counts 02 pulses; 0 1 Timer B counts positive CNT transitions; 1 0 Timer B counts Timer A underflow pulses; 1 1 Timer B counts Timer A underflow pulses while CNT pin is held high." (p349). p343 agrees.

- **Mode 00.** P5, the timer-B instances.
- **Mode 01.**
  - *CIAA:* CRB ← $00. R-LATCH_B($1000). CRB ← $21. "Ready" marker. R-KEY. Wait W E periods, with 1500 ≤ W ≤ 3000. Read ICR → i. Bracketed write CRB ← $00. Read TBHI, TBLO → v. Expected: $1000 − v = 8.
  - *CIAB:* the same without R-KEY. Expected: v = $1000.
  - Preconditions as in P7.5: KP, with timer A as its probe (CRA ← $21), and on CIAA the window (the code sent after the ready marker; SP = 1 in i, CRA staying $00 so that the serial port is in input mode). *Contrasting case:* CRB ← $01 gives $1000 − v = round(W/T_E) ± 2.
- **Mode 10 (both CIAs).**
  - *Stimulus:* CRA ← $00; R-LATCH_A(30); CRB ← $00; R-LATCH_B($FFFF). Read ICR (discard). CRB ← $41 (START, INMODE 10). CRA ← $01. R-SAMPLE(300; ICR). CRA ← $00. Read ICR (final). CRB ← $00. Read TBHI, TBLO → v.
  - *Expected:* $FFFF − v = N_A, the number of ICR reads after CRA's start write, including the final one, that have TA = 1.
  - *Precondition:* the iteration spacing is ≤ P(30)/2, so each TA flag is exactly one underflow. *Contrast:* with R-LATCH_A(2), flags appear in consecutive reads.
  - *Contrasting case:* CRB ← $01 (mode 00) gives $FFFF − v ≥ (window/T_E) − 2, far more than N_A. Leaving CRA at $00 is not a contrast: v = $FFFF and N_A = 0 still satisfy the expected equality, and with no TA flag the precondition cannot be measured.
- **Mode 11 (CIAA only; CIAB's CNT has no driver).**
  - *Stimulus:* two pairs of runs. In each pair, runs X and Y are identical except for the CRB value, which comes from the parameter block (E7): X writes $41 (mode 10), Y writes $61 (mode 11). Every run: CRA ← $00; R-LATCH_A(3); CRB ← $00; R-LATCH_B($FFFF); CRB ← value; bracketed write CRA ← $01; "ready" marker; wait W E periods, with 1500 ≤ W ≤ 3000; read ICR → i; bracketed write CRA ← $00; CRB ← $00; read TBHI, TBLO. The decrements are D_X and D_Y. Pair K sends R-KEY, at the same d_k in both runs; pair N sends no key.
  - *Expected:* only the order is graded.
    - Pair N: D_Y = D_X. Without a key, CNT is high throughout ("Before the transmission starts, both KCLK and KDAT are high", p357), so mode 11 counts every timer A underflow that mode 10 counts: "1 0 Timer B counts Timer A underflow pulses; 1 1 Timer B counts Timer A underflow pulses while CNT pin is held high" (p349).
    - Pair K: D_Y < D_X. With a key, KCLK goes low once per bit ("a pulse on KCLK (low then high)", p357) for "about 20 microseconds" (p358), about 14 E periods at 1/10 of the CPU clock (p353, p2), while timer A with latch 3 underflows every 3 or 4 E periods (F3; Section 3, item 1). Underflows then fall while CNT is low, and mode 11 does not count them.
    - The size of D_X − D_Y is reported, not graded: no page gives the low time exactly, and no port shows KCLK.
  - *Preconditions:*
    - KP, with timer A as the probe: its own run is the pair's program without R-KEY and with CRA ← $21 in place of CRA ← $01, and timer A is unchanged. *Contrast:* its own.
    - Pair K: the window of P7.5, with i read before CRA's stop write: the code is sent after the ready marker, and i has SP = 1 (CRA bit 6 is 0, input mode). *Contrast:* send the key after CRA's stop write; i then has SP = 0.
    - In each pair, the two runs' windows, from the marker after CRA's start write to the marker after its stop write, are equally long in system clock cycles, so that modes 10 and 11 are offered the same timer A underflows. *Contrast:* a different wait W in run Y breaks it.
  - *Contrasting case:* each pair is the other's. Sending the key in pair N gives D_Y < D_X, and "D_Y = D_X" fails; leaving it out of pair K gives D_Y = D_X, and "D_Y < D_X" fails.

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

**Claims.** None graded whole here. P9 grades the divider sentence of sol:F-timers.c8, and the "variable width pulses" and "variable frequency waveforms" of both sol:F-timers.c8 and opus:F-timers.c6 (p341): "This latch (prescalar) value is a divider of the input clocking frequency." [...] "Various timer operating modes allow generation of long time delays, variable width pulses, pulse trains, and variable frequency waveforms." P10 carries both ids.

**Instances.** The four of P1. The sentence is about the latch of every timer: "Each interval timer consists of a 16-bit read-only timer counter and a 16-bit write-only timer latch" (p341).

**Stimulus.**
- **Leg F (frequency).** P3's run, with L ∈ {40, 41, 64, 100}. Where 600 iterations do not cover the 16 flags F3 needs (L = 100 with a short loop body), the sampler is made longer, within the result area (E9); F3's own condition is the guard.
- **Leg W (width).** P2 leg S, with L ∈ {40, 100} and R-SAMPLE long enough to pass the underflow.

**Observation.**
- P(L) from the ICR stores (F3).
- The toggle half-period: the E-index difference between consecutive PRB transitions.
- W(L): the E-index of the first bit-n = 0 sample that follows a bit-n = 1 sample of the same start, minus the E-index of that start's marker. A 0 read before the output has gone high is not its end.

**Expected.**
- δ = P(L) − L is the same for all four L, and δ ∈ {0, 1}.
- Consecutive PRB transitions are P(L) ± s E periods apart, where s is the PRB sampling spacing in E periods.
- W(100) − W(40) = 60 ± s.

Why δ is 0 or 1: p342 says the timer will "count down from the latched value to zero, generate an interrupt, reload"; p346 calls an underflow "timer counts down past 0". The pages leave open which of L or L+1 the period is, so only δ's constancy and range are graded.

**Preconditions.** Leg F: those of P3 (Pre3.1, Pre3.2) and of F3. Leg W: P2's Pre2.2 and Pre2.3, and the E-index frame for leg S's loop (P6's phase check). *Contrasts:* those of P3, F3, P2 and P6.

**Contrasting case.**
- Changing L by Δ changes the period by exactly Δ: every pair of latch values in leg F is a contrast to the others. The run for L = 41 made with a latch of 42 gives a δ one larger than the other three, and "the same for all four L" fails.
- Leg W: the L = 40 run made with a latch of 20 gives W(100) − W(20) = 80, outside 60 ± s. (A larger latch in the L = 100 run could outlast its sampler and leave W undefined.)

A run counting CNT with no input (CR ← $27, or CRB ← $27) is not a contrast: with no underflow, Pre3.2 and F3 fail and the run is not graded instead of red.

**Reference frame.** F2 and F3.

---

### P10: Claims that several propositions grade together

Each claim below states more than any one of P1-P9 grades, so no one of them carries its id. The id is carried by an assertion of its own, one per row, per core:
- it passes when every assertion it lists passed on that core;
- it fails when any of them failed;
- otherwise it is not graded, and its ids go on a record line that names the listed assertion that was not graded.

| Claim | Page | What it states | Graded whole by |
|---|---|---|---|
| sol:F-timers.c13 | 342 | one bit selects toggle or a one-cycle positive pulse on every underflow; the toggle output is set high on start and low by RES | P2, P3, P4 (all four instances each) |
| opus:F-timers.c12, opus:F-timers.c13 | 342 | one bit selects the output while PB on/off is on; every underflow toggles it or gives a one-cycle positive pulse | P3, P4 (all four instances each) |
| sol:F-cra.c16 | 349 | CRA OUTMODE: 0 pulse, 1 toggle | P3, P4 (timer-A instances) |
| opus:F-crb.c3, sol:F-crb.c4, sol:F-crb.c13 | 349, 350 | CRB OUTMODE: 0 pulse, 1 toggle | P3, P4 (timer-B instances) |
| sol:F-cra.c13 | 349 | CRA INMODE: 0 φ2, 1 CNT | P5 (timer-A instances), P7.5 |
| sol:F-crb.c7, sol:F-crb.c10 | 349, 350 | CRB INMODE: 00 φ2, 01 CNT, 10 timer A underflows, 11 timer A underflows while CNT is high | P5 (timer-B instances), P8.65 modes 01, 10 and 11 |
| opus:F-cra.c12, opus:F-cra.c13 | 349 | every field of the CRA bitmap | P7.0 (START); P1, timer-A instances (PBON); P3 and P4, timer-A instances (OUTMODE); P7.3 (RUNMODE); P7.4 (LOAD); P5, timer-A instances, and P7.5 (INMODE); P7.6 (SPMODE); P7.7 (bit 7) |
| opus:F-crb.c13, opus:F-crb.c14 | 350 | every field of the CRB bitmap | P8.0 (START); P1, timer-B instances (PBON); P3 and P4, timer-B instances (OUTMODE); P8.3 (RUNMODE); P8.4 (LOAD); P5, timer-B instances, and P8.65 (INMODE); P8.7 (ALARM) |
| opus:F-timers.c6, sol:F-timers.c8 | 341 | the modes "allow generation of long time delays, variable width pulses, pulse trains, and variable frequency waveforms"; sol:F-timers.c8 adds that the latch "is a divider of the input clocking frequency" | P4 (pulse trains); P9 leg F (variable frequency, and the latch as divider) and leg W (variable width); P8.65 mode 10 (timers "linked for extended operations", p341: long delays) |

The sentence of opus:F-timers.c6 names no value. Each capability it lists is shown by the proposition named in its row, and nothing else of it is graded.

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
7. **Read-back of used control-register bits** is not stated directly. p340 ("16 registers that you may read or write") and p348 (START "automatically reset (= 0)" on a one-shot underflow; LOAD "will always read back a zero") imply that used bits read back. These depend on it: P7.7's c2 = $01, its first contrasting case and its control; leg C of P7.3 and of P8.3 (c bit 0 = 1); and EP1's contrast. The polls of P1 step 4 and P2 leg S also read START; a poll that ended at once would leave the i1 or i_k flag at 0, and Pre1.2 or Pre2.2 would then fail.
8. **The CIAA TOD source** is "VSync or line tick" (p339). P8.7 grades CIAA only in a way that holds for either source.
9. **p352's example** sets one-shot mode before writing TAHI, so its TAHI write already starts the timer (p342). Its DMACON write carries the comment "Kill all chip interrupts". This spec takes from p352 only the latch write order.
10. **The claims' text readings of the bitmaps on p349 and p350** print "=" as "*" or "-" and "1" as "l". The printed images confirm the readings used here.

---

## 4. Build

`floppy_config` is left at the harness's own documented default (`nanomig_harness.h`, `Config::floppy_config = 0x00`) for every run, including the O2-grading runs. That value is one drive, DF0, write-enabled, at normal speed (the harness comment states it this way). No disk image is mounted for any run (no claim in this package needs one).

O2 (the drive LED) is not graded at this port: fdd_led tracks disk DMA, not MTR* latched at a drive's own select (NM-HRM-046). O2's own legs and its calibration precondition, LP, are recorded as measurements, not graded; see section 5.

## 5. What was measured, P1 and P2

Both cores agree on every value in this section; nothing here separates fx68k from TG68K.

**Pre1.4/O1, per instance.** d1 and its own contrast (data driven to 1) were read on all four instances. CIAA/timer A and CIAA/timer B both read $FF for every leg regardless of DDRB or the data register (d1 = $FF, contrast = $FF): the precondition fails on both, and CIA-A's own read-back is recorded rather than graded further. CIAB/timer A and CIAB/timer B read d1 = 0 and the contrast = 1 on bit n, as the page predicts: the precondition holds, and P1's PBON legs and P1-OTHERBITS' pattern-bit clause are graded on these two instances only.

**P1-MAIN (r0, a2, a1, b1, c1, d1, o1c, d2), per instance:**

| instance | r0 | a2 | a1 | b1 | c1 | d1 | o1c | d2 |
|---|---|---|---|---|---|---|---|---|
| CIAA/timer A (not graded) | $FF | $FF | $FF | $FF | $FF | $FF | $FF | $FF |
| CIAA/timer B (not graded) | $FF | $FF | $FF | $FF | $FF | $FF | $FF | $FF |
| CIAB/timer A | $FF | $FF | $FF | $FD | $BB | $BB | $FB | $FB |
| CIAB/timer B | $FF | $FF | $FF | $FD | $7B | $48 | $FB | $FB |

On the graded instances (CIAB), a2, a1, b1 and c1 read back exactly the DDRB/PR-data value the step itself wrote, never the timer's own toggle/pulse state: bit n of b1 ($FD, $FD) and c1 ($BB, $7B) is the data bit both times, not the override. d1 and o1c (Pre1.4's own two legs), which do not depend on PBON's own override, read as the page predicts (0 then 1 on bit n). P1-MAIN-CRA, P1-MAIN-CRB and P1-MAIN are red on both cores.

**P1-OTHERBITS.** On the graded instances (CIAB), the pattern bits of b1 and c1 (bits 0-5) and bit o both match this instance's own written pattern and r0: green on both cores. On CIA-A (not graded), b1 = c1 = $FF on both instances, against a pattern of $2A/$15 in bits 0-5 -- recorded, not graded.

**P1-CONTROL (PBON = 0).** Graded on the same instances as P1-MAIN (CIAB only). Both instances read a2, a1, b1 bit n = 1, c1 bit n = 0, d1 bit n = 0, d2 bit n = 1, exactly the page's own PBON = 0 prediction: green on both cores. This is the green control P1-MAIN's own red needs: the same comparator, a different (and page-predicted) input, passes.

**P2-LEGR, P2-MAIN, P2-LEGH (r1, z1-z4, h1-h3, s0), per instance:**

| instance | r1 | z1 | z2 | z3 | z4 | h1 | h2 | h3 | s0 |
|---|---|---|---|---|---|---|---|---|---|
| CIAA/timer A (not graded) | $FF | $FF | $FF | $FF | $FF | $FF | $FF | $FF | $FF |
| CIAA/timer B (not graded) | $FF | $FF | $FF | $FF | $FF | $FF | $FF | $FF | $FF |
| CIAB/timer A | $FF | $BF | $BF | $BF | $BF | $BF | $BF | $BF | $BF |
| CIAB/timer B | $FF | $7F | $7F | $7F | $7F | $7F | $7F | $7F | $7F |

On CIA-A, r1 and s0 read $FF (the dead read-back): Pre2.3 never holds there (the level before start 1 is never 0), so no start of leg S is graded, and legs R and H are excluded the same way P1-MAIN excludes CIA-A (Pre1.4/O1, reused). On CIAB, s0 and every z_k read the driven 0 leg S's own override wrote (bit n = 0 throughout, both instances): Pre2.3 holds for every start, so every start is graded, and every one reads constant at 0 for all 64 samples -- no rise, "at least one 1" and "exactly one rise" both fail. r1 reads bit n = 1 (pulled up, DDRB never drives it in leg R): leg R's own clause fails. h1, h2 and h3 all read the same driven 0 as z_k: leg H's own clause ("h1 = 1", "h3 = 1") fails, and h2 shows stopping does not change the level either. P2-LEGR, P2-MAIN, P2-LEGH and the combined P2-TOGGLE are red on both cores, on the CIAB instances; CIA-A's exclusion is recorded via the Pre1.4/O1 record P1-MAIN already makes.

**P1-LP, P1-O2, LED-R.** LP's own three legs were measured on CIAB/timer B: the LED reads off when the CPU leaves reset, off after the "on" leg's R-SELECT (MTR* = 0 at select), and off after the "off" leg's R-SELECT (MTR* = 1 at select) -- off throughout, because this package starts no disk DMA, the same result the fdd_led probe gives independently (it follows disk DMA, not the motor latch, NM-HRM-046). LP, the four O2 legs and P2's LED-R are recorded, not graded; nothing is graded on the light.

**P1-OTHERBITS' own mutation, P1-PRE1.1-1.4, P2-PRE2.2-2.4, EP1, EP2, EP3.** Replayed on both cores against the built binaries and diffed line-for-line against the unmutated run. Every one of the twelve flips exactly the assertion its own `// MUTATION` record names, and nothing else, except the two side effects named below:
- P1-OTHERBITS: red (bits 0-5 of b1 no longer match this instance's own pattern). No other line changes.
- P1-PRE1.1: red (r0 bit n reads 0, not 1). No other line changes -- P1-PRE1.4/O1, which also reads a bit-n level, is unaffected: the mutation only pre-drives the line before r0's own read, and DDRB/PRB are already set again (identically) before d1 and its own contrast are reached.
- P1-PRE1.2: red (the $FFFF latch leaves CR bit 0 still set at the poll limit). No other line changes.
- P1-PRE1.3: red (the $0004 latch lets leg C underflow). No other line changes.
- P1-PRE1.4/O1 (CIAB/timer B): red (the contrast leg's data bit stays 0, so it no longer reads 1). Side effect, measured: P1-MAIN-CRB flips from FAIL to PASS, because both of its instances (CIAA/timer B, always excluded, and now CIAB/timer B) are excluded from the conjunction, which is then vacuously true.
- P2-PRE2.2: red (the $FFFF leg-S latch leaves start 1's poll count at the limit and its own TB flag clear). No other line changes.
- P2-PRE2.3: red (start 1 written as $0D with DDRB left an input reads z_1 = 1 instead of the baseline's own driven 0). Side effect, measured: this same mutant leaves DDRB undriven for all of leg S, not only start 1, so CIAB/timer B's own s_4 also reads pulled-up throughout instead of driven 0; P2-CONTRAST's own comparator, which reads that same capture as its baseline, then finds one rise where the normal baseline finds none, and grades a PASS where it is RECORDED (unfalsifiable) at the normal baseline.
- P2-PRE2.4: red (the $0008 leg-H latch sets the ih flag). No other line changes.
- EP1: red (a write to CIAA CRA before SETUP's own read returns $0A instead of $00). No other line changes -- EP1-PER-RUN, read from the eight P1/P2 instance captures rather than this dedicated one, is unaffected.
- EP2: red (Copper DMA started in the window makes the Copper's own fetch of $0C3000 appear). No other line changes.
- EP3: red (the never-DONE program has no final marker). No other line changes.

P2-PRE2.1's own mutation (an inserted start before leg R, `NMH_MUTATE=P2-PRE21`) changes nothing anywhere in the run: it stays exactly the record its own text states, on both cores. P2-LEGH's own START=0 mutation (`NMH_MUTATE=P2-LEGH0`) likewise changes nothing: 19 passed, 7 failed, identical to the baseline, on both cores.

P2-CONTRAST's own mutation (`NMH_MUTATE=P2-CONTRAST`) leaves s_4's own rise count at 0 (unchanged from the baseline, matching its own record) but has a measured side effect: leaving start 4 unstarted means it never underflows, so P2-PRE2.2 -- which checks the ICR flag for all four starts of all four instances -- reads FAIL under this mutation where it reads PASS at baseline: 18 passed, 8 failed under the mutation against the baseline's 19 passed, 7 failed, on both cores. This is a genuine consequence of the mutation (start 4 truly never underflows), not a fixture defect.

**F1-ANCHOR.** Holds on every run measured; F1-ANCHOR-CONTRAST (the same comparator fed 1.25x the measured T_CPU) fails on every run measured, both cores; no separate binary is built for it.

**Records that stay unfalsifiable at this boundary, both cores:** P2-PRE2.1 (an inserted start before leg R reads the same pulled-up r1 as the baseline), P2-CONTRAST (start 4 written unstarted reads the same 0-throughout as the baseline, since this build's s_4 never rises for its own reason), and P2-LEGH-CONTRAST (h1 reads the same driven 0 whether started or not). Each record states the baseline state the mutation could not move.

**P3-P10.** Both cores agree on every value below except where stated. fx68k: 73 passed, 34 failed. TG68K: 72 passed, 35 failed (P8.7-RUNX only).

**P7.0/P8.0 (START).** Both CIAs, both registers: u1 = u2 = $8000, u3 = $7424. Green on both cores; P70-PRE and P80-PRE (no underflow before the stop read) hold.

**P7.3/P8.3 (RUNMODE).** Leg O: exactly one flag, START clears (c bit 0 = 0). Leg C: 48 flags, START stays set. Green on both cores, on both CIAs and both registers. P7.3-CONTRAST/P8.3-CONTRAST (each leg graded by the other's own comparator) are green: leg O's "exactly one" fails on leg C's 48 flags, and leg C's "more than one" fails on leg O's single flag. P7.3-PRE/P8.3-PRE (consecutive ICR stores at most 20 E periods apart, CIAB) are green; the padded-loop mutant (`NMH_MUTATE=P73-PRE`/`P83-PRE`) is red through that clause alone.

**P7.4/P8.4 (LOAD).** Both CIAs, both registers: v1 = $3819, v2 = $4000, c = $00, v3 = $FF91. Green on both cores. P7.4-LD0/P8.4-LD0 (CR ← $00 instead of $10) is green: v2 = v1. P7.4-CONTRAST/P8.4-CONTRAST (CR ← $01 instead of $11, no LOAD) is green: $FFFF-v3 is at least tau_total/T_E-2, far above the LOAD-strobe bound.

**P7.7 (unused bit 7).** Both CIAs: c1 = $00, c2 = $01, w81 = w01 = $02 (|w81-w01| = 0). Green on both cores, and P7.7-PRE (the two runs' bracket-to-bracket time matches within about 1 E period) is green. P7.7-C1CONTRAST (CR ← $02 instead of $80) is green: c1 no longer reads $00. **P7.7-WCONTRAST (CR ← $21 instead of $81) is red on both cores**: w21 does not differ from w01 by more than 1 -- the same bit-5 (INMODE) gap that P7.5 and P8.65 mode 01 measure directly (below) means a used bit that ought to change the timer's behaviour does not, so this contrast cannot go red through the clause it targets.

**P7.5 (CRA INMODE) / P8.65 mode 01 (CRB INMODE).** CIAA: i has SP = 1 (green, P7.5-WINDOW/P8.65-MODE01-WINDOW); v = $0044, so $1000-v = 4028, not 8. CIAB: v = $0044 also, not $1000. **P7.5 and P8.65-MODE01 are red on both cores**: CIAA's timer does not decrement by 8, and CIAB's does move. The phi2 contrast (CR ← $01 instead of $21/STARTVAL=$21) reads the identical v = $0044 on CIAA and CIAB, both instances: the "CNT" setting and the "phi2" setting produce the same decrement, so **P7.5-CONTRAST and P8.65-MODE01-CONTRAST are also red** -- the comparator that should separate them cannot, because the two inputs are not in fact different at this port. **KP-A and KP-B are red for the same reason**: the probe timer (the other timer on CIAA, started the same way) decrements identically whether or not R-KEY is sent (v = $0048 in both the no-key and with-key runs), so KP-A-CONTRAST and KP-B-CONTRAST are also red.

**P7.6 (SPMODE).** CIAA: reuses P7.5's own i (SP = 1); green, and P7.6-CIAA-CONTRAST (the same run without R-KEY, SP = 0) is green. CIAB: 136 of 300 ICR samples have TA = 1 (green precondition, P7.6-CIAB-PRE), but no sample ever has SP = 1. **P7.6-CIAB is red on both cores.** P7.6-CIAB-CONTRAST (input mode, SP = 0 throughout) and P7.6-CIAB-PRECONTRAST (not started, no TA and no SP) are both green.

**P8.65 mode 10.** Both CIAs: nA = 48 (ICR reads with TA = 1, including the final one), $FFFF-v = 48: green on both cores. The measured period is P(30) = 30 on both CIAs, exactly matching the latch, but **P8.65-MODE10-PRE is red on both cores**: not every consecutive gap between flagged E-indices is within 1 of 30 (a few samples land on a merged or split interval), so the period is not "uniquely determined" under this run's own R-SAMPLE granularity, even though its mode is exact. P8.65-MODE10-CONTRAST (CRB ← $01, mode 00) is green: $FFFF-v is far above N_A.

**P8.65 mode 11 (CIAA only).** Pair N (no key): D_X = D_Y = 559 on both cores -- green (P8.65-MODE11-PAIRN). Pair K (key sent): D_X = D_Y = 559 also. **P8.65-MODE11-PAIRK is red on both cores**: mode 11 does not stop counting while CNT is held low, the same INMODE gap P7.5/P8.65-mode-01 measure. P8.65-MODE11-WINDOW (SP = 1 in both pair-K runs) is green.

**P8.7 (ALARM).** CIAB: T0 = 111 (fx68k) / 133 (TG68K), matching the lines elapsed since the TOD ← 0 write within 1 on both cores (P8.7-PRE green; the LSB-never-written mutant, `NMH_MUTATE=P87-PRE`, is red through that clause). Run X: T1X-T0 = 1, matching the one line elapsed; the ALRM flag appears only from the first TODLO sample equal to A&$FF, within one iteration, **on fx68k (P8.7-RUNX green)**; **on TG68K the same run's P8.7-RUNX is red** -- T1X-T0 still matches the lines elapsed, but the ALRM-to-TODLO-match timing that "within one iteration" grades does not hold on this core, whose loop iterations take a different number of clk_sys cycles than fx68k's. Run Y: T1Y = $1000 (no lines elapsed, no delay loop between the marker and the read) and no ALRM flag, on both cores (P8.7-RUNY green). CIAA: run X gives T1X-T0 = 0, run Y gives T1Y-A = 0, both in [0,2] on both cores (P8.7-CIAA green).

**P5 (phi2 is the E clock).** Leg (a), all four instances, both cores: tau1 = 202160 clk_sys, delta1 = 5054 on both cores; tau2 = 1937440 (fx68k) / (the same run, D2 tuned per Pre5.1, see below) clk_sys, delta2 = 48436. Delta2-Delta1 (43382) matches round((tau2-tau1)/(10*T_CPU)) within 1 on both cores: **P5-LEGA is green on both cores**, and it is this package's own measurement of T_E: T_CPU = 4 clk_sys, T_E = 40, to the precision this differential gives. P5-PRE51 (no underflow in the window) and P5-PRE52 (the counter reads the same value twice while stopped) are green on both cores; P5-PRE51-CONTRAST (R-LATCH($1000) with D2 sets the flag) and P5-PRE52-CONTRAST (no stop write, the two reads differ) are both green. **P5-CONTRAST is red on both cores**: counting CNT instead of phi2 (CIAB/timer B, no key sent) gives the identical delta1 = 5054, delta2 = 48436 as the phi2 baseline -- the same INMODE gap, now shown by the package's own most latency-insensitive measurement. Leg (b): (v_i-v_(i+1)) mod 256 matches e_(i+1)-e_i for all four instances, both cores (P5-LEGB green).

**P6 (phi2 is the bus timing reference).** K1 and K2 (CIAA and CIAB PRB reads): phase band 2*T_CPU wide, two distinct values -- green (P6-K1K2), both cores. K3 (the Chip-memory contrast): band 8*T_CPU, five distinct values -- green (P6-K3), both cores. Pre6.1 (K3's own iteration time not a multiple of T_E): I3 = 184 clk_sys on fx68k, 192 on TG68K, neither a multiple of 40 -- green on both cores (the two-NOP pad was tuned to this; a three-NOP pad gave I3 = 200 on fx68k, exactly 5*T_E, and failed there). P6-P4LOOP, P6-P5BLOOP and P6-P9LOOP (the same phase-band check reused on P4's, P5 leg (b)'s and P9 leg W's own sample loops, CIAB/timer B) are all green on both cores.

**P3 (toggle inverts on every underflow).** All four instances, both cores: 88 ICR flags (Q), zero PRB transitions (T = 0). **P3 is red on both cores**: the same PBON-does-not-reach-PRB gap P1-MAIN and P2-TOGGLE already show means the toggle bit never moves on PRB regardless of how many underflows occur. P3-PRE (underflows at least 3 iterations apart, at least 16 of them) is green. P3-PRE31-CONTRAST (L=2, flags in consecutive reads), P3-PRE32-CONTRAST (not started, |Q|=0), P3-CONTRAST-PULSE (OUTMODE=0, same latch, pairing fails) and P3-CONTRAST-PBON0 (PBON=0, pairing fails while flags continue) are all green, on CIAB/timer B, both cores.

**P4 (pulse mode).** All four instances, all six latches (20-25), both cores: the measured period is 18 for L=20-21 and 27 for L=22-25 -- neither L nor L+1, and not uniquely determined under this run's own R-SAMPLE granularity (unlike P8.65 mode 10's L=30, which resolves to exactly 30). Coverage is not met on any latch. **P4 and P4-PRE are red on both cores**: zero of the six latches are usable, so the "at least 2 usable" precondition already fails, before P3's own PBON/PRB gap (which independently keeps every PRB sample constant) is even reached. P4-CONTRAST-NOSTART (CR ← $02, no high sample) is green. P4-CONTRAST-TOGGLE is recorded, not graded: the comparator P4 grades does not test the "about half the residues" shape a toggle-mode run would give.

**P9 (the latch is the divider).** Leg F (CIAB/timer B, reusing P3's own toggle mechanism): measured P(L) is 45 for both L=40 and L=41, 63 for L=64, 99 for L=100. delta = P(L)-L is 5, 4, -1 and -1 -- not constant, and not in {0,1} for three of the four. **P9-LEGF is red on both cores.** Leg W (P2 leg S's own mechanics, CIAB/timer B): W(40) and W(100) are both undefined -- no sample ever shows a high-to-low transition, because PBON's own override never reaches PRB (the same gap P3 shows), so the bit that leg W watches never rises in the first place. **P9-LEGW is red on both cores** for that reason. P9-CONTRAST is recorded: leg F's own four latches are already each other's contrast, and the "same for all four" clause above is the one that goes red.

**P10.** Every row is red on both cores: each is the conjunction of sub-propositions above, and at least one of them is red in every row (opus:F-timers.c6/sol:F-timers.c8 is the sole row whose every named sub-proposition -- P4, P9 leg F, P9 leg W, P8.65 mode 10 -- was actually built and run; the other rows also include P7.5/P8.65's own INMODE gap or P3/P4's own OUTMODE gap).
