// Test purpose: Assert the COPINS dummy address the Copper presents while fetching instructions, and the number of memory cycles each instruction form consumes.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Appendix A (COPINS $08C) and printed p. 20; frozen specification 326802b.
// Requirement IDs: COP-02
// Non-goals: The "four" and "six memory cycle times" figures and the odd-slot parity on p.20 are arbitration properties of the surrounding Agnus, not of this module. A blocked WAIT has no defined cycle count.
//
// Why this test exists: FPGA-056 to FPGA-058 all filtered COPINS out in order
// to find MOVE writes, i.e. they relied on R1 without ever establishing it.
// FPGA-059 then overclaimed that nothing further was testable at this boundary.
//
// MUTATION C1 stimulus red: run_cycles after start() cut from 4000 to 4 (too short to reach the second MOVE)
//   -> FAIL as expected (only one instruction's first word gets fetched, not two).
// MUTATION C2 stimulus red: same run_cycles cut (4000 -> 4) -> both C2 checks FAIL as expected (the second
//   MOVE's destination cycle, and its register/data, never appear).
// MUTATION C3 stimulus red: run_cycles after start() cut from 4000 to 2 (too short for the MOVE's second word)
//   -> FAIL as expected.
// MUTATION C4 stimulus red: same run_cycles cut (4000 -> 2) on the SKIP scenario -> FAIL as expected.
// MUTATION C5/C6 stimulus red (already red at baseline - genuine finding, not a broken assertion): a debug
//   probe showed the "blocks then wakes" WAIT actually consumes 2 memory cycles, identical to a MOVE (p.20
//   claims 3, "one extra to wake up"). A stimulus mutation retargeting the WAIT to a position already
//   satisfied on arrival (seek_beam 0x10, target VP $00) still produced FAIL for both checks with the same
//   2-cycle count - the "extra wake-up cycle" is not reproducible from any reachable stimulus in this harness.
//   Since C3 and C4 (same grants_for_instruction_at() mechanism, same file) both correctly confirm MOVE and
//   SKIP each cost exactly 2, the counting mechanism itself is sound; C5/C6's FAIL is a genuine DUT/RTL gap.

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

constexpr uint32_t LIST = 0x00002000;

constexpr uint16_t ir1_pos(uint8_t vp, uint8_t hp) {
    return static_cast<uint16_t>((vp << 8) | ((hp & 0x7F) << 1) | 1);
}
constexpr uint16_t ir2_mask(bool bfd, uint8_t ve, uint8_t he, bool skip) {
    return static_cast<uint16_t>((bfd ? 0x8000 : 0) | ((ve & 0x7F) << 8) | ((he & 0x7F) << 1) | (skip ? 1 : 0));
}
constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

// One granted Copper bus access.
struct Grant {
    uint16_t reg;       // register address presented on the bus
    uint32_t address;   // chip address fetched
    uint16_t data;
};

class CopperHarness {
public:
    CopperHarness() : dut(new Vagnus_copper) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1; dut->ecs = 0;
        dut->enadma = 1; dut->ackdma = 0; dut->sof = 0; dut->blit_busy = 0;
        dut->hpos = 0; dut->vpos = 0; dut->data_in = 0; dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 12; ++i) cycle();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) cycle();
    }
    ~CopperHarness() { dut->final(); delete dut; }
    CopperHarness(const CopperHarness&) = delete;
    CopperHarness& operator=(const CopperHarness&) = delete;

    void install(const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) memory[LIST + i] = words[i];
    }
    void seek_beam(uint16_t v) { vpos = v; hpos = 0; dut->vpos = vpos; dut->hpos = hpos; }

    void start() {
        write_reg(COP1LCH, static_cast<uint16_t>((LIST >> 15) & 0x1F));
        write_reg(COP1LCL, static_cast<uint16_t>((LIST & 0x7FFF) << 1));
        write_reg(COPJMP1, 0x0000);
        run_cycles(4);
        grants.clear();          // ignore the start handshake itself
        dma_allowed = true;
    }
    void run_cycles(int n) { for (int i = 0; i < n; ++i) cycle(); }

    // Granted cycles consumed while executing the instruction whose first word
    // sits at word offset `first_word`, i.e. all grants fetching from that
    // instruction's own addresses plus the destination cycle that follows.
    int grants_for_instruction_at(uint32_t first_word) const {
        int n = 0;
        for (const auto& g : grants) {
            if (g.address == LIST + first_word || g.address == LIST + first_word + 1) ++n;
        }
        return n;
    }

    bool dma_allowed = false;
    std::vector<Grant> grants;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0, vpos = 0;

    uint16_t read_memory(uint32_t a) const {
        const auto it = memory.find(a);
        return it == memory.end() ? 0xFFFF : it->second;
    }
    void write_reg(uint16_t r, uint16_t d) { cycle(true, r, d); }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        const bool accepted = !cpu_write && dma_allowed && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0; dut->reg_address_in = cpu_reg; dut->data_in = cpu_data;
        } else if (accepted) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            const uint16_t data = read_memory(address);
            dut->ackdma = 1; dut->reg_address_in = reg; dut->data_in = data;
            dut->eval();
            if (reg != IDLE_REG) grants.push_back({reg, address, data});
        } else {
            dut->ackdma = 0; dut->reg_address_in = IDLE_REG; dut->data_in = 0;
        }
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        if (++hpos == 454) { hpos = 0; vpos = static_cast<uint16_t>((vpos + 1) & 0x00FF); }
        dut->hpos = hpos; dut->vpos = vpos;
    }
};

