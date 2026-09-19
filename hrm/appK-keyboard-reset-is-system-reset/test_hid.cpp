// Test purpose: p.411 "asserted on a powerup reset or a keyboard reset",
//   `hid` alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 411
// Non-goals: anything outside the `hid` fixture this package builds, and any
//   claim outside the 2 it cites. A green here means those assertions held
//   on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 2 ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-keyboard-reset-is-system-reset.jsonl` in the
//   sibling repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// appK-keyboard-reset-is-system-reset -- p.411 "asserted on a powerup
// reset or a keyboard reset", `hid` alone. 2 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-keyboard-reset-is-system-reset.jsonl.
//
// STANDING QUALIFIER: `hid` lives in nanomig/src/misc, NanoMig's own
// peripheral-substitute layer, not the shared Minimig chipset lineage --
// upstream Minimig has no counterpart to this file at all. A result here
// is a fact about this substitute peripheral, on this board; it does not
// transfer to "the chipset" the way a `paula` or `agnus` result would.
// See SPECIFICATION.md for the full statement. This qualifier applies
// regardless of how the two assertions below read.
//
// sol:K-0435 and opus:K-0441 are the same compound proposition: a
// keyboard reset mechanism exists, and it produces a full-system reset,
// the same kind a power-up produces. `hid` alone can only speak to the
// first half (the combo exists and fires `kbd_reset`, hid.v:30,159-171);
// whether that pulse becomes the SAME signal as power-up reset is
// minimig_syscontrol.v's own wiring, outside this one-top-module fixture
// -- measured as not-exercised below (note(), not a decline: the uid's
// own restatement is about the mechanism firing, which IS graded).
//
// PROTOCOL. hid's MCU-facing byte-serial interface (data_in_strobe/
// data_in_start/data_in[7:0]) delivers one CMD-1 "keyboard data" event
// per key_event() call: a start byte selecting command 1 (resets `state`
// to 0, hid.v:84-86), then the event byte itself (accepted at hid.v:100-
//104 when `state` is still 0), then one idle tick with the strobe
// deasserted. That trailing idle tick is what lets a caller chain three
// key_event() calls (the reset-combo sequence) and read `kbd_reset`
// correctly immediately after the third call returns, with no extra
// manual tick -- confirmed against a working baseline run in this same
// suite's ch8-keyboard-events package, which uses the identical protocol
// against this identical module and documents the same timing fact.
//
// HID codes (amiga_keymap.v's `code` input) used below, read directly
// from that file: 0x68 -> amiga 0x63 (Left Ctrl, hid.v's own lctrl_down
// check), 0x6b -> amiga 0x66 (Left Amiga / "left meta", lamiga_down),
// 0x6f -> amiga 0x67 (Right Amiga / "right meta", ramiga_down).

#include "Vhid.h"
#include "verilated.h"
#include <iostream>
#include <string>
#include <cstdint>

static Vhid* top;
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

static const uint8_t HID_LEFT_CTRL   = 0x68; // -> amiga 0x63
static const uint8_t HID_LEFT_AMIGA  = 0x6b; // -> amiga 0x66 ("left meta")
static const uint8_t HID_RIGHT_AMIGA = 0x6f; // -> amiga 0x67 ("right meta")
static const uint8_t HID_ONE         = 0x1e; // -> amiga 0x01 ("1"), liveness only

// Drives one CMD-1 keyboard byte through hid's MCU-facing protocol: a
// start byte (selects command 1, resets `state` to 0) then the event byte
// itself (accepted while `state` is still 0), then one idle tick with the
// strobe deasserted.
void key_event(uint8_t code7bit, bool released) {
    uint8_t byte = (released ? 0x80 : 0x00) | (code7bit & 0x7F);
    top->data_in = 0x01; // CMD 1: keyboard data
    top->data_in_start = 1;
    top->data_in_strobe = 1;
    tick();
    top->data_in = byte;
    top->data_in_start = 0;
    tick();
    top->data_in_strobe = 0;
    tick();
}

void do_reset() {
    top->reset = 1;
    settle(2);
    top->reset = 0;
    settle(2);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vhid;

    top->clk = 0;
    top->reset = 0;
    top->data_in_strobe = 0;
    top->data_in_start = 0;
    top->data_in = 0;
    top->db9_port = 0;
    top->iack = 0;
    top->eval();
    do_reset();

    // =====================================================================
    // Fixture liveness -- proven before any claim is graded.
    // =====================================================================
    bool level_before = top->kbd_mouse_level;
    key_event(HID_ONE, false);
    bool level_after = top->kbd_mouse_level;
    note("fixture-liveness: kbd_mouse_level before=" + std::to_string((int)level_before) +
         " after pressing HID 0x1e ('1')=" + std::to_string((int)level_after) +
         " kbd_mouse_data=0x" + std::to_string((int)top->kbd_mouse_data));
    expect(level_before != level_after,
        "[not graded -- apparatus check, no claim uid] fixture-liveness: "
        "the byte-serial protocol delivers a distinguishable, known event");

    // =====================================================================
    // sol:K-0435 / opus:K-0441 -- LCTRL+LAMIGA+RAMIGA pulses kbd_reset.
    // hid.v:30 (kbd_reset output), :159-171 (combo detection, one-cycle
    // pulse: kbd_reset <= kbd_combo_now & ~kbd_combo_prev).
    // =====================================================================
    do_reset();
    key_event(HID_LEFT_CTRL, false);
    key_event(HID_LEFT_AMIGA, false);
    bool reset_before_third = top->kbd_reset;
    note("reset combo, two of three held (Left Ctrl + Left Amiga): kbd_reset=" +
         std::to_string((int)reset_before_third) + " (expected 0)");

#ifdef MUT_nothirdkey
    // MUTATION: omit the third key_event() call (Right Amiga).
#else
    key_event(HID_RIGHT_AMIGA, false);
#endif
    bool reset_pulse = top->kbd_reset;
    note(std::string("reset combo, all three held (Left Ctrl + Left Amiga + Right Amiga)") +
#ifdef MUT_nothirdkey
         " [MUTATION: third key withheld]" +
#endif
         ": kbd_reset=" + std::to_string((int)reset_pulse) + " (expected 1)");

    // One more tick: the pulse is one cycle wide (kbd_combo_prev catches
    // up), so it must be gone again even though the combo is still held.
    tick();
    bool reset_after_pulse = top->kbd_reset;
    note("one cycle later, combo still held: kbd_reset=" +
         std::to_string((int)reset_after_pulse) +
         " (expected 0 -- the pulse is one cycle wide)");

    bool combo_fires = !reset_before_third && reset_pulse && !reset_after_pulse;

    note("hid alone cannot show that this kbd_reset pulse becomes the SAME "
         "signal as a power-up reset -- that merge happens in "
         "minimig_syscontrol.v, outside this one-top-module fixture and "
         "outside src/misc entirely. Not exercised here; see BOUNDARY in "
         "SPECIFICATION.md.");

    // MUTATION sol:K-0435 stimulus red: `make MUT=nothirdkey` -- Right
    // Amiga is never pressed, lctrl_down & lamiga_down & ramiga_down never
    // all reach 1, kbd_reset never pulses, and combo_fires goes false.
    expect(combo_fires, "sol:K-0435");
    // MUTATION opus:K-0441 stimulus red: same as sol:K-0435 (`make MUT=nothirdkey`).
    expect(combo_fires, "opus:K-0441");

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
