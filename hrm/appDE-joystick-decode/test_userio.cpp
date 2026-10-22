// Test purpose: fixture: userio.v, compiled alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 228, 232, 233, 234, 281, 328, 329, 332
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// appDE-joystick-decode -- fixture: userio.v, compiled alone.
//
// Claims graded here: work/2026-09-17_appDE_packages.jsonl, classify.package ==
// "appDE-joystick-decode" (30 claims, all p.329: prose + the CONNECTOR PIN
// USAGE FOR DIGITAL JOYSTICK INPUTS table + the NOTE), grouped into 16
// propositions (sol/opus pairs that read the same page fact are one
// proposition; two are opus-only). 9 propositions (17 uids) are declined --
// already graded, on the SAME axis, by hrm/ch8-joystick-switch-decode,
// hrm/ch8-controller-pin-function-map or hrm/ch8-ciaapra-fire-buttons, or (one
// fragment) not constructible at any module. 7 propositions (13 uids) are
// graded here.
//
// p.329 CONTRADICTS ITSELF IN ITS TEXT LAYER (appE/corpus-defects.md), and the
// PROSE IS RIGHT: the render binds pins 1-4 to <9 xor 8>, <1 xor 0>, <9>, <1>,
// exactly matching the prose four lines above the table -- confirmed here
// against figures/p329.png directly, not against p329.txt's displaced
// HARDWARE REGISTER column. Table cells below quote the render.
//
// ONE PRINTED DEFECT IS KEPT, NOT "FIXED": pin 2's register cell opens with a
// ROUND PARENTHESIS in print -- "JOY[0/1]DAT(1 xor 0>" -- verified at 600 dpi
// and legible at the 200 dpi of figures/p329.png (same defect, same row
// position, at p.328's own pin 2). See the comment on the pin-2 assertion.
//
// RTL notes discovered reading userio.v (needed to follow this file):
//   - joy_swap is a hardwired constant 0 (userio.v:92), so JOY0DAT's switch
//     decode (dmouse0dat, via _djoy1) is driven by the `_joy2` port
//     (userio.v:214-219) -- every stimulus below that targets JOY0DAT drives
//     `_joy2`, matching hrm/ch8-joystick-switch-decode's own convention.
//   - Within either 16-bit joystick word: bit0=RIGHT*, bit1=LEFT*, bit2=BACK*,
//     bit3=FORWARD*, bit4=FIRE* (mode-select), bit5="second button" (POTINP
//     path only). Idle is 1 (open), pressed is 0 (closed) -- undriven bits
//     default to Verilator's 0, which is the PRESSED level here, so every
//     stimulus below drives every bit explicitly, every cycle.
//   - JOY0DAT only shows the switch decode while joy1enable is latched 1
//     (userio.v:222-227,290-293); enable_joy1_mode() below pulses the mode-
//     select bit low and releases it, exactly as hrm/ch8-joystick-switch-
//     decode's own harness does.
//   - potreg (the POTGO write register, userio.v:78,98-105) resets to 0 and
//     this file never writes POTGO, so it stays 0 throughout -- which is what
//     makes potcap[1] (POTINP bit 10, userio.v:171-175) track `_djoy1[5]`
//     directly with mouse_btn==0: potcap[1] <= _mright & _djoy1[5] &
//     ~(potreg[11] & ~potreg[10]) reduces to `_djoy1[5]` alone. That fact
//     carries the group-H assertion below.
//
// A GAP CHECKED AND NOT FILLED: p.329 also marks connector pins 5 and 9
// "Unused". Pin 5 is testable here -- bit 5 of the 16-bit word is userio's
// own "second button" line (already read via POTINP by hrm/ch8-joystick-
// switch-decode L17, sourced from p.233, not this page), and driving it is a
// physically grounded stimulus. Pin 9 is NOT: userio.v's ports carry only a
// role-named 16-bit bus (minimig.v:205-206's own comment: "[fire2,fire,up,
// down,left,right]", six roles, bits 5..0) with no pin-indexed input at all,
// and every board top-level file checked (tang/nano20k/top.sv:54,89 --
// `input [5:0] js1`, `db9_joy1[5:0]`; gatemate/evb/top.sv:59 the same) wires
// only SIX bits from the DB9 connector into that bus -- there is no seventh
// wire anywhere in the tree for a "pin 9" signal to be. Treating bit 9 of the
// internal word as "pin 9" would be inventing a correspondence the sources do
// not state, purely from the two numerals matching -- the opposite of what
// let pin 5 be trusted. Declined below, reversing classify's "status:
// testable" call for that one claim after checking the board wiring, which
// classify's own evidence did not do.
//
// MUTATION records (the exact second token is the label passed to expect()
// below):
//
// MUTATION harness:live-known-words stimulus red drove the "right" case with
//   the second-button bit (5, no effect on the decode) instead of the right
//   pin (0); the two delivered words collapsed to the same value.
// MUTATION sol:E-joystick.c8+opus:E-joystick.c9 stimulus red pressed the
//   right pin instead of the left pin for the "pin 3 alone" case; bit 9
//   stayed 0, failing the p.332-graded clause while the pin-4 pair and the
//   POTINP control read as measured.
// MUTATION sol:E-joystick.c9+opus:E-joystick.c10 stimulus red pressed the
//   left pin instead of the right pin; the decode check (d.right==1) failed
//   while the four-reads-agree half stayed true, showing the assertion reads
//   the real switch state and not just its own self-consistency.
// MUTATION sol:E-joystick.c10+opus:E-joystick.c12 stimulus red pressed the
//   left pin instead of the forward pin (single operand only, per the
//   mouse-quadrature-decode lesson that flipping both XOR operands leaves an
//   XOR invariant); the forward=bit9^bit8 reading stayed 0.
// MUTATION sol:E-joystick.c11+opus:E-joystick.c13 stimulus red pressed the
//   forward pin instead of the back pin (single operand only); the
//   back=bit1^bit0 reading stayed 0.
// MUTATION sol:E-joystick.c12+opus:E-joystick.c14 stimulus red pressed the
//   right pin instead of the left pin; bit 9 stayed 0.
// MUTATION sol:E-joystick.c13+opus:E-joystick.c15 stimulus red pressed the
//   left pin instead of the right pin; bit 1 stayed 0.
// MUTATION opus:E-joystick.c16 stimulus red pressed the right pin together
//   with pin 5/bit 5, so the decode DID move; the idle-vs-pressed comparison
//   this assertion rests on was no longer equal.
// MUTATION opus:8-tbl8-1.c8+sol:8-pin.c9 stimulus red MUT_N: pressed pin 4
//   instead of pin 3 for the "pin 3 alone" probe; JOY0DAT bit9 stayed 0,
//   failing the Table-8-1-graded clause.
//
// Table 8-1 (p.228), "Proportional Controller (Pair)" column, pins 3/4 --
// opus:8-tbl8-1.c8/c12, sol:8-pin.c9/c13. Pin 3 and pin 4 feed JOY0DAT<9>
// and JOY0DAT<1>, proven live by sol:E-joystick.c8+opus:E-joystick.c9 and
// by the pin-3/pin-4 composition assertions elsewhere in this file. PIN 3
// (left button): uncontested, graded. PIN 4 (right
// button): Table 8-1 and p.332's own connector figure both put it on pin 4,
// but Figure 8-4 (p.234), read directly off figures/p234.png (not the text
// layer), prints "pin 3" for the fire button under BOTH the LEFT PADDLE and
// the RIGHT PADDLE diagrams -- a genuine printed self-contradiction, not an
// extraction artifact. Recorded, not graded, both readings and what was
// measured, per the NM-HRM-062 precedent for a source that disagrees with
// itself.
// p.332's own LBUT*/RBUT* rows (sol:E-pot.c18/c19, opus:E-pot.c21/c22) stay
// with hrm/appDE-pot-counters G7's existing decline (p.332's own NOTES
// column reads "See Digital Joystick", a page-stated cross-reference, which
// is why declining there as the same fact under the joystick device name is
// sound) -- that package is out of this lot's scope and not touched here.

