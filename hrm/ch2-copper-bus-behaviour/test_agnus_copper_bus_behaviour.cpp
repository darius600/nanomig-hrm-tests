// Test purpose: Assert that a blocked WAIT releases the bus and that
//   withholding the Copper's DMA slot neither loses its place nor skips or
//   duplicates an instruction.
// Test class: mixed - W1/W2 HRM_CONFORMANCE, W3/W4/W5 RTL_REGRESSION
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, print
//   pages 19 and 22 for W1/W2; NanoMig interface contract for W3-W5, because
//   print page 30 never states where the Copper resumes after DMA returns.
//   Frozen specification 3225eab.
// Requirement IDs: COP-04, COP-07, COP-08
// Non-goals: Which channel wins a contested slot (decided outside this
//   module). COPEN and real DMACON gating (FPGA-055). The exact blanking
//   edge - sof is an input. Cycle-time figures (FPGA-067). Any claim that
//   the observed resume position is HRM-correct; the manual does not
//   describe it.
// Claim ids: ids such as `COP2-about-the-copper.c13` and
//   `COP2-about-the-copper.c14` name one sentence of the manual in this
//   chapter's reading. They carry no reader prefix, and they resolve in the
//   claim files that hrm-testpipeline/acceptance/ch2.json lists under
//   `inventories`.
//   `FPGA-nnn` is a task number in the private workspace where this work was
//   planned; it resolves nowhere public, and docs/legacy-names.md gives the
//   finding keys that replaced the ones that were findings.
//   The `COP-nn` Requirement IDs, COP-01 to COP-09, number the requirements of
//   the frozen specification the suite's first version was written against
//   (docs/legacy-names.md); no claim file carries them.
//
// Boundary limit, stated so no reader over-reads the result: at this module,
// "Copper DMA disabled via COPEN" and "the slot was taken by a higher-priority
// channel" are indistinguishable - both arrive as the absence of a grant, and
// there is no DMACON input on the port list. W3-W5 cover DMA unavailability in
// general, not COPEN.
//
// Machine-readable mutation records for tools/check_gates.py --tests. Every
// assertion is mutated at the STIMULUS, and each mutation is reverted
// immediately after observing red.
// MUTATION W1 stimulus red the WAIT target moved from line $20 to line $00, which the beam has already passed, so the WAIT never blocks and there is no off-the-bus stretch to observe
// MUTATION W2 stimulus red the WAIT target moved to line $90, unreachable inside the cycle budget, so the Copper never wakes and never takes a memory cycle again
// MUTATION W3 stimulus red a self-issued COPJMP1 strobe added to the list, so the Copper legitimately jumps back and its word offsets stop increasing
// MUTATION W3a stimulus red the slot never actually withheld - the same 400 cycles run with DMA allowed - so cycles are granted inside the window the assertion says must stay empty
// MUTATION W4 stimulus red the second MOVE retargeted to COLOR00, so one register is written twice and another not at all
// MUTATION W5 stimulus red the withholding window shortened to zero cycles, so there is no stretch in which the Copper could be seen still asking
//
// W1 FAILED its first mutation, and the assertion was strengthened rather than
// the mutation abandoned. It counted requests *between* the WAIT's second word
// and the next fetch, which is vacuously zero when those two ticks are
// adjacent - so a WAIT that never blocked at all passed it just as a properly
// blocked one did. It could not tell "waited quietly" from "never waited". The
// check now also requires the gap to exceed one scan line; see the comment at
// the assertion. This is the second assertion in this suite found to hold for
// the wrong reason (after SM4's), and both were found the same way.
//
// Disclosure required by docs/model-collaboration-guide.md: the RTL was
// inspected during FPGA-063 and FPGA-065 before this specification was written.

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
constexpr uint16_t IDLE_REG = 0x0FF;
constexpr uint16_t COLOR00 = to_reg(0x180);
constexpr uint16_t COLOR01 = to_reg(0x182);
constexpr uint16_t COLOR02 = to_reg(0x184);

constexpr uint32_t LIST = 0x00002000;

constexpr uint16_t WAIT_ALL = 0xFFFE;
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;

constexpr uint16_t ir1_pos(uint8_t vp, uint8_t hp) {
    return static_cast<uint16_t>((vp << 8) | ((hp & 0x7F) << 1) | 1);
}
constexpr uint16_t ir2_mask(bool bfd, uint8_t ve, uint8_t he, bool skip) {
    return static_cast<uint16_t>((bfd ? 0x8000 : 0) | ((ve & 0x7F) << 8) | ((he & 0x7F) << 1) | (skip ? 1 : 0));
}
constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

