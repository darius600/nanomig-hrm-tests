// appDE-pot-counters
//
// Grades 42 claims (readers opus, sol) against userio.v, compiled alone, on
// Appendix E p.331-332: "GAME PORT INTERFACE TO PROPORTIONAL CONTROLLERS"
// and the CONNECTOR PIN USAGE / POT COUNTERS figures. See SPECIFICATION.md
// for the full claim-by-claim grouping and the reasoning behind each
// decline.
//
// RTL findings used throughout below:
// - userio.v hardwires joy_swap=1'b0 (userio.v:92), which makes the
//   *internal* "_djoy1"/"_sjoy1" (Port 1: dmouse0dat/JOY0DAT/POT0DAT/
//   potcap[0]/potcap[1]) track the EXTERNAL _joy2 DUT port, and
//   "_djoy2"/"_sjoy2" (Port 2) track the EXTERNAL _joy1 DUT port
//   (userio.v:210-219). Same finding ch8-pot-counter-start-and-charge's
//   file header records; re-confirmed here independently.
// - Per-pin to connector-vector-bit correspondence (both connectors use the
//   same layout): pin1=bit3, pin2=bit2, pin3=bit1, pin4=bit0, pin6=bit4
//   (fire), pin9=bit5. Matches hrm/ch8-controller-pin-function-map's own
//   measured mapping.
// - potcap's sequential block (userio.v:160-178) reads only _djoy1[5],
//   _djoy2[5], _mright, _mthird and potreg (POTGO) bits -- never bits
//   0-4 of either connector vector. Measured in group_A2 below; this is
//   the RTL fact behind both the two NEW "Unused" assertions (pins 2, 6)
//   and the pins-3/4 question the build brief raises separately (see
//   SPECIFICATION.md).
//
// MUTATION sol:E-pot.c16 stimulus red MUT_PIN1: pin 2 (bit 2, the horizontal
//   quadrature pair) is toggled instead of pin 1 (bit 3, vertical pair); the
//   high byte never moves, so the +1-then-(-1) comparison fails.
// MUTATION opus:E-pot.c19 stimulus red (same MUT_PIN1 as sol:E-pot.c16)
// MUTATION sol:E-pot.c17 stimulus red MUT_PIN2: pin 9 (bit 5, known to gate
//   potcap) is toggled instead of pin 2; POTINP moves, so the
//   "stayed at baseline" comparison fails.
// MUTATION opus:E-pot.c20 stimulus red (same MUT_PIN2 as sol:E-pot.c17)
// MUTATION sol:E-pot.c21 stimulus red MUT_PIN6: pin 9 (bit 5) is toggled
//   instead of pin 6; POTINP moves, so the "stayed at baseline" comparison
//   fails.
// MUTATION opus:E-pot.c24 stimulus red (same MUT_PIN6 as sol:E-pot.c21)

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
static const int A_JOY0DAT = 0x00a >> 1;
static const int A_JOY1DAT = 0x00c >> 1;
static const int A_POT0DAT = 0x012 >> 1;
static const int A_POT1DAT = 0x014 >> 1;
static const int A_POTINP  = 0x016 >> 1;
static const int A_POTGO   = 0x034 >> 1;
static const int A_DFF104  = 0x104 >> 1; // p.333's printed (wrong) POTINP address
static const int A_IDLE    = 0x000;

// connector-vector bit positions, per the pin correspondence in the file
// header note above
static const int BIT_PIN4 = 0;
static const int BIT_PIN3 = 1;
static const int BIT_PIN2 = 2;
static const int BIT_PIN1 = 3;
static const int BIT_PIN6 = 4;
static const int BIT_PIN9 = 5;

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
static uint16_t joy0dat() { return read_reg(A_JOY0DAT); }

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
static void set_joy1_bit(int bit, int asserted) {
    if (asserted) dut->_joy1 &= (uint16_t)~(1 << bit); else dut->_joy1 |= (uint16_t)(1 << bit);
}
static void set_joy2_bit(int bit, int asserted) {
    if (asserted) dut->_joy2 &= (uint16_t)~(1 << bit); else dut->_joy2 |= (uint16_t)(1 << bit);
}

