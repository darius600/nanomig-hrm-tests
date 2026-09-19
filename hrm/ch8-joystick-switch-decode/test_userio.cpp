// Test purpose: how the joystick direction switches decode into JOYxDAT, at
//   the `userio` boundary. Claims graded here:
//   ch8-s1-build/ch8-joystick-switch-decode.jsonl (52 claims, pages 232,
//   233, 281), grouped into 27 propositions per SPECIFICATION.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 232, 233
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim outside the 52 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 52 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// ch8-joystick-switch-decode -- fixture: userio.v, compiled alone.
//
// Claims graded here: ch8-s1-build/ch8-joystick-switch-decode.jsonl (52 claims,
// pages 232, 233, 281), grouped into 27 propositions per SPECIFICATION.md. Every
// uid ends up on a graded expect() line -- no claim in this file is declined, so
// there is no record() call in this build (the fire-button claims the build brief
// warned about do not appear among these 52; checked before writing the spec).
//
// RTL notes discovered in step 2 (after SPECIFICATION.md was committed), needed to
// read this file:
//   - joy_swap is a hardwired constant 0 inside userio.v (userio.v:92). With it 0,
//     `_sjoy1 <= joy_swap ? joy1 : joy2` resolves to `_sjoy1 <= joy2`, i.e. JOY0DAT's
//     switch decode (dmouse0dat, via _djoy1) is driven by the `_joy2` port, and
//     JOY1DAT's decode (dmouse1dat, via _djoy2) is driven by the `_joy1` port
//     (userio.v:210-219). This is a wiring fact about the fixture, not a claim from
//     any of the 52 -- it only affects which C++ variable this file must poke to
//     reach which register, and is called out here so that choice does not look
//     arbitrary.
//   - Within either 16-bit joystick word, bit0/bit2 are the X-axis pair (right/back)
//     and bit1/bit3 are the Y-axis pair (left/forward), read out as
//     dmouseNdat[1:0] <= {!x0, x0^x2} and dmouseNdat[9:8] <= {!y1, y1^y3}
//     (userio.v:239, 252, 266, 279). bit4 is the per-port mode-select ("fire") line
//     (userio.v:222-227) and bit5 is the second button (userio.v:174, 167).
//   - JOY0DAT (address userio.v:54) only shows the switch decode while joy1enable is
//     latched 1 (userio.v:290-293); JOY1DAT (userio.v:55) shows it unconditionally
//     (userio.v:294-295). joy1enable is a level latch (userio.v:222-227): pulsing
//     the mode-select line low for a few cycles and releasing it leaves joy1enable
//     at 1, as long as mouse_btn[0] (left mouse button) stays released.
//   - A switch's raw pin defaults to Verilator's undriven value (0) if this harness
//     ever forgets to drive it -- and 0 is the *pressed* level on this active-low
//     interface, not idle. A sibling builder on this fixture already measured what
//     that mistake looks like: JOY0DAT/JOY1DAT read $0202 (bits 1 and 9 spuriously
//     set), not $0000. Every stimulus below drives every relevant bit explicitly,
//     every cycle, specifically to avoid reproducing that.
//
// MUTATION records (the exact second token is the label passed to expect() below):
//
// MUTATION harness:live-known-words stimulus red drove the "right" case with the
//   second-button bit (5, no effect on the decode) instead of the right pin (0); the
//   two delivered words collapsed to the same value.
// MUTATION harness:joy1enable-precondition stimulus red never pulsed the mode-select
//   line low, so joy1enable never latched to 1 and JOY0DAT stayed in mouse-counter
//   mode.
// MUTATION opus:8-joy.c1+sol:8-joy.c1 stimulus red drove the "back" case from bit 5
//   (second button, no effect on the X/Y decode) instead of bit 2; the four
//   single-switch results were no longer pairwise distinct.
// MUTATION opus:8-joy.c2+sol:8-joy.c2 stimulus red while driving "right" alone, also
//   drove the back pin low; more than one direction came out asserted.
// MUTATION opus:8-joy.c3+sol:8-joy.c3 stimulus red dropped the forward pin from the
//   forward+right diagonal stimulus, driving right alone.
// MUTATION opus:8-joy.c4+sol:8-joy.c4 stimulus red the per-axis encode being a
//   bijection (bit1=!x0, bit0=x0^x2; bit9=!y1, bit8=y1^y3, userio.v:239, 252) says
//   no two of the 8 *driven patterns* can decode alike -- it says nothing about the
//   8 *stimuli* the test itself chooses, which are this assertion's own data and are
//   exactly as mutable as any other stimulus in this file. Replaced the eighth
//   stimulus (left+forward diagonal) with a duplicate of the first (right alone):
//   codes[0] and codes[7] both decode to 8, all_distinct goes false, and 29
//   passed/0 failed -> 28 passed/1 failed with only this assertion newly red.
//   Reverted (`make clean && make`): 29 passed/0 failed restored.
// MUTATION opus:8-joy.c6+sol:8-joy.c6 stimulus red drove the right pin low ("pressed")
//   while calling it the idle/open baseline; the idle reading was no longer all-zero.
// MUTATION opus:8-joy.c7+sol:8-joy.c7 stimulus red inverted the "press" stimulus on the
//   right pin (drove it high instead of low); the closed reading never appeared.
// MUTATION opus:8-joy.c8+sol:8-joy.c8 stimulus red same shape as c6/c7's: drove the
//   left pin low at what is claimed to be the idle/open baseline.
// MUTATION opus:8-joy.c9+sol:8-joy.c9 stimulus red pressed the right pin instead of the
//   left pin while checking the left decode; left never read closed (1).
// MUTATION opus:8-joy.c10+sol:8-joy.c10 stimulus red never pulsed the mode-select line;
//   JOY0DAT stayed in mouse-counter (single) meaning throughout, so the "two
//   meanings" comparison had nothing to compare.
// MUTATION opus:8-joy.c11+sol:8-joy.c11 stimulus red read JOY0DAT's address a second
//   time and compared it against itself instead of reading JOY1DAT's own address;
//   the two "registers" were no longer independent by construction.
// MUTATION opus:8-joy.c12+sol:8-joy.c12 stimulus red pressed the forward pin instead of
//   the back pin while checking the back decode; back never read closed (1).
// MUTATION opus:8-joy.c13+sol:8-joy.c13 stimulus red pressed the left pin instead of
//   the right pin while reading register bit 1; bit 1 stayed 0.
// MUTATION opus:8-joy.c14+sol:8-joy.c14 stimulus red pressed the right pin instead of
//   the left pin while reading register bit 9; bit 9 stayed 0.
// MUTATION opus:8-joy.c15+sol:8-joy.c15 stimulus red pressed the right pin instead of
//   the back pin (single operand only, per the mouse-quadrature-decode lesson about
//   flipping both XOR operands); the back=bit1^bit0 reading stayed 0 instead of 1.
// MUTATION opus:8-joy.c16+sol:8-joy.c16 stimulus red pressed the left pin instead of
//   the forward pin (single operand only); the forward=bit9^bit8 reading stayed 0.
// MUTATION opus:8-joy.c17 stimulus red drove the _joy2 lines (which reach JOY0DAT, not
//   JOY1DAT) while asserting a right-press should show up on JOY1DAT; JOY1DAT stayed
//   idle.
// MUTATION opus:8-joy2.c5+sol:8-joy.c21 stimulus red drove the mode-select bit (4)
//   instead of the second-button bit (5) as the joystick-side stimulus; it produced
//   no effect on POTINP bit 10, breaking the claimed equivalence with the mouse
//   button.
// MUTATION opus:8-appa-joy.c15+sol:8-appa-joy.c17 stimulus red swapped which pin
//   supplies x0 vs x2 (drove the back pin instead of the right pin for the "right
//   pressed" case); the recovered (bit1,bit0) pair no longer matched the pin state
//   that was actually driven.
// MUTATION opus:8-appa-joy.c16+sol:8-appa-joy.c18 stimulus red drove the back pin (x2)
//   instead of the right pin (x0) for the same stimulus that L18/L22 use; the
//   Table-8-3 "right" reading built from those bits no longer agreed with what was
//   actually pressed.
// MUTATION opus:8-appa-joy.c53+sol:8-appa-joy.c26 stimulus red inverted the forward
//   pin's press level (drove it high instead of low); the active-low convention's
//   closed reading never appeared.
// MUTATION opus:8-appa-joy.c87+sol:8-appa-joy.c52 stimulus red pressed the forward pin
//   instead of the left pin while reading bit 9 (Y1); bit 9 stayed 0.
// MUTATION opus:8-appa-joy.c88+sol:8-appa-joy.c53 stimulus red pressed the back pin
//   instead of the right pin while reading bit 1 (X1); bit 1 stayed 0.
// MUTATION opus:8-appa-joy.c89 stimulus red performed the "bit1 alone / bit1^bit0"
//   comparison while pressing forward instead of back; neither the direct bit nor
//   the XOR reflected the intended switch.
// MUTATION opus:8-appa-joy.c90+sol:8-appa-joy.c54 stimulus red pressed the left pin
//   instead of the forward pin (single operand); Y1 xor Y0 stayed 0 instead of 1.
// MUTATION opus:8-appa-joy.c91+sol:8-appa-joy.c55 stimulus red pressed the back pin
//   instead of the left pin while reading Y1; Y1 stayed 0.
// MUTATION opus:8-appa-joy.c92+sol:8-appa-joy.c56 stimulus red pressed the forward pin
//   instead of the back pin (single operand); X1 xor X0 stayed 0 instead of 1.
// MUTATION opus:8-appa-joy.c93+sol:8-appa-joy.c57 stimulus red pressed the left pin
//   instead of the right pin while reading X1; X1 stayed 0.

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

