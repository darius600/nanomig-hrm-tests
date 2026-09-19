// ch7-bplcon2-live-bits-and-ecs -- BPLCON2's high byte (bits 15-7) measured
// one bit at a time, the Appendix-A/B (E) mark, KILLEHB and ENBPLCN3.
//
// Test purpose: SPECIFICATION.md's P1-P9 plus P10/P11 (added by this repair
//   -- see the REPAIR note below), written from the manual quotations alone
//   before the RTL was opened. A red result below means denise.v disagrees
//   with the manual, not that the assertion is wrong -- no assertion here
//   was, or should be, adjusted to make the RTL pass.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   210 (Table 7-1), 264 (Appendix A preamble), 271 (Appendix A bit matrix
//   and BPLCON2 row), 291 (Appendix B BPLCON2 row) and 307 (Appendix C
//   Genlock Extensions).
// Non-goals: BPLCON2's low seven bits beyond PF2PRI (the PF1P/PF2P
//   priority-placement codes, already owned by ch7-sprite-priority-groups
//   and ch7-inter-sprite-priority) -- PF2PRI (bit 6) itself is minimally
//   exercised below by VIDEO-PRIORITY-PF2PRI-LIVE, the one concrete instance
//   opus:7-vp.c16's "you control video priorities via BPLCON2" claim asks
//   for, not a comprehensive priority-ordering test; BPLCON3's own fields
//   beyond BRDRBLNK (bank, pf2of, loct, spres, brdsprt -- no other BPLCON3
//   field is quoted in this package's claims); BPLCON2's register address
//   and write-only direction (owned by hrm/ch3-register-map-denise, which
//   holds ecs=1 throughout and explicitly leaves killehb untested -- the gap
//   this package fills, per STEP3-NOTES).
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
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
// THE RIG, and why: every sweep-based measurement below uses the SAME
//   six-bitplane, single-playfield, EHB-eligible configuration --
//   BPLCON0=$6000 (bpu=6, ham=0, dpf=0, denise.v:181-183), BPL1..5DAT held
//   at $0000 and BPL6DAT refreshed at $FFFF -- so the colour-table select
//   index presented is fixed at 32 (denise_playfields.v:100 plfdata=
//   bpldata[8:1], denise.v:367-372 gate bpldata[7:1] on l_bpu, giving index
//   bit 5 = bitplane 6's data, bits 4-0 = 0). This is deliberate, not
//   incidental (the coordinator's note): the SAME rig that measures bits
//   15, 14, 13, 12, 11, 10, 8 and 7 changing nothing also carries bit 9
//   (KILLEHB) live at ecs=1 -- a dead-bit measurement taken on a rig that
//   could never show a live bit either proves nothing against the
//   units-error objection. Index 32 is what makes bit 9 reachable at all:
//   denise_colortable.v:34 only folds/halves when `ehb_en` (denise.v:431,
//   itself gated by `!killehb`) is true AND the select's bit 5 is set
//   (denise_colortable.v:52,62 `ehb_sel <= select_xored[5]`), so an index
//   below 32 would make killehb's own bit invisible regardless of whether
//   the RTL implements it -- exactly the "companion condition" trap a
//   sibling denise-family package warned about.
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
//   BPLCON2-BIT9-KILLEHB-LIVE-ECS assertion either way.
//
// THE C1/C3 CLOCKING and the write_reg/tick pattern below are copied
//   unchanged from hrm/ch3-register-map-denise/test_denise.cpp (that
//   package's own file header documents amiga_clk.v's c1/c3 overlap trap in
//   full; repeating the working pattern here rather than re-deriving it).
//
// CLAIM GROUPS -- every one of the 18 claim uids in
//   ch7-bplcon2-live-bits-and-ecs.jsonl is cited, in full, on the single
//   assertion (or note/record) that tests, measures or declines it,
//   matching hrm/ch3-register-map-denise's own convention:
//   CONTROL-FIXTURE-ALIVE (no claim uid)
//   VIDEO-PRIORITY-PF2PRI-LIVE (1 uid)
//   BPLCON2-BIT15-OCS/-ECS-MEASURED + BPLCON2-BIT15-NOT-GRADED (4 uids)
//   BPLCON2-BIT14-OCS-MEASURED + -NOT-GRADED (1 uid) + BIT14-ECS-RECORDED (same uid)
//   BPLCON2-BIT13-OCS-MEASURED + -NOT-GRADED (1 uid) + BIT13-ECS-RECORDED (same uid)
//   BPLCON2-BIT12-OCS-MEASURED + -NOT-GRADED (1 uid) + BIT12-ECS-RECORDED (same uid)
//   BPLCON2-BIT11-OCS-MEASURED + -NOT-GRADED (1 uid) + BIT11-ECS-RECORDED (same uid)
//   BPLCON2-BIT10-OCS-MEASURED + -NOT-GRADED (1 uid) + BIT10-ECS-RECORDED (same uid)
//   EHB-ACTIVE-PRECONDITION (engineering precondition, no manual page)
//   BPLCON2-BIT9-OCS-MEASURED + -NOT-GRADED (1 uid)
//   BPLCON2-BIT9-KILLEHB-LIVE-ECS (2 uids + 2 co-cited (E)-mark uids)
//   BPLCON2-BIT8-OCS/-ECS-MEASURED + -NOT-GRADED (1 uid)
//   BPLCON2-BIT7-OCS/-ECS-MEASURED + -NOT-GRADED (1 uid)
//   ENBPLCN3-ENABLES-BRDRBLNK (1 uid)
//   18 uids total: 4+1+1+1+1+1+1+2+2+1+1+1+1 = 18.
//
// REPAIR (independent validation, work/2026-09-13_ch7_sol_validation_B2.md,
//   slice B2, findings 1/5/6/7):
//   (1) Twelve assertions (BIT15/14/13/12/11/10/9/8/7's dead-on-OCS checks,
//   plus BIT15/8/7's dead-on-ECS checks) required diff==0 for a written 1
//   on a bit p.210/271 mark "Not used (keep at 0)" / X. p.264's Appendix-A
//   preamble ("may cause serious future software incompatibility if ...
//   implemented in the future") tells software what to write; neither page
//   requires the hardware to ignore a 1 already written. DEMOTED to
//   note()+record(), reason_kind source-silent -- the same reading that
//   withdrew six reds in validation slice A.
//   (5) BPLCON2-BIT9-KILLEHB-LIVE-ECS graded only diff>0 between a baseline
//   and a bit-set sweep, which passes for blanking, inversion or any
//   unrelated perturbation of bit 9 -- none of which is "kill halfbrite"
//   (p.307). FIXED in place: now grades an exact colour pair -- killehb=0
//   reads COLOR00 folded-and-halved by the EHB path; killehb=1 reads a
//   distinct, categorically different value (the RAM address the EHB fold
//   no longer redirects through COLOR00), demonstrating the automatic
//   fold itself stopped, not merely that "something" changed. The old
//   diff-count measurement is kept as a note, not deleted. (A first draft
//   of this fix wrongly expected COLOR00 restored unhalved and read a red
//   -- this package's own addressing mistake, corrected before this
//   report; not an RTL defect. See P7's repair note in SPECIFICATION.md.)
//   (6) CONTROL-FIXTURE-ALIVE carried a citation of opus:7-vp.c16 but only
//   ever changed BPL1DAT/COLOR -- BPLCON2 stayed zero throughout, so it
//   could not fail if every BPLCON2 priority bit were disconnected. This
//   claim's own classification says it is testable by toggling a BPLCON2
//   priority bit. PROMOTED into its own assertion,
//   VIDEO-PRIORITY-PF2PRI-LIVE, reproducing diag/dn.cpp's T1 measurement;
//   the false citation is removed from CONTROL-FIXTURE-ALIVE and the old
//   VIDEO-PRIORITY-FRAMING-RECORDED decline is replaced.
//   (7) ENBPLCN3-NOT-ASSERTED declined for lack of a quoted BPLCON3 field,
//   but p.307 -- the same page already used for KILLEHB -- defines
//   BRDRBLNK ("05 BRDRBLNK Border blank") on the same table, and BRDRBLNK
//   is itself gated by ENBPLCN3 in the RTL (denise.v:192,253,515).
//   PROMOTED to ENBPLCN3-ENABLES-BRDRBLNK: a graded enable/disable
//   experiment at the already-opened denise boundary, no debug port added.
//
// MUTATION-LOG SUMMARY (5 assertions, 5 mutation flags; each was actually
//   built with its own flag defined to 1, confirmed to turn only its own
//   assertion red with no change to any expected value, then rebuilt with
//   no flags to confirm the clean baseline returned). Demoted checks
//   (finding 1, above) carry no MUT flag any more -- a note() cannot go
//   red, so there is nothing left to mutation-test; their anti-vacuity
//   control remains CONTROL-FIXTURE-ALIVE, on the same measurement basis,
//   as it always was. One `// MUTATION` line per assertion below, second
//   token the exact expect() label (the token check_gates.py's
//   LEADING_ID_RE reads: the leading uppercase token of the label, up to
//   the first space or colon):
//
// MUTATION CONTROL-FIXTURE-ALIVE stimulus red: the second sample's index
//   was kept at 0 instead of switching to 1 (BPL1DAT stayed $0000), so the
//   "arrives in order, distinct" check saw the same COLOR00 reading twice.
//   (MUT_CONTROL)
// MUTATION VIDEO-PRIORITY-PF2PRI-LIVE stimulus red: the "PF2PRI=1" arm's
//   own BPLCON2 write never actually set bit 6, so both arms painted the
//   same playfield and pf1_front equalled pf2_front. (MUT_VIDEO_PRIORITY)
// MUTATION EHB-ACTIVE-PRECONDITION stimulus red: BPL6DAT was held at
//   $0000 instead of $FFFF for the precondition's own sample, so the
//   select index stayed 0 instead of 32, the EHB fold never engaged, and
//   the sampled pixel came back as COLOR00's raw value instead of its
//   halved value. (MUT_EHB_PRECONDITION)
// MUTATION BPLCON2-BIT9-KILLEHB-LIVE-ECS stimulus red: the "fold removed"
//   arm wrote BPLCON2 bit 10 (ZDCTEN, appendix C p.307, not implemented in
//   this design) instead of bit 9 (KILLEHB), so the EHB fold never
//   actually disengaged and the sampled pixel stayed at COLOR00's folded,
//   halved value instead of the distinct unwritten address the fold
//   redirects away from once killehb genuinely takes effect.
//   (MUT_BIT9_ECS)
// MUTATION ENBPLCN3-ENABLES-BRDRBLNK stimulus red: the "ENBPLCN3=1" arm's
//   own BPLCON0 write never actually set bit 0, so the border never
//   blanked and both arms read white. (MUT_ENBPLCN3)
//
// Baseline (no MUT_ flag defined): 5 passed, 0 failed (measured after this
// repair; see the coordinator's build report for the before/after
// baseline accounting). Every one of the 5 mutations above was built and
// run individually (`make MUT=-DMUT_<NAME>=1`) and every one produced
// exactly 4 passed, 1 failed -- only the named assertion turned red, no
// other assertion moved, and a plain `make` afterward returned to 5
// passed, 0 failed. No expected value was changed by any of them; only
// what each check asks the module to do.
//
// GATE READABILITY, closed: an earlier draft graded all twelve
//   BPLCON2-BITn-DEAD-* assertions through one shared function,
//   `assert_bit_dead()`, whose own `checks.expect(diff == 0, label + ...)`
//   call appeared exactly once in this file's text with `label` a
//   parameter, not a literal -- the "label built outside a literal" shape
//   BUILD-BRIEF warns about for a lambda, one level removed (a shared
//   function instead of a lambda), and disclosure of that gap was judged
//   not enough (a chapter-5 package was found to have reasoned the same
//   way and had it recounted, backwards, by an independent validator).
//   `measure_dead_diff()` below only measures; every call site does its
//   own note()/record() with its own string-literal label, so every
//   claim's disposition is readable from its own call site. No predicate,
//   expected value, stimulus or measurement changed by that earlier fix --
//   confirmed by rebuilding to the identical baseline afterward.
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
#ifndef MUT_VIDEO_PRIORITY
#define MUT_VIDEO_PRIORITY 0
#endif
#ifndef MUT_EHB_PRECONDITION
#define MUT_EHB_PRECONDITION 0
#endif
#ifndef MUT_BIT9_ECS
#define MUT_BIT9_ECS 0
#endif
#ifndef MUT_ENBPLCN3
#define MUT_ENBPLCN3 0
#endif

