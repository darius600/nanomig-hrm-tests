# ch6-blitter-geometry

Integration package for the claims in `ch6-blitter-geometry-spec-stage.jsonl`
(83 claims, chapter 6 of the Amiga Hardware Reference Manual, 3rd edition,
pages 169-183 and 200): the blitter's addressing and masking geometry --
row masks, modulo/blit-size arithmetic, shift decomposition, ascending and
descending pointer order, the arbitrary-region copy recipe, and the minterm
function generator's bit layout.

This document is written from the spec-stage file alone (`uid`, `page`,
`quote`, `says`). It does not open `ch6-blitter-geometry.jsonl` or the RTL.
Register offsets used below are the ones already in
`integration/harness/nanomig.i` (BLTCON0/1, BLTAFWM/ALWM, BLTAPTH/L,
BLTBPTH/L, BLTCPTH/L, BLTDPTH/L, BLTSIZE, BLTAMOD/BMOD/CMOD/DMOD, BLTADAT/
BDAT/CDAT, DMACON) -- part of the harness contract, not the RTL. The bit
layout inside BLTCON0/1 and DMACON (which nibble is ASH/BSH/LF, which bit is
BLTEN) is not in `nanomig.i`; those values are pinned down in step 2/3
against the manual and cross-checked once the RTL is open, not guessed here.

Every program drives `nanomig.v` only through the blitter's custom
registers and observes only the RAM port (`ram_log()`) and
`peek_logical()`, per `integration/harness/nanomig_harness.h`. Each program
enables blitter DMA (master DMA enable + blitter DMA enable in DMACON),
writes the channel pointers, modulos, masks and BLTCON0/1 for its case, and
writes BLTSIZE last to start the blit, then waits and stores a completion
mark before `DONE`.

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
BLTSIZE last. A second build of the same source, selected by an environment
variable, performs every step except the final BLTSIZE write.

Observed: `peek_logical()` on the destination range, after `run_until_write`
returns.

Expected: with BLTSIZE written, the destination equals the known source
pattern exactly. With BLTSIZE withheld, the destination is unchanged from
its pre-blit seed.

Mutation: `MUTATION A0-fixture stimulus red withhold the BLTSIZE write` --
the destination stays at its seed value instead of the source pattern, so
the "destination equals source" check fails.

### A1 -- first/last word of each raster row can be masked
Page 169. Claims: `6-intro.c13`, `6-intro.c14`, `ch6-intro.c12`,
`ch6-intro.c13`.

Proposition: block-move mode can mask the first word of each raster row
(BLTAFWM) and the last word of each raster row (BLTALWM), and it does so on
every row of a multi-row blit, not only the first/last row of the whole
blit.

Program: a copy blit, source all-ones, destination pre-seeded to all-zero,
width 3 words/row, height 2 rows, BLTAFWM masking only the low byte,
BLTALWM masking only the high byte, the middle word unmasked.

Observed: `peek_logical()` on all 6 destination words (2 rows x 3 words).

Expected: on both rows, word 0 = the FWM-masked value, word 1 = all-ones
(unmasked), word 2 = the LWM-masked value -- the same pattern on row 1 as
on row 0, showing the masks apply per row.

Mutation: `MUTATION A1-row-masks stimulus red use a different FWM value`
-- word 0 of each row no longer equals the value the check expects.

### A2 -- zero-modulo raster addressing is a "plus one" continuation
Page 171. Claims: `ch6-dma.c30`, `ch6-dma.c31`.

Proposition: images are normally stored so each word on a line sits at the
address one greater than its left neighbor, and each line is a "plus one"
continuation of the line before it -- i.e. with modulo 0, the last word of
one row and the first word of the next are consecutive addresses.

Program: a copy blit, several rows wide and tall, all modulo registers 0.

Observed: `ram_log()`, filtered to the destination's write accesses,
coalesced and sorted by address.

Expected: the addresses form one unbroken run with no gap at any row
boundary.

Mutation: `MUTATION A2-plus-one stimulus red add a 2-word destination
modulo` -- a gap appears at every row boundary.

### A3 -- embedded-rectangle modulo arithmetic
Page 172. Claim: `6-dma-channels.c65`.

Proposition: for a blit into a rectangle embedded in a wider bitmap, twice
the blit width in words plus the modulo (in bytes) equals the containing
row's full byte width. Using the manual's own figure 6-2 numbers: a 40-byte
row, a rectangle 10 bytes (5 words) wide, 12 bytes of untouched bitmap to
its left and 18 to its right -- modulo = 40 - 2*5 = 30 bytes (the 12 + 18
skipped on each row).

