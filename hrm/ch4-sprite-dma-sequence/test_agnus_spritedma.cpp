// ch4-sprite-dma-sequence -- agnus_spritedma conformance test
//
// Test purpose: what the sprite DMA channel fetches and in what order -- the
//   two control words (SPRxPOS, SPRxCTL) read first, the two data words per
//   displayed line thereafter, the reload of a fresh control pair at the end
//   of a sprite's image data (including the zero-height edge case), the
//   channel's reuse several times within one display field and the minimum
//   separation and beam-position gating that reuse requires, that exactly
//   two DMA cycles are granted per active line, that the control words are
//   freshly refetched every display field, and the 9-bit VSTART/VSTOP split
//   across SPRxPOS/SPRxCTL -- all at the agnus_spritedma module boundary.
//   Package ch4-sprite-dma-sequence, fixture agnus_spritedma (one module,
//   builds standalone), 232 claims in claims.jsonl.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 97-98, 104-110, 113-114, 116-117, 124, 126-128 (ch4-sprite-dma-
//   sequence.jsonl, 232 records -- all 232 are accounted for below, either
//   cited inside an assertion or declined with a reason in CLAIM COVERAGE).
//   A red result below means agnus_spritedma.v disagrees with the manual,
//   not that the assertion is wrong -- no assertion here was, or should be,
//   adjusted to make the RTL pass.
// Non-goals: sprite pixel width (16 px) and "unlimited height" are not this
//   module's domain (pixel shaping is Denise's) and "unlimited" is not a
//   falsifiable property of a finite testbench either way -- declined, see
//   Dw. The existence of eight channels, the SPR0..SPR7 pointer-naming
//   convention, and "the other seven register sets are the same except for
//   the name" are architecture facts about a replicated instantiation this
//   package's single-instance fixture cannot observe -- declined, see Dc.
//   Sprite-data memory being word-addressed/contiguous/16-bit-wide is a RAM
//   organisation fact, not a behaviour of this module that could be right
//   or wrong -- declined, see Dm. SPRxPTL bit 0 being ignored and the
//   pointer advancing by one word per granted cycle would need a raw
//   memory-fetch address output; the only output ports this package uses
//   (reqdma, reg_address_out) are the ones the already-built sibling
//   harness hrm/agnus_spritedma/test_agnus_spritedma.cpp already exercises,
//   and neither is that address -- declined, see Dp. Pixels being produced
//   "automatically under DMA control" is Denise's rendering, not this
//   channel's fetch sequencing -- declined, see Dx. Colour content, the
//   ATTACH bit, collision and playfield priority are ch3-sprite-registers'
//   and denise_sprites' territory, not this package's.
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_spritedma.v, per the task's own
//   package assignment. The port names and register word-address map used
//   below (reqdma, ackdma, reg_address_out, reg_address_in, data_in, hpos,
//   vpos, vblend, vbl, clk7_en, clk, reset, aga, ecs; SPR0PTH=0x90,
//   SPR0PTL=0x91, SPR0POS=0xA0, SPR0CTL=0xA1, SPR0DATA=0xA2, SPR0DATB=0xA3)
//   are not read from agnus_spritedma.v by this pass -- see ORDER OF WORK.
//
// ORDER OF WORK: this file (assertions, the structural relationships they
//   check, the stimulus values, the register word-address map, and the
//   claim-id coverage ledger below) was written and committed from
//   ch4-sprite-dma-sequence.jsonl's quotations alone. agnus_spritedma.v was
//   not opened by this pass before that commit -- no port name, bit slice,
//   or RTL line number appears anywhere in this file. The register
//   word-address map, the clk7_en / tick / tick_hpos four-phase-per-hpos
//   clocking, and the "sample reqdma, then set ackdma and re-evaluate
//   before the clock edge that latches it" grant discipline are reused
//   verbatim from two already-committed, already-built sibling harnesses on
//   this same module and register family -- hrm/agnus_spritedma/
//   test_agnus_spritedma.cpp itself (reqdma/ackdma/reg_address_out and the
//   0x120/0x122/0x140/0x142 byte addresses) and hrm/ch3-sprite-registers/
//   test_denise_sprites.cpp (the SPR0POS/CTL/DATA/DATB word-address
//   constants, which agree with the first harness's byte addresses) -- not
//   learned by reading this package's own forbidden-until-committed RTL.
//   The posWord()/ctlWord() bit-packing formulas come only from this
//   package's own quoted bit tables (CLAIM COVERAGE bucket A9: p107, p128,
//   CH4-sprctl.*, CH4-sprpos.*, CH4-sctl.*, CH4-spos.*). Every numeric
//   prediction below (fetch order, per-line word count, height =
//   VSTOP-VSTART, the 9-bit split) follows from claims.jsonl's quotations
//   alone, not from the RTL.
//
//   AFTER the spec above was committed, agnus_spritedma.v was opened to
//   build this package (WSL/Verilator), which surfaced two harness bugs --
//   fixed here, with no assertion's expected value touched:
//   (1) the sibling-reused port set was complete and every port name
//   compiled clean on the first try, but this file's first draft presented
//   a granted cycle's word only on data_in, leaving reg_address_in at its
//   idle value -- so nothing ever actually landed in a register (every
//   assertion but the pure-ordering A1 read back empty). The RTL's
//   SPRxPOS/SPRxCTL write path (agnus_spritedma.v:205,220) is gated on
//   reg_address_in matching, not on ackdma -- a real shared CPU/DMA
//   register-write bus, exactly the shape hrm/ch3-sprite-registers/ already
//   documented for denise_sprites' own register writes. Fixed by mirroring
//   the granted reg_address_out back onto reg_address_in alongside data_in
//   (Harness::run()'s own comment). (2) that same read (agnus_spritedma.v:
//   286-296) showed reqdma is raised by "vblend || (sprvstop && ~vbl)", not
//   vblend alone, and that the module multiplexes all eight sprite channels
//   internally (register blocks SPR0.. through SPR7.., each requesting its
//   own control pair) -- see A7's own comment and the Dc CORRECTION above
//   for what that changed.
//
// MEASUREMENT BASIS: each assertion below builds a fresh Harness (fresh
//   DUT, fresh reset) and sweeps vpos over a line range and hpos over
//   0..453 (the sibling harness's own proven hpos bound) each line,
//   granting every reqdma the module raises and logging which register
//   word address (reg_address_out) it targets. Because the exact hpos and
//   absolute line number of any one grant is pipeline detail this package
//   was not told and does not need, every assertion below compares
//   *structural* relationships in that log (order, per-line count, which
//   lines are active, how many pairs and at what spacing) rather than
//   hardcoding an assumed absolute hpos or an assumed same-line-vs-next-
//   line edge timing -- the one exception being A6a, whose whole point is
//   that specific edge (see its own comment). This module exposes no raw
//   data-content readback port among the ones this package's sibling-
//   reused port set includes (see Dp in Non-goals), so the CONTROL case's
//   "known word arrives" proof is the register-address sequence itself:
//   two independent runs fed two *distinct* VSTART/VSTOP pairs must
//   produce two distinctly-positioned image-fetch windows, each exactly
//   matching its own fed values -- that is what a fed word "arriving
//   intact, in order, on the cycle it was fetched on" means for an
//   address-generator module with no data_out port, and it is what rules
//   out the chapter-6 failure mode (grant set inside the same eval() that
//   sampled the request, so every run looks identical regardless of input).
//
// CLAIM COVERAGE: all 232 records of ch4-sprite-dma-sequence.jsonl are
//   accounted for below -- cited in full inside an expect()/record() label,
//   or declined here with a reason. Ids restating the identical
//   proposition under a different page, table row, figure callout, or
//   reader are grouped and cited together on the single assertion that
//   tests that proposition, as hrm/ch3-sprite-registers/ does.
//
//   A1  CONTROL-PAIR-FETCHED-FIRST-POS-THEN-CTL (36 ids) -- CH4-build.c3,
//     CH4-build.c6, CH4-build.c7, CH4-det.c11, CH4-det.c12, CH4-det.c13,
//     CH4-fig4-7.c1, CH4-fig47.f1, CH4-fig47.f3, CH4-hw.c16, CH4-hw.c18,
//     CH4-p104.c2, CH4-p104.c4, CH4-p105.c1, CH4-p105.c2, CH4-p110.c1,
//     CH4-p110.c11, CH4-p110.c8, CH4-p124.c12, CH4-p126.c10, CH4-p126.c11,
//     CH4-p126.c12, CH4-p128.c6, CH4-ptr.c1, CH4-ptr.c12, CH4-ptr.c14,
//     CH4-sctl.c1, CH4-sctl.c2, CH4-spos.c3, CH4-spos.c4, CH4-sprpos.c1,
//     CH4-tab41.c2, CH4-tab41.c3, CH4-tab41.c4, CH4-tab41.c5, CH4-tab41.c6
//   A2  TWO-WORDS-PER-LINE-HEIGHT-EQUALS-VSTOP-MINUS-VSTART (43 ids) --
//     CH4-auto.c1, CH4-build.c4, CH4-det.c14, CH4-det.c16, CH4-det.c17,
//     CH4-det.c18, CH4-det.c20, CH4-det.c21, CH4-det.c22, CH4-det.c25,
//     CH4-det.c26, CH4-fig4-7.c2, CH4-fig4-7.c5, CH4-fig4-7.c6,
//     CH4-fig47.f8, CH4-mem.c2, CH4-mem.c3, CH4-p104.c3, CH4-p105.c10,
//     CH4-p105.c3, CH4-p105.c4, CH4-p105.c5, CH4-p105.c6, CH4-p105.c9,
//     CH4-p107.c12, CH4-p109.c1, CH4-p110.c12, CH4-p110.c9, CH4-p126.c13,
//     CH4-p126.c15, CH4-p126.c17, CH4-p126.c20, CH4-ptr.c15, CH4-ptr.c16,
//     CH4-ptr.c17, CH4-sprctl.c10, CH4-tab41.c11, CH4-tab41.c12,
//     CH4-tab41.c13, CH4-tab41.c14, CH4-tab41.c7, CH4-tab41.c8,
//     CH4-tab41.c9
//   A3  DATA/DATB fetch order [CONFLICT] (4 ids) -- CH4-det.c23,
//     CH4-det.c24, CH4-p126.c18, CH4-p126.c19. conflict_resolution: p126
//     ("The first word is stored in SPRxDATA and the second word in
//     SPRxDATB") states the fetch order as DATA-then-DATB; a separate,
//     on-record p123 register-write convention states the opposite write
//     order (SPRxDATB then SPRxDATA), and p107/p121 separately dispute
//     which word is the more-significant colour half -- none of that is
//     resolved by these four ids or by this package's own 232 claims. What
//     both sides agree on is asserted (A3u, below); the order itself is
//     measured and reported (A3-order), not asserted against either page.
//   A4a END-OF-DATA-PAIR-DECODED-AS-NEXT-CONTROL-PAIR (per-usage reload)
//     and A4b VSTOP-EQUALS-VSTART-PRODUCES-NO-OUTPUT (30 ids total) --
//     CH4-build.c10, CH4-det.c29, CH4-det.c30, CH4-det.c31, CH4-det.c32,
//     CH4-eod.c1, CH4-eod.c2, CH4-fig4-7.c7, CH4-fig47.f6, CH4-fig47.f7,
//     CH4-mem.c4, CH4-p104.c5, CH4-p105.c11, CH4-p105.c7, CH4-p108.c12,
//     CH4-p108.c13, CH4-p110.c10, CH4-p116.c3, CH4-p116.c6, CH4-p126.c23,
//     CH4-p126.c24, CH4-p126.c25, CH4-p126.c26, CH4-p127.c1, CH4-ptr.c13,
//     CH4-reuse.c5, CH4-reuse.c6, CH4-reuse.c9, CH4-tab41.c15,
//     CH4-tab41.c16
//   A5  CHANNEL-REUSED-MULTIPLE-TIMES-WITHIN-ONE-FIELD (18 ids) --
//     CH4-det.c15, CH4-fig410.f4, CH4-fig47.f5, CH4-intro.c1,
//     CH4-intro.c11, CH4-intro.c12, CH4-p116.c1, CH4-p116.c2, CH4-p116.c4,
//     CH4-p126.c14, CH4-p127.c7, CH4-p97.c10, CH4-p97.c11, CH4-ptr.c11,
//     CH4-ptrs.c2, CH4-reuse.c1, CH4-reuse.c4, CH4-reuse.c7
//   A6a ONE-LINE-GAP-IS-SUFFICIENT and A6b NEXT-USAGE-VSTART-BEHIND-BEAM-
//     NEVER-ACTIVATES (15 ids total) -- CH4-eod.c3, CH4-fig4-10.c2,
//     CH4-fig4-10.c3, CH4-fig410.f2, CH4-fig410.f3, CH4-fig4-9.c2,
//     CH4-fig49.f2, CH4-p108.c14, CH4-p116.c5, CH4-p117.c1, CH4-p117.c3,
//     CH4-reuse.c10, CH4-reuse.c11, CH4-reuse.c14, CH4-reuse.c8
//   A7  EXACTLY-TWO-DMA-GRANTS-PER-ACTIVE-LINE (3 ids) -- CH4-p117.c2,
//     CH4-reuse.c12, CH4-reuse.c13
//   A8  CONTROL-WORDS-FRESHLY-FETCHED-EACH-DISPLAY-FIELD (16 ids) --
//     CH4-move.c1, CH4-move.c2, CH4-move.c3, CH4-move.c4, CH4-p110.c13,
//     CH4-p110.c15, CH4-p110.c16, CH4-p113.c1, CH4-p113.c2, CH4-p113.c3,
//     CH4-p127.c8, CH4-ptr.c2, CH4-ptrs.c3, CH4-reset.c1, CH4-reset.c3,
//     CH4-reset.c4
//   A9  VSTART-VSTOP-ARE-9BIT-VALUES-SPLIT-ACROSS-SPRXPOS-AND-SPRXCTL (23
//     ids) -- CH4-det.c19, CH4-p107.c10, CH4-p107.c2, CH4-p107.c3,
//     CH4-p107.c5, CH4-p107.c6, CH4-p107.c9, CH4-p126.c16, CH4-p128.c15,
//     CH4-p128.c16, CH4-p128.c4, CH4-p128.c7, CH4-sctl.c14, CH4-sctl.c15,
//     CH4-sctl.c3, CH4-spos.c1, CH4-sprctl.c1, CH4-sprctl.c2,
//     CH4-sprctl.c4, CH4-sprctl.c7, CH4-sprctl.c8, CH4-sprpos.c3,
//     CH4-sprpos.c4
//
//   36+43+4+30+18+15+3+16+23 = 188 ids asserted or measured-and-reported.
//
//   Declined (44 ids, none independently testable at this module's
//   boundary with this package's known port set):
//   Dc EIGHT-CHANNEL-EXISTENCE-AND-NAMING (23 ids) -- CH4-intro.c6,
//     CH4-more.c2, CH4-more.c3, CH4-more.c6, CH4-more.c7, CH4-p110.c2,
//     CH4-p114.c3, CH4-p114.c4, CH4-p127.c4, CH4-p127.c6, CH4-p97.c3,
//     CH4-ptr.c3, CH4-ptr.c4, CH4-ptrs.c1, CH4-ptrs.c10, CH4-ptrs.c11,
//     CH4-ptrs.c5, CH4-ptrs.c6, CH4-ptrs.c7, CH4-ptrs.c8, CH4-ptrs.c9,
//     CH4-regs.c1, CH4-regs.c3 -- declined when this spec was written and
//     committed (a single-instance fixture, which is all a pre-RTL reading
//     of the claims gave any reason to expect, cannot observe "eight
//     channels exist"). CORRECTION, after the RTL was opened per ORDER OF
//     WORK: agnus_spritedma is in fact not single-channel -- it multiplexes
//     all eight sprite channels internally, at register blocks SPR0.. (word
//     addresses 0xA0-0xA3) through SPR7.. (0xBC-0xBF), each independently
//     requesting its own initial control-pair fetch (A1's own baseline log
//     shows exactly this: 16 grants on the first active line, eight
//     POS-then-CTL pairs). That makes CH4-more.c6/CH4-p114.c3 ("all eight
//     sprite pointers must be initialised") and the SPR0..SPR7 naming
//     convention structurally observable after all. No new assertion was
//     added for them here -- per the build brief's own gate, a claim's
//     testability is judged when the spec is written, and re-opening the
//     claim list after reading the RTL to add a newly-possible assertion is
//     exactly the RTL-shapes-the-spec ordering that gate forbids. Recorded
//     here, not acted on; the 23 ids remain uncited. "The other seven
//     register sets are the same except for the name" (CH4-p127.c6,
//     CH4-regs.c3) is not covered by this correction -- this package never
//     compared channel 1-7's own behaviour against channel 0's, so that
//     specific claim is still genuinely undecided by this file, correction
//     or not.
//   Dw SPRITE-PIXEL-WIDTH-AND-UNLIMITED-HEIGHT (4 ids) -- CH4-forming.c2,
//     CH4-intro.c8, CH4-p97.c5, CH4-p98.c2 -- pixel width is Denise's
//     domain, not this DMA channel's; "unlimited" is not falsifiable by any
//     finite sweep.
//   Dm MEMORY-ORGANISATION-FACTS (10 ids) -- CH4-build.c1, CH4-build.c2,
//     CH4-fig4-10.c1, CH4-fig410.f1, CH4-fig4-7.c4, CH4-fig47.f2,
//     CH4-mem.c1, CH4-p104.c1, CH4-p105.c8, CH4-tab41.c1 -- word-addressed,
//     contiguous, 16-bit-wide RAM and "increasing addresses" arrows are
//     properties of memory layout this module's known ports (which carry
//     no raw fetch address) cannot observe either way.
//   Dp POINTER-COMPOSITION-AND-WORD-ALIGNMENT (5 ids) -- CH4-det.c10,
//     CH4-p110.c5, CH4-p110.c6, CH4-ptr.c7, CH4-ptr.c8 -- SPRxPTL bit 0
//     being ignored and the pointer being composed of SPRxPTH/SPRxPTL would
//     need a raw RAM fetch-address output port; neither of the two output
//     ports this package's sibling-reused port set includes (reqdma,
//     reg_address_out) is that port, and opening the RTL to find such a
//     port's name is exactly the pre-spec RTL read the build brief's gate
//     forbids.
//   Dx PIXEL-OUTPUT-OUT-OF-BOUNDARY (2 ids) -- CH4-det.c34, CH4-p127.c3 --
//     "produced as pixels ... under DMA control" is Denise's rendering
//     output, not this channel's fetch sequencing.
//
//   188 + 44 = 232.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id, labelled
//   "[control]") proves the write(via data_in)->grant(via ackdma)->observe
//   (via reg_address_out) path this whole file depends on is alive, and
//   that two distinct fed VSTART/VSTOP pairs really do produce two
//   distinctly-positioned, correctly-bounded image-fetch windows -- before
//   any claim-based assertion below depends on that path. This is the
//   paragraph of the build brief that matters most for this package: a
//   harness that granted ackdma inside the same eval() that sampled
//   reqdma would report every data-dependent assertion below as a finding
//   about agnus_spritedma, when the defect would be the harness's own.
//
// MUTATION LOG: see the end of this file, filled in after this spec's own
//   commit, once agnus_spritedma.v was opened and the package actually
//   built and ran (per ORDER OF WORK -- do not read a result here as
//   predating that build).

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <verilated.h>
#include "Vagnus_spritedma.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For a measured-but-not-asserted record (the disputed half of a
    // conflict) or a fact about the SOURCE itself. Not a pass/fail.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Register word addresses, reused verbatim from two already-committed,
