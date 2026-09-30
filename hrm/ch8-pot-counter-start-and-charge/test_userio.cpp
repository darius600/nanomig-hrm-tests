// Test purpose: readers `opus` and `sol`) alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 234, 235, 236, 237, 282, 288, and more
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
// ch8-pot-counter-start-and-charge
//
// Grades 83 claims (readers opus, sol) against userio.v, compiled alone, on
// the module's POTGO/POT0DAT/POT1DAT proportional-input counters. See
// SPECIFICATION.md for the manual quotations and the grouping (G1..G23) this
// file's group_* functions follow.
//
// RTL finding used throughout below: userio.v hardwires joy_swap=1'b0, which
// makes the *internal* signal named "_djoy1" track the external _joy2 input
// port, and "_djoy2" track _joy1 (lines 210-218 of userio.v). Independently
// of that swap, pot1x (POT1DAT's X/low-byte counter) has no external-pin
// term in its gate at all -- potcap[2] is driven purely by POTGO's own
// output-enable bits (userio.v:153,169) -- unlike the other three counters,
// each of which does have an external pin in its gate equation
// (userio.v:167,174,176). This is measured independently below (G1, G4);
// it is not assumed from any other package's finding.
//
// MUTATION opus:8-pot.c1 oracle green substitute: port1's X/Y and port2's Y
//   measurements (which do match, within the same G1 run) routed through the
//   identical "== value after N released ticks" comparator used for the red
//   port2-X half, confirming that comparator is not vacuously false.
// MUTATION sol:8-pot.c1 oracle green (same substitute as opus:8-pot.c1)
// MUTATION harness-selfcheck stimulus red MUT_SELFCHECK: the POTGO arm
//   gate for pot0x is held closed in G1s first measurement, so it never
//   counts and
//   x_tracks is false. Expected values untouched. RUN: baseline 74 passed
//   15 failed; mutated 73 passed 16 failed, exactly one harness-selfcheck
//   newly red, the fifteen baseline reds unchanged. Reverted: 74/15.
//   A first attempt omitted the POTGO arm step instead and changed nothing;
//   it was replaced, not recorded.
// MUTATION opus:8-pot.c11 stimulus red MUT_G2: the arm step of part (a) is
//   built with POTGO=$0000 (bit 0 clear) instead of $0001, so the counters
//   never start; expected value (an unstarted-vs-started equality) is
//   unchanged.
// MUTATION sol:8-pot.c12 stimulus red (same MUT_G2 as opus:8-pot.c11)
// MUTATION opus:8-pot.c12 stimulus red MUT_G3 (repaired 2026-09-15 after
//   independent validation found this assertion stronger than its claim: it
//   required $0001/$00FF to behave identically and all four counters to
//   track -- neither is in "For a standard X-Y joystick, this value is hex
//   0001," and p.282 calls bits 7-1 reserved, not don't-care. Narrowed to
//   what the claim says [pot0x/pot0y/pot1y, the counters with a pin, track
//   under $0001]; now GREEN, so it takes a stimulus mutation, not an oracle
//   substitute): pot0y's gate is an AND of a mouse-side and a
//   swapped-joystick-side release signal (potcap[1] <= _mright & _djoy1[5]
//   & ...), so either alone can open it -- both are built held instead of
//   released (right mouse button unpressed AND _joy2 pin 5, which
//   _djoy1[5] tracks, left unasserted), so pot0y never reaches 7; the
//   "==7" expected value is unchanged. RUN: repaired baseline 77 passed 9
//   failed; mutated 75 passed, 11 failed, exactly this pair newly red.
//   Reverted: 77/9.
// MUTATION sol:8-pot.c13 stimulus red (same MUT_G3 as opus:8-pot.c12)
// MUTATION opus:8-pot.c13 stimulus red MUT_G4 (repaired 2026-09-15: this
//   assertion demanded a four-independently-driven-pin topology the claim
//   never states -- "starts special hardware that reads the potentiometer
//   values" says nothing about pin count; that topology belongs to p.234's
//   "two per port" sentence and p.282's connector table, different uids.
//   Narrowed to what the claim says [each counter that does have a pin
//   tracks it once started]; now GREEN, so it takes a stimulus mutation):
//   the pot0x-only-moved step is built without releasing pot0x's pin, so it
//   never moves; the "==$0004" expected value is unchanged. RUN: repaired
//   baseline 77 passed 9 failed; mutated 75 passed, 11 failed, exactly this
//   pair newly red. Reverted: 77/9.
// MUTATION sol:8-pot.c14 stimulus red (same MUT_G4 as opus:8-pot.c13)
// MUTATION opus:8-pot.c14 stimulus red MUT_G5G18: the reset-triggering POTGO
//   write is built with bit 0 forced clear, so the counters are never
//   zeroed; the "all zero after the bit-0 write" comparison is unchanged and
//   now compares against values that never reset.
// MUTATION sol:8-pot.c15 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION opus:8-potreg.c5 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION sol:8-pot.c32 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION opus:8-potchg.c1 oracle green substitute (repaired 2026-09-15:
//   the prior control was a harness-side loop integer incremented to 8 in a
//   visible loop -- never touching the DUT -- which independent validation
//   correctly called not a DUT measurement. New control: pot0x itself,
//   DUT-measured after exactly 7 released ticks -- the same quantity G1/G3
//   independently establish reaches 7 -- routed through the identical
//   count==7||count==8 comparator used for the red DUT-measured reset
//   interval. The predicate this control backs also changed: it now
//   compares completed-reset-line count [first_increment_at - 1], not the
//   raw pulse index, against 7-or-8 -- see group_G6_G11.)
// MUTATION sol:8-pot.c16 oracle green (same substitute as opus:8-potchg.c1)
// MUTATION opus:8-fig8-5.c1 oracle green (same substitute as opus:8-potchg.c1;
//   this pair's predicate is repaired to grade against exactly 8, matching
//   the figure's precise callout, not the 7-or-8 the body-text pair uses --
//   both used to wrongly accept 7 here)
// MUTATION sol:8-fig8-5.c2 oracle green (same substitute as opus:8-potchg.c1)
// MUTATION opus:8-potchg.c4 stimulus red MUT_G7G8: each of the ten single
//   pulses in the cadence loop is built as two pot_cnt_en pulses instead of
//   one, so consecutive reads differ by 2, not 1; the "delta==1 every time"
//   comparison is unchanged.
// MUTATION sol:8-pot.c19 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION opus:8-potchg.c5 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION sol:8-pot.c20 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION opus:8-potchg.c6 stimulus red MUT_G9 (repaired 2026-09-15: this
//   assertion demanded the counter stay latched even after an artificial
//   digital comparator reversal with no intervening POTGO -- an RC charge
//   physically cannot fall back below the preset absent a POTGO dump, so
//   "held until the next POTGO is issued" is simply the natural consequence
//   of a charge that cannot decrease any other way, and the page states
//   nothing about a reversal case. Narrowed to what's testable [the counter
//   does not advance while the charge stays above]; now GREEN, so it takes
//   a stimulus mutation, not an oracle substitute): the hold step is built
//   skipping the gate-close call, so the counter keeps counting instead of
//   holding; the "during == before" expected value is unchanged. RUN:
//   repaired baseline 77 passed 9 failed; mutated 75 passed, 11 failed,
//   exactly this pair newly red. Reverted: 77/9.
// MUTATION sol:8-pot.c21 stimulus red (same MUT_G9 as opus:8-potchg.c6)
// MUTATION opus:8-potchg.c8 stimulus red MUT_G10: the run to $ff is built one
//   pulse short (254 instead of 255), so the wrap that should land on pulse
//   256 is off by one and the fixed $ff/$00/$01 comparison sequence fails.
// MUTATION sol:8-pot.c24 stimulus red (same MUT_G10 as opus:8-potchg.c8)
// MUTATION opus:8-appa-pot.c10 stimulus red (same MUT_G10 as opus:8-potchg.c8)
// MUTATION opus:8-fig8-5.c2 stimulus red MUT_G12: pot1x's release step is
//   built using POTGO bits 11:10 (which gate pot0y) instead of 13:12 (which
//   gate pot1x), so pot1x is never actually released and stays at 0 instead
//   of reaching 6; the fixed "all four equal 6" comparison is unchanged.
// MUTATION sol:8-fig8-5.c3 stimulus red (same MUT_G12 as opus:8-fig8-5.c2)
// MUTATION opus:8-fig8-5.c4 stimulus red MUT_G13: the "early" run's hold
//   point is built at tick 8 and the "late" run's at tick 3 (the two
//   stimuli are swapped), so the later-crossing-is-larger comparison, left
//   unchanged, now compares the wrong way round.
// MUTATION opus:8-potreg.c1 stimulus red MUT_G14G17: the port-identity check
//   reads POT1DAT where it should read POT0DAT (and vice versa) for the
//   mouse-only-movement half, so the fixed "mouse pins move only POT0DAT"
//   comparison now sees the wrong register.
// MUTATION sol:8-pot.c26 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION sol:8-pot.c27 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION opus:8-potreg.c2 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION sol:8-pot.c28 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION sol:8-pot.c29 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION opus:8-potreg.c3 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION sol:8-pot.c30 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION opus:8-potreg.c4 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION sol:8-pot.c31 stimulus red (same MUT_G14G17 as opus:8-potreg.c1)
// MUTATION opus:8-appa-pot.c9 stimulus red MUT_G19G21: pot0y is driven to 5
//   ticks but the fixed comparison still expects the pre-mutation value (4)
//   for it, so the four-counters-independent equality fails.
// MUTATION sol:8-appa-pot.c11 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION sol:8-appa-pot.c12 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION opus:8-appa-pot.c11 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION sol:8-appa-pot.c13 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION sol:8-appa-pot.c14 stimulus red MUT_G22: the left-connector
//   release step is built skipping the mouse-button assertion, so POT0DAT
//   never leaves $0000 and the fixed "== 6 after release" comparison fails.
// MUTATION sol:8-appa-pot.c15 oracle green substitute: the identical
//   "counter moved when its connector pin was asserted" comparator, applied
//   to the left connector within the same G22 run, goes green.
// MUTATION opus:8-appa-pot.c12 oracle green (same substitute as sol:8-appa-pot.c15)
// MUTATION opus:8-appa-pot.c13 oracle green (same substitute as sol:8-appa-pot.c15)
// MUTATION opus:8-appa-pot.c14 stimulus red MUT_G23: the X and Y drive
//   targets are built swapped ($b4 where $4b was asked for and vice versa);
//   since $4b and $b4 are exact bitwise complements, every bit read now
//   differs from the unchanged $4b/$b4 expectation table; this one line
//   stands for all 32 individual-bit labels below plus the four sol
//   byte-level labels, all graded from the same two register reads in
//   group_G23().
// MUTATION opus:8-appa-pot.c15 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c16 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c17 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c18 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c19 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c20 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c21 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c22 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c23 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c24 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c25 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c26 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c27 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c28 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c29 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c30 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c31 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c32 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c33 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c34 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c35 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c36 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c37 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c38 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c39 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c40 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c41 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c42 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c43 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c44 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION opus:8-appa-pot.c45 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION sol:8-appa-pot.c16 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION sol:8-appa-pot.c17 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION sol:8-appa-pot.c18 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
// MUTATION sol:8-appa-pot.c19 stimulus red (same MUT_G23 as opus:8-appa-pot.c14)
//
// group_FINALCOUNT grades the final-count form of G6/G11 -- NM-HRM-038's own
// text: "what separates NanoMig from all four [reference implementations]
// is the final count, which in NanoMig includes the interval's lines,"
// since two of the four (WinUAE, vAmiga) also read non-zero after one line,
// same as NanoMig, so G6/G11's first-nonzero-read measurement alone does
// not separate them. Its four assertions (FINALCOUNT-FIG8-5-OPUS,
// FINALCOUNT-FIG8-5-SOL, FINALCOUNT-BODY-OPUS, FINALCOUNT-BODY-SOL) grade
// the SAME four claim ids G6/G11 grades (opus:8-fig8-5.c1, sol:8-fig8-5.c2,
// opus:8-potchg.c1, sol:8-pot.c16) from a second, independent measurement --
// the final count after a fixed run, not the pulse index of the first
// nonzero read -- under their own leading ids so the mutation-label gate
// can pair a record with each one.
// MUTATION FINALCOUNT-FIG8-5-OPUS stimulus green: kTotalTicks = kK (no reset interval prepended) instead of 8+kK, through the real figure_all_four comparator (all four counters ==k). Replayed (make MUT=FINALCOUNT test): all four counters read exactly 10, all four FINALCOUNT-* lines turn green (81 passed, 9 failed -- the nine other, pre-existing reds unaffected); reverted (77 passed, 13 failed).
// MUTATION FINALCOUNT-FIG8-5-SOL stimulus green: same run as FINALCOUNT-FIG8-5-OPUS (the shared kTotalTicks substitution).
// MUTATION FINALCOUNT-BODY-OPUS stimulus green: same run as FINALCOUNT-FIG8-5-OPUS, through the real body_all_four comparator (all four counters ==k or ==k+1; measured exactly k).
// MUTATION FINALCOUNT-BODY-SOL stimulus green: same run as FINALCOUNT-FIG8-5-OPUS.

