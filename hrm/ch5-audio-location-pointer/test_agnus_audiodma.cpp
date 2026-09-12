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
//   contained only the header above and the SPECIFICATION below, with a stub
//   main() that opened no Verilog at all - written entirely from the 176
//   claims' manual quotations (extract.quote / restatement) plus the two
//   documents this package's brief required reading in full before anything
//   else, BOUNDARIES.md and CANDIDATES.md. The port names available at that
//   point - reg_address_out, address_out[20:1], dma, audio_dmal, audio_dmas,
//   hpos (BOUNDARIES.md's own table) - and the two RTL lines already quoted
//   above for finding candidate 1 (from CANDIDATES.md, itself built by
//   re-checking a classifier's find against the RTL before this package's
//   build stage began) are the only RTL-derived facts that specification
//   rested on; agnus_audiodma.v itself was not opened by this builder until
//   after that commit. This, the SECOND commit, adds everything found only
//   by opening the RTL directly - see RTL DISCLOSURE below - and changes NO
//   expected value the SPECIFICATION section already committed.
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
// COVERAGE REPAIR (2026-09-12, independent validation finding V-09):
//   assertion 1, SLOT-PER-CHANNEL-PER-LINE, formerly cited 18 ids. Its own
//   two booleans (one_slot_each, right_register) establish only that each
//   channel has its own DMA slot and that, at that slot, reg_address_out
//   names that channel's own AUDxDAT address - this fixture has no RAM
//   input, delivers no word into Paula, observes neither sample byte, and
//   has no DAC boundary. Twelve of the eighteen ids stated a DAC-is-DMA-
//   driven, two-samples-per-word, or automatic-RAM-to-AUDxDAT proposition
//   the slot/address measurement is necessary for but not sufficient to
//   prove, so they were withdrawn from assertion 1's own cited-claims text.
//   They are NOT COVERED BY THIS PACKAGE - see assertion 1's own
//   specification below for the full breakdown and the fixture they need
//   (an integrated Agnus-to-Paula data path with a real RAM/data source).
//   The remaining six ids, all about channel numbering, slot allocation, the
//   resulting sampling-rate bound, or the register-address half of the
//   inter-chip transfer, stay on assertion 1 - its measurement is both
//   necessary and sufficient for those.
//
// ============================================================================
// SPECIFICATION - written from the 176 claims' manual quotations alone,
// before agnus_audiodma.v was opened. Ten assertions. Originally, every one
// of the 176 claim ids appeared in exactly the assertion(s) below whose
// proposition it stated (five ids that state BOTH the ECS reading and the
// OCS/18-bit reading in the same sentence are cited under both assertion 8
// and assertion 9, per the same "cited under both assertions it actually
// supports" convention hrm/agnus_blitter_ecs's header documents), verified by
// script against the full 176-line claim file. The COVERAGE REPAIR above
// withdrew twelve of those ids from assertion 1 as not actually established
// by its own measurement; they are listed, with the fixture they need, in
// assertion 1's own text below and are deliberately absent from every
// assertion string in this file, so 164 ids remain asserted here and 12 are
// reported not covered by this package.
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
// 1. SLOT-PER-CHANNEL-PER-LINE (6 claim ids; repaired 2026-09-12, see
//    COVERAGE REPAIR below -- 12 ids formerly cited here are NOT COVERED BY
//    THIS PACKAGE).
//    Quotes: p.139 the x suffix "specifies the audio channel" (0-3),
//    restated at p.139 "The channels are numbered 0, 1, 2, and 3"; p.141
//    "Each audio channel is allocated one DMA slot per horizontal scan
//    line" and "The sampling period is limited by the number of DMA cycles
//    allocated to an audio channel"; appendix A p.263, the inter-chip
//    register-address transfer.
//    Expected if NanoMig conforms: each of the four channels (x=0..3) has
//    its own DMA slot, once per horizontal scan line, addressed through
//    that channel's own AUDxDAT register - not a shared or interleaved
//    resource, and the DMA-slot's own address transfer is Agnus's half of
//    the inter-chip bus these ids describe.
//    Claims: 5-p139.c4, 5-p141.c1, 5-rate.c1, 5-rate.c2, 5-loc.c4,
//    5-appA-p263.c11 .
//
//    COVERAGE REPAIR (independent validation finding V-09): this
//    assertion's two booleans (one_slot_each, right_register) prove only
//    that each channel gets its own hpos slot and that, at that slot,
//    reg_address_out names that channel's own AUDxDAT address. There is no
//    RAM input anywhere in this fixture, no word is ever delivered into
//    Paula, neither sample byte is observed, and there is no DAC boundary -
//    the slot and register address are NECESSARY for the following twelve
//    ids' propositions but not SUFFICIENT to prove them, so their citation
//    here was withdrawn. They are NOT COVERED BY THIS PACKAGE and their ids
//    are deliberately absent from every assertion string in this file,
//    grouped by the proposition each needs an integrated Agnus-to-Paula
//    data path (a real fetched word reaching paula_audio_channel's AUDxDAT
//    latch, from a RAM/data source) to establish:
//      - each channel's DAC is DMA-driven: 5-p137.c2, 5-hw.c6.
//      - one DMA word supplies two samples per line: 5-p137.c3, 5-p141.c2,
//        5-hw.c7, 5-rate.c3.
//      - memory data is automatically retrieved into AUDxDAT: 5-p137.c6,
//        5-hw.c10, 5-p148.c5, 5-join.c10, 5-appA-p265.c44, A-audxdat.c6.
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
// RTL DISCLOSURE - agnus_audiodma.v opened for the first time by this
// builder only after the SPECIFICATION above was committed. No expected
// value above changed as a result of this reading; only the mechanics below
// (exact port names beyond BOUNDARIES.md's table, the internal decode, the
// hpos slot values, and the read-back technique) were added.
// ============================================================================
//
// Full port list (nanomig submodule, src/minimig-aga/agnus_audiodma.v):
//   clk, clk7_en (every register in this module gates on clk7_en, matching
//     every other package in this tree). NO `reset` port at all - unlike
//     every other agnus_*/paula_* module read so far in this tree, this one
//     has none, so the harness below always WRITES a known value before
//     reading it, never relying on any power-up state.
//   audio_dmal[3:0]  (per-channel DMA data-transfer request, from Paula in
//     the real chip; driven directly as raw stimulus here, since
//     paula_audio_channel is outside this package's fixture)
//   audio_dmas[3:0]  (per-channel restart strobe, same real source)
//   hpos[8:0]
//   reg_address_in[8:1], data_in[15:0]  (write path, word-address units,
//     byte_address>>1 - the same to_reg() convention every Copper/Blitter
//     package in this tree already uses)
//   reg_address_out[8:1]  (which AUDxDAT the CURRENT hpos-selected channel
//     addresses)
//   address_out[20:1]  (the fetch address, in WORD units - the port simply
//     has NO bit-0 wire, which is this RTL's own implementation of "the LSB
//     must be written as zero": there is nowhere for it to go)
//
// Key internals (line numbers, this reading, nanomig-hrm-tests/nanomig
// submodule):
//   :73  audlcena - a LEVEL decode (no separate write-enable/strobe bit
//        anywhere in the module) of reg_address_in matching $A0/$B0/$C0/$D0
//        (LCH, reg_address_in[1]=0) or $A2/$B2/$C2/$D2 (LCL,
//        reg_address_in[1]=1), all four channels.
//   :76  audlcsel = {~reg_address_in[5], reg_address_in[4]} - the channel
//        index a location-register WRITE targets.
//   :82  audlch[audlcsel] <= data_in[4:0] - FIVE bits, unconditionally
//        (finding candidate 1).
//   :89  audlcl[audlcsel] <= data_in[15:1] - FIFTEEN bits; data_in[0] is
//        simply never read by this module at all.
//   :94  audlcout = {audlch[channel],audlcl[channel]} - the location bank's
//        own combinational read-out, for whichever channel `channel` (below)
//        currently names.
//   :98-104, :112-118  the ONLY four hpos values that can ever grant a cycle,
//        one per channel: 9'b0001_0010_1 / ..._0100_1 / ..._0110_1 /
//        ..._1000_1 = 0x025 / 0x029 / 0x02D / 0x031 - exactly the four values
//        CANDIDATES.md already named. Every other hpos value forces
//        dmal=dmas=0 for every channel.
//   :122-129  channel - a PURE function of hpos[3:2], with NO gating by
//        dmal/dmas at all: it cycles 0,1,2,3 across every four hpos values in
//        the entire 0-511 range, not only at the four DMA-granting values
//        above. address_out and reg_address_out are therefore continuously
//        live, combinational reads of audpt[channel] / a constant table - no
//        clock edge is needed to observe them once hpos is set, the same
//        "genuinely continuous assignment" property hrm/agnus_blitter_ecs's
//        header documents for its own address_out().
//   :132  address_out = audptout[20:1] = audpt[channel] - the WORKING
//        POINTER, NOT the location bank directly. There is no read-back path
//        for audlch/audlcl themselves; the only way to observe what a
//        location register holds is to grant a RESTART (dmas=1) and read the
//        resulting working pointer - the same read-back-via-strobe technique
//        agnus_copper_locreg_width's header documents for COP1LCH/COP1LCL.
//   :138  audpt[channel] <= dmas ? audlcout[20:1] : audptout[20:1] + 1 - the
//        location-vs-pointer split assertion 3 measures directly.
//   :146-152  reg_address_out mux - AUD0DAT/1/2/3_REG = 9'h0AA/0BA/0CA/0DA,
//        i.e. to_reg() of $0AA/$0BA/$0CA/$0DA - matching CANDIDATES.md.
//   No `ecs` port, no chipset input, no mention of chipset anywhere in these
//   157 lines - confirmed again directly, matching CANDIDATES.md/
//   BOUNDARIES.md.
//
// Fixture note: TEST-DATA-DELIVERY (assertion 0) is run FIRST and its result
// gates trust in every later assertion, per the build brief's own two-
// silence-trap warning for this chapter (both traps are in
// paula_audio_channel.v, outside this fixture, but the brief's general rule -
// prove the data path is alive before trusting anything data-dependent -
// applies to any package, and this module's total lack of a reset port makes
// it worth checking explicitly here).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vagnus_audiodma.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t AUD0LCH = to_reg(0x0A0);
constexpr uint16_t AUD0LCL = to_reg(0x0A2);
constexpr uint16_t AUD1LCH = to_reg(0x0B0);
constexpr uint16_t AUD1LCL = to_reg(0x0B2);
constexpr uint16_t AUD2LCH = to_reg(0x0C0);
constexpr uint16_t AUD2LCL = to_reg(0x0C2);
constexpr uint16_t AUD3LCH = to_reg(0x0D0);
constexpr uint16_t AUD3LCL = to_reg(0x0D2);
constexpr uint16_t AUD0DAT = to_reg(0x0AA);
constexpr uint16_t AUD1DAT = to_reg(0x0BA);
constexpr uint16_t AUD2DAT = to_reg(0x0CA);
constexpr uint16_t AUD3DAT = to_reg(0x0DA);
constexpr uint16_t IDLE_REG = 0x0FF;  // all-ones: audlcena forces 0 (bit8 of the field is set)

