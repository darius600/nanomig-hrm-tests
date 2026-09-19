// appF-cia-control-registers -- ciaa conformance test
//
// Test purpose: grade p.348's "There are two control registers in the 8520,
//   CRA and CRB. CRA is associated with Timer A and CRB is associated with
//   Timer B." against `ciaa` (ciaa.v + cia_int.v/cia_timera.v/cia_timerb.v/
//   cia_timerd.v). SPECIFICATION.md records the claim inventory, the
//   duplicate-grading check (nothing in the accepted suite already grades
//   this proposition) and the per-assertion design; this file carries it
//   out.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 348.
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim outside the 5 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 5 ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM COVERAGE: all 3 bare ids / 5 uids assigned to this package are
//   graded with expect() below -- sol:F-cra.c1/opus:F-cra.c1 (CRA-CRB-
//   DISTINCT), sol:F-cra.c2/opus:F-cra.c2 (CRA-GOVERNS-TIMER-A) and
//   sol:F-crb.c1 (CRB-GOVERNS-TIMER-B). No record() line in this package --
//   nothing here duplicates an already-accepted package (see
//   SPECIFICATION.md's duplicate-grading check).
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag (`MUT ?=` in the Makefile), never a runtime string compare.
//   Every mutation was actually built and run (WSL Verilator, `make
//   MUT=<NAME> test`, full `rm -rf obj_dir` rebuild); see the MUTATION
//   comment after each assertion for what was measured.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_CRA_CRB_ALIAS
#define MUT_CRA_CRB_ALIAS 0
#endif
#ifndef MUT_CRA_VIA_CRB
#define MUT_CRA_VIA_CRB 0
#endif
#ifndef MUT_CRB_VIA_CRA
#define MUT_CRB_VIA_CRA 0
#endif

