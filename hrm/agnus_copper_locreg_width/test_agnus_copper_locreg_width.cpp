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
    // The chipset is a parameter since 06.09.2026, defaulting to the original
    // chip set so every existing call is unchanged. Appendix A p.273 and
    // appendix C p.309 state the high half's width per chipset -- "(high 3
    // bits, high 5 bits if ECS)" and "(high 5 bits,was 3 bits)" -- so the width
    // cannot be measured in one mode alone without assuming the answer.
    explicit CopperHarness(bool ecs_mode = false) : dut(new Vagnus_copper) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1; dut->ecs = ecs_mode;
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
    // Prints a record for a claim this file does NOT bind to any assertion
    // here. Never touches passed/failed - this is not an assertion.
    void note(const std::string& l) {
        std::cout << "NOTE: " << l << "\n";
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
              uint16_t hi_val, uint16_t lo_val, uint32_t expected_addr,
              bool ecs_mode = false) {
    CopperHarness h(ecs_mode);
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
            "p25:28-30, both cited, neither adjudicated) "
            "[claims: COP2-cop1lcl.c5, COP2-location-registers.c10, ch2-appa-location-registers.c6, ch2-appa-location-registers.c7, ch2-appa-location-registers.c9, ch2-appb-table.c20, ch2-appb-table.c22, ch2-appb-table.c23, ch2-location-registers.c9]");
    }

    // ------------------------------------------------------------------
    // LR2 - COP2LCL low-field walk, same claims, via COPJMP2.
    //
    // Called a "control on LR1" until 06.09.2026, which it is not. Symmetry
    // cannot validate LR1: a fault shared by both register pairs - and the pairs
    // are written by near-identical RTL, agnus_copper.v:147-153 against
    // :164-170 - would leave both walks agreeing and both wrong. What LR2
    // establishes is that the second pair matches the first, which is a claim
    // the manual makes and worth testing on its own.
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
            "-location-registers.c11) [claims: COP2-cop2lcl.c5, COP2-location-registers.c10, ch2-appa-location-registers.c15, ch2-appa-location-registers.c16, ch2-appa-location-registers.c18, ch2-appb-table.c30, ch2-appb-table.c32, ch2-appb-table.c33]");
    }

    // ------------------------------------------------------------------
    // LR3 - COP2LCH high-field walk. The second pointer's HIGH half.
    //
    // An earlier revision used this slot for a control that re-ran a bit LR1
    // had already exercised. Independent validation on 05.09.2026 found it
    // carried no claim and could not detect any module fault LR1 did not
    // already detect, while the second pointer's high register was never
    // measured at all - a COP2LCH wired differently from COP1LCH would have
    // gone unnoticed. The slot now does that job instead.
    //
    // The sentence that used to follow here said the rig-works control was
    // covered by LR1's bit-0 case, "which fails if the marker detection stops
    // working". **That is backwards**, found by independent review on
    // 06.09.2026: if marker detection died, `bit0_survives` would come out
    // false, which is precisely what LR1 expects. LR1 would then fail only
    // because the seven positive walks found no survivors - so the positive
    // walks are the rig-works evidence, and the bit-0 case is not.
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
            "symmetric. Neither manual reading covers this; both state 3 "
            "[claims: ]");
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
        // Every bit above the field, not a sample of four. The earlier version
        // walked {5, 6, 10, 15} and the comment claimed the bits beyond the
        // field had no effect "at all", leaving seven of them untested -
        // independent review on 06.09.2026 pointed out the gap. Walking all
        // eleven costs nothing and removes the overclaim rather than softening
        // the wording.
        bool beyond_field_inert = true;
        for (int j = 5; j <= 15; ++j) {
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
            "COP2-loc.c8/-location-registers.c12) "
            "[claims: COP2-location-registers.c2, COP2-location-registers.c8]");
    }

    // ------------------------------------------------------------------
    // LW1-LW3 - the high half's width is CHIPSET-DEPENDENT, and the appendix
    // states both values side by side. Added 06.09.2026, FPGA-085, package
    // PKG-APPX-LOCWIDTH. Class: HRM_CONFORMANCE, not regression - LR1-LR4
    // above carry "no oracle" because chapter 2 alone could not settle the
    // width, and that is no longer the situation.
    //
    //   appendix A p.273:  "(high 3 bits, high 5 bits if ECS)"
    //   appendix C p.309:  "COP1LCH  080  W  A  Coprocessor 1st location
    //                       (high 5 bits,was 3 bits)"
    //
    // Appendix C is the stronger of the two because it names the ORIGINAL
    // value explicitly, as "was 3 bits", rather than leaving it to be read out
    // of a parenthesis.
    //
    // LR3/LR4 measure 5 bits with the harness in OCS (`ecs = 0`). The single
    // number is already tracked - FPGA-060, and agnus_copper_control T4 is red
    // on it. What could not be said before is the SHAPE of the defect: the
    // module has no chipset dependence at all here. `cop1lch[20:16] <=
    // data_in[4:0]` (agnus_copper.v:144) and the COP2LCH equivalent
    // (agnus_copper.v:161) latch five bits unconditionally; `ecs` appears in
    // neither assignment.
    //
    // LW2 is a consistency check, NOT a control - see the note at the
    // assertion. An earlier revision of this comment called it the positive
    // control and said it was what made LW1 and LW3 mean something. It is not:
    // it would pass with `ecs` dead, because the module latches five bits
    // whatever that input does. Corrected 06.09.2026 after two independent
    // reviews said so; the first correction changed the assertion text and left
    // this paragraph and the mutation appendix contradicting it, which the
    // second review caught.
    // ------------------------------------------------------------------
    {
        auto high_width = [](uint16_t hi, uint16_t lo, uint16_t jmp, bool ecs_mode) {
            int survivors = 0;
            for (int j = 0; j <= 4; ++j) {
                const uint32_t expected = 1u << (15 + j);
                if (lands_at(hi, lo, jmp,
                             static_cast<uint16_t>(1u << j), 0, expected, ecs_mode)) {
                    ++survivors;
                }
            }
            return survivors;
        };

        const int ocs_width = high_width(COP1LCH, COP1LCL, COPJMP1, false);
        const int ecs_width = high_width(COP1LCH, COP1LCL, COPJMP1, true);
        std::cout << "[measured] COP1LCH high-field width: OCS " << ocs_width
                  << " bits, ECS " << ecs_width << " bits\n";

        // The second pointer, both chipsets. Independent review found that the
        // ECS walk covered only COP1LCH, while LR3 measures COP2LCH in the
        // default OCS mode - so the classified claim that COP2LCH carries five
        // bits under ECS had no test at all. Appendices A and C state the
        // widths for both pairs, not just the first.
        const int ocs_width2 = high_width(COP2LCH, COP2LCL, COPJMP2, false);
        const int ecs_width2 = high_width(COP2LCH, COP2LCL, COPJMP2, true);
        std::cout << "[measured] COP2LCH high-field width: OCS " << ocs_width2
                  << " bits, ECS " << ecs_width2 << " bits\n";

        checks.expect(ocs_width2 == ocs_width && ecs_width2 == ecs_width,
            "LW5 the second location register behaves identically to the first in both "
            "chipsets - appendices A and C state the same widths for both pairs (p273, "
            "p309), so a difference between them would contradict the manual whichever "
            "pair were right "
            "[claims: COP2-cop2lch.c5, COP2-cop2lch.c6, COP2-ecs-location-width.c3, "
            "COP2-ecs-location-width.c4, COP2-cop2lch-ecs.c5, COP2-cop2lch-ecs.c6, "
            "COP2-cop-location-pointers.c7, ch2-appa-location-registers.c12.s2, ch2-appa-location-registers.c13, ch2-appa-location-registers.c14, ch2-appb-table.c27.s2, ch2-appb-table.c28, ch2-appb-table.c29, ch2-appc-ecs.c10, ch2-appc-ecs.c11, ch2-location-registers.c4, COP2-location-registers.c4]");

        checks.note("LW5 bridges COP1LCH's separately-measured widths (LW1, red; LW2, "
            "green) onto COP2LCH by equality, which is how COP2-cop2lch.c5/c6 and the "
            "ECS-appendix COP2-cop2lch-ecs.c5/c6 counterparts are covered above - no "
            "assertion states an absolute expected width for COP2LCH on its own.");

        // LW2 is a CONSISTENCY CHECK, not a control, and calling it a control
        // was wrong. Independent validation on 06.09.2026 pointed out that it
        // would pass just as well with the `ecs` input disconnected or stuck,
        // because the module latches five bits regardless - so it cannot show
        // that the fixture is able to switch chipsets at all.
        //
        // What it does show is that the walk measures a width correctly against
        // a value the manual states, which is worth having but is not the same
        // claim.
        //
        // The evidence that `ecs` genuinely reaches this module comes from a
        // different test on the same RTL: agnus_copper_move_sweep measures a
        // different accepted-address set for ecs=0 and ecs=1 (its ecs=1/cdang=1
        // sweep accepts the whole range where ecs=0 does not). The input is
        // therefore connected and effective somewhere in the module; it simply
        // has no effect on the location-register width, which is LW3's point.
        checks.expect(ecs_width == 5,
            "LW2 [consistency check, HRM_CONFORMANCE] with ecs=1 the COP1LCH high field is "
            "5 bits wide (p309 \"high 5 bits\", p273 \"high 5 bits if ECS\") - the walk "
            "measures a stated width correctly. NOT a control for `ecs` being connected: "
            "it would also pass with that input dead, see agnus_copper_move_sweep for "
            "evidence that it is not "
            "[claims: COP2-cop1lch.c6, COP2-ecs-location-width.c1, COP2-ecs-location-width.c2, COP2-cop1lch-ecs.c5, ch2-appa-location-registers.c5, ch2-appb-table.c19, ch2-appc-ecs.c1, ch2-appc-ecs.c5]");

        checks.expect(ocs_width == 3,
            "LW1 [HRM_CONFORMANCE] with ecs=0 the COP1LCH high field is 3 bits wide "
            "(p309 \"was 3 bits\", p273 \"high 3 bits\") "
            "[claims: COP2-cop1lch.c5, COP2-cop1lch-ecs.c6, COP2-location-registers.c2, ch2-appa-location-registers.c4, ch2-appb-table.c18, ch2-appc-ecs.c6, ch2-location-registers.c2, ch2-location-registers.c7]");

        checks.expect(ocs_width != ecs_width,
            "LW3 [HRM_CONFORMANCE] the high-field width DEPENDS on the chipset - the "
            "appendix states two different values for the two chip sets, so a module "
            "giving the same width in both contradicts it whatever that width is "
            "(p273, p309) [claims: ch2-appa-location-registers.c3.s2, ch2-appb-table.c17.s2]");

        // LW4 - the total, stated outright rather than left to arithmetic.
        //
        // Appendix A p.276: "These are 18-bit pointer registers whose contents
        // are used to modify the program counter for initialization or jumps."
        //
        // LR4 above computes the same total and asserts 20 as a REGRESSION,
        // because chapter 2 could not settle it: page 25 gives both "low 16
        // bits" (which would make 19) and "low 15 bits" (18), and the project
        // recorded that as an unresolved source conflict. The appendix ends the
        // conflict by stating the total directly, in a sentence that quotes no
        // half-widths at all and so cannot be read two ways. This is therefore
        // a conformance claim, and it is the number a programmer actually uses.
        const int total_ocs = measured_low_width + ocs_width;
        checks.expect(total_ocs == 18,
            "LW4 [HRM_CONFORMANCE] the location registers are 18-bit pointers "
            "(p276 \"These are 18-bit pointer registers\") - measured " +
            std::to_string(total_ocs) + " = " + std::to_string(measured_low_width) +
            " low + " + std::to_string(ocs_width) + " high "
            "[claims: COP2-cop-location-pointers.c4, COP2-cop-location-pointers.c7, ch2-appa-copins.c140]");
    }

    // ---------------- RM1-RM8: the register addresses, asserted ----------------
    //
    // Added 06.09.2026, FPGA-088, package PKG-APPX-REGMAP. Appendix A page 273
    // gives each location register an address: COP1LCH $080, COP1LCL $082,
    // COP2LCH $084, COP2LCL $086.
    //
    // Every Copper test in this suite already writes to those addresses through
    // constants like `to_reg(0x080)` and would fail if the module decoded them
    // differently - so the map is exercised, and it is not asserted.
    //
    // WHAT THESE SIX DO AND DO NOT SHOW, stated after independent review
    // objected to an earlier, broader claim. They show that the four addresses
    // are not interchangeable in the two specific ways tested: swapping a
    // pair's high and low halves does not work (RM2, RM4), and the first pair's
    // registers are not aliases of the second pair's (RM5, RM6).
    //
    // They do NOT show that the decoder accepts "only" these four addresses.
    // That would need a sweep over the address space, which the move sweep does
    // for MOVE destinations but nothing does for writes into the Copper's own
    // registers.
    //
    // The earlier comment also claimed the swap would catch a decoder that
    // ignored the low address bit. **It would not.** Under such a decoder both
    // writes land in the same register and the second one - the zero - simply
    // overwrites the first, so the case fails for the ordinary reason and tells
    // you nothing about aliasing.
    {
        // Bit 17 sits in the high half, so the target is only reachable when
        // the high register really is the one that was written.
        const uint32_t target = 1u << 17;

        auto lands_with = [&](uint16_t hi_reg, uint16_t lo_reg, uint16_t jmp) {
            return lands_at(hi_reg, lo_reg, jmp, 0x0004, 0x0000, target);
        };

        checks.expect(lands_with(COP1LCH, COP1LCL, COPJMP1),
            "RM1 COP1LCH at $080 with COP1LCL at $082 places bit 17 of the pointer "
            "(appendix A p273 register rows) "
            "[claims: COP2-cop1lch.c1, COP2-cop1lch.c2, COP2-cop1lch.c4, "
            "COP2-cop1lcl.c1, COP2-cop1lcl.c2, COP2-cop1lcl.c4, COP2-cop-locations.c1, "
            "COP2-location-registers.c7, ch2-appa-location-registers.c1, ch2-appa-location-registers.c2, ch2-appb-legend.c3, ch2-appb-table.c14, ch2-appb-table.c15, ch2-appb-table.c16, ch2-appb-table.c21, ch2-location-registers.c12, ch2-location-registers.c6]");
        checks.expect(!lands_with(COP1LCL, COP1LCH, COPJMP1),
            "RM2 the same two addresses swapped does NOT - $080 and $082 are not "
            "interchangeable (p273). Says nothing about addresses outside this pair "
            "[claims: COP2-location-registers.c7, COP2-location-registers.c10, ch2-location-registers.c8]");

        checks.expect(lands_with(COP2LCH, COP2LCL, COPJMP2),
            "RM3 COP2LCH at $084 with COP2LCL at $086 places bit 17 through the second "
            "pointer (appendix A p273) "
            "[claims: COP2-cop2lch.c1, COP2-cop2lch.c2, COP2-cop2lch.c4, "
            "COP2-cop2lcl.c1, COP2-cop2lcl.c2, COP2-cop2lcl.c4, COP2-cop-locations.c1, "
            "COP2-location-registers.c7, ch2-appa-location-registers.c10, ch2-appa-location-registers.c11, ch2-appb-table.c24, ch2-appb-table.c25, ch2-appb-table.c26, ch2-appb-table.c31]");
        checks.expect(!lands_with(COP2LCL, COP2LCH, COPJMP2),
            "RM4 the second pair swapped does NOT (p273)");

        // The two pairs must not be the same register under two names.
        checks.expect(!lands_at(COP1LCH, COP1LCL, COPJMP2, 0x0004, 0x0000, target),
            "RM5 writing the FIRST pair and strobing COPJMP2 does not reach the target - "
            "$080/$082 and $084/$086 are distinct registers, not aliases (p273, p276)");
        checks.expect(!lands_at(COP2LCH, COP2LCL, COPJMP1, 0x0004, 0x0000, target),
            "RM6 and the converse (p273, p276)");
    }

    // ------------------------------------------------------------------
    // Appendix claims considered for this package and left UNBOUND, with
    // reasons. None of these change any assertion above; they are a printed
    // record only.
    // ------------------------------------------------------------------
    checks.note("UNBOUND (address/access-type exercised as stimulus only, not "
        "independently asserted - unlike RM1/RM3 above, no bit-arithmetic walk "
        "isolates the ECS-table row's own address for COP1LCH/COP2LCH): "
        "COP2-cop1lch-ecs.c1, COP2-cop1lch-ecs.c2, COP2-cop1lch-ecs.c4, "
        "COP2-cop2lch-ecs.c1, COP2-cop2lch-ecs.c2, COP2-cop2lch-ecs.c4, ch2-appc-ecs.c2, ch2-appc-ecs.c3, ch2-appc-ecs.c7, ch2-appc-ecs.c8");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// Machine-readable records for tools/check_gates.py --tests. One line per
