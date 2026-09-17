// appDE-gameport-common -- p.327 "Game Controller Connector Interface
// Specification", userio.v alone. 12 claims, filtered from
// hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl on
// classify.package == "appDE-gameport-common".
//
// p.327 opens the appendix's game-port material: the two 9-pin D-type male
// connectors, the four device categories they interface to (mouse/trackball,
// digital joystick, proportional, light pen), and the anchor paragraph:
//
//     J11 is the right controller port connector (JOY1DAT, POT1DAT).
//     J12 is the left  controller port connector (JOY0DAT, POT0DAT).
//
// That anchor line is what makes the "fire buttons are on the wrong bits"
// finding candidate (work/2026-09-17_appDE_finding_candidates.md #8,
// predicted RED in appDE-fire-buttons-ciaa / appDE-portsig-cia-bindings)
// provable at all: without SOME page saying which physical side is JOY0DAT,
// p.329's "left"/"right" language cannot be joined to a bit number. This
// package holds the four uids that carry that anchoring (sol:E-gameport.c6/
// c7, opus:E-gameport.c8/c9) and grades them carefully below -- see the
// DECLINED CLAIMS section for exactly what this fixture can and cannot see.
//
// This package's 6 distinct propositions (each read by both sol and opus,
// 12 uids total):
//
//   1. "The two 9-pin D-type connectors with pins (male)..." (device count +
//      physical connector form)                                    DECLINE
//   2. "1. Mouse or trackball, 3 buttons max."                      DECLINE
//   3. "2. Digital joystick, 2 buttons max."                        GRADE
//   4. "3. Proportional (pot or proportional joystick), 2 buttons max." GRADE
//   5. "J11 is the right controller port connector (JOY1DAT, POT1DAT)." DECLINE
//   6. "J12 is the left controller port connector (JOY0DAT, POT0DAT)."  DECLINE
//
// A PRINTED AMBIGUITY runs through this page's neighbourhood (p.330 labels
// its connector pair PORT 1/PORT 2; p.332 labels the same pair PORT 0/
// PORT 1; p.333 labels one PORT 0 -- three consecutive figures, two
// numbering conventions, not resolvable on the page -- finding candidate
// #7). p.327 itself never uses "PORT n" language at all: it uses connector
// designators (J11/J12) and the words "right"/"left". None of this
// package's 6 propositions cite a PORT numeral, so the ambiguity is
// recorded here for context and does not change how anything below is
// graded, and no claim resolves it either way.
//
// RTL FACTS USED THROUGHOUT (userio.v, pinned submodule
// d6f7df957fad655bc07f6a381e5094a9edff0be0):
//
//   userio.v:41-42   input [15:0] _joy1 (joystick 1 in, default mouse port),
//                    input [15:0] _joy2 (joystick 2 in, default joystick port)
//   userio.v:92      wire joy_swap = 1'b0;  (tied off)
//   userio.v:211-218 _sjoy1 <= joy_swap ? joy1 : joy2;   -> _sjoy1 = joy2
//                    _sjoy2 <= joy_swap ? joy2 : joy1;   -> _sjoy2 = joy1
//                    (joy1/joy2 == _joy1/_joy2 unmodified, joy_ana_en tied 0)
//   userio.v:219-282 the "Port 1" block (dmouse0dat, read out at JOY0DAT)
//                    is built from _sjoy1/_djoy1 == _joy2; the "Port 2"
//                    block (dmouse1dat, read out at JOY1DAT) is built from
//                    _sjoy2/_djoy2 == _joy1. So the module's OWN internal
//                    "Port 1"/"Port 2" comment labels are the OPPOSITE of
//                    which external port name (_joy1/_joy2) feeds them --
//                    not a trivial pass-through.
//   userio.v:174     potcap[1] <= _mright & _djoy1[5] & ~(...);  (POT0DAT's
//                    Y-gate, bit5 of _djoy1 == _joy2's pin 9)
//   userio.v:164-167 potcap[3] <= _djoy2[5] & ~(...);  (POT1DAT's Y-gate,
//                    bit5 of _djoy2 == _joy1's pin 9)
//   userio.v:313-314 _fire0 = _sjoy1[4] & _mleft = _joy2[4] & _mleft;
//                    _fire1 = _sjoy2[4] = _joy1[4];  (cd32pad tied 0)
//   userio.v:289-310 POTINP's read mux: data_out = {1'b0, potcap[3], 1'b0,
//                    potcap[2], 1'b0, potcap[1], 1'b0, potcap[0], 8'h00} --
//                    exactly 4 non-constant bits (8, 10, 12, 14), the other
//                    12 are hard-wired 0 by construction, independent of any
//                    stimulus.
//
// MUTATION fixture-liveness stimulus red second driven word (pin4/bit0 on _joy1) left idle instead of pressed
//
// MUTATION sol:E-gameport.c3 none impossible: "2 buttons max" (joystick) is
//   graded as the conjunction of (a) pins 1-4 (bits 0-3 of _joy1/_joy2, the
//   direction/quadrature pins) never move _fire0/_fire1/potcap[0..3], and
//   (b) POTINP's 12 hard-wired-0 bit positions (userio.v:289-310) never
//   move under any stimulus. (a): grep over userio.v shows _fire0/_fire1
//   read bit 4 only and potcap[0..3]'s always-blocks read bits 5 and
//   mouse_btn only -- no fan-in from bits 0-3 exists for any stimulus to
//   exploit. (b): those 12 bits are Verilog literal constants in a
//   combinational mux branch, not a function of any register or input --
//   no stimulus can move a literal. Positive control: PIN-GROUP-LIVENESS
//   below shows bits 0-3 do move JOY0DAT/JOY1DAT, so (a) is not vacuous.
// MUTATION opus:E-gameport.c4 none impossible: same measurement as
//   sol:E-gameport.c3 -- this uid restates the same p.327 line.
// MUTATION sol:E-gameport.c4 none impossible: "2 buttons max"
//   (proportional) is graded on the IDENTICAL measurement as
//   sol:E-gameport.c3 -- p.332's proportional-connector table (pins 3/4,
//   LBUT*/RBUT*, "See Digital Joystick") reuses the joystick's own
//   direction pins as its two buttons, and p.332's pin1 (XBUT) carries no
//   HARDWARE REGISTER/NOTES entry at all, matching POTINP's own structural
//   cap of 4 total button-sense bits (2 per port) -- no third channel
//   exists for either device category. Same none-impossible argument as
//   sol:E-gameport.c3.
// MUTATION opus:E-gameport.c5 none impossible: same measurement as
//   sol:E-gameport.c4 -- this uid restates the same p.327 line.
//
// MUTATION PROP-JOY1-TO-JOY1DAT-POT1 stimulus red: _joy1's distinguishing
//   pattern is left idle (never driven) instead of pressed, so JOY1DAT
//   never moves off its baseline and the "JOY1DAT tracks _joy1" half reads
//   false (MUT_joy1binding).
// MUTATION PROP-JOY2-TO-JOY0DAT-POT0 stimulus red: _joy2's distinguishing
//   pattern is left idle instead of pressed, so JOY0DAT never moves off its
//   baseline (MUT_joy2binding).

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

