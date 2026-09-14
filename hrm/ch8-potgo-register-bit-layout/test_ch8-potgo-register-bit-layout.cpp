// Test purpose: Verify POTGO's and POTGOR's bit layout -- the eight OUT/DAT
// cells for the four pot lines, the START bit, and what each bit position
// means -- against userio.v, which has never had a test package before this
// one.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
// p.240 (ch.8 Table 8-4 and its two explanatory paragraphs), p.282
// (Appendix A POTGO/POTGOR entry), and Figure 8-6 (p.237); committed
// specification 67d4b32 (SPECIFICATION.md, this package).
// Requirement IDs: PROP-1 through PROP-13, per SPECIFICATION.md's claim
// inventory (all 81 claims in
// hrm-testpipeline/work/ch8-s1-build/ch8-potgo-register-bit-layout.jsonl are
// accounted for there; uids are not repeated in each label below to keep
// them readable -- see SPECIFICATION.md's inventory table for the uid list
// behind each PROP).
// Non-goals, per SPECIFICATION.md's "Claims not assigned an assertion":
// pot-counter clocking mechanics beyond what PROP-10 needs to observe
// START's own effect; POT1DAT/POTGO/POTGOR's own register addresses and
// widths; POTGOR's former name POTINP; the Figure 8-6 output-driver
// topology (three unlabelled buffer triangles).
//
// Fixture liveness: userio.v has never been measured in this suite before.
// FIXTURE-LIVE runs first, before any other assertion, and proves the
// module accepts two DISTINCT, known 16-bit POTGO words and reflects each
// correctly and distinctly on POTGOR.
//
// RTL facts established by opening userio.v for this package (step 2, after
// SPECIFICATION.md was committed -- none of this shaped the specification
// itself):
//   - potreg (userio.v:78,98-105) is POTGO's write register. Bit 0 (START)
//     self-clears one clk7_en cycle after any write that set it
//     (userio.v:102-104: on every clk7_en cycle whose address is NOT POTGO,
//     potreg[0] <= 0), and the pot-counter reset block (userio.v:125-133)
//     reads potreg[0] to zero all four counters when it sees it set.
//   - The four data bits are backed by potcap[3:0] (userio.v:122,159-178):
//       potcap[3] (bit 14, DATRY) = _djoy2[5] & ~(potreg[15] & ~potreg[14])
//       potcap[2] (bit 12, DATRX) = ~(potreg[13] & ~potreg[12])  -- no
//         external term at all; confirmed independently here, matching
//         what this package's own claims file's prior reading had already
//         found (its `split` field on 8-dio.c13/c11 and appa-potgo.c15/
//         c43): no input reaches this bit's readback.
//       potcap[1] (bit 10, DATLY) = _mright & _djoy1[5] &
//         ~(potreg[11] & ~potreg[10])
//       potcap[0] (bit 8, DATLX)  = _mthird & ~(potreg[9] & ~potreg[8])
//     _mright = ~mouse_btn[1], _mthird = ~mouse_btn[2] (userio.v:361-363).
//     _djoy1/_djoy2 are 2-clk-delayed, and CROSS-WIRED at joy_swap=1'b0
//     (userio.v:92,210-219): _djoy1[5] traces to the module's own `_joy2`
//     input port, _djoy2[5] to `_joy1`. This is an internal wiring fact of
//     this module and does not change what is asserted (bit identity and
//     role), only which of this harness's own input ports must be driven to
//     supply each pair's external term.
//   - The read mux (userio.v:296-309) forces
//     `data_out = {1'b0,potcap[3],1'b0,potcap[2],1'b0,potcap[1],1'b0,
//     potcap[0],8'h00}` for POTGOR/POTINP: bits 15,13,11,9 and 7-0 are
//     literal Verilog constants in that concatenation -- no signal path
//     reaches them at all. This is why PROP-9 and PROP-13 are recorded
//     `none impossible` below: no stimulus in the module's input space can
//     move a hardwired constant.
//   - joy_ana_en, cd32pad are hardwired 1'b0 (userio.v:91,93) in this build,
//     so the CD32-pad and analogue-joystick branches referencing them are
//     dead code; the plain digital pot-counter path (userio.v:139-140,
//     145-147) is what PROP-10 exercises.
//
// MUTATION FIXTURE-LIVE stimulus red: repeated the first POTGO word (0xAA00)
// instead of writing the second, distinct word (0xFF00) -> the second
// reading no longer differs from the first (both 0x0000) -> FAIL as
// expected (16 passed, 1 failed at the point this was run in isolation).
// Reverted.
// MUTATION PROP-1 stimulus red: redirected the write from bit 15 (OUTRY) to
// bit 13 (OUTRX) while still reading bit 14 -> bit 14 no longer moves ->
// FAIL as expected. Reverted.
// MUTATION PROP-2 stimulus red: redirected the write from bit 14 (DATRY) to
// bit 12 (DATRX) while OUTRY stayed the bit driven for the enable half and
// bit 14 stayed the bit read -> FAIL as expected. Reverted.
// MUTATION PROP-3 stimulus red: redirected the write from bit 13 (OUTRX) to
// bit 11 (OUTLY) while still reading bit 12 -> FAIL as expected. Reverted.
// MUTATION PROP-4 stimulus red: redirected the write from bit 12 (DATRX) to
// bit 10 (DATLY) while still reading bit 12 -> FAIL as expected. Reverted.
// MUTATION PROP-5 stimulus red: redirected the write from bit 11 (OUTLY) to
// bit 9 (OUTLX) while still reading bit 10 -> FAIL as expected. Reverted.
// MUTATION PROP-6 stimulus red: redirected the write from bit 10 (DATLY) to
// bit 8 (DATLX) while still reading bit 10 -> FAIL as expected. Reverted.
// MUTATION PROP-7 stimulus red: redirected the write from bit 9 (OUTLX) to
// bit 15 (OUTRY) while still reading bit 8 -> FAIL as expected. Reverted.
// MUTATION PROP-8 stimulus red: redirected the write from bit 8 (DATLX) to
// bit 14 (DATRY) while still reading bit 8 -> FAIL as expected. Reverted.
// MUTATION PROP-9 none impossible: bits 7-1 of POTGOR are the literal
// Verilog constant `8'h00` in the read mux's concatenation (userio.v:305),
// not a function of any signal -- no value written to POTGO, and no
// external input port, can move them. Unfalsifiable at this boundary.
// MUTATION PROP-10 stimulus red: wrote the same OUTLX=1/DATLX=0 pattern but
// with bit 0 (START) left 0 instead of set -> pot0x did not reset to 0
// (stayed at 3) -> FAIL as expected. Reverted.
// MUTATION PROP-11 stimulus red: drove mouse_btn[2] (middle button) instead
// of mouse_btn[1] (right button) while still reading bit 10 -> bit 10 no
// longer responded -> FAIL as expected. Reverted.
// MUTATION PROP-12 stimulus red: drove mouse_btn[1] (right button) instead
// of mouse_btn[2] (middle button) while still reading bit 8 -> bit 8 no
// longer responded -> FAIL as expected. Reverted.
// MUTATION PROP-13 none impossible: bits 15, 13, 11, 9 of POTGOR are the
// literal Verilog constant `1'b0` at four separate positions in the read
// mux's concatenation (userio.v:301,302,303,304), and bit 0 falls inside
// the same `8'h00` constant PROP-9 cites (userio.v:305) -- none of the five
// is a function of any signal. Unfalsifiable at this boundary.
//
// All twelve stimulus mutations and both `none impossible` records above
// were run/verified as described; see the per-run pass/fail counts in this
// package's build report.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vuserio.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_offset) { return static_cast<uint16_t>(byte_offset >> 1); }

