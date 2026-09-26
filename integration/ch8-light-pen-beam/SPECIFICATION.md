# Light pen latch and beam position (chapter 8): test specification

This document specifies the integration test package for the light-pen claims
of chapter 8 of the *Amiga Hardware Reference Manual*, 3rd edition (printed
pages 227-229 and 238-239). It was written from the package's claims
(`claims-spec-stage.jsonl`) and the manual's pages alone, by an author who
never saw the implementation or any test. Page numbers are printed page
numbers. Where the printed page and its text layer differ, quotations follow
the printed page.

---

## 1. Claims not turned into a proposition

| Claim | Page | Reason |
|---|---|---|
| opus:8-lpen.c53 | 239 | "the VPOSR register may be latched at any time" describes something that can happen, not something that must. It names no moment and no value, so it gives no expected value. The latches the pages do define are all tested: pen captures (P3, P5) and the end-of-field latch (P7). |
| sol:8-pen.c47 | 239 | Same sentence, same reason. |
| opus:8-lpen.c54 | 239 | "cannot be relied on as a counter" is advice to software. No observation can contradict it. The behaviour behind the warning is tested in P5 to P8. |
| sol:8-pen.c48 | 239 | Same sentence, same reason. |
| opus:8-lpen.c41 | 239 | "There is no single bit in the system that indicates a light pen trigger" says that something does not exist anywhere in the system. A test can read only the custom registers of Appendix B, not the whole system (the CIAs, for example, are outside it). The sentence names no register and no value. Comparing the custom registers could never establish "no bit in the system". Its practical consequence, that a trigger can only be found by reading VPOSR, is what P11 and P12 test. |
| sol:8-pen.c40 | 239 | Same sentence, same reason. |
| opus:8-lpen.c34 | 239 | Only names bit 3 "LPEN", and a name cannot be observed. Also, the quoted "(LPEN)" is not on the printed page 239, which reads "To enable the light pen input, write a 1 into bit 3 of BPLCON0." The name is printed on p.89 ("Bit 3 - LPEN (light pen enable)") and p.271 ("LPEN =Light pen enable"). |

