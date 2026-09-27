// Test purpose: Table 8-1 (p.228), userio.v alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 228, 233, 240, 282
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
// ch8-controller-pin-function-map -- Table 8-1 (p.228), userio.v alone.
//
// Pin map used throughout (derived from userio.v's own port list and
// parameters -- see the RTL boundary finding section of SPECIFICATION.md):
//   bit0=pin4(HQ/right)  bit1=pin3(VQ/left)  bit2=pin2(H/back)
//   bit3=pin1(V/forward) bit4=pin6(fire)     bit5=pin9(button2/fire2)
// on both _joy1 and _joy2. joy_swap is tied 0, so _sjoy2/_djoy2 (the "Port
// 2" / JOY1DAT decoder) track _joy1, and _sjoy1/_djoy1 (the "Port 1" /
// JOY0DAT decoder) track _joy2. This file drives _joy1 and reads JOY1DAT
// throughout, since JOY1DAT's read mux (data_out = dmouse1dat, unconditional)
// is simpler than JOY0DAT's (which multiplexes on the internal joy1enable
// latch) and both read out the identical decoder logic.
//
// MUTATION opus:8-tbl8-1.c1 stimulus red pin1 (bit3) never pressed
// MUTATION sol:8-pin.c1 stimulus red pin1 (bit3) never pressed
// MUTATION opus:8-tbl8-1.c2 stimulus red pin1 (bit3) never toggled
// MUTATION sol:8-pin.c2 stimulus red pin1 (bit3) never toggled
// MUTATION opus:8-tbl8-1.c4 stimulus red pin2 (bit2) never pressed
// MUTATION sol:8-pin.c5 stimulus red pin2 (bit2) never pressed
// MUTATION opus:8-tbl8-1.c5 stimulus red pin2 (bit2) never toggled
// MUTATION sol:8-pin.c6 stimulus red pin2 (bit2) never toggled
// MUTATION opus:8-tbl8-1.c6 stimulus red pin3 (bit1) never pressed
// MUTATION sol:8-pin.c7 stimulus red pin3 (bit1) never pressed
// MUTATION opus:8-tbl8-1.c7 stimulus red pin3 (bit1) never toggled
// MUTATION sol:8-pin.c8 stimulus red pin3 (bit1) never toggled
// MUTATION opus:8-tbl8-1.c10 stimulus red pin4 (bit0) never pressed
// MUTATION sol:8-pin.c11 stimulus red pin4 (bit0) never pressed
// MUTATION opus:8-tbl8-1.c11 stimulus red pin4 (bit0) never toggled
// MUTATION sol:8-pin.c12 stimulus red pin4 (bit0) never toggled
// MUTATION opus:8-tbl8-1.c32 stimulus red pin9 (_joy1[5]) never pressed before the pot-gate count window
// MUTATION sol:8-pin.c35 stimulus red pin9 (_joy1[5]) never pressed before the pot-gate count window
// MUTATION opus:8-tbl8-1.c33 stimulus red pin9 (_joy1[5]) never pressed before the pot-gate count window
// MUTATION sol:8-pin.c36 stimulus red pin9 (_joy1[5]) never pressed before the pot-gate count window
// MUTATION opus:8-tbl8-1.c15 stimulus green already red at baseline (port 2's pin5 copy has no external term, see Group B2 below): port 1's live mouse_btn[2] pin5 channel substituted for port 2's dead joy2_pin5 channel in the identical two-port comparator, shown green
// MUTATION sol:8-pin.c17 stimulus green same substitution, same comparator, independently guarded
// MUTATION opus:8-tbl8-1.c16 stimulus green same substitution, same comparator
// MUTATION sol:8-pin.c18 stimulus green same substitution, same comparator, independently guarded
// MUTATION opus:8-tbl8-1.c35 stimulus red POTGO OUTLX/DATLX (potreg[9:8]) never written to force pin 5
// MUTATION sol:8-pin.c39 stimulus red POTGO OUTLX/DATLX (potreg[9:8]) never written to force pin 5
// MUTATION opus:8-tbl8-1.c37 stimulus red POTGO OUTLY/DATLY (potreg[15:14]) never written to force pin 9
// MUTATION sol:8-pin.c41 stimulus red POTGO OUTLY/DATLY (potreg[15:14]) never written to force pin 9
// MUTATION fixture-liveness stimulus red second injected word (pin4/bit0) left idle instead of pressed

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

