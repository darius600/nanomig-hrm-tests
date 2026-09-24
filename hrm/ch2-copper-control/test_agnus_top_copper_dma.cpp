// Test purpose: Verify the HRM DMAEN/COPEN Copper start and stop contract through the real Agnus top-level bus.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed pp. 25-26, 30, 222, and 298; frozen specification a26e813.
// Requirement IDs: COP-05, COP-07
// Non-goals: This test does not mirror DMACON logic in C++, inspect private state, or assert exact Copper cycle timing.
//
// Machine-readable mutation records for tools/check_gates.py --tests. All five
// T5 assertions were mutated at the STIMULUS on 05.09.2026, each reverted
// immediately after observing red. Each mutation turned exactly the assertion
// it aimed at red and left the other four green - which is what makes them
// five independent checks rather than one restated.
// MUTATION T5 stimulus red DMAEN set alongside COPEN in the COPEN-only case, so the Copper legitimately runs and the silence the assertion requires is gone
// MUTATION T5 stimulus red COPEN set alongside DMAEN in the DMAEN-only case, same silence broken from the other side
// MUTATION T5 stimulus red only COPEN enabled where the test means to enable both, so execution never starts
// MUTATION T5 stimulus red COP1LC programmed two words above the installed list, so execution begins somewhere other than the strobed address
// MUTATION T5 stimulus red COPEN left set where the test means to clear it, so the Copper keeps fetching and the assertion's freeze never happens

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus.h"

namespace {

constexpr uint16_t COP1LCH = 0x080 >> 1;
constexpr uint16_t COP1LCL = 0x082 >> 1;
constexpr uint16_t COPJMP1 = 0x088 >> 1;
constexpr uint16_t COPINS = 0x08C >> 1;
constexpr uint16_t DMACON = 0x096 >> 1;
constexpr uint16_t COLOR00 = 0x180 >> 1;
constexpr uint16_t IDLE_REG = 0x0FF;

struct CopperBusEvent {
    uint32_t word_address;
    uint16_t reg;
};

class AgnusHarness {
public:
    AgnusHarness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->data_in = 0;
        dut->address_in = 0;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->disk_dmal = 0;
        dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0;
        dut->a1k = 0;
        dut->ecs = 0;
        dut->aga = 0;
        dut->floppy_speed = 0;

        run_cycles(24);
        dut->reset = 0;
        run_cycles(12);
        events.clear();
    }

    ~AgnusHarness() {
        dut->final();
        delete dut;
    }

    AgnusHarness(const AgnusHarness&) = delete;
    AgnusHarness& operator=(const AgnusHarness&) = delete;

    void install_signature_list(uint32_t word_address) {
        memory[word_address + 0] = static_cast<uint16_t>(COLOR00 << 1);
        memory[word_address + 1] = 0x055A;
        memory[word_address + 2] = 0xFFFF;
        memory[word_address + 3] = 0xFFFE;
    }

    void install_long_move_list(uint32_t word_address, int move_count) {
        for (int i = 0; i < move_count; ++i) {
            memory[word_address + 2 * i] = static_cast<uint16_t>(COLOR00 << 1);
            memory[word_address + 2 * i + 1] = static_cast<uint16_t>(0x1000 + i);
        }
        memory[word_address + 2 * move_count] = 0xFFFF;
        memory[word_address + 2 * move_count + 1] = 0xFFFE;
    }

    void program_cop1(uint32_t word_address) {
        write_reg(COP1LCH, static_cast<uint16_t>((word_address >> 15) & 0x1F));
        write_reg(COP1LCL, static_cast<uint16_t>((word_address & 0x7FFF) << 1));
        write_reg(COPJMP1, 0x0000);
        // The HRM sequence strobes the location before DMA is enabled. Allow
        // the public request pipeline to settle without granting Copper DMA.
        run_cycles(12);
    }

