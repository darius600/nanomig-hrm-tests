# Specification: memory-cycle parity of the DMA channels, and sprite channel reuse

This specification was written from this package's claims (`claims-spec-stage.jsonl`) and
the pages of the *Amiga Hardware Reference Manual*, 3rd edition. Its author never saw the
implementation under test or any existing test. Every expected value below is taken from a
printed page and quoted. Where the pages leave a value open or disagree with each other, the
specification says so and grades only what all of them agree on. It specifies twelve
propositions. Nineteen of the package's 22 claims support them; three could not be used.

Page numbers are printed page numbers. "Fig." means a figure of the manual. "p.X" without a
chapter means the page in the manual.

---

## 0. Claims not turned into a proposition

| claim | page | says | reason |
|---|---|---|---|
| `COP2-about-the-copper.c34` | 20 | The blitter DMA channel handles animation. | The parenthesis *"(the DMA channel that handles animation, line drawing, and polygon filling)"* says which channel the sentence means. It describes no behaviour. "Animation" is a use, not a machine function: no register setting, output or value on p.20 or on any other page defines what "handling animation" produces. There is nothing to observe and no expected value. |
| `COP2-about-the-copper.c35` | 20 | The blitter DMA channel handles line drawing. | The same parenthesis. The claim states a purpose and gives no observable and no expected value. What a line-mode blit writes is specified only on pp.189–192, and no sentence there is a claim of this package. A check built from those pages would test behaviour this package holds no claim for, with expected values taken entirely from pages outside it. |
| `COP2-about-the-copper.c36` | 20 | The blitter DMA channel handles polygon filling. | As for c35. Area fill is specified on pp.184–186, which are outside this package's claims. |

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
that the four refresh slots come first in the line and within its first eight slots.

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

**D7. What address a refresh access carries.** No page says. **Handling:** refresh accesses
are identified by exclusion (§2.2), and a guard precondition (A1) checks that the exclusion
holds in the baseline configuration.

**D8. The Copper's wake-up cycle.** p.20: *"The WAIT instruction requires three memory
cycles and six memory cycle times; it takes one extra memory cycle to wake up."* p.275:
*"All instructions (except for WAIT) require two bus cycles"*. The pages do not say whether
the extra cycle is a bus access, or at which address. **Handling:** the Copper's block of
memory holds only Copper lists and padding. Every access into it counts as a Copper access
and is graded (P6). No check counts WAIT cycles.

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
shortest interval between two consecutive memory-refresh groups (§2.3), divided by 227, the
shorter of the two line lengths p.23 gives (*"In PAL, they are all 227 long"*; NTSC alternates
227 and 228). No constant is taken from any RTL source. F3 guards the measurement: every
observed line's own interval must equal exactly 227·S or 228·S, or the derivation is wrong for
that run.

- **Precondition F1 (slot grid).** Graded over the run's DMA accesses only -- every population
  §2.2 declares a block for, excluding the CPU block. S is an integer, and every DMA access
  time t satisfies t ≡ φ (mod S) for one phase φ. No page gives the 680x0's own accesses a
  phase at these ports (D5): the CPU's phase relative to the DMA grid is measured and reported
  on its own line, not folded into F1.
- **Contrast for F1.** None exists. No page describes a stimulus that moves an access off
  the memory-cycle grid. The checker is instead verified by a self-test: a copy of a
  recorded trace with one access moved by one time unit (when S > 1) must fail F1.

### 2.2 Attribution of accesses

Every run lays out Chip memory in declared, disjoint blocks (§2.7). An access is attributed
by its word address:

| block | attributed to |
|---|---|
| CPU block (vector table `$000000`–`$0003FF`, program code, stack, results area) | CPU |
| Copper block (every Copper list of the run, plus padding) | Copper |
| audio block x (x = 0..3) | audio channel x |
| disk block | disk |
| sprite block x (x = 0..7), null-sprite block | sprite DMA |
| bitplane block y (y = 1..6) | bitplane y |
| none of the above | **refresh set R** |

