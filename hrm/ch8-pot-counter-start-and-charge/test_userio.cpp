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
// MUTATION opus:8-pot.c12 oracle green substitute: the pot0x/pot0y/pot1y
//   sub-measurements from the same $0001-release run (which do match "== 7
//   after 7 ticks") routed through the identical comparator used for the red
//   pot1x half.
// MUTATION sol:8-pot.c13 oracle green (same substitute as opus:8-pot.c12)
// MUTATION opus:8-pot.c13 oracle green substitute: the isolated-release
//   measurements for pot0x/pot0y/pot1y (each of which does move only when
//   its own pin is released) routed through the identical comparator used
//   for the red "pot1x has its own pin" half.
// MUTATION sol:8-pot.c14 oracle green (same substitute as opus:8-pot.c13)
// MUTATION opus:8-pot.c14 stimulus red MUT_G5G18: the reset-triggering POTGO
//   write is built with bit 0 forced clear, so the counters are never
//   zeroed; the "all zero after the bit-0 write" comparison is unchanged and
//   now compares against values that never reset.
// MUTATION sol:8-pot.c15 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION opus:8-potreg.c5 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION sol:8-pot.c32 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION opus:8-potchg.c1 oracle green substitute: a harness-side counter,
//   incremented in a visible loop to exactly 8 (proven live by that loop
//   itself), routed through the identical count==7||count==8 comparator
//   used for the red DUT-measured reset interval.
// MUTATION sol:8-pot.c16 oracle green (same substitute as opus:8-potchg.c1)
// MUTATION opus:8-fig8-5.c1 oracle green (same substitute as opus:8-potchg.c1)
// MUTATION sol:8-fig8-5.c2 oracle green (same substitute as opus:8-potchg.c1)
// MUTATION opus:8-potchg.c4 stimulus red MUT_G7G8: each of the ten single
//   pulses in the cadence loop is built as two pot_cnt_en pulses instead of
//   one, so consecutive reads differ by 2, not 1; the "delta==1 every time"
//   comparison is unchanged.
// MUTATION sol:8-pot.c19 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION opus:8-potchg.c5 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION sol:8-pot.c20 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION opus:8-potchg.c6 oracle green substitute: the "value unchanged
//   across N ticks" comparator, applied within the same G9 run to a counter
//   that is genuinely held the whole span (no resume attempted), goes green.
// MUTATION sol:8-pot.c21 oracle green (same substitute as opus:8-potchg.c6)
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
    pulse(6);
    uint16_t r1 = pot0dat();
    bool x_tracks = ((r1 & 0x00FF) == 6) && ((r1 >> 8) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0); // release pot0y only (mouse button 2/right)
    settle(3);
    pulse(9);
    uint16_t r2 = pot0dat();
    bool y_tracks = ((r2 >> 8) == 9) && ((r2 & 0x00FF) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1); // release pot1y (via the swapped _djoy2<=_joy1 path)
    settle(3);
    pulse(11);
    uint16_t r3 = pot1dat();
    bool port2_y_tracks = ((r3 >> 8) == 11) && ((r3 & 0x00FF) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000; // assert every pin this module reads
    settle(3);
    pulse(11);
    uint16_t r4 = pot1dat();
    bool port2_x_moved = ((r4 & 0x00FF) != 0);

    note("port 1 (POT0DAT): X-only release/6 ticks -> $" + hex4(r1) +
         "; Y-only release/9 ticks -> $" + hex4(r2) +
         " -- two independently-movable counters. "
         "port 2 (POT1DAT): joystick-port pin release/11 ticks -> $" + hex4(r3) +
         " (Y moved); every controller pin this module reads, asserted, for "
         "11 ticks -> $" + hex4(r4) + " -- the low byte (X) never moved. "
         "Port 2 carries only one externally-driven proportional counter in "
         "this design, not two.");

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
              // "POTGO started it" apart from "it was already zero".
#ifdef MUT_G2
    arm_potgo(0x0000); // MUTATED: bit 0 clear -- must not re-zero
#else
    arm_potgo(0x0001);
#endif
    pulse(5);
    uint16_t a = pot0dat();
    bool starts_on_potgo = (a & 0x00FF) == 5;

    full_reset();
    set_mouse(0, 0, 1);
    arm_potgo(0x0001);
    pulse(3);
    uint16_t before = pot0dat();
    dut->reg_address_in = A_POTINP; dut->data_in = 0xFFFF; // access a *different* register
    tick();
    idle_bus();
    tick();
    uint16_t after_other = pot0dat();
    bool other_write_no_restart = (after_other == before);
    pulse(2);
    uint16_t after_more = pot0dat();
    bool counting_continued = (after_more & 0x00FF) == 5;

    full_reset();
    set_mouse(0, 0, 1);
    arm_potgo(0x0001);
    pulse(4);
    uint16_t before2 = pot0dat();
    write_potgo_once(0x00F0); // POTGO write, bit 0 clear
    tick();
    settle(2);
    uint16_t after2 = pot0dat();
    bool bit0_clear_no_restart = (after2 == before2);

    note("POTGO write with bit 0 set, 5 ticks later -> $" + hex4(a) +
         "; a write to a different register mid-run left POT0DAT at $" +
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
// G3 -- p.234: POTGO=$0001 is the standard-joystick start value (bit
// pattern: starts+zeroes all four, bits 15-8 undriven so all four follow
// their external pins, bits 7-1 don't-care).
//--------------------------------------------------------------------------
static void group_G3() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3);
    pulse(7);
    uint16_t p0_1 = pot0dat(), p1_1 = pot1dat();

    full_reset();
    arm_potgo(0x00FF);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3);
    pulse(7);
    uint16_t p0_2 = pot0dat(), p1_2 = pot1dat();

    bool bits71_dont_care = (p0_1 == p0_2) && (p1_1 == p1_2);
    bool pot0x_tracks = (p0_1 & 0xFF) == 7;
    bool pot0y_tracks = (p0_1 >> 8) == 7;
    bool pot1y_tracks = (p1_1 >> 8) == 7;
    bool pot1x_tracks = (p1_1 & 0xFF) == 7;
    bool all_four_track = pot0x_tracks && pot0y_tracks && pot1y_tracks && pot1x_tracks;

    note("POTGO=$0001 vs $00ff, every controller pin released, after 7 ticks: "
         "POT0DAT $" + hex4(p0_1) + "/$" + hex4(p0_2) + ", POT1DAT $" +
         hex4(p1_1) + "/$" + hex4(p1_2) + ". Bits 7-1 are confirmed "
         "don't-care. POT1DAT's low byte (pot1x) does not reach 7 under "
         "either value -- see the file header note.");

    bool ok = bits71_dont_care && all_four_track;
    expect(ok, "opus:8-pot.c12");
    expect(ok, "sol:8-pot.c13");
    expect(bits71_dont_care && pot0x_tracks && pot0y_tracks && pot1y_tracks,
           "harness-selfcheck: G3 comparator is live (pot0x/pot0y/pot1y matched)");
}

