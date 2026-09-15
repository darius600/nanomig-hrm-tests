// test_userio.cpp -- ch8-joydat-counter-layout
//
// Fixture: userio.v, compiled alone (Verilator, --top-module userio).
// Grades the 52 testable propositions in SPECIFICATION.md against the
// live module. 4 further claims (the port-1/port-2 label dispute) are
// declined via record() in main() and never appear on a graded line.
//
// REPAIR (2026-09-15): an independent validation (hrm-testpipeline/work/
// 2026-09-15_ch8_s1_sol_validation_B.md, finding 5) found that 44 of these
// 52 assertions tested the wrong mechanism. p.230 says live mouse/
// trackball VERTICAL counts occupy bits 15-8 and HORIZONTAL counts bits
// 7-0; p.281 says four separate counters exist, each clocked by two
// controller pins. The prior version of this file seeded arbitrary values
// into the registers through JOYTEST and derived the low two bits with a
// formula copied out of the RTL (solve_pins()) -- so the byte-assignment,
// one-hot-bit-table and four-counter checks were measurements of the
// JOYTEST load-and-readback path, not of mouse motion. An implementation
// that swapped which byte vertical/horizontal motion updates would have
// kept every one of them green.
//
// This version drives the controller pins through genuine quadrature
// transitions and counts them for those 44 assertions (helpers quad_step /
// zero_baseline / walk_axis below). The other 8 graded assertions --
// opus:8-joydat.c1 (control case), opus:8-joydat.c4 / sol:8-mouse.c15 /
// sol:8-mouse.c16 (16-bit width), and opus:8-appa-joy.c9 / .c10 /
// sol:8-appa-joy.c11 / .c12 (8-bit-counter-pair width) -- are NOT about
// mouse-motion semantics (they are about register readability and field
// width, which JOYTEST-seeded values test validly regardless of mechanism)
// and the validation did not flag them; they, and the 4 p.230/p.233
// port-number declines, are unchanged from the prior version and still use
// drive_register()/solve_pins()/set_quad_static() below.
//
// TECHNIQUE, read from hrm/ch8-mouse-quadrature-decode/test_userio.cpp
// (the sibling package that already grades ONE-COUNT-PER-QUADRATURE-STATE-
// CHANGE and EXACTLY-TWO-LINES-PER-AXIS green by moving pins on this same
// fixture) and its Makefile:
//   - axis 0 (horizontal/X) is driven on bits 2 ("H", main) and 0 ("HQ",
//     quad); axis 1 (vertical/Y) on bits 3 ("V", main) and 1 ("VQ", quad)
//     of whichever of _joy1/_joy2 feeds the register under test
//     (userio.v:234-252 port 1, :261-279 port 2 -- same pairing both
//     ports; p.281's own table, "pins 1,3=Yclock, pins 2,4=Xclock").
//   - A single accepted transition needs two clk7_en-gated edges: the
//     transition-detect compares the live synchronized pin level against a
//     second synchronizer stage one edge further behind (userio.v:213-219).
//   - Case 1 ring order (main leads quad: 00->10->11->01->00...) is
//     Figure 8-2's "Count Up"; Case 2 (quad leads main: 00->01->11->10->
//     00...) is "Count Down". The sibling's own mutation log (VERTICAL-
//     CASE1-COUNTS-UP / -CASE2-COUNTS-DOWN / HORIZONTAL-RIGHT-INCREMENTS /
//     -LEFT-DECREMENTS, each run and reverted) proves this ring order and
//     up/down sense on this exact fixture.
//   - Neither dmouse0dat nor dmouse1dat has a reset branch of its own
//     (userio.v:229-282), so an absolute baseline has to be established by
//     settling pins, not assumed. Holding a counter's own main/quad pins
//     at the static level (1,1) makes the idle "else" branch compute
//     {!1,1^1}={0,0}=0 for that byte's own low two bits and keep
//     recomputing that same 0 every idle cycle (no drift) -- adapted here
//     from the sibling's METHODOLOGY (3)/(4), which instead uses (0,0) and
//     compares deltas because none of its own claims need an absolute
//     all-zero register. This package's per-bit and byte-range claims do
//     need an absolute value (a specific bit set, nothing else), so
//     zero_baseline() below drives (1,1) on both axes and then issues a
//     JOYTEST write with data_in=0 to also clear the upper six bits of
//     each byte (unconditionally for JOY0DAT, userio.v:232-233/245-246;
//     for JOY1DAT only bits [7:2]/[15:10] are JOYTEST-writable,
//     userio.v:259-260/272-273, but the low two are already 0 by then from
//     the settle and JOYTEST never touches them). Not copied verbatim: this
//     file keeps clk7_en permanently asserted (see reset_dut()) rather
//     than toggling it per operation as the sibling's Harness does; the
//     rest of the ring/timing/zeroing logic is unchanged in substance.
//   - ensure_joy1enable() (below) is not called by the new motion-based
//     checks. It exists so JOY0DAT's read mux selects the branch where
//     dmouse0dat's low two bits pass through unmixed with mouse0dat -- but
//     mouse0dat (userio.v:358, fed only by the kbd/PS2-mouse-emulation path
//     at :330-355, gated on kms_level toggling) is provably 0 throughout
//     every check in this file: reset_dut() sets kms_level=0 and nothing
//     in this file ever changes it again, so old_level==kms_level always
//     and that path's edge-detect never fires. With mouse0dat==0 both of
//     JOY0DAT's read-mux branches (userio.v:290-293) reduce to plain
//     dmouse0dat, so joy1enable's value cannot affect any read in this
//     file -- the same fact the sibling package's own METHODOLOGY (5)
//     records and relies on.
//
// Mutation replay: `make MUT=<sanitized-uid>` defines MUT_<sanitized-uid>
// for the build, which the ACTIVE_MUTATION chain below turns back into
// the exact uid string so the one check that owns that uid corrupts its
// own stimulus (and only its own) for that build. Run `make clean` first
// when switching MUT values.
//
// MUTATION header -- one line per graded assertion. Category is always
// "stimulus" (the driven value changed, the comparator/expected value did
// not) and outcome is always "red": each was run once, alone, via
// `make clean && make MUT=<sanitized-uid> && ./obj_dir/Vuserio` (52 separate
// rebuilds), and every one flipped exactly its own PASS to FAIL -- diffed
// against the unmutated baseline output, no other assertion moved, and the
// binary's own exit code (checked directly on the invocation that ran it,
// per the build brief -- never via a later `echo $?`) was 1 in every case.
// Baseline: 52 passed, 0 failed. Every mutated build below: 51 passed, 1
// failed, exit code 1 -- reverted to baseline (52/0, exit 0) before the
// next mutation. See the repair's report for the full per-uid tally.
// MUTATION opus:8-joydat.c1 stimulus red stimulus never driven: JOY0DAT/JOY1DAT left at their measured idle 0x0202 instead of writing the two distinct markers 0x1357/0x2468
// MUTATION opus:8-joydat.c4 stimulus red bit0 dropped from the shared 0x8001 stimulus (0x8000 written instead) to both registers
// MUTATION sol:8-mouse.c15 stimulus red bit0 dropped from the 0x8001 stimulus (0x8000 written instead) to JOY0DAT
// MUTATION sol:8-mouse.c16 stimulus red bit0 dropped from the 0x8001 stimulus (0x8000 written instead) to JOY1DAT
// MUTATION opus:8-joydat.c5 stimulus red vertical claim driven on the HORIZONTAL axis instead (the byte-swap defect the validation named) -- the high byte this claim requires stays at its zero baseline
// MUTATION sol:8-mouse.c17 stimulus red same axis swap as opus:8-joydat.c5 (the twin claim)
// MUTATION opus:8-joydat.c6 stimulus red horizontal claim driven on the VERTICAL axis instead -- the low byte this claim requires stays at its zero baseline
// MUTATION sol:8-mouse.c18 stimulus red same axis swap as opus:8-joydat.c6 (the twin claim)
// MUTATION opus:8-appa-joy.c9 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), both registers
// MUTATION opus:8-appa-joy.c10 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), both registers
// MUTATION sol:8-appa-joy.c11 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), JOY0DAT only
// MUTATION sol:8-appa-joy.c12 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), JOY1DAT only
// MUTATION opus:8-appa-joy.c13 stimulus red JOY1DAT's vertical (vertical/Y) counter never driven with its own quadrature steps, left at its zero baseline instead of the requested 61
// MUTATION sol:8-appa-joy.c15 stimulus red same undriven-counter mutation as opus:8-appa-joy.c13 (the twin claim)
// MUTATION opus:8-appa-joy.c14 stimulus red second controller pin (quad line) of the mutated counter never toggled after the first (main line) was, on all four counters checked -- one representative counter (JOY1DAT/horizontal) has its pin B withheld
// MUTATION sol:8-appa-joy.c16 stimulus red same withheld-pin-B mutation as opus:8-appa-joy.c14 (the twin claim)
// MUTATION opus:8-appa-joy.c19 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c20 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c21 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c22 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c23 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c24 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c25 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c26 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c27 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c28 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c29 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c30 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c31 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c32 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c33 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c34 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c35 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c36 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c37 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c38 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c39 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c40 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c41 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c42 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c43 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c44 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c45 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c46 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c47 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c48 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c49 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c50 stimulus red one-hot quadrature step count moved from 2^bit to 2^((bit+1) mod 8) within the same byte, so the wrong bit is driven while the check still reads the claimed position
// MUTATION sol:8-appa-joy.c21 stimulus red quadrature step count driven 0x96 XOR 0x01 (151) instead of the required 0x96 (150) against JOY0DAT's high (Y) byte, oracle left at 0x96
// MUTATION sol:8-appa-joy.c22 stimulus red quadrature step count driven 0x96 XOR 0x01 (151) instead of the required 0x96 (150) against JOY0DAT's low (X) byte, oracle left at 0x96
// MUTATION sol:8-appa-joy.c23 stimulus red quadrature step count driven 0x96 XOR 0x01 (151) instead of the required 0x96 (150) against JOY1DAT's high (Y) byte, oracle left at 0x96
// MUTATION sol:8-appa-joy.c24 stimulus red quadrature step count driven 0x96 XOR 0x01 (151) instead of the required 0x96 (150) against JOY1DAT's low (X) byte, oracle left at 0x96

