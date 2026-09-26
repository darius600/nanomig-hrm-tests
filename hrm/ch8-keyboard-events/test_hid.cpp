// ch8-keyboard-events -- hid conformance test
//
// Test purpose: three families of keyboard-event behaviour at `hid`
//   (nanomig/src/misc/hid.v) -- p.251's up/down flag on the keycode stream,
//   p.359's Caps Lock press-only behaviour (Appendix G), and p.361's
//   Ctrl-Amiga-Amiga reset combination (Appendix G). Package
//   ch8-keyboard-events, fixture `hid` alone (plus its own `keymap`
//   instance, amiga_keymap.v). THIS IS THE FIRST PACKAGE IN THIS SUITE BUILT
//   AGAINST `hid` -- no Makefile existed for this layer before this
//   package's own, written at step 3.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.251, 359, 361. Claims file ch8-keyboard-events.jsonl, 17 records (8
//   propositions after twins/splits are grouped), PLUS four more added in a
//   second round (see SECOND ROUND below): opus:8-appG.c54/c55,
//   sol:8-appG.c66/c67, none of which are in ch8-keyboard-events.jsonl --
//   they were misclassified `not-asserted` upstream and corrected into this
//   package by an independent reviewer after the first 17 were already built
//   and confirmed. 21 claims, 10 propositions, total. A red result below
//   means hid.v disagrees with the manual, not that the assertion is wrong
//   -- no assertion here was, or should be, adjusted to make the RTL pass.
// Non-goals: p.252's per-key keycode VALUES (which key maps to which code)
//   are `ch8-keycode-table`'s question, exhaustively covered there by a
//   128-value sweep of `keymap` alone -- this package only tests the
//   EVENT-LEVEL behaviour layered on top of that mapping by `hid` itself
//   (up/down flag, Caps Lock's firing rule, the reset combo). Appendix G's
//   printed reset PROTOCOL around the combo -- the $78 warning code, the
//   KCLK/KDAT handshake, the 500 ms pulse -- is not in this claims file and
//   is not tested here; nor is the key MATRIX Appendix G describes, which
//   hid.v:98-99 states in its own comment this Amiga variant does not
//   maintain.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: hid.v ports clk/reset (module-local, not `kbd_reset`; see the
//   SCOPE note in SPECIFICATION.md -- a board top ties this `reset` to
//   power-on reset, never to this module's own `kbd_reset` output, so this
//   harness's `reset` and the `kbd_reset` it measures are never the same
//   signal, matching real board wiring); data_in_strobe/data_in_start/
//   data_in[7:0] (the MCU-side byte-serial protocol that feeds keyboard
//   events in); data_out[7:0] (unused by this package -- CMD 0/4 status
//   queries are not part of any claim here); db9_port/irq/iack (unused --
//   joystick/OSD IRQ path, not part of any claim here); kbd_mouse_level/
//   kbd_mouse_type/kbd_mouse_data (the reported-event outputs Family 1 and
//   2 read); kbd_reset (Family 3's output, hid.v:30); joystick0/joystick1
//   (unused). Internally: hid.v:52-55 instantiates `keymap` (amiga_keymap.v)
//   to translate the incoming 7-bit HID-derived code into the printed Amiga
//   keycode; hid.v:100-104 is the CMD-1 event-acceptance path (Family 1 and
//   2); hid.v:107-109/163/170 is the modifier-latch/combo/pulse path
//   (Family 3).
//
// ORDER OF WORK: SPECIFICATION.md (this package's 8 PROPs, grouping the 17
// claims) was written and committed from ch8-keyboard-events.jsonl's
// `quote`/`restatement` fields alone -- commit 54c9dca -- before hid.v or
// amiga_keymap.v were opened. See SPECIFICATION.md's own "Order of work"
// section for exactly what RTL-derived background this package's task
// assignment had already supplied before that commit (module name, a
// handful of specific hid.v line numbers) and the explicit statement that
// none of it shaped any proposition's wording -- every PROP is argued from
// the manual quotation alone. Port names, stimulus encodings and the test
// design below are all step-3 work, opened only after that commit.
//
// SECOND ROUND, ORDER STATED PLAINLY: after the first 17 claims were built,
// graded and committed (12 passed, 1 failed, all mutations confirmed -- see
// git history), an independent reviewer found that
// `2026-09-16_ch8_s3_classified_all.jsonl` had marked opus:8-appG.c54/c55
// and sol:8-appG.c66/c67 (p.359's Caps-Lock-LED sentence) `not-asserted`
// with `absent_signal: caps_lock_led`, on the ground that no LED output
// exists anywhere in the pinned tree. That ground does not hold: the
// observable these four claims name is `kbd_mouse_data[7]`, a real hid.v
// output already exercised throughout this file -- the LED appears only in
// the sentence's ANTECEDENT, identifying WHICH press is under discussion,
// not as something that has to be wired to a port. THE BUILD BRIEF'S
// spec-before-RTL rule does not apply to this addendum: hid.v has been open
// throughout this round, and the two new propositions below were not
// reverse-engineered from it -- they come from p.359's LED sentence
// (verified here against the raw scan, docs/references/amiga-hrm-3rd/
// appG-keyboard/p359.txt) combined with p.360's own statement that the
// keyboard's start-up sequence ends by shutting the Caps Lock LED off
// (opus:8-appG.c112/c113, sol:8-appG.c133, p360.txt -- these three remain
// correctly `not-asserted` at THIS boundary in their own right, since no
// LED port exists to read the LED shutting off directly; they are cited
// here only as the printed fact that licenses calling the first Caps Lock
// press since reset "the press that turns the LED on"). SPECIFICATION.md
// was updated with the two new PROPs and committed before this file's own
// second-round edit, matching the spirit of the rule even though the
// letter does not require it here.
//
// A HARNESS-CONSTRUCTION FACT, worked out by tracing hid.v's two separate
// `always @(posedge clk)` blocks (mouse/keyboard-event processing at
// hid.v:58-156, and combo/pulse detection at hid.v:165-173) rather than
// assumed: `kbd_combo_now` (hid.v:163, combinational on lctrl_down/
// lamiga_down/ramiga_down) is read by the SECOND block using those
// registers' values as of the START of the current clock edge -- the same
// edge that the FIRST block uses to update one of them following a modifier
// key's event. So driving the third modifier key's event byte on some edge
// N updates that modifier's latch for edge N+1 onward, and `kbd_reset`
// itself does not read the completed three-way AND until edge N+1. Every
// key_event() call below already ends with one idle clock (strobe
// deasserted) after its data byte for exactly this reason, so a caller
// never has to add a manual extra tick before reading `kbd_reset` -- the
// three-call RESET-COMBO sequence below reads `kbd_reset==1` correctly
// immediately after its third key_event() call returns, verified against
// the baseline run.
//
// CLAIM COVERAGE: all 17 records of ch8-keyboard-events.jsonl are accounted
// for below, matching SPECIFICATION.md's 8-proposition grouping exactly.
//   PROP-B1-EVENT-BOTH (p.251) -- opus:8-keycode.c3 (compound), split into
//     EVENT-DOWN-ACCEPTED (sol:8-keycode.c3, down half) and
//     EVENT-UP-ACCEPTED (sol:8-keycode.c4, up half), plus
//     EVENT-BOTH-IN-SEQUENCE (opus:8-keycode.c3 itself) as its own
//     independent measurement on a third key, per SPECIFICATION.md's
//     explicit reasoning that the compound claim is not implied by the two
//     atomic halves without a third check.
//   PROP-B2-DOWN-VALUE / PROP-B3-UP-VALUE (p.251) -- DOWN-VALUE-UNMODIFIED
//     (opus:8-keycode.c5, sol:8-keycode.c5) and UPSTROKE-ADDS-0X80
//     (opus:8-keycode.c6, sol:8-keycode.c6).
//   PROP-C1/C2/C3 (p.359, Caps Lock) -- CAPS-LOCK-DOWN-ACCEPTED
//     (opus:8-appG.c51, sol:8-appG.c63), CAPS-LOCK-RELEASE-NOT-ACCEPTED
//     (opus:8-appG.c52, sol:8-appG.c64 -- **RED at baseline, see RESULTS**),
//     CAPS-LOCK-BIT7-CARRIES-LED-STATE (opus:8-appG.c53, sol:8-appG.c65 --
//     renamed and re-justified in the second round; see that block's own
//     comment for what changed and why).
//   PROP-C4/C5 (p.359's LED sentence, SECOND ROUND) --
//     CAPS-LOCK-LED-ON-PRESS-BIT-ZERO (opus:8-appG.c54, sol:8-appG.c66) and
//     CAPS-LOCK-LED-OFF-PRESS-BIT-ONE (opus:8-appG.c55, sol:8-appG.c67 --
//     **RED at baseline, see RESULTS**), grounded in p.360's start-up
//     sequence as described in the SECOND ROUND note above.
//   PROP-A1/A2 (p.361, reset combo) -- RESET-TASK-CAPABLE
//     (opus:8-appG.c117, sol:8-appG.c142), RESET-COMBO-FIRES-ON-ALL-THREE
//     and RESET-COMBO-PARTIAL-DOES-NOT-FIRE (both opus:8-appG.c118,
//     sol:8-appG.c143 -- the positive and negative halves of the same
//     "simultaneously... the two Amiga keys" proposition).
//
// CONTROL CASES: CONTROL-FIXTURE-DELIVERS-KNOWN-VALUE (no claim id; the
// build brief's "the fixture proves it can deliver a known value before
// anything reads one" -- run first, before any claim-graded assertion).
// CONTROL-NO-EVENT-ON-UNMAPPED-CODE (no claim id) is the required "at least
// one control expected to PASS on the same measurement basis" for
// CAPS-LOCK-RELEASE-NOT-ACCEPTED, the one assertion in this package that is
// RED at baseline: both read the same kbd_mouse_level toggle-or-not signal
// through the same key_event() path, differing only in which stimulus is
// expected (by the manual, or by the keymap's own fallback) to produce no
// event. RESET-COMBO-PARTIAL-DOES-NOT-FIRE is PROP-A2's own built-in
// control (fewer than all three keys must not fire), per SPECIFICATION.md.
// CAPS-LOCK-LED-ON-PRESS-BIT-ZERO (SECOND ROUND) is the required same-basis
// PASS control for CAPS-LOCK-LED-OFF-PRESS-BIT-ONE, the other new
// assertion: same key, same key_event()+bit7 read, differing only in which
// press (first vs second since a known-off LED state) the manual predicts a
// 0 or a 1 for.
//
// RESULTS: baseline (no MUT flag) is 13 passed, 2 failed -- see RESULTS-2
// below for the second round; CAPS-LOCK-RELEASE-NOT-ACCEPTED's own comment
// covers the first.
//
// PER-ASSERTION MUTATION RECORD. Fifteen variants total (thirteen from the
// first round, two more below), each its own full build+run
// (`make MUT=<switch>`, e.g. `make MUT=MUT_7`), built, run, confirmed and
// reverted -- not re-run for this header update. Format
// (tools/check_gates.py): // MUTATION <assertion-label> <oracle|stimulus|
// none> <red|green|impossible> <what changed>. Every mutation below is
// `stimulus` -- what the module is asked to do -- never `oracle`; no
// expected value in any expect() below is ever touched by its own
// mutation build.
//
// MUTATION CONTROL-FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUT_0 skips the '1'-key press entirely -- kbd_mouse_level stays at its post-reset 0 and kbd_mouse_data is never written to 0x01, so the baseline PASS (both conditions true) goes red.
// MUTATION EVENT-DOWN-ACCEPTED stimulus red MUT_1 presses HID code 0x00 (NoEvent, unmapped -- amiga_keymap.v's default 7'h7f) instead of 'a' (0x04) -- hid.v:101's `amiga_keycode != 7'h7f` guard is then false, kbd_mouse_level never toggles, and the baseline PASS goes red.
// MUTATION EVENT-UP-ACCEPTED stimulus red MUT_2 releases HID code 0x00 instead of 'a' -- same unmapped-code guard failure on the release byte; baseline PASS (toggled) goes red.
// MUTATION EVENT-BOTH-IN-SEQUENCE stimulus red MUT_3 omits the release call entirely -- only the press toggles kbd_mouse_level, so toggle_count reads 1, not 2; baseline PASS goes red.
// MUTATION DOWN-VALUE-UNMODIFIED stimulus red MUT_4 presses HID code 0x1e ('1') instead of 0x04 ('a') -- kbd_mouse_data reads 0x01 (the correct value for THAT key), not the 0x20 this assertion's untouched expected value still demands for 'a'; baseline PASS goes red.
// MUTATION UPSTROKE-ADDS-0X80 stimulus red MUT_5 presses 'a' again instead of releasing it (data_in[7] left at 0 on the second call too) -- the second reading equals the first exactly, not plus 0x80; baseline PASS goes red.
// MUTATION CAPS-LOCK-DOWN-ACCEPTED stimulus red MUT_6 presses HID code 0x00 instead of Caps Lock's 0x39 -- unmapped-code guard failure as in MUT_1; baseline PASS goes red.
// MUTATION CAPS-LOCK-RELEASE-NOT-ACCEPTED stimulus green MUT_7 releases HID code 0x00 (unmapped) instead of Caps Lock's own 0x39. This is the one assertion ALREADY RED at baseline (the manual/RTL disagreement this package exists to report), so its mutation runs the opposite direction from every other assertion here, per the build brief's rule for a red baseline: with no real event able to fire on an unmapped release either, "no toggle on release" reads true and the assertion flips to green -- proof the baseline FAIL is attributable to Caps Lock's own release genuinely firing an event (hid.v:101 has no per-key or data_in[7] branch), not to a vacuous, always-failing assertion.
// MUTATION CONTROL-NO-EVENT-ON-UNMAPPED-CODE stimulus red MUT_8 uses HID code 0x04 ('a', mapped) instead of 0x00 -- the event IS accepted and kbd_mouse_level toggles, so the baseline PASS (no toggle) goes red.
// MUTATION CAPS-LOCK-BIT7-CARRIES-LED-STATE stimulus red MUT_9 sends Caps Lock's byte with the released flag set (0xB9) instead of pressed (0x39) -- bit 7 of kbd_mouse_data then reads 1, not the 0 this assertion checks for the first downstroke since reset; baseline PASS goes red.
// MUTATION RESET-COMBO-FIRES-ON-ALL-THREE stimulus red MUT_10 omits the third key_event() call (Right Amiga, HID 0x6f) -- lctrl_down & lamiga_down & ramiga_down never all reach 1, kbd_reset never pulses, baseline PASS goes red.
// MUTATION RESET-COMBO-PARTIAL-DOES-NOT-FIRE stimulus red MUT_11 adds the third key_event() call (Right Amiga) that the baseline deliberately withholds -- the full combo now forms and kbd_reset pulses, so the baseline PASS (no pulse) goes red.
// MUTATION RESET-TASK-CAPABLE stimulus red MUT_12 presses none of the three modifier keys at all -- kbd_reset stays 0 throughout, baseline PASS goes red.
//
// RESULTS-2 (SECOND ROUND): CAPS-LOCK-LED-ON-PRESS-BIT-ZERO passes (13
// passed includes it); CAPS-LOCK-LED-OFF-PRESS-BIT-ONE is the second red --
// hid.v has no memory of how many times Caps Lock has been pressed, so
// hid.v:104 mirrors `data_in[7]` verbatim on every press regardless of
// position in the sequence, and the second press reads bit 7 == 0 exactly
// like the first, where p.359's LED rule requires 1. A distinct defect from
// CAPS-LOCK-RELEASE-NOT-ACCEPTED: that one is about a RELEASE firing an
// event at all; this one is about a PRESS carrying the wrong bit value. A
// driver that only counts accepted events would miss this one; a driver
// that reads the bit's value would miss neither.
// MUTATION CAPS-LOCK-LED-ON-PRESS-BIT-ZERO stimulus red MUT_13 sends the first Caps Lock event as a release (0xB9) instead of a press (0x39) -- bit 7 then reads 1, not the 0 this assertion checks for the LED-turning-on press; baseline PASS goes red.
// MUTATION CAPS-LOCK-LED-OFF-PRESS-BIT-ONE stimulus green MUT_14 sends the SECOND Caps Lock event as a release (0xB9) instead of a second press (0x39). This is the other assertion ALREADY RED at baseline, so its mutation runs the opposite direction, per the same red-baseline rule as CAPS-LOCK-RELEASE-NOT-ACCEPTED above: a release genuinely does carry bit 7 == 1 (see EVENT-UP-ACCEPTED's own family for why), so this specific assertion flips to green -- proof the baseline FAIL is attributable to the second PRESS specifically carrying the wrong bit, not to a vacuous, always-failing check.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vhid.h"
#include "verilated.h"

