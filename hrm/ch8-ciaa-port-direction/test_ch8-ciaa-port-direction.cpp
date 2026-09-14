// Test purpose: assert the general CIAADDRA/CIAAPRA data-direction
//   mechanism described on printed p.241 -- that CIAADDRA is a one-to-one,
//   per-bit-position direction mask over CIAAPRA, and that setting a
//   direction bit to 1 makes the corresponding data bit an output whose
//   read-back follows what was written -- at the `ciaa` module boundary,
//   compiled alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 228 and 241; committed specification 7345496 (SPECIFICATION.md,
//   this package).
// Requirement IDs: PROP-1, PROP-2, per SPECIFICATION.md's claim inventory.
//   Claims file: hrm-testpipeline/work/ch8-s1-build/ch8-ciaa-port-direction.jsonl
//   (10 claims; 3 scored across PROP-1/PROP-2, 7 declined -- see RECORDED
//   lines in main() and SPECIFICATION.md's "Claims not assigned an
//   assertion").
// Non-goals / scope carve-outs, per the build task and SPECIFICATION.md:
//   - the fire lines themselves (DB-9 pin 6, page 228 Table 8-1; "the
//     joystick fire buttons", page 241) belong to sibling package
//     `ch8-ciaapra-fire-buttons` -- declined here, not graded.
//   - where CIAADDRA/CIAAPRA answer on the bus ($BFE201/$BFE001) belongs to
//     sibling package `ch8-ciaapra-address-window` ("a rig over ciaa +
//     gary") -- `ciaa` alone has no address decoder, so this is out of
//     scope on both ownership and boundary grounds.
//   - PA1 (CIA-A PRA bit 1, the red-LED/audio-filter bit) already has an
//     independent register-level assertion in package
//     `ch5-audio-filter-bypass` (confirmed by grepping every Makefile in
//     this suite for `TOP := ciaa` -- it is the only other package testing
//     this fixture). None of this package's 10 claims says anything about
//     PA1's identity, so there is nothing to decline for it -- but to avoid
//     duplicating that package's own measurement of the same bit under a
//     different label, PROP-1/PROP-2 below are built on PA0 instead, using
//     PA2 (a bit with no output driver at all in this implementation, and
//     not claimed by any package) only as a neutral "different position"
//     stimulus for PROP-1's positional-independence check -- nothing is
//     asserted about what PA2 means or does.
//
// RTL read for step 2, after SPECIFICATION.md was committed (7345496): read
//   ciaa.v (388 lines) in full, plus its four instantiated submodules
//   (cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v -- needed only for
//   Verilator to elaborate the top module, not opened/read for content).
//   Also read, per the build brief's "read a sibling's Makefile for the
//   build pattern only after the spec is committed, and say so if you read
//   anything else in one": hrm/ch8-potgo-register-bit-layout/Makefile and,
//   beyond the Makefile itself, that package's full test .cpp (Checks
//   class, main()-layout, and MUTATION-comment convention -- suite coding
//   convention, not RTL content, and it is the class the top-level build
//   brief itself mandates character-identical); and, because it is the only
//   other package in the suite that already builds this exact fixture
//   (`ciaa`), hrm/ch5-audio-filter-bypass/Makefile and its full test .cpp
//   (Harness/write_reg/read_reg/porta_out_now/tick bus-timing protocol for
//   ciaa.v specifically -- reused verbatim below as verified RTL-interface
//   plumbing, not as a source of this package's own assertions or
//   expected values, which come only from SPECIFICATION.md).
//
// RTL facts established by that reading (none of this shaped
//   SPECIFICATION.md, which was committed first):
//   - ciaa.v:82-104 port list: rs[3:0] selects the register (rs=0 -> PRA,
//     rs=2 -> DDRA, ciaa.v:136-150), never by name/address.
//   - ciaa.v:222-238: writing PRA (rs=0) latches
//     regporta[3:0] <= {data_in[7:6], data_in[1:0]} -- i.e. only PA7, PA6,
//     PA1 and PA0 have a writable data-output register at all; PA5-PA2 have
//     none.
//   - ciaa.v:242-248: writing DDRA (rs=2) latches ddrporta[7:0] <=
//     data_in[7:0] in full (all 8 bits stored, even though only 4 have any
//     observable effect below).
//   - ciaa.v:262: porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]),
//     (~ddrporta[1:0] | regporta[1:0])} -- porta_out[0] is PA0's driven
//     value: pulled to 1 when ddrporta[0]=0 (input), equal to regporta[0]
//     (the last PRA write) when ddrporta[0]=1 (output). PA2's direction bit
//     (ddrporta[2]) drives nothing in this expression at all.
//   - ciaa.v:250-259 (pa_out, PRA's own read mux): reading PRA returns
//     {porta_in2[7:2], porta_out[1:0]} -- bits 1 and 0 of a PRA bus-read
//     come directly from porta_out[1:0], unconditionally, so a software
//     read of PRA bit 0 is exactly what PROP-1/PROP-2 need to observe.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild) and the FAIL/PASS lines quoted are what the run
// printed, not predictions; see this package's build report for the actual
// pass/fail counts at each step.
//
// MUTATION FIXTURE-LIVE stimulus red: the second DDRA write changed from
//   the distinct sentinel 0x5A to a repeat of the first (0xA5) -> FAIL,
//   second reading no longer differs from the first (both 0xA5). Reverted.
// MUTATION PROP-1 stimulus red: the "different position" stimulus in the
//   positional-independence step changed from DDRA=0x04 (bit 2 only) to
//   DDRA=0x05 (bit 2 AND bit 0), leaving the expected value (PRA bit 0
//   reads 1) unchanged -> FAIL, PRA bit 0 now reads 0, because bit 0 itself
//   was also switched to output by the mutated stimulus. Reverted.
// MUTATION PROP-2 stimulus red: the DDRA write that sets bit 0 to output
//   (0x01) was omitted, leaving ddrporta at its reset default (0/input),
//   while the same PRA bit-0 write/read sequence ran unchanged -> FAIL, PRA
//   bit 0 reads 1 (pulled), not the expected 0. Reverted.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