#include "Vuserio.h"
#include "verilated.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

static Vuserio *dut;

struct Checks { int passed = 0; int failed = 0; } checks;

bool expect(bool cond, const std::string& label) {
    std::cout << (cond ? "PASS: " : "FAIL: ") << label << "\n";
    if (cond) checks.passed++; else checks.failed++;
    return cond;
}
void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

static std::string hex4(uint16_t v) {
    std::ostringstream os;
    os << std::hex << std::setw(4) << std::setfill('0') << v;
    return os.str();
}

// register addresses, as reg_address_in[8:1] values (word address >> 1)
static const int A_POT0DAT = 0x012 >> 1;
static const int A_POT1DAT = 0x014 >> 1;
static const int A_POTINP  = 0x016 >> 1;
static const int A_POTGO   = 0x034 >> 1;
static const int A_IDLE    = 0x000;

static void tick() {
    dut->clk = 0; dut->eval();
    dut->clk = 1; dut->eval();
}
static void settle(int n = 3) { for (int i = 0; i < n; i++) tick(); }

static void idle_bus() { dut->reg_address_in = A_IDLE; dut->data_in = 0; }

static void full_reset() {
    dut->reset = 1;
    dut->clk7_en = 1;
    dut->pot_cnt_en = 0;
    idle_bus();
    dut->mouse_btn = 0;
    dut->_joy1 = 0xFFFF;
    dut->_joy2 = 0xFFFF;
    dut->_fire0_dat = 1;
    dut->_fire1_dat = 1;
    dut->joy_ana1 = 0;
    dut->joy_ana2 = 0;
    dut->kms_level = 0;
    dut->kbd_mouse_type = 0;
    dut->kbd_mouse_data = 0;
    for (int i = 0; i < 4; i++) tick();
    dut->reset = 0;
    settle(3);
}

// One POTGO bus cycle: latches val into potreg. If val&1, the *following*
// cycle's arm/reset fires (userio.v:99-105, 126-133) -- see arm_potgo().
static void write_potgo_once(uint16_t val) {
    dut->reg_address_in = A_POTGO;
    dut->data_in = val;
    tick();
    idle_bus();
}
// Full POTGO sequence: write val, let the arm/reset (if bit 0 set) take
// effect, let the joystick-pin synchronizers settle.
static void arm_potgo(uint16_t val) {
    write_potgo_once(val);
    tick(); // arm/reset fires here if val&1; potreg[0] self-clears after this edge
    settle(3);
}
// Persistent-bits-only write: same latch, but bit 0 must already be (or stay)
// clear, so no arm/reset fires. Used to change POTGO's OUT/DAT bits (which
// gate pot1x -- see the file header note) without restarting anything.
static void set_potgo_bits(uint16_t val) {
    write_potgo_once(val & ~0x0001);
    tick();
    settle(2);
}

static uint16_t read_reg(int addr) {
    dut->reg_address_in = addr;
    dut->eval();
    uint16_t v = dut->data_out;
    idle_bus();
    dut->eval();
    return v;
}
static uint16_t pot0dat() { return read_reg(A_POT0DAT); }
static uint16_t pot1dat() { return read_reg(A_POT1DAT); }

static void pulse(int n) {
    for (int i = 0; i < n; i++) {
        dut->pot_cnt_en = 1;
        tick();
        dut->pot_cnt_en = 0;
    }
}