// Register (word) addresses, as reg_address_in[8:1] -- derived directly from the
// parameters in userio.v (userio.v:54-61), shifted right by 1 to match the port's
// bit range.
static const uint8_t JOY0DAT_ADDR = 0x00a >> 1;
static const uint8_t JOY1DAT_ADDR = 0x00c >> 1;
static const uint8_t POTINP_ADDR  = 0x016 >> 1;

// Bit indices within a _joy1/_joy2 stimulus word (userio.v:239,252,266,279,222-227,174).
enum { BIT_RIGHT = 0, BIT_LEFT = 1, BIT_BACK = 2, BIT_FWD = 3, BIT_FIRE = 4, BIT_BTN2 = 5 };

// Builds a 16-bit stimulus word: idle-high (open) on every bit except the ones named,
// which are driven low (pressed). Idle must always be driven explicitly, never left
// at Verilator's undriven default -- see the file header.
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

// Latches joy1enable=1 (userio.v:222-227): pulse the mode-select bit (4) of _joy2 low
// for a few cycles, then release it; the latch holds as long as mouse_btn[0] stays 0.
#ifdef MUT_PRECOND
static void enable_joy1_mode() { /* mutated: never pulse the mode-select line */ settle(6); }
#else
static void enable_joy1_mode() {
    top->_joy2 = (uint16_t)(top->_joy2 & ~(uint16_t)(1u << BIT_FIRE));
    settle(3);
    top->_joy2 = (uint16_t)(top->_joy2 | (uint16_t)(1u << BIT_FIRE));
    settle(3);
}
#endif

