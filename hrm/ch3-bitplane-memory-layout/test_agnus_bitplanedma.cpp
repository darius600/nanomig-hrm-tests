// Test purpose: Bitplane MEMORY LAYOUT - the claim that a bitplane is a
//   contiguous, word-granular run of addresses; that its byte count is
//   structurally even; that two bitplanes occupy two separate contiguous
//   blocks; the specific word sequences the manual's own Figures 3-7, 3-10,
//   3-11, 3-17 and 3-18 print; the extra fetch a horizontal hires scroll
//   costs; the display window's selection of a subset of a larger memory
//   picture and its one-scan-line vertical resolution; the ECS
//   display-window range extension; the bus addresses of
//   DIWSTRT/DIWSTOP/DIWHIGH; and the hardware-limited maximum fetched word
//   counts of Table 3-14. This is the last uncovered claim group in chapter
//   3 for this fixture - reached no package before now, for no recorded
//   reason.
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
//
// Port list and register decode confirmed by opening
// nanomig/src/minimig-aga/agnus_bitplanedma.v directly (not merely
// inferred from the siblings): DIWSTRT_REG=9'h08E, DIWSTOP_REG=9'h090,
// DIWHIGH_REG=9'h1E4, BPLPTBASE_REG=9'h0E0, DDFSTRT_REG=9'h092,
// DDFSTOP_REG=9'h094, BPL1MOD_REG=9'h108, BPL2MOD_REG=9'h10a,
// BPLCON0_REG=9'h100, FMODE_REG=9'h1fc (l.51-60). CONFIRMED: no ntsc/pal
// port anywhere in the module header (l.29-47) - D8's decline holds.
// vdiwena (l.207-215) is a single-line `==` compare against an 11-bit
// vdiwstrt/vdiwstop register with no quantization - G11's one-line
// resolution is structurally exact, not approximate. bplptl/bplpth/bpl1mod/
// bpl2mod are all declared starting at bit 1 (`[15:1]`, `[20:16]` paired
// with the low half) - data_in bit 0 is never wired to any of them
// (l.63-70, 81-83) - G3's "LSB silently dropped" mechanism is exactly
// this. address_out itself is declared `[21-1:1]` (l.46) - no bit 0 on
// the port at all. newpt's per-fetch step depends on fmode (l.484-493):
// step is 1 (word) only when fmode[1:0]==2'b00; FMODE can only be
// written when aga=1 (l.307) - so under aga=0 (this package's setup,
// matching both siblings), fmode is permanently locked at its reset
// value 0 and the step is unconditionally 1 word = 2 bytes. This gives
// G2 a genuine STIMULUS mutation: set aga=1 and write FMODE nonzero.
//
// ============================================================================

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include <verilated.h>
#include "Vagnus_bitplanedma.h"

namespace {

constexpr uint16_t DIWSTRT_ADDR = 0x08E;
constexpr uint16_t DIWSTOP_ADDR = 0x090;
constexpr uint16_t DIWHIGH_ADDR = 0x1E4;
constexpr uint16_t DDFSTRT_ADDR = 0x092;
constexpr uint16_t DDFSTOP_ADDR = 0x094;
constexpr uint16_t BPLPT_BASE   = 0x0E0;  // plane N: PTH=BASE+4N, PTL=BASE+4N+2
constexpr uint16_t BPL1MOD_ADDR = 0x108;
constexpr uint16_t BPLCON0_ADDR = 0x100;
constexpr uint16_t FMODE_ADDR   = 0x1FC;

// Standard values, p.60/p.80 (Table 3-14) - printed presets, not RTL-derived.
constexpr uint16_t DDFSTRT_LORES_STD = 0x0038;
constexpr uint16_t DDFSTOP_LORES_STD = 0x00D0;
constexpr uint16_t DDFSTRT_HIRES_STD = 0x003C;
constexpr uint16_t DDFSTOP_HIRES_STD = 0x00D4;
// Hardware limits $18/$D8 (p.80) - driven from beyond both ends ($00/$FE)
// so the HARDWARE clamp, not the software window, is what is measured.
constexpr uint16_t DDFSTRT_HW_MIN = 0x0000;
constexpr uint16_t DDFSTOP_HW_MAX = 0x00FE;
// DIWSTRT/DIWSTOP centering a normal display, p.57's own worked example
// ("$81,$2C"), also the sibling packages' own basis: vdiwstrt=44,
// vdiwstop=300 (verified by hand against the write-decode read above).
constexpr uint16_t DIWSTRT_STD = 0x2C81;
constexpr uint16_t DIWSTOP_STD = 0x2CC1;

uint16_t bplcon0_word(bool hires, int bpu /*0-7*/) {
    // bplcon0 <= {data_in[6], data_in[15], aga&data_in[4], data_in[14:12]}
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
        // vdiwstrt/vdiwstop carry no reset in the RTL (l.99-100) - Verilator
        // zero-initializes them. vdiwena is re-evaluated EVERY clk7_en tick
        // (l.208-215): "vpos==vdiwstrt" opens it, and this is checked
        // continuously, not once. If vpos were left at its own power-up
        // value of 0 while an assertion writes its window registers one at
        // a time, any register not yet written (still reading its 0
        // default) would coincidentally MATCH vpos==0 on every intervening
        // tick, latching vdiwena on before the real value is even written -
        // a harness artifact of write ORDER, not RTL behaviour. Parking
        // vpos at 2000 (11 bits wide, max 2047; no test in this package
        // ever uses a vdiwstrt/vdiwstop anywhere near that) for the whole
        // setup phase removes the coincidence entirely, regardless of which
        // register happens to be written first; run_frame's own loop sets
        // vpos back to 0 as its very first act, once every register holds
        // its real, final, intended value.
        dut->vpos = 2000;
        tick();
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

    // DIW centered per p.57's own example; dmaena on; FMODE written (a
    // no-op under aga=0, kept for parity with both siblings' own setup).
    void setup_standard() {
        write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
        write_reg(DIWSTOP_ADDR, DIWSTOP_STD);
        write_reg(FMODE_ADDR, 0x0000);
        dut->dmaena = 1;
    }

    Vagnus_bitplanedma* dut;
};

struct Fetch {
    uint32_t address;  // address_out*2, a byte address
    uint16_t reg;       // reg_address_out (0x88=plane0, 0x89=plane1, ...)
};

struct LineResult {
    std::vector<Fetch> fetches;
    int count() const { return static_cast<int>(fetches.size()); }
};

// Runs hpos 0..hpos_max-1 at the CURRENT vpos (caller sets h.dut->vpos
// first), recording every dma pulse in time order.
LineResult run_line(Harness& h, int hpos_max = 456) {
    LineResult r;
    for (int hp = 0; hp < hpos_max; ++hp) {
        h.dut->hpos = hp;
        h.tick();
        if (h.dut->dma) {
            r.fetches.push_back({static_cast<uint32_t>(h.dut->address_out) * 2,
                                  static_cast<uint16_t>(h.dut->reg_address_out)});
        }
    }
    return r;
}

// Runs vpos 0..to_vpos inclusive (one hpos sweep per line) and returns
// EVERY line's own fetches, indexed by vpos - lines[v] is line v's result.
// A single continuous sweep from vpos 0, never restarted mid-run, so a
// pointer written once before the sweep begins auto-increments correctly
// across every line and every line boundary in order.
std::vector<LineResult> run_frame(Harness& h, int to_vpos, int hpos_max = 456) {
    std::vector<LineResult> lines;
    for (int v = 0; v <= to_vpos; ++v) {
        h.dut->vpos = v;
        lines.push_back(run_line(h, hpos_max));
    }
    return lines;
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- FIXTURE-DELIVERS-KNOWN-WORD [self-proof, no page] ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));  // lores, BPU=1
        constexpr uint32_t kBase = 0x00004000;
