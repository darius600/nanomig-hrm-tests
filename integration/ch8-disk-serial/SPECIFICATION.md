# Specification: serial-port timing, nine-bit receive and disk bit-cell rate (chapter 8 claims)

This document specifies an integration test package for the FPGA
re-implementation of the Amiga chip set. It was written from the package's
claims (`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware
Reference Manual*, 3rd edition, by an author who never saw the
implementation or any test. Page numbers are printed page numbers.
Quotations follow the printed page (the scan), which governs where the text
layer differs. The package has five propositions, P1 to P5. Of the 34
claims, 5 carry propositions and 29 do not (section 1).

---

## 1. Claims not turned into a proposition

### 1.1 Claims that need data read from or written to a disk surface: cannot be checked yet

| Claims | Page | Reason |
|---|---|---|
| `opus:8-dsk.c10`, `opus:8-dsk.c11`, `opus:8-dsk.c12`, `opus:8-dsk.c13`, `opus:8-dsk.c14`, `opus:8-dsk.c15`, `opus:8-dsk.c16`, `sol:8-dsk.c10`, `sol:8-dsk.c11`, `sol:8-dsk.c12`, `sol:8-dsk.c13`, `sol:8-dsk.c14`, `sol:8-dsk.c15`, `sol:8-dsk.c16`, `sol:8-dsk.c17`, `sol:8-dsk.c18` | 241 | Each says the controller can read and/or write a kind of disk (double-density MFM, Amiga V1.0, MS-DOS 5.25" and 3.5", CP/M, GCR, Apple II). Reading needs a disk input stream, and a write is judged by what reaches the disk surface. The harness serves no disk, and the disk write data is not among the outputs a test sees. Once a disk can be served, each named format becomes checkable one format at a time. Even then, "virtually any" and "most" are hedges that no sample of formats can establish. p.241 ties no format to any register setting. |
| `opus:8-mfm.c7`, `sol:8-gcr.c2` | 250 | MSBSYNC acts on disk read: "MSBSYNC, when a 1, tells the disk controller to look for this sync bit on every disk byte." To check it, the input must carry bytes whose most significant bit is set, which means data read from a disk surface. No page says what the controller receives when no disk is served, so a read without a disk gives no expected value. |
| `opus:8-adkcon.c31`, `sol:8-appa-adkbit.c13`, `opus:8-appA-adkcon.c22` | 265 | These say the same of ADKCON bit 9 ("Enables disk read synchronizing on the MSB"), so the reason in the row above applies. The name "MSBSYNC" cannot be observed. The one thing about bit 9 a test could see without a disk is that ADKCONR returns it, and none of these claims says that. |

### 1.2 A claim that needs a disk drive

| Claims | Page | Reason |
|---|---|---|
| `opus:8-cia.c9`, `sol:8-cia.c7`, `opus:8-cia.c10`, `sol:8-cia.c8` | 244 | DSKRDY* is driven by the drive: "The drive will pull this line low when the motor is known to be rotating at full speed." The harness serves no drive. The claims say when the input may be trusted, not what value it has. With the motor on, the value depends on the drive. With the motor off, the page says only that "configuration information may obscure the meaning of this input". That describes a possibility and gives no expected value. |

### 1.3 Absolute durations

| Claims | Page | Reason |
|---|---|---|
| `sol:8-serper.c4`, `sol:8-serper.c5` | 255 | These give the color clock as 279.36 ns (NTSC) and 281.94 ns (PAL). A test measures time only in system clock cycles, and no page ties the re-implementation's clock to nanoseconds. Selecting NTSC or PAL changes no count a test can take. These values are used only to convert: microseconds into color clocks in P4 (R3), and a bit time into a rate in P3. They are not graded. |
| `opus:8-serper.c10` | printed 284 | The claim's `says` is only a constant, ".2794 microseconds", for no named machine. The claim records p.283, but the printed SERPER entry is on **p.284**: printed p.283 ends with SERDATR bit 00, and SERPER heads p.284. The constant cannot be observed for the same reason as `sol:8-serper.c4`/`c5` above (a test measures cycles, not nanoseconds), and on a PAL machine it disagrees with p.255's own color clock (P2), so it cannot stand in for either page's machine-specific value. |

### 1.4 A claim whose printed quantity is not observable at this boundary

| Claims | Page | Reason |
|---|---|---|
| `opus:8-adkcon.c33` | 265 | "08 FAST Disk data clock rate control 1=fast(2us) 0=slow(4us)" times the disk data clock on Paula's write-data pin (p.380: `38 DKWD* Disk write data O`). `nanomig.v` has no such pin; its disk side is the SD-card sector interface. P4 below times the rate at which disk write DMA reads words at the Chip-memory port instead. No page ties that rate to the bit-cell time -- it would need an unprinted premise (that DSKDAT is the whole of the storage between memory and the pin, or that Paula requests a word only when the previous one has been sent; p.194 lets a device leave a slot unused, "If a device does not request one of its allocated time slots, the slot is open for other uses", and pp.380-381 name only Paula's `DMAL`, "DMA request line"). P4's own rate comparison (P4-MAIN, the four `P4-*-FAST*` lines) is declined for this reason; preconditions 4a-4f, which each grade a different page sentence on their own, stay graded (see P4 below). |

---

## 2. Shared definitions

### 2.1 Time unit

All times in the Chip-port trace, on TxD and on RxD are counted in **system clock cycles** ("cycles").

p.24 calls the 3.58 MHz color clock "the system clock": "One horizontal count takes one cycle of the system clock". This document never uses that name for it.

### R1: the color clock C

**What the pages say.**

- p.194: "During a horizontal scan line (about 63 microseconds), there are 227.5 "color clocks", or memory access cycles. A memory cycle is approximately 280 ns in duration."
- Figure 6-9 (p.195) numbers its slots in color clocks: "Decimal numbers above the illustration represent color clocks". It dimensions one slot as "280 ns". It draws the four memory-refresh slots in alternate positions, two slots apart.

So one Chip-memory slot is one color clock.

**Definition.** C is the number of cycles per slot. In a line, take the four accesses the harness attributes to memory refresh, at times r1 to r4. Then C = (r4 − r1) / 6. C need not be an integer.

**Anchor checks.**

- **(a)** In every line of the run, the four refresh accesses are exactly 2C, 2C and 2C apart, with the same C in every line.
- **(b)** NTSC only: the first refresh access of line L and that of line L+2 are 455·C apart.
  - p.23: "Every other line is a long line (228 color clocks, 0-$E3), with the others being 227 color clocks long."
  - p.194: 227.5 per line.
  - Both give 455 for two lines.

**No line-length check on PAL.** The pages disagree:

- p.23: "In PAL, they are all 227 long."
- p.194: 227.5 color clocks per line, with no machine named.
- p.304: "PAL (625 lines, 227.5 colorclocks per scan line)".

**Contrast.** No stimulus described on the pages moves the refresh slots, so the red case is shown on the checker: grading (a) with a slot length of C/2 or 2C must fail.

### R2: the video standard

**What the pages say.** p.24: "There are actually 262 NTSC (312 PAL) possible vertical positions." and "In NTSC, the fields are 262, then 263 lines and in PAL, 312, then 313 lines."

**Check.** Figure 6-9 gives each line one refresh group. Count the refresh groups between two consecutive vertical sync pulses on the video output. The count must be 262 or 263 on NTSC, and 312 or 313 on PAL.

So that a complete field lies in every run, every program starts with the lead-in G2.

**Contrast.** Selecting the other standard makes the check fail.

### R3: microseconds and color clocks

A color clock is 0.27936 µs on NTSC and 0.28194 µs on PAL (p.255). p.24 gives 3,579,545 Hz and 3,546,895 Hz, which are 0.279365 µs and 0.281937 µs. The two pages agree to the precision printed.

### 2.2 Program conventions

- **G1: interrupts and DMA off.** At the start, INTENA ← $7FFF and DMACON ← $7FFF.
  - Bit 15 = 0 clears every bit written as 1 (p.218: "When you wish to clear a bit (make it a 0), you must place a 1 in the position you wish to clear and a 0 into position 15"; p.278; p.280).
  - The program polls and takes no interrupts.
  - Memory refresh is not a DMACON channel and keeps running.
- **G2: lead-in.** INTREQ ← $0020 (clear VERTB, bit 5). Then twice: poll INTREQR bit 5 until it is 1, and clear it again. VERTB is set "at line 0 (start of vertical blank)" (p.219), so after the lead-in at least one whole field lies in the trace.
- **G3: markers.** At the points named below, the program writes a word to a fixed Chip address MARK. Each value is distinct: 0, k, BRK, UNBRK, READY, START, DONE. The trace shows each marker write with its time.
  - All program data (MARK, the probe addresses, the RESULT area, the disk buffer) is specified below $80000. That is the range an OCS disk pointer reaches (p.279: DSKPTH holds the "high 3 bits, high 5 bits if ECS" and DSKPTL the "low 15 bits"), so the same addresses work under OCS and ECS.
  - Below $80000 is also the ROM overlay window while the overlay is on, the reset default (p.223: "the system RAM that would normally be located at this address is not available... Software must assume that memory is not available"). A CPU write there does not reach Chip RAM until the overlay is cleared, which p.223 says is done through the OVL bit "in one of the 8520 Chips" (Appendix F, p.339: CIA-A ddra $BFE201 set to $03, pra $BFE001 bit 0 is OVL). Any G3 data placed below $80000 needs that clear first.
  - Program data is kept apart from the program code.
  - The probe addresses are PROBE_I, PROBE_D and RESULT.
- **G4: bounded polls.** Every poll loop has a bound of at least three times the expected wait.
  - A poll that ends at its bound is recorded, and fails the precondition that needs its event.
  - Either way, the program always reaches DONE.
- **G5: poll bracket (used in P5).** A bracketing loop runs identical code in every iteration: it reads a register and stores the value to a fixed probe address.
  - For a bit going from 0 to 1, let t0 be the store time of the last sample showing 0, and t1 the store time of the first sample showing 1. Let S be the longest iteration: the largest gap between consecutive stores of that loop in the run.
  - The read that saw 1 came before its store, and the read that saw 0 came less than one iteration before its own store. So the event lies in the half-open interval (t0 − S, t1].
  - The bracket's width is t1 − t0 + S, and its midpoint is the estimate of the event's time.
- **G6: runs.** Every run is made on each of the two CPU cores, with the Enhanced Chip Set and 2 MB of Chip memory. The expected values do not depend on the core. The resolution preconditions must hold on each core.

---

## 3. Propositions

### P1: On NTSC, the transmitter sends one bit every N+1 color clocks

**Proposition.** On an NTSC machine, with SERPER bits 14-0 set to N, the serial transmitter sends one output bit every N+1 color clocks.

**Claims.**

- `sol:8-serper.c8` (p.255)

**Page.**

- p.255: "If the SERPER divisor is set to the number N, then N+1 color clocks occur between samples of the state of the input pin (for receive) or between transmissions of output bits (for transmit). Thus SERPER=(3,579,545/baud)-1."
- p.284: "If this number is N, then the baud rate is 1 bit every (N+1)*.2794 microseconds."
- On NTSC the two agree: .2794 µs is 279.36 ns to the four figures p.284 prints.
- p.255 also says "Bits 14-0 of SERPER are the baud-rate divider bits".

**Stimulus.** There is one run per N, on NTSC.

The values of N and of W (the number of words sent) are:

| N | SERPER | W |
|---|---|---|
| 0 | $0000 | 1 |
| 371 | $0173 | 4 |
| 32396 | $7E8C | 1 |

The run proceeds as follows:

1. G1, then G2.
2. SERPER ← N, with bit 15 (LONG) = 0.
   - p.256: "Receive mode is set by bit 15 of the write-only SERPER register".
   - p.258: transmission starts "at the baud rate you selected". So SERPER is written before SERDAT.
3. INTREQ ← $0001. This clears the TBE request, bit 0 (p.280).
4. MARK ← 0, then SERDAT ← $0155. The value $0155 has bits 15-9 = 0, bit 8 = 1 (the stop bit) and bits 7-0 = $55.
   - This is the layout of Figure 8-12 (p.259), which shows bits 15-8 as "0 0 0 0 0 0 0 1" followed by "8 bits data".
   - p.259: "Above the data bits, 1 bits must be added as stop bits."
   - The word is not all zeros (p.259: "If this register is written with all zeros, no data transmission is initiated").
5. For k = 2 to W:
   - poll INTREQR bit 0 until it is 1 (bounded, G4);
   - SERDAT ← $0155;
   - INTREQ ← $0001;
   - MARK ← k.

   *Why this loop.* The TBE request is set when the SERDAT word moves into the shift register:
   - p.258: "When the transfer to the serial shift register has been completed, SERDAT can accept new data; the TBE interrupt signals this fact."
   - p.283: "This sets the interrupt request TBE".

   The request is cleared only after the next word has been written. That way, its next setting marks the next transfer.

   The SERDATR bit TBE is not used. p.257 and p.283 disagree on whether it mirrors the interrupt request.
6. Poll SERDATR bit 12 (TSRE) until it is 1 (bounded), then MARK ← DONE.

**Observation.** Every TxD level change, with its time, in the window from the MARK=0 write to the MARK=DONE write.

**Expected.** Let t0 be the first TxD change in the window, and let T = (N+1)·C.

- For every k from 0 to 10W−1, there is exactly one TxD change within ±C/2 of t0 + k·T.
- There is no other TxD change in the window.

Where this comes from:

- Each word is one start bit, eight data bits and one stop bit:
  - p.258: "To form a typical data sequence, such as one start bit, eight data bits, and one stop bit, you write into SERDAT the contents shown in Figures 8-11 and 8-12".
  - p.259: "One start bit is automatically generated before the first data bit (bit 0) is sent".
- Bits leave "starting with the least significant bit" (p.258).
- So with data $55, the line changes at every bit boundary: start bit, then 1, 0, 1, 0, 1, 0, 1, 0, then the stop bit.
- Once the next word is waiting, the boundary from one stop bit to the next start bit also carries a change. That boundary is also a boundary "between transmissions of output bits".

The tolerance ±C/2 grades at the unit the page counts in, whole color clocks. Each change is graded against t0, not against the change before it, so any drift is caught.

**Quantifiers.**

- "Between transmissions of output bits" applies to every consecutive pair of bits in the run, and all of them are graded.
- "The number N" means any 15-bit value. The three runs sample it:
  - every divider bit is 1 in one of $0173 and $7E8C, and 0 in the other;
  - N = 0 is the smallest value.

  The sample does not establish the relation for every N.
- For orientation (not graded):
  - N = 32396 is about 110.5 bit/s. p.255 says the UART "is programmable for any rate from 110 to over 1,000,000 bits per second".
  - N = 0 is one color clock per bit. No page excludes it.

**Preconditions.**

- **1a. R1 anchor.** The contrast is given in R1.
- **1b. R2 shows NTSC.**
  - Contrast: select PAL, and 1b fails.
- **1c. TxD idle before the window.** TxD does not change from the start of the run until MARK=0. So every change inside the window belongs to these words.
  - Contrast: after step 2, the program writes ADKCON ← $8800 (UARTBRK set) and then ADKCON ← $0800 (UARTBRK cleared).
  - p.257: "UARTBRK Force the transmit pin to zero". The page predicts two TxD changes before MARK=0, so 1c fails. Nothing else changes.
- **1d. Words sent back to back (N = 371 only).** For k = 2 to 4, MARK=k comes before the change at t0 + (10(k−1)−1)·T. That is the start of word k−1's stop bit. So word k was waiting when word k−1 ended.
  - p.257 (TSRE): "If you stop writing data into the output register (SERDAT), then this bit will become a 1 after both the word currently in the shift register and the word placed into SERDAT have been transmitted."
  - Contrast: the program polls SERDATR bit 12 until it is 1 before writing each word k. Then MARK=k comes after the stop change, and 1d fails.
- **1e. The run completed.** MARK=DONE is present and no poll ended at its bound.
  - Contrast: the program leaves out the SERDAT write for word 2 but still polls for its transfer. The TBE request is set only when data moves into the shift register (p.283), so the poll ends at its bound and 1e fails. DONE is still written.

**Contrasting case.** Write SERPER with N+1 (1, 372, 32397) and grade against N. The change expected at k = 1 then arrives one color clock late, which is more than C/2, so the check fails. Nothing else changes.

**Reference frames.**

- R1 gives C.
- t0 is the first change after MARK=0. p.259 says the start bit comes before data bit 0, so t0 is the start of word 1's start bit. The check uses only times measured from t0.

---

### P2: On PAL, the bit time is one fixed period times N+1

**Claims.**

- `sol:8-serper.c9` (p.255)

**The pages disagree on PAL.**

- p.255: "On a PAL machine, SERPER=(3,546,895/baud)-1". With p.255's PAL color clock of 281.94 ns, that is N+1 PAL color clocks per bit.
- p.284: "If this number is N, then the baud rate is 1 bit every (N+1)*.2794 microseconds." This names no machine. On PAL, .2794 µs is 0.991 of a color clock (0.2794 / 0.28194).

Both pages agree that one bit lasts N+1 times one fixed period. Only that is graded.

**Proposition.** On a PAL machine, the transmitter's bit time is N+1 times one fixed period, the same period for every bit and for every N.

**Stimulus.** As in P1, with PAL selected.

**Observation.** As in P1.

**Expected.**

- **(i) A uniform grid in each run.** Let T_N = (t_last − t0) / (10W−1). For every k from 0 to 10W−1, there is exactly one change within ±C/2 of t0 + k·T_N, and no other change in the window.
- **(ii) The same period in every run.** Let U_N = T_N / (N+1). For every pair of runs:

  |U_N − U_M| ≤ C / ((10W_N − 1)(N+1)) + C / ((10W_M − 1)(M+1)).

- **Diagnostic, not graded.** Report U/C. p.255 predicts 1 and p.284 predicts 0.991.

**Quantifiers.** The same sample of N as in P1.

**Preconditions.** 1a, 1b (showing PAL), 1c, 1d and 1e, with the same contrasts as in P1.

**Contrasting case.** In the N = 371 run, write SERPER ← 372 and grade the run as N = 371. U_371 is then off by a factor 373/372. That is about C/372, far beyond the tolerance in (ii) of about C/(39·372), so (ii) fails.

**Reference frames.** R1, and t0 as in P1.

---

### P3: SERPER = 371 on NTSC is the setting p.255 calls 9600 baud

**Claim.** `sol:8-serper.c10` (p.255): "For example, the proper SERPER value for 9600 baud on an NTSC machine is (3,579,545/9600)-1=371."

**Proposition.** On NTSC, SERPER = 371 gives a bit time of 372 color clocks. That is the setting p.255 names as 9600 baud.

**Stimulus and observation.** The N = 371 run of P1.

**Expected.** Every change of the run lies within ±C/2 of t0 + k·372·C, for k = 0 to 39.

The rate is reported, not graded:

- The rate is 3,579,545 / 372 = 9622.4 bit/s (R3).
- The page's own arithmetic gives 3,579,545 / 9600 − 1 = 371.87, printed as 371. So the "9600 baud" of p.255 is 9622 bit/s.
- The test does not grade 9600. The page gives it only as the name of this setting.

**Preconditions.** As P1 with N = 371: 1a, 1b, 1c, 1d and 1e.

**Contrasting case.** Write SERPER ← 372, the value that rounding 371.87 would give (9596.6 bit/s). The changes then fall at k·373·C, and the check fails at k = 1.

**Reference frames.** R1, and t0 as in P1.

---

### P4: ADKCON FAST sets the disk bit cell to 2 µs or 4 µs

**Claim.** `opus:8-adkcon.c33` (p.265) -- declined (1.4): P4 measures the
Chip-memory read rate, not the bit-cell time the claim states, and no page
ties the one to the other.

**Page.**

- p.265: "08 FAST Disk data clock rate control 1=fast(2us) 0=slow(4us). (fast for MFM, slow for MFM or GCR)"
- p.249: "8 FAST Value of 1 selects two microseconds per bit cell (usually MFM). Data must be valid raw MFM. 0 selects four microseconds per bit (usually GCR)."
- The two pages agree on 2 µs and 4 µs. Their notes on usage differ, but the proposition does not use those notes.

**Proposition.** ADKCON bit 8 = 1 makes a disk bit cell last 2 µs, and bit 8 = 0 makes it last 4 µs.

This would put a disk write's Chip-memory fetch at one word every 32 µs or every 64 µs, if the
Chip-memory read rate tracked the bit cell. No page says it does (1.4), so P4 below declines that
comparison and reports the measured Chip-memory read rate instead.

**Why P4's rate comparison is declined, though the buffer needs no disk or drive.**

- The data buffer holds one word. p.279, DSKDAT: "It contains two bytes of data that are either sent (written) to or received (read) from the disk. ... The DMA controller automatically transfers data to or from this register and RAM".
- The words in memory are raw MFM, one cell per bit. p.250: "The raw MFM data that must be presented to the disk controller will be twice as large as the unencoded data".
- **Neither page says when the next word leaves memory.** "One word per 16 cells" would need DSKDAT to be the whole of the storage between memory and the write-data pin, or Paula to request a word only when the previous one has been sent -- neither is printed. p.194 lets a device leave a slot unused ("If a device does not request one of its allocated time slots, the slot is open for other uses"), and pp.380-381 name only Paula's `DMAL`, "DMA request line". So the Chip-memory read rate P4 observes is not shown to be the bit-cell rate; P4's comparison against the bands below is declined (1.4).
- No disk and no drive are needed to observe the Chip-memory read rate itself. The only conditions p.248 lists for disk DMA are DSKLEN's DMAEN and DMACON's DSKEN and DMAEN ("All the various DMA bits must be true"), which is what preconditions 4c/4d test.

**Stimulus.** There is one run for each combination of FAST value (1 or 0), standard (NTSC or PAL) and core.

1. G1, then G2.
2. Clear the ROM overlay, then fill L = 256 words starting at B = $20000 with $AAAA.
   - The overlay is on at reset (p.223), and B lies inside its window ($0-$7FFFF); a CPU write there is otherwise lost (G3). Clear it first: CIA-A DDRA ($BFE201) ← $03, then CIA-A PRA ($BFE001) ← 0, clearing OVL (bit 0) (p.223; Appendix F, p.339).
   - p.246: "The lowest bit of the address must be zero, and the buffer must be in Chip memory".
   - $AAAA is valid raw MFM. By the table on p.250, a data 0 that follows a 0 is written as 10. p.249 requires valid raw MFM for FAST = 1.
3. ADKCON ← $7F00. This clears bits 14-8 (PRECOMP1/0, MFMPREC, UARTBRK, WORDSYNC, MSBSYNC and FAST).
   - p.249: "If bit 15 is a zero, any ones will clear the corresponding bit".
   - For FAST = 1, then write ADKCON ← $8100, which sets bit 8.
   - Read ADKCONR and store the value in RESULT.
4. DSKPTH ← B, as a single long-word write (p.246: "The value must be written as a single long word to the DSKPTH register ($DFF020)").
5. INTREQ ← $0002. This clears DSKBLK, bit 1.
6. Start the DMA with the sequence of p.247:
   - DMACON ← $8210, setting DMAEN and DSKEN (p.278; p.247 step 1).
   - DSKLEN ← $4000 (step 2).
   - DSKLEN ← $C100: DMAEN, WRITE, and LENGTH = 256 (p.246; step 3).
   - DSKLEN ← $C100 again (step 4: "This actually starts the DMA").
   - MARK ← START.
7. Poll. In every iteration, store DSKBYTR to PROBE_D and INTREQR to PROBE_I.
   - Stop when INTREQR bit 1 is 1 (DSKBLK: "disk DMA has completed", p.250).
   - The bound is at least 3 × 256 × 64 µs.
8. DSKLEN ← $4000 (p.247 step 5), then MARK ← DONE.

**Observation.** The Chip-port reads of address B + 2i (for i = 0 to 255) that the harness attributes to disk DMA. Call t_i the time of the read of word i.

The window runs from t_64 to t_192. This is the middle half of the transfer, away from its start and its end.

**Declined, not graded.** T_cell, the Chip-memory read rate, is measured and printed (MEASURED) on
both cores, in every configuration, converted to microseconds with R3:

T_cell = (t_192 − t_64) / (128 · 16 · C) color clocks.

p.265 and p.249 give the disk data clock's bit-cell time as "2us"/"4us" and "two"/"four
microseconds", to one significant figure, with no further precision -- but that is a different
quantity from T_cell (1.4), so P4-MAIN does not grade T_cell against either band.

For orientation, not graded: 32 µs per word is 114.5 color clocks on NTSC and 113.5 on PAL.

**Quantifiers.** The claim covers both values of the bit, and both are run.

**Preconditions.**

- **4a. R1 anchor.** The contrast is given in R1.
- **4b. R2 shows the standard selected.**
  - Contrast: select the other standard, and 4b fails.
- **4c. The FAST bit took effect.** The ADKCONR value stored in step 3 has bit 8 = 1 in the FAST = 1 run, and bit 8 = 0 in the FAST = 0 run. p.249: ADKCONR "is the read-only address for this register".
  - Contrast: in the FAST = 1 run, write ADKCON ← $0100 instead of $8100. With bit 15 = 0 this clears the bit, it reads 0, and 4c fails.
- **4d. The disk write DMA runs throughout the window.** Every PROBE_D value stored between t_64 and t_192 has bit 14 (DMAON) = 1 and bit 13 (DISKWRITE) = 1.
  - p.248: "DMAON Indicates when DMA is actually enabled" and "DISKWRITE The disk write bit (in DSKLEN) is enabled."
  - Contrast: leave out the second DSKLEN write (p.247 step 4). Then DMAON = 0 and there are no fetches, so 4d fails. The poll ends at its bound and DONE is still written.
- **4e. The fetches cover the window.** Reads of B + 2i are seen for at least i = 0 to 192. Each word is read once, and in rising order of i.
  - p.247: "After each transfer occurs, the value of the pointer is incremented."
  - The program makes no access to the buffer after step 2.
  - Contrast: use LENGTH = 128 ($C080, written twice). The fetches then end at i = 127, and 4e fails.
- **4f. The transfer is in a steady state.** |(t_128 − t_64) − (t_192 − t_128)| < 456·C. A fetch can wait at most one line for a disk slot:
  - p.194 lists "3 cycles for disk DMA" in every line, placed as Figure 6-9 shows;
  - the longest line is 228 color clocks (p.23).
  - Contrast: in the FAST = 1 run, the program waits for about the time of 128 words and then writes ADKCON ← $0100. The page then predicts 4 µs cells for the second half, and 4f fails.

**Contrasting case.** None for P4-MAIN: it is a RECORDED decline, not a graded comparison, so no
stimulus can turn it red or green (1.4). Preconditions 4a-4f keep their own contrasts, below.

**Reference frames.**

- R1 gives C.
- R3 converts to microseconds.
- The window is set by fetch index, counted from the first read of B.

---

### P5: The nine-bit receive setting takes eight data bits plus a parity bit

**Claims.**

- `opus:8-serial.c36` (p.255)
- `sol:8-serial.c21` (p.255)

**Page.**

- p.255: "The number of bits that are to be received before the system tells you that the receive register is full may be defined either as eight or nine (this allows for 8 bit transmission with parity). In either case, the receive circuitry expects to see one start bit, eight or nine data bits, and at least one stop bit."
- p.256: "Receive mode is set by bit 15 of the write-only SERPER register. Bit 15 is a 1 if you chose nine data bits for the receive-register full signal, and a 0 if you chose eight data bits."
- p.256: "When the proper number of bit times have elapsed, the contents of this register are transferred to the serial data read register (SERDATR) ..., and you are signaled that there is data ready for you."
- p.284: "15 LONG Defines serial receive as 9-bit word."

**The pages disagree on SERDATR bit 8.**

- p.257: "9 STP Stop bit if 9 data bits are specified for receive." / "8 STP Stop bit if 8 data bits are specified for receive. OR DB8 9th data bit if 9 bits are specified for receive." / "7-0 DB7-DB0 Low 8 data bits of received data."
- p.283: "09 STP Stop bit" / "08 STP-DB8 Stop bit if LONG, data bit if not." / "07 DB7 Data bit" ... "00 DB0 Data bit".

What both pages agree on:

- bits 7-0 hold eight data bits;
- with the nine-bit setting (LONG), bit 9 is the stop bit.

What they do not agree on: with the nine-bit setting, bit 8 is the ninth data bit on p.257 but the stop bit on p.283. Both readings give 1 when the ninth bit and the stop bit are both 1. The stimulus uses only that case.

**Proposition.** With SERPER bit 15 = 1, a character made of a start bit, eight data bits, a ninth (parity) bit and a stop bit is received whole:

- SERDATR bits 7-0 hold the eight data bits;
- bit 9 holds the stop bit;
- the receive-buffer-full signal comes one bit time later than with bit 15 = 0.

**Reference frame R4 (line levels).**

- A 1 bit is the level TxD shows when idle. Figure 8-11 (p.258) shows the 1 left in the bit-shifted-out position after the last shift. p.259: the register stops "when there is a 1 bit present in the bit-shifted-out position".
- A 0 bit is the level TxD shows while ADKCON UARTBRK is set (p.257: "Force the transmit pin to zero").
- p.257: "Data is TRUE (data you read is the same polarity as the data expected)."
- The data bytes are bit-palindromes, so the result does not depend on the order in which the receiver assembles the bits. The pages state that order only for transmit (p.258).

**Stimulus.** There are two runs on NTSC, on each core: run A with bit 15 = 1, and run B with bit 15 = 0.

The program:

1. G1, then G2.
2. ADKCON ← $8800 (UARTBRK set), then MARK ← BRK. Wait at least 100 color clocks. ADKCON ← $0800 (UARTBRK cleared), then MARK ← UNBRK.
3. SERPER ← $8173 in run A, or $0173 in run B. That is N = 371, so T = 372·C (P1).
4. INTREQ ← $0800 (clear RBF, bit 11), then MARK ← READY.
5. For j = 0 to 7:
   - poll INTREQR, storing every value to PROBE_I, until bit 11 is 1 (bounded at 3 × 16T or more);
   - read SERDATR and store it to RESULT + 2j;
   - INTREQ ← $0800 (p.256: "After reading the contents of this data register, you must reset the INTF_RBF bit in INTREQ to prevent an overrun.");
   - MARK ← j+1.
6. MARK ← DONE.

The RxD driver behaves the same in both runs:

- **Idle.** RxD is held at 1 from the start of the run.
- **Start times.** Character j starts at s_j = s_0 + j·(16T + T/8).
  - s_0 lies at least 2T after MARK=READY.
  - The driver may be scheduled at a fixed time; precondition 5d checks the order.
- **One character.**
  - a start bit of 0, lasting T;
  - the eight data bits of byte_j, one after another, each lasting T;
  - a ninth bit of 1, lasting T;
  - a stop bit of 1, lasting T;
  - then idle at 1.
- **The bytes.** byte_j = $A5, $5A, $81, $7E, $3C, $C3, $66, $99. Each is a bit-palindrome.
- **Why the T/8 step.** It moves each start edge an eighth of a bit later relative to the receiver's timing. Over the eight characters this covers a whole bit time. The pages do not say where the receiver places its samples relative to the start edge; p.255 says only that N+1 color clocks occur between samples.

**Observation.**

- The RESULT values, which are what SERDATR returned.
- The PROBE_I stores, with their times.
- The RxD schedule: every s_j is known exactly.

**Expected.**

- **(a) Data (run A).** For every character j:
  - RESULT bits 7-0 = byte_j;
  - bit 9 = 1;
  - bit 8 = 1.

  Bits 10-15 are not graded here; OVRUN is precondition 5c.
- **(b) No early signal.** In run A, the RBF event for every j comes no earlier than s_j + 9T. In run B, it comes no earlier than s_j + 8T.
  - The start bit is followed by the data bits, so the ninth data bit begins at 9T. Nine data bits cannot have been received before the ninth begins, and eight not before the eighth.
  - This is graded on the G5 bracket: its upper end t1 must be at least s_j + 9T in run A, or at least s_j + 8T in run B.
- **(c) One bit later.** Let D_j be the bracket midpoint minus s_j. Then

  mean_j D_j(run A) − mean_j D_j(run B) must lie in [T/2, 3T/2].

  That is "one bit time later", graded to half a bit, because the page does not state where the receiver samples.

**Quantifiers.**

- "Eight or nine": both settings are run.
- "In either case ... one start bit, eight or nine data bits, and at least one stop bit": each character carries a ninth 1 and a stop 1, which is a valid frame under both settings.
- (a) and (b) are graded on all eight characters.

**Preconditions.**

- **5a. R1 anchor, and R2 shows NTSC.** The contrasts are given in R1 and R2.
- **5b. Level anchor.** TxD between MARK=BRK and MARK=UNBRK is at a different level from TxD after MARK=UNBRK. The driver takes its 0 and 1 levels from these two.
  - Contrast: leave out the UARTBRK set. The two levels are then the same, and 5b fails.
- **5c. No overrun.** Bit 15 (OVRUN) is 0 in every RESULT value.
  - p.256: "If the interrupt is not cleared in time, the OVERRUN bit is set."
  - Contrast: in run A, the program reads and clears character 0 only after s_2. The page predicts OVRUN = 1, so 5c fails. The polls are bounded, so the program still finishes.
- **5d. Every RBF event is observed.**
  - MARK=READY comes before s_0.
  - No PROBE_I value stored before s_0 has bit 11 = 1.
  - For every j, at least one value with bit 11 = 0, stored after s_j, comes before the first value with bit 11 = 1.
  - Contrast: leave out the INTREQ clear in step 5. The poll for the next character then sees 1 at once, and 5d fails.
- **5e. Resolution.** Every G5 bracket is narrower than T/4, so the half-bit grading in (c) is not decided by the polling.
  - Contrast: add a delay of T/4 or more to every poll iteration, and 5e fails.
- **5f. Run B received the same characters.** In run B, RESULT bits 7-0 = byte_j for every j.
  - Contrast: the driver sends run B at a bit time of 2T. Each driven bit is then sampled twice (p.255), so bits 7-0 differ and 5f fails.

**Contrasting cases.**

- For (c): write SERPER ← $8173 in run B as well. The page then predicts no difference between the runs, so the difference is near 0 and (c) fails.
- For (a): drive run A at a bit time of 2T, and (a) fails.
- For (b): a stimulus contrast through (b)'s own clause, run A with SERPER ← $80B9 (N=185), the driver unchanged. p.255's own N+1 rule then has the receiver sample about every 186 colour clocks, near half the driver's own bit time, so it reaches nine samples well before the driver's own ninth bit begins, and (b) fails.

**Recorded, not graded.** A run-A variant in which every ninth bit is 0. Its RESULT bit 8 is recorded. p.257 predicts 0 and p.283 predicts 1, so the pages disagree and the test does not grade it.

**Reference frames.**

- R1 gives C, and so T.
- R2 confirms the standard.
- R4 gives the line levels.
- The s_j come from the driver's own schedule.

---

## 4. Build

This section records what the build added: the programs, the values it
measured, and the mutations it ran. Propositions and expected values above
are unchanged from what this document committed to before the build.

### 4.1 Programs

`Makefile`, `test_ch8_disk_serial.cpp`; `ch8_defs.i` (register offsets from
Appendix B not already in `nanomig.i`, and the bounded-poll macro `POLLBIT`
every program uses for G4, which also ORs a `POLL_TIMEOUT` flag whenever a
poll's own bound is reached); `tx_core.s` (P1/P2/P3's shared engine) and its
nine wrappers `prog_tx_n0[.s]`, `prog_tx_n371[.s]`, `prog_tx_n32396[.s]`,
their three `_plus1` contrast variants, and `prog_tx_n371_1c/1d/1e.s`
(precondition contrasts); `fast_core.s` (P4's shared engine) and its six
wrappers `prog_fast0.s`, `prog_fast1.s`, `prog_fast1_4c/4d/4e/4f.s`; `rx_core.s`
(P5's shared engine, driven from the C++ test's own RxD schedule) and its
seven wrappers `prog_rx_a.s`, `prog_rx_b.s`, `prog_rx_a_5b/5c/5d/5e.s`
(precondition contrasts), `prog_rx_a_altper.s` ((b)'s own contrast; (a)'s,
5f's and (c)'s own contrasts are driver/comparator-level, needing no
separate program).

`ch8_defs.i`'s MARK, RESULT, PROBE_D, PROBE_I and POLL_TIMEOUT sit at
$0C0000 and above, not below $80000 as G3 specifies for program data in
general: below $80000 is the ROM overlay window while the overlay is on
(p.223), and only P4's own stimulus clears it (before its buffer fill); no
other program in this package does, so a lower address would never be
written. P4's own disk buffer, DBUF, does sit at $020000 (below $80000),
inside the range the same overlay-clear step opens up.

**Tuned, not page-defined.** Every poll loop is bounded at least 3x its own
expected wait (G4): `BND_VERTB` = 500000 iterations (VERTB's wait is at
most one field, independent of SERPER); `BND_TBE` = 1000 (only the WMULTI
wrapper's per-word poll uses it; TBE resets after about one word time,
10*(N+1)*C, and per-iteration cost differs enough between the two cores
that 1000 is needed for at least 3x that wait on the faster-executing
core); `BND_TSRE` is set per program from that program's own N and W (200
for N=0, 15000 for N=371, 200000 for N=32396); P4's `BND_POLL` = 200000;
P5's `BND_VERTB` = 500000, `BND_RBF` = 1500 (RBF's own wait scales with the
driver's schedule, not with a program-visible quantity, so one bound covers
every character; tuned above 3x the per-character step, 16.125*(N+1)*C,
while staying small enough that exhausting it on every one of the eight
polls still fits the run's own cycle budget); `prog_rx_a_5c.s`'s `C5DELAY`
= 2000 (tuned to land the delayed read past the following character's own
completion, not merely past its own); `prog_rx_a_5e.s`'s `SLOWCOUNT` = 120;
`prog_fast1_4f.s`'s `MIDSTOP_DELAY` = 100000. None of these bounds is
reached in an unmutated run; 1e's, 2e's, 4d's and 5c's own contrasts are
the cases built to reach one (5c's own poll for the character past the end
of the driven data is what exhausts `BND_RBF`).

### 4.2 Measured values

- R1's C measured 8.000000 (clk_sys cycles per colour-clock slot) on every
  capture, both standards, both cores.
- R2's field-line count measured 263 (NTSC) and 313 (PAL) on every capture,
  both cores.
- P4's T_cell measured 1.331189 us (NTSC) and 1.343483 us (PAL), identical
  for FAST=1 and FAST=0, on both cores.
- P2's own PAL colour-clock ratios: U_N/C measured 1.000000 (N=0), 1.000103
  (N=371) and 1.000000 (N=32396), on both cores.
- P5's (c): mean_j D_j(run A) - mean_j D_j(run B) measured 2944.5 cycles on
  fx68k and 2978.0 cycles on TG68K, against T=2976.0 (0.9896T and 1.0007T),
  both inside [T/2,3T/2].

### 4.3 What the build found

**P1, P2 and P3 are red on both cores, on the same run.** Measured on the
N=371, W=4 NTSC run (both standards, both cores): the main grid check
matches 20 of 40 changes (words 1-2 land inside the +/-C/2 tolerance,
words 3-4 do not); the single-word runs (N=0, N=32396) match 10 of 10, and
the within-word intervals of the N=371 run are all exact -- both are
graded as their own controls, and both pass. P2's own uniform-grid check
(i) passes on every run; the pairwise U comparison (ii) is red through the
pairs that include N=371: |U371-U32396| = 0.000827 against a tolerance of
0.000579. The pairs that exclude N=371 pass on their own tolerance:
|U0-U371| = 0.000827 against 0.889440, and |U0-U32396| = 0.000000 against
0.888916 (graded on its own as a control). Not exercised by N=0 or N=32396
alone, only by back-to-back transmission (W>1).

**P4-MAIN is declined; preconditions 4a-4f are graded and pass on every
capture, both cores.** With the buffer address and the overlay-clear step
both repaired, the DMA fetch of B+2i (DBUF=$020000) is observed on every
capture: 4a, 4b, 4c, 4d, 4e and 4f all pass. T_cell measured 1.331189 us
(NTSC) and 1.343483 us (PAL), identical for FAST=1 and FAST=0 and identical
on both cores. p.265/p.249's bands (1.5-2.5 us and 3.5-4.5 us) time the disk
data clock, a different quantity from T_cell (1.4); P4-MAIN declines the
comparison for that reason rather than grading T_cell against either band.
4f (steady state) passes: the fetch cadence does not drift.

**P5: every character (0-7) reads back correctly on both runs, both
cores.** RESULT bits 7-0 match byte_j for every j on run A and run B, bit 9
and bit 8 read 1 on run A, and the G5 brackets, RBF-observed and
no-early-signal checks all hold. Bit 9 and bit 8 are not graded against
run A alone (they read 1 on run B too, SPECIFICATION.md P5); the
proposition rests on run A's data match together with its own one-bit-
later timing against run B.

### 4.4 Mutations

See `test_ch8_disk_serial.cpp`'s own header comment for the full list, the
`NMH_MUTATE` value each uses, and which labels each one flips (several
share a binary or a capture, so one substitution can flip more than one
label).
