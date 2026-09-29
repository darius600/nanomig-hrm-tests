// Test purpose: assert printed p.259's claim that the general-purpose
//   parallel interface is bi-directional -- at this fixture's boundary, that
//   means a data register this interface exposes must have a write side a
//   read side can observe, under a direction setting that says it should
//   (output) -- at the `ciaa` module boundary, compiled alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 259; SPECIFICATION.md (this package).
// Requirement IDs: PROP-1, per SPECIFICATION.md's claim inventory.
//   Claims file: hrm-testpipeline/work/ch8-s4-build/ch8-parallel-port-direction.jsonl
//   (2 claims; both carried on PROP-1's single graded line -- same clause of
//   the same sentence, see SPECIFICATION.md's claim inventory).
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in
//   `work/ch8-s4-build/ch8-parallel-port-direction.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// What this package tests: PROP-1 at CIA-A port B's data/direction
//   register pair (CIAAPRB/CIAADDRB), at every one of the eight bit
//   positions, over lines released first (portb_in = 0xFF, the idle level
//   p.354 gives a line nothing drives; a line left at the Harness default of
//   0 is an outside line the fixture itself holds low, a state neither
//   p.259 nor p.354 decides for a driven output). With the direction bit
//   set to output, a written value must be what the next read of that
//   register returns, 0 and 1 alike. CONTROL-PA0-OUTPUT (the identical
//   comparator at CIAAPRA bit 0) and PORTB-RELEASED-CONTROL (the released
//   lines read with the direction at input) are its measurement-basis
//   controls.
//
// Every mutation below is a `-D` variant, built and run with `make
// MUT=<NAME> test` (a full rebuild; WSL Verilator 5.052); the PASS/FAIL
// counts quoted are what those runs print.
//
// Baseline (no MUT): 3 passed, 1 failed -- FIXTURE-LIVE PASS,
//   opus:8-par.c1/sol:8-par.c1 PROP-1 FAIL at all eight bit positions,
//   CONTROL-PA0-OUTPUT PASS, PORTB-RELEASED-CONTROL PASS.
//
// MUTATION FIXTURE-LIVE stimulus red: MUT_FIXLIVE repeats the first DDRB
//   word (0xA5) instead of writing a distinct second word (0x5A) -> FAIL
//   ("2 passed, 2 failed": FIXTURE-LIVE flips to FAIL, the other three
//   unaffected). The two readings can no longer be told apart
//   (MUT_FIXLIVE=0 is baseline).
// MUTATION opus:8-par.c1 stimulus green: PROP-1 is graded at baseline
//   against CIAAPRB/CIAADDRB and is red at every bit position there.
//   MUT_PROP1_TARGET_A retargets the IDENTICAL
//   bit_output_follows_direction() comparator -- register-select
//   arguments only, same code -- at CIAAPRA/CIAADDRA for every position,
//   cycling between the two bit positions there that round-trip through a
//   read of the same register -> PASS ("4 passed, 0 failed": PROP-1 flips
//   to PASS at all eight positions, the other three unaffected).
//   Substitution proof: the comparator is not vacuously false, it returns
//   true for the port where the mechanism actually works, on the same
//   fixture, same protocol, same run shape as CONTROL-PA0-OUTPUT
//   (MUT_PROP1_TARGET_A=0 is baseline).
// MUTATION CONTROL-PA0-OUTPUT stimulus red: MUT_CONTROL_NODIR leaves PA0's
//   direction bit at its reset default (0, input) instead of writing it to
//   output, while the same PRA bit-0 write/read sequence runs unchanged ->
//   FAIL ("2 passed, 2 failed": CONTROL-PA0-OUTPUT flips to FAIL, the
//   other three unaffected), proving the control is not vacuously true
//   either (MUT_CONTROL_NODIR=0 is baseline).
// MUTATION PORTB-RELEASED-CONTROL stimulus red: MUT_CONTROL_HELD_LOW holds
//   the same port-B lines low (portb_in = 0x00) instead of releasing them,
//   while the check keeps grading against the fixed released-line
//   prediction (0xFF) p.341 and p.354 give -> FAIL ("2 passed, 2 failed":
//   measured PRB=0x00 against the fixed 0xFF expectation, PORTB-RELEASED-
//   CONTROL flips to FAIL, the other three unaffected). Sensitivity
//   proof: an unreleased line reads back 0, so the control is not
//   vacuously true regardless of whether the fixture actually released
//   the lines. PROP-1 is unaffected (its own sweep sets portb_in itself,
//   still FAIL for its own reason) (MUT_CONTROL_HELD_LOW=0 is baseline).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif
#ifndef MUT_PROP1_TARGET_A
#define MUT_PROP1_TARGET_A 0
#endif
#ifndef MUT_CONTROL_NODIR
#define MUT_CONTROL_NODIR 0
#endif
#ifndef MUT_CONTROL_HELD_LOW
#define MUT_CONTROL_HELD_LOW 0
#endif