    void set_dmacon(uint16_t value) {
        write_reg(DMACON, value);
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

    void clear_events() {
        events.clear();
    }

    int move_count() const {
        int count = 0;
        for (const auto& event : events) {
            if (event.reg == COLOR00) {
                ++count;
            }
        }
        return count;
    }

    std::vector<CopperBusEvent> events;

private:
    Vagnus* dut;
    std::unordered_map<uint32_t, uint16_t> memory;

    uint16_t read_memory(uint32_t word_address) const {
        const auto it = memory.find(word_address);
        return it == memory.end() ? 0xFFFF : it->second;
    }

    void write_reg(uint16_t word_reg, uint16_t data) {
        for (int timeout = 0; timeout < 4000; ++timeout) {
            if (cycle(true, word_reg, data)) {
                dut->aen = 0;
                dut->hwr = 0;
                dut->lwr = 0;
                dut->data_in = 0;
                dut->eval();
                return;
            }
        }
        std::cerr << "FAIL: CPU write timed out for register 0x"
                  << std::hex << word_reg << std::dec << "\n";
    }

    bool cycle(bool cpu_write = false, uint16_t cpu_reg = 0, uint16_t cpu_data = 0) {
        dut->cck = !dut->cck;
        dut->aen = cpu_write;
        dut->rd = 0;
        dut->hwr = cpu_write;
        dut->lwr = cpu_write;
        dut->address_in = cpu_reg;
        dut->data_in = cpu_write ? cpu_data : 0;
        dut->eval();

        const bool cpu_granted = cpu_write
                              && dut->cpu_custom
                              && dut->reg_address_out == cpu_reg;

        if (dut->dbr && !dut->dbwe) {
            dut->data_in = read_memory(dut->address_out);
            dut->eval();
        }

        if (dut->dbr && !dut->refresh && dut->reg_address_out != IDLE_REG) {
            const uint16_t reg = dut->reg_address_out;
            if (reg == COPINS || reg == COLOR00) {
                events.push_back({dut->address_out, reg});
            }
        }

        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        return cpu_granted;
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        if (condition) {
            std::cout << "PASS: " << label << "\n";
            ++passes;
        } else {
            std::cerr << "FAIL: " << label << "\n";
            ++failures;
        }
    }

    int result() const { return failures == 0 ? 0 : 1; }
    int count() const { return failures; }
    int passed() const { return passes; }

private:
    int failures = 0;
    int passes = 0;
};

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    constexpr uint32_t A = 0x02000;

    {
        AgnusHarness h;
        h.install_signature_list(A);
        h.program_cop1(A);
        h.set_dmacon(0x8080); // Set COPEN only; DMAEN remains clear.
        h.clear_events();
        h.run_cycles(3000);
        checks.expect(h.events.empty(),
                      "T5 COPEN without DMAEN produces no Copper fetch or MOVE effect [claims: ch2-appa-copins.c144, ch2-appa-copins.c145, ch2-appa-copins.c146, ch2-starting-the-copper-after-reset.c3, ch2-stopping-the-copper.c7]");
    }

    {
        AgnusHarness h;
        h.install_signature_list(A);
        h.program_cop1(A);
        h.set_dmacon(0x8200); // Set DMAEN only; COPEN remains clear.
        h.clear_events();
        h.run_cycles(3000);
        checks.expect(h.events.empty(),
                      "T5 DMAEN without COPEN produces no Copper fetch or MOVE effect");
    }

    {
        AgnusHarness h;
        h.install_long_move_list(A, 32);
        h.program_cop1(A);
        h.clear_events();
        h.set_dmacon(0x8280); // Set DMAEN and COPEN through real DMACON logic.
        const bool moved = h.run_until([&]() { return h.move_count() >= 1; });
        checks.expect(moved,
                      "T5 DMAEN and COPEN together start Copper execution");
        checks.expect(!h.events.empty() && h.events.front().word_address == A,
                      "T5 enabled execution begins at the strobed COP1LC address");

        h.set_dmacon(0x0080); // Clear COPEN at the next CPU-accessible boundary.
        const size_t transfers_after_clear = h.events.size();
        const int moves_after_clear = h.move_count();
        h.run_cycles(3000);
        checks.expect(h.events.size() == transfers_after_clear
                   && h.move_count() == moves_after_clear,
                      "T5 clearing COPEN stops further Copper fetch and MOVE effects");
    }

    if (checks.count() == 0) {
        std::cout << "FPGA-055 Agnus top-level DMACON checks passed.\n";
    } else {
        std::cerr << "FPGA-055 Agnus top-level DMACON checks failed: "
                  << checks.count() << " assertion(s).\n";
    }

    // This package builds two fixtures (this one and agnus_copper_control),
    // and the coverage ledger sums every program's own terminal summary, so
    // each binary declares its own -- see test_agnus_copper_control.cpp's
    // matching line.
    std::cout << checks.passed() << " passed, " << checks.count()
              << " failed.\n";
    return checks.result();
}
