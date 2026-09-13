// ch7-bplcon2-live-bits-and-ecs -- BPLCON2's high byte (bits 15-7) measured
// one bit at a time, the Appendix-A/B (E) mark, KILLEHB and ENBPLCN3.
//
// Test purpose: SPECIFICATION.md's P1-P9, written from the manual quotations
//   alone before the RTL was opened. A red result below means denise.v
//   disagrees with the manual, not that the assertion is wrong -- no
//   assertion here was, or should be, adjusted to make the RTL pass.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   210 (Table 7-1), 271 (Appendix A bit matrix and BPLCON2 row), 291
//   (Appendix B BPLCON2 row) and 307 (Appendix C Genlock Extensions).
// Non-goals: BPLCON2's low seven bits (playfield/sprite priority fields,
//   already owned by ch7-sprite-priority-groups and
//   ch7-inter-sprite-priority); BPLCON3's own fields and ENBPLCN3's gating
//   effect on them (no BPLCON3 field is quoted in this package's claims --
//   see ENBPLCN3-NOT-ASSERTED below); BPLCON2's register address and
//   write-only direction (owned by hrm/ch3-register-map-denise, which
//   holds ecs=1 throughout and explicitly leaves killehb untested -- the
//   gap this package fills, per STEP3-NOTES).
//
// BOUNDARY: top module `denise` (one Verilator build, sources denise.v +
//   denise_bitplanes.v + denise_bitplane_shifter.v + denise_playfields.v +
//   denise_sprites.v + denise_sprites_shifter.v + denise_collision.v +
//   denise_hamgenerator.v + denise_colortable.v + denise_colortable_ram_mf.v
//   + denise_spritepriority.v + bitplane_ram.v), matching
//   hrm/ch3-register-map-denise's own boundary since both exercise the same
//   BPLCON2-to-pixel path. a1k=0, aga=0 throughout (this package's claims
//   are OCS/ECS only; aga=0 also keeps bit 8's rdram gate, denise.v:219
//   `assign rdram = bplcon2[8] & aga`, inert so bit 8 reads as dead on the
//   same basis as the true-dead bits rather than confounded by AGA).
//
// THE RIG, and why: every sweep-based assertion below uses the SAME
//   six-bitplane, single-playfield, EHB-eligible configuration --
//   BPLCON0=$6000 (bpu=6, ham=0, dpf=0, denise.v:181-183), BPL1..5DAT held
//   at $0000 and BPL6DAT refreshed at $FFFF -- so the colour-table select
//   index presented is fixed at 32 (denise_playfields.v:100 plfdata=
//   bpldata[8:1], denise.v:367-372 gate bpldata[7:1] on l_bpu, giving index
//   bit 5 = bitplane 6's data, bits 4-0 = 0). This is deliberate, not
//   incidental (the coordinator's note): the SAME rig that proves bits 15,
//   14, 13, 12, 11, 10, 8 and 7 dead also carries bit 9 (KILLEHB) live at
//   ecs=1 -- a dead-bit assertion measured on a rig that could never show a
//   live bit either proves nothing against the units-error objection. Index
//   32 is what makes bit 9 reachable at all: denise_colortable.v:34 only
//   folds/halves when `ehb_en` (denise.v:431, itself gated by
//   `!killehb`) is true AND the select's bit 5 is set (denise_colortable.v:
//   52,62 `ehb_sel <= select_xored[5]`), so an index below 32 would make
//   killehb's own bit invisible regardless of whether the RTL implements
//   it -- exactly the "companion condition" trap a sibling denise-family
//   package warned about.
//
// HOW A SAMPLE IS TAKEN (mechanical fact, read out of the RTL after the
//   manual quotations already fixed what counts as pass/fail): a single
//   BPLxDAT write only pulses the plane shifter's parallel load once
//   (denise_bitplane_shifter.v:223-226); Harness::run_sweep below rewrites
//   BPL2..6DAT and BPL1DAT every round (BPL1DAT last, since its write both
//   latches l_bpu, denise.v:195-203, and pulses the shared `load` that
//   parallel-loads every shifter, denise_bitplane_shifter.v:223-226) and
//   samples red/green/blue immediately after each of the six writes, with
//   no extra settle ticks -- 20 rounds x 6 samples = 120 samples, the same
//   "120 sampled pixels" shape STEP3-NOTES discloses. This is reproduced,
//   not adopted: nothing here assumes the disclosed 115-of-120 figure for
//   bit 9 at ecs=1, and the measured count is reported alongside the
//   assertion either way.
//
// THE C1/C3 CLOCKING and the write_reg/tick pattern below are copied
//   unchanged from hrm/ch3-register-map-denise/test_denise.cpp (that
//   package's own file header documents amiga_clk.v's c1/c3 overlap trap in
//   full; repeating the working pattern here rather than re-deriving it).
//
// CLAIM GROUPS -- every one of the 18 claim uids in
//   ch7-bplcon2-live-bits-and-ecs.jsonl is cited, in full, on the single
//   assertion (or record) that tests/declines it, matching
//   hrm/ch3-register-map-denise's convention:
//   CONTROL-FIXTURE-ALIVE (no claim uid)
//   BPLCON2-BIT15-DEAD-OCS / -ECS (4 uids, shared: undisputed both chipsets)
//   BPLCON2-BIT14-DEAD-OCS (1 uid) + BIT14-ECS-RECORDED (same uid)
//   BPLCON2-BIT13-DEAD-OCS (1 uid) + BIT13-ECS-RECORDED (same uid)
//   BPLCON2-BIT12-DEAD-OCS (1 uid) + BIT12-ECS-RECORDED (same uid)
//   BPLCON2-BIT11-DEAD-OCS (1 uid) + BIT11-ECS-RECORDED (same uid)
//   BPLCON2-BIT10-DEAD-OCS (1 uid) + BIT10-ECS-RECORDED (same uid)
//   EHB-ACTIVE-PRECONDITION (engineering precondition, no manual page)
//   BPLCON2-BIT9-DEAD-OCS (1 uid)
//   BPLCON2-BIT9-KILLEHB-LIVE-ECS (2 uids + 2 co-cited (E)-mark uids)
//   BPLCON2-BIT8-DEAD-OCS / -ECS (1 uid)
//   BPLCON2-BIT7-DEAD-OCS / -ECS (1 uid)
//   VIDEO-PRIORITY-FRAMING-RECORDED (1 uid)
//   ENBPLCN3-NOT-ASSERTED (1 uid)
//   18 uids total: 4+1+1+1+1+1+1+2+2+1+1+1+1 = 18.
//
// MUTATION-LOG SUMMARY (15 assertions, 15 mutation flags; each was actually
//   built with its own flag defined to 1, confirmed to turn only its own
//   assertion red with no change to any expected value, then rebuilt with
//   no flags to confirm the clean baseline returned). One `// MUTATION`
//   line per assertion below, second token the exact expect() label (the
//   token check_gates.py's LEADING_ID_RE reads: the leading uppercase
//   token of the label, up to the first space or colon) -- corrected from
//   an earlier draft that used the MUT_ flag's shortened name instead,
//   which paired with nothing:
//
// MUTATION CONTROL-FIXTURE-ALIVE stimulus red: the second sample's index
//   was kept at 0 instead of switching to 1 (BPL1DAT stayed $0000), so the
//   "arrives in order, distinct" check saw the same COLOR00 reading twice.
//   (MUT_CONTROL)
// MUTATION EHB-ACTIVE-PRECONDITION stimulus red: BPL6DAT was held at
//   $0000 instead of $FFFF for the precondition's own sample, so the
//   select index stayed 0 instead of 32, the EHB fold never engaged, and
//   the sampled pixel came back as COLOR00's raw value instead of its
//   halved value. (MUT_EHB_PRECONDITION)
// MUTATION BPLCON2-BIT15-DEAD-OCS stimulus red: the BASELINE arm's own
//   index was pushed off 32 (BPL1DAT driven to $FFFF instead of $0000)
//   while the bit-under-test arm was left alone, so the two 120-sample
//   sweeps differed for a reason having nothing to do with bit 15, and
//   the "diff==0" check failed. (MUT_BIT15_OCS)
// MUTATION BPLCON2-BIT15-DEAD-ECS stimulus red: same injection as
//   BPLCON2-BIT15-DEAD-OCS, run at ecs=1. (MUT_BIT15_ECS)
// MUTATION BPLCON2-BIT14-DEAD-OCS stimulus red: same injection shape,
//   bit 14. (MUT_BIT14_OCS)
// MUTATION BPLCON2-BIT13-DEAD-OCS stimulus red: same injection shape,
//   bit 13. (MUT_BIT13_OCS)
// MUTATION BPLCON2-BIT12-DEAD-OCS stimulus red: same injection shape,
//   bit 12. (MUT_BIT12_OCS)
// MUTATION BPLCON2-BIT11-DEAD-OCS stimulus red: same injection shape,
//   bit 11. (MUT_BIT11_OCS)
// MUTATION BPLCON2-BIT10-DEAD-OCS stimulus red: same injection shape,
//   bit 10. (MUT_BIT10_OCS)
// MUTATION BPLCON2-BIT9-DEAD-OCS stimulus red: same injection shape,
//   bit 9 at ecs=0. (MUT_BIT9_OCS)
// MUTATION BPLCON2-BIT9-KILLEHB-LIVE-ECS stimulus red: the bit-set arm
//   wrote BPLCON2 bit 10 (ZDCTEN, appendix C p.307, not implemented in
//   this design) instead of bit 9 (KILLEHB), so the two sweeps no longer
//   differed and the "diff>0" check failed. (MUT_BIT9_ECS)
// MUTATION BPLCON2-BIT8-DEAD-OCS stimulus red: same injection shape,
//   bit 8 at ecs=0. (MUT_BIT8_OCS)
// MUTATION BPLCON2-BIT8-DEAD-ECS stimulus red: same injection shape,
//   bit 8 at ecs=1. (MUT_BIT8_ECS)
// MUTATION BPLCON2-BIT7-DEAD-OCS stimulus red: same injection shape,
//   bit 7 at ecs=0. (MUT_BIT7_OCS)
// MUTATION BPLCON2-BIT7-DEAD-ECS stimulus red: same injection shape,
//   bit 7 at ecs=1. (MUT_BIT7_ECS)
//
// GATE READABILITY -- said here rather than left looking like a gap: the
//   twelve BPLCON2-BITn-DEAD-* records above name assertions that are all
//   graded through ONE shared function, `assert_bit_dead()` (below), whose
//   own `checks.expect(diff == 0, label + " diff=" + ...)` call appears
//   exactly once in this file's text with `label` a parameter, not a
//   literal. A mechanical scan for `expect(` therefore reads that one call
//   site once and cannot recover any of the twelve per-call labels from it
//   -- the same "label built outside a literal" shape BUILD-BRIEF warns
//   about for a lambda, one level removed (a shared function instead of a
//   lambda). This is disclosed, not hidden: the twelve MUTATION records
//   above are real (each was built with its own MUT_ flag, produced
//   exactly 14 passed/1 failed, and reverted -- see the paragraph below),
//   but a structural, mechanical scan of this file cannot independently
//   confirm that on its own for these twelve, only for the three whose
//   label is a literal at the call site (CONTROL-FIXTURE-ALIVE,
//   EHB-ACTIVE-PRECONDITION, BPLCON2-BIT9-KILLEHB-LIVE-ECS). Making the
//   twelve independently legible would mean inlining each call so its own
//   label is a literal at its own call site -- a change to the assertion
//   code itself, out of scope for this pass.
//
// Baseline (no MUT_ flag defined): 15 passed, 0 failed. Every one of the 15
// mutations above was built and run individually (`make
// MUT=-DMUT_<NAME>=1`) and every one produced exactly 14 passed, 1 failed
// -- only the named assertion turned red, no other assertion moved, and a
// plain `make` afterward returned to 15 passed, 0 failed. No expected value
// was changed by any of them; only what each check asks the module to do.
//
// MEASUREMENT DISAGREEMENT TO REPORT: this file's own 120-sample sweep
// measures BPLCON2 bit 9 at ecs=1 changing 108 of 120 pixels, not the 115
// of 120 STEP3-NOTES discloses. Both readings agree on the shape (most, not
// all, of 120 differ; 0 of 120 for every other bit) but not the exact
// count -- expected, since this file's rig sample-per-write_reg-call timing
// was built independently rather than copied from the classification
// stage's own dn2.cpp harness. Reported per the coordinator's instruction,
// not silently reconciled.

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "Vdenise.h"
#include "verilated.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_EHB_PRECONDITION
#define MUT_EHB_PRECONDITION 0
#endif
#ifndef MUT_BIT15_OCS
#define MUT_BIT15_OCS 0
#endif
#ifndef MUT_BIT15_ECS
#define MUT_BIT15_ECS 0
#endif
#ifndef MUT_BIT14_OCS
#define MUT_BIT14_OCS 0
#endif
#ifndef MUT_BIT13_OCS
#define MUT_BIT13_OCS 0
#endif
#ifndef MUT_BIT12_OCS
#define MUT_BIT12_OCS 0
#endif
#ifndef MUT_BIT11_OCS
#define MUT_BIT11_OCS 0
#endif
#ifndef MUT_BIT10_OCS
#define MUT_BIT10_OCS 0
#endif
#ifndef MUT_BIT9_OCS
#define MUT_BIT9_OCS 0
#endif
#ifndef MUT_BIT9_ECS
#define MUT_BIT9_ECS 0
#endif
#ifndef MUT_BIT8_OCS
#define MUT_BIT8_OCS 0
#endif
#ifndef MUT_BIT8_ECS
#define MUT_BIT8_ECS 0
#endif
#ifndef MUT_BIT7_OCS
#define MUT_BIT7_OCS 0
#endif
#ifndef MUT_BIT7_ECS
#define MUT_BIT7_ECS 0
#endif