namespace {

// reg_address_in is [8:1] (word address >> 1), matching denise.v's own
// `parameter` block (denise.v:56-65) and denise_bitplanes.v:106-115.
constexpr uint16_t IDLE_ADDR    = 0x00;
constexpr uint16_t BPLCON0_ADDR = 0x80;  // 9'h100 >> 1  (denise.v:59)
constexpr uint16_t BPLCON2_ADDR = 0x82;  // 9'h104 >> 1  (denise.v:60)
constexpr uint16_t BPLCON3_ADDR = 0x83;  // 9'h106 >> 1  (denise.v:61)
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
constexpr uint16_t BIT7  = 0x0080, BIT6  = 0x0040;  // BIT6 = PF2PRI (p.210 Table 7-1)

uint32_t pack(uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | b;
}

std::string hexrgb(uint32_t rgb) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%06X", rgb & 0xFFFFFFu);
    return std::string(buf);
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
// colour-select index of 32.
std::vector<uint32_t> run_sweep(bool ecs, uint16_t bplcon2_value) {
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
        h.write_reg(BPL1DAT_ADDR, 0x0000); samples.push_back(h.current_rgb());
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

// Background measurement for BPLCON2-BITn's dead-looking-bit checks
// (repair, finding 1 -- see the header's REPAIR note): how many of THE
// RIG's 120 sampled pixels differ from a BPLCON2=$0000 baseline when only
// `bplcon2_bit` is set. Not graded here -- CONTROL-FIXTURE-ALIVE is this
// file's one anti-vacuity control, on the same measurement basis, for
// every one of these.
int measure_dead_diff(bool ecs, uint16_t bplcon2_bit) {
    auto baseline = run_sweep(ecs, 0x0000);
    auto bitset   = run_sweep(ecs, bplcon2_bit);
    return count_diff(baseline, bitset);
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
    // other measurement in this file relies on: index 0 reads COLOR00, index
    // 1 reads COLOR01, in that order, distinct. Repair (finding 6): this
    // check does not touch BPLCON2 at all, so it no longer cites
    // opus:7-vp.c16 -- that claim is now tested on its own merits by
    // VIDEO-PRIORITY-PF2PRI-LIVE below.
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
                           "control for every dead-bit measurement in this file, on the same "
                           "measurement basis.");
    }

