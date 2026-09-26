# Test specification: AA Blitter pointer and modulo registers, and BEAMCON0 (package aa-s2)

This document specifies an integration test package that checks an FPGA
re-implementation of the Amiga chip set against Commodore's *Functional
Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91. It was written
from the package's claims (`claims-spec-stage.jsonl`) and the specification's
page text alone, with the Amiga Hardware Reference Manual, 3rd edition,
consulted only where the AA specification builds on it. Its author never saw
the implementation or any test. "p.N" is a page of the AA specification as the
corpus numbers it (`pNN.txt`, the PDF page); "HRM p.N" is a printed page of the
Hardware Reference Manual. Every expected value comes from an AA page. HRM pages
are cited only for context and for procedures the AA pages take for granted,
never as the source of an expected value.

The package has 81 claims. 66 of them support the 14 propositions P1-P14 in
section 3. The other 15 cannot be turned into a proposition; section 1 lists
them with the reason for each. Section 5 maps every claim id to where it is
used.

---

## 1. Claims not turned into a proposition

| Claim ids | Reason |
|---|---|
| opus:AA-bltxmod.c3, opus:AA-bltxpth.c3, opus:AA-bltxptl.c3, sol:AA-bltxmod.c3, sol:AA-bltxpth.c3, sol:AA-bltxptl.c3 | The chip column "A" (p.13; legend p.6 and p.10 "A=Agnus/Alice chip") says which chip holds the register. No output a test can see shows which chip a register lives in. A register at the right address that has the right effect is equally correct in any chip. |
| opus:AA-bltxpth.c4, opus:AA-bltxptl.c7 | These record revision marks. p.13 marks `BLTxPTH  h`, but the p.7 rows `BLTAPTH`...`BLTDPTH` carry no h, `BLTxPTL` carries none, and the two legends define h differently (p.6 "h=new for HiRes chip set", p.10 "h= stuff added or changed in hires chips"). A revision mark is a statement about the document's history, and no output shows it. Both legend readings place the marked feature in the chip set under test. That feature is the 5-bit high word (p.6: "PTL,PTH=20 bit Pointer ... (old chips- 18 bits)"), and P2 tests it from other claims. |
| opus:AA-beamcon0.b15, sol:AA-beamcon0.c6 | "15     (unused)" (p.12). The page gives the bit no function and no value. A test must not invent one: neither "writing it has no effect" nor any read-back value is stated. |
| opus:AA-beamcon0.b3, sol:AA-beamcon0.c18 | "3     (unused, formerly BLANKEN)" (p.12). This gives no function and no value, for the same reason. p.29 still refers to BLANKEN as a live bit (section 4, item 4). |
| opus:AA-beamcon0.c37, sol:AA-beamcon0.c54 | "VARCSYEN= enable CSY* from the variable decoders to come out the CSY*" (p.13). The CSY* pin is not among the outputs a test can see: the harness shows 4-bit RGB, horizontal sync and vertical sync only. |
| sol:AA-beamcon0.c50 | "(It doesn't work in 640*400*4 interlace mode)" (p.12). The page does not say what "doesn't work" looks like, so no observation can be graded against it. |
| opus:AA-beamcon0.c18 (the first half only) | "The normal monitor uses the normal composite sync". This is a statement about the CSY* pin, which cannot be seen. The rest of the claim ("the variable composite sync & blank come out the HSY* & VSY* pins") is used in P9. |

Several claims are tagged CONFLICT but are used all the same:

- **The `~` mark:** opus:AA-bltxmod.c4, opus:AA-bltxpth.c6 and
  opus:AA-bltxptl.c5 are used in P7.
- **The `+` mark:** opus:AA-bltxpth.c5 and opus:AA-bltxptl.c4 are used in P2.

In both cases the "conflict" is that the p.7 address list prints the mark and
the p.13 alphabetical entry does not. The alphabetical list has no column in
which it could print it. Its header on p.10 is
`NAME  rev ADDR type  chip  Desription`, and its legend (p.10) defines no `~`,
`+` or `&`. Its silence is therefore not a denial. Where the alphabetical entry
does speak, it agrees with the mark:

- For `+`, it has "High 5 bits", "Low 15 bits" and "This pair of registers
  contains the 20 bit address".
- For `~`, the COPCON entry on p.19-20 agrees.

---

## 2. Setup shared by all propositions

### 2.1 System

- **Configuration:** `Config.chipset_config = 0x10` (AA selected), PAL, 2 MB
  of Chip memory. The test reads these from the harness configuration and
  asserts them before every run. P2 depends on the 2 MB.
- **CPU cores:** every run is executed on both CPU cores, with the second core
  left in its default 68000 mode. The program uses only 68000 instructions.
  Every graded relation must hold on both cores. Parameters that depend on CPU
  speed (sample counts, pulse position in P8) are fitted per core from a dry
  run, under the constraints stated where they are used.
- **Register addresses:** a custom register is at `$DFF000` plus the offset
  printed on pp.6-10.

### 2.2 Program conventions

- **Markers.** The program announces events by writing a word, the event code,
  to a marker area in Chip memory. The port shows each such write with its
  time. Custom-register reads and writes are not Chip-memory accesses, so they
  carry no time at the port. Every "time of X" in this document is the time of
  the marker the program writes immediately after or before X, as stated.
- **Results area.** Values the program reads from registers (DMACONR, INTREQR,
  beam samples) are stored in a results area for the test to check.
- **Initial state for every run:**
  - `DMACON = $7FFF`: bit 15 SET/CLR = 0 clears every enable (p.22).
  - `INTENA = $7FFF`.
  - `INTREQ = $7FFF`: clears every request.
  - `BPLCON0 = $0000`, so `ERSY` (bit 1) = 0 and the sync pads are outputs
    (p.17 "ERSY =External Resync (HSYNC, VSYNC pads become inputs)").
  - `COPCON = $0000`.
  - `BEAMCON0` is written explicitly in every run: `$0020` (PAL only) unless
    the proposition says otherwise. The page states no reset value for
    BEAMCON0 except for HARDDIS ("It is cleared upon reset", p.12), so none is
    assumed.
- **Waiting with a timeout.** Every wait for an event polls a request or status
  bit. It gives up after the vertical-blank request (INTREQR bit 5 VERTB, p.25
  "05 VERTB ... Start of Vertical blank") has been seen set three times, each
  time cleared with `INTREQ = $0020`. The outcome (seen or timed out) is stored.
  This keeps every contrasting case running to the end of the program.
- **Frame sync.** The program waits for VERTB, clears it and writes a marker.

### 2.3 Blit template (used by P1-P7)

This order follows p.14: "Writing to this register will start the Blitter, and
should be done last, after all pointers and control registers have been
initialized."

1. Set `DMACON = $8240` (SET, DMAEN bit 9, BLTEN bit 6; p.22).
2. Write the blitter registers:
   - `BLTAFWM = BLTALWM = $FFFF` (p.13: a zero in these masks overrides
     source A data).
   - `BLTCON1 = $0000`: area mode (bit 0 LINE = 0), DESC = 0 (bit 1), no fill
     (p.14).
   - `BLTCON0` = the channel-enable bits and minterm given in each run, with
     ASH = 0. The bits are 11 USEA, 10 USEB, 9 USEC, 8 USED, 7-0 LF (p.14).
   - The pointer registers, each as two word writes, PTH before PTL.
   - The modulo registers.

   All values written must leave unused bits at zero. The page defines bits
   4-0 of each PTH only ("High 5 bits"), so bits 15-5 of PTH are 0.
3. Clear the BLIT request (`INTREQ = $0040`).
4. Write marker `BLIT_GO`.
5. Write `BLTSIZE = (H << 6) | W`. p.14: bits 15-6 are the height `h9-h0`,
   bits 5-0 the width `w5-w0` in words.
6. Poll INTREQR bit 6 (BLIT, p.25 "06 BLIT 3 Blitter finished"), with the
   timeout of 2.2.
7. Read `DMACONR` and store it. Bit 14 BBUSY must read 0 (p.22).
8. Write marker `BLIT_DONE`.

The minterm values used below (`$F0`: D=A, `$CC`: D=B, `$AA`: D=C, `$FF`: D=1)
follow the minterm numbering of HRM chapter 6. p.14 names LF7-0 only as "Logic
function minterm select lines". The data the blitter writes is never graded,
so the minterm only has to make the blit run.

### 2.4 Reference frames and their anchors

**F1: attributing Chip-memory accesses to Blitter channels.**

- **Regions.** Every Blitter channel `x` used in a run gets its own address
  region `R_x`. The regions are disjoint from each other and from:
  - the program image, the stack, the marker area and the results area;
  - the Copper lists.
- **Attribution.** An access of channel `x` is a port access whose word
  address lies in `R_x` and whose time lies between `BLIT_GO` and `BLIT_DONE`.
- **Direction.** For `x` = A, B, C the accesses must be reads; for D, writes.
  This is part of P1.
- **Anchor check.** Run the same program with step 1 of 2.3 changed to
  `DMACON = $8200`, which is DMAEN without BLTEN. In that run no access may fall
  in any `R_x` in the same marker window. p.22 "06 BLTEN Blitter DMA enable": a
  Blitter without DMA makes no memory cycle.

  If the control run shows an access in a region, the region collides with
  some other traffic (for example refresh) and must be moved. Moving a region
  is the only change a builder may make to the region addresses given below,
  and it must keep every bit pattern the proposition relies on.

