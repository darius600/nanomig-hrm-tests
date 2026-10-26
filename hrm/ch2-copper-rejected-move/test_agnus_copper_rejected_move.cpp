// Test purpose: Check what a Copper does around a MOVE to a forbidden
//   register: the write never reaches the register bus (R1), and a
//   jump-address strobe (R5) or the start of the next frame (R6) restarts
//   the Copper from a location register. What the Copper does with the
//   rejected MOVE's second word and afterwards (R2, R3, R4) is printed as
//   MEASURED lines, not graded.
// Test class: mixed. R1, R5 and R6 are HRM_CONFORMANCE (graded, each with
//   its page claim id). R2, R3 and R4 are not graded: no page defines them
//   (OUT OF ORACLE), so they print as MEASURED.
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., print pages 21 and
//   26 (R1: the Copper cannot write the register), 25 and 26 (R5: a strobe
//   reloads the program counter from the location register; R6: at the
//   start of each vertical blank COP1LC restarts the Copper whatever it is
//   doing).
// Requirement IDs: COP-03
// Non-goals: The register-range boundaries themselves (red as
//   NM-HRM-004). ECS and CDANG=1 rejection variants. What the Copper does
//   after it decodes a forbidden destination: whether it fetches the
//   MOVE's second word, whether it halts, and whether the next MOVE runs.
//   The pages say only that the write does not occur (pp.21, 26), and the
//   MEASURED lines report what NanoMig does without endorsing it.
// Claim ids: ids such as `COP2-control-register.c3` and
//   `COP2-location-registers.c24` name one sentence of the manual in this
//   chapter's reading. They carry no reader prefix, and they resolve in the
//   claim files that hrm-testpipeline/acceptance/ch2.json lists under
//   `inventories`.
//   `FPGA-nnn` is a task number in the private workspace where this work was
//   planned; it resolves nowhere public, and docs/legacy-names.md gives the
//   finding keys that replaced the ones that were findings.
//   The `COP-nn` Requirement IDs, COP-01 to COP-09, number the requirements of
//   a frozen specification (docs/legacy-names.md); no claim file carries them.
//
// Disclosure required by docs/model-collaboration-guide.md: the RTL was
// inspected before this specification was written. The expected values of
// R1, R5 and R6 come from the cited pages; the MEASURED lines describe the
// RTL and are not HRM-derived.
//
// Source conflict recorded in the specification and NOT resolved here: print
// page 21 and print page 26 give incompatible register ranges for the same
// rule. Register $00E is used below because both readings forbid it
// unconditionally, so the conflict cannot influence the result.
//
// MUTATION R1 stimulus red: rejecting_list()'s first MOVE retargeted from FORBIDDEN to COLOR00 (a legal
//   register), and R1's checked register updated to match -> FAIL as expected (write now reaches the bus).
// MUTATION R5 stimulus red: LIST2's MOVE retargeted from COLOR01 to COLOR00 (R5's check left at COLOR01) ->
//   FAIL as expected (the relocated list's actual destination no longer matches what R5 expects to see).
//   Built as `make MUT=R5`, which defines MUT_R5=1.
// MUTATION R5 stimulus red: the COPJMP1 strobe withheld (COP1LC still repointed at LIST2) -> FAIL as expected
//   (nothing restarts the Copper from COP1LC, so LIST2's MOVE never runs). Built as `make MUT=R5_NOSTROBE`.
// MUTATION R6 stimulus red: same LIST2 retarget (COLOR01 -> COLOR00) applied to R6's own block -> FAIL as
//   expected. Built as `make MUT=R6`.
// MUTATION R6 stimulus red: the start-of-frame pulse withheld (COP1LC still repointed at LIST2) -> FAIL as
//   expected. Built as `make MUT=R6_NOSOF`.

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