// already-built sibling harnesses on this same module/register family --
// see ORDER OF WORK above.
constexpr int SPR0PTH = 0x90;
constexpr int SPR0PTL = 0x91;
constexpr int SPR0POS = 0xA0;
constexpr int SPR0CTL = 0xA1;
constexpr int SPR0DATA = 0xA2;
constexpr int SPR0DATB = 0xA3;

// The sibling harness's own proven hpos sweep bound.
constexpr int HPOS_MAX = 454;

// Bit-packing derived only from this package's own quoted bit tables (bucket
// A9): SPRxPOS bits 15-8 hold VSTART bits 7-0; SPRxCTL bits 15-8 hold VSTOP
// bits 7-0, bit 2 holds VSTART bit 8, bit 1 holds VSTOP bit 8. HSTART is not
// this package's subject and is left 0.
inline uint16_t posWord(int vstart) { return (uint16_t)((vstart & 0xFF) << 8); }
inline uint16_t ctlWord(int vstart, int vstop) {
    return (uint16_t)(((vstop & 0xFF) << 8) | (((vstart >> 8) & 1) << 2) | (((vstop >> 8) & 1) << 1));
}

std::string toHex(int v) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "0x%X", v);
    return std::string(buf);
}

std::string regName(int reg) {
    switch (reg) {
        case SPR0POS: return "SPR0POS";
        case SPR0CTL: return "SPR0CTL";
        case SPR0DATA: return "SPR0DATA";
        case SPR0DATB: return "SPR0DATB";
        default: return toHex(reg);
    }
}