//--------------------------------------------------------------------------
// A1 -- p.332 figure: connector pin 1 is XBUT ("Extra Button"), no register
// note printed. NEW: ch8-controller-pin-function-map's own SPECIFICATION.md
// records Table 8-1's matching cell ("Button 3**", XY Prop. Joystick column,
// pin 1) as "neither asserted nor declined" -- never handed to any package
// before this lot. What is gradable: pin 1 is a readable digital input, not
// which device name is wired to it (this fixture has no notion of what is
// plugged into the connector -- the same split
// ch8-controller-pin-function-map Group C draws for other pins).
//--------------------------------------------------------------------------
static void group_A1_pin1() {
    full_reset();
    // Port 1's vertical quadrature pair is bits 1 (pin 3) and 3 (pin 1) --
    // driven via the EXTERNAL _joy2 DUT port, per the swap in the file
    // header note. Pin 3 is held pressed throughout so pin 1's own edges
    // are isolated.
    set_joy2_bit(BIT_PIN3, 1);
    set_joy2_bit(BIT_PIN1, 1);
    settle(4);
    uint16_t before = joy0dat();

#ifdef MUT_PIN1
    set_joy2_bit(BIT_PIN2, 0); // MUTATED: pin 2 (horizontal pair) instead of pin 1
#else
    set_joy2_bit(BIT_PIN1, 0); // release pin 1 only; pin 3 stays pressed
#endif
    settle(4);
    uint16_t after_release = joy0dat();
    int hb_before = (before >> 8) & 0xFF, hb_release = (after_release >> 8) & 0xFF;
    bool rose_by_one = (hb_release - hb_before == 1) && ((after_release & 0xFF) == (before & 0xFF));

#ifdef MUT_PIN1
    set_joy2_bit(BIT_PIN2, 1); // MUTATED counterpart
#else
    set_joy2_bit(BIT_PIN1, 1); // press pin 1 again; pin 3 still pressed
#endif
    settle(4);
    uint16_t after_press = joy0dat();
    int hb_press = (after_press >> 8) & 0xFF;
    bool fell_by_one = (hb_press - hb_release == -1) && ((after_press & 0xFF) == (before & 0xFF));

    bool ok = rose_by_one && fell_by_one;
    note("pin 1 isolated (pin 3 held pressed throughout, low byte untouched): JOY0DAT before=$" +
         hex4(before) + " after release=$" + hex4(after_release) + " after re-press=$" +
         hex4(after_press) + " -- a level presented on pin 1 alone moves the high byte by "
         "exactly one count in each direction. Which external device's contact this is (an "
         "'extra button', per the figure's XBUT mnemonic) is not graded -- not observable at "
         "this boundary.");
    expect(ok, "sol:E-pot.c16");
    expect(ok, "opus:E-pot.c19");
}

