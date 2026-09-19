// Test purpose: Appendix E p.323 "JOY STICKS ...DB9 male" table, userio.v
//   alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 228, 232, 234, 323
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim outside the 45 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 45 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// appDE-joystick-pinout -- Appendix E p.323 "JOY STICKS ...DB9 male" table,
// userio.v alone. 21 claims, filtered from
// hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl on
// classify.package == "appDE-joystick-pinout".
//
// p.323 prints the same pin-to-function map hrm/ch8-controller-pin-function-map
// (fixture `userio`, chapter 8 slice s1 ACCEPTED per OPEN-ISSUES.md O9) already
// grades from p.228 Table 8-1. Eighteen of this package's 21 claims restate a
// proposition that accepted package already asserts or declines, under a
// different uid (a second reader, or a claim read off this page's figure
// instead of that table) -- those are declined below with a record() citing
// the prior uid(s) they duplicate.
//
// Three claims are NEW and are graded here: p.323's pin 6 row, "FIRE*" /
// "(or button 1)". ch8-controller-pin-function-map's own Group D declined all
// four of ITS pin-6 uids as "wrong fixture ... belongs to
// ch8-ciaapra-fire-buttons on the ciaa fixture, not to userio.v". Checked:
// grep -i 'pin.6|DB.?9' over hrm/ch8-ciaapra-fire-buttons's test source and
// SPECIFICATION.md returns exactly two comment lines citing the p.228 pin-6
// footnote, and both are prose, not an assertion; that package's own three
// graded propositions are CIAAPRA bit semantics only (P1 "1 means switch
// open", P2 "0 means switch closed", P3 "port 1 uses bit 6, port 2 uses bit
// 7") and never the DB9-pin-to-fire-line binding. The binding fell between
// the two packages, and it IS constructible right here:
//
//     userio.v:313  assign _fire0 = cd32pad && !cd32pad1_reg_load
//                                    ? fire1_d : _sjoy1[4] & _mleft;
//
// `cd32pad` is tied to 1'b0 (userio.v:91), so this reduces unconditionally,
// for every reachable state, to `_fire0 = _sjoy1[4] & _mleft` -- joystick
// FIRE* (`_sjoy1[4]`) AND mouse button 1 (`_mleft`, userio.v:361
// `assign _mleft = ~mouse_btn[0];`) converging on one output line, exactly as
// p.323 prints them on one row. `joy_swap` is tied to 1'b0 (userio.v:92) and
// `joy_ana_en` to 1'b0 (userio.v:93), so `_sjoy1 <= joy2` (userio.v:215) and
// `joy2 == _joy2` (userio.v:211) -- so on THIS connector, pin 6 is bit 4 of
// the `_joy2` input port, matching the bit map
// ch8-controller-pin-function-map's own header comment already records
// ("bit4=pin6(fire) ... on both _joy1 and _joy2").
//
// Port scope of the new grade: `_fire0` (this connector) is what is graded,
// since userio.v:313 is where the convergence p.323 prints actually lives and
// where classify's own evidence for these three uids is scoped ("observable:
// ... -- `_fire0`/`_fire1` at userio.v:313-314"). `_fire1`, the OTHER
// connector's fire line (userio.v:314, `_fire1 = _sjoy2[4]`, unconditionally
// once cd32pad's tie-off is applied), carries no mouse-button term at all --
// measured below as a note(), not folded into the graded boolean. Unlike
// ch8-controller-pin-function-map's Group B2 (pin 5), which graded a
// port-PARITY question because Table 8-1 states that row identically for
// both physical ports and classify recorded that as the claim under test,
// none of THIS package's three pin-6 uids' restatements say anything about
// the other connector -- they restate p.323's one row, which this fixture
// makes true on the connector userio.v puts it on. Reported as a measurement,
// not graded as a requirement the claims never state.
//
// MUTATION fixture-liveness stimulus red second driven word (pin4/bit0 on _joy1) left idle instead of pressed
// MUTATION sol:E-gameport-pinout.c11 stimulus red pin6 (_joy2 bit4) never pressed
// MUTATION opus:E-joystick-pinout.c8 stimulus red pin6 (_joy2 bit4) never pressed in the joystick-half sub-test (either half failing reds the compound row; the two halves are also graded individually as sol:E-gameport-pinout.c11/c12)
// MUTATION sol:E-gameport-pinout.c12 stimulus red mouse_btn bit0 (button 1) never pressed
// MUTATION opus:E-joystick-pinout.c8 stimulus red mouse_btn bit0 (button 1) never pressed in the mouse-half sub-test

