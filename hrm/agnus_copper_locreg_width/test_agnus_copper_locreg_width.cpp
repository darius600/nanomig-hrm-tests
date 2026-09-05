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
    int measured_low_width = -1;   // set by LR1, consumed by LR5
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
        measured_low_width = survivors;
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
    // LR3 - COP2LCH high-field walk. The second pointer's HIGH half.
    //
    // An earlier revision used this slot for a control that re-ran a bit LR1
    // had already exercised. Independent validation on 05.09.2026 found it
    // carried no claim and could not detect any module fault LR1 did not
    // already detect, while the second pointer's high register was never
    // measured at all - a COP2LCH wired differently from COP1LCH would have
    // gone unnoticed. The slot now does that job instead. The rig-works
    // control it used to provide is covered by LR1's own bit-0 case, which
    // fails if the marker detection stops working.
    // ------------------------------------------------------------------
    int measured_cop2_high = -1;
    {
        int survivors = 0;
        bool clean = true;
        for (int j = 0; j <= 4; ++j) {
            const uint32_t expected = 1u << (15 + j);
            const bool ok = lands_at(COP2LCH, COP2LCL, COPJMP2, static_cast<uint16_t>(1u << j), 0, expected);
            if (ok) ++survivors; else clean = false;
        }
        std::cout << "[measured] COP2LCH high-field width = " << survivors << " bits\n";
        measured_cop2_high = survivors;
        checks.expect(clean && survivors == 5,
            "LR3 [RTL_REGRESSION, no oracle] COP2LCH: measured high-field "
            "width is 5 bits, the same as COP1LCH - the two pointers are "
            "symmetric. Neither manual reading covers this; both state 3");
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
        // Bits beyond the captured 5-bit field must have NO effect at all.
        //
        // An earlier revision checked this by asking whether bit 5 landed at
        // address 1<<20 and bit 15 at 1<<30. Neither can be expressed through
        // a [20:1] port, so both probes passed of necessity and would have
        // missed a bit that was rewired to somewhere reachable. Found in
        // independent validation, 05.09.2026. The check now compares the
        // landing address against the same trial with the bit cleared: if a
        // high bit beyond the field influences the pointer at all, in any
        // direction, the two differ.
        // Low data-word bit 1 alone puts the list head at address 1. Setting a
        // high data-word bit beyond the captured field must leave it there. If
        // such a bit were wired in anywhere reachable, the jump would land
        // somewhere else and the marker at address 1 would not fire - so this
        // probe can actually fail, which the address-1<<30 form could not.
        bool beyond_field_inert = true;
        for (int j : {5, 6, 10, 15}) {
            const bool still_lands =
                lands_at(COP1LCH, COP1LCL, COPJMP1, static_cast<uint16_t>(1u << j), 0x0002, 1);
            if (!still_lands) {
                beyond_field_inert = false;
                std::cout << "[measured] H data-word bit " << j
                          << " moved the landing point away from address 1\n";
            }
        }
        clean = clean && beyond_field_inert;

        // Measured, not pinned: LR1 reports the low width, this block the
        // high width. An earlier revision hardcoded low_width = 15, which made
        // total_width == 20 follow algebraically from survivors == 5 and
        // measured nothing.
        const int high_width = survivors;
        const int total_width = measured_low_width + high_width;
        std::cout << "[measured] COP1LCH high-field width = " << survivors << " bits\n";
        std::cout << "[measured] total implemented pointer width = " << total_width
                  << " bits (" << measured_low_width << " low + " << high_width << " high)\n";
        checks.expect(clean && survivors == 5 && measured_cop2_high == survivors && total_width == 20,
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

// MUTATION RESULTS, each reverted immediately after observing red.
//
// A distinction independent validation insisted on, 05.09.2026, and it is
// the right one: an **oracle mutation** changes the expected value and shows
// only that the boolean can become false. A **stimulus mutation** changes
// what the module is asked to do and shows that the assertion notices a
// different behaviour. The second is the one that means something.
//
// Oracle mutations (three, from the first revision of this file):
//   - LR1: `survivors == 15` -> `== 16`. FAIL, LR2-LR4 unaffected.
//   - LR2: `survivors == 15` -> `== 14`. FAIL.
//   - LR4: `total_width == 20` -> `== 18` (the prose's own total). FAIL.
//
// Stimulus mutation (the one that carries weight):
//   - LR4, inert-bit probe: the swept set `{5, 6, 10, 15}` - all beyond the
//     captured high field - changed to `{4, 6, 10, 15}`, where bit 4 IS in
//     the field. The landing point moved away from address 1, the probe
//     reported "H data-word bit 4 moved the landing point away from address
//     1", and LR4 turned FAIL (3 passed, 1 failed). The probe therefore
//     detects a high bit that reaches the pointer, which its predecessor
//     could not: that version asked whether bit 5 landed at 1<<20 and bit 15
//     at 1<<30, neither of which a [20:1] port can express, so both passed
//     of necessity.
//
// Not separately mutated: LR3, which repeats LR4's measurement on the second
// register pair. Its earlier occupant was a control that re-ran a bit LR1
// had already exercised and carried no claim of its own.