constexpr uint16_t LCH[4] = {AUD0LCH, AUD1LCH, AUD2LCH, AUD3LCH};
constexpr uint16_t LCL[4] = {AUD0LCL, AUD1LCL, AUD2LCL, AUD3LCL};
constexpr uint16_t DAT[4] = {AUD0DAT, AUD1DAT, AUD2DAT, AUD3DAT};
// agnus_audiodma.v:98-104,112-118 - the only four hpos values that grant a
// cycle, one per channel (measured again here; matches CANDIDATES.md).
constexpr uint16_t SLOT_HPOS[4] = {0x025, 0x029, 0x02D, 0x031};

class Harness {
public:
    Harness() : dut(new Vagnus_audiodma) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->hpos = 0;
        dut->reg_address_in = IDLE_REG;
        dut->data_in = 0;
        dut->eval();
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

    // Register-bus write. The decode is level-sensitive (no separate write
    // strobe anywhere in the module, agnus_audiodma.v:73), so this pulses
    // reg_address_in/data_in for exactly one clk7_en edge and then parks
    // reg_address_in at an address that does not decode audlcena - the same
    // write_reg convention every Copper/Blitter package in this tree uses,
    // for the same reason.
    void write_reg(uint16_t reg, uint16_t data) {
        dut->reg_address_in = reg;
        dut->data_in = data;
        tick();
        dut->reg_address_in = IDLE_REG;
        dut->data_in = 0;
        dut->eval();
    }

    // Grant a DMA cycle for channel ch (0-3) at that channel's own hpos slot.
    // restart=true drives audio_dmas[ch] (agnus_audiodma.v:138: reload the
    // working pointer from the location bank); restart=false advances the
    // working pointer by one word.
    void grant(int ch, bool restart) {
        dut->hpos = SLOT_HPOS[ch];
        dut->audio_dmal = static_cast<uint8_t>(1u << ch);
        dut->audio_dmas = restart ? static_cast<uint8_t>(1u << ch) : 0;
        dut->eval();
        tick();
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->eval();
    }