#include "Vuserio.h"
#include "verilated.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <cstdio>

static int g_passed = 0, g_failed = 0;

void expect(bool cond, const std::string& label) {
    if (cond) { ++g_passed; std::cout << "PASS: " << label << "\n"; }
    else      { ++g_failed; std::cout << "FAIL: " << label << "\n"; }
}
void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

static Vuserio* top;

static void tick() {
    top->clk = 0; top->eval();
    top->clk = 1; top->eval();
}
static void settle(int n = 4) { for (int i = 0; i < n; ++i) tick(); }

// Register (word) addresses, as reg_address_in[8:1] -- derived directly from
// the parameters in userio.v (userio.v:54-61), shifted right by 1 to match
// the port's bit range.
static const uint8_t JOY0DAT_ADDR = 0x00a >> 1;
static const uint8_t POTINP_ADDR  = 0x016 >> 1;

// Bit indices within a _joy1/_joy2 stimulus word (userio.v:239,252,222-227,
// 171-175; minimig.v:205-206 names the same six roles).
enum { BIT_RIGHT = 0, BIT_LEFT = 1, BIT_BACK = 2, BIT_FWD = 3, BIT_FIRE = 4, BIT_BTN2 = 5 };

// Builds a 16-bit stimulus word: idle-high (open) on every bit except the
// ones named, which are driven low (pressed). Idle must always be driven
// explicitly, never left at Verilator's undriven default (0 = pressed here).
static uint16_t stim(std::initializer_list<int> pressed) {
    uint16_t v = 0xFFFF;
    for (int b : pressed) v &= ~(uint16_t)(1u << b);
    return v;
}