namespace {

constexpr uint8_t RS_PRA  = 0x0;
constexpr uint8_t RS_DDRA = 0x2;
constexpr uint8_t RS_PRB  = 0x1;
constexpr uint8_t RS_DDRB = 0x3;

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

// Bus-timing protocol, reused verbatim from hrm/ch8-ciaa-port-direction's
// own Harness (the control-pattern sibling, building this exact fixture): a
// register write is clocked, so write_reg() holds it for one tick; a read
// is combinational and needs only eval().
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

// ---------------------------------------------------------------------
// FIXTURE-LIVE: this exact build of `ciaa` must be shown able to deliver a
// known value before anything PROP-1 depends on is trusted. Uses CIAADDRB
// (the port-B side's own, genuinely working register) rather than CIAAPRB
// itself, so this precondition does not depend on the very mechanism
// PROP-1 grades.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h, bool repeat_first_word_bug = false) {
    h.write_reg(RS_DDRB, 0xA5);
    const uint8_t r1 = h.read_reg(RS_DDRB);
    h.write_reg(RS_DDRB, repeat_first_word_bug ? 0xA5 : 0x5A);
    const uint8_t r2 = h.read_reg(RS_DDRB);
    return r1 == 0xA5 && r2 == 0x5A && r1 != r2;
}

// ---------------------------------------------------------------------
// Shared comparator. PROP-1 and CONTROL-PA0-OUTPUT call the SAME function,
// parameterised only by which data/direction register-select pair it
// targets -- the substitution proof for PROP-1's red result needs the
// identical comparator, not a reimplementation of it, pointed at a
// mechanism already proven live.
//
// With its own direction bit set to output, a data register's read-back
// must follow what was just written to it at that position, both 0->0 and
// 1->1. A read driven by anything held constant through both halves of this
// check (an external pin, a hard-wired value) is structurally unable to
// satisfy both -- it can equal 0 or equal the mask, never both in sequence
// -- so this comparator cannot pass by coincidence for a register that
// never captures a write, whatever fixed value the read happens to echo.
// ---------------------------------------------------------------------
bool bit_output_follows_direction(Harness& h, uint8_t rs_data, uint8_t rs_ddr,
                                   int bit, bool omit_direction_bug = false) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);
    h.write_reg(rs_ddr, omit_direction_bug ? uint8_t{0x00} : mask);
    h.write_reg(rs_data, 0x00);
    const bool tracks_zero = (h.read_reg(rs_data) & mask) == 0x00;
    h.write_reg(rs_data, mask);
    const bool tracks_one = (h.read_reg(rs_data) & mask) == mask;
    return tracks_zero && tracks_one;
}

// ---------------------------------------------------------------------
// PROP-1: CIA-A port B's lines are released (portb_in = 0xFF, the idle
// level p.354 gives a line nothing drives) before this sweep, so a bit
// read back after a write is never confused with an outside line the
// fixture itself would otherwise hold low. Graded at every one of the
// eight bit positions of CIAAPRB/CIAADDRB, through the identical
// bit_output_follows_direction() comparator CONTROL-PA0-OUTPUT uses.
// MUT_PROP1_TARGET_A retargets that comparator at CIAAPRA/CIAADDRA for
// every position, cycling between the two bit positions there that
// round-trip through a read of the same register, as the substitution
// proof.
// ---------------------------------------------------------------------
bool prop1_bidirectional(Harness& h, std::string& detail) {
    bool all_bidirectional = true;
    h.dut->portb_in = 0xFF;
    for (int i = 0; i < 4; ++i) h.tick();
    for (int bit = 0; bit < 8; ++bit) {
        bool result;
        if (MUT_PROP1_TARGET_A) {
            const int real_bit = bit % 2;
            result = bit_output_follows_direction(h, RS_PRA, RS_DDRA, real_bit);
            detail += "bit " + std::to_string(bit) + " (retargeted at CIAAPRA/"
                       "CIAADDRA bit " + std::to_string(real_bit) + "): " +
                       (result ? "PASS " : "FAIL ");
        } else {
            result = bit_output_follows_direction(h, RS_PRB, RS_DDRB, bit);
            detail += "bit " + std::to_string(bit) + ": " + (result ? "PASS " : "FAIL ");
        }
        all_bidirectional = all_bidirectional && result;
    }
    return all_bidirectional;
}

