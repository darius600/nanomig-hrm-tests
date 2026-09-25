# ch6-blitter-geometry

Integration package for the claims in `ch6-blitter-geometry-spec-stage.jsonl`
(83 claims, chapter 6 of the Amiga Hardware Reference Manual, 3rd edition,
pages 169-183 and 200): the blitter's addressing and masking geometry --
row masks, modulo/blit-size arithmetic, shift decomposition, ascending and
descending pointer order, the arbitrary-region copy recipe, the minterm
function generator's bit layout, and the four figures of the chapter, read
as blit geometry rather than as pictures.

This is the package's second revision, after an independent validation
found fit and coverage defects (`validation-ch6-blitter-geometry.md`) and
several declines that did not hold against the page. Every register-bit
layout, DMACON encoding and pointer/modulo rule this file states is
verified against `nanomig.i` (register offsets) and the manual (page
citations throughout); nothing here is stated only because a run on the
RTL showed it, except where marked `MEASURED` in a program's own comments.

Every program drives `nanomig.v` only through the blitter's custom
registers and observes only the RAM port (`ram_log()`), `peek_logical()`/
`peek_word()`, per `integration/harness/nanomig_harness.h`. Each program
enables blitter DMA (DMACON $8240: SET, DMAEN bit 9, BLTEN bit 6), writes
the channel pointers, modulos, masks and BLTCON0/1 for its case, and
writes BLTSIZE last to start the blit, then polls DMACONR bit 14 (BBUSY)
twice per test -- p.186: "The first 'test' of the blitter done bit may not
return the correct result, but the second will" -- before DONE.

Mutations are separate runs, not part of the normal `make test` summary:
each assertion's program is chosen by the `NMH_MUTATE` environment
variable naming that assertion's own label; with it unset, every program
is the normal one.

Many claim ids are near-duplicates of each other -- the same fact cut from
the same or an adjacent quote by a different extraction run (`6-dma-
channels.*` vs `ch6-dma.*`, `6-memory-layout.*` vs `ch6-memlayout.*`,
`6-shifts-and-masks.*` vs `ch6-shifts.*`, `6-copying-arbitrary-regions.*`
vs `ch6-copy.*`). Where that is so, both ids sit on the same PASS/FAIL
line: the evidence for one is the evidence for the other.

## Assertions

### A0 -- fixture: registers set up, BLTSIZE last, known result delivered
Page 169, 200. Claims: `ch6-intro.c20`, `ch6-keypoints.c1`.

Proposition: a blit is performed by initializing the blitter's registers
first and starting it by writing BLTSIZE last; only after that write does
the destination show the blit's result.

Program: the CPU writes a short, known source pattern into a scratch chip-
RAM word range, sets BLTCON0/1 for a plain word copy (D = A, no shift, no
masking, ascending), sets the A and D pointers, size and moduli, and writes
BLTSIZE last.

Observed: `peek_logical()` on the destination range.

Expected: the destination equals the known source pattern exactly.

Mutation `NMH_MUTATE=A0-fixture`: withhold the BLTSIZE write -- the
destination stays at its pre-blit seed instead of the source pattern.

### A1 -- first/last word of each raster row can be masked
Page 169. Claims: `6-intro.c13`, `6-intro.c14`, `ch6-intro.c12`,
`ch6-intro.c13`.

Proposition: block-move mode can mask the first word of each raster row
(BLTAFWM) and the last word of each raster row (BLTALWM), on every row of
a multi-row blit, not only the first/last row of the whole blit.

Program: a copy blit, source all-ones, destination pre-seeded to all-zero,
width 3 words/row, height 2 rows, BLTAFWM masking only the low byte,
BLTALWM masking only the high byte, the middle word unmasked.

Observed: `peek_logical()` on all 6 destination words (2 rows x 3 words).

Expected: on both rows, word 0 = the FWM-masked value, word 1 = all-ones
(unmasked), word 2 = the LWM-masked value.

Mutation `NMH_MUTATE=A1-row-masks`: a different BLTAFWM value -- word 0 of
row 0 no longer matches.

### A2 -- zero-modulo raster addressing is a "plus one" continuation
Page 171. Claims: `ch6-dma.c30`, `ch6-dma.c31`.

Proposition: images are normally stored so each word on a line sits at the
address one greater than its left neighbor, and each line is a "plus one"
continuation of the line before it -- with modulo 0, the last word of one
row and the first word of the next are consecutive addresses.

