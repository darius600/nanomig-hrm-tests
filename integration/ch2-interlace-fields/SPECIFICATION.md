# Specification: interlaced fields (Chapter 2, pages 24, 27 and 34)

This specification was written only from the package's claims
(`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference
Manual*, 3rd edition. Its author never saw the implementation or any test.
It defines six propositions (P1 to P6), the two 68000 programs that exercise
them, what each check observes and where, the expected value together with the
sentence it comes from, the preconditions a check asserts before it grades,
and a contrasting case for every check and every precondition. Page numbers
are printed page numbers. Where this document quotes the manual, the wording
was checked against the printed scan.

---

## 1. Claims that are not turned into a proposition

| Claim | Reason |
|---|---|
| `ch2-using-the-copper-in-interlaced-mode.c1`, `COP2-using-the-copper-in-interlaced-mode.c1` ("twice the normal number of vertical lines") | For scan lines, "twice" is not exact on the page itself: the next sentences give PAL as "312 lines normally and 625 in interlaced mode" (p34), and 625 is not 2 x 312. For picture lines, the doubling comes from the program's data layout (two pointer values and a modulo), not from the chip set. That doubling is graded as P4(d). The concrete counts on the same page are graded in P1 and P2. The word "twice" gives no expected value of its own. |
| `COP2-using-the-copper-in-interlaced-mode.c3` ("An interlaced NTSC display has 524 lines") | p24 disagrees: "In NTSC, the fields are 262, then 263 lines", which makes 525. Table 3-13 (p79, "=525-(21\*2)") and p304 ("NTSC (525 lines, ...)") also give 525. Only the part both readings agree on is graded (P2). The total of 524 or 525 is not graded. |
| `COP2-using-the-copper-in-interlaced-mode.c7` ("Each NTSC interlaced scan displays 262 lines at a time") | Same disagreement with p24 ("262, then 263"). Only the agreed part is graded (P2): a field has 262 or 263 lines, and no two consecutive fields both have 263. |
| `COP2-vertical-beam-position.c13` ("The first field length in the NTSC alternation is 262 lines"), `COP2-vertical-beam-position.c14` ("The next field length in the NTSC alternation is 263 lines") | c13: no page fixes which of the recorded fields is the first of the alternation. The capture opens at a wrap that no page ties to a field type (section 6, item 8), so the claim has no field to be graded on; p34 agrees with its value (262) and does not decide the order. c14: p34 disagrees ("262 lines at a time", "524 lines"). Neither is graded as an expected value: P2 allows 262 or 263 for every field but requires no order or alternation. c14 is used only as the gate of P3 (P3 is graded only when the fields do alternate between 262 and 263) and as a permitted value in P2. |
| `COP2-using-the-copper-in-interlaced-mode.c10` ("The even-numbered lines from the second scan are interlaced with the odd-numbered lines"), and the on-screen part of `ch2-...c8`, `ch2-...c9`, `COP2-...c8` and `COP2-...c9` ("displayed", "interlaced with") | Whether the lines sit between each other on the screen depends on the vertical half-line offset of alternate fields: "the scanning circuitry vertically offsets the start of every other field by half a scan line" (p49). The test sees video only after a line doubler, and no page says what an interlaced field looks like at that output, so the page gives no expected value there. No page ties the odd field (the "first scan") to anything the hardware exposes either: not to LOF, not to the long or short field. No assertion grades these four ids, not even their retrieval part: P4 grades the retrieval through other ids (the memory lines each field fetches), and these four are declined whole. |
| `ch2-using-the-copper-in-interlaced-mode.c19`, `COP2-using-the-copper-in-interlaced-mode.c18` ("the organization of the data in memory matches the apparent organization on the screen") | These two say that memory matches the screen, and where the lines land on the screen is not graded (the reason of the row above: no page says what an interlaced field looks like at the test's video output). P4(d) observes only the memory addresses the bitplane DMA fetches, so a design that fetched the stated addresses but displayed the lines in a different screen order would pass it. Declined. The memory-only claim `COP2-using-the-copper-in-interlaced-mode.c19` ("odd and even bitplane lines are interlaced together" in memory) is kept in P4(d). |
| `ch2-using-the-copper-in-interlaced-mode.c11` ("In Figure 2-1 the odd field is the field shown at time t") | No page ties the odd field to anything the hardware exposes, and P4e accepts either list as the first recorded field, so P4e establishes an alternation and never which field is the odd one or which lies at time t. Declined; P4e keeps `ch2-using-the-copper-in-interlaced-mode.c20.s2`. |
| `COP2-using-the-copper-in-interlaced-mode.c14` ("The illustrated even field occurs 16.6 milliseconds after the odd field"), and the "16.6 ms" in `ch2-...c12` | The figure label gives no precision and no tolerance. At the NTSC clock of p24 (3,579,545 Hz) and 227.5 colour clocks per line (p23, p194), 16.6 ms is 261.2 lines, and no field of whole lines lasts that long. The manual's own figures give 16.65 ms for a 262-line field and 16.72 ms for a 263-line field. The only gradable content, that the even field is the field directly after the odd field, is graded in P4(e) under `ch2-using-the-copper-in-interlaced-mode.c20.s2`; `ch2-using-the-copper-in-interlaced-mode.c12` is declined whole, as it bundles the 16.6 ms figure with that ordering claim. |

Every id in this section is printed in full by the normal run as a `RECORDED:` line that gives its reason. Every other claim is used. The mapping is:

| Proposition | Claims |
|---|---|
| P1 | `COP2-vertical-beam-position.c12`, `.c15`, `.c16`; `COP2-using-the-copper-in-interlaced-mode.c5`, `.c6`, `.c11`; `ch2-using-the-copper-in-interlaced-mode.c6`; with the LACE-clear control, `ch2-vertical-beam-position.c12` (P1-NOLACE) |
| P2 | Agreed part of `COP2-using-the-copper-in-interlaced-mode.c3`, `.c7` and `COP2-vertical-beam-position.c14` |
| P3 | `ch2-vertical-beam-position.c15`, `.c16`, `.c17`, `.c18`, `.c19`; `COP2-vertical-beam-position.c17`, `.c18`, `.c19`, `.c20`, `.c21`, `.c22` |
| P4 | `ch2-using-the-copper-in-interlaced-mode.c10`, `.c13`, `.c14`, `.c15`, `.c16`, `.c17`, `.c18`, `.c20.s2`; `COP2-using-the-copper-in-interlaced-mode.c12`, `.c13`, `.c15`, `.c16`, `.c17`, `.c19` |
| P5 | `ch2-...-interlaced-mode.c16`, `COP2-...-interlaced-mode.c15`; with the no-rewrite run, `ch2-putting-together-a-copper-instruction-list.c3` (P5-NOREWRITE) |
| P6 | `ch2-putting-together-a-copper-instruction-list.c4`, with the no-rewrite run (P6-NOREWRITE) |

---

## 2. Ground rules shared by every check

### 2.1 System setting

Enhanced Chip Set (the default), 2 MB Chip memory, PAL unless a program says
NTSC. No page distinguishes OCS from ECS for these claims. Each check is
graded once per CPU core, and no expected value depends on the core. Both
programs use only 68000 instructions and run each core in its default 68000
mode. The level-3 autovector is used at $6C, which assumes the vector base is
at its reset value of 0.
`BEAMCON0` and `DIWHIGH` are never written. p305 says of `BEAMCON0` "Do not
write to this register", and p306 says that if `DIWHIGH` "is not written, the
old scheme for DIWSTRT and DIWSTOP described above holds".

### 2.2 Registers

Addresses are custom-chip offsets from `$DFF000` (Appendix B, pp. 289-292; the
base is implied by p310, "$DFF03E through $DFF07E").

| Register | Offset | Use here | Bits used (page) |
|---|---|---|---|
| VPOSR | $004 | read LOF and V8 | bit 15 LOF, bit 0 V8 (p216, p285) |
| VHPOSR | $006 | read V7-V0 | bits 15-8 V7-V0, bits 7-0 H8-H1 (p216) |
| COP1LCH/L | $080/$082 | first Copper list address, written as a long word | p25 |
| COPJMP1 | $088 | strobe: load the Copper PC from COP1LC | p26, p30 |
| DIWSTRT / DIWSTOP | $08E / $090 | display window | p58, p59 |
| DDFSTRT / DDFSTOP | $092 / $094 | data fetch | p60 |
| DMACON | $096 | 15 SET/CLR, 9 DMAEN, 8 BPLEN, 7 COPEN, 5 SPREN | p222 |
| INTENA / INTREQ | $09A / $09C | 15 SET/CLR, 14 INTEN, 4 COPER | p218, p219, p35 |
| BPL1PTH/L | $0E0/$0E2 | bitplane 1 pointer | p21, p291 |
| BPLCON0 | $100 | 14-12 BPU, 9 COLOR, 3 LPEN, 2 LACE | p51, p89, p271 |
| BPLCON1, BPLCON2 | $102, $104 | written 0 | p271 |
| BPL1MOD, BPL2MOD | $108, $10A | modulo | p62, p270 |
| SPR0PTH/L ... SPR7PTH/L | $120 ... $13E | sprite pointers | p110, p291 |
| COLOR00, COLOR01, COLOR17-19 | $180, $182, $1A2-$1A6 | colours | p292 |

The encodings of Copper instructions are those of p21, p22 and p36:

- MOVE is `reg offset, data`.
- WAIT for vertical position V and horizontal position H (in colour clocks,
  $00-$E2) is `IR1 = (V AND $FF) << 8 OR (H AND $FE) OR 1`, `IR2 = $FFFE`
  (bit 15 BFD = 1 and all compare-enable bits set, as p22 and p24 say is
  normal). An example on p33 is `$00E3,$80FE ; Wait for Horizontal $E2`.
- A WAIT that ignores the horizontal position uses `IR2 = $FF00`, as in
  `$9601,$FF00` on p23.
- The end of a list is `$FFFF,$FFFE` (p23, p29, p30).

### 2.3 Units

p24 says "One horizontal count takes one cycle of the system clock", and p194
says "there are 227.5 'color clocks', or memory access cycles" per line. All
line lengths below are in colour clocks (horizontal counts). If the harness
counts time in a different clock, the builder converts with the harness's
fixed ratio. The builder never uses a ratio fitted to these measurements.

A short line is 227 colour clocks and a long line is 228. p23: "Every other
line is a long line (228 color clocks, 0-$E3), with the others being 227
color clocks long. In PAL, they are all 227 long."

### 2.4 Line numbering and the field

- **Line numbering.** Lines are numbered from 0. p219: "Bit 5, VERTB, causes
  an interrupt at line 0 (start of vertical blank)".
- **Field.** A field is the run of vertical positions from 0 up to the point
  where the vertical position returns to 0.
- **Field length N.** The number of distinct vertical positions in the field.
  This is what p24 counts: "There are actually 262 NTSC (312 PAL) possible
  vertical positions", and "the fields are 262, then 263 lines".
- **Last line.** The last line of a field is position N - 1.

### 2.5 Measurement M1: sampling the beam position with the CPU

Program I and Program T both run this sampling routine. It runs with the
light pen disabled (BPLCON0 bit 3 = 0), because p239 says "when the light pen
latch is enabled, the VPOSR register may be latched at any time".

1. **Take a sample.** Read the long word at `$DFF004` (VPOSR in the high word,
   VHPOSR in the low word; p216: "the address pairs VPOSR,VHPOSR ... can be
   read ... as long words"). Read it twice in succession. Accept the sample
   only if bits 16-8 are equal in both reads; otherwise discard both reads.
   The long word is two bus reads, and a line change between them would
   otherwise pair the wrong V8 with V7-V0.
2. **Decode it.** V is bits 16-8 of the accepted long word. LOF is bit 31.
3. **Detect a wrap.** An accepted sample whose V is lower than the previous
   accepted V is a wrap. A wrap ends one field and starts the next.
4. **Record each field** when its wrap occurs, as a record in Chip memory:
   - the V of the field's first accepted sample;
   - the V of its last accepted sample (so N = that value + 1);
   - a gap flag, set if any accepted V exceeds the previous accepted V by
     more than 1;
   - the OR and the AND of the LOF bits seen.

   The write of each record is visible at the Chip-memory port and so gives
   the time of the wrap.
5. **Capture window.** Recording starts at the second wrap after the setup
   has written its last register. The page does not say when a LACE write
   takes effect, so the transition is not graded. At that wrap the routine
   writes a start word. Each later wrap writes the record of the field it
   ends. Every recorded field is therefore bracketed by two port-visible
   writes. The routine records K complete fields and then writes a completion
   word. The harness keeps the system running after the completion word
   until the next Copper restart has been fetched: until the Copper's next
   fetch of the first word of a list, within a bounded run of a few fields, so
   that the last recorded field is not cut off before its own end.

A record is **valid** if its first V is 0 and its gap flag is clear. If every
field was sampled at least once per line, its length is N = last V + 1. LOF is
recorded for diagnosis only, because no claim of this package concerns it.

### 2.6 Measurement M2: the Copper restart as field anchor

Several pages say the Copper restarts from COP1LC once per field, during
vertical blanking:

- p25: "COP1LC is automatically used to start the program counter";
- p30: "every time vertical blanking occurs the Copper will restart at the
  same location";
- p32: "restored to the Copper's program counter at the start of the vertical
  blanking period".

The **restart** R_k is the time of the Copper fetch of the first word of the
list that COP1LC points to. It is the first Copper fetch after a stretch in
which the Copper was waiting for a position that cannot occur (`$FFFF,$FFFE`).

The port-side field k is the interval [R_k, R_{k+1}). M1 record k is matched
to the restart interval that contains the midpoint between the port times of
the record writes that open and close M1 field k.

**Anchor check.** The M1 fields and the restarts correspond one to one, in
order:

- each of the K M1 fields, taken as the span between the port times of the
  record writes that open and close it, holds exactly one restart;
- the restart intervals the M1 fields are matched to (the rule above) follow
  each other by exactly one, field after field;
- before each of those K restarts, the Copper's last read of a list word up
  to the lists' terminal `$FFFF,$FFFE` waits is a read of a terminal wait (the
  stretch the definition above requires; in Program I a restart of one list
  follows the other list's wait). The terminal waits are found in the lists as
  the program assembled them. A read of the word after a terminal wait is not
  a list fetch for this clause; measured on both cores, the Copper makes one
  such read 2 colour clocks before every restart (section 7).

The third clause has a page-predicted contrast. p25 says that "no matter what
the Copper is doing" it is forced to restart when vertical blanking ends, and
p20 gives four memory cycle times for a MOVE. A list that waits for line 255
and then holds more MOVEs than the rest of the field can hold is therefore
restarted in the middle of the list, and the last list word the Copper read
before the restart is a MOVE word, not a terminal wait (I-v for Program I,
T-k for Program T; sections 3.5 and 4.4). The first two clauses hold there and
the third fails.

Pages disagree on whether the restart is at the start or the end of vertical
blanking (section 6); one restart per M1 field holds under either reading.

### 2.7 Measurement M3: line marks (Program T only)

Every line gets a timestamp taken at a fixed horizontal position. There are two
kinds of mark:

- **Copper mark** m_c(k, v): the port time of the fetch of the first word of
  the `MOVE COLOR00` that directly follows `WAIT (v, $E0)` in field k, for
  v = 0 ... 254.
- **Bitplane mark** m_b(k, v): the port time of the first bitplane fetch on
  line v of field k. The bitplane fetches of a field come in groups of 20
  words, one group per line. The i-th group (counting from 0) belongs to line
  44 + i.

Δ = m_c(k, v) - m_b(k, v) on the lines 44 ... 254, where both kinds of mark
exist. The length of line v of field k is:

- v = 0 ... 253: the time from the Copper mark of line v to the next Copper
  mark the list produces. With the full list, that is m_c(k, v+1) - m_c(k, v);
- v = 254 ... N_k - 2: m_b(k, v+1) - m_b(k, v);
- v = N_k - 1 (the last line): m_c(k+1, 0) - m_b(k, N_k - 1) - Δ.

The Copper marks are anchored on the WAIT comparison (p22: "The WAIT
instruction causes the Copper to wait until the video beam counters are equal
to (or greater than) the coordinates specified in the instruction"). The
bitplane marks are anchored on the data-fetch start (p60). The anchor checks
are PT-3 and PT-4 in section 4.

---

## 3. Program I: interlaced PAL bitplane display with one Copper list per field

This program serves P1, P4, P5 and P6.

### 3.1 Data in Chip memory

Every item is word-aligned and in Chip memory (p29 note 1, p104 warning). The
addresses are free, provided the items do not overlap and PIC plus 11 x 20,480
bytes stays below 2 MB (variant I-c reads that far).

- **PIC**: 512 lines of 40 bytes, 20,480 bytes. Every word of memory line j
  (j = 0 ... 511, line j at PIC + 40j) holds the tag `$8000 OR j`. In Figure
  2-1 numbering, memory line j is screen line j + 1.
- **SPR**: sprite 0 structure, the Table 4-1 layout (p105), using the example
  format of p109:
  - `$5060, $5200`: VSTART $50, HSTART $60, VSTOP $52;
  - `$0FF0, $0000` and `$0FF0, $0000`: two lines of data;
  - `$0000, $0000`: end-of-data (p108: "the value 0 should be used for both
    words when ending the usage of a sprite").

  The structure is 16 bytes long.
- **NULLSPR**: `$0000, $0000`, for sprites 1-7. p114 says all eight sprite
  pointers must be initialised once sprite DMA is on.
- **LISTA**, **LISTB**: the Copper lists of section 3.2 (relocated in I-v).
- **Handler**, a handler-counter word **HCNT**, and the M1 record area
  with its completion word. Which list the handler writes into COP1LC next
  is kept in a CPU register.
- **FIXMARK**: a word the program writes once, `$BEEF`, before M1 starts: the
  fixture's own proof that it can deliver a known value (FIX-I, section 3.6).

### 3.2 Copper lists

LISTA is:

```
MOVE BPL1PTH, hi(PIC)        ; pointer rewrite during vertical blanking (p27)
MOVE BPL1PTL, lo(PIC)
MOVE SPR0PTH, hi(SPR)        ; sprite pointer rewrite (p27, p109 step 4)
MOVE SPR0PTL, lo(SPR)
MOVE SPR1PTH..SPR7PTL <- NULLSPR   (14 MOVEs)
WAIT line $2D, horizontal ignored  ($2D01,$FF00)   ; "just after the first line of the display" (p35)
MOVE INTREQ, $8010           ; Copper interrupt, as p33 and Table 2-1 (p35)
WAIT $FFFF,$FFFE             ; end of list (p30)
```

LISTB is identical except that it writes `PIC + 40` into BPL1PTH/L. p62 says:
"For odd fields, the bitplane pointers begin at START. For even fields, the
bitplane pointers begin at START+40."

The pointer MOVEs come first, with no WAIT before them. They therefore execute
right after the restart, which every page places in vertical blanking (p25,
p30, p32, p35).

### 3.3 Level-3 handler (68000 autovector 27, address $6C; COPER is level 3, p219)

1. Write the address of the list that COP1LC does not hold into COP1LC as
   one long word (p25).
2. Remember the list that COP1LC held as the one to write next time.
3. Add 1 to HCNT in Chip memory. This is a CPU write, visible at the port.
4. Write `$0010` to INTREQ, which clears COPER (p218).
5. Return with RTE.

Why the handler toggles: p35 describes changing COP1LC "to point to the
second list" and says "during the vertical blanking interval, COP1LC will be
automatically reset to point to the original list". p33 names the agent of
that reset: "The COP1LC register is written at the end of the vertical
blanking period by a graphics interrupt handler". A program that has taken
over the machine has no such handler, so this handler does both writes, one
per field.

### 3.4 CPU sequence

1. Write `$7FFF` to INTENA, DMACON and INTREQ. This clears everything; the
   set/clear convention is on p218.
2. Build the data of section 3.1.
3. Write the display registers:
   - `BPLCON0 = $1204`: one plane, COLOR, LACE, LPEN = 0. The example on p51
     is `$A204`.
   - `BPLCON1 = 0`, `BPLCON2 = 0`.
   - `BPL1MOD = BPL2MOD = 40`. p64: "Set modulo to 0 for non-interlaced, 40
     for interlaced."
   - `DDFSTRT = $0038`, `DDFSTOP = $00D0` (p60).
   - `DIWSTRT = $2C81`, `DIWSTOP = $2CC1`. These are the normal PAL values of
     p59: window lines 44 ... 299, 256 lines, an even number as p77 asks.
   - COLOR00, COLOR01, COLOR17-19.
4. Write `BPL1PTH/L = PIC` and `SPR0PTH/L = SPR`, as p110 does with the CPU.
5. Write the handler address to $6C. The first list the handler writes is
   LISTB. Set `HCNT = 0`.
6. Write COP1LC = LISTA as a long word, then **write** COPJMP1. p30: "you must
   initialize one of the Copper location registers ... and write to its
   strobe address before Copper DMA is turned on" (see section 6, item 5).
7. Write `DMACON = $83A0`: SET, DMAEN, BPLEN, COPEN, SPREN. The order follows
   p30.
8. Write `INTREQ = $0010`, then `INTENA = $C010` (SET, INTEN, COPER).
9. Set SR to `$2000`, so interrupt levels 3 and up are accepted.
10. Run M1 with K = 8. Then write the completion word and stop.

### 3.5 Variants of Program I

Each variant changes only what is listed.

| Variant | Change from I-0 (the program above) |
|---|---|
| I-a | `BPLCON0 = $1200` (LACE = 0) |
| I-b | LISTB writes PIC, not PIC + 40, into BPL1PTH/L |
| I-c | Both lists omit the two BPL1PT MOVEs. The CPU still writes BPL1PT = PIC once (step 4). |
| I-d | The handler omits step 1 (it still counts and clears) |
| I-e | Both lists omit the two SPR0PT MOVEs. The CPU still writes SPR0PT = SPR once (step 4). |
| I-f | `DMACON = $8320` (COPEN not set) |
| I-g | `INTENA = $C000` (COPER not enabled) |
| I-h | `DMACON = $82A0` (BPLEN not set) |
| I-i | `DMACON = $8380` (SPREN not set) |
| I-j | M1 waits (M1_DELAY, section 7) between two samples, several times longer than one line |
| I-k | PIC is filled with $0000, not with the tags |
| I-l | In both lists the two BPL1PT MOVEs are placed after the `WAIT $2D01,$FF00` |
| I-m | In both lists the two SPR0PT MOVEs are placed after the `WAIT $2D01,$FF00` |
| I-n | FIXMARK stores $DEAD, not $BEEF |
| I-o | `BPLCON0 = $1200` (LACE = 0) and M1 waits (M1_DELAY, section 7) between two samples |
| I-p | `BPL1MOD = BPL2MOD = 38` |
| I-q | I-c, and M1 waits (M1_DELAY, section 7) between two samples |
| I-r | I-e, and M1 waits (M1_DELAY, section 7) between two samples |
| I-s | Both lists end in `$0000,$0000` (a MOVE to register `$000`, which the Copper cannot write, p21 and p26) in front of the `$FFFF,$FFFE` that stays in the list |
| I-t | In both lists the two BPL1PT MOVEs are placed after `WAIT $2001,$FF00` (line 32): after the end of vertical blanking (p219, section 6 item 7) and in front of the first bitplane fetch (line 44) |
| I-u | In both lists the two SPR0PT MOVEs are placed after `WAIT $1901,$FF00` (line 25): outside the 25 lines of vertical blanking that p219 guarantees, and in front of the sprite's first fetch (measured on both cores: line 25, colour clock 19; see PI-12 in section 3.6) |
| I-v | LISTA is at `$0A0000` and LISTB at `$0A5000`. In both, in front of the terminal `$FFFF,$FFFE`, a `WAIT $FF01,$FFFE` (line 255) is followed by 4,700 `MOVE COLOR00, bg` (four memory cycle times each, p20: 18,800 colour clocks, more than the 57 lines of a PAL field after line 255) |

### 3.6 Preconditions of Program I

The contrasting case that makes each one fail is in the last column.

A precondition is graded only where the run has what it reads. Where it does
not, the run prints NOT GRADED for it: it neither passes nor fails.

- A precondition that reads the M1 records needs PI-1: PI-2 and PI-3, and every
  one of the following that reads the restart frame.
- A precondition that reads the restart frame (PI-4, PI-5, PI-7, PI-12) needs
  every M1 field to be matched to a restart (section 2.6).
- PI-4, PI-6, PI-7(bpl) and PI-12(bpl) apply only where both lists hold the two
  BPL1PT MOVEs, and PI-7(spr) and PI-12(spr) only where they hold the two
  SPR0PT MOVEs. The MOVEs are looked up in the lists as the program assembled
  them (stimulus data in chip memory, not a fetch).
- PI-6 needs PI-4: where PI-4 holds, every field has its 256 groups of 20
  fetches inside PIC, so every field has fetches there for PI-6 to read.
- PI-7(bpl) needs a bitplane fetch in every field to order the MOVEs against,
  and PI-7(spr) a sprite-0 fetch. PI-12 needs the matching PI-7.

| Id | Precondition | Fails in |
|---|---|---|
| FIX-I | The program writes `$BEEF` to FIXMARK before M1 starts, and the harness reads `$BEEF` back after the run: the fixture can deliver a known value. | I-n (`$DEAD`) |
| PI-1 | The completion word was written. All K M1 records are valid (section 2.5). Every record's last V is at least 255, so no wrap is spurious. | I-j |
| PI-2 | Anchor check of section 2.6, with LISTA and LISTB: one restart per M1 field, in order, each the first-word fetch of LISTA or of LISTB, preceded, among the list's words up to the terminal wait, by the fetch of the terminal `$FFFF,$FFFE` wait. | I-f (no restart); I-v (the third clause, page-predicted: the Copper is restarted in the middle of the list); I-s is a self-test of the third clause on this design only, not page-predicted (section 7) |
| PI-3 | Interlace is active: the check of P1 holds. It is also a precondition of P4, whose retrieval lines are graded only on an interlaced run. PI-3 and P1 read one comparator, so I-a turns PI-3, P1 and the first half of P1-NOLACE red together. | I-a |
| PI-4 | Every field contains exactly 256 groups of bitplane fetches inside PIC. Each group is 20 fetches of consecutive ascending word addresses. | I-h |
| PI-5 | HCNT rises by exactly 1 in every field. | I-g |
| PI-6 | Every bitplane fetch from inside PIC returns the tag of the memory line at its address. Fetches outside PIC are not covered. | I-k |
| PI-7(bpl) | In every field, the Copper fetch of the BPL1PT MOVEs precedes the field's first bitplane fetch, the earlier of the first fetch inside PIC and the first 20-word group that P4(a), P4(b) and P5 find from PIC upward (for P4 and P5). | I-l |
| PI-7(spr) | In every field, the Copper fetch of the SPR0PT MOVEs precedes the field's first sprite-0 fetch (for P6). | I-m |
| PI-8 | The sprite-0 structure at SPR is fetched at least once during the run (for P6). | I-i |
| PI-12(bpl) | In every field, the Copper fetch of the BPL1PT MOVEs lies in vertical blanking: less than 23 lines after the write that opens the M1 field (for P4 and P5). | I-t |
| PI-12(spr) | In every field, the Copper fetch of the SPR0PT MOVEs lies in vertical blanking, as for PI-12(bpl) (for P6). | I-u |
| PI-9 | The LACE-clear control run (I-a) is a valid fixture: PI-1 and PI-2 hold on that run (for P1-NOLACE). | I-o |
| PI-10 | The no-BPL1PT-rewrite run (I-c) is a valid fixture: PI-1 and PI-2 hold on that run, and the first 20-word fetch of every one of its fields is found (for P5-NOREWRITE). | I-q |
| PI-11 | The no-SPR0PT-rewrite run (I-e) is a valid fixture: PI-1, PI-2 and PI-4 hold on that run, and the sprite structure is fetched at least once (for P6-NOREWRITE). | I-r |

**PI-12.** p219 gives the minimum time of vertical blanking as 25 horizontal scan
lines for a PAL system, beginning at line 0 ("The range starts at line 0 and
ends at line ... 25 for PAL"; section 6, item 7). The write that opens an M1
field follows the sample that shows the field's line 0 (section 2.5) by less
than one and a half lines: the sample lies in line 0, and the wrap routine
that writes the record is a few instructions. A Copper fetch less than 23 lines
after that write therefore lies before line 25. The limit is that arithmetic,
not a fitted value. Where the lists hold no such MOVE, PI-12 is not graded.

The two contrasts differ in reach. I-t puts the BPL1PT MOVEs at line 32, after
the vertical blank stop of line 29 that p79 gives for PAL as well as after the 25
lines of p219: nothing in front of line 44 depends on them. I-u cannot go
beyond line 25: measured on both cores, sprite DMA fetches the first words of
the sprite structure in line 25 (colour clock 19), so SPR0PT MOVEs placed later
would turn PI-7(spr) red as well. At line 25 the MOVEs lie outside the 25 lines
p219 guarantees but not outside p79's blank stop; PI-12(spr) grades the
guaranteed range only, and I-u shows that its bound bites.

---

## 4. Program T: line timing at the field boundaries

This program serves P2 and P3.

### 4.1 Data

- **PLANE**: 11,000 bytes of $0000.
- **LISTT**: the Copper list of section 4.2.
- The M1 record area.
- **FIXMARK**: a word the program writes once, `$BEEF`, before M1 starts: the
  fixture's own proof that it can deliver a known value (FIX-T, section 4.5).

### 4.2 Copper list LISTT (the same list in every field)

```
WAIT (0, $E0)          ; $00E1,$FFFE
MOVE COLOR00, bg       ; mark, line 0
MOVE BPL1PTH, hi(PLANE); pointer rewrite during vertical blanking (p27)
MOVE BPL1PTL, lo(PLANE)
WAIT (1, $E0)          ; $01E1,$FFFE
MOVE COLOR00, bg       ; mark, line 1
... one WAIT (v, $E0) + MOVE COLOR00, bg for every v up to 254 ...
WAIT $FFFF,$FFFE
```

`bg` is the value already in COLOR00, so the marks leave the picture
unchanged. The horizontal position $E0 lies after the line's bitplane fetches
(DDFSTOP $D0) and before the end of the shortest line ($E2, p23). The marks
stop at line 254, because a Copper WAIT compares only V7-V0 (p24: "the
comparison works on the least significant bits of the vertical count").

### 4.3 CPU sequence

1. Select NTSC in the harness.
2. Write `$7FFF` to INTENA, DMACON and INTREQ.
3. Write the display registers:
   - `BPLCON0 = $1204`, `BPLCON1 = 0`, `BPLCON2 = 0`;
   - `BPL1MOD = BPL2MOD = 0`;
   - `DDFSTRT = $0038`, `DDFSTOP = $00D0`;
   - `DIWSTRT = $2C81`;
   - `DIWSTOP = $39C1`. VSTOP is `$39` with its most significant bit forced
     to the complement of the next one, which gives `$139` = line 313. p59:
     "forcing the MSB of the stop position to be the complement of the next
     MSB". Line 313 lies beyond the last line of every NTSC and PAL field, so
     the window stays open to the last line of each field.
4. Write COP1LC = LISTT, then write COPJMP1 (p30).
5. Write `DMACON = $8380`: SET, DMAEN, BPLEN, COPEN.
6. Leave interrupts disabled.
7. Run M1 with K = 13, then write the completion word.

### 4.4 Variants of Program T

| Variant | Change from T-0 (the program above) |
|---|---|
| T-a | `BPLCON0 = $1200` (LACE = 0) |
| T-b | PAL selected in the harness |
| T-c | The first instruction of LISTT is `WAIT (0, $00)` (`$0001,$FFFE`) |
| T-d | The mark of line 100 uses `WAIT (100, $C0)`, not `(100, $E0)` |
| T-e | `DIWSTOP = $F4C1`, the normal NTSC value of p59 (VSTOP line 244) |
| T-f | M1 waits (M1_DELAY, section 7) between two samples, several times longer than one line |
| T-g | The WAIT of line 10 uses `(10, $40)`, not `(10, $E0)`; every mark stays in the list |
| T-h | `DMACON = $8300` (COPEN not set) |
| T-i | FIXMARK stores `$DEAD`, not `$BEEF` |
| T-j | LISTT ends in `$0000,$0000` (as in I-s) in front of the `$FFFF,$FFFE` that stays in the list |
| T-k | LISTT, in front of its terminal `$FFFF,$FFFE`, waits for line 255 (`WAIT $FF01,$FFFE`) and then holds 900 `MOVE COLOR00, bg` (four memory cycle times each, p20: 3,600 colour clocks, more than the 8 lines of an NTSC field after line 255; the list and the code of M1 behind it end below PLANE) |

### 4.5 Preconditions of Program T

As in section 3.6, a precondition is graded only where the run has what it
reads, and prints NOT GRADED otherwise. PT-2, PT-3, PT-5 and PT-7 and the gate
PT-6 read the field lengths N_k and need PT-1; PT-2 also needs every M1 field to
be matched to a restart; PT-3 needs PT-2 (the bitplane marks it reads are
there); PT-5 needs PT-3 and the gate PT-4.

| Id | Precondition | Fails in |
|---|---|---|
| FIX-T | The program writes `$BEEF` to FIXMARK before M1 starts, and the harness reads `$BEEF` back after the run: the fixture can deliver a known value. | T-i (`$DEAD`) |
| PT-1 | The completion word was written, and all K M1 records are valid. | T-f |
| PT-2 | In every field, the bitplane fetches are exactly N_k - 44 groups, each of 20 consecutive ascending word addresses, with no fetch left over (the number of fetches is 20 x (N_k - 44)). This shows that the bitplane marks run from line 44 to the last line, and it confirms N_k independently of the CPU. | T-e |
| PT-3 | Δ has one single value on every line 44 ... 254 of every field. | T-d |
| PT-4 (gate of PT-5 and P3) | In every field, m_c(k, 1) - m_c(k, 0) is 227 or 228 colour clocks: the marks of line 0 and line 1 lie on consecutive lines, so the Copper mark of line 0 is anchored on line 0. p25 leaves open whether the restart lies at the start or at the end of vertical blanking, and a restart at the end of blanking satisfies the WAIT (0, $E0) at once, so PT-4 is printed as a gate state and is not graded. When it is closed, PT-5 and P3 are not graded. | T-c |
| PT-5 | Every line length of section 2.7 is 227 or 228. Graded only when PT-3 and PT-4 hold, because the last line's length uses Delta and every mark of line 0 uses the anchor PT-4 gives. | T-g |
| PT-6 (gate of P3) | Consecutive field lengths alternate between the two values p24 gives for the selected standard: 262 and 263 in NTSC, 312 and 313 in PAL. p34 reads the NTSC fields as all 262 lines, so an implementation that follows p34 closes the gate; PT-6 is printed as a gate state and is not graded, and P3 is reported as not graded, gate closed. | T-a |
| PT-7 | Anchor check of section 2.6, with LISTT: one restart per M1 field, in order, each the first-word fetch of LISTT, preceded, among the list's words up to the terminal wait, by the fetch of the terminal `$FFFF,$FFFE` wait. | T-h (no restart); T-k (the third clause, page-predicted: the Copper is restarted in the middle of the list); T-j is a self-test of the third clause on this design only, not page-predicted (section 7) |

---

## 5. Propositions

### P1: PAL interlaced fields alternate between 312 and 313 lines, and do not alternate without interlace

1. **Proposition.** With LACE set in PAL, the beam produces two kinds of field
   that alternate. Every field has 312 or 313 lines and consecutive fields
   differ, so any two consecutive fields together have 625 lines. Long and
   short fields occur in interlace only.
   - Claims: `COP2-vertical-beam-position.c12`, `.c15`, `.c16`;
     `COP2-using-the-copper-in-interlaced-mode.c5`, `.c6`, `.c11`;
     `ch2-using-the-copper-in-interlaced-mode.c6`. With the LACE-clear
     control, `ch2-vertical-beam-position.c12` (P1-NOLACE).
   - Pages 24 and 34.
2. **Stimulus.** Program I (I-0): `BPLCON0 = $1204` with a one-plane display
   and the Copper lists running.
3. **Observation.** The lengths N_k of the K = 8 recorded fields (M1). The
   interval is each field from wrap to wrap, as defined in section 2.4.
4. **Expected.**
   - p24: "There are alternating long and short lines, there are also long
     and short fields (interlace only). In NTSC, the fields are 262, then 263
     lines and in PAL, 312, then 313 lines."
   - p34: "PAL has 312 lines normally and 625 in interlaced mode. In
     interlaced mode, the video beam scans the screen twice from top to
     bottom", and "The scanning circuitry thus treats an interlaced display
     as two display fields".

   The check: N_k is 312 or 313 for all 8 fields, and N_{k+1} differs from
   N_k for all 7 consecutive pairs, hence N_k + N_{k+1} = 625 for each pair.
   "Alternating" is graded on every pair, not on one. Nothing on the page says
   which length comes first ("312, then 313" gives no anchor), so either
   phase is accepted, and "first" in `.c15` is not graded. Which length
   coincides with LOF = 1 is recorded but not graded.
5. **Preconditions.** PI-1, PI-2, PI-4.
6. **Contrasting case.** I-a (LACE = 0). p24 limits long and short fields to
   "(interlace only)". The test expects all 8 N_k to be equal: the P1 check
   goes red and the equality is a live control of the mutation run. The
   common value is recorded but not graded; section 6, item 6 explains why.
   P1 and PI-3 read one comparator (the check of P1), so I-a turns P1, PI-3 and
   the first half of P1-NOLACE red together; the mutation runs of P1 and PI-3
   print only their own line.
   The contrasts for the preconditions are I-j, I-f and I-h (section 3.6).
   `ch2-vertical-beam-position.c12` says that long and short fields occur
   in interlace only, which holds both halves: P1's alternation with LACE
   set and the equality with LACE clear. It is graded by one assertion,
   P1-NOLACE, the conjunction of P1's check on Program I and the equality
   of all 8 recorded field lengths on a run of I-a (run directly, not as a
   mutation), behind PI-9, which says that the I-a run is itself a valid
   fixture (PI-1 and PI-2 on that run; contrast I-o). Its contrasts are I-a
   in the place of the interlace run (the first half goes red) and I-0 in
   the place of the control run (the second half goes red); in each, the
   other half is a live control.
7. **Reference frame.** The field of section 2.4, located by the wraps of M1
   and cross-checked against the Copper restarts (PI-2).

### P2: NTSC interlaced fields have 262 or 263 lines, and never two 263-line fields in a row

1. **Proposition.** Every NTSC interlaced field has 262 or 263 lines, and of
   any two consecutive fields at least one has 262 lines. This is exactly the
   part on which p24 and p34 agree.
   - Claims: the agreed part of `COP2-using-the-copper-in-interlaced-mode.c3`,
     `.c7` and `COP2-vertical-beam-position.c14`. `COP2-vertical-beam-position.c13`
     is not graded (section 1) and the run records it as declined.
   - Pages 24 and 34.

   The two pages, quoted:
   - p24: "In NTSC, the fields are 262, then 263 lines".
   - p34: "Whereas a normal NTSC display has 262 lines, an interlaced NTSC
     display has 524 lines. ... In interlaced mode, the video beam scans the
     screen twice from top to bottom, displaying, in the case of NTSC, 262
     lines at a time."

   p79 ("=525-(21\*2)") and p304 ("NTSC (525 lines, ...") side with p24.
2. **Stimulus.** Program T (T-0): NTSC, `BPLCON0 = $1204`.
3. **Observation.** N_k for the K = 13 recorded fields (M1). PT-2 confirms
   each N_k by counting bitplane lines.
4. **Expected.** N_k is 262 or 263 for every k, and no k has
   N_k = N_{k+1} = 263.

   Not graded:
   - whether a 263-line field occurs at all;
   - the frame total of 524 or 525.

   P2 has no interlace precondition. Read from p24 ("262 NTSC ... possible
   vertical positions"), a non-interlaced NTSC display would also satisfy this
   check. T-a, which clears LACE, measures 263 lines in all 13 recorded fields
   instead (section 7), and P2 would be red on it; section 6, item 6 leaves
   open whether a non-interlaced field has 262 or 263 lines. P2 alone
   therefore does not show that interlace is active. P3 does, when its gate
   opens.
5. **Preconditions.** PT-1, PT-2, PT-7.
6. **Contrasting case.** T-b (PAL). p24 gives PAL fields of "312, then 313
   lines", so N_k is outside {262, 263} and the check goes red. PT-1, PT-2
   and PT-7 still hold. The contrasts for the preconditions are T-f, T-e and
   T-h.
7. **Reference frame.** As in P1.

### P3: the NTSC four-field pattern

1. **Proposition.** When NTSC interlaced fields alternate between 262 and 263
   lines, the sequence of fields and of the line each field ends on repeats
   every four fields:
   - a short field ending on a short line;
   - a long field ending on a long line;
   - a short field ending on a long line;
   - a long field ending on a short line;
   - and back to the beginning.

   Claims: `ch2-vertical-beam-position.c15` to `.c19`, and
   `COP2-vertical-beam-position.c17` to `.c22`. Page 24.

   The gate (PT-6) rests on `COP2-vertical-beam-position.c13` and `.c14`.
   It exists because p34 denies NTSC fields of 263 lines (see P2). Every page
   is consistent with "if the fields alternate between 262 and 263 lines,
   they follow this pattern": p34 denies the premise and says nothing against
   the consequence. The pattern is therefore graded only when PT-6 holds. If
   the fields are all 262 lines (p34's reading), P3 is reported as "not
   graded, gate closed". That is not a failure.
2. **Stimulus.** Program T (T-0).
3. **Observation.**
   - f_k: the type of field k, "short" if N_k = 262 and "long" if N_k = 263.
   - x_k: the length of its last line, from M3 (section 2.7), taken
     between the bitplane mark of line N_k - 1 in field k and the Copper mark
     of line 0 in field k + 1.

   Fields k = 0 ... 11 of the 13 recorded fields are graded. The last line of
   field 11 needs the line-0 mark of field 12.
4. **Expected.**
   - p24: "This alternation of lines and fields produces the standard NTSC 4
     field repeating pattern: short field ending on short line / long field
     ending on long line / short field ending on long line / long field
     ending on short line / and back to the beginning..."
   - A short line is 227 and a long line 228 colour clocks (p23, quoted in
     section 2.3).

   The check: there is one phase φ in {0, 1, 2, 3} such that, for every
   k = 0 ... 11, (f_k, x_k) = Q[(k + φ) mod 4], where
   Q = [(short, 227), (long, 228), (short, 228), (long, 227)].

   The same φ must hold for all 12 fields, that is for three whole cycles.
   This grades "repeating" and "back to the beginning". Which field of the
   pattern comes first is not tied to anything on the page, so φ is free.
   Line-length alternation inside a field is p23's statement and is not
   graded here, but every line length is recorded.
5. **Preconditions.** PT-1 to PT-7. PT-6 and PT-4 are gates: when either
   is closed, P3 is reported as not graded.
6. **Contrasting case.** T-b (PAL). p23 says "In PAL, they are all 227
   long", so x_k = 227 for every field. No (long, 228) entry occurs and the
   check goes red. PT-1 to PT-7 still hold, because PAL fields alternate
   between 312 and 313 (P1) and 227 is a valid length. The contrasts for the
   preconditions are T-f (PT-1), T-e (PT-2), T-d (PT-3), T-c (PT-4), T-g
   (PT-5), T-a (PT-6; p24: long and short fields occur "interlace only") and
   T-h (PT-7).
7. **Reference frame.**
   - Line marks at fixed horizontal positions (section 2.7).
   - The WAIT position anchors the Copper marks (p22) and DDFSTRT anchors the
     bitplane marks (p60). Their offset Δ is checked by PT-3.
   - Line 0 of the next field is anchored at the start of vertical blanking
     (p219), checked by PT-4.
   - The last line is N_k - 1, checked by PT-2.

### P4: interlaced bitplane retrieval with a separate Copper list per field

1. **Proposition.** Each field has its own Copper list, and the lists are put
   in COP1LC in turn as p35 describes. One list points bitplane 1 at START
   and the other at START + 40, one line further on. The field that runs the
   first list retrieves memory lines 1, 3, 5, ... in Figure 2-1 numbering.
   The field directly after it retrieves lines 2, 4, 6, ... Each such pair of
   fields therefore retrieves every line of the picture exactly once, from a
   memory that holds the lines in screen order.
   - Claims: `ch2-using-the-copper-in-interlaced-mode.c10`, `.c13`, `.c14`,
     `.c15`, `.c16`, `.c17`, `.c18`, `.c20.s2`;
     `COP2-using-the-copper-in-interlaced-mode.c12`, `.c13`, `.c15`, `.c16`,
     `.c17`, `.c19`. Declined, and printed as such by the run (section 1):
     `ch2-using-the-copper-in-interlaced-mode.c8`, `.c9`, `.c11`, `.c12`, `.c19`;
     `COP2-using-the-copper-in-interlaced-mode.c8`, `.c9`, `.c10`, `.c14`, `.c18`.
   - Pages 34 and 35; p62 describes the same layout.
2. **Stimulus.** Program I (I-0), with PIC tagged by line and the lists
   LISTA / LISTB of section 3.2.
3. **Observation.** For every port-side field [R_k, R_{k+1}) among the 8
   recorded fields:
   - the list the Copper restarted with (LISTA or LISTB, from the address of
     the restart fetch);
   - the bitplane fetch addresses, grouped by line;
   - the first 20-word bitplane fetch group of the field, found wherever it
     lies from PIC upward (the identity-mapped data area; the program's own
     code lies below PIC), by the search P5 uses, so that a first group
     outside PIC's window is measured at its real address and not skipped.

   A field running LISTA is an **A-field**; one running LISTB is a
   **B-field**. A **frame** is an A-field together with the B-field directly
   after it.
4. **Expected.**
   - (a) The first 20-word bitplane fetch group of every A-field starts at
     PIC, and of every B-field at PIC + 40; both kinds of field occur (the gate
     (e) shows it). p34: "The system retrieves data for bitplane
     displays by using pointers to the starting address of the data in
     memory."
   - (b) For every two consecutive recorded fields, which alternate between
     the two lists, the B-field's starting address (the start of its first
     20-word group) minus the A-field's starting address is 40 bytes, one line of a 320-pixel picture (40 bytes
     per line, p53), whichever of the two comes first; both orders occur.
     p34: "the starting address for the even-numbered fields is one line
     greater than the starting address for the odd-numbered fields.
     Therefore, the bitplane pointer must contain a different value for
     alternate fields of the interlaced display."
   - (c) The recorded fields alternate between the two lists. In every
     A-field, fetch group i (i = 0 ... 255) reads memory line 2i. In every
     B-field, group i reads memory line 2i + 1. This is Figure 2-1: odd field lines 1, 3, 5; even field lines 2, 4, 6; data in memory
     1 to 6 in order.
   - (d) Every two consecutive recorded fields (an A-field and a B-field,
     in either order) together fetch memory lines 0 ... 511, each exactly
     once. Taken in the order A-group 0, B-group 0, A-group 1, B-group 1,
     ... A-group 255, B-group 255, the group addresses rise by 40 bytes at
     every step: from A-group i to B-group i and from B-group i to A-group
     i + 1. This grades memory only: odd and even lines are interlaced
     together in the memory that the two fields fetch (p34: "(i.e., odd and
     even lines are interlaced together)"). Where the lines land on the screen
     is not observed, and the sentence's statement that memory matches the
     screen (`ch2-using-the-copper-in-interlaced-mode.c19`,
     `COP2-using-the-copper-in-interlaced-mode.c18`) is declined (section 1).
   - (e) Over the recorded fields the restart list strictly alternates
     between LISTA and LISTB, so every B-field is the field directly after
     an A-field. No page fixes which list the first recorded field runs. (e) is the
     gate of (a) to (d): where the fields do not alternate, (a) to (d) are not
     graded.
     p34: "This is accomplished by having a separate Copper instruction
     list for each field to manage displaying the data." Which field is the
     odd one, and the "time t" and "time t + 16.6ms" of Figure 2-1, are not
     graded (section 1).

   Not graded, because no page states them:
   - which hardware field (long or short, LOF 1 or 0) runs LISTA;
   - where the lines land on the screen.
5. **Preconditions.** PI-1 to PI-6, PI-7(bpl) and PI-12(bpl); for (a) to (d)
   also the gate (e). The proposition attributes the retrieval to the BPL1PT
   MOVEs of each field's list, and PI-7(bpl) proves that those MOVEs are
   fetched before the field's first bitplane fetch; with the MOVEs after the
   first fetch the retrieval is not theirs. PI-12(bpl) shows that they are
   fetched in vertical blanking.
6. **Contrasting cases.**
   - I-b (LISTB also writes PIC). The page requires "a different value for
     alternate fields". With the same value, the even memory lines 1, 3, ...,
     511 are never fetched: (b), (c) and (d) go red. By p34's pointer
     sentence ("The system retrieves data for bitplane displays by using
     pointers to the starting address of the data in memory"), a B-field
     whose list wrote PIC starts its retrieval at PIC, so (a), as worded
     ("of every B-field at PIC + 40"), goes red as well. (e) stays green,
     because the restart list still alternates LISTA, LISTB. I-b is (a)'s
     own contrasting case. The comparators overlap: (a) implies (b) and (c)
     implies (d), so I-b turns (a), (b), (c) and (d) red together and no run
     separates them. P5 stays green on I-b: it reads the address the list
     wrote, and LISTB wrote PIC.
   - I-d (the handler does not change COP1LC). COP1LC keeps LISTA, so every
     field runs LISTA (p25: the restart uses "the address contained in
     COP1LC"). (e) goes red, which closes its gate: (a) to (d) are not graded
     (two consecutive fields fetch only the even-numbered memory lines, and
     there is no B-field to grade). PI-5 still holds, because the handler
     still runs.
   - I-p (BPL1MOD = BPL2MOD = 38; both lists still write PIC and PIC + 40).
     A-group i to B-group i still rises by 40 bytes, and B-group i to
     A-group i + 1 rises by 38: the second step of (d) and the lines-once
     clause go red while the first step stays green, which the mutation run
     prints clause by clause. (a) and (e) stay green. (c) goes red as well, and
     (c) implies (d), so no run turns (d) red alone.
   - The contrasts for the preconditions are I-j, I-f, I-a, I-h, I-g, I-k, I-l
     and I-t. On I-a (LACE clear), PI-3 is red and P4 is reported as not
     graded; on I-l (the BPL1PT MOVEs after the `WAIT $2D01,$FF00`), PI-7(bpl)
     is red and P4(a) to P4(e) are reported as not graded; on I-t (the BPL1PT
     MOVEs after the `WAIT $2001,$FF00`, line 32), PI-12(bpl) is red and P4 is
     reported as not graded.
7. **Reference frame.**
   - Field: the restart interval (M2).
   - Frame: an A-field and the next B-field.
   - Line numbering: Figure 2-1, where memory line j is screen line j + 1.
   - Anchor checks: PI-2, PI-4 and PI-7(bpl).

### P5: bitplane pointers rewritten during vertical blanking start every field at the rewritten address

1. **Proposition.** When the bitplane pointer is rewritten during vertical
   blanking, the retrieval of every field starts at the rewritten address.
   When it is not rewritten, the next field's retrieval does not start there.
   - Claims: `ch2-using-the-copper-in-interlaced-mode.c16` and
     `COP2-using-the-copper-in-interlaced-mode.c15` (P5, the rewrite half),
     `ch2-putting-together-a-copper-instruction-list.c3` (P5-NOREWRITE, the
     conjunction of both halves).
   - Page 27.
2. **Stimulus.** Program I (I-0). Each list rewrites BPL1PTH/L first thing
   after the restart. For P5-NOREWRITE also Program I with the variant I-c,
   run directly in the normal run.
3. **Observation.** The address of the first bitplane fetch of each of the 8
   recorded fields. PI-7(bpl) confirms that the rewrite precedes it.
4. **Expected.** The first fetch is at the address the field's list wrote
   into BPL1PT, read from the list as the program assembled it: PIC in every
   A-field and PIC + 40 in every B-field of the program above. p27: "the
   bitplane pointers used in playfield displays and the sprite pointers must
   be rewritten during the vertical blanking interval so the data will be
   properly retrieved when the screen display starts again."

   `ch2-putting-together-a-copper-instruction-list.c3` says "must be
   rewritten", which holds both halves: with the rewrite the retrieval starts
   at the rewritten address, and without it the next field's retrieval does
   not start there. P5-NOREWRITE grades both in one assertion: the P5 check on
   Program I, and on a run of I-c that no recorded field after the first
   starts its first 20-word fetch at the address its list would have written
   (PIC for an A-field, PIC + 40 for a B-field). The first recorded field is
   not graded in the second half, because it may be the first field after
   DMA-on, which starts at the pointer the CPU wrote.
5. **Preconditions.** PI-1, PI-2, the populated 20-word search and, where
   the lists hold the BPL1PT MOVEs, PI-7(bpl) and PI-12(bpl); for
   P5-NOREWRITE also PI-10.
6. **Contrasting case.** I-c (no rewrite; the CPU wrote PIC once before DMA
   was turned on).
   - Over the whole run, the first field after DMA-on starts at PIC.
   - Every later field starts at an address that is neither PIC nor PIC + 40,
     so the check goes red for all recorded fields. p63: "Resetting is
     necessary because the pointers have been incremented to point to each
     successive word in memory and must be repointed to the first word for
     the next display."
   - p61 predicts the exact address (the last fetch of the previous field +
     2 + modulo). That address is recorded but not graded.
   - PI-1 and PI-2 still hold. PI-4, PI-6, PI-7(bpl) and PI-12(bpl) do not
     apply and are not graded: PI-4 counts groups inside PIC's window, which
     the fields' fetches leave (PI-6 is graded only where PI-4 holds), and
     there is no rewrite for PI-7(bpl) and PI-12(bpl) to order or to place.
     The first 20-word fetch of every field is found wherever it lies, and
     that every field has one is a live control. For lists that hold no
     BPL1PT MOVEs the expected address is the one the list would have
     written: PIC for an A-field, PIC + 40 for a B-field.

   P5-NOREWRITE's contrasts are I-c in the place of the rewrite run (the first
   half goes red; the second half is a live control) and I-0 in the place of
   the no-rewrite run (the second half goes red; the first half is a live
   control). The contrast of PI-10 is I-q.

   The contrasts for the preconditions are those of section 3.6, I-l for
   PI-7(bpl) and I-t for PI-12(bpl).
7. **Reference frame.** The restart interval (M2).

### P6: sprite pointers rewritten during vertical blanking make the sprite data be fetched in every field

1. **Proposition.** When sprite pointer 0 is rewritten during vertical
   blanking, the sprite's data is fetched in every field. When it is not,
   later fields do not fetch it.
   - Claim: `ch2-putting-together-a-copper-instruction-list.c4`
     (P6-NOREWRITE, the conjunction of both halves; the P6 line grades the
     rewrite half and carries no claim).
   - Page 27.
2. **Stimulus.** Program I (I-0). Each list rewrites SPR0PTH/L after the
   restart, and SPREN is set. For P6-NOREWRITE also Program I with the
   variant I-e, run directly in the normal run.
3. **Observation.** The fetches of the words at SPR + 4, SPR + 6, SPR + 8
   and SPR + 10 (the two data lines) in each of the 8 recorded fields.
4. **Expected.** All four data words are fetched in every recorded field.
   - p27, the sentence quoted in P5.
   - p110: "If you want the sprite to be displayed in subsequent display
     fields, you must rewrite the contents of the sprite pointers during each
     vertical blanking interval."
5. **Preconditions.** PI-1, PI-2, PI-8 and, where the lists hold the SPR0PT
   MOVEs, PI-7(spr) and PI-12(spr); for P6-NOREWRITE also PI-11.
6. **Contrasting case.** I-e (no rewrite; the CPU wrote SPR once before DMA
   was turned on). p110: "during the display field, the pointers are
   incremented to point to the data which is being fetched".
   - Over the whole run, the structure at SPR is fetched in the first field
     after DMA-on, so PI-8 holds.
   - None of SPR ... SPR + 14 is fetched in any recorded field, so the check
     goes red.
   - PI-7(spr) does not apply, because there is no rewrite.

   P6-NOREWRITE grades both halves in one assertion: the P6 check on Program
   I, and on a run of I-e that none of the four data words is fetched in any
   recorded field after the first (the first recorded field is not graded, as
   for P5-NOREWRITE). Its contrasts are I-e in the place of the rewrite run
   (the first half goes red; the second half is a live control) and I-0 in the
   place of the no-rewrite run (the second half goes red; the first half is a
   live control). The contrast of PI-11 is I-r.

   The contrasts for the preconditions are I-j, I-f, I-m, I-u and I-i.
7. **Reference frame.** The restart interval (M2).

---

## 6. Where the pages disagree or leave things open

1. **NTSC interlaced line count.**
   - p34: "an interlaced NTSC display has 524 lines" and "262 lines at a
     time".
   - p24: "the fields are 262, then 263 lines".
   - p79: "=525-(21\*2)"; p304: "NTSC (525 lines ...)".

   Only the agreed part is graded (P2). The four-field pattern (P3) is graded
   behind a gate.
2. **"Twice".** p34 calls the interlaced display "twice the normal number of
   vertical lines" and in the same paragraph gives PAL as 312 and 625.
3. **When the Copper restarts.** p25 says, in one note, "At the start of each
   vertical blanking interval, COP1LC is automatically used" and "when the
   end of vertical blanking occurs, the Copper is automatically forced to
   restart". p30 and p32 say start; p33 says COP1LC is written "at the end of
   the vertical blanking period" by the operating system.
   - M2 works under either reading.
   - P3's measurement of the last line needs the restart on line 0 before
     position $E0. PT-4 gates it instead of assuming it: when it does not
     hold, PT-5 and P3 are not graded.
4. **Who resets COP1LC.** p35: "during the vertical blanking interval, COP1LC
   will be automatically reset to point to the original list". This is not
   something p25 says the hardware does, and p33 gives the task to a graphics
   interrupt handler. In Program I the handler does it (section 3.3).
5. **The COPJMP1 strobe.** p30's text says "write to its strobe address",
   but its code reads it (`MOVE.W COPJMP1(a1),d0`). Appendix B (p287)
   defines a strobe as "Writing the register causes the effect". The programs
   here write it.
6. **The end of the field.** p27 places "the end of the display" at
   "(226,262) NTSC (or (226,312) PAL)", which read as inclusive coordinates
   means 263 and 313 lines. p24, p34 and p79 count 262 and 312. p100 allows
   sprite start positions "from line 0 to line 262". For this reason the
   non-interlaced field length is recorded but not graded (P1, contrast).
7. **The length of vertical blanking.** p79 gives "Vertical Blank Stop $15
   (21)" NTSC and "$1D (29)" PAL. p219 gives a minimum of "20 horizontal scan
   lines for an NTSC system and 25 horizontal scan lines for a PAL system".
   p239 speaks of the "end of internal vertical blanking (vertical position
   20)". Nothing here depends on the exact value, except PI-12, which uses the 25-line
   minimum.
8. **Odd and even against the hardware fields.** No page ties the odd field
   (the "first scan") to LOF or to the long or short field. p216 says only
   that LOF is "Used to initialize interlaced displays". On p49-50 the text
   layer labels Figure 3-5's fields "Odd Field" and "Even Field", but the
   printed figure labels them "Field 1" and "Field 2".
9. **Waiting beyond line 255.** p24's recipe (WAIT for (0,255), then WAIT for
   vertical 0 through 5) does not say how the second WAIT avoids ending at
   once. By p23 and p27 a WAIT ends as soon as the beam is "equal to or
   greater than" the position, and at line 255 the counter's low bits are
   already above 0 through 5. Program T therefore marks lines at or above 255
   with bitplane fetches, not with Copper WAITs.
10. **Figure 2-1's "16.6ms".** It corresponds to 261.2 NTSC lines, which is
    not a whole-line field (section 1).
11. **Table 3-8 (p53, checked on the scan).** It gives 8,192 bytes for a
    320 x 256 bitplane and 16,384 for 320 x 512. At 40 bytes per line (p53)
    these are 10,240 and 20,480. The table is not used; PIC is 512 x 40 =
    20,480 bytes.

---

## 7. Build

Files: `prog_i.s` (Program I) with variants `prog_i_a.s` to `prog_i_v.s`
(I-a to I-v, section 3.5); `prog_t.s` (Program T) with variants `prog_t_a.s`,
`prog_t_c.s` to `prog_t_k.s` (T-a, T-c to T-k, section 4.4; T-b is the same
binary as T-0 with PAL selected in the harness, not a separate source);
`m1.i` (Measurement M1, shared); `ch2.i` (register offsets Appendix B gives
that `nanomig.i` does not, and the CIA-A address used only to release the
reset-time overlay so Program I can install its own interrupt vector at
$6C, independently of Appendix B); `listt_body.i` (LISTT's 255 WAIT/MOVE
pairs, generated once, not by hand); `test_ch2_interlace_fields.cpp`.

**Clock ratio.** Every check that compares a cycle count against colour
clocks (p194, section 2.3) converts by a fixed 8 clk_sys cycles per colour
clock (the harness's own clock generator ratio, not a value fitted to a
measurement).

**M1_DELAY.** The precondition contrasts I-j and T-f set `M1_DELAY=3000`,
a busy-wait of 3000 iterations inside the M1 sampling loop. It is not a
page value; it is guarded by the very check it is meant to break (PI-1,
PT-1), and by design several times longer than one line, so the contrast
does not depend on its exact size. Measured, per core: the first M1
record's own first V (which the page ties only to a wrap, section 2.4)
is 12 (fx68k) and 35 (tg68k) under I-j, and 122 (fx68k) and 3 (tg68k)
under T-f -- several lines' worth either way, on both cores. After the completion word
the run goes on until the next fetch of the first word of a list (section
2.5), so that the last field of an I-j run is complete: measured on both cores,
PI-2, PI-4 and PI-5 hold under I-j, PI-1 is the only red, and PI-3, which
reads the M1 lengths, is not graded.

**Measured (recorded, not graded).**
- P1 (Program I, both cores): the 8 recorded PAL field lengths are
  313, 312, 313, 312, 313, 312, 313, 312.
- Program T's Delta (section 2.7, both cores): 1288 clk_sys cycles
  (161 colour clocks).
- P3's last-line length x_k (section 2.7, both cores): 227 on every one
  of the 12 graded fields, never 228; P3 measures red on both cores. PT-5
  (every measured length is 227 or 228) is unaffected and stays green,
  since 227 is one of the two page values.
- The normal run prints Delta, the type f_k and the length x_k of each of
  the 12 graded fields, and the number of measured line lengths that are 227
  colour clocks, 228 colour clocks and neither, as MEASURED lines before the
  P3 line.

**PI-7(bpl)'s own contrast (I-l).** Measured on both cores: with the
BPL1PT MOVEs read at their real list offset (after the SPR0-7PT MOVEs and
the WAIT $2D01,$FF00), the fetch of BPL1PTH lands at 10,215 colour clocks
after the restart -- line 45 ($2D) -- while the field's first bitplane
fetch comes earlier, at 10,049 colour clocks (line 44). PI-7(bpl) is red
in every field under I-l, on both cores, as p22's WAIT semantics and
p35's DIWSTRT ($2C81, line 44) predict. Under I-l the gates close and the
lines P4(a) to P4(e) and P5 print as not graded, with PI-1 to PI-6 held.

**The sampling blackout (PI-1, PT-1).** M1 samples the beam position in a
tight loop. Whatever keeps the CPU away from that loop for so long that the
next accepted sample lies two or more lines past the previous one sets the
gap flag that PI-1 and PT-1 grade; an absence of more than one line does so
or not according to where it begins. Two things keep the CPU away: the wrap routine of M1 and the level-3
handler of Program I. M1 therefore keeps its run state (record pointer, wrap
count, record count, gap flag) in address registers and touches chip memory
only for the record words and the two markers, and the handler keeps the
address of the list COP1LC receives next in a register. Measured on both
cores: PI-1 and PT-1 are graded in the normal runs and hold there, and
each contrast run that prints them as a live control (among them I-a, T-a and
T-g) shows them holding in that run.

**PI-4 in I-c.** With the BPL1PT MOVEs left out, the fetches of the later
fields lie outside PIC's window. PI-4 (which counts groups inside that window),
PI-6 (graded only where PI-4 holds), PI-7(bpl) and PI-12(bpl) are not
applicable in I-c and print as not graded on both cores. The first 20-word
fetch of every field is found wherever it lies instead. p61's rule predicts the
first address of field 1 from the last fetch of field 0 + 2 + the modulo;
measured on both cores, the prediction (573440) equals field 1's first
fetch (573440).

**PI-2 and PT-7 (I-s, T-j).** The anchor of section 2.6 reads the terminal
wait from the list as assembled. Measured, both cores: in the normal runs 8 of
8 (Program I) and 13 of 13 (Program T) M1 fields hold exactly one restart, each
preceded, among the list words up to the terminal wait, by the Copper's fetch
of the terminal wait, and the matched restarts are consecutive. In every
field, 2 colour clocks before the restart, the Copper also reads the word after
the terminal wait ($080854 after LISTA's, $080954 after LISTB's, $081004 after
LISTT's); the anchor does not count it. Under I-s and T-j every field still
holds exactly one restart and the matched restarts are still consecutive, but
no restart is preceded by a fetch of the terminal wait (0 of 8, 0 of 13). No
page says that the Copper stops after a MOVE it cannot write; read literally,
p25 ("Instruction fetch continues sequentially until the Copper is interrupted
by another jump address strobe") and p30 ("No stop instruction is provided for
the Copper") would let it go on to the terminal wait and leave PI-2 and PT-7
green. I-s and T-j are therefore not page-predicted contrasts and not
mutations of PI-2 and PT-7: they are self-tests of the anchor's third clause
(`SELFTEST-PI-2-ANCHOR`, `SELFTEST-PT-7-ANCHOR`). Measured on both cores, the
Copper fetches the MOVE's first word, fetches nothing more of the list, and
reads the MOVE's second word 2 colour clocks before the restart; on this
design the comparator therefore goes red through the third clause alone, with
everything else holding. PI-2 and PT-7 keep their page-based contrasts, I-f
and T-h, and the third clause has its own, page-predicted contrast, I-v and
T-k (section 2.6). Measured on both cores, under I-v 8 of 8 and under T-k 13
of 13 M1 fields hold exactly one restart, the matched restarts are
consecutive, and no restart is preceded by a read of a terminal wait: the
Copper is in the middle of the list when the restart comes. Only the third
clause goes red; every other precondition holds.

**PI-12 (I-t, I-u).** Measured on both cores: the Copper fetches the two BPL1PT
MOVEs 0 and 4 and the two SPR0PT MOVEs 8 and 12 colour clocks after the
restart, and the write that opens the M1 field follows the restart by 165 to
259 (fx68k) and 99 to 185 (TG68K) colour clocks. The later MOVE fetch therefore
lies 161 to 255 (BPL1PT) and 153 to 247 (SPR0PT) colour clocks before that write
on fx68k, and 95 to 181 and 87 to 173 on TG68K, against the limit of 5,221
colour clocks (23 lines). Under I-t the later BPL1PT fetch lies 7,053 to 7,083
(fx68k) and 7,064 to 7,164 (TG68K) colour clocks after the write, under I-u the
later SPR0PT fetch 5,464 to 5,494 (fx68k) and 5,475 to 5,573 (TG68K); PI-7(bpl)
and PI-7(spr) hold there. The sprite's first fetch comes 5,694 colour clocks
after the restart (line 25, colour clock 19 at 227 colour clocks per line) and
the first bitplane fetch 10,049 (line 44), the same on both cores.

**T-g.** Line 10's WAIT uses `(10, $40)`. Measured on both cores, field 0's
line 9 then measures 536 clk_sys (67 colour clocks); 24 of the 3150 line
lengths measured over the 12 fields fall outside 227 and 228 colour clocks;
PT-1, PT-3 and every Copper mark the line lengths need stay live.