#include "Vuserio.h"
#include "verilated.h"
#include <iostream>
#include <string>
#include <cstdint>

static Vuserio* top;
static vluint64_t sim_time = 0;
static int passed = 0, failed = 0;

// ---- the fixed harness (character-identical across the suite) ----
void expect(bool cond, const std::string& label) {
    if (cond) { std::cout << "PASS: " << label << "\n"; passed++; }
    else      { std::cout << "FAIL: " << label << "\n"; failed++; }
}
void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// hex formatting for register words in MEASURED lines.
std::string hex16(uint16_t v) {
    static const char* digits = "0123456789abcdef";
    std::string s(4, '0');
    for (int i = 3; i >= 0; i--) { s[i] = digits[v & 0xF]; v >>= 4; }
    return s;
}

// ---- clocking ----
void tick() {
    top->clk = 0; top->eval(); sim_time++;
    top->clk = 1; top->eval(); sim_time++;
}
void settle(int n) { for (int i = 0; i < n; i++) tick(); }

// ---- register bus (matches userio.v's own JOY0DAT/JOY1DAT parameters) ----
static const uint16_t JOY1DAT = 0x00c;
static const uint8_t  ADDR_NONE = 0xFF; // no register in this file's scope decodes to this

static uint8_t addr_of(uint16_t regaddr) { return (uint8_t)((regaddr >> 1) & 0xFF); }

uint16_t read_reg(uint16_t regaddr) {
    top->reg_address_in = addr_of(regaddr);
    top->eval();
    uint16_t v = top->data_out;
    top->reg_address_in = ADDR_NONE;
    top->eval();
    return v;
}

