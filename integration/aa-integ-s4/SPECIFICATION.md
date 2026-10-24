# Specification: AA bit-plane fetch window, display window and DMA control (package aa-s4)

This is the specification of one integration test package for an FPGA
re-implementation of the Amiga chip set, graded against Commodore's
*Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91
(marked PRELIMINARY). It was written from the package's claims
(`claims-spec-stage.jsonl`: 70 claims, all from pages 21 and 22) and from the
specification's pages, by an author who never saw the implementation or any
test. "p.NN" is a page of the AA specification (its PDF page; the document
prints no page numbers). "HRM p.NN" is a printed page of the *Amiga Hardware
Reference Manual*, 3rd edition. The HRM is quoted only where the AA
specification builds on it, to explain a choice of stimulus or a restriction;
no expected value below is taken from it. The quoted lines of p.21 and p.22
were compared with the page scans and read the same there.

The package has 23 propositions (P1 to P23), and three joint assertions
(J1 to J3, 2.7) for the claims that name two registers at once. Twenty-five
claims are asserted by no line. They are listed first. P5 is a record of
measurements and asserts nothing about bit 2; P6, P13 and P15 are graded
lines that carry no claim id.

---

## 1 Claims that give no proposition

| Claim | Page | Why no proposition |
|---|---|---|
| opus:AA-ddf.use-b00, opus:AA-ddf.use-b08, opus:AA-ddf.use-b09, opus:AA-ddf.use-b10, opus:AA-ddf.use-b11, opus:AA-ddf.use-b12, opus:AA-ddf.use-b13, opus:AA-ddf.use-b14, opus:AA-ddf.use-b15 | 21 | These bits are printed `X`. The only sentence about them is a rule for the programmer: "(X bits should always be driven with 0 to maintain upward compatability)". The page states no effect of writing 1 to them, so there is no expected value. The package adopts the rule (every DDFSTRT and DDFSTOP write has bit 0 and bits 15-8 at 0) and grades nothing about these bits. |
| opus:AA-ddf.use-b01 (bit 01 is H2) | 21 | The page names bit 1 as the position bit H2 but gives no condition under which one H2 step (2 memory cycles, see 2.4) moves the fetch. The manual the page builds on lists bit 1 as unused for the original chips ("Bits 1-0 - not used", HRM p.91) and says that even the next bit up is "only respected in Hires Mode" (HRM p.91). No page says in which resolution or fetch mode H2 acts, so any expected value would be invented. |
| opus:AA-ddf.c10, sol:AA-ddf.c6 | 21 | "Note that these numbers will vary with variable beam counter mode set. (The maxes & mins, that is)". No number is given for that mode, so there is no expected value. The package keeps variable beam counter mode off (2.5). |
| opus:AA-dmacon.b09b (DMAEN also enables UHRES DMA) | 22 | The UHRES pointer addresses VRAM ("VRam (UHRES) bit plane pointer", p.10), and its cycle is identified to external logic on the RGA bus ("BPLHDAT comes out as an identifier on the RGA lines when the pointer address is valid so that external detectors can use this to do the special cycle for the VRAMs", p.15). The test sees neither VRAM nor the RGA bus, and no page says the cycle is also a Chip-memory access. The enabling condition is incomplete as well: UHRES "(also needs bits in DMACON)" (p.16) does not name those bits. A test could never go red. |
| opus:AA-dmacon.b10b ("Blitter Nasty") | 22 | The line gives a second name for bit 10. It states no behaviour. |
| opus:AA-ddfstrt.t1, sol:AA-ddfstrt.c5, opus:AA-ddfstop.t3, sol:AA-ddfstop.c7 | 21 | Each claim is a row that the table labels "(max)": "Extra wide (max) * 0 0 1 0 1" and "wide (max) 1 1 0 1 1". "(max)" states a limit: no DDFSTRT value starts the fetch earlier than the extra-wide row, and no DDFSTOP value ends it later than the wide row. No proposition grades that limit. Grading it would need values beyond the rows, and for DDFSTRT the manual the page builds on puts the limit elsewhere ("a leftmost limit to DDFSTRT of ($18)", HRM p.80). The claims are declined whole. The two rows' bits (0x0028, 0x00D8) remain configurations of P1-P4, as the rows against which the other rows are placed. |
| opus:AA-dmacon.b10c | 22 | "(disables /BLS pin, preventing micro from stealing any bus cycles while blitter DMA is running)". /BLS is a pin, not a port of `nanomig.v` (port list), so the clause about the pin cannot be graded at this boundary, and the claim is declined whole. P20 grades BLTPRI under opus:AA-dmacon.b10. |
| opus:AA-dmacon.c6 | 22 | "This register controls all of the DMA channels, and contains Blitter DMA status bits." The page prints two status bits, BBUSY (bit 14) and BZERO (bit 13). P21 grades BBUSY's status behaviour. BZERO's status behaviour, that the bit follows the blitter's zero result, is only a precondition of P23 ((ii)), and no proposition grades it. The claim is declined whole. |
| opus:AA-ddf.use-b02 (bit 02 is H3) | 21 | The page names bit 2 H3 and states no condition. The pages that bear on the hires fetch do not give one reading (section 4, item 3). HRM p.91 puts H3 into force in hires and HRM p.60 gives "DDFSTRT = DDFSTOP - (4 * (word count - 2)) for high resolution", while HRM p.195 Figure 6-9 says "Data fetch start can only be specified at even multiples of 8 clocks" and HRM p.197 Figure 6-12 draws the hires fetch in aligned blocks; HRM p.80's "only one word is fetched at this limit" belongs to the `$D8` limit, which the hires configurations of P5 do not use. No exact movement of either fetch edge, and no unchanged opposite edge, follows from every one of these readings. P5 records what the hires configurations show and asserts nothing about bit 2. |
| opus:AA-ddf.c6, sol:AA-ddf.c3 | 21 | "The vertical Bit Plane DMA timing is identical to the Display windows described above." Identity needs two line sets that are measured independently: the lines on which bit-plane DMA fetches, and the lines of the display window. The window instrument (2.4, RF-V) shows the window only where bit-plane data is fetched (5.5), so its line set is not independent of the fetch, and no page fixes an absolute window line set (the stop line's convention is open, section 4, item 9). No assertion that grades equality of the complete DMA line set and the complete window line set fits, and the claims are declined whole. P6 grades the DMA lines against the registers and carries neither id. |
| opus:AA-diwhigh.bit-b01, opus:AA-diwhigh.bit-b09 | 22 | Bit 1 is V9 of the start and bit 9 is V9 of the stop. A weight of 0x200 lines puts the start at 0x228 or the stop at 0x310, lines that a PAL field (about 312 lines) never reaches. The only observation is whether the window is present, or stays open to a given line. A missing window does not separate bit 1 from any other mapping that moves the start out of range (V10, a larger position, or another disabling condition), and a window that stays open does not separate bit 9 from any mapping that puts the stop later than the last line graded. No observation at the ports distinguishes an exact weight of 0x200 lines, and variable beam counter mode, which could lengthen the field, has no page-given numbers (opus:AA-ddf.c10 above). The claims are declined whole. P13 and P15 grade the consequence the page predicts and carry neither id. |

None of the package's claims needs a disk surface.

---

## 2 Rules shared by every proposition

### 2.1 System and program

- AA chip set selected (`Config.chipset_config = 0x10`), PAL, 2 MB Chip
  memory. Each proposition is run on both builds that `nanomig.mk` makes, in
  the CPU core's default 68000 behaviour. No proposition uses 68010 or 68020
  behaviour.
- Both builds are graded. The chip set is selected by the `chipset_config`
  port, which every run sets to 0x10; `ENABLE_AGA` appears in no simulated
  source, so the two builds run the same chip set and differ only in the CPU
  core. Every proposition, precondition and contrast prints PASS or FAIL on
  both builds.
- The programs are assembled for the 68000, vasm's default CPU. With
  `-m68010` or higher, vasm assembles `MOVE #0,<ea>` as `CLR <ea>`, and on a
  68000 a CLR reads its address before it writes it: a store of index 0 to
  MARK would then add a CPU read in a region that 2.3 allows only CPU writes.
- The program runs from Chip memory. Its first instruction sets the status
  register to 0x2700 (interrupts masked). It then writes INTENA <- 0x7FFF,
  INTREQ <- 0x7FFF and DMACON <- 0x7FFF. For DMACON this clears every
  control bit: "15 SET/CLR Set/Clear control bit. Determines if bits written
  with a 1 get set or cleared. Bits written with a zero are unchanged."
  (p.22). DMA channels are later turned on by writes with SET/CLR = 1.
- Custom registers are addressed at their offsets from 0xDFF000 (register
  list, p.6-p.10).
- Every bit the pages print as `X` or `x` is written 0: "(X bits should always
  be driven with 0 to maintain upward compatability)" (p.21), "Don't care (x)
  bits should always be written to 0 to maintain upwards compatibility."
  (p.22), "x= don't care; but drive to 0 for upward compatibility !" (p.16).
- The program writes no register that a proposition does not name, except
  those in the standard set-ups (2.5).
- Bit-plane pointers are written again in every vertical blank: "This pointer
  must be reinitalized by the processor or coprocessor to point to the
  beginning of Bit Plane data every vertical blank time." (p.15). Sprite
  pointers follow the same rule: "These address registers must be initialized
  by the processor or coprocessor every vertical blank time." (p.28).
- The Copper is started only in the order p.21 prescribes: "It is important
  that one of the jump registers be initalized and it's jump strobe address
  hit, after power up but before Coprocessor DMA is initalized." The program
  writes COP1LCH and COP1LCL with the list's address, then writes COPJMP1
  (a strobe; any data), and only then sets COPEN (with DMAEN) in DMACON.
- A blit is started by writing BLTSIZE last: "Writing to this register will
  start the Blitter, and should be done last, after all pointers and control
  registers have been initialized." (p.14).
- After every DMACON write that a proposition names, the program reads DMACONR
  and stores the value to a word of MARK. DMACONR is "DMA control (and
  blitter status) read" (p.22), so the stored word shows which control bits
  are in force.

### 2.2 Several configurations in one run

Most propositions compare configurations. Each configuration may be run
separately. It may also be one field of a longer run, under these rules:

- The CPU finds the vertical blank by reading VPOSR (bit 0 is V8) and VHPOSR
  (bits 15-8 are V7-V0) (p.29). It waits until V8 = 0 and
  0x04 <= V7-V0 <= 0x14. It then writes every register of the configuration,
  stores the configuration's index to MARK, and waits until V7-V0 >= 0x30
  before it looks for the next vertical blank.
- A configuration is observed in the field in which it was written, from
  beam line 0x1D to the end of that field.
- A configuration whose display window does not close before the end of the
  field is the last of its run.

### 2.3 Memory layout and attributing Chip-memory accesses

Every region lies in the Chip memory that the harness maps one to one:
"Program, stack and results all live in chip RAM that nanomig.v maps
one-to-one ($080000-$1FFFFF with 2 MB chip RAM)" (`nanomig.i`). Nothing lies
below $080000. After reset "An additional copy of the system ROM responds
starting at memory location $00000000. The system RAM that would normally be
located at this address is not available." (HRM p.223), and no program of
this package clears the overlay. Every region also lies below $100000, so
that a DMA pointer reaches it whether the "20 bit address" of the blitter and
sprite pointers (p.13, p.28) counts bytes or words. The program starts at
`PROGRAM_ORG` = $080400, MARK starts at `RESULTS` = $0C0000, `DONE_ADDR` is
$0C0FFE, and the stack grows down from `PROGRAM_STACK` = $0FFF00 (all
`nanomig.i`). Regions are word-aligned, disjoint, and at least 2 KB apart, so
that a pointer running past the end of its region shows up. Addresses are
byte addresses:

| Region | Address, size | Content | Accessed during an observation only by |
|---|---|---|---|
| PROG | $080400, ends below $08C000 | the program | CPU (instruction fetches) |
| COP | $08C000, at most 2 KB | Copper list | Copper reads |
| SPR0-SPR7 | SPRx at $090000 + x * $1000, 40 bytes each | sprite data (P19) | sprite DMA reads |
| BLT_S | $098000, 8 KB | blitter source | blitter reads |
| BLT_D | $09C000, 8 KB | blitter destination | blitter writes |
| SCR | $0A0000, 40 000 bytes | scratch area (P20) | CPU writes |
| B1 | $0B0000, 32 KB | bit-plane data | bit-plane DMA reads |
| MARK | $0C0000, at most 2 KB | configuration indices, DMACONR copies, end marker, the loop markers of P20 | CPU writes |
| SAMP | $0C2000, 16 KB | DMACONR samples (P21-P23) | CPU writes |
| B2 | $0C8000, 32 KB | bit-plane data | bit-plane DMA reads |
| B3 | $0D1000, 32 KB | bit-plane data | bit-plane DMA reads |
| B4 | $0DA000, 32 KB | bit-plane data | bit-plane DMA reads |
| Z | $0E3000, 32 KB | bit-plane data, all zero | bit-plane DMA reads |

The CPU writes the content of COP, B1-B4, Z, SPRx and BLT_S before it
enables the DMA that reads them. After that it does not access them. Every
proposition asserts, as a precondition, that no agent other than the one
listed accesses a region it uses during the observed interval.