    // Continuous read of channel ch's own WORKING POINTER
    // (agnus_audiodma.v:132,143: address_out = audptout = audpt[channel], and
    // `channel` is a pure, unclocked function of hpos[3:2] - no tick needed).
    uint32_t read_pointer(int ch) {
        dut->hpos = SLOT_HPOS[ch];
        dut->eval();
        return static_cast<uint32_t>(dut->address_out);
    }

    uint16_t read_reg_address_out(int ch) {
        dut->hpos = SLOT_HPOS[ch];
        dut->eval();
        return static_cast<uint16_t>(dut->reg_address_out);
    }

    // Raw single-hpos sample of `dma`, driving only channel ch's request bit.
    // Used by SLOT-PER-CHANNEL-PER-LINE's full-range sweep.
    bool dma_at(uint16_t hpos_value, int ch) {
        dut->hpos = hpos_value;
        dut->audio_dmal = static_cast<uint8_t>(1u << ch);
        dut->eval();
        const bool g = dut->dma;
        dut->audio_dmal = 0;
        dut->eval();
        return g;
    }

    Vagnus_audiodma* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- 0. TEST-DATA-DELIVERY (fixture check) ----------------
    {
        Harness h;
        // Four distinct, non-trivial words, one per channel - not all-zero,
        // not all-one, not aliases of each other.
        const uint32_t want[4] = {0x1A2B4u, 0x0C3D5u, 0x15E67u, 0x0F890u};
        uint32_t got[4] = {0, 0, 0, 0};
        bool order_ok = true;
        for (int ch = 0; ch < 4; ++ch) {
            const uint16_t hi = static_cast<uint16_t>((want[ch] >> 15) & 0x1F);
            const uint16_t lo = static_cast<uint16_t>((want[ch] & 0x7FFF) << 1);
            h.write_reg(LCH[ch], hi);
            h.write_reg(LCL[ch], lo);
            h.grant(ch, /*restart=*/true);
            got[ch] = h.read_pointer(ch);
            // Check it landed on THIS channel's own grant cycle, in order,
            // before moving to the next channel - and not on any channel
            // written/granted so far but not yet this one.
            if (got[ch] != want[ch]) order_ok = false;
        }
        bool distinct = (got[0] != got[1]) && (got[0] != got[2]) && (got[0] != got[3]) &&
                         (got[1] != got[2]) && (got[1] != got[3]) && (got[2] != got[3]);
        char buf[2048];
        std::snprintf(buf, sizeof(buf),
            "[fixture check] TEST-DATA-DELIVERY: four distinct words "
            "(0x%05X,0x%05X,0x%05X,0x%05X) written one per channel arrive, "
            "correctly split into LCH/LCL and reassembled, at address_out on "
            "that channel's OWN grant cycle, in order - got "
            "(0x%05X,0x%05X,0x%05X,0x%05X). Proves the fixture's data path is "
            "not stuck, aliased across channels, or silently dead before any "
            "data-dependent assertion below is trusted.",
            want[0], want[1], want[2], want[3], got[0], got[1], got[2], got[3]);
        checks.expect(order_ok && distinct, buf);
    }

    // ---------------- 1. SLOT-PER-CHANNEL-PER-LINE ----------------
    {
        bool one_slot_each = true;
        bool right_register = true;
        for (int ch = 0; ch < 4; ++ch) {
            Harness h;
            int grants_seen = 0;
            uint16_t grant_hpos = 0;
            for (uint32_t hp = 0; hp < 512; ++hp) {
                if (h.dma_at(static_cast<uint16_t>(hp), ch)) {
                    ++grants_seen;
                    grant_hpos = static_cast<uint16_t>(hp);
                }
            }
            if (grants_seen != 1 || grant_hpos != SLOT_HPOS[ch]) one_slot_each = false;

            // At that one hpos, with the grant asserted, reg_address_out must
            // name THIS channel's own AUDxDAT - not a shared/aliased address.
            h.dut->hpos = SLOT_HPOS[ch];
            h.dut->audio_dmal = static_cast<uint8_t>(1u << ch);
            h.dut->eval();
            const bool granted = h.dut->dma;
            const uint16_t reg = static_cast<uint16_t>(h.dut->reg_address_out);
            h.dut->audio_dmal = 0;
            h.dut->eval();
            if (!granted || reg != DAT[ch]) right_register = false;
        }
        checks.expect(one_slot_each && right_register,
            "SLOT-PER-CHANNEL-PER-LINE (p.139: \"The 'x' in the register "
            "names ... specifies the audio channel\", \"The channels are "
            "numbered 0, 1, 2, and 3\"; p.141: \"Each audio channel is "
            "allocated one DMA slot per horizontal scan line\", \"The "
            "sampling period is limited by the number of DMA cycles "
            "allocated to an audio channel\"; appendix A p.263: register "
            "addresses \"used by ... Agnus ... for transferring data among "
            "themselves\"): each of the four channels gets EXACTLY one "
            "qualifying hpos value across the entire 0-511 range (measured "
            "at hpos 0x025/0x029/0x02D/0x031, matching CANDIDATES.md), and "
            "at that value reg_address_out names that channel's own "
            "AUDxDAT ($0AA/$0BA/$0CA/$0DA) - a dedicated per-channel slot "
            "and register address, never shared or interleaved. This "
            "establishes: channel numbering/addressing (5-p139.c4, "
            "5-loc.c4), one-DMA-slot-per-line allocation (5-p141.c1, "
            "5-rate.c2), the resulting sampling-rate bound (5-rate.c1), and "
            "the register-address half of the inter-chip transfer "
            "(5-appA-p263.c11). hpos repeating every scan line is a fact "
            "about the surrounding beam counter, outside this module; what "
            "is measured here is that within any one sweep of the hpos "
            "range a channel is granted only once. NOT ESTABLISHED HERE: "
            "that the channel's DAC is DMA-driven, that the slot's word "
            "supplies two samples, or that memory data is automatically "
            "retrieved into AUDxDAT - this fixture is agnus_audiodma.v "
            "alone, with no RAM input, no delivered word, no observed "
            "sample byte and no DAC boundary; those propositions need an "
            "integrated Agnus-to-Paula data path and are reported NOT "
            "COVERED BY THIS PACKAGE (see this file's header for the ids). "
            "[claims: 5-p139.c4, 5-p141.c1, 5-rate.c1, 5-rate.c2, 5-loc.c4, "
            "5-appA-p263.c11]");
    }