**F2: blit lines.** A blit has width `W` words and height `H` lines.

- **Lines.** Channel `x`'s accesses, in time order `a_0 .. a_{WH-1}`, form
  lines: line `k` is `a_{kW} .. a_{kW+W-1}`.
- **Anchor check.** Every enabled channel shows exactly `W*H` accesses.
- **Increment.** Within every line, consecutive addresses must differ by one
  constant step `inc`, measured in the same run. This `inc` is the "increment"
  of p.13, whose size that page does not state. HRM p.172 says one word. Only
  its constancy is a precondition here; its size is not graded.

**F3: colour clock and port line.**

- **Colour clock.** `T_C` is the duration of one Chip-memory slot in system
  clocks, as the harness numbers slots. p.21: "(memory cycles are 280 ns)",
  which is one colour clock (CCK).
- **Port line.** p.29 says "There are 4 refresh time slots" in every line.
  In a stretch where all DMA enables are clear and the CPU is halted, every
  port access is a refresh access. The CPU is halted by a 68000
  `STOP #$2000`, woken by the level-3 vertical-blank interrupt. For that, the
  program writes the level-3 autovector at `$6C` and a handler that clears
  VERTB and returns.

  If the program cannot own the vector, refresh accesses are instead the
  accesses outside every address range the program uses.
- **Line start and length.** A port line starts at the first access of each
  group of four. `T_L` is the time between consecutive group starts, and
  `L = T_L / T_C` is the line length in CCK.
- **Anchor check.** Refresh accesses come in groups of exactly four, one group
  per line period, and `L` is an integer on every line.
- **Line numbers.** The program writes a marker `VMARK(v)` as soon as its
  VHPOSR polling sees vertical position `v`. The port line containing
  `VMARK(v)` is line `v`; the following port lines are `v+1`, `v+2` and so on.

**F4: beam samples.**

- **Sample loop.** The CPU reads VPOSR and VHPOSR as one long word
  (`MOVE.L $DFF004,(An)+`) and stores it. The store is a Chip-memory write, so
  every sample carries a time.
- **Position fields.** From p.29 (`USE LOF I6 ... I0,LOL -- -- -- --v10 v9 V8`
  and `USE V7 ... V0,H8 ... H1`):
  - vertical `V = (VPOSR & 7) * 256 + (VHPOSR >> 8)`;
  - horizontal `H = VHPOSR & $FF`;
  - LOL is VPOSR bit 7.

  Other VPOSR bits (LOF, the chip id) are never graded.
- **HHPOSR samples.** Where HHPOSR is sampled, a `MOVE.W $DFF1DA,(An)+` is
  interleaved in the same loop.
- **Anchor check.** In a dry run in normal mode (`BEAMCON0 = $0020`,
  `BPLCON0 = 0`), consecutive `(V,H)` samples increase strictly in
  lexicographic order, except across the wrap at the end of a frame. The
  counter runs and is being read.

**F5: the observed sync lines are the chip's HSY* and VSY* pins** (anchor for
P9 and P10).

The claims in P9 and P10 concern the HSY* and VSY* pins. The harness shows
horizontal and vertical sync only after a line doubler, which may regenerate
them. The following runs decide whether the pins can be seen. In all of them:
HSSTRT = `$0020`, HSSTOP = `$0030`, VSSTRT = 100, VSSTOP = 103, all other
BEAMCON0 bits 0 except PAL.

- **S-H1:** `BEAMCON0 = $0120` (PAL + VARHSYEN).
- **S-H2:** the same with HSSTOP = `$0034`.
- **S-H3:** `BEAMCON0 = $0121` (adds HSYTRUE).
- **S-V1:** `BEAMCON0 = $0220` (PAL + VARVSYEN).
- **S-V2:** the same with VSSTOP = 105.
- **S-V3:** `BEAMCON0 = $0222` (adds VSYTRUE).

The expectations come from these page statements:

- p.12 "VARHSYEN= comparator HSY -> HSY* pin. Set on HSSTRT value, reset on
  HSSTOP value."
- p.12 "VARVSYEN= comparator VSY -> VSY* pin. The variable VSY is set
  vertically on VSSTRT, reset vertically on VSSTOP, with the horizontal
  position for set & reset HSSTRT on short fields (all fields are short if
  LACE=0)".
- p.13 "HSYTRUE,VSYTRUE,CSYTRUE= These change the polarity of the HSY*, VSY*,
  & CSY* pins".

The anchor holds when all of the following are true in the graded frame (F6):

- **S-H1 against S-H2:**
  - The observed horizontal sync differs on every chip line.
  - On each line it differs in one interval of `4*T_C`, with a tolerance of
    one output pixel clock.
  - It is identical elsewhere.
  - The line period is the same in both runs.
- **S-H3 against S-H1:** the observed horizontal sync is the logical inverse
  at every sample.
- **S-V1 against S-V2:**
  - The observed vertical sync differs in exactly one interval of `2*T_L`,
    with a tolerance of one output pixel clock.
  - It is identical elsewhere.
- **S-V3 against S-V1:** the observed vertical sync is inverted at every sample.

A chip line on the output is an interval of length `T_L` (F3) that starts at
an active edge of S-H1's observed horizontal sync. `V_var` is the output
interval in which S-V1's vertical sync is active. `V_norm` is the interval in
which the vertical sync is active in the reference run R0
(`BEAMCON0 = $0020`, same registers).

If S-H fails, the HSY* parts of P9 and P10 cannot be checked in this harness.
They are then reported as not checkable, not as red. If S-V fails, the same
holds for the VSY* parts. The anchor's own contrasting cases are built in:
S-H2 against S-H1 and S-V2 against S-V1 must differ, and S-H3 and S-V3 must
invert.

**F6: graded frame and run alignment.**

- **Graded frame.** For a run, the graded frame is the frame between the
  second and third frame-sync markers after the program's last register write.
- **Alignment.** Runs that are compared sample by sample (F5, P9, P10) must be
  cycle-aligned. Their refresh access times (F3) over the graded frame must be
  identical; otherwise the comparison is void and reported as a precondition
  failure. The paired runs differ only in the values written, not in the
  instruction sequence, so they align in a deterministic system.

---

## 3. Propositions

Region addresses are byte addresses. The port shows word addresses, which are
the byte address divided by 2. `M_x` is the modulo written for channel `x`,
taken as a signed 16-bit value; section 4, item 8 explains why every value used
has bit 15 equal to bit 14 and bit 0 clear.

### P1: The family members sit at the addresses of the p.7 list, the generic entries print channel A's, and A, B, C are sources while D is the destination

**Claims:** opus:AA-bltxpth.c1, opus:AA-bltxptl.c1, opus:AA-bltxmod.c1,
sol:AA-bltxpth.c1, sol:AA-bltxptl.c1, sol:AA-bltxmod.c1, opus:AA-bltxpth.c2,
opus:AA-bltxptl.c2, opus:AA-bltxmod.c2, sol:AA-bltxpth.c2, sol:AA-bltxptl.c2,
sol:AA-bltxmod.c2, opus:AA-bltxpth.c9, sol:AA-bltxpt.c2, opus:AA-bltxmod.c5,
sol:AA-bltxmod.c4. **Pages:** p.13, p.7.

**Proposition.** A CPU write takes effect at each address below, which is the
type W on p.13 and p.7. The pointer pair and modulo that channel `x` uses are:

| Channel | Pointer (PTH/PTL) | Modulo |
|---|---|---|
| C | `048`/`04A` | `060` |
| B | `04C`/`04E` | `062` |
| A | `050`/`052` | `064` |
| D | `054`/`056` | `066` |

The generic entries `BLTxPTH 050`, `BLTxPTL 052` and `BLTxMOD 064` are
channel A's. Channels A, B and C read (sources) and channel D writes
(destination).

**Page text.**

- p.13:
  - `BLTxPTH  h 050  W   A        Blitter Pointer to x (High 5 bits)`
  - `BLTxPTL    052  W   A        Blitter Pointer to x (Low 15 bits)`
  - `BLTxMOD    064  W   A        Blitter Modulo x`
  - "of Blitter source (x=A,B,C) or dest. (x=D) DMA data"
  - "the Modulo for Blitter source(x=A,B,C) or Dest (x=D)"
- p.7: `BLTCPTH   + ~048`, `BLTCPTL   + ~04A`, `BLTBPTH   + ~04C`,
  `BLTBPTL   + ~04E`, `BLTAPTH   + ~050`, `BLTAPTL   + ~052`,
  `BLTDPTH   + ~054`, `BLTDPTL   + ~056`, `BLTCMOD     ~060`,
  `BLTBMOD     ~062`, `BLTAMOD     ~064`, `BLTDMOD     ~066`, all `W`.

**Stimulus.**

- Before each blit, the program writes all eight pointer registers and all
  four modulo registers at the addresses above:

  | Channel | Pointer written | `R_x` | Modulo written |
  |---|---|---|---|
  | C | `$0C0000` | `$0C0000-$0CFFFF` | 8 |
  | B | `$0D0000` | `$0D0000-$0DFFFF` | 16 |
  | A | `$0E0000` | `$0E0000-$0EFFFF` | 24 |
  | D | `$0F0000` | `$0F0000-$0FFFFF` | 32 |

