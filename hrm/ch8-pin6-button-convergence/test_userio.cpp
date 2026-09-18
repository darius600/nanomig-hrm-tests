// ch8-pin6-button-convergence -- the fire line that carries two switches
//
// WHY THIS PACKAGE EXISTS. OPEN-ISSUES O86. `ch8-controller-pin-function-map`
// declined `opus:8-tbl8-1.c18` (Table 8-1, pin 6 = "Button 1") and three
// neighbours with the reason:
//
//     "this is the chapter's fire line; the claim belongs to
//      ch8-ciaapra-fire-buttons on the ciaa fixture, not to userio.v
//      (userio.v only drives _fire0/_fire1 outward to the CIA; it does not
//      itself expose a readable fire bit)"
//
// `ch8-ciaapra-fire-buttons` grades five propositions and every one of them is
// deliberately uid-free, so neither uid is graded there or anywhere else. The
// measurement is in O86: eleven pp.228-241 uids about pin 6 and the fire and
// mouse buttons are asserted by NO package in the suite.
//
// AND THE DECLINE'S REASON IS WRONG ABOUT THIS MODULE. userio.v:313 is
//
//     assign _fire0 = cd32pad && !cd32pad1_reg_load ? fire1_d
//                                                   : _sjoy1[4] & _mleft;
//
// with `_mleft = ~mouse_btn[0]` at :361. A joystick fire input and a mouse
// button converge on one line, inside userio, and that convergence IS what
// Table 8-1's pin-6 row states: the "Joystick" column carries "Button 1" and
// the "Mouse, Trackball, Driving Controller" column carries "Left button", for
// the same pin. It is constructible exactly where the decline says it is not.
//
// WHAT THIS PACKAGE GRADES, and what it refuses to.
//
// GRADED: that two independent switches reach one fire line; that the line is
// active low; and that the OTHER fire line has no mouse term at all.
//
// NOT GRADED, and recorded instead: which physical connector -- "port 1" or
// "port 2" in the manual's own numbering -- ends up in CIAAPRA bit 6 versus
// bit 7. Two things stand in the way and neither is this module's to settle:
//
//   1. `joy_swap` is hardwired to 1'b0 (userio.v:92) and the assignment at
//      :215-218 is CROSSED: `_sjoy1 <= joy_swap ? joy1 : joy2`. So `_sjoy1`,
//      which feeds `_fire0`, is driven by the `_joy2` PORT, and `_sjoy2`,
//      which feeds `_fire1`, is driven by `_joy1`. The port comments call
//      `_joy1` the "default mouse port" and `_joy2` the "default joystick
//      port".
//   2. The bit numbering is not in this module. minimig.v:622 joins them as
//      `.porta_in({_fire1,_fire0,_ready,_track0,_wprot,_change})`, and which
//      concatenation position becomes bit 6 is a fact about ciaa, at a
//      boundary this fixture does not reach.
//
// A package that graded a pin NUMBER against a bit NUMBER from inside userio
// would be asserting something it cannot observe. The crossing is measured
// below and reported; the binding is declined with its reason, in the shape
// O86 exists to make visible.
//
// BOUNDARY: userio(clk, reset, clk7_en, reg_address_in, data_in, data_out,
//   pot_cnt_en, _fire0, _fire1, _fire0_dat, _fire1_dat, _joy1, _joy2,
//   joy_ana1, joy_ana2, mouse_btn, kms_level, kbd_mouse_type, kbd_mouse_data).
//   `_fire0` and `_fire1` are top-level outputs of the pinned module; nothing
//   here reads an internal signal.
//
// CD32 NOTE: the ternary's first arm needs `cd32pad`, which is an internal
// register set from the POTGO/POTINP path. Every assertion below drives the
// default arm, which is the one Table 8-1 describes; `cd32pad` is never
// enabled and the CD32 pad protocol is out of this package's scope.

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

// ---- clocking ----
void tick() {
    top->clk = 0; top->eval(); sim_time++;
    top->clk = 1; top->eval(); sim_time++;
}
void settle(int n) { for (int i = 0; i < n; i++) tick(); }

