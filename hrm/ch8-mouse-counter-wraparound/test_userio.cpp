// Test purpose: ch8-mouse-counter-wraparound Subject: HRM page 231 -- the
//   mouse/trackball counters at their limits.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 231 -- the mouse counter description and Table 8-2's wrapped
//   examples.
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
// ch8-mouse-counter-wraparound
//
// Subject: HRM page 231 -- the mouse/trackball counters at their limits.
// See SPECIFICATION.md for the quotations and per-proposition rationale.
//
// Fixture: userio.v compiled alone. dmouse0dat (behind JOY0DAT) is driven by
// the _joy2 port; dmouse1dat (behind JOY1DAT) is driven by the _joy1 port --
// joy_swap is hardwired 0, which reverses the port-name intuition (see
// userio.v:210-219). mouse_btn[0] is held asserted throughout so joy1enable
// stays forced to 0 (userio.v:222-227), keeping JOY0DAT in its plain
// mouse-counter read mode (userio.v:292-293) rather than the joystick-bit
// layout (userio.v:290-291). Every _joy1/_joy2 bit not under test is held at
// idle-high (1) explicitly: Verilator defaults undriven inputs to 0, which
// on this active-low interface reads as PRESSED and, left undriven, settles
// the low 2 bits of each counter byte to $02 instead of $00 (see
// userio.v:238-239, 251-252 -- the idle "else" branch mirrors the delayed
// pin phase into the counter's own low 2 bits).
//
// MUTATION lines use the label exactly as passed to expect() (uids,
// comma-joined, no spaces so it is one token). The MUT_<NAME> preprocessor guards are a separate, arbitrary replay
// key (Makefile: `make MUT=<NAME>`) -- never the assertion label itself.

#include <cstdint>
#include <iostream>
#include <string>
#include <cstdlib>
#include "verilated.h"
#include "Vuserio.h"

// ---- harness verbs (fixed form; do not rename) ---------------------------
static int g_passed = 0;
static int g_failed = 0;

void expect(bool cond, const std::string& label) {
    if (cond) { g_passed++; std::cout << "PASS: " << label << "\n"; }
    else      { g_failed++; std::cout << "FAIL: " << label << "\n"; }
}

void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }

void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// ---- register addresses (reg_address_in is [8:1], i.e. word address) -----
static const uint8_t JOY0DAT_ADDR = 0x05; // 9'h00a >> 1
static const uint8_t JOY1DAT_ADDR = 0x06; // 9'h00c >> 1
static const uint8_t JOYTEST_ADDR = 0x1B; // 9'h036 >> 1
static const uint8_t NEUTRAL_ADDR = 0x00; // matches no decoded register

// ---- model + pin bookkeeping ----------------------------------------------
static Vuserio* top = nullptr;
static uint16_t joy1_raw = 0xFFFF; // drives _joy1 (decodes JOY1DAT / dmouse1dat)
static uint16_t joy2_raw = 0xFFFF; // drives _joy2 (decodes JOY0DAT / dmouse0dat)

static void push_pins() {
    top->_joy1 = joy1_raw;
    top->_joy2 = joy2_raw;
}

static void tick() {
    push_pins();
    top->clk = 0; top->eval();
    top->clk = 1; top->eval();
}

static uint16_t read_reg(uint8_t addr) {
    top->reg_address_in = addr;
    top->eval();
    uint16_t v = top->data_out;
    top->reg_address_in = NEUTRAL_ADDR;
    top->eval();
    return v;
}

// ---- quadrature stimulus ---------------------------------------------------
// Legal single-bit-flip Gray ring for one (A,B) pin pair, in the order that
// userio.v's own increment boolean recognises as forward (checked against
// the RTL's four INC clauses by hand, and empirically by the
// fixture-liveness and row1/row2 checks below).
static const int RING[4][2] = { {0,0}, {0,1}, {1,1}, {1,0} };
static const int IDLE_IDX = 2; // (1,1) -- both pins idle-high

enum Dir { INC, DEC };