Program: a copy blit, several rows wide and tall, all modulo registers 0.

Observed: `ram_log()`, the destination's write accesses, coalesced.

Expected: the addresses form one unbroken run with no gap at any row
boundary.

Mutation `NMH_MUTATE=A2-plus-one`: a 2-word destination modulo -- a gap
appears at every row boundary.

### A3 -- embedded-rectangle modulo arithmetic, and figure 6-2 (p.173)
Page 172, 173. Claims: `6-dma-channels.c65`, `6-dma-channels.c73`,
`6-dma-channels.c74`, `6-dma-channels.c75`, `6-dma-channels.c76`,
`ch6-dma.f5`, `ch6-dma.f7`, `ch6-dma.f8`.

Proposition: for a blit into a rectangle embedded in a wider bitmap, twice
the blit width in words plus the modulo (in bytes) equals the containing
row's full byte width (`6-dma-channels.c65`). Figure 6-2 draws this at a
specific position and with specific numbers, testable as the same blit
geometry rather than as a picture: a 40-byte row (`c73`, `f5`), the
rectangle at row 13, byte column 12 (`c74`), a 12-byte left margin (`c75`,
`f7`) and an 18-byte right margin (`c76`, `f8`) -- 12 + 18 = 30-byte
modulo, and 2*5 words + 30 bytes = 40 bytes, the same arithmetic as
`c65`.

Program: a scratch bitmap of 40 bytes/row x 18 rows (rows 0-17), its
margins and untouched rows pre-seeded with a sentinel; a copy blit into
the embedded 5-word rectangle at row 13, byte column 12, with BLTDMOD=30.

Observed: `peek_logical()` across the whole bitmap.

Expected: the rectangle holds the source data on rows 13-17; every other
word (rows 0-12 entirely, and the 12-byte left / 18-byte right margins of
rows 13-17) keeps the sentinel, unchanged.

Mutation `NMH_MUTATE=A3-embedded-modulo`: a 20-byte modulo instead of 30
-- rows land at the wrong offset and the margin check fails.

### A4 -- a cookie-cut mask is one bitplane regardless of background depth
Page 176, 183. Claims: `6-function-generator.c40`, `ch6-minterms.c9`,
`ch6-copy.c6`.

Proposition: a cookie-cut mask needs only a single bitplane no matter how
many bitplanes the background has, because the same mask plane is reused
per background plane; the cookie-cut function is $CA.

Program: one mask pattern (channel A), one "car" source pattern (channel
B), and two different background patterns (channel C, in turn, C=D so the
background is also the destination), each blitted with BLTCON0's function
set to $CA. The A/B pointers are reloaded before each of the two runs:
ascending mode leaves every active pointer advanced past the end of its
buffer once a blit completes.

Observed: `peek_logical()` on both destination results.

Expected: each destination equals the source data where the mask is set
and equals that run's own background data where it is clear.

Mutation `NMH_MUTATE=A4-cookiecut`: function $AC instead of $CA -- at
least one destination word no longer matches "mask ? source : background".

### A5 -- the destination background is fetched before it is written
Page 176. Claim: `ch6-minterms.c12`.

Proposition: because there is no way to write only a portion of a word,
and only a portion of a masked destination word may need to change, the
background is fetched (read) before the word is written -- and the write
covers the whole word (both byte lanes), which is exactly why the fetch
is what keeps the unmodified background bits.

Program: the A4 cookie-cut program, background channel C pointed at the
same address as destination channel D.

Observed: `ram_log()` at the destination/background address (read order
and byte lanes); `peek_logical()` on the destination.

Expected: a read of the destination address precedes every write to it;
every write there uses both byte lanes; the destination equals A5A5 (its
0505 half is the true C=5555 background, kept only because it was read).