namespace {

constexpr uint8_t RS_PRA = 0x0;
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

// Bus-timing protocol for ciaa.v, reused from hrm/ch5-audio-filter-bypass's
// own Harness (the only other package in this suite that builds this exact
// fixture): regporta/ddrporta are posedge-clk, clk7_en-gated (ciaa.v:227-
// 248), so a write needs one tick to latch; pa_out and porta_out are
// combinational (ciaa.v:250-262) and need only eval().
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

    uint8_t porta_out_now() {
        dut->eval();
        return dut->porta_out;
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
// FIXTURE-LIVE: `ciaa` has never been measured by this package before.
// Two distinct, known bytes must both arrive at DDRA and read back
// correctly and distinctly before anything PROP-1/PROP-2 depend on is
// trusted.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h, bool repeat_first_word_bug = false) {
    h.write_reg(RS_DDRA, 0xA5);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, repeat_first_word_bug ? 0xA5 : 0x5A);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0xA5 && r2 == 0x5A && r1 != r2;
}

// ---------------------------------------------------------------------
// PROP-1: CIAADDRA's direction mask corresponds one-to-one, by bit
// position, with CIAAPRA -- not to a different position, not as a single
// shared switch. Tested on PA0 (bit 0), using PA2 (bit 2, no output driver
// in this implementation, unclaimed by any package) only as a neutral
// "different position" stimulus.
// ---------------------------------------------------------------------
bool prop1_positional_correspondence(Harness& h, bool cross_bit_bug = false) {
    // (a) baseline: bit 0 left as input (DDRA=0x00), PRA bit 0 written 0 ->
    // pulled high, reads 1.
    h.write_reg(RS_DDRA, 0x00);
    h.write_reg(RS_PRA, 0x00);
    const bool baseline_ok = (h.read_reg(RS_PRA) & 0x01) == 0x01;

    // (b) a DIFFERENT position (bit 2) switched to output, bit 0 left as
    // input, same PRA bit-0 write (0) -> bit 0 must still read 1: bit 2's
    // own direction bit must not govern bit 0.
    h.write_reg(RS_DDRA, cross_bit_bug ? 0x05 : 0x04);
    h.write_reg(RS_PRA, 0x00);
    const bool other_position_no_crosstalk = (h.read_reg(RS_PRA) & 0x01) == 0x01;

    // (c) bit 0 itself switched to output, same PRA bit-0 write (0) -> now
    // bit 0 must read 0: it is bit 0's OWN direction bit, not any other
    // bit's, that governs bit 0.
    h.write_reg(RS_DDRA, 0x01);
    h.write_reg(RS_PRA, 0x00);
    const bool own_position_governs = (h.read_reg(RS_PRA) & 0x01) == 0x00;

    return baseline_ok && other_position_no_crosstalk && own_position_governs;
}