static uint16_t read_addr(uint8_t addr) {
    top->reg_address_in = addr;
    top->eval();
    return (uint16_t)top->data_out;
}

struct Decode { int right, left, back, forward; };
static Decode decode16(uint16_t v) {
    int b0 = (v >> 0) & 1, b1 = (v >> 1) & 1, b8 = (v >> 8) & 1, b9 = (v >> 9) & 1;
    Decode d;
    d.right   = b1;      // Table 8-3: bit 1 = right (direct)
    d.left    = b9;      // Table 8-3: bit 9 = left (direct)
    d.back    = b1 ^ b0; // Table 8-3: back = bit1 XOR bit0
    d.forward = b9 ^ b8; // Table 8-3: forward = bit9 XOR bit8
    return d;
}
// A 4-bit summary of a Decode, used only to compare direction *patterns* for
// distinctness. dmouse0dat[7:2] and [15:10] are a leftover mouse-quadrature
// accumulator (userio.v:234-237, 247-250) that the steady-state decode branch
// never clears -- it only ever (over)writes bits [1:0]/[9:8] (userio.v:239,252) --
// so it carries noise forward from every earlier stimulus transition in this run.
// None of the 52 claims say anything about those bits, so comparisons below use
// this code (or decode16() itself), never the raw 16-bit word.
static int code4(const Decode& d) { return d.right * 8 + d.left * 4 + d.back * 2 + d.forward; }