Mutation `NMH_MUTATE=A5-read-before-write`: channel C points somewhere
else -- no read is seen at the destination address before its write, and
the destination no longer equals A5A5 (it picks up the misdirected
channel's own data instead, A9A9).

### A6 -- a shift over 15 bits is a pointer displacement plus a residual shift
Page 179. Claims: `6-shifts-and-masks.c10`, `6-shifts-and-masks.c11`,
`6-shifts-and-masks.c12`, `6-shifts-and-masks.c13`, `ch6-shifts.c10`,
`ch6-shifts.c11`, `ch6-shifts.c12`.

Proposition: the blitter's shift field only holds 0-15 bits, so a shift
greater than 15 bits is realized by advancing the destination pointer to a
higher address by shift/16 words and using shift mod 16 as the residual
barrel shift. Worked example: a 100-bit right shift is a 6-word (12-byte)
pointer advance plus a 4-bit residual shift.

Program: a known multi-word source pattern; destination pointer advanced
12 bytes past the naive (same-address) position; shift field set to 4.

Observed: `peek_logical()` on the destination words.

Expected: equal to a 100-bit logical right shift of the source pattern,
computed independently in the test as the reference (word[i] built from
source words shifted by the 6-word/4-bit decomposition).

Mutation `NMH_MUTATE=A6-shift-decomposition`: advance the pointer by 5
words instead of 6 -- the result no longer equals the 100-bit shift
reference.

### A7a -- descending mode moves an overlapping image down correctly
Page 182. Three assertions on the same block (4 rows x 2 words, source =
rows 0-2, destination = rows 1-3, in place):

**A7a-ascending-corrupts** (`ch6-desc.c1`): p.182, "we overwrite the
second row before we get a chance to copy it" -- a standard (ascending)
copy of the one-row-down move predicts rows 1-3 all reading back as row
0's own data (1111 2222 x3), not merely "not a clean shift". Expected:
`peek_logical()` on SRC+4..+14 equals 1111 2222 1111 2222 1111 2222
exactly. Mutation `NMH_MUTATE=A7a-ascending-corrupts`: use descending mode
instead -- the result no longer matches that corruption pattern.

**A7a-descending-overlap** (`6-descending-mode.c9`, `ch6-desc.c10`):
pointers at the block's last word instead of its first. Expected: a clean
one-row-down shift. Mutation `NMH_MUTATE=A7a-descending-overlap`: DESC=1
but the pointers left at the block's first word -- the result no longer
matches the clean shift.

**A7a-descending-modulo** (`ch6-desc.c11`): "the modulo values ... are set
the same as for an ascending copy" is untested by a modulo of 0 (p.182:
in descending mode "the modulo values will be subtracted rather than
added" -- adding or subtracting zero is the same result either way, so a
zero modulo never exercises this clause). Program: a descending copy of
A3's embedded rectangle (height 5, rows 13-17), pointers at the
rectangle's last word, the same positive BLTDMOD=30 as the ascending A3
run. Expected: the same rectangle-and-margins result as A3. Mutation
`NMH_MUTATE=A7a-descending-modulo`: BLTDMOD=0 instead of 30 -- rows land
at the wrong offset.

### A7b -- control: ascending mode moves an overlapping image up correctly
Page 183. No claim id (see "Claims not asserted": `ch6-copy.c10`).

Proposition: if source and destination overlap, ascending mode is correct
when the destination has the lower memory address -- the same overlap
mechanism as A7a, mirrored, and the package's required control case on
the same measurement basis as A7a's case under suspicion.

Program: the same block, copied one row up (destination = source - 1 row,
overlapping), run in ascending mode.

Observed: `peek_logical()` on the destination rows.

Expected: a clean one-row-up shift of the original pattern.

Mutation `NMH_MUTATE=A7b-ascending-overlap`: descending mode for the same
one-row-up move -- descending order now reads an already-overwritten row,
and the result stops matching the clean shift.

### A8a -- arbitrary-region copy, destination spans more words than source
Page 183. Claims: `6-copying-arbitrary-regions.c1`, `ch6-copy.c1`,
`ch6-copy.c2`, `ch6-copy.c13`, `ch6-copy.c14`, `ch6-copy.c4`, `ch6-copy.c5`.

Proposition: a source image spanning only two words may, shifted, span
three words at the destination -- the manual's own example, a 23-bit-wide
rectangle shifted 12 bits, spans three words. In that case the blit size is
set to the larger span (3, so both source and destination fit), the A
channel's shift value is set to zero, and the first/last masks are set as
if masking the destination (D) data.

Program: a known 23-bit-wide pattern occupying 2 source words, shifted 12
bits, BLTSIZE width 3 words. Channel A (ASH=0) is a constant all-ones
buffer, masked by BLTAFWM=$000F/BLTALWM=$E000 to the destination's own
true edge shape, ANDed with the shifted source (LF=$C0, D=A AND B) -- this
clips the real neighbour word fetched past the true 2-word source (a
stand-in for real adjacent bitmap data) out of the trailing destination
word.