// mutation actually performed; assertions with no line here have never been
// mutation-tested, and the gate says so rather than assuming they discriminate.
// MUTATION LR1 oracle red survivors == 15 changed to == 16
// MUTATION LR2 oracle red survivors == 15 changed to == 14
// MUTATION LR4 oracle red total_width == 20 changed to == 18
// MUTATION LR4 stimulus red inert-bit sweep {5,6,10,15} changed to {4,6,10,15}; bit 4 is inside the captured field and moved the landing address
//
// Added 05.09.2026 - LR1 and LR2 had only oracle mutations, and LR3 none at
// all, so all three were mutated at the stimulus:
// MUTATION LR1 stimulus red the low-field walk strobed through COPJMP2 while still writing COP1LC, so the pointer under measurement is never the one the jump reads
// MUTATION LR2 stimulus red the same swap in the other direction: COP2LC written, COPJMP1 strobed, so the second pointer is never the one read
// MUTATION LR3 stimulus red the walked bit moved from the high half to the low half, so the high register the assertion measures is never written
//
// Added 06.09.2026 with LW1-LW3 (FPGA-085, PKG-APPX-LOCWIDTH):
// MUTATION LW2 stimulus red the walk writes COP1LCH but strobes COPJMP2, so the pointer under measurement is never the one the jump reads and no bit lands anywhere
//
// LW1, LW3 and LW4 ARE RED AT BASELINE, so none carries a red-going mutation -
// there is nothing green to break, which is trivially true and not evidence of
// anything.
//
// Two things an earlier revision got wrong here, both caught by independent
// review on 06.09.2026:
//
//   - "no stimulus makes them green" is FALSE as an absolute. An
//     under-stimulated walk covering only bits 0-2 yields ocs_width == 3 and
//     turns LW1 and LW4 green; walking one mode differently from the other
//     turns LW3 green. Those are broken measurements rather than compliant
//     behaviour, and the true statement is narrower: no LEGITIMATE stimulus
//     makes them green, because both high registers latch five bits
//     unconditionally (agnus_copper.v:144 and :161).
//
//   - their credibility does NOT rest on LW2. LW2 would pass with `ecs` dead
//     and therefore vouches for nothing about chipset switching. What it does
//     show is that the walk can measure a stated width at all. The evidence
//     that `ecs` reaches the module is external: agnus_copper_move_sweep
//     requires ECS+CDANG=1 to accept addresses that OCS+CDANG=1 refuses, which
//     a dead input would fail.
//
// LW4 IS NOT A SEPARATE DEFECT and should not be counted as one. 15 + 5 = 20
// where the appendix says 18; given LW1, LW4 follows arithmetically. It is
// asserted anyway because p276 states the total outright, in a sentence that
// quotes no half-widths and so cannot be read two ways, and because 18 is the
// number a programmer works with. One fault, two places it shows.
//
// LW3 additionally cannot be turned green by any stimulus at all: the module
// has no chipset dependence in this register to exercise
// (`cop1lch[20:16] <= data_in[4:0]`, agnus_copper.v:144, with no `ecs` in the
// assignment). Same limitation class as AC4 in agnus_copper_cycle_times - the
// forbidden behaviour cannot be induced from outside, only by changing the RTL.
//
// Added 06.09.2026 with RM1-RM6 (FPGA-088, PKG-APPX-REGMAP):
// MUTATION RM1 stimulus red the target moved to bit 21, one place beyond the 20-bit address_out, so no correctly decoded write can reach it
// MUTATION RM3 stimulus red the same move
// MUTATION RM2 none impossible negative assertion - turning it red means producing the address aliasing it forbids, which no external stimulus can do
// MUTATION RM4 none impossible negative assertion, same reason as RM2
// MUTATION RM5 none impossible negative assertion, same reason as RM2
// MUTATION RM6 none impossible negative assertion, same reason as RM2
// MUTATION LW1 none impossible red at baseline and no LEGITIMATE stimulus makes it green - both high registers latch five bits unconditionally (agnus_copper.v:144, :161). An under-stimulated walk covering only bits 0-2 would turn it green, but that is a broken measurement, not compliant behaviour
// MUTATION LW3 none impossible red at baseline; the module has no chipset dependence in this register to exercise
// MUTATION LW4 none impossible red at baseline; follows arithmetically from LW1 and is not a separate defect
// MUTATION LW5 none impossible green at baseline and asserts equality between the two register pairs; no stimulus makes them differ, because the two are written by near-identical RTL (agnus_copper.v:139-153 against :156-170)
//
// RM2, RM4, RM5 and RM6 are NEGATIVE assertions - they require that a wrongly
// addressed write does NOT land. No stimulus can turn them red, because that
// would mean producing the very decoder fault they forbid; only an RTL change
// could. They are worth having anyway: nothing else in the suite shows the
// module decodes ONLY the stated address, and a decoder that also accepted a
// neighbour would pass every other test here. Third instance of this limitation
// today, after AC4 and LW3.
//
// LR1's and LR3's mutations also took LR4 down with them, which is expected
// and worth stating: LR4 consumes LR1's measured width, so breaking the
// measurement breaks what depends on it. That is a dependency between the
// assertions, not a second defect.
//
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