#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif
#ifndef MUT_3
#define MUT_3 0
#endif
#ifndef MUT_4
#define MUT_4 0
#endif
#ifndef MUT_5
#define MUT_5 0
#endif
#ifndef MUT_6
#define MUT_6 0
#endif
#ifndef MUT_7
#define MUT_7 0
#endif
#ifndef MUT_8
#define MUT_8 0
#endif
#ifndef MUT_9
#define MUT_9 0
#endif
#ifndef MUT_10
#define MUT_10 0
#endif
#ifndef MUT_11
#define MUT_11 0
#endif
#ifndef MUT_12
#define MUT_12 0
#endif
#ifndef MUT_13
#define MUT_13 0
#endif
#ifndef MUT_14
#define MUT_14 0
#endif

namespace {

// HID source codes (amiga_keymap.v's `code` input, 7 bits) used as stimulus
// below, with the Amiga code (amiga_keymap.v's `amiga` output) each
// resolves to, read directly from that file:
constexpr uint8_t HID_A            = 0x04; // -> amiga 0x20 ("a")
constexpr uint8_t HID_B            = 0x05; // -> amiga 0x33 ("b")
constexpr uint8_t HID_ONE          = 0x1e; // -> amiga 0x01 ("1")
constexpr uint8_t HID_CAPS_LOCK    = 0x39; // -> amiga 0x62
constexpr uint8_t HID_UNMAPPED     = 0x00; // "NoEvent" -- no case in
                                            // amiga_keymap.v, falls through
                                            // to its default, 7'h7f
constexpr uint8_t HID_LEFT_CTRL    = 0x68; // -> amiga 0x63
constexpr uint8_t HID_LEFT_AMIGA   = 0x6b; // -> amiga 0x66 ("left meta")
constexpr uint8_t HID_RIGHT_AMIGA  = 0x6f; // -> amiga 0x67 ("right meta")

constexpr uint8_t AMIGA_A          = 0x20;
constexpr uint8_t AMIGA_ONE        = 0x01;
constexpr uint8_t AMIGA_CAPS_LOCK  = 0x62;

class Harness {
public:
    Vhid* dut;
    long long tickcount = 0;