//--------------------------------------------------------------------------
// A2 -- p.332 figure: connector pins 2 and 6 are "Unused" (no description,
// no register note). NEW, negative: no accepted package grades that these
// two pins carry nothing in the proportional-input/POT-counter path.
// Measured: with every potcap-relevant input held fixed, pressing and
// releasing pin 2 or pin 6 (bits 2 and 4, on either connector) never moves
// POTINP.
//
// Also measured here, NOT graded (no classified claim names it -- see
// SPECIFICATION.md "the pins-3/4 question"): pins 1, 3 and 4 (bits 3, 1, 0)
// are structurally identical -- potcap never reads them either.
//--------------------------------------------------------------------------
static void group_A2_pin2_pin6_exclusion() {
    full_reset();
    arm_potgo(0x0001);
    // Baseline: every potcap-relevant input at a known, fixed, non-trivial
    // state (mouse_btn=0 -> _mright=_mthird=1; both connectors idle on pin
    // 9/bit 5; potreg all zero after the POTGO arm self-clears) -- every
    // potcap term evaluates to 1, so POTINP reads $5500 (see file header
    // note; not $0000, so a real change would be visible on any bit).
    set_mouse(0, 0, 0);
    settle(3);
    uint16_t baseline = read_reg(A_POTINP);

    bool pin2_excluded = true, pin6_excluded = true;
    for (int which = 1; which <= 2; which++) {
        int bit2 =
#ifdef MUT_PIN2
            BIT_PIN9; // MUTATED: known-live pin 9 instead of pin 2
#else
            BIT_PIN2;
#endif
        if (which == 1) { set_joy1_bit(bit2, 1); settle(3); if (read_reg(A_POTINP) != baseline) pin2_excluded = false;
                           set_joy1_bit(bit2, 0); settle(3); if (read_reg(A_POTINP) != baseline) pin2_excluded = false; }
        else             { set_joy2_bit(bit2, 1); settle(3); if (read_reg(A_POTINP) != baseline) pin2_excluded = false;
                           set_joy2_bit(bit2, 0); settle(3); if (read_reg(A_POTINP) != baseline) pin2_excluded = false; }

        int bit6 =
#ifdef MUT_PIN6
            BIT_PIN9; // MUTATED: known-live pin 9 instead of pin 6
#else
            BIT_PIN6;
#endif
        if (which == 1) { set_joy1_bit(bit6, 1); settle(3); if (read_reg(A_POTINP) != baseline) pin6_excluded = false;
                           set_joy1_bit(bit6, 0); settle(3); if (read_reg(A_POTINP) != baseline) pin6_excluded = false; }
        else             { set_joy2_bit(bit6, 1); settle(3); if (read_reg(A_POTINP) != baseline) pin6_excluded = false;
                           set_joy2_bit(bit6, 0); settle(3); if (read_reg(A_POTINP) != baseline) pin6_excluded = false; }
    }

    note("POTINP baseline (every potcap term driven to its held value) = $" + hex4(baseline) +
         "; pressing and releasing pin 2 and pin 6, on both connectors in turn, " +
         std::string((pin2_excluded && pin6_excluded) ? "never moved it."
                                                        : "moved it -- see FAIL lines below."));

    bool pins134_excluded = true;
    int extra_bits[3] = { BIT_PIN1, BIT_PIN3, BIT_PIN4 };
    for (int i = 0; i < 3; i++) {
        set_joy1_bit(extra_bits[i], 1); settle(3); if (read_reg(A_POTINP) != baseline) pins134_excluded = false;
        set_joy1_bit(extra_bits[i], 0); settle(3); if (read_reg(A_POTINP) != baseline) pins134_excluded = false;
        set_joy2_bit(extra_bits[i], 1); settle(3); if (read_reg(A_POTINP) != baseline) pins134_excluded = false;
        set_joy2_bit(extra_bits[i], 0); settle(3); if (read_reg(A_POTINP) != baseline) pins134_excluded = false;
    }
    note("MEASURED for the pins-3/4 question the build brief raises separately (no classified "
         "claim names it; not graded): pins 1, 3 and 4 (bits 3, 1, 0 of either connector) were "
         "pressed and released the same way and " +
         std::string(pins134_excluded ? "also never moved POTINP."
                                       : "DID move POTINP -- unexpected, contradicts the file "
                                         "header note.") +
         " potcap's sequential block (userio.v:160-178) reads only _djoy1[5], _djoy2[5], "
         "_mright, _mthird and potreg bits -- never bits 0-4 of either connector vector, so "
         "pins 1, 2, 3, 4 and 6 are ALL structurally excluded from potcap/POTINP alike, not "
         "only the bits-[0]/[1] pair the brief names. p.332's own NOTES column for pins 3/4 "
         "is consistent with this: it prints 'See Digital Joystick' and names no POTGO/POTINP "
         "register at all, unlike pins 5 and 9, which list POTGO and POTINP explicitly.");

    expect(pin2_excluded, "sol:E-pot.c17");
    expect(pin2_excluded, "opus:E-pot.c20");
    expect(pin6_excluded, "sol:E-pot.c21");
    expect(pin6_excluded, "opus:E-pot.c24");
}

