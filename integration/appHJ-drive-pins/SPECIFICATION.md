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
  also serves no drive. The claim needs no data from a disk surface; what blocks
  it is the missing device.
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

- **A pin's direction cannot be seen.** A test could observe only the function:
  a trigger that freezes VPOSR/VHPOSR (pp. 238-239). This claim does not state
  that function.
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

- **An output direction has nothing the test can see.** The test sees no chip
  pin. No page names an effect of Agnus driving LP* that shows at an output or
  in a register.
- **The pages contradict each other** (see section 8.1). p. 379 gives the same
  signal on the Original Agnus as "Light pen input I". Chapter 8 describes the
  light pen of the A500, A2000 and A3000 as a trigger that Agnus takes in:
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
  direction.
- **The function column cannot be checked either.** "28MHZ clk divided by four"
  describes the function, not the claim, which states the direction. Checking
  that ratio would need the test's time base ("system clock cycles") related to
  the 28 MHz master clock. Neither the claim nor the harness description gives
  that relation.

### 1.5 Part of opus:AHJ-090, sol:H-064 (p. 370): "approximately 17 microseconds"

The duration of the DRESB- pulse after a RESET instruction gives no expected
value:

- the test sees neither pin 10 of the disk connector nor the system reset line;
- the drive light shows only the state after the reset;
- "approximately" carries no tolerance.

Proposition P10 uses the rest of the claim: a RESET instruction activates the
system reset.

### 1.6 Disk surface

No claim in this package needs data read from or written to a disk surface.
Propositions P9 and P10 need a drive 0 with a motor and an activity light, but
no disk. If the harness does not model drive 0 without a disk image, their
preconditions fail and they report "not checkable" (sections 6.5 and 7.5).

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
| P7 | Paula POT1X (right port, pin 5) is bidirectional | opus:AHJ-249, sol:J-116 | 380 |
| P8 | Paula POT1Y (right port, pin 9) is bidirectional | opus:AHJ-251, sol:J-118 | 380 |
| P9 | A drive's motor follows its select-clocked flip-flop, not the motor line | opus:AHJ-085, sol:H-060 | 370 |
| P10 | A RESET instruction activates the system reset, which turns a latched-on drive motor off | opus:AHJ-090, sol:H-064 | 370 |

---

## 3. Common setup

### 3.1 Registers

**Custom registers** (Appendix B, p. 289; the $DFF000 base is from the
addresses on p. 229):

| Register | Address | Access | Page row |
|----------|---------|--------|----------|
| VHPOSR | $DFF006 | R | "Read vert and horiz. position of beam" (bits 15-8 are V7-V0, p. 238) |
| JOY0DAT | $DFF00A | R | "Joystick-mouse 0 data (vert,horiz)" |
| JOY1DAT | $DFF00C | R | "Joystick-mouse 1 data (vert,horiz)" |
| POTGOR | $DFF016 | R | "Pot port data read (formerly POTINP)" |
| POTGO | $DFF034 | W | "Pot port data write and start" |
| BPLCON0 | $DFF100 | W | bit 3 "enables the light pen latch" (p. 229) |

**CIA-B registers** (p. 340). These are not custom registers. P9 and P10 need
them. If the harness does not map CIA-B at these addresses, P9 and P10 cannot
run.

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
  on the Chip-memory port's time base (system clock cycles). The builder
  converts scan lines to that base with the harness's own clock definition.

### 3.4 Reference frame: which controller port is which

The pages use four names for the two ports.

| Name used for the first port | Name used for the second port | Where |
|------------------------------|-------------------------------|-------|
| "left" | "right" | App. A p. 281 ("0=left controller pair, 1=right controller pair"), p. 282 |
| J12, "the left controller port connector (JOY0DAT, POT0DAT)" | J11, "the right controller port connector (JOY1DAT, POT1DAT)" | App. E p. 327 |
| port 1 | port 2 | ch. 8, e.g. p. 230: "JOY0DAT and JOY1DAT. These registers contain counts for ports 1 and 2 respectively." |
| game port 0 | game port 1 | App. F p. 351, for the fire buttons |

In this document "left" means left / port 1 / J12. The rules are:

- The builder takes from the harness documentation which harness port is the
  manual's left port. The tests do not infer the mapping from their own
  results.
