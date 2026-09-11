// Test purpose: Verify Copper WAIT/SKIP comparison semantics - greater-or-equal, compare-enable masks, the unmaskable top bit, BFD and counter rollover.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed pp. 22-23, 27, 30, 31, 32 and 33; frozen specification 2e6a42a.
// Requirement IDs: COP-04, COP-08
// Non-goals: Bus contention (needs the Agnus top level), exact release cycle counts, NTSC long/short line alternation, and interrupt generation are out of scope. No independent ECS WAIT/SKIP claim is made.
//
// MUTATION W1 stimulus red: WAIT target retargeted from line $96 to $10 (already passed) -> the "does not run
//   before line $96" check goes FAIL as expected. A second run retargeted it to $FF (unreachable in the run
//   budget) -> both W1 checks FAIL as expected.
// MUTATION W2 stimulus red: WAIT target moved from $10 (already-passed) to $90 (unreachable in the 2000-cycle
//   budget) -> FAIL as expected.
// MUTATION W3 stimulus red: the second WAIT's target moved from $3C (identical to the first) to $50
//   (unreachable in the run budget) -> FAIL as expected (the COLOR01 MOVE never runs).
// MUTATION W4a stimulus red: the fixture's run shortened from 20000 cycles to 200,
//   so it no longer drives a full line -> 43 passed, 1 failed, W4a the only
//   assertion that moves. W4b stays green (a smaller hpos is still below $7F) and
//   W4 stays green (the MOVE still never runs). Reverted: 44 passed, 0 failed.
// MUTATION W4b none impossible: W4b asserts that the HP comparison field reaches
//   only $6E on this fixture, below the terminal WAIT's $7F. The bound comes from
//   the LINE LENGTH -- 454 counts, hpos[8:2] -- and no stimulus in this package
//   varies it. The attempt is recorded rather than the conclusion alone: the run
//   was lengthened from 20000 cycles to 200000, ten times over, and the result was
//   44 passed 0 failed, unchanged, because hpos wraps every line and the maximum
//   does not grow. Lengthening the line needs a BEAMCON0/HTOTAL write, which this
//   package's harness does not offer.
// MUTATION W4 stimulus red: the terminal $FFFF/$FFFE sentinel replaced with an ordinary always-true WAIT
//   ($00,$00 fully masked) -> FAIL as expected, confirming the check does notice a release when one occurs.
// MUTATION W5 stimulus red: masked target nibble changed from $0F to $03 -> FAIL as expected (release now
//   happens on a line ending in $3, not $F).
// MUTATION W6 stimulus red: seek_beam moved from $80 down to $20 (before the unmaskable top bit is set) ->
//   FAIL as expected (now behaves like W5: release only on a line ending in $F).
// MUTATION W7 stimulus red: WAIT confined to the same line (VP target lowered from $41 to $40, matching the
//   start position) with HP moved to $7F and the run budget cut from 6000 to 300 cycles -> FAIL as expected,
//   proving the horizontal compare is not simply being skipped.
// MUTATION S1 stimulus red: SKIP bit (IR2 bit0) cleared, turning the instruction into a WAIT -> the "next MOVE
//   is skipped" check FAILs as expected. A second run kept the SKIP bit but retargeted the follow-up MOVE from
//   COLOR01 to COLOR00 -> both S1 checks FAIL as expected.
// MUTATION S2 stimulus red: SKIP target moved from $64 (future, condition false) to $00 (already passed,
//   condition true) -> FAIL as expected.
// MUTATION S3 stimulus red: SKIP target's VP changed from $70 to $F0, setting the unmaskable top bit while the
//   beam starts at VP=0 -> FAIL as expected (the top bit is still compared even with every VE/HE bit clear, so
//   "always true" no longer holds within the run budget) - this is the same top-bit gotcha the file's own
//   comment warns about for $FF.
// MUTATION S4 stimulus red: IR2 bit0 set (turning the WAIT into a SKIP) with the already-true position ->
//   FAIL as expected.
// MUTATION B2s stimulus red: BFD flipped from 0 to 1 in the SKIP, so the busy blitter no longer holds the
//   condition back -> FAIL as expected (the position alone satisfies the SKIP and the MOVE is suppressed).
// MUTATION B1 stimulus red: BFD flipped from 1 to 0 while the blitter is busy -> FAIL as expected (the WAIT is
//   now held, per B2).
// MUTATION B2 stimulus red: blitter_busy changed from true to false -> FAIL as expected (nothing holds the
//   WAIT, so it releases immediately).
// MUTATION B3 stimulus red: the blitter is never actually un-busied (set_blitter_busy(false) changed to
//   set_blitter_busy(true)) -> FAIL as expected (the WAIT never releases).
// MUTATION W8 stimulus red: VP target moved from $50 (reachable) to $A0 (unreachable in the 12000-cycle
//   budget) -> FAIL as expected.

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPINS = to_reg(0x08C);
constexpr uint16_t IDLE_REG = 0x0FF;

constexpr uint16_t COLOR00 = to_reg(0x180);
constexpr uint16_t COLOR01 = to_reg(0x182);

constexpr uint32_t LIST_ADDRESS = 0x00002000;

// Instruction word helpers, straight from the HRM encoding tables.
// WAIT/SKIP IR1: bit0=1, bits15-8 = VP, bits7-1 = HP.
constexpr uint16_t ir1_pos(uint8_t vp, uint8_t hp) {
    return static_cast<uint16_t>((vp << 8) | ((hp & 0x7F) << 1) | 1);
}
// IR2: bit15 = BFD, bits14-8 = VE, bits7-1 = HE, bit0 = 0 for WAIT / 1 for SKIP.
constexpr uint16_t ir2_mask(bool bfd, uint8_t ve, uint8_t he, bool skip) {
    return static_cast<uint16_t>((bfd ? 0x8000 : 0) | ((ve & 0x7F) << 8) | ((he & 0x7F) << 1) | (skip ? 1 : 0));
}
// A MOVE is IR1 = register byte address (bit 0 clear), IR2 = data.
constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

struct MoveEvent {
    uint16_t reg;
    uint16_t data;
    uint16_t vpos;
    uint16_t hpos;
};