namespace {

// reg_address_in is [8:1] (word address >> 1), matching denise.v's own
// `parameter` block (denise.v:56-65) and denise_bitplanes.v:106-115.
constexpr uint16_t IDLE_ADDR    = 0x00;
constexpr uint16_t BPLCON0_ADDR = 0x80;  // 9'h100 >> 1  (denise.v:59)
constexpr uint16_t BPLCON2_ADDR = 0x82;  // 9'h104 >> 1  (denise.v:60)
constexpr uint16_t DIWSTRT_ADDR = 0x47;  // 9'h08E >> 1  (denise.v:56)
constexpr uint16_t DIWSTOP_ADDR = 0x48;  // 9'h090 >> 1  (denise.v:57)
constexpr uint16_t BPL1DAT_ADDR = 0x88;  // 9'h110 >> 1  (denise_bitplanes.v:107)
constexpr uint16_t BPL2DAT_ADDR = 0x89;  // 9'h112 >> 1  (denise_bitplanes.v:108)
constexpr uint16_t BPL3DAT_ADDR = 0x8A;  // 9'h114 >> 1  (denise_bitplanes.v:109)
constexpr uint16_t BPL4DAT_ADDR = 0x8B;  // 9'h116 >> 1  (denise_bitplanes.v:110)
constexpr uint16_t BPL5DAT_ADDR = 0x8C;  // 9'h118 >> 1  (denise_bitplanes.v:111)
constexpr uint16_t BPL6DAT_ADDR = 0x8D;  // 9'h11A >> 1  (denise_bitplanes.v:112)
constexpr uint16_t COLOR_BASE   = 0xC0;  // 9'h180 >> 1  (denise.v:65); COLORi = COLOR_BASE+i.

// BPLCON2 bit constants under test.
constexpr uint16_t BIT15 = 0x8000, BIT14 = 0x4000, BIT13 = 0x2000, BIT12 = 0x1000;
constexpr uint16_t BIT11 = 0x0800, BIT10 = 0x0400, BIT9  = 0x0200, BIT8  = 0x0100;
constexpr uint16_t BIT7  = 0x0080;

uint32_t pack(uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | b;
}

class Harness {
public:
    explicit Harness(bool ecs) : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = ecs ? 1 : 0;
        dut->aga = 0;
        phase = 0;
        drive_phase();
        dut->eval();
        for (int i = 0; i < 32; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();
    }
    ~Harness() { delete dut; }