One claim is used only in part. opus:8-intro.c2 (p.227, "Two multiple purpose
mouse/joystick/light pen control ports") reads, as its `says` puts it, that
*each* port serves a light pen. p.238 ties the light pen to one port per model.
P1 therefore grades only "at least one port" (see 10.4).

## 2. Propositions and the claims they rest on

| Proposition | Claims | Pages |
|---|---|---|
| P1 A light pen is accepted on one of the controller ports | opus:8-lpen.c1, sol:8-pen.c1, opus:8-intro.c12, sol:8-overview.c6, opus:8-intro.c2 (in part) | 238, 227 |
| P2 Pin 6 is the beam-trigger line; the other signal pins are not | opus:8-lpen.c16, sol:8-pen.c17, opus:8-tbl8-1.c20, sol:8-pin.c22 | 238, 228 |
| P3 A trigger captures the beam position exactly at its instant | opus:8-lpen.c17, opus:8-lpen.c18, sol:8-pen.c18, sol:8-pen.c19, sol:8-pen.c20 | 238 |
| P4 VPOSR holds the high-order bit, VHPOSR the low-order bits of the pen position | opus:8-regs.c18, opus:8-regs.c20, sol:8-reg.c20, sol:8-reg.c22 | 229 |
| P5 After a trigger, VPOSR is frozen until the release | opus:8-lpen.c36, sol:8-pen.c36 | 239 |
| P6 While latched, VPOSR never shows the current beam position | opus:8-lpen.c38, sol:8-pen.c38 | 239 |
| P7 Without a trigger, the counters latch at the end of the field | opus:8-lpen.c37, sol:8-pen.c37 | 239 |
| P8 The freeze is released at the end of internal vertical blanking | opus:8-lpen.c39, sol:8-pen.c39 | 239 |
| P9 The capture circuitry re-arms by itself as vertical blank ends | opus:8-lpen.c12, sol:8-pen.c12 | 238 |
| P10 Bit 3 of BPLCON0 enables the light pen input and latch | opus:8-lpen.c35, sol:8-pen.c35, opus:8-regs.c16, sol:8-reg.c18 | 239, 229 |
| P11 The two-read detection method gives the right answer | opus:8-lpen.c43, sol:8-pen.c41, opus:8-lpen.c47, sol:8-pen.c44, opus:8-lpen.c48, sol:8-pen.c45, sol:8-pen.c46 | 239 |
| P12 The single read during vertical blanking gives the right answer | opus:8-lpen.c52 | 239 |
| P13 The pen-pressed switch on pin 5 reads like the middle mouse button | opus:8-lpen.c10, opus:8-lpen.c11, sol:8-pen.c10, sol:8-pen.c11 | 238 |

sol:8-pen.c45 and sol:8-pen.c46 quote the sentence "If the value is less, the
light pen has triggered and the value read is the screen position of the light
pen". It is printed twice on p.239, once in each method. They are graded in
P11, and P12 grades the same sentence in the second method.

## 3. System, runs and inputs

- **Configurations.** Every main run is made in OCS/PAL, OCS/NTSC, ECS/PAL and
  ECS/NTSC, each with both CPU cores (eight main runs), with 2 MB of Chip
  memory. Each contrasting run of §8 is made in ECS/PAL with both CPU cores.
  K1 and K2 are also made in OCS/NTSC, because the release lines and the
  threshold comparison depend on the standard.
- **ECS.** The program never writes BEAMCON0. On p.305, bit 13 is
  "LPENDIS    Ignore latched pen value on vertical pos read", and the page adds
  "Do not write to this register." The pages give no reset value for LPENDIS.
  The ECS expectation that the latch can be seen without touching BEAMCON0
  rests on p.295: "These chip revisions support advanced features in addition
  to all of the standard features previously available" and "plug-compatible
  replacements". Suppose an ECS run shows no latch at all while the OCS run of
  the same standard does. The report must then name LPENDIS as the open point,
  alongside the red result.
- **Idle inputs.** Apart from the changes listed in §6, every input stays
  released throughout. On both controller ports, pins 1-4, 5, 6 and 9 are at
  the level of an open switch (high). Mouse counts do not change, no keyboard
  code is sent, and serial RxD is idle.
- **Pin 6 is only driven by the harness.** p.229 lists "CIAAPRA ($BFE001) Input
  and output for pin 6", and p.241 says the fire buttons "can also be
  configured as outputs" through CIAADDRA. The program never touches the CIA
  registers, which are not in Appendix B. Pin 6 therefore stays an input that
  only the harness drives.
- **Registers the program never writes:** BEAMCON0, VPOSW, VHPOSW, DIWHIGH,
  COP1LC/COP2LC, CIA registers. The only exceptions are the contrasting runs
  that name one of them.

## 4. Program

The program runs in supervisor mode from Chip memory. Every register write is a
whole word, as p.89 requires ("Bits in this register cannot be independently
set").

```
        move.w  #$2700,sr               ; CPU ignores all interrupt levels
        lea     $dff000,a6
        move.w  #$7fff,$09a(a6)         ; INTENA: clear every enable bit (bit 15 = 0 clears, p.218)
        move.w  #$7fff,$09c(a6)         ; INTREQ: clear every request
        move.w  #$7fff,$096(a6)         ; DMACON: clear every DMA enable (p.278): no Copper, bitplane,
                                        ;   sprite, audio, disk or blitter DMA; only refresh remains
        move.w  #$0000,$100(a6)         ; BPLCON0: BPU=0, HIRES=0, LACE=0 (non-interlaced), ERSY=0, LPEN=0
        move.w  #M0,MARK
; ---- phase A: beam counter, light pen off
        lea     BUFA,a0
        lea     BUFA_END,a1
.a      move.l  $dff004,(a0)+           ; VPOSR:VHPOSR as one long word (p.238)
        cmpa.l  a1,a0
        bne.s   .a
        bsr     wait150
        move.w  #$0008,$100(a6)         ; BPLCON0: the same word with bit 3 = 1 (p.239)
        move.w  #M1,MARK
; ---- phase B: light pen on
        lea     BUFB,a0
        lea     BUFB_END,a1
.b      move.l  $dff004,(a0)+
        cmpa.l  a1,a0
        bne.s   .b
        bsr     wait150
        move.w  #$0000,$100(a6)         ; BPLCON0: bit 3 = 0 again
        move.w  #M2,MARK
; ---- phase C: light pen off again
        lea     BUFC,a0
        lea     BUFC_END,a1
.c      move.l  $dff004,(a0)+
        cmpa.l  a1,a0
        bne.s   .c
        move.w  #$ff00,$034(a6)         ; POTGO: OUTRY DATRY OUTRX DATRX OUTLY DATLY OUTLX DATLX = 1,
                                        ;   bits 7-1 = 0, START = 0 (p.240, p.241, p.236)
        move.w  #M3,MARK
; ---- phase D: pot pins as buttons
        lea     BUFD,a0
        lea     BUFD_END,a1
.d      move.w  $dff016,(a0)+           ; POTGOR / POTINP (p.240)
        cmpa.l  a1,a0
        bne.s   .d
        move.w  #M4,MARK
.stop   bra.s   .stop

wait150 move.l  $dff004,d0              ; wait until a read shows vertical position 150
        lsr.l   #8,d0
        andi.w  #$01ff,d0
        cmpi.w  #150,d0
        bne.s   wait150
        rts
```

- `MARK` is one word address in Chip memory. The marker writes M0 to M4 (five
  distinct values) are visible at the Chip-memory port. They bracket the
  BPLCON0 writes and mark where each phase starts. The buffers must not overlap
  the program. The builder sizes them from the sample rate measured in phase A,
  with at least 10 % spare, so that CP5 holds: phase A at least 4.5 fields,
  phase B up to line 40 of field B21, phase C up to line 40 of field C3, and
  phase D at least 12 ms.
- Within phase B the program writes no custom register. It only reads $DFF004
  and $DFF006 and writes Chip memory. P9 depends on this.
- `wait150` runs in phase A, where the counter runs. At the end of phase B it
  runs after field B20, when no input is changed, so the value it waits for is
  a running one.
- The sequences the manual prescribes are followed and cited in the comments.
  Whole-word writes to BPLCON0 (p.89). The SET/CLR convention of INTENA,
  INTREQ and DMACON (p.218, p.278). POTGO written at $DFF034 and read at
  $DFF016 (p.240: "This register must be written to at the POTGO address, and
  read from the POTINP address"). Output enable and data both set to 1 to read
  a button (p.241). START left 0 (p.236: "All counts are reset to zero when
  POTGO is written with bit zero high").

## 5. Reference frame

### 5.1 Positions and values

- A **sample** is one `move.l $dff004,(a0)+`. The 68000 reads $DFF004 (VPOSR)
  and then $DFF006 (VHPOSR). p.238: "The software can refer to this register
  set as a long word whose address is VPOSR."
- A **position** is (V, H). V is nine bits: VPOSR bit 0 (V8) and VHPOSR bits
  15-8 (V7-V0). H is VHPOSR bits 7-0 (H8-H1). These are the bit assignments of
  p.238.
- One H unit is one colour clock. p.285: "RESOLUTION = 1/160 of screen width
  (280 ns)". p.194: "A memory cycle is approximately 280 ns in duration."
- The **masked value** is m = (VPOSR:VHPOSR) AND $0001FFFF = 256*V + H. This is
  the quantity p.239 compares with $10500, which is 256*261.

### 5.2 Colour-clock grid and read instant

- **Colour-clock grid.** Accesses at the Chip-memory port take memory cycles.
  Their times, in system clock cycles, give the period and phase of the colour
  clock.
- **Read instant of a sample.** The trace may show the CPU's read of $DFF006.
  If it does, that read's time is the read instant: a = b = that time.
  Otherwise the two register reads of the instruction fall in the gap between
  two visible CPU accesses of that instruction. The earlier bound is the
  fetch that precedes the operand reads. The later bound is the first CPU
  access after them (the prefetch, or the first write of the stored long word).
  Take [a, b] as that gap. The builder derives the bus-cycle order from the
  CPU core and confirms it from the trace: the gap must be as long as at least
  two bus cycles.
- **Model uncertainty.** u is the largest b - a in phase A, in colour clocks.

### 5.3 The beam model

The model is fitted on the samples of phase A, when bit 3 is clear and
VPOSR/VHPOSR read the beam counter. p.216 describes VHPOSR as "Read vertical
and horizontal position of the counter that is producing the beam on the
screen". p.238 says "The light pen register is at the same address as the
beam counters".

- The model gives a position beam(t) for any time t of the run. Within a line,
  H advances by one per colour clock.
- Line lengths (p.194 gives 227.5 colour clocks, so lengths may alternate),
  the last line L of each field and the phase all come from the fit.
- The pattern of line and field lengths must repeat with a period of one or
  two fields.
- Because the read instant is only known within [a, b], beam(t) is an
  interval of width u.
- H_ref(t) is the model computed with every phase-A read instant taken at b.
  It is used only for differences (P3 c).
- A sample is **current** when its position lies in beam(t) for some t in its
  [a, b].

**Torn samples.** A sample whose two register reads straddle a change of V8 can
combine V8 of one line with V7-V0 of another. These changes happen at model
line 255 to 256 and at L to 0. Samples whose [a - 2, b] (in colour clocks)
contains such a change are exempt from every check.

### 5.4 Anchors

- **A1.** Every phase-A sample that is not torn is current.
- **A2.** Phase A covers at least two full periods of the model (at least four
  fields). Within a field V never decreases, and V returns to 0 exactly once
  per field. u is at most 6 colour clocks.
- **A3 (refresh).** In every model line of phases A, B and C there is one group
  of four refresh accesses at the Chip-memory port, at the same model H in
  every line. p.194 lists "4 cycles for memory refresh" per line, and
  Figure 6-9 on p.195 draws the refresh slots at a fixed place at the start of
  every line. The figure says of itself that it "has been 'adjusted'", so
  only the constancy is used, not its clock numbers.
- **A4 (video).** In phases A, B and C, one vertical-sync pulse begins in each
  model field, at the same model position in every field.

A1 and A2 anchor the numbering. The page's "vertical position 20" and
"V = 261" are values of this counter. A3 and A4 show, independently of VPOSR,
that the beam keeps its timing in phases B and C. In those phases VPOSR is
latched for long stretches and cannot serve as its own reference.

### 5.5 Fields, lines and windows

- **Fields.** A field boundary is where the model's V returns to 0.
  - B0 is the field in which the phase-B BPLCON0 write falls (at line 150,
    between the last read of `wait150` and M1). B1, B2 and so on are the
    fields after it.
  - C0 is the field of the phase-C write. C1, C2 and C3 follow it.
  - L_k is the last line of field k.
- **Vrun** is 21 in NTSC and 29 in PAL. Every page puts the end of vertical
  blanking before this line (10.1).
- **VB(k):** samples of field k whose [a, b] lies within lines 0 to 19.
- **RUN(k):** samples of field k whose [a, b] lies within lines Vrun to
  L_k - 1 and ends before the onset of the first pin-6 pulse of field k at
  line 32 or later, as the **main-run** schedule places it. Pulses in lines 10
  and 19, and pulses on other pins, do not end RUN.
- **Pulse times.** Each scheduled input change has an onset t_on. A pin-6 pulse
  has an end t_off = t_on + 4 colour clocks.
- **POST(p):** for a pulse p in field k, the samples with a > t_off, up to the
  last sample of VB(k+1).
- A **graded pin-6 pulse** is a pin-6 pulse at line 32 or later in fields B4
  to B18. In B4 and B5 it counts only if it is captured (P1 decides that). In
  B14 only the first pulse counts.

### 5.6 Pulses

- **Shape.** A pulse drives the line low from its released high level for
  4 colour clocks and then releases it. Every onset sits at the same phase of
  the colour-clock grid, so onsets differ by whole colour clocks.
- **Placement.** The schedule gives a pulse's position as the model position
  H_ref(t_on) and the model line at t_on.
- **Scheduling.** The harness computes pulse times from the model of the same
  configuration and CPU core. It may take the model from a first run without
  input changes. The system is deterministic, and nothing the inputs do changes
  the program's timing before the pulse. CP4 checks the placement in the
  graded run.
- **Why this shape.** The light-pen line is named LPENTG* on p.333 and LP* on
  p.381, active low in the manual's usage (p.219: "INT6* becomes a logic
  0"). A low pulse contains both a falling and a rising edge, so the capture
  window [t_on, t_off] covers either edge (10.5).

## 6. Input schedule

Line and H are model values. Each pulse is 4 colour clocks at the fixed grid
phase. "Both" means the same pin on port 1 and port 2 at the same time.

| Field | BPLCON0 bit 3 | Input changes (everything else released) |
|---|---|---|
| A (at least 4.5 fields) | 0 | none |
| B0 | 1 from line 150 | none (not graded) |
| B1, B2, B3 | 1 | none |
| B4 | 1 | pin 6, port 1 only, at (100, 100) |
| B5 | 1 | pin 6, port 2 only, at (100, 100) |
| B6, B7, B8, B9, B10 | 1 | pin 6 both, at line 100, H = 101, 102, 103, 105, 108 respectively |
| B11 | 1 | pin 6 both at (40, 100) |
| B12 | 1 | pin 6 both at (200, 100) |
| B13 | 1 | pin 6 both at (258, 100) |
| B14 | 1 | pin 6 both at (80, 100), then pin 6 both at (150, 100) |
| B15 | 1 | pin 6 both at (10, 100), then pin 6 both at (32, 100) |
| B16 | 1 | none |
| B17 | 1 | pin 6 both at (19, 100), then pin 6 both at (32, 100) |
| B18 | 1 | pins 1, 2, 3, 4, 5, 9 both, at lines 40, 60, 80, 100, 120, 140 respectively (H = 100); then pin 6 both at (160, 100) |
| B19 | 1 | pin 6 both at (280, 100) in PAL, (261, 100) in NTSC. **Observation only**: recorded and reported, not graded (10.2) |
| B20, B21, until C0 | 1 | none |
| C0 | 0 from line 150 | none (not graded) |
| C1 | 0 | pin 6 both at (100, 100) |
| C2, C3 | 0 | none |
| D | (POTGO = $FF00) | times from M3: 2-4 ms port 1 pin 5 low; 5-7 ms port 2 pin 5 low; 8-10 ms port 1 pin 9 and port 2 pin 9 low; released otherwise, up to 12 ms |

## 7. Common preconditions

Each precondition must hold before any proposition that depends on it is
graded. If one fails, the dependent propositions are reported as not
evaluated, not as green.

| Id | Precondition | Contrasting run |
|---|---|---|
| CP1 | The beam model is anchored on phase A: A1 and A2 of 5.4 hold. | K4 |
| CP2 | Sampling is dense. In phases A to C, consecutive read instants are at most 56 colour clocks apart (a quarter line), and every line of every graded window holds at least two samples. In phase D, consecutive POTGOR samples are at most 50 microseconds apart. | K5 |
| CP3 | No stray trigger reaches the latch from the idle inputs. Every sample of RUN(B1) is current. | K11 |
| CP4 | Inputs are delivered as scheduled. Each input change in the schedule of the run being made took place, by the harness's own record, at its scheduled model line and H (exactly, at the fixed grid phase), and each pulse lasted 4 colour clocks. No other input changed in phases A to D. For a contrasting run, "the schedule" is its changed schedule. | K12 |
| CP5 | The program ran as specified. M0 to M4 appear in order. The BPLCON0 writes fall between the last read of `wait150` and M1 or M2, at line 150. Coverage: phase A at least two model periods, phase B up to line 40 of B21, phase C up to line 40 of C3, phase D at least 12 ms after M3. | K13 |
| CP6 | The beam keeps its timing in phases B and C: A3 and A4 hold there. | K14 |

## 8. Contrasting runs

Each contrasting run is the main run with one change. It is graded **with the
main run's expectations**. For each run, the page predicts a different outcome
on the named checks, which must come out red. The program still finishes, and
the preconditions other than the one a run targets still hold.

| Run | Change | What the pages predict | Red |
|---|---|---|---|
| K1 | Phase B writes BPLCON0 = $0000 instead of $0008. | No light pen input and no latch in phase B. p.239: "To enable the light pen input, write a 1 into bit 3 of BPLCON0". p.229: "Bit 3 enables the light pen latch". | P1, P2 (ii), P3, P4, P5, P6, P7 (b), P8 (a), P9 (b), P10 (a), P11, P12 |
| K2 | A pin-6 pulse (both ports) at (100, 100) is added in B2 and in B3. | Captures in B2 and B3, with VB(B3) and VB(B4) holding those positions (p.238 step 4, p.239). | P7 (a)(b), P8 (b), P11, P12 |
| K3 | Phase C writes BPLCON0 = $0008 instead of $0000. | The C1 pulse is captured and the C1 and C2 field ends latch. | P10 (b) |
| K4 | Phase A writes BPLCON0 = $0008 instead of $0000. | Phase-A field ends latch (p.239: "If no trigger is seen, the counters latch at the end of the display field"). | CP1 (A1) |
| K5 | The phase-B loop gets a delay of at least 300 colour clocks per sample. | Samples more than a line apart. | CP2 |
| K6 | The line-10 pulse of B15 and the line-19 pulse of B17 are moved to line 30 (H = 100). | Line 30 is after vertical blank on every page, so these pulses are captured (p.238 step 1). | P9 (a)(b) |
| K7 | The line-32 pulses of B15 and B17 are moved to line 12. | Inside vertical blank, so they are not captured. | P9 (b) |
| K8 | The first pulse of B14 (line 80) is removed. | The capture happens at (150, 100) instead. | P5 |
| K9 | In B18, the pin-1 pulse at line 40 is replaced by a pin-6 pulse. | Capture at (40, 100). | P2 (i) |
| K10 | In phase D, pin 9 is driven low in the windows where the main run drives pin 5 low. | Bits 8 and 12 of POTGOR stay 1 (p.240, Table 8-4: pin 9 is bits 10 and 14). | P13 |
| K10b | In phase D, port 1 pin 5 is held low throughout. | Bit 8 reads 0 in every window. | P13 |
| K11 | A pin-6 pulse (both ports) at (100, 100) is added in B1. | Capture in B1. | CP3 |
| K12 | Every pulse is placed one line later than the schedule says. | Placement differs from the schedule. | CP4 |
| K13 | The phase-B buffer is shortened so that phase B ends in B19. | Coverage missing. | CP5 |
| K14 | Directly after the `wait150` that ends phase B (line 150, after every graded phase-B window), the program writes VHPOSW = $6400 once (line 100, H 0). p.285: "Write vertical and horizontal position of beam or lightpen"; Appendix B p.289: "Write vert and horiz position of beam". | The counter jumps back 50 lines and to H 0. The refresh group, drawn at a fixed place in the line (p.195), and the field boundary no longer sit where the model puts them in phase C. | CP6 (A3 or A4) |

---

## 9. Propositions

### P1 — A light pen is accepted on one of the controller ports

1. **Proposition.** A pulse on pin 6 of at least one of the two controller
   ports is captured as a light-pen trigger. Which port it is, the pages leave
   open.
   Claims: opus:8-lpen.c1, sol:8-pen.c1 (p.238); opus:8-intro.c12,
   sol:8-overview.c6, opus:8-intro.c2 in part (p.227).
2. **Stimulus.** Program of §4. The pin-6 pulse in B4 is on port 1 only, and in
   B5 on port 2 only, both at (100, 100). There are no other inputs in these
   fields.
3. **Observation.** Stored samples, POST of the B4 pulse and POST of the B5
   pulse.
4. **Expected.**
   - The pages. p.238: "A light pen can be connected to one of the controller
     ports. On the A1000, the light pen must be connected to port 1. [...] On
     the A500, A2000 and A3000 the default is port 2. An internal jumper can
     select port 1." p.227: "Usually, the nine-pin connectors are used with a
     mouse or joystick but they will also accept input from light pens".
   - Graded: at least one of the two pulses is captured. Every sample of its
     POST has the position that P3 (a) and (b) require for that pulse.
   - Reported: which port or ports captured.
   - Not graded: which port, and whether both do. p.238 ties the port to the
     model and to a jumper, and this system is none of the models it names.
     p.227's "Two multiple purpose mouse/joystick/light pen control ports"
     could be read as both ports, which p.238 contradicts for every model it
     names (10.4).
5. **Preconditions.** CP1 to CP6.
6. **Contrasting case.** K1: neither pulse is captured, so the check is red.
7. **Reference frame.** 5.3 to 5.6.

### P2 — Pin 6 is the beam-trigger line; the other signal pins are not

1. **Proposition.** On the controller connector, a pulse on pin 6 is captured
   as the light-pen trigger, and pulses on pins 1-4, 5 and 9 are not.
   Claims: opus:8-lpen.c16, sol:8-pen.c17 (p.238); opus:8-tbl8-1.c20,
   sol:8-pin.c22 (p.228).
2. **Stimulus.** Field B18: pulses on pins 1, 2, 3, 4, 5, 9 (both ports) at
   lines 40, 60, 80, 100, 120, 140, then pin 6 (both) at (160, 100).
3. **Observation.** RUN(B18), which runs from line Vrun to the pin-6 pulse at
   line 160, and POST of the pin-6 pulse.
4. **Expected.**
   - The pages. p.238: "The pen converts this light pulse into an electrical
     pulse on the "Beam Trigger" line (pin 6)." Table 8-1 (p.228), Light pen
     column: pin 6 "beam trigger"; pins 1-4 "---"; pin 5 "pen pressed to
     screen"; pin 9 "button 2**".
   - (i) Every sample of RUN(B18) is current: none of the six other pulses is
     captured. If one were, the line of the latched value would name the pin.
   - (ii) The pin-6 pulse is captured (P3 (a) and (b)).
   - Quantifier. "The Beam Trigger line (pin 6)" makes pin 6 the only trigger,
     so all six other signal pins are driven. Pins 7 (+5V) and 8 (ground)
     cannot be driven.
5. **Preconditions.** CP1 to CP6. By CP4, the six pulses were delivered on
   both ports at their lines.
6. **Contrasting case.** K9 predicts a capture at line 40, so (i) is red. K1
   predicts no capture, so (ii) is red.
7. **Reference frame.** 5.3 to 5.6.

### P3 — A trigger captures the beam position exactly at its instant

1. **Proposition.** With the light pen input enabled, a pulse on the trigger
   line captures the beam counter's vertical and horizontal value at the
   instant of the pulse, exactly.
   Claims: opus:8-lpen.c17, opus:8-lpen.c18, sol:8-pen.c18, sol:8-pen.c19,
   sol:8-pen.c20 (p.238).
2. **Stimulus.** Every graded pin-6 pulse (5.5):
   - the capturing one of B4 and B5;
   - B6 to B13;
   - the first pulse of B14;
   - the line-32 pulses of B15 and B17;
   - the pin-6 pulse of B18.

   This gives at least 13 captures per run. They cover lines 32, 40, 80, 100,
   160, 200 and 258, and at line 100 the H values 100 to 103, 105 and 108.
3. **Observation.** For each pulse p, the positions of the samples in POST(p).
   They are all equal (P5). Call that position P(p).
4. **Expected.**
   - The page. p.238: "This trigger signal tells the internal circuitry to
     capture and save the current contents of the beam register, VPOSR. This
     allows you to determine where the pen was placed by reading the exact
     horizontal and vertical value of the counter beam at the instant the beam
     passed the light pen."
   - (a) V(P) equals the model line at t_on. The pulses sit at mid-line, so
     every instant of the pulse lies in the same line.
   - (b) H(P) lies within beam([t_on, t_off]). The page states no delay between
     pulse and capture and does not say which edge of the pulse counts. The
     window is the pulse's own duration plus the model uncertainty u of 5.2.
     It is not a tolerance granted to the implementation.
   - (c) **Exactness.** The offset d(p) = H(P) - H_ref(t_on) is the same for
     every graded pulse, with a spread of 0, across all lines and all H steps.
     A shift of the onset by k colour clocks must shift the captured H by
     exactly k, to the register's stated resolution (p.239: "Horizontal 2 low
     resolution pixels", which is one H unit). The model's constant reference
     error cancels in (c).
   - Quantifier: every graded pulse.
5. **Preconditions.** CP1 to CP6. By CP4, the H sweep and the lines were placed
   as scheduled.
6. **Contrasting case.** K1: the page predicts no capture. POST(p) holds
   current samples, so (a), (b) and (c) are red.
7. **Reference frame.** 5.2 (grid, u), 5.3 (beam, H_ref), 5.6 (onset phase).

### P4 — VPOSR holds the high-order bit, VHPOSR the low-order bits of the pen position

1. **Proposition.** The captured pen position is read with its high-order bit
   (V8) in VPOSR and its low-order bits (V7-V0, H8-H1) in VHPOSR.
   Claims: opus:8-regs.c18, opus:8-regs.c20, sol:8-reg.c20, sol:8-reg.c22
   (p.229).
2. **Stimulus.** Captures of B13 at line 258 (V8 = 1); B11 at 40, B12 at 200
   and B6 to B10 at 100 (V8 = 0).
3. **Observation.** In every sample of POST(p), the word read at $DFF004 and the
   word read at $DFF006, taken separately. The window runs through VB(k+1),
   where the current line (0 to 19) has V8 = 0.
4. **Expected.**
   - The pages. p.229: "VPOSR ($DFF004) Read light pen position (high order
     bits)" and "VHPOSR ($DFF006) Read light pen position (low order bits)".
     p.238: VPOSR "Bit 0 V8 (most significant bit of vertical position)";
     VHPOSR "Bits 15-8 V7-V0 (vertical position)", "Bits 7-0 H8-H1 (horizontal
     position)".
   - In every POST sample, VPOSR bit 0 is bit 8 of the capture line. That is 1
     for B13, including in VB(B14), and 0 for the others.
   - VHPOSR bits 15-8 are the low eight bits of the capture line: $02 for B13
     (258 = $102), $28, $C8 and $64 for lines 40, 200 and 100.
   - VHPOSR bits 7-0 are H(P) of P3.
   - VPOSR bits 15-1 are not graded as values. p.238: "Chip ID code. Do not
     depend on value!"; p.216 calls bits 14-1 "Unused".
5. **Preconditions.** CP1 to CP6. By CP4, captures were scheduled at V8 = 1
   (B13) and at V8 = 0 (B11, B12).
6. **Contrasting case.** K1: B13's pulse is not captured, and VB(B14) shows the
   current lines 0 to 19 with VPOSR bit 0 = 0, so the check is red.
7. **Reference frame.** 5.1, 5.3 to 5.5.

### P5 — After a trigger, VPOSR is frozen until the release

1. **Proposition.** Once the light pen input is enabled and a trigger arrives,
   the value of VPOSR stays the same up to the release. A second trigger does
   not change it, and neither does the end of the field.
   Claims: opus:8-lpen.c36, sol:8-pen.c36 (p.239).
2. **Stimulus.** Every graded capture (P3), in particular:
   - B14: a second pin-6 pulse at (150, 100) after the capture at (80, 100);
   - B15: a pulse at line 10, inside VB(B15), after B14's capture.
3. **Observation.** For each graded capture in field k, every sample of POST(p).
   The window ends with VB(k+1) at line 19, the last line that every page
   places before the end of vertical blanking (10.1).
4. **Expected.**
   - The pages. p.239: "Once the light pen input is enabled and the light pen
     issues a trigger signal, the value in VPOSR is frozen." The freeze is
     bounded by "This freeze is released at the end of internal vertical
     blanking (vertical position 20)."
   - Every sample of the window has the same 32-bit value (VPOSR and VHPOSR).
     That value is the one of the first sample after the pulse.
   - B14's second pulse does not change it. B15's line-10 pulse does not
     change it. The field end between k and k+1 does not change it either:
     p.239 latches at the field end only "If no trigger is seen".
   - Quantifier: every sample of every window, at least 13 windows per run.
   - The comparison is of the whole value because the page says "the value in
     VPOSR is frozen". If two values differ only in VPOSR bit 15 (LOF), the
     report says so (10.8).
5. **Preconditions.** CP1 to CP6.
6. **Contrasting case.** K8: the page predicts the capture at (150, 100), so the
   samples of B14 between lines 80 and 150 are current and the check is red. K1
   predicts nothing frozen, so it is red too.
7. **Reference frame.** 5.3 to 5.5.

### P6 — While latched, VPOSR never shows the current beam position

1. **Proposition.** As long as VPOSR is latched, whether by a pen capture or by
   the end-of-field latch, no read returns the current beam position.
   Claims: opus:8-lpen.c38, sol:8-pen.c38 (p.239).
2. **Stimulus.** Phase B:
   - pen captures: every graded capture of P3;
   - end-of-field latches: the ends of B1, B2, B3 and B20, fields with no
     input change.
3. **Observation.** Two kinds of latched window:
   - after a pen capture in field k: the samples of POST(p) whose read
     interval starts at least one line (227 colour clocks) after t_off, up to
     VB(k+1);
   - after a no-trigger field k: VB(k+1).

   The test knows the register is latched there from the page's own account,
   not from VPOSR. A capture (p.238) or the end of a field without trigger
   (p.239) latches it until the end of internal vertical blanking, which no
   page puts before line 20. CP6 gives the current position independently of
   VPOSR.
4. **Expected.**
   - The page. p.239: "It is impossible to read the current beam location
     while the VPOSR register is latched."
   - No sample in these windows is current (5.3), and all samples of one
     window have the same position.
   - Quantifier ("impossible"): every sample of every window.
5. **Preconditions.** CP1 to CP6.
6. **Contrasting case.** K1: the page predicts no latch, so the samples are
   current and the check is red.
7. **Reference frame.** 5.3 to 5.5; A3 and A4 supply the current position
   during the latch.

### P7 — Without a trigger, the counters latch at the end of the field

1. **Proposition.** With the light pen input enabled and no trigger in a field,
   the beam counters as read through VPOSR/VHPOSR latch at the end of that
   field.
   Claims: opus:8-lpen.c37, sol:8-pen.c37 (p.239).
2. **Stimulus.** Fields B1, B2, B3 and B20. They have no input change and lie
   wholly under bit 3 = 1.
3. **Observation.** For each such field k:
   - RUN(k);
   - the samples of line L_k, which are not graded;
   - VB(k+1).
4. **Expected.**
   - The page. p.239: "If no trigger is seen, the counters latch at the end of
     the display field."
   - (a) Every sample of RUN(k) is current. Nothing latches before the last
     line.
   - (b) Every sample of VB(k+1) has one position F_k, and V(F_k) = L_k, the
     last line of field k.
   - (c) m(F_k) is at least m(R_k), where R_k is the last current sample of
     field k.
   - The page gives no H for "the end of the display field". The samples of
     line L_k are not graded, and H(F_k) is graded only through (c).
   - How F_k compares with $10500 is graded in P11 and P12.
   - Quantifier: every no-trigger field end, four per run.
5. **Preconditions.** CP1 to CP6. CP3 shows that the idle inputs do not
   trigger.
6. **Contrasting case.** K2: the page predicts pen captures in B2 and B3. RUN
   of those fields stops being current from line 100, and VB(B3) and VB(B4)
   hold line-100 positions, so (a) and (b) are red. K1: no latch, so (b) is red
   for all four fields.
7. **Reference frame.** 5.3 to 5.5 (L_k, field boundaries).

### P8 — The freeze is released at the end of internal vertical blanking

1. **Proposition.** Every latch holds until the end of internal vertical
   blanking and no longer. After that, VPOSR shows the current position again.
   Claims: opus:8-lpen.c39, sol:8-pen.c39 (p.239).
2. **Stimulus.** Phase B. Every latched window ending in fields B2 to B21, from
   captures as well as from no-trigger field ends. Whichever port P1 finds, the
   ends of B4 and B5 are latched either way.
3. **Observation.** For each such field j = k+1:
   - VB(j);
   - RUN(j), from Vrun to the first graded pin-6 pulse of j, or to L_j - 1 if
     there is none.
4. **Expected.**
   - The page. p.239: "This freeze is released at the end of internal vertical
     blanking (vertical position 20)."
   - (a) Every sample of VB(j), lines 0 to 19, has one and the same value: the
     value latched by the capture in field k (P5) or at the end of field k
     (P7). For VB(B5), VB(B6) and VB(B20) it may be either kind, depending on
     P1's port or on the observation-only field B19.
   - (b) Every sample of RUN(j) is current.
   - Not graded: NTSC line 20 and PAL lines 20 to 28. p.219 ends vertical
     blanking at line 25 in PAL, and p.79 stops it at $15 (21) in NTSC and
     $1D (29) in PAL (10.1).
   - Reported for each window: the model line and H of the first current
     sample. p.239 predicts line 20.
   - Quantifier: every window, at least 19 per run.
5. **Preconditions.** CP1 to CP6.
6. **Contrasting case.** K1: no latch, so (a) is red. K2: RUN(B2) and RUN(B3)
   are latched from line 100 on, so (b) is red.
7. **Reference frame.** 5.3 to 5.5; Vrun from 10.1.

### P9 — The capture circuitry re-arms by itself as vertical blank ends

1. **Proposition.** At the end of vertical blank the capture circuitry is
   enabled again without any action by the program. A pulse before that point
   is not captured, and a pulse after it is.
   Claims: opus:8-lpen.c12, sol:8-pen.c12 (p.238).
2. **Stimulus.**
   - B15: pin-6 pulses at (10, 100) and (32, 100), after B14's capture.
   - B17: pin-6 pulses at (19, 100) and (32, 100), after the no-trigger field
     B16.
   - The phase-B loop writes no register (§4).
3. **Observation.**
   - RUN(B15) and RUN(B17), from Vrun up to the line-32 pulse;
   - POST of the two line-32 pulses;
   - the POST of every graded capture of B6 to B18.
4. **Expected.**
   - The page. p.238: "Just as the system exits vertical blank, the capture
     circuitry for the light pen is automatically enabled."
   - (a) A pulse inside vertical blank is not captured. Every sample of
     RUN(B15) and RUN(B17) is current, so neither the line-10 nor the line-19
     pulse is held over the release.
   - (b) A pulse after vertical blank is captured without software action. In
     B15 and in B17, POST of the line-32 pulse holds that pulse's position
     (P3 (a) and (b)).
   - (c) The re-arming repeats in every field. Every graded pin-6 pulse of B6
     to B18 is captured, although the program wrote nothing after the phase-B
     enable.
   - "Just as" is graded only as far as all pages agree. Lines 10 and 19 are
     inside vertical blank on every page, and line 32 is after it on every
     page (10.1). Lines 20 to 31 are not probed.
5. **Preconditions.** CP1 to CP6. The builder asserts from the program listing
   that the phase-B loop contains no write to a custom register.
6. **Contrasting case.**
   - K6: the page predicts a capture at line 30. RUN is latched from line 30
     and the captured value is not the line-32 position, so (a) and (b) are
     red.
   - K7: the page predicts no capture at line 12. POST shows current samples,
     so (b) is red.
7. **Reference frame.** 5.3 to 5.6; Vrun from 10.1.

### P10 — Bit 3 of BPLCON0 enables the light pen input and latch

1. **Proposition.** With bit 3 of BPLCON0 set, pulses are captured and fields
   without a trigger latch at their end. With bit 3 clear, neither happens.
   Claims: opus:8-lpen.c35, sol:8-pen.c35 (p.239); opus:8-regs.c16,
   sol:8-reg.c18 (p.229).
2. **Stimulus.**
   - BPLCON0 = $0008 in phase B and $0000 in phases A and C. The two words
     differ only in bit 3 and are whole-word writes (p.89).
   - Phase C: C1 has a pin-6 pulse (both ports) at (100, 100); C2 and C3 have
     no input change.
3. **Observation.**
   - Phase B: the P3 and P7 windows.
   - Phase C: C1 from Vrun to L - 1, including after the pulse; all lines of C2
     (0 to L - 1); VB(C3).
4. **Expected.**
   - The pages. p.239: "To enable the light pen input, write a 1 into bit 3 of
     BPLCON0." p.229: "BPLCON0 ($DFF100) Bit 3 enables the light pen latch".
     p.271: "LPEN =Light pen enable (reset on power up)".
   - (a) Bit 3 = 1: every graded pin-6 pulse is captured (P3 (a)), and every
     no-trigger field end latches (P7 (b)).
   - (b) Bit 3 = 0: every sample of the phase-C windows is current. The C1
     pulse is not captured, and neither the C1 nor the C2 field end latches.
   - Not graded: B0, C0 and VB(C1). These are the fields in which bit 3
     changes, and the field end they reach. The pages do not say when a change
     of bit 3 takes effect relative to the automatic enabling at the end of
     vertical blank (p.238 step 1).
5. **Preconditions.** CP1 to CP6. By CP5, the BPLCON0 writes are bracketed by
   markers.
6. **Contrasting case.** K1 makes (a) red. K3 makes (b) red.
7. **Reference frame.** 5.3 to 5.5.

### P11 — The two-read detection method gives the right answer

1. **Proposition.** The first method on p.239 (read VPOSR twice as a long word,
   compare, mask, compare with $10500) answers correctly whether the light pen
   has triggered since the last top-of-screen. When it answers yes, the value
   is the pen's position.
   Claims: opus:8-lpen.c43, sol:8-pen.c41 (step 2); opus:8-lpen.c47,
   sol:8-pen.c44 (step 4, "greater"); opus:8-lpen.c48, sol:8-pen.c45,
   sol:8-pen.c46 (step 4, "less") (p.239).
2. **Stimulus.** Phase B (§6).
3. **Observation.**
   - The pairs. Every two consecutive samples (i, i+1) of phase B are the
     page's "Read (long) VPOSR twice".
   - Samples excluded: those in NTSC line 20 or PAL lines 20 to 28, those in
     line L of any field, torn samples, and samples whose [a, b] overlaps a
     pulse's [t_on, t_off].
   - Pairs excluded: those in fields B0, B4, B5 and B19, and in VB(B1), VB(B5),
     VB(B6) and VB(B20). These windows depend on a field that is not graded, on
     P1's port, or on the observation-only field B19.
   - The truth T(i) for "the light pen has triggered since the last
     top-of-screen": a graded pin-6 pulse lies between the start of the last
     model line 20 and a of sample i.
   - The method's answer:
     - step 2: v(i) differs from v(i+1) in any bit, so "not triggered";
     - steps 3 and 4 otherwise, with m = v AND $1FFFF: m > $10500 gives "not
       triggered", and m < $10500 gives "triggered, value = pen position";
     - m = $10500 gets no answer from the page and is not graded.
4. **Expected.**
   - The page, p.239:
     - "1. Read (long) VPOSR twice."
     - "2. If both values are not the same, the light pen has not triggered
       since the last top-of-screen (V = 20)."
     - "3. If both values are the same, mask off the upper 15 bits of the
       32-bit word and compare it with the hex value of $10500 (V=261)."
     - "4. If the VPOSR value is greater than $10500, the light pen has not
       triggered since the last top-of-screen. If the value is less, the light
       pen has triggered and the value read is the screen position of the light
       pen."
   - For every graded pair, the method's answer equals T(i). Where it answers
     "triggered", m equals m(P) of the capture since the last top-of-screen.
   - After a capture, therefore, pairs must be equal (step 2 may not say "not
     triggered") and below $10500. Without a capture, equal pairs must lie
     above $10500.
   - All graded captures are below line 261. For a capture at line 261 or later
     (B19), step 4 contradicts p.238 step 4 (10.2), so it is not graded; its
     pairs are reported.
   - Quantifier: every graded pair.
5. **Preconditions.** CP1 to CP6. By CP4 and the schedule, both classes occur:
   pairs with T true (after 13 or more captures) and latched equal pairs with T
   false (four no-trigger field ends).
6. **Contrasting case.**
   - K1: after the pulses the pairs are unequal while T is true, so the check
     is red.
   - K2: the method answers "triggered" in B2, B3, VB(B3) and VB(B4), where the
     main run's T is false, so the check is red.
7. **Reference frame.** 5.1 (m), 5.3 to 5.6; Vrun from 10.1.

### P12 — The single read during vertical blanking gives the right answer

1. **Proposition.** The simplified method on p.239 (one long read of VPOSR
   during internal vertical blanking, masked and compared with $10500) answers
   correctly whether the light pen triggered in the preceding field. When it
   answers yes, the value is the pen's position.
   Claim: opus:8-lpen.c52 (p.239).
2. **Stimulus.** Phase B (§6).
3. **Observation.**
   - The samples: every sample whose [a, b] lies within lines 1 to 19 (the
     page's "0<V20", 10.3) of fields B2, B3, B4, B7 to B19 and B21.
   - Excluded: VB(B1) (after B0), VB(B5) and VB(B6) (after the single-port
     fields), and VB(B20) (after B19).
   - The truth: the preceding field contains a graded pin-6 pulse.
4. **Expected.**
   - The page, p.239: "A somewhat simplified method [...] read the register
     only during the internal vertical blanking period of 0<V20:
     1. Read (long) VPOSR once, during the period of 0<V20.
     2. Mask off the upper 15 bits of the 32-bit word and compare it with the
     hex value of $10500 (V=261).
     3. If the VPOSR value is greater than $10500, the light pen has not
     triggered since the last top-of-screen. If the value is less, the light
     pen has triggered and the value read is the screen position of the light
     pen."
   - Where the truth is "triggered": m < $10500 and m = m(P) of that capture.
   - Where it is "not triggered": m is not less than $10500. The page's "greater"
     leaves m = $10500 without an answer, so that value is not graded.
   - Quantifier: every sample of every listed window.
5. **Preconditions.** CP1 to CP6. By the schedule, windows of both kinds exist:
   after captures (B7 to B19) and after no-trigger fields (B2, B3, B4, B21).
6. **Contrasting case.**
   - K1: the samples are current, with m below 256*20. That is "triggered"
     where the truth is "not triggered", and not the pen position where it is
     "triggered", so the check is red.
   - K2: VB(B3) and VB(B4) hold pen positions where the main-run truth is "not
     triggered", so the check is red.
7. **Reference frame.** 5.1 (m), 5.3 to 5.5.

### P13 — The pen-pressed switch on pin 5 reads like the middle mouse button

1. **Proposition.** The pen-pressed-to-screen signal on pin 5, a potentiometer
   line, is read the way a mouse button on a pot line is read. POTGOR shows 0
   while the switch is pressed and 1 while it is not.
   Claims: opus:8-lpen.c10, opus:8-lpen.c11, sol:8-pen.c10, sol:8-pen.c11
   (p.238).
2. **Stimulus.**
   - POTGO = $FF00: all four pot pins set as outputs driven high, with START
     = 0 (§4; p.241, p.236).
   - The program reads POTGOR ($DFF016) as a word in a loop.
   - Inputs per §6, phase D. Pressing is pin 5 held low: p.241, "A button is a
     normally open switch that shorts to ground".
3. **Observation.**
   - POTGOR bit 8 (DATLX, port 1 pin 5) and bit 12 (DATRX, port 2 pin 5).
   - The graded windows run from 300 microseconds after each input change
     (and after M3) to the next change. p.240: "There are large capacitors on
     these lines, and it can take up to 300 microseconds for the line to change
     state."
4. **Expected.**
   - The pages:
     - p.238: "Note that this switch is connected to one of the potentiometer
       inputs and must be read as same as the right or middle button on a
       mouse."
     - Table 8-1 (p.228), Light pen column: pin 5 "pen pressed to screen".
     - p.232: "Button 3, when used, is connected to pin 5, the other
       proportional controller input."
     - Table 8-4 (p.240): "08 DATLX data for port 1, pin 5 (middle mouse
       button)", "12 DATRX data for port 2, pin 5".
     - p.241: "set the proper pin to output, and drive the line high (set both
       OUT... and DAT... to 1). Reading POTINP will produce a 0 if the button
       is pressed, a 1 if it is not."
   - Bit 8 is 0 in every sample of the window in which port 1 pin 5 is low,
     and 1 in every sample of every other graded window.
   - Bit 12 behaves the same way for port 2 pin 5.
   - In the pin-9 window, both bits stay 1.
   - Other bits are not graded: bits 10 and 14 are the right button, and
     bits 7-1 are the "chip revision identification number".
   - Both ports are graded because p.238 makes the pen's port model-dependent
     and adds "Regardless of the port used, the light pen design is the same."
5. **Preconditions.** CP5 (POTGO written before M3, phase D coverage), CP2
   (phase D sample spacing), CP4 (level changes delivered as scheduled).
6. **Contrasting case.** K10: pin 9 instead of pin 5, so the page predicts bits
   8 and 12 stay 1, and the check is red. K10b: pin 5 held low throughout, so
   bit 8 is 0 where 1 is expected, and the check is red.
7. **Reference frame.** Time from M3 at the Chip-memory port; the input record
   of the harness.

---

## 10. What the pages leave open or state in conflict

1. **End of vertical blanking, and so the release line.**
   - p.239: "released at the end of internal vertical blanking (vertical
     position 20)", "top-of-screen (V = 20)".
   - p.219: "The range starts at line 0 and ends at line 20 for NTSC or line 25
     for PAL."
   - p.79: "Vertical Blank Stop $15 (21)" for NTSC and "$1D (29)" for PAL.
   - p.239 alone speaks of the latch and calls its blanking "internal". It does
     not restrict its "20" to one standard.
   - All pages agree only that lines 0 to 19 are blanked and that blanking is
     over by line 21 (NTSC) or 29 (PAL). P5, P6 and P8 grade the latch through
     line 19. P8 and P9 grade the running counter or re-arming from Vrun = 21
     or 29 and report the line where it actually happens.
2. **The threshold $10500 (V=261) against captures at or below line 261.**
   - p.239 step 4 answers "not triggered" for any value above $10500.
   - p.238 step 4 says the captured value is the pen's exact position. That
     includes lines 261 to 311 in PAL (p.216: "Allows PAL line counts (313)")
     and NTSC line 261 with H > 0, a displayable line (p.79: 241 displayable
     lines after 21 blanked).
   - For such a pen the two statements contradict each other. P11 and P12 grade
     the method only for captures below line 261. Field B19 records one capture
     beyond that as observation only.
3. **"0<V20".** The printed page lacks a relation sign in both places
   ("internal vertical blanking period of 0<V20"; "during the period of
   0<V20"). P12 grades lines 1 to 19, the part common to every reading.
4. **Which port.**
   - p.238: port 1 on the A1000; port 2 by default on the A500, A2000 and
     A3000, with a jumper for port 1.
   - p.227: "Two multiple purpose mouse/joystick/light pen control ports".
   - The system under test is none of those models. P1 grades "at least one
     port" and reports which.
5. **Trigger polarity, pulse width, delay.**
   - p.238 says only "an electrical pulse".
   - p.333 names the line LPENTG* and p.381 LP*, active low in the manual's
     notation.
   - p.333 adds "Note: depending on the maker, the light pen input may be
     either." It does not say either of what: polarity or port.
   - No page gives a minimum width or a delay. The stimulus is a low pulse of 4
     colour clocks. P3 (b) spans the pulse, and P3 (c) is independent of both
     edge and delay.
6. **Address of BPLCON0.**
   - p.229, p.271 and Appendix B (p.291) give $DFF100.
   - The figure on p.333 labels "BPLCON0 write only DFF104" and also "POTINP
     read only DFF104". p.229 and p.240 give $DFF016 for POTINP/POTGOR, and
     p.271 assigns $104 to BPLCON2.
   - The pages agree that bit 3 of BPLCON0 enables the light pen. The program
     uses the Appendix B address, as the test contract requires.
7. **VPOSR bits 14-1.** p.238: "Chip ID code. Do not depend on value!" p.216:
   "Bits 14-1 Unused". Not graded as values.
8. **LOF while latched.**
   - The pages do not say whether VPOSR bit 15 changes from field to field in
     non-interlaced mode. p.285: "LOF=Long frame (auto toggle control bit in
     BPLCON0)".
   - p.239 says "the value in VPOSR is frozen" (pen capture) and "the counters
     latch" (no trigger), and its step 2 compares whole long words.
   - P5 and P11 compare whole values, as p.239 does, and report separately when
     a difference lies only in bit 15. P6 to P8 compare positions.
9. **Instant of the end-of-field latch.** "At the end of the display field"
   gives no H. P7 grades it at line resolution plus the ordering F >= R.
10. **When a change of bit 3 takes effect** relative to the automatic enabling
    at the end of vertical blank (p.238 step 1): not stated. The fields in
    which bit 3 changes (B0, C0) and the field end they reach are not graded.
11. **The value exactly $10500** is neither "greater" nor "less". The page
    gives no answer for it, and it is not graded.
12. **ECS LPENDIS reset value:** not stated (p.305). BEAMCON0 is not written
    (§3).
13. **Text layer against print, no effect on expectations.**
    - p.239: the text layer adds "(LPEN)", which is not printed.
    - p.238: the print reads "the right or middle button on a mouse".
    - p.240: the print names the read address POTINP, where the text layer has
      POTGOR.
    - p.228: the printed Table 8-1 heads its first column "Pen" instead of
      "Pin".
14. **Figure 6-9 (p.195)** says it "has been 'adjusted'". The refresh anchor A3
    uses only the constancy of the refresh position, not its clock numbers.