constexpr uint16_t POTGO   = to_reg(0x034);
constexpr uint16_t POTGOR  = to_reg(0x016);  // POTINP in the RTL/figure; POTGOR per p.240
constexpr uint16_t POT0DAT = to_reg(0x012);
constexpr uint16_t IDLE    = 0x0FF;          // matches none of the above

constexpr uint16_t BIT(int n) { return static_cast<uint16_t>(1u << n); }
bool rd(uint16_t word, int n) { return ((word >> n) & 1u) != 0; }

// Builds a POTGO write word from the eight OUT/DAT bits plus START. Bits
// 7-1 are always left 0 here; PROP-9's own test drives them directly.
uint16_t potgo_word(bool out_ry, bool dat_ry, bool out_rx, bool dat_rx,
                     bool out_ly, bool dat_ly, bool out_lx, bool dat_lx,
                     bool start = false) {
    uint16_t w = 0;
    if (out_ry) w |= BIT(15);
    if (dat_ry) w |= BIT(14);
    if (out_rx) w |= BIT(13);
    if (dat_rx) w |= BIT(12);
    if (out_ly) w |= BIT(11);
    if (dat_ly) w |= BIT(10);
    if (out_lx) w |= BIT(9);
    if (dat_lx) w |= BIT(8);
    if (start)  w |= BIT(0);
    return w;
}