- Four blits, each with the template of 2.3, `W = 2`, `H = 2`
  (`BLTSIZE = $0082`):

  | Run | BLTCON0 | Channels |
  |---|---|---|
  | P1-A | `$09F0` | USEA, USED, D=A |
  | P1-B | `$05CC` | USEB, USED, D=B |
  | P1-C | `$03AA` | USEC, USED, D=C |
  | P1-D | `$01FF` | USED only |

- All registers are rewritten before every blit.

**Observation.** The F1 accesses in every region `R_C`, `R_B`, `R_A` and
`R_D`, between `BLIT_GO` and `BLIT_DONE` of each blit.

**Expected**, from the addresses above and p.13 "automatically added to the
address at the end of each line":

- **In P1-A:**
  - Accesses fall in `R_A` and `R_D` only; none in `R_B` or `R_C`.
  - The `R_A` accesses are reads, starting at byte `$0E0000`. The first access
    of line 1 is at the last access of line 0 plus `inc` plus 24.
  - The `R_D` accesses are writes, starting at `$0F0000`. The first access of
    line 1 is at the last access of line 0 plus `inc` plus 32.
- **P1-B and P1-C:** the same pattern with their own region, start address and
  modulo (16 and 8), all reads, plus `R_D` as in P1-A.
- **P1-D:** writes in `R_D` only, as in P1-A.

Together the four blits cover every one of the twelve registers. A single blit
would establish only its own channel.

**Preconditions,** each with its contrasting case:

- **F1 anchor.** Contrast: the `BLTEN`-off run (F1) shows no region access.
- **The blit finished.** BLIT was seen, BBUSY = 0, and every enabled channel
  shows `W*H` = 4 accesses (F2). Contrast: `BLTSIZE = $00C2` (`H = 3`) gives
  6 accesses per enabled channel (p.14 "h=Height=Vertical lines").
- **Constant `inc` within lines (F2).** Contrast: none is needed, since this
  only makes the line step computable. It is asserted, not varied.
- **2 MB Chip memory, AA selected (2.1).** Contrast: the configuration is
  asserted from the harness, and no run varies it.

**Contrasting case.**

- Swap the values written to `048/04A` and `050/052`: C gets `$0E0000` and A
  gets `$0C0000`. The page then predicts that P1-A reads start at `$0C0000`
  and P1-C reads at `$0E0000`. The region bounds are taken from what was
  written.
- Swap the values written to `060` and `064`: A's line step becomes
  `inc + 8` and C's becomes `inc + 24`.

**Reference frame.** F1, F2.

### P2: A pointer pair is one 20-bit address: PTH bits 4-0 are address bits 20-16, PTL bits 15-1 are address bits 15-1

**Claims:** opus:AA-bltxpth.c7, opus:AA-bltxptl.c6, opus:AA-bltxpth.c8,
sol:AA-bltxpth.c4, sol:AA-bltxptl.c4, sol:AA-bltxpt.c1, opus:AA-bltxpth.c5,
opus:AA-bltxptl.c4. **Pages:** p.13, p.6, p.7.

**Proposition.** For every channel `x`, the word address of the channel's
first access equals `PTH[4:0] * 2^15 + PTL[15:1]`. This holds for each of the
20 address bits individually.

**Page text.**

- p.13: "This pair of registers contains the 20 bit address", "(High 5 bits)",
  "(Low 15 bits)".
- p.6: "+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4." and
  "PTL,PTH=20 bit Pointer that addresses DMA data ... (old chips- 18 bits)".
- p.7: every Blitter pointer row carries `+`.

**Stimulus.** Two pointer values, `V1 = $155554` (PTH `$0015`, PTL `$5554`)
and `V2 = $0AAAAA` (PTH `$000A`, PTL `$AAAA`). Each of byte-address bits 1-20
is set in exactly one of them. For each channel `x` in A, B, C, D and each
value `V` in V1, V2 there is one blit, eight in all, each with `W = 1`, `H = 1`
(`BLTSIZE = $0041`):

- For `x` = A, B or C: a single-source blit `x`→D, with BLTCON0 as in P1.
  Channel `x`'s pointer is `V`. The D pointer is `$0F0000` (neutral region
  `R_D`).
- For `x` = D: the D-only blit with the D pointer `V`.
- `R_x` is `V` to `V + $FF`.

**Observation.** The channel's single access (F1).

**Expected.**

| Value written | Word address of the access |
|---|---|
| V1 = `$155554` | `$0AAAAA` |
| V2 = `$0AAAAA` | `$055555` |

This holds for every channel. A dropped, stuck or swapped address bit makes at
least one of the two differ.

**Preconditions,** each with its contrasting case:

- **2 MB of Chip memory,** since V1 lies above 1 MB. Asserted from the
  configuration.
- **The program image, stack and results do not overlap either region.**
  Asserted from the program's own address map.
- **F1 anchor and blit finished,** as in P1, with the same contrasts.

**Contrasting case.** V1 against V2 is the contrast. Every bit changes, so the
page predicts a different address for every bit. Writing PTH bits 15-5 is not
used as a contrast, because the page says nothing about them.

**Reference frame.** F1. The port's word address is compared directly; no
further anchor is needed.

**The conflicts.** p.7 prints `+` on each member's row. The p.13 entry does not,
because that list has no column for it (section 1). Its own text, "High 5 bits"
and "Low 15 bits", states the same split. The expectation above is what both
pages agree on.

### P3: The first access of every enabled channel is at the address its pointer was preloaded with

**Claims:** opus:AA-bltxpth.c10, sol:AA-bltxpt.c3. **Page:** p.13 (and p.6).

**Proposition.** After BLTSIZE is written, the first access of each enabled
channel A, B, C and D is at the address written to that channel's pointer pair.

**Page text.**

- p.13: "This pointer must be preloaded with the starting address of the data
  to be processed by the blitter."
- p.6: "Must be reloaded by a processor before use (... prior to starting the
  Blitter for Blitter pointers)".

**Stimulus.** Template 2.3 with `BLTCON0 = $0FF0` (all four channels, D=A),
`W = 3`, `H = 2` (`BLTSIZE = $0083`), all modulos 0. Pointers:

| Channel | Pointer |
|---|---|
| A | `$0E0100` |
| B | `$0D0100` |
| C | `$0C0100` |
| D | `$0F0100` |

The regions are those of P1.

- **Run P3-asc:** `BLTCON1 = $0000`.
- **Run P3-desc:** `BLTCON1 = $0002` (DESC, p.14 "Descending (decreasing
  address)control bit").

**Observation.** For each channel, the earliest F1 access in its region.

**Expected.** In both runs, each channel's first access is at the pointer
written for it. The quantifier "each" covers all four channels and both
directions. For P3-desc only the first access is graded: p.13 does not say
where a descending blit's later accesses fall relative to the modulo.

**Preconditions,** each with its contrasting case:

- **F1 anchor.** Contrast: the BLTEN-off run.
- **The blit finished,** with 6 accesses per channel. Contrast: `H = 3` gives
  9.

**Contrasting case.** Change A's pointer to `$0E0200`. The page then predicts
A's first access at `$0E0200`, with the other channels unchanged.

**Reference frame.** F1.

### P4: After a blit, each pointer holds the last data address plus increment plus modulo

**Claims:** opus:AA-bltxpth.c11, sol:AA-bltxpt.c4. **Page:** p.13.

**Proposition.** When a blit has finished, each enabled channel's pointer
contains the channel's last data address plus the increment plus that
channel's modulo.

**Page text.** p.13: "After the Blitter is finished it will contain the last
data address (plus increment and modulo)."

**Stimulus.** The pointers are write-only (`W`, p.13), so their contents are
observed through a second blit that uses them without rewriting them.

- **Blit 1:** template 2.3, `BLTCON0 = $0FF0`, `W = 3`, `H = 2`. Pointers as in
  P1 (A `$0E0000`, B `$0D0000`, C `$0C0000`, D `$0F0000`). Modulos:

  | Channel | Modulo |
  |---|---|
  | A | +6 |
  | B | -2 (`$FFFE`) |
  | C | 0 |
  | D | +10 |

- **Blit 2:** after `BLIT_DONE` of blit 1, the program clears BLIT, writes
  marker `BLIT_GO2`, writes `BLTSIZE = $0083` again, waits and writes
  `BLIT_DONE2`. No pointer or modulo register is written between the two
  blits.

**Observation.** For each channel:

- the last F1 access of blit 1 (window `BLIT_GO` to `BLIT_DONE`);
- the first F1 access of blit 2 (window `BLIT_GO2` to `BLIT_DONE2`).

**Expected.** For every channel, the first access of blit 2 equals the last
access of blit 1 plus `inc` plus `M_x`. Here `inc` is measured in blit 1 (F2)
and `M_x` sign-extended. With a one-word `inc` this gives:

| Channel | First access of blit 2 |
|---|---|
| A | `$0E0018` |
| B | `$0D0008` |
| C | `$0C000C` |
| D | `$0F0020` |

The grading uses the relation, not this table.

**Preconditions,** each with its contrasting case:

- **Blit 1 finished before blit 2 started.** BLIT was seen, BBUSY = 0, and 6
  accesses per channel were seen before `BLIT_GO2`. Contrast: the BLTEN-off
  run shows no accesses and times out, and blit 2 then shows nothing either.
- **`inc` is constant within blit 1's lines (F2).** Asserted.
- **No pointer or modulo write between the blits.** This follows from the
  program text. It is not visible at the port.