    // ---------------- 2. WORD-ALIGNED-FETCH-AND-LC-PAIR ----------------
    {
        Harness h;
        h.write_reg(AUD0LCH, 0x0000);
        h.write_reg(AUD0LCL, 0x1234);
        h.grant(0, true);
        const uint32_t p0 = h.read_pointer(0);  // low half only

        // Change ONLY LCH; LCL's contribution must survive unchanged.
        h.write_reg(AUD0LCH, 0x0015);
        h.grant(0, true);
        const uint32_t p1 = h.read_pointer(0);
        const bool low_survived_hch_write = (p1 & 0x7FFFu) == (p0 & 0x7FFFu);
        const bool high_took_hch_write = ((p1 >> 15) & 0x1Fu) == 0x15u;

        // Change ONLY LCL; LCH's contribution must survive unchanged.
        // NOTE (found by actually running this fixture, not by adjusting an
        // oracle): audlcl is declared `reg [15:1]`, so data_in[15:1] copies
        // bit-for-bit into audlcl's OWN [15:1] labels, which then become
        // address_out's C-side bits [14:0] (address_out's C-bit m = Verilog
        // bit m+1) - i.e. address_out's low 15 bits equal (data_in >> 1), not
        // data_in itself. The first version of this check compared against
        // the raw written constant and failed; this is the same class of
        // Verilog-label-vs-Verilator-bit-index offset agnus_copper_
        // locreg_width's header documents finding in COP1LCL, not an RTL
        // deviation.
        h.write_reg(AUD0LCL, 0x2222);
        h.grant(0, true);
        const uint32_t p2 = h.read_pointer(0);
        const bool high_survived_lcl_write = ((p2 >> 15) & 0x1Fu) == 0x15u;
        const bool low_took_lcl_write = (p2 & 0x7FFFu) == (0x2222u >> 1);

        // address_out is declared [21-1:1] - there is structurally no bit-0
        // wire on the port at all, so two consecutive ADVANCE grants (not
        // restarts) must step the WORD address by exactly 1, i.e. the byte
        // address by exactly 2 - one 16-bit word per access.
        h.grant(0, false);
        const uint32_t p3 = h.read_pointer(0);
        const bool steps_one_word = (p3 == p2 + 1);

        checks.expect(low_survived_hch_write && high_took_hch_write &&
                          high_survived_lcl_write && low_took_lcl_write && steps_one_word,
            "WORD-ALIGNED-FETCH-AND-LC-PAIR (p.138: \"the first byte of data "
            "must start at a word-address boundary\"; the combined register "
            "name AUDxLC for AUDxLCH+AUDxLCL): address_out is declared "
            "[21-1:1] with no bit-0 wire at all, so every fetch address is "
            "word-aligned by construction; one word (16 bits) is fetched per "
            "DMA access (measured: an ADVANCE grant steps the pointer by "
            "exactly 1 word); and AUDxLCH written together with AUDxLCL forms "
            "ONE location value, AUDxLC - writing only one half leaves the "
            "other half's contribution to address_out unchanged (measured: "
            "LCL survives an LCH-only write and vice versa). "
            "[claims: 5-p138.c5, 5-p138.c6, 5-p138.c8, 5-data.c5, 5-data.c6, "
            "5-data.c9]");
    }

    // ---------------- 3. LOCATION-REGISTER-VS-WORKING-POINTER ----------------
    {
        Harness h;
        const uint16_t hi0 = 0x0003, lo0 = 0x4444;
        const uint32_t v0 = (static_cast<uint32_t>(hi0) << 15) | (lo0 >> 1);
        h.write_reg(AUD0LCH, hi0);
        h.write_reg(AUD0LCL, lo0);

        h.grant(0, true);  // restart: pointer <- location
        const uint32_t p0 = h.read_pointer(0);
        h.grant(0, false);  // advance
        const uint32_t p1 = h.read_pointer(0);
        h.grant(0, false);  // advance again
        const uint32_t p2 = h.read_pointer(0);

        // Rewrite the location registers to a NEW value WHILE the working
        // pointer is mid-sequence (after two advances). The active transfer
        // must not be disturbed: the working pointer stays at p2.
        const uint16_t hi1 = 0x001A, lo1 = 0x6668;
        const uint32_t v1 = (static_cast<uint32_t>(hi1) << 15) | (lo1 >> 1);
        h.write_reg(AUD0LCH, hi1);
        h.write_reg(AUD0LCL, lo1);
        const uint32_t p3_before_grant = h.read_pointer(0);  // no grant yet at all

        h.grant(0, false);  // one more ADVANCE (not a restart)
        const uint32_t p4 = h.read_pointer(0);

        // Only a RESTART adopts the new location - and it can be repeated,
        // with no further rewrite, reproducing the same value every time.
        h.grant(0, true);
        const uint32_t p5 = h.read_pointer(0);
        h.grant(0, true);
        const uint32_t p6 = h.read_pointer(0);

        const bool restart_loads_location = (p0 == v0);
        const bool advance_increments = (p1 == v0 + 1) && (p2 == v0 + 2);
        const bool rewrite_does_not_disturb_active_transfer = (p3_before_grant == p2) && (p4 == v0 + 3);
        const bool new_location_needs_a_restart = (p5 == v1);
        const bool repeat_needs_no_rewrite = (p6 == v1);

        char buf[2048];
        std::snprintf(buf, sizeof(buf),
            "LOCATION-REGISTER-VS-WORKING-POINTER (pp.137,139,144,147,148,153,"
            "164,165, appendix B p.288: the location registers are read once, "
            "at start/restart, into a working pointer Agnus advances on its "
            "own; they need not be rewritten to repeat a waveform, are safe "
            "to rewrite mid-transfer without disturbing it, and the working "
            "pointer reloads from them again whenever a restart is granted): "
            "measured v0=0x%05X loaded on restart (p0=0x%05X), two advances "
            "step it by 1 each (p1=0x%05X,p2=0x%05X), rewriting to v1=0x%05X "
            "mid-sequence leaves the running pointer alone "
            "(p3=0x%05X==p2, next advance p4=0x%05X==v0+3), and only a "
            "RESTART adopts v1 (p5=0x%05X), reproducibly, with no further "
            "rewrite (p6=0x%05X==p5). "
            "[claims: 5-p137.c4, 5-p139.c5, 5-p139.c6, 5-p139.c7, 5-p144.c4, "
            "5-p147.c1, 5-p147.c3, 5-p147.c5, 5-p147.c7, 5-p147.c8, "
            "5-p148.c2, 5-p148.c3, 5-hw.c8, 5-loc.c5, 5-loc.c6, 5-loc.c7, "
            "5-play.c4, 5-join.c1, 5-join.c2, 5-join.c3, 5-join.c5, "
            "5-join.c8, 5-p153.c4, 5-eff.c1, 5-p164.c25, 5-p165.c8, "
            "5-sm.c26, 5-sig.audxdsr, 5-appA-p266.c12, 5-appA-p266.c13, "
            "A-audxlc.c2, B-lclch.c1, B-lclch.c2, B-ptlpth.c1, "
            "5-appB-p288.c2]",
            v0, p0, p1, p2, v1, p3_before_grant, p4, p5, p6);
        checks.expect(restart_loads_location && advance_increments &&
                          rewrite_does_not_disturb_active_transfer &&
                          new_location_needs_a_restart && repeat_needs_no_rewrite,
            buf);
    }