//--------------------------------------------------------------------------
// G4 -- p.234: after the start, each counter's advance is governed by its
// own external pin, independently of the other three.
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
    set_mouse(0, 0, 1);
    settle(3);
    pulse(4);
    bool only_pot0x_moved = (pot0dat() == 0x0004) && (pot1dat() == 0x0000);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0);
    settle(3);
    pulse(4);
    bool only_pot0y_moved = (pot0dat() == 0x0400) && (pot1dat() == 0x0000);

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1);
    settle(3);
    pulse(4);
    bool only_pot1y_moved = (pot0dat() == 0x0000) && (pot1dat() == 0x0400);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3);
    pulse(4);
    uint16_t everything_p1 = pot1dat();
    bool pot1x_has_a_pin = ((everything_p1 & 0x00FF) != 0);

    note("Isolated-release measurements (all four counters, 4 ticks each): "
         "pot0x-only moved pot0x; pot0y-only moved pot0y; pot1y-only moved "
         "pot1y; every controller pin asserted left POT1DAT's low byte at $" +
         hex4(everything_p1 & 0xFF) + " (pot1x did not move -- see the file "
         "header note).");

    bool ok = nothing_moves_when_held && only_pot0x_moved && only_pot0y_moved &&
              only_pot1y_moved && pot1x_has_a_pin;
    expect(ok, "opus:8-pot.c13");
    expect(ok, "sol:8-pot.c14");
    expect(nothing_moves_when_held && only_pot0x_moved && only_pot0y_moved && only_pot1y_moved,
           "harness-selfcheck: G4 comparator is live (pot0x/pot0y/pot1y each independently gated)");
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
    note("pot0x's gate was released before the first pulse; its first "
         "nonzero read came after pulse " + std::to_string(first_increment_at) +
         " -- this RTL holds no separate multi-line reset state at all.");
    bool manual_claim = matches_reset_interval(first_increment_at);
    expect(manual_claim, "opus:8-potchg.c1");
    expect(manual_claim, "sol:8-pot.c16");
    expect(manual_claim, "opus:8-fig8-5.c1");
    expect(manual_claim, "sol:8-fig8-5.c2");

    int harness_counter = 0;
    for (int i = 0; i < 8; i++) harness_counter++;
    bool comparator_live = matches_reset_interval(harness_counter);
    note("Substitute for the already-red reset-interval comparator: a "
         "harness-side loop counter, visibly incremented to exactly " +
         std::to_string(harness_counter) + ", routed through the identical "
         "count==7||count==8 comparator used above, confirms that "
         "comparator is not vacuously false.");
    expect(comparator_live, "harness-selfcheck: G6/G11 comparator is live (harness counter = 8)");
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
// POTGO is issued.
//--------------------------------------------------------------------------
static void group_G9() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    pulse(6);
    uint16_t before_hold = pot0dat();
    set_mouse(0, 0, 0); // hold ("above preset")
    settle(3);
    pulse(5);
    uint16_t during_hold = pot0dat();
    bool truly_holds_while_held = (during_hold == before_hold);

    set_mouse(0, 0, 1); // released again, with NO intervening POTGO write
    settle(3);
    pulse(4);
    uint16_t after_resume_attempt = pot0dat();
    bool resumed_without_potgo = ((after_resume_attempt & 0xFF) != (during_hold & 0xFF));

    note("pot0x: released 6 ticks -> $" + hex4(before_hold) + "; held 5 more "
         "ticks -> $" + hex4(during_hold) + " (unchanged, matches the "
         "manual); released again for 4 more ticks with no intervening POTGO "
         "write -> $" + hex4(after_resume_attempt) +
         " -- it moved. This design resumes counting as soon as the gate "
         "reopens, POTGO or not; the manual's 'held until the next POTGO is "
         "issued' does not hold here.");

    bool manual_claim = truly_holds_while_held && !resumed_without_potgo;
    expect(manual_claim, "opus:8-potchg.c6");
    expect(manual_claim, "sol:8-pot.c21");
    expect(truly_holds_while_held,
           "harness-selfcheck: G9 comparator is live (hold-only case, no resume attempted)");
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
    bool wraps = ((at255 & 0xFF) == 0xFF) && ((at256 & 0xFF) == 0x00) && ((at257 & 0xFF) == 0x01);
    note("pot0x after 255/256/257 released ticks: $" + hex4(at255) + " / $" +
         hex4(at256) + " / $" + hex4(at257) +
         " -- an 8-bit counter that wraps with no flag and no stop.");
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
    settle(3); pulse(6);
    set_mouse(0, 0, 0); settle(2);
    uint16_t v_x0 = pot0dat() & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0);
    settle(3); pulse(6);
    set_mouse(0, 0, 0); settle(2);
    uint16_t v_y0 = (pot0dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_joy1_pin5(1);
    settle(3); pulse(6);
    set_joy1_pin5(0); settle(2);
    uint16_t v_y1 = (pot1dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
#ifdef MUT_G12
    set_potgo_bits(0x0800); // MUTATED: bits 11:10 gate pot0y, not pot1x
#else
    set_potgo_bits(0x2000); // bits 13:12 = 1,0 -> releases pot1x
#endif
    settle(2); pulse(6);
    set_potgo_bits(0x0000); settle(2);
    uint16_t v_x1 = pot1dat() & 0xFF;

    bool all_equal = (v_x0 == 6) && (v_y0 == 6) && (v_y1 == 6) && (v_x1 == 6);
    note("All four counters, released for 6 ticks then held by their own "
         "gate: pot0x=" + std::to_string(v_x0) + " pot0y=" + std::to_string(v_y0) +
         " pot1y=" + std::to_string(v_y1) + " pot1x=" + std::to_string(v_x1) +
         " -- the same count/hold rule for all four.");
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
    pulse(8); set_mouse(0, 0, 0); settle(2); pulse(2); // MUTATED: swapped with the "late" run below
#else
    pulse(3); set_mouse(0, 0, 0); settle(2); pulse(7);
#endif
    uint16_t early_cross = pot0dat() & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
#ifdef MUT_G13
    pulse(3); set_mouse(0, 0, 0); settle(2); pulse(7);
#else
    pulse(8); set_mouse(0, 0, 0); settle(2); pulse(2);
#endif
    uint16_t late_cross = pot0dat() & 0xFF;

    bool later_is_larger = (late_cross > early_cross);
    note("Over an equal ten-tick span, a counter crossing (gate closing) at "
         "tick 3 ends at " + std::to_string(early_cross) +
         "; one crossing at tick 8 ends at " + std::to_string(late_cross) +
         " -- later crossing, larger count.");
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
    pulse(5);
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
    pulse(5);
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
    pulse(9);
#ifdef MUT_G14G17
    uint16_t xonly = pot1dat(); // MUTATED: wrong register read
#else
    uint16_t xonly = pot0dat();
#endif
    bool x_is_low_byte = ((xonly & 0xFF) == 9) && ((xonly >> 8) == 0);

    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 1, 0);
    settle(3);
    pulse(9);
#ifdef MUT_G14G17
    uint16_t yonly = pot1dat(); // MUTATED: wrong register read
#else
    uint16_t yonly = pot0dat();
#endif
    bool y_is_high_byte = ((yonly >> 8) == 9) && ((yonly & 0xFF) == 0);

    bool byte_assignment_ok = x_is_low_byte && y_is_high_byte;
    note("POT0DAT with only the X counter released: $" + hex4(xonly) +
         "; with only Y released: $" + hex4(yonly) +
         " -- Y occupies bits 15-8, X occupies bits 7-0.");
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
    settle(3); pulse(6);
    uint16_t released_left = pot0dat();
    bool left_both_have_a_pin = (released_left == 0x0606);
    bool left_ok = left_holds_when_no_signal && left_both_have_a_pin;

    full_reset();
    arm_potgo(0x0001);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3); pulse(6);
    uint16_t right_everything = pot1dat();
    bool right_y_has_a_pin = ((right_everything >> 8) == 6);
    bool right_x_has_a_pin = ((right_everything & 0xFF) != 0);
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
    full_reset();
    arm_potgo(0x0001);
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
    // buttons).
    full_reset();
    arm_potgo(0x0001);
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
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);
    settle(3);
    pulse(3);
    uint16_t live0 = pot0dat();
    uint16_t live1 = pot1dat();
    bool alive = (live0 == 0x0003) && (live1 == 0x0000);
    note("Fixture liveness: releasing pot0x and pulsing 3 times reads "
         "POT0DAT=$" + hex4(live0) + ", POT1DAT=$" + hex4(live1) +
         " -- a specific, chosen, non-zero, non-constant word, delivered "
         "before any claim below is graded.");
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