#include "Vuserio.h"
#include "verilated.h"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------
// Harness contract (fixed by the build brief): expect() grades, note()
// prints a measurement graded by nothing, record() declines a claim.
// ---------------------------------------------------------------------------

struct Checks { int passed = 0; int failed = 0; } checks;

void expect(bool cond, const std::string& label) {
    if (cond) { checks.passed++; std::cout << "PASS: " << label << "\n"; }
    else      { checks.failed++; std::cout << "FAIL: " << label << "\n"; }
}
void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

static std::string hex4(uint16_t v) {
    std::ostringstream os;
    static const char* digits = "0123456789ABCDEF";
    char buf[5];
    for (int i = 3; i >= 0; i--) { buf[3 - i] = digits[(v >> (i * 4)) & 0xF]; }
    buf[4] = 0;
    return std::string(buf);
}

// ---------------------------------------------------------------------------
// Mutation dispatch. Exactly one MUT_<name> is defined per build (or none
// for the baseline/control build). ACTIVE_MUTATION resolves that define
// back to the full uid string at compile time so the rest of the file
// only has to compare strings, never preprocessor tokens.
// ---------------------------------------------------------------------------
#if defined(MUT_opus_8_joydat_c1)
  #define ACTIVE_MUTATION "opus:8-joydat.c1"
