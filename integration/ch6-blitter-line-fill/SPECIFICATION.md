# ch6-blitter-line-fill

Built from the spec-stage claims alone (`ch6-blitter-line-fill-spec-stage.jsonl`):
21 claim ids across two unrelated chapter-6 topics plus a third group that
this package cannot settle. Every id below is either carried by an
assertion or withdrawn with a reason; none is silently dropped.

## Topic 1: line-mode dx/dy (page 190-191)

The manual's line-drawing setup is arithmetic a program performs before it
touches any blitter register: given two endpoints, compute the raw
coordinate deltas, then normalize them so the larger becomes "dx" (the
major axis) and the smaller "dy" (the minor axis), swapping if necessary.
This is CPU arithmetic executed by the 68000 core inside `nanomig.v`; it is
tested by running it and reading back what the program stored, the same
mechanism the apparatus check already proves works.

### Assertion LINE-RAW-DELTA

- Claims: `6-line-mode.c39`, `ch6-line.c12` (page 190, "dx = abs(x2 - x1)");
  `6-line-mode.c40`, `ch6-line.c13` (page 190, "dy = abs(y2 - y1)").
- Proposition: for chosen endpoints, the pre-normalization dx equals
  abs(x2-x1) and dy equals abs(y2-y1).
- Program: `prog_line.s` computes dx0, dy0 for endpoints
  (x1,y1)=(10,5), (x2,y2)=(3,40) -- chosen so x2-x1 and y2-y1 are both
  negative, which actually exercises the abs step -- and stores both to
  `RESULTS`.
- Observed: `peek_logical(RESULTS+0)`, `peek_logical(RESULTS+2)`.
- Expected, from the page's formula applied to the chosen endpoints:
  dx0 = abs(3-10) = 7, dy0 = abs(40-5) = 35.

### Assertion LINE-NORMALIZE