    Harness() : dut(new Vhid) {
        dut->clk = 0;
        dut->reset = 0;
        dut->data_in_strobe = 0;
        dut->data_in_start = 0;
        dut->data_in = 0;
        dut->db9_port = 0;
        dut->iack = 0;
        do_reset();
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        ++tickcount;
    }

    // hid.v's own `reset` (hid.v:9) -- NOT `kbd_reset`. Clears state,
    // irq/irq_enable, kbd_mouse_level and the three modifier latches
    // (hid.v:59-66), and separately kbd_combo_prev/kbd_reset (hid.v:166-168).
    // It does NOT clear kbd_mouse_data/kbd_mouse_type/command/mouse_buttons/
    // joystick0/joystick1 -- irrelevant here, since every measurement below
    // reads kbd_mouse_data only after a key_event() call of its own, never
    // relying on its reset-time value, and command is always re-established
    // by key_event()'s own start byte before anything reads state again.
    void do_reset() {
        dut->reset = 1;
        tick(); tick();
        dut->reset = 0;
        tick(); tick();
    }

    // Drives one CMD-1 keyboard byte through hid's MCU-facing protocol:
    // a start byte (selects command 1, resets `state` to 0 -- hid.v:84-86)
    // then the data byte itself (the event, accepted at hid.v:100-104 when
    // `state` is still 0 from the start byte just sent). The trailing idle
    // tick (strobe deasserted) is not needed for THIS module's own outputs,
    // which update on the data byte's own edge -- it exists so a caller
    // chaining three of these (the reset-combo tests) sees kbd_reset's
    // combinational precursor `kbd_combo_now` (hid.v:163) already reflect
    // the modifier latch this call just wrote, on the very next edge. See
    // the header's HARNESS-CONSTRUCTION FACT note.
    void key_event(uint8_t code7bit, bool released) {
        uint8_t byte = (released ? 0x80 : 0x00) | (code7bit & 0x7f);
        dut->data_in = 0x01;           // CMD 1: keyboard data
        dut->data_in_start = 1;
        dut->data_in_strobe = 1;
        tick();
        dut->data_in = byte;
        dut->data_in_start = 0;
        tick();
        dut->data_in_strobe = 0;
        tick();
    }
};

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

std::string hex2(uint8_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%02X", v);
    return std::string(buf);
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    Harness h;

    // =========================================================================
    // CONTROL-FIXTURE-DELIVERS-KNOWN-VALUE (no claim id; harness precondition).
    // Per the build brief: the fixture must prove it can deliver a known
    // value before anything below reads one.
    // =========================================================================
    {
        bool level_before = h.dut->kbd_mouse_level;
#if MUT_0
        // MUTATION: no stimulus at all.
#else
        h.key_event(HID_ONE, false);
#endif
        checks.expect(
            level_before == false
            && h.dut->kbd_mouse_level == true
            && h.dut->kbd_mouse_data == AMIGA_ONE,
            "CONTROL-FIXTURE-DELIVERS-KNOWN-VALUE (no manual claim; harness "
            "precondition): after hid's own reset, kbd_mouse_level reads a "
            "known 0, and a single press of HID code " + hex2(HID_ONE) +
            " ('1') both flips kbd_mouse_level to 1 and delivers the exact "
            "expected byte " + hex2(AMIGA_ONE) + " on kbd_mouse_data -- "
            "proving this harness can drive a distinguishable, predictable "
            "value through hid before any claim below relies on that. "
            "MUTATION: send no stimulus at all -- kbd_mouse_level must stay "
            "0 and kbd_mouse_data must not become " + hex2(AMIGA_ONE) + ".");
    }

    // =========================================================================
    // PROP-B1-EVENT-BOTH (p.251) -- "These codes include both the down and
    // up transitions of the keys." opus:8-keycode.c3 (compound), split by
    // the other reader into sol:8-keycode.c3 (down half) and
    // sol:8-keycode.c4 (up half). Graded as three assertions: the two
    // atomic halves, plus the compound reading as its own measurement on a
    // fresh key (SPECIFICATION.md's reasoning: the compound claim is not
    // implied by the two atomic checks alone without a third, independent
    // count).
    // =========================================================================
    {
        h.do_reset();
        bool level_before = h.dut->kbd_mouse_level;
#if MUT_1
        h.key_event(HID_UNMAPPED, false);
#else
        h.key_event(HID_A, false);
#endif
        bool toggled = (h.dut->kbd_mouse_level != level_before);
        checks.expect(toggled,
            "EVENT-DOWN-ACCEPTED (p.251, \"These codes include both the "
            "down and up transitions of the keys.\"): a key's downstroke "
            "(HID " + hex2(HID_A) + ", 'a') produces a reported event -- "
            "kbd_mouse_level toggles. sol:8-keycode.c3. MUTATION: press an "
            "unmapped HID code (" + hex2(HID_UNMAPPED) + ", NoEvent) "
            "instead of 'a' -- the level must then NOT toggle.");
    }
    {
        h.do_reset();
        h.key_event(HID_A, false); // establish pressed state first
        bool level_before = h.dut->kbd_mouse_level;
#if MUT_2
        h.key_event(HID_UNMAPPED, true);
#else
        h.key_event(HID_A, true);
#endif
        bool toggled = (h.dut->kbd_mouse_level != level_before);
        checks.expect(toggled,
            "EVENT-UP-ACCEPTED (p.251, same sentence as EVENT-DOWN-ACCEPTED): "
            "a key's upstroke ALSO produces a reported event -- releasing "
            "the same key (HID " + hex2(HID_A) + ", 'a') toggles "
            "kbd_mouse_level a second time. sol:8-keycode.c4. MUTATION: "
            "release an unmapped HID code (" + hex2(HID_UNMAPPED) + ") "
            "instead of 'a' -- the level must then NOT toggle.");
    }
    {
        h.do_reset();
        int toggle_count = 0;
        bool level = h.dut->kbd_mouse_level;
        h.key_event(HID_B, false); // a THIRD key, independent of the two atomic checks above
        if (h.dut->kbd_mouse_level != level) { ++toggle_count; level = h.dut->kbd_mouse_level; }
#if MUT_3
        // MUTATION: omit the release call entirely.
#else
        h.key_event(HID_B, true);
        if (h.dut->kbd_mouse_level != level) { ++toggle_count; level = h.dut->kbd_mouse_level; }
#endif
        checks.expect(toggle_count == 2,
            "EVENT-BOTH-IN-SEQUENCE (p.251): a single press-then-release "
            "sequence on one key (HID " + hex2(HID_B) + ", 'b') reports "
            "BOTH transitions -- two separate kbd_mouse_level toggles, not "
            "one. opus:8-keycode.c3. MUTATION: omit the release call -- "
            "exactly one transition (the press) is reported, not two.");
    }

    // =========================================================================
    // PROP-B2-DOWN-VALUE (p.251) -- "A downstroke of the key transmits the
    // value shown here." opus:8-keycode.c5, sol:8-keycode.c5.
    // =========================================================================
    {
        h.do_reset();
#if MUT_4
        h.key_event(HID_ONE, false);
#else
        h.key_event(HID_A, false);
#endif
        uint8_t down_value = h.dut->kbd_mouse_data;
        checks.expect(down_value == AMIGA_A,
            "DOWN-VALUE-UNMODIFIED (p.251): a downstroke transmits the "
            "printed keycode value unmodified -- pressing HID code " +
            hex2(HID_A) + " ('a') yields kbd_mouse_data == " + hex2(AMIGA_A) +
            ", no offset applied. opus:8-keycode.c5, sol:8-keycode.c5. "
            "MUTATION: press a different key (HID " + hex2(HID_ONE) +
            ", '1') instead -- kbd_mouse_data must then read " +
            hex2(AMIGA_ONE) + ", not " + hex2(AMIGA_A) + " (this "
            "assertion's own expected value is untouched by the mutation).");
    }

    // =========================================================================
    // PROP-B3-UP-VALUE (p.251) -- "An upstroke of the key transmits this
    // value plus $80." opus:8-keycode.c6, sol:8-keycode.c6.
    // =========================================================================
    {
        h.do_reset();
        h.key_event(HID_A, false);
        uint8_t press_value = h.dut->kbd_mouse_data;
        checks.note("UPSTROKE-ADDS-0X80 basis: pressing 'a' (HID " +
            hex2(HID_A) + ") first read kbd_mouse_data == " +
            hex2(press_value) + " (graded above as DOWN-VALUE-UNMODIFIED, "
            "not re-graded here).");
#if MUT_5
        h.key_event(HID_A, false); // press again instead of releasing
#else
        h.key_event(HID_A, true);
#endif
        uint8_t release_value = h.dut->kbd_mouse_data;
        checks.expect(release_value == (uint8_t)(press_value + 0x80),
            "UPSTROKE-ADDS-0X80 (p.251): an upstroke transmits the same "
            "value plus $80 -- releasing 'a' (HID " + hex2(HID_A) +
            ") after pressing it yields kbd_mouse_data == " +
            hex2((uint8_t)(press_value + 0x80)) + ", the press value (" +
            hex2(press_value) + ") plus 0x80. opus:8-keycode.c6, "
            "sol:8-keycode.c6. MUTATION: press 'a' again instead of "
            "releasing it -- the second reading must then equal the first "
            "exactly, not plus 0x80.");
    }

    // =========================================================================
    // PROP-C1-CAPS-DOWN (p.359) -- "This key is different from all the
    // others in that it generates a keycode only when it is pushed down,
    // never when it is released." (pressed half.) opus:8-appG.c51,
    // sol:8-appG.c63.
    // =========================================================================
    {
        h.do_reset();
        bool level_before = h.dut->kbd_mouse_level;
#if MUT_6
        h.key_event(HID_UNMAPPED, false);
#else
        h.key_event(HID_CAPS_LOCK, false);
#endif
        bool toggled = (h.dut->kbd_mouse_level != level_before);
        checks.expect(toggled && h.dut->kbd_mouse_data == AMIGA_CAPS_LOCK,
            "CAPS-LOCK-DOWN-ACCEPTED (p.359): pressing Caps Lock (HID " +
            hex2(HID_CAPS_LOCK) + ") produces a reported event carrying its "
            "printed code, " + hex2(AMIGA_CAPS_LOCK) + ". opus:8-appG.c51, "
            "sol:8-appG.c63. MUTATION: press an unmapped HID code (" +
            hex2(HID_UNMAPPED) + ") instead of Caps Lock -- no event must "
            "be reported.");
    }

    // =========================================================================
    // PROP-C2-CAPS-NO-RELEASE (p.359) -- same sentence, released half:
    // "...never when it is released." opus:8-appG.c52, sol:8-appG.c64.
    //
    // MEASURED: hid.v:101's acceptance condition (`state == 4'd0 &&
    // amiga_keycode != 7'h7f`) carries no branch on the key's identity and
    // no branch on data_in[7] -- nothing in hid singles Caps Lock's release
    // out. This assertion is RED AT BASELINE: Caps Lock's release IS
    // accepted as a reported event on this RTL, contradicting the printed
    // "never when it is released." That is a finding about hid.v, reported
    // as such -- the assertion is not reworded and no oracle is adjusted.
    // =========================================================================
    {
        h.do_reset();
        h.key_event(HID_CAPS_LOCK, false); // establish pressed state first (not re-graded here)
        bool level_before = h.dut->kbd_mouse_level;
#if MUT_7
        h.key_event(HID_UNMAPPED, true);
#else
        h.key_event(HID_CAPS_LOCK, true);
#endif
        bool toggled_on_release = (h.dut->kbd_mouse_level != level_before);
        checks.expect(!toggled_on_release,
            "CAPS-LOCK-RELEASE-NOT-ACCEPTED (p.359, \"generates a keycode "
            "only when it is pushed down, never when it is released\"): "
            "releasing Caps Lock (HID " + hex2(HID_CAPS_LOCK) + ") must NOT "
            "toggle kbd_mouse_level. opus:8-appG.c52, sol:8-appG.c64. "
            "MUTATION: release an unmapped HID code (" + hex2(HID_UNMAPPED) +
            ") instead of Caps Lock's own " + hex2(HID_CAPS_LOCK) +
            " -- with nothing able to fire either way, this specific "
            "assertion reads green, proving the baseline FAIL is "
            "attributable to Caps Lock's release genuinely firing an event "
            "on this RTL, not to a vacuous, always-failing check.");
    }
    // CONTROL-NO-EVENT-ON-UNMAPPED-CODE: the required same-basis PASS
    // control for the red assertion just above -- same key_event() +
    // level-toggle read, a case both the manual's silence and the keymap's
    // own fallback agree produces no event.
    {
        h.do_reset();
        bool level_before = h.dut->kbd_mouse_level;
#if MUT_8
        h.key_event(HID_A, false);
#else
        h.key_event(HID_UNMAPPED, false);
#endif
        bool toggled = (h.dut->kbd_mouse_level != level_before);
        checks.expect(!toggled,
            "CONTROL-NO-EVENT-ON-UNMAPPED-CODE (no manual claim; control "
            "for CAPS-LOCK-RELEASE-NOT-ACCEPTED, same measurement basis): "
            "an unmapped HID code (" + hex2(HID_UNMAPPED) +
            ", amiga_keymap.v's default 7'h7f) must not toggle "
            "kbd_mouse_level -- proving the toggle-read this package relies "
            "on can and does read \"no event\" correctly. MUTATION: use a "
            "mapped code (HID " + hex2(HID_A) + ", 'a') instead -- the "
            "level must then toggle.");
    }

    // =========================================================================
    // PROP-C3-CAPS-UPDOWN-BIT (p.359) -- "However, the up/down bit is still
    // used." opus:8-appG.c53, sol:8-appG.c65.
    //
    // SECOND ROUND, CORRECTED READING. This assertion's label and
    // justification originally read: "Caps Lock's own downstroke event
    // carries bit 7 == 0, the same up/down bit position PROP-B2-DOWN-VALUE
    // established for an ordinary key's downstroke -- Caps Lock's byte is
    // not shaped differently just because it is exceptional about when it
    // fires." That is WRONG, and p.359's own next sentence (not in this
    // package's claims file until the second round) says so directly: "When
    // pushing the Caps Lock key turns on the Caps Lock LED, the up/down bit
    // will be 0; when pushing Caps Lock shuts off the LED, the up/down bit
    // will be 1." Caps Lock's byte IS shaped differently -- bit 7 here means
    // the LED's target state after this press, not "key currently down" the
    // way PROP-B2/B3's mechanism reads it for an ordinary key. The original
    // reading happened to land on the right expected value for the FIRST
    // press by coincidence (LED-target-state 0 and physical-press-state 0
    // agree on the first press since a known-off LED, see p.360), which is
    // exactly why it went uncaught by this assertion alone -- it takes the
    // SECOND press, graded separately below, to tell the two readings
    // apart. Corrected here: expected 0 is re-derived from p.359's LED rule
    // plus p.360's start-up-shuts-the-LED-off fact, not from an assumed
    // analogy to ordinary keys. "The up/down bit is still used" (c53/c65)
    // is, read this way, the claim that the bit position is populated with
    // real, LED-driven information rather than blanked out just because
    // Caps Lock fires only on the downstroke -- and that is still exactly
    // what this assertion checks.
    // =========================================================================
    {
        h.do_reset(); // LED state, per p.360, is off immediately after start-up
#if MUT_9
        h.key_event(HID_CAPS_LOCK, true);
#else
        h.key_event(HID_CAPS_LOCK, false); // the first press since that known-off state
#endif
        bool bit7 = (h.dut->kbd_mouse_data >> 7) & 1;
        checks.expect(bit7 == 0,
            "CAPS-LOCK-BIT7-CARRIES-LED-STATE (p.359, \"However, the "
            "up/down bit is still used.\"): Caps Lock's own accepted "
            "downstroke event still carries a real, populated bit 7 -- on "
            "the first press since a known-off LED state (p.360's "
            "start-up sequence), that bit reads 0, consistent with p.359's "
            "own LED rule for the press that turns the LED on (graded on "
            "its own citation by CAPS-LOCK-LED-ON-PRESS-BIT-ZERO below, not "
            "here). opus:8-appG.c53, sol:8-appG.c65. MUTATION: send Caps "
            "Lock's byte with the released flag set (" +
            hex2(0x80 | HID_CAPS_LOCK) + ") instead of pressed (" +
            hex2(HID_CAPS_LOCK) + ") -- bit 7 must then read 1.");
    }

    // =========================================================================
    // PROP-C4-CAPS-LED-ON-PRESS (p.359, SECOND ROUND) -- "When pushing the
    // Caps Lock key turns on the Caps Lock LED, the up/down bit will be 0"
    // (first half of the sentence following PROP-C3's). opus:8-appG.c54,
    // sol:8-appG.c66. Grounded by p.360: "Finally, the Caps Lock LED is
    // shut off. This marks the end of the start-up sequence..." -- the
    // keyboard's own LED is off immediately after start-up, so the FIRST
    // Caps Lock press since that point is, by p.359's own rule, the press
    // that turns the LED on. No LED port exists anywhere in the pinned
    // tree (confirmed again here -- hid.v's complete port list, read at
    // the top of this file, has none), and none is needed: the LED
    // appears only in the antecedent identifying WHICH press this is: the
    // graded observable is kbd_mouse_data[7], the same real hid.v output
    // every other assertion in this file already reads.
    // =========================================================================
    {
        h.do_reset(); // known-off LED state, per p.360
#if MUT_13
        h.key_event(HID_CAPS_LOCK, true);
#else
        h.key_event(HID_CAPS_LOCK, false); // the press that turns the LED on
#endif
        bool bit7 = (h.dut->kbd_mouse_data >> 7) & 1;
        checks.expect(bit7 == 0,
            "CAPS-LOCK-LED-ON-PRESS-BIT-ZERO (p.359, \"When pushing the "
            "Caps Lock key turns on the Caps Lock LED, the up/down bit "
            "will be 0\", combined with p.360's start-up sequence ending "
            "\"Finally, the Caps Lock LED is shut off\" -- the LED is "
            "known off immediately after start-up, so the FIRST Caps Lock "
            "press since then is the press that turns it on): that first "
            "press carries bit 7 == 0 on kbd_mouse_data. opus:8-appG.c54, "
            "sol:8-appG.c66. MUTATION: send that event as a release (" +
            hex2(0x80 | HID_CAPS_LOCK) + ") instead of a press (" +
            hex2(HID_CAPS_LOCK) + ") -- bit 7 must then read 1.");
    }

    // =========================================================================
    // PROP-C5-CAPS-LED-OFF-PRESS (p.359, SECOND ROUND) -- "when pushing
    // Caps Lock shuts off the LED, the up/down bit will be 1" (second half
    // of the same sentence). opus:8-appG.c55, sol:8-appG.c67. The SECOND
    // Caps Lock press since the known-off start (one press, one release,
    // one more press) is, by the same rule, the press that shuts the LED
    // back off.
    //
    // MEASURED: hid.v has no register anywhere that counts how many times
    // Caps Lock has been pressed, or that tracks an LED state at all --
    // hid.v:104 mirrors `data_in[7]` verbatim into bit 7 on every accepted
    // event, press or release, first or fifth, with no memory of what came
    // before. THIS ASSERTION IS RED AT BASELINE: the second press reads
    // bit 7 == 0, identically to the first, where p.359's rule requires 1.
    // That is a finding about hid.v, reported as such -- the assertion is
    // not reworded and no oracle is adjusted.
    // =========================================================================
    {
        h.do_reset();
        h.key_event(HID_CAPS_LOCK, false); // press #1 -- turns the LED on (graded above, not here)
        h.key_event(HID_CAPS_LOCK, true);  // release #1 (graded above by CAPS-LOCK-RELEASE-NOT-ACCEPTED, not here)
#if MUT_14
        h.key_event(HID_CAPS_LOCK, true);  // release, not a second press
#else
        h.key_event(HID_CAPS_LOCK, false); // press #2 -- the press that shuts the LED back off
#endif
        bool bit7 = (h.dut->kbd_mouse_data >> 7) & 1;
        checks.expect(bit7 == 1,
            "CAPS-LOCK-LED-OFF-PRESS-BIT-ONE (p.359, \"when pushing Caps "
            "Lock shuts off the LED, the up/down bit will be 1\", same LED "
            "rule as CAPS-LOCK-LED-ON-PRESS-BIT-ZERO, now on the SECOND "
            "press since the known-off start): that second press must "
            "carry bit 7 == 1 on kbd_mouse_data -- the bit that says the "
            "LED is going off. opus:8-appG.c55, sol:8-appG.c67. MUTATION: "
            "send the second event as a release (" +
            hex2(0x80 | HID_CAPS_LOCK) + ") instead of a second press (" +
            hex2(HID_CAPS_LOCK) + ") -- a release genuinely does carry bit "
            "7 == 1, so this specific assertion reads green, proving the "
            "baseline FAIL is attributable to the second PRESS specifically "
            "carrying the wrong bit, not to a vacuous, always-failing "
            "check.");
    }

    // =========================================================================
    // PROP-A1-RESET-TASK (p.361) -- "The keyboard has the additional task
    // of resetting the computer on the command of the user."
    // opus:8-appG.c117, sol:8-appG.c142. The coarser capability claim, kept
    // as its own assertion per SPECIFICATION.md even though it shares its
    // mechanism with PROP-A2 below (ch7-overlay-source's PROP-A/PROP-C
    // precedent: collapsing them would under-cite the manual).
    // =========================================================================
    {
        h.do_reset();
#if MUT_12
        // MUTATION: press none of the three modifier keys at all.
#else
        h.key_event(HID_LEFT_CTRL, false);
        h.key_event(HID_LEFT_AMIGA, false);
        h.key_event(HID_RIGHT_AMIGA, false);
#endif
        checks.expect(h.dut->kbd_reset == 1,
            "RESET-TASK-CAPABLE (p.361): some mechanism reachable from the "
            "keyboard side of this boundary is capable of producing a "
            "reset pulse -- pressing Left Ctrl, Left Amiga and Right Amiga "
            "(HID " + hex2(HID_LEFT_CTRL) + "/" + hex2(HID_LEFT_AMIGA) +
            "/" + hex2(HID_RIGHT_AMIGA) + ") makes kbd_reset pulse high. "
            "opus:8-appG.c117, sol:8-appG.c142. MUTATION: press none of "
            "the three modifier keys at all -- kbd_reset must stay 0.");
    }

    // =========================================================================
    // PROP-A2-RESET-COMBO (p.361) -- "The user initiates Reset Warning by
    // simultaneously pressing the Ctrl key and the two Amiga keys."
    // opus:8-appG.c118, sol:8-appG.c143. Two assertions: the positive
    // (all three -> fires) and the negative (fewer than three -> does not
    // fire, the control this proposition's own "simultaneously" wording
    // requires per SPECIFICATION.md).
    // =========================================================================
    {
        h.do_reset();
        h.key_event(HID_LEFT_CTRL, false);
        h.key_event(HID_LEFT_AMIGA, false);
#if MUT_10
        // MUTATION: omit the third key_event() call (Right Amiga).
#else
        h.key_event(HID_RIGHT_AMIGA, false);
#endif
        checks.expect(h.dut->kbd_reset == 1,
            "RESET-COMBO-FIRES-ON-ALL-THREE (p.361): pressing all three of "
            "Ctrl, Left Amiga and Right Amiga pulses kbd_reset. "
            "opus:8-appG.c118, sol:8-appG.c143. MUTATION: omit the third "
            "key_event() call (Right Amiga, HID " + hex2(HID_RIGHT_AMIGA) +
            ") -- kbd_reset must then never pulse.");
    }
    {
        h.do_reset();
        h.key_event(HID_LEFT_CTRL, false);
        h.key_event(HID_LEFT_AMIGA, false);
        // Right Amiga deliberately withheld -- the baseline stimulus for
        // this assertion IS the two-of-three case.
#if MUT_11
        h.key_event(HID_RIGHT_AMIGA, false);
#endif
        checks.expect(h.dut->kbd_reset == 0,
            "RESET-COMBO-PARTIAL-DOES-NOT-FIRE (p.361, same sentence's "
            "\"simultaneously\"): holding only two of the three keys (Ctrl "
            "and Left Amiga) must NOT pulse kbd_reset -- the combination, "
            "not any two of its members, is what the page names. "
            "opus:8-appG.c118, sol:8-appG.c143. MUTATION: also press Right "
            "Amiga (HID " + hex2(HID_RIGHT_AMIGA) + ") -- kbd_reset must "
            "then read 1, not 0.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
