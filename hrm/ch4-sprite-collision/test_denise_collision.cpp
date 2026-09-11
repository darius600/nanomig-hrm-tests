// ch4-sprite-collision -- denise_collision conformance test
//
// Test purpose: what Chapter 4 itself states about sprite collision, on
//   printed page 113 -- as distinct from Appendix A's CLXCON/CLXDAT
//   bit-by-bit table, which hrm/ch3-collision-registers/ already tests at
//   this same module boundary. Package ch4-sprite-collision, fixture
//   denise_collision (one Verilator build, this module only -- the same
//   fixture hrm/denise_collision/ and hrm/ch3-collision-registers/ already
//   build).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p. 113.
// Page note: the build task that assigned this package also named page
//   129. This package's own assigned claims file (ch4-sprite-collision.
//   jsonl, 5 records, read in full) cites only page 113 in every record;
//   none of the 5 draws from page 129, so nothing about page 129 is
//   asserted here -- checked against the claims file itself, not assumed.
// Non-goals: this file does not retest Appendix A's CLXCON/CLXDAT register
//   identity, address, write-only/read-clear semantics, the per-bitplane
//   MVBP/ENBP mechanism, the six-bitplane-disabled NOTE, the ENSP odd/even
//   sprite OR, the full 15-entry CLXDAT bit/pair table, or bit 15 -- all
//   98 of those Appendix A propositions are hrm/ch3-collision-registers/'s
//   own inventory and are already asserted there, on their own page
//   citations (pp. 272, 273, 289, 290), not chapter 4's. It does not
//   assert "you can use the hardware to detect these collisions and
//   modify the display in some way" beyond the bare detection capability
//   chapter 4's own page 113 states -- the elaboration of how, and what
//   "modify the display" means, is cross-referenced to Chapter 7 by the
//   manual itself (see DECLINED CLAIMS below) and is not this package's
//   to assert.
//
// CLAIM SOURCE: this package's own assigned claims file is
//   C:\code4nanomig\code\hrm-testpipeline\work\ch4_packages\
//   ch4-sprite-collision.jsonl (5 records, all page 113): CH4-coll.c1,
//   CH4-coll.c2, CH4-coll.c3, CH4-p113.c5, CH4-p113.c6.
//
// BOUNDARY: denise_collision.v, reached at
//   $(NANOMIG)/src/minimig-aga/denise_collision.v. NOT opened by this
//   agent for this commit.
//
// ORDER OF WORK: this file -- every assertion, its expected value, the
//   claim ids it cites, and the stimulus each measurement uses -- was
//   written and committed from claims.jsonl's own quotations plus two
//   already-committed sibling harnesses' disclosed interface for this
//   exact fixture: hrm/denise_collision/test_denise_collision.cpp (the
//   original FPGA-031 harness -- port list, register word-address
//   convention $098->0x4C/$00E->0x07, clk/clk7_en/reset/aga/
//   reg_address_in/data_in/dblpf/bpldata/nsprite/data_out, bpldata as an
//   8-bit per-bitplane hit vector, nsprite as an 8-bit per-sprite hit
//   vector) and hrm/ch3-collision-registers/test_denise_collision.cpp
//   (the HRM-format harness for the same module, which documents this
//   same reuse in its own ORDER OF WORK section, and additionally
//   discloses the CLXCON bit-position helpers (MVBP/ENBP), the CLXDAT
//   bit->collision-pair table, the pulse()/write_reg()/read_reg() timing
//   convention, and a harness-hygiene drain-read needed because clxdat
//   carries no reset branch of its own). denise_collision.v itself was
//   not opened by this agent to obtain any of this -- reusing an
//   already-committed sibling harness's disclosed interface is the
//   precedent hrm/ch4-attached-sprites/ already relies on for this same
//   gate, and is followed the same way here.
//
// ALREADY-COVERED BOUNDARY: hrm/ch3-collision-registers/ already records
//   a finding at exactly the playfield/sprite collision boundary this
//   package's own CH4-coll.c2 touches -- NM-HRM-008 in docs/findings.md,
//   "playfield-1 collision depends on playfield 2" (red), scoped to
//   single-playfield mode (dblpf held at its power-up default, 0,
//   throughout that package's own 98 assertions -- see its own
//   Non-goals). Per this package's own build task instruction (not
//   discovered by this agent, and not by opening denise_collision.v):
//   that finding is not this package's to restate as new.
//   SPRITE-TO-PLAYFIELD-COLLISION-EITHER below is therefore measured in
//   DUAL-playfield mode (dblpf=1) -- the mode chapter 4's own "either of
//   the two playfields" phrase most naturally describes (two
//   independently present playfields, each its own collision partner),
//   and precisely the mode hrm/ch3-collision-registers/ left untested.
//   If a single-playfield-mode (dblpf=0) measurement of the same
//   proposition is ever wanted, it lands on the already-recorded
//   boundary above, not a fresh one.
//
// THE THREE PROPOSITIONS UNDER TEST, IN THE MANUAL'S OWN WORDS (p.113):
//   "As sprites move about on the screen, they can collide with each
//   other or with either of the two playfields. You can use the hardware
//   to detect [these collisions and modify the display in some way]."
//   Read as three separate, individually-quoted propositions (CH4-coll.
//   c1, c2, c3 -- see CLAIM SOURCE): (1) sprites can collide with each
//   other, (2) sprites can collide with either playfield, (3) the
//   hardware -- the register interface -- can be used to detect that a
//   collision happened. Each is asserted below as its own proposition, on
//   its own measurement basis, matching this package's own classify
//   block ("observable, yes, at denise_collision").
//
// MEASUREMENT BASIS (SPRITE-TO-SPRITE-COLLISION-OCCURS, CH4-coll.c1):
//   sprite 0 presented alone (nsprite bit 0) versus sprite 0 presented
//   together with sprite 2 (nsprite bits 0 and 2, the same pixel cycle)
//   -- both cases with bpldata held at 0 so, under CLXCON's own power-up
//   default (every bitplane enabled with match=1 required, disclosed by
//   hrm/ch3-collision-registers/'s own harness-hygiene note), both
//   playfields stay definitively mismatched/absent throughout, isolating
//   the sprite-pair bits this assertion actually measures. "Sprite 0
//   alone" must register no SPR01-SPR23 collision; "sprite 0 together
//   with sprite 2" must register one -- two sprites have to actually be
//   present together to collide "with each other".
//
// MEASUREMENT BASIS (SPRITE-TO-PLAYFIELD-COLLISION-EITHER, CH4-coll.c2):
//   dual-playfield mode (dblpf=1, see ALREADY-COVERED BOUNDARY above).
//   Two independent legs, each its own fresh, freshly-reset harness: (a)
//   only bitplane 1 (odd, playfield 1) enabled+required in CLXCON, its
//   own live bit presented as a match, sprite 0 presented at the same
//   cycle; (b) only bitplane 2 (even, playfield 2) enabled+required,
//   its own live bit matched, sprite 0 presented. Each leg must show
//   sprite 0 colliding with the one playfield under test.
//
// MEASUREMENT BASIS (COLLISION-DETECTABLE-VIA-REGISTER-READ, CH4-coll.
//   c3): a single harness, read once with nothing presented (idle,
//   after the constructor's own drain read) and again after sprite 4 and
//   sprite 6 are presented together (a genuine SPR45-SPR67 collision,
//   deliberately a fresh sprite pair, not the one CH4-coll.c1 already
//   used, so this is not a bare repeat of that measurement). The idle
//   read must show nothing registered; the post-collision read must show
//   a changed, non-zero value at the expected bit -- proving a collision
//   is something software can find out about by reading CLXDAT, not only
//   something that happens on screen.
//
// DECLINED CLAIMS (CH4-p113.c5, CH4-p113.c6): both were already flagged
//   defective by this package's own cross-read step before this agent
//   ever saw them -- classify.crossread.verdict "defect",
//   defect ["restatement_exceeds_quote"] -- because each restatement
//   ("collision hardware can detect overlap between sprites" / "...
//   between a sprite and either playfield") rests on the quoted sentence
//   "As sprites move about on the screen, they can collide with each
//   other or with either of the two playfields," which states only that
//   collision HAPPENS, not that hardware DETECTS it. The detection
//   capability is the manual's OWN NEXT sentence, "You can use the
//   hardware to detect these collisions [and modify the display in some
//   way]" -- not quoted by c5 or c6 -- and per this package's own
//   crossread note, that sentence's own elaboration is cross-referenced
//   to Chapter 7. This package does not assert c5 or c6 as their own
//   propositions: the "sprites overlap each other" half of each
//   restatement is already asserted, on solid quote ground, as
//   CH4-coll.c1 above (c5) / CH4-coll.c2 above (c6); the "hardware can
//   detect" half is already asserted, on solid quote ground, as
//   CH4-coll.c3 above (both); and the "modify the display" elaboration
//   neither restatement actually reaches is Chapter 7's, not this
//   package's, to assert. Recorded, not asserted -- matching
//   hrm/ch4-attached-sprites/'s own convention for a declined claim
//   (checks.record(), no checks.expect()).
//
// CLAIM COVERAGE -- all 5 records of this package's own claims.jsonl
//   cited in full inside an expect()/record() label:
//     SPRITE-TO-SPRITE-COLLISION-OCCURS        -- CH4-coll.c1
//     SPRITE-TO-PLAYFIELD-COLLISION-EITHER     -- CH4-coll.c2
//     COLLISION-DETECTABLE-VIA-REGISTER-READ   -- CH4-coll.c3
//     SPRITE-OVERLAP-DETECT-DECLINED [declined]     -- CH4-p113.c5
//     PLAYFIELD-OVERLAP-DETECT-DECLINED [declined]  -- CH4-p113.c6
//   3 graded + 2 declined = 5.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id, labelled
//   "[control]" so the mutation gate does not require a claim id for it)
//   proves the write -> pulse -> read measurement basis every graded
//   assertion above shares actually delivers distinct, correctly
//   positioned values before any of them is trusted -- per the build
//   brief's own chapter-6 warning that a silently dead data path reports
//   every data-dependent assertion as a finding about the design instead
//   of the harness. Two distinct, known sprite-pair stimuli must produce
//   two distinct, correctly-positioned CLXDAT readings.
//
// RESULTS: PENDING -- this file is committed before denise_collision.v
//   has been opened or built, per the build brief's gate ("write the
//   specification, commit it, only then open the RTL"). Filled in by a
//   follow-up commit after the WSL/Verilator build and the mutation runs
//   below.
//
// MUTATION LOG: PENDING -- filled in by the same follow-up commit, after
//   each mutation below is actually run (edit -> rebuild -> run -> confirm
//   only the named assertion changed colour -> revert -> rebuild ->
//   confirm baseline restored), not asserted in advance. Planned, one
//   stimulus change per graded assertion plus the control, expected zero
//   change to any expected value or comparison logic:
//     FIXTURE-LIVE-DATA-PATH: repeat the first known stimulus instead of
//       presenting a genuinely different second one -- expect only this
//       assertion to go red.
//     SPRITE-TO-SPRITE-COLLISION-OCCURS: withhold sprite 2 from the
//       "together" measurement (sprite 0 stays alone) -- expect only this
//       assertion to go red.
//     SPRITE-TO-PLAYFIELD-COLLISION-EITHER: withhold bitplane 1's own
//       live-bit stimulus in leg (a) -- expect only this assertion to go
//       red.
//     COLLISION-DETECTABLE-VIA-REGISTER-READ: withhold the sprite 4 +
//       sprite 6 stimulus from the "after" measurement -- expect only
//       this assertion to go red.
//   SPRITE-OVERLAP-DETECT-DECLINED and PLAYFIELD-OVERLAP-DETECT-DECLINED
//   are declined (record(), no expect()) -- no red/green state exists for
//   a mutation to move, matching hrm/ch4-attached-sprites/'s own
//   convention for its declined claim.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vdenise_collision.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_COLL
#define MUT_COLL 0
#endif
#ifndef MUT_PF
#define MUT_PF 0
#endif
#ifndef MUT_DETECT
#define MUT_DETECT 0
#endif

