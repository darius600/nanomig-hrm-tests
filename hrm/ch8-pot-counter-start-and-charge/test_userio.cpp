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
// BIT 0 CLEAR, an open point. p.236 ("All counts are reset to zero when
// POTGO is written with bit zero high") is one-way, and p.234's sentence
// behind G2/G5/G18's ids has no bit-0 condition: no page says a POTGO
// write with bit 0 clear leaves the counters alone. The "bit 0 clear
// leaves them unchanged" halves of G2 and G5/G18 are therefore printed as
// MEASURED lines, not graded. G12, G19/G21 and G23 each need to close (or
// re-release) pot1x through a further POTGO write of their own after
// pot1x has already counted; each orders that write so it never falls
// between the start of pot1x's own count and the read that grades it, so
// none of the three depends on what an unspecified bit-0-clear write does.
//

// MUTATION opus:8-pot.c1 oracle green substitute: port2-X (the red half,
//   graded != 0) cannot itself take a stimulus mutation, so port1's X/Y and
//   port2's Y measurements (which do match, within the same G1 run) are
//   routed through their own "== value after N released ticks" comparators
//   instead -- not the != 0 comparator the red half uses -- confirming the
//   read/grade pipeline that feeds all four is not itself vacuous.
// MUTATION sol:8-pot.c1 oracle green (same substitute as opus:8-pot.c1)
// MUTATION harness-selfcheck stimulus red MUT_SELFCHECK: the POTGO arm
//   gate for pot0x is held closed in G1's first measurement, so it never
//   counts and x_tracks is false. Expected values untouched. RUN: baseline
//   76 passed 13 failed; mutated 75 passed 14 failed, exactly one
//   harness-selfcheck newly red, the thirteen baseline reds unchanged.
//   Reverted: 76/13.
// MUTATION opus:8-pot.c11 stimulus red MUT_G2: the SECOND POTGO write
//   (POTGO=$0001, the one this clause grades for zeroing) is omitted
//   entirely -- not a bit-0-clear write, which would rest on the page-open
//   bit-0-clear behaviour (see the file header) and move no line on a copy
//   that zeroes on bit-0-clear anyway; the FIRST POTGO write (the genuine
//   start that establishes before_write) is untouched. RUN: baseline 76
//   passed 13 failed; mutated 74 passed, 15 failed, exactly this pair
//   newly red. Reverted: 76/13. On u8, u8z, u8p and u8n (all four green at
//   baseline, 84 passed/5 failed): mutated 82 passed, 7 failed on each,
//   the same pair newly red.
// MUTATION sol:8-pot.c12 stimulus red (same MUT_G2 as opus:8-pot.c11)
// MUTATION opus:8-pot.c12 stimulus red MUT_G3 (this assertion's scope is
//   what the claim says: pot0x/pot0y/pot1y, the counters with a pin, track
//   under $0001 -- neither "$0001/$00FF behave identically" nor "all four
//   counters track" is in "For a standard X-Y joystick, this value is hex
//   0001" [p.282 calls bits 7-1 reserved, not don't-care]. GREEN, so it
//   takes a stimulus mutation, not an oracle substitute): pot0y's gate is
//   an AND of a mouse-side and a
//   swapped-joystick-side release signal (potcap[1] <= _mright & _djoy1[5]
//   & ...), so either alone can open it -- both are built held instead of
//   released (right mouse button unpressed AND _joy2 pin 5, which
//   _djoy1[5] tracks, left unasserted), so pot0y never reaches 7; the
//   "==7" expected value is unchanged. RUN: baseline 76 passed 13 failed;
//   mutated 74 passed, 15 failed, exactly this pair newly red. Reverted:
//   76/13.
// MUTATION sol:8-pot.c13 stimulus red (same MUT_G3 as opus:8-pot.c12)
// MUTATION opus:8-pot.c13 stimulus red MUT_G4 (this assertion's scope is
//   what the claim says: each counter that does have a pin tracks it once
//   started -- "starts special hardware that reads the potentiometer
//   values" says nothing about pin count; a four-independently-driven-pin
//   topology belongs to p.234's "two per port" sentence and p.282's
//   connector table, different uids. GREEN, so it takes a stimulus
//   mutation):
//   the pot0x-only-moved step is built without releasing pot0x's pin, so it
//   never moves; the "==4" expected value is unchanged. RUN: baseline 76
//   passed 13 failed; mutated 74 passed, 15 failed, exactly this pair newly
//   red. Reverted: 76/13.
// MUTATION sol:8-pot.c14 stimulus red (same MUT_G4 as opus:8-pot.c13)
// MUTATION opus:8-pot.c14 stimulus red MUT_G5G18: the reset-triggering
//   POTGO write (POTGO=$2001) is omitted entirely -- no POTGO write at
//   all, not a bit-0-clear write, which would rest on the page-open
//   bit-0-clear behaviour (see the file header) and move no line on a
//   copy that zeroes on bit-0-clear anyway; the "all zero after the write"
//   comparison is unchanged and now compares against values that were
//   never written. RUN: baseline 76 passed 13 failed; mutated 72 passed,
//   17 failed, exactly these four newly red. Reverted: 76/13. On u8 and
//   u8z (which zero on any bit-0-clear POTGO write): mutated 80 passed, 9
//   failed on both, the same four newly red.
// MUTATION sol:8-pot.c15 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION opus:8-potreg.c5 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION sol:8-pot.c32 stimulus red (same MUT_G5G18 as opus:8-pot.c14)
// MUTATION opus:8-potchg.c1 stimulus green MUT_G6HOLD: this assertion is RED
//   at baseline (pot0x's own gate opens immediately after POTGO on NanoMig,
//   so the first nonzero read comes at pulse 1 -- zero completed reset
//   lines; NanoMig implements no reset interval). Per this suite's rule
//   against mutating an already-failing check, the stimulus substitutes a
//   genuine, independently-live change directly inside group_G6_G11's own
//   measurement: pot0x's own pin is held through strobes 1-8 and released
//   before strobe 9, through the identical, unchanged
//   matches_reset_interval()/==8 comparators. RUN (make MUT=G6HOLD test):
//   baseline 76 passed, 13 failed; completed_reset_lines reads exactly 8,
//   all four G6/G11 lines FAIL->PASS (80 passed, 9 failed -- the nine
//   other, unrelated reds unaffected). Reverted: 76/13.
// MUTATION sol:8-pot.c16 stimulus green MUT_G6HOLD (same substitute as
//   opus:8-potchg.c1)
// MUTATION opus:8-fig8-5.c1 stimulus green MUT_G6HOLD (same substitute as
//   opus:8-potchg.c1; this pair grades against exactly 8, matching the
//   figure's precise callout, which the forced completed_reset_lines==8
//   also satisfies)
// MUTATION sol:8-fig8-5.c2 stimulus green MUT_G6HOLD (same substitute as
//   opus:8-potchg.c1)
// MUTATION opus:8-potchg.c4 stimulus red MUT_G7G8: each of the ten single
//   pulses in the cadence loop is built as two pot_cnt_en pulses instead of
//   one, so consecutive reads differ by 2, not 1; the "delta==1 every time"
//   comparison is unchanged. RUN: baseline 76 passed 13 failed; mutated 72
//   passed, 17 failed, exactly these four newly red. Reverted: 76/13.
// MUTATION sol:8-pot.c19 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION opus:8-potchg.c5 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION sol:8-pot.c20 stimulus red (same MUT_G7G8 as opus:8-potchg.c4)
// MUTATION opus:8-potchg.c6 stimulus red MUT_G9 (this assertion's testable
//   scope: the counter does not advance while the charge stays above the
//   preset -- an RC charge physically cannot fall back below the preset
//   absent a POTGO dump, so "held until the next POTGO is issued" is
//   simply the natural consequence of a charge that cannot decrease any
//   other way, and the page states nothing about an artificial digital
//   comparator reversal with no intervening POTGO. GREEN, so it takes a
//   stimulus mutation, not an oracle substitute): the hold step is built
//   skipping the gate-close call, so the counter keeps counting instead of
//   holding; the "during == before" expected value is unchanged. RUN:
//   baseline 76 passed 13 failed; mutated 74 passed, 15 failed, exactly
//   this pair newly red. Reverted: 76/13.
// MUTATION sol:8-pot.c21 stimulus red (same MUT_G9 as opus:8-potchg.c6)
// MUTATION opus:8-potchg.c8 stimulus red MUT_G10: the run to $ff is built one
//   pulse short (254 instead of 255), so the wrap that should land on pulse
//   256 is off by one and the fixed $ff/$00/$01 comparison sequence fails.
//   RUN: baseline 76 passed 13 failed; mutated 73 passed, 16 failed,
//   exactly these three newly red. Reverted: 76/13.
// MUTATION sol:8-pot.c24 stimulus red (same MUT_G10 as opus:8-potchg.c8)
// MUTATION opus:8-appa-pot.c10 stimulus red (same MUT_G10 as opus:8-potchg.c8)
// MUTATION opus:8-fig8-5.c2 stimulus red MUT_G12: pot1x's release step is
//   built using POTGO bits 11:10 (which gate pot0y) instead of 13:12 (which
//   gate pot1x), so pot1x is never actually released and stays at 0 instead
//   of reaching 6; the fixed "all four equal 6" comparison is unchanged.
//   RUN: baseline 76 passed 13 failed; mutated 74 passed, 15 failed,
//   exactly this pair newly red. Reverted: 76/13.
// MUTATION sol:8-fig8-5.c3 stimulus red (same MUT_G12 as opus:8-fig8-5.c2)
// MUTATION opus:8-fig8-5.c4 stimulus red MUT_G13: the "early" run's hold
//   point is built at tick 8 and the "late" run's at tick 3 (the two
//   stimuli are swapped), so the later-crossing-is-larger comparison, left
//   unchanged, now compares the wrong way round. RUN: baseline 76 passed 13
//   failed; mutated 75 passed, 14 failed, exactly this one newly red.
//   Reverted: 76/13.
// MUTATION opus:8-potreg.c1 stimulus red MUT_G14G17: the port-identity check
//   reads POT1DAT where it should read POT0DAT (and vice versa) for the
//   mouse-only-movement half, so the fixed "mouse pins move only POT0DAT"
//   comparison now sees the wrong register; the same substitution also
//   misreads the byte-assignment half. RUN: baseline 76 passed 13 failed;
//   mutated 66 passed, 23 failed, exactly these ten newly red. Reverted:
//   76/13.
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
//   for it, so the four-counters-independent equality fails. RUN: baseline
//   76 passed 13 failed; mutated 71 passed, 18 failed, exactly these five
//   newly red. Reverted: 76/13.
// MUTATION sol:8-appa-pot.c11 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION sol:8-appa-pot.c12 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION opus:8-appa-pot.c11 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION sol:8-appa-pot.c13 stimulus red (same MUT_G19G21 as opus:8-appa-pot.c9)
// MUTATION sol:8-appa-pot.c14 stimulus red MUT_G22: the left-connector
//   release step is built skipping the mouse-button assertion, so POT0DAT
//   never leaves $0000 and the fixed "== 6 after release" comparison
//   fails. RUN: baseline 76 passed 13 failed; mutated 74 passed, 15
//   failed, exactly this pair (this label and the G22 self-check) newly
//   red. Reverted: 76/13.
// MUTATION sol:8-appa-pot.c15 oracle green substitute: right_x_has_a_pin
//   (the red half) is graded != 0; the substitute instead routes the left
//   connector, within the same G22 run, through its own "== 6 after
//   release" comparator -- not the != 0 comparator the red half uses --
//   confirming the read/grade pipeline is not itself vacuous.
// MUTATION opus:8-appa-pot.c12 oracle green (same substitute as sol:8-appa-pot.c15)
// MUTATION opus:8-appa-pot.c13 oracle green (same substitute as sol:8-appa-pot.c15)
// MUTATION opus:8-appa-pot.c14 stimulus red MUT_G23: the X and Y drive
//   targets are built swapped ($b4 where $4b was asked for and vice versa);
//   since $4b and $b4 are exact bitwise complements, every bit read now
//   differs from the unchanged $4b/$b4 expectation table; this one line
//   stands for all 32 individual-bit labels below plus the four sol
//   byte-level labels, all graded from the same two register reads in
//   group_G23(). RUN: baseline 76 passed 13 failed; mutated 40 passed, 49
//   failed, exactly these thirty-six newly red. Reverted: 76/13.
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
// MUTATION FINALCOUNT-FIG8-5-OPUS stimulus green: kTotalTicks = kK (no reset interval prepended) instead of 8+kK, through the real figure_all_four comparator (all four counters ==k). RUN (make MUT=FINALCOUNT test): baseline 76 passed, 13 failed; all four counters read exactly 10, all four FINALCOUNT-* lines turn green (80 passed, 9 failed -- the nine other, unrelated reds unaffected). Reverted: 76/13.
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

