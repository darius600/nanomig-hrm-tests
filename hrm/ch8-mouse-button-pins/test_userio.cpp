// Test purpose: HRM conformance package for printed page 232's mouse
//   second- and third-button paths (controller-port pins 9 and 5) and the
//   module's support for as many as three mouse buttons, against the
//   `userio` module compiled alone. 11 claims, from
//   work/ch8-s1-build/ch8-mouse-button-pins.jsonl. Full derivation in
//   SPECIFICATION.md (committed as 610dfc4 before this file was written).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.232 ("There
//   are two buttons on the standard Amiga mouse. However, the control
//   circuitry and software support up to three buttons." / "Button 2
//   (right button on Amiga mouse) is connected to pin 9 of the controller
//   ports, one of the proportional pins." / "Button 3, when used, is
//   connected to pin 5, the other proportional controller input.").
// Fixture boundary: `userio` (userio.v) alone -- a single-file module, no
//   sibling files needed to elaborate it (confirmed by this file's own
//   Makefile SRC list, the only place a module could be added; that
//   Makefile is byte-identical across hrm/ch8-register-map-userio,
//   hrm/ch8-joystick-switch-decode and hrm/ch8-mouse-quadrature-decode --
//   the three sibling Makefiles read per the build brief for the build
//   pattern, and nothing else in those three directories was opened).
//
// DISCLOSURE (beyond the three sibling Makefiles named by the brief): to
//   settle the boundary question the brief posed, this build also read, in
//   full, hrm/ch8-ciaapra-fire-buttons/SPECIFICATION.md and
//   test_ch8-ciaapra-fire-buttons.cpp -- an ALREADY-BUILT, green sibling on
//   a DIFFERENT fixture (ciaa), read before this package's own
//   SPECIFICATION.md was written, specifically to check (a) which uids it
//   declined by module-boundary reasoning and whether either appears in
//   this package's own claims file (neither does -- checked directly
//   against ch8-mouse-button-pins.jsonl's 11 ids), and (b) what "primary
//   fire button reaching CIAAPRA" content that package already owns, so
//   this package does not re-derive it. No RTL fact and no port name of
//   `userio` came from that read; it is a different module compiled with a
//   different Makefile. This disclosure is in addition to, not a
//   substitute for, the three sibling Makefiles.
//
// RTL FACTS USED THROUGHOUT (userio.v, read after SPECIFICATION.md was
//   committed):
//   - mouse_btn[2:0] is the module's mouse-button input (userio.v:46).
//     Active-high-pressed: _mleft=~mouse_btn[0], _mright=~mouse_btn[1],
//     _mthird=~mouse_btn[2] (userio.v:361-363) -- the internal signals are
//     active-low, the port itself is active-high.
//   - bit 0 (_mleft) feeds _fire0 = _sjoy1[4] & _mleft (userio.v:313,
//     cd32pad hardwired 0 at userio.v:91 so the ternary's true branch is
//     dead). No claim in this package characterizes _fire0 itself -- that
//     is hrm/ch8-ciaapra-fire-buttons' content on a different fixture; this
//     package observes only that toggling bit 0 changes this one module
//     output distinctly from bits 1/2's effects (P1).
//   - bit 1 (_mright) and bit 2 (_mthird) feed potcap[1] and potcap[0]
//     respectively (userio.v:171-176; the cd32pad/joy1enable-gated branches
//     are dead since cd32pad=0 hardwired at userio.v:91):
//       potcap[1] <= _mright & _djoy1[5] & ~(potreg[11] & ~potreg[10]);
//       potcap[0] <= _mthird & joy1_pin5;   // joy1_pin5 = ~(potreg[9] & ~potreg[8])
//     With potreg never written (this fixture never addresses POTGO, so
//     potreg stays its reset value of 0 throughout -- userio.v:99-105) and
//     with _joy1/_joy2 held at their idle-high default throughout (so
//     _djoy1[5], sourced from one of those two ports through the
//     _sjoy1/_djoy1 pipeline at userio.v:210-219, settles high and stays
//     there): potcap[1] reduces to _mright alone and potcap[0] to _mthird
//     alone. potcap is read back at POTINP (userio.v:59, address 9'h016 --
//     the register the manual calls POTGOR), bit 10 = potcap[1], bit 8 =
//     potcap[0] (userio.v:300-305).
//   - potcap[1]/potcap[0] also gate the two pot position counters
//     (userio.v:134-149, the joy_ana_en branch is dead since joy_ana_en is
//     hardwired 0 at userio.v:93): "if (!potcap[0]) pot0x <= pot0x+1;
//     if (!potcap[1]) pot0y <= pot0y+1;" once per pot_cnt_en pulse. POT0DAT
//     (userio.v:57, address 9'h012) reads back {pot0y, pot0x}
//     (userio.v:296-297) -- pin 9/potcap[1] gates the Y (high) byte, pin
//     5/potcap[0] gates the X (low) byte, two separate 8-bit registers.
//   - reg_address_in is the module's own [8:1] word-address input; the
//     values used below (0x09 for POT0DAT, 0x0B for POTINP) are each
//     register's own parameter (userio.v:54-70) shifted right by 1, exactly
//     as the module's own address comparisons do (e.g. userio.v:296:
//     "reg_address_in[8:1]==POT0DAT[8:1]"). 0x00 (never a match for any
//     parameter on userio.v:54-70) is used as an idle/non-selecting address
//     so this fixture never latches a write to POTGO.
//
// BOUNDARY (decided in SPECIFICATION.md before this RTL was read, carried
//   here unchanged): nothing below characterizes what _fire0's value means
//   at CIAAPRA, whether it is "independent of the joystick," or what "0"
//   there is defined to mean -- that is hrm/ch8-ciaapra-fire-buttons'
//   content, on the `ciaa` fixture, already built and green. P1 grades only
//   that userio's own bit-0 effect is distinct from bits 1/2's, inside this
//   module. opus:8-joy2.c3 / sol:8-joy.c19 (declined by that sibling) do
//   not appear in this package's own claims file -- confirmed again here,
//   nothing to decide.
//
// DECLINED: opus:8-mbtn.c9 ("Button 2 is the right button on the Amiga
//   mouse") -- see P2-DECLINED below.
//
// IDLE-HIGH DISCIPLINE (build-brief carryover): _joy1, _joy2, _fire0_dat,
//   _fire1_dat are all active-low-style ports on this module; every one of
//   them is driven to its idle (all-1s / 1) level explicitly in do_reset()
//   below and never left at Verilator's undriven default of 0, which on
//   this module's active-low nets would read as "pressed"/"asserted."
//   mouse_btn itself is active-HIGH-pressed (0 is its own idle level, so
//   Verilator's 0 default is safe for it, and it is still driven
//   explicitly below for clarity).
//
// MUTATION METHODOLOGY. This fixture's Makefile (byte-identical to the
//   three siblings read for the build pattern) mutates at COMPILE time:
//   `make MUT=<NAME>` defines MUT_<NAME>=1 via -CFLAGS, guarding an
//   `#ifdef MUT_<NAME>` block below that changes only the STIMULUS handed
//   to the module for that one assertion; the `expect()` call's own
//   comparison/expected-value code is never touched by any #ifdef. `make
//   clean` was run between every MUT switch (the Makefile's own obj_dir
//   reuse note). Baseline (no MUT define): 12 passed, 0 failed (A0-DIGITAL,
//   A0-COUNTER plus the 10 graded assertions; opus:8-mbtn.c9 is a
//   record(), not graded). Every one of the 10 mutations below was run
//   with its own MUT=<NAME>, confirmed red (each reddened exactly and only
//   its own assertion -- 11 passed, 1 failed in every one of the 10 runs,
//   never more), then reverted (no MUT define) and confirmed green again.
//
// MUTATION P1-BIT0-DISTINCT-EFFECT stimulus red: the "bit-0-only" stimulus drives bit 1 instead of bit 0 (MUT=P1A)
// MUTATION P1-COMPLEMENT-REMOVAL-DISTINGUISHES-BITS stimulus red: the "release bit 1" step releases bit 2 instead (MUT=P1B)
// MUTATION P2-BIT1-REACHES-PIN9-EXCLUSIVELY stimulus red: bit 1 stuck pressed throughout the matrix, ignoring the intended pattern (MUT=P2A)
// MUTATION P2-BIT1-REACHES-PIN9-TWIN stimulus red: bit 1 stuck released throughout the matrix, ignoring the intended pattern (MUT=P2B)
// MUTATION P2-PIN9-DRIVES-Y-COUNTER stimulus red: the counting phase drives mouse_btn=0 instead of bit 1 pressed (MUT=P2C)
// MUTATION P2-PIN9-DRIVES-Y-COUNTER-TWIN stimulus red: bit 1 stays pressed during the segment meant to be released (over-counts) (MUT=P2D)
// MUTATION P3-BIT2-REACHES-PIN5-EXCLUSIVELY stimulus red: bit 2 stuck pressed throughout the matrix, ignoring the intended pattern (MUT=P3A)
// MUTATION P3-BIT2-REACHES-PIN5-TWIN stimulus red: bit 2 stuck released throughout the matrix, ignoring the intended pattern (MUT=P3B)
// MUTATION P3-PIN5-IS-DISTINCT-SECOND-COUNTER stimulus red: the counting phase drives bit 1 instead of bit 2 (MUT=P3C)
// MUTATION P3-BOTH-COUNTERS-INDEPENDENT-SIMULTANEOUS stimulus red: bit 2 silently dropped from the "both pressed" stimulus (MUT=P3D)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vuserio.h"
#include "verilated.h"

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