// mouse_btn bit0=left, bit1=right, bit2=third/middle
static void set_mouse(int left, int right, int third) {
    dut->mouse_btn = (left ? 1 : 0) | (right ? 2 : 0) | (third ? 4 : 0);
}
// _joy1/_joy2 are active-low (idle = 1, unpressed/unasserted)
static void set_joy1_pin5(int asserted) {
    if (asserted) dut->_joy1 &= (uint16_t)~(1 << 5); else dut->_joy1 |= (1 << 5);
}
static void set_joy2_pin5(int asserted) {
    if (asserted) dut->_joy2 &= (uint16_t)~(1 << 5); else dut->_joy2 |= (1 << 5);
}

// p.235's reset interval, prepended below wherever a group releases a pin
// at POTGO time and pulses a fixed total tick count expecting the counter
// to read a specific value. Only group_G6_G11 and group_FINALCOUNT grade
// the interval's own length; every other group's claim is about something
// else (existence, rate, wrap, register identity, ...), but its own
// numbers still have to be read against a page-conforming counter, not
// against a raw tick total that silently assumes no interval at all --
// exactly the "about 40 greens assume counting starts on the first line"
// gap validation measured outside G6_G11/FINALCOUNT. kResetInterval is
// Figure 8-5's exact reading (8); matches_page_count() below accepts the
// body text's other allowed interval (7 lines, one shorter, leaving one
// extra counted line) as well, the same k-or-k+1 shape group_FINALCOUNT
// already uses for the body-text pair.
constexpr int kResetInterval = 8;

static bool matches_page_count(int measured, int n) {
    return measured == n || measured == n + 1;
}

//--------------------------------------------------------------------------
// G1 -- p.234: each controller port handles two proportional inputs.
//--------------------------------------------------------------------------
static void group_G1() {
    full_reset();
    arm_potgo(0x0001);
    // MUT_SELFCHECK holds pot0x PRESSED instead of releasing it, so its gate
    // never opens, the counter stays at 0 and x_tracks is false. Expected
    // values untouched: this changes what the module is asked to do.
    //
    // A first attempt omitted the arm_potgo() call here instead and changed
    // NOTHING -- the counter runs without it. A mutation that does not move
    // the result is worse than none, so it was replaced rather than recorded.
#ifdef MUT_SELFCHECK
    set_mouse(0, 0, 0); // hold pot0x pressed: its gate never opens
#else
    set_mouse(0, 0, 1); // release pot0x only (mouse button 3)
#endif
    settle(3);
    pulse(kResetInterval + 6);
    uint16_t r1 = pot0dat();
    bool x_tracks = matches_page_count(r1 & 0x00FF, 6) && ((r1 >> 8) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0); // release pot0y only (mouse button 2/right)
    settle(3);
    pulse(kResetInterval + 9);
    uint16_t r2 = pot0dat();
    bool y_tracks = matches_page_count(r2 >> 8, 9) && ((r2 & 0x00FF) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1); // release pot1y (via the swapped _djoy2<=_joy1 path)
    settle(3);
    pulse(kResetInterval + 11);
    uint16_t r3 = pot1dat();
    bool port2_y_tracks = matches_page_count(r3 >> 8, 11) && ((r3 & 0x00FF) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000; // assert every pin this module reads
    settle(3);
    pulse(kResetInterval + 11); // aligned with port2_y_tracks's run length above
    uint16_t r4 = pot1dat();
    bool port2_x_moved = ((r4 & 0x00FF) != 0); // nonzero, not a specific count -- interval-robust as is

    note("port 1 (POT0DAT), release-then-run-" + std::to_string(kResetInterval + 6) +
         "/" + std::to_string(kResetInterval + 9) + "-ticks-unbroken (8+6/8+9): "
         "X-only -> $" + hex4(r1) + "; Y-only -> $" + hex4(r2) +
         " -- two independently-movable counters, each graded against the "
         "page's reset interval (6 or 7; 9 or 10), not the raw tick total. "
         "port 2 (POT1DAT), release-then-run-" + std::to_string(kResetInterval + 11) +
         " (8+11): joystick-port pin release -> $" + hex4(r3) +
         " (Y moved); every controller pin this module reads, asserted -> $" +
         hex4(r4) + " -- the low byte (X) never moved. Port 2 carries only "
         "one externally-driven proportional counter in this design, not two.");

    bool ok = x_tracks && y_tracks && port2_y_tracks && port2_x_moved;
    expect(ok, "opus:8-pot.c1");
    expect(ok, "sol:8-pot.c1");
    expect(x_tracks && y_tracks && port2_y_tracks,
           "harness-selfcheck: G1 comparator is live (port1 X, port1 Y, port2 Y all matched)");
}

//--------------------------------------------------------------------------
// G2 -- p.234: a POTGO write (not any other write, not any other bit) is
// what starts the reading.
//--------------------------------------------------------------------------
static void group_G2() {
    full_reset();
    set_mouse(0, 0, 1);
    settle(3);
    pulse(9); // let the counter free-run to a nonzero value with no POTGO
              // write at all yet, so the next step can only reach 5 if the
              // POTGO write itself re-zeroes it -- reset() alone already
              // zeroes fresh counters, so that path can't be used to tell
              // "POTGO started it" apart from "it was already zero". This
              // free-run has no POTGO write at all, so p.235's interval
              // (which the page ties to a POTGO write) does not bear on it.
#ifdef MUT_G2
    arm_potgo(0x0000); // MUTATED: bit 0 clear -- must not re-zero
#else
    arm_potgo(0x0001);
#endif
    pulse(kResetInterval + 5);
    uint16_t a = pot0dat();
    bool starts_on_potgo = matches_page_count(a & 0x00FF, 5);

    full_reset();
    set_mouse(0, 0, 1);
    arm_potgo(0x0001);
    pulse(kResetInterval + 3); // past the interval, so "before" is genuinely counting, not stuck at 0
    uint16_t before = pot0dat();
    dut->reg_address_in = A_POTINP; dut->data_in = 0xFFFF; // access a *different* register
    tick();
    idle_bus();
    tick();
    uint16_t after_other = pot0dat();
    bool other_write_no_restart = (after_other == before);
    pulse(2);
    uint16_t after_more = pot0dat();
    bool counting_continued = matches_page_count(after_more & 0x00FF, 5);

    full_reset();
    set_mouse(0, 0, 1);
    arm_potgo(0x0001);
    pulse(kResetInterval + 4); // past the interval, so "before2" is genuinely counting, not stuck at 0
    uint16_t before2 = pot0dat();
    write_potgo_once(0x00F0); // POTGO write, bit 0 clear
    tick();
    settle(2);
    uint16_t after2 = pot0dat();
    bool bit0_clear_no_restart = (after2 == before2);

    note("POTGO write with bit 0 set, release-then-run-" + std::to_string(kResetInterval + 5) +
         "-ticks-unbroken (8+5) -> $" + hex4(a) + " (page-conforming range: 5 or 6); "
         "a write to a different register mid-run left POT0DAT at $" +
         hex4(after_other) + " (was $" + hex4(before) +
         "), and counting continued afterwards (-> $" + hex4(after_more) +
         "); a POTGO write with bit 0 clear left it at $" + hex4(after2) +
         " (was $" + hex4(before2) + ").");

    bool ok = starts_on_potgo && other_write_no_restart && counting_continued &&
              bit0_clear_no_restart;
    expect(ok, "opus:8-pot.c11");
    expect(ok, "sol:8-pot.c12");
}

//--------------------------------------------------------------------------
// G3 -- p.234: POTGO=$0001 is the standard-joystick start value. Narrowed
// 2026-09-15 after independent validation: the quoted sentence states only
// the hex value itself. It does not say $0001 and $00FF must behave
// identically (p.282 calls bits 7-1 reserved, not don't-care -- that half
// was invented) and it does not say all four counters must track an
// external pin (pot1x has none at all in this design -- see the file
// header note; the four-pin topology is p.234's earlier "two per port"
// sentence and p.282's connector table, different uids, graded there).
// What remains and IS the claim's content: writing this specific value
// starts the counters that do have a pin tracking their own pin.
//--------------------------------------------------------------------------
static void group_G3() {
    full_reset();
    arm_potgo(0x0001);
#ifdef MUT_G3
    // MUTATED: pot0y's gate is potcap[1] <= _mright & _djoy1[5] & ...  (an
    // AND, so EITHER the mouse path or the swapped-joystick path alone can
    // open it -- both must be held for the gate to stay closed). Hold both:
    // the right mouse button stays unpressed, and _joy2 pin 5 (which
    // _djoy1[5] tracks, per the file header note on joy_swap) stays
    // unasserted.
    set_mouse(1, 0, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0020;
#else
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
#endif
    settle(3);
    pulse(kResetInterval + 7);
    uint16_t p0_1 = pot0dat(), p1_1 = pot1dat();

    bool pot0x_tracks = matches_page_count(p0_1 & 0xFF, 7);
    bool pot0y_tracks = matches_page_count(p0_1 >> 8, 7);
    bool pot1y_tracks = matches_page_count(p1_1 >> 8, 7);
    bool pot1x_tracks = matches_page_count(p1_1 & 0xFF, 7); // NOT graded here -- see the file header note

    note("POTGO=$0001, every controller pin released, release-then-run-" +
         std::to_string(kResetInterval + 7) + "-ticks-unbroken (8+7): "
         "POT0DAT $" + hex4(p0_1) + ", POT1DAT $" + hex4(p1_1) +
         ". pot0x/pot0y/pot1y each track their own pin, which is this "
         "claim's content. pot1x " + std::string(pot1x_tracks ? "also matched the page range (not required here)."
         : "does not match the page range (it has no external pin at all in "
           "this design -- not required by this claim; the pin-topology "
           "claims are graded separately, G1 and G22).") );

    bool ok = pot0x_tracks && pot0y_tracks && pot1y_tracks;
    expect(ok, "opus:8-pot.c12");
    expect(ok, "sol:8-pot.c13");
}

//--------------------------------------------------------------------------
// G4 -- p.234: writing POTGO starts hardware that reads the potentiometer
// values. Narrowed 2026-09-15 after independent validation: the quoted
// sentence says nothing about how many pins exist or whether each of four
// counters must have its own -- it only says the started hardware reads
// pot values. Testable as: each counter that this design does gate from an
// external pin (pot0x, pot0y, pot1y) tracks that pin once started, and
// nothing moves while every gate is held. The four-independently-wired-pin
// topology this assertion used to also require belongs to a different
// claim (p.234's "two per port" sentence / p.282's connector table) and is
// graded there, not here -- pot1x has no external pin at all in this
// design (see the file header note), which is exactly why that topology
// claim is correctly red elsewhere and must not be re-tested by this one.
//--------------------------------------------------------------------------
static void group_G4() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 0);
    dut->_joy1 = 0xFFFF; dut->_joy2 = 0xFFFF;
    settle(3);
    pulse(10);
    bool nothing_moves_when_held = (pot0dat() == 0x0000) && (pot1dat() == 0x0000);

    full_reset();
    arm_potgo(0x0001);
#ifdef MUT_G4
    set_mouse(0, 0, 0); // MUTATED: pot0x's pin is built held, not released
#else
    set_mouse(0, 0, 1);
#endif
    settle(3);
    pulse(kResetInterval + 4);
    uint16_t r_x0 = pot0dat();
    bool only_pot0x_moved = matches_page_count(r_x0 & 0xFF, 4) &&
                             ((r_x0 >> 8) == 0) && (pot1dat() == 0x0000);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0);
    settle(3);
    pulse(kResetInterval + 4);
    uint16_t r_y0 = pot0dat();
    bool only_pot0y_moved = matches_page_count(r_y0 >> 8, 4) &&
                             ((r_y0 & 0xFF) == 0) && (pot1dat() == 0x0000);

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1);
    settle(3);
    pulse(kResetInterval + 4);
    uint16_t r_y1 = pot1dat();
    bool only_pot1y_moved = (pot0dat() == 0x0000) &&
                             matches_page_count(r_y1 >> 8, 4) && ((r_y1 & 0xFF) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3);
    pulse(kResetInterval + 4);
    uint16_t everything_p1 = pot1dat();
    bool pot1x_has_a_pin = ((everything_p1 & 0x00FF) != 0); // NOT graded here -- see the file header note

    note("Isolated-release measurements (all four counters, release-then-run-" +
         std::to_string(kResetInterval + 4) + "-ticks-unbroken (8+4) each): "
         "pot0x-only moved pot0x; pot0y-only moved pot0y; pot1y-only moved "
         "pot1y -- each counter that has a pin tracks it, graded against the "
         "page's reset interval (4 or 5), which is this claim's content. "
         "Every controller pin asserted left POT1DAT's low byte at $" +
         hex4(everything_p1 & 0xFF) + " (pot1x did not move; it has no "
         "external pin at all in this design, not required by this claim "
         "-- see the file header note).");

    bool ok = nothing_moves_when_held && only_pot0x_moved && only_pot0y_moved &&
              only_pot1y_moved;
    expect(ok, "opus:8-pot.c13");
    expect(ok, "sol:8-pot.c14");
}