The harness attributes no access to an agent: a logged access carries its
time, word address, data, direction and byte lanes (`RamAccess` in
`nanomig_harness.h`), and apart from `refresh`, which marks a refresh cycle,
no port in the port list says which agent made an access. Each access is
therefore attributed by region and direction alone, and the precondition
above is checked in the form the ports allow: during the observed interval
every access in a region has the direction the table gives, and the program
contains no instruction that accesses COP, B1-B4, Z, SPRx, BLT_S or BLT_D
after it has enabled the DMA that uses them.

### 2.4 Reference frames

The page states horizontal positions as bits H8, H7, ... and vertical
positions as V7, ..., V0. The frames below tie these names to the times the
test sees at the Chip-memory port and to the rows it sees on the video
output.

**H-bit weights.** Bits 7-1 of DDFSTRT and DDFSTOP are H8-H2: "USE X X X X X
X X X H8 H7 H6 H5 H4 H3 H2 X" (p.21). The beam counter uses the same names
on the same bit positions: VHPOSR "USE V7 V6 V5 V4 V3 V2 V1 V0,H8 H7 H6 H5
H4 H3 H2 H1", "RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)" (p.29). The
page also says "(memory cycles are 280 ns)" (p.21). So H1 is one memory
cycle and Hn is 2^(n-1) memory cycles. Read as a number, the register value
(bit 0 zero) is a horizontal beam position in memory cycles. DDFSTRT and
DDFSTOP keep these names. The renaming to 35 ns steps applies to "all 3
display window registers" (p.22 NOTE). The list of comparators that now
have 35 ns resolution is "DIWHIGH,HBSTRT,HBSTOP,SPRCTL,BPLCON1" (p.5), and it
does not include DDFSTRT or DDFSTOP. The Copper's WAIT uses the same names:
IR1 bits 7-1 are HP8-HP2 (p.20).

**RF-M: the memory cycle, P.** Copper list L-A (2.5) contains WAIT (0x3C,
0x40) followed by WAIT (0x3C, 0xC0). WAIT means "Wait until beam counter is
equal to, or greater than. (keeps Coprocessor off of bus until beam position
has been reached)" (p.20). Let t40 and tC0 be the times of the Copper's
fetch of the first word of the instruction after each of those two WAITs.
Then P = (tC0 - t40) / 128 system clock cycles per memory cycle. Both WAITs
end at an even horizontal position, and the Copper "requests the bus only
during odd memory cycles" (p.21), so both resumptions have the same delay.
No bit-plane, sprite, audio, disk or blitter DMA runs on line 0x3C in the
set-ups that use RF-M. Checks: tC0 - t40 is a positive multiple of 128
system clock cycles, and P is the same in every field and every run.
Contrast for the check: WAIT (0x3C, 0xBE) in place of (0x3C, 0xC0) makes the
difference 126 memory cycles, and the multiple-of-128 check fails.

**RF-L: line anchors.** L-A contains WAIT (v, 0x04) for every beam line
v = 0x3E ... 0xF2. A(v) is the time of the Copper's fetch of the first word
of the instruction after WAIT (v, 0x04). Line v is the interval
[A(v), A(v+1)), for v = 0x3E ... 0xF1. T0 is the time of the field's first
Copper fetch from the start of the list: "COP1LC is automatically used at
the beginning of each vertical blank time." (p.21). Checks:

- A(v+1) - A(v) is the same value, the line period L, for every v in range.
  In PAL, with variable beam counter mode off, there are no long lines. The
  strobe that identifies "lines with long counts(228- NTSC, HTOTAL+2-
  VARBEAMEN=1 hires chips only)" (p.29) names only NTSC and variable beam
  counter mode, and the manual the page builds on says "In PAL, they are all
  227 long." (HRM p.23). The sentence "If the PAL bit & LOLDIS are not high,
  long line/short line toggle will occur" (p.24) belongs to HTOTAL, which is
  "Active if VARBEAMEN=1 or DUAL=1" (p.24), and is not the source here. Both
  set-ups that use RF-L write PAL = 1 and VARBEAMEN = 0 (2.5).
- L and A(0x3E) - T0 are the same in every field and every run of the
  package.
- No Copper fetch lies outside COP, and there is none between the fetch of
  the list's END instruction and the next T0.

Contrast for the check: WAIT (v, 0x0C) in place of (v, 0x04) in one
configuration moves A(0x3E) - T0 later, and the check fails. The contrast
position lies past the four refresh cycles, which the manual the page builds
on draws on odd cycles up to $05 (HRM p.195, Figure 6-9). The Copper
"requests the bus only during odd memory cycles" (p.21), so after
WAIT (v, 0x04) its fetch falls at $05, or at $07 if refresh holds $05. A
contrast at 0x06, whose fetch falls at $07, could then leave A(v) where it
was.

**RF-F: fields for propositions without a Copper list (P16-P23).** Field k
is the interval between the CPU's k-th and (k+1)-th store to MARK in the
vertical-blank loop of 2.2. Check: consecutive spacings are equal to within
Q, the instrument resolution. The page basis for equal fields is "there are
also long and short fields (interlace only)" (HRM p.24): with LACE = 0 all
fields have the same length. Each store follows a polled wait for the
vertical blank, so the store's time is known only to within one pass of the
program's polling loop. Q is that duration. It is derived from the run, not
from a page: Q is the longest interval between two consecutive instruction
fetches of the polling loop's first instruction on the RAM-port log, over
the polling stretches of the two fields, and the test prints Q and the
spacings as MEASURED lines. A tolerance of Q says only that the instrument
cannot resolve the field boundary more finely. Q is required to be shorter
than a line (1816 system clock cycles, the line period RF-L measures).
Contrasts for the check: leaving out the "wait until V7-V0 >= 0x30" step
puts two stores into one vertical blank, and the check fails; and
BPLCON0 bit 2 (LACE) = 1 makes alternate fields differ by one line
(HRM p.24: 262 then 263 lines, PAL 312 then 313), far more than Q, so the
check fails through the equality clause while the other preconditions of the
proposition hold.

**RF-V: video rows and beam lines.** This frame uses the window instrument
of 2.5, in which the display window shows red and the border shows black.
Output rows are delimited by horizontal sync and fields by vertical sync.
The video shares the simulation's time base. The observed video field of a
configuration is the first one whose marker rows appear after the
configuration's store to MARK. A pixel is red when R = 0xF, G = 0, B = 0,
and black when R = G = B = 0.

The Copper list's marker block makes beam lines 0x20 and 0x24 show COLOR00
(red) across the whole unblanked width, because it clears BRDRBLNK for those
two lines. Let r20 and r24 be the first rows of the first and the second red
block of the field. Then k = (r24 - r20) / 4 output rows per beam line,
because the output passes a line doubler. The beam line of row r is
0x20 + floor((r - r20) / k). Checks:

- k is a whole number >= 1.
- Each marker block is exactly k rows tall.
- The rows of lines 0x1D-0x1F, 0x21-0x23 and 0x25 contain no red.
- On marker rows, red reaches further left than on any window row, which
  separates marker rows from window rows.

Win is the set of beam lines, other than 0x20 and 0x24, whose rows contain
a red pixel. Contrast for the check: BPLCON3 <- 0x0C00 (BRDRBLNK = 0)
throughout makes every unblanked row red, and the marker checks fail.

### 2.5 Standard set-ups and Copper lists