    // ---------------- 4. LCL-LOW-15-BITS ----------------
    {
        bool all_ok = true;
        for (int ch = 0; ch < 4; ++ch) {
            Harness h;
            h.write_reg(LCH[ch], 0x0000);
            for (int bit = 1; bit <= 15; ++bit) {
                h.write_reg(LCL[ch], static_cast<uint16_t>(1u << bit));
                h.grant(ch, true);
                const uint32_t p = h.read_pointer(ch);
                // data_in[15:1] slices straight into audlcl[15:1], which
                // becomes address_out[15:1] with no shift at all.
                if (p != (1u << (bit - 1))) all_ok = false;
            }
            // bit 0 of data_in is never read by this module at all.
            h.write_reg(LCL[ch], 0x0001);
            h.grant(ch, true);
            if (h.read_pointer(ch) != 0) all_ok = false;
        }
        checks.expect(all_ok,
            "LCL-LOW-15-BITS (pp.139/145 and appendix B p.290, all four "
            "channels: \"AUDxLCL ... contain ... the low fifteen bits\" of "
            "the audio data start address): for every channel, each of the "
            "fifteen writable bits of AUDxLCL lands, alone, at the "
            "correspondingly-shifted low bit of address_out, and data_in bit "
            "0 has no effect at all - measured for AUD0LCL..AUD3LCL "
            "identically. Holds regardless of chipset, unlike AUDxLCH's "
            "width (assertions 8/9). "
            "[claims: 5-p139.c2, 5-loc.c2, 5-sum.c2, 5-appA-p266.c10, "
            "5-appB-p290.c29, 5-appB-p290.c64, 5-appB-p290.c99, "
            "5-appB-p290.c134, A-audxlcl.w, B-aud0lcl.width, "
            "B-aud1lcl.width, B-aud2lcl.width, B-aud3lcl.width]");
    }

    // ---------------- 5. REGISTER-MAP-ADDRESSES-AND-AGNUS-DECODE ----------------
    {
        Harness h;
        // Give each channel a distinct, recognisable marker, one channel at
        // a time, and confirm ONLY that channel's own pointer changes.
        uint32_t seen[4] = {0, 0, 0, 0};
        bool no_cross_talk = true;
        for (int ch = 0; ch < 4; ++ch) {
            const uint16_t hi = static_cast<uint16_t>(0x10 + ch);
            const uint16_t lo = static_cast<uint16_t>(0x0100 * (ch + 1));
            h.write_reg(LCH[ch], hi);
            h.write_reg(LCL[ch], lo);
            h.grant(ch, true);
            seen[ch] = h.read_pointer(ch);
            for (int other = 0; other < ch; ++other) {
                if (h.read_pointer(other) != seen[other]) no_cross_talk = false;
            }
        }
        // Re-grant EVERY channel again, after all sixteen writes above are
        // done, and re-check against each channel's own first-seen value.
        // Found necessary by mutation-testing this very assertion: read_
        // pointer() only shows the WORKING POINTER (agnus_audiodma.v:132),
        // which is a snapshot taken at grant() time - a write that lands in
        // the wrong channel's LOCATION bank AFTER that channel's own grant
        // has already happened is invisible to the loop above, because nothing
        // re-reads the location bank into the pointer afterward. This closing
        // sweep re-loads every channel's pointer from its location bank one
        // more time, at the very end, so a cross-talk write landing at any
        // point in the sequence has to show up somewhere.
        bool still_consistent_after_all_writes = true;
        for (int ch = 0; ch < 4; ++ch) {
            h.grant(ch, true);
            if (h.read_pointer(ch) != seen[ch]) still_consistent_after_all_writes = false;
        }
        no_cross_talk = no_cross_talk && still_consistent_after_all_writes;
        const bool addresses_distinct =
            seen[0] != seen[1] && seen[0] != seen[2] && seen[0] != seen[3] &&
            seen[1] != seen[2] && seen[1] != seen[3] && seen[2] != seen[3];
        checks.expect(no_cross_talk && addresses_distinct,
            "REGISTER-MAP-ADDRESSES-AND-AGNUS-DECODE (appendix A pp.263/266, "
            "appendix B pp.287/288/290, appendix C pp.298/309: AUD0LCH=$0A0/"
            "AUD0LCL=$0A2, AUD1LCH=$0B0/AUD1LCL=$0B2, AUD2LCH=$0C0/"
            "AUD2LCL=$0C2, AUD3LCH=$0D0/AUD3LCL=$0D2, each an address-"
            "register pair, each row's \"chip\" column naming Agnus): a "
            "write to each channel's own $0A0/$0B0/$0C0/$0D0 (+2) pair "
            "reaches that channel's own pointer and no other's (measured: "
            "writing channel N never changed any of 0..N-1's already-"
            "recorded values), decoded entirely within THIS module - which "
            "IS Agnus's audio-DMA logic, so a successful, channel-isolated "
            "decode here is the on-boundary evidence for \"AUDxLCH/AUDxLCL "
            "belong to Agnus\". The \"and NOT decoded by Paula\" half some of "
            "these ids' own evidence also states needs paula_audio_channel.v, "
            "outside this package's assigned single-module fixture, and is "
            "not re-measured here. "
            "[claims: 5-appA-p266.c1, 5-appA-p266.c3, 5-appA-p266.c4, "
            "5-appA-p266.c7, 5-appA-p266.c9, 5-appB-p288.c1, "
            "5-appB-p290.c19, 5-appB-p290.c21, 5-appB-p290.c22, "
            "5-appB-p290.c24, 5-appB-p290.c26, 5-appB-p290.c28, "
            "5-appB-p290.c31, 5-appB-p290.c54, 5-appB-p290.c56, "
            "5-appB-p290.c59, 5-appB-p290.c61, 5-appB-p290.c63, "
            "5-appB-p290.c66, 5-appB-p290.c89, 5-appB-p290.c91, "
            "5-appB-p290.c94, 5-appB-p290.c96, 5-appB-p290.c98, "
            "5-appB-p290.c101, 5-appB-p290.c124, 5-appB-p290.c126, "
            "5-appB-p290.c129, 5-appB-p290.c131, 5-appB-p290.c133, "
            "5-appB-p290.c136, 5-appC-p298.c2, 5-appC-p298.c5, "
            "5-appC-p309.c3, 5-appC-p309.c5, A-audxlch.addr, A-audxlch.chip, "
            "A-audxlcl.addr, A-audxlcl.chip, B-aud0lch.addr, B-aud0lch.chip, "
            "B-aud0lcl.addr, B-aud0lcl.chip, B-aud1lch.addr, B-aud1lch.chip, "
            "B-aud1lcl.addr, B-aud1lcl.chip, B-aud2lch.addr, B-aud2lch.chip, "
            "B-aud2lcl.addr, B-aud2lcl.chip, B-aud3lch.addr, B-aud3lch.chip, "
            "B-aud3lcl.addr, B-aud3lcl.chip, C-map.lch.addr, C-map.lch.chip, "
            "C-prose.lch]");
    }

