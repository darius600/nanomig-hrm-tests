// Test purpose: The bitplane pointer pair and the modulo - pointer writes
//   (BPLxPTH/BPLxPTL), the word-by-word advance of a pointer during a line,
//   BPL1MOD for odd-numbered planes and BPL2MOD for even-numbered planes,
//   and the address arithmetic that decides where the NEXT line starts.
//   Sibling packages on this same fixture already own the horizontal fetch
//   WINDOW (ch3-ddf-fetch-window: DDFSTRT/DDFSTOP, word counts) and the
//   register MAP (ch3-register-map-agnus: which 9-bit address decodes to
//   which field, address-decode selectivity only). This package is neither
//   of those: it is the pointer/modulo ARITHMETIC - what value a pointer
//   holds, how it moves, and what changes it between lines.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 42, 52, 53, 54, 60, 61, 62, 63, 64, 72, 73, 74, 75, 76, 80, 81,
//   82, 84, 85, 91, 270, 272, 277, 287, 288, 291.
// Non-goals: which 9-bit bus address selects which register (that is
//   ch3-register-map-agnus's own address-decode-selectivity boundary, not
//   repeated here); DDFSTRT/DDFSTOP fetch-window word counts and the
//   4-pixel/16-pixel granularity story (ch3-ddf-fetch-window's boundary);
//   BPLxDAT / bit-to-colour resolution (Denise's palette, not this DMA
//   module); the DMACON register's own bit decode (agnus_top's boundary -
//   this module only consumes the resulting `dmaena` level, which IS in
//   scope for claim 3-dma.c01 below).
// Claim ids: ids such as `B-pointer.c1` and `A-bplptr.c8` name one sentence of
//   the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch3.json
//   lists under `inventories`.
//
// BOUNDARY (from the build brief, not yet from opening the file):
//   nanomig/src/minimig-aga/agnus_bitplanedma.v:54 (BPLPTBASE_REG 9h0E0),
//   pointer banks l.217-242, pointer arithmetic l.484-493, address_out[20:1]
//   port l.46 - there is no address bit 0.
//
// ALREADY MEASURED on this fixture (d_bpldma, 10.09.2026 - not repeated
// here): NTSC 320x200 lores, one bitplane, DDFSTRT=$38/DDFSTOP=$D0/BPU=1
// gives exactly 8,000 fetched bytes total, addresses $0..$1F3E contiguous
// with step 2. BPL1MOD 0/40/80 gives a line-to-line address step of
// 40/80/120 bytes. NanoMig's pointer is address_out[20:1]
// (agnus_bitplanedma.v:46) - 20 significant bits, no bit 0.
//
// A CONFLICT THIS PACKAGE DOES NOT RESOLVE: the manual contradicts itself
// about the bitplane pointer's total width. Chapter text (p.54, quoted
// below as background, not itself in this package's claim set) is read
// elsewhere as describing a 19-bit address; Appendix A p.272 and Appendix B
// p.291 both say an 18-bit pointer, split "high 3 bits" (BPLxPTH) + "low 15
// bits" (BPLxPTL). NanoMig implements neither figure - the RTL boundary
// evidence above gives a 20-bit-significant address_out[20:1] port. This
// package treats the exact bit count as UNSETTLED and does not pick a side:
// every claim below that names a specific width (3 bits / 15 bits / 18 bits
// / 19 bits / 20 bits) is recorded as a MEASUREMENT, never asserted
// PASS/FAIL. What every printed source agrees on - that BPLxPTH and BPLxPTL
// together form ONE pointer, that the pointer must be even, and that it
// addresses chip memory - is exactly what IS asserted.
//
// CLAIMS (verbatim quote + restatement from
// the classify records for `ch3-bitplane-pointers-and-modulo` -- a build-time
// scratch file, never published and no longer on disk; the claim ids
// cited below are the surviving public record,
// all 173 records). Grouped by the proposition each supports, not by page
// order; ids restating the same proposition are grouped and cited together
// on whichever assertion(s) test that proposition.
//
// -- G1: BPLxPTH/BPLxPTL together hold a bitplane's starting memory address
//    (p.42, 52, 54, 91, 272) --
//   3-color.c5 (sol, p42): "For a four-color display, you build two
//     bitplanes in memory." (multi-plane existence, minimal case)
//   3-mem.c03 (opus, p52): "...set the bitplane pointer registers
//     (BPLxPTH/BPLxPTL) to point to the starting memory address of each
//     bitplane you are using."
//   3-mem.c04 (opus, p52): "The starting address is the memory word that
//     contains the bits of the upper left-hand corner of the bitplane."
//   3-memory.c3 (sol, p52): same idea as 3-mem.c03, second reading.
//   3-memory.c4 (sol, p52): same idea as 3-mem.c04, second reading.
//   3-memory.c24 (sol, p54): "Access to bitplanes in memory is provided by
//     two address registers, BPLxPTH and BPLxPTL, for each bitplane (12
//     registers in all)."
//   3-memory.c26 (sol, p54): "The 'x' position in the name holds the
//     bitplane number; for example BPL1PTH and BPL1PTL hold the starting
//     address of PLANE 1."
//   3-ptr.c01 (opus, p54): "Access to bitplanes in memory is provided by two
//     address registers..."
//   3-ptr.c03 (opus, p54): "...BPL1PTH and BPL1PTL hold the starting address
//     of PLANE 1."
//   3-regsum-pointers.c1 (sol, p91): "BPLxPTH - Bitplane Pointer (Bitplane
//     pointer high word, where x is the bitplane number)"
//   3-regsum-pointers.c2 (sol, p91): "BPLxPTL - Bitplane Pointer (Bitplane
//     pointer low word, where x is the bitplane number)"
//   3-rptr.c01 (opus, p91): "(Bitplane pointer high word, where x is the
//     bitplane number)"
//   3-rptr.c02 (opus, p91): "(Bitplane pointer low word, where x is the
//     bitplane number)"
//   A-bplptr.c8 (sol, p272): "This pair of registers contains the 18-bit
//     pointer to the address of bitplane x (x=1,2,3,4,5,6) DMA data." (the
//     "pair -> one pointer" half only; the "18-bit" figure is the disputed
//     width, recorded not asserted, see G6 below)
//   A-bplxpt.c02 (opus, p272): "...the address of bitplane x (x=1,2,3,4,5,6)
//     DMA data."
//   B-pointer.c1 (sol, p288): "Chip memory pointer that addresses DMA
//     data."
//
// -- G2: six bitplanes, twelve pointer registers in all (p.54) --
//   3-memory.c25 (sol, p54): "Access to bitplanes in memory is provided by
//     two address registers, BPLxPTH and BPLxPTL, for each bitplane (12
//     registers in all)." (the "12 registers, six bitplanes" half)
//   3-ptr.c02 (opus, p54): "BPLxPTH and BPLxPTL, for each bitplane (12
//     registers in all)."
//
// -- G3: the PTH/PTL pair may be written as one 32-bit long word, high word
//    to PTH (p.54, 63) --
//   3-memory.c28 (sol, p54): "68000 programmers may treat these as one
//     32-bit address and write to them as one long word."
//   3-memory.c29 (sol, p54): "You write to the high order word, which is
//     the register whose name ends in 'PTH.'"
//   3-ptr.c05 (opus, p54): "...may treat these as one 32-bit address and
//     write to them as one long word."
//   3-ptr.c06 (opus, p54): "You write to the high order word, which is the
//     register whose name ends in..."
//   3-sum.c02 (opus, p63): "BPLxPTH - pointer to bitplane starting position
//     in memory (written as a long word)."
//
// -- G4: bitplane pointers always contain an even number - the undisputed
//    sub-claim of the width conflict (p.61, 287) --
//   3-modulo.c14 (sol, p61): "Note that the pointers always contain an even
//     number, because data is fetched from the display a word at a time."
//   3-ptr.c12 (opus, p61): same sentence, second reading.
//   B-legend.c3 (sol, p287): "+ Address register pair. Must be an even
//     address pointing to chip memory."
//   B-legend.c4 (sol, p287): same legend entry, second reading (the "must be
//     even" half).
//
// -- G5: the pointer addresses chip memory and is a hardware-maintained,
//    software-loaded register - the module fetches from wherever it is
//    pointed, and does not reinitialize itself (p.272, 288) --
//   A-bplptr.c9 (sol, p272): "This pointer must be reinitialized by the
//     processor or copper to point to the beginning of bitplane data every
//     vertical blank time." (places the reinit duty on software, not
//     hardware - testable as "the module does not reset the pointer on its
//     own at a frame boundary")
//   B-pointer.c2 (sol, p288): "Must be reloaded by a processor before use
//     (vertical blank for bitplane and sprite pointers...)."
//
// -- G6: MEASUREMENT ONLY, not asserted - the disputed pointer width (p.54
//    background, 272, 291) --
//   A-bplptr.c3 (sol, p272): "BPLxPTH 0E0 W A Bitplane x pointer (high 3
//     bits)"
//   A-bplptr.c6 (sol, p272): "BPLxPTL 0E2 W A Bitplane x pointer (low 15
//     bits)"
//   A-bplxpth.fn (opus, p272): same BPLxPTH row, third reading. CONFLICT
//     (source_defines): "conflict: page 54 versus page 272 (and appendix B
//     page 291)".
//   A-bplxptl.fn (opus, p272): same BPLxPTL row, third reading. Same
//     CONFLICT note.
//   B-subject.c29/c36/c43/c50/c57/c64/c71/c78/c85/c92/c99/c106 (sol, p291):
//     Appendix B subject index, BPL1PTH..BPL6PTL, each restating its own
//     "(high 3 bits)"/"(low 15 bits)" field width. All twelve carry the same
//     CONFLICT note as A-bplxpth.fn/A-bplxptl.fn above (twelve ids: six
//     planes x {PTH,PTL}).
//   The remaining B-subject.c* ids for these same twelve rows (c30/31, c37/
//   38, c44/45, c51/52, c58/59, c65/66, c72/73, c79/80, c86/87, c93/94, c100/
//   101, c107/108 - "is one half of an address-register pair" / "must hold
//   an even address") are NOT part of this width conflict; they restate G1/
//   G4 instead and are cited there.
//
// -- G7: modulo definition - byte distance from the last word of one line
//    to the first word of the next (p.60, 61, 270, 288) --
//   3-mod.c01 (opus, p60): "Modulo refers to the number of bytes in memory
//     between the last word on one horizontal line and the beginning of the
//     first word on the next line. Thus, the modulo enables the system to
//     convert bitplane data..."
//   3-mod.c02 (opus, p60): same passage, "...stored in linear form...into
//     rectangular form."
//   3-modulo.c1 (sol, p60): "Modulo refers to the number of bytes in memory
//     between the last word on one horizontal line and the beginning of the
//     first word on the next line."
//   A-bplmod.c9 (sol, p270): "A modulo is a number that is automatically
//     added to the address at the end of each line, so that the address
//     then points to the start of the next line."
//   A-bplmod.c10 (sol, p270): same sentence, "the address...points to the
//     next line's start" half.
//   A-mod.c01 (opus, p270): same sentence, Appendix A reading.
//   B-mod.c1 (sol, p288): "15-bit modulo. A number that is automatically
//     added to the memory address at the end of each line to generate the
//     address for the beginning of the next line." (mechanism half; the
//     "15-bit" figure is verified against the RTL directly below - it is
//     NOT part of the pointer-width conflict in G6 and is not automatically
//     assumed correct either)
//   B-mod.c2 (sol, p288): same sentence, second reading.
//
// -- G8: modulo is zero when the memory picture equals the display window
//    (basic playfield) (p.61, 73) --
//   3-mod.c03 (opus, p61): "...the same size as the display window, the
//     modulo is zero because the memory [area contains exactly the same
//     number of bytes]."
//   3-modulo.c2 (sol, p61): "For the basic playfield, where the playfield in
//     memory is the same size as the display window, the modulo is zero..."
//   3-big.c01 (opus, p73): "If the big picture in memory is larger than the
//     display window, you must respecify the modulos. The modulo must be
//     some value other than 0."
//   3-big.c02 (opus, p73): same passage, "modulo must be some value other
//     than 0" half.
//   3-large.c1 (sol, p73): "If the big picture in memory is larger than the
//     display window, you must respecify the modulos."
//   3-large.c2 (sol, p73): "The modulo must be some value other than 0."
//
// -- G9: the modulo is added ONCE, at end-of-line (defined by DDFSTOP), to
//    the pointer already advanced by the line's own words; the worked
//    START..START+38 / next-line-START+40 example (p.61) --
//   3-mod.c05 (opus, p61): "the end-of-line condition is reached (defined by
//     the data-fetch register, DDFSTOP) the modulo is added to the bitplane
//     pointers, adjusting the pointer to the first word to be fetched for
//     the next horizontal line."
//   3-modulo.c5 (sol, p61): same sentence, second reading.
//   3-modulo.c6 (sol, p61): same sentence, "modulo is added...pointers"
//     half.
//   3-modulo.c7 (sol, p61): same sentence, "post-modulo pointer addresses
//     the first fetched word" half.
//   3-mod.f01 (opus, p61, figure): Figure 3-10, "Data for line 1:" location
//     row START, START+2, ..., START+38 - line 1 fetched as 20 words in
//     2-byte steps.
//   3-mod.f04 (opus, p61, figure): Figure 3-11, "Data for line 2:" location
//     row START+40, START+42, ..., START+78 - with modulo 0, line 2 begins
//     at START+40.
//   3-modulo.c10 (sol, p61): "After the first line is fetched, the bitplane
//     pointers BPLxPTH and BPLxPTL contain the value START+40."
//   3-modulo.c11 (sol, p61): "The modulo (in this case, 0) is added to the
//     current value of the pointer, so when the pointer begins the data
//     fetch for the next line, it fetches the data you want on that line."
//   3-modulo.c12 (sol, p61): "The data for the next line begins at memory
//     location START+40."
//   3-ptr.c07 (opus, p61): "The bitplane address pointers (BPLxPTH and
//     BPLxPTL) are used by the system to fetch the data to the screen.
//     These pointers are dynamic; once the data fetch begins, the pointers
//     are continuously incremented to point to the next word to be fetched
//     (data is fetched two bytes at a time)."
//   3-ptr.c08 (opus, p61): "...These pointers are dynamic;" (dynamism half)
//   3-ptr.c11 (opus, p61): "After the first line is fetched, the bitplane
//     pointers BPLxPTH and BPLxPTL contain the value START+40. The modulo
//     (in this case, 0) is added..."
//   3-mem.c10 (opus, p53): "...The memory for each bitplane must be
//     continuous." (the word-by-word advance has no gaps within a line -
//     the concrete, RTL-measurable shape of "continuous" at this boundary)
//
// -- G10: playfield redisplay requires resetting the bitplane pointers -
//    without a fresh write, the pointer keeps whatever the last fetch left
//    it at, it does not rewind itself (p.63) --
//   3-display.c2 (sol, p63): "Each time the playfield is redisplayed, you
//     have to reset the bitplane pointers."
//   3-display.c3 (sol, p63): "Resetting is necessary because the pointers
//     have been incremented to point to each successive word in memory..."
//   3-display.c4 (sol, p63): "...and must be repointed to the first word for
//     the next display."
//   3-redis.c01 (opus, p63): "Each time the playfield is redisplayed, you
//     have to reset the bitplane..."
//
// -- G11: chapter/appendix summary of modulo defaults - 0 for non-interlaced,
//    40 for interlaced (p.64) --
//   3-sum.c18 (opus, p64): "5. Define Modulo. Set registers BPL1MOD and
//     BPL2MOD. Set modulo to 0 for non-interlaced, 40 for interlaced."
//   3-sum.c19 (opus, p64): same line, "...40 for interlaced" half.
//   3-summary.c5 (sol, p64): "Set modulo to 0 for non-interlaced, 40 for
//     interlaced." (0 half)
//   3-summary.c6 (sol, p64): same sentence (40 half).
//
// -- G12: two independent modulo registers - BPL1MOD for odd-numbered
//    planes, BPL2MOD for even-numbered planes, usable for differing
//    dual-playfield widths (p.62, 91, 270, 288, 291) --
//   3-mod.c06 (opus, p62): "There are two modulo registers -- BPL1MOD for
//     the odd-numbered bitplanes..."
//   3-mod.c07 (opus, p62): "...and BPL2MOD for the even-numbered bitplanes.
//     This allows for differing..."
//   3-mod.c08 (opus, p62): "...modulos for each playfield in dual-playfield
//     mode."
//   3-modulo.c16 (sol, p62): "There are two modulo registers -- BPL1MOD for
//     the odd-numbered bitplanes and BPL2MOD for the even-numbered
//     bitplanes." (BPL1MOD/odd half)
//   3-modulo.c17 (sol, p62): same sentence (BPL2MOD/even half).
//   3-modulo.c18 (sol, p62): "This allows for differing modulos for each
//     playfield in dual-playfield mode."
//   3-regsum-mod.c1 (sol, p91): "BPL1MOD - Bitplane Modulo (Odd-numbered
//     bitplanes, playfield 1)"
//   3-regsum-mod.c2 (sol, p91): "BPL2MOD - Bitplane Modulo (Even-numbered
//     bitplanes, playfield 2)"
//   3-rmod.c01 (opus, p91): "(Odd-numbered bitplanes, playfield 1)"
//   3-rmod.c02 (opus, p91): "(Even-numbered bitplanes, playfield 2)"
//   A-bpl1mod.fn (opus, p270): "BPL1MOD 108 W A Bitplane modulo (odd
//     planes)"
//   A-bpl2mod.fn (opus, p270): "BPL2MOD 10A W A Bitplane modulo (even
//     planes)"
//   A-bplmod.c4 (sol, p270): same BPL1MOD row, second reading.
//   A-bplmod.c8 (sol, p270): same BPL2MOD row, second reading.
//   A-bplmod.c11 (sol, p270): "Since they have separate modulos, the odd and
//     even bitplanes may have sizes that are different from each other, as
//     well as different from the display window size."
//   A-bplmod.c12 (sol, p270): same sentence, second reading.
//   A-mod.c02 (opus, p270): same sentence, Appendix A reading.
//   B-subject.c129 (sol, p291): "BPL1MOD 108 W A Bitplane modulo (odd
//     planes)"
//   B-subject.c133 (sol, p291): "BPL2MOD 10A W A Bitplane modulo (even
//     planes)"
//
// -- G13: interlaced redisplay uses modulo 40 to skip the other field's
//    lines; odd/even fields start their pointers at START/START+40 - the
//    STARTING value is a software choice (Copper/CPU), the modulo-40
//    line-to-line skip is the hardware mechanism this package can measure
//    (p.62, 64) --
//   3-mod.c09 (opus, p62): "For interlaced mode, you must redefine the
//     modulo, because interlaced mode..."
//   3-mod.c16 (opus, p62): "...Therefore, you use a modulo of 40 to skip the
//     lines in the other field. For odd fields, the bitplane pointers begin
//     at START. For even fields..."
//   3-mod.c17 (opus, p62): "...For odd fields, the bitplane pointers begin
//     at START."
//   3-mod.c18 (opus, p62): "...even fields, the bitplane pointers begin at
//     START+40."
//   3-modulo.c28 (sol, p62): "Therefore, you use a modulo of 40 to skip the
//     lines in the other field."
//   3-modulo.c29 (sol, p62): "For odd fields, the bitplane pointers begin at
//     START."
//   3-modulo.c30 (sol, p62): "For even fields, the bitplane pointers begin
//     at START+40."
//   3-copper-redisplay.c1 (sol, p62): "You can use the Copper to handle
//     resetting of the bitplane pointers for interlaced displays." (a
//     SOFTWARE-usage note - the Copper is one way to perform the ordinary
//     register write this module has no way to distinguish from a CPU
//     write; see NOT ASSERTED below)
//
// -- G14: bitplane modulos limit playfield WIDTH (not height) to 262,144
//    pixels (p.76, 277) --
//   3-big.c19 (opus, p76): "...Another constraint on playfield size is the
//     bit plane modulos which limit the width (but not the height) of a
//     playfield to 262,144 pixels."
//   3-big.c20 (opus, p76): same sentence, "262,144 pixels" half.
//   3-large.c24 (sol, p76): "Another constraint on playfield size is the bit
//     plane modulos which limit the width (but not the height) of a
//     playfield to 262,144 pixels." (width-limit half)
//   3-large.c25 (sol, p76): same sentence (height-unconstrained half).
//   A-ddf.c8 (sol, p277): "The bitplane modulos are dependent on the
//     bitplane horizontal size and on this data-fetch window size." (context
//     for G14/G8 - too general for its own pass/fail, cited alongside them)
//
// -- G15: worked example, modulo=40 (wider-than-window playfield): line 1
//    still START..START+38 (unchanged from modulo 0), pointer after line 1
//    = START+40, +modulo(40) => line 2 starts at START+80 (p.74) --
//   3-big.c06 (opus, p74): "At this point, BPLxPTH and BPLxPTL contain the
//     value START+40."
//   3-big.c07 (opus, p74): "...modulo, which is 40, is added to the current
//     value of the pointer so that..."
//   3-big.f03 (opus, p74, figure): Figure 3-16, "Data for line 2:" location
//     row START+80, START+82, ..., START+118 - with modulo 40, line 2 begins
//     at START+80.
//   3-large.c7 (sol, p74, figure): Figure 3-15, line 1 labels START through
//     START+38 when modulo is 40 (unchanged from the modulo-0 case).
//   3-large.c9 (sol, p74): "At this point, BPLxPTH and BPLxPTL contain the
//     value START+40."
//   3-large.c10 (sol, p74): "The modulo, which is 40, is added to the
//     current value of the pointer so that when it begins the data fetch
//     for the next line, it fetches the data that you intend for that
//     line."
//   3-large.c11 (sol, p74, figure): Figure 3-16, line 2 labels START+80
//     through START+118 when modulo is 40.
//
// -- G16: the modulo mechanism does not care about the pointer's own base
//    value - selecting a different horizontal slice (START+40 instead of
//    START) with the same modulo(40) still steps by the same amount line to
//    line (p.75) --
//   3-big.c08 (opus, p75): "START+40 rather than START with the modulo
//     remaining at 40. The data..."
//   3-big.c09 (opus, p75): same passage, "the modulo remaining at 40" half.
//   3-big.c10 (opus, p75): "Now, the bitplane pointers contain the value
//     START+80. The modulo (40) is..."
//   3-large.c12 (sol, p75): "To display the right half of the big picture,
//     you set up a vertical blanking routine to start the bitplane pointers
//     at location START+40 rather than START with the modulo remaining at
//     40."
//   3-large.c13 (sol, p75): same sentence, "modulo remaining at 40" half.
//   3-large.c16 (sol, p75): "The modulo (40) is added to the pointers so
//     that when they begin the data fetch for the second line, the correct
//     data is fetched."
//
// -- G17: horizontal-scroll modulo compensation formula - modulo = (address
//    of the next desired word) - (address of the last word fetched) - 2;
//    worked example, 21-word/42-byte scrolled line 1 -> pointer START+42,
//    +modulo(38) => line 2 at START+80, matching the unscrolled step (p.82,
//    84) --
//   3-hscr.c13 (opus, p82): "As always, the modulo is two counts less than
//     the difference between the address of the next word you want to fetch
//     and the address of the last word that was fetched."
//   3-hscroll.c15 (sol, p82): same sentence, second reading.
//   3-hscr.c18 (opus, p84): "At this point, the bitplane pointers contain
//     the value START+42. Adding..."
//   3-hscr.c19 (opus, p84): "...the modulo of 38 gives the correct starting
//     point for the next line."
//   3-hscroll.c24 (sol, p84): "At this point, the bitplane pointers contain
//     the value START+42."
//   3-hscroll.c25 (sol, p84): "Adding the modulo of 38 gives the correct
//     starting point for the next line."
//
// -- G18: dual-playfield scroll compensation - holding one playfield
//    stationary while the other scrolls requires offsetting ITS pointers and
//    modulo by -2 (lores) / -4 (hires); the modulo must be written to BOTH
//    BPL1MOD and BPL2MOD in dual-playfield mode (p.72) --
//   3-dual-scroll.c5 (sol, p72): "In low resolution mode, you adjust the
//     pointers by -2 and the modulo by -2."
//   3-dual-scroll.c7 (sol, p72): "In high resolution mode, you adjust the
//     pointers by -4 and the modulo by -4."
//   3-dual.c38 (opus, p72): "...you must adjust the modulo and the bitplane
//     pointers of the playfield that is not being scrolled to maintain its
//     position on the display. In low resolution mode, you adjust the
//     pointers by -2 and the modulo by -2. In high resolution mode, you
//     adjust the pointers by -4 and the modulo by -4."
//   3-dual.c51 (opus, p72): "Write the modulo to both BPL1MOD and BPL2MOD as
//     you will be..."
//
// -- G19: vertical scroll - progressively stepping the pointer's STARTING
//    value by one line's worth per field moves which memory line appears at
//    the top of the display; increasing moves down through the picture,
//    decreasing moves up; pointers are reset during vertical blanking
//    (p.80, 81, 85) --
//   3-scr.c01 (opus, p80): "In vertical scrolling, the playfield appears to
//     move smoothly up or down..."
//   3-scr.c02 (opus, p80): "...All you need do for vertical scrolling is
//     progressively increase or decrease the starting address for the
//     bitplane pointers by the size of a horizontal line in the
//     playfield..."
//   3-vscr.c01 (opus, p81): "...the bitplane pointers start at a
//     progressively [higher or lower place]."
//   3-vscr.c09 (opus, p81): "...have to increase by enough to ensure that
//     the pointers begin one line [lower each time]."
//   3-vscr.c10 (opus, p81): "...For a normal-sized, low resolution display
//     in which the modulo is 0, the pointers would be incremented by 40
//     bytes each time."
//   3-vscr.f01 (opus, p81, figure): Figure 3-23, "Bitplane pointer start
//     address" labels the top-left corner of the visible display frame
//     within the larger memory picture.
//   3-vscr.f02 (opus, p81, figure): Figure 3-23, increasing the pointer
//     shows more of the LOWER part of the picture; decreasing shows more of
//     the UPPER part.
//   3-vscroll.c1 (sol, p81): "As the value of the pointer increases, more of
//     the lower part of the picture is shown and the picture appears to
//     scroll upward."
//   3-vscroll.c2 (sol, p81): "As the value of the pointer decreases, more of
//     the upper part is shown and the picture scrolls downward."
//   3-vscr.c11 (opus, p85): "Reset BPLxPTH and BPLxPTL during the vertical
//     blanking interval." (ties to G5/G10 - the reset is a software action
//     during a specific interval, not a self-resetting hardware behaviour)
//
// -- G20: bitplane DMA (and therefore pointer advance) only runs once the
//    pointers are set AND DMA is turned on - both conditions independently
//    necessary (p.62) --
//   3-dma.c01 (opus, p62): "You start playfield display by making certain
//     that the bitplane pointers are set and bitplane DMA is turned on. You
//     turn on bitplane DMA by writing a 1 to bit BPLEN in the DMACON..."
//     (the DMACON bit-decode itself is agnus_top's boundary; this module's
//     own `dmaena` input is the level DMACON's decode ultimately produces,
//     and is in scope)
//
// -- NOT ASSERTED (software/CPU technique, or a different module's
//    boundary, not a hardware behaviour of agnus_bitplanedma) --
//   3-copper-redisplay.c1 (sol, p62): "You can use the Copper to handle
//     resetting of the bitplane pointers for interlaced displays." - the
//     Copper performs an ordinary register write; nothing on this module's
//     ports distinguishes "the Copper wrote this" from "the CPU wrote
//     this," exactly the same reasoning ch3-ddf-fetch-window already
//     applied to its own Copper-usage claim.
//   A-bplxdat.c06 (opus, p272): "...triggered whenever bitplane #1 is
//     written, indicating the completion of all bitplanes for that word (16
//     pixels). The MSB is output first..." - describes the BPLxDAT data
//     register's own write-completion signalling, not the pointer/DMA
//     address path this module owns; this module has no data-value port at
//     all (confirmed already by the sibling ch3-ddf-fetch-window package
//     reading the same port list).
//
// PLANNED ASSERTIONS (names, id groups and intent only; no RTL line numbers,
// no fixture code below this comment block - added in the next commit, after
// agnus_bitplanedma.v is opened for the first time):
//
//  0. FIXTURE-DELIVERS-KNOWN-POINTERS-IN-ORDER [self-proof, no page]: write
//     several DISTINCT, easily-told-apart pointer values, one per bitplane,
//     and show each arrives, unchanged and in order, as that plane's own
//     fetched address stream - the precondition every arithmetic assertion
//     below depends on. Cites G1/G5's own statement of what a pointer is
//     for (B-pointer.c1, A-bplptr.c8's "pair -> pointer" half).
//
//  1. POINTER-PAIR-IS-ONE-ADDRESS (G1, G3): BPLxPTH and BPLxPTL, written
//     independently, combine into exactly one starting address - writing
//     PTH then PTL (or PTL then PTH) produces the identical fetched address
//     either order, matching "treat these as one 32-bit address" (p.54).
//
//  2. SIX-PLANE-TWELVE-REGISTER-BANK (G2): all six OCS-scope planes
//     (BPL1..BPL6) hold independent pointers at once - extends assertion
//     0's proof from four planes to all six the manual's "12 registers in
//     all" sentence names.
//
//  3. POINTER-ALWAYS-EVEN (G4): writing an ODD raw value to a PTL register
//     still produces an EVEN fetched address - the concrete, RTL-measurable
//     shape of "the pointers always contain an even number" (p.61) and "+
//     ...Must be an even address" (p.287).
//
//  4. POINTER-NOT-SELF-RESETTING (G5, G10, G19c): across a frame boundary
//     (vpos wraps) with NO new pointer write, the pointer keeps advancing
//     from wherever the last fetch left it rather than rewinding to its
//     original value - the operational form of "must be reinitialized BY
//     THE PROCESSOR OR COPPER" (p.272) and "you have to reset the bitplane
//     pointers" (p.63): the hardware does not do this by itself.
//
//  5. POINTER-WIDTH-DISPUTED [measurement only, G6]: records the RTL's own
//     field split for BPLxPTH/BPLxPTL against the three disputed printed
//     figures, with no PASS/FAIL attached to any of them.
//
//  6. MODULO-ZERO-CONTIGUOUS-LINES (G7, G8, G9): with modulo=0 and a
//     40-byte line, this package's OWN control re-verification of the
//     pre-supplied measurement - line 1 fetched START..START+38, pointer
//     afterward START+40, line 2 begins exactly at START+40 (Figure 3-10/
//     3-11's own worked numbers) - and the word-by-word advance within the
//     line is a plain, gap-free +2-byte step (the "continuous" memory
//     claim, 3-mem.c10).
//
//  7. MODULO-NONZERO-FOR-WIDER-PLAYFIELD (G8, G15): modulo=40 (memory
//     picture wider than the display window) reproduces Figure 3-15/3-16's
//     worked numbers exactly - line 1 UNCHANGED at START..START+38, pointer
//     afterward START+40, +modulo(40) => line 2 begins at START+80.
//
//  8. MODULO-INDEPENDENT-OF-BASE-VALUE (G16): the SAME modulo(40) applied
//     from a different starting pointer (START+40, selecting the picture's
//     right half) produces the SAME 80-byte line-to-line step as assertion
//     7 - the mechanism does not care what value it is added to.
//
//  9. BPL1MOD-ODD-BPL2MOD-EVEN-INDEPENDENT (G12): with multiple planes
//     active, BPL1MOD and BPL2MOD each independently control only their own
//     named group's (odd-numbered / even-numbered) line-to-line step -
//     changing one leaves the other's step unaffected.
//
//  10. MODULO-40-FIELD-SKIP (G13): BPL1MOD/BPL2MOD=40 on a normal-width
//     (40-byte) line gives an 80-byte line-to-line step - twice the
//     ordinary 40-byte step - the concrete, hardware-measurable shape of
//     "use a modulo of 40 to skip the lines in the other field," with the
//     odd/even STARTING value itself (START vs START+40) noted as the
//     software-chosen half this module does not decide.
//
//  11. MODULO-DEFAULT-SUMMARY (G11): re-cites assertions 6 and 10 (modulo 0
//     / modulo 40) directly against the chapter summary's own "Set modulo
//     to 0 for non-interlaced, 40 for interlaced" line - same runs, not a
//     separate measurement basis.
//
//  12. HSCROLL-MODULO-COMPENSATION-FORMULA (G17): with a 21-word (42-byte)
//     scrolled fetch and modulo=38, the pointer after line 1 (START+42)
//     plus modulo lands exactly on START+80 - the SAME line-to-line step as
//     an unscrolled 20-word/modulo-40 line (assertion 6/10's own basis) -
//     confirming "modulo is two counts less than the difference" holds
//     arithmetically, not just as the one printed instance.
//
//  13. DUAL-PLAYFIELD-SCROLL-POINTER-MODULO-OFFSET (G18): in dual-playfield
//     mode, offsetting one playfield's pointer AND modulo by -2 (lores)
//     holds its displayed content stationary while the other playfield's
//     own pointer/modulo scroll normally - both BPL1MOD and BPL2MOD driving
//     their own plane group simultaneously, extending assertion 9 to the
//     scroll-compensation case specifically.
//
//  14. VERTICAL-SCROLL-POINTER-STEP (G19): writing a progressively
//     different STARTING pointer value before each of several successive
//     single-line runs (simulating one per field) steps which memory line
//     is fetched by exactly one line's worth (40 bytes at modulo 0) per
//     step, in the increasing = "reveals lower picture" direction the
//     manual states.
//
//  15. MODULO-WIDTH-262144-PIXEL-LIMIT (G14): measures the modulo field's
//     actual signed range against the RTL (not assumed from the disputed
//     15-bit-plus-sign figure) and checks whether a playfield many lines
//     taller than any modulo-range figure still fetches every line
//     correctly - the "limits width, not height" half of the claim, tested
//     directly rather than assumed.
//
//  16. BPLEN-DMA-GATES-POINTER-ADVANCE (G20): with the module's own dmaena
//     input held low, a fully-set, in-range pointer produces NO fetches at
//     all; raising dmaena with the SAME pointer produces the expected
//     stream - "pointers are set AND bitplane DMA is turned on" as two
//     independently necessary conditions.
//
// MEASUREMENT RECORDS (not assertions):
//   - G6 (pointer width): the disputed 3-bit/15-bit/18-bit (Appendix A/B)
//     versus chapter-text 19-bit versus this RTL's 20-bit-significant
//     address_out[20:1] figures - recorded, never asserted against each
//     other.
//   - B-mod.c1/c2's "15-bit modulo...plus sign extend" (B-mod.c4, NOT part
//     of this package's claim set but part of the same appendix passage) -
//     the modulo register's actual bit width is measured directly against
//     the RTL once opened, not assumed from the printed figure, per assertion
//     15 above.
//
// No RTL has been opened yet to write this comment block. Every port name,
// register offset and bit-slice fact above (BPLPTBASE_REG, pointer banks,
// pointer arithmetic, address_out[20:1]) comes from this package's own build
// brief (PACKAGE.md), and every numeric fetch-count/address/line-count fact
// comes from the ALREADY MEASURED section above, itself handed to this
// package rather than re-derived. The fixture, Harness helpers and assertion
// bodies are added in the next commit, after agnus_bitplanedma.v is read for
// the first time.

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after agnus_bitplanedma.v was
// read in full for the first time - the specification comment above this
// point was committed on its own first, per the chapter's spec-before-RTL
// gate; see this package's own git history for the spec-only commit).
//
// PORTS (agnus_bitplanedma.v:29-47): clk, clk7_en, reset, harddis, aga, ecs,
// a1k, sof, dmaena, vpos[10:0], hpos[8:0] are inputs; hde, dma, address_out
// [20:1], reg_address_out[8:1] are outputs; reg_address_in[8:1]/data_in
// [15:0] are the register write bus - identical port list to the sibling
// packages on this same fixture (ch3-ddf-fetch-window, ch3-register-map-
// agnus). hpos/vpos are driven in from outside, exactly as those siblings'
// own proven-good Harness/run_to_line/prime_to_line/run_single_line drive
// pattern already does - reused here verbatim rather than reinvented.
//
// POINTER BANK (l.217-242, 82-84): bplpth is `reg [20:16] bplpth [7:0]` (a
// FIVE-bit high field, 8 entries) and bplptl is `reg [15:1] bplptl [7:0]`
// (a FIFTEEN-bit low field, 8 entries). Write decode: `bplptr_sel = dma ?
// plane[2:0] : reg_address_in[4:2]` selects the bank (8 planes addressable,
// BASE+4N for N=0..7); `bplpth_in = dma ? newpt[20:16] : data_in[4:0]`
// captures FIVE raw bits from data_in on a PTH write (reg_address_in[1]==0),
// `bplptl_in = dma ? newpt[15:1] : data_in[15:1]` captures FIFTEEN raw bits
// on a PTL write (reg_address_in[1]==1) - data_in's own bit 0 is never
// read for PTL, exactly the same "pointer bit 0 does not exist" shape as
// the pointer/modulo arithmetic below. `address_out[20:16] = bplpth[plane]`,
// `address_out[15:1] = bplptl[plane]` - the two banks compose into ONE
// 20-bit-significant address with no independent adjustment; this directly
// settles G3 (PTH/PTL as one 32-bit write) at the write-decode level: PTH
// and PTL land in two disjoint bit ranges of the SAME address, so writing
// them in either order produces the identical final address (assertion 1).
//
// NEITHER bplpth NOR bplptl HAS A RESET PATH. Read literally: neither
// always-block that drives them (l.223-228, 235-240) has an `if (reset)`
// branch of any kind - they are written ONLY on a `dma` cycle (the pointer's
// own auto-increment) or a matching bus write. This is the RTL-measurable
// form of A-bplptr.c9/B-pointer.c2 ("must be reinitialized by the processor
// or copper"/"must be reloaded by a processor before use"): nothing in this
// module ever reinitializes a pointer on its own; assertion 4 demonstrates
// the operational consequence directly (multiple lines of pure forward
// advance with no register write, then an explicit rewrite that DOES take
// effect).
//
// POINTER/MODULO ARITHMETIC (l.483-493): `newpt[20:1] = address_out[20:1] +
// (fmode[1:0]==2'b11 ? 4 : fmode[1:0]==2'b00 ? 1 : 2)` ordinarily (FMODE=0,
// this package's whole basis per the ALREADY MEASURED section, gives +1 -
// i.e. +2 bytes once Verilator's [20:1]-indexed port is read back and
// doubled, matching the pre-supplied "address_out stepping 2 bytes"
// measurement exactly). When `mod` is asserted (once per line, at the last
// DMA cycle - l.443-454), the SAME cycle instead computes `newpt[20:1] =
// address_out[20:1] + {sign-extend(bplNmod_bscan[15]), bplNmod_bscan[15:1]}
// + (fmode-based +1/+2/+4)` - i.e. the ordinary word-advance and the modulo
// add happen TOGETHER on the line's last fetch, exactly the worked
// START+40-then-modulo arithmetic p.61's Figure 3-10/3-11 describe.
// `plane[0]` selects which modulo register: plane index 0 (BPL1, an
// ODD-numbered plane in the manual's 1-based naming) takes the `else`
// branch -> bpl1mod_bscan; plane index 1 (BPL2, EVEN-numbered) takes the
// `if(plane[0])` branch -> bpl2mod_bscan (l.486-489) - confirmed from the
// raw index parity, not assumed, and it lines up with "BPL1MOD for the
// odd-numbered bitplanes, BPL2MOD for the even-numbered" (p.62) exactly
// because the manual's plane numbering is 1-based while this array is
// 0-based (plane 1 = index 0 = even index = "odd-numbered" in the manual).
//
// MODULO REGISTER WIDTH (l.57-58, 67-70, 262-274): bpl1mod/bpl2mod are
// `reg [15:1]`, captured from `data_in[15:1]` on a matching write (data_in's
// own bit 0 never read, same "always even" shape as the pointers). In the
// arithmetic the field is sign-extended from its own top bit (`[15]`, i.e.
// the ORIGINAL data_in bit 15) across 5 more bits before being added to
// address_out[20:1] - a 15-bit-plus-sign-extend modulo, which is exactly
// what B-mod.c1/B-mod.c4 (Appendix B's MOD legend entry - not itself a
// claim id in this package's 173, but the same passage as this package's own
// B-mod.c1/c2/c3) states, and is NOT part of the pointer-width conflict
// (G6): this is a separate register with its own separate width, measured
// directly here (assertion 15) rather than assumed correct because it
// happens to agree.
//
// ddfstrt/ddfstop/BPLCON0/DIWSTRT/DIWSTOP decode, hardware $18/$D8 limits,
// hde generation, and the fetch-window/word-count sequencer (l.50-60, 336-
// 493) are identical to what ch3-ddf-fetch-window's own IMPLEMENTATION
// NOTES already documented on this fixture - not re-derived here, only
// reused where a pointer/modulo assertion needs a known fetch window (the
// standard lores/hires DDFSTRT/DDFSTOP pairs, and the DDFSTRT=$0030 scrolled
// case for assertions 12/13).
//
// bpu (l.298-300): with `aga`=0 (this package's default, matching the
// manual's OCS/ECS-era six-bitplane scope), `bpu = bplcon0_delayed[2:0]`
// unless that field is 3'b111 (forced to 4 - the real "dual-playfield 4+4"
// encoding) - so BPU=6 (three odd-numbered + three even-numbered planes,
// exactly six pointer pairs) is directly settable and used by assertion 2.
//
// dmaena (l.316-322, 472): `dmaena_delayed` is a 2-tick shift register of
// the raw `dmaena` input, and `dma = ddfrun && dmaena_delayed[1] && ...` -
// dmaena gates dma multiplicatively (an AND term), confirming "pointers set
// AND DMA on" (3-dma.c01) as two independently necessary conditions at the
// RTL level, not merely as two facts that happen to coincide in the
// examples above (assertion 16).
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
// Standard DIW, matching the "starting position of ($81,$2C) centers a
// normal size display" example (p.57) and the sibling packages' own known-
// good values on this fixture.
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
    // ($81,$2C) example, dmaena on, harddis off, FMODE=0/BPL1MOD=BPL2MOD=0
    // (this package's own explicit baseline - never relying on Verilator's
    // implicit zero-init for a register the RTL itself never resets).
    void setup_standard(bool ecs_on = true) {
        dut->ecs = ecs_on ? 1 : 0;
        write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
        write_reg(DIWSTOP_ADDR, DIWSTOP_STD);
        write_reg(FMODE_ADDR, 0x0000);
        write_reg(BPL1MOD_ADDR, 0x0000);
        write_reg(BPL2MOD_ADDR, 0x0000);
        dut->dmaena = 1;
    }

    Vagnus_bitplanedma* dut;
};

