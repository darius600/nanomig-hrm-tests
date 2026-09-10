// Test purpose: Where display data fetch starts and stops and how many words
//   come out of a line - DDFSTRT/DDFSTOP, their 4-pixel/CCK bit granularity,
//   the $18/$D8 hardware limits, the extra word a horizontal scroll costs,
//   the modulo applied at end-of-line, and the per-line word/byte counts for
//   low and high resolution (including the printed manual's own arithmetic
//   error in Table 3-8's PAL byte counts, which this package measures rather
//   than asserts).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 40, 41, 42, 45, 49, 51, 53, 54, 57, 59, 60, 61, 62, 63, 72, 73, 74,
//   75, 76, 80, 81, 82, 83, 84, 85, 91, 272, 277, 290.
// Non-goals: pixel *colour* (p.42, p.49 "16 colors") - Denise's palette, not
//   observable at this DMA-count boundary; the blitter's maximum draw area
//   (p.76) - agnus_blitter's boundary, not this one; sprite-disable side
//   effects of an early DDFSTRT (p.80, p.82, p.83) - agnus_spritedma's
//   boundary; the Copper's ability to reset pointers during vblank (p.80) -
//   agnus_copper's boundary and a software-usage note, not a DMA-count fact.
//   DIWSTRT/DIWSTOP *horizontal* placement/centering (p.57, p.59) and the
//   interlaced-field line-3 stride example (p.62) are held PENDING below:
//   this package's given boundary evidence names only DDFSTRT_REG/DDFSTOP_REG,
//   not DIWSTRT/DIWSTOP, and the one DIWSTOP fact already measured on this
//   fixture (V $F2..$F6 -> 198..202 lines) is DIWSTOP's VERTICAL half, not its
//   horizontal one - whether horizontal DIW or interlace reach this module's
//   ports at all is an RTL question, answered below the RTL-open line.
//
// BOUNDARY (from the build brief, not yet from opening the file):
//   nanomig/src/minimig-aga/agnus_bitplanedma.v:55-56 (DDFSTRT_REG 9h092,
//   DDFSTOP_REG 9h094), write decode l.247-259 (ddfstrt[8:2] <= data_in[7:1]),
//   position compare l.336-354, hard limits $18/$D8 l.356-374, ddfena
//   l.407-414.
//
//   The write-decode fact alone (ddfstrt[8:2] <= data_in[7:1]) already
//   settles the "4-pixel resolution" / bit-field-width family of claims
//   (G7, G17 below) without opening the RTL further: data_in bit 0 is never
//   read (bits 1-0 of the stored field do not exist), and the stored field's
//   own LSB (bit 2) comes from data_in bit 1, not bit 0 - so two DDFSTRT
//   writes differing only in their raw value's bit 0 must be indistinguishable
//   to this module, and two differing by exactly 2 in raw value must differ
//   in stored position by exactly one field step. That is the concrete,
//   measurable shape "4-pixel resolution" and "pixel 0 is position 0, pixel 4
//   is position 1" take at this boundary.
//
// ALREADY MEASURED on this fixture (d_bpldma, 10.09.2026 - not repeated here):
//   NTSC 320x200 lores, one bitplane: DDFSTRT=$38, DDFSTOP=$D0, BPU=1 gives
//   exactly 20 dma cycles/line, reg_address_out=$110 each cycle, address_out
//   stepping 2 bytes/cycle, 8,000 bytes total across 200 lines, addresses
//   $0..$1F3E contiguous step 2 (20 words/line x 200 lines x 2 bytes/word).
//   With DDFSTRT=$00/DDFSTOP=$FE (outside the $18/$D8 hardware limits), a
//   line still yields at most 25 lores fetches - the hardware clamp already
//   visible in a measurement, not yet tied to a named assertion.
//   DIWSTOP V $F2..$F6 -> 198/199/200/201/202 active lines.
//   BPL1MOD 0/40/80 -> line-to-line address step 40/80/120 bytes.
//
// CLAIMS (verbatim quote + restatement from
// C:\Users\x\AppData\Local\Temp\claude\d--code\674b5d99-c2c3-47f0-95e4-93eefcde2be3\scratchpad\build\ch3-ddf-fetch-window\claims.jsonl,
// all 239 records classify.status == "testable"). Grouped by the proposition
// each supports, not by page order; ids restating the same proposition
// (different reader, different wording, different printed occurrence of the
// same table row) are grouped and will be cited together on whichever
// assertion(s) test that proposition.
//
// -- G1: standard low-resolution playfield = 320 pixels = 20 words/line
//    (p.40, 41, 45, 49, 63) --
//   3-video.c12 (opus, p40): "picture element, the smallest addressable part
//     of a screen display." -> "A pixel is the smallest addressable part of a
//     screen display."
//   3-video.c16 (sol, p40): same idea, full sentence -> "A pixel is the
//     smallest addressable part of the display."
//   3-res.c01 (opus, p41): "In low resolution mode, the normal playfield has
//     a width of 320 pixels." -> same.
//   3-res.f01 (opus, p41, figure): Figure 3-2 left screen, "320 Pixels"
//     double arrow, caption "In normal resolution mode, 320 pixels fill a
//     horizontal line."
//   3-video.c17 (sol, p41, figure): same figure, same reading.
//   3-video.c19 (sol, p41): "Horizontal resolution can be adjusted to operate
//     in low resolution or high resolution mode." (mode-existence umbrella;
//     cited on both G1 and G2)
//   3-video.c21 (sol, p41): "In low resolution mode, the normal playfield has
//     a width of 320 pixels."
//   3-basic-size.c1 (sol, p45): "...you can use a width of either 320 pixels
//     or 640 pixels, depending upon the resolution..." -> low-res half.
//   3-basic.c01 (opus, p45): same quote, low-res half.
//   3-res.c07 (opus, p49): "Low resolution mode provides 320 pixels for each
//     horizontal line."
//   3-resolution.c1 (sol, p49): same idea, full sentence.
//   3-sum.c03 (opus, p63): "320 pixels in each horizontal line." (chapter
//     summary restatement)
//
// -- G2: standard high-resolution playfield = 640 pixels = 40 words/line,
//    same physical width as G1's 320 (p.41, 45, 49, 62, 63, 75) --
//   3-res.c02 (opus, p41): "...320 pixels... High resolution mode gives finer
//     horizontal resolution -- 640 pixels in the same physical display area."
//   3-video.c18 (sol, p41, figure): Figure 3-2 right screen, "640 pixels
//     fill a horizontal line in high-resolution mode."
//   3-video.c22 (sol, p41): "High resolution mode gives finer horizontal
//     resolution -- 640 pixels in the same physical display area."
//   3-basic-size.c2 (sol, p45): high-res half of the 320/640 sentence.
//   3-basic.c02 (opus, p45): same, high-res half.
//   3-res.c08 (opus, p49): "high resolution mode, which provides 640 pixels
//     for each horizontal line."
//   3-res.c09 (opus, p49): "If you define an object in low resolution mode
//     and then display it in high resolution mode, the object will be only
//     half as wide." (the 320:640 physical-width identity, from the other
//     direction)
//   3-resolution.c2 (sol, p49): same 640-pixel sentence as 3-res.c08.
//   3-resolution.c3 (sol, p49): same half-width sentence as 3-res.c09.
//   3-sum.c05 (opus, p63): "640 pixels in each horizontal line." (summary)
//   3-ddf.c18 (opus, p62): "you need to fetch 80 bytes for each line,
//     instead of 40." (hires basic playfield byte count, one bitplane)
//   3-modulo.c19 (sol, p62): same sentence.
//   3-big.c11 (opus, p75): "in high resolution mode, you need to fetch twice
//     as many bytes as in low resolution mode."
//   3-big.c12 (opus, p75): "For a normal-sized display, you fetch 80 bytes
//     for each horizontal line instead of 40."
//   3-large.c18 (sol, p75): same "twice as many bytes" sentence.
//   3-large.c19 (sol, p75): same "80 bytes...instead of 40" sentence.
//
// -- NOT ASSERTED (color/palette, not this boundary) --
//   3-bpl.c02 (opus, p42): "...whether a single bit is on or off. If the bit
//     is 0, the pixel is one user-defined color; if the bit is 1, the pixel
//     is another..." -> bit-to-colour mapping is Denise's palette lookup, not
//     observable on agnus_bitplanedma's ports (which carry fetched data
//     words, not resolved colours).
//   3-res.c14 (opus, p49): "...up to 16 colors." -> same reason.
//   3-resolution.c7 (sol, p49): same "16 colors" sentence.
//
// -- G3: high-resolution bitplane-count ceiling (p.49) - PENDING RTL --
//   3-res.c13 (opus, p49): "Note that in high resolution mode, you can have
//     up to four bitplanes in [the playfield]."
//   3-resolution.c6 (sol, p49): same sentence.
//   Whether agnus_bitplanedma itself enforces a BPU<=4 ceiling in hires mode,
//   or whether this is a software convention the RTL does not check, is
//   answered after the RTL is open. If the RTL enforces nothing, these two
//   ids move to the "could not assert" list, not to a green assertion built
//   on an assumption.
//
// -- G6: low-resolution byte counts, 40 bytes/line, 8,000 bytes/bitplane,
//    Figure 3-7's 40-byte line stride (p.51, 53, 54) --
//   3-resolution.c23 (sol, p51): "...high resolution or interlaced playfields
//     contain more data and require larger bitplanes." (resolution scales
//     required bitplane memory - the qualitative form of G1/G2's 40 vs 80
//     byte facts)
//   3-mem.c07 (opus, p53): "each line of the bitplane requires 40 bytes (320
//     bits divided by 8 bits [per byte = 40])."
//   3-mem.c08 (opus, p53): "...Multiply the 200 lines times 40 bytes per line
//     to get [8,000 bytes per bitplane]."
//   3-memory.c14 (sol, p53): full worked sentence, 320px/200 lines -> 40
//     bytes/line.
//   3-memory.c15 (sol, p53): full worked sentence, 200 lines x 40 bytes ->
//     8,000 bytes.
//   3-memory.c19 (sol, p53): "Figure 3-7 shows an 8,000-byte memory area
//     organized as 200 lines of 40 bytes each..."
//   3-mem.f02 (opus, p54, figure): Figure 3-7 second row "Mem. location N+40"
//     -> next display line starts 40 bytes after the previous one.
//
// -- PENDING RTL: DIWSTRT/DIWSTOP horizontal placement (p.57, 59) --
//   3-diw.c06 (opus, p57): "[resolution of horizontal] start and stop is one
//     low resolution pixel."
//   3-window.c18 (sol, p57): "A starting position of ($81,$2C) centers a
//     normal size display, leaving a border of eight low resolution pixels
//     around the display window."
//   3-window.c9 (sol, p57): "The resolution of horizontal start and stop is
//     one low resolution pixel."
//   3-diw.c24 (opus, p59): "...the starting position, it is interpreted in
//     low resolution, non-interlaced [mode]." (DIWSTOP encoding convention)
//   See the Non-goals note above: held pending whether DIWSTRT/DIWSTOP reach
//   this module's ports at all, beyond the DIWSTOP V (vertical) field already
//   measured for line-enable count.
//
// -- G7: DDFSTRT/DDFSTOP have 4-pixel resolution, unlike DIW's 1-pixel
//    (p.60), and the "programming resolution of 16 pixels (8 clocks low-res
//    / 4 clocks high-res)" passage on the same page, cited as context for the
//    same field-width fact --
//   3-ddf.c01 (opus, p60): "[the] data-fetch registers have a four-pixel
//     resolution (unlike the display [window registers])."
//   3-fetch.c1 (sol, p60): "The data-fetch registers have a four-pixel
//     resolution (unlike the display window registers, which have a
//     one-pixel resolution)."
//   3-ddf.c03 (opus, p60): "[each position] specified is four pixels from
//     the last one. Pixel 0 is position 0; pixel 4 [is position 1]."
//   3-fetch.c3 (sol, p60): "Each position specified is four pixels from the
//     last one."
//   3-fetch.c4 (sol, p60): "Pixel 0 is position 0; pixel 4 is position 1, and
//     so on."
//   3-fetch.c5 (sol, p60): same sentence, cited for the "position 1 = pixel
//     4" half.
//   3-ddf.c06 (opus, p60): "[rounds] to a programming resolution of 16 pixels
//     (8 clocks in low resolution mode,"
//   3-ddf.c07 (opus, p60): "4 clocks in high resolution mode)." - cited here
//     as context; exactly which measurable fact this "16 pixel" figure
//     describes (as opposed to the 4-pixel field-storage granularity G7
//     actually asserts) is confirmed, not assumed, once the RTL is read.
//   3-fetch.c8 (sol, p60): "there is a difference between the value of
//     window start and data-fetch start of 4.5 color clocks." -> the DISPUTED
//     number, see below; not asserted here, cited only as the sentence the
//     conflict concerns.
//
// -- DISPUTED, recorded as a measurement, not an assertion (p.60): the
//    printed prose's "4.5 color clocks" window-start-to-data-fetch-start
//    offset contradicts the worked example immediately below it, which
//    subtracts 8.5 for the low-resolution case ($81/2 - 8.5 = $38) and 4.5
//    only for the high-resolution case. Both $38 (lores) and $3C (hires) are
//    independently, un-disputedly given elsewhere on the same page (G7b
//    below) and match the pre-supplied measurement (DDFSTRT=$38 lores). The
//    undisputed fact is those two standard DDFSTRT values themselves, tested
//    directly in G7b - not the subtraction formula that supposedly produces
//    them from DIWSTRT, which is where the printed 4.5-vs-8.5 contradiction
//    actually lives. --
//   3-ddf.c09 (opus, p60): "there is a difference between the value of
//     window start and data-fetch start of 4.5 color clocks." Same disputed
//     figure as 3-fetch.c8.
//   3-ddf.c10 (opus, p60): the id the classifier itself marked CONFLICT:
//     "start of 4.5 color clocks." restatement records the 4.5-vs-8.5
//     contradiction directly; source_defines: "conflict: page 60 prose gives
//     4.5 colour clocks, the worked example on the same page subtracts 8.5
//     for low resolution".
//   3-ddf.c08 (opus, p60): "the first data fetch before it can actually
//     display the data. As a result, [there is a difference...]" - the
//     qualitative lead-in ("hardware needs time after the first fetch before
//     it can display"), cited as context for the disputed sentence it
//     introduces, not asserted as its own numeric fact.
//   3-ddf.c04 (opus, p60): "The data-fetch start and display window starting
//     positions interact with [each other]." - the general relationship the
//     disputed formula quantifies; too vague to test as its own boolean, and
//     not re-tested once the disputed number underneath it is set aside.
//   3-fetch.c6 (sol, p60): "The data-fetch start and display window starting
//     positions interact with each other." Same as 3-ddf.c04.
//
// -- G7b: standard DDFSTRT/DDFSTOP register values (p.60) - the undisputed
//    facts the formula above was trying to derive, given directly --
//   3-ddf.c11 (opus, p60): "The normal low resolution DDFSTRT is ($0038)."
//   3-ddf.c12 (opus, p60): "The normal high resolution DDFSTRT is ($003C)."
//   3-ddf.c16 (opus, p60): "The normal low resolution DDFSTOP is ($00D0)."
//   3-ddf.c17 (opus, p60): "[The normal high resolution] DDFSTOP is
//     ($00D4)."
//   3-fetch.c9 (sol, p60): "The normal low resolution DDFSTRT is ($0038)."
//   3-fetch.c10 (sol, p60): "The normal high resolution DDFSTRT is
//     ($003C)."
//   3-fetch.c14 (sol, p60): "The normal low resolution DDFSTOP is
//     ($00D0)."
//   3-fetch.c15 (sol, p60): "The normal high resolution DDFSTOP is
//     ($00D4)."
//
// -- G7c: DDFSTRT/DDFSTOP/word-count formula (p.60) - independently
//    testable across MORE than the one pre-measured standard case, since it
//    relates three quantities entirely within the DDF domain (no DIW
//    needed) --
//   3-ddf.c13 (opus, p60): "Recall that the hardware resolution of display
//     window start and stop is twice the hardware resolution of data
//     fetch:" (lead-in to the formula; the "twice" comparison itself is a
//     DIW-vs-DDF field-width statement, held pending alongside the DIW
//     cluster above, not re-asserted from the formula's own test)
//   3-ddf.c14 (opus, p60): "DDFSTRT = DDFSTOP - (8 * (word count - 1)) for
//     low resolution"
//   3-ddf.c15 (opus, p60): "DDFSTRT = DDFSTOP - (4 * (word count - 2)) for
//     high resolution"
//   3-fetch.c11 (sol, p60): "the hardware resolution of display window
//     start/stop is twice the hardware resolution of data fetch." (same
//     lead-in as 3-ddf.c13)
//   3-fetch.c12 (sol, p60): low-resolution formula, restated in words.
//   3-fetch.c13 (sol, p60): high-resolution formula, restated in words.
//
// -- G19: end-of-line condition and modulo (p.61, 81) - already covered by
//    the pre-supplied BPL1MOD 0/40/80 -> 40/80/120 measurement --
//   3-mod.c04 (opus, p61): "the end-of-line condition is reached (defined by
//     the data-fetch register, [DDFSTOP])"
//   3-mod.f02 (opus, p61, figure): Figure 3-10 note, "Screen data fetch stops
//     (DDFSTOP) for each horizontal line after the last word on the line has
//     been fetched." - DDFSTOP terminates AFTER the last word, not before.
//   3-modulo.c5 (sol, p61): "When the end-of-line condition is reached
//     (defined by [[DDFSTOP]]) the modulo is added to the bitplane
//     pointers, adjusting the pointer to the first word to be fetched for
//     the next horizontal line."
//   3-modulo.c9 (sol, p61, figure): same Figure 3-10 reading as 3-mod.f02.
//   3-ptr.c09 (opus, p61): "the data fetch begins, the pointers are
//     continuously incremented to point [to the next word]."
//   3-vscroll.c9 (sol, p81): "For a normal-sized, low resolution display in
//     which the modulo is 0, the pointers would be incremented by 40 bytes
//     each time." - the zero-modulo case of the same fact, already the
//     BPL1MOD=0 -> 40-byte-step point on the pre-supplied measurement curve.
//
// -- PENDING RTL: interlaced-field line stride (p.62) --
//   3-mod.c15 (opus, p62): Figure/example, "Line 3 START+80" - in a
//     320-pixel-wide interlaced picture, successive displayed lines of one
//     field are 80 bytes apart (double the non-interlaced 40-byte stride).
//   Whether agnus_bitplanedma's own ports carry interlace/field information,
//   or whether interlaced addressing is assembled one level up (in `agnus`,
//   from this module's plain per-line address plus a field-parity offset),
//   is an RTL question.
//
// -- G10: dual-playfield scrolling shares one DDFSTRT/DDFSTOP pair; scrolling
//    one playfield costs one extra word (lores) or two (hires); holding the
//    other stationary needs its modulo offset by -2 (lores) / -4 (hires)
//    (p.72) --
//   3-dual-scroll.c1 (sol, p72): "...you must fetch one word more than the
//     width of the playfield you are trying to scroll (two words more in
//     high resolution mode)..." -> low-res half.
//   3-dual-scroll.c2 (sol, p72): same sentence -> high-res half.
//   3-dual-scroll.c3 (sol, p72): "Only one data-fetch start register...[is]
//     available, and [is] shared by both playfields." -> start register.
//   3-dual-scroll.c4 (sol, p72): same sentence -> stop register.
//   3-dual-scroll.c6 (sol, p72): "In low resolution mode, you adjust the
//     pointers by -2 and the modulo by -2."
//   3-dual-scroll.c8 (sol, p72): "In high resolution mode, you adjust the
//     pointers by -4 and the modulo by -4."
//   3-dual.c34 (opus, p72): full paragraph, low-res "+1 word" half.
//   3-dual.c36 (opus, p72): "There is only one data-fetch start register in
//     the machine."
//   3-dual.c37 (opus, p72): "There is only one data-fetch stop register, and
//     both data-fetch registers are shared by both playfields."
//   3-dual.c40 (opus, p72): "Low resolution: the non-scrolled playfield's
//     modulo is adjusted by -2."
//   3-dual.c42 (opus, p72): "High resolution: the non-scrolled playfield's
//     modulo is adjusted by -4."
//
// -- G11: wider-playfield-than-display modulo example, Figures 3-14/3-15
//    (p.73, 74) --
//   3-big.f01 (opus, p73, figure): Figure 3-14, 80-byte RAM line, left half
//     (40 bytes) displayed.
//   3-large.c5 (sol, p73, figure): same figure, same reading.
//   3-big.f02 (opus, p74, figure): Figure 3-15, "Data Fetch for the First
//     Line When Modulo = 40" - locations START..START+38, 20 words, even
//     under a 40-byte modulo.
//   3-large.c6 (sol, p74): "Because 40 bytes are to be fetched for each
//     line, the data fetch for line 1 is as shown in Figure 3-15."
//   3-large.c8 (sol, p74, figure): same figure - line 1 stops after
//     START+38.
//
// -- G13: DDFSTRT/DDFSTOP define the fetch window; total playfield memory
//    formula bytes/line x lines x bitplanes (p.76) --
//   3-large.c20 (sol, p76): "The data-fetch registers specify the beginning
//     and end positions for data placement on each horizontal line of the
//     display." (the package's own foundational proposition)
//   3-large.c21 (sol, p76): "bytes per line * lines in playfield * # of
//     bitplanes"
//   3-large.c23 (sol, p76): "80 * 200 * 2 = 32,000 bytes of memory" - a
//     2-bitplane, 200-line, 80-byte/line (hires) total, checkable directly
//     against a real 2-bitplane hires run's total fetched byte count.
//
// -- NOT ASSERTED (agnus_blitter's boundary, not agnus_bitplanedma's) --
//   3-big.c22 (opus, p76): "...the largest area the blitter can draw in is
//     increased to 16368 by 16384 pixels..." -> blitter draw-area limit,
//     unrelated module.
//   3-large.c26 (sol, p76): "With the original chip set the largest area the
//     blitter can draw in is 1008 by 1024." -> same reason.
//   3-large.c27 (sol, p76): "With the Enhanced Chip Set (ECS), the largest
//     area the blitter can draw in is increased to 16368 by 16384 pixels."
//     -> same reason.
//
// -- G14: hardware limits (Table 3-14, p.80) - DDFSTRT leftmost $18, DDFSTOP
//    rightmost $D8, max words 25 (lores) / 49 (hires), the standard values
//    repeated, max display pixels 368 (lores; table gives no hires figure
//    for this row) --
//   3-max-window.c10 (sol, p80): "the hardware sets a rightmost limit to
//     DDFSTOP of ($D8) and a leftmost limit to DDFSTRT of ($18)." -> DDFSTOP
//     half.
//   3-max-window.c11 (sol, p80): same sentence -> DDFSTRT half.
//   3-max-window.c12 (sol, p80): "This gives a maximum of 25 words fetched in
//     low resolution mode."
//   3-max-window.c13 (sol, p80): "In high resolution mode the maximum here
//     is 49 words, because the rightmost limit remains ($D8) and only one
//     word is fetched at this limit."
//   3-max-window.c14 (sol, p80): same sentence, cited for the "one word
//     fetched at the limit" half.
//   3-max-window.c19 (sol, p80): "DDFSTRT (standard) $0038 $003C" -> low-res
//     cell.
//   3-max-window.c20 (sol, p80): same table row -> high-res cell.
//   3-max-window.c21 (sol, p80): "DDFSTOP (standard) $00D0 $00D4" -> low-res
//     cell.
//   3-max-window.c22 (sol, p80): same table row -> high-res cell.
//   3-max-window.c23 (sol, p80): "DDFSTRT (hw limits) $0018 $0018" -> low-res
//     cell.
//   3-max-window.c24 (sol, p80): same table row -> high-res cell.
//   3-max-window.c25 (sol, p80): "DDFSTOP (hw limits) $00D8 $00D8" -> low-res
//     cell.
//   3-max-window.c26 (sol, p80): same table row -> high-res cell.
//   (3-max-window.c19..c26: Table 3-14 rows - DDFSTRT standard $0038/$003C,
//     DDFSTOP standard $00D0/$00D4, DDFSTRT hw limit $0018/$0018, DDFSTOP hw
//     limit $00D8/$00D8, eight ids, one per low-res/high-res cell of four
//     rows.)
//   3-max-window.c29 (sol, p80): "max display pixels 368 (low res)" - Table
//     3-14's own max-pixel row; 368 = 23 words * 16 px, i.e. one word short
//     of the 25-word absolute maximum (an asymmetry between "max words
//     fetched" and "max display pixels" the table itself states, checked as
//     its own fact rather than assumed to equal 25*16).
//   3-max.c03 (opus, p80): "...sets a rightmost limit to DDFSTOP of ($D8)
//     and a leftmost limit to [DDFSTRT]" -> DDFSTOP half, opus's reading.
//   3-max.c04 (opus, p80): "DDFSTRT of ($18). This gives a maximum of 25
//     words fetched in low [resolution mode]." -> DDFSTRT half plus the
//     25-word figure.
//   3-max.c05 (opus, p80): "resolution mode. In high resolution mode the
//     maximum here is 49 words," -> low-res 25-word figure, opus's reading.
//   3-max.c06 (opus, p80): same quote -> high-res 49-word figure.
//   3-max.c07 (opus, p80): "because the rightmost limit remains ($D8) and
//     only one word is fetched at" -> hires one-word-at-limit detail, opus's
//     reading.
//   3-t14.c01, 3-t14.c02, 3-t14.c03, 3-t14.c04, 3-t14.c05, 3-t14.c06,
//     3-t14.c07, 3-t14.c08, 3-t14.c09, 3-t14.c10, 3-t14.c11 (sol, p80):
//     Table 3-14 read a second time, same eleven cells as
//     3-max-window.c19/c20/c21/c22/c23/c24/c25/c26/c29 above plus the two
//     "max words fetched 25/49" cells (DDFSTRT standard/hw-limit x2, DDFSTOP
//     standard/hw-limit x2, max words fetched x2, max display pixels x1) -
//     cited alongside their 3-max-window counterparts on the same
//     assertions.
//
// -- NOT ASSERTED (sprite DMA is agnus_spritedma's boundary, not this
//    module's) --
//   3-max-window.c18 (sol, p80): "...using a data-fetch start earlier than
//     ($38) will disable some sprites." -> sprite side-effect.
//   3-max.c12 (opus, p80): "data-fetch start earlier than ($38) will disable
//     some sprites." -> same reason.
//   3-hscr.c10 (opus, p82): "...sooner (DDFSTRT = $0030). Incidentally, this
//     will disable sprite 7." -> same reason.
//   3-hscroll.c12 (sol, p82): "Incidentally, this will disable sprite 7." ->
//     same reason.
//   3-hscr.c17 (opus, p83): "NOTE: Fetching an extra word for scrolling will
//     disable some sprites." -> same reason.
//   3-hscroll.c17 (sol, p83): same sentence -> same reason.
//
// -- NOT ASSERTED (Copper usage note, not a DMA-count fact) --
//   3-scroll-copper.c2 (sol, p80): "For either type of scrolling, resetting
//     of pointers or data-fetch registers can be handled by the Copper
//     during the vertical blanking interval." -> describes a SOFTWARE
//     technique (use the Copper), not a hardware behaviour of
//     agnus_bitplanedma itself; nothing on this module's ports distinguishes
//     "the Copper wrote this register" from "the CPU wrote this register".
//
// -- G16: horizontal-scroll extra word (p.80, 82, 83, 84, 85) - normal
//    DDFSTRT=$38 becomes $30 (one word/8 CCK earlier) when scrolling,
//    DDFSTOP unchanged, fetch grows from 40 to 42 bytes (20 to 21 words),
//    delay range 0-15 pixels reveals the early word, modulo drops by 2 --
//   3-scr.c04 (opus, p80): "...you must arrange to fetch one more word of
//     data [for each display line]"
//   3-scr.c05 (opus, p80): "for each display line and delay the display of
//     this data."
//   3-scroll.c5 (sol, p80): "Horizontal scrolling works differently from
//     vertical scrolling -- you must arrange to fetch one more word of data
//     for each display line and delay the display of this data." (extra-word
//     half)
//   3-scroll.c6 (sol, p80): same sentence (delay half).
//   3-hscr.c02 (opus, p82): "of delay in pixels. Delay means that an extra
//     word of data is fetched but [not immediately displayed]."
//   3-hscr.c03 (opus, p82): "not immediately displayed. The extra word is
//     placed just to the left of the window's leftmost edge and before
//     normal data fetch."
//   3-hscr.c04 (opus, p82): "shifts to the right, the bits in this extra
//     word appear on-screen at the left-hand side of the window..."
//   3-hscr.c08 (opus, p82): "The normal data-fetch start for non-scrolled
//     displays is ($38)."
//   3-hscr.c09 (opus, p82): "...If horizontal scrolling is desired, then the
//     data fetch must start one word sooner (DDFSTRT = $0030)."
//   3-hscr.c11 (opus, p82): "DDFSTOP remains unchanged. Remember that the
//     settings of the data-fetch [registers affect both playfields]."
//   3-hscr.c12 (opus, p82): "registers affect both playfields."
//   3-hscr.c15 (opus, p82): "scrolling requires a data fetch of two extra
//     bytes, the data for each line [will be 42 bytes long]."
//   3-hscr.c16 (opus, p82): "will be 42 bytes long."
//   3-hscroll.c1 (sol, p82): "Delay means that an extra word of data is
//     fetched but not immediately displayed."
//   3-hscroll.c2 (sol, p82): "The extra word is placed just to the left of
//     the window's leftmost edge and before normal data fetch."
//   3-hscroll.c3 (sol, p82): "As the display shifts to the right, the bits
//     in this extra word appear on-screen at the left-hand side of the
//     window as bits on the right-hand side disappear off-screen."
//   3-hscroll.c5 (sol, p82): "For each pixel of delay, the on-screen data
//     shifts one pixel to the right each display field."
//   3-hscroll.c7 (sol, p82): "You can have up to 15 pixels of delay."
//   3-hscroll.c10 (sol, p82): "...the data fetch must start one word sooner
//     (DDFSTRT = $0030)."
//   3-hscroll.c11 (sol, p82): same sentence, cited for the $0030 value
//     itself.
//   3-hscroll.c13 (sol, p82): "DDFSTOP remains unchanged."
//   3-hscroll.c14 (sol, p82): "Remember that the settings of the data-fetch
//     registers affect both playfields."
//   3-hscroll.c16 (sol, p82): "Because horizontal scrolling requires a data
//     fetch of two extra bytes, the data for each line will be 42 bytes
//     long."
//   3-hscr.f01 (opus, p83, figure): Figure 3-24, "Data Fetch 21 words" vs
//     "Display Window 320 bits (20 words)".
//   3-hscr.f02 (opus, p83, figure): same figure, "16 bits (1 word)" gap
//     between data-fetch start and display-window start.
//   3-hscr.f03 (opus, p83, figure): same figure, "0-15 bits of delay will
//     cause the system to show the early-fetched word."
//   3-hscr.f05 (opus, p83, figure): same figure, "This data is displayed if
//     scroll = 0."
//   3-hscr.f06 (opus, p83, figure): same figure, "This data is displayed if
//     scroll = 15."
//   3-hscr.f08 (opus, p83, figure): same figure, "Data Fetch start" ticked
//     LEFT of "Display Window start".
//   3-hscroll.c18 (sol, p83, figure): same figure, 320-bit/20-word window
//     served by a 21-word fetch.
//   3-hscroll.c19 (sol, p83, figure): same figure, 0-15 bits of delay.
//   3-hscr.f10 (opus, p84, figure): Figure 3-26, "Data for line 1" locations
//     START..START+40 (21 words, one more than unscrolled's START+38).
//   3-hscroll.c23 (sol, p84, figure): same figure, START..START+40, 42
//     bytes.
//   3-hscroll.c26 (sol, p84, figure): Figure 3-27, line-2 locations
//     START+80..START+120.
//   3-hscr.c27 (opus, p85): "Fetch one extra word per horizontal line and
//     start it 16 pixels [before the normal (unscrolled) data-fetch
//     start]."
//   3-hscr.c28 (opus, p85): "before the normal (unscrolled) data-fetch
//     start."
//   3-hscr.c29 (opus, p85): "The modulo is two counts less than when there
//     is no scrolling."
//   3-hscroll.c34 (sol, p85): "Fetch one extra word per horizontal line
//     [...]"
//   3-hscroll.c35 (sol, p85): "...start it 16 pixels before the normal
//     (unscrolled) data-fetch start."
//   3-hscroll.c36 (sol, p85): "The modulo is two counts less than when there
//     is no scrolling."
//
// -- G17: DDFSTRT/DDFSTOP register bit-field structure and register-table
//    entries (p.91, 277, 290) - already settled at the write-decode level by
//    the boundary evidence's own ddfstrt[8:2] <= data_in[7:1] fact, cited
//    here as the manual-side statement of the same bit layout --
//   3-rddf.c01 (opus, p91): "DDFSTRT - Data-fetch Start"
//   3-rddf.c02 (opus, p91): "...Bits 7-2 - pixel position H8-H3 (bit H3 only
//     respected in Hires Mode.)" (DDFSTRT instance)
//   3-rddf.c03 (opus, p91): same block, "bit H3 only respected in Hires
//     Mode" (DDFSTRT instance)
//   3-rddf.c04 (opus, p91): "DDFSTOP - Data-fetch Stop"
//   3-rddf.c05 (opus, p91): same block (DDFSTOP instance, bits 7-2/H3-hires)
//   3-rddf.c06 (opus, p91): "Bits 15-8 - not used" (both instances)
//   3-rddf.c07 (opus, p91): "Bits 1-0 - not used" (both instances)
//   3-regsum-ddfstop.c1 (sol, p91): "DDFSTOP controls the ending position for
//     data fetch."
//   3-regsum-ddfstop.c2 (sol, p91): "DDFSTOP bits 15-8 are unused on OCS."
//   3-regsum-ddfstop.c3 (sol, p91): "DDFSTOP bits 7-2 hold horizontal
//     pixel-position bits H8-H3."
//   3-regsum-ddfstop.c4 (sol, p91): "DDFSTOP position bit H3 affects
//     fetching only in high-resolution mode."
//   3-regsum-ddfstop.c5 (sol, p91): "DDFSTOP bits 1-0 are unused."
//   (3-regsum-ddfstop.c1..c5: DDFSTOP register summary - controls ending
//     position, bits 15-8 unused, bits 7-2 = H8-H3, H3 hires-only, bits 1-0
//     unused, five ids, one per field.)
//   3-regsum-ddfstrt.c1 (sol, p91): "DDFSTRT controls the beginning position
//     for data fetch."
//   3-regsum-ddfstrt.c2 (sol, p91): "DDFSTRT bits 15-8 are unused on OCS."
//   3-regsum-ddfstrt.c3 (sol, p91): "DDFSTRT bits 7-2 hold horizontal
//     pixel-position bits H8-H3."
//   3-regsum-ddfstrt.c4 (sol, p91): "DDFSTRT position bit H3 affects
//     fetching only in high-resolution mode."
//   3-regsum-ddfstrt.c5 (sol, p91): "DDFSTRT bits 1-0 are unused."
//   (3-regsum-ddfstrt.c1..c5: same five fields as 3-regsum-ddfstop above,
//     for DDFSTRT.)
//   A-ddf-bits.c1 (sol, p277): bit 15 unused. A-ddf-bits.c2: bit 14 unused.
//     A-ddf-bits.c3: bit 13 unused. A-ddf-bits.c4: bit 12 unused.
//     A-ddf-bits.c5: bit 11 unused. A-ddf-bits.c6: bit 10 unused.
//     A-ddf-bits.c7: bit 9 unused. A-ddf-bits.c8: bit 8 unused.
//     A-ddf-bits.c9: bit 7 = H8. A-ddf-bits.c10: bit 6 = H7.
//     A-ddf-bits.c11: bit 5 = H6. A-ddf-bits.c12: bit 4 = H5.
//     A-ddf-bits.c13: bit 3 = H4. A-ddf-bits.c14: bit 2 = H3.
//     A-ddf-bits.c15: bit 1 unused. A-ddf-bits.c16: bit 0 unused.
//   (A-ddf-bits.c1..c16, all sol, p277: Appendix A bit table, "BIT# 15..00 /
//     USE X X X X X X X X H8 H7 H6 H5 H4 H3 X X" - sixteen ids, one per bit
//     position (bits 15-8 unused, 7-2 = H8..H3, 1-0 unused). All sixteen
//     quote the identical table row; distinguished only by which bit
//     position their restatement names.)
//   A-ddf.bits1 (opus, p277): same table, "bits 7-2 carry H8,H7,H6,H5,H4,H3".
//   A-ddf.bits2 (opus, p277): same table, "bits 15-8 and 1-0 marked X".
//   A-ddf.c01 (opus, p277): "These registers control the horizontal timing
//     of the beginning and end of the bitplane DMA display data fetch."
//   A-ddf.c03 (opus, p277): "The bitplane modulos are dependent on the
//     bitplane horizontal size and on this data-fetch window size." (not
//     independently tested here - the modulo/word-count relationship this
//     names is exactly G11/G13's territory)
//   A-ddf.c5, A-ddf.c6 (opus, p277): same "control the horizontal timing"
//     sentence, DDFSTRT/DDFSTOP halves.
//   A-ddf.c9 (opus, p277): same modulo-dependency sentence as A-ddf.c03.
//   A-ddf.c10 (opus, p277): "(Always set X bits to 0 to maintain upward
//     compatibility)" - a software-authoring instruction about UNUSED bits;
//     the unused-bits-have-no-effect half is what G17 actually measures
//     (whether they are literally required to be 0 for CORRECT operation
//     today, versus merely a forward-compatibility convention, is a
//     software-policy question this hardware-behaviour boundary cannot
//     settle either way - cited for context, not turned into its own pass/
//     fail).
//   A-ddf.c11 (opus, p277): "Extra wide (max) * 0 0 1 0 1" -> DDFSTRT H8-H4.
//   A-ddf.c12 (opus, p277): "Wide 0 0 1 1 0" -> DDFSTRT H8-H4.
//   A-ddf.c13 (opus, p277): "Normal 0 0 1 1 1" -> DDFSTRT H8-H4.
//   A-ddf.c14 (opus, p277): "Narrow 0 1 0 0 0" -> DDFSTRT H8-H4.
//   A-ddf.c15 (opus, p277): "Narrow 1 1 0 0 1" -> DDFSTOP H8-H4.
//   A-ddf.c16 (opus, p277): "Normal 1 1 0 1 0" -> DDFSTOP H8-H4.
//   A-ddf.c17 (opus, p277): "Wide (max) 1 1 0 1 1" -> DDFSTOP H8-H4.
//   (A-ddf.c11..c17: the named preset-code table - extra wide/wide/normal/
//     narrow for DDFSTRT H8-H4, narrow/normal/wide for DDFSTOP H8-H4 - seven
//     ids, each one row of named bit patterns.)
//   A-ddfstrt.fn, A-ddfstrt.n, A-ddfstrt.nw, A-ddfstrt.w, A-ddfstrt.xw (opus,
//     p277): register identity plus the same DDFSTRT preset table read a
//     second time (five ids).
//   A-ddfstop.fn, A-ddfstop.n, A-ddfstop.nw, A-ddfstop.w (opus, p277): same
//     for DDFSTOP (four ids).
//   B-subject.c18 (sol, p290): "DDFSTRT 092 W ... Display bitplane data
//     fetch start (horiz. position)" - Appendix B subject index: DDFSTRT is
//     WRITE-ONLY at $092.
//   B-subject.c21 (sol, p290): "DDFSTOP 094 W ... Display bitplane data
//     fetch stop (horiz. position)" - same, DDFSTOP at $094.
//
// -- G18: BPLxDAT receives the fetched DMA data (p.272) - this is also the
//    fixture's OWN required self-proof that its data path is alive before
//    any data-dependent assertion above is trusted --
//   A-bpldat.c4 (opus, p272): "These registers receive the DMA data fetched
//     from RAM by the bitplane address pointers described above."
//   A-bplxdat.c01 (sol, p272): same sentence, second reading.
//
// PLANNED ASSERTIONS (names, id groups and intent only; no RTL line numbers,
// no fixture code below this comment block - added in the next commit, after
// agnus_bitplanedma.v is opened for the first time):
//
//  0. FIXTURE-DELIVERS-KNOWN-VALUES-IN-ORDER [no page, harness self-proof]:
//     drive several DISTINCT known data words into the fetch path and show
//     each one is observed at the port carrying fetched data, IN THE ORDER
//     fetched - the precondition every other assertion below depends on.
//     Cites G18 (A-bpldat.c4, A-bplxdat.c01) as the manual's own statement of
//     what that data path is for.
//
//  1. LORES-STANDARD-20-WORDS-320-PIXELS (G1): DDFSTRT=$38/DDFSTOP=$D0/BPU=1
//     lores gives 20 fetches (already the pre-supplied measurement,
//     re-verified here as this package's own control case and cited on the
//     manual claims it actually proves).
//
//  2. HIRES-STANDARD-40-WORDS-640-PIXELS (G2): DDFSTRT=$3C/DDFSTOP=$D4/BPU=1
//     hires gives 40 fetches - the hires half G1's control does not cover,
//     same measurement basis (word count over one line).
//
//  3. LOW-RES-40-BYTES-8000-BYTES-PER-BITPLANE (G6): re-expresses assertion
//     1's word count as bytes (20 words x 2 = 40 bytes/line; x200 lines =
//     8,000), and checks the Figure 3-7 line-to-line stride is exactly 40
//     bytes (BPL1MOD=0 case of the pre-supplied BPL1MOD measurement).
//
//  4. HIRES-DOUBLE-BYTES-32000-TWO-PLANES (G2's byte-count ids, G13):
//     assertion 2's hires word count as bytes (80/line), and the p.76
//     bytes*lines*planes formula checked directly: BPU=2 hires, 200 lines,
//     total fetched bytes == 32,000.
//
//  5. DDF-FIELD-GRANULARITY-RAW-BIT0-IGNORED (G7): two DDFSTRT writes whose
//     raw 16-bit values differ only in bit 0 produce IDENTICAL fetch
//     behaviour; two writes differing by exactly 2 in raw value produce
//     DIFFERENT behaviour (one field step) - the concrete, RTL-measurable
//     shape of "4-pixel resolution" / "pixel 0 = position 0, pixel 4 =
//     position 1" at this boundary, derived from the write-decode fact
//     already in the boundary evidence.
//
//  6. DDFSTRT-DDFSTOP-WORD-COUNT-FORMULA (G7c): for at least one NON-standard
//     word count in each resolution, compute the expected DDFSTRT from
//     DDFSTOP via the printed formula (DDFSTRT = DDFSTOP - 8*(n-1) lores /
//     DDFSTOP - 4*(n-2) hires), write it, and confirm the ACTUAL fetched
//     word count equals n - testing the formula's own arithmetic beyond the
//     one pre-measured standard case, entirely within the DDF domain (no
//     disputed DIW offset needed).
//
//  7. STANDARD-DDFSTRT-DDFSTOP-VALUES (G7b): DDFSTRT=$0038/$003C and
//     DDFSTOP=$00D0/$00D4 for low/high resolution are exactly the values
//     that produce assertions 1 and 2's word counts - cites the standalone
//     "the normal ... DDFSTRT/DDFSTOP is (...)" sentences directly, on the
//     same runs as 1 and 2 rather than a separate run.
//
//  8. HW-LIMIT-DDFSTRT-LEFT-0018 / 9. HW-LIMIT-DDFSTOP-RIGHT-00D8 (G14):
//     DDFSTRT below $18 clamps to the $18 behaviour (no further leftward
//     fetch growth); DDFSTOP above $D8 clamps to the $D8 behaviour - the
//     already-measured "$00/$FE gives at most 25" fact, attributed to which
//     specific limit does the clamping, tested one at a time.
//
//  10. MAX-WORDS-25-LORES-49-HIRES (G14): at the hardware limits themselves
//     ($18/$D8), lores yields exactly 25 words and hires yields exactly 49 -
//     the two "maximum...words fetched" figures Table 3-14 states as flat
//     numbers, not just "at most" from the wider-than-limit measurement.
//
//  11. MAX-DISPLAY-PIXELS-368-LORES (G14): at the lores hardware limit, 25
//     words are fetched but Table 3-14 states max DISPLAY pixels as 368 (23
//     words' worth), one word short of 25 - checked as its own fact (two
//     numbers Table 3-14 gives that are NOT the same figure restated twice).
//
//  12. HSCROLL-EXTRA-WORD-21-FOR-20 (G16): DDFSTRT=$30 (one word earlier
//     than the standard $38), DDFSTOP unchanged at $D0, lores, BPU=1 yields
//     21 fetches where the unscrolled case (assertion 1) yields 20 - the
//     "+1 word / 42 bytes" and "DDFSTOP unchanged" claims together, and
//     (via the addresses fetched) the Figure 3-24/26 "one extra word placed
//     before/left of the normal fetch, at START" placement.
//
//  13. DUAL-PLAYFIELD-SHARED-DDF-REGISTERS (G10): both playfields' bitplanes
//     (odd/even BPU split) fetched under ONE DDFSTRT/DDFSTOP pair still
//     produce the single combined word-count assertion 1/2's formula
//     predicts for the total BPU - direct test of "only one data-fetch
//     start/stop register... shared by both playfields", distinct from
//     assertion 12's single-playfield scroll case.
//
//  14. END-OF-LINE-MODULO-APPLIED-AFTER-LAST-WORD (G19): re-cites the
//     pre-supplied BPL1MOD 0/40/80 -> 40/80/120 measurement under its
//     manual claims (DDFSTOP, not DDFSTRT, defines the end-of-line point
//     where the modulo is added; the modulo changes the NEXT line's start,
//     never the CURRENT line's own word count) - a control showing modulo
//     changes address stride without changing fetched word count is the
//     comparison basis.
//
//  15. WIDER-PLAYFIELD-FIRST-LINE-UNCHANGED (G11): BPL1MOD=40 (wider RAM
//     line than displayed line) still fetches the SAME 20 words at the SAME
//     addresses ($0..$1F3E, i.e. START..START+38) as BPL1MOD=0 for line 1 -
//     Figure 3-15's point that a nonzero modulo changes line 2 onward, not
//     line 1 itself.
//
//  16. BITPLANE-COUNT-x-BYTES-x-LINES-FORMULA [control, no new page beyond
//     G13/A-ddf.c03/c09]: total fetched bytes over N lines scales linearly
//     with BPU (1 vs 2 vs 4 planes, same DDFSTRT/DDFSTOP) - the general form
//     of the p.76 arithmetic, with assertion 4 as its one concretely-cited
//     (80*200*2=32,000) instance.
//
// MEASUREMENT RECORDS (not assertions - printed manual errors this package
// must not turn into red RTL findings):
//   - The p.60 4.5-vs-8.5 color-clock offset (3-ddf.c08/c09/c10, 3-fetch.c8):
//     whichever of {4.5, 8.5} actually produces DDFSTRT=$0038 from
//     DIWSTRT=$81 in this RTL, if it can be observed at all at this
//     boundary (DIWSTRT is not named in the given boundary evidence), is
//     recorded as a measurement alongside this note - not asserted as "the"
//     correct manual value, because the manual itself does not agree with
//     itself on it.
//   - Table 3-8's PAL byte-per-bitplane counts: PER THE BUILD BRIEF, the
//     printed manual gives 8,192 / 16,384 / 16,384 / 32,768 where the
//     arithmetic gives 10,240 / 20,480 / 20,480 / 40,960 (Table 3-7's NTSC
//     figures rounded up to powers of two). No claim id in this package's
//     239 quotes Table 3-8's PAL rows directly (the claims corpus for this
//     package is entirely NTSC/generic, per the ids above) - if the fixture
//     ends up exercising a PAL byte-per-bitplane configuration for any other
//     reason, its result is recorded as a measurement, never asserted
//     against the printed 8,192/16,384/16,384/32,768 figures.
//
// PENDING, to be resolved only after the RTL is read (moved to a real
// assertion above, or to "could not turn into an assertion" in this
// package's final report - never silently dropped):
//   - G3 (3-res.c13, 3-resolution.c6): does agnus_bitplanedma enforce a
//     BPU<=4 ceiling in hires mode, or is this a software convention this
//     RTL does not check?
//   - DIW cluster (3-diw.c06, 3-diw.c24, 3-window.c18, 3-window.c9): do
//     DIWSTRT/DIWSTOP's horizontal fields reach this module's ports at all?
//   - Interlace cluster (3-mod.c15): does this module carry any
//     interlace/field-parity signal, or is interlaced addressing assembled
//     one level up?
//   - 3-ddf.c13/3-fetch.c11's "twice the hardware resolution" DIW-vs-DDF
//     comparison: testable only if the DIW cluster above turns out to be in
//     scope.
//
// No RTL has been opened yet to write this comment block. Every port name,
// register offset and bit-slice fact above (DDFSTRT_REG/DDFSTOP_REG,
// ddfstrt[8:2]<=data_in[7:1], the $18/$D8 limits, ddfena) comes from this
// package's own build brief (PACKAGE.md), and every numeric fetch-count/
// address/line-count fact comes from the ALREADY MEASURED section above,
// itself handed to this package rather than re-derived. The fixture,
// Harness helpers and assertion bodies are added in the next commit, after
// agnus_bitplanedma.v is read for the first time.

#include <verilated.h>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    // Placeholder. Fixture and assertions follow the RTL-open line above,
    // in the next commit - this file is committed now, on its own, to
    // satisfy the spec-before-RTL gate.
    return 0;
}