Copper instructions follow p.20. MOVE: IR1 carries the register offset
(DA8-DA1 in bits 8-1, bit 0 = 0), and IR2 carries the data. WAIT: IR1 is
VP7-VP0 in bits 15-8, HP8-HP2 in bits 7-1, and bit 0 = 1. IR2 = 0xFFFE:
BFD = 1 ("When this bit is true, the Blitter Finished flag will have no
effect on the Coprocessor", p.20), VE6-VE0 and HE8-HE2 all 1, bit 0 = 0.
END is the pair 0xFFFF, 0xFFFE.

**Marker block.**

    WAIT (0x20,0x1C); MOVE BPLCON3 <- 0x0C00
    WAIT (0x21,0x1C); MOVE BPLCON3 <- 0x0C20
    WAIT (0x24,0x1C); MOVE BPLCON3 <- 0x0C00
    WAIT (0x25,0x1C); MOVE BPLCON3 <- 0x0C20

The WAIT position 0x1C puts each change of BRDRBLNK inside horizontal
blanking, so that it falls between two visible stretches of the output. The
manual the page builds on says "The horizontal beam position has a value of
$0 to $E2." and "Horizontal blanking falls in the range of $0F to $35. The
standard screen (320 pixels wide) has an unused horizontal portion of $04 to
$47 (during which only the background color is displayed)." (HRM p.23). The
visible stretch that follows the blanking of line v therefore runs from $35
of line v to $0E of line v+1. A change made between $00 and $0E of line v
would colour the right end of the stretch of line v-1, and the rows of line
0x1F or 0x23 would show red. After WAIT (v, 0x1C) the Copper fetches the MOVE
on the next two odd memory cycles (p.21), and the write lands inside
$0F-$35.

**List L-A** (propositions graded at the Chip-memory port):

    MOVE BPL1PTH <- high word of B1;  MOVE BPL1PTL <- low word of B1
    [P7 only]  MOVE BPL2PTH/BPL2PTL <- B2
    [P6 only]  marker block
    WAIT (0x3C,0x40); WAIT (0x3C,0xC0)                  (RF-M)
    WAIT (v,0x04)  for v = 0x3E, 0x3F, ..., 0xF2          (RF-L)
    END

In P6, the pointer MOVEs load Z in place of B1.

**List L-V** (window propositions graded on video):

    MOVE BPL1PTH <- high word of Z;  MOVE BPL1PTL <- low word of Z
    marker block
    END

**S-FETCH** (P1-P5, P7):

- BEAMCON0 <- 0x0020: PAL = 1, VARBEAMEN = 0, all other bits 0 (p.12). This
  is the program's first custom-register write after the three clearing
  writes of 2.1. It makes explicit that variable beam counter mode is off,
  because the DDF numbers are given for that case ("these numbers will vary
  with variable beam counter mode set", p.21). The AA pages state a reset
  value only for HARDDIS.
- BPLCON0 as the proposition states. The base value is 0x1000: BPU2-0 = 001,
  one bit-plane; HIRES = SHRES = BPU3 = ECSENA = 0.
- BPLCON1 <- 0, BPLCON2 <- 0, BPLCON3 <- 0x0C00, BPLCON4 <- 0x0011. The last
  two are the reset values p.16 prints: PF2OF1 = PF2OF0 = 1, and
  ESPRM4 = OSPRM4 = 1.
- FMODE <- 0x0000: "0 0 by 2 bytes (as before) normal CAS 16" (p.23).
- DIWSTRT <- 0x4081, DIWSTOP <- 0xF0C1. DIWHIGH is not written.
- COLOR00 <- 0x0000.
- DDFSTRT, DDFSTOP, BPL1MOD and BPL2MOD as the proposition states (the
  modulos are 0 unless stated).
- Copper list L-A, started as in 2.1. Then DMACON <- 0x8380 (SET, DMAEN,
  BPLEN, COPEN). Sprite, blitter, disk and audio DMA stay off.

**S-WIN, the window instrument** (P6, P8-P15):

- BEAMCON0 <- 0x0020 (PAL = 1, all other bits 0), as in S-FETCH, as the
  program's first custom-register write after the three clearing writes of
  2.1. The window propositions need a PAL field: they observe lines up to
  0x12F, P11, P12, P14 and P15 grade window lines beyond 0x100, and the
  "never reached" arguments of P8, P13 and P15 count on a field that ends
  between 0x12F and 0x140. The bit that selects it is "PAL= Set appropriate
  decodes (in normal mode) for PAL." (p.12), and the AA pages give it no
  reset value. RF-L, which P6 uses, rests on the PAL bit as well (2.4).
  HARDDIS = 0 is its reset value ("It is cleared upon reset", p.12).
  VARVBEN = 0 and
  VARBEAMEN = 0 keep the hard stops of the display window, which either bit
  disables when set ("It also disables the hard stop on the vertical display
  window"; "It also disables hard display stops on both horizontal &
  vertical", p.12).
- BPLCON0 <- 0x1001: one plane, ECSENA = 1. Then BPLCON1 <- 0,
  BPLCON2 <- 0, BPLCON3 <- 0x0C20 (BRDRBLNK = 1, PF2OF1 = PF2OF0 = 1),
  BPLCON4 <- 0x0011, FMODE <- 0. BPLCON0 is written before BPLCON3, so that
  ECSENA is already high when BRDRBLNK is written. The pages describe the
  gate three ways: ECSENA "forces" BRDRBLNK low, and "When ECSENA is set high
  again, the former settings for these bits are restored" (p.4); the bits
  "can always be set by writing to BPLCON3, however there effects are
  inhibited until ECSENA goes high" (p.17); and BRDRBLNK is marked
  "****RESET BY ECSENA*****" (p.18). This order is correct under each.
- COLOR00 <- 0x0F00 and COLOR01 <- 0x0F00. With LOCT = 0 and BANK = 0 the
  4-bit values are extended to 8 bits (p.19).
- DDFSTRT <- 0x0038, DDFSTOP <- 0x00D0, BPL1MOD <- 0.
- DIWSTRT, DIWSTOP and DIWHIGH as the proposition states.
- Copper list L-V (P6: L-A with the marker block). Then DMACON <- 0x8380.

Why the window becomes visible: "BRDRBLNK bit in BPLCON3 allows the
background to be BLACK and not the contents of COLOR00." (p.4); "BRDRBLNK=
"border area" is blanked instead of color(0)." (p.18); and the bit takes
effect only with ECSENA set: "ECSENA= When low(default), the following bits
in BPLCON3 are disabled: BRDRBLNK,... however there effects are inhibited
until ECSENA goes high." (p.17). The border is therefore black. Inside the
window, colour index 0 and colour index 1 are both red. The window
instrument shows the lines of the display window on the video output. No page
says what the display window shows when no bit-plane data is fetched (BPU = 0:
"Shows only a background color; no playfield is visible", HRM p.48), so the
instrument is used only where bit-plane DMA runs, and P6 does not grade
against it.

### 2.6 How differences are graded

Where a proposition compares configurations, it is graded on every line
that the compared configurations share: every fetched line (RF-L) or every
line in the stated range (RF-V). A single line never stands for "every".
Where a page does not state an absolute value, only differences between
configurations are graded, and the proposition says so.

### 2.7 Claims that name both registers of a pair

The page describes DDFSTRT and DDFSTOP together ("These registers control
the horizontal timing of the beginning and end of the Bit Plane DMA display
data fetch.", and one USE line for both, p.21), and DIWSTRT and DIWSTOP
together (one USE line for both, p.22). A claim taken from such a line names
both registers, and a proposition about one of them grades only that
register's half of it. No single-register proposition therefore carries
these claims. Each is carried by a joint assertion, which holds exactly when
both of its propositions hold (item 4 of each, under the preconditions of
each), and which is graded only when both are:

- **J1 = P1 and P2** (page 21): opus:AA-ddf.c5, opus:AA-ddf.use-b03,
  opus:AA-ddf.use-b04, opus:AA-ddf.use-b05, opus:AA-ddf.use-b06,
  opus:AA-ddf.use-b07.
- **J2 = P8 and P9** (page 22): opus:AA-diw.use-b08, opus:AA-diw.use-b09,
  opus:AA-diw.use-b10, opus:AA-diw.use-b11, opus:AA-diw.use-b12,
  opus:AA-diw.use-b13, opus:AA-diw.use-b14.
- **J3 = P10 and P11** (page 22): opus:AA-diw.use-b15.

A joint assertion's contrasts are those of its two propositions.

---

## 3 Propositions

### P1. DDFSTRT sets the left edge of the fetch; its bits H4-H8 weigh 8, 16, 32, 64 and 128 memory cycles

Claims: sol:AA-ddf.c1, opus:AA-ddfstrt.c9. Page 21. The claims that name
DDFSTRT and DDFSTOP together are carried by J1 (2.7).

1. **Proposition.** In lores with FMODE = 0, changing DDFSTRT from a to b
   (H3 = H2 = 0 in both) moves the first bit-plane fetch of every fetched
   line by exactly b - a memory cycles, and leaves the last fetch where it
   was.
2. **Stimulus.** S-FETCH, BPLCON0 = 0x1000, DDFSTOP = 0x00D0. Seven
   configurations: DDFSTRT = 0x0028, 0x0030, 0x0038, 0x0040, 0x0060, 0x0068,
   0x00B8. Each position bit is changed alone in at least one pair: H4 in
   0x0030/0x0038, H5 in 0x0028/0x0038, H6 in 0x0040/0x0060, H7 in
   0x0028/0x0068, H8 in 0x0038/0x00B8. Bit 0 and bits 15-8 are 0.
3. **Observation.** Reads in B1. For each line v (RF-L) that has at least
   one read in [A(v), A(v+1)): f(v) is the time of the earliest such read
   minus A(v), and l(v) is the time of the latest such read minus A(v).
   The observation covers the configuration's observed field, lines 0x3E to
   0xF1.
4. **Expected.** Within one configuration c, f(v) = f_c and l(v) = l_c on
   every fetched line. For any two configurations a and b:
   f_b - f_a = (b - a) x P, and l_b = l_a. Sources: "DDFSTRT 092 W A Display
   data fetch start(Horiz.Position)"; "These registers control the
   horizontal timing of the beginning and end of the Bit Plane DMA display
   data fetch."; "The tables below show the start and stop timing for
   different register contents." (all p.21). The weights come from 2.4. The
   end of the fetch belongs to DDFSTOP ("DDFSTOP(Right edge of display data
   fetch)", p.21), so a change of DDFSTRT alone does not move it.
   The page gives no fixed offset between the programmed position and the
   first fetch, and none is graded. The proposition is restricted to lores,
   FMODE = 0, and values with H3 = H2 = 0, for two reasons. First, the page
   says nothing about fetch granularity. Second, for the original chips the
   manual it builds on makes the effect of the low position bit depend on
   the resolution: "bit H3 only respected in Hires Mode." (HRM p.91).
5. **Preconditions.**
   - (i) The RF-M and RF-L checks hold in every configuration.
   - (ii) The set of fetched lines is the same in all seven configurations
     and is not empty.
   - (iii) Within every fetched line, consecutive B1 reads are at
     consecutive word addresses, "by 2 bytes (as before)" (p.23), which
     shows that FMODE = 0 is in force.
   - (iv) Every B1 read lies within [A(0x3E), A(0xF2)).
   - (v) The stored DMACONR shows DMAEN, BPLEN and COPEN set.
6. **Contrasting case.** Each configuration is the others' contrast. For
   example, 0x0038 against 0x00B8 predicts +128 memory cycles; an
   implementation that ignored H8 would show 0. Contrasts for the
   preconditions:
   - (ii) DIWSTRT = 0x5081 in one configuration changes the set of fetched
     lines (P6 predicts this), and (ii) fails.
   - (iii) FMODE = 0x0001 (BPL32) gives "by 4 bytes" (p.23), and (iii)
     fails.
   - (iv) DIWSTRT = 0x3D81 in all seven configurations puts reads on line
     0x3D, before A(0x3E), and (iv) fails. Line 0x3C, which RF-M needs free
     of bit-plane DMA (2.4), stays free, and the set of fetched lines of
     RF-L, which starts at 0x3E, stays the same, so (i) and (ii) still hold.
   - (v) The DMACONR copy is read and stored before the write
     DMACON <- 0x8380 instead of after it. After DMACON <- 0x7FFF (2.1) it
     shows none of DMAEN, BPLEN and COPEN, while the fetch is unchanged, and
     (v) fails alone. (DMACON <- 0x8280, without BPLEN, would clear bit 8
     too, but it would also empty the fetch and fail (ii).)
   - (i) See the contrasts in 2.4.
7. **Reference frame.** RF-M and RF-L.

### P2. DDFSTOP sets the right edge of the fetch; its bits H4-H8 weigh 8, 16, 32, 64 and 128 memory cycles

Claims: sol:AA-ddf.c2, opus:AA-ddfstop.c9. Page 21. The claims that name
DDFSTRT and DDFSTOP together are carried by J1 (2.7).

1. **Proposition.** In lores with FMODE = 0, changing DDFSTOP from c to d
   (H3 = H2 = 0 in both) moves the last bit-plane fetch of every fetched
   line by exactly d - c memory cycles, and leaves the first fetch where it
   was.
2. **Stimulus.** S-FETCH, BPLCON0 = 0x1000, DDFSTRT = 0x0038. Six
   configurations: DDFSTOP = 0x0058, 0x0098, 0x00B8, 0x00C8, 0x00D0, 0x00D8.
   Single-bit pairs: H4 in 0x00D0/0x00D8, H5 in 0x00C8/0x00D8, H6 in
   0x0098/0x00B8, H7 in 0x0098/0x00D8, H8 in 0x0058/0x00D8. No value lies
   beyond the table's "wide (max)" row.
3. **Observation.** As in P1: f(v) and l(v) per fetched line.
4. **Expected.** Within one configuration, f and l are constant over all
   fetched lines. For any two configurations: l_d - l_c = (d - c) x P, and
   f_d = f_c. Sources: "DDFSTOP 094 W A Display data fetch
   stop(Horiz.Position)"; "These registers control the horizontal timing of
   the beginning and end of the Bit Plane DMA display data fetch." (p.21).
   Weights from 2.4. No absolute offset is graded; restrictions as in P1.
5. **Preconditions.** (i)-(v) of P1, for these six configurations.
6. **Contrasting case.** The configurations themselves. An implementation
   that ignored H8 would give the same last fetch for 0x0058 and 0x00D8,
   where +128 memory cycles is predicted. Precondition contrasts as in P1.
7. **Reference frame.** RF-M and RF-L.

### P3. The DDFSTRT table rows: extra wide, wide, normal, narrow

Claims: opus:AA-ddfstrt.t2, opus:AA-ddfstrt.t3, opus:AA-ddfstrt.t4,
sol:AA-ddfstrt.c6, sol:AA-ddfstrt.c7, sol:AA-ddfstrt.c8. Page 21. The
extra-wide row's claims (opus:AA-ddfstrt.t1, sol:AA-ddfstrt.c5) are declined
(section 1).

1. **Proposition.** The four DDFSTRT rows of the table place the left edge
   of the fetch in the order extra wide, wide, normal, narrow, each exactly
   8 memory cycles after the one before.
2. **Stimulus.** S-FETCH, BPLCON0 = 0x1000, DDFSTOP = 0x00D0. DDFSTRT is set
   to the table's H8-H4 bits, with H3 = H2 = 0 and all X bits 0:
   - "Extra wide (max) * 0 0 1 0 1" gives 0x0028;
   - "wide 0 0 1 1 0" gives 0x0030;
   - "normal 0 0 1 1 1" gives 0x0038;
   - "narrow 0 1 0 0 0" gives 0x0040.

   The table gives only H8-H4. Writing 0 to H3 and H2 is this package's
   choice; the page states nothing for them. These four configurations are
   also configurations of P1, and they may be the same fields.
3. **Observation.** f_c and l_c as in P1.
4. **Expected.** f(0x0030) - f(0x0028) = f(0x0038) - f(0x0030) =
   f(0x0040) - f(0x0038) = 8 x P. l is equal in all four. The order follows
   the table heading "DDFSTRT(Left edge of display data fetch)" and the
   labels: a wider fetch has an earlier left edge. The spacing is the
   weight of H4 (2.4), because consecutive rows differ by one H4 step.
   The extra-wide row enters as the row against which the wide row is
   placed. Its label "(max)" states a limit that this proposition does not
   grade, so its claims are declined (section 1), and no value below it is
   used.
5. **Preconditions.** (i)-(v) of P1.
6. **Contrasting case.** Writing the narrow row's bits (0x0040) in place of
   the normal row's predicts a left edge 8 memory cycles later. An
   implementation that ignored H7 would read 0x0040 as 0x0000 and start
   earlier than the extra-wide row, so the check fails. Precondition
   contrasts as in P1.
7. **Reference frame.** RF-M and RF-L.

### P4. The DDFSTOP table rows: narrow, normal, wide

Claims: opus:AA-ddfstop.t1, opus:AA-ddfstop.t2, sol:AA-ddfstop.c5,
sol:AA-ddfstop.c6. Page 21. The wide row's claims (opus:AA-ddfstop.t3,
sol:AA-ddfstop.c7) are declined (section 1).

1. **Proposition.** The three DDFSTOP rows of the table place the right
   edge of the fetch in the order narrow, normal, wide, each exactly 8
   memory cycles after the one before.
2. **Stimulus.** S-FETCH, BPLCON0 = 0x1000, DDFSTRT = 0x0038. DDFSTOP is
   set to the table's bits, with H3 = H2 = 0:
   - "narrow 1 1 0 0 1" gives 0x00C8;
   - "normal 1 1 0 1 0" gives 0x00D0;
   - "wide (max) 1 1 0 1 1" gives 0x00D8.

   These are also configurations of P2.
3. **Observation.** f_c and l_c as in P1.
4. **Expected.** l(0x00D0) - l(0x00C8) = l(0x00D8) - l(0x00D0) = 8 x P,
   and f is equal in all three. Source: the heading "DDFSTOP(Right edge of
   display data fetch)" with its row labels, and the weight of H4 (2.4).
   The wide row enters as the row against which the normal row is placed.
   Its label "(max)" states a limit that this proposition does not grade, so
   its claims are declined (section 1), and no value above it is used.
5. **Preconditions.** (i)-(v) of P1.
6. **Contrasting case.** The narrow row predicts a right edge 16 memory
   cycles before the wide row. An implementation that ignored H4 would read
   0xC8 as 0xC0 and both 0xD0 and 0xD8 as 0xD0. The normal and wide edges
   would then coincide, and the 8 x P spacing check fails. Precondition
   contrasts as in P1.
7. **Reference frame.** RF-M and RF-L.

### P5. Hires, bit 2 of DDFSTRT and DDFSTOP (H3): measured, not graded

Record: opus:AA-ddf.use-b02 (declined, section 1). Page 21. P5 asserts
nothing about bit 2. It measures the hires configurations below and prints
the result as MEASURED lines. The only graded line of this run that is not
a precondition is P5-ctl.

1. **Record.** In hires with FMODE = 0, the first and the last bit-plane
   fetch of every fetched line are measured for configurations that differ
   in bit 2 (H3) of DDFSTRT or of DDFSTOP, and the differences are printed.
2. **Stimulus.** S-FETCH with seven configurations:
   - L0: BPLCON0 = 0x1000 (lores), DDFSTRT = 0x0038, DDFSTOP = 0x00D0.
   - H1: BPLCON0 = 0x9000 (HIRES = 1, one plane), DDFSTRT = 0x0038,
     DDFSTOP = 0x00D4.
   - H2: BPLCON0 = 0x9000, DDFSTRT = 0x003C, DDFSTOP = 0x00D4.
   - H3: BPLCON0 = 0x9000, DDFSTRT = 0x003C, DDFSTOP = 0x00D0.
   - C1: BPLCON0 = 0x9000, DDFSTRT = 0x0030, DDFSTOP = 0x00D0.
   - C2: BPLCON0 = 0x9000, DDFSTRT = 0x0038, DDFSTOP = 0x00D0.
   - C3: BPLCON0 = 0x9000, DDFSTRT = 0x0038, DDFSTOP = 0x00C8.
3. **Observation.** f_c, l_c and n_c as in P1, for every configuration.
4. **Recorded.** The MEASURED line "P5" prints f_c and l_c per
   configuration and whether f(H2) - f(H1) = 4 x P with l(H2) = l(H1) (the
   DDFSTRT half), and whether l(H2) - l(H3) = 4 x P with f(H2) = f(H3) (the
   DDFSTOP half). The MEASURED line "P5-last" prints l(H1) and l(H2). The
   lines state why nothing is graded: AA p.21 prints bit 2 as H3, HRM p.91
   says H3 is respected in hires and HRM p.60 gives the hires relation
   "DDFSTRT = DDFSTOP - (4 * (word count - 2))", while HRM p.195 Figure 6-9
   says "Data fetch start can only be specified at even multiples of 8
   clocks" and HRM p.197 Figure 6-12 draws the hires fetch in aligned
   blocks, so a DDFSTRT change across a block boundary may move the last
   fetch too; HRM p.80's "only one word is fetched at this limit" is the
   statement about the `$D8` limit, and H1 and H2 use DDFSTOP = `$D4`. The
   pages therefore give no exact fetch-edge or word-count oracle for bit 2,
   and neither measured half is a verdict on the implementation.
5. **Control, graded.** P5-ctl: with bit 2 clear in both registers, in
   hires, bit 3 (H4) weighs 8 memory cycles: f(C2) - f(C1) = 8 x P and
   l(C2) = l(C1) (DDFSTRT 0x0038 against 0x0030), and l(C2) - l(C3) = 8 x P
   and f(C2) = f(C3) (DDFSTOP 0x00D0 against 0x00C8). This is the weight of
   H4 from 2.4 applied in hires; the line carries no claim id.
6. **Preconditions** (they gate P5-ctl).
   - (i)-(v) of P1, for H1, H2, H3, C1, C2 and C3.
   - (vi) Hires is in force: n in each hires configuration is greater than
     n(L0). The mode doubles the pixels per line, "HIRES=High
     resolution(640*200/640*400interlace) mode" (p.16), so it needs more
     data per line.
7. **Contrasting case.** P5-ctl: C1 writes DDFSTRT 0x0038, the value of C2,
   and the f difference of 8 x P fails. Contrast for (vi): BPLCON0 = 0x1000
   in the hires configurations gives n equal to the lores count, and (vi)
   fails.
8. **Reference frame.** RF-M and RF-L.

### P6. Bit-plane DMA runs on the lines of the display window the registers give

Claims: none asserted. opus:AA-ddf.c6 and sol:AA-ddf.c3 are declined
(section 1). Page 21.

1. **Proposition.** The set of beam lines on which bit-plane DMA fetches
   starts at the line DIWSTRT bits 15-8 name, and its last line moves with
   DIWSTOP bits 15-8 as P9 states for the window.
2. **Stimulus.** S-WIN with list L-A including the marker block (the
   pointer MOVEs load Z). Two configurations:
   - W1: DIWSTRT = 0x4081, DIWSTOP = 0xF0C1.
   - W2: DIWSTRT = 0x5081, DIWSTOP = 0xA0C1.

   DIWHIGH is not written.
3. **Observation.** Fetch is the set of lines v (RF-L) with at least one
   read in Z inside [A(v), A(v+1)), over the configuration's observed field.
   Win (RF-V) is measured in the same field and printed, and not graded.
4. **Expected.** Fetch is one contiguous run of lines in W1 and in W2; its
   first line is 0x40 in W1 and 0x50 in W2 (DIWSTRT bits 15-8, as in P8); and
   last(Fetch, W1) - last(Fetch, W2) = 0xF0 - 0xA0 = 0x50 lines (DIWSTOP bits
   15-8, as in P9, whose rule grades differences only). Context: "The
   vertical Bit Plane DMA timing is identical to the Display windows
   described above." (p.21). "Above" refers to DIWSTRT and DIWSTOP, whose
   description in this document follows on p.21-p.22. P6 grades the fetched
   lines against the registers and not against the window instrument: on
   this design the instrument follows the fetch (it shows no window with
   BPU = 0, 5.5), so a comparison with it would compare two sets that both
   come from the fetch.

   What P6 grades is limited. A contiguous run, two exact first lines and
   the difference of the two last lines do not establish that the DMA line
   set equals the display window line set: the same constant displacement of
   both last lines would pass. For that reason P6 carries neither claim id.
   P8 and P9 grade the display window as the video output shows it; P6
   grades the bit-plane DMA lines against the registers.
5. **Preconditions.**
   - (i) The RF-L checks hold.
   - (ii) Fetch is non-empty, and every Z read lies within
     [A(0x3E), A(0xF2)).
   - (iii) The stored DMACONR shows DMAEN, BPLEN and COPEN.
   - (iv) The window the registers give lies inside 0x3E-0xF1: in both
     configurations DIWSTRT bits 15-8 are at least 0x3E, and DIWSTOP bits 15-8
     lie in 0x80-0xF1 (so that V8 = not V7 puts the stop inside the
     observation).
6. **Contrasting case.** The pointer and the observed region stay as they
   are and a page-defined window position changes: W2 writes
   DIWSTOP = 0xB0C1 in place of 0xA0C1, while the comparison keeps the
   0xA0C1 window. DIWSTOP bits 15-8 weigh one line per unit (P9), so the DMA
   lines of W2 end 0x10 lines later, the last-line difference is 0x40 where
   the comparison expects 0x50, and P6 fails through the unchanged
   comparator. An implementation that fetched a fixed range of lines, or
   followed only one of the two registers, would fail the first-line or the
   last-line difference in the same way. Contrasts for the preconditions:
   - (ii): BPLCON0 <- 0x0001 (BPU = 000, ECSENA = 1) empties Fetch: "BPUx
     =Bit plane use code 0000-1000 (NONE thru 8 inclusive)" (p.16).
   - (iii): the DMACONR copy stored before the DMACON write, as in P1.
   - (iv): DIWSTOP = 0x10C1 puts the stop line past 0xF1.
7. **Reference frame.** RF-L for the fetch lines. It anchors on the Copper's
   WAIT positions (p.20).

### P7. The bit-plane pointer advances by the fetched bytes plus the modulo, so the modulo depends on the plane's size and on the fetch window

Claims: opus:AA-ddf.c7, sol:AA-ddf.c4, sol:AA-ddf.c5. Page 21, with p.6,
p.16 and p.23.

1. **Proposition.** For each plane, the first fetch address of each fetched
   line equals the address after the last fetch of the previous fetched
   line plus that plane's modulo. So for a plane of row size W, the modulo
   that keeps the rows aligned is W minus the bytes fetched per line. That
   amount changes with the plane's size and with the fetch window.
2. **Stimulus.** S-FETCH, BPLCON0 = 0x2000 (BPU = 010, two planes: plane 1
   odd, plane 2 even), DDFSTRT = 0x0038. L-A also loads BPL2PT with B2.
   Three configurations:
   - M-A: DDFSTOP = 0x00D0, BPL1MOD = 0x0008, BPL2MOD = 0x0018.
   - M-B: DDFSTOP = 0x00D8, BPL1MOD = 0x0008, BPL2MOD = 0x0018.
   - M-C: DDFSTOP = 0x00D0, BPL1MOD = 0x0018, BPL2MOD = 0x0008.
3. **Observation.** For plane p (region B1 or B2) and fetched line v:
   s_p(v) is the address of the first read, e_p(v) the address of the last
   read, and n_p(v) the number of reads. The observation covers all pairs
   of consecutive fetched lines in the observed field.
4. **Expected.**
   - For every consecutive pair (v, v+1) and each plane:
     s_p(v+1) = e_p(v) + 2 + MOD_p, where MOD_1 = BPL1MOD and
     MOD_2 = BPL2MOD. Equivalently, the row pitch
     s_p(v+1) - s_p(v) = 2 x n_p(v) + MOD_p.
   - In M-A, pitch_1 - pitch_2 = 0x0008 - 0x0018 = -16 bytes.
   - M-B against M-A: pitch_p rises by exactly 2 x (n_p(M-B) - n_p(M-A)).
   - M-C against M-A: the two planes' pitches swap.

   Sources: "The Bit Plane Modulos are dependent on the Bit Plane
   horizontal size, and on this data fetch window size." (p.21); "A Modulo
   is a number that is automatically added to the address at the end of
   each line, in order that the address then points to the start of the
   next line." (p.16); "BPL1MOD ... (odd planes)", "BPL2MOD ... (even
   planes)" (p.16); "0 0 by 2 bytes (as before)" (p.23). The page does not
   state the number of words fetched per line for a DDF setting, so it is
   measured (n_p), not predicted.
5. **Preconditions.**
   - (i) The RF-L checks hold.
   - (ii) Within each fetched line, consecutive reads of a plane are 2
     bytes apart.
   - (iii) n_1(v) = n_2(v) on every line, and n is the same on every line
     of a configuration.
   - (iv) n(M-B) > n(M-A): the "wide" right edge fetches more than the
     "normal" one.
   - (v) The set of fetched lines is the same in all three configurations
     and is not empty.
6. **Contrasting case.** M-B (larger fetch window, same modulos) predicts a
   larger pitch. M-C (modulos swapped) predicts swapped pitches. An
   implementation that added the modulo of the wrong plane, or ignored it,
   fails the pitch relation. Contrasts for the preconditions:
   - (ii): FMODE = 0x0001 predicts 4-byte steps (p.23).
   - (iv): DDFSTOP = 0x00D0 in M-B makes n equal.
   - (v): DIWSTRT = 0x5081 in one configuration changes the fetched lines.
7. **Reference frame.** RF-L.

### P8. Bits 15-8 of DIWSTRT are the vertical start V7-V0: bits 8-14

Claims: none of its own. Page 22. Its bits are named by claims that cover
DIWSTRT and DIWSTOP together (opus:AA-diw.use-b08 to opus:AA-diw.use-b14);
they are carried by J2 (2.7).

1. **Proposition.** With DIWHIGH never written, the first line of the
   display window is the beam line whose number equals DIWSTRT bits 15-8.
   A change in bit 8+k moves it by exactly 2^k lines.
2. **Stimulus.** S-WIN, list L-V, DIWSTOP = 0xF0C1, DIWHIGH never written.
   DIWSTRT bits 15-8 take the values 0x40, 0x41, 0x42, 0x44, 0x48, 0x50,
   0x60, 0x30 and 0x70, with bits 7-0 = 0x81. Each of bits 8-13 is changed
   alone against 0x40. Bit 14 is changed alone between 0x30 and 0x70. All
   these values have V7 = 0; bit 15 is P10's.
3. **Observation.** Win (RF-V) in each configuration's observed field,
   lines 0x26-0x12F.
4. **Expected.**
   - The first line of Win equals DIWSTRT bits 15-8 in every configuration.
     Each single-bit pair therefore differs by 2^k lines.
   - The last line of Win is the same in all configurations.
   - Win is one contiguous run of lines.

   Sources: "These registers control the Display window size and position,
   by locating the upper left and lower right corners." and "USE V7 V6 V5
   V4 V3 V2 V1 V0 H9 H8 H7 H6 H5 H4 H3 H2" (p.22). The line number includes
   V8 = 0, which P10 grades on its own: "DIWSTRT is vertically restricted
   to the upper 2/3 of the display (V8=0)" (p.22).
5. **Preconditions.**
   - (i) The RF-V checks hold.
   - (ii) DIWHIGH has not been written since reset: "If it is not written,
     the above (DIWSTRT,STOP) description holds." (p.22). The reset pin
     clears DIWHIGH: "A RST_ input pin has been added, which resets all the
     bits contained in registers that were new for ECS or LISA:
     BPLCON3,BPLCON4,CLXCON2,DIWHIGH,FMODE" (p.2).
6. **Contrasting case.** The single-bit pairs. Contrast for (ii): a run
   that writes DIWHIGH <- 0x2001 after DIWSTRT = 0x4081 (start V8 = 1). The
   page predicts a start at line 0x140, which a PAL field does not reach, so
   no window line appears where the base run has lines 0x40-0xEF. The
   contrast for (i) is in 2.4.
7. **Reference frame.** RF-V.

### P9. Bits 15-8 of DIWSTOP are the vertical stop V7-V0: bits 8-14

Claims: none of its own. Page 22. Its bits are named by claims that cover
DIWSTRT and DIWSTOP together (opus:AA-diw.use-b08 to opus:AA-diw.use-b14);
they are carried by J2 (2.7).

1. **Proposition.** With DIWHIGH never written, a change in DIWSTOP bit 8+k
   (k = 0 ... 6) moves the last line of the display window by exactly 2^k
   lines.
2. **Stimulus.** S-WIN, list L-V, DIWSTRT = 0x4081, DIWHIGH never written.
   DIWSTOP bits 15-8 take the values 0xB0, 0xB1, 0xB2, 0xB4, 0xB8, 0xA0,
   0x90 and 0xF0, with bits 7-0 = 0xC1. Each differs from 0xB0 in one of
   bits 8-14. All have V7 = 1; bit 15 is P11's.
3. **Observation.** Win (RF-V), lines 0x26-0x12F.
4. **Expected.** last(Win, x) - last(Win, 0xB0) = x - 0xB0 lines, for every
   x: +1, +2, +4, +8, -16, -32 and +64. The first line of Win is 0x40
   everywhere. Source: the USE line and "lower right corners" (p.22). The
   page does not say whether the line numbered VSTOP is the last line
   inside the window or the first line outside it. So only differences are
   graded, and the absolute last line is not.
5. **Preconditions.** (i) and (ii) of P8.
6. **Contrasting case.** The single-bit pairs. Contrast for (ii): writing
   DIWHIGH <- 0x2100 after DIWSTRT and DIWSTOP = 0xB0C1 (stop V8 = 1), as the
   last configuration of its run. The page predicts a stop at 0x1B0, which
   is not reached, so every line from 0x40 to 0x120 is in the window where
   the base run ends near 0xB0.
7. **Reference frame.** RF-V.

### P10. Without DIWHIGH, the vertical start has V8 = 0 whatever V7 is

Claims: opus:AA-diwstrt.c5, sol:AA-diwstrt.c5. Page 22. Bit 15 of both
registers (opus:AA-diw.use-b15) is carried by J3 (2.7).

1. **Proposition.** With DIWHIGH never written, the display window starts
   at line 0x040 + V6-V0 when V7 = 0, and at line 0x0C0 + V6-V0 when
   V7 = 1. V8 is 0 in both cases, and bit 15 carries V7.
2. **Stimulus.** S-WIN, list L-V, DIWSTOP = 0xF0C1, DIWHIGH never written.
   Two configurations: DIWSTRT = 0x4081 and DIWSTRT = 0xC081 (only bit 15
   differs).
3. **Observation.** Win (RF-V), lines 0x26-0x12F.
4. **Expected.** The first line of Win is 0x40 in one configuration and 0xC0
   in the other, exactly. The last line is the same in both. Sources:
   "DIWSTRT is vertically restricted to the upper 2/3 of the display (V8=0)"
   and the asterisk's footnote "* Poof.. ( See DIWHIGH for exceptions)"
   (p.22). The fraction "2/3" is graded only as the bit rule V8 = 0, which
   the page states for every value.
5. **Preconditions.** (i) and (ii) of P8.
6. **Contrasting case.** 0x4081 against 0xC081. If the stop register's rule
   (V8 = not V7) were applied to the start, 0x4081 would start at 0x140 and
   show no window. If V8 = V7, 0xC081 would start at 0x1C0 and show no
   window. Contrast for (ii): DIWHIGH <- 0x2001 written last with
   DIWSTRT = 0x4081 gives no window line (P8, item 6).
7. **Reference frame.** RF-V.

### P11. Without DIWHIGH, the vertical stop has V8 = not V7

Claims: opus:AA-diwstop.c5, sol:AA-diwstop.c5. Page 22. Bit 15 of both
registers (opus:AA-diw.use-b15) is carried by J3 (2.7).

1. **Proposition.** With DIWHIGH never written, clearing V7 in DIWSTOP puts
   V8 = 1. So DIWSTOP bits 15-8 = 0x10 stops the window 0x80 lines later
   than 0x90 does: 0x110 against 0x090.
2. **Stimulus.** S-WIN, list L-V, DIWSTRT = 0x4081, DIWHIGH never written.
   Two configurations: DIWSTOP = 0x90C1 and DIWSTOP = 0x10C1 (only bit 15
   differs).
3. **Observation.** Win (RF-V), lines 0x26-0x12F.
4. **Expected.** last(Win, 0x10C1) - last(Win, 0x90C1) = +0x80 lines
   exactly, and the first line is 0x40 in both. So the 0x10C1 window
   includes lines 0x100 and beyond. Source: "DIWSTOP is vertically
   restricted to the lower 1/2 of the display (V8=/=V7)" (p.22). The
   fraction "1/2" is graded only as the bit rule. Only the difference is
   graded (see P9, item 4).
5. **Preconditions.** (i) and (ii) of P8.
6. **Contrasting case.** 0x90C1 against 0x10C1. If V8 were always 0, 0x10C1
   would stop at line 0x010, before the start, and the window would not run
   from 0x40 to about 0x110; the difference check fails. Contrast for (ii):
   DIWHIGH <- 0x2100 written last with DIWSTOP = 0x90C1, as the last
   configuration of its run, gives a stop at 0x190, which is not reached.
   Every line 0x40-0x120 is then in the window, where the base run ends
   near 0x90.
7. **Reference frame.** RF-V.

### P12. DIWHIGH bit 0 is V8 of the vertical start

Claim: opus:AA-diwhigh.bit-b00. Page 22.

1. **Proposition.** When DIWHIGH is written after DIWSTRT and DIWSTOP,
   setting its bit 0 moves the window's first line from 0x028 to 0x128.
2. **Stimulus.** S-WIN, list L-V. In each vertical blank the program writes,
   in this order, DIWSTRT = 0x2881, DIWSTOP = 0x2AC1, then DIWHIGH. Two
   configurations: DIWHIGH = 0x2100 (base: stop V8 = 1, stop H10 = 1) and
   DIWHIGH = 0x2101 (start V8 also set). The horizontal bits keep the usual
   window: start H10 = 0, and stop H10 = 1, which the stop register implies
   when DIWHIGH is not written ("horizontally restricted to the right 1/4
   of the display(H8=1)", p.22). H1, H0 and both V10 bits are 0.
3. **Observation.** Win (RF-V), lines 0x26-0x12F.
4. **Expected.** First line of Win: 0x28 in the base configuration and
   0x128 in the other, exactly. The last line is the same in both. Sources:
   "If this register is written last in a sequence of setting the display
   window, it sets direct start & stop positions anywhere on the screen."
   and the bit row "x x H10 H1 H0 V10 V9 V8 x x H10 H1 H0 V10 V9 V8" under
   "(stop) | (start)" (p.22).
5. **Preconditions.**
   - (i) The RF-V checks hold.
   - (ii) DIWHIGH is written after DIWSTRT and DIWSTOP in the same vertical
     blank.
   - (iii) Both windows are non-empty.
6. **Contrasting case.** The base configuration. Contrast for (ii): a
   configuration that writes DIWHIGH = 0x2101 before DIWSTRT and DIWSTOP.
   The page then predicts a first line of 0x28, not 0x128, because
   "DIWHIGH is reset by writes to DIWSTRT or DIWSTOP." (p.4) and because the
   register was not written last (p.22). Only the first line is graded in
   this contrast: the pages do not say whether the reset brings back the
   not-written rule for the stop or leaves a zero stop V8 (section 4).
7. **Reference frame.** RF-V.

### P13. With DIWHIGH bit 1 set, a start at line 0x228 leaves a PAL field without a window

Claims: none asserted. opus:AA-diwhigh.bit-b01 is declined (section 1).
Page 22.

1. **Proposition.** When DIWHIGH is written last, setting its bit 1 puts
   the vertical start at 0x228, a line a PAL field never reaches, so no
   window line appears.
2. **Stimulus.** As P12. Base DIWHIGH = 0x2100; varied DIWHIGH = 0x2102
   (start V9 = 1, start V8 = 0). The base configuration comes first.
3. **Observation.** Win (RF-V), lines 0x1D to the end of the field.
4. **Expected.** Base: Win runs from 0x28 to about 0x12A. Varied: Win is
   empty. Sources as in P12. V9 is the start position's bit of weight
   0x200. The start line 0x228 lies beyond the last line of a PAL field, so
   the upper-left corner is never reached. This is a graded consequence of
   the page. It does not establish that bit 1 is V9: a missing window shows
   only that the effective start is out of range, and a mapping of bit 1 to
   V10, to a larger position or to another disabling condition would give
   the same observation. The line therefore carries no claim id.
5. **Preconditions.** (i)-(ii) of P12. (iii) The base window is non-empty.
6. **Contrasting case.** The base configuration. An implementation that
   ignored V9 opens the window at 0x28. One that took bit 1 as V8 opens it
   at 0x128. Both fail. Contrast for (ii) as in P12.
7. **Reference frame.** RF-V.

### P14. DIWHIGH bit 8 is V8 of the vertical stop

Claim: opus:AA-diwhigh.bit-b08. Page 22.

1. **Proposition.** When DIWHIGH is written last, setting its bit 8 moves
   the window's last line exactly 0x100 lines later.
2. **Stimulus.** As P12, with DIWSTRT = 0x2881 and DIWSTOP = 0x2AC1. Two
   configurations: DIWHIGH = 0x2000 (stop 0x02A) and DIWHIGH = 0x2100 (stop
   0x12A).
3. **Observation.** Win (RF-V), lines 0x26-0x12F.
4. **Expected.** last(Win, 0x2100) - last(Win, 0x2000) = 0x100 lines
   exactly, and the first line is 0x28 in both. Only the difference is
   graded (P9, item 4). Sources as in P12.
5. **Preconditions.** (i)-(ii) of P12. (iii) Both windows are non-empty.
6. **Contrasting case.** The pair itself. An implementation that ignored
   bit 8 gives equal last lines. Contrast for (ii) as in P12.
7. **Reference frame.** RF-V.

### P15. With DIWHIGH bits 8 and 9 set, a stop at line 0x310 keeps the window open

Claims: none asserted. opus:AA-diwhigh.bit-b09 is declined (section 1).
Page 22.

1. **Proposition.** When DIWHIGH is written last, with bit 8 set the window
   closes at the stop the registers give, and with bits 8 and 9 set (stop
   0x310) the window stays open past line 0x110, to the end of the graded
   range. With bit 8 set, one step of DIWSTOP bits 15-8 moves the last line
   by exactly one line.
2. **Stimulus.** As P12, with DIWSTRT = 0x2881. Three configurations, in this
   order:
   - A: DIWSTOP = 0x10C1, DIWHIGH = 0x2100 (stop V8 = 1: stop 0x110).
   - B: DIWSTOP = 0x11C1, DIWHIGH = 0x2100 (stop 0x111).
   - V: DIWSTOP = 0x10C1, DIWHIGH = 0x2300 (stop V9 = V8 = 1: 0x310).

   V is the last configuration of its run (2.2).
3. **Observation.** Win (RF-V), lines 0x26-0x120.
4. **Expected.**
   - A: Win starts at line 0x28 and is one contiguous run that ends before
     line 0x120, with last line l_A; lines l_A + 1 through 0x120 are not in
     Win.
   - B: the same shape, with last line l_B = l_A + 1 exactly.
   - V: every line from 0x28 through 0x120 is in Win.

   Sources as in P12. No absolute last line is graded: the pages do not say
   whether the line numbered VSTOP is inside the window or the first line
   outside it (section 4, item 9), so only the difference of the stops is
   graded. Grading stops at line 0x120, clear of the end of the field. The
   pages do not say where a window whose stop is never reached ends, nor
   whether the hardwired limits that BEAMCON0's HARDDIS disables ("This bit
   is used to disable the hardwire vertical and horizontal window limits",
   p.12) cut it off near the end of the field. A window that stays open
   to line 0x120 does not establish that bit 9 is V9: any mapping that puts
   the stop later than 0x120 gives the same observation. The line therefore
   carries no claim id.
5. **Preconditions.**
   - (i)-(ii) of P12.
   - (iii) Before line 0x28 of the V configuration's observed field, no line
     is in Win: the window of the previous configuration had closed. A is
     non-empty.
6. **Contrasting case.** The pair A and B shows that the stop moves with
   DIWSTOP while DIWHIGH is in force. An implementation that ignored bit 9
   closes the window in V as in A. One that took bit 9 as V8 does too,
   because V8 is already set. Contrast for (ii) as in P12.
7. **Reference frame.** RF-V.

### P16. DMACON bit 8 (BPLEN) enables bit-plane DMA

Claim: opus:AA-dmacon.b08. Page 22.

1. **Proposition.** With DMAEN set, bit-plane fetches happen when BPLEN is
   set and do not happen when it is clear.
2. **Stimulus.**
   - Display: BPLCON0 <- 0x4000 (BPU = 100, four planes, lores), BPLCON1 <-
     0, BPLCON2 <- 0, BPLCON3 <- 0x0C00, FMODE <- 0, DIWSTRT <- 0x4081,
     DIWSTOP <- 0xF0C1, DDFSTRT <- 0x0038, DDFSTOP <- 0x00D0, BPL1MOD =
     BPL2MOD <- 0.
   - In every vertical blank the CPU writes BPL1PT-BPL4PT <- B1-B4 (2.1).
   - DMACON <- 0x8300 (SET, DMAEN, BPLEN). In the contrast: DMACON <- 0x8200.
   - Copper, sprite, blitter, disk and audio DMA are off.
3. **Observation.** Reads in B1-B4 during the two fields (RF-F) after the
   DMACON write.
4. **Expected.** With BPLEN = 1, each field has at least one read in each of
   B1, B2, B3 and B4. With BPLEN = 0, no read occurs in B1-B4 in either
   field. Source: "08 BPLEN Bit Plane DMA enable" (p.22).
5. **Preconditions.**
   - (i) The RF-F check holds.
   - (ii) The stored DMACONR shows bit 9 (DMAEN) set, and bit 8 as written.
   - (iii) BPU is not 0.
6. **Contrasting case.** BPLEN = 0 (above). Contrasts for the preconditions:
   - (ii): DMACON <- 0x8100 (BPLEN without DMAEN) predicts no reads, "09
     DMAEN Enable all DMA below" (p.22), and DMACONR bit 9 = 0.
   - (iii): BPLCON0 <- 0x0000 predicts no reads, "BPUx =Bit plane use code
     0000-1000 (NONE thru 8 inclusive)" (p.16).
7. **Reference frame.** RF-F.

### P17. DMACON bit 7 (COPEN) enables Copper DMA

Claim: opus:AA-dmacon.b07. Page 22.

1. **Proposition.** With DMAEN set, the Copper fetches its list when COPEN
   is set and does not when COPEN is clear.
2. **Stimulus.**
   - COP holds MOVE COLOR00 <- 0x0000, then END.
   - The program writes COP1LCH and COP1LCL <- COP, writes COPJMP1, then
     DMACON <- 0x8280 (SET, DMAEN, COPEN), in the order p.21 prescribes
     (2.1). In the contrast the order is the same, with DMACON <- 0x8200.
   - No other DMA is on.
3. **Observation.** Reads in COP during the two fields (RF-F) after the
   DMACON write.
4. **Expected.** With COPEN = 1, at least one read occurs in COP. With
   COPEN = 0, none occurs. Source: "07 COPEN Coprocessor DMA enable"
   (p.22). The page also restarts the list every vertical blank ("COP1LC is
   automatically used at the beginning of each vertical blank time",
   p.21), but that belongs to another claim and is not graded here.
5. **Preconditions.**
   - (i) The RF-F check holds.
   - (ii) The stored DMACONR shows DMAEN set and bit 7 as written.
   - (iii) Nothing other than the Copper reads COP.
6. **Contrasting case.** COPEN = 0 (above). Contrast for (ii): DMACON <-
   0x8080 (COPEN without DMAEN) predicts no reads (p.22 bit 9), and DMACONR
   bit 9 = 0.
7. **Reference frame.** RF-F.

### P18. DMACON bit 6 (BLTEN) enables blitter DMA

Claim: opus:AA-dmacon.b06. Page 22.

1. **Proposition.** With DMAEN set, a started blit reads its source and
   writes its destination when BLTEN is set, and makes no access when BLTEN
   is clear.
2. **Stimulus.**
   - DMACON <- 0x8240 (SET, DMAEN, BLTEN). In the contrast: DMACON <- 0x8200.
   - Then BLTCON0 <- 0x09F0 (USEA = 1, USED = 1, LF7-LF0 = 0xF0),
     BLTCON1 <- 0x0000 (area mode), BLTAFWM <- 0xFFFF, BLTALWM <- 0xFFFF,
     BLTAMOD <- 0, BLTDMOD <- 0, BLTAPT <- BLT_S, BLTDPT <- BLT_D.
   - Last, BLTSIZE <- 0x0410 (height 16 lines, width 16 words; p.14), after
     which the program stores to MARK.
   - The program then waits two fields without looking at the blitter and
     stores an end marker.
3. **Observation.** Reads in BLT_S and writes in BLT_D between the MARK
   store after the BLTSIZE write and the end marker.
4. **Expected.** With BLTEN = 1, at least one read in BLT_S and at least one
   write in BLT_D occur. With BLTEN = 0, none occurs. Source: "06 BLTEN
   Blitter DMA enable" (p.22).
5. **Preconditions.**
   - (i) The stored DMACONR shows DMAEN set and bit 6 as written.
   - (ii) No other agent accesses BLT_S or BLT_D.
6. **Contrasting case.** BLTEN = 0 (above). Contrast for (i): DMACON <-
   0x8040 (BLTEN without DMAEN) predicts no accesses (p.22 bit 9), and
   DMACONR bit 9 = 0.
7. **Reference frame.** The two MARK stores bracket the observation.

### P19. DMACON bit 5 (SPREN) enables sprite DMA

Claim: opus:AA-dmacon.b05. Page 22.

1. **Proposition.** With DMAEN set, sprite DMA reads the data of all eight
   sprites when SPREN is set, and reads none when SPREN is clear.
2. **Stimulus.**
   - Each region SPRx (x = 0 ... 7) holds SPRxPOS = 0x6040 + 8x (SV7-SV0 =
     0x60, SH10-SH3 = 0x40 + 8x), then SPRxCTL = 0x6800 (EV7-EV0 = 0x68;
     all other bits 0, p.28), then 16 data words (0xFFFF, 0x0000 per
     line), then 0x0000, 0x0000.
   - In every vertical blank the CPU writes SPRxPTH and SPRxPTL <- SPRx
     (p.28).
   - BPLCON3 <- 0x0C00, FMODE <- 0. Bit-plane, Copper, blitter, disk and
     audio DMA are off.
   - DMACON <- 0x8220 (SET, DMAEN, SPREN). In the contrast: DMACON <- 0x8200.
3. **Observation.** Reads in SPR0-SPR7 during the two fields (RF-F) after
   the DMACON write.
4. **Expected.** With SPREN = 1, each field has at least one read in each
   of the eight regions. With SPREN = 0, no read occurs in any of them.
   Source: "05 SPREN Sprite DMA enable" (p.22).
5. **Preconditions.**
   - (i) The RF-F check holds.
   - (ii) The stored DMACONR shows DMAEN set and bit 5 as written.
6. **Contrasting case.** SPREN = 0 (above). Contrast for (ii): DMACON <-
   0x8020 (SPREN without DMAEN) predicts no reads, and DMACONR bit 9 = 0.
7. **Reference frame.** RF-F.

### P20. BLTPRI: while the blitter uses every cycle, the CPU gets none

Claim: opus:AA-dmacon.b10. Page 22. (opus:AA-dmacon.b10c is declined,
section 1: /BLS is not a port.) The id stays on P20 for what b10 says:
"Blitter DMA priority(over CPU micro)".

1. **Proposition.** With BLTPRI set, the CPU makes no Chip-memory access
   between the blitter's second access and its second-to-last access in a
   running blit, where the blitter has every cycle. With BLTPRI clear, it
   makes at least one.
2. **Stimulus.**
   - Blitter set-up as in P18, but with BLTSIZE <- 0x2020: height 128 lines,
     width 32 words, 4096 words.
   - The A-and-D copy is chosen because its cycle sequence has no idle cycle
     between the second access and the second-to-last: "9  A D  A0 - A1 D0 A2
     D1 - D2" (HRM p.188, Table 6-2) leaves the blitter idle only after the
     first access and before the last, and "a copy cycle, using A and D,
     takes four clock ticks per cycle" (HRM p.193) gives one A and one D
     access in two memory cycles in between.
   - DMACON <- 0x8640 (SET, BLTPRI, DMAEN, BLTEN) before the blitter
     registers. In the contrast: DMACON <- 0x8240.
   - The CPU stores 0x5A01 to MARK + 6 immediately before the BLTSIZE write,
     and right after the BLTSIZE write runs this loop, 20 000 times, with
     a1 = SCR:

         loop: move.w d0,(a1)+
               addq.w #1,d0
               dbra   d1,loop

     Every iteration fetches instructions from PROG and writes to SCR.
   - After the loop the CPU stores 0x5A02 to MARK + 8. Then an end marker.
     No other DMA is on.
3. **Observation.** The blitter's accesses (reads in BLT_S and writes in
   BLT_D after the BLTSIZE write) are taken in time order. I is the open
   interval between the second access and the second-to-last. The test
   counts the CPU's accesses (reads in PROG, writes in SCR and MARK) whose
   time lies in I. The accesses of the two edge intervals (between the
   first and the second access, and between the second-to-last and the last)
   are counted and printed as MEASURED lines, and not graded.
4. **Expected.** With BLTPRI = 1, the count in I is 0. In I the blitter uses
   every cycle (HRM p.188, p.193), so a CPU access there is a cycle taken
   from a blitter that has the priority: "the blitter will keep the bus for
   every available Chip memory cycle" (HRM p.198, with BLTPRI, "DMAF_BLITHOG",
   set). Source: "10 BLTPRI Blitter DMA priority(over CPU micro)" (p.22). The
   two edge intervals each contain a cycle that Table 6-2 leaves idle; no page
   says whether a cycle the blitter does not use goes to the CPU, so what the
   CPU does there is measured.