struct Fetch {
    uint32_t address;   // address_out, real byte address (Verilator's raw
                         // [20:1]-indexed read, doubled)
    uint16_t reg;        // reg_address_out (raw 8-bit field; *2 = real BPLxDAT addr)
};

struct LineResult {
    int count = 0;
    std::vector<Fetch> fetches;
};

// Runs vpos 0..target_vpos inclusive, hpos 0..hpos_max-1 each line, and
// records every dma pulse seen while vpos==target_vpos, in time order.
// Reused verbatim from the sibling ch3-ddf-fetch-window/ch3-register-map-
// agnus packages' own proven-good drive pattern on this same fixture.
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

// Sweeps vpos 0..through_vpos WITHOUT recording anything - primes vdiwena
// and the pipelined delay chains before a fresh pointer write is trusted to
// be "the first fetch this plane makes." Bitplane pointers auto-increment
// on EVERY dma cycle from the moment vdiwena goes active, so a pointer
// written once before a long sweep is no longer at its written value by the
// time the target line is reached - this is why every assertion below
// writes its pointer(s) AFTER priming, immediately before the line it
// measures.
void prime_to_line(Harness& h, int through_vpos, int hpos_max = 456) {
    for (int v = 0; v <= through_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
        }
    }
}

// Runs exactly one more line at h.dut->vpos = this_vpos and records every
// dma pulse, in order. Caller must already have primed through this_vpos-1.
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

