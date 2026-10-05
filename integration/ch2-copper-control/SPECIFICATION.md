# Copper control: integration test specification

This document specifies one integration test package for the Copper-control
claims of chapter 2 of the *Amiga Hardware Reference Manual*, 3rd edition:
turning Copper DMA on and off, starting the Copper after reset, the restart
from COP1LC, the Copper interrupt, the set/clear bit of INTREQ, the masked
WAIT, and the blitter-finished wait. It was written from the package's
claims (`claims-spec-stage.jsonl`) and the manual's printed pages alone, by
an author who never saw the implementation or any test. A builder implements
the package from it and may not change what it specifies. Page numbers are
printed page numbers, and quotations follow the printed page.

---

## 1. Claims not turned into propositions

| Claim | Page | Reason |
|---|---|---|
| `ch2-stopping-the-copper.c9` | 30 | A name ("Bit 7 of DMACON is called COPEN"). No output of the chip set carries a bit's name. The bit's position and function are tested in P1. |
| `COP2-stopping-the-copper.c12` | 30 | Same name claim as above. |
| `ch2-the-copper-and-the-680x0.c4` | 35 | A name ("Bit 15 of INTREQ is named SET/CLR"). The function is tested in P10. |
| `ch2-the-copper-and-the-680x0.c6` | 35 | A name ("Bit 4 of INTREQ is named COPEN"). The function is tested in P9. The name also conflicts with other pages (section 4, item 1). |
| `COP2-the-copper-and-the-680x0.c8` | 35 | Same name claim as above. |
| `ch2-a-copper-loop-example.c18` | 33 | States what the operating system does ("COP1LC is written to by the operating system"). The test system runs only the test's own 68000 program, with no operating system and no vertical-blank server chain. The hardware behaviour the passage relies on, the restart from COP1LC, is tested in P5 and P11. |
| `ch2-a-copper-loop-example.c19` | 33 | Describes a graphics interrupt handler in the operating system's vertical-blank server chain. There is no operating system (see above). |
| `ch2-a-copper-loop-example.c20` | 33 | "As long as this server is intact": a condition on operating-system software. There is no operating system (see above). |
| `COP2-a-copper-loop-example.c22` | 33 | "The operating system writes COP1LC": there is no operating system (see above). |
| `COP2-a-copper-loop-example.c24` | 33 | "A graphics interrupt handler writes COP1LC": there is no operating system (see above). |
| `ch2-using-the-copper-with-the-blitter.c9` | 35 | "It could possibly prevent some screen displays": the page only says this is possible, so there is no expected value. It is also unclear what "This bit" refers to (section 4, item 7). |
| `COP2-using-the-copper-with-the-blitter.c10` | 35 | Same sentence. It states a possibility, not an outcome. |
| `COP2-using-the-copper-with-the-blitter.c11` | 35 | Same sentence. It states a possibility, not an outcome. |
| `COP2-putting-together-a-copper-instruction-list.c8` | 27 | Locates a point: "Coordinate (0,0) lies at the upper-left corner of the screen." The point cannot be checked: position (0,0) is on line 0, which is vertical blanking (p.219), so nothing is displayed there. P6 grades the orientation of the beam coordinates on the screen, which is not this claim, and carries no claim id. |
| `COP2-putting-together-a-copper-instruction-list.c11` | 27 | Locates a point: "The display endpoint lies at the lower-right corner of the screen." The point cannot be checked: the pages disagree on its coordinates (section 4, item 5). P6 grades only the orientation, which is not this claim (see above). |

The other 25 claims support eleven of the twelve propositions below. P6
carries no claim id.

| # | Proposition (short) | Claims |
|---|---|---|
| P1 | DMACON bit 7 enables Copper DMA | `ch2-stopping-the-copper.c10`, `COP2-stopping-the-copper.c13` |
| P2 | Clearing DMACON bit 7 stops fetching and data placement | `ch2-stopping-the-copper.c7.s2`, `COP2-stopping-the-copper.c10` |
| P3 | DMACON controls every DMA channel | `ch2-stopping-the-copper.c8`, `COP2-stopping-the-copper.c11` |
| P4 | Setting a location register and writing its strobe before enabling gives a known start address | `ch2-starting-the-copper-after-reset.c1.s2`, `ch2-starting-the-copper-after-reset.c2.s2` |
| P5 | With COP1LC unchanged, the Copper restarts at the same place and the screen is stable | `ch2-starting-the-copper-after-reset.c7.s2`, `COP2-starting-the-copper-after-reset.c4` |
| P6 | Beam coordinates run left to right and top to bottom on the screen | none (both p.27 claims declined above) |
| P7 | A WAIT for $0F under a $0F mask is true on every line past 128 | `ch2-a-copper-loop-example.c5.s2` |
| P8 | The 680x0 can poll INTREQ flags that various devices set | `ch2-the-copper-and-the-680x0.c2`, `COP2-the-copper-and-the-680x0.c2`, `COP2-the-copper-and-the-680x0.c3` |
| P9 | A Copper MOVE of a 1 into INTREQ bit 4 interrupts the 680x0 | `ch2-the-copper-and-the-680x0.c1`, `ch2-the-copper-and-the-680x0.c3.s2`, `ch2-the-copper-and-the-680x0.c7`, `COP2-the-copper-and-the-680x0.c9` |
| P10 | INTREQ bit 15 decides whether 1-bits are set or cleared | `ch2-the-copper-and-the-680x0.c5`, `COP2-the-copper-and-the-680x0.c5`, `COP2-the-copper-and-the-680x0.c6`, `COP2-the-copper-and-the-680x0.c7` |
| P11 | Interlace recipe: a handler rewrites COP1LC and the next field runs the new list | `ch2-using-the-copper-in-interlaced-mode.c21`, `ch2-using-the-copper-in-interlaced-mode.c22.s2` |
| P12 | A WAIT with BFD = 0 holds the Copper until the blit has finished | `ch2-using-the-copper-with-the-blitter.c1.s2` |

---

## 2. Common ground

### 2.1 System and program conventions

- Configuration: 68000, 2 MB of Chip memory, ECS selected, PAL. Every
  proposition runs on each of the two CPU cores. No expected value depends
  on the CPU's instruction timing. Where timing matters, the test measures
  it (2.4).
- Register access: 680x0 address = chip address + $DFF000 (p.263). Names
  and chip addresses are those of Appendix B (p.289–291): DMACONR $002,
  VPOSR $004, VHPOSR $006, DSKBYTR $01A, INTENAR $01C, INTREQR $01E, DSKPTH
  $020, DSKLEN $024, COPCON $02E, BLTCON0 $040, BLTCON1 $042, BLTDPTH $054,
  BLTSIZE $058, BLTDMOD $066, COP1LCH $080, COP2LCH $084, COPJMP1 $088,
  COPJMP2 $08A, DIWSTRT $08E, DIWSTOP $090, DDFSTRT $092, DDFSTOP $094,
  DMACON $096, INTENA $09A, INTREQ $09C, AUDxLCH $0A0/$0B0/$0C0/$0D0 (LEN
  +4, PER +6, VOL +8), BPL1PTH $0E0, BPL2PTH $0E4, BPLCON0 $100, BPL1MOD
  $108, BPL2MOD $10A, SPR0PTH $120 … SPR7PTH $13C, COLOR00 $180 (p.29).