#elif defined(MUT_opus_8_joydat_c4)
  #define ACTIVE_MUTATION "opus:8-joydat.c4"
#elif defined(MUT_sol_8_mouse_c15)
  #define ACTIVE_MUTATION "sol:8-mouse.c15"
#elif defined(MUT_sol_8_mouse_c16)
  #define ACTIVE_MUTATION "sol:8-mouse.c16"
#elif defined(MUT_opus_8_joydat_c5)
  #define ACTIVE_MUTATION "opus:8-joydat.c5"
#elif defined(MUT_sol_8_mouse_c17)
  #define ACTIVE_MUTATION "sol:8-mouse.c17"
#elif defined(MUT_opus_8_joydat_c6)
  #define ACTIVE_MUTATION "opus:8-joydat.c6"
#elif defined(MUT_sol_8_mouse_c18)
  #define ACTIVE_MUTATION "sol:8-mouse.c18"
#elif defined(MUT_opus_8_appa_joy_c9)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c9"
#elif defined(MUT_opus_8_appa_joy_c10)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c10"
#elif defined(MUT_sol_8_appa_joy_c11)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c11"
#elif defined(MUT_sol_8_appa_joy_c12)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c12"
#elif defined(MUT_opus_8_appa_joy_c13)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c13"
#elif defined(MUT_sol_8_appa_joy_c15)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c15"
#elif defined(MUT_opus_8_appa_joy_c14)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c14"
#elif defined(MUT_sol_8_appa_joy_c16)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c16"
#elif defined(MUT_opus_8_appa_joy_c19)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c19"
#elif defined(MUT_opus_8_appa_joy_c20)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c20"
#elif defined(MUT_opus_8_appa_joy_c21)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c21"
#elif defined(MUT_opus_8_appa_joy_c22)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c22"
#elif defined(MUT_opus_8_appa_joy_c23)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c23"
#elif defined(MUT_opus_8_appa_joy_c24)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c24"
#elif defined(MUT_opus_8_appa_joy_c25)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c25"
#elif defined(MUT_opus_8_appa_joy_c26)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c26"
#elif defined(MUT_opus_8_appa_joy_c27)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c27"
#elif defined(MUT_opus_8_appa_joy_c28)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c28"
#elif defined(MUT_opus_8_appa_joy_c29)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c29"
#elif defined(MUT_opus_8_appa_joy_c30)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c30"
#elif defined(MUT_opus_8_appa_joy_c31)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c31"
#elif defined(MUT_opus_8_appa_joy_c32)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c32"
#elif defined(MUT_opus_8_appa_joy_c33)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c33"
#elif defined(MUT_opus_8_appa_joy_c34)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c34"
#elif defined(MUT_opus_8_appa_joy_c35)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c35"
#elif defined(MUT_opus_8_appa_joy_c36)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c36"
#elif defined(MUT_opus_8_appa_joy_c37)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c37"
#elif defined(MUT_opus_8_appa_joy_c38)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c38"
#elif defined(MUT_opus_8_appa_joy_c39)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c39"
#elif defined(MUT_opus_8_appa_joy_c40)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c40"
#elif defined(MUT_opus_8_appa_joy_c41)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c41"
#elif defined(MUT_opus_8_appa_joy_c42)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c42"
#elif defined(MUT_opus_8_appa_joy_c43)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c43"
#elif defined(MUT_opus_8_appa_joy_c44)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c44"
#elif defined(MUT_opus_8_appa_joy_c45)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c45"
#elif defined(MUT_opus_8_appa_joy_c46)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c46"
#elif defined(MUT_opus_8_appa_joy_c47)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c47"
#elif defined(MUT_opus_8_appa_joy_c48)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c48"
#elif defined(MUT_opus_8_appa_joy_c49)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c49"
#elif defined(MUT_opus_8_appa_joy_c50)
  #define ACTIVE_MUTATION "opus:8-appa-joy.c50"
#elif defined(MUT_sol_8_appa_joy_c21)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c21"
#elif defined(MUT_sol_8_appa_joy_c22)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c22"
#elif defined(MUT_sol_8_appa_joy_c23)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c23"
#elif defined(MUT_sol_8_appa_joy_c24)
  #define ACTIVE_MUTATION "sol:8-appa-joy.c24"
#else
  #define ACTIVE_MUTATION ""
#endif

static const std::string MUT = ACTIVE_MUTATION;
static bool mutated(const char* uid) { return !MUT.empty() && MUT == uid; }

// ---------------------------------------------------------------------------
// DUT wrapper
// ---------------------------------------------------------------------------

static Vuserio* top;
static vluint64_t sim_time = 0;

static const uint16_t JOY0DAT_ADDR = 0x00a;
static const uint16_t JOY1DAT_ADDR = 0x00c;
static const uint16_t JOYTEST_ADDR = 0x036;

