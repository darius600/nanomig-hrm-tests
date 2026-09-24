// example -- the shortest package that is still a real one
//
// Test purpose: show the shape of a conformance package in one screen. This
//   grades one bit of CIAA's port-A data-direction register: after reset,
//   DDRA reads 0x00, so every port-A line is an input. Printed page 292 of
//   the Amiga Hardware Reference Manual, third edition, states it.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p. 292.
// Non-goals: this package tests nothing else. It is an example, not coverage:
//   the real CIAA packages are hrm/appDE-fire-buttons-ciaa and
//   hrm/ch8-ciaa-port-direction, and the fixture here is theirs.
//
// WHY THIS FILE EXISTS. docs/adding-a-test.md names
// hrm/ch3-beam-counters-and-blanking/test_agnus_beamcounter.cpp as the worked
// example to copy from. It is 1,749 lines, and a persona review pointed out
// that "a package to copy from" ought not to mean searching a file that long
// for the thirty lines that matter. That file stays the worked example --
// it shows what a full package looks like. This one shows what the minimum
// looks like, and the two are meant to be read in that order.
//
// THE THREE VERBS, and the difference between them is the whole grading model:
//   expect()  grades. PASS or FAIL, and a FAIL is a finding.
//   note()    prints MEASURED:. It records what the RTL did without judging it.
//   record()  prints RECORDED:. It states something the test knows and does
//             not measure -- a page number, a decision, a limit of the fixture.
// Only expect() lines count as assertions. A uid on a graded line makes the
// claim it cites ASSERTED; on a note() line it does not.
//
// MUTATION DDRA stimulus red: under MUTATE=1 the test writes 0xFF to DDRA after
//   reset and before the read; DDRA reads back 0xFF and this assertion goes
//   FAIL (example: 0 passed, 1 failed). Without MUTATE: 1 passed, 0 failed.
//   Every assertion carries one record like this; `DDRA` is the id the gate
//   reads from the first word of the expect() label.

#include "Vciaa.h"
#include "verilated.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

constexpr uint8_t RS_DDRA = 0x2;

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vciaa dut;
    Checks c;

    // Drive every input. A Verilated model starts with X, so an input left
    // alone is not zero -- it is undefined, and the first real package to
    // forget one spent an afternoon on it. clk7_en and clk7n_en are held high
    // so that every clock edge is a synchronising one.
    dut.clk = 0; dut.clk7_en = 1; dut.clk7n_en = 1;
    dut.aen = 0; dut.rd = 0; dut.wr = 0; dut.rs = 0; dut.data_in = 0;
    dut.tick = 0; dut.eclk = 0; dut.porta_in = 0; dut.portb_in = 0;
    dut.kms_level = 0; dut.kbd_mouse_type = 0; dut.kbd_mouse_data = 0;
    dut.hrtmon_en = 0;

    dut.reset = 1;
    for (int i = 0; i < 16; ++i) { dut.clk = !dut.clk; dut.eval(); }
    dut.reset = 0;
    for (int i = 0; i < 8; ++i)  { dut.clk = !dut.clk; dut.eval(); }

    // MUTATION GUARD. Every expect() in a real package is guarded like this,
    // so that `MUTATE=1 make test` can prove the assertion is capable of going
    // red. It changes what the module is ASKED TO DO -- here, a write of 0xFF
    // to DDRA before the read -- and never the expected value: flipping an
    // expected value proves only that a boolean can be false. An assertion
    // that cannot fail measures nothing, and the mutation gate is what
    // catches one that cannot.
    if (getenv("MUTATE") != nullptr) {
        dut.aen = 1; dut.wr = 1; dut.rs = RS_DDRA; dut.data_in = 0xFF;
        for (int i = 0; i < 4; ++i) { dut.clk = !dut.clk; dut.eval(); }
        dut.aen = 0; dut.wr = 0; dut.data_in = 0;
        for (int i = 0; i < 4; ++i) { dut.clk = !dut.clk; dut.eval(); }
    }

    // Read DDRA. The read mux is combinational, so eval() is enough.
    dut.aen = 1; dut.rd = 1; dut.rs = RS_DDRA;
    dut.eval();
    const uint8_t ddra = dut.data_out;

    char hex[3];
    std::snprintf(hex, sizeof hex, "%02X", ddra);
    c.note(std::string("DDRA after reset reads 0x") + hex);
    c.record("oracle: HRM 3rd ed., printed p. 292 -- CIA data-direction "
             "registers are cleared by reset, making every line an input");

    c.expect(ddra == 0x00,
             "DDRA is 0x00 after reset, so port A is all inputs "
             "[appDE-p292.c1]");

    std::cout << "\nexample: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed ? 1 : 0;
}
