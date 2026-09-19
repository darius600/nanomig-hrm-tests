// Test purpose: assert printed p.12's claim that "the 8520 I/O chips
//   provide timers for developers who are bypassing the operating system"
//   -- at this fixture's boundary, that means `ciab` (the second of the
//   two 8520s on this board) must, like `ciaa` (test_ciaa.cpp), own a
//   timer a developer can load with a count, start, and see reach a
//   detectable completion after real clock ticks -- at the `ciab` module
//   boundary, compiled alone with its CIA submodules. The manual's plural
//   "the 8520 I/O chips" names both physical chips, so this package grades
//   the identical proposition a second time here rather than treating
//   test_ciaa.cpp's PASS as standing in for both.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 12; committed specification 069192a (SPECIFICATION.md, this
//   package).
// Requirement IDs: PROP-TIMER, per SPECIFICATION.md's claim inventory.
//   Claims file: hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl,
//   filtered to classify.package == "ch1-cia-timers" (4 claims; 2 carried
//   on PROP-TIMER here and in test_ciaa.cpp -- same proposition, graded
//   once per physical chip -- 2 declined, on test_ciaa.cpp's RECORDED
//   line, not repeated here to avoid citing the same uids on two graded
//   files' RECORDED lines with different supporting text).
// Non-goals: anything outside the `ciab` fixture this file builds -- this
//   package Verilates two independent tops from two sources, and this is
//   one of them -- and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision, not that the
//   module is correct and not that the chapter is covered.
//
// Order-of-work disclosure: identical situation to test_ciaa.cpp, see that
//   file's header and SPECIFICATION.md's own disclosure section. Nothing
//   further to add here.
//
// RTL read for step 2, after SPECIFICATION.md was committed (069192a):
//   ciab.v (251 lines, pinned submodule commit
//   d6f7df957fad655bc07f6a381e5094a9edff0be0) in full; cia_timera.v and
//   cia_int.v already read in full for test_ciaa.cpp (identical submodule
//   instances, same file, not re-read a second time for content);
//   cia_timerb.v/cia_timerd.v not opened for content, only present so
//   Verilator can elaborate `ciab`. Also read, per the task instructions
//   naming it as a control-pattern sibling for a `ciab` fixture
//   specifically: hrm/ch8-ciabprb-disk-control-bits's Makefile and test
//   file (Checks class, Harness bus-timing protocol for `ciab`, and the
//   MUTATION-comment convention -- reused below as verified RTL-interface
//   plumbing, not as a source of this package's own assertions).
//
// RTL facts established by that reading (none of this shaped
//   SPECIFICATION.md, which was committed first):
//   - ciab.v:195 (`cia_timera tmra`) and :215 (`cia_timerb tmrb`) instance
//     the identical `cia_timera.v`/`cia_timerb.v` modules `ciaa` uses --
//     same 16-bit down-counter, same TALO/TAHI/CRA register-select values
//     (ciab.v:50-64: pra=0x0, prb=0x1, ddra=0x2, ddrb=0x3, talo=0x4,
//     tahi=0x5, icrs=0xD, cra=0xE), same `count = eclk` level-enable
//     mechanism (cia_timera.v:32, not re-read a second time for content).
//   - ciab.v:175-190 (`cia_int cnt`): identical instantiation shape to
//     ciaa.v's, `.ta(ta)` from Timer A's `irq` (underflow), `.irq(irq)`
//     driving ciab's own top-level `irq` output port (:18) -- the same
//     unmask-via-ICR-then-observe-irq mechanism test_ciaa.cpp uses applies
//     here unchanged. One difference from ciaa's instance: `.flag(flag)`
//     is wired to ciab's own top-level `flag` input (ciaa hardwires
//     `.flag(1'b0)` instead) -- irrelevant to PROP-TIMER, `flag` is held
//     at its default (0) throughout this fixture, per the Harness
//     constructor below.
//   - ciab.v:19-21 (port list): `porta_in[5:0]`, `porta_out[7:6]`,
//     `portb_out[7:0]` -- unlike `ciaa`, `ciab` has a real, fully working
//     `portb_out` (regportb written at :131-141, pull-up-OR'd to the
//     output at :164). Not used by PROP-TIMER; noted only because it is
//     the generic port this package's declined page-6 claim (see
//     test_ciaa.cpp's RECORDED line) is talking about when it says
//     "modem control" / "disk motor and stepping control" cannot be told
//     apart at this boundary.
//
// MUTATION METHODOLOGY: identical to test_ciaa.cpp -- a compile-time `-D`
//   flag per assertion, no `MUT_*` token in any expect() label. Every
//   mutation was actually built and run (WSL Verilator, `make MUT=<NAME>
//   test`, full `rm -rf obj_dir_ciaa obj_dir_ciab` rebuild) and the
//   PASS/FAIL lines and tick counts quoted below are what that run
//   printed, not predictions.
//
// Baseline (no MUT, WSL Verilator 5.052): "2 passed, 0 failed" --
//   FIXTURE-LIVE-CIAB PASS, CIAB-TIMER-PROVIDED PASS. Measured: a Timer A
//   load of 2 reaches irq after exactly 2 ticks, a load of 10 after
//   exactly 10 -- identical shape to test_ciaa.cpp's own measurement,
//   confirming `ciab`'s `cia_timera` instance behaves the same as
//   `ciaa`'s (same source file, independently instantiated).
//
// MUTATION FIXTURE-LIVE-CIAB stimulus red: MUT_FIXLIVE_CIAB repeats the
//   first DDRA word (0x33) instead of writing a distinct second word
//   (0xCC) -> the two readings can no longer be told apart -> measured
//   "1 passed, 1 failed" (FIXTURE-LIVE-CIAB FAIL, CIAB-TIMER-PROVIDED
//   unaffected PASS). Reverted (baseline is MUT_FIXLIVE_CIAB=0).
// MUTATION CIAB-TIMER-PROVIDED stimulus red: MUT_NOTICK_CIAB holds `eclk`
//   at 0 for the whole run instead of 1 -> Timer A never underflows, `irq`
//   never asserts -> measured "1 passed, 1 failed" (both
//   ticks_to_underflow() calls return -1, CIAB-TIMER-PROVIDED FAIL,
//   FIXTURE-LIVE-CIAB unaffected PASS). Reverted (baseline is
//   MUT_NOTICK_CIAB=0).

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciab.h"