static void tick() {
    top->clk = 0; top->eval(); sim_time++;
    top->clk = 1; top->eval(); sim_time++;
}
static void settle(int n) { for (int i = 0; i < n; i++) tick(); }

static void set_addr(uint16_t addr9) {
    top->reg_address_in = (addr9 >> 1) & 0xFF;
}

// Combinational read: address decode and the output mux have no clocked
// state of their own, so no clock edge is needed between setting the
// address and reading data_out -- only eval().
static uint16_t read_reg(uint16_t addr9) {
    set_addr(addr9);
    top->eval();
    return top->data_out;
}

static void set_bit(vluint16_t& reg, int idx, int val) {
    if (val) reg = (vluint16_t)(reg | (1u << idx));
    else     reg = (vluint16_t)(reg & ~(1u << idx));
}

static void reset_dut() {
    top->reset = 1;
    top->clk7_en = 1;
    top->pot_cnt_en = 0;
    top->_fire0_dat = 1;
    top->_fire1_dat = 1;
    top->_joy1 = 0;
    top->_joy2 = 0;
    top->joy_ana1 = 0;
    top->joy_ana2 = 0;
    top->mouse_btn = 0;
    top->kms_level = 0;
    top->kbd_mouse_type = 0;
    top->kbd_mouse_data = 0;
    top->data_in = 0;
    top->reg_address_in = 0;
    settle(4);
    top->reset = 0;
    settle(4);
}

// Sets joy1enable=1 (the "read port 1 joystick" mux path for JOY0DAT,
// used so JOY0DAT's dmouse0dat bits show up unmixed with the separate
// kbd-mouse-emulated mouse0dat counters -- MEASURED: JOY0DAT is read via
// data_out=mouse0dat+dmouse0dat when joy1enable=0 and via
// {..,dmouse0dat[9:8],..,dmouse0dat[1:0]} when joy1enable=1; userio.v:
// 289-293. _sjoy1[4] is driven from module input _joy2[4] (see the cross
// -wired synchronizer at userio.v:213-219: _sjoy1<=joy_swap?joy1:joy2,
// and joy_swap is tied 0), so driving _joy2[4]=0 and mouse_btn[0]=0 (so
// _mleft=1, userio.v:361) lets joy1enable settle to 1 (userio.v:221-227).
// Used only by the retained JOYTEST-seeded checks below (control case,
// width, 8-bit-pair) -- the new motion-based checks don't need it; see the
// header comment on why (mouse0dat is provably 0 throughout this file).
static void ensure_joy1enable() {
    set_bit(top->_joy2, 4, 0);
    top->mouse_btn = top->mouse_btn & ~0x1;
    settle(4);
}

// JOYTEST write: test_load is combinational on the address
// (reg_address_in==JOYTEST), so it is only live for the cycle(s) the
// address is held there. One clocked cycle with data_in=value loads
// value[15:10] and value[7:2] into whichever of dmouse0dat/dmouse1dat/
// xcount/ycount the port's own always-block accepts from test_data on
// that edge (userio.v:232-282,341-343); address is then moved off
// JOYTEST so later reads are not affected by test_load being asserted.
static void write_joytest(uint16_t value) {
    set_addr(JOYTEST_ADDR);
    top->data_in = value;
    tick();
    set_addr(0);
}

// Sets the two quadrature-state bits ([9:8] or [1:0] of a counter byte)
// to the 2-bit code the BIT# table on p.281 predicts for a given pair of
// controller-pin levels, using the module's own static (no-transition)
// formula {!pinA, pinA^pinB} (userio.v:239,252,266,279 -- the trailing
// "else" arm of each per-byte always-block). Solves pinA=!code[1],
// pinB=pinA^code[0] for the desired 2-bit code. Used only by the retained
// JOYTEST-seeded checks below.
static void solve_pins(int code, int& pinA, int& pinB) {
    pinA = !((code >> 1) & 1);
    pinB = pinA ^ (code & 1);
}

// reg_is_joy0: true drives the pins that feed dmouse0dat (JOY0DAT's
// quadrature bytes) -- which MEASURED evidence in this package's claims
// file, confirmed by reading userio.v:213-219, is module input _joy2 (a
// cross-wire: _sjoy1<=_joy2), not _joy1. false drives _joy1, which feeds
// dmouse1dat (JOY1DAT) via the same cross-wire (_sjoy2<=_joy1). Shared by
// both the retained JOYTEST-seeded checks and the new motion-based ones.
static void set_quad_static(bool reg_is_joy0, int code98, int code10) {
    int pinA98, pinB98, pinA10, pinB10;
    solve_pins(code98, pinA98, pinB98);
    solve_pins(code10, pinA10, pinB10);
    vluint16_t& pins = reg_is_joy0 ? top->_joy2 : top->_joy1;
    set_bit(pins, 1, pinA98);
    set_bit(pins, 3, pinB98);
    set_bit(pins, 0, pinA10);
    set_bit(pins, 2, pinB10);
}

// Drives an arbitrary 16-bit value into JOY0DAT (joystick-mode, i.e.
// ensure_joy1enable() must already have been called) or JOY1DAT: bits
// [15:10] and [7:2] land via a JOYTEST write of the value itself (those
// slices of test_data pass straight through, userio.v:260,273,342-343),
// bits [9:8] and [1:0] land via the two static pin pairs. Used only by
// the retained JOYTEST-seeded checks (control case, width, 8-bit-pair) --
// NOT by the new motion-based checks, which is exactly the repair: this
// helper's low-two-bit formula was copied out of the RTL, so it cannot
// falsify anything the RTL does about which byte is which axis.
static void drive_register(bool reg_is_joy0, uint16_t value) {
    write_joytest(value);
    settle(2);
    set_quad_static(reg_is_joy0, (value >> 8) & 0x3, value & 0x3);
    settle(3);
}