namespace {

constexpr int CLXCON_ADDR = 0x4C;  // $098 >> 1
constexpr int CLXDAT_ADDR = 0x07;  // $00E >> 1
constexpr int IDLE_ADDR = 0x00;    // outside both decoded addresses.

// CLXCON bit positions (disclosed by hrm/ch3-collision-registers/'s own
// harness for this same module): MVBPn at bit (n-1), ENBPn at bit
// (6+n-1).
constexpr uint16_t MVBP(int n) { return static_cast<uint16_t>(1u << (n - 1)); }
constexpr uint16_t ENBP(int n) { return static_cast<uint16_t>(1u << (6 + n - 1)); }

class Harness {
public:
    Harness() : dut(new Vdenise_collision) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->aga = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->dblpf = 0;
        dut->bpldata = 0;
        dut->nsprite = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        // Drain: disclosed by hrm/ch3-collision-registers/'s own harness --
        // clxdat carries no reset branch of its own, and on the very first
        // simulated edge clxcon still reads as Verilator's power-on 0x0000
        // (before its own reset assignment has propagated), which can latch
        // a stray bit into clxdat before clxcon reaches its real reset
        // value. One read clears it. Harness hygiene, re-included here
        // because a fix does not travel with a copied file -- this file's
        // own assertions are what prove it still holds.
        read_reg(CLXDAT_ADDR);
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