Observed: `peek_logical()` on the 3 destination words.

Expected: equal to the source pattern shifted right 12 bits into a 3-word
field, computed independently in the test (000F FFFF E000).

Mutation `NMH_MUTATE=A8a-dest-grows`: blit size 2 instead of 3 -- the
third destination word never receives its part of the shifted pattern.

### A8b -- arbitrary-region copy, source spans more words than destination
Page 183. Claims: `6-copying-arbitrary-regions.c2`, `ch6-copy.c3`,
`ch6-copy.c4`.

Proposition: an image spanning three words may, for certain shifts, fit in
two destination words. In that case the blit size is still set to the
larger of the two spans (3, the source's own span, so the shifter is not
truncated mid-blit).

Program: a 3-word source (20 true bits, split across all three words)
shifted right 2 bits; blit size 3.

Observed: `peek_logical()` on all 3 destination words.

Expected: the true content compacts into the second and third words (0000
FFFF F000, the reference computed independently in the test); the third
destination word depends on the source's third (real) fetch, which only
happens because the blit size is 3.

Mutation `NMH_MUTATE=A8b-source-larger`: blit size 2 instead of 3 -- the
third fetch never happens and the second (real, nonzero) destination word
stays at its unwritten seed instead of the computed value.

### A8c -- arbitrary-region copy's A-channel row mask
Page 183. Claims: `ch6-copy.c11`, `ch6-copy.c12`.

Proposition: p.183's recipe uses the A DMA channel disabled and preloaded
with all ones as a row mask (rule 1), whose shift value is set the same as
the source (B channel)'s own shift (rule 4, `ch6-copy.c11`), and whose
first/last masks are set as if masking the B source data's own edges
(`ch6-copy.c12`) -- that mask's edges must follow B's shift.

Program: channel A disabled (no DMA fetch), BLTADAT=$FFFF, ASH=BSH=2 (the
same shift as B), BLTAFWM=$0003/BLTALWM=$C000 (B's own shifted edge
shape, since B here is uniformly $FFFF -- "past the true image" data on
both edges); B = $FFFF x3; C=D = background $5555 x3; LF=$CA (cookie-cut,
D = A?B:C).

Observed: `peek_logical()` on the 3 destination words.

Expected: 5555 FFFF F555, the reference computed independently in the
test from the minterm truth table and the barrel-shift arithmetic.

Mutation `NMH_MUTATE=A8c-arb-mask`: $FFFF masks instead of the computed
ones -- word 0 becomes 7FFF instead of 5555.

### A9 -- minterm n is bit n of the LF byte, for every n = 0..7
Page 178. Claim: `ch6-venn.c2`.

Proposition: the minterm numbers run 7 6 5 4 3 2 1 0, most significant
first, so minterm n is bit n of the LF (function) byte -- for every n
individually, not only at n=7 and as a group for n=0-3 (which a
permutation within that group, or any placement of minterms 4 and 5,
would still pass).

Program: known A/B/C channel patterns (A=FF00, B=F0F0, C=CCCC) covering
every combination of bits; eight blits, one per n, LF = 1<<n, each to its
own destination word, A/B/C pointers reloaded before each blit.

Observed: `peek_logical()` on all 8 destination words.

Expected, for n = 7..0: C000 3000 0C00 0300 00C0 0030 000C 0003, each
computed bitwise in the test from the known inputs.

Mutation `NMH_MUTATE=A9-minterm-bit`: n=4's slot gets n=3's bit -- that
one word no longer matches.

### A9b -- the inverse of a source is minterms 0-3
Page 179. Claim: `ch6-venn.c8`.

Proposition: NOT A is minterms 0, 1, 2 and 3, i.e. LF = $0F selects
exactly where A is 0, whatever B and C are.

Program: the same A/B/C patterns as A9, one blit with LF = $0F.

Observed: `peek_logical()` on the destination.

Expected: equal to NOT A, computed bitwise in the test (00FF).

Mutation `NMH_MUTATE=A9b-not-a`: LF = $30 (minterms 5, 4, both requiring
A=1) instead of $0F -- no longer equals NOT A.

### A10 -- minterms combine by OR-ing their LF bits
Page 178, 179. Claims: `ch6-venn.c5`, `ch6-venn.c11`.

Proposition: minterms are combined ("or"ed) by OR-ing their bit values
together; the combination "A and B" (regardless of C) is minterms 7 and 6,
i.e. LF = $C0.