**Contrasting case.** Blit 1 with A's modulo +14 instead of +6. The page then
predicts A's blit-2 start 16 bytes higher (`$0E0028` with a one-word `inc`),
and B, C and D unchanged.

**Reference frame.** F1, F2.

### P5: At the end of every line the channel's modulo is added, so the next line starts at last address + increment + modulo, for positive, zero and negative modulos

**Claims:** opus:AA-bltxmod.c6, sol:AA-bltxmod.c5, sol:AA-bltxmod.c6.
**Pages:** p.13, p.6.

**Proposition.** For every channel and every line boundary `k` to `k+1` of a
blit:

`a_{(k+1)W} = a_{kW+W-1} + inc + M_x`

`M_x` is sign-extended: p.6 "Uses 15 bits, plus sign extend".

**Page text.**

- p.13: "A Modulo is a number that is automatically added to the address at
  the end of each line, in order that the address then points to the start of
  the next line."
- p.6: "MOD=15 bit Modulo. ... Uses 15 bits, plus sign extend."

**Stimulus.**

- **Blits:** five blits, each `BLTCON0 = $0FF0`, `W = 2`, `H = 4`
  (`BLTSIZE = $0102`).
- **Regions:**

  | Channel | Region | Pointer (region base + `$10000`) |
  |---|---|---|
  | A | `$100000-$11FFFF` | `$110000` |
  | B | `$120000-$13FFFF` | `$130000` |
  | C | `$140000-$15FFFF` | `$150000` |
  | D | `$160000-$17FFFF` | `$170000` |

- **Modulo values:** the set `{$0000, $0006, $FFFC (-4), $3FFE (+16382),
  $C002 (-16382)}`. Bit 15 equals bit 14 in each, so every reading of "15 bits
  plus sign extend" gives the same number (section 4, item 8).
- **Rotation:** blit `j` (`j` = 0..4) gives channel A, B, C, D the values with
  indices `j`, `j+1`, `j+2`, `j+3` (mod 5). Over the five blits every channel
  meets every value.

**Observation.** All F1 accesses of all channels in each blit.

**Expected.** The relation above at every one of the three line boundaries of
every channel in every blit. That is 60 boundaries, and all must hold.

**Preconditions,** each with its contrasting case:

- **Blit finished,** with 8 accesses per channel. Contrast: BLTEN off.
- **`inc` constant within lines.** Asserted.
- **Regions large enough.** The largest excursion, `3 * (4 + 16382)` bytes,
  stays inside each region. This is checked from the values.

**Contrasting case.** The blit with modulo `$0000` against the one with
`$0006` for the same channel. The page predicts line starts 6 bytes further
apart per boundary.

**Reference frame.** F1, F2.

### P6: Each channel uses its own modulo, and each channel processes an identical area

**Claims:** opus:AA-bltxmod.c7, sol:AA-bltxmod.c7, sol:AA-bltxmod.c8.
**Page:** p.13.

**Proposition.** In one blit with four different modulos:

- each channel's line-to-line step uses that channel's own modulo;
- every channel makes the same number of accesses, `W` per line for `H` lines.

**Page text.** p.13: "Each source or destination has it's own Modulo, allowing
each to be a different size, while an identical area of each is used in the
Blitter operation."

**Stimulus.** `BLTCON0 = $0FF0`, `W = 2`, `H = 3` (`BLTSIZE = $00C2`), with the
pointers and regions of P1. Modulos:

| Channel | Modulo |
|---|---|
| A | +8 |
| B | -4 (`$FFFC`) |
| C | 0 |
| D | +20 |

**Observation.** The F1 accesses of all four channels.

**Expected.**

- For every channel, at both line boundaries, the step
  `a_{(k+1)W} - a_{kW+W-1}` equals `inc + M_x` with the channel's own `M_x`.
  That makes four different steps.
- Each channel shows exactly 6 accesses, 2 per line.

**Preconditions,** each with its contrasting case:

- **F1 anchor and blit finished.** Contrasts as in P1.

**Contrasting case.** Change only B's modulo to +12. The page predicts:

- B's steps change by 16;
- A's, C's and D's steps and all access counts stay the same.

**Reference frame.** F1, F2.

### P7: The Copper can write the Blitter pointer and modulo registers only when COPCON bit 1 is set

**Claims:** opus:AA-bltxmod.c4, opus:AA-bltxpth.c6, opus:AA-bltxptl.c5.
**Pages:** p.6, p.7, p.19-20.

**Proposition.** For every one of the twelve registers `BLTxPTH`, `BLTxPTL` and
`BLTxMOD` (x = A, B, C, D):

- with COPCON bit 1 (CDANG) = 0, a Copper MOVE to the register does not change
  it;
- with CDANG = 1, it does.

**Page text.**

- p.6 legend: "~=Address not writable by the Coprocessor unless COPCON bit 1 is
  set true."
- p.7: every one of the twelve rows carries `~`.
- p.19-20, COPCON: "This is a 1 bit register that when set true, allows the
  Coprocessor to access the Blitter hardware. This bit is cleared by power on
  reset, so that the Coprocessor cannot access the Blitter hardware." and
  "01     CDANG     Coprocessor danger mode. Allows coprocessor access to all
  RGA registers if true. (if 0, access to RGA>7E)".
- The p.13 entries are silent: they have no column for `~` (section 1).

All pages that speak agree.

**Stimulus.**

1. The CPU writes base values into all twelve registers:

   | Channel | Pointer | Modulo |
   |---|---|---|
   | A | `$0E0000` | 8 |
   | B | `$0D0000` | 8 |
   | C | `$0C0000` | 8 |
   | D | `$0F0000` | 8 |

2. It writes `COPCON = $0000` (run P7-0) or `COPCON = $0002` (run P7-1).
3. It builds twelve Copper lists `L1..L12` in Chip memory, one per register:
   - `WAIT $3C01,$FFFE` (wait for line 60; p.20 WAIT format: IR1 bit 0 = 1,
     IR2 bit 0 = 0);
   - `MOVE INTREQ,$8010` (sets COPER, p.25; INTREQ `09C` is above `$7E`, so it
     is always writable);
   - `MOVE <register k>,<Copper value k>`;
   - `WAIT $FFFF,$FFFE`.

   The Copper values:

   | Channel | PTH | PTL | Pointer | Modulo |
   |---|---|---|---|---|
   | A | `$0011` | `$4000` | `$114000` | 24 |
   | B | `$0012` | `$4000` | `$124000` | 24 |
   | C | `$0013` | `$4000` | `$134000` | 24 |
   | D | `$0014` | `$4000` | `$144000` | 24 |

   Base and Copper values differ in both PTH and PTL, so each half is
   identified separately.
