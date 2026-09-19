// Test purpose: Printed page: 227 (Amiga Hardware Reference Manual, chapter
//   8).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 227, 233, 240
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim outside the 10 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 10 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// ch8-controller-port-inventory -- printed page 227.
//
// Subject: two controller ports exist as independent boundaries, and both
// accept the device kinds page 227 lists (mouse, joystick, paddle,
// trackball -- light pens are named on the page but covered by no claim in
// this package's ten and are not tested).
//
// Fixture: userio.v, compiled alone. "Connector A" is the port whose
// register is JOY0DAT/POT0DAT; "connector B" is the port whose register is
// JOY1DAT/POT1DAT. Measuring which external pins (_joy1 vs _joy2) drive
// which register is itself part of what this package establishes (see
// PHASE P1/P2) -- SPECIFICATION.md deliberately does not name them, and the
// two labels are this package's own bookkeeping, chosen to sidestep KNOWN
// CONFLICT 3 (printed p.233 numbers the ports 0/1, printed p.240 and the
// rest of the chapter number them 1/2; neither of those numberings is
// asserted here).
//
// MUTATION port_carries_input[sol:8-overview.c2] stimulus red do not step connector A at all (send the same idle word the whole time) so the +5 word never arrives
// MUTATION two_independent_ports[opus:8-intro.c1,opus:8-intro.c8,sol:8-overview.c1] stimulus red also step connector B while connector A is being stepped, so the "B unaffected" half of the measurement is false
// MUTATION accepts_mouse_quadrature[sol:8-overview.c4] stimulus red step connector A's low-byte pair in the decrementing order for the leg the assertion expects to be incrementing
// MUTATION accepts_joystick_switches[sol:8-overview.c5] stimulus red drive the second static combination with the same bit pattern as the first, so two combinations that must read back distinctly no longer do
// MUTATION accepts_paddle_timing[sol:8-overview.c7,opus:8-intro.c13] stimulus red leave connector A's pot sense pin released during the pulses that should be counted, instead of asserted
// MUTATION accepts_trackball_quadrature[sol:8-overview.c8,opus:8-intro.c14] stimulus red step connector B's low-byte pair in the decrementing order for the leg the assertion expects to be incrementing

#include "Vuserio.h"
#include "verilated.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

// ---- fixed assertion harness (brief: expect/note/record, no substitutes) ----
struct Checks {
    int passed = 0;
    int failed = 0;
} checks;

void expect(bool cond, const std::string& label) {
    if (cond) {
        checks.passed++;
        std::cout << "PASS: " << label << "\n";
    } else {
        checks.failed++;
        std::cout << "FAIL: " << label << "\n";
    }
}

void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }

void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// ---- register addresses, read from userio.v's own parameter block ----
static const uint16_t JOY0DAT = 0x00a;
static const uint16_t JOY1DAT = 0x00c;
static const uint16_t POT0DAT = 0x012;
static const uint16_t POT1DAT = 0x014;

static Vuserio* top;

static void tick() {
    top->clk = 0;
    top->eval();
    top->clk = 1;
    top->eval();
}

static void settle(int n) {
    for (int i = 0; i < n; i++) tick();
}

static uint16_t read_reg(uint16_t addr9) {
    top->reg_address_in = (addr9 >> 1) & 0xFF;
    top->eval();
    return top->data_out;
}