std::string join(const std::vector<int>& v) {
    std::string s = "[";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) s += ",";
        s += std::to_string(v[i]);
    }
    s += "]";
    return s;
}

struct Grant {
    int line;
    int hpos;
    int reg;
    uint16_t data_sent;
};

// One isolated measurement: fresh DUT, reset, base pointer setup (values
// reused from the sibling harness, see its own constructor comment below),
// then a caller-driven vpos/hpos sweep. Clocking and the request/grant
// discipline are reused from hrm/agnus_spritedma/test_agnus_spritedma.cpp,
// an already-committed, already-built sibling harness for this same
// module -- not learned from this package's own forbidden-until-committed
// RTL (see ORDER OF WORK).
class Harness {
public:
    Vagnus_spritedma* dut;

    Harness() : dut(new Vagnus_spritedma) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->aga = 0;
        dut->ecs = 0;
        dut->ackdma = 0;
        dut->vbl = 0;
        dut->vblend = 0;
        dut->hpos = 0;
        dut->vpos = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        tick();
        dut->reset = 0;

        // Base pointer setup, reused verbatim (word addresses and values)
        // from the sibling harness's own, already-proven-working setup.
        // This package's measurements never depend on the pointer's
        // actual value -- this module exposes no RAM fetch-address output
        // among the ports this package's port set includes (see Dp above)
        // to read it back from -- it is written only because the sibling
        // harness writes it before its own sweep ever sees a granted DMA
        // cycle.
        dut->reg_address_in = SPR0PTH;
        dut->data_in = 0;
        tick_hpos();
        dut->reg_address_in = SPR0PTL;
        dut->data_in = 0x2000;
        tick_hpos();
        dut->reg_address_in = 0;
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

