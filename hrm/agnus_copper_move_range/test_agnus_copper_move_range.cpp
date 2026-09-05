// Test purpose: Verify which destination registers a Copper MOVE may write, in OCS and ECS, against the HRM register-protection rules.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed pp. 21 and 25, Appendix B register list, and Appendix C p. 310; frozen specification 19b2eb2.
// Requirement IDs: COP-03
// Non-goals: What the Copper does after a rejected MOVE is not asserted - the HRM does not define it. IR1 bits 15-9 non-zero, an exhaustive address sweep, AGA extensions and MOVE timing are out of scope.
//
// Source conflict, resolved in the specification and repeated here so the code
// is readable on its own: print page 21 states the writable ranges as
// $20/$10/below-$10 while print page 25 and Appendix B state them as
// $80/$40-$7E/$00-$3E. The two are incompatible as byte addresses. Page 25 and
// Appendix B are used as the oracle because they agree with each other and are
// unambiguous full byte addresses.
//
// MUTATION T1 stimulus red: swapped move_reaches() target from COLOR00 to BLTCON0 (still CDANG=0) -> FAIL as expected.
// MUTATION T2 stimulus red: swapped move_reaches() target from BPL1PTH to BLTCON0 (still CDANG=0) -> FAIL as expected.
// MUTATION T3 stimulus red: flipped cdang argument false->true on the BLTCON0 call -> FAIL as expected (write now succeeds, unblocked assertion fails).
// MUTATION T4 stimulus red: flipped cdang argument false->true on the BLTSIZE call -> FAIL as expected.
// MUTATION T5 stimulus red: flipped cdang argument true->false on the BLTCON0 call -> FAIL as expected (write now blocked).
// MUTATION T6 stimulus red: flipped cdang argument true->false on the BLTSIZE call -> FAIL as expected.
// MUTATION T7 stimulus red: flipped ecs argument false->true on the COPCON call -> FAIL as expected (ECS+CDANG unlocks COPCON, per T10).
// MUTATION T8 stimulus red: swapped move_reaches() target from DMACON to COPCON (still CDANG=1) -> FAIL as expected.
// MUTATION T9 stimulus green: swapped move_reaches() target from BLTCON0 to COLOR00 (ecs=1,cdang=0, unprotected register) -> PASS as expected;
//   confirms the harness/check mechanism does register a write in ECS mode, so T9's baseline FAIL is a genuine
//   DUT non-conformance (Appendix C p.310 range scaling not implemented for ECS+CDANG=0), not a broken assertion.
// MUTATION T9b stimulus red: swapped move_reaches() target from BLTCON0 to BLTSIZE (still ecs=1,cdang=0) -> still FAIL;
//   the gap is range-wide (both CDANG-range boundaries), not isolated to one register constant, reinforcing it is a real RTL gap.
// MUTATION T10 stimulus red: flipped ecs argument true->false on the COPCON call -> FAIL as expected (OCS+CDANG=1 still blocks COPCON, per T7).
// MUTATION T11 stimulus red: forced cdang=true and set_cdang_explicitly=true (simulating "reset fails to clear CDANG") -> FAIL as expected.
// MUTATION T12 stimulus red: changed the data written by install_single_move from 0xA55A to 0xA55B, oracle data left at 0xA55A -> FAIL as expected.

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