class CopperHarness {
public:
    CopperHarness() : dut(new Vagnus_copper) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->sof = 0;
        dut->blit_busy = 0;
        dut->hpos = 0;
        dut->vpos = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 12; ++i) cycle();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) cycle();
    }

    ~CopperHarness() { dut->final(); delete dut; }
    CopperHarness(const CopperHarness&) = delete;
    CopperHarness& operator=(const CopperHarness&) = delete;

    void install(const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) {
            memory[LIST_ADDRESS + i] = words[i];
        }
    }

    void seek_beam(uint16_t target_vpos) {
        vpos = target_vpos;
        hpos = 0;
        dut->vpos = vpos;
        dut->hpos = hpos;
    }

    void set_blitter_busy(bool busy) { dut->blit_busy = busy ? 1 : 0; }

    void start_list() {
        write_reg(COP1LCH, static_cast<uint16_t>((LIST_ADDRESS >> 15) & 0x1F));
        write_reg(COP1LCL, static_cast<uint16_t>((LIST_ADDRESS & 0x7FFF) << 1));
        write_reg(COPJMP1, 0x0000);
        run_cycles(4);
        dma_allowed = true;
    }

    void run_cycles(int count) { for (int i = 0; i < count; ++i) cycle(); }

    int move_count(uint16_t reg) const {
        int n = 0;
        for (const auto& e : moves) if (e.reg == reg) ++n;
        return n;
    }

    // vpos at which the first write to `reg` was observed, or -1.
    int first_move_vpos(uint16_t reg) const {
        for (const auto& e : moves) if (e.reg == reg) return static_cast<int>(e.vpos);
        return -1;
    }

    // The horizontal counterpart, for the HP-field walk. The event already
    // carried hpos; nothing read it until 06.09.2026.
    int first_move_hpos(uint16_t reg) const {
        for (const auto& e : moves) if (e.reg == reg) return static_cast<int>(e.hpos);
        return -1;
    }

    bool dma_allowed = false;
    std::vector<MoveEvent> moves;

    // How far this fixture actually drove the beam. Without these two numbers W4
    // can be asserted but not justified: everything there turns on which values
    // hpos and vpos could reach at all.
    uint16_t max_hpos_driven = 0;
    uint16_t max_vpos_driven = 0;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0;
    uint16_t vpos = 0;

    uint16_t read_memory(uint32_t word_address) const {
        const auto it = memory.find(word_address);
        return it == memory.end() ? 0xFFFF : it->second;
    }

    void write_reg(uint16_t word_reg, uint16_t data) { cycle(true, word_reg, data); }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        const bool accepted_dma = !cpu_write && dma_allowed && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (accepted_dma) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            const uint16_t data = read_memory(address);
            dut->ackdma = 1;
            dut->reg_address_in = reg;
            dut->data_in = data;
            dut->eval();
            if (reg != COPINS && reg != IDLE_REG) {
                moves.push_back({reg, data, vpos, hpos});
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
        if (++hpos == 454) {
            hpos = 0;
            vpos = static_cast<uint16_t>((vpos + 1) & 0x00FF);
        }
        dut->hpos = hpos;
        dut->vpos = vpos;
        if (hpos > max_hpos_driven) max_hpos_driven = hpos;
        if (vpos > max_vpos_driven) max_vpos_driven = vpos;
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // Prints a record for a claim this file does NOT bind to any assertion
    // here. Never touches passed/failed - this is not an assertion.
    void note(const std::string& label) { std::cout << "NOTE: " << label << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- WAIT (COP-04) ----------------

    // W1 - HRM p.22 example: WAIT $9601,$FF00 waits for line $96 with the
    // horizontal position masked off. ($FF00 has BFD=1, VE=$7F, HE=0.)
    {
        CopperHarness h;
        h.seek_beam(0x90);
        h.install({ir1_pos(0x96, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                   move_ir1(COLOR00), 0x0111,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(6000);
        const int at = h.first_move_vpos(COLOR00);
        checks.expect(h.move_count(COLOR00) == 1, "W1 WAIT line $96, HP masked: the following MOVE runs "
                      "[claims: COP2-about-the-copper.c11, COP2-what-is-a-copper-instruction.c3, "
                      "COP2-the-wait-instruction.c5, ch2-about-the-copper.c12, ch2-about-the-copper.c18, ch2-copper-loops-and-branches.c5, ch2-the-wait-instruction.c15, ch2-the-wait-instruction.c4, ch2-what-is-a-copper-instruction.c3]");
        checks.expect(at >= 0x96, "W1 WAIT line $96: the MOVE does not run before line $96 "
                      "[claims: COP2-copper-loops-and-branches-and-comparison-enable.c9]");
    }

    // W2 - p.27: greater-or-equal, so a WAIT for an already-passed position
    // completes immediately instead of locking up.
    {
        CopperHarness h;
        h.seek_beam(0x50);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                   move_ir1(COLOR00), 0x0222,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(2000);
        checks.expect(h.move_count(COLOR00) == 1, "W2 WAIT for an already-passed line completes (greater-or-equal, p.27) "
                      "[claims: COP2-copins-kinds.c4, COP2-copins-kinds.c5, "
                      "COP2-the-wait-instruction.c2, COP2-putting-together-a-copper-instruction-list.c15, "
                      "COP2-putting-together-a-copper-instruction-list.c16, COP2-the-wait-instruction.c30, "
                      "COP2-the-wait-instruction.c31, COP2-the-wait-instruction.c32, COP2-a-copper-loop-example.c6, ch2-a-copper-loop-example.c6, ch2-appa-copins.c9, ch2-putting-together-a-copper-instruction-list.c14, ch2-putting-together-a-copper-instruction-list.c15, ch2-putting-together-a-copper-instruction-list.c17, ch2-the-wait-instruction.c1, ch2-the-wait-instruction.c29, ch2-the-wait-instruction.c30]");
    }

    // W3 - p.27 worked example: two WAITs for the same position, both MOVEs run.
    {
        CopperHarness h;
        h.seek_beam(0x30);
        h.install({ir1_pos(0x3C, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                   move_ir1(COLOR00), 0x0333,
                   ir1_pos(0x3C, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                   move_ir1(COLOR01), 0x0444,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(6000);
        checks.expect(h.move_count(COLOR00) == 1 && h.move_count(COLOR01) == 1,
                      "W3 two WAITs for the same position: both MOVEs run (p.27) [claims: ch2-putting-together-a-copper-instruction-list.c16, ch2-putting-together-a-copper-instruction-list.c18, ch2-putting-together-a-copper-instruction-list.c19, ch2-putting-together-a-copper-instruction-list.c20]");
    }

    // W4 - p.22/p.30: the terminal WAIT $FFFF,$FFFE can never complete.
    //
    // The check below counts zero MOVEs, and that is true - but it was true for
    // a reason that lived in this comment and not in any assertion. An
    // independent pass drove the module's `hpos` input directly with raw values
    // this fixture never produces and got the WAIT to release. `hpos` is an
    // input (agnus_copper.v:70) and nothing inside the module bounds it; what
    // bounds it is the beam counter of a real machine, and that is exactly what
    // the manual's claim is about.
    //
    // So the reason is now asserted rather than described. W4a witnesses that
    // the fixture really did drive a full line and reach the top of the vertical
    // range - the vertical half of the terminal WAIT is therefore satisfied, and
    // only the horizontal half holds it. W4b asserts that horizontal half: on a
    // 454-count line the HP comparison field cannot reach the sentinel's $7F.
    //
    // Same shape as a defect found in W1 on 06.09.2026, where a check counted
    // bus requests between two adjacent ticks and was vacuously true.
    {
        CopperHarness h;
        h.seek_beam(0xF0);
        h.install({0xFFFF, 0xFFFE,
                   move_ir1(COLOR00), 0x0555});
        h.start_list();
        h.run_cycles(20000);

        // The comparison field is hpos[8:2] and the line is 454 units long.
        const int hp_reached = h.max_hpos_driven >> 2;
        std::ostringstream hp_hex;
        hp_hex << std::hex << std::uppercase << hp_reached;

        checks.expect(h.max_vpos_driven >= 0xFF && h.max_hpos_driven >= 400,
                      "W4a [domain witness] the fixture drove a full line and reached "
                      "vertical $FF, so the terminal WAIT's vertical half is satisfied");
        checks.expect(hp_reached < 0x7F,
                      "W4b [the reason, asserted] on a 454-count line the HP comparison "
                      "field only reaches $" + hp_hex.str() + ", below the terminal "
                      "WAIT's $7F, so the horizontal half can never match (p.22, p.30)");
        checks.expect(h.move_count(COLOR00) == 0,
                      "W4 terminal WAIT $FFFF,$FFFE never releases - for the reason "
                      "asserted in W4a/W4b (p.22, p.30) "
                      "[claims: COP2-stopping-the-copper.c2, COP2-stopping-the-copper.c8, "
                      "COP2-a-copper-loop-example.c20, ch2-a-copper-loop-example.c17, ch2-stopping-the-copper.c2, ch2-stopping-the-copper.c3, ch2-stopping-the-copper.c6, ch2-the-wait-instruction.c16, ch2-the-wait-instruction.c17]");
    }

    // W5 - p.32 masking rule: with VE=$0F only the low four vertical bits are
    // compared, so a WAIT for VP=$0F matches on a line whose low nibble is $F.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install({ir1_pos(0x0F, 0x00), ir2_mask(true, 0x0F, 0x00, false),
                   move_ir1(COLOR00), 0x0666,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(12000);
        const int at = h.first_move_vpos(COLOR00);
        checks.expect(h.move_count(COLOR00) == 1 && at >= 0 && (at & 0x0F) == 0x0F,
                      "W5 VE=$0F: WAIT for VP=$0F releases on a line ending in $F (p.32) "
                      "[claims: COP2-the-wait-instruction.c7, COP2-the-wait-instruction.c8, "
                      "COP2-the-wait-instruction.c9, COP2-copper-loops-and-branches-and-comparison-enable.c10, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c11, ch2-a-copper-loop-example.c1, ch2-copper-loops-and-branches.c8, ch2-the-wait-instruction.c6, ch2-vertical-beam-position.c9]");
    }

    // W6 - p.32 worked example: the top bit cannot be masked, because IR2
    // bit 15 is BFD. Once VP passes $80 the same WAIT is already true.
    {
        CopperHarness h;
        h.seek_beam(0x80);
        h.install({ir1_pos(0x0F, 0x00), ir2_mask(true, 0x0F, 0x00, false),
                   move_ir1(COLOR00), 0x0777,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(2000);
        const int at = h.first_move_vpos(COLOR00);
        checks.expect(h.move_count(COLOR00) == 1 && at >= 0 && (at & 0x0F) != 0x0F,
                      "W6 the top bit is unmaskable: past VP $80 the same WAIT is already true (p.32) "
                      "[claims: COP2-copins-bfd.c7, COP2-copper-loops-and-branches-and-comparison-enable.c12, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c16, COP2-a-copper-loop-example.c1, "
                      "COP2-a-copper-loop-example.c2, COP2-a-copper-loop-example.c3, COP2-a-copper-loop-example.c15, "
                      "COP2-a-copper-loop-example.c17, COP2-a-copper-loop-example.c12, ch2-a-copper-loop-example.c12, ch2-a-copper-loop-example.c2, ch2-a-copper-loop-example.c3, ch2-a-copper-loop-example.c4, ch2-a-copper-loop-example.c5, ch2-appa-copins.c125, ch2-copper-loops-and-branches.c9]");
    }

    // W7 - a horizontal position with HE enabled is honoured.
    {
        CopperHarness h;
        h.seek_beam(0x40);
        h.install({ir1_pos(0x41, 0x40), ir2_mask(true, 0x7F, 0x7F, false),
                   move_ir1(COLOR00), 0x0888,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(6000);
        checks.expect(h.move_count(COLOR00) == 1, "W7 WAIT with an enabled horizontal compare releases "
                      "[claims: COP2-the-wait-instruction.c6, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c4, ch2-the-wait-instruction.c5]");
    }

    // ---------------- SKIP (COP-08) ----------------

    // S1 - p.31: SKIP skips the next instruction when the beam is at or past
    // the position. The instruction after the skipped one still runs.
    {
        CopperHarness h;
        h.seek_beam(0x70);
        h.install({ir1_pos(0x64, 0x00), ir2_mask(true, 0x7F, 0x00, true),
                   move_ir1(COLOR00), 0x0999,
                   move_ir1(COLOR01), 0x0AAA,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR00) == 0, "S1 SKIP with condition true: the next MOVE is skipped (p.31) "
                      "[claims: COP2-copins-kinds.c8, COP2-copins-kinds.c9, COP2-copins-encoding.c96, "
                      "COP2-what-is-a-copper-instruction.c7, COP2-the-skip-instruction.c2, "
                      "COP2-the-skip-instruction.c10, COP2-the-skip-instruction.c17, ch2-the-skip-instruction.c13, ch2-what-is-a-copper-instruction.c5]");
        checks.expect(h.move_count(COLOR01) == 1, "S1 SKIP with condition true: the MOVE after that still runs");
    }

    // S2 - condition false, so nothing is skipped.
    {
        CopperHarness h;
        h.seek_beam(0x10);
        h.install({ir1_pos(0x64, 0x00), ir2_mask(true, 0x7F, 0x00, true),
                   move_ir1(COLOR00), 0x0BBB,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR00) == 1, "S2 SKIP with condition false: the next MOVE runs "
                      "[claims: COP2-the-skip-instruction.c7, COP2-the-skip-instruction.c17]");
    }

    // S3 - with every compare enable clear the comparison is always true,
    // so the SKIP always takes effect.
    //
    // The position must keep its top bit clear. IR2 bit 15 is BFD, so VP bit 7
    // has no compare enable and is always compared (R4 of the specification).
    // A VP of $FF would therefore *not* give an always-true comparison - an
    // earlier revision of this test used $FF and failed for exactly that
    // reason, which is a defect of the test case, not of the Copper.
    {
        CopperHarness h;
        h.seek_beam(0x00);
        h.install({ir1_pos(0x70, 0x00), ir2_mask(true, 0x00, 0x00, true),
                   move_ir1(COLOR00), 0x0CCC,
                   move_ir1(COLOR01), 0x0DDD,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR00) == 0 && h.move_count(COLOR01) == 1,
                      "S3 all compare enables clear: comparison always true, SKIP takes effect (p.32) "
                      "[claims: COP2-the-skip-instruction.c14, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c7, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c11]");
    }

    // S4 - the same two words with IR2 bit 0 clear are a WAIT, not a SKIP:
    // the following MOVE runs instead of being skipped.
    {
        CopperHarness h;
        h.seek_beam(0x70);
        h.install({ir1_pos(0x64, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                   move_ir1(COLOR00), 0x0EEE,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR00) == 1, "S4 IR2 bit 0 alone distinguishes WAIT from SKIP "
                      "[claims: COP2-copins-encoding.c94, COP2-copins-encoding.c96, "
                      "COP2-the-skip-instruction.c3, COP2-summary-of-copper-instructions.c94, "
                      "COP2-summary-of-copper-instructions.c96, COP2-the-skip-instruction.c12, "
                      "COP2-the-skip-instruction.c4, COP2-the-skip-instruction.c5, "
                      "COP2-the-wait-instruction.c13, ch2-appa-copins.c107, ch2-appa-copins.c109, ch2-summary-of-copper-instructions.c94, ch2-summary-of-copper-instructions.c96, ch2-the-skip-instruction.c2, ch2-the-skip-instruction.c3, ch2-the-skip-instruction.c8, ch2-the-wait-instruction.c10]");
    }

    // ---------------- BFD (COP-08, p.33) ----------------

    // B1 - BFD=1 is the normal case: only the beam comparison controls the WAIT,
    // even while the blitter is busy.
    {
        CopperHarness h;
        h.seek_beam(0x50);
        h.set_blitter_busy(true);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                   move_ir1(COLOR00), 0x1111,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR00) == 1, "B1 BFD=1: a busy blitter does not hold the WAIT (p.33) "
                      "[claims: COP2-copins-bfd.c2, COP2-copins-encoding.c4, "
                      "COP2-the-wait-instruction.c14, COP2-copper-loops-and-branches-and-comparison-enable.c13, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c14, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c15, "
                      "COP2-using-the-copper-with-the-blitter.c5, "
                      "COP2-summary-of-copper-instructions.c4, ch2-appa-copins.c122, ch2-appa-copins.c17, ch2-copper-loops-and-branches.c10, ch2-copper-loops-and-branches.c11, ch2-summary-of-copper-instructions.c4, ch2-the-wait-instruction.c11, ch2-using-the-copper-with-the-blitter.c3, ch2-using-the-copper-with-the-blitter.c5]");
    }

    // B2 - BFD=0 additionally requires the blitter to have finished.
    {
        CopperHarness h;
        h.seek_beam(0x50);
        h.set_blitter_busy(true);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(false, 0x7F, 0x00, false),
                   move_ir1(COLOR00), 0x2222,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR00) == 0, "B2 BFD=0: the WAIT is held while the blitter is busy (p.33) "
                      "[claims: COP2-copins-bfd.c3, COP2-copins-bfd.c5, COP2-copins-encoding.c4, "
                      "COP2-the-wait-instruction.c14, COP2-copper-loops-and-branches-and-comparison-enable.c13, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c14, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c15, "
                      "COP2-using-the-copper-with-the-blitter.c6, COP2-using-the-copper-with-the-blitter.c8, "
                      "COP2-summary-of-copper-instructions.c4, ch2-appa-copins.c17, ch2-using-the-copper-with-the-blitter.c6]");
    }

    // B2s - the same for SKIP. p.33 puts BFD in IR2, which both instructions
    // share, so the blitter term belongs to the SKIP condition as much as to the
    // WAIT condition: with BFD=0 and the blitter busy the condition is not
    // satisfied, so nothing is skipped and the following MOVE runs.
    //
    // This case was found on 07.09.2026 by measuring the pre-pipeline test
    // agnus_copper_skip against this file rather than assuming it superseded.
    // Every one of its other cases was already covered here and covered more
    // strictly - this one was not covered at all. Five of six is what an
    // impression would have called "redundant".
    {
        CopperHarness h;
        h.seek_beam(0x50);
        h.set_blitter_busy(true);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(false, 0x7F, 0x00, true),
                   move_ir1(COLOR00), 0x3333,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR00) == 1,
                      "B2s BFD=0: a busy blitter leaves the SKIP condition unsatisfied, "
                      "so the following MOVE runs (p.33) "
                      "[claims: COP2-copins-bfd.c4, COP2-copins-bfd.c6, "
                      "COP2-the-skip-instruction.c13, COP2-summary-of-copper-instructions.c6, ch2-appa-copins.c124, ch2-summary-of-copper-instructions.c6, ch2-the-skip-instruction.c9]");
    }

    // B3 - and releases once the blitter reports finished.
    {
        CopperHarness h;
        h.seek_beam(0x50);
        h.set_blitter_busy(true);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(false, 0x7F, 0x00, false),
                   move_ir1(COLOR00), 0x3333,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(2000);
        const bool held = h.move_count(COLOR00) == 0;
        h.set_blitter_busy(false);
        h.run_cycles(2000);
        checks.expect(held && h.move_count(COLOR00) == 1, "B3 BFD=0: the WAIT releases once the blitter finishes (p.33) "
                      "[claims: COP2-copins-bfd.c3, COP2-copins-bfd.c5, "
                      "COP2-using-the-copper-with-the-blitter.c8]");
    }

    // ---------------- Counter limits (COP-04) ----------------
    //
    // The specification listed a rollover case R1. It is NOT constructible at
    // this module boundary and is deliberately not asserted:
    //
    // The comparison is greater-or-equal, so a WAIT for any position at or
    // below the current counter is satisfied immediately. There is therefore no
    // WAIT that is false just before the vertical counter wraps at $FF and true
    // just after it - anything reachable is already true at $FF. Masking does
    // not help, because masked-out bits are compared as equal.
    //
    // The observable consequence of the counter limits the HRM actually
    // describes - that HP never exceeds $E2, so $FFFF,$FFFE can never be
    // reached - is asserted by W4 above.
    //
    // W8 records the other half of the same reasoning: a WAIT whose vertical
    // position is reachable *does* release even when its horizontal position is
    // one that never occurs, because the vertical comparison alone carries it.
    // This is the trap the HRM warns about on p.22 ("You may be tempted to wait
    // for horizontal position $FE ... This will not lead to the desired
    // result").
    {
        CopperHarness h;
        h.seek_beam(0x40);
        h.install({ir1_pos(0x50, 0x7F), ir2_mask(true, 0x7F, 0x7F, false),
                   move_ir1(COLOR00), 0x4444,
                   0xFFFF, 0xFFFE});
        h.start_list();
        h.run_cycles(12000);
        const int at = h.first_move_vpos(COLOR00);
        checks.expect(h.move_count(COLOR00) == 1 && at > 0x50,
                      "W8 VP reachable with an unreachable HP still releases once VP passes it (p.22 warning)");
    }

    // Machine-readable mutation records for the block below, all stimulus,
    // each reverted immediately after observing red. One mutation covers the
    // whole walk because the walk is one fixture: shifting the encoding by one
    // position turns every bit assertion red at once, which is the correct
    // shape -- they share a mapping, so they fail together or not at all.
    // MUTATION EVC stimulus red the beam started at line $10 instead of $00, so a WAIT for VP $00 is satisfied on a line other than the one the control names

    // MUTATION EV stimulus red the walked bit shifted one position down before encoding, so every WAIT asks for half the line it is checked against - one shift breaks all eight at once, which is why the family carries one record
    // ---------------- EV0-EV7: the VP field's bit mapping ----------------
    //
    // Added 06.09.2026, FPGA-086, package PKG-APPX-WAITSKIP-DECODE, from
    // appendix A's COPINS table on printed page 274. The table states the
    // mapping row by row: IR1 bit 15 is VP7, bit 14 is VP6, down to bit 8 as
    // VP0.
    //
    // WHY THIS WAS MISSING. Every Copper test in this suite builds its WAITs
    // with `ir1_pos(vp, hp)`, which encodes that mapping in one line of C++
    // (`(vp << 8) | ...`). No test asserts it. The helper is an assumption
    // shared by every WAIT and SKIP case here, in agnus_copper_wait,
    // agnus_copper_skip and agnus_copper_list -- and an assumption that wrong
    // would make all of them build the wrong instruction, some of them still
    // passing. The chapter states the field but not the bit positions; only the
    // appendix table gives them, and the appendix was not in the corpus until
    // 05.09.2026.
    //
    // The measurement is a walking one over the vertical comparison field: for
    // each bit, a WAIT whose VP has only that bit set must release exactly when
    // the beam reaches that line, because the comparison is greater-or-equal
    // and the beam starts below it. A shifted mapping releases on a different
    // line; a dropped bit never releases at all.
    //
    // Horizontal comparison is masked out (HE = 0) so the vertical bit under
    // test is the only thing deciding release.
    {
        auto release_line = [](uint8_t vp) -> int {
            CopperHarness h;
            h.seek_beam(0x00);
            h.install({ir1_pos(vp, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                       move_ir1(COLOR00), 0x5A5A,
                       0xFFFF, 0xFFFE});
            h.start_list();
            // One scan line is 454 ticks; allow the full walk to reach line
            // $80 with room to spare rather than scaling per bit.
            h.run_cycles(160 * 454);
            return h.move_count(COLOR00) == 1 ? h.first_move_vpos(COLOR00) : -1;
        };

        // Control first. VP = 0 is satisfied on arrival, so the MOVE must run
        // on the starting line. If this does not hold, the fixture is not
        // measuring release lines and none of EV0-EV7 means anything.
        const int immediate = release_line(0x00);
        checks.expect(immediate == 0x00,
            "EVC [control] a WAIT for VP $00 releases on the starting line - the fixture "
            "reports the line a WAIT releases on (p274 table, greater-or-equal per p.27) "
            "[claims: COP2-the-wait-instruction.c1, "
            "COP2-the-wait-instruction.c30, COP2-putting-together-a-copper-instruction-list.c14, "
            "COP2-what-is-a-copper-instruction.c4, COP2-vertical-beam-position.c5, "
            "COP2-vertical-beam-position.c6, COP2-vertical-beam-position.c8, ch2-appa-copins.c117]");

        // COP2-copins-encoding claim ids as a literal array, one per bit,
        // rather than built from an arithmetic expression at runtime: the same
        // reason the comment above gives for keeping "EV" itself a literal - a
        // claim id assembled only from std::to_string(45 - 6*bit) never
        // appears as a full, grep-able substring anywhere in this file's text.
        static const char* const kEvApxClaimId[8] = {
            "COP2-copins-encoding.c45, ch2-appa-copins.c58", "COP2-copins-encoding.c39, ch2-appa-copins.c52",
            "COP2-copins-encoding.c33, ch2-appa-copins.c46", "COP2-copins-encoding.c27, ch2-appa-copins.c40",
            "COP2-copins-encoding.c21, ch2-appa-copins.c34", "COP2-copins-encoding.c15, ch2-appa-copins.c28",
            "COP2-copins-encoding.c9, ch2-appa-copins.c22",  "COP2-copins-encoding.c3, ch2-appa-copins.c16",
        };
        // Same reasoning as kEvApxClaimId above, for the body-claims id family:
        // a claim id built only from std::to_string(45 - 6*bit) never appears as
        // a literal substring anywhere in this file's text, so a static search
        // over assertion strings would not find it even though the compiled
        // program prints it correctly. Kept as a literal array instead.
        static const char* const kEvBodyClaimId[8] = {
            "COP2-summary-of-copper-instructions.c45, ch2-summary-of-copper-instructions.c45", "COP2-summary-of-copper-instructions.c39, ch2-summary-of-copper-instructions.c39",
            "COP2-summary-of-copper-instructions.c33, ch2-summary-of-copper-instructions.c33", "COP2-summary-of-copper-instructions.c27, ch2-summary-of-copper-instructions.c27",
            "COP2-summary-of-copper-instructions.c21, ch2-summary-of-copper-instructions.c21", "COP2-summary-of-copper-instructions.c15, ch2-summary-of-copper-instructions.c15",
            "COP2-summary-of-copper-instructions.c9, ch2-summary-of-copper-instructions.c9",  "COP2-summary-of-copper-instructions.c3, ch2-summary-of-copper-instructions.c3",
        };
        for (int bit = 0; bit <= 7; ++bit) {
            const uint8_t vp = static_cast<uint8_t>(1u << bit);
            const int at = release_line(vp);
            std::cout << "[measured] WAIT VP bit " << bit << " (VP=$" << std::hex
                      << static_cast<int>(vp) << std::dec << ") released on line " << at << "\n";
            // The id is a literal so tools/check_gates.py can read it and tie
            // it to the mutation records above. Composed ids - "EV" + the bit
            // number - left all eight invisible to the gate, which then could
            // not check that they had been mutated at all. The bit number moved
            // into the text, where it reads just as well.
            checks.expect(at == static_cast<int>(vp),
                "EV walking one: IR1 bit " + std::to_string(bit + 8) +
                " carries VP" + std::to_string(bit) +
                ", so a WAIT with only that vertical bit set releases on line " +
                std::to_string(vp) + " (appendix A p274 instruction table) "
                "[claims: " + std::string(kEvApxClaimId[bit]) +
                ", " + std::string(kEvBodyClaimId[bit]) +
                ", COP2-the-wait-instruction.c11, COP2-the-wait-instruction.c32]");
        }
    }

    // Machine-readable mutation record for the block below. EVC's record sits
    // with the EV block above, where it belongs.
    // MUTATION EHC stimulus red the same same-line change; the value calibrated from bit 0 is then not a latency at all
    //
    // The same-line mutation leaves the lowest bit green and takes every other
    // one down,
    // which is the correct shape: with the floor back, bit 0's own target is
    // below it and coincides with the floor by accident, while the bits above
    // it no longer separate. That is precisely why the walk waits for the next
    // line, and the mutation demonstrates the reason rather than only the rule.

    // MUTATION EH stimulus red bit 0 forced into every walked HP value, so each target gains 4 while the latency calibrated from bit 0 stays put - all seven fall together
    // ---------------- EH0-EH6: the HP field's bit mapping ----------------
    //
    // The other half of appendix A's table on printed page 274: IR1 bits 7
    // down to 1 carry HP8 down to HP2. Same reasoning as EV0-EV7 above - the
    // mapping is encoded in `ir1_pos()` and was never asserted.
    //
    // The horizontal case needs one thing the vertical case did not. The beam
    // comparison is on the combined (vertical, horizontal) position, not two
    // independent ones, so a WAIT for line 0 and a small HP is satisfied before
    // the Copper has even finished fetching it: measured, bits 0, 1 and 2 all
    // released at hpos 31, the floor set by the list start, and were
    // indistinguishable. Waiting for the NEXT line removes the floor - the
    // Copper is already blocked when the line begins, so the horizontal target
    // is met exactly where the field says.
    //
    // Measured with VP = 1, all seven bits separate cleanly and the offset from
    // the predicted position is constant at 15 - the fetch latency of the MOVE
    // that follows the WAIT. The assertion calibrates that latency from the
    // first bit rather than hard-coding it, so it tests the MAPPING and not the
    // fixture's timing.
    //
    // A caution about bit 6 (HP8). This fixture's line is 454 hpos units long
    // (see cycle()); a real Amiga line is 227. HP8's target of 256 is therefore
    // reachable here and is NOT reachable on hardware - which is the trap page
    // 22 warns about and W4/W8 above assert. A green EH6 says the bit is wired
    // where the table says, not that a program may use it.
    {
        auto release_hpos = [](uint8_t hp) -> int {
            CopperHarness h;
            h.seek_beam(0x00);
            h.install({ir1_pos(0x01, hp), ir2_mask(true, 0x7F, 0x7F, false),
                       move_ir1(COLOR00), 0x5A5A,
                       0xFFFF, 0xFFFE});
            h.start_list();
            h.run_cycles(3 * 454);
            return h.move_count(COLOR00) == 1 ? h.first_move_hpos(COLOR00) : -1;
        };

        // The HP field sits at IR1 bits 7..1 and is compared against the beam's
        // horizontal position in units of four, so field value h means hpos 4h.
        const int first = release_hpos(1);
        const int latency = first - 4;
        std::cout << "[measured] HP walk: bit 0 released at hpos " << first
                  << ", so the MOVE's fetch latency is " << latency << "\n";

        checks.expect(first > 0 && latency >= 0 && latency < 64,
            "EHC [control] the HP walk's first bit releases at a plausible position, so "
            "the fetch latency can be calibrated from it rather than assumed "
            "[claims: ch2-appa-copins.c118]");

        static const char* const kEhApxClaimId[7] = {
            "COP2-copins-encoding.c87, ch2-appa-copins.c100", "COP2-copins-encoding.c81, ch2-appa-copins.c94",
            "COP2-copins-encoding.c75, ch2-appa-copins.c88", "COP2-copins-encoding.c69, ch2-appa-copins.c82",
            "COP2-copins-encoding.c63, ch2-appa-copins.c76", "COP2-copins-encoding.c57, ch2-appa-copins.c70",
            "COP2-copins-encoding.c51, ch2-appa-copins.c64",
        };
        // Body-claims counterpart, literal for the same grep-ability reason as
        // kEhApxClaimId and kEvBodyClaimId above.
        static const char* const kEhBodyClaimId[7] = {
            "COP2-summary-of-copper-instructions.c87, ch2-summary-of-copper-instructions.c87", "COP2-summary-of-copper-instructions.c81, ch2-summary-of-copper-instructions.c81",
            "COP2-summary-of-copper-instructions.c75, ch2-summary-of-copper-instructions.c75", "COP2-summary-of-copper-instructions.c69, ch2-summary-of-copper-instructions.c69",
            "COP2-summary-of-copper-instructions.c63, ch2-summary-of-copper-instructions.c63", "COP2-summary-of-copper-instructions.c57, ch2-summary-of-copper-instructions.c57",
            "COP2-summary-of-copper-instructions.c51, ch2-summary-of-copper-instructions.c51",
        };
        for (int bit = 0; bit <= 6; ++bit) {
            const uint8_t hp = static_cast<uint8_t>(1u << bit);
            const int expected = 4 * (1 << bit) + latency;
            const int at = release_hpos(hp);
            std::cout << "[measured] HP bit " << bit << " (HP" << (bit + 2)
                      << ") released at hpos " << at << ", expected " << expected << "\n";
            checks.expect(at == expected,
                "EH walking one: IR1 bit " + std::to_string(bit + 1) +
                " carries HP" + std::to_string(bit + 2) +
                ", so a WAIT with only that horizontal bit set releases at hpos " +
                std::to_string(4 * (1 << bit)) + " plus the fetch latency "
                "(appendix A p274 instruction table) "
                "[claims: " + std::string(kEhApxClaimId[bit]) +
                ", " + std::string(kEhBodyClaimId[bit]) +
                ", COP2-the-wait-instruction.c12]");
        }
    }
    //
    // All seven fall together, which is right: a rotation breaks every pairing
    // at once. That they fall together and not one at a time is what shows the
    // assertions test a mapping rather than seven unrelated facts.

    // MUTATION EM stimulus red the mask clears the NEXT bit instead of the one under test, so every pairing asks about a mask bit governing a different comparison bit - all seven fall together
    // ---------------- EM0-EM6: the VE mask field's bit mapping ----------------
    //
    // The IR2 half of appendix A's table: bits 14 down to 8 carry VE6 down to
    // VE0, and page 275's legend calls them "Enable comparison (mask bit)".
    // The note under the table adds the exception: "Note that the V7
    // comparison cannot be masked" - which is why VE has seven bits for eight
    // VP bits, and which W6 above already asserts.
    //
    // W5 exercises one mask value ($0F) and W6 the unmaskable bit. Neither
    // establishes WHICH mask bit governs which comparison bit, and `ir2_mask()`
    // encodes that assumption exactly as `ir1_pos()` encoded the other.
    //
    // The measurement pairs each bit against itself. A WAIT for VP = 2^j blocks
    // until line 2^j when its mask bit is set - that is EV<j> above. Clear only
    // that one mask bit and the comparison must ignore that position bit, so
    // the same WAIT is satisfied on arrival. Anything else means the mask bit
    // is wired to a different comparison bit, or to none.
    {
        auto release_line = [](uint8_t vp, uint8_t ve) -> int {
            CopperHarness h;
            h.seek_beam(0x00);
            h.install({ir1_pos(vp, 0x00), ir2_mask(true, ve, 0x00, false),
                       move_ir1(COLOR00), 0x5A5A,
                       0xFFFF, 0xFFFE});
            h.start_list();
            h.run_cycles(160 * 454);
            return h.move_count(COLOR00) == 1 ? h.first_move_vpos(COLOR00) : -1;
        };

        static const char* const kEmApxClaimId[7] = {
            "COP2-copins-encoding.c46, ch2-appa-copins.c119, ch2-appa-copins.c59", "COP2-copins-encoding.c40, ch2-appa-copins.c53",
            "COP2-copins-encoding.c34, ch2-appa-copins.c47", "COP2-copins-encoding.c28, ch2-appa-copins.c41",
            "COP2-copins-encoding.c22, ch2-appa-copins.c35", "COP2-copins-encoding.c16, ch2-appa-copins.c29",
            "COP2-copins-encoding.c10, ch2-appa-copins.c23",
        };
        // Body-claims counterpart, literal for the same grep-ability reason as
        // kEmApxClaimId and kEvBodyClaimId above.
        static const char* const kEmBodyClaimId[7] = {
            "COP2-summary-of-copper-instructions.c46, ch2-summary-of-copper-instructions.c46", "COP2-summary-of-copper-instructions.c40, ch2-summary-of-copper-instructions.c40",
            "COP2-summary-of-copper-instructions.c34, ch2-summary-of-copper-instructions.c34", "COP2-summary-of-copper-instructions.c28, ch2-summary-of-copper-instructions.c28",
            "COP2-summary-of-copper-instructions.c22, ch2-summary-of-copper-instructions.c22", "COP2-summary-of-copper-instructions.c16, ch2-summary-of-copper-instructions.c16",
            "COP2-summary-of-copper-instructions.c10, ch2-summary-of-copper-instructions.c10",
        };
        for (int bit = 0; bit <= 6; ++bit) {
            const uint8_t vp = static_cast<uint8_t>(1u << bit);
            const uint8_t masked_out = static_cast<uint8_t>(0x7F & ~(1u << bit));
            const int with_mask = release_line(vp, 0x7F);
            const int without   = release_line(vp, masked_out);
            std::cout << "[measured] VE bit " << bit << ": enabled -> line " << with_mask
                      << ", disabled -> line " << without << "\n";
            // p275's "Enable comparison (mask bit)" legend itself is
            // COP2-copins-fields-p275.c12, bound once here (bit 0) rather than
            // repeated on all seven - it is one naming claim, not seven.
            checks.expect(with_mask == static_cast<int>(vp) && without == 0,
                "EM walking one: IR2 bit " + std::to_string(bit + 8) +
                " carries VE" + std::to_string(bit) +
                ", so with it set the WAIT blocks to line " + std::to_string(vp) +
                " and with it cleared the same WAIT is satisfied on arrival - the mask bit "
                "governs exactly VP" + std::to_string(bit) +
                " (appendix A p274 table, p275 \"Enable comparison (mask bit)\") "
                "[claims: " + std::string(kEmApxClaimId[bit]) +
                ", " + std::string(kEmBodyClaimId[bit]) +
                ", COP2-the-wait-instruction.c16" +
                (bit == 0 ? std::string(", "
                                        "COP2-copper-loops-and-branches-and-comparison-enable.c5")
                          : std::string()) + "]");
        }
    }

    // ------------------------------------------------------------------
    // Appendix claims considered for this package and left UNBOUND, with
    // reasons. None of these change any assertion above; they are a printed
    // record only.
    // ------------------------------------------------------------------
    checks.note("UNBOUND (SKIP's own IR1 field values at every bit position - EV0-EV7 "
        "and EH0-EH6 above walk only the WAIT form, skip=false throughout; S1-S4 and "
        "B2s exercise SKIP's condition at fixed positions but never isolate individual "
        "bit positions the way the EV/EH walks do for WAIT, so a SKIP-specific decode "
        "fault distinct from WAIT's would go undetected): "
        "COP2-copins-encoding.c5, COP2-copins-encoding.c11, COP2-copins-encoding.c17, "
        "COP2-copins-encoding.c23, COP2-copins-encoding.c29, COP2-copins-encoding.c35, "
        "COP2-copins-encoding.c41, COP2-copins-encoding.c47, COP2-copins-encoding.c53, "
        "COP2-copins-encoding.c59, COP2-copins-encoding.c65, COP2-copins-encoding.c71, "
        "COP2-copins-encoding.c77, COP2-copins-encoding.c83, COP2-copins-encoding.c89, ch2-appa-copins.c102, ch2-appa-copins.c18, ch2-appa-copins.c24, ch2-appa-copins.c30, ch2-appa-copins.c36, ch2-appa-copins.c42, ch2-appa-copins.c48, ch2-appa-copins.c54, ch2-appa-copins.c60, ch2-appa-copins.c66, ch2-appa-copins.c72, ch2-appa-copins.c78, ch2-appa-copins.c84, ch2-appa-copins.c90, ch2-appa-copins.c96, ch2-summary-of-copper-instructions.c12, ch2-summary-of-copper-instructions.c18, ch2-summary-of-copper-instructions.c24, ch2-summary-of-copper-instructions.c30, ch2-summary-of-copper-instructions.c36, ch2-summary-of-copper-instructions.c42, ch2-summary-of-copper-instructions.c48, ch2-summary-of-copper-instructions.c54, ch2-summary-of-copper-instructions.c60, ch2-summary-of-copper-instructions.c66, ch2-summary-of-copper-instructions.c72, ch2-summary-of-copper-instructions.c78, ch2-summary-of-copper-instructions.c84, ch2-summary-of-copper-instructions.c90, ch2-the-skip-instruction.c11, ch2-the-skip-instruction.c5, ch2-the-skip-instruction.c6, ch2-the-wait-instruction.c8, ch2-the-wait-instruction.c9, ch2-vertical-beam-position.c6, ch2-vertical-beam-position.c7");
    checks.note("UNBOUND (SKIP's own IR2 field values at every bit position, same reason "
        "as the IR1 note above - no per-bit walk exists for the SKIP form; c96, bit 0, "
        "is bound at S1/S4 separately): "
        "COP2-copins-encoding.c6, COP2-copins-encoding.c12, COP2-copins-encoding.c18, "
        "COP2-copins-encoding.c24, COP2-copins-encoding.c30, COP2-copins-encoding.c36, "
        "COP2-copins-encoding.c42, COP2-copins-encoding.c48, COP2-copins-encoding.c54, "
        "COP2-copins-encoding.c60, COP2-copins-encoding.c66, COP2-copins-encoding.c72, "
        "COP2-copins-encoding.c78, COP2-copins-encoding.c84, COP2-copins-encoding.c90, ch2-appa-copins.c103, ch2-appa-copins.c19, ch2-appa-copins.c25, ch2-appa-copins.c31, ch2-appa-copins.c37, ch2-appa-copins.c43, ch2-appa-copins.c49, ch2-appa-copins.c55, ch2-appa-copins.c61, ch2-appa-copins.c67, ch2-appa-copins.c73, ch2-appa-copins.c79, ch2-appa-copins.c85, ch2-appa-copins.c91, ch2-appa-copins.c97, ch2-the-skip-instruction.c10");
    checks.note("UNBOUND (COP2-copins-fields-p275.c10, .c11 and .c12: \"VP\" denotes a "
        "vertical-beam-position bit, \"HP\" a horizontal one, \"VE\" a comparison-enable "
        "mask bit - three names, not three requirements. EVC measures release at line "
        "zero, EHC measures release latency and EM0 measures bit 8's masking, and each "
        "does what it says; none could fail because a field is CALLED something else. "
        "They were bound there until 11.09.2026, while this same file already treated "
        "the IR1, IR2 and BFD naming claims as unbound - the inconsistency was with "
        "its own neighbours)");
    checks.note("UNBOUND (the HE field's own bit-position mapping and masking - EM0-EM6 "
        "walk VE's masking role bit by bit, but no equivalent walk exists for HE; W7 "
        "sets HE=$7F generally and W1 sets HE=$00, but W1's target HP happens to be 0, "
        "which a HE=0 comparison-against-hpos-itself would satisfy trivially regardless "
        "of whether masking actually works, so it does not discriminate): "
        "COP2-copins-encoding.c52, COP2-copins-encoding.c58, COP2-copins-encoding.c64, "
        "COP2-copins-encoding.c70, COP2-copins-encoding.c76, COP2-copins-encoding.c82, "
        "COP2-copins-encoding.c88, COP2-copins-fields-p275.c13, COP2-the-wait-instruction.c17, ch2-appa-copins.c101, ch2-appa-copins.c120, ch2-appa-copins.c65, ch2-appa-copins.c71, ch2-appa-copins.c77, ch2-appa-copins.c83, ch2-appa-copins.c89, ch2-appa-copins.c95, ch2-copper-loops-and-branches.c6, ch2-copper-loops-and-branches.c7, ch2-the-wait-instruction.c13, ch2-the-wait-instruction.c14");
    checks.note("UNBOUND (COP2-copins-fields.c1, COP2-copins-fields.c2: the page-274 "
        "legend \"IR1=First instruction register\" / \"IR2=Second instruction register\" "
        "is a purely definitional naming claim, not independently RTL-observable - and "
        "the RTL's own internal register naming inverts the manual's IR1/IR2 labels per "
        "the classifier's own evidence note on the MOVE-decode claims, which this file "
        "does not attempt to adjudicate) [claims: ch2-appa-copins.c110, ch2-appa-copins.c111]");

    // ------------------------------------------------------------------
    // Body claims (body_claims.jsonl) considered for this package and left
    // UNBOUND, with reasons. None of these change any assertion above; they
    // are a printed record only. Several land on exactly the same table rows
    // the appendix notes above already flag, reached independently.
    // ------------------------------------------------------------------
    checks.note("UNBOUND COP2-the-wait-instruction.c20, COP2-the-wait-instruction.c21 "
        "(\"line 255 is a valid WAIT target\" / \"255 is the max representable VP value\") "
        "-- no assertion measures it. No WAIT target in this file uses VP=$FF; EV's walk "
        "stops at bit 7 (VP=$80) and never exercises the fully-set field., ch2-the-wait-instruction.c19, ch2-the-wait-instruction.c20");
    checks.note("UNBOUND COP2-the-wait-instruction.c24 (\"the line number in the comparison "
        "never exceeds $FF\") -- no assertion measures it. This is the vpos port's 8-bit "
        "width, a fixed interface fact no stimulus here varies independently of it. [claims: ch2-the-wait-instruction.c23]");
    checks.note("UNBOUND COP2-horizontal-beam-position.c2, COP2-vertical-beam-position.c2 "
        "(\"the HP field's LSB does not participate\" / \"max vertical value is 255\") -- "
        "no assertion measures it. EH measures the field in units of 4 hpos ticks on this "
        "454-tick fixture line (2x a real 227-tick line), which is a different, "
        "fixture-scaled quantity from the manual's 1-bit/113-position claim, and no "
        "assertion here converts between them; the vertical claim is the same 8-bit-width "
        "fact as c21 above, restated. [claims: ch2-horizontal-beam-position.c2, ch2-vertical-beam-position.c1, ch2-vertical-beam-position.c2, ch2-vertical-beam-position.c5]");
    checks.note("UNBOUND COP2-summary-of-copper-instructions.c52, "
        "COP2-summary-of-copper-instructions.c58, COP2-summary-of-copper-instructions.c64, "
        "COP2-summary-of-copper-instructions.c70, COP2-summary-of-copper-instructions.c76, "
        "COP2-summary-of-copper-instructions.c82, COP2-summary-of-copper-instructions.c88 "
        "(\"bit N of the second WAIT word carries HEn\", the HE per-bit mapping) -- no "
        "assertion measures it. Same gap as the appendix note above for "
        "COP2-copins-encoding.c52/58/64/70/76/82/88: EM0-EM6 walk VE bit by bit, nothing "
        "walks HE the same way, and W1/W7 only exercise HE at its extremes (all-clear, "
        "all-set), which cannot show which bit governs which comparison bit. [claims: ch2-summary-of-copper-instructions.c52, ch2-summary-of-copper-instructions.c58, ch2-summary-of-copper-instructions.c64, ch2-summary-of-copper-instructions.c70, ch2-summary-of-copper-instructions.c76, ch2-summary-of-copper-instructions.c82, ch2-summary-of-copper-instructions.c88]");
    checks.note("UNBOUND (COP2-copins-bfd.c1: \"BFD denotes blitter-finished "
        "disable\" - a name, not a requirement. B1 and B2 listed it until 11.09.2026 "
        "alongside c2, the behaviour they do check; no assertion can fail because a "
        "bit is called something else)");
    checks.note("UNBOUND COP2-using-the-copper-with-the-blitter.c7 (\"BFD cleared: WAIT "
        "requires a true beam-counter comparison\") -- no assertion measures it. B2 and B3 "
        "both start with the position already satisfied (seek_beam past the WAIT's target), "
        "so neither can tell \"BFD=0 needs position true\" apart from \"BFD=0 ignores "
        "position and waits on the blitter alone\" - both hypotheses predict the same "
        "observed hold-then-release. Only the blitter-finished half (c8, bound at B2/B3) is "
        "actually discriminated. [claims: ch2-appa-copins.c123, ch2-using-the-copper-with-the-blitter.c7]");
    checks.note("UNBOUND (SKIP's own per-bit VP/HP field mapping in the page-36 summary "
        "table, IR1 side - same gap as the appendix IR1 note above, independently found: "
        "S1-S4 and B2s exercise SKIP at fixed combined positions, never isolate one bit at "
        "a time the way EV/EH do for WAIT): "
        "COP2-summary-of-copper-instructions.c5, COP2-summary-of-copper-instructions.c11, "
        "COP2-summary-of-copper-instructions.c17, COP2-summary-of-copper-instructions.c23, "
        "COP2-summary-of-copper-instructions.c29, COP2-summary-of-copper-instructions.c35, "
        "COP2-summary-of-copper-instructions.c41, COP2-summary-of-copper-instructions.c47, "
        "COP2-summary-of-copper-instructions.c53, COP2-summary-of-copper-instructions.c59, "
        "COP2-summary-of-copper-instructions.c65, COP2-summary-of-copper-instructions.c71, "
        "COP2-summary-of-copper-instructions.c77, COP2-summary-of-copper-instructions.c83, "
        "COP2-summary-of-copper-instructions.c89, ch2-summary-of-copper-instructions.c11, ch2-summary-of-copper-instructions.c17, ch2-summary-of-copper-instructions.c23, ch2-summary-of-copper-instructions.c29, ch2-summary-of-copper-instructions.c35, ch2-summary-of-copper-instructions.c41, ch2-summary-of-copper-instructions.c47, ch2-summary-of-copper-instructions.c5, ch2-summary-of-copper-instructions.c53, ch2-summary-of-copper-instructions.c59, ch2-summary-of-copper-instructions.c65, ch2-summary-of-copper-instructions.c71, ch2-summary-of-copper-instructions.c77, ch2-summary-of-copper-instructions.c83, ch2-summary-of-copper-instructions.c89");
    checks.note("UNBOUND (SKIP's own per-bit VE/HE/BFD field mapping in the page-36 summary "
        "table, IR2 side - same gap, same reason as the IR1 note just above; c6 is bound at "
        "B2s separately and listed there, not repeated as unbound here): "
        "COP2-summary-of-copper-instructions.c12, COP2-summary-of-copper-instructions.c18, "
        "COP2-summary-of-copper-instructions.c24, COP2-summary-of-copper-instructions.c30, "
        "COP2-summary-of-copper-instructions.c36, COP2-summary-of-copper-instructions.c42, "
        "COP2-summary-of-copper-instructions.c48, COP2-summary-of-copper-instructions.c54, "
        "COP2-summary-of-copper-instructions.c60, COP2-summary-of-copper-instructions.c66, "
        "COP2-summary-of-copper-instructions.c72, COP2-summary-of-copper-instructions.c78, "
        "COP2-summary-of-copper-instructions.c84, COP2-summary-of-copper-instructions.c90, ch2-summary-of-copper-instructions.c12, ch2-summary-of-copper-instructions.c18, ch2-summary-of-copper-instructions.c24, ch2-summary-of-copper-instructions.c30, ch2-summary-of-copper-instructions.c36, ch2-summary-of-copper-instructions.c42, ch2-summary-of-copper-instructions.c48, ch2-summary-of-copper-instructions.c54, ch2-summary-of-copper-instructions.c60, ch2-summary-of-copper-instructions.c66, ch2-summary-of-copper-instructions.c72, ch2-summary-of-copper-instructions.c78, ch2-summary-of-copper-instructions.c84, ch2-summary-of-copper-instructions.c90");
    checks.note("UNBOUND COP2-the-skip-instruction.c1, COP2-the-skip-instruction.c9 "
        "(\"SKIP skips when the beam counter EQUALS the encoded value\") -- no assertion "
        "measures it. Every SKIP scenario in this file (S1-S4) places the beam either "
        "already past the target or short of it within the run budget; none arranges an "
        "exact equality hit the way W3's WAIT scenario does by letting the beam increment "
        "up to a fixed line. Only the greater-than half of \"equal to or greater than\" is "
        "exercised for SKIP. [claims: ch2-the-skip-instruction.c1]");
    checks.note("UNBOUND COP2-the-skip-instruction.c8, COP2-the-skip-instruction.c15, "
        "COP2-the-skip-instruction.c16 (\"bits 7-1 of the first/second SKIP word carry HP / "
        "HE\") -- no assertion measures it. Every SKIP install() in this file uses hp=0x00 "
        "and he=0x00 throughout; the horizontal half of the SKIP position/mask is never "
        "varied, so nothing here shows those bits function as HP or HE for SKIP "
        "specifically (as opposed to being wired to nothing). [claims: ch2-the-skip-instruction.c11, ch2-the-skip-instruction.c12, ch2-the-skip-instruction.c6, ch2-the-wait-instruction.c31]");
    checks.note("UNBOUND COP2-the-skip-instruction.c11 (\"bits 15 through 1 of the first "
        "SKIP word form the combined comparison position\") -- no assertion measures it as "
        "a combined field. S1/S2 vary only the VP half (bits 15-8); with HP fixed at 0 "
        "throughout, the claim's HP-bits-7-1 half is untested, so the full 15-bit "
        "combination is not established, only the VP sub-field (see the bound "
        "COP2-the-skip-instruction.c7 at S1/S2)., ch2-the-skip-instruction.c7");
    checks.note("UNBOUND COP2-copper-loops-and-branches-and-comparison-enable.c6 (\"bits "
        "14-1 of SKIP word 2 select horizontal-position bits for the comparison\") -- no "
        "assertion measures it, same reason as the HE notes above: he=0x00 throughout every "
        "SKIP scenario in this file. [claims: ch2-copper-loops-and-branches.c4]");
    checks.note("UNBOUND COP2-copper-loops-and-branches-and-comparison-enable.c17 (\"SKIP "
        "cannot mask the most significant beam-position bit\") -- no assertion measures it. "
        "W6 establishes this for WAIT (VP=$0F with the top bit reached by the live beam); "
        "S3, the nearest SKIP analogue, uses VP=$70 (top bit clear) and never drives the "
        "beam or the target past $80 for a SKIP, so the SKIP-specific version of the "
        "unmaskable-top-bit behaviour is asserted nowhere., ch2-copper-loops-and-branches.c12");
    checks.note("UNBOUND COP2-stopping-the-copper.c3 (\"while an impossible WAIT is "
        "pending, the Copper does nothing\") -- no assertion measures it in full. The "
        "claim's own evidence names three simultaneous observations (reqdma low, "
        "address_out unchanged, no destination on reg_address_out); W4 here checks only "
        "the third (move_count(COLOR00)==0). This harness never records reqdma or "
        "address_out directly, so a hypothetical implementation that cycled the bus "
        "without ever completing a MOVE would still pass W4. COP2-stopping-the-copper.c8 "
        "(\"the final WAIT never completes\"), the narrower claim that the Copper does not "
        "advance past it, IS bound at W4 above.");
    checks.note("UNBOUND COP2-a-copper-loop-example.c9, COP2-a-copper-loop-example.c13, "
        "COP2-a-copper-loop-example.c14, COP2-a-copper-loop-example.c18 (worked-example "
        "narrative detail - waiting for HP $E2 forcing the next line; the first loop "
        "instruction being skipped at VP>=127 and control dropping into the second loop; "
        "the high-bit-set rule applying to the example's horizontal WAIT) -- no assertion "
        "measures it. These restate the general WAIT/SKIP greater-or-equal and "
        "unmaskable-top-bit mechanisms (already bound at W2/W6 above) in the specific "
        "narrative shape of the book's worked example; no fixture here reproduces that "
        "exact program to check the narrative claims themselves, and c18 specifically needs "
        "a horizontal WAIT case this file's masking tests never build (see the HE notes "
        "above). [claims: ch2-a-copper-loop-example.c13, ch2-a-copper-loop-example.c14, ch2-a-copper-loop-example.c15]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