// Returns the address of the FIRST fetch tagged with the given
// reg_address_out slot within a LineResult, or 0 if that plane made no
// fetch this line (0 is never a valid fetched address in this package's own
// runs, all of which use a nonzero base pointer).
uint32_t first_fetch_addr_for_plane(const LineResult& r, uint16_t reg) {
    for (auto& f : r.fetches)
        if (f.reg == reg) return f.address;
    return 0;
}

// Runs `n` consecutive lines starting at `from_vpos` (caller has already
// primed through from_vpos-1) and returns plane `reg`'s first-fetch address
// on each line, in order - the direct, multi-line measurement of the
// line-to-line step this whole package is about.
std::vector<uint32_t> line_starts(Harness& h, int from_vpos, int n, uint16_t reg,
                                   int hpos_max = 456) {
    std::vector<uint32_t> starts;
    for (int i = 0; i < n; ++i) {
        LineResult r = run_single_line(h, from_vpos + i, hpos_max);
        starts.push_back(first_fetch_addr_for_plane(r, reg));
    }
    return starts;
}

// Byte deltas can wrap around the RTL's 21-bit unsigned chip address space
// at the extreme moduli assertion 15 drives; compare via the low 21 bits so
// an intentional wrap still matches the expected delta exactly, rather than
// masking the arithmetic with a wider/narrower comparison type.
uint32_t delta21(uint32_t from, uint32_t to) { return (to - from) & 0x1FFFFFu; }

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

    // ---------------- FIXTURE-DELIVERS-KNOWN-POINTERS-IN-ORDER [self-proof, no page] ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 4));  // lores, BPU=4

        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase[4] = {0x001000, 0x003000, 0x005000, 0x007000};
        for (int p = 0; p < 4; ++p) h.write_ptr(p, kBase[p]);

        LineResult r = run_single_line(h, 0x50);
        std::map<uint16_t, std::vector<uint32_t>> by_plane;
        for (auto& f : r.fetches) by_plane[f.reg].push_back(f.address);

        bool all_present = by_plane.size() == 4 && by_plane.count(0x88) &&
            by_plane.count(0x89) && by_plane.count(0x8A) && by_plane.count(0x8B);
        bool starts_match = all_present &&
            by_plane[0x88].front() == kBase[0] && by_plane[0x89].front() == kBase[1] &&
            by_plane[0x8A].front() == kBase[2] && by_plane[0x8B].front() == kBase[3];
        bool increments = true;
        int checked = 0;
        if (all_present) {
            for (auto& [reg, addrs] : by_plane)
                for (size_t i = 1; i < addrs.size(); ++i) {
                    if (addrs[i] != addrs[i - 1] + 2) increments = false;
                    ++checked;
                }
        }
        checks.expect(
            all_present && starts_match && increments && checked > 0,
            "FIXTURE-DELIVERS-KNOWN-POINTERS-IN-ORDER: four DISTINCT known "
            "pointers (0x001000/0x003000/0x005000/0x007000, one per plane) "
            "each arrive at their OWN reg_address_out slot (0x88/0x89/0x8A/"
            "0x8B), each one's FIRST fetched address equals exactly the "
            "pointer programmed for that plane, and each plane's own "
            "successive fetches increment by exactly 2 bytes, IN ORDER - "
            "this fixture's write-then-fetch path is not silently dead "
            "before any arithmetic assertion below is trusted to mean "
            "anything. 'Chip memory pointer that addresses DMA data' "
            "(p.288), 'This pair of registers contains the...pointer to the "
            "address of bitplane x...DMA data' (p.272). B-pointer.c1, "
            "A-bplptr.c8, A-bplxpt.c02.");
    }

    // ---------------- POINTER-PAIR-IS-ONE-ADDRESS (p.54, 63) ----------------
    {
        constexpr uint32_t kAddr = 0x00123456;
        uint16_t pth = static_cast<uint16_t>((kAddr >> 16) & 0x1F);
        uint16_t ptl = static_cast<uint16_t>(kAddr & 0xFFFF);
        uint16_t pth_addr = BPLPT_BASE;
        uint16_t ptl_addr = static_cast<uint16_t>(BPLPT_BASE + 2);

        Harness ha;
        ha.setup_standard();
        ha.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        ha.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        ha.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        prime_to_line(ha, 0x4F);
        ha.write_reg(pth_addr, pth);  // PTH first
        ha.write_reg(ptl_addr, ptl);  // then PTL
        uint32_t addr_pth_first = first_fetch_addr_for_plane(run_single_line(ha, 0x50), 0x88);

        Harness hb;
        hb.setup_standard();
        hb.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        hb.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        hb.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        prime_to_line(hb, 0x4F);
        hb.write_reg(ptl_addr, ptl);  // PTL first
        hb.write_reg(pth_addr, pth);  // then PTH
        uint32_t addr_ptl_first = first_fetch_addr_for_plane(run_single_line(hb, 0x50), 0x88);

        checks.expect(
            addr_pth_first == kAddr && addr_ptl_first == kAddr,
            "POINTER-PAIR-IS-ONE-ADDRESS: BPL1PTH and BPL1PTL, written "
            "independently in EITHER order, compose into exactly the same "
            "single fetched address (0x" + std::to_string(kAddr) +
            "), PTH-then-PTL giving 0x" + std::to_string(addr_pth_first) +
            " and PTL-then-PTH giving 0x" + std::to_string(addr_ptl_first) +
            " - '68000 programmers may treat these as one 32-bit address "
            "and write to them as one long word' (p.54), 'BPLxPTH - pointer "
            "to bitplane starting position in memory (written as a long "
            "word)' (p.63 summary). 3-memory.c28, 3-memory.c29, 3-ptr.c05, "
            "3-ptr.c06, 3-sum.c02.");
    }

    // ---------------- SIX-PLANE-TWELVE-REGISTER-BANK (p.42, 54) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 6));  // lores, BPU=6

        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase6[6] = {0x001000, 0x003000, 0x005000,
                                         0x007000, 0x009000, 0x00B000};
        for (int p = 0; p < 6; ++p) h.write_ptr(p, kBase6[p]);
        LineResult r = run_single_line(h, 0x50);

        std::map<uint16_t, uint32_t> first;
        for (auto& f : r.fetches) if (!first.count(f.reg)) first[f.reg] = f.address;

        bool six_present = true;
        for (int p = 0; p < 6; ++p) {
            uint16_t reg = static_cast<uint16_t>(0x88 + p);
            if (!first.count(reg) || first[reg] != kBase6[p]) six_present = false;
        }
        bool only_six = !first.count(0x8E) && !first.count(0x8F);
        checks.expect(
            six_present && only_six,
            "SIX-PLANE-TWELVE-REGISTER-BANK: with BPU=6, all six bitplane "
            "pointer pairs (BPL1..BPL6) hold independent starting addresses "
            "simultaneously, each arriving at its own reg_address_out slot "
            "(0x88..0x8D), and no fetch is tagged for a seventh/eighth "
            "plane (0x8E/0x8F) - 'Access to bitplanes in memory is provided "
            "by two address registers, BPLxPTH and BPLxPTL, for each "
            "bitplane (12 registers in all)' (p.54). 'For a four-color "
            "display, you build two bitplanes in memory' (p.42, the "
            "minimal multi-plane instance); Appendix B's own subject index "
            "restates each of the six 'Bitplane N pointer' rows as 'one "
            "half of an address-register pair' (p.291). 3-color.c5, "
            "3-memory.c25, 3-ptr.c02, B-subject.c30, B-subject.c37, "
            "B-subject.c44, B-subject.c51, B-subject.c58, B-subject.c65, "
            "B-subject.c72, B-subject.c79, B-subject.c86, B-subject.c93, "
            "B-subject.c100, B-subject.c107.");
    }

    // ---------------- POINTER-ALWAYS-EVEN (p.61, 287) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        prime_to_line(h, 0x4F);
        h.write_reg(BPLPT_BASE, 0x0000);                          // PTH = 0
        h.write_reg(static_cast<uint16_t>(BPLPT_BASE + 2), 0x2469);  // PTL, ODD raw value

        uint32_t addr = first_fetch_addr_for_plane(run_single_line(h, 0x50), 0x88);
        checks.expect(
            addr == 0x2468 && (addr % 2) == 0,
            "POINTER-ALWAYS-EVEN: writing the ODD raw value 0x2469 to "
            "BPL1PTL produces a fetched address of 0x" + std::to_string(addr) +
            " (bit 0 is never read/stored) - 'Note that the pointers always "
            "contain an even number, because data is fetched from the "
            "display a word at a time' (p.61), '+ Address register pair. "
            "Must be an even address pointing to chip memory' (p.287); "
            "Appendix B's subject index restates the same 'must hold an "
            "even address' fact once per plane, for all six (p.291). "
            "3-modulo.c14, 3-ptr.c12, B-legend.c3, B-legend.c4, "
            "B-subject.c31, B-subject.c38, B-subject.c45, B-subject.c52, "
            "B-subject.c59, B-subject.c66, B-subject.c73, B-subject.c80, "
            "B-subject.c87, B-subject.c94, B-subject.c101, B-subject.c108.");
    }

    // ---------------- POINTER-NOT-SELF-RESETTING (p.63, 272, 288) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase = 0x004000;
        h.write_ptr(0, kBase);

        std::vector<uint32_t> starts = line_starts(h, 0x50, 5, 0x88);
        bool advances = starts.size() == 5 &&
            starts[0] == kBase && starts[1] == kBase + 40 &&
            starts[2] == kBase + 80 && starts[3] == kBase + 120 &&
            starts[4] == kBase + 160;  // pure forward advance, no write in between

        // An explicit rewrite (the software "reset for redisplay") DOES take
        // hold on the very next line, confirming that when the manual says
        // "you have to reset the bitplane pointers," an ordinary register
        // write is exactly what is required and exactly what works.
        h.write_ptr(0, kBase);
        uint32_t after_rewrite = first_fetch_addr_for_plane(run_single_line(h, 0x55), 0x88);

        checks.expect(
            advances && after_rewrite == kBase,
            "POINTER-NOT-SELF-RESETTING: across five consecutive lines with "
            "NO register write in between, the pointer strictly advances by "
            "the plain 40-byte step every time (0x004000, +40, +80, +120, "
            "+160) - it never reverts toward its original value on its own, "
            "the RTL-measurable shape of 'must be reinitialized by the "
            "processor or copper' (p.272) and 'must be reloaded by a "
            "processor before use' (p.288); an explicit rewrite to the SAME "
            "base value afterward DOES take effect on the next line ('you "
            "have to reset the bitplane pointers' each redisplay, p.63). "
            "A-bplptr.c9, B-pointer.c2, 3-display.c2, 3-display.c3, "
            "3-display.c4, 3-redis.c01.");
    }

    // ---------------- POINTER-WIDTH-DISPUTED [measurement only, no PASS/FAIL] (p.54 background, 272, 291) ----------------
    checks.note(
        "POINTER-WIDTH-DISPUTED: agnus_bitplanedma.v declares bplpth as "
        "`reg [20:16] bplpth [7:0]` (a FIVE-bit high field, l.82) and bplptl "
        "as `reg [15:1] bplptl [7:0]` (a FIFTEEN-bit low field, l.83), "
        "composing into a 20-bit-significant address_out[20:1] (l.46, "
        "confirmed directly by POINTER-PAIR-IS-ONE-ADDRESS above). The "
        "printed manual disagrees with itself: Appendix A p.272 and "
        "Appendix B p.291 both say 'Bitplane x pointer (high 3 bits)' / "
        "'(low 15 bits)', an 18-bit total ('This pair of registers contains "
        "the 18-bit pointer...', A-bplptr.c8); chapter text elsewhere is "
        "read as a 19-bit address. This RTL implements neither figure - not "
        "3 bits, not 19-bit total, not 18-bit total, but 5+15=20 "
        "significant bits (an AGA-era extension beyond the OCS/ECS 1 MB "
        "addressing the printed appendices describe). No PASS/FAIL is "
        "attached to any of these widths; only the undisputed sub-claims "
        "(one pointer, always even, addresses chip memory) are asserted, "
        "in POINTER-PAIR-IS-ONE-ADDRESS and POINTER-ALWAYS-EVEN above. "
        "A-bplptr.c3, A-bplptr.c6, A-bplxpth.fn, A-bplxptl.fn, "
        "B-subject.c29, B-subject.c36, B-subject.c43, B-subject.c50, "
        "B-subject.c57, B-subject.c64, B-subject.c71, B-subject.c78, "
        "B-subject.c85, B-subject.c92, B-subject.c99, B-subject.c106.");

    // ---------------- MODULO-ZERO-CONTIGUOUS-LINES (p.53, 60, 61, 64) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        h.write_reg(BPL1MOD_ADDR, 0x0000);
        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase = 0x008000;
        h.write_ptr(0, kBase);

        LineResult line1 = run_single_line(h, 0x50);
        LineResult line2 = run_single_line(h, 0x51);
        bool line1_contiguous = line1.count == 20 &&
            line1.fetches.front().address == kBase &&
            line1.fetches.back().address == kBase + 38;
        for (size_t i = 1; i < line1.fetches.size() && line1_contiguous; ++i)
            if (line1.fetches[i].address != line1.fetches[i - 1].address + 2)
                line1_contiguous = false;
        uint32_t line2_start = first_fetch_addr_for_plane(line2, 0x88);

        checks.expect(
            line1_contiguous && line2_start == kBase + 40,
            "MODULO-ZERO-CONTIGUOUS-LINES: modulo=0, a 40-byte line, "
            "re-verifies the pre-supplied measurement as this package's own "
            "control - line 1 fetched START..START+38 in a gap-free 2-byte "
            "step (20 words, 'the memory for each bitplane must be "
            "continuous', p.53), line 2 begins exactly at START+40 - "
            "'Modulo refers to the number of bytes in memory between the "
            "last word...and the...first word on the next line' (p.60), "
            "matching Figure 3-10/3-11's own worked numbers and the chapter "
            "summary's 'Set modulo to 0 for non-interlaced' (p.64). "
            "3-mem.c10, 3-mod.c01, 3-mod.c02, 3-modulo.c1, 3-mod.c03, "
            "3-modulo.c2, 3-modulo.c3, 3-mod.c05, 3-modulo.c5, 3-modulo.c6, 3-modulo.c7, "
            "3-mod.f01, 3-mod.f04, 3-modulo.c10, 3-modulo.c11, "
            "3-modulo.c12, 3-ptr.c07, 3-ptr.c08, 3-ptr.c11, 3-sum.c18, "
            "3-summary.c5, A-bplmod.c9, A-bplmod.c10, A-mod.c01, B-mod.c1, "
            "B-mod.c2, B-mod.c3 ('This allows the...display window to "
            "operate on...a window of data that is smaller than the "
            "actual picture in memory' (p.288) - the display-window half "
            "only; this module has no blitter port).");
    }

    // ---------------- MODULO-NONZERO-FOR-WIDER-PLAYFIELD (p.73, 74) ----------------
    uint32_t g15_step_ab = 0;  // shared with MODULO-INDEPENDENT-OF-BASE-VALUE below
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        h.write_reg(BPL1MOD_ADDR, 40);
        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase = 0x008000;
        h.write_ptr(0, kBase);

        LineResult line1 = run_single_line(h, 0x50);
        LineResult line2 = run_single_line(h, 0x51);
        bool line1_unchanged = line1.count == 20 &&
            line1.fetches.front().address == kBase &&
            line1.fetches.back().address == kBase + 38;
        uint32_t line2_start = first_fetch_addr_for_plane(line2, 0x88);
        g15_step_ab = delta21(kBase, line2_start);

        checks.expect(
            line1_unchanged && line2_start == kBase + 80,
            "MODULO-NONZERO-FOR-WIDER-PLAYFIELD: modulo=40 (a memory "
            "picture wider than the display window) reproduces Figure "
            "3-15/3-16's own worked numbers - line 1 is UNCHANGED at "
            "START..START+38 (modulo affects only the NEXT line, not the "
            "current one's own fetch), pointer afterward is START+40, "
            "+modulo(40) puts line 2 at exactly START+80 - 'If the big "
            "picture in memory is larger than the display window, you must "
            "respecify the modulos. The modulo must be some value other "
            "than 0' (p.73), 'the modulo, which is 40, is added to the "
            "current value of the pointer' (p.74). Also the concrete, "
            "hardware-measurable shape of 'use a modulo of 40 to skip the "
            "lines in the other field' (interlace, p.62) and the summary's "
            "'40 for interlaced' (p.64) - an 80-byte step is exactly double "
            "the ordinary 40-byte one. 3-big.c01, 3-big.c02, 3-large.c1, "
            "3-large.c2, 3-big.c06, 3-big.c07, 3-big.f03, 3-large.c7, "
            "3-large.c9, 3-large.c10, 3-large.c11, 3-mod.c09, 3-mod.c16, "
            "3-mod.c17, 3-mod.c18, 3-modulo.c28, 3-modulo.c29, "
            "3-modulo.c30, 3-sum.c19, 3-summary.c6, 3-big.c19, 3-big.c20, "
            "3-large.c24, 3-large.c25, A-ddf.c8.");
    }

    // ---------------- MODULO-INDEPENDENT-OF-BASE-VALUE (p.75) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        h.write_reg(BPL1MOD_ADDR, 40);
        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase2 = 0x008028;  // START+40, "right half" selection
        h.write_ptr(0, kBase2);

        LineResult line1 = run_single_line(h, 0x50);
        LineResult line2 = run_single_line(h, 0x51);
        uint32_t line2_start = first_fetch_addr_for_plane(line2, 0x88);
        uint32_t step_here = delta21(kBase2, line2_start);

        checks.expect(
            line1.fetches.front().address == kBase2 && step_here == 80 &&
            step_here == g15_step_ab,
            "MODULO-INDEPENDENT-OF-BASE-VALUE: the SAME modulo(40) applied "
            "from a DIFFERENT starting pointer (START+40 instead of START, "
            "selecting a different horizontal slice of the memory picture) "
            "produces the IDENTICAL 80-byte line-to-line step as "
            "MODULO-NONZERO-FOR-WIDER-PLAYFIELD's own START-based "
            "measurement - the mechanism does not care what value it is "
            "added to. 'To display the right half of the big picture, you "
            "set up a vertical blanking routine to start the bitplane "
            "pointers at location START+40 rather than START with the "
            "modulo remaining at 40' (p.75). 3-big.c08, 3-big.c09, "
            "3-big.c10, 3-large.c12, 3-large.c13, 3-large.c16.");
    }

    // ---------------- BPL1MOD-ODD-BPL2MOD-EVEN-INDEPENDENT (p.62, 91, 270, 291) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 2));  // lores, BPU=2
        h.write_reg(BPL1MOD_ADDR, 40);
        h.write_reg(BPL2MOD_ADDR, 80);
        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase0 = 0x00010000, kBase1 = 0x00020000;
        h.write_ptr(0, kBase0);  // BPL1, odd-numbered -> BPL1MOD
        h.write_ptr(1, kBase1);  // BPL2, even-numbered -> BPL2MOD

        LineResult line1 = run_single_line(h, 0x50);
        LineResult line2 = run_single_line(h, 0x51);
        uint32_t step0 = delta21(kBase0, first_fetch_addr_for_plane(line2, 0x88));
        uint32_t step1 = delta21(kBase1, first_fetch_addr_for_plane(line2, 0x89));

        checks.expect(
            first_fetch_addr_for_plane(line1, 0x88) == kBase0 &&
            first_fetch_addr_for_plane(line1, 0x89) == kBase1 &&
            step0 == 80 && step1 == 120,
            "BPL1MOD-ODD-BPL2MOD-EVEN-INDEPENDENT: with BPL1MOD=40 and "
            "BPL2MOD=80 held simultaneously (BPU=2, BPL1+BPL2 active), "
            "BPL1's own line-to-line step is 80 bytes (40 line + its own "
            "40 modulo) while BPL2's is 120 bytes (40 line + its own 80 "
            "modulo) - two DIFFERENT values confirm each register drives "
            "only its own named plane group, not the other's - 'There are "
            "two modulo registers -- BPL1MOD for the odd-numbered "
            "bitplanes and BPL2MOD for the even-numbered bitplanes. This "
            "allows for differing modulos for each playfield in "
            "dual-playfield mode' (p.62). 3-mod.c06, 3-mod.c07, 3-mod.c08, "
            "3-modulo.c16, 3-modulo.c17, 3-modulo.c18, 3-regsum-mod.c1, "
            "3-regsum-mod.c2, 3-rmod.c01, 3-rmod.c02, A-bpl1mod.fn, "
            "A-bpl2mod.fn, A-bplmod.c4, A-bplmod.c8, A-bplmod.c11, "
            "A-bplmod.c12, A-mod.c02, B-subject.c129, B-subject.c133.");
    }

    // ---------------- HSCROLL-MODULO-COMPENSATION-FORMULA (p.82, 84) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, 0x0030);  // one word (8 CCK) earlier - scrolled
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);  // unchanged
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        h.write_reg(BPL1MOD_ADDR, 38);  // "two counts less than the...difference"
        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase = 0x00030000;
        h.write_ptr(0, kBase);

        LineResult line1 = run_single_line(h, 0x50);
        LineResult line2 = run_single_line(h, 0x51);
        uint32_t line2_start = first_fetch_addr_for_plane(line2, 0x88);

        checks.expect(
            line1.count == 21 && first_fetch_addr_for_plane(line1, 0x88) == kBase &&
            delta21(kBase, line2_start) == 80,
            "HSCROLL-MODULO-COMPENSATION-FORMULA: the scrolled fetch window "
            "(DDFSTRT=$0030, one word earlier) makes line 1 fetch 21 words "
            "(42 bytes) starting at the SAME base pointer; with modulo=38 "
            "('two counts less than the difference') line 2 begins exactly "
            "at START+80 - the SAME line-to-line step MODULO-NONZERO-FOR- "
            "WIDER-PLAYFIELD measured for an entirely unscrolled 20-word/"
            "modulo-40 line, confirming the compensation formula "
            "arithmetically rather than trusting the one printed instance. "
            "'As always, the modulo is two counts less than the difference "
            "between the address of the next word you want to fetch and "
            "the address of the last word that was fetched' (p.82), 'At "
            "this point, the bitplane pointers contain the value START+42. "
            "Adding the modulo of 38 gives the correct starting point for "
            "the next line' (p.84). 3-hscr.c13, 3-hscroll.c15, 3-hscr.c18, "
            "3-hscr.c19, 3-hscroll.c24, 3-hscroll.c25.");
    }

    // ---------------- DUAL-PLAYFIELD-SCROLL-POINTER-MODULO-OFFSET (p.72) ----------------
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, 0x0030);       // shared, scroll-shifted DDF window
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 2));  // BPU=2: one plane per playfield
        h.write_reg(BPL1MOD_ADDR, 0x0000);       // BPL1 (scrolling playfield): no compensation
        h.write_reg(BPL2MOD_ADDR, 0xFFFE);       // BPL2 (stationary playfield): -2 bytes
        prime_to_line(h, 0x4F);
        constexpr uint32_t kBase0 = 0x00040000, kBase1 = 0x00050000;
        h.write_ptr(0, kBase0);
        h.write_ptr(1, kBase1);

        run_single_line(h, 0x50);
        LineResult line2 = run_single_line(h, 0x51);
        uint32_t step_scrolling  = delta21(kBase0, first_fetch_addr_for_plane(line2, 0x88));
        uint32_t step_stationary = delta21(kBase1, first_fetch_addr_for_plane(line2, 0x89));

        checks.expect(
            step_scrolling == 42 && step_stationary == 40,
            "DUAL-PLAYFIELD-SCROLL-POINTER-MODULO-OFFSET: with the shared, "
            "scroll-shifted DDF window applied to BOTH planes (BPU=2), the "
            "SCROLLING playfield's own plane (BPL1MOD=0) naturally grows to "
            "a 42-byte step (the extra scroll word), while the STATIONARY "
            "playfield's plane (BPL2MOD=-2, written as $FFFE) is pulled "
            "back to exactly the ordinary 40-byte step - offsetting its "
            "pointer and modulo by -2 exactly cancels the shared window's "
            "extra word. 'In low resolution mode, you adjust the pointers "
            "by -2 and the modulo by -2' (p.72), '...adjust the modulo and "
            "the bitplane pointers of the playfield that is not being "
            "scrolled to maintain its position on the display' (p.72), "
            "'Write the modulo to both BPL1MOD and BPL2MOD' (p.72). "
            "3-dual-scroll.c5, 3-dual-scroll.c7, 3-dual.c38, 3-dual.c51.");
    }

    // ---------------- VERTICAL-SCROLL-POINTER-STEP (p.80, 81, 85) ----------------
    {
        constexpr uint32_t kBase = 0x00060000;
        std::vector<uint32_t> firsts;
        for (int k = 0; k < 4; ++k) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
            h.write_reg(BPL1MOD_ADDR, 0x0000);
            prime_to_line(h, 0x4F);
            h.write_ptr(0, kBase + 40u * static_cast<uint32_t>(k));  // reset each "field"
            firsts.push_back(first_fetch_addr_for_plane(run_single_line(h, 0x50), 0x88));
        }
        checks.expect(
            firsts.size() == 4 && firsts[0] == kBase && firsts[1] == kBase + 40 &&
            firsts[2] == kBase + 80 && firsts[3] == kBase + 120,
            "VERTICAL-SCROLL-POINTER-STEP: writing a progressively higher "
            "STARTING pointer value before each of four successive "
            "redisplays (0x60000, +40, +80, +120 - one line's worth per "
            "step, matching 'the pointers would be incremented by 40 bytes "
            "each time' for a normal-sized low-resolution display with "
            "modulo 0, p.81) moves the FIRST fetched address of that "
            "redisplay by exactly the same amount each time - 'All you "
            "need do for vertical scrolling is progressively increase or "
            "decrease the starting address for the bitplane pointers by "
            "the size of a horizontal line' (p.80); a numerically higher "
            "start address is later in the same linear memory picture, "
            "which is what 'As the value of the pointer increases, more of "
            "the lower part of the picture is shown' (p.81) names; 'Reset "
            "BPLxPTH and BPLxPTL during the vertical blanking interval' "
            "(p.85) is exactly the per-redisplay rewrite this assertion "
            "performs. 3-scr.c01, 3-scr.c02, 3-vscr.c01, 3-vscr.c09, "
            "3-vscr.c10, 3-vscr.f01, 3-vscr.f02, 3-vscroll.c1, "
            "3-vscroll.c2, 3-vscr.c11.");
    }

    // ---------------- MODULO-WIDTH-262144-PIXEL-LIMIT (p.76) ----------------
    {
        // Manual-derived conversion only (never an RTL constant): 262,144
        // pixels / 8 bits-per-byte (p.53's own "320 bits divided by 8 bits"
        // convention) = 32,768 bytes.
        constexpr uint32_t kManualWidthLimitBytes = 262144 / 8;

        // Line width (20 words = 40 bytes, the same standard lores line
        // every other assertion above uses) is ALWAYS added on the same
        // end-of-line cycle as the modulo (see IMPLEMENTATION NOTES,
        // l.483-493: the ordinary word-advance and the modulo add are one
        // arithmetic expression, not two separate steps) - so the modulo's
        // OWN contribution is the measured total line-to-line delta MINUS
        // that fixed 40-byte line width, never the raw total by itself.
        constexpr uint32_t kLineWidthBytes = 40;

        Harness hp;
        hp.setup_standard();
        hp.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        hp.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        hp.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        hp.write_reg(BPL1MOD_ADDR, 0x7FFE);  // largest positive field value
        prime_to_line(hp, 0x4F);
        constexpr uint32_t kBaseP = 0x00003000;
        hp.write_ptr(0, kBaseP);
        std::vector<uint32_t> startsP;
        std::vector<uint32_t> countsP;
        for (int i = 0; i < 5; ++i) {
            LineResult r = run_single_line(hp, 0x50 + i);
            countsP.push_back(static_cast<uint32_t>(r.count));
            startsP.push_back(first_fetch_addr_for_plane(r, 0x88));
        }
        uint32_t total_deltaP = delta21(startsP[0], startsP[1]);
        uint32_t mod_contribP = delta21(kLineWidthBytes, total_deltaP);  // total - 40

        Harness hn;
        hn.setup_standard();
        hn.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        hn.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        hn.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        hn.write_reg(BPL1MOD_ADDR, 0x8000);  // most-negative field value
        prime_to_line(hn, 0x4F);
        constexpr uint32_t kBaseN = 0x00100000;
        hn.write_ptr(0, kBaseN);
        uint32_t startN0 = first_fetch_addr_for_plane(run_single_line(hn, 0x50), 0x88);
        uint32_t startN1 = first_fetch_addr_for_plane(run_single_line(hn, 0x51), 0x88);
        uint32_t total_deltaN = delta21(startN0, startN1);
        uint32_t mod_contribN = delta21(kLineWidthBytes, total_deltaN);  // total - 40

        bool heights_ok = countsP.size() == 5;
        for (uint32_t c : countsP) if (c != 20) heights_ok = false;

        int32_t mod_contribN_signed =
            mod_contribN >= (1u << 20) ? static_cast<int32_t>(mod_contribN) - (1 << 21)
                                        : static_cast<int32_t>(mod_contribN);

        checks.expect(
            mod_contribP == kManualWidthLimitBytes - 2 &&
            mod_contribN_signed == -static_cast<int32_t>(kManualWidthLimitBytes) &&
            heights_ok,
            "MODULO-WIDTH-262144-PIXEL-LIMIT: isolating the modulo's OWN "
            "contribution from the fixed 40-byte line width (total "
            "line-to-line delta minus 40), the largest positive modulo "
            "field value ($7FFE) contributes +" + std::to_string(mod_contribP) +
            " bytes and the most-negative ($8000) contributes " +
            std::to_string(mod_contribN_signed) +
            " bytes - the positive extreme lands 2 bytes short of the "
            "manual's own 262,144-pixel figure divided by 8 bits/byte = "
            "32,768 bytes (the same 'always even' rounding "
            "POINTER-ALWAYS-EVEN measures), and the negative extreme lands "
            "on exactly -32,768; five further lines under the same extreme "
            "modulo still fetch the full 20 words EVERY line - the modulo "
            "constrains how far an address can move (WIDTH), never how "
            "many more lines can be fetched (HEIGHT). 'Another constraint "
            "on playfield size is the bit plane modulos which limit the "
            "width (but not the height) of a playfield to 262,144 pixels' "
            "(p.76). 3-big.c19, 3-big.c20, 3-large.c24, 3-large.c25.");
    }

    // ---------------- BPLEN-DMA-GATES-POINTER-ADVANCE (p.62) ----------------
    {
        Harness h_off;
        h_off.setup_standard();
        h_off.dut->dmaena = 0;  // BPLEN never set - DMA held off
        h_off.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h_off.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h_off.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        prime_to_line(h_off, 0x4F);
        h_off.write_ptr(0, 0x00070000);
        LineResult off = run_single_line(h_off, 0x50);

        Harness h_on;
        h_on.setup_standard();  // dmaena=1
        h_on.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h_on.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h_on.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));
        prime_to_line(h_on, 0x4F);
        h_on.write_ptr(0, 0x00070000);
        LineResult on = run_single_line(h_on, 0x50);

        checks.expect(
            off.count == 0 && on.count == 20,
            "BPLEN-DMA-GATES-POINTER-ADVANCE: with an IDENTICAL, fully "
            "programmed pointer and fetch window, holding dmaena (the "
            "level DMACON's BPLEN bit ultimately drives into this module) "
            "low produces ZERO fetches; the same setup with dmaena raised "
            "produces the expected 20 - 'You start playfield display by "
            "making certain that the bitplane pointers are set and "
            "bitplane DMA is turned on' (p.62): both conditions "
            "independently necessary, neither alone sufficient. 3-dma.c01.");
    }

    // ---------------- NOT ASSERTED (software/CPU technique, or a different module's boundary) ----------------
    checks.note(
        "NOT ASSERTED: 3-copper-redisplay.c1 (p.62, 'You can use the "
        "Copper to handle resetting of the bitplane pointers for "
        "interlaced displays') - the Copper performs an ordinary register "
        "write; nothing on this module's ports distinguishes a Copper "
        "write from a CPU write, the same reasoning the sibling "
        "ch3-ddf-fetch-window package already applied to its own "
        "Copper-usage claim. A-bplxdat.c06 (p.272, '...triggered whenever "
        "bitplane #1 is written, indicating the completion of all "
        "bitplanes for that word...') - describes the BPLxDAT data "
        "register's own write-completion signalling; this module has no "
        "data-value port at all (confirmed by the sibling packages reading "
        "the same port list), so nothing here can assert or refute it.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    // A red result IS a valid, finished outcome -- that principle forbids
    // changing the assertion until it passes. It does not license hiding the
    // red from the suite. run_all.sh classifies a package solely by its exit
    // status, so returning 0 here launders any future red into a suite PASS.
    return checks.failed ? 1 : 0;
}