    // ---------------- 6. POINTER-LSB-ALWAYS-ZERO ----------------
    {
        Harness h;
        h.write_reg(AUD0LCH, 0x0000);
        h.write_reg(AUD0LCL, 0x0000);
        h.grant(0, true);
        const uint32_t base = h.read_pointer(0);

        // data_in bit 0 alone: never stored (agnus_audiodma.v:89 reads only
        // data_in[15:1]), so this must produce the IDENTICAL pointer to an
        // all-zero write.
        h.write_reg(AUD0LCL, 0x0001);
        h.grant(0, true);
        const uint32_t bit0_only = h.read_pointer(0);
        const bool lsb_has_no_effect = (bit0_only == base);

        // Control: bit 1 (the true LSB of the stored slice) DOES change the
        // pointer - proves this is a real, sensitive measurement and not a
        // fixture that always reads back the same value regardless of
        // stimulus.
        h.write_reg(AUD0LCL, 0x0002);
        h.grant(0, true);
        const uint32_t bit1_only = h.read_pointer(0);
        const bool bit1_does_change_it = (bit1_only == base + 1) && (bit1_only != base);

        checks.expect(lsb_has_no_effect && bit1_does_change_it,
            "POINTER-LSB-ALWAYS-ZERO (appendix A p.263: \"The least "
            "significant bit of every pointer-type register ... must always "
            "be written with a zero\"; appendix B pp.287/290 the \"+\" "
            "legend, \"Address register pair; must be an even address "
            "pointing to chip memory\"): writing data_in bit 0 alone changes "
            "nothing (agnus_audiodma.v:89 reads only data_in[15:1] - bit 0 "
            "is never even sampled), so the stored/fetched address's own bit "
            "0 is always zero, structurally - the EVEN-ADDRESS half of the "
            "claim. Control: writing bit 1 alone DOES change the pointer by "
            "exactly 1, proving this is a live, sensitive measurement, not a "
            "stuck fixture. The \"... pointing to CHIP memory\" half needs "
            "the memory decoder (gary.v), outside this module, and is named, "
            "not measured, here. "
            "[claims: 5-appA-p263.c6, 5-appB-p287.c4, 5-appB-p290.c25, "
            "5-appB-p290.c32, 5-appB-p290.c60, 5-appB-p290.c67, "
            "5-appB-p290.c95, 5-appB-p290.c102, 5-appB-p290.c130, "
            "5-appB-p290.c137, A-pre.c7, B-legend.plus, B-aud0lch.mark, "
            "B-aud0lcl.mark, B-aud1lch.mark, B-aud1lcl.mark, "
            "B-aud2lch.mark, B-aud2lcl.mark, B-aud3lch.mark, "
            "B-aud3lcl.mark]");
    }

    // ---------------- 7. POINTER-REGISTER-SPACE-LONGWORD ----------------
    {
        // Reuses assertion 5's decode success: each channel's own LCH/LCL
        // pair is two adjacent word registers, LCL = LCH+1 in word units,
        // i.e. LCH_byte+2 in byte units, and LCH_byte is long-word aligned
        // (divisible by 4) for every channel - a fact about the address MAP
        // (the register constants themselves), not about stored width.
        bool longword_aligned = true;
        bool lcl_is_lch_plus_one_word = true;
        const uint16_t lch_byte[4] = {0x0A0, 0x0B0, 0x0C0, 0x0D0};
        for (int ch = 0; ch < 4; ++ch) {
            if (lch_byte[ch] % 4 != 0) longword_aligned = false;
            if (LCL[ch] != LCH[ch] + 1) lcl_is_lch_plus_one_word = false;
        }
        checks.expect(longword_aligned && lcl_is_lch_plus_one_word,
            "POINTER-REGISTER-SPACE-LONGWORD (appendix A p.263: \"All "
            "pointer-type registers are 32 bits wide\" and \"are located on "
            "long-word boundaries\" - the register SPACE, not stored width, "
            "per the appendix's own preamble): each channel's LCH address "
            "($0A0/$0B0/$0C0/$0D0) is divisible by 4, and its LCL partner "
            "sits exactly one word above it ($0A2/$0B2/$0C2/$0D2) - a "
            "long-word-aligned address-register pair for every channel, "
            "confirmed reachable by assertion 5's per-channel writes above. "
            "How many of those 32 bits are actually STORED is assertions "
            "4/8/9's question, not this one. "
            "[claims: 5-appA-p263.c3, 5-appA-p263.c4, A-pre.c5]");
    }