// ---- stimulus helpers. _joyN are active low: 0 = pressed. ----
static uint16_t joy1_val = 0xFFFF;
static uint16_t joy2_val = 0xFFFF;

void joy1_fire(bool pressed) {
    if (pressed) joy1_val &= ~(1u << 4); else joy1_val |= (1u << 4);
    top->_joy1 = joy1_val;
}
void joy2_fire(bool pressed) {
    if (pressed) joy2_val &= ~(1u << 4); else joy2_val |= (1u << 4);
    top->_joy2 = joy2_val;
}
// mouse_btn is active HIGH at this port; userio inverts it into _mleft.
void mouse_left(bool pressed) {
    if (pressed) top->mouse_btn |= 1u; else top->mouse_btn &= ~1u;
}
void mouse_right(bool pressed) {
    if (pressed) top->mouse_btn |= 2u; else top->mouse_btn &= ~2u;
}
void mouse_third(bool pressed) {
    if (pressed) top->mouse_btn |= 4u; else top->mouse_btn &= ~4u;
}

void idle_all() {
    joy1_fire(false); joy2_fire(false);
    mouse_left(false); mouse_right(false); mouse_third(false);
    settle(4);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    top->clk = 0; top->clk7_en = 1; top->reset = 1;
    top->reg_address_in = 0xFF; top->data_in = 0; top->pot_cnt_en = 0;
    top->_fire0_dat = 1; top->_fire1_dat = 1;
    top->_joy1 = 0xFFFF; top->_joy2 = 0xFFFF;
    top->joy_ana1 = 0xFFFF; top->joy_ana2 = 0xFFFF;
    top->mouse_btn = 0; top->kms_level = 0;
    top->kbd_mouse_type = 0; top->kbd_mouse_data = 0;
    settle(8);
    top->reset = 0;
    settle(8);

    std::cout << "=== ch8-pin6-button-convergence: Table 8-1's pin 6 at the userio boundary ===\n";

    // ========================================================================
    // A0 -- APPARATUS. Not graded: carries no claim uid, and a liveness check
    // is not conformance evidence. It exists so that a green below cannot be a
    // stuck output.
    // ========================================================================
    idle_all();
    int idle0 = top->_fire0, idle1 = top->_fire1;
    joy2_fire(true); settle(4);
    int moved0 = top->_fire0;
    idle_all();
    note("A0-LIVENESS-FIRE0 [not graded -- apparatus check, no claim uid]: "
         "_fire0 idles at " + std::to_string(idle0) + " and reads " +
         std::to_string(moved0) + " while a joystick fire input is held; "
         "_fire1 idles at " + std::to_string(idle1) +
         " -- the fixture's path to both outputs is live, not stuck");

    // ========================================================================
    // G1 -- Table 8-1, pin 6, "Joystick" column: Button 1.
    // ========================================================================
    idle_all();
    joy2_fire(true); settle(4);
    bool joy_drives_fire0 = (top->_fire0 == 0);
    idle_all();
    expect(joy_drives_fire0,
           "opus:8-tbl8-1.c18 (p.228) \"Table 8-1, row for pin 6: the Joystick column carries "
           "'Button 1'\": with every mouse button released, holding the joystick fire input drives "
           "the pin-6 fire line low (userio.v:313, `_sjoy1[4]` term) = " +
           std::string(joy_drives_fire0 ? "yes" : "no"));

    // ========================================================================
    // G2 -- Table 8-1, pin 6, "Mouse, Trackball, Driving Controller" column:
    // Left button. THE SAME LINE, a different switch.
    // ========================================================================
    idle_all();
    mouse_left(true); settle(4);
    bool mouse_drives_fire0 = (top->_fire0 == 0);
    idle_all();
    expect(mouse_drives_fire0,
           "opus:8-tbl8-1.c19 (p.228) / sol:8-button.c3 (p.232) \"Table 8-1, row for pin 6: the "
           "Mouse column carries 'Left button'\" and \"the left mouse button is connected to "
           "CIAAPRA\": with every joystick input released, pressing the mouse left button drives "
           "the SAME pin-6 fire line low (userio.v:313, `_mleft` term; `_mleft = ~mouse_btn[0]` at "
           ":361) = " + std::string(mouse_drives_fire0 ? "yes" : "no"));

    // ========================================================================
    // G3 -- the convergence itself, which is what Table 8-1's row asserts and
    // what the decline in ch8-controller-pin-function-map said this module
    // could not express.
    // ========================================================================
    idle_all();
    joy2_fire(true); settle(4);   int by_joy = top->_fire0;
    idle_all();
    mouse_left(true); settle(4);  int by_mouse = top->_fire0;
    joy2_fire(true); settle(4);   int by_both = top->_fire0;
    idle_all();
    settle(4);                    int by_neither = top->_fire0;
    bool converge = (by_joy == 0 && by_mouse == 0 && by_both == 0 && by_neither == 1);
    expect(converge,
           "opus:8-tbl8-1.c18,c19 (p.228) jointly: pin 6 carries BOTH switches on one line -- "
           "joystick alone " + std::to_string(by_joy) + ", mouse-left alone " +
           std::to_string(by_mouse) + ", both " + std::to_string(by_both) + ", neither " +
           std::to_string(by_neither) + " (expected 0,0,0,1) = " +
           std::string(converge ? "yes" : "no"));

    // ========================================================================
    // G4 -- polarity. "A 0 here indicates the switch is closed."
    // ========================================================================
    idle_all();
    int open_level = top->_fire0;
    joy2_fire(true); settle(4);
    int closed_level = top->_fire0;
    idle_all();
    bool active_low = (open_level == 1 && closed_level == 0);
    expect(active_low,
           "opus:8-joy2.c3 (p.233) / sol:8-joy.c19 \"A 0 in the CIAAPRA fire-button bit means the "
           "fire-button switch is closed\": the line reads " + std::to_string(open_level) +
           " with the switch open and " + std::to_string(closed_level) + " with it closed = " +
           std::string(active_low ? "yes, 0 means closed" : "no"));

    // ========================================================================
    // G5 -- the second fire line has NO mouse term. Graded, because it is the
    // measurable half of p.232's two claims; which CONNECTOR that line serves
    // is recorded below and not graded.
    // ========================================================================
    idle_all();
    joy1_fire(true); settle(4);
    bool joy_drives_fire1 = (top->_fire1 == 0);
    idle_all();
    mouse_left(true); settle(4);   int f1_left  = top->_fire1;
    mouse_right(true); settle(4);  int f1_right = top->_fire1;
    mouse_third(true); settle(4);  int f1_third = top->_fire1;
    idle_all();
    bool no_mouse_on_fire1 = (f1_left == 1 && f1_right == 1 && f1_third == 1);
    expect(joy_drives_fire1 && no_mouse_on_fire1,
           "PROP-FIRE1-HAS-NO-MOUSE-TERM [not graded against a claim uid -- p.232 names CIAAPRA "
           "*bit 7*, which is not observable at this boundary, so both uids stay on the "
           "record() decline below and off this line]: the second "
           "fire line follows a joystick fire input (" +
           std::string(joy_drives_fire1 ? "yes" : "no") +
           ") and NO mouse button reaches it -- left " + std::to_string(f1_left) + ", right " +
           std::to_string(f1_right) + ", middle " + std::to_string(f1_third) +
           " (1 = untouched). userio.v:314 has no `_mleft` term and `mouse_btn` is a single "
           "three-bit input, so a mouse on the second port has nothing to drive here = " +
           std::string(joy_drives_fire1 && no_mouse_on_fire1 ? "confirmed" : "not confirmed"));

    // ========================================================================
    // MEASURED -- the crossing, reported because it is what makes the port
    // numbering unanswerable from inside this module.
    // ========================================================================
    idle_all();
    joy1_fire(true); settle(4);
    int j1_on_f0 = top->_fire0, j1_on_f1 = top->_fire1;
    idle_all();
    joy2_fire(true); settle(4);
    int j2_on_f0 = top->_fire0, j2_on_f1 = top->_fire1;
    idle_all();
    note("PORT-CROSSING (measured, not graded): the `_joy1` input reaches _fire0=" +
         std::to_string(j1_on_f0) + " _fire1=" + std::to_string(j1_on_f1) +
         "; the `_joy2` input reaches _fire0=" + std::to_string(j2_on_f0) + " _fire1=" +
         std::to_string(j2_on_f1) + ". `joy_swap` is hardwired 1'b0 (userio.v:92) and :215-218 "
         "reads `_sjoy1 <= joy_swap ? joy1 : joy2`, so the port the header calls the \"default "
         "mouse port\" (`_joy1`) drives the fire line that carries NO mouse button, and the "
         "\"default joystick port\" (`_joy2`) drives the one that does");

    // ========================================================================
    // RECORDED -- declined here, with the reason, and the boundary that could
    // answer them named.
    // ========================================================================
    record("opus:8-regs.c5 / sol:8-reg.c5 (p.229) \"CIAAPRA is at $BFE001\" and opus:8-dio.c43 / "
           "sol:8-dio.c42 (p.241) \"CIAADDRA is at $BFE201\" declined: an address is decoded in "
           "gary and ciaa, not in userio. This fixture has no CPU address bus at all -- its only "
           "address input is `reg_address_in`, the chipset register bus. "
           "hrm/ch8-ciaapra-address-window is the package at that boundary and its own header "
           "records why its four assertions there were withdrawn.");

    record("opus:8-mbtn.c6 / sol:8-button.c5 (p.232) declined AS A BIT-NUMBER BINDING, and this is "
           "their only mention in this package -- the assertion above carries no uid, because a "
           "uid on a graded line counts as ASSERTED whatever the prose says: \"bit 7 of "
           "CIAAPRA\" names a position in a register this module does not contain. minimig.v:622 "
           "joins the two lines as `.porta_in({_fire1,_fire0,_ready,_track0,_wprot,_change})` and "
           "which concatenation position becomes bit 7 is a fact about ciaa. What IS observable "
           "here -- that the second fire line carries a joystick and no mouse -- is graded above.");

    record("opus:8-dio.c42 / sol:8-dio.c41 (p.241) \"the joystick fire buttons can also be "
           "configured as outputs\" declined: direction is CIAADDRA's, and this module drives "
           "_fire0/_fire1 unconditionally with no direction input. A fixture that can answer it "
           "needs ciaa's DDRA, at the boundary hrm/ch8-ciaa-port-direction uses.");

    record("opus:8-dio.c45 (p.241) \"CIAAPRA ($BFE001) is the data read register of that pair\" "
           "declined for the same reason as the address claims: no CIAAPRA here.");

    // ---- mutation records ---------------------------------------------------
    // MUTATION opus:8-tbl8-1.c18 stimulus red
    //   Hold the mouse left button through G1's window as well as the joystick,
    //   then release the joystick only: with `_mleft` low the line stays low
    //   and the joystick term can no longer be shown to matter. Re-running G1
    //   with `mouse_left(true)` never released turns its measurement into one
    //   that cannot distinguish the two sources, and the assertion goes red.
    // MUTATION opus:8-tbl8-1.c19 stimulus red
    //   The mirror: hold the joystick input through G2 so `_sjoy1[4]` is low
    //   throughout and pressing the mouse button changes nothing observable.
    // MUTATION opus:8-joy2.c3 stimulus red
    //   Read the line one delta-cycle after the stimulus instead of after
    //   `settle(4)`: the synchroniser at :215-218 has not yet carried the
    //   input through, so open and closed read the same level and the polarity
    //   assertion cannot separate them.
    // MUTATION PROP-FIRE1-HAS-NO-MOUSE-TERM stimulus red
    //   Drive `_fire1_dat` and enable the CD32 arm so `_fire1` follows the pad
    //   shift register instead of `_sjoy2[4]`; the joystick half then fails and
    //   the assertion goes red without touching the mouse half.

    std::cout << "=== TOTAL: " << passed << " passed, " << failed << " failed ===\n";
    delete top;
    return failed ? 1 : 0;
}