    void tick_hpos() {
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        tick();
        tick();
        tick();
    }

    // Sweeps vpos over [firstLine,lastLine], hpos over 0..HPOS_MAX-1 each
    // line, granting every reqdma the module raises in the same phase the
    // sibling harness already grants it in: ackdma is set and the DUT
    // re-evaluated BEFORE the clock edge that latches it, never inside the
    // same eval() that merely sampled reqdma -- see the CONTROL case and
    // ORDER OF WORK for why that distinction is this package's first
    // requirement. vblend is asserted for exactly firstLine, unless
    // assertVblend is false. ctrlWords[i] supplies the (vstart,vstop) pair
    // presented for the i-th control-pair fetch this run encounters
    // (0-based, first-encountered-first); once exhausted, 0 is presented.
    // Every DATA/DATB grant gets a distinct, line-and-register-tagged
    // marker word whether or not the caller's assertion cares about it.
    std::vector<Grant> run(int firstLine, int lastLine,
                            const std::vector<std::pair<int, int>>& ctrlWords,
                            bool assertVblend = true) {
        std::vector<Grant> log;
        size_t ctrlIdx = 0;
        for (int line = firstLine; line <= lastLine; ++line) {
            dut->vpos = line;
            dut->vblend = (assertVblend && line == firstLine) ? 1 : 0;
            for (int hp = 0; hp < HPOS_MAX; ++hp) {
                dut->hpos = hp;
                dut->clk7_en = 1;
                dut->eval();
                if (dut->reqdma) {
                    dut->ackdma = 1;
                    dut->eval();
                    int reg = dut->reg_address_out;
                    uint16_t val;
                    if (reg == SPR0POS) {
                        val = ctrlIdx < ctrlWords.size() ? posWord(ctrlWords[ctrlIdx].first) : 0;
                    } else if (reg == SPR0CTL) {
                        val = ctrlIdx < ctrlWords.size()
                                  ? ctlWord(ctrlWords[ctrlIdx].first, ctrlWords[ctrlIdx].second)
                                  : 0;
                        if (ctrlIdx < ctrlWords.size()) ++ctrlIdx;
                    } else {
                        val = (uint16_t)(0xD000 | ((line & 0x3F) << 2) | (reg & 0x3));
                    }
                    // Mirror the granted request's own register address back onto
                    // reg_address_in alongside data_in: the module's SPRxPOS/SPRxCTL write
                    // path (confirmed once the RTL was opened, per ORDER OF WORK -- this is
                    // a harness fix, not an oracle change) is gated on reg_address_in
                    // matching, exactly like a real shared CPU/DMA register-write bus. A
                    // granted DMA cycle that presented data_in without also presenting the
                    // matching reg_address_in would be the chapter-6 failure mode this
                    // package's own CONTROL case exists to catch.
                    dut->reg_address_in = reg;
                    dut->data_in = val;
                    dut->eval();
                    log.push_back(Grant{line, hp, reg, val});
                } else {
                    dut->ackdma = 0;
                    dut->reg_address_in = 0;
                }
                tick();
                dut->clk7_en = 0;
                dut->reg_address_in = 0;
                tick();
                tick();
                tick();
            }
        }
        return log;
    }
};

std::map<int, std::vector<int>> byLine(const std::vector<Grant>& log) {
    std::map<int, std::vector<int>> m;
    for (auto& g : log) m[g.line].push_back(g.reg);
    return m;
}

bool isCtrlPair(const std::vector<int>& v) {
    return v.size() == 2 && v[0] == SPR0POS && v[1] == SPR0CTL;
}

bool isDataPair(const std::vector<int>& v) {
    return v.size() == 2 &&
           ((v[0] == SPR0DATA && v[1] == SPR0DATB) || (v[0] == SPR0DATB && v[1] == SPR0DATA));
}

