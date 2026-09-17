// Test purpose: page 328's "GAME CONTROLLER INTERFACE TO MOUSE/TRACKBALL
//   QUADRATURE INPUTS" section of Appendix E -- specifically the four claims
//   the classify stage identified as testable here rather than declined:
//   (a) the CONNECTOR PIN USAGE table's HARDWARE REGISTER column, composed
//   end to end (pin number -> mnemonic -> JOY[0/1]DAT byte half) for pins
//   1-4, which no accepted assertion's own label states as one proposition
//   even though its parts are separately graded elsewhere; (b) the ">127"
//   distance/direction ambiguity sentence; and (c) the mouse-counter
//   readability guarantee ("always valid and may be read at any time").
//   Package appDE-mouse-quadrature, fixture userio.v (same fixture as
//   hrm/ch8-mouse-quadrature-decode, which this Makefile is modelled on).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 328 (Appendix E). Table read from figures/p328.png, not from
//   p328.txt -- the text layer is OCR-damaged in the register column (see
//   NOTE ON THE PRINTED DEFECT below) even though the corpus-defects.md
//   rule for this appendix says the bindings are otherwise visually aligned.
// Non-goals (all graded elsewhere, in an accepted package, on the SAME
//   proposition this page restates -- declined below with the precedent
//   named in each record()): the generic "planar motion becomes pulse
//   trains" and "quadrature preserves direction and magnitude" sentences;
//   JOY0DAT/JOY1DAT's Y-high/X-low byte layout; the four Up/Down/Right/Left
//   direction-sense sentences; pins 5, 6 and 9 of this same table (button
//   and pot-gate pins, not quadrature pins). Also not this package's
//   subject: CIAAPRA bit-to-decimal-label numbering (the contested
//   candidate in the build brief concerns appDE-fire-buttons-ciaa and
//   appDE-portsig-cia-bindings, not any claim in this file); the wraparound
//   mechanism itself (hrm/ch8-mouse-counter-wraparound); Table 8-1's own
//   pin-to-mnemonic binding (hrm/ch8-controller-pin-function-map, p.228).
//
// BOUNDARY: nanomig/src/minimig-aga/userio.v module header -- ports clk,
//   reset, clk7_en, reg_address_in[8:1], data_in[15:0], data_out[15:0],
//   pot_cnt_en, _fire0, _fire1, _fire0_dat, _fire1_dat, _joy1[15:0],
//   _joy2[15:0], joy_ana1[15:0], joy_ana2[15:0], mouse_btn[2:0], kms_level,
//   kbd_mouse_type[1:0], kbd_mouse_data[7:0]. Register addresses and the
//   per-bit pin roles below are read directly from userio.v (parameters at
//   l.54-61; the port-1 counter logic at l.230-254; the data-output mux at
//   l.289-310), cross-checked against hrm/ch8-mouse-quadrature-decode's own
//   already-accepted methodology comment, which documents the identical
//   fixture.
//
// COMPOSITION, NOT DUPLICATION (A1-A4). Three separate facts, each already
//   graded in a different accepted package, chain together to make up one
//   printed table row:
//     pin number  -> mnemonic     Table 8-1, p.228   (ch8-controller-pin-function-map)
//     mnemonic    -> axis         Fig 8-2/8-3, p.229-230, 233 (ch8-mouse-quadrature-decode A4)
//     axis        -> byte half    p.230 text          (ch8-joydat-counter-layout)
//   No accepted assertion's own label states the composed proposition this
//   page's HARDWARE REGISTER column actually prints -- "pin 1 -> JOY[0/1]DAT
//   <15:8>", etc. -- as one claim, even though the RTL fact underneath it
//   was already measured once (ch8-mouse-quadrature-decode A4, using the
//   manual's mnemonics, not pin numbers). A1-A4 below measure it again here,
//   independently, as the composed pin-to-register-field proposition this
//   package owns, on BOTH JOY0DAT and JOY1DAT.
//
// NOTE ON THE PRINTED DEFECT (pin 2, A2 below). p.328's HARDWARE REGISTER
//   column prints a ROUND PARENTHESIS where a chevron belongs, at the pin 2
//   row only: `JOY[0/1]DAT(7:0>`. Verified directly against
//   figures/p328.png (plainly legible at its 200 dpi) and confirmed in
//   docs/references/amiga-hrm-3rd/appE/corpus-defects.md, which additionally
//   verified it at 600 dpi and names it a typesetting defect, not a scan
//   artefact (the same substitution recurs at pin 2 of p.329's table, one
//   row position, two consecutive tables). Pins 1, 3 and 4 print an ordinary
//   chevron; only pin 2 does not. corpus-defects.md separately flags that
//   `sol:E-mouse.c16` -- part of this row's own claim pair -- silently
//   normalised the round parenthesis to a chevron in the first reading. A2
//   below does not: its label carries `JOY[0/1]DAT(7:0>` exactly as printed.
//   This is a typesetting defect in the GLYPH only -- the field it denotes
//   is still bits [7:0], confirmed independently against the RTL and against
//   pins 1/3/4's own byte assignments, which is why A2 still grades PASS/FAIL
//   on the bit range rather than on the punctuation.
//
// METHODOLOGY. Register addresses and the per-bit pin roles, confirmed
//   directly against userio.v and matching hrm/ch8-mouse-quadrature-decode's
//   own already-accepted measurement of the same fixture: port 1's
//   quadrature decode (feeding JOY0DAT) is driven from input _joy2 via the
//   synchronizer pair _sjoy1/_djoy1 (userio.v:215-218, joy_swap hard 0 at
//   l.92); port 2's (feeding JOY1DAT) from _joy1 via _sjoy2/_djoy2. Within
//   the driving word, bit 2 (H, "leading") and bit 0 (HQ, "following") drive
//   the LOW byte (l.230-241); bit 3 (V, "leading") and bit 1 (VQ,
//   "following") drive the HIGH byte (l.243-254). A single isolated line,
//   toggled alone from a freshly-cleared baseline, moves exactly the byte
//   its axis owns and leaves the other fixed -- this is what A1-A4 each
//   measure once per pin, on both ports.
//
// READABILITY GUARANTEE (A6). p.328's own NOTE -- "The horizontal and
//   vertical counters are always valid and may be read at any time" -- is
//   one of three such guarantees printed across pp.328-329 (the other two,
//   for JOY[0/1]DAT generally and for the fire buttons, belong to different
//   packages). Measured independently before writing this file:
//   `grep -rniE 'always valid|read at any time'` over all 194 directories
//   under nanomig-hrm-tests/hrm/ returns 0 lines -- no package graded any of
//   the three before this one. The proposition is constructible here because
//   userio.v's counters advance on clk7_en regardless of bus access
//   (l.230-254) and the read is a pure combinational mux keyed only on
//   reg_address_in (l.289-310, no clock edge, no arm/status bit unlike
//   POTINP's POTGO-gated path) -- so a read may be issued repeatedly with no
//   settle time and without disturbing the counter.
//
// CLAIM COVERAGE: all 32 claims in appDE-mouse-quadrature's slice of
//   2026-09-17_appDE_packages.jsonl are cited below, in full (reader, id)
//   uid form, either inside an expect() label or inside a record() call with
//   a stated reason. 6 new graded assertions (12 claims, each combining the
//   sol and opus twin of the same proposition); 10 claims declined via
//   record() (20 claims -- also combining sol/opus twins), each against a
//   prior assertion in an accepted package that grades the identical
//   proposition.
//
// MUTATION LOG. Every entry below was actually run under WSL/Verilator:
//   `make clean && make MUT=<NAME> test` -> `cat out.txt` (make itself exits
//   non-zero on a red, so its own stdout stops before the `cat` step -- the
//   file is the authority, not make's exit code) -> confirm the named
//   assertion, and only that one, newly red against the 6-passed/0-failed
//   baseline -> `make clean && make test` -> confirm the baseline exactly
//   restored, before moving to the next one. All six are `stimulus`
//   mutations (a driven input changes; check_gates.py:586 does not accept
//   an `oracle`-only record) and all six came back red on the first
//   attempt -- no FIRST ATTEMPT STAYED GREEN case in this package.
//
// MUTATION PIN1-V-TO-HIGH-BYTE stimulus red: pin 1's own line (V, bit 3)
//   replaced by pin 2's line (H, bit 2) -> only this assertion newly red
//   (the low byte moves instead of the high byte). Reverted, baseline
//   restored.
// MUTATION PIN2-H-TO-LOW-BYTE stimulus red: pin 2's own line (H, bit 2)
//   replaced by pin 1's line (V, bit 3) -> only this assertion newly red
//   (the high byte moves instead of the low byte). Reverted, baseline
//   restored.
// MUTATION PIN3-VQ-TO-HIGH-BYTE stimulus red: pin 3's own line (VQ, bit 1)
//   replaced by pin 4's line (HQ, bit 0) -> only this assertion newly red.
//   Reverted, baseline restored.
// MUTATION PIN4-HQ-TO-LOW-BYTE stimulus red: pin 4's own line (HQ, bit 0)
//   replaced by pin 3's line (VQ, bit 1) -> only this assertion newly red.
//   Reverted, baseline restored.
// MUTATION COUNTS-AMBIGUOUS-BEYOND-127 stimulus red: the "beyond 127" delta
//   changed from 200 to 100 -- itself inside the page's own stated safe
//   range -> the signed decode then round-trips correctly (100==100)
//   instead of misreading both magnitude and sign, and the "unsafe_wrong"
//   clause goes false -> only this assertion newly red. Reverted, baseline
//   restored.
// MUTATION MOUSE-COUNTERS-READABLE-ANY-TIME stimulus red: an unrequested
//   extra quadrature transition injected during what is supposed to be a
//   held-constant 80-cycle wait -> the "later" read legitimately differs
//   from "immediate" -> only this assertion newly red. FIRST ATTEMPT
//   injected the extra step via the same walk() helper the setup phase
//   uses, which resets its axis to phase 0 before stepping -- from the
//   setup's own final phase, that reset-then-one-step happened to be a net
//   no-op (a Case-2 step back to (0,0) cancelling a Case-1 step forward
//   again), so the mutation stayed at 6 passed, 0 failed and proved
//   nothing. Fixed to track ring phase by hand across the whole block and
//   drive the injected step as a continuation of the SAME ring instead of
//   through walk() -> "5 passed, 1 failed", only this assertion newly red.
//   Reverted, baseline restored.
//
// Final rebuild after the last revert: 6 passed, 0 failed, matching the
// baseline captured in out.txt.

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