// p.235's reset interval. Only group_G6_G11 and group_FINALCOUNT grade the
// interval's own length -- that is what their claim is about. Every other
// group's claim is about something else (existence, rate, wrap, register
// identity, ...), and none of those clauses may assume when counting
// starts: each one holds every pin through kWarmup ticks (longer than
// either printed interval), releases the pin(s) under test, pulses exactly
// n, and grades exactly n in the counter's own byte and 0 elsewhere. Once
// the warm-up has run out the clock, the count from release is exact on
// any implementation, conforming or not.
//
// A few clauses (G9, G13, G14-G17's port-identity half) release the pin
// once at POTGO time and then run kResetInterval+extra ticks before taking
// a "genuinely counting, not stuck in reset" baseline, rather than holding
// and re-releasing; their own claim is a DELTA from that baseline (a hold
// produced no further movement, a later crossing is larger, a moved
// counter reads nonzero), so the interval's exact length does not enter
// the grading, only that enough of it has elapsed. That margin held only
// under the convention that the first strobe after the POTGO write is line
// 1 of the interval; the page does not itself say whether the line holding
// the write counts. kResetInterval is Figure 8-5's exact reading (8);
// group_FINALCOUNT's k-or-k+1 comparators accept the body text's other
// allowed interval (7 lines) as well.
constexpr int kResetInterval = 8;

