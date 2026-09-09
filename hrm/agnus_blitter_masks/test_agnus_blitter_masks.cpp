// Test purpose: Verify BLTAFWM (channel A's first-word mask) and BLTALWM
//   (channel A's last-word mask) at the agnus_blitter boundary: BLTAFWM
//   applied on the first word of every row, BLTALWM on the last, both ANDed
//   together on a one-word row, the AND happening before the masked value
//   enters the barrel shifter and before it enters the minterm generator's
//   logic function, the mask multiplexer and the AND-into-shifter that
//   Figure 6-13 draws, the two registers' own identity/address/writability
//   and their bit-for-bit independence, the two worked mask values in
//   Figure 6-4 and its accompanying prose, and the first/last-word roles
//   swapping (by row position, not by RTL wire) in descending mode.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 170, 180-182, 191-192, 198-199, 266.
// Non-goals: the leaf barrel shifter's own arithmetic (PKG-BLT-SHIFT); the
//   fuller shift-chain claims (zero-fill on the first word only, previous
//   word/row bits chaining in, shift-on-load of BLTBDAT) beyond the single
//   "mask happens before the shift" ordering fact independently asserted
//   here (PKG-BLT-SHIFT-CHAIN, hrm/agnus_blitter_shift_chain); minterm/LF
//   decode correctness beyond using two already-simple LF settings as
//   plumbing (PKG-BLT-MINTERM); full register-decode completeness for every
//   blitter register, including BLTBDAT/BLTCDAT/BLTDDAT (PKG-BLT-REGDECODE);
//   line-mode's own FSM, octant encoding and Bresenham correctness
//   (PKG-BLT-LINE); the fill logic's own carry-chain arithmetic
//   (PKG-BLT-FILL) - FILL-MASK-STAYS-ACTIVE below only shows the A mask is
//   consulted during fill mode, not that fill's carry math is right.
//
// LINE-MODE MASK CLAIMS ARE NOT ASSERTED HERE (reported, not silently
// dropped): 6-appa-registers.c13, ch6-line.c24, ch6-linesum.c2,
// ch6-linesum.c3 all say BLTAFWM/BLTALWM should be $FFFF for line mode.
// Reading agnus_blitter.v shows this IS plausible to test - bltaold's
// register update is gated `next_word && !line` (agnus_blitter.v:336, frozen
// at 0 for the whole line) but the barrel shifter's `new_val` input
// (`bltadat & bltamask`, agnus_blitter.v:345) is a continuous assignment not
// gated by `line` at all, and bltahold registers unconditionally on every
// `enable` cycle (agnus_blitter.v:351-355) - so the A mask plausibly still
// reaches bltahold, minterm_out and eventually data_out during line mode.
// But exercising it needs a valid line-mode stimulus: BLTCON1[4:2] octant
// select, BLTCON1[1] sign, first_pixel/first_line_pixel sequencing and the
// BLT_L1-BLT_L4 FSM (agnus_blitter.v:795-847). That is PKG-BLT-LINE's
// dedicated 225-claim fixture. Building a throwaway line-mode stimulus here
// risks an invalid octant/sign combination that never reaches a D-store at
// all (a silently vacuous "pass") or duplicating PKG-BLT-LINE's own
// construction work for a single claim. Deferred to PKG-BLT-LINE, not
// silently dropped - the same discipline the PKG-BLT-SHIFT builder used
// when it declined shift-on-load ("the module has no clock and no
// register... whatever the brief says"), applied in the other direction: the
// module here DOES have everything needed, but exercising it correctly is a
// different package's proportionate job.
//
// Supersedes hrm/agnus_blitter_mask, per the build register and
// docs/blitter-prior-art.md ("Defect 1"). That package compiles a LOCAL
// COPY of agnus_blitter.v:311-319 pasted into agnus_blitter_mask.v - a
// change to the real agnus_blitter.v cannot make it go red, because nothing
// keeps the copy in step. This package compiles the design
// (agnus_blitter.v itself, via $(NANOMIG)/src/minimig-aga/) instead. It is
// NOT deleted here: it stays in the tree, unchanged and still green, until
// this package is reviewed and confirmed in its place. The prior art also
// notes agnus_blitter_mask.v adds one line the original file does not
// literally have - `assign data_out = bltadat & bltamask` - but that exact
// expression appears twice in the real file, at agnus_blitter.v:337 (into
// bltaold) and :345 (into the barrel shifter's new_val), so the copy
// reproduces existing logic rather than inventing behaviour; no claim here
// rests on that provenance note, since it is a statement about a copied
// file, not about the manual.
//
// TWO CLAIMS OVERTURNED BY PROMPT 5'S ATTACK, asserted against the RTL as
// instructed (not as figure geometry): 6-blitter-block-diagram.c1 and .c2.
// packages.md records why the discard was wrong: "Figure 6-13's upper left
// IS the mask multiplexer agnus_blitter.v:311-319 and the AND into the
// shifter :345. My rule table treated them as figure geometry because the
// restatement says 'AND gates' and not 'mask register'." Both are asserted
// below (MASK-MUX-THREE-REGIMES for .c1, MASK-16-BITS-INDEPENDENT for .c2)
// directly against those RTL lines, not against the picture.
//
// FIXTURE. Started from the working harness in hrm/agnus_blitter_status/
// (the only agnus_blitter fixture in the tree proven to deliver data
// correctly) and from hrm/agnus_blitter_shift_chain/, which was built after
// this package's original brief and already exercises masking before the
// shift - both read in full before writing this file. Harness::cycle and
// the TEST-DATA-DELIVERY assertion immediately below are the same mechanism
// as both files, reproduced here (not a fresh copy of the salvaged
// diagnostic) and re-proven green in THIS build before anything
// data-dependent is asserted. All five timing traps documented in
// agnus_blitter_status's header apply to any agnus_blitter fixture; the
// two this file actually touches:
//   1. `we`/`data_out` are transient, sampled at the grant (Harness::cycle,
//      the settling eval() before the clock edge), never polled after the
//      tick - this file follows that pattern throughout.
//   2. THIS IS THE TRAP THIS PACKAGE IS ABOUT. BLTAFWM/BLTALWM default to 0
//      after reset and bltamask uses them whenever first_word/last_word is
//      true (agnus_blitter.v:311-319) - a test that drives channel A
//      without writing both to a deliberate value first sees A silently
//      masked on its first and/or last word, on a one-word blit its only
//      word. Since masking is exactly what this file measures, every block
//      below writes BOTH BLTAFWM and BLTALWM explicitly before running -
//      the all-ones ("no masking") case is written out loud as a control
//      value in whichever block needs it (e.g. MASK-BEFORE-SHIFT's
//      BLTALWM=0xFFFF, MASK-FWM-EVERY-ROW's BLTALWM=0xFFFF), never left to
//      the reset default. busy/int3/zero (traps 3-5) are not read anywhere
//      in this file, so their settle-margin and sampling-order traps do not
//      apply here.
// A NEW fixture addition for this package: `a_fetch_addresses`, capturing
// `address_out` at the same settled moment as `data_out`, whenever the
// register bus target is BLTADAT (i.e. whenever the current DMA grant is
// channel A's). address_out is combinational off the currently-stored
// pointer register (agnus_blitter_adrgen.v:82-84, `assign address_out =
// bltptr_out`), valid at the same pre-edge moment as everything else this
// harness samples, and is the only way to observe fetch ORDER (as opposed
// to fetch VALUE) - needed for the descending-mode blocks, where "row's
// last word" is a statement about address, not about which value arrives
// in which queue slot.
//
// CLAIM COVERAGE. All 74 records of classify.package=="PKG-BLT-MASKS" and
// classify.status=="testable" in work/2026-09-08_fpga-090_classified.jsonl
// are covered below; both readers (sol/opus) cover the same ground for most
// of this package, so many ids restate one proposition twice (or the same
// reader states it once in general form and once against a worked example
// or a figure) - each proposition is asserted once and every covering id is
// written out in full inside that assertion's message, per the coverage
// script's requirement. Assertion name -> covered ids:
//   MASK-REG-BLTAFWM-IDENTITY: 6-shifts-and-masks.c23, 6-shifts-and-masks.c25,
//     ch6-shifts.c20, ch6-blockdiag.c3, 6-appa-registers.c1,
//     6-appa-registers.c2, 6-appa-registers.c4, ch6-appa-masks.c4,
//     ch6-fig613.f01
//   MASK-REG-BLTALWM-IDENTITY: 6-shifts-and-masks.c24, 6-shifts-and-masks.c26,
//     ch6-shifts.c20, ch6-blockdiag.c3, 6-appa-registers.c5,
//     6-appa-registers.c6, 6-appa-registers.c8, ch6-appa-masks.c8
//   ONLY-A-IS-MASKED: 6-dma-channels.c11, 6-dma-channels.c12, ch6-dma.c9
//   MASK-BEFORE-SHIFT: 6-shifts-and-masks.c27, ch6-shifts.c22, ch6-shifts.c23
//   MASK-BIT-EXCLUDED-FROM-LOGIC: 6-shifts-and-masks.c28, ch6-shifts.c24,
//     6-shifts-and-masks.c36, 6-shifts-and-masks.c37, 6-shifts-and-masks.c39,
//     ch6-shifts.f6, ch6-shifts.f7, ch6-appa-masks.c10, 6-appa-registers.c11
//   MASK-FWM-EVERY-ROW: 6-shifts-and-masks.c29, ch6-shifts.c25,
//     ch6-shifts.c18, 6-appa-registers.c9, ch6-appa-masks.c9
//   MASK-LWM-EVERY-ROW: 6-shifts-and-masks.c30, ch6-shifts.c26,
//     ch6-shifts.c18, 6-appa-registers.c10, ch6-appa-masks.c9
//   MASK-BOTH-ON-ONE-WORD-ROW: 6-shifts-and-masks.c31, ch6-shifts.c27,
//     ch6-blockdiag.c4
//   FIG64-EDGE-MASKS: 6-shifts-and-masks.c33, 6-shifts-and-masks.c34,
//     6-shifts-and-masks.c35, 6-shifts-and-masks.c38, ch6-shifts.c19,
//     ch6-shifts.f2, ch6-shifts.f4, ch6-shifts.f8, ch6-shifts.f9
//   SECOND-WORKED-EXAMPLE-MASKS: ch6-shifts.c29, ch6-shifts.c30
//   ARITHMETIC-BITS-TO-LEAVE-ALONE: ch6-shifts.c28
//   ALT-SHIFT-SAME-BOUNDARY: ch6-shifts.c37, ch6-shifts.c38, ch6-shifts.c39
//   DESC-FETCH-ORDER-ADDRESS: 6-descending-mode.c7, ch6-desc.c8
//   DESC-FWM-ON-LAST-WORD: 6-descending-mode.c6, ch6-desc.c7
//   DESC-LWM-ON-FIRST-WORD: 6-descending-mode.c8, ch6-desc.c9
//   FILL-MASK-STAYS-ACTIVE: 6-appa-registers.c12
//   MASK-MUX-THREE-REGIMES: 6-blitter-block-diagram.c1, ch6-fig613.f03,
//     ch6-fig613.f04, ch6-fig613.f05
//   MASK-16-BITS-INDEPENDENT: 6-blitter-block-diagram.c2, ch6-fig613.f02,
//     ch6-fig613.f07
//   NOT ASSERTED (line mode, reason above): 6-appa-registers.c13,
//     ch6-line.c24, ch6-linesum.c2, ch6-linesum.c3
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean) and the FAIL/PASS lines quoted are what the run
// printed, not predictions.
//
// MUTATION DATA-DELIVERY stimulus red: fed C word at index 2 changed from
//   0x9ABC to 0x9ABD (expected constants left untouched) -> FAIL, the third
//   captured data_out no longer matches; every other assertion PASS.
// MUTATION MASK-REG-BLTAFWM-IDENTITY stimulus red: the sentinel written to
//   BLTAFWM changed from 0x1234 to 0x1235 (BLTALWM, a_word and the expected
//   constant all left untouched) -> FAIL, data_out no longer matches the
//   (unchanged) expected 0x1234; every other assertion PASS.
// MUTATION MASK-REG-BLTALWM-IDENTITY stimulus red: the sentinel written to
//   BLTALWM changed from 0x5678 to 0x5679 -> FAIL, data_out no longer
//   matches the unchanged expected 0x5678; every other assertion PASS.
// MUTATION ONLY-A-IS-MASKED stimulus red: the second fed C word changed
//   from 0x5678 to 0x5679 (expected array left untouched) -> FAIL, the
//   middle captured word no longer matches; every other assertion PASS.
// MUTATION MASK-BEFORE-SHIFT stimulus red: BLTAFWM changed from 0xFFF0 to
//   0xFFFF (the low nibble this block masks off is no longer masked) ->
//   FAIL, word 2 no longer matches the unchanged expected value; every
//   other assertion PASS.
// MUTATION MASK-BIT-EXCLUDED-FROM-LOGIC stimulus red: BLTAFWM and BLTALWM
//   both changed from 0x00FF to 0xFFFF (masking removed entirely; b_word
//   and the expected constant left untouched) -> FAIL, data_out reads
//   0xFFFF, not the unchanged expected 0x00FF; every other assertion PASS.
//   (A first attempt changed only b_word from 0x0000 to 0x00FF and stayed
//   green - vacuous, because that b_word happens to equal the expected
//   masked-A value already, so ORing it in changes nothing observable;
//   diagnosed and replaced with the mask-removal stimulus above, which
//   actually exercises what this assertion is about.)
// MUTATION MASK-FWM-EVERY-ROW stimulus red: height changed from 2 to 1
//   (only one row is blitted) -> FAIL, data_writes has only 2 entries, so
//   the row-2 index this assertion reads does not exist; every other
//   assertion PASS.
// MUTATION MASK-LWM-EVERY-ROW stimulus red: height changed from 2 to 1 ->
//   FAIL, same mechanism as above; every other assertion PASS.
// MUTATION MASK-BOTH-ON-ONE-WORD-ROW stimulus red: width changed from 1 to
//   2 (no longer a one-word row, so first_word and last_word are no longer
//   simultaneously true) -> FAIL, data_writes[0] is masked by BLTAFWM alone
//   (0xFF00), not the AND of both masks (0x0000); every other assertion
//   PASS.
// MUTATION FIG64-EDGE-MASKS stimulus red: BLTALWM changed from the figure's
//   0xFE00 to 0xFFFF (no clipping) -> FAIL, data_writes[1] no longer
//   matches the unchanged expected 0xFE00; every other assertion PASS.
// MUTATION SECOND-WORKED-EXAMPLE-MASKS stimulus red: BLTAFWM changed from
//   the worked-example value 0x07FF to 0xFFFF -> FAIL, data_writes[0] no
//   longer matches the unchanged expected 0x07FF; every other assertion
//   PASS.
// MUTATION ARITHMETIC-BITS-TO-LEAVE-ALONE stimulus red: the computed
//   quantity changed from (2*16 - 23 - 5) to (2*16 - 22 - 5) (one of the
//   page's own three numbers perturbed, expected 4 left untouched) -> FAIL,
//   5 != 4; every other assertion PASS.
// MUTATION ALT-SHIFT-SAME-BOUNDARY stimulus red: ash changed from 5 to 6
//   (BLTAFWM/BLTALWM and the fed words left untouched) -> FAIL, both
//   data_writes entries no longer match the ascending_shift(5, ...)
//   expected values; every other assertion PASS.
// MUTATION DESC-FETCH-ORDER-ADDRESS stimulus red: BLTCON1 changed from
//   descending (bit 1 set) to ascending (0x0000) -> FAIL, the two captured
//   A-fetch addresses now increase instead of decrease; every other
//   assertion PASS.
// MUTATION DESC-FWM-ON-LAST-WORD stimulus red: BLTAFWM changed from 0xFFF0
//   to 0xFFFF -> FAIL, data_writes[0] no longer matches the unchanged
//   expected 0xFFF0; every other assertion PASS.
// MUTATION DESC-LWM-ON-FIRST-WORD stimulus red: BLTALWM changed from
//   0xFFF0 to 0xFFFF -> FAIL, data_writes[1] no longer matches the
//   unchanged expected 0xFFF0; every other assertion PASS.
// MUTATION FILL-MASK-STAYS-ACTIVE stimulus red: the "test" run's BLTAFWM
//   changed from 0x0000 back to 0xFFFF (now identical to the control run)
//   -> FAIL, the two captured results are now equal instead of different;
//   every other assertion PASS.
// MUTATION MASK-MUX-THREE-REGIMES stimulus red: the center word's fed value
//   changed from 0x2222 to 0x2223 (expected constants left untouched) ->
//   FAIL, the center-word check no longer matches; the first- and
//   last-word checks in the same assertion still PASS on their own terms,
//   but the assertion as a whole (which requires all three) FAILs.
// MUTATION MASK-16-BITS-INDEPENDENT stimulus red: for k==7 only, an
//   ADDITIONAL bit (bit 3) cleared in the value actually written to
//   BLTAFWM, simulating a hypothetical crosstalk defect, while `expected`
//   for that iteration was left computed from the original single-bit-k
//   stimulus -> FAIL (only at k=7; every other bit position still PASS),
//   lanes_independent==15, not the unchanged expected 16; every other
//   assertion PASS.

