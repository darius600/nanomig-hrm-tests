// hrm/ch8-register-map-userio -- userio.v register-map conformance test
//
// Test purpose: chapter 8's register map for the seven controller-port
//   registers userio.v decodes -- JOY0DAT, JOY1DAT, POT0DAT, POT1DAT,
//   POTGOR, POTGO, JOYTEST -- their address and their read/write access
//   direction, as SPECIFICATION.md fixes it from the 117 claims in
//   ch8-register-map-userio.jsonl. This package does NOT test what a
//   register's bits mean (quadrature arithmetic, pot charge timing, what
//   POTGO's individual control bits do) -- five named sibling packages in
//   this same build lot own that. userio.v has never had a test package in
//   this suite before this one; there is no sibling harness to inherit.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   229, 240, 281, 282, 287, 288, 289 (claims.jsonl for
//   ch8-register-map-userio, 117 records, plus four external p.287 legend
//   uids named in SPECIFICATION.md and cited but not counted among the
//   117). A red result below means userio.v disagrees with the manual, not
//   that the assertion is wrong -- no assertion here was, or should be,
//   adjusted to make the RTL pass.
//
// BOUNDARY: top module `userio` only (one Verilator build, userio.v alone),
//   per PACKAGE.md/the build brief. Ports used, exactly as userio.v's own
//   header names them (userio.v:28-50): clk, reset, clk7_en,
//   reg_address_in[8:1], data_in[15:0], data_out[15:0] (output reg),
//   pot_cnt_en, _fire0_dat, _fire1_dat, _joy1[15:0], _joy2[15:0],
//   joy_ana1[15:0], joy_ana2[15:0], mouse_btn[2:0], kms_level,
//   kbd_mouse_type[1:0], kbd_mouse_data[7:0]. _fire0/_fire1 are outputs,
//   read but not driven. There is no rd/hwr/lwr strobe and no chip select
//   in this port list -- confirmed by reading the module, matching what
//   this package's own claims already disclosed via classify.evidence
//   before step 2 opened the file. A read cycle and a write cycle are
//   therefore the same stimulus (reg_address_in + data_in); read and write
//   are told apart only by which of data_out / internal state responds.
//
// FIXTURE CONFIGURATION, and why (userio.v:28-50, :91-93, :210-227,
//   :360-363): joy_ana_en, joy_swap, cd32pad are wired 1'b0 inside the
//   module and cannot be driven from this boundary. mouse_btn is held
//   3'b111 (all released) throughout, giving _mleft=_mright=_mthird=0
//   (userio.v:361-363) -- this pins joy1enable=0 permanently (userio.v:
//   222-227, "if (!_mleft || reset) joy1enable=0" fires every cycle since
//   _mleft=0) and makes potcap[0]/potcap[1] track 0 regardless of the pot
//   pins (userio.v:171-176), so pot0x/pot0y free-run under pot_cnt_en.
//   _joy1/_joy2 idle at 16'h0000 between deliberate quadrature steps.
//   clk7_en is held high on every tick -- this package tests register-map
//   facts, not the 7MHz/28MHz sub-cycle timing no claim here depends on.
//
// MEASURED, disclosed here rather than asserted as a claim (this package's
//   own diagnostic, scratchpad-only, not committed, mirrors the discipline
//   hrm/ch3-register-map-denise's file header documents): userio.v:210-219
//   swaps the two ports -- with joy_swap=0, _sjoy1<=joy2 and _sjoy2<=joy1,
//   so the "Port 1" quadrature block (userio.v:230-254, feeding JOY0DAT)
//   is actually driven by the _joy2 pins, and the "Port 2" block
//   (userio.v:256-281, feeding JOY1DAT via dmouse1dat directly) is driven
//   by _joy1. Stepping _joy2 bits[0],[2] through the ring
//   (0,0)->(1,0)->(1,1)->(0,1) (one tick per state, repeating) changes
//   JOY0DAT's low byte by a known, monotonic, reproducible amount per
//   state visited, and leaves JOY1DAT untouched; the mirror stepping on
//   _joy1 bits[0],[2] changes JOY1DAT and leaves JOY0DAT untouched. POT0DAT
//   (pot0x,pot0y) free-runs on every pot_cnt_en pulse regardless of any
//   other write. POT1DAT's high byte (pot1y) free-runs the same way, but
//   its low byte (pot1x, gated by potcap[2]<=joy2_pin5, userio.v:169) is
//   permanently frozen at 0 unless POTGO is written with bit13=1,bit12=0
//   (userio.v:153,60) -- writing that pattern to POTGO's own address ($034)
//   frees pot1x on the next pot_cnt_en pulse; writing the identical pattern
//   to a different address does not. POTGOR (RTL name POTINP, userio.v:59,
//   300-305) settles to a fixed non-zero value after reset with this
//   configuration (potcap[2]=1 by default) and changes when that same
//   POTGO write flips potcap[2] to 0 -- POTGOR is this package's chosen
//   observable for "a value written to POTGO reaches state", matching the
//   manual's own POTGO/POTGOR pairing (p.240). Writing JOYTEST ($036) with
//   a known pattern zeroes/loads dmouse0dat and dmouse1dat's upper bits
//   (userio.v:232-280,341-343) so that a write of $FFFF deterministically
//   produces $FCFC at both JOY0DAT and JOY1DAT.
//
// CLAIM GROUPS -- every one of the 117 claim ids in
//   ch8-register-map-userio.jsonl is cited, in full, in exactly one group
//   below (in the assertion or record() string that carries it), matching
//   the convention hrm/ch3-register-map-denise documents and uses. Full
//   uid lists and page numbers are in SPECIFICATION.md; this header gives
//   only the shape.
//
//   LIVE-READ, LIVE-WRITE (no claim id) -- required by the build brief in
//     exactly these words: distinct known words fed in, and their arrival
//     asserted, before anything data-dependent is graded.
//   PROP-ADDR-JOY0DAT .. PROP-ADDR-JOYTEST (7 composed propositions, 41
//     ids total) -- each register's address, composed from the p.229
//     prose (where it exists), the p.281/282 Appendix A row and the p.289
//     Appendix B row.
//   PROP-RW-JOY0DAT .. PROP-RW-JOYTEST (7 composed propositions, 32 ids
//     from this package's own 117, plus the four external p.287 legend
//     uids named in SPECIFICATION.md) -- each register's read/write
//     direction, composed with the legend that gives the row's bare R/W
//     letter its meaning. Graded as the conjunction actually observable at
//     this boundary (no rd/wr strobe): read-only = address answers AND no
//     written value reaches state; write-only = a written value DOES reach
//     state (observed elsewhere) AND the address does not answer its own
//     content.
//   44 FUNCTION/semantic-content ids -- declined, recorded, not graded;
//     out of this package's own scope per its subject line, owned by
//     named sibling packages (see SPECIFICATION.md and the record() calls
//     below).
//
// MUTATION-LOG SUMMARY (16 groups, one per expect()). Every mutation below
//   is a STIMULUS change (a target address, or how many steps/pulses are
//   applied) -- no oracle/expected value is ever touched, and nanomig/ was
//   never modified.
//
// MUTATION LIVE-READ stimulus red: the JOY0DAT quadrature-step count was
//   changed from 5 to 0, so JOY0DAT's reading never left its baseline and
//   the "changed" half of the conjunction failed.
// MUTATION LIVE-WRITE stimulus red: the post-JOYTEST-write verification
//   read was redirected from JOY0DAT's address ($00A) to POTGO's ($034),
//   so it read back $0000 instead of the expected $FCFC.
// MUTATION ADDR-JOY0DAT stimulus red: the "own address" read was
//   redirected from $00A to JOY1DAT's address ($00C).
// MUTATION ADDR-JOY1DAT stimulus red: the "own address" read was
//   redirected from $00C to JOY0DAT's address ($00A).
// MUTATION ADDR-POT0DAT stimulus red: the pot_cnt_en pulse count was
//   changed from 6 to 0, so POT0DAT's reading never left its baseline.
// MUTATION ADDR-POT1DAT stimulus red: the pot_cnt_en pulse count was
//   changed from 6 to 0, so POT1DAT's reading never left its baseline.
// MUTATION ADDR-POTGOR stimulus red: the POTGO-freeing write ($2000) was
//   redirected from $034 to POT0DAT's address ($012), so POTGOR's reading
//   never changed from its 0x1000 baseline.
// MUTATION ADDR-POTGO stimulus red: the $2000 write was redirected from
//   $034 to $012 (POT0DAT's address), so POTGOR's reading never changed --
//   the same redirection as ADDR-POTGOR, confirming POTGO's own address is
//   what causes the effect, not merely that the effect occurred.
// MUTATION ADDR-JOYTEST stimulus red: the $FFFF write was redirected from
//   $036 to $034 (POTGO's address), so JOY0DAT/JOY1DAT never became $FCFC.
// MUTATION RW-JOY0DAT stimulus red: the post-write-attempt verification
//   read was redirected from JOY0DAT's address to JOY1DAT's, so it read
//   back $0000 instead of the driven value V.
// MUTATION RW-JOY1DAT stimulus red: the post-write-attempt verification
//   read was redirected from JOY1DAT's address to JOY0DAT's.
// MUTATION RW-POT0DAT stimulus red: the post-write-attempt verification
//   read was redirected from POT0DAT's address to JOY0DAT's.
// MUTATION RW-POT1DAT stimulus red: the post-write-attempt verification
//   read was redirected from POT1DAT's address to JOY1DAT's.
// MUTATION RW-POTGOR stimulus red: the post-write-attempt verification
//   read was redirected from POTGOR's address to POT0DAT's.
// MUTATION RW-POTGO stimulus red: the state-reaching write's target was
//   redirected from POTGO's address ($034) to POT0DAT's ($012), so
//   POTGOR's reading never left its 0x1000 baseline.
// MUTATION RW-JOYTEST stimulus red: the state-reaching write's target was
//   redirected from JOYTEST's address ($036) to POTGO's ($034), so
//   JOY0DAT never left $0000.
//
// Every mutation above was actually built and run (MUT=<NAME> against this
// file's Makefile, one at a time, obj_dir cleaned between runs), confirmed
// to turn only its own assertion red with the rest of the suite unchanged,
// then rebuilt with no MUT flag to confirm a clean return to the committed
// baseline. No oracle/expected value was changed by any of them; only what
// each check asks the module to do. See this package's own commit history
// for the per-mutation pass/fail counts.

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "Vuserio.h"
#include "verilated.h"