namespace {
// Register addresses, each a manual-quoted-page-adjacent RTL parameter
// (userio.v:54-70) shifted right by 1 -- exactly the module's own
// [8:1]-vs-parameter[8:1] comparison basis.
const uint16_t ADDR_IDLE   = 0x00;  // matches none of userio.v's parameters
const uint16_t ADDR_POT0DAT = 0x012 >> 1;  // 0x09
const uint16_t ADDR_POTINP  = 0x016 >> 1;  // 0x0B
}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vuserio* dut = new Vuserio;
    Checks checks;

    int clk7_cycle = 0;
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    auto do_reset = [&]() {
        clk7_cycle = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->reg_address_in = ADDR_IDLE;
        dut->data_in = 0;
        dut->pot_cnt_en = 0;
        dut->_fire0_dat = 1;   // idle-high: active-low port, never left undriven
        dut->_fire1_dat = 1;
        dut->_joy1 = 0xFFFF;   // idle-high on every bit: active-low port
        dut->_joy2 = 0xFFFF;   // idle-high on every bit: active-low port
        dut->joy_ana1 = 0;     // dead path (joy_ana_en hardwired 0), defined for hygiene
        dut->joy_ana2 = 0;
        dut->mouse_btn = 0;    // idle for this port IS 0 (active-high-pressed)
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        for (int i = 0; i < 16; i++) tick();   // several clk7_en-qualified edges while reset=1
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    // Advance until at least one clk7_en-qualified edge has passed since the
    // last stimulus change, with margin (potcap needs exactly one; this
    // gives several).
    auto settle = [&]() {
        for (int n = 0; n < 3; n++) {
            while (!dut->clk7_en) tick();
            tick();
        }
    };

    // Deliver exactly one pot_cnt_en pulse coincident with a clk7_en-
    // qualified posedge, n times.
    auto pulse_counts = [&](int n) {
        for (int i = 0; i < n; i++) {
            while (!dut->clk7_en) tick();
            dut->pot_cnt_en = 1;
            tick();
            dut->pot_cnt_en = 0;
        }
    };

    auto read_reg = [&](uint16_t addr) -> uint16_t {
        dut->reg_address_in = addr;
        dut->eval();
        uint16_t v = dut->data_out;
        dut->reg_address_in = ADDR_IDLE;
        dut->eval();
        return v;
    };

    auto set_mouse_btn = [&](bool b0, bool b1, bool b2) {
        dut->mouse_btn = (b2 ? 4 : 0) | (b1 ? 2 : 0) | (b0 ? 1 : 0);
    };

    // ========================================================================
    // A0-DIGITAL -- FIXTURE DATA-PATH LIVENESS (digital/status basis). No
    // claim id, no manual page: the build brief's own requirement that the
    // fixture prove it can deliver a known, distinct word before any
    // read-based assertion is trusted. Two mutually-exclusive mouse_btn
    // patterns are driven and POTINP + _fire0 are read back; the expected
    // bit pattern is derived from the RTL facts recorded in the header
    // above (potcap[1]->POTINP bit10, potcap[0]->POTINP bit8, both
    // active-low), not assumed.
    // ========================================================================
    do_reset();
    {
        set_mouse_btn(true, true, false);   // bit0+bit1 pressed, bit2 idle
        settle();
        uint16_t v1 = read_reg(ADDR_POTINP);
        bool fire0_1 = dut->_fire0;
        checks.note("A0-DIGITAL-WORD1: mouse_btn=3'b011 -> POTINP=0x" +
                    std::to_string(v1) + " (_fire0=" + std::to_string(fire0_1) + ")");

        set_mouse_btn(false, false, true);  // bit2 pressed, bit0+bit1 idle (disjoint pattern)
        settle();
        uint16_t v2 = read_reg(ADDR_POTINP);
        bool fire0_2 = dut->_fire0;
        checks.note("A0-DIGITAL-WORD2: mouse_btn=3'b100 -> POTINP=0x" +
                    std::to_string(v2) + " (_fire0=" + std::to_string(fire0_2) + ")");

        bool word1_ok = ((v1 >> 10) & 1) == 0 && ((v1 >> 8) & 1) == 1;  // bit1 pressed->bit10=0, bit2 idle->bit8=1
        bool word2_ok = ((v2 >> 10) & 1) == 1 && ((v2 >> 8) & 1) == 0;  // bit1 idle->bit10=1, bit2 pressed->bit8=0
        bool distinct = (v1 != v2) && (fire0_1 != fire0_2);
        checks.expect(word1_ok && word2_ok && distinct,
            "A0-DIGITAL-LIVENESS: two disjoint, known mouse_btn patterns arrive at POTINP and _fire0 exactly as "
            "the RTL's own potcap/_mleft/_mright/_mthird wiring predicts, and the two reads are mutually "
            "distinguishable -- this fixture's digital read path is live, not stuck");
    }

    // ========================================================================
    // A0-COUNTER -- FIXTURE DATA-PATH LIVENESS (counter/accumulation
    // basis). Same requirement, the module's other observable mechanism:
    // holding both button-2 and button-3 pressed and pulsing pot_cnt_en a
    // known number of times must produce that exact, non-default count on
    // both POT0DAT bytes -- proving the counting path is live before any
    // "proportional pin" assertion below is trusted.
    // ========================================================================
    do_reset();
    {
        set_mouse_btn(false, true, true);
        settle();
        pulse_counts(10);
        uint16_t pot0dat = read_reg(ADDR_POT0DAT);
        uint8_t pot0x = pot0dat & 0xFF;
        uint8_t pot0y = (pot0dat >> 8) & 0xFF;
        checks.note("A0-COUNTER: 10 pot_cnt_en pulses with mouse_btn=3'b110 held -> POT0DAT X=" +
                    std::to_string((int)pot0x) + " Y=" + std::to_string((int)pot0y));
        checks.expect(pot0x == 10 && pot0y == 10,
            "A0-COUNTER-LIVENESS: 10 known pot_cnt_en pulses advance both POT0DAT bytes to exactly 10, not stuck "
            "at the reset default of 0 -- this fixture's counter read path is live");
    }

    // ========================================================================
    // P1 -- opus:8-mbtn.c2, sol:8-button.c2 (p.232, "up to three buttons"):
    // three button-input bits, each with its own distinct, non-overlapping
    // observable effect. Grades the count alone; characterizes none of the
    // three effects (see header BOUNDARY note on bit 0 / _fire0).
    // ========================================================================
    do_reset();
    {
        // S0: bit0 only. S1: bit1 only. S2: bit2 only.
#ifdef MUT_P1A
        set_mouse_btn(false, true, false);  // MUTATION: drives bit1 where bit0-only was intended
#else
        set_mouse_btn(true, false, false);
#endif
        settle();
        bool s0_fire0 = dut->_fire0;
        uint16_t s0_pot = read_reg(ADDR_POTINP);

        set_mouse_btn(false, true, false);
        settle();
        bool s1_fire0 = dut->_fire0;
        uint16_t s1_pot = read_reg(ADDR_POTINP);

        set_mouse_btn(false, false, true);
        settle();
        bool s2_fire0 = dut->_fire0;
        uint16_t s2_pot = read_reg(ADDR_POTINP);

        auto sig = [&](bool f, uint16_t p) { return (f ? 1u : 0u) | (((uint32_t)p) << 1); };
        uint32_t sig0 = sig(s0_fire0, s0_pot), sig1 = sig(s1_fire0, s1_pot), sig2 = sig(s2_fire0, s2_pot);
        checks.note("P1-SIGNATURES: bit0-only=(" + std::to_string(s0_fire0) + "," + std::to_string(s0_pot) +
                    ") bit1-only=(" + std::to_string(s1_fire0) + "," + std::to_string(s1_pot) +
                    ") bit2-only=(" + std::to_string(s2_fire0) + "," + std::to_string(s2_pot) + ")");
        checks.expect(sig0 != sig1 && sig1 != sig2 && sig0 != sig2,
            "P1-BIT0-DISTINCT-EFFECT (p.232, opus:8-mbtn.c2, quote 'the control circuitry and software support up "
            "to three buttons'): driving mouse_btn's three bits one at a time each produces its own "
            "observable effect (over _fire0 and POTINP together), pairwise distinct from the other two -- three "
            "independent button inputs, not two");
    }

    do_reset();
    {
        // Complement/removal method: all three pressed, then release one at
        // a time and confirm each removal changes exactly its own
        // observable and none of the other two -- a different measurement
        // method from P1's own individual-bit method, broadening coverage.
        set_mouse_btn(true, true, true);
        settle();
        bool base_fire0 = dut->_fire0;
        uint16_t base_pot = read_reg(ADDR_POTINP);

        set_mouse_btn(false, true, true);   // release bit0
        settle();
        bool rm0_fire0 = dut->_fire0;
        uint16_t rm0_pot = read_reg(ADDR_POTINP);
        bool rm0_ok = (rm0_fire0 != base_fire0) && (rm0_pot == base_pot);

        set_mouse_btn(true, true, true);
        settle();
#ifdef MUT_P1B
        set_mouse_btn(true, true, false);   // MUTATION: releases bit2 instead of bit1
#else
        set_mouse_btn(true, false, true);   // release bit1
#endif
        settle();
        bool rm1_fire0 = dut->_fire0;
        uint16_t rm1_pot = read_reg(ADDR_POTINP);
        bool rm1_ok = (rm1_fire0 == base_fire0) && (((rm1_pot >> 10) & 1) != ((base_pot >> 10) & 1)) &&
                      (((rm1_pot >> 8) & 1) == ((base_pot >> 8) & 1));

        set_mouse_btn(true, true, true);
        settle();
        set_mouse_btn(true, true, false);   // release bit2
        settle();
        bool rm2_fire0 = dut->_fire0;
        uint16_t rm2_pot = read_reg(ADDR_POTINP);
        bool rm2_ok = (rm2_fire0 == base_fire0) && (((rm2_pot >> 8) & 1) != ((base_pot >> 8) & 1)) &&
                      (((rm2_pot >> 10) & 1) == ((base_pot >> 10) & 1));

        checks.note("P1-COMPLEMENT: baseline=(" + std::to_string(base_fire0) + "," + std::to_string(base_pot) +
                    ") remove-bit0=(" + std::to_string(rm0_fire0) + "," + std::to_string(rm0_pot) +
                    ") remove-bit1=(" + std::to_string(rm1_fire0) + "," + std::to_string(rm1_pot) +
                    ") remove-bit2=(" + std::to_string(rm2_fire0) + "," + std::to_string(rm2_pot) + ")");
        checks.expect(rm0_ok && rm1_ok && rm2_ok,
            "P1-COMPLEMENT-REMOVAL-DISTINGUISHES-BITS (p.232, sol:8-button.c2, quote 'the control circuitry and "
            "software support up to three buttons', twin of opus:8-mbtn.c2, graded independently, complement "
            "method): starting from all three pressed, releasing any one of the three bits changes exactly its "
            "own observable and none of the other two -- three independently removable button inputs");
    }

    // ========================================================================
    // P2 -- button 2 (pin 9). Connectivity: opus:8-mbtn.c10, sol:8-button.c8
    // (core only -- see P2-DECLINED). Counter/"proportional": opus:8-mbtn.c11,
    // sol:8-button.c9.
    // ========================================================================
    checks.record(
        "P2-DECLINED (p.232, opus:8-mbtn.c9, quote 'Button 2 (right button on Amiga mouse) is connected to pin 9 "
        "of the controller ports, one of the proportional pins.'): not asserted -- the restatement is entirely "
        "the housing label \"right button.\" No signal in userio.v carries handedness (grep over this fixture's "
        "one source file for \"right\" or \"left\": zero hits in the module body). The quote's remaining, "
        "gradeable content -- button 2 connected to pin 9, pin 9 being one of the proportional pins -- is already "
        "graded below under opus:8-mbtn.c10 and opus:8-mbtn.c11. Declined, not re-measured with a weakened "
        "restatement.");

    do_reset();
    {
        // Matrix 1: vary (bit1, bit2), bit0 fixed idle. Proves bit1's effect
        // on POTINP bit10 is independent of bit2.
        const bool combo_b1[4] = {true, true, false, false};
        const bool combo_b2[4] = {false, true, true, false};
        bool ok = true;
        std::string trail;
        for (int i = 0; i < 4; i++) {
#ifdef MUT_P2A
            bool b1_drive = true;  // MUTATION: bit1 stuck pressed, ignoring the intended pattern
#else
            bool b1_drive = combo_b1[i];
#endif
            set_mouse_btn(false, b1_drive, combo_b2[i]);
            settle();
            uint16_t v = read_reg(ADDR_POTINP);
            bool bit10 = (v >> 10) & 1;
            bool expected_bit10 = !combo_b1[i];
            if (bit10 != expected_bit10) ok = false;
            trail += (bit10 == expected_bit10) ? "1" : "0";
        }
        checks.note("P2A-MATRIX (vary bit1,bit2 / bit0 idle): matches=" + trail);
        checks.expect(ok,
            "P2-BIT1-REACHES-PIN9-EXCLUSIVELY (p.232, opus:8-mbtn.c10, quote 'Button 2 ... is connected to pin 9 "
            "of the controller ports': connectivity half graded, housing label declined above): POTINP bit 10 "
            "(pin 9's status) tracks mouse_btn bit 1 exactly, across every combination of bit 2 underneath it");
    }

    do_reset();
    {
        // Matrix 2 (twin, broadened coverage): vary (bit1, bit0), bit2 fixed
        // idle. Proves bit1's effect on POTINP bit10 is ALSO independent of
        // bit0 -- a different pair from Matrix 1, not a repeat.
        const bool combo_b1[4] = {true, true, false, false};
        const bool combo_b0[4] = {false, true, true, false};
        bool ok = true;
        std::string trail;
        for (int i = 0; i < 4; i++) {
#ifdef MUT_P2B
            bool b1_drive = false;  // MUTATION: bit1 stuck released, ignoring the intended pattern
#else
            bool b1_drive = combo_b1[i];
#endif
            set_mouse_btn(combo_b0[i], b1_drive, false);
            settle();
            uint16_t v = read_reg(ADDR_POTINP);
            bool bit10 = (v >> 10) & 1;
            bool expected_bit10 = !combo_b1[i];
            if (bit10 != expected_bit10) ok = false;
            trail += (bit10 == expected_bit10) ? "1" : "0";
        }
        checks.note("P2B-MATRIX (vary bit1,bit0 / bit2 idle): matches=" + trail);
        checks.expect(ok,
            "P2-BIT1-REACHES-PIN9-TWIN (p.232, sol:8-button.c8, quote 'The right Amiga mouse button is connected "
            "to controller-port pin 9', connectivity half graded, twin of opus:8-mbtn.c10, graded independently "
            "under its own uid, different matrix than P2A): POTINP bit 10 tracks mouse_btn bit 1 exactly, across "
            "every combination of bit 0 underneath it");
    }

    do_reset();
    {
        set_mouse_btn(false, true, false);   // bit1 only
        settle();
#ifdef MUT_P2C
        set_mouse_btn(false, false, false);  // MUTATION: not actually pressing bit1 during the count phase
#endif
        pulse_counts(12);
        uint16_t pot0dat = read_reg(ADDR_POT0DAT);
        uint8_t pot0x = pot0dat & 0xFF, pot0y = (pot0dat >> 8) & 0xFF;
        checks.note("P2C-COUNT: 12 pulses, bit1 held -> POT0DAT X=" + std::to_string((int)pot0x) +
                    " Y=" + std::to_string((int)pot0y));
        checks.expect(pot0y == 12 && pot0x == 0,
            "P2-PIN9-DRIVES-Y-COUNTER (p.232, opus:8-mbtn.c11, quote 'one of the proportional pins'): holding "
            "mouse_btn bit 1 (pin 9) pressed across 12 pot_cnt_en pulses advances POT0DAT's Y byte by exactly 12 "
            "while the X byte (pin 5's counter) stays at 0 -- pin 9 drives an accumulating position counter, not "
            "just a status bit, which is what this package takes \"proportional\" to operationally mean (see "
            "SPECIFICATION.md)");
    }

    do_reset();
    {
        // Twin, broadened: interleaved press/release/press at a different
        // total magnitude, testing gating precision as well as the raw
        // count.
        set_mouse_btn(false, true, false);
        settle();
        pulse_counts(5);
#ifdef MUT_P2D
        // MUTATION: bit1 stays pressed during the segment meant to be
        // released, so it over-counts instead of pausing.
        set_mouse_btn(false, true, false);
#else
        set_mouse_btn(false, false, false);  // release: must NOT count
#endif
        settle();
        pulse_counts(3);
        set_mouse_btn(false, true, false);   // press again
        settle();
        pulse_counts(4);
        uint16_t pot0dat = read_reg(ADDR_POT0DAT);
        uint8_t pot0x = pot0dat & 0xFF, pot0y = (pot0dat >> 8) & 0xFF;
        checks.note("P2D-COUNT: 5 pressed + 3 released(should-not-count) + 4 pressed -> POT0DAT X=" +
                    std::to_string((int)pot0x) + " Y=" + std::to_string((int)pot0y));
        checks.expect(pot0y == 9 && pot0x == 0,
            "P2-PIN9-DRIVES-Y-COUNTER-TWIN (p.232, sol:8-button.c9, quote 'Controller-port pin 9 is a "
            "proportional pin', twin of opus:8-mbtn.c11, graded independently, different magnitude and an "
            "interleaved release segment that must NOT count): 5+4=9 counted pulses while bit 1 was pressed, 3 "
            "released pulses contribute nothing, X byte untouched throughout");
    }

    // ========================================================================
    // P3 -- button 3 (pin 5). Connectivity: opus:8-mbtn.c12, sol:8-button.c10.
    // Counter/"the other, distinct proportional input": opus:8-mbtn.c13,
    // sol:8-button.c11.
    // ========================================================================
    do_reset();
    {
        const bool combo_b2[4] = {true, true, false, false};
        const bool combo_b0[4] = {false, true, true, false};
        bool ok = true;
        std::string trail;
        for (int i = 0; i < 4; i++) {
#ifdef MUT_P3A
            bool b2_drive = true;  // MUTATION: bit2 stuck pressed, ignoring the intended pattern
#else
            bool b2_drive = combo_b2[i];
#endif
            set_mouse_btn(combo_b0[i], false, b2_drive);
            settle();
            uint16_t v = read_reg(ADDR_POTINP);
            bool bit8 = (v >> 8) & 1;
            bool expected_bit8 = !combo_b2[i];
            if (bit8 != expected_bit8) ok = false;
            trail += (bit8 == expected_bit8) ? "1" : "0";
        }
        checks.note("P3A-MATRIX (vary bit2,bit0 / bit1 idle): matches=" + trail);
        checks.expect(ok,
            "P3-BIT2-REACHES-PIN5-EXCLUSIVELY (p.232, opus:8-mbtn.c12, quote 'Button 3, when used, is connected "
            "to pin 5, the other proportional controller input'): POTINP bit 8 (pin 5's status) tracks mouse_btn "
            "bit 2 exactly, across every combination of bit 0 underneath it");
    }

    do_reset();
    {
        const bool combo_b2[4] = {true, true, false, false};
        const bool combo_b1[4] = {false, true, true, false};
        bool ok = true;
        std::string trail;
        for (int i = 0; i < 4; i++) {
#ifdef MUT_P3B
            bool b2_drive = false;  // MUTATION: bit2 stuck released, ignoring the intended pattern
#else
            bool b2_drive = combo_b2[i];
#endif
            set_mouse_btn(false, combo_b1[i], b2_drive);
            settle();
            uint16_t v = read_reg(ADDR_POTINP);
            bool bit8 = (v >> 8) & 1;
            bool expected_bit8 = !combo_b2[i];
            if (bit8 != expected_bit8) ok = false;
            trail += (bit8 == expected_bit8) ? "1" : "0";
        }
        checks.note("P3B-MATRIX (vary bit2,bit1 / bit0 idle): matches=" + trail);
        checks.expect(ok,
            "P3-BIT2-REACHES-PIN5-TWIN (p.232, sol:8-button.c10, quote 'A third mouse button, when present, is "
            "connected to controller-port pin 5', twin of opus:8-mbtn.c12, graded independently, different "
            "matrix than P3A -- bit 1 varied underneath instead of bit 0): POTINP bit 8 tracks mouse_btn bit 2 "
            "exactly, across every combination of bit 1 underneath it");
    }

    do_reset();
    {
        set_mouse_btn(false, false, true);   // bit2 only
        settle();
#ifdef MUT_P3C
        set_mouse_btn(false, true, false);   // MUTATION: drives bit1 (pin9) instead of bit2 (pin5) during the count phase
#endif
        pulse_counts(15);
        uint16_t pot0dat = read_reg(ADDR_POT0DAT);
        uint8_t pot0x = pot0dat & 0xFF, pot0y = (pot0dat >> 8) & 0xFF;
        checks.note("P3C-COUNT: 15 pulses, bit2 held -> POT0DAT X=" + std::to_string((int)pot0x) +
                    " Y=" + std::to_string((int)pot0y));
        checks.expect(pot0x == 15 && pot0y == 0,
            "P3-PIN5-IS-DISTINCT-SECOND-COUNTER (p.232, opus:8-mbtn.c13, quote 'Pin 5 is the other proportional "
            "controller input, the second of the two'): holding mouse_btn bit 2 (pin 5) pressed across 15 "
            "pot_cnt_en pulses advances POT0DAT's X byte by exactly 15 while the Y byte (pin 9's own counter, "
            "P2's counter) stays at 0 -- pin 5 drives its own, separate accumulating counter, not the same one "
            "pin 9 drives");
    }

    do_reset();
    {
        // Twin, strongest form: both pressed simultaneously, both counters
        // must advance together and independently -- proving the two
        // counters are genuinely separate hardware, not a shared resource
        // that can only track one button at a time.
        set_mouse_btn(false, true, true);   // bit1 AND bit2 both pressed
#ifdef MUT_P3D
        set_mouse_btn(false, true, false);  // MUTATION: bit2 silently dropped from the "both pressed" stimulus
#endif
        settle();
        pulse_counts(10);
        uint16_t pot0dat = read_reg(ADDR_POT0DAT);
        uint8_t pot0x = pot0dat & 0xFF, pot0y = (pot0dat >> 8) & 0xFF;
        checks.note("P3D-COUNT: 10 pulses, bit1+bit2 both held -> POT0DAT X=" + std::to_string((int)pot0x) +
                    " Y=" + std::to_string((int)pot0y));
        checks.expect(pot0x == 10 && pot0y == 10,
            "P3-BOTH-COUNTERS-INDEPENDENT-SIMULTANEOUS (p.232, sol:8-button.c11, quote 'Controller-port pin 5 is "
            "the other proportional-controller input', twin of opus:8-mbtn.c13, graded independently, strongest "
            "form -- both bits held at once): with mouse_btn bits 1 and 2 both pressed for 10 pot_cnt_en pulses, "
            "POT0DAT's X and Y bytes both advance to exactly 10 concurrently -- pin 5's counter and pin 9's "
            "counter operate independently, at the same time, not as a shared single resource");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