#ifndef MUT_FIXLIVE_CIAB
#define MUT_FIXLIVE_CIAB 0
#endif
#ifndef MUT_NOTICK_CIAB
#define MUT_NOTICK_CIAB 0
#endif

namespace {

constexpr uint8_t RS_DDRA = 0x2;
constexpr uint8_t RS_TALO = 0x4;
constexpr uint8_t RS_TAHI = 0x5;
constexpr uint8_t RS_ICR  = 0xD;
constexpr uint8_t RS_CRA  = 0xE;

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

// Bus-timing protocol for ciab.v, reused verbatim from
// hrm/ch8-ciabprb-disk-control-bits's own Harness (the suite's existing
// `ciab` fixture): regporta/ddrporta/timer registers are posedge-clk,
// clk7_en-gated, so a write needs one tick to latch; pa_out/porta_out/irq
// are combinational and need only eval(). `ciab` additionally exposes
// portb_out/flag -- held at their default (0), since PROP-TIMER concerns
// neither.
class Harness {
public:
    Harness() : dut(new Vciab) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->reset = 1;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->flag = 0;
        dut->porta_in = 0;
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

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciab* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE-CIAB: this exact build of `ciab` must be shown able to
// deliver a known value before anything PROP-TIMER depends on is trusted.
// Uses CIAADDRA -- a plain port register with no connection to the timer
// path -- so this precondition shares no mechanism with what PROP-TIMER
// grades.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h) {
    h.write_reg(RS_DDRA, 0x33);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, MUT_FIXLIVE_CIAB ? 0x33 : 0xCC);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0x33 && r2 == 0xCC && r1 != r2;
}

// ---------------------------------------------------------------------
// Identical mechanism to test_ciaa.cpp's ticks_to_underflow(): load
// Timer A's latch, unmask it in ICR, start it (continuous mode), tick the
// clock until `irq` reads high. Returns ticks needed (>=1), or -1 if it
// never fired within `max_ticks`.
// ---------------------------------------------------------------------
int ticks_to_underflow(Harness& h, uint16_t count, int max_ticks) {
    h.dut->eclk = MUT_NOTICK_CIAB ? 0 : 1;
    h.write_reg(RS_TALO, static_cast<uint8_t>(count & 0xFF));
    h.write_reg(RS_TAHI, static_cast<uint8_t>((count >> 8) & 0xFF));
    h.write_reg(RS_ICR, 0x81);   // unmask Timer A (bit7 set-mode, bit0 TA)
    h.write_reg(RS_CRA, 0x01);   // start, continuous mode, no force-load
    for (int i = 1; i <= max_ticks; ++i) {
        h.tick();
        if (h.dut->irq) return i;
    }
    return -1;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h),
            "FIXTURE-LIVE-CIAB: two distinct, known CIAADDRA bytes (0x33, "
            "0xCC) both arrive and read back correctly and distinctly "
            "before any data-dependent assertion is graded "
            "[no page -- infrastructure]");
    }

    int t_small, t_large;
    {
        Harness h;
        t_small = ticks_to_underflow(h, /*count=*/2, /*max_ticks=*/8);
    }
    {
        Harness h;
        t_large = ticks_to_underflow(h, /*count=*/10, /*max_ticks=*/16);
    }
    checks.note("CIAB-TIMER measured: a Timer A load of 2 reached a live, "
                "unmasked irq after " + std::to_string(t_small) +
                " clock tick(s) (-1 = never, within the bound given); a "
                "load of 10 reached it after " + std::to_string(t_large) +
                " tick(s).");
    checks.expect(t_small >= 1 && t_large >= 1 && t_large > t_small,
        "CIAB-TIMER-PROVIDED (p.12, sol:1-guidelines.c46, "
        "opus:1-guidelines.c46): ciab owns a timer a developer talking to "
        "its registers directly can load with a count, start, and see "
        "reach a detectable completion (ciab's own irq pin, unmasked "
        "through ICR) after real clock ticks -- not on the write itself "
        "(t_small/t_large both >=1 tick) and proportionally to the loaded "
        "count, not a fixed-latency pulse (a load of 10 takes measurably "
        "longer than a load of 2) [p.12 \"The 8520 I/O chips provide "
        "timers for developers who are bypassing the operating system\"]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