    // Same measured working pattern as hrm/ch3-register-map-denise: phase 0
    // clk7_en, phases 0-1 c1, phases 1-2 c3 (the C1/C3 TRAP note above).
    void tick() {
        drive_phase();
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) & 3;
    }

    void drive_phase() {
        dut->clk7_en = (phase == 0) ? 1 : 0;
        dut->c1 = (phase == 0 || phase == 1) ? 1 : 0;
        dut->c3 = (phase == 1 || phase == 2) ? 1 : 0;
    }

    void write_reg(uint16_t addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
    }

    void pulse_strhor() {
        dut->strhor = 1;
        tick(); tick(); tick(); tick();
        dut->strhor = 0;
    }

    // hpos increments by 1 once every 4 ticks (denise.v:136-142).
    void advance_hpos(int units) {
        for (int i = 0; i < units; ++i) { tick(); tick(); tick(); tick(); }
    }

    uint32_t current_rgb() const {
        return pack(dut->red, dut->green, dut->blue);
    }

    uint32_t read_rgb() {
        tick(); tick(); tick(); tick();
        return current_rgb();
    }

    uint32_t sample_at_hpos(int target_hpos) {
        pulse_strhor();
        advance_hpos(target_hpos - 2);
        return read_rgb();
    }

    Vdenise* dut;
    int phase;
};

