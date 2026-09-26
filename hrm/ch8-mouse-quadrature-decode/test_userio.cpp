// Test purpose: page 229's mouse/trackball quadrature-input text, Figure 8-2
//   (printed p.230, "Mouse Quadrature" -- the Case 1/Case 2 waveforms and
//   the V/VQ:D1/D0 truth table), the axis-routing half of Figure 8-3
//   (printed p.233, "Joystick to Counter Connections"), and the Appendix A
//   p.281 sentence on sampling and clocking the mouse counters. Package
//   ch8-mouse-quadrature-decode, fixture userio.v (one Verilator build, this
//   module only -- userio.v has never had a test package before this one).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 229, 230, 233, 281.
// Non-goals: JOY0DAT/JOY1DAT's own address and 16-bit word layout (a
//   differently-scoped claims file, ch8-joydat-counter-layout.jsonl, covers
//   that); the DE-9 connector pin numbers and joystick function names Figure
//   8-3 also draws (not a signal of this module, and composing them with a
//   joystick function additionally needs Table 8-3, not in this claims
//   file); CIAAPRA fire buttons; the pot counters; the keyboard/PS2
//   mouse-emulation path (kbd_mouse_type/kbd_mouse_data/kms_level) that also
//   feeds JOY0DAT in real hardware -- held inactive throughout (see
//   BOUNDARY) so every measurement below isolates the DE-9 quadrature decode
//   path Figure 8-2 documents.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: nanomig/src/minimig-aga/userio.v:28-50 module header -- ports
//   clk, reset, clk7_en, reg_address_in[8:1], data_in[15:0],
//   data_out[15:0], pot_cnt_en, _fire0, _fire1, _fire0_dat, _fire1_dat,
//   _joy1[15:0], _joy2[15:0], joy_ana1[15:0], joy_ana2[15:0], mouse_btn[2:0],
//   kms_level, kbd_mouse_type[1:0], kbd_mouse_data[7:0]. Read directly by
//   this agent AFTER SPECIFICATION.md was committed (commit 7458312) -- no
//   port name, register address or RTL line appears anywhere in that file.
//
// METHODOLOGY, established by direct measurement on this fixture (a scratch
//   diagnostic, not part of this package, confirmed each point below before
//   any assertion here was written):
//
// (1) SIGNAL ROUTING. userio.v internally swaps its two joystick input buses
//   through a pair of synchronizer registers: port 1's quadrature decode
//   (feeding JOY0DAT, l.229-254) is driven from input _joy2, and port 2's
//   (feeding JOY1DAT, l.256-281) from input _joy1. Within the driving word,
//   bits {0,2} are one axis's quadrature pair and bits {1,3} are the other's
//   (l.234-252): bit 2 ("H"/"V", the manual's "leading" line) and bit 0
//   ("HQ"/"VQ", the "following" line) drive the LOW byte (X/horizontal
//   count); bit 3 ("V") and bit 1 ("VQ") drive the HIGH byte (Y/vertical
//   count) -- matching Figure 8-3's own routing (pins 1&3 to the Y counter,
//   pins 2&4 to the X counter) exactly. Measured directly: toggling each of
//   these four bits alone, one at a time, moves exactly one byte and leaves
//   the other fixed (A4 below).
//
// (2) THE RING. A single-bit change on the LEADING line (bit 2 or bit 3)
//   while the FOLLOWING line (bit 0 or bit 1) is 0, or the reverse pairing,
//   is what l.234-237/247-250's boolean conditions recognise as a valid
//   quadrature step; which of the two lines moves first sets the sign. The
//   four-state ring (main,quad) = 00->10->11->01->00, walked with the MAIN
//   line changing first each half-step, measures as a clean +1 per step
//   (Figure 8-2's "Case 1: Count Up"); walked with the QUADRATURE line
//   changing first, a clean -1 per step ("Case 2: Count Down"). Two
//   clk7_en-gated clock edges are needed per stimulus change for it to
//   register as one such step: userio.v's own two-stage synchronizer
//   (l.213-219) means the "old" reference value used by the increment/
//   decrement comparison (l.234 etc.) is two clk7_en edges behind whatever
//   is driven onto _joy1/_joy2, not one.
//
// (3) THE D1/D0 LATCH. When neither a valid increment nor a valid decrement
//   is detected, l.239/252 fall through to an "else" that overwrites the
//   BYTE'S OWN LOW TWO BITS with {!quadrature_line, main_line^quadrature_
//   line} -- fed from the CURRENT pin state alone, every such cycle,
//   independent of the byte's accumulated count. Measured directly against
//   all four rows of Figure 8-2's table (holding the pins at each of the
//   four states in turn, long enough to settle): this formula reproduces
//   the table exactly, cell for cell, when the manual's "V" is read as this
//   module's MAIN line and "VQ" as its QUADRATURE line (A10-A17 below). One
//   consequence, also measured directly and load-bearing for every
//   assertion in this file: because this same "else" fires on every idle
//   cycle where an axis's own two pins are unchanged, an axis that is NOT
//   being driven does not read back as zero once settled -- it settles to
//   and holds {2'b10} in its own low two bits (numerically 0x02), with its
//   upper six bits wherever JOYTEST last left them. Every assertion below
//   that checks a byte for "unchanged" or establishes a baseline reads that
//   baseline BACK from the fixture with `read()` rather than assuming a
//   hand-derived constant, for exactly this reason.
//
// (4) BASELINE. JOY0DAT's own JOYTEST-triggered clear (l.232-233, l.245-246)
//   unconditionally zeroes BOTH of port 1's bytes, all bits, including the
//   low two the D1/D0 latch would otherwise hold. JOY1DAT's clear (l.259-
//   260, l.272-273) is different -- test_data is loaded into bits [7:2] and
//   [15:10] only, and port 2's own low two bits per byte are left exactly as
//   the D1/D0 latch last set them (measured directly: a JOYTEST write with
//   data_in=0xFFFF, port 2 pins idle at their last settled state, left
//   JOY1DAT's low two bits of each byte at that settled D1/D0 value, not at
//   the written pattern). Every baseline below is therefore established by
//   settling the relevant axis pins to a known state FIRST, issuing JOYTEST,
//   and then reading the result back rather than asserting a hand-derived
//   number outright -- consistent with the recorded RTL observation that
//   these counters have no reset branch of their own.
//
// (5) ISOLATION FROM THE OTHER COUNTING PATH. JOY0DAT in mouse mode is
//   `mouse0dat + dmouse0dat` (l.292-293) where mouse0dat = {ycount,xcount}
//   (l.358) is driven only by the keyboard/PS2 mouse-emulation path
//   (l.330-355), gated on kms_level toggling. reset clears xcount/ycount
//   (l.336-338) and kms_level is never toggled anywhere in this file, so
//   mouse0dat is provably 0 throughout every measurement below (0 plus
//   dmouse0dat is dmouse0dat) -- confirmed directly (JOY0DAT read back
//   exactly matches dmouse0dat's own arithmetic in every case measured).
//   This is what makes it unnecessary to force joystick mode (joy1enable)
//   for any assertion here: both of JOY0DAT's read paths reduce to the same
//   value under this setup.
//
// CLAIM COVERAGE: all 61 claims in ch8-mouse-quadrature-decode.jsonl are
//   cited below, in full uid form, either inside an expect()/note() label or
//   inside a record() call with a stated reason -- see SPECIFICATION.md for
//   the complete accounting. 18 new graded assertions; no note()s; the
//   remaining 5 claims declined via record(), matching SPECIFICATION.md's
//   discard table exactly.
//
// CONTROL CASES: A6 (vertical Case 1, count up) and A7 (vertical Case 2,
//   count down) are a same-basis pair -- identical ring-driving harness,
//   identical delta comparator, opposite ring order -- placed back to back
//   in the code and in the printed output; likewise A8/A9 on the horizontal
//   pair. Nothing in this package came back red, so no PASS/RED pairing was
//   needed the way some chapter-7 packages required.
//
// MUTATION LOG (every entry below was actually run by this agent under WSL/
//   Verilator: `make clean && make MUT=<NAME>` -> run -> confirm the named
//   assertion, and only that one, newly red against the 18-passed/0-failed
//   baseline -> `make clean && make` -> confirm the baseline exactly
//   restored, before moving to the next one):
//
// MUTATION LIVENESS-KNOWN-DISTINCT-AXIS-COUNTS stimulus red: an extra,
//   unrequested vertical-axis step was injected partway through the
//   horizontal-only stimulus run -> baseline shifted from "18 passed, 0
//   failed" to "17 passed, 1 failed", only this assertion newly red (the
//   vertical byte no longer reads unchanged). Reverted, baseline restored.
// MUTATION COUNTERS-ARE-8-BIT-AND-WRAP stimulus red: the up-run step count
//   changed from 260 (past the 256 wrap) to 255 (short of it). FIRST
//   ATTEMPT stayed green (18/0, unchanged) -- the draft recomputed
//   expected_low from the up_steps variable itself, so the oracle silently
//   tracked the mutated stimulus and proved nothing (the same class of
//   mistake ch7-beam-position-word-layout's own mutation log records and
//   warns about). Fixed to compare against the literal 260; rebuilt ->
//   "17 passed, 1 failed", only this assertion newly red. Reverted,
//   baseline restored.
// MUTATION ADVANCE-WITHOUT-REGISTER-ACCESS stimulus red: a JOYTEST address
//   hit was injected in the middle of the drive run that is supposed to run
//   entirely with reg_address_in parked on an unrelated address -> only this
//   assertion newly red (the mid-run clear makes the final delta come up
//   short of the requested step count). Reverted, baseline restored.
// MUTATION EXACTLY-TWO-LINES-PER-AXIS stimulus red: the bit driven for the
//   "V only" probe changed from bit 3 to bit 0 (HQ) -> only this assertion
//   newly red (the low/X byte moves when the proposition says it must not).
//   Reverted, baseline restored.
// MUTATION ONE-COUNT-PER-QUADRATURE-STATE-CHANGE stimulus red: the
//   requested step count changed from 13 (up) to 14 while the oracle still
//   compares against 13 -> only this assertion newly red. Reverted, baseline
//   restored.
// MUTATION VERTICAL-CASE1-COUNTS-UP stimulus red: the ring order swapped
//   from Case 1 (V leading VQ) to Case 2 (VQ leading V) while the oracle
//   still expects a positive delta -> only this assertion newly red (the
//   byte moves the wrong way). Reverted, baseline restored.
// MUTATION VERTICAL-CASE2-COUNTS-DOWN stimulus red: same swap, the other
//   direction (Case 2 ring order replaced with Case 1) -> only this
//   assertion newly red. Reverted, baseline restored.
// MUTATION HORIZONTAL-RIGHT-INCREMENTS stimulus red: ring order swapped
//   Case 1 -> Case 2 on the horizontal pair -> only this assertion newly
//   red. Reverted, baseline restored.
// MUTATION HORIZONTAL-LEFT-DECREMENTS stimulus red: ring order swapped
//   Case 2 -> Case 1 on the horizontal pair -> only this assertion newly
//   red. Reverted, baseline restored.
// MUTATION FIG8-2-V0-VQ0-D1 stimulus red: the settled state changed from
//   (V=0,VQ=0) to (V=1,VQ=1) (both bits flipped) while the oracle still
//   expects D1=1 -> only this assertion newly red (V=1,VQ=1 settles D1=0).
//   Reverted.
// MUTATION FIG8-2-V0-VQ0-D0 stimulus red: FIRST ATTEMPT (both bits flipped,
//   V=0,VQ=0 -> V=1,VQ=1, the same swap as the D1 mutation above) stayed
//   green (18/0, unchanged) -- D0 = V^VQ is invariant under flipping BOTH
//   inputs at once (0^0=0, 1^1=0, same value), so that stimulus change
//   could never falsify this assertion no matter what the RTL does. WHAT
//   MADE ME LOOK TWICE: every other truth-table mutation in this batch
//   shifted the count from 18/0 to 17/1; this was the only one, alongside
//   its three D0 siblings below, that came back unchanged. Fixed to flip V
//   ONLY (V=0,VQ=0 -> V=1,VQ=0), which always toggles the XOR -> "17
//   passed, 1 failed", only this assertion newly red. Reverted.
// MUTATION FIG8-2-V0-VQ1-D1 stimulus red: settled state (0,1) replaced with
//   (1,0) (both bits flipped) -> only this assertion newly red. Reverted.
// MUTATION FIG8-2-V0-VQ1-D0 stimulus red: same both-bits-flipped mistake as
//   V0-VQ0-D0 above, same fix (flip V only: V=0,VQ=1 -> V=1,VQ=1) -> only
//   this assertion newly red once fixed. Reverted.
// MUTATION FIG8-2-V1-VQ0-D1 stimulus red: settled state (1,0) replaced with
//   (0,1) (both bits flipped) -> only this assertion newly red. Reverted.
// MUTATION FIG8-2-V1-VQ0-D0 stimulus red: same fix as above (flip V only:
//   V=1,VQ=0 -> V=0,VQ=0) -> only this assertion newly red once fixed.
//   Reverted.
// MUTATION FIG8-2-V1-VQ1-D1 stimulus red: settled state (1,1) replaced with
//   (0,0) (both bits flipped) -> only this assertion newly red. Reverted.
// MUTATION FIG8-2-V1-VQ1-D0 stimulus red: same fix as above (flip V only:
//   V=1,VQ=1 -> V=0,VQ=1) -> only this assertion newly red once fixed.
//   Reverted.
// MUTATION JOY1DAT-MIRRORS-JOY0DAT stimulus red: port 2's ring order swapped
//   Case 1 -> Case 2 while the oracle still expects a positive delta -> only
//   this assertion newly red. Reverted, baseline restored.
//
// Final rebuild after the last revert: 18 passed, 0 failed, matching the
// committed baseline exactly (see out.txt).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vuserio.h"

