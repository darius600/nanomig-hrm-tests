// Test purpose: Verify the three Copper instruction forms, their bit-0 discrimination, sequential two-word list execution, relocation and both list pointers.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed pp. 20, 21, 22, 26-27, 30, 31, 32 and 36; frozen specification d9cad33.
// Requirement IDs: COP-02, COP-06
// Non-goals: Reserved encodings are marked "X = don't care" on p.36, so no conformance claim about them is possible. Instruction cycle counts (p.20) are arbitration-level and need the Agnus top level. Beam-order list authoring advice is guidance, not hardware behaviour. The vertical-blank COP1LC reload belongs to COP-07.
//
// Machine-readable mutation records for tools/check_gates.py --tests. All eight
// assertions were mutated at the STIMULUS on 05.09.2026 - the scenario changed,
// never the expected value - and each was reverted immediately after observing
// red. Where an ID carries two assertions, only the one named by the mutation
// went red; that is the point of the mutation and not a partial result.
// MUTATION L1 stimulus red the last two MOVEs swapped in the list, so all three still run but not in the order the assertion names
// MUTATION L2 stimulus red the WAIT target moved from line $40 to line $10, which the beam has already passed, so the following MOVE runs immediately instead of waiting
// MUTATION L3 stimulus red the SKIP bit cleared, turning the instruction into a WAIT, so nothing is suppressed and the MOVE the assertion expects to be skipped runs
// MUTATION L4 stimulus red IR1 bit 0 cleared, so the word really is a MOVE to COLOR00 and the register the assertion says is never written gets written
// MUTATION L5 stimulus red the rewrite of COP1LC removed, so the strobe restarts list A instead of relocating to list B
// MUTATION L6 stimulus red COP2LC pointed at the uninstalled LIST_A instead of LIST_B, so running from the second pointer reaches no instruction at all
// MUTATION L7 stimulus red the terminal WAIT removed, so the instructions the assertion says are unreachable become reachable
// MUTATION L8 stimulus red the data word that deliberately looks like a MOVE opcode replaced by an ordinary constant, so the scenario no longer poses the question the assertion answers

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
constexpr uint16_t COP2LCH = to_reg(0x084);
constexpr uint16_t COP2LCL = to_reg(0x086);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPJMP2 = to_reg(0x08A);
constexpr uint16_t COPINS = to_reg(0x08C);
constexpr uint16_t IDLE_REG = 0x0FF;

constexpr uint16_t COLOR00 = to_reg(0x180);
constexpr uint16_t COLOR01 = to_reg(0x182);
constexpr uint16_t COLOR02 = to_reg(0x184);
constexpr uint16_t COLOR03 = to_reg(0x186);

constexpr uint32_t LIST_A = 0x00002000;
constexpr uint32_t LIST_B = 0x00003000;

// Encodings straight from the HRM tables (pp. 21, 22, 31, 36).
constexpr uint16_t ir1_pos(uint8_t vp, uint8_t hp) {
    return static_cast<uint16_t>((vp << 8) | ((hp & 0x7F) << 1) | 1);   // bit0 = 1: WAIT or SKIP
}
constexpr uint16_t ir2_mask(bool bfd, uint8_t ve, uint8_t he, bool skip) {
    return static_cast<uint16_t>((bfd ? 0x8000 : 0) | ((ve & 0x7F) << 8) | ((he & 0x7F) << 1) | (skip ? 1 : 0));
}
constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }  // bit0 = 0: MOVE

struct MoveEvent {
    uint16_t reg;
    uint16_t data;
    uint16_t vpos;
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

    void install(uint32_t at, const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) memory[at + i] = words[i];
    }

    void seek_beam(uint16_t target_vpos) {
        vpos = target_vpos;
        hpos = 0;
        dut->vpos = vpos;
        dut->hpos = hpos;
    }

    void program_location(int index, uint32_t word_address) {
        write_reg(index == 1 ? COP1LCH : COP2LCH, static_cast<uint16_t>((word_address >> 15) & 0x1F));
        write_reg(index == 1 ? COP1LCL : COP2LCL, static_cast<uint16_t>((word_address & 0x7FFF) << 1));
    }

    void strobe(int index) {
        write_reg(index == 1 ? COPJMP1 : COPJMP2, 0x0000);
        run_cycles(4);
    }

    void start(int index, uint32_t word_address) {
        program_location(index, word_address);
        strobe(index);
        dma_allowed = true;
    }

    void run_cycles(int count) { for (int i = 0; i < count; ++i) cycle(); }

    int move_count(uint16_t reg) const {
        int n = 0;
        for (const auto& e : moves) if (e.reg == reg) ++n;
        return n;
    }
    int first_move_vpos(uint16_t reg) const {
        for (const auto& e : moves) if (e.reg == reg) return static_cast<int>(e.vpos);
        return -1;
    }
    // Index of the first write to `reg` within the observed write sequence.
    int order_of(uint16_t reg) const {
        for (size_t i = 0; i < moves.size(); ++i) if (moves[i].reg == reg) return static_cast<int>(i);
        return -1;
    }
    uint16_t data_of(uint16_t reg) const {
        for (const auto& e : moves) if (e.reg == reg) return e.data;
        return 0;
    }

    bool dma_allowed = false;
    std::vector<MoveEvent> moves;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0;
    uint16_t vpos = 0;

    uint16_t read_memory(uint32_t a) const {
        const auto it = memory.find(a);
        return it == memory.end() ? 0xFFFF : it->second;
    }
    void write_reg(uint16_t reg, uint16_t data) { cycle(true, reg, data); }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        const bool accepted = !cpu_write && dma_allowed && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (accepted) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            const uint16_t data = read_memory(address);
            dut->ackdma = 1;
            dut->reg_address_in = reg;
            dut->data_in = data;
            dut->eval();
            if (reg != COPINS && reg != IDLE_REG) moves.push_back({reg, data, vpos});
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = IDLE_REG;
            dut->data_in = 0;
        }
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        if (++hpos == 454) { hpos = 0; vpos = static_cast<uint16_t>((vpos + 1) & 0x00FF); }
        dut->hpos = hpos;
        dut->vpos = vpos;
    }
};

