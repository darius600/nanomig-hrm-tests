// Test purpose: assert the general CIAADDRA/CIAAPRA data-direction mechanism
//   described on printed p.241 and p.341 (Appendix F) -- that CIAADDRA is a
//   one-to-one, per-bit-position direction mask over CIAAPRA, and that
//   setting a direction bit to 1 makes the corresponding data bit an output
//   whose read-back is the pin the output drives -- at the `ciaa` module
//   boundary, compiled alone. PROP-1 and PROP-2 are graded across all eight
//   bit positions (PA0-PA7): the printed claim is byte-wide ("a mask that
//   corresponds one-to-one ... makes the corresponding bit an output"), not
//   a claim about one bit, so the assertion carrying these uids must
//   exercise all eight positions, not one.
// Fixture: all six external port-A lines are held released -- porta_in =
//   0x3F, the idle state p.354 describes ("a read of the ports will return
//   all highs because of passive pull-ups"). With nothing outside
//   contesting the line, a bit read back as 0 can only come from the port
//   driving it, so PROP-1 and PROP-2 grade the written-0 half p.341
//   predicts (an output driven 0 reads 0); a written 1 over a line held
//   low is a different case, one no page in this package's oracle set
//   decides, and this fixture does not present it.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 241, 341 and 354; committed specification 02448ac (SPECIFICATION.md,
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
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s1-build/ch8-ciaa-port-direction.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// What changed and why: PROP-1 and PROP-2 originally held the fixture's six
//   external port-A lines low (all switches closed / drives asserted, in
//   the manual's active-low terms) and then wrote a 1 over that held-low
//   line -- a case p.241, p.341 and p.354 together leave undecided (no page
//   says whether the CIA's own output or an outside pull wins that
//   contest). That made both assertions decide on the undecided case
//   instead of the one p.341 and p.354 do decide: an output driven 0 where
//   nothing outside pulls the line, and a released input reading high. The
//   fixture now releases all six lines (porta_in = 0x3F) so every
//   observation below grades only what the cited pages state.
//
// MUTATION FIXTURE-LIVE, CONTROL-PA0-POSITIONAL and CONTROL-PA0-OUTPUT below
// are runtime switches, guarded by mutating("<ID>") and read once from the
// MUTATE environment variable at startup, so one build serves the real run
// (MUTATE unset) and every mutation run (MUTATE=<ID>).
//
// MUTATION FIXTURE-LIVE stimulus red: the second DDRA write changed from
//   the distinct sentinel 0x5A to a repeat of the first (0xA5), so the
//   second reading no longer differs from the first (both 0xA5)
//   (MUTATE=FIXTURE-LIVE: FIXTURE-LIVE FAIL, 2 passed, 3 failed).
// MUTATION opus:8-dio.c44 stimulus green: PROP-1 is graded at baseline
//   across all eight bit positions and is red there (MEASURED: PA0=PASS
//   PA1=PASS PA2=FAIL PA3=FAIL PA4=FAIL PA5=FAIL PA6=FAIL PA7=FAIL).
//   Already-red assertions cannot be mutated redder, so this is a
//   substitution proof: the identical observe_position() comparator PROP-1
//   sweeps above, instantiated at bit 0 alone instead of swept across all
//   eight (CONTROL-PA0-POSITIONAL below, PA0 proven live by FIXTURE-LIVE)
//   reads PASS, in the same run (no switch: this control assertion is
//   unconditional baseline code). The comparator is therefore not
//   vacuously false: it returns true for the one position where p.241 and
//   p.341's prediction holds.
// MUTATION opus:8-dio.c46 stimulus green: PROP-2 is graded at baseline
//   across all eight bit positions and is red there (MEASURED: PA0=PASS
//   PA1=PASS PA2=FAIL PA3=FAIL PA4=FAIL PA5=FAIL PA6=FAIL PA7=FAIL).
//   Substitution proof: the identical observe_bit() comparator instantiated
//   at bit 0 alone (CONTROL-PA0-OUTPUT below) reads PASS, in the same run
//   (no switch: this control assertion is unconditional baseline code).
// MUTATION CONTROL-PA0-POSITIONAL stimulus red: the "different position"
//   stimulus changed from DDRA=0x04 (bit 2 only) to DDRA=0x05 (bit 2 AND
//   bit 0), leaving the expected value unchanged, so PRA bit 0 now reads 0
//   because bit 0 itself was also switched to output by the mutated
//   stimulus (MUTATE=CONTROL-PA0-POSITIONAL: CONTROL-PA0-POSITIONAL FAIL,
//   2 passed, 3 failed).
// MUTATION CONTROL-PA0-OUTPUT stimulus red: the DDRA write that sets bit 0
//   to output (0x01) was omitted, leaving ddrporta at its reset default
//   (0/input), while the same PRA bit-0 write/read sequence ran unchanged,
//   so PRA bit 0 reads 1 (pulled), not the expected 0
//   (MUTATE=CONTROL-PA0-OUTPUT: CONTROL-PA0-OUTPUT FAIL, 2 passed,
//   3 failed).

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