    void tick_7mhz() {
        dut->clk7_en = 1; tick();
        dut->clk7_en = 0; tick(); tick(); tick();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick_7mhz();
    }

    uint16_t read_reg(int addr) {
        dut->reg_address_in = addr;
        tick_7mhz();
        uint16_t out = dut->data_out;
        dut->reg_address_in = IDLE_ADDR;
        tick_7mhz();
        return out;
    }

    // Presents bpldata/nsprite for exactly one 7MHz tick, then clears both.
    void pulse(uint8_t bpl, uint8_t spr) {
        dut->bpldata = bpl;
        dut->nsprite = spr;
        tick_7mhz();
        dut->bpldata = 0;
        dut->nsprite = 0;
    }

    Vdenise_collision* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // For records that state what the SOURCE says, or why a claim is
    // declined. Not a measurement -- must never be printed as though it
    // were one.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- FIXTURE-LIVE-DATA-PATH [control] ----------------
    {
        Harness h;
        h.pulse(0x00, 0x05);  // SPR0 + SPR2 -> SPR01-SPR23, bit 9
        uint16_t a = h.read_reg(CLXDAT_ADDR);
        uint8_t second_stim = 0x11;  // SPR0 + SPR4 -> SPR01-SPR45, bit 10
#if MUT_CONTROL
        second_stim = 0x05;  // MUTATION: repeat the first pair instead of a genuinely different one
#endif
        h.pulse(0x00, second_stim);
        uint16_t b = h.read_reg(CLXDAT_ADDR);
        checks.expect(
            (a & 0x7FFF) == 0x0200 && (b & 0x7FFF) == 0x0400 && a != b,
            "[control] FIXTURE-LIVE-DATA-PATH: two distinct, known "
            "sprite-collision stimuli (SPR0+SPR2, then SPR0+SPR4) produce "
            "two distinct, correctly-positioned CLXDAT readings ($0200 "
            "then $0400) through write/pulse/read, before any assertion "
            "below trusts that path. No manual page: the control for "
            "every assertion in this file, on the same measurement "
            "basis.");
    }

