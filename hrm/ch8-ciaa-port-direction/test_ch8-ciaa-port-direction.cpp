// Test purpose: assert the general CIAADDRA/CIAAPRA data-direction mechanism
//   described on printed p.241 -- that CIAADDRA is a one-to-one,
//   per-bit-position direction mask over CIAAPRA, and that setting a
//   direction bit to 1 makes the corresponding data bit an output whose
//   read-back follows what was written -- at the `ciaa` module boundary,
//   compiled alone. PROP-1 and PROP-2 are graded across all eight bit
//   positions (PA0-PA7): the printed claim is byte-wide ("a mask that
//   corresponds one-to-one ... makes the corresponding bit an output"), not
//   a claim about one bit, so the assertion carrying these uids must
//   exercise all eight positions, not one.
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
//   - the fire lines' OWN claims (DB-9 pin 6, page 228 Table 8-1; "the
//     joystick fire buttons", page 241) belong to sibling package
//     `ch8-ciaapra-fire-buttons` -- declined here, not graded. PA6/PA7 (the
//     fire-line bit positions) are still exercised below as two of the
//     eight PROP-1/PROP-2 data points: that is a different claim (the
//     general mask correspondence, byte-wide) and does not re-grade the
//     fire-button claim under a different label.
//   - where CIAADDRA/CIAAPRA answer on the bus ($BFE201/$BFE001) belongs to
//     sibling package `ch8-ciaapra-address-window` ("a rig over ciaa +
//     gary") -- `ciaa` alone has no address decoder, so this is out of
//     scope on both ownership and boundary grounds.
//   - PA1 (CIA-A PRA bit 1, the red-LED/audio-filter bit) has an
//     independent register-level assertion in package
//     `ch5-audio-filter-bypass`, about a different claim (what PA1 means).
//     PA1 is still exercised below as one of the eight PROP-1/PROP-2 data
//     points -- omitting it would leave the eight-position claim untested
//     at that position, and this does not re-grade ch5-audio-filter-bypass's
//     own claim under a different label.
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim outside the 10 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 10 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s1-build/ch8-ciaa-port-direction.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
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
//     PA1 and PA0 have a writable data-output register bit at all; PA5-PA2
//     have none, and a write to those bit positions is simply dropped.
//   - ciaa.v:242-248: writing DDRA (rs=2) latches ddrporta[7:0] <=
//     data_in[7:0] in full (all 8 bits stored, even though only 4 have any
//     observable effect below).
//   - ciaa.v:262: porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]),
//     (~ddrporta[1:0] | regporta[1:0])} -- porta_out[0]/[1] are PA0's/PA1's
//     driven values; porta_out[2]/[3] are PA6's/PA7's. PA2-PA5's direction
//     bits (ddrporta[5:2]) drive nothing in this expression at all.
//   - ciaa.v:253-254 (pa_out, PRA's own read mux): reading PRA returns
//     {porta_in2[7:2], porta_out[1:0]} -- bits 7 down to 2 of a PRA read are
//     ALWAYS the raw synchronised input, unconditionally, regardless of
//     ddrporta; only bits 1 and 0 come from porta_out, the only pair wired
//     to honour ddrporta at this read mux.
//   - Consequence, confirmed by direct measurement below: a CIAAPRA read at
//     this fixture boundary shows write-follows-read ("output") behaviour
//     ONLY for PA0 and PA1. PA2-PA5 have no captured register bit at all
//     (dropped on a PRA write). PA6-PA7's write IS captured (into
//     regporta[3:2]) and DOES drive the porta_out port -- consumed
//     downstream by the fire-button wiring in minimig.v/userio.v, out of
//     scope here -- but pa_out never reads porta_out[3:2] back, so a
//     CIAAPRA read never reflects it for those two positions either. Two
//     different RTL reasons, one shared observable result: the one-to-one
//     mask/output claim fails for six of the eight printed bit positions
//     when graded against the register the manual names, "the data read
//     register, CIAAPRA".
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild) and the FAIL/PASS lines quoted are what the run
// printed, not predictions.
//
// MUTATION FIXTURE-LIVE stimulus red: the second DDRA write changed from
//   the distinct sentinel 0x5A to a repeat of the first (0xA5) -> FAIL,
//   second reading no longer differs from the first (both 0xA5). Reverted.
// MUTATION opus:8-dio.c44 stimulus green: PROP-1 is graded at baseline
//   across all eight bit positions and is red there (own-position-governs
//   fails for PA2-PA7). Already-red assertions cannot be mutated redder, so
//   this is a substitution proof: the identical bit_position_correspondence()
//   comparator PROP-1 sweeps above, instantiated at bit 0 alone instead of
//   swept across all eight (CONTROL-PA0-POSITIONAL below, PA0 proven live by
//   FIXTURE-LIVE) -> PASS, in the same run. The comparator is therefore not
//   vacuously false: it returns true for the one position where the
//   mechanism actually works.
// MUTATION opus:8-dio.c46 stimulus green: PROP-2 is graded at baseline
//   across all eight bit positions and is red there
//   (bit_output_follows_direction() fails for PA2-PA7). Substitution proof:
//   the identical comparator instantiated at bit 0 alone (CONTROL-PA0-OUTPUT
//   below) -> PASS, in the same run.
// MUTATION CONTROL-PA0-POSITIONAL stimulus red: the "different position"
//   stimulus changed from DDRA=0x04 (bit 2 only) to DDRA=0x05 (bit 2 AND
//   bit 0), leaving the expected value unchanged -> FAIL, PRA bit 0 now
//   reads 0, because bit 0 itself was also switched to output by the
//   mutated stimulus. Reverted.
// MUTATION CONTROL-PA0-OUTPUT stimulus red: the DDRA write that sets bit 0
//   to output (0x01) was omitted, leaving ddrporta at its reset default
//   (0/input), while the same PRA bit-0 write/read sequence ran unchanged
//   -> FAIL, PRA bit 0 reads 1 (pulled), not the expected 0. Reverted.

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
// Shared per-bit comparators. PROP-1 and PROP-2 sweep these across all
// eight bit positions; CONTROL-PA0-POSITIONAL and CONTROL-PA0-OUTPUT call
// the SAME functions restricted to bit 0 -- the substitution proof for
// PROP-1/PROP-2's red result needs the identical comparator, not a
// reimplementation of it, put through a mechanism proven live.
// ---------------------------------------------------------------------

