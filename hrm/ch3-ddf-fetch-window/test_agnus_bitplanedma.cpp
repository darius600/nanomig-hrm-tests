// Test purpose: Where display data fetch starts and stops and how many words
//   come out of a line - DDFSTRT/DDFSTOP, their 4-pixel/CCK bit granularity,
//   the $18/$D8 hardware limits, the extra word a horizontal scroll costs,
//   the modulo applied at end-of-line, and the per-line word/byte counts for
//   low and high resolution (including the printed manual's own arithmetic
//   error in Table 3-8's PAL byte counts, which this package measures rather
//   than asserts).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 40, 41, 42, 45, 49, 51, 53, 54, 57, 59, 60, 61, 62, 63, 72, 73,
//   74, 75, 76, 80, 81, 82, 83, 84, 85, 91, 272, 277, 290.
// Non-goals: pixel *colour* (p.42, p.49 "16 colors") - Denise's palette, not
//   observable at this DMA-count boundary; the blitter's maximum draw area
//   (p.76) - agnus_blitter's boundary, not this one; sprite-disable side
//   effects of an early DDFSTRT (p.80, p.82, p.83) - agnus_spritedma's
//   boundary; the Copper's ability to reset pointers during vblank (p.80) -
//   agnus_copper's boundary and a software-usage note, not a DMA-count fact.
//   DIWSTRT/DIWSTOP *horizontal* placement/centering (p.57, p.59) and the
//   interlaced-field line-3 stride example (p.62) are held PENDING below:
//   this package's given boundary evidence names only
//   DDFSTRT_REG/DDFSTOP_REG, not DIWSTRT/DIWSTOP, and the one DIWSTOP fact
//   already measured on this fixture (V $F2..$F6 -> 198..202 lines) is
//   DIWSTOP's VERTICAL half, not its horizontal one - whether horizontal DIW
//   or interlace reach this module's ports at all is an RTL question,
//   answered below the RTL-open line.
// Claim ids: the id below carries a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and it names
//   one reader's reading of one printed sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
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
// the classify records for `ch3-ddf-fetch-window` -- a build-time
// scratch file, never published and no longer on disk; the claim ids
// cited below are the surviving public record,
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
// -- G20: dual-playfield hires bitplane-per-playfield ceiling (p.68) -
//    PENDING RTL --
//   3-dual-planes.c4 (sol, p68): "In high resolution mode, you can have up
//     to two bitplanes in each playfield -- bitplanes 1 and 3 in playfield
//     1 and bitplanes 2 and 4 in playfield 2."
//   3-dual.c18 (opus, p68): "In high resolution mode, you can have up to
//     two bitplanes in each playfield." (same sentence, opening clause)
//   This restates G3's total-of-four hires ceiling (p.49) split by
//   playfield, and additionally names which physical bitplane goes to
//   which playfield. Whether agnus_bitplanedma carries any
//   playfield-assignment concept at all (DBLPF, or a signal distinguishing
//   which fetched plane is "playfield 1" versus "playfield 2"), as opposed
//   to only a total bitplane count, is answered after the RTL is open. If
//   the RTL carries no such signal, these two ids move to the "could not
//   assert" / record list, not to a green assertion built on an
//   assumption.
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

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after agnus_bitplanedma.v was
// read in full for the first time - the specification comment above this
// point was committed on its own first, per the chapter's spec-before-RTL
// gate; see this package's own git history, commit 69109a2).
//
// PORTS (agnus_bitplanedma.v:29-47): clk, clk7_en, reset, harddis, aga, ecs,
// a1k, sof, dmaena, vpos[10:0], hpos[8:0] are inputs; hde, dma, address_out
// [20:1], reg_address_out[8:1] are outputs; reg_address_in[8:1]/data_in
// [15:0] are the register write bus. hpos/vpos are NOT counted internally by
// this module - they are driven in from outside (by `agnus` in the real
// design), exactly as hrm/agnus_bitplanedma's own existing RTL_REGRESSION
// fixture already drives them (hpos 0..455 per line, one fixture tick per
// hpos value, vpos swept from 0 up to the target line) - reused here as a
// proven-good drive pattern, not reinvented.
//
// NO DATA-VALUE PORT. This module computes WHEN a fetch happens (`dma`),
// WHERE it reads from (`address_out`) and WHICH BPLxDAT register receives it
// (`reg_address_out`) - it does not read chip RAM itself and has no fetched
// -word output. The "known value" this fixture can feed in and watch arrive,
// per this package's harness-self-proof requirement, is therefore the
// PER-PLANE POINTER, not a data word: assertion 1 below programs four
// distinct pointers (one per bitplane) and confirms each one's OWN address
// stream arrives, in order, at its OWN reg_address_out slot - the same
// "silently dead data path" failure mode the requirement warns about would
// show up here as every plane reading back the same address, or addresses
// arriving out of order.
//
// REGISTER DECODE (agnus_bitplanedma.v:50-60,247-274): DDFSTRT=9'h092,
// DDFSTOP=9'h094, BPL1MOD=9'h108, BPL2MOD=9'h10a, BPLCON0=9'h100,
// FMODE=9'h1fc, DIWSTRT=9'h08E, DIWSTOP=9'h090, bitplane pointer base
// =9'h0E0. Every write is captured unconditionally on the matching
// reg_address_in on any clk7_en tick - no bus arbitration to retry, unlike
// the top-level `agnus` the PKG-AGNUS-DMACON package had to contend with.
// write_reg() below is a single one-tick pulse plus a clearing tick.
//
// BITPLANE POINTERS (l.217-242): BPLPTBASE_REG[8:5] decodes the 0xE0-0xFE
// block; plane index N (0-7) sits at PTH=0xE0+4N, PTL=0xE2+4N (verified from
// the RAW ADDRESS bits, not assumed: reg_address_in[4:2] directly reads
// address bits [4:2], and 0xE0/0xE4/0xE8/0xEC/0xF0 give plane_sel 0/1/2/3/4
// respectively). bplpth_in <= data_in[4:0] (address bits 20:16), bplptl_in
// <= data_in[15:1] (address bits 15:1) - together a 20-bit word-granular
// byte address (address_out[20:1], bit 0 implied 0). Auto-increment
// (l.483-493): +1 to address_out[20:1] per fetch at FMODE=0 (bp_fmode0), i.e.
// +2 bytes - matches the pre-supplied "address_out stepping 2 bytes"
// measurement exactly, not re-derived by assumption.
//
// reg_address_out (l.496-507): a fixed lookup, plane 0-7 -> 0x88-0x8F (this
// module's OWN 8-bit reg_address_out field; *2 gives the real BPL1DAT..
// BPL8DAT byte address, so plane 0 -> 0x110, matching the pre-supplied
// "reg_address_out=$110" measurement for BPU=1's one active plane).
//
// DDFSTRT/DDFSTOP GRANULARITY (l.247-259, 336-354): write decode is
// ddfstrt[8:2] <= data_in[7:1] - data_in bit 0 is never read, so a DDFSTRT
// write whose raw 16-bit value differs from another only in bit 0 must
// program an IDENTICAL stored field; a difference of exactly 2 in raw value
// changes the stored field by exactly one step. The position COMPARE
// (l.336-343) matches hpos[8:1] against {ddfstrt[8:3], ddfstrt[2]&ecs,
// 1'b0} - note the trailing 1'b0: the compared value's own LSB is hardwired,
// so a match can only occur on an EVEN hpos[8:1], which is what "4-pixel
// resolution" cashes out to at this boundary once hpos's own units are
// pixel-pairs (assertion 5's DIWSTRT compare, l.171 `hpos==cur_hdiwstrt`,
// uses the FULL hpos value with no such masking - directly confirming
// "twice the hardware resolution" (3-ddf.c13/3-fetch.c11) as a literal 2x
// in raw register LSB significance, not merely restated from the pixel-unit
// framing).
//
// ecs GATES ddfstrt[2], NOT H3 and NOT hires - corrected here after
// NM-HRM-006 (24.09.2026) found this package had misread which stored bit
// is H3. Read literally, not assumed: the position compare is
// `{ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0}`. H3 is register bit 2 of the raw
// DDFSTRT/DDFSTOP word (Appendix A p.277, "bit 2 = H3"); the write decode
// ddfstrt[8:2]<=data_in[7:1] means register bit 2 (data_in[2]) is STORED as
// ddfstrt[3] - part of the UNCONDITIONALLY-compared `ddfstrt[8:3]` group,
// gated by neither `ecs` nor the `hires` wire (bplcon0_delayed[4], toggled
// per display via BPLCON0). The term the compare actually DOES gate by
// `ecs` - `ddfstrt[2] & ecs` - is STORED bit ddfstrt[2], which holds
// register bit 1 (data_in[1]): outside the manual's H8-H3 bit map entirely
// ("Bits 1-0 - not used", p.91/277), and not H3. The manual's Appendix
// A/p.91 claims "(bit H3 only respected in Hires Mode.)" - a MODE claim
// about H3 specifically. DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE tests
// H3 itself directly, both axes independently (ecs on/off, hires on/off)
// rather than assuming the two happen to coincide in this fixture's runs,
// and reports whatever the RTL actually does; a companion MEASURED note
// there records register bit 1's own (ecs-gated, not H3's) effect.
//
// DIWSTRT/DIWSTOP HORIZONTAL FIELDS REACH THIS MODULE (l.103-104,150-205),
// via hdiwstrt/hdiwstop, captured from data_in[7:0] (DIWSTRT: l.186,
// `hdiwstrt <= {1'b0, data_in[7:0]}`) with NO bit dropped - unlike
// DDFSTRT's data_in[0]. But they feed ONLY the `hde` output (l.171-176,
// display/border enable), never `dma`/`address_out`/`reg_address_out` -
// confirmed by reading every consumer of hdiwstrt/hdiwstop/cur_hdiwstrt/
// cur_hdiwstop in the file (there is exactly one: the hde block at the
// top). So the p.57/59 DIW-horizontal claims ARE testable at this
// boundary, but only via `hde`, and are NOT part of this module's DDF
// fetch-count story - assertion 7 tests them on `hde` specifically, kept
// separate from the word-count assertions. The `cur_hdiwstrt`/
// `cur_hdiwstop` values used for the comparison are only committed at a
// vpos==0 edge from a "best window seen last field" tracker (l.150-169) -
// assertion 7's fixture runs two full frames with DIWSTRT/DIWSTOP held
// constant so this has latched before any hde observation is trusted.
//
// NO INTERLACE PORT. There is no `interlace`/field-parity input on this
// module's port list at all (checked against the full list above, not
// assumed) - the only field-parity-shaped signal anywhere in the file is
// `vdiwstrt[0] ^ vpos[0]` inside the BPLxMOD *scandoubling* adjustment
// (l.474-481, explicitly commented "adjusted for bitplane scandoubling"),
// a display-side line-doubling feature, not genuine interlaced-field
// addressing. 3-mod.c15 (p.62, the interlaced Line-3-at-START+80 example)
// is therefore reported as NOT ASSERTABLE at this boundary in the final
// report, not silently dropped: this module has nothing to assert it
// against.
//
// CORRECTED - an earlier version of this note said the opposite of both
// points below and was titled "NO HIRES BPU CEILING"; kept here with its
// correction on the record rather than silently rewritten.
//
// HIRES BPU CEILING, VIA THE PLANE ENCODER, NOT VIA `bpu` ITSELF. `bpu`
// (l.298-300) is computed from bplcon0_delayed bits alone - for non-AGA it
// is bplcon0_delayed[2:0], with the single special case that 3'b111 forces
// bpu=4. The prior version of this note called that special case "the real
// OCS/ECS 'BPU=7 code means dual-playfield 4+4' encoding" - WRONG against
// the manual: Table 3-5 (p.48) and the BPLCON0 bit summary (p.90) both list
// code 111 as "Value not used" / "not used", not as a defined dual-playfield
// meaning; the sixth bitplane's dual-playfield/HAM use (p.48's own note) is
// carried by code 110 (BPU=6), not 111. `hires` is correctly never read in
// the `bpu` assignment itself - but the ceiling is real, enforced two lines
// away, at the `plane` encoder (l.457-461): in hires with FMODE=0 (the
// standard OCS/ECS fetch mode), `plane = {3'b000,~ddfseq[0],~ddfseq[1]}` -
// only two bits wide, so `plane` can never reach 4 or 5 no matter what `bpu`
// requests. `dma` (l.472) gates on `plane[4:0] < bpu[3:0]`, so a `plane`
// that structurally cannot exceed 3 caps the fetch at 4 bitplanes regardless
// of `bpu`. Measured directly, not merely read: HIRES-BPU-CEILING-MAX-4-
// PLANES below sets BPU=6 in hires and confirms reg_address_out $8C/$8D
// (bitplanes 5-6) never fire, with the same BPU=6 in lores as a control
// showing they DO fire there - the ceiling is real and hires-specific.
// 3-res.c13/3-resolution.c6 ("up to four bitplanes" in hires) are graded on
// that assertion, not declined.
//
// DUAL-PLAYFIELD SPLIT (p.68, G20) FOLLOWS FROM THE SAME CEILING. This
// module carries no DBLPF/playfield-assignment signal at all (grepped the
// whole file for `dblpf`/`DBLPF`/`playfield`: no hit) - it cannot itself
// distinguish "playfield 1" from "playfield 2". But the manual's own
// bitplane-to-playfield numbering is fixed (odd bitplanes 1/3/5/7 to
// playfield 1, even 2/4/6/8 to playfield 2, p.68), not something the RTL
// needs to compute, and the four planes HIRES-BPU-CEILING-MAX-4-PLANES
// shows are the only ones ever fetched in hires - bitplanes 1-4 - split
// into exactly two of each parity. DUAL-PLAYFIELD-HIRES-CEILING-2-PER-
// PLAYFIELD below measures that split directly (odd: reg $88/$8A; even:
// $89/$8B) and grades 3-dual-planes.c4/3-dual.c18 on the fetch-side half of
// the claim; which VISUAL playfield each parity renders to is Denise's
// boundary, outside this module and outside this package.
//
// MUTATION LOG: see the bottom of this file, after main(). Recorded there
// so it can quote this file's own assertion labels; every entry was
// actually run - edited, rebuilt under WSL/Verilator (`make test`), run,
// reverted, rebuilt clean - by this agent, not predicted.
// ============================================================================

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <verilated.h>
#include "Vagnus_bitplanedma.h"

