// Test purpose: p.324's `PARALLEL PORT (J8)` table (Appendix E, read from
//   `figures/p324.png` -- the NOTES column is displaced against NAME in the
//   text layer on this page) and its surrounding prose, restricted to the
//   D0-D7 data-bus rows and the "may also be used for input"/"eight-bit
//   bidirectional bus" clauses -- at the `ciaa` module boundary, compiled
//   alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 324 (figure and body text); p.319's Centronics pinout table read
//   for comparison only, not itself the source of any assertion here (its
//   own classify.package filter carries no claims into this package).
// Requirement IDs: PORTB-WIDTH, PORTB-DIRECTION, per this package's own
//   claim inventory. Claims file:
//   hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl, filtered to
//   classify.package == "appDE-parallel-port-ciaa" (11 claims: 8 sol
//   per-line D0-D7 restatements off the figure, sol:E-parallel.c69..c76;
//   2 duplicate-bidirectionality-prose claims, sol:E-parallel.c66 and
//   opus:E-parallel-iface.c5; 1 opus restatement of the whole D0-D7 bus
//   clause, opus:E-parallel-iface.c10).
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// Claim disposition:
//   - sol:E-parallel.c66, opus:E-parallel-iface.c5 (p.324 prose, "This
//     interface may also be used for input"/"may also be used for input or
//     bidirectional data transfers"): the identical proposition p.259's
//     "bi-directional" already states, and hrm/ch8-parallel-port-direction's
//     PROP-1 already grades it (FAIL) at this same ciaa boundary. Declined.
//   - sol:E-parallel.c69..c76 (figure rows D0..D7) and opus:E-parallel-
//     iface.c10 (figure prose "D0-D7 comprise an eight-bit bidirectional
//     bus"): each restates BOTH that its named line is one of eight
//     independently-addressable positions (D0..D7 -- a WIDTH claim PROP-1
//     does not test) AND that each line's own DIR column reads I/O (a
//     DIRECTION claim), distinct from the plain-bidirectionality prose
//     above. Graded here as TWO assertions (PORTB-WIDTH, PORTB-DIRECTION),
//     both carrying all nine uids, because the two component claims (width;
//     direction) measure independently and forcing them into one AND would
//     hide which half is true. PORTB-WIDTH is TRUE (measured).
//     PORTB-DIRECTION is FALSE at every one of the eight positions
//     (measured) -- the same eight-position sweep
//     hrm/ch8-parallel-port-direction's PROP-1 grades against p.259, graded
//     here against p.324's per-line DIR column.
//   - PORTB-DIRECTION's fixture releases every PB line before the sweep
//     (portb_in = 0xFF, the idle state p.354 gives a line nothing drives)
//     instead of leaving them at the Harness default of 0, a line held low
//     from outside -- a state no page decides for a driven output.
//     PORTB-RELEASED-CONTROL shows the released lines read back as 1
//     through the same registers. The finding is NM-HRM-054's.
//
// Every mutation below is a `-D` variant, built and run with `make
// MUT=<NAME> test` (a full rebuild via the Makefile's `test` recipe; WSL
// Verilator 5.052); the PASS/FAIL counts quoted are what those runs print.
// The file as committed builds at MUT unset (baseline).
//
// Baseline (no MUT): "3 passed, 1 failed" -- FIXTURE-LIVE PASS,
//   PORTB-WIDTH PASS, PORTB-DIRECTION FAIL at all eight positions,
//   PORTB-RELEASED-CONTROL PASS.
//
// MUTATION FIXTURE-LIVE stimulus red: MUT_FIXLIVE repeats the first DDRB
//   byte (0xA5) instead of a distinct second one -> FIXTURE-LIVE FAIL,
//   "2 passed, 2 failed".
// MUTATION PORTB-WIDTH stimulus red: MUT_WIDTH_SAMEPATTERN presents the
//   SAME portb_in byte (0x01) at every swept bit position instead of a
//   distinct single-bit pattern per position -> PORTB-WIDTH FAIL,
//   "2 passed, 2 failed" (all eight reads now equal, isolation cannot be
//   shown). PORTB-DIRECTION is unaffected (still FAIL, for its own
//   independent reason).
// MUTATION PORTB-DIRECTION stimulus green: MUT_DIRECTION_TARGET_A
//   retargets the whole eight-position sweep at RS_PRA/RS_DDRA, cycling
//   bit%2 across the two positions (0, 1) at which a PRA read returns the
//   written value, instead of RS_PRB/RS_DDRB -- a real substitution,
//   reusing hrm/ch8-parallel-port-direction's own validated
//   bit_output_follows_direction() unchanged -> PORTB-DIRECTION PASS,
//   "4 passed, 0 failed". Substitution proof: the comparator is not
//   hardwired false, it returns true for the register pair where the
//   mechanism actually works, on the same fixture and protocol.
//   PORTB-WIDTH and PORTB-RELEASED-CONTROL are unaffected (still PASS;
//   neither reads PRA/DDRA).
// MUTATION PORTB-RELEASED-CONTROL stimulus red: MUT_CONTROL_HELD_LOW holds
//   the same PB lines low (portb_in = 0x00) instead of releasing them,
//   while the check keeps grading against the fixed released-line
//   prediction (0xFF) p.341 and p.354 give -> PORTB-RELEASED-CONTROL FAIL,
//   "2 passed, 2 failed" (measured PRB=0x00 against the fixed 0xFF
//   expectation). Sensitivity proof: an un-released line reads back 0, so
//   the control is not vacuously true regardless of whether the fixture
//   actually released the lines. PORTB-DIRECTION is unaffected (its own
//   sweep sets portb_in itself, still FAIL for its own reason).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif
#ifndef MUT_WIDTH_SAMEPATTERN
#define MUT_WIDTH_SAMEPATTERN 0
#endif
#ifndef MUT_DIRECTION_TARGET_A
#define MUT_DIRECTION_TARGET_A 0
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