class UserioHarness {
public:
    Vuserio* dut;

    UserioHarness() : dut(new Vuserio) {
        dut->clk = 0;
        dut->reset = 1;
        dut->clk7_en = 1;
        dut->reg_address_in = IDLE;
        dut->data_in = 0;
        dut->pot_cnt_en = 0;
        dut->_fire0_dat = 1;
        dut->_fire1_dat = 1;
        dut->_joy1 = 0xFFFF;
        dut->_joy2 = 0xFFFF;
        dut->joy_ana1 = 0;
        dut->joy_ana2 = 0;
        dut->mouse_btn = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        for (int i = 0; i < 8; ++i) cycle();
        dut->reset = 0;
        // A few extra cycles beyond reset release: the joystick input path
        // runs through a 2-stage synchronizer (userio.v:214-219) that must
        // flush to the held _joy1=_joy2=0xFFFF ("everything released")
        // level before any test relies on it.
        for (int i = 0; i < 6; ++i) cycle();
    }
    ~UserioHarness() { dut->final(); delete dut; }
    UserioHarness(const UserioHarness&) = delete;
    UserioHarness& operator=(const UserioHarness&) = delete;

    void cycle() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
    void settle(int n) { for (int i = 0; i < n; ++i) cycle(); }

    // Writes `value` to POTGO for one addressed clk7_en cycle, then returns
    // the address to idle for one more cycle. Two cycles are required, not
    // one: potcap[] (POTGOR's readback source) is a *registered* function of
    // potreg, one clk7_en cycle behind the write that changes potreg, and
    // potreg[0] (START) needs the idle cycle to be sampled by the
    // counter-reset logic and to self-clear (userio.v:98-105, 125-133).
    void write_potgo(uint16_t value) {
        dut->reg_address_in = POTGO;
        dut->data_in = value;
        cycle();
        dut->reg_address_in = IDLE;
        dut->data_in = 0;
        cycle();
    }

    uint16_t read(uint16_t reg_offset) {
        dut->reg_address_in = reg_offset;
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = IDLE;
        dut->eval();
        return v;
    }
    uint16_t read_potgor() { return read(POTGOR); }
    uint16_t read_pot0dat() { return read(POT0DAT); }

    // _joy1[5] feeds the RY pair's external term (via the cross-wired
    // synchronizer, userio.v:210-219); _joy2[5] feeds the LY pair's.
    void set_joy1_bit5(bool level) {
        dut->_joy1 = level ? static_cast<uint16_t>(dut->_joy1 | 0x0020)
                            : static_cast<uint16_t>(dut->_joy1 & ~0x0020);
        settle(3);
    }
    void set_joy2_bit5(bool level) {
        dut->_joy2 = level ? static_cast<uint16_t>(dut->_joy2 | 0x0020)
                            : static_cast<uint16_t>(dut->_joy2 & ~0x0020);
        settle(3);
    }
    void set_mouse_btn(uint8_t mask) {
        dut->mouse_btn = mask;
        settle(1);
    }