#ifndef MUT_FIXTURE_DELIVERS_KNOWN_WORD
        h.write_ptr(0, kBase);
#else
        h.write_ptr(0, kBase + 2);  // wrong pointer: the check below still compares against kBase
#endif
        std::vector<LineResult> frame = run_frame(h, 44);  // vdiwena activates at vpos==44
        LineResult& line = frame[44];
        bool starts_right = line.count() == 20 && !line.fetches.empty() &&
            line.fetches.front().address == kBase && line.fetches.front().reg == 0x88;
        bool steps_right = true;
        for (size_t i = 1; i < line.fetches.size(); ++i)
            if (line.fetches[i].address != line.fetches[i - 1].address + 2) steps_right = false;
        checks.expect(
            starts_right && steps_right,
            "FIXTURE-DELIVERS-KNOWN-WORD [self-proof, no page]: a single "
            "programmed pointer (0x00004000) arrives at reg_address_out "
            "0x88 as the FIRST fetched word of a standard 20-word lores "
            "line, and every subsequent fetch in that line steps by "
            "exactly 2 bytes - this fixture's write-then-fetch path is "
            "alive before any claim below is trusted to mean anything.");
    }
    // ---------------- BITPLANE-CONTIGUOUS-WORD-SEQUENCE (p.52) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
#ifndef MUT_BITPLANE_CONTIGUOUS_WORD_SEQUENCE
        h.write_reg(BPL1MOD_ADDR, 0x0000);
#else
        h.write_reg(BPL1MOD_ADDR, 0x0028);  // 40-byte gap inserted at the line boundary
#endif
        h.write_ptr(0, 0x00010000);
        std::vector<LineResult> frame = run_frame(h, 45);
        std::vector<uint32_t> run;
        for (auto& f : frame[44].fetches) run.push_back(f.address);
        for (auto& f : frame[45].fetches) run.push_back(f.address);
        bool contiguous = run.size() == 40;
        for (size_t i = 1; i < run.size() && contiguous; ++i)
            if (run[i] != run[i - 1] + 2) contiguous = false;
        checks.expect(
            contiguous,
            "BITPLANE-CONTIGUOUS-WORD-SEQUENCE: across two consecutive "
            "lines (40 fetches total), every fetched address is exactly 2 "
            "bytes more than the one before it - no gap, no repeat, over "
            "the whole run - 'A bitplane consists of an end-to-end "
            "sequence of words at consecutive memory locations' (p.52). "
            "3-mem.c02, 3-memory.c1.");
    }

    // ---------------- TWO-BYTES-PER-FETCH (p.61) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
#ifndef MUT_TWO_BYTES_PER_FETCH
        // aga stays 0 (Harness default): FMODE is permanently locked at
        // its reset value (fmode[1:0]==0), so the per-fetch step is
        // unconditionally 1 word = 2 bytes (l.307, l.484-493).
#else
        h.dut->aga = 1;
        h.write_reg(FMODE_ADDR, 0x0001);  // fmode[1:0]==01: step becomes 2 words = 4 bytes
