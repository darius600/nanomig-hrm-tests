// Test purpose: Bitplane MEMORY LAYOUT - the claim that a bitplane is a
//   contiguous, word-granular run of addresses; that its byte count is
//   structurally even; that two bitplanes occupy two separate contiguous
//   blocks; the specific word sequences the manual's own Figures 3-7, 3-10,
//   3-11, 3-17 and 3-18 print; the extra fetch a horizontal hires scroll
//   costs; the display window's selection of a subset of a larger memory
//   picture and its one-scan-line vertical resolution; the ECS display-window
//   range extension; the bus addresses of DIWSTRT/DIWSTOP/DIWHIGH; and the
//   hardware-limited maximum fetched word counts of Table 3-14. This is the
//   last uncovered claim group in chapter 3 for this fixture - reached no
//   package before now, for no recorded reason.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 42, 52, 53, 54, 55, 57, 59, 61, 72, 75, 76, 80, 81, 277, 290, 293.
// Non-goals: Table 3-7/3-8's own byte totals and the two-bitplane/wide-
//   playfield worked totals (below, DECLINED - resolution/geometry
//   arithmetic, not a fetch this module performs); the code-example comment
//   line restating the same 8,000-byte figure (DECLINED, excluded by the
//   extraction rules already); the blanking-clipped 368-pixel/23-word
//   DISPLAYABLE width of p.80 (DECLINED - a denise-side display-clip
//   concept, and a bare pixel/word division, not a DMA fetch count); the
//   NTSC/PAL vertical-scroll step-size family of p.81 (DECLINED - needs the
//   line-count/ntsc-pal boundary, not this one); the ECS-extension
//   provenance sentence of p.59 (DECLINED - a documentation-history remark
//   naming no number of its own; the substantive ECS gate is asserted
//   separately, see G12). Register MAP claims for registers other than
//   DIWSTRT/DIWSTOP/DIWHIGH, and the horizontal fetch WINDOW (DDFSTRT/
//   DDFSTOP word counts), and the pointer/modulo ARITHMETIC in general, are
//   this fixture's other two packages' boundaries (ch3-register-map-agnus,
//   ch3-ddf-fetch-window, ch3-bitplane-pointers-and-modulo) - not repeated
//   here except where THIS package's own claim ids happen to land on the
//   same ground from a different printed citation.
//
// SIBLING OVERLAP CHECK (done before writing a single assertion): grepped
//   both hrm/ch3-ddf-fetch-window/test_agnus_bitplanedma.cpp and
//   hrm/ch3-bitplane-pointers-and-modulo/test_agnus_bitplanedma.cpp for
//   every one of this package's 64 claim ids, by exact id string, not by
//   topic or impression. Zero matches in either file. None of these 64 is
//   already asserted by a sibling package on this fixture.
//
// BOUNDARY (from the two sibling packages' own already-built Harness code,
//   which enumerates every port they drive - not yet from opening the RTL
//   file myself): dut ports clk, clk7_en, reset, harddis, aga, ecs, a1k,
//   sof, dmaena, vpos, hpos, reg_address_in, data_in, address_out,
//   reg_address_out, dma, hde. Register bus addresses already used by both
//   siblings: DIWSTRT $08E, DIWSTOP $090, DDFSTRT $092, DDFSTOP $094,
//   BPLxPT base $0E0 (plane N: PTH=base+4N, PTL=base+4N+2), BPL1MOD $108,
//   BPL2MOD $10A, BPLCON0 $100, FMODE $1FC. Neither sibling's port list
//   includes an ntsc/pal selector on this module - a fact that matters
//   directly to G-declined-8 below, and is CONFIRMED, not merely inferred
//   from this absence, only after the RTL is opened myself (see the
//   RTL-OPENED marker further down).
//
// CLAIMS (verbatim quote + restatement from
// C:\Users\x\AppData\Local\Temp\claude\d--code\674b5d99-c2c3-47f0-95e4-93eefcde2be3\scratchpad\residue2\ch3-bitplane-memory-layout\claims.jsonl,
// all 64 records). Grouped by the proposition each supports, not by page
// order; ids restating the same proposition are grouped and cited together
// on whichever assertion(s) test that proposition. Each group states this
// package's testability verdict and why, BEFORE the RTL was opened.
//
// -- G1: a bitplane is an end-to-end sequence of words at consecutive
//    memory locations (p.52) - ASSERT --
//   3-mem.c02 (opus, p52): "sequence of words at consecutive memory
//     locations. When operating under" -> "A bitplane is an end-to-end
//     sequence of words at consecutive memory locations."
//   3-memory.c1 (sol, p52): "A bitplane consists of an end-to-end sequence
//     of words at consecutive memory locations." -> same idea, second
//     reading.
//   Witness: successive DMA fetches of one bitplane advance the fetched
//   address by a constant, non-zero step with no gaps and no repeats, over
//   a run of many fetches - the concrete shape "contiguous sequence" takes
//   at a DMA-address port.
//
// -- G2: bitplane data is fetched two bytes (one word) at a time (p.61) -
//    ASSERT --
//   3-modulo.c4 (sol, p61): "These pointers are dynamic; once the data
//     fetch begins, the pointers are continuously incremented to point to
//     the next word to be fetched (data is fetched two bytes at a time)."
//     -> "Bitplane display DMA fetches two bytes at a time."
//   3-ptr.c10 (opus, p61): "to the next word to be fetched (data is fetched
//     two bytes at a time). When" -> same sentence, second reading.
//   Witness: the address step between one fetch and the next is exactly 2,
//   never 1 and never any other value - the fetch GRANULARITY, a distinct
//   proposition from G1's gaplessness and G3's parity, even though all
//   three read the same underlying step.
//
// -- G3: the number of bytes allocated for a bitplane must be even (p.52,
//    76) - ASSERT --
//   3-mem.c05 (opus, p52): "Keep in mind that the number of bytes you
//     allocate for a bitplane must be" -> "The number of bytes allocated
//     for a bitplane must be even."
//   3-memory.c9 (sol, p52): "Keep in mind that the number of bytes you
//     allocate for a bitplane must be even." -> same, full sentence.
//   3-big.c14 (opus, p76): "The number of bytes must be even.  Thus, if the
//     wide playfield described" -> same rule restated in the wide-playfield
//     section.
//   Witness: this is a property of the ALLOCATION, not of any one fetch;
//   what the fetch port can show is that its own granularity structurally
//   forecloses an odd total - every fetch step is exactly 2 (G2), so the
//   byte count after any whole number of fetches is even BY CONSTRUCTION.
//   Tested as: no observed address delta is ever odd, across many fetches
//   and several configurations.
//
// -- G4: each bitplane's memory must be contiguous; two bitplanes need two
//    SEPARATE contiguous blocks (p.53) - ASSERT (contiguity/separateness
//    only - the "8,000-byte" size figure is G-declined-3 below; do not
//    bundle a size fact with a shape fact) --
//   3-mem.c11 (opus, p53): "continuous, so you need to have two 8,000-byte
//     blocks of available memory." -> "Two bitplanes therefore need two
//     separate contiguous 8,000-byte blocks."
//   3-memory.c17 (sol, p53): "The memory for each bitplane must be
//     continuous, so you need to have two 8,000-byte blocks of available
//     memory." -> "Each bitplane must occupy a contiguous memory block."
//   Witness: two bitplanes driven from two independent BPLxPT pointers each
//   produce their own gapless run (G1, per-plane) that does not overlap the
//   other plane's run - "separate" as non-overlap, "contiguous" as gapless,
//   both directly observable; the specific 8,000-byte SIZE of each block is
//   not part of what is asserted here.
//
// -- G5: Figure 3-7's first line / Figure 3-10 - the labelled word sequence
//    START, START+2, ... START+38 (p.53-54, p.61) - ASSERT --
//   3-mem.f01 (opus, p54, figure): "Figure 3-7 (printed p.54): the first
//     row of cells is labelled 'Mem. location N' at the left and 'Mem.
//     location N+38' at the right, joined by an arrow - one 40-byte display
//     line runs from N to N+38 inclusive (20 words)."
//   3-modulo.c8 (sol, p61, figure): "[FIGURE 3-10] -> The labeled first-line
//     locations show successive fetched words at START, START+2, START+4,
//     through START+38."
//   Both figures illustrate the identical arithmetic fact (a standard
//   40-byte/20-word display line's own first-line address progression);
//   grouped because they are the same proposition read from two printed
//   pages, not because their numbers happen to coincide.
//
// -- G6: Figure 3-11 - the second line's word sequence START+40, START+42,
//    ... START+78 (p.61) - ASSERT --
//   3-modulo.c13 (sol, p61, figure): "[FIGURE 3-11] -> The labeled
//     second-line locations show successive fetched words at START+40,
//     START+42, START+44, through START+78."
//   Witness: over a two-line run with the modulo that makes lines
//   contiguous (0), line 2 begins exactly where line 1's own 20-word,
//   40-byte advance would put it (G1 extended across a line boundary) -
//   the genuine "next line follows the first with no gap" case, distinct
//   from G8's wide-playfield case below even though the printed numbers
//   happen to match.
//
// -- G7: Figure 3-7's last row - an 8,000-byte bitplane organized as 200
//    lines, the 200th ending at N+7960..N+7998 (p.53-54) - ASSERT (the
//    SHAPE - 200 lines, last-line addresses - not the 8,000-byte SIZE,
//    which is G-declined-1's arithmetic) --
//   3-memory.c18 (sol, p53): "Figure 3-7 shows an 8,000-byte memory area
//     organized as 200 lines of 40 bytes each, providing 1 bit for each
//     pixel position in the display plane." -> "An 8,000-byte normal NTSC
//     low-resolution bitplane is organized as 200 lines."
//   3-mem.f03 (opus, p54, figure): "Figure 3-7 (printed p.54): the last row
//     is labelled 'Mem. location N+7960' ... 'Mem. location N+7998' - the
//     200th line of an 8,000-byte bitplane ends at N+7998."
//   Witness: drive a real 200-line, zero-modulo, standard-lores run and
//   read the 200th line's own fetched addresses; the expected N+7960 and
//   N+7998 offsets follow from the manual's OWN stated 40-bytes/line x 200
//   lines (200-1)*40=7960, +38 for the last word of a 20-word line - not
//   from any RTL-observed number.
//
// -- G8: Figures 3-17/3-18 - a wide-playfield RIGHT HALF, two lines,
//    START+40..START+78 then START+120..START+158 (p.75) - ASSERT --
//   3-large.c14 (sol, p75, figure): "[FIGURE 3-17] -> The right-half line-1
//     labels show fetched words from START+40 through START+78."
//   3-large.c17 (sol, p75, figure): "[FIGURE 3-18] -> The right-half
//     line-2 labels show fetched words from START+120 through START+158."
//   Companion figures of ONE scenario (consecutive lines of the same
//   worked example), grouped for that reason. Distinct from G6: G6 is a
//   normal-width display's own line 2; G8 is a WIDE underlying RAM row of
//   which only the right 40-byte half is displayed, requiring a nonzero
//   modulo to skip the undisplayed left half of the next row - the two
//   propositions coincide numerically (both read START+40..+78 for their
//   own "line 1") only because the manual chose matching example numbers,
//   not because they are the same claim.
//
// -- G9: in high resolution mode, scrolling costs two words more than the
//    playfield width, not one (p.72) - ASSERT --
//   3-dual.c35 (opus, p72): "   playfield you are trying to scroll (two
//     words more in high resolution" -> "In high resolution mode two words
//     more than the playfield width must be fetched."
//   Witness: the hires analogue of the lores "+1 word" scroll compensation
//   the ch3-ddf-fetch-window sibling already measured under its own claim
//   ids (HSCROLL-EXTRA-WORD-21-FOR-20) - a scroll-widened hires DDFSTRT/
//   DDFSTOP window fetches exactly 2 more words than the unscrolled hires
//   baseline, not 1.
//
// -- G10: display-window sizing selects the visible portion of a larger
//    memory picture (p.42) - ASSERT --
//   3-video.c29 (sol, p42): "You choose which part of this larger memory
//     picture to display by specifying a different size for the display
//     window." -> "Display-window sizing selects the visible portion of a
//     larger memory picture."
//   Witness: a narrowed display window (DIWSTRT/DIWSTOP) yields fewer
//   active/fetched lines than a wider one over the identical driven vpos
//   range - the window selects a SUBSET, not the whole; a qualitative
//   proposition, distinct from G11's specific step size.
//
// -- G11: the resolution of vertical display-window start/stop is one scan
//    line (p.57) - ASSERT --
//   3-window.c8 (sol, p57): "The resolution of vertical start and stop is
//     one scan line." -> "Display-window vertical start and stop have
//     one-scan-line resolution."
//   Witness: a relation, not a magic constant - changing DIWSTRT or
//   DIWSTOP by exactly one raw unit changes the measured active-line
//   boundary by exactly one line, never zero and never more than one.
//
// -- G12: ECS extends the display-window minimum/maximum values beyond the
//    Table 3-9 (OCS) range (p.59) - ASSERT --
//   3-window.c59 (sol, p59): "The minimum and maximum values for display
//     windows have been extended in the enhanced version of the Amiga's
//     custom chip set (ECS)." -> "ECS extends the display-window minimum
//     and maximum values beyond the OCS ranges in Table 3-9."
//   Witness: an ecs=0 vs ecs=1 sweep of a window value outside the OCS
//   Table 3-9 range - the module either honours the extended value only
//   under ecs=1, or it does not; both are a valid, informative result.
//
// -- G13/G14/G15: DIWSTRT/DIWSTOP/DIWHIGH bus addresses (p.277, 290, 293) -
//    ASSERT, address-decode SELECTIVITY only (does writing the documented
//    address change this module's window behaviour, and does writing an
//    adjacent WRONG address not) --
//   G13, DIWSTRT = $08E: A-diw.c5 (sol, p277): "DIWSTRT    08E      W
//     A      Display window start (upper left" -> "DIWSTRT is at
//     custom-register offset $08E." A-diwstrt.addr (opus, p277): same row,
//     second reading -> "Appendix A: DIWSTRT is at address $08E."
//     B-diwstrt.addr (opus, p290): "    DIWSTRT        08E    W   A
//     Display window start (upper left" -> "Appendix B: DIWSTRT is at
//     address $08E." B-subject.c9 (sol, p290): same row, second reading ->
//     "DIWSTRT is at custom-register offset $08E."
//   G14, DIWSTOP = $090: A-diw.c3 (sol, p277): "DIWSTOP    090      W
//     A      Display window stop (lower right" -> "DIWSTOP is at
//     custom-register offset $090." A-diwstop.addr (opus, p277): same row
//     -> "Appendix A: DIWSTOP is at address $090." B-diwstop.addr (opus,
//     p290): "    DIWSTOP        090    W               Display window
//     stop (lower right" -> "Appendix B: DIWSTOP is at address $090."
//     B-subject.c13 (sol, p290): same row -> "DIWSTOP is at custom-register
//     offset $090."
//   G15, DIWHIGH = $1E4: A-diw.c1 (sol, p277): "DIWHIGH    1E4      W
//     A,D( E ) Display window -  upper bits for" -> "ECS DIWHIGH is at
//     custom-register offset $1E4." A-diwhigh.addr (opus, p277): same row
//     -> "Appendix A: DIWHIGH is at address $1E4." B-diwhigh.addr (opus,
//     p293): "DIWHIGH       1E4   w   A, D(E)    Display window - upper
//     bits for start, stop" -> "Appendix B: DIWHIGH is at address $1E4."
//     B-subject.c292 (sol, p293): same row -> "DIWHIGH is at custom-register
//     offset $1E4."
//
// -- G16/G17: Table 3-14's own maximum FETCHED word counts - 25 lores, 49
//    hires, at the DDFSTRT/DDFSTOP hardware limits (p.80) - ASSERT (this is
//    the DMA FETCH count Table 3-14 itself names, squarely this module's
//    business - distinct from G-declined-7's DISPLAYABLE-width sentence on
//    the same page) --
//   G16: 3-max-window.c27 (sol, p80): "max words fetched         25
//     49" -> "Table 3-14 states a low-resolution maximum of 25 fetched
//     words."
//   G17: 3-max-window.c28 (sol, p80): same table row -> "Table 3-14 states
//     a high-resolution maximum of 49 fetched words."
//   Witness: drive DDFSTRT/DDFSTOP to the documented $18/$D8 hardware
//   limits in each resolution and count real fetched words - a
//   RESOLUTION-INDEPENDENT per-line word count this module produces on
//   demand, unlike the resolution+line-count+interlace TOTALS declined
//   below.
//
// ============================================================================
// DECLINED (32 of the 64... no: 30 of the 64 - see the exact count in the
// final report). Every id below is cited in a checks.note() call at the end
// of main() as well as here, so it is not "covered only in a comment".
//
// -- D1: Table 3-7 (NTSC) byte-per-resolution totals (p.52) - DECLINE --
//   3-memory.c5 (sol, p52): "320 X 200      Low resolution,       8,000
//     non-interlaced" -> "An NTSC 320x200 low-resolution non-interlaced
//     bitplane requires 8,000 bytes."
//   3-memory.c6 (sol, p52): "320 X 400 ... 16,000 ... interlaced" -> same,
//     320x400 interlaced.
//   3-memory.c7 (sol, p52): "640 X 200 ... 16,000 ... High resolution,
//     non-interlaced" -> same, 640x200 hires.
//   3-memory.c8 (sol, p52): "640 X 400 ... 32,000 ... interlaced" -> same,
//     640x400 hires interlaced.
//   3-t7.c01/c02/c03/c04 (opus, p52): the same four table rows, second
//     reading.
//   Why declined: "320x200 needs 8,000 bytes" packages together THREE
//   independent facts this module never combines into one number of its
//   own: words fetched per line (a DDFSTRT/DDFSTOP fact), how many lines
//   are driven (an external vpos/frame-timing fact this module does not
//   originate), and whether the field is interlaced (a frame-cadence fact,
//   also not this module's). Naming a total under a RESOLUTION LABEL
//   requires the test to already presuppose the very line-count and
//   interlace-factor the label stands for; the module itself has no
//   register or port that says "this is a 320x200 display". This is
//   exactly the prototype case the build brief itself names: a DMA engine
//   fetches what it is pointed at, and never performs this multiplication.
//
// -- D2: Table 3-8 (PAL) byte-per-resolution totals (p.53) - DECLINE, same
//    reasoning as D1 --
//   3-memory.c10 (sol, p53): "320 X 256 ... 8,192 ... non-interlaced" ->
//     PAL 320x256 lores.
//   3-memory.c11 (sol, p53): "320 X 512 ... 16,384 ... interlaced" -> PAL
//     320x512 lores interlaced.
//   3-memory.c12 (sol, p53): "640 X 256 ... 16,384 ... High resolution,
//     non-interlaced" -> PAL 640x256 hires.
//   3-memory.c13 (sol, p53): "640 X 512 ... 32,768 ... interlaced" -> PAL
//     640x512 hires interlaced.
//   3-t8.c01/c02/c03/c04 (opus, p53): the same four PAL table rows, second
//     reading.
//
// -- D3: two-bitplane playfield totals (p.53) - DECLINE, same reasoning as
//    D1 (adds a plane-COUNT multiplication on top) --
//   3-mem.c09 (opus, p53): "A low resolution, non-interlaced playfield made
//     up of two bitplanes" -> "requires 16,000 bytes" (the SIZE half; the
//     shape/separateness half of this same sentence family is G4, asserted
//     above under different ids).
//   3-memory.c16 (sol, p53): "A low resolution, non-interlaced playfield
//     made up of two bitplanes requires 16,000 bytes of memory area." ->
//     same total, second reading.
//
// -- D4: wide-playfield worked arithmetic, 80*200*2=32,000 (p.76) -
//    DECLINE, same reasoning as D1/D3 --
//   3-big.c15 (opus, p76): "            80 * 200 * 2 = 32,000 bytes of
//     memory" -> "An 80-byte-wide, 200-line, two-bitplane playfield
//     requires 32,000 bytes."
//
// -- D5: code-example comment line (p.55) - DECLINE, already flagged in
//    the inventory itself --
//   3-code.c08 (opus, p55): "        MOVE.W  #2000,d0        ; Write 2000
//     longwords = 8000 bytes" -> already marked in claims.jsonl,
//     source_defines: "no: REJECTED by sol's cross-read: This is a
//     code-example line, explicitly excluded by the extraction rules." This
//     package agrees and does not reopen it; it restates D1's own
//     8,000-byte figure besides.
//
// -- D6: ECS-extension provenance sentence (p.59) - DECLINE (names no
//    number of its own; the substantive gate is G12 above, different id) --
//   3-diw.c40 (opus, p59): "The minimum and maximum values for display
//     windows have been extended" -> "The minimum and maximum display-
//     window values were extended in the Enhanced Chip Set; the Table 3-9
//     limits are therefore the original chip set's." This is a remark about
//     which printed TABLE belongs to which chipset generation - a
//     documentation-provenance fact, not itself a numeric behaviour to
//     drive and observe. G12 (id 3-window.c59, a different id, same page)
//     already asserts the substantive ECS/OCS behavioural gate this
//     sentence's neighbour states.
//
// -- D7: blanking-limited DISPLAYABLE width, 368 pixels = 23 words (p.80) -
//    DECLINE --
//   3-max-window.c16 (sol, p80): "However, horizontal blanking actually
//     limits the displayable video to 368 low resolution pixels (23
//     words)." -> "The 368-pixel displayable width corresponds to 23
//     low-resolution words."
//   3-max.c10 (opus, p80): "video to 368 low resolution pixels (23 words).
//     These numbers are the same" -> same sentence, second reading.
//   Why declined: this is what can be DISPLAYED after horizontal blanking
//   clips it - a Denise-side display concept - not what this module
//   FETCHES (that is Table 3-14's own "max words fetched" row, G16/G17
//   above, a different and larger number: 25/49, because extra words are
//   fetched and then blanked, never shown). agnus_bitplanedma has no
//   blanking-vs-fetched distinction on its own ports (confirmed pending
//   RTL-open, see below). "23 words" is also a bare division (368/16
//   pixels-per-lores-word), not a behaviour.
//
// -- D8: NTSC/PAL vertical-scroll step-size family (p.81) - DECLINE --
//   3-vscr.c04 (opus, p81): "NTSC system, with a display that has 200
//     vertical lines, each step can be\nas little as 1/200th of the
//     screen. In interlaced mode each step could be" -> "NTSC, 200-line
//     display: the smallest vertical scroll step is 1/200th of the
//     screen."
//   3-vscr.c07 (opus, p81): "the odd/even field relationship. Using a PAL
//     system with 256 lines on the\ndisplay, the step can be 1/256th of a
//     screen, or 1/512th of a screen in\ninterlace." -> "PAL, 256-line
//     display: the vertical scroll step can be 1/256th of a screen."
//   3-vscr.c08 (opus, p81): "display, the step can be 1/256th of a
//     screen, or 1/512th of a screen in\ninterlace." -> "PAL interlaced:
//     the vertical scroll step can be 1/512th of a screen."
//   3-vscroll.c3 (sol, p81): "On an NTSC system, with a display that has
//     200 vertical lines, each step can be as little as 1/200th of the
//     screen." -> same as vscr.c04, second reading.
//   3-vscroll.c4 (sol, p81): "In interlaced mode each step could be
//     1/400th of the screen if clever manipulation of the pointers is
//     used..." -> "With pointer manipulation, a 400-line interlaced NTSC
//     playfield can scroll in 1/400-screen steps."
//   3-vscroll.c5 (sol, p81): "Using a PAL system with 256 lines on the
//     display, the step can be 1/256th of a screen..." -> same as
//     vscr.c07, second reading.
//   3-vscroll.c6 (sol, p81): "...or 1/512th of a screen in interlace." ->
//     same as vscr.c08, second reading.
//   Why declined: "1/200th" versus "1/256th" is only meaningful relative to
//   a TOTAL line count that is itself an NTSC-vs-PAL frame-timing fact,
//   which lives on agnus_beamcounter (the module with the ntsc/pal
//   selector), not on this fetch-only module - confirmed pending RTL-open,
//   see below, and already strongly indicated by neither sibling Harness
//   driving any ntsc/pal port on this DUT. Separately, "the smallest step
//   is one line" is true of ANY pointer-addressed bitplane at ANY line
//   count purely because a line's worth of bytes is the smallest
//   meaningful vertical increment - it is not a restriction this module
//   enforces, so there is no stimulus that could make it read otherwise
//   even in principle. Needs a wider boundary than this package's assigned
//   fixture provides.
// ============================================================================

// ===================== RTL-OPENED FROM HERE DOWN =====================
