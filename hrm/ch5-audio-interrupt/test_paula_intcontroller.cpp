// Test purpose: HRM conformance package for the audio side of Paula's
//   interrupt controller - the AUDxIP/AUDxIR signal pair (chapter 5, printed
//   p.165) and the INTENA/INTENAR/INTREQ/INTREQR register map and priority
//   encoder that those signals feed into (appendix A printed p.280; appendix
//   B printed pp.289-290). 106 testable claims, printed pages 165-290, from
//   work/ch5_packages/ch5-audio-interrupt.jsonl.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.165 (chapter 5 signal glossary); appendix A printed p.280; appendix B
//   printed pp.289-290, as quoted verbatim in the 106 claims named above.
// Fixture boundary: paula_intcontroller.v ONLY (BOUNDARIES.md row 6 - 176
//   lines, "AUDxIP / AUDxIR", ports audpen[3:0]/_ipl[2:0]/data_out[15:0] from
//   audint[3:0]). No other module is instantiated anywhere in this file.
//
// THIS PACKAGE EXTENDS hrm/paula_intcontroller, IT DOES NOT REPLACE IT.
//   hrm/paula_intcontroller/test_paula_intcontroller.cpp is HRM_CONFORMANCE
//   against chapter 7, printed pp.217-222 (FPGA-038/039/040) - a DIFFERENT
//   chapter's reading of the SAME module. That file is not opened, modified,
//   weakened, or duplicated by this one. Its three existing checks are:
//     FPGA-038: INTENA/INTREQ write + INTENAR/INTREQR readback, one clear.
//     FPGA-039: a VBLANK hardware pulse latches into INTREQ.
//     FPGA-040: _ipl inactive (7) with nothing enabled; VBLANK alone enabled
//       gives level 3; VBLANK+COPER both enabled+pending still gives level 3
//       (same level, not a priority contest); EXTERNAL alone gives level 6.
//   None of those four scenarios is repeated here. This package's own 106
//   claims are chapter 5's first reading of pp.165/280/289-290 and cover
//   ground FPGA-038/039/040 does not: the AUDxIP/AUDxIR signal identity
//   itself; the full 14-source bit/level table (chapter 7's package only
//   ever exercises VBLANK, COPER and EXTERNAL - AUD0-3, DSKSYN, RBF, BLIT,
//   PORTS, SOFT, DSKBLK and TBE are untouched by it); the write-only
//   address's constant read-back; bit 15 SET/CLR semantics as their own
//   assertion, including "both a set and a clear operation are required to
//   load arbitrary data"; the sticky/must-clear-by-write behaviour as its
//   own assertion; and the bit 14 INTEN/no-request-counterpart conflict with
//   the RTL, which chapter 7's package does not mention.
//
// A CONFLICT THIS PACKAGE'S TASK NAMED THAT ITS OWN 106 CLAIMS DO NOT
//   CONTAIN, REPORTED RATHER THAN PAPERED OVER. The task briefing for this
//   package describes a four-way conflict about the MOMENT the audio
//   interrupt is raised (printed p.147, p.157, p.164, appendix A p.265) and
//   a channel-boundary measurement ("first intreq two delivered words after
//   DMA start"). A search of this package's own claim file,
//   work/ch5_packages/ch5-audio-interrupt.jsonl (106 lines), for pages 147,
//   157, 164 or 265, and for the string "conflict", returns ZERO matches -
//   verified twice, once by page number and once by keyword, before any
//   code below was written. That conflict is CANDIDATES.md item 5 ("In
//   direct mode the interrupt is raised before a word is output"), measured
//   at paula_audio_channel.v:283-285 - a different module, assigned to a
//   different package (this chapter's channel/direct-output/register
//   packages, not this one). It is also, independently, outside what THIS
//   module's ports can see: paula_intcontroller.v's audint[3:0] input is the
//   channel's ALREADY-DECIDED interrupt pulse (AUDxIR, p.165) - this module
//   has no DMA-start signal, no delivered-word count and no channel state
//   anywhere on its port list, so there is no cadence to measure "two
//   delivered words after DMA start" against at this boundary even if a
//   claim asked for it. Per BRIEF.md's HALT-AND-REPORT conditions ("a claim
//   cannot be settled by the port list or by a measurement"; "the package
//   needs a wider module boundary than the one assigned"), this package
//   asserts nothing, measures nothing and records nothing about that
//   four-way conflict: there is no claim of its own to turn into an
//   assertion, and the fixture this package was assigned could not measure
//   it even if there were one. See the build report for the same statement
//   made to the lead directly.
//
// WHAT THIS PACKAGE RECORDS BUT DOES NOT ASSERT (per the task's own
//   instruction, and confirmed against the RTL below): appendix A p.280
//   gives bit 14 (INTEN) as "Master interrupt (enable only, no request)".
//   paula_intcontroller.v implements the ENABLE half exactly as printed
//   (assertion 5 below asserts it, and it is GREEN) but also implements a
//   REQUEST half for the same bit - `intreq[14] <= tmp[14]` - that the
//   manual's "no request" wording says should not exist. paula.v's own
//   comment on the wire that carries this into the module calls it the
//   "implementation of intreq[14] (Unreal needs it)", i.e. a deliberate
//   compatibility addition, not an oversight. Per the task's explicit
//   instruction this half is MEASURED, not asserted (see assertion 5's
//   MEASURED block) - claims 5-appA-p280.c20 and A-int.b14 are cited there,
//   not folded into a pass/fail check for the half that does not hold.
//
// Non-goals: anything about paula_audio_channel's own state machine or the
//   moment it decides to raise AUDxIR (separate packages' territory, and the
//   conflict discussed above); DSKSYNC/serial/disk pin behaviour beyond what
//   is needed to exercise this module's own priority encoder (those pins'
//   OWN semantics are other chapters' claims - only their bit position and
//   priority LEVEL are this chapter's claims, from the same appendix A
//   table that lists the audio bits); simultaneous same-level or
//   multi-level priority contests beyond the single-source-at-a-time
//   sweep assertion 6 performs (chapter 7's package already covers one
//   two-source-same-level case; this package's claims do not ask for more).
//
// ============================================================================
// SPECIFICATION - written from the 106 claims' manual quotations alone,
// before paula_intcontroller.v was opened. Nine assertions (0-8). Every one
// of the 106 claim ids appears in exactly the assertion(s) below whose
// proposition it states.
// ============================================================================
//
// 0. TEST-DATA-DELIVERY (fixture check, no claim id, no manual page).
//    Per the build brief's own silence-trap requirement: prove the fixture
//    can deliver KNOWN, DISTINCT values through both halves of this module's
//    register interface - a write/read round trip through INTENA/INTENAR,
//    and an audint[3:0] pulse arriving at audpen[3:0] - before trusting any
//    assertion below that depends on data or a pulse actually moving through
//    this module.
//
// 1. AUDIO-INTERRUPT-SIGNAL-ROUNDTRIP (4 claim ids).
//    Quotes: p.165 "AUDxIP  Audio interrupt pending (input to channel from
//    interrupt circuitry)."; "AUDxIR  Audio interrupt request (output from
//    channel to interrupt circuitry)".
//    Expected if NanoMig conforms: the channel's AUDxIR (this module's
//    audint[3:0] INPUT) is what causes a pending flag to appear; that flag
//    is delivered back to the channel as AUDxIP (this module's audpen[3:0]
//    OUTPUT) - i.e. audint in, audpen out, is exactly the request/pending
//    round trip the glossary describes, per channel, independently.
//    Claims: 5-p165.c2, 5-p165.c3, 5-sig.audxip, 5-sig.audxir .
//
// 2. REGISTER-MAP-ADDRESS-ACCESS-OWNER (43 claim ids).
//    Quotes: appendix A p.280 "INTENA 09A W P Interrupt enable bits (clear
//    or set bits)"; "INTENAR 01C R P Interrupt enable bits (read)"; "INTREQ
//    09C W P Interrupt request bits (clear or set)"; "INTREQR 01E R P
//    Interrupt request bits (read)"; "This register contains interrupt
//    request bits (or flags)."; appendix B pp.289-290, the same four
//    registers' summary rows repeating address and R/W.
//    Expected if NanoMig conforms: four distinct registers at four distinct
//    addresses - INTENA $09A (write), INTENAR $01C (read), INTREQ $09C
//    (write), INTREQR $01E (read) - all implemented by Paula (this module,
//    instantiated only inside paula.v), each write-address readable ONLY as
//    a fixed constant (never as stored content - the "write-only" half of
//    the claim, measured here as "presenting the WRITE address to the READ
//    path never exposes register content"), and each pair's read register
//    genuinely reflecting what was written to its write counterpart.
//    Claims: 5-appA-p280.c1, 5-appA-p280.c2, 5-appA-p280.c3, 5-appA-p280.c4,
//    5-appA-p280.c5, 5-appA-p280.c6, 5-appA-p280.c7, 5-appA-p280.c8,
//    5-appA-p280.c9, 5-appA-p280.c10, 5-appA-p280.c11, 5-appA-p280.c12,
//    5-appA-p280.c13, 5-appA-p280.c14, 5-appA-p280.c15, 5-appA-p280.c16,
//    5-appB-p289.c11, 5-appB-p289.c12, 5-appB-p289.c13, 5-appB-p289.c14,
//    5-appB-p289.c16, 5-appB-p289.c17, 5-appB-p289.c18, 5-appB-p289.c19,
//    5-appB-p290.c6, 5-appB-p290.c7, 5-appB-p290.c8, 5-appB-p290.c10,
//    5-appB-p290.c11, 5-appB-p290.c12, A-intena.addr, A-intena.acc,
//    A-intena.chip, A-intenar.addr, A-intenar.acc, A-intenar.chip,
//    A-intreq.addr, A-intreq.acc, A-intreq.chip, A-intreqr.addr,
//    A-intreqr.acc, A-intreqr.chip, A-intreq.c1 .
//
// 3. SHARED-BIT-LAYOUT-INTENA-INTREQ (4 claim ids).
//    Quotes: appendix A p.280 "This register contains interrupt enable
//    bits. The bit assignment for both the request and enable registers is
//    given below."; "The bit assignments are identical to the enable
//    register[.]" (an edition difference is recorded on two of these four
//    ids - the Developer CD corpus and the printed page differ in trailing
//    wording, "above register"/"identical to the enable register ." versus
//    "next register"/"...enable register BELOW" - the cross-read verdict on
//    both is that the restatement does not depend on the differing words,
//    so the proposition itself stands unaffected).
//    Expected if NanoMig conforms: whichever bit position gates a given
//    source in INTENA gates the identical source's request in INTREQ - one
//    shared bit numbering, not two independent ones.
//    Claims: 5-appA-p280.c17, 5-appA-p280.c40, A-intena.c1, A-intreq.c5 .
//
// 4. SET-CLR-BIT15-SEMANTICS (5 claim ids).
//    Quotes: appendix A p.280 "15 SET/CLR Set/clear control bit. Determines
//    if bits written with a 1 get set or cleared. Bits written with a zero
//    are always unchanged."; "Both a set and clear operation are required to
//    load arbitrary data into this register."
//    Expected if NanoMig conforms: a write with bit 15=1 SETS every other
//    bit written as 1 and leaves every bit written as 0 untouched; a write
//    with bit 15=0 CLEARS every other bit written as 1 and again leaves
//    zero-written bits untouched; consequently loading an arbitrary pattern
//    (one that both sets some bits and clears others relative to the current
//    content) takes two writes, not one.
//    Claims: 5-appA-p280.c18, 5-appA-p280.c19, 5-appA-p280.c37, A-int.b15,
//    A-intreq.c3 .
//
// 5. MASTER-ENABLE-BIT14-GATE (2 claim ids; ASSERTS ONLY THE ENABLE HALF).
//    Quote: appendix A p.280 "14 INTEN Master interrupt (enable only, no
//    request)".
//    Expected and ASSERTED: with every individual source enabled and
//    pending, _ipl stays inactive (7) while INTEN (bit 14 of INTENA) is
//    clear, and goes active the instant INTEN is set - a master gate over
//    every other enable bit.
//    NOT asserted (see the MEASURED block at this assertion's own site,
//    and the header note above): the second half of the same two claims,
//    "no request", which paula_intcontroller.v does not implement - a
//    genuine REQUEST bit exists at intreq[14] in the RTL. Recording this
//    rather than asserting it away is the task's explicit instruction.
//    Claims: 5-appA-p280.c20, A-int.b14 .
//
// 6. INTERRUPT-SOURCE-BIT-AND-PRIORITY-LEVEL-TABLE (42 claim ids).
//    Quotes: appendix A p.280's full bit table - bit 13 EXTER level 6; bit
//    12 DSKSYN level 5; bit 11 RBF level 5; bit 10 AUD3 level 4; bit 9 AUD2
//    level 4; bit 8 AUD1 level 4; bit 7 AUD0 level 4; bit 6 BLIT level 3;
//    bit 5 VERTB level 3; bit 4 COPER level 3; bit 3 PORTS level 2; bit 2
//    SOFT level 1; bit 1 DSKBLK level 1; bit 0 TBE level 1.
//    Expected if NanoMig conforms: for each of these 14 sources, in
//    isolation (that source enabled and pending, no other source enabled),
//    the pending bit appears at exactly its printed bit position in
//    INTREQR, and _ipl reports exactly its printed 68000 priority level
//    (encoded active-low, so level L reads back as 7-L); with nothing
//    enabled at all, _ipl is inactive (7) - the control case every one of
//    the fourteen per-source checks is measured against.
//    Claims: 5-appA-p280.c21, 5-appA-p280.c22, 5-appA-p280.c23,
//    5-appA-p280.c24, 5-appA-p280.c25, 5-appA-p280.c26, 5-appA-p280.c27,
//    5-appA-p280.c28, 5-appA-p280.c29, 5-appA-p280.c30, 5-appA-p280.c31,
//    5-appA-p280.c32, 5-appA-p280.c33, 5-appA-p280.c34, A-int.b13.bit,
//    A-int.b13.lvl, A-int.b12.bit, A-int.b12.lvl, A-int.b11.bit,
//    A-int.b11.lvl, A-int.b10.bit, A-int.b10.lvl, A-int.b09.bit,
//    A-int.b09.lvl, A-int.b08.bit, A-int.b08.lvl, A-int.b07.bit,
//    A-int.b07.lvl, A-int.b06.bit, A-int.b06.lvl, A-int.b05.bit,
//    A-int.b05.lvl, A-int.b04.bit, A-int.b04.lvl, A-int.b03.bit,
//    A-int.b03.lvl, A-int.b02.bit, A-int.b02.lvl, A-int.b01.bit,
//    A-int.b01.lvl, A-int.b00.bit, A-int.b00.lvl .
//
// 7. PENDING-BITS-POLLABLE-INDEPENDENT-OF-ENABLE (3 claim ids).
//    Quotes: appendix A p.280 "These bits may be polled by the processor; if
//    enabled by the bits listed in the [above/next] register, they may
//    cause processor interrupts." (edition difference recorded on two of
//    these three ids, same as assertion 3 - the restatement is unaffected).
//    Expected if NanoMig conforms: a source can be made pending (readable
//    via INTREQR) with its own INTENA bit left clear - polling still shows
//    it - while _ipl stays inactive until that bit's own enable is set; i.e.
//    "pollable" and "causes an interrupt" are genuinely two different
//    things in the RTL, not the same test worded twice.
//    Claims: 5-appA-p280.c35, 5-appA-p280.c36, A-intreq.c2 .
//
// 8. STICKY-UNTIL-EXPLICIT-CLEAR (3 claim ids).
//    Quotes: appendix A p.280 "These status bits are not automatically reset
//    when the interrupt is serviced, and must be reset when desired by
//    writing to this address."
//    Expected if NanoMig conforms: a pending bit, once set, stays set across
//    an arbitrary number of idle clocks with no other stimulus - nothing
//    "services" it automatically - and clears only on an explicit write to
//    INTREQ naming that bit with bit 15 (SET/CLR) clear.
//    Claims: 5-appA-p280.c38, 5-appA-p280.c39, A-intreq.c4 .
//
// ============================================================================
// RTL DISCLOSURE placeholder - paula_intcontroller.v had not been opened by
// this builder as of this commit. The register addresses, bit positions and
// priority levels named above come from the 106 claims' OWN classify.evidence
// fields (already part of this package's input, quoting exact RTL line
// numbers - the same allowance BOUNDARIES.md and CANDIDATES.md document for
// this whole chapter) and from BOUNDARIES.md's port table, not from this
// builder independently reading the module. The next commit opens the RTL to
// get the exact port list, register widths and internal signal names needed
// to WRITE the harness, and will disclose here what if anything that reading
// added beyond what the claims already gave, per this chapter's own stated
// rule: "what the rule protects is the expected value, not your ignorance."
// No expected value stated above is to change as a result of that reading.
// ============================================================================

#include <iostream>
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    std::cout << "SPECIFICATION-ONLY COMMIT: ch5-audio-interrupt harness not "
                 "yet implemented. See header for the nine assertions this "
                 "package will build.\n";
    return 1;
}
