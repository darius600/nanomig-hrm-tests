// Test purpose: Verify Copper list-register strobes, address formation, terminal waiting, and automatic COP1 restart through public ports.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed pp. 25-26, 30, 32-35, 273, 290, and 298; frozen specification a26e813.
// Requirement IDs: COP-05, COP-07
// Non-goals: DMACON bit gating is tested separately at the real Agnus top level; this test does not inspect private Copper state.

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

namespace {

constexpr uint16_t COP1LCH = 0x080 >> 1;
constexpr uint16_t COP1LCL = 0x082 >> 1;
constexpr uint16_t COP2LCH = 0x084 >> 1;
constexpr uint16_t COP2LCL = 0x086 >> 1;
constexpr uint16_t COPJMP1 = 0x088 >> 1;
constexpr uint16_t COPJMP2 = 0x08A >> 1;
constexpr uint16_t COPINS = 0x08C >> 1;
constexpr uint16_t IDLE_REG = 0x0FF;

constexpr uint16_t COLOR00 = 0x180 >> 1;
constexpr uint16_t COLOR01 = 0x182 >> 1;
constexpr uint16_t COLOR02 = 0x184 >> 1;

struct MoveEvent {
    uint16_t reg;
    uint16_t data;
};

class CopperHarness {
public:
    explicit CopperHarness(bool ecs_mode = false) : dut(new Vagnus_copper) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = ecs_mode;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->sof = 0;
        dut->blit_busy = 0;
        dut->hpos = 0;
        dut->vpos = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;

        for (int i = 0; i < 12; ++i) {
            cycle();
        }
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) {
            cycle();
        }
    }

    ~CopperHarness() {
        dut->final();
        delete dut;
    }

    CopperHarness(const CopperHarness&) = delete;
    CopperHarness& operator=(const CopperHarness&) = delete;

    void install_signature_list(uint32_t word_address, uint16_t signature_reg) {
        memory[word_address + 0] = static_cast<uint16_t>(signature_reg << 1);
        memory[word_address + 1] = static_cast<uint16_t>(0x0100 | (signature_reg & 0x00FF));
        memory[word_address + 2] = 0xFFFF;
        memory[word_address + 3] = 0xFFFE;
    }

    void install_two_move_list(uint32_t word_address, uint16_t first_reg, uint16_t second_reg) {
        memory[word_address + 0] = static_cast<uint16_t>(first_reg << 1);
        memory[word_address + 1] = 0x0111;
        memory[word_address + 2] = static_cast<uint16_t>(second_reg << 1);
        memory[word_address + 3] = 0x0222;
        memory[word_address + 4] = 0xFFFF;
        memory[word_address + 5] = 0xFFFE;
    }

    void install_wait_then_forbidden_list(uint32_t word_address, uint16_t first_reg, uint16_t forbidden_reg) {
        memory[word_address + 0] = static_cast<uint16_t>(first_reg << 1);
        memory[word_address + 1] = 0x0111;
        memory[word_address + 2] = 0xFFFF;
        memory[word_address + 3] = 0xFFFE;
        memory[word_address + 4] = static_cast<uint16_t>(forbidden_reg << 1);
        memory[word_address + 5] = 0x0BAD;
    }

    void program_location(int index, uint32_t word_address, bool low_bit = false) {
        const uint16_t high = static_cast<uint16_t>((word_address >> 15) & 0x1F);
        const uint16_t low = static_cast<uint16_t>(((word_address & 0x7FFF) << 1) | (low_bit ? 1 : 0));
        program_location_raw(index, high, low);
    }

    void program_location_raw(int index, uint16_t high, uint16_t low) {
        write_reg(index == 1 ? COP1LCH : COP2LCH, high);
        write_reg(index == 1 ? COP1LCL : COP2LCL, low);
    }

    void strobe(int index) {
        write_reg(index == 1 ? COPJMP1 : COPJMP2, 0x0000);
        // The public strobe is pipelined onto a Copper clock opportunity.
        // Let that documented start request settle before a caller enables DMA;
        // accesses remain visible through the normal public handshake.
        run_cycles(4);
    }

    void pulse_sof() {
        dut->sof = 1;
        cycle();
        dut->sof = 0;
    }

    bool run_until(const std::function<bool()>& predicate, int max_cycles = 12000) {
        for (int i = 0; i < max_cycles; ++i) {
            if (predicate()) {
                return true;
            }
            cycle();
        }
        return predicate();
    }

    void run_cycles(int count) {
        for (int i = 0; i < count; ++i) {
            cycle();
        }
    }

    int move_count(uint16_t reg) const {
        int count = 0;
        for (const auto& event : moves) {
            if (event.reg == reg) {
                ++count;
            }
        }
        return count;
    }

    bool dma_allowed = false;
    std::vector<uint32_t> fetches;
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

    void write_reg(uint16_t word_reg, uint16_t data) {
        cycle(true, word_reg, data);
    }

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

            // Every non-dummy Copper transfer reads the current instruction
            // word address. A MOVE's second-word read simultaneously presents
            // its destination register on the public register bus.
            if (reg != IDLE_REG) {
                fetches.push_back(address);
            }
            if (reg != COPINS && reg != IDLE_REG) {
                moves.push_back({reg, data});
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
        if (condition) {
            std::cout << "PASS: " << label << "\n";
        } else {
            std::cerr << "FAIL: " << label << "\n";
            ++failures;
        }
    }

    int result() const { return failures == 0 ? 0 : 1; }
    int count() const { return failures; }

