# ch6-blitter-line-fill

21 claim ids across two unrelated chapter-6 topics plus a third group this
package cannot settle. Every id below is either carried by an assertion or
withdrawn with a reason; none is silently dropped.

## Topic 1: the line-mode claims (page 189-191)

Nine ids describe the recipe a program follows before starting a
line-drawing blit: compute the raw coordinate deltas (page 190), then
normalize them so the larger becomes the major axis "dx" and the smaller
the minor axis "dy", swapping if necessary (page 191). The recipe has no
port-visible consequence of its own -- there is no register that reports
"dx" or "dy" back; its only observable effect is the line the blitter
draws once the normalized values are programmed into the registers page
191's "REGISTER SUMMARY FOR LINE MODE" lists. So the assertion is on the
drawn line itself, not on an intermediate CPU value.

### Assertion (drawn line)

- Claims: `6-line-mode.c39`, `6-line-mode.c40`, `ch6-line.c12`,
  `ch6-line.c13` (page 190, raw dx/dy); `6-line-mode.c41`,
  `6-line-mode.c42`, `ch6-line.c14`, `ch6-line.c15`, `ch6-line.c16` (page
  191, normalized dx/dy and the rearrangement). One assertion settles all
  nine: the normalized values are exactly what a correctly drawn line for
  these endpoints requires, and a line drawn from unnormalized values comes
  out the wrong length (see Mutation) -- so the drawn line is evidence for
  the whole recipe, not just its last step.