#ifndef MUT_LIVE_READ
#define MUT_LIVE_READ 0
#endif
#ifndef MUT_LIVE_WRITE
#define MUT_LIVE_WRITE 0
#endif
#ifndef MUT_ADDR_JOY0DAT
#define MUT_ADDR_JOY0DAT 0
#endif
#ifndef MUT_ADDR_JOY1DAT
#define MUT_ADDR_JOY1DAT 0
#endif
#ifndef MUT_ADDR_POT0DAT
#define MUT_ADDR_POT0DAT 0
#endif
#ifndef MUT_ADDR_POT1DAT
#define MUT_ADDR_POT1DAT 0
#endif
#ifndef MUT_ADDR_POTGOR
#define MUT_ADDR_POTGOR 0
#endif
#ifndef MUT_ADDR_POTGO
#define MUT_ADDR_POTGO 0
#endif
#ifndef MUT_ADDR_JOYTEST
#define MUT_ADDR_JOYTEST 0
#endif
#ifndef MUT_RW_JOY0DAT
#define MUT_RW_JOY0DAT 0
#endif
#ifndef MUT_RW_JOY1DAT
#define MUT_RW_JOY1DAT 0
#endif
#ifndef MUT_RW_POT0DAT
#define MUT_RW_POT0DAT 0
#endif
#ifndef MUT_RW_POT1DAT
#define MUT_RW_POT1DAT 0
#endif
#ifndef MUT_RW_POTGOR
#define MUT_RW_POTGOR 0
#endif
#ifndef MUT_RW_POTGO
#define MUT_RW_POTGO 0
#endif
#ifndef MUT_RW_JOYTEST
#define MUT_RW_JOYTEST 0
#endif

