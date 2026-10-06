# Specification: appDE-ports-disk

This document specifies the integration test package `appDE-ports-disk`. It
was written from the package's claims (`claims-spec-stage.jsonl`) and the pages
of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never
saw the implementation or any test. Page numbers are printed page numbers.
Every sentence quoted below was checked against the rendered printed page. The
text layer is not used where the two differ.

The package covers three topics from Appendix E: the light pen on the game
ports (p.327, p.333), the pen-pressed button (p.327), and the identification
mode of the external disk connector (p.335). Five propositions come out of the
light-pen and pen-pressed claims. None comes out of the identification claims
(section 0.1).

---

## 0. Claims not turned into a proposition

### 0.1 External disk identification mode, p.335 (19 claims)

| uid | what it says |
|---|---|
| opus:E-extdisk-id.c2, sol:E-disk-id.c1 | an identification mode reads a 32-bit serial stream from an external device |
| opus:E-extdisk-id.c3, sol:E-disk-id.c2 | to initialize the mode the motor must be turned on, then off (pin 8, MTRXD*) |
| opus:E-extdisk-id.c4, sol:E-disk-id.c3 | the motor-on to motor-off transition reinitializes the serial shift register |
| opus:E-extdisk-id.c5, sol:E-disk-id.c4 | after initialization SELxB* is left inactive |
| opus:E-extdisk-id.c6, sol:E-disk-id.c5 | the loop drives SELxB* active, reads RDY* (pin 1), drives SELxB* inactive |
| opus:E-extdisk-id.c7, sol:E-disk-id.c6, sol:E-disk-id.c7 | 32 repetitions, most significant bit first |
| opus:E-extdisk-id.c9, sol:E-disk-id.c8 | `$0000 0000 - no drive present.` |
| opus:E-extdisk-id.c10, sol:E-disk-id.c9 | `$FFFF FFFF - Amiga standard 3.25 diskette.` |
| opus:E-extdisk-id.c13, sol:E-disk-id.c12 | the serial input is active low and must be inverted |

These claims cannot be checked, for three reasons. Each reason is enough on its own.

