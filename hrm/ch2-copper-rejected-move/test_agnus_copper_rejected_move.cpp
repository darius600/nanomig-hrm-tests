// Test purpose: Protect NanoMig's observed behaviour after the Copper
//   rejects a MOVE to a forbidden register - it halts, never fetches the
//   instruction's second word, and resumes only on a strobe or start of
//   frame.
// Test class: RTL_REGRESSION (R1 is a secondary HRM_CONFORMANCE check)
// Primary oracle: NanoMig interface contract. The HRM (3rd edition, print
//   pages 21 and 26) states only that the write does not occur; it does not
//   define what the Copper does next. Frozen specification 8ad61bd.
// Requirement IDs: COP-03
// Non-goals: The register-range boundaries themselves (FPGA-056, red as
//   NM-HRM-004). ECS and CDANG=1 rejection variants. Whether halting is
//   correct for real Agnus silicon - this protects observed behaviour, it
//   does not endorse it. Any claim that the single-word fetch violates the
//   HRM.
// Claim ids: ids such as `COP2-control-register.c3` and
//   `COP2-location-registers.c24` name one sentence of the manual in this
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
// Disclosure required by docs/model-collaboration-guide.md: the RTL was
// inspected during FPGA-063 and FPGA-065 before this specification was written.
// For a regression test that is the intended situation, and it is precisely why
// this test must not be presented as HRM-derived.
//
// Source conflict recorded in the specification and NOT resolved here: print
// page 21 and print page 26 give incompatible register ranges for the same
// rule. Register $00E is used below because both readings forbid it
// unconditionally, so the conflict cannot influence the result.
//
// MUTATION R1 stimulus red: rejecting_list()'s first MOVE retargeted from FORBIDDEN to COLOR00 (a legal
//   register), and R1's checked register updated to match -> FAIL as expected (write now reaches the bus).
// MUTATION R3 stimulus red: same retarget of rejecting_list()'s first MOVE to COLOR00 -> FAIL as expected
//   (the now-legal first MOVE causes its second word, at LIST+1, to be fetched).
// MUTATION R4 stimulus red: same retarget of rejecting_list()'s first MOVE to COLOR00 -> FAIL as expected
//   (with no halt, the following COLOR00 MOVE also executes).
// MUTATION R2 stimulus green (unexpected on the R1/R3/R4 retarget): the same retarget left R2 PASSING - the
//   sentinel WAIT (TERMINAL_IR1/WAIT_ALL) that follows still makes the Copper idle within the run window, so a
//   >2000-cycle gap appears whether or not a rejection ever happened. See the dedicated R2 mutation below.
// MUTATION R2 stimulus red: replaced the poked list with 200 back-to-back legal MOVEs to COLOR00 (no rejection
//   at all) -> FAIL as expected (sustained bus activity keeps the last-grant gap under 2000 cycles, proving R2
//   does distinguish "halted" from "still busy").
// MUTATION R5 stimulus red: LIST2's MOVE retargeted from COLOR01 to COLOR00 (R5's check left at COLOR01) ->
//   FAIL as expected (the relocated list's actual destination no longer matches what R5 expects to see).
// MUTATION R6 stimulus red: same LIST2 retarget (COLOR01 -> COLOR00) applied to R6's own block -> FAIL as
//   expected.

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
constexpr uint16_t COPINS  = to_reg(0x08C);
constexpr uint16_t IDLE_REG = 0x0FF;
constexpr uint16_t COLOR00 = to_reg(0x180);
constexpr uint16_t COLOR01 = to_reg(0x182);

// Forbidden under both HRM readings: "lower than $10" (p.21) and "$00 to $3E" (p.26).
constexpr uint16_t FORBIDDEN = to_reg(0x00E);

constexpr uint32_t LIST  = 0x00002000;   // list whose first instruction is rejected
constexpr uint32_t LIST2 = 0x00003000;   // legal list used to prove the halt was released

constexpr uint16_t WAIT_ALL = 0xFFFE;
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;

constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

struct Grant {
    long tick;
    uint32_t address;
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