struct Grant {
    long tick;
    uint32_t word_offset;
    uint16_t reg;
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
        grants.clear();
        req_ticks.clear();
        tick = 0;
        dma_allowed = true;
    }
    void run_cycles(int n) { for (int i = 0; i < n; ++i) cycle(); }

    // Withhold the slot: the Copper may request, but nothing is granted.
    void withhold_slot(int n) {
        dut->enadma = 0;
        dma_allowed = false;
        run_cycles(n);
        dma_allowed = true;
        dut->enadma = 1;
    }

    long first_tick_at(uint32_t word_offset) const {
        for (const auto& g : grants) if (g.word_offset == word_offset) return g.tick;
        return -1;
    }
    int count_grants_for_register(uint16_t reg) const {
        int n = 0;
        for (const auto& g : grants) if (g.reg == reg) ++n;
        return n;
    }
    int requests_between(long from, long to) const {
        int n = 0;
        for (const long t : req_ticks) if (t > from && t < to) ++n;
        return n;
    }
    int requests_in_window(long from, long to) const { return requests_between(from - 1, to + 1); }
    long now() const { return tick; }

    std::vector<Grant> grants;
    std::vector<long> req_ticks;      // every tick on which reqdma was asserted
    bool dma_allowed = false;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0, vpos = 0;
    long tick = 0;

    uint16_t read_memory(uint32_t a) const {
        const auto it = memory.find(a);
        return it == memory.end() ? 0xFFFF : it->second;
    }
    void write_reg(uint16_t r, uint16_t d) { cycle(true, r, d); }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        if (dut->reqdma) req_ticks.push_back(tick);
        const bool accepted = !cpu_write && dma_allowed && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0; dut->reg_address_in = cpu_reg; dut->data_in = cpu_data;
        } else if (accepted) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            const uint16_t data = read_memory(address);
            dut->ackdma = 1; dut->reg_address_in = reg; dut->data_in = data;
            dut->eval();
            if (reg != IDLE_REG) grants.push_back({tick, address - LIST, reg, data});
        } else {
            dut->ackdma = 0; dut->reg_address_in = IDLE_REG; dut->data_in = 0;
        }
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        ++tick;
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
    // Prints a record for a claim this file does NOT bind to any assertion
    // here. Never touches passed/failed - this is not an assertion.
    void note(const std::string& l) { std::cout << "NOTE: " << l << "\n"; }
    int passed = 0, failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // W1/W2 - p.22: "While waiting, the Copper is off the bus and not using
    // memory cycles." p.19: once satisfied it takes memory cycles again.
    {
        CopperHarness h;
        h.seek_beam(0x10);
        h.install({ir1_pos(0x20, 0x00), ir2_mask(true, 0x7F, 0x00, false),   // blocks, then wakes
                   move_ir1(COLOR00), 0x0111,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(20000);

        const long second_word = h.first_tick_at(1);      // WAIT's own second word
        const long wake_fetch  = h.first_tick_at(2);      // next instruction after the WAIT
        // Between the WAIT being fully fetched and it waking, the Copper must
        // not ask for the bus at all.
        //
        // The gap must also be a real one. Counting requests *between* two
        // adjacent ticks yields zero whether the Copper waited quietly or never
        // waited at all, and the assertion cannot tell those apart: a
        // stimulus mutation that moves the WAIT to an already-passed line
        // leaves this check green. The beam starts at line
        // $10 and the WAIT is for line $20, so a genuine block spans sixteen
        // lines; requiring more than one line's worth of cycles admits the real
        // wait and rejects the degenerate one, without pinning the exact
        // duration this test does not claim to measure.
        const long one_line = 454;
        const long blocked_for = wake_fetch - second_word;
        checks.expect(second_word >= 0 && wake_fetch > second_word &&
                          blocked_for > one_line &&
                          h.requests_between(second_word, wake_fetch) == 0,
                      "W1 [conformance] a blocked WAIT never asserts reqdma - off the bus, no memory cycles (p.22, p.19) "
                      "[claims: COP2-about-the-copper.c13, COP2-about-the-copper.c14, COP2-the-wait-instruction.c3, COP2-the-wait-instruction.c4, "
                      "COP2-copins-kinds.c3, COP2-copins-kinds.c4, COP2-copins-kinds.c5, COP2-copins-kinds.c6, "
                      "COP2-copper-loops-and-branches-and-comparison-enable.c8, ch2-about-the-copper.c14, ch2-about-the-copper.c15, ch2-about-the-copper.c16, ch2-appa-copins.c10, ch2-the-wait-instruction.c2, ch2-the-wait-instruction.c3]");

        checks.expect(wake_fetch > 0 && h.count_grants_for_register(COLOR00) == 1,
                      "W2 [conformance] once the WAIT is satisfied the Copper takes memory cycles again (p.19) "
                      "[claims: COP2-copins-kinds.c3, COP2-copins-kinds.c4, COP2-copins-kinds.c5, ch2-about-the-copper.c17]");
    }

    // W3/W4/W5 - the slot is withheld in the middle of a MOVE list.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install({move_ir1(COLOR00), 0x0222,
                   move_ir1(COLOR01), 0x0333,
                   move_ir1(COLOR02), 0x0444,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(10);                      // let the first MOVE get under way

        const size_t grants_before = h.grants.size();
        const long withhold_from = h.now();
        h.withhold_slot(400);
        const long withhold_to = h.now();

        // W5 - while the slot is withheld the Copper keeps asking for it. This
        // is the opposite of W1 and the contrast is the point: waiting for the
        // beam releases the bus, waiting for a slot does not.
        checks.expect(h.requests_in_window(withhold_from, withhold_to) > 0,
                      "W5 [regression] while the slot is withheld the Copper keeps asserting reqdma");

        // Nothing may be granted while the slot is withheld.
        checks.expect(h.grants.size() == grants_before,
                      "W3a [regression] no cycle is granted while the slot is withheld");

        h.run_cycles(600);

        // W3 - the Copper resumed at the word it was on, so the list is intact
        // and in order.
        bool in_order = true;
        uint32_t previous = 0;
        bool first = true;
        for (const auto& g : h.grants) {
            if (!first && g.word_offset < previous) in_order = false;
            previous = g.word_offset;
            first = false;
        }
        checks.expect(in_order,
                      "W3 [regression] after the slot returns the Copper resumes in order, without losing its place");

        // W4 - each MOVE executed exactly once: nothing skipped, nothing repeated.
        checks.expect(h.count_grants_for_register(COLOR00) == 1 &&
                          h.count_grants_for_register(COLOR01) == 1 &&
                          h.count_grants_for_register(COLOR02) == 1,
                      "W4 [regression] withholding the slot neither skips nor duplicates an instruction");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
