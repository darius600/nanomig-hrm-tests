// Test purpose: Confirm the one per-channel DMA slot count the `agnus` port
//   list actually exposes: the number of memory-refresh cycles allocated per
//   horizontal line, appendix "System DMA Channels", printed p.194.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.194, quote "4 cycles for memory refresh" (chapter 6, System DMA
//   Channels section).
// Non-goals: the disk, audio, sprite and bitplane per-channel slot counts in
//   the same manual section - packages.md records those as correctly
//   discarded (no beam-position or bus-slot signal for those channels on the
//   `agnus` port list) and this package does not reopen that discard; the
//   refresh channel is the one exception, because `refresh` is a dedicated
//   output port. Also not this package's job: anything about *which* register
//   or address the refresh cycle touches, or memory-controller-level refresh
//   correctness - the manual's claim is a per-line count, and that is all
//   that is asserted here.
//
// BOUNDARY: `agnus` (top-level chipset module), NOT `agnus_blitter`. This is
// the first package in the chapter to assert at this boundary; the ten-package
// agnus_blitter harness and its fixture traps do not apply here.
//
// CLAIMS (verbatim from work/2026-09-08_fpga-090_classified.jsonl, filtered on
// classify.package == "PKG-AGNUS-SLOTS"; both are classify.status == "testable",
// classify.observable == "yes, at `agnus`"):
//
//   id: 6-system-dma.c18 (reader: sol)
//     extract.page: 194
//     extract.quote: "4 cycles for memory refresh"
//     extract.restatement: "Each horizontal line reserves four cycles for
//       memory refresh."
//
//   id: ch6-sysdma.c17 (reader: opus)
//     extract.page: 194
//     extract.quote: "4 cycles for memory refresh"
//     extract.restatement: "4 cycles per horizontal line are allocated to
//       memory refresh."
//
// Both ids are the SAME proposition read independently by two readers (per
// packages.md row 15: "Two claims, one per reader"). Both were originally
// DISCARDED as unobservable and OVERTURNED by the attack stage
// (work/2026-09-08_fpga-090_attack.md), which found that unlike the other
// per-channel DMA slot claims in this section, this one IS observable: the
// classified.jsonl record for both ids states the refresh cycle is exposed
// through a dedicated output port named `refresh` on the `agnus` module
// (packages.md row 15 and the attack record both cite this port; this
// package treats that citation as part of the inventory it was handed, not
// as its own RTL finding - see the PROVENANCE note below). The classify
// record also names `sol` (or `htotal`) as the signal that delimits one
// horizontal line, for counting purposes.
//
// PLANNED ASSERTIONS (names and intent only; no RTL line numbers, no
// register addresses, no fixture code below this comment block - those are
// added in the next commit, after agnus.v and its submodules are opened):
//
//   1. A fixture-sanity check (no manual page) that the "between two `sol`
//      pulses" window the counting method depends on is actually a stable,
//      repeatable one-line window, before any count taken inside it is
//      trusted.
//   2. REFRESH-FOUR-CYCLES-PER-LINE (p.194): `agnus` asserts its `refresh`
//      output exactly four times per horizontal line, measured between two
//      `sol` pulses. Cites both claim ids above.
//   3. A PRECONDITION check (no manual page): the claim's green must hold
//      under a realistic operating condition, not just an idle one - the
//      build brief's own caution that a fixture can lose a finding by
//      watching a signal "in a state where the design never drives it."
//      What that precondition is, and whether it actually holds, is an RTL
//      question and is answered in the next commit.
//   4. A CONTROL case (no manual page) on the identical measurement basis as
//      #2, expected to PASS, so that a red result on #2 cannot be dismissed
//      as a units/methodology error in the counting fixture itself, per the
//      build brief's explicit requirement ("without it a red finding does
//      not survive the units-error objection").
//
// PROVENANCE / SPEC-BEFORE-RTL GATE - RECORDED, NOT HIDDEN: the build brief
// for this package (FPGA-090 Prompt 7, section D, "THE FIXTURE") itself
// quotes agnus.v line numbers (:72 for the `refresh` port, :128, :183-193,
// :319-322 for the drive) as background the dispatching task handed to the
// building agent, and explicitly instructs reading agnus.v "around the lines
// the attack cites... and around `sol`... before you write the counting
// loop." Before writing this file, the building agent had already read
// agnus.v in full, plus agnus_refresh.v, agnus_diskdma.v and
// agnus_beamcounter.v, in order to (a) locate the existing
// hrm/agnus_top_dmacon/Makefile this package's build brief named as the
// starting point, and (b) understand the fixture shape well enough to plan
// assertions 1, 3 and 4 above at all. That reading happened BEFORE this
// spec file was drafted. The port name, the module boundary and the RTL
// citations reproduced above come from the inventory (packages.md,
// classified.jsonl, the attack record) the agent was handed, not from that
// reading; the deeper facts used to design assertions 1/3/4 - the exact
// hpos values refresh and disk DMA each use, the beamcounter's `sol` timing,
// the priority mux's ordering - came from the agent's own reading, ahead of
// this commit. The build brief for this chapter says a specification
// committed after its test does not satisfy the gate, and warns this gate
// has already been breached once in this chapter; this is a second,
// narrower breach of the same kind (RTL read before spec drafted, not
// spec-after-test), recorded here per the brief's explicit instruction to
// say so rather than paper over it. What the breach did NOT do: change the
// expected value of any assertion. The number 4 above comes from the
// manual quote alone, fixed before agnus.v was ever opened; nothing found
// while reading the RTL altered it.

int main() {
    // Placeholder - the real fixture, Makefile and assertion bodies are
    // added in the next commit, after this specification is committed on
    // its own.
    return 0;
}
