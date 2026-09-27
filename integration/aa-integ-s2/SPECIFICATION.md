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

The package has 81 claims. 60 of them support the propositions P1-P14 in
section 3; P11 carries none of them and is the measurement P12 needs. The
other 21 cannot be graded whole by any proposition; section 1 lists them with
the reason for each. Section 5 maps every claim id to where it is used.

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
| opus:AA-beamcon0.c18 | The claim joins two statements. "The normal monitor uses the normal composite sync" is a statement about the CSY* pin, which cannot be seen. The rest ("the variable composite sync & blank come out the HSY* & VSY* pins") restates the CSCBEN routing that P9 grades under opus:AA-beamcon0.c15, opus:AA-beamcon0.c16, sol:AA-beamcon0.c33 and sol:AA-beamcon0.c34. A claim id is graded whole or not at all, and no test can grade the first half, so the id is not used. |
| opus:AA-bltxpth.c2, opus:AA-bltxptl.c2, opus:AA-bltxmod.c2 | Each says the register "is write only: the entry's type column is W". The legends define only "W=Write, R=Read," (p.6, p.10) and give no value for a read of a W address, so "only" has no observable consequence and cannot be graded. That the entry's type is W, and that a write to it takes effect, is graded in P1 under sol:AA-bltxpth.c2, sol:AA-bltxptl.c2 and sol:AA-bltxmod.c2 ("makes ... W access"). |
| opus:AA-beamcon0.c30, sol:AA-beamcon0.c47 | "DUAL= Run the horizontal comparators with the alternate horizontal beam counter" (p.12). The claim covers all the horizontal comparators, which p.24 names for the alternate counter: "doing the comparisons for HBSTRT,STOP,HTOTAL,HSSTRT,HSSTOP". P11 grades HTOTAL only, so neither id is graded whole. P11 stays, without claim ids, as the measurement P12 needs. |

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

- **Configuration:** `Config.chipset_config = 0x10` (AA selected) and 2 MB
  of Chip memory (`Config.memory_config = 0x03`, "11 = 2.0 MB" in
  `nanomig_harness.h`). The test reads these from the harness configuration
  and asserts them before every run. P2 depends on the 2 MB. PAL is not a
  harness setting: `Config` has no field for it. Every run selects the PAL
  decodes itself, through BEAMCON0 bit 5 (2.2).