// hex formatting for register words in MEASURED lines (std::to_string is
// decimal; register values read far more clearly in hex).
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

// ---- register bus (matches userio.v's own JOY0DAT/JOY1DAT/... parameters) ----
static const uint16_t JOY0DAT = 0x00a;
static const uint16_t JOY1DAT = 0x00c;
static const uint16_t POT0DAT = 0x012;
static const uint16_t POT1DAT = 0x014;
static const uint16_t POTINP  = 0x016;
static const uint16_t POTGO   = 0x034;
static const uint8_t  ADDR_NONE = 0xFF; // no register in this file's scope decodes to this

static uint8_t addr_of(uint16_t regaddr) { return (uint8_t)((regaddr >> 1) & 0xFF); }

void write_reg(uint16_t regaddr, uint16_t data) {
    top->reg_address_in = addr_of(regaddr);
    top->data_in = data;
    tick(); // one clk7_en cycle with the address held -> latches
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
    // Fixture liveness -- proven before any Table 8-1 claim is graded.
    // Two distinct words driven into _joy1 must arrive as two distinct
    // JOY1DAT readings, through the whole synchronise -> decode -> mux path.
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
    // Group A -- pins 1-4, joystick direct-read (page 228, Table 8-1)
    // Control case: pin4/right (opus:8-tbl8-1.c10, sol:8-pin.c11) -- the
    // cleanest single-bit read in this file, same measurement basis as
    // everything else below.
    // =====================================================================

    // --- pin 1 = Joystick "Forward" (bit3), read via JOY1DAT bit8 ---
    // bit8 = (pin3 level) XOR (pin1 level); hold pin3 idle so bit8 reads
    // pin1 alone.
    joy1_bit(1, false); // pin3 idle
#ifndef MUT_pin1fwd
    joy1_bit(3, true);  // press pin1 (forward)
#endif
    settle(4);
    { uint16_t v = read_reg(JOY1DAT);
      bool bit8 = (v >> 8) & 1;
      note("pin1 forward: JOY1DAT=0x" + hex16(v) + " bit8=" + std::to_string(bit8));
      expect(bit8, "opus:8-tbl8-1.c1");
      expect(bit8, "sol:8-pin.c1"); }
    joy1_bit(3, false); settle(4);

    // --- pin 1 = Mouse/TB/DC "V-pulse": toggling pin1 alone (bit3) moves
    // the vertical byte (JOY1DAT[15:8]) and leaves the horizontal byte
    // (JOY1DAT[7:0]) unchanged. Pin 3 held idle throughout. ---
    { uint16_t before = read_reg(JOY1DAT);
      uint8_t horiz_before = before & 0xFF;
#ifndef MUT_pin1v
      joy1_bit(3, true); settle(6); // one clean idle->pressed edge: one pulse
#else
      settle(6); // mutation: pin1 (bit3) never toggled
#endif
      uint16_t after = read_reg(JOY1DAT);
      uint8_t horiz_after = after & 0xFF;
      bool moved = (before & 0xFF00) != (after & 0xFF00);
      bool horiz_steady = horiz_before == horiz_after;
      note("pin1 V-pulse: JOY1DAT before=0x" + hex16(before) +
           " after=0x" + hex16(after));
      expect(moved && horiz_steady, "opus:8-tbl8-1.c2");
      expect(moved && horiz_steady, "sol:8-pin.c2"); }
    joy1_bit(3, false); settle(4);

    // --- pin 2 = Joystick "Back" (bit2), read via JOY1DAT bit0 ---
    // bit0 = (pin4 level) XOR (pin2 level); hold pin4 idle.
    joy1_bit(0, false); // pin4 idle
#ifndef MUT_pin2back
    joy1_bit(2, true); // press pin2 (back)
#endif
    settle(4);
    { uint16_t v = read_reg(JOY1DAT);
      bool bit0 = v & 1;
      note("pin2 back: JOY1DAT=0x" + hex16(v) + " bit0=" + std::to_string(bit0));
      expect(bit0, "opus:8-tbl8-1.c4");
      expect(bit0, "sol:8-pin.c5"); }
    joy1_bit(2, false); settle(4);

    // --- pin 2 = Mouse/TB/DC "H-pulse": toggling pin2 alone (bit2) moves
    // the horizontal byte and leaves the vertical byte unchanged. ---
    { uint16_t before = read_reg(JOY1DAT);
      uint8_t vert_before = (before >> 8) & 0xFF;
#ifndef MUT_pin2h
      joy1_bit(2, true); settle(6); // one clean idle->pressed edge: one pulse
#else
      settle(6);
#endif
      uint16_t after = read_reg(JOY1DAT);
      uint8_t vert_after = (after >> 8) & 0xFF;
      bool moved = (before & 0x00FF) != (after & 0x00FF);
      bool vert_steady = vert_before == vert_after;
      note("pin2 H-pulse: JOY1DAT before=0x" + hex16(before) +
           " after=0x" + hex16(after));
      expect(moved && vert_steady, "opus:8-tbl8-1.c5");
      expect(moved && vert_steady, "sol:8-pin.c6"); }
    joy1_bit(2, false); settle(4);

    // --- pin 3 = Joystick "Left" (bit1), read via JOY1DAT bit9 (clean:
    // depends only on pin3) ---
#ifndef MUT_pin3left
    joy1_bit(1, true); // press pin3 (left)
#endif
    settle(4);
    { uint16_t v = read_reg(JOY1DAT);
      bool bit9 = (v >> 9) & 1;
      note("pin3 left: JOY1DAT=0x" + hex16(v) + " bit9=" + std::to_string(bit9));
      expect(bit9, "opus:8-tbl8-1.c6");
      expect(bit9, "sol:8-pin.c7"); }
    joy1_bit(1, false); settle(4);

    // --- pin 3 = Mouse/TB/DC "VQ-pulse": toggling pin3 alone (bit1) moves
    // the vertical byte, horizontal byte unchanged. ---
    { uint16_t before = read_reg(JOY1DAT);
      uint8_t horiz_before = before & 0xFF;
#ifndef MUT_pin3vq
      joy1_bit(1, true); settle(6); // one clean idle->pressed edge: one pulse
#else
      settle(6);
#endif
      uint16_t after = read_reg(JOY1DAT);
      uint8_t horiz_after = after & 0xFF;
      bool moved = (before & 0xFF00) != (after & 0xFF00);
      bool horiz_steady = horiz_before == horiz_after;
      note("pin3 VQ-pulse: JOY1DAT before=0x" + hex16(before) +
           " after=0x" + hex16(after));
      expect(moved && horiz_steady, "opus:8-tbl8-1.c7");
      expect(moved && horiz_steady, "sol:8-pin.c8"); }
    joy1_bit(1, false); settle(4);

    // --- pin 4 = Joystick "Right" (bit0), read via JOY1DAT bit1 (clean:
    // depends only on pin4) -- CONTROL CASE ---
#ifndef MUT_pin4right
    joy1_bit(0, true); // press pin4 (right)
#endif
    settle(4);
    { uint16_t v = read_reg(JOY1DAT);
      bool bit1 = (v >> 1) & 1;
      note("pin4 right (control case): JOY1DAT=0x" + hex16(v) + " bit1=" + std::to_string(bit1));
      expect(bit1, "opus:8-tbl8-1.c10");
      expect(bit1, "sol:8-pin.c11"); }
    joy1_bit(0, false); settle(4);

    // --- pin 4 = Mouse/TB/DC "HQ-pulse": toggling pin4 alone (bit0) moves
    // the horizontal byte, vertical byte unchanged. ---
    { uint16_t before = read_reg(JOY1DAT);
      uint8_t vert_before = (before >> 8) & 0xFF;
#ifndef MUT_pin4hq
      joy1_bit(0, true); settle(6); // one clean idle->pressed edge: one pulse
#else
      settle(6);
#endif
      uint16_t after = read_reg(JOY1DAT);
      uint8_t vert_after = (after >> 8) & 0xFF;
      bool moved = (before & 0x00FF) != (after & 0x00FF);
      bool vert_steady = vert_before == vert_after;
      note("pin4 HQ-pulse: JOY1DAT before=0x" + hex16(before) +
           " after=0x" + hex16(after));
      expect(moved && vert_steady, "opus:8-tbl8-1.c11");
      expect(moved && vert_steady, "sol:8-pin.c12"); }
    joy1_bit(0, false); settle(4);

    // =====================================================================
    // Group B -- pin 9, proportional-controller pot gating (POT1DAT, port 2:
    // clean, no ANDed mouse-button term -- potcap[3] <= _djoy2[5] & ...,
    // and _djoy2[5] tracks _joy1[5] = pin 9 directly).
    // =====================================================================
    write_reg(POTGO, 0x0001); // START: pot1x=pot1y=0, and not forcing pin9 as output
    settle(4);
#ifndef MUT_pin9y
    joy1_bit(5, true); // press pin9 (hold low): X/Y gates free-run while held
#endif
    settle(4);         // let the 2-stage synchroniser settle before counting
    uint16_t pot1_before = read_reg(POT1DAT);
    static const int PULSES = 20;
    for (int i = 0; i < PULSES; i++) { top->pot_cnt_en = 1; tick(); top->pot_cnt_en = 0; tick(); }
    uint16_t pot1_held = read_reg(POT1DAT);
    joy1_bit(5, false); // release pin9: the Y byte should now freeze
    settle(4);
    for (int i = 0; i < PULSES; i++) { top->pot_cnt_en = 1; tick(); top->pot_cnt_en = 0; tick(); }
    uint16_t pot1_released = read_reg(POT1DAT);
    {
        uint8_t y_before = (pot1_before >> 8) & 0xFF, y_held = (pot1_held >> 8) & 0xFF, y_released = (pot1_released >> 8) & 0xFF;
        uint8_t x_before = pot1_before & 0xFF, x_held = pot1_held & 0xFF, x_released = pot1_released & 0xFF;
        note("pin9 pot-gate: POT1DAT before=0x" + hex16(pot1_before) +
             " held=0x" + hex16(pot1_held) + " released=0x" + hex16(pot1_released));
        bool y_advanced_while_held = y_held != y_before;
        bool y_frozen_after_release = y_released == y_held;
        bool x_essentially_static = (x_before == x_held) && (x_held == x_released);
        bool gate_ok = y_advanced_while_held && y_frozen_after_release && x_essentially_static;
        expect(gate_ok, "opus:8-tbl8-1.c32");
        expect(gate_ok, "sol:8-pin.c35");
        expect(gate_ok, "opus:8-tbl8-1.c33");
        expect(gate_ok, "sol:8-pin.c36");
    }

    // =====================================================================
    // Group E -- pins 5 & 9, output-configurability footnote ("* These pins
    // may also be configured as outputs"). Tested through POTGO (write) and
    // POTINP (read) alone, since the observable effect is a forced,
    // input-independent bit in POTINP once the corresponding OUTxx/DATxx
    // pair is written -- no DB9-facing port is required for this claim.
    // =====================================================================
    write_reg(POTGO, 0x0001); // clear any forcing bits, reset pot state
    settle(4);

    // --- pin 5 (port 1's copy: potcap[0], POTINP bit 8) ---
    top->mouse_btn = 0; // mouse middle button not pressed throughout
    settle(4);
    uint16_t potinp_baseline5 = read_reg(POTINP);
#ifndef MUT_pin5out
    write_reg(POTGO, 0x0200); // OUTLX=1 (bit9), DATLX=0 (bit8): force pin5 low
#endif
    settle(4);
    uint16_t potinp_forced5 = read_reg(POTINP);
    {
        bool bit8_baseline = (potinp_baseline5 >> 8) & 1;
        bool bit8_forced = (potinp_forced5 >> 8) & 1;
        note("pin5 output-capable: POTINP baseline=0x" + hex16(potinp_baseline5) +
             " forced=0x" + hex16(potinp_forced5));
        // same external state (mouse_btn unchanged) throughout; the forced
        // reading must differ from the baseline for the claim to hold.
        bool ok = bit8_baseline != bit8_forced;
        expect(ok, "opus:8-tbl8-1.c35");
        expect(ok, "sol:8-pin.c39");
    }
    write_reg(POTGO, 0x0001); // release the force, reset again
    settle(4);

    // --- pin 9 (port 2's copy: potcap[3], POTINP bit 14) ---
    joy1_bit(5, false); // pin9 idle throughout this sub-test
    settle(4);
    uint16_t potinp_baseline9 = read_reg(POTINP);
#ifndef MUT_pin9out
    write_reg(POTGO, 0x8000); // OUTLY=1 (bit15), DATLY=0 (bit14): force pin9 low
#endif
    settle(4);
    uint16_t potinp_forced9 = read_reg(POTINP);
    {
        bool bit14_baseline = (potinp_baseline9 >> 14) & 1;
        bool bit14_forced = (potinp_forced9 >> 14) & 1;
        note("pin9 output-capable: POTINP baseline=0x" + hex16(potinp_baseline9) +
             " forced=0x" + hex16(potinp_forced9));
        bool ok = bit14_baseline != bit14_forced;
        expect(ok, "opus:8-tbl8-1.c37");
        expect(ok, "sol:8-pin.c41");
    }

    // =====================================================================
    // Group B2 -- pin 5, proportional-controller pot gating. A port that
    // carries no external signal into the X/Right-POT gate is the
    // deviation itself, not a reason the claim stops being testable --
    // classify.status is "testable" for all four uids with no narrowing
    // "split" field, unlike Group C below, where classify's own "split"
    // text concedes the device-identity gap up front. p.282's register
    // table lists pin 5 on BOTH connectors feeding a pot counter ("RIGHT X
    // RX 5 35 (POT1X)", "LEFT X LX 5 32 (POT0X)"), so a Table-8-1 claim
    // that is worded the same for both physical ports (see KNOWN CONFLICT 3
    // handling above) needs both copies of the gate to sense it.
    //
    // Measured directly against userio.v: port 1's copy (potcap[0],
    // POT0DAT's X byte) is `_mthird & joy1_pin5` -- a real external signal
    // (mouse_btn[2]) reaches it and it senses correctly. Port 2's copy
    // (potcap[2], POT1DAT's X byte) is `joy2_pin5 = ~(potreg[13] &
    // ~potreg[12])`, a function of the POTGO write register ALONE; no
    // combination of this fixture's inputs (mouse buttons, _joy1, _joy2)
    // ever moves it. The identical RX/potcap[2] gap is independently
    // reported RED by the sibling hrm/ch8-pot-pins-as-digital-io (Group 5,
    // ENTIRE-REGISTER-SENSES) against the same RTL fact.
    // =====================================================================
    {
        write_reg(POTGO, 0x0001); // START: reset counters, release forcing
        settle(4);

        // --- Port 1 (POT0DAT): press/release mouse_btn[2], the live pin5
        // substitute (userio.v: potcap[0] <= _mthird & joy1_pin5). ---
        top->mouse_btn = 0x00; // pin5 idle
        settle(4);
        uint16_t port1_before = read_reg(POT0DAT);
        top->mouse_btn = 0x04; // press pin5 (mouse middle-button term, bit2)
        settle(4);
        for (int i = 0; i < PULSES; i++) { top->pot_cnt_en = 1; tick(); top->pot_cnt_en = 0; tick(); }
        uint16_t port1_held = read_reg(POT0DAT);
        top->mouse_btn = 0x00; // release pin5
        settle(4);
        for (int i = 0; i < PULSES; i++) { top->pot_cnt_en = 1; tick(); top->pot_cnt_en = 0; tick(); }
        uint16_t port1_released = read_reg(POT0DAT);

        // --- Port 2 (POT1DAT): every input this fixture has, driven to its
        // most active combination at once -- all mouse buttons pressed,
        // every _joy1/_joy2 line pressed. ---
        uint16_t port2_before = read_reg(POT1DAT);
        top->mouse_btn = 0x07;
        joy1_val = 0x0000; top->_joy1 = joy1_val;
        joy2_val = 0x0000; top->_joy2 = joy2_val;
        settle(4);
        for (int i = 0; i < PULSES; i++) { top->pot_cnt_en = 1; tick(); top->pot_cnt_en = 0; tick(); }
        uint16_t port2_after = read_reg(POT1DAT);
        // restore idle defaults for what follows
        top->mouse_btn = 0x00;
        joy1_val = 0xFFFF; top->_joy1 = joy1_val;
        joy2_val = 0xFFFF; top->_joy2 = joy2_val;
        settle(4);

        uint8_t port1_x_before = port1_before & 0xFF, port1_x_held = port1_held & 0xFF, port1_x_released = port1_released & 0xFF;
        uint8_t port1_y_before = (port1_before >> 8) & 0xFF, port1_y_held = (port1_held >> 8) & 0xFF, port1_y_released = (port1_released >> 8) & 0xFF;
        uint8_t port2_x_before = port2_before & 0xFF, port2_x_after = port2_after & 0xFF;

        // port2's Y-byte (potcap[3], pin9) is incidentally unblocked by this
        // section's "every input pressed" stimulus -- irrelevant to the
        // pin5/X claim under test, so the note prints X-bytes only (masked)
        // to keep the comparison unambiguous; the full registers are still
        // in port1_before/held/released and port2_before/after above.
        note("pin5 X-gate (X-byte only): port1 POT0DAT.X before=0x" + hex16(port1_x_before) +
             " held=0x" + hex16(port1_x_held) + " released=0x" + hex16(port1_x_released) +
             "; port2 POT1DAT.X before=0x" + hex16(port2_x_before) +
             " after-max-stimulus=0x" + hex16(port2_x_after));

        bool port1_advanced_while_pressed = port1_x_held != port1_x_before;
        bool port1_frozen_after_release = port1_x_released == port1_x_held;
        bool port1_y_static = (port1_y_before == port1_y_held) && (port1_y_held == port1_y_released);
        bool port1_senses_pin5 = port1_advanced_while_pressed && port1_frozen_after_release && port1_y_static;

        bool port2_senses_pin5 = port2_x_after != port2_x_before;

        // Table 8-1's pin map is port-agnostic (Groups A, B and E all rest
        // on this too); the claim needs both physical copies to sense
        // pin 5, so the graded boolean is their conjunction -- port 2's
        // absence makes it false.
        bool gate_ok = port1_senses_pin5 && port2_senses_pin5;
        expect(gate_ok, "opus:8-tbl8-1.c15");
        expect(gate_ok, "sol:8-pin.c17");
        expect(gate_ok, "opus:8-tbl8-1.c16");
        expect(gate_ok, "sol:8-pin.c18");

        // Substitution proof (an already-red assertion is not
        // stimulus-mutated further). A
        // mechanism proven live above -- port 1's mouse_btn[2] channel --
        // put through the identical two-port comparator alone, shown green.
        bool substitution_green = port1_senses_pin5;
        note(std::string("Substitution check for opus:8-tbl8-1.c15/c16, ") +
             "sol:8-pin.c17/c18: port 1's live pin5 channel, run through the " +
             "same comparator port 2 fails, reads " +
             (substitution_green ? "green" : "red") + " -- the comparator is not vacuous.");
    }

    // =====================================================================
    // Declined claims -- named individually, one record() line per uid.
    // =====================================================================

    // Group C -- pin 5/9 device-identity claims: the pin is a readable
    // digital level; which named external device produced that level is
    // wiring outside userio.v's boundary, not something the module decides.
    record("opus:8-tbl8-1.c14 (pin 5 = Mouse/TB/DC middle button) declined: "
           "userio.v senses a level on this path (via mouse_btn[2] -> "
           "potcap[0] -> POTINP bit8), but has no notion of which physical "
           "device produced it; 'it was specifically a mouse's middle "
           "button' is not observable here.");
    record("sol:8-pin.c15 (pin 5 = Mouse/TB/DC middle button) declined: same "
           "reason as opus:8-tbl8-1.c14.");
    record("opus:8-tbl8-1.c17 (pin 5 = Light Pen pen-pressed-to-screen) "
           "declined: same sensed level as opus:8-tbl8-1.c14 (POTINP bit8), "
           "same device-identity gap.");
    record("sol:8-pin.c19 (pin 5 = Light Pen pen-pressed-to-screen) declined: "
           "same reason as opus:8-tbl8-1.c17.");
    record("opus:8-tbl8-1.c30 (pin 9 = Joystick Button 2) declined: userio.v "
           "senses a level on this path (potcap[1]/potcap[3] -> POTINP "
           "bit10/bit14), but cannot tell a joystick's button 2 from a "
           "mouse's right button or a light pen's button 2 producing the "
           "same level.");
    record("sol:8-pin.c32 (pin 9 = Joystick Button 2) declined: same reason "
           "as opus:8-tbl8-1.c30.");
    record("opus:8-tbl8-1.c31 (pin 9 = Mouse/TB/DC right button) declined: "
           "same sensed level as opus:8-tbl8-1.c30, same device-identity "
           "gap.");
    record("sol:8-pin.c34 (pin 9 = Mouse/TB/DC right button) declined: same "
           "reason as opus:8-tbl8-1.c31.");
    record("opus:8-tbl8-1.c34 (pin 9 = Light Pen button 2) declined: same "
           "sensed level as opus:8-tbl8-1.c30, same device-identity gap.");
    record("sol:8-pin.c37 (pin 9 = Light Pen button 2) declined: same reason "
           "as opus:8-tbl8-1.c34.");

    // Group D -- pin 6's fire-line claims: not built in this package. The
    // binding IS observable at this fixture: userio.v:313 drives the output
    // port _fire0 from _sjoy1[4] (pin 6) and _mleft (mouse button 1, :361),
    // so no readable register bit is needed. hrm/appDE-joystick-pinout
    // grades the proposition at this boundary; the uids are declined in
    // this package.
    record("opus:8-tbl8-1.c18 (pin 6 = Joystick Button 1) declined: not "
           "built in this package. The binding is observable at the userio "
           "boundary -- userio.v:313 drives the output port _fire0 from "
           "_sjoy1[4], no readable fire bit needed -- and "
           "hrm/appDE-joystick-pinout grades it there.");
    record("sol:8-pin.c20 (pin 6 = Joystick Button 1) declined: same reason "
           "as opus:8-tbl8-1.c18.");
    record("opus:8-tbl8-1.c19 (pin 6 = Mouse/TB/DC left button) declined: "
           "same reason as opus:8-tbl8-1.c18 -- _fire0 carries _mleft "
           "(userio.v:313, :361).");
    record("sol:8-pin.c21 (pin 6 = Mouse/TB/DC left button) declined: same "
           "reason as opus:8-tbl8-1.c19.");

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