#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t BLTCON0 = to_reg(0x040);
constexpr uint16_t BLTCON1 = to_reg(0x042);
constexpr uint16_t BLTAFWM = to_reg(0x044);
constexpr uint16_t BLTALWM = to_reg(0x046);
constexpr uint16_t BLTAPTL = to_reg(0x052);
constexpr uint16_t BLTADAT = to_reg(0x074);
constexpr uint16_t BLTBDAT = to_reg(0x072);
constexpr uint16_t BLTCDAT = to_reg(0x070);
constexpr uint16_t BLTSIZE = to_reg(0x058);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEB = 1u << 10;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

// BLTCON1 bits relevant here: bit 1 is descending mode (agnus_blitter.v:286,
// only when line/bit0 is clear), bit 3 is inclusive fill enable
// (agnus_blitter.v:288, likewise only outside line mode).
constexpr uint16_t BLTCON1_DESC = 1u << 1;
constexpr uint16_t BLTCON1_IFE = 1u << 3;

// Minterm LF settings (agnus_blitter_minterm.v): bit k of LF selects the
// (A,B,C) combination whose binary value is k, MSB-first as (A,B,C).
constexpr uint16_t LF_A_PASSTHROUGH = 0xF0;  // D = A, for every B/C value
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C, for every A/B value
constexpr uint16_t LF_A_OR_B = 0xFC;         // D = A | B, independent of C

