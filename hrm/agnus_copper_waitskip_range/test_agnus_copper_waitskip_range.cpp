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
// MUTATION B1 stimulus red: BFD flipped from 1 to 0 while the blitter is busy -> FAIL as expected (the WAIT is
//   now held, per B2).
// MUTATION B2 stimulus red: blitter_busy changed from true to false -> FAIL as expected (nothing holds the
//   WAIT, so it releases immediately).
// MUTATION B3 stimulus red: the blitter is never actually un-busied (set_blitter_busy(false) changed to
//   set_blitter_busy(true)) -> FAIL as expected (the WAIT never releases).
// MUTATION W8 stimulus red: VP target moved from $50 (reachable) to $A0 (unreachable in the 12000-cycle
//   budget) -> FAIL as expected.

#include <cstdint>
#include <iostream>
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
        checks.expect(h.move_count(COLOR00) == 1, "W1 WAIT line $96, HP masked: the following MOVE runs");
        checks.expect(at >= 0x96, "W1 WAIT line $96: the MOVE does not run before line $96");
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
        checks.expect(h.move_count(COLOR00) == 1, "W2 WAIT for an already-passed line completes (greater-or-equal, p.27)");
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
                      "W3 two WAITs for the same position: both MOVEs run (p.27)");
    }

    // W4 - p.22/p.30: the terminal WAIT $FFFF,$FFFE can never complete,
    // because HP never exceeds $E2.
    {
        CopperHarness h;
        h.seek_beam(0xF0);
        h.install({0xFFFF, 0xFFFE,
                   move_ir1(COLOR00), 0x0555});
        h.start_list();
        h.run_cycles(20000);
        checks.expect(h.move_count(COLOR00) == 0, "W4 terminal WAIT $FFFF,$FFFE never releases (p.22, p.30)");
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
                      "W5 VE=$0F: WAIT for VP=$0F releases on a line ending in $F (p.32)");
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
                      "W6 the top bit is unmaskable: past VP $80 the same WAIT is already true (p.32)");
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
        checks.expect(h.move_count(COLOR00) == 1, "W7 WAIT with an enabled horizontal compare releases");
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
        checks.expect(h.move_count(COLOR00) == 0, "S1 SKIP with condition true: the next MOVE is skipped (p.31)");
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
        checks.expect(h.move_count(COLOR00) == 1, "S2 SKIP with condition false: the next MOVE runs");
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
                      "S3 all compare enables clear: comparison always true, SKIP takes effect (p.32)");
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
        checks.expect(h.move_count(COLOR00) == 1, "S4 IR2 bit 0 alone distinguishes WAIT from SKIP");
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
        checks.expect(h.move_count(COLOR00) == 1, "B1 BFD=1: a busy blitter does not hold the WAIT (p.33)");
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
        checks.expect(h.move_count(COLOR00) == 0, "B2 BFD=0: the WAIT is held while the blitter is busy (p.33)");
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
        checks.expect(held && h.move_count(COLOR00) == 1, "B3 BFD=0: the WAIT releases once the blitter finishes (p.33)");
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
    // MUTATION EV0 stimulus red the walked bit shifted one position down before encoding, so every WAIT asks for half the line it is checked against
    // MUTATION EV1 stimulus red the same shift; bit 9 then releases on line 1, not 2
    // MUTATION EV2 stimulus red the same shift
    // MUTATION EV3 stimulus red the same shift
    // MUTATION EV4 stimulus red the same shift
    // MUTATION EV5 stimulus red the same shift
    // MUTATION EV6 stimulus red the same shift
    // MUTATION EV7 stimulus red the same shift
    // MUTATION EVC stimulus red the beam started at line $10 instead of $00, so a WAIT for VP $00 is satisfied on a line other than the one the control names

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
            "reports the line a WAIT releases on (p274 table, greater-or-equal per p.27)");

        for (int bit = 0; bit <= 7; ++bit) {
            const uint8_t vp = static_cast<uint8_t>(1u << bit);
            const int at = release_line(vp);
            std::cout << "[measured] WAIT VP bit " << bit << " (VP=$" << std::hex
                      << static_cast<int>(vp) << std::dec << ") released on line " << at << "\n";
            checks.expect(at == static_cast<int>(vp),
                "EV" + std::to_string(bit) +
                " IR1 bit " + std::to_string(bit + 8) + " carries VP" + std::to_string(bit) +
                ": a WAIT with only that vertical bit set releases on line " +
                std::to_string(vp) + " (appendix A p274 instruction table)");
        }
    }

    // Machine-readable mutation records for the block below, both stimulus.
    // MUTATION EH0 stimulus red the WAIT aimed at the current line instead of the next, restoring the start-up floor that makes the low bits indistinguishable
    // MUTATION EH1 stimulus red bit 0 forced into every walked HP value, so each target gains 4 while the latency calibrated from bit 0 stays put
    // MUTATION EH2 stimulus red the same forced bit
    // MUTATION EH3 stimulus red the same forced bit
    // MUTATION EH4 stimulus red the same forced bit
    // MUTATION EH5 stimulus red the same forced bit
    // MUTATION EH6 stimulus red the same forced bit
    // MUTATION EHC stimulus red the same same-line change; the value calibrated from bit 0 is then not a latency at all
    //
    // The same-line mutation leaves EH0 green and takes every other bit down,
    // which is the correct shape: with the floor back, bit 0's own target is
    // below it and coincides with the floor by accident, while the bits above
    // it no longer separate. That is precisely why the walk waits for the next
    // line, and the mutation demonstrates the reason rather than only the rule.

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
            "the fetch latency can be calibrated from it rather than assumed");

        for (int bit = 0; bit <= 6; ++bit) {
            const uint8_t hp = static_cast<uint8_t>(1u << bit);
            const int expected = 4 * (1 << bit) + latency;
            const int at = release_hpos(hp);
            std::cout << "[measured] HP bit " << bit << " (HP" << (bit + 2)
                      << ") released at hpos " << at << ", expected " << expected << "\n";
            checks.expect(at == expected,
                "EH" + std::to_string(bit) +
                " IR1 bit " + std::to_string(bit + 1) + " carries HP" + std::to_string(bit + 2) +
                ": a WAIT with only that horizontal bit set releases at hpos " +
                std::to_string(4 * (1 << bit)) + " plus the fetch latency "
                "(appendix A p274 instruction table)");
        }
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