std::string hex2(unsigned long v) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "0x%02lX", v);
    return std::string(buf);
}

// Bus-timing protocol, reused verbatim from
// hrm/ch8-parallel-port-direction's own Harness (this exact fixture, same
// top module): a register write is clocked, so write_reg() holds it for one
// tick; a read is combinational and needs only eval().
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
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
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
        step();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        step();
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

    void step() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE: two distinct, known DDRB bytes must both arrive and read
// back correctly and distinctly before anything below is trusted. Uses
// DDRB (the port-B side's own genuinely working register) rather than PRB
// itself, so this precondition does not depend on the very mechanism
// PORTB-DIRECTION grades -- same reasoning hrm/ch8-parallel-port-direction's
// own FIXTURE-LIVE uses.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h) {
    h.write_reg(RS_DDRB, 0xA5);
    const uint8_t r1 = h.read_reg(RS_DDRB);
    h.write_reg(RS_DDRB, MUT_FIXLIVE ? 0xA5 : 0x5A);
    const uint8_t r2 = h.read_reg(RS_DDRB);
    return r1 == 0xA5 && r2 == 0x5A && r1 != r2;
}

// ---------------------------------------------------------------------
// PORTB-WIDTH: "D0-D7 comprise an eight-bit bidirectional bus" -- the WIDTH
// half. Toggling portb_in at one bit position must move ONLY that PRB bit,
// not its neighbours, at each of the eight named lines. Input-side only
// (this half does not require a write path to exist; PORTB-DIRECTION below
// grades that separately).
// ---------------------------------------------------------------------
bool portb_width(Harness& h, std::string& detail) {
    bool all_isolated = true;
    uint8_t prev = 0x00;
    for (int bit = 0; bit < 8; ++bit) {
        const uint8_t pattern = MUT_WIDTH_SAMEPATTERN
            ? uint8_t{0x01}
            : static_cast<uint8_t>(1u << bit);
        h.dut->portb_in = pattern;
        for (int i = 0; i < 4; ++i) h.step();
        const uint8_t got = h.read_reg(RS_PRB);
        const bool echoes = (got == pattern);
        // The position claim, not just the echo claim: this bit's own
        // reading must differ from the PREVIOUS position's, or a stimulus
        // that collapses every position onto the same pattern (the
        // mutation below) would pass this check vacuously -- an echo test
        // alone cannot tell "8 distinct positions" from "1 position,
        // asked about 8 times".
        const bool distinct_from_prev = (bit == 0) || (got != prev);
        const bool isolated = echoes && distinct_from_prev;
        all_isolated = all_isolated && isolated;
        detail += "D" + std::to_string(bit) + ": portb_in=" + hex2(pattern) +
                   " PRB=" + hex2(got) + (isolated ? " (isolated) " : " (NOT isolated) ");
        prev = got;
    }
    return all_isolated;
}

