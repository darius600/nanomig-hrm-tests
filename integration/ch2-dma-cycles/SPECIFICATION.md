# Specification: memory-cycle parity of the DMA channels, and sprite channel reuse

This specification was written from this package's claims (`claims-spec-stage.jsonl`) and
the pages of the *Amiga Hardware Reference Manual*, 3rd edition. Its author never saw the
implementation under test or any existing test. Every expected value below is taken from a
printed page and quoted. Where the pages leave a value open or disagree with each other, the
specification says so and grades only what all of them agree on. It specifies twelve
propositions; P6 is withdrawn. Eighteen of the package's 22 claims support them; four could not be used.

Page numbers are printed page numbers. "Fig." means a figure of the manual. "p.X" without a
chapter means the page in the manual.

---

## 0. Claims not turned into a proposition

| claim | page | says | reason |
|---|---|---|---|
| `COP2-about-the-copper.c34` | 20 | The blitter DMA channel handles animation. | The parenthesis *"(the DMA channel that handles animation, line drawing, and polygon filling)"* says which channel the sentence means. It describes no behaviour. "Animation" is a use, not a machine function: no register setting, output or value on p.20 or on any other page defines what "handling animation" produces. There is nothing to observe and no expected value. |
| `COP2-about-the-copper.c35` | 20 | The blitter DMA channel handles line drawing. | The same parenthesis. The claim states a purpose and gives no observable and no expected value. What a line-mode blit writes is specified only on pp.189–192, and no sentence there is a claim of this package. A check built from those pages would test behaviour this package holds no claim for, with expected values taken entirely from pages outside it. |
| `COP2-about-the-copper.c36` | 20 | The blitter DMA channel handles polygon filling. | As for c35. Area fill is specified on pp.184–186, which are outside this package's claims. |
| `ch2-about-the-copper.c21` | 20 | The Copper requests the bus only during odd-numbered memory cycles, which prevents collision with audio, disk, refresh, sprites and most low-resolution display DMA. | One sentence bundles three things: the requests, the collisions, and five DMA channels. The Chip-memory port shows the accesses that happen, not the requests made or the collisions avoided. P6, the parity of the accesses in the Copper block, is withdrawn (P6 below): the port shows accesses, not requests. P7 grades that the Copper's cycles do not move when the even-cycle channels run; it observes neither a request nor a collision, and leaves the disk-to-memory direction undone (D9). The id is therefore not cited on any graded line. |

The sentence those three claims come from also says something testable: *"The Copper,
therefore, needs priority over only the 680x0 and the blitter"*. No claim of this package
states it, so it is not specified here.

Several claims are two readings of the same sentence, one `ch2-…` uid and one `COP2-…` uid.
They are merged into one proposition, and both uids are cited there.

---

## 1. Where the pages disagree or leave a point open

Each item names the pages, quotes them, and says how the specification handles it.

**D1. Which parity is called "even".**
- p.20: *"The Copper is a two-cycle processor that requests the bus only during odd-numbered
  memory cycles. This prevents collision with audio, disk, refresh, sprites, and most low
  resolution display DMA access, all of which use only the even-numbered memory cycles."*
- Appendix A p.275 (printed page checked): *"The Copper is basically a two-cycle machine that
  requests the bus only during odd memory cycles (4 memory cycles per instruction). This
  prevents collisions with display, audio, disk, refresh, and sprites, all of which use only
  even cycles."*
- p.196: *"The 68000 uses only the even-numbered memory access cycles."* Fig. 6-10 (p.196)
  labels the halves of a 68000 cycle *"odd cycle, assigned to other devices"* and *"even
  cycle, available to the 68000"*.

p.20 and p.275 put refresh, disk, audio and sprites on the even cycles. p.196 puts the
68000 on the even cycles and the "other devices" on the odd ones. The pages agree on the
relation: those channels share one parity, and the Copper uses the other. They disagree on
which of the two is named "even". **Handling:** the reference frame (§2.3) numbers each
line's cycles from that line's first refresh access. "Even" in this specification therefore
means "the parity of the refresh accesses", which is the class p.20 and p.275 call even.
Every check is a relation between accesses of the same line. No check depends on the
absolute name, and none concerns the 68000.

**D2. How much of the display DMA uses the even cycles.**
- p.20 says *"most low resolution display DMA access"*.
- Appendix A p.275 says *"display"*, with no qualification.
- Fig. 6-9 (p.195) draws low-resolution bitplanes 1–4 on the same parity as refresh, disk,
  audio and sprites, bitplanes 5 and 6 on the other parity, and 640-mode bitplanes in every
  slot of the fetch cycle. Fig. 6-11 (p.197) draws the six-bitplane pattern
  `_ 4 6 2 _ 3 5 1`: planes 1–4 on the odd offsets from T, planes 5 and 6 on the even ones.
- p.196: *"If the display contains four or fewer low resolution bitplanes, the 68000 can be
  granted alternate memory cycles … However, if there are more than four bitplanes, bitplane
  DMA will begin to steal cycles from the 68000 during the display."*
- p.197: four high-resolution bitplanes need *"all of the available memory time slots"* and
  lock out *"the 68000 (as well as the blitter or Copper)"*.

**Handling:** every page puts low-resolution bitplanes 1–4 on the even parity, so that is
graded. The pages also agree that more than half of the low-resolution fetches in a line are
on the even parity, so that is graded too. The parity of planes 5 and 6 and of
high-resolution fetches is not graded, because p.275 contradicts the figures and pp.196–197
on it.

**D3. The cycle numbering of Fig. 6-9 is not absolute.** The figure's hex tick marks sit on
slot *boundaries*. The drawing alone does not say whether `$0` labels the slot to its left
(the first refresh slot) or the slot to its right. Its footnote reads: *"# This cycle 0
appears to exclude one of the memory refresh cycles. This is not the case. Actual system
hardware demands certain specific values for data fetch start and display start. Therefore
this timing chart has been "adjusted" to match those requirements."* The footnote places
cycle 0 on the first refresh slot, and it also declares every absolute position in the chart
adjusted. **Handling:** no absolute slot number from Fig. 6-9 is graded. The chart is used
only for relations that do not depend on its origin: which drawn slots share a parity, and
where the four refresh slots lie relative to the first of them. The figure hatches four
slots, on every second slot, with one unhatched slot between each pair: at 0, 2, 4 and 6
memory cycles from the first hatched slot, the one its footnote calls cycle 0. F2 grades
those offsets.

**D4. Length of a PAL line.** p.23: *"In PAL, they are all 227 long."* p.194: *"there are
227.5 "color clocks", or memory access cycles"* per line. p.304 lists *"PAL (625 lines,
227.5 colorclocks per scan line)"*. For NTSC, p.23 gives lines of 228 and 227 cycles
alternating. **Handling:** the reference frame accepts lines of 227 or 228 memory cycles in
either standard and depends on nothing else about line length.

**D5. The 68000 and the Copper.** p.20 says the Copper *"needs priority over only the 680x0
and the blitter"*, which implies that the 680x0 competes for the Copper's odd cycles. p.196
says the 68000 uses *"only the even-numbered memory access cycles"*. No claim here concerns
the 68000, so nothing is graded on it. The CPU is kept off the bus wherever a check could
depend on it (§2.6).

**D6. What DMAON in DSKBYTR reflects.** p.248: DMAON *"Indicates when DMA is actually
enabled. All the various DMA bits must be true. This means the DMAEN bit in DSKLEN, and the
DSKEN & DMAEN bits in DMACON."* Appendix A p.278: *"Mirror of bit 15 (DMAEN) in DSKLEN, ANDed
with Bit09 (DMAEN) in DMACON"*. The two pages disagree on whether DSKEN takes part.
**Handling:** DMAON is used only as a precondition read with every bit set, where both pages
predict 1. Its contrast clears DSKLEN bit 15, where both predict 0.

**D7. What address a refresh access carries.** No page says. Measured, not a page fact: the
refresh accesses read word 0 of the vector table. **Handling:** refresh accesses are
identified by exclusion (§2.2), and a guard precondition (A1) checks that the exclusion holds
in the baseline configuration. The vector table is therefore not declared as a block (§2.2).

**D8. The Copper's wake-up cycle.** p.20: *"The WAIT instruction requires three memory
cycles and six memory cycle times; it takes one extra memory cycle to wake up."* p.275:
*"All instructions (except for WAIT) require two bus cycles"*. The pages do not say whether
the extra cycle is a bus access, or at which address. **Handling:** the Copper's block of
memory holds only Copper lists and padding. Every access into it counts as a Copper access
in the attribution table (§2.2); the check that would grade them, P6, is withdrawn. No check
counts WAIT cycles.

**D9. Disk read DMA without a drive.** p.250: *"If the WORDSYNC bit is enabled in ADKCON, no
data is transferred until a word is found in the input stream that matches the word in the
DSKSYNC register."* The system under test has no drive. No page says whether read DMA
transfers words when there is no input stream. **Handling:** read-direction disk DMA is run
with WORDSYNC clear, under a precondition that disk accesses actually occur. If none occur,
the read direction is reported as not exercised, not as passing.

**D10. Registers the Copper may write.** p.21 says any register at `$20` or above. p.26 says
*"The registers that the Copper can always affect are numbered $80 through $FF inclusive"*.
Read literally, p.26 would exclude the colour and sprite registers, yet p.27 (MOVE to
COLOR17–19, MOVE to sprite pointer 1) and pp.28–29 (MOVE to COLOR00 and BPLCON0) show the
Copper writing them. **Handling:** the Copper lists write only registers that pp.21, 27–29
and 110 show being written by the Copper: pointers `$080`–`$08A`, `$0E0`–`$0F6` and
`$120`–`$13E`, and colours `$180`–`$1AE`.