- Proposition: for endpoints (x1,y1)=(10,5), (x2,y2)=(3,40), a line-mode
  blit set up per page 191's register summary, with the octant fixed by
  Fig 6-8/Table 6-3 (page 190) for dy(35) > dx(7) -- Y the major axis, x
  falling, y rising -- draws a line with the following three properties,
  the only ones the pages fix:
  1. First pixel at (10,5), last pixel at (3,40) (page 189: "the blitter
     draws a line from one point to another").
  2. Exactly dx_normalized+1 = 36 pixels (page 191: "We set the blit
     height to the length of the line, which is dx + 1").
  3. Consequently one pixel in every row y=5..40, with x falling by 0 or 1
     between consecutive rows (dx is the minor axis and dx < dy, so x
     cannot change in every row, and cannot change by more than one row
     per pixel since dy is major).
  What the pages do not fix: the initial error term and the two modulos
  fix the exact row at which x falls, but not a formula this package's
  claims state -- so no exact intermediate (x,y) pair is graded, only the
  three properties above.
- Program: `prog_line.s` builds this blit (BLTAPT as the Bresenham
  accumulator, BLTAMOD/BLTBMOD as its two increments, BLTCON0's ASH as the
  starting bit position, BLTCON1's octant bits) and leaves the result in a
  36-word row buffer, one word per row y=5..40, each word's single set bit
  at bit-index x (words never cross a boundary since x stays within
  3..10). The register-level encoding (which bit is which octant flag, the
  accumulator's bit-drop/sign-extension format) is mechanical setup, not a
  page-derived value; it is what the full claims file's RTL citations
  (`agnus_blitter.v`'s line-mode states, `agnus_blitter_adrgen.v`'s
  accumulator sign) are for, and its correctness was checked against the
  three graded properties above by running the blit, not assumed.
- Observed: `peek_logical` on all 36 row words directly (the program does
  not copy them through `RESULTS`).
- Expected: row 0's word has its one set bit at index 10; row 35's word
  has its one set bit at index 3; every row's word has exactly one set
  bit; consecutive rows' bit indices never rise and never fall by more
  than one.

### Fixture: FIXTURE-LINE

Before the blit runs, `prog_line.s` stores a fixed constant ($BEEF) to
`RESULTS`, read back and checked first -- proof `load_program`/
`peek_logical` work for this package's own new program before the drawn
line is trusted.

### Mutation

`prog_line_mut.s` = `prog_line.s` with `MUTATE_UNNORM` defined: it feeds
the raw, unswapped deltas (dx0=7 as if it were the major axis, dy0=35 as
if it were the minor) into BLTAPT/BLTAMOD/BLTBMOD/BLTSIZE instead of the
normalized dmax=35/dmin=7. BLTSIZE's height field becomes dx0+1=8 instead
of 36, so the drawn line stops 28 rows short: rows 8-35 are never written
(fail "exactly one set bit") and the last-pixel check fails outright.
Recorded as:

    MUTATION 6-line-mode.c39 stimulus red unnormalized deltas fed into BLTAPT/BLTAMOD/BLTBMOD/BLTSIZE, height becomes 8 instead of 36

`prog_line_fixture_mut.s` = `prog_line.s` with `MUTATE_FIXTURE` defined:
stores $DEAD instead of $BEEF, turning FIXTURE-LINE red on its own,
unchanged expected value. Recorded as:

    MUTATION FIXTURE-LINE stimulus red stored constant changed to DEAD instead of BEEF

## Topic 2: area fill requires descending mode (pages 182, 184, 200)

Six ids across three pages state one rule two ways: four say descending
mode is necessary/must be used for area fill (pages 182, 184); two say
area fill only works correctly in descending mode (page 200) -- the same
rule stated as a requirement and as its converse. Only a run that is *not*
descending can falsify "necessary": a descending run that fills correctly
is equally consistent with "descending happens to work" and with
"descending is required", but an ascending run that also filled correctly
would refute "required" outright. So the six ids sit on the ascending
assertion, which is the one that can fail that way; the descending run is
the control on the same measurement basis, carrying no ids of its own.

### Fill direction, from the printed pages alone

- **Word order relative to the descending pointer.** Page 182: "initialize
  the address pointer registers to point to the last word in a block,
  rather than the first word", and "the first word mask masks the last
  word in a row (which is still the first word fetched)". The last word of
  a row is its right edge (page 181: "a first word mask with the most
  significant five bits set to zero ... and a last word mask with the
  least significant four bits set to zero", Figure 6-4 draws the first
  word on the left; page 182: "a last word mask with the rightmost nine
  bits set to zero"). So word 4 (the row's last word, where the descending
  pointer starts) holds the rightmost pixels.
- **Bit order within a word.** Page 180: "the least significant
  (rightmost) four bits of the first word shifted in" -- bit 0 is the
  rightmost pixel of a word.
- **Direction of the fill.** Page 186: "The blitter uses the fill
  carry-in bit as the starting fill state beginning at the rightmost edge
  of each line. For each '1' bit in the source area, the blitter flips
  the fill state, either filling or not filling the space with ones. This
  continues for each line until the left edge of the blit is reached, at
  which point the filling stops." Descending mode -- pointer at the last
  (rightmost) word, stepping toward the first (leftmost) -- is what walks
  the row in this right-to-left order the fill logic assumes; the other
  direction does not.
- **Inclusive fill.** Page 184: "The inclusive fill mode fills between
  lines, leaving the lines intact", and "(Of course, fills are always done
  on full 16-bit words.)"
- **Starting state.** Appendix A, page 267: `BLTCON1` bit 02 is FCI. The
  program's $0002 bit clear leaves it 0, so the fill starts outside.
  (Page 185 prints "FILL_CARRYIN" at bit 3 of `BLTCON1`, which conflicts
  with both page 184, where bit 3 is FILL_OR (IFE), and page 267's own
  table; page 267's bit 02=FCI/bit 03=IFE is what the program follows, and
  what page 184's own running text names.)
- **Reading the row 0100 0000 0100 0000 0100 from the rightmost edge**
  (word4 bit 0) **leftward:**

  | Word | Result | How |
  |---|---|---|
  | word4 | $FF00 | bits 0-7 outside; bit 8 flips the state; the line is kept |
  | word3 | $FFFF | inside |
  | word2 | $01FF | bits 0-7 inside; bit 8 flips back; the line is kept |
  | word1 | $0000 | outside |
  | word0 | $FF00 | as word4 |

  Word1 and word3 (the two marker-free words the package reads) depend
  only on how many markers lie to their right in this scan, so only the
  word order matters for them; the bit order only matters for words 0, 2
  and 4, which the package does not read. The descending run therefore
  reads word1 = $0000, word3 = $FFFF; running the identical blit ascending
  reverses the scan and gives the complementary word1 = $FFFF, word3 =
  $0000 -- not what page 186 describes, and the numbers this package's
  ascending assertion grades against.

### Precondition: PRECONDITION-FILL-DESC, PRECONDITION-FILL-ASC

Each fill program's destination starts holding the sentinel $A5A5 in
every word, not zero, and the harness waits for the blit as page 186's own
caution requires: "The first 'test' of the blitter done bit may not
return the correct result, but the second will" -- one throwaway
`DMACONR` read, then read-and-test in a loop. Before grading word1/word3
against either fill assertion's expected value, a precondition checks they
no longer read back as $A5A5: a blit that never ran, or was read before it
finished, would leave the sentinel in place and must not be mistaken for
a fill result. Carries no claim id.

### Assertion: CONTROL-FILL-DESC (no claim ids)

- Proposition: an area-fill blit of the 5-word/3-marker row, run with the
  blitter's descending-mode bit set and the pointer at word4, reads back
  word1 = $0000, word3 = $FFFF, per the derivation above.
- Program: `prog_fill_desc.s`.
- Observed: `peek_logical(RESULTS+0)`, `peek_logical(RESULTS+2)`.
- Expected: word1 = $0000, word3 = $FFFF.
- This is the control on the same measurement basis as the ascending
  assertion below: it proves the comparator and the fill datapath work
  before the ascending run's mismatch is trusted.

### Assertion (ascending)

- Claims: `6-descending-mode.c10`, `ch6-desc.c13` (page 182, "descending
  mode is also necessary for area filling"); `6-area-fill.c7`,
  `ch6-fill.c6` (page 184, "descending mode must be used"); `6-blitter-
  key-points.c18`, `ch6-keypoints.c20` (page 200, "area fill only works
  correctly in descending mode").
- Proposition: the identical blit, with only the descending bit cleared
  (pointer started at word0 instead) and nothing else changed, does not
  reproduce the descending-mode/control result -- the run that could
  falsify "necessary", and does.
- Program: `prog_fill_asc.s` -- `prog_fill_desc.s` included with the
  descending/ascending symbol flipped (starting pointer and DESC bit
  together), nothing else different.
- Observed: same two words.
- Expected: NOT (word1 = $0000 AND word3 = $FFFF). This is the literal,
  minimal reading of "only works correctly"/"necessary": it commits to the
  fill being wrong in ascending mode, not to which particular wrong
  pattern comes out -- though the derivation above predicts the specific
  complement, word1=$FFFF/word3=$0000, the assertion itself only grades
  the mismatch.

### Fixture: FIXTURE-FILL

`prog_fill_plain.s` runs the same 5-word/3-marker row through a plain,
fill-disabled blit (a minterm copy) into a destination that starts as the
sentinel $A5A5, before any fill assertion is trusted, and stores all 5
result words. Expected: source reproduced exactly (word0/2/4 carry their
marker bit, word1/word3 are zero) -- proof the marker data and the blit
datapath reach `RESULTS` correctly (and that the sentinel was genuinely
overwritten, since $A5A5 matches none of the five expected words) before
fill logic is layered on.

### Mutation

CONTROL-FILL-DESC: `prog_fill_desc_mut.s` moves the middle marker from
word 2 to word 1 (same descending setup otherwise). Word 1 is no longer a
marker-free interior word, so it cannot read back as the clean $0000 the
unchanged expected value calls for -- the check must go red. Recorded as:

    MUTATION CONTROL-FILL-DESC stimulus red middle marker moved from word 2 to word 1

Ascending assertion: `prog_fill_asc_mut.s` runs the ascending
configuration with word0's marker removed (only word2 and word4 remain).
Dropping that crossing shifts the parity so the ascending scan's
word1/word3 happen to land on the same $0000/$FFFF pair the descending
run produces -- the mismatch check must then go red, even though the mode
is still ascending. Recorded as:

    MUTATION 6-descending-mode.c10 stimulus red word0's marker dropped, ascending scan coincides with the descending pattern

FIXTURE-FILL: `prog_fill_plain_mut.s` drops the word4 marker, so the exact
five-word reproduction check must go red. Recorded as:

    MUTATION FIXTURE-FILL stimulus red word4's marker dropped

PRECONDITION-FILL-DESC and PRECONDITION-FILL-ASC carry no dedicated
mutation: not exercised, by a scope decision. Their own stimulus is
identical to CONTROL-FILL-DESC's and the ascending assertion's, whose
mutations (marker moved / dropped) change which fill pattern comes out,
not whether the blit writes at all; a mutation isolating "the blit
completed without ever writing channel D" (for example clearing
`BLTCON0`'s USED bit) is out of scope for this repair.

## Withdrawn: Table 6-2's pipeline assumptions (page 189)

- Claims: `6-pipeline.c43`, `ch6-pipe.c8` ("no fill"); `6-pipeline.c44`,
  `ch6-pipe.c9` ("no competing bus activity"); `6-pipeline.c45`,
  `ch6-pipe.c10` ("three-word blit").
- These six ids are the stated assumptions behind an illustrative pipeline
  table, not values from it: this package's claims do not include Table
  6-2's own cell content (the cycle-by-cycle DMA slot sequence the table
  actually depicts). Without those entries there is no expected value from
  the page to compare against -- a "no fill, no competing bus activity,
  three-word blit" precondition can be reproduced exactly (fill disabled,
  a 3-word blit, no other DMA channel enabled while it runs), but nothing
  in this package's claims says what such a blit's cycle timing should be.
  `ram_log()` timestamps every access with a clk_sys cycle, so the
  observation surface exists; only the table's own numbers are missing.
  Settling these ids needs a claims batch that quotes Table 6-2's entries
  themselves.

## Summary

- 2 assertions carrying 15 claim ids (9 on the drawn line, 6 on the
  ascending fill).
- 6 claim ids withdrawn (Table 6-2's assumptions; no expected value in
  this package's claims).
- 2 fixtures (FIXTURE-LINE, FIXTURE-FILL), 1 control (CONTROL-FILL-DESC,
  no ids), 2 preconditions (sentinel overwritten, descending and
  ascending), 5 stimulus mutations recorded; the two preconditions carry
  no mutation of their own, noted as a scope decision.
- Both CPU cores (fx68k, TG68K) run every program; a verdict is reported
  for each, separately, per assertion.