// Ring position is real hardware state and must be tracked *across* calls,
// not re-guessed each time: assuming "idle" at the start of every call would
// silently emit an illegal two-bit jump whenever the pins are not actually
// at idle, which the RTL sometimes happens to score as one step (by ring
// symmetry) and sometimes does not -- exactly the kind of bug a single-shot
// measurement would not catch.
// One idx per independently-driven channel; reset_counters() resets both.
static int idx_joy1x = IDLE_IDX; // _joy1 bits {0,2} -> dmouse1dat[7:0] (JOY1DAT)
static int idx_joy2x = IDLE_IDX; // _joy2 bits {0,2} -> dmouse0dat[7:0] (JOY0DAT)

// Drives `steps` legal quadrature transitions into the bit pair (bitA,bitB)
// of `raw`, two ticks per transition (one tick lets _sjoy catch the new pin
// state, a second lets _djoy catch up so the comparator in userio.v sees a
// clean old-vs-new pair -- see userio.v:213-219 vs. 230-254/261-281).
static void drive_quadrature(uint16_t& raw, int& idx, int bitA, int bitB, int steps, Dir dir) {
    for (int i = 0; i < steps; i++) {
        idx = (dir == INC) ? (idx + 1) % 4 : (idx + 3) % 4;
        int a = RING[idx][0], b = RING[idx][1];
        raw = (uint16_t)((raw & ~(1u << bitA)) | (a << bitA));
        raw = (uint16_t)((raw & ~(1u << bitB)) | (b << bitB));
        tick();
        tick();
    }
}

// One JOYTEST write, after idle-settling the pins, gives a fully known-zero
// baseline for both counter bytes on both ports (dmouse0dat is fully cleared
// by test_load; dmouse1dat's low 2 bits of each byte are not touched by
// test_load and must already be zero from the idle settle -- userio.v:232-
// 233, 245-246, 259-260, 272-273).
static void reset_counters() {
    joy1_raw = 0xFFFF;
    joy2_raw = 0xFFFF;
    idx_joy1x = IDLE_IDX;
    idx_joy2x = IDLE_IDX;
    for (int i = 0; i < 8; i++) tick(); // let _sjoy/_djoy settle to idle-high
    top->reg_address_in = JOYTEST_ADDR;
    top->data_in = 0;
    tick();
    tick();
    top->reg_address_in = NEUTRAL_ADDR;
    tick();
}

// Empirically measures how many state changes, in the given direction, are
// needed to move JOY0DAT's low byte from `from_val` to `to_val`, starting
// each measurement from a freshly-cleared counter (`from_val` is reached by
// direct increments from 0 -- always unambiguous since 0 <= from_val < 256).
static int measure_steps_to_target(int from_val, int to_val, Dir dir, int max_steps = 260) {
    reset_counters();
    drive_quadrature(joy2_raw, idx_joy2x, 0, 2, from_val, INC);
    uint16_t cur = read_reg(JOY0DAT_ADDR) & 0xFF;
    int steps = 0;
    while (cur != (uint16_t)to_val && steps < max_steps) {
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 1, dir);
        steps++;
        cur = read_reg(JOY0DAT_ADDR) & 0xFF;
    }
    return (cur == (uint16_t)to_val) ? steps : -1;
}

static int measure_min_steps(int from_val, int to_val) {
    int a = measure_steps_to_target(from_val, to_val, INC);
    int b = measure_steps_to_target(from_val, to_val, DEC);
    if (a < 0) return b;
    if (b < 0) return a;
    return (a < b) ? a : b;
}