static void drive_idle_and_reset() {
    top->reset = 1;
    top->clk7_en = 1;
    top->reg_address_in = 0;
    top->data_in = 0;
    top->pot_cnt_en = 0;
    top->_fire0_dat = 1;
    top->_fire1_dat = 1;
    top->_joy1 = stim({});
    top->_joy2 = stim({});
    top->joy_ana1 = 0;
    top->joy_ana2 = 0;
    top->mouse_btn = 0;
    top->kms_level = 0;
    top->kbd_mouse_type = 0;
    top->kbd_mouse_data = 0;
    settle(4);
    top->reset = 0;
    settle(4);
}

// Latches joy1enable=1 (userio.v:222-227): pulse the mode-select bit (4) of
// _joy2 low for a few cycles, then release it; the latch holds as long as
// mouse_btn[0] stays 0. Same mechanism hrm/ch8-joystick-switch-decode uses.
static void enable_joy1_mode() {
    top->_joy2 = (uint16_t)(top->_joy2 & ~(uint16_t)(1u << BIT_FIRE));
    settle(3);
    top->_joy2 = (uint16_t)(top->_joy2 | (uint16_t)(1u << BIT_FIRE));
    settle(3);
}

static uint16_t read_addr(uint8_t addr) {
    top->reg_address_in = addr;
    top->eval();
    return (uint16_t)top->data_out;
}

struct Decode { int right, left, back, forward; };
static Decode decode16(uint16_t v) {
    int b0 = (v >> 0) & 1, b1 = (v >> 1) & 1, b8 = (v >> 8) & 1, b9 = (v >> 9) & 1;
    Decode d;
    d.right   = b1;
    d.left    = b9;
    d.back    = b1 ^ b0;
    d.forward = b9 ^ b8;
    return d;
}
static int code4(const Decode& d) { return d.right * 8 + d.left * 4 + d.back * 2 + d.forward; }

static std::string hex16(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%04X", v);
    return std::string(buf);
}