// ---------------------------------------------------------------------
// PORTB-DIRECTION: each of D0-D7's own DIR column reads I/O (bidirectional)
// -- a value written to CIAAPRB at that bit position, with the direction
// bit set to output, must be what a subsequent read of that register
// returns, both 0 and 1. Graded with every PB line released beforehand
// (portb_in = 0xFF, the idle state p.354 gives a line nothing drives),
// so an outside line held low is not the reason a written value fails to
// read back -- PORTB-RELEASED-CONTROL below is the honest positive check
// that this same released state, on these same registers, can read 1.
// Swept across all eight bit positions; MUT_DIRECTION_TARGET_A retargets
// the same comparator at RS_PRA/RS_DDRA, cycling between the two
// positions (bit 0, bit 1) that genuinely round-trip through a PRA read,
// as the substitution proof.
// ---------------------------------------------------------------------
bool bit_output_follows_direction(Harness& h, uint8_t rs_data, uint8_t rs_ddr, int bit) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);
    h.write_reg(rs_ddr, mask);
    h.write_reg(rs_data, 0x00);
    const bool tracks_zero = (h.read_reg(rs_data) & mask) == 0x00;
    h.write_reg(rs_data, mask);
    const bool tracks_one = (h.read_reg(rs_data) & mask) == mask;
    return tracks_zero && tracks_one;
}

bool portb_direction(Harness& h, std::string& detail) {
    bool all_writable = true;
    h.dut->portb_in = 0xFF;
    for (int i = 0; i < 4; ++i) h.step();
    for (int bit = 0; bit < 8; ++bit) {
        bool writable;
        if (MUT_DIRECTION_TARGET_A) {
            const int real_bit = bit % 2;  // only bits 0,1 of PRA round-trip
            writable = bit_output_follows_direction(h, RS_PRA, RS_DDRA, real_bit);
            detail += "D" + std::to_string(bit) + " (retargeted at PRA/DDRA bit " +
                       std::to_string(real_bit) + "): writable=" + (writable ? "yes " : "no ");
        } else {
            writable = bit_output_follows_direction(h, RS_PRB, RS_DDRB, bit);
            detail += "D" + std::to_string(bit) + ": writable=" + (writable ? "yes " : "no ");
        }
        all_writable = all_writable && writable;
    }
    return all_writable;
}