//--------------------------------------------------------------------------
// C -- p.332 vs p.333: POTINP's address. p.332 prints DFF016; p.333 prints
// DFF104 (the same address it gives BPLCON0 "write only", two boxes
// above). Per the build brief and the NM-HRM-062 precedent
// (hrm/appF-cia-address-decode, Group H): measure both readings, record
// both, grade neither. No expect() for these two uids.
//--------------------------------------------------------------------------
static void group_C_potinp_address_conflict() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(1, 0, 0); // a non-baseline state, so a wrong-address read would visibly differ
    settle(3);
    uint16_t potinp_at_016 = read_reg(A_POTINP);
    note("p.332/p.333 conflict on POTINP's address -- reading userio at $DFF016 (p.332's "
         "printed address) returns $" + hex4(potinp_at_016) + ": the module's own POTINP case "
         "(userio.v:59, parameter POTINP=9'h016; userio.v:300-305) answers there.");

    uint16_t at_104 = read_reg(A_DFF104);
    note("Reading userio at $DFF104 (p.333's printed address for the SAME register, 'POTINP "
         "read only') returns $" + hex4(at_104) + ": userio.v's read mux (userio.v:289-309) "
         "carries no case for this offset at all and falls through to the default 16'h0000. "
         "$DFF104 is not POTINP under either reading at this module: appA-playfield/p271.txt:12 "
         "and appB3/p291.txt:29 both print BPLCON2 at 104 (BPLCON0 is 100), and denise.v:59-60 "
         "agrees (BPLCON0=9'h100, BPLCON2=9'h104) -- a different module, outside this package's "
         "boundary.");

    record(
        "sol:E-pot.c32, opus:E-pot.c36 (classify.source_defines: conflict): p.332's own POT "
        "COUNTERS figure prints 'POTINP  DFF016'; p.333's LIGHT PEN figure prints 'POTINP read "
        "only  DFF104', the same address p.333 gives 'BPLCON0 write only' two boxes above. Both "
        "readings measured above via note(). Per the build brief and the NM-HRM-062 precedent: "
        "measure both, record both, grade neither. hrm/ch8-register-map-userio "
        "PROP-ADDR-POTGOR already grades $DFF016 for this register (p.240's POTGOR, 'formerly "
        "called POTINP') in an accepted chapter -- the DFF016 side would be a re-grade even if "
        "graded, and the DFF104 side would contradict it and red a correct decoder. The DFF104 "
        "side of this same conflict belongs to appDE-lightpen-userio (sol:E-lightpen.c19, "
        "opus:E-lightpen.c22), not to this package.");
}

//--------------------------------------------------------------------------
// G1 -- p.331: "The JOY[0/1]DAT registers contain digital translation "
// values for y in the high byte and x in the low byte." Already graded.
//--------------------------------------------------------------------------
static void group_G1_joydat_byte_layout() {
    record(
        "sol:E-pot.c2, opus:E-pot.c3 (p.331, JOY[0/1]DAT byte layout -- Y high byte, X low "
        "byte): hrm/ch8-joydat-counter-layout's 'Byte assignment within the register' "
        "proposition (p.230) already grades this exact fact in an accepted chapter -- driving "
        "only the vertical-count input changes bits 15-8 and leaves bits 7-0 alone, and the "
        "converse. Identical proposition, different page. Declined as a re-grade.");
}