- **CPU cores and which build is graded:** every run is executed on both
  builds `nanomig.mk` makes, with the TG68K core left in its default 68000
  mode. The program uses only 68000 instructions. The AA specification is
  graded on the TG68K build only: it is the build made with `ENABLE_AGA`
  (`nanomig.mk`'s `test-tg68k` rule passes `-DENABLE_AGA`), NanoMig's AGA
  configuration. The fx68k build is made without `ENABLE_AGA`, NanoMig's
  non-AGA configuration, so it is not an AA configuration: its results are recorded, not graded, and no proposition,
  precondition or anchor depends on the fx68k build having an AA feature.
  Parameters that depend on CPU speed (sample counts, pulse position in P8)
  are fitted per build from a dry run, under the constraints stated where
  they are used.
- **Register addresses:** a custom register is at `$DFF000` plus the offset
  printed on pp.6-10.

### 2.2 Program conventions

- **Markers.** The program announces events by writing a word, the event code,
  to a marker area in Chip memory. The port shows each such write with its
  time. Custom-register reads and writes are not Chip-memory accesses, so they
  carry no time at the port. Every "time of X" in this document is the time of
  the marker the program writes immediately after or before X, as stated.
- **Results area.** Values the program reads from registers (DMACONR,
  INTREQR) and the outcome of every wait are stored in the results area,
  from `RESULTS` (`$0C0000`) up to, not including, `DONE_ADDR` (`$0C0FFE`,
  where the `DONE` mark goes); both constants are `nanomig.i`'s.
- **Sample buffer.** Beam samples (F4) are stored from `$1B0000` upward and
  never at or above `$200000`. How many samples a frame holds depends on the
  CPU core, so they do not go into the results area, which ends at
  `DONE_ADDR`: the harness ends a run at the first write of `DONE_VALUE`
  (`$600D`) to `DONE_ADDR`, and a sample could be that write. No other
  address this document fixes lies in `$1B0000-$1FFFFF`.
- **Addresses the harness fixes.** The program starts at `PROGRAM_ORG`
  (`$080400`) and the stack grows down from `PROGRAM_STACK` (`$0FFF00`), both
  `nanomig.i`'s. No Blitter region, list or buffer of this document lies in
  the results area or between `$0F8000` and `PROGRAM_STACK`, which is left
  to the stack.
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
   4-0 of each PTH only ("High 5 bits"), so bits 15-5 of PTH are 0. The one
   exception is P2's contrast from above, which sets PTH bit 5 on purpose.
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
- **Anchor check.** Every enabled channel shows exactly `W*H` accesses. P1-P5
  and P7 take this as a precondition. P6 does not: that every channel
  processes the same area is what P6 grades, so there the count is an
  expectation (section 3, P6).
- **Increment.** Within every line, consecutive addresses must differ by one
  constant step `inc`, measured in the same run. This `inc` is the "increment"
  of p.13, whose size that page does not state. HRM p.172 says one word. Only
  its constancy is a precondition here; its size is not graded.

**F3: colour clock and port line.**

- **Colour clock.** `T_C` is one colour clock (CCK) in system clocks. p.21
  gives the memory cycle as "(memory cycles are 280 ns)", and p.29 gives the
  horizontal beam position the same unit: VHPOSR `H8 ... H1`,
  "RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)". `T_C` is therefore measured at
  the port as the time per unit of `H` (F4): in the F4 dry run, for each line
  `V` with at least two samples, the store time of the line's last sample
  minus that of its first,
  divided by their difference in `H`, averaged over all lines of the frame
  and rounded to a whole number of system clocks. Every sample is stored by
  the same instruction after its read, so the difference cancels the delay
  between read and store. No harness constant and no sampling rate of the
  harness enters `T_C`.
- **Port line.** p.29 says "There are 4 refresh time slots" in every line.
  In a stretch where all DMA enables are clear and the CPU is halted, every
  port access is a refresh access. The CPU is halted by a 68000
  `STOP #$2000`, woken by the level-3 vertical-blank interrupt (p.25:
  "05 VERTB 3 Start of Vertical blank"). For that the program, once, before
  its first `STOP`:
  1. Clears the ROM overlay. After reset "An additional copy of the system
     ROM responds starting at memory location $00000000. The system RAM that
     would normally be located at this address is not available" (HRM
     p.223), and while the overlay is on, the harness serves CPU reads of
     `$000000-$07FFFF` from the ROM shadow and a CPU write there reaches no
     memory (`nanomig_harness.h`). "The OVL bit in one of the 8520 Chips
     disables the overlay" (HRM p.223): the program writes CIA-A `ddra`
     (`$BFE201`) = `$03` first, which makes bit 0 an output (HRM p.339
     "Direction for port A (BFE001);1=output (set to 0x03)"), then CIA-A
     `pra` (`$BFE001`) with bit 0 (`OVL`) = 0.
  2. Writes the level-3 autovector at `$6C`. It points at a handler in the
     program image that writes `INTREQ = $0020` and returns with `RTE`.

  Around every `STOP` the program writes `INTREQ = $0020`, then
  `INTENA = $C020` (SET, bit 14 INTEN "Master interrupt (enable only , no
  request)", bit 5 VERTB; p.25), then `STOP #$2000`. After waking it writes
  `INTENA = $7FFF`, the initial state of 2.2. p.24: request bits "may be
  polled by the processor, and if enabled by the bits listed in the next
  register, they may cause processor interrupts". VERTB is enabled only
  across the `STOP`, so every polling wait of 2.2 still sees VERTB set.

  If the program cannot own the vector, refresh accesses are instead the
  accesses outside every address range the program uses.
- **Line start and length.** A port line starts at the first access of each
  group of four. `T_L` is the time between consecutive group starts, and
  `L = T_L / T_C` is the line length in CCK.
- **Anchor check.** Refresh accesses come in groups of exactly four, one group
  per line period, and every `T_L` is a whole multiple of `T_C`, so `L` is an
  integer on every line.
- **Line numbers.** The program writes a marker `VMARK(v)` as soon as its
  VHPOSR polling sees vertical position `v`. The port line containing
  `VMARK(v)` is line `v`; the following port lines are `v+1`, `v+2` and so on.

**F4: beam samples.**

- **Sample loop.** The CPU reads VPOSR and VHPOSR as one long word
  (`MOVE.L $DFF004,(An)+`) and stores it, with `An` in the sample buffer
  (2.2). The store is a Chip-memory write, so every sample carries a time.
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
HSSTRT = `$0020`, HSSTOP = `$0030`, VSSTRT = 100, VSSTOP = 103,
HTOTAL = `$00E2`, VTOTAL = `$0138`, and BEAMCON0 has PAL (bit 5) and
VARBEAMEN (bit 7) set and every other bit 0 except those named below.