Program: a scratch "bitmap" of 40 bytes/row x 3 rows, its 12-byte left and
18-byte right margins pre-seeded with a sentinel pattern; a copy blit into
the embedded 5-word rectangle with BLTDMOD = 30.

Observed: `peek_logical()` across the whole 3-row bitmap.

Expected: the rectangle holds the source data on every row; the left and
right margins keep the sentinel, unchanged, on every row.

Mutation: `MUTATION A3-embedded-modulo stimulus red use a 20-byte modulo`
-- rows land at the wrong offset and the margin check on row >= 1 fails.

### A4 -- a cookie-cut mask is one bitplane regardless of background depth
Page 176, 183. Claims: `6-function-generator.c40`, `ch6-minterms.c9`,
`ch6-copy.c6`.

Proposition: a cookie-cut mask needs only a single bitplane no matter how
many bitplanes the background has, because the same mask plane is reused
per background plane; the cookie-cut function is $CA.

Program: one mask pattern (channel A), one "car" source pattern (channel
B), and two different background patterns (channel C, in turn), each
blitted with BLTCON0's function set to $CA.

Observed: `peek_logical()` on both destination results.

Expected: each destination equals the source data where the mask is set
and equals that run's own background data where it is clear -- the same
mask producing a correct cut against two different backgrounds without
being changed.

Mutation: `MUTATION A4-cookiecut stimulus red use function $AC instead of
$CA` -- at least one destination word no longer matches "mask ? source :
background".

### A5 -- the destination background is fetched before it is written
Page 176. Claim: `ch6-minterms.c12`.

Proposition: because only a portion of a masked destination word may need
to change, the background is fetched (read) before the word is written.

Program: the A4 cookie-cut program, background channel C pointed at the
same address as destination channel D.

Observed: `ram_log()` at the destination/background address.

Expected: a read of that address precedes the write to it.

Mutation: `MUTATION A5-read-before-write stimulus red point channel C
somewhere else` -- no read is seen at the destination address before its
write.

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

Mutation: `MUTATION A6-shift-decomposition stimulus red advance the
pointer by 5 words instead of 6` -- the result no longer equals the 100-bit
shift reference.

### A7a -- descending mode moves an overlapping image down correctly
Page 182. Claims: `6-descending-mode.c9`, `ch6-desc.c1`, `ch6-desc.c10`,
`ch6-desc.c11`.

Proposition: moving an image one row down (to increasing addresses) with a
standard (ascending) copy overwrites the second row before it is copied.
Descending mode avoids this by pointing the address registers at the
block's last word instead of its first, keeping the modulo, blit size and
every other parameter the same as the ascending case.

Program: a source block of several rows with a known per-row pattern,
copied one row down within the same memory (destination = source + 1 row,
overlapping). Run once in ascending mode (pointers at the first word), once
in descending mode (pointers at the last word), same BLTSIZE and modulo in
both runs.

Observed: `peek_logical()` on the destination rows after each run.

Expected: the ascending run's result does not equal a clean one-row-down
shift of the original pattern (the overwrite the page describes). The
descending run's result equals a clean one-row-down shift.

Mutation: `MUTATION A7a-descending-overlap stimulus red leave the pointers
at the block's first word in the descending run` -- the descending run's
result stops matching the clean shift.

### A7b -- control: ascending mode moves an overlapping image up correctly
Page 183. Claim: `ch6-copy.c10`.

Proposition: if source and destination overlap, ascending mode is correct
when the destination has the lower memory address (higher on the display)
-- the same overlap mechanism as A7a, mirrored.

Program: the same block, copied one row up (destination = source - 1 row,
overlapping), run in ascending mode.

Observed: `peek_logical()` on the destination rows.

Expected: PASS on the same basis as A7a -- the result equals a clean
one-row-up shift of the original pattern. This is the package's control
case: a green result on the same overlap/measurement setup as the case
under suspicion in A7a.

Mutation: `MUTATION A7b-ascending-overlap stimulus red use descending mode
for the same one-row-up move` -- descending order now reads an
already-overwritten row, and the result stops matching the clean shift.

### A8a -- arbitrary-region copy, destination spans more words than source
Page 183. Claims: `6-copying-arbitrary-regions.c1`, `ch6-copy.c1`,
`ch6-copy.c2`, `ch6-copy.c13`, `ch6-copy.c14`, `ch6-copy.c4`, `ch6-copy.c9`,
`ch6-copy.c5`.