// The ascending-mode shift formula, independently validated in
// hrm/agnus_blitter_barrelshifter (PKG-BLT-SHIFT, 4.2M-case sweep) and
// reused verbatim by hrm/agnus_blitter_shift_chain. Used here only where a
// shift is actually part of what is being measured (MASK-BEFORE-SHIFT,
// ALT-SHIFT-SAME-BOUNDARY); every other block below runs with shift=0.
constexpr uint16_t ascending_shift(uint16_t shift, uint16_t previous, uint16_t current) {
    return shift == 0
        ? current
        : static_cast<uint16_t>((static_cast<uint32_t>(previous) << (16u - shift)) |
                                 (static_cast<uint32_t>(current) >> shift));
}

// Copied in mechanism from hrm/agnus_blitter_status and
// hrm/agnus_blitter_shift_chain (both read in full before writing this
// file) - see this file's header for why every step is where it is.
class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->clkena = 1;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        data_writes.clear();
        a_fetch_addresses.clear();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint16_t addr, uint16_t data) { cycle(true, addr, data); tick_bookkeeping(); }

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    // Runs until `data_writes.size()` reaches `n`, or `max_ticks` is
    // exhausted. Returns true if the target was reached.
    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    std::deque<uint16_t> a_words, b_words, c_words;
    std::vector<uint16_t> data_writes;
    // address_out captured at the same settled moment as data_out,
    // whenever the grant was channel A's (reg_address_out==BLTADAT) - see
    // file header. Used only by the descending-mode blocks.
    std::vector<uint32_t> a_fetch_addresses;