Program: the same known A/B/C patterns as A9, one blit with LF = $C0.

Observed: `peek_logical()` on the destination.

Expected: equal to A AND B, computed bitwise in the test, independent of C
(F000).

Mutation `NMH_MUTATE=A10-minterm-or`: LF = $80 instead of $C0 -- the
result loses the AB~C=110 case and no longer equals A AND B.

### A11 -- the blitter operates on words: a pointer's low bit is ignored
Page 172. Claim: `6-dma-channels.c79`.

Proposition: the blitter only operates on words, so the least significant
bit of a pointer is ignored.

Program: two known, distinct words at consecutive even addresses; a copy
blit whose A pointer is set to the first word's address plus 1 (an odd
byte address).

Observed: `peek_logical()` on the destination word.

Expected: equal to the word at the even (LSB-truncated) address.

Mutation `NMH_MUTATE=A11-word-only`: point the pointer's even part at the
next word's address instead (still an odd byte address, LSB still
ignored) -- the destination now holds the second word's data.

### A12 -- shift direction is set by the mode; the shift value is within-word
Page 182, 183, 200. Claims: `ch6-copy.c7`, `ch6-copy.c8`, `ch6-copy.c9`.

Proposition: when shifting, ascending mode shifts right and descending
mode shifts left (`ch6-copy.c7`, `ch6-copy.c8` -- p.182: "shifts are then
towards the left, rather than the right"; p.200 gives the same rule for
both modes). The shift value itself is the bit position of the leftmost
edge within a word, not an absolute shift (`ch6-copy.c9`): moving an image
four pixels left can be realized as a rightward within-word shift (ASH=12)
combined with a destination pointer one word lower, because the edge lands
at bit 14 of the word before.

Program, three cases on source 1111 2222 3333:
- **A12-shift-right**: ASH=4, ascending, same-address dest pointer.
- **A12-shift-left**: the same ASH=4, descending, pointers at the last
  word.
- **A12-within-word**: ASH=12, ascending, dest pointer one word lower
  than "naive" (same-address).

Observed: `peek_logical()` on the destination words in each case.

Expected: shift-right gives 0111 1222 2333; shift-left gives 1112 2223
3330; within-word gives 0001 1112 2223 at DST-2, DST, DST+2 -- all
computed independently in the test from the barrel-shift arithmetic.

Mutations: `NMH_MUTATE=A12-shift-right` (descending instead of ascending)
and `NMH_MUTATE=A12-shift-left` (ascending instead of descending) are each
other's mutant -- the wrong direction's result no longer matches.
`NMH_MUTATE=A12-within-word`: the naive (same-address) pointer instead of
one word lower -- the result no longer matches the leftward-move
reference.

### A13 -- figure 6-1 (p.171) as a blit
Page 171. Claims: `6-dma-channels.c30`, `6-dma-channels.c31`,
`6-dma-channels.c32`, `ch6-dma.f1`, `ch6-dma.f2`, `ch6-dma.f3`.

Proposition: figure 6-1 draws a bitplane's word addresses increasing by
one across a row and across the row boundary, from word 20 to word 61 (7
columns x 6 rows) -- testable as the blit geometry it depicts rather than
as a picture.

Program: BLTAPT = word address 20 ($000028), a plain copy, width 7,
height 6, BLTAMOD=0. The overlay is cleared first (any write to CIA-A):
word 20 is inside $000000-$07FFFF, and while the overlay is active chip0's
own bank is gated by "!ovl || cpu_hlt", not just CPU reads.

Observed: `ram_log()`, channel A's read accesses, coalesced.

Expected: exactly word addresses 20 through 61 (42 reads), consecutive
within each row and across every row boundary -- the figure's own extent,
forced by its own start (20) and size (6x7) exactly as its cells show.

Mutation `NMH_MUTATE=A13-fig61`: start channel A one word later (word 21)
-- the read range no longer matches 20..61.

### A14 -- figure 6-3 (p.178)
Page 177, 178. Claim: `ch6-venn.f-labels`.

Proposition: the diagram's three circles are labelled A, B and C: item 1
of p.178 gives the A circle as minterms 7,6,5,4 (LF=$F0, D=A); Table 6-1
(p.177) gives LF=$CC for the B circle (D=B) and LF=$AA for the C circle
(D=C).