// One-to-one positional correspondence at a single bit position: switching
// a DIFFERENT bit's direction to output must not move this position's
// read-back away from its own input-state baseline; switching THIS
// position's own direction to output, then writing the opposite of that
// baseline, must move the read-back to that opposite value. Baseline is
// measured, not assumed to be 1: this implementation only models an input
// pull-up on bits 0/1 (ciaa.v:262's ~ddrporta term), so other positions
// read the harness's fixed porta_in=0 as their input-state baseline
// instead of a pulled-high 1.
bool bit_position_correspondence(Harness& h, int bit, int other_bit,
                                  bool cross_bit_bug = false) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);
    const uint8_t other_mask = static_cast<uint8_t>(1u << other_bit);

    // (a) baseline: bit under test left as input.
    h.write_reg(RS_DDRA, 0x00);
    h.write_reg(RS_PRA, 0x00);
    const uint8_t baseline = h.read_reg(RS_PRA) & mask;

    // (b) a DIFFERENT position's direction switched to output, bit under
    // test still input, same PRA write (0) -> its reading must stay at
    // baseline: another bit's direction bit must not govern it.
    h.write_reg(RS_DDRA, cross_bit_bug
        ? static_cast<uint8_t>(other_mask | mask) : other_mask);
    h.write_reg(RS_PRA, 0x00);
    const bool no_crosstalk = (h.read_reg(RS_PRA) & mask) == baseline;

    // (c) the bit's OWN direction switched to output, PRA written to the
    // opposite of baseline -> the reading must now show that opposite
    // value: it is this bit's own direction bit, and only its own, that
    // governs it.
    const uint8_t target = baseline ? uint8_t{0x00} : mask;
    h.write_reg(RS_DDRA, mask);
    h.write_reg(RS_PRA, target);
    const bool own_governs = (h.read_reg(RS_PRA) & mask) == target;

    return no_crosstalk && own_governs;
}

// Output behaviour at a single bit position: with that position's own
// direction bit set to 1, its CIAAPRA read-back must follow what was just
// written to CIAAPRA at that position, both 0->0 and 1->1, not stuck at
// either value.
bool bit_output_follows_direction(Harness& h, int bit,
                                   bool omit_direction_bug = false) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);
    h.write_reg(RS_DDRA, omit_direction_bug ? uint8_t{0x00} : mask);
    h.write_reg(RS_PRA, 0x00);
    const bool tracks_zero = (h.read_reg(RS_PRA) & mask) == 0x00;
    h.write_reg(RS_PRA, mask);
    const bool tracks_one = (h.read_reg(RS_PRA) & mask) == mask;
    return tracks_zero && tracks_one;
}