// Longer than either printed reset interval (7 or 8 lines), with margin.
// Used as a held (or, where the pin is already released, a run-out) warm-up
// so the measurement immediately following it does not depend on when
// counting starts.
constexpr int kWarmup = 15;

//--------------------------------------------------------------------------
// G1 -- p.234: each controller port handles two proportional inputs.
//--------------------------------------------------------------------------
static void group_G1() {
    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup); // held warm-up, every pin still closed from full_reset()
    // MUT_SELFCHECK holds pot0x PRESSED instead of releasing it, so its gate
    // never opens, the counter stays at 0 and x_tracks is false. Expected
    // values untouched: this changes what the module is asked to do.
    // Omitting the arm_potgo() call instead changes nothing (the counter
    // runs without it), so a mutation that must move the result holds the
    // pin instead.
#ifdef MUT_SELFCHECK
    // MUTATED: hold pot0x pressed -- its gate never opens
#else
    set_mouse(0, 0, 1); // release pot0x only (mouse button 3)
#endif
    settle(3);
    pulse(6);
    uint16_t r1 = pot0dat();
    bool x_tracks = ((r1 & 0x00FF) == 6) && ((r1 >> 8) == 0);

    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup);
    set_mouse(0, 1, 0); // release pot0y only (mouse button 2/right)
    settle(3);
    pulse(9);
    uint16_t r2 = pot0dat();
    bool y_tracks = ((r2 >> 8) == 9) && ((r2 & 0x00FF) == 0);

    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup);
    set_joy1_pin5(1); // release pot1y (via the swapped _djoy2<=_joy1 path)
    settle(3);
    pulse(11);
    uint16_t r3 = pot1dat();
    bool port2_y_tracks = ((r3 >> 8) == 11) && ((r3 & 0x00FF) == 0);

    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000; // assert every pin this module reads
    settle(3);
    pulse(11); // aligned with port2_y_tracks's run length above
    uint16_t r4 = pot1dat();
    bool port2_x_moved = ((r4 & 0x00FF) != 0); // nonzero, not a specific count -- structural either way

    note("port 1 (POT0DAT), held " + std::to_string(kWarmup) + " ticks past "
         "either printed interval, then release-then-run-6/9: X-only -> $" +
         hex4(r1) + "; Y-only -> $" + hex4(r2) + " -- " +
         std::string(x_tracks && y_tracks ? "two independently-movable "
         "counters, each graded for exactly the ticks run after release, "
         "not against the raw tick total." : "not both independently "
         "movable to their exact target.") + " "
         "port 2 (POT1DAT), same warm-up, release-then-run-11: joystick-port "
         "pin release -> $" + hex4(r3) + " (Y moved); every controller pin "
         "this module reads, asserted -> $" + hex4(r4) + " -- the low byte "
         "(X) " + std::string(port2_x_moved ? "moved too." :
         "did not move. On this fixture, port 2 carries only one "
         "externally-driven proportional counter, not two."));

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
    arm_potgo(0x0001); // a genuine POTGO start -- legitimate on every
                        // page-conforming implementation, including one
                        // that holds every counter in reset until the
                        // first POTGO start (see u8n in the file header);
                        // establishes a baseline the write below can then
                        // zero, without resting on counting ever happening
                        // before any POTGO write at all
    set_mouse(0, 0, 1); // release pot0x
    settle(3);
    pulse(kWarmup); // run past either printed interval, so before_write is
                     // genuinely counting, not stuck at 0 by the interval
    uint16_t before_write = pot0dat(); // must be nonzero, or the read right
                                        // after the write below is 0 either
                                        // way and proves nothing about POTGO
    bool before_write_nonzero = (before_write != 0);
