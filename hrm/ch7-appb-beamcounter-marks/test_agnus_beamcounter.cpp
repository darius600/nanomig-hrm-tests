// Test purpose: Appendix B's per-register MARKS for the beam registers --
//   direction (R/W), chip column (Agnus) and the "(E)" enhanced-chip-set
//   mark -- for VPOSR $004, VHPOSR $006, VPOSW $02A, VHPOSW $02C and
//   BEAMCON0 $1DC. Package ch7-appb-beamcounter-marks, fixture
//   agnus_beamcounter (one Verilator build, this module only, independent
//   of every sibling package's own obj_dir).
//   ADDRESS is explicitly NOT this package's job: hrm/ch3-register-map-
//   agnus-beamcounter already asserts the address of all five registers
//   from these same printed rows (see its own out.txt, 26/0) and this
//   package cites that rather than duplicating it, per its own
//   SPECIFICATION.md and the claims file's `covered_by` fields.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed pages 263, 264,
//   288, 289, 293 (Appendix A preamble/legend, Appendix B legend and
//   register rows).
// Non-goals: address (see above), bit-field layouts, counting/blanking/
//   sync VALUES, and the Copper "*"/"~" write-protection mark. The last of
//   these was flagged as a possible gap in SPECIFICATION.md and confirmed
//   by the lead: it is hrm/ch7-appb-copper-write-protection's subject (its
//   own PROP-2/3/7/8 already assert VPOSR/VHPOSR/VPOSW/VHPOSW's "*" mark
//   from the same printed rows this file quotes) -- nothing is asserted
//   about it here.
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_beamcounter.v:27-57 module
//   header -- ports clk, clk7_en, reset, cck, ntsc, aga, ecs, a1k,
//   data_in[15:0], data_out[15:0], reg_address_in[8:1], hpos[8:0],
//   vpos[10:0], _hsync, _vsync, field1, lace, _csync, hblank, vblank, vbl,
//   vblend, eol, eof, vbl_int, htotal_out[8:0], harddis_out, varbeamen_out.
//   Harness mechanics (tick/run_cck/write_reg/read/force_vpos) are reused
//   verbatim from hrm/ch3-register-map-agnus-beamcounter/
//   test_agnus_beamcounter.cpp -- same module, ports and addresses already
//   public in that already-built sibling's own file, so reading it to reuse
//   the mechanics is not "opening the RTL" a second time (same precedent
//   that file itself cites). agnus_beamcounter.v was read directly by this
//   package only AFTER SPECIFICATION.md was committed (commit d189b7a).
//
// METHODOLOGY:
//
// (1) READ-ONLY (VPOSR $004, VHPOSR $006): tested by attempting a write at
//   the register's own address and confirming the value subsequently read
//   there (and at its sibling read address) is unchanged from an
//   identically-set-up harness that received no such write. This is
//   falsifiable in both directions: agnus_beamcounter.v's data_out mux
//   (l.108-115) and its vpos/long_frame/hpos write-always-blocks
//   (l.264-330) are two SEPARATE pieces of RTL, so a hidden write-decode at
//   $004/$006 would show up here even though nothing else in this file
//   depends on that non-existence.
//
// (2) WRITE-ONLY, direction (VPOSW $02A, VHPOSW $02C, BEAMCON0 $1DC): per
//   the lead's note on the committed spec, "write-only" is falsifiable in
//   one direction only -- a read returning the written/live content REFUTES
//   it (definite), but a read returning a constant like 0 does not CONFIRM
//   it (a written-but-unread register and an undecoded address both read
//   0). Every direction assertion below therefore also reads an address
//   this module never decodes at all under the same harness ($00E, Denise's
//   CLXDAT, or $01C, Paula's INTENAR -- confirmed by inspection: neither
//   name appears anywhere in agnus_beamcounter.v, so data_out's mux `else`
//   branch (l.114) is the only path either address can take), reported via
//   note(). For VPOSW/VHPOSW this MEASURED, per the claims file's own
//   evidence and reproduced here directly: reading $02A/$02C returns the
//   SAME word VPOSR/VHPOSR returns (agnus_beamcounter.v:109-112 decodes
//   both a register's R and W address into the identical mux arm) -- a
//   plain refutation, graded FAIL/red, not worked around. For BEAMCON0
//   the read is indistinguishable from the undecoded baseline (both 0), so
//   that half is declined via record(), not graded green.
//
// (3) CHIP OWNERSHIP is established structurally: this Makefile's
//   TOP_MODULE is agnus_beamcounter alone and SRC compiles exactly
//   nanomig/src/minimig-aga/agnus_beamcounter.v -- no Denise, Paula, CPU or
//   other custom-chip source is anywhere in this Verilator build (reused
//   verbatim from the sibling package's own METHODOLOGY (2), same module,
//   same reasoning). Each CHIP assertion below shows the target address
//   produces an effect distinguishable from an address outside this
//   package's five rows, under that same structural reasoning.
//
// (4) THE "(E)" MARK is two different RTL mechanisms, matching the
//   legend's own two classes (Appendix A/B p.264/288, cited jointly, not
//   scored on their own -- see SPECIFICATION.md): VPOSR's mark is
//   "extended" -- agnus_beamcounter.v:110's data_out expression includes
//   `ecs` directly as bit 13, so the SAME address returns one extra bit of
//   content depending on chipset. BEAMCON0's mark is "completely new" --
//   its own dedicated write-always-block (l.117-125) is gated
//   `&& ecs`, so the register does not merely behave differently under OCS,
//   it does not accept writes at all. The three unmarked registers
//   (VHPOSR, VPOSW, VHPOSW) are this file's control cases: their own mux
//   arm/always-blocks carry no `ecs` term anywhere, so their behaviour must
//   be identical at ecs=0 and ecs=1 -- BUILD-BRIEF's required "control case
//   on the same measurement basis."
//
// (5) PRECONDITION: BEAMCON0's write-only proposition is only meaningful at
//   ecs=1 (METHODOLOGY (2) above). Rather than relying on the harness
//   constructor's `ecs=true` argument as an assumed default, its own
//   BEAMCON0 assertion is preceded by an explicit PRECONDITION expect()
//   that reads VPOSR's own ecs-identification bit (bit 13, per (4)) back
//   out of the DUT and confirms it reads 1 -- i.e. it confirms ecs=1
//   actually reached the DUT's internal logic, not merely that the
//   testbench's own input mirror was set that way.
//
// CLAIM COVERAGE (all 24 claims in ch7-appb-beamcounter-marks-spec-
//   stage.jsonl / ch7-appb-beamcounter-marks.jsonl, per SPECIFICATION.md):
//   - LEGEND-A (sol:7-appA-legend.c5, twin opus:7-appA-pre.c15), LEGEND-A2
//     (sol:7-appA-legend.c6, twin opus:7-appA-pre.c17), LEGEND-B
//     (sol:7-appB-legend.c8, twin opus:7-appB.c11) and UNIVERSAL-EXCLUSIVITY
//     (opus:7-appA-pre.c10) name no single register; cited in this comment
//     block and in the assertions they jointly support, not scored on their
//     own -- exactly as SPECIFICATION.md commits to.
//   - 5 DIRECTION assertions (sol:7-appB-{vposr,vhposr,vposw,vhposw,
//     beamcon0}.c2, each confirmed by its bundled opus twin .c15/.c17/.c25/
//     .c27/.c35).
//   - 5 CHIP assertions (sol:...c3, same confirming twins).
//   - 2 "(E)"-mark assertions (sol:7-appB-vposr.c6, sol:7-appB-beamcon0.c5).
//   - 3 control assertions, evidenced by the same sol:...c2/.c3 pairs for
//     VHPOSR/VPOSW/VHPOSW (their own row's absence of "(E)").
//   No claim id is declined; every one is cited on an expect() label.
//
// CONTROL CASES: every DIRECTION/CHIP assertion carries its own internal
//   control (a "wrong address" harness expected to match baseline -- see
//   (1)/(2)/(3)). The three "(E)"-absence registers (VHPOSR, VPOSW, VHPOSW)
//   are the control basis for the two "(E)"-mark assertions, required
//   because an ECS-mark assertion with no register that must show NO
//   ECS-dependence cannot tell a real mark from a fixture that ignores the
//   `ecs` signal entirely. FIXTURE-DELIVERS-KNOWN-VALUE additionally proves
//   the write-then-read pipe is alive on this package's own independently
//   built obj_dir before anything else relies on it.
//
// INDEPENDENCE, per the lead's question ("how many propositions are
//   independent and how many are one proposition observed several times"),
//   reproducing the step-3 notes' own finding about the Copper "*"/"~" mark
//   (agnus_copper.v:225 -- read directly, not adopted from the notes:
//   `if (ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs)` -- one pair
//   of address-RANGE comparisons on the top address bits, gating a whole
//   block of addresses at once, not a per-register decode) and applying the
//   same question to this package's own 15 propositions:
//   - The 5 CHIP assertions are 5 manual propositions but ONE independent
//     RTL fact observed 5 times: "chip = Agnus" here reduces entirely to
//     METHODOLOGY (3)'s structural argument (this build's only source is
//     agnus_beamcounter.v), which does not vary per register -- it is
//     established once and cited five times, same shape as the Copper
//     mark's address-range gate covering many rows from one conditional.
//   - VPOSR/VPOSW's DIRECTION assertions (and, separately, VHPOSR/VHPOSW's)
//     are NOT independent measurements of each other, even though they are
//     independent manual propositions about different printed rows: both
//     pairs are read out of the SAME shared mux arm in agnus_beamcounter.v
//     (l.109-112, one `if` per pair) -- a change to that one arm moves both
//     assertions in a pair together, which is exactly why VPOSR reads
//     correctly (read-only holds) while VPOSW is simultaneously readable
//     (write-only is violated): one shared mechanism, two different
//     manual-vs-RTL outcomes.
//   - BEAMCON0's DIRECTION assertion and its "(E)"-mark assertion likewise
//     share one mechanism (the single `&& ecs` write-gate, l.121/172) --
//     independent PROPOSITIONS, not independent RTL facts; a change to that
//     one gate moves both together (see the PER-ASSERTION MUTATION RECORDS
//     below).
//   - The two "(E)"-mark assertions (VPOSR "extended" vs BEAMCON0
//     "completely new") ARE independent RTL facts: one is a bit inside a
//     shared read mux, the other is a whole write-always-block's own gate
//     condition -- different code, different mechanism, matching the
//     legend's own two distinct classes.
//   - The 3 control assertions are 3 independent RTL facts: VHPOSR's own
//     mux arm (l.111-112) carries no `ecs` term at all (independent of
//     VPOSR/VPOSW's arm, which does); VPOSW's and VHPOSW's own write-
//     always-blocks (l.327-328, l.264-265/329-330) are two further,
//     separately-coded blocks that also carry no `ecs` term -- three
//     separate pieces of code, not one.
//   Net count: 15 manual propositions, asserted as 15 expect() calls (plus
//   1 precondition and 1 fixture-liveness check) below, resting on
//   noticeably fewer independent RTL facts -- roughly 6 (one shared
//   VPOSR/VPOSW arm, one shared VHPOSR/VHPOSW arm, one BEAMCON0 write gate,
//   one module-boundary structural fact reused 5x, plus VPOSW's and
//   VHPOSW's own write blocks) -- reported here rather than left implicit.
//
// PER-ASSERTION MUTATION RECORDS (every entry below was actually run by this agent under
//   WSL/Verilator: edit -> rebuild -> run -> confirm the named assertion(s)
//   flip -> revert -> rebuild -> confirm the full suite back to its
//   committed baseline (15 passed, 2 failed) -- before moving to the next
//   one. No oracle text or expected-value literal was ever touched;
//   nanomig/ was never touched. Baseline before this log: 15 passed
//   (2 expected, documented reds: VPOSW-DIRECTION-WRITE-ONLY,
//   VHPOSW-DIRECTION-WRITE-ONLY).
//
// MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: force_vpos's vpos11
//   argument changed from 0x2AA to 0x000 -> 14 passed, 3 failed; only
//   FIXTURE-DELIVERS-KNOWN-VALUE newly red (bits2:0 now read 0b000, not
//   0b010). Reverted: 15/2.
// MUTATION VPOSR-DIRECTION-READ-ONLY stimulus red: the disruptive write's
//   target changed from A_VPOSR to A_VPOSW (a real write path) -> 14/3;
//   only VPOSR-DIRECTION-READ-ONLY newly red (VPOSR's readback changed
//   0xA001 -> 0x2004). Reverted: 15/2.
// MUTATION VPOSR-CHIP-AGNUS stimulus red: the "undecoded" comparison
//   target changed from A_UNDECODED_CLXDAT to A_VPOSR (i.e. compared VPOSR
//   against itself) -> 14/3; only VPOSR-CHIP-AGNUS newly red (0xA001 no
//   longer distinguishable from "undecoded" 0xA001). Reverted: 15/2.
// MUTATION VPOSR-ECS-MARK-EXTENDED stimulus red: both ecs=1/ecs=0 read
//   targets changed from A_VPOSR to A_VHPOSR (the unmarked control
//   register) -> 14/3; only VPOSR-ECS-MARK-EXTENDED newly red (bit 13 no
//   longer differs: 0x0000 both times). Reverted: 15/2.
// MUTATION VHPOSR-DIRECTION-READ-ONLY stimulus red: the disruptive write's
//   target changed from A_VHPOSR to A_VHPOSW (a real write path) -> 14/3;
//   only VHPOSR-DIRECTION-READ-ONLY newly red (0xAA00 -> 0x9999).
//   Reverted: 15/2.
// MUTATION VHPOSR-CHIP-AGNUS stimulus red: the "undecoded" comparison
//   target changed from A_UNDECODED_INTENAR to A_VHPOSR -> 14/3; only
//   VHPOSR-CHIP-AGNUS newly red. Reverted: 15/2.
// MUTATION VHPOSR-NO-ECS-DEPENDENCE stimulus red: both ecs=1/ecs=0 read
//   targets changed from A_VHPOSR to A_VPOSR (the marked register) ->
//   14/3; only VHPOSR-NO-ECS-DEPENDENCE newly red (0x2002 vs 0x0002, now
//   DOES differ by ecs). Reverted: 15/2.
// MUTATION VPOSW-DIRECTION-WRITE-ONLY stimulus green: this assertion is
//   RED at baseline (a documented finding -- see METHODOLOGY (2)), so a red
//   cannot be made to fail harder; its mutation instead runs the other way,
//   turning it green, to prove the check is live rather than stuck. The
//   read target changed from A_VPOSW to A_UNDECODED_CLXDAT (i.e. read the
//   reference address instead of the address under test) -> 16 passed,
//   1 failed; VPOSW-DIRECTION-WRITE-ONLY flips FAIL->PASS (now trivially
//   compares the undecoded address to itself), VHPOSW-DIRECTION-WRITE-ONLY
//   stays red. This shows the FAIL above is a real address-decode-aliasing
//   finding, not a harness artifact that reports FAIL regardless of which
//   address is read. Reverted: 15/2.
// MUTATION VPOSW-CHIP-AGNUS stimulus red: the "correct address" write
//   target changed from A_VPOSW to A_UNDECODED_CLXDAT -> 14/3; only
//   VPOSW-CHIP-AGNUS newly red (0xA000 no longer differs from baseline).
//   Reverted: 15/2.
// MUTATION VPOSW-NO-ECS-DEPENDENCE stimulus red: the ecs=0 harness's
//   written value changed from 0x8005 to 0x8006 -> 14/3; only
//   VPOSW-NO-ECS-DEPENDENCE newly red (masked readback now 0x8005 vs
//   0x8006). Reverted: 15/2.
// MUTATION VHPOSW-DIRECTION-WRITE-ONLY stimulus green: symmetric to
//   VPOSW-DIRECTION-WRITE-ONLY above -- this assertion is also RED at
//   baseline (documented finding), so its mutation likewise runs
//   red-to-green to prove the check is live rather than stuck: the read
//   target changed from A_VHPOSW to A_UNDECODED_INTENAR -> 16 passed,
//   1 failed; VHPOSW-DIRECTION-WRITE-ONLY flips FAIL->PASS,
//   VPOSW-DIRECTION-WRITE-ONLY stays red. Reverted: 15/2.
// MUTATION VHPOSW-CHIP-AGNUS stimulus red: the "correct address" write
//   target changed from A_VHPOSW to A_UNDECODED_INTENAR -> 14/3; only
//   VHPOSW-CHIP-AGNUS newly red. Reverted: 15/2.
// MUTATION VHPOSW-NO-ECS-DEPENDENCE stimulus red: the ecs=0 harness's
//   written value changed from 0x3344 to 0x3355 -> 14/3; only
//   VHPOSW-NO-ECS-DEPENDENCE newly red. Reverted: 15/2.
// MUTATION PRECONDITION-BEAMCON0-EFFECTIVE-ECS1 stimulus red: the
//   BEAMCON0-block harness's ecs argument changed from true to false ->
//   13 passed, 4 failed; PRECONDITION-BEAMCON0-EFFECTIVE-ECS1 newly red
//   (bit 13 now reads 0) AND BEAMCON0-DIRECTION-WRITE-ONLY also newly red,
//   as expected collateral (the write no longer takes effect once ecs=0)
//   -- confirms the precondition is genuinely load-bearing, not
//   decorative. Reverted: 15/2.
// MUTATION BEAMCON0-DIRECTION-WRITE-ONLY stimulus red: the write's target
//   changed from A_BEAMCON0 to A_UNDECODED_CLXDAT -> 14/3; only
//   BEAMCON0-DIRECTION-WRITE-ONLY newly red (harddis_out/varbeamen_out
//   stay false). Reverted: 15/2. Note this assertion is ALSO moved, as
//   expected collateral, by the separate PRECONDITION-BEAMCON0-EFFECTIVE-
//   ECS1 mutation above (forcing ecs=0 on the same harness makes this
//   assertion red too, since the write no longer takes effect) -- two
//   different stimulus changes each independently flip this one id.
// MUTATION BEAMCON0-CHIP-AGNUS-ENHANCED stimulus red: the "correct
//   address" write target changed from A_BEAMCON0 to A_UNDECODED_CLXDAT ->
//   14/3; only BEAMCON0-CHIP-AGNUS-ENHANCED newly red. Reverted: 15/2.
// MUTATION BEAMCON0-ECS-MARK-NEW stimulus red: the ecs=0 harness's ecs
//   argument changed from false to true (both harnesses now ecs=1) ->
//   14/3; only BEAMCON0-ECS-MARK-NEW newly red (ecs=0 case no longer stays
//   at baseline). Reverted: 15/2.
//
// Final rebuild after the last revert: 15 passed, 2 failed, confirmed
//   again immediately before this comment was written; every code line
//   outside this header comment is unchanged from the pre-mutation
//   version (each revert's own sed output was checked against the
//   original text before moving to the next mutation).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vagnus_beamcounter.h"

