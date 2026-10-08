# Specification: external disk interface and custom-chip pin directions

This document specifies the integration test package for the claims in
`claims-spec-stage.jsonl`. The claims come from Appendix H (External Disk
Connector Interface, pp. 367-371) and Appendix J (Custom Chip Pin Allocation
List, pp. 379-381) of the *Amiga Hardware Reference Manual*, 3rd edition. The
author wrote it from those claims and the manual's printed pages alone, and
never saw the implementation or any test. Page numbers are printed page numbers.
Quotations come from the printed page. The tables of Appendices H and J were
read from the page images, because their text layer misbinds rows. Quotations
from chapter 8 and Appendix A were checked against the scan.

---

## 1. Claims that give no proposition

### 1.1 opus:AHJ-118, sol:H-092 (p. 371): device I.D. $FFFF FFFF is "Amiga standard 3.25"

Printed row: "1111 1111 1111 1111 1111 1111 1111 1111   Amiga standard 3.25($FFFF FFFF)".

- **No expected value exists for this system.** The I.D. belongs to the
  attached drive. The drive shifts the I.D. out on RDY- (p. 371: "(8) the serial
  input data is read on RDY (pin 1)"). The table says what each I.D. means. No
  page says which kind of drive this system has on any select line. The
  identification mode is defined for "an external device" (p. 335), and no page
  says whether the internal drive 0 answers it.
- **The test cannot act as a drive.** It can drive the controller ports, the
  mouse, the keyboard, serial RxD and clock values. It cannot drive any line of
  the disk connector, so it cannot present a drive of known type. The harness
  serves no disk image: its default `floppy_config` (0, "one drive (DF0),
  write-enabled, normal speed", `nanomig_harness.h`) already gives the system
  drive 0, but no `sdc_*` activity, so no drive answers with any I.D.
  sequence. The drive the harness's default `floppy_config` models has a type
  that no page and no harness text states. The claim needs no data from a
  disk surface; what blocks it is the missing expected value.
- **The pages disagree on how the I.D. is formed.** Only the label of
  $FFFF FFFF is the same on both pages.
  - p. 371: "Convert the 32 values of RDY- into a two 16-bit word. The most
    significant bit is the first value and so on. This 32-bit quantity is the
    device I.D.."
  - p. 335: "The serial input data is active low and must therefore be inverted
    to be consistent with the above table."
  - For $0000 0000, p. 371 prints "Reserved ($0000 0000)" and p. 335 prints
    "$0000 0000 - no drive present."
  - For the sequence, p. 370 steps 5 and 6 select and deselect the drive once
    more before the read loop. p. 371 says: "After initialization, the SELxB
    signal is driven (first active then) inactive as in steps 5 and 6." p. 335
    has no such pulse: "After initialization, the SELxB* signal should be left
    in the inactive state. Now enter a loop where SELxB* is driven active, read
    serial input data on RDY* (pin 1), and drive SELxB* inactive."

### 1.2 opus:AHJ-161, sol:J-030 (p. 379): LP* on the Original Agnus is an input

Printed row: "37 LP* Light pen input I".

- **A pin's direction cannot be seen; only the function of a light-pen input
  can.** The port list lets a test drive pin 6 of either controller port (the
  fire inputs of `joystick0` and `joystick1`, mapped as in section 3.4; p. 238
  names pin 6 the "Beam Trigger" line) and read VPOSR/VHPOSR, which a trigger
  freezes once BPLCON0 bit 3 is set (p. 239). The claim stays declined for the
  two reasons below, which no stimulus removes: a trigger test could not say
  which Agnus took the trigger in.
- **No harness setting corresponds to this chip.** The claim concerns one chip,
  the ORIGINAL AGNUS. The harness selects OCS or ECS *behaviour*, not a chip.
  The pages show two pre-ECS Agnus chips: the Original Agnus of p. 379 and the
  "old Fat Agnus" of p. 381. p. 381 gives the Fat Agnus's LP* as O, with no
  version qualifier. No page links any configuration the test can select to
  the Original Agnus.
- **The connector port depends on the model.** p. 238: "On the A1000, the light
  pen must be connected to port 1. Changing ports requires a minor internal
  modification. On the A500, A2000 and A3000 the default is port 2. An internal
  jumper can select port 1."

### 1.3 opus:AHJ-326 (p. 381): LP* on the Fat Agnus is an output

Printed row: "78 LP* Light pen O".

- **An internal output pin's direction has no observable effect at this
  boundary.** The test sees no chip pin. No page names an effect of Agnus
  driving LP* that shows at an output or in a register.
- **The other pages give the signal on other chips or describe the pen.**
  p. 379 is the table of the Original Agnus and p. 381 the table of the Fat
  Agnus (see section 7.1); each gives the direction of its own chip's pin.
  p. 379 gives the signal on the Original Agnus as "Light pen input I".
  Chapter 8 describes the light pen of the A500, A2000 and A3000 as a trigger
  that Agnus takes in:
  - p. 238: "The pen converts this light pulse into an electrical pulse on the
    "Beam Trigger" line (pin 6)." and "This trigger signal tells the internal
    circuitry to capture and save the current contents of the beam register,
    VPOSR."
  - p. 239: "To enable the light pen input, write a 1 into bit 3 of BPLCON0.
    Once the light pen input is enabled and the light pen issues a trigger
    signal, the value in VPOSR is frozen."
- **What every page agrees on cannot be tested.** All pages agree only that
  Agnus has a pin LP* that serves the light pen and is active low (p. 379:
  "NOTE: * Means an active low signal."). Nothing testable follows from that
  alone.

### 1.4 opus:AHJ-300, sol:J-166 (p. 381): 7MHZ on the Fat Agnus is an output

Printed row: "38 7MHZ 28MHZ clk divided by four O".

- **The pin is internal.** It is a clock pin between chips inside the system.
  The test sees no chip pin, and no page names a visible effect of its
  direction. The port list's `clk7_en` is a clock enable of the system top; no
  page or harness text ties it to an Agnus pin, so its presence says nothing
  about which chip drives the 7 MHz clock.
- **The function column cannot be checked either.** "28MHZ clk divided by four"
  describes the function, not the claim, which states the direction. Checking
  that ratio would need the test's time base ("system clock cycles") related to
  the 28 MHz master clock. Neither the claim nor the harness description gives
  that relation.

### 1.5 opus:AHJ-090, sol:H-064 (p. 370): a RESET instruction drives DRESB- low "for approximately 17 microseconds"

Declined whole: the test sees no pin of the disk connector, no page
identifies any port of the port list with the system reset signal of which
DRESB- is the buffered copy (so `cpu_nrst_out` has no meaning here), and
"approximately" carries no tolerance.

### 1.6 opus:AHJ-085, sol:H-060 (p. 370): the output of the external drive's flip-flop controls its motor

Printed sentence: "The output of this flip-flop controls the disk drive
motor."

- **The flip-flop is the external drive's.** p. 369 and p. 370 describe the
  logic an external drive must have: "All external drives must have logic
  equivalent to a D flip-flop, whose D input is the MTRXD- signal, and whose
  clock input is activated by the off-to-on (high-to-low) transition of its
  SELxB- signal."
- **The only drive the ports show is not an external drive.** Drive 0 is the
  internal drive (p. 245: "PB3 DSKSEL0* Select drive 0 (internal drive)
  (active low)."). The port list has one drive light, `fdd_led`, and no page
  or harness text ties it to any external drive.
- **The sentence that covers drive 0 is not a claim of this package.** p. 244
  gives a latch for every drive ("Each drive will latch the motor signal at
  the time its select signal turns on."), but no claim in
  `claims-spec-stage.jsonl` carries it.

Declined whole. Section 6 records what `fdd_led` showed and grades nothing.

### 1.7 Disk surface

No claim in this package needs data read from or written to a disk surface.
The drive-light record of section 6 needs drive 0 with its light, but no disk
(section 3.6).

---

## 2. Propositions at a glance

| Id | Proposition (short) | Claims | Page |
|----|---------------------|--------|------|
| P1 | Denise M0H is an input: left-port pins 2 and 4 drive JOY0DAT's horizontal counter | opus:AHJ-178, sol:J-046 | 380 |
| P2 | Denise M0V is an input: left-port pins 1 and 3 drive JOY0DAT's vertical counter | opus:AHJ-206, sol:J-074 | 380 |
| P3 | Denise M1H is an input: right-port pins 2 and 4 drive JOY1DAT's horizontal counter | opus:AHJ-176, sol:J-044 | 380 |
| P4 | Denise M1V is an input: right-port pins 1 and 3 drive JOY1DAT's vertical counter | opus:AHJ-208, sol:J-076 | 380 |
| P5 | Paula POT0X (left port, pin 5) is bidirectional | opus:AHJ-243, sol:J-110 | 380 |
| P6 | Paula POT0Y (left port, pin 9) is bidirectional | opus:AHJ-245, sol:J-112 | 380 |
| P7 | Paula POT1X (right port, pin 5) is bidirectional -- **withdrawn** (5.1, 8.2): the input half cannot be set up at this boundary | opus:AHJ-249, sol:J-116 | 380 |
| P8 | Paula POT1Y (right port, pin 9) is bidirectional | opus:AHJ-251, sol:J-118 | 380 |

Section 6 records the drive light and grades nothing.

---

## 3. Common setup

### 3.1 Registers

**Custom registers** (Appendix B, p. 289; the $DFF000 base is from the
addresses on p. 229):

| Register | Address | Access | Page row |
|----------|---------|--------|----------|
| VPOSR | $DFF004 | R | holds V8, the ninth bit of the vertical position (bit 0; p. 238) |
| VHPOSR | $DFF006 | R | "Read vert and horiz. position of beam" (bits 15-8 are V7-V0, p. 238) |
| JOY0DAT | $DFF00A | R | "Joystick-mouse 0 data (vert,horiz)" |
| JOY1DAT | $DFF00C | R | "Joystick-mouse 1 data (vert,horiz)" |
| JOYTEST | $DFF036 | W | "Write to all four joystick-mouse counters at once" (p. 289); bits 15-10 and 7-2 of each counter (p. 282). Written only by the PJ3 contrast (4.5) |
| POTGOR | $DFF016 | R | "Pot port data read (formerly POTINP)" |
| POTGO | $DFF034 | W | "Pot port data write and start" |
| BPLCON0 | $DFF100 | W | bit 3 "enables the light pen latch" (p. 229) |

**CIA-B registers** (p. 340). These are not custom registers. The drive-light
record of section 6 needs them. If the harness does not map CIA-B at these
addresses, the record cannot run.

- PRB at $BFD100: "BFD100 prb /MTR /SEL3 /SEL2 /SEL1 /SEL0 /SIDE DIR /STEP"
  (bits 7 to 0).
- DDRB at $BFD300: "Direction for Port B (BFD100);1 = output (set to 0xFF)".
- Both are accessed as bytes.
- p. 244 prints "CIAAPRB ($BFD100)" for these output bits. The address is the
  one used.

### 3.2 How the program reports

- The program writes **step markers** (a word identifying the step) to a fixed
  Chip-memory address. It stores **samples** (the words it read, each preceded
  by the VHPOSR value read just before) into a Chip-memory buffer.
- The test takes the time, address and data of every such write from the
  Chip-memory port. It uses nothing else the program does.
- **Memory layout.** Everything the program places or writes lies in the
  Chip memory that the harness maps one-to-one at $080000-$1FFFFF, which the
  ROM overlay never covers (`nanomig.i`; `nanomig_harness.h`: while the
  overlay is on, a write inside $000000-$07FFFF reaches no memory; p. 223:
  after reset "the ROM image appears at location $00000000").

  | Use | Address | Source |
  |-----|---------|--------|
  | program | from $080400 | `PROGRAM_ORG` |
  | step marker (one word; every marker rewrites it, and the port log keeps each write) | $0C0000 | `RESULTS` |
  | done mark | $0C0FFE | `DONE_ADDR` |
  | sample buffer (the samples of sections 4 and 5) | from $0C1000 upward, ending below $0F0000 | above `DONE_ADDR`, clear of the stack |
  | stack | below $0FFF00, growing down | `PROGRAM_STACK` |

  The buffer starts above `DONE_ADDR`: section 4's run stores at least 2,048
  samples of 8 bytes (four phases of at least 512 lines), which from
  `RESULTS` would run through `DONE_ADDR`, and a stored word equal to
  `DONE_VALUE` ($600D, a possible VHPOSR value) would end the run there.
- Programs are assembled for the 68000 (vasm's default CPU). Every register
  write named in this document is a plain MOVE.
- Custom-register and CIA accesses do not appear at the Chip-memory port.
  Markers and samples are therefore the only record of when the program acted
  and what it read.
- The program writes BPLCON0 = $0000 once at the start (bit 3 = 0). p. 239:
  "when the light pen latch is enabled, the VPOSR register may be latched at
  any time, and cannot be relied on as a counter"; the waits below count lines
  with VHPOSR.
- No proposition needs DMA, video or audio.

### 3.3 Waiting and time units

- The program waits by counting changes of VHPOSR bits 15-8 (one change per
  scan line), not by counting loop iterations. p. 245: "Loops that decrement a
  counter to provide delay are not acceptable."
- A scan line lasts "about 63 microseconds", with "227.5 "color clocks", or
  memory access cycles. A memory cycle is approximately 280 ns in duration."
  (p. 194).
- The harness schedules its input changes, and the test measures its windows,
  on the Chip-memory port's time base (`clk_sys` cycles). Neither the pages nor
  the harness state how many `clk_sys` cycles a scan line lasts. Every line
  count that the test or the harness applies (as opposed to the program, which
  counts VHPOSR changes itself) is therefore counted in a page-defined event
  seen at the Chip-memory port: a change of VHPOSR bits 15-8 (p. 238), as the
  program records it. The sampling loop of P1-P8 stores one sample per such
  change (PJ2 checks this), so "n lines after a change" means "from the n-th
  sample stored after it". The harness makes each input change by counting
  samples as they appear at the port.

### 3.4 Reference frame: which controller port is which

The pages use four names for the two ports.

| Name used for the first port | Name used for the second port | Where |
|------------------------------|-------------------------------|-------|
| "left" | "right" | App. A p. 281 ("0=left controller pair, 1=right controller pair"), p. 282 |
| J12, "the left controller port connector (JOY0DAT, POT0DAT)" | J11, "the right controller port connector (JOY1DAT, POT1DAT)" | App. E p. 327 |
| port 1 | port 2 | ch. 8, e.g. p. 230: "JOY0DAT and JOY1DAT. These registers contain counts for ports 1 and 2 respectively." |
| game port 0 | game port 1 | App. F p. 351, for the fire buttons |

In this document "left" means left / port 1 / J12. The rules are:

- Which connector pin each bit of `joystick0`, `joystick1` and
  `mouse_buttons` feeds, and at which level a bit means "pressed", is harness
  setup, not an expected value: the builder reads it from the system top and
  states it in the package. The tests do not infer it from their own results.
- The mapping is part of the stimulus, not a precondition: at the ports it
  shows only through the clauses P1-P8 grade, and a precondition must not
  depend on the clause under test.
- **Consistency record:** P1 and P5 run on the same harness input. Pages 281,
  282 and 327 agree that pins 2 and 4 (JOY0DAT, P1) and pin 5 (POTGOR bit 8,
  P5) belong to the left connector. If exactly one of P1 and P5 passes, the
  report records it. The verdicts stand as graded.
- The fire-button bits of CIA-A are not used as an anchor, because the pages
  disagree about them (section 7.4).

### 3.5 Harness input conventions

- **Direction lines.** Pins 1-4 are up/forward, down/back, left, right (p. 228,
  Table 8-1). The schedules give each pin's level at the connector: 1 = switch
  open, 0 = pressed. p. 232: "When the switches are pressed, the input line is
  shorted to ground. An open switch reads as "1", a closed switch as "0"."
  The harness sets these levels through the input bits and levels of section
  3.4.
- **Pot lines.** Pin 9 carries the second button and pin 5 the third (p. 232).
  The harness drives no line level: the port list gives it one input bit per
  button, pressed or released (`joystick0`, `joystick1`, `mouse_buttons`).
  "Released" is therefore the open switch of p. 241, which drives nothing: "A
  button is a normally open switch that shorts to ground."
- **Inputs not under test.** Every controller input not named by a step is
  released throughout: all direction lines, fire buttons, pot lines, mouse
  movement and the mouse buttons. The harness's mouse count injection stays
  idle.

### 3.6 Configurations

- Every proposition runs on both CPU cores, the second in its default 68000
  mode, in the harness's default configuration only (ECS, 2 MB Chip memory;
  `nanomig_harness.h`, `Config`). No proposition names a chip-set difference:
  p. 380's Denise table distinguishes its two versions only at pin 34, which
  no proposition grades, and the Paula table has no version distinction. The
  harness has no video-standard setting, and no proposition depends on one:
  every interval is counted in scan lines (section 3.3).
- The drive-light record of section 6 needs drive 0. The harness's default
  `floppy_config` is 0, "one drive (DF0), write-enabled, normal speed"
  (`nanomig_harness.h`), which already gives the system drive 0; the record
  uses the harness's default `Config` unchanged. No disk image is mounted:
  the `sdc_*` inputs stay idle as the harness sets them.

### 3.7 Verdicts

| Verdict | Meaning |
|---------|---------|
| pass | every precondition holds and every graded value matches |
| fail | the preconditions hold and a graded value contradicts the page |
| not checkable | a precondition failed. The report names it. It never counts as a pass. |

---

## 4. P1-P4: Denise mouse inputs M0H, M0V, M1H, M1V

### 4.1 Propositions

**P1** (opus:AHJ-178, sol:J-046; p. 380, "09 M0H Mouse 0 horizontal I"):
Denise's M0H is an input. The levels of pins 2 and 4 of the left controller port
enter Denise and determine the state bits and the count of the horizontal
counter in JOY0DAT.

**P2** (opus:AHJ-206, sol:J-074; p. 380, "38 M0V Mouse 0 vertical I"):
Denise's M0V is an input. The levels of pins 1 and 3 of the left port determine
the state bits and the count of the vertical counter in JOY0DAT.

**P3** (opus:AHJ-176, sol:J-044; p. 380, "08 M1H Mouse 1 horizontal I"):
Denise's M1H is an input. Pins 2 and 4 of the right port determine JOY1DAT's
horizontal counter.

**P4** (opus:AHJ-208, sol:J-076; p. 380, "39 M1V Mouse 1 vertical I"):
Denise's M1V is an input. Pins 1 and 3 of the right port determine JOY1DAT's
vertical counter.

p. 281 routes the connector pins to exactly these Denise pins:

| Connector pin | Joystick function | Mouse function | Denise pin | Denise name |
|---------------|-------------------|----------------|------------|-------------|
| L1 | FORW* | Y | 38 | M0V at CCK |
| L3 | LEFT* | YQ | 38 | M0V at CCK* |
| L2 | BACK* | X | 9 | M0H at CCK |
| L4 | RIGH* | XQ | 9 | M0H at CCK* |
| R1 | FORW* | Y | 39 | M1V at CCK |
| R3 | LEFT* | YQ | 39 | M1V at CCK* |
| R2 | BACK* | X | 8 | M1H at CCK |
| R4 | RIGH* | XQ | 8 | M1H at CCK* |

Pin numbers 8, 9, 38 and 39 match p. 380. p. 281 also gives: "Mouse counter
usage: (pins 1,3=Yclock, pins 2,4=Xclock)".

**Per-proposition parameters**

| | Port | pin a | pin b | Register | Counter byte | State bits (hi, lo) |
|---|---|---|---|---|---|---|
| P1 | left | 2 (down) | 4 (right) | JOY0DAT | bits 7-0 | bits 1, 0 |
| P2 | left | 1 (up) | 3 (left) | JOY0DAT | bits 15-8 | bits 9, 8 |
| P3 | right | 2 | 4 | JOY1DAT | bits 7-0 | bits 1, 0 |
| P4 | right | 1 | 3 | JOY1DAT | bits 15-8 | bits 9, 8 |

### 4.2 Stimulus

**Program.**

- It writes no counter register. JOYTEST is not used, except in the PJ3
  contrast of section 4.5.
- It runs a sampling loop: wait until VHPOSR bits 15-8 change, then read
  VHPOSR, JOY0DAT, JOY1DAT and VPOSR, and store the four words as the next
  buffer entry.
- The loop stores exactly TOTAL_SAMPLES = 2,048 samples, the length of the four
  phases (4 x 512 lines); the harness applies each phase's first schedule
  entry at that phase's first sample. The program then writes a done marker.

**Harness,** on the proposition's pins a and b only, holding each entry for at
least 8 scan lines. Here and below, lines are counted as samples stored at the
Chip-memory port after the change (section 3.3):

1. **Baseline:** a = b = 1 for at least 16 lines.
2. **Forward:** at least 6 cycles of (a,b) = (0,1), (0,0), (1,0), (1,1). For
   P1 one forward cycle is: press pin 2, press pin 4, release pin 2, release
   pin 4.
3. **Backward:** at least 9 cycles of (a,b) = (1,0), (0,0), (0,1), (1,1).
4. **Final:** a = b = 1 for at least 16 lines.

Every change of the schedule changes exactly one pin. The pages define the count
only for single steps (Figure 8-2). A change of both pins at once has no
expected count and must not occur.

The four propositions may share one run, one phase each, each phase with its
own baseline. All other inputs stay as in section 3.5.

### 4.3 Observation

- **Data:** the stored samples, taken from the Chip-memory port with their
  store times.
- **Hold window of step k:** the samples stored after t_k and before t_(k+1),
  without the first two stored after t_k (the 2-line guard). t_k is the
  harness time of the change that starts step k; for a baseline it is the
  phase's first sample.
- **Why two lines of guard:** Denise samples the pins at every colour clock
  (p. 281, "at CCK", "at CCK*"). The guard covers the delay between a register
  read and its store.
- Only samples inside hold windows are graded.

### 4.4 Expected values

The following hold for **every** graded sample in **every** window.

**E1: state bits.** The (hi, lo) state bits are fixed by the pin levels:

| a | b | hi | lo |
|---|---|----|----|
| 1 | 1 | 0 | 0 |
| 0 | 1 | 0 | 1 |
| 0 | 0 | 1 | 0 |
| 1 | 0 | 1 | 1 |

Sources:

- p. 281: "Bits 1 and 0 of each counter may be read to determine the state of
  these two clock pins."
- p. 281: "(Note that the joystick functions are all "active low" at the
  connector pins.)" and "The LEFT and RIGHT joystick functions (active high)
  are directly available on the Y1 and X1 bits of each counter. In order to
  recreate the FORWARD and BACK joystick functions, however, it is necessary to
  logically combine (exclusive OR) the lower two bits of each counter."
- p. 281's table: "Forward Y1 xor Y0 (BIT#09 xor BIT#08) / Left Y1 / Back X1 xor
  X0 (BIT#01 xor BIT#00) / Right X1". Hence hi = NOT b and hi xor lo = NOT a.
- The same table appears under Figure 8-2 (p. 230), with a = V and b = VQ:
  "V VQ : D1 D0 / 0 0 : 1 0 / 0 1 : 0 1 / 1 0 : 1 1 / 1 1 : 0 0".
- Table 8-3 (p. 232) and p. 329 agree.
- Figure 8-2 is drawn for V/VQ. p. 281's X relations give the identical table
  for the horizontal pair.

**E2: count.** The counter byte equals (c0 + n) mod 256, where:

- c0 is the byte in the baseline window;
- n is the net number of steps applied before the window, forward +1 and
  backward -1.

Sources:

- p. 229: "Pulses entering the mouse inputs are converted to separate
  horizontal and vertical counts. The 8 bit wide horizontal and vertical
  counter registers can track mouse movement without processor intervention."
- Figure 8-2 (p. 230) draws "Case 1: Count Up" and "Case 2: Count Down". D0
  changes at every edge of V or VQ, D1 at every second edge, D2 at every
  fourth. The counter therefore changes by one per edge.
- In Case 1 the inputs run V rise, VQ rise, V fall, VQ fall. That is the
  forward cyclic order above, in which D1 D0 runs 0, 1, 2, 3, 0.
- p. 231: "These counters will "wrap around" in either the positive or negative
  direction."

**E3: independence.**

- The other byte of the same register equals its baseline value.
- Both bytes of the other port's register equal their baseline values.

Sources: p. 229 ("separate horizontal and vertical counts"); p. 281 ("Each
counter is clocked by signals from two controller pins", "0=left controller
pair, 1=right controller pair").

**Quantifiers.**

- "each counter" (p. 281) is graded through all four propositions together.
- "every" graded sample is required.
- One window does not establish any state. Each of the four (a,b) states occurs
  at least 15 times per phase, in both directions of travel.

### 4.5 Preconditions

| Id | Precondition |
|----|--------------|
| PJ1 | The harness input log shows the scheduled changes, at the scheduled times, on the proposition's two pins. It shows no other controller-port input, mouse movement or mouse-button activity. |
| PJ2 | Every hold window contains at least 3 graded samples. VHPOSR bits 15-8 of consecutive samples differ by +1 modulo 256, or return to $00 at the start of a frame, which VPOSR bit 0 (V8, p. 238) shows by going from 1 in the earlier sample to 0 in the later one. Every gap between the store times of consecutive samples is at most 1.5 times the run's median gap: one sample is stored per line, so a line the loop missed, also a frame's last line, doubles a gap. So the loop kept pace. |
| PJ3 | In the baseline window all samples of JOY0DAT and JOY1DAT are identical: nothing else moves the counters. |

The harness input that carries each pin is fixed by section 3.4 as part of the
stimulus. It is not a precondition, because at the ports it shows only through
E1-E3.

**Contrasting case for each precondition:**

| Id | Change | Result |
|----|--------|--------|
| PJ1 | Drop one scheduled change from the harness while the grader keeps its plan. | PJ1 reports the missing change. |
| PJ1 | Apply one phase's schedule to the corresponding pins of the other port. | PJ1 reports the port actually driven, which is not the proposition's. |
| PJ2 | Shorten one hold to 2 scan lines. | The window has fewer than 3 graded samples, so PJ2 is not met. |
| PJ2 | Leave line 255 out of the sampling loop, so that a stored sample ends at VHPOSR bits 15-8 = $00 with V8 going from 0 to 1. | The return to $00 is not at a frame start, so PJ2 is not met. |
| PJ2 | Leave the frame's last line out of the sampling loop (the program learns that line from one frame wrap before it starts sampling). | The return to $00 still shows V8 going from 1 to 0, but the gap before it is about twice the median, so PJ2 is not met. |
| PJ3 | The program writes JOYTEST once inside the baseline window, which sets all four joystick-mouse counters at once. The harness input log is unchanged, so PJ1 and PJ2 stay met. | The baseline samples differ, so PJ3 is not met. |
| PJ1 | Inject mouse counts during the baseline, through the port list's single mouse input (`kbd_mouse_*`), on whichever connector it feeds. | PJ1 reports the mouse movement, and the baseline samples differ, so PJ3 is not met either. |

### 4.6 Contrasting cases

In each case below the program is unchanged and the harness departs from the plan that PJ1 checks (4.5).

1. **Other port.** Apply the same schedule to the corresponding pins of the
   other port. The byte under test stays at its baseline in every window, and
   the schedule shows up in the other register instead.
2. **Other axis.** Apply the schedule to the other pin pair of the same port
   (pins 1/3 instead of 2/4, or the reverse). The byte under test stays at its
   baseline.
3. **Direction.** Swap the forward and backward phases. The byte runs through
   c0 - n instead of c0 + n, so the direction of counting is graded.

Case 1 is run as P1-WRONGPORT to P4-WRONGPORT: PJ1 reports the port actually
driven, and P1-P4 are not checkable (8.4). Cases 2 and 3 are not run. The
register-side counterpart of case 1 leaves PJ1 true:
the program stores JOY1DAT and JOY0DAT in the other order for the phase under
test. The register that the proposition reads then holds the other port's
count, and the byte under test stays at its baseline in every window.

### 4.7 Reference frames

- **Ports:** section 3.4.
- **Time:** harness change times and store times on the Chip-memory port's time
  base, with the 2-line guard of section 4.3.
- **State order:** the cyclic order of Figure 8-2's "Case 1: Count Up". The
  anchor check is E1: each state's bits must match the figure's table before
  any count is graded.

---

## 5. P5-P8: Paula pot pins POT0X, POT0Y, POT1X, POT1Y

### 5.1 Propositions

**P5** (opus:AHJ-243, sol:J-110; p. 380, "32 POT0X Pot 0X I/O"): Paula's POT0X
pin is bidirectional. With POTGO bit 9 (OUTLX) at 1, Paula drives pin 5 of the
left port at the level of bit 8 (DATLX). POTGOR bit 8 reports that pin's actual
level, including a level forced by a button on the pin.

**P6** (opus:AHJ-245, sol:J-112; p. 380, "33 POT0Y Pot 0Y I/O"): the same for
pin 9 of the left port, with OUTLY (bit 11) and DATLY (bit 10).

**P7** (opus:AHJ-249, sol:J-116; p. 380, "35 POT1X Pot 1X I/O"): the same for
pin 5 of the right port, with OUTRX (bit 13) and DATRX (bit 12).
**Withdrawn** (8.2): the input half of this claim cannot be set up at this
boundary. The right port's only input is `joystick0`; Table 8-1 (p. 228)
gives a joystick nothing on pin 5 ("---"). The claim id is recorded, not
graded; C1, C3, C4 and C8 (the output half) are measured without a claim
id.

**P8** (opus:AHJ-251, sol:J-118; p. 380, "36 POT1Y Pot 1Y I/O"): the same for
pin 9 of the right port, with OUTRY (bit 15) and DATRY (bit 14).

**Supporting pages.**

- p. 282, connector to Paula:

  | Loc. | Dir. | Sym | Pin | Paula pin | Paula name |
  |------|------|-----|-----|-----------|------------|
  | RIGHT | Y | RY | 9 | 36 | (POT1Y) |
  | RIGHT | X | RX | 5 | 35 | (POT1X) |
  | LEFT | Y | LY | 9 | 33 | (POT0Y) |
  | LEFT | X | LX | 5 | 32 | (POT0X) |

- p. 282, POTGO/POTGOR bits by Paula pin: "15 OUTRY Output enable for Paula
  pin 36 / 14 DATRY I/O data Paula pin 36 / ... / 09 OUTLX Output enable for
  Paula pin 32 / 08 DATLX I/O data Paula pin 32".
- p. 240, Table 8-4, by port and pin: DATLX "data for port 1, pin 5 (middle
  mouse button)", DATLY "data for port 1, pin 9 (right mouse button)", DATRX
  "data for port 2, pin 5", DATRY "data for port 2, pin 9".
- The Paula pin numbers match p. 380.

### 5.2 Stimulus

**Base value B = $FF00.** All four OUT and DAT bits are 1, bits 7-1 are 0, and
START (bit 0) is 0. This is p. 241's button setup applied to all four pins:
"The Amiga must provide a pull-up resistance on the sense pin. To do this, set
the proper pin to output, and drive the line high (set both OUT... and DAT...
to 1)." START stays 0 throughout; the pot counters are not part of this check.
p. 236: "All counts are reset to zero when POTGO is written with bit zero
high."

**Steps.** For the pin under test, with OUT bit o and DAT bit d:

| Step | POTGO value | Button on the pin under test |
|------|-------------|------------------------------|
| C1 | B | released |
| C2 | B | pressed |
| C3 | B | released |
| C4 | B with bit d = 0 | released |
| C5 | B with bit d = 0 | pressed |
| C6 | B with bits o and d = 0 (pin is an input) | pressed |
| C7 | B with bits o and d = 0 | released |
| C8 | B | released |

**How the steps are driven.**

- Each step changes either POTGO (a program write) or the button (a harness
  change), never both.
- The program writes POTGO (a word write) at fixed line counts. It follows each
  write with a marker that holds the POTGO value written.
- Throughout the run the program samples VHPOSR and POTGOR once per scan line
  into the buffer.
- The harness changes the button only inside steps without a POTGO write.
- Each step lasts at least 24 scan lines. Here and below, lines are counted
  as samples stored at the Chip-memory port (section 3.3).
- The four propositions may share one run, one phase per pin.

### 5.3 Observation

- **Data:** the stored POTGOR samples.
- **Graded samples:** those stored after the latest change (the POTGO marker
  or the harness button change) and before the next change, without the first
  six stored after it.
- **Why 6 lines:** p. 240 says "There are large capacitors on these lines, and
  it can take up to 300 microseconds for the line to change state." 300 us is
  less than 5 scan lines of "about 63 microseconds" (p. 194). One more line
  covers the delay between a read and its store.

### 5.4 Expected values

The following hold for **every** graded sample of the step.

| Step | POTGOR bit d | Source |
|------|--------------|--------|
| C1, C3, C8 | 1 | p. 241: "Reading POTINP will produce a 0 if the button is pressed, a 1 if it is not." |
| C2 | 0 | p. 241 (same sentence) |
| C4 | 0 | p. 240: "If you set the output enable for any pin to a 1, the Amiga disconnects the potentiometer control circuitry from the port, and configures the pin for output. The state of the data bit controls the logic level on the output pin." p. 282 names the bit "I/O data Paula pin nn". |
| C5 | 0 | output low (p. 240) and button to ground (p. 241) |
| C6 | 0 | p. 240: "To use the entire register as an input, sensing the current state of the pot pins, write all 0s to POTGO. Thereafter you can read the current state by using read-only address POTINP."; the pressed button grounds the pin (p. 241) |
| C7 | **no expected value**; record only | With the pin as input nothing on the pages drives it. p. 241 says the pull-up exists only when the Amiga drives the line. |

C4 rests on section 3.5: a released button is the open switch of p. 241 and
drives nothing, and the harness has no other state for it. The only level on
the pin in C4 is the one the data bit sets.

**The other bits.**

- In every step, the other three DAT bits read 1: their pins stay in the C1
  configuration with their buttons released (p. 241).
- Not graded: POTGOR bits 15, 13, 11 and 9 (no page says what the OUT bits read
  back as), bits 7-1 (the pages disagree, section 7.5) and bit 0.

**Quantifiers.** p. 240 says "any pin", so all four pins are graded (P5-P8).
"Every" graded sample is required. C1 and C3 both grade "a 1 if it is not"
after a press.

### 5.5 Preconditions

| Id | Precondition |
|----|--------------|
| PP1 | The harness input log shows the button changes on the pin under test only. Each change lies in its intended step, with at least 6 samples stored between it and each of that step's markers. Every other controller input and the mouse are idle. |
| PP2 | The POTGO values stored with the markers are the planned ones, and bit 0 is 0 in all of them. |
| PP3 | Every graded window holds at least 3 samples. |

As in section 4.5, the harness input that carries each pin is fixed by section
3.4 as part of the stimulus, not checked as a precondition.

**Contrasting case for each precondition:**

| Id | Change | Result |
|----|--------|--------|
| PP1 | Omit the press of C2. | PP1 reports the scheduled change as missing instead of grading C2. |
| PP1 | Apply one pin's press to the other pot pin of the same port, or to the corresponding pin of the other port. | PP1 reports the source and bit actually driven, which are not the pin under test's. |
| PP2 | Write POTGO with bit 0 = 1 in C1. | PP2 is not met. |
| PP3 | Shorten one step to 6 lines. | PP3 is not met. |

### 5.6 Contrasting cases

Cases 1 and 2 are comparisons within the base run: the program finishes and
the preconditions hold.

1. **Output.** C4 differs from C1 only in DAT bit d, with the button released.
   The page predicts 0 in C4 and 1 in C1. An implementation whose pin only
   follows the external level reads 1 in C4 and fails.
2. **Input path.** C2 differs from C1 only in the button. The page predicts 0
   in C2 and 1 in C1. A register that merely echoed POTGO reads 1 in C2 and
   fails.
3. **Binding.** Repeat C2 with the press on the corresponding pin of the other
   port, or on the other pot pin of the same port. Bit d stays 1, and the
   pressed pin's own DAT bit reads 0.

Case 3 is run as P5-WRONGPIN, P6-WRONGPIN and P8-WRONGPIN: PP1 reports the
source and bit actually driven, and P5, P6 and P8 are not checkable (8.4). Its
register-side counterpart leaves PP1 true: for the samples of the pin under
test, the program stores POTGOR with bit d replaced by the DAT bit of the other
pot pin of the same port (P5: bit 10, P6: bit 8, P8: bit 12). That pin is
released and driven high throughout, so p. 241 ("a 1 if it is not") and p. 240
predict 1 where the pin under test's own bit reads 0 (C2, C4, C5, C6), and the
proposition fails there; the other three DAT bits are read unchanged.

### 5.7 Reference frames

- **Ports:** section 3.4.
- **Steps:** step windows are bounded by the program's POTGO markers and the
  harness's button-change times, on the Chip-memory port's time base.
- **Anchor check:** each marker's POTGO value equals the planned value for its
  step (PP2).

---

## 6. Record: the drive light (no claim, not graded)

No claim of this package is graded on the drive light (section 1.6). The
package records what the port list's `fdd_led` output showed, in one
`RECORDED:` line per core, with no verdict, precondition or contrast.

**Stimulus.**

1. Write BPLCON0 = $0000 (section 3.2).
2. Write CIA-B PRB = $FF, then DDRB = $FF. p. 354 says that after reset "The
   port pins are set as inputs and port registers to zero (although a read of
   the ports will return all highs because of passive pull-ups)." Writing DDRB
   first would drive all eight lines low at once, and p. 244 says: "All
   software that selects drives must set up the motor signal before selecting
   any drives."
3. Write PRB with the values below, once each, in order. Each write is
   followed by a marker at `RESULTS` and held for 64 changes of VHPOSR bits
   15-8. Bits 2-0 stay 1: no side change, DIR high, no step pulse. The order
   follows p. 369: "When the system wants to turn on a disk drive motor, it
   first deselects the drive (if selected), pulls MTRXD- low, and selects the
   drive. To turn the motor off, the system deselects the drive, pulls MTRXD-
   high, and selects the drive."
4. DONE.

| Step | PRB | Change |
|------|-----|--------|
| S0 | $FF | motor line high, no drive selected |
| S1 | $7F | motor line low, no drive selected |
| S2 | $77 | select drive 0 (motor line low) |
| S3 | $7F | deselect |
| S4 | $FF | motor line high, no drive selected |
| S5 | $F7 | select drive 0 (motor line high) |
| S6 | $FF | deselect |
| S7 | $7F | motor line low, no drive selected |
| S8 | $FF | motor line high |

**Record.** The level of `fdd_led` in the `clk_sys` cycle before each next
marker (before the done mark for S8), S0 to S8, as read at the port. The page
context is p. 244: "Each drive will latch the motor signal at the time its
select signal turns on. The disk drive motor will stay in this state until
the next time select turns on. DSKMOTOR* also controls the activity light on
the front of the disk drive." No claim of this package carries these
sentences.

---

## 7. Where the pages disagree or leave the point open

### 7.1 Direction of LP*

The pages give the direction of LP* in three places:

- p. 379 (Original Agnus): "37 LP* Light pen input I".
- p. 381 (Fat Agnus): "78 LP* Light pen O".
- Chapter 8 (pp. 238-239) describes the pen trigger on the A500, A2000 and
  A3000 as a signal taken in, which freezes VPOSR.

Consequence: no proposition (sections 1.2 and 1.3).

### 7.2 Device I.D.

p. 371 and p. 335 differ on three points (quoted in section 1.1):

- whether the RDY- samples are inverted;
- what $0000 0000 means;
- whether an extra select pulse (steps 5 and 6) precedes the read loop.

Both pages print the label as "3.25". p. 241 says "One 3.5" drive is installed
in the basic unit." The label is left as printed.

### 7.3 The drive activity light

p. 244: "DSKMOTOR* also controls the activity light on the front of the disk
drive." The page does not say whether the light follows the latched motor state
or the line itself. Nothing in this package is graded on the light; section 6
only records it.

### 7.4 Port numbering and the fire-button bits

The pages disagree on which CIA-A bit serves which port's fire button:

- p. 232: "Port 1 uses bit 6 and port 2 uses bit 7."
- App. F p. 351 and App. E p. 337: "PA7..game port 1, pin 6 (fire button*)"
  and "PA6..game port 0, pin 6 (fire button*)".
- App. E p. 329: "PRA0 bit 7 = Fire* left controller port / PRA0 bit 6 = Fire*
  right controller port".

Consequence: no test here uses the fire bits (section 3.4).

### 7.5 POTGOR bits 7-1

- p. 240: "chip revision identification number".
- p. 282: "Reserved for chip ID code (presently 0)".

Consequence: not graded.

### 7.6 CIA-B port B naming

p. 244 prints "CIAAPRB ($BFD100)". p. 241 lists "CIABPRB ($BFD100)", and p. 340
places prb at BFD100 in the CIAB map. The address is used.

---

## 8. Build

Three programs, `prog_mouse.s` (section 4), `prog_pot.s` (section 5) and
`prog_drivelight.s` (section 6), run by `test_appHJ_drive_pins.cpp`. The
contrasts of 8.4 are wrappers that set their contrast's symbols (one each;
`prog_pot_p5.s`, `prog_pot_p6.s` and `prog_pot_p8.s` set three: WRONGREG,
WRONGBIT and OTHERBIT) and include a base program:
`prog_mouse_pj2.s` (PJ2-FRAME), `prog_mouse_pj2e.s` (PJ2-FRAMEEND),
`prog_mouse_pj3.s` (PJ3) and
`prog_mouse_sw0.s` to `prog_mouse_sw3.s` (P1-WRONGREG to P4-WRONGREG) include
`prog_mouse.s`; `prog_pot_pp2.s`, `prog_pot_pp3.s` (PP2, PP3) and
`prog_pot_p5.s`, `prog_pot_p6.s`, `prog_pot_p8.s` (P5-, P6-, P8-WRONGREG)
include `pot_body.s`, as `prog_pot.s` does. Fixture, buffer and CIA-B
addresses used by the programs are in `hj_defs.i`.

### 8.1 Stimulus mapping (section 3.4, 5.1: assigned to the builder)

Established by matching each harness input's observed bit pattern against
what Table 8-1 (p. 228) and Figure 8-2 (p. 230) name for a connector pin,
not by treating "some register moved" as sufficient on its own:

- `joystick1` drives JOY0DAT (the left port, P1/P2); `joystick0` drives
  JOY1DAT (the right port, P3/P4). Confirmed against Figure 8-2's state
  table (E1, section 4.4).
- Within either port, bit 3 = up (pin 1), bit 2 = down (pin 2), bit 1 = left
  (pin 3), bit 0 = right (pin 4), each bit active high at this boundary
  (1 = pressed).
- `mouse_buttons` has no bit-to-button correspondence named anywhere at this
  boundary (neither the port declaration nor `nanomig_harness.h`). POTGOR
  bit 8 (POT0X/DATLX) follows `mouse_buttons` bit 2; bit 10 (POT0Y/DATLY)
  follows `mouse_buttons` bit 1 (`joystick1` bit 5 also reaches it; this
  build presses only the mouse bit). The two bits are matched to pins 5 and
  9 because Table 8-1 gives the left connector's pins 5 and 9 the mouse's
  middle and right buttons, and no other device on either port offers a
  second independently switched pin-5/pin-9 pair. Bit 14 (POT1Y/DATRY)
  follows `joystick0` bit 5.
- POTGOR bit 12 (POT1X/DATRX) follows no harness input this package can
  drive. This is not an unmatched bit: Table 8-1 gives a joystick nothing on
  pin 5 ("---"), and the right port's only input at this boundary is
  `joystick0`, a joystick-shaped port. No device the page puts on pin 5 (the
  mouse's middle button, a proportional pair's right POT, an X-Y joystick's
  POT X, a light pen) is offered at the right port. See 8.2.
- The harness's default `floppy_config` (0) already selects drive 0
  (`nanomig_harness.h`'s own `Config` comment documents 0 as "one drive
  (DF0), write-enabled, normal speed"). Section 6 uses the harness's
  default `Config` unchanged.

### 8.2 P7 (POT1X): withdrawn -- no harness input reaches its input half

C2 and C6 read the same level as C1, C3, C4 and C8 regardless of any input
this package can drive: the input half of the claim (a pressed button
forcing the pin) cannot be set up. C5 is not part of this: it reads the
level p.240 predicts (0), because the program's own OUTPUT write in C4 is
still driving the pin low, not because any button was read.

P7 is withdrawn to a RECORDED line (5.1): the right port's only input is
`joystick0` (8.1), and Table 8-1 (p. 228) gives a joystick nothing on pin 5
("---"). C1, C3, C4 and C8 (the output half, unaffected by the missing
input) are measured without a claim id.

### 8.3 Measured results (`out_fx68k.txt`, `out_tg68k.txt`)

Both cores: 19 passed, 0 failed, identically. FIXTURE (x3), the three DONE
checks, PJ1, PJ2, PJ3, PP1, PP2, PP3 and P1, P2, P3, P4, P5, P6, P8 all
pass. P7 is withdrawn to a RECORDED line (8.2), with its output steps (C1,
C3, C4, C8) reported as a separate, id-less MEASURED note. The 11 declined
claims of section 1 print as RECORDED lines. Section 6 fdd_led is recorded,
not graded:

    RECORDED: section 6 [p.244]: fdd_led level just before the next marker,
    S0..S8 (motor/select sequence, drive 0): S0=off S1=off S2=off S3=off
    S4=off S5=off S6=off S7=off S8=off
    MEASURED: section 6 markers observed (PRB byte written, zero-extended):
    $00FF $007F $0077 $007F $00FF $00F7 $00FF $007F $00FF

fdd_led never moves over the whole sequence. Both cores read identically.

PJ2 prints the store-time gap between consecutive samples:

    MEASURED: PJ2 store-time gap between consecutive samples, in clock
    cycles: median 1768, maximum 1864 (fx68k)
    MEASURED: PJ2 store-time gap between consecutive samples, in clock
    cycles: median 1848, maximum 1848 (TG68K)

The maximum is within 1.5 times the median on both cores; under PJ2-FRAME and
PJ2-FRAMEEND it is about twice the median (two lines).

The DAT bit of each pot pin per step (one value per step; C7 is not graded)
prints once per core, identically:

    MEASURED: P5-P8 observed DAT bit per step (C1..C8; - = not graded,
    0/1 = the samples of the step differ, ? = no sample):
    P5 [C1=1 C2=0 C3=1 C4=0 C5=0 C6=0 C7=- C8=1]
    P6 [C1=1 C2=0 C3=1 C4=0 C5=0 C6=0 C7=- C8=1]
    P7 [C1=1 C2=1 C3=1 C4=0 C5=0 C6=1 C7=- C8=1]
    P8 [C1=1 C2=0 C3=1 C4=0 C5=0 C6=0 C7=- C8=1]

The RECORDED line of opus:AHJ-326 states the boundary reason alone:
"declined -- an internal output pin's direction has no observable effect at
this boundary: the test sees no chip pin, and no page names a register or
output effect of Agnus driving LP*".

### 8.4 Mutations (both cores, identically; `NMH_MUTATE=<value>`, header
comment of `test_appHJ_drive_pins.cpp`)

Every mutation runs the target precondition's or proposition's own
comparator unchanged; a mutation never inverts a verdict. The graded
propositions (P1-P6 and P8) are gated on their section's own preconditions
(spec 3.7): when any of a section's preconditions fails, that section's
graded propositions print as a `RECORDED: ... not checkable` line instead of
PASS/FAIL, and are not counted in the totals below. P7 is withdrawn (8.2)
and prints no such line.

Proposition (stimulus) mutations print a literal `FAIL: <label>` line
through the named proposition's own comparator, nothing else affected. The
action record and every other precondition stay true:

| Mutation | Prints FAIL through | Totals (both cores) |
|----------|---------------------|----------------------|
| P1-WRONGREG | `P1 [p.380: ...]` | 14 passed, 1 failed (P1) |
| P2-WRONGREG | `P2 [p.380: ...]` | 14 passed, 1 failed (P2) |
| P3-WRONGREG | `P3 [p.380: ...]` | 14 passed, 1 failed (P3) |
| P4-WRONGREG | `P4 [p.380: ...]` | 14 passed, 1 failed (P4) |
| P5-WRONGREG | `P5 [p.380: ...]` | 14 passed, 1 failed (P5) |
| P6-WRONGREG | `P6 [p.380: ...]` | 14 passed, 1 failed (P6) |
| P8-WRONGREG | `P8 [p.380: ...]` | 14 passed, 1 failed (P8) |

In P5-, P6- and P8-WRONGREG the stored bit of the pin under test reads 1 in
every step, on both cores, so the proposition fails at C2, C4, C5 and C6.

Mutations that drive the wrong port or pin print `FAIL: PJ1 ...` or
`FAIL: PP1 ...`, because the action record shows the port, source or bit
actually driven, and gate their section's propositions to "not checkable":

| Mutation | Prints FAIL through | Side effect measured | Totals (both cores) |
|----------|---------------------|----------------------|----------------------|
| P1-WRONGPORT | `PJ1 [...]` | P1, P2, P3, P4 all become "not checkable" | 10 passed, 1 failed (PJ1) |
| P2-WRONGPORT | `PJ1 [...]` | as P1-WRONGPORT | 10 passed, 1 failed (PJ1) |
| P3-WRONGPORT | `PJ1 [...]` | as P1-WRONGPORT | 10 passed, 1 failed (PJ1) |
| P4-WRONGPORT | `PJ1 [...]` | as P1-WRONGPORT | 10 passed, 1 failed (PJ1) |
| P5-WRONGPIN | `PP1 [...]` | P5, P6, P8 all become "not checkable" | 11 passed, 1 failed (PP1) |
| P6-WRONGPIN | `PP1 [...]` | as P5-WRONGPIN | 11 passed, 1 failed (PP1) |
| P8-WRONGPIN | `PP1 [...]` | as P5-WRONGPIN | 11 passed, 1 failed (PP1) |

Precondition mutations print `FAIL: <id> ...` through the precondition's own
comparator (the same label the normal run uses, PASS there), and gate their
whole section's propositions to "not checkable":

| Mutation | Prints FAIL through | Side effect measured | Totals (both cores) |
|----------|---------------------|------------------------|----------------------|
| PJ1 | `PJ1 [...]` | P1, P2, P3, P4 all become "not checkable" (section 4 gated) | 10 passed, 1 failed (PJ1) |
| PJ2 | `PJ2 [...]` | P1, P2, P3, P4 all become "not checkable" | 10 passed, 1 failed (PJ2) |
| PJ2-FRAME | `PJ2 [...]` | P1, P2, P3, P4 all become "not checkable" | 10 passed, 1 failed (PJ2) |
| PJ2-FRAMEEND | `PJ2 [...]` | P1, P2, P3, P4 all become "not checkable" | 10 passed, 1 failed (PJ2) |
| PJ3 | `PJ3 [...]` | P1, P2, P3, P4 all become "not checkable"; PJ1 and PJ2 stay met | 10 passed, 1 failed (PJ3) |
| PJ3-KBDMOUSE | `PJ1 [...]` and `PJ3 [...]` | the injected mouse count is the mouse movement PJ1's own idle clause forbids; P1, P2, P3, P4 all become "not checkable" | 9 passed, 2 failed (PJ1, PJ3) |
| PP1 | `PP1 [...]` | P5, P6, P8 all become "not checkable" (section 5 gated) | 11 passed, 1 failed (PP1) |
| PP2 | `PP2 [...]` | P5, P6, P8 all become "not checkable" | 11 passed, 1 failed (PP2) |
| PP3 | `PP3 [...]` | P5, P6, P8 all become "not checkable" | 11 passed, 1 failed (PP3) |

P7 is not part of any mutation replay above: its RECORDED withdrawal and
id-less output-steps measurement print only in the normal run (8.3), gated
the same way section 6's record is.