#ifdef MUT_G2
    // MUTATED: no second POTGO write at all -- the counter is never asked
    // to restart, so it must not read 0 right after
#else
    arm_potgo(0x0001); // the POTGO write this clause grades: must zero pot0x
#endif
    uint16_t right_after_write = pot0dat(); // must read 0 (was nonzero) if POTGO restarted it
    bool zeroed_by_potgo = (right_after_write == 0x0000);
    pulse(kWarmup); // run past either printed interval before taking a
                     // "genuinely counting" baseline -- this clause is about
                     // restart-on-POTGO and continuation, not about when
                     // counting starts, so it does not grade the interval
    uint16_t before = pot0dat();
    bool before_nonzero = (before != 0);

    // POTINP half, MEASURED only: no page says an access to another
    // register leaves the counters alone. This half is therefore not part
    // of the graded `ok` below.
    dut->reg_address_in = A_POTINP; dut->data_in = 0xFFFF; // access a *different* register
    tick();
    idle_bus();
    tick();
    uint16_t after_other = pot0dat();
    bool other_write_no_restart = (after_other == before);
    pulse(2);
    uint16_t after_more = pot0dat();
    bool counting_continued = (after_more == (uint16_t)(before + 2));
    bool starts_on_potgo = before_write_nonzero && zeroed_by_potgo && before_nonzero;

    // Bit-0-clear half, MEASURED only: p.236's "All counts are reset to
    // zero when POTGO is written with bit zero high" is one-way, and
    // p.234's sentence behind this uid has no bit-0 condition -- no page
    // says a POTGO write with bit 0 clear leaves the counters alone. This
    // half is therefore not part of the graded `ok` below.
    full_reset();
    set_mouse(0, 0, 1);
    arm_potgo(0x0001);
    pulse(kWarmup); // pot0x released throughout, run past either printed
                     // interval, so before2 is genuinely counting, not
                     // stuck at 0 by the interval
    uint16_t before2 = pot0dat();
    bool before2_nonzero = (before2 != 0);
    write_potgo_once(0x00F0); // POTGO write, bit 0 clear
    tick();
    settle(2);
    uint16_t after2 = pot0dat();
    bool bit0_clear_no_restart = (after2 == before2);

    note("After a genuine POTGO start and a " + std::to_string(kWarmup) +
         "-tick released warm-up, before the second POTGO write: POT0DAT $" +
         hex4(before_write) + " (" +
         std::string(before_write_nonzero ? "nonzero, so the write below is "
                     "not vacuous" : "zero -- the write below cannot be read") +
         "). Right after that second POTGO write, with bit 0 set: POT0DAT $" +
         hex4(right_after_write) + ". After a further " + std::to_string(kWarmup) +
         "-tick warm-up: $" + hex4(before) + ". NOT GRADED (no page states an "
         "access to another register leaves the counters alone): a write to "
         "a different register (POTINP) mid-run left POT0DAT at $" +
         hex4(after_other) + " (was $" + hex4(before) + "), and counting " +
         std::string(counting_continued ? "continued afterwards, exactly two "
                     "more" : "did not continue by exactly two more") +
         " (-> $" + hex4(after_more) + "). NOT GRADED (no page states "
         "a bit-0-clear condition): pot0x released, run " + std::to_string(kWarmup) +
         " ticks past either printed interval -> $" + hex4(before2) + " (" +
         std::string(before2_nonzero ? "nonzero, so the next line is not "
                     "vacuous" : "zero -- the next line cannot be read") +
         "); a POTGO write with bit 0 clear left it at $" + hex4(after2) +
         " (was $" + hex4(before2) + "), " +
         std::string(bit0_clear_no_restart ? "unchanged" : "changed") + ".");

    bool ok = starts_on_potgo;
    expect(ok, "opus:8-pot.c11");
    expect(ok, "sol:8-pot.c12");
}

//--------------------------------------------------------------------------
// G3 -- p.234: POTGO=$0001 is the standard-joystick start value. The quoted
// sentence states only the hex value itself. It does not say $0001 and
// $00FF must behave identically (p.282 calls bits 7-1 reserved, not
// don't-care) and it does not say all four counters must track an external
// pin (pot1x has none at all in this design -- see the file header note;
// the four-pin topology is p.234's earlier "two per port" sentence and
// p.282's connector table, different uids, graded there). What remains and
// IS the claim's content: writing this specific value starts the counters
// that do have a pin tracking their own pin.
//--------------------------------------------------------------------------
static void group_G3() {
    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup); // held warm-up, every pin still closed from full_reset()
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
    pulse(7);
    uint16_t p0_1 = pot0dat(), p1_1 = pot1dat();

    bool pot0x_tracks = ((p0_1 & 0xFF) == 7);
    bool pot0y_tracks = ((p0_1 >> 8) == 7);
    bool pot1y_tracks = ((p1_1 >> 8) == 7);
    bool pot1x_tracks = ((p1_1 & 0xFF) == 7); // NOT graded here -- see the file header note

    note("POTGO=$0001, held " + std::to_string(kWarmup) + " ticks past either "
         "printed interval, every controller pin then released, "
         "release-then-run-7: POT0DAT $" + hex4(p0_1) + ", POT1DAT $" +
         hex4(p1_1) + ". pot0x " + std::string(pot0x_tracks ? "tracks" : "does NOT track") +
         " its own pin, pot0y " + std::string(pot0y_tracks ? "tracks" : "does NOT track") +
         " its own pin, pot1y " + std::string(pot1y_tracks ? "tracks" : "does NOT track") +
         " its own pin -- this claim's content. pot1x " +
         std::string(pot1x_tracks ? "also moved (not required here)."
         : "did not move (it has no external pin at all in this design -- "
           "not required by this claim; the pin-topology claims are graded "
           "separately, G1 and G22).") );

    bool ok = pot0x_tracks && pot0y_tracks && pot1y_tracks;
    expect(ok, "opus:8-pot.c12");
    expect(ok, "sol:8-pot.c13");
}