class Checks {
public:
    void expect(bool c, const std::string& l) {
        std::cout << (c ? "PASS: " : "FAIL: ") << l << "\n";
        c ? ++passed : ++failed;
    }
    int passed = 0, failed = 0;
};

constexpr uint16_t WAIT_ALL = 0xFFFE;
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // C1/C2 - Appendix A: instruction word fetches carry COPINS; a MOVE's
    // second cycle carries the destination register instead.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install({move_ir1(COLOR00), 0x0111,
                   move_ir1(COLOR01), 0x0222,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(4000);

        int copins_on_first_words = 0, dest_on_second_words = 0, violations = 0;
        for (const auto& g : h.grants) {
            const uint32_t off = g.address - LIST;
            const bool is_move_first = (off == 0 || off == 2);
            const bool is_move_second = (off == 1 || off == 3);
            if (is_move_first && g.reg == COPINS) ++copins_on_first_words;
            if (is_move_second && g.reg != COPINS) ++dest_on_second_words;
            if (is_move_second && g.reg == COPINS) ++violations;   // forbidden by Appendix A
        }
        checks.expect(copins_on_first_words == 2,
                      "C1 each MOVE's instruction-word fetch presents COPINS (Appendix A)");
        checks.expect(dest_on_second_words == 2 && violations == 0,
                      "C2 a MOVE's second cycle presents the destination register, never COPINS (Appendix A)");

        // The destination cycles must carry the right registers and data.
        bool color00_ok = false, color01_ok = false;
        for (const auto& g : h.grants) {
            if (g.reg == COLOR00 && g.data == 0x0111) color00_ok = true;
            if (g.reg == COLOR01 && g.data == 0x0222) color01_ok = true;
        }
        checks.expect(color00_ok && color01_ok, "C2 the destination cycles carry the expected register and data");
    }

    // C3 - p.20: a MOVE consumes two memory cycles.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install({move_ir1(COLOR00), 0x0333,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(4000);
        checks.expect(h.grants_for_instruction_at(0) == 2, "C3 a MOVE consumes two memory cycles (p.20)");
    }

    // C4 - p.20: a SKIP also consumes two.
    {
        CopperHarness h;
        h.seek_beam(0x70);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, true),   // condition true
                   move_ir1(COLOR00), 0x0444,                                // skipped
                   move_ir1(COLOR01), 0x0555,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(4000);
        checks.expect(h.grants_for_instruction_at(0) == 2, "C4 a SKIP consumes two memory cycles (p.20)");
    }

    // C5 - p.20: a WAIT consumes three, "one extra memory cycle to wake up".
    // Only an already-satisfied WAIT has a defined count; a blocked one uses
    // no further bus cycles by definition.
    {
        // The WAIT must genuinely block and then wake, because the HRM's extra
        // cycle is explicitly the wake-up. An earlier revision used a WAIT that
        // was already satisfied on arrival; such a WAIT never wakes, so it
        // could not exhibit the claimed third cycle. That was a defect of the
        // test scenario - and of the specification's reasoning behind it.
        CopperHarness h;
        h.seek_beam(0x10);
        h.install({ir1_pos(0x20, 0x00), ir2_mask(true, 0x7F, 0x00, false),   // blocks, then wakes
                   move_ir1(COLOR00), 0x0666,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(20000);
        const int wait_cycles = h.grants_for_instruction_at(0);
        const int move_cycles = h.grants_for_instruction_at(2);
        checks.expect(wait_cycles == 3, "C5 a satisfied WAIT consumes three memory cycles (p.20)");
        // C6 - the claim stated as the relative one the HRM actually makes.
        checks.expect(wait_cycles == move_cycles + 1,
                      "C6 a WAIT consumes exactly one memory cycle more than a MOVE (p.20 'one extra to wake up')");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