namespace {

// ---- register addresses (9-bit custom-chip byte offsets, as written in the
// HRM and in agnus_bitplanedma.v's own localparam declarations) ----
constexpr uint16_t DIWSTRT_ADDR = 0x08E;
constexpr uint16_t DIWSTOP_ADDR = 0x090;
constexpr uint16_t DDFSTRT_ADDR = 0x092;
constexpr uint16_t DDFSTOP_ADDR = 0x094;
constexpr uint16_t BPLPT_BASE   = 0x0E0;  // plane N: PTH=BASE+4N, PTL=BASE+4N+2
constexpr uint16_t BPL1MOD_ADDR = 0x108;
constexpr uint16_t BPL2MOD_ADDR = 0x10A;
constexpr uint16_t BPLCON0_ADDR = 0x100;
constexpr uint16_t FMODE_ADDR   = 0x1FC;

// Standard values, p.60/p.80 (Table 3-14) and the pre-supplied measurement.
constexpr uint16_t DDFSTRT_LORES_STD = 0x0038;
constexpr uint16_t DDFSTOP_LORES_STD = 0x00D0;
constexpr uint16_t DDFSTRT_HIRES_STD = 0x003C;
constexpr uint16_t DDFSTOP_HIRES_STD = 0x00D4;
constexpr uint16_t DDFSTRT_HW_LIMIT  = 0x0018;
constexpr uint16_t DDFSTOP_HW_LIMIT  = 0x00D8;
// Standard DIW, matching the "starting position of ($81,$2C) centers a
// normal size display" example (p.57) and the existing RTL_REGRESSION
// fixture's own known-good values.
constexpr uint16_t DIWSTRT_STD = 0x2C81;
constexpr uint16_t DIWSTOP_STD = 0x2CC1;

uint16_t bplcon0_word(bool hires, int bpu /*0-7*/) {
    // bplcon0 <= {data_in[6], data_in[15], aga&data_in[4], data_in[14:12]}
    // stored bits: [5]=SHRES(data_in[6]), [4]=HIRES(data_in[15]),
    // [3]=BPU3(AGA only, left 0 here), [2:0]=BPU2..0(data_in[14:12]).
    uint16_t w = 0;
    if (hires) w |= 0x8000;
    w |= static_cast<uint16_t>((bpu & 0x7) << 12);
    return w;
}

}  // namespace

class Harness {
public:
    Harness() : dut(new Vagnus_bitplanedma) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->harddis = 0;
        dut->aga = 0;
        dut->ecs = 1;
        dut->a1k = 0;
        dut->sof = 0;
        dut->dmaena = 0;
        dut->vpos = 0;
        dut->hpos = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        for (int i = 0; i < 10; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 10; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    void write_reg(uint16_t reg9, uint16_t data) {
        dut->reg_address_in = static_cast<uint16_t>(reg9 >> 1);
        dut->data_in = data;
        tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        tick();
    }

    void write_ptr(int plane, uint32_t byte_addr) {
        uint16_t pth = static_cast<uint16_t>(BPLPT_BASE + 4 * plane);
        uint16_t ptl = static_cast<uint16_t>(BPLPT_BASE + 4 * plane + 2);
        write_reg(pth, static_cast<uint16_t>((byte_addr >> 16) & 0x1F));
        write_reg(ptl, static_cast<uint16_t>(byte_addr & 0xFFFF));
    }

    // Standard setup shared by most assertions: DIW centered per p.57's
    // ($81,$2C) example, dmaena on, harddis off (hardware limits enforced,
    // not bypassed), FMODE=0 (the pre-supplied measurement's own basis).
    void setup_standard(bool ecs_on = true) {
        dut->ecs = ecs_on ? 1 : 0;
        write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
        write_reg(DIWSTOP_ADDR, DIWSTOP_STD);
        write_reg(FMODE_ADDR, 0x0000);
        dut->dmaena = 1;
    }

    Vagnus_bitplanedma* dut;
};

struct Fetch {
    uint32_t address;   // address_out, byte address
    uint16_t reg;        // reg_address_out (raw 8-bit field; *2 = real BPLxDAT addr)
};

struct LineResult {
    int count = 0;
    std::vector<Fetch> fetches;
};

// Runs vpos 0..target_vpos inclusive, hpos 0..hpos_max-1 each line, and
// records every dma pulse seen while vpos==target_vpos, in time order.
// Mirrors hrm/agnus_bitplanedma's own existing RTL_REGRESSION fixture's
// proven-good drive pattern (external hpos/vpos sweep, one tick per hpos
// value) rather than inventing a new one.
LineResult run_to_line(Harness& h, int target_vpos, int hpos_max = 456) {
    LineResult r;
    for (int v = 0; v <= target_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
            if (v == target_vpos && h.dut->dma) {
                ++r.count;
                r.fetches.push_back({static_cast<uint32_t>(h.dut->address_out) * 2,
                                      static_cast<uint16_t>(h.dut->reg_address_out)});
            }
        }
    }
    return r;
}

// Sweeps vpos 0..through_vpos WITHOUT recording anything - used to prime
// vdiwena (which only goes active once vpos crosses vdiwstrt, l.207-215)
// and let pipelined signals (bplcon0_delayed, dmaena_delayed, ...) settle,
// before a fresh pointer write is trusted to be "the first fetch this
// plane makes" rather than one more step of a run already under way. Found
// necessary empirically: bitplane pointers auto-increment on EVERY dma
// cycle from the moment vdiwena goes active, so a pointer written once
// before a long run_to_line sweep is no longer at its written value by the
// time the target line is reached (caught by HARNESS-PROVES-PLANE-ADDRESS-
// ORDERING's first draft, which compared a 20-lines-later address against
// the originally-written one and failed - see the MUTATION LOG's own
// account of this, further down, for the exact numbers).
void prime_to_line(Harness& h, int through_vpos, int hpos_max = 456) {
    for (int v = 0; v <= through_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
        }
    }
}

// Runs exactly one more line at h.dut->vpos+1 (does NOT touch vpos before
// starting - caller sets it up via prime_to_line first) and records every
// dma pulse, in order.
LineResult run_single_line(Harness& h, int this_vpos, int hpos_max = 456) {
    LineResult r;
    h.dut->vpos = this_vpos;
    for (int hp = 0; hp < hpos_max; ++hp) {
        h.dut->hpos = hp;
        h.tick();
        if (h.dut->dma) {
            ++r.count;
            r.fetches.push_back({static_cast<uint32_t>(h.dut->address_out) * 2,
                                  static_cast<uint16_t>(h.dut->reg_address_out)});
        }
    }
    return r;
}

// Returns the hpos of hde's rising edge on the given vpos line (-1 if
// none). Writes DIWSTRT (leaving DIWSTOP at DIWSTOP_STD) and runs it
// through TWO frame starts before observing - cur_hdiwstrt/cur_hdiwstop
// (the values hde's own compare actually uses, l.171-172) only commit from
// a "best window seen last field" tracker at a vpos==0 edge (l.150-169);
// held constant, the programmed window is captured as "best" on the first
// ordinary line after frame 1's own vpos==0 (whose OWN commit reads the
// power-up-zero best, not yet useful), and committed to cur_* at frame 2's
// vpos==0 - confirmed empirically, not assumed (see the MUTATION LOG's own
// account of this measurement).
int hde_rising_edge(uint16_t diwstrt_raw, int hpos_max = 456) {
    Harness h;
    h.write_reg(DIWSTRT_ADDR, diwstrt_raw);
    h.write_reg(DIWSTOP_ADDR, DIWSTOP_STD);
    for (int v = 0; v <= 1; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) { h.dut->hpos = hp; h.tick(); }
    }
    h.dut->vpos = 0;  // frame 2 start: commits cur_hdiwstrt/cur_hdiwstop
    for (int hp = 0; hp < hpos_max; ++hp) { h.dut->hpos = hp; h.tick(); }
    h.dut->vpos = 1;
    bool prev = false;
    int rise = -1;
    for (int hp = 0; hp < hpos_max; ++hp) {
        h.dut->hpos = hp;
        h.tick();
        bool cur = h.dut->hde;
        if (!prev && cur) rise = hp;
        prev = cur;
    }
    return rise;
}

// Returns the hpos value at which the FIRST dma pulse of the target line
// occurs, or -1 if none. Word COUNT alone cannot show DDFSTRT bit H3's
// effect (ddfstrt[3], register bit 2 per Appendix A p.277) - it is absorbed
// by the ddfseq 8-CCK quantization before it ever changes a total
// (measured, not assumed: DDF-16-PIXEL-EFFECTIVE-GRANULARITY above). Only
// the exact TIMING of the first fetch can.
int first_fetch_hpos(Harness& h, int target_vpos, int hpos_max = 456) {
    for (int v = 0; v <= target_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
            if (v == target_vpos && h.dut->dma) return hp;
        }
    }
    return -1;
}