static uint16_t addr_of(bool reg_is_joy0) { return reg_is_joy0 ? JOY0DAT_ADDR : JOY1DAT_ADDR; }

// ---------------------------------------------------------------------------
// Quadrature-motion primitives -- the repair. axis 0 = horizontal/X:
// main=bit2 ("H"), quad=bit0 ("HQ"); axis 1 = vertical/Y: main=bit3 ("V"),
// quad=bit1 ("VQ") of whichever of _joy1/_joy2 feeds the register under
// test (userio.v:234-252 port 1, :261-279 port 2). Technique read from
// hrm/ch8-mouse-quadrature-decode/test_userio.cpp -- see the header
// comment above for what was read and how it is adapted here.
// ---------------------------------------------------------------------------

// One registered quadrature step: two clk7_en-gated edges, because the
// transition-detect compares the live synchronized pin level against a
// second synchronizer stage one edge further behind (userio.v:213-219) --
// a single edge only updates the first stage.
static void quad_step(bool reg_is_joy0, int axis, int main_lvl, int quad_lvl) {
    vluint16_t& pins = reg_is_joy0 ? top->_joy2 : top->_joy1;
    int main_bit = axis ? 3 : 2;
    int quad_bit = axis ? 1 : 0;
    set_bit(pins, main_bit, main_lvl);
    set_bit(pins, quad_bit, quad_lvl);
    settle(2);
}

// Establishes a genuine, stable, all-zero baseline (all 16 bits) for one
// register's underlying quadrature counters. Neither dmouse0dat nor
// dmouse1dat has a reset branch of its own (userio.v:229-282), so this has
// to be driven, not assumed. Holding BOTH axes' pin pairs at the static
// level (main=1,quad=1) makes the RTL's own idle "else" branch compute
// {!1,1^1}={0,0}=0 for the low two bits of both bytes and keep
// recomputing that same 0 every idle cycle the pins stay put (no drift).
// A JOYTEST write with data_in=0 then zeroes the upper six bits of each
// byte (unconditionally for JOY0DAT, userio.v:232-233/245-246; for
// JOY1DAT only bits [7:2]/[15:10] are JOYTEST-writable, userio.v:259-260/
// 272-273 -- but the low two are already 0 by the time JOYTEST fires, and
// JOYTEST never touches them, so the result is the same either way).
static void zero_baseline(bool reg_is_joy0) {
    vluint16_t& pins = reg_is_joy0 ? top->_joy2 : top->_joy1;
    pins = 0x0F; // both axes' main+quad bits (0,1,2,3) held at 1
    settle(6);   // >= two-stage synchronizer convergence, with margin
    write_joytest(0);
    settle(3);   // re-confirm the idle latch reproduces 0 with pins unchanged
}

// Walks exactly n valid single-step quadrature transitions on one axis,
// starting from the (main=1,quad=1) phase zero_baseline() left it at
// (phase index 2 in both sequences below -- the ring is cyclic, so
// starting mid-ring does not change which steps count as up or down).
// Case 1 (main changes first, Figure 8-2's "Count Up") if up==true, Case 2
// ("Count Down") otherwise -- ring order and sense read from the sibling
// package's walk_ring(), whose own mutation log (VERTICAL-CASE1-COUNTS-UP
// / -CASE2-COUNTS-DOWN / HORIZONTAL-RIGHT-INCREMENTS / -LEFT-DECREMENTS)
// confirms this exact sequence on this fixture.
static void walk_axis(bool reg_is_joy0, int axis, bool up, int n) {
    static const int case1[4][2] = {{0,0},{1,0},{1,1},{0,1}};
    static const int case2[4][2] = {{0,0},{0,1},{1,1},{1,0}};
    const int (*seq)[2] = up ? case1 : case2;
    int phase = 2; // (1,1), zero_baseline()'s own resting phase
    for (int i = 0; i < n; i++) {
        phase = (phase + 1) & 3;
        quad_step(reg_is_joy0, axis, seq[phase][0], seq[phase][1]);
    }
}

// ---------------------------------------------------------------------------
// Checks -- one function per family of assertions, called once per uid.
// ---------------------------------------------------------------------------

// opus:8-joydat.c1 -- control case / fixture liveness. Unchanged by the
// repair: register readability/liveness does not depend on which
// mechanism loads the registers.
static void check_control_case(const char* uid) {
    reset_dut();
    ensure_joy1enable();
    uint16_t idle0 = read_reg(JOY0DAT_ADDR);
    uint16_t idle1 = read_reg(JOY1DAT_ADDR);
    note("idle JOY0DAT=0x" + hex4(idle0) + " idle JOY1DAT=0x" + hex4(idle1) + " immediately after reset, before any stimulus");
    const uint16_t markerA = 0x1357, markerB = 0x2468;
    bool skip = mutated(uid); // mutation: never drive a distinct value at all
    if (!skip) drive_register(true, markerA);
    uint16_t v0 = read_reg(JOY0DAT_ADDR);
    if (!skip) drive_register(false, markerB);
    uint16_t v1 = read_reg(JOY1DAT_ADDR);
    note("after stimulus JOY0DAT=0x" + hex4(v0) + " JOY1DAT=0x" + hex4(v1));
    bool ok = (v0 == markerA) && (v1 == markerB) && (v0 != idle0) && (v1 != idle1);
    expect(ok, uid);
}

