// Test purpose: Measure the width of the Copper location registers
//   (COP1LCH/COP1LCL, COP2LCH/COP2LCL) that NanoMig actually implements, by
//   walking single bits through the register-bus write path and reading
//   which ones survive to address_out after a jump strobe.
// Test class: RTL_REGRESSION
// Primary oracle: none. Print page 25 contradicts itself: the register
//   table (p25:19-22) says "Low 16 bits"; the prose (p25:28-30) says "low
//   15 bits" of the same register and, in the same sentence, states the
//   total address is "18-bit" - only arithmetically consistent with a
//   3-bit high half plus a 15-bit low half (3+15=18; 3+16=19 contradicts
//   the sentence's own stated total). No conformance oracle exists for
//   either width. Frozen specification:
//   fpga-lab/work/2026-09-05_fpga-078_locreg-width_testspecification.md
// Requirement IDs: COP-05 (Using the Copper Registers, p.25). Additive;
//   does not touch the existing red finding on that row (FPGA-060).
// Non-goals: declaring 15, 16, or any other number "correct" for the real
//   Amiga; CDANG/ECS gating of location-register writes; the automatic
//   sof-triggered restart; jump-strobe timing beyond "the write becomes
//   observable at address_out".
// Disclosure: agnus_copper.v was read beyond the port list (the location-
//   register write blocks at lines ~139-170 and the address_out reload mux
//   at ~194-202) because there is no register read-back port: the only way
//   to observe what a location register holds is to strobe a jump and see
//   where the module fetches from next. That reading showed
//   `cop1lcl[15:1] <= data_in[15:1]` (15 bits, data_in bit 0 excluded) and
//   `cop1lch[20:16] <= data_in[4:0]` (5 bits, not the documented 3), same
//   for cop2lch/cop2lcl. The FIRST version of this test's fixture, built
//   from a naive reading of that assignment (assuming Verilog preserves
//   literal bit-index labels across the concatenation into address_out),
//   produced results that did not match either manual reading OR a clean
//   bit-for-bit pattern (input bit i landing at a shifted, seemingly
//   inconsistent position) and was WRONG - not because of an RTL surprise,
//   but because address_out is declared as a [20:1] port: Verilog's own
//   label 1 becomes Verilator's C++ bit 0 of `dut->address_out`, an offset
//   the first fixture did not account for. That bug was found by installing
//   a marker instruction at every candidate output-bit address at once and
//   noticing two input bits alias to the same marker - a result that cannot
//   come from a clean bit-independent mapping, and was the tell. The
//   fixture below installs exactly one marker per trial, at the address a
//   correctly-offset reading of the RTL predicts, and the pass/fail below
//   is the simulation's own answer, not a copy of that prediction: every
//   width and address stated below was independently confirmed by running
//   this fixture, not assumed from the source reading alone.
// Mutation check: see the MUTATION RESULT comment near the bottom of this
//   file.

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
constexpr uint16_t IDLE_REG = 0x0FF;
constexpr uint16_t MARK = to_reg(0x180);  // one dedicated marker register per isolated trial

constexpr uint16_t WAIT_ALL = 0xFFFE;
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;
constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

struct Grant { long tick; uint32_t address; uint16_t reg; uint16_t data; };