    void pulse_pot_cnt_en(int n) {
        for (int i = 0; i < n; ++i) {
            dut->pot_cnt_en = 1;
            cycle();
            dut->pot_cnt_en = 0;
        }
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

// ---------------------------------------------------------------------
// FIXTURE-LIVE: userio.v has never been measured before. Two distinct,
// known POTGO words must both arrive and produce distinct, correctly
// differing POTGOR readings before anything data-dependent is graded.
// ---------------------------------------------------------------------
bool fixture_live(UserioHarness& h, bool repeat_first_word_bug = false) {
    h.write_potgo(0xAA00);  // all four pairs OUT=1,DAT=0 -> every data bit forced 0
    uint16_t r1 = h.read_potgor();
    h.write_potgo(repeat_first_word_bug ? 0xAA00 : 0xFF00);  // OUT=1,DAT=1 -> released, reads 1
    uint16_t r2 = h.read_potgor();
    bool word1_ok = r1 == 0x0000;
    bool word2_ok = r2 == 0x5500;
    bool distinct = r1 != r2;
    return word1_ok && word2_ok && distinct;
}

// ---------------------------------------------------------------------
// PROP-1 / PROP-2 -- OUTRY (bit 15) / DATRY (bit 14). External term:
// _joy1[5] (RY pair), held released unless the test varies it.
// ---------------------------------------------------------------------
bool prop1_outry(UserioHarness& h, int drive_out_bit = 15) {
    h.set_joy1_bit5(true);
    h.write_potgo(potgo_word(drive_out_bit == 15, false, drive_out_bit == 13, false,
                              false, false, false, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 14) && rd(r1, 12) && rd(r1, 10) && rd(r1, 8);
    h.write_potgo(potgo_word(false, false, false, false, false, false, false, false));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 14) && rd(r2, 12) && rd(r2, 10) && rd(r2, 8);
    return a && b;
}

bool prop2_datry(UserioHarness& h) {
    h.set_joy1_bit5(true);
    h.write_potgo(potgo_word(true, false, false, false, false, false, false, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 14);
    h.write_potgo(potgo_word(true, true, false, false, false, false, false, false));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 14);
    h.write_potgo(potgo_word(false, true, false, false, false, false, false, false));
    h.set_joy1_bit5(false);
    uint16_t r3 = h.read_potgor();
    bool c = !rd(r3, 14);
    h.set_joy1_bit5(true);
    uint16_t r4 = h.read_potgor();
    bool d = rd(r4, 14);
    bool nocross = rd(r1, 12) && rd(r1, 10) && rd(r1, 8)
                 && rd(r2, 12) && rd(r2, 10) && rd(r2, 8)
                 && rd(r3, 12) && rd(r3, 10) && rd(r3, 8)
                 && rd(r4, 12) && rd(r4, 10) && rd(r4, 8);
    return a && b && c && d && nocross;
}

// ---------------------------------------------------------------------
// PROP-3 / PROP-4 -- OUTRX (bit 13) / DATRX (bit 12). No external term
// reaches this pair at all (userio.v: potcap[2] <= joy2_pin5, a pure
// function of potreg[13:12]) -- confirmed independently here. Only the
// "OUT=1 forces" half of PROP-4 is assertable; the "OUT=0 senses external
// state" half is recorded, not asserted, for lack of any signal to sense.
// ---------------------------------------------------------------------
bool prop3_outrx(UserioHarness& h, int drive_out_bit = 13) {
    h.write_potgo(potgo_word(drive_out_bit == 15, false, drive_out_bit == 13, false,
                              false, false, false, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 12) && rd(r1, 14) && rd(r1, 10) && rd(r1, 8);
    h.write_potgo(potgo_word(false, false, false, false, false, false, false, false));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 12) && rd(r2, 14) && rd(r2, 10) && rd(r2, 8);
    return a && b;
}

bool prop4_datrx(UserioHarness& h) {
    h.write_potgo(potgo_word(false, false, true, false, false, false, false, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 12);
    h.write_potgo(potgo_word(false, false, true, true, false, false, false, false));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 12);
    bool nocross = rd(r1, 14) && rd(r1, 10) && rd(r1, 8)
                 && rd(r2, 14) && rd(r2, 10) && rd(r2, 8);
    return a && b && nocross;
}

// ---------------------------------------------------------------------
// PROP-5 / PROP-6 -- OUTLY (bit 11) / DATLY (bit 10). External terms:
// mouse_btn[1] (right button) AND _joy2[5]. Held released here; PROP-11
// tests the mouse-button identity specifically.
// ---------------------------------------------------------------------
bool prop5_outly(UserioHarness& h, int drive_out_bit = 11) {
    h.set_joy2_bit5(true);
    h.set_mouse_btn(0x00);
    h.write_potgo(potgo_word(drive_out_bit == 9, false, false, false,
                              drive_out_bit == 11, false, false, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 10) && rd(r1, 14) && rd(r1, 12) && rd(r1, 8);
    h.write_potgo(potgo_word(false, false, false, false, false, false, false, false));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 10) && rd(r2, 14) && rd(r2, 12) && rd(r2, 8);
    return a && b;
}

bool prop6_datly(UserioHarness& h) {
    h.set_joy2_bit5(true);
    h.set_mouse_btn(0x00);
    h.write_potgo(potgo_word(false, false, false, false, true, false, false, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 10);
    h.write_potgo(potgo_word(false, false, false, false, true, true, false, false));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 10);
    h.write_potgo(potgo_word(false, false, false, false, false, true, false, false));
    h.set_joy2_bit5(false);
    uint16_t r3 = h.read_potgor();
    bool c = !rd(r3, 10);
    h.set_joy2_bit5(true);
    uint16_t r4 = h.read_potgor();
    bool d = rd(r4, 10);
    bool nocross = rd(r1, 14) && rd(r1, 12) && rd(r1, 8)
                 && rd(r2, 14) && rd(r2, 12) && rd(r2, 8)
                 && rd(r3, 14) && rd(r3, 12) && rd(r3, 8)
                 && rd(r4, 14) && rd(r4, 12) && rd(r4, 8);
    return a && b && c && d && nocross;
}

// ---------------------------------------------------------------------
// PROP-7 / PROP-8 -- OUTLX (bit 9) / DATLX (bit 8). External term:
// mouse_btn[2] (middle button) only. This pair also serves as this
// package's control case: same write/read mechanism as every other pair,
// deterministic, no ambiguity about button polarity.
// ---------------------------------------------------------------------
bool prop7_outlx(UserioHarness& h, int drive_out_bit = 9) {
    h.set_mouse_btn(0x00);
    h.write_potgo(potgo_word(drive_out_bit == 15, false, false, false,
                              false, false, drive_out_bit == 9, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 8) && rd(r1, 14) && rd(r1, 12) && rd(r1, 10);
    h.write_potgo(potgo_word(false, false, false, false, false, false, false, false));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 8) && rd(r2, 14) && rd(r2, 12) && rd(r2, 10);
    return a && b;
}

bool prop8_datlx(UserioHarness& h) {
    h.set_mouse_btn(0x00);
    h.write_potgo(potgo_word(false, false, false, false, false, false, true, false));
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 8);
    h.write_potgo(potgo_word(false, false, false, false, false, false, true, true));
    uint16_t r2 = h.read_potgor();
    bool b = rd(r2, 8);
    h.write_potgo(potgo_word(false, false, false, false, false, false, false, true));
    h.set_mouse_btn(0x04);
    uint16_t r3 = h.read_potgor();
    bool c = !rd(r3, 8);
    h.set_mouse_btn(0x00);
    uint16_t r4 = h.read_potgor();
    bool d = rd(r4, 8);
    bool nocross = rd(r1, 14) && rd(r1, 12) && rd(r1, 10)
                 && rd(r2, 14) && rd(r2, 12) && rd(r2, 10)
                 && rd(r3, 14) && rd(r3, 12) && rd(r3, 10)
                 && rd(r4, 14) && rd(r4, 12) && rd(r4, 10);
    return a && b && c && d && nocross;
}

// ---------------------------------------------------------------------
// PROP-9 -- bits 7 down to 1: constant 0 on POTGOR, regardless of what is
// written to POTGO. Written in p.282's form; see SPECIFICATION.md for why.
// ---------------------------------------------------------------------
bool prop9_bits7_1_zero(UserioHarness& h) {
    h.write_potgo(0x0000);
    uint16_t r1 = h.read_potgor();
    h.write_potgo(0xFFFF);
    uint16_t r2 = h.read_potgor();
    h.write_potgo(0x00FE);  // bits 7-1 set, nothing else
    uint16_t r3 = h.read_potgor();
    bool z1 = ((r1 >> 1) & 0x7F) == 0;
    bool z2 = ((r2 >> 1) & 0x7F) == 0;
    bool z3 = ((r3 >> 1) & 0x7F) == 0;
    return z1 && z2 && z3;
}

// ---------------------------------------------------------------------
// PROP-10 -- bit 0, START. Zeroes pot0x (and the other three counters) and
// re-enables counting from that fresh 0.
// ---------------------------------------------------------------------
bool prop10_start(UserioHarness& h, bool omit_start_bug = false) {
    h.write_potgo(potgo_word(false, false, false, false, false, false, true, false));
    h.pulse_pot_cnt_en(3);
    uint16_t before = h.read_pot0dat();
    bool a = (before & 0x00FF) == 3;

    h.write_potgo(potgo_word(false, false, false, false, false, false, true, false,
                              /*start=*/ !omit_start_bug));
    uint16_t after_start = h.read_pot0dat();
    bool b = (after_start & 0x00FF) == 0;

    h.pulse_pot_cnt_en(4);
    uint16_t after_count = h.read_pot0dat();
    bool c = omit_start_bug ? true : (after_count & 0x00FF) == 4;

    return a && b && c;
}

// ---------------------------------------------------------------------
// PROP-11 / PROP-12 -- bit 10 / bit 8, as inputs (OUTLY=OUTLX=0), reflect
// the right / middle mouse button.
// ---------------------------------------------------------------------
bool prop11_right_button(UserioHarness& h, int drive_btn_index = 1) {
    h.write_potgo(potgo_word(false, false, false, false, false, false, false, false));
    h.set_joy2_bit5(true);
    h.set_mouse_btn(0x00);
    uint16_t r_rel = h.read_potgor();
    bool a = rd(r_rel, 10);
    h.set_mouse_btn(static_cast<uint8_t>(1u << drive_btn_index));
    uint16_t r_pressed = h.read_potgor();
    bool b = !rd(r_pressed, 10);
    h.set_mouse_btn(0x00);
    uint16_t r_rel2 = h.read_potgor();
    bool c = rd(r_rel2, 10);
    bool nocross = rd(r_rel, 14) && rd(r_rel, 12) && rd(r_rel, 8)
                 && rd(r_pressed, 14) && rd(r_pressed, 12) && rd(r_pressed, 8);
    return a && b && c && nocross;
}

bool prop12_middle_button(UserioHarness& h, int drive_btn_index = 2) {
    h.write_potgo(potgo_word(false, false, false, false, false, false, false, false));
    h.set_mouse_btn(0x00);
    uint16_t r_rel = h.read_potgor();
    bool a = rd(r_rel, 8);
    h.set_mouse_btn(static_cast<uint8_t>(1u << drive_btn_index));
    uint16_t r_pressed = h.read_potgor();
    bool b = !rd(r_pressed, 8);
    h.set_mouse_btn(0x00);
    uint16_t r_rel2 = h.read_potgor();
    bool c = rd(r_rel2, 8);
    bool nocross = rd(r_rel, 14) && rd(r_rel, 12) && rd(r_rel, 10)
                 && rd(r_pressed, 14) && rd(r_pressed, 12) && rd(r_pressed, 10);
    return a && b && c && nocross;
}

// ---------------------------------------------------------------------
// PROP-13 -- bits 15, 13, 11, 9 and 0 of POTGOR: constant 0, regardless of
// what is written to POTGO. Distinct from PROP-9 (bits 7-1, which carries
// the p.240/p.282 naming conflict).
// ---------------------------------------------------------------------
bool prop13_write_only_bits_read_zero(UserioHarness& h) {
    h.write_potgo(0xFFFF);
    uint16_t r1 = h.read_potgor();
    bool a = !rd(r1, 15) && !rd(r1, 13) && !rd(r1, 11) && !rd(r1, 9) && !rd(r1, 0);
    h.write_potgo(0x0000);
    uint16_t r2 = h.read_potgor();
    bool b = !rd(r2, 15) && !rd(r2, 13) && !rd(r2, 11) && !rd(r2, 9) && !rd(r2, 0);
    return a && b;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // FIXTURE-LIVE must run first: userio.v has never been measured before,
    // and every assertion below depends on its write/read path actually
    // working.
    {
        UserioHarness h;
        checks.expect(fixture_live(h),
                      "FIXTURE-LIVE: two distinct, known POTGO words (0xAA00, "
                      "0xFF00) both arrive and produce distinct, correctly "
                      "differing POTGOR readings (0x0000, 0x5500) before any "
                      "data-dependent assertion is graded");
    }

    { UserioHarness h; checks.expect(prop1_outry(h),
        "PROP-1: POTGO bit 15 (OUTRY) gates whether bit 14's write value "
        "forces POTGOR bit 14 -- with DATRY held at the SAME value (0) in "
        "both readings, only OUTRY changes and the reading changes with it "
        "(forced 0 when OUTRY=1, releases to 1 when OUTRY=0); the other "
        "three pairs (bits 12,10,8) do not move [p.240 Table 8-4 row "
        "\"15 OUTRY Output enable for bit 14\"; p.282 \"Output enable for "
        "Paula pin 36\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop2_datry(h),
        "PROP-2: with OUTRY=1, POTGO bit 14 (DATRY) forces POTGOR bit 14 to "
        "its own written value (0 or 1, tested against a released external "
        "line so DATRY's own effect is isolated); with OUTRY=0, DATRY's "
        "value is irrelevant and bit 14 instead tracks the external line "
        "(shown by toggling that line while DATRY is held fixed at the "
        "\"wrong\" value); the other three pairs do not move "
        "[p.240 \"14 DATRY data for port 2, pin 9\"; p.282 \"I/O data Paula "
        "pin 36\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop3_outrx(h),
        "PROP-3: POTGO bit 13 (OUTRX) gates whether bit 12's write value "
        "forces POTGOR bit 12, DATRX held at the same value (0) in both "
        "readings; the other three pairs do not move "
        "[p.240 \"13 OUTRX Output enable for bit 12\"; p.282 \"Output "
        "enable for Paula pin 35\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop4_datrx(h),
        "PROP-4: with OUTRX=1, POTGO bit 12 (DATRX) forces POTGOR bit 12 to "
        "its own written value (0 or 1); the other three pairs do not move "
        "-- output-forcing half only, per this fixture's own RTL (see "
        "header note and the RECORDED line below for the sensing half) "
        "[p.240 \"12 DATRX data for port 2, pin 5\"; p.282 \"I/O data Paula "
        "pin 35\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop5_outly(h),
        "PROP-5: POTGO bit 11 (OUTLY) gates whether bit 10's write value "
        "forces POTGOR bit 10, DATLY held at the same value (0) in both "
        "readings; the other three pairs do not move "
        "[p.240 \"11 OUTLY Output enable for bit 10\"; p.282 \"Output "
        "enable for Paula pin 33\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop6_datly(h),
        "PROP-6: with OUTLY=1, POTGO bit 10 (DATLY) forces POTGOR bit 10 to "
        "its own written value (against a released external line); with "
        "OUTLY=0, bit 10 instead tracks the external line regardless of "
        "DATLY; the other three pairs do not move -- mouse-button identity "
        "specifically is PROP-11, not this assertion "
        "[p.240 \"10 DATLY data for port 1, pin 9\"; p.282 \"I/O data Paula "
        "pin 33\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop7_outlx(h),
        "PROP-7 [control]: POTGO bit 9 (OUTLX) gates whether bit 8's write "
        "value forces POTGOR bit 8, DATLX held at the same value (0) in "
        "both readings; the other three pairs do not move "
        "[p.240 \"09 OUTLX Output enable for bit 8\"; p.282 \"Output "
        "enable for Paula pin 32\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop8_datlx(h),
        "PROP-8 [control]: with OUTLX=1, POTGO bit 8 (DATLX) forces POTGOR "
        "bit 8 to its own written value; with OUTLX=0, bit 8 instead tracks "
        "the middle-mouse-button line regardless of DATLX; the other three "
        "pairs do not move -- this pair's deterministic, unambiguous "
        "behaviour is this package's control case, on the same write/read "
        "mechanism as every other PROP [p.240 \"08 DATLX data for port 1, "
        "pin 5\"; p.282 \"I/O data Paula pin 32\"; Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop9_bits7_1_zero(h),
        "PROP-9: POTGOR bits 7 down to 1 read 0 regardless of what is "
        "written to POTGO (0x0000, 0xFFFF, and 0x00FE all tried) -- written "
        "in p.282's form (\"Reserved for chip ID code (presently 0)\") "
        "because a constant 0 cannot distinguish that from p.240's \"chip "
        "revision identification number\"; see SPECIFICATION.md PROP-9 for "
        "why [p.240 \"07-01 X chip revision identification number\"; p.282 "
        "\"07-01 0 Reserved for chip ID code (presently 0)\"]"); }

    { UserioHarness h; checks.expect(prop10_start(h),
        "PROP-10: writing POTGO bit 0 (START) zeroes pot0x (read via "
        "POT0DAT) and counting resumes from that fresh 0 on subsequent "
        "pot_cnt_en pulses -- POTGO bit 9/DATLX bit 8 held at the pattern "
        "that keeps pot0x free to count throughout, so START's own effect "
        "(reset, then re-enable) is isolated [p.240 \"00 START Start pots "
        "(dump capacitors, start counters)\"; p.282, identical wording; "
        "Figure 8-6 p.237]"); }

    { UserioHarness h; checks.expect(prop11_right_button(h),
        "PROP-11: with OUTLY=0 (bit 10/DATLY left as an input) and the "
        "_joy2[5] precondition held released, POTGOR bit 10 follows the "
        "right mouse button (mouse_btn[1]): released reads 1, pressed reads "
        "0, released again reads 1; the other three pairs do not move "
        "[p.240 \"10 DATLY data for port 1, pin 9 (right mouse button)\"]"); }

    { UserioHarness h; checks.expect(prop12_middle_button(h),
        "PROP-12: with OUTLX=0 (bit 8/DATLX left as an input), POTGOR bit 8 "
        "follows the middle mouse button (mouse_btn[2]): released reads 1, "
        "pressed reads 0, released again reads 1; the other three pairs do "
        "not move [p.240 \"08 DATLX data for port 1, pin 5 (middle mouse "
        "button)\"]"); }

    { UserioHarness h; checks.expect(prop13_write_only_bits_read_zero(h),
        "PROP-13: POTGOR bits 15, 13, 11, 9 and 0 (the four output-enable "
        "positions and START) read 0 regardless of what is written to "
        "POTGO (0xFFFF and 0x0000 both tried) -- only bits 14, 12, 10 and 8 "
        "vary; distinct from PROP-9 (bits 7-1) [Figure 8-6 p.237: POTINP's "
        "own row of cells shows \"0\" under every POTGO column except the "
        "four DAT columns]"); }

    // ------------------------------------------------------------------
    // Claim considered for this package and left unbound, with the reason.
    // Does not touch passed/failed -- this is not an assertion.
    // ------------------------------------------------------------------
    checks.record("PROP-4's \"OUTRX=0, DATRX senses the external pin\" half "
        "(the mirror of PROP-2's third/fourth sub-check and PROP-6's) is not "
        "assertable: userio.v's potcap[2] <= joy2_pin5 = ~(potreg[13] & "
        "~potreg[12]) is a pure function of potreg alone (userio.v:153,169) "
        "-- no _joy2/_joy1/mouse_btn term reaches it, so with OUTRX=0 this "
        "bit reads a constant 1 regardless of any input this fixture can "
        "drive. This is the same asymmetry this package's own claims file "
        "had already found (8-dio.c13/c11, 8-appa-potgo.c15/sol.c43 `split` "
        "fields: \"no input reaches bit 12\"), confirmed independently here "
        "by reading the RTL directly in step 2, not assumed from that file.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