// opus:8-joydat.c4 -- both registers 16 bits wide (collective). Unchanged
// by the repair: width is not a mouse-motion claim.
static void check_width_both(const char* uid) {
    reset_dut();
    ensure_joy1enable();
    uint16_t stim = 0x8001;
    if (mutated(uid)) stim = 0x8000; // drop bit0 from the stimulus
    drive_register(true, stim);
    uint16_t v0 = read_reg(JOY0DAT_ADDR);
    drive_register(false, stim);
    uint16_t v1 = read_reg(JOY1DAT_ADDR);
    expect(v0 == 0x8001 && v1 == 0x8001, uid);
}

// sol:8-mouse.c15 / c16 -- one register's 16-bit width. Unchanged.
static void check_width_one(bool reg_is_joy0, const char* uid) {
    reset_dut();
    if (reg_is_joy0) ensure_joy1enable();
    uint16_t stim = 0x8001;
    if (mutated(uid)) stim = 0x8000;
    drive_register(reg_is_joy0, stim);
    uint16_t v = read_reg(addr_of(reg_is_joy0));
    expect(v == 0x8001, uid);
}

// opus:8-joydat.c5/c6, sol:8-mouse.c17/c18 -- vertical/horizontal byte
// assignment, both registers. REPAIRED: previously seeded a nonzero byte
// through JOYTEST and read it back at the claimed position -- which only
// tests that a loaded byte lands where it was loaded, not that VERTICAL
// controller-pin motion is what lands in bits 15-8 and HORIZONTAL motion
// in bits 7-0. Now drives real quadrature transitions on the claimed
// axis's own pins (vertical=Y pins, horizontal=X pins) from a zeroed
// baseline and checks the resulting byte -- and only that byte -- carries
// the step count.
static void check_byte_assignment(bool checking_vertical, const char* uid) {
    const int STEPS = 53;
    bool ok = true;
    std::string meas;
    for (int reg = 0; reg < 2; reg++) {
        bool reg_is_joy0 = (reg == 0);
        reset_dut();
        zero_baseline(reg_is_joy0);
        int axis = checking_vertical ? 1 : 0; // 1=vertical/Y, 0=horizontal/X
        if (mutated(uid)) axis = 1 - axis; // drive the OTHER axis -- the swapped-byte defect the validation named
        walk_axis(reg_is_joy0, axis, /*up=*/true, STEPS);
        uint16_t v = read_reg(addr_of(reg_is_joy0));
        uint8_t hi = (uint8_t)(v >> 8), lo = (uint8_t)(v & 0xFF);
        bool this_ok = checking_vertical ? (hi == STEPS && lo == 0) : (lo == STEPS && hi == 0);
        ok = ok && this_ok;
        meas += (reg_is_joy0 ? " JOY0DAT=0x" : " JOY1DAT=0x") + hex4(v);
    }
    note("byte-assignment probe (" + std::string(checking_vertical ? "vertical->high byte" : "horizontal->low byte") +
         "), " + std::to_string(STEPS) + " quadrature steps driven on the claimed axis from a zero baseline:" + meas);
    expect(ok, uid);
}

// opus:8-appa-joy.c9/c10, sol:8-appa-joy.c11/c12 -- 8-bit-wide counter
// pair. scope: 0=both registers, 1=JOY0DAT only, 2=JOY1DAT only. Unchanged
// by the repair: field width/overflow is not a mouse-motion claim.
static void check_8bit_pair(int scope, const char* uid) {
    reset_dut();
    ensure_joy1enable();
    uint16_t stim = 0xFFFF;
    if (mutated(uid)) stim = 0x7F7F; // MSB of each byte withheld from the stimulus
    bool ok = true;
    if (scope == 0 || scope == 1) {
        drive_register(true, stim);
        ok = ok && (read_reg(JOY0DAT_ADDR) == 0xFFFF);
    }
    if (scope == 0 || scope == 2) {
        drive_register(false, stim);
        ok = ok && (read_reg(JOY1DAT_ADDR) == 0xFFFF);
    }
    expect(ok, uid);
}

// opus:8-appa-joy.c13, sol:8-appa-joy.c15 -- four counters total.
// REPAIRED: previously loaded four distinct JOYTEST-seeded bytes and
// checked they read back distinct -- never drove a controller pin. Now
// zeroes all four counters and drives four distinct, known quadrature
// step counts, one per axis per register, checking each byte reads back
// its own count and none has leaked into another.
static void check_four_counters(const char* uid) {
    reset_dut();
    zero_baseline(true);
    zero_baseline(false);
    const int N0X = 17, N0Y = 29, N1X = 43, N1Y = 61; // four distinct, known counts
    bool skip_1y = mutated(uid); // mutation: JOY1DAT's vertical counter never driven
    walk_axis(true,  0, true, N0X);
    walk_axis(true,  1, true, N0Y);
    walk_axis(false, 0, true, N1X);
    if (!skip_1y) walk_axis(false, 1, true, N1Y);
    uint16_t v0 = read_reg(JOY0DAT_ADDR);
    uint16_t v1 = read_reg(JOY1DAT_ADDR);
    uint8_t v0x = (uint8_t)(v0 & 0xFF), v0y = (uint8_t)(v0 >> 8);
    uint8_t v1x = (uint8_t)(v1 & 0xFF), v1y = (uint8_t)(v1 >> 8);
    note("four-counters probe: JOY0DAT.X=" + std::to_string(v0x) + " (want " + std::to_string(N0X) +
         ") JOY0DAT.Y=" + std::to_string(v0y) + " (want " + std::to_string(N0Y) +
         ") JOY1DAT.X=" + std::to_string(v1x) + " (want " + std::to_string(N1X) +
         ") JOY1DAT.Y=" + std::to_string(v1y) + " (want " + std::to_string(N1Y) + ")");
    bool ok = (v0x == N0X) && (v0y == N0Y) && (v1x == N1X) && (v1y == N1Y) &&
              (v0x != v0y) && (v1x != v1y) && (v0 != v1);
    expect(ok, uid);
}