// ---------------------------------------------------------------------
// PROP-2: setting a CIAADDRA direction bit to 1 makes the corresponding
// CIAAPRA bit an output -- its read-back follows what was last written to
// CIAAPRA, in both directions (0 and 1), not just one. Tested on PA0.
// ---------------------------------------------------------------------
bool prop2_direction_bit_makes_output(Harness& h, bool omit_direction_bug = false) {
    h.write_reg(RS_DDRA, omit_direction_bug ? 0x00 : 0x01);
    h.write_reg(RS_PRA, 0x00);
    const bool tracks_zero = (h.read_reg(RS_PRA) & 0x01) == 0x00;
    h.write_reg(RS_PRA, 0x01);
    const bool tracks_one = (h.read_reg(RS_PRA) & 0x01) == 0x01;
    return tracks_zero && tracks_one;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h),
            "FIXTURE-LIVE: two distinct, known DDRA bytes (0xA5, 0x5A) both "
            "arrive and read back correctly and distinctly before any "
            "data-dependent assertion is graded [no page -- infrastructure]");
    }

    { Harness h; checks.expect(prop1_positional_correspondence(h),
        "opus:8-dio.c44 PROP-1: CIAADDRA's direction mask corresponds "
        "one-to-one, by bit position, with CIAAPRA -- with bit 0 held as "
        "input, switching a DIFFERENT bit (bit 2) to output leaves bit 0's "
        "read-back unaffected (still pulled to 1); switching bit 0's OWN "
        "direction bit to output makes the identical write (0) actually "
        "take effect on bit 0 (reads 0) -- it is each bit's own direction "
        "position that governs it, not any other bit's, and not a shared "
        "switch [p.241 \"CIAADDRA ($BFE201) contains a mask that "
        "corresponds one-to-one with the data read register, CIAAPRA "
        "($BFE001)\"]"); }

    { Harness h; checks.expect(prop2_direction_bit_makes_output(h),
        "opus:8-dio.c46 sol:8-dio.c43 PROP-2: setting a 1 in CIAADDRA's bit "
        "0 makes CIAAPRA bit 0 an output -- its read-back follows what was "
        "just written to CIAAPRA bit 0, both 0->0 and 1->1, not stuck at "
        "either value [p.241 \"Setting a 1 in the direction position makes "
        "the corresponding bit an output\"]"); }

    // ------------------------------------------------------------------
    // Declined claims. Each is a graded proposition this package does not
    // score, for the reason given -- per the build task's carry-forward
    // instructions, these belong to sibling packages and are recorded, not
    // graded, here. No uid below appears on a graded (expect()) line.
    // ------------------------------------------------------------------
    checks.record(
        "opus:8-tbl8-1.c36 (p.228, Table 8-1's pin-6 footnote, restated to "
        "pin 6 specifically): pin 6 is listed on that same table as "
        "\"Button 1\" on a joystick -- the fire button. Belongs to "
        "ch8-ciaapra-fire-buttons; not graded here.");
    checks.record(
        "sol:8-pin.c40 (p.228, twin of opus:8-tbl8-1.c36, same pin-6/fire "
        "content): belongs to ch8-ciaapra-fire-buttons; not graded here.");
    checks.record(
        "opus:8-dio.c42 (p.241, \"The joystick fire buttons can also be "
        "configured as outputs\" -- names the fire buttons directly): "
        "belongs to ch8-ciaapra-fire-buttons; not graded here.");
    checks.record(
        "sol:8-dio.c41 (p.241, twin of opus:8-dio.c42, same fire-buttons "
        "topic sentence): belongs to ch8-ciaapra-fire-buttons; not graded "
        "here.");
    checks.record(
        "opus:8-dio.c43 (p.241, \"CIAADDRA is at address $BFE201\"): names "
        "where CIAADDRA answers on the bus. Belongs to "
        "ch8-ciaapra-address-window (\"a rig over ciaa + gary\"); `ciaa` "
        "compiled alone has no address decoder, so this is out of scope on "
        "boundary grounds too. Not graded here.");
    checks.record(
        "opus:8-dio.c45 (p.241, \"CIAAPRA ($BFE001) is the data read "
        "register of that pair\" -- restatement identifies the register by "
        "its bus address): belongs to ch8-ciaapra-address-window for the "
        "same reason as opus:8-dio.c43. Not graded here.");
    checks.record(
        "sol:8-dio.c42 (p.241, composite restatement carrying both "
        "CIAADDRA's and CIAAPRA's bus addresses plus the one-to-one "
        "correspondence fact): the address content belongs to "
        "ch8-ciaapra-address-window; the correspondence content is not "
        "lost -- it is the same fact opus:8-dio.c44 states, already scored "
        "as PROP-1 above. Not graded here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