    // =========================================================================
    // VIDEO-PRIORITY-PF2PRI-LIVE -- opus:7-vp.c16 (p.210, "You control the
    // video priorities by using the bits in BPLCON2 ... as shown in Table
    // 7-1") is testable, not merely framing (repair, finding 6:
    // SPECIFICATION.md's earlier "Not turned into assertions" reading is
    // corrected here). This claim's own classification already names the
    // concrete measurement: a dual-playfield, two-bitplane rig where PF2PRI
    // (bit 6) alone decides which playfield paints -- reproducing
    // diag/dn.cpp's T1 measurement independently on this file's own
    // harness.
    // =========================================================================
    {
        auto measure_priority = [](uint16_t bplcon2_value) -> uint32_t {
            Harness h(true);  // ecs=1 (this file's rig default; PF2PRI itself
                               // is undisputed on both chipsets, p.210/271)
            h.write_reg(BPLCON0_ADDR, 0x2400);        // bpu=2, dpf=1, ham=0 (denise.v:181-183)
            h.write_reg(BPLCON2_ADDR, bplcon2_value);  // PF2PRI = bit 6
            h.write_reg(COLOR_BASE + 1, 0x0F00);  // COLOR01: red   (playfield 1 wins, PF2PRI=0)
            h.write_reg(COLOR_BASE + 9, 0x00F0);  // COLOR09: green (playfield 2 wins, PF2PRI=1;
                                                   // denise_playfields.v:78-79, aga=0 dual-pf
                                                   // encoding forces index bit 3 set for pf2)
            h.write_reg(DIWSTRT_ADDR, 0x0030);
            h.write_reg(DIWSTOP_ADDR, 0x00A0);
            h.sample_at_hpos(0x0130);
            h.pulse_strhor();
            h.advance_hpos(0x30 - 2);
            for (int r = 0; r < 8; ++r) {
                h.write_reg(BPL2DAT_ADDR, 0xFFFF);  // playfield 2 (even bitplane)
                h.write_reg(BPL1DAT_ADDR, 0xFFFF);  // playfield 1 (odd bitplane), latches l_bpu last
            }
            return h.current_rgb();
        };

        uint16_t pf2pri_set = BIT6;
#if MUT_VIDEO_PRIORITY
        pf2pri_set = 0x0000;  // stimulus red: the "PF2PRI=1" arm never actually sets the bit
#endif
        uint32_t pf1_front = measure_priority(0x0000);
        uint32_t pf2_front = measure_priority(pf2pri_set);
        uint32_t red   = pack(0xFF, 0x00, 0x00);
        uint32_t green = pack(0x00, 0xFF, 0x00);
        checks.expect(pf1_front == red && pf2_front == green && pf1_front != pf2_front,
            "VIDEO-PRIORITY-PF2PRI-LIVE (opus:7-vp.c16; p.210, \"You control the video "
            "priorities by using the bits in BPLCON2 ... as shown in Table 7-1\"): with a "
            "fully-covered two-bitplane dual-playfield line (BPLCON0=$2400), PF2PRI=0 paints "
            "COLOR01 (0x" + hexrgb(pf1_front) + ", playfield 1 in front) and PF2PRI=1 paints "
            "COLOR09 (0x" + hexrgb(pf2_front) + ", playfield 2 in front).");
    }

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
    // BPLCON2-BIT15 -- undisputed inert-looking on both chipsets
    // (SPECIFICATION.md P1), DEMOTED from an assertion to a measurement
    // (repair, finding 1): "15-7 Not used (keep at 0)" (p.210) and "15
    // HIRES X X" (p.271) tell software what to write. The Appendix-A
    // preamble (p.264) -- "Setting unused bits ... may cause serious
    // future software incompatibility if those bits or addresses are
    // implemented in the future" -- is a forward-compatibility warning to
    // programmers, not a specification that today's hardware must ignore a
    // bit already written 1. Requiring diff==0 asserted a requirement
    // neither page makes (the same reading that withdrew six reds in
    // validation slice A).
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT15);
        checks.note("BPLCON2-BIT15-OCS-MEASURED (sol:7-video.c18, opus:7-vp.c17, "
            "sol:7-appA-bplcon2.c6, opus:7-appA-bplcon2.c5; p.210/271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
    }
    {
        int diff = measure_dead_diff(true, BIT15);
        checks.note("BPLCON2-BIT15-ECS-MEASURED (sol:7-video.c18, opus:7-vp.c17, "
            "sol:7-appA-bplcon2.c6, opus:7-appA-bplcon2.c5; p.210/271, appendix C p.307's "
            "table never reaches bit 15): diff=" + std::to_string(diff) + " of 120 sampled "
            "pixels.");
    }
    checks.record("BPLCON2-BIT15-NOT-GRADED (sol:7-video.c18, opus:7-vp.c17, "
        "sol:7-appA-bplcon2.c6, opus:7-appA-bplcon2.c5; p.210, p.264): \"Not used (keep at 0)\" "
        "and the Appendix-A preamble's future-incompatibility warning tell software what to "
        "write, not what the hardware must do with a bit already set. reason_kind "
        "source-silent. Graded until this repair; withdrawn on the independent validation's "
        "finding (slice B2, finding 1).");

    // =========================================================================
    // BPLCON2-BIT14 -- OCS half DEMOTED (repair, finding 1; SPECIFICATION.md
    // P2, same p.210/264/271 reasoning as BIT15 above). ECS half (ZDBPSEL2,
    // p.307) stays recorded, not asserted, per opus:7-appA-bplcon2.c6's own
    // conflict_resolution ("the signal does not exist in this design at
    // all"), confirmed here: bit 14 is latched at denise.v:215 but never
    // read anywhere else in denise.v or any sub-module it instantiates --
    // a different, unrelated reason from the OCS half's.
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT14);
        checks.note("BPLCON2-BIT14-OCS-MEASURED (opus:7-appA-bplcon2.c6; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
        checks.record("BPLCON2-BIT14-OCS-NOT-GRADED (opus:7-appA-bplcon2.c6; p.210, p.264, "
            "p.271): same reasoning as BPLCON2-BIT15-NOT-GRADED above -- \"keep at 0\" is not "
            "a hardware requirement to ignore a 1. reason_kind source-silent. Graded until "
            "this repair; withdrawn on the independent validation's finding (slice B2, "
            "finding 1).");
    }
    {
        auto baseline = run_sweep(true, 0x0000);
        auto bitset   = run_sweep(true, BIT14);
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
    // ZDBPSEL1. OCS half DEMOTED (repair, finding 1).
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT13);
        checks.note("BPLCON2-BIT13-OCS-MEASURED (opus:7-appA-bplcon2.c7; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
        checks.record("BPLCON2-BIT13-OCS-NOT-GRADED (opus:7-appA-bplcon2.c7; p.210, p.264, "
            "p.271): same reasoning as BPLCON2-BIT15-NOT-GRADED above. reason_kind "
            "source-silent. Graded until this repair; withdrawn on the independent "
            "validation's finding (slice B2, finding 1).");
    }
    {
        auto baseline = run_sweep(true, 0x0000);
        auto bitset   = run_sweep(true, BIT13);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT13-ECS (ZDBPSEL1, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c7's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT13-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c7; p.307): appendix C "
                       "names bit 13 ZDBPSEL1 on ECS; recorded per SPECIFICATION.md P3, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT12 -- same shape (SPECIFICATION.md P4). ECS name: ZDBPSEL0.
    // OCS half DEMOTED (repair, finding 1).
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT12);
        checks.note("BPLCON2-BIT12-OCS-MEASURED (opus:7-appA-bplcon2.c8; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
        checks.record("BPLCON2-BIT12-OCS-NOT-GRADED (opus:7-appA-bplcon2.c8; p.210, p.264, "
            "p.271): same reasoning as BPLCON2-BIT15-NOT-GRADED above. reason_kind "
            "source-silent. Graded until this repair; withdrawn on the independent "
            "validation's finding (slice B2, finding 1).");
    }
    {
        auto baseline = run_sweep(true, 0x0000);
        auto bitset   = run_sweep(true, BIT12);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT12-ECS (ZDBPSEL0, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c8's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT12-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c8; p.307): appendix C "
                       "names bit 12 ZDBPSEL0 on ECS; recorded per SPECIFICATION.md P4, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT11 -- same shape (SPECIFICATION.md P5). ECS name: ZDBPEN.
    // OCS half DEMOTED (repair, finding 1).
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT11);
        checks.note("BPLCON2-BIT11-OCS-MEASURED (opus:7-appA-bplcon2.c9; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
        checks.record("BPLCON2-BIT11-OCS-NOT-GRADED (opus:7-appA-bplcon2.c9; p.210, p.264, "
            "p.271): same reasoning as BPLCON2-BIT15-NOT-GRADED above. reason_kind "
            "source-silent. Graded until this repair; withdrawn on the independent "
            "validation's finding (slice B2, finding 1).");
    }
    {
        auto baseline = run_sweep(true, 0x0000);
        auto bitset   = run_sweep(true, BIT11);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT11-ECS (ZDBPEN, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c9's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT11-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c9; p.307): appendix C "
                       "names bit 11 ZDBPEN on ECS; recorded per SPECIFICATION.md P5, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT10 -- same shape (SPECIFICATION.md P6). ECS name: ZDCTEN.
    // OCS half DEMOTED (repair, finding 1).
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT10);
        checks.note("BPLCON2-BIT10-OCS-MEASURED (opus:7-appA-bplcon2.c10; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
        checks.record("BPLCON2-BIT10-OCS-NOT-GRADED (opus:7-appA-bplcon2.c10; p.210, p.264, "
            "p.271): same reasoning as BPLCON2-BIT15-NOT-GRADED above. reason_kind "
            "source-silent. Graded until this repair; withdrawn on the independent "
            "validation's finding (slice B2, finding 1).");
    }
    {
        auto baseline = run_sweep(true, 0x0000);
        auto bitset   = run_sweep(true, BIT10);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT10-ECS (ZDCTEN, appendix C p.307): diff=" + std::to_string(diff) +
                     " of 120 -- confirms opus:7-appA-bplcon2.c10's conflict_resolution that this "
                     "design does not implement it.");
        checks.record("BPLCON2-BIT10-ECS-NOT-ASSERTED (opus:7-appA-bplcon2.c10; p.307): appendix C "
                       "names bit 10 ZDCTEN on ECS; recorded per SPECIFICATION.md P6, not asserted.");
    }

    // =========================================================================
    // BPLCON2-BIT9 -- OCS half DEMOTED (repair, finding 1; SPECIFICATION.md
    // P7 OCS half, same p.210/264/271 reasoning as BIT15 above).
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT9);
        checks.note("BPLCON2-BIT9-OCS-MEASURED (opus:7-appA-bplcon2.c11; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
        checks.record("BPLCON2-BIT9-OCS-NOT-GRADED (opus:7-appA-bplcon2.c11; p.210, p.264, "
            "p.271): same reasoning as BPLCON2-BIT15-NOT-GRADED above. reason_kind "
            "source-silent. Graded until this repair; withdrawn on the independent "
            "validation's finding (slice B2, finding 1).");
    }

    // =========================================================================
    // BPLCON2-BIT9 -- ECS half, background sweep measurement
    // (SPECIFICATION.md P7 ECS half): the same 120-sample sweep-diff basis
    // as every other bit's measurement above. Kept as a note only --
    // repair, finding 5: a bare diff>0 postcondition on this number would
    // pass for blanking, inversion, or any unrelated perturbation of bit 9,
    // none of which is "kill halfbrite" (p.307). BPLCON2-BIT9-KILLEHB-
    // LIVE-ECS below grades the exact restored colour instead.
    // =========================================================================
    {
        auto baseline = run_sweep(true, 0x0000);
        auto bitset   = run_sweep(true, BIT9);
        int diff = count_diff(baseline, bitset);
        checks.note("BPLCON2-BIT9-ECS-SWEEP-DIFF (background measurement only -- see "
                     "BPLCON2-BIT9-KILLEHB-LIVE-ECS below for the graded postcondition): diff="
                     + std::to_string(diff) + " of 120 sampled pixels.");
        checks.note("BPLCON2-BIT9-ECS diff count against STEP3-NOTES' disclosed measurement "
                     "(115 of 120): " + std::to_string(diff) + " of 120 -- reproduced independently "
                     "on this file's own rig, not adopted.");
    }

    // =========================================================================
    // BPLCON2-BIT9 -- ECS half asserted LIVE (SPECIFICATION.md P7 ECS half,
    // the package's centerpiece): KILLEHB kills Extra Half-Brite. REPAIRED
    // (finding 5) to grade an exact colour pair rather than "some
    // difference": same index-32 rig as EHB-ACTIVE-PRECONDITION above.
    // MEASURED WHILE FIXING (not adopted from any prior stage): killehb is
    // not a simple "restore COLOR00 unhalved" switch on this rig. The fold
    // multiplexer (denise_colortable.v:34) reads two DIFFERENT addresses
    // depending on killehb -- `ehb_en` ? {3'b0, select[4:0]} : select -- so
    // at select=32 (this rig's fixed index), killehb=0 folds down to
    // address 0 (COLOR00) and halves it, while killehb=1 reads the raw,
    // unfolded address 32 directly. denise_colortable.v:30's write address
    // is masked to 5 bits when aga=0 (`wr_adr = {bank[2:0],
    // reg_address_in[5:1]}`, bank forced 0), so register writes can never
    // reach RAM address 32 -- it is a real, distinct, 256-entry-deep table
    // cell (denise_colortable_ram_mf.v:12-13) that this rig never writes,
    // reading its zero-initialised default. A first draft of this
    // assertion wrongly expected COLOR00 restored unhalved and read
    // 0x000000 where it expected 0xFF00FF; that was this package's own
    // addressing mistake, not an RTL defect (killehb=0's halved COLOR00
    // reading, 0x7F007F, matched exactly). Fixed below to grade what the
    // hardware actually does: killehb=0 reads COLOR00 folded-and-halved;
    // killehb=1 reads the distinct, unwritten address 32 -- the automatic
    // half-brite substitution demonstrably stops, which is what "kill
    // halfbrite" (p.307) means.
    // =========================================================================
    {
        auto sample_with = [](uint16_t bplcon2_value) -> uint32_t {
            Harness h(true);  // ecs=1
            h.write_reg(BPLCON0_ADDR, 0x6000);   // bpu=6, ham=0, dpf=0 (THE RIG)
            h.write_reg(BPLCON2_ADDR, bplcon2_value);
            h.write_reg(COLOR_BASE + 0, 0x0F0F);  // COLOR00 -> bytes FF,00,FF
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
                h.write_reg(BPL6DAT_ADDR, 0xFFFF);   // index 32 (the rig's fixed select index)
                h.write_reg(BPL1DAT_ADDR, 0x0000);
            }
            return h.current_rgb();
        };

        uint16_t kill_bit = BIT9;
#if MUT_BIT9_ECS
        kill_bit = BIT10;  // ZDCTEN, not implemented -- EHB never actually gets killed
#endif
        uint32_t halved       = sample_with(0x0000);
        uint32_t fold_removed = sample_with(kill_bit);
        uint32_t expect_halved = pack(0x7F, 0x00, 0x7F);
        // WHAT p.307 REQUIRES, AND WHAT IT DOES NOT. "Kill halfbrite" requires
        // the automatic half-brite substitution to STOP. It says nothing about
        // the colour that appears instead, so the colour is measured and
        // reported and NOT graded. An earlier repair graded killehb=1 ==
        // 0x000000, which is this harness's RAM address 32 -- unwritten because
        // denise masks the write address to five bits at aga=0, so no register
        // write can reach it. That is a fact about the fixture, and grading it
        // is the same defect the independent validation found in eight SPR-*
        // assertions in ch7-inter-sprite-priority: an expected value taken from
        // the implementation rather than from the page.
        checks.expect(halved == expect_halved && fold_removed != expect_halved,
            "BPLCON2-BIT9-KILLEHB-LIVE-ECS (sol:7-appC-bplcon2.c5, opus:7-appC-bplcon2.c7; "
            "p.307, \"09 KILLEHB Kill halfbrite\"; co-cited sol:7-appA-bplcon2.c4 p.271 and "
            "sol:7-appB-bplcon2.c5 p.291, the (E) mark): killehb=0 reads 0x" + hexrgb(halved) +
            " (expected 0x7F007F, COLOR00 folded and halved) and killehb=1 reads 0x"
            + hexrgb(fold_removed) + ", which is NOT the halved colour -- the automatic "
            "half-brite substitution demonstrably stops when killehb is set. The colour "
            "that appears instead is measured, not graded: p.307 does not name one, and on "
            "this fixture it is RAM address 32, which no register write can reach.");
    }

    // =========================================================================
    // BPLCON2-BIT8 -- undisputed inert-looking on both chipsets within this
    // package's scope (SPECIFICATION.md P8), DEMOTED (repair, finding 1;
    // same p.210/264/271 reasoning as BIT15 above). Gated by AGA, not ECS
    // (denise.v:219 `rdram = bplcon2[8] & aga`), and aga=0 throughout this
    // file.
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT8);
        checks.note("BPLCON2-BIT8-OCS-MEASURED (opus:7-appA-bplcon2.c12; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
    }
    {
        int diff = measure_dead_diff(true, BIT8);
        checks.note("BPLCON2-BIT8-ECS-MEASURED (opus:7-appA-bplcon2.c12; p.271, aga=0 "
            "throughout this file -- bit 8 is gated by aga, not ecs): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
    }
    checks.record("BPLCON2-BIT8-NOT-GRADED (opus:7-appA-bplcon2.c12; p.210, p.264, p.271): "
        "same reasoning as BPLCON2-BIT15-NOT-GRADED above. reason_kind source-silent. Graded "
        "until this repair; withdrawn on the independent validation's finding (slice B2, "
        "finding 1).");

    // =========================================================================
    // BPLCON2-BIT7 -- undisputed inert-looking on both chipsets
    // (SPECIFICATION.md P9), DEMOTED (repair, finding 1; same
    // p.210/264/271 reasoning as BIT15 above).
    // =========================================================================
    {
        int diff = measure_dead_diff(false, BIT7);
        checks.note("BPLCON2-BIT7-OCS-MEASURED (opus:7-appA-bplcon2.c13; p.271): diff="
            + std::to_string(diff) + " of 120 sampled pixels.");
    }
    {
        int diff = measure_dead_diff(true, BIT7);
        checks.note("BPLCON2-BIT7-ECS-MEASURED (opus:7-appA-bplcon2.c13; p.271, appendix C "
            "p.307's table never reaches bit 7): diff=" + std::to_string(diff) + " of 120 "
            "sampled pixels.");
    }
    checks.record("BPLCON2-BIT7-NOT-GRADED (opus:7-appA-bplcon2.c13; p.210, p.264, p.271): "
        "same reasoning as BPLCON2-BIT15-NOT-GRADED above. reason_kind source-silent. Graded "
        "until this repair; withdrawn on the independent validation's finding (slice B2, "
        "finding 1).");

    // =========================================================================
    // ENBPLCN3-ENABLES-BRDRBLNK -- opus:7-appC-bplcon2.c9 (p.307, "00
    // ENBPLCN3 ... Enable new BLPCON3 register"), PROMOTED from a decline
    // to a graded assertion (repair, finding 7): the same p.307 table
    // already used for KILLEHB also defines BRDRBLNK ("05 BRDRBLNK Border
    // blank"), so a BPLCON3 field IS quoted in this package's claims after
    // all -- the earlier decline's premise was wrong. At the opened
    // denise.v boundary RGB is exposed, and BRDRBLNK (bplcon3[5]) is itself
    // gated by ENBPLCN3 (denise.v:192 ecsena=bplcon0[0]; :253
    // brdrblnk=bplcon3[5]&ecsena; :515 t_blank=ecs&ecsena&brdrblnk&
    // (~window_del|~display_ena); :518 red/green/blue forced to 0 when
    // t_blank), giving a graded enable/disable experiment without adding a
    // debug port. This does not test BPLCON3's other fields (bank, pf2of,
    // loct, spres, brdsprt) -- only the ENBPLCN3 gate itself, via the one
    // field p.307 already puts on this page.
    // =========================================================================
    {
        auto sample_border = [](bool enbplcn3) -> uint32_t {
            Harness h(true);  // ecs=1 (p.307 is the Appendix C / ECS account)
            h.write_reg(BPLCON0_ADDR, enbplcn3 ? 0x0001 : 0x0000);  // ENBPLCN3 = bit 0
            h.write_reg(BPLCON3_ADDR, 0x0020);                       // BRDRBLNK = bit 5
            h.write_reg(COLOR_BASE + 0, 0x0FFF);  // COLOR00 (border colour): white
            h.write_reg(DIWSTRT_ADDR, 0x0030);
            h.write_reg(DIWSTOP_ADDR, 0x00A0);
            h.sample_at_hpos(0x0130);  // flush any latched 'window' reg garbage (denise.v:328-334)
            h.pulse_strhor();          // hpos -> 2, before hdiwstrt(0x30): the border region
            return h.current_rgb();
        };

        bool inject = false;
#if MUT_ENBPLCN3
        inject = true;
#endif
        uint32_t border_off = sample_border(false);
        uint32_t border_on  = sample_border(inject ? false : true);  // injected: ENBPLCN3 never set

        uint32_t white = pack(0xFF, 0xFF, 0xFF);
        uint32_t black = pack(0x00, 0x00, 0x00);
        checks.expect(border_off == white && border_on == black,
            "ENBPLCN3-ENABLES-BRDRBLNK (opus:7-appC-bplcon2.c9; p.307, \"00 ENBPLCN3 ... "
            "Enable new BLPCON3 register\", tested via the same page's \"05 BRDRBLNK Border "
            "blank\"): with BPLCON3 bit 5 fixed at 1 and a border-region pixel sampled "
            "(COLOR00 written white), ENBPLCN3=0 reads 0x" + hexrgb(border_off) +
            " and ENBPLCN3=1 reads 0x" + hexrgb(border_on) + ".");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