- A pointer or location register pair is written as one long word to its
  "H" register (p.25: "you write the 18-bit address by moving one long word
  to the register whose name ends in 'H'"; p.30 example `MOVE.L
  a0,COP1LC(a1)`).
- Strobes (COPJMP1, COPJMP2) are always triggered by a **write**, never a
  read (p.26 "When you write to a Copper strobe address"; p.276 "When written
  to"; p.287 "S Strobe (write address with no register bits). Writing the
  register causes the effect"). See section 4, item 4.
- Unless a proposition says otherwise, the program begins by writing DMACON
  = $07FF (clears bits 10–0), INTENA = $7FFF (clears bits 14–0, including
  the master enable) and INTREQ = $3FFF (clears request bits 13–0). The set/clear
  rule is on p.218. Bits 14 and 13 of DMACON are read-only status bits
  (p.222), and bit 14 of INTREQ requests nothing (p.280), so none of these
  writes selects them.
- Memory layout: program, stack, record buffers, marker cells, each Copper
  list, and each DMA data region occupy separate address ranges in Chip
  memory. All Copper lists and DMA regions lie below $80000, so no
  proposition depends on the extra ECS address bits (p.273, p.309). Once a
  Copper list has been written, the 68000 never reads it and never writes it
  again, except where a proposition says so.
- Overlay (P3 and P5–P12): the area below $80000 is not Chip RAM after
  reset. p.223: "An additional copy of the system ROM responds starting at
  memory location $00000000. The system RAM that would normally be located
  at this address is not available … The OVL bit in one of the 8520 Chips
  disables the overlay". So the common prologue of P3 and P5–P12 begins,
  before anything is written below $80000, with CIA-A DDRA ($BFE201) = $03
  and then CIA-A PRA ($BFE001) with bit 0 (OVL) = 0. p.339: PRA bit 0 is
  OVL, and DDRA is "Direction for port A (BFE001);1-output (set to 0x03)".
  The 680x0 reads its interrupt vectors from the lowest kilobyte of this
  area, so no list or data region uses $000000–$0003FF, and a handler
  (2.6, and the contrasting cases of P8 and P10 that install one) is
  installed only after the overlay is off.
- Programs are assembled for the 68000, vasm's default CPU (`nanomig.mk`
  passes no CPU option). With `-m68010` or higher vasm assembles
  `MOVE #0,<ea>` as `CLR`, which on the 68000 reads its operand before it
  writes it. A strobe written that way would also be read, which the
  strobe rule above and section 4, item 4 keep out of this package.

### 2.2 Copper list encoding

Taken from p.21, p.22, p.31 and the summary on p.36 (Table 2-2):

- MOVE: IR1 = destination chip address (bits 8–1 DA8–DA1, bit 0 = 0, bits
  15–9 = 0); IR2 = the data. Example: `$0180,$0F00` moves $0F00 to COLOR00.
- WAIT: IR1 = VP (bits 15–8), HP (bits 7–1, holding H8–H2), bit 0 = 1. IR2 =
  BFD (bit 15), VE (bits 14–8), HE (bits 7–1), bit 0 = 0. VHPOSR shows the
  horizontal count as H8–H1 in bits 7–0 (p.216). So for horizontal count h,
  the low byte of IR1 is `(h AND $FE) OR 1`, as in p.23 (`$FFFF` = "H = 254")
  and p.33 (`$00E3` = "Wait for Horizontal $E2").
- "WAIT for line v, ignore horizontal" = `v<<8 | $01, $FF00` (p.23 example
  `$9601,$FF00`).
- "WAIT for (v,h), all bits compared" = `v<<8 | (h AND $FE) | 1, $FFFE`
  (p.24: "Bits 14-1 are normally set to all 1s"; BFD = 1).
- End of list = `$FFFF,$FFFE` (p.23, p.29, p.30).
- Every MOVE destination in this package lies at $080 or above. p.26: "The
  registers that the Copper can always affect are numbered $80 through $FF
  inclusive". The one exception is P12, which writes blitter registers with
  CDANG = 1.

### 2.3 Port observations

- **Copper fetch**: a read at the Chip-memory port at an address inside a
  Copper-list region that is neither a 68000 access nor a memory-refresh
  access. The RAM log (`RamAccess` in `nanomig_harness.h`) names no
  requester, so both exclusions rest on something else:
  - 68000 accesses: once a list has been written, the 68000 never reads it
    (2.1), so no read inside a list region is the 68000's.
  - Refresh in P1, P2 and P4: recognised by its address. Every refresh
    access measures as a read of word address 0 and of no other address,
    and no list region of those propositions contains word address 0 (5.1:
    lists at $001000 and above).
  - Refresh in P3 and P5–P12: a refresh access is one made while the
    `refresh` output (port list: "ram refresh cycle") is asserted.

  The instruction word follows from the offset from the list start: IR1 at
  0 mod 4, IR2 at 2 mod 4.
- **MOVE execution** = the fetch of that MOVE's IR2 word. p.20: "Each time
  the Copper fetches an instruction, it fetches both words". p.275: "RD =RAM
  data moved by MOVE instruction at IR2 time directly from RAM to the
  address given by the DA field."
- **Markers**: to time a register write W that the port cannot see, the
  program writes a marker word to a marker cell in Chip memory immediately
  before W (M1) and immediately after W (M2). The port times of these CPU
  writes bound W: t(M1) < t(W) < t(M2).
- **DMA channel attribution** (P3, P8, P12): an access belongs to a channel
  if it falls in that channel's data region and is neither a 68000 access
  nor a refresh access. The slot numbers of Figure 6-9 (p.195) are not used
  to find refresh, because the figure's own footnote says "this timing chart
  has been 'adjusted'". As for Copper fetches, the RAM log names no
  requester. So in P3, P8 and P12 the 68000 is kept out by layout: it
  writes a data region only to prepare it, before the window graded on
  that region opens, and makes no access there inside the window. A
  refresh access is recognised by the `refresh` output, as for Copper
  fetches in P3 and P5–P12.

### 2.4 Reference frame B: beam records (lines and frames)

The program measures the beam itself. In a tight loop it reads VPOSR/VHPOSR
as one long word (p.216: "the address pairs VPOSR,VHPOSR … can be read from
… as long words"), then reads VPOSR again, and stores all of it to a record
buffer. When a proposition says so, it first reads INTREQR and stores that
too. Each record's stores are visible at the port with their times.

- Decoding (p.216): V = VPOSR bit 0 (V8) followed by VHPOSR bits 15–8
  (V7–V0); H = VHPOSR bits 7–0 (H8–H1).
- VPOSR/VHPOSR is two separate bus reads (p.216), so a record whose true V
  crosses a wrap boundary between them can tear: the two words then
  describe two different instants. A record is kept only if the second
  VPOSR read agrees with the first on bit 0 (V8); one where they disagree
  is dropped.
- A record counts as *stored before t* if all its words were written before
  t, and *stored after t* if all its words were written after t. A record is
  read before it is stored, and each record is read after the previous one
  was stored.
- **Line of a port event at time t**: let a be the last record stored before
  t, and b the record that follows the first record stored after t. Record a
  was read before t and record b after t. If V(a) = V(b) = L, the event lies
  on line L. Otherwise it is *undecided* and is neither counted nor counted
  against.
- **Frame (field) boundary**: a wrap is a decrease from V ≥ 256 to V ≤ 1
  (p.24: the vertical position "appears to wrap to 0" past line 255; p.219:
  line 0 is the start of vertical blank). A *complete frame* is the
  interval between two consecutive wraps. An event belongs to frame k if
  the records a and b above both belong to frame k.
- **Middle of line L**: the store time of the first record on line L whose H
  is at least $71 (half of $E2, p.23).
- Anchor checks, asserted whenever a proposition uses this frame:
  - (B1) Within a frame, V never decreases between consecutive records.
  - (B2) Every line in the graded range carries at least 7 records. This is
    a density requirement of the test, not a page value. It makes sure
    events on every graded line can be decided.
  - (B3) No line in the graded range is missing from the frame.

  Contrasting case for B2/B3: a sampler with a delay loop, so that records
  are more than one line apart, fails B2 and B3.
- The Copper compares only V7–V0. On lines 256 and above (PAL), the vertical
  position "appears to wrap to 0" (p.24). Graded ranges in this package lie
  within lines $10–$FF unless a proposition says otherwise.

### 2.5 Reference frame V: video frames and rows

The video output is split into frames at vertical-sync pulses and into rows
at horizontal-sync pulses. The harness samples the video pins once per
clk_sys cycle (`VideoSample` in `nanomig_harness.h`), so a pixel's column
is its count of clk_sys samples from the row's horizontal-sync edge. The
harness also records that the video output passes through a scan doubler:
a row is one output line of the doubler, not one chipset line
(`find_lines`), and one chipset line can appear as more than one row. Rows
and columns are compared only for order (above/below, left/right) and for
identity; no count of rows per chipset line is used. No absolute mapping
from beam counts to rows or columns is derived from the pages. Anchor check
(V1): every compared frame has the same number of clk_sys samples and the
same number of rows. Row lengths are not compared: the harness gives the
doubler's rows only as coming "at roughly twice the rate" of the
chipset's own horizontal sync, and no page describes the doubler.
Contrasting case: write BEAMCON0 ($1DC) with its PAL bit clear a frame
and a half after the enable, so that the compared frames differ in length,
and V1 fails. Setting LACE (BPLCON0 bit 2) is not used as the contrast: p.24
("long and short fields (interlace only)") predicts fields of 313 and 312
lines, but the four compared frames measure equal with LACE set (5.6).

### 2.6 Interrupt-level recorders (P9, P11)

The program installs a handler for each 680x0 interrupt level 1–6. Each
handler writes a record (its level, INTREQR, VPOSR/VHPOSR) to a record
buffer. The level-3 handler also does what the proposition says. The pages
give the level ("Bit 4, COPER, is used by the Copper to issue a level 3
interrupt", p.219; LEVEL column on p.280) but not the vector, so the
handlers are installed through whatever vectors the 680x0 uses for those
levels. Those vectors lie in low memory, which is Chip RAM only once the
overlay is off (2.1, p.223), so the program clears the overlay before it
installs them. The program lowers the 680x0 interrupt mask so that levels
1–6 are accepted.

### 2.7 Starting the Copper (used by every proposition)

This is the sequence p.30 prescribes: write COP1LC (a long word to COP1LCH),
write COPJMP1, then write DMACON with SET/CLR, DMAEN (bit 9) and COPEN (bit
7), plus any other channel bits. For the Copper alone this is $8280; with
bitplanes it is $8380, the p.30 example
`DMAF_SETCLR!DMAF_COPPER!DMAF_RASTER!DMAF_MASTER`. Bit positions are from
p.222 (Table 7-6) and p.278. P4 tests this sequence itself.

### 2.8 Starting the Copper at a chosen list (P3, P5–P12)

P4 grades whether a strobe written before Copper DMA is turned on sets the
Copper's start address. So that no other proposition's stimulus depends on
the clause P4 grades, P3 and P5–P12 start the Copper with the sequence of
2.7 followed by one more strobe, written while Copper DMA is on:

1. COP1LC = the list (a long word to COP1LCH), then COPJMP1 (p.30).
2. DMACON with SET/CLR, DMAEN, COPEN and the proposition's other channel
   bits (2.7).
3. COPJMP1 again, followed immediately by a marker MJ.

p.25: "When a jump address strobe is written, the corresponding location
register is loaded into the Copper program counter." p.26: "When you write
to a Copper strobe address, the Copper reloads its program counter from the
corresponding location register." p.276: "When written to, they cause the
Copper to jump indirect using the address contained in the first or second
location registers". None of these makes the reload depend on DMACON, and
the p.33 loop writes COPJMP1 from a running list. From the step-3 strobe
on, the Copper runs the list from its IR1, whatever the step-1 strobe did.
Where the Copper fetches between the step-2 DMACON write and t(MJ) is not
graded; P3 counts such fetches only as accesses to region COP. Where a
list's first instruction is not a WAIT that holds the Copper until
after t(MJ), the list can run twice from its start in the frame of the
start; where that would disturb a check, the proposition says when the
sequence is written.

A *restart* is the first Copper fetch after a fetch of the IR2 of a list's
final WAIT (`$FFFF,$FFFE`). That WAIT "never finishes" (p.30), and while
waiting the Copper "does not use the memory bus at all" (p.19), so the next
Copper fetch is the one the restart from COP1LC in vertical blanking causes
(p.25, p.30, p.32). This locates the restart in the fetch sequence without
placing it against a frame boundary. At the port the restart is identified
inside the list's own region (2.3): the next channel read in that region
after the final WAIT's IR2 must be a read of the list's first word (IR1);
a pair whose next read in the region is any other word is not a restart.

---

## 3. Propositions

### P1: DMACON bit 7 (COPEN) enables Copper DMA

1. **Proposition.** With DMAEN set, writing DMACON with SET/CLR and bit 7
   set makes the Copper fetch its list. While bit 7 is 0 the Copper fetches
   nothing. Claims `ch2-stopping-the-copper.c10`,
   `COP2-stopping-the-copper.c13`; p.30.
2. **Stimulus.**
   1. Common prologue (2.1). The Copper is not yet started.
   2. List L1 at address A: `$0180,$0F00` (MOVE COLOR00) and `$FFFF,$FFFE`.
   3. Write COP1LC = A, then write COPJMP1 (p.30).
   4. Run the sampler until a record shows V between $40 and $C0.
   5. M1; DMACON = $8280; M2.
   6. Read DMACONR and store it. Run the sampler for two more complete
      frames.
3. **Observation.** Copper fetches in L1's region, in two windows:
   - W0: from the start of the program to t(M1).
   - W1: from t(M2) to the end of the second complete frame after M2
     (frames per 2.4).
4. **Expected.** In W1 there is at least one fetch of L1's IR1 in each of
   the two complete frames. p.30: "Bit 7, COPEN, enables Copper DMA when set
   to 1." The case with bit 7 at 0 rests on p.222: "The contents of this
   register are shown in Table 7-6 (bit on if enabled)", and is graded by
   the contrasting case. The two cases are graded together as one
   assertion, which carries both claims; each case is also a line of its own
   and carries no claim id.
5. **Preconditions.**
   - (a) No Copper fetch in W0.
   - (b) The stored DMACONR has bit 9 = 1 and bit 7 = 1 (p.222: "There is a
     read address as well as a write address to the DMA control register so
     you can tell which DMA channels are enabled").
   - (c) Anchor checks B1–B3 over the two frames.

   The same three preconditions guard the contrasting main case. For it,
   (b) requires bit 9 = 1 (DMAEN set, so the absence of fetches is the
   COPEN bit's) and leaves bit 7 = 0 to the assertion.
6. **Contrasting cases.**
   - Main: DMACON = $8200 (SET/CLR and DMAEN only). Expected: no Copper
     fetch in L1's region from t(M2) on (which includes W1), and DMACONR
     bit 7 = 0.
   - For (a): write DMACON = $8280 at the start of the program, before step
     3. Fetches then appear in W0.
   - For (b): DMACON = $8080 (COPEN without DMAEN). DMACONR bit 9 reads 0,
     and the pages also predict no Copper fetch (p.222 "DMAEN … enables the
     DMA for all of the channels at bits 8-0"; p.144 "For any audio DMA to
     occur (or any other DMA, for that matter), the DMAEN bit in DMACON must
     be set").
   - For (c): the delayed sampler of 2.4.
7. **Reference frames.** 2.3 (Copper fetch), 2.4 (frames).

### P2: Clearing COPEN stops instruction fetching and data placement

1. **Proposition.** After the 68000 clears DMACON bit 7, the Copper neither
   retrieves instructions nor places data. Claims
   `ch2-stopping-the-copper.c7.s2`, `COP2-stopping-the-copper.c10`; p.30.
2. **Stimulus.**
   1. Common prologue.
   2. List L2 at A: `$009C,$8010` (MOVE INTREQ, setting bit 4) and
      `$FFFF,$FFFE`. It places data once per frame, right after the Copper
      restarts.
   3. Start the Copper per 2.7 (DMACON $8280).
   4. Phase 1: when a record shows V between $40 and $C0, write INTREQ =
      $0010 (clear bit 4, p.218), with a marker MPC immediately after. Then
      run a polling sampler (record = INTREQR, VPOSR/VHPOSR) until one
      complete frame has passed.
   5. Phase 2: when a record shows V between $80 and $C0, do M1; DMACON =
      $0080 (SET/CLR = 0, bit 7 selected: clear); M2. Then INTREQ = $0010,
      then the polling sampler for two complete frames.
3. **Observation.**
   - (i) Copper fetches in L2's region after t(M2), up to the end of the
     second complete frame after M2.
   - (ii) INTREQR bit 4 in every phase-2 record stored after the clear.
4. **Expected.** (i) zero Copper fetches; (ii) bit 4 = 0 in every such
   record. p.30: "You can also stop the Copper by disabling its ability to
   use DMA for retrieving instructions or placing data." "Retrieving
   instructions" is graded by (i) and "placing data" by (ii). The window
   covers two vertical blanks because the page gives the stop no time limit.
5. **Preconditions.**
   - (a) After t(MPC), the marker following the phase-1 clear: at least one
     record is stored before the first IR2 fetch of L2's MOVE that follows,
     bit 4 is 0 in every record stored between t(MPC) and that fetch, and 1
     in a record stored after it.
   - (b) INTENAR bits 14–0 read 0 (read and stored before phase 2), and the
     program writes INTREQ nowhere in phase 2 except the one clear. So
     nothing but the Copper could set bit 4 (p.219: bit 4 "has been reserved
     for specifically identifying the Copper as the interrupt source").
   - (c) B1–B3.
6. **Contrasting cases.**
   - Main: in phase 2 write DMACON = $0000 instead of $0080. p.218:
     "Positions 14-0 that have 0 value will not be affected when you do the
     write". Expected: fetches continue in each frame, and bit 4 is 1 again
     after the next restart. This case clears no bit, so its line is a
     control and carries no claim id.
   - For (a): L2 with the MOVE replaced by `$0180,$0F00`. Bit 4 is never
     set in phase 1.
   - For (b): INTENA = $8010 (bit 4 set, the master enable bit 14 left at
     0). p.218: "If this bit is a 0, it disables all other interrupts."
     INTENAR is then non-zero, with no live interrupt to interfere with the
     rest of the run.
   - For (c): as in 2.4.
7. **Reference frames.** 2.3, 2.4.

### P3: DMACON controls every DMA channel

1. **Proposition.** Each DMA channel that DMACON names makes memory accesses
   only while both its own enable bit and DMAEN (bit 9) are 1. The channels
   are bitplane BPLEN (bit 8), Copper COPEN (7), blitter BLTEN (6), sprite
   SPREN (5), disk DSKEN (4) and audio AUD3EN–AUD0EN (3–0). Clearing one
   channel's bit silences that channel and no other. Clearing DMAEN silences
   all of them. Claims `ch2-stopping-the-copper.c8`,
   `COP2-stopping-the-copper.c11`; p.30, with the bit assignment from p.222
   (Table 7-6) and p.278.

   The quantifier "all" is graded over every channel p.222 lists, each as a
   case of its own. p.30's sentence ("The register called DMACON controls
   all of the DMA channels") stands in the paragraph on disabling Copper
   DMA through DMACON, and speaks of the channels DMACON enables. Memory
   refresh is also DMA (p.20, p.194), but no page gives it a DMACON bit, so
   it is not one of those channels and is not graded (section 4, item 11).
   Both claims are graded whole by one assertion that conjoins Base's
   coverage, the nine channel cases and case M. Those lines are components
   and carry no claim id.
2. **Stimulus.** Nine disjoint data regions of 32 KB ($8000 bytes) each,
   in the order BPL, SPR, AUD0, AUD1, AUD2, AUD3, DSK, BLT, COP: region k
   (k = 0 … 8) starts at $10000 + k × $8000, so together they fill
   $10000–$57FFF. That keeps them below $80000 (2.1), clear of the vector
   table (2.1) and clear of the program, stack, results and record buffers
   at $080000 and above (`nanomig.i`: `PROGRAM_ORG` $080400, `RESULTS`
   $0C0000). The largest use is BPL: with its pointer written once, one
   plane of 40 bytes × 256 lines (p.59, p.60, p.64) is fetched in each of
   the two frames the window spans, 20,480 bytes in all. Every other
   channel uses less
   (audio 16 words, disk 100 words, blitter 64 × 32 words, Copper one
   instruction). Each channel's own pointer or location register is written
   $100 (256 bytes) above its own region's start, not at the region's first
   word: this leaves a band below the pointer, still inside the region,
   where precondition (c) can catch another channel's accesses landing
   there. Each case below is its own run.
   1. Common prologue, with the overlay cleared (2.1).
   2. Bitplane registers (p.59, p.60, p.63, p.64): BPLCON0 = $1200 (one
      plane, bits 14–12 = 1, colour bit 9), DIWSTRT = $2C81, DIWSTOP = $2CC1
      ("The normal PAL DIWSTOP is ($2CC1)", p.59), DDFSTRT = $0038, DDFSTOP
      = $00D0, BPL1MOD = 0.
   3. Audio x = 0–3: AUDxLC = region AUDx's start + $100 (p.139), AUDxLEN =
      16 (p.139), AUDxPER = 447 (p.143), AUDxVOL = 48 (p.140).
   4. Copper: list `$FFFF,$FFFE` at region COP's start + $100. Write COP1LC
      (that same address) and COPJMP1 (p.30).
   5. Prepare the sprite data structures in SPR, $100 above region SPR's own
      start: sprite 0 as chapter 4 describes (p.110: control words, data
      words, end-of-data words), and sprites 1–7 as end-of-data words only.
   6. Wait for vertical blank: INTREQ = $0020, then poll INTREQR until bit 5
      = 1 (p.219).
   7. In that vertical blank, write BPL1PT = region BPL's start + $100 and
      SPR0PT–SPR7PT = their structures, $100 above region SPR's own start
      (p.110: "During the vertical blanking interval before the first
      display of the sprite, you need to write the sprite's memory address
      into these pointers").
   8. M1; DMACON = $8000 | P (the case's pattern, below); M2; then
      COPJMP1 again and MJ (2.8, step 3), so that the Copper's start in
      the runs with bits 9 and 7 set does not depend on the step-4
      strobe.
   9. Disk, in p.247's order (DMACON has just been written):
      - DSKPTH = region DSK's start + $100, as a long word (p.246).
      - DSKLEN = $4000.
      - DSKLEN = $C064 (bit 15 DMAEN, bit 14 WRITE "RAM -> disk", 100
        words).
      - DSKLEN = $C064 again ("This actually starts the DMA").
      - Read DSKBYTR and store it.
   10. Blitter: BLTCON0 = $01FF (USED, all minterms 1: p.267), BLTCON1 = 0,
       BLTDMOD = 0, BLTDPT = region BLT's start + $100, BLTSIZE = $1020
       last (p.200: "Write BLTSIZE last; writing this register starts the
       blit").
   11. Read DMACONR and store it. Run the sampler until the end of the first
       complete frame after M2.

   Cases (P gives bits 9–0):

   | Case | P |
   |---|---|
   | Base | $03FF |
   | Case X, one for each of the nine channel bits | $03FF with bit X cleared |
   | Case M | $01FF (DMAEN clear) |

3. **Observation.** Accesses attributed to each region (2.3) from t(M2) to
   the end of the first complete frame after M2. Also DSKBYTR bit 14 (DMAON).
4. **Expected.**
   - Base: every region other than DSK has at least one access (DSK is
     precondition (a)).
   - Case X: region X has none, and every other region that has an access
     in Base has at least one.
   - Case M: every region has none.
   - DSKBYTR bit 14 = 1 in Base and 0 in the DSK case and in case M.

   Sentences:
   - p.30: "The register called DMACON controls all of the DMA channels."
   - p.222: "DMAEN … This is a master DMA enable bit. It enables the DMA for
     all of the channels at bits 8-0."
   - p.144: "For any audio DMA to occur (or any other DMA, for that matter),
     the DMAEN bit in DMACON must be set."
   - p.248 (DMAON): "Indicates when DMA is actually enabled. All the various
     DMA bits must be true. This means the DMAEN bit in DSKLEN, and the DSKEN
     & DMAEN bits in DMACON."
5. **Preconditions.**
   - (a) Region DSK has at least one access in Base. The pages do not say
     whether disk DMA runs with no drive attached (section 4, item 12), so
     the Base run must show it. Without it, the DSK case is inconclusive,
     not green, and region DSK is left out of the other cases' "every other
     region" check; DSKBYTR bit 14 is graded either way.
   - (b) In every run, the stored DMACONR bits 9–0 equal P (p.222).
   - (c) In Base, the lowest address accessed in each region is the
     address the program wrote into that region's own channel (its pointer
     or location register; for SPR, the lowest of SPR0PT–SPR7PT). So no
     other channel's accesses land in the region below it. The check uses
     addresses, not the order of accesses in time, which the pages do not
     fix between channels.
   - (d) B1 and B3 for the frame boundary.
6. **Contrasting cases.**
   - Main: the case runs contrast with Base. Each differs only in one bit,
     and the page predicts that region falls silent.
   - For (a): in Base, leave out the second DSKLEN = $C064 write (p.247:
     "Write this value again into the DSKLEN register. This actually starts
     the DMA"). Region DSK is then silent in Base.
   - For (b): write DMACON = $83FE (SET/CLR set, every channel and DMAEN on,
     bit 0 AUD0EN left clear) graded against P = $3FF. DMACONR bits 9–0 then
     read $3FE, not $3FF, and every channel but AUD0 keeps running.
   - For (c): in Base, point SPR1PT at the start of region AUD0, and AUD0LC
     256 bytes above it. The lowest address accessed in AUD0 is then
     SPR1PT's, and the check fails whichever of the two channels is fetched
     first.
7. **Reference frames.** Data regions (2.3), frames (2.4).

### P4: Loading a location register and writing its strobe before enabling gives a known start address

1. **Proposition.** After reset, if the program loads a location register
   (COP1LC or COP2LC) and writes that register's strobe before Copper DMA is
   turned on, the Copper's first fetch after DMA is turned on is at that
   location register's address. Claims
   `ch2-starting-the-copper-after-reset.c1.s2`,
   `ch2-starting-the-copper-after-reset.c2.s2`; p.30.

   Two parts of the passage give no expected value. The "known state"
   (p.30; p.276 says "determined startup address and state") is not defined
   in observable terms, so it is not graded. The page does not say what
   happens if the order is broken, so no case tests a broken order.
2. **Stimulus.** Lists A, B and C sit at different addresses. Each is
   `$0180,x` followed by `$FFFF,$FFFE`, with a different x in each list. The
   program's very first action is to read DMACONR and store it. The common
   DMACON clear is skipped in this proposition; INTENA and INTREQ are still
   cleared (2.1).

   The location registers and the strobe are written only after a record
   shows V between $40 and $C0 (p.219: vertical blanking ends at line 25
   for PAL), so that no vertical blank lies between them and Copper DMA
   being turned on. A marker MS precedes the first location-register
   write.

   | Run | Writes after MS, before M1 |
   |---|---|
   | Run 1 (COP1) | COP2LC = B, COP1LC = A, COPJMP1 |
   | Run 2 (COP2) | COP1LC = A, COP2LC = B, COPJMP2 |
   | Run 3 (strobe, then rewrite) | COP1LC = A, COP2LC = B, COPJMP2, COP2LC = C (no strobe) |

   In every run: M1; DMACON = $8280; M2; sampler until the end of the next
   complete frame.
3. **Observation.** The first Copper fetch in the A, B or C regions after
   t(M1): its exact word address, which is compared with the first word of
   the selected list (IR1), and the frame it lies in.
4. **Expected.**

   | Run | First fetch after M1 |
   |---|---|
   | Run 1 | A's IR1 |
   | Run 2 | B's IR1 |
   | Run 3 | B's IR1, not C's |

   In each run the first fetch lies before the first frame wrap after M1.

   Sentences:
   - p.30: "you must initialize one of the Copper location registers (COP1LC
     or COP2LC) and write to its strobe address before Copper DMA is turned
     on. This ensures a known start address and known state."
   - p.25: "When a jump address strobe is written, the corresponding
     location register is loaded into the Copper program counter."
   - p.26: "COPJMP2 Restart Copper from address contained in COP2LC."
   - p.276: "They are transferred to the program counter whenever strobe
     addresses COPJMP1 or COPJMP2 are written."

   Runs 1 and 2 grade "one of … COP1LC or COP2LC": both registers are
   cases, graded together as one assertion. Run 3 and its reorder (6) carry
   no claim id of their own and are graded by the same rule, without one.
5. **Preconditions.**
   - (a) The first stored DMACONR has bit 7 = 0: Copper DMA was off when
     the program started ("At power-on or reset time").
   - (b) No Copper fetch in any list region before t(M1).
   - (c) No frame wrap and no record with V ≤ 25 lies between t(MS) and the
     first fetch. Otherwise a vertical blank (p.219: blanking is lines 0–25
     for PAL) lies in that stretch, and the restart from COP1LC (p.25)
     could have supplied the address instead of the strobe under test.

   Each of (a)-(c) holds in every one of the six captures (Runs 1, 2 and 3,
   Run 3-reorder, and the two comparator contrasts) and is printed before
   any line that grades that capture.
6. **Contrasting cases.**
   - Main: Run 1 and Run 2 differ only in which strobe is written, and the
     page predicts A and B respectively, graded together. For Run 3, move
     the COP2LC = C write to before COPJMP2 (Run 3-reorder). The first
     fetch is then C's IR1 (p.25); Run 2, Run 3 and Run 3-reorder are
     graded together against this rule.
   - Comparator contrast: COP1LC = B (not A), then COPJMP1, with COP2LC = A
     as an unstrobed decoy (mirroring Run 1's own shape). The page predicts
     B. This isolates which list ends up at the strobed register from
     everything else Run 1 does, and shows the comparator is not vacuous.
   - For (a): the program first writes DMACON = $8080 (COPEN without
     DMAEN, p.222) and only then reads DMACONR. Bit 7 reads 1, with DMAEN
     left at 0 so Copper DMA does not actually start and the rest of the
     run is unaffected.
   - For (b): run the whole sequence, DMACON write included, once before M1.
     Fetches then appear before t(M1).
   - For (c): write M1, wait for a frame wrap, then write DMACON. A wrap then
     lies between t(M1) and the first fetch.
7. **Reference frames.** 2.3, 2.4.

### P5: With COP1LC unchanged, the Copper restarts at the same place every frame and the screen is stable

1. **Proposition.** When COP1LC is not changed, the Copper restarts at the
   same location for every later frame. With a correctly formulated list,
   consecutive frames of video output are identical. Claims
   `ch2-starting-the-copper-after-reset.c7.s2`,
   `COP2-starting-the-copper-after-reset.c4`; p.30. The page's "correctly
   formulated" list is its own complete sample list (p.28–29). That list
   reloads the bitplane pointers every frame, which p.27 and p.63 require.
2. **Stimulus.**
   1. Common prologue.
   2. Display registers: DIWSTRT = $2C81, DIWSTOP = $2CC1, DDFSTRT = $0038,
      DDFSTOP = $00D0, BPL1MOD = BPL2MOD = 0 (p.59, p.60, p.64).
   3. Bitplane data at P1 and P2, 40 bytes × 256 lines each. Plane 1 has
      words alternating $FFFF/$0000 along each line. Plane 2 has lines
      alternating all-$FFFF/all-$0000. All four colours then appear above
      and below line 150.
   4. List S is the p.29 list word for word, with $21000/$25000 replaced by
      P1/P2 (note 2 on p.29: "Bitplane addresses used in the example are
      arbitrary"):
      - BPL1PTH/L, BPL2PTH/L.
      - COLOR00–03 = $0FFF, $0F00, $00F0, $000F.
      - BPLCON0 = $2200.
      - `$9601,$FF00` (WAIT for line 150).
      - COLOR00–03 = $0000, $0FF0, $00FF, $0F0F.
      - `$FFFF,$FFFE`.
   5. Start per 2.8 with DMACON = $8380.
   6. Run the sampler for six complete frames. COP1LC is never written
      again.
3. **Observation.**
   - (i) Video frames 2, 3, 4 and 5 after the enable (2.5). Frame 1 is not
     graded.
   - (ii) At the port, every restart (2.8) after t(MJ).
4. **Expected.**
   - (i) Video frames 2–5 are identical at every clk_sys sample (r, g, b,
     hs and vs; 2.5).
   - (ii) Each of the six complete frames contains a restart (2.8), frame
     membership taken on the frame time ranges of 5.2 (the rule P1's
     per-frame fetch check uses), and every restart is a fetch of S's IR1,
     at S's address. While the Copper waits on the final WAIT it makes no
     fetches (p.19: "it does not use the memory bus at all"), so each
     restart is the Copper's first fetch of a new frame.

   Sentence, p.30: "Now, if the contents of COP1LC are not changed, every
   time vertical blanking occurs the Copper will restart at the same
   location for each subsequent video screen. This forms a repeatable loop
   which, if the list is correctly formulated, will cause the displayed
   screen to be stable."

   The quantifier "each subsequent" is graded over four consecutive frames.
   No finite run can show more.

   Where in vertical blanking the restart happens is not graded, because
   the pages disagree (section 4, item 2).
5. **Preconditions.**
   - (a) The Copper runs S: after t(MJ), S's IR1 is fetched and that run
     reaches the fetch of S's final WAIT. This rests on the strobe of 2.8,
     not on the restart that (ii) grades, so a missing restart shows as a
     red (ii), not as an ungraded one.
   - (b) In each graded video frame, both colour sets appear, with the rows
     showing the second set below the rows showing the first. The frames
     must not match merely because the Copper stopped or the display is
     blank.
   - (c) In each complete frame, the first bitplane fetches are at P1 and P2
     (the pointer reload, p.27).
   - (d) V1 (2.5), and B1 and B3 over the six frames.

   (b) and (c) guard (i) only; (ii) is graded whenever (a) and (d) hold.
6. **Contrasting cases.**
   - Main: once per frame, when a record shows V = $A0, the 68000 writes
     COP1LC, alternating between S and S'. S' is S with its first COLOR00
     value changed to $0F0F. Expected: each restart is at the list written
     in the frame before (p.32: "The value in the COP1LC location registers
     will be restored to the Copper's program counter at the start of the
     vertical blanking period"), so every second restart is at S', not S,
     and (ii) fails. The video frames alternate between two different images,
     so consecutive frames are not identical.
   - Also for (ii): once, when a record in the third complete frame shows
     V = $A0, write DMACON = $0080 (COPEN clear, p.30: "You can also stop
     the Copper by disabling its ability to use DMA"). The later frames
     then contain no restart, and (ii) fails.
   - For (a): start with DMACON = $8200 (COPEN clear). S is never fetched.
   - For (b): remove the four MOVEs after the line-150 WAIT. The lower rows
     then show the upper colours, and (b) fails.
   - For (c): remove the four bitplane-pointer MOVEs. The frames after the
     first no longer fetch from P1/P2 (p.63: "the pointers have been
     incremented … and must be repointed").
   - For (d): as in 2.4 for B1 and B3, and as in 2.5 for V1.
7. **Reference frames.** 2.4 (port frames), 2.5 (video frames), 2.8
   (restarts). The page's line 150 is not mapped to a video row; only
   "below" is graded.

### P6: Beam coordinates run left to right and top to bottom on the screen

1. **Proposition.** Beam positions (x,y) run across the screen from the
   upper left towards the lower right. A Copper change at a larger x on the
   same line appears further right, and one at a larger y appears lower.
   p.27. No claim id: this is a graded orientation check only.

   The corner points themselves cannot be checked. Position (0,0) is on line
   0, which is vertical blanking (p.219). The end-point coordinates disagree
   between pages (section 4, item 5). The two p.27 claims locate those
   points, so both are declined whole (section 1). What the test can see is
   the orientation, and that is what P6 grades.
2. **Stimulus.**
   1. Common prologue.
   2. BPLCON0 = $0200 (no bitplanes, colour on). The whole displayed area
      then shows COLOR00 (p.23: "only the background color is displayed").
   3. List O:
      - `$0180,C0`
      - `$6061,$FFFE`, `$0180,C1`
      - `$60A1,$FFFE`, `$0180,C2`
      - `$A061,$FFFE`, `$0180,C3`
      - `$FFFF,$FFFE`

      C0–C3 = $0F00, $00F0, $000F, $0FF0. Horizontal counts $60 and $A0 lie
      outside horizontal blanking ("$0F to $35", p.23). Lines $60 and $A0
      lie after vertical blanking ("ends at … line 25 for PAL", p.219).
   4. Start per 2.8 (DMACON $8280).
   5. Run the sampler of 2.4 until four frame wraps have passed after
      t(MJ), then end. The records only time the run; none is graded, so
      B1–B3 do not apply.
3. **Observation.** The second complete video frame after the enable. Let
   r1 be the first row that contains C1 and r3 the first row that contains
   C3. On row r1, find the runs of C0, C1 and C2.
4. **Expected.** On row r1, read from left to right, a C0 run comes first,
   then the C1 run, then a C2 run. Row r3 is below row r1 (r3 > r1).
   Sentence, p.27: "The video beam traverses the screen from position (0,0)
   in the upper left hand corner of the screen to the end of the display
   (226,262) NTSC (or (226,312) PAL) in the lower right hand corner. The
   first 0 in (0,0) represents the x position. The second 0 represents the y
   position."
5. **Preconditions.**
   - (a) All four colours appear in the frame.
   - (b) At the port, at least three restarts (2.8) follow t(MJ), and
     between each two consecutive restarts each of the four MOVEs is
     executed exactly once, in list order. Restarts are used because the
     pages give no mapping from a video frame to port time.
   - (c) V1.
6. **Contrasting cases.**
   - (i) Change the second WAIT from `$60A1` to `$60C1`. Expected: the right
     end of the C1 run on row r1 moves to a larger column.
   - (ii) Change the third WAIT from `$A061` to `$B061`. Expected: r3
     increases.
   - For (a): place the C3 MOVE after the final WAIT. C3 then never appears.
   - For (b): DMACON = $8200. The Copper makes no fetch, so there is no
     restart and no MOVE is executed.
   - For (c): as in 2.5.
7. **Reference frames.** 2.5, used for order only; 2.3 and 2.8 for (b).

### P7: A WAIT for $0F under a $0F vertical mask is true on every line past 128

1. **Proposition.** A Copper loop does three things: it waits for vertical
   position $0F with only the low four vertical compare-enable bits set and
   the horizontal position ignored; it then writes the Copper interrupt bit;
   and it then loops. Once the vertical position is past 128, this loop
   issues the interrupt on every scan line, because the most significant
   vertical bit cannot be masked. Claim `ch2-a-copper-loop-example.c5.s2`;
   p.32.
2. **Stimulus.**
   1. Common prologue (all interrupts disabled).
   2. List Q at A is the page's COPPERL1 without its SKIP (p.33):
      - `$0F01,$8F00`
      - `$009C,$8010`
      - `$00E3,$80FE`
      - `$0088,$0000` (MOVE COPJMP1)
   3. Write INTREQ = $0010 and read INTREQR (store it).
   4. Start per 2.8 (DMACON $8280).
   5. Run the sampler (VPOSR/VHPOSR only) for the frame in progress and two
      complete frames.
   6. Read INTREQR and store it.
3. **Observation.** Executions of the MOVE to INTREQ (IR2 fetch at A+6) in
   the second complete frame, placed against the middles of lines (2.4).
4. **Expected.** Every window from the middle of line L−1 to the middle of
   line L, for L = $81 to $FF, contains at least one execution. Sentence,
   p.32: "When the Copper is waiting for $0F and the vertical position is
   past 128 (hex $80), this test will always be true. In this case, the
   minimum value in the comparison will be $80, which is always greater
   than $0F, and the interrupt will happen on every scan line."

   The windows are built like those of precondition (a). The loop's $E2
   WAIT holds the Copper to the end of a line (p.32: "This will
   force the Copper to the next line before the next WAIT"), so the jump
   and the next MOVE can fall at the very start of the following line.
   There the last record stored before the MOVE can still show the
   previous line, and the line-of-event rule of 2.4 would leave the
   execution undecided. The window ending at the middle of line L holds
   the execution at the start of line L either way.

   The quantifier "every" is graded over all 127 lines, not a sample. Three
   things are left ungraded:
   - Line $80: p.32 says "past 128" while p.33 says "fail after vertical
     position 127" (section 4, item 6).
   - Lines from 256 on: there the comparison sees V7–V0 = 0 to 55 (p.24).
   - The number of executions per line: the page says "on every scan line",
     not "once".
5. **Preconditions.**
   - (a) Below 128 the mask works. Each window from the middle of line $xE
     to the middle of line $xF, for x = 1 to 7, contains exactly one
     execution, and no execution between the middle of line $10 and the
     middle of line $7F lies outside those windows. The page says the mask
     "should compare 'true' for $1F, $2F, $3F, etc." (p.32). The loop's
     $E2 WAIT exists so the Copper does not "cause another interrupt on the
     same scan line" (p.32).
   - (b) The MOVE really sets bit 4: the INTREQR read in step 3 shows bit 4
     = 0, and the read in step 6 shows 1.
   - (c) B1–B3 for lines $10–$FF.
6. **Contrasting cases.**
   - Main: the page's second loop, WAIT `$8F01,$8F00` and `$80E3,$80FE`
     with the rest unchanged. p.33: "it waits for $xF with the high bit set
     (binary 1xxx1111)". Expected: between the middle of line $80 and the
     middle of line $FF there is exactly one execution in each window from
     mid-line $xE to mid-line $xF, for x = 8 to F, and none elsewhere. So
     windows such as the one ending at the middle of line $81 are empty,
     and the main check goes red.
   - For (a): first WAIT's IR2 = $8000 (no vertical enable bits). p.32: "If
     the corresponding enable bit is 0, the comparison is always true".
     Executions then occur on every line below 128 as well.
   - For (b): MOVE data $0010. Bit 4 is then never set (P10).
   - For (c): as in 2.4.
7. **Reference frames.** 2.4 (frames, middle of line), list offsets (2.3).

### P8: The 680x0 can poll INTREQ flags that various devices set

1. **Proposition.** With every interrupt disabled, a 68000 reading INTREQR
   sees a request flag change from 0 to 1 when its device sets it, and sees
   it stay 1 until the program clears it. This is shown for three devices:
   the Copper (bit 4), the start of vertical blank (bit 5) and the blitter
   (bit 6). Claims `ch2-the-copper-and-the-680x0.c2`,
   `COP2-the-copper-and-the-680x0.c2`, `COP2-the-copper-and-the-680x0.c3`;
   p.35.

   The quantifier "various devices" is graded as three distinct sources, the
   three the chip set sets on its own (p.219, p.220). No page lists which
   devices the word covers.
2. **Stimulus.** Common prologue (INTENA cleared: polling only). The polling
   sampler's record is INTREQR followed by VPOSR/VHPOSR. Each case below is
   its own run.
   In each case the program's clear of the case's bit is followed
   immediately by a marker MC (2.3), so the records stored after the clear
   can be told from those stored before it.
   - **COP**:
     1. List `$8001,$FF00`, `$009C,$8010`, `$FFFF,$FFFE`.
     2. Start per 2.8.
     3. When a record shows V between $40 and $60, write INTREQ = $0010;
        MC.
     4. Poll until the end of the frame.
   - **VB**:
     1. Once a record has shown a frame wrap (2.4) after the prologue, and
        then a record shows V ≥ $F0, write INTREQ = $0020; MC.
     2. Poll through the next frame wrap until V = $20.
   - **BLIT**:
     1. DMACON = $8240; INTREQ = $0040; MC.
     2. D-only blit: BLTCON0 = $01FF, BLTCON1 = 0, BLTDMOD = 0, BLTDPT =
        region D, BLTSIZE = $1020 last.
     3. Poll for one complete frame.
3. **Observation.** The case's bit in each poll record stored after t(MC),
   and the time of the device's event:
   - COP: the MOVE's IR2 fetch.
   - VB: the first frame wrap stored after t(MC).
   - BLIT: the last write in region D.
4. **Expected.**
   - (i) Every record stored after t(MC) and before the event reads 0.
   - (ii) A later record reads 1.
   - (iii) After the first record that reads 1, every record in the window
     reads 1.
   - For VB, the change from 0 to 1 falls between a record on the last line
     of the old frame or on line 0 and a record on line 0 or line 1.

   Sentences:
   - p.35: "The 680x0 can poll for interrupt flags set in the INTREQ
     register by various devices."
   - p.280: "These bits may be polled by the processor … These status bits
     are not automatically reset when the interrupt is serviced, and must be
     reset when desired by writing to this address."
   - p.219: "Bit 5, VERTB, causes an interrupt at line 0 (start of vertical
     blank)".
   - p.220: "Bit 6, BLIT, signals 'blitter finished.' If this bit is a 1, it
     indicates that the blitter has completed the requested data transfer."
   - p.187: "The blitter also has an interrupt flag that is set whenever a
     blit finishes."
5. **Preconditions.**
   - (a) INTENAR bits 14–0 read 0 (read and stored): the flags are polled,
     not serviced. This is read and graded in each of the three cases.
   - (b) The first record stored after t(MC) reads 0, so a later 1 is new.
   - (c) The event happens inside the window:
     - COP: the IR2 fetch is present.
     - BLIT: 2048 writes of $FFFF appear in D.
     - VB: a frame wrap stored after t(MC) is present, and the change from 0
       to 1 falls between a record on the last line of the old frame or on
       line 0 and a record on line 0 or line 1.

   Every one of these is printed before the assertion that depends on it.
   The three devices are graded by one assertion that carries the three
   claims; each device is also a line of its own and carries no claim id.
6. **Contrasting cases.**
   - COP: replace the MOVE with `$0180,$0F00`. Bit 4 stays 0.
   - BLIT: omit the BLTSIZE write. Bit 6 stays 0.
   - VB: poll only from V = $40 to V = $F0 of one frame. Bit 5 stays 0
     (p.219 places the event at line 0). This removes the event as well,
     so VB's (c) fails with it: no page gives an input that keeps line 0
     in the window and VERTB clear. The contrast shows only that (ii)'s
     comparator can fail.
   - For (iii), COP case: once a poll record stored after t(MC) reads 1,
     the program writes INTREQ = $0010 again. p.280: the status
     bits "must be reset when desired by writing to this address". A later
     record then reads 0, and (iii) fails.
   - For (a): INTENA = $C020 with a level-3 handler that clears bit 5.
     INTENAR is then non-zero.
   - For (b): skip the clear in the VB case. Line 0 has passed since the
     prologue's clear (VB step 1), so bit 5 reads 1 at once.
   - For (c), COP case: DMACON = $8200. No IR2 fetch occurs.
7. **Reference frames.** 2.4 (record positions and frame wraps), 2.3
   (markers, data region).

### P9: A Copper MOVE that stores a 1 into INTREQ bit 4 interrupts the 680x0

1. **Proposition.** A Copper MOVE that writes $8010 to INTREQ sets bit 4.
   With INTENA's master enable (bit 14) and bit 4 set, the 68000 then takes
   a level-3 interrupt. Its handler, running 68000 code, finds bit 4 set in
   INTREQR. Claims `ch2-the-copper-and-the-680x0.c1`,
   `ch2-the-copper-and-the-680x0.c3.s2`, `ch2-the-copper-and-the-680x0.c7`,
   `COP2-the-copper-and-the-680x0.c9`; p.35, with the level from p.219 and
   p.280.
2. **Stimulus.**
   1. Common prologue.
   2. Install the recorders of 2.6. The level-3 handler reads INTREQR,
      stores its record, writes INTREQ = $0010, and returns.
   3. Write INTREQ = $0010, then read INTREQR and store it.
   4. Write INTENA = $C010; read INTENAR and store it.
   5. List I: `$8001,$FF00`, `$009C,$8010`, `$FFFF,$FFFE`.
   6. Start per 2.8 when a record shows V between $20 and $60: vertical
      blanking has ended (p.219) and I's WAIT for line $80 holds the Copper
      until after t(MJ). So I's MOVE is executed once in that frame, even
      if the Copper also ran from I's start before the second strobe; two
      executions in quick succession would leave no room for a handler
      record between them. The 68000 then loops for three complete frames
      without writing INTREQ or INTENA. It runs the sampler to find the
      frames.
3. **Observation.** The IR2 fetch times of I's MOVE, and the handler
   records with their store times, levels and INTREQR values.
4. **Expected.**
   - After each MOVE execution, and before the next one, there is a level-3
     record, and it shows INTREQR bit 4 = 1.
   - No record appears at any other level.
   - No record appears before the first MOVE execution.

   Sentences:
   - p.35: "To interrupt the 680x0, use the Copper MOVE instruction to store
     a 1 into the following bits of INTREQ: … 15 SET/CLR … 4 COPEN
     Coprocessor interrupting 680x0."
   - p.35: "you can interrupt the 680x0 and use its instruction set
     instead."
   - p.219: "Bit 4, COPER, is used by the Copper to issue a level 3
     interrupt. … this bit has been reserved for specifically identifying
     the Copper as the interrupt source."

   No page gives a latency, so only the order is graded.
5. **Preconditions.**
   - (a) The stored INTENAR equals $4010 in bits 14–0. Bit 15 of a read
     address "has no meaning" (p.218).
   - (b) The INTREQR read in step 3 shows bit 4 = 0.
   - (c) I's MOVE is executed once in each of the three frames.
   - (d) B1 and B3 (lines $10–$FF) for the frame boundaries. B2 is not
     asserted: P9 grades no line, and the handler interrupts the sampler.
6. **Contrasting cases.**
   - Main: MOVE data $0010 (bit 15 = 0, so bit 4 is cleared: p.35 Table
     2-1, p.218). No level-3 record appears.
   - For (a): INTENA = $C000. No record appears, as p.217 predicts
     ("Control registers … allow you to mask certain of these sources and
     prevent them from generating a 680x0 interrupt"). With INTENA = $8010
     (no INTEN), p.218 predicts the same: "If this bit is a 0, it disables
     all other interrupts".
   - For (b): the 68000 writes INTREQ = $8010 before the read, and INTREQ =
     $0010 after it. Bit 4 reads 1, and the rest of the run is unchanged.
   - For (c): list I without the MOVE.
   - For (d): as in 2.4.
7. **Reference frames.** 2.3, 2.4 (frames), 2.6.

### P10: INTREQ bit 15 decides whether bits written as 1 are set or cleared

1. **Proposition.** When the Copper writes INTREQ, the bits written as 1 are
   set if bit 15 of the value is 1 and cleared if it is 0. Bits written as 0
   are unchanged. Claims `ch2-the-copper-and-the-680x0.c5`,
   `COP2-the-copper-and-the-680x0.c5`, `COP2-the-copper-and-the-680x0.c6`,
   `COP2-the-copper-and-the-680x0.c7`; p.35. The rule for bits written as 0
   comes from p.218 and p.280.

   The plural "bits" is graded on two bits, together and separately: bit 4
   (named in Table 2-1) and bit 2 (SOFT, "Reserved for software-initiated
   interrupt", p.280). These are the bits no device sets on its own. The
   other request bits have device sources (p.219–220) that the pages do not
   say are idle, so they are not graded.
2. **Stimulus.**
   1. Common prologue (all interrupts disabled).
   2. Write INTREQ = $0014.
   3. List R:
      - `$4001,$FF00`, `$009C,$8014`
      - `$5001,$FF00`, `$009C,$0010`
      - `$6001,$FF00`, `$009C,$0004`
      - `$7001,$FF00`, `$009C,$8010`
      - `$FFFF,$FFFE`
   4. Start per 2.8 while a record shows V < $30. R's first WAIT (line
      $40) holds the Copper until after t(MJ), and the samples below lie in
      this same frame, so R's first run is the one graded.
   5. Run the polling sampler (INTREQR and position). Its records at lines
      $38, $48, $58, $68 and $78 are the five samples. For each line, the
      sample is the first record on that line.
3. **Observation.** Bits 4 and 2 of the five samples; the IR2 fetch times
   of R's four MOVEs; the store times of the samples.
4. **Expected.** (bit 4, bit 2) at each sample:

   | Line | Expected |
   |---|---|
   | $38 | (0,0) |
   | $48 | (1,1) |
   | $58 | (0,1) |
   | $68 | (0,0) |
   | $78 | (1,0) |

   Sentences:
   - p.35: "15 SET/CLR Set/Clear control bit. Determines if bits written
     with a 1 get set or cleared."
   - p.218: "When you wish to set a bit (make it a 1), you must place a 1 in
     the position you want to set and a 1 into position 15. When you wish to
     clear a bit (make it a 0), you must place a 1 in the position you wish
     to clear and a 0 into position 15. … Positions 14-0 that have 0 value
     will not be affected when you do the write."
   - p.280: "Bits written with a zero are always unchanged."
5. **Preconditions.**
   - (a) Order: MOVE k's IR2 fetch lies after the store of sample k−1 and
     before the store of the record just before sample k. So each sample is
     read after exactly one more MOVE.
   - (b) The $38 sample shows (0,0).
   - (c) INTENAR bits 14–0 read 0 (read and stored after step 2), and the
     68000 writes INTREQ nowhere after step 2.
6. **Contrasting cases.**
   - Main: change the second MOVE's data from $0010 to $8010. Expected at
     $58: (1,1). Change the third MOVE's data from $0004 to $8004. Expected
     at $68: bit 2 = 1.
   - For (a): change the second WAIT to `$5A01,$FF00`, two lines after the
     $58 sample. MOVE 2 then follows the store of sample 2, and the order
     check fails. (A WAIT for line $58 itself would place MOVE 2 at the
     start of the sample's own line, where the order against the record
     just before the sample is not fixed.)
   - For (b): in step 2 write INTREQ = $8004. The first sample then shows
     bit 2 = 1.
   - For (c): INTENA = $C004 with a level-1 handler that clears bit 2 (SOFT
     is level 1, p.280). INTENAR is then non-zero.
7. **Reference frames.** 2.4 (sample lines), 2.3.

### P11: Interlace recipe: a handler rewrites COP1LC, and the next field runs the new list

1. **Proposition.** In the recipe, the Copper issues a 68000 interrupt just
   after the first display line, and the handler writes COP1LC to point to
   the other field's list. The Copper then finishes the current field's list
   without jumping, and starts the next field at the newly written address.
   So the two lists alternate field by field. Claims
   `ch2-using-the-copper-in-interlaced-mode.c21`,
   `ch2-using-the-copper-in-interlaced-mode.c22.s2`; p.35. The restart from
   COP1LC is on p.25 and p.32.

   Two things are not graded. p.35 says COP1LC "will be automatically reset
   to point to the original list" (section 4, item 3). And no page ties
   either list to a particular field parity: p.34 says only that the even
   field's data start one line further on in memory ("the starting address
   for the even-numbered fields is one line greater").
2. **Stimulus.**
   1. Common prologue.
   2. Interlaced one-plane display: BPLCON0 = $1204 (one plane, colour, LACE
      bit 2: p.63–64), DIWSTRT = $2C81, DIWSTOP = $2CC1 (p.59: in interlace
      "the stopping position does not change"), DDFSTRT = $0038, DDFSTOP =
      $00D0, BPL1MOD = 40 (p.64: "40 for interlaced").
   3. Bitplane data at P: 512 lines × 40 bytes (p.64: "512 for PAL").
   4. List A:
      - BPL1PTH/L = P
      - `$0180,$0000`, `$0182,$0FFF`
      - `$2D01,$FF00` (line $2D, just after the first display line $2C,
        which is DIWSTRT's VSTART, p.58)
      - `$009C,$8010`
      - `$9601,$FF00`, `$0180,$0F00`
      - `$FFFF,$FFFE`
   5. List B: the same, at another address, with BPL1PT = P+40 (p.34: "the
      starting address for the even-numbered fields is one line greater").
   6. INTENA = $C010. The level-3 handler (2.6):
      1. Write INTREQ = $0010.
      2. MH1; write COP1LC = the list it did not write last time (B first);
         MH2.
      3. Store its record.
   7. Start per 2.8 with COP1LC = A and DMACON = $8380, when a record
      shows V between $1A and $28: vertical blanking has ended (p.219:
      line 25 for PAL) and A's WAIT for line $2D has not come yet. So the
      handler runs once in that field, even if the Copper also ran from A's
      start before the second strobe.
   8. Run the sampler for six fields.
3. **Observation.**
   - Fields are delimited by frame wraps (2.4).
   - Restarts (2.8).
   - The handler's MH1 and MH2 times.
   - Copper fetches in A's and B's regions between each MH2 and the next
     restart.
   - The address of the first bitplane fetch after each restart.
4. **Expected.**
   - (R1) The first restart after each MH2 is a fetch of the IR1 of the
     list that handler run wrote into COP1LC. The handler alternates, so
     consecutive restarts alternate between A's IR1 and B's IR1: the
     fields run A, B, A, B, …
   - (R2) Between each MH2 and the next restart, the running list's MOVE
     after its WAIT for $96 is fetched (IR1 and IR2) at its own address,
     and no Copper fetch lies in the other list's region: writing COP1LC
     causes no jump. The fetch of the WAIT for $96 itself is not graded:
     the Copper fetches it right after the INTREQ MOVE, and no page says
     whether that comes before or after the handler's write.
   - (R3) The first bitplane fetch after a restart at A's IR1 is at P, and
     after a restart at B's IR1 at P+40, so it alternates between P and
     P+40 from field to field.

   Sentences:
   - p.35: "To get the Copper to execute the correct list, you set an
     interrupt to the 680x0 just after the first line of the display. When
     the interrupt is executed, you change the contents of the COP1LC
     location register to point to the second list."
   - p.32: "The value in the COP1LC location registers will be restored to
     the Copper's program counter at the start of the vertical blanking
     period."
   - p.25: "Instruction fetch continues sequentially until the Copper is
     interrupted by another jump address strobe."
   - p.34: "the bitplane pointer must contain a different value for
     alternate fields of the interlaced display."
5. **Preconditions.**
   - (a) Interlace is on: consecutive fields differ in length by one line.
     p.24: "there are also long and short fields (interlace only). In NTSC,
     the fields are 262, then 263 lines and in PAL, 312, then 313 lines".
     Field lengths are measured as the highest V per field (2.4).
   - (b) In every complete field, a level-3 record, MH1 and MH2 follow the
     IR2 fetch of the running list's INTREQ MOVE, before the next restart;
     and MH2 is stored before the first record on line $96, so
     R2's MOVE comes after MH2.
   - (c) One restart per field: each complete field (time ranges as in
     5.2) contains exactly one restart, and every restart is a fetch of
     A's or B's IR1.
   - (d) B1 and B3 (lines $10–$FF) for the field boundaries. B2 is not
     asserted: P11 grades no line, and the handler interrupts the sampler.
6. **Contrasting cases.**
   - Main (R1, R3): the handler always writes COP1LC = A. Every restart is
     then A's IR1 and every first bitplane fetch is at P (p.30: "if the
     contents of COP1LC are not changed … the Copper will restart at the
     same location"). The alternation that R1 and R3 grade fails.
   - For R2: in the first handled field only, the handler writes COPJMP1
     after COP1LC. p.26: "When you write to a Copper strobe address, the
     Copper reloads its program counter from the corresponding location
     register". Expected in that field: B's instructions are fetched after
     MH2 (B runs from its start), and A's MOVE after its WAIT for $96 is
     not fetched before the next restart. B then runs to its end and the
     program still finishes. B's WAIT for $2D is already true, so the
     handler runs once more and writes COP1LC = A without a strobe.
   - For (a): BPLCON0 = $1200 (LACE clear). All fields then have equal
     length.
   - For (b): INTENA = $C000. No handler runs.
   - For (c): in one field the handler writes COP1LC = X, a third list that
     holds only `$FFFF,$FFFE`. The next restart is then X's IR1, which
     belongs to neither A nor B.
   - For (d): as in 2.4.
7. **Reference frames.** 2.4 (fields), 2.3, 2.8 (restarts), markers.

### P12: A WAIT with BFD = 0 holds the Copper until the blit it started has finished

1. **Proposition.** Suppose the Copper has started a blit and then executes
   a WAIT whose BFD bit is 0 and whose beam comparison is already true. It
   does not execute the next instruction until the blit has finished. So a
   following blitter start happens only after the earlier blit's last memory
   access. With BFD = 1, the beam comparison alone controls the WAIT. Claim
   `ch2-using-the-copper-with-the-blitter.c1.s2`; p.35, with the BFD
   paragraph on the same page.

   Not graded: "Changing blitter registers while the blitter is operating
   causes unpredictable results" gives no expected value. No run writes
   blitter registers while a blit is running.
2. **Stimulus.**
   1. Common prologue.
   2. Region D1 (64 lines × 32 words) and D2 (one word), both prefilled with
      $0000 by the 68000 before step 4 (2.3: the 68000 stays out of a data
      region inside its graded window).
   3. List K:
      - `$4001,$FF00`
      - `$009C,$0040` (clears INTREQ bit 6, the blitter-finished interrupt
        flag, p.220/p.187)
      - `$0040,$01FF`, `$0042,$0000`, `$0066,$0000`, `$0054,hi(D1)`,
        `$0056,lo(D1)`, `$0058,$1020` (blit 1; BLTSIZE last, p.200)
      - `$0001,$0000` (W1: BFD = 0. All compare-enable bits are 0, so the
        beam comparison is true at once. p.32: "If the corresponding enable
        bit is 0, the comparison is always true")
      - `$0180,$0F00` (marker MOVE)
      - `$0001,$0000` (W2: BFD = 0)
      - `$0054,hi(D2)`, `$0056,lo(D2)`, `$0058,$0041` (blit 2)
      - `$FFFF,$FFFE`
   4. Start per 2.8 with DMACON = $82C0 (SET/CLR, DMAEN, COPEN, BLTEN;
      BLTPRI, bit 10, stays 0), when a record shows V between $1A and $30.
      K's first WAIT (line $40) then holds the Copper until after t(MJ),
      so K's first run after t(MJ) begins at its IR1, and no blitter
      register is written twice by a list that also ran from K's start
      before the second strobe.
   5. Immediately after MJ, COPCON = $0002 (CDANG). p.26: "When CDANG is 1,
      these registers can be written by the Copper". p.310, for the ECS:
      "if this bit is set, the Copper can access all of the Amiga chip
      registers". So every page lets the Copper write $40–$7E here (section
      4, item 9). CDANG is set only once the second strobe has pointed the
      Copper at K: until then it may be running from an address the program
      did not choose, and p.26 gives CDANG's purpose as "Preventing the
      Copper from accessing the blitter control registers prevents a
      runaway Copper (caused by a poorly formed instruction list) from
      accidentally affecting system memory". Line $40 is at least 16 lines
      after MJ, so CDANG is set before K's first blitter MOVE.
   6. After the first complete frame, read INTREQR and store it.
3. **Observation.** In K's first run after t(MJ) (2.8), up to K's next
   restart:
   - t_s1: the IR2 fetch of blit 1's BLTSIZE MOVE.
   - t_mk: the IR2 fetch of the marker MOVE.
   - t_s2: the IR2 fetch of blit 2's BLTSIZE MOVE.
   - The first and last writes of $FFFF in D1 (t_last1 for the last).
   - t_b2: the write of $FFFF in D2.
4. **Expected.** t_mk > t_last1, and t_s2 > t_last1, and t_b2 > t_last1.

   Sentences, p.35:
   - "When the BFD bit is a 0, the logic of the Copper WAIT instruction is
     modified. The Copper will WAIT until the beam counter comparison is
     true and the blitter has finished."
   - "If the Copper is used to start up a sequence of blitter operations,
     it must wait for the blitter-finished interrupt before starting another
     blitter operation."

   A blit has not finished before its last write.
5. **Preconditions.**
   - (a) Blit 1 runs: 2048 writes of $FFFF in D1, the first after t_s1.
   - (b) Blit 1 lasts longer than the Copper takes for W1 plus the marker
     MOVE when BFD = 1. The main contrasting case shows this: there, t_mk <
     t_last1.
   - (c) The INTREQR read after the frame shows bit 6 = 1: the
     blitter-finished flag was set (p.220).
6. **Contrasting cases.**
   - Main: W1's IR2 = $8000 (BFD = 1). p.35: "Normally, this bit is a 1 and
     only the beam counter comparisons control the WAIT." Expected: t_mk <
     t_last1, so the marker runs while blit 1 is still writing. W2 stays at
     BFD = 0, so blit 2 still starts only after blit 1 and no blitter
     register is written during a blit.
   - For (a): DMACON = $8280 (no BLTEN). No write appears in D1. The program
     still finishes. What the Copper does at W1 in this run is not graded.
   - For (b): the main contrasting case (W1 BFD = 1) with blit 1's BLTSIZE
     = $0041, one word. p.193 gives a blit using only D four ticks per word
     ("The minimum blitter cycle is four ticks"; PAL clock "7.09
     megahertz"), under one microsecond, while W1 and the marker MOVE take
     six and four memory-cycle times (p.20) of about 280 ns each (p.194).
     Blit 1 then ends before the marker MOVE, and (b) fails.
   - For (c): drop both BLTSIZE MOVEs. Bit 6 stays 0 after the list's clear.
     This contrast only shows that check (c) can fail. What W1 does when no
     blit was started is not graded; the pages do not say.
7. **Reference frames.** 2.3 (list offsets and data regions), 2.4 (the
   start window only), 2.8 (MJ, restart).

---

## 4. Open points and disagreements between pages

1. **Name of INTREQ bit 4.** p.35 Table 2-1 (checked on the scan) names it
   "COPEN". p.219 ("Bit 4, COPER") and p.280 ("04 COPER") name it COPER.
   "COPEN" is also the name of DMACON bit 7 (p.30, p.222, p.278). Names are
   not graded. The function comes from p.35 and p.219.
2. **When the COP1LC restart happens.** The p.25 box says "At the start of
   each vertical blanking interval, COP1LC is automatically used … when the
   end of vertical blanking occurs, the Copper is automatically forced to
   restart". Its two sentences disagree. p.30 ("When the screen display ends
   and vertical blanking starts"), p.32 ("at the start of the vertical
   blanking period") and p.276 ("at the beginning of each vertical blank
   time") say start. p.33 puts the operating system's COP1LC write at the
   end. Every page agrees on one restart per frame, somewhere in vertical
   blanking, so that is all P5 and P11 grade.
3. **Who resets COP1LC.** p.35 says "during the vertical blanking interval,
   COP1LC will be automatically reset to point to the original list".
   p.33 says a graphics interrupt handler in the operating system writes
   COP1LC. p.25 and p.276 say only that the hardware loads the program
   counter *from* COP1LC. No test expects the hardware to rewrite COP1LC.
4. **Strobe by read.** The p.30 example strobes with `MOVE.W COPJMP1(a1),d0`,
   a read, commented "Causes copper to load PC from COP1LC". The prose on
   p.30, p.26 and p.276, and Appendix B's "S" code (p.287), say write. The
   package strobes by writing only and tests no read of a strobe address.
5. **End point of the display.** p.27 gives (226,262) NTSC and (226,312)
   PAL. p.24 says "262 NTSC (312 PAL) possible vertical positions", and p.34
   says "PAL has 312 lines normally". Lines 262 and 312 would therefore exist
   only in long interlaced fields (p.24, p.216 "PAL line counts (313)").
   p.23 gives NTSC long lines as "0-$E3", not a maximum of 226. P6 grades
   orientation only, and the two p.27 claims that locate the corner points
   are declined (section 1).
6. **Where the mask problem begins.** p.32 says "past 128 (hex $80)"; p.33
   says "fail after vertical position 127" and "greater than or equal 127".
   P7 grades only lines $81–$FF, where both statements agree.
7. **"This bit should be unset with caution"** (p.35). It is unclear whether
   this refers to BFD or to the "blitter-finished flag" named in the
   sentence before. Either way, the page only calls the consequence possible.
8. **Which flag BFD tests.** p.35 says "The blitter has finished when the
   blitter-finished flag is set", but names no register bit. Candidates are
   INTREQ bit 6 BLIT (p.220, p.187) and the busy flag DMACONR bit 14 (p.186).
   On older chips the busy flag is set only when the blitter gets its first
   DMA cycle; "Starting with the Fat Agnus" it is set as soon as BLTSIZE is
   written (p.187). P12's list clears INTREQ bit 6 before starting blit 1,
   and p.35's rule is unconditional. P12 grades only the relation to the
   blit's last write.
9. **Registers the Copper may write.** Three pages give different ranges:
   - p.21: "$20 or above"; "between $10 and $20" with the danger bit; never
     "lower than $10".
   - p.26: always $80–$FF; never $00–$3E; $40–$7E only with CDANG.
   - p.310 (ECS): with CDANG clear, $DFF03E–$DFF07E; with CDANG set, all
     registers. The same page gives the standard set's CDANG range as
     starting at $03E, where p.26 says $40.

   The package writes only $80 and above, plus $40–$7E with CDANG = 1 (P12),
   where every page allows the write.
10. **Line length.** p.23 says "In PAL, they are all 227 long". p.194 says
    "there are 227.5 'color clocks'" per line, in an NTSC setting ("about 63
    microseconds"). The package uses no line length. Lines come from beam
    records (2.4).
11. **"All of the DMA channels"** (p.30). Memory refresh is DMA (p.20, p.194,
    Figure 6-9), but neither DMACON table (p.222, p.278) has a bit for it.
    p.30's sentence stands in the paragraph on disabling Copper DMA through
    DMACON, and speaks of the channels DMACON enables; refresh, with no
    DMACON bit, is not one of them. P3 grades the channels the tables name,
    and so grades its two claims whole.
12. **Disk DMA with no drive.** p.246–248 do not say whether disk DMA
    transfers words when no drive is attached. P3 turns this into a
    precondition ((a)) rather than an expected value.
13. **Figure 6-9** (p.195). Its footnote says "This cycle 0 appears to
    exclude one of the memory refresh cycles. This is not the case. Actual
    system hardware demands certain specific values … Therefore this timing
    chart has been 'adjusted'". No slot number from the figure is used.


---

## 5. Recorded

Measured values of the plain run on each core (fx68k and TG68K), the layout
the programs use, and the mutation runs. Section 2's common ground is
unchanged.

### 5.1 Memory layout

Every program clears the overlay first (a write to $BFE001, the CIA-A
port), so the low $000000-$07FFFF area used for Copper lists and
DMA-channel data regions (2.1) is reachable for both CPU writes and
channel fetches; without it, measured, the region answers no access at
all. This package's own map: LIST1-LIST4 at $001000/$001200/$001400/
$001600, DATA1/DATA2 at $002000/$004000 (all below $80000, reachable once
the overlay is clear); RESULTS, marker cells (MARKBUF, $0C1000) and the
beam/poll sample buffer (RECBUF, $0C4000) in the ordinary identity-mapped
chip RAM above $80000.

### 5.2 Reference frame B (2.4)

Each record is four words (INTREQR, then VPOSR/VHPOSR as one long word,
then VPOSR read again) written by the 68000 program; the C++ side finds
each word's own store time from the RAM-port log, the same log the
fetch/channel checks use, never assuming a fixed sampling cadence. A record
is kept only if the two VPOSR reads agree on bit 0 (V8); a frame boundary
is a decrease from V >= 256 to V <= 1, not any V decrease.

Measured, on both cores: with that filter and that boundary, vsync comes
every 568,408 clk_sys from cycle 9,912 on. No frame is skipped or filtered;
"two complete frames" is the literal first two wrap-to-wrap spans
captured.

A dense loop around the sample, unrolled within each iteration, is what
reaches the anchor B2 density. Measured, on both cores: every graded line
carries 8 records except one, which carries 7; B2 is set at >= 7 records
per line on this basis.

A whole-frame membership question (does a fetch fall within complete frame
k) is measured on that frame's own time range, taken from the two
neighbouring wrap-detecting records' own store times (extended back to the
previous frame's last record, forward to the next frame's first): a
Copper restart's own fetch measures consistently just before the record
that first shows the lower V, in the same few-hundred-cycle gap on both
cores.

### 5.3 P4: the first fetch's exact address

Each of the six captures (Run 1, Run 2, Run 3, Run 3-reorder, and the two
comparator contrasts) is compared at the exact word address of its first
Copper fetch after t(M1) with the first word of the list the page selects
(p.25, p.26, p.30, p.276). The plain run prints each address. Measured, on
both cores:

| Capture | Selected list's first word | First fetch after t(M1) | Equal |
|---|---|---|---|
| Run 1 (COP1LC=A, COPJMP1) | A, $001000 | $001000 | yes |
| Run 2 (COP1LC=A, COP2LC=B, COPJMP2) | B, $001200 | $001000 | no |
| Run 3 (... COPJMP2, then COP2LC=C) | B, $001200 | $001000 | no |
| Run 3-reorder (COP2LC=C before COPJMP2) | C, $001400 | $001000 | no |
| comparator contrast B (COP1LC=B, COPJMP1) | B, $001200 | $001200 | yes |
| comparator contrast C (COP1LC=C, COPJMP1) | C, $001400 | $001400 | yes |

In each of the four runs the first fetch is exactly A's IR1. Each capture
is guarded by its own three preconditions (5 (a)-(c)): the first
stored DMACONR has bit 7 = 0, no Copper fetch in A, B or C before t(M1), and
the stretch from t(MS) to the first fetch lies outside vertical blanking.
All eighteen guard lines read PASS on both cores.

`P4` (Run 1 and Run 2 together, carrying `ch2-starting-the-copper-after-reset.c1.s2`
and `ch2-starting-the-copper-after-reset.c2.s2`) is red through Run 2's
clause, and `P4-Run2` states that clause for Run 2 alone. `P4-Run3` and
`P4-Run3-reorder` carry no claim id and grade the narrower clause of p.25's
"is loaded into the Copper program counter" against each run's own
location-register value at the moment its strobe was written: the value
that Run 3 held at the strobe was B, and the first fetch is A's. 
`P4-Run3-reorder` alone cannot isolate "the value at the strobe" (COP2LC
holds C both at the strobe and at DMA-on), so it repeats `P4-Run2`'s clause
with C. `P4-Run3-PAIR` grades Run 3 and Run 3-reorder together, as the
specification's own pair (P4 items 4 and 6). All five are red on both
cores. The comparator contrasts `P4-CTRL` and `P4-CTRL-C` read B's and C's
IR1 through the same comparator and are green on both cores.

### 5.4 The plain run

Measured, on both cores: 123 passed, 5 failed, and no RECORDED line. The
five failures are `P4`, `P4-Run2`, `P4-Run3`, `P4-Run3-reorder` and
`P4-Run3-PAIR`. Both cores print the same lines. A line whose
preconditions did not hold prints `RECORDED: <label> NOT GRADED` and
neither PASS nor FAIL; none does in the plain run.

### 5.5 P2 and P3

P2c: with the phase-2 sampler unrolled (5.2), P2c (anchor checks B1-B3
over the two complete frames after M2) reads PASS on both fx68k and
TG68K. The control `P2-OFF` writes DMACON = $0000, a no-op (p.218), carries
no claim id, and reads PASS on both cores: fetching continues and INTREQR
bit 4 returns.

P3 has nine data regions (P3 stimulus step 2), 32 KB each: BPL
$010000-$017FFF, SPR $018000-$01FFFF, AUD0 $020000-$027FFF, AUD1
$028000-$02FFFF, AUD2 $030000-$037FFF, AUD3 $038000-$03FFFF, DSK
$040000-$047FFF, BLT $048000-$04FFFF, COP $050000-$057FFF.

Refresh recognition (2.3): measured, on prog_p1.bin's captured log, every
RAM read where the refresh port reads asserted is at word address 0, and at
least one such read exists, the same signature 5.1 measured for P1/P2/P4,
checked directly against the port. Measured again on prog_p3_run_base.bin's
captured log: at least one refresh-asserted read exists, and none of them
falls inside any of the nine regions. Both lines carry no claim id. Since
every one of those regions starts at $10000 or above, a word-address-0 read
can never fall inside one; P3's region-bounded checks therefore exclude word
address 0 directly, exactly as P1/P2/P4 do, rather than sampling the
refresh port cycle by cycle for every access. No program controls the
address of a refresh access, so no stimulus can falsify the address
invariants of these two lines.

Disk DMA (precondition (a), section 4 item 12): the harness's `floppy_config`
= 0 is one drive (DF0), write-enabled, with no disk image served; the
program itself selects no drive. Measured, on both cores: disk DMA runs and
completes even so -- region DSK shows an access in Base, within its own
100-word transfer starting at DSKPTH ($100 above region DSK's own start),
none in the DSK case or in case M; DSKBYTR bit 14 (DMAON) reads 1 in Base
and 0 in the DSK case and in case M.

Base coverage (`P3-BASE`): measured, on both cores, every region other than
DSK has at least one access in Base -- BPL, SPR, AUD0-AUD3, BLT and COP are
all live there. Precondition (c)'s comparator: measured, on both cores, in
Base the lowest address accessed in each region equals that region's own
start plus $100, for every region a channel touches. The aggregate
`P3-ALL`, which carries `ch2-stopping-the-copper.c8` and
`COP2-stopping-the-copper.c11`, conjoins Base's coverage, the nine channel
cases and case M, and reads PASS on both cores; `P3-BASE`, the nine
channel cases and `P3-M` are its components and carry no claim id.

### 5.6 P6, P8 and P10

P6's own list O reuses the LIST1 address ($001000; a fresh harness instance
per run, so no other proposition's use of that address is disturbed).
Precondition (b) identifies each restart at the port as a read of the
list's first word after the fetch of the final WAIT's IR2, and its
per-restart MOVE-order check passes on both cores.

P6c (V1) in the plain run: the four video frames after the enable measure
568,408 clk_sys samples and 627 doubler rows each. With LACE (BPLCON0 bit
2) set, alone and with one bitplane fetched, the four frames measure
567,500 clk_sys samples and 626 doubler rows each and are equal to each other, so V1 holds and LACE does not
falsify it (908 samples and one doubler row fewer than without LACE). With BEAMCON0 written with its PAL bit clear a frame and a half
after the enable, the first frame measures 568,408 samples and 627 rows and
the next three measure 477,608 samples and 527 rows each, and P6c reads
FAIL.

P8: three device cases (COP, VB, BLIT) share one grading function applied
to each device's own bit, marker and event. The BLIT case reuses the DATA1
region ($002000) established for P3's BLT region; region D there measures
2048 writes of $FFFF in the graded window on both cores. The VB event is
the first frame wrap stored after t(MC). The wrap boundary measures a record
on line 312 followed by a record on line 0 on both cores, which is the
boundary the assertion requires. INTENAR bits 14-0 read 0 in all three
cases.

P10: list R reuses the LIST1 address ($001000). The five samples (lines
$38, $48, $58, $68, $78) and the four MOVE executions are located from the
same port-log mechanism P1-P8 use; precondition (a)'s ordering check (each
MOVE's IR2 fetch between the previous sample's store and the record just
before the next sample) passes on both cores.

### 5.7 Mutation runs

Each run below executes the built binary of one core with `NMH_ONLY` set to
the section and `NMH_MUTATE` set to the listed selectors, which act on
different lines. The last column lists the lines that read the stated
verdict; fx68k and TG68K agree on every one. A record in the test source's
header names one selector and one line.

| Run | NMH_ONLY | NMH_MUTATE | Lines |
|---|---|---|---|
| fixture | FIXTURE | FIXTURE | FAIL: FIXTURE |
| p1_a | P1 | P1-FIX-DONE-NODONE, P1-OFF-FIX-STRUCT-EMPTY | FAIL: P1-FIX-DONE, P1-OFF-FIX-STRUCT |
| p1_b | P1 | P1-OFF-FIX-DONE-NODONE, P1-FIX-STRUCT-EMPTY | FAIL: P1-OFF-FIX-DONE, P1-FIX-STRUCT |
| p1_c | P1 | P1-A, P1-B, P1-C, P1-OFFA, P1-OFFB, P1-OFFC | FAIL: P1a, P1b, P1c, P1-OFFa, P1-OFFb, P1-OFFc |
| p1_d | P1 | P1-SWAP | FAIL: P1, P1-ON-OFF |
| p1_e | P1 | P1OFF-SWAP | FAIL: P1-OFF, P1-ON-OFF |
| p2_a | P2 | P2-FIX-DONE-NODONE, P2-OFF-FIX-STRUCT-EMPTY | FAIL: P2-FIX-DONE, P2-OFF-FIX-STRUCT |
| p2_b | P2 | P2-OFF-FIX-DONE-NODONE, P2-FIX-STRUCT-EMPTY | FAIL: P2-OFF-FIX-DONE, P2-FIX-STRUCT |
| p2_c | P2 | P2-A, P2-B, P2-C | FAIL: P2a, P2b, P2c |
| p2_d | P2 | P2-SWAP, P2OFF-SWAP | FAIL: P2, P2-OFF |
| p3_a | P3 | P3-APP-I-NODONE, P3-REFRESH-NODONE | FAIL: P3-APP-I, P3-REFRESH |
| p3_fix | P3 | P3-FIX-Base-NODONE, P3-FIX-BPL-NODONE, P3-FIX-SPR-NODONE, P3-FIX-AUD0-NODONE, P3-FIX-AUD1-NODONE, P3-FIX-AUD2-NODONE, P3-FIX-AUD3-NODONE, P3-FIX-DSK-NODONE, P3-FIX-BLT-NODONE, P3-FIX-COP-NODONE, P3-FIX-M-NODONE | FAIL: P3-FIX-Base, P3-FIX-BPL, P3-FIX-SPR, P3-FIX-AUD0, P3-FIX-AUD1, P3-FIX-AUD2, P3-FIX-AUD3, P3-FIX-DSK, P3-FIX-BLT, P3-FIX-COP, P3-FIX-M |
| p3_pre | P3 | P3-A, P3-B, P3-C, P3-D | FAIL: P3a, P3b, P3c, P3d |
| p3_base | P3 | P3-BASE-SWAP | FAIL: P3-BASE, P3-ALL |
| p3_bpl | P3 | P3-BPL-SWAP | FAIL: P3-BPL, P3-ALL |
| p3_swaps | P3 | P3-SPR-SWAP, P3-AUD0-SWAP, P3-AUD1-SWAP, P3-AUD2-SWAP, P3-AUD3-SWAP, P3-DSK-SWAP, P3-BLT-SWAP, P3-COP-SWAP, P3-M-SWAP, P3-DSKBYTR-SWAP | FAIL: P3-SPR, P3-AUD0, P3-AUD1, P3-AUD2, P3-AUD3, P3-DSK, P3-BLT, P3-COP, P3-M, P3-DSKBYTR |
| p4_fix | P4 | P4-FIX-Run1-NODONE, P4-FIX-Run2-NODONE, P4-FIX-Run3-NODONE, P4-FIX-Run3-reorder-NODONE, P4-FIX-CTRL-NODONE, P4-FIX-CTRL-C-NODONE | FAIL: P4-FIX-Run1, P4-FIX-Run2, P4-FIX-Run3, P4-FIX-Run3-reorder, P4-FIX-CTRL, P4-FIX-CTRL-C |
| p4_guards | P4 | P4-A-RUN1, P4-A-RUN2, P4-A-RUN3, P4-A-RUN3R, P4-A-CTRL, P4-A-CTRLC, P4-B-RUN1, P4-B-RUN2, P4-B-RUN3, P4-B-RUN3R, P4-B-CTRL, P4-B-CTRLC, P4-C-RUN1, P4-C-RUN2, P4-C-RUN3, P4-C-RUN3R, P4-C-CTRL, P4-C-CTRLC | FAIL: P4a-Run1, P4a-Run2, P4a-Run3, P4a-Run3-reorder, P4a-CTRL, P4a-CTRL-C, P4b-Run1, P4b-Run2, P4b-Run3, P4b-Run3-reorder, P4b-CTRL, P4b-CTRL-C, P4c-Run1, P4c-Run2, P4c-Run3, P4c-Run3-reorder, P4c-CTRL, P4c-CTRL-C |
| p4_swaps | P4 | P4-RUN2-CTRL, P4-RUN3-CTRL, P4-RUN3R-CTRL, P4-CTRL-RUN1, P4-CTRLC-RUN1 | FAIL: P4-CTRL, P4-CTRL-C; PASS: P4, P4-Run2, P4-Run3, P4-Run3-reorder |
| p4_pair | P4 | P4-RUN3-PAIR-A | PASS: P4-Run3-PAIR |
| p10_a | P10 | P10-FIX-DONE-NODONE, P10-MAIN2-FIX-DONE-NODONE | FAIL: P10-FIX-DONE, P10-MAIN2-FIX-DONE |
| p10_b | P10 | P10-FIX-LINES-EMPTY | FAIL: P10-FIX-LINES |
| p10_c | P10 | P10-A, P10-B, P10-C | FAIL: P10a, P10b, P10c |
| p10_d | P10 | P10-SWAP, P10-MAIN2-SWAP | FAIL: P10, P10-MAIN2 |
| p8_fix | P8 | P8-COP-FIX-DONE-NODONE, P8-VB-FIX-DONE-NODONE, P8-BLIT-FIX-DONE-NODONE, P8-COP-DEV-FIX-DONE-NODONE, P8-COP-III-FIX-DONE-NODONE, P8-VB-DEV-FIX-DONE-NODONE, P8-BLIT-DEV-FIX-DONE-NODONE | FAIL: P8-COP-FIX-DONE, P8-VB-FIX-DONE, P8-BLIT-FIX-DONE, P8-COP-DEV-FIX-DONE, P8-COP-III-FIX-DONE, P8-VB-DEV-FIX-DONE, P8-BLIT-DEV-FIX-DONE |
| p8_guards | P8 | P8-C, P8-VB-C, P8-BLIT-C, P8-A, P8-VB-A, P8-BLIT-A, P8-COP-B, P8-B, P8-BLIT-B | FAIL: P8c-COP, P8c-VB, P8c-BLIT, P8a-COP, P8a-VB, P8a-BLIT, P8b-COP, P8b-VB, P8b-BLIT |
| p8_bnd | P8 | P8-VB-BOUNDARY | FAIL: P8-VB-BOUNDARY |
| p8_cop | P8 | P8-COP-SWAP | FAIL: P8-COP, P8 |
| p8_vb | P8 | P8-VB-SWAP | FAIL: P8-VB, P8 |
| p8_blit | P8 | P8-BLIT-SWAP | FAIL: P8-BLIT, P8 |
| p8_dev | P8 | P8-COP-DEV-SWAP, P8-COP-III-SWAP, P8-VB-DEV-SWAP, P8-BLIT-DEV-SWAP | FAIL: P8-COP-DEV, P8-COP-III, P8-VB-DEV, P8-BLIT-DEV |
| p6_a | P6 | P6-FIX-FRAME-NODONE | FAIL: P6-FIX-FRAME |
| p6_b | P6 | P6-FIX-ROWS-MUTA | FAIL: P6-FIX-ROWS |
| p6_c | P6 | P6-i-FIX-NODONE, P6-ii-FIX-NODONE, P6-A, P6-B, P6-C3 | FAIL: P6-i-FIX, P6-ii-FIX, P6a, P6b, P6c |
| p6_d | P6 | P6-BELOW-SWAP, P6-ORDER-SWAP, P6-I-SWAP, P6-II-SWAP | FAIL: P6-BELOW, P6-ORDER, P6-i, P6-ii |

### 5.8 Not built

P5, P7, P9, P11 and P12 are not built.
