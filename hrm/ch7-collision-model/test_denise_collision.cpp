// ch7-collision-model -- denise_collision conformance test
//
// Test purpose: printed p.213's collision model -- the three named collision
//   classes (sprite-group to sprite-group, sprite-group to either playfield,
//   playfield to playfield), that they combine, and the defining condition
//   for ANY of them: two or more objects, at the SAME pixel position, which
//   then sets a bit in CLXDAT. The same-pixel requirement is the
//   load-bearing part and is what distinguishes this package from the
//   existing coverage (per its own task sheet). Package ch7-collision-model,
//   fixture denise_collision (one Verilator build, this module only).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.213 (claims.jsonl for ch7-collision-model, 9 records). A red result
//   below means denise_collision.v disagrees with the manual, not that the
//   assertion is wrong -- no assertion here was, or should be, adjusted to
//   make the RTL pass.
// Non-goals: the appendix-A CLXCON/CLXDAT bit-by-bit register table
//   (`ch3-collision-registers`, 98 claims, already asserted there); chapter
//   4's own bare-capability statement at its own page 113
//   (`ch4-sprite-collision`); the dblpf single- vs dual-playfield comparison
//   and the odd/even-bitplane-to-playfield equations at p.214/appendix A
//   (`ch7-collision-playfield-mode`, confirmed a separate, already-specified
//   package -- see ORDER OF WORK below).
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: nanomig/src/minimig-aga/denise_collision.v (read in full for
//   this build). Instantiated at denise.v:474-486 with bpldata and nsprite.
//
// ORDER OF WORK: SPECIFICATION.md in this directory was written from
//   claims.jsonl's quotations alone and committed (6d30b7d) before this
//   agent opened denise_collision.v. This file's assertions, expected
//   values, and claim-id coverage follow that committed specification; the
//   RTL was opened only after the commit, per the build brief's gate. Before
//   writing SPECIFICATION.md, this agent additionally read two required
//   sibling packages per its own task sheet ("existing coverage you must
//   read before writing"): hrm/ch3-collision-registers/ and
//   hrm/denise_collision/ (both already-committed test SOURCES, not RTL),
//   plus hrm/ch4-sprite-collision/ for the same reason ch4-sprite-collision
//   itself gives -- reusing an already-committed sibling harness's disclosed
//   interface (port list; clk/clk7_en/reset/aga/reg_address_in/data_in/
//   dblpf/bpldata/nsprite/data_out; the CLXDAT word address $00E->0x07; the
//   pulse()/read_reg() timing convention; the harness-hygiene drain-read)
//   is this pipeline's own established precedent for writing a
//   specification without opening the module's own RTL file. This agent
//   also read hrm/ch7-collision-playfield-mode/SPECIFICATION.md (a sibling
//   package's own already-committed SPECIFICATION.md, not its RTL) to
//   confirm the seam between that package's page-214 mode comparison and
//   this package's page-213 bare-capability propositions -- exactly the
//   two-step gate's own purpose, two builders reasoning from specifications
//   rather than from each other's tests.
//
// CORRECTION (the lead's, not this agent's): SPECIFICATION.md line 44 names
//   an RTL expression -- "because oddmatch is gated by `(dblpf | evenmatch)`
//   in the RTL" -- inside a document meant to predate the implementation.
//   That sentence reached this agent through the classify.evidence field of
//   claim sol:7-collision.c2 in ch7-collision-model.jsonl, which carries an
//   earlier pipeline stage's own RTL measurement; the lead has since said
//   this leaked into the task sheet ahead of the gate and is a defect in the
//   brief, not in this package. The committed SPECIFICATION.md is left as
//   written -- the record is more useful honest than edited after the fact.
//   Per the lead's instruction, this note says where it came from and
//   confirms which expected values, if any, rest on it: NONE. Every
//   assertion below was independently re-derived by this agent from its own
//   reading of denise_collision.v (this build, after the spec commit), and
//   checked specifically for this dependency:
//     - A2 (SPRITE-GROUP-TO-PLAYFIELD-COLLISION) is measured at dblpf=1,
//       which unconditionally satisfies oddmatch's own gate term regardless
//       of evenmatch -- so its expected values hold whether or not that gate
//       exists at all, let alone has the shape the leaked sentence describes.
//     - A3 (PLAYFIELD-TO-PLAYFIELD-COLLISION) and A5 (MULTIPLICITY-REQUIRED)
//       both use a playfield-1-alone leg at dblpf=0, which the leaked
//       sentence would suggest is exactly where the gate matters -- checked
//       by hand against the RTL actually read: A3's "alone" legs are clear
//       because evenmatch=0 there regardless of oddmatch's value (CLXDAT bit
//       0 is evenmatch&oddmatch), and A5's "alone" legs are clear because
//       every relevant CLXDAT bit ANDs oddmatch or evenmatch against a
//       sprite-match or the other playfield's match, both 0 with nothing
//       else presented -- so oddmatch's own value cannot move any bit in
//       either assertion. Neither expected value depends on the gate's
//       existence or its exact form.
//   If a future change to this file ever makes an expected value depend on
//   that gate, that is a HALT to report, not something to fold in silently.
//
// RTL OPENED (this commit only, after the specification commit above):
//   denise_collision.v's port list and clocking matched the disclosed-
//   interface assumptions the spec commit was written from exactly -- no
//   HALT was needed. Facts learned here, not assumed before: cl[0..14] are
//   purely combinational (`assign`) conjunctions of two independent
//   presence terms (oddmatch/evenmatch/sprmatch[0..3]); only clxdat itself
//   is a register, and it carries NO reset branch (confirmed directly:
//   its own always-block has no `if (reset)` arm at all), matching the
//   sibling harnesses' own disclosed drain-read hygiene, reused below. Under
//   CLXCON's power-up default (16'h0fff), every one of the six real
//   bitplanes matches when its own bpldata bit is 1 (MVBP=1, ENBP=1 for
//   all six), so no CLXCON write is needed anywhere in this file. bpldata
//   bit (n-1) is bitplane n's live bit (odd n = playfield 1, even n =
//   playfield 2); nsprite bit k is sprite k's own hit bit, and with CLXCON's
//   default (bits 12-15 clear) only the even-numbered sprite of each pair
//   counts on its own (sprmatch[k] = nsprite[2k] | (nsprite[2k+1] &
//   clxcon[12+k])) -- so sprite 0, 2, 4, 6 alone each drive their own group's
//   match bit directly, which is what every stimulus below uses.
//
// CLXDAT bit map relied on below (from denise_collision.v:81-95): bit 0
//   PF1-PF2, bit 1 PF1-SPR01, bit 5 PF2-SPR01, bit 9 SPR01-SPR23, bit 10
//   SPR01-SPR45.
//
// MEASUREMENT BASIS, per assertion:
//   SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION: sprite 0 alone, sprite 2 alone,
//     each independently read all-clear at bit 9; sprite 0 and sprite 2
//     presented together at one pixel sets bit 9.
//   SPRITE-GROUP-TO-PLAYFIELD-COLLISION: dblpf=1 (see CORRECTION above for
//     why), an isolated single-playfield stimulus (odd bitplanes only, or
//     even bitplanes only) together with sprite 0 at one pixel sets the
//     corresponding bit (1 for playfield 1, 5 for playfield 2); playfield
//     alone and sprite alone each read clear at bit 1.
//   PLAYFIELD-TO-PLAYFIELD-COLLISION: playfield 1 alone and playfield 2
//     alone each read clear at bit 0; both playfields' bitplanes matched
//     together at one pixel sets bit 0. Measured at dblpf=0 (default); see
//     CORRECTION above for why this pairing's expected values do not depend
//     on the oddmatch/dblpf gate either way.
//   COMBINATION-OF-CLASSES: one pixel staging a playfield-playfield pair, a
//     playfield-sprite pair, and a sprite-sprite pair simultaneously (all
//     six bitplanes matched, sprite 0 + sprite 2 both presented) reads bits
//     0, 1, and 9 all set together in the one CLXDAT read.
//   MULTIPLICITY-REQUIRED: sprite 0 alone, playfield 1 alone, and sprite 4
//     alone each independently read entirely clear (no bit anywhere) --
//     contrasted with SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION's own
//     "together" leg, the identical apparatus, which does set a bit once a
//     second object joins.
//   SAME-PIXEL-REQUIRED (the assertion this package exists for): sprite 0
//     and sprite 2, the same pair SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION
//     shows DOES collide when co-resident, are instead presented one pixel
//     apart, and separately several pixels apart in the same continuous run
//     (no reset between them -- the same line), never co-resident in either
//     case -- both read clear at bit 9. The "several pixels apart, same
//     run" leg is the control the lead asked for: without it, this
//     assertion would pass equally on a module that collides on mere
//     proximity/same-line rather than on the exact same pixel.
//   COLLISION-SETS-CLXDAT-BIT: CLXDAT read idle (nothing presented since the
//     constructor's own drain read) shows nothing; the same register, read
//     again after sprite 0 and sprite 2 are presented together, shows a
//     changed, non-zero value.
//
// CLAIM COVERAGE -- all 9 records of this package's own claims.jsonl cited
//   in full inside an expect() label:
//     SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION  -- sol:7-collision.c1, opus:7-cd.c1
//     SPRITE-GROUP-TO-PLAYFIELD-COLLISION     -- sol:7-collision.c2, opus:7-cd.c1
//     PLAYFIELD-TO-PLAYFIELD-COLLISION        -- sol:7-collision.c3, opus:7-cd.c1
//     COMBINATION-OF-CLASSES                  -- sol:7-collision.c4, opus:7-cd.c1
//     MULTIPLICITY-REQUIRED                   -- sol:7-collision.c6, opus:7-cd.c4
//     SAME-PIXEL-REQUIRED                     -- sol:7-collision.c6, opus:7-cd.c4
//     COLLISION-SETS-CLXDAT-BIT               -- sol:7-collision.c7, opus:7-cd.c5
//   `opus:7-cd.c1` is the whole-sentence twin of sol's four (per the claims
//   file's own twin note) and is cited on all four rather than spawning a
//   fifth assertion; `opus:7-cd.c4` and `opus:7-cd.c5` are identical-quote
//   twins of sol's c6 and c7 respectively, cited alongside them. No claim
//   in this package's inventory was declined; 7 assertions cover all 9.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no claim id, "[control]" so the
//   mutation gate does not require a claim id for it) proves the
//   write/pulse/read measurement basis every graded assertion above shares
//   actually delivers distinct, correctly positioned values before any of
//   them is trusted -- per the build brief's own chapter-6 warning that a
//   silently dead data path reports every data-dependent assertion as a
//   finding about the design instead of the harness.
//
// RESULTS (WSL/Verilator 5.052, built and run 13.09.2026, after the
//   specification commit -- denise_collision.v was opened only once, for
//   this build, not before): 8 passed, 0 failed. All 7 graded propositions
//   and the FIXTURE-LIVE-DATA-PATH control are GREEN: printed p.213's
//   collision model -- the three named classes, their combination, the
//   two-or-more/same-pixel condition, and CLXDAT as the register a
//   collision sets a bit in -- holds at this module boundary. No finding.
//
// MUTATION LOG (WSL/Verilator, same session as RESULTS above; each entry
//   actually run: rebuild with the named -CFLAGS -DMUT_*=1 flag -> run ->
//   read the PASS/FAIL lines -> confirm only the named assertion changed
//   colour, no other assertion moved -> rebuild with no flags -> confirm
//   the 8-passed/0-failed baseline was restored):
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: (MUT_CONTROL) built with
//   -DMUT_CONTROL=1, repeats the first stimulus as the second -> FAIL (7
//   passed, 1 failed), only FIXTURE-LIVE-DATA-PATH newly red. Baseline
//   restored after.
// MUTATION SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION stimulus red: (MUT_A1)
//   built with -DMUT_A1=1, withholds sprite 2 from the "together" stimulus
//   -> FAIL (7 passed, 1 failed), only
//   SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION newly red. Baseline restored.
// MUTATION SPRITE-GROUP-TO-PLAYFIELD-COLLISION stimulus red: (MUT_A2) built
//   with -DMUT_A2=1, withholds the sprite from both playfield "together"
//   legs -> FAIL (7 passed, 1 failed), only
//   SPRITE-GROUP-TO-PLAYFIELD-COLLISION newly red. Baseline restored.
// MUTATION PLAYFIELD-TO-PLAYFIELD-COLLISION stimulus red: (MUT_A3) built
//   with -DMUT_A3=1, withholds playfield 1's own bitplanes from the "both"
//   stimulus -> FAIL (7 passed, 1 failed), only
//   PLAYFIELD-TO-PLAYFIELD-COLLISION newly red. Baseline restored.
// MUTATION COMBINATION-OF-CLASSES stimulus red: (MUT_A4) built with
//   -DMUT_A4=1, withholds sprite 2, removing the sprite-sprite class from
//   the combined stimulus -> FAIL (7 passed, 1 failed), only
//   COMBINATION-OF-CLASSES newly red. Baseline restored.
// MUTATION MULTIPLICITY-REQUIRED stimulus red: (MUT_A5) built with
//   -DMUT_A5=1, adds sprite 0 to the third "alone" leg -> FAIL (7 passed, 1
//   failed), only MULTIPLICITY-REQUIRED newly red. Baseline restored.
// MUTATION SAME-PIXEL-REQUIRED stimulus red: (MUT_A6) built with
//   -DMUT_A6=1, sprite 0 re-asserted in the same pulse as sprite 2's
//   far-apart presentation, making the pair co-resident -> FAIL (7 passed,
//   1 failed), only SAME-PIXEL-REQUIRED newly red. Baseline restored.
// MUTATION COLLISION-SETS-CLXDAT-BIT stimulus red: (MUT_A7) built with
//   -DMUT_A7=1, withholds the collision stimulus from the "after"
//   measurement -> FAIL (7 passed, 1 failed), only
//   COLLISION-SETS-CLXDAT-BIT newly red. Baseline restored.
//
// Every one of the 8 mutations moved exactly the one assertion it named and
// no other; the final rebuild with no flags reproduced the original
// 8-passed/0-failed baseline exactly.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vdenise_collision.h"

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
#ifndef MUT_A5
#define MUT_A5 0
#endif
#ifndef MUT_A6
#define MUT_A6 0
#endif
#ifndef MUT_A7
#define MUT_A7 0
#endif

namespace {

constexpr int CLXDAT_ADDR = 0x07;  // $00E >> 1
constexpr int IDLE_ADDR = 0x00;    // outside the decoded address.

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
        // Drain: clxdat carries no reset branch of its own (confirmed by
        // reading denise_collision.v for this build), so a stray bit from
        // Verilator's power-on state can be latched before clxcon reaches
        // its real reset value. One read clears it. Disclosed by
        // hrm/ch3-collision-registers/ and hrm/ch4-sprite-collision/'s own
        // harnesses; re-included here because a fix does not travel with a
        // copied file -- this file's own assertions are what prove it still
        // holds.
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

    void idle_ticks(int n) { for (int i = 0; i < n; ++i) tick_7mhz(); }

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
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
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
            "every assertion in this file, on the same measurement basis.");
        // MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: (MUT_CONTROL)
        // repeats the first stimulus as the second instead of a genuinely
        // different one.
    }

    // ---------------- SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION ----------------
    {
        Harness h;
        h.pulse(0x00, 0x01);  // sprite 0 alone
        uint16_t alone0 = h.read_reg(CLXDAT_ADDR);
        h.pulse(0x00, 0x04);  // sprite 2 alone
        uint16_t alone2 = h.read_reg(CLXDAT_ADDR);

        uint8_t together_spr = 0x05;  // sprite 0 + sprite 2, same pixel
#if MUT_A1
        together_spr = 0x01;  // MUTATION: sprite 2 withheld, sprite 0 stays alone
#endif
        h.pulse(0x00, together_spr);
        uint16_t together = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (alone0 & 0x0200) == 0 && (alone2 & 0x0200) == 0 &&
            (together & 0x0200) != 0,
            "SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION: sprite 0 alone and "
            "sprite 2 alone each register no SPR01-SPR23 collision (CLXDAT "
            "bit 9 clear); presented together at the same pixel, that bit "
            "sets. 'You can use the hardware to detect collisions between "
            "one sprite group and another sprite group' (p.213). "
            "sol:7-collision.c1, opus:7-cd.c1.");
        // MUTATION SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION stimulus red:
        // (MUT_A1) withholds sprite 2 from the "together" stimulus, so
        // sprite 0 stays alone.
    }

    // ---------------- SPRITE-GROUP-TO-PLAYFIELD-COLLISION ----------------
    // Measured at dblpf=1; see this file's CORRECTION note above for why.
    {
        Harness ha;
        ha.dut->dblpf = 1;
        ha.pulse(0x15, 0x00);  // playfield 1 alone (odd bitplanes 1,3,5), no sprite
        uint16_t pf1_alone = ha.read_reg(CLXDAT_ADDR);
        ha.pulse(0x00, 0x01);  // sprite 0 alone, no playfield content
        uint16_t spr_alone = ha.read_reg(CLXDAT_ADDR);

        uint8_t spr_for_pf1 = 0x01;
#if MUT_A2
        spr_for_pf1 = 0x00;  // MUTATION: sprite withheld from the "together" leg
#endif
        ha.pulse(0x15, spr_for_pf1);  // playfield 1 + sprite 0, same pixel
        uint16_t pf1_hit = ha.read_reg(CLXDAT_ADDR);

        Harness hb;
        hb.dut->dblpf = 1;
        uint8_t spr_for_pf2 = 0x01;
#if MUT_A2
        spr_for_pf2 = 0x00;  // MUTATION: same withholding, other playfield leg
#endif
        hb.pulse(0x2A, spr_for_pf2);  // playfield 2 (even bitplanes 2,4,6) + sprite 0
        uint16_t pf2_hit = hb.read_reg(CLXDAT_ADDR);

        checks.expect(
            (pf1_alone & 0x0002) == 0 && (spr_alone & 0x0002) == 0 &&
            (pf1_hit & 0x0002) != 0 && (pf2_hit & 0x0020) != 0,
            "SPRITE-GROUP-TO-PLAYFIELD-COLLISION: playfield 1 alone and "
            "sprite 0 alone each register no PF1-SPR01 collision (CLXDAT "
            "bit 1 clear); sprite 0 together with playfield 1 sets bit 1, "
            "and sprite 0 together with playfield 2 sets bit 5 -- the "
            "sprite group collides with either playfield. Measured in "
            "dual-playfield mode so an isolated single-playfield stimulus "
            "is unambiguous; see this file's CORRECTION note for why. "
            "'any sprite group and either of the playfields' (p.213). "
            "sol:7-collision.c2, opus:7-cd.c1.");
        // MUTATION SPRITE-GROUP-TO-PLAYFIELD-COLLISION stimulus red:
        // (MUT_A2) withholds the sprite from both "together" legs, so only
        // the playfield is present in each.
    }

    // ---------------- PLAYFIELD-TO-PLAYFIELD-COLLISION ----------------
    {
        Harness h;
        h.pulse(0x15, 0x00);  // playfield 1 alone
        uint16_t pf1_alone = h.read_reg(CLXDAT_ADDR);
        h.pulse(0x2A, 0x00);  // playfield 2 alone
        uint16_t pf2_alone = h.read_reg(CLXDAT_ADDR);

        uint8_t both_pf = 0x3F;  // all six bitplanes matched: PF1 and PF2 together
#if MUT_A3
        both_pf = 0x2A;  // MUTATION: playfield 1's own bitplanes withheld, playfield 2 stays matched
#endif
        h.pulse(both_pf, 0x00);
        uint16_t both = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (pf1_alone & 0x0001) == 0 && (pf2_alone & 0x0001) == 0 &&
            (both & 0x0001) != 0,
            "PLAYFIELD-TO-PLAYFIELD-COLLISION: playfield 1 alone and "
            "playfield 2 alone each register no PF1-PF2 collision (CLXDAT "
            "bit 0 clear); both playfields' bitplanes matched together at "
            "the same pixel sets that bit. Measured at dblpf=0 (default) "
            "-- this pairing's expected values hold independently of the "
            "oddmatch/dblpf gate either way, per this file's CORRECTION "
            "note. 'the two playfields' (p.213). sol:7-collision.c3, "
            "opus:7-cd.c1.");
        // MUTATION PLAYFIELD-TO-PLAYFIELD-COLLISION stimulus red: (MUT_A3)
        // withholds playfield 1's own bitplanes from the "both" stimulus,
        // leaving only playfield 2 matched.
    }

    // ---------------- COMBINATION-OF-CLASSES ----------------
    {
        Harness h;
        uint8_t nsprite_stim = 0x05;  // sprite 0 + sprite 2 -> SPR01-SPR23, bit 9
#if MUT_A4
        nsprite_stim = 0x01;  // MUTATION: sprite 2 withheld -- the sprite-sprite class no longer represented
#endif
        h.pulse(0x3F, nsprite_stim);  // both playfields matched + sprite pair, one pixel
        uint16_t combined = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (combined & 0x0001) != 0 &&   // bit 0: playfield-to-playfield
            (combined & 0x0002) != 0 &&   // bit 1: playfield-to-sprite (PF1-SPR01)
            (combined & 0x0200) != 0,     // bit 9: sprite-to-sprite (SPR01-SPR23)
            "COMBINATION-OF-CLASSES: one pixel staged so a "
            "playfield-to-playfield pair (bit 0), a playfield-to-sprite "
            "pair (bit 1), and a sprite-to-sprite pair (bit 9) are all "
            "simultaneously true reads all three bits set together in one "
            "CLXDAT read -- the three classes combine, not only one at a "
            "time. 'or any combination of these items' (p.213). "
            "sol:7-collision.c4, opus:7-cd.c1.");
        // MUTATION COMBINATION-OF-CLASSES stimulus red: (MUT_A4) withholds
        // sprite 2, removing the sprite-to-sprite class from the combined
        // stimulus while the other two classes' content is left intact.
    }

    // ---------------- MULTIPLICITY-REQUIRED ----------------
    {
        Harness h;
        h.pulse(0x00, 0x01);  // sprite 0 alone
        uint16_t spr_alone = h.read_reg(CLXDAT_ADDR);
        h.pulse(0x15, 0x00);  // playfield 1 alone
        uint16_t pf_alone = h.read_reg(CLXDAT_ADDR);

        uint8_t third_stim = 0x10;  // sprite 4 alone
#if MUT_A5
        third_stim = 0x11;  // MUTATION: sprite 0 added -- no longer alone
#endif
        h.pulse(0x00, third_stim);
        uint16_t third = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (spr_alone & 0x7FFF) == 0 && (pf_alone & 0x7FFF) == 0 &&
            (third & 0x7FFF) == 0,
            "MULTIPLICITY-REQUIRED: sprite 0 alone, playfield 1 alone, and "
            "sprite 4 alone each register no collision at all (CLXDAT "
            "reads entirely clear) -- one object present is not enough. "
            "Contrasted with SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION above, "
            "the identical apparatus, which shows a bit does set once a "
            "second object joins. 'Collisions are detected when two or "
            "more objects attempt to overlap' (p.213). sol:7-collision.c6, "
            "opus:7-cd.c4.");
        // MUTATION MULTIPLICITY-REQUIRED stimulus red: (MUT_A5) adds
        // sprite 0 to the third stimulus, so it is no longer a single
        // object alone.
    }

    // ---------------- SAME-PIXEL-REQUIRED ----------------
    // The assertion this package exists for (per its own task sheet).
    {
        Harness h;
        // adjacent pixel: sprite 0, then (next pixel) sprite 2 -- never
        // co-resident in one pixel.
        h.pulse(0x00, 0x01);
        h.pulse(0x00, 0x04);
        uint16_t adjacent = h.read_reg(CLXDAT_ADDR);

        // Same continuous run (no reset between -- the same line), several
        // pixels apart, still never co-resident. This leg is the control
        // the lead asked for: without it, this assertion would pass equally
        // on a module that collides on mere same-line proximity rather than
        // the exact same pixel.
        h.pulse(0x00, 0x01);
        h.idle_ticks(5);
        uint8_t far_stim = 0x04;  // sprite 2 alone, several pixels later
#if MUT_A6
        far_stim = 0x05;  // MUTATION: sprite 0 re-asserted in the SAME pulse as sprite 2 -- now co-resident
#endif
        h.pulse(0x00, far_stim);
        uint16_t far_apart = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (adjacent & 0x0200) == 0 && (far_apart & 0x0200) == 0,
            "SAME-PIXEL-REQUIRED: sprite 0 and sprite 2 -- the same pair "
            "SPRITE-GROUP-TO-SPRITE-GROUP-COLLISION above shows DOES "
            "collide when co-resident -- register no SPR01-SPR23 collision "
            "when each is instead presented alone: one pixel apart, and "
            "separately several pixels apart in the same continuous run "
            "(no reset between them -- the same line), never co-resident "
            "either way. Guards specifically against a module that collides "
            "on same-line proximity rather than the same pixel: if either "
            "non-co-resident leg above showed the bit set, that would be "
            "this package's own finding. 'Collisions are detected ... when "
            "... [objects] attempt to overlap in the same pixel position' "
            "(p.213). sol:7-collision.c6, opus:7-cd.c4.");
        // MUTATION SAME-PIXEL-REQUIRED stimulus red: (MUT_A6) re-asserts
        // sprite 0 in the same pulse as sprite 2's far-apart presentation,
        // making the pair co-resident after all.
    }

    // ---------------- COLLISION-SETS-CLXDAT-BIT ----------------
    {
        Harness h;
        uint16_t idle = h.read_reg(CLXDAT_ADDR);  // quiescent: nothing presented yet

        uint8_t collision_spr = 0x05;  // sprite 0 + sprite 2, same pixel -> bit 9
#if MUT_A7
        collision_spr = 0x00;  // MUTATION: the collision withheld -- nothing presented
#endif
        h.pulse(0x00, collision_spr);
        uint16_t after = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (idle & 0x7FFF) == 0 && (after & 0x7FFF) == 0x0200 && idle != after,
            "COLLISION-SETS-CLXDAT-BIT: a CLXDAT read taken before any "
            "object is presented shows nothing registered; the same "
            "register, read again after sprite 0 and sprite 2 are "
            "presented together at one pixel (a genuine SPR01-SPR23 "
            "collision), returns a changed, non-zero value -- the "
            "collision is visible by reading the register, not only an "
            "internal condition. 'This will set a bit in the collision "
            "data register' (p.213). sol:7-collision.c7, opus:7-cd.c5.");
        // MUTATION COLLISION-SETS-CLXDAT-BIT stimulus red: (MUT_A7)
        // withholds the collision stimulus from the "after" measurement,
        // so nothing is presented.
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