// ---- _joy1 / _joy2 bit helpers (active-low: 0 = pressed, 1 = idle) ----
static uint16_t joy1_val = 0xFFFF;
static uint16_t joy2_val = 0xFFFF;
void joy1_bit(int bit, bool pressed) {
    if (pressed) joy1_val &= ~(1u << bit); else joy1_val |= (1u << bit);
    top->_joy1 = joy1_val;
}
void joy2_bit(int bit, bool pressed) {
    if (pressed) joy2_val &= ~(1u << bit); else joy2_val |= (1u << bit);
    top->_joy2 = joy2_val;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    // known, idle defaults for every input this file does not otherwise drive
    top->clk = 0;
    top->reset = 1;
    top->clk7_en = 1; // hold the enable high throughout; nothing here tests its timing ratio
    top->reg_address_in = ADDR_NONE;
    top->data_in = 0;
    top->pot_cnt_en = 0;
    top->_fire0_dat = 1;
    top->_fire1_dat = 1;
    top->_joy1 = joy1_val;
    top->_joy2 = joy2_val;
    top->joy_ana1 = 0;
    top->joy_ana2 = 0;
    top->mouse_btn = 0;
    top->kms_level = 0;
    top->kbd_mouse_type = 0;
    top->kbd_mouse_data = 0;
    top->eval();

    settle(5);
    top->reset = 0;
    settle(5);

    // =====================================================================
    // Fixture liveness -- proven before any claim is graded, on a channel
    // OTHER than the one the graded claims use below (_joy1 -> _sjoy2 ->
    // JOY1DAT, an unconditional read mux -- userio.v:294), so it is not
    // circular with the _joy2/_sjoy1/_fire0 path the pin-6 claims measure.
    // Same shape as ch8-controller-pin-function-map's own fixture-liveness
    // check (same connector, same bit, same register).
    // =====================================================================
    joy1_val = 0xFFFF; top->_joy1 = joy1_val; settle(4);
    uint16_t word_a = read_reg(JOY1DAT);
#ifndef MUT_fixtureliveness
    joy1_val = 0xFFFE; top->_joy1 = joy1_val; settle(4); // bit0 (pin4) pressed: distinct word
#else
    joy1_val = 0xFFFF; top->_joy1 = joy1_val; settle(4); // mutation: same word again
#endif
    uint16_t word_b = read_reg(JOY1DAT);
    note("fixture-liveness JOY1DAT word A=0x" + hex16(word_a) +
         " word B=0x" + hex16(word_b));
    expect(word_a != word_b, "fixture-liveness");
    joy1_val = 0xFFFF; top->_joy1 = joy1_val; settle(4);

    // =====================================================================
    // NEW -- DB9 pin 6, "FIRE*" / "(or button 1)" (p.323). The coverage gap:
    // graded nowhere else in the suite (see file header). Both sub-tests hold
    // the OTHER half's input at idle throughout, so each measures its own
    // convergent term in isolation; the compound uid grades their conjunction.
    // =====================================================================
    {
        // Baseline: pin6 idle, mouse button 1 idle.
        joy2_bit(4, false);
        top->mouse_btn = 0x00;
        settle(4);
        bool fire0_idle = top->_fire0;

        // --- joystick half: press pin6 (_joy2 bit4) alone ---
#ifndef MUT_pin6joy
        joy2_bit(4, true);
#endif
        settle(4);
        bool fire0_on_joy = !top->_fire0;
        joy2_bit(4, false); settle(4); // release
        bool fire0_release_after_joy = top->_fire0;

        note("pin6 joystick-half (_joy2 bit4 -> _fire0): fire0_idle=" +
             std::to_string((int)fire0_idle) + " fire0_fired_while_pressed=" +
             std::to_string((int)!fire0_on_joy) + " fire0_idle_after_release=" +
             std::to_string((int)fire0_release_after_joy));
        bool joy_half_ok = fire0_idle && fire0_on_joy && fire0_release_after_joy;
        expect(joy_half_ok, "sol:E-gameport-pinout.c11");

        // --- mouse half: press mouse button 1 (mouse_btn bit0) alone ---
#ifndef MUT_pin6mouse
        top->mouse_btn = 0x01;
#endif
        settle(4);
        bool fire0_on_mouse = !top->_fire0;
        top->mouse_btn = 0x00; settle(4); // release
        bool fire0_release_after_mouse = top->_fire0;

        note("pin6 mouse-half (mouse_btn bit0 -> _mleft -> _fire0): idle=" +
             std::to_string((int)fire0_idle) + " fire0_while_pressed=" +
             std::to_string((int)!fire0_on_mouse) + " released=" +
             std::to_string((int)fire0_release_after_mouse));
        bool mouse_half_ok = fire0_idle && fire0_on_mouse && fire0_release_after_mouse;
        expect(mouse_half_ok, "sol:E-gameport-pinout.c12");

        // --- combined row (opus's single claim spans both halves) ---
        expect(joy_half_ok && mouse_half_ok, "opus:E-joystick-pinout.c8");

        // --- measured, not graded: the OTHER connector's fire line carries
        // no mouse-button term at all (userio.v:314, _fire1 = _sjoy2[4]
        // alone, once cd32pad's tie-off at userio.v:91 is applied). Driven
        // through _joy1 bit4 (this connector's pin 6) and mouse_btn, which
        // has no wire into _fire1 at all. ---
        {
            joy1_bit(4, false); top->mouse_btn = 0x00; settle(4);
            bool fire1_idle = top->_fire1;
            joy1_bit(4, true); settle(4);
            bool fire1_on_joy = !top->_fire1;
            joy1_bit(4, false); settle(4);
            top->mouse_btn = 0x01; settle(4);
            bool fire1_on_mousebtn = !top->_fire1; // expected to stay idle: no term
            top->mouse_btn = 0x00; settle(4);
            note("pin6 other connector (_joy1 bit4 -> _fire1): idle=" +
                 std::to_string((int)fire1_idle) + " fire1_while_joy_pressed=" +
                 std::to_string((int)fire1_on_joy) + " fire1_while_mousebtn_pressed=" +
                 std::to_string((int)fire1_on_mousebtn) +
                 " -- joystick half present, mouse-button half absent on this copy");
        }
    }

    // =====================================================================
    // Declined claims -- one record() line per uid. Every one of these
    // eighteen restates a proposition hrm/ch8-controller-pin-function-map or
    // hrm/ch8-mouse-button-pins already grades (ASSERT or DECLINE), under a
    // different uid, from p.228 Table 8-1 instead of p.323's figure.
    // =====================================================================

    // --- pins 1-4, joystick-column halves: p.323 prints these direction
    // names with a trailing "*" (FORWARD*, BACK*, LEFT*, RIGHT*) where p.228
    // Table 8-1 prints them bare (Forward, Back, Left, Right).
    // ch8-controller-pin-function-map's Group A asserts the pin->direction
    // BINDING from the bare p.228 wording and states no polarity either way
    // (a level-in/level-out measurement, no "active-low" language). That is
    // the SAME proposition this row's joystick half restates, so it is
    // declined against it. CONDITION: if the "*" is read as an assertion
    // that this line is active-low, that reading is NOT what Group A graded
    // and is NOT covered by this decline -- no assertion in this package or
    // in Group A grades polarity, so a polarity claim on this pin stays
    // ungraded, not declined.
    record("sol:E-gameport-pinout.c1 (pin 1 = joystick FORWARD*) declined: "
           "the pin1->Forward BINDING is the same proposition "
           "hrm/ch8-controller-pin-function-map Group A already asserts "
           "(opus:8-tbl8-1.c1, sol:8-pin.c1) from p.228 Table 8-1's bare "
           "'Forward'. CONDITION: Group A's measurement states no polarity; "
           "if the printed trailing '*' here is read as an active-low "
           "assertion, that reading is NOT covered by this decline and is "
           "not graded by anything in this package either.");
    record("sol:E-gameport-pinout.c2 (pin 1 = mouse (MOUSE V)) declined: same "
           "proposition as hrm/ch8-controller-pin-function-map Group A "
           "(opus:8-tbl8-1.c2, sol:8-pin.c2), 'V-pulse'; p.323's '(MOUSE V)' "
           "names the identical quadrature line, no polarity marker on this "
           "half.");
    record("sol:E-gameport-pinout.c3 (pin 2 = joystick BACK*) declined: same "
           "BINDING as hrm/ch8-controller-pin-function-map Group A "
           "(opus:8-tbl8-1.c4, sol:8-pin.c5), p.228's bare 'Back'. Same "
           "CONDITION as c1: binding declined, polarity not covered by "
           "anything.");
    record("sol:E-gameport-pinout.c4 (pin 2 = mouse (MOUSE H)) declined: same "
           "proposition as Group A (opus:8-tbl8-1.c5, sol:8-pin.c6), "
           "'H-pulse'.");
    record("sol:E-gameport-pinout.c5 (pin 3 = joystick LEFT*) declined: same "
           "BINDING as Group A (opus:8-tbl8-1.c6, sol:8-pin.c7), p.228's bare "
           "'Left'. Same CONDITION as c1.");
    record("sol:E-gameport-pinout.c6 (pin 3 = mouse (MOUSE VQ)) declined: "
           "same proposition as Group A (opus:8-tbl8-1.c7, sol:8-pin.c8), "
           "'VQ-pulse'.");
    record("sol:E-gameport-pinout.c7 (pin 4 = joystick RIGHT*) declined: same "
           "BINDING as Group A (opus:8-tbl8-1.c10, sol:8-pin.c11), p.228's "
           "bare 'Right'. Same CONDITION as c1.");
    record("sol:E-gameport-pinout.c8 (pin 4 = mouse (MOUSE HQ)) declined: "
           "same proposition as Group A (opus:8-tbl8-1.c11, sol:8-pin.c12), "
           "'HQ-pulse'.");

    // --- pin 5: POT X (Group B2, graded RED there -- NM-HRM-037) and mouse
    // button 3 (ch8-mouse-button-pins P3).
    record("sol:E-gameport-pinout.c9 (pin 5 = joystick POT X) declined: same "
           "proposition as hrm/ch8-controller-pin-function-map Group B2 "
           "(opus:8-tbl8-1.c16, sol:8-pin.c18; also c15/c17 for the 'Right "
           "POT' naming), graded RED there -- that red is NM-HRM-037, 'the "
           "right controller port's X potentiometer input does not exist' "
           "(docs/findings/NM-HRM-037.md). p.323 is a fifth printed page "
           "stating pin 5 = POT X, after pp.228, 234, 240 and 282 already "
           "cited in that finding; corroborating, no new testable content.");
    record("sol:E-gameport-pinout.c10 (pin 5 = mouse '(or button 3 ... if "
           "used)') declined: same proposition as hrm/ch8-mouse-button-pins "
           "P3 (opus:8-mbtn.c12, sol:8-button.c10), p.232 'Button 3, when "
           "used, is connected to pin 5'. p.323 numbers the button rather "
           "than naming a housing position, landing on the ASSERTED side of "
           "ch8-controller-pin-function-map's own Group C / P3 split, not the "
           "declined device-identity side.");

    // --- pin 9: POT Y (Group B) and mouse button 2 (ch8-mouse-button-pins
    // P2).
    record("sol:E-gameport-pinout.c15 (pin 9 = joystick POT Y) declined: same "
           "proposition as hrm/ch8-controller-pin-function-map Group B "
           "(opus:8-tbl8-1.c33, sol:8-pin.c36; also c32/c35 for 'Left POT'), "
           "p.228 Table 8-1's pin 9 row. Identical wording, nothing narrower "
           "here.");
    record("sol:E-gameport-pinout.c16 (pin 9 = mouse '(or button 2)') "
           "declined: same proposition as hrm/ch8-mouse-button-pins P2 "
           "(opus:8-mbtn.c10, sol:8-button.c8), p.232 'Button 2 ... is "
           "connected to pin 9'. Numbered button, asserted side of the "
           "Group C / P2 split, same as c10 above.");

    // --- opus's four combined-row claims (pins 1-4): one uid spans both the
    // joystick and mouse halves of its row. Both halves are covered
    // elsewhere, so the whole row is declined; the CONDITION on the "*"
    // polarity reading applies to its joystick half exactly as for sol's
    // split claims above.
    record("opus:E-joystick-pinout.c3 (pin 1: 'FORWARD* as a joystick and "
           "MOUSE V as a mouse') declined: both halves already graded by "
           "hrm/ch8-controller-pin-function-map Group A -- joystick half "
           "opus:8-tbl8-1.c1/sol:8-pin.c1, mouse half opus:8-tbl8-1.c2/"
           "sol:8-pin.c2. CONDITION on the joystick half: Group A's binding "
           "measurement states no polarity; a reading of p.323's trailing "
           "'*' as an active-low assertion is not covered by this decline "
           "and is not graded by anything in this package.");
    record("opus:E-joystick-pinout.c4 (pin 2: 'BACK* as a joystick and MOUSE "
           "H as a mouse') declined: both halves already graded by Group A "
           "-- opus:8-tbl8-1.c4/sol:8-pin.c5 (joystick), opus:8-tbl8-1.c5/"
           "sol:8-pin.c6 (mouse). Same CONDITION on the joystick half as "
           "c3.");
    record("opus:E-joystick-pinout.c5 (pin 3: 'LEFT* as a joystick and MOUSE "
           "VQ as a mouse') declined: both halves already graded by Group A "
           "-- opus:8-tbl8-1.c6/sol:8-pin.c7 (joystick), opus:8-tbl8-1.c7/"
           "sol:8-pin.c8 (mouse). Same CONDITION on the joystick half as "
           "c3.");
    record("opus:E-joystick-pinout.c6 (pin 4: 'RIGHT* as a joystick and "
           "MOUSE HQ as a mouse') declined: both halves already graded by "
           "Group A -- opus:8-tbl8-1.c10/sol:8-pin.c11 (joystick), "
           "opus:8-tbl8-1.c11/sol:8-pin.c12 (mouse). Same CONDITION on the "
           "joystick half as c3.");

    // --- opus's two remaining combined-row claims (pins 5, 9).
    record("opus:E-joystick-pinout.c7 (pin 5: 'POT X, or button 3 if used') "
           "declined: joystick half already graded RED by Group B2 "
           "(opus:8-tbl8-1.c16/sol:8-pin.c18 -- NM-HRM-037), mouse half "
           "already graded by hrm/ch8-mouse-button-pins P3 "
           "(opus:8-mbtn.c12/sol:8-button.c10). Same as sol's c9/c10 above, "
           "combined into one uid.");
    record("opus:E-joystick-pinout.c11 (pin 9: 'POT Y, or button 2') "
           "declined: joystick half already graded by Group B "
           "(opus:8-tbl8-1.c33/sol:8-pin.c36), mouse half already graded by "
           "hrm/ch8-mouse-button-pins P2 (opus:8-mbtn.c10/sol:8-button.c8). "
           "Same as sol's c15/c16 above, combined into one uid.");

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
