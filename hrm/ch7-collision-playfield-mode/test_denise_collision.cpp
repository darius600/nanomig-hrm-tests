// ch7-collision-playfield-mode -- denise_collision conformance test
//
// Test purpose: the one comparison no existing package measures -- whether
//   denise_collision's collision result (CLXDAT) is unaffected by the
//   single-/dual-playfield mode select (dblpf), and whether the playfield-1/
//   playfield-2 parity mapping (odd bitplanes / even bitplanes) holds when
//   both playfields are genuinely, distinguishably populated at once.
//   Package ch7-collision-playfield-mode, fixture denise_collision (one
//   Verilator build, this module only, per
//   AUFTRAG-ch7-collision-playfield-mode.md).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.214 (chapter 7 body) and p.273 (Appendix A, CLXDAT NOTE). Claims file
//   ch7-collision-playfield-mode.jsonl, 8 records. A red result below means
//   denise_collision.v disagrees with the manual, not that the assertion is
//   wrong -- no assertion here was, or should be, adjusted to make the RTL
//   pass.
// Non-goals: this package does not re-litigate CLXCON/CLXDAT register
//   identity, write-only/read-clear behaviour, or the full 15-pair bitmap --
//   ch3-collision-registers already covers those (with dblpf pinned at 0,
//   its own stated non-goal). This package's only subject is the mode
//   dimension ch3 and ch4 between them never compared, plus the parity
//   mapping re-measured under a non-vacuous, both-playfields-live condition.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: nanomig/src/minimig-aga/denise_collision.v, ports clk, clk7_en,
//   reset, aga, reg_address_in, data_in, data_out, dblpf, bpldata, nsprite
//   (denise_collision.v:4-16); CLXCON $098, CLXCON2 $10e, CLXDAT $00e
//   (:19-21).
//
// ORDER OF WORK: SPECIFICATION.md (this package's propositions, test intent,
// control-case and mutation plan) was written and committed from
// ch7-collision-playfield-mode.jsonl's quotations alone -- commit b2589af --
// before denise_collision.v was opened. The harness's port names and
// clocking mechanics (clk/clk7_en toggling, reg_address_in/data_in
// sequencing, the $098->0x4C/$00E->0x07 word-address convention, bpldata as
// an 8-bit per-bitplane hit vector, nsprite as an 8-bit per-sprite hit
// vector, the CLXCON MVBP/ENBP/ENSP bit layout, and the CLXDAT bit->pair
// table) are reused verbatim from hrm/ch3-collision-registers/
// test_denise_collision.cpp -- an already-committed, already-built sibling
// harness for the SAME module -- not learned by opening this package's own
// forbidden-until-committed file. That sibling's own committed finding
// (oddmatch gated by evenmatch, denise_collision.v:62-63, "an undocumented
// function in the real Denise chip ... trigger was the game Rotor") is this
// package's reason to exist and is treated below as another reader's
// measurement to reproduce at this package's own boundary (the mode sweep
// ch3 explicitly declined), not as an expected value adopted in advance.
//
// CLXDAT bit->pair table (from ch3-collision-registers, A-clxdat.b00..b14,
// p.273): 0 PF1-PF2, 1 PF1-SPR01, 2 PF1-SPR23, 3 PF1-SPR45, 4 PF1-SPR67,
// 5 PF2-SPR01, 6 PF2-SPR23, 7 PF2-SPR45, 8 PF2-SPR67, 9 SPR01-SPR23,
// 10 SPR01-SPR45, 11 SPR01-SPR67, 12 SPR23-SPR45, 13 SPR23-SPR67,
// 14 SPR45-SPR67, 15 not used (hard-wired high on every successful CLXDAT
// read, per ch3's BIT15-UNUSED finding -- orthogonal here, masked with
// & 0x7FFF throughout, not re-measured by this package).
//
// CLAIM COVERAGE: all 8 records of ch7-collision-playfield-mode.jsonl are
// cited below, in full, inside expect() labels. Two readers (sol, opus)
// independently quote each of four sentences; both uids are cited together
// on the one assertion (or assertion group) that tests that sentence, per
// the pairing the claims file itself records explicitly for the first pair
// (opus:7-cd.c26's "twin" field naming sol:7-collision.c27) and by identical
// quote/page for the other three pairs.
//
//   A1 PLAYFIELD-MODE-INVARIANCE (p.214, sol:7-collision.c27 +
//      opus:7-cd.c26) -- "collision detection does not change when you
//      select either single- or dual-playfield mode". Swept across all
//      fifteen CLXDAT pairs: one relation per pair, CLXDAT(pair, dblpf=0) ==
//      CLXDAT(pair, dblpf=1), captured and compared -- never a captured
//      value compared against a baked-in constant.
//   A2 BITPLANE-CONTENT-DETERMINES-COLLISION (p.214, sol:7-collision.c28 +
//      opus:7-cd.c27) -- "Collision detection depends only on the actual
//      bits present in the odd-numbered or even-numbered bitplanes". A1
//      already covers the exclusivity half (mode carries no information);
//      A2 covers the complementary, non-redundant half -- holding dblpf
//      fixed, changing bitplane content changes the result.
//   A3 PLAYFIELD-1-IS-ODD-BITPLANES (p.273, sol:7-appA-clxdat.c6 +
//      opus:7-appA-clxdat.c3) -- re-measured with BOTH playfields enabled
//      and match-required at once (never disabled -> never vacuously
//      matched), swept across dblpf, closing the gap in
//      ch3-collision-registers's PLAYFIELD-ODD-EVEN group (one bitplane
//      enabled at a time, the opposite playfield disabled and therefore
//      vacuously present regardless of its actual content).
//   A4 PLAYFIELD-2-IS-EVEN-BITPLANES (p.273, sol:7-appA-clxdat.c7 +
//      opus:7-appA-clxdat.c4) -- mirrors A3 for the even/PF2 side.
//   4 uid-pairs, 8 claims, none declined.
//
// CONTROL CASES: HARNESS-DATA-PATH-ALIVE (the fixture can deliver two
// distinct, known collision outcomes, in order, before anything below
// trusts it -- carries no claim id, graded as expect() because it can fail).
// A1's own loop supplies its control on the same measurement basis: 11 of
// its 15 pairs are expected to hold (equal across dblpf) using the exact
// same comparator as the 4 that do not -- a passing majority on one side of
// the same mechanism that finds the minority red, not a separately-invented
// control.
//
// RESULTS (after opening the RTL and building): 16 passed, 5 failed across
// 21 expect() calls. Two genuine, real findings account for all 5 failures
// -- no assertion was adjusted to hide them, and none was invented after
// the fact to match what the RTL does:
//
//   FINDING 1 -- oddmatch is gated by evenmatch, exactly as
//     ch3-collision-registers found at its own boundary (dblpf pinned 0).
//     denise_collision.v:62-63: `assign oddmatch = bm[6]&bm[4]&bm[2]&bm[0]
//     & (dblpf | evenmatch); assign evenmatch = bm[7]&bm[5]&bm[3]&bm[1];`
//     Measured independently here at the boundary ch3 explicitly declined
//     (the dblpf sweep): 4 of A1's 15 pairs -- PF1-SPR01/23/45/67 (bits
//     1-4), every pair naming PF1 except PF1-PF2 itself -- go red: CLXDAT
//     differs between dblpf=0 and dblpf=1 for an identical, non-vacuous
//     stimulus (BP1 matched, BP2 genuinely mismatched, not disabled). This
//     directly contradicts "collision detection does not change when you
//     select either single- or dual-playfield mode" (p.214). The other 11
//     pairs (PF1-PF2, and every pair not naming PF1) are genuinely
//     invariant across mode and pass -- the in-loop control for the same
//     mechanism that finds the 4 red pairs, per PACKAGE brief.
//   FINDING 2 -- the same gate breaks the appendix A parity claim
//     specifically in single-playfield mode: A3-PLAYFIELD-1-IS-ODD-
//     BITPLANES dblpf=0 is red even though BP1 (odd) is enabled,
//     match-required, and genuinely presented, and BP2 (even) is
//     genuinely, non-vacuously mismatched (not disabled) -- oddmatch is
//     unconditionally forced to 0 at dblpf=0 whenever evenmatch is 0,
//     regardless of whether BP1's own match holds. "Playfield 1 is all
//     odd-numbered enabled bitplanes" (p.273) does not hold at dblpf=0
//     under this non-vacuous stimulus. A3 at dblpf=1 and A4 at both dblpf
//     settings hold: evenmatch carries no dblpf term, so PF2's own
//     attribution is unaffected by mode either way -- ch3's
//     PLAYFIELD-ODD-EVEN group could not have shown this asymmetry, since
//     its one-bitplane-enabled stimulus makes the opposite playfield
//     vacuously present regardless of dblpf.
//
// A2 (content-sensitivity, dblpf=1) holds cleanly, and the
// HARNESS-DATA-PATH-ALIVE control passes.
//
// ASSERTION-ID GRANULARITY, checked against tools/check_gates.py itself
// (LEADING_ID_RE), not assumed: the gate reads an assertion's id as the
// leading run of `[A-Z0-9_-]` characters at the start of its expect()
// label, up to the first whitespace or colon. A1's 15 pair-checks and
// A3/A4's two dblpf sub-cases each put their per-iteration suffix (the
// pair name; "dblpf=1"/"dblpf=0") AFTER a space, so that suffix falls
// outside the id the gate reads -- all 15 A1 calls read back as the SAME
// id, A1-PLAYFIELD-MODE-INVARIANCE, and both A3 calls read back as
// A3-PLAYFIELD-1-IS-ODD-BITPLANES (A4 likewise). This is not a gap being
// worked around: it is the same convention hrm/ch3-collision-registers's
// own BITMAP-PAIR (15 pairs) and PLAYFIELD-ODD-EVEN (2 cases) groups use,
// one mutation record per shared id, because every iteration of a loop
// calls the identical shared mechanism (capture_pair() for A1, the run()
// closure for A3/A4) -- a mutation on any one iteration demonstrates that
// shared mechanism is stimulus-sensitive for all of them. One recorded
// mutation per id below is therefore the correct count, not thirteen
// missing ones.
//
// MUTATION LOG (each flag built alone via `-CFLAGS -DMUT_X=1`, run,
// confirmed to flip only its own named assertion(s) red with every other
// line identical to the 16-passed/5-failed baseline, then rebuilt with no
// flags and reconfirmed that baseline). Second token on each line is the
// assertion's own label id, exactly as read from its expect() call:
//
// MUTATION HARNESS-DATA-PATH-ALIVE stimulus red: (flag MUT_CONTROL) the
//   second sprite stimulus was changed from SPR0+SPR4 (0x11) to a repeat
//   of the first pair, SPR0+SPR2 (0x05), so the second read came back
//   equal to the first ($0200 both) instead of the expected distinct
//   $0400; only this control's own line flipped (15 passed/6 failed);
//   reverted and the 16/5 baseline returned.
// MUTATION A1-PLAYFIELD-MODE-INVARIANCE stimulus red: (flag MUT_A1)
//   representative case pair index 9 (SPR01-SPR23, a pair this
//   proposition predicts and measures as invariant) had its dblpf=1
//   capture present SPR45 instead of SPR23, so that one capture actually
//   measured a different pair (bit 10, $0400) than the dblpf=0 capture
//   (bit 9, $0200); only the SPR01-SPR23 line flipped from PASS to FAIL
//   (15 passed/6 failed), all 14 other pairs and every other assertion
//   stayed exactly at baseline; reverted and the 16/5 baseline returned.
// MUTATION A2-BITPLANE-CONTENT-DETERMINES-COLLISION stimulus red: (flag
//   MUT_A2) case B's BP1 stimulus was changed from mismatched (0x00) to
//   matched (0x01), identical to case A, so the two stimuli no longer
//   differ and bit 1 came back set in both; only the A2 line flipped (15
//   passed/6 failed); reverted and the 16/5 baseline returned.
// MUTATION A3-PLAYFIELD-1-IS-ODD-BITPLANES stimulus red: (flag MUT_A3)
//   representative case dblpf=1: run2 (meant to match only BP2) was
//   changed to also match BP1, so PF1-SPR01 (bit 1) incorrectly set in
//   the run this assertion requires it clear in; only A3-dblpf=1 flipped
//   from PASS to FAIL (15 passed/6 failed) -- A4-dblpf=1 stayed PASS
//   (BP2's own bit 5 is untouched by BP1 also matching), and both
//   dblpf=0 lines were untouched since the mutation is gated to the
//   dblpf=1 case only; reverted and the 16/5 baseline returned.
// MUTATION A4-PLAYFIELD-2-IS-EVEN-BITPLANES stimulus red: (flag MUT_A4)
//   symmetric to MUT_A3, representative case dblpf=1: run1 (meant to
//   match only BP1) was changed to also match BP2, so PF2-SPR01 (bit 5)
//   incorrectly set in the run this assertion requires it clear in; only
//   A4-dblpf=1 flipped (15 passed/6 failed) -- A3-dblpf=1 stayed PASS
//   (BP1's own bit 1 is untouched by BP2 also matching); reverted and the
//   16/5 baseline returned.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vdenise_collision.h"
#include "verilated.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_A1
#define MUT_A1 0
#endif
#ifndef MUT_A2
#define MUT_A2 0
#endif
#ifndef MUT_A3
#define MUT_A3 0
#endif
#ifndef MUT_A4
#define MUT_A4 0
#endif