    // ---------------- SPRITE-TO-SPRITE-COLLISION-OCCURS ----------------
    {
        Harness h;
        h.pulse(0x00, 0x01);  // SPR0 alone
        uint16_t alone = h.read_reg(CLXDAT_ADDR);

        uint8_t together_spr = 0x05;  // SPR0 + SPR2
#if MUT_COLL
        together_spr = 0x01;  // MUTATION: SPR2 withheld, SPR0 stays alone
#endif
        h.pulse(0x00, together_spr);
        uint16_t together = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (alone & 0x0200) == 0 && (together & 0x0200) != 0,
            "SPRITE-TO-SPRITE-COLLISION-OCCURS: presenting sprite 0 alone "
            "registers no SPR01-SPR23 collision (CLXDAT bit 9 clear); "
            "presenting sprite 0 together with sprite 2, the same pixel "
            "cycle, sets that bit -- two sprites must both be present to "
            "collide 'with each other'. 'As sprites move about on the "
            "screen, they can collide with each other or' (p.113). "
            "CH4-coll.c1.");
    }

    // ------------- SPRITE-TO-PLAYFIELD-COLLISION-EITHER -------------
    // Dual-playfield mode (dblpf=1); see ALREADY-COVERED BOUNDARY above
    // for why single-playfield mode is deliberately not measured here.
    {
        Harness ha;
        ha.dut->dblpf = 1;
        ha.write_reg(CLXCON_ADDR, ENBP(1) | MVBP(1));  // BP1 (odd, PF1) enabled+required
        ha.read_reg(CLXDAT_ADDR);  // drain whatever accumulated before this config took effect
        uint8_t bp1_live = 0x01;   // bitplane 1's own live bit
#if MUT_PF
        bp1_live = 0x00;  // MUTATION: bitplane 1's own live bit withheld
#endif
        ha.pulse(bp1_live, 0x01);  // BP1 matched, SPR0 present
        uint16_t pf1_hit = ha.read_reg(CLXDAT_ADDR);

        Harness hb;
        hb.dut->dblpf = 1;
        hb.write_reg(CLXCON_ADDR, ENBP(2) | MVBP(2));  // BP2 (even, PF2) enabled+required
        hb.read_reg(CLXDAT_ADDR);
        hb.pulse(0x02, 0x01);  // BP2 matched (its own live bit), SPR0 present
        uint16_t pf2_hit = hb.read_reg(CLXDAT_ADDR);

        checks.expect(
            (pf1_hit & 0x0002) != 0 && (pf2_hit & 0x0020) != 0,
            "SPRITE-TO-PLAYFIELD-COLLISION-EITHER: measured in "
            "dual-playfield mode (dblpf=1), sprite 0 registers a "
            "collision against playfield 1 (bitplane 1 alone enabled and "
            "matched -> CLXDAT bit 1) and, independently, against "
            "playfield 2 (bitplane 2 alone enabled and matched -> CLXDAT "
            "bit 5): sprites collide with EITHER of the two playfields, "
            "not only one of them. 'with either of the two playfields. "
            "You can use the hardware to detect' (p.113). CH4-coll.c2.");
    }