#endif
        h.write_ptr(0, 0x00011000);
        std::vector<LineResult> frame = run_frame(h, 44);
        bool step_is_2 = frame[44].count() >= 2;
        for (size_t i = 1; i < frame[44].fetches.size() && step_is_2; ++i)
            if (frame[44].fetches[i].address != frame[44].fetches[i - 1].address + 2)
                step_is_2 = false;
        checks.expect(
            step_is_2,
            "TWO-BYTES-PER-FETCH: the address step between one dma pulse "
            "and the next is exactly 2 bytes, on every one of " +
                std::to_string(frame[44].count()) +
                " fetches in the line - 'the pointers are continuously "
                "incremented to point to the next word to be fetched (data "
                "is fetched two bytes at a time)' (p.61). 3-modulo.c4, "
                "3-ptr.c10.");
    }

    // ---------------- BITPLANE-BYTES-ALWAYS-EVEN (p.52, 76) ----------------
    {
        auto first_addr_for_ptl = [&](uint16_t ptl_raw) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(static_cast<uint16_t>(BPLPT_BASE), 0x0001);  // plane0 PTH
            h.write_reg(static_cast<uint16_t>(BPLPT_BASE + 2), ptl_raw);  // plane0 PTL
            std::vector<LineResult> frame = run_frame(h, 44);
            return frame[44].fetches.empty() ? 0xFFFFFFFFu : frame[44].fetches.front().address;
        };
        uint32_t addr_even = first_addr_for_ptl(0x2000);
#ifndef MUT_BITPLANE_BYTES_ALWAYS_EVEN
        uint32_t addr_odd = first_addr_for_ptl(0x2001);  // same word, LSB dropped: must match addr_even
#else
        uint32_t addr_odd = first_addr_for_ptl(0x2002);  // the NEXT word: legitimately different, breaks the check
#endif

        auto line_step_for_mod = [&](uint16_t mod_raw) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(BPL1MOD_ADDR, mod_raw);
            h.write_ptr(0, 0x00030000);
            std::vector<LineResult> frame = run_frame(h, 45);
            return frame[45].fetches.front().address - frame[44].fetches.front().address;
        };
        uint32_t step_even_mod = line_step_for_mod(40);  // 0x0028
        uint32_t step_odd_mod = line_step_for_mod(41);   // 0x0029, LSB dropped: must match

        checks.expect(
            addr_even == addr_odd && step_even_mod == step_odd_mod,
            "BITPLANE-BYTES-ALWAYS-EVEN: an ostensibly-odd pointer low "
            "word (raw 0x2001) fetches from the SAME address (" +
                std::to_string(addr_even) + ") as the adjacent even value "
                "(raw 0x2000), and an odd modulo (41) produces the SAME "
                "line-to-line step (" + std::to_string(step_even_mod) +
                " bytes) as 40 - data_in bit 0 is never wired into either "
                "register, so this hardware cannot express an odd byte "
                "address or an odd byte modulo. 'Keep in mind that the "
                "number of bytes you allocate for a bitplane must be "
                "even' (p.52), 'The number of bytes must be even' (p.76). "
                "3-mem.c05, 3-memory.c9, 3-big.c14.");
    }

    // ---------------- TWO-BITPLANES-SEPARATE-CONTIGUOUS-BLOCKS (p.53) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
#ifndef MUT_TWO_BITPLANES_SEPARATE_BLOCKS
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 2));  // BPU=2
#else
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));  // BPU=1: plane 1 never fetches
#endif
        constexpr uint32_t kBase0 = 0x00020000;
        constexpr uint32_t kBase1 = 0x00080000;
        h.write_ptr(0, kBase0);
        h.write_ptr(1, kBase1);
        std::vector<LineResult> frame = run_frame(h, 44);
        std::vector<uint32_t> p0, p1;
        for (auto& f : frame[44].fetches) {
            if (f.reg == 0x88) p0.push_back(f.address);
            if (f.reg == 0x89) p1.push_back(f.address);
        }
        auto contiguous_from = [](const std::vector<uint32_t>& v, uint32_t base) {
            if (v.size() != 20 || v.front() != base) return false;
            for (size_t i = 1; i < v.size(); ++i)
                if (v[i] != v[i - 1] + 2) return false;
            return true;
        };
        bool ok = contiguous_from(p0, kBase0) && contiguous_from(p1, kBase1);
        checks.expect(
            ok,
            "TWO-BITPLANES-SEPARATE-CONTIGUOUS-BLOCKS: two independently "
            "pointed bitplanes (reg 0x88 at 0x00020000, reg 0x89 at "
            "0x00080000) each produce their OWN gapless 20-word run "
            "starting exactly at their own pointer, and the two runs do "
            "not overlap - 'The memory for each bitplane must be "
            "continuous, so you need to have two ... blocks of available "
            "memory' (p.53). 3-mem.c11, 3-memory.c17.");
    }
    // ---------------- LINE-1-WORD-SEQUENCE (p.53-54, p.61) ----------------
    {
        Harness h;
        h.setup_standard();
#ifndef MUT_LINE_1_WORD_SEQUENCE
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
#else
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, 0x0068);  // window narrowed: fewer than 20 words
#endif
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        constexpr uint32_t kStart = 0x00030000;
        h.write_ptr(0, kStart);
        std::vector<LineResult> frame = run_frame(h, 44);
        std::vector<uint32_t> addrs;
        for (auto& f : frame[44].fetches) addrs.push_back(f.address);
        bool ok = addrs.size() == 20;
        for (size_t i = 0; i < addrs.size() && ok; ++i)
            if (addrs[i] != kStart + 2 * i) ok = false;
        checks.expect(
            ok,
            "LINE-1-WORD-SEQUENCE: a standard lores line's own fetched "
            "addresses run START, START+2, START+4, ... through START+38 "
            "(20 words) exactly - Figure 3-7's first row, 'Mem. location "
            "N' to 'Mem. location N+38' (p.53-54), and Figure 3-10's "
            "labelled first-line locations 'START, START+2, ... START+38' "
            "(p.61) - the identical arithmetic fact read from two printed "
            "figures. 3-mem.f01, 3-modulo.c8.");
    }

    // ---------------- LINE-2-CONTIGUOUS-SEQUENCE (p.61) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
