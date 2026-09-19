// appDE-lightpen-userio
//
// Grades 6 claims (3 propositions, each claimed independently by readers sol
// and opus) against `userio` (compiled alone), from Appendix E p.333's
// CONNECTOR PIN USAGE table and LIGHT PEN figure.
//
// uid -> proposition:
//   sol:E-lightpen.c9,  opus:E-lightpen.c11  pin 5 = LPENPR*, "Light pen
//     pressed ... See Proportional Inputs" (connector table)
//   sol:E-lightpen.c19, opus:E-lightpen.c22  "POTINP read only  DFF104"
//     (figure box -- the printed defect, see below)
//   sol:E-lightpen.c20, opus:E-lightpen.c23  "PEN PRESS = POT0X", dashed
//     route from PORT 0 pin 5 to the POTINP box (figure)
//
// Test purpose: appDE-lightpen-userio Grades 6 claims (3 propositions, each
//   claimed independently by readers sol and opus) against `userio`
//   (compiled alone), from Appendix E p.333's CONNECTOR PIN USAGE table and
//   LIGHT PEN figure.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 333 (Appendix E, CONNECTOR PIN USAGE table and LIGHT PEN figure).
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// DISPOSITION -- all six claims DECLINE, both readers, all three
//   propositions. None asserted; see each record() below for the specific
//   reason (a re-grade, a non-constructible device-identity split, or the
//   page's own printed self-contradiction).
//
// THE PIN-5 PATH (c9/c11, c20/c23). classify's own "split" field for both
//   propositions already separates a PATH half (pin 5's level reaches
//   POTINP) from a DEVICE-IDENTITY half (that the level's external cause is
//   specifically "a light pen pressed to the screen"). The PATH half is
//   already graded, RED, in TWO accepted chapters: hrm/ch8-controller-pin-
//   function-map's Group B2 and hrm/ch8-pot-pins-as-digital-io's Group 5
//   (ENTIRE-REGISTER-SENSES/POTGOR-REPORTS-STATE) both measure the same
//   RTL fact -- port 1's gate is live (userio.v:176,
//   `potcap[0] <= _mthird & joy1_pin5`) while port 2's copy senses no
//   external line at all (userio.v:169,
//   `joy2_pin5 = ~(potreg[13] & ~potreg[12])`, a pure function of the POTGO
//   write register). The DEVICE-IDENTITY half is declined as non-
//   constructible: hrm/ch8-controller-pin-function-map's Group C already
//   declined the identical p.228 cell ("5* ... Light Pen ... Pen pressed to
//   screen") on exactly this reasoning -- a digital fixture with no notion
//   of what is plugged into the D-connector cannot adjudicate which named
//   device (light pen vs. mouse middle button) produced a sensed level.
//   Neither half is re-measured below; both are cited and declined.
//   (Cross-checked against the sibling appDE-pot-counters package, built in
//   this same lot against p.332's own POTX/device-identity cells -- its
//   Group G5/G8 reach the identical disposition for the identical
//   underlying RTL fact, under different uids. Not relied on as a citation
//   -- it is an uncommitted sibling, not an accepted chapter -- but it is
//   independent corroboration that this is the right call.)
//
// THE POTINP/DFF104 DEFECT (c19/c22). p.333's own figure prints
//   'POTINP read only  DFF104', the SAME address it prints for 'BPLCON0
//   write only' two boxes above -- one address, two registers, on the same
//   page. p.332's own POT COUNTERS figure prints 'POTINP  DFF016' for the
//   same register. $DFF104 is neither address: appA-playfield/p271.txt:12
//   and appB3/p291.txt:29 both put BPLCON2 at 104 (BPLCON0 is 100), and
//   this package's own boundary confirms userio.v has no case for $104 at
//   all (measured in FIXTURE-LIVENESS/DFF104-PROBE below). Per the build
//   brief ("Where the manual contradicts itself, grade neither side") and
//   the NM-HRM-062 precedent: measure both readings, record both, grade
//   neither. hrm/ch8-register-map-userio's PROP-RW-POTGOR assertion
//   already grades $DFF016's read-only direction in an accepted chapter
//   (verified read directly: "$016 answers a non-zero value ... and an
//   attempted write of $BEEF to $016 leaves the next reading at $016
//   unchanged", citing p.282, p.289, p.240, and the p.287 legend) -- so
//   even the DIRECTION half, if treated separately from the disputed
//   address, is a re-grade, not new content.
//
// FIXTURE LIVENESS / DFF104-PROBE. No claim id, no manual page: proves (a)
//   the harness can drive a chosen, non-default state through to POTINP's
//   real address before any claim above is trusted to have had a real
//   fixture behind it, and (b) supplies the actual DFF104 measurement the
//   POTINP-defect record() below cites. Harness mechanics (tick/settle/
//   full_reset/read_reg) reused, trimmed to what this package needs, from
//   the sibling appDE-pot-counters/test_userio.cpp built in this same lot
//   against the same module and the same claims.jsonl evidence table.
//
// MUTATION FIXTURE-LIVENESS stimulus red: the mouse third-button release
//   (set_mouse's third argument) was changed from "released" (1) to "held"
//   (0, matching the reset default), so POTINP's bit 8 never moved from 1
//   to 0 and the before/after comparison found no change. Built with
//   `make MUT=FIXTURE_LIVENESS`, confirmed "0 passed, 1 failed", reverted
//   (obj_dir cleaned, rebuilt with no MUT flag) and confirmed "1 passed, 0
//   failed" restored.