//--------------------------------------------------------------------------
// G2 -- p.331 item 1: reset interval (7 NTSC / 8 PAL lines), capacitor
// discharge. Already graded; the NTSC/PAL split cannot be independently
// tested at this boundary.
//--------------------------------------------------------------------------
static void group_G2_reset_interval() {
    record(
        "sol:E-pot.c5, opus:E-pot.c6 (p.331, reset interval and capacitor discharge): "
        "hrm/ch8-pot-counter-start-and-charge G6 (p.235, 'seven or eight scan lines') already "
        "grades this proposition in an accepted chapter, RED -- that package measured that this "
        "RTL holds no separate multi-line reset state at all (userio.v:130-134: the very next "
        "pot_cnt_en pulse after the POTGO arm already increments the counters). p.331 is the "
        "only page in this lot that ties the 7/8 split explicitly to NTSC vs PAL, but `ntsc` is "
        "not a port of userio.v (userio.v:28-49) -- the line rate reaches this module only as "
        "pot_cnt_en, so a fixture at this boundary cannot grade the NTSC/PAL distinction "
        "separately from the count G6 already grades. Declined as a re-grade. The capacitor-"
        "discharge half of the same sentence names a component this digital design does not "
        "have (joy_ana_en is tied 1'b0, userio.v:93) and is not constructible regardless.");
}

//--------------------------------------------------------------------------
// G3 -- p.331 item 2: continuous compare against an internal reference.
// Already graded.
//--------------------------------------------------------------------------
static void group_G3_conversion_compare() {
    record(
        "sol:E-pot.c6, opus:E-pot.c8 (p.331, continuous compare against internal reference): "
        "hrm/ch8-pot-counter-start-and-charge G7 (p.235, 'compares once per scan line') already "
        "grades this in an accepted chapter. Declined as a re-grade. The 'continuously compared "
        "to an internal reference level' wording is an analog description of the same digital "
        "per-line gate check G7 grades; joy_ana_en is tied 1'b0 (userio.v:93), so no analog "
        "comparison branch is reachable in this design regardless of wording.");
}

//--------------------------------------------------------------------------
// G4 -- p.331 item 3: latch-and-hold on threshold crossing. Already graded.
//--------------------------------------------------------------------------
static void group_G4_conversion_latch() {
    record(
        "sol:E-pot.c7, opus:E-pot.c9 (p.331, latch current counter value on threshold "
        "crossing): hrm/ch8-pot-counter-start-and-charge G9 (p.235, 'held until the next POTGO "
        "is issued') already grades this in an accepted chapter. Declined as a re-grade.");
}

//--------------------------------------------------------------------------
// G5 -- p.331 NOTE: POTY/POTX designated "right mouse button"/"unused mouse
// button". Already graded.
//--------------------------------------------------------------------------
static void group_G5_button_identity() {
    record(
        "sol:E-pot.c9, opus:E-pot.c11 (p.331 NOTE, POTY='right mouse button', POTX='unused "
        "mouse button'): hrm/ch8-potgo-register-bit-layout PROP-11/PROP-12 (p.240's parenthetical "
        "'right mouse button'/'pin 5 (middle mouse button)') and hrm/ch8-mouse-button-pins P2/P3 "
        "(p.232) already grade which mouse button reaches which pot-gate pin, in accepted "
        "chapters. Declined as a re-grade -- the wording differs (this page's 'unused mouse "
        "button' vs. p.240's 'middle button') but the proposition, which button reaches which "
        "pot pin, is the same one.");
}