static std::string hex16(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%04X", v);
    return std::string(buf);
}

// Drives one stimulus onto _joy2 (which reaches JOY0DAT, per the file header) and
// settles long enough for the two-stage synchronizer (_sjoy1, then _djoy1) and the
// decode's steady-state ("else") branch to both catch up (userio.v:214-219, 230-254).
static uint16_t joy0dat_for(uint16_t joy2_word) {
    top->_joy2 = joy2_word;
    settle(6);
    return read_addr(JOY0DAT_ADDR);
}
static uint16_t joy1dat_for(uint16_t joy1_word) {
    top->_joy1 = joy1_word;
    settle(6);
    return read_addr(JOY1DAT_ADDR);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    drive_idle_and_reset();

    // ---- Harness liveness: prove the fixture can deliver two distinct known words
    // before anything reads one (chapter 6 lost a harness to a silently dead path).
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

    // ---- Precondition (L09's mechanism): joy1enable must actually be settable.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
        uint16_t w = joy0dat_for(stim({BIT_RIGHT}));
        Decode d = decode16(w);
        expect(d.right == 1, "harness:joy1enable-precondition");
    }

    // ==================== Page 232 ====================

    drive_idle_and_reset();
    top->mouse_btn = 0;
    enable_joy1_mode();

    // L01 opus:8-joy.c1+sol:8-joy.c1 -- four directional switches: four distinct,
    // independently-controllable single-switch results.
    {
        int r = code4(decode16(joy0dat_for(stim({BIT_RIGHT}))));
        int l = code4(decode16(joy0dat_for(stim({BIT_LEFT}))));
#ifdef MUT_L01
        int b = code4(decode16(joy0dat_for(stim({BIT_BTN2})))); // mutated: wrong pin, no effect
#else
        int b = code4(decode16(joy0dat_for(stim({BIT_BACK}))));
#endif
        int f = code4(decode16(joy0dat_for(stim({BIT_FWD}))));
        expect(r != 0 && l != 0 && b != 0 && f != 0 &&
               r != l && r != b && r != f && l != b && l != f && b != f,
               "opus:8-joy.c1+sol:8-joy.c1");
    }

    // L02 opus:8-joy.c2+sol:8-joy.c2 -- each switch activates on its own: pressing one
    // asserts only that direction.
    {
        Decode r = decode16(joy0dat_for(stim({BIT_RIGHT})));
#ifdef MUT_L02
        Decode l = decode16(joy0dat_for(stim({BIT_LEFT, BIT_BACK}))); // mutated: extra press
#else
        Decode l = decode16(joy0dat_for(stim({BIT_LEFT})));
#endif
        Decode b = decode16(joy0dat_for(stim({BIT_BACK})));
        Decode f = decode16(joy0dat_for(stim({BIT_FWD})));
        expect(r.right == 1 && r.left == 0 && r.back == 0 && r.forward == 0 &&
               l.left == 1  && l.right == 0 && l.back == 0 && l.forward == 0 &&
               b.back == 1  && b.right == 0 && b.left == 0 && b.forward == 0 &&
               f.forward == 1 && f.right == 0 && f.left == 0 && f.back == 0,
               "opus:8-joy.c2+sol:8-joy.c2");
    }

    // L03 opus:8-joy.c3+sol:8-joy.c3 -- diagonal (forward+right, adjacent) activates
    // both switches at once.
    {
#ifdef MUT_L03
        Decode d = decode16(joy0dat_for(stim({BIT_RIGHT}))); // mutated: dropped forward
#else
        Decode d = decode16(joy0dat_for(stim({BIT_RIGHT, BIT_FWD})));
#endif
        expect(d.right == 1 && d.forward == 1 && d.back == 0 && d.left == 0,
               "opus:8-joy.c3+sol:8-joy.c3");
    }

    // L04 opus:8-joy.c4+sol:8-joy.c4 -- 8 total directions: the 4 cardinals and 4
    // adjacent diagonals give 8 pairwise-distinct decode patterns.
    {
        int codes[8] = {
            code4(decode16(joy0dat_for(stim({BIT_RIGHT})))),
            code4(decode16(joy0dat_for(stim({BIT_LEFT})))),
            code4(decode16(joy0dat_for(stim({BIT_BACK})))),
            code4(decode16(joy0dat_for(stim({BIT_FWD})))),
            code4(decode16(joy0dat_for(stim({BIT_FWD, BIT_RIGHT})))),
            code4(decode16(joy0dat_for(stim({BIT_RIGHT, BIT_BACK})))),
            code4(decode16(joy0dat_for(stim({BIT_BACK, BIT_LEFT})))),
#ifdef MUT_L04
            code4(decode16(joy0dat_for(stim({BIT_RIGHT})))), // mutated: duplicate of the first stimulus, not the left+forward diagonal
#else
            code4(decode16(joy0dat_for(stim({BIT_LEFT, BIT_FWD})))),
#endif
        };
        bool all_distinct = true;
        for (int i = 0; i < 8 && all_distinct; ++i)
            for (int j = i + 1; j < 8; ++j)
                if (codes[i] == codes[j]) { all_distinct = false; break; }
        expect(all_distinct, "opus:8-joy.c4+sol:8-joy.c4");
    }

    // L05 opus:8-joy.c6+sol:8-joy.c6 -- normally open: nothing driven low reads
    // not-closed (0) on every switch.
    {
#ifdef MUT_L05
        Decode d = decode16(joy0dat_for(stim({BIT_RIGHT}))); // mutated: right held pressed at "idle"
#else
        Decode d = decode16(joy0dat_for(stim({})));
#endif
        expect(d.right == 0 && d.left == 0 && d.back == 0 && d.forward == 0,
               "opus:8-joy.c6+sol:8-joy.c6");
    }

    // L06 opus:8-joy.c7+sol:8-joy.c7 -- pressing shorts the line to ground: it is
    // specifically the low level that produces a closed (1) reading.
    {
        int idle_right = decode16(joy0dat_for(stim({}))).right;
#ifdef MUT_L06
        // mutated: drive the "press" stimulus high (i.e. do not press) instead of low
        int pressed_right = decode16(joy0dat_for(stim({}))).right;
#else
        int pressed_right = decode16(joy0dat_for(stim({BIT_RIGHT}))).right;
#endif
        expect(idle_right == 0 && pressed_right == 1, "opus:8-joy.c7+sol:8-joy.c7");
    }

    // L07 opus:8-joy.c8+sol:8-joy.c8 -- open reads 1 at the switch's own input line:
    // the same idle-high convention L05/L06 rest on, read here for right and left.
    {
#ifdef MUT_L07
        Decode d = decode16(joy0dat_for(stim({BIT_LEFT}))); // mutated: left held pressed at "idle"
#else
        Decode d = decode16(joy0dat_for(stim({})));
#endif
        expect(d.right == 0 && d.left == 0, "opus:8-joy.c8+sol:8-joy.c8");
    }

    // L08 opus:8-joy.c9+sol:8-joy.c9 -- closed reads 0 at the input line, i.e. produces
    // the register's closed(1) reading -- checked here on left.
    {
#ifdef MUT_L08
        Decode d = decode16(joy0dat_for(stim({BIT_RIGHT}))); // mutated: wrong pin pressed
#else
        Decode d = decode16(joy0dat_for(stim({BIT_LEFT})));
#endif
        expect(d.left == 1, "opus:8-joy.c9+sol:8-joy.c9");
    }

    // L09 opus:8-joy.c10+sol:8-joy.c10 -- one register, two meanings: the same address
    // (JOY0DAT) composes its low two bits differently depending on joy1enable.
    //
    // With the raw mouse counter (mouse0dat) at 0, both mux branches (userio.v:290-293)
    // reduce to the same value for every bit (adding 0 is a no-op) -- toggling
    // joy1enable alone cannot show "two meanings" without ever exercising the mouse
    // side of the shared register. So this injects a known, small mouse count through
    // the keyboard-mouse path (userio.v:330-354) whose low two bits are nonzero (1):
    // the mouse-mode formula adds it into the low byte, the joystick-mode formula does
    // not (it substitutes the switch decode there directly), so the two readings are
    // provably different in bits[1:0] specifically, regardless of whatever
    // accumulator noise sits in bits[7:2] elsewhere in the byte.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        // dmouse0dat[1:0] settles to {!x0,x0^x2} = {1,1} = 3 (right decode: bit1 set
        // directly, bit0 = x0^x2 = 0^1 = 1, per userio.v:239).
        top->_joy2 = stim({BIT_RIGHT});
        settle(6);

        top->kbd_mouse_type = 0;
        top->kbd_mouse_data = 1; // xcount += 1 once, on the kms_level edge below
        top->kms_level = 1;
        tick();
        top->kbd_mouse_data = 0;
        settle(4);

        int before_low2 = read_addr(JOY0DAT_ADDR) & 0x3; // joy1enable still 0: mouse meaning