5. **Preconditions.**
   - (i) The blit completes: 4096 reads in BLT_S and 4096 writes in BLT_D
     (height x width, p.14).
   - (ii) The CPU is inside its loop throughout the blit. Between the store
     of 0x5A01 to MARK + 6 and the store of 0x5A02 to MARK + 8 the program
     executes only the BLTSIZE write and the loop, so the CPU has a pending
     access at every time in between. (ii) holds when the first store lies
     before the blitter's first access, the second store lies after the
     blitter's last access, and SCR writes continue after the blit.
   - (iii) No bit-plane, sprite, Copper, disk or audio access occurs between
     the blitter's first and last access.
   - (iv) The stored DMACONR shows bits 10, 9 and 6 as written.
6. **Contrasting case.** BLTPRI = 0, all else unchanged. With it clear, the
   count in I is at least 1: the page presents BLTPRI as the priority over the
   CPU, and without it the CPU is served in I. The rate at which the
   original chips released the bus (HRM p.198: one cycle after the 68000
   has waited three consecutive cycles) is not graded. Contrasts for the
   preconditions:
   - (i): BLTSIZE <- 0x2010 (height 128 lines, width 16 words) gives 2048
     reads and 2048 writes (height x width, p.14), and (i) fails, while the
     blit is still bounded by blitter accesses. (DMACON <- 0x8600, without
     BLTEN, would leave I empty.)
   - (ii): in the BLTPRI = 0 configuration, a loop of 10 iterations ends
     while the blit is still running, the store of 0x5A02 lies before the
     blitter's last access, no SCR write follows the blit, and (ii) fails. This contrast is run without BLTPRI: with it, the priority under
     test would itself hold the loop back until the blit ends.