namespace {

// Fixed per the build brief: expect() grades, note() measures without
// grading, record() declines with a reason and is never a measurement.
class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// Register addresses, as reg_address_in ([8:1]) values -- confirmed directly
// against userio.v:54-61's own parameter declarations.
constexpr int A_JOY0DAT  = 0x00a >> 1;
constexpr int A_JOY1DAT  = 0x00c >> 1;
constexpr int A_JOYTEST  = 0x036 >> 1;
// An address this module decodes nowhere (userio.v:54-61's parameter list
// has no entry at $000), used by A3 to show the counters advance with the
// bus parked on unrelated territory.
constexpr int A_UNMAPPED = 0x000;

std::string hex16(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%04x", v);
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vuserio) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->pot_cnt_en = 0;
        dut->_fire0_dat = 1;
        dut->_fire1_dat = 1;
        dut->_joy1 = 0;
        dut->_joy2 = 0;
        dut->joy_ana1 = 0;
        dut->joy_ana2 = 0;
        dut->mouse_btn = 0;      // all buttons idle (active-high raw input)
        dut->kms_level = 0;      // never toggled anywhere below -- see
                                  // METHODOLOGY (5): this keeps mouse0dat==0
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        for (int i = 0; i < 10; ++i) { dut->clk7_en = 1; tick(); }
        dut->reset = 0;
        dut->clk7_en = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // clk7_en-gated idle ticks with every input held -- lets the D1/D0 latch
    // (METHODOLOGY (3)) settle fresh off whatever the pins currently show.
    void idle(int n) {
        dut->clk7_en = 1;
        for (int i = 0; i < n; ++i) tick();
        dut->clk7_en = 0;
    }