// ---------------------------------------------------------------------
// PORTB-RELEASED-CONTROL: an honest positive control for PORTB-DIRECTION's
// released-line fixture, on the same registers and protocol -- not one of
// this package's own claims, so it carries no uid. With every PB bit's
// direction left at input (DDRB = 0x00) and the lines released the same
// way PORTB-DIRECTION releases them (portb_in = 0xFF), a read of CIAAPRB
// must return the released lines' 1 at every position. This shows the
// released fixture and the read path both work on this basis, so
// PORTB-DIRECTION's red is not an unreleased or broken fixture.
// MUT_CONTROL_HELD_LOW holds the same lines low instead
// (portb_in = 0x00), the opposite input p.354 predicts a 0 read for, as
// the sensitivity proof.
// ---------------------------------------------------------------------
bool portb_released_control(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRB, 0x00);
    const uint8_t portb_in = MUT_CONTROL_HELD_LOW ? 0x00 : 0xFF;
    h.dut->portb_in = portb_in;
    for (int i = 0; i < 4; ++i) h.step();
    const uint8_t got = h.read_reg(RS_PRB);
    detail = "DDRB=0x00 (input), portb_in=" + hex2(portb_in) + ", PRB=" + hex2(got);
    // Fixed at 0xFF, the released-line reading p.341 and p.354 predict --
    // not at portb_in's own value -- so MUT_CONTROL_HELD_LOW's un-released
    // stimulus is graded against the same prediction the baseline is, and
    // fails it, instead of vacuously comparing the stimulus to itself.
    return got == 0xFF;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    { Harness h; checks.expect(fixture_live(h),
        "FIXTURE-LIVE: two distinct, known DDRB bytes (0xA5, 0x5A) both "
        "arrive and read back correctly and distinctly before any "
        "data-dependent assertion is graded [no page -- infrastructure]"); }

    {
        Harness h; std::string d;
        const bool r = portb_width(h, d);
        checks.expect(r, "PORTB-WIDTH: D0-D7 comprise eight "
            "independently-addressable bit positions on ciaa's port B -- "
            "toggling portb_in at one position moves only that PRB bit, "
            "not its neighbours, at each of the eight named lines "
            "[p.324, PARALLEL PORT (J8), figure -- D0-D7 rows and their "
            "bracketed 'D0-D7 comprise an eight-bit bidirectional bus' "
            "note] sol:E-parallel.c69 sol:E-parallel.c70 sol:E-parallel.c71 "
            "sol:E-parallel.c72 sol:E-parallel.c73 sol:E-parallel.c74 "
            "sol:E-parallel.c75 sol:E-parallel.c76 opus:E-parallel-iface.c10");
        checks.note("PORTB-WIDTH: " + d);
    }

    {
        Harness h; std::string d;
        const bool r = portb_direction(h, d);
        checks.expect(r, "PORTB-DIRECTION: each of D0-D7's own DIR column "
            "reads I/O -- with its direction bit set to output, a value "
            "written to CIAAPRB at that bit position must be what a "
            "subsequent read of that register returns, both 0 and 1, at "
            "every one of the eight named lines, with every PB line "
            "released beforehand (p.354's idle level) -- the same "
            "eight-position sweep hrm/ch8-parallel-port-direction's PROP-1 "
            "grades against p.259 "
            "[p.324, PARALLEL PORT (J8), figure -- D0-D7 rows' DIR=I/O "
            "column] sol:E-parallel.c69 sol:E-parallel.c70 "
            "sol:E-parallel.c71 sol:E-parallel.c72 sol:E-parallel.c73 "
            "sol:E-parallel.c74 sol:E-parallel.c75 sol:E-parallel.c76 "
            "opus:E-parallel-iface.c10");
        checks.note("PORTB-DIRECTION: " + d);
    }

    {
        Harness h; std::string d;
        const bool r = portb_released_control(h, d);
        checks.expect(r, "PORTB-RELEASED-CONTROL: with every PB bit's "
            "direction left at input and the lines released the same way "
            "PORTB-DIRECTION releases them, a read of CIAAPRB must return "
            "the released lines' 1 at every position -- an honest control "
            "for PORTB-DIRECTION's released-line fixture, on the same "
            "registers and protocol; grades no claim of this package's own "
            "[p.341, 'you read the actual current state of the I/O pins' / "
            "'the corresponding PR bit is defined as an input'; p.354, 'a "
            "read of the ports will return all highs because of passive "
            "pull-ups']");
        checks.note("PORTB-RELEASED-CONTROL: " + d);
    }

    // ------------------------------------------------------------------
    // Declined claims (2 total): the identical bidirectionality proposition
    // p.259 already states and hrm/ch8-parallel-port-direction's PROP-1
    // already grades (FAIL) at this same ciaa boundary. No uid above
    // appears on this line.
    // ------------------------------------------------------------------
    checks.record(
        "sol:E-parallel.c66 opus:E-parallel-iface.c5 (p.324 prose, 'This "
        "interface may also be used for input' / 'The parallel interface "
        "may also be used for input or bidirectional data transfers'): "
        "the identical proposition printed p.259's 'the general-purpose "
        "parallel interface is bi-directional' already states -- "
        "hrm/ch8-parallel-port-direction's PROP-1 (opus:8-par.c1, "
        "sol:8-par.c1) already grades this at this same ciaa boundary and "
        "found it FAIL. Not graded here as a duplicate.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
