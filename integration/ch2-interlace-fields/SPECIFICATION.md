# Specification: interlaced fields (Chapter 2, pages 24, 27 and 34)

This specification was written only from the package's claims
(`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference
Manual*, 3rd edition. Its author never saw the implementation or any test.
It defines six propositions (P1 to P6; P3 has a second line, P3(pattern)), the
two 68000 programs that exercise them, what each check observes and where, the expected value together with the
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
| `ch2-using-the-copper-in-interlaced-mode.c19`, `COP2-using-the-copper-in-interlaced-mode.c18` ("the organization of the data in memory matches the apparent organization on the screen") | These two say that memory matches the screen, and where the lines land on the screen is not graded (the reason of the row above: no page says what an interlaced field looks like at the test's video output). P4(d) observes only the memory addresses the bitplane DMA fetches, so a design that fetched the stated addresses but displayed the lines in a different screen order would pass it. Declined. |
| `ch2-using-the-copper-in-interlaced-mode.c15` ("In Figure 2-1 the data in memory holds lines 1 through 6 consecutively in ascending order"), `COP2-using-the-copper-in-interlaced-mode.c19` ("Odd and even bitplane lines are interlaced together in memory") | Both state how the data is laid out in memory. The layout is built by the program (PIC, section 3.1) and stays the same in both contrasts of P4(d) (I-b, I-p): changing a Copper list or the modulo turns P4(d) red while both claims remain true. P4(d) grades the order in which the bitplane DMA retrieves the lines of that memory over two consecutive fields, which neither id says. Declined whole; P4(d) stays graded and carries neither id. |
| `ch2-using-the-copper-in-interlaced-mode.c10` ("The scanning circuitry treats an interlaced display as two display fields, one holding the even-numbered lines and one holding the odd-numbered lines"), `ch2-using-the-copper-in-interlaced-mode.c13`, `.c14` ("In Figure 2-1 the odd field holds display lines 1, 3 and 5", "the even field holds display lines 2, 4 and 6"), `COP2-using-the-copper-in-interlaced-mode.c12`, `.c13` ("One interlaced display field contains the even-numbered lines", "... the odd-numbered lines") | These five say which lines a field displays. P4(c) observes only the addresses the bitplane DMA fetches in each field. A design that fetched the same addresses but sent the lines to the wrong field, or to the wrong place on the screen, would pass it, and the test sees video only after a line doubler (the reason of the on-screen row above). Declined whole, as the neighbouring on-screen ids are; P4(c) stays graded as the retrieval order of the memory lines and carries none of them. |
| `COP2-vertical-beam-position.c15` ("The first field length in the PAL alternation is 312 lines"), `COP2-vertical-beam-position.c16` ("The next field length in the PAL alternation is 313 lines") | p24 says "in PAL, 312, then 313 lines" and no page ties a recorded field to the first of the alternation: the capture opens at a wrap that no page ties to a field type (section 6, item 8). P1 accepts either phase and grades that the lengths are 312 or 313 and differ on every consecutive pair; it therefore establishes neither "first" nor "next". Declined whole; P1 carries neither id. |
| `ch2-putting-together-a-copper-instruction-list.c3` ("The bitplane pointers used in playfield displays must be rewritten during the vertical blanking interval so the data is properly retrieved"), `.c4` (the same for "the sprite pointers") | Both are plural and speak of properly retrieved data. P5-NOREWRITE observes bitplane pointer 1 and the address of its first 20-word fetch group; P6-NOREWRITE observes sprite pointer 0 and four data words of its structure. Neither exercises another active bitplane pointer or sprite pointer, and neither compares the data a whole field or a whole sprite structure returns. Declined whole; P5-NOREWRITE and P6-NOREWRITE stay graded, for BPL1PT and for SPR0PT, and carry neither id. |
| `ch2-using-the-copper-in-interlaced-mode.c11` ("In Figure 2-1 the odd field is the field shown at time t") | No page ties the odd field to anything the hardware exposes, and P4e accepts either list as the first recorded field, so P4e establishes an alternation and never which field is the odd one or which lies at time t. Declined; P4e keeps `ch2-using-the-copper-in-interlaced-mode.c20.s2`. |
| `COP2-using-the-copper-in-interlaced-mode.c14` ("The illustrated even field occurs 16.6 milliseconds after the odd field"), and the "16.6 ms" in `ch2-...c12` | The figure label gives no precision and no tolerance. At the NTSC clock of p24 (3,579,545 Hz) and 227.5 colour clocks per line (p23, p194), 16.6 ms is 261.2 lines, and no field of whole lines lasts that long. The manual's own figures give 16.65 ms for a 262-line field and 16.72 ms for a 263-line field. The only gradable content, that the even field is the field directly after the odd field, is graded in P4(e) under `ch2-using-the-copper-in-interlaced-mode.c20.s2`; `ch2-using-the-copper-in-interlaced-mode.c12` is declined whole, as it bundles the 16.6 ms figure with that ordering claim. |

Every id in this section is printed in full by the normal run as a `RECORDED:` line that gives its reason. Every other claim is used. The mapping is:

| Proposition | Claims |
|---|---|
| P1 | `COP2-vertical-beam-position.c12`; `COP2-using-the-copper-in-interlaced-mode.c5`, `.c6`, `.c11`; `ch2-using-the-copper-in-interlaced-mode.c6`; with the LACE-clear control, `ch2-vertical-beam-position.c12` (P1-NOLACE) |
| P2 | Agreed part of `COP2-using-the-copper-in-interlaced-mode.c3`, `.c7` and `COP2-vertical-beam-position.c14` |
| P3 | none: P3 grades p23's and p24's alternation of long and short lines over the complete sequence of lines of every field |
| P3(pattern) | `ch2-vertical-beam-position.c15`, `.c16`, `.c17`, `.c18`, `.c19`; `COP2-vertical-beam-position.c17`, `.c18`, `.c19`, `.c20`, `.c21`, `.c22` (the NTSC four-field pattern, its four fields and its repetition) |
| P4 | (a) `ch2-using-the-copper-in-interlaced-mode.c16`, `COP2-using-the-copper-in-interlaced-mode.c15`; (b) `ch2-using-the-copper-in-interlaced-mode.c17`, `.c18`, `COP2-using-the-copper-in-interlaced-mode.c16`, `.c17`; (e) `ch2-using-the-copper-in-interlaced-mode.c20.s2`; (c) and (d) carry no id |
| P5 | `ch2-...-interlaced-mode.c16`, `COP2-...-interlaced-mode.c15`; P5-NOREWRITE carries no id |
| P6 | none: P6 and P6-NOREWRITE carry no id (`ch2-putting-together-a-copper-instruction-list.c4` is declined above) |

---

## 2. Ground rules shared by every check

### 2.1 System setting

Enhanced Chip Set (the default), 2 MB Chip memory, PAL unless a program says
NTSC. No page distinguishes OCS from ECS for these claims. Each check is
graded once per CPU core, and no expected value depends on the core. Both
programs use only 68000 instructions and run each core in its default 68000
mode. The level-3 autovector is used at $6C, which assumes the vector base is
at its reset value of 0.
`DIWHIGH` is never written, in any run, and `BEAMCON0` is never written in the
normal runs (Program I, I-0, and Program T, T-0) or in any variant except T-l,
T-m and the T-n family (T-n, T-q, T-r, T-s, T-t; section 4.4), which write
`BEAMCON0 = $0080` (VARBEAMEN) and `HTOTAL` to change a line length, and are the only
runs of the package that leave this regime. p305 says of `BEAMCON0` "Do not write to this
register" (the warning is addressed to programs that change the scan rate of a monitor,
section 4.5, PT-5), and p306 says that if `DIWHIGH` "is not written, the old
scheme for DIWSTRT and DIWSTOP described above holds".

`VTOTAL` is never written, in any run. p304 (read on the scan) says of it: "VTOTAL contains the line
number at which to reset the vertical position counter. This value represents the number of lines in a
field(+1). The exception is if the INTERLACE bit is set (BPLCON0). In this case this value represents
the number of lines in the long field (+2) and the number of lines in the short field (+1)." The page
does not say whether these offsets are added to the number of lines or taken from it, so it gives no
value for the 262 and 263 lines of an interlaced NTSC field, and p298 lists the register without one. The
records T-l, T-m and the T-n family therefore rest on the value `VTOTAL` has after reset, which the
design chooses; the number of lines it produces is printed by the gate PT-6 on every run
(N_0 ... N_12 = 263, 262, ...), and it is not a page value.

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
- before each of those K restarts, the Copper has run through the list as far
  as the field lets it, which is the stretch the definition above requires. In
  Program I the lists end in a terminal `$FFFF,$FFFE` wait that the Copper
  reaches long before the field ends: the Copper's last read of a list word up
  to the lists' terminal waits is a read of a terminal wait (a restart of one
  list follows the other list's wait). The terminal waits are found in the lists
  as the program assembled them. A read of the word after a terminal wait is not
  a list fetch for this clause; measured on both cores, the Copper makes one
  such read 2 colour clocks before every restart (section 7). In Program T the
  Copper has marks up to the last line of the field (section 2.7) and is busy
  up to the restart, which comes right behind the mark of the last line, so
  that neither the terminal wait nor the WAIT for the line after the last one is
  read in any field (section 7). The clause there is: the Copper's last read of a word of the list
  it ran, before each of those restarts, is a read at or beyond the last word of
  the instruction in front of the first WAIT of that list that waits for a line
  the field does not have (line >= N_k, from the lines the list's WAITs name, in
  the order of the list), or in front of the terminal wait if there is none; the
  field's lines are known from M1 (PT-1). The lists of T-0 hold a mark for every line
  up to 312, so the first WAIT for a line the field does not have is the WAIT for line
  N_k, and the word the clause asks for is the second word of the mark of line N_k - 1. The list of Program T
  leaves the Copper no idle stretch in front of its terminal wait, which is why the clause
  differs there; its contrast is the same (T-k).

The third clause has a page-predicted contrast. p25 says that "no matter what
the Copper is doing" it is forced to restart when vertical blanking ends, and
p20 gives four memory cycle times for a MOVE. A list that waits for line 255
and then holds more MOVEs than fit between line 255 and the end of the next
field's vertical blanking at its latest (line 29 PAL, line 21 NTSC, p79) is
therefore restarted in the middle of the list under either reading of p25
(section 6, item 3), and the last list word the Copper read
before the restart is a MOVE word in the middle of the run, not the word the
clause asks for (I-v for Program I, T-k for Program T; sections 3.5 and 4.4). The
first two clauses hold there and the third fails.

Pages disagree on whether the restart is at the start or the end of vertical
blanking (section 6); one restart per M1 field holds under either reading.

### 2.7 Measurement M3: line marks (Program T only)

Every line gets a timestamp taken at a fixed horizontal position. There are two
kinds of mark:

- **Copper mark** m_c(k, v): the port time of the fetch of the first word of
  the `MOVE COLOR00` that directly follows `WAIT (v, $E0)` in field k, for
  v = 0 ... 262. p24: "WAIT for position (0,255)" and then "WAIT for any horizontal
  position with vertical position 0 through 5, covering the last 6 lines of the scan
  before vertical blanking occurs", "Thus the total of 256 + 6 = 262 lines of video
  beam travel during which Copper instructions can be executed". The list of Program
  T follows that recipe behind line 254: `WAIT (255,$E0)` and its mark, then three more
  MOVEs that carry the Copper past the end of line 255 (the second WAIT would
  otherwise end at once, section 6, item 9), then `WAIT (v - 256,$E0)` and a mark for
  each of the lines 256 ... 262. A field of 262 lines has no line 262.
- **Bitplane mark** m_b(k, v): the port time of the first bitplane fetch on
  line v of field k. The bitplane fetches of a field come in groups of 20
  words, one group per line. The i-th group (counting from 0) belongs to line
  44 + i: the display window of Program T opens at line 44 (VSTART = $2C, section
  4.3) and stays open to the last line, so the last line N_k - 1 of every
  field has a group.

Δ = m_c(k, v) - m_b(k, v) on the lines 44 ... 254 (PT-3) and on the lines 255 ...
N_k - 1 (PT-9), where both kinds of mark exist. The length of line v of field k,
observed directly, is:

- v = 0 ... 253: the time from the Copper mark of line v to the next Copper
  mark the list produces. With the full list, that is m_c(k, v+1) - m_c(k, v);
- v = 254 ... N_k - 2: m_b(k, v+1) - m_b(k, v). The Copper marks of those lines give
  the same lengths, m_c(k, v+1) - m_c(k, v); the run prints how many of them are equal
  to the bitplane lengths (MEASURED, not graded: a second source of the same
  lengths).

- v = N_k - 1 (the last line, fields 0 ... K-2, the field after it being recorded):
  y_k = m_c(k+1, 0) - m_c(k, N_k - 1).

Why y_k is the length of the last line. Both marks are Copper marks of the same
kind, taken at H = $E0 of their lines: the mark of line N_k - 1 of field k, and the
mark of line 0 of field k+1. Everything between them is the last line from H = $E0 to
its end and line 0 of field k+1 up to H = $E0, which is the length of the last line, whatever the
length of line 0 is. Only marks of one kind are subtracted from each other: Δ, the offset
between the two kinds, is not used. What the measurement assumes is what every other length
here assumes: a mark of one kind stands at the same offset in every line. For the Copper mark of
the last line this is shown by PT-9 (its offset to the bitplane mark of the same line is Δ, the value
PT-3 finds on the lines 44 ... 254), for the marks of lines 0 and 1 by PT-4 (they lie 227 or 228
colour clocks apart).

Section 6, item 12 gives the other bases that were weighed for the last line and why they are not used.

The last recorded field (k = K-1) has no field after it: its lines 0 ... N_k - 2
are in the sequence, its last line is not.

The **sequence** of field k is the list of its line lengths in order, lines 0 ...
N_k - 2 and, for k < K-1, y_k (the last line). P3 reads the sequences of all
fields and the transition from y_k to the first length of the sequence of field
k+1.

The Copper marks are anchored on the WAIT comparison (p22: "The WAIT
instruction causes the Copper to wait until the video beam counters are equal
to (or greater than) the coordinates specified in the instruction"). The
bitplane marks are anchored on the data-fetch start (p60). The anchor checks
are PT-3, PT-4 and PT-9 in section 4.

The test reads the Copper list as the program assembled it (stimulus data in chip memory, not a
fetch), instruction by instruction up to the terminal wait, and takes for the mark of line v the
instruction behind the last WAIT of line v in the list; a WAIT whose vertical position is lower than
the one before it is 256 lines further on (p23: "line 256 will appear as a zero in the comparison").
No address is computed from the position of a pair in the list.

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
3. Add 1 to HCNT in Chip memory. This is a CPU write, visible at the port with
   the new value as its data.
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
   LISTB. Set `HCNT = 0` with a CPU write (the first write to HCNT in the run,
   at the start of the program, in front of the handler).
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
| I-w | The handler adds 2 to HCNT, not 1 (it still toggles COP1LC and clears COPER) |
| I-x | The program writes `$0007` to HCNT before the handler is installed, not 0 |
| I-v | LISTA is at `$0A0000` and LISTB at `$0A6000`. In both, in front of the terminal `$FFFF,$FFFE`, a `WAIT $FF01,$FFFE` (line 255) is followed by 5,200 `MOVE COLOR00, bg` (four memory cycle times each, p20: 20,800 colour clocks, more than the 88 lines (19,976 colour clocks) from line 255 of a 313-line field through line 29 of the next) |

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
- PI-5 needs PI-13: the values it compares are counted from the known initial
  value; where PI-13 fails, PI-5 is not graded.
- PI-6 needs PI-4: where PI-4 holds, every field has its 256 groups of 20
  fetches inside PIC, so every field has fetches there for PI-6 to read.
- PI-7(bpl) needs a bitplane fetch in every field to order the MOVEs against
  (where one is missing, PI-4 fails). PI-7(spr) needs PI-8; where PI-8 holds, a
  field without a sprite-0 fetch fails PI-7(spr). PI-12 needs the matching PI-7.

| Id | Precondition | Fails in |
|---|---|---|
| FIX-I | The program writes `$BEEF` to FIXMARK before M1 starts, and the harness reads `$BEEF` back after the run: the fixture can deliver a known value. | I-n (`$DEAD`) |
| PI-1 | The completion word was written. All K M1 records are valid (section 2.5). Every record's last V is at least 255, so no wrap is spurious. | I-j |
| PI-2 | Anchor check of section 2.6, with LISTA and LISTB: one restart per M1 field, in order, each the first-word fetch of LISTA or of LISTB, preceded, among the list's words up to the terminal wait, by the fetch of the terminal `$FFFF,$FFFE` wait. | I-f (no restart); I-v (the third clause, page-predicted: the Copper is restarted in the middle of the list); I-s is a self-test of the third clause on this design only, not page-predicted (section 7) |
| PI-3 | Interlace is active: the check of P1 holds. It is also a precondition of P4, whose retrieval lines are graded only on an interlaced run. PI-3 and P1 read one comparator, so I-a turns PI-3, P1 and the first half of P1-NOLACE red together. | I-a |
| PI-4 | Every field contains exactly 256 groups of bitplane fetches inside PIC. Each group is 20 fetches of consecutive ascending word addresses. | I-h |
| PI-13 | HCNT starts from a known value: the first write to HCNT in the run is the program's write of 0, in front of every write by the handler (for PI-5). | I-x (`$0007`) |
| PI-5 | HCNT is written exactly once in every field, and every write in the run, from the first handler write on, carries the previous written value plus 1. Graded only where PI-13 holds. | I-w (the handler adds 2); I-g (COPER is not enabled, no field holds a write) |
| PI-6 | Every bitplane fetch from inside PIC returns the tag of the memory line at its address. Fetches outside PIC are not covered. | I-k |
| PI-7(bpl) | In every field, the Copper fetch of the BPL1PT MOVEs precedes the field's first bitplane fetch, the earlier of the first fetch inside PIC and the first 20-word group that P4(a), P4(b) and P5 find from PIC upward (for P4 and P5). | I-l |
| PI-7(spr) | In every field, the Copper fetch of the SPR0PT MOVEs precedes the field's first sprite-0 fetch (for P6). | I-m |
| PI-8 | The sprite-0 structure at SPR is fetched at least once during the run (for P6). | I-i |
| PI-12(bpl) (gate of P4 and P5) | In every field, the Copper fetch of the BPL1PT MOVEs lies in vertical blanking: less than 23 lines after the write that opens the M1 field. p25 leaves open whether the restart lies at the start or at the end of vertical blanking (section 6, item 3); after a restart at the end the MOVEs are fetched after line 25, so PI-12 is printed as a gate state and is not graded; when it is closed, P4 and P5 are not graded. | I-t |
| PI-12(spr) (gate of P6) | In every field, the Copper fetch of the SPR0PT MOVEs lies in vertical blanking, as for PI-12(bpl). It is printed as a gate state and is not graded; when it is closed, P6 is not graded. | I-u |
| PI-9 | The LACE-clear control run (I-a) is a valid fixture: PI-1 and PI-2 hold on that run (for P1-NOLACE). | I-o |
| PI-10 | The no-BPL1PT-rewrite run (I-c) is a valid fixture: PI-1 and PI-2 hold on that run, and the first 20-word fetch of every one of its fields is found (for P5-NOREWRITE). | I-q |
| PI-11 | The no-SPR0PT-rewrite run (I-e) is a valid fixture: PI-1, PI-2 and PI-4 hold on that run, and the sprite structure is fetched at least once (for P6-NOREWRITE). | I-r |

**PI-12.** p219 gives the minimum time of vertical blanking as 25 horizontal scan
lines for a PAL system, beginning at line 0 ("The range starts at line 0 and
ends at line ... 25 for PAL"; section 6, item 7). The write that opens an M1
field lies less than one and a half lines after the start of the field's
line 0: the sample that shows line 0 (section 2.5) lies in that line, and the
wrap routine writes the record a few instructions after it. A Copper fetch less
than 23 lines after that write therefore lies before line 24.5. The limit is that arithmetic,
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

- **PLANE**: 13,000 bytes of $0000 (a window from line 44 to the last line of a 313-line PAL field is 269 groups of 40 bytes, 10,760 bytes).
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
WAIT (255, $E0)        ; $FFE1,$FFFE
MOVE COLOR00, bg       ; mark, line 255
MOVE COLOR00, bg       ; three MOVEs that carry the Copper past the end of line 255
MOVE COLOR00, bg
MOVE COLOR00, bg
WAIT (0, $E0)          ; $00E1,$FFFE: line 256, the vertical comparison sees 0
MOVE COLOR00, bg       ; mark, line 256
... one WAIT (v - 256, $E0) + MOVE COLOR00, bg for each of the lines 257 ... 312 ...
WAIT $FFFF,$FFFE
```

`bg` is the value already in COLOR00, so the marks leave the picture
unchanged. The horizontal position $E0 lies after the line's bitplane fetches
(DDFSTOP $D0) and before the end of the shortest line ($E2, p23). A Copper WAIT compares
only V7-V0 (p24: "the comparison works on the least significant bits of the vertical
count"), and the list follows the recipe of p24 behind line 254: "WAIT for position
(0,255)", then a WAIT for vertical position 0 through 5, "Thus the total of 256 + 6 = 262
lines of video beam travel during which Copper instructions can be executed". The three
MOVEs behind the mark of line 255 are what keeps the next WAIT from ending at once (section 6,
item 9). The marks run to line 312 so that the PAL run (T-b), whose fields have 312 or 313 lines, has a mark on
every line as well; an NTSC field of 262 lines has no line 262. The restart comes right behind the mark of the
last line of the field (the mark of line 261 in a 262-line field, of line 262 in a 263-line field), so the WAIT
for the next line is not read (section 7).

### 4.3 CPU sequence

1. Select NTSC in the harness.
2. Write `$7FFF` to INTENA, DMACON and INTREQ.
3. Write the display registers:
   - `BPLCON0 = $1204`, `BPLCON1 = 0`, `BPLCON2 = 0`;
   - `BPL1MOD = BPL2MOD = 0`;
   - `DDFSTRT = $0038`, `DDFSTOP = $00D0`;
   - `DIWSTRT = $2C81`: VSTART = $2C = 44, so the window opens at line 44 and a data
     fetch group lies on every line from line 44 to the last line of the field.
     p79: "No data can be displayed in the vertical blanking area", with Vertical Blank
     Stop $15 (21) for NTSC and $1D (29) for PAL (Table 3-13); p219 gives the minimum
     vertical blanking as 20 lines for NTSC and 25 for PAL, and says that after that
     minimum range "you can control where the display actually starts by using the
     DIWSTRT (display window start) register to extend the effective vertical blanking
     time". p59 (Table 3-9) gives $00 to $FF as the possible values of VSTART. Line 44
     lies after the vertical blank stop of both standards, so the window does not ask
     for a fetch inside the vertical blanking;
   - `DIWSTOP = $39C1`. VSTOP is `$39` with its most significant bit forced
     to the complement of the next one, which gives `$139` = line 313. p59:
     "forcing the MSB of the stop position to be the complement of the next
     MSB". Line 313 lies beyond the last line of every NTSC and PAL field, so
     the window stays open to the last line of each field, and a data fetch group lies
     on each of them.
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
| T-g | The WAIT of line 10 uses `(10, $40)`, not `(10, $E0)`; every mark stays in the list. It moves a measuring mark and changes no physical line (a gating observation of PT-5's marks, section 4.5) |
| T-h | `DMACON = $8300` (COPEN not set) |
| T-i | FIXMARK stores `$DEAD`, not `$BEEF` |
| T-j | LISTT ends in `$0000,$0000` (as in I-s) in front of the `$FFFF,$FFFE` that stays in the list |
| T-k | LISTT, in front of its terminal `$FFFF,$FFFE`, waits for line 255 (`WAIT $FF01,$FFFE`) and then holds 1,800 `MOVE COLOR00, bg` (four memory cycle times each, p20: 7,200 colour clocks, more than the 30 lines (at most 6,840 colour clocks) from line 255 of a 263-line field through line 21 of the next; LISTT is relocated to `$0A0000`, clear of PLANE, with the code of M1 behind it). It has no Copper marks of the lines 255 and above (`listt_high.i` is left out), so PT-9 is not graded there |
| T-m | As T-l's setup (`BEAMCON0 = $0080`, `HTOTAL = $E2` from the CPU), but `listt_alt_body.i` replaces the list: every line v from 0 to 254 holds `WAIT (v,$40)`, `MOVE HTOTAL` (`$E3` for even v, `$E2` for odd v), then `WAIT (v,$E0)` and `MOVE COLOR00, bg` (the mark), so every write is early in its line and every mark is live; the marks of the lines 255 ... 262 follow as in T-0 and carry no HTOTAL write. This list writes HTOTAL on the lines 0 ... 254 only, by construction, so the last write stays in force above line 254, and the list restarts at the same phase in every field. Contrast of P3 (section 5, P3): the lines 0 ... 254 alternate between 228 and 227 colour clocks, the lines above 254 and the field boundaries do not |
| T-n | As T-l's setup (`BEAMCON0 = $0080`, `HTOTAL = $E2` from the CPU), then two Copper lists, one at LISTT and one at `$0A0000` (`listt_chain.i`), that alone write `HTOTAL` in every line 0 ... 262: `WAIT (v,$40)` and `MOVE HTOTAL`, `$E3` or `$E2` alternating from line to line, then `WAIT (v,$E0)` and the mark, as in `listt_alt_body.i` and with the lines 255 ... 262 as in `listt_high.i`. The lists carry the phase from one field into the next through COP1LC, which the Copper writes itself: at the start of a list it sets COP1LC to the list it restarts with when the field has no line 262 (the same list: a 262-line field ends on the other length than it began with, so the next field begins with the same phase), and in line 262, after `WAIT (6,$40)`, to the other list (a 263-line field ends on the length it began with, so the next field begins with the other phase). The CPU writes nothing after its setup; there is no HTOTAL write in any list at the lines 100 and 101. The alternation continues in the lines above 254 and across the field boundaries, whatever the length of the field. Contrast of P3 and P3(pattern) (section 5): the measured sequences alternate between 228 and 227 colour clocks without a repeat |
| T-l | The CPU writes `BEAMCON0 = $0080` (VARBEAMEN; bit 5 clear, as in NTSC) and `HTOTAL = $E2` (the count the default line has) before the Copper starts; the `MOVE COLOR00, bg` of line 100 is `MOVE HTOTAL, $E8` and that of line 101 is `MOVE HTOTAL, $E2`, at the same list addresses, so every WAIT stays at `(v, $E0)` and every mark is still read. It changes an actual line length (a contrast of PT-5, section 4.5) |
| T-o | The WAIT of line 256 uses `(0,$40)`, not `(0,$E0)`; every mark stays in the list. Contrast of PT-9: the Copper mark of line 256 stands early in its line, and Δ on line 256 differs from the Δ of lines 44 ... 254 |
| T-p | `DIWSTRT = $2D81`: the display window starts one line late (line 45). Contrast of the start clause of PT-2: the groups of every field are N_k - 45 |
| T-q | As T-n, but no list writes COP1LC: the Copper restarts the same list in every field, so the phase restarts at line 0 in every field while the lines still alternate inside every field, the lines above 254 and the last lines included. Stimulus record for the boundary clause of P3 (selector `P3(boundary)`) |
| T-r | As T-n, but line 262 writes `HTOTAL = $E8` (233 colour clocks): the last line of a 263-line field is neither 227 nor 228 colour clocks long. Stimulus record for the range clause of P3 on the last lines (selector `P3(range)`) |
| T-s | As T-n, but the lines 51 ... 60 write the value of the line before them: line 51 repeats the length of line 50 and, with the parity of the lines shifted, line 61 repeats the length of line 60 and the parity is back. Stimulus record that separates P3 from P3(pattern) (selector `P3(interior)`) |
| T-t | As T-n, but a chain of ten states of lists (`listt_chain_slip.i`: ten states of two lists, one list per phase) instead of two, one state of which restarts with the phase of the length it ended on (it chooses the list the other way round). The nine other transitions of the chain are those of T-n and the chain ends in T-n's two lists, so one boundary of the run does not alternate. Stimulus record for the single-phase clause of P3(pattern) (selector `P3(slip)`) |

### 4.5 Preconditions of Program T

As in section 3.6, a precondition is graded only where the run has what it
reads, and prints NOT GRADED otherwise. PT-2, PT-3, PT-5 and PT-7 and the gate
PT-6 and PT-8 read the field lengths N_k and need PT-1; PT-2 also needs every M1 field to
be matched to a restart; PT-3 needs PT-2 (the bitplane marks it reads are
there); PT-5 needs PT-3 and the gate PT-4; PT-9 needs PT-3 and a WAIT (255,$E0) in
the lists as assembled (T-k has none, so PT-9 is not graded there).

| Id | Precondition | Fails in |
|---|---|---|
| FIX-T | The program writes `$BEEF` to FIXMARK before M1 starts, and the harness reads `$BEEF` back after the run: the fixture can deliver a known value. | T-i (`$DEAD`) |
| PT-1 | The completion word was written, and all K M1 records are valid. | T-f |
| PT-2 | In every field, the bitplane fetches are exactly N_k - 44 groups, each of 20 consecutive ascending word addresses, with no fetch left over (the number of fetches is 20 x (N_k - 44)). This shows that the bitplane marks run from line 44 to the last line, and it confirms N_k independently of the CPU. The count of N_k - 44 groups is the one value that the start of the window (line 44) and its stop (the last line N_k - 1) give together, and each of them has a contrast: T-p, whose window starts one line late (N_k - 45 groups), and T-e, whose window ends early (the groups stop at line 243). The comparator counts; it does not say which end of the window moved, and the run prints the group counts per field. The clauses "20 consecutive ascending words" and "no fetch left over" have no contrast: the DMA fetches the 20 words of a line as one group, so a partial or a non-consecutive group needs a fetch cut inside a line or an address step inside the group, which Program T's one plane at BPL1MOD = 0 does not show, and a cut made by the Copper inside a line changes the count as well, so it would not isolate the clause. | T-e (window ends early), T-p (window starts one line late) |
| PT-3 | Δ has one single value on every line 44 ... 254 of every field. | T-d |
| PT-9 | In every field, the Copper mark of every line 255 ... N_k - 1 is there, and Δ has the same value (the one PT-3 found) on each of those lines, where a Copper mark and a bitplane mark lie on the same line. The last-line length of section 2.7 reads the Copper mark of line N_k - 1, a line PT-3 does not read; this shows that the mark stands on its line, at the same offset from the bitplane mark as on lines 44 ... 254, and that the marks the sequence uses above line 254 are present in every field. Graded only where PT-3 holds and the lists hold a WAIT (255,$E0). | T-o (line 256's Copper mark moves to H=$40, Δ on line 256 differs) |
| PT-4 (gate of PT-5 and P3) | In every field, m_c(k, 1) - m_c(k, 0) is 227 or 228 colour clocks: the marks of line 0 and line 1 lie on consecutive lines, so the Copper mark of line 0 is anchored on line 0. p25 leaves open whether the restart lies at the start or at the end of vertical blanking, and a restart at the end of blanking satisfies the WAIT (0, $E0) at once, so PT-4 is printed as a gate state and is not graded. When it is closed, PT-5 and P3 are not graded. | T-c |
| PT-5 | Every directly observed line length of section 2.7 (lines 0 ... N_k - 2 of every field) is 227 or 228 colour clocks, the two values p23 gives. Graded only when PT-3 and PT-4 hold: PT-3 shows that the Copper marks lie at one fixed horizontal offset from the bitplane marks on lines 44 ... 254, so that the differences of the two kinds of mark are lengths of the same lines, and PT-4 anchors the Copper mark of line 0. A program can change a physical line length: p305 lists BEAMCON0 bit 7 as "VARBEAMEN Variable beam counter comparator enable", and p298 lists HTOTAL as "Highest number count, horiz line (VARBEAMEN=1)" (p304: "The value in this register represents the number of 280ns increments on the horizontal line."). The warning on p305 "Other modes, such as VGA (31 KHz programmable mode) disable the hard stops on display sync and blank. Do not write to this register." is addressed to programs that change the scan rate of a monitor; the register is in the table and its write changes the count. T-l writes VARBEAMEN and HTOTAL and changes the length of a line with every mark live. T-g, which moves a measuring mark, is a gating observation of the marks, not a contrast of PT-5. | T-l |
| PT-6 (gate of P3) | The selected standard is NTSC or PAL as the harness was configured, and consecutive field lengths alternate between the two values p24 gives for that standard: 262 and 263 in NTSC, 312 and 313 in PAL (every N_k is one of the two, and no two consecutive N_k are equal). p34 reads the NTSC fields as all 262 lines, so an implementation that follows p34 closes the gate; PT-6 is printed as a gate state with the standard it checked and is not graded, and P3 is reported as not graded, gate closed. | T-a |
| PT-8 (for P2) | Interlace is active: no two consecutive recorded field lengths are equal. p24: "there are also long and short fields (interlace only)"; with interlace off there are no long and short fields, so consecutive fields have one length. Graded; where it fails, P2 is not graded. | T-a (LACE clear: all 13 fields have one length) |
| PT-7 | Anchor check of section 2.6, with LISTT (the lists of T-n and its records: one of two or more lists): one restart per M1 field, in order, each the first-word fetch of a list, preceded, among the words of the list the Copper ran, by a read at or beyond the last word of the instruction in front of the first WAIT of that list that waits for a line the field does not have (line >= N_k), or in front of the terminal wait if there is none: the Copper ran through the list as far as the field's lines let it. | T-h (no restart); T-k (the third clause, page-predicted: the Copper is restarted in the middle of the list); T-j is a self-test of the third clause on this design only, not page-predicted (section 7) |

---

## 5. Propositions

### P1: PAL interlaced fields alternate between 312 and 313 lines, and do not alternate without interlace

1. **Proposition.** With LACE set in PAL, the beam produces two kinds of field
   that alternate. Every field has 312 or 313 lines and consecutive fields
   differ, so any two consecutive fields together have 625 lines. Long and
   short fields occur in interlace only.
   - Claims: `COP2-vertical-beam-position.c12`;
     `COP2-using-the-copper-in-interlaced-mode.c5`, `.c6`, `.c11`;
     `ch2-using-the-copper-in-interlaced-mode.c6`. With the LACE-clear
     control, `ch2-vertical-beam-position.c12` (P1-NOLACE).
     `COP2-vertical-beam-position.c15` and `.c16` ("first" 312, "next" 313)
     are declined (section 1): P1 accepts either phase.
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
   phase is accepted, and "first" and "next" are not graded
   (`COP2-vertical-beam-position.c15`, `.c16`, section 1). Which length
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

   P2 itself would also hold for a non-interlaced NTSC display whose fields
   all have 262 lines (p24: "262 NTSC ... possible vertical positions");
   section 6, item 6 leaves open whether a non-interlaced field has 262 or 263
   lines. p24 limits long and short fields to "(interlace only)". P2 is
   therefore graded behind PT-8, which measures that interlace is active:
   consecutive recorded fields differ in length. Writing LACE in the program
   does not show that the design honoured it; PT-8 does.
5. **Preconditions.** PT-1, PT-2, PT-7, PT-8.
6. **Contrasting case.** T-b (PAL). p24 gives PAL fields of "312, then 313
   lines", so N_k is outside {262, 263} and the check goes red. PT-1, PT-2,
   PT-7 and PT-8 still hold. The contrasts for the preconditions are T-f,
   T-e, T-h and, for PT-8, T-a (LACE clear: the fields all have one length;
   PT-8 goes red and P2 is not graded).
7. **Reference frame.** As in P1.

### P3: the NTSC alternation of long and short lines

1. **Proposition.** When NTSC interlaced fields alternate between 262 and 263
   lines, the Copper sees alternating long and short lines in every field:
   every other line is a long line of 228 colour clocks and the others are
   227 colour clocks long. The proposition is about every line, the last line
   of a field and the first line of the field after it included: p23 says
   "Every other line", and p24 describes the NTSC pattern as the product of
   "this alternation of lines and fields". What is graded is the complete
   sequence of line lengths of every recorded field, in order, and the
   transition from each field's last line to the next field's line 0
   (section 2.7: lines 0 ... N_k - 2 observed directly, the last line N_k - 1
   measured against the next field). The four-field pattern that follows from
   it is P3(pattern).

   Claims: none. Pages 23 and 24.

   The gate (PT-6) rests on `COP2-vertical-beam-position.c14` ("the next field length
   ... is 263"; `.c13` is declined, section 1, because no recorded field is tied to the
   first of the alternation). It exists because p34 denies NTSC fields of 263 lines (see P2). Every page
   is consistent with "if the fields alternate between 262 and 263 lines,
   their lines alternate between 228 and 227 colour clocks": p34 denies the
   premise and says nothing against the consequence. The alternation is
   therefore graded only when PT-6 holds. If the fields are all 262 lines
   (p34's reading), P3 is reported as "not graded, gate closed". That is not a
   failure.
2. **Stimulus.** Program T (T-0).
3. **Observation.**
   - The sequence of field k, S_k(0) ... S_k(N_k - 1): the lengths of the lines
     0 ... N_k - 2 of section 2.7 (between the Copper marks for lines 0 ... 253,
     between the bitplane marks for lines 254 ... N_k - 2) and, for the fields 0
     ... 11, the length y_k of the last line N_k - 1 (section 2.7). The last
     recorded field, field 12, has no field after it and holds lines 0 ...
     N_k - 2 only. 3,400 observed lengths and 12 last lines in the 13 recorded
     fields.
   - Recorded, not graded: the number of lengths that are 227 and 228 colour
     clocks; the type f_k of field k ("short" if N_k = 262, "long" if N_k = 263)
     with y_k; the number of the 12 transitions that keep the length; how many of the
     lengths of lines 254 ... N_k - 2 that the Copper marks give equal the bitplane lengths.
4. **Expected.**
   - p23: "Every other line is a long line (228 color clocks, 0-$E3), with the
     others being 227 color clocks long. In PAL, they are all 227 long. The
     display sees all these lines as 227 1/2 color clocks long, while the
     copper sees alternating long and short lines."
   - p24: "There are alternating long and short lines, there are also long and
     short fields (interlace only)."

   The check: for every recorded field k, every length of the sequence of
   field k is 227 or 228 colour clocks, no two consecutive lines of the
   sequence have the same length (S_k(v) and S_k(v+1) are 227 and 228 in either
   order), and the first length of the sequence of field k+1 differs from the
   last length of the sequence of field k, y_k. Which line of a field is the
   long one is not tied to anything on the page, so either phase is accepted.
5. **Preconditions.** PT-1 to PT-5, PT-7 and PT-9. PT-6 and PT-4 are gates:
   when either is closed, P3 is reported as not graded.
6. **Contrasting case.** T-n (selector `P3(alternating)`) makes the comparator
   green. The CPU writes `BEAMCON0 = $0080` and `HTOTAL = $E2`, and the Copper lists
   write `HTOTAL` in every line 0 ... 262, `$E3` and `$E2` in turn; the count `$E3` is
   the 228 colour clocks (0-$E3) of p23 and `$E2` the 227. The Copper reaches every line
   (p24: "262 lines of video beam travel during which Copper instructions can be
   executed"), and the lists carry the alternation from one field into the next through
   COP1LC, whether the field has 262 or 263 lines, so the alternation does not restart at a
   field boundary and reaches the lines above 254. The comparator and its expected value
   are those of the normal run. T-m (selector `P3(restart)`) is the contrast
   of the parts of the comparator that the Copper-driven list of T-m leaves
   out: its lines 0 ... 254 alternate between 228 and 227, the write of line 254 stays in force above
   it, and the list restarts at the same phase in every field; the sequences
   of T-m hold repeats above line 254 and keep the length from each field's
   last line to line 0 of the next, so the comparator is red there. Four stimulus records
   change one thing of T-n each and show the clause of the comparators that reads it
   (each with PT-1 to PT-9, P2 and the gates holding):
   - T-q (selector `P3(boundary)`): no list writes COP1LC, so the phase restarts in every field
     and the lines alternate inside every field and above line 254; the clause "line 0 of each
     field differs in length from the last line of the field before it" is the only clause the
     sequences fail, and P3(pattern) does not fit. It isolates the boundary clause.
   - T-r (selector `P3(range)`): the last line of a 263-line field is 233 colour clocks;
     every length that is neither 227 nor 228 is a last line (the clause "every length is 227
     or 228" on the last lines), PT-5, which reads lines 0 ... N_k - 2, holds.
   - T-s (selector `P3(interior)`): two lines in every field repeat the length of the line
     before; P3 is red through the clause "no two consecutive lines have the same length",
     and P3(pattern), which reads only the last line of each field and the field type, is
     green. It separates the two comparators.
   - T-t (selector `P3(slip)`): one boundary of the run does not alternate (the Copper restarts
     with the phase of the length it ended on, once); P3 is red through the boundary clause at that one
     boundary, and P3(pattern) does not fit at one phase.
   T-l, whose two writes (`HTOTAL = $E8` at line 100 and `$E2` at line 101)
   land after the count has passed `$E2`, changes one line per field to a
   length that is neither 227 nor 228 (PT-5, section 4.5). T-b (PAL) runs the
   same comparator on the standard for which p23 says "In PAL, they are all
   227 long": every length is 227 and the check goes red, as it does on the
   NTSC run. PT-1 to PT-7 and PT-9 still hold on it, because PAL fields
   alternate between 312 and 313 (P1) and 227 is a valid length; that is a
   live control of the preconditions while the comparator is red. P3(pattern) is not graded on that
   run and says so. The contrasts for the preconditions are T-f (PT-1), T-e and T-p (PT-2), T-d (PT-3), T-c
   (PT-4), T-a (PT-6; p24: long and short fields occur "interlace only"), T-h and T-k
   (PT-7), T-l (PT-5) and T-o (PT-9).
7. **Reference frame.**
   - Line marks at fixed horizontal positions (section 2.7).
   - The WAIT position anchors the Copper marks (p22) and DDFSTRT anchors the
     bitplane marks (p60). Their offset Δ is checked by PT-3 and PT-9.
   - Line 0 of the next field is anchored at the start of vertical blanking
     (p219), checked by PT-4.

### P3(pattern): the NTSC four-field pattern

1. **Proposition.** p24: "In NTSC, the fields are 262, then 263 lines ...
   This alternation of lines and fields produces the standard NTSC 4 field
   repeating pattern: short field ending on short line; long field ending on
   long line; short field ending on long line; long field ending on short
   line; and back to the beginning." A short field has 262 lines, a long
   field 263; a short line is 227 colour clocks and a long line 228 (p23).
   The pattern is defined by the length of the last line of each field, which
   section 2.7 measures (y_k).

   Claims: `ch2-vertical-beam-position.c15`, `.c16`, `.c17`, `.c18`, `.c19`;
   `COP2-vertical-beam-position.c17`, `.c18`, `.c19`, `.c20`, `.c21`, `.c22`
   (the pattern, its four fields and its repetition). Page 24. The gate PT-6 is
   that of P3.
2. **Stimulus.** Program T (T-0).
3. **Observation.** The type f_k and the measured last-line length y_k of the
   fields 0 ... 11 (the fields that have a field after them).
4. **Expected.** The four slots in order: (short, 227), (long, 228), (short,
   228), (long, 227). The check: there is one phase q in 0 ... 3 such that
   (f_k, y_k) is slot (k + q) mod 4 for every k = 0 ... 11. Which of the four
   slots field 0 is, is not tied to anything on the page, so any phase is
   accepted; the three repetitions of the pattern in twelve fields grade "and
   back to the beginning".
5. **Preconditions.** Those of P3 (PT-1 to PT-5, PT-7, PT-9 and the gates PT-4
   and PT-6). The pattern is the NTSC pattern: the line is not graded on a PAL
   run (selector `P3`), where it prints "not graded, the pattern is the standard NTSC pattern
   (p24) and this run is PAL".
6. **Contrasting case.** T-n (selector `P3(pattern)`): the alternation of lines
   is continuous over the field boundaries, so the field types and the last
   lines of the twelve fields follow the pattern at one phase, and the
   comparator is green. T-t (selector `P3(slip)`) breaks the phase once, so the pattern fits
   at no single phase (the single-phase clause); T-s (selector `P3(interior)`) leaves the last lines
   alone and keeps this comparator green while P3 is red; T-q (selector `P3(boundary)`) restarts the phase in
   every field, which the pattern does not fit either. T-m (selector `P3(restart)`) restarts the phase in
   every field and leaves the lines above 254 at one length: every field ends on
   a long line (228 colour clocks), the fit is `no`, and the comparator is red.
7. **Reference frame.** That of P3 (section 2.7: the last-line length is
   the time from the Copper mark of line N_k - 1 to the Copper mark of line 0 of the next
   field).

### P4: interlaced bitplane retrieval with a separate Copper list per field

1. **Proposition.** Each field has its own Copper list, and the lists are put
   in COP1LC in turn as p35 describes. One list points bitplane 1 at START
   and the other at START + 40, one line further on. The field that runs the
   first list retrieves memory lines 1, 3, 5, ... in Figure 2-1 numbering.
   The field directly after it retrieves lines 2, 4, 6, ... Each such pair of
   fields therefore retrieves every line of the picture exactly once, from a
   memory that holds the lines in screen order.
   - Claims: (a) `ch2-using-the-copper-in-interlaced-mode.c16`,
     `COP2-using-the-copper-in-interlaced-mode.c15`; (b)
     `ch2-using-the-copper-in-interlaced-mode.c17`, `.c18`,
     `COP2-using-the-copper-in-interlaced-mode.c16`, `.c17`; (e)
     `ch2-using-the-copper-in-interlaced-mode.c20.s2`. (c) and (d) carry no
     id. Declined, and printed as such by the run (section 1):
     `ch2-using-the-copper-in-interlaced-mode.c8`, `.c9`, `.c10`, `.c11`, `.c12`,
     `.c13`, `.c14`, `.c15`, `.c19`;
     `COP2-using-the-copper-in-interlaced-mode.c8`, `.c9`, `.c10`, `.c12`,
     `.c13`, `.c14`, `.c18`, `.c19`.
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
     1 to 6 in order. It grades which memory lines each field's bitplane DMA
     fetches, not which lines the field displays: the ids that say so
     (`ch2-using-the-copper-in-interlaced-mode.c10`, `.c13`, `.c14`,
     `COP2-using-the-copper-in-interlaced-mode.c12`, `.c13`) are declined
     (section 1).
   - (d) Every two consecutive recorded fields (an A-field and a B-field,
     in either order) together fetch memory lines 0 ... 511, each exactly
     once. Taken in the order A-group 0, B-group 0, A-group 1, B-group 1,
     ... A-group 255, B-group 255, the group addresses rise by 40 bytes at
     every step: from A-group i to B-group i and from B-group i to A-group
     i + 1. This grades the order in which the bitplane DMA retrieves the
     lines of the memory the program built, over two fields. How that memory
     is laid out (`ch2-using-the-copper-in-interlaced-mode.c15`,
     `COP2-using-the-copper-in-interlaced-mode.c19`) is the program's data
     and is declined, and where the lines land on the screen is not observed:
     the sentence's statement that memory matches the screen
     (`ch2-using-the-copper-in-interlaced-mode.c19`,
     `COP2-using-the-copper-in-interlaced-mode.c18`) is declined as well
     (section 1).
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
5. **Preconditions.** PI-1 to PI-6, PI-7(bpl) and the gate PI-12(bpl); for (a) to (d)
   also the gate (e). The proposition attributes the retrieval to the BPL1PT
   MOVEs of each field's list, and PI-7(bpl) proves that those MOVEs are
   fetched before the field's first bitplane fetch; with the MOVEs after the
   first fetch the retrieval is not theirs. The gate PI-12(bpl) requires
   that they are fetched in vertical blanking.
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
     MOVEs after the `WAIT $2001,$FF00`, line 32), the gate PI-12(bpl) prints closed
     and P4 is reported as not graded.
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
     `COP2-using-the-copper-in-interlaced-mode.c15` (P5, the rewrite half).
     P5-NOREWRITE carries no id: `ch2-putting-together-a-copper-instruction-list.c3`
     speaks of the bitplane pointers, in the plural, and of properly
     retrieved data, and the check observes bitplane pointer 1 and the
     address of its first 20-word fetch group only (declined, section 1).
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

   p27's "must be rewritten" holds both halves: with the rewrite the
   retrieval starts at the rewritten address, and without it the next field's
   retrieval does not start there. P5-NOREWRITE grades both in one assertion:
   the P5 check on Program I, and on a run of I-c that no recorded field
   starts its first 20-word fetch at the address its list would have written
   (PIC for an A-field, PIC + 40 for a B-field). Recorded field 0 is graded
   like the others: the capture opens at the second wrap after the setup has
   written its last register and DMA is switched on before that (section
   2.5), so field 0 is already a field that follows others, to which "when the
   screen display starts again" applies.
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
   PI-7(bpl) and I-t for the gate PI-12(bpl).
7. **Reference frame.** The restart interval (M2).

### P6: sprite pointers rewritten during vertical blanking make the sprite data be fetched in every field

1. **Proposition.** When sprite pointer 0 is rewritten during vertical
   blanking, the sprite's data is fetched in every field. When it is not,
   later fields do not fetch it.
   - Claims: none. `ch2-putting-together-a-copper-instruction-list.c4`
     speaks of the sprite pointers, in the plural, and of properly retrieved
     data; the checks observe sprite pointer 0 and the occurrence of reads at
     four data addresses of its structure only (declined, section 1).
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
   recorded field (field 0 included, as for P5-NOREWRITE). Its contrasts are I-e in the place of the rewrite run
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

   Only the agreed part is graded (P2). The alternation of long and short
   lines (P3) and the four-field pattern of p24 (P3(pattern)) are graded behind
   a gate, the lengths of the last line of every field included (section 2.7).
2. **"Twice".** p34 calls the interlaced display "twice the normal number of
   vertical lines" and in the same paragraph gives PAL as 312 and 625.
3. **When the Copper restarts.** p25 says, in one note, "At the start of each
   vertical blanking interval, COP1LC is automatically used" and "when the
   end of vertical blanking occurs, the Copper is automatically forced to
   restart". p30 and p32 say start; p33 says COP1LC is written "at the end of
   the vertical blanking period" by the operating system.
   - M2 works under either reading.
   - The Copper mark of line 0, which PT-5 and P3 read, needs the restart on
     line 0 before
     position $E0. PT-4 gates it instead of assuming it: when it does not
     hold, PT-5 and P3 are not graded.
   - PI-12 needs the restart at the start of vertical blanking; it gates P4, P5
     and P6 instead of assuming it.
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
   20)". Nothing here depends on the exact value, except the gate PI-12, which uses the 25-line
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
   already above 0 through 5: a WAIT (0,$E0) read while the beam is still in line
   255 ends at once. Program T keeps the Copper in line 255 until its end with
   three MOVEs behind the mark of line 255 (four memory cycle times each, p20),
   so that it reads the WAIT (0,$E0) in line 256, where the vertical comparison
   sees 0 (p23: "line 256 will appear as a zero in the comparison"), and every WAIT up to
   (6,$E0) in its own line. p24 says that these are lines "during which Copper
   instructions can be executed": the Copper reaches every line 0 ... 262, and Program T
   marks the lines at or above 255 with Copper marks like the others (section 2.7).
   PT-9 shows that the marks stand on their lines.
10. **Figure 2-1's "16.6ms".** It corresponds to 261.2 NTSC lines, which is
    not a whole-line field (section 1).
11. **Table 3-8 (p53, checked on the scan).** It gives 8,192 bytes for a
    320 x 256 bitplane and 16,384 for 320 x 512. At 40 bytes per line (p53)
    these are 10,240 and 20,480. The table is not used; PIC is 512 x 40 =
    20,480 bytes.

12. **Measuring the last line of a field.** The last line has a Copper mark,
    like every line from 0 to 262 (the WAITs behind line 254, section 2.7), and so does line 0 of the
    next field: the length of the last line is the time between those two marks, the same
    kind of difference of two marks at the same horizontal position of consecutive lines as every
    other length. Two other bases were weighed and not used:
    - The interval between two `vs` events. p23: "The display sees all these
      lines as 227 1/2 color clocks long, while the copper sees alternating
      long and short lines", and p49: "the scanning circuitry vertically offsets
      the start of every other field by half a scan line". A `vs` interval is
      the display's field. It is the sum of the Copper's lines plus the
      difference between where the vertical sync stands in the line in one
      field and in the next, and no page gives that position in terms of the
      horizontal counter the Copper compares with. A half line is larger than
      the one colour clock that separates a long last line from a short one,
      so an interval between `vs` events is not by itself a field length in
      the Copper's lines, and it cannot show the last line's length.
    - A residual: the field's length from the marks of line 0 of two
      consecutive fields (or from two Copper restarts), less the directly observed lines
      0 ... N_k - 2. It is not a second source: with Δ the same on the last line as on the
      lines 44 ... 254 (PT-9) it is the same number as y_k, because
      m_c(k+1, 0) - m_b(k, N_k - 1) - Δ = m_c(k+1, 0) - m_c(k, N_k - 1), and
      the interval between the Copper marks of line 0 of two fields is the sum of all N_k lines
      of the field; the Copper restart stands at one fixed offset from the mark of line 0 (1,776
      clk_sys on both cores in every recorded field), so the interval between two restarts differs
      from that sum by nothing. It is an identity, not a measurement, and the run does not print it.
    Nothing is derived from the quantity under test and graded; the graded
    value y_k is a difference of two Copper marks.

---

## 7. Build

Files: `prog_i.s` (Program I) with variants `prog_i_a.s` to `prog_i_x.s`
(I-a to I-x, section 3.5); `prog_t.s` (Program T) with variants `prog_t_a.s`,
`prog_t_c.s` to `prog_t_t.s` (T-a, T-c to T-t, section 4.4; T-b is the same
binary as T-0 with PAL selected in the harness, not a separate source);
`m1.i` (Measurement M1, shared); `ch2.i` (register offsets Appendix B gives
that `nanomig.i` does not, and the CIA-A address used only to release the
reset-time overlay so Program I can install its own interrupt vector at
$6C, independently of Appendix B); `listt_body.i` (LISTT's 255 WAIT/MOVE
pairs, generated once, not by hand); `listt_high.i` (the marks of the lines
255 ... 312); `listt_alt_body.i` (T-m's list, generated); `listt_chain.i` (one list of
T-n and of its stimulus records, written with assembler macros) and
`listt_chain_slip.i` (the twenty lists of T-t, ten states of two lists each, generated);
`longline.patch` (section 7, "The private long-line beam counter");
`test_ch2_interlace_fields.cpp`.

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
is 12 (fx68k) and 29 (tg68k) under I-j, and 38 (fx68k) and 82 (tg68k)
under T-f -- several lines' worth either way, on both cores. After the completion word
the run goes on until the next fetch of the first word of a list (section
2.5), so that the last recorded field is not cut off. Measured on both cores,
PI-1 is the only red under I-j: PI-2 to PI-7, PI-12(bpl) and PI-12(spr) need
PI-1 (section 3.6) and print NOT GRADED, and PI-8 holds.

**Measured (recorded, not graded).**
- P1 (Program I, both cores): the 8 recorded PAL field lengths are
  313, 312, 313, 312, 313, 312, 313, 312.
- P3's lengths (section 2.7, both cores): of the 3,302 line lengths measured
  between the Copper marks (lines 0 to 253 of the 13 recorded fields), all
  3,302 are 227 colour clocks and none is 228, so P3 measures red on both
  cores. Of the 3,400 directly observed lengths (lines 0 to N_k - 2), all
  3,400 are 227, and PT-5 (every length is 227 or 228) stays green, since 227
  is one of the two page values.
- The measured last-line length y_k (section 2.7, both cores): 227 on every
  one of the fields 0 to 11, never 228, so every one of the 12 transitions from
  a last line to line 0 of the next field keeps the length; the four-field
  pattern of p24 fits it at no phase, and P3 and P3(pattern) are red. Of the 98 lengths of
  the lines 254 to N_k - 2 that both kinds of mark give, 98 are equal (section 2.7). The Copper
  restart precedes the mark of line 0 by 1,776 clk_sys in all 13 recorded fields.
- T-a (LACE clear, both cores): all 13 recorded NTSC fields measure 263 lines
  (the PT-6 gate line of the PT-6 run; PT-8 red in its own run, P2 NOT
  GRADED).
- PI-5's contrasts (both cores): under I-w the first twelve writes to HCNT
  carry 0, 2, 4, ... 22; under I-g the run holds the one write of the program
  and none by a handler; under I-x the first write carries 7.
- The normal run prints, as MEASURED lines before the P3 line, the number of
  directly observed line lengths that are 227 colour clocks, 228 colour clocks
  and neither (over lines 0 to N_k - 2 and over the Copper-mark lines), the
  type f_k and the measured last-line length y_k of fields 0 to 11 with the
  number of last lines of each length and of transitions that keep the length,
  the count of lengths that the Copper marks and the bitplane marks give equally,
  the offset of the restart from the mark of line 0, and the fit of the four-field pattern.

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
preceded, in Program I, among the list words up to the terminal wait, by the Copper's fetch
of the terminal wait, and the matched restarts are consecutive. In Program I, in every
field, 2 colour clocks before the restart, the Copper also reads the word after
the terminal wait ($080854 after LISTA's, $080954 after LISTB's); the anchor does not
count it. In Program T the Copper has marks up to the last line of the field and is busy up
to the restart: the list word it reads last before the restart is the second word of the mark MOVE
of the field's last line, $081042 in the 6 fields of 262 lines (the mark of line 261) and $08104A in
the 7 fields of 263 lines (the mark of line 262), on both cores; the WAIT for the next line and the
terminal wait are not read in any field, and the restart comes right behind the mark. That is the read
the clause of section 2.6 asks for in Program T (13 of 13 fields). Under I-s every field still
holds exactly one restart and the matched restarts are still consecutive, but
no restart is preceded by a fetch of the terminal wait (0 of 8), and under T-j no restart is
preceded by the read the clause asks for (0 of 13). No
page says that the Copper stops after a MOVE it cannot write; read literally,
p25 ("Instruction fetch continues sequentially until the Copper is interrupted
by another jump address strobe") and p30 ("No stop instruction is provided for
the Copper") would let it go on to the terminal wait and leave PI-2 and PT-7
green. I-s and T-j are therefore not page-predicted contrasts and not
mutations of PI-2 and PT-7: they are diagnostics of the anchor's third clause
(`SELFTEST-PI-2-ANCHOR`, `SELFTEST-PT-7-ANCHOR`), printed as MEASURED lines
that state whether the comparator holds; they are not assertions. Measured on
both cores, under I-s the Copper fetches the MOVE's first word, fetches nothing more of
the list, and reads the MOVE's second word 2 colour clocks before the restart;
on this design the comparator therefore does not hold, through the third
clause alone, with everything else holding. Under T-j (the MOVE stands behind the mark of line
254) the Copper reads the two words of the MOVE and nothing after them: the last list word read
before the restart is the MOVE's second word, $081002, in 13 of 13 fields, so PT-7's comparator
does not hold through the third clause, with PT-1, PT-2, PT-3, PT-5 and PT-8 holding (PT-9 does
not: the marks of the lines 255 and above are never fetched). PI-2 and PT-7 keep their page-based contrasts, I-f
and T-h, and the third clause has its own, page-predicted contrast, I-v and
T-k (section 2.6). Measured on both cores, under I-v 8 of 8 and under T-k 13
of 13 M1 fields hold exactly one restart, the matched restarts are
consecutive, and no restart is preceded by the read the clause asks for (a read of a terminal
wait in Program I): the Copper is in the middle of the list when the restart comes. Only
the third clause goes red; every other precondition holds (PT-9 is not graded under T-k, which has no Copper marks
of the lines 255 and above).

**PI-12 (I-t, I-u).** Measured on both cores: the Copper fetches the two BPL1PT
MOVEs 0 and 4 and the two SPR0PT MOVEs 8 and 12 colour clocks after the
restart, and the write that opens the M1 field follows the restart by 165 to
259 (fx68k) and 81 to 185 (TG68K) colour clocks. The later MOVE fetch therefore
lies 161 to 255 (BPL1PT) and 153 to 247 (SPR0PT) colour clocks before that write
on fx68k, and 77 to 181 and 69 to 173 on TG68K, against the limit of 5,221
colour clocks (23 lines). Under I-t the later BPL1PT fetch lies 7,053 to 7,083
(fx68k) and 7,064 to 7,158 (TG68K) colour clocks after the write, under I-u the
later SPR0PT fetch 5,464 to 5,494 (fx68k) and 5,475 to 5,591 (TG68K); PI-7(bpl)
and PI-7(spr) hold there, the gate PI-12 prints closed, and P4, P5 (I-t) or P6 (I-u) and their NOREWRITE lines print NOT GRADED. The sprite's first fetch comes 5,694 colour clocks
after the restart (line 25, colour clock 19 at 227 colour clocks per line) and
the first bitplane fetch 10,049 (line 44), the same on both cores.

**T-g.** Line 10's WAIT uses `(10, $40)`. Measured on both cores, field 0's
line 9 then measures 536 clk_sys (67 colour clocks); 26 of the 3,400 directly
observed line lengths of the 13 fields fall outside 227 and 228 colour clocks;
PT-1, PT-3, PT-9 and every Copper mark the line lengths need stay live, P2 is
graded and P3 and P3(pattern) are not graded. The run prints PT-5's comparator
as a gating observation (`PT-5(mark)`), not as an assertion: it moves a measuring
mark and no physical line. Line 10 lies in front of the display window (line 44),
so it has no bitplane mark, PT-3 (lines 44 to 254) and PT-9 (lines 255 and above) still hold,
and the selector `PT-9` runs T-o as the contrast of PT-9.

**T-l.** `BEAMCON0 = $0080`, `HTOTAL = $E2`, then the Copper's `HTOTAL = $E8` in line 100's
MOVE slot and `HTOTAL = $E2` in line 101's. Measured on both cores (the fx68k build and the default
TG68K build, the board's kernel): in every one of the 13 fields line 101 measures 2,048 clk_sys (256
colour clocks); 13 of the 3,400 directly observed line lengths fall outside 227 and 228 colour clocks
and all 13 are line 101 (the E8 write comes after line 100's count $E2 has passed, so line 101 would
end at $E8, and the $E2 write in line 101 again comes after $E2 has passed, so the count runs on to
256). PT-1, PT-2, PT-3, PT-7 and PT-8 hold, the gate PT-4 is open (line 0 to 1 is 1,816 clk_sys),
the gate PT-6 is open, P2 is graded and passes, PT-5's comparator prints FAIL and P3 and P3(pattern) are not graded.

**T-m.** `BEAMCON0 = $0080`, `HTOTAL = $E2`, then in every line v from 0 to 254 the
Copper's `WAIT (v,$40)` and `MOVE HTOTAL` (`$E3` for even v, `$E2` for odd v) in
front of the mark; the marks of the lines 255 ... 262 follow as in T-0 and carry no
HTOTAL write, so the list writes HTOTAL on the lines 0 ... 254 only, by construction.
Measured on the fx68k build and the default TG68K build: of the
3,302 Copper-mark line lengths of the 13 fields 1,651 are 228 colour clocks and
1,651 are 227 (lines 0 to 5 of field 0: 1,824 1,816 1,824 1,816 1,824 1,816
clk_sys), and PT-1, PT-2, PT-3, PT-5, PT-7, PT-8 and PT-9 hold, the gates PT-4 and
PT-6 are open (N_0..N_12 = 263, 262, ...), and P2 is graded and passes. The
write of line 254 stays in force above it: of the 3,412 lengths of the complete
sequences 1,761 are 228 colour clocks, 1,651 are 227, none is neither, 97 equal the
length of the line before in their field, all of them above line 254, and every
one of the 12 last lines measures 228 colour clocks, so all 12 transitions from a
last line to line 0 of the next field keep the length. P3 and P3(pattern) print
FAIL (the pattern fits at no phase). The selector `P3(restart)` runs it; the
normal run does not.

**T-n.** `BEAMCON0 = $0080` and `HTOTAL = $E2` from the CPU, then two Copper lists (at LISTT
and at `$0A0000`, `listt_chain.i`) that alone write `HTOTAL` in every line 0 to 262, at
`(v,$40)`: `$E3` and `$E2` in turn. The CPU writes nothing after its setup, and neither list has an
HTOTAL write at the lines 100 and 101. Each list marks every line as the normal list does (the
lines 255 to 262 as in `listt_high.i`) and chooses the list of the next field through COP1LC, which
the Copper writes itself: at its start it sets COP1LC to its own address, and in line 262 (behind
`WAIT (6,$40)`, which a 262-line field never reaches) to the other list. A 262-line field ends on the
other length than it began with, so the next field begins with the same phase; a 263-line field
ends on the length it began with, so the next field begins with the other phase. Measured on the fx68k build and the
default TG68K build: of the 3,412 lengths of the complete sequences 1,706 are
228 colour clocks and 1,706 are 227, none is neither, none equals the line before
in its field, and none of the 12 transitions from a last line to line 0 of the
next field keeps the length (lines 0 to 5 of field 0: 1,816 1,824 1,816 1,824 1,816
1,824 clk_sys; its last six lines 1,824 1,816 1,824 1,816 1,824 1,816); the lists the
Copper restarted with in fields 0 to 12 are B A A B B A A B B A A B B (the second list
after each 263-line field, the same list after each 262-line field); PT-1 to
PT-5, PT-7, PT-8 and PT-9 hold, no M1 record sets its gap flag, the gates PT-4
and PT-6 are open, and P3 and P3(pattern) print PASS (the pattern fits at phase 3). The
last lines are 227, 227, 228, 228, 227, 227, 228, 228, ... for the fields 0 to 11. The Copper
alone writes the lines, so the verdict does not depend on the CPU's sampling loop: an
M1 loop that is slower than the unmodified one leaves the control green wherever PT-1
holds. Measured on both cores with NOPs added to every pass of the sampling loop and, in
other runs, to the code that closes a field (a probe build of `m1.i`, not part of the
package; every other line of the run unchanged): on fx68k P3, P3(pattern) and P2 print PASS
with 0, 1, 2, 4, 8, 12 and 16 NOPs in every pass and with 1, 2, 4, 16 and 40 in the close
path; with 20, 30, 40, 60 and 80 in every pass and with 60 and 80 in the close path an M1 record
is not valid, PT-1 does not hold and the lines print NOT GRADED. On TG68K they print PASS
with 1 to 60 NOPs in every pass and in the close path, and with 80 PT-1 does not hold. In none of
the runs does P3 or P3(pattern) print FAIL: what limits the control is the fixture PT-1 (the
M1 loop sampling every line), which limits every run of Program T alike. The selectors `P3(alternating)` and `P3(pattern)` run it; the normal run does not.

**T-o.** Line 256's WAIT uses `(0,$40)`, so its mark lies early in line 256. Measured on both cores: PT-9's
comparator prints FAIL, PT-1, PT-2, PT-3, PT-5, PT-7 and PT-8 hold, the gates are open, P2 is
graded and passes, and P3 and P3(pattern) are not graded. The selector `PT-9` runs it.

**T-p.** `DIWSTRT = $2D81`. Measured on both cores: every field has one group fewer than the
N_k - 44 PT-2 expects (218 for N = 263, 217 for N = 262), no read is left over after the last group, PT-2
prints FAIL, PT-3, PT-5, PT-9 and P2 and P3 and P3(pattern) are not graded, PT-1, PT-7 and PT-8 hold. The same
run for T-e (`DIWSTOP = $F4C1`): 200 groups in every field, the last of them on line 243. The selectors
`PT-2(start)` and `PT-2` run them.

**T-q to T-t.** T-q (selector `P3(boundary)`) writes no COP1LC in any list: of the 3,412 lengths of the
complete sequences 1,709 are 228 colour clocks and 1,703 are 227, none is neither, none equals the
line before in its field, and 6 of the 12 transitions from a last line to line 0 keep the length (the
last lines are 228 for the fields of 263 lines and 227 for those of 262 lines); P3 and P3(pattern) print FAIL. T-r
(selector `P3(range)`) writes `HTOTAL = $E8` in line 262: the lines 262 of the 263-line fields (6 of the 12
measured last lines) are 233 colour clocks long, none of the other lengths is outside 227 and 228, P3
and P3(pattern) print FAIL. T-s (selector `P3(interior)`): 26 of the 3,412 lengths equal the line before
(lines 51 and 61 of every field), none of the 12 transitions keeps the length, the last lines are those of T-n; P3
prints FAIL and P3(pattern) PASS. T-t (selector `P3(slip)`): 1 of the 12 transitions keeps the length, none
of the 3,412 lengths equals the line before in its field, the Copper restarted with the lists 5, 6, 8, 11, 13,
14, 16, 18, 18, 19, 19, 18, 18 of the twenty in the fields 0 to 12 (the chain stands in its states 2 to 8 in the
fields 0 to 6 and in its last state from field 7 on), P3 and P3(pattern) print FAIL. All four runs: PT-1 to PT-5,
PT-7, PT-8 and PT-9 hold, the gates open, P2 passes; both cores print the same.

**The private long-line beam counter (`longline.patch`).** `make longline-test-fx68k` and
`make longline-test-tg68k` build the package against a private copy of the NanoMig chip-set RTL: a copy of
the pinned submodule's `minimig-aga` directory to which `longline.patch` is applied. The patch changes one line
of `agnus_beamcounter.v`: without VARBEAMEN, an NTSC line that `long_line` marks ends at `$E3` (228 colour
clocks, p23) instead of at `$E2` (the signal `long_line` toggles at every line end in the pinned RTL and is
not used). The submodule is not modified; each core builds in its own object directory
(`obj_dir_longline_<core>/`) and writes its own output file (`out_longline_<core>.txt`), so the unmodified
build and its output stay those `make test` uses. The patch is a test construction that gives NTSC lines of
228 and 227 colour clocks, so that P3 and P3(pattern) have a design to pass on; it is not a proposed fix of the RTL.