4. It starts the Copper in the order p.21 prescribes ("It is important that one
   of the jump registers be initalized and it's jump strobe address hit, after
   power up but before Coprocessor DMA is initalized"; also HRM p.30):
   - write `COP1LCH` and `COP1LCL` = `L1`;
   - write `COPJMP1`;
   - only then `DMACON = $8280` (SET, DMAEN, COPEN).
5. For `k = 1..12`:
   - wait for INTREQR COPER, with the 2.2 timeout, and store the outcome;
   - clear COPER;
   - poll VHPOSR until `V >= 100`;
   - write `COP1LC = L(k+1)`;
   - wait for VERTB.

   p.21: "COP1LC is automatically used at the beginning of each vertical blank
   time", so frame `k+1` runs `L(k+1)`.
6. Write `DMACON = $0080` (clear COPEN), then run one blit with template 2.3,
   except that step 2 writes no pointer or modulo register:
   `BLTCON0 = $0FF0`, `W = 2`, `H = 2`.

**Observation.**

1. **The Copper's fetches, at the port.** In frame `k`, reads of the words at
   `L_k+8` and `L_k+10`. These are the two words of the MOVE to register `k`.
2. **The blit.** Each channel's first access gives the PTH and PTL in effect.
   Each channel's line step minus `inc` gives the modulo in effect. The regions
   cover both the base and the Copper addresses and every mixture (for example
   `$0E4000`, base PTH with Copper PTL).

**Expected.**

- **P7-0:** all twelve registers hold the base values. First accesses are at
  `$0E0000`, `$0D0000`, `$0C0000` and `$0F0000`, and every step is
  `inc + 8`.
- **P7-1:** all twelve registers hold the Copper values. First accesses are at
  `$114000`, `$124000`, `$134000` and `$144000`, and every step is
  `inc + 24`.

"Unless" is graded both ways, and "not writable" covers every register, so
every register is graded separately.

The page says nothing about what the Copper does after a refused MOVE. The
MOVE to INTREQ is therefore placed before the protected MOVE, and each register
has its own frame and list, so a Copper that stops at a refused MOVE still
reaches every register.

**Preconditions,** each with its contrasting case:

1. **In every frame `k`, both words of the MOVE to register `k` were fetched.**
   A register whose MOVE was never fetched is not graded, and the run fails
   its precondition. Contrast: replace that MOVE in `L_k` by `MOVE COLOR00,$0000`
   (`180`). The page then predicts no write to register `k` in either COPCON
   setting, so in P7-1 register `k` keeps its base value.
2. **The Copper executes MOVEs to always-writable registers:** COPER was seen
   in every frame. Contrast: remove the INTREQ MOVE from the lists. COPER is
   then never set, the wait times out, and the program still finishes.
3. **The blit ran,** with 4 accesses per channel. Contrast: the BLTEN-off run.

**Contrasting case.** P7-0 against P7-1. Every register's expected value
changes.

**Reference frame.** F1, F2, and the Copper list addresses at the port.

### P8: With LPE set, LPENDIS low makes VHPOSR, VPOSR and HHPOSR read the latched light-pen position; LPENDIS high makes them read the actual beam counters

**Claims:** opus:AA-beamcon0.b13, sol:AA-beamcon0.c8, opus:AA-beamcon0.c9,
opus:AA-beamcon0.c10, opus:AA-beamcon0.c8, sol:AA-beamcon0.c24,
sol:AA-beamcon0.c25, sol:AA-beamcon0.c26. **Pages:** p.12, p.16-17.

**Proposition.**

- With BPLCON0 bit 3 set and BEAMCON0 bit 13 (LPENDIS) = 0, after a light-pen
  trigger VHPOSR, VPOSR (position bits) and HHPOSR return the position latched
  at the trigger.
- With LPENDIS = 1 and the same trigger, they return the running counters.

**Page text.**

- p.12: "13     LPENDIS" and "LPENDIS= When this bit is a low and LPE
  (BPLCON0,BIT 3) is enabled, the light-pen latched value (beam hit position)
  will be read by VHPOSR, VPOSR and HHPOSR. When the bit is a high the
  light-pen latched value is ignored and the actual beam counter position is
  read by VHPOSR, VPOSR and HHPOSR."
- p.16: bit 3 of BPLCON0 is `LPEN`.
- p.17: "LPEN =Light pen enable(reset on power up)".

p.12 calls the same bit "LPE" (section 4, item 5).

**Trigger.** The AA pages do not say where the light-pen trigger enters. HRM
p.238 places it on pin 6 ("Beam Trigger") of a controller port, port 1 or 2
depending on the machine. The harness drives pin 6 of both controller ports
together, so the port question does not arise.

**Stimulus.**

1. Set `DMACON = $7FFF` and `BPLCON0 = $0000`.
2. Set `BEAMCON0 = $0020` (P8-0: LPENDIS = 0) or `$2020` (P8-1: LPENDIS = 1).
3. Frame sync, then poll VPOSR/VHPOSR (F4) until `V = 254` and `H >= $80`.
4. Set `BPLCON0 = $0008` (LPEN).
5. Write marker `LP_GO`.
6. Take `N` samples in one loop: the F4 long word, then HHPOSR (word).
7. Write marker `LP_END`, then set `BPLCON0 = $0000`.

**The pulse.** The harness drives pin 6 of both controller ports low for at
most `2*T_C`, then releases it. The pulse starts at `LP_GO + D`, where `D` and
`N` are fitted per CPU core from the dry run below. They must satisfy all of
the following:

- at least 4 samples fall before the pulse;
- at least 8 fall after it;
- the pulse falls on line 255;
- in the dry run the samples after the pulse reach `V >= 256`, so that the
  running counter changes VPOSR bit 0 (V8) inside the window.

**Dry run P8-dry:** as P8-0 without the pulse.

**Observation.** The samples, each dated by its store time at the port (F4):

- "before" samples were stored before the pulse started;
- "after" samples were stored more than one sample interval after the pulse
  ended.

The graded fields are `V` (VPOSR bits 2-0 with VHPOSR bits 15-8), `H` (VHPOSR
bits 7-0) and HHPOSR (all 16 bits). LOF, LOL and the chip id bits of VPOSR are
not graded: p.12 speaks of the "beam hit position".

**Expected.**

- **P8-0, VPOSR and VHPOSR:**
  - All "after" samples have one identical `(V,H)`, the hit position.
  - The hit position is not earlier than the last "before" sample.
  - It is not later than the position the running counter reached at the end
    of the pulse. That position is the last "before" position plus the elapsed
    time in CCK (`T_C`, F3), plus one sample interval of tolerance.
  - The hit's `V` is 255, so VPOSR bit 0 (V8) stays 0 in the "after" samples.
- **P8-0, HHPOSR:** all "after" samples are identical.
- **P8-1:**
  - `(V,H)` increases strictly from each sample to the next across the whole
    window, including after the pulse.
  - The last samples have `V >= 256`, so VPOSR bit 0 = 1: VPOSR, too, is
    shown to read the actual counter.
  - HHPOSR: consecutive "after" samples are not all identical.

All three registers are graded in both settings, subject to precondition 3.

**Preconditions,** each with its contrasting case:

1. **The trigger reaches the chip.** P8-0 shows the freeze. This is also the
   precondition for P8-1: the green of P8-1 means nothing unless the same
   pulse froze the reads in P8-0. Contrast: P8-dry (no pulse), whose samples
   advance throughout. The page speaks of a latched value only where a hit
   exists; HRM p.239 describes the trigger.
2. **LPE enables the latch.** Contrast: P8-0 with step 4 writing
   `BPLCON0 = $0000`. The samples then advance through the pulse: "LPEN =Light
   pen enable" (p.17), and p.12 conditions the latched read on LPE.
3. **The alternate counter runs,** so that HHPOSR can be graded. In P8-dry,
   HHPOSR takes at least two different values. If it does not, HHPOSR is not
   graded in either setting (reported). A counter that never moves would pass
   the freeze check without meaning anything. The page does not say whether
   this counter runs with DUAL = 0.
4. **F4 anchor** in P8-dry. Contrast: none beyond the P8-0 freeze, which is
   the same measurement going the other way.

**Contrasting case.** P8-1 against P8-0: same stimulus, LPENDIS changed, and
the page predicts running counters instead of a frozen value.

**Reference frame.**

- F4 for the sample times.
- F3 for `T_C`.
- Line 255 and V8 as anchors. The test checks that the P8-dry samples cross
  from `V = 255` to `V = 256` inside the window.

### P9: With CSCBEN set, the variable composite sync comes out on HSY* and the variable composite blank on VSY*

**Claims:** opus:AA-beamcon0.b10, sol:AA-beamcon0.c11, opus:AA-beamcon0.c15,
opus:AA-beamcon0.c16, opus:AA-beamcon0.c18 (second half), sol:AA-beamcon0.c33,
sol:AA-beamcon0.c34, opus:AA-beamcon0.c38. **Pages:** p.12, p.13, p.24, p.29.

**Proposition.** With BEAMCON0 bit 10 (CSCBEN) = 1, VARHSYEN = 0 and
VARVSYEN = 0:

- (a) the HSY* output carries the variable composite sync. It becomes active
  at the HSSTRT position on every line, and on vertical-sync lines HCENTER
  also affects it.
- (b) the VSY* output carries the variable composite blank, which follows
  VBSTRT.

With CSCBEN = 0 none of these registers affects the two pins.

**Page text.**

- p.12: "10     CSCBEN" and "CSCBEN= The variable composite sync comes out on
  the HSY* pin, and the variable composite blank comes out on the VSY* pin.
  ... The normal monitor uses the normal composite sync, and the variable
  composite sync & blank come out the HSY* & VSY* pins."
- p.13: "(VARCSY is set on HSSTRT match always, and also on HCENTER match when
  in vertical sync. It is reset on HSSTOP match when VSY*, and on both HBSTRT
  & HBSTOP matches during VSY."
- p.29, VBSTRT/VBSTOP: "Affects CSY* pin if BLANKEN=1 & VSY* Pin if CSCBEN=1
  (see BEAMCON0)".
- p.24, HSSTRT/HSSTOP: "See BEAMCON0 for details of when these 2 are active";
  HCENTER: "See BEAMCON0 for when it affects chip outputs".

The link between "variable composite sync" (CSCBEN) and VARCSY (VARCSYEN) is
a reading of the page; section 4, item 7 explains it.

**Stimulus.** All runs use the F5 register set, which fixes these values:

| Register | Value | Unit |
|---|---|---|
| HSSTRT | `$20` | CCK, bits 7-0 (p.24 "Sets # of color clocks", "HTOTAL for bits") |
| HSSTOP | `$30` | CCK |
| HBSTRT | `$0010` | bits 7-0 in 280 ns steps, fine bits 10-8 = 0 (p.24) |
| HBSTOP | `$0081` | as HBSTRT |
| HCENTER | `$91` | CCK |
| VSSTRT | 100 | line |
| VSSTOP | 103 | line |
| VBSTRT | 150 | line |
| VBSTOP | 170 | line |

The HBSTRT, HBSTOP and HCENTER values follow the page's own recipe (p.13):
"A reasonable composite can be generated by setting HCENTER half a horiz line
from HSSTRT, and HBSTOP at (HSSTOP-HSSTRT) before HCENTER, with HBSTRT at
(HSSTOP-HSSTRT) before HSSTRT."

The runs come in pairs that differ in one register.

- **With `BEAMCON0 = $0420` (PAL, CSCBEN):**
  - **H-pair:** HSSTRT `$20` against `$24`.
  - **C-pair:** HCENTER `$91` against `$99`. HBSTOP (`$81`) stays below both.
  - **V-pair:** VBSTRT 150 against 152.
- **With `BEAMCON0 = $0020`:** the same three pairs (the contrasts).

**Observation.** The observed horizontal sync (for the H-pair and C-pair) and
vertical sync (for the V-pair), over the graded frame (F6), compared sample by
sample between the two runs of a pair. Chip lines, `V_var` and `V_norm` are as
in F5.

**Expected.**

- **(a) H-pair, CSCBEN = 1:**
  - The two runs differ on every chip line of the graded frame. "Always" is
    graded over all lines, including vertical-sync and vertical-blank lines.
  - On each line they differ in one interval of `4*T_C`, with a tolerance of
    one output pixel clock.
  - They are identical elsewhere.
- **(a) C-pair, CSCBEN = 1:**
  - The runs differ on at least one chip line of the graded frame.
  - Every differing line lies within `V_var` or `V_norm`, with a tolerance of
    one line.
  - Setting VSSTRT/VSSTOP makes "in vertical sync" a known set of lines under
    either reading of which vertical sync the page means.
  - This is what makes the signal a composite sync rather than a plain
    horizontal sync.
- **(b) V-pair, CSCBEN = 1:**
  - The runs differ on chip lines 150 and 151 and nowhere else.
  - A partial line at either end is tolerated, because the page does not say
    at which horizontal position the vertical blank starts.
  - Each of the two lines contains differing samples.
  - Polarity and the horizontal content of the composite blank are not
    graded: the page does not state them.
- **Contrasting pairs, CSCBEN = 0:** all three pairs identical at every sample.

**Preconditions,** each with its contrasting case:

1. **F5 anchor S-H holds** (for (a)) and S-V holds (for (b)). The anchor's
   contrasts are part of F5. If S-H or S-V fails, the corresponding part is
   reported as not checkable.
2. **All paired runs are cycle-aligned (F6).** Contrast: none. This is a
   validity check on the comparison, not a condition the page speaks to.
3. **ERSY = 0** (sync pads are outputs; p.17). Contrast: none is run. With
   ERSY = 1 the pads become inputs, and the output then no longer shows the
   chip's sync. That would break the setup, which the contrasting case must
   leave working.
4. **`V_var` and `V_norm` are non-empty in the graded frame.** Asserted from
   S-V1 and R0.

**Contrasting case.** CSCBEN = 0: the pairs become identical, because the
three registers act on the pins only through the BEAMCON0 enables (p.24, p.29).

**Reference frame.** F3 (`T_C`, `T_L`), F5 (chip lines on the output, `V_var`,
`V_norm`), F6.

### P10: VARHSYEN and VARVSYEN have priority over CSCBEN

**Claims:** opus:AA-beamcon0.c19, sol:AA-beamcon0.c35. **Page:** p.12.

**Proposition.**

- With CSCBEN = 1 and VARHSYEN = 1, HSY* carries the comparator HSY: set on
  HSSTRT, reset on HSSTOP on every line, independent of HCENTER.
- With CSCBEN = 1 and VARVSYEN = 1, VSY* carries the comparator VSY,
  independent of VBSTRT.

**Page text.**

- p.12: "The bits VARVSYEN & VARHSYEN (below) have priority over this control
  bit."
- p.12: "VARHSYEN= comparator HSY -> HSY* pin. Set on HSSTRT value, reset on
  HSSTOP value."
- p.12: "VARVSYEN= comparator VSY -> VSY* pin. The variable VSY is set
  vertically on VSSTRT, reset vertically on VSSTOP".

**Stimulus.** The P9 register set.

- **With `BEAMCON0 = $0520` (PAL, CSCBEN, VARHSYEN):**
  - **C-pair:** HCENTER `$91` against `$99`.
  - **S-pair:** HSSTOP `$30` against `$34`.
- **With `BEAMCON0 = $0620` (PAL, CSCBEN, VARVSYEN):**
  - **V-pair:** VBSTRT 150 against 152.
  - **VS-pair:** VSSTOP 103 against 105.

**Observation.** As in P9.

**Expected.**

- **With `$0520`:**
  - The C-pair is identical at every sample.
  - The S-pair differs on every chip line, including the vertical-sync lines,
    in one interval of `4*T_C`, with a tolerance of one output pixel clock.
    During VSY the composite would not be reset at HSSTOP (p.13), so a
    difference on those lines shows the comparator HSY.
- **With `$0620`:**
  - The V-pair is identical at every sample.
  - The VS-pair differs in exactly one interval of `2*T_L`, with a tolerance
    of one output pixel clock.

**Preconditions,** each with its contrasting case:

- **F5 anchors S-H and S-V.** Contrasts as in F5.
- **Cycle alignment (F6).** Contrast: none; this is a validity check on the
  comparison.
- **The vertical-sync lines exist in the graded frame.** Asserted as in P9.

**Contrasting case.**

- Clear VARHSYEN (`$0420`): the C-pair differs on the vertical-sync lines
  (P9 (a)).
- Clear VARVSYEN (`$0420`): the V-pair differs on lines 150-151 (P9 (b)).

**Reference frame.** As in P9.

### P11: DUAL runs the horizontal comparators, among them HTOTAL, on the alternate horizontal counter instead of the main one

**Claims:** opus:AA-beamcon0.c30, sol:AA-beamcon0.c47. **Pages:** p.12, p.24.

**Proposition.** With BEAMCON0 bit 6 (DUAL) = 1 and VARBEAMEN = 0, HTOTAL
bounds the alternate counter (read via HHPOSR). It does not bound the main
horizontal counter (read via VHPOSR).

**Page text.**

- p.12: "DUAL= Run the horizontal comparators with the alternate horizontal
  beam counter".
- p.24, HHPOSW/HHPOSR: "This is the secondary beam counter for the faster
  mode, triggering the UHRES pointers & doing the comparisons for
  HBSTRT,STOP,HTOTAL,HSSTRT,HSSTOP (See HTOTAL for bits)".
- p.24, HTOTAL: "Highest color clock count in horiz line ... Active if
  VARBEAMEN=1 or DUAL=1."

**Stimulus.**

- `HTOTAL = $0038`.
- `BEAMCON0 = $0060` (PAL, DUAL).
- Frame sync.
- Sample loop F4 with HHPOSR interleaved, over one full frame. Every sample
  stores `(V,H)` and HHPOSR.

**Observation.**

- The maximum `H` over all samples of the frame.
- Every HHPOSR sample (bits 7-0, the HTOTAL bit positions).
- The number of HHPOSR decreases, which are wraps, within each main line
  (same `V`).

**Expected.**

- The maximum `H` exceeds `$38`: HTOTAL does not reset the main counter.
- Every HHPOSR sample is at most `$38`: HTOTAL is the alternate counter's
  highest count.

**Preconditions,** each with its contrasting case:

1. **The alternate counter runs and wraps.** On at least one main line,
   HHPOSR decreases at least twice. Otherwise "at most `$38`" could pass with
   a stuck counter. Contrast: the DUAL run with `HTOTAL = $00E2`. HTOTAL is
   the alternate counter's highest count, so the page predicts that its period
   grows by `($E2 + 1) / ($38 + 1)`. The number of wraps per main line must
   fall by that ratio, with a tolerance of 1.
2. **F4 anchor.** Contrast as in F4.

**Contrasting case.**

- `BEAMCON0 = $08A0` (PAL, VARBEAMEN, LOLDIS; DUAL off), with
  `VTOTAL = $0138` because VTOTAL is in use when VARBEAMEN = 1 (p.29). The page
  then predicts that HTOTAL governs the main counter: the maximum `H` is at
  most `$38`. p.12: "VARBEAMEN= Enables the variable beam counter comparators
  to operate ... on the main horiz counter". p.24 gives "this many + 1", and
  with LOLDIS = 1 there are no long lines.
- For the bound on HHPOSR: DUAL run with `HTOTAL = $0070`. Some HHPOSR sample
  must now exceed `$38`, and all must stay at most `$70`.

**Reference frame.** F4. Main lines are grouped by the sampled `V`.

### P12: DUAL starts the UHRES pointer chain at each reset of the alternate counter, so the UHRES pointers come out more than once per main line

**Claims:** opus:AA-beamcon0.c31, opus:AA-beamcon0.c32, sol:AA-beamcon0.c48,
sol:AA-beamcon0.c49. **Pages:** p.12, p.15, p.16, p.22, p.27.

**Proposition.** With UHRES enabled, DUAL = 1 and free memory cycles
available, the UHRES bit-plane pointer and the UHRES sprite pointer each
appear at the Chip-memory port more times per main line than with DUAL = 0.
The count per main line follows the number of alternate-counter resets.

**Page text.**

- p.12: "DUAL= ... and starts the UHRES pointer chain with the reset of this
  counter rather than the normal one. This allows the UHRES pointers to come
  out more than once in a horizontal line, assuming there is some memory
  bandwidth left".
- p.15: "When UHRES is enabled, this pointer comes out on the 2nd 'free' cycle
  after the start of each horiz. line. ... The SHRHDAT gets the first and
  third free cycles."
- p.27, SPRHPTH/L: "This pointer is activated in the 1st & 3rd 'free' cycles
  (see BPLHPTH,L) after horiz line start."
- p.16: "UHRES= ultrahi res enables the UHRES pointers (for 1k*1k) (also needs
  bits in DMACON)".