Program: known A/B/C patterns (A=FF00, B=F0F0, C=CCCC); three blits, LF=
$F0, $CC, $AA in turn, each to its own destination, pointers reloaded
before each blit.

Observed: `peek_logical()` on all three destinations.

Expected: FF00 (=A), F0F0 (=B), CCCC (=C).

Mutation `NMH_MUTATE=A14-fig63`: LF=$0F (NOT A) instead of $F0 for the
first blit -- no longer equals A.

### A15 -- figure 6-4 (p.181) extraction recipe
Page 181. Claims: `6-shifts-and-masks.c32`, `ch6-shifts.f1`.

Proposition: the figure's own recipe (BLTCON0=$07CA, BLTCON1=$5000,
BLTADAT=$FFFF, BLTAFWM=$07FF, BLTALWM=$FFF0) reads exactly two consecutive
source words per row through channel B and writes two per row through
channel D -- the drawing's whole span is a two-word source row.

Program: the p.181 recipe, source 2 words/row x 3 rows.

Observed: `ram_log()`, channel B's reads and channel D's writes, counted.

Expected: exactly 6 B-channel reads and 6 D-channel writes (2/row x 3
rows).

Caution, recorded rather than graded: the figure's drawn second-word mask
is 7 bits ("11111110 00000000", matching a 23-bit extraction), but its
printed final rows take 8 bits from the source (row 1 "00000000
11111111", where the recipe's own second source word FE00 gives "00000001
11111111"). That one-bit conflict between the drawing and the printed
numbers is not resolved here, so this assertion grades the recipe's
structure (reads/writes per row), not those exact final-row values.

Mutation `NMH_MUTATE=A15-fig64`: 3 words/row instead of 2 -- the counts
become 9 and 9.

### A16 -- ch6-minterms.c8: the worked example's five minterms
Page 175. Claim: `ch6-minterms.c8`.

Proposition: p.175's worked example ("A and not C or B") expands into
"the five minterms", which "correspond to BLTCON0 bit positions of 6, 4,
7, 3, and 2" -- LF = $DC.

Program: known A/B/C patterns (A=FF00, B=F0F0, C=CCCC), one blit with
LF=$DC.

Observed: `peek_logical()` on the destination.

Expected: F3F0, computed independently in the test from the minterm truth
table and the stated bit positions.

Mutation `NMH_MUTATE=A16-mintdc`: LF=$DE instead of $DC -- no longer
equals F3F0.

### A17 -- p.174's own worked truth-table-reading example
Page 174. Claims: `6-function-generator.c24`, `ch6-funcgen.c6`.

Proposition: p.174's own worked example, D = A+B, read from the bottom of
the truth table up, is LF = $FC; the top-down (bit-reversed) reading would
give a different function.

Program: known A/B patterns (A=FF00, B=F0F0), one blit with LF=$FC.

Observed: `peek_logical()` on the destination.

Expected: FFF0 (A OR B), computed independently in the test.

Mutation `NMH_MUTATE=A17-truthorder`: the top-down (bit-reversed) reading,
LF=$3F -- gives 0FFF instead (NOT (A AND B)), a different function.

### A18 -- moving a sixteen-colour image needs four copy operations
Page 171. Claims: `6-dma-channels.c36`, `ch6-dma.c34`.

Proposition: because a copy blit moves one contiguous source/destination
pair per BLTSIZE trigger, moving a four-bitplane image needs four separate
copy operations, one per plane.

Program: four one-word "plane" buffers, one blit per plane, each its own
pointer pair.

Observed: `peek_logical()` on all four plane destinations.

Expected: after all four blits, every destination equals its own source.

Mutation `NMH_MUTATE=A18-fourcopies`: stop after three of the four planes
-- the fourth destination stays at its unwritten seed instead of its
source.

### A19 -- some of a pointer's most significant bits are ignored
Page 170. Claims: `6-dma-channels.c21`, `ch6-dma.c19`.

Proposition: some of the most significant bits of a pointer address are
ignored, because only Chip memory is accessible -- true independent of
chip size, since BLTDPTH (and the other pointer-high registers) only
store the low 5 bits of the word written to them (pointer bits 20:16), so
bits above 20 of a 32-bit-style address are structurally unreachable.

Program: a plain copy blit whose destination pointer is DST with bit 24
also set (DST|$01000000).

Observed: `peek_logical()` on the destination.

Expected: the source pattern lands at DST unchanged -- bit 24 never
reaches the register.

Mutation: not exercised, by a scope decision. The register only stores
bits 20:16 of the word it is written; every possible stimulus already
demonstrates the same structural truncation, so there is no variant that
would leave it undemonstrated.

### A20 -- 512 KB Chip RAM ignores 13 pointer bits; more memory ignores fewer
Page 170. Claims: `6-dma-channels.c22`, `6-dma-channels.c23`,
`ch6-dma.c20`, `ch6-dma.c21`.

Proposition: on a machine with 512 KB of Chip memory, a pointer's most
significant 13 bits are ignored; on a machine with more Chip memory, fewer
bits are ignored.

Program: the harness's `Config.memory_config` field selects the Chip RAM
size; under 0 (512 KB), every Chip bank folds onto the first 512 KB
(confirmed against `minimig_bankmapper.v`: under `memory_config=0`,
`bank_r[3:0] = {0,0,0, chip3|chip2|chip1|chip0}`), so any logical address
in $080000-$0FFFFF collapses to its offset within $000000-$07FFFF. Because
`load_program()`/`peek_logical()` assume the 2 MB identity map (physical
== logical), this case pokes the assembled fixture program directly with
`poke_word()` at the collapsed physical address and reads results with
`peek_word()`, bypassing those two harness calls -- a harness gap named
here rather than a change to the harness. The normal run uses
`memory_config=0`; the plain fixture program and its own known source
pattern are reused unchanged.

Observed: `peek_word()` at the collapsed physical address corresponding to
logical DST.

Expected: the source pattern (1234 ...) at physical DST-$080000
($006000), confirming bit 19 (and above, within the pointer's 21-bit
range) is ignored under the 512 KB configuration.

Mutation `NMH_MUTATE=A20-mem512`: `memory_config=3` (2 MB) instead of 0,
with the normal `load_program()`/`peek_logical()` (no collapse needed) --
the pattern is honoured at the uncollapsed address instead, so nothing
reaches the physical location the 512 KB case expects, demonstrating that
a larger chip ignores fewer bits.

## Claims not asserted

13 of the 83 claims are declined, all as a **scope decision** (a fixture
this package did not build would settle them), not as unfalsifiable at
this boundary -- bitplane DMA addresses on the RAM port, and r/g/b, do
encode both a screen's resolution and its color depth, so a display
fixture is buildable in principle, just not built here.

**Display-memory arithmetic, 10 ids** (`6-memory-layout.c5`,
`6-memory-layout.c6`, `6-memory-layout.c7`, `6-memory-layout.c8`,
`6-memory-layout.c9`, `ch6-memlayout.c3`, `ch6-memlayout.c4`,
`ch6-memlayout.c5`, `ch6-memlayout.c6`, `ch6-memlayout.c7`): a 320x200,
16-color screen's four-bitplane, 8000-byte-per-plane, 200-row,
40-byte/20-word-per-row layout. The fixture that would settle it: BPLCON0
BPU=4, lowres, DDFSTRT=$0038, DDFSTOP=$00D0 (p.60), 200 lines, BPLxMOD=0 --
each plane fetched as 20 words (40 bytes) per line over 200 lines, one
contiguous 8000-byte range on the RAM port; and pixel values 0-15 on
`video()` showing 16 distinct colours.

**Bit-depth corollary's display half, 2 ids** (`6-dma-channels.c35`,
`ch6-dma.c33`): "a 16-color image needs four bitplanes." The "four copy
operations" half of the same corollary (`6-dma-channels.c36`,
`ch6-dma.c34`) is graded in A18 without a display fixture; this half needs
the same display fixture as the memory-layout ids above.

**`ch6-copy.c10`'s display clause, 1 id**: the claim bundles "ascending
mode is used if the destination has a lower memory address" (graded by
A7b, memory-only) with "a lower memory address is higher on the display".
A7b does not measure the display clause, so the id is withdrawn whole per
the coverage rule (a bundled clause this package does not test is taken
off, not partially credited). The fixture that would settle it: the same
display fixture, showing a bitplane whose lower-address rows appear on
earlier r/g/b lines than its higher-address rows.

## Coverage note

70 of the 83 claims are asserted, across 25 assertions (some ids appear on
two assertions' lines where a duplicate-extraction pair states the same
fact, or where the manual's own duplicated numbers -- e.g. figure 6-2's
`c65`-equivalent arithmetic -- are the same evidence). 13 claims are
declined, all as a scope decision naming the fixture that would settle
them.