namespace {

constexpr int CLXCON_ADDR = 0x4C;  // $098 >> 1
constexpr int CLXDAT_ADDR = 0x07;  // $00E >> 1
constexpr int IDLE_ADDR = 0x00;    // outside both decoded addresses.

// CLXCON bit positions (p.272): MVBPn at bit (n-1), ENBPn at bit (6+n-1),
// ENSP1/3/5/7 at bits 12/13/14/15. (Reused from ch3-collision-registers.)
constexpr uint16_t MVBP(int n) { return static_cast<uint16_t>(1u << (n - 1)); }
constexpr uint16_t ENBP(int n) { return static_cast<uint16_t>(1u << (6 + n - 1)); }
constexpr uint16_t ENSP(int oddSprite) { return static_cast<uint16_t>(1u << (12 + (oddSprite - 1) / 2)); }

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
        // Drain: clxcon has no reset-propagation guarantee on the very
        // first simulated edge (Verilator power-on 0x0000 looks like every
        // bitplane disabled, vacuously matched), which can latch a stray
        // bit into clxdat before clxcon reaches its real reset value.
        // Harness hygiene, inherited from ch3-collision-registers -- not a
        // claim about collision behaviour.
        read_reg(CLXDAT_ADDR);
    }
    ~Harness() { delete dut; }

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

