// Test purpose: p.330 "CONNECTOR PIN USAGE FOR FIRE BUTTON INPUTS" table,
//   userio.v alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 232, 233, 330, 332, 333, 337, and more
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim outside the 12 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 12 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// appDE-fire-buttons-userio -- p.330 "CONNECTOR PIN USAGE FOR FIRE BUTTON
// INPUTS" table, userio.v alone. 12 claims, filtered from
// hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl on
// classify.package == "appDE-fire-buttons-userio".
//
// This is the connector-vector side of the fire-button material: which DB9
// pins do NOT carry the fire function, tested where the pin vectors
// (_joy1/_joy2) live. The CIA-register side (CIAAPRA bit 6/7 semantics,
// p.232-233) is `hrm/ch8-ciaapra-fire-buttons`, a `ciaa`-alone fixture; this
// package never reads CIAAPRA and never asserts anything about a bit
// position on that register.
//
// p.330's own table (read from figures/p330.png, per the build brief --
// its text layer loses the PORT 1/PORT 2 diagram and invents a phantom row,
// "1  1  1  1  I" at p330.txt:28, with no counterpart on the page; the
// PIN/MNEMONIC/DESCRIPTION table itself, rows 1-9, IS faithfully preserved
// in the text layer and matches the render exactly):
//
//     PIN MNEMONIC DESCRIPTION
//     1   -x-
//     2   -x-
//     3   -x-
//     4   -x-
//     5   -x-
//     6   FIRE*    Left mouse button/fire button
//     7   -x-
//     8   ground
//     9   -x-
//
// This package's 12 claims are the six "-x-" rows this claims file carries
// (pins 1, 2, 3, 4, 5, 9 -- NOT 7 or 8, which this claims file does not
// include), each read twice (sol + opus): an EXCLUSION proposition, "this
// pin carries no fire-button function." Pin 6 itself (the one row that DOES
// carry FIRE*) belongs to hrm/ch8-ciaapra-fire-buttons /
// hrm/ch8-controller-pin-function-map's Group D / appDE-joystick-pinout's
// NEW grade, not to this package.
//
// TWO PRINTED ANOMALIES ON THIS SAME PAGE, recorded per the build brief,
// NEITHER of which binds any of the 12 claims graded below:
//
//   (1) Ambiguous port numbering. p.330's figure labels its two DB9
//   connectors "PORT 1" / "PORT 2"; p.332's POT COUNTERS figure labels the
//   same pair of connectors "PORT 0" / "PORT 1"; p.333 labels one "PORT 0".
//   Three consecutive figures, same draughtsman's style, two numbering
//   conventions -- not resolvable on the page (work/2026-09-17_appDE_finding_
//   candidates.md #7). None of this package's 12 claims name a PORT
//   numeral: each is a bare "pin N carries no fire function" proposition
//   from the PIN/MNEMONIC/DESCRIPTION table, which does not use "PORT 1/2"
//   language at all, and every assertion below is graded against BOTH of
//   this module's connector nets (_joy1 and _joy2) precisely so no reading
//   of the ambiguity is silently assumed.
//
//   (2) Printed defect, DDRA figure. The DDRA row on the same page prints
//   bits 5-2 as value `0` labelled `OUT`. Appendix F p.341: a DDR bit of 0
//   is an INPUT. Appendix E p.337/appendix F p.351: PA5-PA2 are the four
//   disk sense lines (RDY*/TK0*/WPRO*/CHNG*), which the drive asserts and
//   the CIA reads -- so those bits must be inputs; the printed values (0)
//   are right and the printed labels (OUT) are wrong, and the page prints
//   both. Recorded per the build brief's instruction for a printed
//   self-contradiction: measured, neither side graded. DDRA governs PRA's
//   read/write direction on the CIAAPRA register, which this userio-alone
//   fixture does not even instantiate (no ciaa.v in this build's SRC) and
//   no claim in this package's 12 mentions a data-direction bit at all.
//
// RTL FACTS USED THROUGHOUT (userio.v, pinned submodule
// d6f7df957fad655bc07f6a381e5094a9edff0be0, read after this package's
// claims were filtered):
//
//   userio.v:92    wire joy_swap = 1'b0;  (tied off)
//   userio.v:211-218  synchronizer crosses the module's own port names:
//                     _sjoy1 <= joy_swap ? joy1 : joy2;   -> _sjoy1 = joy2
//                     _sjoy2 <= joy_swap ? joy2 : joy1;   -> _sjoy2 = joy1
//                     (_djoy1/_djoy2 are _sjoy1/_sjoy2 delayed one cycle)
//   userio.v:313   assign _fire0 = cd32pad && !cd32pad1_reg_load
//                                   ? fire1_d : _sjoy1[4] & _mleft;
//   userio.v:314   assign _fire1 = cd32pad && !cd32pad2_reg_load
//                                   ? fire2_d : _sjoy2[4];
//   userio.v:91    wire cd32pad = 1'b0;  (tied off, so both reduce
//                   unconditionally to _fire0 = _sjoy1[4] & _mleft = _joy2[4]
//                   & _mleft, and _fire1 = _sjoy2[4] = _joy1[4])
//   userio.v:361   assign _mleft = ~mouse_btn[0];
//   userio.v:363   assign _mthird = ~mouse_btn[2];
//   userio.v:176   potcap[0] <= _mthird & joy1_pin5;  (pin 5, port-1 copy)
//   userio.v:154   wire joy1_pin5 = ~(potreg[9] & ~potreg[8]);  (POTGO-only
//                  by default, i.e. idle=1 unless POTGO forces it -- no
//                  external DB9 pin feeds this wire at all)
//   userio.v:169   potcap[2] <= joy2_pin5;  (pin 5, port-2 copy: POTGO-only,
//                  wire joy2_pin5 = ~(potreg[13] & ~potreg[12]), userio.v:153
//                  -- confirms the build brief's "port 2's copy carries no
//                  external term at all")
//
// So: over the whole 16-bit _joy1/_joy2 vectors, bits 0-3 are the
// direction/quadrature pins (1-4), bit 4 is the fire pin (6, not graded
// here), bit 5 is the pin-9 second button/pot-Y-gate; grep over userio.v for
// every occurrence of _sjoy1[ / _sjoy2[ / _djoy1[ / _djoy2[ confirms bit 4 is
// the ONLY index either fire expression reads. Pin 5's only live channel in
// this fixture (mouse_btn[2] on port 1) is likewise absent from both fire
// expressions. Grepping userio.v for any reference to "pin 7" or "pin 8" (or
// any bit index above 5 read from a _joy* vector) returns nothing: those two
// physical pins have no net anywhere in this module, matching the page's own
// print (pin 7 "-x-", pin 8 "ground") -- neither is one of this package's 12
// claims, so that absence is noted, not graded.
//
// Each exclusion below is graded against BOTH connector nets (_joy1 and
// _joy2), one at a time, against BOTH _fire0 and _fire1 -- not just the
// "own" fire line -- so no assumption about which physical port p.330's
// "PORT 1"/"PORT 2" labels correspond to is smuggled into the fixture.
//
// MUTATION METHODOLOGY. Every exclusion claim below reduces, by the RTL
// facts above, to "no fan-in path exists from this bit/signal into _fire0 or
// _fire1 at all" -- confirmed by reading the two assign statements, which
// name bit 4 and nothing else. No stimulus value on an excluded pin, in any
// combination with any other input this fixture can drive, can move a
// boolean expression that never reads that bit -- so no STIMULUS mutation
// can turn any of these red; only an ORACLE mutation (flipping the expected
// boolean) or editing userio.v itself (out of scope: this package builds the
// unmodified pinned submodule) could. Recorded below as `none impossible`
// for every graded uid, per the build brief's explicit allowance, together
// with a positive-control measurement for each excluded signal (proving it
// moves something else real in this fixture, so the exclusion is not
// vacuously true because the pin is simply dead in the testbench).
//
// MUTATION fixture-liveness stimulus red second driven word (pin4/bit0 on _joy1) left idle instead of pressed
// MUTATION sol:E-fire.c7 none impossible: pin1 = bit3 of _joy1/_joy2; grep over userio.v shows _fire0/_fire1 (userio.v:313-314) read only bit 4 of _sjoy1/_sjoy2 -- no fan-in from bit3 exists on either connector for any stimulus to exploit. Positive control: bit3 does move JOY0DAT/JOY1DAT (PIN1-LIVENESS below), so the exclusion is not vacuous.
// MUTATION opus:E-firebutton.c10 none impossible: same as sol:E-fire.c7 -- pin1/bit3, no fan-in into _fire0/_fire1 on either connector.
// MUTATION sol:E-fire.c8 none impossible: pin2 = bit2 of _joy1/_joy2; same zero-fan-in argument as pin1, on bit2. Positive control: PIN2-LIVENESS below.
// MUTATION opus:E-firebutton.c11 none impossible: same as sol:E-fire.c8 -- pin2/bit2.
// MUTATION sol:E-fire.c9 none impossible: pin3 = bit1 of _joy1/_joy2; same zero-fan-in argument, on bit1. Positive control: PIN3-LIVENESS below.
// MUTATION opus:E-firebutton.c12 none impossible: same as sol:E-fire.c9 -- pin3/bit1.
// MUTATION sol:E-fire.c10 none impossible: pin4 = bit0 of _joy1/_joy2; same zero-fan-in argument, on bit0. Positive control: PIN4-LIVENESS below.
// MUTATION opus:E-firebutton.c13 none impossible: same as sol:E-fire.c10 -- pin4/bit0.
// MUTATION sol:E-fire.c11 none impossible: pin5's only drivable channel in this fixture is mouse_btn[2] (_mthird, port 1 copy, userio.v:176); _mleft (userio.v:361) reads mouse_btn[0] only, so mouse_btn[2] has no fan-in into _fire0/_fire1 either. Port 2's copy (userio.v:169) has no external term at all (POTGO-only) and so is not independently drivable here. Positive control: PIN5-LIVENESS below (mouse_btn[2] does move POTINP bit8).
// MUTATION opus:E-firebutton.c14 none impossible: same as sol:E-fire.c11 -- pin5/mouse_btn[2].
// MUTATION sol:E-fire.c15 none impossible: pin9 = bit5 of _joy1/_joy2; same zero-fan-in argument as pins 1-4, on bit5 (the fire expressions read bit4 only, never bit5). Positive control: PIN9-LIVENESS below (bit5 moves POTINP via potcap).
// MUTATION opus:E-firebutton.c18 none impossible: same as sol:E-fire.c15 -- pin9/bit5.

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

