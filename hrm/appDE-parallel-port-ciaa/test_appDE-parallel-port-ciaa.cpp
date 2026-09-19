// Test purpose: p.324's `PARALLEL PORT (J8)` table (Appendix E, read from
//   `figures/p324.png` per this lot's build brief -- the NOTES column is
//   displaced against NAME in the text layer on this page) and its
//   surrounding prose, restricted to the D0-D7 data-bus rows and the "may
//   also be used for input"/"eight-bit bidirectional bus" clauses -- at the
//   `ciaa` module boundary, compiled alone.
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
// RTL read for step 2: ciaa.v (388 lines) in full; cia_int.v, cia_timera.v,
//   cia_timerb.v, cia_timerd.v (needed only for Verilator to elaborate the
//   top module). Also read in full: hrm/ch8-parallel-port-direction's
//   Makefile and test_ch8-parallel-port-direction.cpp (its Harness/
//   write_reg/read_reg/tick bus-timing protocol and its
//   bit_output_follows_direction() comparator -- reused below verbatim as
//   verified RTL-interface plumbing, not as a source of this package's own
//   assertions or expected values) -- that package's own header discloses
//   the same ciaa.v line numbers this package independently re-confirms
//   below, plus minimig.v:136 (a stale change-log comment) and minimig.v
//   :607-651 (CIAA1/CIAB1 instantiation), read there and not re-read here.
//   This package additionally read minimig.v:202-203,624 for the portb_in
//   source (see below), a fact hrm/ch8-parallel-port-direction's own header
//   does not carry.
//
// RTL facts established by that reading:
//   - ciaa.v:97, `input [7:0] portb_in` -- eight raw external input pins,
//     synchronisation-free (unlike porta_in, which IS latched through
//     porta_in2 at :227-230; portb_in is read live, ciaa.v:293-294).
//   - ciaa.v:114, `wire [7:0] portb_out;` -- declared, and appearing
//     NOWHERE else in the file (confirmed by grep: one occurrence, the
//     declaration itself). Not assigned, not read, not even exposed as a
//     module output port (ciaa's own port list, :80-104, declares `output
//     [3:0] porta_out` but no portb_out output at all) -- a dead wire in
//     the fullest sense: it cannot reach outside this module even in
//     principle.
//   - ciaa.v:267 `//reg [7:0] regportb;` and :270-279 (its entire write
//     block) are commented out. No register in this design ever catches a
//     write to CIAAPRB, at any bit position.
//   - ciaa.v:291-299 (pb_out, PRB/DDRB's shared read mux): a CIAAPRB read
//     (`!wr && prb`, :293-294) returns `portb_in[7:0]` UNCONDITIONALLY --
//     ddrportb does not appear in that branch at all. ddrportb itself is
//     real (written :282-288, read back at its own address :295-296) but
//     governs nothing about what a CIAAPRB read returns.
//   - minimig.v:624, `.portb_in({_joy4[0],_joy4[1],_joy4[2],_joy4[3],
//     _joy3[0],_joy3[1],_joy3[2],_joy3[3]})` -- ciaa's port B is wired as a
//     four-bit-each joystick-3/joystick-4 adapter in this design, not to
//     any parallel/Centronics connector. minimig.v is not compiled by this
//     package and this fact plays no role in either assertion below (both
//     grade ciaa's own boundary, where portb_in is this fixture's own
//     free input); it is recorded because the build brief named it as
//     established and not to be rediscovered, and because it explains WHY
//     ciaa's port B behaves the way it does rather than just THAT it does.
//   - hrm/ch8-parallel-port-direction's own PROP-1 (opus:8-par.c1,
//     sol:8-par.c1, printed p.259 "the general-purpose parallel interface
//     is bi-directional") already grades this same write-path absence at
//     ciaa's boundary, at bit 0 of PRB/DDRB, and its own committed result
//     is FAIL, with the finding stated in its own words as holding "at any
//     bit position" -- i.e. one shared root cause (regportb's entire
//     absence), not eight independent per-bit mechanisms that could read
//     differently from each other.
//   - ciaa.v:262, porta_out[3:0] = {~ddrporta[7:6]|regporta[3:2],
//     ~ddrporta[1:0]|regporta[1:0]}, with regporta[3:0] <= {data_in[7:6],
//     data_in[1:0]} on a PRA write (:238) -- a REAL, round-trip-through-
//     the-same-register write/read path exists at PRA/DDRA bit positions 0
//     and 1 (pa_out[1:0] = porta_out[1:0] on a PRA read, :254), already
//     the shape hrm/ch8-parallel-port-direction's own CONTROL-PA0-OUTPUT
//     control used at bit 0. Bits 2-5 of a PRA read return porta_in2
//     (external input) regardless of direction; bits 6-7 drive real output
//     pins (porta_out[3:2]) but that output is not visible through a PRA
//     read (pa_out[7:2] = porta_in2[7:2] always) -- so only bits 0 and 1
//     round-trip through the SAME register a write targeted, the two
//     positions this package's PORTB-DIRECTION mutation below reuses.
//
// Claim disposition (worked out from classify.note, independently
//   confirmed against the RTL above, not taken on faith):
//   - sol:E-parallel.c66, opus:E-parallel-iface.c5 (p.324 prose, "This
//     interface may also be used for input"/"may also be used for input or
//     bidirectional data transfers"): the identical proposition p.259's
//     "bi-directional" already states, and hrm/ch8-parallel-port-direction's
//     PROP-1 already grades it (FAIL) at this same ciaa boundary. Declined.
//   - sol:E-parallel.c69..c76 (figure rows D0..D7) and opus:E-parallel-
//     iface.c10 (figure prose "D0-D7 comprise an eight-bit bidirectional
//     bus"): each restates BOTH that its named line is one of eight
//     independently-addressable positions (D0..D7 -- a WIDTH claim PROP-1
//     never tested; PROP-1 checked only bit 0) AND that each line's own DIR
//     column reads I/O (a DIRECTION claim). classify.note calls this "the
//     width-and-per-bit-direction half" and instructs grading it, distinct
//     from the plain-bidirectionality prose above. Graded here as TWO
//     assertions (PORTB-WIDTH, PORTB-DIRECTION), both carrying all nine
//     uids, because the two component claims (width; direction) measure
//     independently and forcing them into one AND would hide which half is
//     true. PORTB-WIDTH is TRUE (measured). PORTB-DIRECTION is FALSE at
//     every one of the eight positions (measured), for the single shared
//     reason PROP-1 already names, now confirmed individually at bits
//     1-7, which PROP-1 itself never exercised.
//
// Every mutation below was actually built (WSL Verilator 5.052, `make
// MUT=<NAME> test`, full `rm -rf obj_dir` rebuild via the Makefile's `test`
// recipe) and the PASS/FAIL/pass-count lines quoted are what that run
// printed, not predictions. All mutations were reverted afterwards; this
// file builds at MUT unset (baseline).
//
// Baseline (no MUT): "2 passed, 1 failed" -- FIXTURE-LIVE PASS,
//   PORTB-WIDTH PASS, PORTB-DIRECTION FAIL (expected and by design, see
//   above -- CIAA's port B has no write path at any bit position).
//
// MUTATION FIXTURE-LIVE stimulus red: MUT_FIXLIVE repeats the first DDRB
//   byte (0xA5) instead of a distinct second one -> FIXTURE-LIVE FAIL,
//   "1 passed, 2 failed". Reverted.
// MUTATION PORTB-WIDTH stimulus red: MUT_WIDTH_SAMEPATTERN presents the
//   SAME portb_in byte (0x01) at every swept bit position instead of a
//   distinct single-bit pattern per position -> PORTB-WIDTH FAIL,
//   "1 passed, 2 failed" (all eight reads now equal, isolation cannot be
//   shown). PORTB-DIRECTION is unaffected (still FAIL, for its own
//   independent reason). Reverted.
// MUTATION PORTB-DIRECTION stimulus green: MUT_DIRECTION_TARGET_A
//   retargets the whole eight-position sweep at RS_PRA/RS_DDRA, cycling
//   bit%2 across the two positions (0, 1) that genuinely round-trip
//   through a PRA read (see RTL facts above) instead of RS_PRB/RS_DDRB --
//   a real substitution, reusing hrm/ch8-parallel-port-direction's own
//   validated bit_output_follows_direction() unchanged -> PORTB-DIRECTION
//   PASS, "3 passed, 0 failed". Substitution proof: the comparator is not
//   hardwired false, it returns true for the register pair where the
//   mechanism actually works, on the same fixture and protocol.
//   PORTB-WIDTH is unaffected (still PASS; it never reads PRA/DDRA).
//   Reverted.

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