// Which edge sense (INC or DEC) empirically realizes the SHORTER of the two
// wraparound paths from from_val to to_val. This is a real, DUT-measured
// fact -- not a compile-time property of the two byte values -- because
// which of the two directions comes out shorter depends on the counter's
// actual modulus (256, not the manual's 255) and on drive_quadrature really
// walking the ring that many times. Used to grade the *direction* half of
// Table 8-2's wrapped rows (see A9b/A10b) without claiming this module's
// pins carry a known physical up/down mapping -- that mapping is out of
// scope (A3's scope note; belongs to ch8-mouse-quadrature-decode). What IS
// in scope and checkable here: rows sharing a printed direction label use
// the same edge sense on this counter as the row1/row2 control cases (A7
// drives DEC for "Up (Left)"; A8 drives INC for "Down (Right)") that already
// established which edge sense the harness associates with each label.
static Dir winning_direction(int from_val, int to_val, int max_steps = 260) {
    int a = measure_steps_to_target(from_val, to_val, INC, max_steps);
    int b = measure_steps_to_target(from_val, to_val, DEC, max_steps);
    if (a < 0 && b < 0) return INC; // neither reached in budget; the paired
                                     // count assertion on the same row already
                                     // reports this case red on its own terms.
    if (a < 0) return DEC;
    if (b < 0) return INC;
    return (a <= b) ? INC : DEC;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    // Power-up / idle stimulus, driven explicitly on every relevant bit.
    top->clk7_en   = 1; // held high throughout; this fixture does not model
                         // the 7MHz/bus-clock ratio, only the counter logic
                         // that is gated by clk7_en, so a permanent 1 makes
                         // every posedge a processed edge.
    top->pot_cnt_en = 0;
    top->_fire0_dat = 1;
    top->_fire1_dat = 1;
    top->mouse_btn  = 0x1; // bit0 (left) held asserted -> _mleft=0 always ->
                            // joy1enable forced 0 every cycle (userio.v:224),
                            // independent of any settle timing on the fire
                            // pins -- keeps JOY0DAT in mouse-read mode.
    top->kms_level      = 0; // never toggled: keeps the keyboard-mouse-
                              // emulation adder (mouse0dat / xcount,ycount)
                              // at 0 so it never contaminates JOY0DAT's sum.
    top->kbd_mouse_type = 0;
    top->kbd_mouse_data = 0;
    top->joy_ana1 = 0;
    top->joy_ana2 = 0;
    top->data_in  = 0;
    top->reg_address_in = NEUTRAL_ADDR;
    joy1_raw = 0xFFFF;
    joy2_raw = 0xFFFF;

    top->reset = 1;
    for (int i = 0; i < 4; i++) tick();
    top->reset = 0;
    for (int i = 0; i < 4; i++) tick();

    reset_counters();

    // ---- Fixture liveness (not a page-231 claim) --------------------------
    // The harness must prove it can deliver a known word on each of the two
    // counters, and that the two are not aliased, before anything reads one.
    // These are graded through expect(), so they get real stimulus
    // mutations too -- a liveness check that cannot fail
    // would report "the data path is live" whether or not it actually is.
    // MUTATION harness:fixture-liveness-joy0dat-known-word stimulus red drove 6 steps into JOY0DAT's channel instead of 7 (comparator still checks for 7); measured (MUT=L1) 15 passed/3 failed -> 14 passed/4 failed, only this line flipped
    // MUTATION harness:fixture-liveness-joy1dat-known-word stimulus red drove 12 steps into JOY1DAT's channel instead of 13 (comparator still checks for 13); measured (MUT=L2) 15 passed/3 failed -> 14 passed/4 failed, only this line flipped
    // MUTATION harness:fixture-liveness-not-aliased stimulus red forced JOY1DAT's channel to the same driven count as JOY0DAT's (7 instead of 13), so the two independently-wired counters legitimately read equal; this unavoidably also flips harness:fixture-liveness-joy1dat-known-word (its 13 no longer arrives), since both checks are read off the same two measured words and there is no third, independent quantity to perturb -- measured (MUT=L3) 15 passed/3 failed -> 13 passed/5 failed, two lines flipped (documented, not concealed)
    {
        reset_counters();
        int steps_j0 = 7;
#ifdef MUT_L1
        steps_j0 = 6;
#endif
        int steps_j1 = 13;
#ifdef MUT_L2
        steps_j1 = 12;
#endif
#ifdef MUT_L3
        steps_j1 = steps_j0; // collapse the two channels' driven counts together
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, steps_j0, INC); // JOY0DAT low byte
        drive_quadrature(joy1_raw, idx_joy1x, 0, 2, steps_j1, INC); // JOY1DAT low byte
        uint16_t j0 = read_reg(JOY0DAT_ADDR) & 0xFF;
        uint16_t j1 = read_reg(JOY1DAT_ADDR) & 0xFF;
        note("fixture liveness: drove JOY0DAT low byte +" + std::to_string(steps_j0) +
             ", JOY1DAT low byte +" + std::to_string(steps_j1) +
             " from a cleared baseline; read back " + std::to_string(j0) + " and " + std::to_string(j1));
        expect(j0 == 7,  "harness:fixture-liveness-joy0dat-known-word");
        expect(j1 == 13, "harness:fixture-liveness-joy1dat-known-word");
        expect(j0 != j1, "harness:fixture-liveness-not-aliased");
    }

    // ---- A1: opus:8-limits.c1, sol:8-mouse-limit.c1 ------------------------
    // "These counters will \"wrap around\" in either the positive ... direction."
    // MUTATION opus:8-limits.c1,sol:8-mouse-limit.c1 stimulus red stopped one short of the 255->0 crossing (254 increments instead of 255, so the wrap step never happens)
    {
        reset_counters();
        int steps_to_255 = 255;
#ifdef MUT_A1
        steps_to_255 = 254;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, steps_to_255, INC);
        uint16_t v255 = read_reg(JOY0DAT_ADDR) & 0xFF;
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 1, INC);
        uint16_t v_after = read_reg(JOY0DAT_ADDR) & 0xFF;
        note("positive wrap: after " + std::to_string(steps_to_255) +
             " increments byte=" + std::to_string(v255) + ", one more increment gives " +
             std::to_string(v_after));
        expect(v255 == 255 && v_after == 0, "opus:8-limits.c1,sol:8-mouse-limit.c1");
    }

    // ---- A2: opus:8-limits.c2, sol:8-mouse-limit.c2 ------------------------
    // "... or negative direction."
    // MUTATION opus:8-limits.c2,sol:8-mouse-limit.c2 stimulus red decrement step skipped (0 decrements driven instead of 1; counter stays at 0, never reaches 255)
    {
        reset_counters();
        int dec_steps = 1;
#ifdef MUT_A2
        dec_steps = 0;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, dec_steps, DEC);
        uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
        note("negative wrap: from 0, " + std::to_string(dec_steps) +
             " decrement(s) gives " + std::to_string(v));
        expect(v == 255, "opus:8-limits.c2,sol:8-mouse-limit.c2");
    }

    // ---- A3: opus:8-limits.c11, sol:8-mouse-limit.c11 ----------------------
    // "The sign of the difference ... lets you determine which direction the
    // mouse is traveling." Scope: no direction port exists on this module;
    // tested as "sign reverses when phase order reverses" (see SPECIFICATION.md).
    // MUTATION opus:8-limits.c11,sol:8-mouse-limit.c11 stimulus red second run driven with the same (incrementing) phase order instead of reversing it, so its delta no longer differs in sign from the first
    {
        reset_counters();
        uint16_t p0 = read_reg(JOY0DAT_ADDR) & 0xFF;
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 24, INC);
        uint16_t p1 = read_reg(JOY0DAT_ADDR) & 0xFF;
        Dir second_dir = DEC;