// ---------------------------------------------------------------------
// PROP-1: CIAADDRA's direction mask corresponds one-to-one, by bit
// position, with CIAAPRA -- graded across all eight bit positions, PA0
// through PA7. p.241's claim is byte-wide; a test that exercises only one
// bit is not a test of it.
// ---------------------------------------------------------------------
bool prop1_universal_positional_correspondence(Harness& h, std::string& detail) {
    bool all_ok = true;
    for (int i = 0; i < 8; ++i) {
        const int other = (i + 1) % 8;
        const bool ok = bit_position_correspondence(h, i, other);
        detail += "PA" + std::to_string(i) + "=" + (ok ? "PASS " : "FAIL ");
        all_ok = all_ok && ok;
    }
    return all_ok;
}

// ---------------------------------------------------------------------
// PROP-2: setting a 1 in a CIAADDRA direction position makes the
// corresponding CIAAPRA bit an output -- graded across all eight bit
// positions, PA0 through PA7.
// ---------------------------------------------------------------------
bool prop2_universal_direction_makes_output(Harness& h, std::string& detail) {
    bool all_ok = true;
    for (int i = 0; i < 8; ++i) {
        const bool ok = bit_output_follows_direction(h, i);
        detail += "PA" + std::to_string(i) + "=" + (ok ? "PASS " : "FAIL ");
        all_ok = all_ok && ok;
    }
    return all_ok;
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

    {
        Harness h;
        std::string detail;
        const bool result = prop1_universal_positional_correspondence(h, detail);
        checks.expect(result,
            "opus:8-dio.c44 PROP-1: CIAADDRA's direction mask corresponds "
            "one-to-one, by bit position, with CIAAPRA -- graded across all "
            "eight bit positions (PA0-PA7): for each position, a DIFFERENT "
            "bit's direction setting must leave that position's read-back "
            "at its own input-state baseline, and that position's OWN "
            "direction bit, switched to output, must move its read-back to "
            "the written value. True only for PA0 and PA1; false for "
            "PA2-PA7, whose own direction bit never governs a CIAAPRA "
            "read-back at this fixture boundary -- the correspondence the "
            "page claims does not hold across the eight positions "
            "[p.241 \"CIAADDRA ($BFE201) contains a mask that corresponds "
            "one-to-one with the data read register, CIAAPRA ($BFE001)\"]");
        checks.note("PROP-1 per-bit positional correspondence: " + detail);
    }

    {
        Harness h;
        std::string detail;
        const bool result = prop2_universal_direction_makes_output(h, detail);
        checks.expect(result,
            "opus:8-dio.c46 sol:8-dio.c43 PROP-2: setting a 1 in CIAADDRA's "
            "direction position makes the corresponding CIAAPRA bit an "
            "output -- graded across all eight bit positions (PA0-PA7): "
            "each position's read-back must follow what was just written "
            "to CIAAPRA at that position, both 0->0 and 1->1, once its own "
            "direction bit is set to 1. True only for PA0 and PA1; false "
            "for PA2-PA7 -- PA2-PA5 have no output register bit captured on "
            "a PRA write at all, and PA6/PA7's write IS captured and does "
            "drive the porta_out port, but that value is never read back "
            "through a CIAAPRA read (pa_out uses only porta_out[1:0]), so a "
            "CPU read of CIAAPRA never reflects it for those two positions "
            "either [p.241 \"Setting a 1 in the direction position makes "
            "the corresponding bit an output\"]");
        checks.note("PROP-2 per-bit output-follows-direction: " + detail);
    }

    { Harness h; checks.expect(
        bit_position_correspondence(h, /*bit=*/0, /*other_bit=*/2),
        "CONTROL-PA0-POSITIONAL: the same bit_position_correspondence() "
        "comparator PROP-1 sweeps above, instantiated at bit 0 alone (PA2 "
        "as the neutral different-position stimulus) -- the one position "
        "PROP-1 shows this mechanism actually working for. Not itself a "
        "page claim: it is PROP-1's measurement-basis control and the "
        "substitution proof that PROP-1's comparator is not vacuously "
        "false [no page -- infrastructure control]"); }

    { Harness h; checks.expect(
        bit_output_follows_direction(h, /*bit=*/0),
        "CONTROL-PA0-OUTPUT: the same bit_output_follows_direction() "
        "comparator PROP-2 sweeps above, instantiated at bit 0 alone -- the "
        "one position PROP-2 shows this mechanism actually working for. "
        "Not itself a page claim: it is PROP-2's measurement-basis control "
        "and the substitution proof that PROP-2's comparator is not "
        "vacuously false [no page -- infrastructure control]"); }

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
        "lost -- it is the same fact already scored as PROP-1 above. Not "
        "graded here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