// Drives one stimulus onto _joy2 (which reaches JOY0DAT) and settles long
// enough for the two-stage synchronizer and the decode's steady-state branch
// to both catch up (userio.v:214-219, 230-254).
static uint16_t joy0dat_for(uint16_t joy2_word) {
    top->_joy2 = joy2_word;
    settle(6);
    return read_addr(JOY0DAT_ADDR);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    drive_idle_and_reset();

    // ---- Harness liveness: prove the fixture can deliver two distinct known
    // words before anything reads one.
    {
        top->mouse_btn = 0;
        enable_joy1_mode();
        Decode idle_d = decode16(joy0dat_for(stim({})));
#ifdef MUT_LIVE
        Decode right_d = decode16(joy0dat_for(stim({BIT_BTN2}))); // mutated: no-effect bit
#else
        Decode right_d = decode16(joy0dat_for(stim({BIT_RIGHT})));
#endif
        note("JOY0DAT idle decode = " + std::to_string(code4(idle_d)) +
             ", right-pressed decode = " + std::to_string(code4(right_d)));
        expect(code4(idle_d) == 0 && right_d.right == 1 && code4(idle_d) != code4(right_d),
               "harness:live-known-words");
    }

    // ==================== Declined: already graded elsewhere ====================
    //
    // These are the record()s the brief's decline CONDITION asks for: state
    // WHICH prior assertion grades the SAME proposition, on the SAME axis,
    // not merely a related one.

    record("DECLINED sol:E-joystick.c1+opus:E-joystick.c2 -- \"A joystick is a "
        "device with four normally opened switches arranged 90 degrees "
        "apart.\" (p.329 prose, top of page). The switch-count/normally-open "
        "half is the SAME proposition hrm/ch8-joystick-switch-decode already "
        "grades: L01 (opus:8-joy.c1+sol:8-joy.c1, four independent, "
        "pairwise-distinct direction stimuli) and L05 (opus:8-joy.c6+"
        "sol:8-joy.c6, normally open -- idle reads not-closed on every "
        "switch), both at userio.v:239,252. \"Arranged 90 degrees apart\" is "
        "the joystick's OWN mechanical geometry -- observable at no digital "
        "signal in userio or any other module in this tree; not "
        "constructible.");

    record("DECLINED sol:E-joystick.c2+opus:E-joystick.c3 -- \"The JOY[0/1]DAT "
        "registers become encoded switch input ports.\" Same proposition as "
        "hrm/ch8-joystick-switch-decode L09 (opus:8-joy.c10+sol:8-joy.c10, "
        "one address carries two meanings gated by joy1enable, userio.v:"
        "222-227,290-293) and L10 (opus:8-joy.c11+sol:8-joy.c11, JOY0DAT and "
        "JOY1DAT are two independently addressed registers, userio.v:294-"
        "295).");

    record("DECLINED sol:E-joystick.c3+opus:E-joystick.c4 -- \"Forward: bit#9 "
        "xor bit#8\" (p.329 prose). Same proposition, same axis (function -> "
        "bit, no pin involved) as hrm/ch8-joystick-switch-decode L15 (p.232 "
        "Table 8-3, opus:8-joy.c16+sol:8-joy.c16) and L24 (p.281, "
        "opus:8-appa-joy.c90+sol:8-appa-joy.c54). RTL: userio.v:252.");

    record("DECLINED sol:E-joystick.c4+opus:E-joystick.c5 -- \"Left: bit#9\" "
        "(p.329 prose). Same proposition as hrm/ch8-joystick-switch-decode "
        "L13 (opus:8-joy.c14+sol:8-joy.c14) and L25 (opus:8-appa-joy.c91+"
        "sol:8-appa-joy.c55). RTL: userio.v:252 `{!_djoy1[1], ...}`.");

    record("DECLINED sol:E-joystick.c5+opus:E-joystick.c6 -- \"Back: bit#l "
        "xor bit#0\" (p.329 prose; the text layer's lowercase l stands for "
        "digit 1). Same proposition as hrm/ch8-joystick-switch-decode L14 "
        "(opus:8-joy.c15+sol:8-joy.c15) and L26 (opus:8-appa-joy.c92+"
        "sol:8-appa-joy.c56). RTL: userio.v:239.");

    record("DECLINED sol:E-joystick.c6+opus:E-joystick.c7 -- \"Right: bit#l\" "
        "(p.329 prose). Same proposition as hrm/ch8-joystick-switch-decode "
        "L12 (opus:8-joy.c13+sol:8-joy.c13) and L27 (opus:8-appa-joy.c93+"
        "sol:8-appa-joy.c57). RTL: userio.v:239 `{!_djoy1[0], ...}`.");

    record("DECLINED sol:E-joystick.c7+opus:E-joystick.c8 -- \"Data is "
        "encoded to facilitate the mouse/trackball operating mode.\" The "
        "testable content is that the joystick switch decode lands in the "
        "SAME word the mouse quadrature counter advances -- exactly what "
        "hrm/ch8-joystick-switch-decode L09 (opus:8-joy.c10+sol:8-joy.c10) "
        "grades. RTL: userio.v:239,252 write dmouse0dat[1:0]/[9:8], the same "
        "register the mouse counter logic above it increments/decrements.");

    record("DECLINED sol:E-joystick.c15+opus:E-joystick.c17 -- CONNECTOR PIN "
        "USAGE table, pin 6: FIRE* / \"Left mouse button\" / See Fire "
        "Button. Same proposition as hrm/ch8-controller-pin-function-map "
        "(Table 8-1, p.228: pin 6 = joystick Button 1 / left mouse button) "
        "and hrm/ch8-ciaapra-fire-buttons (the CIAAPRA side). Note what this "
        "row actually prints: pin 6's DESCRIPTION is \"Left mouse button\", "
        "not \"fire button\" -- which is why userio.v:313 ANDs the two "
        "sources onto one output, `_fire0 = ... _sjoy1[4] & _mleft`.");

    record("DECLINED opus:E-joystick.c20 -- CONNECTOR PIN USAGE table, pin 9: "
        "\"Unused\". Checked against the board wiring before accepting "
        "classify's \"status: testable\" call, and it does not hold: "
        "userio.v's ports carry only a role-named 16-bit bus (minimig.v:"
        "205-206's own port comment, \"[fire2,fire,up,down,left,right]\", "
        "names six roles at bits 5..0 and nothing else), and every board "
        "top-level file in this tree (checked: tang/nano20k/top.sv:54 "
        "`input [5:0] js1`, :89 `wire [5:0] db9_joy1 = {...}`; "
        "gatemate/evb/top.sv:59 the same) wires exactly those same six bits "
        "in from the DB9 connector -- there is no seventh wire anywhere for "
        "a \"pin 9\" signal to be. Unlike pin 5 (graded below: bit 5 is "
        "userio's own \"second button\" line, independently identified by "
        "its effect on POTINP and already graded there by hrm/ch8-joystick-"
        "switch-decode L17), nothing in this tree ties any bit of the "
        "16-bit word to pin 9 specifically -- picking bit 9 to stand for it "
        "would be inventing a correspondence from the two numerals matching "
        "alone. Not constructible at userio without that invention.");

    // ==================== Graded here ====================

    // sol:E-joystick.c8+opus:E-joystick.c9 -- NOTE: "The right and left
    // direction inputs are also designed to be right and left buttons,
    // respectively, for use with proportional inputs. In this case, the
    // forward and back inputs are not used, while right and left become
    // button inputs rather than joystick inputs." (p.329, the NOTE)
    //
    // Graded against p.332's CONNECTOR PIN USAGE FOR PROPORTIONAL INPUTS
    // table, which gives pin 3 (LBUT*, Left button) and pin 4 (RBUT*, Right
    // button) the same register note: "See Digital Joystick" -- p.329's own
    // table sends those same two pins to JOY[0/1]DAT<9> and JOY[0/1]DAT<1>
    // alone, no companion bit. The button reading the NOTE describes is
    // therefore that same single bit: pin 3 alone sets bit 9, with bit 9
    // xor bit 8 staying 0 (the forward input the NOTE says goes unused);
    // pin 4 alone sets bit 1, with bit 1 xor bit 0 staying 0 (back,
    // likewise unused). POTINP bit 10 is read alongside as a control, not a
    // graded clause: no page sends pin 3 or pin 4 there, so it is expected
    // to stay put.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
        settle(6);

        uint16_t idle_potinp = read_addr(POTINP_ADDR);
        int idle_btn = (idle_potinp >> 10) & 1;

#ifdef MUT_H
        uint16_t left_word = joy0dat_for(stim({BIT_RIGHT})); // mutated: wrong pin
#else
        uint16_t left_word = joy0dat_for(stim({BIT_LEFT}));
#endif
        Decode left_d = decode16(left_word);
        int left_btn = (read_addr(POTINP_ADDR) >> 10) & 1;

        uint16_t right_word = joy0dat_for(stim({BIT_RIGHT}));
        Decode right_d = decode16(right_word);
        int right_btn = (read_addr(POTINP_ADDR) >> 10) & 1;

        note("p.332 grades pin 3/pin 4 (LBUT*/RBUT*) as \"See Digital "
             "Joystick\": pin 3 alone -> JOY0DAT bit9=" +
             std::to_string(left_d.left) + ", bit9 xor bit8=" +
             std::to_string(left_d.forward) + "; pin 4 alone -> JOY0DAT "
             "bit1=" + std::to_string(right_d.right) + ", bit1 xor bit0=" +
             std::to_string(right_d.back) + "; POTINP bit10 (control) idle=" +
             std::to_string(idle_btn) + " pin3=" + std::to_string(left_btn) +
             " pin4=" + std::to_string(right_btn));

        expect(left_d.left == 1 && left_d.forward == 0 &&
               right_d.right == 1 && right_d.back == 0 &&
               left_btn == idle_btn && right_btn == idle_btn,
               "sol:E-joystick.c8+opus:E-joystick.c9");
    }

    // opus:8-tbl8-1.c8+sol:8-pin.c9 / opus:8-tbl8-1.c12+sol:8-pin.c13 --
    // Table 8-1 (p.228), "Proportional Controller (Pair)" column: pin 3 =
    // "Left button", pin 4 = "Right button".
    // PIN 3 is uncontested (Figure 8-4, p.234, also draws the LEFT
    // paddle's fire button on pin 3) and is graded. PIN 4 is CONTESTED:
    // Table 8-1 and p.332 put the right paddle's button on pin 4, but
    // Figure 8-4 prints "pin 3" under the RIGHT PADDLE diagram too (verified
    // against figures/p234.png directly, not the text layer) -- grading
    // either side would pick a winner this suite has no ground to pick, so
    // it is recorded, both readings and what was measured, per the
    // NM-HRM-062 precedent.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();