#include "Vuserio.h"
#include "verilated.h"
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

static Vuserio* dut;

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
} checks;

std::string hex4(uint16_t v) {
    std::ostringstream os;
    os << "0x" << std::hex << std::setw(4) << std::setfill('0') << v;
    return os.str();
}

// Register addresses, as reg_address_in[8:1] values (word index = byte
// address >> 1). A_DFF104 is p.333's printed (wrong) POTINP address.
constexpr int A_POTINP = 0x016 >> 1;
constexpr int A_POTGO  = 0x034 >> 1;
constexpr int A_DFF104 = 0x104 >> 1;
constexpr int A_IDLE   = 0x000;

void tick() {
    dut->clk = 0; dut->eval();
    dut->clk = 1; dut->eval();
}
void settle(int n = 3) { for (int i = 0; i < n; i++) tick(); }
void idle_bus() { dut->reg_address_in = A_IDLE; dut->data_in = 0; }

void full_reset() {
    dut->reset = 1;
    dut->clk7_en = 1;
    dut->pot_cnt_en = 0;
    idle_bus();
    dut->mouse_btn = 0;
    dut->_joy1 = 0xFFFF;
    dut->_joy2 = 0xFFFF;
    dut->_fire0_dat = 1;
    dut->_fire1_dat = 1;
    dut->joy_ana1 = 0;
    dut->joy_ana2 = 0;
    dut->kms_level = 0;
    dut->kbd_mouse_type = 0;
    dut->kbd_mouse_data = 0;
    for (int i = 0; i < 4; i++) tick();
    dut->reset = 0;
    settle(3);
}

void arm_potgo(uint16_t val) {
    dut->reg_address_in = A_POTGO;
    dut->data_in = val;
    tick();
    idle_bus();
    tick();  // arm/reset fires here if val&1; potreg[0] self-clears after this edge
    settle(3);
}

uint16_t read_reg(int addr) {
    dut->reg_address_in = addr;
    dut->eval();
    uint16_t v = dut->data_out;
    idle_bus();
    dut->eval();
    return v;
}