// The shared rig (file header, THE RIG): BPLCON0=$6000 (bpu=6, ham=0,
// dpf=0), BPL1..5DAT held at $0000, BPL6DAT refreshed at $FFFF -- a fixed
// colour-select index of 32. `inject_off_index`, when true, drives BPL1DAT
// high on every round instead of low (index 33 instead of 32) -- used only
// by the per-assertion MUTATION flags below to prove a "no difference"
// result is not vacuous.
std::vector<uint32_t> run_sweep(bool ecs, uint16_t bplcon2_value, bool inject_off_index) {
    Harness h(ecs);
    h.write_reg(BPLCON0_ADDR, 0x6000);
    h.write_reg(BPLCON2_ADDR, bplcon2_value);
    // COLOR00 must be a distinct, known, non-zero value: index 32 (this
    // rig's fixed select index) reads COLOR00 through the EHB fold when
    // ehb_en is true (denise_colortable.v:34) and reads RAM address 32
    // directly otherwise -- with COLOR00 left at its unwritten reset value
    // (0), both paths would read 0 and every BIT9 comparison would be
    // vacuously equal regardless of killehb. See EHB-ACTIVE-PRECONDITION.
    h.write_reg(COLOR_BASE + 0, 0x0F0F);
    h.write_reg(DIWSTRT_ADDR, 0x0030);
    h.write_reg(DIWSTOP_ADDR, 0x00A0);
    h.sample_at_hpos(0x0130);   // flush any latched 'window' reg garbage (denise.v:328-334)
    h.pulse_strhor();
    h.advance_hpos(0x30 - 2);

    std::vector<uint32_t> samples;
    samples.reserve(120);
    for (int round = 0; round < 20; ++round) {
        h.write_reg(BPL2DAT_ADDR, 0x0000); samples.push_back(h.current_rgb());
        h.write_reg(BPL3DAT_ADDR, 0x0000); samples.push_back(h.current_rgb());
        h.write_reg(BPL4DAT_ADDR, 0x0000); samples.push_back(h.current_rgb());
        h.write_reg(BPL5DAT_ADDR, 0x0000); samples.push_back(h.current_rgb());
        h.write_reg(BPL6DAT_ADDR, 0xFFFF); samples.push_back(h.current_rgb());
        h.write_reg(BPL1DAT_ADDR, inject_off_index ? 0xFFFF : 0x0000); samples.push_back(h.current_rgb());
    }
    return samples;
}