// Bus-timing protocol for ciaa.v, reused verbatim from
// hrm/ch8-parallel-port-direction's own Harness (this exact fixture, same
// top module): regporta/ddrporta/ddrportb are posedge-clk, clk7_en-gated,
// so a write needs one tick to latch; pa_out/pb_out/porta_out are
// combinational and need only eval().
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
// returns, both 0 and 1. Swept across all eight bit positions; MUT_
// DIRECTION_TARGET_A retargets the same comparator at RS_PRA/RS_DDRA,
// cycling between the two positions (bit 0, bit 1) that genuinely
// round-trip through a PRA read, as the substitution proof.
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
            "every one of the eight named lines. False at all eight: "
            "CIAAPRB's write path (regportb) is entirely commented out "
            "(ciaa.v:267,:270-279), so a CIAAPRB read always echoes the "
            "raw external portb_in pins (ciaa.v:293-294), never what was "
            "written, at any bit position -- the same finding "
            "hrm/ch8-parallel-port-direction's PROP-1 already reports at "
            "bit 0 alone, confirmed here individually at bits 1-7 too, "
            "which PROP-1 never exercised "
            "[p.324, PARALLEL PORT (J8), figure -- D0-D7 rows' DIR=I/O "
            "column] sol:E-parallel.c69 sol:E-parallel.c70 "
            "sol:E-parallel.c71 sol:E-parallel.c72 sol:E-parallel.c73 "
            "sol:E-parallel.c74 sol:E-parallel.c75 sol:E-parallel.c76 "
            "opus:E-parallel-iface.c10");
        checks.note("PORTB-DIRECTION: " + d);
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
        "found it FAIL (regportb's write path is entirely commented out). "
        "Not graded here as a duplicate.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