// Register addresses, as reg_address_in ([8:1]) values -- confirmed
// directly against userio.v's own parameter declarations (l.54-61).
constexpr int A_JOY0DAT  = 0x00a >> 1;
constexpr int A_JOY1DAT  = 0x00c >> 1;
constexpr int A_JOYTEST  = 0x036 >> 1;

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
        dut->kms_level = 0;      // never toggled -- keeps mouse0dat==0, the
                                  // keyboard/PS2 mouse-emulation path is not
                                  // this package's subject
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
    // settle fresh off whatever the pins currently show (same shape as
    // hrm/ch8-mouse-quadrature-decode's own harness).
    void idle(int n) {
        dut->clk7_en = 1;
        for (int i = 0; i < n; ++i) tick();
        dut->clk7_en = 0;
    }

    // Combinational readback (userio.v:289 is an always @(*) mux) -- no
    // clock edge needed.
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
    // clk7_en edges -- userio.v's own two-stage synchronizer needs both
    // before a pin change is seen as one step. axis selects which pair
    // (0=horizontal: bits 2,0; 1=vertical: bits 3,1); main/quad name the
    // manual's "leading"/"following" line within that pair (H/V and HQ/VQ
    // respectively). other_axis_* holds the OTHER pair steady, used to
    // isolate one pin's line alone.
    void step_port1(int axis, int main_bit, int quad_bit, int other_axis_main = 0, int other_axis_quad = 0) {
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
        dut->_joy2 = w;
        dut->clk7_en = 1;
        tick();
        tick();
        dut->clk7_en = 0;
    }
    // Same shape, port 2 (_joy1 -> JOY1DAT).
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

    // Walk n valid quadrature steps on the given axis, Case 1 (main leads
    // quad, "Count Up") if up==true, Case 2 otherwise. Always starts by
    // driving (0,0) and settling, so every call begins from a known phase.
    void walk(bool port1, int axis, bool up, int n, int other_main = 0, int other_quad = 0) {
        static const int case1[4][2] = {{0,0},{1,0},{1,1},{0,1}};
        static const int case2[4][2] = {{0,0},{0,1},{1,1},{1,0}};
        const int (*seq)[2] = up ? case1 : case2;
        if (port1) step_port1(axis, 0, 0, other_main, other_quad);
        else       step_port2(axis, 0, 0, other_main, other_quad);
        int phase = 0;
        for (int i = 0; i < n; ++i) {
            phase = (phase + 1) & 3;
            int m = seq[phase][0], q = seq[phase][1];
            if (port1) step_port1(axis, m, q, other_main, other_quad);
            else       step_port2(axis, m, q, other_main, other_quad);
        }
    }

    // Baseline: clear via JOYTEST, then settle -- used before every
    // measurement below, matching hrm/ch8-mouse-quadrature-decode's own
    // pattern (these counters have no reset branch of their own).
    uint16_t baseline1() {
        step_port1(0, 0, 0, 0, 0);
        idle(2);
        joytest_write();
        idle(1);
        return read(A_JOY0DAT);
    }
    uint16_t baseline2() {
        step_port2(0, 0, 0, 0, 0);
        idle(2);
        joytest_write();
        idle(1);
        return read(A_JOY1DAT);
    }

    Vuserio* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= A1: PIN1-V-TO-HIGH-BYTE =================
    // (p.328 sol:E-mouse.c15, opus:E-mouse.c17)
    // Table row: "1  V  Vertical pulses  JOY[0/1]DAT<15:8>". Pin 1 = V is
    // Table 8-1's job (ch8-controller-pin-function-map); V-moves-the-
    // vertical-count is ch8-mouse-quadrature-decode A4's job; the composed
    // proposition -- pin 1 specifically reaches the HIGH byte of
    // JOY[0/1]DAT, on both ports -- is measured fresh here.
    {
        Harness h;
        uint16_t base1 = h.baseline1();
        bool mutate = false;
#ifdef MUT_PIN1_V_TO_HIGH_BYTE
        mutate = true;  // drive H (pin 2's line) instead of V (pin 1's)
#endif
        int axis = mutate ? 0 : 1;  // 1 = V's pair (bit 3); 0 = H's pair (bit 2), under mutation
        h.step_port1(axis, 1, 0, 0, 0);
        uint16_t after1 = h.read(A_JOY0DAT);
        bool port1_ok = ((after1 >> 8) != (base1 >> 8)) && ((after1 & 0xFF) == (base1 & 0xFF));

        Harness h2;
        uint16_t base2 = h2.baseline2();
        h2.step_port2(axis, 1, 0, 0, 0);
        uint16_t after2 = h2.read(A_JOY1DAT);
        bool port2_ok = ((after2 >> 8) != (base2 >> 8)) && ((after2 & 0xFF) == (base2 & 0xFF));

        checks.expect(
            port1_ok && port2_ok,
            "PIN1-V-TO-HIGH-BYTE (p.328 sol:E-mouse.c15, opus:E-mouse.c17): "
            "connector pin 1 (mnemonic V, \"Vertical pulses\"), driven alone "
            "from a cleared baseline, moves only the HIGH byte of "
            "JOY[0/1]DAT<15:8> and leaves the low byte fixed, on both "
            "JOY0DAT and JOY1DAT -- the composed pin-to-register-field "
            "binding this table's HARDWARE REGISTER column prints for row "
            "1. Measured JOY0DAT " + hex16(base1) + "->" + hex16(after1) +
            "; JOY1DAT " + hex16(base2) + "->" + hex16(after2) + ".");
    }

    // ================= A2: PIN2-H-TO-LOW-BYTE =================
    // (p.328 sol:E-mouse.c16, opus:E-mouse.c18)
    // Table row, AS PRINTED (see the NOTE ON THE PRINTED DEFECT above):
    // "2  H  Horizontal pulses  JOY[0/1]DAT(7:0>" -- a round parenthesis,
    // not a chevron, verified against figures/p328.png and
    // corpus-defects.md's 600 dpi check. The label below carries that
    // glyph unchanged; the byte range it denotes ([7:0], the LOW byte) is
    // still what is graded PASS/FAIL, cross-checked against pins 1/3/4's
    // own (undamaged) chevron notation for the same byte pairing.
    {
        Harness h;
        uint16_t base1 = h.baseline1();
        bool mutate = false;
#ifdef MUT_PIN2_H_TO_LOW_BYTE
        mutate = true;  // drive V (pin 1's line) instead of H (pin 2's)
#endif
        int axis = mutate ? 1 : 0;  // 0 = H's pair (bit 2); 1 = V's pair (bit 3), under mutation
        h.step_port1(axis, 1, 0, 0, 0);
        uint16_t after1 = h.read(A_JOY0DAT);
        bool port1_ok = ((after1 & 0xFF) != (base1 & 0xFF)) && ((after1 >> 8) == (base1 >> 8));

        Harness h2;
        uint16_t base2 = h2.baseline2();
        h2.step_port2(axis, 1, 0, 0, 0);
        uint16_t after2 = h2.read(A_JOY1DAT);
        bool port2_ok = ((after2 & 0xFF) != (base2 & 0xFF)) && ((after2 >> 8) == (base2 >> 8));

        checks.expect(
            port1_ok && port2_ok,
            "PIN2-H-TO-LOW-BYTE (p.328 sol:E-mouse.c16, opus:E-mouse.c18): "
            "connector pin 2 (mnemonic H, \"Horizontal pulses\"), driven "
            "alone from a cleared baseline, moves only the LOW byte -- "
            "printed on this row as JOY[0/1]DAT(7:0>, a round parenthesis "
            "where pins 1/3/4 print a chevron (typesetting defect, held as "
            "printed, not corrected here) -- and leaves the high byte "
            "fixed, on both JOY0DAT and JOY1DAT. Measured JOY0DAT " +
            hex16(base1) + "->" + hex16(after1) + "; JOY1DAT " +
            hex16(base2) + "->" + hex16(after2) + ".");
    }

    // ================= A3: PIN3-VQ-TO-HIGH-BYTE =================
    // (p.328 sol:E-mouse.c17, opus:E-mouse.c19)
    // Table row: "3  VQ  Vertical quadrature pulses  JOY[0/1]DAT<15:8>".
    {
        Harness h;
        uint16_t base1 = h.baseline1();
        bool mutate = false;
#ifdef MUT_PIN3_VQ_TO_HIGH_BYTE
        mutate = true;  // drive HQ (pin 4's line) instead of VQ (pin 3's)
#endif
        int axis = mutate ? 0 : 1;  // 1 = VQ's pair (bit 1); 0 = HQ's pair (bit 0), under mutation
        h.step_port1(axis, 0, 1, 0, 0);
        uint16_t after1 = h.read(A_JOY0DAT);
        bool port1_ok = ((after1 >> 8) != (base1 >> 8)) && ((after1 & 0xFF) == (base1 & 0xFF));

        Harness h2;
        uint16_t base2 = h2.baseline2();
        h2.step_port2(axis, 0, 1, 0, 0);
        uint16_t after2 = h2.read(A_JOY1DAT);
        bool port2_ok = ((after2 >> 8) != (base2 >> 8)) && ((after2 & 0xFF) == (base2 & 0xFF));

        checks.expect(
            port1_ok && port2_ok,
            "PIN3-VQ-TO-HIGH-BYTE (p.328 sol:E-mouse.c17, opus:E-mouse.c19): "
            "connector pin 3 (mnemonic VQ, \"Vertical quadrature pulses\"), "
            "driven alone from a cleared baseline, moves only the HIGH "
            "byte of JOY[0/1]DAT<15:8> and leaves the low byte fixed, on "
            "both JOY0DAT and JOY1DAT. Measured JOY0DAT " + hex16(base1) +
            "->" + hex16(after1) + "; JOY1DAT " + hex16(base2) + "->" +
            hex16(after2) + ".");
    }

    // ================= A4: PIN4-HQ-TO-LOW-BYTE =================
    // (p.328 sol:E-mouse.c18, opus:E-mouse.c20)
    // Table row: "4  HQ  Horizontal quadrature pulses  JOY[0/1]DAT<7:0>".
    {
        Harness h;
        uint16_t base1 = h.baseline1();
        bool mutate = false;
#ifdef MUT_PIN4_HQ_TO_LOW_BYTE
        mutate = true;  // drive VQ (pin 3's line) instead of HQ (pin 4's)
#endif
        int axis = mutate ? 1 : 0;  // 0 = HQ's pair (bit 0); 1 = VQ's pair (bit 1), under mutation
        h.step_port1(axis, 0, 1, 0, 0);
        uint16_t after1 = h.read(A_JOY0DAT);
        bool port1_ok = ((after1 & 0xFF) != (base1 & 0xFF)) && ((after1 >> 8) == (base1 >> 8));

        Harness h2;
        uint16_t base2 = h2.baseline2();
        h2.step_port2(axis, 0, 1, 0, 0);
        uint16_t after2 = h2.read(A_JOY1DAT);
        bool port2_ok = ((after2 & 0xFF) != (base2 & 0xFF)) && ((after2 >> 8) == (base2 >> 8));

        checks.expect(
            port1_ok && port2_ok,
            "PIN4-HQ-TO-LOW-BYTE (p.328 sol:E-mouse.c18, opus:E-mouse.c20): "
            "connector pin 4 (mnemonic HQ, \"Horizontal quadrature "
            "pulses\"), driven alone from a cleared baseline, moves only "
            "the LOW byte of JOY[0/1]DAT<7:0> and leaves the high byte "
            "fixed, on both JOY0DAT and JOY1DAT. Measured JOY0DAT " +
            hex16(base1) + "->" + hex16(after1) + "; JOY1DAT " +
            hex16(base2) + "->" + hex16(after2) + ".");
    }

    // ================= A5: COUNTS-AMBIGUOUS-BEYOND-127 =================
    // (p.328 sol:E-mouse.c9, opus:E-mouse.c10)
    // "if either count changes by more than 127, both distance and
    // direction become ambiguous." Graded, not declined: this is the
    // CONSEQUENCE of the wrap (hrm/ch8-mouse-counter-wraparound grades the
    // wrap mechanism itself), a different axis the classify stage found
    // stated only on this page. A delta within the stated safe range
    // (<=127) round-trips exactly through the natural signed-8-bit
    // reading; a delta beyond it does not -- it reads back as a SHORTER,
    // OPPOSITE-SIGN value that exactly matches what a genuinely different,
    // smaller, opposite motion would also produce, so the register alone
    // cannot tell the two apart.
    {
        Harness h;
        const int N_SAFE = 100;  // <= 127: the page's own stated safe range
        uint16_t base1 = h.baseline1();
        h.walk(true, 0, true, N_SAFE);  // horizontal axis, N_SAFE up-steps
        uint16_t after_safe = h.read(A_JOY0DAT);
        int8_t decode_safe = static_cast<int8_t>(static_cast<uint8_t>((after_safe & 0xFF) - (base1 & 0xFF)));
        bool safe_correct = (decode_safe == N_SAFE);

        int n_unsafe = 200;  // > 127, per the page's own stated threshold
#ifdef MUT_COUNTS_AMBIGUOUS_BEYOND_127
        n_unsafe = 100;  // pulled back inside the safe range the claim names
#endif
        uint16_t base2 = h.baseline1();
        h.walk(true, 0, true, n_unsafe);
        uint16_t after_unsafe = h.read(A_JOY0DAT);
        uint8_t raw_unsafe = static_cast<uint8_t>((after_unsafe & 0xFF) - (base2 & 0xFF));
        int8_t decode_unsafe = static_cast<int8_t>(raw_unsafe);
        int m = 256 - n_unsafe;  // the complementary, opposite-direction motion
        bool unsafe_wrong = (decode_unsafe != n_unsafe) && (static_cast<int>(decode_unsafe) == -m);

        uint16_t base3 = h.baseline1();
        h.walk(true, 0, false, m);  // a genuinely different, smaller, opposite motion
        uint16_t after_opposite = h.read(A_JOY0DAT);
        uint8_t raw_opposite = static_cast<uint8_t>((after_opposite & 0xFF) - (base3 & 0xFF));
        bool collides = (raw_unsafe == raw_opposite);

        checks.expect(
            safe_correct && unsafe_wrong && collides,
            "COUNTS-AMBIGUOUS-BEYOND-127 (p.328 sol:E-mouse.c9, "
            "opus:E-mouse.c10): a delta inside the page's stated safe range "
            "(driven +" + std::to_string(N_SAFE) + ") round-trips exactly "
            "through the signed reading; a delta beyond it (driven +" +
            std::to_string(n_unsafe) + ") does not -- it decodes as " +
            std::to_string(static_cast<int>(decode_unsafe)) + ", matching "
            "neither its own distance nor its own direction, and that "
            "misread value exactly collides with a genuinely different, "
            "smaller, opposite motion (driven -" + std::to_string(m) +
            "): both land on the identical raw byte, so the register alone "
            "cannot distinguish the two -- both distance and direction "
            "become ambiguous once a single interval's change exceeds 127. "
            "Measured safe base=" + hex16(base1) + " after=" + hex16(after_safe) +
            "; unsafe base=" + hex16(base2) + " after=" + hex16(after_unsafe) +
            "; opposite base=" + hex16(base3) + " after=" + hex16(after_opposite) + ".");
    }

    // ================= A6: MOUSE-COUNTERS-READABLE-ANY-TIME =================
    // (p.328 sol:E-mouse.c14, opus:E-mouse.c15)
    // NOTE: "The horizontal and vertical counters are always valid and may
    // be read at any time." Measured independently before writing this
    // file: grep -rniE 'always valid|read at any time' over all 194
    // directories under nanomig-hrm-tests/hrm/ returns 0 lines -- no
    // package grades any of the three such guarantees pp.328-329 print.
    // This is the mouse-counter one.
    {
        Harness h;
        uint16_t base = h.baseline1();
        // Put real, non-trivial content in both bytes: 5 valid Case-1 steps
        // on the vertical axis, tracking phase by hand (not via walk(),
        // which resets to phase 0 on every call) so the injected mutation
        // step below can continue this SAME ring instead of first undoing
        // it -- a reset-then-step cancels itself and would make the
        // mutation vacuous.
        static const int case1[4][2] = {{0,0},{1,0},{1,1},{0,1}};
        int phase = 0;
        h.step_port1(1, 0, 0, 0, 0);
        for (int i = 0; i < 5; ++i) {
            phase = (phase + 1) & 3;
            h.step_port1(1, case1[phase][0], case1[phase][1], 0, 0);
        }

        // Repeated reads with zero clock activity between them return the
        // identical value -- reading is non-destructive/idempotent, unlike
        // e.g. a clear-on-read status register.
        uint16_t r1 = h.read(A_JOY0DAT);
        uint16_t r2 = h.read(A_JOY0DAT);
        uint16_t r3 = h.read(A_JOY0DAT);
        bool idempotent = (r1 == r2) && (r2 == r3);

        // A read taken immediately (no settle beyond the step's own
        // required two clk7_en edges) equals a read of the same, untouched
        // state taken much later -- no extra window is needed before a
        // read may be trusted "at any time".
        uint16_t immediate = h.read(A_JOY0DAT);
        bool pins_actually_changed_during_wait = false;
#ifdef MUT_MOUSE_COUNTERS_READABLE_ANY_TIME
        pins_actually_changed_during_wait = true;  // an unrequested extra step during the "held" wait
#endif
        if (pins_actually_changed_during_wait) {
            // One more valid transition, continuing the same ring from its
            // current phase -- an unrequested change during what should be
            // a held-constant wait.
            phase = (phase + 1) & 3;
            h.step_port1(1, case1[phase][0], case1[phase][1], 0, 0);
        }
        h.idle(80);  // a long, arbitrary wait
        uint16_t later = h.read(A_JOY0DAT);
        bool stable_over_time = (immediate == later);

        checks.expect(
            idempotent && stable_over_time,
            "MOUSE-COUNTERS-READABLE-ANY-TIME (p.328 sol:E-mouse.c14, "
            "opus:E-mouse.c15): three consecutive reads with no clock "
            "activity between them return the identical value (reading is "
            "non-destructive), and a read taken the instant a transition "
            "registers equals a read of the same, untouched state taken "
            "80 clk7_en cycles later -- no arm, no settle window, and no "
            "special access sequence is needed: the counters are valid and "
            "may be read at any time. Measured base=" + hex16(base) +
            " r1/r2/r3=" + hex16(r1) + "/" + hex16(r2) + "/" + hex16(r3) +
            " immediate=" + hex16(immediate) + " later=" + hex16(later) + ".");
    }

    // ================= Declined claims =================
    // Each pair below states the SAME proposition an accepted assertion in
    // a different package already grades at this same fixture -- the
    // condition the build brief requires for a decline, not merely a
    // resemblance.

    checks.record(
        "sol:E-mouse.c1, opus:E-mouse.c2 (p.328): \"a mouse or trackball "
        "translates planar motion into pulse trains\" -- already graded, "
        "same proposition: hrm/ch8-mouse-quadrature-decode A1 "
        "(LIVENESS-KNOWN-DISTINCT-AXIS-COUNTS) and A4 "
        "(EXACTLY-TWO-LINES-PER-AXIS) grade, at this same fixture, that "
        "planar motion resolves into independent pulse trains on "
        "independent quadrature lines; the near-identical p.229 sentence "
        "underlies both. This page's generic restatement adds nothing new.");

    checks.record(
        "sol:E-mouse.c2, opus:E-mouse.c3 (p.328): \"quadrature techniques "
        "preserve direction as well as magnitude\" -- already graded, same "
        "proposition: hrm/ch8-mouse-quadrature-decode A6-A9 "
        "(VERTICAL-CASE1-COUNTS-UP, VERTICAL-CASE2-COUNTS-DOWN, "
        "HORIZONTAL-RIGHT-INCREMENTS, HORIZONTAL-LEFT-DECREMENTS) grade "
        "that the phase relationship between the two lines, not the pulse "
        "count alone, sets the sign of the count -- exactly what "
        "\"preserves direction as well as magnitude\" states.");

    checks.record(
        "sol:E-mouse.c3, opus:E-mouse.c4 (p.328): \"JOY0DAT and JOY1DAT "
        "become counter registers, y displacement in the high byte and x "
        "in the low byte\" -- already graded, same proposition: "
        "hrm/ch8-joydat-counter-layout grades this exact byte assignment "
        "from the printed p.230 figure; userio.v:244-252 (high byte) "
        "against :231-239 (low byte) is the same RTL evidence.");

    checks.record(
        "sol:E-mouse.c4, opus:E-mouse.c5 (p.328): \"Up: y decrements\" -- "
        "already graded, same proposition: "
        "hrm/ch8-mouse-quadrature-decode A7 (VERTICAL-CASE2-COUNTS-DOWN) "
        "grades this exact direction sense at this fixture.");

    checks.record(
        "sol:E-mouse.c5, opus:E-mouse.c6 (p.328): \"Down: y increments\" -- "
        "already graded, same proposition: "
        "hrm/ch8-mouse-quadrature-decode A6 (VERTICAL-CASE1-COUNTS-UP) "
        "grades this exact direction sense at this fixture.");

    checks.record(
        "sol:E-mouse.c6, opus:E-mouse.c7 (p.328): \"Right: x increments\" "
        "-- already graded, same proposition: "
        "hrm/ch8-mouse-quadrature-decode A8 (HORIZONTAL-RIGHT-INCREMENTS) "
        "grades this exact direction sense at this fixture.");

    checks.record(
        "sol:E-mouse.c7, opus:E-mouse.c8 (p.328): \"Left: x decrements\" -- "
        "already graded, same proposition: "
        "hrm/ch8-mouse-quadrature-decode A9 (HORIZONTAL-LEFT-DECREMENTS) "
        "grades this exact direction sense at this fixture.");

    checks.record(
        "sol:E-mouse.c19, opus:E-mouse.c21 (p.328): pin 5 is UBUT*, "
        "\"Unused mouse button; See Proportional Inputs\" -- already "
        "graded, same proposition: hrm/ch8-controller-pin-function-map "
        "(Table 8-1, p.228) Group B2 grades pin 5 as the port's X/'Right "
        "POT' gate, and hrm/ch8-mouse-button-pins P3 grades the third "
        "mouse button reaching the pin-5 path (userio.v:169 potcap[2] <= "
        "joy2_pin5). This page adds only the mnemonic and the "
        "cross-reference -- not a quadrature pin, out of this package's "
        "subject either way.");

    checks.record(
        "sol:E-mouse.c20, opus:E-mouse.c22 (p.328): pin 6 is LBUT*, \"Left "
        "mouse button; See Fire Button\" -- already graded, same "
        "proposition: hrm/ch8-controller-pin-function-map (Table 8-1, "
        "p.228) grades pin 6 = left mouse button / joystick Button 1, and "
        "hrm/ch8-ciaapra-fire-buttons grades the CIAAPRA side of "
        "userio.v:313 (_fire0 = ... _sjoy1[4] & _mleft, _mleft = "
        "~mouse_btn[0] at l.361). This is the pin-to-function half only; "
        "it does not touch the contested CIAAPRA bit-to-decimal-label "
        "numbering the build brief raises for appDE-fire-buttons-ciaa and "
        "appDE-portsig-cia-bindings, which is a different pair of claims "
        "in a different package.");

    checks.record(
        "sol:E-mouse.c23, opus:E-mouse.c25 (p.328): pin 9 is RBUT*, "
        "\"Right mouse button; See Proportional Inputs\" -- already "
        "graded, same proposition: hrm/ch8-controller-pin-function-map "
        "(Table 8-1, p.228) Group B grades pin 9 as the port's Y/'Left "
        "POT' gate, and hrm/ch8-mouse-button-pins P2 grades the second "
        "mouse button reaching the pin-9 path (userio.v:174 potcap[1] <= "
        "_mright & _djoy1[5] & ...). Not a quadrature pin, out of this "
        "package's subject either way.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