int count_diff(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) {
    int n = 0;
    for (size_t i = 0; i < a.size() && i < b.size(); ++i)
        if (a[i] != b[i]) ++n;
    return n;
}

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

// One dead-bit assertion: bit `bplcon2_bit` of BPLCON2, at the given `ecs`
// setting, changes none of the 120 sampled pixels relative to a baseline of
// BPLCON2=$0000. `inject` (only true under the assertion's own MUT_ flag)
// pushes the baseline off-index so the check is not vacuous.
void assert_bit_dead(Checks& checks, bool ecs, uint16_t bplcon2_bit,
                      bool inject, const std::string& label) {
    auto baseline = run_sweep(ecs, 0x0000, inject);
    auto bitset   = run_sweep(ecs, bplcon2_bit, false);
    int diff = count_diff(baseline, bitset);
    checks.expect(diff == 0, label + " diff=" + std::to_string(diff) + " of 120 sampled pixels.");
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // CONTROL-FIXTURE-ALIVE -- the fixture can deliver a known, distinct
    // word before anything below trusts a "0 of 120 differ" reading (package
    // brief: "the fixture proves it can deliver a known word before anything
    // reads one"). Uses the exact rig (BPLCON0=$6000, six bitplanes) every
    // other assertion in this file relies on: index 0 reads COLOR00, index 1
    // reads COLOR01, in that order, distinct.
    // =========================================================================
    {
        Harness h(true);
        h.write_reg(BPLCON0_ADDR, 0x6000);
        h.write_reg(BPLCON2_ADDR, 0x0000);
        h.write_reg(COLOR_BASE + 0, 0x0F00);  // COLOR00: pure red nibble -> bytes FF,00,00
        h.write_reg(COLOR_BASE + 1, 0x00F0);  // COLOR01: pure green nibble -> bytes 00,FF,00
        h.write_reg(DIWSTRT_ADDR, 0x0030);
        h.write_reg(DIWSTOP_ADDR, 0x00A0);
        h.sample_at_hpos(0x0130);
        h.pulse_strhor();
        h.advance_hpos(0x30 - 2);

        for (int r = 0; r < 8; ++r) {
            h.write_reg(BPL2DAT_ADDR, 0x0000);
            h.write_reg(BPL3DAT_ADDR, 0x0000);
            h.write_reg(BPL4DAT_ADDR, 0x0000);
            h.write_reg(BPL5DAT_ADDR, 0x0000);
            h.write_reg(BPL6DAT_ADDR, 0x0000);
            h.write_reg(BPL1DAT_ADDR, 0x0000);  // index 0
        }
        uint32_t sample0 = h.current_rgb();

        bool second_index_high = true;
#if MUT_CONTROL
        second_index_high = false;
#endif
        for (int r = 0; r < 8; ++r) {
            h.write_reg(BPL2DAT_ADDR, 0x0000);
            h.write_reg(BPL3DAT_ADDR, 0x0000);
            h.write_reg(BPL4DAT_ADDR, 0x0000);
            h.write_reg(BPL5DAT_ADDR, 0x0000);
            h.write_reg(BPL6DAT_ADDR, 0x0000);
            h.write_reg(BPL1DAT_ADDR, second_index_high ? 0xFFFF : 0x0000);  // index 1 (or 0 under MUT_CONTROL)
        }
        uint32_t sample1 = h.current_rgb();

        uint32_t expected0 = pack(0xFF, 0x00, 0x00);
        uint32_t expected1 = pack(0x00, 0xFF, 0x00);
        bool ok = sample0 == expected0 && sample1 == expected1 && sample0 != sample1;
        checks.expect(ok, "CONTROL-FIXTURE-ALIVE: with the six-bitplane rig this file uses "
                           "throughout, index 0 reads COLOR00 and index 1 reads COLOR01, in "
                           "that order and distinctly, before any 'diff=0' reading below is "
                           "trusted to mean dead rather than stuck. No manual page: the "
                           "control for every dead-bit assertion in this file, on the same "
                           "measurement basis. Also exercises opus:7-vp.c16 (p.210, \"You "
                           "control the video priorities by using the bits in BPLCON2 ... as "
                           "shown in Table 7-1\") -- the manual's own licence for using "
                           "denise's pixel output as the observable this whole file measures "
                           "bit-liveness through.");
    }

    // =========================================================================
    // VIDEO-PRIORITY-FRAMING-RECORDED -- opus:7-vp.c16 is framing, not a
    // proposition of its own (SPECIFICATION.md, "Not turned into
    // assertions"): it licenses the observable, and CONTROL-FIXTURE-ALIVE
    // above is where that licence is actually exercised.
    // =========================================================================
    checks.record("VIDEO-PRIORITY-FRAMING: opus:7-vp.c16 (p.210) states that BPLCON2 "
                   "governs a directly observable display characteristic; this is the "
                   "methodological basis for every dead/live-bit assertion in this file "
                   "(exercised concretely by CONTROL-FIXTURE-ALIVE above), not a separate "
                   "testable proposition of its own.");

    // =========================================================================
    // EHB-ACTIVE-PRECONDITION -- engineering precondition (BUILD-BRIEF's
    // fixture-delivers-a-known-word rule applied to P7's ECS half, not
    // sourced to a manual page): before KILLEHB's disabling effect is
    // graded, show the baseline it is disabling from actually reaches an
    // Extra-Half-Brite-active state. With killehb=0, ecs=1 and the rig's
    // index fixed at 32, denise_colortable.v:34's fold reads COLOR00 and
    // :62's halving applies -- the sampled pixel must equal COLOR00's own
    // written value with every channel divided by two exactly, not merely
    // "some other value".
    // =========================================================================
    {
        Harness h(true);
        h.write_reg(BPLCON0_ADDR, 0x6000);
        h.write_reg(BPLCON2_ADDR, 0x0000);  // killehb=0
        h.write_reg(COLOR_BASE + 0, 0x0F0F);  // COLOR00 -> bytes FF,00,FF; halved -> 7F,00,7F
        h.write_reg(DIWSTRT_ADDR, 0x0030);
        h.write_reg(DIWSTOP_ADDR, 0x00A0);
        h.sample_at_hpos(0x0130);
        h.pulse_strhor();
        h.advance_hpos(0x30 - 2);

        bool index_at_32 = true;
#if MUT_EHB_PRECONDITION
        index_at_32 = false;
#endif
        for (int r = 0; r < 8; ++r) {
            h.write_reg(BPL2DAT_ADDR, 0x0000);
            h.write_reg(BPL3DAT_ADDR, 0x0000);
            h.write_reg(BPL4DAT_ADDR, 0x0000);
            h.write_reg(BPL5DAT_ADDR, 0x0000);
            h.write_reg(BPL6DAT_ADDR, index_at_32 ? 0xFFFF : 0x0000);
            h.write_reg(BPL1DAT_ADDR, 0x0000);
        }
        uint32_t measured = h.current_rgb();
        uint32_t expected = pack(0x7F, 0x00, 0x7F);
        checks.expect(measured == expected,
            "EHB-ACTIVE-PRECONDITION: with killehb=0, ecs=1, bpu=6/ham=0/dpf=0/a1k=0 and "
            "the colour-select index at 32 (denise.v:431 ehb_en, denise_colortable.v:34/62), "
            "the sampled pixel equals COLOR00's own written value with every channel halved "
            "-- proving the EHB fold-and-halve path is genuinely engaged before "
            "BPLCON2-BIT9-KILLEHB-LIVE-ECS below is trusted to mean anything.");
    }

    // =========================================================================
    // BPLCON2-BIT15 -- undisputed dead on both chipsets (SPECIFICATION.md
    // P1). "15-7 Not used (keep at 0)" (p.210, Table 7-1); "15 HIRES X X"
    // (p.271, Appendix A, BPLCON2 column). sol:7-video.c18, opus:7-vp.c17,
    // sol:7-appA-bplcon2.c6, opus:7-appA-bplcon2.c5.
    // =========================================================================
    assert_bit_dead(checks, false, BIT15, MUT_BIT15_OCS,
        "BPLCON2-BIT15-DEAD-OCS (sol:7-video.c18, opus:7-vp.c17, "
        "sol:7-appA-bplcon2.c6, opus:7-appA-bplcon2.c5; p.210/271):");
    assert_bit_dead(checks, true, BIT15, MUT_BIT15_ECS,
        "BPLCON2-BIT15-DEAD-ECS (sol:7-video.c18, opus:7-vp.c17, "
        "sol:7-appA-bplcon2.c6, opus:7-appA-bplcon2.c5; p.210/271, undisputed on ECS too "
        "-- appendix C p.307's table never reaches bit 15):");

    // =========================================================================
    // BPLCON2-BIT14 -- OCS half asserted (SPECIFICATION.md P2); ECS half
    // (ZDBPSEL2, p.307) recorded, not asserted, per opus:7-appA-bplcon2.c6's
    // own conflict_resolution ("the signal does not exist in this design at
    // all"), confirmed here: bit 14 is latched at denise.v:215 but never
    // read anywhere else in denise.v or any sub-module it instantiates.
    // =========================================================================
    assert_bit_dead(checks, false, BIT14, MUT_BIT14_OCS,
        "BPLCON2-BIT14-DEAD-OCS (opus:7-appA-bplcon2.c6; p.271):");
    {
        auto baseline = run_sweep(true, 0x0000, false);
        auto bitset   = run_sweep(true, BIT14, false);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT14-ECS (ZDBPSEL2, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c6's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT14-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c6; p.307): appendix C "
                       "names bit 14 ZDBPSEL2 on ECS; recorded per SPECIFICATION.md P2, not asserted "
                       "-- the manual states a function this design does not implement, so there is "
                       "nothing to grade a pass/fail against.");
    }

    // =========================================================================
    // BPLCON2-BIT13 -- same shape as BIT14 (SPECIFICATION.md P3). ECS name:
    // ZDBPSEL1.
    // =========================================================================
    assert_bit_dead(checks, false, BIT13, MUT_BIT13_OCS,
        "BPLCON2-BIT13-DEAD-OCS (opus:7-appA-bplcon2.c7; p.271):");
    {
        auto baseline = run_sweep(true, 0x0000, false);
        auto bitset   = run_sweep(true, BIT13, false);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT13-ECS (ZDBPSEL1, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c7's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT13-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c7; p.307): appendix C "
                       "names bit 13 ZDBPSEL1 on ECS; recorded per SPECIFICATION.md P3, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT12 -- same shape (SPECIFICATION.md P4). ECS name: ZDBPSEL0.
    // =========================================================================
    assert_bit_dead(checks, false, BIT12, MUT_BIT12_OCS,
        "BPLCON2-BIT12-DEAD-OCS (opus:7-appA-bplcon2.c8; p.271):");
    {
        auto baseline = run_sweep(true, 0x0000, false);
        auto bitset   = run_sweep(true, BIT12, false);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT12-ECS (ZDBPSEL0, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c8's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT12-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c8; p.307): appendix C "
                       "names bit 12 ZDBPSEL0 on ECS; recorded per SPECIFICATION.md P4, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT11 -- same shape (SPECIFICATION.md P5). ECS name: ZDBPEN.
    // =========================================================================
    assert_bit_dead(checks, false, BIT11, MUT_BIT11_OCS,
        "BPLCON2-BIT11-DEAD-OCS (opus:7-appA-bplcon2.c9; p.271):");
    {
        auto baseline = run_sweep(true, 0x0000, false);
        auto bitset   = run_sweep(true, BIT11, false);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT11-ECS (ZDBPEN, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c9's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT11-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c9; p.307): appendix C "
                       "names bit 11 ZDBPEN on ECS; recorded per SPECIFICATION.md P5, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT10 -- same shape (SPECIFICATION.md P6). ECS name: ZDCTEN.
    // =========================================================================
    assert_bit_dead(checks, false, BIT10, MUT_BIT10_OCS,
        "BPLCON2-BIT10-DEAD-OCS (opus:7-appA-bplcon2.c10; p.271):");
    {
        auto baseline = run_sweep(true, 0x0000, false);
        auto bitset   = run_sweep(true, BIT10, false);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT10-ECS (ZDCTEN, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c10's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT10-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c10; p.307): appendix C "
                       "names bit 10 ZDCTEN on ECS; recorded per SPECIFICATION.md P6, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT9 -- OCS half asserted dead (SPECIFICATION.md P7 OCS half).
    // =========================================================================
    assert_bit_dead(checks, false, BIT9, MUT_BIT9_OCS,
        "BPLCON2-BIT9-DEAD-OCS (opus:7-appA-bplcon2.c11; p.271):");

    // =========================================================================
    // BPLCON2-BIT9 -- ECS half asserted LIVE (SPECIFICATION.md P7 ECS half,
    // the package's centerpiece): KILLEHB kills Extra Half-Brite. This is
    // the live-bit measurement, on the identical rig and in the identical
    // run shape as every dead-bit assertion above, that the coordinator
    // asked be used deliberately as the cross-check against "a dead-bit
    // assertion just means the harness observes nothing".
    // =========================================================================
    {
        uint16_t set_bit = BIT9;
#if MUT_BIT9_ECS
        set_bit = BIT10;  // ZDCTEN, not implemented -- makes the diff vanish
#endif
        auto baseline = run_sweep(true, 0x0000, false);
        auto bitset   = run_sweep(true, set_bit, false);
        int diff = count_diff(baseline, bitset);
        checks.expect(diff > 0,
            "BPLCON2-BIT9-KILLEHB-LIVE-ECS (sol:7-appC-bplcon2.c5, opus:7-appC-bplcon2.c7; "
            "p.307, \"09 KILLEHB Kill halfbrite\"; co-cited sol:7-appA-bplcon2.c4 p.271 and "
            "sol:7-appB-bplcon2.c5 p.291, the (E) mark) diff=" + std::to_string(diff) + " of 120 "
            "sampled pixels.");
        checks.note("BPLCON2-BIT9-ECS diff count against STEP3-NOTES' disclosed measurement "
                     "(115 of 120): " + std::to_string(diff) + " of 120 -- reproduced independently "
                     "on this file's own rig, not adopted.");
    }

    // =========================================================================
    // BPLCON2-BIT8 -- undisputed dead on both chipsets within this
    // package's scope (SPECIFICATION.md P8). Gated by AGA, not ECS
    // (denise.v:219 `rdram = bplcon2[8] & aga`), and aga=0 throughout this
    // file, so it is dead on the same OCS/ECS basis as bits 15/7.
    // =========================================================================
    assert_bit_dead(checks, false, BIT8, MUT_BIT8_OCS,
        "BPLCON2-BIT8-DEAD-OCS (opus:7-appA-bplcon2.c12; p.271):");
    assert_bit_dead(checks, true, BIT8, MUT_BIT8_ECS,
        "BPLCON2-BIT8-DEAD-ECS (opus:7-appA-bplcon2.c12; p.271, dead on ECS too within this "
        "package's aga=0 scope -- bit 8 is gated by aga, not ecs):");

    // =========================================================================
    // BPLCON2-BIT7 -- undisputed dead on both chipsets (SPECIFICATION.md
    // P9).
    // =========================================================================
    assert_bit_dead(checks, false, BIT7, MUT_BIT7_OCS,
        "BPLCON2-BIT7-DEAD-OCS (opus:7-appA-bplcon2.c13; p.271):");
    assert_bit_dead(checks, true, BIT7, MUT_BIT7_ECS,
        "BPLCON2-BIT7-DEAD-ECS (opus:7-appA-bplcon2.c13; p.271, undisputed on ECS too "
        "-- appendix C p.307's table never reaches bit 7):");

    // =========================================================================
    // ENBPLCN3-NOT-ASSERTED -- opus:7-appC-bplcon2.c9 (p.307, "00 ENBPLCN3
    // ... Enable new BLPCON3 register"). Its own scope_note says it stands
    // in this package only because it shares Appendix C p.307's genlock
    // table with the BPLCON2 columns this package owns. Recorded per
    // SPECIFICATION.md's "Not turned into assertions": no BPLCON3 field is
    // quoted anywhere in this package's claims, so what ENBPLCN3 "enables"
    // has no manual-quoted expected value to grade here, regardless of what
    // denise.v itself wires it to -- the oracle is the manual, not the RTL.
    // =========================================================================
    checks.record("ENBPLCN3-NOT-ASSERTED (opus:7-appC-bplcon2.c9; p.307): BPLCON0 bit 0 is "
                   "named ENBPLCN3 and the manual says it enables the new BPLCON3 register; "
                   "this package's claims quote no BPLCON3 field, so its enabling effect has "
                   "no manual-sourced expected value to grade here and belongs to a package "
                   "built against BPLCON3's own claims.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