// mouse_btn[2:0] = {third, right, left} (userio.v:362-363: _mright =
// mouse_btn[1], _mthird = mouse_btn[2]).
void set_mouse(int left, int right, int third) {
    dut->mouse_btn = (uint8_t)((third << 2) | (right << 1) | left);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    dut = new Vuserio;

    // ================= FIXTURE-LIVENESS / DFF104-PROBE (no page, no uid) ====
    full_reset();
    arm_potgo(0x0001);
    uint16_t potinp_before = read_reg(A_POTINP);
    // mouse_btn is active-low at this boundary (userio.v:363,
    // `assign _mthird = ~mouse_btn[2];`), so full_reset's mouse_btn=0
    // default reads as the third button HELD (measured: potinp_before
    // below carries bit 8 set). Driving mouse_btn bit 2 high releases it.
    int third_released = 1;
#ifdef MUT_FIXTURE_LIVENESS
    third_released = 0;  // stays held, matching the reset default: no change forced
#endif
    set_mouse(/*left=*/0, /*right=*/0, /*third=*/third_released);
    settle(3);
    uint16_t potinp_after = read_reg(A_POTINP);
    checks.expect(
        potinp_after != potinp_before,
        "FIXTURE-LIVENESS: releasing the mouse third button (driving mouse_btn bit 2 "
        "high, which userio.v:363 inverts to `_mthird`) changes userio.v:176's "
        "`potcap[0] <= _mthird & joy1_pin5` gate and changes what a subsequent read "
        "at POTINP's real address ($016) reports (" + hex4(potinp_before) + " -> " +
        hex4(potinp_after) + ") -- the drive-then-read data path to POTINP is "
        "alive at this boundary before any claim below is declined on the strength of "
        "what this fixture COULD measure.");

    uint16_t potinp_at_016 = read_reg(A_POTINP);
    checks.note(
        "p.332/p.333 conflict on POTINP's address -- reading userio at its own $DFF016 "
        "(p.332's printed address, and the address this package's own read mux answers "
        "at) returns " + hex4(potinp_at_016) + ": userio.v's POTINP case (userio.v:59, "
        "parameter POTINP=9'h016; userio.v:300-305) answers there.");
    uint16_t at_104 = read_reg(A_DFF104);
    checks.note(
        "Reading userio at $DFF104 (p.333's printed address for the SAME register, "
        "'POTINP read only') returns " + hex4(at_104) + ": userio.v's read mux "
        "(userio.v:289-309) carries no case for this offset at all and falls through "
        "to the default 16'h0000. $DFF104 is not POTINP under either reading at this "
        "module: appA-playfield/p271.txt:12 and appB3/p291.txt:29 both print BPLCON2 "
        "at 104 (BPLCON0 is 100), and denise.v:59-60 agrees (BPLCON0=9'h100, "
        "BPLCON2=9'h104) -- a different module, outside this package's boundary.");

    // ================= sol:E-lightpen.c9, opus:E-lightpen.c11 =================
    checks.record(
        "sol:E-lightpen.c9, opus:E-lightpen.c11 (p.333 CONNECTOR PIN USAGE table: "
        "pin 5 = LPENPR*, 'Light pen pressed', 'See Proportional Inputs'): splits into "
        "a PATH half (pin 5's level reaches POTINP) and a DEVICE-IDENTITY half (that "
        "the level's external cause is specifically a light pen pressed to the "
        "screen, not a mouse's middle button). PATH: hrm/ch8-controller-pin-function-"
        "map Group B2 and hrm/ch8-pot-pins-as-digital-io Group 5 already grade pin 5's "
        "pot-gating in accepted chapters, RED -- port 1's gate (potcap[0], userio.v:176) "
        "is live, port 2's copy (potcap[2], userio.v:169) senses no external line at "
        "all. Declined as a re-grade of that same structural fact, not re-measured "
        "here. DEVICE IDENTITY: hrm/ch8-controller-pin-function-map Group C already "
        "declined the identical p.228 cell ('5* ... Light Pen ... Pen pressed to "
        "screen') for lack of observability at this boundary -- a digital fixture with "
        "no notion of what is plugged into the D-connector cannot adjudicate which "
        "named device produced a sensed level. Declined as non-constructible, same "
        "reasoning, different page.");

    // ================= sol:E-lightpen.c19, opus:E-lightpen.c22 =================
    checks.record(
        "sol:E-lightpen.c19, opus:E-lightpen.c22 (p.333 LIGHT PEN figure: 'POTINP read "
        "only  DFF104'): p.332's own POT COUNTERS figure prints 'POTINP  DFF016'; "
        "p.333's LIGHT PEN figure prints 'POTINP read only  DFF104', the same address "
        "p.333 gives 'BPLCON0 write only' two boxes above. Both readings measured above "
        "via note(). Per the build brief and the NM-HRM-062 precedent: measure both, "
        "record both, grade neither. ADDRESS not graded either way. DIRECTION: "
        "hrm/ch8-register-map-userio's PROP-RW-POTGOR already grades $DFF016 read-only "
        "in an accepted chapter (p.282, p.289, p.240 'POTGOR (formerly POTINP) "
        "($DFF016) is the read-only address', p.287 legend) -- the DFF016 side's "
        "direction would be a re-grade even if graded on its own axis, and the DFF104 "
        "side names an address this module does not decode at all (measured above), so "
        "grading it would red a correct decoder. The DFF016 side of this same conflict "
        "belongs to appDE-pot-counters (sol:E-pot.c32, opus:E-pot.c36), not to this "
        "package.");

    // ================= sol:E-lightpen.c20, opus:E-lightpen.c23 =================
    checks.record(
        "sol:E-lightpen.c20, opus:E-lightpen.c23 (p.333 LIGHT PEN figure: dashed route "
        "'PEN PRESS = POT0X', from PORT 0 pin 5 to the POTINP box's bit 8): same PATH / "
        "DEVICE-IDENTITY split as sol:E-lightpen.c9 and opus:E-lightpen.c11 above, "
        "restated under the POT0X/PORT 0 naming this figure adds -- the dashed line "
        "traces to POTINP bit 8, which is potcap[0] (userio.v:304), port 1's copy of "
        "the identical gate Group B2/Group 5 already grade. PATH: same two accepted-"
        "chapter citations, same RED result, declined as a re-grade. DEVICE IDENTITY: "
        "'PEN PRESS' is a device-identity label on the same wire hrm/ch8-controller-"
        "pin-function-map Group C already declined for the same reason; declined as "
        "non-constructible. Port numbering itself (PORT 0 here vs. PORT 1/PORT 2 on "
        "p.330, PORT 0/PORT 1 on p.332) is a separate, known, unresolved printed "
        "conflict (hrm/ch8-ciaapra-fire-buttons KNOWN CONFLICT 3, and this package's own "
        "classify.note) -- not asserted or resolved by this decline either.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
