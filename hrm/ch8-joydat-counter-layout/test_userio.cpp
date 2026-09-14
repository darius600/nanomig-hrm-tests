// test_userio.cpp -- ch8-joydat-counter-layout
//
// Fixture: userio.v, compiled alone (Verilator, --top-module userio).
// Grades the 52 testable propositions in SPECIFICATION.md against the
// live module. 4 further claims (the port-1/port-2 label dispute) are
// declined via record() in main() and never appear on a graded line.
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
// MUTATION opus:8-joydat.c1 stimulus red stimulus never driven: JOY0DAT/JOY1DAT left at their measured idle 0x0202 instead of writing the two distinct markers 0x1357/0x2468
// MUTATION opus:8-joydat.c4 stimulus red bit0 dropped from the shared 0x8001 stimulus (0x8000 written instead) to both registers
// MUTATION sol:8-mouse.c15 stimulus red bit0 dropped from the 0x8001 stimulus (0x8000 written instead) to JOY0DAT
// MUTATION sol:8-mouse.c16 stimulus red bit0 dropped from the 0x8001 stimulus (0x8000 written instead) to JOY1DAT
// MUTATION opus:8-joydat.c5 stimulus red low byte polluted with 0xCD alongside the intended high-byte-only marker, both registers
// MUTATION sol:8-mouse.c17 stimulus red low byte polluted with 0xCD alongside the intended high-byte-only marker, both registers
// MUTATION opus:8-joydat.c6 stimulus red high byte polluted with 0xCD alongside the intended low-byte-only marker, both registers
// MUTATION sol:8-mouse.c18 stimulus red high byte polluted with 0xCD alongside the intended low-byte-only marker, both registers
// MUTATION opus:8-appa-joy.c9 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), both registers
// MUTATION opus:8-appa-joy.c10 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), both registers
// MUTATION sol:8-appa-joy.c11 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), JOY0DAT only
// MUTATION sol:8-appa-joy.c12 stimulus red 0xFFFF stimulus narrowed to 0x7F7F (MSB of each byte withheld), JOY1DAT only
// MUTATION opus:8-appa-joy.c13 stimulus red JOY1DAT driven with JOY0DAT's own pattern 0x1122 instead of its distinct 0x3344
// MUTATION sol:8-appa-joy.c15 stimulus red JOY1DAT driven with JOY0DAT's own pattern 0x1122 instead of its distinct 0x3344
// MUTATION opus:8-appa-joy.c14 stimulus red second controller pin (_joy1[2]) never toggled after the first (_joy1[0]) was
// MUTATION sol:8-appa-joy.c16 stimulus red second controller pin (_joy1[2]) never toggled after the first (_joy1[0]) was
// MUTATION opus:8-appa-joy.c19 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c20 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c21 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c22 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c23 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c24 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c25 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c26 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c27 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c28 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c29 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c30 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c31 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c32 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c33 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c34 stimulus red one-hot stimulus bit moved from its claimed JOY0DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c35 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c36 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c37 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c38 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c39 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c40 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c41 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c42 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c43 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c44 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c45 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c46 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c47 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c48 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c49 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION opus:8-appa-joy.c50 stimulus red one-hot stimulus bit moved from its claimed JOY1DAT position to (position+1)%16 while the check still reads the claimed position
// MUTATION sol:8-appa-joy.c21 stimulus red marker byte 0x96 XORed with 0x01 in the stimulus before driving JOY0DAT's claimed high byte
// MUTATION sol:8-appa-joy.c22 stimulus red marker byte 0x96 XORed with 0x01 in the stimulus before driving JOY0DAT's claimed low byte
// MUTATION sol:8-appa-joy.c23 stimulus red marker byte 0x96 XORed with 0x01 in the stimulus before driving JOY1DAT's claimed high byte
// MUTATION sol:8-appa-joy.c24 stimulus red marker byte 0x96 XORed with 0x01 in the stimulus before driving JOY1DAT's claimed low byte

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
// pinB=pinA^code[0] for the desired 2-bit code.
static void solve_pins(int code, int& pinA, int& pinB) {
    pinA = !((code >> 1) & 1);
    pinB = pinA ^ (code & 1);
}

// reg_is_joy0: true drives the pins that feed dmouse0dat (JOY0DAT's
// quadrature bytes) -- which MEASURED evidence in this package's claims
// file, confirmed by reading userio.v:213-219, is module input _joy2 (a
// cross-wire: _sjoy1<=_joy2), not _joy1. false drives _joy1, which feeds
// dmouse1dat (JOY1DAT) via the same cross-wire (_sjoy2<=_joy1).
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
// bits [9:8] and [1:0] land via the two static pin pairs.
static void drive_register(bool reg_is_joy0, uint16_t value) {
    write_joytest(value);
    settle(2);
    set_quad_static(reg_is_joy0, (value >> 8) & 0x3, value & 0x3);
    settle(3);
}

static uint16_t addr_of(bool reg_is_joy0) { return reg_is_joy0 ? JOY0DAT_ADDR : JOY1DAT_ADDR; }

// ---------------------------------------------------------------------------
// Checks -- one function per family of assertions, called once per uid.
// ---------------------------------------------------------------------------

// opus:8-joydat.c1 -- control case / fixture liveness.
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