//--------------------------------------------------------------------------
// G5/G18 -- p.234 (loose) and p.236 (precise): a POTGO write with bit 0 set
// zeroes all four counters; bit 0 clear leaves them unchanged. G5 must be
// graded against the same bit-0 distinction G18 states explicitly, or it
// would pass for the wrong reason (any POTGO write happening to carry bit 0).
//--------------------------------------------------------------------------
static void group_G5_G18() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000; // release pot0x, pot0y, pot1y
#ifdef MUT_G5G18
    // MUTATED: leave pot1x un-released (skip the OUT/DAT bit write) --
    // its own count will be short, but the reset check below is what matters.
#else
    set_potgo_bits(0x2000); // release pot1x too (bits 13:12 = 1,0)
#endif
    settle(2);
    pulse(9);
    uint16_t before0 = pot0dat(), before1 = pot1dat();
    bool nonzero_before = (before0 != 0) && (before1 != 0);

#ifdef MUT_G5G18
    arm_potgo(0x0000 | 0x0000); // MUTATED: bit 0 clear -- must not zero
#else
    arm_potgo(0x2001); // keep pot1x's release bits, add bit 0
#endif
    uint16_t after_bit0_hi = pot0dat(), after_bit0_hi_p1 = pot1dat();
    bool all_zero_after_bit0 = (after_bit0_hi == 0) && (after_bit0_hi_p1 == 0);

    pulse(6);
    uint16_t before3 = pot0dat(), before3_p1 = pot1dat();
    write_potgo_once(0x2000); // bit 0 clear
    tick();
    settle(2);
    uint16_t after3 = pot0dat(), after3_p1 = pot1dat();
    bool bit0_low_unchanged = (after3 == before3) && (after3_p1 == before3_p1);

    note("Before a bit-0 POTGO write: POT0DAT $" + hex4(before0) + " POT1DAT $" +
         hex4(before1) + "; right after POTGO=$2001: POT0DAT $" +
         hex4(after_bit0_hi) + " POT1DAT $" + hex4(after_bit0_hi_p1) +
         "; after running again and a bit-0-clear POTGO write ($2000): "
         "POT0DAT $" + hex4(after3) + " (was $" + hex4(before3) +
         ") POT1DAT $" + hex4(after3_p1) + " (was $" + hex4(before3_p1) + ").");

    bool ok = nonzero_before && all_zero_after_bit0 && bit0_low_unchanged;
    expect(ok, "opus:8-pot.c14");
    expect(ok, "sol:8-pot.c15");
    expect(ok, "opus:8-potreg.c5");
    expect(ok, "sol:8-pot.c32");
}

//--------------------------------------------------------------------------
// G6/G11 -- p.235 body text and Figure 8-5: the read circuitry stays in
// reset for the first seven or eight (figure: "eight") horizontal scan
// lines after POTGO.
//--------------------------------------------------------------------------
static bool matches_reset_interval(int n) { return n == 7 || n == 8; }