// Register-bus loopback harness, identical technique to every existing
// Copper test.
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

    void install(uint32_t base, const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) memory[base + static_cast<uint32_t>(i)] = words[i];
    }
    void write_reg(uint16_t reg, uint16_t data) { cycle(true, reg, data); }
    void run_cycles(int n) { for (int i = 0; i < n; ++i) cycle(); }

    bool find_grant(uint16_t reg, Grant& out) const {
        for (const auto& g : grants) if (g.reg == reg) { out = g; return true; }
        return false;
    }

    std::vector<Grant> grants;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0, vpos = 0;
    long tick = 0;

    uint16_t read_memory(uint32_t a) const {
        const auto it = memory.find(a);
        return it == memory.end() ? 0xFFFF : it->second;
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        const bool accepted = !cpu_write && dut->reqdma;
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

// One fully isolated trial: fresh harness, fresh (empty) memory. Write
// hi_val/lo_val into the given register pair, strobe jmp_reg, and check
// whether a marker installed at expected_addr (a MOVE targeting MARK) gets
// granted - i.e. whether the jump actually landed there. Every trial gets
// its own harness, so there is no cross-trial contamination and no
// candidate-address collision to manage.
bool lands_at(uint16_t hi_reg, uint16_t lo_reg, uint16_t jmp_reg,
              uint16_t hi_val, uint16_t lo_val, uint32_t expected_addr) {
    CopperHarness h;
    h.install(expected_addr, {
        move_ir1(MARK), 0xBEEF,
        TERMINAL_IR1, WAIT_ALL   // local trap: park here once the marker fires
    });
    h.write_reg(hi_reg, hi_val);
    h.write_reg(lo_reg, lo_val);
    h.write_reg(jmp_reg, 0x0000);
    h.run_cycles(80);
    Grant g{};
    return h.find_grant(MARK, g);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ------------------------------------------------------------------
    // LR1 - COP1LCL low-field walk. Claims: COP2-loc.c3/-location-
    // registers.c3 (table: 16 bits) vs COP2-loc.c7/3/-location-registers.c11
    // (prose: 15 bits).
    // ------------------------------------------------------------------
    {
        int survivors = 0;
        bool clean = true;
        // Data-word bit 0 is expected NOT to survive (excluded from
        // cop1lcl's own write slice per the RTL disclosure above); bits 1-15
        // are expected to survive at address 2^(bit-1) once verified against
        // Verilator's zero-based numbering of the [20:1] address_out port
        // (see Disclosure). "Expected" here means this fixture's own
        // measured behaviour, pinned as a baseline - not a manual number.
        const bool bit0_survives = lands_at(COP1LCH, COP1LCL, COPJMP1, 0, 0x0001, 1);
        clean = clean && !bit0_survives;
        for (int i = 1; i <= 15; ++i) {
            const uint32_t expected = 1u << (i - 1);
            const bool ok = lands_at(COP1LCH, COP1LCL, COPJMP1, 0, static_cast<uint16_t>(1u << i), expected);
            if (ok) ++survivors; else clean = false;
        }
        std::cout << "[measured] COP1LCL low-field width = " << survivors << " bits\n";
        checks.expect(clean && survivors == 15,
            "LR1 [RTL_REGRESSION, no oracle] COP1LCL: measured low-field "
            "width is 15 bits (data-word bit 0 dropped, bits 1-15 survive "
            "individually at address 2^(bit-1)) - matches the prose's "
            "\"low 15 bits\", not the table's \"low 16 bits\" (p25:20 vs "
            "p25:28-30, both cited, neither adjudicated)");
    }

    // ------------------------------------------------------------------
    // LR2 - COP2LCL low-field walk, same claims, via COPJMP2. Control on
    // LR1: confirms the two location-register pairs behave symmetrically.
    // ------------------------------------------------------------------
    {
        int survivors = 0;
        bool clean = true;
        const bool bit0_survives = lands_at(COP2LCH, COP2LCL, COPJMP2, 0, 0x0001, 1);
        clean = clean && !bit0_survives;
        for (int i = 1; i <= 15; ++i) {
            const uint32_t expected = 1u << (i - 1);
            const bool ok = lands_at(COP2LCH, COP2LCL, COPJMP2, 0, static_cast<uint16_t>(1u << i), expected);
            if (ok) ++survivors; else clean = false;
        }
        std::cout << "[measured] COP2LCL low-field width = " << survivors << " bits\n";
        checks.expect(clean && survivors == 15,
            "LR2 [RTL_REGRESSION, no oracle] COP2LCL: same measurement "
            "repeated via COPJMP2 - width and bit-0 exclusion match COP1LCL "
            "(COP2-loc.c5/-location-registers.c5 vs COP2-loc.c7/3/"
            "-location-registers.c11)");
    }

    // ------------------------------------------------------------------
    // LR3 - control, same measurement basis. A bit already known (from LR1)
    // to survive must still survive when re-checked in isolation. Defends a
    // red result elsewhere in this file against "the rig never actually
    // detects a landed jump at all."
    // ------------------------------------------------------------------
    {
        const bool ok = lands_at(COP1LCH, COP1LCL, COPJMP1, 0, 0x0002 /* bit 1 */, 1);
        checks.expect(ok,
            "LR3 [control, same measurement basis] a bit already shown to "
            "survive (COP1LCL data-word bit 1, landing at address 1) is "
            "re-confirmed in an independent trial - the marker-detection "
            "rig itself works");
    }

    // ------------------------------------------------------------------
    // LR4 - COP1LCH high-field walk plus total-width statement. Claim:
    // COP2-loc.c8/-location-registers.c12 (prose: 18-bit total address).
    // ------------------------------------------------------------------
    {
        int survivors = 0;
        bool clean = true;
        for (int j = 0; j <= 4; ++j) {
            const uint32_t expected = 1u << (15 + j);
            const bool ok = lands_at(COP1LCH, COP1LCL, COPJMP1, static_cast<uint16_t>(1u << j), 0, expected);
            if (ok) ++survivors; else clean = false;
        }
        // Spot checks: bits beyond the captured 5-bit field (data_in[4:0])
        // must NOT extend the high field further. Checked at the addresses
        // a continuing identity mapping would predict for bits 5 and 15.
        const bool bit5_extends = lands_at(COP1LCH, COP1LCL, COPJMP1, static_cast<uint16_t>(1u << 5), 0, 1u << 20);
        const bool bit15_extends = lands_at(COP1LCH, COP1LCL, COPJMP1, static_cast<uint16_t>(1u << 15), 0, 1u << 30);
        clean = clean && !bit5_extends && !bit15_extends;

        const int low_width = 15;   // pinned by LR1/LR2 above
        const int high_width = survivors;
        const int total_width = low_width + high_width;
        std::cout << "[measured] COP1LCH high-field width = " << survivors << " bits\n";
        std::cout << "[measured] total implemented pointer width = " << total_width << " bits\n";
        checks.expect(clean && survivors == 5 && total_width == 20,
            "LR4 [RTL_REGRESSION, no oracle] COP1LCH: measured high-field "
            "width is 5 bits (data-word bits 0-4 all captured, landing at "
            "address bits 15-19), not the 3 bits either manual reading "
            "states. Combined with the 15-bit low field, NanoMig's total "
            "implemented pointer is 20 bits - matching NEITHER the table's "
            "19 (3+16) NOR the prose's 18 (3+15) (p25:19-22, p25:28-30, "
            "COP2-loc.c8/-location-registers.c12)");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION RESULT: four separate one-line mutations, each reverted
// immediately after observing red (documented here, not re-run on every
// future rebuild):
//   - LR1: `survivors == 15` changed to `survivors == 16` -> LR1 turned
//     FAIL (3 passed, 1 failed), LR2-LR4 unaffected.
//   - LR2: `survivors == 15` changed to `survivors == 14` -> LR2 turned
//     FAIL, together with the independent LR3 mutation below (2 passed,
//     2 failed that run).
//   - LR3: the expected landing address changed from 1 to 2 (a bit known
//     to survive at address 1 was asked to have survived at address 2
//     instead) -> LR3 turned FAIL in the same run as the LR2 mutation.
//   - LR4: `total_width == 20` changed to `total_width == 18` (the prose's
//     own total) -> LR4 turned FAIL (3 passed, 1 failed).
// All four assertions discriminate; none is vacuously true. Reverted to
// the values shown in this file, which reproducibly pass (4 passed, 0
// failed).