// The fixed assertion harness (build brief): expect() grades, note()
// measures without grading, record() declines with a reason. note()/
// record() are declared for completeness; this package asserts all 8
// claims, so record() is not called.
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

// Shared CLXCON configuration for every assertion below: BP1 (odd) and BP2
// (even) are the only two bitplanes enabled+match-required -- both always
// LIVE, never disabled, so neither playfield is ever vacuously matched
// (the exact fix this package exists to make over ch3's PLAYFIELD-ODD-EVEN
// group). All four ENSP bits are set so each sprite group's odd nsprite bit
// alone suffices to raise its sprmatch.
constexpr uint16_t CLXCON_CFG =
    ENBP(1) | MVBP(1) | ENBP(2) | MVBP(2) |
    ENSP(1) | ENSP(3) | ENSP(5) | ENSP(7);

// Entity stimuli for the 15-pair sweep (A1), identical convention to
// ch3-collision-registers's BITMAP-PAIR group.
enum Entity { PF1 = 0, PF2, SPR01, SPR23, SPR45, SPR67, NUM_ENTITIES };
struct EntityStim { uint8_t bpl; uint8_t spr; const char* name; };
constexpr EntityStim STIM[NUM_ENTITIES] = {
    {0x01, 0x00, "PF1"},
    {0x02, 0x00, "PF2"},
    {0x00, 0x01, "SPR01"},
    {0x00, 0x04, "SPR23"},
    {0x00, 0x10, "SPR45"},
    {0x00, 0x40, "SPR67"},
};