#ifdef MUT_A3
        second_dir = INC;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 24, second_dir);
        uint16_t p2 = read_reg(JOY0DAT_ADDR) & 0xFF;
        int d1 = (int)p1 - (int)p0;
        int d2 = (int)p2 - (int)p1;
        note("sign test: p0=" + std::to_string(p0) + " p1=" + std::to_string(p1) +
             " p2=" + std::to_string(p2) + " d1=" + std::to_string(d1) +
             " d2=" + std::to_string(d2));
        expect(d1 > 0 && d2 < 0, "opus:8-limits.c11,sol:8-mouse-limit.c11");
    }

    // ---- A4: opus:8-limits.c13, sol:8-mouse-limit.c12 ----------------------
    // "The new value of a counter minus the previous value will represent the
    // number of mouse counts since the last check."
    // MUTATION opus:8-limits.c13,sol:8-mouse-limit.c12 stimulus red the 50-pulse case actually drove 49 state changes while the comparator still checked for 50
    {
        int ns[5] = {10, 20, 30, 40, 50};
        bool ok = true;
        for (int i = 0; i < 5; i++) {
            reset_counters();
            int n = ns[i];
            int driven = n;
#ifdef MUT_A4
            if (i == 4) driven = n - 1;
#endif
            drive_quadrature(joy2_raw, idx_joy2x, 0, 2, driven, INC);
            uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
            note("delta test: drove " + std::to_string(driven) +
                 " state changes, byte reads " + std::to_string(v) +
                 " (comparing against " + std::to_string(n) + ")");
            if ((int)v != n) ok = false;
        }
        expect(ok, "opus:8-limits.c13,sol:8-mouse-limit.c12");
    }

    // ---- A5: opus:8-limits.c14 ---------------------------------------------
    // "It treats both counts as unsigned values, ranging from 0 to 255."
    // MUTATION opus:8-limits.c14 stimulus impossible unfalsifiable at this boundary: dmouse0dat[7:0] is a native 8-bit Verilog register (userio.v:230-241); no value driven through _joy1/_joy2 can make it read above 255, so no stimulus mutation can turn this assertion red
    {
        reset_counters();
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 255, INC);
        uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
        note("range test: after 255 increments from 0, byte=" + std::to_string(v));
        expect(v == 255, "opus:8-limits.c14");
    }

    // ---- A6: opus:8-limits.c15 (reader-flagged "borderline") ---------------
    // "A count of 100 pulses is measured in each case." -- tested against all
    // four rows of Table 8-2.
    {
        int r1 = measure_min_steps(200, 100);
        int r2 = measure_min_steps(100, 200);
        int r3 = measure_min_steps(200, 45);
        int r4 = measure_min_steps(45, 200);
        note("Table 8-2 rows, measured minimum state changes: row1(200->100)=" +
             std::to_string(r1) + " row2(100->200)=" + std::to_string(r2) +
             " row3(200->45)=" + std::to_string(r3) + " row4(45->200)=" +
             std::to_string(r4) + " (manual claims 100 for every row)");
        expect(r1 == 100 && r2 == 100 && r3 == 100 && r4 == 100, "opus:8-limits.c15");
    }
    // MUTATION opus:8-limits.c15 stimulus green already RED (rows 3-4 measure 101, not 100); substituted row 1's proven-live 100-step transition through the same required-steps-equals-100 comparator to show it is not vacuously always-false
    // MUTATION opus:8-limits.c15,substitute-check stimulus red broke the substitute's own transition (99 decrementing steps instead of the needed 100), leaving the comparator (v==100) untouched; measured (MUT=A6SUB) 15 passed/3 failed -> 14 passed/4 failed, only this line flipped
    {
        reset_counters();
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 200, INC);
        int sub_steps = 100;