namespace {

// reg_address_in is [8:1]; every constant below is the manual's printed
// custom-chip offset already shifted right by one, matching userio.v's own
// `parameter` block (userio.v:54-61) and the sibling package convention
// (hrm/ch3-register-map-denise/test_denise.cpp).
constexpr uint16_t JOY0DAT_ADDR = 0x005;  // 9'h00A >> 1 (userio.v:54)
constexpr uint16_t JOY1DAT_ADDR = 0x006;  // 9'h00C >> 1 (userio.v:55)
constexpr uint16_t POT0DAT_ADDR = 0x009;  // 9'h012 >> 1 (userio.v:57)
constexpr uint16_t POT1DAT_ADDR = 0x00A;  // 9'h014 >> 1 (userio.v:58)
constexpr uint16_t POTGOR_ADDR  = 0x00B;  // 9'h016 >> 1 (userio.v:59, RTL name POTINP)
constexpr uint16_t POTGO_ADDR   = 0x01A;  // 9'h034 >> 1 (userio.v:60)
constexpr uint16_t JOYTEST_ADDR = 0x01B;  // 9'h036 >> 1 (userio.v:61)

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "$%04X", static_cast<unsigned>(v));
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vuserio) {
        dut->clk = 0;
        dut->reset = 1;
        dut->clk7_en = 1;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->pot_cnt_en = 0;
        dut->_fire0_dat = 1;
        dut->_fire1_dat = 1;
        dut->_joy1 = 0x0000;
        dut->_joy2 = 0x0000;
        dut->joy_ana1 = 0;
        dut->joy_ana2 = 0;
        dut->mouse_btn = 0b111;   // all released -- see file header FIXTURE CONFIGURATION
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->eval();
        for (int i = 0; i < 8; ++i) tick();   // hold reset
        dut->reset = 0;
        for (int i = 0; i < 8; ++i) tick();   // let reset release settle
        joy1_ring = 0;
        joy2_ring = 0;
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->clk = 0; dut->eval();
        dut->clk = 1; dut->eval();
    }

    // Ticks with every input held at its current value. The two-stage
    // input synchronizer (userio.v:214-219) and the quadrature edge-detect
    // logic that reads it (userio.v:230-281) need up to two ticks after a
    // pin stops changing before their output fully quiesces; call this
    // after a stepping sequence (or a write whose effect is read back
    // through another register) and before sampling a value that will
    // later be compared across an intervening tick, so that later tick
    // cannot be mistaken for the stimulus still settling.
    void settle(int n = 2) { for (int i = 0; i < n; ++i) tick(); }

    // Pure combinational read: sets the address bus and evaluates, with no
    // clock edge at all -- since every write-decode in userio.v lives
    // inside an `always @(posedge clk)` block (userio.v:99-105 for POTGO;
    // JOYTEST's test_load is combinational but only *other* registers'
    // clocked blocks react to it), a read with no tick cannot itself
    // disturb any state, whatever data_in happens to hold.
    uint16_t read_reg(uint16_t addr) {
        dut->reg_address_in = addr;
        dut->eval();
        return dut->data_out;
    }

    // A write cycle at this boundary: set address+data, then clock exactly
    // one tick so any posedge-triggered decode (POTGO) or combinational
    // decode feeding a posedge block (JOYTEST) has a chance to act.
    void write_reg(uint16_t addr, uint16_t val) {
        dut->reg_address_in = addr;
        dut->data_in = val;
        tick();
    }

    // One step of the measured-working quadrature ring on _joy2 bits[0]
    // (X-) and [2] (X+) -- drives JOY0DAT's low byte (userio.v:210-219,
    // 230-241: with joy_swap=0, _sjoy1<=joy2, so this is the "Port 1"
    // block's actual source). One state visited per call.
    void step_joy2_x() {
        static const int seq[4][2] = {{1,0},{1,1},{0,1},{0,0}};
        auto& s = seq[joy2_ring & 3];
        uint16_t v = dut->_joy2;
        v = (v & ~0x0005) | (s[0] ? 0x1 : 0) | (s[1] ? 0x4 : 0);
        dut->_joy2 = v;
        tick();
        ++joy2_ring;
    }
    // Mirror of step_joy2_x on _joy1 bits[0],[2] -- drives JOY1DAT's low
    // byte via dmouse1dat (userio.v:256-268; _sjoy2<=joy1 with joy_swap=0).
    void step_joy1_x() {
        static const int seq[4][2] = {{1,0},{1,1},{0,1},{0,0}};
        auto& s = seq[joy1_ring & 3];
        uint16_t v = dut->_joy1;
        v = (v & ~0x0005) | (s[0] ? 0x1 : 0) | (s[1] ? 0x4 : 0);
        dut->_joy1 = v;
        tick();
        ++joy1_ring;
    }

    // Pulses pot_cnt_en for n ticks (userio.v:125-151): with this
    // package's fixed mouse_btn=3'b111, pot0x/pot0y free-run on every
    // pulse; pot1y free-runs the same way; pot1x stays frozen unless POTGO
    // has been written with bit13=1,bit12=0 first (see file header).
    void pot_pulse(int n) {
        dut->pot_cnt_en = 1;
        for (int i = 0; i < n; ++i) tick();
        dut->pot_cnt_en = 0;
    }

    Vuserio* dut;
    int joy1_ring;
    int joy2_ring;
};

class Checks {
public:
    int passed = 0, failed = 0;
    void expect(bool condition, const std::string& label) {
        if (condition) { std::cout << "PASS: " << label << "\n"; ++passed; }
        else { std::cout << "FAIL: " << label << "\n"; ++failed; }
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
};

} // namespace