namespace {

const char* g_mutate = nullptr;
inline bool mutating(const std::string& id) {
    return g_mutate != nullptr && id == g_mutate;
}

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

// Bus-timing protocol for this fixture, reused from hrm/ch5-audio-filter-
// bypass's own Harness (the only other package in this suite that builds
// this exact fixture): a register write needs one tick to latch; a read is
// combinational and needs only eval().
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
        dut->porta_in = 0x3F;
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
// Shared per-bit observation. PROP-1 and PROP-2 sweep this across all
// eight bit positions; CONTROL-PA0-POSITIONAL and CONTROL-PA0-OUTPUT call
// the SAME underlying observation restricted to bit 0 -- the substitution
// proof for PROP-1/PROP-2's result needs the identical comparator, not a
// reimplementation of it, put through a mechanism proven live.
//
// The fixture releases all six external port-A lines (porta_in = 0x3F,
// the idle state p.354 describes: "a read of the ports will return all
// highs because of passive pull-ups"). With nothing outside contesting
// the line, three values at each bit position are graded against p.241
// and p.341:
//   - control (direction bit 0, PRA written 0): the released input,
//     expected 1;
//   - drive0 (direction bit 1, PRA written 0): the pin is what the
//     output drives (p.341's read-returns-the-pin rule) and nothing else
//     pulls it, expected 0;
//   - drive1 (direction bit still 1, PRA written 1): expected 1.
// Each observed value is carried out alongside the verdict so the run's
// own output shows the numbers p.241/p.341/p.354 predict, not just
// PASS/FAIL.
// ---------------------------------------------------------------------
struct BitObservation {
    bool ok;
    int control;  // observed CIAAPRA bit, DDRA bit = 0, PRA bit = 0
    int drive0;   // observed CIAAPRA bit, DDRA bit = 1, PRA bit = 0
    int drive1;   // observed CIAAPRA bit, DDRA bit = 1, PRA bit = 1
};

BitObservation observe_bit(Harness& h, int bit, bool omit_direction_bug = false) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);

    h.write_reg(RS_DDRA, 0x00);
    h.write_reg(RS_PRA, 0x00);
    const int control = (h.read_reg(RS_PRA) & mask) ? 1 : 0;

    h.write_reg(RS_DDRA, omit_direction_bug ? uint8_t{0x00} : mask);
    h.write_reg(RS_PRA, 0x00);
    const int drive0 = (h.read_reg(RS_PRA) & mask) ? 1 : 0;

    h.write_reg(RS_PRA, mask);
    const int drive1 = (h.read_reg(RS_PRA) & mask) ? 1 : 0;

    return BitObservation{control == 1 && drive0 == 0 && drive1 == 1,
                           control, drive0, drive1};
}

// Positional correspondence adds one more observation ahead of the same
// three: a DIFFERENT position's direction bit switched to output, this
// position's own direction bit still 0, same PRA write (0) -- this
// position's reading must stay at the released-input value, 1. Only this
// position's own direction bit may move it.
struct PositionObservation {
    bool ok;
    int control;
    int crosstalk;  // observed CIAAPRA bit, other position's DDRA bit = 1
    int drive0;
    int drive1;
};

PositionObservation observe_position(Harness& h, int bit, int other_bit,
                                      bool cross_bit_bug = false) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);
    const uint8_t other_mask = static_cast<uint8_t>(1u << other_bit);

    h.write_reg(RS_DDRA, 0x00);
    h.write_reg(RS_PRA, 0x00);
    const int control = (h.read_reg(RS_PRA) & mask) ? 1 : 0;

    h.write_reg(RS_DDRA, cross_bit_bug
        ? static_cast<uint8_t>(other_mask | mask) : other_mask);
    h.write_reg(RS_PRA, 0x00);
    const int crosstalk = (h.read_reg(RS_PRA) & mask) ? 1 : 0;

    h.write_reg(RS_DDRA, mask);
    h.write_reg(RS_PRA, 0x00);
    const int drive0 = (h.read_reg(RS_PRA) & mask) ? 1 : 0;
    h.write_reg(RS_PRA, mask);
    const int drive1 = (h.read_reg(RS_PRA) & mask) ? 1 : 0;

    return PositionObservation{
        control == 1 && crosstalk == 1 && drive0 == 0 && drive1 == 1,
        control, crosstalk, drive0, drive1};
}