    // ------------- COLLISION-DETECTABLE-VIA-REGISTER-READ -------------
    {
        Harness h;
        uint16_t idle = h.read_reg(CLXDAT_ADDR);  // quiescent: nothing presented yet

        uint8_t collision_spr = 0x50;  // SPR4 + SPR6 -> SPR45-SPR67, bit 14
#if MUT_DETECT
        collision_spr = 0x00;  // MUTATION: the collision withheld -- nothing presented
#endif
        h.pulse(0x00, collision_spr);
        uint16_t after = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (idle & 0x7FFF) == 0 && (after & 0x7FFF) == 0x4000 && idle != after,
            "COLLISION-DETECTABLE-VIA-REGISTER-READ: a CLXDAT read taken "
            "before any sprite is presented shows no collision "
            "registered; the SAME register, read again after sprite 4 "
            "and sprite 6 are presented together (a genuine SPR45-SPR67 "
            "collision), returns a changed, non-zero value -- a "
            "collision is something software can find out about by "
            "reading the hardware, not only something that happens on "
            "screen. 'with either of the two playfields. You can use the "
            "hardware to detect' (p.113). CH4-coll.c3.");
    }

    // --------- SPRITE-OVERLAP-DETECT-DECLINED [declined] ---------
    checks.record(
        "SPRITE-OVERLAP-DETECT-DECLINED [declined]: this package's own "
        "crossread step already flagged CH4-p113.c5 defective before this "
        "agent saw it (verdict 'defect', restatement_exceeds_quote) -- "
        "its restatement 'Collision hardware can detect overlap between "
        "sprites' rests on the quote 'As sprites move about on the "
        "screen, they can collide with each other or with either of the "
        "two playfields' (p.113), which states only that collision "
        "happens, not that hardware detects it; the detection sentence "
        "was not quoted by this record and is cross-referenced to "
        "Chapter 7. The 'sprites overlap each other' half is already "
        "asserted on solid quote ground as SPRITE-TO-SPRITE-COLLISION-"
        "OCCURS (CH4-coll.c1) above; the 'hardware can detect' half is "
        "already asserted as COLLISION-DETECTABLE-VIA-REGISTER-READ "
        "(CH4-coll.c3) above. Not asserted separately. CH4-p113.c5.");

    // --------- PLAYFIELD-OVERLAP-DETECT-DECLINED [declined] ---------
    checks.record(
        "PLAYFIELD-OVERLAP-DETECT-DECLINED [declined]: as "
        "SPRITE-OVERLAP-DETECT-DECLINED above -- this package's own "
        "crossread step already flagged CH4-p113.c6 defective before "
        "this agent saw it (verdict 'defect', restatement_exceeds_quote) "
        "-- its restatement 'Collision hardware can detect overlap "
        "between a sprite and either playfield' rests on the same "
        "'sprites can collide' quote, not the (unquoted, Chapter-7-"
        "cross-referenced) detection sentence. The 'sprite overlaps a "
        "playfield' half is already asserted on solid quote ground as "
        "SPRITE-TO-PLAYFIELD-COLLISION-EITHER (CH4-coll.c2) above; the "
        "'hardware can detect' half is already asserted as "
        "COLLISION-DETECTABLE-VIA-REGISTER-READ (CH4-coll.c3) above. Not "
        "asserted separately. CH4-p113.c6.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