private:
    Vagnus_blitter* dut;

    static uint16_t pop_or_zero(std::deque<uint16_t>& q) {
        if (q.empty()) return 0;
        uint16_t v = q.front();
        q.pop_front();
        return v;
    }

    bool last_we = false;
    uint16_t last_data_out = 0;

    void tick_bookkeeping() {
        if (last_we) data_writes.push_back(last_data_out);
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        last_we = false;
        last_data_out = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            if (reg == BLTADAT) v = pop_or_zero(a_words);
            else if (reg == BLTBDAT) v = pop_or_zero(b_words);
            else if (reg == BLTCDAT) v = pop_or_zero(c_words);
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the
            // combinational network BEFORE the clock edge - the step the
            // salvaged diag.cpp skipped (see hrm/agnus_blitter_status's
            // header). address_out is combinational off the currently
            // stored pointer register (agnus_blitter_adrgen.v:82-84), so it
            // is valid at this same settled moment.
            dut->eval();
            if (reg == BLTADAT) a_fetch_addresses.push_back(dut->address_out);
            if (dut->we) {
                last_we = true;
                last_data_out = dut->data_out;
            }
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = IDLE_REG;
            dut->data_in = 0;
        }
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
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

// BLTSIZE data word for OCS-style sizing: height in bits[15:6], width in
// bits[5:0] (agnus_blitter.v:521-525, 555-556).
constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