private:
    int failures = 0;
};

uint32_t observe_first_fetch(bool ecs, uint16_t high, uint16_t low) {
    CopperHarness h(ecs);
    h.program_location_raw(1, high, low);
    h.strobe(1);
    h.dma_allowed = true;
    const bool seen = h.run_until([&]() { return !h.fetches.empty(); });
    return seen ? h.fetches.front() : 0xFFFFFFFFu;
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    constexpr uint32_t A = 0x12345;
    constexpr uint32_t B = 0x23456;
    constexpr uint32_t C = 0x34567;

    {
        CopperHarness h;
        h.install_signature_list(A, COLOR00);
        h.program_location(1, A);
        h.strobe(1);
        h.dma_allowed = true;
        checks.expect(h.run_until([&]() { return h.move_count(COLOR00) == 1; }),
                      "T1 COPJMP1 executes the COP1LC signature list");
        const bool sequential = h.fetches.size() >= 2
                             && h.fetches[0] == A
                             && h.fetches[1] == A + 1;
        checks.expect(sequential,
                      "T1 first two instruction words are fetched sequentially from COP1LC");
        if (!sequential) {
            std::cerr << "  observed fetch word addresses:";
            for (const uint32_t address : h.fetches) {
                std::cerr << " 0x" << std::hex << address;
            }
            std::cerr << std::dec << "\n";
        }
    }

    {
        CopperHarness h;
        h.install_signature_list(A, COLOR00);
        h.install_signature_list(B, COLOR01);
        h.program_location(1, A);
        h.program_location(2, B);
        h.strobe(2);
        h.dma_allowed = true;
        checks.expect(h.run_until([&]() { return h.move_count(COLOR01) == 1; }),
                      "T2 COPJMP2 executes the COP2LC signature list");
        checks.expect(h.move_count(COLOR00) == 0,
                      "T2 COPJMP2 does not select COP1LC");
        h.strobe(1);
        checks.expect(h.run_until([&]() { return h.move_count(COLOR00) == 1; }),
                      "T2 COPJMP1 subsequently selects COP1LC");
    }

    {
        CopperHarness h;
        h.install_two_move_list(A, COLOR00, COLOR01);
        h.install_signature_list(C, COLOR02);
        h.program_location(1, A);
        h.strobe(1);
        h.dma_allowed = true;
        checks.expect(h.run_until([&]() { return h.move_count(COLOR00) == 1; }),
                      "T3 list A begins before COP1LC is updated");
        h.program_location(1, C);
        checks.expect(h.run_until([&]() { return h.move_count(COLOR01) == 1; }),
                      "T3 current list remains sequential after a location-only update");
        checks.expect(h.move_count(COLOR02) == 0,
                      "T3 rewriting COP1LC alone does not jump to the new list");
    }

    {
        const uint16_t low_even = 0x2468;
        const uint16_t low_odd = 0x2469;
        const uint16_t high = 0x001D;
        const uint32_t expected_low = low_even >> 1;

        const uint32_t aligned_even = observe_first_fetch(false, 0x0001, low_even);
        const uint32_t aligned_odd = observe_first_fetch(false, 0x0001, low_odd);
        checks.expect(aligned_even == aligned_odd,
                      "T4 COP1LCL bit 0 does not change the fetch word address");

        const uint32_t ocs_fetch = observe_first_fetch(false, high, low_even);
        const uint32_t ocs_expected = ((high & 0x0007u) << 15) | expected_low;
        checks.expect(ocs_fetch == ocs_expected,
                      "T4 OCS mode uses only the documented three high address bits");
        if (ocs_fetch != ocs_expected) {
            std::cerr << "  observed word address 0x" << std::hex << ocs_fetch
                      << ", HRM expected 0x" << ocs_expected << std::dec << "\n";
        }

        const uint32_t ecs_fetch = observe_first_fetch(true, high, low_even);
        const uint32_t ecs_expected = ((high & 0x001Fu) << 15) | expected_low;
        checks.expect(ecs_fetch == ecs_expected,
                      "T4 ECS mode uses the documented five high address bits");
    }

    {
        CopperHarness h;
        h.install_wait_then_forbidden_list(A, COLOR00, COLOR01);
        h.program_location(1, A);
        h.strobe(1);
        h.dma_allowed = true;
        checks.expect(h.run_until([&]() { return h.move_count(COLOR00) == 1; }),
                      "T6 signature before the terminal WAIT executes");
        h.run_cycles(5000);
        checks.expect(h.move_count(COLOR01) == 0,
                      "T6 instruction after $FFFF,$FFFE is not executed in the same field");
        h.pulse_sof();
        checks.expect(h.run_until([&]() { return h.move_count(COLOR00) == 2; }),
                      "T6 automatic field restart executes COP1LC again");
    }

    {
        CopperHarness h;
        h.install_signature_list(A, COLOR00);
        h.install_signature_list(B, COLOR01);
        h.program_location(1, A);
        h.program_location(2, B);
        h.strobe(2);
        h.dma_allowed = true;
        checks.expect(h.run_until([&]() { return h.move_count(COLOR01) == 1; }),
                      "T7 COPJMP2 starts list B before vertical blank");
        h.pulse_sof();
        checks.expect(h.run_until([&]() { return h.move_count(COLOR00) == 1; }),
                      "T7 automatic field restart selects COP1LC list A");
        checks.expect(h.move_count(COLOR01) == 1,
                      "T7 automatic field restart does not select COP2LC again");
    }

    {
        CopperHarness h;
        h.install_signature_list(A, COLOR00);
        h.install_signature_list(C, COLOR02);
        h.program_location(1, A);
        h.strobe(1);
        h.dma_allowed = true;
        checks.expect(h.run_until([&]() { return h.move_count(COLOR00) == 1; }),
                      "T8 original COP1LC list executes before the update");
        h.program_location(1, C);
        h.run_cycles(1200);
        checks.expect(h.move_count(COLOR02) == 0,
                      "T8 updated COP1LC does not redirect the current field without a strobe");
        h.pulse_sof();
        checks.expect(h.run_until([&]() { return h.move_count(COLOR02) == 1; }),
                      "T8 next automatic field restart uses the updated COP1LC");
    }

    if (checks.count() == 0) {
        std::cout << "FPGA-055 Copper register/control module checks passed.\n";
    } else {
        std::cerr << "FPGA-055 Copper register/control module checks failed: "
                  << checks.count() << " assertion(s).\n";
    }
    return checks.result();
}