#ifndef MUT_R5
#define MUT_R5 0
#endif
#ifndef MUT_R5_NOSTROBE
#define MUT_R5_NOSTROBE 0
#endif
#ifndef MUT_R6
#define MUT_R6 0
#endif
#ifndef MUT_R6_NOSOF
#define MUT_R6_NOSOF 0
#endif

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
    void note(const std::string& m) { std::cout << "MEASURED: " << m << "\n"; }
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

    // R1 (graded) / R2 / R3 / R4 (MEASURED) - one scenario, four observations.
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

        // R3 - whether the second instruction word is fetched. No page says.
        const bool first_word_fetched = h.any_grant_fetching(LIST);
        const bool second_word_fetched = h.any_grant_fetching(LIST + 1);
        checks.note("R3 the rejected MOVE's first word is fetched: " + std::string(first_word_fetched ? "yes" : "no") +
                    "; its second word is fetched: " + std::string(second_word_fetched ? "yes" : "no") +
                    " (no page defines the fetch after a forbidden destination)");

        // R2 - whether the Copper keeps requesting the bus. No page says.
        const long last = h.last_grant_tick();
        checks.note("R2 ticks from the last Copper bus grant to the end of the run: " +
                    std::to_string(last >= 0 ? h.now() - last : -1L) +
                    " (no page defines whether the Copper halts after a forbidden MOVE)");

        // R4 - whether the next legal MOVE runs. No page says.
        checks.note("R4 the legal MOVE after the rejected one executes: " +
                    std::string(h.any_grant_for_register(COLOR00) ? "yes" : "no") +
                    " (no page defines the instruction that follows a forbidden destination)");
    }

    // R5 - a COPJMP1 strobe restarts the Copper from COP1LC (p.25, p.26). The
    // Copper first runs a list that holds a forbidden MOVE, then COP1LC is
    // repointed at a legal list, so the restart is unambiguous rather than a
    // repeat of the same rejection.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.poke(LIST, rejecting_list());
        uint16_t r5_reg = COLOR01;
#if MUT_R5
        r5_reg = COLOR00;  // MUTATION: LIST2's MOVE retargeted; the check below stays at COLOR01.
#endif
        h.poke(LIST2, {move_ir1(r5_reg), 0x0DEF,
                       TERMINAL_IR1,     WAIT_ALL});
        h.start_at(LIST);
        h.run_cycles(2000);
        const long before_strobe = h.now();

        h.point_at(LIST2);
#if !MUT_R5_NOSTROBE
        h.strobe_copjmp1();  // MUTATION (MUT_R5_NOSTROBE): withheld.
#endif
        h.run_cycles(2000);

        checks.expect(h.grants_after(before_strobe) > 0 && h.any_grant_for_register(COLOR01),
                      "R5 [conformance] a COPJMP1 strobe restarts the Copper from COP1LC: the relocated list's MOVE runs "
                      "(p.25, p.26) [claims: COP2-jump-strobe-address.c6]");
    }

    // R6 - a start-of-frame pulse releases the halt the same way.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.poke(LIST, rejecting_list());
        uint16_t r6_reg = COLOR01;
#if MUT_R6
        r6_reg = COLOR00;  // MUTATION: LIST2's MOVE retargeted; the check below stays at COLOR01.
#endif
        h.poke(LIST2, {move_ir1(r6_reg), 0x0FED,
                       TERMINAL_IR1,     WAIT_ALL});
        h.start_at(LIST);
        h.run_cycles(2000);
        const long before_sof = h.now();

        h.point_at(LIST2);
#if !MUT_R6_NOSOF
        h.pulse_sof();  // MUTATION (MUT_R6_NOSOF): withheld.
#endif
        h.run_cycles(2000);

        checks.expect(h.grants_after(before_sof) > 0 && h.any_grant_for_register(COLOR01),
                      "R6 [conformance] the start of a frame restarts the Copper from COP1LC whatever it was doing "
                      "(p.25) [claims: COP2-location-registers.c24]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