//--------------------------------------------------------------------------
// G4 -- p.234: writing POTGO starts hardware that reads the potentiometer
// values. The quoted sentence says nothing about how many pins exist or
// whether each of four counters must have its own -- it only says the
// started hardware reads pot values. Testable as: each counter that this
// design does gate from an external pin (pot0x, pot0y, pot1y) tracks that
// pin once started, and nothing moves while every gate is held. The
// four-independently-wired-pin topology belongs to a different claim
// (p.234's "two per port" sentence / p.282's connector table) and is
// graded there, not here -- pot1x has no external pin at all in this
// design (see the file header note), which is exactly why that topology
// claim is correctly red elsewhere and is not re-tested by this one.
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
    settle(3);
    pulse(kWarmup); // held warm-up, every pin still closed from full_reset()
#ifdef MUT_G4
    // MUTATED: pot0x's pin is built held, not released
#else
    set_mouse(0, 0, 1);
#endif
    settle(3);
    pulse(4);
    uint16_t r_x0 = pot0dat();
    bool only_pot0x_moved = ((r_x0 & 0xFF) == 4) &&
                             ((r_x0 >> 8) == 0) && (pot1dat() == 0x0000);

    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup);
    set_mouse(0, 1, 0);
    settle(3);
    pulse(4);
    uint16_t r_y0 = pot0dat();
    bool only_pot0y_moved = ((r_y0 >> 8) == 4) &&
                             ((r_y0 & 0xFF) == 0) && (pot1dat() == 0x0000);

    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup);
    set_joy1_pin5(1);
    settle(3);
    pulse(4);
    uint16_t r_y1 = pot1dat();
    bool only_pot1y_moved = (pot0dat() == 0x0000) &&
                             ((r_y1 >> 8) == 4) && ((r_y1 & 0xFF) == 0);

    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup);
    set_mouse(1, 1, 1);
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3);
    pulse(4);
    uint16_t everything_p1 = pot1dat();
    bool pot1x_has_a_pin = ((everything_p1 & 0x00FF) != 0); // NOT graded here -- see the file header note

    note("Isolated-release measurements (all four counters, held " +
         std::to_string(kWarmup) + " ticks past either printed interval "
         "then release-then-run-4 each): held-throughout " +
         std::string(nothing_moves_when_held ? "moved nothing; " : "moved something (unexpected); ") +
         "pot0x-only-released " + std::string(only_pot0x_moved ? "moved only pot0x; " : "did NOT move only pot0x; ") +
         "pot0y-only-released " + std::string(only_pot0y_moved ? "moved only pot0y; " : "did NOT move only pot0y; ") +
         "pot1y-only-released " + std::string(only_pot1y_moved ? "moved only pot1y" : "did NOT move only pot1y") +
         " -- this claim's content: each counter that has a pin tracks it, "
         "graded exactly. Every controller pin asserted left POT1DAT's low "
         "byte at $" + hex4(everything_p1 & 0xFF) + " (pot1x " +
         std::string(pot1x_has_a_pin ? "moved too." :
         "did not move; it has no external pin at all in this design, not "
         "required by this claim -- see the file header note.") + ")");

    bool ok = nothing_moves_when_held && only_pot0x_moved && only_pot0y_moved &&
              only_pot1y_moved;
    expect(ok, "opus:8-pot.c13");
    expect(ok, "sol:8-pot.c14");
}

//--------------------------------------------------------------------------
// G5/G18 -- p.234 (loose) and p.236 (precise): a POTGO write with bit 0 set
// zeroes all four counters. G5 must be graded against the same bit-0
// distinction G18 states explicitly, or it would pass for the wrong reason
// (any POTGO write happening to carry bit 0). p.236's sentence is one-way
// ("reset to zero when POTGO is written with bit zero high") and p.234's
// sentence behind G5's ids has no bit-0 condition at all: no page says a
// bit-0-clear write leaves the counters unchanged, so that half is
// MEASURED below, not graded (see the file header's open point).
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
    pulse(kWarmup); // run past either printed interval before "before0/before1"
    uint16_t before0 = pot0dat(), before1 = pot1dat();
    bool nonzero_before = (before0 != 0) && (before1 != 0);

#ifdef MUT_G5G18
    // MUTATED: no POTGO write at all -- not a bit-0-clear write, which
    // rests on the page-open bit-0-clear behaviour (see the file header);
    // omitting the write entirely does not.
#else
    arm_potgo(0x2001); // keep pot1x's release bits, add bit 0
