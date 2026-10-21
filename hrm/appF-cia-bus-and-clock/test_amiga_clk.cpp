// appF-cia-bus-and-clock -- amiga_clk conformance test
//
// Test purpose: grade p.353's "The E clock runs at 1/10 of the CPU clock"
//   against `amiga_clk` (amiga_clk.v). SPECIFICATION.md records the claim
//   inventory and the duplicate-grading check for this package's other three
//   (already-graded) uids, printed from the sibling
//   test_minimig_m68k_bridge.cpp binary. This file carries the one
//   proposition assigned to the `amiga_clk` fixture.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 353.
// Non-goals: anything outside the `amiga_clk` fixture this file builds -- this
//   package Verilates two independent tops from two sources, and this
//   is one of them
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM COVERAGE: sol:F-signals.c71, opus:F-signals.c3 (E-CLOCK-IS-TENTH).
//
// RTL read directly: amiga_clk.v (66 lines) in full, and
// `classify.evidence`'s own already-cited lines (amiga_clk.v:22 `shifter`
// declared 10 bits; :36 rotates it once per clk7_cnt==2'b01, i.e. once per
// 7MHz-equivalent period; :64 `eclk = shifter`; :43-44 `clk7_en`/`clk7n_en`
// from the same 2-bit counter that gates c1/c3, which is what the CPU's
// own bus phases are built from elsewhere in the tree per
// minimig_m68k_bridge's own timing-diagram comment -- "CPU_CLK" toggles at
// the clk7_en/clk7n_en rate). "CPU clock" is therefore operationalized as
// the `clk7_en` pulse rate at this fixture's own boundary, and "E clock" as
// one bit of the 10-bit one-hot `eclk` bus -- `eclk[8]`, the specific bit
// minimig.v:630/645 wires into each CIA's single-bit `eclk` port (confirmed
// read directly; by the shifter's own symmetry every bit behaves
// identically, so this choice is cosmetic, made only to match what the
// rest of the tree actually uses).
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag. Every mutation was actually built and run (WSL Verilator,
//   `make MUT=<NAME> test`, full `rm -rf obj_dir_amiga_clk` rebuild).

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vamiga_clk.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif
#ifndef MUT_COUNT_RAW_TICKS
#define MUT_COUNT_RAW_TICKS 0
#endif

namespace {

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

class Harness {
public:
    Harness() : dut(new Vamiga_clk) {
        dut->clk_28 = 0;
        dut->reset_n = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset_n = 1;
        for (int i = 0; i < 8; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One clk_28 period.
    void tick() {
        dut->eval();
        dut->clk_28 = 1;
        dut->eval();
        dut->clk_28 = 0;
        dut->eval();
    }

    Vamiga_clk* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // FIXTURE-LIVENESS: this exact build of `amiga_clk` must be shown to
    // actually pulse clk7_en at all (a dead/tied-off clk7_en would make
    // E-CLOCK-IS-TENTH vacuously untestable, not vacuously true) before
    // that assertion is trusted.
    // =====================================================================
    {
        Harness h;
        // Counts RISING EDGES of clk7_en, not "samples read high" -- a
        // level count cannot tell a real pulse train from a wire stuck
        // high, which is exactly the failure mode this control exists to
        // catch. Under MUT_FIXLIVE the measurement loop calls only
        // dut->eval() -- never h.tick(), so clk_28 is never toggled during
        // this window and clk7_en_reg (a register clocked only on
        // posedge clk_28) cannot change: a withheld stimulus, local to
        // this measurement window, not a change to the expected value.
        // (Harness's own construction above still ticks normally, so
        // reset completes for real before this window starts.)
        int edges = 0;
        bool prev = h.dut->clk7_en;
        for (int i = 0; i < 40; ++i) {
            if (MUT_FIXLIVE) {
                h.dut->eval();
            } else {
                h.tick();
            }
            const bool cur = h.dut->clk7_en;
            if (cur && !prev) ++edges;
            prev = cur;
        }
        checks.note("FIXTURE-LIVENESS: " + std::to_string(edges) +
                    " clk7_en rising edge(s) observed over a 40-tick window.");
        checks.expect(edges >= 8,
            "FIXTURE-LIVENESS: clk7_en (the CPU-clock reference "
            "E-CLOCK-IS-TENTH is measured against) actually pulses, "
            "repeatedly, before that assertion is trusted [no page -- "
            "infrastructure]");
    }
    // MUTATION FIXTURE-LIVENESS stimulus red MUT_FIXLIVE: never call h.tick() during the measurement window (dut->eval() only, clk_28 never toggled for those 40 iterations) -- clk7_en_reg is a register clocked solely on posedge clk_28, so it cannot change without that edge; edges stays 0, and the edges >= 8 conjunct goes red.

    // =====================================================================
    // E-CLOCK-IS-TENTH
    // =====================================================================
    {
        Harness h;
        const int bit = 8;  // the bit minimig.v:630/645 actually wires into each CIA's eclk port
        // Find the tick index of the first two rising edges of eclk[bit],
        // and the clk7_en pulse count strictly between them. MUT_COUNT_RAW_TICKS
        // counts every clk_28 tick instead of only clk7_en-qualified ones --
        // the wrong time base for "CPU clock".
        int prev_bit = (h.dut->eclk >> bit) & 1;
        int edges_seen = 0;
        int clk7_en_between = 0;
        bool counting = false;
        for (int i = 0; i < 200 && edges_seen < 2; ++i) {
            h.tick();
            const int cur_bit = (h.dut->eclk >> bit) & 1;
            if (counting && (MUT_COUNT_RAW_TICKS || h.dut->clk7_en)) ++clk7_en_between;
            if (cur_bit && !prev_bit) {
                ++edges_seen;
                if (edges_seen == 1) counting = true;
                else counting = false;
            }
            prev_bit = cur_bit;
        }
        checks.note("E-CLOCK-IS-TENTH: " + std::to_string(clk7_en_between) +
                    " clk7_en pulse(s) (\"CPU clock\" ticks) observed strictly between two "
                    "consecutive rising edges of eclk[" + std::to_string(bit) + "].");
        checks.expect(clk7_en_between == 10,
            "E-CLOCK-IS-TENTH (p.353, sol:F-signals.c71, opus:F-signals.c3): "
            "\"The E clock runs at 1/10 of the CPU clock.\" -- exactly 10 "
            "clk7_en (\"CPU clock\") pulses occur between consecutive "
            "rising edges of the eclk bit minimig.v wires into the CIAs, "
            "confirmed by direct count, not assumed from the module's own "
            "port comments.");
    }
    // MUTATION E-CLOCK-IS-TENTH stimulus red MUT_COUNT_RAW_TICKS: count every clk_28 tick between eclk[8] edges instead of only clk7_en-qualified ones (the wrong time base for "CPU clock") -- measures 40 clk_28 ticks (10 CPU-clock periods x 4 clk_28 ticks each) instead of 10, so the == 10 conjunct goes red.

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