#ifdef MUT_A6SUB
        sub_steps = 99;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, sub_steps, DEC);
        uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
        expect(v == 100, "opus:8-limits.c15,substitute-check");
    }

    // ---- A7: opus:8-limits.c16, sol:8-mouse-limit.c13 -----------------------
    // Table 8-2 row 1: previous 200, current 100. Control case (no wraparound).
    // MUTATION opus:8-limits.c16,sol:8-mouse-limit.c13 stimulus red drove 99 decrementing state changes from 200 instead of the needed 100
    {
        reset_counters();
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 200, INC);
        int steps = 100;
#ifdef MUT_A7
        steps = 99;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, steps, DEC);
        uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
        note("row1: 200, then " + std::to_string(steps) + " decrements -> " + std::to_string(v));
        expect(v == 100, "opus:8-limits.c16,sol:8-mouse-limit.c13");
    }

    // ---- A8: opus:8-limits.c17, sol:8-mouse-limit.c14 -----------------------
    // Table 8-2 row 2: previous 100, current 200. Control case (no wraparound).
    // MUTATION opus:8-limits.c17,sol:8-mouse-limit.c14 stimulus red drove 99 incrementing state changes from 100 instead of the needed 100
    {
        reset_counters();
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 100, INC);
        int steps = 100;