- **Anchor check, independent of the mapping:** P1 and P5 run on the same
  harness port. The port whose pins 2 and 4 move JOY0DAT (P1) must be the port
  whose pin 5 moves POTGOR bit 8 (P5). Pages 281, 282 and 327 agree that both
  belong to the left connector. If exactly one of P1 and P5 passes on the port
  the harness calls left, the mapping is reported as inconsistent.
- The fire-button bits of CIA-A are not used as an anchor, because the pages
  disagree about them (section 8.4).

### 3.5 Harness input conventions

- **Direction lines.** Pins 1-4 are up/forward, down/back, left, right (p. 228,
  Table 8-1). The harness drives their digital level: 1 = switch open, 0 =
  pressed. p. 232: "When the switches are pressed, the input line is shorted to
  ground. An open switch reads as "1", a closed switch as "0"."
- **Pot lines.** Pin 9 carries the second button and pin 5 the third (p. 232).
  "Released" is taken to be the open switch of p. 241: "A button is a normally
  open switch that shorts to ground." If the harness drives the line high when
  released, the cases marked in section 5.4 have no expected value.
- **Inputs not under test.** Every controller input not named by a step is
  released throughout: all direction lines, fire buttons, pot lines, mouse
  movement and the mouse buttons. The harness's mouse count injection stays
  idle.

### 3.6 Configurations

- Every proposition runs on both CPU cores, the second in its default 68000
  mode, with PAL and 2 MB Chip memory.
- P1-P8 run under ECS and again under OCS. p. 380's Denise table covers both
  Denise versions and distinguishes them only at pin 34. The Paula table has no
  version distinction.
- P9 and P10 run under the default ECS. Chapter 8 and Appendix H make no
  chip-set distinction.
- The expected values are the same in every configuration.

### 3.7 Verdicts

| Verdict | Meaning |
|---------|---------|
| pass | every precondition holds and every graded value matches |
| fail | the preconditions hold and a graded value contradicts the page |
| not checkable | a precondition failed. The report names it. It never counts as a pass. |
| no verdict | P9 only: the observed light matches a reading of p. 244 under which the flip-flop is not visible (section 6.4) |

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

- It writes no counter register. JOYTEST is not used.
- It runs a sampling loop: wait until VHPOSR bits 15-8 change, then read
  VHPOSR, JOY0DAT and JOY1DAT, and store the three words as the next buffer
  entry.
- The loop starts before the harness schedule and runs for a fixed number of
  lines longer than the schedule. The program then writes a done marker.

**Harness,** on the proposition's pins a and b only, holding each entry for at
least 8 scan lines:

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
- **Hold window of step k:** stores with time in [t_k + 2 scan lines, t_(k+1)),
  where t_k is the harness time of the change that starts step k.
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
| PJ2 | Every hold window contains at least 3 graded samples. VHPOSR advances by one line between consecutive samples, so the loop kept pace. |
| PJ3 | In the baseline window all samples of JOY0DAT and JOY1DAT are identical: nothing else moves the counters. |
| PJ4 | The harness port used is the one documented as the manual's left port (P1, P2) or right port (P3, P4). See section 3.4. |

**Contrasting case for each precondition:**

| Id | Change | Result |
|----|--------|--------|
| PJ1 | Drop one scheduled change from the harness while the grader keeps its plan. | PJ1 reports the missing change. |
| PJ2 | Shorten one hold to 2 scan lines. | The window has fewer than 3 graded samples, so PJ2 is not met. |
| PJ3 | Inject mouse counts on the port under test during the baseline. | The baseline samples differ, so PJ3 is not met. |
| PJ4 | Grade P1 while the schedule runs on the other port. | The anchor check of section 3.4 flags the mapping. |

### 4.6 Contrasting cases

In each case below the program is unchanged and all preconditions hold.

1. **Other port.** Apply the same schedule to the corresponding pins of the
   other port. The byte under test stays at its baseline in every window, and
   the schedule shows up in the other register instead.
2. **Other axis.** Apply the schedule to the other pin pair of the same port
   (pins 1/3 instead of 2/4, or the reverse). The byte under test stays at its
   baseline.