#endif
    uint16_t after_bit0_hi = pot0dat(), after_bit0_hi_p1 = pot1dat();
    bool all_zero_after_bit0 = (after_bit0_hi == 0) && (after_bit0_hi_p1 == 0);

    pulse(kWarmup); // run past either printed interval again before "before3",
                     // so the MEASURED bit-0-clear write below is read against
                     // a counter genuinely counting, not stuck at 0
    uint16_t before3 = pot0dat(), before3_p1 = pot1dat();
    bool before3_nonzero = (before3 != 0) && (before3_p1 != 0);
    // Bit-0-clear half, MEASURED only: p.236's "All counts are reset to
    // zero when POTGO is written with bit zero high" is one-way, and
    // p.234's sentence behind this uid has no bit-0 condition -- no page
    // says a POTGO write with bit 0 clear leaves the counters alone. Not
    // part of the graded `ok` below.
    write_potgo_once(0x2000); // bit 0 clear
    tick();
    settle(2);
    uint16_t after3 = pot0dat(), after3_p1 = pot1dat();
    bool bit0_low_unchanged = (after3 == before3) && (after3_p1 == before3_p1);

    note("Before a bit-0 POTGO write, held " + std::to_string(kWarmup) +
         " ticks past either printed interval: POT0DAT $" + hex4(before0) +
         " POT1DAT $" + hex4(before1) + "; right after POTGO=$2001: POT0DAT $" +
         hex4(after_bit0_hi) + " POT1DAT $" + hex4(after_bit0_hi_p1) +
         " (" + std::string(all_zero_after_bit0 ? "both zeroed, matches p.236" :
                             "not both zeroed") + "). NOT GRADED (no page "
         "states a bit-0-clear condition): after running " +
         std::to_string(kWarmup) + " ticks again (POT0DAT $" + hex4(before3) +
         " POT1DAT $" + hex4(before3_p1) + ") and a bit-0-clear POTGO write "
         "($2000): POT0DAT $" + hex4(after3) + " POT1DAT $" + hex4(after3_p1) +
         " -- " + std::string(bit0_low_unchanged ? "unchanged" : "changed") + ".");

    bool ok = nonzero_before && all_zero_after_bit0 && before3_nonzero;
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
#ifndef MUT_G6HOLD
    set_mouse(0, 0, 1);
    settle(3);