    // Combinational readback (userio.v:289 is an always @(*) mux) -- no
    // clock edge needed, matching the sibling harness precedent this file's
    // own read() is modelled on.
    uint16_t read(int addr) {
        dut->reg_address_in = static_cast<uint16_t>(addr & 0x1FF);
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = 0;
        dut->eval();
        return v;
    }

    void joytest_write(uint16_t data = 0) {
        dut->reg_address_in = A_JOYTEST;
        dut->data_in = data;
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->eval();
    }

    // One registered quadrature step on port 1 (_joy2 -> JOY0DAT), two
    // clk7_en edges -- see METHODOLOGY (2). main/quad name the manual's
    // "V"/"H" (bit 3/2) and "VQ"/"HQ" (bit 1/0) roles generically; axis
    // selects which pair (0=horizontal: bits 2,0; 1=vertical: bits 3,1).
    void step_port1(int axis, int main_bit, int quad_bit, int other_axis_main = 0, int other_axis_quad = 0) {
        uint16_t w = 0;
        if (axis == 1) {  // vertical drives bits 3,1; horizontal (bits 2,0) held at other_axis_*
            if (main_bit) w |= (1 << 3);
            if (quad_bit) w |= (1 << 1);
            if (other_axis_main) w |= (1 << 2);
            if (other_axis_quad) w |= (1 << 0);
        } else {
            if (main_bit) w |= (1 << 2);
            if (quad_bit) w |= (1 << 0);
            if (other_axis_main) w |= (1 << 3);
            if (other_axis_quad) w |= (1 << 1);
        }
        dut->_joy2 = w;
        dut->clk7_en = 1;
        tick();
        tick();
        dut->clk7_en = 0;
    }
    void step_port2(int axis, int main_bit, int quad_bit, int other_axis_main = 0, int other_axis_quad = 0) {
        uint16_t w = 0;
        if (axis == 1) {
            if (main_bit) w |= (1 << 3);
            if (quad_bit) w |= (1 << 1);
            if (other_axis_main) w |= (1 << 2);
            if (other_axis_quad) w |= (1 << 0);
        } else {
            if (main_bit) w |= (1 << 2);
            if (quad_bit) w |= (1 << 0);
            if (other_axis_main) w |= (1 << 3);
            if (other_axis_quad) w |= (1 << 1);
        }
        dut->_joy1 = w;
        dut->clk7_en = 1;
        tick();
        tick();
        dut->clk7_en = 0;
    }