namespace {

// The fixed assertion form, verbatim from BUILD-BRIEF.md.
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

std::string hex16(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%04X", v);
    return std::string(buf);
}

// Register addresses, as reg_address_in values (port carries address bits
// [8:1]; a custom-chip byte address is always even, so the port value is
// the byte address shifted right by one). Reused verbatim from the sibling
// package's own file (same module, same claims-file evidence table).
constexpr int A_VPOSR    = 0x004 >> 1;
constexpr int A_VPOSW    = 0x02A >> 1;
constexpr int A_VHPOSR   = 0x006 >> 1;
constexpr int A_VHPOSW   = 0x02C >> 1;
constexpr int A_BEAMCON0 = 0x1DC >> 1;
// Addresses this module never decodes at all -- Denise's CLXDAT and
// Paula's INTENAR -- used as the "known undecoded" reference baseline
// (METHODOLOGY (2)/(3)).
constexpr int A_UNDECODED_CLXDAT  = 0x00E >> 1;
constexpr int A_UNDECODED_INTENAR = 0x01C >> 1;

// BEAMCON0 bit numbers (appendix C p.305; bit numbers confirmed directly
// from agnus_beamcounter.v:128-135).
constexpr int BC_HARDDIS   = 14;
constexpr int BC_VARBEAMEN = 7;

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

