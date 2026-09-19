// ch4-attached-sprites -- denise_sprites conformance test
//
// Test purpose: attached-sprite behaviour at the denise_sprites module
//   boundary -- the ATTACH bit (bit 7 of sprite control word 2) that
//   pairs an odd-numbered sprite with its even-numbered partner, and
//   Table 4-5's row 0 (four-bit combined code 0000), the disputed row
//   of the combined-code-to-colour-register mapping. Package
//   ch4-attached-sprites, fixture denise_sprites (one Verilator build,
//   denise_sprites.v + denise_sprites_shifter.v, top module
//   denise_sprites -- the same fixture hrm/denise_sprites/ and
//   hrm/ch3-sprite-registers/ already build).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition,
//   printed pp. 122, 128.
//
// CLAIM SOURCE -- READ THIS FIRST: this package's own assigned claims
//   file is hrm-testpipeline/work/ch4_packages/
//   ch4-regression-attached-row-0000.jsonl (4 records: CH4-p122.c1,
//   CH4-tab45.c0, CH4-tab45.c1, CH4-tab45.c17 -- all Table 4-5 row 0).
//   By direct instruction from the build task (not discovered by this
//   agent), this file ALSO asserts 5 further claim records that belong
//   to two OTHER, not-yet-built packages' own inventories:
//   CH4-attach.c25, CH4-attach.c26, CH4-attach.c27 (ch4-sprite-colour-
//   encoding.jsonl, p122) and CH4-p128.c8, CH4-sctl.c4 (ch4-sprite-
//   shifter-and-arming.jsonl, p128). The build task's stated reason:
//   two independent classifiers working different lots measured the
//   ATTACH-bit-parity question the same way, and a lead verified it
//   against the source and both printed pages, which is why this
//   candidate finding is being tested now rather than waiting for
//   those packages' own turn. Whoever later builds ch4-sprite-colour-
//   encoding or ch4-sprite-shifter-and-arming should treat these 5 ids
//   as already covered here, not uncovered in their own package.
// Non-goals: this package does not test the fifteen non-zero rows of
//   Table 4-5 (codes 0001-1111, claim ids CH4-tab45.c2..c16 and
//   CH4-p122.c2..c16) -- per this package's own classify block, those
//   stay with whichever package carries the rest of Table 4-5; only
//   row 0 (the disputed row) was assigned here, as a regression class.
//   It does not test collision, priority against the playfield, or
//   vertical positioning -- this boundary carries no vertical-position
//   input (only hpos), matching hrm/ch3-sprite-registers/'s own
//   Non-goals note for the same module.
//
// BOUNDARY: denise_sprites.v / denise_sprites_shifter.v, reached at
//   $(NANOMIG)/src/minimig-aga/. Port list, register word-address map,
//   four-phase 7MHz clocking, and the DATB-then-DATA arm-on-write
//   convention are NOT read from those two files by this agent for
//   this package -- they are reused verbatim from two SIBLING packages
//   already committed and already built against this exact fixture:
//   hrm/denise_sprites/test_denise_sprites.cpp (the original FPGA-030
//   harness) and hrm/ch3-sprite-registers/test_denise_sprites.cpp (the
//   HRM-format harness for the same module, which itself documents
//   this same reuse in its own ORDER OF WORK section). Reusing an
//   already-committed sibling harness's disclosed interface is the
//   precedent that package itself relies on ("not learned by opening
//   this package's own forbidden-until-committed file") and is
//   followed the same way here.
//
// ORDER OF WORK: this file -- every assertion, its expected value, the
//   claim ids it cites, and the stimulus each measurement uses -- was
//   written and committed from claims.jsonl's quotations (this
//   package's own 4, plus the 5 the build task named directly, see
//   CLAIM SOURCE above) and the two already-committed sibling
//   harnesses' disclosed interface ONLY. denise_sprites.v and
//   denise_sprites_shifter.v themselves were not opened by this agent
//   until after that commit, per the build brief's explicit gate. The
//   register word-address convention (SPR0POS=0xA0 .. SPR1DATB=0xA7),
//   the clk7_en/c1/c3 four-phase clocking, and the arm-on-write
//   sequencing are reused from the sibling harnesses' own already-
//   built code, not learned by reading this package's own RTL.
//
// THE TWO PROPOSITIONS UNDER TEST, IN THE MANUAL'S OWN WORDS:
//   p122 (CH4-attach.c25, CH4-attach.c26, CH4-attach.c27): "Attachment
//   is in effect only when the ATTACH bit, bit 7 in sprite control
//   word 2, is set to 1 in the data structure for the odd-numbered"
//   [sprite].
//   p128 (CH4-p128.c8, CH4-sctl.c4): "Bit 7 is the attach bit. This
//   bit is valid only for odd-numbered" [sprites].
//   Read together, on two separate printed pages, these state a single
//   proposition with two halves: (a) setting bit 7 in the ODD sprite's
//   own control word puts attachment into effect, and (b) that bit is
//   valid ONLY for the odd-numbered sprite -- i.e. setting the SAME
//   bit in the EVEN sprite's own control word, with the odd sprite's
//   own bit left clear, must NOT put attachment into effect. This file
//   asserts (a) as a control (ATTACH-BIT-ODD-SPRITE-ENABLES-
//   ATTACHMENT, expected PASS) and (b) as the candidate finding
//   (ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE) on the identical
//   measurement basis, so a red result on (b) cannot be dismissed as a
//   units-error in the harness -- (a), measured the same way, is
//   right there. Per the build brief: a red result on (b) is a
//   finished outcome, not something to adjust the oracle to avoid.
//
// MEASUREMENT BASIS (ATTACH-BIT-* assertions): both sprites of the
//   pair (sprite 0 = even, sprite 1 = odd) are armed at the same
//   SPRxPOS byte (0x40) with a non-degenerate, non-identical image
//   each (SPR0: DATB=0x0000, DATA=0x8000; SPR1: DATB=0x8000,
//   DATA=0x8000 -- both already-validated-reachable values, reused
//   from hrm/ch3-sprite-registers/'s own measure_attach_color()
//   recipe for this same module), and the full sprdata trace is
//   recorded while hpos sweeps 0..511 (9-bit start value, full range).
//   Three configurations of the two control words' bit 7 are compared
//   against each other, never against a hand-picked constant:
//     (i)   baseline:  SPR0CTL bit7=0, SPR1CTL bit7=0 (neither set)
//     (ii)  odd-set:   SPR0CTL bit7=0, SPR1CTL bit7=1 (the manual's
//                       documented case)
//     (iii) even-set:  SPR0CTL bit7=1, SPR1CTL bit7=0 (the candidate
//                       finding's case)
//   ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT requires trace(ii) !=
//   trace(i). ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE requires trace(iii)
//   == trace(i). Every trace is measured on a freshly reset DUT.
//
// MEASUREMENT BASIS (CODE-0000-YIELDS-TRANSPARENT-PIXEL): the pair is
//   genuinely attached (SPR1CTL bit7=1, the manual's documented,
//   control-proven mechanism) and BOTH sprites' DATA and DATB are
//   written 0x0000, so the combined four-bit code is 0000 throughout
//   the run. Table 4-5's own row 0 and its footnote both agree code
//   0000 shows no colour; this is asserted directly.
//
// MEASUREMENT BASIS (FIXTURE-LIVE-DATA-PATH, [control]): proves the
//   write -> arm -> hpos-sweep -> read-sprdata path is alive, and that
//   it delivers DISTINCT fetched values in order, before any claim-
//   based assertion depends on it (per the build brief's own
//   chapter-6 warning: a harness whose data path is silently dead
//   reports every data-dependent assertion as a finding about the
//   design instead of about the harness). Sprite 0 alone is armed
//   three times, from a fresh reset each time, with DATB held at
//   0x0000 and DATA holding a single 1-bit at three different bit
//   positions: 0x8000 (bit 15, the word's MSB), 0x0080 (bit 7, a
//   middle bit), 0x0001 (bit 0, the LSB). Each run is required to show
//   EXACTLY one active (non-zero sprdata) sample across the whole
//   hpos 0..511 sweep -- a single shifted-out 1-bit, not a stuck value
//   -- and the three active hpos values are required to be distinct
//   and monotonic in the SAME order as the three bit positions (MSB,
//   middle, LSB), in either direction. This proves three distinct
//   words placed on the register-write interface are each fetched,
//   shifted out, and observed in the order their bits were loaded --
//   without this file needing to know, or assume, denise_sprites_
//   shifter.v's shift direction or exact per-pixel clock ratio, which
//   are RTL facts this package's own ORDER OF WORK gate does not
//   permit assuming in advance.
//
// CLAIM COVERAGE -- 9 records cited (4 from this package's own claims
//   file, 5 borrowed per CLAIM SOURCE above), all cited in full inside
//   an expect()/record() label:
//
//   ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT (3 ids, control/PASS)
//     -- CH4-attach.c25, CH4-attach.c26, CH4-attach.c27
//   ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE (3 ids, candidate finding)
//     -- CH4-p128.c8, CH4-sctl.c4, CH4-attach.c27
//   CODE-0000-YIELDS-TRANSPARENT-PIXEL (2 ids, precondition/PASS)
//     -- CH4-p122.c1, CH4-tab45.c17
//   TABLE45-ROW0-REGISTER-NUMBER-CONFLICT [declined] (2 ids)
//     -- CH4-tab45.c0, CH4-tab45.c1
//   3+3+2+2 = 10 citations over 9 distinct ids (CH4-attach.c27 is cited
//   on both ATTACH-BIT assertions, since "for the odd-numbered" is the
//   textual basis for both "the odd one enables it" and "not the even
//   one" -- see THE TWO PROPOSITIONS above). All 4 of this package's
//   own claims.jsonl records are covered: 2 asserted
//   (CH4-p122.c1/CH4-tab45.c17), 2 declined with reason
//   (CH4-tab45.c0/CH4-tab45.c1 -- see TABLE45-ROW0-REGISTER-NUMBER-
//   CONFLICT below, matching this package's own classify block:
//   "Measured for the record and asserted nowhere").
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id, labelled
//   "[control]" so the mutation gate does not require a mutation
//   record naming a claim id for it, matching hrm/ch3-sprite-registers/
//   's and hrm/ch3-collision-registers/'s own convention) proves the
//   measurement basis every other assertion in this file shares is
//   alive. ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT is additionally a
//   control FOR ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE: both are
//   measured the same way (a trace comparison against the identical
//   baseline (i)), so if the candidate finding comes out red, this
//   sibling assertion passing on the same basis rules out a units-
//   error in the harness itself.
//
// RESULTS (WSL/Verilator 5.052, built and run 11.09.2026, AFTER the
//   specification commit -- denise_sprites.v and denise_sprites_
//   shifter.v were opened only once, for this build, not before):
//   3 passed, 1 failed. FIXTURE-LIVE-DATA-PATH, ATTACH-BIT-ODD-SPRITE-
//   ENABLES-ATTACHMENT, and CODE-0000-YIELDS-TRANSPARENT-PIXEL are all
//   GREEN. ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE is RED -- the
//   candidate finding is CONFIRMED, exactly as classified, not
//   adjusted to pass.
//
//   FINDING -- denise_sprites.v:484 reads `if (attach1 || (!aga &&
//   attach0))`. At aga=0 (OCS/ECS -- this package's own measurement
//   basis, matching the sibling harnesses' own default), the EVEN
//   sprite's own ATTACH bit (attach0) alone -- with the ODD sprite's
//   own bit (attach1) clear -- is enough to open the combined-colour
//   path, the same path the ODD sprite's own bit opens. This
//   contradicts 'This bit is valid only for odd-numbered' (p.128) and
//   '...set to 1 in the data structure for the odd-numbered' (p.122):
//   on this RTL, at aga=0, the bit is not odd-only. The MUTATION LOG's
//   own ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE entry below additionally
//   shows the finding is SCOPED to aga=0 -- at aga=1 (AGA) the same
//   line's `!aga` term is 0, attach0 can no longer open the path, and
//   the RTL matches the manual.
//
// MUTATION LOG (WSL/Verilator, same session as RESULTS above; each
//   entry actually run: rebuild with the named -D flag -> run -> read
//   the PASS/FAIL lines -> confirm only the named assertion changed
//   colour -> rebuild with no flags -> confirm the 3-passed/1-failed
//   baseline was restored):
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: built with -DMUT_FIXTURE=1, which makes the MSB-run stimulus 0x0001 instead of 0x8000 (identical to the LSB-run) -> FAIL (2 passed, 2 failed), only FIXTURE-LIVE-DATA-PATH newly red (MSB hpos=145 became equal to LSB hpos=145, no longer distinct or monotonic; mid hpos=138 unchanged). Rebuilt with no flags: 3 passed, 1 failed, the one finding unchanged.
// MUTATION ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT stimulus red: built with -DMUT_ODD=1, which withholds the ATTACH stimulus in case (ii) (SPR1CTL bit7 measured at 0 instead of 1) -> FAIL (2 passed, 2 failed), only ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT newly red (trace(ii) became identical to trace(i), no longer distinct); ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE, already red at baseline, stayed red for its own reason, confirming the isolation held. Rebuilt with no flags: 3 passed, 1 failed, the one finding unchanged.
// MUTATION ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE stimulus green: built with -DMUT_EVEN=1, which re-measures case (iii) and a fresh case-(i)-shaped baseline both at aga=1 instead of aga=0 (denise_sprites.v:484's own `!aga` term, read while diagnosing the baseline red result above) -> PASS (4 passed, 0 failed), only ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE newly green -- confirming the finding is scoped to aga=0 (OCS/ECS): at aga=1 (AGA) this RTL matches the manual. Rebuilt with no flags: 3 passed, 1 failed, the one finding unchanged. CORRECTION, recorded rather than dropped: this mutation's first design instead changed case (iii)'s own SPR0DATA image (0x4000 instead of 0x8000, aga left at 0) -- built and run, it produced NO colour change at all (3 passed, 1 failed, identical to baseline), because the finding holds for any non-zero image once attach0 opens the path at aga=0. Replaced by the aga-based mutation above, which does move the assertion; the assertion's own expected value and comparison (trace equality) were never touched in either attempt.
// MUTATION CODE-0000-YIELDS-TRANSPARENT-PIXEL stimulus red: built with -DMUT_ZERO=1, which changes SPR0DATA from 0x0000 to 0x8000 (combined code no longer 0000) -> FAIL (2 passed, 2 failed), only CODE-0000-YIELDS-TRANSPARENT-PIXEL newly red (sprdata went non-zero at the expected shifted-out position); ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE, already red at baseline, stayed red for its own reason. Rebuilt with no flags: 3 passed, 1 failed, the one finding unchanged.
// MUTATION TABLE45-ROW0-REGISTER-NUMBER-CONFLICT: no record -- this claim is declined (checks.record() only, no checks.expect()), so there is no red/green state for a mutation to move.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_sprites.h"