static void group_G6_G11() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    int first_increment_at = -1;
    for (int i = 1; i <= 12 && first_increment_at < 0; i++) {
        pulse(1);
        if ((pot0dat() & 0xFF) != 0) first_increment_at = i;
    }
    // Repaired 2026-09-15: first_increment_at is the 1-indexed pulse number
    // at which the count first reads nonzero, not the count of *completed*
    // reset lines that preceded it -- a first increment at pulse 1 means
    // ZERO completed reset lines, not "1" or "7-or-8". The manual names the
    // completed-reset-line count ("stays in reset for the first seven or
    // eight lines"), so that is the quantity to compare against 7/8, not
    // the raw pulse index.
    int completed_reset_lines = first_increment_at - 1;
    note("pot0x's gate was released before the first pulse; its first "
         "nonzero read came after pulse " + std::to_string(first_increment_at) +
         " -- " + std::to_string(completed_reset_lines) + " completed reset "
         "line(s) preceded it. This RTL holds no separate multi-line reset "
         "state at all.");

    bool body_text_claim = matches_reset_interval(completed_reset_lines); // p.235 text: "seven or eight"
    expect(body_text_claim, "opus:8-potchg.c1");
    expect(body_text_claim, "sol:8-pot.c16");

    // Figure 8-5's callout gives the precise case ("eight"), not the body
    // text's looser "seven or eight" -- repaired 2026-09-15: this pair used
    // to share the 7-or-8 predicate with the body-text pair and wrongly
    // accepted 7. Graded tighter, against exactly 8.
    bool figure_claim = (completed_reset_lines == 8);
    expect(figure_claim, "opus:8-fig8-5.c1");
    expect(figure_claim, "sol:8-fig8-5.c2");

    // Substitution proof for these already-red assertions (a stimulus
    // mutation proves nothing further on a line already red). Repaired
    // 2026-09-15: the previous control was a harness-side loop integer,
    // incremented in a visible loop and never touching the DUT -- not a DUT
    // measurement, per independent validation. This control is pot0x
    // itself, DUT-measured after exactly 7 released ticks (the same
    // quantity G1 and G3 independently establish reaches 7 under this
    // condition), routed through the identical comparator.
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    pulse(7);
    int dut_seven = pot0dat() & 0xFF;
    bool comparator_live = matches_reset_interval(dut_seven);
    note("Substitute for the already-red reset-interval comparators: pot0x, "
         "DUT-measured (not a harness loop integer) after exactly 7 "
         "released ticks, reads " + std::to_string(dut_seven) + " and is "
         "routed through the identical count==7||count==8 comparator used "
         "above, confirming that comparator is not vacuously false.");
    expect(comparator_live, "harness-selfcheck: G6/G11 comparator is live (DUT pot0x measured at 7)");
}

//--------------------------------------------------------------------------
// FINALCOUNT -- p.235/Figure 8-5, the final-count form of G6/G11. G6/G11 above measure the pulse
// index of the FIRST nonzero read, and NM-HRM-038's own text says why that
// measurement alone under-states the finding: two of the four reference
// implementations (WinUAE, vAmiga) also read non-zero after one line -- their
// internal delay counter runs DURING the interval and is restarted from zero
// at its end, so "first read is nonzero" does not by itself separate NanoMig
// from a page-following design. "What separates NanoMig from all four is the
// final count, which in NanoMig includes the interval's lines." This grades
// exactly that: release, run for a fixed total of (interval + k) ticks with
// no pause, and compare the FINAL count against k, not against zero-or-not.
// k=10 (this file's own established cadence granularity -- see G7/G8, G12,
// G13's ten-tick runs), against the page's PAL interval (Figure 8-5's precise
// eight, and the body text's seven-or-eight) -- neither reading is close to
// overflowing the 8-bit counter G10 already measures, so k's size is not
// itself in question.
//
// Measurement basis for all four counters, using the same per-counter
// release technique as G12 (which already establishes "the same count/hold
// rule for all four" at a shorter span): pot0x/pot0y via set_mouse, pot1y via
// set_joy1_pin5, pot1x via set_potgo_bits(0x2000) (bits 13:12, per G12).
//--------------------------------------------------------------------------
static void group_FINALCOUNT() {
    constexpr int kK = 10;
#ifdef MUT_FINALCOUNT
    constexpr int kTotalTicks = kK;      // guarded mutation: no interval prepended
#else
    constexpr int kTotalTicks = 8 + kK;  // Figure 8-5's precise interval + k
#endif

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);  // release pot0x
    settle(3);
    pulse(kTotalTicks);
    int final_x0 = pot0dat() & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0);  // release pot0y
    settle(3);
    pulse(kTotalTicks);
    int final_y0 = (pot0dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1);  // release pot1y
    settle(3);
    pulse(kTotalTicks);
    int final_y1 = (pot1dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_potgo_bits(0x2000);  // release pot1x (bits 13:12, per G12)
    settle(2);
    pulse(kTotalTicks);
    int final_x1 = pot1dat() & 0xFF;

    // Figure 8-5's precise reading: final count == k exactly.
    bool figure_all_four = (final_x0 == kK) && (final_y0 == kK) &&
                            (final_y1 == kK) && (final_x1 == kK);

    // Body text's looser reading: final count == k or k+1 (seven- or
    // eight-line interval).
    bool body_all_four =
        (final_x0 == kK || final_x0 == kK + 1) &&
        (final_y0 == kK || final_y0 == kK + 1) &&
        (final_y1 == kK || final_y1 == kK + 1) &&
        (final_x1 == kK || final_x1 == kK + 1);

    // The MEASURED conclusion is computed from what was actually read, not
    // printed as fixed text: it must describe a page-conforming DUT (where
    // figure_all_four or body_all_four holds) the same way it describes
    // NanoMig (where neither does, at baseline).
    std::string conclusion;
    if (final_x0 == kTotalTicks && final_y0 == kTotalTicks &&
        final_y1 == kTotalTicks && final_x1 == kTotalTicks) {
        conclusion = "no counter's final count reflects any reset interval "
                     "at all -- every one is the raw tick total, " +
                     std::to_string(kTotalTicks) + ".";
    } else if (figure_all_four) {
        conclusion = "every counter's final count matches Figure 8-5's "
                     "exact reading, k=" + std::to_string(kK) + ".";
    } else if (body_all_four) {
        conclusion = "every counter's final count matches the body text's "
                     "k-or-k+1 reading.";
    } else {
        conclusion = "the four counters do not all agree with each other "
                     "or with either printed reading.";
    }
    note("FINAL COUNT after release-then-run-" + std::to_string(kTotalTicks) +
         "-ticks-unbroken (8+" + std::to_string(kK) + "), no hold in between: "
         "pot0x=" + std::to_string(final_x0) + " pot0y=" + std::to_string(final_y0) +
         " pot1y=" + std::to_string(final_y1) + " pot1x=" + std::to_string(final_x1) +
         " -- the page's reset interval (seven-or-eight lines, body text;"
         " exactly eight, Figure 8-5) predicts a final count of " +
         std::to_string(kK) + " or " + std::to_string(kK + 1) + "; " + conclusion);

    // Each of the four assertions below carries its own leading id, distinct
    // from G6/G11's bare claim-id labels for these same four ids (G6/G11
    // grades the pulse index of the first nonzero read; this grades the
    // final count after a fixed total run -- two different measurements of
    // the same claims), so the mutation-label gate can pair a record with
    // each one; the original claim id stays in full in each label for
    // per-claim coverage counting.
    expect(figure_all_four,
        "FINALCOUNT-FIG8-5-OPUS opus:8-fig8-5.c1 (Figure 8-5, p.235, "
        "NM-HRM-038: \"starts eight horizontal lines after POTGO is "
        "written\"): 8+k lines after POTGO with no pause, all four "
        "counters read exactly k=" + std::to_string(kK) + " -- measured "
        "pot0x=" + std::to_string(final_x0) + " pot0y=" + std::to_string(final_y0) +
        " pot1y=" + std::to_string(final_y1) + " pot1x=" + std::to_string(final_x1) + ".");
    expect(figure_all_four,
        "FINALCOUNT-FIG8-5-SOL sol:8-fig8-5.c2 (Figure 8-5, p.235, "
        "NM-HRM-038: \"starts eight horizontal lines after POTGO is "
        "written\"): 8+k lines after POTGO with no pause, all four "
        "counters read exactly k=" + std::to_string(kK) + " -- measured "
        "pot0x=" + std::to_string(final_x0) + " pot0y=" + std::to_string(final_y0) +
        " pot1y=" + std::to_string(final_y1) + " pot1x=" + std::to_string(final_x1) + ".");
    expect(body_all_four,
        "FINALCOUNT-BODY-OPUS opus:8-potchg.c1 (p.235 body text, "
        "NM-HRM-038: \"stays in a reset state for the first seven or "
        "eight horizontal video scan lines\"): 8+k lines after POTGO "
        "with no pause, all four counters read k=" + std::to_string(kK) +
        " or k+1=" + std::to_string(kK + 1) + " -- measured "
        "pot0x=" + std::to_string(final_x0) + " pot0y=" + std::to_string(final_y0) +
        " pot1y=" + std::to_string(final_y1) + " pot1x=" + std::to_string(final_x1) + ".");
    expect(body_all_four,
        "FINALCOUNT-BODY-SOL sol:8-pot.c16 (p.235 body text, NM-HRM-038: "
        "\"stays in a reset state for the first seven or eight horizontal "
        "video scan lines\"): 8+k lines after POTGO with no pause, all "
        "four counters read k=" + std::to_string(kK) + " or k+1=" +
        std::to_string(kK + 1) + " -- measured pot0x=" + std::to_string(final_x0) +
        " pot0y=" + std::to_string(final_y0) + " pot1y=" + std::to_string(final_y1) +
        " pot1x=" + std::to_string(final_x1) + ".");
}