#ifdef MUT_L09
        // mutated: never enable joystick mode
        settle(6);
#else
        enable_joy1_mode();
#endif
        int after_low2 = read_addr(JOY0DAT_ADDR) & 0x3;
        note("JOY0DAT low 2 bits before enabling joystick mode = " + std::to_string(before_low2) +
             ", after = " + std::to_string(after_low2));
        // mouse meaning: (injected 1 + decode 3) mod 4 = 0; joystick meaning: decode 3 directly.
        expect(before_low2 == 0 && after_low2 == 3, "opus:8-joy.c10+sol:8-joy.c10");
        enable_joy1_mode(); // restore for the assertions that follow
    }

    // L10 opus:8-joy.c11+sol:8-joy.c11 -- JOY0DAT and JOY1DAT are two independently
    // addressed registers: a right-press wired only to JOY0DAT's controlling pins
    // shows there and not on JOY1DAT.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        enable_joy1_mode();
        top->_joy2 = stim({BIT_RIGHT});
        top->_joy1 = stim({});
        settle(6);
        uint16_t j0 = read_addr(JOY0DAT_ADDR);
#ifdef MUT_L10
        uint16_t j1 = read_addr(JOY0DAT_ADDR); // mutated: read JOY0DAT again, not JOY1DAT
#else
        uint16_t j1 = read_addr(JOY1DAT_ADDR);