#ifdef MUT_A8
        steps = 99;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, steps, INC);
        uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
        note("row2: 100, then " + std::to_string(steps) + " increments -> " + std::to_string(v));
        expect(v == 200, "opus:8-limits.c17,sol:8-mouse-limit.c14");
    }

    // ---- A9a: row 3 + footnote * -- the COUNT half ---------------------------
    // opus:8-limits.c20, sol:8-mouse-limit.c17 (the footnote's numeric claim)
    // "* Because 200-45 = 155, ... the true count must be 255 - (200-45) = 100."
    // (opus:8-limits.c18 / sol:8-mouse-limit.c15 -- the bare row's "...Down"
    // label -- are NOT this claim's count half; see A9b below. This
    // count-only predicate never exercises direction: measure_min_steps()
    // tries both edge senses and keeps only the smaller step count, never
    // which sense won. So the row+footnote claim is split into its count
    // half (here) and its direction half (A9b), each carrying only the uids
    // it actually tests.)
    {
        int measured = measure_min_steps(200, 45);
        note("row3+footnote*: manual's true count is 100; measured " +
             std::to_string(measured) +
             " state changes actually required (8-bit/mod-256 hardware; the manual's formula divides by 255)");
        expect(measured == 100, "opus:8-limits.c20,sol:8-mouse-limit.c17");
    }
    // MUTATION opus:8-limits.c20,sol:8-mouse-limit.c17 stimulus green already RED (measured 101, not 100); substituted row 1's proven-live 100-step transition through the same required-steps-equals-100 comparator
    // MUTATION opus:8-limits.c20,sol:8-mouse-limit.c17,substitute-check stimulus red broke the substitute's own transition (99 decrementing steps instead of the needed 100), leaving the comparator (v==100) untouched; measured (MUT=A9SUB) 15 passed/3 failed -> 14 passed/4 failed, only this line flipped
    {
        reset_counters();
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 200, INC);
        int sub_steps = 100;
