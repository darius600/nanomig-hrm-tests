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
| opus:8-lpen.c41 | 239 | "There is no single bit in the system that indicates a light pen trigger" says that something does not exist anywhere in the system. The sentence names no register and no value. A program can read the custom registers of Appendix B and the CIA registers of Appendix F, but not every bit of the system, so no set of readings could establish "no bit in the system". Its practical consequence, that a trigger can only be found by reading VPOSR, is what P11 and P12 test. |
| sol:8-pen.c40 | 239 | Same sentence, same reason. |
| opus:8-lpen.c34 | 239 | Only names bit 3 "LPEN", and a name cannot be observed. Also, the quoted "(LPEN)" is not on the printed page 239, which reads "To enable the light pen input, write a 1 into bit 3 of BPLCON0." The name is printed on p.89 ("Bit 3 - LPEN (light pen enable)") and p.271 ("LPEN =Light pen enable"). |
| opus:8-intro.c2 | 227 | Its `says` reads "Two multiple purpose mouse/joystick/light pen control ports" as *each* port serving a light pen. p.238 ties the light pen to one port per model ("On the A1000, the light pen must be connected to port 1. [...] On the A500, A2000 and A3000 the default is port 2. An internal jumper can select port 1."), and the system under test is none of those models. P1 grades that at least one port captures and reports which; "each" is not graded, so the id is not claimed (10.4). |

## 2. Propositions and the claims they rest on

| Proposition | Claims | Pages |
|---|---|---|
| P1 A light pen is accepted on one of the controller ports | opus:8-lpen.c1, sol:8-pen.c1, opus:8-intro.c12, sol:8-overview.c6 | 238, 227 |
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
  memory. Every contrasting run of §8 that targets a proposition needing a
  latch (P1 to P9, P10 (a), P11, P12: K1, K2, K6, K7, K8, K9, K9b, K16, K19,
  K20) is made in OCS/PAL, not ECS/PAL: in ECS an implementation may go
  unlatched by LPENDIS's unstated reset value (below), and §8's own runs must
  be able to turn a graded check red, which the ECS record exists to avoid.
  K3, K4, K17 and K18 target P10 (b) or CP1 but are made in OCS/PAL for the
  same reason: their own page-model contrast (below) synthesizes a bit-3
  latch, which ECS's unstated LPENDIS reset value does not give. Every other
  contrasting run (targeting a precondition other than CP1, or P13, neither
  of which the ECS record covers) stays in ECS/PAL, its original chipset.
- **Page model (§8).** Because a run may show no latch at all in OCS as well
  as in ECS, a contrasting run cannot by itself show a graded check going
  from green to red: there may be no green to start from. Each main run, and
  each of the ten OCS/PAL contrasting runs of P1 to P12 above, is also graded
  a second way: the page's own latch (p.238 step 4, p.239) is applied to that run's
  own measured phase-B samples, and the result is graded with this
  package's own P1-P13 code, under the label prefix "PAGE-MODEL ". Capture
  is graded from line 20 on (p.238 step 1); a trigger's onset freezes the
  value (p.238 step 4); with no trigger, the counters latch at the field end
  (p.239); the freeze holds until line 20 of the next field (p.239). The
  page model is a control of the comparators on the run's own samples, taken
  in the model's own frame (the latch value is the model's position at the
  onset); it shows that a comparator can turn red when a run's samples
  change, and not that the comparators agree with a latch implemented in
  hardware. That is shown by an implementation that follows pp.238-239,
  `latch.patch` (`make latch-test-fx68k`, `make latch-test-tg68k`), which
  every main run is also made against (§8, "Conforming implementation"). K3,
  K4, K17 and K18 target a phase-A or phase-C proposition instead, so each
  carries its own analogous page-model contrast (§8), built the same way from
  that run's own measured phase-A or phase-C samples and the same latch rule,
  and graded with the same unchanged comparator (the one function that
  grades P10 (b), or CP1) rather than the phase-B P1-P13 grader.
- **Standard.** PAL or NTSC is a configuration of the machine, not of the
  program: p.295, "the chips from the US factory are configured for NTSC mode.
  In order to use them on a PAL system, you may have to reset the motherboard
  jumpers". The program never writes BEAMCON0 (whose bit 5 is "PAL
  Programmable pal mode enable", p.305). A2 (5.4) confirms from the measured
  field length that each run is in the standard it is graded as.
- **Harness configuration.** The chipset (OCS or ECS) and the standard (PAL or
  NTSC) are harness configuration. The builder takes them from the harness's
  documented `Config` values (`nanomig_harness.h`). A configuration the
  harness cannot make leaves its runs, main or contrasting, recorded as not
  made, not graded.
- **ECS.** The program never writes BEAMCON0. On p.305, bit 13 is
  "LPENDIS    Ignore latched pen value on vertical pos read", and the page adds
  "Do not write to this register." The pages give no reset value for LPENDIS.
  The ECS expectation that the latch can be seen without touching BEAMCON0
  rests on p.295: "These chip revisions support advanced features in addition
  to all of the standard features previously available" and "plug-compatible
  replacements". Whether an ECS part that never has BEAMCON0 written shows the
  latch at all rests on LPENDIS's reset value, so every ECS run records the
  propositions that need a latch (P1 to P9, P10 (a), P11, P12) instead of
  grading them, whether or not its phase B shows a latch, naming the unknown
  reset value of LPENDIS (p.305) as the open point. The preconditions, P10
  (b) and P13 are graded in that run as usual. The program writes BEAMCON0 in
  no run, so no run grades an ECS configuration on those propositions.
- **Idle inputs.** Apart from the changes listed in §6, every input stays
  released throughout. On both controller ports, pins 1-4, 5, 6 and 9 are at
  the level of an open switch (high). Mouse counts do not change, no keyboard
  code is sent, and serial RxD is idle.
- **Pin 6 is only driven by the harness.** p.229 lists "CIAAPRA ($BFE001) Input
  and output for pin 6", and p.241 says the fire buttons "can also be
  configured as outputs" through CIAADDRA. The program writes no CIA
  register; its one CIA access is the read of CIAAPRA that CP3 uses. Pin 6
  therefore stays an input that only the harness drives.
- **Port mapping.** Measured at the ports, not from harness field names or
  RTL files: joystick0 reaches port 2 (CIAAPRA bit 7 low when its pin-6 bit
  is set; POTGOR bit 14 low when its pin-9 bit is set), and joystick1
  reaches port 1 (CIAAPRA bit 6; POTGOR bit 10). mouse_buttons bit 0 also
  reaches port 1's pin 6; bit 1 reaches port 1's pin 9 (POTGOR bit 10); bit 2
  reaches port 1's pin 5 (POTGOR bit 8). No input this harness exposes
  (every joystick0/1 bit, every mouse_buttons bit) reaches port 2's pin 5
  (POTGOR bit 12): driving each one in turn leaves it unchanged.
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
        move.b  $bfe001,FIRE            ; CIAAPRA, read only: bit 6 port 1 and bit 7 port 2 pin 6,
                                        ;   1 = "switch open" (p.229, p.232); for CP3
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
        DONE                            ; stores DONE_VALUE at DONE_ADDR, then hangs (nanomig.i)

