// appF-cia-io-ports -- ciaa conformance test
//
// Test purpose: this package's 44 claims (p.341, "I/O PORTS (PRA, PRB, DDRA,
//   DDRB)") are, per SPECIFICATION.md, **44 of 44 already graded elsewhere**
//   -- every uid's own `classify.note` names an accepted package and uid
//   that already grades the identical printed proposition at this same
//   `ciaa` boundary. Per the build brief, that shape is declined in full on
//   `record()` lines citing the accepted package/uid, not re-graded. This is
//   a correct outcome, not a failure -- `hrm/ch1-userio-ports` and
//   `hrm/ch1-keyboard-translation` are this batch's own precedent for it.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 341; the duplicate-grading citations below point to
//   `hrm/ch8-parallel-port-direction` (PROP-1, FAIL) and
//   `hrm/ch8-ciaa-port-direction` (PROP-1 and PROP-2, both FAIL), verified
//   directly against those packages' own test sources -- see
//   SPECIFICATION.md's "Duplicate-grading check".
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM COVERAGE: all 44 uids assigned to this package are declined below,
//   on the three record() lines that follow the fixture-liveness control --
//   none is graded with expect() here.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

namespace {

constexpr uint8_t RS_DDRA = 0x2;

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

// Bus-timing protocol for ciaa.v, the same as this batch's other `ciaa`
// packages (hrm/ch1-cia-timers's own Harness for this fixture): registers
// are posedge-clk, clk7_en-gated, so a write needs one tick to latch;
// data_out is combinational and needs only eval().
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

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif

// FIXTURE-LIVENESS: this exact build of `ciaa` must be shown able to
// deliver a known value before a record()-only package's decline is
// trusted not to be concealing a dead harness. Uses CIAADDRA (rs=0x2) --
// the same register hrm/ch1-cia-timers's own fixture-liveness check uses
// for this fixture, reused here as verified plumbing.
bool fixture_live(Harness& h) {
    h.write_reg(RS_DDRA, 0x55);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, MUT_FIXLIVE ? 0x55 : 0xAA);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0x55 && r2 == 0xAA && r1 != r2;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h),
            "FIXTURE-LIVENESS: two distinct, known CIAADDRA bytes (0x55, "
            "0xAA) both arrive and read back correctly and distinctly "
            "before any record() line below is trusted [no page -- "
            "infrastructure]");
    }
    // MUTATION FIXTURE-LIVENESS stimulus red MUT_FIXLIVE: repeat the first CIAADDRA byte (0x55) instead of writing a distinct second one (0xAA) -- the two readings can no longer be told apart -- baseline PASS goes red.

    checks.record(
        "GROUP-A (p.341, ch8-parallel-port-direction PROP-1, "
        "opus:8-par.c1/sol:8-par.c1, verified FAIL at baseline): covers "
        "every page-341 proposition this package's classifier reduced to "
        "port B's write-then-read path -- \"Ports A and B each consist of "
        "an 8-bit peripheral data register (PR) and an 8-bit data "
        "direction register (DDR)\" (register existence, port-B half), "
        "\"When you READ a PR register, you read the actual current state "
        "of the I/O pins ... regardless of whether you have set them to be "
        "inputs or outputs\" (port-B half), and the REG NAME table's PRB "
        "and DDRB rows (p.341, rows 1 and 3), each restating the same "
        "port-B fact at its own register address. PROP-1 already measures, "
        "at this identical ciaa boundary, that CIAAPRB's write register is "
        "commented out in the RTL, so a read always echoes portb_in rather "
        "than anything the CPU itself wrote -- FAIL. Re-grading any of "
        "these 22 uids at this package's own boundary would test the "
        "identical mechanism PROP-1 already measured and risks reaching a "
        "different verdict about the same register. Declined in full, not "
        "re-asserted. [claims: opus:F-ports.c1, opus:F-ports.c4, "
        "opus:F-ports.c9, opus:F-ports.c11, sol:F-ports.c1, "
        "sol:F-ports.c4, sol:F-ports.c16, sol:F-ports.c17, "
        "sol:F-ports.c18, sol:F-ports.c19, sol:F-ports.c20, "
        "sol:F-ports.c21, sol:F-ports.c22, sol:F-ports.c23, "
        "sol:F-ports.c32, sol:F-ports.c33, sol:F-ports.c34, "
        "sol:F-ports.c35, sol:F-ports.c36, sol:F-ports.c37, "
        "sol:F-ports.c38, sol:F-ports.c39]");

    checks.record(
        "GROUP-B (p.341, ch8-ciaa-port-direction PROP-2, "
        "opus:8-dio.c46/sol:8-dio.c43, verified FAIL at baseline): \"If a "
        "bit in the DDR is set to a 1, the corresponding bit position in "
        "the PR becomes an output. If a DDR bit is set to a 0, the "
        "corresponding PR bit is defined as an input\" (both directions of "
        "the DDR-bit-meaning sentence, port-A framing). PROP-2 already "
        "measures this exact proposition at this identical ciaa boundary, "
        "graded across all eight PA0-PA7 bit positions, and reports it "
        "true only for PA0/PA1, false for PA2-PA7 -- FAIL overall. "
        "Re-grading here would test the identical mechanism. Declined in "
        "full, not re-asserted. [claims: opus:F-ports.c2, opus:F-ports.c3, "
        "sol:F-ports.c2, sol:F-ports.c3]");

    checks.record(
        "GROUP-C (p.341, ch8-ciaa-port-direction PROP-1, opus:8-dio.c44, "
        "verified FAIL at baseline): covers the REG NAME table's PRA and "
        "DDRA rows (p.341, rows 0 and 2) -- the one-to-one bit-position "
        "correspondence between CIAADDRA and CIAAPRA the table's own "
        "column layout asserts. PROP-1 already measures this exact "
        "correspondence at this identical ciaa boundary, graded across all "
        "eight PA0-PA7 positions, and reports it true only for PA0/PA1, "
        "false for PA2-PA7 -- FAIL overall. Re-grading here would test the "
        "identical mechanism. Declined in full, not re-asserted. [claims: "
        "opus:F-ports.c8, opus:F-ports.c10, sol:F-ports.c8, "
        "sol:F-ports.c9, sol:F-ports.c10, sol:F-ports.c11, "
        "sol:F-ports.c12, sol:F-ports.c13, sol:F-ports.c14, "
        "sol:F-ports.c15, sol:F-ports.c24, sol:F-ports.c25, "
        "sol:F-ports.c26, sol:F-ports.c27, sol:F-ports.c28, "
        "sol:F-ports.c29, sol:F-ports.c30, sol:F-ports.c31]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