#endif
        expect(decode16(j0).right == 1 && decode16(j1).right == 0, "opus:8-joy.c11+sol:8-joy.c11");
    }

    // L11 opus:8-joy.c12+sol:8-joy.c12 -- true logic state 1=closed, at the register
    // level: checked here on back.
    {
#ifdef MUT_L11
        Decode d = decode16(joy0dat_for(stim({BIT_FWD}))); // mutated: wrong pin
#else
        Decode d = decode16(joy0dat_for(stim({BIT_BACK})));
#endif
        expect(d.back == 1, "opus:8-joy.c12+sol:8-joy.c12");
    }

    // L12 opus:8-joy.c13+sol:8-joy.c13 -- Table 8-3: bit 1 = right.
    {
#ifdef MUT_L12
        uint16_t w = joy0dat_for(stim({BIT_LEFT})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_RIGHT}));
#endif
        expect(((w >> 1) & 1) == 1, "opus:8-joy.c13+sol:8-joy.c13");
    }

    // L13 opus:8-joy.c14+sol:8-joy.c14 -- Table 8-3: bit 9 = left.
    {
#ifdef MUT_L13
        uint16_t w = joy0dat_for(stim({BIT_RIGHT})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_LEFT}));
#endif
        expect(((w >> 9) & 1) == 1, "opus:8-joy.c14+sol:8-joy.c14");
    }

    // L14 opus:8-joy.c15+sol:8-joy.c15 -- Table 8-3: back = bit1 XOR bit0. Mutation
    // flips only the pressed pin (single operand), per the mouse-quadrature-decode
    // lesson that flipping both XOR operands leaves the XOR invariant.
    {
#ifdef MUT_L14
        uint16_t w = joy0dat_for(stim({BIT_RIGHT})); // mutated: single operand only
#else
        uint16_t w = joy0dat_for(stim({BIT_BACK}));
#endif
        int back = ((w >> 1) & 1) ^ (w & 1);
        expect(back == 1, "opus:8-joy.c15+sol:8-joy.c15");
    }

    // L15 opus:8-joy.c16+sol:8-joy.c16 -- Table 8-3: forward = bit9 XOR bit8.
    {
#ifdef MUT_L15
        uint16_t w = joy0dat_for(stim({BIT_LEFT})); // mutated: single operand only
#else
        uint16_t w = joy0dat_for(stim({BIT_FWD}));
#endif
        int fwd = ((w >> 9) & 1) ^ ((w >> 8) & 1);
        expect(fwd == 1, "opus:8-joy.c16+sol:8-joy.c16");
    }

    // L16 opus:8-joy.c17 -- Table 8-3's title covers both registers: the same
    // right=bit1 fact holds on JOY1DAT too.
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
#ifdef MUT_L16
        uint16_t w = joy1dat_for(stim({})); // mutated: drove the wrong port (_joy2 stays idle)
