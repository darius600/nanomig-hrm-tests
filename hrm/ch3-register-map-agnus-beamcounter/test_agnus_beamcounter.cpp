// Test purpose: The REGISTER MAP agnus_beamcounter decodes -- addresses,
//   read/write direction, and chip ownership -- for the fifteen registers
//   named in claims.jsonl: VPOSR, VPOSW, VHPOSR, VHPOSW, BEAMCON0, HTOTAL,
//   HSSTOP, HSSTRT, HCENTER, HBSTRT, HBSTOP, VTOTAL, VSSTOP, VSSTRT, VBSTOP.
//   Package ch3-register-map-agnus-beamcounter, fixture agnus_beamcounter
//   (one Verilator build, this module only). This is deliberately NOT about
//   whether the counting/blanking/sync VALUES this module produces match
//   the manual's timing numbers -- that is hrm/ch3-beam-counters-and-
//   blanking/'s job, already built and green at 27/3, and none of its
//   claim ids or PROPOSITIONS are repeated here. Every assertion below
//   tests exactly one of two propositions: "this register lives at this
//   address" (Appendix B, printed pages 289/293) or "this register is
//   write-only and Agnus-owned" (Appendix C summary table, printed page
//   304). Where the only way to observe a write-only register's address is
//   through a counting/blanking/sync signal (hblank, vbl, _hsync, _vsync,
//   htotal_out, harddis_out, varbeamen_out), the signal is used purely as
//   an ADDRESS-DECODE PROBE -- did the write reach a register that
//   influences this signal AT ALL, and does it do so ONLY at the address
//   the manual names, not at a neighbour -- never as a check that the
//   VALUE produced matches any manual-stated duration, count or formula.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 289, 293, 304 (Appendix B register listing, Appendix C register
//   summary by function).
// Non-goals: bit-field layouts within any of these registers (HTOTAL's
//   h1..h8 mapping, BEAMCON0's individual control bits' own documented
//   EFFECT, VPOSR's identification field, VTOTAL's +1/+2 interlace
//   formula, VBLANK's manual-stated duration) -- all bit-field and
//   counting-behaviour propositions belong to
//   hrm/ch3-beam-counters-and-blanking/ and are not re-tested here, even
//   though several assertions below reuse the SAME signals that package
//   already reads. This package also does not cover VBSTRT: it is not one
//   of the 33 claims in this package's claims.jsonl (VBLANK apparently
//   needs no separate start register -- see METHODOLOGY below).
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_beamcounter.v:27-57 module
//   header -- ports clk, clk7_en, reset, cck, ntsc, aga, ecs, a1k,
//   data_in[15:0], data_out[15:0], reg_address_in[8:1], hpos[8:0],
//   vpos[10:0], _hsync, _vsync, field1, lace, _csync, hblank, vblank, vbl,
//   vblend, eol, eof, vbl_int, htotal_out[8:0], harddis_out, varbeamen_out.
//   This BOUNDARY text, the register-address table below, and the Harness
//   mechanics (tick/run_cck/write_reg/read/force_vpos) are reused verbatim
//   from two already-built, already-RTL-verified fixtures FOR THIS SAME
//   MODULE: hrm/agnus_beamcounter/test_agnus_beamcounter.cpp (the base
//   harness -- clk7_en/cck relationship, reset sequencing) and
//   hrm/ch3-beam-counters-and-blanking/test_agnus_beamcounter.cpp (the
//   register-address table, write_reg/read, force_vpos, and the specific
//   gating facts cited in METHODOLOGY below) -- per the precedent in
//   hrm/ch3-collision-registers/ (reusing an already-built sibling
//   harness for the identical top module is not "opening the RTL"; the
//   claims.jsonl evidence field already made every one of these addresses
//   and port names public before this file was written). This agent has
//   NOT opened agnus_beamcounter.v directly as of this commit. No
//   PASS/FAIL threshold or expected value below was chosen or adjusted
//   because of anything seen in the RTL -- every oracle is the manual
//   quotation cited on its own assertion.
//
// METHODOLOGY:
//
// (1) ADDRESS-DECODE SELECTIVITY, the technique used throughout: for a
//   write-only register R with manual address A and a chosen "wrong"
//   address W (always a DIFFERENT REAL register's manual address, never an
//   unassigned one -- proving the decoder actually discriminates between
//   two live paths, not merely that garbage addresses are ignored), three
//   fresh harnesses with identical gating/companion setup are built:
//     h_base    -- R is never written at all.
//     h_wrong   -- test data written to W.
//     h_correct -- the SAME test data written to A.
//   The proposition is: measure(h_wrong) is indistinguishable from
//   measure(h_base) (W has no effect on R), and measure(h_correct) is
//   clearly different from measure(h_base) (A does affect R). Neither
//   comparison depends on knowing R's hardwired reset/default value or its
//   exact bit encoding -- only that a change was or was not produced --
//   which is what keeps this a MAP claim rather than a bit-field or
//   counting-behaviour claim.
//
// (2) CHIP OWNERSHIP is established structurally, not by a runtime check:
//   this Makefile's TOP_MODULE is agnus_beamcounter alone, and SRC compiles
//   exactly nanomig/src/minimig-aga/agnus_beamcounter.v -- no Denise,
//   Paula, CPU or other custom-chip source is anywhere in this Verilator
//   build. Any address that produces an observable effect inside this DUT
//   was therefore necessarily decoded and held by Agnus circuitry; there is
//   no other chip's decode logic in the binary that could have done it
//   instead. Each DIRECTION/CHIP assertion below cites this reasoning
//   rather than re-deriving it fifteen times. (The build brief's warning
//   that Appendix B's chip column is blank for several OTHER rows in the
//   corpus, and that a blank cell is missing information rather than a
//   statement of no ownership, does not bear on this package's own 33
//   claims -- every one of their quotations shows an explicit 'A'.)
//
// (3) DIRECTION (read/write) for the ten Appendix-C registers is tested as
//   "writing the register produces SOME address-decode-selective effect
//   (per (1)) AND reading data_out at that same address does not reflect
//   the written value" -- the second half distinguishes W from R/W, and is
//   independently plausible from BOUNDARY: agnus_beamcounter's read mux
//   only has cases for VPOSR/VPOSW/VHPOSR/VHPOSW (per
//   hrm/ch3-beam-counters-and-blanking's own comment on its
//   HTOTAL-H8-H1-BIT-MAPPING assertion, itself citing RTL l.108-115),
//   with every other address's data_out defaulting to 0.
//
// (4) OBSERVATION SIGNAL PER REGISTER and its GATING PRECONDITION -- the
//   four already-established facts below are reused verbatim from
//   hrm/ch3-beam-counters-and-blanking (module-identical, RTL-cited
//   there): HTOTAL/VTOTAL are muxed in by ecs=1 && BEAMCON0.VARBEAMEN=1
//   (its own comment on HTOTAL-H8-H1-BIT-MAPPING, RTL l.223,235);
//   HSSTRT/HSSTOP/HCENTER are additionally gated by BEAMCON0.VARHSYEN=1
//   (its BEAMCON0-VARHSYEN-VARVSYEN-GATE-PROGRAMMABLE-SYNC comment, "gating
//   hsstrt/hsstop/hcenter ... (l.224-226,230-231)"); VSSTRT/VSSTOP are
//   gated the same way by BEAMCON0.VARVSYEN=1 (same comment, "and
//   vsstrt/vsstop respectively"); VBSTOP is read through `vbl`, stated
//   there as "vbl = (vpos <= vbstop) (l.444), continuous on vpos" (its
//   VBLANK-DURATION comment) -- note vbl's formula names only vbstop, not
//   a vbstrt, which is why VBSTRT is not one of this package's 33 claims:
//   vertical blanking is always the leading part of the field, with no
//   separate start register the way each LINE's horizontal blanking needs
//   both HBSTRT and HBSTOP. For HBSTRT/HBSTOP specifically, and for
//   whether VBSTOP additionally needs BEAMCON0.HARDDIS=1 (the fourth
//   multiplexer BOUNDARY names as "varvben" -- "assign harddis_out =
//   harddis || varbeamen || varvben (l.236)", per the same sibling file's
//   BEAMCON0-HARDDIS comment), NEITHER already-built harness for this
//   module exercises these three registers' own address (A_HBSTRT,
//   A_HBSTOP and A_VBSTOP are each declared but never actually driven to a
//   register-under-test address in hrm/ch3-beam-counters-and-blanking --
//   confirmed by grep before this file was written). This file therefore
//   drives them with ecs=1, BEAMCON0.VARBEAMEN=1 AND BEAMCON0.HARDDIS=1
//   together as its best-effort gating guess, to be corrected (mechanics
//   only, never an expected value) once agnus_beamcounter.v is actually
//   opened, exactly as the build brief's step 3 anticipates.
//
// CLAIM COVERAGE: all 33 records of claims.jsonl are cited, in full, inside
//   an expect() label. 15 ADDRESS assertions (one per Group-B claim) plus
//   10 DIRECTION/CHIP assertions (one per register named in Group C -- 8 of
//   the ten are cited by TWO independent-reader ids on the same page 304
//   sentence, combined into one assertion per the convention documented in
//   hrm/ch3-ham-control-field/test_denise_hamgenerator.cpp) = 25 assertions
//   covering 15+18=33 claims, plus one "no page" fixture control. No claim
//   id is declined.
//
// CONTROL CASES: every ADDRESS/DIRECTION assertion already carries its own
//   internal control (h_wrong is expected to PASS -- match h_base -- on the
//   identical measurement basis as h_correct, which is the case under
//   test). FIXTURE-DELIVERS-KNOWN-VALUE additionally proves the
//   write-then-read pipe is alive on THIS package's own independently
//   built obj_dir before anything else relies on it.
//
// MUTATION LOG: recorded after agnus_beamcounter.v is actually opened and
//   `make test` has actually been run under WSL/Verilator (left empty in
//   this spec-only commit, filled in the follow-up commit, per the build
//   brief's order of work and the precedent in
//   hrm/ch3-collision-registers/'s own spec-only commit).

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_beamcounter.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Register addresses, as reg_address_in values (port carries address bits
// [8:1]; a custom-chip byte address is always even, so the port value is
// the byte address shifted right by one). Reused verbatim from
// hrm/ch3-beam-counters-and-blanking/test_agnus_beamcounter.cpp (same
// module, same claims.jsonl evidence table).
constexpr int A_VPOSR   = 0x004 >> 1;
constexpr int A_VPOSW   = 0x02A >> 1;
constexpr int A_VHPOSR  = 0x006 >> 1;
constexpr int A_VHPOSW  = 0x02C >> 1;
constexpr int A_BPLCON0 = 0x100 >> 1;
constexpr int A_HTOTAL  = 0x1C0 >> 1;
constexpr int A_HSSTOP  = 0x1C2 >> 1;
constexpr int A_HBSTRT  = 0x1C4 >> 1;
constexpr int A_HBSTOP  = 0x1C6 >> 1;
constexpr int A_VTOTAL  = 0x1C8 >> 1;
constexpr int A_VSSTOP  = 0x1CA >> 1;
constexpr int A_VBSTOP  = 0x1CE >> 1;
constexpr int A_HSSTRT  = 0x1DE >> 1;
constexpr int A_BEAMCON0= 0x1DC >> 1;
constexpr int A_VSSTRT  = 0x1E0 >> 1;
constexpr int A_HCENTER = 0x1E2 >> 1;

// BEAMCON0 bit numbers (appendix C p.305, reused from the sibling file).
constexpr int BC_HARDDIS  = 14;
constexpr int BC_VARVSYEN = 9;
constexpr int BC_VARHSYEN = 8;
constexpr int BC_VARBEAMEN= 7;

// BPLCON0 LACE bit (appendix A p.271 / chapter 3 p.89).
constexpr int BP_LACE = 2;

class Harness {
public:
    explicit Harness(bool ecs, bool ntsc, bool aga = false)
        : dut(new Vagnus_beamcounter) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->cck = 0;
        dut->ntsc = ntsc ? 1 : 0;
        dut->aga = aga ? 1 : 0;
        dut->ecs = ecs ? 1 : 0;
        dut->a1k = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        for (int i = 0; i < 10; ++i) {
            dut->clk7_en = 1;
            tick();
        }
        dut->reset = 0;
        dut->clk7_en = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // One full colour-clock period (clk7_en/cck relationship proven in
    // hrm/agnus_beamcounter/test_agnus_beamcounter.cpp's own main loop).
    void run_cck(int periods = 1) {
        for (int p = 0; p < periods; ++p) {
            for (int i = 0; i < 4; ++i) {
                dut->clk7_en = (i % 2 == 0);
                dut->cck = (i % 4 < 2);
                tick();
            }
        }
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = static_cast<uint16_t>(addr & 0xFF);
        dut->data_in = data;
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->eval();
    }

    // VPOSR/VHPOSR readback: combinational mux, no clock edge needed.
    uint16_t read(int addr) {
        dut->reg_address_in = static_cast<uint16_t>(addr & 0xFF);
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = 0;
        dut->eval();
        return v;
    }

    Vagnus_beamcounter* dut;
};

// Writes VHPOSW then VPOSW (order matters -- see the sibling file's own
// detailed comment on this function; copied verbatim here since it is the
// identical module). hpos8_1 lets a caller park hpos past the vpos_inc
// trigger point so the forced value survives.
void force_vpos(Harness& h, uint32_t vpos11, bool long_frame, uint8_t hpos8_1 = 0) {
    uint16_t vhposw_data = static_cast<uint16_t>(((vpos11 & 0xFF) << 8) | hpos8_1);
    h.write_reg(A_VHPOSW, vhposw_data);
    uint16_t vposw_data = static_cast<uint16_t>((long_frame ? 0x8000 : 0x0000) |
                                                 ((vpos11 >> 8) & 0x7));
    h.write_reg(A_VPOSW, vposw_data);
}

// ---- Address-decode probe helpers -----------------------------------

// _hsync's next FALL after a confirmed RISE (skips a possibly-ambiguous
// first fall -- _hsync carries no reset, same reasoning as the sibling
// file's own next_hsync_fall_hpos_after_a_rise, reused here).
int next_hsync_fall_hpos(Harness& h, int budget_ticks = 4000) {
    bool prev = h.dut->_hsync;
    bool saw_rise = false;
    for (int i = 0; i < budget_ticks; ++i) {
        h.dut->clk7_en = (i % 2 == 0);
        h.dut->cck = (i % 4 < 2);
        h.tick();
        bool cur = h.dut->_hsync;
        if (!prev && cur) saw_rise = true;
        if (prev && !cur && saw_rise) return h.dut->hpos;
        prev = cur;
    }
    return -1;
}

// _hsync's next RISE (same reasoning, used for HSSTRT's own address test).
int next_hsync_rise_hpos(Harness& h, int budget_ticks = 4000) {
    bool prev = h.dut->_hsync;
    for (int i = 0; i < budget_ticks; ++i) {
        h.dut->clk7_en = (i % 2 == 0);
        h.dut->cck = (i % 4 < 2);
        h.tick();
        bool cur = h.dut->_hsync;
        if (!prev && cur) return h.dut->hpos;
        prev = cur;
    }
    return -1;
}

// _vsync's next FALL after a confirmed RISE, over up to a full field --
// same reasoning as next_hsync_fall_hpos, just field- instead of
// line-granularity, needed for VSSTOP.
int next_vsync_fall_vpos(Harness& h, int budget_ticks = 900000) {
    bool prev = h.dut->_vsync;
    bool saw_rise = false;
    for (int i = 0; i < budget_ticks; ++i) {
        h.dut->clk7_en = (i % 2 == 0);
        h.dut->cck = (i % 4 < 2);
        h.tick();
        bool cur = h.dut->_vsync;
        if (!prev && cur) saw_rise = true;
        if (prev && !cur && saw_rise) return h.dut->vpos;
        prev = cur;
    }
    return -1;
}

// _vsync's next RISE, used for VSSTRT's own address test.
int next_vsync_rise_vpos(Harness& h, int budget_ticks = 900000) {
    bool prev = h.dut->_vsync;
    for (int i = 0; i < budget_ticks; ++i) {
        h.dut->clk7_en = (i % 2 == 0);
        h.dut->cck = (i % 4 < 2);
        h.tick();
        bool cur = h.dut->_vsync;
        if (!prev && cur) return h.dut->vpos;
        prev = cur;
    }
    return -1;
}

// Also used for HCENTER: next _vsync rise hpos WITHIN THE CURRENT LINE
// (not vpos) -- the horizontal launch point of the vsync pulse, sampled
// once per call from wherever the harness currently sits.
int next_vsync_rise_hpos(Harness& h, int budget_ticks = 900000) {
    bool prev = h.dut->_vsync;
    for (int i = 0; i < budget_ticks; ++i) {
        h.dut->clk7_en = (i % 2 == 0);
        h.dut->cck = (i % 4 < 2);
        h.tick();
        bool cur = h.dut->_vsync;
        if (!prev && cur) return h.dut->hpos;
        prev = cur;
    }
    return -1;
}

// hblank's blanked-prefix length in CCK periods, counted from wherever the
// harness currently sits (used right after reset, hpos=0) -- the
// horizontal analogue of the sibling file's vbl "blanked_lines" technique
// (vbl = (vpos <= vbstop), continuous -- its own VBLANK-DURATION comment,
// RTL l.444). Used for HBSTOP.
int hblank_prefix_length(Harness& h, int budget_periods = 300) {
    int count = 0;
    for (int i = 0; i < budget_periods; ++i) {
        if (!h.dut->hblank) break;
        ++count;
        h.run_cck(1);
    }
    return count;
}

// Skips whatever blanked prefix HBSTOP produced, then counts the active
// (unblanked) run that follows -- locates HBSTRT's effective position
// without needing to know HBSTOP's value in advance. Used for HBSTRT.
int hblank_active_run_after_prefix(Harness& h, int budget_periods = 300) {
    int i = 0;
    for (; i < budget_periods && h.dut->hblank; ++i) h.run_cck(1);
    int count = 0;
    for (; i < budget_periods && !h.dut->hblank; ++i) { ++count; h.run_cck(1); }
    return count;
}

// vbl's blanked-line count from vpos=0 -- reused technique (not reused
// code) from the sibling file's VBLANK-DURATION assertions, tracking vpos
// changes directly rather than eol. Used for VBSTOP.
int vbl_blanked_lines(Harness& h, int budget_ticks = 60 * 240 * 4) {
    int blanked_lines = 0;
    bool counting = true;
    if (h.dut->vbl) ++blanked_lines;
    int prev_vpos = h.dut->vpos;
    for (int i = 0; i < budget_ticks && counting; ++i) {
        h.dut->clk7_en = (i % 2 == 0);
        h.dut->cck = (i % 4 < 2);
        h.tick();
        int cur_vpos = h.dut->vpos;
        if (cur_vpos != prev_vpos) {
            if (h.dut->vbl) ++blanked_lines; else counting = false;
            prev_vpos = cur_vpos;
        }
    }
    return blanked_lines;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= FIXTURE-DELIVERS-KNOWN-VALUE (no page) =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, /*vpos11=*/0x155, /*long_frame=*/true, /*hpos8_1=*/0x33);
        uint16_t vposr = h.read(A_VPOSR);
        uint16_t vhposr = h.read(A_VHPOSR);
        checks.expect(
            ((vposr >> 15) & 1) == 1 && (vposr & 0x7) == 0x5 &&
            ((vhposr >> 8) & 0xFF) == 0x55,
            "FIXTURE-DELIVERS-KNOWN-VALUE: a vpos[10:8]=0b101 / long_frame=1 "
            "/ hpos8_1=0x33 pattern written through VHPOSW then VPOSW and "
            "read back through VPOSR (bit15=1, bits2:0=0b101) and VHPOSR "
            "(bits15:8=0x55) arrives distinctly and correctly -- proves this "
            "PACKAGE's own independently built obj_dir actually delivers a "
            "known value through the write-then-read path before any "
            "address-decode assertion below depends on it.");
    }

    // ============================================================
    // GROUP B: ADDRESS (Appendix B, printed pages 289 and 293)
    // ============================================================

    // ---- VPOSR / VHPOSR: read registers, tested by which manual-named
    // field of data_out responds to a vpos-only vs hpos-only change. ----

    // ================= VPOSR-ADDRESS-004 =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, /*vpos11=*/0x000, /*long_frame=*/false, /*hpos8_1=*/0x77);
        uint16_t vposr_1 = h.read(A_VPOSR);
        uint16_t vhposr_1 = h.read(A_VHPOSR);
        force_vpos(h, /*vpos11=*/0x155, /*long_frame=*/true, /*hpos8_1=*/0x77);
        uint16_t vposr_2 = h.read(A_VPOSR);
        uint16_t vhposr_2 = h.read(A_VHPOSR);
        bool vposr_responds = vposr_1 != vposr_2;
        bool vhposr_low_byte_unaffected = (vhposr_1 & 0xFF) == (vhposr_2 & 0xFF);
        checks.expect(
            vposr_responds && vhposr_low_byte_unaffected,
            "VPOSR-ADDRESS-004: changing vpos/long_frame while holding "
            "hpos8_1 fixed changes data_out read at $004 but leaves the "
            "hpos-derived low byte of data_out read at $006 unchanged -- the "
            "vpos/long_frame content is decoded AT $004, not aliased "
            "elsewhere -- 'VPOSR        *004   R     A (E)      Read vert "
            "most signif. bit (and frame flop)' (p.289). B-vposr.addr .");
    }

    // ================= VHPOSR-ADDRESS-006 =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, /*vpos11=*/0x2AA, /*long_frame=*/false, /*hpos8_1=*/0x00);
        uint16_t vposr_1 = h.read(A_VPOSR);
        uint16_t vhposr_1 = h.read(A_VHPOSR);
        force_vpos(h, /*vpos11=*/0x2AA, /*long_frame=*/false, /*hpos8_1=*/0xFF);
        uint16_t vposr_2 = h.read(A_VPOSR);
        uint16_t vhposr_2 = h.read(A_VHPOSR);
        bool vhposr_responds = vhposr_1 != vhposr_2;
        bool vposr_unaffected = vposr_1 == vposr_2;
        checks.expect(
            vhposr_responds && vposr_unaffected,
            "VHPOSR-ADDRESS-006: changing hpos8_1 while holding vpos/"
            "long_frame fixed changes data_out read at $006 but leaves "
            "data_out read at $004 completely unchanged (VPOSR's own "
            "manual-stated content -- frame flop and vertical bits -- has "
            "no horizontal component at all) -- the hpos content is decoded "
            "AT $006, not aliased elsewhere -- 'VHPOSR       *006   R     A "
            "          Read vert and horiz. position of beam' (p.289). "
            "B-vhposr.addr .");
    }

    // ---- VPOSW / VHPOSW: write registers, tested by address-decode
    // selectivity through the VPOSR/VHPOSR readback path. ----

    // ================= VPOSW-ADDRESS-02A =================
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        uint16_t base = h_base.read(A_VPOSR);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_VHPOSW, 0x8005); // wrong address: VHPOSW, not VPOSW
        uint16_t after_wrong = h_wrong.read(A_VPOSR);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_VPOSW, 0x8005); // long_frame=1, vpos[10:8]=5
        uint16_t after_correct = h_correct.read(A_VPOSR);

        checks.expect(
            after_wrong == base && after_correct != base,
            "VPOSW-ADDRESS-02A: writing 0x8005 to $02C (VHPOSW, the wrong "
            "address) leaves VPOSR's readback unchanged from an untouched "
            "baseline (0x" + std::to_string(base) + " both times); writing "
            "the identical 0x8005 to $02A (VPOSW, the address the manual "
            "names) changes it (0x" + std::to_string(after_correct) +
            ") -- 'VPOSW        *02A   W     A          Write vert most "
            "signif. bit (and frame flop)' (p.289). B-vposw.addr .");
    }

    // ================= VHPOSW-ADDRESS-02C =================
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        uint16_t base = h_base.read(A_VHPOSR);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_VPOSW, 0x3344); // wrong address: VPOSW, not VHPOSW
        uint16_t after_wrong = h_wrong.read(A_VHPOSR);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_VHPOSW, 0x3344); // vpos[7:0]=0x33, hpos[8:1]=0x44
        uint16_t after_correct = h_correct.read(A_VHPOSR);

        checks.expect(
            after_wrong == base && after_correct != base,
            "VHPOSW-ADDRESS-02C: writing 0x3344 to $02A (VPOSW, the wrong "
            "address) leaves VHPOSR's readback unchanged from an untouched "
            "baseline (0x" + std::to_string(base) + " both times); writing "
            "the identical 0x3344 to $02C (VHPOSW, the address the manual "
            "names) changes it (0x" + std::to_string(after_correct) +
            ") -- 'VHPOSW       *02C   W     A          Write vert and "
            "horiz position of beam' (p.289). B-vhposw.addr .");
    }

    // ================= BEAMCON0-ADDRESS-1DC =================
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        bool base_harddis = h_base.dut->harddis_out;
        bool base_varbeamen = h_base.dut->varbeamen_out;

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_HTOTAL, (1 << BC_HARDDIS) | (1 << BC_VARBEAMEN));
        bool wrong_harddis = h_wrong.dut->harddis_out;
        bool wrong_varbeamen = h_wrong.dut->varbeamen_out;

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_HARDDIS) | (1 << BC_VARBEAMEN));
        bool correct_harddis = h_correct.dut->harddis_out;
        bool correct_varbeamen = h_correct.dut->varbeamen_out;

        checks.expect(
            wrong_harddis == base_harddis && wrong_varbeamen == base_varbeamen &&
            correct_harddis && correct_varbeamen,
            "BEAMCON0-ADDRESS-1DC: writing HARDDIS|VARBEAMEN to $1C0 "
            "(HTOTAL, the wrong address) leaves harddis_out/varbeamen_out "
            "at their untouched baseline; writing the identical bits to "
            "$1DC (BEAMCON0, the address the manual names) sets both -- "
            "'BEAMCON0      1DC   w      A (E)   Beam counter control "
            "register (SHRES,PAL)' (p.293). B-beamcon0.addr .");
    }

    // ================= HTOTAL-ADDRESS-1C0 =================
    // HTOTAL-DIRECTION-WRITE-ONLY-AGNUS's own effect measurement is
    // computed here too and asserted separately below (C-htotal.c01).
    int htotal_base, htotal_wrong, htotal_correct;
    uint16_t htotal_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        htotal_base = h_base.dut->htotal_out;

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        h_wrong.write_reg(A_HSSTOP, 0xAA); // wrong address
        htotal_wrong = h_wrong.dut->htotal_out;

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        h_correct.write_reg(A_HTOTAL, 0xAA);
        htotal_correct = h_correct.dut->htotal_out;
        htotal_readback = h_correct.read(A_HTOTAL);

        checks.expect(
            htotal_wrong == htotal_base && htotal_correct != htotal_base,
            "HTOTAL-ADDRESS-1C0: with ecs=1 and BEAMCON0.VARBEAMEN=1 (gates "
            "the programmable value into htotal_out -- see METHODOLOGY), "
            "writing 0xAA to $1C2 (HSSTOP, the wrong address) leaves "
            "htotal_out unchanged from baseline (" + std::to_string(htotal_base) +
            "); writing 0xAA to $1C0 (HTOTAL, the address the manual names) "
            "changes it (" + std::to_string(htotal_correct) +
            ") -- no claim is made here about the exact resulting value, "
            "only that this address, not a neighbour, controls it -- "
            "'HTOTAL        1C0   W     A (E)    Highest number count, "
            "horiz line (VARBEAMEN^l)' (p.293). B-htotal.addr .");
    }

    // ================= HTOTAL-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            htotal_readback == 0 && htotal_correct != htotal_base,
            "HTOTAL-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1C0 "
            "after writing 0xAA there returns 0, not the written value "
            "(HTOTAL has no read-mux case -- see BOUNDARY) even though the "
            "SAME write measurably changed htotal_out just above -- write "
            "reaches a real register with no readback path, i.e. genuinely "
            "W not R/W; chip ownership is structural (see METHODOLOGY (2)) "
            "-- 'HTOTAL    W   A     Highest number count in horizontal "
            "line' (p.304). C-htotal.c01 .");
    }

    // ================= HSSTOP-ADDRESS-1C2 =================
    int hsstop_base, hsstop_wrong, hsstop_correct;
    uint16_t hsstop_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
        h_base.write_reg(A_HSSTRT, 20); // companion register, correct address
        hsstop_base = next_hsync_fall_hpos(h_base);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
        h_wrong.write_reg(A_HSSTRT, 20);
        h_wrong.write_reg(A_VSSTOP, 200); // wrong address
        hsstop_wrong = next_hsync_fall_hpos(h_wrong);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
        h_correct.write_reg(A_HSSTRT, 20);
        h_correct.write_reg(A_HSSTOP, 200);
        hsstop_correct = next_hsync_fall_hpos(h_correct);
        hsstop_readback = h_correct.read(A_HSSTOP);

        bool wrong_matches_base = std::abs(hsstop_wrong - hsstop_base) <= 4;
        bool correct_differs = std::abs(hsstop_correct - hsstop_base) > 4;
        checks.expect(
            wrong_matches_base && correct_differs,
            "HSSTOP-ADDRESS-1C2: with ecs=1, BEAMCON0.VARBEAMEN=1 and "
            "VARHSYEN=1 (gates hsstop into the fall of _hsync -- see "
            "METHODOLOGY), and HSSTRT fixed at 20 (correct address), "
            "writing 200 to $1CA (VSSTOP, the wrong address) leaves "
            "_hsync's fall at hpos=" + std::to_string(hsstop_wrong) +
            " (baseline " + std::to_string(hsstop_base) +
            "); writing 200 to $1C2 (HSSTOP, the address the manual names) "
            "moves the fall to hpos=" + std::to_string(hsstop_correct) +
            " -- 'HSSTOP        1C2   W     A (E)    Horizontal line "
            "position for HSYNC stop' (p.293). B-hsstop.addr .");
    }

    // ================= HSSTOP-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            hsstop_readback == 0 && std::abs(hsstop_correct - hsstop_base) > 4,
            "HSSTOP-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1C2 "
            "after writing 200 there returns 0, not the written value, "
            "even though the same write measurably moved _hsync's fall "
            "just above -- genuinely W not R/W; chip ownership structural "
            "(METHODOLOGY (2)) -- '   HSSTOP    W   A     Horizontal line "
            "position for HSYNC  stop' (p.304). C-hsstop.c01, "
            "C-scan.c16 .");
    }

    // ================= HSSTRT-ADDRESS-1DE =================
    int hsstrt_base, hsstrt_wrong, hsstrt_correct;
    uint16_t hsstrt_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
        h_base.write_reg(A_HSSTOP, 200); // companion register, correct address
        hsstrt_base = next_hsync_rise_hpos(h_base);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
        h_wrong.write_reg(A_HSSTOP, 200);
        h_wrong.write_reg(A_HCENTER, 20); // wrong address
        hsstrt_wrong = next_hsync_rise_hpos(h_wrong);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
        h_correct.write_reg(A_HSSTOP, 200);
        h_correct.write_reg(A_HSSTRT, 20);
        hsstrt_correct = next_hsync_rise_hpos(h_correct);
        hsstrt_readback = h_correct.read(A_HSSTRT);

        bool wrong_matches_base = std::abs(hsstrt_wrong - hsstrt_base) <= 4;
        bool correct_differs = std::abs(hsstrt_correct - hsstrt_base) > 4;
        checks.expect(
            wrong_matches_base && correct_differs,
            "HSSTRT-ADDRESS-1DE: with ecs=1, BEAMCON0.VARBEAMEN=1 and "
            "VARHSYEN=1, and HSSTOP fixed at 200 (correct address), writing "
            "20 to $1E2 (HCENTER, the wrong address) leaves _hsync's rise "
            "at hpos=" + std::to_string(hsstrt_wrong) + " (baseline " +
            std::to_string(hsstrt_base) + "); writing 20 to $1DE (HSSTRT, "
            "the address the manual names) moves the rise to hpos=" +
            std::to_string(hsstrt_correct) + " -- 'HSSTRT        1DE   w   "
            "   A (E)   Horizontal sync start (VARHSY)' (p.293). "
            "B-hsstrt.addr .");
    }

    // ================= HSSTRT-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            hsstrt_readback == 0 && std::abs(hsstrt_correct - hsstrt_base) > 4,
            "HSSTRT-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1DE "
            "after writing 20 there returns 0, not the written value, even "
            "though the same write measurably moved _hsync's rise just "
            "above -- genuinely W not R/W; chip ownership structural "
            "(METHODOLOGY (2)) -- '   HSSTRT    W   A     Horizontal line "
            "position for HSYNC  start' (p.304). C-hsstrt.c01, "
            "C-scan.c15 .");
    }

    // ================= HCENTER-ADDRESS-1E2 =================
    // HCENTER only affects the vsync pulse's HORIZONTAL launch point, and
    // only on the interlaced field it applies to -- which of the two
    // field1 parities that is is not assumed, only measured: both are
    // tried and the assertion passes if EITHER shows the address-decode
    // effect, exactly as uncertain-mechanism assertions elsewhere in this
    // repo are handled (see hrm/ch3-beam-counters-and-blanking's own
    // INTERLACE-HALF-LINE-VERTICAL-OFFSET-VIA-VSYNC-TIMING).
    int hcenter_base0, hcenter_wrong0, hcenter_correct0;
    int hcenter_base1, hcenter_wrong1, hcenter_correct1;
    uint16_t hcenter_readback;
    {
        auto measure = [](bool field1_long_frame) {
            Harness h(/*ecs=*/true, /*ntsc=*/false);
            h.write_reg(A_BPLCON0, 1 << BP_LACE);
            force_vpos(h, 0, /*long_frame=*/field1_long_frame);
            h.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
            return next_vsync_rise_hpos(h);
        };
        hcenter_base0 = measure(false);
        hcenter_base1 = measure(true);

        auto measure_wrong = [](bool field1_long_frame) {
            Harness h(/*ecs=*/true, /*ntsc=*/false);
            h.write_reg(A_BPLCON0, 1 << BP_LACE);
            force_vpos(h, 0, /*long_frame=*/field1_long_frame);
            h.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
            h.write_reg(A_HSSTRT, 20); // wrong address
            return next_vsync_rise_hpos(h);
        };
        hcenter_wrong0 = measure_wrong(false);
        hcenter_wrong1 = measure_wrong(true);

        auto measure_correct = [](bool field1_long_frame) {
            Harness h(/*ecs=*/true, /*ntsc=*/false);
            h.write_reg(A_BPLCON0, 1 << BP_LACE);
            force_vpos(h, 0, /*long_frame=*/field1_long_frame);
            h.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARHSYEN));
            h.write_reg(A_HCENTER, 20);
            return next_vsync_rise_hpos(h);
        };
        hcenter_correct0 = measure_correct(false);
        hcenter_correct1 = measure_correct(true);

        Harness h_ro(/*ecs=*/true, /*ntsc=*/false);
        h_ro.write_reg(A_HCENTER, 20);
        hcenter_readback = h_ro.read(A_HCENTER);

        bool field0_selective = std::abs(hcenter_wrong0 - hcenter_base0) <= 4 &&
                                 std::abs(hcenter_correct0 - hcenter_base0) > 4;
        bool field1_selective = std::abs(hcenter_wrong1 - hcenter_base1) <= 4 &&
                                 std::abs(hcenter_correct1 - hcenter_base1) > 4;
        checks.expect(
            field0_selective || field1_selective,
            "HCENTER-ADDRESS-1E2: with LACE=1, ecs=1, BEAMCON0.VARBEAMEN=1 "
            "and VARHSYEN=1 (the same gate group as HSSTRT/HSSTOP -- see "
            "METHODOLOGY), and testing BOTH interlace field parities "
            "(long_frame false: base/wrong/correct hpos=" +
            std::to_string(hcenter_base0) + "/" + std::to_string(hcenter_wrong0) +
            "/" + std::to_string(hcenter_correct0) + "; long_frame true: " +
            std::to_string(hcenter_base1) + "/" + std::to_string(hcenter_wrong1) +
            "/" + std::to_string(hcenter_correct1) +
            "), writing 20 to $1DE (HSSTRT, the wrong address) leaves "
            "_vsync's horizontal launch point at its baseline on at least "
            "one field parity, while writing 20 to $1E2 (HCENTER, the "
            "address the manual names) moves it there -- 'HCENTER       "
            "1E2   w      A (E)   Horizontal position for Vsync on "
            "interlace' (p.293). B-hcenter.addr .");
    }

    // ================= HCENTER-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        bool field0_selective = std::abs(hcenter_wrong0 - hcenter_base0) <= 4 &&
                                 std::abs(hcenter_correct0 - hcenter_base0) > 4;
        bool field1_selective = std::abs(hcenter_wrong1 - hcenter_base1) <= 4 &&
                                 std::abs(hcenter_correct1 - hcenter_base1) > 4;
        checks.expect(
            hcenter_readback == 0 && (field0_selective || field1_selective),
            "HCENTER-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1E2 "
            "after writing 20 there returns 0, not the written value, even "
            "though the same write measurably moved _vsync's horizontal "
            "launch point on at least one field parity just above -- "
            "genuinely W not R/W; chip ownership structural (METHODOLOGY "
            "(2)) -- '   HCENTER   W   A     Horizontal position for Vsync "
            "on interlace' (p.304). C-hcenter.c01, C-scan.c17 .");
    }

    // ================= HBSTOP-ADDRESS-1C6 =================
    int hbstop_base, hbstop_wrong, hbstop_correct;
    uint16_t hbstop_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        hbstop_base = hblank_prefix_length(h_base);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        h_wrong.write_reg(A_VBSTOP, 3); // wrong address
        hbstop_wrong = hblank_prefix_length(h_wrong);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        h_correct.write_reg(A_HBSTOP, 3);
        hbstop_correct = hblank_prefix_length(h_correct);
        hbstop_readback = h_correct.read(A_HBSTOP);

        checks.expect(
            hbstop_wrong == hbstop_base && hbstop_correct != hbstop_base,
            "HBSTOP-ADDRESS-1C6: with ecs=1, BEAMCON0.VARBEAMEN=1 and "
            "HARDDIS=1 (this package's best-effort gating guess for the "
            "programmable blank-stop path -- see METHODOLOGY (4), to be "
            "confirmed once the RTL is opened), writing 3 to $1CE (VBSTOP, "
            "the wrong address) leaves hblank's blanked-prefix length at "
            "baseline (" + std::to_string(hbstop_base) + " CCK periods); "
            "writing 3 to $1C6 (HBSTOP, the address the manual names) "
            "changes it (" + std::to_string(hbstop_correct) +
            ") -- 'HBSTOP        1C6   W     A(E)     Horizontal line "
            "position for HBLANK stop' (p.293). B-hbstop.addr .");
    }

    // ================= HBSTOP-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            hbstop_readback == 0 && hbstop_correct != hbstop_base,
            "HBSTOP-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1C6 "
            "after writing 3 there returns 0, not the written value, even "
            "though the same write measurably changed hblank's "
            "blanked-prefix length just above -- genuinely W not R/W; chip "
            "ownership structural (METHODOLOGY (2)) -- '   HBSTOP    W   A "
            "    Horizontal line position for HBLANK stop' (p.304). "
            "C-hbstop.c01, C-scan.c19 .");
    }

    // ================= HBSTRT-ADDRESS-1C4 =================
    int hbstrt_base, hbstrt_wrong, hbstrt_correct;
    uint16_t hbstrt_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        h_base.write_reg(A_HBSTOP, 3); // companion register, correct address
        hbstrt_base = hblank_active_run_after_prefix(h_base);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        h_wrong.write_reg(A_HBSTOP, 3);
        h_wrong.write_reg(A_VTOTAL, 20); // wrong address
        hbstrt_wrong = hblank_active_run_after_prefix(h_wrong);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        h_correct.write_reg(A_HBSTOP, 3);
        h_correct.write_reg(A_HBSTRT, 20);
        hbstrt_correct = hblank_active_run_after_prefix(h_correct);
        hbstrt_readback = h_correct.read(A_HBSTRT);

        checks.expect(
            hbstrt_wrong == hbstrt_base && hbstrt_correct != hbstrt_base,
            "HBSTRT-ADDRESS-1C4: with the same best-effort gating as "
            "HBSTOP-ADDRESS-1C6 above and HBSTOP fixed at 3 (correct "
            "address), writing 20 to $1C8 (VTOTAL, the wrong address) "
            "leaves hblank's active-run length at baseline (" +
            std::to_string(hbstrt_base) + " CCK periods); writing 20 to "
            "$1C4 (HBSTRT, the address the manual names) changes it (" +
            std::to_string(hbstrt_correct) + ") -- 'HBSTRT        1C4   W  "
            "   A (E)    Horizontal line position for HBLANK start' "
            "(p.293). B-hbstrt.addr .");
    }

    // ================= HBSTRT-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            hbstrt_readback == 0 && hbstrt_correct != hbstrt_base,
            "HBSTRT-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1C4 "
            "after writing 20 there returns 0, not the written value, even "
            "though the same write measurably changed hblank's active-run "
            "length just above -- genuinely W not R/W; chip ownership "
            "structural (METHODOLOGY (2)) -- '   HBSTRT    W   A     "
            "Horizontal line position for HBLANK start' (p.304). "
            "C-hbstrt.c01, C-scan.c18 .");
    }

    // ================= VTOTAL-ADDRESS-1C8 =================
    // VTOTAL has no dedicated output port (unlike HTOTAL's htotal_out), so
    // the only available observable is field length via eol/eof, the same
    // signal hrm/ch3-beam-counters-and-blanking uses for the (different,
    // not-repeated-here) +1/+2 interlace-formula proposition. A small
    // VTOTAL value (3) is used purely to keep the address-decode
    // measurement fast, LACE stays 0 to avoid the interlace formula
    // entirely.
    int vtotal_base, vtotal_wrong, vtotal_correct;
    uint16_t vtotal_readback;
    {
        auto measure_field_length = [](Harness& h, int budget_ticks) {
            int cur = 0;
            bool prev_eol = false, prev_eof = false;
            for (int i = 0; i < budget_ticks; ++i) {
                h.dut->clk7_en = (i % 2 == 0);
                h.dut->cck = (i % 4 < 2);
                h.tick();
                bool eol_now = h.dut->eol;
                if (eol_now && !prev_eol) ++cur;
                prev_eol = eol_now;
                bool eof_now = h.dut->eof;
                if (eof_now && !prev_eof) return cur;
                prev_eof = eof_now;
            }
            return -1;
        };

        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        vtotal_base = measure_field_length(h_base, 800000);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        h_wrong.write_reg(A_VSSTOP, 3); // wrong address
        vtotal_wrong = measure_field_length(h_wrong, 800000);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, 1 << BC_VARBEAMEN);
        h_correct.write_reg(A_VTOTAL, 3);
        vtotal_correct = measure_field_length(h_correct, 800000);
        vtotal_readback = h_correct.read(A_VTOTAL);

        checks.expect(
            vtotal_wrong == vtotal_base && vtotal_correct != vtotal_base &&
            vtotal_correct != -1,
            "VTOTAL-ADDRESS-1C8: with ecs=1 and BEAMCON0.VARBEAMEN=1 "
            "(gates the programmable value into the field-length counter, "
            "same as HTOTAL -- METHODOLOGY), LACE=0, writing 3 to $1CA "
            "(VSSTOP, the wrong address) leaves the measured field length "
            "at baseline (" + std::to_string(vtotal_base) +
            " lines); writing 3 to $1C8 (VTOTAL, the address the manual "
            "names) changes it (" + std::to_string(vtotal_correct) +
            ") -- no claim is made here about the +1/+2 formula, only that "
            "this address, not a neighbour, controls field length -- "
            "'VTOTAL        1C8   W     A(E)     Highest numbered vertical "
            "line (VARBEAMEN=1)' (p.293). B-vtotal.addr .");
    }

    // ================= VTOTAL-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            vtotal_readback == 0 && vtotal_correct != vtotal_base,
            "VTOTAL-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1C8 "
            "after writing 3 there returns 0, not the written value, even "
            "though the same write measurably changed the field length "
            "just above -- genuinely W not R/W; chip ownership structural "
            "(METHODOLOGY (2)) -- '   VTOTAL    W   A     Highest numbered "
            "vertical line' (p.304). C-vtotal.c01 .");
    }

    // ================= VSSTOP-ADDRESS-1CA =================
    int vsstop_base, vsstop_wrong, vsstop_correct;
    uint16_t vsstop_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARVSYEN));
        h_base.write_reg(A_VSSTRT, 5); // companion register, correct address
        vsstop_base = next_vsync_fall_vpos(h_base);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARVSYEN));
        h_wrong.write_reg(A_VSSTRT, 5);
        h_wrong.write_reg(A_HBSTOP, 50); // wrong address
        vsstop_wrong = next_vsync_fall_vpos(h_wrong);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARVSYEN));
        h_correct.write_reg(A_VSSTRT, 5);
        h_correct.write_reg(A_VSSTOP, 50);
        vsstop_correct = next_vsync_fall_vpos(h_correct);
        vsstop_readback = h_correct.read(A_VSSTOP);

        bool wrong_matches_base = std::abs(vsstop_wrong - vsstop_base) <= 3;
        bool correct_differs = std::abs(vsstop_correct - vsstop_base) > 3;
        checks.expect(
            wrong_matches_base && correct_differs,
            "VSSTOP-ADDRESS-1CA: with ecs=1, BEAMCON0.VARBEAMEN=1 and "
            "VARVSYEN=1 (gates vsstop into the fall of _vsync -- "
            "METHODOLOGY), and VSSTRT fixed at 5 (correct address), "
            "writing 50 to $1C6 (HBSTOP, the wrong address) leaves "
            "_vsync's fall at vpos=" + std::to_string(vsstop_wrong) +
            " (baseline " + std::to_string(vsstop_base) +
            "); writing 50 to $1CA (VSSTOP, the address the manual names) "
            "moves the fall to vpos=" + std::to_string(vsstop_correct) +
            " -- 'VSSTOP        1CA   W     A (E)    Vertical line "
            "position for VSYNC stop' (p.293). B-vsstop.addr .");
    }

    // ================= VSSTOP-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            vsstop_readback == 0 && std::abs(vsstop_correct - vsstop_base) > 3,
            "VSSTOP-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1CA "
            "after writing 50 there returns 0, not the written value, even "
            "though the same write measurably moved _vsync's fall just "
            "above -- genuinely W not R/W; chip ownership structural "
            "(METHODOLOGY (2)) -- '   VSSTOP    W   A     Vertical line "
            "position for VSYNC  stop' (p.304). C-vsstop.c01, "
            "C-scan.c14 .");
    }

    // ================= VSSTRT-ADDRESS-1E0 =================
    int vsstrt_base, vsstrt_wrong, vsstrt_correct;
    uint16_t vsstrt_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARVSYEN));
        h_base.write_reg(A_VSSTOP, 50); // companion register, correct address
        vsstrt_base = next_vsync_rise_vpos(h_base);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARVSYEN));
        h_wrong.write_reg(A_VSSTOP, 50);
        h_wrong.write_reg(A_VBSTOP, 5); // wrong address
        vsstrt_wrong = next_vsync_rise_vpos(h_wrong);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_VARVSYEN));
        h_correct.write_reg(A_VSSTOP, 50);
        h_correct.write_reg(A_VSSTRT, 5);
        vsstrt_correct = next_vsync_rise_vpos(h_correct);
        vsstrt_readback = h_correct.read(A_VSSTRT);

        bool wrong_matches_base = std::abs(vsstrt_wrong - vsstrt_base) <= 3;
        bool correct_differs = std::abs(vsstrt_correct - vsstrt_base) > 3;
        checks.expect(
            wrong_matches_base && correct_differs,
            "VSSTRT-ADDRESS-1E0: with ecs=1, BEAMCON0.VARBEAMEN=1 and "
            "VARVSYEN=1, and VSSTOP fixed at 50 (correct address), writing "
            "5 to $1CE (VBSTOP, the wrong address) leaves _vsync's rise at "
            "vpos=" + std::to_string(vsstrt_wrong) + " (baseline " +
            std::to_string(vsstrt_base) + "); writing 5 to $1E0 (VSSTRT, "
            "the address the manual names) moves the rise to vpos=" +
            std::to_string(vsstrt_correct) + " -- 'VSSTRT        1E0   w   "
            "   A (E)   Vertical sync start   (VARVSY)' (p.293). "
            "B-vsstrt.addr .");
    }

    // ================= VSSTRT-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            vsstrt_readback == 0 && std::abs(vsstrt_correct - vsstrt_base) > 3,
            "VSSTRT-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1E0 "
            "after writing 5 there returns 0, not the written value, even "
            "though the same write measurably moved _vsync's rise just "
            "above -- genuinely W not R/W; chip ownership structural "
            "(METHODOLOGY (2)) -- '   VSSTRT    W   A     Vertical line "
            "position for VSYNC  start' (p.304). C-vsstrt.c01, "
            "C-scan.c13 .");
    }

    // ================= VBSTOP-ADDRESS-1CE =================
    int vbstop_base, vbstop_wrong, vbstop_correct;
    uint16_t vbstop_readback;
    {
        Harness h_base(/*ecs=*/true, /*ntsc=*/false);
        h_base.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        vbstop_base = vbl_blanked_lines(h_base);

        Harness h_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_wrong.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        h_wrong.write_reg(A_HBSTOP, 3); // wrong address
        vbstop_wrong = vbl_blanked_lines(h_wrong);

        Harness h_correct(/*ecs=*/true, /*ntsc=*/false);
        h_correct.write_reg(A_BEAMCON0, (1 << BC_VARBEAMEN) | (1 << BC_HARDDIS));
        h_correct.write_reg(A_VBSTOP, 3);
        vbstop_correct = vbl_blanked_lines(h_correct);
        vbstop_readback = h_correct.read(A_VBSTOP);

        checks.expect(
            vbstop_wrong == vbstop_base && vbstop_correct != vbstop_base,
            "VBSTOP-ADDRESS-1CE: with ecs=1, BEAMCON0.VARBEAMEN=1 and "
            "HARDDIS=1 (same best-effort gating guess as HBSTOP/HBSTRT -- "
            "METHODOLOGY (4)), writing 3 to $1C6 (HBSTOP, the wrong "
            "address) leaves vbl's blanked-line count at baseline (" +
            std::to_string(vbstop_base) + " lines); writing 3 to $1CE "
            "(VBSTOP, the address the manual names) changes it (" +
            std::to_string(vbstop_correct) + ") -- no claim is made here "
            "about the manual's specific blanking-duration numbers (that "
            "is hrm/ch3-beam-counters-and-blanking's job), only that this "
            "address, not a neighbour, controls vbl -- 'VBSTOP        1CE  "
            " W     A (E)    Vertical line for VBLANK stop' (p.293). "
            "B-vbstop.addr .");
    }

    // ================= VBSTOP-DIRECTION-WRITE-ONLY-AGNUS =================
    {
        checks.expect(
            vbstop_readback == 0 && vbstop_correct != vbstop_base,
            "VBSTOP-DIRECTION-WRITE-ONLY-AGNUS: reading data_out at $1CE "
            "after writing 3 there returns 0, not the written value, even "
            "though the same write measurably changed vbl's blanked-line "
            "count just above -- genuinely W not R/W; chip ownership "
            "structural (METHODOLOGY (2)) -- '   VBSTOP    W   A     "
            "Vertical   line position for VBLANK stop' (p.304). "
            "C-vbstop.c01, C-scan.c21 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