// MUT_FIXTURE: replay FIXTURE-LIVE-DATA-PATH's own mutation record above
// (-DMUT_FIXTURE=1). Makes the MSB-run and LSB-run stimuli identical.
#ifndef MUT_FIXTURE
#define MUT_FIXTURE 0
#endif
// MUT_ODD: replay ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT's own mutation
// record above (-DMUT_ODD=1). Withholds the ATTACH stimulus in case (ii).
#ifndef MUT_ODD
#define MUT_ODD 0
#endif
// MUT_EVEN: replay ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE's own mutation
// record above (-DMUT_EVEN=1). Changes case (iii)'s own SPR0DATA image.
#ifndef MUT_EVEN
#define MUT_EVEN 0
#endif
// MUT_ZERO: replay CODE-0000-YIELDS-TRANSPARENT-PIXEL's own mutation
// record above (-DMUT_ZERO=1). Makes SPR0DATA non-zero.
#ifndef MUT_ZERO
#define MUT_ZERO 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records that state what the SOURCE says, or why a claim is
    // declined. Not a measurement -- must never be printed as though it
    // were one.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map, reused from hrm/denise_sprites/ and
// hrm/ch3-sprite-registers/'s own test_denise_sprites.cpp for this same
// module (SPRPOSCTLBASE byte address 0x140 -> word address 0xA0 for
// SPR0POS; 4 consecutive word addresses per sprite: POS, CTL, DATA, DATB).
constexpr int SPR0POS = 0xA0;
constexpr int SPR0CTL = 0xA1;
constexpr int SPR0DATA = 0xA2;
constexpr int SPR0DATB = 0xA3;
constexpr int SPR1POS = 0xA4;
constexpr int SPR1CTL = 0xA5;
constexpr int SPR1DATA = 0xA6;
constexpr int SPR1DATB = 0xA7;
constexpr int IDLE_ADDR = 0x00;