The blitter is never enabled: DMACON bit 6 (BLTEN) stays 0 in every run (Appendix A
p.278).

If the harness reports the requesting channel with each access, the test must also assert
that this report agrees with the address attribution. That assertion is a precondition.

- **Precondition A1 (attribution guard).** In the baseline configuration C0 (§2.9), every
  access in the observed frames is either in the refresh set R, in the CPU block, or in the
  Copper block. Nothing falls into any other declared block.
- **Contrast for A1.** Run C0 with AUD0LCH/L pointing at audio block 0, AUD0LEN = 16,
  AUD0PER = 124, and AUD0EN set. p.144 predicts audio DMA: *"When both DMAEN and AUDxEN are
  set, the DMA will start for channel x"*. Accesses then fall into audio block 0 and A1
  fails.

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
- **Precondition F2.** Every refresh group in the observed frames has at least two accesses.
  (p.194 lists *"4 cycles for memory refresh"*. That count is not graded here; F2 only makes
  the refresh check non-vacuous.)
- **Precondition F3.** For every pair of consecutive groups in the observed frames,
  A_{n+1} − A_n equals 227·S or 228·S (the values p.23 names; p.194 and p.304 average 227.5,
  see D4).
- **Contrast for F2 and F3 (run C0-m).** C0 with the list L0m (§2.8). It sends the Copper
  through a MOVE whose words lie outside every declared block, and makes it fetch those
  words near horizontal position `$70` of line `$80`. p.19 and p.22 predict Copper fetches
  there, after the WAIT resolves. Those unattributed accesses sit more than 16 cycles from
  any refresh group, so they form a group of their own. That splits line `$80` into two
  pieces whose lengths are neither 227 nor 228, and F3 fails.

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
  sprite pointers must be initialized to either a real sprite or a safe null sprite"*).
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
  INTENA ← `$C020` (SET, INTEN, VERTB).
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
- Every block is word aligned (p.104, p.138, p.246) and disjoint from all others, except in
  the alias contrasts named below. Each block is padded on both sides by at least 64 bytes of
  zero words that belong to it.
- The Copper block holds only Copper lists.
- The sub-list **U** used by C0-u and C0-m lies outside every declared block.
- The builder chooses the addresses within these rules.

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
| **L0u** | MOVE COP2LCH/L ← U; `DC.W $8005,$FFFE` (WAIT VP=`$80`, HP=`$04`); `DC.W $008A,$0000` (COPJMP2). **U**, outside all blocks: `DC.W $0180,$0000`; END |
| **L0m** | as L0u but with `DC.W $8071,$FFFE` (HP=`$70`) |
| **L_S1** | V_spr (sprite 0 → structure Z, sprites 1–7 → null block); V_bpl(1) (→ zero plane); `DC.W $01A2,$0F00`; `DC.W $01A4,$00F0`; `DC.W $01A6,$000F` (COLOR17–19 ← red, green, blue); END |
| **L_S1c** | as L_S1, but before END insert `DC.W $4801,$FFFE` (WAIT VP=`$48`, the line after the last line of use 1), `DC.W $01A2,$0FF0`, `DC.W $01A4,$00FF`, `DC.W $01A6,$0F0F` |
| **L_A** | V_spr (sprite 0 → A, sprite 2 → R, others → null); V_bpl(1); COLOR17–19 as in L_S1; `DC.W $01AA,$0FF0`; `DC.W $01AC,$00FF`; `DC.W $01AE,$0F0F` (COLOR21–23); MOVE COP1LCH (`$080`) ← hi(L_B); MOVE COP1LCL (`$082`) ← lo(L_B); END |
| **L_B** | as L_A with sprite 0 → B and COP1LC ← L_A |
| **L3x** | V_spr (sprite 0 → structure X, others → null); MOVE COP2LCH/L ← X+4; `DC.W $A001,$FFFE` (WAIT VP=`$A0`, HP=0); `DC.W $008A,$0000` (COPJMP2) |

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