wait150 move.l  $dff004,d0              ; wait until a read shows vertical position 150
        lsr.l   #8,d0
        andi.w  #$01ff,d0
        cmpi.w  #150,d0
        bne.s   wait150
        rts
```

- The program is assembled for the 68000, vasm's default, with no `-m` option.
  With `-m68010` or higher, vasm assembles `move.w #$0000,$100(a6)` as
  `clr.w $100(a6)`, which on a 68000 reads the write-only BPLCON0 before
  writing it; the listing must show MOVE for every BPLCON0 write.
- `MARK` is one word address in Chip memory, `FIRE` one byte. The marker writes
  M0 to M4 (five distinct values) are visible at the Chip-memory port. They
  bracket the BPLCON0 writes and mark where each phase starts. The builder
  sizes the buffers from the sample rate measured in phase A, with at least
  10 % spare, so that CP5 holds: phase A at least 4.5 fields, phase B up to
  line 40 of field B21, phase C up to line 40 of field C3, and phase D at
  least 12 ms.
- **Placement.** `MARK`, `FIRE` and all four buffers lie in the Chip memory
  that the harness maps one to one, $080000-$1FFFFF (`nanomig.i`: "Program,
  stack and results all live in chip RAM that nanomig.v maps one-to-one
  ($080000-$1FFFFF with 2 MB chip RAM)"). Nothing is placed below $080000.
  p.223: after reset "An additional copy of the system ROM responds starting
  at memory location $00000000. [...] Software must assume that memory is not
  available. The OVL bit in one of the 8520 Chips disables the overlay"
  (CIAAPRA bit 0, p.339). The program never writes CIA-A, so the overlay
  stays on, and `nanomig_harness.h` says "A test program must not write
  inside the overlay window". No two of these areas overlap, and none
  overlaps the program (from `PROGRAM_ORG` = $080400), the supervisor stack,
  which grows down from `PROGRAM_STACK` = $0FFF00 and holds the return
  address of `bsr wait150`, or `DONE_ADDR` = $0C0FFE. The 1 MB from $100000
  to $1FFFFF lies above all of these.
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
- **m(P)** of a pulse P is the masked value of the position that P3 (a) and
  (b) require for it: V is the model line of its onset and H lies in
  [H_ref(t_on), H_ref(t_on) + 4 + u] (9, P3). P11 and P12 compare the value
  read with this position, not with a sample of the run itself.

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
  two bus cycles, that is four colour clocks (p.196: a 68000 access takes two
  memory cycles).
- **Model uncertainty.** u is the largest b - a in phase A, in colour clocks.

### 5.3 The beam model

The model is fitted on samples of phase A, when bit 3 is clear and
VPOSR/VHPOSR read the beam counter. p.216 describes VHPOSR as "Read vertical
and horizontal position of the counter that is producing the beam on the
screen". p.238 says "The light pen register is at the same address as the
beam counters".

Only the phase-A samples of lines Vrun to L - 1 enter the fit and A1. Lines 0
to Vrun - 1 and line L are left out: whether they read the running counter
while bit 3 is clear is part of what P10 (b) grades (the end-of-field latch
and its release, p.239), and the anchor does not rest on it. The fitted
samples are chosen by their read values. A field of reads ends where the read
V falls and the next sample stays below the value before the fall. L is the
largest V read in a field of reads (for the cut fields at the two ends of
phase A, the L of the complete fields), and the fit takes the samples whose
read V lies from Vrun to L - 1. Torn samples (below) are left out of the fit
and do not end a field of reads. The model extends to lines 0 to Vrun - 1 and
to line L by its fitted line lengths and phase.

- The model gives a position beam(t) for any time t of the run. Within a line,
  H advances by one per colour clock.
- Line lengths (p.194 gives 227.5 colour clocks, so lengths may alternate),
  the last line L of each field and the phase all come from the fit.
- The pattern of line and field lengths must repeat with a period of one or
  two fields.
- Because the read instant is only known within [a, b], beam(t) is an
  interval of width u.
- H_ref(t) is the model computed with every fitted read instant taken at b:
  its anchor is the median over the fitted samples of b minus the sample's
  position in colour clocks (the model of the other bullets takes a). Every
  true read instant lies in [a, b], so H_ref(t) is the counter's position at
  time t less between 0 and u colour clocks: H_ref(t) <= counter(t) <=
  H_ref(t) + u (P3 b). It is used for P3 (b), P3 (c) and P4.
- A sample is **current** when its position lies in beam(t) for some t in its
  [a, b]. beam(t) is the counter's position, with V as 5.3 below places it.

**The counter's V-increment phase.** The counter's V does not change at H = 0
of the model's line: it changes a few colour clocks into the line, at an H
the pages do not state (p.238 gives the register's bits, not when V
increments). While H is below that phase, a read returns the new line's H
with the previous line's V. The phase is measured on the fitted samples of
phase A, from their read values alone. A fitted sample whose reading puts the
anchor one line later than the median anchor read the previous line's V, and
its H is below the phase; a fitted sample at an H of at most 15 whose
reading agrees with the median anchor read the new line's V, and its H is at
or above the phase. The phase lies in the bracket from the largest H of the
first kind plus one to the smallest H of the second kind, and it is taken at
the lower end; both ends, and the number of fitted samples of the first kind,
are printed as MEASURED. The model's V is the previous line's V while H is
below the phase, and the model's line 0 reads L until then; the model's line
start for V = N, used for the windows of 5.5, is the line's start plus the
phase. A2 requires the bracket to hold (lower end not above upper end) and
the phase to be at most 15 colour clocks. Measured: fx68k at H = 2 (bracket
2 to 3), TG68K at H = 2 (bracket 2 to 3), in all four configurations.

**Torn samples.** A sample whose two register reads straddle a change of V8 can
combine V8 of one line with V7-V0 of another. These changes happen at model
line 255 to 256 and at L to 0, at the counter's V-increment phase. Samples
whose [a - 2, b] (in colour clocks) contains such a change are torn, and are
exempt from every check. The same combination is also checked directly by
value: a sample that matches no model position in [a, b], but whose reading
matches one with V8 flipped, straddled that V8 change regardless of where the
[a - 2, b] margin places it. Only a V8 change tears a sample this way: an
ordinary line boundary (V8 unchanged) does not combine two different bit-8
values, and is graded normally. No other sample is exempt: a reading that
matches no model position in [a, b] and no position with V8 flipped is not
current, whatever it equals elsewhere in the field.

### 5.4 Anchors

- **A1.** Every phase-A sample in model lines Vrun to L - 1 that is not torn is
  current.
- **A2.** Phase A covers at least two full periods of the model (at least four
  fields). Within the fitted lines of a field V never decreases, and line Vrun
  of each field begins L + 1 model lines after line Vrun of the field before
  (L of that earlier field), so the counter numbers lines 0 to L and returns
  to 0 exactly once per field. u is at most 15 colour clocks, and at least four
  (two bus cycles, 5.2's own floor). The V-increment phase of 5.3 is bracketed
  consistently and is at most 15 colour clocks, the bound u has. Every model
  field has 262 or 263 lines in the
  NTSC runs and 312 or 313 lines in the PAL runs (p.24: "In NTSC, the fields
  are 262, then 263 lines and in PAL, 312, then 313 lines"; p.40: "Each
  complete set of lines (262/NTSC or 312/PAL) is called a display field").
  This confirms that the run is in the standard it is graded as.

  **The u bound.** No page states a maximum for u. p.196's "three 68000
  accesses at two memory cycles each" (the fetch at a, then VPOSR and VHPOSR)
  gives only a minimum, b - a >= 6 colour clocks, and the same page allows
  more: "Some 68000 instructions ... cause cycles to be missed. If cycles are
  missed, the 68000 must wait until its next available memory slot", and
  refresh (p.194, "4 cycles for memory refresh") outranks the 68000 while the
  program keeps it running (section 4). u is instead bounded by what the
  model needs it for. Of the three uses (the width of beam(t), the P3(b)
  window, and the release's uncertainty), the narrowest separation any graded
  check or its contrasting case must make with u is P3(b): a capture 20
  colour clocks away from the main onset must fall outside a window 4 + u
  wide (K16 moves the B7 capture 20 colour clocks earlier, which falls below
  the window whatever u is; the same distance later must fall above it), so
  20 > 4 + u, which gives u <= 15. Every other use separates positions at
  least a line (227 colour clocks) apart, or keeps a mid-line pulse (H
  100-108) inside its line, which needs u < ~115; both hold at u <= 15.

  **Measured u, per core** (fitted phase-A samples, all four configurations):
  fx68k 7 colour clocks; TG68K 7 colour clocks.
- **A3 (refresh).** In every model line of every complete field of phases A,
  B and C -- one whose whole span lies inside the graded window -- there is
  one group of four refresh accesses at the Chip-memory port, at the same
  model H in every line of every one of those fields, with the complete
  line population present (not merely a sample of lines). Fields and lines
  are keyed together (field, line), so the same line number in two
  different fields is never merged into one population. p.194 lists "4
  cycles for memory refresh" per line, and Figure 6-9 on p.195 draws the
  refresh slots at a fixed place at the start of every line. The figure
  says of itself that it "has been 'adjusted'", so only the constancy is
  used, not its clock numbers.
- **A4 (video).** In every complete model field of phases A, B and C, one
  vertical-sync pulse begins -- exactly one, not zero and not more than
  one -- at the same model position in every field.

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
- **Counter lines.** Windows are counted by the counter, whose V changes at
  the phase of 5.3 after the model's line start: line N of field k starts at
  the model's line start plus that phase.
- **Vrun** is 21 in NTSC and 29 in PAL. Every page puts the end of vertical
  blanking before this line (10.1).
- **VB(k):** samples of field k whose [a, b] lies within lines 0 to 19.
- **RUN(k):** samples of field k whose [a, b] lies within lines Vrun to
  L_k - 1 and ends before the onset of the first pin-6 pulse of field k at
  line 32 or later, as the **main-run** schedule places it. Pulses in lines 10
  and 19, and pulses on other pins, do not end RUN. P8 ends RUN(j) at the
  first pin-6 pulse from line 20 on (9 P8).
- **The release.** The freeze of p.239 ends when the counter reaches V = 20.
  The a-model of 5.3 reaches it no later than the counter does and the
  counter reaches it at most u colour clocks later. A sample is **before** the
  release when its whole [a, b] lies before the model's V = 20, **after** it
  when a lies at least u colour clocks after the model's V = 20, and **in the
  release's uncertainty** between the two. Only samples before or after the
  release are graded where the release matters (P8 c, P11, P12).
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
- **Placement.** The schedule gives a pulse's position as the model line and
  H at t_on of the model whose read instants are taken at a (5.3). H_ref(t_on)
  is the same instant in the model whose read instants are taken at b.
- **Scheduling.** The harness computes pulse times from the model of the same
  configuration and CPU core. It may take the model from a first run without
  input changes. The system is deterministic, and nothing the inputs do changes
  the program's timing before the pulse. CP4 checks the placement in the
  graded run.
- **Why this shape.** The light-pen line is named LPENTG* on p.333 and LP* on
  p.381, active low in the manual's usage (p.219: "INT6* becomes a logic
  0"). A low pulse contains both a falling and a rising edge, so the capture
  window [t_on, t_off] covers either edge (10.5).

### 5.7 Microseconds

Phase D (§6), CP2 and P13 give times in milliseconds and microseconds, the
unit of p.240's "up to 300 microseconds". They are counted in colour clocks of
the grid of 5.2 and converted with the colour-clock length the pages give.
p.255: "All timing is done on the basis of a "color clock," which is 279.36ns
long on NTSC machines and 281.94ns on PAL machines." p.141 gives the same
interval, "Clock Interval 0.279365 0.281937 microseconds per interval" (NTSC,
PAL). A duration of t microseconds is the smallest whole number of colour
clocks that lasts at least t at that length: t / 0.27936 in NTSC and
t / 0.28194 in PAL, rounded up. Phase D time 0 is the M3 write at the
Chip-memory port.

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
| B19 | 1 | pin 6 both at (20, 150), the only pulse of the field (the P9 boundary pulse, B19c) |
| B20, B21, until C0 | 1 | none |
| C0 | 0 from line 150 | none (not graded) |
| C1 | 0 | pin 6 both at (100, 100) |
| C2, C3 | 0 | none |
| D | (POTGO = $FF00) | times from M3, counted as in 5.7: 2-4 ms port 1 pin 5 low; 5-7 ms port 2 pin 5 low; 8-10 ms port 1 pin 9 and port 2 pin 9 low; released otherwise, up to 12 ms |

## 7. Common preconditions

Each precondition must hold before any proposition that depends on it is
graded. If one fails, the dependent propositions are reported as not
evaluated, not as green.

| Id | Precondition | Contrasting run |
|---|---|---|
| CP1 | The beam model is anchored on phase A: A1 and A2 of 5.4 hold. CP1 is printed as one line and as its two components, CP1 (A1) and CP1 (A2). | K15 (A2, standard), K17 (A2), K18 (A1) |
| CP2 | Sampling is dense. In phases A to C, consecutive read instants are at most 56 colour clocks apart (a quarter line), and every line of every graded window holds at least two samples. In phase D, consecutive POTGOR samples are at most 50 microseconds apart (5.7). | K5 |
| CP3 | No trigger is presented outside the schedule: pin 6 of both ports stays at the level of an open switch except during the scheduled pin-6 pulses. (a) The CIAAPRA read of §4, made in phase A, has bits 6 and 7 = 1. p.229: "CIAAPRA ($BFE001) Input and output for pin 6 (port 1 and 2 fire buttons)"; p.232: "Port 1 uses bit 6 and port 2 uses bit 7. A logic state of 1 means "switch open."" (b) By the record of every write the run made to the harness's input ports (the schedule's changes and any other write), pin 6 of both ports was at that same level throughout phases A to C, except during the scheduled pin-6 pulses: every interval in which pin 6 of either port was closed lasts exactly 4 colour clocks, and none lies in phase A. The pin is read through the CIA, not through the light-pen latch, so CP3 does not rest on any clause that P1 to P12 grade. | K11 |
| CP4 | Inputs are delivered as scheduled. Each input change in the schedule of the run being made took place, by the harness's own record, at its scheduled model line and H (exactly, at the fixed grid phase), and each pulse lasted 4 colour clocks. No other input changed in phases A to D. For a contrasting run whose own target is a different clause, "the schedule" is its changed schedule, so CP4 stays a satisfied precondition for that clause. In the main runs and in K12, CP4 also compares the cycles on which the phase-B and phase-C changes landed with the cycles of the §6 schedule, computed from the run's model. K12's own target is CP4 itself, so its placement is compared with the unmutated §6 schedule: grading it against its own (already shifted) schedule could never show a placement error. | K12 |
| CP5 | The program ran as specified. M0 to M4 appear in order, and the program reaches DONE (the marker macro's own store of `DONE_VALUE` at `DONE_ADDR`, section 4). The BPLCON0 writes fall between the last read of `wait150` and M1 or M2, at line 150. Coverage: phase A at least two model periods, phase B up to line 40 of B21, phase C up to line 40 of C3, phase D at least 12 ms after M3. A proposition depends only on the part of CP5 for the phases it observes: P1 to P9, P10 (a), P11 and P12 on M0, M1, the phase-B write and the coverage of phases A and B; P10 (b) on M0, M2, the phase-C write and the coverage of phases A and C; P13 on M3, M4 and the coverage of phase D. The `wait150` that ends phase B reads VPOSR with bit 3 set, so it returns only after a release (P8). If it never returns, the phase-B propositions are graded on the stored phase-B samples, P10 (b) is graded on phase A only, and P13 is not evaluated. | K13 |
| CP6 | The beam keeps its timing in phases B and C: A3 and A4 hold there. | K14 |

## 8. Contrasting runs

Each contrasting run is the main run with one change. It is graded **with the
main run's expectations**. For each run, the page predicts a different outcome
on the named checks, which must come out red. The program still finishes, and
the preconditions other than the one a run targets still hold. K1, K2, K6,
K7, K8, K9, K9b, K16, K19 and K20 run in OCS/PAL, not ECS/PAL (§3), and are
also graded under the page model (§3): its own red, through the same named
clause, is the run's result where the raw run cannot turn red on its own.
The patched copies of the chip-set RTL below demonstrate the other direction:
a conforming implementation passes, and one that releases late fails exactly
the release lines.

| Run | Change | What the pages predict | Red |
|---|---|---|---|
| K1 | Phase B writes BPLCON0 = $0000 instead of $0008. | No light pen input and no latch in phase B. p.239: "To enable the light pen input, write a 1 into bit 3 of BPLCON0". p.229: "Bit 3 enables the light pen latch". | P1, P2 (ii), P3, P4, P5, P6, P7 (b), P8 (a), P9 (b)(c), P10 (a), P11, P12 |
| K2 | A pin-6 pulse (both ports) at (100, 100) is added in B2 and in B3. | Captures in B2 and B3, with VB(B3) and VB(B4) holding those positions (p.238 step 4, p.239). | P7 (a)(b), P8 (b), P11, P12 |
| K3 | OCS, not ECS (the ECS record does not cover P10 (b), but ECS leaves LPENDIS's reset value unstated, p.305, so it cannot show this comparator turning red for a page-predicted reason; OCS has no such gap). Phase C writes BPLCON0 = $0008 instead of $0000. | The C1 pulse is captured and the C1 and C2 field ends latch. In the run's own samples every phase-C sample stays current (measured: P10 (b) stays green), so the red is graded through a page-model contrast: this run's own phase-C samples, latched by the page's rule under BPLCON0 = $0008 throughout phase C, against the one function that grades P10 (b), called unchanged on them. | P10 (b) |
| K4 | OCS, not ECS, for the same reason as K3. Phase A writes BPLCON0 = $0008 instead of $0000. | Phase-A field ends latch (p.239: "If no trigger is seen, the counters latch at the end of the display field"), so lines 0 to 19 of the phase-A fields hold the latched value. The latch is released at the end of vertical blanking, before line Vrun (p.239, 10.1), so the fitted lines still run and CP1 holds. Graded through the same page-model contrast as K3, applied to phase A with no pulses: P10 (b)'s function is called unchanged on the synthesized phase-A samples and the run's own phase-C samples. | P10 (b) (phase A) |
| K5 | The phase-B loop gets a delay of at least 300 colour clocks per sample. | Samples more than a line apart. | CP2 |
| K6 | The line-10 pulse of B15 and the line-19 pulse of B17 are moved to line 30 (H = 100). | Line 30 is after vertical blank on every page, so these pulses are captured (p.238 step 1). | P9 (a)(b) |
| K7 | The line-32 pulses of B15 and B17 are moved to line 12. | Inside vertical blank, so they are not captured. | P9 (b) |
| K8 | The first pulse of B14 (line 80) is removed. | The capture happens at (150, 100) instead. | P5 |
| K9 | The real stimulus is unchanged (B18's pin-1 pulse at line 40 is delivered exactly as scheduled); a page-model contrast treats pin 1's own onset as a pin-6 trigger. | Capture at (40, 100) if pin 1 were (mis-)wired to the trigger circuit. | P2 (i) |
| K9b | The pin-6 pulses of B4 (port 1), B5 (port 2) and B18 (both ports) are moved to pin 9 of the same ports, at the same positions. | No capture in B4, B5 or B18: the trigger line is pin 6 (p.238: "the "Beam Trigger" line (pin 6)"), and Table 8-1 (p.228) gives pin 9 of the light pen "button 2**". | P1, P2 (ii) |
| K10 | In phase D, pin 9 is driven low in the windows where the main run drives pin 5 low. | Bits 8 and 12 of POTGOR stay 1 (p.240, Table 8-4: pin 9 is bits 10 and 14). | P13 |
| K10b | In phase D, port 1 pin 5 is held low throughout. | Bit 8 reads 0 in every window. | P13 |
| K11 | Pin 6 of both ports is held closed (low) from the start of the run until one field after M0, then released. Bit 3 is clear throughout, so the light pen latch is not involved. | CIAAPRA bits 6 and 7 read 0 (p.232: "A logic state of 0 means "switch closed.""), and the pin is off the open-switch level outside any scheduled pulse: the record of the run's writes to the input ports shows pin 6 closed from before the run's first cycle until one field after M0, an interval of more than 4 colour clocks and one that lies in phase A. The run goes on to grade P1 to P13 under the failed precondition, so the propositions it gates print as not evaluated. | CP3 (a)(b) |
| K12 | Every pulse is placed one line later than the schedule says. | Placement differs from the schedule. | CP4 |
| K13 | The phase-B buffer is shortened so that phase B ends before the schedule has run out (the schedule's last pulse is in B19). The run prints, per core, the field in which M2 is written. | Coverage missing. | CP5 |
| K14 | Directly after the `wait150` that ends phase B (line 150, after every graded phase-B window), the program writes VHPOSW = $6400 once (line 100, H 0). p.285: "Write vertical and horizontal position of beam or lightpen"; Appendix B p.289: "Write vert and horiz position of beam". | The counter jumps back 50 lines and to H 0. The refresh group, drawn at a fixed place in the line (p.195), and the field boundary no longer sit where the model puts them in phase C. | CP6 (A3 or A4) |
| K15 | The standard of the ECS/PAL run is switched: its harness `Config` selects NTSC, and phase D is timed in NTSC microseconds (the phase-B and phase-C schedule has no standard-dependent pulse). CP1 is graded as PAL, the other preconditions as NTSC; P1 to P13 are then graded under the CP1 result. | Fields of 262 or 263 lines, where the ECS/PAL run's A2 expects 312 or 313 (p.24, p.40). The propositions CP1 gates print as not evaluated. | CP1 (A2) |
| K16 | The B7 pulse is placed at (100, 82) instead of (100, 102), and the B11 pulse at (39, 100) instead of (40, 100). P3 is graded with the main schedule's onsets. Both captures precede the main onsets, so POST(B7) and POST(B11) hold the moved value throughout. | Captures at (100, 82) and (39, 100): the value of the counter "at the instant the beam passed the light pen" (p.238). | P3 (a) for B11; P3 (b) for B7 (the captured H lies 20 colour clocks below H_ref, outside the window [H_ref, H_ref + 4 + u]); P3 (c) for B7 (the changed member differs from the common offset by 20); the combined P3 assertion through all three |
| K17 | In OCS/PAL (§3). As K4, and a pin-6 pulse (both ports) at (200, 100) is added in every phase-A field, each delivered when its cycle is reached (the positions come from the model of a first pass of the program without input changes). | Each pulse is captured and VPOSR stays frozen until the end of vertical blanking (p.238 step 4, p.239), so the phase-A samples of lines 200 to L - 1 are not current and the largest V read in each field is 200, not L. Line 200 comes after the line 150 that `wait150` waits for, so the program still finishes. The contrast latches the run's phase-A samples at the delivered onsets, requires L = 200 of the fit on them, and grades them with CP1. | CP1 (A2): the fitted L is 200, not 312 or 313 |
| K18 | In OCS/PAL (§3). As K4, and one pin-6 pulse (both ports) at (Vrun, 100), line 29, in the third phase-A field. | The pulse is captured and VPOSR stays frozen until the end of vertical blanking, so the samples after the pulse, in lines 29 to L - 1 of that field, read the captured position and are not current. The captured line is the first fitted line, so V does not decrease within the fitted lines of a field and CP1 (A2) stays green. | CP1 (A1) |
| K19 | Both pin-6 pulses of B4 (port 1) and B5 (port 2) are placed at (100, 80) instead of (100, 100). P1 is graded with the main schedule's onsets. | The pulses are captured at (100, 80), 20 colour clocks before the onset P3 (b) requires, outside the position P1 requires (P3 (a) and (b)); every precondition holds and the program finishes. | P1 (the position clause) |
| K20 | The pin-6 pulse of B19c is placed at (19, 150) instead of (20, 150). P9 (boundary) is graded with the main schedule's onset. | Line 19 is inside vertical blanking, before the release at vertical position 20 (p.238 step 1, p.239): the pulse is not captured. | P9 (boundary), P9 (release) |
| FIXTURE | The program stores a different value where M0 belongs (`prog_fixture.s`) and continues through M1-M4 and DONE. | The M0 marker is not the known constant. | FIXTURE (M0) |
| FIXTURE-DONE | The program hangs after M4 without storing `DONE_VALUE` (`prog_nodone.s`). | DONE is not reached. | FIXTURE (DONE) |
| PIN9CTRL | Phase D omits the 8-10 ms pin-9 low drive. | POTGOR bits 10 and 14 stay 1 in that window (p.240, Table 8-4). | P13 (pin 9 control) |

**Conforming implementation.** Every main run is also made against a copy of
the chip-set RTL: `latch.patch` applied to the pinned submodule's
`minimig-aga` directory by `make latch-test-fx68k` and `make
latch-test-tg68k`. The copy implements the pages' reading of the latch:
BPLCON0 bit 3 enables it; the counter reaching V = 20 releases any freeze and
arms the capture; a falling edge of the trigger input (pin 6 of either port)
while armed captures VPOSR and VHPOSR; an end of field with nothing latched
latches the counters; reads return the latched words. LPENDIS is not
modelled, so an ECS configuration behaves as OCS. A graded line that is red
on this implementation is a defect of the test, not a finding. A patched run
is an implementation demonstration, not a stimulus mutation: it changes the
implementation, never the stimulus or a comparator. The page-model lines are
printed on it as on any RTL; they apply the page's latch to samples the
implementation has already latched and are not part of what it demonstrates.

**Late release.** `late.patch` (`make late-test-fx68k`, `make
late-test-tg68k`) is `latch.patch` with the release and the arming one line
late: the counter entering V = 21 in place of V = 20. The lines that grade
the release at vertical position 20 (P8, P8 (release-at-20), P9 (boundary),
P9 (release) and P11) go red on it, and no other graded line.

---

## 9. Propositions

### P1 — A light pen is accepted on one of the controller ports

1. **Proposition.** A pulse on pin 6 of at least one of the two controller
   ports is captured as a light-pen trigger. Which port it is, the pages leave
   open.
   Claims: opus:8-lpen.c1, sol:8-pen.c1 (p.238); opus:8-intro.c12,
   sol:8-overview.c6 (p.227).
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
     names (10.4). opus:8-intro.c2, whose `says` reads it that way, is
     declined (§1).
5. **Preconditions.** CP1 to CP6.
6. **Contrasting case.** K9b: both pulses are on pin 9, the page predicts no
   capture, so the check is red through the trigger pin. K1: neither pulse is
   captured, so the check is red. K19: both pulses are placed at (100, 80), so
   the page predicts captures at (100, 80), which P1 grades against the main
   onsets and finds outside the position P3 (a) and (b) require: the position
   clause is red.
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
   - (i) Every sample of RUN(B18) is current: none of the driven other-pin
     pulses is captured. If one were, the line of the latched value would
     name the pin.
   - (ii) The pin-6 pulse is captured (P3 (a) and (b)).
   - The combined P2 assertion is (i) and (ii) together, carrying the four
     claim ids; (i) and (ii) are also graded on their own component lines,
     neither of which is independently sufficient for an id.
   - Quantifier. "The Beam Trigger line (pin 6)" makes pin 6 the only trigger,
     so all six other signal pins are driven. Pins 7 (+5V) and 8 (ground)
     cannot be driven. Port-2 pin 5 (POTGOR bit 12) is not one of the pins
     this harness can drive (section 3, "Port mapping"): (i) is graded over
     the driven population only (pins 1-4 and 9 on both ports, pin 5 on
     port 1), and port-2 pin 5 is recorded separately, without claim ids, as
     not established: the driven pulses establish the claims whole.
5. **Preconditions.** CP1 to CP6. By CP4, the driven pulses were delivered on
   both ports (pin 5 on port 1 only) at their lines.
6. **Contrasting case.** K9 preserves the real pin-1 delivery and grades a
   page-model contrast that treats pin 1's own timing as a pin-6 trigger,
   failing (i) through pin identity rather than through an added valid
   trigger. K9b moves the line-160 pulse to pin 9 and predicts no capture,
   so (ii) is red through the pin. K1 predicts no capture, so (ii) is red
   too.
7. **Reference frame.** 5.3 to 5.6.

### P3 — A trigger captures the beam position exactly at its instant

1. **Proposition.** With the light pen input enabled, a pulse on the trigger
   line captures the beam counter's vertical and horizontal value at the
   instant of the pulse, exactly.
   Claims: opus:8-lpen.c17, opus:8-lpen.c18, sol:8-pen.c18 (general: neither
   names an axis) need (a), (b) and (c) together; sol:8-pen.c19 ("the
   horizontal beam value at the instant", axis-specific) needs (b) and (c)
   together. These four ids therefore sit on one combined P3 assertion
   ((a) and (b) and (c)), not on the separate component lines below.
   sol:8-pen.c20 ("the vertical beam value at the instant", axis-specific)
   is independently sufficient from (a) alone and stays on that component
   line; (b) and (c) have no id that either establishes alone (p.238).
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
   - (b) H(P) lies in [H_ref(t_on), H_ref(t_on) + 4 + u]. The page states no
     delay between pulse and capture and does not say which edge of the pulse
     counts: the capture instant lies in [t_on, t_off], and the counter's
     position at any time lies in [H_ref, H_ref + u] (5.3), so the window is
     the pulse's own duration plus the model uncertainty u of 5.2. It is not
     a tolerance granted to the implementation.
   - (c) **Exactness.** The offset d(p) = H(P) - H_ref(t_on) is the same for
     every graded pulse, with a spread of 0, across all lines and all H steps.
     A shift of the onset by k colour clocks must shift the captured H by
     exactly k, to the register's stated resolution (p.239: "Horizontal 2 low
     resolution pixels", which is one H unit). The model's constant reference
     error cancels in (c).
   - Quantifier: every graded pulse contributes one valid offset. A pulse
     whose POST is empty or not constant (so (a) and (b) already fail it)
     does not merely drop out of the offset population silently: the
     population falling short of the graded count is itself part of (c)'s
     failure, not something (c) can pass around.
5. **Preconditions.** CP1 to CP6. By CP4, the H sweep and the lines were placed
   as scheduled.
6. **Contrasting case.**
   - K16: the page predicts captures at (39, 100) in B11 and (100, 82) in B7.
     Both precede the main onsets, so POST(B11) and POST(B7) hold the moved
     capture and are constant. Graded with the main schedule's onsets, (a) is
     red for B11. For B7 the captured H is 20 colour clocks before H_ref of
     the main onset, below the window of (b), and 20 away from the common
     offset of (c); so (b) and (c) are red, each through the captured value.
   - K1: the page predicts no capture. POST(p) holds current samples, so (a),
     (b) and (c) are red.
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
   - VHPOSR bits 7-0 are H(P) of P3, graded here against H_ref(t_on) plus
     P3(c)'s own offset (the same exactness the H window of P3(b) uses), not
     just against the sample's own repeated value. Where P3(c) has no
     validated offset, this half of the check fails rather than passing
     unconditionally: H8-H1 is part of the p.238 register layout and the P4
     label, and its absence from grading is never itself a pass.
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
     p.229 leaves which controller port carries the pen to the model (§3,
     "Port mapping"): B4 and B5 are single-port pulses, and only the one P1
     found capturing on at least one port is graded here, reported
     alongside; both are still graded for every other case.
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
     input change. B16 also has none; its end is left out for the reason
     given in P7.
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
   - Quantifier ("impossible"): every sample of every window. As in P5, B4
     and B5 are graded only for the port P1 found capturing.
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
2. **Stimulus.** Fields B1, B2, B3 and B20. They have no input change, lie
   wholly under bit 3 = 1, and the vertical blank after each of them has no
   input change either. B16 also has no input change, but VB(B17) holds the
   line-19 pulse that P9 grades, so a red there could come through P9's
   clause; its end is not graded here.
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
     field k. (c)'s population is "RUN(k) and VB(k+1) both non-empty for a
     no-trigger field k", counted whether or not (b) itself held for that
     field: a field where VB(k+1) is not one consistent value has a
     population but no single F_k to compare, so it is reported as not
     evaluated (because (b) failed), never folded into an empty-population
     record that is not true.
   - The page gives no H for "the end of the display field". The samples of
     line L_k are not graded, and H(F_k) is graded only through (c).
   - How F_k compares with $10500 is graded in P11 and P12.
   - Quantifier: every no-trigger field end whose following vertical blank
     has no input change, four per run.
5. **Preconditions.** CP1 to CP6. By CP3 and CP4, pin 6 stays at the
   open-switch level through these fields, so no trigger is seen in them.
6. **Contrasting case.** K2: the page predicts pen captures in B2 and B3. RUN
   of those fields stops being current from line 100, and VB(B3) and VB(B4)
   hold line-100 positions, so (a) and (b) are red. K1: no latch, so (b) is red
   for all four fields. (c) has no contrasting input: every latch the pages
   describe comes after the last current read of its field, so no input makes
   them predict m(F_k) < m(R_k). (c) is unfalsifiable by stimulus at this
   boundary.
7. **Reference frame.** 5.3 to 5.5 (L_k, field boundaries).

### P8 — The freeze is released at the end of internal vertical blanking

1. **Proposition.** Every latch holds until the end of internal vertical
   blanking and no longer. After that, VPOSR shows the current position again.
   Claims: opus:8-lpen.c39, sol:8-pen.c39 (p.239). sol:8-pen.c39 fixes the
   release at "vertical position 20"; p.239 alone speaks of the light-pen
   latch and calls its blanking "internal" without restricting "20" to one
   standard, unlike p.219/p.79's general video/display blanking (10.1), so
   it is graded through (c) below rather than declined.
2. **Stimulus.** Phase B. Every latched window ending in fields B2 to B21, from
   captures as well as from no-trigger field ends, and B16's field end (whose
   own latch P7 leaves out only because VB(B17) also holds the line-19 pulse
   P9 grades). Whichever port P1 finds, the ends of B4 and B5 are latched
   either way -- and are graded either way too: the port P1 did not find
   still sees no successful trigger that field, so P7's own field-end latch
   applies to it instead, and (a)'s internal-consistency half (VB(j) is one
   value) is required regardless of which mechanism latched it.
3. **Observation.** For each such field j = k+1 (including j = 20, following
   B19c's capture):
   - VB(j);
   - RUN(j), from Vrun to the first pin-6 pulse of j from the release line on
     (line >= 20, not just 5.5's narrower "graded" population of line >= 32
     pulses in fields B4-B18), or to L_j - 1 if there is none. j = 19 (after
     B18's capture) has one such pulse, B19c (line 20, the P9 boundary pulse),
     which ends RUN(19) at line 20. Where such an early pulse preempts the
     whole window (its onset at or before Vrun), RUN(j) is empty and (b) is
     satisfied vacuously: the field has no unlatched span left to grade, not
     missing data.
   - (c) the samples of VB(j), and the samples of model line 20 of field j
     that are after the release (5.5) and end before the first pin-6 pulse of
     j from line 20 on.
4. **Expected.**
   - The page. p.239: "This freeze is released at the end of internal vertical
     blanking (vertical position 20)."
   - (a) Every sample of VB(j), lines 0 to 19, has one and the same value.
     Where the value is also known (a capture's own POST(p), P5), it equals
     that value too; for VB(B5) and VB(B6) it may be either kind, depending
     on P1's port. Positions are compared (10.8); torn samples are left out
     of the comparison.
   - (b) Every sample of RUN(j) is current.
   - (c, sol:8-pen.c39) The held value is still visible before line 20 (no
     sample of VB(j) is current) and the live counter is visible from the
     stated line-20 release (every sample of line 20 of field j that is after
     the release is current). Both sets must be non-empty. The release is the
     counter's V = 20, which the model reaches at the V-increment phase after
     the line's start and the counter at most u later (5.5): samples in
     between are not graded. p.239's "vertical position 20" is graded here at
     line resolution, independent of standard, unlike (b)'s Vrun-based span.
   - Not graded by (a)/(b): NTSC line 20 and PAL lines 20 to 28. p.219 ends
     vertical blanking at line 25 in PAL, and p.79 stops it at $15 (21) in
     NTSC and $1D (29) in PAL (10.1).
   - Reported for each window: the model line and H of the first sample that
     matches the live beam (not merely the first sample of the window).
     p.239 predicts line 20.
   - Quantifier: every window, 20 per run (including B19c's).
5. **Preconditions.** CP1 to CP6.
6. **Contrasting case.** K1: no latch, so (a) and (c) are red. K2: RUN(B2) and
   RUN(B3) are latched from line 100 on, so (b) is red.
7. **Reference frame.** 5.3 to 5.5; Vrun from 10.1.

### P9 — The capture circuitry re-arms by itself as vertical blank ends

1. **Proposition.** At the end of vertical blank the capture circuitry is
   enabled again without any action by the program. A pulse after that point
   is captured, and a pulse inside vertical blank is not held over the
   release.
   Claims: opus:8-lpen.c12, sol:8-pen.c12 (p.238 step 1). "Just as" needs
   both halves, nothing held over from before the release (a) and a pulse at
   the release captured (d), so the two ids sit on one aggregate, P9
   (release), and on neither component line. P8 (c) establishes V = 20 as the
   release line.
2. **Stimulus.**
   - B15: pin-6 pulses at (10, 100) and (32, 100), after B14's capture.
   - B17: pin-6 pulses at (19, 100) and (32, 100), after the no-trigger field
     B16.
   - B19: one pin-6 pulse at (20, 150), sitting exactly at the stated
     release line and well before Vrun. It is the only pulse of its field:
     not B21 (whose own run region is one of P8's no-trigger currency
     checks) and not a second pulse in B15/B17 (where a later pulse in the
     same field would not change the frozen value, P5, and so would not
     show its own capture).
   - The phase-B loop writes no register (§4).
3. **Observation.**
   - RUN(B15) and RUN(B17), from Vrun up to the line-32 pulse;
   - POST of the two line-32 pulses;
   - the POST of every graded capture of B6 to B18;
   - (d) the samples from the line-20 pulse's own release to line 200 of the
     same field (B19).
4. **Expected.**
   - The page. p.238: "Just as the system exits vertical blank, the capture
     circuitry for the light pen is automatically enabled."
   - (a) No hold-over past the release. Every sample of RUN(B15) and
     RUN(B17) is current, so neither the line-10 nor the line-19 pulse is held
     over the release. (a) grades only that. Every vertical blank of phase B
     is already latched (p.239), so a capture inside it that does not
     overwrite the frozen value leaves nothing at the ports. An overwrite is
     graded in P5 (the line-10 pulse of B15) and P8 (a) (the line-19 pulse of
     B17).
   - (b) A pulse after vertical blank is captured without software action. In
     B15 and in B17, POST of the line-32 pulse holds that pulse's position
     (P3 (a) and (b)).
   - (c) The re-arming repeats in every field. Every graded pin-6 pulse of B6
     to B18 is captured, although the program wrote nothing after the phase-B
     enable.
   - (d) A pulse exactly at the stated release line is captured -- the first
     eligible position, once P8 establishes V = 20 as the release line. The
     window is one held value from that pulse's own release to line 200 of
     the same field.
   - P9 (release), opus:8-lpen.c12 and sol:8-pen.c12: (a) and (d) together.
     P9 (a) and P9 (boundary), the component lines of (a) and (d), carry no
     id; the combined P9 assertion of (a), (b) and (c) carries none either.
   - "Just as" is graded, through (a) and (d), at the one line the pages agree on for
     the light-pen-specific latch (p.239's own "vertical position 20", not
     tied to a standard, 10.1); lines 10 and 19 remain inside vertical blank
     on every page, and line 32 remains after it on every page.
5. **Preconditions.** CP1 to CP6. The builder asserts from the program listing
   that the phase-B loop contains no write to a custom register.
6. **Contrasting case.**
   - K6: the page predicts a capture at line 30. RUN is latched from line 30
     and the captured value is not the line-32 position, so (a) and (b) are
     red.
   - K7: the page predicts no capture at line 12. POST shows current samples,
     so (b) is red.
   - K1: the page predicts no capture in phase B, so (c) and (d) are red.
   - K20: B19c is placed at (19, 150), inside vertical blank, and the page
     predicts no capture, so (d) is red and (a), (b) and (c) are not.
7. **Reference frame.** 5.3 to 5.6; Vrun from 10.1.

### P10 — Bit 3 of BPLCON0 enables the light pen input and latch

1. **Proposition.** With bit 3 of BPLCON0 set, pulses are captured and fields
   without a trigger latch at their end. With bit 3 clear, neither happens.
   Claims: opus:8-lpen.c35, sol:8-pen.c35 (p.239); opus:8-regs.c16,
   sol:8-reg.c18 (p.229). These enabling claims sit on (a) only: (b) is the
   control that a design latching nothing at all also satisfies, and carries
   no id.
2. **Stimulus.**
   - BPLCON0 = $0008 in phase B and $0000 in phases A and C. The two words
     differ only in bit 3 and are whole-word writes (p.89).
   - Phase C: C1 has a pin-6 pulse (both ports) at (100, 100); C2 and C3 have
     no input change.
3. **Observation.**
   - Phase A: VB(k) of every phase-A field k whose preceding field end comes
     after M0. These lines are left out of the model fit and of A1 (5.3), so
     they are graded here and not assumed by the anchor.
   - Phase B: the P3 and P7 windows.
   - Phase C: C1 from Vrun to L - 1, including after the pulse; all lines of C2
     (0 to L - 1); VB(C3).
4. **Expected.**
   - The pages. p.239: "To enable the light pen input, write a 1 into bit 3 of
     BPLCON0." p.229: "BPLCON0 ($DFF100) Bit 3 enables the light pen latch".
     p.271: "LPEN =Light pen enable (reset on power up)".
   - (a) Bit 3 = 1: every graded pin-6 pulse is captured (P3 (a)), and every
     no-trigger field end latches (P7 (b)).
   - (b) Bit 3 = 0: every sample of the phase-A and phase-C windows is
     current. No phase-A field end latches, the C1 pulse is not captured, and
     neither the C1 nor the C2 field end latches.
   - Not graded: B0, C0 and VB(C1). These are the fields in which bit 3
     changes, and the field end they reach. The pages do not say when a change
     of bit 3 takes effect relative to the automatic enabling at the end of
     vertical blank (p.238 step 1).
5. **Preconditions.** CP1 to CP6. By CP5, the BPLCON0 writes are bracketed by
   markers.
6. **Contrasting case.** K1 makes (a) red. K3 makes (b) red in phase C, and
   K4 in phase A.
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
     VB(B6) and VB(B20). These windows depend on a field that is not graded,
     on P1's port, or on the ungraded boundary pulse B19c.
   - Pairs with a sample in the release's uncertainty (5.5) are excluded: such
     a sample may show either side of the release.
   - The truth T(i) for "the light pen has triggered since the last
     top-of-screen": a graded pin-6 pulse lies between the model's V = 20 last
     before a of sample i (the previous field's, if sample i is itself before
     the release of its own field) and a of sample i, and the
     freeze that pulse set is still in place at sample i+1 too -- step 2 can
     only read "the same" while both reads see the same frozen value, so a
     pair spanning the release (sample i still latched, sample i+1 already
     past line 20) reads "not the same" whether or not a trigger occurred
     earlier, and T(i) is graded against what the method can show, not
     against the trigger alone.
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
   - All graded captures are below line 261; for a capture at line 261 or
     later step 4 contradicts p.238 step 4 (10.2).
   - Quantifier: every graded pair.
5. **Preconditions.** CP1 to CP6. By CP4 and the schedule, both classes occur:
   pairs with T true (after 13 or more captures) and latched equal pairs with T
   false (the ends of the no-trigger fields B1, B2, B3, B16 and B20).
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
     page's "0<V20", 10.3) and before the model's V = 20, of fields B2, B3,
     B4, B7 to B19 and B21.
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
   after captures (B7 to B16, B18, B19) and after no-trigger fields (B2, B3,
   B4, B17, B21).
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
     and 1 in every sample of every other graded window. This half, alone,
     carries the four claim ids: port 1 pin 5 is driven both low and idle, so
     both halves of the proposition are established for it (P13(port1)).
   - Bit 12: no input this harness exposes reaches port 2 pin 5 (§3), so its
     active-low half is never exercised; only its idle-high half is measured
     (always 1 in every graded window). The ids are not claimed through this
     half: p.238's "regardless of the port used" does not by itself
     establish the active-low case for a port this harness cannot drive.
   - In the pin-9 window, both bits stay 1 (P13(pin9-control), no id: a
     control confirming bits 10/14, not 8/12, track pin 9).
   - Other bits are not graded: bits 10 and 14 are the right button, and
     bits 7-1 are the "chip revision identification number".
   - Both ports would be graded, per p.238's "Regardless of the port used,
     the light pen design is the same", if both were drivable; only port 1
     is (§3, "Port mapping").
5. **Preconditions.** CP5 (POTGO written before M3, phase D coverage), CP2
   (phase D sample spacing), CP4 (level changes delivered as scheduled).
6. **Contrasting case.** K10: pin 9 instead of pin 5, so the page predicts bit
   8 stays 1, and the check is red. K10b: pin 5 held low throughout, so
   bit 8 is 0 where 1 is expected, and the check is red. PIN9CTRL: the 8-10ms
   pin-9 low drive is omitted, so the page predicts bits 10 and 14 stay 1
   where P13(pin9-control) expects 0.
7. **Reference frame.** Time from M3 at the Chip-memory port, in microseconds
   as 5.7 counts them; the input record of the harness.

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
     or 29 and report the line where it actually happens. sol:8-pen.c39, whose
     `says` fixes the release at line 20, is graded through P8(c) and P9(d):
     since p.239 alone (not p.219 or p.79) speaks of this light-pen-specific
     internal latch, and does not restrict "20" to one standard, the release
     is graded at line 20 in both PAL and NTSC, independent of Vrun.
2. **The threshold $10500 (V=261) against captures at or below line 261.**
   - p.239 step 4 answers "not triggered" for any value above $10500.
   - p.238 step 4 says the captured value is the pen's exact position. That
     includes lines 261 to 311 in PAL (p.216: "Allows PAL line counts (313)")
     and NTSC line 261 with H > 0, a displayable line (p.79: 241 displayable
     lines after 21 blanked).
   - For such a pen the two statements contradict each other. P11 and P12 grade
     the method only for captures below line 261.
3. **"0<V20".** The printed page lacks a relation sign in both places
   ("internal vertical blanking period of 0<V20"; "during the period of
   0<V20"). P12 grades lines 1 to 19, the part common to every reading.
4. **Which port.**
   - p.238: port 1 on the A1000; port 2 by default on the A500, A2000 and
     A3000, with a jumper for port 1.
   - p.227: "Two multiple purpose mouse/joystick/light pen control ports".
   - The system under test is none of those models. P1 grades "at least one
     port" and reports which. opus:8-intro.c2, whose `says` reads p.227 as
     each port, is declined (§1).
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
     a difference lies only in bit 15. P6 to P8 compare positions, and leave
     torn samples out of the comparison.
9. **Instant of the end-of-field latch.** "At the end of the display field"
   gives no H. P7 grades it at line resolution plus the ordering F >= R.
10. **When a change of bit 3 takes effect** relative to the automatic enabling
    at the end of vertical blank (p.238 step 1): not stated. The fields in
    which bit 3 changes (B0, C0) and the field end they reach are not graded.
11. **The value exactly $10500** is neither "greater" nor "less". The page
    gives no answer for it, and it is not graded.
12. **ECS LPENDIS reset value:** not stated (p.305). BEAMCON0 is not written
    (§3). An ECS run whose phase B shows no latch at all is recorded with
    LPENDIS as the open point, not graded red on the latch propositions (§3).
13. **Text layer against print, no effect on expectations.**
    - p.239: the text layer adds "(LPEN)", which is not printed.
    - p.238: the print reads "the right or middle button on a mouse".
    - p.240: the print names the read address POTINP, where the text layer has
      POTGOR.
    - p.228: the printed Table 8-1 heads its first column "Pen" instead of
      "Pin".
14. **Figure 6-9 (p.195)** says it "has been 'adjusted'". The refresh anchor A3
    uses only the constancy of the refresh position, not its clock numbers.