constexpr int HPOS_MAX = 512;  // SH8..SH0 is a 9-bit value: 0..511 covers it.

struct Meas {
    std::vector<uint8_t> trace;  // sprdata at every swept hpos.
};

// Clocking (clk7_en/c1/c3 four-phase tick, reg_address_in/data_in write
// sequencing) reused from the sibling harnesses for this same module, not
// learned from this package's own RTL (see ORDER OF WORK above).
class Harness {
public:
    Harness(uint8_t aga, uint8_t spres) : dut(new Vdenise_sprites) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->aga = aga;
        dut->spres = spres;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->hpos = 0;
        dut->sprena = 1;
        dut->esprm = 0;
        dut->osprm = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
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

    // c1=0,c3=0 -> shift/load; c1=1,c3=0; c1=1,c3=1 (clk7n_en); c1=0,c3=1.
    void tick_phase() {
        phase = (phase + 1) % 4;
        switch (phase) {
            case 0: dut->c1 = 0; dut->c3 = 0; dut->clk7_en = 1; break;
            case 1: dut->c1 = 1; dut->c3 = 0; dut->clk7_en = 0; break;
            case 2: dut->c1 = 1; dut->c3 = 1; dut->clk7_en = 0; break;
            case 3: dut->c1 = 0; dut->c3 = 1; dut->clk7_en = 0; break;
        }
        tick();
    }