std::vector<int> linesMatching(const std::map<int, std::vector<int>>& m, bool (*pred)(const std::vector<int>&)) {
    std::vector<int> out;
    for (auto& kv : m)
        if (pred(kv.second)) out.push_back(kv.first);
    return out;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;

    // ---- CONTROL: FIXTURE-LIVE-DATA-PATH ----
    {
        Harness hA;
        std::vector<std::pair<int, int>> pairsA = {{40, 44}, {0, 0}};
        auto logA = hA.run(0, 50, pairsA);

        Harness hB;
        std::vector<std::pair<int, int>> pairsB = {{12, 15}, {0, 0}};
#ifdef MUT_CTRL
        pairsB = pairsA;  // withhold the difference between the two runs
#endif
        auto logB = hB.run(0, 50, pairsB);

        auto mA = byLine(logA);
        auto mB = byLine(logB);
        auto dataLinesA = linesMatching(mA, isDataPair);
        auto dataLinesB = linesMatching(mB, isDataPair);

        c.note("Run A (VSTART=40,VSTOP=44) image-fetch lines: " + join(dataLinesA));
        c.note("Run B (VSTART=" + std::to_string(pairsB[0].first) + ",VSTOP=" +
                std::to_string(pairsB[0].second) + ") image-fetch lines: " + join(dataLinesB));

        bool windowAcorrect = dataLinesA == std::vector<int>{40, 41, 42, 43};
        bool windowBcorrect = dataLinesB == std::vector<int>{12, 13, 14};
        bool distinct = dataLinesA != dataLinesB;

        c.expect(windowAcorrect && windowBcorrect && distinct,
            "[control] FIXTURE-LIVE-DATA-PATH: two independent runs fed two distinct VSTART/VSTOP "
            "words through data_in, exactly at the cycle each control word was granted, each produce "
            "their own correctly-positioned image-fetch window (40-43 for VSTART=40/VSTOP=44, 12-14 "
            "for VSTART=12/VSTOP=15, never the same window) -- the fixture's write(data_in)->grant"
            "(ackdma)->observe(reg_address_out) path delivers a fed word intact, in order, on the "
            "cycle it was fetched on, and is not chapter 6's zero-data-path failure mode (ackdma set "
            "inside the same eval() that sampled reqdma).");
    }

    // ---- A1: CONTROL-PAIR-FETCHED-FIRST-POS-THEN-CTL ----
    {
        // Starts at line 5, not line 0: opening the RTL (see A7's own comment on when that
        // happened) showed reqdma is also raised whenever vpos already equals the reset value
        // of VSTOP (0), independent of vblend, so sweeping from line 0 cannot isolate vblend's
        // own contribution the way starting past that coincidence can.
        Harness h;
        bool useVblend = true;
#ifdef MUT_A1
        useVblend = false;  // withhold vblend: the channel is never told it may fetch
#endif
        std::vector<std::pair<int, int>> pairs = {{40, 44}};
        auto log = h.run(5, 15, pairs, useVblend);

        bool ok = log.size() >= 2 && log[0].reg == SPR0POS && log[1].reg == SPR0CTL &&
                  log[0].line == 5 && log[1].line == 5;
        c.note("First two DMA grants: " + std::to_string(log.size()) + " total; first=" +
               (log.empty() ? std::string("<none>") : regName(log[0].reg)) + ", second=" +
               (log.size() < 2 ? std::string("<none>") : regName(log[1].reg)));

        c.expect(ok,
            "A1-CONTROL-PAIR-FETCHED-FIRST-POS-THEN-CTL: on activation the channel's first two "
            "granted DMA cycles target SPRxPOS then SPRxCTL, in that order, before any image data is "
            "fetched (CH4-build.c3, CH4-build.c6, CH4-build.c7, CH4-det.c11, CH4-det.c12, CH4-det.c13, "
            "CH4-fig4-7.c1, CH4-fig47.f1, CH4-fig47.f3, CH4-hw.c16, CH4-hw.c18, CH4-p104.c2, "
            "CH4-p104.c4, CH4-p105.c1, CH4-p105.c2, CH4-p110.c1, CH4-p110.c11, CH4-p110.c8, "
            "CH4-p124.c12, CH4-p126.c10, CH4-p126.c11, CH4-p126.c12, CH4-p128.c6, CH4-ptr.c1, "
            "CH4-ptr.c12, CH4-ptr.c14, CH4-sctl.c1, CH4-sctl.c2, CH4-spos.c3, CH4-spos.c4, "
            "CH4-sprpos.c1, CH4-tab41.c2, CH4-tab41.c3, CH4-tab41.c4, CH4-tab41.c5, CH4-tab41.c6)");
    }

    // ---- A2: TWO-WORDS-PER-LINE-HEIGHT-EQUALS-VSTOP-MINUS-VSTART ----
    {
        Harness h;
        int vstop = 44;
#ifdef MUT_A2
        vstop = 45;
#endif
        std::vector<std::pair<int, int>> pairs = {{40, vstop}};
        auto log = h.run(0, 50, pairs);
        auto m = byLine(log);
        auto dataLines = linesMatching(m, isDataPair);

        c.note("Image-fetch lines for VSTART=40,VSTOP=" + std::to_string(vstop) + ": " + join(dataLines));
        bool ok = dataLines == std::vector<int>{40, 41, 42, 43};

        c.expect(ok,
            "A2-TWO-WORDS-PER-LINE-HEIGHT-EQUALS-VSTOP-MINUS-VSTART: for VSTART=40,VSTOP=44 exactly "
            "the four lines 40-43 (height = VSTOP-VSTART = 4) each receive exactly two DMA grants "
            "(the line's color-descriptor pair), and no line outside that window receives any "
            "(CH4-auto.c1, CH4-build.c4, CH4-det.c14, CH4-det.c16, CH4-det.c17, CH4-det.c18, "
            "CH4-det.c20, CH4-det.c21, CH4-det.c22, CH4-det.c25, CH4-det.c26, CH4-fig4-7.c2, "
            "CH4-fig4-7.c5, CH4-fig4-7.c6, CH4-fig47.f8, CH4-mem.c2, CH4-mem.c3, CH4-p104.c3, "
            "CH4-p105.c10, CH4-p105.c3, CH4-p105.c4, CH4-p105.c5, CH4-p105.c6, CH4-p105.c9, "
            "CH4-p107.c12, CH4-p109.c1, CH4-p110.c12, CH4-p110.c9, CH4-p126.c13, CH4-p126.c15, "
            "CH4-p126.c17, CH4-p126.c20, CH4-ptr.c15, CH4-ptr.c16, CH4-ptr.c17, CH4-sprctl.c10, "
            "CH4-tab41.c11, CH4-tab41.c12, CH4-tab41.c13, CH4-tab41.c14, CH4-tab41.c7, CH4-tab41.c8, "
            "CH4-tab41.c9)");
    }

    // ---- A3: SPRxDATA/SPRxDATB fetch order [CONFLICT] ----
    {
        Harness h;
        int vstart = 40, vstop = 44;
#ifdef MUT_A3U
        vstart = 100; vstop = 104;  // shift the window away from the lines this check inspects
#endif
        std::vector<std::pair<int, int>> pairs = {{vstart, vstop}};
        auto log = h.run(0, 110, pairs);
        auto m = byLine(log);

        bool allDistinct = isDataPair(m[40]) && isDataPair(m[41]) && isDataPair(m[42]) && isDataPair(m[43]);

        c.expect(allDistinct,
            "A3u-TWO-DISTINCT-REGISTERS-RECEIVE-THE-FETCHED-PAIR (undisputed sub-claim of a conflict; "
            "conflict_resolution: p126 states the fetch order as SPRxDATA-then-SPRxDATB, a separate "
            "on-record p123 register-write convention states the opposite write order, and p107/p121 "
            "separately dispute which word is the more-significant colour half -- CH4-det.c23, "
            "CH4-det.c24, CH4-p126.c18, CH4-p126.c19 do not settle which page is right, so only what "
            "all sides agree on is asserted here: for each displayed line, its two fetched words land "
            "in two distinct registers, SPRxDATA and SPRxDATB, one word each, never the same register "
            "twice) (CH4-det.c23, CH4-det.c24, CH4-p126.c18, CH4-p126.c19)");

        std::vector<std::string> orderPerLine;
        for (int line : {40, 41, 42, 43}) {
            auto& v = m[line];
            if (v.size() == 2) orderPerLine.push_back(v[0] == SPR0DATA ? "DATA-then-DATB" : "DATB-then-DATA");
        }
        bool uniform = !orderPerLine.empty() &&
                       std::all_of(orderPerLine.begin(), orderPerLine.end(),
                                   [&](const std::string& s) { return s == orderPerLine[0]; });
        c.record(
            "A3-order-DATA-VS-DATB-FETCH-ORDER measured, not asserted (conflict_resolution: p126 vs "
            "the on-record p123 convention disagree on which of SPRxDATA/SPRxDATB is fetched first; "
            "CH4-det.c23, CH4-det.c24, CH4-p126.c18, CH4-p126.c19 name only the p126 side, so no page "
            "is picked here): agnus_spritedma.v's reg_address_out sequence puts " +
            (orderPerLine.empty() ? std::string("<no data measured -- see note above>") : orderPerLine[0]) +
            " on every one of lines 40-43" + (uniform ? "" : " (NOT uniform across lines -- see raw log)") +
            ".");
    }

    // ---- A4a: END-OF-DATA-PAIR-DECODED-AS-NEXT-CONTROL-PAIR ----
    {
        Harness h;
        int vstopA = 44;
#ifdef MUT_A4A
        vstopA = 200;  // reload never observed within the swept window
#endif
        std::vector<std::pair<int, int>> pairs = {{40, vstopA}, {50, 53}};
        auto log = h.run(0, 55, pairs);
        auto m = byLine(log);
        auto ctrlLines = linesMatching(m, isCtrlPair);

        c.note("Control-pair lines: " + join(ctrlLines));
        bool ok = ctrlLines.size() >= 2 && ctrlLines[1] > 43;

        c.expect(ok,
            "A4a-END-OF-DATA-PAIR-DECODED-AS-NEXT-CONTROL-PAIR: once the last line of one usage's "
            "image data has been fetched, the very next word pair the channel requests again targets "
            "SPRxPOS then SPRxCTL -- the same two-register, same-order format as the initial control "
            "fetch -- rather than more image data (CH4-build.c10, CH4-det.c29, CH4-det.c30, "
            "CH4-det.c31, CH4-det.c32, CH4-eod.c1, CH4-eod.c2, CH4-fig4-7.c7, CH4-fig47.f6, "
            "CH4-fig47.f7, CH4-mem.c4, CH4-p104.c5, CH4-p105.c11, CH4-p105.c7, CH4-p108.c12, "
            "CH4-p108.c13, CH4-p110.c10, CH4-p116.c3, CH4-p116.c6, CH4-p126.c23, CH4-p126.c24, "
            "CH4-p126.c25, CH4-p126.c26, CH4-p127.c1, CH4-ptr.c13, CH4-reuse.c5, CH4-reuse.c6, "
            "CH4-reuse.c9, CH4-tab41.c15, CH4-tab41.c16)");
    }

    // ---- A4b: VSTOP-EQUALS-VSTART-PRODUCES-NO-IMAGE-OUTPUT-BUT-CHAIN-CONTINUES ----
    {
        Harness h;
        int vstop1 = 20;
#ifdef MUT_A4B
        vstop1 = 25;  // usage 1 no longer has zero height
#endif
        std::vector<std::pair<int, int>> pairs = {{20, vstop1}, {30, 33}};
        auto log = h.run(0, 40, pairs);
        auto m = byLine(log);
        auto dataLines = linesMatching(m, isDataPair);

        c.note("Image-fetch lines for a zero-height usage (VSTART=VSTOP=20) chained into a normal "
               "usage (VSTART=30,VSTOP=33): " + join(dataLines));
        bool noOutputForZeroHeight = std::none_of(dataLines.begin(), dataLines.end(), [](int l) { return l == 20; });
        bool chainContinues = dataLines == std::vector<int>{30, 31, 32};

        c.expect(noOutputForZeroHeight && chainContinues,
            "A4b-VSTOP-EQUALS-VSTART-PRODUCES-NO-IMAGE-OUTPUT-BUT-CHAIN-CONTINUES: a usage whose "
            "VSTOP equals its VSTART produces no image-fetch line of its own (\"no sprite output "
            "occurs\"), and the channel still correctly proceeds to the next usage's own control pair "
            "and image window rather than getting stuck (CH4-det.c29, CH4-det.c30, CH4-det.c31, "
            "CH4-p126.c23, CH4-p126.c24, CH4-p126.c25, CH4-p126.c26)");
    }

    // ---- A5: CHANNEL-REUSED-MULTIPLE-TIMES-WITHIN-ONE-FIELD ----
    {
        Harness h;
        int vstop2 = 53;
#ifdef MUT_A5
        vstop2 = 54;
#endif
        std::vector<std::pair<int, int>> pairs = {{40, 43}, {50, vstop2}, {60, 62}, {0, 0}};
        auto log = h.run(0, 70, pairs);
        auto m = byLine(log);
        auto dataLines = linesMatching(m, isDataPair);

        c.note("Image-fetch lines across three chained usages (40-42,50-52,60-61): " + join(dataLines));
        bool ok = dataLines == std::vector<int>{40, 41, 42, 50, 51, 52, 60, 61};

        c.expect(ok,
            "A5-CHANNEL-REUSED-MULTIPLE-TIMES-WITHIN-ONE-FIELD: a channel fed three chained usages "
            "(40-42, 50-52, 60-61, terminated by an all-zero pair) produces all three image windows, "
            "each independently, later in the same field, each strictly lower on-screen than the one "
            "before it (CH4-det.c15, CH4-fig410.f4, CH4-fig47.f5, CH4-intro.c1, CH4-intro.c11, "
            "CH4-intro.c12, CH4-p116.c1, CH4-p116.c2, CH4-p116.c4, CH4-p126.c14, CH4-p127.c7, "
            "CH4-p97.c10, CH4-p97.c11, CH4-ptr.c11, CH4-ptrs.c2, CH4-reuse.c1, CH4-reuse.c4, "
            "CH4-reuse.c7)");
    }

    // ---- A6a: ONE-LINE-GAP-IS-SUFFICIENT-FOR-NEXT-USAGE ----
    {
        // p117.c1: "the bottom line of one usage ... must be separated from the top line of the
        // next usage by at least one horizontal scan line." Usage 1's last displayed line is 43
        // (VSTOP=44); "at least one" scan-line separation makes 45 the earliest legal next-usage
        // VSTART (44 itself is the one intervening, undisplayed line p117.c3 says the channel
        // needs to fetch the next control pair in) -- this is the one assertion in this package
        // that DOES pin an exact line number, because that exact number is this claim's own
        // subject.
        Harness h;
        int vstop2 = 48;
#ifdef MUT_A6A
        vstop2 = 49;
#endif
        std::vector<std::pair<int, int>> pairs = {{40, 44}, {45, vstop2}};
        auto log = h.run(0, 55, pairs);
        auto m = byLine(log);
        auto dataLines = linesMatching(m, isDataPair);

        c.note("Image-fetch lines with a minimal one-line gap (usage1 40-43, usage2 45-47): " + join(dataLines));
        bool ok = dataLines == std::vector<int>{40, 41, 42, 43, 45, 46, 47};

        c.expect(ok,
            "A6a-ONE-LINE-GAP-IS-SUFFICIENT-FOR-NEXT-USAGE: a next usage whose VSTART (45) is "
            "exactly one scan line below the previous usage's last displayed line (43) activates "
            "correctly, using line 44 -- the one intervening line -- purely to fetch its own control "
            "pair, matching \"at least one\" rather than needing more (CH4-eod.c3, CH4-fig4-10.c2, "
            "CH4-fig4-10.c3, CH4-fig410.f2, CH4-fig410.f3, CH4-fig4-9.c2, CH4-fig49.f2, CH4-p108.c14, "
            "CH4-p116.c5, CH4-p117.c1, CH4-p117.c3, CH4-reuse.c10, CH4-reuse.c11, CH4-reuse.c14, "
            "CH4-reuse.c8)");
    }

    // ---- A6b: NEXT-USAGE-VSTART-BEHIND-CURRENT-BEAM-NEVER-ACTIVATES-THIS-FIELD ----
    {
        Harness h;
        int vstart2 = 42;
#ifdef MUT_A6B
        vstart2 = 46;  // now ahead of the beam instead of behind it
#endif
        std::vector<std::pair<int, int>> pairs = {{40, 44}, {vstart2, 50}};
        auto log = h.run(0, 60, pairs);
        auto m = byLine(log);
        auto dataLines = linesMatching(m, isDataPair);

        c.note("Image-fetch lines with a next-usage VSTART=" + std::to_string(vstart2) +
               " fed at the reload (vpos already 44): " + join(dataLines));
        bool onlyUsage1 = dataLines == std::vector<int>{40, 41, 42, 43};

        c.expect(onlyUsage1,
            "A6b-NEXT-USAGE-VSTART-BEHIND-CURRENT-BEAM-NEVER-ACTIVATES-THIS-FIELD: a next-usage "
            "control pair whose VSTART (42) is already behind the beam position (44, since vpos only "
            "increases within a field) at the moment it is read never produces an image window for "
            "the rest of this field, unlike a VSTART ahead of the beam (CH4-eod.c3, CH4-p108.c14)");
    }

    // ---- A7: EXACTLY-TWO-DMA-GRANTS-PER-ACTIVE-LINE ----
    {
        Harness h;
        std::vector<std::pair<int, int>> pairs = {{40, 43}, {50, 53}, {60, 62}, {0, 0}};
        std::vector<Grant> log;
        size_t ctrlIdx = 0;
        long seen = 0;
        (void)seen;
        for (int line = 0; line <= 70; ++line) {
            h.dut->vpos = line;
            h.dut->vblend = (line == 0) ? 1 : 0;
            for (int hp = 0; hp < HPOS_MAX; ++hp) {
                h.dut->hpos = hp;
                h.dut->clk7_en = 1;
                h.dut->eval();
                if (h.dut->reqdma) {
                    bool grantIt = true;
#ifdef MUT_A7
                    grantIt = (seen % 3) != 2;  // withhold every third observed request
                    ++seen;
#endif
                    if (grantIt) {
                        h.dut->ackdma = 1;
                        h.dut->eval();
                        int reg = h.dut->reg_address_out;
                        uint16_t val;
                        if (reg == SPR0POS) {
                            val = ctrlIdx < pairs.size() ? posWord(pairs[ctrlIdx].first) : 0;
                        } else if (reg == SPR0CTL) {
                            val = ctrlIdx < pairs.size() ? ctlWord(pairs[ctrlIdx].first, pairs[ctrlIdx].second) : 0;
                            if (ctrlIdx < pairs.size()) ++ctrlIdx;
                        } else {
                            val = (uint16_t)(0xD000 | ((line & 0x3F) << 2) | (reg & 0x3));
                        }
                        h.dut->reg_address_in = reg;  // see run()'s own comment on this mirroring
                        h.dut->data_in = val;
                        h.dut->eval();
                        log.push_back(Grant{line, hp, reg, val});
                    } else {
                        h.dut->ackdma = 0;
                        h.dut->reg_address_in = 0;
                    }
                } else {
                    h.dut->ackdma = 0;
                    h.dut->reg_address_in = 0;
                }
                h.tick();
                h.dut->clk7_en = 0;
                h.dut->reg_address_in = 0;
                h.tick();
                h.tick();
                h.tick();
            }
        }
        // Opening the RTL (per ORDER OF WORK, after this spec's own commit) showed
        // agnus_spritedma is not a single-channel instance: it internally cycles through all
        // eight sprite channels, each with its own register block (SPR0.. at 0xA0-0xA3, SPR1..
        // at 0xA4-0xA7, ... SPR7.. at 0xBC-0xBF -- line 0's 16 grants are each of the eight
        // channels' own initial POS/CTL fetch, exactly the "each of the eight channels" this
        // package's own Dc decline bucket wasn't able to observe before that RTL read). This
        // package still exercises only channel 0, as the CLAIM COVERAGE ledger's own citations
        // (CH4-p117.c2, CH4-reuse.c12, CH4-reuse.c13) are about "each ... channel"
        // individually, not the combined 8-channel bus, so the per-line count below is
        // filtered to channel 0's own four register addresses -- a harness fix (which
        // addresses count), not a change to the expected value (still exactly two).
        std::vector<Grant> ch0log;
        for (auto& g : log)
            if (g.reg == SPR0POS || g.reg == SPR0CTL || g.reg == SPR0DATA || g.reg == SPR0DATB) ch0log.push_back(g);
        auto m = byLine(ch0log);
        bool allTwo = !m.empty() && std::all_of(m.begin(), m.end(),
            [](const std::pair<const int, std::vector<int>>& kv) { return kv.second.size() == 2; });

        c.note("Channel 0's own active lines and grant counts: " + std::to_string(m.size()) + " lines active");
        c.expect(allTwo,
            "A7-EXACTLY-TWO-DMA-GRANTS-PER-ACTIVE-LINE: across three chained usages, every line the "
            "channel is active on (control-pair or image-pair lines alike) receives exactly two "
            "granted DMA cycles, never one, never three or more (CH4-p117.c2, CH4-reuse.c12, "
            "CH4-reuse.c13)");
    }

    // ---- A8: CONTROL-WORDS-FRESHLY-FETCHED-EACH-DISPLAY-FIELD ----
    {
        Harness h;
        std::vector<std::pair<int, int>> field1 = {{10, 13}};
        auto log1 = h.run(0, 20, field1);
        auto m1 = byLine(log1);
        auto dataLines1 = linesMatching(m1, isDataPair);

        // A fresh vbl pulse and a new vblend-triggered fetch, as if a new display field has
        // begun, feeding a DIFFERENT control pair than field 1's.
        h.dut->vbl = 1;
        h.tick_hpos();
        h.dut->vbl = 0;

        int vstop2 = 8;
#ifdef MUT_A8
        vstop2 = 9;
#endif
        std::vector<std::pair<int, int>> field2 = {{5, vstop2}};
        auto log2 = h.run(0, 20, field2);
        auto m2 = byLine(log2);
        auto dataLines2 = linesMatching(m2, isDataPair);

        c.note("Field 1 (VSTART=10,VSTOP=13) image-fetch lines: " + join(dataLines1));
        c.note("Field 2 (VSTART=5,VSTOP=" + std::to_string(vstop2) + ") image-fetch lines: " + join(dataLines2));

        bool field1ok = dataLines1 == std::vector<int>{10, 11, 12};
        bool field2ok = dataLines2 == std::vector<int>{5, 6, 7};

        c.expect(field1ok && field2ok,
            "A8-CONTROL-WORDS-FRESHLY-FETCHED-EACH-DISPLAY-FIELD: a second, independent vblend-"
            "triggered control fetch fed a DIFFERENT VSTART/VSTOP pair than the first produces its "
            "own, differently-positioned image window in that same run -- the channel re-reads its "
            "control words fresh every field rather than caching the first field's values, which is "
            "also what would let a value changed between two fields (or two individually-granted "
            "words) end up combining old and new information (CH4-move.c1, CH4-move.c2, CH4-move.c3, "
            "CH4-move.c4, CH4-p110.c13, CH4-p110.c15, CH4-p110.c16, CH4-p113.c1, CH4-p113.c2, "
            "CH4-p113.c3, CH4-p127.c8, CH4-ptr.c2, CH4-ptrs.c3, CH4-reset.c1, CH4-reset.c3, "
            "CH4-reset.c4)");
    }

    // ---- A9: VSTART-VSTOP-ARE-9BIT-VALUES-SPLIT-ACROSS-SPRXPOS-AND-SPRXCTL ----
    {
        Harness h;
        int vstop = 310;
#ifdef MUT_A9
        vstop = 311;
#endif
        // VSTART=300 and VSTOP=310 both exceed 255, so both require the bit-8 extension the
        // quoted bit tables assign to SPRxCTL bits 2 and 1.
        std::vector<std::pair<int, int>> pairs = {{300, vstop}};
        auto log = h.run(290, 315, pairs);
        auto m = byLine(log);
        auto dataLines = linesMatching(m, isDataPair);

        std::vector<int> expected;
        for (int l = 300; l < 310; ++l) expected.push_back(l);

        c.note("Image-fetch lines for VSTART=300,VSTOP=" + std::to_string(vstop) + ": " + join(dataLines));
        bool ok = dataLines == expected;

        c.expect(ok,
            "A9-VSTART-VSTOP-ARE-9BIT-VALUES-SPLIT-ACROSS-SPRXPOS-AND-SPRXCTL: feeding VSTART=300, "
            "VSTOP=310 (both requiring the bit-8 extension the manual's own bit tables assign to "
            "SPRxCTL bits 2 and 1, on top of the 8 low bits SPRxPOS carries) produces an image-fetch "
            "window at exactly vpos 300..309 (height 10) -- the module reconstructs each 9-bit value "
            "from SPRxPOS's low 8 bits combined with the matching SPRxCTL high bit, exactly as the "
            "quoted bit tables describe (CH4-det.c19, CH4-p107.c10, CH4-p107.c2, CH4-p107.c3, "
            "CH4-p107.c5, CH4-p107.c6, CH4-p107.c9, CH4-p126.c16, CH4-p128.c15, CH4-p128.c16, "
            "CH4-p128.c4, CH4-p128.c7, CH4-sctl.c14, CH4-sctl.c15, CH4-sctl.c3, CH4-spos.c1, "
            "CH4-sprctl.c1, CH4-sprctl.c2, CH4-sprctl.c4, CH4-sprctl.c7, CH4-sprctl.c8, "
            "CH4-sprpos.c3, CH4-sprpos.c4)");
    }

    std::cout << "\n" << c.passed << " passed, " << c.failed << " failed.\n";
    return c.failed > 0 ? 1 : 0;
}