    Vuserio* dut;
};

// Ring order for "axis" (0=horizontal, 1=vertical), Case 1 (main leads
// quad -- Figure 8-2's "Count Up") if up==true, Case 2 (quad leads main --
// "Count Down") otherwise. Walks exactly n steps from whatever state the
// pins are already in. other_main/other_quad hold the OTHER axis steady
// throughout (used by A1/A4 to prove axis independence).
void walk_ring(Harness& h, bool port1, int axis, bool up, int n,
               int other_main = 0, int other_quad = 0) {
    // Case 1 sequence of (main,quad): 00->10->11->01->00 ...
    // Case 2 sequence:                00->01->11->10->00 ...
    static const int case1[4][2] = {{0,0},{1,0},{1,1},{0,1}};
    static const int case2[4][2] = {{0,0},{0,1},{1,1},{1,0}};
    const int (*seq)[2] = up ? case1 : case2;
    // find current phase 0 as a fresh start: always begin the walk by first
    // driving state (0,0) and settling, so every call starts from a known
    // phase regardless of history.
    if (port1) h.step_port1(axis, 0, 0, other_main, other_quad);
    else       h.step_port2(axis, 0, 0, other_main, other_quad);
    int phase = 0;
    for (int i = 0; i < n; ++i) {
        phase = (phase + 1) & 3;
        int m = seq[phase][0], q = seq[phase][1];
        if (port1) h.step_port1(axis, m, q, other_main, other_quad);
        else       h.step_port2(axis, m, q, other_main, other_quad);
    }
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= A1: LIVENESS-KNOWN-DISTINCT-AXIS-COUNTS =================
    // (p.229 opus:8-mouse.c1, opus:8-mouse.c4, sol:8-mouse.c1, sol:8-mouse.c2;
    //  p.281 opus:8-appa-joy.c86, sol:8-appa-joy.c51)
    // Mandatory fixture-liveness proof: userio.v has never had a test
    // package before this one. Two DISTINCT, known stimuli (5 horizontal-
    // only steps, then separately 7 vertical-only steps) must each arrive
    // at their OWN byte and leave the other exactly where it started, before
    // anything else in this file is graded.
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);
        const int N_H = 5;
        int extra_v_steps = 0;
#ifdef MUT_LIVENESS_KNOWN_DISTINCT_AXIS_COUNTS
        extra_v_steps = 1;  // an unrequested vertical step during the "horizontal only" run
#endif
        walk_ring(h, /*port1=*/true, /*axis=*/0, /*up=*/true, N_H);
        if (extra_v_steps) walk_ring(h, true, /*axis=*/1, true, extra_v_steps);
        uint16_t after_h = h.read(A_JOY0DAT);
        uint8_t low_delta = static_cast<uint8_t>((after_h & 0xFF) - (base & 0xFF));
        bool h_ok = (low_delta == N_H) && ((after_h >> 8) == (base >> 8));

        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base2 = h.read(A_JOY0DAT);
        const int N_V = 7;
        walk_ring(h, true, /*axis=*/1, true, N_V);
        uint16_t after_v = h.read(A_JOY0DAT);
        uint8_t high_delta = static_cast<uint8_t>((after_v >> 8) - (base2 >> 8));
        bool v_ok = (high_delta == N_V) && ((after_v & 0xFF) == (base2 & 0xFF));

        checks.expect(
            h_ok && v_ok,
            "LIVENESS-KNOWN-DISTINCT-AXIS-COUNTS (p.229 opus:8-mouse.c1, "
            "opus:8-mouse.c4, sol:8-mouse.c1, sol:8-mouse.c2; p.281 "
            "opus:8-appa-joy.c86, sol:8-appa-joy.c51): a distinct, known "
            "number of quadrature transitions driven on the horizontal pair "
            "alone (5) arrives at the horizontal count and leaves the "
            "vertical count exactly where it started; a different distinct "
            "known number on the vertical pair alone (7) arrives at the "
            "vertical count and leaves the horizontal count exactly where "
            "it started -- proves this fixture's data path is live on its "
            "first-ever use, before anything else here is graded. Measured "
            "horizontal-only: base=" + hex16(base) + " after=" + hex16(after_h) +
            "; vertical-only: base=" + hex16(base2) + " after=" + hex16(after_v) + ".");
    }

    // ================= A2: COUNTERS-ARE-8-BIT-AND-WRAP =================
    // (p.229 opus:8-mouse.c2, sol:8-mouse.c3, sol:8-mouse.c4; p.230
    //  opus:8-fig8-2.c3)
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);
        int up_steps = 260;  // > 256: must wrap at least once
#ifdef MUT_COUNTERS_ARE_8_BIT_AND_WRAP
        up_steps = 255;  // short of the wrap