- p.22: "09    DMAEN       Enable all DMA below (also UHRES dma)".

**Stimulus.**

| Register | Value |
|---|---|
| `BPLCON0` | `$0080` (UHRES) |
| `BPLHPTH/L` | `$180000` (region `$180000-$18FFFF`) |
| `BPLHMOD` | `$0000` |
| `BPLHSTRT` | 100 |
| `BPLHSTOP` | 110 (bit 15 BPLHWRM = 0) |
| `SPRHPTH/L` | `$190000` (region `$190000-$19FFFF`) |
| `SPRHSTRT` | 100 |
| `SPRHSTOP` | 110 (bit 15 SPRHWRM = 0) |
| `HTOTAL` | `$0038` |
| `DMACON` | `$8200` (DMAEN only) |

1. **Run P12-0:** `BEAMCON0 = $0020`. **Run P12-1:** `BEAMCON0 = $0060`
   (DUAL).
2. In the graded frame the program writes `VMARK(90)` (F3). It then halts the
   CPU with `STOP` until the next vertical blank, so that the port shows only
   refresh and UHRES traffic in lines 90-120.

If P12-0 shows no access in either region, both runs are repeated with
`DMACON = $8320` (adds BPLEN and SPREN). The page leaves open which DMACON bits
"also" are needed (section 4, item 13). In that repeat all eight sprite
pointers point at a zeroed null sprite at `$1A0000`, whose region is excluded
from the counts. The report states which DMACON setting was used.