- Claims: `6-line-mode.c41`, `ch6-line.c15` (page 191, "dx = max(...)");
  `6-line-mode.c42`, `ch6-line.c16` (page 191, "dy = min(...)");
  `ch6-line.c14` (page 191, "rearranged if necessary so dx is greater than
  dy"). These three phrasings describe one act -- the max/min formulas and
  the "rearrange" prose are the same normalization -- so one test settles
  all five ids: it can only produce a swap by comparing the two raw deltas
  and picking the larger as dx, which is exactly "rearrange so dx > dy" and
  exactly "dx = max(...), dy = min(...)".
- Proposition: normalized dx is the greater of the two raw absolute deltas,
  normalized dy the lesser, with the values swapped if the raw dx was not
  already the greater.
- Program: `prog_line.s`, continuing from LINE-RAW-DELTA's dx0=7, dy0=35 --
  raw dx0 < dy0, so a real swap must happen -- computes dx_n = max(dx0,dy0),
  dy_n = min(dx0,dy0) and stores both to `RESULTS+4`/`RESULTS+6`.
- Observed: `peek_logical(RESULTS+4)`, `peek_logical(RESULTS+6)`.
- Expected: dx_n = max(7,35) = 35, dy_n = min(7,35) = 7.

### Control: LINE-NORMALIZE-NOOP

Same comparator, a second endpoint pair (x1,y1)=(0,0), (x2,y2)=(50,10)
where dx0=50, dy0=10 is already ordered (dx0 > dy0), so normalization must
leave both unchanged: dx_n=50, dy_n=10. This is the "at least one control
case expected to PASS on the same measurement basis" fixture for the line
cluster -- it proves the max/min/store path is not accidentally always
swapping (or always not swapping).

### Fixture

Before any computed value is trusted, `prog_line.s` first stores a fixed
constant ($BEEF) to `RESULTS-2` unconditionally, as its very first
instruction after `lea RESULTS,a1` -- read back and checked first, ahead of
LINE-RAW-DELTA, so a failure there is never explained by
`load_program`/`peek_logical` themselves (already proven by the apparatus
check, re-verified here since this package's program is new).

### Mutation

`prog_line_mut.s` = `prog_line.s` included with different endpoint symbols:
(x1,y1)=(0,0), (x2,y2)=(6,2). Both deltas are already ordered (dx0=6,
dy0=2), so this program's true dx0/dy0/dx_n/dy_n are 6,2,6,2 -- all four
different from the hardcoded expected 7,35,35,7. Loading it (selected by an
environment variable) while keeping the LINE-RAW-DELTA/LINE-NORMALIZE
checks unchanged must turn both red. Recorded as:

    MUTATION LINE-RAW-DELTA stimulus red endpoints changed from (10,5)-(3,40) to (0,0)-(6,2)
    MUTATION LINE-NORMALIZE stimulus red endpoints changed from (10,5)-(3,40) to (0,0)-(6,2)

## Topic 2: area fill requires descending mode (pages 182, 184, 200)

Six ids across three pages state one rule two ways: four say descending
mode is necessary/must be used for area fill (pages 182, 184); two say area
fill only works correctly in descending mode (page 200) -- the same rule
stated as a requirement and as its converse (other modes do not work). That
split maps directly onto two assertions: one shows descending mode
produces the correct fill, the other shows ascending mode (same blit,
otherwise unchanged) does not.

Area fill's own toggle mechanism is not itself among this package's claims
(no claim here states inclusive/exclusive fill semantics), but reading it
off `nanomig/src/minimig-aga/agnus_blitter_fill.v` -- the standard
Amiga XOR/toggle fill, `carry[0] = fci ^ in[0]`, `carry[j] = carry[j-1] ^
in[j]` for the rest, i.e. bit 0 (a word's own LSB) first, bit 15 last, with
the final carry latched as the next word's carry-in -- and
`agnus_blitter.v`'s pointer control (`incptr = ~desc; decptr = desc`,
lines 674-677 etc., one word address step per processed word) fixes,
mechanically, which of two complementary fill patterns a 3-marker row
produces in each mode. This is standard, chip-independent Amiga fill
arithmetic (not a fact about this particular RTL that could be wrong), so
using it to pin down the test's own expected numbers is setup, not
grading the RTL against itself: the grading in FILL-DESC-CORRECT and
FILL-ASC-INCORRECT is the choice of DESC bit against a fixed pair of
observed words, not the carry arithmetic.

- Row: 5 words, 1 line. Word 0, word 2 and word 4 each carry a single set
  bit (the same bit position in each, so no other bit is touched); words 1
  and 3 are all zero going in and carry no marker, so each reads out
  uniform ($0000 or $FFFF, never a mix) after any fill, independent of
  where in a marker word its own set bit sits. Fill starts with carry-in
  (`BLTCON1` bit 2) clear.
- With the pointer started at word 4 and DESC set, the blitter visits
  word4, word3, word2, word1, word0 in that cycle order (one address step
  per word, decrementing). Word 4 is the first word processed
  (`first_word_del`), so its carry-in is the clear `BLTCON1` bit 2; tracing
  the XOR chain word by word: word4 (one set bit) flips the running carry
  on; word3 (no marker) reads back all-on, $FFFF, and passes the carry on
  unchanged; word2 (one set bit) flips the carry back off; word1 (no
  marker) reads back all-off, $0000.
- With the pointer started at word 0 and DESC clear, the visiting order
  reverses (word0..word4, incrementing), and the same trace gives the
  complementary result: word1 = $FFFF, word3 = $0000.

### Assertion FILL-DESC-CORRECT

- Claims: `6-descending-mode.c10`, `ch6-desc.c13` (page 182, "descending
  mode is also necessary for area filling"); `6-area-fill.c7`,
  `ch6-fill.c6` (page 184, "descending mode must be used" for area fill).
- Proposition: an area-fill blit of the 5-word/3-marker row, run with the
  blitter's descending-mode bit set, produces the descending-mode trace
  above.
- Program: `prog_fill_desc.s` places the marker row in chip RAM, points the
  boundary-source pointer at word4 (the row's last word), and sets up an
  area-fill blit over it with line mode disabled and the descending bit
  set, then stores the resulting word1 and word3 to `RESULTS`.
- Observed: `peek_logical(RESULTS+0)`, `peek_logical(RESULTS+2)` (word1,
  word3 of the filled row).
- Expected: word1 = $0000, word3 = $FFFF.

### Assertion FILL-ASC-INCORRECT

- Claims: `6-blitter-key-points.c18`, `ch6-keypoints.c20` (page 200, "area
  fill only works correctly in descending mode").
- Proposition: the identical blit, with only the descending bit cleared
  (pointer started at word0 instead, ascending) and nothing else changed,
  does not reproduce the descending-mode result.
- Program: `prog_fill_asc.s` -- `prog_fill_desc.s` included with the
  descending/ascending symbol flipped (starting pointer and DESC bit
  together), nothing else different.
- Observed: same two words.
- Expected: NOT (word1 = $0000 AND word3 = $FFFF). This is the literal,
  minimal reading of "only works correctly in descending mode": it commits
  to the fill being wrong in ascending mode, not to which particular wrong
  pattern the hardware produces -- that finer claim is not among this
  package's ids. (The trace above predicts the specific complementary
  pattern, word1=$FFFF/word3=$0000; the assertion itself only grades the
  mismatch, so it does not depend on that finer prediction holding
  exactly.)

### Fixture

`prog_fill_plain.s` runs the same 5-word/3-marker row through a plain,
fill-disabled blit (a minterm copy) before any fill assertion is trusted,
and stores all 5 result words. Expected: source reproduced exactly
(word0/2/4 carry their marker bit, word1/word3 are zero) -- proof the
marker data and the blit datapath reach `RESULTS` correctly before fill
logic is layered on.

### Mutation

FILL-DESC-CORRECT: `prog_fill_desc_mut.s` moves the middle marker from
word 2 to word 1 (same descending setup otherwise). Word 1 is no longer a
marker-free interior word, so it cannot read back as the clean $0000 the
unchanged expected value calls for -- the check must go red. Recorded as:

    MUTATION FILL-DESC-CORRECT stimulus red middle marker moved from word 2 to word 1

FILL-ASC-INCORRECT: this assertion's own expected outcome (mismatch) is
only known once step 2 shows whether ascending mode is actually green or
red on this RTL. The mutation recorded in the test file's header will
follow whichever the brief prescribes for that outcome -- a stimulus change
that flips a green FILL-ASC-INCORRECT red, or, if it starts red, a
different proven-live input through the same word1/word3 comparator that
goes green (e.g. reusing the descending configuration through the same
code path) -- decided and recorded once the real result is known.

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

- 4 assertions carrying 15 claim ids (9 line-mode, 6 area-fill).
- 6 claim ids withdrawn (Table 6-2's assumptions; no expected value in this
  package's claims).
- 2 fixtures (a known constant for the line program, a known plain-copy
  blit result for the fill program), 1 control case (no-swap endpoints),
  2 stimulus mutations recorded, 1 mutation deferred pending step 2's
  result for FILL-ASC-INCORRECT.
- Both CPU cores (fx68k, TG68K) run every program; a verdict is reported
  for each, separately, per assertion.