struct Pair { Entity x, y; int bit; };
constexpr Pair PAIRS[15] = {
    {PF1, PF2,   0},
    {PF1, SPR01, 1},
    {PF1, SPR23, 2},
    {PF1, SPR45, 3},
    {PF1, SPR67, 4},
    {PF2, SPR01, 5},
    {PF2, SPR23, 6},
    {PF2, SPR45, 7},
    {PF2, SPR67, 8},
    {SPR01, SPR23, 9},
    {SPR01, SPR45, 10},
    {SPR01, SPR67, 11},
    {SPR23, SPR45, 12},
    {SPR23, SPR67, 13},
    {SPR45, SPR67, 14},
};

// Captures CLXDAT (bit-15 masked off) for one pair's stimulus at one dblpf
// setting, from a fresh Harness instance. `mut_y` overrides the Y entity
// actually presented (MUT_A1's hook); pass the pair's own y when not
// mutating.
uint16_t capture_pair(int pair_index, Entity y_override, bool dblpf) {
    Harness h;
    h.write_reg(CLXCON_ADDR, CLXCON_CFG);
    h.dut->dblpf = dblpf ? 1 : 0;
    h.read_reg(CLXDAT_ADDR);  // drain whatever accumulated before config took effect
    const Pair& p = PAIRS[pair_index];
    uint8_t bpl = static_cast<uint8_t>(STIM[p.x].bpl | STIM[y_override].bpl);
    uint8_t spr = static_cast<uint8_t>(STIM[p.x].spr | STIM[y_override].spr);
    h.pulse(bpl, spr);
    return h.read_reg(CLXDAT_ADDR) & 0x7FFF;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // HARNESS-DATA-PATH-ALIVE (no claim id; required precondition) -- two
    // distinct, known collision stimuli, presented in order, must produce
    // two distinct, correctly-positioned CLXDAT readings. Chapter 6's
    // salvaged-harness lesson (a DMA grant sampled inside the same eval()
    // as the request, so every data_out came back zero) is why this is
    // graded through expect(), not merely noted -- a silently-dead data
    // path would make every assertion below a false report about the
    // design instead of about the harness.
    // =========================================================================
    {
        Harness h;
        // BP1 and BP2 (one odd, one even) enabled with match=1 required,
        // bpldata held at 0 below -> both playfields are definitively
        // ABSENT throughout (mismatched, not vacuous) -- otherwise, with
        // every bitplane disabled, PF1-PF2 (bit 0) registers continuously
        // (denise_collision.v:54-55's vacuous-match wiring) and corrupts
        // the two sprite-only readings this control measures. Same fix
        // ch3-collision-registers's own CONTROL-KNOWN-VALUES-ARRIVE uses.
        h.write_reg(CLXCON_ADDR, CLXCON_CFG);
        h.read_reg(CLXDAT_ADDR);  // drain
        h.pulse(0x00, 0x05);  // SPR0+SPR2 -> SPR01-SPR23, bit 9
        uint16_t a = h.read_reg(CLXDAT_ADDR) & 0x7FFF;
        uint8_t second_stim = 0x11;  // SPR0+SPR4 -> SPR01-SPR45, bit 10
#if MUT_CONTROL
        second_stim = 0x05;  // repeat the first pair instead of a genuinely different one
#endif
        h.pulse(0x00, second_stim);
        uint16_t b = h.read_reg(CLXDAT_ADDR) & 0x7FFF;
        checks.expect(
            a == 0x0200 && b == 0x0400 && a != b,
            "[control] HARNESS-DATA-PATH-ALIVE: two distinct, known sprite "
            "stimuli (SPR0+SPR2, then SPR0+SPR4) produce two distinct, "
            "correctly-positioned CLXDAT readings ($0200 then $0400) "
            "through write_reg/pulse/read_reg before any assertion below "
            "trusts that path. No manual page.");
    }

    // =========================================================================
    // A1 PLAYFIELD-MODE-INVARIANCE (p.214) -- sol:7-collision.c27,
    // opus:7-cd.c26. "collision detection does not change when you select
    // either single- or dual-playfield mode", swept across all fifteen
    // CLXDAT pairs. One relation per pair: capture CLXDAT at dblpf=0 and at
    // dblpf=1 from the SAME stimulus, compare the two captures -- never a
    // capture compared against a constant read off one run.
    // =========================================================================
    {
        static const char* PAIR_NAMES[15] = {
            "PF1-PF2", "PF1-SPR01", "PF1-SPR23", "PF1-SPR45", "PF1-SPR67",
            "PF2-SPR01", "PF2-SPR23", "PF2-SPR45", "PF2-SPR67",
            "SPR01-SPR23", "SPR01-SPR45", "SPR01-SPR67",
            "SPR23-SPR45", "SPR23-SPR67", "SPR45-SPR67",
        };
        for (int i = 0; i < 15; ++i) {
            Entity y0 = PAIRS[i].y;
            Entity y1 = PAIRS[i].y;
#if MUT_A1
            // Representative mutation on pair index 9 (SPR01-SPR23, a pair
            // this proposition predicts as invariant): the dblpf=1 capture
            // presents SPR45 instead of SPR23, breaking the "same
            // stimulus" precondition so the two captures differ for a
            // reason that has nothing to do with mode.
            if (i == 9) y1 = SPR45;
#endif
            uint16_t r0 = capture_pair(i, y0, false);
            uint16_t r1 = capture_pair(i, y1, true);
            char hex0[8], hex1[8];
            std::snprintf(hex0, sizeof hex0, "%04X", r0);
            std::snprintf(hex1, sizeof hex1, "%04X", r1);
            checks.expect(
                r0 == r1,
                std::string("A1-PLAYFIELD-MODE-INVARIANCE ") + PAIR_NAMES[i] +
                    " (p.214): CLXDAT&0x7FFF at dblpf=0 ($" + hex0 +
                    ") equals CLXDAT&0x7FFF at dblpf=1 ($" + hex1 +
                    ") for the same " + PAIR_NAMES[i] +
                    " stimulus -- \"collision detection does not change "
                    "when you select either single- or dual-playfield "
                    "mode\" (p.214). sol:7-collision.c27, opus:7-cd.c26.");
        }
    }

    // =========================================================================
    // A2 BITPLANE-CONTENT-DETERMINES-COLLISION (p.214) --
    // sol:7-collision.c28, opus:7-cd.c27. "Collision detection depends only
    // on the actual bits present in the odd-numbered or even-numbered
    // bitplanes." A1 covers the exclusivity half (mode carries no
    // information); this covers the complementary half -- holding dblpf
    // fixed at 1 (chosen to keep this reading clear of A1's own subject,
    // the dblpf-conditioned oddmatch/evenmatch gate), changing bitplane
    // content must change the result.
    // =========================================================================
    {
        uint8_t bp1_case_a = 0x01;  // BP1 matched -> PF1 present
        uint8_t bp1_case_b = 0x00;  // BP1 mismatched -> PF1 absent
#if MUT_A2
        bp1_case_b = 0x01;  // identical to case A -- the two stimuli no longer differ
#endif
        Harness ha;
        ha.write_reg(CLXCON_ADDR, CLXCON_CFG);
        ha.dut->dblpf = 1;
        ha.read_reg(CLXDAT_ADDR);
        ha.pulse(bp1_case_a, 0x01);  // BP1 matched, SPR0 present
        uint16_t ra = ha.read_reg(CLXDAT_ADDR) & 0x7FFF;

        Harness hb;
        hb.write_reg(CLXCON_ADDR, CLXCON_CFG);
        hb.dut->dblpf = 1;
        hb.read_reg(CLXDAT_ADDR);
        hb.pulse(bp1_case_b, 0x01);  // BP1 mismatched (unless mutated), SPR0 present
        uint16_t rb = hb.read_reg(CLXDAT_ADDR) & 0x7FFF;

        checks.expect(
            (ra & 0x0002) != 0 && (rb & 0x0002) == 0,
            "A2-BITPLANE-CONTENT-DETERMINES-COLLISION (p.214): with dblpf "
            "held at 1, BP1 (odd) matched sets PF1-SPR01 (bit 1); BP1 "
            "mismatched, nothing else changed, clears it -- \"Collision "
            "detection depends only on the actual bits present in the "
            "odd-numbered or even-numbered bitplanes\" (p.214). "
            "sol:7-collision.c28, opus:7-cd.c27.");
    }

    // =========================================================================
    // A3/A4 PLAYFIELD-1-IS-ODD-BITPLANES / PLAYFIELD-2-IS-EVEN-BITPLANES
    // (p.273) -- sol:7-appA-clxdat.c6+opus:7-appA-clxdat.c3 (A3),
    // sol:7-appA-clxdat.c7+opus:7-appA-clxdat.c4 (A4). BP1 (odd) and BP2
    // (even) are BOTH enabled+match-required in every run below (CLXCON_CFG)
    // -- never disabled, so neither playfield is ever vacuously matched,
    // unlike ch3-collision-registers's PLAYFIELD-ODD-EVEN group (one
    // bitplane enabled at a time, the other disabled). run1 matches only
    // the odd bitplane; run2 matches only the even bitplane; both runs are
    // repeated at dblpf=0 and dblpf=1 since the manual's playfield
    // definitions do not carve out a mode exception either.
    // =========================================================================
    {
        auto run = [&](bool bp1_matched, bool bp2_matched, bool dblpf) -> uint16_t {
            Harness h;
            h.write_reg(CLXCON_ADDR, CLXCON_CFG);
            h.dut->dblpf = dblpf ? 1 : 0;
            h.read_reg(CLXDAT_ADDR);
            uint8_t bpl = static_cast<uint8_t>((bp1_matched ? 0x01 : 0x00) |
                                                (bp2_matched ? 0x02 : 0x00));
            h.pulse(bpl, 0x01);  // SPR0 present in every run
            return h.read_reg(CLXDAT_ADDR) & 0x7FFF;
        };

        for (bool dblpf : {true, false}) {
            bool mut3 = false, mut4 = false;
#if MUT_A3
            if (dblpf) mut3 = true;  // representative case: dblpf=1
#endif
#if MUT_A4
            if (dblpf) mut4 = true;  // representative case: dblpf=1
#endif
            // run1: odd (BP1) matched, even (BP2) mismatched -- unless
            // MUT_A4 also matches BP2 here, breaking A4's "run1.bit5==0"
            // expectation without touching A3's run1.bit1 dependency.
            uint16_t r1 = run(true, mut4, dblpf);
            // run2: even (BP2) matched, odd (BP1) mismatched -- unless
            // MUT_A3 also matches BP1 here, breaking A3's "run2.bit1==0"
            // expectation without touching A4's run2.bit5 dependency.
            uint16_t r2 = run(mut3, true, dblpf);

            const char* mode = dblpf ? "dblpf=1" : "dblpf=0";
            char h1[8], h2[8];
            std::snprintf(h1, sizeof h1, "%04X", r1);
            std::snprintf(h2, sizeof h2, "%04X", r2);

            checks.expect(
                (r1 & 0x0002) != 0 && (r2 & 0x0002) == 0,
                std::string("A3-PLAYFIELD-1-IS-ODD-BITPLANES ") + mode +
                    " (p.273): with BP1(odd) and BP2(even) both enabled+"
                    "required, only-BP1-matched sets PF1-SPR01 (bit 1, "
                    "run1=$" + h1 + "); only-BP2-matched (BP1 genuinely, "
                    "non-vacuously mismatched) leaves it clear (run2=$" +
                    h2 + ") -- \"Playfield 1 is all odd-numbered enabled "
                    "bitplanes\" (p.273). sol:7-appA-clxdat.c6, "
                    "opus:7-appA-clxdat.c3.");

            checks.expect(
                (r2 & 0x0020) != 0 && (r1 & 0x0020) == 0,
                std::string("A4-PLAYFIELD-2-IS-EVEN-BITPLANES ") + mode +
                    " (p.273): with BP1(odd) and BP2(even) both enabled+"
                    "required, only-BP2-matched sets PF2-SPR01 (bit 5, "
                    "run2=$" + h2 + "); only-BP1-matched (BP2 genuinely, "
                    "non-vacuously mismatched) leaves it clear (run1=$" +
                    h1 + ") -- \"Playfield 2 is all even-numbered enabled "
                    "bitplanes\" (p.273). sol:7-appA-clxdat.c7, "
                    "opus:7-appA-clxdat.c4.");
        }
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
