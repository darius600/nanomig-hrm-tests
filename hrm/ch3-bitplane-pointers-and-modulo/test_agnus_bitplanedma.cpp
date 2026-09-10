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
//   pages 42, 52, 53, 54, 60, 61, 62, 63, 64, 72, 73, 74, 75, 76, 80, 81, 82,
//   84, 85, 91, 270, 272, 277, 287, 288, 291.
// Non-goals: which 9-bit bus address selects which register (that is
//   ch3-register-map-agnus's own address-decode-selectivity boundary, not
//   repeated here); DDFSTRT/DDFSTOP fetch-window word counts and the
//   4-pixel/16-pixel granularity story (ch3-ddf-fetch-window's boundary);
//   BPLxDAT / bit-to-colour resolution (Denise's palette, not this DMA
//   module); the DMACON register's own bit decode (agnus_top's boundary -
//   this module only consumes the resulting `dmaena` level, which IS in
//   scope for claim 3-dma.c01 below).
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
// C:\Users\x\AppData\Local\Temp\claude\d--code\674b5d99-c2c3-47f0-95e4-93eefcde2be3\scratchpad\build\ch3-bitplane-pointers-and-modulo\claims.jsonl,
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