**D11. When the Copper restarts.** Both sentences are in one box on p.25: *"At the start of
each vertical blanking interval, COP1LC is automatically used"* and *"when the end of
vertical blanking occurs, the Copper is automatically forced to restart"*. **Handling:** a
frame boundary is the Copper's fetch at the COP1LC address, wherever in vertical blanking it
happens (§2.4). No check depends on which line that is.

**D12. The gap between sprite uses.** p.117: *"at least one horizontal scan line"* must
separate uses. p.108: *"If the VSTART value contained in these words is lower than the
current beam position, this sprite will not be reused in this display field."* The pages
leave open what happens when the next VSTART equals the line in which the control words are
fetched. **Handling:** every reuse below leaves a gap of at least four lines, and nothing is
graded at the boundary.

**D13. "Field" and "frame".** p.27 says *"during the same display field"* for reuse and
*"during a full display frame"* for colours. All runs are non-interlaced (BPLCON0 bit 2
clear, p.64), so a frame is one field (p.34).

---

## 2. Observation model and common machinery

### 2.1 The trace and the slot period S

The test reads the Chip-memory port trace. Each access has a time in the harness's system
clock cycles, a word address, data, and a direction.

p.24: *"One horizontal count takes one cycle of the system clock (processor is twice this).
NTSC- 3,579,545 Hz PAL - 3,546,895 Hz"*. p.194: *"A memory cycle is approximately 280 ns in
duration."* One memory cycle is therefore one horizontal count at the colour clock of p.24.

**S** is the number of trace time units in one memory cycle. It is measured: S is the
most common interval between the first accesses of consecutive memory-refresh groups (§2.3),
divided by 227, the
shorter of the two line lengths p.23 gives (*"In PAL, they are all 227 long"*; NTSC alternates
227 and 228). No constant is taken from any RTL source. F3 guards the measurement: every
observed line's own interval must equal exactly 227·S or 228·S, or the derivation is wrong for
that run.

- **Precondition F1 (slot grid).** Graded over the run's DMA accesses only -- every population
  §2.2 declares a block for, excluding the CPU block. S is an integer, and every DMA access
  time t satisfies t ≡ φ (mod S) for one phase φ. No page gives the 680x0's own accesses a
  phase at these ports (D5): the CPU's phase relative to the DMA grid is measured and reported
  on its own line, not folded into F1.
- **Contrast for F1.** None, impossible. The DMA slot grid is the memory cycle of the machine,
  and no register and no program instruction moves a DMA channel to another phase of it. The
  harness samples the port every 7 MHz period, that is twice per memory cycle, and the 68000's
  accesses lie on another phase than the DMA channels' (the CPU-phase line prints it; it is
  measured, not graded): F1's scope excludes them. A run in which the CPU reads a word of a
  declared DMA block would turn F1 red, but through the address classifier, which would call a
  CPU access DMA, and not through the grid; no run uses it as a mutation. The checker has a
  self-test instead, SELFTEST-F1: a synthetic trace with one access moved by one time unit must
  fail F1. F2's comparators have the same kind of self-test, SELFTEST-F2.

### 2.2 Attribution of accesses

Every run lays out Chip memory in declared, disjoint blocks (§2.7). An access is attributed
by its word address:

| block | attributed to |
|---|---|
| CPU block (program code, stack and results area, at or above logical `$080000`; and the level-3 vector words at `$00006C`–`$00006F`) | CPU |
| Copper block (every Copper list of the run, plus padding) | Copper |
| audio block x (x = 0..3) | audio channel x |
| disk block | disk |
| sprite block x (x = 0..7), null-sprite block | sprite DMA |
| bitplane block y (y = 1..6) | bitplane y |
| none of the above | **refresh set R** |

The rest of the vector table (`$000000`–`$0003FF` except those two words) is not a block:
the refresh accesses read word 0 of it (D7, measured), so it belongs to the refresh set R.
The level-3 vector words are the CPU's: the program installs the vector by a CPU write, and
the CPU reads it on every vertical-blank exception. Measured in C0, C2a, C6, S2 and C7a: the
only CPU reads below `$080000` in the observed frames are those words, and no write below
`$080000` falls in the observed frames.

The blitter is never enabled: DMACON bit 6 (BLTEN) stays 0 in every run (Appendix A
p.278).

If the harness reports the requesting channel with each access, the test must also assert
that this report agrees with the address attribution. That assertion is a precondition.

- **Precondition A1 (attribution guard).** In the baseline configuration C0 (§2.9), every
  access in the observed frames is either in the refresh set R, in the CPU block, or in the
  Copper block. Nothing falls into any other declared block. The measurement declares every
  reserved block that is not the refresh set: the Copper block, the four audio blocks, the
  disk block, the eight sprite blocks and the null block, and all six bitplane blocks. The
  contrast changes only the program.
- **Contrast for A1.** Run C1a (C0's list, four audio channels with AUDxLEN = 16, AUDxPER =
  124 and AUDxEN set) graded with the same table as C0. p.144 predicts audio DMA: *"When both
  DMAEN and AUDxEN are set, the DMA will start for channel x"*. Accesses then fall into the
  audio blocks and A1 fails.

### 2.3 Reference frame F: lines and in-line cycle numbers

**Anchor on the page.** Fig. 6-9 numbers the memory cycles of one horizontal line from
`$0`. It draws the four memory-refresh slots first, on alternate slots within the first
eight, and labelled *"MEMORY REFRESH DMA TIME"*. Its footnote `#` places cycle 0 on the
first refresh slot. p.23 gives the horizontal position as `$0`–`$E2` (to `$E3` on long NTSC
lines). p.20 says refresh uses the even-numbered cycles. The frame therefore puts cycle 0 of
a line on that line's first refresh access. Under this anchor "even" is exactly the class
p.20 calls even (see D1).

**Construction.**
1. Sort the refresh set R of the run by time. Start a new **refresh group** at every access
   that comes more than 16 memory cycles (16·S) after the previous access in R. (Fig. 6-9
   draws all refresh within the first 8 cycles of a line of 227–228 cycles; the value 16
   only allows a margin.)
2. Let A_n be the time of the first access of group n. **Line n** is the interval
   [A_n, A_{n+1}).
3. For any access a in line n, its **in-line cycle number** is s(a) = (t(a) − A_n) / S.
   **Even** means s even, and **odd** means s odd.

The video output is not used for this frame. hsync passes a line doubler, and its relation
to the memory cycles is not stated on any page.

**How the test checks the anchor:**
- **Precondition F2 (the four refresh slots).** In every refresh group in the observed
  frames, the first four accesses lie at 0, 2, 4 and 6 memory cycles from the first one.
  These are the four hatched *"MEMORY REFRESH DMA TIME"* slots of Fig. 6-9 (p.195), drawn on
  every second slot from the first one, which the figure's footnote calls cycle 0; p.194
  lists *"4 cycles for memory refresh"*. F2 asserts the four slots and their drawn offsets.
  It does not tell four evenly spaced accesses of another channel in unattributed memory from
  a refresh group; F3, which requires every group to start one line after the previous one,
  rules out a spurious group. F2 and F3 together anchor A_n: the first access of each group
  is the figure's first refresh slot, before any parity is graded from it. The offsets are
  read off the figure; the figure does not print them as numbers.
- **Precondition F2-COUNT (exactly four).** In every refresh group in the observed frames, in
  the same runs and frames as F2, the group holds exactly four accesses. p.194 lists *"4 cycles
  for memory refresh"*. F2 reads the first four accesses of a group only and so accepts a group
  with more; F2-COUNT is the line that rejects it. A group with more than four accesses on a
  machine that follows the pages does not exist: a fifth access would be another agent's access
  to memory no block declares (D7).
- **Precondition F3.** For every pair of consecutive groups in the observed frames,
  A_{n+1} − A_n equals 227·S or 228·S (the values p.23 names; p.194 and p.304 average 227.5,
  see D4).
- **Contrast for F2 and F2-COUNT.** None, impossible. Fig. 6-9 and p.194 allocate four refresh
  cycles to every line, at fixed positions; DMACON has no enable bit for refresh (Appendix A
  p.278), and no register and no instruction moves a refresh slot or adds one. Any other access
  that falls into memory no block declares joins the refresh set R, but that is the attribution
  by exclusion (D7) at work, not a change of refresh, and no run uses it. The comparators have
  a synthetic self-test, SELFTEST-F2: a group whose second slot is moved to the third memory
  cycle, a group of three and a group of five (the four slots and a fifth entry). F2 must fail
  the first two and F2-COUNT the last two. F2 accepts the group of five, which is why F2-COUNT
  exists.
- **Contrast for F3 (run C0-htotal).** C0, but S2 writes HTOTAL = `$E0` and BEAMCON0 = `$00A0`
  (VARBEAMEN, PAL kept), which the Enhanced Chip Set offers (Appendix A p.280: HTOTAL,
  *"Highest number count, horiz. line (VARBEAMEN=1)"*). The line then ends at count `$E0`, two
  counts before the `$E2` of p.23, so each line is 225 memory cycles long. The Copper list is
  L0, no channel but refresh runs, the CPU sleeps in STOP, and every access keeps its
  attribution. S is a property of the memory cycle, not of the line length: it is measured in
  the normal C0 run, as in the normal runs, and applied to this run unchanged. F1, F2 and
  F2-COUNT hold. A_{n+1} − A_n is 225·S in every observed pair, and F3 fails.

### 2.4 Frames and observation spans