    void tick_7mhz() {
        tick_phase();
        tick_phase();
        tick_phase();
        tick_phase();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick_7mhz();
    }

    Vdenise_sprites* dut;
    int phase = 0;
};

// Arms sprite 0 alone at pos_byte with the given DATA word (DATB held at
// 0), sweeps hpos, records the full sprdata trace. Used only by
// FIXTURE-LIVE-DATA-PATH, which needs no sprite 1 and no ATTACH bit.
Meas measure_single(uint8_t pos_byte, uint16_t data_word) {
    Harness h(/*aga=*/0, /*spres=*/0);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, 0x00);
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, data_word);
    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        m.trace.push_back(static_cast<uint8_t>(h.dut->sprdata));
    }
    return m;
}

// Arms BOTH sprites of the pair at the same pos_byte with a fixed,
// non-degenerate, non-identical image each (reused from
// hrm/ch3-sprite-registers/'s own measure_attach_color() recipe for this
// module), with independently chosen bit-7 (ATTACH) values on each
// sprite's own control word. Used by the two ATTACH-BIT-* assertions.
// `aga` defaults to 0 (OCS/ECS, this package's own measurement basis);
// MUT_EVEN's own mutation below is the only caller that overrides it.
Meas measure_pair(uint8_t pos_byte, uint8_t ctl0_bit7, uint8_t ctl1_bit7,
                   uint16_t spr0data_override = 0x8000, uint8_t aga = 0) {
    Harness h(aga, /*spres=*/0);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, static_cast<uint16_t>(ctl0_bit7 ? 0x80 : 0x00));
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, spr0data_override);
    h.write_reg(SPR1POS, pos_byte);
    h.write_reg(SPR1CTL, static_cast<uint16_t>(ctl1_bit7 ? 0x80 : 0x00));
    h.write_reg(SPR1DATB, 0x8000);
    h.write_reg(SPR1DATA, 0x8000);
    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        m.trace.push_back(static_cast<uint8_t>(h.dut->sprdata));
    }
    return m;
}