1. **The behaviour belongs to a device the test cannot attach.** The stream
   comes "from an external device" (p.335). The shift register that the
   motor-off transition reinitializes is that device's own. Appendix H says
   the same thing (p.369: "All external drives must have logic equivalent to a
   D flip-flop, whose D input is the MTRXD- signal"). A test cannot drive RDY*
   (pin 1), and it cannot see MTRXD* (pin 8) or SEL1B*..SEL3B* on the external
   disk connector. These lines are not among the inputs a test drives or the
   outputs it sees. So no stimulus can produce an identification stream, and
   no observation can show the program's select/motor sequence at the
   connector.
2. **The only device-free case has no agreed value.** With no device present,
   p.335 gives `$0000 0000 - no drive present.` and says "The serial input
   data is active low and must therefore be inverted to be consistent with the
   above table." Appendix H lists the same pattern as "Reserved ($0000 0000)"
   (p.371). It also says "Convert the 32 values of RDY- into a two 16-bit word.
   The most significant bit is the first value and so on. This 32-bit quantity
   is the device I.D." and never mentions inversion. Read literally, the two
   appendices assign opposite IDs to the same line levels. That case also has
   no contrasting case, because any other ID needs a device.
3. The step claims (c3–c7, c2–c7) describe what software does. They are not
   something the chip set produces.

**None of the claims in this package needs data read from or written to a disk
surface.** The missing disk image is not why these claims fail.

### 0.2 Light-pen pins marked "Unused", p.333 (10 claims)

opus:E-lightpen.c7 (pin 1), opus:E-lightpen.c8 (pin 2), opus:E-lightpen.c9
(pin 3), opus:E-lightpen.c10 (pin 4), opus:E-lightpen.c15 (pin 9),
sol:E-lightpen.c5 (pin 1), sol:E-lightpen.c6 (pin 2), sol:E-lightpen.c7
(pin 3), sol:E-lightpen.c8 (pin 4), sol:E-lightpen.c13 (pin 9).

"Unused" states no behaviour, so the claims give no expected value. The same
pins are the joystick direction switches (p.329) and the POTY line (p.332).
Driving them changes JOY0DAT/JOY1DAT and POTGOR, so a test cannot take
"unused" to mean "driving it has no effect". For pin 9 the pages also
disagree. p.333 prints "9 Unused". Table 8-1 on p.228 gives the light-pen
column's pin 9 as "button 2**" with the footnote "** These buttons are
optional".

### 0.3 Parts of used claims that are not graded

- **opus:E-lightpen.c4, sol:E-lightpen.c3, first half:** "There is no hardware
  bit to indicate this trigger". This is a statement about every bit in the
  system, and no finite observation establishes it. The pages also leave open
  which bits it covers: pin 6 is the fire button, and its level shows in CIA-A
  PRA bit 6/7 (p.329, p.337) while the pulse lasts. Only the second half ("this
  can be determined in the two ways as shown in chapter 8") is used, in P4.
- **opus:E-lightpen.c24, "PORT 0":** the pages disagree about which port
  carries the light pen (section 1, item 2). The test reports the port where
  the latch occurs but does not grade it.
- **opus:E-lightpen.c5, sol:E-lightpen.c4, "but MAY be available prior to
  that":** p.333 calls this only possible, so the claim alone gives no value
  before blanking. The value graded in P3 comes from p.239, which states it
  definitely.
- **opus:E-gameport.c6, sol:E-gameport.c5:** the list of four device types is
  not a behaviour. Only the pen-pressed-to-screen button is graded (P5).

---

## 1. Where the pages disagree or are unclear, and what is specified

| # | topic | pages | specified here |
|---|---|---|---|
| 1 | end of internal vertical blanking, which releases the light-pen latch | p.239: "This freeze is released at the end of internal vertical blanking (vertical position 20)." p.219: "The range starts at line 0 and ends at line 20 for NTSC or line 25 for PAL." p.79, Table 3-13: "Vertical Blank Stop $15 (21) [NTSC] $1D (29) [PAL]". | All pages agree that the latch is held in lines 1–19 and released by line 21 (NTSC) or line 29 (PAL). Line 20 (NTSC) and lines 20–28 (PAL) are not graded. |
| 2 | which port carries the light pen | p.333 figure: light pen on "PORT 0" pin 6. p.238: "On the A1000, the light pen must be connected to port 1. ... On the A500, A2000 and A3000 the default is port 2. An internal jumper can select port 1." p.229 (JOY0DAT "port 1", JOY1DAT "port 2"), p.327 ("J12 is the left controller port connector (JOY0DAT, POT0DAT)", J11 right), p.337 ("game port 0"/"game port 1") make the figure's PORT 0 the same as chapter 8's port 1, the left connector J12. | Graded: pin 6 of *a* controller port. The test tries both connectors and requires at least one to latch. Which one latches is reported, not graded. |
| 3 | register addresses in the p.333 figure | The figure labels both "BPLCON0 write only DFF104" and "POTINP read only DFF104". p.229 gives BPLCON0 as $DFF100, p.240 gives POTINP as $DFF016, the p.332 figure gives POTINP DFF016, and Appendix B gives BPLCON0 100 and POTGOR 016. | Appendix B addresses. The figure is right only about the bit number (bit 3) for BPLCON0. |
| 4 | the POTINP bit that the p.333 figure's "PEN PRESS = POT0X" arrow points to | The drawn register has 15 fields for 16 bits. The arrow falls on bit 9 counting from 15 and on bit 8 counting from 0. p.240: "08 DATLX data for port 1, pin 5". | Bit numbers from p.240. |
| 5 | the threshold $10500 (V=261) in p.239's methods | The threshold matches NTSC. In PAL, lines 262–311 are display lines, and a trigger there would read above $10500, which p.239 calls "not triggered". | Checks of the triggered branch use trigger lines ≤ 260 only, in both standards. |
| 6 | VPOSR bits 14–1 | p.216: "Unused". p.238: "Chip ID code. Do not depend on value!" | Not graded. Equality and comparisons use the 17 position bits (section 2.2). |
| 7 | field length without interlace | p.24: "262 NTSC (312 PAL) possible vertical positions", and long and short fields are "(interlace only). In NTSC, the fields are 262, then 263 lines and in PAL, 312, then 313 lines." p.216: "PAL line counts (313)". | The anchor accepts 262 or 263 lines (NTSC) and 312 or 313 lines (PAL), constant from field to field. |
| 8 | p.333: "Note: depending on the maker, the light pen input may be either." | The sentence breaks off. "Either" (port? polarity?) is not resolved. | The pulse contains both edges, and the latched H is graded against the whole pulse span, so either reading passes. |
| 9 | pen-pressed line | p.333 row 5 "LPENPR* Light pen pressed See Proportional Inputs". p.228: pin 5 "pen pressed to screen". p.238: "connected to one of the potentiometer inputs and must be read as same as the right or middle button on a mouse". | Pin 5 (all pages agree). p.238's "right or" is not graded. |
| 10 | the moment of the no-trigger latch | p.239: "If no trigger is seen, the counters latch at the end of the display field." No position is given. | Only the relation "greater than $10500" is graded (P4). The moment of the latch and the exact value are not. |
| 11 | disk identification | p.335 against pp.370–371 | Section 0.1. |

---

## 2. Common setup

### 2.1 Configurations

Every run described below is performed on **both CPU cores**, with the chip set
in **OCS and in ECS**, with **PAL and with NTSC** selected, 2 MB Chip memory,
and no interlace. The core that can also behave as a 68010 or a 68020 runs in
its default 68000 mode: none of the claims involves a later processor, and the
read brackets of section 2.4 assume the 68000 bus sequence. Each contrasting
run is performed at least in ECS with PAL on both cores. Neither the video nor
the audio output is used by this package.

### 2.2 Registers and the position value

Addresses are from Appendix B (pp.289–293): VPOSR $DFF004 (read), VHPOSR
$DFF006 (read), JOY0DAT $DFF00A, JOY1DAT $DFF00C, POTGOR $DFF016 ("formerly
POTINP"), POTGO $DFF034, DMACON $DFF096, INTENA $DFF09A, BPLCON0 $DFF100,
BEAMCON0 $DFF1DC (ECS only).

Bit layouts:

- VPOSR bit 0 is V8. VHPOSR bits 15–8 are V7–V0 and bits 7–0 are H8–H1
  (p.216, p.238, p.285). The horizontal resolution is one color clock
  (p.285: "1/160 of screen width (280 ns)"). p.24: "One horizontal count takes
  one cycle of the system clock".
- BPLCON0 bit 3 is LPEN (p.271: "LPEN =Light pen enable (reset on power up)";
  p.229: "Bit 3 enables the light pen latch").
- BEAMCON0 bit 13 is LPENDIS ("Ignore latched pen value on vertical pos
  read") and bit 5 is PAL ("Programmable pal mode enable") (p.305).
- POTGO/POTGOR bits are as in Table 8-4 (p.240).

**Position value** P of a long read: bits 16–0 of the 32-bit word, i.e.
P = ((VPOSR & 1) << 16) | VHPOSR. This is p.239's "mask off the upper 15 bits
of the 32-bit word". V(P) = P >> 8 and H(P) = P & $FF. All equality checks and
comparisons in P1–P4 use P.

### 2.3 The sampling program (P1–P4)

The program is loaded into Chip memory. A data block in Chip memory holds the
configuration words cfg_BEAMCON0, cfg_BPLCON0_ref (= $0000) and cfg_BPLCON0.
Each register below that gets a configuration value is written with a move
*from its memory word* (for example `MOVE.W cfg,$DFF100`). That way the
Chip-memory port shows the CPU reading the value just before the write.

- **S1.** INTENA ← $7FFF and DMACON ← $7FFF. Bit 15 is SET/CLR, so these clear
  every enable (p.280). No interrupt and no DMA channel then disturbs the loop.
- **S2.** Connector identity phase (section 2.8, C0): 2 fields of
  `MOVE.W $DFF00A,(A0)+ ; MOVE.W $DFF00C,(A0)+`.
- **S3.** ECS only: BEAMCON0 ← cfg_BEAMCON0. The value is $0020 when PAL is
  selected and $0000 when NTSC is selected, so LPENDIS = 0 and the PAL bit
  restates the selected standard. p.305 gives no reset value for BEAMCON0, so
  the test writes it instead of assuming it. p.305's warning ("Do not write to
  this register") concerns monitors, and the value written only restates the
  selected standard.
- **S4.** BPLCON0 ← cfg_BPLCON0_ref = $0000 (LPEN = 0, LACE = 0, no
  bitplanes).
- **S5. Reference phase:** N_ref iterations of `MOVE.L $DFF004,(A0)+`, looped
  with DBF (and an outer counter where needed). p.216: "the address pairs
  VPOSR,VHPOSR ... can be read from ... as long words". N_ref is chosen so
  that the phase covers at least 3 complete fields.
- **S6.** BPLCON0 ← cfg_BPLCON0: $0008 in the base runs (only bit 3 set) and
  $0000 in the P1 contrast.
- **S7. Measurement phase:** the same loop for N_meas iterations, covering the
  schedule of section 2.5 through line 100 of field F10.
- **S8.** Write a done marker (a fixed word at a fixed address).

The program contains no branch that depends on a value it reads, so it runs
for the same number of iterations and finishes in every variant.

### 2.4 Read instants

The program runs from Chip memory, so the port shows every instruction fetch
and every store with its time. Each long read consists of two word reads
(VPOSR, then VHPOSR). They happen between visible CPU accesses.

- The **bracket** of a word read runs from the last visible CPU access before
  it to the first visible CPU access after it, following the 68000 bus
  sequence of `MOVE.L abs.L,(An)+` on the core in use.
- If the port record includes the CPU's custom-register accesses themselves,
  their times are used and the bracket has width 0.

Sample i therefore has a value P_i and two brackets: B_i^hi for the VPOSR
word and B_i^lo for the VHPOSR word.

### 2.5 Field schedule and the pin-6 pulse

The fields are numbered from the field in which S6's write occurs (F0). The
harness drives each pulse on **pin 6 of the connector under test**. Each pulse
works as follows:

- Pin 6 goes low for exactly 4 color clocks and then high again.
- The falling edge comes half a color clock after the moment at which the
  anchor model (section 2.6) gives the counter the target position (v, h).
- All other controller pins of both connectors stay at their inactive high
  level for the whole measurement phase, including pin 6 of the other
  connector.

| field | pin-6 pulse(s) at (V, H) | role |
|---|---|---|
| F1 | none | untriggered, latch enabled for the whole field |
| F2 | (100, $40) | triggered |
| F3 | (100, $41) | triggered, H + 1 relative to F2 |
| F4 | (100, $48) | triggered, H + 8 relative to F2 |
| F5 | (101, $40) | triggered, V + 1 relative to F2 |
| F6 | (258, $40) | triggered, V8 = 1 |
| F7 | (60, $40) and (180, $40) | two pulses in one field |
| F8 | none | untriggered |
| F9 | (10, $40) | pulse during vertical blanking, before the release |
| F10 | none | untriggered; the run ends at line 100 |

Every target position lies at a line ≥ 29 or at line 10. Its H is at least
$40 color clocks from the start of the line and far from the line's end, so
neither the pulse width nor the anchor uncertainty can move a pulse into
another line.

The schedule is run twice per configuration: **run J12**, with pulses on the
left connector (JOY0/POT0, the figure's PORT 0, chapter 8's port 1), and
**run J11**, with pulses on the right connector. The connector on which P2
passes is called the light-pen connector below.

### 2.6 Reference frame: beam position against harness time (the anchor)

The pages anchor the frame as follows:

- p.23: "The horizontal beam position has a value of $0 to $E2." Also: "Every
  other line is a long line (228 color clocks, 0-$E3), with the others being
  227 color clocks long. In PAL, they are all 227 long."
- p.24: the field lengths quoted in section 1, item 7, and "One horizontal
  count takes one cycle of the system clock".
- p.216: VHPOSR reads "the counter that is producing the beam on the screen".
  This holds while LPEN = 0.

**Construction.** From the reference phase (LPEN = 0), the test determines
four quantities:

1. R, the harness cycles per color clock. The builder takes it from the
   harness configuration, and the reads confirm it.
2. The time at which some line starts.
3. The pattern of long and short lines.
4. The field length L_f.

The resulting model gives the counter (V(t), H(t)) for every harness time t
in the run. The model's **uncertainty u** is the largest uncertainty of a
count boundary that the reads allow, rounded up to whole color clocks.

**Anchor checks.** A1, A3 and A4 together form precondition C3. A2 is a
graded line of its own and not part of C3. The light-pen sentences are stated
on "the counter that is producing the beam on the screen" (p.216), on "the
current contents of the beam register" (p.238) and on V alone (the release at
V = 20, $10500 = V 261); none of them depends on the line-length pattern of
p.23. The light-pen lines are therefore graded in NTSC as in PAL, whatever A2
reads.

- **A1.** Every reference-phase word read equals the model value at some
  instant inside its bracket.
- **A2.** Line lengths are 227 and 228 alternating (NTSC) or all 227 (PAL).
  H never exceeds $E3 (NTSC) or $E2 (PAL). This is graded as a proposition of
  its own (p.23), in every configuration.
- **A3.** L_f ∈ {262, 263} for NTSC or {312, 313} for PAL, the same for every
  reference field.
- **A4.** In every measurement field, every read in that field's live window
  (section 2.7) equals the model value inside its bracket. This checks the
  anchor again in every field, from reads taken in the same field as that
  field's pulse.

For the absolute H check of P2 (4b), u ≤ 2 is required.

### 2.7 Windows (per field f; line numbers from the model)

Rb is 21 for NTSC and 29 for PAL (section 1, item 1). L_f − 1 is the last line
of the field.

- **W_blank(f):** reads whose brackets both lie in lines 1–19 of f. p.239
  prints the period as "0<V20", i.e. 0 < V < 20.
- **W_live(f):** reads whose brackets both lie in lines Rb … L_f − 2 of f. In
  a field with a pulse, the window ends at the last read before the first
  pulse's falling edge.
- **W_trig(f):** for a field with a pulse, the reads from the first read whose
  VPOSR bracket starts after the first pulse's rising edge to the last read
  whose VHPOSR bracket ends before line 20 of field f+1 begins.
- **Not graded:** line 0 and line L_f − 1 of untriggered fields (the page does
  not locate the no-trigger latch; section 1, item 10), lines 20 … Rb − 1, and
  any read whose bracket overlaps a pulse.

### 2.8 Common preconditions, each with its contrasting case

| id | precondition the test asserts first | contrasting case that turns it red |
|---|---|---|
| C0 | **Connector identity.** In S2 the harness drives pin 4 (RIGHT*) of harness connector A low during the first of the two fields and high in the second. Bit 1 of exactly one of JOY0DAT and JOY1DAT follows pin 4 (p.329: "Right: bit#1"; p.281: "0=left controller pair, 1=right controller pair"). If it is JOY0DAT, A is J12 (p.327). | The harness drives pin 3 (LEFT*) instead. p.329 gives Left as "bit#9", so bit 1 does not follow and C0 fails. |
| C1 | The program finished: the done marker is present and the sample count is N_ref + N_meas (plus the S2 samples). | Remove S8. C1 fails. |
| C2 | The stimulus values were written: the port shows the CPU reading each configuration word, with the declared value, in the order S3, S4, S6. | Declare $0008 and store $0000 in cfg_BPLCON0 (this is the P1 contrast). C2 reports the mismatch. |
| C3 | The anchor holds: A1, A3 and A4 (section 2.6). | A3: NTSC is selected through chipset_config while PAL is declared; the field then has 263 lines, not 312 or 313. A1: a VPOSW write inside the reference phase flips V8 of the vertical counter for 100 reads (and a second write flips it back); the model fits the other reads and the flipped ones disagree with it. A4: the same VPOSW flip for 100 reads of F2 before its pulse. |
| A2 | Graded line (section 2.6), not a precondition. | A VHPOSW write inside the reference phase takes H above $E2. Declaring the other video standard than the one selected, or R one harness cycle too large, changes the expected side only; those two changes are oracle-side. |
| C4 | Density: consecutive VHPOSR reads are at most 32 color clocks apart in every graded window, every W_blank has at least 10 reads, and after each pulse at least 2 reads fall within 64 color clocks of its rising edge. | Add a delay of more than one line to the loop body. C4 fails. |
| C5 | The pulses are as scheduled: the harness log shows each pulse on pin 6 of the connector under test, 4 color clocks wide, at its scheduled time, and no other controller pin changes during S5–S8. | Log a pulse on pin 5 instead of pin 6. C5 fails. |
| C6 | ECS only: BEAMCON0 was written with bit 13 (LPENDIS) = 0 (checked through C2). | BEAMCON0 bit 13 = 1. p.305: "Ignore latched pen value on vertical pos read". The V bits of the reads in W_trig then equal the model V (the moving beam), so the hold checks of P1 and P3 fail on V. |

---

## P1. BPLCON0 bit 3 enables the light-pen latch

**1. Proposition.** Bit 3 of BPLCON0 is the light-pen enable. With it set, a
pulse on the light-pen trigger freezes VPOSR/VHPOSR. With it clear, the same
pulse leaves them reading the moving beam.

- Claims: opus:E-lightpen.c21 and sol:E-lightpen.c18 (p.333 figure: bit 3 of
  BPLCON0, "light pen enable"); opus:E-lightpen.c3 and sol:E-lightpen.c2
  (p.333: "a trigger pulse is generated which can be enabled to latch the
  horizontal and vertical beam positions").
- Page: p.333.
- Both halves are graded, each on its own line, and the claims above sit on
  the aggregate of the two: the set half (P1A) and the clear half (P1B).

**2. Stimulus.** Section 2.3 with cfg_BPLCON0 = $0008 (P1A), and the schedule
of section 2.5 on the light-pen connector. If neither J12 nor J11 passes P2,
P1A is graded on both runs and fails. P1B is the same program and schedule
with cfg_BPLCON0 = $0000, run on both connectors in every configuration. The
sequence follows p.239: "To enable the
light pen input, write a 1 into bit 3 of BPLCON0." The first graded field
(F1) begins after the write, so the vertical-blank exit at which "the capture
circuitry for the light pen is automatically enabled" (p.238) lies after the
enable.

**3. Observation.** The stored samples:

- W_trig(f) for f = F2 … F7;
- W_blank(f) for f = F2, F9, F10 (after the untriggered fields F1, F8, F9);
- W_live(f) for all f.

**4. Expected.**

**P1A (bit 3 set).**

- In every W_trig(f), every read has the same P. p.239: "Once the light pen
  input is enabled and the light pen issues a trigger signal, the value in
  VPOSR is frozen."
- In every W_blank(f) listed above, every read has the same P. p.239: "If no
  trigger is seen, the counters latch at the end of the display field. It is
  impossible to read the current beam location while the VPOSR register is
  latched."

**P1B (bit 3 clear).** In every W_trig(f) for f = F2 … F7 and in every
W_blank(f) for f = F2, F9, F10, every read equals the model value and
consecutive reads differ. p.216: VHPOSR reads "the counter that is producing
the beam on the screen". p.239 ties both the trigger latch and the end-of-field
latch to the state "Once the light pen input is enabled".

**P1 (aggregate).** P1A holds on the light-pen connector(s) and P1B holds on
both connectors.

"Every" is graded over every read, every window, every configuration and
both cores.

**5. Preconditions.** C0–C6. In addition, each triggered field's W_live holds
at least 2 reads that equal the model value. The register was therefore live
up to the pulse, so the hold that follows starts with this field's pulse and
is not left over from earlier.

**6. Contrasting cases.**

- **For P1A:** cfg_BPLCON0 = $0000, everything else identical. The pages
  predict that every read in W_trig and W_blank equals the model value and
  that consecutive reads differ (the P1B prediction), so P1A's equality checks
  fail. The program finishes. C0, C1 and C3–C6 hold, and C2 records $0000.
- **For P1B:** cfg_BPLCON0 = $0008 stored and declared in the bit-clear run.
  The pages predict a freeze after each trigger, so P1B's checks fail. The
  program finishes and the preconditions hold.

**7. Reference frame.** Sections 2.6 and 2.7. The line numbers of every
window come from the model, and A4 checks the model in every field.

---

## P2. A pulse on pin 6 latches the beam position of that instant

**1. Proposition.** With LPEN set, a pulse on pin 6 (LPENTG*, "Light pen
trigger", registers VPOSR and VHPOSR) of the light-pen connector latches, into
VPOSR/VHPOSR, the vertical and horizontal beam position of the instant of the
pulse.

- Claims: opus:E-lightpen.c3 and sol:E-lightpen.c2 (p.333 text);
  sol:E-lightpen.c21 and opus:E-lightpen.c24 (p.333 figure, "latches V & H
  positions"); opus:E-lightpen.c12 and sol:E-lightpen.c10 (p.333 table row
  6: "LPENTG* Light pen trigger VPOSR, VHPOSR").
- Page: p.333.

**2. Stimulus.** As in P1 (cfg_BPLCON0 = $0008), with the schedule of section
2.5. There are two runs per configuration, J12 and J11.

**3. Observation.** For each triggered field f = F2 … F7, P_f is the value of
the first read in W_trig(f), i.e. the first read after the pulse's rising
edge. t_fall and t_rise are the harness times of the pulse edges, and H(t) is
the model's counter at time t.

**4. Expected.** From p.238: "This trigger signal tells the internal
circuitry to capture and save the current contents of the beam register,
VPOSR. This allows you to determine where the pen was placed by reading the
exact horizontal and vertical value of the counter beam at the instant the
beam passed the light pen." From p.239, the resolution: "Vertical 1 scan line
in non-interlaced mode", "Horizontal 2 low resolution pixels in either high or
low resolution".

- **(a) V is exact.** V(P_f) is the target line: 100 for F2–F4, 101 for F5,
  258 for F6 (VPOSR bit 0 = 1, V7–V0 = $02), and 60 for F7.
- **(b) H is absolute.** H(P_f) ∈ [H(t_fall) − u, H(t_rise) + u], which is
  [h − u, h + 4 + u] for target H h. The page names "the instant the beam
  passed the light pen" but does not say which edge of the pulse marks it, so
  the whole pulse span is allowed. Nothing wider is allowed.
- **(c) H and V are relative.** These checks do not depend on latency.
  - H(P_F3) − H(P_F2) = 1.
  - H(P_F4) − H(P_F2) = 8.
  - H(P_F5) = H(P_F2) and V(P_F5) = V(P_F2) + 1.
  
  These are exact. The pulses have the same shape and the same phase within
  the color clock. So "the exact ... value ... at the instant" leaves no room
  for any difference except the beam's own movement.
- **(d) Port.** In at least one of the runs J12 and J11, (a)–(c) hold in every
  triggered field. The report names that connector and says whether the other
  run latched. Neither is graded (section 1, item 2).

**5. Preconditions.** C0–C6, and u ≤ 2 for check (b). Each target lies inside
W_live territory (line ≥ Rb, except F9, which is not used here). Each target
is also more than 4 + u color clocks away from both line ends. In the same
field, the reads before the pulse equal the model value, so the register was
live when the pulse came.

**6. Contrasting cases.**

- Remove the F2 pulse (C5's log changes with it). The pages predict an
  untriggered F2: the reads after the would-be instant keep equalling the
  model value (p.239: "If both values are not the same, the light pen has not
  triggered since the last top-of-screen (V = 20)"), and F3's W_blank holds a
  value above $10500. Checks (a) and (b) for F2 fail.
- Move the F3 pulse 2 color clocks later. Check (c) now predicts a difference
  of 3, so the check with its unchanged expectation of 1 fails.

The program finishes and the preconditions hold in both runs.

**7. Reference frame.** Section 2.6. H(t_fall) and H(t_rise) come from the
model. A4 checks the model on the live reads of the same field, just before
the pulse.

---

## P3. The latched position stays readable until the end of vertical blanking

**1. Proposition.** After a trigger, VPOSR/VHPOSR show the latched position,
unchanged, from the trigger through the following vertical blanking (where
"Light pen position is usually read"), and return to the moving beam at the
end of internal vertical blanking. A pulse while they are held changes
nothing, and a pulse before the capture circuitry is re-enabled is not
captured.

- Claims: opus:E-lightpen.c5 and sol:E-lightpen.c4 (p.333: "Light pen
  position is usually read during blanking, but MAY be available prior to
  that."), together with opus:E-lightpen.c3 and sol:E-lightpen.c2.
- Page: p.333. The expected values come from p.239 and p.238, because the
  claim's "MAY" gives no value by itself (section 0.3).

**2. Stimulus.** P1's base runs. F7 carries two pulses, and F9 carries a
pulse at line 10.

**3. Observation.**

- W_trig(f) for f = F2 … F7. This window reaches into lines 1–19 of f+1,
  which is the blanking read the claim names.
- W_live(f+1) for the same fields.
- F7's reads after its second pulse.
- F9's W_blank and W_live.

**4. Expected.**

- **(a)** Every read in W_trig(f) has P = P_f: right after the trigger, and in
  lines 1–19 of f+1. p.239: "the value in VPOSR is frozen"; "It is impossible
  to read the current beam location while the VPOSR register is latched."
- **(b)** In F7, the reads after the second pulse (line 180) still have P =
  P_F7, the position of the line-60 pulse. The value is "frozen" until the
  release.
- **(c)** Every read in W_live(f+1) equals the model value, and consecutive
  reads differ. p.239: "This freeze is released at the end of internal
  vertical blanking (vertical position 20)." p.216: VHPOSR then reads "the
  counter that is producing the beam on the screen." Rb follows section 1,
  item 1.
- **(d)** F9, whose pulse at line 10 follows the untriggered F8:
  - every read in W_blank(F9) has one and the same P, the value latched at
    the end of F8, and the line-10 pulse does not change it;
  - every read in W_live(F9) equals the model value.
  
  p.239: "If no trigger is seen, the counters latch at the end of the display
  field." The value of F8's end is held through F9's blanking, and p.238:
  "Just as the system exits vertical blank, the capture circuitry for the
  light pen is automatically enabled." A pulse before that is not captured.

**5. Preconditions.** C0–C6. (c) is graded only where (a) holds: where no
position was held in the preceding fields there is nothing to release, and
(c) is NOT GRADED instead of passing on a running counter. C4's density after
each pulse also applies.

**6. Contrasting cases.**

- **For (a):** remove the F2 pulse. The reads after the would-be instant
  equal the model value (p.239, method step 2), so (a) fails for F2.
- **For (c):** add a pulse at (50, $40) in F8, whose W_live is declared
  untriggered. The pages predict a hold from line 50 (p.239), so the live
  check over W_live(F8) fails.
- **For (d):** move F9's pulse from line 10 to line 50. The pages predict a
  hold from line 50 of F9, so (d)'s live check fails.

The program finishes and the preconditions hold in all three runs.

**7. Reference frame.** Section 2.6. The boundaries at line 20 and at Rb come
from the model.

---

## P4. The chapter-8 determinations tell a triggered field from an untriggered one

**1. Proposition.** The two ways of chapter 8 determine whether the light pen
triggered:

- After a trigger at a line ≤ 260, two long reads agree, and the position
  value is below $10500 and equals the pen position.
- Without a trigger, two reads in the display part differ, and the value held
  during 0 < V < 20 is above $10500.

Claims: opus:E-lightpen.c4 and sol:E-lightpen.c3, second half (p.333: "this
can be determined in the two ways as shown in chapter 8, 'Interface
Hardware.'"). Page: p.333. The expected values come from p.239.

**2. Stimulus.** P1's base runs.

**3. Observation.**

- **Method 1** (p.239, steps 1–4): every pair of consecutive reads in
  W_trig(f) for the triggered fields F2 … F7, and in W_live(f) for the
  untriggered fields F1 and F8.
- **Simplified method** (p.239: "read the register only during the internal
  vertical blanking period of 0<V20"): every read in W_blank(f+1).

The harness pulse log (C5) says which fields are triggered and which are not.
F9 counts as untriggered, because its line-10 pulse is not captured (P3(d)).

**4. Expected.**

- **Triggered fields F2 … F7** (every trigger line is ≤ 260; section 1,
  item 5):
  - every pair in W_trig(f) is equal;
  - P < $10500 and P = P_f from P2;
  - every read in W_blank(f+1) also has P < $10500 and P = P_f.
  
  p.239: "If the value is less, the light pen has triggered and the value
  read is the screen position of the light pen."
- **Untriggered fields F1, F8:** every pair in W_live(f) differs. p.239: "If
  both values are not the same, the light pen has not triggered since the
  last top-of-screen (V = 20)."
- **After the untriggered fields F1, F8, F9:** all reads in W_blank(f+1) are
  equal, and P > $10500. p.239: "If the VPOSR value is greater than $10500,
  the light pen has not triggered since the last top-of-screen."
- **Not graded:** the exact value and moment of the no-trigger latch
  (section 1, item 10).

**5. Preconditions.** C0–C6. The pulse log matches the schedule.

**6. Contrasting cases.**

- Add a pulse at (100, $40) to F1. The pages predict that F1 now reads as
  triggered: the pairs after the pulse are equal and F2's W_blank holds
  P < $10500. The untriggered-branch check for F1 fails.
- Remove F2's pulse. The triggered-branch check for F2 fails.

**7. Reference frame.** Section 2.6.

---

## P5. The pen-pressed button on pin 5 reads back in POTGOR

**1. Proposition.** The light pen's pen-pressed-to-screen button, on pin 5
(LPENPR*) of a controller connector, is readable: with all 0s written to
POTGO, POTGOR bit 8 (left connector) or bit 12 (right connector) follows the
level of that pin.

- Claims: opus:E-gameport.c6 and sol:E-gameport.c5 (p.327: "4. Light pen,
  including pen-pressed-to-screen button.").
- Page: p.327. Supporting pages:
  - p.333: row 5 "LPENPR* Light pen pressed See Proportional Inputs", and the
    figure "PEN PRESS = POT0X".
  - p.228: pin 5 "pen pressed to screen".
  - p.240: the POTGO/POTGOR bits.

**2. Stimulus.**

- **Program:**
  - INTENA ← $7FFF, DMACON ← $7FFF, BPLCON0 ← $0000;
  - a connector-identity loop (for C0): `MOVE.B $BFE001,D0` (CIA-A port A,
    p.232) stored to a ring of words, while the harness holds the left mouse
    button pressed and then releases it;
  - POTGO ← $0000 from its memory word. p.240: "To use the entire register as
    an input, sensing the current state of the pot pins, write all 0s to
    POTGO. Thereafter you can read the current state by using read-only
    address POTINP.";
  - then a loop of `MOVE.L $DFF004,(A0)+ ; MOVE.W $DFF016,(A0)+` for at least
    12 fields, and a done marker.
- **Harness:** all controller pins of both connectors stay high, except that
  pin 5 of connector X is driven low for 700,000 clk_sys cycles, high for
  700,000, low for 700,000 and high for 700,000. The changes are not aligned to
  fields or to a line. The graded samples (P5.3) are chosen by their own V and
  by their time after the change, so the oracle does not depend on the
  alignment. X is the connector the harness's mouse inputs reach, identified
  by C0 (P5.5). The run is performed in every configuration of section 2.1.

**3. Observation.** The graded samples are those that meet both conditions:

- The sample's own VPOSR read gives V in 100–200. The register reads the
  moving beam in these runs, because LPEN = 0. This window avoids the
  per-field capacitor discharge "For the first 7 (NTSC) or 8 (PAL) horizontal
  display lines" (p.331), whose line numbers the pages do not pin down.
- The sample's POTGOR bracket begins at least 300 µs after the latest level
  change of any pot line. p.240: "There are large capacitors on these lines,
  and it can take up to 300 microseconds for the line to change state."

**4. Expected.**

- **The pin-5 bit of connector X follows its level.** Bit 8 for J12 (p.240:
  "08 DATLX data for port 1, pin 5"); bit 12 for J11 (p.240: "12 DATRX data
  for port 2, pin 5"). Port 1 is JOY0DAT, the left connector (p.229, p.281),
  and J12 is the left connector (p.327). In every graded sample the bit is 0
  while pin 5 is low and 1 while pin 5 is high. This rests on p.240's "sensing
  the current state of the pot pins", read with "The state of the data bit
  controls the logic level on the output pin": a data bit of 1 is the high
  logic level.
- **The other pot bits stay at 1.** In every graded sample, bits 10 and 14
  (pin 9 of each connector) and the other connector's pin-5 bit are 1.
- **Both connectors are graded.** p.327 says "The two 9-pin D-type connectors
  ... are used to interface to four types of devices". p.240 provides the pin-5
  bit for each of them. No input of the design that the harness drives reaches
  the pin-5 bit of the connector that C0 does not identify as X, so that
  half is a RECORDED line next to the bits' measured reads.

**5. Preconditions.** Each is evaluated and printed before the lines that
depend on it. When one fails, P5(a) and P5(b) of the affected windows are NOT
GRADED.

- **C1.** The program finished (the done marker is present).
- **C0.** Connector identity. While the identity loop runs, the harness holds
  the left mouse button pressed and then releases it. Exactly one of bit 6 and
  bit 7 of CIA-A port A follows it. p.232: "The left button on the Amiga mouse
  is connected to CIAAPRA ($BFE001). Port 1 uses bit 6 and port 2 uses bit 7."
  Bit 6 makes X = J12 (port 1, the left connector, p.327), bit 7 makes X = J11.
  The pin-5 bit graded is bit 8 for J12 and bit 12 for J11.
- The port shows the CPU reading POTGO's value $0000 from memory before the
  loop.
- **Schedule.** The harness log shows the four level changes of pin 5 (low,
  high, low, high) at their scheduled cycles, each read back as set; every
  controller input is inactive before the first change; and no input other
  than pin 5 and pin 9 of X changes. Pin 5 and pin 9 are the two lines of X
  that P5(a) and P5(b) examine.
- Each level interval contains at least 50 graded samples.

**6. Contrasting cases.**

- **Main check:** in the second low interval, drive pin 9 of connector X low
  instead of pin 5. p.240 predicts that bit 10 or 14 (DATLY/DATRY, "pin 9")
  becomes 0 and the pin-5 bit stays 1, so the pin-5 check (P5(a)) and the
  pin-9 check (P5(b)) fail in that interval.
- **C0:** the harness holds the right mouse button instead of the left one in
  the identity loop. Neither CIA-A bit follows, and the precondition fails.
- **C1:** remove the done marker. The precondition fails.
- **POTGO precondition:** store $0001 in POTGO's memory word. The recorded
  value differs from the declared $0000, so the precondition fails.
- **Schedule:** drive an input of the other connector during the second level
  interval. The precondition fails.
- **Sample-count precondition:** shorten one level interval to less than
  300 µs. The interval then has no graded sample, so the precondition fails.

The program finishes in every one of these runs.

**7. Reference frame.** The line of each sample comes from its own live
VPOSR read (p.216). The connector's identity comes from C0 (P5.5).

---

## 3. Runs

This table summarizes the runs above; it adds nothing to them.

| run | propositions | configurations |
|---|---|---|
| base, J12 and J11 (cfg_BPLCON0 = $0008) | P1–P4 | all (section 2.1) |
| cfg_BPLCON0 = $0000, both connectors | P1B (graded) | all (section 2.1) |
| cfg_BPLCON0 = $0000 in the base run; $0008 in the bit-clear run | P1A, P1B contrasts | at least ECS/PAL, both cores |
| F2 pulse removed; F3 pulse +2 color clocks | P2, P3(a), P4 contrasts | at least ECS/PAL, both cores |
| extra pulse (100, $40) in F1; extra pulse (50, $40) in F8; F9 pulse moved to line 50 | P3(c), P3(d), P4 contrasts | at least ECS/PAL, both cores |
| BEAMCON0 bit 13 = 1 | C6 contrast | ECS/PAL, both cores |
| NTSC selected with PAL declared; VPOSW flip inside S5; VHPOSW write inside S5; loop delayed; S8 removed; pin 3 instead of pin 4 in S2 | C3 (A3, A1), A2, C4, C1, C0 contrasts | ECS/PAL, both cores |
| other standard declared; R declared too large | A2, A3, A1 oracle-side changes | ECS/PAL, both cores |
| pen-pressed, X the connector C0 identifies | P5 | all |
| pin 9 instead of pin 5; right button in the identity loop; no done marker; POTGO word $0001; another input driven; short interval | P5 contrasts | at least ECS/PAL, both cores |

Contrasts listed in the same row change fields that do not overlap, so they
may share one run. Each contrast must still be reported red on its own.

---

## 4. Build notes

### 4.1 Scope built

P1 to P5 and the preconditions C0 to C6 are built, and A2 is a graded line.
The section 0 claims are named on RECORDED lines and graded nowhere.

What is not graded, and why:

- **P2(b), the absolute horizontal position.** Its precondition (u <= 2,
  sections 2.6 and 2.8) does not hold. The port shows no custom-register
  access, so each long read is bracketed by the last instruction fetch before
  it and the first store after it, which are 6 colour clocks apart (3 bus
  cycles) on both cores. The anchor then stays uncertain by u = 7 colour clocks
  (4.5). Every run prints the horizontal offset of each P_f as a MEASURED line
  and a RECORDED line says that P2(b) is NOT GRADED; (a), (c) and (d) are
  graded.
- **Claim ids.** opus:E-lightpen.c24 ("PORT 0") and opus:E-lightpen.c4,
  sol:E-lightpen.c3 ("There is no hardware bit to indicate this trigger") each
  bundle a clause that nothing here grades (section 0.3), so they sit on
  RECORDED lines only. opus:E-lightpen.c3 and sol:E-lightpen.c2 ("a trigger
  pulse is generated which can be enabled to latch the horizontal and vertical
  beam positions") are established by P1 and P2 together, so they sit on the
  aggregate line LP-LATCH-ENABLE-AND-POSITION, which requires both.
  opus:E-lightpen.c21 and sol:E-lightpen.c18 ("light pen enable", bit 3 of
  BPLCON0) are established by both halves of P1, so they sit on the aggregate
  line LP-P1-LATCH-ENABLE, which requires P1A and P1B.
  opus:E-gameport.c6 and sol:E-gameport.c5 sit on the RECORDED line of P5's
  second connector only (below): the graded P5 lines examine one connector.
- **A4 population.** A4 is asserted on the reads that no proposition grades
  by itself: F2 before its pulse, and the count of at least two current reads
  before the pulse in each triggered field. The live windows of F1, F3 to F8
  and F9 are graded by P4, P3(c) and P3(d); an anchor that depended on the
  clause it anchors could not turn that clause red.
- **A4 on the repository's chipset.** There the registers read the moving
  beam before every pulse, so a change of the pulse schedule cannot make A4
  red (measured: the F2 pulse at line 40 leaves A4 green on both cores). A4's
  contrasts are therefore a VPOSW write in F2 before the pulse, which flips V8
  of the vertical counter for 100 reads and flips it back (repository
  chipset), and the pulse moved to line 40 (private chipset of 4.6).
- **A2 and NTSC.** A2 (p.23: "Every other line is a long line (228 color
  clocks, 0-$E3), with the others being 227 color clocks long") fails in every
  NTSC configuration: the field length of the reference phase (4.5) divides
  into 263 lines of 227 colour clocks and into no alternating pattern. A2 is
  not part of C3 (section 2.6), so the light-pen lines are graded in the four
  NTSC runs as in the PAL runs. A2 is the only line of 4.5's table whose
  alternating-line branch has no green control: the chipsets of 4.6 do not
  produce alternating lines. Its all-227 branch is the one that passes in the
  PAL runs.
- **P3(c) with nothing held.** P3(c) (release) is graded only where P3(a)
  holds; where no position was held in the preceding fields it prints NOT
  GRADED instead of passing for lack of anything to release.

P5 is graded in every configuration. The connector comes from C0 (P5.5): the
harness's mouse inputs reach port 1, J12 (bit 6 of CIA-A port A follows the
left button), so the pin-5 bit graded is POTGOR bit 8 and the pin-9 bit is bit
10. Bits 12 and 14 (J11) are examined by P5(b) and stay 1. The second
connector's pin-5 half is a RECORDED line: nothing this harness drives changes
bit 12, and driving the mouse input that moves bit 8 while sampling bit 12
leaves it at a constant 1 (the run's own MEASURED line). The pin-9 contrast
drives `joystick1` bit 5, which moves bit 10.

### 4.2 Configurations run

P5 and the light pen: every configuration of section 2.1 (ECS and OCS, PAL
and NTSC). P5 selects them through `chipset_config` (bit 3 for ECS, bit 1 for
NTSC). For the light pen, each configuration runs the pulse schedule on both
harness connectors (`joystick0` and `joystick1` of `nanomig.v`), once with
BPLCON0 bit 3 set and once with it clear. The connector that is J12 or J11 is
read from C0 in each run, not assumed. S3 writes BEAMCON0 with $0020 (PAL) or
$0000 (NTSC) in the ECS runs and is left out of the OCS program. The
contrasting runs use ECS, PAL and `joystick1`, beside the run with bit 3
clear.

### 4.3 Programs and harness

- `prog_pot.s` and `prog_pot_nodone.s` are the P5 programs; `prog_pot_fix.s`
  stores a different first fixture constant. The program samples CIA-A port
  A (`MOVE.B $BFE001,D0`, 3000 iterations) while the harness presses and
  releases the left mouse button, then reads POTGO's value from its memory
  word and samples VPOSR:VHPOSR and POTGOR 10000 times.
- `prog_lpen.s` is S1 to S8 of section 2.3: S2 reads JOY0DAT and JOY1DAT
  (3000 iterations), S5 is 20000 reads and S7 is 46000
  reads of `MOVE.L $DFF004,(A0)+` looped with DBF, stored from $100000 on. Each
  configuration value is read from its memory word ($0C0010 BEAMCON0, $0C0012
  BPLCON0 for S4, $0C0014 BPLCON0 for S6) so that the port shows the read. A
  marker word ($0C0024) is stored between S5 and S6. `prog_lpen_ocs.s` leaves
  out S3, `prog_lpen_nodone.s` leaves out S8, `prog_lpen_fix.s` stores a
  different first fixture constant, `prog_lpen_slow.s` puts a delay of about
  3.3 lines in each loop body and uses 600 and 2000 reads, `prog_lpen_v8.s`
  flips V8 of the vertical counter (VPOSW) after 3000 reads of S5 for 100 reads
  and flips it back, `prog_lpen_m8.s` flips V8 for 100 reads of S7, after 4600
  of them, which lie in F2 before its pulse, and `prog_lpen_hw.s` writes a
  horizontal count of $F0 (VHPOSW, with the current vertical count) after
  19000 reads of S5.
- The harness waits for the first fixture store (the first write to the
  marker word, whatever it stores; the fixture line grades the value), drives
  pin 4 (or pin 3 in the contrast) of the connector under test for 400000
  cycles (S2), and waits for the phase marker, then for the S5 marker. It fits
  the model (4.4) to the S5 reads, places F0 in the field of the S6 write,
  schedules every pulse from the model, applies each pin change at its exact
  cycle and reads the pin back, and runs to the done mark.
- A sample's bracket is the cycle of the last fetch from the program's own
  code before its first store and the cycle of that store.

### 4.4 The beam model

The model has a field of N lines with a pattern of line lengths in colour
clocks and a start instant T0. Its field length is the median of the video
port's field lengths in the reference phase (the first, partial field is
dropped). Of the hypotheses whose field length equals it (all lines 227, 227
then alternating, 228 then alternating, all lines 228) it keeps the first under
which every reference word read is current, where a word is current if it equals
the model counter at some instant of its bracket. T0 is fitted by interval
intersection: each read bounds T0 to an interval, the model keeps the largest
set of reads whose intervals overlap, and u is the width of that overlap in
colour clocks, rounded up.

The field length is read from the vertical sync of the video port, which
deviates from section 2.1's statement that the video output is not used: the
video port is used for this one number. The reads cross-check it. A1 requires
every reference word read to equal the model counter, so a field length that
disagrees with the reads cannot pass A1.

The model carries one parameter the specification does not list: the number of
colour clocks by which the vertical count trails the horizontal wrap (4.5). The
pages do not tie the two counters to one instant. A read at the first
horizontal counts of a line can show the previous line's vertical count; with a
lag of zero the model has no instant at which that pair appears, and A1 fails
for those reads. The lag is the smallest of 0, 1 and 2 under which A1 holds. It
moves no window boundary by more than that many colour clocks and no pulse
target, which all sit at H = $40 and later.

The windows of section 2.7 use the model's line boundaries. F0 is the field
that contains the S6 configuration read. Pulses fall half a colour clock after
the model instant of their target and last 4 colour clocks. Every pulse is
driven on pin 6 (bit 4 of the joystick byte).

### 4.5 Measured values

Both cores, every light-pen run (the two connectors agree):

| | fx68k | TG68K |
|---|---|---|
| lines per field, PAL / NTSC | 313 / 263 | 313 / 263 |
| line pattern | all 227 colour clocks | all 227 colour clocks |
| reference field length, PAL / NTSC (clk_sys) | 568408 / 477608 | 568408 / 477608 |
| lag of the vertical count behind the horizontal wrap | 1 colour clock | 2 colour clocks |
| anchor uncertainty u | 7 colour clocks | 7 colour clocks |
| largest H read | 225 | 226 |
| largest gap between consecutive reads | 20 colour clocks (PAL, NTSC) | 21 colour clocks (PAL, NTSC) |

Outcome of the normal run on the repository's chipset. fx68k and TG68K print
the same verdict lines: 230 passed, 56 failed on each.

- P5, in each of the four configurations: C1, C0, the POTGO read, the
  schedule, the four window counts, P5(a) and P5(b) pass. Bit 6 of CIA-A port
  A follows the left button (J12 is identified), POTGOR bit 8 follows pin 5,
  and bits 10, 12 and 14 stay 1. Bit 12 reads 1 in every sample while the run
  drives the input that moves bit 8, and the RECORDED line for J11's pin 5
  says so.
- The fixture and C0, C1, C2, A1, A3, A4, C4, C5 hold in all eight light-pen
  runs and all eight runs with bit 3 clear, on both cores; C6 holds in the
  eight ECS runs and is not applicable in the eight OCS runs. A2 holds in the
  four PAL runs and fails in the four NTSC runs.
- P1B passes in all eight runs with bit 3 clear.
- PAL and NTSC runs (ECS and OCS, both connectors). The first read after each
  pulse reads the moving beam, 7 to 24 colour clocks beyond the pulse position
  (the MEASURED lines give each offset), and no read is held. P2 fails (no
  connector latches), and P1A, P3(a), P3(b), P3(d) and P4 fail on both
  connectors, as do the P1 aggregate and LP-LATCH-ENABLE-AND-POSITION. P3(c) is
  NOT GRADED because nothing was held. The 56 failures are A2 in the four NTSC
  runs (4), P2 (4), P1A (8), P3(a) (8), P3(b) (8), P3(d) (8), P4 (8), the P1
  aggregate (4) and LP-LATCH-ENABLE-AND-POSITION (4).

Contrasting runs, `NMH_MUTATE` selects the change. The P5 and fixture rows run
in ECS, PAL; the light-pen rows run in ECS, PAL on `joystick1`, beside the run
with bit 3 clear. Every run below printed FAIL on the named line on fx68k and on
TG68K; the repository rows are on the repository's chipset.

| line | `NMH_MUTATE` | chipset | change | result (both cores) |
|---|---|---|---|---|
| FIXTURE-LOAD | FIX-LOAD | repository | a program that does not exist is loaded | red, FIXTURE-MARK0 NOT GRADED |
| FIXTURE-MARK0 | FIX-MARK0 | repository | first store of the fixture holds $BEEF | red, only this line |
| P5-C1-DONE | C1 | repository | no done mark | red, P5(a) and P5(b) NOT GRADED |
| P5-C0-IDENTITY | C0 | repository | right button held in the identity loop | red, P5(a) and P5(b) NOT GRADED |
| P5-POTGO-READ | P5-POTGO | repository | POTGO word $0001 | red, P5(a) and P5(b) NOT GRADED |
| P5-SCHEDULE | P5-SCHED | repository | joystick0 bit 0 driven in the second level interval | red, P5(a) and P5(b) NOT GRADED |
| P5-WINDOW-COUNT | P5-SHORT | repository | third level interval 5000 cycles | red for window 2; P5(a) and P5(b) of window 2 NOT GRADED |
| P5A-PIN5-FOLLOWS, P5B-OTHER-POT-BITS-1 | P5-PIN9 | repository | pin 9 driven instead of pin 5 in window 2 | red for window 2 on both lines |
| LP-FIXTURE-MARK0 | LP-FIX | repository | first fixture store holds $BEEF | red, only this precondition line; the proposition lines are NOT GRADED |
| LP-C3-A1-READS-CURRENT | LP-A1W | repository | V8 of the vertical counter flipped for 100 reads of S5 | red, only this line |
| LP-A2-LINE-LENGTHS | LP-A2 | repository | H written above $E2 inside S5 | red, with LP-C3-A1-READS-CURRENT and LP-C3-A4-FIELD-ANCHOR |
| LP-C3-A3-FIELD-LENGTH | LP-A3 | repository | NTSC selected, PAL declared | red, only this line |


### 4.6 Controls on a chipset that follows the pages

On the repository's chipset the registers read the moving beam in every light-pen
run, so every light-pen line that grades the latch is red there, and a stimulus
change cannot make a red line redder. Each of these lines has a control that
runs its comparator on a live input from a different mechanism: a private copy
of the chipset sources, outside the repository, that contains a latch following
p.238 and p.239. In it BPLCON0 bit 3 enables the capture, a falling edge on pin
6 of either connector latches the beam position, the latch holds until the start
of line 20 and is re-enabled then, and a field that ends without a trigger
latches at its end. The package's own run on that copy passes every light-pen
line in the PAL and in the NTSC runs (4.5), and the stimulus contrasts of the
table below run on it.

P1B is green on the repository's chipset, whose registers always read the
moving beam, and on the private chipset, where bit 3 gates the latch. A second
private copy, in which the latch ignores bit 3, shows that P1B can fail on a
chipset that latches whatever bit 3 holds (4.5).

### 4.7 Timing margins tuned on this design

Two spin counts and one interval length were tuned on this design rather than
taken from the page, each guarded by a precondition the run checks:

- The chipset's own internal reset holds the design for a fixed span
  (measured at about 2.27 million clk_sys cycles, close to four video
  fields) after `cpu_nrst_out` releases, before the CPU's first bus access
  is visible on the port. The program's own first store (a fixed marker,
  ahead of anything graded) is used to synchronise the run to this,
  instead of assuming the span holds exactly; precondition C1 (the
  program reaches its done mark) guards the case where it does not.
- The connector-identity loop's and the sampling loop's own iteration
  counts are sized so each loop keeps running for the whole span the
  harness drives against it (the identity pin's active/released halves;
  the four pen-pressed level intervals in turn). Precondition C1 guards
  a loop that runs out early.
- Each of the four level intervals is 700'000 clk_sys cycles: long enough
  that the free-running beam (LPEN=0 throughout this program, so
  VPOSR/VHPOSR always read the moving beam, never a latch) is guaranteed
  to cross V=100-200 at least once inside it, on both video standards.
  The per-window "at least 50 graded samples" precondition guards a
  window that misses the crossing.
- The light-pen program reads 20000 reference and 46000 measurement samples.
  The reference phase covers four complete PAL fields and five NTSC fields
  (section 2.6 needs three), and the measurement phase reaches line 100 of F10;
  C1 guards the sample count and C4 guards the coverage.