#ifdef MUT_N
        uint16_t p3_word = joy0dat_for(stim({BIT_RIGHT})); // mutated: wrong pin
#else
        uint16_t p3_word = joy0dat_for(stim({BIT_LEFT}));  // pin 3 alone
#endif
        Decode p3_d = decode16(p3_word);

        uint16_t p4_word = joy0dat_for(stim({BIT_RIGHT})); // pin 4 alone
        Decode p4_d = decode16(p4_word);

        expect(p3_d.left == 1,
            "opus:8-tbl8-1.c8+sol:8-pin.c9 -- Table 8-1 (p.228), "
            "Proportional Controller (Pair) column, pin 3 = \"Left button\"; "
            "Figure 8-4 (p.234, \"Typical Paddle Wiring Diagram\") draws the "
            "LEFT PADDLE's fire button on pin 3 too, and p.234's own prose "
            "agrees (\"For the paddle controllers, the left and right "
            "joystick direction lines serve as the fire buttons for the "
            "left and right paddles.\"); pin 3 alone reads JOY0DAT bit 9 = 1 "
            "(Table 8-3, p.232: bit 9 = left).");

        record(
            "opus:8-tbl8-1.c12+sol:8-pin.c13 -- Table 8-1 (p.228), "
            "Proportional Controller (Pair) column, pin 4 = \"Right button\" "
            "(p.332's own connector figure agrees: RBUT* on pin 4, \"See "
            "Digital Joystick\"). p.234's own prose, under the figure, sides "
            "with Table 8-1: \"For the paddle controllers, the left and "
            "right joystick direction lines serve as the fire buttons for "
            "the left and right paddles.\" NOT GRADED: Figure 8-4 (p.234, "
            "\"Typical Paddle Wiring Diagram\"), read directly off "
            "figures/p234.png, prints \"pin 3\" for the fire button under "
            "BOTH the LEFT PADDLE and the RIGHT PADDLE diagrams -- not a "
            "text-layer artifact, confirmed at the image. The same figure's "
            "RIGHT PADDLE diagram also repeats the LEFT paddle's pin 9 for "
            "the pot, where Table 8-1 prints \"right POT\" on pin 5 -- a "
            "second, independent point where Figure 8-4's right-paddle "
            "wiring does not match Table 8-1. Table 8-1/p.332 and Figure "
            "8-4 disagree on which pin carries the right paddle's button; "
            "this package does not adjudicate which printed source is "
            "right, per the NM-HRM-062 precedent for a source that "
            "contradicts itself. MEASURED, both readings: pin 3 alone -> "
            "JOY0DAT bit9=" + std::to_string(p3_d.left) + "; pin 4 alone -> "
            "JOY0DAT bit1=" + std::to_string(p4_d.right) + " -- both pins "
            "are independently live and distinct, so a future reader "
            "adjudicating the print can rely on this measurement without "
            "re-deriving it. Already catalogued, not freshly found: "
            "integration/ch8-controller-ports/SPECIFICATION.md, "
            "\"Disagreements between pages\" / P4, reaches the same "
            "Figure-8-4-versus-Table-8-1 disagreement over the same two "
            "claim ids from its own system-level boundary and grades them "
            "only in the narrower agreed form (a closed switch on pin 4 "
            "reads at X1 -- p.281, p.232 Table 8-3 and Figure 8-3 agree on "
            "that much). This package, built only at userio.v with no "
            "system-level corroboration to narrow the claim the same way, "
            "takes the more conservative reading and records rather than "
            "narrows -- a difference in how cautious each boundary can "
            "afford to be, not a contradiction: neither package asserts "
            "\"pin 4 is the right paddle's own button\" as such.");
    }

    // sol:E-joystick.c9+opus:E-joystick.c10 -- "The JOY[0/1]DAT registers are
    // always valid and may be read at any time." data_out is a pure
    // combinational mux (userio.v:289-310) over registers that update every
    // clk7_en (userio.v:239,252) -- so "always valid, any time" means: no
    // settle needed between back-to-back reads, no decay after many idle
    // cycles, and no disturbance from reading a different register in
    // between.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