//--------------------------------------------------------------------------
// G6 -- p.331: "The buttons are also available in POTGO and POTINP
// registers." POTINP half already graded; POTGO half is write-only here --
// measured below -- so grading a POTGO readback would red a correct
// write-only register.
//--------------------------------------------------------------------------
static void group_G6_potgo_potinp_readback() {
    full_reset();
    arm_potgo(0x0001);
    // Write a known, non-zero, distinctive pattern to POTGO, then read it
    // back.
    dut->reg_address_in = A_POTGO;
    dut->data_in = 0xBEEF;
    tick();
    idle_bus();
    uint16_t potgo_readback = read_reg(A_POTGO);
    note("POTGO write-only check: wrote $BEEF to $DFF034, read back $" + hex4(potgo_readback) +
         " -- userio.v's read mux (userio.v:289-309) carries no case for POTGO's address at "
         "all, so any read of $034 returns the default 16'h0000 regardless of what was written.");

    record(
        "sol:E-pot.c11, opus:E-pot.c13 (p.331, 'the buttons are also available in POTGO and "
        "POTINP registers'): the POTINP half is already graded (hrm/ch8-potgo-register-bit-"
        "layout PROP-6/PROP-8/PROP-11/PROP-12; hrm/ch8-pot-pins-as-digital-io Group 6) in "
        "accepted chapters. Declined as a re-grade for that half. The POTGO half is measured "
        "above to be write-only in this design; grading 'available in POTGO' as a readback claim "
        "would red a correct write-only register, so it is not graded either. p.240's own note "
        "that POTGOR was 'formerly called POTINP' says the readable register is the $016 one, "
        "consistent with this measurement.");
}

//--------------------------------------------------------------------------
// G7 -- p.332 figure: connector pins 3/4, LBUT*/RBUT* ('Left button'/'Right
// button', NOTES column: 'See Digital Joystick'). Already graded under the
// joystick device name; the page's own cross-reference says it is the same
// proposition, not a narrower or differently-axised one.
//--------------------------------------------------------------------------
static void group_G7_pin34_device_identity() {
    record(
        "sol:E-pot.c18, sol:E-pot.c19, opus:E-pot.c21, opus:E-pot.c22 (p.332 figure, pins 3/4: "
        "LBUT*='Left button', RBUT*='Right button', NOTES column 'See Digital Joystick'): "
        "hrm/ch8-controller-pin-function-map Group A already grades pin 3 = joystick left / pin "
        "4 = joystick right (the electrical fact, at JOY0DAT/JOY1DAT) in an accepted chapter. "
        "p.332's own NOTES column for both cells names no register of its own and instead "
        "cross-references 'Digital Joystick' -- the page itself says this is the same fact under "
        "a different device name, unlike pins 5/9's device-identity cells (declined elsewhere, "
        "e.g. ch8-controller-pin-function-map Group C, for lack of observability at this "
        "boundary, a DIFFERENT reason). Declined as a re-grade; the underlying line-exists fact "
        "is Group A's, not re-measured here.");
}

//--------------------------------------------------------------------------
// G8 -- p.332 figure: pin 5, POTX. Already graded, RED.
//--------------------------------------------------------------------------
static void group_G8_pin5_potx() {
    record(
        "sol:E-pot.c20, opus:E-pot.c23 (p.332 figure, pin 5: POTX, 'X analog in', "
        "POT[0/1]DAT<7:0>, POTGO, POTINP): hrm/ch8-controller-pin-function-map Group B2 and "
        "hrm/ch8-pot-pins-as-digital-io Group 5 already grade this pin's pot-gating in accepted "
        "chapters, RED -- port 2's copy of the gate has no external term at all (userio.v:169, "
        "joy2_pin5 = ~(potreg[13] & ~potreg[12]), a pure function of the POTGO write register). "
        "Declined as a re-grade. The register names this page adds (POT[0/1]DAT<7:0>, POTGO, "
        "POTINP) are themselves covered by hrm/ch8-pot-counter-start-and-charge G17 and "
        "hrm/ch8-potgo-register-bit-layout PROP-12.");
}

//--------------------------------------------------------------------------
// G9 -- p.332 figure: pin 9, POTY. Already graded, ASSERT.
//--------------------------------------------------------------------------
static void group_G9_pin9_poty() {
    record(
        "sol:E-pot.c24, opus:E-pot.c27 (p.332 figure, pin 9: POTY, 'Y analog in', "
        "POT[0,1]DAT<15:8>, POTGO, POTINP): hrm/ch8-controller-pin-function-map Group B already "
        "grades this pin's pot-gating in an accepted chapter, ASSERT -- this gate is symmetric "
        "and live on both ports. Declined as a re-grade, for the same reason as pin 5's decline "
        "above. The register names this page adds are covered by "
        "hrm/ch8-pot-counter-start-and-charge G16 and hrm/ch8-potgo-register-bit-layout "
        "PROP-11.");
}