std::string hex16(uint16_t v) {
    static const char* digits = "0123456789abcdef";
    std::string s(4, '0');
    for (int i = 3; i >= 0; i--) { s[i] = digits[v & 0xF]; v >>= 4; }
    return s;
}

void tick() {
    top->clk = 0; top->eval(); sim_time++;
    top->clk = 1; top->eval(); sim_time++;
}
void settle(int n) { for (int i = 0; i < n; i++) tick(); }

static const uint16_t JOY0DAT = 0x00a;
static const uint16_t JOY1DAT = 0x00c;
static const uint16_t POT0DAT = 0x012;
static const uint16_t POT1DAT = 0x014;
static const uint16_t POTINP  = 0x016;
static const uint16_t POTGO   = 0x034;
static const uint8_t  ADDR_NONE = 0xFF;

static uint8_t addr_of(uint16_t regaddr) { return (uint8_t)((regaddr >> 1) & 0xFF); }

void write_reg(uint16_t regaddr, uint16_t data) {
    top->reg_address_in = addr_of(regaddr);
    top->data_in = data;
    tick();
    top->reg_address_in = ADDR_NONE;
    top->data_in = 0;
    tick();
}
uint16_t read_reg(uint16_t regaddr) {
    top->reg_address_in = addr_of(regaddr);
    top->eval();
    uint16_t v = top->data_out;
    top->reg_address_in = ADDR_NONE;
    top->eval();
    return v;
}

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
void reset_joys() {
    joy1_val = 0xFFFF; joy2_val = 0xFFFF;
    top->_joy1 = joy1_val; top->_joy2 = joy2_val;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    top->clk = 0;
    top->reset = 1;
    top->clk7_en = 1;
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
    write_reg(POTGO, 0x0001);
    settle(4);

    note("page context: p.330 labels its connector pair PORT 1/PORT 2, "
         "p.332 labels the same pair PORT 0/PORT 1, p.333 labels one PORT 0 "
         "-- a printed ambiguity not resolvable on the page (finding "
         "candidate #7). p.327 itself never uses PORT-numeral language -- "
         "only J11/J12 and right/left -- and none of this package's 6 "
         "propositions cites a PORT numeral, so the ambiguity is recorded "
         "for context and does not bind any grade below.");

    // =====================================================================
    // Fixture liveness.
    // =====================================================================
    reset_joys(); settle(4);
    uint16_t word_a = read_reg(JOY1DAT);
#ifndef MUT_fixtureliveness
    joy1_bit(0, true); settle(4);
#else
    settle(4);
#endif
    uint16_t word_b = read_reg(JOY1DAT);
    note("fixture-liveness JOY1DAT word A=0x" + hex16(word_a) +
         " word B=0x" + hex16(word_b));
    expect(word_a != word_b, "fixture-liveness");
    reset_joys(); settle(4);

    // =====================================================================
    // DECLINED -- claim 1: "The two 9-pin D-type connectors with pins
    // (male) are used to interface to four types of devices." (sol:c1,
    // opus:c2). Two independent, each-sufficient reasons, neither leaves
    // anything gradable: the DEVICE-COUNT/KIND half ("four types of
    // devices", enumerated 1-4) is hrm/ch8-controller-port-inventory's own
    // proposition (printed p.227); the CONNECTOR-FORM half ("9-pin D-type
    // with pins (male)") is not constructible at this or any RTL fixture --
    // no net carries a shell, gender or contact-count fact -- exactly as
    // accepted opus:8-intro.c9 already records for the same connector.
    // =====================================================================
    record("sol:E-gameport.c1 (p.327, 'two 9-pin D-type connectors... four "
           "types of devices') declined: device-count/kind half is the same "
           "proposition hrm/ch8-controller-port-inventory already grades "
           "(p.227); connector-form half ('9-pin D-type with pins (male)') "
           "is not constructible at any RTL fixture -- no net carries "
           "shell/gender/contact-count -- same as accepted opus:8-intro.c9 "
           "for the identical connector. Nothing remains ungraded here to "
           "assert.");
    record("opus:E-gameport.c2 (p.327, 'two 9-pin D-type connectors... four "
           "types of devices') declined: same reasoning as sol:E-gameport.c1 "
           "-- both halves are, respectively, a duplicate of "
           "hrm/ch8-controller-port-inventory and not constructible.");

    // =====================================================================
    // DECLINED -- claim 2: "Mouse or trackball, 3 buttons max." (sol:c2,
    // opus:c3). Already graded: hrm/ch8-mouse-button-pins P1 grades "as
    // many as three mouse buttons" at userio, and hrm/ch8-controller-port-
    // inventory grades mouse/trackball device acceptance. Same proposition.
    // =====================================================================
    record("sol:E-gameport.c2 (p.327, 'Mouse or trackball, 3 buttons max.') "
           "declined: same proposition as hrm/ch8-mouse-button-pins P1 "
           "('as many as three mouse buttons', at userio) and "
           "hrm/ch8-controller-port-inventory (mouse/trackball acceptance).");
    record("opus:E-gameport.c3 (p.327, 'Mouse or trackball, 3 buttons "
           "max.') declined: same reasoning as sol:E-gameport.c2.");

    // =====================================================================
    // GRADED -- claims 3 & 4: the "2 buttons max" UPPER BOUND for a digital
    // joystick and for a proportional controller. Neither device category's
    // button COUNT is graded anywhere else in the suite (joystick
    // acceptance and the pot pins themselves are, per each uid's own
    // classify.note, but not this numeric ceiling). Constructed as: no
    // pin/bit outside the fire pin (6, not part of this package) and the
    // pin-9 second-button/pot-gate (5) ever reaches a button-sensing net,
    // and POTINP's read mux physically carries only 4 non-constant bits
    // (2 per port) -- an architectural cap that applies identically to
    // both device categories, since both re-use the same fire0/fire1/
    // potcap machinery (p.332's proportional table sends LBUT*/RBUT*,
    // pins 3-4, to "See Digital Joystick").
    // =====================================================================
    bool fire0_base, fire1_base;
    uint16_t potinp_base;
    {
        reset_joys(); top->mouse_btn = 0; write_reg(POTGO, 0x0001); settle(4);
        fire0_base = top->_fire0;
        fire1_base = top->_fire1;
        potinp_base = read_reg(POTINP);
        note("button-bound baseline: fire0=" + std::to_string((int)fire0_base) +
             " fire1=" + std::to_string((int)fire1_base) + " POTINP=0x" +
             hex16(potinp_base));
    }

    bool bound_ok = true;
    std::string bound_detail;
    // (a) pins 1-4 (bits 0-3), both connectors, never move fire0/fire1/POTINP.
    for (int bit = 0; bit <= 3; bit++) {
        reset_joys(); top->mouse_btn = 0; settle(4);
        joy1_bit(bit, true); settle(4);
        bool f0a = top->_fire0, f1a = top->_fire1; uint16_t pia = read_reg(POTINP);
        joy1_bit(bit, false); settle(4);
        joy2_bit(bit, true); settle(4);
        bool f0b = top->_fire0, f1b = top->_fire1; uint16_t pib = read_reg(POTINP);
        joy2_bit(bit, false); settle(4);
        bool ok = (f0a == fire0_base) && (f1a == fire1_base) && (pia == potinp_base) &&
                  (f0b == fire0_base) && (f1b == fire1_base) && (pib == potinp_base);
        if (!ok) bound_ok = false;
        bound_detail += "bit" + std::to_string(bit) + "[_joy1: fire0=" +
            std::to_string((int)f0a) + " fire1=" + std::to_string((int)f1a) +
            " POTINP=0x" + hex16(pia) + "; _joy2: fire0=" + std::to_string((int)f0b) +
            " fire1=" + std::to_string((int)f1b) + " POTINP=0x" + hex16(pib) + "] ";
    }
    // (b) POTINP's 12 hard-wired-0 bits stay 0 under a maximal stimulus sweep.
    {
        const uint16_t combos[] = {0x0000, 0xFFFF, 0xAAAA, 0x5555};
        const uint16_t const_zero_mask = (uint16_t)~((1u << 8) | (1u << 10) | (1u << 12) | (1u << 14));
        for (uint16_t c : combos) {
            joy1_val = c; joy2_val = c; top->_joy1 = joy1_val; top->_joy2 = joy2_val;
            top->mouse_btn = 0x07;
            write_reg(POTGO, 0xFFFF);
            settle(4);
            uint16_t pi = read_reg(POTINP);
            bool ok = (pi & const_zero_mask) == 0;
            if (!ok) bound_ok = false;
            bound_detail += "sweep(0x" + hex16(c) + ")=0x" + hex16(pi) + " ";
            write_reg(POTGO, 0x0001);
        }
        top->mouse_btn = 0;
        reset_joys(); settle(4);
    }
    note("2-buttons-max bound: " + bound_detail);
    expect(bound_ok, "sol:E-gameport.c3");
    expect(bound_ok, "opus:E-gameport.c4");
    expect(bound_ok, "sol:E-gameport.c4");
    expect(bound_ok, "opus:E-gameport.c5");

    // Positive control: pins 1-4 ARE live in this fixture (move JOY*DAT),
    // so (a) above is not vacuously true because the pins are unconnected.
    {
        reset_joys(); settle(4);
        uint16_t before = read_reg(JOY1DAT);
        joy1_bit(3, true); settle(4);
        uint16_t after = read_reg(JOY1DAT);
        note("PIN-GROUP-LIVENESS: _joy1 bit3 (pin1) pressed moves JOY1DAT "
             "from 0x" + hex16(before) + " to 0x" + hex16(after) +
             " -- pins 1-4 are live in this fixture, not simply unconnected");
        joy1_bit(3, false); settle(4);
    }

    // =====================================================================
    // DECLINED, with uid-free support -- claims 5 & 6: J11/J12's binding to
    // JOY1DAT/POT1DAT and JOY0DAT/POT0DAT. classify's own "split" field
    // concedes the connector DESIGNATOR (J11/J12, "right"/"left") is not
    // constructible here: no net in userio.v carries a schematic reference
    // designator or a physical side. What IS constructible, and measured
    // below on two uid-free lines, is the register-bank binding itself --
    // and per the file-header RTL facts, that binding is the OPPOSITE of
    // userio.v's own internal "Port 1"/"Port 2" comment labels, so it is
    // not a trivial pass-through to state.
    // =====================================================================
    {
        reset_joys(); settle(4);
        uint16_t j1_base = read_reg(JOY1DAT), j0_base = read_reg(JOY0DAT);
#ifndef MUT_joy1binding
        joy1_bit(3, true); joy1_bit(1, true); // distinguishing pattern on _joy1 only
#endif
        settle(4);
        uint16_t j1_after1 = read_reg(JOY1DAT), j0_after1 = read_reg(JOY0DAT);
        joy1_bit(3, false); joy1_bit(1, false); settle(4);

        bool joy1_moves_joy1dat = (j1_after1 != j1_base);
        bool joy1_leaves_joy0dat = (j0_after1 == j0_base);

        // POT1DAT's Y-gate half: with _mright held true (mouse_btn[1]=0,
        // idle/active-low), press _joy1 bit5 (pin9) and confirm POT1DAT's Y
        // byte advances while POT0DAT's does not.
        write_reg(POTGO, 0x0001); settle(4);
        top->mouse_btn = 0x00; settle(4); // _mright = ~0 = 1 (asserted)
        uint16_t pot1_before = read_reg(POT1DAT), pot0_before = read_reg(POT0DAT);
        joy1_bit(5, true); settle(4);
        for (int i = 0; i < 20; i++) { top->pot_cnt_en = 1; tick(); top->pot_cnt_en = 0; tick(); }
        uint16_t pot1_after = read_reg(POT1DAT), pot0_after = read_reg(POT0DAT);
        joy1_bit(5, false); settle(4);
        bool joy1_moves_pot1y = ((pot1_after >> 8) != (pot1_before >> 8));
        bool joy1_leaves_pot0y = ((pot0_after >> 8) == (pot0_before >> 8));

        bool joy1_binding_ok = joy1_moves_joy1dat && joy1_leaves_joy0dat &&
                                joy1_moves_pot1y && joy1_leaves_pot0y;
        note("PROP-JOY1-TO-JOY1DAT-POT1 detail: JOY1DAT " + hex16(j1_base) +
             "->" + hex16(j1_after1) + " (moved=" + std::to_string((int)joy1_moves_joy1dat) +
             "), JOY0DAT " + hex16(j0_base) + "->" + hex16(j0_after1) +
             " (unaffected=" + std::to_string((int)joy1_leaves_joy0dat) +
             "), POT1DAT.Y " + std::to_string(pot1_before >> 8) + "->" +
             std::to_string(pot1_after >> 8) + " (moved=" +
             std::to_string((int)joy1_moves_pot1y) + "), POT0DAT.Y " +
             std::to_string(pot0_before >> 8) + "->" + std::to_string(pot0_after >> 8) +
             " (unaffected=" + std::to_string((int)joy1_leaves_pot0y) + ")");
        expect(joy1_binding_ok, "PROP-JOY1-TO-JOY1DAT-POT1");

        record("sol:E-gameport.c6 (p.327, 'J11 is the right controller port "
               "connector (JOY1DAT, POT1DAT).') declined: the connector "
               "DESIGNATOR (J11) and the SIDE label (right) are not "
               "constructible at this fixture -- userio.v carries no "
               "schematic reference or physical-side fact for either of its "
               "two input nets, per classify's own split. What is "
               "constructible -- that the _joy1 net (module comment: "
               "'default mouse port') feeds JOY1DAT and POT1DAT's Y-gate, "
               "and NOT JOY0DAT/POT0DAT -- is measured above on the "
               "uid-free PROP-JOY1-TO-JOY1DAT-POT1 line. This fixture "
               "cannot say whether that net is physically wired to a "
               "connector marked J11, nor to whichever side of the case "
               "the manual calls 'right'.");
        record("opus:E-gameport.c8 (p.327, 'J11 is the right controller "
               "port connector (JOY1DAT, POT1DAT).') declined: same "
               "reasoning as sol:E-gameport.c6.");
    }
    {
        reset_joys(); settle(4);
        uint16_t j0_base = read_reg(JOY0DAT), j1_base = read_reg(JOY1DAT);
#ifndef MUT_joy2binding
        joy2_bit(2, true); joy2_bit(0, true); // distinguishing pattern on _joy2 only
#endif
        settle(4);
        uint16_t j0_after1 = read_reg(JOY0DAT), j1_after1 = read_reg(JOY1DAT);
        joy2_bit(2, false); joy2_bit(0, false); settle(4);

        bool joy2_moves_joy0dat = (j0_after1 != j0_base);
        bool joy2_leaves_joy1dat = (j1_after1 == j1_base);

        write_reg(POTGO, 0x0001); settle(4);
        top->mouse_btn = 0x00; settle(4);
        uint16_t pot0_before = read_reg(POT0DAT), pot1_before = read_reg(POT1DAT);
        joy2_bit(5, true); settle(4);
        for (int i = 0; i < 20; i++) { top->pot_cnt_en = 1; tick(); top->pot_cnt_en = 0; tick(); }
        uint16_t pot0_after = read_reg(POT0DAT), pot1_after = read_reg(POT1DAT);
        joy2_bit(5, false); settle(4);
        bool joy2_moves_pot0y = ((pot0_after >> 8) != (pot0_before >> 8));
        bool joy2_leaves_pot1y = ((pot1_after >> 8) == (pot1_before >> 8));

        bool joy2_binding_ok = joy2_moves_joy0dat && joy2_leaves_joy1dat &&
                                joy2_moves_pot0y && joy2_leaves_pot1y;
        note("PROP-JOY2-TO-JOY0DAT-POT0 detail: JOY0DAT " + hex16(j0_base) +
             "->" + hex16(j0_after1) + " (moved=" + std::to_string((int)joy2_moves_joy0dat) +
             "), JOY1DAT " + hex16(j1_base) + "->" + hex16(j1_after1) +
             " (unaffected=" + std::to_string((int)joy2_leaves_joy1dat) +
             "), POT0DAT.Y " + std::to_string(pot0_before >> 8) + "->" +
             std::to_string(pot0_after >> 8) + " (moved=" +
             std::to_string((int)joy2_moves_pot0y) + "), POT1DAT.Y " +
             std::to_string(pot1_before >> 8) + "->" + std::to_string(pot1_after >> 8) +
             " (unaffected=" + std::to_string((int)joy2_leaves_pot1y) + ")");
        expect(joy2_binding_ok, "PROP-JOY2-TO-JOY0DAT-POT0");

        record("sol:E-gameport.c7 (p.327, 'J12 is the left controller port "
               "connector (JOY0DAT, POT0DAT).') declined: same boundary "
               "reason as sol:E-gameport.c6 -- the connector DESIGNATOR "
               "(J12) and SIDE label (left) are not constructible here. "
               "What is constructible -- that the _joy2 net (module "
               "comment: 'default joystick port') feeds JOY0DAT and "
               "POT0DAT's Y-gate, and NOT JOY1DAT/POT1DAT -- is measured "
               "above on the uid-free PROP-JOY2-TO-JOY0DAT-POT0 line.");
        record("opus:E-gameport.c9 (p.327, 'J12 is the left controller port "
               "connector (JOY0DAT, POT0DAT).') declined: same reasoning as "
               "sol:E-gameport.c7.");
    }

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