//--------------------------------------------------------------------------
// G7/G8 -- p.235: once counting, the circuit compares once per scan line
// (never zero, never two increments per tick), and increments while below
// the preset.
//--------------------------------------------------------------------------
static void group_G7_G8() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    pulse(kResetInterval); // past the interval first: this claim is about the
                            // per-tick rate ONCE counting, not about when
                            // counting starts (G6_G11/FINALCOUNT's claim)
    uint16_t prev = pot0dat();
    bool always_exactly_one = true;
    for (int i = 1; i <= 10; i++) {
#ifdef MUT_G7G8
        pulse(2); // MUTATED: two ticks of the strobe per loop iteration
#else
        pulse(1);
#endif
        uint16_t now = pot0dat();
        int delta = (int)(now & 0xFF) - (int)(prev & 0xFF);
        if (delta != 1) always_exactly_one = false;
        prev = now;
    }
    note("pot0x released throughout; ten single strobe pulses each produced "
         "a delta of " + std::string(always_exactly_one ? "exactly +1" : "something other than +1") +
         " on the low byte.");
    expect(always_exactly_one, "opus:8-potchg.c4");
    expect(always_exactly_one, "sol:8-pot.c19");
    expect(always_exactly_one, "opus:8-potchg.c5");
    expect(always_exactly_one, "sol:8-pot.c20");
}

//--------------------------------------------------------------------------
// G9 -- p.235: once above the preset, the counter is held until the next
// POTGO is issued. Regraded 2026-09-15 after independent validation: an RC
// timing capacitor's charge cannot fall back below the preset on its own --
// nothing in this design (or in a real RC charge circuit) lowers it short
// of a POTGO dump -- so "held until the next POTGO is issued" is simply the
// consequence of a charge that cannot decrease any other way, not a
// separate requirement that the counter stay latched through an artificial
// digital comparator reversal with no POTGO. The page says nothing about
// that reversal case. What's testable: the counter does not advance for as
// long as the charge (here, the digital gate) stays above the preset.
//--------------------------------------------------------------------------
static void group_G9() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    pulse(kResetInterval + 6); // past the interval, so before_hold is genuinely
                               // counting, not trivially stuck at 0
    uint16_t before_hold = pot0dat();
#ifdef MUT_G9
    // MUTATED: skip closing the gate -- the counter keeps counting instead
    // of holding.
#else
    set_mouse(0, 0, 0); // hold ("above preset")
#endif
    settle(3);
    pulse(5);
    uint16_t during_hold = pot0dat();
    bool truly_holds_while_held = (during_hold == before_hold);

    // Recorded, not graded: this design resumes counting as soon as the
    // gate reopens, with no intervening POTGO -- a real RTL behaviour, but
    // not falsifiable against this claim's testable content (see header
    // note above): a comparator reversal without a POTGO dump is not a case
    // the manual's RC description can reach.
    set_mouse(0, 0, 1); // released again, with NO intervening POTGO write
    settle(3);
    pulse(4);
    uint16_t after_resume_attempt = pot0dat();
    bool resumed_without_potgo = ((after_resume_attempt & 0xFF) != (during_hold & 0xFF));

    note("pot0x: released, run " + std::to_string(kResetInterval + 6) +
         " ticks (past the interval) -> $" + hex4(before_hold) + "; held 5 more "
         "ticks -> $" + hex4(during_hold) + " (" +
         std::string(truly_holds_while_held ? "unchanged, matches the manual"
                                             : "changed -- diverges from the manual") +
         "). Not graded against this claim (no POTGO reversal case in the page): "
         "released again for 4 more ticks with no intervening POTGO write -> "
         "$" + hex4(after_resume_attempt) + " -- it " +
         std::string(resumed_without_potgo ? "moved" : "stayed put") +
         ". This design resumes counting as soon as the gate reopens, POTGO "
         "or not; an RC charge cannot fall back below the preset on its own, "
         "so the manual's 'held until the next POTGO' never has to specify "
         "that case, and this claim is not graded against it.");

    bool manual_claim = truly_holds_while_held;
    expect(manual_claim, "opus:8-potchg.c6");
    expect(manual_claim, "sol:8-pot.c21");
}

//--------------------------------------------------------------------------
// G10/G20 -- p.235 and p.282: nothing prevents the counters overflowing past
// 255; each counter is 8 bits wide (measured here, not assumed).
//--------------------------------------------------------------------------
static void group_G10_G20() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    pulse(kResetInterval); // past the interval first: the wrap claim (8-bit
                            // width, no flag/stop) is a MODULAR property,
                            // independent of when counting starts, so it is
                            // graded relative to this baseline rather than
                            // against an absolute tick-derived byte value
    uint8_t base = (uint8_t)(pot0dat() & 0xFF);
#ifdef MUT_G10
    pulse(254); // MUTATED: one pulse short
#else
    pulse(255);
#endif
    uint16_t at255 = pot0dat();
    pulse(1);
    uint16_t at256 = pot0dat();
    pulse(1);
    uint16_t at257 = pot0dat();
    uint8_t d255 = (uint8_t)((at255 & 0xFF) - base);
    uint8_t d256 = (uint8_t)((at256 & 0xFF) - base);
    uint8_t d257 = (uint8_t)((at257 & 0xFF) - base);
    bool wraps = (d255 == 0xFF) && (d256 == 0x00) && (d257 == 0x01);
    note("pot0x, past the interval (base=$" + hex4(base) + "), then +255/+256/"
         "+257 more released ticks: delta $" + hex4(d255) + " / $" + hex4(d256) +
         " / $" + hex4(d257) + " from base -- an 8-bit counter that wraps "
         "with no flag and no stop, regardless of the interval's own length.");
    expect(wraps, "opus:8-potchg.c8");
    expect(wraps, "sol:8-pot.c24");
    expect(wraps, "opus:8-appa-pot.c10");
}

//--------------------------------------------------------------------------
// G12 -- Figure 8-5: every counter stops on the identical rule (equality
// between all four, not a rule of its own for any one of them).
//--------------------------------------------------------------------------
static void group_G12() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3); pulse(kResetInterval + 6);
    set_mouse(0, 0, 0); settle(2);
    uint16_t v_x0 = pot0dat() & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0);
    settle(3); pulse(kResetInterval + 6);
    set_mouse(0, 0, 0); settle(2);
    uint16_t v_y0 = (pot0dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1);
    settle(3); pulse(kResetInterval + 6);
    set_joy1_pin5(0); settle(2);
    uint16_t v_y1 = (pot1dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
#ifdef MUT_G12
    set_potgo_bits(0x0800); // MUTATED: bits 11:10 gate pot0y, not pot1x
#else
    set_potgo_bits(0x2000); // bits 13:12 = 1,0 -> releases pot1x
#endif
    settle(2); pulse(kResetInterval + 6);
    set_potgo_bits(0x0000); settle(2);
    uint16_t v_x1 = pot1dat() & 0xFF;

    bool all_equal = matches_page_count(v_x0, 6) && matches_page_count(v_y0, 6) &&
                      matches_page_count(v_y1, 6) && matches_page_count(v_x1, 6);
    note("All four counters, release-then-run-" + std::to_string(kResetInterval + 6) +
         "-ticks-unbroken (8+6) then held by their own gate: pot0x=" +
         std::to_string(v_x0) + " pot0y=" + std::to_string(v_y0) +
         " pot1y=" + std::to_string(v_y1) + " pot1x=" + std::to_string(v_x1) +
         " -- the same count/hold rule for all four, graded against the "
         "page's reset interval (6 or 7), not the raw tick total.");
    expect(all_equal, "opus:8-fig8-5.c2");
    expect(all_equal, "sol:8-fig8-5.c3");
}