#endif
    int first_increment_at = -1;
    for (int i = 1; i <= 12 && first_increment_at < 0; i++) {
#ifdef MUT_G6HOLD
        // MUTATION substitute (rule against mutating an already-failing
        // check): hold pot0x's own pin through strobes 1-8, release it
        // before strobe 9 -- a genuine, independently-live stimulus change,
        // inside this same measurement, through the identical
        // matches_reset_interval()/==8 comparators below.
        if (i == 1) { set_mouse(0, 0, 0); settle(3); }
        else if (i == 9) { set_mouse(0, 0, 1); settle(3); }
#endif
        pulse(1);
        if ((pot0dat() & 0xFF) != 0) first_increment_at = i;
    }
    // first_increment_at is the 1-indexed pulse number
    // at which the count first reads nonzero, not the count of *completed*
    // reset lines that preceded it -- a first increment at pulse 1 means
    // ZERO completed reset lines, not "1" or "7-or-8". The manual names the
    // completed-reset-line count ("stays in reset for the first seven or
    // eight lines"), so that is the quantity to compare against 7/8, not
    // the raw pulse index.
    int completed_reset_lines = first_increment_at - 1;
    std::string reset_state_conclusion;
    if (completed_reset_lines == 0) {
        reset_state_conclusion = "This RTL holds no separate multi-line "
            "reset state at all.";
    } else if (matches_reset_interval(completed_reset_lines)) {
        reset_state_conclusion = "This RTL's reset state spans exactly the "
            "page's printed interval.";
    } else {
        reset_state_conclusion = "This RTL's reset state matches neither "
            "printed reading.";
    }
    note("pot0x's gate was released before the first pulse; its first "
         "nonzero read came after pulse " + std::to_string(first_increment_at) +
         " -- " + std::to_string(completed_reset_lines) + " completed reset "
         "line(s) preceded it. " + reset_state_conclusion);

    bool body_text_claim = matches_reset_interval(completed_reset_lines); // p.235 text: "seven or eight"
    expect(body_text_claim, "opus:8-potchg.c1");
    expect(body_text_claim, "sol:8-pot.c16");

    // Figure 8-5's callout gives the precise case ("eight"), not the body
    // text's looser "seven or eight" -- this pair is graded tighter, against
    // exactly 8, not the 7-or-8 the body-text pair shares.
    bool figure_claim = (completed_reset_lines == 8);
    expect(figure_claim, "opus:8-fig8-5.c1");
    expect(figure_claim, "sol:8-fig8-5.c2");
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
    pulse(kWarmup); // past either printed interval first: this claim is about
                     // the per-tick rate ONCE counting, not about when
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
// POTGO is issued. An RC timing capacitor's charge cannot fall back below
// the preset on its own -- nothing in this design (or in a real RC charge
// circuit) lowers it short of a POTGO dump -- so "held until the next
// POTGO is issued" is simply the consequence of a charge that cannot
// decrease any other way, not a separate requirement that the counter stay
// latched through an artificial digital comparator reversal with no POTGO.
// The page says nothing about that reversal case. What's testable: the
// counter does not advance for as long as the charge (here, the digital
// gate) stays above the preset.
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
         std::string(resumed_without_potgo ?
         "moved: this design resumes counting as soon as the gate reopens, "
         "POTGO or not." : "stayed put on this build.") +
         " An RC charge cannot fall back below the preset on its own, so "
         "the manual's 'held until the next POTGO' never has to specify "
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
    pulse(kWarmup); // past either printed interval first: the wrap claim
                     // (8-bit width, no flag/stop) is a MODULAR property,
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
         " / $" + hex4(d257) + " from base -- " +
         std::string(wraps ? "an 8-bit counter that wraps with no flag and "
                     "no stop, regardless of the interval's own length."
                     : "does not match the expected $ff/$00/$01 wrap "
                     "sequence."));
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
    settle(3); pulse(kWarmup); // held warm-up past either printed interval
    set_mouse(0, 0, 1);
    settle(3); pulse(6);
    set_mouse(0, 0, 0); settle(2);
    uint16_t v_x0 = pot0dat() & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    settle(3); pulse(kWarmup);
    set_mouse(0, 1, 0);
    settle(3); pulse(6);
    set_mouse(0, 0, 0); settle(2);
    uint16_t v_y0 = (pot0dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    settle(3); pulse(kWarmup);
    set_joy1_pin5(1);
    settle(3); pulse(6);
    set_joy1_pin5(0); settle(2);
    uint16_t v_y1 = (pot1dat() >> 8) & 0xFF;

    full_reset();
    arm_potgo(0x0001);
    settle(2); pulse(kWarmup);
#ifdef MUT_G12
    set_potgo_bits(0x0800); // MUTATED: bits 11:10 gate pot0y, not pot1x
#else
    set_potgo_bits(0x2000); // bits 13:12 = 1,0 -> releases pot1x
#endif
    settle(2); pulse(6);
    // Read pot1x before closing its gate again: closing it is itself a
    // further bit-0-clear POTGO write (set_potgo_bits always clears bit 0),
    // and no page defines what that write does to the counters -- see the
    // file header's open point. Reading first means this grade does not
    // depend on it either way.
    uint16_t v_x1 = pot1dat() & 0xFF;
    set_potgo_bits(0x0000); settle(2);

    bool all_equal = (v_x0 == 6) && (v_y0 == 6) && (v_y1 == 6) && (v_x1 == 6);
    note("All four counters, held " + std::to_string(kWarmup) + " ticks past "
         "either printed interval, then release-then-run-6: pot0x=" +
         std::to_string(v_x0) + " pot0y=" + std::to_string(v_y0) +
         " pot1y=" + std::to_string(v_y1) + " pot1x=" + std::to_string(v_x1) +
         " -- " + std::string(all_equal ? "the same count/hold rule for all "
         "four, graded exactly, not against the raw tick total."
         : "not all four agree.") );
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
         std::to_string(late_cross) + " -- " +
         std::string(later_is_larger ? "later crossing, larger count."
                     : "later crossing did NOT yield a larger count."));
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
    note("Releasing every mouse-gated pin " +
         std::string(mouse_only_moves_pot0dat ? "moved only POT0DAT" : "did NOT move only POT0DAT") +
         " ($" + hex4(p0) + "/$" + hex4(p1) + "); releasing the joystick-port "
         "pin that gates POT1DAT's Y counter " +
         std::string(port2_pin_only_moves_pot1dat ? "moved only POT1DAT" : "did NOT move only POT1DAT") +
         " ($" + hex4(p0b) + "/$" + hex4(p1b) + "). " +
         std::string(port_identity_ok ?
         "POT0DAT is the mouse-connector register (port 1, per the p.236 "
         "reading this package's conflict resolution adopts); POT1DAT is "
         "the other one (port 2)."
         : "The measured register identity does not match that reading."));
    expect(port_identity_ok, "opus:8-potreg.c1");
    expect(port_identity_ok, "sol:8-pot.c26");
    expect(port_identity_ok, "sol:8-pot.c27");
    expect(port_identity_ok, "opus:8-potreg.c2");
    expect(port_identity_ok, "sol:8-pot.c28");
    expect(port_identity_ok, "sol:8-pot.c29");

    full_reset();
    arm_potgo(0x0001);
    settle(3);
    pulse(kWarmup); // held warm-up past either printed interval
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
    settle(3);
    pulse(kWarmup);
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
    note("POT0DAT, held " + std::to_string(kWarmup) + " ticks past either "
         "printed interval, with only the X counter then released, "
         "release-then-run-9: $" + hex4(xonly) + "; with only Y released: $" +
         hex4(yonly) + " -- " +
         std::string(byte_assignment_ok ? "Y occupies bits 15-8, X occupies "
         "bits 7-0, graded exactly, not against the raw tick total."
         : "the measured byte assignment does not match that reading."));
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
    pulse(kWarmup); // past either printed interval first, with every pin
                     // still held (default from full_reset()): this claim
                     // is about four independently-addressable counters
                     // reaching four DISTINCT values, not about when
                     // counting starts, and the four target values below
                     // (3/4/9/13) would collide once shifted by an uncertain
                     // 7-or-8-line interval if graded per-phase instead
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
    // Joint read of both registers here, before any POTGO write touches
    // pot1x: p.282's own claim is that these addresses each read a PAIR
    // ("These addresses each read a pair of 8-bit pot counters"), so
    // pot0x/pot0y and pot1y are graded through the same joint register
    // reads the page describes, not through four independent byte reads --
    // a joint read catches, for example, a design where pot0x keeps
    // counting while pot0y's own gate is open (see u8c in the file header).
    uint16_t p0 = pot0dat();          // pot0y=4 (high byte), pot0x=3 (low byte)
    uint16_t p1_before_x = pot1dat(); // pot1y=9 (high byte), pot1x=0 (low byte, not yet touched)
    // pot1x is released through a POTGO write of its own (set_potgo_bits
    // always clears bit 0); read it on its own, immediately after its own
    // count, before the trailing close below -- that close is itself a
    // further bit-0-clear POTGO write with no page-defined effect on the
    // counters (see the file header's open point), so this grade does not
    // depend on it either way. This is the one counter that cannot join
    // the read above without resting on that open point.
    set_potgo_bits(0x2000); settle(2); pulse(13); // pot1x = 13 ($0d)
    uint8_t v_x1 = (uint8_t)(pot1dat() & 0xFF);
    set_potgo_bits(0x0000); settle(2);

    uint16_t p1 = (uint16_t)((p1_before_x & 0xFF00) | v_x1);
    bool four_independent = (p0 == 0x0403) && (p1_before_x == 0x0900) && (v_x1 == 0x0D);
    note("Four counters driven to four different values independently: "
         "joint POT0DAT=$" + hex4(p0) + " (pot0y=" + std::to_string(p0 >> 8) +
         ",pot0x=" + std::to_string(p0 & 0xFF) + "), joint POT1DAT=$" +
         hex4(p1_before_x) + " before pot1x is touched (pot1y=" +
         std::to_string(p1_before_x >> 8) + ",pot1x=" +
         std::to_string(p1_before_x & 0xFF) + "), pot1x's own read after "
         "its count=" + std::to_string(v_x1) + " -- " +
         std::string(four_independent ? "four separately-addressable "
         "counters, each register reading a pair." : "not the four target "
         "values (pot0y=4,pot0x=3,pot1y=9,pot1x=13)."));
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
    settle(3); pulse(kWarmup); // held warm-up past either printed interval
    uint16_t held_left = pot0dat();
    bool left_holds_when_no_signal = (held_left == 0x0000);