    void poke(uint32_t address, const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) memory[address + i] = words[i];
    }
    void seek_beam(uint16_t v) { vpos = v; hpos = 0; dut->vpos = vpos; dut->hpos = hpos; }

    void point_at(uint32_t list) {
        write_reg(COP1LCH, static_cast<uint16_t>((list >> 15) & 0x1F));
        write_reg(COP1LCL, static_cast<uint16_t>((list & 0x7FFF) << 1));
    }
    void strobe_copjmp1() { write_reg(COPJMP1, 0x0000); }
    void pulse_sof() {
        dut->sof = 1;
        run_cycles(2);
        dut->sof = 0;
    }
    void start_at(uint32_t list) {
        point_at(list);
        strobe_copjmp1();
        run_cycles(4);
        grants.clear();
        tick = 0;
        dma_allowed = true;
    }
    void run_cycles(int n) { for (int i = 0; i < n; ++i) cycle(); }

    bool any_grant_for_register(uint16_t reg) const {
        for (const auto& g : grants) if (g.reg == reg) return true;
        return false;
    }
    bool any_grant_fetching(uint32_t address) const {
        for (const auto& g : grants) if (g.address == address) return true;
        return false;
    }
    int grants_after(long from_tick) const {
        int n = 0;
        for (const auto& g : grants) if (g.tick >= from_tick) ++n;
        return n;
    }
    long last_grant_tick() const { return grants.empty() ? -1 : grants.back().tick; }
    long now() const { return tick; }

    std::vector<Grant> grants;
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
        const bool accepted = !cpu_write && dma_allowed && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0; dut->reg_address_in = cpu_reg; dut->data_in = cpu_data;
        } else if (accepted) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            const uint16_t data = read_memory(address);
            dut->ackdma = 1; dut->reg_address_in = reg; dut->data_in = data;
            dut->eval();
            if (reg != IDLE_REG) grants.push_back({tick, address, reg, data});
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
    int passed = 0, failed = 0;
};

// A list whose first instruction is a MOVE to a forbidden register, followed by
// a legal MOVE that must never run, then a terminal WAIT.
std::vector<uint16_t> rejecting_list() {
    return {move_ir1(FORBIDDEN), 0x0001,
            move_ir1(COLOR00),   0x0ABC,
            TERMINAL_IR1,        WAIT_ALL};
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // R1 (conformance) / R2 / R3 / R4 - one scenario, four observations.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.poke(LIST, rejecting_list());
        h.start_at(LIST);
        h.run_cycles(3000);

        // R1 - HRM p.21/p.26: the write must not reach the register.
        checks.expect(!h.any_grant_for_register(FORBIDDEN),
                      "R1 [conformance] the forbidden register is never presented on the register bus (p.21, p.26) "
                      "[claims: COP2-control-register.c3]");

        // R3 - the second instruction word is never fetched.
        checks.expect(h.any_grant_fetching(LIST) && !h.any_grant_fetching(LIST + 1),
                      "R3 [regression] the rejected MOVE's second instruction word is never fetched");

        // R2 - the Copper stops requesting the bus altogether.
        const long last = h.last_grant_tick();
        checks.expect(last >= 0 && h.now() - last > 2000,
                      "R2 [regression] after the rejected MOVE the Copper stops requesting the bus");

        // R4 - the halt persists: the following legal MOVE never runs.
        checks.expect(!h.any_grant_for_register(COLOR00),
                      "R4 [regression] the halt persists - the legal MOVE after the rejected one never executes");
    }

    // R5 - a COPJMP1 strobe releases the halt. COP1LC is repointed at a legal
    // list first, so resumption is unambiguous rather than a repeat of the same
    // rejection.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.poke(LIST, rejecting_list());
        h.poke(LIST2, {move_ir1(COLOR01), 0x0DEF,
                       TERMINAL_IR1,      WAIT_ALL});
        h.start_at(LIST);
        h.run_cycles(2000);
        const long halted_at = h.now();

        h.point_at(LIST2);
        h.strobe_copjmp1();
        h.run_cycles(2000);

        checks.expect(h.grants_after(halted_at) > 0 && h.any_grant_for_register(COLOR01),
                      "R5 [regression] a COPJMP1 strobe releases the halt and the Copper runs the relocated list");
    }

    // R6 - a start-of-frame pulse releases the halt the same way.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.poke(LIST, rejecting_list());
        h.poke(LIST2, {move_ir1(COLOR01), 0x0FED,
                       TERMINAL_IR1,      WAIT_ALL});
        h.start_at(LIST);
        h.run_cycles(2000);
        const long halted_at = h.now();

        h.point_at(LIST2);
        h.pulse_sof();
        h.run_cycles(2000);

        checks.expect(h.grants_after(halted_at) > 0 && h.any_grant_for_register(COLOR01),
                      "R6 [regression] a start-of-frame pulse releases the halt in the same way "
                      "[claims: COP2-location-registers.c24]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