namespace {

// Register addresses are given as byte addresses in the HRM. The Copper
// instruction encodes DA8-1, i.e. the byte address shifted right by one, and
// that is also the width of the module's public register ports.
constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPINS = to_reg(0x08C);
constexpr uint16_t COPCON = to_reg(0x02E);
constexpr uint16_t IDLE_REG = 0x0FF;

// Destinations probed by the test, chosen at the range boundaries named on
// print page 25 and in Appendix B.
constexpr uint16_t COLOR00 = to_reg(0x180);   // always writable
constexpr uint16_t BPL1PTH = to_reg(0x0E0);   // always writable
constexpr uint16_t DMACON  = to_reg(0x096);   // always writable, just above $80
constexpr uint16_t BLTCON0 = to_reg(0x040);   // CDANG-protected, low boundary
constexpr uint16_t BLTSIZE = to_reg(0x07E);   // CDANG-protected, high boundary

constexpr uint32_t LIST_ADDRESS = 0x00002000;

struct MoveEvent {
    uint16_t reg;
    uint16_t data;
};

class CopperHarness {
public:
    explicit CopperHarness(bool ecs_mode) : dut(new Vagnus_copper) {
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

    // A minimal list: one MOVE of `data` to `reg`, then a terminating WAIT.
    void install_single_move(uint16_t reg, uint16_t data) {
        memory[LIST_ADDRESS + 0] = static_cast<uint16_t>(reg << 1);
        memory[LIST_ADDRESS + 1] = data;
        memory[LIST_ADDRESS + 2] = 0xFFFF;
        memory[LIST_ADDRESS + 3] = 0xFFFE;
    }

    void set_cdang(bool value) {
        write_reg(COPCON, value ? 0x0002 : 0x0000);
        run_cycles(2);
    }

    void start_list() {
        const uint16_t high = static_cast<uint16_t>((LIST_ADDRESS >> 15) & 0x1F);
        const uint16_t low = static_cast<uint16_t>((LIST_ADDRESS & 0x7FFF) << 1);
        write_reg(COP1LCH, high);
        write_reg(COP1LCL, low);
        write_reg(COPJMP1, 0x0000);
        run_cycles(4);
        dma_allowed = true;
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

    bool saw_move(uint16_t reg, uint16_t data) const {
        for (const auto& event : moves) {
            if (event.reg == reg && event.data == data) {
                return true;
            }
        }
        return false;
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

            // A MOVE's second-word fetch presents the destination register on
            // the public register bus. COPINS is the Copper's own instruction
            // fetch and is not a MOVE to a destination.
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
            ++passed;
        } else {
            std::cout << "FAIL: " << label << "\n";
            ++failed;
        }
    }

    int passed = 0;
    int failed = 0;
};

// Runs one MOVE of 0x0ABC to `reg` and reports whether the write appeared.
bool move_reaches(bool ecs, bool cdang, uint16_t reg, bool set_cdang_explicitly = true) {
    CopperHarness h(ecs);
    if (set_cdang_explicitly) {
        h.set_cdang(cdang);
    }
    h.install_single_move(reg, 0x0ABC);
    h.start_list();
    h.run_cycles(600);
    return h.move_count(reg) > 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // T1, T2, T8 - permitted destinations must actually work, otherwise a
    // negative result below would not be meaningful.
    checks.expect(move_reaches(false, false, COLOR00), "T1 OCS CDANG=0: MOVE to COLOR00 ($180) is written");
    checks.expect(move_reaches(false, false, BPL1PTH), "T2 OCS CDANG=0: MOVE to BPL1PTH ($0E0) is written");
    checks.expect(move_reaches(false, true, DMACON), "T8 OCS CDANG=1: MOVE to DMACON ($096) is written");

    // T3, T4 - the CDANG-protected range is blocked while CDANG is clear.
    checks.expect(!move_reaches(false, false, BLTCON0), "T3 OCS CDANG=0: MOVE to BLTCON0 ($040) is blocked");
    checks.expect(!move_reaches(false, false, BLTSIZE), "T4 OCS CDANG=0: MOVE to BLTSIZE ($07E) is blocked");

    // T5, T6 - and permitted once CDANG is set.
    checks.expect(move_reaches(false, true, BLTCON0), "T5 OCS CDANG=1: MOVE to BLTCON0 ($040) is written");
    checks.expect(move_reaches(false, true, BLTSIZE), "T6 OCS CDANG=1: MOVE to BLTSIZE ($07E) is written");

    // T7 - below the protected range nothing is writable in OCS, not even with
    // CDANG set. COPCON itself lives here, so the Copper cannot set its own
    // danger bit.
    checks.expect(!move_reaches(false, true, COPCON), "T7 OCS CDANG=1: MOVE to COPCON ($02E) is blocked");

    // T9, T10 - Appendix C p. 310: ECS shifts the scale by one step. With
    // CDANG clear the ECS Copper already reaches the blitter range, and with
    // CDANG set it reaches every chip register.
    checks.expect(move_reaches(true, false, BLTCON0), "T9 ECS CDANG=0: MOVE to BLTCON0 ($040) is written (Appendix C p.310)");
    checks.expect(move_reaches(true, true, COPCON), "T10 ECS CDANG=1: MOVE to COPCON ($02E) is written (Appendix C p.310)");

    // T11 - print page 25 warns CDANG is cleared by reset. Without any COPCON
    // write the Copper must behave as CDANG = 0.
    checks.expect(!move_reaches(false, false, BLTCON0, /*set_cdang_explicitly=*/false),
                  "T11 OCS after reset, no COPCON write: MOVE to BLTCON0 ($040) is blocked");

    // T12 - the 16 bits of IR2 reach the destination unmodified.
    {
        CopperHarness h(false);
        h.set_cdang(false);
        h.install_single_move(COLOR00, 0xA55A);
        h.start_list();
        h.run_cycles(600);
        checks.expect(h.saw_move(COLOR00, 0xA55A), "T12 OCS: IR2 data $A55A arrives at COLOR00 unmodified");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