A **restart fetch** is the Copper's read of the word at the address held in COP1LC
(p.25; Appendix A p.276: *"COP1LC is automatically used at the beginning of each vertical
blank time"*). Frame k is the interval from the k-th restart fetch after the program's
DMACON write in step S4 (§2.6) to the next one.

The **observed frames** of a run are frames 3, 4 and 5. A check covers every line of the
observed frames whose anchor A_n lies inside them. Accesses outside those lines are not
graded.

### 2.5 Video measurement (propositions P8–P12)

- **Background.** COLOR00 is `$000` in every sprite run. p.46: *"COLOR00 is always reserved
  for the background color. The background color shows in any area on the display where there
  is no other object present and is also displayed outside the defined display window"*. The
  only bitplane holds zeros, so every output pixel whose colour is not `$000` belongs to a
  sprite.
- **Colour value.** p.46, Table 3-3: bits 11–8 of a colour register are red, 7–4 green, 3–0
  blue. The output's 4-bit red, green and blue must equal those nibbles.
- **Sprite pixel width p.** On the first line of the reference image (named in each
  proposition), all 16 sprite pixels are set. That line's run of non-background output pixels
  has length W. Then p = W/16. p.98 and p.101 fix the width: *"Sprites are 16 pixels wide"*.
  **Precondition Q2:** W > 0 and W is divisible by 16. **Contrast:** make the low data word of
  that line `$7FFF`. Its leftmost pixel then becomes transparent (p.108: *"binary 00 always
  means transparent"*), W becomes 15·p, and Q2 fails.
- **Beam lines.** Output lines map to beam lines through the harness's line doubler, which
  produces k_v output lines per beam line (a fixed harness property). Vertical distances are
  measured in output lines and divided by k_v. **Precondition Q3:** every measured image top,
  and every image height, in output lines is a multiple of k_v. There is no stimulus contrast:
  no page describes a stimulus that puts a sprite edge between beam lines. The checker is
  verified by a self-test on a synthetic frame whose edge is shifted by one output line.
- **Output frames used.** From the third complete output frame after S4 up to the last
  complete output frame before S7.

### 2.6 Common program skeleton

Every run's 68000 program executes these steps in this order, on each of the two CPU cores
separately. Register addresses are those of Appendix B.

- **S0 — quiesce.** Write `$7FFF` to DMACON (`$096`), INTENA (`$09A`), INTREQ (`$09C`) and
  ADKCON (`$09E`). Bit 15 = 0 clears every bit written as 1: Appendix A p.278 (DMACON:
  *"Determines if bits written with a 1 get set or cleared"*), Appendix A p.280 (INTENA,
  INTREQ), and p.249 (ADKCON). Clearing ADKCON also clears the audio modulation bits 7–0
  (p.249). Every block §2.7 places below logical `$080000` is unreachable to a CPU write
  while the CIA-A boot overlay covers it, so S0 also releases the overlay: write DDRA
  (`$BFE201`) ← `$03`, then PRA (`$BFE001`) ← `$00` (p.223: *"The OVL bit in one of the 8520
  Chips disables the overlay"*; Appendix F p.339: DDRA *"1-output (set to 0x03)"*, PRA bit 0
  OVL).
- **S1 — prepare Chip memory**, by CPU write, as §2.7 and §2.9 describe for the run: the
  Copper list(s) and every sprite structure the run's own Copper list points at.
- **S2 — write the run's registers** (§2.9). Every sprite and bitplane pointer the run's
  Copper list rewrites is also written here once, with the same value (p.114: *"all eight
  sprite pointers must be initialized to either a real sprite or a safe null sprite"*). The
  programs do this for the pointers §2.7 lists as written; for the others the Copper list's own
  pointer writes in every vertical blanking interval set the same values before the observed
  frames.
- **S3 — install the Copper list.** Do a long-word write of the list address to COP1LCH
  (`$080`, p.25: *"you write the 18-bit address by moving one long word to the register
  whose name ends in "H"*"). Then write the strobe COPJMP1 (`$088`). p.30: *"you must
  initialize one of the Copper location registers (COP1LC or COP2LC) and write to its strobe
  address before Copper DMA is turned on."* Appendix A p.276 says the same.
- **S4 — enable DMA.** Write DMACON ← `$8000` | DMAEN (`$0200`) | COPEN (`$0080`) | the
  run's channel bits (§2.9). For disk runs this is step 1 of p.247.
- **S5 — interrupt.** Install a level-3 interrupt handler. It adds 1 to a frame counter,
  writes INTREQ ← `$0020` to clear VERTB (Appendix A p.280: *"These status bits are not
  automatically reset … and must be reset when desired"*), and returns. Then write
  INTENA ← `$C020` (SET, INTEN, VERTB). The programs write the handler's vector (`$00006C`) in
  S0, once the overlay is released, and not here; INTENA stays cleared from S0 until this
  step, so no exception can occur before the vector holds the handler.
- **S6 — wait with the CPU off the bus.** Loop on `STOP #$2000` until the counter reaches 7.
  In disk runs, once the counter reaches 2, execute steps 2–4 of p.247: DSKLEN (`$024`) ←
  `$4000`, then DSKLEN ← the run's value, then DSKLEN ← the same value again. Then read
  DSKBYTR (`$01A`), store the value in the results area, and continue the loop.
- **S7 — finish.** Write DMACON ← `$7FFF`. In disk runs, write DSKLEN ← `$4000` (p.247
  step 5: *"After the DMA is complete, set the DSKLEN register back to $4000"*). Write
  INTENA ← `$7FFF`, then store a completion word.

**Precondition C1 (program finished).** The completion word is present. This holds for
every run. Its contrast is any run with the STOP loop's exit condition removed: the
completion word is then missing.

### 2.7 Memory layout rules

- Every block lies below `$80000`, so the same layout works with the OCS selection. Appendix
  A p.273 and p.279 give the location and pointer registers "high 3 bits, high 5 bits if ECS".
- Every block is word aligned (p.104, p.138, p.246) and disjoint from all others. As built,
  a block's declared range is its data and zero words after the data, never before it: an
  audio block is 512 bytes for 16 data words, a sprite block 1024 bytes for 86 data words,
  the null-sprite block 4096 bytes, a plane block 32768 bytes for the 10240 (low resolution)
  or 20480 (high resolution) bytes the display fetches. The disk block is exactly its 8000
  data words. No block has padding on its low side. In the observed frames no channel's read falls outside its declared block: such a read would
  join the refresh set and lengthen a refresh group (F2-COUNT) or start one of its own (F3), and
  both hold in every run; no write below `$080000` falls in the observed frames (§2.2, measured
  in C0, C2a, C6, S2 and C7a).
- The Copper block holds only Copper lists.
- The builder chooses the addresses within these rules.
- **As built, the pointer writes of S2 and the lists differ from §2.6 and §2.8 as follows; no
  result depends on any of it.**
  - S2 writes SPR0PT–SPR7PT in C5, C6 and its variants, S1 and S2, and none in C3, C3-nospren,
    C7a and C7b. The Copper list writes all eight in every vertical blanking interval, before
    the first observed frame.
  - S2 writes the pointers of the planes the run fetches: BPL1PT–BPL4PT in C5, C6 and its
    variants and in C7b, BPL1PT–BPL6PT in C7a, and none beyond the run's last plane in C4-n.
  - L5 writes BPL5PT and BPL6PT ← 0 in C5, C6 and its variants and C7b, not the address of
    planes 5 and 6 (the same reason), and the addresses in C7a.
  - The vector of the level-3 handler is written in S0 (§2.6, S5).

### 2.8 Copper lists

Instructions are written as `DC.W IR1,IR2` (Appendix A p.274–275). MOVE: IR1 = register
address with bit 0 = 0, IR2 = data (p.21). WAIT: IR1 = VP<<8 | HP | 1, IR2 = `$FFFE`, with
BFD = 1 (p.22: *"Normally, this bit is a 1"*) and every compare-enable bit set. SKIP: the
same with IR2 bit 0 = 1 (Appendix A p.274–275). HP is even; p.23: *"The least significant
bit is not used in the comparison"*. **END** is `DC.W $FFFF,$FFFE` (p.23, p.30).

- **V_spr** — for x = 0..7: MOVE SPRxPTH (`$120`+4x) ← high word, then MOVE SPRxPTL
  (`$122`+4x) ← low word of that sprite's structure address.
- **V_bpl(n)** — for y = 1..n: MOVE BPLyPTH (`$0E0`+4(y−1)) ← high word, then MOVE BPLyPTL
  ← low word of plane y's block address.
- **V_all** — V_spr followed by V_bpl(6).

Pointer rewrites come first in every list, so they happen early in vertical blanking. p.110:
*"If you want the sprite to be displayed in subsequent display fields, you must rewrite the
contents of the sprite pointers during each vertical blanking interval"*. p.63 says the same
for bitplane pointers.

| list | contents |
|---|---|
| **L0** | END |
| **L3** | V_spr; END |
| **L4(n)** | V_bpl(n); END |
| **L5** | V_all; MOVE COP2LCH (`$084`) ← hi(L5b); MOVE COP2LCL (`$086`) ← lo(L5b); then for i = 0..112: WAIT (VP = `$40`+i, HP = 2i), i.e. `DC.W ((($40+i)<<8)|(2i)|1),$FFFE`, followed by three × `DC.W $0180,$0000` (MOVE COLOR00 ← `$000`); then `DC.W $B401,$FFFE` (WAIT VP=`$B4`, HP=0); `DC.W $B441,$FFFF` (SKIP, VP=`$B4`, HP=`$40`); `DC.W $0180,$0000`; `DC.W $008A,$0000` (write the strobe COPJMP2, p.26: *"any MOVE instruction that addresses COPJMP2 strobes this indirect address into the program counter"*). |
| **L5b** | `DC.W $B821,$FFFE` (WAIT VP=`$B8`, HP=`$20`); four × `DC.W $0180,$0000`; END |
| **L5'** | L5 with the WAIT for i = 32 changed from `DC.W $6041,$FFFE` (HP=`$40`) to `DC.W $6043,$FFFE` (HP=`$42`) |
| **L_S1** | V_spr (sprite 0 → structure Z, sprites 1–7 → null block); V_bpl(1) (→ zero plane); `DC.W $01A2,$0F00`; `DC.W $01A4,$00F0`; `DC.W $01A6,$000F` (COLOR17–19 ← red, green, blue); END |
| **L_S1c** | as L_S1, but before END insert `DC.W $4801,$FFFE` (WAIT VP=`$48`, the line after the last line of use 1), `DC.W $01A2,$0FF0`, `DC.W $01A4,$00FF`, `DC.W $01A6,$0F0F` |
| **L_A** | V_spr (sprite 0 → A, sprite 2 → R, others → null); V_bpl(1); COLOR17–19 as in L_S1; `DC.W $01AA,$0FF0`; `DC.W $01AC,$00FF`; `DC.W $01AE,$0F0F` (COLOR21–23); MOVE COP1LCH (`$080`) ← hi(L_B); MOVE COP1LCL (`$082`) ← lo(L_B); END |
| **L_B** | as L_A with sprite 0 → B and COP1LC ← L_A |

L_A and L_B alternate from frame to frame. Each one writes the other's address into COP1LC,
and the next restart uses COP1LC (p.25; p.26: *"The Copper can write its own location
registers and strobe addresses"*).

### 2.9 Configurations

Every configuration uses the §2.6 skeleton. "DMACON bits" are set in S4 together with
DMAEN | COPEN. BLTEN is never set.

| id | DMACON bits | list | registers written in S2 (and data) |
|---|---|---|---|
| **C0** | — | L0 | none |
| **C1a** | AUD0EN–AUD3EN (`$000F`) | L0 | for x = 0..3, in the order of p.137 steps 3–6: AUDxLCH/L ← audio block x (long write, p.139); AUDxLEN ← 16 (p.139); AUDxVOL ← 64 (p.139–140); AUDxPER ← 124. Each block holds 16 words of samples, for example Table 5-1 (p.138). p.140: *"The minimum period value you should use is 124 ticks per sample NTSC (123 PAL)"*. |
| **C1b** | as C1a | L0 | as C1a with AUDxPER ← 447 (the example value of p.143) |
| **C2a** | DSKEN (`$0010`) | L0 | ADKCON ← `$9100` (SET, MFMPREC, FAST: p.249 *"Value of 1 selects two microseconds per bit cell (usually MFM)"*); DSKPTH/L ← disk block (one long write, p.246). The S6 value is `$C000`\|8000 (DMAEN, WRITE, LENGTH = 8000 words, p.246 Table 8-6). The disk block holds 8000 words. |
| **C2b** | DSKEN | L0 | as C2a with the S6 value `$8000`\|8000 (read). WORDSYNC stays 0 (D9). |
| **C3** | SPREN (`$0020`) | L3 | eight structures, sprite x in sprite block x (x = 0..7). Use 1: VSTART `$50`, VSTOP `$64`, HSTART `$70`+`$18`·x. Use 2: VSTART `$80`, VSTOP `$94`, same HSTART. Then the end words `$0000,$0000`. Control words are built per pp.107, 128: SPRxPOS = VSTART<<8 \| HSTART>>1; SPRxCTL = VSTOP<<8 \| (HSTART & 1). The data words are any non-zero values, two per line (p.105 Table 4-1). |
| **C4-n** (n = 1..6) | BPLEN (`$0100`) | L4(n) | BPLCON0 (`$100`) ← (n<<12) \| `$0200` (p.63: bits 12–14 = number of bitplanes; bit 15 HIRES = 0; bit 2 LACE = 0, p.64); BPLCON1 ← 0; BPL1MOD, BPL2MOD ← 0 (p.64 step 5); DIWSTRT ← `$2C81`; DIWSTOP ← `$2CC1` for PAL or `$F4C1` for NTSC (p.59); DDFSTRT ← `$0038`; DDFSTOP ← `$00D0` (p.60). Plane blocks of 40 bytes × 256 lines, any data. |
| **C4-4w** | BPLEN | L4(4) | as C4-4 with DDFSTRT ← `$0030`, DDFSTOP ← `$00C8` (p.60: *"DDFSTRT = DDFSTOP - (8 * (word count - 1)) for low resolution"*, 20 words; a multiple of 8 as p.60 recommends) |
| **C5** | — | L5 | every register of C6, so the list does the same MOVEs; only DMACON differs |
| **C6** | AUD0EN–AUD3EN \| DSKEN \| SPREN \| BPLEN | L5 | C1a + C2a + C3 + C4-4 together |
| **C7a** | BPLEN | L5 | the registers of C4-6 |
| **C7b** | BPLEN | L5 | BPLCON0 ← `$C200` (HIRES, 4 planes, p.63); DDFSTRT ← `$003C`; DDFSTOP ← `$00D4` (p.60, "normal high resolution"); modulos 0; plane blocks of 80 bytes × 256 lines |
| **S1** | SPREN \| BPLEN | L_S1 | BPLCON0 ← `$1200`; BPL1MOD ← 0; DIWSTRT/DIWSTOP/DDFSTRT/DDFSTOP as C4-1; plane 1 all zeros; COLOR00 ← `$000`; structure Z (below) in sprite block 0; null block for sprites 1–7 |
| **S2** | SPREN \| BPLEN | L_A (S3 installs L_A) | as S1; structures A and B in sprite block 0; R in sprite block 2 (below) |

**Structure Z (S1, sprite 0, four uses).** Each data line is written low word first, then
high word (p.105, Table 4-1). The first data line of every use is `$FFFF,$0000`: all 16
pixels have value 01.

| use | control words | height | data lines 2.. (low, high) | pixel values, left to right |
|---|---|---|---|---|
| U1 | `$4050,$4800` (VSTART `$40`, HSTART `$A0`, VSTOP `$48`) | 8 | 7 × `$F0F0,$FF00` | 3333 2222 1111 0000 |
| U2 | `$5064,$6000` (VSTART `$50`, HSTART `$C8`, VSTOP `$60`) | 16 | 15 × `$0F0F,$00FF` | 0000 1111 2222 3333 |
| U3 | `$6848,$6C00` (VSTART `$68`, HSTART `$90`, VSTOP `$6C`) | 4 | 3 × `$CCCC,$F0F0` | 3322 1100 3322 1100 |
| U4 | `$7878,$8400` (VSTART `$78`, HSTART `$F0`, VSTOP `$84`) | 12 | 11 × `$AAAA,$CCCC` | 3210 3210 3210 3210 |
| end | `$0000,$0000` | | | |

A pixel's value is 2·(bit of the high word) + (bit of the low word), with the most
significant bit leftmost. p.107: *"The bits in the high order color descriptor word
contribute the leftmost digit of the binary color selector number for each pixel"*; p.124:
*"most significant (leftmost) bit first"*. Every gap between uses is at least 8 lines (D12).
Every HSTART lies inside the window `$81`–`$1C1` (p.59, p.99).

**Structures for S2.** A: `$6050,$6A00` (VSTART `$60`, HSTART `$A0`, height 10). B:
`$6A5C,$7400` (VSTART `$6A`, HSTART `$B8`, height 10). Both have the same ten data lines: the
first is `$FFFF,$0000`, the rest are `$0F0F,$00FF`. Both end with `$0000,$0000`. R (sprite 2,
the reference): `$4048,$4600` (VSTART `$40`, HSTART `$90`, height 6), with six data lines
`$FFFF,$0000`, followed by the end words. Every HSTART of S2 lies inside the window
`$81`–`$1C1` (p.59), so no image is clipped.

**Null block.** `$0000,$0000` (p.108: *"the value 0 should be used for both words when ending
the usage of a sprite"*).

### 2.10 Modes and cores

The default mode is PAL with the Enhanced Chip Set. C0, C6 and S1 also run with NTSC/ECS,
PAL/OCS and NTSC/OCS. The NTSC runs of C0 and C6 exercise the alternating 228/227-cycle lines
of p.23. Every run is made on both CPU cores and graded per core.

### 2.11 Preconditions gate their lines

A proposition's line is graded only when every precondition it lists has been evaluated and
printed before it, with the comparator the normal run uses, and holds. When one fails the
line prints `NOT GRADED` and the failed names, and counts as neither PASS nor FAIL. No
precondition is part of a graded expression.

- **Observation preconditions of a run:** it reached DONE (C1), its restart fetches delimit
  the observed frames (§2.4), and F1, F2, F2-COUNT and F3 hold. They are evaluated on the run's own
  declared blocks and gate every line that grades that run: P2–P5, P7, K2b, K3, K4, K5, K6,
  T1, T2 and T3. **P1 is the exception:** its gate has C1, the restarts, F1 and F3 of its runs but
  neither F2 nor F2-COUNT, which grade the very accesses P1 grades (P1.4). K2a reads a
  stored value and observes no run. A run that has not finished, or has no observed frames,
  leaves its own F1, F2, F2-COUNT and F3 lines NOT GRADED.
- **Contrasts for the observation preconditions** (the lines K2b, K3, K4, K5, K6, T1, T2 and
  T3, which all grade C6, each with C6 replaced in turn by one of three runs):
  C6-nofinish, C6 with the wait loop's exit condition removed (C1 fails); C6-htotal, C6 with
  HTOTAL = `$E0` and BEAMCON0 = `$00A0`, as in §2.3's contrast for F3 (every line is 225
  memory cycles and F3 fails); and C6-norestart, C6 with COP1LC pointing at L5b, so that the
  Copper never reads the word the run declares as its list start (no restart fetch is
  found). The line prints `NOT GRADED` with the failed name. F1, F2 and F2-COUNT have no
  such contrast: no program moves a DMA channel to another phase or changes the refresh
  slots (§2.1, §2.3).
- **Per proposition:** A1 gates P1–P5; K2a and K2b gate P2; K3 gates P3; K4 gates P4; K5
  gates P5; K6 gates P7; T1, T2 and T3 gate P7. A K line is NOT GRADED when the runs it
  grades have no observed frames.
- **Comparators follow the run.** K2b, K3, K4, K5 and K6 read the declared blocks and list
  starts of the run they grade.
- **A channel's own activity.** K2b and K3 count a channel's accesses without the words of
  the Copper's lists (L5, L5b): the Copper reads them whether or not the channel runs, so
  they cannot show the channel active.
- **Populations.** "Every" needs a non-empty population: an empty one fails its line. T1
  therefore requires every segment of C5 to hold at least one Copper access in every
  observed frame, and T2 requires at least one line spanned by a segment.

---

## 3. Propositions

### P1 — Refresh uses only the even memory cycles

1. **Proposition.** Every memory-refresh access lies on the even-numbered memory cycles of
   its line. Claims `ch2-about-the-copper.c24` and `COP2-about-the-copper.c29`, p.20;
   Appendix A p.275 says the same.
2. **Stimulus.** Refresh has no stimulus of its own. DMACON has no enable bit for it
   (Appendix A p.278), and p.194 allocates it in every line (*"The time-slot allocation per
   horizontal line is: 4 cycles for memory refresh …"*). It is graded in every run: C0, C1a,
   C1b, C2a, C2b, C3, C4-1…6, C4-4w, C5, C6, C7a, C7b, S1, S2, in every mode of §2.10.
3. **Observation.** The refresh set R (§2.2) of every line of the observed frames (§2.4),
   placed in frame F (§2.3).
4. **Expected value.** For every access a in R: s(a) is even. The sentence is p.20: *"This
   prevents collision with audio, disk, refresh, sprites, and most low resolution display DMA
   access, all of which use only the even-numbered memory cycles."* The quantifier "only" is
   graded over every refresh access of every line in the observed frames of every run and
   mode; one line does not establish it. Within each line the first refresh access has s = 0
   by construction, and F2 puts the next three at s = 2, 4 and 6; F2 and F2-COUNT on the same
   run grade the same accesses (in every run P1's population is four per group). Neither
   gates P1 (P1.5): a refresh access on an odd cycle turns P1 red and F2 red in the same run.
   P1 also grades what F2 and F2-COUNT do not: a write attributed to R, and the part of a
   group that starts before the observed frames. R is defined by exclusion (D7), so an access
   of another agent to memory no block declares falls into R.
5. **Preconditions.** C1 (§2.6), the restart fetches (§2.4), F1 and F3 (§2.3) of every run it
   covers, and A1 (§2.2). F2 and F2-COUNT are graded on their own lines before P1, but they
   are not P1's preconditions (§2.11): they decide the same accesses P1 does, so a gate on
   them would leave P1 unable to fail through its own clause. P1 is NOT GRADED when one of
   its own preconditions fails in any run it covers.
6. **Contrasting case.** None for a run, impossible. The four refresh slots of every line
   are fixed by the hardware (P1.2: p.194 allocates four refresh cycles to every line, Fig.
   6-9 draws them at fixed slots, and Appendix A p.278 gives DMACON no enable bit for
   refresh), and no register and no instruction moves one to an odd cycle. A program that
   puts another agent's access into memory no block declares (D7) makes P1 red through a
   false attribution, not a changed refresh. A read there is a displaced or additional access
   in a refresh group, F2's or F2-COUNT's to reject, or starts a group of its own and breaks
   F3; a CPU write there breaks F1 (the CPU's own phase, §2.1); a DMA write there, which only
   the blitter's D channel could make and no run enables (§2.2), turns P1 red with C1, the
   restarts, F1 and F3 met (P1.4: P1 grades writes attributed to R). P1's red path is therefore shown by a self-test on a synthetic trace, **SELFTEST-P1**:
   fourteen lines of 227 memory cycles, each with the four slots at 0, 2, 4 and 6, and a restart
   fetch in every second line; the second slot of one observed line is moved to the third
   memory cycle. C1, the restarts, F1, F3 and F2-COUNT hold in it, F2 fails, and P1's comparator
   under P1's own gate fails; the trace without the moved slot passes, and the gate with F2
   and F2-COUNT would have left P1 NOT GRADED. The contrasts for F1–F3, A1 and C1 are given
   in §2.1–2.3 and §2.6.
7. **Reference frame.** Frame F: cycle 0 is the line's first refresh access, as Fig. 6-9
   draws it (D3). It is checked by F2 and F3.

### P2 — Disk DMA uses only the even memory cycles

1. **Proposition.** Every disk-DMA access lies on the even-numbered memory cycles of its
   line. Claims `ch2-about-the-copper.c23` and `COP2-about-the-copper.c28`, p.20. Both claims
   say "only", over the disk channel in both directions. P2's graded line covers the write
   direction (C2a, C6) and cites neither id: the read direction (C2b) shows no disk-block
   access in the observed frames, so it is declined (D9), and the two ids are printed as
   RECORDED lines saying so, not as graded ones.
2. **Stimulus.** C2a (write direction: the channel reads memory), C2b (read direction: the
   channel writes memory), and C6. The disk start follows p.247: *"1. Enable disk DMA in the
   DMACON register … 2. Set DSKLEN to $4000 … 3. Put the value you want into the DSKLEN
   register. 4. Write this value again into the DSKLEN register. This actually starts the
   DMA. 5. After the DMA is complete, set the DSKLEN register back to $4000"*. Step 1 happens
   in S4, steps 2–4 in S6 after the second VERTB, and step 5 in S7.
3. **Observation.** Every access in the disk block during the observed frames, reads and
   writes alike. p.247 gives the rate: *"As each data word is transferred, the length value is
   decremented"*. At 2 µs per bit cell (FAST = 1, p.249), the 8000 words last through every
   observed frame.
4. **Expected value.** Every disk-block access has even s (p.20, quoted in P1.4). "Only" is
   graded over every access of both directions in every observed frame, both alone (C2a, C2b:
   every odd cycle free) and under full load (C6).
5. **Preconditions.**
   - F1–F3, F2-COUNT, A1, C1.
   - **K2a:** the stored DSKBYTR value has bit 14 (DMAON) = 1, and bit 13 (DISKWRITE) = 1 in
     C2a and C6 or 0 in C2b (p.248, Appendix A p.278; see D6).
   - **K2b:** the disk block receives at least one access, not counting the words of the
     Copper's lists, in each observed frame of each run. p.194 allots at most 3 disk cycles
     to a line and p.246 gives the transfer 8000 words, so a transfer that is running cannot
     finish within the three observed frames. If K2b fails in C2b only, the read direction
     is reported as not exercised (D9).
6. **Contrasting case.** None for P2 itself, impossible. Disk DMA is allocated to slots that no
   register moves to an odd cycle (p.20, p.194); pointing DSKPT elsewhere changes which words
   it reads, not when. A run that points DSKPT at the Copper's list and declares that span as
   the disk block would turn the check red through the attribution (the Copper's reads counted
   as disk), not through the channel, and no run uses it. The preconditions have contrasts:
   - **Contrast for K2a:** C2a with DSKBYTR read right after DSKLEN ← `$4000` (p.247 step 2),
     before the two writes that start the transfer, so DSKLEN bit 15 is clear when it is read.
     Both p.248 and Appendix A p.278 predict DMAON = 0 in the value read. The transfer is then
     started as in C2a: the disk block keeps its accesses, K2b and P2's population are
     unchanged, and only K2a fails.
   - **Contrast for K2b:** C2a with DSKPTH/L at `$00C000`, outside the declared disk block.
     Both DSKLEN writes keep bit 15 set, so DMAON = 1 and K2a holds, the transfer runs, and
     none of its accesses falls into the disk block. The run's table declares the moved
     transfer (`$00C000` for 8000 words) as a block of its own, so its accesses do not join the
     refresh set and F1–F3 and F2-COUNT hold; only K2b fails.
7. **Reference frame.** Frame F.

### P3 — Audio DMA uses only the even memory cycles

1. **Proposition.** Every audio-DMA access, of all four channels, lies on the even-numbered
   memory cycles of its line. Claims `ch2-about-the-copper.c22` and
   `COP2-about-the-copper.c27`, p.20.
2. **Stimulus.** C1a (period 124), C1b (period 447), and C6. The order follows p.137:
   *"3. Set registers telling the system where to find the data and the length of the data.
   4. Select the volume … 5. Select the sampling period … 6. Select an audio channel and start
   up the DMA."* DMA starts per p.144: *"When both DMAEN and AUDxEN are set, the DMA will
   start for channel x."*
3. **Observation.** Every access in audio blocks 0–3 during the observed frames. The
   16-word samples repeat, and p.139 explains why: *"Each time the system retrieves the last
   audio word from the data area, it uses the contents of these location registers to again
   find the start of the data"*. The location-reload fetches are therefore included.
4. **Expected value.** Every audio-block access has even s (p.20). "Only" is graded over
   every fetch of every channel at two periods, alone and under full load.
5. **Preconditions.** F1–F3, F2-COUNT, A1, C1. **K3:** each of the four channels is active:
   it has at least one access, not counting the words of the Copper's lists, in each
   observed frame of each run (p.144: *"When both DMAEN and AUDxEN are set, the DMA will
   start for channel x"*).
6. **Contrasting case.** None for P3 itself, impossible. Audio DMA is allocated to slots that no
   register moves to an odd cycle (p.20, p.194); AUDxLC and AUDxLEN change which words a channel
   reads, not when. A run that aliases a channel with the Copper's list and declares the span
   as audio would turn the check red through the attribution, not through the channel, and no
   run uses it. The precondition has a contrast:
   - **Contrast for K3:** C1a without AUD0EN in the S4 write. Channel 0 then has no accesses
     in any observed frame, and K3 fails.
7. **Reference frame.** Frame F.

### P4 — Sprite DMA uses only the even memory cycles

1. **Proposition.** Every sprite-DMA access, of all eight channels and of every kind
   (control words, data words, end words, and the control words of a reuse), lies on the
   even-numbered memory cycles of its line. Claims `ch2-about-the-copper.c25` and
   `COP2-about-the-copper.c30`, p.20.
2. **Stimulus.** C3 (every sprite channel with two uses), C6, S1 and S2. Sprite display
   follows p.109: *"1. Decide which of the eight sprite DMA channels to use … 2. Set the
   sprite pointers … 3. Turn on sprite direct memory access … 4. For each subsequent display
   field, during the vertical blanking interval, rewrite the sprite pointers."* Step 4 is done
   by V_spr.
3. **Observation.** Every access in sprite blocks 0–7 and in the null block during the
   observed frames.
4. **Expected value.** Every sprite-block access has even s (p.20). "Only" is graded over all
   eight channels and every kind of fetch, in every observed frame, alone (C3) and under full
   load (C6).
5. **Preconditions.** F1–F3, F2-COUNT, A1, C1. **K4:** in each observed frame of C3 and C6, every word
   of each of the eight structures is read at least once, including the use-2 control words,
   which are fetched in the VSTOP line of use 1. p.108: *"When the vertical position of the
   beam counter is equal to the VSTOP value in the sprite control words, the next two words
   fetched from the sprite data structure are written into the sprite control registers"*.
6. **Contrasting case.** None for P4 itself, impossible. Sprite DMA is allocated to slots that
   no register moves to an odd cycle (p.20, p.194); the sprite pointers and the data structure
   change which words a channel reads, not when. A run in which the Copper executes words that
   a sprite channel reads as data, with that span declared as sprite, would turn the check red
   through the attribution, not through the channel, and no run uses it. The precondition has
   a contrast:
   - **Contrast for K4:** C3 without SPREN. No sprite accesses occur, and K4 fails.
7. **Reference frame.** Frame F.

### P5 — Most low-resolution display DMA uses only the even memory cycles

1. **Proposition.** In a low-resolution display, every fetch for bitplanes 1–4 lies on the
   even-numbered memory cycles, and in every line even-cycle bitplane fetches outnumber
   odd-cycle ones. Claims `ch2-about-the-copper.c26` and `COP2-about-the-copper.c31`, p.20,
   read together with Fig. 6-9 (p.195), Fig. 6-11 (p.197), p.196 and Appendix A p.275 (D2).
2. **Stimulus.** C4-1 through C4-6, C4-4w, and C6. The steps follow p.63–64: number of
   bitplanes and HIRES in BPLCON0, pointers, display window DIWSTRT/DIWSTOP, data fetch
   DDFSTRT/DDFSTOP, modulos, Copper rewrite of the pointers. Bitplane DMA is enabled together
   with the Copper, as in the example on p.30.
3. **Observation.** Every access in plane blocks 1–n during the observed frames. Per line,
   the counts of bitplane fetches with even s and with odd s.
4. **Expected value.**
   - (a) Every access in plane blocks 1–4 has even s, in every configuration with 1 to 6
     low-resolution planes and at both fetch starts. Sources: p.20, *"most low resolution
     display DMA access … use only the even-numbered memory cycles"*; Appendix A p.275;
     Fig. 6-9 and Fig. 6-11, which draw planes 1–4 on the parity of refresh, disk, audio and
     sprites; p.196, *"If the display contains four or fewer low resolution bitplanes, the
     68000 can be granted alternate memory cycles"*.
   - (b) "Most": in every line with bitplane fetches, the number of bitplane fetches with even
     s is greater than the number with odd s.
   - Not graded: the parity of planes 5 and 6, and of high-resolution fetches (D2).
5. **Preconditions.** F1–F3, F2-COUNT, A1, C1. **K5:** every enabled plane has fetches in at least
   as many lines of every observed frame as the display window is high. The programs write
   DIWSTRT `$2C81` and DIWSTOP `$2CC1`; p.59, Table 3-9, gives PAL VSTART `$2C` and VSTOP
   `$2C` (= `$12C`), so the window is `$12C` − `$2C` = 256 lines high. That every line of
   the window is fetched is Appendix A p.277: *"The vertical bitplane DMA timing is identical
   to the display windows described above."*
6. **Contrasting case.** None for P5 itself, impossible. The fetch slots of the low-resolution
   planes 1–4 are fixed on the even cycles (Fig. 6-9, Fig. 6-11), and DDFSTRT, DDFSTOP, BPLCON1
   and the modulos change which words and lines are fetched, not the parity of a slot. A
   high-resolution display fetches in every slot (D2), but P5 is stated for low resolution, so
   that run breaks the premise and not the clause. A run that aliases plane 1 with the Copper's
   list would turn the check red through the attribution, not through the channel, and no run
   uses it. The precondition has a contrast:
   - **Contrast for K5:** C4-4 without BPLEN. No plane fetches occur, and K5 fails.
7. **Reference frame.** Frame F.

### P6 — The Copper requests only the odd memory cycles (withdrawn)

P6 is not graded and has no mutation. It is kept here so that the sentence it would have
graded, the measurement that stands in its place and the reason are in one place.

1. **Proposition.** p.20: *"The Copper is a two-cycle processor that requests the bus only
   during odd-numbered memory cycles."* Appendix A p.275 says the same. p.20 also says MOVE
   and SKIP need two requested memory cycles over four memory-cycle times and WAIT three over
   six (one extra to wake up), and gives the Copper no exception at wake-up, WAIT, MOVE, SKIP or
   the end of a line; a waiting Copper is off the bus (p.19, p.22). The claim id of the
   sentence is `ch2-about-the-copper.c21` (§0).
2. **Why it is withdrawn.** The sentence is about requests. The Chip-memory port shows
   accesses, and an access carries no requester. So the port cannot tell a Copper request from
   a bus transfer held for a second memory cycle: two samples of the same word, direction, data
   and byte enables one memory cycle apart are one held transfer or two requests in adjacent
   memory cycles, and nothing in the log decides. And it cannot tell a Copper access from
   another channel's access to the same word. Naming the requester from Agnus's internal
   signals would take the implementation as its own oracle, and the harness's access record has
   no requester field. A check on accesses by address, with the samples that repeat the
   sample before them removed and the first fetch after S4 left out, would not establish the
   request clause; the raw observations below cannot establish a violation of it either.
3. **What is measured, not graded.** Three MEASURED lines of the normal run:
   - per run, the Copper-block reads in the observed frames on an even in-line cycle: how many
     there are, how many continue the sample directly before them (same word, direction, data
     and byte enables, exactly one memory cycle S later), how many remain, and the in-line
     cycle numbers they fall on. The harness samples the port every 7 MHz period, that is
     twice per memory cycle: S is 8 system-clock cycles and the sampling period 4.
   - the number of Copper-block samples that repeat the sample before them at S/2, all runs.
   - the Copper's first fetch after S4, with the in-line cycle of its line and its parity, for
     C0, C7a, S2 and C2a. The in-line cycle is read only from a regular line: the line from one
     refresh group that has the four slots of Fig. 6-9 to the next such group, 227·S or 228·S
     later. Before the first fetch there is a read of word 0 (measured: two memory cycles before it in every run on fx68k and in C0, S2 and C2a on TG68K; in C7a on TG68K three such reads, one, two and three memory cycles before it), which
     the exclusion of §2.2 puts in R: the plain construction of §2.3 starts a line of its own at
     it and cuts the regular line, so the plain line holding the first fetch is not used. The
     MEASURED line also prints the length of the plain line, and drops the fetch when no regular
     line brackets it.
4. **Held cycle.** The restart fetches that delimit the frames (§2.4) and the lines of the
   MEASURED line above count a bus cycle held over two samples once. The helper that does so
   has a synthetic self-test, HELD-CONTROL: it keeps the first sample of a held cycle, and keeps
   identical samples that are two memory cycles apart or differ in datum, direction or byte
   enables. It has no stimulus mutation: two genuine adjacent identical Copper accesses cannot
   be told from one held bus cycle at this port, so no stimulus produces the case.
5. **Contrasting case.** None; nothing is graded.

### P7 — The channels on the even cycles never delay the Copper

1. **Proposition.** Because the Copper requests only odd cycles, audio, disk, refresh, sprite
   and low-resolution (≤ 4 planes) DMA never collide with it: the Copper's fetches land on the
   same cycles whether those channels run or not. The sentence is on p.20; Appendix A p.275
   says the same. The claim id of that sentence is withdrawn (§0): neither requests nor
   collisions are observable at this port.
2. **Stimulus.** C5 (Copper alone) and C6 (the same list L5, with audio, disk, eight sprites
   and four low-resolution planes running). The CPU is in STOP (S6). The blitter is off.
3. **Observation.** For i = 0..112, the **segment i** of the list is the Copper-block
   accesses after the two words of WAIT i, up to and including the two words of WAIT i+1 (for
   i = 112, of the WAIT at `$B4`). Each access in a segment is recorded as (line offset from
   the line of the segment's first access, s). This is done for each observed frame of both
   runs.
4. **Expected value.** For every i and every observed frame, the list of (line offset, s)
   pairs of segment i is identical in C5 and C6. p.20: *"This prevents collision with audio,
   disk, refresh, sprites, and most low resolution display DMA access"*. Appendix A p.275:
   *"It therefore needs (and has) priority over only the blitter and microprocessor."* No
   collision means the Copper never waits for these channels, so its cycles cannot move. All
   113 segments are graded, in every observed frame, in PAL/ECS: C5 has no run in the other
   modes of §2.10.
5. **Preconditions.**
   - F1–F3, F2-COUNT, C1, K6 (both runs).
   - **T1:** every segment has the same number of Copper accesses in both runs, and every
     segment of C5 has at least one in every observed frame.
   - **T2:** no CPU-block access lies in any line spanned by a segment, in either run, and at
     least one line is spanned by a segment.
   - **T3:** in the lines of the segments in C6, bitplane fetches occur in every line; sprite
     fetches occur in every line of segments i = 16..35 and 64..83 (VP `$50`–`$63` and
     `$80`–`$93`); and audio and disk accesses each occur in the segment lines of every observed
     frame, which shows both channels active while the segments run.
6. **Contrasting case.** None for P7 itself, impossible. The Copper takes only the odd cycles
   and audio, disk, refresh, sprite and low-resolution plane 1–4 DMA take only the even ones
   (p.20, Fig. 6-9, Fig. 6-11); no register and no instruction moves one of those channels to
   an odd cycle, so no program makes an even-cycle channel delay the Copper. The channels
   that take odd cycles (low-resolution planes 5 and 6, high-resolution planes, D2) lie
   outside P7's premise (P7.1: at most four low-resolution planes). Run **C6'** is kept as a
   self-test of the comparator, SELFTEST-P7: C6 with list L5' (WAIT i = 32 at HP `$42` instead
   of `$40`), compared with C5 on L5. p.22: *"The WAIT instruction causes the Copper to wait
   until the video beam counters are equal to (or greater than) the coordinates specified in
   the instruction"*. Segment 32 starts later and its s-values differ, so the comparator finds
   mismatches; the cause is the Copper's own list, which p.22 predicts, and not an even-cycle
   channel, so the run is not a mutation of P7. T1–T3 still hold.
   - **Contrast for T2:** C6 with `STOP #$2000` replaced by a loop that branches to itself.
     The CPU then fetches in every line, and T2 fails.
   - **Contrast for T3:** C6 without SPREN. Sprite fetches are then absent, and T3 fails.
   - **Contrast for T1:** C6 in which the Copper repeats WAIT i = 50's line with the SKIP loop
     of p.33 (p.26: a MOVE to COPJMP2 strobes COP2LC into the program counter) and then
     goes on. Segment 49's third MOVE loads COP2LCL with the address of WAIT 50 (`$1398`);
     segment 50's three MOVEs become `DC.W $7301,$FFFF` (SKIP, VP `$73`, HP 0),
     `DC.W $008A,$0000` (the strobe) and the `DC.W $0180,$0000` they were; segment 51's first
     MOVE loads COP2LCL with the address of L5b again. While the beam is above line `$73` the
     SKIP does not skip and the strobe returns the Copper to WAIT 50; when the beam has
     reached line `$73` the SKIP skips the strobe and the list goes on. Segments 49 and 50
     then hold more Copper accesses than in C5, and every other segment is unchanged: T1
     fails, and K6, T2, T3 and F1–F3 hold.
   - **Contrast for K6:** C6 with the trailer's MOVE of the COPJMP2 strobe (the last two words
     of L5) replaced by `DC.W $FFFF,$FFFE`. The Copper reads every word of L5 and then stops
     (p.23) without jumping to L5b, so L5b is never read and K6 fails; T1, T2, T3 and F1–F3
     hold.
7. **Reference frame.** Frame F, with segments anchored on the list's own WAITs.

### P8 — A sprite DMA channel produces a movable object

1. **Proposition.** The object a sprite DMA channel displays is placed by the position in its
   data structure, and moves by exactly the change in that position from one field to the
   next. Claim `COP2-putting-together-a-copper-instruction-list.c1`, p.27; the position rule
   is from pp.106–107, 113 and 128.
2. **Stimulus.** S2. L_A and L_B alternate by frame. Sprite 0 reads structure A in L_A
   frames and structure B in L_B frames; the two differ only in position (ΔHSTART = +24,
   ΔVSTART = +10). Sprite 2 shows the fixed reference R. p.113: *"A sprite generated in
   automatic mode can be moved by specifying a different position in the data structure. For
   each display field, the data is reread and the sprite redrawn."* The two structures avoid
   rewriting control words while the channel may read them, which p.113 warns against.
3. **Observation.**
   - In each output frame (§2.5): the top-left pixel of sprite 0's image (COLOR17 on its first
     line) and of sprite 2's image (COLOR21 on its first line).
   - At the port: which list's first word was the restart fetch of each frame.
4. **Expected value.** In each output frame, the offset of sprite 0 from sprite 2 is either
   (+16 sprite pixels, +32 lines) for A or (+40, +42) for B. Consecutive frames alternate, and
   the difference between them is ±24 pixels horizontally and ±10 lines vertically. Sources:
   p.107, *"This word (SPR?POS) contains the vertical (VSTART) and horizontal (HSTART) starting
   position for the sprite"*; p.98, *"Sprite position is always defined as though the display
   modes were low resolution and non-interlaced"*. Graded over every pair of consecutive output
   frames used (at least 3).
5. **Preconditions.**
   - C1, Q2 (calibrated on R's first line), Q3.
   - **Q4:** the colours written by the list are pairwise distinct from each other and from
     COLOR00.
   - **Q5:** every word of L_A and L_B is fetched in its frames, and restart fetches alternate
     between L_A and L_B.
   - **Q-ref:** sprite 2's image is at the same output position in every frame used.
6. **Contrasting case.** **S2-still:** L_B also points sprite 0 at A. p.113 then predicts no
   movement, and the alternating-offset check goes red. Q-ref, Q4 and Q5 still hold.
   - **Contrast for Q-ref:** L_B points sprite 2 at the null block. R is then absent in B
     frames, and Q-ref fails.
   - **Contrast for Q4:** COLOR17 written as `$0000`. It then equals COLOR00, and Q4 fails.
   - **Contrast for Q5:** END placed after V_spr in L_A. The rest of L_A (including the
     COP1LC MOVEs) is not fetched, so restarts no longer alternate, and Q5 fails.
7. **Reference frame.** Positions are relative to sprite 2's image in the same output frame,
   in sprite pixels (p, §2.5) and beam lines (k_v, §2.5). No absolute screen position is
   graded.

### P9 — One sprite DMA channel is reused several times in the same field

1. **Proposition.** One sprite DMA channel, following one chained data structure, displays
   four separate images in the same field and reads the words of all four uses in address
   order within that field. Claims `COP2-putting-together-a-copper-instruction-list.c2` and
   `ch2-putting-together-a-copper-instruction-list.c5`, p.27; pp.97, 116–117 and 126–127.
2. **Stimulus.** S1: structure Z (four uses) for sprite 0 alone, and SPR0PT is the only
   pointer that addresses it. The chaining follows p.116: *"To reuse a DMA channel, you replace
   this pair of zero words with another complete sprite data structure, which describes the
   reuse of the DMA channel at a position lower on the screen than the first use. You place the
   two all-zero words at the end of the data structure that contains the information for all
   usages of the DMA channel."*
3. **Observation.**
   - Video: the non-background images in the output frames used.
   - Port: the accesses in sprite block 0 within each observed frame (§2.4).
4. **Expected value.**
   - (a) Every output frame used shows exactly four images of sprite 0. Their tops, relative
     to U1's top, are at 0, +16, +40 and +56 beam lines, and their left edges, relative to
     U1's, at 0, +40, −16 and +80 sprite pixels (the VSTART and HSTART differences of Z).
   - (b) In each observed frame, the words of Z from U1's first control word through the end
     words are read in increasing address order, each exactly once: 8 control words, 80 data
     words and 2 end words. p.110: *"they are incremented by the sprite DMA channel to point
     first to the control words, then to the data words, and finally to the end-of-data
     words"*.
   - "Multiple times" is graded with three reuses (four uses), in every observed frame. "The
     same display field": all four images and all 90 reads fall between two consecutive
     restart fetches.
5. **Preconditions.** C1, Q2 (on U1's first line), Q3, Q4, Q5 (for L_S1). **K9:** in each
   observed frame, sprite block 0 is read from U1's first control word onward.
6. **Contrasting case.** **S1-end:** U2's control words replaced by `$0000,$0000`. p.127:
   *"To stop the sprite after that first data set, the pair of words should contain all
   zeros."* p.108: *"If the VSTART value contained in these words is lower than the current
   beam position, this sprite will not be reused in this display field."* Only U1 appears and
   the reads stop after that pair, so (a) and (b) go red. K9, Q2 and Q3 still hold.
   - **Contrast for K9:** S1 without SPREN. Nothing is read, and K9 fails.
   - The contrasts for Q2–Q5 are as in §2.5 and P8.
7. **Reference frame.** Relative to U1's image in the same output frame, and at the port to
   the frame's restart fetches.

### P10 — The size of a sprite can differ between its reuses

1. **Proposition.** Each use of a reused sprite is displayed with its own height,
   VSTOP − VSTART of that use. Claims `COP2-putting-together-a-copper-instruction-list.c3`
   and `ch2-putting-together-a-copper-instruction-list.c6` (size part), p.27. "Size" can only
   mean height: the width is fixed (p.101, *"Sprites are 16 pixels wide and can be almost any
   height you wish"*).
2. **Stimulus.** S1, whose uses have heights 8, 16, 4 and 12.
3. **Observation.** The height in output lines of each of the four images, divided by k_v.
4. **Expected value.** The heights are 8, 16, 4 and 12 beam lines. p.107: *"The value (VSTOP
   - VSTART) defines how many scan lines high the sprite will be when it is displayed."*
   p.27: *"You can change the size and shape of the reuses of a sprite"*. Graded for all four
   uses in every output frame used.
5. **Preconditions.** C1, Q2, Q3, Q4, Q5, K9. The same four images as in P9(a) must be
   identifiable.
6. **Contrasting case.** **S1-h:** U2's control word 2 becomes `$5C00` (VSTOP `$5C`), and
   four of its data pairs are removed, so the structure stays consistent. p.107 predicts a
   height of 12, the check (expecting 16) goes red, and U1, U3 and U4 are unchanged.
7. **Reference frame.** Beam lines via k_v (§2.5).

### P11 — The shape of a sprite can differ between its reuses

1. **Proposition.** Each use of a reused sprite shows the pixel pattern of its own data
   words. Claims `COP2-putting-together-a-copper-instruction-list.c4` and
   `ch2-putting-together-a-copper-instruction-list.c6` (shape part), p.27.
2. **Stimulus.** S1, whose uses have four different patterns (the table in §2.9).
3. **Observation.** For every displayed line of every image, the 16 sprite pixels, each an
   interval of p output pixels starting at the image's left edge.
4. **Expected value.** For every line: the pixels whose value is 0 show background, and the
   others do not. Pixels of equal value show equal colours, and pixels of different non-zero
   value show different colours. The values come from the line's data words as in §2.9.
   Sources: p.108, *"binary 00 always means transparent"*; p.107 and p.124 on bit order;
   p.101, *"You define a sprite's shape by specifying which pixels actually appear in each of
   the sprite's locations."* Graded for every line of every use, in every output frame used.
   The patterns of U1–U4 differ, so each use shows a different shape.
5. **Preconditions.** C1, Q2, Q3, Q4, Q5, K9.
6. **Contrasting case.** **S1-s:** U2's data line 5 becomes `$F0F0,$FF00` (U1's pattern). The
   pages predict that line then shows values 3333 2222 1111 0000, and the check (expecting
   0000 1111 2222 3333) goes red. Nothing else changes.
7. **Reference frame.** Sprite pixels via p (§2.5), from each image's left edge.

### P12 — All reuses of a sprite normally use the same colours during a frame

1. **Proposition.** When nothing rewrites COLOR17–19 during a frame, every use of sprite 0
   shows each pixel value in the same colour, the one held by the register that value selects.
   A Copper write between two uses changes the colours of the later uses, as the next sentence
   of the page says. Claims `ch2-putting-together-a-copper-instruction-list.c7` and
   `COP2-putting-together-a-copper-instruction-list.c5`, p.27; the register assignment is from
   pp.102–103 and 108.
2. **Stimulus.** S1 with list L_S1. COLOR17–19 are written by the Copper only in the list's
   vertical-blanking part, with `$0F00`, `$00F0` and `$000F`.
3. **Observation.** The colour of every non-transparent sprite pixel of U1–U4, in every
   output frame used.
4. **Expected value.** Every pixel of value 1 shows `$F00`, of value 2 `$0F0`, and of value 3
   `$00F`, in all four uses. p.27: *"every multiple reuse normally uses the same set of colors
   during a full display frame"*. p.108: *"Sprite channel 0, for example, takes its colors from
   registers 17 - 19"*, with Table 4-2 giving 01 → 17, 10 → 18, 11 → 19. "Every" is graded over
   all four uses and all three values in every output frame used. "Normally" is graded through
   the contrast.
5. **Preconditions.** C1, Q2, Q3, Q4, Q5, K9, and the four images as in P9(a).
6. **Contrasting case.** **S1-c:** list L_S1c, the page's own recipe from p.27: *"You can
   change sprite colors mid-screen with a Copper instruction list that waits until the last
   line of the first use of the sprite processor and changes the colors before the first line
   of the next use of the same sprite processor"*. The pages predict U1 in `$F00`/`$0F0`/`$00F`
   and U2–U4 in `$FF0`/`$0FF`/`$F0F`. The same-colours check goes red. Q5 holds for L_S1c, and
   the images and program are unaffected.
7. **Reference frame.** The colour nibbles of Table 3-3 (p.46), compared with the output's
   4-bit red, green and blue.

---

## 4. Build

### Built: P1–P5 and P7

P1–P5 and P7 (the memory-cycle-parity half of this specification) are built: `Makefile`,
`test_ch2_dma_cycles.cpp`, 35 `.s` programs. P6 is withdrawn (P6). Every named precondition (A1, K2a,
K2b, K3, K4, K5, K6, T1, T2, T3) is its own graded line, printed before the lines that depend on it
and never part of their graded expression (§2.11). K2b, K3, K4, K5, K6, T1, T2 and T3 are gated by the
observation preconditions of the runs they grade, as P2–P5 and P7 are; P1's gate leaves out F2 and
F2-COUNT (§2.11). F1, F2, F2-COUNT and F3 (§2.1, §2.3) are their own
graded lines too, printed per run before P1, for every configuration §2.9 and §2.10 name (C0, C1a,
C1b, C2a, C2b, C3, C4-1..6, C4-4w, C5, C6, C7a, C7b, S1, S2 in PAL/ECS; C0, C6 and S1 also in
NTSC/ECS, PAL/OCS and NTSC/OCS). Four self-tests of the comparators, SELFTEST-F1, SELFTEST-F2,
SELFTEST-P1 and HELD-CONTROL, run on synthetic traces. D9's own carve-out is exercised organically: C2b (read
direction, no drive) shows no disk-block access in any observed frame on both cores, so its read
direction is recorded declined rather than graded; `ch2-about-the-copper.c23` and
`COP2-about-the-copper.c28`, which say "only" over both directions, are recorded as not established
for both (P2.1), and `ch2-about-the-copper.c21` is recorded as not asserted (§0, P6).

Per-core result, from `out_fx68k.txt` / `out_tg68k.txt` (`make -k -B test`):

| line | fx68k | tg68k |
|---|---|---|
| F1, F2, F2-COUNT, F3 (every run and mode) | PASS; every refresh group (939 per PAL run, 789 per NTSC run) is exactly four accesses at offsets 0, 2, 4, 6 | PASS, same groups |
| SELFTEST-F1, SELFTEST-F2, SELFTEST-P1, HELD-CONTROL | PASS | PASS |
| FIXTURE, FIXTURE-PEEK | PASS | PASS |
| P1 | PASS (gate: C1, restarts, F1, F3 of every run, A1) | PASS |
| K2a, K2b | PASS; K2b: disk-block accesses per observed frame 936/939/939 in C2a and in C6 (K2b/D9 declines C2b: 0/0/0) | same counts |
| P2 | PASS | PASS |
| P3, K3 | PASS; K3: every channel has accesses in every observed frame (C1a 286/287/286, C1b 80/79/80, C6 286/287/286) | PASS (C6 287/286/287) |
| P4, K4 | PASS | PASS |
| P6 (withdrawn, MEASURED, not graded) | the Copper-block reads on an even in-line cycle are held continuations, all at s = 226 (the cycle before the next line's first refresh), each the sample of the same word, direction, data and byte enables one memory cycle (S) after the s = 225 sample: 24 (C5), 24 (C6), 39 (C7a), 264 (C7b), and 24 in each of C6 NTSC/ECS, C6 PAL/OCS and C6 NTSC/OCS; none remains on an even cycle once they are set aside; no sample repeats the one before it at S/2 | identical counts |
| A1 | PASS (0 violations) | PASS |
| P5, K5 | PASS; K5: 256 lines with fetches for every enabled plane in every observed frame | PASS |
| K6, T1, T2, T3 | PASS (K6 gated by the observation preconditions of C5, C6, C7a and C7b, T1 and T2 by those of C5 and C6, T3 by those of C6); T1: 0 segments differ, 0 segments empty in C5; T2: 342 segment lines, 0 CPU accesses in them; T3: audio lines per frame 107/105/107, disk lines per frame 113/113/113 | PASS (audio 106/106/106) |
| P7 | PASS (0 of 339 segment/frame pairs mismatch) | PASS (0 mismatches) |
| Copper first fetch after S4 (MEASURED, not graded) | in a regular line of 227 memory cycles, at s = 165 (C0), 135 (C7a), 111 (S2) and 189 (C2a): odd in all four | s = 155 (C0), 125 (S2) and 179 (C2a), odd in all three; C7a dropped (the regular lines around the fetch are 454 memory cycles apart) |
| NOT GRADED lines | none | none |
| summary | 134 passed, 0 failed | 134 passed, 0 failed |

Every mutation record of the header comment that has a selector was replayed on both cores from
the built object directories (`NMH_MUTATE=<value>`). The stimulus records print FAIL through the
comparator they target, unchanged, with the program reaching DONE (the FIXTURE mutation removes
DONE, which is what that check tests), after printing the line's own preconditions, all met, and
the lines the contrast must leave alone: K2b's counts and P2's population for K2a; F1–F3 and
F2-COUNT for K2b; T1, T2, T3 and P7's comparator for K6; K6, T2 and T3 for T1 (P7's comparator, which T1 gates, finds the 6 changed segment/frame pairs).
The 24 gate records (`<line>-NOFINISH`, `-HTOTAL` and `-NORESTART` for K2b, K3, K4, K5, K6, T1,
T2 and T3, §2.11) print NOT GRADED with the failed precondition. A mutation changes the stimulus
only, never an expected value, window, population, gate or comparator. P1–P5, F1, F2, F2-COUNT and
P7 have no stimulus mutation, and the records say which hardware fact no program changes; P1's red
path and F1's and F2's are shown by the synthetic self-tests SELFTEST-P1, SELFTEST-F1 and
SELFTEST-F2. F3's mutation is C0-htotal (§2.3); SELFTEST-P7 keeps run C6' as a self-test of the P7
comparator.

The NTSC/ECS, PAL/OCS and NTSC/OCS mode variants of C0, C6 and S1 (§2.10) are graded for P1,
through the same `Config` the harness already exposes; no new program was needed.

### Not built, by a scope decision

- **P8–P12** (sprite channel reuse, the video half of this specification's title): not
  implemented. They need a video-measurement apparatus this package does not have: frame
  boundaries from `vs`, an output-line-to-beam-line calibration (k_v) with its own self-test
  (Q3), a sprite-pixel-width self-test (Q2), and per-frame sprite-image edge and colour
  extraction from the captured video trace, none of which exists in this package yet. S1 and S2
  (§2.9) are built and already graded for P4 (sprite DMA parity).
- **The blitter DMA channel** (`COP2-about-the-copper.c34`–`c36`): not built. §0 already
  withdraws these three ids: their own sentence names a use (animation, line drawing, polygon
  filling), not a machine function this package's claims give an observable for.