**Observation.**

- The port accesses in the two regions, per port line (F3), on lines 102-108.
  These are the lines inside 100-110, with one line of margin at each end for
  the marker's placement.
- `k(line)`: the number of alternate-counter resets in the line. It is
  measured in P11's DUAL run as `T_L / T_alt`, where
  `T_alt = (HTOTAL + 1) / r` and `r` is the HHPOSR count rate per `T_C`. `r`
  is the slope of the HHPOSR samples against their store times between wraps.

**Expected.**

- **P12-1 (DUAL):** on every graded line:
  - the BPLHPT region has at least 2 accesses, and the count lies in
    `{floor(k), ceil(k)}`;
  - the SPRHPT region has more than 2 accesses. A single chain start gives
    SPRHPT two cycles (p.15, p.27).
- **P12-0 (the contrast):** on every graded line, exactly 1 BPLHPT access and
  exactly 2 SPRHPT accesses ("the 2nd 'free' cycle after the start of each
  horiz. line", "the first and third free cycles").

Addresses within the regions are not graded. p.15 says both "added ... every
line, and then another 2 is added" (BPLHMOD) and "Its modulo is added every
time it comes out" (BPLHPT), and with DUAL these differ (section 4, item 14).

**Preconditions,** each with its contrasting case:

1. **UHRES DMA exists,** so that P12-0 shows its one-per-line pattern.
   Contrast: `BPLCON0 = $0000`, which shows no access in either region (p.16).
2. **Bandwidth is left.** All DMA other than UHRES is off (and the null sprite
   in the fallback), and the CPU is stopped. Contrast: none. The page
   conditions the claim on free bandwidth, and this setup provides the
   maximum.
3. **The alternate counter wraps at least twice per main line,** so `k >= 2`
   (P11 precondition 1). Contrast: `HTOTAL = $00E2` makes `k` fall. The page
   then predicts fewer appearances; `k` and the count must move together.
4. **F3 anchor**, and `VMARK(90)` lies in the port line the test calls 90.
   The marker is written within a few CCK of the line start because the poll
   reads VHPOSR repeatedly. Contrast: none; this is how the reference frame is
   placed.

**Contrasting case.** P12-0 (DUAL off): one bit-plane and two sprite pointer
appearances per line.

**Reference frame.** F3 port lines and line numbers, F4 for `r`.

### P13: In variable-beam-counter mode, the PAL bit disables the long line/short line toggle, and every line is short

**Claim:** sol:AA-beamcon0.c53. **Pages:** p.13 (continuing p.12), p.24, p.29.

**Proposition.** With VARBEAMEN = 1, LOLDIS = 0 and PAL = 1:

- every line has HTOTAL + 1 CCK;
- VPOSR's LOL bit is 0 on every line.

With PAL = 0 (and LOLDIS = 0), lines alternate between HTOTAL + 1 and
HTOTAL + 2 CCK.

**Page text.**

- p.12-13: "PAL= Set appropriate decodes (in normal mode) for PAL. In variable
  beam counter mode this bit disables the long line/short line toggle- ends up
  short line."
- p.24, HTOTAL: "Horiz line has this many + 1 280ns increments. If the PAL bit
  & LOLDIS are not high, long line/short line toggle will occur, and there
  will be this many + 2 every other line."
- p.29: "LOL=Long line bit. When low, it indicates short raster line."

**Stimulus.** `HTOTAL = $00E2`, `VTOTAL = $0138`.

- **Run P13-1:** `BEAMCON0 = $00A0` (VARBEAMEN, PAL).
- **Run P13-0:** `BEAMCON0 = $0080` (VARBEAMEN).

Frame A: the F4 sample loop over a full frame, storing VPOSR (for LOL) with
`(V,H)`. Frame B: the CPU executes `STOP` for the whole frame (F3).

**Observation.**

- **Frame B:** the length `L` of every port line (F3).
- **Frame A:** the LOL bit per sampled line `V`.

**Expected.**

- **P13-1:** every port line of frame B has `L = $E2 + 1 = 227` CCK, and LOL
  is 0 on every sampled line of frame A. "Ends up short line" is graded over
  every line of a full frame, not over one.
- **P13-0 (the contrast):**
  - port lines alternate between 227 and 228 CCK;
  - LOL alternates between consecutive sampled lines;
  - there are exactly as many 228-CCK port lines in frame B as LOL = 1 lines
    in frame A, with a tolerance of 1.

**Preconditions,** each with its contrasting case:

1. **Variable-beam mode is in effect:** line length follows HTOTAL. Contrast:
   `HTOTAL = $00C0` in P13-1 gives `L = 193` on every line.
2. **LOLDIS = 0,** so that only PAL can suppress the toggle. Contrast:
   P13-0 with LOLDIS set (`$0880`) shows no alternation. p.12: "LOLDIS=
   Disable long line/short line toggle".
3. **F3 anchor** in frame B, and the **F4 anchor** in frame A. Contrasts as in
   F3 and F4.

**Contrasting case.** P13-0: toggling returns.

**Reference frame.** F3 (`L` per port line), F4 (LOL per `V`).

### P14: In normal mode, the PAL bit selects different decodes from PAL = 0; the page names no PAL values

**Claims:** opus:AA-beamcon0.c35, sol:AA-beamcon0.c52, opus:AA-beamcon0.b5,
sol:AA-beamcon0.c16. **Pages:** p.12, p.29.

**Proposition.** With VARBEAMEN = 0, the frame timing with BEAMCON0 bit 5
(PAL) = 1 differs from the timing with PAL = 0. With PAL = 0, every other line
is a long line of 228 CCK.

**Page text.**

- p.12: "5     PAL" and "PAL= Set appropriate decodes (in normal mode) for
  PAL."
- p.29, STRLONG: "used during the second refresh time slot of every other
  line, to identify lines with long counts(228- NTSC, HTOTAL+2- VARBEAMEN=1
  hires chips only)".
- p.29: "LOL=Long line bit".

**What is left open.** The AA pages do not state the PAL decodes: no line
count per frame and no PAL line length. "Appropriate" is not a value. The HRM
states them, but it is not the oracle for this package, so no PAL number is
graded. The only relation the page supports is that PAL = 1 decodes are not
the PAL = 0 (NTSC) decodes.

**Stimulus.**

- **Run P14-1:** `BEAMCON0 = $0020`. **Run P14-0:** `BEAMCON0 = $0000`.
- Each run has one sampling frame (F4, a full frame of `(V,H)` and LOL) and
  one stopped frame (F3).
- Additionally, the program writes a frame-sync marker at every VERTB, so the
  port gives the frame period.

**Observation.**

- The frame length in lines, `V_max + 1`, from the samples.
- The frame period, from consecutive frame-sync markers, in units of `T_L`.
- The port line lengths `L`.
- LOL per line.

**Expected.**

- **P14-0:**
  - LOL alternates between consecutive lines;
  - the port lines alternate, the longer ones being 228 CCK;
  - "every other line": no two consecutive port lines are both long.
- **P14-1:** at least one of these differs from P14-0: `V_max + 1`, or the
  line-length pattern (the sequence of `L`).

**Preconditions,** each with its contrasting case:

1. **Normal mode:** VARBEAMEN = 0 and LOLDIS = 0. Contrast: none beyond P13,
   which covers the variable mode.
2. **The frame is stable in both runs:** two consecutive frames give the same
   `V_max` and the same frame period. Contrast: none; this is a validity check
   of the measurement.
3. **F3 and F4 anchors.** Contrasts as in F3 and F4.

**Contrasting case.** P14-0 against P14-1.

**Reference frame.** F3, F4.

---

## 4. What the pages leave open, and where they disagree

1. **Revision mark of BEAMCON0.** The address list marks it `h` (p.10
   `BEAMCON0   h  1DC`), the alphabetical entry `H` (p.12 `BEAMCON0 H 1DC`).
   The p.6 legend defines only `h` ("new for HiRes chip set"). The p.10
   legend distinguishes "H= new register in hires chips" from "h= stuff added
   or changed in hires chips". The two legends also use lower-case `p`
   differently: p.6 "p=new for IAA chip set", p.10 "p= stuff added or
   changedin hires chips" (beside "P= new register in Pandora Chip Set"). No
   proposition depends on a revision mark.
2. **Revision mark of the Blitter pointers.** `BLTxPTH` is marked `h` on p.13,
   but none of `BLTAPTH`...`BLTDPTH` on p.7 is, and `BLTxPTL` is unmarked on
   both. No behaviour depends on it.
3. **Marks the alphabetical list cannot carry.** Its header (p.10) has no
   column for `~`, `+` or `&`, and its legend defines none of them. The
   CONFLICT claims about those marks therefore compare against a table that
   cannot express them. Where the alphabetical text speaks, it agrees with the
   list (P2, P7).
4. **BLANKEN.** p.12 lists BEAMCON0 bit 3 as "(unused, formerly BLANKEN)". p.29
   (VBSTRT/VBSTOP) still says "Affects CSY* pin if BLANKEN=1 & VSY* Pin if
   CSCBEN=1 (see BEAMCON0)". The HRM (p.305) still lists bit 3 as "BLANKEN
   Composite blank redirection". Only the CSCBEN half of the p.29 sentence is
   used (P9).
5. **LPE or LPEN.** p.12 names BPLCON0 bit 3 "LPE"; p.16 and p.17 name it
   "LPEN". It is the same bit number on both pages.
6. **The VARCSYEN paragraph** (p.13) opens a parenthesis at "(VARCSY is set on
   HSSTRT match always," that is never closed. Its notation is also
   ambiguous: "reset on HSSTOP match when VSY*, and on both HBSTRT & HBSTOP
   matches during VSY". "When VSY*" is read as "outside vertical sync" (the
   inverted signal) and "during VSY" as "in vertical sync". The page does not
   say whether this vertical sync is the hardwired one or the one set by
   VSSTRT/VSSTOP. P9 makes the two coincide as far as it can, and grades only
   that HCENTER matters on vertical-sync lines.