// opus:8-appa-joy.c14, sol:8-appa-joy.c16 -- each counter clocked by two
// controller pins. REPAIRED: previously toggled real pins but only for
// ONE of the four counters (JOY1DAT's low byte); the other three counters
// this claim covers were never touched. Now checks all four: for each of
// JOY0DAT.X, JOY0DAT.Y, JOY1DAT.X, JOY1DAT.Y, driving only the main pin
// (holding quad fixed) moves the counter, and then driving only the quad
// pin (holding main at its new level) moves it again -- two independent
// input lines each affect every one of the four counters.
static void check_two_pins_per_counter(const char* uid) {
    reset_dut();
    bool ok = true;
    std::string meas;
    for (int reg = 0; reg < 2; reg++) {
        bool reg_is_joy0 = (reg == 0);
        uint16_t addr = addr_of(reg_is_joy0);
        vluint16_t& pins = reg_is_joy0 ? top->_joy2 : top->_joy1;
        for (int axis = 0; axis < 2; axis++) {
            int main_bit = axis ? 3 : 2;
            int quad_bit = axis ? 1 : 0;
            set_bit(pins, main_bit, 0);
            set_bit(pins, quad_bit, 0);
            settle(4);
            uint16_t base = read_reg(addr);
            uint8_t base_b = axis ? (uint8_t)(base >> 8) : (uint8_t)(base & 0xFF);
            set_bit(pins, main_bit, 1); // pin A (main) alone
            settle(4);
            uint16_t after_a = read_reg(addr);
            uint8_t a_b = axis ? (uint8_t)(after_a >> 8) : (uint8_t)(after_a & 0xFF);
            // mutation: withhold pin B on one representative counter (JOY1DAT/horizontal)
            bool skip_b = mutated(uid) && (reg == 1) && (axis == 0);
            if (!skip_b) set_bit(pins, quad_bit, 1); // pin B (quad) alone, A already at its new level
            settle(4);
            uint16_t after_b = read_reg(addr);
            uint8_t b_b = axis ? (uint8_t)(after_b >> 8) : (uint8_t)(after_b & 0xFF);
            bool this_ok = (a_b != base_b) && (b_b != a_b);
            ok = ok && this_ok;
            meas += " reg" + std::to_string(reg) + "axis" + std::to_string(axis) +
                    ":base=" + std::to_string(base_b) + " afterA=" + std::to_string(a_b) +
                    " afterB=" + std::to_string(b_b);
        }
    }
    note("two-pins-per-counter probe, all four counters:" + meas);
    expect(ok, uid);
}

struct BitClaim { int bit; char letter; int idx; const char* uid; };

static const BitClaim JOY0_BITS[16] = {
    { 15, 'Y', 7, "opus:8-appa-joy.c19" },
    { 14, 'Y', 6, "opus:8-appa-joy.c20" },
    { 13, 'Y', 5, "opus:8-appa-joy.c21" },
    { 12, 'Y', 4, "opus:8-appa-joy.c22" },
    { 11, 'Y', 3, "opus:8-appa-joy.c23" },
    { 10, 'Y', 2, "opus:8-appa-joy.c24" },
    {  9, 'Y', 1, "opus:8-appa-joy.c25" },
    {  8, 'Y', 0, "opus:8-appa-joy.c26" },
    {  7, 'X', 7, "opus:8-appa-joy.c27" },
    {  6, 'X', 6, "opus:8-appa-joy.c28" },
    {  5, 'X', 5, "opus:8-appa-joy.c29" },
    {  4, 'X', 4, "opus:8-appa-joy.c30" },
    {  3, 'X', 3, "opus:8-appa-joy.c31" },
    {  2, 'X', 2, "opus:8-appa-joy.c32" },
    {  1, 'X', 1, "opus:8-appa-joy.c33" },
    {  0, 'X', 0, "opus:8-appa-joy.c34" },
};

static const BitClaim JOY1_BITS[16] = {
    { 15, 'Y', 7, "opus:8-appa-joy.c35" },
    { 14, 'Y', 6, "opus:8-appa-joy.c36" },
    { 13, 'Y', 5, "opus:8-appa-joy.c37" },
    { 12, 'Y', 4, "opus:8-appa-joy.c38" },
    { 11, 'Y', 3, "opus:8-appa-joy.c39" },
    { 10, 'Y', 2, "opus:8-appa-joy.c40" },
    {  9, 'Y', 1, "opus:8-appa-joy.c41" },
    {  8, 'Y', 0, "opus:8-appa-joy.c42" },
    {  7, 'X', 7, "opus:8-appa-joy.c43" },
    {  6, 'X', 6, "opus:8-appa-joy.c44" },
    {  5, 'X', 5, "opus:8-appa-joy.c45" },
    {  4, 'X', 4, "opus:8-appa-joy.c46" },
    {  3, 'X', 3, "opus:8-appa-joy.c47" },
    {  2, 'X', 2, "opus:8-appa-joy.c48" },
    {  1, 'X', 1, "opus:8-appa-joy.c49" },
    {  0, 'X', 0, "opus:8-appa-joy.c50" },
};