Proposition: a source image spanning only two words may, shifted, span
three words at the destination -- the manual's own example, a 23-bit-wide
rectangle shifted 12 bits, spans three words. In that case the blit size is
set to the larger span (3, so both source and destination fit), the A
channel's shift value is set to zero, and the first/last masks are set as
if masking the destination (D) data -- the shift values themselves are the
bit position of the pattern's left edge within a word (0-15), not an
absolute shift.

Program: a known 23-bit-wide pattern occupying 2 source words, shifted 12
bits, BLTSIZE width 3 words, masks set for the 3-word destination span.

Observed: `peek_logical()` on the 3 destination words.

Expected: equal to the source pattern shifted right 12 bits into a 3-word
field, computed independently in the test.

Mutation: `MUTATION A8a-dest-grows stimulus red set blit size to 2 instead
of 3` -- the third destination word never receives its part of the
shifted pattern, and the result stops matching the reference.

### A8b -- arbitrary-region copy, source spans more words than destination
Page 183. Claims: `6-copying-arbitrary-regions.c2`, `ch6-copy.c3`,
`ch6-copy.c11`, `ch6-copy.c12`, `ch6-copy.c4`, `ch6-copy.c5`.

Proposition: an image spanning three words may, for certain shifts, fit in
two destination words. In that case the blit size is still set to the
larger of the two spans (3, the source's own span, so it is not truncated
mid-blit), the A channel's shift value is set the same as the source (B
channel)'s shift, and the first/last masks are set as if masking the B
source data; masking is applied to mask out the unwanted (spilled-over)
data.

Program: a known 3-word source pattern, shifted so its non-zero content
compacts into 2 destination words, blit size 3, masks computed for the
source's own span, a sentinel word immediately after the 2-word real
footprint.

Observed: `peek_logical()` on the 2 real destination words and the
sentinel word.

Expected: the 2 real words equal the compacted pattern, computed
independently in the test; the sentinel word is unchanged.

Mutation: `MUTATION A8b-source-larger stimulus red use $FFFF masks instead
of the computed ones` -- unmasked data spills into the sentinel word and
the "sentinel unchanged" check fails.

### A9 -- minterm n is bit n of the LF byte
Page 178, 179. Claims: `ch6-venn.c2`, `ch6-venn.c8`.

Proposition: the minterm numbers run 7 6 5 4 3 2 1 0, most significant
first, so minterm n is bit n of the LF (function) byte. Checked at both
ends: minterm 7 alone (LF = $80) selects exactly where A, B and C are all
1; minterms 0-3 together (LF = $0F, "not A") select exactly where A is 0,
regardless of B and C.

Program: known A/B/C channel patterns with every combination of bits
present; two blits, one with LF = $80, one with LF = $0F.

Observed: `peek_logical()` on both destinations.

Expected: LF = $80 result equals A AND B AND C, computed bitwise in the
test from the known inputs. LF = $0F result equals NOT A.

Mutation: `MUTATION A9-minterm-bit stimulus red use LF = $40 instead of
$80` -- the result no longer equals A AND B AND C.

### A10 -- minterms combine by OR-ing their LF bits
Page 178, 179. Claims: `ch6-venn.c5`, `ch6-venn.c11`.

Proposition: minterms are combined ("or"ed) by OR-ing their bit values
together; the combination "A and B" (regardless of C) is minterms 7 and 6,
i.e. LF = $C0.

Program: the same known A/B/C patterns as A9, one blit with LF = $C0.

Observed: `peek_logical()` on the destination.

Expected: equal to A AND B, computed bitwise in the test, independent of C.

Mutation: `MUTATION A10-minterm-or stimulus red use LF = $80 instead of
$C0` -- the result loses the AB~C=110 case and no longer equals A AND B.

### A11 -- the blitter operates on words: a pointer's low bit is ignored
Page 172. Claim: `6-dma-channels.c79`.

Proposition: the blitter only operates on words, so the least significant
bit of a pointer is ignored.

Program: two known, distinct words at consecutive even addresses; a copy
blit whose A pointer is set to the first word's address plus 1 (an odd
byte address).

Observed: `peek_logical()` on the destination word.

Expected: equal to the word at the even (LSB-truncated) address, not the
next word and not a byte-shifted mix.

Mutation: `MUTATION A11-word-only stimulus red point the pointer at the
next word's address plus 1` -- the destination now holds the second word's
data, which no longer equals the value the check still expects.

## Claims not asserted