#ifdef MUT_I
        top->_joy2 = stim({BIT_LEFT}); // mutated: wrong pin
#else
        top->_joy2 = stim({BIT_RIGHT});
#endif
        settle(6);

        uint16_t first_read  = read_addr(JOY0DAT_ADDR);
        uint16_t second_read = read_addr(JOY0DAT_ADDR);      // no settle between reads
        (void)read_addr(POTINP_ADDR);                        // read a different register
        uint16_t third_read  = read_addr(JOY0DAT_ADDR);       // and come back
        settle(40);                                           // arbitrarily later
        uint16_t fourth_read = read_addr(JOY0DAT_ADDR);

        Decode d = decode16(first_read);
        note("JOY0DAT, right pressed, read at four different times = " +
             hex16(first_read) + ", " + hex16(second_read) + ", " +
             hex16(third_read) + ", " + hex16(fourth_read));
        expect(first_read == second_read && second_read == third_read &&
               third_read == fourth_read && d.right == 1,
               "sol:E-joystick.c9+opus:E-joystick.c10");
    }

    // ==================== Pin -> register-field composition ====================
    //
    // p.329's CONNECTOR PIN USAGE FOR DIGITAL JOYSTICK INPUTS table prints
    // PIN -> REGISTER FIELD, not function -> bit. hrm/ch8-controller-pin-
    // function-map grades pin -> function (e.g. "pin 1 = Forward"); hrm/
    // ch8-joystick-switch-decode grades function -> bit ("Forward = bit9 xor
    // bit8"). Neither grades their COMPOSITION, and the lot-F classifier
    // found nothing else that does. Read from figures/p329.png, not the
    // displaced text-layer table (appE/corpus-defects.md), and confirmed:
    // the render binds pins 1-4 to <9 xor 8>, <1 xor 0>, <9>, <1>, matching
    // the prose.

    // sol:E-joystick.c10+opus:E-joystick.c12 -- pin 1, FORWARD* ->
    // JOY[0/1]DAT<9 xor 8>.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