// Per-bit p.281 table claims (opus:8-appa-joy.c19..c50). REPAIRED:
// previously JOYTEST-loaded a one-hot 16-bit word directly and read it
// back -- proves only that the load-and-readback path preserves a bit
// position, not that counter bit k (Y_k or X_k) is what that register bit
// reflects. Now zeroes the target counter and drives exactly 2^(bit
// within its own byte) quadrature UP steps from that zero: since the
// counter is a plain binary up-counter (dmouse0dat[7:0] <= ...+1'd1,
// userio.v:235 et al.) and its own bit `idx` was 0 in the zero baseline,
// adding 2^idx sets bit `idx` alone with no carry into or out of it --
// so the byte reads back exactly 1<<idx, i.e. a 1 at bit `c.bit` of the
// register and 0 everywhere else, reached by counting the claimed
// counter's own transitions rather than by loading a word.
static void check_one_hot_bit(bool reg_is_joy0, const BitClaim& c) {
    reset_dut();
    zero_baseline(reg_is_joy0);
    bool is_y = (c.letter == 'Y');
    int axis = is_y ? 1 : 0;
    int steps = 1 << c.idx;
    if (mutated(c.uid)) steps = 1 << ((c.idx + 1) % 8); // one-hot moved to the wrong bit of the same byte
    walk_axis(reg_is_joy0, axis, /*up=*/true, steps);
    uint16_t v = read_reg(addr_of(reg_is_joy0));
    uint16_t expected = (uint16_t)(1u << c.bit);
    bool ok = (v == expected);
    expect(ok, c.uid);
}

// sol:8-appa-joy.c21..c24 -- byte-range restatements of the same table.
// REPAIRED: previously JOYTEST-loaded the marker byte directly. Now drives
// the marker's own value as a quadrature UP step count from a zero
// baseline on the claimed axis, so the byte is reached by counting real
// transitions on the axis the claim names, not by a register write.
static void check_byte_range(bool reg_is_joy0, bool upper, const char* uid) {
    reset_dut();
    zero_baseline(reg_is_joy0);
    const uint16_t EXPECTED = 0x96; // 150 decimal, 10010110: not a palindrome, catches bit-order errors
    int axis = upper ? 1 : 0; // upper byte (15-8) = vertical/Y, lower (7-0) = horizontal/X
    int steps = EXPECTED;
    if (mutated(uid)) steps = EXPECTED ^ 0x01; // one transition short/over of the required count
    walk_axis(reg_is_joy0, axis, /*up=*/true, steps);
    uint16_t v = read_reg(addr_of(reg_is_joy0));
    uint8_t got = upper ? (uint8_t)(v >> 8) : (uint8_t)(v & 0xFF);
    expect(got == EXPECTED, uid);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vuserio;

    note("Verilator 5.052, userio.v built alone, --top-module userio, fixture ch8-joydat-counter-layout");

    check_control_case("opus:8-joydat.c1");

    check_width_both("opus:8-joydat.c4");
    check_width_one(true,  "sol:8-mouse.c15");
    check_width_one(false, "sol:8-mouse.c16");

    check_byte_assignment(true,  "opus:8-joydat.c5");
    check_byte_assignment(true,  "sol:8-mouse.c17");
    check_byte_assignment(false, "opus:8-joydat.c6");
    check_byte_assignment(false, "sol:8-mouse.c18");

    record("opus:8-joydat.c2 declined: p.230 numbers the JOY0DAT port \"1\"; p.233 numbers what this claims file's own conflict_resolution field says is the same pair of registers differently. p.233 is outside this package's assigned pages (230, 281), so the dispute cannot be adjudicated here.");
    record("opus:8-joydat.c3 declined: same p.230/p.233 port-number dispute as opus:8-joydat.c2, for JOY1DAT/port 2.");
    record("sol:8-mouse.c13 declined: same p.230/p.233 port-number dispute as opus:8-joydat.c2, restated by the other reader.");
    record("sol:8-mouse.c14 declined: same p.230/p.233 port-number dispute as opus:8-joydat.c2, restated by the other reader, for JOY1DAT/port 2.");

    check_8bit_pair(0, "opus:8-appa-joy.c9");
    check_8bit_pair(0, "opus:8-appa-joy.c10");
    check_8bit_pair(1, "sol:8-appa-joy.c11");
    check_8bit_pair(2, "sol:8-appa-joy.c12");

    check_four_counters("opus:8-appa-joy.c13");
    check_four_counters("sol:8-appa-joy.c15");

    check_two_pins_per_counter("opus:8-appa-joy.c14");
    check_two_pins_per_counter("sol:8-appa-joy.c16");

    for (const auto& c : JOY0_BITS) check_one_hot_bit(true, c);
    for (const auto& c : JOY1_BITS) check_one_hot_bit(false, c);

    check_byte_range(true,  true,  "sol:8-appa-joy.c21");
    check_byte_range(true,  false, "sol:8-appa-joy.c22");
    check_byte_range(false, true,  "sol:8-appa-joy.c23");
    check_byte_range(false, false, "sol:8-appa-joy.c24");

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";

    delete top;
    return checks.failed == 0 ? 0 : 1;
}