3. **Direction.** Swap the forward and backward phases. The byte runs through
   c0 - n instead of c0 + n, so the direction of counting is graded.

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
- Each step lasts at least 24 scan lines.
- The four propositions may share one run, one phase per pin.

### 5.3 Observation

- **Data:** the stored POTGOR samples.
- **Graded samples:** those stored at least 6 scan lines after the latest change
  (the POTGO marker or the harness button change) and before the next change.
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
| C4 | 0 (*) | p. 240: "If you set the output enable for any pin to a 1, the Amiga disconnects the potentiometer control circuitry from the port, and configures the pin for output. The state of the data bit controls the logic level on the output pin." p. 282 names the bit "I/O data Paula pin nn". |
| C5 | 0 | output low (p. 240) and button to ground (p. 241) |
| C6 | 0 | p. 240: "To use the entire register as an input, sensing the current state of the pot pins, write all 0s to POTGO. Thereafter you can read the current state by using read-only address POTINP."; the pressed button grounds the pin (p. 241) |
| C7 | **no expected value**; record only | With the pin as input nothing on the pages drives it. p. 241 says the pull-up exists only when the Amiga drives the line. |

(*) C4 assumes that "released" does not drive the line (section 3.5). If the
harness drives the line high when released, C4 has no expected value.

**The other bits.**

- In every step, the other three DAT bits read 1: their pins stay in the C1
  configuration with their buttons released (p. 241).
- Not graded: POTGOR bits 15, 13, 11 and 9 (no page says what the OUT bits read
  back as), bits 7-1 (the pages disagree, section 8.5) and bit 0.

**Quantifiers.** p. 240 says "any pin", so all four pins are graded (P5-P8).
"Every" graded sample is required. C1 and C3 both grade "a 1 if it is not"
after a press.

### 5.5 Preconditions

| Id | Precondition |
|----|--------------|
| PP1 | The harness input log shows the button changes on the pin under test only. Each change lies in its intended step, at least 6 lines from that step's markers. Every other controller input and the mouse are idle. |
| PP2 | The POTGO values stored with the markers are the planned ones, and bit 0 is 0 in all of them. |
| PP3 | Every graded window holds at least 3 samples. |
| PP4 | Port mapping as in section 3.4. |

**Contrasting case for each precondition:**

| Id | Change | Result |
|----|--------|--------|
| PP1 | Omit the press of C2. | PP1 reports the scheduled change as missing instead of grading C2. |
| PP2 | Write POTGO with bit 0 = 1 in C1. | PP2 is not met. |
| PP3 | Shorten one step to 6 lines. | PP3 is not met. |
| PP4 | As PJ4. | The anchor check of section 3.4 flags the mapping. |

### 5.6 Contrasting cases

In each case the program still finishes and the preconditions hold.

1. **Output.** C4 differs from C1 only in DAT bit d, with the button released.
   The page predicts 0 in C4 and 1 in C1. An implementation whose pin only
   follows the external level reads 1 in C4 and fails.
2. **Input path.** C2 differs from C1 only in the button. The page predicts 0
   in C2 and 1 in C1. A register that merely echoed POTGO reads 1 in C2 and
   fails.
3. **Binding.** Repeat C2 with the press on the corresponding pin of the other
   port, or on the other pot pin of the same port. Bit d stays 1, and the
   pressed pin's own DAT bit reads 0.

### 5.7 Reference frames

- **Ports:** section 3.4.
- **Steps:** step windows are bounded by the program's POTGO markers and the
  harness's button-change times, on the Chip-memory port's time base.
- **Anchor check:** each marker's POTGO value equals the planned value for its
  step (PP2).

---

## 6. P9: a drive's motor follows its select-clocked flip-flop

### 6.1 Proposition