namespace {

constexpr uint8_t RS_TALO = 0x4;
constexpr uint8_t RS_TAHI = 0x5;
constexpr uint8_t RS_TBLO = 0x6;
constexpr uint8_t RS_TBHI = 0x7;
constexpr uint8_t RS_CRA  = 0xE;
constexpr uint8_t RS_CRB  = 0xF;

std::string hex8(uint8_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%02X", v);
    return std::string(buf);
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Bus-timing protocol for ciaa.v, the same as hrm/ch1-cia-timers's own
// Harness for this fixture: registers are posedge-clk, clk7_en-gated, so a
// write needs one tick to latch; data_out is combinational and needs only
// eval().
class Harness {
public:
    Harness() : dut(new Vciaa) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->clk7n_en = 1;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0;
        dut->portb_in = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint8_t rs, uint8_t data) {
        dut->aen = 1;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        tick();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        tick();
    }

    uint8_t read_reg(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    void load_timer(uint8_t lo_rs, uint8_t hi_rs, uint16_t val) {
        write_reg(lo_rs, static_cast<uint8_t>(val & 0xFF));
        write_reg(hi_rs, static_cast<uint8_t>((val >> 8) & 0xFF));
    }

    uint16_t read_timer16(uint8_t lo_rs, uint8_t hi_rs) {
        const uint8_t lo = read_reg(lo_rs);
        const uint8_t hi = read_reg(hi_rs);
        return static_cast<uint16_t>(lo) | (static_cast<uint16_t>(hi) << 8);
    }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // CRA-CRB-DISTINCT -- also this package's control/liveness case: it
    // does not use the timer-counting mechanism the other two assertions
    // depend on, so a dead fixture shows up here first, independently.
    // =====================================================================
    {
        Harness h;
        constexpr uint8_t CRA_V1 = 0x26;       // bits 1,2,5 -- no start, no force-load, no reserved bit7
        constexpr uint8_t CRB_V1_BASE = 0x4A;  // bits 1,3,6 -- distinct from CRA_V1
        const uint8_t CRB_V1 = MUT_CRA_CRB_ALIAS ? CRA_V1 : CRB_V1_BASE;

        h.write_reg(RS_CRA, CRA_V1);
        const uint8_t cra_read1 = h.read_reg(RS_CRA);
        h.write_reg(RS_CRB, CRB_V1);
        const uint8_t crb_read1 = h.read_reg(RS_CRB);
        const uint8_t cra_read2 = h.read_reg(RS_CRA);  // re-read CRA after the CRB write

        checks.note("CRA-CRB-DISTINCT: wrote CRA=0x" + hex8(CRA_V1) + " -> read 0x" +
                    hex8(cra_read1) + "; wrote CRB=0x" + hex8(CRB_V1) + " -> read 0x" +
                    hex8(crb_read1) + "; re-read CRA after the CRB write -> 0x" +
                    hex8(cra_read2) + ".");
        checks.expect(
            cra_read1 == CRA_V1 && crb_read1 == CRB_V1 &&
            cra_read2 == CRA_V1 && cra_read1 != crb_read1,
            "CRA-CRB-DISTINCT (p.348, sol:F-cra.c1, opus:F-cra.c1): \"There "
            "are two control registers in the 8520, CRA and CRB.\" -- "
            "rs=0xE (CRA) and rs=0xF (CRB) each store and read back their "
            "own distinct written byte, and writing CRB does not disturb "
            "CRA's already-written value.");
    }
    // MUTATION CRA-CRB-DISTINCT stimulus red MUT_CRA_CRB_ALIAS: write the identical byte to CRB that was just written to CRA, instead of a different one -- a genuinely shared/aliased single register could no longer be told apart from two independent ones by this measurement, so the cra_read1 != crb_read1 conjunct goes red.

    // =====================================================================
    // CRA-GOVERNS-TIMER-A
    // =====================================================================
    {
        Harness h;
        // eclk stays 0 (Harness() default) through setup, so writing the
        // start bit itself (write_reg's own settle tick) cannot also count
        // as a counting edge -- only the explicit tick loop below may.
        constexpr uint16_t TA_LOAD = 16;
        constexpr uint16_t TB_LOAD = 240;  // stays well clear of underflow over this run
        constexpr int TICKS = 5;

        h.load_timer(RS_TALO, RS_TAHI, TA_LOAD);
        h.load_timer(RS_TBLO, RS_TBHI, TB_LOAD);
        if (MUT_CRA_VIA_CRB)
            h.write_reg(RS_CRB, 0x01);  // mutated: start bit sent to CRB instead of CRA
        else
            h.write_reg(RS_CRA, 0x01);  // start, continuous mode, no force-load
        h.dut->eclk = 1;
        for (int i = 0; i < TICKS; ++i) h.tick();
        const uint16_t ta_after = h.read_timer16(RS_TALO, RS_TAHI);
        const uint16_t tb_after = h.read_timer16(RS_TBLO, RS_TBHI);

        checks.note("CRA-GOVERNS-TIMER-A: TALO/TAHI loaded " + std::to_string(TA_LOAD) +
                    ", TBLO/TBHI loaded " + std::to_string(TB_LOAD) + "; after writing "
                    "the start bit and " + std::to_string(TICKS) + " clock tick(s), Timer "
                    "A reads " + std::to_string(ta_after) + ", Timer B reads " +
                    std::to_string(tb_after) + ".");
        checks.expect(
            ta_after == static_cast<uint16_t>(TA_LOAD - TICKS) && tb_after == TB_LOAD,
            "CRA-GOVERNS-TIMER-A (p.348, sol:F-cra.c2, opus:F-cra.c2): \"CRA "
            "is associated with Timer A[, and CRB is associated with Timer "
            "B].\" -- writing the start bit at rs=0xE (CRA) counts Timer "
            "A's own latched value down by exactly one per clock tick, and "
            "leaves Timer B's latched value untouched.");
    }
    // MUTATION CRA-GOVERNS-TIMER-A stimulus red MUT_CRA_VIA_CRB: write the start bit to CRB (rs=0xF) instead of CRA (rs=0xE) -- Timer A never starts (stays at its loaded 16) and Timer B starts instead, so the ta_after conjunct goes red.

    // =====================================================================
    // CRB-GOVERNS-TIMER-B
    // =====================================================================
    {
        Harness h;
        // eclk stays 0 through setup, for the same reason as above.
        constexpr uint16_t TA_LOAD = 240;  // stays well clear of underflow over this run
        constexpr uint16_t TB_LOAD = 16;
        constexpr int TICKS = 5;

        h.load_timer(RS_TALO, RS_TAHI, TA_LOAD);
        h.load_timer(RS_TBLO, RS_TBHI, TB_LOAD);
        if (MUT_CRB_VIA_CRA)
            h.write_reg(RS_CRA, 0x01);  // mutated: start bit sent to CRA instead of CRB
        else
            h.write_reg(RS_CRB, 0x01);  // start, continuous mode, no force-load
        h.dut->eclk = 1;
        for (int i = 0; i < TICKS; ++i) h.tick();
        const uint16_t ta_after = h.read_timer16(RS_TALO, RS_TAHI);
        const uint16_t tb_after = h.read_timer16(RS_TBLO, RS_TBHI);

        checks.note("CRB-GOVERNS-TIMER-B: TALO/TAHI loaded " + std::to_string(TA_LOAD) +
                    ", TBLO/TBHI loaded " + std::to_string(TB_LOAD) + "; after writing "
                    "the start bit and " + std::to_string(TICKS) + " clock tick(s), Timer "
                    "A reads " + std::to_string(ta_after) + ", Timer B reads " +
                    std::to_string(tb_after) + ".");
        checks.expect(
            tb_after == static_cast<uint16_t>(TB_LOAD - TICKS) && ta_after == TA_LOAD,
            "CRB-GOVERNS-TIMER-B (p.348, sol:F-crb.c1): \"CRB is associated "
            "with Timer B.\" -- writing the start bit at rs=0xF (CRB) "
            "counts Timer B's own latched value down by exactly one per "
            "clock tick, and leaves Timer A's latched value untouched.");
    }
    // MUTATION CRB-GOVERNS-TIMER-B stimulus red MUT_CRB_VIA_CRA: write the start bit to CRA (rs=0xE) instead of CRB (rs=0xF) -- Timer B never starts and Timer A starts instead, so the tb_after conjunct goes red.

    checks.note("Measured, not graded (no page-348 CRB-bit-7/alarm claim in "
                "this package): ciaa.v:379's .tcr(crb) also feeds "
                "cia_timerd, whose own crb7 register (cia_timerd.v:122-129) "
                "latches data_in[7] on every crb-selected write -- the "
                "same 8-bit CRB write also sets cia_timerd's ALARM-select "
                "bit. Consistent with p.350's own CRB bit-7 (SET ALARM) "
                "documentation for this same register, not a second hidden "
                "register; appF-cia-tod-clock-and-alarm owns that page.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