#ifdef MUT_G22
    // MUTATED: skip asserting the mouse buttons
#else
    set_mouse(1, 1, 1);
#endif
    settle(3);
    pulse(6);
    uint16_t released_left = pot0dat();
    bool left_both_have_a_pin = ((released_left >> 8) == 6) &&
                                 ((released_left & 0xFF) == 6);
    bool left_ok = left_holds_when_no_signal && left_both_have_a_pin;

    full_reset();
    arm_potgo(0x0001);
    settle(3); pulse(kWarmup); // held warm-up, every joystick pin still idle
    dut->_joy1 = 0x0000; dut->_joy2 = 0x0000;
    settle(3); pulse(6);
    uint16_t right_everything = pot1dat();
    bool right_y_has_a_pin = ((right_everything >> 8) == 6);
    bool right_x_has_a_pin = ((right_everything & 0xFF) != 0); // structurally always false -- pot1x has no pin, see file header note
    bool right_both_have_a_pin = right_y_has_a_pin && right_x_has_a_pin;

    note("Left connector (mouse), held " + std::to_string(kWarmup) + " ticks "
         "past either printed interval: -> $" + hex4(held_left) +
         "; every mouse pin then asserted, release-then-run-6 -> $" +
         hex4(released_left) + " (" +
         std::string(left_both_have_a_pin ? "both counters moved"
                     : "not both counters moved") +
         "). Right connector: same warm-up, then every joystick pin on both "
         "_joy1 and _joy2 asserted, release-then-run-6 -> $" +
         hex4(right_everything) + " -- the Y counter (high byte) " +
         std::string(right_y_has_a_pin ? "moved" : "did not move") +
         "; the X counter (low byte) " +
         std::string(right_x_has_a_pin ? "moved too."
         : "did not: it has no connector pin driving it in this design at "
           "all (chip-internal only, see the file header note / G1 / G4)."));

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
    // kWarmup is spent up front, with pot1x still held (the default out of
    // full_reset()), so either printed interval is exhausted before
    // X_TARGET's own pulse count begins -- a bit-level table cannot be
    // graded k-or-k+1 per bit (75 and 76 do not differ by a single bit), so
    // this claim is tested against the exact target, past the interval,
    // rather than by widening the comparator.
    full_reset();
    arm_potgo(0x0001);
    pulse(kWarmup);
    set_potgo_bits(0x2000); settle(2);
    pulse(X_TARGET);
    // Read pot1x's own value now, before any further bit-0-clear POTGO
    // write: closing its gate again (next line) is itself such a write,
    // with no page-defined effect on the counters (file header open
    // point), so this grade does not depend on it either way.
    uint16_t right_x = pot1dat() & 0xFF;
    set_potgo_bits(0x0000); settle(2); // hold pot1x again so the Y phase below does not also move it
    set_joy1_pin5(1); settle(3);
    pulse(Y_TARGET);
    uint16_t right_y = (pot1dat() >> 8) & 0xFF;
    uint16_t right_reg = (uint16_t)((right_y << 8) | right_x);
    uint16_t right_expect = (uint16_t)((0xB4u << 8) | 0x4B);
    check_bit_table(right_reg, right_expect, RIGHT_BITS, 16);
    expect((right_reg >> 8) == 0xB4, "sol:8-appa-pot.c16");
    expect((right_reg & 0xFF) == 0x4B, "sol:8-appa-pot.c17");

    // POT0DAT ("LEFT"): both counters have real external pins (mouse
    // buttons). Same kWarmup warm-up, pot0x still held, before X_TARGET's
    // own pulse count begins.
    full_reset();
    arm_potgo(0x0001);
    pulse(kWarmup);
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

    bool right_matches = (right_reg == right_expect);
    bool left_matches = (left_reg == left_expect);
    note("POT1DAT driven to pot1y=$b4/pot1x=$4b via chip-internal release for "
         "X, external pin for Y -> read back $" + hex4(right_reg) + " (" +
         std::string(right_matches ? "matches" : "does NOT match") +
         "); POT0DAT driven to pot0y=$b4/pot0x=$4b via mouse buttons -> read "
         "back $" + hex4(left_reg) + " (" +
         std::string(left_matches ? "matches" : "does NOT match") +
         ") -- " +
         std::string(right_matches && left_matches ?
         "both match the manual's table bit for bit."
         : "at least one does not match the manual's table bit for bit."));
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
    // group_FINALCOUNT below grade, and this gate must not prejudge it. Same
    // constant (kWarmup) every other group below uses for its own held
    // warm-up.
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1);  // release pot0x only
    settle(3);
    pulse(kWarmup);
    uint16_t live0 = pot0dat();
    uint16_t live1 = pot1dat();
    bool pot0x_counted = (live0 & 0x00FF) != 0;
    bool others_untouched = ((live0 & 0xFF00) == 0) && (live1 == 0x0000);
    bool alive = pot0x_counted && others_untouched;
    note("Fixture liveness: releasing pot0x only and pulsing " +
         std::to_string(kWarmup) + " times (longer than either "
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