    // VPOSR/VHPOSR/VPOSW/VHPOSW readback: combinational mux, no clock edge
    // needed (agnus_beamcounter.v:108-115).
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

// Writes VHPOSW then VPOSW (order matters: VPOSW's write also sets
// long_frame, and both share the vpos_inc-driven update in the same
// always-block region -- see agnus_beamcounter.v:322-337). Reused verbatim
// from the sibling package's own file (identical module).
void force_vpos(Harness& h, uint32_t vpos11, bool long_frame, uint8_t hpos8_1 = 0) {
    uint16_t vhposw_data = static_cast<uint16_t>(((vpos11 & 0xFF) << 8) | hpos8_1);
    h.write_reg(A_VHPOSW, vhposw_data);
    uint16_t vposw_data = static_cast<uint16_t>((long_frame ? 0x8000 : 0x0000) |
                                                 ((vpos11 >> 8) & 0x7));
    h.write_reg(A_VPOSW, vposw_data);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= FIXTURE-DELIVERS-KNOWN-VALUE (no page) =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, /*vpos11=*/0x2AA, /*long_frame=*/true, /*hpos8_1=*/0x5A);
        uint16_t vposr = h.read(A_VPOSR);
        uint16_t vhposr = h.read(A_VHPOSR);
        checks.expect(
            ((vposr >> 15) & 1) == 1 && (vposr & 0x7) == 0x2 &&
            ((vhposr >> 8) & 0xFF) == 0xAA,
            "FIXTURE-DELIVERS-KNOWN-VALUE: a vpos[10:8]=0b010 / long_frame=1 "
            "/ hpos8_1=0x5A pattern written through VHPOSW then VPOSW and "
            "read back through VPOSR (bit15=1, bits2:0=0b010) and VHPOSR "
            "(bits15:8=0xAA) arrives distinctly and correctly -- this "
            "PACKAGE's own independently built obj_dir actually delivers a "
            "known, distinct value through the write-then-read path before "
            "any mark assertion below depends on it.");
    }

    // ============================================================
    // VPOSR $004 -- direction, chip, "(E)" mark
    // ============================================================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, /*vpos11=*/0x155, /*long_frame=*/true, /*hpos8_1=*/0x77);
        uint16_t vposr_before  = h.read(A_VPOSR);
        uint16_t vhposr_before = h.read(A_VHPOSR);
        uint16_t undecoded     = h.read(A_UNDECODED_CLXDAT);
        h.write_reg(A_VPOSR, 0x1234);  // attempted write to the read-only address
        uint16_t vposr_after  = h.read(A_VPOSR);
        uint16_t vhposr_after = h.read(A_VHPOSR);

        checks.note(
            "VPOSR reference: an address this module never decodes at all "
            "($00E, Denise's CLXDAT) reads " + hex16(undecoded) +
            " under the same harness.");
        checks.expect(
            vposr_after == vposr_before && vhposr_after == vhposr_before,
            "VPOSR-DIRECTION-READ-ONLY: writing 0x1234 to $004 leaves the "
            "value subsequently read there (" + hex16(vposr_before) + " -> " +
            hex16(vposr_after) + ") and at $006 (" + hex16(vhposr_before) +
            " -> " + hex16(vhposr_after) + ") unchanged -- no write path "
            "reaches $004 -- 'VPOSR        *004   R     A (E)      Read "
            "vert most signif. bit (and frame flop)' (p.289). "
            "sol:7-appB-vposr.c2, confirmed by opus:7-appB.c15. Also "
            "instantiates opus:7-appA-pre.c10 ('Registers are either "
            "read-only or write-only', p.263): the read-only half holds "
            "here.");
        checks.expect(
            vposr_before != undecoded,
            "VPOSR-CHIP-AGNUS: $004 answers with live content (" +
            hex16(vposr_before) + ") distinguishable from an address this "
            "module never decodes (" + hex16(undecoded) + "); this "
            "Verilator build's only source is agnus_beamcounter.v, so that "
            "effect is necessarily Agnus's own (METHODOLOGY (3)) -- "
            "'VPOSR        *004   R     A (E) ...' (p.289). "
            "sol:7-appB-vposr.c3, confirmed by opus:7-appB.c15.");

        Harness h_e1(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h_e1, /*vpos11=*/0x000, /*long_frame=*/false, /*hpos8_1=*/0x00);
        uint16_t v_ecs1 = h_e1.read(A_VPOSR);
        Harness h_e0(/*ecs=*/false, /*ntsc=*/false);
        force_vpos(h_e0, /*vpos11=*/0x000, /*long_frame=*/false, /*hpos8_1=*/0x00);
        uint16_t v_ecs0 = h_e0.read(A_VPOSR);
        bool ecs_bit_differs = ((v_ecs1 >> 13) & 1) != ((v_ecs0 >> 13) & 1);
        bool rest_identical = (v_ecs1 & 0xDFFF) == (v_ecs0 & 0xDFFF);
        checks.expect(
            ecs_bit_differs && rest_identical,
            "VPOSR-ECS-MARK-EXTENDED: with an identical vpos/long_frame/"
            "hpos pattern, VPOSR's readback differs from ecs=0 (" +
            hex16(v_ecs0) + ") to ecs=1 (" + hex16(v_ecs1) +
            ") ONLY in bit 13 -- the same address, one added bit, matching "
            "the legend's 'extended' class (Appendix A p.264 / Appendix B "
            "p.288, cited jointly, not scored on their own) -- 'VPOSR "
            "        *004   R     A (E)      Read vert most signif. bit "
            "(and frame flop)' (p.289), extension per Appendix C p.299. "
            "sol:7-appB-vposr.c6.");
    }

    // ============================================================
    // VHPOSR $006 -- direction, chip, control (no "(E)" mark)
    // ============================================================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, /*vpos11=*/0x2AA, /*long_frame=*/false, /*hpos8_1=*/0x00);
        uint16_t vhposr_before = h.read(A_VHPOSR);
        uint16_t vposr_before  = h.read(A_VPOSR);
        uint16_t undecoded     = h.read(A_UNDECODED_INTENAR);
        h.write_reg(A_VHPOSR, 0x9999);  // attempted write to the read-only address
        uint16_t vhposr_after = h.read(A_VHPOSR);
        uint16_t vposr_after  = h.read(A_VPOSR);

        checks.note(
            "VHPOSR reference: an address this module never decodes at all "
            "($01C, Paula's INTENAR) reads " + hex16(undecoded) +
            " under the same harness.");
        checks.expect(
            vhposr_after == vhposr_before && vposr_after == vposr_before,
            "VHPOSR-DIRECTION-READ-ONLY: writing 0x9999 to $006 leaves the "
            "value subsequently read there (" + hex16(vhposr_before) +
            " -> " + hex16(vhposr_after) + ") and at $004 (" +
            hex16(vposr_before) + " -> " + hex16(vposr_after) +
            ") unchanged -- no write path reaches $006 -- 'VHPOSR       "
            "*006   R     A          Read vert and horiz. position of "
            "beam' (p.289). sol:7-appB-vhposr.c2, confirmed by "
            "opus:7-appB.c17. Also instantiates opus:7-appA-pre.c10 "
            "(p.263): the read-only half holds here.");
        checks.expect(
            vhposr_before != undecoded,
            "VHPOSR-CHIP-AGNUS: $006 answers with live content (" +
            hex16(vhposr_before) + ") distinguishable from an address this "
            "module never decodes (" + hex16(undecoded) +
            "); structural (METHODOLOGY (3)) -- 'VHPOSR       *006   R     "
            "A          Read vert and horiz. position of beam' (p.289). "
            "sol:7-appB-vhposr.c3, confirmed by opus:7-appB.c17.");

        Harness h_c1(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h_c1, /*vpos11=*/0x2AA, /*long_frame=*/false, /*hpos8_1=*/0x00);
        uint16_t vhr_ecs1 = h_c1.read(A_VHPOSR);
        Harness h_c0(/*ecs=*/false, /*ntsc=*/false);
        force_vpos(h_c0, /*vpos11=*/0x2AA, /*long_frame=*/false, /*hpos8_1=*/0x00);
        uint16_t vhr_ecs0 = h_c0.read(A_VHPOSR);
        checks.expect(
            vhr_ecs1 == vhr_ecs0,
            "VHPOSR-NO-ECS-DEPENDENCE (control): the identical pattern "
            "reads back identically at ecs=1 (" + hex16(vhr_ecs1) +
            ") and ecs=0 (" + hex16(vhr_ecs0) +
            ") -- this row carries no '(E)' mark ('VHPOSR       *006   R   "
            "  A          Read vert and horiz. position of beam', p.289), "
            "and its own mux arm (agnus_beamcounter.v:111-112) carries no "
            "`ecs` term at all. This is the control basis, on the same "
            "measurement mechanism, for VPOSR-ECS-MARK-EXTENDED above. "
            "sol:7-appB-vhposr.c2, sol:7-appB-vhposr.c3.");
    }

    // ============================================================
    // VPOSW $02A -- direction (EXPECTED RED, see METHODOLOGY (2)), chip,
    // control (no "(E)" mark)
    // ============================================================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_VPOSW, 0x8005);  // long_frame=1, vpos[10:8]=5: distinct, nonzero
        uint16_t vposw_read  = h.read(A_VPOSW);
        uint16_t vposr_alias = h.read(A_VPOSR);
        uint16_t undecoded   = h.read(A_UNDECODED_CLXDAT);

        checks.note(
            "VPOSW reference: an address this module never decodes at all "
            "($00E, Denise's CLXDAT) reads " + hex16(undecoded) +
            " under the same harness.");
        checks.expect(
            vposw_read == undecoded,
            "VPOSW-DIRECTION-WRITE-ONLY: reading $02A after writing a "
            "distinct pattern there should be indistinguishable from an "
            "address this module never decodes (" + hex16(undecoded) +
            ") if $02A is genuinely write-only. MEASURED instead: $02A "
            "reads " + hex16(vposw_read) + ", equal to VPOSR's own live "
            "readback at $004 (" + hex16(vposr_alias) +
            ") in the same harness -- agnus_beamcounter.v:109-112 decodes "
            "$004 and $02A into the identical data_out mux arm, so this "
            "address is readable, contradicting 'VPOSW        *02A   W    "
            " A          Write vert most signif. bit (and frame flop)' "
            "(p.289). This is the counter-example opus:7-appA-pre.c10 "
            "(p.263, 'Registers are either read-only or write-only') is "
            "cited for in this package. sol:7-appB-vposw.c2, confirmed by "
            "opus:7-appB.c25.");

        Harness h_c3_base(/*ecs=*/true, /*ntsc=*/false);
        uint16_t c3_base = h_c3_base.read(A_VPOSR);
        Harness h_c3_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_c3_wrong.write_reg(A_UNDECODED_CLXDAT, 0x8005);
        uint16_t c3_wrong = h_c3_wrong.read(A_VPOSR);
        Harness h_c3_correct(/*ecs=*/true, /*ntsc=*/false);
        h_c3_correct.write_reg(A_VPOSW, 0x8005);
        uint16_t c3_correct = h_c3_correct.read(A_VPOSR);
        checks.expect(
            c3_wrong == c3_base && c3_correct != c3_base,
            "VPOSW-CHIP-AGNUS: writing 0x8005 to $00E (an address outside "
            "this claims file's five rows) leaves VPOSR's readback "
            "unchanged (" + hex16(c3_base) + " both times); writing the "
            "identical pattern to $02A changes it (" + hex16(c3_correct) +
            ") -- structural (METHODOLOGY (3)) -- 'VPOSW        *02A   W   "
            "  A          Write vert most signif. bit (and frame flop)' "
            "(p.289). sol:7-appB-vposw.c3, confirmed by opus:7-appB.c25.");

        Harness h_ctrl2_e1(/*ecs=*/true, /*ntsc=*/false);
        h_ctrl2_e1.write_reg(A_VPOSW, 0x8005);
        uint16_t r_e1 = h_ctrl2_e1.read(A_VPOSR);
        Harness h_ctrl2_e0(/*ecs=*/false, /*ntsc=*/false);
        h_ctrl2_e0.write_reg(A_VPOSW, 0x8005);
        uint16_t r_e0 = h_ctrl2_e0.read(A_VPOSR);
        // Mask to bit15 (long_frame) and bits2:0 (vpos[10:8]) -- the bits
        // this write actually controls. Bit13 is deliberately EXCLUDED: it
        // mirrors the live `ecs` input itself (VPOSR-ECS-MARK-EXTENDED
        // above), not anything VPOSW's own write controls, and comparing
        // it here would conflate the read-alias's known ECS-dependence
        // with the write-side behaviour this control case is about.
        constexpr uint16_t VPOS_WRITE_MASK = 0x8007;
        checks.expect(
            (r_e1 & VPOS_WRITE_MASK) == (r_e0 & VPOS_WRITE_MASK),
            "VPOSW-NO-ECS-DEPENDENCE (control): the identical write's "
            "effect on long_frame/vpos[10:8] (mask 0x8007) is identical "
            "whether ecs=1 (" + hex16(r_e1 & VPOS_WRITE_MASK) +
            ") or ecs=0 (" + hex16(r_e0 & VPOS_WRITE_MASK) +
            ") -- this row carries no '(E)' mark and its own write-"
            "always-block (agnus_beamcounter.v:327-328, 345-346) carries "
            "no `ecs` term. Control basis, on the same write/readback "
            "mechanism, for BEAMCON0-ECS-MARK-NEW below. "
            "sol:7-appB-vposw.c2, sol:7-appB-vposw.c3.");
    }

    // ============================================================
    // VHPOSW $02C -- direction (EXPECTED RED, see METHODOLOGY (2)), chip,
    // control (no "(E)" mark)
    // ============================================================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        h.write_reg(A_VHPOSW, 0x3344);
        uint16_t vhposw_read  = h.read(A_VHPOSW);
        uint16_t vhposr_alias = h.read(A_VHPOSR);
        uint16_t undecoded    = h.read(A_UNDECODED_INTENAR);

        checks.note(
            "VHPOSW reference: an address this module never decodes at "
            "all ($01C, Paula's INTENAR) reads " + hex16(undecoded) +
            " under the same harness.");
        checks.expect(
            vhposw_read == undecoded,
            "VHPOSW-DIRECTION-WRITE-ONLY: reading $02C after writing a "
            "distinct pattern there should be indistinguishable from an "
            "address this module never decodes (" + hex16(undecoded) +
            ") if $02C is genuinely write-only. MEASURED instead: $02C "
            "reads " + hex16(vhposw_read) + ", equal to VHPOSR's own live "
            "readback at $006 (" + hex16(vhposr_alias) +
            ") in the same harness -- agnus_beamcounter.v:111-112 decodes "
            "$006 and $02C into the identical data_out mux arm, so this "
            "address is readable, contradicting 'VHPOSW       *02C   W    "
            " A          Write vert and horiz position of beam' (p.289). "
            "Second instance of opus:7-appA-pre.c10's (p.263) "
            "counter-example. sol:7-appB-vhposw.c2, confirmed by "
            "opus:7-appB.c27.");

        Harness h_c4_base(/*ecs=*/true, /*ntsc=*/false);
        uint16_t c4_base = h_c4_base.read(A_VHPOSR);
        Harness h_c4_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_c4_wrong.write_reg(A_UNDECODED_INTENAR, 0x3344);
        uint16_t c4_wrong = h_c4_wrong.read(A_VHPOSR);
        Harness h_c4_correct(/*ecs=*/true, /*ntsc=*/false);
        h_c4_correct.write_reg(A_VHPOSW, 0x3344);
        uint16_t c4_correct = h_c4_correct.read(A_VHPOSR);
        checks.expect(
            c4_wrong == c4_base && c4_correct != c4_base,
            "VHPOSW-CHIP-AGNUS: writing 0x3344 to $01C (an address outside "
            "this claims file's five rows) leaves VHPOSR's readback "
            "unchanged (" + hex16(c4_base) + " both times); writing the "
            "identical pattern to $02C changes it (" + hex16(c4_correct) +
            ") -- structural (METHODOLOGY (3)) -- 'VHPOSW       *02C   W   "
            "  A          Write vert and horiz position of beam' (p.289). "
            "sol:7-appB-vhposw.c3, confirmed by opus:7-appB.c27.");

        Harness h_ctrl3_e1(/*ecs=*/true, /*ntsc=*/false);
        h_ctrl3_e1.write_reg(A_VHPOSW, 0x3344);
        uint16_t r3_e1 = h_ctrl3_e1.read(A_VHPOSR);
        Harness h_ctrl3_e0(/*ecs=*/false, /*ntsc=*/false);
        h_ctrl3_e0.write_reg(A_VHPOSW, 0x3344);
        uint16_t r3_e0 = h_ctrl3_e0.read(A_VHPOSR);
        checks.expect(
            r3_e1 == r3_e0,
            "VHPOSW-NO-ECS-DEPENDENCE (control): the identical write's "
            "effect (readback via VHPOSR, whose own mux arm carries no "
            "`ecs` term at all) is identical whether ecs=1 (" +
            hex16(r3_e1) + ") or ecs=0 (" + hex16(r3_e0) +
            ") -- this row carries no '(E)' mark. sol:7-appB-vhposw.c2, "
            "sol:7-appB-vhposw.c3.");
    }

    // ============================================================
    // BEAMCON0 $1DC -- precondition, direction, chip, "(E)" mark
    // ============================================================
    {
        // PRECONDITION: confirm ecs=1 actually reached the DUT's internal
        // logic (via VPOSR's own bit 13, per VPOSR-ECS-MARK-EXTENDED
        // above) rather than assuming the testbench's `ecs=true`
        // constructor argument took effect.
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        uint16_t vposr_precheck = h.read(A_VPOSR);
        checks.expect(
            ((vposr_precheck >> 13) & 1) == 1,
            "PRECONDITION-BEAMCON0-EFFECTIVE-ECS1: VPOSR's own "
            "ecs-identification bit (bit 13) reads 1 (" +
            hex16(vposr_precheck) +
            ") in the harness used for BEAMCON0's write-only assertion "
            "below, confirming ecs=1 actually reached the DUT rather than "
            "being assumed from the constructor argument -- BEAMCON0's "
            "write-only claim is only meaningful under this precondition: "
            "MEASURED on sol:7-appB-beamcon0.c2/.c3/.c5, 'the write decode "
            "is gated on `ecs`, so at ecs=0 the register does not exist "
            "as far as this boundary can tell' -- reproduced directly "
            "below in BEAMCON0-ECS-MARK-NEW.");

        bool base_harddis   = h.dut->harddis_out;
        bool base_varbeamen = h.dut->varbeamen_out;
        h.write_reg(A_BEAMCON0, (1 << BC_HARDDIS) | (1 << BC_VARBEAMEN));
        bool after_harddis   = h.dut->harddis_out;
        bool after_varbeamen = h.dut->varbeamen_out;
        uint16_t beamcon0_read = h.read(A_BEAMCON0);
        uint16_t undecoded     = h.read(A_UNDECODED_CLXDAT);

        checks.expect(
            after_harddis && after_varbeamen && !base_harddis && !base_varbeamen,
            "BEAMCON0-DIRECTION-WRITE-ONLY (scoped to ecs=1, precondition "
            "above): writing HARDDIS|VARBEAMEN to $1DC raises harddis_out/"
            "varbeamen_out from their reset baseline (both false) to both "
            "true -- the write half of 'write-only' -- 'BEAMCON0      1DC "
            "  W      A(E)   Beam counter control register (SHRES,PAL)' "
            "(p.293, restored quote; the corpus row itself prints a "
            "lower-case 'w', recorded as unrepaired OCR residue against "
            "both the legend's and the printed page's 'W'). "
            "sol:7-appB-beamcon0.c2, confirmed by opus:7-appB.c35.");

        checks.record(
            "BEAMCON0-READBACK-INCONCLUSIVE: reading $1DC after the write "
            "above returns " + hex16(beamcon0_read) +
            ", identical to an address this module never decodes at all "
            "($00E reads " + hex16(undecoded) +
            "). This is consistent with 'write-only' (no readback path) "
            "but equally consistent with 'not decoded on the read side at "
            "all' -- a zero here cannot, by itself, confirm the claim, "
            "only fail to refute it (METHODOLOGY (2)). Declined rather "
            "than graded green.");

        Harness h_c5_base(/*ecs=*/true, /*ntsc=*/false);
        bool c5_base_h = h_c5_base.dut->harddis_out;
        Harness h_c5_wrong(/*ecs=*/true, /*ntsc=*/false);
        h_c5_wrong.write_reg(A_UNDECODED_CLXDAT, 1 << BC_HARDDIS);
        bool c5_wrong_h = h_c5_wrong.dut->harddis_out;
        Harness h_c5_correct(/*ecs=*/true, /*ntsc=*/false);
        h_c5_correct.write_reg(A_BEAMCON0, 1 << BC_HARDDIS);
        bool c5_correct_h = h_c5_correct.dut->harddis_out;
        checks.expect(
            c5_wrong_h == c5_base_h && !c5_base_h && c5_correct_h,
            "BEAMCON0-CHIP-AGNUS-ENHANCED: writing HARDDIS to $00E (an "
            "address outside this claims file's five rows) leaves "
            "harddis_out at its untouched baseline (false); writing it to "
            "$1DC sets harddis_out -- structural (METHODOLOGY (3)); "
            "Appendix C p.305 identifies BEAMCON0 with the enhanced Agnus "
            "-- 'BEAMCON0      1DC   W      A(E)   Beam counter control "
            "register (SHRES,PAL)' (p.293, restored quote). "
            "sol:7-appB-beamcon0.c3, confirmed by opus:7-appB.c35.");

        Harness h_e2_ecs1(/*ecs=*/true, /*ntsc=*/false);
        bool e2_ecs1_base_h = h_e2_ecs1.dut->harddis_out;
        h_e2_ecs1.write_reg(A_BEAMCON0, (1 << BC_HARDDIS) | (1 << BC_VARBEAMEN));
        bool e2_ecs1_h = h_e2_ecs1.dut->harddis_out;
        bool e2_ecs1_v = h_e2_ecs1.dut->varbeamen_out;
        Harness h_e2_ecs0(/*ecs=*/false, /*ntsc=*/false);
        bool e2_ecs0_base_h = h_e2_ecs0.dut->harddis_out;
        h_e2_ecs0.write_reg(A_BEAMCON0, (1 << BC_HARDDIS) | (1 << BC_VARBEAMEN));
        bool e2_ecs0_h = h_e2_ecs0.dut->harddis_out;
        bool e2_ecs0_v = h_e2_ecs0.dut->varbeamen_out;
        checks.expect(
            e2_ecs1_h && e2_ecs1_v && !e2_ecs1_base_h &&
            !e2_ecs0_h && !e2_ecs0_v && !e2_ecs0_base_h,
            "BEAMCON0-ECS-MARK-NEW: the identical write to $1DC raises "
            "harddis_out/varbeamen_out at ecs=1 (from false/false baseline "
            "to true/true) and changes NOTHING at ecs=0 (stays "
            "false/false) -- BEAMCON0 does not merely behave differently "
            "under ECS, it does not exist as a functioning register at "
            "all outside it, matching the legend's 'completely new' class "
            "(Appendix A p.264 / Appendix B p.288, cited jointly) -- "
            "'BEAMCON0      1DC   W      A(E)   Beam counter control "
            "register (SHRES,PAL)' (p.293, restored quote), new per "
            "Appendix C p.298. sol:7-appB-beamcon0.c5. Shares its ecs=1 "
            "measurement with BEAMCON0-DIRECTION-WRITE-ONLY above -- one "
            "RTL mechanism (the `&& ecs` write gate), two manual "
            "propositions (see the INDEPENDENCE note above the "
            "PER-ASSERTION MUTATION RECORDS).");
    }

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
    return 0;
}