7. **"Variable composite sync" and VARCSY.** The CSCBEN paragraph (p.12)
   speaks of "the variable composite sync". The VARCSYEN paragraph (p.13)
   defines VARCSY, the composite sync "from the variable decoders". The page
   never says they are the same signal. P9 reads them as one. P9 (a) grades
   HSSTRT, and the definition of VARCSY is what ties HSSTRT to that signal.
8. **Modulo width.** p.6: "MOD=15 bit Modulo ... Uses 15 bits, plus sign
   extend". The page does not say which bit is the sign or whether bit 0 is
   used. HRM p.172 calls it a signed 16-bit byte value whose least significant
   bit is ignored. P5 uses only values with bit 15 equal to bit 14 and bit 0
   clear, which every reading takes the same way.
9. **"Increment".** p.13 says the pointer ends at "the last data address (plus
   increment and modulo)" but never states the increment's size. P4 and P5
   measure it (F2); HRM p.172 gives one word.
10. **Copper access to registers.** COPCON on p.19-20 says CDANG "Allows
    coprocessor access to all RGA registers if true", which includes `000-03E`.
    HRM p.26 says the Copper can never write `$00-$3E`. P7 uses only `048-066`,
    where both agree.
11. **HSSTRT's description.** HSSTRT's alphabetical entry (p.24) repeats
    HSSTOP's description, "Horiz line position for HSYNC stop". The line below
    it says "Sets # of color clocks for sync start". The address list (p.10)
    calls it "Horizontal Sync start (VARHSY)". P9 uses the start meaning.
12. **PAL decodes.** p.12 gives none (P14). Neither does p.29, whose STRLONG
    text gives the long-line length for NTSC (228) and for VARBEAMEN = 1
    (HTOTAL+2), not for PAL in normal mode.
13. **DMA enables for UHRES.** p.16 "(also needs bits in DMACON)" and p.22
    "DMAEN ... (also UHRES dma)" do not settle which DMACON bits UHRES needs.
    P12 tries DMAEN alone first and reports the setting used.
14. **UHRES modulo.** p.15 says BPLHMOD is "added to the UHRES bit plane
    pointer ... every line, and then another 2 is added". Three lines later it
    says "Its modulo is added every time it comes out". With DUAL the pointer
    comes out more than once per line, so the two differ. P12 grades counts,
    not addresses. p.15 also prints "SHRHDAT" where SPRHDAT is meant.
15. **Composite blank.** The pages do not define the composite blank's
    horizontal part or polarity (P9 (b) grades only its dependence on VBSTRT).
16. **Pages not seen as images.** `tools/show_page.py` renders only the
    Hardware Reference Manual's PDF, and no PDF of the AA specification is
    among the readable references. The AA pages' lines that rest on a single
    reading, including all of the Blitter pointer and modulo prose on p.13 and
    the LPENDIS and DUAL paragraphs on p.12, could therefore not be compared
    with the printed page. In substance, the HRM corroborates:
    - the Blitter prose: HRM p.270 has the same text with "18-bit" for
      "20 bit";
    - LPENDIS: HRM p.305 has "Ignore latched pen value on vertical pos read".

    Nothing corroborates the DUAL paragraph.

---

## 5. Claim index

| Claim id | Used in |
|---|---|
| opus:AA-beamcon0.b10 | P9 |
| opus:AA-beamcon0.b13 | P8 |
| opus:AA-beamcon0.b15 | not used (section 1) |
| opus:AA-beamcon0.b3 | not used (section 1) |
| opus:AA-beamcon0.b5 | P14 |
| opus:AA-beamcon0.c10 | P8 |
| opus:AA-beamcon0.c15 | P9 |
| opus:AA-beamcon0.c16 | P9 |
| opus:AA-beamcon0.c18 | P9 (second half); first half not used (section 1) |
| opus:AA-beamcon0.c19 | P10 |
| opus:AA-beamcon0.c30 | P11 |
| opus:AA-beamcon0.c31 | P12 |
| opus:AA-beamcon0.c32 | P12 |
| opus:AA-beamcon0.c35 | P14 |
| opus:AA-beamcon0.c37 | not used (section 1) |
| opus:AA-beamcon0.c38 | P9 |
| opus:AA-beamcon0.c8 | P8 |
| opus:AA-beamcon0.c9 | P8 |
| opus:AA-bltxmod.c1 | P1 |
| opus:AA-bltxmod.c2 | P1 |
| opus:AA-bltxmod.c3 | not used (section 1) |
| opus:AA-bltxmod.c4 | P7 |
| opus:AA-bltxmod.c5 | P1 |
| opus:AA-bltxmod.c6 | P5 |
| opus:AA-bltxmod.c7 | P6 |
| opus:AA-bltxpth.c1 | P1 |
| opus:AA-bltxpth.c10 | P3 |
| opus:AA-bltxpth.c11 | P4 |
| opus:AA-bltxpth.c2 | P1 |
| opus:AA-bltxpth.c3 | not used (section 1) |
| opus:AA-bltxpth.c4 | not used (section 1) |
| opus:AA-bltxpth.c5 | P2 |
| opus:AA-bltxpth.c6 | P7 |
| opus:AA-bltxpth.c7 | P2 |
| opus:AA-bltxpth.c8 | P2 |
| opus:AA-bltxpth.c9 | P1 |
| opus:AA-bltxptl.c1 | P1 |
| opus:AA-bltxptl.c2 | P1 |
| opus:AA-bltxptl.c3 | not used (section 1) |
| opus:AA-bltxptl.c4 | P2 |
| opus:AA-bltxptl.c5 | P7 |
| opus:AA-bltxptl.c6 | P2 |
| opus:AA-bltxptl.c7 | not used (section 1) |
| sol:AA-beamcon0.c11 | P9 |
| sol:AA-beamcon0.c16 | P14 |
| sol:AA-beamcon0.c18 | not used (section 1) |
| sol:AA-beamcon0.c24 | P8 |
| sol:AA-beamcon0.c25 | P8 |
| sol:AA-beamcon0.c26 | P8 |
| sol:AA-beamcon0.c33 | P9 |
| sol:AA-beamcon0.c34 | P9 |
| sol:AA-beamcon0.c35 | P10 |
| sol:AA-beamcon0.c47 | P11 |
| sol:AA-beamcon0.c48 | P12 |
| sol:AA-beamcon0.c49 | P12 |
| sol:AA-beamcon0.c50 | not used (section 1) |
| sol:AA-beamcon0.c52 | P14 |
| sol:AA-beamcon0.c53 | P13 |
| sol:AA-beamcon0.c54 | not used (section 1) |
| sol:AA-beamcon0.c6 | not used (section 1) |
| sol:AA-beamcon0.c8 | P8 |
| sol:AA-bltxmod.c1 | P1 |
| sol:AA-bltxmod.c2 | P1 |
| sol:AA-bltxmod.c3 | not used (section 1) |
| sol:AA-bltxmod.c4 | P1 |
| sol:AA-bltxmod.c5 | P5 |
| sol:AA-bltxmod.c6 | P5 |
| sol:AA-bltxmod.c7 | P6 |
| sol:AA-bltxmod.c8 | P6 |
| sol:AA-bltxpt.c1 | P2 |
| sol:AA-bltxpt.c2 | P1 |
| sol:AA-bltxpt.c3 | P3 |
| sol:AA-bltxpt.c4 | P4 |
| sol:AA-bltxpth.c1 | P1 |
| sol:AA-bltxpth.c2 | P1 |
| sol:AA-bltxpth.c3 | not used (section 1) |
| sol:AA-bltxpth.c4 | P2 |
| sol:AA-bltxptl.c1 | P1 |
| sol:AA-bltxptl.c2 | P1 |
| sol:AA-bltxptl.c3 | not used (section 1) |
| sol:AA-bltxptl.c4 | P2 |