// Starts an ASCENDING, non-fill, non-line blit (BLTCON1=0x0000). Blocks that
// need descending mode or fill mode write BLTCON1 (and BLTAPTL, for
// descending) themselves instead of using this helper.
void start_blit(Harness& h, uint16_t bltcon0, uint16_t width, uint16_t height) {
    h.write_reg(BLTCON0, bltcon0);
    h.write_reg(BLTCON1, 0x0000);
    h.write_reg(BLTSIZE, bltsize_word(width, height));
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture self-check, no page) ----------------
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        start_blit(h, USEC | USED | LF_C_PASSTHROUGH, 4, 1);
        h.run_until_writes(4, 60);

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY: four distinct C words fed into the "
            "fixture arrive at data_out, in order - proves the DMA grant handshake "
            "actually delivers data (mechanism copied from hrm/agnus_blitter_status "
            "and hrm/agnus_blitter_shift_chain), which every data-dependent assertion "
            "below depends on");
    }

    // ---------------- MASK-REG-BLTAFWM-IDENTITY (p.170, p.180, p.199, p.266) ----------------
    {
        // A sentinel value (not a boolean bit pattern) written specifically
        // to BLTAFWM's own address ($044) and nowhere else; BLTALWM held
        // neutral (0xFFFF) so only BLTAFWM's effect is visible. width=1
        // makes the word both first and last, so bltamask = bltafwm &
        // bltalwm = bltafwm exactly (agnus_blitter.v:311-313).
        Harness h;
        h.a_words = {0xFFFF};
        h.write_reg(BLTAFWM, 0x1234);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 1, 1);
        h.run_until_writes(1, 60);

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x1234,
            "MASK-REG-BLTAFWM-IDENTITY: writing the sentinel 0x1234 specifically to "
            "custom-register offset $044 and nothing else changes source A's masked "
            "value to exactly 0x1234, proving BLTAFWM is a real, independently "
            "addressed, writable register whose value is what gets ANDed with source "
            "A on the first word - 'one has a first and last word mask register' "
            "(6-dma-channels.c11, p.170), 'the A channel can mask the first and last "
            "word of each row coming through' (6-shifts-and-masks.c23, ch6-shifts.c18, "
            "p.180), 'BLTAFWM and BLTALWM, for blitter A channel first and last word "
            "masks' (6-shifts-and-masks.c25, ch6-shifts.c20, p.180), 'the upper left "
            "corner shows how the first- and last-word masks are applied to the "
            "incoming A-source data' (ch6-blockdiag.c3, p.198), 'BLTAFWM 044 W A "
            "Blitter first-word mask for source A' (6-appa-registers.c1, "
            "6-appa-registers.c2, 6-appa-registers.c4, ch6-appa-masks.c4, p.266); "
            "Figure 6-13 draws 'FW Mask' as one of the boxes fed directly from the "
            "DATA BUS (ch6-fig613.f01, p.199) - this assertion confirms that box is "
            "BLTAFWM specifically, reached over reg_address_in/data_in the same bus "
            "BLTADAT ('A new') is written on in this same fixture; the other two "
            "boxes f01 names, B new and C hold (BLTBDAT/BLTCDAT), are full "
            "register-decode completeness outside this package's boundary "
            "(PKG-BLT-REGDECODE)");
    }

    // ---------------- MASK-REG-BLTALWM-IDENTITY (p.170, p.180, p.266) ----------------
    {
        Harness h;
        h.a_words = {0xFFFF};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0x5678);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 1, 1);
        h.run_until_writes(1, 60);

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x5678,
            "MASK-REG-BLTALWM-IDENTITY: writing the sentinel 0x5678 specifically to "
            "custom-register offset $046 and nothing else changes source A's masked "
            "value to exactly 0x5678, proving BLTALWM is a real, independently "
            "addressed, writable register distinct from BLTAFWM - 'one has a first "
            "and last word mask register' (6-dma-channels.c12, p.170), 'the A channel "
            "can mask the first and last word of each row' (6-shifts-and-masks.c24, "
            "ch6-shifts.c18, p.180), 'BLTAFWM and BLTALWM, for blitter A channel first "
            "and last word masks' (6-shifts-and-masks.c26, ch6-shifts.c20, p.180), "
            "'the upper left corner shows how the first- and last-word masks are "
            "applied to the incoming A-source data' (ch6-blockdiag.c3, p.198), "
            "'BLTALWM 046 W A Blitter last-word mask for source A' "
            "(6-appa-registers.c5, 6-appa-registers.c6, 6-appa-registers.c8, "
            "ch6-appa-masks.c8, p.266)");
    }

    // ---------------- ONLY-A-IS-MASKED (p.170) ----------------
    {
        // Channel C run through the same fixture, with BOTH A-channel mask
        // registers written to the most aggressive clipping value possible
        // (0x0000, which would force an all-zero result on every word if it
        // applied). BLTAFWM/BLTALWM exist only for the A path
        // (agnus_blitter.v:150-151, 293-319) - no equivalent register or
        // multiplexer exists for B, C or D - so C's data must pass through
        // completely unaffected regardless of what these registers hold.
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC};
        h.write_reg(BLTAFWM, 0x0000);
        h.write_reg(BLTALWM, 0x0000);
        start_blit(h, static_cast<uint16_t>(USEC | USED | LF_C_PASSTHROUGH), 3, 1);
        h.run_until_writes(3, 60);

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC};
        checks.expect(h.data_writes == expected,
            "ONLY-A-IS-MASKED: with BLTAFWM=BLTALWM=0x0000 (the value that would zero "
            "every bit if it applied), a 3-word channel-C row still delivers all three "
            "raw words completely unmasked, at the first, middle and last position "
            "alike - 'one blitter DMA channel has a first and last word mask "
            "register' (6-dma-channels.c11, 6-dma-channels.c12, ch6-dma.c9, p.170) "
            "names exactly ONE of the four channels, and this is the direct negative "
            "half of that claim: no equivalent register or mask path exists on C");
    }

    // ---------------- MASK-BEFORE-SHIFT (p.180) ----------------
    {
        // BLTAFWM masks off only word 0's low nibble (0xFFF0, not 0xFFFF).
        // bltaold is loaded from bltadat & bltamask (agnus_blitter.v:337) -
        // the AND happens BEFORE the value is captured for the next word's
        // shift-in, not after. If masking happened after the shift instead,
        // the masked-off bits would still leak into word 2's shift-in via
        // an unmasked old_val. BLTALWM is left at 0xFFFF (word 2 is also
        // this 2-word blit's last word) so the OTHER mask does not confound
        // this one.
        Harness h;
        h.a_words = {0xFFFF, 0x0000};
        h.write_reg(BLTAFWM, 0xFFF0);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>((4u << 12) | USEA | USED | LF_A_PASSTHROUGH), 2, 1);
        h.run_until_writes(2, 60);

        const uint16_t masked_word0 = 0xFFFF & 0xFFF0;                       // 0xFFF0
        const uint16_t expected = ascending_shift(4, masked_word0, 0x0000);  // 0x0000
        checks.expect(h.data_writes.size() == 2 && h.data_writes[1] == expected,
            "MASK-BEFORE-SHIFT: word 2 reads 0x0000, not 0xF000 - the low nibble "
            "BLTAFWM masked off word 0's raw value (0xFFFF -> 0xFFF0) does NOT "
            "survive into word 2's shift-in, proving the mask is applied to the "
            "source data before it enters the shift chain, not after - 'these masks "
            "are \"anded\" with the source data, before any shifts are [applied]' "
            "(6-shifts-and-masks.c27, ch6-shifts.c22, ch6-shifts.c23, p.180)");
    }

    // ---------------- MASK-BIT-EXCLUDED-FROM-LOGIC (p.180, p.181, p.266) ----------------
    {
        // width=1 (first==last, so bltamask=bltafwm&bltalwm=0x00FF exactly
        // with both written to 0x00FF). a_word is raw all-ones, so the
        // masked-off high byte would still read as 1 if the mask forced it
        // only "toward" zero rather than out of the logic function - and
        // LF=A|B (0xFC, independent of C) means if a masked-off A bit still
        // "participated" as its raw value, the OR would still read 1 there.
        // b_word=0x0000 contributes nothing, so ANY 1 bit in the high byte
        // of the result can only have come from A having genuinely
        // participated there.
        Harness h;
        h.a_words = {0xFFFF};
        h.b_words = {0x0000};
        h.write_reg(BLTAFWM, 0x00FF);
        h.write_reg(BLTALWM, 0x00FF);
        start_blit(h, static_cast<uint16_t>(USEA | USEB | USED | LF_A_OR_B), 1, 1);
        h.run_until_writes(1, 60);

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x00FF,
            "MASK-BIT-EXCLUDED-FROM-LOGIC: ORing masked source A (raw 0xFFFF, mask "
            "0x00FF) with B=0x0000 reads 0x00FF, not 0xFFFF - the high byte, despite "
            "A's raw bits there being 1, contributes nothing to the OR, so a "
            "zero-mask bit does not merely force a 0 into an AND, it removes that bit "
            "of A from the logic operation entirely - 'only when there is a 1 bit in "
            "the first-word mask will that bit [of source A be used]' "
            "(6-shifts-and-masks.c28, ch6-shifts.c24, p.180), 'source is passed "
            "through mask when it is a one, otherwise the destination is copied' "
            "(6-shifts-and-masks.c36, 6-shifts-and-masks.c37, 6-shifts-and-masks.c39, "
            "ch6-shifts.f6, ch6-shifts.f7, Figure 6-4, p.181), 'a zero in any bit [of "
            "the mask overrides data from source A]' (ch6-appa-masks.c10, "
            "6-appa-registers.c11, p.266)");
    }

    // ---------------- MASK-FWM-EVERY-ROW (p.180, p.266) ----------------
    {
        // 2 rows x 2 words. All four raw A words are identical (0xFFFF) so
        // any masking difference between positions is directly visible.
        // BLTAFWM=0xFFF0 (partial), BLTALWM=0xFFFF (neutral, isolates FWM).
        // width_cnt_rld fires at every row boundary (agnus_blitter.v:528),
        // so first_word becomes true again at the start of row 2 - the
        // question this assertion answers is whether BLTAFWM actually
        // re-applies there, or only ever masked the blit's very first word.
        Harness h;
        h.a_words = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
        h.write_reg(BLTAFWM, 0xFFF0);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 2, 2);
        h.run_until_writes(4, 200);

        checks.expect(h.data_writes.size() == 4 &&
                       h.data_writes[0] == 0xFFF0 && h.data_writes[2] == 0xFFF0,
            "MASK-FWM-EVERY-ROW: BOTH row 1's first word (index 0) and row 2's first "
            "word (index 2) read 0xFFF0 (masked) - BLTAFWM re-applies at the start of "
            "EVERY row, not only the blit's first word overall - 'the first word of "
            "each row is anded with BLTAFWM' (6-shifts-and-masks.c29, ch6-shifts.c25, "
            "ch6-shifts.c18, p.180), 'the patterns in [BLTAFWM] are anded with the "
            "first ... word[s] of each line of data from source A' "
            "(6-appa-registers.c9, ch6-appa-masks.c9, p.266)");
    }

    // ---------------- MASK-LWM-EVERY-ROW (p.180, p.266) ----------------
    {
        Harness h;
        h.a_words = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFF0);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 2, 2);
        h.run_until_writes(4, 200);

        checks.expect(h.data_writes.size() == 4 &&
                       h.data_writes[1] == 0xFFF0 && h.data_writes[3] == 0xFFF0,
            "MASK-LWM-EVERY-ROW: BOTH row 1's last word (index 1) and row 2's last "
            "word (index 3) read 0xFFF0 (masked) - BLTALWM re-applies at the end of "
            "EVERY row - 'the last word is anded with BLTALWM' "
            "(6-shifts-and-masks.c30, ch6-shifts.c26, ch6-shifts.c18, p.180), 'the "
            "patterns in [BLTALWM] are anded with the ... last word[s] of each line "
            "of data from source A' (6-appa-registers.c10, ch6-appa-masks.c9, "
            "p.266)");
    }

    // ---------------- MASK-BOTH-ON-ONE-WORD-ROW (p.180, p.198) ----------------
    {
        // width=1: the one word is simultaneously first_word and last_word,
        // so agnus_blitter.v:312-313 selects bltafwm & bltalwm. Values
        // chosen with NO overlapping 1-bits (0xFF00, 0x00FF) so the AND is
        // 0x0000 - a result neither mask alone would produce, proving both
        // are genuinely combined rather than only one winning.
        Harness h;
        h.a_words = {0xFFFF};
        h.write_reg(BLTAFWM, 0xFF00);
        h.write_reg(BLTALWM, 0x00FF);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 1, 1);
        h.run_until_writes(1, 60);

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x0000,
            "MASK-BOTH-ON-ONE-WORD-ROW: with BLTAFWM=0xFF00 and BLTALWM=0x00FF (no "
            "bit set in both), the one word of a one-word row reads 0x0000 - neither "
            "mask value alone, proving both masks are ANDed together, not just one "
            "applied - 'if the width of the row is a single word, both masks are "
            "applied' (6-shifts-and-masks.c31, ch6-shifts.c27, p.180), 'when the blit "
            "shrinks to one word wide, both masks are applied' (ch6-blockdiag.c4, "
            "p.198)");
    }

    // ---------------- FIG64-EDGE-MASKS (p.181) ----------------
    {
        // Figure 6-4's own labelled values: first-word mask "11111111
        // 11111111" (0xFFFF, f4/c34 - no clipping on that edge) and the
        // figure's second/last-word mask "11111110 00000000" (0xFE00,
        // c35/f2 - clips the rightmost 9 bits). Raw A words are all-ones so
        // the clip pattern is directly visible on data_out.
        Harness h;
        h.a_words = {0xFFFF, 0xFFFF};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFE00);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 2, 1);
        h.run_until_writes(2, 60);

        const std::vector<uint16_t> expected = {0xFFFF, 0xFE00};
        checks.expect(h.data_writes == expected,
            "FIG64-EDGE-MASKS: with the first-word mask Figure 6-4 draws as all ones "
            "(0xFFFF - 'extract a 23-bit image', 6-shifts-and-masks.c33, "
            "6-shifts-and-masks.c34, ch6-shifts.f2, ch6-shifts.f4, p.181) word 1 is "
            "unclipped (0xFFFF), and with the figure's own second-word mask "
            "'11111110 00000000' (0xFE00, 6-shifts-and-masks.c35, p.181) word 2 is "
            "clipped to exactly that pattern (0xFE00) - demonstrating the independent "
            "per-edge clipping that lets 'rectangular data [be extracted] from a "
            "source whose right and left edges are between word boundaries' "
            "(ch6-shifts.c19, p.180); the figure's D-points-at-C-address setup "
            "(6-shifts-and-masks.c38, ch6-shifts.f9, p.181) and its all-ones "
            "'destination before blit' block (ch6-shifts.f8, p.181) are the figure's "
            "read-modify-write CONTEXT for combining a masked A with an unmasked "
            "destination via a chosen minterm - a programming technique built from "
            "the masking this assertion proves (here) and the minterm selection "
            "PKG-BLT-MINTERM proves, not a separate hardware primitive of its own");
    }

    // ---------------- SECOND-WORKED-EXAMPLE-MASKS (p.181) ----------------
    {
        // The page's other worked mask pair, independent of Figure 6-4:
        // "$07FF" (most significant five bits zero) as a first-word mask
        // and "$FFF0" (least significant four bits zero) as a last-word
        // mask.
        Harness h;
        h.a_words = {0xFFFF, 0xFFFF};
        h.write_reg(BLTAFWM, 0x07FF);
        h.write_reg(BLTALWM, 0xFFF0);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 2, 1);
        h.run_until_writes(2, 60);

        const std::vector<uint16_t> expected = {0x07FF, 0xFFF0};
        checks.expect(h.data_writes == expected,
            "SECOND-WORKED-EXAMPLE-MASKS: writing the page's own worked values "
            "produces exactly the bit patterns their prose describes - 'a first word "
            "mask with the most significant five bits set to zero ($07FF)' "
            "(ch6-shifts.c29, p.181) applied to an all-ones word reads 0x07FF, and 'a "
            "last word mask with the least significant four bits set to zero "
            "($FFF0)' (ch6-shifts.c30, p.181) applied to an all-ones word reads "
            "0xFFF0");
    }

    // ---------------- ARITHMETIC-BITS-TO-LEAVE-ALONE (p.181) ----------------
    {
        // Pure arithmetic from the page's own worked numbers, not an RTL
        // measurement - flagged as such. "2 times 16, less 23, less 5" is
        // the page's own computation of how many trailing bits of the last
        // word are left unmasked in its worked 23-bit extraction.
        const int bits_to_leave_alone = 2 * 16 - 23 - 5;
        checks.expect(bits_to_leave_alone == 4,
            "ARITHMETIC-BITS-TO-LEAVE-ALONE [not an RTL measurement - the page's own "
            "worked arithmetic]: 2 times 16, less 23, less 5, equals 4 - 'the number "
            "of bits to leave alone at the end of the last word is 2 times 16, less "
            "23, less 5' (ch6-shifts.c28, p.181)");
    }

    // ---------------- ALT-SHIFT-SAME-BOUNDARY (p.182) ----------------
    {
        // The page's alternative configuration for the same extraction: "an
        // A shift of five, a first word mask of all ones, and a last word
        // mask with the rightmost nine bits set to zero". Expected values
        // computed via the same already-validated ascending_shift formula
        // used throughout this file and independently proven in
        // PKG-BLT-SHIFT, not hand-derived, to avoid arithmetic error.
        Harness h;
        h.a_words = {0xABCD, 0x1234};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFE00);  // rightmost nine bits zero
        start_blit(h, static_cast<uint16_t>((5u << 12) | USEA | USED | LF_A_PASSTHROUGH), 2, 1);
        h.run_until_writes(2, 60);

        const uint16_t expected_word1 = ascending_shift(5, 0x0000, 0xABCD);
        const uint16_t expected_word2 = ascending_shift(5, 0xABCD, static_cast<uint16_t>(0x1234 & 0xFE00));
        checks.expect(h.data_writes.size() == 2 &&
                       h.data_writes[0] == expected_word1 && h.data_writes[1] == expected_word2,
            "ALT-SHIFT-SAME-BOUNDARY: programming the page's alternative register "
            "values - ash=5, first word mask all ones, last word mask with the "
            "rightmost nine bits zero - 'use an A shift of five, a first word mask "
            "of all ones, and a last word mask with the rightmost nine bits set to "
            "zero' (ch6-shifts.c37, ch6-shifts.c38, ch6-shifts.c39, p.182) produces "
            "exactly the mask-then-shift result the same already-validated formula "
            "predicts for this different (shift, mask) pair, confirming this "
            "alternative configuration is a real, working combination and not just "
            "descriptive prose");
    }

    // ---------------- DESC-FETCH-ORDER-ADDRESS (p.182) ----------------
    {
        // Descending mode (BLTCON1 bit 1), 2-word row. BLTAPTL programmed
        // to a small nonzero starting address so two decrements stay
        // positive. first_word/last_word are purely width_cnt position
        // (agnus_blitter.v:539-540), independent of desc - what changes
        // with desc is which direction the pointer steps
        // (agnus_blitter.v:674-675, incptr=~desc, decptr=desc). This
        // assertion establishes the address FACT the other two descending
        // assertions below rely on: the word fetched FIRST is at the
        // HIGHER address.
        Harness h;
        h.a_words = {0x1111, 0x2222};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTAPTL, 0x0020);
        h.write_reg(BLTCON0, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH));
        h.write_reg(BLTCON1, BLTCON1_DESC);
        h.write_reg(BLTSIZE, bltsize_word(2, 1));
        h.run_until_writes(2, 60);

        checks.expect(h.a_fetch_addresses.size() == 2 &&
                       h.a_fetch_addresses[0] == h.a_fetch_addresses[1] + 1,
            "DESC-FETCH-ORDER-ADDRESS: in descending mode the FIRST A fetch happens "
            "at the HIGHER address and the second at address-minus-one - the pointer "
            "steps backward from wherever it starts, so the row's last (highest "
            "address) word is fetched before its first (lowest address) word - "
            "'the last word mask masks the first word in a row (which is still the "
            "first word fetched), and the last word mask masks the first word in a "
            "row' (6-descending-mode.c7, ch6-desc.c8, p.182)");
    }

    // ---------------- DESC-FWM-ON-LAST-WORD (p.182) ----------------
    {
        // Same descending 2-word setup. BLTAFWM partial (0xFFF0), BLTALWM
        // neutral (0xFFFF) so only BLTAFWM's effect is visible.
        // first_word fires on the FIRST fetch regardless of desc
        // (agnus_blitter.v:540), and DESC-FETCH-ORDER-ADDRESS above (and
        // reconfirmed here) shows that first fetch is the row's highest
        // address - i.e. positionally the row's LAST word.
        Harness h;
        h.a_words = {0xFFFF, 0xFFFF};
        h.write_reg(BLTAFWM, 0xFFF0);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTAPTL, 0x0020);
        h.write_reg(BLTCON0, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH));
        h.write_reg(BLTCON1, BLTCON1_DESC);
        h.write_reg(BLTSIZE, bltsize_word(2, 1));
        h.run_until_writes(2, 60);

        const bool higher_address_first =
            h.a_fetch_addresses.size() == 2 && h.a_fetch_addresses[0] > h.a_fetch_addresses[1];
        checks.expect(higher_address_first &&
                       h.data_writes.size() == 2 &&
                       h.data_writes[0] == 0xFFF0 && h.data_writes[1] == 0xFFFF,
            "DESC-FWM-ON-LAST-WORD: the word fetched at the HIGHER address (the row's "
            "last word, positionally) is the one BLTAFWM masks (0xFFF0), while the "
            "lower-address word is unmasked - 'the first word mask masks the last "
            "word in a row (which is still the first word fetched)' "
            "(6-descending-mode.c6, ch6-desc.c7, p.182)");
    }

    // ---------------- DESC-LWM-ON-FIRST-WORD (p.182) ----------------
    {
        Harness h;
        h.a_words = {0xFFFF, 0xFFFF};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFF0);
        h.write_reg(BLTAPTL, 0x0020);
        h.write_reg(BLTCON0, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH));
        h.write_reg(BLTCON1, BLTCON1_DESC);
        h.write_reg(BLTSIZE, bltsize_word(2, 1));
        h.run_until_writes(2, 60);

        const bool higher_address_first =
            h.a_fetch_addresses.size() == 2 && h.a_fetch_addresses[0] > h.a_fetch_addresses[1];
        checks.expect(higher_address_first &&
                       h.data_writes.size() == 2 &&
                       h.data_writes[0] == 0xFFFF && h.data_writes[1] == 0xFFF0,
            "DESC-LWM-ON-FIRST-WORD: the word fetched at the LOWER address (the row's "
            "first word, positionally) is the one BLTALWM masks (0xFFF0), while the "
            "higher-address word is unmasked - 'the last word mask masks the first "
            "word in a row' (6-descending-mode.c8, ch6-desc.c9, p.182)");
    }

    // ---------------- FILL-MASK-STAYS-ACTIVE (p.266) ----------------
    {
        // Two otherwise-identical single-word, inclusive-fill-mode runs
        // (BLTCON1 bit 3), differing only in BLTAFWM. If masking were
        // bypassed or ignored during fill mode, BLTAFWM's value could not
        // change the result. LF=A-passthrough routes A directly into the
        // minterm generator regardless of B/C, and the fill logic's own
        // bypass (agnus_blitter_fill.v:37-38, ife=efe=0 -> out=in) is NOT
        // relied on here since ife=1 - the fill carry math itself is not
        // predicted or asserted (PKG-BLT-FILL's job), only that changing
        // the mask changes the result at all.
        uint16_t control_result = 0xDEAD, test_result = 0xBEEF;
        {
            Harness h;
            h.a_words = {0x1234};
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTCON0, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH));
            h.write_reg(BLTCON1, BLTCON1_IFE);
            h.write_reg(BLTSIZE, bltsize_word(1, 1));
            if (h.run_until_writes(1, 60)) control_result = h.data_writes[0];
        }
        {
            Harness h;
            h.a_words = {0x1234};
            h.write_reg(BLTAFWM, 0x0000);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTCON0, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH));
            h.write_reg(BLTCON1, BLTCON1_IFE);
            h.write_reg(BLTSIZE, bltsize_word(1, 1));
            if (h.run_until_writes(1, 60)) test_result = h.data_writes[0];
        }
        checks.expect(control_result != test_result,
            "FILL-MASK-STAYS-ACTIVE: with inclusive fill mode active, changing "
            "BLTAFWM from all-ones (0xFFFF) to fully-clipping (0x0000) changes the "
            "stored result - the mask is not bypassed or ignored while fill mode is "
            "active, which is exactly why '[BLTAFWM/BLTALWM] should be set to all 1s "
            "for fill mode' (6-appa-registers.c12, p.266): leaving them at their "
            "post-reset default of 0 would clip fill-mode data the same way it clips "
            "an ordinary blit's edge words");
    }

    // ---------------- MASK-MUX-THREE-REGIMES (p.199, Figure 6-13) ----------------
    // Overturned by Prompt 5's attack from a figure-geometry discard;
    // asserted here directly against the RTL it depicts, not against the
    // picture (see file header). agnus_blitter.v:311-319 is a genuine
    // 3-way (4-branch, with the both-true case folded in) multiplexer:
    // bltafwm alone on first_word, bltalwm alone on last_word, bltafwm &
    // bltalwm when both, 0xFFFF (pass-through) otherwise - and line 345
    // ANDs whichever value that selects into the shifter's input. A 3-word
    // row exercises all three single-condition branches on one blit.
    {
        Harness h;
        h.a_words = {0x1111, 0x2222, 0x3333};
        h.write_reg(BLTAFWM, 0xF0F0);
        h.write_reg(BLTALWM, 0x0F0F);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 3, 1);
        h.run_until_writes(3, 60);

        const std::vector<uint16_t> expected = {0x1010, 0x2222, 0x0303};
        checks.expect(h.data_writes == expected,
            "MASK-MUX-THREE-REGIMES: word 1 (first, masked by BLTAFWM alone) reads "
            "0x1010, word 2 (center, masked by neither) reads 0x2222 unchanged, and "
            "word 3 (last, masked by BLTALWM alone) reads 0x0303 - three genuinely "
            "distinct regimes, matching Figure 6-13's drawn structure, which is the "
            "mask multiplexer agnus_blitter.v:311-319 and the AND into the shifter "
            "input agnus_blitter.v:345 (this is the RTL the figure depicts, asserted "
            "directly rather than as figure geometry - see file header): 'FW Mask and "
            "the A source combined in an AND gate enabled by \"first word time\"' "
            "(ch6-fig613.f03), 'the A source alone passes an AND gate enabled by "
            "\"center words time\"' (ch6-fig613.f04), 'LW Mask and the A source "
            "combined in an AND gate enabled by \"last word time\"' (ch6-fig613.f05, "
            "p.199); this is exactly the routing 6-blitter-block-diagram.c1 restates "
            "as 'Figure 6-13's upper-left labels route FW Mask, A new, and LW Mask "
            "through first-word, center-word, and last-word AND gates respectively' "
            "(p.199)");
    }

    // ---------------- MASK-16-BITS-INDEPENDENT (p.199, Figure 6-13) ----------------
    // Also overturned by Prompt 5's attack (same figure-geometry reasoning
    // corrected in the header), asserted directly against
    // agnus_blitter.v:337 (bltadat & bltamask stored into bltaold every
    // word) and :345 (the same AND feeding the shifter). One bit of
    // BLTAFWM cleared at a time, width=1 so bltamask=bltafwm exactly; if
    // the sixteen bit lanes were not independent, clearing one bit could
    // affect others.
    {
        int lanes_independent = 0;
        for (int k = 0; k < 16; ++k) {
            Harness h;
            h.a_words = {0xFFFF};
            const uint16_t fwm = static_cast<uint16_t>(~(1u << k));
            h.write_reg(BLTAFWM, fwm);
            h.write_reg(BLTALWM, 0xFFFF);
            start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 1, 1);
            h.run_until_writes(1, 60);
            const uint16_t expected = fwm;  // 0xFFFF & fwm == fwm
            if (h.data_writes.size() == 1 && h.data_writes[0] == expected) ++lanes_independent;
        }
        checks.expect(lanes_independent == 16,
            "MASK-16-BITS-INDEPENDENT: clearing exactly one bit of BLTAFWM at a time "
            "(all 16 positions swept) clears exactly that one bit of the masked "
            "result and no other, every time - the mask is sixteen independent "
            "bit-lanes, not a shared/aliased structure, matching 'sixteen parallel "
            "mask blocks feeding an OR, into A-old' (6-blitter-block-diagram.c2, "
            "p.199), 'each of \"FW Mask\", \"A new\" and \"LW Mask\" drives a 16-wide "
            "bus, marked with a slash and the numeral 16' (ch6-fig613.f02, p.199), "
            "and 'the mask block is annotated \"16 BLOCKS\": the drawn logic is one "
            "bit slice replicated sixteen times' (ch6-fig613.f07, p.199), asserted "
            "directly against agnus_blitter.v:337 (the masked value stored into "
            "bltaold every word) and :345 (the same AND feeding the shifter), not "
            "against the picture");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