#endif
        walk_ring(h, true, 0, true, up_steps);
        uint16_t after_up = h.read(A_JOY0DAT);
        // Fixed expected delta (260), NOT up_steps -- comparing against the
        // stimulus variable itself would let the oracle silently track a
        // mutated stimulus and prove nothing (caught by running this very
        // mutation: the first draft did exactly that and stayed green).
        uint8_t expected_low = static_cast<uint8_t>((base & 0xFF) + 260);
        bool wrap_up_ok = static_cast<uint8_t>(after_up & 0xFF) == expected_low;

        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base2 = h.read(A_JOY0DAT);
        walk_ring(h, true, 0, false, 3);  // 3 down-steps from a low baseline
        uint16_t after_down = h.read(A_JOY0DAT);
        uint8_t expected_low2 = static_cast<uint8_t>((base2 & 0xFF) - 3);
        bool wrap_down_ok = static_cast<uint8_t>(after_down & 0xFF) == expected_low2;

        checks.expect(
            wrap_up_ok && wrap_down_ok,
            "COUNTERS-ARE-8-BIT-AND-WRAP (p.229 opus:8-mouse.c2, "
            "sol:8-mouse.c3, sol:8-mouse.c4; p.230 opus:8-fig8-2.c3): the "
            "horizontal counter is 8 bits wide and wraps in both directions "
            "-- 260 up-steps from a known baseline land exactly at "
            "baseline+260 mod 256, and 3 down-steps from a low baseline "
            "wrap below 0. Measured: up base=" + hex16(base) + " after=" +
            hex16(after_up) + " (expected low byte " + hex16(expected_low) +
            "); down base=" + hex16(base2) + " after=" + hex16(after_down) +
            " (expected low byte " + hex16(expected_low2) + ").");
    }

    // ================= A3: ADVANCE-WITHOUT-REGISTER-ACCESS =================
    // (p.229 opus:8-mouse.c3, sol:8-mouse.c5)
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        h.dut->reg_address_in = A_UNMAPPED;
        h.dut->eval();
        uint16_t base = h.read(A_JOY0DAT);
        h.dut->reg_address_in = A_UNMAPPED;  // parked here for the whole run
        h.dut->eval();
        const int N = 6;
        bool inject_joytest_midrun = false;
#ifdef MUT_ADVANCE_WITHOUT_REGISTER_ACCESS
        inject_joytest_midrun = true;
#endif
        // Walk N steps by hand (not via walk_ring, so reg_address_in is
        // never touched by a read() call during the run).
        static const int case1[4][2] = {{0,0},{1,0},{1,1},{0,1}};
        h.step_port1(0, 0, 0, 0, 0);
        int phase = 0;
        for (int i = 0; i < N; ++i) {
            phase = (phase + 1) & 3;
            h.step_port1(0, case1[phase][0], case1[phase][1], 0, 0);
            if (inject_joytest_midrun && i == N / 2) h.joytest_write();
            h.dut->reg_address_in = A_UNMAPPED;  // still parked, never read
            h.dut->eval();
        }
        uint16_t after = h.read(A_JOY0DAT);
        uint8_t delta = static_cast<uint8_t>((after & 0xFF) - (base & 0xFF));
        checks.expect(
            delta == N,
            "ADVANCE-WITHOUT-REGISTER-ACCESS (p.229 opus:8-mouse.c3, "
            "sol:8-mouse.c5): the counters advance purely from quadrature-"
            "pin transitions -- 6 valid transitions driven with "
            "reg_address_in parked on an address this module does not "
            "decode for the entire run still produce a delta of exactly 6 "
            "once finally read. Measured base=" + hex16(base) + " after=" +
            hex16(after) + " delta=" + std::to_string(delta) + ".");
    }

    // ================= A4: EXACTLY-TWO-LINES-PER-AXIS =================
    // (p.229 opus:8-mouse.c5, sol:8-mouse.c6; p.233 opus:8-fig8-3.c1..c8,
    //  sol:8-fig8-3.c1..c4 -- axis-routing halves only)
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);

        // H alone (bit 2): expect low byte to move, high byte fixed.
        h.step_port1(0, 1, 0, 0, 0);
        uint16_t after_h = h.read(A_JOY0DAT);
        bool h_moves_low_only = ((after_h & 0xFF) != (base & 0xFF)) &&
                                 ((after_h >> 8) == (base >> 8));

        // reset back to baseline
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base2 = h.read(A_JOY0DAT);

        // HQ alone (bit 0): expect low byte to move, high byte fixed.
        h.step_port1(0, 0, 1, 0, 0);
        uint16_t after_hq = h.read(A_JOY0DAT);
        bool hq_moves_low_only = ((after_hq & 0xFF) != (base2 & 0xFF)) &&
                                  ((after_hq >> 8) == (base2 >> 8));

        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base3 = h.read(A_JOY0DAT);

        // V alone (bit 3, or mutated to bit 0/HQ): expect HIGH byte to move.
        int v_bit_is_quad_instead = 0;
#ifdef MUT_EXACTLY_TWO_LINES_PER_AXIS
        v_bit_is_quad_instead = 1;  // drive HQ (bit 0) instead of V (bit 3)
#endif
        if (!v_bit_is_quad_instead) h.step_port1(1, 1, 0, 0, 0);
        else h.step_port1(0, 0, 1, 0, 0);
        uint16_t after_v = h.read(A_JOY0DAT);
        bool v_moves_high_only = ((after_v >> 8) != (base3 >> 8)) &&
                                  ((after_v & 0xFF) == (base3 & 0xFF));

        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base4 = h.read(A_JOY0DAT);

        // VQ alone (bit 1): expect high byte to move.
        h.step_port1(1, 0, 1, 0, 0);
        uint16_t after_vq = h.read(A_JOY0DAT);
        bool vq_moves_high_only = ((after_vq >> 8) != (base4 >> 8)) &&
                                   ((after_vq & 0xFF) == (base4 & 0xFF));

        checks.expect(
            h_moves_low_only && hq_moves_low_only && v_moves_high_only && vq_moves_high_only,
            "EXACTLY-TWO-LINES-PER-AXIS (p.229 opus:8-mouse.c5, "
            "sol:8-mouse.c6; p.233 opus:8-fig8-3.c1, opus:8-fig8-3.c2, "
            "opus:8-fig8-3.c3, opus:8-fig8-3.c4, opus:8-fig8-3.c5, "
            "opus:8-fig8-3.c6, opus:8-fig8-3.c7, opus:8-fig8-3.c8, "
            "sol:8-fig8-3.c1, sol:8-fig8-3.c2, sol:8-fig8-3.c3, "
            "sol:8-fig8-3.c4): each of the four direction lines, toggled "
            "alone, moves exactly one axis's count and leaves the other "
            "fixed -- H,HQ move only the horizontal/X byte, V,VQ move only "
            "the vertical/Y byte, matching Figure 8-3's own pins-1&3-to-Y/"
            "pins-2&4-to-X routing. Measured H:" + hex16(base) + "->" +
            hex16(after_h) + " HQ:" + hex16(base2) + "->" + hex16(after_hq) +
            " V:" + hex16(base3) + "->" + hex16(after_v) + " VQ:" +
            hex16(base4) + "->" + hex16(after_vq) + ".");
    }

    // ================= A5: ONE-COUNT-PER-QUADRATURE-STATE-CHANGE =================
    // (p.229 opus:8-mouse.c6, sol:8-mouse.c7; p.281 opus:8-appa-joy.c86,
    //  sol:8-appa-joy.c51)
    // Deliberately a DIFFERENT transition count than any number a prior
    // classification pass reports having used -- reproduced independently,
    // not adopted (see SPECIFICATION.md, A5).
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);
        const int N_UP = 13;
        int requested_up = N_UP;