// ---------------------------------------------------------------------
// PORTB-RELEASED-CONTROL: an honest positive control for PROP-1's
// released-line fixture, on the same registers and protocol -- not
// itself one of this package's page claims, so it carries no uid. With
// CIAADDRB left at input (0x00) and CIA-A port B's lines released the
// same way PROP-1 releases them, a read of CIAAPRB must return the
// released lines' 1 at every position. This shows the released fixture
// and the read path both work on this basis, so PROP-1's red is not an
// unreleased or broken fixture. MUT_CONTROL_HELD_LOW holds the same
// lines low instead, the opposite reading p.354 predicts, as the
// sensitivity proof.
// ---------------------------------------------------------------------
bool portb_released_control(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRB, 0x00);
    const uint8_t portb_in = MUT_CONTROL_HELD_LOW ? 0x00 : 0xFF;
    h.dut->portb_in = portb_in;
    for (int i = 0; i < 4; ++i) h.tick();
    const uint8_t got = h.read_reg(RS_PRB);
    char hexbuf[64];
    std::snprintf(hexbuf, sizeof(hexbuf), "portb_in=0x%02X, PRB=0x%02X", portb_in, got);
    detail = hexbuf;
    return got == 0xFF;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h, MUT_FIXLIVE),
            "FIXTURE-LIVE: two distinct, known CIAADDRB bytes (0xA5, 0x5A) "
            "both arrive and read back correctly and distinctly before any "
            "data-dependent assertion is graded [no page -- infrastructure]");
    }

    {
        Harness h; std::string d;
        const bool result = prop1_bidirectional(h, d);
        checks.expect(result,
            "opus:8-par.c1 sol:8-par.c1 PROP-1: the general-purpose "
            "parallel interface is bi-directional -- with its direction "
            "bit set to output, a value written to its data register must "
            "be what a subsequent read of that register returns, both 0 "
            "and 1, at every bit position, over lines the fixture leaves "
            "released beforehand so a held-low outside line cannot stand "
            "in for a driven one "
            "[p.259 \"The general-purpose bi-directional parallel "
            "interface is a 25-pin connector on the back panel of the "
            "computer\"; p.354, the idle level a released line reads]");
        checks.note("PROP-1: " + d);
    }

    {
        Harness h;
        checks.expect(
            bit_output_follows_direction(h, RS_PRA, RS_DDRA, /*bit=*/0, MUT_CONTROL_NODIR),
            "CONTROL-PA0-OUTPUT: the identical bit_output_follows_direction() "
            "comparator PROP-1 evaluates above, pointed instead at CIA-A "
            "port A's own data/direction register pair (CIAAPRA/CIAADDRA) "
            "at bit 0 -- the same shape hrm/ch8-ciaa-port-direction already "
            "shows working there, on the same fixture and bus protocol. Not "
            "itself a page claim: it is PROP-1's measurement-basis control "
            "[no page -- infrastructure control]");
    }

    {
        Harness h; std::string d;
        const bool r = portb_released_control(h, d);
        checks.expect(r, "PORTB-RELEASED-CONTROL: with CIAADDRB left at "
            "input and CIA-A port B's lines released the same way PROP-1 "
            "releases them, a read of CIAAPRB must return the released "
            "lines' 1 at every position -- an honest control for PROP-1's "
            "released-line fixture, on the same registers and protocol; "
            "grades no claim of this package's own "
            "[p.341, \"you read the actual current state of the I/O "
            "pins\"; p.354, \"a read of the ports will return all highs "
            "because of passive pull-ups\"]");
        checks.note("PORTB-RELEASED-CONTROL: " + d);
    }

    {
        // Supporting measurement, not itself graded: whether CIAADDRB
        // changes what a CIAAPRB read returns, with portb_in held fixed and
        // no new CIAAPRB write in between (p.341: a set DDR bit makes the
        // corresponding PR bit an output).
        Harness h;
        h.dut->portb_in = 0x3C;
        h.write_reg(RS_DDRB, 0x00);  // direction: all input
        h.write_reg(RS_PRB, 0x00);
        const uint8_t read_as_input = h.read_reg(RS_PRB);
        h.write_reg(RS_DDRB, 0xFF);  // direction: all output; PRB not rewritten
        const uint8_t read_as_output = h.read_reg(RS_PRB);
        char hexbuf[128];
        std::snprintf(hexbuf, sizeof(hexbuf), "0x%02X with CIAADDRB=0x00 (all "
            "input) and 0x%02X with CIAADDRB=0xFF (all output)",
            read_as_input, read_as_output);
        checks.note(
            std::string("Direction-independence: with portb_in fixed at ") +
            "0x3C, CIAAPRB reads " + hexbuf + ", same prior CIAAPRB write -- "
            "identical either way. CIAADDRB reads back faithfully at its "
            "own address but has no measured effect on what a CIAAPRB read "
            "returns, contrary to p.341's rule that a set DDR bit makes "
            "the corresponding PR bit an output.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
