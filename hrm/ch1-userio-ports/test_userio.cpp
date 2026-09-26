// ch1-userio-ports -- userio conformance test
//
// Test purpose: grade page 2's "Two button opto-mechanical mouse and two
//   reconfigurable controller ports (for mice, joysticks, light pens,
//   paddles, or custom controllers)." and page 6's "The Amiga also includes
//   a two-button, opto-mechanical mouse plus a keyboard with numeric keypad,
//   cursor controls, and 10 function keys in the base system." / "Other
//   types of controllers can be attached through the two controller ports on
//   the base unit including joysticks, keypads, trackballs, light pens, and
//   graphics tablets." against `userio` (userio.v). SPECIFICATION.md
//   (committed before this file or userio.v were opened -- commit 2c11fe2)
//   records the reasoning in full; this file carries it out and states the
//   final, RTL-confirmed disposition.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 2 and 6.
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM COVERAGE: all six uids assigned to this package (sol/opus x
//   1-components.c18, c128, c131) are declined below, on record() lines --
//   none is graded with expect() here.
//
// Why, in one paragraph (full reasoning and the required grep evidence are
//   in SPECIFICATION.md): every structurally-gradeable component of these
//   three sentences that this suite can test at all is already tested, at
//   this identical userio.v-alone boundary, by two accepted chapter-8
//   packages -- ch8-controller-port-inventory (two independent ports
//   exist; ports accept mouse/joystick/paddle/trackball input -- all PASS
//   at baseline) and ch8-mouse-button-pins (the module accepts at least
//   three independent button-input lines, which strictly entails "two
//   button mouse" -- PASS at baseline). The remaining device names these
//   three sentences add ("custom controllers", "keypads", "graphics
//   tablets") have no defined electrical counterpart anywhere in this
//   suite's corpus (confirmed against ch8-controller-pin-function-map's
//   own transcription of Table 8-1, the manual's own pin-function map,
//   which names exactly five device-type columns and none of these three).
//   Light pens are named on both pages but, confirmed now directly against
//   userio.v's full text (grep for "ltpen|lpen|light|beam|pen\b": zero
//   hits), carry no signal anywhere on this module's boundary -- out of
//   scope for this fixture. The keyboard half of the page-6 sentence is
//   also out of this module's boundary: userio.v's only keyboard-adjacent
//   ports (kms_level, kbd_mouse_type, kbd_mouse_data) carry an auxiliary
//   MOUSE-position feed arriving over the keyboard's serial link, not key
//   identity -- confirmed by reading their only use (userio.v's mouse-
//   counter block advances xcount/ycount/mouse0scr from kbd_mouse_data),
//   which has nothing to do with a numeric keypad, cursor controls, or
//   function keys.
//
// FIXTURE-LIVENESS below carries no claim uid -- a harness precondition
//   (build brief: "the fixture proves it can deliver a known value before
//   anything reads one"), so a record()-only package still rests on a
//   proven-live fixture rather than an unproven or silently-broken one.

#include "Vuserio.h"
#include "verilated.h"
#include <cstdint>
#include <iostream>
#include <string>

namespace {

struct Checks {
    int passed = 0;
    int failed = 0;
};

void expect(Checks& c, bool cond, const std::string& label) {
    if (cond) {
        c.passed++;
        std::cout << "PASS: " << label << "\n";
    } else {
        c.failed++;
        std::cout << "FAIL: " << label << "\n";
    }
}

void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// register address, from userio.v's own parameter block (userio.v:54)
const uint16_t JOY0DAT = 0x00a;

Vuserio* top;

void tick() {
    top->clk = 0;
    top->eval();
    top->clk = 1;
    top->eval();
}

void settle(int n) {
    for (int i = 0; i < n; i++) tick();
}

uint16_t read_reg(uint16_t addr9) {
    top->reg_address_in = (addr9 >> 1) & 0xFF;
    top->eval();
    return top->data_out;
}

void reset_all() {
    top->reset = 1;
    top->clk7_en = 1;
    top->pot_cnt_en = 0;
    top->_fire0_dat = 1;
    top->_fire1_dat = 1;
    top->_joy1 = 0x0000;
    top->_joy2 = 0x0000;
    top->joy_ana1 = 0;
    top->joy_ana2 = 0;
    top->mouse_btn = 0;
    top->kms_level = 0;
    top->kbd_mouse_type = 0;
    top->kbd_mouse_data = 0;
    top->data_in = 0;
    settle(5);
    top->reset = 0;
    settle(5);
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;
    Checks checks;

    // =========================================================================
    // FIXTURE-LIVENESS (no uid; harness precondition). Port 1's default
    // state after reset is mouse mode (userio.v:224, "if (!_mleft ||
    // reset) joy1enable = 0;"), and JOY0DAT's mouse-quadrature decode is
    // driven from _sjoy1, which tracks the _joy2 pins, not _joy1
    // (userio.v:215-217's "_sjoy1 <= joy_swap ? joy1 : joy2;" with
    // joy_swap tied to constant 0 at userio.v:92) -- the port-1-register-
    // reads-_joy2-pins reversal ch8-controller-port-io-path's
    // SPECIFICATION.md already flagged for this exact fixture, confirmed
    // again here directly. Driving a known 6-step quadrature sequence on
    // _joy2 bits 2/0 (the same two bits, same fixture, same direction
    // convention ch8-controller-port-inventory's test_userio.cpp already
    // calibrated and proved PASS) and reading JOY0DAT demonstrates the
    // fixture delivers a known, distinct value on a path independent of
    // any of this package's six declined claims, before any record() line
    // below is trusted.
    // =========================================================================
    {
        reset_all();
        uint16_t before = read_reg(JOY0DAT);

        static const int hi_t[4] = {0, 0, 1, 1};
        static const int lo_t[4] = {0, 1, 1, 0};
        int idx = 0;
#ifdef MUT_FIXTURE_LIVENESS
        // mutated: withhold the stimulus entirely -- idx never advances,
        // _joy2 stays at its idle value the whole time.
#else
        for (int i = 0; i < 6; i++) {
            idx = (idx + 3) & 3; // measured direction that increments this counter
            uint16_t v = top->_joy2;
            if (hi_t[idx]) v |= (1 << 2); else v &= ~(uint16_t)(1 << 2);
            if (lo_t[idx]) v |= (1 << 0); else v &= ~(uint16_t)(1 << 0);
            top->_joy2 = v;
            tick();
        }
        tick(); // flush the two-stage input synchroniser
#endif
        uint16_t after = read_reg(JOY0DAT);
        int8_t delta = (int8_t)((after & 0xFF) - (before & 0xFF));
        note("FIXTURE-LIVENESS: JOY0DAT before=" + std::to_string(before) +
             " after=" + std::to_string(after) +
             " delta=" + std::to_string((int)delta));
        expect(checks, delta == 6,
               "FIXTURE-LIVENESS: a known 6-step quadrature stimulus on the _joy2 "
               "pins arrives at JOY0DAT's mouse-position counter as delta=+6, "
               "before any claim below is graded. No manual page (harness "
               "precondition).");
    }
    // MUTATION FIXTURE-LIVENESS stimulus red MUT_FIXTURE_LIVENESS: withhold the quadrature stimulus entirely (leave _joy2 at its idle value the whole time) so JOY0DAT never advances -- baseline PASS goes red.

    note("userio.v read in full (365 lines) for step 2. grep for "
         "\"ltpen|lpen|light|beam|pen\\b\": zero hits -- no light-pen signal "
         "anywhere on this module's boundary. The module does carry a disabled "
         "CD32-pad shift-register path (cd32pad, userio.v:91, tied to constant "
         "0, \"currently disabled but may become used, again\"), which is a "
         "different, specific, named controller type -- not what page 2's "
         "generic \"custom controllers\" or page 6's \"keypads\"/\"graphics "
         "tablets\" describe, and not itself named by either page.");

    // =========================================================================
    // Declined claims (record() only -- no expect(), no MUTATION line, per
    // the build brief: a MUTATION line's second token is the leading token
    // of a label passed to expect(), and none of these three is).
    // =========================================================================

    record(
        "PAGE2-MOUSE-AND-PORTS (p.2): \"Two button opto-mechanical mouse and "
        "two reconfigurable controller ports (for mice, joysticks, light "
        "pens, paddles, or custom controllers).\" Every structurally-"
        "gradeable component is already graded on this identical userio.v-"
        "alone boundary by an accepted package: two independent ports exist "
        "(ch8-controller-port-inventory, two_independent_ports[opus:8-intro."
        "c1,opus:8-intro.c8,sol:8-overview.c1], PASS); ports accept mice "
        "(accepts_mouse_quadrature[sol:8-overview.c4], PASS); ports accept "
        "joysticks (accepts_joystick_switches[sol:8-overview.c5], PASS); "
        "ports accept paddles (accepts_paddle_timing[sol:8-overview.c7,opus:"
        "8-intro.c13], PASS); the mouse has (at least) two independently-"
        "effective buttons, strictly entailed by ch8-mouse-button-pins's own "
        "PASS that (at least) three are independently effective (P1-BIT0-"
        "DISTINCT-EFFECT/P1-COMPLEMENT-REMOVAL-DISTINGUISHES-BITS, citing "
        "opus:8-mbtn.c2/sol:8-button.c2). 'Opto-mechanical' is a mouse-"
        "internal sensing technology, outside any RTL. 'Light pens': "
        "confirmed above (FIXTURE-LIVENESS note) -- no signal on this "
        "module's boundary at all; ch8-controller-port-inventory's own spec "
        "independently found a light pen's substantive function to be 'a "
        "different signal path than the joystick/mouse pins', and "
        "ch8-controller-pin-function-map's Group C independently found a "
        "light pen indistinguishable from an ordinary button at the pins "
        "that are shared. 'Custom controllers': not one of Table 8-1's five "
        "device-type columns (ch8-controller-pin-function-map) and no "
        "mechanism for it is defined anywhere in this suite's corpus -- "
        "source-silent, not gradeable, not invented. Declined in full, not "
        "re-asserted. [claims: sol:1-components.c18, opus:1-components.c18]");

    record(
        "PAGE6-MOUSE-AND-KEYBOARD (p.6): \"The Amiga also includes a "
        "two-button, opto-mechanical mouse plus a keyboard with numeric "
        "keypad, cursor controls, and 10 function keys in the base system.\" "
        "Mouse half: identical proposition to PAGE2-MOUSE-AND-PORTS above, "
        "same disposition (duplicate of ch8-mouse-button-pins's PASS). "
        "Keyboard half: out of this module's boundary. userio.v's full port "
        "list (read in full for step 2) has no key-identity input of any "
        "kind. Its only keyboard-adjacent ports -- kms_level, "
        "kbd_mouse_type, kbd_mouse_data (userio.v:47-49) -- are read, "
        "confirmed by their only use (userio.v:330-355's mouse-counter "
        "block), to carry an auxiliary MOUSE-motion feed arriving over the "
        "keyboard's serial link (xcount/ycount/mouse0scr advance from "
        "kbd_mouse_data when kms_level toggles), not a numeric keypad, "
        "cursor controls, or function-key identity. The physical keyboard's "
        "own key decode is a different module entirely (keymap/"
        "amiga_keymap.v, graded, on its own fixture, by the sibling package "
        "ch1-keyboard-translation for the near-identical page-2 sentence "
        "1-components.c19 -- a different bare id, not this claim, and not "
        "cited here as covering c128, only as evidence that the keyboard "
        "subject lives on a fixture this package does not touch). Declined "
        "in full: mouse half as duplicate, keyboard half as out-of-boundary "
        "for this fixture. [claims: sol:1-components.c128, "
        "opus:1-components.c128]");

    record(
        "PAGE6-OTHER-CONTROLLERS (p.6): \"Other types of controllers can be "
        "attached through the two controller ports on the base unit "
        "including joysticks, keypads, trackballs, light pens, and graphics "
        "tablets.\" Two ports exist: duplicate of "
        "two_independent_ports[opus:8-intro.c1,opus:8-intro.c8,sol:8-"
        "overview.c1] (restated). Joysticks: duplicate of "
        "accepts_joystick_switches[sol:8-overview.c5]. Trackballs: "
        "duplicate of accepts_trackball_quadrature[sol:8-overview.c8,opus:"
        "8-intro.c14] (electrically identical interface to the mouse, "
        "graded there as its own independent measurement). Light pens: same "
        "finding as PAGE2-MOUSE-AND-PORTS above -- no signal on this "
        "module's boundary. Keypads and graphics tablets: neither is one of "
        "Table 8-1's five device-type columns and neither has any defined "
        "electrical mechanism anywhere in this suite's corpus -- "
        "source-silent, not gradeable, not invented (inventing one, e.g. "
        "assuming a 'keypad' behaves exactly like the already-tested "
        "joystick switches, is exactly the mechanism-the-page-never-asked-"
        "for inflation the build brief's chapter-1 guidance warns against). "
        "Declined in full, not re-asserted. [claims: sol:1-components.c131, "
        "opus:1-components.c131]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