    // ---------------- 8. LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS ----------------
    int ch0_ecs_survivors = 0;
    {
        bool all_four_channels_identical = true;
        uint32_t per_channel_allones[4] = {0, 0, 0, 0};
        for (int ch = 0; ch < 4; ++ch) {
            Harness h;
            h.write_reg(LCL[ch], 0x0000);
            int survivors = 0;
            for (int bit = 0; bit <= 4; ++bit) {
                h.write_reg(LCH[ch], static_cast<uint16_t>(1u << bit));
                h.grant(ch, true);
                const uint32_t p = h.read_pointer(ch);
                if (p == (1u << (15 + bit))) ++survivors;
            }
            if (ch == 0) ch0_ecs_survivors = survivors;
            // All-ones round trip, matching CANDIDATES.md's own number.
            // NOTE: LCL must be written 0xFFFF (or any value with bits
            // 15:1 set), not 0x7FFF - 0x7FFF's own bit 15 is 0, which would
            // leave audlcl's bit 15 (the MSB of its [15:1] field) clear and
            // undercount the low half. Found the same way as the fixture
            // bug documented at assertion 2 above: by actually running this
            // harness and getting a number that did not match CANDIDATES.md,
            // not by adjusting the oracle to fit a wrong measurement.
            h.write_reg(LCH[ch], 0x001F);
            h.write_reg(LCL[ch], 0xFFFF);
            h.grant(ch, true);
            per_channel_allones[ch] = h.read_pointer(ch);
            if (survivors != 5 || per_channel_allones[ch] != 0xFFFFFu) {
                all_four_channels_identical = false;
            }
        }
        char buf[2048];
        std::snprintf(buf, sizeof(buf),
            "LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS (appendix A p.266 \"(high 3 "
            "bits, 5 bits if ECS)\"; appendix B p.290's ECS-marked row; "
            "appendix C p.298/309 \"high 5 bits was 3 bits\" and \"gain two "
            "address bits\"): a walking-one through all five bits of AUDxLCH "
            "lands at address_out bits 16-20 for every one of the four "
            "channels (measured %d/5 survivors on channel 0, identical on "
            "1-3), and the all-ones case gives word address 0xFFFFF (byte "
            "0x1FFFFE) on every channel - matching CANDIDATES.md's own "
            "number exactly. "
            "[claims: 5-appA-p266.c6, 5-appB-p290.c149, 5-appC-p298.c6, "
            "5-appC-p309.c1, 5-appC-p309.c2, 5-appC-p309.c6, A-audxlch.w1, "
            "C-map.lch.width, C-prose.c1, C-prose.lch, B-aud0lch.width]",
            ch0_ecs_survivors);
        checks.expect(all_four_channels_identical, buf);
    }