//--------------------------------------------------------------------------
// G10 -- p.332 POT COUNTERS figure: port -> counter -> register structure
// and byte layout. Already graded; port numbering is a known, unresolved
// printed ambiguity, carried forward and not adjudicated here.
//--------------------------------------------------------------------------
static void group_G10_counter_layout() {
    record(
        "sol:E-pot.c25, sol:E-pot.c26, sol:E-pot.c28, sol:E-pot.c29, opus:E-pot.c29, "
        "opus:E-pot.c30, opus:E-pot.c31, opus:E-pot.c33 (p.332 POT COUNTERS figure: "
        "PORT->POT[0/1][X/Y]->POT[0/1]DAT structure, high/low byte assignment): "
        "hrm/ch8-pot-counter-start-and-charge G14-G17 already grade the byte assignment (Y high "
        "byte, X low byte) and which connector feeds which register, in an accepted chapter. "
        "Declined as a re-grade. PORT NUMBERING IS NOT SETTLED AND IS NOT RESOLVED HERE: p.330 "
        "labels its two connectors PORT 1/PORT 2; p.332's own POT COUNTERS figure labels the "
        "identical pair PORT 0/PORT 1; p.333's LIGHT PEN figure labels one of them PORT 0 -- "
        "three consecutive figures, two numbering conventions, within one appendix. "
        "hrm/ch8-ciaapra-fire-buttons carries this as KNOWN CONFLICT 3 and grades only the "
        "numeral-free core; this package does the same -- no claim here asserts a specific "
        "numeral binding, and none is resolved by assumption.");
}

//--------------------------------------------------------------------------
// G11 -- p.332 figure: POT0DAT/POT1DAT/POTGO register addresses. Already
// graded.
//--------------------------------------------------------------------------
static void group_G11_register_map() {
    full_reset();
    arm_potgo(0x0001);
    set_mouse(0, 0, 1); // release pot0x's gate so a known, chosen value can be driven
    settle(3);
    pulse(6);
    uint16_t at_012 = pot0dat();
    note("Address confirmation: after 6 known pot_cnt_en pulses, $DFF012 (POT0DAT) reads $" +
         hex4(at_012) + ".");

    record(
        "sol:E-pot.c27, sol:E-pot.c30, sol:E-pot.c31, opus:E-pot.c32, opus:E-pot.c34, "
        "opus:E-pot.c35 (p.332 figure: POT0DAT=DFF012, POT1DAT=DFF014, POTGO=DFF034): "
        "hrm/ch8-register-map-userio already grades all three under PROP-ADDR-POT0DAT, "
        "PROP-ADDR-POT1DAT and PROP-ADDR-POTGO, in an accepted chapter, from p.229, p.281/282 "
        "and p.289. Identical propositions; declined as re-grades.");
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
    bool alive = (live0 == 0x0003);
    note("Fixture liveness: releasing pot0x and pulsing 3 times reads POT0DAT=$" + hex4(live0) +
         " -- a specific, chosen, non-zero word, delivered before any claim below is graded.");
    if (!alive) {
        std::cerr << "FATAL: fixture cannot deliver a known word; aborting before grading any "
                     "claim.\n";
        return 2;
    }

    group_A1_pin1();
    group_A2_pin2_pin6_exclusion();
    group_C_potinp_address_conflict();
    group_G1_joydat_byte_layout();
    group_G2_reset_interval();
    group_G3_conversion_compare();
    group_G4_conversion_latch();
    group_G5_button_identity();
    group_G6_potgo_potinp_readback();
    group_G7_pin34_device_identity();
    group_G8_pin5_potx();
    group_G9_pin9_poty();
    group_G10_counter_layout();
    group_G11_register_map();

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