#ifdef MUT_J
        uint16_t w = joy0dat_for(stim({BIT_LEFT})); // mutated: wrong pin, single operand
#else
        uint16_t w = joy0dat_for(stim({BIT_FWD}));
#endif
        int fwd = ((w >> 9) & 1) ^ ((w >> 8) & 1);
        expect(fwd == 1, "sol:E-joystick.c10+opus:E-joystick.c12");
    }

    // sol:E-joystick.c11+opus:E-joystick.c13 -- pin 2, BACK* -> printed as
    // "JOY[0/1]DAT(1 xor 0>". GLYPH, KEPT AS PRINTED: the book opens this
    // bit range with a ROUND PARENTHESIS, not a chevron -- the same defect,
    // same row position, sits at p.328's own pin 2 (appE/corpus-defects.md,
    // verified at 600 dpi, confirmed legible at 200 dpi on both
    // figures/p328.png and figures/p329.png). Do not silently normalise it
    // to <1 xor 0> -- two records in the first reading did exactly that and
    // it altered the evidence. The bit range under test, <1 xor 0>, is the
    // same regardless of which glyph opens it.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
#ifdef MUT_K
        uint16_t w = joy0dat_for(stim({BIT_FWD})); // mutated: wrong pin, single operand
#else
        uint16_t w = joy0dat_for(stim({BIT_BACK}));