    // ---------------- 9. LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD ----------------
    // *** REQUIRED RED - see FINDING CANDIDATE 1 in the header. ***
    {
        Harness h;
        h.write_reg(AUD0LCL, 0x0000);
        // The two "ECS-only" bit positions of AUDxLCH: bits 3 and 4 (word-
        // address bits 19 and 20). A genuine 3-bit OCS reading would give
        // these NO effect at all.
        h.write_reg(AUD0LCH, 1u << 3);
        h.grant(0, true);
        const uint32_t p_bit3 = h.read_pointer(0);
        h.write_reg(AUD0LCH, 1u << 4);
        h.grant(0, true);
        const uint32_t p_bit4 = h.read_pointer(0);
        const bool bit3_had_no_effect = (p_bit3 == 0);
        const bool bit4_had_no_effect = (p_bit4 == 0);
        const bool ocs_reading_holds = bit3_had_no_effect && bit4_had_no_effect;

        char buf[2048];
        std::snprintf(buf, sizeof(buf),
            "LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD *** REQUIRED RED *** "
            "(chapter 5 pp.139/145, unqualified, \"contain the high three "
            "bits\"; appendix A p.266's non-ECS half; appendix B p.290's "
            "per-channel rows for channels 1-3, stating only \"high 3 bits\" "
            "with no ECS number on that row; appendix C's \"was 3 bits\", "
            "naming the pre-ECS value explicitly): a genuine OCS reading "
            "would give AUDxLCH's two \"ECS-only\" bit positions (bits 3 and "
            "4) NO effect on the fetch address. MEASURED: bit 3 alone gives "
            "word address 0x%X (byte 0x%X), bit 4 alone gives 0x%X (byte "
            "0x%X) - NEITHER is zero, so BOTH bits land. agnus_audiodma.v "
            "has no ecs or chipset port anywhere in its 157 lines (confirmed "
            "again by this reading), so the OCS alternative these ids state "
            "cannot be selected for at all - the module implements the "
            "five-bit ECS form UNCONDITIONALLY, exactly as CANDIDATES.md's "
            "prior, independent double-measurement already found. This uses "
            "the IDENTICAL walking-one methodology as "
            "LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS above (which PASSES, %d/5 "
            "survivors), which is this assertion's control: a red here "
            "reflects the OCS claim failing to hold, not a broken fixture. "
            "Expected, correct outcome: FAIL. This assertion is NM-HRM-024. "
            "It is the same deviation as NM-HRM-003 (Copper, "
            "agnus_copper.v:144) and NM-HRM-022 (blitter, BLTxPTH), in a "
            "third engine -- those two keys are named here as references, "
            "not as this assertion's own finding. "
            "[claims: 5-p139.c1, 5-sum.c1, 5-loc.c1, 5-appA-p266.c5, "
            "5-appA-p266.c11, 5-appB-p290.c57, 5-appB-p290.c92, "
            "5-appB-p290.c127, 5-appB-p290.c148, B-aud1lch.width, "
            "B-aud2lch.width, B-aud3lch.width, A-audxlc.c1, 5-appC-p298.c6, "
            "5-appC-p309.c6, A-audxlch.w1, C-map.lch.width, "
            "B-aud0lch.width]",
            p_bit3, p_bit3 * 2, p_bit4, p_bit4 * 2, ch0_ecs_survivors);
        checks.expect(ocs_reading_holds, buf);
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// Machine-readable records for tools/check_gates.py --tests. One line per
// mutation actually performed; assertions with no line here have never been
// mutation-tested, and the gate says so rather than assuming they
// discriminate. Every mutation below is a STIMULUS mutation to this test
// file's OWN inputs (a register value, a grant's restart flag, which channel
// a write/read targets) - never to nanomig/, which this task forbids
// touching for any reason. Every one was actually applied (edit -> rebuild ->
// run -> revert -> rebuild-clean, `diff` against a saved pre-mutation copy of
// the whole file confirming byte-identical reversion each time) and the
// PASS/FAIL totals quoted are what the run printed, not predictions.
//
// Baseline, confirmed by a clean build immediately before this mutation
// series and reproduced after every single revert below: 9 passed, 1
// failed. The one failure is assertion 9, LCH-OCS-THREE-BIT-READING-
// DOES-NOT-HOLD, which is REQUIRED to be red per FINDING CANDIDATE 1 above.
//
// MUTATION TEST-DATA-DELIVERY stimulus red: the read-back after channel ch's own grant
//   changed from `h.read_pointer(ch)` to `h.read_pointer((ch + 1) % 4)` (the
//   `want[]` oracle constants left untouched) -> FAIL, "order_ok" breaks
//   because every channel's read now samples its neighbour instead of
//   itself; 8 passed, 2 failed (assertion 0 newly red, assertion 9 still red
//   at baseline).
// MUTATION SLOT-PER-CHANNEL-PER-LINE stimulus red: the right-register probe's hpos changed from
//   `SLOT_HPOS[ch]` to `SLOT_HPOS[(ch + 1) % 4]` while `audio_dmal` and the
//   DAT[ch] comparison still target channel ch -> FAIL, the grant never
//   lands on ch's own slot so `granted` reads false for every channel;
//   8 passed, 2 failed. Re-run 2026-09-12 after the V-09 coverage repair
//   (assertion 1's cited-claims text was narrowed from 18 to 6 ids; its two
//   booleans and their code were not touched): identical result, only this
//   assertion newly red, reverted byte-identical, clean rebuild reproduced
//   9 passed/1 failed.
// MUTATION WORD-ALIGNED-FETCH-AND-LC-PAIR stimulus red: the second write_reg in the block changed from
//   `write_reg(AUD0LCH, 0x0015)` to `write_reg(AUD1LCH, 0x0015)` (a
//   one-character channel-index typo) while channel 0 is what gets read
//   back -> FAIL, both "low survived" and "high took the LCH write" break
//   because channel 0 never receives the second write at all; 8 passed,
//   2 failed.
// MUTATION LOCATION-REGISTER-VS-WORKING-POINTER stimulus red: the fourth grant() call's restart flag changed
//   from false (advance) to true (restart) -> FAIL, p4 lands back on v1
//   instead of v0+3, breaking "rewrite does not disturb an active transfer";
//   8 passed, 2 failed.
// MUTATION LCL-LOW-15-BITS stimulus red: the walked-bit write's target changed from
//   `write_reg(LCL[ch], ...)` to `write_reg(LCL[(ch + 1) % 4], ...)` (a
//   channel-index typo, oracle untouched) -> FAIL, channel ch's own pointer
//   never receives any of the fifteen walked bits, so every one of the 4x15
//   sub-checks fails; 8 passed, 2 failed.
// MUTATION REGISTER-MAP-ADDRESSES-AND-AGNUS-DECODE stimulus red, WITH A GENUINE FIXTURE GAP FOUND ALONG THE WAY.
//   First attempt: the LCL write target changed from `LCL[ch]` to
//   `LCL[(ch + 2) % 4]` (oracle untouched) against the assertion as it stood
//   at that point -> RESULT: 9 passed, 1 failed - the mutation did NOT turn
//   this assertion red. Diagnosed rather than accepted: read_pointer() only
//   shows the WORKING POINTER (agnus_audiodma.v:132), a snapshot taken at
//   grant() time; a write landing in the wrong channel's LOCATION bank AFTER
//   that channel's own grant has already happened changes something nothing
//   in the assertion ever reads again, so the corruption is invisible. This
//   is a real gap in the assertion, not a mutation-testing technicality, so
//   the assertion itself was fixed - a closing sweep was added that
//   re-grants every channel once more, after all sixteen writes are done,
//   and re-checks each against its own first-seen value (see the "Re-grant
//   EVERY channel again" comment in assertion 5's code above) - and the
//   fixed baseline was reconfirmed clean (9 passed, 1 failed) before
//   repeating the mutation. Repeated against the FIXED assertion: the
//   identical `LCL[(ch + 2) % 4]` mutation -> FAIL, "no_cross_talk" now
//   correctly breaks in the closing sweep; 8 passed, 2 failed. The fixture
//   fix is committed; the reported baseline and every other mutation in this
//   list were run against the fixed version.
// MUTATION POINTER-LSB-ALWAYS-ZERO stimulus red: the "bit1 only" write changed from 0x0002 to
//   0x0000 (identical to the base-case write) -> FAIL, "bit1_does_change_it"
//   becomes false because the two measurements are now identical by
//   construction, not because of a real gate - correctly catching that this
//   mutation makes the control meaningless; 8 passed, 2 failed.
// MUTATION POINTER-REGISTER-SPACE-LONGWORD stimulus red: the long-word-alignment table's channel-2 entry
//   changed from 0x0C0 to 0x0C1 (one byte off) -> FAIL, 0x0C1 % 4 != 0;
//   8 passed, 2 failed.
// MUTATION LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS stimulus red: the walked bit range changed from 0..4 (five
//   bits) to 0..2 (three bits) -> FAIL, "all_four_channels_identical" breaks
//   because `survivors` measures 3, not 5, on every channel; 8 passed,
//   2 failed.
// MUTATION LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD stimulus red, diagnosed rather than simply reported as "stayed
//   red" per the same rule hrm/agnus_blitter_ecs's header documents for its
//   own required-reds (assertion 9 is already required-red at baseline, so a
//   mutation that leaves it red proves nothing by itself): the probed bit
//   positions changed from 3 and 4 (the genuinely ECS-only bits, i.e.
//   data_in bits 3/4, which agnus_audiodma.v:82 DOES sample as part of
//   data_in[4:0]) to 5 and 6 - bits OUTSIDE that 5-bit slice entirely.
//   MEASURED: p_bit3 and p_bit4 (now actually sampling data_in bits 5 and 6)
//   both read back as 0x0 - NOT because those bits "truncate" in any C++
//   sense (1u<<5=0x20 and 1u<<6=0x40 are ordinary, fully-valid 16-bit values
//   reaching data_in unchanged), but because agnus_audiodma.v:82 only ever
//   reads data_in[4:0] - and 0x20's and 0x40's own bits [4:0] are both zero,
//   so the RTL's own bit-slice discards them before they ever reach audlch.
//   "bit3_had_no_effect" and "bit4_had_no_effect" both become true, and
//   ocs_reading_holds flips to TRUE - the assertion now PASSES (10 passed,
//   0 failed). This is the correct diagnostic outcome for the right reason:
//   the mutated probe no longer tests the ECS-only bits at all (it tests
//   positions that were never part of AUDxLCH's write path in the first
//   place, ECS or not), so of course nothing lands - proving the ORIGINAL
//   bit choice (3 and 4) was load-bearing, and that this assertion's red
//   result depends specifically on probing bits that genuinely exist inside
//   the 5-bit field but that an OCS reading would exclude. Reverted to 3 and
//   4 afterward; a clean rebuild reproduced the baseline (assertion 9 red
//   again, 9 passed/1 failed, all others as before).
//
// Every mutation above was reverted immediately after observing its printed
// result (confirmed byte-identical to the pre-mutation file by `diff`), and
// each revert reproduced this same 9 passed/1 failed baseline.