// Arms the (attached) pair with both sprites' DATA/DATB at zero, so the
// combined four-bit code is 0000 throughout. Used by
// CODE-0000-YIELDS-TRANSPARENT-PIXEL.
Meas measure_pair_zero(uint8_t pos_byte, uint16_t spr0data_override = 0x0000) {
    Harness h(/*aga=*/0, /*spres=*/0);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, 0x00);
    h.write_reg(SPR0DATB, 0x0000);
    h.write_reg(SPR0DATA, spr0data_override);
    h.write_reg(SPR1POS, pos_byte);
    h.write_reg(SPR1CTL, 0x80);  // ATTACH on the odd sprite: the pair IS attached.
    h.write_reg(SPR1DATB, 0x0000);
    h.write_reg(SPR1DATA, 0x0000);
    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        m.trace.push_back(static_cast<uint8_t>(h.dut->sprdata));
    }
    return m;
}

// First (and, for a single-1-bit stimulus, only) hpos at which sprdata
// is non-zero, or -1. Also reports whether more than one sample was
// non-zero, so FIXTURE-LIVE-DATA-PATH can require "exactly one".
struct FirstActive {
    int hpos = -1;
    int active_count = 0;
};
FirstActive first_active(const Meas& m) {
    FirstActive r;
    for (size_t i = 0; i < m.trace.size(); ++i) {
        if (m.trace[i] != 0) {
            if (r.hpos == -1) r.hpos = static_cast<int>(i);
            ++r.active_count;
        }
    }
    return r;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- FIXTURE-LIVE-DATA-PATH [control] ----------------
    {
        uint16_t lsb_word = 0x0001;
#if MUT_FIXTURE
        uint16_t msb_word = 0x0001;  // MUTATION: identical to lsb_word.
#else
        uint16_t msb_word = 0x8000;
#endif
        Meas msb = measure_single(/*pos=*/0x40, msb_word);
        Meas mid = measure_single(/*pos=*/0x40, 0x0080);
        Meas lsb = measure_single(/*pos=*/0x40, lsb_word);
        FirstActive fa_msb = first_active(msb);
        FirstActive fa_mid = first_active(mid);
        FirstActive fa_lsb = first_active(lsb);
        bool each_single = fa_msb.active_count == 1 && fa_mid.active_count == 1 &&
                            fa_lsb.active_count == 1;
        bool all_found = fa_msb.hpos != -1 && fa_mid.hpos != -1 && fa_lsb.hpos != -1;
        bool distinct = fa_msb.hpos != fa_mid.hpos && fa_mid.hpos != fa_lsb.hpos &&
                         fa_msb.hpos != fa_lsb.hpos;
        bool monotonic = distinct && all_found &&
            ((fa_msb.hpos < fa_mid.hpos && fa_mid.hpos < fa_lsb.hpos) ||
             (fa_msb.hpos > fa_mid.hpos && fa_mid.hpos > fa_lsb.hpos));
        checks.expect(each_single && all_found && distinct && monotonic,
            "[control] FIXTURE-LIVE-DATA-PATH: arming sprite 0 (SPR0POS=0x40, "
            "SPR0CTL=0x00, SPR0DATB=0x0000) three times from a fresh reset, "
            "each time with a single distinct 1-bit SPR0DATA word (0x8000, "
            "0x0080, 0x0001 -- bit 15, bit 7, bit 0) and sweeping hpos "
            "0..511, makes exactly one sprdata sample go non-zero per run, "
            "at three distinct hpos values that are monotonic in the same "
            "order as the three bit positions -- proving the write -> arm "
            "-> hpos-sweep -> read-sprdata measurement basis every "
            "assertion below shares actually delivers distinct fetched "
            "values, in order, at the cycle each was fetched on, not a "
            "single stuck value. Fixture check, no claim id, no page. "
            "MSB hpos=" + std::to_string(fa_msb.hpos) +
            " mid hpos=" + std::to_string(fa_mid.hpos) +
            " LSB hpos=" + std::to_string(fa_lsb.hpos) + ".");
    }

    // Shared baseline (i): both sprites armed, non-degenerate, non-identical
    // images, neither control word's bit 7 set. Used by both ATTACH-BIT-*
    // assertions below as the SAME reference trace.
    Meas baseline = measure_pair(/*pos=*/0x40, /*ctl0_bit7=*/0, /*ctl1_bit7=*/0);

    // ---------------- ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT ----------------
    {
#if MUT_ODD
        Meas odd_set = measure_pair(/*pos=*/0x40, /*ctl0_bit7=*/0, /*ctl1_bit7=*/0);  // MUTATION: ATTACH withheld.
#else
        Meas odd_set = measure_pair(/*pos=*/0x40, /*ctl0_bit7=*/0, /*ctl1_bit7=*/1);
#endif
        bool differs = odd_set.trace != baseline.trace;
        checks.expect(differs,
            "ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT: with SPR0 and SPR1 "
            "both armed at the same position with fixed, non-degenerate "
            "images (SPR0 DATB=0x0000/DATA=0x8000, SPR1 DATB=0x8000/"
            "DATA=0x8000), setting bit 7 of SPR1CTL -- the ODD-numbered "
            "sprite of the pair -- alone changes the full sprdata trace "
            "measured across hpos 0..511, compared to neither control "
            "word's bit 7 being set: 'Attachment is in effect only when "
            "the ATTACH bit, bit 7 in sprite control word 2, is set to 1 "
            "in the data structure for the odd-numbered' [sprite] (p.122). "
            "CH4-attach.c25, CH4-attach.c26, CH4-attach.c27 .");
    }

    // ---------------- ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE ----------------
    // Candidate finding: build-task instruction, not this package's own
    // claims.jsonl (see CLAIM SOURCE above). Same measurement basis as the
    // control immediately above -- ATTACH-BIT-ODD-SPRITE-ENABLES-
    // ATTACHMENT passing on that basis is this assertion's control.
    {
#if MUT_EVEN
        // MUTATION: aga changed from 0 to 1 for BOTH sides of the
        // comparison (a fresh, self-contained pair, not the outer aga=0
        // `baseline`). denise_sprites.v:484 reads `attach1 || (!aga &&
        // attach0)` -- at aga=1 the `!aga` term is 0, so attach0 (the
        // even sprite's own bit) can no longer open the attach path, and
        // this assertion is expected to flip from red to green.
        Meas baseline_mut = measure_pair(/*pos=*/0x40, /*ctl0_bit7=*/0, /*ctl1_bit7=*/0,
                                          /*spr0data_override=*/0x8000, /*aga=*/1);
        Meas even_set = measure_pair(/*pos=*/0x40, /*ctl0_bit7=*/1, /*ctl1_bit7=*/0,
                                      /*spr0data_override=*/0x8000, /*aga=*/1);
        bool matches_baseline = even_set.trace == baseline_mut.trace;
#else
        Meas even_set = measure_pair(/*pos=*/0x40, /*ctl0_bit7=*/1, /*ctl1_bit7=*/0);
        bool matches_baseline = even_set.trace == baseline.trace;
#endif
        checks.expect(matches_baseline,
            "ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE: with the same two "
            "images as the control above, setting bit 7 of SPR0CTL -- the "
            "EVEN-numbered sprite of the pair -- alone, with SPR1CTL bit 7 "
            "left clear, must NOT change the measured sprdata trace from "
            "the neither-bit-set baseline: 'Bit 7 is the attach bit. This "
            "bit is valid only for odd-numbered' [sprites] (p.128); 'the "
            "ATTACH bit, bit 7 in sprite control word 2, is set to 1 in "
            "the data structure for the odd-numbered' [sprite] (p.122) -- "
            "read together, the bit has no effect when it is the EVEN "
            "sprite's own copy that is set. A red result here is a "
            "finding about denise_sprites.v, not an error in this "
            "assertion; see RESULTS above for what was measured. "
            "CH4-p128.c8, CH4-sctl.c4, CH4-attach.c27 .");
    }

    // ---------------- CODE-0000-YIELDS-TRANSPARENT-PIXEL ----------------
    {
#if MUT_ZERO
        Meas zero = measure_pair_zero(/*pos=*/0x40, /*spr0data_override=*/0x8000);  // MUTATION: SPR0DATA no longer zero.
#else
        Meas zero = measure_pair_zero(/*pos=*/0x40);
#endif
        bool all_transparent = true;
        for (uint8_t v : zero.trace) { if (v != 0) { all_transparent = false; break; } }
        checks.expect(all_transparent,
            "CODE-0000-YIELDS-TRANSPARENT-PIXEL: with the pair attached "
            "(SPR1CTL bit 7 set, the mechanism the control above already "
            "shows live) and both sprites' DATA and DATB registers held "
            "at 0x0000 -- combined four-bit code 0000 throughout the run "
            "-- sprdata stays zero (no sprite pixel) across the whole "
            "hpos 0..511 sweep, matching Table 4-5's own row 0 and its "
            "footnote, the part of the row both disputing pages agree on: "
            "'0            0000            16 *' (CH4-p122.c1/"
            "CH4-tab45.c1, p.122) and '* Unused; yields transparent "
            "pixel.' (CH4-tab45.c17, p.122). This is the transparent "
            "precondition ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT and "
            "ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE both rest on (a pair "
            "that shows no pixel at all if never attached, and no pixel "
            "for this one all-zero code even when it is) -- asserted "
            "directly here rather than left implicit. CH4-p122.c1, "
            "CH4-tab45.c17 .");
        checks.note("CODE-0000-YIELDS-TRANSPARENT-PIXEL: measured sprdata "
            "for combined code 0000 under an attached pair, across the "
            "whole hpos sweep, was uniformly 0x00. This harness's own "
            "sprdata output carries only the colour-index value denise "
            "would present, not a raw 'which Table-4-5 row fired' signal, "
            "so it cannot adjudicate which of the two disputed printed "
            "numbers is correct -- Table 4-5's own row 0 names register "
            "16 (CH4-tab45.c1, p.122) while the chapter's prose instead "
            "reads 'The binary numbers 0 through 15 select registers 17 "
            "through 31' (CH4-tab45.c0, p.122), sixteen codes onto "
            "fifteen registers. See TABLE45-ROW0-REGISTER-NUMBER-CONFLICT "
            "below.");
    }

    // --------- TABLE45-ROW0-REGISTER-NUMBER-CONFLICT [declined] ---------
    // No checks.expect() and no MUTATION record: there is no assertion
    // here for a stimulus to turn red, matching this package's own
    // classify block ("Measured for the record and asserted nowhere").
    checks.record(
        "TABLE45-ROW0-REGISTER-NUMBER-CONFLICT [declined]: page 122 "
        "contradicts itself on which colour register the four-bit code "
        "0000 addresses. The chapter's prose states 'The binary numbers 0 "
        "through 15 select registers 17 through 31 as shown in' [Table "
        "4-5] (CH4-tab45.c0) -- sixteen values (0 through 15) mapped onto "
        "fifteen registers (17 through 31), which cannot hold for all "
        "sixteen. Table 4-5's own row 0 immediately below instead reads "
        "'0            0000            16 *' (CH4-tab45.c1) -- code 0000 "
        "addresses register 16, one below the prose's stated start of "
        "17. The fifteen non-zero rows (0001-1111 -> 17-31) are "
        "undisputed and outside this package's own scope (see Non-goals "
        "above); only row 0 conflicts, and only between two passages on "
        "the SAME printed page. Declined, not asserted either way, "
        "matching this package's own classify block: 'Measured for the "
        "record and asserted nowhere.' CH4-tab45.c0, CH4-tab45.c1 .");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