**VARBEAMEN in every run of F5, P9 and P10.** The registers these runs
depend on (HSSTRT, HSSTOP, HCENTER, HBSTRT, HBSTOP, VSSTRT, VSSTOP, VBSTRT)
are compared with the beam counters. p.12: "VARBEAMEN= Enables the variable
beam counter comparators to operate (allowing different beam counter total
values) on the main horiz counter". p.24 says of HSSTRT and HSSTOP only "See
BEAMCON0 for details of when these 2 are active", and of HCENTER that it "is
necessary for interlace mode with variable beam counters". The pages do not
say that VARHSYEN, VARVSYEN or CSCBEN alone makes the comparators operate,
so VARBEAMEN is set together with them (section 4, item 17). With VARBEAMEN
set, HTOTAL and VTOTAL are in use (p.24 HTOTAL "Active if VARBEAMEN=1 or
DUAL=1"; p.29 VTOTAL "Highest numbered vertical line (VARBEAMEN=1)"), so
every run writes them: HTOTAL = `$00E2` gives lines of 227 CCK (p.24 "Horiz
line has this many + 1 280ns increments"; with PAL set there is no long line,
p.13), and VTOTAL = `$0138` gives 313 lines (p.29 "there's this many + 1 in
a field"). Every horizontal position written in F5, P9 and P10 lies below
HTOTAL, and every vertical one below VTOTAL.

- **S-H1:** `BEAMCON0 = $01A0` (PAL + VARBEAMEN + VARHSYEN).
- **S-H2:** the same with HSSTOP = `$0034`.
- **S-H3:** `BEAMCON0 = $01A1` (adds HSYTRUE).
- **S-V1:** `BEAMCON0 = $02A0` (PAL + VARBEAMEN + VARVSYEN).
- **S-V2:** the same with VSSTOP = 105.
- **S-V3:** `BEAMCON0 = $02A2` (adds VSYTRUE).

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
  - On each line it differs in one interval of exactly `4*T_C`.
  - It is identical elsewhere.
  - The line period is the same in both runs.
- **S-H3 against S-H1:** the observed horizontal sync is the logical inverse
  at every sample.
- **S-V1 against S-V2:**
  - The observed vertical sync differs in exactly one interval, of exactly
    `2*T_L`.
  - It is identical elsewhere.
- **S-V3 against S-V1:** the observed vertical sync is inverted at every sample.

**Interval lengths are exact.** The pages state no tolerance for them, so
none is applied, here or in P9 and P10. An interval is counted in video
samples, one per `clk_sys` (`nanomig_harness.h`: `video()` holds one
`VideoSample` per cycle), and must be exactly `4*T_C` or `2*T_L` long, with
`T_C` and `T_L` measured in the same run: `T_L` from that run's refresh
groups over the graded frame (F3; F6 already requires them), and `T_C` from
an F4 dry-run frame that the same program takes before it writes the
registers of the run (F3). Where the two lines of a `2*T_L` interval differ
in length, `2*T_L` is the sum of their own `T_L`.

A chip line on the output is an interval of length `T_L` (F3) that starts at
an active edge of S-H1's observed horizontal sync. `V_var` is the output
interval in which S-V1's vertical sync is active. `V_norm` is the interval in
which the vertical sync is active in the reference run R0
(`BEAMCON0 = $00A0`, PAL + VARBEAMEN with no sync enable, same registers, so
that its beam counters run as in the runs it is compared with).

Where a part needs line numbers on the output (P9 (b)), they are counted
from `V_var`. p.12 sets the variable VSY "vertically on VSSTRT", "with the
horizontal position for set & reset HSSTRT on short fields (all fields are
short if LACE=0)", and VSSTRT = 100. Chip line 100 is the interval of length
`T_L` that starts where `V_var` starts, and chip line `n` starts
`(n - 100) * T_L` after it. This numbering uses S-V1 and F3 only, so the
VSY* parts do not depend on S-H.

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
sol:AA-bltxpth.c1, sol:AA-bltxptl.c1, sol:AA-bltxmod.c1, sol:AA-bltxpth.c2,
sol:AA-bltxptl.c2, sol:AA-bltxmod.c2, opus:AA-bltxpth.c9, sol:AA-bltxpt.c2,
opus:AA-bltxmod.c5, sol:AA-bltxmod.c4. **Pages:** p.13, p.7.

The "write only" claims of the same entries are declined (section 1): P1
grades the type W and that a write takes effect, not what a read returns.

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
  | C | `$0B0000` | `$0B0000-$0BFFFF` | 8 |
  | B | `$0D0000` | `$0D0000-$0DFFFF` | 16 |
  | A | `$0E0000` | `$0E0000-$0EFFFF` | 24 |
  | D | `$0F0000` | `$0F0000-$0F7FFF` | 32 |

  The results area (`RESULTS` `$0C0000` to `DONE_ADDR` `$0C0FFE`) is written
  inside the window of every blit (DMACONR, template 2.3 step 7), and the
  stack grows down from `$0FFF00`; no region reaches either (2.2).

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
  gets `$0B0000`. The page then predicts that P1-A reads start at `$0B0000`
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
20 address bits individually, and PTH bit 5 is not an address bit: the same
pointer written with PTH bit 5 set addresses the same word as with it clear.

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

**Contrast from above.** For each channel `x`, one more blit as above with
`V1`, except that channel `x`'s PTH is written as `$0035` (bit 5 set; PTL
`$5554`), four in all. p.6: "+=Address register pair.Low word uses
DB1-DB15,High word DB0-DB4." DB5 of the high word is not among the bits the
pair uses, so the page predicts the same access as with PTH `$0015`. An
implementation that takes PTH bit 5 as address bit 21 accesses byte
`$355554` (word `$1AAAAA`), which the port can show: the harness logs
`ram_address` as a 22-bit word address (`nanomig_harness.h`, `kRamWordCount
= 1u << 22`). For these blits `R_x` is `V1` to `V1 + $FF`, and the window is
also searched for any access at byte `$355554-$355653`.

**Observation.** The channel's single access (F1), and in the contrast from
above any access in the window at byte `$355554-$355653`.

**Expected.**

| Value written | Word address of the access |
|---|---|
| V1 = `$155554` | `$0AAAAA` |
| V2 = `$0AAAAA` | `$055555` |
| V1 with PTH `$0035` | `$0AAAAA`, and no access at byte `$355554-$355653` |

This holds for every channel. A dropped, stuck or swapped address bit makes at
least one of the first two differ; a pointer wider than the "20 bit address"
(p.13) makes the third differ.

**Preconditions,** each with its contrasting case:

- **2 MB of Chip memory,** since V1 lies above 1 MB. Asserted from the
  configuration.
- **The program image, stack and results do not overlap either region.**
  Asserted from the program's own address map.
- **F1 anchor and blit finished,** as in P1, with the same contrasts.

**Contrasting case.**

- **From below:** V1 against V2. Every bit changes, so the page predicts a
  different address for every bit.
- **From above:** V1 with PTH bit 5 set against V1. The page predicts the same
  address, so a correct pair shows no difference, and a pair that uses a 21st
  bit shows one. Only bit 5 is set: it is the bit that would be the next
  address bit, and PTH bits 15-6 stay 0 (2.3).

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
| C | `$0B0100` |
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
  P1 (A `$0E0000`, B `$0D0000`, C `$0B0000`, D `$0F0000`). Modulos:

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
| C | `$0B000C` |
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

- **F1 anchor, and the blit finished:** BLIT was seen and BBUSY = 0.
  Contrasts as in P1. The number of accesses per channel is not part of this
  precondition, unlike in P1: "an identical area of each is used" is what the
  second expectation grades, so a channel with too many or too few accesses
  is red there, not ungraded.

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
   | C | `$0B0000` | 8 |
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
  `$0E0000`, `$0D0000`, `$0B0000` and `$0F0000`, and every step is
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

1. **In every frame `k`, the first word of the MOVE to register `k` was
   fetched:** a read of `L_k+8` returned that register's address. A register
   whose MOVE was never reached is not graded, and the run fails its
   precondition. The second word (`L_k+10`) is observed and recorded but is
   not part of the precondition: the page says nothing about whether a
   Copper that may not write a register fetches the data word of the MOVE,
   so requiring it in P7-0 would make the precondition depend on the clause
   under test. In P7-1 a missing data fetch shows as a register that keeps
   its base value, which is red. Contrast: replace that MOVE in `L_k` by `MOVE COLOR00,$0000`
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
depending on the machine, and HRM p.229 names pin 6 as the fire-button line
("Input and output for pin 6 (port 1 & 2 fire buttons)"). The harness has
no light-pen function of its own. The test drives pin 6 of both controller
ports together through the fire inputs of the port list's `joystick0` and
`joystick1`, set with `ports()`, so the port question does not arise.

**Stimulus.**

1. Set `DMACON = $7FFF` and `BPLCON0 = $0000`.
2. Set `BEAMCON0 = $0020` (P8-0: LPENDIS = 0) or `$2020` (P8-1: LPENDIS = 1).
3. Frame sync, then poll VPOSR/VHPOSR (F4) until `V = 254` and `H >= $80`.
4. Set `BPLCON0 = $0008` (LPEN).
5. Write marker `LP_GO`.
6. Take `N` samples in one loop: the F4 long word, then HHPOSR (word).
7. Write marker `LP_END`, then set `BPLCON0 = $0000`.

**The pulse.** The test drives pin 6 of both controller ports to its active
(pressed) level for at most `2*T_C`, then releases it. The pulse starts at `LP_GO + D`, where `D` and
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
    time in CCK (`T_C`, F3), plus one sample interval. The added sample
    interval is the quantisation of the measurement, not a page value: a
    sample is dated by its store, and the read that produced it happened up
    to one sample interval earlier.
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

1. **The trigger reaches the chip:** the pulse freezes `(V,H)` in at least
   one of P8-0 and P8-1 (all "after" samples identical). Which LPENDIS
   setting shows the latched value is what P8 grades, so the precondition
   does not name one: an implementation that reads the latch with LPENDIS
   high instead of low still passes this precondition and is red in both
   expectations. If neither run freezes, the chip did not see the trigger,
   and P8 is reported as not checkable. The green of P8-1 means something
   only when this precondition holds. Contrast: P8-dry (no pulse), whose
   samples advance throughout. The page speaks of a latched value only where
   a hit exists; HRM p.239 describes the trigger.
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
opus:AA-beamcon0.c16, sol:AA-beamcon0.c33, sol:AA-beamcon0.c34,
opus:AA-beamcon0.c38. **Pages:** p.12, p.13, p.24, p.29.

**Proposition.** With BEAMCON0 bit 10 (CSCBEN) = 1, VARHSYEN = 0 and
VARVSYEN = 0 (and VARBEAMEN = 1, F5):

- (a) the HSY* output carries the variable composite sync. It becomes active
  at the HSSTRT position on every line, and on vertical-sync lines HCENTER
  also affects it.
- (b) the VSY* output carries the variable composite blank, which follows
  VBSTRT.

With CSCBEN = 0 (VARBEAMEN still 1) none of these registers affects the two
pins.

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
| HTOTAL | `$00E2` | highest CCK count of a line (F5) |
| VTOTAL | `$0138` | highest line number (F5) |

The HBSTRT, HBSTOP and HCENTER values follow the page's own recipe (p.13):
"A reasonable composite can be generated by setting HCENTER half a horiz line
from HSSTRT, and HBSTOP at (HSSTOP-HSSTRT) before HCENTER, with HBSTRT at
(HSSTOP-HSSTRT) before HSSTRT."

The runs come in pairs that differ in one register.

- **With `BEAMCON0 = $04A0` (PAL, VARBEAMEN, CSCBEN):**
  - **H-pair:** HSSTRT `$20` against `$24`.
  - **C-pair:** HCENTER `$91` against `$99`. HBSTOP (`$81`) stays below both.
  - **V-pair:** VBSTRT 150 against 152.
- **With `BEAMCON0 = $00A0` (PAL, VARBEAMEN):** the same three pairs (the
  contrasts). They differ from the runs above in CSCBEN alone.

**Observation.** The observed horizontal sync (for the H-pair and C-pair) and
vertical sync (for the V-pair), over the graded frame (F6), compared sample by
sample between the two runs of a pair. Chip lines, `V_var` and `V_norm` are as
in F5.

**Expected.**

- **(a) H-pair, CSCBEN = 1:**
  - The two runs differ on every chip line of the graded frame. "Always" is
    graded over all lines, including vertical-sync and vertical-blank lines.
  - On each line they differ in one interval of exactly `4*T_C` (F5,
    "Interval lengths are exact").
  - They are identical elsewhere.
- **(a) C-pair, CSCBEN = 1:**
  - The runs differ on at least one chip line of the graded frame. On every
    line where they differ, they differ in one interval of exactly `8*T_C`
    (HCENTER `$91` against `$99`; p.24 HCENTER "Horizontal position (CCKs)";
    F5, "Interval lengths are exact"), and are identical elsewhere.
  - The differing lines are consecutive, and there are exactly as many as
    there are lines in vertical sync under one of the two readings of which
    vertical sync the page means (section 4, item 6):
    - `V_var`: exactly 3. p.12 sets the variable VSY on line VSSTRT = 100 and
      resets it on line VSSTOP = 103, both at the horizontal position HSSTRT
      (`$20`), which lies before HCENTER in every line, so HCENTER falls
      inside `V_var` on lines 100, 101 and 102 only.
    - `V_norm`: `floor(|V_norm| / T_L)` or `ceil(|V_norm| / T_L)`, where
      `|V_norm|` is the length of `V_norm`. The page does not fix where in a
      line the hardwired vertical sync starts, and an interval of that length
      holds one or the other number of HCENTER positions, one line apart,
      depending on that phase. The "or" is the quantisation of counting whole
      lines, not a page value.
  - Where the differing lines lie on the HSY* output relative to `V_var` and
    `V_norm` on the VSY* output is not graded. The two outputs reach the port
    by different paths (the harness's `hs` comes from the scan doubler, its
    `vs` is passed through; `nanomig_harness.h`), and nothing in the pages or
    the harness fixes the delay between them in whole lines, so a
    line-by-line comparison across the two outputs would rest on an offset no
    anchor measures.
  - Setting VSSTRT/VSSTOP makes "in vertical sync" a known set of lines under
    either reading of which vertical sync the page means.
  - This is what makes the signal a composite sync rather than a plain
    horizontal sync: HCENTER changes it on one block of consecutive lines
    only.
- **(b) V-pair, CSCBEN = 1:**
  - Every differing sample lies within one interval of exactly `2*T_L` that
    starts in chip line 150 (numbered from `V_var`, F5; both on the VSY*
    output). The page does not say at which horizontal position the vertical
    blank starts, but only VBSTRT differs between the two runs, so the blank
    starts at the same point of the line in both, on line 150 in one run and
    on line 152 in the other: the interval is exactly two lines long,
    wherever in line 150 it starts.
  - Each of the interval's two `T_L` halves contains differing samples.
  - Polarity and the horizontal content of the composite blank are not
    graded: the page does not state them, and where horizontal blanking
    covers a sample in both runs, the runs need not differ there.
- **Contrasting pairs, CSCBEN = 0:** all three pairs identical at every sample.

**Preconditions,** each with its contrasting case:

1. **F5 anchor S-H holds** (for (a)) and S-V holds (for (b)). The anchor's
   contrasts are part of F5. If S-H or S-V fails, the corresponding part is
   reported as not checkable, and its claim ids come off the assertion
   (they are recorded as not graded, with the failed anchor as the reason):
   - (a) grades opus:AA-beamcon0.c15, sol:AA-beamcon0.c33 and
     opus:AA-beamcon0.c38;
   - (b) grades opus:AA-beamcon0.c16 and sol:AA-beamcon0.c34;
   - opus:AA-beamcon0.b10 and sol:AA-beamcon0.c11 ("10 CSCBEN") are graded
     by either part, and come off only if neither is checkable.
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

- **With `BEAMCON0 = $05A0` (PAL, VARBEAMEN, CSCBEN, VARHSYEN):**
  - **C-pair:** HCENTER `$91` against `$99`.
  - **S-pair:** HSSTOP `$30` against `$34`.
- **With `BEAMCON0 = $06A0` (PAL, VARBEAMEN, CSCBEN, VARVSYEN):**
  - **V-pair:** VBSTRT 150 against 152.
  - **VS-pair:** VSSTOP 103 against 105.

VARBEAMEN is set in every run, as in F5.

**Observation.** As in P9.

**Expected.**

- **With `$05A0`:**
  - The C-pair is identical at every sample.
  - The S-pair differs on every chip line, including the vertical-sync lines,
    in one interval of exactly `4*T_C` (F5, "Interval lengths are exact").
    During VSY the composite would not be reset at HSSTOP (p.13), so a
    difference on those lines shows the comparator HSY.
- **With `$06A0`:**
  - The V-pair is identical at every sample.
  - The VS-pair differs in exactly one interval, of exactly `2*T_L` (F5,
    "Interval lengths are exact").

**Preconditions,** each with its contrasting case:

- **F5 anchors S-H and S-V.** Contrasts as in F5. Both claims,
  opus:AA-beamcon0.c19 and sol:AA-beamcon0.c35, say that VARVSYEN and
  VARHSYEN have priority, so each needs both halves: if S-H or S-V fails,
  both ids come off the assertion and are recorded as not graded, with the
  failed anchor as the reason; the checkable half is still run and reported.
- **Cycle alignment (F6).** Contrast: none; this is a validity check on the
  comparison.
- **The vertical-sync lines exist in the graded frame.** Asserted as in P9.

**Contrasting case.**

- Clear VARHSYEN (`$04A0`): the C-pair differs on the vertical-sync lines
  (P9 (a)).
- Clear VARVSYEN (`$04A0`): the V-pair differs on lines 150-151 (P9 (b)).

**Reference frame.** As in P9.

### P11: With DUAL set, HTOTAL bounds the alternate horizontal counter and not the main one

**Claims:** none. **Pages:** p.12, p.24.

P11 is the measurement P12 needs (the alternate counter's period, P12
precondition 3 and `k`), and its assertion carries no claim id. The claims
about DUAL's comparators name all the horizontal comparators, and P11 grades
HTOTAL only, so they are declined (section 1).

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

1. **The alternate counter runs.** On every main line of the frame, HHPOSR
   takes at least two different values. Otherwise "at most `$38`" could pass
   with a stuck counter. Where the counter wraps is not part of this
   precondition: that HTOTAL bounds it is what the expectation grades, and a
   counter that ignores HTOTAL has to be red there, not fail a precondition.
   Contrast: none beyond the contrasting cases below, which run the same
   counter under other bounds.
2. **F4 anchor.** Contrast as in F4.

**Contrasting case.**

- For the alternate counter's period: the DUAL run with `HTOTAL = $00E2`.
  HTOTAL is the alternate counter's highest count, so the page predicts that
  its period grows by `($E2 + 1) / ($38 + 1)`. The number of HHPOSR decreases
  (wraps) per main line must fall by that ratio, to within 1. The 1 is the
  quantisation of the count, not a page value: wraps are counted in whole
  units, and a main line holds the whole number of periods just below or
  just above its length divided by the period, depending on where in the
  line the first wrap falls.
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
| `DMACON` | `$8320` (DMAEN, BPLEN, SPREN) |
| `SPRxPTH/L`, x = 0-7 (`120`-`13E`) | `$1A0000`, a null sprite |

1. **Run P12-0:** `BEAMCON0 = $0020`. **Run P12-1:** `BEAMCON0 = $0060`
   (DUAL).
2. In the graded frame the program writes `VMARK(90)` (F3). It then halts the
   CPU with `STOP` until the next vertical blank (F3), so that the port shows
   only refresh, UHRES and null-sprite traffic in lines 90-120.

DMACON sets DMAEN, BPLEN and SPREN together in both runs. p.16 says UHRES
"also needs bits in DMACON" without naming them, and p.22 names DMAEN ("also
UHRES dma"); the pages do not say which bits suffice, so the bit-plane and
sprite enables are set with it (section 4, item 13). BPLCON0 has BPU = 0, so
BPLEN fetches no bit plane (p.16 "BPUx =Bit plane use code 0000-1000 (NONE
thru 8 inclusive)"). The eight sprite pointers point at a null sprite at
`$1A0000`, whose first two words the program sets to zero; its region
`$1A0000-$1AFFFF` is excluded from the counts. The program rewrites the
sprite pointers after every frame sync (p.28: "These address registers must
be initialized by the processor or coprocessor every vertical blank time").

**Observation.**

- The port accesses in the two regions, per port line (F3), on lines 102-108.
  These are the lines inside 100-110, with one line of margin at each end for
  the marker's placement.
- `k(line)`: the number of alternate-counter resets in the line. It is
  `L / T_alt`, both in CCK: `L` is the length of that port line (F3), and
  `T_alt = (HTOTAL + 1) / r` is measured in P11's DUAL run, where `r` is the
  HHPOSR count per `T_C`, the slope of the HHPOSR samples against their
  store times between wraps.

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
2. **Bandwidth is left.** No bit plane is fetched (BPU = 0), the sprites
   fetch only the null sprite, all other DMA is off, and the CPU is stopped.
   Contrast: none. The page conditions the claim on free bandwidth, and this
   setup leaves as much as the enables above allow.
3. **The alternate counter wraps at least twice per main line,** so `k >= 2`:
   in P11's DUAL run, HHPOSR decreases at least twice within every main line.
   Here this is a precondition and not the clause under test, which is the
   UHRES chain. Contrast: `HTOTAL = $00E2` makes `k` fall. The page
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
  - there are as many 228-CCK port lines in frame B as LOL = 1 lines in
    frame A, to within 1. The 1 is the quantisation of the count, not a page
    value: frames A and B are different frames, a frame has
    `VTOTAL + 1 = 313` lines (p.29 "there's this many + 1 in a field"), an
    odd number, so alternating lines give a frame 156 or 157 long lines
    according to which kind it starts with, and the page does not fix which.

**Preconditions,** each with its contrasting case:

1. **Variable-beam mode is in effect:** line length follows HTOTAL. Contrast:
   `HTOTAL = $00C0` in P13-1. p.24 counts both settings from HTOTAL ("this
   many + 1", "this many + 2"), so every port line of frame B is shorter by
   `$E2 - $C0 = 34` CCK than with `HTOTAL = $00E2` (shortest line against
   shortest, longest against longest). The precondition is this difference,
   not an absolute length: the absolute length is what P13 grades, and an
   implementation that got it wrong has to be red there, not fail a
   precondition.
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
    coprocessor access to all RGA registers if true. (if 0, access to
    RGA>7E)", which includes `000-03E`, and "(on old chips- access to only
    RGA>3E if CDANG=1)". HRM p.26 says CDANG "protects all registers numbered
    between $40 and $7E inclusive. This range includes the blitter control
    registers. When CDANG is 0, these registers cannot be written by the
    Copper. When CDANG is 1, these registers can be written by the Copper."
    HRM p.26 speaks of `$40-$7E` only. P7 uses only `048-066`, inside that
    range, where both agree.
11. **HSSTRT's description.** HSSTRT's alphabetical entry (p.24) repeats
    HSSTOP's description, "Horiz line position for HSYNC stop". The line below
    it says "Sets # of color clocks for sync start". The address list (p.10)
    calls it "Horizontal Sync start (VARHSY)". P9 uses the start meaning.
12. **PAL decodes.** p.12 gives none (P14). Neither does p.29, whose STRLONG
    text gives the long-line length for NTSC (228) and for VARBEAMEN = 1
    (HTOTAL+2), not for PAL in normal mode.
13. **DMA enables for UHRES.** p.16 "(also needs bits in DMACON)" and p.22
    "DMAEN ... (also UHRES dma)" do not settle which DMACON bits UHRES needs.
    P12 sets DMAEN, BPLEN and SPREN together, with no bit plane selected and
    the sprites on a null sprite.
14. **UHRES modulo.** p.15 says BPLHMOD is "added to the UHRES bit plane
    pointer ... every line, and then another 2 is added". Three lines later it
    says "Its modulo is added every time it comes out". With DUAL the pointer
    comes out more than once per line, so the two differ. P12 grades counts,
    not addresses. p.15 also prints "SHRHDAT" where SPRHDAT is meant.
15. **Composite blank.** The pages do not define the composite blank's
    horizontal part or polarity (P9 (b) grades only its dependence on VBSTRT).
16. **Lines compared with the printed page.** Several lines this document
    relies on rest on a single reading of the corpus (`[1/3]` in
    `pNN.provenance.tsv`). The band images of the printed pages
    (`docs/references/amiga-aa-1991/bands/`) show the following lines word
    for word as the corpus gives them:
    - p.13: the BLTxPTH/BLTxPTL and BLTxMOD rows and all of their prose, the
      end of the PAL paragraph ("ends up short line"), and the VARCSYEN
      paragraph with its parenthesis never closed (item 6)
      (`p13_band1.png`, `p13_band2.png`);
    - p.12: the LPENDIS, CSCBEN, VARVSYEN, VARHSYEN, VARBEAMEN, DUAL and PAL
      paragraphs (`p12_band2.png`, `p12_band3.png`);
    - p.6: the `+` legend "Low word uses DB1-DB15,High word DB0-DB4", "W=Write,
      R=Read,", "PTL,PTH=20 bit Pointer" and "Uses 15 bits, plus sign extend"
      (`p06_band1.png`, `p06_band2.png`);
    - p.21: "(memory cycles are 280 ns)" (`p21_band1.png`);
    - p.24: the HTOTAL entry, including "Horiz line has this many + 1 280ns
      increments" and "Active if VARBEAMEN=1 or DUAL=1", and the INTREQR
      text (`p24_band3.png`);
    - p.29: the STRLONG text with "There are 4 refresh time slots", the VBSTRT
      text, and the VPOSR and VHPOSR bit rows with "RESOLUTION=1/160 OF SCREEN
      WIDTH (280 NS)" (`p29_band1.png`, `p29_band2.png`).
17. **Which enable makes the beam comparators operate.** p.12 says VARBEAMEN
    "Enables the variable beam counter comparators to operate ... on the main
    horiz counter", and DUAL runs "the horizontal comparators with the
    alternate horizontal beam counter". p.24 says of HSSTRT and HSSTOP only
    "See BEAMCON0 for details of when these 2 are active", and of HCENTER
    "See BEAMCON0 for when it affects chip outputs". The VARHSYEN, VARVSYEN
    and CSCBEN paragraphs route comparator signals to pins but do not say
    whether the comparators operate without VARBEAMEN. F5, P9 and P10
    therefore set VARBEAMEN with every sync enable, and write HTOTAL and
    VTOTAL, which VARBEAMEN puts in use.

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
| opus:AA-beamcon0.c18 | not used (section 1) |
| opus:AA-beamcon0.c19 | P10 |
| opus:AA-beamcon0.c30 | not used (section 1) |
| opus:AA-beamcon0.c31 | P12 |
| opus:AA-beamcon0.c32 | P12 |
| opus:AA-beamcon0.c35 | P14 |
| opus:AA-beamcon0.c37 | not used (section 1) |
| opus:AA-beamcon0.c38 | P9 |
| opus:AA-beamcon0.c8 | P8 |
| opus:AA-beamcon0.c9 | P8 |
| opus:AA-bltxmod.c1 | P1 |
| opus:AA-bltxmod.c2 | not used (section 1) |
| opus:AA-bltxmod.c3 | not used (section 1) |
| opus:AA-bltxmod.c4 | P7 |
| opus:AA-bltxmod.c5 | P1 |
| opus:AA-bltxmod.c6 | P5 |
| opus:AA-bltxmod.c7 | P6 |
| opus:AA-bltxpth.c1 | P1 |
| opus:AA-bltxpth.c10 | P3 |
| opus:AA-bltxpth.c11 | P4 |
| opus:AA-bltxpth.c2 | not used (section 1) |
| opus:AA-bltxpth.c3 | not used (section 1) |
| opus:AA-bltxpth.c4 | not used (section 1) |
| opus:AA-bltxpth.c5 | P2 |
| opus:AA-bltxpth.c6 | P7 |
| opus:AA-bltxpth.c7 | P2 |
| opus:AA-bltxpth.c8 | P2 |
| opus:AA-bltxpth.c9 | P1 |
| opus:AA-bltxptl.c1 | P1 |
| opus:AA-bltxptl.c2 | not used (section 1) |
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
| sol:AA-beamcon0.c47 | not used (section 1) |
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