#ifndef MUT_LINE_2_CONTIGUOUS_SEQUENCE
        h.write_reg(BPL1MOD_ADDR, 0x0000);
#else
        h.write_reg(BPL1MOD_ADDR, 0x0028);  // 40-byte gap: line 2 starts 40 bytes late
#endif
        constexpr uint32_t kStart = 0x00040000;
        h.write_ptr(0, kStart);
        std::vector<LineResult> frame = run_frame(h, 45);  // 44 = "line 1", 45 = "line 2"
        std::vector<uint32_t> line2;
        for (auto& f : frame[45].fetches) line2.push_back(f.address);
        bool ok = line2.size() == 20;
        for (size_t i = 0; i < line2.size() && ok; ++i)
            if (line2[i] != kStart + 40 + 2 * i) ok = false;
        checks.expect(
            ok,
            "LINE-2-CONTIGUOUS-SEQUENCE: with a zero modulo, the SECOND "
            "active line's fetched addresses run START+40, START+42, "
            "START+44, ... through START+78 exactly - line 2 begins "
            "precisely where line 1's own 20-word, 40-byte advance would "
            "put it, no gap at the line boundary - Figure 3-11's labelled "
            "second-line locations (p.61). 3-modulo.c13.");
    }

    // ---------------- FULL-FRAME-200TH-LINE (p.53-54) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
#ifndef MUT_FULL_FRAME_200TH_LINE
        h.write_reg(BPL1MOD_ADDR, 0x0000);
        constexpr int kLastActiveVpos = 44 + 199;  // the 200th active line
#else
        h.write_reg(BPL1MOD_ADDR, 0x0028);  // 40-byte gap added on every line: breaks the 7960/7998 arithmetic
        constexpr int kLastActiveVpos = 44 + 199;
#endif
        constexpr uint32_t kStart = 0x00050000;
        h.write_ptr(0, kStart);
        std::vector<LineResult> frame = run_frame(h, kLastActiveVpos);
        std::vector<uint32_t> line200;
        for (auto& f : frame[kLastActiveVpos].fetches) line200.push_back(f.address);
        bool ok = line200.size() == 20 && !line200.empty() &&
            line200.front() == kStart + 7960 && line200.back() == kStart + 7998;
        for (size_t i = 1; i < line200.size() && ok; ++i)
            if (line200[i] != line200[i - 1] + 2) ok = false;
        checks.expect(
            ok,
            "FULL-FRAME-200TH-LINE: driven for a genuine 200-line, "
            "zero-modulo, standard-lores run, the 200TH active line's own "
            "fetched addresses run START+7960 through START+7998 (20 "
            "words) exactly - (200-1)*40=7960 plus the last word of a "
            "20-word line at +38, both facts the manual itself states, "
            "not an RTL-observed number - 'Figure 3-7 shows an 8,000-byte "
            "memory area organized as 200 lines of 40 bytes each' (p.53), "
            "and the figure's own last row, 'Mem. location N+7960' to "
            "'Mem. location N+7998' (p.54). 3-memory.c18, 3-mem.f03.");
    }

    // ---------------- WIDE-PLAYFIELD-RIGHT-HALF-TWO-LINES (p.75) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
#ifndef MUT_WIDE_PLAYFIELD_RIGHT_HALF
        h.write_reg(BPL1MOD_ADDR, 0x0028);  // 40-byte modulo skips the undisplayed left half
#else
        h.write_reg(BPL1MOD_ADDR, 0x0000);  // no compensation: line 2 starts 40 bytes early
#endif
        constexpr uint32_t kBase = 0x00060000;   // the wide row's own "START"
        constexpr uint32_t kPtr = kBase + 40;    // pointer already 40 bytes into the row (right half)
        h.write_ptr(0, kPtr);
        std::vector<LineResult> frame = run_frame(h, 45);
        std::vector<uint32_t> line1, line2;
        for (auto& f : frame[44].fetches) line1.push_back(f.address);
        for (auto& f : frame[45].fetches) line2.push_back(f.address);
        bool ok = line1.size() == 20 && line2.size() == 20 &&
            line1.front() == kBase + 40 && line1.back() == kBase + 78 &&
            line2.front() == kBase + 120 && line2.back() == kBase + 158;
        checks.expect(
            ok,
            "WIDE-PLAYFIELD-RIGHT-HALF-TWO-LINES: with the pointer already "
            "40 bytes into an 80-byte-wide underlying row and a 40-byte "
            "modulo skipping the undisplayed left half of the next row, "
            "line 1 fetches START+40 through START+78 and line 2 fetches "
            "START+120 through START+158 exactly - Figure 3-17's "
            "right-half line-1 labels and Figure 3-18's right-half line-2 "
            "labels, companion figures of one worked example (p.75). "
            "3-large.c14, 3-large.c17.");
    }
    // ---------------- HIRES-SCROLL-TWO-EXTRA-WORDS (p.72) ----------------
    {
        auto word_count = [&](bool hires, uint16_t ddfstrt_raw, uint16_t ddfstop_raw) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, ddfstrt_raw);
            h.write_reg(DDFSTOP_ADDR, ddfstop_raw);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(hires, 1));
            std::vector<LineResult> frame = run_frame(h, 44);
            return frame[44].count();
        };
        int lores_a = word_count(false, DDFSTRT_LORES_STD, DDFSTOP_LORES_STD);          // $38
        int lores_b = word_count(false, DDFSTRT_LORES_STD - 8, DDFSTOP_LORES_STD);      // $30
        int hires_a = word_count(true, DDFSTRT_HIRES_STD, DDFSTOP_HIRES_STD);           // $3C