#else
        uint16_t w = joy1dat_for(stim({BIT_RIGHT}));
#endif
        expect(((w >> 1) & 1) == 1, "opus:8-joy.c17");
    }

    // ==================== Page 233 ====================

    // L17 opus:8-joy2.c5+sol:8-joy.c21 -- the joystick's second button reads the same
    // way as the right mouse button, via POTINP bit 10 (userio.v:174, 300-305).
    {
        drive_idle_and_reset();
        top->mouse_btn = 0;
        top->_joy2 = stim({});
        settle(4);
        int neither = (read_addr(POTINP_ADDR) >> 10) & 1;

        top->mouse_btn = 0;
#ifdef MUT_L17
        top->_joy2 = stim({BIT_FIRE}); // mutated: wrong bit, not the second button
#else
        top->_joy2 = stim({BIT_BTN2});
#endif
        settle(4);
        int joy_btn = (read_addr(POTINP_ADDR) >> 10) & 1;

        top->_joy2 = stim({});
        top->mouse_btn = (1u << 1); // right mouse button pressed
        settle(4);
        int mouse_btn_result = (read_addr(POTINP_ADDR) >> 10) & 1;

        note("POTINP bit10: neither=" + std::to_string(neither) +
             " joy2nd=" + std::to_string(joy_btn) +
             " mouseRight=" + std::to_string(mouse_btn_result));
        expect(joy_btn == mouse_btn_result && joy_btn != neither,
               "opus:8-joy2.c5+sol:8-joy.c21");
        top->mouse_btn = 0;
    }

    // ==================== Page 281 (Appendix A) ====================

    drive_idle_and_reset();
    top->mouse_btn = 0;
    enable_joy1_mode();

    // L18 opus:8-appa-joy.c15+sol:8-appa-joy.c17 -- bits 1,0 reveal the state of the
    // two clock pins: pressing only the right pin (x0=0, x2=1) recovers (bit1,bit0)
    // = (1,1), the specific pair that state implies -- not just "some" distinct value.
    {
#ifdef MUT_L18
        uint16_t w = joy0dat_for(stim({BIT_BACK})); // mutated: drove x2 instead of x0
#else
        uint16_t w = joy0dat_for(stim({BIT_RIGHT}));
#endif
        int bit1 = (w >> 1) & 1, bit0 = w & 1;
        expect(bit1 == 1 && bit0 == 1, "opus:8-appa-joy.c15+sol:8-appa-joy.c17");
    }

    // L19 opus:8-appa-joy.c16+sol:8-appa-joy.c18 -- those same pins double as joystick
    // switch inputs: the bits L18 reads also compose (Table 8-3) into the correct
    // right/back switch reading for that same stimulus.
    {
#ifdef MUT_L19
        uint16_t w = joy0dat_for(stim({BIT_BACK})); // mutated: drove x2 instead of x0
#else
        uint16_t w = joy0dat_for(stim({BIT_RIGHT}));
#endif
        Decode d = decode16(w);
        expect(d.right == 1 && d.back == 0, "opus:8-appa-joy.c16+sol:8-appa-joy.c18");
    }

    // L20 opus:8-appa-joy.c53+sol:8-appa-joy.c26 -- active-low at the connector pins,
    // restated from Appendix A: checked here on forward.
    {
        int idle_fwd = decode16(joy0dat_for(stim({}))).forward;
#ifdef MUT_L20
        int pressed_fwd = decode16(joy0dat_for(stim({}))).forward; // mutated: not pressed
#else
        int pressed_fwd = decode16(joy0dat_for(stim({BIT_FWD}))).forward;
#endif
        expect(idle_fwd == 0 && pressed_fwd == 1, "opus:8-appa-joy.c53+sol:8-appa-joy.c26");
    }

    // L21 opus:8-appa-joy.c87+sol:8-appa-joy.c52 -- LEFT active-high directly on Y1
    // (bit 9).
    {
#ifdef MUT_L21
        uint16_t w = joy0dat_for(stim({BIT_FWD})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_LEFT}));