// ============================================================================
// MUTATION LOG (11.09.2026). Every entry below was actually applied by this
// agent, rebuilt and run under WSL/Verilator (`make`), confirmed red, then
// reverted and rebuilt clean (14/14 green again) - not predicted. Each
// mutation changes what the harness ASKS the module to do (the stimulus)
// while leaving every assertion's own expected-value logic untouched, per
// the build brief's rule that flipping an expected value proves nothing.
// Baseline result before and after every mutation below: 14 passed, 0
// failed. No assertion in this package is red at baseline, so no "none
// impossible" entries are needed.
//
// MUTATION FIXTURE-DELIVERS-KNOWN-POINTERS-IN-ORDER stimulus red: pointer write loop shortened from 4 planes to 3; plane 3 (reg 0x8B) never received its pointer, starts_match failed.
// MUTATION POINTER-PAIR-IS-ONE-ADDRESS stimulus red: the PTL-then-PTH run's PTH value changed from pth to pth+1; PTL-then-PTH result (0x1258582) no longer equalled PTH-then-PTL's (0x1193046).
// MUTATION SIX-PLANE-TWELVE-REGISTER-BANK stimulus red: BPU changed from 6 to 5; plane 6 (BPL6, reg 0x8D) never fetched, six_present failed.
// MUTATION POINTER-ALWAYS-EVEN stimulus red: PTL raw value changed from 0x2469 to 0x246B; fetched address became 0x9322, not the expected 0x2468.
// MUTATION POINTER-NOT-SELF-RESETTING stimulus red: BPL1MOD written to 40 (nonzero) before the 5-line sweep; the per-line step became 80 bytes, not the expected plain 40.
// MUTATION MODULO-ZERO-CONTIGUOUS-LINES stimulus red: DDFSTOP changed from $00D0 to $00C0; word count and line-2 start no longer matched the modulo-0 worked numbers.
// MUTATION MODULO-NONZERO-FOR-WIDER-PLAYFIELD stimulus red: BPL1MOD changed from 40 to 80; line 2 started at START+120, not the expected START+80.
// MUTATION MODULO-INDEPENDENT-OF-BASE-VALUE stimulus red: BPL1MOD changed from 40 to 20; the START+40-based step became 60 bytes, no longer matching MODULO-NONZERO-FOR-WIDER-PLAYFIELD's own 80-byte step.
// MUTATION BPL1MOD-ODD-BPL2MOD-EVEN-INDEPENDENT stimulus red: BPL2MOD changed from 80 to 40 (same as BPL1MOD); BPL2's step became 80 bytes, indistinguishable from BPL1's, not the expected 120.
// MUTATION HSCROLL-MODULO-COMPENSATION-FORMULA stimulus red: DDFSTRT changed from scrolled $0030 to standard $0038; line 1 fetched 20 words, not the expected 21.
// MUTATION DUAL-PLAYFIELD-SCROLL-POINTER-MODULO-OFFSET stimulus red: BPL2MOD (the stationary playfield's -2 compensation) changed from $FFFE to $0000; its step became 42 bytes, not the expected 40.
// MUTATION VERTICAL-SCROLL-POINTER-STEP stimulus red: the per-redisplay pointer step changed from 40 bytes to 20; successive starts no longer matched the expected 0x60000/+40/+80/+120 sequence.
// MUTATION MODULO-WIDTH-262144-PIXEL-LIMIT stimulus red: the positive-extreme modulo changed from $7FFE to $3FFE (half); the isolated modulo contribution became +16382 bytes, not the expected +32766.
// MUTATION BPLEN-DMA-GATES-POINTER-ADVANCE stimulus red: the "off" harness's dmaena changed from 0 to 1; it produced 20 fetches, not the expected 0.
// ============================================================================