7. **Reference frame.** I, bounded by the blitter's own accesses.

### P21. BBUSY (DMACONR bit 14) reports a blit in progress

Claim: opus:AA-dmacon.b14. Page 22. (opus:AA-dmacon.c6 is declined,
section 1.)

1. **Proposition.** DMACONR bit 14 reads 1 while a blit is running and 0
   before a blit is started and after it has ended.
2. **Stimulus.**
   - Blit: D-only clear. BLTCON0 <- 0x0100 (USED = 1, LF7-LF0 = 0x00),
     BLTCON1 <- 0, BLTDMOD <- 0, BLTDPT <- BLT_D, BLTSIZE <- 0x2020 (4096
     words).
   - DMACON <- 0x8240 (SET, DMAEN, BLTEN; BLTPRI = 0).
   - Samples: each sample is `move.w $DFF002,(a1)+` with a1 in SAMP. One
     store to SAMP comes before the first sample.
   - Phase A: 16 samples. Then the BLTSIZE write. Phase B: 3000 samples in a
     `dbra` loop. Then a wait for the next vertical blank (2.2) and for
     V7-V0 >= 0x30. Phase C: 16 samples.
3. **Observation.** A sample's DMACONR read lies between the store before it
   and its own store. A phase-B sample is inside the blit when both of
   those stores lie strictly between the first and the last write in BLT_D.