#ifndef MUT_HIRES_SCROLL_TWO_EXTRA_WORDS
        int hires_b = word_count(true, DDFSTRT_HIRES_STD - 8, DDFSTOP_HIRES_STD);       // $34, hires
#else
        int hires_b = word_count(false, DDFSTRT_HIRES_STD - 8, DDFSTOP_HIRES_STD);      // BUG: measured as lores
#endif
        int delta_lores = lores_b - lores_a;
        int delta_hires = hires_b - hires_a;
        checks.expect(
            delta_lores > 0 && delta_hires == 2 * delta_lores,
            "HIRES-SCROLL-TWO-EXTRA-WORDS: widening DDFSTRT by the SAME "
            "raw 8-unit amount adds " + std::to_string(delta_lores) +
                " word(s) in lores but " + std::to_string(delta_hires) +
                " word(s) in hires - exactly double, not equal - 'two "
                "words more in high resolution' to scroll a playfield, "
                "where the lores case (this fixture's own sibling package) "
                "needs only one (p.72). 3-dual.c35.");
    }

    // ---------------- DIW-SELECTS-SUBSET-OF-MEMORY-PICTURE (p.42) ----------------
    {
        auto active_lines = [&](uint16_t diwstop_raw, int to_vpos) {
            Harness h;
            h.write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
            h.write_reg(DIWSTOP_ADDR, diwstop_raw);
            h.write_reg(FMODE_ADDR, 0x0000);
            h.dut->dmaena = 1;
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            std::vector<LineResult> frame = run_frame(h, to_vpos);
            int n = 0;
            for (auto& line : frame) if (line.count() > 0) ++n;
            return n;
        };
        constexpr int kDriven = 309;
        int standard_active = active_lines(DIWSTOP_STD, kDriven);          // vdiwstop=300
#ifndef MUT_DIW_SELECTS_SUBSET
        int narrow_active = active_lines(0x9681, kDriven);                 // vdiwstop=150
#else
        int narrow_active = active_lines(DIWSTOP_STD, kDriven);            // no narrowing at all
#endif
        checks.expect(
            narrow_active < standard_active && standard_active < kDriven + 1,
            "DIW-SELECTS-SUBSET-OF-MEMORY-PICTURE: over the identical " +
                std::to_string(kDriven + 1) + "-line driven range, a "
                "narrowed display window fetches on only " +
                std::to_string(narrow_active) + " lines, versus " +
                std::to_string(standard_active) +
                " for the standard window - both are a PORTION of the "
                "driven range, and narrowing the window shrinks that "
                "portion - 'You choose which part of this larger memory "
                "picture to display by specifying a different size for "
                "the display window' (p.42). 3-video.c29.");
    }

    // ---------------- DIW-VERTICAL-ONE-LINE-RESOLUTION (p.57) ----------------
    {
        auto first_active_vpos = [&](uint8_t vstart_hi) {
            Harness h;
            h.write_reg(DIWSTRT_ADDR, static_cast<uint16_t>((vstart_hi << 8) | 0x81));
            h.write_reg(DIWSTOP_ADDR, DIWSTOP_STD);
            h.write_reg(FMODE_ADDR, 0x0000);
            h.dut->dmaena = 1;
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            std::vector<LineResult> frame = run_frame(h, 60);
            for (size_t v = 0; v < frame.size(); ++v)
                if (frame[v].count() > 0) return static_cast<int>(v);
            return -1;
        };
        int first_44 = first_active_vpos(44);
#ifndef MUT_DIW_VERTICAL_ONE_LINE_RESOLUTION
        int first_45 = first_active_vpos(45);
#else
        int first_45 = first_active_vpos(47);  // +3 raw units instead of +1
#endif
        checks.expect(
            first_44 >= 0 && first_45 - first_44 == 1,
            "DIW-VERTICAL-ONE-LINE-RESOLUTION: changing DIWSTRT's vertical "
            "start by exactly one raw unit moves the first active line by "
            "exactly one (line " + std::to_string(first_44) + " to line " +
                std::to_string(first_45) +
                "), never zero and never more than one - 'The resolution "
                "of vertical start and stop is one scan line' (p.57). "
                "3-window.c8.");
    }

    // ---------------- ECS-EXTENDS-DIW-RANGE (p.59) ----------------
    {
        auto active_at_400 = [&](bool ecs_on) {
            Harness h;
            h.dut->ecs = ecs_on ? 1 : 0;
            h.write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
            h.write_reg(DIWSTOP_ADDR, 0x2081);  // B=0x20: vdiwstop=288 without DIWHIGH
#ifndef MUT_ECS_EXTENDS_DIW_RANGE
            h.write_reg(DIWHIGH_ADDR, 0x0200);  // vdiwstop[10:8]<=3'b010 (only takes effect if ecs)
#else
            // no DIWHIGH write at all: ecs can no longer make a difference
#endif
            h.write_reg(FMODE_ADDR, 0x0000);
            h.dut->dmaena = 1;
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            std::vector<LineResult> frame = run_frame(h, 400);
            return frame[400].count() > 0;
        };
        bool active_ecs0 = active_at_400(false);
        bool active_ecs1 = active_at_400(true);
        checks.expect(
            !active_ecs0 && active_ecs1,
            "ECS-EXTENDS-DIW-RANGE: at vpos 400 (beyond the 288-line "
            "window DIWSTOP alone can reach), the fetch window is CLOSED "
            "under ecs=0 but still OPEN under ecs=1 with the identical "
            "DIWHIGH write - the extension only takes effect on ECS - "
            "'The minimum and maximum values for display windows have "
            "been extended in the enhanced version of the Amiga's custom "
            "chip set (ECS)' (p.59). 3-window.c59.");
    }
    // ---------------- DIWSTRT-ADDRESS (p.277, 290) ----------------
    {
        auto first_active_after_write = [&](uint16_t write_addr, uint8_t v_hi) {
            Harness h;
            h.write_reg(DIWSTOP_ADDR, DIWSTOP_STD);
            h.write_reg(FMODE_ADDR, 0x0000);
            h.dut->dmaena = 1;
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(write_addr, static_cast<uint16_t>((v_hi << 8) | 0x81));
            std::vector<LineResult> frame = run_frame(h, 150);
            for (size_t v = 0; v < frame.size(); ++v)
                if (frame[v].count() > 0) return static_cast<int>(v);
            return -1;
        };
        int correct = first_active_after_write(DIWSTRT_ADDR, 100);
#ifndef MUT_DIWSTRT_ADDRESS
        int wrong = first_active_after_write(0x0180, 100);  // an address this module decodes nowhere
#else
        int wrong = first_active_after_write(DIWSTRT_ADDR, 100);  // "wrong" probe is the real address again
#endif
        checks.expect(
            correct == 100 && wrong != 100,
            "DIWSTRT-ADDRESS: writing V=100 at $08E moves the first "
            "active line to exactly 100; writing the identical data at an "
            "address this module decodes nowhere leaves it at " +
                std::to_string(wrong) + ", not 100 - 'DIWSTRT 08E W A "
                "Display window start' (Appendix A p.277, Appendix B "
                "p.290). A-diw.c5, A-diwstrt.addr, B-diwstrt.addr, "
                "B-subject.c9.");
    }

    // ---------------- DIWSTOP-ADDRESS (p.277, 290) ----------------
    {
        auto still_open_at_200 = [&](uint16_t write_addr) {
            Harness h;
            h.write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
            h.write_reg(DIWSTOP_ADDR, DIWSTOP_STD);  // baseline: open through vpos 300
            h.write_reg(FMODE_ADDR, 0x0000);
            h.dut->dmaena = 1;
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(write_addr, 0x9681);  // attempts to narrow to vdiwstop=150
            std::vector<LineResult> frame = run_frame(h, 200);
            return frame[200].count() > 0;
        };
        bool correct_narrowed = !still_open_at_200(DIWSTOP_ADDR);
#ifndef MUT_DIWSTOP_ADDRESS
        bool wrong_unaffected = still_open_at_200(0x0180);
#else
        bool wrong_unaffected = still_open_at_200(DIWSTOP_ADDR);  // "wrong" probe is the real address again
#endif
        checks.expect(
            correct_narrowed && wrong_unaffected,
            "DIWSTOP-ADDRESS: writing a narrower value at $090 closes the "
            "window before vpos 200; writing the identical data at an "
            "address this module decodes nowhere leaves the wide baseline "
            "window untouched, still fetching at vpos 200 - 'DIWSTOP 090 "
            "W A Display window stop' (Appendix A p.277, Appendix B "
            "p.290). A-diw.c3, A-diwstop.addr, B-diwstop.addr, "
            "B-subject.c13.");
    }

    // ---------------- DIWHIGH-ADDRESS (p.277, 293) ----------------
    {
        auto active_at_400_after_write = [&](uint16_t write_addr) {
            Harness h;
            h.dut->ecs = 1;
            h.write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
            h.write_reg(DIWSTOP_ADDR, 0x2081);  // B=0x20: vdiwstop=288 baseline, no DIWHIGH needed
            h.write_reg(write_addr, 0x0200);    // attempts vdiwstop[10:8]<=3'b010 -> 544
            h.write_reg(FMODE_ADDR, 0x0000);
            h.dut->dmaena = 1;
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            std::vector<LineResult> frame = run_frame(h, 400);
            return frame[400].count() > 0;
        };
        bool correct_extended = active_at_400_after_write(DIWHIGH_ADDR);
#ifndef MUT_DIWHIGH_ADDRESS
        bool wrong_not_extended = !active_at_400_after_write(0x0180);
#else
        bool wrong_not_extended = !active_at_400_after_write(DIWHIGH_ADDR);  // "wrong" probe is the real address again
#endif
        checks.expect(
            correct_extended && wrong_not_extended,
            "DIWHIGH-ADDRESS: writing the extension bits at $1E4 (with "
            "ecs=1) opens the window out to vpos 400; writing the "
            "identical data at an address this module decodes nowhere "
            "leaves the 288-line baseline in place, closed by vpos 400 - "
            "'DIWHIGH 1E4 W A,D(E) Display window - upper bits for "
            "start, stop' (Appendix A p.277, Appendix B p.293). A-diw.c1, "
            "A-diwhigh.addr, B-diwhigh.addr, B-subject.c292.");
    }

    auto words_with = [&](bool hires, uint16_t strt, uint16_t stop) {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, strt);
        h.write_reg(DDFSTOP_ADDR, stop);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(hires, 1));
        std::vector<LineResult> frame = run_frame(h, 44);
        return frame[44].count();
    };

    // ---------------- MAX-WORDS-25-LORES (p.80) ----------------
    {
#ifndef MUT_MAX_WORDS_25_LORES
        int lores_max = words_with(false, DDFSTRT_HW_MIN, DDFSTOP_HW_MAX);
#else
        int lores_max = words_with(false, DDFSTRT_LORES_STD, DDFSTOP_LORES_STD);  // standard window, not the hw limit
#endif
        checks.expect(
            lores_max == 25,
            "MAX-WORDS-25-LORES: driven from beyond both hardware limits "
            "($00/$FE, past $18/$D8), a lores line fetches " +
                std::to_string(lores_max) + " words at most - Table "
                "3-14's own 'max words fetched' row states 25 for low "
                "resolution (p.80). 3-max-window.c27.");
    }

    // ---------------- MAX-WORDS-49-HIRES (p.80) ----------------
    {
#ifndef MUT_MAX_WORDS_49_HIRES
        int hires_max = words_with(true, DDFSTRT_HW_MIN, DDFSTOP_HW_MAX);
#else
        int hires_max = words_with(true, DDFSTRT_HIRES_STD, DDFSTOP_HIRES_STD);  // standard window, not the hw limit
#endif
        checks.expect(
            hires_max == 49,
            "MAX-WORDS-49-HIRES: driven from beyond both hardware limits "
            "($00/$FE, past $18/$D8), a hires line fetches " +
                std::to_string(hires_max) + " words at most - Table "
                "3-14's own 'max words fetched' row states 49 for high "
                "resolution (p.80); MAX-WORDS-25-LORES above is this "
                "assertion's control, same measurement basis, same "
                "hardware-limit setup, expected and found to PASS. "
                "3-max-window.c28.");
    }
    // ---------------- DECLINED (30 of the 64 claims) ----------------
    checks.note(
        "DECLINED, aggregate resolution/geometry arithmetic - a bitplane "
        "DMA engine fetches whatever window and however many lines it is "
        "driven for; it has no register or port naming a 'resolution', so "
        "a total-bytes-for-a-named-resolution figure requires presupposing "
        "the very line-count and interlace-factor the resolution label "
        "stands for, not something this boundary can witness as a single "
        "fact. Table 3-7 (NTSC, p.52): 3-memory.c5, 3-memory.c6, "
        "3-memory.c7, 3-memory.c8, 3-t7.c01, 3-t7.c02, 3-t7.c03, "
        "3-t7.c04. Table 3-8 (PAL, p.53): 3-memory.c10, 3-memory.c11, "
        "3-memory.c12, 3-memory.c13, 3-t8.c01, 3-t8.c02, 3-t8.c03, "
        "3-t8.c04. Two-bitplane playfield totals (p.53): 3-mem.c09, "
        "3-memory.c16. Wide-playfield worked arithmetic, 80*200*2=32,000 "
        "(p.76): 3-big.c15.");
    checks.note(
        "DECLINED, code-example comment line (p.55): 3-code.c08 "
        "('MOVE.W #2000,d0 ; Write 2000 longwords = 8000 bytes') - already "
        "flagged in the inventory itself as excluded by the extraction "
        "rules (a code-example line), and it restates the same 8,000-byte "
        "figure declined above besides.");
    checks.note(
        "DECLINED, documentation-provenance sentence naming no number of "
        "its own (p.59): 3-diw.c40 ('The minimum and maximum values for "
        "display windows have been extended') - a remark about which "
        "printed table belongs to which chipset generation, not itself a "
        "numeric behaviour to drive and observe. The substantive ECS/OCS "
        "gate this sentence's neighbour states is asserted separately as "
        "ECS-EXTENDS-DIW-RANGE, under a different id (3-window.c59).");
    checks.note(
        "DECLINED, blanking-limited DISPLAYABLE width, not this module's "
        "FETCHED width (p.80): 3-max-window.c16, 3-max.c10 ('368 low "
        "resolution pixels (23 words)') - what horizontal blanking lets "
        "Denise actually SHOW is a display-side concept this DMA-only "
        "module has no port for (it fetches 25 words at the hardware "
        "limits, MAX-WORDS-25-LORES above - MORE than 23, because extra "
        "words are fetched and then blanked, never displayed); '23 words' "
        "is also a bare division (368/16 pixels-per-lores-word), not a "
        "behaviour.");
    checks.note(
        "DECLINED, needs a wider boundary than this fixture, NTSC/PAL "
        "vertical-scroll step-size family (p.81): 3-vscr.c04, 3-vscr.c07, "
        "3-vscr.c08, 3-vscroll.c3, 3-vscroll.c4, 3-vscroll.c5, "
        "3-vscroll.c6. CONFIRMED by opening the RTL: agnus_bitplanedma.v's "
        "module header (l.29-47) has no ntsc/pal port at all - the "
        "200-vs-256-line distinction that '1/200th' versus '1/256th' is "
        "relative to lives on agnus_beamcounter (the module WITH that "
        "selector), not here. Separately, 'the smallest step is one line' "
        "is true of "
        "ANY pointer-addressed bitplane at ANY line count purely because "
        "a line's worth of bytes is the smallest meaningful vertical "
        "increment - not a restriction this module enforces, so no "
        "stimulus this module accepts could even in principle make it "
        "read otherwise.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    // A red result IS a valid, finished outcome. run_all.sh classifies a
    // package solely by its exit status, so returning 0 here would launder
    // any future red into a suite PASS.
    return checks.failed ? 1 : 0;
}