**P9** (opus:AHJ-085, sol:H-060; p. 370: "The output of this flip-flop controls
the disk drive motor. Thus, the system can control all four motors using only
one signal on the cable (MTRXD-)."): a drive's motor takes the level of the
motor line only at the moment the drive's select line goes from high to low.
It keeps that state until the next such transition, whatever the motor line
does in between. P9 checks this on drive 0 through its activity light.

**Context on the same and neighbouring pages.**

- p. 369: "All external drives must have logic equivalent to a D flip-flop,
  whose D input is the MTRXD- signal, and whose clock input is activated by the
  off-to-on (high-to-low) transition of its SELxB- signal."
- p. 334: "Motor on data, clocked into drive's motor-on flip-flop by the active
  transition of SELxB*."
- p. 244 extends the latch to every drive, drive 0 included: "Each drive will
  latch the motor signal at the time its select signal turns on. The disk drive
  motor will stay in this state until the next time select turns on. DSKMOTOR*
  also controls the activity light on the front of the disk drive." It also
  says: "The drive will "remember" the state of its motor when it is not
  selected."
- p. 245: "PB3 DSKSEL0* Select drive 0 (internal drive) (active low)."

**Scope.**

- Appendix H states the flip-flop for external drives. The test sees only the
  harness's drive light. The check therefore uses drive 0, which p. 244 covers.
- "all four motors" (p. 370) cannot be graded beyond drive 0.

### 6.2 Stimulus

**Setup.**

1. Write PRB = $FF.
2. Then write DDRB = $FF.

The order is prescribed by the pages. p. 354 says that after reset "The port
pins are set as inputs and port registers to zero (although a read of the ports
will return all highs because of passive pull-ups)." Writing DDRB first would
drive all eight lines low at once: every drive selected while the motor line
changes. p. 244 forbids that: "All software that selects drives must set up the
motor signal before selecting any drives."

**Sequence.** The sequence follows p. 369: "When the system wants to turn on a
disk drive motor, it first deselects the drive (if selected), pulls MTRXD- low,
and selects the drive. To turn the motor off, the system deselects the drive,
pulls MTRXD- high, and selects the drive."

- Each step is one PRB write followed by a marker.
- Each step is held for H = 64 scan lines (at least).
- p. 369 requires "The system will always set MTRXD- at least 1.4 microseconds
  before it selects the drive, and will not change MTRXD- for at least
  1.4 microseconds after selecting the drive." H meets this many times over.
- Bits 2-0 stay 1 throughout: no side change, DIR high, no step pulse.
- The sequence runs twice (cycle 1 and cycle 2).
- A done marker follows.

| Step | PRB | Change | Latched reading: light = motor = flip-flop | Line reading: light = DSKMOTOR* |
|------|-----|--------|------------------|------------------|
| S0 | $FF | motor line high, no drive selected | off | off |
| S1 | $7F | motor line low, no drive selected | off | on |
| S2 | $77 | select drive 0 (motor line low) | on | on |
| S3 | $7F | deselect | on | on |
| S4 | $FF | motor line high, no drive selected | on | off |
| S5 | $F7 | select drive 0 (motor line high) | off | off |
| S6 | $FF | deselect | off | off |
| S7 | $7F | motor line low, no drive selected | off | on |
| S8 | $FF | motor line high | off | off |

### 6.3 Observation

- **Signal:** the harness drive light.
- **Window of step k:** W_k = [m_k + H/2, m_(k+1)), where m_k is the time of the
  step's marker at the Chip-memory port.
- **Graded value:** the light must be constant in W_k. That constant level is
  the step's observed value.
- Light changes outside the windows are reported, not graded.

### 6.4 Expected values and verdict

**The two readings.** p. 244 says "DSKMOTOR* also controls the activity light".
It does not say whether the light is controlled through the latch. This gives
two readings:

- **latched:** the light shows the drive's motor, which the flip-flop drives;
- **line:** the light follows DSKMOTOR* directly.

Only under the latched reading does the light show the flip-flop.

**Grading.**

| Steps | Rule |
|-------|------|
| S0, S2, S3, S5, S6, S8 | Both readings agree (table above). Any other value in either cycle is a **fail**. |
| S1, S4, S7 (discriminating) | Decided by the verdict table below. |

**Verdict for S1, S4 and S7:**

| Observed in both cycles | Verdict |
|-------------------------|---------|
| off, on, off (latched) | **pass** |
| on, off, on (line) | **no verdict**: the light follows the motor line, and p. 244 does not settle whether it should, so the flip-flop is not observable |
| any other combination | **fail**: it matches neither reading |

**Quantifier.** "until the next time select turns on" (p. 244) is graded in
every hold step of both cycles.

### 6.5 Preconditions

| Id | Precondition |
|----|--------------|
| P9-a | The light is off in W_S0 of cycle 1. p. 244: "All drive motors turn off after system reset." |
| P9-b | The light is on in W_S2 of cycle 1. This shows the harness models drive 0 and its light without a disk. |
| P9-c | All markers are present, in order, spaced at least H lines apart. |

If a precondition fails, the verdict is "not checkable" and the report names
the precondition.

**Contrasting case for each precondition:**

| Id | Change | Result |
|----|--------|--------|
| P9-a | Precede S0 with S1-S3 and write $7F instead of $FF in S0. | Both readings predict on in S0, so P9-a is not met. |
| P9-b | Write $FF in S1 and $F7 in S2 (select drive 0 with the motor line high). | Both readings predict off, so P9-b is not met. |
| P9-c | Omit one marker. | P9-c is not met. |

### 6.6 Contrasting case

In variant P9-V, S5 writes $FF instead of $F7: the motor-off select is
omitted. Everything else is unchanged, P9-a and P9-b still hold, and the
program still finishes.

- The latched reading then predicts the light on from S5 of cycle 1 onward,
  where the main run predicts off in S5, S6, S7 and S8.
- Grading the variant's observations against the main run's table must
  therefore fail for an implementation that passes the main run. This shows
  that the check can go red.
- Graded against its own table, the variant passes under the latched reading.
- After the graded steps, the variant ends with a motor-off select (S3, S4, S5
  as in the main run) so the program leaves the drive off.

The in-sequence pairs S1/S2 and S4/S5 are further contrasts: in each pair the
two steps differ only by the select edge.

### 6.7 Reference frame

- **Steps:** marker times at the Chip-memory port. The light is observed on the
  same time base.
- **Anchor check:** P9-c. The order and spacing of the markers show that the
  program ran the sequence as planned.

---

## 7. P10: a RESET instruction turns a latched-on drive motor off

### 7.1 Proposition

**P10** (opus:AHJ-090, sol:H-064; p. 370): executing the RESET instruction
activates the system reset signal, of which DRESB- is the buffered copy on the
disk connector. After it, drive 0's motor is off, although its flip-flop held
the motor on and nothing selected the drive.

The page reads: "This signal is a buffered version of the system reset signal.
Three things can make it go active (low):", and then "System CPU executes a
RESET instruction (DRESB- will go low for approximately 17 microseconds);".

**Consequences on the pages.**

- p. 244: "All drive motors turn off after system reset."
- p. 370: "External disk drives should respond to DRESB- by shutting off their
  motor flip-flops and write protecting themselves."
- p. 334: "Amiga system reset. Drives should reset their motor-on flip-flops
  and set their write-protect flip-flops."

**Not graded.**

- The duration of the pulse (section 1.5).
- "All drive motors": only drive 0 is visible.
- Write protection: it needs a disk.

### 7.2 Stimulus

1. Set up CIA-B as in P9.
2. Run S0 to S4 of P9 once, each held H lines, with markers. After S4, drive
   0's flip-flop holds the motor on while the motor line is high and no drive is
   selected.
3. Load A0 with the address of a spin loop L (a branch to itself) in Chip
   memory.
4. Write marker "R". Its time at the Chip-memory port is **t_R**.
5. Execute RESET, followed by JMP (A0).
   - RESET is a privileged 68000 instruction, so the program runs in supervisor
     mode. This is a CPU fact, not a page statement.
   - p. 223: "The 68000 RESET instruction works much like external reset or
     power on. All memory and AUTOCONFIG(TM) cards disappear, and the ROM image
     appears at location $00000000. The difference is that the CPU continues
     execution with the next instruction. Since RAM may not be available,
     special care is needed to write reboot code that will reliably reboot all
     Amiga models."
   - The pages give no rule for this "special care". The builder places RESET
     and JMP (A0) so that the JMP has already been fetched when RESET executes.
   - The builder places L at an address the system still serves after RESET, if
     one exists. p. 223: "On some Amiga models, portions of the RAM still
     respond. On other models, no RAM responds."
   - Precondition P10-c checks the outcome.
6. The harness ends the run at E, at least 64 scan lines after t_R.

### 7.3 Observation

- The harness drive light over [t_R, E].
- The CPU's accesses at the Chip-memory port over [t_R, E].

### 7.4 Expected value

- The light is off at E.
- Once it goes off after t_R, it stays off until E.

The pages give no delay between the RESET instruction and the motor turning
off. The test records t_off - t_R and does not grade it. E is a test choice, not
a page value, and is the same in the contrast run.

### 7.5 Preconditions

| Id | Precondition |
|----|--------------|
| P10-a | The light is off in W_S0, as P9-a. |
| P10-b | The light is on continuously from the second half of S2 up to t_R. That includes S4, where the flip-flop holds "on" while the motor line is high. This establishes that the light shows the flip-flop (the latched reading of section 6.4). Under the line reading the light is already off in S4, and its state after RESET would show nothing about the motor. |
| P10-c | After t_R the CPU makes no write at the Chip-memory port. Its first read of L's address comes within 2 scan lines of t_R. From then until E, every CPU access is a read of L's words, with at least one in every scan line. This shows that only the test's code ran after RESET. Other code could write CIA-B, and selecting drive 0 with the motor line high would also turn the light off. If L's reads never appear (for example because the overlay of p. 223 hides that memory), the check is "not checkable" at that address. |

**Contrasting case for each precondition:**

| Id | Change | Result |
|----|--------|--------|
| P10-a | As P9-a. | P10-a is not met. |
| P10-b | Write $FF in S1 and $F7 in S2. | The light is off at S4, so P10-b is not met. |
| P10-c | Point A0 at a second copy L' of the loop, at another address, while the grader expects L. | P10-c is not met. |

### 7.6 Contrasting case

A NOP ($4E71) replaces RESET; everything else is unchanged.

- Nothing turns the flip-flop off, so the page predicts the light stays on from
  S4 until E.
- The CPU reaches L and loops there. P10-c holds, and the program ends by
  reaching L.

### 7.7 Reference frame

- **t_R** is the time of marker "R" at the Chip-memory port. E is counted from
  it.
- **Anchor check:** a CPU read of the address where the program placed the
  RESET opcode word ($4E70) appears adjacent to the marker-"R" write among the
  CPU accesses at the Chip-memory port (just before or just after it).
- **Cores:** P10 runs on both cores in 68000 mode.

---

## 8. Where the pages disagree or leave the point open

### 8.1 Direction of LP*

The pages give three different directions:

- p. 379 (Original Agnus): "37 LP* Light pen input I".
- p. 381 (Fat Agnus): "78 LP* Light pen O".
- Chapter 8 (pp. 238-239) describes the pen trigger on the A500, A2000 and
  A3000 as a signal taken in, which freezes VPOSR.

Consequence: no proposition (sections 1.2 and 1.3).

### 8.2 Device I.D.

p. 371 and p. 335 differ on three points (quoted in section 1.1):

- whether the RDY- samples are inverted;
- what $0000 0000 means;
- whether an extra select pulse (steps 5 and 6) precedes the read loop.

Both pages print the label as "3.25". p. 241 says "One 3.5" drive is installed
in the basic unit." The label is left as printed.

### 8.3 The drive activity light

p. 244: "DSKMOTOR* also controls the activity light on the front of the disk
drive." The page does not say whether the light follows the latched motor state
or the line itself. P9's verdict table and P10's precondition P10-b handle
this.

### 8.4 Port numbering and the fire-button bits

The pages disagree on which CIA-A bit serves which port's fire button:

- p. 232: "Port 1 uses bit 6 and port 2 uses bit 7."
- App. F p. 351 and App. E p. 337: "PA7..game port 1, pin 6 (fire button*)"
  and "PA6..game port 0, pin 6 (fire button*)".
- App. E p. 329: "PRA0 bit 7 = Fire* left controller port / PRA0 bit 6 = Fire*
  right controller port".

Consequence: no test here uses the fire bits (section 3.4).

### 8.5 POTGOR bits 7-1

- p. 240: "chip revision identification number".
- p. 282: "Reserved for chip ID code (presently 0)".

Consequence: not graded.

### 8.6 CIA-B port B naming

p. 244 prints "CIAAPRB ($BFD100)". p. 241 lists "CIABPRB ($BFD100)", and p. 340
places prb at BFD100 in the CIAB map. The address is used.