#ifdef MUT_A9SUB
        sub_steps = 99;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, sub_steps, DEC);
        uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
        expect(v == 100, "opus:8-limits.c20,sol:8-mouse-limit.c17,substitute-check");
    }

    // ---- A9b: row 3 -- the DIRECTION half -------------------------------------
    // opus:8-limits.c18, sol:8-mouse-limit.c15 (the bare row's "200 45 Down *")
    // Scope, same boundary as A3: this module has no direction port, and which
    // physical mouse motion is "up" vs "down" is Table 8-1/Figure 8-2 territory
    // (ch8-mouse-quadrature-decode), not this package's. What IS testable here:
    // A7 realized row 1 ("Up (Left)") by driving DEC edges; A8 realized row 2
    // ("Down (Right)") by driving INC edges -- the harness's own, already-
    // exercised association between an edge sense and a printed label. Row 3
    // carries the SAME label as row 2 ("Down"), so its true (short, ~100-step)
    // wraparound path should be realized by the SAME edge sense as row 2's:
    // INC. This is a DUT fact -- it depends on the counter's actual modulus
    // (256) and genuinely could have come out DEC on buggy hardware -- not a
    // restatement of the compile-time row values.
    // MUTATION opus:8-limits.c18,sol:8-mouse-limit.c15 stimulus red retargeted the row-3 measurement from 200->45 to 200->150 (still on the previous=200 baseline this row starts from), which needs only 50 DEC steps against 206 INC steps, so the empirically shorter path flips from INC to DEC; measured (MUT=A9DIR) 15 passed/3 failed -> 14 passed/4 failed, only this line flipped
    {
        int to_val = 45;
#ifdef MUT_A9DIR
        to_val = 150;
#endif
        Dir winner = winning_direction(200, to_val);
        note("row3 direction: shortest wraparound path from 200 to " + std::to_string(to_val) +
             " uses " + std::string(winner == INC ? "INC" : "DEC") +
             " edges (row2's 'Down (Right)' control, A8, also used INC)");
        expect(winner == INC, "opus:8-limits.c18,sol:8-mouse-limit.c15");
    }

    // ---- A10a: row 4 + footnote ** -- the COUNT half --------------------------
    // opus:8-limits.c21, sol:8-mouse-limit.c18 (the footnote's numeric claim)
    // "** 45 - 200 = -155. ... the true count must be 255 + (-155) = 100."
    // (opus:8-limits.c19 / sol:8-mouse-limit.c16 -- the row/table's "...Up"
    // label -- are NOT this claim's count half; see A10b below. Split as in
    // A9a: this count-only predicate never checks which edge sense won.)
    {
        int measured = measure_min_steps(45, 200);
        note("row4+footnote**: manual's true count is 100; measured " +
             std::to_string(measured) +
             " state changes actually required (8-bit/mod-256 hardware; the manual's formula divides by 255)");
        expect(measured == 100, "opus:8-limits.c21,sol:8-mouse-limit.c18");
    }
    // MUTATION opus:8-limits.c21,sol:8-mouse-limit.c18 stimulus green already RED (measured 101, not 100); substituted row 2's proven-live 100-step transition through the same required-steps-equals-100 comparator
    // MUTATION opus:8-limits.c21,sol:8-mouse-limit.c18,substitute-check stimulus red broke the substitute's own transition (99 incrementing steps instead of the needed 100), leaving the comparator (v==200) untouched; measured (MUT=A10SUB) 15 passed/3 failed -> 14 passed/4 failed, only this line flipped
    {
        reset_counters();
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, 100, INC);
        int sub_steps = 100;
#ifdef MUT_A10SUB
        sub_steps = 99;
#endif
        drive_quadrature(joy2_raw, idx_joy2x, 0, 2, sub_steps, INC);
        uint16_t v = read_reg(JOY0DAT_ADDR) & 0xFF;
        expect(v == 200, "opus:8-limits.c21,sol:8-mouse-limit.c18,substitute-check");
    }

    // ---- A10b: row 4 -- the DIRECTION half -------------------------------------
    // opus:8-limits.c19, sol:8-mouse-limit.c16 (the table's "45 200 Up **" row)
    // Same reasoning as A9b, mirrored: row 4 carries the SAME label as row 1
    // ("Up"), so its true (short) wraparound path should be realized by the
    // same edge sense A7 used for row 1: DEC.
    // MUTATION opus:8-limits.c19,sol:8-mouse-limit.c16 stimulus red retargeted the row-4 measurement from 45->200 to 45->100 (still on the previous=45 baseline this row starts from), which needs only 55 INC steps against 201 DEC steps, so the empirically shorter path flips from DEC to INC; measured (MUT=A10DIR) 15 passed/3 failed -> 14 passed/4 failed, only this line flipped
    {
        int to_val = 200;
#ifdef MUT_A10DIR
        to_val = 100;
#endif
        Dir winner = winning_direction(45, to_val);
        note("row4 direction: shortest wraparound path from 45 to " + std::to_string(to_val) +
             " uses " + std::string(winner == INC ? "INC" : "DEC") +
             " edges (row1's 'Up (Left)' control, A7, also used DEC)");
        expect(winner == DEC, "opus:8-limits.c19,sol:8-mouse-limit.c16");
    }

    std::cout << g_passed << " passed, " << g_failed << " failed\n";
    int rc = (g_failed == 0) ? 0 : 1;
    delete top;
    return rc;
}