#endif
        expect(((w >> 9) & 1) == 1, "opus:8-appa-joy.c87+sol:8-appa-joy.c52");
    }

    // L22 opus:8-appa-joy.c88+sol:8-appa-joy.c53 -- RIGHT active-high directly on X1
    // (bit 1).
    {
#ifdef MUT_L22
        uint16_t w = joy0dat_for(stim({BIT_BACK})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_RIGHT}));
#endif
        expect(((w >> 1) & 1) == 1, "opus:8-appa-joy.c88+sol:8-appa-joy.c53");
    }

    // L23 opus:8-appa-joy.c89 -- FORWARD/BACK are not directly on one bit: pressing
    // back leaves bit1 alone at 0, only bit1^bit0 recovers it.
    {
#ifdef MUT_L23
        uint16_t w = joy0dat_for(stim({BIT_FWD})); // mutated: pressed forward, not back
#else
        uint16_t w = joy0dat_for(stim({BIT_BACK}));
#endif
        int bit1 = (w >> 1) & 1, bit0 = w & 1;
        expect(bit1 == 0 && (bit1 ^ bit0) == 1, "opus:8-appa-joy.c89");
    }

    // L24 opus:8-appa-joy.c90+sol:8-appa-joy.c54 -- Forward = Y1 xor Y0 (single
    // operand mutated, per the XOR-invariant lesson).
    {
#ifdef MUT_L24
        uint16_t w = joy0dat_for(stim({BIT_LEFT})); // mutated: single operand only
#else
        uint16_t w = joy0dat_for(stim({BIT_FWD}));
#endif
        int y1 = (w >> 9) & 1, y0 = (w >> 8) & 1;
        expect((y1 ^ y0) == 1, "opus:8-appa-joy.c90+sol:8-appa-joy.c54");
    }

    // L25 opus:8-appa-joy.c91+sol:8-appa-joy.c55 -- Left = Y1 alone.
    {
#ifdef MUT_L25
        uint16_t w = joy0dat_for(stim({BIT_BACK})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_LEFT}));
#endif
        expect(((w >> 9) & 1) == 1, "opus:8-appa-joy.c91+sol:8-appa-joy.c55");
    }

    // L26 opus:8-appa-joy.c92+sol:8-appa-joy.c56 -- Back = X1 xor X0 (single operand
    // mutated).
    {
#ifdef MUT_L26
        uint16_t w = joy0dat_for(stim({BIT_FWD})); // mutated: single operand only
#else
        uint16_t w = joy0dat_for(stim({BIT_BACK}));
#endif
        int x1 = (w >> 1) & 1, x0 = w & 1;
        expect((x1 ^ x0) == 1, "opus:8-appa-joy.c92+sol:8-appa-joy.c56");
    }

    // L27 opus:8-appa-joy.c93+sol:8-appa-joy.c57 -- Right = X1 alone.
    {
#ifdef MUT_L27
        uint16_t w = joy0dat_for(stim({BIT_LEFT})); // mutated: wrong pin
#else
        uint16_t w = joy0dat_for(stim({BIT_RIGHT}));
#endif
        expect(((w >> 1) & 1) == 1, "opus:8-appa-joy.c93+sol:8-appa-joy.c57");
    }

    std::cout << g_passed << " passed, " << g_failed << " failed\n";

    delete top;
    return g_failed == 0 ? 0 : 1;
}
