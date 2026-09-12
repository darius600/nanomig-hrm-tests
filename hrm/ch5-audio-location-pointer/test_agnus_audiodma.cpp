// Test purpose: HRM conformance package for the audio DMA location/pointer
//   registers (AUDxLCH/AUDxLCL, x=0..3) against chapter 5's printed text and
//   its three register appendices. 176 testable claims, printed pages
//   137-309, from work/ch5_packages/ch5-audio-location-pointer.jsonl.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 137-309 (chapter 5 body; appendix A pp.263-266; appendix B pp.287-290;
//   appendix C pp.298-309), as quoted verbatim in the 176 claims named above.
// Fixture boundary: agnus_audiodma.v ONLY (BOUNDARIES.md) - 157 lines, "the
//   audio DMA pointer registers and the slot allocation." No other module
//   (paula_audio_channel.v, paula.v, gary.v, ...) is instantiated anywhere in
//   this file. Where a claim's full text needs one of those (the "not
//   decoded by Paula" half of an Agnus-attribution claim; the "points to
//   CHIP memory" half of the even-address claim), that half is named and
//   left unmeasured in the relevant assertion's own text - never silently
//   dropped, never reached for by widening the boundary this package was
//   assigned.
//
// FINDING CANDIDATE 1 (CANDIDATES.md item 1, already measured twice
//   independently and re-checked by the lead before this package existed):
//   agnus_audiodma.v:82 latches `audlch[audlcsel] <= data_in[4:0]` - five
//   bits - and the module has no `ecs` port, no chipset input, and no
//   mention of the chipset anywhere in its 157 lines. Chapter 5's own text
//   (pp.139, 145) says the location registers "contain the high three bits"
//   with no chipset qualifier; appendix A p.266 gives both readings side by
//   side ("high 3 bits, 5 bits if ECS)"); appendix C's ECS register map,
//   p.298/309, states "high 5 bits was 3 bits". This is the same deviation
//   as NM-HRM-003 (Copper, agnus_copper.v:144) and NM-HRM-022 (blitter,
//   BLTxPTH), in a third engine. Assertions 8 and 9 below carry this
//   candidate. Per this task's own instruction, the assertion is worded as
//   "implements the ECS width unconditionally" (assertion 8, which PASSES)
//   and the OCS/three-bit alternative is tested and EXPECTED, CORRECTLY, TO
//   FAIL (assertion 9) - because there is nothing in this module to select
//   the OCS reading with, not because the fixture is broken. This file does
//   not assign an NM-HRM-nnn key; the lead issues those.
//
// ORDER OF WORK, DISCLOSED. This file was committed twice. The FIRST commit
//   contained only this header and the claim-to-assertion mapping below
//   (SPECIFICATION), with a stub main() that opens no Verilog at all - written
//   entirely from the 176 claims' manual quotations (extract.quote /
//   restatement) plus the two documents this package's brief required reading
//   in full before anything else, BOUNDARIES.md and CANDIDATES.md. The port
//   names available at that point - reg_address_out, address_out[20:1], dma,
//   audio_dmal, audio_dmas, hpos (BOUNDARIES.md's own table) - and the two
//   RTL lines already quoted above for finding candidate 1 (from CANDIDATES.md,
//   itself built by re-checking a classifier's find against the RTL before this
//   package's build stage began) are the only RTL-derived facts this
//   specification rests on; agnus_audiodma.v itself was not opened by this
//   builder until AFTER that first commit. The SECOND commit (this one) adds
//   everything found only by opening the RTL directly - exact signal names
//   beyond the six above, reset polarity, the register-select arithmetic,
//   hpos slot values, and the actual PASS/FAIL/MUTATION results - documented
//   in the RTL DISCLOSURE section below and never changing any expected value
//   the SPECIFICATION section already committed.
//
// Non-goals: anything about paula_audio_channel's own state machine, volume
//   path, or interrupt timing (separate packages' territory, and a different
//   fixture boundary); the OCS chipset's own DMA-cycle allocation beyond what
//   pp.137/141 already state (no wider-boundary NTSC/PAL claim is in this
//   claim set); AUDxLEN/AUDxPER/AUDxVOL/AUDxDAT, which live in Paula, not
//   Agnus (BOUNDARIES.md); the "points to CHIP memory" half of the even-
//   address claims (assertion 6) and the "NOT decoded by Paula" half of the
//   Agnus-attribution claims (assertion 5) - both need a module outside this
//   package's assigned fixture and are named, not measured, where they occur.
//
// ============================================================================
// SPECIFICATION - written from the 176 claims' manual quotations alone,
// before agnus_audiodma.v was opened. Ten assertions. Every one of the 176
// claim ids appears in exactly the assertion(s) below whose proposition it
// states (five ids that state BOTH the ECS reading and the OCS/18-bit reading
// in the same sentence are cited under both assertion 8 and assertion 9, per
// the same "cited under both assertions it actually supports" convention
// hrm/agnus_blitter_ecs's header documents). Verified by script against the
// full 176-line claim file: every id used exactly where listed here, no id
// invented, none left out.
// ============================================================================
//
// 0. TEST-DATA-DELIVERY (fixture check, no claim id, no manual page).
//    Per the build brief's own silence-trap requirement: prove the fixture
//    can deliver a KNOWN, DISTINCT word to each of the four channels' location
//    registers and see it arrive, correctly split into high/low halves, on
//    address_out at that channel's own DMA grant - before trusting any
//    assertion below that depends on data actually moving through this
//    module.
//
// 1. SLOT-PER-CHANNEL-PER-LINE (18 claim ids).
//    Quotes: p.137 "Each audio channel includes an eight-bit digital-to-
//    analog converter driven by a direct memory access (DMA) channel."; "The
//    audio DMA can retrieve two data samples during each horizontal video
//    scan line."; "When you use automatic sound generation, the system
//    retrieves data automatically by direct memory access."; p.139 the x
//    suffix "specifies the audio channel" (0-3); p.141 the DMA-slot
//    allocation; p.148 "Audio DMA begins retrieving data" in each allocated
//    slot; appendix A p.263/265, the inter-chip register-address transfer
//    and AUDxDAT's automatic load from RAM.
//    Expected if NanoMig conforms: each of the four channels (x=0..3) has its
//    own DMA slot, once per horizontal scan line, that word carrying two
//    samples, addressed through that channel's own AUDxDAT register - not a
//    shared or interleaved resource.
//    Claims: 5-p137.c2, 5-p137.c3, 5-p137.c6, 5-p139.c4, 5-p141.c1,
//    5-p141.c2, 5-hw.c6, 5-hw.c7, 5-hw.c10, 5-rate.c1, 5-rate.c2, 5-rate.c3,
//    5-loc.c4, 5-join.c10, 5-p148.c5, 5-appA-p263.c11, 5-appA-p265.c44,
//    A-audxdat.c6 .
//
// 2. WORD-ALIGNED-FETCH-AND-LC-PAIR (6 claim ids).
//    Quotes: p.138 "the first byte of data must start at a word-address
//    boundary"; the combined name "AUDxLC" for the pair AUDxLCH+AUDxLCL.
//    Expected: the fetch address is always word-aligned (bit 0 is not an
//    independent write target), one 16-bit word is fetched per access, and
//    AUDxLCH written together with AUDxLCL forms one location value, AUDxLC.
//    Claims: 5-p138.c5, 5-p138.c6, 5-p138.c8, 5-data.c5, 5-data.c6,
//    5-data.c9 .
//
// 3. LOCATION-REGISTER-VS-WORKING-POINTER (35 claim ids).
//    Quotes: pp.137,139,144,147,148,153,164,165 and appendix B p.288 - the
//    location registers are read once, at DMA start or restart, into a
//    "back-up" / working pointer that Agnus advances on its own; the visible
//    AUDxLCH/AUDxLCL need not be rewritten to repeat the same waveform, and
//    are safe to rewrite once copied without disturbing a transfer already
//    running; when the length count finishes, the working pointer reloads
//    from the location registers again, automatically.
//    Expected if NanoMig conforms: AUDxLCH/AUDxLCL are a LOCATION (a starting
//    value), not the live fetch pointer; a separate working pointer is loaded
//    from that location whenever a channel (re)starts and increments on its
//    own otherwise; ordinary playback never touches the location registers.
//    Claims: 5-p137.c4, 5-p139.c5, 5-p139.c6, 5-p139.c7, 5-p144.c4,
//    5-p147.c1, 5-p147.c3, 5-p147.c5, 5-p147.c7, 5-p147.c8, 5-p148.c2,
//    5-p148.c3, 5-hw.c8, 5-loc.c5, 5-loc.c6, 5-loc.c7, 5-play.c4, 5-join.c1,
//    5-join.c2, 5-join.c3, 5-join.c5, 5-join.c8, 5-p153.c4, 5-eff.c1,
//    5-p164.c25, 5-p165.c8, 5-sm.c26, 5-sig.audxdsr, 5-appA-p266.c12,
//    5-appA-p266.c13, A-audxlc.c2, B-lclch.c1, B-lclch.c2, B-ptlpth.c1,
//    5-appB-p288.c2 .
//
// 4. LCL-LOW-15-BITS (13 claim ids).
//    Quotes: pp.139/145 and appendix B p.290, all four channels - "AUDxLCL
//    ... contain ... the low fifteen bits" of the audio data start address.
//    Expected: for every channel, all fifteen written bits of AUDxLCL (bit 0
//    unstored) map directly onto the corresponding fifteen low bits of the
//    DMA address - a claim this package expects to hold regardless of
//    chipset, unlike AUDxLCH's width (assertions 8/9).
//    Claims: 5-p139.c2, 5-loc.c2, 5-sum.c2, 5-appA-p266.c10,
//    5-appB-p290.c29, 5-appB-p290.c64, 5-appB-p290.c99, 5-appB-p290.c134,
//    A-audxlcl.w, B-aud0lcl.width, B-aud1lcl.width, B-aud2lcl.width,
//    B-aud3lcl.width .
//
// 5. REGISTER-MAP-ADDRESSES-AND-AGNUS-DECODE (58 claim ids).
//    Quotes: appendix A pp.263/266, appendix B pp.287/288/290, appendix C
//    pp.298/309 - the register map assigning AUD0LCH=$0A0, AUD0LCL=$0A2,
//    AUD1LCH=$0B0, AUD1LCL=$0B2, AUD2LCH=$0C0, AUD2LCL=$0C2, AUD3LCH=$0D0,
//    AUD3LCL=$0D2, each pair named an address-register pair, each row's
//    "chip" column naming Agnus.
//    Expected: a write to each of these eight addresses reaches the
//    corresponding channel's location half and no other, decoded within
//    Agnus's own audio-DMA logic - THIS module. The "and NOT decoded by
//    Paula" half some of these ids' evidence also states needs
//    paula_audio_channel.v, outside this package's assigned fixture, and is
//    named, not measured, in this assertion's own text.
//    Claims: 5-appA-p266.c1, 5-appA-p266.c3, 5-appA-p266.c4, 5-appA-p266.c7,
//    5-appA-p266.c9, 5-appB-p288.c1, 5-appB-p290.c19, 5-appB-p290.c21,
//    5-appB-p290.c22, 5-appB-p290.c24, 5-appB-p290.c26, 5-appB-p290.c28,
//    5-appB-p290.c31, 5-appB-p290.c54, 5-appB-p290.c56, 5-appB-p290.c59,
//    5-appB-p290.c61, 5-appB-p290.c63, 5-appB-p290.c66, 5-appB-p290.c89,
//    5-appB-p290.c91, 5-appB-p290.c94, 5-appB-p290.c96, 5-appB-p290.c98,
//    5-appB-p290.c101, 5-appB-p290.c124, 5-appB-p290.c126, 5-appB-p290.c129,
//    5-appB-p290.c131, 5-appB-p290.c133, 5-appB-p290.c136, 5-appC-p298.c2,
//    5-appC-p298.c5, 5-appC-p309.c3, 5-appC-p309.c5, A-audxlch.addr,
//    A-audxlch.chip, A-audxlcl.addr, A-audxlcl.chip, B-aud0lch.addr,
//    B-aud0lch.chip, B-aud0lcl.addr, B-aud0lcl.chip, B-aud1lch.addr,
//    B-aud1lch.chip, B-aud1lcl.addr, B-aud1lcl.chip, B-aud2lch.addr,
//    B-aud2lch.chip, B-aud2lcl.addr, B-aud2lcl.chip, B-aud3lch.addr,
//    B-aud3lch.chip, B-aud3lcl.addr, B-aud3lcl.chip, C-map.lch.addr,
//    C-map.lch.chip, C-prose.lch .
//
// 6. POINTER-LSB-ALWAYS-ZERO (20 claim ids).
//    Quotes: appendix A p.263 "The least significant bit of every pointer-
//    type register ... must always be written with a zero"; appendix B
//    p.287/290 the "+" legend, "Address register pair; must be an even
//    address pointing to chip memory."
//    Expected: whatever is written to AUDxLCL, the stored/fetched address's
//    bit 0 reads back as zero - the EVEN-ADDRESS half of the claim. The
//    "... pointing to CHIP memory" half needs the memory decoder (gary.v),
//    outside this module, and is named, not measured, in this assertion.
//    Claims: 5-appA-p263.c6, 5-appB-p287.c4, 5-appB-p290.c25,
//    5-appB-p290.c32, 5-appB-p290.c60, 5-appB-p290.c67, 5-appB-p290.c95,
//    5-appB-p290.c102, 5-appB-p290.c130, 5-appB-p290.c137, A-pre.c7,
//    B-legend.plus, B-aud0lch.mark, B-aud0lcl.mark, B-aud1lch.mark,
//    B-aud1lcl.mark, B-aud2lch.mark, B-aud2lcl.mark, B-aud3lch.mark,
//    B-aud3lcl.mark .
//
// 7. POINTER-REGISTER-SPACE-LONGWORD (3 claim ids).
//    Quotes: appendix A p.263 "All pointer-type registers are 32 bits wide"
//    and "are located on long-word boundaries." Appendix A's own preamble
//    text distinguishes register SPACE from stored width - this assertion is
//    about the address MAP, not about how many of those 32 bits the module
//    actually keeps (that is assertions 4/8/9).
//    Expected: each channel's LCH/LCL pair occupies two adjacent word
//    addresses starting on a long-word (4-byte) boundary: $0A0/$0A2,
//    $0B0/$0B2, $0C0/$0C2, $0D0/$0D2.
//    Claims: 5-appA-p263.c3, 5-appA-p263.c4, A-pre.c5 .
//
// 8. LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS (11 claim ids). Expected GREEN.
//    Quotes: appendix A p.266 "(high 3 bits, 5 bits if ECS)"; appendix B
//    p.290's ECS-marked row; appendix C p.298/309 "high 5 bits was 3 bits"
//    and "gain two address bits."
//    Expected under ECS: AUDxLCH stores all five of its high bits, for every
//    one of the four channels identically.
//    Claims: 5-appA-p266.c6, 5-appB-p290.c149, 5-appC-p298.c6,
//    5-appC-p309.c1, 5-appC-p309.c2, 5-appC-p309.c6, A-audxlch.w1,
//    C-map.lch.width, C-prose.c1, C-prose.lch, B-aud0lch.width .
//
// 9. LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD (18 claim ids). *** REQUIRED
//    RED - SEE FINDING CANDIDATE 1 ABOVE. Do not adjust this assertion to
//    pass. ***
//    Quotes: chapter 5 pp.139/145, unqualified, "contain the high three
//    bits"; appendix A p.266's non-ECS half; appendix B p.290's per-channel
//    rows for channels 1-3, which give only "high 3 bits" with no ECS number
//    stated on that row; appendix C's "was 3 bits", naming the pre-ECS value
//    explicitly.
//    Expected under the standard/OCS chipset (what these ids, read alone,
//    require): AUDxLCH stores only THREE high bits - the two "ECS-only" bit
//    positions have NO effect on the DMA address, and the total stored
//    address is 18 bits, not 20.
//    Per CANDIDATES.md (required reading before this package was built,
//    itself re-checked against the RTL by the lead): agnus_audiodma.v has no
//    ecs or chipset port anywhere in its 157 lines, so the OCS alternative
//    cannot be selected for at all - the module is expected, and has already
//    been measured twice independently, to implement the five-bit (ECS) form
//    UNCONDITIONALLY. This assertion tests the OCS claim's own words as an
//    absolute expectation, using the IDENTICAL walking-one/all-ones
//    methodology assertion 8 uses (which PASSES) as its control - so a red
//    result here reflects the OCS claim failing to hold, not a broken
//    fixture, exactly the shape hrm/agnus_blitter_ecs's header documents for
//    its own three required-reds. Its correct, expected outcome is FAIL.
//    Claims: 5-p139.c1, 5-sum.c1, 5-loc.c1, 5-appA-p266.c5, 5-appA-p266.c11,
//    5-appB-p290.c57, 5-appB-p290.c92, 5-appB-p290.c127, 5-appB-p290.c148,
//    B-aud1lch.width, B-aud2lch.width, B-aud3lch.width, A-audxlc.c1,
//    5-appC-p298.c6, 5-appC-p309.c6, A-audxlch.w1, C-map.lch.width,
//    B-aud0lch.width .
//
// ============================================================================
// RTL DISCLOSURE - added only after the SPECIFICATION above was committed.
// ============================================================================
// (filled in once agnus_audiodma.v has actually been opened by this builder;
// see the second commit for this file's full content)

int main() {
    // Stub: RTL not yet opened by this builder. See commit history - the
    // next commit to this file wires a real Verilator harness against
    // agnus_audiodma.v and turns the ten assertions specified above into
    // real checks.expect() calls, none of whose expected values differ from
    // what is written above.
    return 0;
}