// ---- clocking ----
void tick() {
    top->clk = 0; top->eval(); sim_time++;
    top->clk = 1; top->eval(); sim_time++;
}
void settle(int n) { for (int i = 0; i < n; i++) tick(); }

// ---- register bus ----
static const uint16_t JOY0DAT = 0x00a;
static const uint16_t JOY1DAT = 0x00c;
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
void reset_joys() {
    joy1_val = 0xFFFF; joy2_val = 0xFFFF;
    top->_joy1 = joy1_val; top->_joy2 = joy2_val;
}

// One exclusion sub-test: drive `bit` pressed on _joy1 alone (idle _joy2),
// confirm _fire0/_fire1 unaffected; release; repeat on _joy2 alone. Returns
// true iff neither fire line ever moved on either connector.
bool exclusion_holds(int bit, bool fire0_base, bool fire1_base, std::string& detail) {
    bool ok = true;
    reset_joys(); top->mouse_btn = 0; settle(4);

    joy1_bit(bit, true); settle(4);
    bool f0a = top->_fire0, f1a = top->_fire1;
    joy1_bit(bit, false); settle(4);

    joy2_bit(bit, true); settle(4);
    bool f0b = top->_fire0, f1b = top->_fire1;
    joy2_bit(bit, false); settle(4);

    if (f0a != fire0_base || f1a != fire1_base) ok = false;
    if (f0b != fire0_base || f1b != fire1_base) ok = false;
    detail = "bit" + std::to_string(bit) + " on _joy1: fire0=" + std::to_string((int)f0a) +
             " fire1=" + std::to_string((int)f1a) + "; on _joy2: fire0=" +
             std::to_string((int)f0b) + " fire1=" + std::to_string((int)f1b) +
             " (baseline fire0=" + std::to_string((int)fire0_base) +
             " fire1=" + std::to_string((int)fire1_base) + ")";
    return ok;
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
    write_reg(POTGO, 0x0001); // release any POTGO forcing, reset pot state
    settle(4);

    note("page context: p.330's figure labels its two connectors PORT 1 / "
         "PORT 2; p.332 labels the same pair PORT 0 / PORT 1; p.333 labels "
         "one PORT 0 -- a printed ambiguity across three consecutive "
         "figures, not resolvable on the page (finding candidate #7). "
         "p.330's DDRA row also prints bits 5-2 as value 0 labelled OUT, "
         "which appendix F p.341 and p.337 show should read IN (finding "
         "candidate #4/NM-HRM shape). Neither anomaly is named by any of "
         "this package's 12 claims -- none references a PORT numeral or a "
         "DDRA/data-direction bit -- so both are recorded here for context "
         "and graded nowhere in this file. Every exclusion below is tested "
         "against BOTH connector nets (_joy1 and _joy2) and BOTH fire lines "
         "(_fire0 and _fire1) so no reading of the ambiguity is assumed.");

    // =====================================================================
    // Fixture liveness -- proven before any claim is graded.
    // =====================================================================
    reset_joys(); settle(4);
    uint16_t word_a = read_reg(JOY1DAT);
#ifndef MUT_fixtureliveness
    joy1_bit(0, true); settle(4);
#else
    settle(4); // mutation: same word again
#endif
    uint16_t word_b = read_reg(JOY1DAT);
    note("fixture-liveness JOY1DAT word A=0x" + hex16(word_a) +
         " word B=0x" + hex16(word_b));
    expect(word_a != word_b, "fixture-liveness");
    reset_joys(); settle(4);

    // Baseline fire0/fire1 with everything idle.
    top->mouse_btn = 0; settle(4);
    bool fire0_base = top->_fire0;
    bool fire1_base = top->_fire1;
    note("baseline (all pins idle): fire0=" + std::to_string((int)fire0_base) +
         " fire1=" + std::to_string((int)fire1_base) +
         " (both expected idle/high, active-low fire lines)");

    // =====================================================================
    // Positive controls -- each excluded bit/signal DOES move something
    // real in this fixture, so the exclusions below are not vacuously true
    // because the testbench never actually drives the pin.
    // =====================================================================
    {
        reset_joys(); settle(4);
        uint16_t before = read_reg(JOY1DAT);
        joy1_bit(3, true); settle(4); // pin1 = bit3
        uint16_t after = read_reg(JOY1DAT);
        note("PIN1-LIVENESS: _joy1 bit3 (pin1) pressed moves JOY1DAT from 0x" +
             hex16(before) + " to 0x" + hex16(after) + " -- pin1 is live in this fixture");
        joy1_bit(3, false); settle(4);
    }
    {
        reset_joys(); settle(4);
        uint16_t before = read_reg(JOY1DAT);
        joy1_bit(2, true); settle(4); // pin2 = bit2
        uint16_t after = read_reg(JOY1DAT);
        note("PIN2-LIVENESS: _joy1 bit2 (pin2) pressed moves JOY1DAT from 0x" +
             hex16(before) + " to 0x" + hex16(after) + " -- pin2 is live in this fixture");
        joy1_bit(2, false); settle(4);
    }
    {
        reset_joys(); settle(4);
        uint16_t before = read_reg(JOY1DAT);
        joy1_bit(1, true); settle(4); // pin3 = bit1
        uint16_t after = read_reg(JOY1DAT);
        note("PIN3-LIVENESS: _joy1 bit1 (pin3) pressed moves JOY1DAT from 0x" +
             hex16(before) + " to 0x" + hex16(after) + " -- pin3 is live in this fixture");
        joy1_bit(1, false); settle(4);
    }
    {
        reset_joys(); settle(4);
        uint16_t before = read_reg(JOY1DAT);
        joy1_bit(0, true); settle(4); // pin4 = bit0
        uint16_t after = read_reg(JOY1DAT);
        note("PIN4-LIVENESS: _joy1 bit0 (pin4) pressed moves JOY1DAT from 0x" +
             hex16(before) + " to 0x" + hex16(after) + " -- pin4 is live in this fixture");
        joy1_bit(0, false); settle(4);
    }
    {
        reset_joys(); top->mouse_btn = 0; settle(4);
        uint16_t before = read_reg(POTINP);
        top->mouse_btn = 0x04; settle(4); // pin5 port-1 copy: mouse_btn[2]
        uint16_t after = read_reg(POTINP);
        note("PIN5-LIVENESS: mouse_btn[2] (pin5, port-1 copy) pressed moves "
             "POTINP from 0x" + hex16(before) + " to 0x" + hex16(after) +
             " (bit8) -- pin5's port-1 copy is live in this fixture; the "
             "port-2 copy has no external term at all (userio.v:169) and is "
             "not independently drivable here");
        top->mouse_btn = 0; settle(4);
    }
    {
        reset_joys(); settle(4);
        uint16_t before = read_reg(POTINP);
        joy1_bit(5, true); settle(4); // pin9 = bit5
        uint16_t after = read_reg(POTINP);
        note("PIN9-LIVENESS: _joy1 bit5 (pin9) pressed moves POTINP from 0x" +
             hex16(before) + " to 0x" + hex16(after) + " -- pin9 is live in this fixture");
        joy1_bit(5, false); settle(4);
    }

    // =====================================================================
    // GRADED -- the six exclusion propositions, pins 1, 2, 3, 4, 5, 9.
    // =====================================================================
    std::string d;

    bool p1 = exclusion_holds(3, fire0_base, fire1_base, d); // pin1 = bit3
    note("pin1 exclusion: " + d);
    expect(p1, "sol:E-fire.c7");
    expect(p1, "opus:E-firebutton.c10");

    bool p2 = exclusion_holds(2, fire0_base, fire1_base, d); // pin2 = bit2
    note("pin2 exclusion: " + d);
    expect(p2, "sol:E-fire.c8");
    expect(p2, "opus:E-firebutton.c11");

    bool p3 = exclusion_holds(1, fire0_base, fire1_base, d); // pin3 = bit1
    note("pin3 exclusion: " + d);
    expect(p3, "sol:E-fire.c9");
    expect(p3, "opus:E-firebutton.c12");

    bool p4 = exclusion_holds(0, fire0_base, fire1_base, d); // pin4 = bit0
    note("pin4 exclusion: " + d);
    expect(p4, "sol:E-fire.c10");
    expect(p4, "opus:E-firebutton.c13");

    // pin5: only the port-1 copy (mouse_btn[2]) is drivable here; the port-2
    // copy has no external term at all (userio.v:169, POTGO-only), so its
    // exclusion is falsifiable on port 1 only, exactly as the build brief's
    // own evidence for this claims file states.
    {
        reset_joys(); top->mouse_btn = 0; settle(4);
        bool f0_before = top->_fire0, f1_before = top->_fire1;
        top->mouse_btn = 0x04; settle(4); // pin5 port-1 copy pressed
        bool f0_pressed = top->_fire0, f1_pressed = top->_fire1;
        top->mouse_btn = 0; settle(4);
        bool p5 = (f0_pressed == f0_before) && (f1_pressed == f1_before) &&
                  (f0_before == fire0_base) && (f1_before == fire1_base);
        note("pin5 exclusion (port-1 copy, mouse_btn[2]): before fire0=" +
             std::to_string((int)f0_before) + " fire1=" + std::to_string((int)f1_before) +
             "; pressed fire0=" + std::to_string((int)f0_pressed) + " fire1=" +
             std::to_string((int)f1_pressed) + "; port-2 copy not independently "
             "drivable in this fixture (no external term, userio.v:169) so it "
             "is not separately exercised here");
        expect(p5, "sol:E-fire.c11");
        expect(p5, "opus:E-firebutton.c14");
    }

    bool p9 = exclusion_holds(5, fire0_base, fire1_base, d); // pin9 = bit5
    note("pin9 exclusion: " + d);
    expect(p9, "sol:E-fire.c15");
    expect(p9, "opus:E-firebutton.c18");

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