#ifdef MUT_ONE_COUNT_PER_QUADRATURE_STATE_CHANGE
        requested_up = N_UP + 1;  // one extra, unrequested step
#endif
        walk_ring(h, true, /*axis=*/1, true, requested_up);
        uint16_t after_up = h.read(A_JOY0DAT);
        int up_delta = static_cast<uint8_t>((after_up >> 8) - (base >> 8));

        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base2 = h.read(A_JOY0DAT);
        const int N_DOWN = 17;
        walk_ring(h, true, 1, false, N_DOWN);
        uint16_t after_down = h.read(A_JOY0DAT);
        int down_delta = static_cast<uint8_t>((base2 >> 8) - (after_down >> 8));

        checks.expect(
            up_delta == N_UP && down_delta == N_DOWN,
            "ONE-COUNT-PER-QUADRATURE-STATE-CHANGE (p.229 opus:8-mouse.c6, "
            "sol:8-mouse.c7; p.281 opus:8-appa-joy.c86, sol:8-appa-joy.c51): "
            "each single accepted quadrature transition advances or "
            "retreats its axis's count by exactly one, across a run of "
            "many -- 13 Case-1 transitions give a delta of exactly +13, "
            "17 Case-2 transitions give exactly -17 (numbers chosen "
            "independently of any prior classification pass's own "
            "measurement). Measured up delta=" + std::to_string(up_delta) +
            " (base=" + hex16(base) + " after=" + hex16(after_up) +
            "); down delta=" + std::to_string(down_delta) + " (base=" +
            hex16(base2) + " after=" + hex16(after_down) + ").");
    }

    // ================= A6: VERTICAL-CASE1-COUNTS-UP =================
    // (p.230 opus:8-fig8-2.c1, sol:8-fig8-2.c1; p.229 opus:8-mouse.c7,
    //  sol:8-mouse.c8 [general phase-determines-direction, primary
    //  citation -- also underlies A7-A9 on the same basis], opus:8-mouse.c13,
    //  sol:8-mouse.c10)
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);
        bool use_case2_instead = false;
#ifdef MUT_VERTICAL_CASE1_COUNTS_UP
        use_case2_instead = true;
#endif
        const int N = 9;
        walk_ring(h, true, /*axis=*/1, /*up=*/!use_case2_instead, N);
        uint16_t after = h.read(A_JOY0DAT);
        int delta = static_cast<int8_t>(static_cast<uint8_t>((after >> 8) - (base >> 8)));
        checks.expect(
            delta == N,
            "VERTICAL-CASE1-COUNTS-UP (p.230 opus:8-fig8-2.c1, "
            "sol:8-fig8-2.c1; p.229 opus:8-mouse.c7, sol:8-mouse.c8, "
            "opus:8-mouse.c13, sol:8-mouse.c10): driving the vertical pair "
            "through the Case-1 phase order (V leading VQ, the figure's "
            "'Count Up') increases the vertical count -- 9 Case-1 "
            "transitions give a delta of exactly +9, matching both the "
            "figure's label and the text's 'down...increment[s]' sentence. "
            "Measured base=" + hex16(base) + " after=" + hex16(after) +
            " delta=" + std::to_string(delta) + ".");
    }

    // ================= A7: VERTICAL-CASE2-COUNTS-DOWN =================
    // (p.230 opus:8-fig8-2.c2, sol:8-fig8-2.c2; p.229 opus:8-mouse.c15,
    //  sol:8-mouse.c12) -- same basis as A6, opposite ring order.
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);
        bool use_case1_instead = false;
#ifdef MUT_VERTICAL_CASE2_COUNTS_DOWN
        use_case1_instead = true;
#endif
        const int N = 9;
        walk_ring(h, true, /*axis=*/1, /*up=*/use_case1_instead, N);
        uint16_t after = h.read(A_JOY0DAT);
        int delta = static_cast<int8_t>(static_cast<uint8_t>((after >> 8) - (base >> 8)));
        checks.expect(
            delta == -N,
            "VERTICAL-CASE2-COUNTS-DOWN (p.230 opus:8-fig8-2.c2, "
            "sol:8-fig8-2.c2; p.229 opus:8-mouse.c15, sol:8-mouse.c12): "
            "driving the vertical pair through the Case-2 phase order (VQ "
            "leading V, the figure's 'Count Down') decreases the vertical "
            "count -- 9 Case-2 transitions give a delta of exactly -9. "
            "Measured base=" + hex16(base) + " after=" + hex16(after) +
            " delta=" + std::to_string(delta) + ".");
    }

    // ================= A8: HORIZONTAL-RIGHT-INCREMENTS =================
    // (p.229 opus:8-mouse.c12, sol:8-mouse.c9)
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);
        bool use_case2_instead = false;
#ifdef MUT_HORIZONTAL_RIGHT_INCREMENTS
        use_case2_instead = true;
