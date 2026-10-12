# Specification: controller-port package (chapter 8 and Appendix A)

This specification was written from the package's claims
(`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference
Manual*, 3rd edition, by an author who never saw the implementation or any
test. Page numbers are printed page numbers. Where the text layer and the
printed page differ, the printed page (rendered image) was read and is what is
quoted.

The package has 22 claims. They give four propositions; 2 claims give none.
10 of the 22 are asserted (2 in P1, 4 in P2, 4 in P3). 12 are declined: the 2
Denise-sampling claims, and the 10 of P4, which state which controller button is
wired to which connector pin, which no port of the design shows. P4 keeps its
p.281 pin-decode assertions without those ids.

| proposition | claims | page |
|---|---|---|
| P1: POTGO bit 0 (START) dumps the capacitors on the pot lines that are inputs | `opus:8-dio.c21`, `sol:8-dio.c19` | 240 |
| P2: JOY0DAT serves the left connector, JOY1DAT the right one | `opus:8-appa-joy.c11`, `opus:8-appa-joy.c12`, `sol:8-appa-joy.c13`, `sol:8-appa-joy.c14` | 281 |
| P3: pins 1 and 3 clock the Y counter, pins 2 and 4 the X counter | `opus:8-appa-joy.c17`, `opus:8-appa-joy.c18`, `sol:8-appa-joy.c19`, `sol:8-appa-joy.c20` | 281 |
| P4: the Table 8-1 buttons on pins 3, 4 and 1 read as closed in their connector's JOYxDAT | `opus:8-tbl8-1.c3`, `opus:8-tbl8-1.c8`, `opus:8-tbl8-1.c9`, `opus:8-tbl8-1.c12`, `opus:8-tbl8-1.c13`, `sol:8-pin.c3`, `sol:8-pin.c9`, `sol:8-pin.c10`, `sol:8-pin.c13`, `sol:8-pin.c14` (declined, see P4 section 1) | 228 |

---

## Claims that give no proposition

### Sampling into Denise (p.281)

- `opus:8-appa-joy.c51`, `sol:8-appa-joy.c25`: "The pins (and their functions)
  are sampled (multiplexed) into the DENISE chip during the clock times shown in
  the table."

Reason:

- The sampling and the multiplexing happen inside the chip. Which chip samples
  a pin, through which of its pins (38, 9, 39, 8), and at which clock (CCK or
  CCK*) never appears on any output a test sees. It is also not something a
  program can read back.
- A test drives steady digital levels on the connector pins. It cannot place a
  level change at a chosen phase of CCK, and the page gives no expected value
  for a change shorter than a sampling period.
- The page itself says, in the next sentence: "This table is for reference only
  and should not be needed by the programmer."

Everything the table leads to that a program can see is covered by P2 to P4:
which pins reach which counter of which register, and what the low counter bits
show.

Two parts of other claims also cannot be checked. They are named inside the
propositions that use the rest of those claims:

- which pin a real controller wires its button to (P4);
- a mouse's quadrature pulse trains (P3).

---

## Terms and setup used by every proposition

### Connectors and levels

- **Connector A** is the connector whose fire button reads at CIAAPRA bit 6,
  as asserted by the anchor (section 7 of P2): "0=left controller pair"
  (p.281), "port 1" in chapter 8's numbering (p.230), the "PORT 1 (mouse)"
  connector of Figure 8-3 (p.233). **Connector B** is the other one.
- A **level** is the electrical level the harness puts on a connector pin:
  **1 = open** (switch released) and **0 = shorted to ground** (switch closed).
  - p.232: "Digital joystick switches are of the normally open type. When the
    switches are pressed, the input line is shorted to ground."
  - p.281: "the joystick functions are all 'active low' at the connector pins."
- **Pins** (Table 8-1, p.228): 1 forward/up, 2 back/down, 3 left, 4 right,
  6 fire, 5 and 9 the pot lines. The harness's "up, down, left, right" are pins
  1, 2, 3, 4 in that order.
- **Released** means level 1 on every one of these pins, with no mouse movement
  injected and no key sent.

### Registers

From Appendix B, p.289 to p.291, and Appendix A, p.281 and p.282, except
CIAAPRA and CIAADDRA, which come from p.229 and p.241.

| name | address | access | use here |
|---|---|---|---|
| JOY0DAT | $DFF00A | R | counters of connector A (P2) |
| JOY1DAT | $DFF00C | R | counters of connector B (P2) |
| POTGO | $DFF034 | W | pot port data write and start |
| POTGOR | $DFF016 | R | pot port data read (the print also calls it POTINP) |
| VPOSR / VHPOSR | $DFF004 / $DFF006 | R | beam position, read as one long word at VPOSR (p.238) |
| BPLCON0 | $DFF100 | W | bit 3 LPEN must be 0 |
| DMACON | $DFF096 | W | setup only |
| INTENA | $DFF09A | W | setup only |
| CIAAPRA | $BFE001 | R | bit 6: fire button of port 1; bit 7: of port 2 (p.232) |
| CIAADDRA | $BFE201 | R/W | direction mask for CIAAPRA (p.241) |

JOY0DAT and JOY1DAT have the same layout (p.281). Bits 15-8 are the Y (vertical)
counter, Y7 to Y0. Bits 7-0 are the X (horizontal) counter, X7 to X0.

### Setup, at the start of every program

- **S1.** INTENA ← $7FFF and DMACON ← $7FFF. With bit 15 SET/CLR = 0, this
  clears every enable (p.278, p.280). Nothing in this package needs DMA or
  interrupts.
- **S2.** BPLCON0 ← $0000. LPEN = 0, so VPOSR counts and is not latched. p.239:
  "when the light pen latch is enabled, the VPOSR register may be latched at any
  time, and cannot be relied on as a counter".
- **Beam position.** V = (bit 0 of VPOSR) × 256 + (bits 15-8 of VHPOSR), from
  p.238: VPOSR bit 0 is V8, VHPOSR bits 15-8 are V7-V0.

### Configurations

- Every program runs on each of the two CPU cores.
- It runs in PAL and NTSC, and with ECS and OCS selected, with 2 MB of Chip
  memory.
- The pages state no dependence on any of these, so each run is graded on its
  own and in the same way.

---

## Proposition 1: POTGO bit 0 (START) dumps the capacitors on the pot lines that are inputs

### 1. Proposition

Writing POTGO ($DFF034) with bit 0 (START) set dumps (discharges) the capacitor
on each of the four pot lines that is configured as an input. So while the read
circuitry is held in its reset state, POTGOR ($DFF016) reads 0 in the data bit
of every such line.

- Claims: `opus:8-dio.c21` and `sol:8-dio.c19`, printed page **240**,
  Table 8-4: "00 START Start pots (dump capacitors, start counters)".
  Appendix A (p.282) prints the same row.
- The claims cover the dump only. "start counters", and the POT0DAT/POT1DAT
  reset of p.236 ("All counts are reset to zero when POTGO is written with bit
  zero high"), are not among this package's claims and are not graded.

Why POTGOR shows the dump:

- The capacitors sit on the pot lines. p.240: "There are large capacitors on
  these lines". Figure 8-6 (p.237) draws a 47 nF capacitor from the pot line to
  ground, between connector pin 9 and the chip.
- POTGOR reads the lines. p.240: "To use the entire register as an input,
  sensing the current state of the pot pins, write all 0s to POTGO. Thereafter
  you can read the current state by using read-only address POTGOR."
- The capacitor stays discharged through the reset interval. p.235: "The read
  circuitry stays in a reset state for the first seven or eight horizontal
  video scan lines. Following the reset interval, the circuit allows a charge to
  begin building up on a timing capacitor". Figure 8-5 starts both charging
  curves at zero, "eight horizontal lines after POTGO is written".

A dumped capacitor is at ground, and so is the line it sits on.

The page limits the proposition to input lines. p.240: "If you set the output
enable for any pin to a 1, the Amiga disconnects the potentiometer control
circuitry from the port, and configures the pin for output. The state of the
data bit controls the logic level on the output pin." Also p.240: "Note that
bits set as inputs will be connected to the proportional counters (See the
description of the START bit in POTGO)."

Quantifiers:

- "capacitors" is plural, and p.240 says the register "shares the same four
  pins as the four pot inputs". Every one of the four lines is graded, and all
  four must pass.
- The circuitry "stays" in reset. Every graded read inside the window of
  section 7 must show the dump.

### 2. Stimulus

POTGO/POTGOR bits (p.240 Table 8-4; p.282):

| bit | name | meaning |
|---|---|---|
| 15 | OUTRY | output enable for bit 14 (1=output) |
| 14 | DATRY | data for port 2, pin 9 (Paula pin 36) |
| 13 | OUTRX | output enable for bit 12 |
| 12 | DATRX | data for port 2, pin 5 (Paula pin 35) |
| 11 | OUTLY | output enable for bit 10 |
| 10 | DATLY | data for port 1, pin 9 (Paula pin 33) |
| 09 | OUTLX | output enable for bit 8 |
| 08 | DATLX | data for port 1, pin 5 (Paula pin 32) |
| 07-01 | | chip ID; always written as 0 |
| 00 | START | start pots (dump capacitors, start counters) |

The names LX, LY, RX and RY below refer only to these bit positions. P1 does not
depend on which connector a bit belongs to.

**Harness:** all inputs released throughout. In particular, the four pot lines
(pins 5 and 9 of both connectors) are held at level 1, the level a released
button gives. The contrasting case K5 is the one exception.

**Setup:** S1, S2.

**The two steps used for every POTGO write:**

- **W (write step), line 4:** wait for V == 4. Read V (Vw0), write POTGO, read V
  (Vw1). Then store marker M1 to Chip memory, together with Vw0 and Vw1. In step
  Fc, POTGOR is also read once between Vw0 and the POTGO write (Rpre).
- **R (read step), line 10:** wait for V == 10. Repeat the following for as
  long as V is still 10: read V (Vr0), store marker M2, read POTGOR, read V
  (Vr1), store the three values. Every record with Vr0 == Vr1 == 10 counts.
  There must be at least one.

The write happens during vertical blanking at the top of the screen, as the page
prescribes. p.234: "During vertical blanking, you write a value into an address
called POTGO. For a standard X-Y joystick, this value is hex 0001." p.235: "You
normally issue POTGO at the beginning of a video screen". Lines 4 and 10 are
both inside internal vertical blanking, which ends at vertical position 20
(p.239).

**Sequence.** Each step below occupies one frame and uses the next time V
reaches 4.

Phase P (checks that POTGOR reads the pins):

- F1: W with POTGO ← $FF00: all four lines outputs, driven high ("set both
  OUT... and DAT... to 1", p.241). Then R, giving Rhigh.
- F2: W with POTGO ← $AA00: all four lines outputs, driven low. Then R, giving
  Rlow.
- F3: W with POTGO ← $FF00. Then R, giving Rhigh2.

Phase D (the dump), run for each configuration C in this order:

| C | value | output (driven high) | inputs |
|---|---|---|---|
| C0 | $0000 | none | LX, LY, RX, RY |
| C1 | $0300 | LX | LY, RX, RY |
| C2 | $0C00 | LY | LX, RX, RY |
| C3 | $3000 | RX | LX, LY, RY |
| C4 | $C000 | RY | LX, LY, RX |

Each configuration takes three frames:

- Fa: W with POTGO ← $FF00. Then R, giving Ra.
- Fb: W with POTGO ← C. Then R, giving Rb.
- Fc: W with Rpre, and POTGO ← C | $0001. Then R, giving Rpost. **Rpost is the
  graded read.**

The START write differs from the configuration already in place only in bit 0.

### 3. Observation

The test reads what the program stored in Chip memory (the POTGOR words and the
V words), and the Chip-memory-port timestamps of the CPU writes of M1 and M2.

- Graded: the Rpost records of every Fc, in line L0+6, where L0 is the line of
  the START write (L0 = 4, so line 10).
- Bits: only 14, 12, 10 and 8 are graded (mask $5500).
- Not graded: reads in lines L0 to L0+5, reads after line L0+6, and all other
  POTGOR bits.

### 4. Expected value

For every counted Rpost record:

| C | expected `Rpost & $5500` | lines at 0 (dumped) | line at 1 |
|---|---|---|---|
| C0 | $0000 | LX, LY, RX, RY | none |
| C1 | $0100 | LY, RX, RY | LX (output) |
| C2 | $0400 | LX, RX, RY | LY (output) |
| C3 | $1000 | LX, LY, RY | RX (output) |
| C4 | $4000 | LX, LY, RX | RY (output) |

- **The proposition** is the "0" column. Each line must read 0 in every counted
  record of every configuration in which it is an input.
- **Where the claim ids sit.** The claims say every one of the four lines, and
  each of C0 to C4 shows a different set of them as inputs. One aggregate line,
  `P1-START`, is the conjunction of the "0" columns of C0 to C4 (and of PC1 to PC4); it carries
  `opus:8-dio.c21` and `sol:8-dio.c19`, once per configuration of Chip set and
  video standard, each graded on its own. The five `C0-Fc` to `C4-Fc` lines
  grade one configuration each and carry no id.
- **The "1" column** comes from the output-enable sentence of p.240. A failure
  there is reported against that sentence, not against the START claims.

Sources: p.240 Table 8-4 (and p.282) "00 START Start pots (dump capacitors,
start counters)"; p.235 as quoted in section 1; p.240 "Thereafter you can read
the current state by using read-only address POTGOR."

### 5. Preconditions

If one fails, the result is "not established", never "pass".

- **PC1: line position.**
  - For every W: Vw0 == Vw1 == 4, with V8 = 0.
  - For every counted R record: Vr0 == Vr1 == 10.
  - LPEN = 0 (S2).

  If a W straddles a line change, the program repeats that frame's step in the
  next frame.
- **PC2: POTGOR follows the level on the pins, and the harness's level 1 gives
  way to the chip's own drive.**
  - Rhigh, Rhigh2 and every Ra: `& $5500` == $5500.
  - Rlow: `& $5500` == $0000, although the harness holds the lines at 1.

  Basis: p.240 "The state of the data bit controls the logic level on the output
  pin."; p.241 "Reading POTGOR will produce a 0 if the button is pressed, a 1 if
  it is not."
- **PC3: each input line reads 1 just before the dump.**
  - Rb & $5500 == $5500 and Rpre & $5500 == $5500 in every configuration.
  - The harness's drive log shows the pot lines at level 1 from Fa to the end of
    Fc.

  If a line reads 0 here, a 0 in Rpost would not show a dump. That line is then
  "not established" in that configuration, and so is the proposition's "all
  four".
- **PC4: the graded read comes after the only settling time the pages give.**
  - For every R record used above: t(M2) − t(M1) ≥ 300 µs, converted from
    system clock cycles with the harness clock frequency.

  Basis: p.240 "There are large capacitors on these lines, and it can take up to
  300 microseconds for the line to change state." The pages give no settling
  time for the dump itself.

### 6. Contrasting cases

- **K1 (bit 0).** Every Fc writes C instead of C | $0001.
  - Predicted: no dump; Rpost & $5500 == $5500 in every configuration.
  - The "0" checks go red for every input line. PC1 to PC4 are unchanged.
- **K2 (per line; already in the sequence).** In C1 to C4, one line is an
  output driven high when START is written, and must read 1 while the other
  three read 0. This catches a POTGOR that reads 0 in every data bit after any
  START.
- **K3 (PC1).** The R step of every Fc moves to line 12. PC1 must fail, and the
  result must be "not established".
- **K4 (PC2).** F1 writes $AA00 instead of $FF00. Rhigh reads $0000, and PC2
  must fail.
- **K5 (PC3).** The harness grounds every pot line it can reach at these ports
  (LX, LY, RY; RX has no port at this boundary) from each configuration's Fb W
  step through the end of its Fc R step.
  - Rb and Rpre read $1000 (only bit 12, RX, stays 1: it cannot be reached), the
    drive log shows the grounded ports at level 0, and PC3 must fail through both
    clauses.
  - The result must be "not established" for every configuration, even though
    three of the four input lines of Rpost already read 0.
- **K6.** The R step of every Fc moves to line 5.
  - A record only counts when Vr0=Vr1=10 (the grader's own fixed target, section
    3): moving the R step to line 5 means no record of any Fc ever counts, so
    PC1 ("every R step has at least one qualifying record") fails, not PC4.
- **K7 (PC4).** Every W step stores its marker M1 after waiting for line 9
  instead of straight after the POTGO write and the V read at line 4; the
  POTGO write, the V reads and every R step are unchanged.
  - PC1 to PC3 hold, since PC1 reads the V words and not the timestamps. The
    time from M1 to M2 is shorter than a line, so PC4 must fail, and the
    result must be "not established".
- **Green control for C0-Fc to C4-Fc and P1-START.** These are confirmed reds,
  not stimulus defects, and no stimulus at the ports turns them green with
  PC1 to PC4 holding (grounding a line after START is a different cause of a 0,
  and breaks PC3's drive-log clause). The green evidence is a run of the
  unchanged package against a private copy of the RTL outside the repository, in
  which the pot lines that are inputs read 0 in POTGOR for eight lines after a
  POTGO write with START (section "The private copy of the RTL"). There all 24
  lines print `PASS` with PC1 to PC4 green; K1 (START withheld) turns them red
  again in that copy.
- **OUTLOW (the "1" column, C1-Fc-OUT to C4-Fc-OUT).** In C1 to C4, Fc's write
  sets the output-enable bit but drives its data bit to 0 instead of 1 ($0201,
  $0801, $2001, $8001), with START unchanged.
  - Predicted: the output-driven line reads 0, not 1 (p.240 "The state of the
    data bit controls the logic level on the output pin"), so the "1" check
    must fail for that configuration's own line, through its own clause.
  - PC1 to PC4 are unaffected.

### 7. Reference frame

- **Line numbering:** V (p.238), non-interlaced. L0 = 4, the line of the START
  write.
- **The reset interval as drawn:** "the first seven or eight horizontal video
  scan lines" (p.235); charging "starts eight horizontal lines after POTGO is
  written" (Figure 8-5).
- **What is left open:** whether the partly elapsed line L0 counts, and whether
  the reset starts at the write or at the next line.
- **The window every reading agrees on:** lines L0+1 to L0+6. If L0 counts, the
  reset ends at the start of L0+7; otherwise at the start of L0+8 or later.
  Graded reads use L0+6.
- **How the test checks the anchor:** PC1's V brackets; lines 4 and 10 are in
  the same frame with V8 = 0 and below 20; the M1 and M2 times increase from W
  to R.

---

## The joystick program (shared by P2, P3 and P4)

### Setup

- S1, S2.
- POTGO ← $FF00: all four pot lines set to output, driven high. p.241's
  button-reading recipe: "set the proper pin to output, and drive the line
  high (set both OUT... and DAT... to 1). Reading POTGOR will produce a 0 if
  the button is pressed, a 1 if it is not." Nothing in this program's stimulus
  presses a button on a pot line, so Q2 grades the released ("1") side of that
  recipe.
- Read CIAADDRA, clear bits 6 and 7, and write it back, which makes both fire
  bits inputs. p.241: "CIAADDRA ($BFE201) contains a mask that corresponds
  one-to-one with the data read register, CIAAPRA ($BFE001). Setting a 1 in the
  direction position makes the corresponding bit an output." The program does
  not write JOYTEST.

### Step protocol, for step k = 0, 1, 2, ...

1. The program stores the step number k to a fixed Chip-memory word (marker
   S_k).
2. When the test sees that CPU write at the Chip-memory port, it sets the pin
   levels of step k at time t_apply(k). It holds them until it sees S_(k+1).
3. The program waits until V has changed twice, so that at least one full line
   passes. It then stores marker T_k.
4. The program reads JOY0DAT, JOY1DAT, CIAAPRA (byte) and POTGOR, and stores the
   four values (record k; the time of its first store is t(U_k)).

The pages give no time for a level on a pin to reach the counter register. They
say only that the pins are sampled "at CCK" and "at CCK*" (p.281). The wait of
at least one line is a margin, not a value from the page.

### Step table

Only the lines named in a step differ from released. Every step changes exactly
one pin from the step before it. For a pair (p, q), the values are the levels
of those two pins in that order.

- **N0, N1:** everything released (baseline).
- **Block A:** the steps below on connector A, with connector B released.
- **Block B:** the same steps on connector B, with connector A released.

Steps of one block, on connector P:

| steps | pins driven | levels, in order |
|---|---|---|
| F1(P), F0(P) | pin 6 | 0, then 1 |
| Y-up(P) | (pin 1, pin 3) | (0,1), (0,0), (1,0), (1,1) |
| Y-down(P) | (pin 1, pin 3) | (1,0), (0,0), (0,1), (1,1) |
| X-up(P) | (pin 2, pin 4) | (0,1), (0,0), (1,0), (1,1) |
| X-down(P) | (pin 2, pin 4) | (1,0), (0,0), (0,1), (1,1) |
| Buttons(P) | set of pins held at 0, among 1, 3, 4 | {1}, {1,3}, {3}, {3,4}, {1,3,4}, {1,4}, {4}, {} |

In the Y steps pins 2 and 4 are 1. In the X steps pins 1 and 3 are 1. In the
Buttons steps pin 2 is 1.

These are steady levels, each held for at least a line, as a joystick produces
when it moves between neutral, a direction and a diagonal. No pulse train is
driven, and the harness's mouse path (counts per axis) is not used.

### Preconditions shared by P2, P3 and P4

A record that fails one of these is not graded, and the proposition is "not
established" for the steps it covers.

- **Q1: the harness applied the levels before the read and held them.**
  - t_apply(k) < t(T_k), and the levels of step k are unchanged in the drive log
    until t(U_k).
  - The drive log is a trace of every port the test could drive (both
    joystick ports, the mouse buttons and the keyboard-mouse port), sampled
    after every system clock cycle the test itself advances, from the end of
    boot to the program's DONE. The trace holds what the ports carried, not what
    the test meant to apply.
  - The hold clause: the trace shows the levels the test applied at t_apply(k)
    in force at t_apply(k), and no port changes in the interval (t_apply(k),
    t(U_k)].
- **Q2: no other input was active.**
  - In every record outside the F steps, CIAAPRA bits 6 and 7 read 1 (p.232:
    "A logic state of 1 means 'switch open.' A logic state of 0 means 'switch
    closed.'"); in F0(A) bit 7 and in F0(B) bit 6 read 1 as well. The F1
    records belong to the anchor of P2 section 7, and the own-connector bit of
    an F0 record to Q4, so that no precondition repeats another's reading.
  - POTGOR bits 14, 12, 10 and 8 read 1: the pot lines are driven high as
    outputs (p.241's button recipe) and released.
  - The drive trace shows no mouse button, no mouse movement and no key at any
    cycle of the run: the four mouse and keyboard-mouse port fields are 0 in
    every sample.
- **Q3: the counters do not move without input.**
  - JOY0DAT and JOY1DAT are equal in N0 and N1.
  - Basis, p.281: "Each counter is clocked by signals from two controller pins."
- **Q4: the harness level convention matches the page.**
  - Level 0 on pin 6 of A reads 0 in CIAAPRA bit 6 in F1(A), and level 1 reads 1
    in F0(A).
  - This ties the harness's levels to the page's "shorted to ground" and "open".

Contrasting cases for these preconditions. Each keeps the program running.

- **Q1:** the harness applies the levels of Y-up(A) only after it sees U_k.
  Q1 must fail for those records, and they must not be graded.
- **Q1 (hold):** the harness changes one level of connector B after it sees
  T_k of one step and before U_k. The trace then shows a change inside
  (t_apply(k), t(U_k)], and Q1 must fail through its hold clause alone.
- **Q2:** the harness holds pin 9 of A at 0 during Y-up(A). POTGOR bit 10 (pin 9
  of the left connector is DATLY) then reads 0 in those records, and Q2 must fail
  for them.
- **Q2 (transient input):** the harness holds a mouse button for 64 cycles
  inside one step's wait and releases it again, and in a second case holds a
  keyboard-mouse data byte for 64 cycles and releases it. Both are back at 0
  long before that step's reads, so only the trace shows them, and Q2 must fail
  through its trace clause alone.
- **Q3:** the harness applies the first Y-up(A) level between N0 and N1. The two
  records then differ, and Q3 must fail.
- **Q4:** F0(A) is held at level 0 instead of 1. CIAAPRA bit 6 reads 0 in
  F0(A) instead of 1, and Q4 must fail; F1(A) is unchanged, so the anchor
  (which reads only F1 records) is unaffected, and no other precondition reads
  the own-connector bit of an F0 record.

---

## Proposition 2: JOY0DAT serves the left connector, JOY1DAT the right one

### 1. Proposition

Every change on the direction lines (pins 1 to 4) of the left connector (A)
shows in JOY0DAT and never in JOY1DAT. Every change on the right connector (B)
shows in JOY1DAT and never in JOY0DAT. Both counters of each register belong to
the same connector.

- Claims: `opus:8-appa-joy.c11`, `opus:8-appa-joy.c12`, `sol:8-appa-joy.c13`,
  `sol:8-appa-joy.c14`, printed page **281**: "These addresses each read a pair
  of 8-bit mouse counters. 0=left controller pair, 1=right controller pair (four
  counters total)."
- The same assignment appears on p.281 in the register rows ("JOY0DAT ...
  Joystick-mouse 0 data (left vertical, horizontal)", "JOY1DAT ...
  Joystick-mouse 1 data (right vertical, horizontal)"), on p.230 ("These
  registers contain counts for ports 1 and 2 respectively"), and in Figure 8-3
  (p.233): "PORT 1 (mouse)" wired to "JOY0DAT DFF00A", "JOY1DAT DFF00C is wired
  similarly" for "PORT 2".

### 2. Stimulus

The joystick program: block A, then block B.

### 3. Observation

- JOY0DAT and JOY1DAT in every record of the Y, X and Buttons steps of both
  blocks.
- The reference value for each register is its value in the last record before
  the block: N1 for block A, the last step of block A for block B.

### 4. Expected

- **In block A:** in every record, JOY1DAT equals its value at N1. In every
  record of a Y, X or Buttons step, JOY0DAT differs from its value in the record
  before. (The F steps change pin 6 only, which is not a counter pin; P3 grades
  them.)
- **In block B:** in every record, JOY0DAT equals its value at the last record
  of block A. In every record of a Y, X or Buttons step, JOY1DAT differs from its
  value in the record before.
- **"four counters total":**
  - The Y steps of A move bits 15-8 of JOY0DAT, and the X steps of A move bits
    7-0 of JOY0DAT.
  - The Y steps of B move bits 15-8 of JOY1DAT, and the X steps of B move bits
    7-0 of JOY1DAT.
  - None of the four counters ever moves on a step of the other connector.

The size of each change is graded by P3, not here.

Where the claim ids sit: the claims say "0=left controller pair, 1=right
controller pair (four counters total)". The four block lines and the
four-counters line each grade part of it and carry no id. One line, `P2-JOYDAT`,
is the conjunction of all five and carries `opus:8-appa-joy.c11`,
`opus:8-appa-joy.c12`, `sol:8-appa-joy.c13` and `sol:8-appa-joy.c14`.

Source sentences: p.281 as quoted in section 1; p.230 "These registers contain
counts for ports 1 and 2 respectively."

### 5. Preconditions

Q1 to Q4, and the anchor of section 7.

### 6. Contrasting case

- **Built in:** block B changes the lines of the other connector, and the page
  predicts that JOY0DAT stays unchanged there. The "JOY0DAT moves" check does
  not apply in block B; the "JOY0DAT unchanged" check does.
- **Variant:** the harness applies block A's steps to connector B and block B's
  to connector A, keeping the F steps where they are. The block-A expectation
  "JOY0DAT moves, JOY1DAT does not" must then go red. Q1 to Q4 and the anchor
  still hold. The swap is clean, so P3 and P4 read the swapped registers and stay
  green; the swap is reported under P2 only.

### 7. Reference frame: which connector is left

- **What the page draws:** the left connector is the one read as JOY0DAT
  (p.281), "port 1" (p.230), whose fire button is CIAAPRA bit 6 (p.232: "Port 1
  uses bit 6 and port 2 uses bit 7"; p.233: "The fire buttons for ports 0 and 1
  are connected to bits 6 and 7 of CIAAPRA").
- **Where the pages disagree:** they number the ports differently (1 and 2 on
  p.230 and p.232, 0 and 1 in p.233's text). They agree on the order: the
  connector of JOY0DAT is the one whose fire button is bit 6.
- **The anchor:**
  - Connector A is the connector whose fire button reads at CIAAPRA bit 6, as
    the fire-button check below asserts; nothing else in this package assumes
    the assignment ahead of that check.
  - The fire button checks this. In F1(A), CIAAPRA bit 6 must read 0 and bit 7
    must read 1. In F1(B), bit 7 must read 0 and bit 6 must read 1.
- **If the anchor fails** (A's fire shows at bit 7, or B's at bit 6), P2 is
  "not established". The disagreement is reported, and the test must not
  silently swap A and B.
- **Contrast for the anchor:** F1(A) additionally grounds pin 6 of connector B.
  Bit 7 then reads 0 in F1(A) instead of 1, and the anchor must fail; A's own
  F1 pattern (which Q4 reads) is unchanged, so Q4 is unaffected, and Q2 leaves
  the F1 records to the anchor.

---

## Proposition 3: pins 1 and 3 clock the Y counter, pins 2 and 4 the X counter

### 1. Proposition

On each connector:

- A change of pin 1 or pin 3 clocks the Y counter (bits 15-8 of the connector's
  JOYxDAT) by one count.
- A change of pin 2 or pin 4 clocks the X counter (bits 7-0) by one count.
- Neither pair of pins moves the other counter, and pin 6 moves neither.
- Bits 1 and 0 of each counter show the state of its two pins.

- Claims: `opus:8-appa-joy.c17`, `sol:8-appa-joy.c19` (pins 1 and 3 are the Y
  clock) and `opus:8-appa-joy.c18`, `sol:8-appa-joy.c20` (pins 2 and 4 are the X
  clock), printed page **281**: "Mouse counter usage: (pins 1,3=Yclock, pins
  2,4=Xclock)".
- The same page: "Each counter is clocked by signals from two controller pins.
  Bits 1 and 0 of each counter may be read to determine the state of these two
  clock pins." And: "After being sampled, these connector pin signals are used
  in quadrature to clock the mouse counters."

What stays uncheckable: a mouse's own pulse trains. The harness delivers mouse
movement as counts per axis, which bypasses the pins, so the mouse path says
nothing about which pin clocks which counter. The pins are driven as joystick
levels instead.

### 2. Stimulus

The joystick program. Every Y, X, Buttons and F step of both blocks.

### 3. Observation

- For each step k and its connector P: the register of P in records k−1 and k.
  That register is JOY0DAT for A and JOY1DAT for B, as P2 assigns.
- If P2 finds a clean swap (A's steps move only JOY1DAT and B's only JOY0DAT),
  P3 uses the swapped registers, and the swap is reported under P2 only.

### 4. Expected

**Quadrature state.** For the Y counter, V = level of pin 1 and VQ = level of
pin 3. For the X counter, H = level of pin 2 and HQ = level of pin 4. Figure 8-2
(p.230) gives the state:

| V VQ (or H HQ) | D1 D0 | s |
|---|---|---|
| 1 1 | 0 0 | 0 |
| 0 1 | 0 1 | 1 |
| 0 0 | 1 0 | 2 |
| 1 0 | 1 1 | 3 |

**For every step that changes a pin of a counter:**

- **Count.** That counter's byte in record k equals its byte in record k−1 plus
  d, modulo 256. d is +1 if s_new ≡ s_old + 1 (mod 4), and −1 if
  s_new ≡ s_old − 1 (mod 4). Every step changes one pin, so d is always ±1.
- **Low bits.** The counter's bits 1 and 0 (bits 9 and 8 for Y, 1 and 0 for X)
  equal D1 D0 of s_new.
- **The other counter** of the same register is unchanged.

**For every F step:** both counter bytes are unchanged.

**Quantifiers:**

- Both pins of each pair, in both directions (the up and down cycles), on all
  four counters.
- "Clock" means counting, not only showing the pins. Over a full up cycle the
  byte changes by +4, and over a down cycle by −4.
- The step from s = 3 to s = 0, or from 0 to 3, must also change bits 7-2 by
  one. That is the step that tells a counter from a pin readout.

Sources:

- p.281 as quoted in section 1.
- Figure 8-2: the table "V VQ : D1 D0 / 0 0 : 1 0 / 0 1 : 0 1 / 1 0 : 1 1 /
  1 1 : 0 0", and the "Count Up" and "Count Down" waveforms, in which D0, D1
  ("D2 etc") are the counter bits, D0 changing on every edge of V or VQ.
- Figure 8-3 (p.233): FORWARD and LEFT (pins 1 and 3) are wired to the "MOUSE 0
  Y counter" ("vertical"), and BACK and RIGHT (pins 2 and 4) to the "MOUSE 0
  X counter" ("horizontal").

### 5. Preconditions

Q1 to Q4, the anchor of section 7 of P2, and P3's own reference-frame anchor
(section 7 below).

### 6. Contrasting cases

- **Built in:**
  - The down cycles predict d = −1 where the up cycles predict +1.
  - The X steps predict an unchanged Y byte.
  - The F steps predict no change at all.

  Each of these would turn a wrong mapping red.
- **Variant:** in Y-up(A), the harness drives pin 2 where the table says pin 1.
  The Y byte then does not follow the table, and the X byte moves. The Y
  expectations must go red.

### 7. Reference frame

- **The state numbering s** is anchored on Figure 8-2's table, read with V and
  VQ as the levels on pins 1 and 3.
  - This is the only reading that agrees with p.281 ("The LEFT and RIGHT
    joystick functions (active high) are directly available on the Y1 and X1
    bits") and with p.232 Table 8-3 ("The true logic state of the switch data in
    these registers is '1 = switch closed.'"). Pin 3 at 0 gives D1 = 1.
- **For X**, the page draws no table. The same numbering follows from p.281's
  "Back: X1 xor X0 (BIT#01 xor BIT#00)" and "Right: X1", which have the same
  form as Forward and Left for Y, and from Figure 8-3.
- **How the test checks the anchor:** a dedicated assertion, printed before P3
  is graded and gating P3 and P4: at N0 (all open, s = 0), bits 9, 8, 1 and 0 of
  both registers read 0. Its own mutation drives Yup(A) sub-step 0's pattern at
  N0 and at N1 instead of leaving them released, so that Q3 (N0 equals N1)
  still holds, which must turn the anchor red.

---

## Proposition 4: the Table 8-1 buttons on pins 3, 4 and 1 read as closed in their connector's JOYxDAT

### 1. Proposition

Table 8-1 (p.228) says which physical controller button is connected to each
connector pin. That is a property of the controller, which no port of the
design shows, so the claims below are **declined** and no id of this section
sits on any assertion.

What the package grades under this heading is the chip set's own decode of a
pin pulled to ground (p.281): on each connector, a pin that Table 8-1 gives a
button reads as closed in that connector's JOYxDAT exactly while it is shorted
to ground:

| pin | Table 8-1 button | read at |
|---|---|---|
| 3 | left button of a proportional controller pair; button 1 of an X-Y proportional joystick | bit 9 (Y1) |
| 4 | right button of a proportional controller pair; button 2 of an X-Y proportional joystick | bit 1 (X1) |
| 1 | button 3 of an X-Y proportional joystick | bit 9 xor bit 8 |

- Declined claims, printed page **228**, Table 8-1 (printed in the run as one
  `RECORDED:` line naming all ten ids, with the reason above):
  - pin 3: `opus:8-tbl8-1.c8` and `sol:8-pin.c9` (left button),
    `opus:8-tbl8-1.c9` and `sol:8-pin.c10` (button 1);
  - pin 4: `opus:8-tbl8-1.c12` and `sol:8-pin.c13` (right button),
    `opus:8-tbl8-1.c13` and `sol:8-pin.c14` (button 2);
  - pin 1: `opus:8-tbl8-1.c3` and `sol:8-pin.c3` (button 3).
- The rows quoted: "3 Left VQ-pulse Left button Button 1 ---", "4 Right
  HQ-pulse Right button Button 2 ---", "1 Forward V-pulse --- Button 3** ---".

What Table 8-1 states and a test cannot check: that a real controller wires its
button to that pin. The assertions `P4-PIN1` to `P4-PIN4` check what the chip
set does with a pin shorted to ground, by p.281 and not by p.228.

How the chip set reads these pins:

- p.234: "For the paddle controllers, the left and right joystick direction
  lines serve as the fire buttons for the left and right paddles."
- p.281: "The LEFT and RIGHT joystick functions (active high) are directly
  available on the Y1 and X1 bits of each counter", and "Forward: Y1 xor Y0
  (BIT#09 xor BIT#08)".
- "** These buttons are optional" (Table 8-1) concerns the controller. The pin
  is read either way.

### 2. Stimulus

The joystick program. Buttons(P) walks through all eight combinations of pins
1, 3 and 4, changing one pin per step. Every other record also has a known
combination: N0, N1, the Y and X cycles, and the F steps.

### 3. Observation

The register of connector P (as assigned by P2, with the same swap rule as P3),
in every record of block P, and in N0 and N1.

### 4. Expected

In every record, and on both connectors:

- bit 9 = 1 exactly when pin 3 is at 0;
- bit 1 = 1 exactly when pin 4 is at 0;
- (bit 9 xor bit 8) = 1 exactly when pin 1 is at 0;
- (bit 1 xor bit 0) = 1 exactly when pin 2 is at 0. Pin 2 carries no Table 8-1
  button; this checks that no button reads where none is closed.

"Exactly" is graded both ways, in all eight combinations of the Buttons walk:

- pressed reads 1 and released reads 0;
- one button's press never shows as another's, including when two or three are
  closed together.

Sources: p.281 "To detect / Read these counter bits: Forward Y1 xor Y0 (BIT#09
xor BIT#08); Left Y1; Back X1 xor X0 (BIT#01 xor BIT#00); Right X1"; p.232
Table 8-3 ("1: True logic state of 'right' switch. 9: True logic state of
'left' switch." with the XOR rows) and "The true logic state of the switch data
in these registers is '1 = switch closed.'"; p.234 as quoted in section 1.

### 5. Preconditions

Q1 to Q4, the anchor of section 7 of P2, and P3's own reference-frame anchor
(P3 section 7).

### 6. Contrasting cases

- **Built in:** every press is followed by a release, and the page predicts 0
  for the released button.
- **Variant:** in Buttons(A), the harness grounds pin 2 wherever the table says
  pin 1. The pin-1 reading (bit 9 xor bit 8) then stays 0 while it is expected
  to be 1, and must go red.
- **Variants for pins 3 and 4:** in Buttons(A), the harness grounds pin 2
  wherever the table says pin 3 (pin 3 stays released), and in a second case
  wherever the table says pin 4. The pin-3 reading (bit 9) and the pin-4 reading
  (bit 1) then stay 0 while they are expected to be 1, and must go red.

### 7. Reference frame

- The level convention (0 = shorted to ground), checked by Q4.
- The bit assignment of p.281, whose anchor is P3's N0 check.

---

## Open points (the pages give no value; the test must not invent one)

- **P1, the line level after the reset interval:** the charge rate "will be
  controlled by the position of the external controller resistance" (p.235),
  and a harness pot line carries no resistance. Not graded.
- **P1, reads in lines L0 to L0+5:** the pages give no time for the dump to pull
  the line down. Not graded.
- **P1, a pot line held high by an external driver:** the pages describe the
  dump only on a line nothing else holds. PC2's Rlow shows that the harness's
  level 1 gives way to the chip's own low output. If the harness level did not
  give way to the dump in the same manner, that would be a harness limitation,
  not a finding.
- **POTGOR bits 7-1, bit 0 and bits 15, 13, 11, 9 when read:**
  - bits 7-1: p.240 "chip revision identification number"; p.282 "Reserved for
    chip ID code (presently 0)".
  - bit 0 and the output-enable positions: Figure 8-6 draws them as 0, and
    Table 8-4 does not say what they read.

  None of these is graded.
- **"start counters" and the POT0DAT/POT1DAT reset** (p.236): outside this
  package's claims. Not graded here.
- **P3 and P4, the time for a pin level to reach JOYxDAT:** not given. The
  sampling is "at CCK" / "at CCK*" (p.281). The program waits at least one line.
- **Two pins of one counter changing at the same instant:** Figure 8-2 does not
  say what the counter does. The step table never does it.

---

## Disagreements between pages

- **Reset length (P1).** p.235 says "the first seven or eight horizontal video
  scan lines"; Figure 8-5 says "starts eight horizontal lines after POTGO is
  written". Only the window both allow is used.
- **The right paddle's button (P4).** Table 8-1 (p.228) puts the paddle pair's
  "right button" on pin 4, and the p.234 prose agrees ("the left and right
  joystick direction lines serve as the fire buttons for the left and right
  paddles"). Figure 8-4 (p.234) draws both the left and the right paddle with
  the fire button on pin 3 and the pot on pin 9, and Table 8-1 puts the right
  pot on pin 5.
  - Where a right paddle's button is wired is a controller property the test
    cannot see either way.
  - What P4 grades for pin 4 (a closed switch reads at X1) is stated alike on
    p.281, in p.232 Table 8-3 and in Figure 8-3.
  - The claims `opus:8-tbl8-1.c12` and `sol:8-pin.c13` are declined (P4, section
    1).
- **Port numbering (P2).**
  - p.230, p.232, p.236 and p.240 number the ports 1 and 2 (JOY0DAT, POT0DAT,
    CIAAPRA bit 6 and DATLX/DATLY belong to port 1). Appendix A (p.281, p.282)
    says left and right. p.233's text says "ports 0 and 1".
  - Figure 8-6 (p.237) draws a "PORT 1 connector" wired to POT1Y/POT1DAT and to
    OUTRY/DATRY/RY. With the 1-based numbering of p.236 ("POT1DAT - port 2
    data") and p.240 ("DATRY data for port 2, pin 9") that is a contradiction.
    With p.233's 0-based numbering it agrees.
  - The only mapping all pages share is the order: the connector of JOY0DAT is
    the one whose fire button is CIAAPRA bit 6. P2's anchor uses only that.
- **The name of $DFF016.** The printed p.240, p.241 and Figure 8-6 call it
  POTINP. p.282 says "Pot port data read (formerly called POTINP)" under
  POTGOR, and Appendix B p.289 says "POTGOR ... (formerly POTINP)". Same
  address; no effect on the test.
- **Table 8-1's header.** The printed table heads its first column "Pen" where
  the Developer CD text has "Pin". The rows are pin numbers.

---

## The build

### Programs

- `prog_fixture.s` / `prog_fixture_mut.s`: apparatus check, no claim id.
- `prog_p1.s`: Proposition 1's stimulus exactly as sections 1-7 above (Phase
  P: F1, F2, F3; Phase D: C0-C4, each Fa/Fb/Fc), storing every Vw0/Vw1/M1
  (Fc also Rpre) and, per R step, four records of Vr0/M2/POTGOR/Vr1,
  sequentially from $0C1000.
- `prog_p1_k1.s`, `prog_p1_k3.s`, `prog_p1_k4.s`, `prog_p1_k6.s`,
  `prog_p1_k7.s`, `prog_p1_outlow.s`: `prog_p1.s` included with an
  assembly-time symbol set (K1/K3/K4/K6/K7/OUTLOW), for the contrasting cases
  of section 6. K2 needs no separate program: it is already inside the
  sequence (each configuration's own output-driven line, graded by the same
  assertion's "1" column). K5 is a harness-side stimulus variant of
  the unmutated `prog_p1.s` (see the K5 record below).
- `start_model.patch` and the `sm-*` targets of the Makefile: the private copy
  of the RTL of the green evidence (section "The private copy of the RTL").
- `prog_joy.s`: the joystick program (54 steps: N0, N1, Block A and Block B
  of 26 steps each), shared by P2, P3 and P4 exactly as specified. One
  binary serves the baseline run and every one of P2/P3/P4's contrasting
  cases (Q1-Q4, P2's variant and anchor contrast, P3's variant, P4's
  variant): none of them changes the program's own code, only which
  harness port level `test_ch8_controller_ports.cpp` applies for a given
  step, selected by `NMH_MUTATE`.
- The apparatus selectors `P1LOAD`, `CFGMUT` and `JOYLOAD` name a program file
  that does not exist (`prog_p1_absent.bin`, `prog_joy_absent.bin`); the
  repository holds no such file.
- A selector runs only the part of the package whose lines it changes (the POTGO
  program, or the joystick program), and every selector runs all four
  PAL/NTSC x ECS/OCS configurations (the joystick program in each, graded in
  each on its own: Q1 to Q4, the anchors, P2, P3 and P4, with the configuration
  in the label). The clean run, with no selector, runs both
  parts.

### Connector assignment (measured, not assumed)

`test_ch8_controller_ports.cpp` assigns Connector A (JOY0DAT, CIAAPRA bit
6) to the harness's `joystick1` port and Connector B (JOY1DAT, CIAAPRA bit
7) to `joystick0` — the opposite of the naive index reading ("joystick0" =
"0=left"). This choice is confirmed, not assumed, by the P2 anchor
(section 7): `P2-ANCHOR` printed `PASS` on both fx68k and TG68K.

### Measured: the R step's own timing margin

The R step's instruction count had to fit inside the one scan line (~1820
`clk_sys` cycles at Appendix F's PAL clock, p.352 "PAL 28.37516 MHz"; that
`clk_sys` is this clock comes from the implementation, since the harness
contract defines no clock) that PC1 grades. `READV` (`ch8_regs.i`) reads
`VPOSR`/`VHPOSR` as one long word (p.238's own technique) and extracts
V7-V0/V8 with `SWAP` and word shifts. PC1 is the precondition that checks
this margin at every run; a run where it fails is "not established", not
"pass". Measured: fx68k yields 20 of a possible 72 records (4 per R step x
18 R steps) with Vr0=Vr1=10; TG68K yields 42.

### Measured: P1's main claim on this build

Both cores print the identical verdict pattern in every one of the four
SPECIFICATION.md "Configurations" (PAL/NTSC x ECS/OCS), each graded on its
own: PC1 through PC4 all print `PASS`. Every "0" column (`C0-Fc` through
`C4-Fc`) prints `FAIL`: POTGOR does not read 0 in the input lines after
START, and `P1-START`, which carries the two claim ids, prints `FAIL` with
them. Every "1" column (`C1-Fc-OUT` through `C4-Fc-OUT`) prints `PASS`:
the output-driven line still reads 1, as p.240's output-enable sentence
predicts. `P1-FIXTURE` and `P1-CONFIGS` print `PASS` on both cores (PAL+ECS,
and NTSC+ECS, PAL+OCS and NTSC+OCS, load, release and reach `DONE`). The
qualifying-record counts (PC1, PC4) differ by configuration and by core; each is
printed against its own configuration's labels.

Measured at `nanomig.v`'s ports (K5, below, with LX, LY and RY grounded):
bits 8, 10 and 14 read 0; bit 12 reads 1. K5 is scoped to the ports that
reach bits 8, 10 and 14 (mouse_buttons bit 2 for bit 8; mouse_buttons bit 1
and joystick1 bit 5 for bit 10; joystick0 bit 5 for bit 14); no port at this
boundary reaches bit 12.

Final run of the clean package, every core: 109 passed, 24 failed (133 graded
lines: the 20 confirmed reds `C0-Fc` to `C4-Fc` in the four configurations, the
four `P1-START` reds, and 109 passes). The joystick part grades each of the four
configurations on its own: 18 lines each (Q1 to Q4, `P2-ANCHOR`, the six P2
lines, `P3-ANCHOR`, `P3-Y`, `P3-X`, and the four `P4-PIN*`), plus `JOY-FIXTURE`
and `JOY-CONFIGS`, 74 lines in all; every one prints `PASS`.

The same package against the private copy of the RTL (section "The private copy
of the RTL"): 133 passed, 0 failed on both cores; the 24 lines above print
`PASS`, with PC1 to PC4 green.

### The private copy of the RTL

`start_model.patch` is a patch to a copy of the chip-set RTL, made under
`obj_dir_sm/` by `make sm-rtl` (the pinned submodule is never modified). In the
copy, POTGOR reads 0 on every pot line that is an input (its output-enable bit
is 0), for eight lines after a POTGO write with START (Figure 8-5: charging
starts eight lines after POTGO is written); a line that is an output reads as
before. `make sm-test-fx68k` and `make sm-test-tg68k` build and run the
unchanged package against it. The graded read is at L0+6, inside the interval
under either reading of p.235. This copy is the green evidence for `C0-Fc` to
`C4-Fc` and `P1-START`; it is not the repository's RTL, and the repository's RTL
stays red on these lines.

### Mutations: what went red, and through which clause

Run once each, both cores, via `NMH_MUTATE=<value>`, from each object
directory (`./Vnanomig ..`). Every result below is the pass/fail count and
the clause that actually printed; fx68k and TG68K printed the identical
pattern for every mutation below, line for line. A selector runs only its own
part (see Programs), so the counts are of that part: the POTGO part is 59
lines, the joystick part 74. A line whose preconditions are not met prints
`NOT GRADED` (a `RECORDED` line naming the unmet preconditions), neither `PASS`
nor `FAIL`, and is not counted.

POTGO part (all four configurations each; "the sixteen output lines" are
`C1-Fc-OUT` to `C4-Fc-OUT` in the four configurations, "the twenty reds" are
`C0-Fc` to `C4-Fc` in the four configurations):

- **FIXTURE** (apparatus check; no other part runs): 0 passed, 1 failed on
  both cores -- the program stores $FACE where the comparator expects $FEED (the
  read-back itself is faithful), and `FIXTURE` alone flips from `PASS` to
  `FAIL`.
- **K1** (`C1-Fc-OUT` through `C4-Fc-OUT`): 35 passed, 24 failed -- no line
  moves: the same 24 reds print (the twenty `C*-Fc` and the four `P1-START`),
  and the sixteen output lines print `PASS`, unchanged. Against the private copy
  the same selector gives 35 passed, 24 failed: the 24 lines that print `PASS`
  there are red again when START is withheld.
- **K3** (`PC1`): 15 passed, 4 failed -- `PC1` prints `FAIL` in each of the
  four configurations ("every R step has at least one record with
  Vr0=Vr1=10"); because `PC1` gates "established", the twenty `C*-Fc`, the
  sixteen output lines and the four `P1-START` lines are `NOT GRADED`; `PC2`,
  `PC3` and `PC4` stay `PASS`.
- **K4** (`PC2`): 15 passed, 4 failed -- `PC2` prints `FAIL` in each
  configuration and the same 40 lines are `NOT GRADED`; `PC1`, `PC3` and `PC4`
  stay `PASS`.
- **K5** (`PC3`): 15 passed, 4 failed -- `PC3` prints `FAIL` in each
  configuration through both of its clauses (Rb and Rpre read $1000, and the
  drive log shows the pot ports at level 0; `PC2` prints `PASS`: Phase P is
  outside this mutation's Fb/Fc windows), and the same 40 lines are `NOT
  GRADED`. Measured (RECORDED, in each configuration): Rb=Rpre=$1000 with LX, LY
  and RY grounded -- bit 12 (RX) is outside this mutation's reach and stays 1,
  the three others read 0.
- **K6** (`PC1`, not `PC4`): 15 passed, 4 failed -- moving the R step to line 5
  leaves no record with Vr0=Vr1=10, so `PC1` prints `FAIL` in each
  configuration and `PC4` prints `PASS` (nothing there for it to check); the
  same 40 lines are `NOT GRADED`.
- **K7** (`PC4`): 15 passed, 4 failed -- `PC4` prints `FAIL` in each
  configuration, `PC1`, `PC2` and `PC3` stay `PASS`, and the same 40 lines are
  `NOT GRADED`.
- **OUTLOW** (`C1-Fc-OUT` through `C4-Fc-OUT`): 19 passed, 40 failed --
  `PC1` through `PC4` stay `PASS`; the sixteen output lines all print `FAIL`
  (the output-driven line reads 0 with its data bit held low), and the twenty
  `C*-Fc` reds and the four `P1-START` reds are unaffected.
- **P1LOAD** (`P1-FIXTURE`): 26 passed, 19 failed -- the PAL+ECS run loads a
  file that does not exist, `P1-FIXTURE` prints `FAIL`, the 14 default-label
  lines of that configuration are not graded (a `RECORDED: NOT GRADED` line
  names them), and the three other configurations print as in the clean run.
- **CFGMUT** (`P1-CONFIGS`): 26 passed, 19 failed -- the NTSC+ECS run loads a
  file that does not exist, `P1-CONFIGS` prints `FAIL`, and that
  configuration's 14 lines are not graded; the other three configurations and
  `P1-FIXTURE` print as in the clean run.

Joystick part (each selector runs all four configurations):

- **JOYLOAD** (`JOY-FIXTURE`): 1 passed, 1 failed -- the program is not
  loaded, `JOY-FIXTURE` prints `FAIL` and nothing else is graded (a
  `RECORDED: NOT GRADED` line says so).
- **JOYCFG** (`JOY-CONFIGS`): 74 passed, 1 failed -- a further NTSC+ECS run
  takes Q3's stimulus and its records are compared with the PAL+ECS run's;
  `JOY-CONFIGS` prints `FAIL` ("disagreed: NTSC+ECS") and nothing else moves.
- **Q1** (timing clause): 23 passed, 4 failed -- `Q1` prints `FAIL` in each
  configuration (t_apply(k) < t(T_k), graded over all 54 steps; the note prints
  "timing clause fails, hold clause holds"), and, gated by it, the 24 P2/P3/P4
  lines are `NOT GRADED`; `P3-ANCHOR` is unaffected (`PASS`).
- **Q1HOLD** (hold clause): 23 passed, 4 failed -- `Q1` prints `FAIL` with the
  note "timing clause holds, hold clause fails": the trace shows a change of
  joystick0 inside (t_apply(k), t(U_k)] of step 5; the same lines are `NOT
  GRADED`.
- **Q2**: 23 passed, 4 failed -- `Q2` prints `FAIL` (the additional ground on
  connector A pin 9 during Yup(A) breaks its POTGOR clause: POTGOR&$5500 =
  $5100, bit 10 reads 0, in the four Yup(A) records, steps 4-7), and the same
  P2/P3/P4 lines are `NOT GRADED`; `Q1`, `Q3`, `Q4` and `P2-ANCHOR` print `PASS`.
- **Q2MOUSE**, **Q2KEY**: 23 passed, 4 failed each -- the transient is back at
  0 before the step's reads, no record changes, and `Q2` prints `FAIL` through
  its drive-trace clause alone; the same lines are `NOT GRADED`.
- **Q3**: 23 passed, 4 failed -- `Q3` prints `FAIL` ("JOY0DAT and JOY1DAT are
  equal in N0 and N1"), and the same lines are `NOT GRADED`; `Q1`, `Q2` and
  `Q4` print `PASS`.
- **Q4**: 23 passed, 4 failed -- `Q4` prints `FAIL` (F0(A) held low instead of
  released), and the same lines are `NOT GRADED`; `Q2` and `P2-ANCHOR` stay
  `PASS`.
- **P2VAR**: 51 passed, 24 failed -- the six P2 lines (`P2-BLOCKA-JOY1`,
  `P2-BLOCKA-JOY0`, `P2-BLOCKB-JOY0`, `P2-BLOCKB-JOY1`, `P2-FOUR-COUNTERS`,
  `P2-JOYDAT`) print `FAIL` in each configuration through their own clause
  (swapping which connector receives each block's steps reverses what each
  register is supposed to do). The swap is clean, a `MEASURED` line reports it
  under P2, and `P3-Y`, `P3-X` and the four `P4-PIN*` lines read the swapped
  registers and print `PASS`; `Q1` to `Q4` and `P2-ANCHOR` are unaffected
  (`PASS`: the F1/F0 steps stay on their own connector).
- **P2ANCHOR**: 23 passed, 4 failed -- `P2-ANCHOR` prints `FAIL` through its
  own clause (F1(A) additionally grounds pin 6 of B); `Q2` and `Q4` stay
  `PASS`, and the 24 P2/P3/P4 lines are `NOT GRADED`.
- **P3ANCHOR**: 47 passed, 4 failed -- `P3-ANCHOR` prints `FAIL` through its
  own clause (N0 and N1 carry Yup(A) sub-step 0's pattern instead of staying
  released), `Q3` stays `PASS` (N0 still equals N1), and `P3-Y`, `P3-X` and the
  four `P4-PIN*` lines are `NOT GRADED`; `P2-JOYDAT` and the five P2 component
  lines stay `PASS`.
- **P3VAR**: 51 passed, 24 failed -- `P2-FOUR-COUNTERS`, `P2-JOYDAT`, `P3-Y`,
  `P3-X`, `P4-PIN1` and `P4-PIN2` print `FAIL` (in Yup(A) the pattern of pin 1
  is driven on pin 2, so the Y byte does not follow pin 1 and the X byte moves
  during the Y-up steps); `P4-PIN3`, `P4-PIN4` and the four block lines are
  unaffected (`PASS`).
- **P4VAR**: 59 passed, 16 failed -- `P3-Y`, `P3-X`, `P4-PIN1` and `P4-PIN2` print
  `FAIL` (grounding pin 2 wherever Buttons(A) grounds pin 1 corrupts both axes'
  records the same way); `P2-FOUR-COUNTERS`, `P2-JOYDAT`, `P4-PIN3` and
  `P4-PIN4` are unaffected (`PASS`).
- **P4VAR3**: 59 passed, 16 failed -- pin 2 is grounded wherever Buttons(A)
  grounds pin 3: `P3-Y`, `P3-X`, `P4-PIN2` and `P4-PIN3` print `FAIL`;
  `P4-PIN1`, `P4-PIN4` and the P2 lines are unaffected (`PASS`).
- **P4VAR4**: 63 passed, 12 failed -- pin 2 is grounded wherever Buttons(A)
  grounds pin 4: `P3-X`, `P4-PIN2` and `P4-PIN4` print `FAIL`; `P3-Y`,
  `P4-PIN1`, `P4-PIN3` and the P2 lines are unaffected (`PASS`).