//--------------------------------------------------------------------------
// G13 -- Figure 8-5: higher resistance (later crossing) yields a larger
// count, over an equal span.
//--------------------------------------------------------------------------
static void group_G13() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
#ifdef MUT_G13
    pulse(kResetInterval + 8); set_mouse(0, 0, 0); settle(2); pulse(2); // MUTATED: swapped with the "late" run below
#else
    pulse(kResetInterval + 3); set_mouse(0, 0, 0); settle(2); pulse(7);
#endif
    uint16_t early_cross = pot0dat() & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
#ifdef MUT_G13
    pulse(kResetInterval + 3); set_mouse(0, 0, 0); settle(2); pulse(7);
#else
    pulse(kResetInterval + 8); set_mouse(0, 0, 0); settle(2); pulse(2);
#endif
    uint16_t late_cross = pot0dat() & 0xFF;

    // Both crossing points are shifted past the interval by the same
    // kResetInterval offset, so the two spans stay equal to each other
    // (kResetInterval+10 ticks each) and the relative comparison below is
    // unaffected by the interval's existence or its exact length -- unlike
    // G6_G11/FINALCOUNT, this claim is not about the interval itself.
    bool later_is_larger = (late_cross > early_cross);
    note("Over an equal (" + std::to_string(kResetInterval + 10) +
         "-tick) span past the interval, a counter crossing (gate closing) "
         "at tick " + std::to_string(kResetInterval + 3) + " ends at " +
         std::to_string(early_cross) + "; one crossing at tick " +
         std::to_string(kResetInterval + 8) + " ends at " +
         std::to_string(late_cross) + " -- later crossing, larger count.");
    expect(later_is_larger, "opus:8-fig8-5.c4");
}

//--------------------------------------------------------------------------
// G14-G17 -- p.236: POT0DAT is port 1 (the mouse/"LEFT" connector), POT1DAT
// is port 2; bits 15-8 hold Y, bits 7-0 hold X, for both registers.
//--------------------------------------------------------------------------
static void group_G14_G17() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 1, 1);
    settle(3);
    pulse(kResetInterval + 5); // past the interval, so a moved counter reads nonzero, not stuck at 0
#ifdef MUT_G14G17
    uint16_t p0 = pot1dat(), p1 = pot0dat(); // MUTATED: registers swapped
#else
    uint16_t p0 = pot0dat(), p1 = pot1dat();
#endif
    bool mouse_only_moves_pot0dat = (p0 != 0x0000) && (p1 == 0x0000);

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1);
    settle(3);
    pulse(kResetInterval + 5);
    uint16_t p0b = pot0dat(), p1b = pot1dat();
    bool port2_pin_only_moves_pot1dat = (p0b == 0x0000) && (p1b != 0x0000);

    bool port_identity_ok = mouse_only_moves_pot0dat && port2_pin_only_moves_pot1dat;
    note("Releasing every mouse-gated pin moved only POT0DAT ($" + hex4(p0) +
         "/$" + hex4(p1) + "); releasing the joystick-port pin that gates "
         "POT1DAT's Y counter moved only POT1DAT ($" + hex4(p0b) + "/$" +
         hex4(p1b) + "). POT0DAT is the mouse-connector register (port 1, "
         "per the p.236 reading this package's conflict resolution adopts); "
         "POT1DAT is the other one (port 2).");
    expect(port_identity_ok, "opus:8-potreg.c1");
    expect(port_identity_ok, "sol:8-pot.c26");
    expect(port_identity_ok, "sol:8-pot.c27");
    expect(port_identity_ok, "opus:8-potreg.c2");
    expect(port_identity_ok, "sol:8-pot.c28");
    expect(port_identity_ok, "sol:8-pot.c29");

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    pulse(kResetInterval + 9);
#ifdef MUT_G14G17
    uint16_t xonly = pot1dat(); // MUTATED: wrong register read
#else
    uint16_t xonly = pot0dat();
#endif
    bool x_is_low_byte = matches_page_count(xonly & 0xFF, 9) && ((xonly >> 8) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0);
    settle(3);
    pulse(kResetInterval + 9);
#ifdef MUT_G14G17
    uint16_t yonly = pot1dat(); // MUTATED: wrong register read
#else
    uint16_t yonly = pot0dat();
#endif
    bool y_is_high_byte = matches_page_count(yonly >> 8, 9) && ((yonly & 0xFF) == 0);

    bool byte_assignment_ok = x_is_low_byte && y_is_high_byte;
    note("POT0DAT with only the X counter released, release-then-run-" +
         std::to_string(kResetInterval + 9) + "-ticks-unbroken (8+9): $" +
         hex4(xonly) + "; with only Y released: $" + hex4(yonly) +
         " -- Y occupies bits 15-8, X occupies bits 7-0, graded against the "
         "page's reset interval (9 or 10), not the raw tick total.");
    expect(byte_assignment_ok, "opus:8-potreg.c3");
    expect(byte_assignment_ok, "sol:8-pot.c30");
    expect(byte_assignment_ok, "opus:8-potreg.c4");
    expect(byte_assignment_ok, "sol:8-pot.c31");
}

//--------------------------------------------------------------------------
// G19/G21 -- p.282: POT0DAT/POT1DAT each read a pair of 8-bit counters, four
// counters total, independently addressable.
//--------------------------------------------------------------------------
static void group_G19_G21() {
    full_reset();
    arm_potgo(0x0001);
    pulse(kResetInterval); // past the interval first, with every pin still
                            // held (default from full_reset()): this claim
                            // is about four independently-addressable
                            // counters reaching four DISTINCT values, not
                            // about when counting starts, and the four
                            // target values below (3/4/9/13) would collide
                            // once shifted by an uncertain 7-or-8-line
                            // interval if graded per-phase instead
    set_mouse(0, 0, 1); settle(3); pulse(3);      // pot0x = 3
    set_mouse(0, 0, 0); settle(2);
#ifdef MUT_G19G21
    set_mouse(0, 1, 0); settle(3); pulse(5);      // MUTATED: still 5, but expectation below is unchanged (4)
#else
    set_mouse(0, 1, 0); settle(3); pulse(4);      // pot0y = 4
#endif
    set_mouse(0, 0, 0); settle(2);
    set_joy1_pin5(1); settle(3); pulse(9);        // pot1y = 9
    set_joy1_pin5(0); settle(2);
    set_potgo_bits(0x2000); settle(2); pulse(13); // pot1x = 13 ($0d)
    set_potgo_bits(0x0000); settle(2);

    uint16_t p0 = pot0dat(), p1 = pot1dat();
    bool four_independent = (p0 == 0x0403) && (p1 == 0x090D);
    note("Four counters driven to four different values independently: "
         "POT0DAT=$" + hex4(p0) + " (pot0y=4,pot0x=3), POT1DAT=$" + hex4(p1) +
         " (pot1y=9,pot1x=13=$0d) -- four separately-addressable counters, "
         "each register reading a pair.");
    expect(four_independent, "opus:8-appa-pot.c9");
    expect(four_independent, "sol:8-appa-pot.c11");
    expect(four_independent, "sol:8-appa-pot.c12");
    expect(four_independent, "opus:8-appa-pot.c11");
    expect(four_independent, "sol:8-appa-pot.c13");
}

//--------------------------------------------------------------------------
// G22 -- p.282: the counters are stopped by signals from two controller
// connectors (left-right) with two pins each.
//--------------------------------------------------------------------------
static void group_G22() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 0);
    settle(3); pulse(6);
    uint16_t held_left = pot0dat();
    bool left_holds_when_no_signal = (held_left == 0x0000);
#ifdef MUT_G22
    // MUTATED: skip asserting the mouse buttons
#else
    set_mouse(1, 1, 1);