4. **Expected.**
   - Bit 14 = 0 in every phase-A sample.
   - Bit 14 = 1 in every phase-B sample inside the blit.
   - Bit 14 = 0 in every phase-C sample.

   Phase-B samples that are not inside the blit are not graded, for two
   reasons. The page does not say how soon after BLTSIZE the bit is set or
   how soon after the last write it clears. The manual it builds on warns
   that the bit "may not yet be set" before the blitter first gets the bus
   (HRM p.186). Sources: "14 BBUSY Blitter busy status bit (read only)";
   "DMACONR 002 R A P DMA control (and blitter status) read"; "This
   register controls all of the DMA channels, and contains Blitter DMA
   status bits." (p.22).
5. **Preconditions.**
   - (i) The blit completes: 4096 writes in BLT_D.
   - (ii) At least one phase-B sample is inside the blit.
   - (iii) The stored DMACONR shows DMAEN and BLTEN set.
6. **Contrasting case.** The same program with phase A lengthened to 4000
   samples, so that the blit starts later. The page predicts 0 in all of
   phase A. That includes the times at which the base run read 1, because
   phase A lasts longer than the base run's first 16 samples plus its
   blit. An implementation whose bit followed the time since reset, or the
   sample count, rather than the blit fails one of the two runs.
   Preconditions (i)-(iii) still hold in this run. Contrasts for the
   preconditions:
   - (i): BLTSIZE <- 0x2010 gives 2048 writes (height x width, p.14), and
     (i) fails.
   - (ii): between the BLTSIZE write and phase B the program waits twice
     for a vertical blank as in 2.2 (each time also for V7-V0 >= 0x30), a
     whole field; by the manual the page builds on, the blit takes about
     4 x 128 x 32 / 7.09 microseconds, 2.3 ms ("The minimum blitter cycle
     is four ticks", HRM p.193). The blit ends before the first phase-B
     sample, (i) and (iii) still hold, and (ii) fails. (A blit of height 1
     and width 1 would also fail (i).)
   - (iii): the DMACONR copy stored before the DMACON write, as in P1.
7. **Reference frame.** The blitter's own writes bracket "inside the blit".
   Consecutive CPU stores bracket each sample.

### P22. BBUSY cannot be written

Claim: opus:AA-dmacon.b14b. Page 22.

1. **Proposition.** Writing bit 14 of DMACON, with SET/CLR 1 or 0, does not
   change what DMACONR bit 14 reads.
2. **Stimulus.** Blit and DMACON as in P21. Sequence:
   - bracketing store; s0;
   - DMACON <- 0xC000 (SET/CLR = 1, bit 14 = 1); s1;
   - BLTSIZE <- 0x2020;
   - 200 samples (B1);
   - DMACON <- 0x4000 (SET/CLR = 0, bit 14 = 1);
   - 1000 samples (B2);
   - wait for the next vertical blank;
   - s2; DMACON <- 0xC000; s3.

   Each s is one sample as in P21.
3. **Observation.** Bit 14 of the samples, bracketed as in P21.
4. **Expected.**
   - Bit 14 = 0 in s0, s1, s2 and s3.
   - Bit 14 = 1 in the last B1 sample and in every B2 sample inside the
     blit.
   - Bits 9 and 6 (DMAEN, BLTEN) read 1 in s1 and in the B2 samples, since
     "Bits written with a zero are unchanged." (p.22).

   Source: "14 BBUSY Blitter busy status bit (read only)" (p.22). The
   quantifier here is both directions of write, at both values of the bit.
5. **Preconditions.**
   - (i)-(iii) of P21.
   - (iv) The last B1 sample and the first B2 sample are both inside the
     blit, so the clearing write fell while the bit was 1.
6. **Contrasting case.** The same writes aimed at a writable bit, at the
   idle positions: DMACON <- 0x8400 between s0 and s1, and DMACON <- 0x0400
   between s2 and s3. The page predicts DMACONR bit 10 (BLTPRI) = 1 in s1
   and 0 in s3. This shows that DMACON writes at those positions do take
   effect, so an unchanged bit 14 is not explained by a write that never
   happened. In this run BLTPRI stays set while the blit runs, and the page
   says that it prevents the micro "from stealing any bus cycles while
   blitter DMA is running" (p.22). The B1 and B2 samples then need not fall
   inside the blit, so (iv) is not required in this run, and only s1, s2
   and s3 are read: bit 10 = 1 in s1 and s2, and 0 in s3. Contrast for (iv):
   3000 samples in B1 put the clearing write after the blit.
7. **Reference frame.** As in P21.

### P23. BZERO cannot be written

Claim: opus:AA-dmacon.b13b. Page 22. (opus:AA-dmacon.c6 is declined,
section 1: BZERO's status behaviour is only precondition (ii) here.)

1. **Proposition.** Writing bit 13 of DMACON, with SET/CLR 1 or 0, does not
   change what DMACONR bit 13 reads, whichever value the bit holds.
2. **Stimulus.**
   - Blit X: BLTCON0 <- 0x0100 (USED = 1, LF7-LF0 = 0x00), BLTCON1 <- 0,
     BLTDPT <- BLT_D, BLTDMOD <- 0, BLTSIZE <- 0x0410. Wait for the next
     vertical blank. Then: bracketing store; z0; DMACON <- 0xA000; z1;
     DMACON <- 0x2000; z2.
   - Blit Y: as X, with BLTCON0 <- 0x01FF (LF7-LF0 = 0xFF). Wait for the
     next vertical blank. Then: y0; DMACON <- 0xA000; y1; DMACON <- 0x2000;
     y2.
   - DMACON <- 0x8240 before blit X.
   - No source is enabled (USEA = USEB = USEC = 0). So minterm select 0x00
     makes every destination word 0, and 0xFF makes every destination word
     all ones, whichever minterm stands for which source combination ("LF7-0
     Logic function minterm select lines", p.14).
3. **Observation.** Bit 13 of the six samples, and the data of the writes in
   BLT_D.
4. **Expected.** Bit 13 is equal in z0, z1 and z2, and equal in y0, y1 and
   y2. Source: "13 BZERO Blitter logic zero status bit.(read only)"
   (p.22).
5. **Preconditions.**
   - (i) Each blit writes its 256 words before its first sample, every
     word the value its minterm select gives with no source enabled:
     0x0000 for LF7-LF0 = 0x00 (X), 0xFFFF for LF7-LF0 = 0xFF (Y).
   - (ii) Bit 13 of z0 differs from bit 13 of y0. The bit reports the
     blitter's zero status ("contains Blitter DMA status bits", p.22), so
     the two blits leave it with different values, and both values are
     exercised. The page does not say which value means "all zero"; only
     the difference is required.
   - (iii) The stored DMACONR shows DMAEN and BLTEN set.
6. **Contrasting case.** A blit between two samples: running blit Y between
   z2 and one further sample predicts a change of bit 13 (precondition ii).
   The bit is live, so an unchanged value after a DMACON write is a real
   result. As in P22, DMACON <- 0x8400 at the same positions predicts
   DMACONR bit 10 = 1 in the following sample. Contrast for (ii): LF7-LF0 =
   0x00 in both blits. Both then write 0x0000, which (i) expects for that
   minterm, so (i) still holds; bit 13 is predicted equal in z0 and y0, and
   (ii) fails.
7. **Reference frame.** Consecutive CPU stores bracket each sample.

---

## 4 What the pages leave open or state inconsistently

1. **Two naming schemes for H bits.** The display window registers were
   renamed so that H2 is 140 ns (p.22 NOTE; p.5). The beam counter (p.29),
   the Copper (p.20) and DDFSTRT/DDFSTOP (p.21) keep the old names, in which
   H1 is 280 ns. The DDF weights in 2.4 rest on this reading. The p.22 NOTE
   limits the renaming to "all 3 display window registers", and p.5's list
   of 35 ns comparators leaves DDF out.
2. **Bit 1 of DDFSTRT/DDFSTOP.** The AA page names it H2. The HRM says bits
   1-0 are unused (HRM p.91). No page says in which mode H2 acts, so no
   proposition grades it.
3. **H3.** The AA page gives no condition. The HRM says "bit H3 only
   respected in Hires Mode" (HRM p.91). The HRM itself is not consistent on
   the hires data-fetch resolution. For a weight of 4 clocks: HRM p.60
   ("DDFSTRT = DDFSTOP - (4 * (word count - 2)) for high resolution", normal
   hires $3C and $D4) and HRM p.91 (H3 respected in hires). Against it: HRM
   p.195 Figure 6-9 ("Data fetch start can only be specified at even
   multiples of 8 clocks. This is the clock position which should be
   specified for the normal width display. (20 word fetch for 320 pixel, 40
   word fetch for 640 pixel width)") and p.197 Figure 6-12 (the hires timing
   cycle T..T+7, every plane fetched twice), which put the hires fetch on a
   resolution of 8 clocks. HRM p.80 ("only one word is fetched at this
   limit") is the statement about the `$D8` limit and does not apply to a
   DDFSTOP of `$D4`. The pages admit two readings of the hires fetch edges,
   so no exact edge movement, and no unchanged opposite edge, is graded for
   H3 (P5 is a record, and opus:AA-ddf.use-b02 is declined).
4. **"(max)", the asterisk, and "maxes & mins".**
   - The extra-wide row carries "(max) *", but the page has no footnote for
     that asterisk. The next asterisks on the pages belong to DIWSTRT and
     DIWSTOP (p.22).
   - The note "(The maxes & mins, that is)" does not say which numbers are
     the minima.
   - The package reads "(max)" as a limit: no DDFSTRT value starts the
     fetch earlier than the extra-wide row, and no DDFSTOP value ends it
     later than the wide row. No proposition grades it, and the four row
     claims that carry it are declined (section 1). For DDFSTOP the row's
     value, 0xD8, is the hardware limit of the manual the page builds on
     ("a rightmost limit to DDFSTOP of ($D8)", HRM p.80). For DDFSTRT that
     manual gives the limit as ($18) (HRM p.80), not the row's 0x28. No
     value below 0x28 or above 0xD8 is used.
5. **Variable beam counter mode and PAL.** The pages give no DDF numbers for
   variable beam counter mode. The AA pages state no reset value for
   VARBEAMEN or PAL (p.12 states one only for HARDDIS), so the package writes
   BEAMCON0 explicitly in both display set-ups, S-FETCH and S-WIN.
6. **"described above" (p.21).** The display window registers it refers to
   follow on p.21-p.22 in this alphabetical list. The sentence is carried
   over from the HRM.
7. **Horizontal restriction names.** The DIWSTRT/DIWSTOP restrictions use
   the old name "(H8=0)" / "(H8=1)", while the USE line above them already
   uses the renamed H9-H2. In the new naming, the implied bit would be H10.
   It is H10 that DIWHIGH carries.
8. **"upper 2/3" and "lower 1/2".** These fractions do not correspond
   exactly to the bit rules V8 = 0 and V8 = not V7. The package grades the
   bit rules.
9. **Stop line.** The pages do not say whether the line numbered VSTOP is
   inside the window or the first line outside it. Only differences of the
   stop are graded.
10. **The DIWHIGH interlock.** "DIWHIGH is reset by writes to DIWSTRT or
    DIWSTOP." (p.4) does not say whether the reset brings back the
    not-written rules of p.22 or leaves zero upper bits in direct mode. The
    two readings differ for the stop when V7 = 0. The P12-P15 contrast
    therefore grades only the start.
11. **Windows that never close.** No page says how a window whose stop is
    never reached ends, or whether the hardwired window limits (p.12,
    HARDDIS) end it. Grading in P15 stops at line 0x120.
12. **BZERO polarity.** The AA page does not state which value of BZERO
    means "all zero". The HRM does (HRM p.222: "Remains 1 if, during a
    blitter operation, the blitter output was always zero"). No polarity is
    graded.
13. **BLTPRI.** The page says "stealing any bus cycles"; the HRM says "the
    blitter will keep the bus for every available Chip memory cycle" (HRM
    p.198). The HRM's cycle sequence for an A-and-D copy leaves two cycles
    idle, one after the first access and one before the last ("9  A D  A0 -
    A1 D0 A2 D1 - D2", HRM p.188, Table 6-2). No page says whether a cycle the
    blitter does not use is one the CPU may take, or one the blitter keeps. The
    package grades the CPU's accesses between the blitter's second access and
    its second-to-last, where the blitter has every cycle, and prints the
    accesses in the two edge intervals as MEASURED lines. It also requires at
    least one CPU access without BLTPRI, which the page implies but whose
    rate it does not state.
14. **UHRES enabling bits.** p.16 says UHRES "also needs bits in DMACON"
    without naming them. p.22 names DMAEN.
15. **DIWHIGH start bits in the HRM.** The HRM labels DIWHIGH bits 2-0
    "Vertical stop, most significant 3 bits" (HRM p.306), where the AA page
    labels the right half "(start)". The package follows the AA page.
16. **HARDDIS.** HARDDIS is described differently by the two documents:
    "disable the hardwire vertical and horizontal window limits" (p.12)
    against "Disable hardwired vertical/horizontal blank" (HRM p.305). It is
    not used.
17. **Minor inconsistencies.**
    - DMACON's chip column reads "A D P" on p.22 but "A P" in the address
      list (p.7).
    - The legends for the revision marks h/p/H/P differ between p.6 and
      p.10.
    - The text layer of p.21 ends with two lines of scanner noise that
      repeat the DDFSTOP heading and the USE line. The scan (p.21, bands 2
      and 3) shows the USE line with H2 at bit 1 and the tables as quoted
      above.


---

## 5 What the build adds

### 5.1 Program and command list

One program, `prog_s4.s`, runs every proposition. It sets the status register to
0x2700, writes INTENA, INTREQ and DMACON with 0x7FFF, and then interprets a
command list that the test places in Chip memory at PARAM = $0F0000 before the
program starts. The command list is the stimulus of a configuration: register
writes (one MOVE.W per write), Copper lists, sprite data, bit-plane data, blits,
the vertical-blank loop of 2.2 and the DMACONR sampling loops. PARAM lies below
$100000, apart from the regions of 2.3 and from the stack, and the program reads it
only between commands. Commands: W (register write), WVB (wait for V8 = 0 and
0x04 <= V7-V0 <= 0x14), WL30 (wait for V7-V0 >= 0x30), WLN (wait for V7-V0 >= n),
MARK (store the configuration index to MARK), CPYD (store DMACONR to the next
word of MARK + $100), SAMPLE and SSTORE (the DMACONR samples and the bracketing
store, to SAMP), STORE, COPYW, FILLL, ZERO (CPU writes of data before DMA is
enabled), RUN20 (the blit-time loop of P20) and WBLT, WSAMP (a register write
immediately followed by a MARK store or by the sampling loop).

MARK words: + 0 the configuration index, + 2 the constant $BEEF (the fixture,
stored first), + 4 the end marker, + 6 and + 8 the loop markers of P20 ($5A01,
$5A02), + $100 and up the DMACONR copies. The program
is assembled for the 68000. Z is written with zeros by the CPU (ZERO) before DMA is
enabled; the planes B1-B4 are written with a counting pattern.

### 5.2 Labels

The first token of an expect label is the label that NMH_MUTATE selects. A
proposition is `Pn`, a joint assertion `Jn`, a control `Pn-ctl`. `P5` and
`P5-last` are MEASURED lines (5.4), not assertions. Preconditions:
`Pn-fix` (the fixture constant read back at the end of each run), `Pn-i-RFM`,
`Pn-i-RFL` (the two parts of precondition (i) of the fetch propositions),
`Pn-i-RFV` (RF-V), `Pn-ii` ... `Pn-vi` (the numbered preconditions of the
proposition), `Pn-dm` (the DMACONR copy of the window propositions, which
the specification lists under P6 (iii) only), `Pn-reg` (the region precondition of
2.3, evaluated from the port log and from the command list), and for
P16-P19 `Pn-i` (RF-F), `Pn-ii` (the DMACONR copy), `Pn-reg`. P3 and P4 are graded
under the preconditions printed for P1 and P2 (the same runs, a subset of their
configurations). `CONFIG` is the precondition that the host's own configuration value
selects the AA chip set (chipset_config 0x10) and 2 MB of Chip memory (a
check of the stimulus, not a measurement of the DUT: AA in force at the ports
is shown by the FMODE contrasts of P1, P5 and P7, whose B1 steps are 4 bytes under
chipset_config 0x10 with FMODE = 0x0001), `RF-RUNS` the check that P, L and
A(0x3E) - T0 are the same in every run.

### 5.3 Where the build had to choose

1. **RF-L, "none between the fetch of END and the next T0".** In every field of
   every run the Copper reads one further word, at the byte address of the
   word after the list's END instruction, after it has read both words of END
   (see 5.4). That address lies inside COP. The read comes at the
   vertical-blank restart: the MEASURED line prints that it lies 16 system
   clock cycles (2 memory cycles) before the next field's first fetch from
   COP1LC (T0), and not during the wait on END. No page describes the bus cycles of that
   restart. "(keeps Coprocessor off of bus until beam position has been
   reached)" (p.20) covers the wait; "COP1LC is automatically used at the
   beginning of each vertical blank time." (p.21) names only the pointer.
   `Pn-i-RFL` therefore grades the part that has a page (each anchor fetch is
   read once, A(v+1) - A(v) constant, A(0x3E) - T0 constant, both words of END
   fetched) and the read after END is a MEASURED line.
2. **Precondition (ii) of P8-P11.** "DIWHIGH has not been written" is evaluated
   from the command list: it contains no write of DIWHIGH. Precondition (ii) of
   P12-P15 ("DIWHIGH written after DIWSTRT and DIWSTOP in the same vertical
   blank") is evaluated from the command list in the same way.
3. **Precondition (iv) of P20 and (ii) of P16-P19**, "as written": the stored
   DMACONR is compared with the bits of the last DMACON write of the command
   list, so that the mutations of the assertions (which change the written
   value) leave the precondition true.
4. **The mutation of P6.** The pointer and the observed region stay as they
   are. W2 writes DIWSTOP = 0xB0C1 in place of 0xA0C1, a page-defined window
   position, while the comparator keeps the 0xA0C1 window. The last-line
   difference is then 0x40 where the comparison expects 0x50, and P6 fails
   through the unchanged comparator.
5. **P5 and P5-last** are MEASURED lines. They have no mutation, because no
   assertion is graded; the green line of that run is P5-ctl, which has its own
   record.
6. **The loop markers of P20.** The program stores $5A01 to MARK + 6 before
   the BLTSIZE write and $5A02 to MARK + 8 after the loop; P20 (ii) compares
   their times with the blitter's first and last access.

### 5.4 Measured values

The final run of each core (`make -k -B test`; out_tg68k.txt and out_fx68k.txt).
Both builds print the same lines except where noted (P20, P21 and P22-ii). System clock cycles are
counted from A(v); P = 8 system clock cycles per memory cycle and L = 1816
system clock cycles per line in every run on both builds.

```text
P1 measured: Copper reads after the second word of the list's END: 7 in 7 fields, at byte address 0x08C2E8, inside COP, 16 system clock cycles before the next T0
P1 measured (system clock cycles from A(v)): 0x0028 f=344 l=1688 n=22; 0x0030 f=408 l=1688 n=21; 0x0038 f=472 l=1688 n=20; 0x0040 f=536 l=1688 n=19; 0x0060 f=792 l=1688 n=15; 0x0068 f=856 l=1688 n=14; 0x00B8 f=1496 l=1688 n=4; P = 8
P2 measured: Copper reads after the second word of the list's END: 6 in 6 fields, at byte address 0x08C2E8, inside COP, 16 system clock cycles before the next T0
P2 measured (system clock cycles from A(v)): 0x0058 f=472 l=728 n=5; 0x0098 f=472 l=1240 n=13; 0x00B8 f=472 l=1496 n=17; 0x00C8 f=472 l=1624 n=19; 0x00D0 f=472 l=1688 n=20; 0x00D8 f=472 l=1752 n=21; P = 8
P5 measured: Copper reads after the second word of the list's END: 7 in 7 fields, at byte address 0x08C2E8, inside COP, 16 system clock cycles before the next T0
P5 measured (system clock cycles from A(v)): H1 f=440 l=1752; H2 f=472 l=1720; H3 f=472 l=1720; C1 f=376 l=1688; C2 f=440 l=1688; C3 f=440 l=1624; P = 8
P5 measured: the DDFSTRT half (H2 against H1) does not hold, the DDFSTOP half (H2 against H3) does not hold
P5-last measured, not graded: l(H1) = 1752, l(H2) = 1720 system clock cycles from A(v), DDFSTOP = 0x00D4 in both and DDFSTRT 0x0038 against 0x003C; the pages admit two readings of the hires fetch edges: HRM p.195 Figure 6-9 (data fetch start only at even multiples of 8 clocks) and HRM p.197 Figure 6-12 (aligned blocks) against HRM p.91 and AA p.21 (bit 2 is H3); HRM p.80's 'only one word is fetched at this limit' is about the $D8 limit, and H1 and H2 use DDFSTOP $D4
P5 measured, not graded: f(H2) - f(H1) = 32, l(H2) - l(H3) = 0 system clock cycles (4 memory cycles = 32); the pages admit two readings of the hires fetch edges: HRM p.195 Figure 6-9 (data fetch start only at even multiples of 8 clocks) and HRM p.197 Figure 6-12 (aligned blocks) against HRM p.91 and AA p.21 (bit 2 is H3); HRM p.80's 'only one word is fetched at this limit' is about the $D8 limit, and H1 and H2 use DDFSTOP $D4
P6 measured, configuration 1: Fetch 176 lines (040 - 0EF), Win 176 lines (040 - 0EF), k = 2, RF-V holds, Win equals Fetch: yes
P6 measured, configuration 2: Fetch 80 lines (050 - 09F), Win 80 lines (050 - 09F), k = 2, RF-V holds, Win equals Fetch: yes
P7 measured (bytes): pitch plane 1 / plane 2: M-A 48/64, M-B 50/66, M-C 64/48
P8 measured (first-last line of Win): 40: 040-0EF; 41: 041-0EF; 42: 042-0EF; 44: 044-0EF; 48: 048-0EF; 50: 050-0EF; 60: 060-0EF; 30: 030-0EF; 70: 070-0EF
P9 measured (first-last line of Win): B0: 040-0AF; B1: 040-0B0; B2: 040-0B1; B4: 040-0B3; B8: 040-0B7; A0: 040-09F; 90: 040-08F; F0: 040-0EF
P10 measured (first-last line of Win): 0x4081: 040-0EF; 0xC081: 0C0-0EF
P11 measured (first-last line of Win): 0x90C1: 040-08F; 0x10C1: 040-10F
P12 measured (first-last line of Win): DIWHIGH 0x2100: 028-129; 0x2101: 128-129
P13 measured (first-last line of Win, from line 0x1D): DIWHIGH 0x2100: 028-129; 0x2102: empty
P14 measured (first-last line of Win): DIWHIGH 0x2000: 028-029; 0x2100: 028-129
P15 measured (first-last line of Win up to 0x120): DIWSTOP 0x10C1, DIWHIGH 0x2100: 028-10F; DIWSTOP 0x11C1, DIWHIGH 0x2100: 028-110; DIWSTOP 0x10C1, DIWHIGH 0x2300: 028-120
P16 measured (reads per field and plane, BPLEN set / clear): 3520/0, 3520/0, 3520/0, 3520/0, 3520/0, 3520/0, 3520/0, 3520/0
P17 measured (reads in COP in the two fields, COPEN set / clear): 10 / 0
P18 measured (BLT_S reads / BLT_D writes): BLTEN set 251 / 252, BLTEN clear 0 / 0
P19 measured (reads per field and sprite, SPREN set / clear): 20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0,20/0
P23 measured: DMACONR bit 13 in z0 z1 z2 y0 y1 y2: 111 000
RF-F measured: spacings of the first 3 stores to MARK (system clock cycles) / longest pass Q of the polling loop (system clock cycles), run by run (set run, clear run) in the order P16, P17, P19
    TG68K: P16 568376, 568424 / 600 and 568376, 568424 / 600; P17 568424, 568424 / 280 and 568376, 568424 / 280; P19 568424, 568376 / 600 and 568424, 568376 / 600
    fx68k: P16 568456, 568344 / 768 and 568392, 568408 / 768; P17 568320, 568496 / 400 and 568288, 568408 / 400; P19 568408, 568408 / 760 and 568408, 568408 / 752
    The polling loop is at 0x080472; the passes counted per run are between 3507 and 4698.
P20 measured (system clock cycles from the blitter's first and last access): marker 0x5A01 and marker 0x5A02
    TG68K: BLTPRI set -76 / +1928508, BLTPRI clear -76 / +1895212
    fx68k: BLTPRI set -76 / +1771740, BLTPRI clear -76 / +1736628
P20 measured (CPU accesses between the blitter's second and second-to-last access, BLTPRI set / clear)
    TG68K: 0 / 2071     fx68k: 0 / 1982
P20 measured (CPU accesses between the blitter's first and second access, BLTPRI set / clear): 1 / 1; between its second-to-last and last access: 1 / 1
    (both builds)
P22-ii (samples of B1 or B2 inside the blit)
    TG68K: 507          fx68k: 509
P21 (phase-B samples inside the blit, base run / long phase A)
    TG68K: 511 / 511    fx68k: 515 / 514
```

Graded counts: both builds print 149 passed, 0 failed.

RF-F. The stores to MARK that bound a field are made by a polled wait. The
measured spacings of consecutive stores differ by up to 176 system clock
cycles (568288 to 568496); Q, the longest pass of the polling loop, is 280 to
768 system clock cycles, shorter than a line (1816). Every RF-F line prints
PASS on both builds. With LACE = 1 the two spacings are 566576 to 566696 and
568304 to 568456, a difference of 1608 to 1824 system clock cycles, and the RF-F
lines print FAIL (mutations P16-i-lace, P17-i-lace, P19-i-lace).

P5. In hires the first read moves with DDFSTRT bit 2 (H3) by 4 memory cycles
(f: 440 to 472), the last read moves 4 memory cycles earlier with DDFSTOP
unchanged (l: 1752 to 1720), and DDFSTOP bit 2 (0x00D4 against 0x00D0, DDFSTRT
0x003C) leaves the last read where it was (1720 and 1720). These are measured,
not graded. The control P5-ctl, the same comparators on bit 3 of both
registers in hires, prints PASS.

P20. With BLTPRI set no CPU access falls between the blitter's second access and
its second-to-last; with BLTPRI clear 2071 (TG68K) and 1982 (fx68k) fall there.
In each edge interval one CPU access falls with BLTPRI set and one with it clear,
on both builds (printed as MEASURED lines, not graded).

### 5.5 Measurements outside the graded run

1. **Window instrument without bit planes.** In the mutation run P6-ii
   (BPLCON0 = 0x0001, BPU = 0) both builds show no red row between the markers:
   Win is empty (0 lines) in both configurations, with Fetch empty as well. The
   window instrument therefore follows the fetch on this design, and P6 is
   graded against the registers (P6).

### 5.6 Mutations

Every assertion, control and precondition has one record in the header of
`test_aa_integ_s4.cpp`, selected by `NMH_MUTATE=<label>`. A mutation changes
the stimulus (the command list, the Copper list or the harness configuration)
and prints FAIL through the unchanged comparator on both builds, with every run
reaching DONE. Where the mutated stimulus also changes a neighbouring
precondition, the record lists it as "Also printing FAIL". P5 and P5-last
are MEASURED lines and have no record. The RF-F lines P16-i, P17-i and P19-i
each have a second record, selector P16-i-lace, P17-i-lace and P19-i-lace,
that sets LACE = 1.