#endif
        int back = ((w >> 1) & 1) ^ (w & 1);
        expect(back == 1, "sol:E-joystick.c11+opus:E-joystick.c13");
    }

    // sol:E-joystick.c12+opus:E-joystick.c14 -- pin 3, LEFT* ->
    // JOY[0/1]DAT<9>.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
#ifdef MUT_L
        uint16_t w = joy0dat_for(stim({BIT_RIGHT})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_LEFT}));
#endif
        expect(((w >> 9) & 1) == 1, "sol:E-joystick.c12+opus:E-joystick.c14");
    }

    // sol:E-joystick.c13+opus:E-joystick.c15 -- pin 4, RIGHT* ->
    // JOY[0/1]DAT<1>.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
#ifdef MUT_M
        uint16_t w = joy0dat_for(stim({BIT_LEFT})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_RIGHT}));
#endif
        expect(((w >> 1) & 1) == 1, "sol:E-joystick.c13+opus:E-joystick.c15");
    }

    // opus:E-joystick.c16 -- pin 5 "Unused" (p.329's CONNECTOR PIN USAGE
    // table, scoped to DIGITAL JOYSTICK INPUTS -- its own heading). Bit 5 is
    // not silent everywhere: it is userio's own "second button" line, and
    // hrm/ch8-joystick-switch-decode L17 already grades its effect on
    // POTINP (sourced from p.233, not this page). What THIS page states is
    // narrower and still true to check: pin 5 plays no part in the
    // JOY[0/1]DAT switch decode this table describes.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
        Decode idle = decode16(joy0dat_for(stim({})));
#ifdef MUT_O
        Decode d = decode16(joy0dat_for(stim({BIT_BTN2, BIT_RIGHT}))); // mutated: also pressed right
#else
        Decode d = decode16(joy0dat_for(stim({BIT_BTN2})));
#endif
        expect(code4(d) == code4(idle), "opus:E-joystick.c16");
    }

    std::cout << g_passed << " passed, " << g_failed << " failed\n";

    delete top;
    return g_failed == 0 ? 0 : 1;
}