// Thin bool wrappers: CONTROL-PA0-POSITIONAL and CONTROL-PA0-OUTPUT below
// call these directly, unchanged, so they keep calling the identical
// comparator PROP-1/PROP-2 use rather than a reimplementation of it.
bool bit_position_correspondence(Harness& h, int bit, int other_bit,
                                  bool cross_bit_bug = false) {
    return observe_position(h, bit, other_bit, cross_bit_bug).ok;
}

bool bit_output_follows_direction(Harness& h, int bit,
                                   bool omit_direction_bug = false) {
    return observe_bit(h, bit, omit_direction_bug).ok;
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
        const PositionObservation r = observe_position(h, i, other);
        detail += "PA" + std::to_string(i) + "=" + (r.ok ? "PASS" : "FAIL") +
            "(ctrl=" + std::to_string(r.control) +
            " cross=" + std::to_string(r.crosstalk) +
            " drv0=" + std::to_string(r.drive0) +
            " drv1=" + std::to_string(r.drive1) + ") ";
        all_ok = all_ok && r.ok;
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
        const BitObservation r = observe_bit(h, i);
        detail += "PA" + std::to_string(i) + "=" + (r.ok ? "PASS" : "FAIL") +
            "(ctrl=" + std::to_string(r.control) +
            " drv0=" + std::to_string(r.drive0) +
            " drv1=" + std::to_string(r.drive1) + ") ";
        all_ok = all_ok && r.ok;
    }
    return all_ok;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h, mutating("FIXTURE-LIVE")),
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
            "eight bit positions (PA0-PA7), fixture released (porta_in = "
            "0x3F, p.354's idle-high state): for each position, a DIFFERENT "
            "bit's direction setting must leave that position's read-back "
            "at the released-input value, 1; and that position's OWN "
            "direction bit, switched to output, must move its read-back to "
            "the written value -- 0 after a written 0, 1 after a written 1 "
            "-- because a read returns the pin (p.341) and, released, "
            "nothing else drives it. See MEASURED below for the per-bit "
            "control/cross/drv0/drv1 values this run read "
            "[p.241 \"CIAADDRA ($BFE201) contains a mask that corresponds "
            "one-to-one with the data read register, CIAAPRA ($BFE001)\"; "
            "p.341 \"When you READ a PR register, you read the actual "
            "current state of the I/O pins\"; p.354 \"a read of the ports "
            "will return all highs because of passive pull-ups\"]");
        checks.note("PROP-1 per-bit positional correspondence: " + detail);
    }

    {
        Harness h;
        std::string detail;
        const bool result = prop2_universal_direction_makes_output(h, detail);
        checks.expect(result,
            "opus:8-dio.c46 sol:8-dio.c43 PROP-2: setting a 1 in CIAADDRA's "
            "direction position makes the corresponding CIAAPRA bit an "
            "output -- graded across all eight bit positions (PA0-PA7), "
            "fixture released (porta_in = 0x3F, p.354's idle-high state): "
            "each position's read-back must follow what was just written "
            "to CIAAPRA at that position, 0->0 and 1->1, once its own "
            "direction bit is set to 1, because a read returns the pin "
            "(p.341) and, released, nothing else drives it; with the "
            "direction bit left at 0 (control), the read-back must stay at "
            "the released value, 1, whatever was written. See MEASURED "
            "below for the per-bit control/drv0/drv1 values this run read "
            "[p.241 \"Setting a 1 in the direction position makes the "
            "corresponding bit an output\"; p.341 \"When you READ a PR "
            "register, you read the actual current state of the I/O "
            "pins\"; p.354 \"a read of the ports will return all highs "
            "because of passive pull-ups\"]");
        checks.note("PROP-2 per-bit output-follows-direction: " + detail);
    }

    { Harness h; checks.expect(
        bit_position_correspondence(h, /*bit=*/0, /*other_bit=*/2,
                                     mutating("CONTROL-PA0-POSITIONAL")),
        "CONTROL-PA0-POSITIONAL: the same bit_position_correspondence() "
        "comparator PROP-1 sweeps above, instantiated at bit 0 alone (PA2 "
        "as the neutral different-position stimulus) -- the one position "
        "PROP-1 shows this mechanism actually working for. Not itself a "
        "page claim: it is PROP-1's measurement-basis control and the "
        "substitution proof that PROP-1's comparator is not vacuously "
        "false [no page -- infrastructure control]"); }

    { Harness h; checks.expect(
        bit_output_follows_direction(h, /*bit=*/0, mutating("CONTROL-PA0-OUTPUT")),
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