**Display memory layout arithmetic -- unfalsifiable at this boundary.**
`nanomig.v` has no port that encodes a screen's resolution or color depth;
these are programmer arithmetic (200 rows x 40 bytes = 8000 bytes/plane,
16 colors = 4 planes) about how a screen would be organized, not a
register or observable chip behavior:
`6-memory-layout.c5`, `6-memory-layout.c6`, `6-memory-layout.c7`,
`6-memory-layout.c8`, `6-memory-layout.c9`, `ch6-memlayout.c3`,
`ch6-memlayout.c4`, `ch6-memlayout.c5`, `ch6-memlayout.c6`,
`ch6-memlayout.c7`.

**Readings of a printed figure -- unfalsifiable at this boundary.** These
claims state what is drawn or labelled on a page image (a grid of cell
numbers, an axis caption and its tick marks, a skip-left/skip-right
callout, circle labels). No port renders or labels a diagram; the
underlying geometric principle a figure illustrates is asserted instead,
from its prose claims, where this document does so (figure 6-1's zero-
modulo continuation in A2; figure 6-2's modulo arithmetic in A3):
`6-dma-channels.c30`, `6-dma-channels.c31`, `ch6-dma.f1`, `ch6-dma.f2`,
`ch6-dma.f3` (figure 6-1); `6-dma-channels.c73`, `6-dma-channels.c74`,
`6-dma-channels.c75`, `6-dma-channels.c76`, `ch6-dma.f5`, `ch6-dma.f7`,
`ch6-dma.f8` (figure 6-2); `ch6-venn.f-labels` (figure 6-3);
`6-shifts-and-masks.c32`, `ch6-shifts.f1` (figure 6-4).

**A pedagogical example's own numbers -- unfalsifiable at this boundary.**
"The example bitplane occupies word addresses 20 through 61" is the
figure's illustrative numbering, not a hardware invariant; nothing forces
those addresses. The zero-modulo mechanism it illustrates is asserted
generally in A2. `6-dma-channels.c32`.

**Bit-depth arithmetic corollary -- unfalsifiable at this boundary.** "A
16-color image needs four bitplanes, so moving it needs four copy
operations" is color-depth arithmetic plus the general fact that one blit
call moves one contiguous address range, which every other assertion in
this package already demonstrates by construction (each uses one blit per
region). No port distinguishes this claim from that already-covered fact:
`6-dma-channels.c35`, `6-dma-channels.c36`, `ch6-dma.c33`, `ch6-dma.c34`.

**Shift-direction mode choice -- not exercised, by a scope decision.**
"Ascending mode when shifting right, descending when shifting left" is the
same overlap-avoidance mechanism as A7a/A7b, combined with a simultaneous
shift. A7a/A7b test the overlap mechanism without a shift; A6/A8a/A8b test
shifting without overlap. Settling this needs a program that shifts data
while source and destination overlap, in both shift directions, comparing
ascending against descending outcomes -- feasible, not built in this pass:
`ch6-copy.c7`, `ch6-copy.c8`.

**A printed table's reading order -- unfalsifiable at this boundary.**
"LF-bit values are read from the truth table bottom-to-top" describes how
a person derives the LF byte from a page's printed truth table, not a
chip behavior; no port reflects how a table was read:
`6-function-generator.c24`, `ch6-funcgen.c6`.

**Antecedent not resolvable from this claim alone.** "Those five minterms
correspond to BLTCON0 bit positions 6, 4, 7, 3 and 2" does not name which
five minterms within its own `says`/`quote` -- this file's step-1 rule is
to use the spec-stage claim alone, and this claim's own text does not
supply a determinate expected value. Settling it needs the surrounding
page 175 prose this claim was cut from: `ch6-minterms.c8`.

**Needs a non-default memory configuration, itself needing the RTL --
declining rather than reading ahead.** "On a 512 KB machine the top 13
pointer bits are ignored; more chip memory ignores fewer" would need the
harness's `Config.memory_config` set to a smaller chip-RAM size than the
2.0 MB (`0x03`) case the harness comment documents and `nanomig.i`'s
`PROGRAM_ORG`/`RESULTS`/chip-RAM layout assumes. Which of the field's other
three encodings means 512 KB, and whether the program's own load address
stays safely backed by chip RAM under it, is not stated anywhere this step
reads (only the 2.0 MB case is documented in the harness). This is a
harness-capability question to settle first, not an RTL change to attempt:
`6-dma-channels.c21`, `6-dma-channels.c22`, `6-dma-channels.c23`,
`ch6-dma.c19`, `ch6-dma.c20`, `ch6-dma.c21`.

## Coverage note

42 of the 83 claims are asserted, across 14 assertions (some ids appear on
two assertions' lines where the same rule -- "blit size is the larger of
the two spans", "masking removes unwanted data" -- is independently
re-established with a different concrete pair of spans in A8a and A8b).
41 claims are withdrawn whole, grouped above by shared reason.