// ============================================================================
// MUTATION LOG (11.09.2026). Every entry below was actually built under
// WSL/Verilator (`make clean && make MUT=<name> test`), the changed
// PASS/FAIL line read from real stdout (quoted in the entries below), then
// reverted to a plain `make clean && make test` and reconfirmed back at
// this package's true baseline: 17 passed, 1 failed (MAX-WORDS-49-HIRES,
// unchanged) - not predicted. Every mutation is a STIMULUS change; no
// checks.expect() call's own expected-value logic was ever touched.
//
// Baseline before and after every mutation below: 17 passed, 1 failed.
// Applying any ONE mutation flag moved the count to 16 passed, 2 failed -
// exactly the targeted assertion, and no other - confirmed by reading each
// run's own full PASS/FAIL line set, not the one familiar label alone.
// MAX-WORDS-49-HIRES is the sole exception: it is ALREADY red at baseline
// (measured 50 words, not the manual's 49 - a genuine finding, left
// unadjusted per the build brief). Its own mutation does not flip a
// passed count (it was already counted failed), but it does change WHICH
// wrong number comes out (50 -> 40), proving the assertion tracks its own
// stimulus rather than returning a fixed value regardless of input.
//
// MUTATION FIXTURE-DELIVERS-KNOWN-WORD stimulus red: the written pointer changed from kBase to kBase+2 while the check still compares against kBase; the first fetched address no longer matched 0x00004000.
// MUTATION BITPLANE-CONTIGUOUS-WORD-SEQUENCE stimulus red: BPL1MOD changed from 0 to 0x0028 (40); a 40-byte gap appeared at the line boundary, breaking the strict +2-byte step across all 40 fetches.
// MUTATION TWO-BYTES-PER-FETCH stimulus red: aga raised to 1 and FMODE written to 1 (fmode[1:0]=01); the per-fetch step became 4 bytes, not 2 (and the fetched word count changed to 11).
// MUTATION BITPLANE-BYTES-ALWAYS-EVEN stimulus red: the second pointer probe changed from the adjacent odd raw value (0x2001) to the next WORD (0x2002), a genuinely different address; addr_odd no longer equalled addr_even.
// MUTATION TWO-BITPLANES-SEPARATE-CONTIGUOUS-BLOCKS stimulus red: BPU dropped from 2 to 1; plane 1 (reg 0x89) never fetched at all, failing its own contiguous-run check outright.
// MUTATION LINE-1-WORD-SEQUENCE stimulus red: DDFSTOP narrowed from 0x00D0 to 0x0068; fewer than 20 words were fetched, breaking the START..START+38 sequence.
// MUTATION LINE-2-CONTIGUOUS-SEQUENCE stimulus red: BPL1MOD changed from 0 to 0x0028 (40); line 2 started 40 bytes later than expected, not at START+40.
// MUTATION FULL-FRAME-200TH-LINE stimulus red: BPL1MOD changed from 0 to 0x0028 (40); the extra per-line gap accumulated over 200 lines, so the 200th line no longer began at START+7960.
// MUTATION WIDE-PLAYFIELD-RIGHT-HALF-TWO-LINES stimulus red: BPL1MOD changed from 40 to 0; with no compensation, line 2 began at START+80 instead of START+120.
// MUTATION HIRES-SCROLL-TWO-EXTRA-WORDS stimulus red: the hires_b measurement was taken with hires left false (a lores measurement mislabelled hires); the measured ratio collapsed (delta_hires=-19, not 2x delta_lores=2).
// MUTATION DIW-SELECTS-SUBSET-OF-MEMORY-PICTURE stimulus red: the "narrowed" DIWSTOP was changed to the standard value (no narrowing applied at all); narrow_active equalled standard_active (256==256) instead of being smaller.
// MUTATION DIW-VERTICAL-ONE-LINE-RESOLUTION stimulus red: the second DIWSTRT sample changed from +1 raw unit (45) to +3 (47); the measured line delta became 3 (line 44 to line 47), not 1.
// MUTATION ECS-EXTENDS-DIW-RANGE stimulus red: the DIWHIGH write was removed entirely; ecs could no longer make a difference, so the window stayed closed at vpos 400 under both ecs=0 and ecs=1.
// MUTATION DIWSTRT-ADDRESS stimulus red: the "wrong address" probe was changed to the real DIWSTRT address; both measurements read 100, so the required inequality (wrong != 100) failed.
// MUTATION DIWSTOP-ADDRESS stimulus red: the "wrong address" probe was changed to the real DIWSTOP address; the window narrowed in both measurements, so the required untouched-baseline case no longer held.
// MUTATION DIWHIGH-ADDRESS stimulus red: the "wrong address" probe was changed to the real DIWHIGH address; the window extended in both measurements, so the required unextended-baseline case no longer held.
// MUTATION MAX-WORDS-25-LORES stimulus red: DDFSTRT/DDFSTOP changed from the hardware-limit probe (0x00/0x00FE) to the standard lores window (0x0038/0x00D0); the measured maximum dropped to 20 words, not 25.
// MUTATION MAX-WORDS-49-HIRES stimulus red: DDFSTRT/DDFSTOP changed from the hardware-limit probe (0x00/0x00FE) to the standard hires window (0x003C/0x00D4); the wrong count changed from 50 to 40 - already red at baseline (a genuine finding, manual states 49), but the mutation shows the assertion tracks its own stimulus, not a fixed output.
// ============================================================================