#endif
        const int N = 6;
        walk_ring(h, true, /*axis=*/0, /*up=*/!use_case2_instead, N);
        uint16_t after = h.read(A_JOY0DAT);
        int delta = static_cast<int8_t>(static_cast<uint8_t>((after & 0xFF) - (base & 0xFF)));
        checks.expect(
            delta == N,
            "HORIZONTAL-RIGHT-INCREMENTS (p.229 opus:8-mouse.c12, "
            "sol:8-mouse.c9): driving the horizontal pair through the same "
            "phase order Figure 8-2 labels 'Count Up' for the vertical pair "
            "(H leading HQ) increases the horizontal count -- 6 such "
            "transitions give a delta of exactly +6, matching 'the mouse "
            "is moved to the right...increment[s]'. Measured base=" +
            hex16(base) + " after=" + hex16(after) + " delta=" +
            std::to_string(delta) + ".");
    }

    // ================= A9: HORIZONTAL-LEFT-DECREMENTS =================
    // (p.229 opus:8-mouse.c14, sol:8-mouse.c11)
    {
        Harness h;
        h.step_port1(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base = h.read(A_JOY0DAT);
        bool use_case1_instead = false;
#ifdef MUT_HORIZONTAL_LEFT_DECREMENTS
        use_case1_instead = true;
#endif
        const int N = 6;
        walk_ring(h, true, /*axis=*/0, /*up=*/use_case1_instead, N);
        uint16_t after = h.read(A_JOY0DAT);
        int delta = static_cast<int8_t>(static_cast<uint8_t>((after & 0xFF) - (base & 0xFF)));
        checks.expect(
            delta == -N,
            "HORIZONTAL-LEFT-DECREMENTS (p.229 opus:8-mouse.c14, "
            "sol:8-mouse.c11): driving the horizontal pair through the "
            "'Count Down' phase order (HQ leading H) decreases the "
            "horizontal count -- 6 such transitions give a delta of "
            "exactly -6, matching 'the mouse is moved to the "
            "left...decrement[s]'. Measured base=" + hex16(base) +
            " after=" + hex16(after) + " delta=" + std::to_string(delta) + ".");
    }

    // ================= A10-A17: Figure 8-2's V/VQ:D1/D0 truth table =================
    // One assertion per cell, per the task's own instruction. Each settles
    // the vertical pair to the named (V,VQ) state, with generous settle
    // ticks, and reads JOY0DAT bits [9:8] (=D1,D0 -- METHODOLOGY (3)). Each
    // mutation settles to the WRONG state while the oracle keeps expecting
    // the named cell's value, forcing exactly that one assertion red.

    // ---- A10: FIG8-2-V0-VQ0-D1 (p.229 opus:8-mouse.c8, p.230 sol:8-fig8-2.c3) ----
    {
        Harness h;
        int v = 0, vq = 0;
#ifdef MUT_FIG8_2_V0_VQ0_D1
        v = 1; vq = 1;
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d1 = (j0 >> 9) & 1;
        checks.expect(
            d1 == 1,
            "FIG8-2-V0-VQ0-D1 (p.229 opus:8-mouse.c8, p.230 sol:8-fig8-2.c3): "
            "at V=0, VQ=0, the decode's D1 output is 1. Measured JOY0DAT=" +
            hex16(j0) + " D1=" + std::to_string(d1) + ".");
    }

    // ---- A11: FIG8-2-V0-VQ0-D0 (p.229 opus:8-mouse.c8, p.230 sol:8-fig8-2.c4) ----
    {
        Harness h;
        int v = 0, vq = 0;
#ifdef MUT_FIG8_2_V0_VQ0_D0
        v = 1;  // flip V only (not VQ) -- D0=V^VQ is invariant under
                // flipping BOTH bits, so only a single-bit flip changes it
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d0 = (j0 >> 8) & 1;
        checks.expect(
            d0 == 0,
            "FIG8-2-V0-VQ0-D0 (p.229 opus:8-mouse.c8, p.230 sol:8-fig8-2.c4): "
            "at V=0, VQ=0, the decode's D0 output is 0. Measured JOY0DAT=" +
            hex16(j0) + " D0=" + std::to_string(d0) + ".");
    }

    // ---- A12: FIG8-2-V0-VQ1-D1 (p.229 opus:8-mouse.c9, p.230 sol:8-fig8-2.c5) ----
    {
        Harness h;
        int v = 0, vq = 1;
#ifdef MUT_FIG8_2_V0_VQ1_D1
        v = 1; vq = 0;
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d1 = (j0 >> 9) & 1;
        checks.expect(
            d1 == 0,
            "FIG8-2-V0-VQ1-D1 (p.229 opus:8-mouse.c9, p.230 sol:8-fig8-2.c5): "
            "at V=0, VQ=1, the decode's D1 output is 0. Measured JOY0DAT=" +
            hex16(j0) + " D1=" + std::to_string(d1) + ".");
    }

    // ---- A13: FIG8-2-V0-VQ1-D0 (p.229 opus:8-mouse.c9, p.230 sol:8-fig8-2.c6) ----
    {
        Harness h;
        int v = 0, vq = 1;
#ifdef MUT_FIG8_2_V0_VQ1_D0
        v = 1;  // flip V only -- see the note on the V0-VQ0-D0 mutation
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d0 = (j0 >> 8) & 1;
        checks.expect(
            d0 == 1,
            "FIG8-2-V0-VQ1-D0 (p.229 opus:8-mouse.c9, p.230 sol:8-fig8-2.c6): "
            "at V=0, VQ=1, the decode's D0 output is 1. Measured JOY0DAT=" +
            hex16(j0) + " D0=" + std::to_string(d0) + ".");
    }

    // ---- A14: FIG8-2-V1-VQ0-D1 (p.229 opus:8-mouse.c10, p.230 sol:8-fig8-2.c7) ----
    {
        Harness h;
        int v = 1, vq = 0;
#ifdef MUT_FIG8_2_V1_VQ0_D1
        v = 0; vq = 1;
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d1 = (j0 >> 9) & 1;
        checks.expect(
            d1 == 1,
            "FIG8-2-V1-VQ0-D1 (p.229 opus:8-mouse.c10, p.230 sol:8-fig8-2.c7): "
            "at V=1, VQ=0, the decode's D1 output is 1. Measured JOY0DAT=" +
            hex16(j0) + " D1=" + std::to_string(d1) + ".");
    }

    // ---- A15: FIG8-2-V1-VQ0-D0 (p.229 opus:8-mouse.c10, p.230 sol:8-fig8-2.c8) ----
    {
        Harness h;
        int v = 1, vq = 0;
#ifdef MUT_FIG8_2_V1_VQ0_D0
        v = 0;  // flip V only -- see the note on the V0-VQ0-D0 mutation
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d0 = (j0 >> 8) & 1;
        checks.expect(
            d0 == 1,
            "FIG8-2-V1-VQ0-D0 (p.229 opus:8-mouse.c10, p.230 sol:8-fig8-2.c8): "
            "at V=1, VQ=0, the decode's D0 output is 1. Measured JOY0DAT=" +
            hex16(j0) + " D0=" + std::to_string(d0) + ".");
    }

    // ---- A16: FIG8-2-V1-VQ1-D1 (p.229 opus:8-mouse.c11, p.230 sol:8-fig8-2.c9) ----
    {
        Harness h;
        int v = 1, vq = 1;
#ifdef MUT_FIG8_2_V1_VQ1_D1
        v = 0; vq = 0;
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d1 = (j0 >> 9) & 1;
        checks.expect(
            d1 == 0,
            "FIG8-2-V1-VQ1-D1 (p.229 opus:8-mouse.c11, p.230 sol:8-fig8-2.c9): "
            "at V=1, VQ=1, the decode's D1 output is 0. Measured JOY0DAT=" +
            hex16(j0) + " D1=" + std::to_string(d1) + ".");
    }

    // ---- A17: FIG8-2-V1-VQ1-D0 (p.229 opus:8-mouse.c11, p.230 sol:8-fig8-2.c10) ----
    {
        Harness h;
        int v = 1, vq = 1;
#ifdef MUT_FIG8_2_V1_VQ1_D0
        v = 0;  // flip V only -- see the note on the V0-VQ0-D0 mutation
#endif
        h.step_port1(/*axis=*/1, v, vq, 0, 0);
        h.idle(4);
        uint16_t j0 = h.read(A_JOY0DAT);
        int d0 = (j0 >> 8) & 1;
        checks.expect(
            d0 == 0,
            "FIG8-2-V1-VQ1-D0 (p.229 opus:8-mouse.c11, p.230 sol:8-fig8-2.c10): "
            "at V=1, VQ=1, the decode's D0 output is 0. Measured JOY0DAT=" +
            hex16(j0) + " D0=" + std::to_string(d0) + ".");
    }

    // ================= A18: JOY1DAT-MIRRORS-JOY0DAT =================
    // (p.233 opus:8-fig8-3.c11, sol:8-fig8-3.c6)
    {
        Harness h;
        h.step_port2(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base_up = h.read(A_JOY1DAT);
        bool use_case2_instead = false;
#ifdef MUT_JOY1DAT_MIRRORS_JOY0DAT
        use_case2_instead = true;
#endif
        const int N = 4;
        walk_ring(h, /*port1=*/false, /*axis=*/1, /*up=*/!use_case2_instead, N);
        uint16_t after_up = h.read(A_JOY1DAT);
        int up_delta = static_cast<int8_t>(static_cast<uint8_t>((after_up >> 8) - (base_up >> 8)));

        h.step_port2(0, 0, 0, 0, 0);
        h.idle(2);
        h.joytest_write();
        h.idle(1);
        uint16_t base_down = h.read(A_JOY1DAT);
        walk_ring(h, false, 1, /*up=*/false, N);
        uint16_t after_down = h.read(A_JOY1DAT);
        int down_delta = static_cast<int8_t>(static_cast<uint8_t>((after_down >> 8) - (base_down >> 8)));

        checks.expect(
            up_delta == N && down_delta == -N,
            "JOY1DAT-MIRRORS-JOY0DAT (p.233 opus:8-fig8-3.c11, "
            "sol:8-fig8-3.c6): port 2's quadrature pair reproduces the same "
            "direction sense as port 1's -- 4 Case-1 transitions on port "
            "2's vertical-equivalent pair give a delta of exactly +4 on "
            "JOY1DAT, and 4 Case-2 transitions give exactly -4, "
            "independently of port 1. Measured up base=" + hex16(base_up) +
            " after=" + hex16(after_up) + "; down base=" + hex16(base_down) +
            " after=" + hex16(after_down) + ".");
    }

    // ================= Declined claims (see SPECIFICATION.md for the full table) =================
    checks.record(
        "opus:8-fig8-2.c4 (p.230): image annotates one D0 pulse and the "
        "following D1 pulse with digits '1' and '0'. Subsumed by A10-A17, "
        "which grade the full V/VQ:D1/D0 table exhaustively, all four rows, "
        "both columns; the claims file itself flags this record 'borderline'.");
    checks.record(
        "opus:8-fig8-2.c5 (p.230): the in-figure table matches the page-229 "
        "text table. Subsumed by A10-A17, each of which cites a page-229-"
        "text uid together with a page-230-figure uid for the same cell; "
        "the claims file itself flags this record 'borderline'.");
    checks.record(
        "opus:8-fig8-3.c9 (p.233): JOY0DAT captioned '$DFF00A, read only'. "
        "Out of this package's subject (register address and access mode, "
        "not quadrature decoding) -- see SPECIFICATION.md organisation "
        "point 4.");
    checks.record(
        "sol:8-fig8-3.c5 (p.233): same JOY0DAT address/read-only caption "
        "(twin of opus:8-fig8-3.c9). Same reason.");
    checks.record(
        "opus:8-fig8-3.c10 (p.233): the two counter boxes are drawn as "
        "8+8=16 bit cells, one register. Out of this package's subject "
        "(register width, not quadrature decoding) -- see SPECIFICATION.md "
        "organisation point 4.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