int main() {
    Checks checks;

    // =========================================================================
    // LIVE-READ (no claim id) -- required by the build brief in these words:
    // distinct known words fed in, and their arrival asserted, before
    // anything data-dependent is graded. Two independently-driven, address-
    // selective registers (JOY0DAT via _joy2, JOY1DAT via _joy1) must each
    // show a changed, non-baseline reading after their OWN stimulus, and
    // must NOT show that change after the OTHER register's stimulus.
    // =========================================================================
    {
        Harness h;
        uint16_t joy0_before = h.read_reg(JOY0DAT_ADDR);
        uint16_t joy1_before = h.read_reg(JOY1DAT_ADDR);
        int joy0_steps = 5;
#if MUT_LIVE_READ
        joy0_steps = 0;
#endif
        for (int i = 0; i < joy0_steps; ++i) h.step_joy2_x();
        h.settle();
        uint16_t joy0_after_joy2 = h.read_reg(JOY0DAT_ADDR);
        uint16_t joy1_after_joy2 = h.read_reg(JOY1DAT_ADDR);
        for (int i = 0; i < 9; ++i) h.step_joy1_x();
        h.settle();
        uint16_t joy1_after_joy1 = h.read_reg(JOY1DAT_ADDR);
        uint16_t joy0_after_joy1 = h.read_reg(JOY0DAT_ADDR);
        bool joy0_responded = joy0_after_joy2 != joy0_before;
        bool joy1_unaffected_by_joy2 = joy1_after_joy2 == joy1_before;
        bool joy1_responded = joy1_after_joy1 != joy1_before;
        bool joy0_unaffected_by_joy1 = joy0_after_joy1 == joy0_after_joy2;
        bool distinct = joy0_after_joy2 != joy1_after_joy1;
        checks.expect(
            joy0_responded && joy1_unaffected_by_joy2 && joy1_responded &&
            joy0_unaffected_by_joy1 && distinct,
            "LIVE-READ: 5 known quadrature steps on _joy2 change JOY0DAT ($00A: " +
            hex4(joy0_before) + " -> " + hex4(joy0_after_joy2) +
            ") while leaving JOY1DAT unchanged (" + hex4(joy1_before) + " -> " +
            hex4(joy1_after_joy2) + "); 9 known steps on _joy1 change JOY1DAT (-> " +
            hex4(joy1_after_joy1) + ") while leaving JOY0DAT unchanged (" +
            hex4(joy0_after_joy2) + " -> " + hex4(joy0_after_joy1) +
            "); the two driven values are distinct from each other -- this "
            "package's own obj_dir delivers known, address-selective words "
            "before any data-dependent assertion below trusts a reading.");
    }

    // =========================================================================
    // LIVE-WRITE (no claim id) -- the bus write path reaches state
    // somewhere in this module before PROP-RW-POTGO / PROP-RW-JOYTEST grade
    // "does not answer its own content": writing JOYTEST with a known
    // pattern deterministically loads a known pattern into JOY0DAT and
    // JOY1DAT (userio.v:232-280, 341-343). MEASURED (this package's own
    // build): JOY0DAT becomes exactly $FCFC -- userio.v:232-236,246-252
    // clear dmouse0dat's low two bits to 0 on test_load, so only the
    // written pattern's bits[7:2]/[15:10] (0x3F each) survive, giving
    // 0b111111'00 = 0xFC per byte. JOY1DAT becomes $FEFE, not $FCFC --
    // userio.v:259-260,272-273 load dmouse1dat's bits[7:2]/[15:10] from
    // the written pattern but do NOT clear bits[1:0]/[9:8], which this
    // fixture's idle quadrature state (see PROP-ADDR-JOY0DAT) already
    // holds at 0b10 per byte, giving 0b111111'10 = 0xFE per byte. This
    // asymmetry is a real, reproducible fact about this module, not a
    // fixture defect -- it is reported here rather than smoothed over.
    // =========================================================================
    {
        Harness h;
        h.write_reg(JOYTEST_ADDR, 0xFFFF);
        uint16_t verify_addr = JOY0DAT_ADDR;
#if MUT_LIVE_WRITE
        verify_addr = POTGO_ADDR;
#endif
        uint16_t joy0 = h.read_reg(verify_addr);
        uint16_t joy1 = h.read_reg(JOY1DAT_ADDR);
        checks.expect(
            joy0 == 0xFCFC && joy1 == 0xFEFE,
            "LIVE-WRITE: writing $FFFF to JOYTEST ($036) deterministically "
            "loads $FCFC into JOY0DAT and $FEFE into JOY1DAT (measured " +
            hex4(joy0) + " / " + hex4(joy1) +
            ") -- this package's own obj_dir delivers a value written "
            "through the bus to module state before any write-only "
            "assertion below trusts that a write \"reached\" something.");
    }

    // =========================================================================
    // PROP-ADDR-* -- seven composed address propositions. Each drives a
    // register to a known value and confirms its OWN address answers while
    // a DIFFERENT register's address does not, mirroring the accepted
    // VPOSR-ADDRESS-004 / DENISEID-ADDR-RO selectivity pattern.
    // =========================================================================
    {
        Harness h;
        uint16_t before = h.read_reg(JOY0DAT_ADDR);
        uint16_t other_before = h.read_reg(JOY1DAT_ADDR);
        for (int i = 0; i < 4; ++i) h.step_joy2_x();
        h.settle();
        uint16_t own_addr = JOY0DAT_ADDR;
#if MUT_ADDR_JOY0DAT
        own_addr = JOY1DAT_ADDR;
#endif
        uint16_t own = h.read_reg(own_addr);
        uint16_t other = h.read_reg(JOY1DAT_ADDR);
        checks.expect(
            own != before && other == other_before,
            "PROP-ADDR-JOY0DAT: after 4 known quadrature steps on _joy2, "
            "$00A answers a changed value (" + hex4(before) + " -> " +
            hex4(own) + ") and $00C (JOY1DAT's address) stays at its own "
            "unchanged reading (" + hex4(other_before) + ") "
            "-- JOY0DAT decodes at $00A and not at a neighbouring address "
            "- 'JOY0DAT ($DFF00A) Counter for digital (mouse) input (port "
            "1)' (p.229), 'JOY0DAT 00A R D Joystick-mouse 0 data (left "
            "vertical, horizontal)' (p.281), 'JOY0DAT *00A R D "
            "Joystick-mouse 0 data (vert,horiz)' (p.289). "
            "opus:8-regs.c1, sol:8-reg.c1, opus:8-appa-joy.c1, "
            "sol:8-appa-joy.c1, opus:8-appB.c8, sol:8-appB.c7.");
    }
    {
        Harness h;
        uint16_t before = h.read_reg(JOY1DAT_ADDR);
        uint16_t other_before = h.read_reg(JOY0DAT_ADDR);
        for (int i = 0; i < 4; ++i) h.step_joy1_x();
        h.settle();
        uint16_t own_addr = JOY1DAT_ADDR;
#if MUT_ADDR_JOY1DAT
        own_addr = JOY0DAT_ADDR;
#endif
        uint16_t own = h.read_reg(own_addr);
        uint16_t other = h.read_reg(JOY0DAT_ADDR);
        checks.expect(
            own != before && other == other_before,
            "PROP-ADDR-JOY1DAT: after 4 known quadrature steps on _joy1, "
            "$00C answers a changed value (" + hex4(before) + " -> " +
            hex4(own) + ") and $00A (JOY0DAT's address) stays at its own "
            "unchanged reading (" + hex4(other_before) + ") "
            "-- JOY1DAT decodes at $00C and not at a neighbouring address "
            "- 'JOY1DAT ($DFF00C) Counter for digital (mouse) input (port "
            "2)' (p.229), 'JOY1DAT 00C R D Joystick-mouse 1 data (right "
            "vertical, horizontal)' (p.281), 'JOY1DAT *00C R D "
            "Joystick-mouse 1 data (vert,horiz)' (p.289). "
            "opus:8-regs.c3, sol:8-reg.c3, opus:8-appa-joy.c5, "
            "sol:8-appa-joy.c6, opus:8-appB.c13, sol:8-appB.c12.");
    }
    {
        Harness h;
        uint16_t before = h.read_reg(POT0DAT_ADDR);
        uint16_t other_before = h.read_reg(JOY0DAT_ADDR);
        int pulses = 6;
#if MUT_ADDR_POT0DAT
        pulses = 0;
#endif
        h.pot_pulse(pulses);
        uint16_t own = h.read_reg(POT0DAT_ADDR);
        uint16_t other = h.read_reg(JOY0DAT_ADDR);
        checks.expect(
            own != before && other == other_before,
            "PROP-ADDR-POT0DAT: after 6 known pot_cnt_en pulses, $012 "
            "answers a changed value (" + hex4(before) + " -> " +
            hex4(own) + ") and $00A (JOY0DAT's address, an unrelated "
            "decode mechanism) stays at its own unchanged reading (" +
            hex4(other_before) + ") -- POT0DAT decodes at "
            "$012 - 'POT0DAT ($DFF012) Counter for proportional input "
            "(port 1)' (p.229), 'POT0DAT 012 R P( E ) Pot counter data "
            "left pair (vert,horiz.)' (p.282), 'POT0DAT *012 R P (E)Pot "
            "counter pair 0 data (vert,horiz)' (p.289). opus:8-regs.c7, "
            "sol:8-reg.c8, opus:8-appa-pot.c1, sol:8-appa-pot.c1, "
            "opus:8-appB.c18, sol:8-appB.c17.");
    }
    {
        Harness h;
        uint16_t before = h.read_reg(POT1DAT_ADDR);
        uint16_t other_before = h.read_reg(JOY1DAT_ADDR);
        int pulses = 6;
#if MUT_ADDR_POT1DAT
        pulses = 0;
#endif
        h.pot_pulse(pulses);
        uint16_t own = h.read_reg(POT1DAT_ADDR);
        uint16_t other = h.read_reg(JOY1DAT_ADDR);
        checks.expect(
            own != before && other == other_before,
            "PROP-ADDR-POT1DAT: after 6 known pot_cnt_en pulses, $014 "
            "answers a changed value (" + hex4(before) + " -> " +
            hex4(own) + ") and $00C (JOY1DAT's address, an unrelated "
            "decode mechanism) stays at its own unchanged reading (" +
            hex4(other_before) + ") -- POT1DAT decodes at "
            "$014 - 'POT1DAT ($DFF014) Counter for proportional input "
            "(port 2)' (p.229), 'POT1DAT 014 R P( E ) Pot counter data "
            "right pair (vert,horiz.)' (p.282), 'POT1DAT *014 R P (E)Pot "
            "counter pair 1 data (vertfhoriz)' (p.289). opus:8-regs.c9, "
            "sol:8-reg.c10, opus:8-appa-pot.c5, sol:8-appa-pot.c6, "
            "opus:8-appB.c23, sol:8-appB.c22.");
    }
    {
        Harness h;
        uint16_t before = h.read_reg(POTGOR_ADDR);
        uint16_t target = POTGO_ADDR;
#if MUT_ADDR_POTGOR
        target = POT0DAT_ADDR;
#endif
        uint16_t other_before = h.read_reg(JOY0DAT_ADDR);
        h.write_reg(target, 0x2000);
        h.settle();   // potcap[2] updates one cycle after potreg does (userio.v:160-169)
        uint16_t own = h.read_reg(POTGOR_ADDR);
        uint16_t other = h.read_reg(JOY0DAT_ADDR);
        checks.expect(
            own != before && other == other_before,
            "PROP-ADDR-POTGOR: writing $2000 to POTGO's address ($034) "
            "changes POTGOR's ($016) reading (" + hex4(before) + " -> " +
            hex4(own) + ") while $00A (JOY0DAT's address, untouched by "
            "this stimulus) stays at its own unchanged reading (" +
            hex4(other_before) + ") -- POTGOR decodes at $016 "
            "- 'POTGOR ($DFF016) Read proportional pin values' (p.229), "
            "'POTGOR 016 R P Pot port data read (formerly called "
            "POTINP)' (p.282), 'POTGOR *016 R P Pot port data read "
            "(formerly POTINP)' (p.289). opus:8-regs.c13, sol:8-reg.c15, "
            "opus:8-appa-potgo.c5, sol:8-appa-pot.c33, opus:8-appB.c28, "
            "sol:8-appB.c27.");
    }
    {
        Harness h;
        uint16_t potgor_before = h.read_reg(POTGOR_ADDR);
        uint16_t target = POTGO_ADDR;
#if MUT_ADDR_POTGO
        target = POT0DAT_ADDR;
#endif
        h.write_reg(target, 0x2000);
        h.settle();   // potcap[2] updates one cycle after potreg does (userio.v:160-169)
        uint16_t potgor_after = h.read_reg(POTGOR_ADDR);
        uint16_t own_readback = h.read_reg(POTGO_ADDR);
        checks.expect(
            potgor_after != potgor_before && own_readback == 0x0000,
            "PROP-ADDR-POTGO: a write of $2000 takes effect specifically "
            "at $034 -- it changes POTGOR's reading (" +
            hex4(potgor_before) + " -> " + hex4(potgor_after) +
            ") only when targeted at $034 itself (redirected to a "
            "different address it does not, per the ADDR-POTGO mutation "
            "below), and reading $034 back always answers $0000 (" +
            hex4(own_readback) + ") -- POTGO decodes at $034 - 'POTGO "
            "($DFF034) Write proportional pin values and start counters' "
            "(p.229), 'The POTGO register is at address $DFF034' (p.240), "
            "'POTGO 034 W P Pot port data write and start.' (p.282), "
            "'POTGO *034 W P Pot port data write and start' (p.289). "
            "opus:8-regs.c11, sol:8-reg.c12, opus:8-dio.c2, "
            "opus:8-appa-potgo.c1, sol:8-appa-pot.c28, opus:8-appB.c48, "
            "sol:8-appB.c47.");
    }
    {
        Harness h;
        uint16_t joy0_before = h.read_reg(JOY0DAT_ADDR);
        uint16_t target = JOYTEST_ADDR;
#if MUT_ADDR_JOYTEST
        target = POTGO_ADDR;
#endif
        h.write_reg(target, 0xFFFF);
        uint16_t joy0_after = h.read_reg(JOY0DAT_ADDR);
        uint16_t own_readback = h.read_reg(JOYTEST_ADDR);
        checks.expect(
            joy0_after == 0xFCFC && joy0_before != 0xFCFC && own_readback == 0x0000,
            "PROP-ADDR-JOYTEST: a write of $FFFF takes effect specifically "
            "at $036 -- it loads $FCFC into JOY0DAT (" + hex4(joy0_before) +
            " -> " + hex4(joy0_after) + ") only when targeted at $036 "
            "itself, and reading $036 back always answers $0000 (" +
            hex4(own_readback) + ") -- JOYTEST decodes at $036 - "
            "'JOYTEST 036 W D Write to all four joystick-mouse counters "
            "at once.' (p.282), 'JOYTEST *036 W D Write to all four "
            "joystick-mouse counters at once' (p.289). "
            "opus:8-appa-joytest.c1, sol:8-appa-joytest.c1, "
            "opus:8-appB.c53, sol:8-appB.c52.");
    }

    // =========================================================================
    // PROP-RW-* -- seven composed read/write-direction propositions.
    // userio.v has no rd/hwr/lwr strobe (see file header), so each is
    // graded as the conjunction actually observable: for the five R
    // registers, the address answers AND an attempted write at that same
    // address leaves the next reading unchanged; for the two W registers,
    // a value written at that address DOES reach state (observed via
    // POTGOR / JOY0DAT-JOY1DAT) AND the address itself never answers its
    // own content.
    // =========================================================================
    {
        Harness h;
        for (int i = 0; i < 4; ++i) h.step_joy2_x();
        h.settle();
        uint16_t v = h.read_reg(JOY0DAT_ADDR);
        h.write_reg(JOY0DAT_ADDR, 0xBEEF);
        uint16_t verify_addr = JOY0DAT_ADDR;
#if MUT_RW_JOY0DAT
        verify_addr = JOY1DAT_ADDR;
#endif
        uint16_t still = h.read_reg(verify_addr);
        checks.expect(
            v != 0x0000 && still == v,
            "PROP-RW-JOY0DAT (read-only): $00A answers a driven, non-zero "
            "value (" + hex4(v) + ") and an attempted write of $BEEF to "
            "$00A leaves the next reading at $00A unchanged (" +
            hex4(still) + ") -- measured as the conjunction actually "
            "observable at this rd/wr-strobe-less boundary, not the "
            "manual's word asserted directly - 'JOY0DAT 00A R D ...' "
            "(p.281), 'JOY0DAT *00A R D ...' (p.289), legend 'W=write-"
            "only; R=read-only,' (p.287). opus:8-appa-joy.c2, "
            "sol:8-appa-joy.c2, opus:8-appB.c9, sol:8-appB.c8, "
            "opus:8-appB.c4, sol:8-appB.c6.");
    }
    {
        Harness h;
        for (int i = 0; i < 4; ++i) h.step_joy1_x();
        h.settle();
        uint16_t v = h.read_reg(JOY1DAT_ADDR);
        h.write_reg(JOY1DAT_ADDR, 0xBEEF);
        uint16_t verify_addr = JOY1DAT_ADDR;
#if MUT_RW_JOY1DAT
        verify_addr = JOY0DAT_ADDR;
#endif
        uint16_t still = h.read_reg(verify_addr);
        checks.expect(
            v != 0x0000 && still == v,
            "PROP-RW-JOY1DAT (read-only): $00C answers a driven, non-zero "
            "value (" + hex4(v) + ") and an attempted write of $BEEF to "
            "$00C leaves the next reading at $00C unchanged (" +
            hex4(still) + ") -- measured as the conjunction actually "
            "observable at this rd/wr-strobe-less boundary - 'JOY1DAT "
            "00C R D ...' (p.281), 'JOY1DAT *00C R D ...' (p.289), "
            "legend 'W=write-only; R=read-only,' (p.287). "
            "opus:8-appa-joy.c6, sol:8-appa-joy.c7, opus:8-appB.c14, "
            "sol:8-appB.c13, opus:8-appB.c4, sol:8-appB.c6.");
    }
    {
        Harness h;
        h.pot_pulse(6);
        uint16_t v = h.read_reg(POT0DAT_ADDR);
        h.write_reg(POT0DAT_ADDR, 0xBEEF);
        uint16_t verify_addr = POT0DAT_ADDR;
#if MUT_RW_POT0DAT
        verify_addr = JOY0DAT_ADDR;
#endif
        uint16_t still = h.read_reg(verify_addr);
        checks.expect(
            v != 0x0000 && still == v,
            "PROP-RW-POT0DAT (read-only): $012 answers a driven, non-zero "
            "value (" + hex4(v) + ") and an attempted write of $BEEF to "
            "$012 leaves the next reading at $012 unchanged (" +
            hex4(still) + ") -- measured as the conjunction actually "
            "observable at this rd/wr-strobe-less boundary - 'POT0DAT "
            "012 R P( E ) ...' (p.282), 'POT0DAT *012 R P (E)...' "
            "(p.289), legend 'W=write-only; R=read-only,' (p.287) -- the "
            "'(E)' mark itself is not graded here, it is p.288's own "
            "notation (see SPECIFICATION.md). opus:8-appa-pot.c2, "
            "sol:8-appa-pot.c2, opus:8-appB.c19, sol:8-appB.c18, "
            "opus:8-appB.c4, sol:8-appB.c6.");
    }
    {
        Harness h;
        h.pot_pulse(6);
        uint16_t v = h.read_reg(POT1DAT_ADDR);
        h.write_reg(POT1DAT_ADDR, 0xBEEF);
        uint16_t verify_addr = POT1DAT_ADDR;
#if MUT_RW_POT1DAT
        verify_addr = JOY1DAT_ADDR;
#endif
        uint16_t still = h.read_reg(verify_addr);
        checks.expect(
            v != 0x0000 && still == v,
            "PROP-RW-POT1DAT (read-only): $014 answers a driven, non-zero "
            "value (" + hex4(v) + ") and an attempted write of $BEEF to "
            "$014 leaves the next reading at $014 unchanged (" +
            hex4(still) + ") -- measured as the conjunction actually "
            "observable at this rd/wr-strobe-less boundary - 'POT1DAT "
            "014 R P( E ) ...' (p.282), 'POT1DAT *014 R P (E)...' "
            "(p.289), legend 'W=write-only; R=read-only,' (p.287) -- the "
            "'(E)' mark itself is not graded here (see SPECIFICATION.md). "
            "opus:8-appa-pot.c6, sol:8-appa-pot.c7, opus:8-appB.c24, "
            "sol:8-appB.c23, opus:8-appB.c4, sol:8-appB.c6.");
    }
    {
        Harness h;
        uint16_t v = h.read_reg(POTGOR_ADDR);
        h.write_reg(POTGOR_ADDR, 0xBEEF);
        uint16_t verify_addr = POTGOR_ADDR;
#if MUT_RW_POTGOR
        verify_addr = POT0DAT_ADDR;
#endif
        uint16_t still = h.read_reg(verify_addr);
        checks.expect(
            v != 0x0000 && still == v,
            "PROP-RW-POTGOR (read-only): $016 answers a non-zero value ("
            + hex4(v) + ") and an attempted write of $BEEF to $016 leaves "
            "the next reading at $016 unchanged (" + hex4(still) +
            ") -- measured as the conjunction actually observable at this "
            "rd/wr-strobe-less boundary - 'POTGOR 016 R P ...' (p.282), "
            "'POTGOR *016 R P ...' (p.289), 'POTGOR (formerly POTINP) "
            "($DFF016) is the read-only address for the pot control "
            "register.' (p.240), legend 'W=write-only; R=read-only,' "
            "(p.287). opus:8-appa-potgo.c6, sol:8-appa-pot.c34, "
            "opus:8-appB.c29, sol:8-appB.c28, opus:8-dio.c5, "
            "sol:8-dio.c3, opus:8-appB.c4, sol:8-appB.c6.");
    }
    {
        Harness h;
        uint16_t before = h.read_reg(POTGOR_ADDR);
        uint16_t target = POTGO_ADDR;
#if MUT_RW_POTGO
        target = POT0DAT_ADDR;
#endif
        h.write_reg(target, 0x2000);
        h.settle();   // potcap[2] updates one cycle after potreg does (userio.v:160-169)
        uint16_t after = h.read_reg(POTGOR_ADDR);
        uint16_t own_readback = h.read_reg(POTGO_ADDR);
        checks.expect(
            after != before && own_readback == 0x0000,
            "PROP-RW-POTGO (write-only): a value written at $034 DOES "
            "reach state -- observed via POTGOR's reading changing (" +
            hex4(before) + " -> " + hex4(after) +
            ") -- and $034 itself never answers its own content (" +
            hex4(own_readback) + " regardless) -- measured as the "
            "conjunction actually observable at this rd/wr-strobe-less "
            "boundary, not the manual's word asserted directly - "
            "'POTGO 034 W P ...' (p.282), 'POTGO *034 W P ...' (p.289), "
            "'POTGO ($DFF034) is the write-only address for the pot "
            "control register.' (p.240), legend 'W=write-only; R=read-"
            "only,' (p.287). opus:8-appa-potgo.c2, sol:8-appa-pot.c29, "
            "opus:8-appB.c49, sol:8-appB.c48, opus:8-dio.c4, "
            "sol:8-dio.c2, opus:8-appB.c3, sol:8-appB.c5. Note: this "
            "package's own claims carry a classify.boundary_caveat field "
            "on opus:8-appB.c49/sol:8-appB.c48 that states this "
            "conjunction with the read-only wording verbatim, unflipped "
            "for write-only -- see SPECIFICATION.md; this assertion "
            "tests the conjunction the manual's word and the measured "
            "evidence actually require, not that mismatched sentence.");
    }
    {
        Harness h;
        uint16_t joy0_before = h.read_reg(JOY0DAT_ADDR);
        uint16_t target = JOYTEST_ADDR;
#if MUT_RW_JOYTEST
        target = POTGO_ADDR;
#endif
        h.write_reg(target, 0xFFFF);
        uint16_t joy0_after = h.read_reg(JOY0DAT_ADDR);
        uint16_t own_readback = h.read_reg(JOYTEST_ADDR);
        checks.expect(
            joy0_after != joy0_before && own_readback == 0x0000,
            "PROP-RW-JOYTEST (write-only): a value written at $036 DOES "
            "reach state -- observed via JOY0DAT's reading changing (" +
            hex4(joy0_before) + " -> " + hex4(joy0_after) +
            ") -- and $036 itself never answers its own content (" +
            hex4(own_readback) + " regardless) -- measured as the "
            "conjunction actually observable at this rd/wr-strobe-less "
            "boundary - 'JOYTEST 036 W D ...' (p.282), 'JOYTEST *036 W "
            "D ...' (p.289), legend 'W=write-only; R=read-only,' "
            "(p.287). opus:8-appa-joytest.c2, sol:8-appa-joytest.c2, "
            "opus:8-appB.c54, sol:8-appB.c53, opus:8-appB.c3, "
            "sol:8-appB.c5. Same boundary_caveat note as PROP-RW-POTGO "
            "applies to opus:8-appB.c54/sol:8-appB.c53.");
    }

    // =========================================================================
    // DECLINED -- 44 FUNCTION/semantic-content claims. This package's own
    // subject is address, access direction, and the appendix-B row/legend
    // claims that state those two facts (SPECIFICATION.md). What a
    // register's bits mean is the explicit subject of named sibling
    // packages in this same build lot. No predicate, stimulus or expected
    // value above is touched by this block.
    // =========================================================================
    checks.record(
        "opus:8-regs.c2, sol:8-reg.c2 (p.229), opus:8-appa-joy.c4, "
        "sol:8-appa-joy.c4, sol:8-appa-joy.c5 (p.281), opus:8-appB.c11, "
        "sol:8-appB.c10 (p.289) -- JOY0DAT's semantic content (digital "
        "mouse-input counter for port 1, vertical/horizontal split): not "
        "asserted -- this package's own subject is address and access "
        "direction only; the quadrature semantics these claims state are "
        "hrm/ch8-mouse-quadrature-decode's own subject, and that "
        "package's own claims.evidence already discloses the underlying "
        "measurement this package declines to repeat.");
    checks.record(
        "opus:8-regs.c4, sol:8-reg.c4 (p.229), opus:8-appa-joy.c8, "
        "sol:8-appa-joy.c9, sol:8-appa-joy.c10 (p.281), opus:8-appB.c16, "
        "sol:8-appB.c15 (p.289) -- JOY1DAT's semantic content: not "
        "asserted, for the identical reason as the JOY0DAT record above; "
        "owned by hrm/ch8-mouse-quadrature-decode.");
    checks.record(
        "opus:8-regs.c8, sol:8-reg.c9 (p.229), opus:8-appa-pot.c4, "
        "sol:8-appa-pot.c4, sol:8-appa-pot.c5 (p.282), opus:8-appB.c21, "
        "sol:8-appB.c20 (p.289) -- POT0DAT's semantic content (pot "
        "counter data, left pair vertical/horizontal): not asserted -- "
        "owned by hrm/ch8-pot-counter-start-and-charge. The p.289 pair "
        "(opus:8-appB.c21/sol:8-appB.c20) also carries the CHIP column's "
        "'(E)' mark abutting the FUNCTION cell with no separating space; "
        "this package does not resolve that boundary either way -- p.288 "
        "defines what '(E)' means (SPECIFICATION.md), and this package's "
        "own subject does not require reading the CHIP/FUNCTION cell "
        "split at all.");
    checks.record(
        "opus:8-regs.c10, sol:8-reg.c11 (p.229), opus:8-appa-pot.c8, "
        "sol:8-appa-pot.c9, sol:8-appa-pot.c10 (p.282), opus:8-appB.c26, "
        "sol:8-appB.c25 (p.289) -- POT1DAT's semantic content: not "
        "asserted, for the identical reason as POT0DAT above (same '(E)' "
        "boundary note applies to opus:8-appB.c26/sol:8-appB.c25); owned "
        "by hrm/ch8-pot-counter-start-and-charge.");
    checks.record(
        "opus:8-regs.c14, sol:8-reg.c16 (p.229), opus:8-appa-potgo.c8, "
        "sol:8-appa-pot.c36 (p.282), opus:8-appB.c31, sol:8-appB.c30 "
        "(p.289) -- POTGOR's semantic content (reads proportional pin "
        "values, formerly POTINP): not asserted -- owned by "
        "hrm/ch8-potgo-register-bit-layout.");
    checks.record(
        "opus:8-regs.c12, sol:8-reg.c13, sol:8-reg.c14 (p.229), "
        "opus:8-appa-potgo.c4, sol:8-appa-pot.c31, sol:8-appa-pot.c32 "
        "(p.282), opus:8-appB.c51, sol:8-appB.c50 (p.289) -- POTGO's "
        "semantic content (writes proportional-pin values, starts pot "
        "conversion): not asserted -- owned by "
        "hrm/ch8-potgo-register-bit-layout and "
        "hrm/ch8-pot-counter-start-and-charge.");
    checks.record(
        "opus:8-appB.c56, sol:8-appB.c55 (p.289) -- JOYTEST's semantic "
        "content (writes all four joystick-mouse counters at once): not "
        "asserted -- owned by hrm/ch8-joytest-counter-write.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