class Checks {
public:
    void expect(bool c, const std::string& l) {
        std::cout << (c ? "PASS: " : "FAIL: ") << l << "\n";
        c ? ++passed : ++failed;
    }
    // Records an unbound-claim reason without checking anything: does not
    // touch passed/failed, so it cannot change the suite's pass/fail counts.
    void note(const std::string& l) { std::cout << "NOTE: " << l << "\n"; }
    int passed = 0, failed = 0;
};

constexpr uint16_t WAIT_ALL = 0xFFFE;   // BFD=1, VE=$7F, HE=$7F, bit0=0
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // COP2-what-is-a-copper-instruction.c2 ("the instruction set contains
    // exactly three instructions") and COP2-stopping-the-copper.c1 ("no stop
    // instruction is provided") are both UNBOUND. Both are exhaustiveness/
    // completeness claims about the whole instruction SET - no 4th form and
    // no stop opcode exist - and every assertion in this suite tests the
    // BEHAVIOUR of a decoded MOVE, WAIT or SKIP, never the absence of a
    // fourth kind. Demonstrating that negative would need a claim this task
    // does not license writing. Reason: no assertion measures it.
    checks.note("UNBOUND COP2-what-is-a-copper-instruction.c2 -- no assertion measures it "
                 "(exhaustiveness of the 3-instruction set; no assertion tests for the absence of a 4th form) [claims: ch2-what-is-a-copper-instruction.c1]");
    checks.note("UNBOUND COP2-stopping-the-copper.c1 -- no assertion measures it "
                 "(absence of a stop opcode; not testable as a positive behavioural assertion) [claims: ch2-stopping-the-copper.c1]");

    // L1 - p.27: multiple MOVEs may follow a single WAIT, and they execute in
    // list order.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install(LIST_A, {ir1_pos(0x30, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                           move_ir1(COLOR00), 0x0111,
                           move_ir1(COLOR01), 0x0222,
                           move_ir1(COLOR02), 0x0333,
                           TERMINAL_IR1, WAIT_ALL});
        h.start(1, LIST_A);
        h.run_cycles(8000);
        const bool all_ran = h.move_count(COLOR00) == 1 && h.move_count(COLOR01) == 1 && h.move_count(COLOR02) == 1;
        checks.expect(all_ran, "L1 three MOVEs after one WAIT all execute (p.27) "
                      "[claims: COP2-putting-together-a-copper-instruction-list.c17, ch2-putting-together-a-copper-instruction-list.c21]");
        checks.expect(all_ran && h.order_of(COLOR00) < h.order_of(COLOR01) && h.order_of(COLOR01) < h.order_of(COLOR02),
                      "L1 the three MOVEs execute in list order");
    }

    // L2 - MOVE, WAIT, MOVE: both MOVEs run, the second not before the WAIT.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install(LIST_A, {move_ir1(COLOR00), 0x0444,
                           ir1_pos(0x40, 0x00), ir2_mask(true, 0x7F, 0x00, false),
                           move_ir1(COLOR01), 0x0555,
                           TERMINAL_IR1, WAIT_ALL});
        h.start(1, LIST_A);
        // Reaching line $40 from line $20 needs 32 lines of 454 cycles, so the
        // budget must exceed ~14.5k cycles. An earlier revision used 12k and
        // failed here purely for lack of run time - a harness defect, not a
        // Copper one.
        h.run_cycles(20000);
        checks.expect(h.move_count(COLOR00) == 1 && h.move_count(COLOR01) == 1, "L2 MOVE, WAIT, MOVE: both MOVEs execute");
        checks.expect(h.first_move_vpos(COLOR01) >= 0x40, "L2 the MOVE after the WAIT does not run before its position");
    }

    // L3 - a list containing all three forms. The SKIP suppresses exactly one
    // instruction, the following one still runs.
    {
        CopperHarness h;
        h.seek_beam(0x60);
        h.install(LIST_A, {move_ir1(COLOR00), 0x0666,
                           ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, true),   // SKIP, condition true
                           move_ir1(COLOR01), 0x0777,                                // skipped
                           move_ir1(COLOR02), 0x0888,                                // runs
                           TERMINAL_IR1, WAIT_ALL});
        h.start(1, LIST_A);
        h.run_cycles(8000);
        checks.expect(h.move_count(COLOR00) == 1 && h.move_count(COLOR01) == 0 && h.move_count(COLOR02) == 1,
                      "L3 MOVE/SKIP/MOVE list: the SKIP suppresses exactly one instruction");
    }

    // L4 - IR1 bit 0 alone decides MOVE versus beam-compare. The same first
    // word with bit 0 set is not a MOVE, so no write to that register happens.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        // COLOR00's byte address $180 with bit 0 forced to 1 becomes a WAIT for
        // VP=$01, HP=$40 - not a MOVE. The following MOVE proves the list ran.
        h.install(LIST_A, {static_cast<uint16_t>(move_ir1(COLOR00) | 1), ir2_mask(true, 0x7F, 0x00, false),
                           move_ir1(COLOR01), 0x0999,
                           TERMINAL_IR1, WAIT_ALL});
        h.start(1, LIST_A);
        h.run_cycles(8000);
        checks.expect(h.move_count(COLOR00) == 0, "L4 IR1 bit 0 set: the word is a beam compare, not a MOVE to that register "
                      "[claims: COP2-the-move-instruction.c4, COP2-the-wait-instruction.c10, COP2-the-skip-instruction.c6, "
                      "COP2-summary-of-copper-instructions.c91, COP2-summary-of-copper-instructions.c93, COP2-summary-of-copper-instructions.c95, "
                      "COP2-copins-encoding.c91, COP2-copins-encoding.c93, COP2-copins-encoding.c95, ch2-appa-copins.c106, ch2-appa-copins.c104, ch2-summary-of-copper-instructions.c91, ch2-summary-of-copper-instructions.c93, ch2-summary-of-copper-instructions.c95, ch2-the-move-instruction.c4, ch2-the-skip-instruction.c4, ch2-the-wait-instruction.c7]");
        checks.expect(h.move_count(COLOR01) == 1, "L4 the list continues past it");
    }

    // L5 - p.32: relocation. Start in list A, then point COP1LC at list B and
    // strobe COPJMP1; execution continues in list B.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install(LIST_A, {move_ir1(COLOR00), 0x0AAA,
                           TERMINAL_IR1, WAIT_ALL});
        h.install(LIST_B, {move_ir1(COLOR01), 0x0BBB,
                           TERMINAL_IR1, WAIT_ALL});
        h.start(1, LIST_A);
        h.run_cycles(2000);
        const bool a_ran = h.move_count(COLOR00) == 1 && h.move_count(COLOR01) == 0;
        h.program_location(1, LIST_B);
        h.strobe(1);
        h.run_cycles(2000);
        checks.expect(a_ran, "L5 list A runs first [claims: ch2-using-the-copper-in-interlaced-mode.c20, ch2-using-the-copper-in-interlaced-mode.c22]");
        checks.expect(h.move_count(COLOR01) == 1, "L5 after rewriting COP1LC and strobing COPJMP1 execution continues in list B (p.32)");
    }

    // L6 - p.32: the same works through the second pointer.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install(LIST_B, {move_ir1(COLOR03), 0x0CCC,
                           TERMINAL_IR1, WAIT_ALL});
        h.start(2, LIST_B);
        h.run_cycles(4000);
        checks.expect(h.move_count(COLOR03) == 1, "L6 a list can be run from COP2LC via COPJMP2 (p.32)");
    }

    // L7 - p.30: instructions after a terminal WAIT are never reached.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install(LIST_A, {move_ir1(COLOR00), 0x0DDD,
                           TERMINAL_IR1, WAIT_ALL,
                           move_ir1(COLOR01), 0x0EEE});
        h.start(1, LIST_A);
        h.run_cycles(20000);
        checks.expect(h.move_count(COLOR00) == 1 && h.move_count(COLOR01) == 0,
                      "L7 instructions after a terminal WAIT are never reached (p.30)");
    }

    // L8 - p.20: instructions are consumed as word pairs. If the Copper
    // consumed one word per instruction, the data word 0x0111 below would be
    // read as an instruction and COLOR02 would never be written with 0x1234.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install(LIST_A, {move_ir1(COLOR00), move_ir1(COLOR01),   // data word looks like a MOVE opcode
                           move_ir1(COLOR02), 0x1234,
                           TERMINAL_IR1, WAIT_ALL});
        h.start(1, LIST_A);
        h.run_cycles(8000);
        const bool paired = h.move_count(COLOR00) == 1 && h.data_of(COLOR00) == move_ir1(COLOR01)
                         && h.move_count(COLOR02) == 1 && h.data_of(COLOR02) == 0x1234;
        checks.expect(paired, "L8 instructions are consumed as word pairs, the data word is not decoded (p.20) "
                      "[claims: COP2-what-is-a-copper-instruction.c8, COP2-what-is-a-copper-instruction.c14, "
                      "COP2-copins-instructions.c7, ch2-what-is-a-copper-instruction.c11, ch2-what-is-a-copper-instruction.c6]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