**Structure X (for contrast X-spr).** `$5050,$5800`; seven × `$0180,$0000`; `$FFFF,$FFFE`;
`$0000,$0000`. As a sprite, this is one use of eight lines. As Copper code starting at X+4,
it is seven × MOVE COLOR00 ← `$000` followed by END.

**Null block.** `$0000,$0000` (p.108: *"the value 0 should be used for both words when ending
the usage of a sprite"*).

### 2.10 Modes and cores

The default mode is PAL with the Enhanced Chip Set. C0, C6 and S1 also run with NTSC/ECS,
PAL/OCS and NTSC/OCS. The NTSC runs of C0 and C6 exercise the alternating 228/227-cycle lines
of p.23. Every run is made on both CPU cores and graded per core.

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
   by construction, so the check bites on the second and later accesses, and F2 guarantees
   there is at least one.
5. **Preconditions.** F1, F2, F3 (§2.3), A1 (§2.2), C1 (§2.6).
6. **Contrasting case.** Run **C0-u**: C0 with list L0u. The Copper jumps to sub-list U,
   which lies outside every declared block, and fetches U's words right after the refresh
   group of line `$80` (WAIT HP=`$04`). p.20 and p.275 put Copper fetches on odd cycles, so
   line `$80`'s refresh set gains accesses with odd s, and the check goes red. The extra
   accesses come within 16 cycles of the group, so they join it without moving its first
   access. F2 and F3 therefore still hold, and the program finishes. The contrasts for
   F1–F3, A1 and C1 are given in §2.1–2.3 and §2.6.
7. **Reference frame.** Frame F: cycle 0 is the line's first refresh access, as Fig. 6-9
   draws it (D3). It is checked by F2 and F3.

### P2 — Disk DMA uses only the even memory cycles

1. **Proposition.** Every disk-DMA access lies on the even-numbered memory cycles of its
   line. Claims `ch2-about-the-copper.c23` and `COP2-about-the-copper.c28`, p.20.
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
   - F1–F3, A1, C1.
   - **K2a:** the stored DSKBYTR value has bit 14 (DMAON) = 1, and bit 13 (DISKWRITE) = 1 in
     C2a and C6 or 0 in C2b (p.248, Appendix A p.278; see D6).
   - **K2b:** at least 500 disk-block accesses in the observed frames of each run. If K2b
     fails in C2b only, the read direction is reported as not exercised (D9).
6. **Contrasting case.** Run **X-dsk**: C2a with list L5 instead of L0. DSKPTH/L points at
   the start of the Copper block, and the S6 value is `$C000`\|8000. The 16000 bytes from that
   address hold the Copper block and its padding only. In this run the disk block is declared
   as DSKPT … DSKPT + 16000, so it contains L5. Disk DMA reads L5's words on even cycles
   (p.20), and the Copper fetches the same words on odd cycles (p.20, p.275), so the disk-block
   check finds odd-s accesses and goes red. The disk DMA still runs, so K2a and K2b hold.
   - **Contrast for K2a and K2b:** C2a with both DSKLEN writes of S6 set to `$4000`\|8000
     (bit 15 clear). Both p.248 and Appendix A p.278 predict DMAON = 0, and no disk accesses
     occur.
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
5. **Preconditions.** F1–F3, A1, C1. **K3:** each of the four channels has at least 100
   accesses in the observed frames of each run.
6. **Contrasting case.** Run **X-aud**: C1a with list L5, AUD0LCH/L at the start of the
   Copper block, and AUD0LEN equal to the number of words in the Copper block. In this run
   audio block 0 is declared as that span. Audio channel 0 and the Copper then read the same
   words, one on even and one on odd cycles (p.20), so the audio check goes red. K3 still
   holds.
   - **Contrast for K3:** C1a without AUD0EN in the S4 write. Channel 0 then has no accesses,
     and K3 fails.
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
5. **Preconditions.** F1–F3, A1, C1. **K4:** in each observed frame of C3 and C6, every word
   of each of the eight structures is read at least once, including the use-2 control words,
   which are fetched in the VSTOP line of use 1. p.108: *"When the vertical position of the
   beam counter is equal to the VSTOP value in the sprite control words, the next two words
   fetched from the sprite data structure are written into the sprite control registers"*.
6. **Contrasting case.** Run **X-spr**: C3 with list L3x and structure X for sprite 0. From
   line `$A0` the Copper executes X+4 onward as code, the words sprite channel 0 has read as
   data in lines `$50`–`$57`. The sprite reads are on even cycles and the Copper's on odd
   cycles (p.20), so the check on sprite block 0 goes red. K4 still holds, because every word
   of X (both control words, eight data pairs, end words) is read by the sprite channel.
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
5. **Preconditions.** F1–F3, A1, C1. **K5:** every enabled plane has fetches in at least 150
   lines of every observed frame.
6. **Contrasting case.** Run **X-bpl**: C4-4 with list L5, whose V_all writes BPL1PTH/L with
   the start of the Copper block. In this run plane block 1 is declared as that address plus
   40 × 256 bytes, which contains L5 and the padding after it. Plane 1 and the Copper read the
   same words on even and odd cycles respectively (p.20, p.275), so check (a) goes red. The
   display still runs, so K5 holds. (Check (b) counts bitplane accesses only, and is not the
   target of this contrast.)
   - **Contrast for K5:** C4-4 without BPLEN. No plane fetches occur, and K5 fails.
7. **Reference frame.** Frame F.

### P6 — The Copper requests only the odd memory cycles

1. **Proposition.** Every Copper access lies on the odd-numbered memory cycles of its line,
   so it never falls on a cycle of the refresh, disk, audio, sprite or low-resolution-plane
   1–4 class. Claim `ch2-about-the-copper.c21`, p.20; Appendix A p.275.
2. **Stimulus.** Every run has Copper DMA. The Copper is installed per p.30 (S3 before S4).
   The quantifier is covered by these cases:
   - restart fetches in every run;
   - in C5, C6, C7a and C7b, fetches after WAITs at all 113 horizontal positions `$00`–`$E0`
     (list L5, p.23: *"there are 113 positions available for Copper operations"*),
     back-to-back MOVEs, a SKIP, and a COPJMP2 jump to L5b;
   - under the full even-cycle load of C6;
   - under odd-cycle contention from low-resolution planes 5–6 (C7a) and from
     high-resolution planes (C7b).
3. **Observation.** Every access in the Copper block during the observed frames of every
   run.
4. **Expected value.** Every Copper-block access has odd s. p.20: *"The Copper is a two-cycle
   processor that requests the bus only during odd-numbered memory cycles."* p.20 also says
   *"Because only the odd memory cycles are requested by the Copper …"*, and p.275 *"Since
   only the odd bus cycles are requested"*. "Only" is graded over every Copper access of every
   run and mode. C7a and C7b grade it where the Copper is short of odd cycles. Those two runs
   grade nothing about the bitplanes (D2).
5. **Preconditions.** F1–F3, A1, C1. **K6:** in every observed frame of C5, C6, C7a and C7b,
   every word of L5 and L5b is read at least once (the list ran to its end).
6. **Contrasting case.** Run **X-aud** (P3.6). Audio channel 0 reads the Copper block on
   even cycles (p.20), so the check on the Copper block goes red. The Copper still runs L5 to
   its end, so K6 holds.
   - **Contrast for K6:** L5 with END placed directly after V_all. The WAIT/MOVE words after
     it are then never fetched, and K6 fails.
7. **Reference frame.** Frame F.

### P7 — The channels on the even cycles never delay the Copper

1. **Proposition.** Because the Copper requests only odd cycles, audio, disk, refresh, sprite
   and low-resolution (≤ 4 planes) DMA never collide with it: the Copper's fetches land on the
   same cycles whether those channels run or not. Claim `ch2-about-the-copper.c21`, p.20;
   Appendix A p.275.
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
   113 segments are graded, in every observed frame and mode.
5. **Preconditions.**
   - F1–F3, C1, K6 (both runs).
   - **T1:** every segment has the same number of Copper accesses in both runs.
   - **T2:** no CPU-block access lies in any line spanned by a segment, in either run.
   - **T3:** in the lines of the segments in C6, bitplane fetches occur in every line; sprite
     fetches occur in every line of segments i = 16..35 and 64..83 (VP `$50`–`$63` and
     `$80`–`$93`); and audio and disk accesses each occur in at least half of the lines.
6. **Contrasting case.** Run **C6'**: C6 with list L5' (WAIT i = 32 at HP `$42` instead of
   `$40`). Compared with C5 on L5, segment 32 must start later: p.22 says *"The WAIT
   instruction causes the Copper to wait until the video beam counters are equal to (or
   greater than) the coordinates specified in the instruction"*. Its s-values therefore
   differ, and the check goes red. T1–T3 still hold.
   - **Contrast for T2:** C6 with `STOP #$2000` replaced by a loop that branches to itself.
     The CPU then fetches in every line, and T2 fails.
   - **Contrast for T3:** C6 without SPREN. Sprite fetches are then absent, and T3 fails.
   - **Contrast for T1:** C6 with one extra `DC.W $0180,$0000` after WAIT i = 50. Segment 50
     then has two more accesses, and T1 fails.
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

### Built: P1–P7

P1–P7 (the memory-cycle-parity half of this specification) are built: `Makefile`,
`test_ch2_dma_cycles.cpp`, 29 `.s` programs. Every named precondition (A1, K2a, K2b, K3, K4,
K5, K6, T1, T2, T3) is its own graded line, not folded silently into its proposition. D9's own
carve-out is exercised organically: C2b (read direction, no drive) shows 0 disk-block accesses
in the observed frames of both cores, so its read direction is recorded declined rather than
graded.

Per-core result, from `out_fx68k.txt` / `out_tg68k.txt` (`make -k -B test`):

| line | fx68k | tg68k |
|---|---|---|
| FIXTURE | PASS | PASS |
| P1 | PASS | PASS |
| K2a, K2b | PASS (K2b/D9 declines C2b) | PASS (K2b/D9 declines C2b) |
| P2 | PASS | PASS |
| P3, K3 | PASS | PASS |
| P4, K4 | PASS | PASS |
| P6 | **FAIL**: 24 (C5), 24 (C6), 39 (C7a), 264 (C7b) Copper-block reads at even s = 226 (the cycle before the next line's first refresh), each repeating the previous read (s = 225) of the same word; every other Copper-block read in these runs, and every read in C0/C1a/C1b/C2a/C2b/C3/C4-1..6/C4-4w/S1/S2, is on odd s | **FAIL**, identical counts |
| A1 | PASS | PASS |
| P5, K5 | PASS | PASS |
| K6, T1, T2, T3 | PASS | PASS |
| P7 | PASS (0 of 339 segment/frame pairs mismatch) | PASS (0 mismatches) |
| summary | 17 passed, 1 failed | 17 passed, 1 failed |

Every mutation of the header comment was run once on both cores from the built object
directories (`NMH_MUTATE=<value>`) and went red through the clause it targets, or (K2a, K3, K4,
K5) through the precondition it targets. F1 is a self-test, not an RTL run.

### Not built, by a scope decision

- **P8–P12** (sprite channel reuse, the video half of this specification's title): not
  implemented. They need a video-measurement apparatus this package does not have: frame
  boundaries from `vs`, an output-line-to-beam-line calibration (k_v) with its own self-test
  (Q3), a sprite-pixel-width self-test (Q2), and per-frame sprite-image edge and colour
  extraction from the captured video trace, none of which exists in this package yet. S1 and S2
  (§2.9) are built and already graded for P4 (sprite DMA parity); the structure/colour
  mutations `ZMut::EndAfterU1/ShortU2Height/SwapU2Line5` and `list_L_S1`'s `colours_variant`
  are present but unused, available to whichever future package builds P9–P12's own video
  apparatus.
- **NTSC/ECS, PAL/OCS and NTSC/OCS mode variants of C0, C6 and S1** (§2.10), for P1 and P6:
  not run. No new program is needed, only a different `Config` per run.