static void reset_all() {
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

// One quadrature Gray step on a connector's (bitHi,bitLo) pair.
// idx cycles 0,1,2,3 -> (0,0),(0,1),(1,1),(1,0). Advancing idx FORWARD
// (0->1->2->3) was measured to DECREMENT the counter; advancing it BACKWARD
// (0->3->2->1) was measured to INCREMENT it (calibrated empirically against
// this exact fixture before this file was written, matching the two Gray
// rings Figure 8-2 draws).
static void quad_step(bool isJoy2, int bitHi, int bitLo, int& idx, bool forward) {
    static const int hi_t[4] = {0, 0, 1, 1};
    static const int lo_t[4] = {0, 1, 1, 0};
    idx = forward ? (idx + 1) & 3 : (idx + 3) & 3;
    uint16_t v = isJoy2 ? top->_joy2 : top->_joy1;
    if (hi_t[idx]) v |= (1 << bitHi); else v &= ~(uint16_t)(1 << bitHi);
    if (lo_t[idx]) v |= (1 << bitLo); else v &= ~(uint16_t)(1 << bitLo);
    if (isJoy2) top->_joy2 = v; else top->_joy1 = v;
    tick();
}

// Step N times in the INCREMENTING direction (idx 0->3->2->1->0...), then
// one extra settle tick to flush the two-stage input synchroniser (measured
// necessary and sufficient on this fixture: a pipeline of N steps needs
// exactly one more tick than N before the last transition is visible).
static void step_n_increment(bool isJoy2, int bitHi, int bitLo, int& idx, int n) {
    for (int i = 0; i < n; i++) quad_step(isJoy2, bitHi, bitLo, idx, /*forward=*/false);
    tick();
}
static void step_n_decrement(bool isJoy2, int bitHi, int bitLo, int& idx, int n) {
    for (int i = 0; i < n; i++) quad_step(isJoy2, bitHi, bitLo, idx, /*forward=*/true);
    tick();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    // ---------------------------------------------------------------
    // PHASE: idle baseline (not a manual claim -- background orientation)
    // ---------------------------------------------------------------
    reset_all();
    uint16_t idle0 = read_reg(JOY0DAT);
    uint16_t idle1 = read_reg(JOY1DAT);
    note("idle JOY0DAT=" + std::to_string(idle0) + " JOY1DAT=" + std::to_string(idle1) +
         " (both undriven _joy1/_joy2 default to 0 under Verilator, the PRESSED"
         " level on this active-low interface)");

    // ---------------------------------------------------------------
    // PHASE P2 -- port_carries_input [sol:8-overview.c2]
    // "Each controller connector can carry input": drive connector A (the
    // _joy2 pins) with a known, distinct stimulus and confirm the known word
    // arrives at connector A's own register, before anything else is
    // graded. This is the package's control/precondition case: it rules out
    // a dead data path that would otherwise make PHASE P1's independence
    // result a false clean pass (both sides unmoved because neither works).
    // ---------------------------------------------------------------
    {
        reset_all();
        uint16_t before = read_reg(JOY0DAT);
        int idx = 0;
#ifdef MUT_port_carries_input
        step_n_increment(/*isJoy2=*/true, 2, 0, idx, 0); // mutated: stimulus withheld
#else
        step_n_increment(/*isJoy2=*/true, 2, 0, idx, 5);
#endif
        uint16_t after = read_reg(JOY0DAT);
        int8_t delta = (int8_t)((after & 0xFF) - (before & 0xFF));
        note("connector A driven with 5 known steps: before=" + std::to_string(before) +
             " after=" + std::to_string(after) + " delta=" + std::to_string((int)delta));
        expect(delta == 5,
               "port_carries_input[sol:8-overview.c2]");
    }

    // ---------------------------------------------------------------
    // PHASE P1 -- two_independent_ports
    //   [opus:8-intro.c1,opus:8-intro.c8,sol:8-overview.c1]
    // "Each Amiga has two ... connectors": a stimulus delivered through one
    // must not appear as a change of state attributable to the other.
    // Measured both directions in one fresh, self-contained experiment.
    // ---------------------------------------------------------------
    {
        reset_all();
        uint16_t base0 = read_reg(JOY0DAT);
        uint16_t base1 = read_reg(JOY1DAT);

        int idxA = 0;
        step_n_increment(/*isJoy2=*/true, 2, 0, idxA, 6); // drive connector A (_joy2)
#ifdef MUT_two_independent_ports
        int idxB_leak = 0;
        step_n_increment(/*isJoy2=*/false, 2, 0, idxB_leak, 6); // mutated: also drive B here
#endif
        uint16_t afterA_onA = read_reg(JOY0DAT);
        uint16_t afterA_onB = read_reg(JOY1DAT);

        int idxB = 0;
        step_n_increment(/*isJoy2=*/false, 2, 0, idxB, 4); // drive connector B (_joy1)
        uint16_t afterB_onB = read_reg(JOY1DAT);
        uint16_t afterB_onA = read_reg(JOY0DAT);

        int8_t deltaA_own = (int8_t)((afterA_onA & 0xFF) - (base0 & 0xFF));
        int8_t deltaB_own = (int8_t)((afterB_onB & 0xFF) - (afterA_onB & 0xFF));
        bool a_moved_alone = (deltaA_own == 6) && (afterA_onB == base1);
        bool b_moved_alone = (deltaB_own == 4) && (afterB_onA == afterA_onA);

        note("connector A step: JOY0DAT " + std::to_string(base0) + "->" + std::to_string(afterA_onA) +
             ", JOY1DAT meanwhile " + std::to_string(base1) + "->" + std::to_string(afterA_onB));
        note("connector B step: JOY1DAT " + std::to_string(afterA_onB) + "->" + std::to_string(afterB_onB) +
             ", JOY0DAT meanwhile " + std::to_string(afterA_onA) + "->" + std::to_string(afterB_onA));

        expect(a_moved_alone && b_moved_alone,
               "two_independent_ports[opus:8-intro.c1,opus:8-intro.c8,sol:8-overview.c1]");
    }

    // ---------------------------------------------------------------
    // PHASE P3 -- accepts_mouse_quadrature [sol:8-overview.c4]
    // A mouse reports motion as a two-phase pulse train; connector A must
    // accumulate a count whose sign follows which phase leads. One
    // connector, one axis, both directions, its own stimulus and readback.
    // ---------------------------------------------------------------
    {
        reset_all();
        uint16_t base = read_reg(JOY0DAT);
        int idx = 0;
#ifdef MUT_accepts_mouse_quadrature
        step_n_decrement(/*isJoy2=*/true, 2, 0, idx, 7); // mutated: wrong-direction leg
#else
        step_n_increment(/*isJoy2=*/true, 2, 0, idx, 7);
#endif
        uint16_t up = read_reg(JOY0DAT);
        int8_t deltaUp = (int8_t)((up & 0xFF) - (base & 0xFF));

        step_n_decrement(/*isJoy2=*/true, 2, 0, idx, 7);
        uint16_t down = read_reg(JOY0DAT);
        int8_t deltaDown = (int8_t)((down & 0xFF) - (up & 0xFF));

        note("connector A quadrature: base=" + std::to_string(base) + " +7-> " + std::to_string(up) +
             " (delta " + std::to_string((int)deltaUp) + ") -7-> " + std::to_string(down) +
             " (delta " + std::to_string((int)deltaDown) + ")");

        expect(deltaUp == 7 && deltaDown == -7,
               "accepts_mouse_quadrature[sol:8-overview.c4]");
    }

    // ---------------------------------------------------------------
    // PHASE P4 -- accepts_joystick_switches [sol:8-overview.c5]
    // A digital joystick reports discrete, static switch state, not a
    // moving count. Four static combinations on connector A's low-byte pair
    // must read back four mutually distinct values (the module actually
    // discriminates switch position), not just "something changed".
    // ---------------------------------------------------------------
    {
        reset_all();
        auto set_static = [](int bitHi, int bitLo, int hi, int lo) {
            uint16_t v = top->_joy2;
            if (hi) v |= (1 << bitHi); else v &= ~(uint16_t)(1 << bitHi);
            if (lo) v |= (1 << bitLo); else v &= ~(uint16_t)(1 << bitLo);
            top->_joy2 = v;
            settle(5);
        };
        set_static(2, 0, 0, 0);
        uint16_t c0 = read_reg(JOY0DAT) & 0x3;
        set_static(2, 0, 0, 1);
        uint16_t c1 = read_reg(JOY0DAT) & 0x3;
        set_static(2, 0, 1, 1);
#ifdef MUT_accepts_joystick_switches
        set_static(2, 0, 0, 1); // mutated: repeat c1's pattern instead of a new one
#else
        set_static(2, 0, 1, 0);
#endif
        uint16_t c3 = read_reg(JOY0DAT) & 0x3;
        // recompute c2 fresh from the actually-driven third combination
        set_static(2, 0, 1, 1);
        uint16_t c2 = read_reg(JOY0DAT) & 0x3;

        note("connector A static combos, JOY0DAT[1:0]: (0,0)=" + std::to_string(c0) +
             " (0,1)=" + std::to_string(c1) + " (1,1)=" + std::to_string(c2) +
             " (1,0)-slot=" + std::to_string(c3));

        bool all_distinct = (c0 != c1) && (c0 != c2) && (c0 != c3) &&
                             (c1 != c2) && (c1 != c3) && (c2 != c3);
        expect(all_distinct,
               "accepts_joystick_switches[sol:8-overview.c5]");
    }

    // ---------------------------------------------------------------
    // PHASE P5 -- accepts_paddle_timing
    //   [sol:8-overview.c7,opus:8-intro.c13]
    // A paddle's position is timed as a count of a periodic enable pulse,
    // gated by whether the paddle's sense pin is still "charging" (held) or
    // has released. Tested on both connectors, each with its own pulse
    // count, each first proven silent while released.
    // ---------------------------------------------------------------
    {
        reset_all();
        auto pot_pulse = [](int n) {
            for (int i = 0; i < n; i++) {
                top->pot_cnt_en = 1;
                tick();
                top->pot_cnt_en = 0;
                tick();
            }
        };

        // connector A: bit 5 of _joy2 gates POT0DAT's high byte counter.
        top->_joy2 |= (1 << 5); // released
        settle(5);
        uint16_t a_released_before = read_reg(POT0DAT);
        pot_pulse(4);
        uint16_t a_released_after = read_reg(POT0DAT);

        top->_joy2 &= ~(uint16_t)(1 << 5); // asserted: "still charging"
        settle(5);
#ifdef MUT_accepts_paddle_timing
        top->_joy2 |= (1 << 5); // mutated: released again during the counted pulses
        settle(5);
#endif
        uint16_t a_before = read_reg(POT0DAT);
        pot_pulse(6);
        uint16_t a_after = read_reg(POT0DAT);
        int a_delta = (int)((a_after >> 8) - (a_before >> 8)) & 0xFF;
        if (a_delta > 200) a_delta -= 256;

        // connector B: bit 5 of _joy1 gates POT1DAT's high byte counter.
        top->_joy1 |= (1 << 5); // released
        settle(5);
        uint16_t b_released_before = read_reg(POT1DAT);
        pot_pulse(3);
        uint16_t b_released_after = read_reg(POT1DAT);

        top->_joy1 &= ~(uint16_t)(1 << 5); // asserted
        settle(5);
        uint16_t b_before = read_reg(POT1DAT);
        pot_pulse(4);
        uint16_t b_after = read_reg(POT1DAT);
        int b_delta = (int)((b_after >> 8) - (b_before >> 8)) & 0xFF;
        if (b_delta > 200) b_delta -= 256;

        note("connector A paddle: released 4 pulses " + std::to_string(a_released_before) + "->" +
             std::to_string(a_released_after) + "; asserted 6 pulses " + std::to_string(a_before) +
             "->" + std::to_string(a_after) + " (delta " + std::to_string(a_delta) + ")");
        note("connector B paddle: released 3 pulses " + std::to_string(b_released_before) + "->" +
             std::to_string(b_released_after) + "; asserted 4 pulses " + std::to_string(b_before) +
             "->" + std::to_string(b_after) + " (delta " + std::to_string(b_delta) + ")");

        bool ok = (a_released_after == a_released_before) && (a_delta == 6) &&
                  (b_released_after == b_released_before) && (b_delta == 4);
        expect(ok,
               "accepts_paddle_timing[sol:8-overview.c7,opus:8-intro.c13]");
    }

    // ---------------------------------------------------------------
    // PHASE P6 -- accepts_trackball_quadrature
    //   [sol:8-overview.c8,opus:8-intro.c14]
    // Electrically identical interface to the mouse (paired quadrature
    // phases); graded on connector B with its own stimulus/step count/
    // readback, independent of PHASE P3's connector-A measurement.
    // ---------------------------------------------------------------
    {
        reset_all();
        uint16_t base = read_reg(JOY1DAT);
        int idx = 0;
#ifdef MUT_accepts_trackball_quadrature
        step_n_decrement(/*isJoy2=*/false, 2, 0, idx, 5); // mutated: wrong-direction leg
#else
        step_n_increment(/*isJoy2=*/false, 2, 0, idx, 5);
#endif
        uint16_t up = read_reg(JOY1DAT);
        int8_t deltaUp = (int8_t)((up & 0xFF) - (base & 0xFF));

        step_n_decrement(/*isJoy2=*/false, 2, 0, idx, 5);
        uint16_t down = read_reg(JOY1DAT);
        int8_t deltaDown = (int8_t)((down & 0xFF) - (up & 0xFF));

        note("connector B quadrature: base=" + std::to_string(base) + " +5-> " + std::to_string(up) +
             " (delta " + std::to_string((int)deltaUp) + ") -5-> " + std::to_string(down) +
             " (delta " + std::to_string((int)deltaDown) + ")");

        expect(deltaUp == 5 && deltaDown == -5,
               "accepts_trackball_quadrature[sol:8-overview.c8,opus:8-intro.c14]");
    }

    // ---------------------------------------------------------------
    // Declined claims: none. Every uid in this package's ten claims
    // resolves to a signal already present on userio.v's own boundary; see
    // SPECIFICATION.md's "Absent-signal review". No record() lines needed.
    // ---------------------------------------------------------------

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