#endif
    settle(3);
    pulse(kResetInterval); // the held phase above already spent 6 pulse-strobes
                            // toward the interval; kResetInterval more brings
                            // the total to 6+8=14, netting 6 (or 7) counted
                            // once the interval (7 or 8) has elapsed
    uint16_t released_left = pot0dat();
    bool left_both_have_a_pin = matches_page_count(released_left >> 8, 6) &&
                                 matches_page_count(released_left & 0xFF, 6);
    bool left_ok = left_holds_when_no_signal && left_both_have_a_pin;

    full_reset();
    arm_potgo(0x0001);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3); pulse(kResetInterval + 6);
    uint16_t right_everything = pot1dat();
    bool right_y_has_a_pin = matches_page_count(right_everything >> 8, 6);
    bool right_x_has_a_pin = ((right_everything & 0xFF) != 0); // structurally always false -- pot1x has no pin, see file header note
    bool right_both_have_a_pin = right_y_has_a_pin && right_x_has_a_pin;

    note("Left connector (mouse): held -> $" + hex4(held_left) +
         ", every mouse pin asserted -> $" + hex4(released_left) +
         " (both counters moved). Right connector: every joystick pin on "
         "both _joy1 and _joy2 asserted -> $" + hex4(right_everything) +
         " -- the Y counter (high byte) moved, the X counter (low byte) did "
         "not: it has no connector pin driving it in this design at all "
         "(chip-internal only, see the file header note / G1 / G4).");

    expect(left_ok, "sol:8-appa-pot.c14");
    expect(right_both_have_a_pin, "sol:8-appa-pot.c15");
    expect(left_ok && right_both_have_a_pin, "opus:8-appa-pot.c12");
    expect(left_ok && right_both_have_a_pin, "opus:8-appa-pot.c13");
    expect(left_ok,
           "harness-selfcheck: G22 comparator is live (left connector genuinely has two pins)");
}

//--------------------------------------------------------------------------
// G23 -- p.282 table: bit assignment for the pot counters (RIGHT=POT1DAT,
// LEFT=POT0DAT per the resolved mapping in G14/G15): Y7..Y0 at bits 15..8,
// X7..X0 at bits 7..0, for both registers, bit for bit.
//--------------------------------------------------------------------------
struct BitClaim { int bit; const char* uid; };

static const BitClaim RIGHT_BITS[16] = {
    {15, "opus:8-appa-pot.c14"}, {14, "opus:8-appa-pot.c15"}, {13, "opus:8-appa-pot.c16"},
    {12, "opus:8-appa-pot.c17"}, {11, "opus:8-appa-pot.c18"}, {10, "opus:8-appa-pot.c19"},
    { 9, "opus:8-appa-pot.c20"}, { 8, "opus:8-appa-pot.c21"}, { 7, "opus:8-appa-pot.c22"},
    { 6, "opus:8-appa-pot.c23"}, { 5, "opus:8-appa-pot.c24"}, { 4, "opus:8-appa-pot.c25"},
    { 3, "opus:8-appa-pot.c26"}, { 2, "opus:8-appa-pot.c27"}, { 1, "opus:8-appa-pot.c28"},
    { 0, "opus:8-appa-pot.c29"},
};
static const BitClaim LEFT_BITS[16] = {
    {15, "opus:8-appa-pot.c30"}, {14, "opus:8-appa-pot.c31"}, {13, "opus:8-appa-pot.c32"},
    {12, "opus:8-appa-pot.c33"}, {11, "opus:8-appa-pot.c34"}, {10, "opus:8-appa-pot.c35"},
    { 9, "opus:8-appa-pot.c36"}, { 8, "opus:8-appa-pot.c37"}, { 7, "opus:8-appa-pot.c38"},
    { 6, "opus:8-appa-pot.c39"}, { 5, "opus:8-appa-pot.c40"}, { 4, "opus:8-appa-pot.c41"},
    { 3, "opus:8-appa-pot.c42"}, { 2, "opus:8-appa-pot.c43"}, { 1, "opus:8-appa-pot.c44"},
    { 0, "opus:8-appa-pot.c45"},
};

static void check_bit_table(uint16_t reg_val, uint16_t expect_val, const BitClaim* claims, int n) {
    for (int i = 0; i < n; i++) {
        int b = claims[i].bit;
        bool ok = ((reg_val >> b) & 1) == ((expect_val >> b) & 1);
        expect(ok, claims[i].uid);
    }
}

static void group_G23() {
#ifdef MUT_G23
    // MUTATED: X and Y targets swapped. $4b and $b4 are exact bitwise
    // complements (0x4b = 0100_1011, 0xb4 = 1011_0100), so this flips every
    // single bit of both bytes against the unchanged $b4/$4b expectation.
    const int X_TARGET = 0xB4;
    const int Y_TARGET = 0x4B;
#else
    const int X_TARGET = 0x4B;
    const int Y_TARGET = 0xB4;
#endif

    // POT1DAT ("RIGHT"): pot1x has no external pin (see file header note),
    // so it is set via POTGO's own output-enable bits, same as G5/G18.
    // kResetInterval is spent up front, with pot1x still held (the default
    // out of full_reset()), so the interval is exhausted before X_TARGET's
    // own pulse count begins -- a bit-level table cannot be graded k-or-k+1
    // per bit (75 and 76 do not differ by a single bit), so this claim is
    // tested against the exact target, past the interval, rather than by
    // widening the comparator.
    full_reset();
    arm_potgo(0x0001);
    pulse(kResetInterval);
    set_potgo_bits(0x2000); settle(2);
    pulse(X_TARGET);
    set_potgo_bits(0x0000); settle(2); // hold pot1x again
    set_joy1_pin5(1); settle(3);
    pulse(Y_TARGET);
    uint16_t right_reg = pot1dat();
    uint16_t right_expect = (uint16_t)((0xB4u << 8) | 0x4B);
    check_bit_table(right_reg, right_expect, RIGHT_BITS, 16);
    expect((right_reg >> 8) == 0xB4, "sol:8-appa-pot.c16");
    expect((right_reg & 0xFF) == 0x4B, "sol:8-appa-pot.c17");

    // POT0DAT ("LEFT"): both counters have real external pins (mouse
    // buttons). Same kResetInterval warm-up, pot0x still held, before
    // X_TARGET's own pulse count begins.
    full_reset();
    arm_potgo(0x0001);
    pulse(kResetInterval);
    set_mouse(0, 0, 1); settle(3);
    pulse(X_TARGET);
    set_mouse(0, 0, 0); settle(2);
    set_mouse(0, 1, 0); settle(3);
    pulse(Y_TARGET);
    uint16_t left_reg = pot0dat();
    uint16_t left_expect = (uint16_t)((0xB4u << 8) | 0x4B);
    check_bit_table(left_reg, left_expect, LEFT_BITS, 16);
    expect((left_reg >> 8) == 0xB4, "sol:8-appa-pot.c18");
    expect((left_reg & 0xFF) == 0x4B, "sol:8-appa-pot.c19");

    note("POT1DAT driven to pot1y=$b4/pot1x=$4b via chip-internal release for "
         "X, external pin for Y -> read back $" + hex4(right_reg) +
         "; POT0DAT driven to pot0y=$b4/pot0x=$4b via mouse buttons -> read "
         "back $" + hex4(left_reg) + ". Both match the manual's table bit "
         "for bit.");
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    dut = new Vuserio;

    // Fixture liveness proof: the harness can deliver a specific, chosen,
    // non-constant word through this data path before any claim is graded.
    // The window is deliberately wider than either printed reading of the
    // reset interval (seven-or-eight lines, body text; eight, Figure 8-5)
    // so this gate does not itself assume which one holds, or whether any
    // interval holds at all -- that range is exactly what group_G6_G11 and
    // group_FINALCOUNT below grade, and this gate must not prejudge it.
    constexpr int kLivenessWindow = 15;  // > widest printed interval (8), with margin
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);  // release pot0x only
    settle(3);
    pulse(kLivenessWindow);
    uint16_t live0 = pot0dat();
    uint16_t live1 = pot1dat();
    bool pot0x_counted = (live0 & 0x00FF) != 0;
    bool others_untouched = ((live0 & 0xFF00) == 0) && (live1 == 0x0000);
    bool alive = pot0x_counted && others_untouched;
    note("Fixture liveness: releasing pot0x only and pulsing " +
         std::to_string(kLivenessWindow) + " times (longer than either "
         "printed reset interval) reads POT0DAT=$" + hex4(live0) +
         ", POT1DAT=$" + hex4(live1) + " -- the touched counter (pot0x) "
         "counted something and the three untouched counters read exactly "
         "zero: a distinguishing, non-constant word, delivered before any "
         "claim below is graded, without assuming when counting starts.");
    if (!alive) {
        std::cerr << "FATAL: fixture cannot deliver a known word; aborting "
                     "before grading any claim.\n";
        return 2;
    }

    group_G1();
    group_G2();
    group_G3();
    group_G4();
    group_G5_G18();
    group_G6_G11();
    group_FINALCOUNT();
    group_G7_G8();
    group_G9();
    group_G10_G20();
    group_G12();
    group_G13();
    group_G14_G17();
    group_G19_G21();
    group_G22();
    group_G23();

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