// opus:8-joydat.c4 -- both registers 16 bits wide (collective).
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

// sol:8-mouse.c15 / c16 -- one register's 16-bit width.
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
// assignment, both registers.
static void check_byte_assignment(bool checking_vertical, const char* uid) {
    reset_dut();
    ensure_joy1enable();
    const uint8_t nz = 0xAB;
    uint16_t stim = checking_vertical ? (uint16_t)(nz << 8) : (uint16_t)nz;
    if (mutated(uid)) {
        // pollute the byte that must stay 0
        stim = checking_vertical ? (uint16_t)(stim | 0x00CD) : (uint16_t)(stim | 0xCD00);
    }
    drive_register(true, stim);
    uint16_t v0 = read_reg(JOY0DAT_ADDR);
    drive_register(false, stim);
    uint16_t v1 = read_reg(JOY1DAT_ADDR);
    bool ok;
    if (checking_vertical)
        ok = ((v0 >> 8) == nz) && ((v0 & 0xFF) == 0) && ((v1 >> 8) == nz) && ((v1 & 0xFF) == 0);
    else
        ok = ((v0 & 0xFF) == nz) && ((v0 >> 8) == 0) && ((v1 & 0xFF) == nz) && ((v1 >> 8) == 0);
    expect(ok, uid);
}

// opus:8-appa-joy.c9/c10, sol:8-appa-joy.c11/c12 -- 8-bit-wide counter
// pair. scope: 0=both registers, 1=JOY0DAT only, 2=JOY1DAT only.
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
static void check_four_counters(const char* uid) {
    reset_dut();
    ensure_joy1enable();
    uint16_t j0 = 0x1122, j1 = 0x3344;
    if (mutated(uid)) j1 = 0x1122; // JOY1DAT driven with JOY0DAT's own pattern
    drive_register(true, j0);
    uint16_t v0 = read_reg(JOY0DAT_ADDR);
    drive_register(false, j1);
    uint16_t v1 = read_reg(JOY1DAT_ADDR);
    bool ok = (v0 == 0x1122) && (v1 == 0x3344) &&
              ((v0 >> 8) != (v0 & 0xFF)) && ((v1 >> 8) != (v1 & 0xFF)) && (v0 != v1);
    expect(ok, uid);
}

// opus:8-appa-joy.c14, sol:8-appa-joy.c16 -- each counter clocked by two
// controller pins. Uses JOY1DAT's low byte (dmouse1dat[1:0], fed by
// module input _joy1[0] and _joy1[2] via the cross-wired synchronizer,
// userio.v:266) so this check does not depend on the joystick-mode mux
// used elsewhere for JOY0DAT.
static void check_two_pins_per_counter(const char* uid) {
    reset_dut();
    set_bit(top->_joy1, 0, 0);
    set_bit(top->_joy1, 2, 0);
    settle(4);
    uint16_t base = read_reg(JOY1DAT_ADDR) & 0x3;
    set_bit(top->_joy1, 0, 1); // pin A alone
    settle(4);
    uint16_t after_a = read_reg(JOY1DAT_ADDR) & 0x3;
    bool skip_b = mutated(uid); // mutation: pin B is never actually moved
    if (!skip_b) set_bit(top->_joy1, 2, 1); // pin B alone (A already at its new level)
    settle(4);
    uint16_t after_b = read_reg(JOY1DAT_ADDR) & 0x3;
    note("two-pin probe: base=" + std::to_string(base) + " after A=" + std::to_string(after_a) + " after B=" + std::to_string(after_b));
    bool ok = (after_a != base) && (after_b != after_a);
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

// Per-bit p.281 table claims (opus:8-appa-joy.c19..c50): drive exactly one
// bit of the target register to 1 (all others 0) using drive_register(),
// then check that bit -- and only that bit -- reads back 1.
static void check_one_hot_bit(bool reg_is_joy0, const BitClaim& c) {
    reset_dut();
    if (reg_is_joy0) ensure_joy1enable();
    int stim_bit = c.bit;
    if (mutated(c.uid)) stim_bit = (c.bit + 1) % 16; // one-hot moved to the wrong position
    drive_register(reg_is_joy0, (uint16_t)(1u << stim_bit));
    uint16_t v = read_reg(addr_of(reg_is_joy0));
    bool ok = (v == (uint16_t)(1u << c.bit));
    expect(ok, c.uid);
}

// sol:8-appa-joy.c21..c24 -- byte-range restatements of the same table.
static void check_byte_range(bool reg_is_joy0, bool upper, const char* uid) {
    reset_dut();
    if (reg_is_joy0) ensure_joy1enable();
    const uint8_t marker = 0x96; // 1001 0110: not a palindrome, catches bit-order errors
    uint16_t stim = upper ? (uint16_t)(marker << 8) : (uint16_t)marker;
    if (mutated(uid)) {
        uint8_t bad = marker ^ 0x01;
        stim = upper ? (uint16_t)(bad << 8) : (uint16_t)bad;
    }
    drive_register(reg_is_joy0, stim);
    uint16_t v = read_reg(addr_of(reg_is_joy0));
    uint8_t got = upper ? (uint8_t)(v >> 8) : (uint8_t)(v & 0xFF);
    expect(got == marker, uid);
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