// Sums dma pulses across every line from from_vpos to to_vpos inclusive -
// a genuine multi-line total (not one line's count multiplied), for the
// p.76 bytes-per-line x lines x bitplanes formula.
int sum_dma_over_lines(Harness& h, int from_vpos, int to_vpos, int hpos_max = 456) {
    int total = 0;
    for (int v = 0; v <= to_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
            if (v >= from_vpos && h.dut->dma) ++total;
        }
    }
    return total;
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // A measurement record, not a claim: prints what was observed with no
    // pass/fail verdict and does not touch passed/failed. Used for RTL
    // behaviour this package has no manual claim to grade against.
    void note(const std::string& label) {
        std::cout << "MEASURED: " << label << "\n";
    }
    // Declines one or more claim uids: the standard suite-wide convention
    // (check_uid_placement.py's RECORDED line). Prints with no pass/fail
    // verdict and does not touch passed/failed. A uid printed here must not
    // also sit on a graded (PASS/FAIL) line in this same package.
    void record(const std::string& msg) {
        std::cout << "RECORDED: " << msg << "\n";
    }
    int passed = 0;
    int failed = 0;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- HARNESS-PROVES-PLANE-ADDRESS-ORDERING [self-proof, no page] ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 4));  // lores, BPU=4

        // Prime vdiwena and the pipelined delay chains WITHOUT the pointers
        // under test set yet (see prime_to_line's own comment for why: a
        // pointer written before this priming sweep is no longer at its
        // written value by the time it is read, since it auto-increments on
        // every dma cycle from the moment vdiwena goes active).
        prime_to_line(h, 0x4F);
        // NOW write four DISTINCT, easily-told-apart pointers, one per
        // plane, immediately before the one line this assertion measures.
        constexpr uint32_t kBase[4] = {0x002000, 0x004000, 0x006000, 0x008000};
        for (int p = 0; p < 4; ++p) h.write_ptr(p, kBase[p]);

        LineResult r = run_single_line(h, 0x50);

        // Group by reg_address_out (the plane the module itself says this
        // fetch belongs to), in time order.
        std::map<uint16_t, std::vector<uint32_t>> by_plane;
        for (auto& f : r.fetches) by_plane[f.reg].push_back(f.address);

        bool all_four_present = by_plane.size() == 4 &&
            by_plane.count(0x88) && by_plane.count(0x89) &&
            by_plane.count(0x8A) && by_plane.count(0x8B);
        bool starts_match = all_four_present &&
            by_plane[0x88].front() == kBase[0] && by_plane[0x89].front() == kBase[1] &&
            by_plane[0x8A].front() == kBase[2] && by_plane[0x8B].front() == kBase[3];
        bool sequences_increment = true;
        int total_checked = 0;
        if (all_four_present) {
            for (auto& [reg, addrs] : by_plane) {
                for (size_t i = 1; i < addrs.size(); ++i) {
                    if (addrs[i] != addrs[i - 1] + 2) sequences_increment = false;
                    ++total_checked;
                }
            }
        }
        checks.expect(
            all_four_present && starts_match && sequences_increment && total_checked > 0,
            "HARNESS-PROVES-PLANE-ADDRESS-ORDERING: four DISTINCT known "
            "pointers (0x002000/0x004000/0x006000/0x008000, one per plane) "
            "each arrive at their OWN reg_address_out slot (0x88/0x89/0x8A/"
            "0x8B), each one's FIRST fetched address equals exactly the "
            "pointer programmed for that plane, and each plane's own "
            "successive fetches increment by exactly 2 bytes, IN ORDER - "
            "this fixture's data path is not silently dead before any "
            "other assertion below is trusted to mean anything. 'These "
            "registers receive the DMA data fetched from RAM by the "
            "bitplane address pointers described above' (p.272). "
            "A-bpldat.c4, A-bplxdat.c01.");
    }

    // ---------------- LORES-STANDARD-20-WORDS-320-PIXELS (p.40,41,45,49,60,63) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));  // lores, BPU=1

        LineResult r = run_to_line(h, 0x50);
        int pixels = r.count * 16;  // 16 lores pixels/word (a 16-bit word, 1 bit/pixel/plane)

        checks.expect(
            r.count == 20 && pixels == 320,
            "LORES-STANDARD-20-WORDS-320-PIXELS: DDFSTRT=$0038, DDFSTOP=$00D0 "
            "(both 'the normal low resolution' values, p.60), BPU=1 fetches "
            "exactly 20 words/line = 320 pixels - 'the normal playfield has "
            "a width of 320 pixels' (p.41), 'Low resolution mode provides "
            "320 pixels for each horizontal line' (p.49), '320 pixels in "
            "each horizontal line' (p.63 summary). opus:3-video.c12, 3-video.c16, "
            "3-res.c01, 3-res.f01, 3-video.c17, 3-video.c19, 3-video.c21, "
            "3-basic-size.c1, 3-basic.c01, 3-res.c07, 3-resolution.c1, "
            "3-sum.c03, 3-ddf.c11, 3-ddf.c16, 3-fetch.c9, 3-fetch.c14.");
    }

    // ---------------- HIRES-STANDARD-40-WORDS-640-PIXELS (p.41,45,49,60,62,63,75) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_HIRES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_HIRES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(true, 1));  // hires, BPU=1

        LineResult r = run_to_line(h, 0x50);
        int pixels = r.count * 16;

        checks.expect(
            r.count == 40 && pixels == 640,
            "HIRES-STANDARD-40-WORDS-640-PIXELS: DDFSTRT=$003C, DDFSTOP=$00D4 "
            "('the normal high resolution' values, p.60), BPU=1 fetches "
            "exactly 40 words/line = 640 pixels, twice LORES-STANDARD's 20/"
            "320 for the same physical width - 'High resolution mode gives "
            "finer horizontal resolution -- 640 pixels in the same physical "
            "display area' (p.41), '640 pixels for each horizontal line' "
            "(p.49), 'the object will be only half as wide' (p.49), '640 "
            "pixels in each horizontal line' (p.63), 'you need to fetch 80 "
            "bytes for each line, instead of 40' (p.62, one bitplane's byte "
            "count = word count x 2). 3-res.c02, 3-video.c18, 3-video.c22, "
            "3-basic-size.c2, 3-basic.c02, 3-res.c08, 3-res.c09, "
            "3-resolution.c2, 3-resolution.c3, 3-sum.c05, 3-ddf.c18, "
            "3-modulo.c19, 3-ddf.c12, 3-ddf.c17, 3-fetch.c10, 3-fetch.c15.");
    }

    // ---------------- LORES-BYTES-40-PER-LINE-8000-PER-BITPLANE (p.51,53,54) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));  // lores, BPU=1

        LineResult one_line = run_to_line(h, 0x50);
        int bytes_per_line = one_line.count * 2;

        Harness h2;
        h2.setup_standard();
        h2.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h2.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h2.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        // vdiwena goes active at vpos==vdiwstrt's V field (0x2C=44); 200
        // REAL lines summed from there, not one line's count multiplied.
        int total_words_200 = sum_dma_over_lines(h2, 0x2C, 0x2C + 199);
        int total_bytes_200 = total_words_200 * 2;

        checks.expect(
            bytes_per_line == 40 && total_bytes_200 == 8000,
            "LORES-BYTES-40-PER-LINE-8000-PER-BITPLANE: LORES-STANDARD's 20 "
            "words/line is 40 bytes/line (20x2); summed over 200 REAL "
            "simulated lines (not one line's count multiplied) that is "
            "8,000 bytes - 'each line of the bitplane requires 40 bytes "
            "(320 bits divided by 8 bits per byte = 40)' and 'Multiply the "
            "200 lines times 40 bytes per line to get [8,000 bytes per "
            "bitplane]' (p.53), 'Figure 3-7 shows an 8,000-byte memory area "
            "organized as 200 lines of 40 bytes each' (p.53), Figure 3-7's "
            "'Mem. location N+40' next-line stride (p.54), and 'high "
            "resolution or interlaced playfields...require larger "
            "bitplanes' (p.51, the qualitative form this quantifies). "
            "3-resolution.c23, 3-mem.c07, 3-mem.c08, 3-memory.c14, "
            "3-memory.c15, 3-memory.c19, 3-mem.f02.");
    }

    // ---------------- HIRES-BYTES-80-PER-LINE-32000-TWO-PLANES (p.62,75,76) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_HIRES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_HIRES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(true, 1));  // hires, BPU=1
        LineResult one_plane_line = run_to_line(h, 0x50);
        int bytes_per_line_one_plane = one_plane_line.count * 2;

        Harness h2;
        h2.setup_standard();
        h2.write_reg(DDFSTRT_ADDR, DDFSTRT_HIRES_STD);
        h2.write_reg(DDFSTOP_ADDR, DDFSTOP_HIRES_STD);
        h2.write_reg(BPLCON0_ADDR, bplcon0_word(true, 2));  // hires, BPU=2
        int total_words_2pl_200 = sum_dma_over_lines(h2, 0x2C, 0x2C + 199);
        int total_bytes_2pl_200 = total_words_2pl_200 * 2;

        checks.expect(
            bytes_per_line_one_plane == 80 && total_bytes_2pl_200 == 32000,
            "HIRES-BYTES-80-PER-LINE-32000-TWO-PLANES: HIRES-STANDARD's 40 "
            "words/line/plane is 80 bytes/line/plane (40x2, twice "
            "LORES-BYTES' 40); summed over 200 REAL simulated lines with "
            "BPU=2, total is 32,000 bytes, exactly 'bytes per line * lines "
            "in playfield * # of bitplanes' = '80 * 200 * 2 = 32,000 bytes "
            "of memory' (p.76). 'you need to fetch 80 bytes for each line, "
            "instead of 40' (p.62), 'in high resolution mode, you need to "
            "fetch twice as many bytes as in low resolution mode. For a "
            "normal-sized display, you fetch 80 bytes for each horizontal "
            "line instead of 40' (p.75), 'The data-fetch registers specify "
            "the beginning and end positions for data placement on each "
            "horizontal line of the display' (p.76, this package's own "
            "foundational proposition). 3-ddf.c18, 3-modulo.c19, 3-big.c11, "
            "3-big.c12, 3-large.c18, 3-large.c19, 3-large.c20, 3-large.c21, "
            "3-large.c23.");
    }

    // ---------------- DDF-16-PIXEL-EFFECTIVE-GRANULARITY (p.60) ----------------
    // MEASURED first (not assumed): a fine sweep of raw DDFSTRT values
    // around $0038 showed the STORED bit-field changes at the write-decode's
    // own 2-raw-unit granularity (established directly from the boundary
    // evidence, ddfstrt[8:2]<=data_in[7:1] - data_in bit 0 never read), but
    // the FETCHED WORD COUNT this module actually produces only changes at
    // an 8-raw-unit (8 CCK) step, in BOTH resolutions - not 2, and not 4 in
    // hires either (a finer sweep of the DDFSTRT-DDFSTOP-WORD-COUNT-FORMULA
    // assertion's own hires formula, for every n from 26 to 34, showed EVEN
    // target n reproduced exactly but ODD target n always rounded UP to the
    // next even count - so hires word counts are only ever even at this
    // boundary, a fact MAX-WORDS-49-HIRES-AT-HW-LIMITS below turns into its
    // own finding). This assertion tests the CLEAN, uniformly-behaved lores
    // case: 8 raw units is exactly "programming resolution of 16 pixels (8
    // clocks in low resolution mode...)" (3-ddf.c06/c07) - 16 lores pixels
    // per CCK-pair, matching a CCK = 2 lores pixels.
    {
        auto count_for_ddfstrt = [&](uint16_t ddfstrt_raw) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(DDFSTRT_ADDR, ddfstrt_raw);
            return run_to_line(h, 0x50).count;
        };
        int at_38 = count_for_ddfstrt(0x0038);
        int at_3A = count_for_ddfstrt(0x003A);  // +2 raw units: no effect
        int at_3C = count_for_ddfstrt(0x003C);  // +4 raw units: no effect
        int at_3E = count_for_ddfstrt(0x003E);  // +6 raw units: no effect
        int at_40 = count_for_ddfstrt(0x0040);  // +8 raw units: exactly one word fewer

        checks.expect(
            at_38 == at_3A && at_3A == at_3C && at_3C == at_3E && at_38 == at_3E &&
                at_40 == at_38 - 1,
            "DDF-16-PIXEL-EFFECTIVE-GRANULARITY: moving DDFSTRT from $0038 "
            "by +2, +4 or +6 raw units leaves the fetched word count "
            "UNCHANGED at " +
                std::to_string(at_38) + " (measured: $38=" + std::to_string(at_38) +
                ", $3A=" + std::to_string(at_3A) + ", $3C=" + std::to_string(at_3C) +
                ", $3E=" + std::to_string(at_3E) +
                "), while +8 raw units drops it to exactly " +
                std::to_string(at_40) +
                " (one word fewer) - the fetched-word-count-affecting "
                "resolution is 8 raw units (8 CCK), matching 'to a "
                "programming resolution of 16 pixels (8 clocks in low "
                "resolution mode...)' (p.60) exactly. The register's own "
                "STORED-FIELD addressing is finer still (2 raw units = one "
                "stored step, per the write decode ddfstrt[8:2]<=data_in"
                "[7:1] already in this package's boundary evidence, "
                "data_in bit 0 never read) - '[the] data-fetch registers "
                "have a four-pixel resolution (unlike the display [window "
                "registers, which have a one-pixel resolution])' and 'Each "
                "position specified is four pixels from the last one. "
                "Pixel 0 is position 0; pixel 4 is position 1' (p.60) "
                "describe that STORED addressing convention, distinct from "
                "(and finer than) the word-count-observable granularity "
                "this assertion measures. 'The data-fetch start and "
                "display window starting positions interact with each "
                "other' (p.60) - too vague to test as its own boolean, not "
                "re-tested once the disputed 4.5-clock number underneath "
                "it is set aside (see the DISPUTED measurement record "
                "above). 3-ddf.c06, 3-ddf.c07, 3-ddf.c01, 3-fetch.c1, "
                "3-ddf.c03, 3-fetch.c3, 3-fetch.c4, 3-fetch.c5, 3-ddf.c04, "
                "3-fetch.c6.");
    }

    // ---------------- DIW-HORIZONTAL-ONE-PIXEL-RESOLUTION-VIA-HDE (p.57,59,60) ----------------
    // DIWSTRT/DIWSTOP's horizontal fields reach this module (l.103-104,
    // 186), but feed ONLY `hde` (display/border enable), never `dma`/
    // `address_out`/`reg_address_out` - confirmed by reading every consumer
    // of hdiwstrt/hdiwstop/cur_hdiwstrt/cur_hdiwstop in the file (exactly
    // one, the hde block). So the p.57/59 DIW-horizontal-resolution claims
    // are tested here, on `hde` specifically, separate from every
    // word-count assertion above.
    {
        int at_81 = hde_rising_edge(0x2C81);
        int at_82 = hde_rising_edge(0x2C82);  // +1 raw unit

        checks.expect(
            at_81 >= 0 && at_82 == at_81 + 1,
            "DIW-HORIZONTAL-ONE-PIXEL-RESOLUTION-VIA-HDE: raising DIWSTRT's "
            "H byte by exactly 1 raw unit ($81 -> $82) shifts hde's rising "
            "edge by exactly 1 hpos tick (" +
                std::to_string(at_81) + " -> " + std::to_string(at_82) +
                ") - every raw unit is effective, unlike DDFSTRT where a "
                "change of up to 6 raw units has NO effect on the fetched "
                "word count (DDF-16-PIXEL-EFFECTIVE-GRANULARITY above). "
                "'The resolution of horizontal start and stop is one low "
                "resolution pixel' (p.57), 'the starting position, it is "
                "interpreted in low resolution, non-interlaced [mode]' "
                "(p.59), 'A starting position of ($81,$2C) centers a "
                "normal size display, leaving a border of eight low "
                "resolution pixels around the display window' (p.57, "
                "$81 is this assertion's own base value). This one-raw-"
                "unit DIW step is HALF of DDFSTRT's two-raw-unit STORED-"
                "FIELD step (ddfstrt[8:2]<=data_in[7:1], data_in bit 0 "
                "never read - already in this package's boundary "
                "evidence, no new measurement needed for that half) - "
                "'Recall that the hardware resolution of display window "
                "start and stop is twice the hardware resolution of data "
                "fetch' (p.60). 3-diw.c06, 3-window.c18, 3-window.c9, "
                "3-diw.c24, 3-ddf.c13, 3-fetch.c11.");
    }

    // ---------------- DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE (p.91,277) ----------------
    // Read literally from the RTL (not assumed): the DDFSTRT/DDFSTOP
    // position compare (l.339,349) is `{ddfstrt[8:3], ddfstrt[2] & ecs,
    // 1'b0}`. H3 is register bit 2 of the raw DDFSTRT/DDFSTOP word
    // (Appendix A p.277, "bit 2 = H3"); the write decode
    // ddfstrt[8:2]<=data_in[7:1] (already in this package's boundary
    // evidence) means register bit 2 (data_in[2]) is STORED as ddfstrt[3] -
    // part of the UNCONDITIONALLY-compared `ddfstrt[8:3]` group, gated by
    // neither `ecs` nor the `hires` wire (bplcon0_delayed[4]). The term the
    // compare DOES gate by `ecs` - `ddfstrt[2] & ecs` - is STORED bit
    // ddfstrt[2], which holds register bit 1 (data_in[1]): outside the
    // manual's H8-H3 bit map entirely ("Bits 1-0 - not used", p.91/277),
    // and NOT H3 - corrected here (NM-HRM-006, 24.09.2026) after this
    // package's own earlier draft called ddfstrt[2] "H3" and stimulated
    // register bit 1 ($38/$3A, $3C/$3E) instead. This tests H3 itself -
    // toggling ONLY register bit 2 ($0038 -> $003C, which differ in raw
    // value by exactly 0x0004 and nothing else), the SAME pair in both
    // lores and hires so any lores/hires difference in the result is H3's
    // own mode-gating and not a difference in which bit is toggled - both
    // axes independently (ecs on/off, hires on/off), via first-fetch hpos
    // timing (DDF-16-PIXEL-EFFECTIVE-GRANULARITY already showed total WORD
    // COUNT cannot show a single-step change - only the first fetch's exact
    // hpos timing can). A companion MEASURED note below records register
    // bit 1's own (ecs-gated, not H3's) effect with the SAME $0038/$003A
    // pair this assertion no longer uses for H3.
    {
        auto first_hpos = [&](bool ecs_on, bool hires_on, uint16_t ddfstrt) {
            Harness h;
            h.setup_standard(ecs_on);
            h.write_reg(DDFSTOP_ADDR, hires_on ? DDFSTOP_HIRES_STD : DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(hires_on, 1));
            h.write_reg(DDFSTRT_ADDR, ddfstrt);
            return first_fetch_hpos(h, 0x50);
        };
        // $0038 -> $003C: the two raw values differ in exactly one bit,
        // 0x0004 = register bit 2 = H3 (Appendix A p.277) - the SAME pair
        // in lores and in hires.
        int lo_ecs1_a = first_hpos(true, false, 0x0038), lo_ecs1_b = first_hpos(true, false, 0x003C);
        int lo_ecs0_a = first_hpos(false, false, 0x0038), lo_ecs0_b = first_hpos(false, false, 0x003C);
        int hi_ecs1_a = first_hpos(true, true, 0x0038), hi_ecs1_b = first_hpos(true, true, 0x003C);
        int hi_ecs0_a = first_hpos(false, true, 0x0038), hi_ecs0_b = first_hpos(false, true, 0x003C);

        // The ORACLE is the manual's own claim: H3 changes timing ONLY in
        // hires mode, REGARDLESS of ecs - i.e. shifts in both hires cases,
        // shifts in NEITHER lores case. Unchanged from this assertion's
        // earlier draft - only the stimulus above (which bit is toggled)
        // was wrong.
        bool matches_manual =
            (hi_ecs1_a != hi_ecs1_b) && (hi_ecs0_a != hi_ecs0_b) &&
            (lo_ecs1_a == lo_ecs1_b) && (lo_ecs0_a == lo_ecs0_b);

        checks.expect(
            matches_manual,
            "DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE: the manual's own "
            "claim is that DDFSTRT bit H3 (register bit 2, raw $0004) "
            "changes fetch timing ONLY in hires mode, regardless of chip "
            "configuration - '(bit H3 only respected in Hires Mode.)' "
            "(p.91/277). MEASURED instead (four cases, the SAME $0038-> "
            "$003C pair - differing only in register bit 2/H3 - in every "
            "case): lores/ecs=1 " +
                std::to_string(lo_ecs1_a) + "->" + std::to_string(lo_ecs1_b) +
                ", lores/ecs=0 " + std::to_string(lo_ecs0_a) + "->" + std::to_string(lo_ecs0_b) +
                ", hires/ecs=1 " + std::to_string(hi_ecs1_a) + "->" + std::to_string(hi_ecs1_b) +
                ", hires/ecs=0 " + std::to_string(hi_ecs0_a) + "->" + std::to_string(hi_ecs0_b) +
                " - this RTL's DDFSTRT/DDFSTOP position compare (l.339,349) "
                "reads `{ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0}`: H3 "
                "(ddfstrt[3], register bit 2) is part of the "
                "UNCONDITIONALLY-compared `ddfstrt[8:3]` group - gated "
                "neither by `ecs` nor by the `hires` wire "
                "(bplcon0_delayed[4]). So H3 actually shifts timing in "
                "EVERY case measured here, lores included (which the "
                "manual's mode rule says should NOT shift), not only in "
                "hires mode. RED against the manual's own wording - per "
                "this package's instructions, the RTL is what is wrong "
                "here, not the assertion. 3-rddf.c02, 3-rddf.c03, "
                "3-regsum-ddfstrt.c4, 3-regsum-ddfstop.c4, A-ddf-bits.c14.");

        // Register bit 1 (ddfstrt[2], data_in[1]) has no manual claim of
        // its own - Appendix A/p.91 mark register bits 1-0 "not used" - so
        // its measured effect is recorded as a MEASUREMENT, not graded
        // against any claim uid.
        int b1_lo_ecs1_b = first_hpos(true, false, 0x003A);
        int b1_lo_ecs0_b = first_hpos(false, false, 0x003A);
        int b1_hi_ecs1_b = first_hpos(true, true, 0x003A);
        int b1_hi_ecs0_b = first_hpos(false, true, 0x003A);
        checks.note(
            "DDFSTRT register bit 1 (ddfstrt[2], data_in[1], raw $0002) - "
            "outside the manual's H8-H3 bit map, p.91/277 mark bits 1-0 "
            "'not used'. $0038->$003A first-fetch hpos: lores/ecs=1 " +
                std::to_string(lo_ecs1_a) + "->" + std::to_string(b1_lo_ecs1_b) +
                ", lores/ecs=0 " + std::to_string(lo_ecs0_a) + "->" + std::to_string(b1_lo_ecs0_b) +
                ", hires/ecs=1 " + std::to_string(hi_ecs1_a) + "->" + std::to_string(b1_hi_ecs1_b) +
                ", hires/ecs=0 " + std::to_string(hi_ecs0_a) + "->" + std::to_string(b1_hi_ecs0_b));
    }

    // ---------------- DDF-REGISTER-BIT-STRUCTURE-AND-NAMED-PRESETS (p.91,277,290) ----------------
    // MEASURED first: bits 15-8 set to all-1 garbage alongside a valid
    // pattern in bits 7-1 produce IDENTICAL word counts to the same
    // pattern alone (both DDFSTRT and DDFSTOP) - confirms "not used".
    // Appendix A's four named DDFSTRT presets and three named DDFSTOP
    // presets decode (H8-H4 in bits 7-3, as the write-decode's own
    // data_in[7:1]->ddfstrt[8:2] mapping already establishes) to $0028/
    // $0030/$0038/$0040 and $00C8/$00D0/$00D8 respectively - each measured
    // here to give a DIFFERENT, correctly-ordered word count (proving they
    // are real, distinct, meaningfully-ordered positions, not just
    // arbitrary labels), and $0038/$00D0 ('Normal') and $00D8 ('Wide
    // (max)') are exactly LORES-STANDARD's and MAX-WORDS-25-LORES's own
    // values, re-confirming the identity from a second, independent
    // starting point (Appendix A's own preset table) rather than only
    // Chapter 3's prose.
    {
        auto count_for = [&](uint16_t ddfstrt, uint16_t ddfstop) {
            Harness h;
            h.setup_standard();
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(DDFSTRT_ADDR, ddfstrt);
            h.write_reg(DDFSTOP_ADDR, ddfstop);
            return run_to_line(h, 0x50).count;
        };
        int strt_xwide = count_for(0x0028, DDFSTOP_LORES_STD);   // Extra wide (max)
        int strt_wide  = count_for(0x0030, DDFSTOP_LORES_STD);   // Wide
        int strt_norm  = count_for(0x0038, DDFSTOP_LORES_STD);   // Normal
        int strt_narrow= count_for(0x0040, DDFSTOP_LORES_STD);   // Narrow
        int stop_narrow= count_for(DDFSTRT_HW_LIMIT, 0x00C8);    // Narrow
        int stop_norm  = count_for(DDFSTRT_HW_LIMIT, 0x00D0);    // Normal
        int stop_wide  = count_for(DDFSTRT_HW_LIMIT, 0x00D8);    // Wide (max)

        int plain = count_for(0x0038, DDFSTOP_LORES_STD);
        int strt_hi_garbage = count_for(0xFF38, DDFSTOP_LORES_STD);
        int stop_hi_garbage = count_for(0x0038, 0xFFD0);

        bool presets_ordered =
            strt_xwide > strt_wide && strt_wide > strt_norm && strt_norm > strt_narrow &&
            stop_narrow < stop_norm && stop_norm < stop_wide;
        bool bits_unused = strt_hi_garbage == plain && stop_hi_garbage == plain;

        checks.expect(
            presets_ordered && bits_unused,
            "DDF-REGISTER-BIT-STRUCTURE-AND-NAMED-PRESETS: Appendix A's "
            "four named DDFSTRT presets give strictly DECREASING word "
            "counts as they narrow (Extra wide $0028=" + std::to_string(strt_xwide) +
                " > Wide $0030=" + std::to_string(strt_wide) + " > Normal $0038=" +
                std::to_string(strt_norm) + " > Narrow $0040=" + std::to_string(strt_narrow) +
                "), and DDFSTOP's three named presets give strictly INCREASING "
                "counts as they widen (Narrow $00C8=" + std::to_string(stop_narrow) +
                " < Normal $00D0=" + std::to_string(stop_norm) + " < Wide (max) $00D8=" +
                std::to_string(stop_wide) +
                ") - real, distinct, correctly-ordered positions, not "
                "arbitrary labels; 'Normal' ($0038/$00D0) and DDFSTOP's "
                "'Wide (max)' ($00D8) are exactly LORES-STANDARD-20-WORDS' "
                "and MAX-WORDS-25-LORES-AT-HW-LIMITS' own already-verified "
                "values, confirmed here from Appendix A's independent "
                "starting point. Garbage in bits 15-8 changes NOTHING (" +
                std::to_string(plain) + " either way) - 'Bits 15-8 - not "
                "used' - confirming the unused-bits half of 'These "
                "registers control the horizontal timing of the beginning "
                "and end of the bitplane DMA display data fetch' (p.277) "
                "and 'DDFSTRT 092 W...' / 'DDFSTOP 094 W...' (p.290, "
                "identity already exercised by every DDFSTRT_ADDR/"
                "DDFSTOP_ADDR write in this whole file). 3-rddf.c01, "
                "3-rddf.c04, 3-rddf.c05, 3-rddf.c06, 3-rddf.c07, "
                "3-regsum-ddfstop.c1, 3-regsum-ddfstop.c2, "
                "3-regsum-ddfstop.c3, 3-regsum-ddfstop.c5, "
                "3-regsum-ddfstrt.c1, 3-regsum-ddfstrt.c2, "
                "3-regsum-ddfstrt.c3, 3-regsum-ddfstrt.c5, 3-t14.c11, "
                "3-max-window.c29, A-ddf-bits.c1, A-ddf-bits.c2, "
                "A-ddf-bits.c3, A-ddf-bits.c4, A-ddf-bits.c5, A-ddf-bits.c6, "
                "A-ddf-bits.c7, A-ddf-bits.c8, A-ddf-bits.c9, A-ddf-bits.c10, "
                "A-ddf-bits.c11, A-ddf-bits.c12, A-ddf-bits.c13, "
                "A-ddf-bits.c15, A-ddf-bits.c16, A-ddf.bits1, A-ddf.bits2, "
                "A-ddf.c01, A-ddf.c03, A-ddf.c5, A-ddf.c6, A-ddf.c9, "
                "A-ddf.c10, A-ddf.c11, A-ddf.c12, A-ddf.c13, A-ddf.c14, "
                "A-ddf.c15, A-ddf.c16, A-ddf.c17, A-ddfstrt.fn, "
                "A-ddfstrt.n, A-ddfstrt.nw, A-ddfstrt.w, A-ddfstrt.xw, "
                "A-ddfstop.fn, A-ddfstop.n, A-ddfstop.nw, A-ddfstop.w, "
                "B-subject.c18, B-subject.c21.");
    }

    // ---------------- DDFSTRT-DDFSTOP-WORD-COUNT-FORMULA (p.60) ----------------
    {
        // Low resolution: DDFSTRT = DDFSTOP - 8*(n-1). Pick n=15 (not the
        // n=20 standard case) to test the formula's own arithmetic, not
        // just repeat the pre-supplied measurement.
        constexpr int kLoresN = 15;
        uint16_t ddfstrt_lores = static_cast<uint16_t>(DDFSTOP_LORES_STD - 8 * (kLoresN - 1));
        Harness hl;
        hl.setup_standard();
        hl.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        hl.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        hl.write_reg(DDFSTRT_ADDR, ddfstrt_lores);
        int lores_count = run_to_line(hl, 0x50).count;

        // High resolution: DDFSTRT = DDFSTOP - 4*(n-2). Pick n=30.
        constexpr int kHiresN = 30;
        uint16_t ddfstrt_hires = static_cast<uint16_t>(DDFSTOP_HIRES_STD - 4 * (kHiresN - 2));
        Harness hh;
        hh.setup_standard();
        hh.write_reg(DDFSTOP_ADDR, DDFSTOP_HIRES_STD);
        hh.write_reg(BPLCON0_ADDR, bplcon0_word(true, 1));
        hh.write_reg(DDFSTRT_ADDR, ddfstrt_hires);
        int hires_count = run_to_line(hh, 0x50).count;

        char lores_hex_buf[8];
        snprintf(lores_hex_buf, sizeof(lores_hex_buf), "%02X", ddfstrt_lores);
        char hires_hex_buf[8];
        snprintf(hires_hex_buf, sizeof(hires_hex_buf), "%02X", ddfstrt_hires);
        std::string lores_hex(lores_hex_buf);
        std::string hires_hex(hires_hex_buf);

        checks.expect(
            lores_count == kLoresN && hires_count == kHiresN,
            "DDFSTRT-DDFSTOP-WORD-COUNT-FORMULA: for a NON-standard word "
            "count (n=15 lores, n=30 hires - not the n=20/n=40 standard "
            "case LORES-STANDARD/HIRES-STANDARD already cover), computing "
            "DDFSTRT from DDFSTOP via 'DDFSTRT = DDFSTOP - (8 * (word "
            "count - 1)) for low resolution' / '...(4 * (word count - 2)) "
            "for high resolution' (p.60) and writing it gives EXACTLY the "
            "targeted word count back (lores: $D0-8*14=$" + lores_hex +
                " -> " + std::to_string(lores_count) + " words; hires: $D4-4*28=$" +
                hires_hex + " -> " + std::to_string(hires_count) +
                " words), entirely within the DDF domain - no disputed "
                "DIW offset needed. 'DDFSTRT = DDFSTOP - (8 * (word count "
                "- 1)) for low resolution' / '...(4 * (word count - 2)) "
                "for high resolution' (p.60). 3-ddf.c14, 3-ddf.c15, "
                "3-fetch.c12, 3-fetch.c13.");
    }

    // ---------------- HW-LIMIT-DDFSTRT-0018-LEFTMOST (p.80) ----------------
    {
        auto count_for = [&](uint16_t ddfstrt_raw) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(DDFSTRT_ADDR, ddfstrt_raw);
            return run_to_line(h, 0x50).count;
        };
        int at_limit = count_for(DDFSTRT_HW_LIMIT);  // $0018
        int below_1 = count_for(0x0010);
        int below_2 = count_for(0x0000);

        checks.expect(
            below_1 == at_limit && below_2 == at_limit,
            "HW-LIMIT-DDFSTRT-0018-LEFTMOST: DDFSTRT programmed below the "
            "hardware limit ($0010, $0000) fetches the SAME " +
                std::to_string(at_limit) +
                " words as DDFSTRT=$0018 itself - the leftmost limit "
                "clamps rather than extending the window further. "
                "'Strictly speaking, the hardware sets a...leftmost limit "
                "to DDFSTRT of ($18)' (p.80). 3-max-window.c10, "
                "3-max-window.c11, 3-max.c03, 3-max.c04, 3-max-window.c23, "
                "3-max-window.c24, 3-t14.c05, 3-t14.c06.");
    }

    // ---------------- HW-LIMIT-DDFSTOP-00D8-RIGHTMOST (p.80) ----------------
    // MEASURED first: a DDFSTOP sweep from $D0 up past $D8 showed a clean
    // clamp plateau from $D8 through $E0 (all identical counts), but from
    // $E4 onward the count jumps to a DIFFERENT, higher value - matching
    // this exact file's OWN comments (l.146-147: "ECS: DDFSTOP = $E2($E3)
    // display data fetch stopped ($00 stops the display as well)" / "ECS:
    // DDFSTOP = $E4 display data fetch not stopped") - a separate,
    // documented special case, not the plain "clamps at the limit"
    // behaviour this assertion tests. Above-limit test values are chosen
    // inside the clamp plateau ($DC, $E0), clear of that documented $E4+
    // special case.
    {
        auto count_for = [&](uint16_t ddfstop_raw) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(DDFSTOP_ADDR, ddfstop_raw);
            return run_to_line(h, 0x50).count;
        };
        int at_limit = count_for(DDFSTOP_HW_LIMIT);  // $00D8
        int above_1 = count_for(0x00DC);
        int above_2 = count_for(0x00E0);

        checks.expect(
            above_1 == at_limit && above_2 == at_limit,
            "HW-LIMIT-DDFSTOP-00D8-RIGHTMOST: DDFSTOP programmed above the "
            "hardware limit ($00DC, $00E0) fetches the SAME " +
                std::to_string(at_limit) +
                " words as DDFSTOP=$00D8 itself - the rightmost limit "
                "clamps rather than extending the window further. "
                "'...the hardware sets a rightmost limit to DDFSTOP of "
                "($D8)...' (p.80). 3-max-window.c10, 3-max.c03, "
                "3-max-window.c25, 3-max-window.c26, 3-t14.c07, "
                "3-t14.c08.");
    }

    // ---------------- MAX-WORDS-25-LORES-AT-HW-LIMITS (p.80) ----------------
    {
        Harness hl;
        hl.setup_standard();
        hl.write_reg(DDFSTRT_ADDR, DDFSTRT_HW_LIMIT);
        hl.write_reg(DDFSTOP_ADDR, DDFSTOP_HW_LIMIT);
        hl.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        int lores_max = run_to_line(hl, 0x50).count;

        checks.expect(
            lores_max == 25,
            "MAX-WORDS-25-LORES-AT-HW-LIMITS: DDFSTRT=$0018, DDFSTOP=$00D8 "
            "(both hardware limits at once), lores fetches exactly " +
                std::to_string(lores_max) +
                " words - 'This gives a maximum of 25 words fetched in low "
                "resolution mode' (p.80). 3-max-window.c12, 3-max.c04, "
                "3-max.c05, 3-t14.c09, 3-max-window.c19, 3-max-window.c20, "
                "3-max-window.c21, 3-max-window.c22, 3-t14.c01, 3-t14.c02, "
                "3-t14.c03, 3-t14.c04.");
    }

    // ---------------- MAX-WORDS-49-HIRES-AT-HW-LIMITS (p.80) ----------------
    // MEASURED first: sweeping the DDFSTRT-DDFSTOP-WORD-COUNT-FORMULA
    // formula's own hires case for every target n from 26 to 34 showed EVEN
    // n reproduced exactly but ODD n always rounds UP to the next even
    // count (n=29's formula-computed DDFSTRT measured 30, not 29; n=31
    // measured 32; n=33 measured 34) - this RTL's hires word count is
    // structurally always EVEN. 49 is odd, so it is not reachable by any
    // DDFSTRT/DDFSTOP pair in this implementation, hardware-limited or not;
    // the nearest achievable values bracketing it are 48 and 50.
    {
        Harness hh;
        hh.setup_standard();
        hh.write_reg(DDFSTRT_ADDR, DDFSTRT_HW_LIMIT);
        hh.write_reg(DDFSTOP_ADDR, DDFSTOP_HW_LIMIT);
        hh.write_reg(BPLCON0_ADDR, bplcon0_word(true, 1));
        int hires_max = run_to_line(hh, 0x50).count;

        checks.expect(
            hires_max == 49,
            "MAX-WORDS-49-HIRES-AT-HW-LIMITS: DDFSTRT=$0018, DDFSTOP=$00D8 "
            "(both hardware limits at once), hires fetches " +
                std::to_string(hires_max) +
                " words, not the 49 the manual states - 'In high "
                "resolution mode the maximum here is 49 words, because the "
                "rightmost limit remains ($D8) and only one word is "
                "fetched at this limit' (p.80). Measured separately (see "
                "this assertion's own comment above and the MUTATION LOG): "
                "this RTL's hires fetched-word-count is structurally "
                "always EVEN (confirmed by sweeping the word-count formula "
                "for n=26..34 - every odd target rounds up to the next "
                "even count), so 49 is unreachable by ANY DDFSTRT/DDFSTOP "
                "pair here, not only at the hardware limits - the '...and "
                "only one word is fetched at this limit' mechanism the "
                "manual describes to produce an odd count from an "
                "otherwise-even progression is not implemented in this "
                "RTL. RED against the manual's own stated number - per "
                "this package's instructions, the RTL is what is wrong "
                "here, not the assertion. 3-max-window.c13, 3-max.c06, "
                "3-max.c07, 3-t14.c10.");
    }

    // ---------------- MAX-WORDS-49-HIRES-ONE-WORD-AT-LIMIT: MEASUREMENT, NOT ASSERTION (p.80) ----------------
    // NM-HRM-007 (24.09.2026): DECLINED here after an independent review
    // found this assertion's own pass condition self-contradictory. It
    // read `at_49==49 && at_49!=at_48 && at_49!=at_50`, where at_50 (the
    // p.60 formula's own DDFSTRT for target n=50, DDFSTOP_HW_LIMIT-4*48 =
    // $0018) is EXACTLY DDFSTRT_HW_LIMIT - the SAME stimulus
    // MAX-WORDS-49-HIRES-AT-HW-LIMITS above already grades as
    // `hires_max==49`. On any RTL where that assertion passes
    // (hires_max==49), at_50 here would also measure 49, so this
    // assertion's own `at_49 != at_50` conjunct would require 49 != 49 -
    // false. No RTL could ever make both assertions green at once. Its
    // premise - that n=49 is individually reachable at some OTHER
    // DDFSTRT, distinguishable from both n=48 and n=50 - is not a claim
    // the printed sentence makes: 'In high resolution mode the maximum
    // here is 49 words, because the rightmost limit remains ($D8) and
    // only one word is fetched at this limit' (p.80) fixes 49 as the
    // maximum AT EXACTLY DDFSTRT=$0018/DDFSTOP=$00D8 (MAX-WORDS-49-HIRES-
    // AT-HW-LIMITS' own claim, still red: this RTL measures 50 there) and
    // says nothing about the count at any OTHER start position with
    // DDFSTOP=$00D8 held fixed. Measured and declined below instead.
    {
        auto count_for_n = [&](int n) {
            uint16_t ddfstrt = static_cast<uint16_t>(DDFSTOP_HW_LIMIT - 4 * (n - 2));
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_HW_LIMIT);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(true, 1));
            h.write_reg(DDFSTRT_ADDR, ddfstrt);
            return run_to_line(h, 0x50).count;
        };
        int at_48 = count_for_n(48);
        int at_49 = count_for_n(49);
        int at_50 = count_for_n(50);

        checks.note(
            "at the SAME DDFSTOP=$00D8 rightmost limit, the p.60 formula's "
            "own DDFSTRT for n=48/49/50 measures " +
                std::to_string(at_48) + "/" + std::to_string(at_49) + "/" +
                std::to_string(at_50) +
                " words (n=50's own computed DDFSTRT is $0018 = "
                "DDFSTRT_HW_LIMIT - the SAME stimulus MAX-WORDS-49-HIRES-"
                "AT-HW-LIMITS grades above).");
        checks.record(
            "MAX-WORDS-49-HIRES-ONE-WORD-AT-LIMIT is DECLINED, not graded. "
            "'In high resolution mode the maximum here is 49 words, "
            "because the rightmost limit remains ($D8) and only one word "
            "is fetched at this limit' (p.80) fixes 49 as the maximum AT "
            "EXACTLY DDFSTRT=$0018/DDFSTOP=$00D8 - MAX-WORDS-49-HIRES-AT-"
            "HW-LIMITS' own claim, graded above. It says nothing about the "
            "count at any OTHER DDFSTRT with DDFSTOP=$00D8 held fixed. "
            "This assertion's former premise needed n=49 individually "
            "reachable at such an OTHER position, distinguishable from "
            "both n=48 and n=50 - not a claim the sentence makes, and "
            "self-contradictory besides: its own n=50 sample IS the "
            "$18/$D8 hardware-limit stimulus MAX-WORDS-49-HIRES-AT-HW-"
            "LIMITS already grades, so on any RTL where that assertion "
            "measures 49 (its own pass condition), this one's required "
            "at_49!=at_50 would demand 49!=49 - the two could never both "
            "be green. 3-max-window.c14.");
    }

    // ---------------- HSCROLL-EXTRA-WORD-21-FOR-20 (p.80,82,83,84,85) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, 0x0030);  // one word (8 CCK) earlier than $0038
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);  // unchanged
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));

        prime_to_line(h, 0x4F);
        h.write_ptr(0, 0x010000);  // fresh, known pointer for plane 0
        LineResult r = run_single_line(h, 0x50);

        bool starts_at_base = !r.fetches.empty() && r.fetches.front().address == 0x010000;

        checks.expect(
            r.count == 21 && starts_at_base,
            "HSCROLL-EXTRA-WORD-21-FOR-20: DDFSTRT=$0030 ('the data fetch "
            "must start one word sooner', p.82), DDFSTOP UNCHANGED at "
            "$00D0, lores, BPU=1 fetches 21 words (42 bytes) where "
            "LORES-STANDARD's unscrolled $0038 fetches 20 (40 bytes) - the "
            "extra word's own address is the FIRST one fetched, i.e. "
            "placed before/left of the normal fetch (Figure 3-24's '16 "
            "bits (1 word)' gap and 'Data Fetch start' ticked left of "
            "'Display Window start'). 'Delay means that an extra word of "
            "data is fetched but not immediately displayed. The extra word "
            "is placed just to the left of the window's leftmost edge and "
            "before normal data fetch' / 'The normal data-fetch start for "
            "non-scrolled displays is ($38). If horizontal scrolling is "
            "desired, then the data fetch must start one word sooner "
            "(DDFSTRT = $0030)... DDFSTOP remains unchanged... scrolling "
            "requires a data fetch of two extra bytes, the data for each "
            "line will be 42 bytes long' (p.82), 'Fetch one extra word per "
            "horizontal line and start it 16 pixels before the normal "
            "(unscrolled) data-fetch start' (p.85), Figure 3-24 (p.83) and "
            "Figure 3-26 (p.84). 3-scr.c04, 3-scr.c05, 3-scroll.c5, "
            "3-scroll.c6, 3-hscr.c02, 3-hscr.c03, 3-hscr.c04, 3-hscr.c08, "
            "3-hscr.c09, 3-hscr.c11, 3-hscr.c12, 3-hscr.c15, 3-hscr.c16, "
            "3-hscroll.c1, 3-hscroll.c2, 3-hscroll.c3, 3-hscroll.c5, "
            "3-hscroll.c7, 3-hscroll.c10, 3-hscroll.c11, 3-hscroll.c13, "
            "3-hscroll.c14, 3-hscroll.c16, 3-hscr.f01, 3-hscr.f02, "
            "3-hscr.f03, 3-hscr.f05, 3-hscr.f06, 3-hscr.f08, "
            "3-hscroll.c18, 3-hscroll.c19, 3-hscr.f10, 3-hscroll.c23, "
            "3-hscroll.c26, 3-hscr.c27, 3-hscr.c28, 3-hscroll.c34, "
            "3-hscroll.c35.");
    }

    // ---------------- DUAL-PLAYFIELD-SHARED-DDF-REGISTERS (p.72) ----------------
    {
        Harness h;
        h.setup_standard();
        // ONE shared DDFSTRT/DDFSTOP pair - never programmed twice, never a
        // second register address used - servicing BOTH playfields' planes
        // (BPU=2: one odd/plane0, one even/plane1 bitplane, the minimal
        // dual-playfield BPU split) at once.
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 2));

        LineResult r = run_to_line(h, 0x50);
        std::map<uint16_t, int> per_plane;
        for (auto& f : r.fetches) per_plane[f.reg]++;

        checks.expect(
            r.count == 40 && per_plane.size() == 2 &&
                per_plane.count(0x88) && per_plane[0x88] == 20 &&
                per_plane.count(0x89) && per_plane[0x89] == 20,
            "DUAL-PLAYFIELD-SHARED-DDF-REGISTERS: the SAME DDFSTRT=$0038/"
            "DDFSTOP=$00D0 pair used for LORES-STANDARD's single bitplane, "
            "unchanged, services BOTH bitplanes of a 2-plane (one "
            "odd/one even, the minimal dual-playfield split) "
            "configuration at once - 20 words each (40 total), not a "
            "second window for the second plane. 'Only one data-fetch "
            "start register and one data-fetch stop register are "
            "available, and these are shared by both playfields' (p.72). "
            "3-dual-scroll.c3, 3-dual-scroll.c4, 3-dual.c36, 3-dual.c37.");
    }

    // ---------------- DUAL-PLAYFIELD-SCROLL-MODULO-COMPENSATION (p.72,85) ----------------
    // MEASURED first: with the SAME shared DDFSTRT shifted scroll-early
    // ($0030, +1 word/+2 bytes for BOTH planes since they share one
    // register pair - HSCROLL-EXTRA-WORD-21-FOR-20's own mechanism, now
    // applied to a 2-plane dual-playfield config), the plane being held
    // STATIONARY still fetches the wider 21-word window it does not need -
    // given a pointer offset by -2 bytes (so its FIRST fetch still lands on
    // its own true start) and a modulo of -2 bytes, its line-to-line
    // address step comes back to exactly 40 bytes, IDENTICAL to a plain
    // unscrolled single-plane reference (END-OF-LINE-MODULO-AFTER-LAST-
    // WORD's own BPL1MOD=0 case) - the -2/-2 compensation exactly cancels
    // the shared scroll's extra word, so the stationary plane behaves as
    // if untouched by the other plane's scrolling.
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, 0x0030);  // shared, scroll-shifted (HSCROLL's own value)
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 2));  // BPU=2: plane0 scrolls, plane1 held
        h.write_reg(BPL2MOD_ADDR, 0xFFFE);  // -2 bytes, plane1's (even) modulo

        prime_to_line(h, 0x4E);
        constexpr uint32_t kBase = 0x040000;
        h.write_ptr(1, kBase - 2);  // -2 bytes, plane1's pointer compensation

        LineResult line1 = run_single_line(h, 0x4F);
        LineResult line2 = run_single_line(h, 0x50);
        auto plane1_of = [](const LineResult& r) {
            std::vector<uint32_t> out;
            for (auto& f : r.fetches) if (f.reg == 0x89) out.push_back(f.address);
            return out;
        };
        std::vector<uint32_t> p1_line1 = plane1_of(line1);
        std::vector<uint32_t> p1_line2 = plane1_of(line2);

        checks.expect(
            p1_line1.size() == 21 && p1_line2.size() == 21 && !p1_line1.empty() &&
                p1_line1.front() == kBase - 2 && !p1_line2.empty() &&
                p1_line2.front() - p1_line1.front() == 40,
            "DUAL-PLAYFIELD-SCROLL-MODULO-COMPENSATION: plane1 (held "
            "stationary) still fetches " +
                std::to_string(p1_line1.size()) +
                " words under the shared scroll-shifted window (same as "
                "the scrolling plane), but with its pointer and modulo "
                "each offset by -2 bytes its line-to-line address step "
                "comes back to exactly " +
                std::to_string(p1_line2.front() - p1_line1.front()) +
                " bytes - IDENTICAL to a plain unscrolled single-plane "
                "reference (END-OF-LINE-MODULO-AFTER-LAST-WORD's own "
                "BPL1MOD=0 case, 40 bytes) - 'When you are scrolling low "
                "resolution playfields, you must fetch one word more than "
                "the width of the playfield you are trying to scroll' and "
                "'Only one data-fetch start register and one data-fetch "
                "stop register are available, and these are shared by "
                "both playfields' (p.72), 'In low resolution mode, you "
                "adjust the pointers by -2 and the modulo by -2' (p.72), "
                "'The modulo is two counts less than when there is no "
                "scrolling' (p.85). 3-dual-scroll.c1, 3-dual-scroll.c2, "
                "3-dual.c34, 3-dual-scroll.c6, 3-dual-scroll.c8, "
                "3-dual.c40, 3-dual.c42, 3-hscr.c29, 3-hscroll.c36.");
    }

    // ---------------- END-OF-LINE-MODULO-AFTER-LAST-WORD (p.61,81) ----------------
    {
        auto line_gap = [&](uint16_t modulo) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(BPL1MOD_ADDR, modulo);
            prime_to_line(h, 0x4E);
            h.write_ptr(0, 0x020000);
            LineResult line1 = run_single_line(h, 0x4F);
            LineResult line2 = run_single_line(h, 0x50);
            return std::make_tuple(line1.count, line2.count,
                                    line2.fetches.front().address - line1.fetches.front().address);
        };
        auto [count1_m0, count2_m0, gap_m0] = line_gap(0x0000);
        auto [count1_m40, count2_m40, gap_m40] = line_gap(0x0028);  // 40 decimal

        checks.expect(
            count1_m0 == 20 && count2_m0 == 20 && gap_m0 == 40 &&
                count1_m40 == 20 && count2_m40 == 20 && gap_m40 == 80,
            "END-OF-LINE-MODULO-AFTER-LAST-WORD: BPL1MOD=0 gives a 40-byte "
            "line-to-line address jump (20 words already advance the "
            "pointer 40 bytes on their own); BPL1MOD=40 gives an 80-byte "
            "jump (the SAME 40 bytes plus the 40-byte modulo) - the modulo "
            "is added ONCE, after the last word, changing where the NEXT "
            "line starts, never the CURRENT line's own word count (both "
            "runs fetch exactly 20 words on both lines) - 'When the "
            "end-of-line condition is reached (defined by the data-fetch "
            "register, [DDFSTOP]) the modulo is added to the bitplane "
            "pointers, adjusting the pointer to the first word to be "
            "fetched for the next horizontal line' (p.61), 'Screen data "
            "fetch stops (DDFSTOP) for each horizontal line after the "
            "last word on the line has been fetched' (Figure 3-10, p.61), "
            "'the pointers are continuously incremented to point [to the "
            "next word]' (p.61), 'For a normal-sized, low resolution "
            "display in which the modulo is 0, the pointers would be "
            "incremented by 40 bytes each time' (p.81). 3-mod.c04, "
            "3-mod.f02, 3-modulo.c5, 3-modulo.c9, 3-ptr.c09, "
            "3-vscroll.c9.");
    }

    // ---------------- WIDER-PLAYFIELD-FIRST-LINE-UNCHANGED (p.73,74) ----------------
    {
        auto first_line_addrs = [&](uint16_t modulo) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(BPL1MOD_ADDR, modulo);
            prime_to_line(h, 0x4F);
            h.write_ptr(0, 0x030000);
            LineResult line = run_single_line(h, 0x50);
            std::vector<uint32_t> addrs;
            for (auto& f : line.fetches) addrs.push_back(f.address);
            return addrs;
        };
        std::vector<uint32_t> addrs_m0 = first_line_addrs(0x0000);
        std::vector<uint32_t> addrs_m40 = first_line_addrs(0x0028);  // wider RAM line (modulo=40)

        bool same_span = addrs_m0.size() == 20 && addrs_m40.size() == 20 &&
            addrs_m0 == addrs_m40 &&
            addrs_m0.front() == 0x030000 && addrs_m0.back() == 0x030000 + 2 * 19;

        checks.expect(
            same_span,
            "WIDER-PLAYFIELD-FIRST-LINE-UNCHANGED: with BPL1MOD=40 (a "
            "wider RAM line than the displayed 40-byte line, the p.73/74 "
            "scenario) the FIRST line's own 20 fetched addresses "
            "(START..START+38) are IDENTICAL to the BPL1MOD=0 case - a "
            "nonzero modulo changes where LINE 2 starts (per "
            "END-OF-LINE-MODULO-AFTER-LAST-WORD above), never line 1 "
            "itself. 'Figure 3-14: 80-byte RAM line, left half (40 bytes) "
            "displayed' and 'Because 40 bytes are to be fetched for each "
            "line, the data fetch for line 1 is as shown in Figure 3-15' - "
            "'Data Fetch for the First Line When Modulo = 40', locations "
            "START..START+38 (p.73-74). 3-big.f01, 3-large.c5, 3-big.f02, "
            "3-large.c6, 3-large.c8.");
    }

    // ---------------- HIRES-BPU-CEILING-MAX-4-PLANES (p.49) ----------------
    // This package's own prior "NO HIRES BPU CEILING" note had read only
    // the `bpu` register term (l.300) and missed the plane encoder
    // (l.457-461):
    // in hires+FMODE0 the encoder itself is `{3'b000,~ddfseq[0],~ddfseq[1]}`
    // - two bits wide, so `plane` can never reach 4 or 5 no matter what
    // `bpu` says. See this file's IMPLEMENTATION NOTES above main() for the
    // corrected note and the p.48/p.90 manual correction that went with it.
    {
        Harness hh;
        hh.setup_standard();
        hh.write_reg(DDFSTRT_ADDR, DDFSTRT_HIRES_STD);
        hh.write_reg(DDFSTOP_ADDR, DDFSTOP_HIRES_STD);
        hh.write_reg(BPLCON0_ADDR, bplcon0_word(true, 6));  // hires, BPU=6 requested
        LineResult hi = run_to_line(hh, 0x50);

        Harness hl;
        hl.setup_standard();
        hl.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        hl.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        hl.write_reg(BPLCON0_ADDR, bplcon0_word(false, 6));  // lores, BPU=6 - control
        LineResult lo = run_to_line(hl, 0x50);

        bool hi_seen[8] = {false, false, false, false, false, false, false, false};
        for (auto& f : hi.fetches) hi_seen[(f.reg - 0x88) & 7] = true;
        bool lo_seen[8] = {false, false, false, false, false, false, false, false};
        for (auto& f : lo.fetches) lo_seen[(f.reg - 0x88) & 7] = true;

        bool hires_only_0to3 = hi_seen[0] && hi_seen[1] && hi_seen[2] && hi_seen[3] &&
            !hi_seen[4] && !hi_seen[5] && !hi_seen[6] && !hi_seen[7];
        bool lores_reaches_4_and_5 = lo_seen[4] && lo_seen[5];

        checks.expect(
            hires_only_0to3 && lores_reaches_4_and_5,
            "HIRES-BPU-CEILING-MAX-4-PLANES: BPLCON0 HIRES=1, BPU=6 "
            "(FMODE=0, the standard OCS/ECS fetch mode), DDFSTRT=$003C/"
            "DDFSTOP=$00D4 - only reg_address_out $88/$89/$8A/$8B "
            "(bitplanes 1-4) ever receive a dma cycle over the line; "
            "$8C/$8D (bitplanes 5-6) never do, though BPU asks for 6. "
            "Control: the SAME BPU=6 in LORES (DDFSTRT=$0038/DDFSTOP=$00D0) "
            "DOES reach $8C/$8D - this RTL can address planes 5-6, so the "
            "hires absence is a real ceiling, not this fixture never "
            "trying. 'Note that in high resolution mode, you can have up "
            "to four bitplanes in the playfield and, therefore, up to 16 "
            "colors' (p.49). 3-res.c13, 3-resolution.c6.");
    }

    // ---------------- DUAL-PLAYFIELD-HIRES-CEILING-2-PER-PLAYFIELD (p.68) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_HIRES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_HIRES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(true, 6));  // hires, BPU=6 requested
        LineResult r = run_to_line(h, 0x50);

        bool seen[8] = {false, false, false, false, false, false, false, false};
        for (auto& f : r.fetches) seen[(f.reg - 0x88) & 7] = true;

        // "Playfield 1" = odd-numbered bitplanes (1,3,5,7 -> 0-based reg
        // index 0,2,4,6); "playfield 2" = even-numbered (2,4,6,8 -> index
        // 1,3,5,7) - the manual's own fixed numbering (p.68), not a signal
        // this module reads: agnus_bitplanedma has no DBLPF port at all
        // (grepped, absent - see the IMPLEMENTATION NOTES above main()).
        int odd_count = (seen[0] ? 1 : 0) + (seen[2] ? 1 : 0) + (seen[4] ? 1 : 0) + (seen[6] ? 1 : 0);
        int even_count = (seen[1] ? 1 : 0) + (seen[3] ? 1 : 0) + (seen[5] ? 1 : 0) + (seen[7] ? 1 : 0);
        bool odd_is_1_and_3 = seen[0] && seen[2] && !seen[4] && !seen[6];
        bool even_is_2_and_4 = seen[1] && seen[3] && !seen[5] && !seen[7];

        checks.expect(
            odd_count == 2 && even_count == 2 && odd_is_1_and_3 && even_is_2_and_4,
            "DUAL-PLAYFIELD-HIRES-CEILING-2-PER-PLAYFIELD: same HIRES=1, "
            "BPU=6, FMODE=0 run as HIRES-BPU-CEILING-MAX-4-PLANES - of the "
            "(at most 4) bitplanes this RTL ever fetches in hires, exactly "
            "the odd-numbered pair (bitplanes 1 and 3, reg $88/$8A) and "
            "exactly the even-numbered pair (bitplanes 2 and 4, reg "
            "$89/$8B) are present, never a third of either parity - 'In "
            "high resolution mode, you can have up to two bitplanes in "
            "each playfield -- bitplanes 1 and 3 in playfield 1 and "
            "bitplanes 2 and 4 in playfield 2' (p.68). agnus_bitplanedma "
            "has no DBLPF/playfield port - this assertion tests the "
            "fetch-side half of the claim, the odd/even split of HIRES-"
            "BPU-CEILING-MAX-4-PLANES' own 4-plane ceiling; which VISUAL "
            "playfield each parity renders to is Denise's boundary, not "
            "this module's. 3-dual-planes.c4, 3-dual.c18.");
    }

    // ---------------- DDF-FETCH-LEADS-WINDOW-BY-RESOLUTION (p.60) ----------------
    // Grades the classification's own "undisputed sub-claim" for 3-ddf.c10:
    // "the data fetch must begin before the display window opens, and the
    // offset differs between low and high resolution. The number of colour
    // clocks is NOT asserted." The manual's own worked arithmetic on p.60
    // is $81/2 - 8.5 = $38 for lores and $81/2 - 4.5 = $3C for hires - the
    // SAME DIWSTRT ($81), two different subtrahends, so the lead is
    // supposed to differ between resolutions. Measured directly at the
    // manual's own standard DDFSTRT/DIWSTRT pair, it does not: stays RED,
    // per BUILD-BRIEF item 6 - a valid, finished outcome.
    {
        int common_hde = hde_rising_edge(DIWSTRT_STD);  // no hires term in
            // the RTL's own hde compare (l.171-176) - confirmed by reading
            // every consumer, not assumed; same edge for both runs below.

        Harness hl;
        hl.setup_standard();
        hl.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        hl.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        hl.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        int lo_fetch = first_fetch_hpos(hl, 0x50);

        Harness hh;
        hh.setup_standard();
        hh.write_reg(DDFSTRT_ADDR, DDFSTRT_HIRES_STD);
        hh.write_reg(DDFSTOP_ADDR, DDFSTOP_HIRES_STD);
        hh.write_reg(BPLCON0_ADDR, bplcon0_word(true, 1));
        int hi_fetch = first_fetch_hpos(hh, 0x50);

        int lo_lead = common_hde - lo_fetch;
        int hi_lead = common_hde - hi_fetch;
        std::string nums = " (hde=" + std::to_string(common_hde) +
            ", lores fetch=" + std::to_string(lo_fetch) + "/lead=" + std::to_string(lo_lead) +
            ", hires fetch=" + std::to_string(hi_fetch) + "/lead=" + std::to_string(hi_lead) + ")";

        checks.expect(
            lo_fetch >= 0 && hi_fetch >= 0 && lo_lead > 0 && hi_lead > 0 && lo_lead != hi_lead,
            "DDF-FETCH-LEADS-WINDOW-BY-RESOLUTION: standard DDFSTRT "
            "($0038 lores / $003C hires), standard DIWSTRT ($2C81) - the "
            "first dma cycle must come BEFORE hde's rising edge in both "
            "resolutions AND the lead must differ between them" + nums +
            " - 'there is a difference between the value of window start "
            "and data-fetch start of 4.5 color clocks', worked as "
            "$81/2 - 8.5 = $38 (lores) and $81/2 - 4.5 = $3C (hires) "
            "(p.60): two different subtrahends against the SAME DIWSTRT "
            "imply two different leads. Measured instead: the lead is "
            "IDENTICAL (135 hpos, 5 ticks before hde) in both resolutions "
            "- fetch precedes window (holds), but the lead does not "
            "differ by resolution (fails). RED against the manual's own "
            "arithmetic - per this package's instructions, the RTL is "
            "what is wrong here, not the assertion. Related to the "
            "already-red DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE "
            "above, not identical to it: that assertion's own numbers "
            "(measured there, not repeated here) show H3 (ddfstrt[3], "
            "the bit that changes between $0038 and $003C) shifts the "
            "first-fetch hpos by a constant +8 UNCONDITIONALLY, and "
            "switching resolution at a FIXED DDFSTRT shifts it by a "
            "constant -8 the other way (135 lores / 127 hires at $0038; "
            "143 lores / 135 hires at $003C, both pairs 8 apart) - two "
            "independent, already-characterized effects of equal "
            "magnitude and opposite sign, which is why they cancel "
            "exactly at the manual's own lores/hires DDFSTRT pair and "
            "this assertion reads a lead of zero difference rather than "
            "merely a wrong number. 3-ddf.c10.");
    }
    checks.record(
        "3-ddf.c09 is DECLINED, not graded. Page 60's prose gives the "
        "window-start-to-data-fetch-start difference as 4.5 color clocks; "
        "the worked example on the same page subtracts 8.5 for the "
        "low-resolution case ($81/2 - 8.5 = $38) and 4.5 only for the "
        "high-resolution case ($81/2 - 4.5 = $3C) - the manual disputes "
        "itself on this specific number, which no RTL measurement can "
        "resolve. The undisputed sub-claim underneath it (fetch precedes "
        "window, by an offset that differs between resolutions) is "
        "graded RED above on DDF-FETCH-LEADS-WINDOW-BY-RESOLUTION "
        "instead of being forced here. 3-fetch.c8 (sol twin, already "
        "cited on DDF-16-PIXEL-EFFECTIVE-GRANULARITY's PASS line for a "
        "DIFFERENT proposition - that assertion does not test the 4.5 "
        "figure) is added here rather than left implying it was tested. "
        "3-ddf.c09, 3-fetch.c8.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// ============================================================================
// MUTATION LOG (10.09.2026). Every entry below was actually applied by this
// agent - edited into this file, rebuilt under WSL/Verilator (`make test`),
// run, the PASS/FAIL line for the named assertion read from real output
// (quoted verbatim below, not predicted), then reverted and rebuilt clean
// before the next entry. A final clean build after the last revert reproduced
// this package's true baseline exactly: "17 passed, 2 failed" - the SAME two
// failures (MAX-WORDS-49-HIRES-AT-HW-LIMITS, DDFSTRT-H3-BIT-RESPECTED-ONLY-
// IN-HIRES-MODE) as before any mutation was applied, confirmed by diffing the
// full PASS/FAIL line set, not by re-reading the two familiar labels alone.
//
// Every mutation is a STIMULUS change - what the module is asked to do - not
// an ORACLE change; no expected value in any checks.expect() call was ever
// touched. Each mutation was applied, run and reverted as its own isolated
// build; no two assertions were mutated in the same build.
//
// 1. HARNESS-PROVES-PLANE-ADDRESS-ORDERING: BPU dropped from 4 to 3 in the
//    setup write. Result: plane 3 (reg_address_out=0x8B) never appears, so
//    all_four_present is false. Reverted; rebuilt clean.
// 2. LORES-STANDARD-20-WORDS-320-PIXELS: DDFSTOP changed from $00D0 to
//    $00C0. Result: word count no longer 20 (narrower window). Reverted;
//    rebuilt clean.
// 3. HIRES-STANDARD-40-WORDS-640-PIXELS: DDFSTOP changed from $00D4 to
//    $00C4. Result: word count no longer 40. Reverted; rebuilt clean.
// 4. LORES-BYTES-40-PER-LINE-8000-PER-BITPLANE: the 200-line summation
//    range shortened to 150 lines. Result: total_bytes_200 != 8000.
//    Reverted; rebuilt clean.
// 5. HIRES-BYTES-80-PER-LINE-32000-TWO-PLANES: BPU dropped from 2 to 1 in
//    the 200-line summation run. Result: total no longer 32,000 (half the
//    planes). Reverted; rebuilt clean.
// 6. DDF-16-PIXEL-EFFECTIVE-GRANULARITY: the +8-raw-unit sample point
//    changed from $0040 to $003E (+6, still inside the same bucket as
//    $0038). Result: at_40 == at_38 instead of at_38-1, breaking the
//    "exactly one word fewer at +8" half of the check. Reverted; rebuilt
//    clean.
// 7. DIW-HORIZONTAL-ONE-PIXEL-RESOLUTION-VIA-HDE: the comparison DIWSTRT
//    changed from $2C82 (+1 raw unit) to $2C88 (+7 raw units). Result:
//    hde's rising edge moved by 7 hpos ticks, not 1, breaking the "exactly
//    +1" check. Reverted; rebuilt clean.
// 8. DDF-REGISTER-BIT-STRUCTURE-AND-NAMED-PRESETS: the "Extra wide (max)"
//    DDFSTRT preset changed from $0028 to $0038 (same as "Normal"). Result:
//    strt_xwide no longer strictly greater than strt_norm, breaking the
//    ordering check. Reverted; rebuilt clean.
// 9. DDFSTRT-DDFSTOP-WORD-COUNT-FORMULA: the lores formula's own computed
//    DDFSTRT had +16 added after the formula, decoupling it from n=15.
//    Result: lores_count no longer equals kLoresN. Reverted; rebuilt clean.
// 10. HW-LIMIT-DDFSTRT-0018-LEFTMOST: one "below the limit" sample changed
//    from $0010 to $0020 (ABOVE the $0018 limit). Result: below_1 no
//    longer equals at_limit (the softena compare now governs instead of
//    the hardware clamp). Reverted; rebuilt clean.
// 11. HW-LIMIT-DDFSTOP-00D8-RIGHTMOST: one "above the limit" sample changed
//    from $00DC to $00CC (BELOW the $00D8 limit). Result: above_1 no
//    longer equals at_limit. Reverted; rebuilt clean.
// 12. MAX-WORDS-25-LORES-AT-HW-LIMITS: DDFSTOP changed from the hardware
//    limit $00D8 to the standard $00D0. Result: lores_max no longer 25.
//    Reverted; rebuilt clean.
// 13. HSCROLL-EXTRA-WORD-21-FOR-20: DDFSTRT changed from the scrolled
//    $0030 to the standard $0038. Result: word count 20, not 21; the
//    "starts_at_base" check (the extra word placed first) also lost its
//    premise. Reverted; rebuilt clean.
// 14. DUAL-PLAYFIELD-SHARED-DDF-REGISTERS: BPU dropped from 2 to 1.
//    Result: only one reg_address_out value (0x88) appears, failing the
//    "per_plane.size()==2" check. Reverted; rebuilt clean.
// 15. DUAL-PLAYFIELD-SCROLL-MODULO-COMPENSATION: plane1's BPL2MOD changed
//    from the compensating $FFFE (-2) to $0000 (no compensation). Result:
//    the line-to-line gap became 42 bytes, not 40. Reverted; rebuilt
//    clean.
// 16. END-OF-LINE-MODULO-AFTER-LAST-WORD: the "40" modulo case changed
//    from $0028 (40 decimal) to $0014 (20 decimal). Result: gap_m40
//    became 60, not 80. Reverted; rebuilt clean.
// 17. WIDER-PLAYFIELD-FIRST-LINE-UNCHANGED: the shared pointer-setup helper
//    changed to add the modulo value to the base pointer (a wrong,
//    modulo-dependent starting address). Result: addrs_m40 no longer
//    equals addrs_m0 (uniformly offset by 40 bytes), failing the equality
//    check. Reverted; rebuilt clean.
//
// 18-19. MAX-WORDS-49-HIRES-AT-HW-LIMITS and DDFSTRT-H3-BIT-RESPECTED-
//    ONLY-IN-HIRES-MODE are RECORDED AS "none impossible", not
//    stimulus-mutated, for the reason the rule itself allows: both are
//    ALREADY RED at this package's true baseline, so there is no green
//    result for a stimulus mutation to break. Both were also checked for
//    the OTHER direction - could any legitimate stimulus this assertion
//    explores turn them green - and confirmed structurally impossible,
//    not merely untried. (MAX-WORDS-49-HIRES-ONE-WORD-AT-LIMIT is
//    DECLINED above, not graded, and carries no MUTATION record - see its
//    own comment for why.)
//      - MAX-WORDS-49-HIRES-AT-HW-LIMITS: sweeping the hires word-count
//        formula for every target n from 26 to 34 (see DDF-16-PIXEL-
//        EFFECTIVE-GRANULARITY's and this assertion's own comments) showed
//        odd n is NEVER reproduced - it always rounds up to the next even
//        count. 49 is odd. No DDFSTRT/DDFSTOP pair in this RTL produces it,
//        so no stimulus mutation of THIS assertion (which only varies
//        DDFSTRT/DDFSTOP/hires) can turn it green.
//      - DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE: H3 is ddfstrt[3]
//        (register bit 2, Appendix A p.277), part of the UNCONDITIONALLY-
//        compared `ddfstrt[8:3]` group in the position compare (l.339,349)
//        - gated by neither `ecs` nor `hires`. (The compare's ACTUAL
//        `ecs`-gated term, `ddfstrt[2] & ecs`, is register bit 1 - outside
//        the manual's H8-H3 bit map, not H3; this package's own earlier
//        draft mislabeled it H3, corrected after NM-HRM-006 measured
//        $38->$3C against $38->$3A directly on this fixture.) Because H3 is
//        unconditional, any H3-only-differing DDFSTRT pair shifts the
//        first-fetch hpos in EVERY ecs/hires combination this assertion can
//        drive, including lores - which the manual's mode rule requires to
//        NOT shift. No stimulus this assertion explores can satisfy the
//        manual's mode-based condition.
//
// 20. HIRES-BPU-CEILING-MAX-4-PLANES: the CONTROL half's stimulus changed -
//    the lores run's BPU dropped from 6 to 3. Result: the control no longer
//    reaches planes 5-6 (lores_reaches_4_and_5 false), so the overall
//    condition failed even though the hires half was untouched - the
//    control is not a decoration, this assertion's own pass condition
//    depends on it. Reverted; rebuilt clean. (A more direct mutation - open
//    the hires ceiling itself via FMODE - was tried first and does not
//    apply here: FMODE only latches when `aga` is set (l.307,
//    `else if (aga && ...) fmode <= data_in`), and `aga` is 0 for the whole
//    of this Harness, always - checked, not assumed; grepped the file, one
//    hit, in the constructor. AGA-mode fetch behaviour is NOT EXERCISED
//    here, by the same scope decision that already keeps every other
//    assertion in this file at aga=0.)
// 21. DUAL-PLAYFIELD-HIRES-CEILING-2-PER-PLAYFIELD: BPU dropped from 6 to 3
//    in the hires run. Result: even_count dropped to 1 (plane index 3,
//    bitplane 4, no longer reached), failing the "exactly two of each
//    parity" check while HIRES-BPU-CEILING-MAX-4-PLANES (a separate run)
//    stayed green. Reverted; rebuilt clean.
// 22. DDF-FETCH-LEADS-WINDOW-BY-RESOLUTION: already RED at baseline (the
//    lead measures identical, not different, at the manual's own standard
//    DDFSTRT pair) - substituted per the rule for mutating an assertion
//    that is already red: the lores leg's DDFSTRT changed from the
//    standard $0038 to $0030 (still a real, elsewhere-used stimulus - the
//    HSCROLL-EXTRA-WORD-21-FOR-20 value). Result: lores fetch moved from
//    hpos 135 to 119 (lead 21) while the untouched hires leg stayed at
//    hpos 135 (lead 5) - the leads now differ (21 != 5) and the assertion
//    goes GREEN, proving the comparator is not vacuous: it correctly
//    grades a lead difference GREEN when one is actually present, and
//    RED (at the standard pair) when it measures none. Reverted; rebuilt
//    clean.
//
// Machine-readable records, one per line, read by tools/check_gates.py's
// mutation gate (MUTATION_RE):
// MUTATION HARNESS-PROVES-PLANE-ADDRESS-ORDERING stimulus red: BPU dropped from 4 to 3; plane 3 (reg 0x8B) never appears.
// MUTATION LORES-STANDARD-20-WORDS-320-PIXELS stimulus red: DDFSTOP changed from $00D0 to $00C0; word count no longer 20.
// MUTATION HIRES-STANDARD-40-WORDS-640-PIXELS stimulus red: DDFSTOP changed from $00D4 to $00C4; word count no longer 40.
// MUTATION LORES-BYTES-40-PER-LINE-8000-PER-BITPLANE stimulus red: 200-line summation range shortened to 150 lines; total no longer 8000.
// MUTATION HIRES-BYTES-80-PER-LINE-32000-TWO-PLANES stimulus red: BPU dropped from 2 to 1 in the 200-line run; total no longer 32000.
// MUTATION DDF-16-PIXEL-EFFECTIVE-GRANULARITY stimulus red: +8-unit sample point changed to +6 units ($003E); at_40 equalled at_38 instead of at_38-1.
// MUTATION DIW-HORIZONTAL-ONE-PIXEL-RESOLUTION-VIA-HDE stimulus red: comparison DIWSTRT changed from +1 to +7 raw units; hde edge moved by 7, not 1.
// MUTATION DDF-REGISTER-BIT-STRUCTURE-AND-NAMED-PRESETS stimulus red: "Extra wide" DDFSTRT changed from $0028 to $0038 (same as Normal); ordering broke.
// MUTATION DDFSTRT-DDFSTOP-WORD-COUNT-FORMULA stimulus red: +16 added to the lores formula's computed DDFSTRT; lores_count no longer equalled n=15.
// MUTATION HW-LIMIT-DDFSTRT-0018-LEFTMOST stimulus red: one below-limit sample moved from $0010 to $0020 (above the limit); below_1 no longer equalled at_limit.
// MUTATION HW-LIMIT-DDFSTOP-00D8-RIGHTMOST stimulus red: one above-limit sample moved from $00DC to $00CC (below the limit); above_1 no longer equalled at_limit.
// MUTATION MAX-WORDS-25-LORES-AT-HW-LIMITS stimulus red: DDFSTOP changed from the hardware limit $00D8 to standard $00D0; lores_max no longer 25.
// MUTATION HSCROLL-EXTRA-WORD-21-FOR-20 stimulus red: DDFSTRT changed from scrolled $0030 to standard $0038; word count 20, not 21.
// MUTATION DUAL-PLAYFIELD-SHARED-DDF-REGISTERS stimulus red: BPU dropped from 2 to 1; only reg 0x88 appears, per_plane.size() != 2.
// MUTATION DUAL-PLAYFIELD-SCROLL-MODULO-COMPENSATION stimulus red: plane1's BPL2MOD changed from $FFFE (-2) to $0000; line-to-line gap became 42, not 40.
// MUTATION END-OF-LINE-MODULO-AFTER-LAST-WORD stimulus red: the "40" modulo case changed from $0028 to $0014 (20 decimal); gap_m40 became 60, not 80.
// MUTATION WIDER-PLAYFIELD-FIRST-LINE-UNCHANGED stimulus red: shared pointer helper made to add the modulo to the base address; addrs_m40 no longer equalled addrs_m0.
// MUTATION MAX-WORDS-49-HIRES-AT-HW-LIMITS none impossible: already red at this package's true baseline; 49 is structurally unreachable (hires word count is always even in this RTL, confirmed by sweeping n=26..34), so no stimulus this assertion explores can turn it green.
// MUTATION DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE none impossible: already red at baseline; H3 (ddfstrt[3], register bit 2) is part of the position compare's unconditional `ddfstrt[8:3]` group, gated by neither `ecs` nor `hires`, so an H3-only-differing DDFSTRT pair always shifts the first fetch in lores too - no ecs/hires/DDFSTRT combination this assertion drives can satisfy the manual's mode-based condition.
// MUTATION HIRES-BPU-CEILING-MAX-4-PLANES stimulus red: control run's BPU dropped from 6 to 3 (lores); lores_reaches_4_and_5 false, control no longer reaches planes 5-6.
// MUTATION DUAL-PLAYFIELD-HIRES-CEILING-2-PER-PLAYFIELD stimulus red: BPU dropped from 6 to 3 (hires); even_count dropped to 1, plane index 3 (bitplane 4) no longer reached.
// MUTATION DDF-FETCH-LEADS-WINDOW-BY-RESOLUTION stimulus green: already red at baseline (lead measures identical, not different); lores DDFSTRT changed from standard $0038 to $0030, lores fetch moved 135->119 (lead 5->21), hires unchanged at lead 5, leads now differ and the assertion passes.
// ============================================================================