// RESULTS: baseline (make, no MUTFLAGS) is 12 passed, 0 failed -- every
// claim-based and control expect() below is GREEN against agnus_spritedma.v
// as built today. No finding is on record for this package; the two
// deviations from this file's first draft (see ORDER OF WORK above) were
// both this harness's own bugs, fixed before any result was read as a
// finding, not RTL disagreements. Each `make MUTFLAGS=-DMUT_<id>=1` run
// below was built and run in isolation, confirmed to flip ONLY its own
// named assertion red (all 11 others staying exactly at the green
// baseline), then rebuilt with no MUTFLAGS to confirm the 12/0 baseline
// returned before moving to the next id.
//
// MUTATION LOG (one physical line each; run today, in this order, under
// WSL/Verilator, immediately after the harness fixes above):
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: Run B's fed pair changed from
//   (VSTART=12,VSTOP=15) to Run A's own (40,44), so the two runs stop
//   differing -> FAIL ("11 passed, 1 failed"), only [control]
//   FIXTURE-LIVE-DATA-PATH newly red (both runs' image-fetch lines became
//   [40,41,42,43], no longer distinct). Reverted: 12/0.
// MUTATION A1-CONTROL-PAIR-FETCHED-FIRST-POS-THEN-CTL stimulus red: vblend withheld for the whole run (also moved
//   the sweep to start at line 5 rather than 0, once opening the RTL showed
//   reqdma is independently raised whenever vpos already equals VSTOP's
//   reset value of 0 -- see ORDER OF WORK -- which had been masking this
//   exact mutation at line 0) -> FAIL ("11 passed, 1 failed"), only
//   A1-CONTROL-PAIR-FETCHED-FIRST-POS-THEN-CTL newly red (no grant at all
//   within the swept window). Reverted: 12/0.
// MUTATION A2-TWO-WORDS-PER-LINE-HEIGHT-EQUALS-VSTOP-MINUS-VSTART stimulus red: fed VSTOP changed from 44 to 45 -> FAIL ("11
//   passed, 1 failed"), only A2-TWO-WORDS-PER-LINE-HEIGHT-EQUALS-VSTOP-
//   MINUS-VSTART newly red (measured window grew to [40,41,42,43,44], not
//   the asserted [40,41,42,43]). Reverted: 12/0.
// MUTATION A3u-TWO-DISTINCT-REGISTERS-RECEIVE-THE-FETCHED-PAIR stimulus red: fed pair changed from (40,44) to (100,104), so
//   lines 40-43 (the lines this check inspects) go idle -> FAIL ("11
//   passed, 1 failed"), only A3u-TWO-DISTINCT-REGISTERS-RECEIVE-THE-
//   FETCHED-PAIR newly red (m[40..43] all empty). Reverted: 12/0.
// MUTATION A4a-END-OF-DATA-PAIR-DECODED-AS-NEXT-CONTROL-PAIR stimulus red: usage 1's fed VSTOP changed from 44 to 200, so
//   its reload never appears inside the swept window -> FAIL ("11 passed,
//   1 failed"), only A4a-END-OF-DATA-PAIR-DECODED-AS-NEXT-CONTROL-PAIR
//   newly red (only one control-pair line measured, the initial one).
//   Reverted: 12/0.
// MUTATION A4b-VSTOP-EQUALS-VSTART-PRODUCES-NO-IMAGE-OUTPUT-BUT-CHAIN-CONTINUES stimulus red: usage 1's fed VSTOP changed from 20 (equal to
//   its own VSTART) to 25, so it is no longer a zero-height usage -> FAIL
//   ("11 passed, 1 failed"), only A4b-VSTOP-EQUALS-VSTART-PRODUCES-NO-
//   IMAGE-OUTPUT-BUT-CHAIN-CONTINUES newly red (line 20 now shows image
//   output). Reverted: 12/0.
// MUTATION A5-CHANNEL-REUSED-MULTIPLE-TIMES-WITHIN-ONE-FIELD stimulus red: usage 2's fed VSTOP changed from 53 to 54 ->
//   FAIL ("11 passed, 1 failed"), only A5-CHANNEL-REUSED-MULTIPLE-TIMES-
//   WITHIN-ONE-FIELD newly red (measured line list gained a 53). Reverted:
//   12/0.
// MUTATION A6a-ONE-LINE-GAP-IS-SUFFICIENT-FOR-NEXT-USAGE stimulus red: usage 2's fed VSTOP changed from 48 to 49 ->
//   FAIL ("11 passed, 1 failed"), only A6a-ONE-LINE-GAP-IS-SUFFICIENT-FOR-
//   NEXT-USAGE newly red (measured line list gained a 48). Reverted: 12/0.
// MUTATION A6b-NEXT-USAGE-VSTART-BEHIND-CURRENT-BEAM-NEVER-ACTIVATES-THIS-FIELD stimulus red: the next-usage VSTART fed at the reload
//   changed from 42 (behind the beam) to 46 (ahead of it) -> FAIL ("11
//   passed, 1 failed"), only A6b-NEXT-USAGE-VSTART-BEHIND-CURRENT-BEAM-
//   NEVER-ACTIVATES-THIS-FIELD newly red (a second image window appeared).
//   Reverted: 12/0.
// MUTATION A7-EXACTLY-TWO-DMA-GRANTS-PER-ACTIVE-LINE stimulus red: every third observed reqdma request (across all
//   eight channels, counted globally) had its ackdma withheld -> FAIL ("11
//   passed, 1 failed"), only A7-EXACTLY-TWO-DMA-GRANTS-PER-ACTIVE-LINE
//   newly red (some of channel 0's own active lines dropped to one grant).
//   Reverted: 12/0.
// MUTATION A8-CONTROL-WORDS-FRESHLY-FETCHED-EACH-DISPLAY-FIELD stimulus red: field 2's fed VSTOP changed from 8 to 9 ->
//   FAIL ("11 passed, 1 failed"), only A8-CONTROL-WORDS-FRESHLY-FETCHED-
//   EACH-DISPLAY-FIELD newly red (field 2's measured window gained an 8).
//   Reverted: 12/0.
// MUTATION A9-VSTART-VSTOP-ARE-9BIT-VALUES-SPLIT-ACROSS-SPRXPOS-AND-SPRXCTL stimulus red: fed VSTOP changed from 310 to 311 -> FAIL ("11
//   passed, 1 failed"), only A9-VSTART-VSTOP-ARE-9BIT-VALUES-SPLIT-ACROSS-
//   SPRXPOS-AND-SPRXCTL newly red (measured window gained a 310, breaking
//   the exact expected 300..309). Reverted: 12/0.
// MUTATION A3-order-DATA-VS-DATB-FETCH-ORDER none impossible: not an expect() (checks.record() only,
//   no checks.expect) -- the disputed fetch order is measured and printed,
//   never compared against an expected value, so there is no red/green
//   state for a mutation to move.
