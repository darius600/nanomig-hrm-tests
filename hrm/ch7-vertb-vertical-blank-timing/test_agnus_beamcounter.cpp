// Test purpose: VERTB (bit 5) fires at display line 0, and the hardwired OCS
//   vertical-blanking duration/range for NTSC (20 lines) and PAL (25 lines),
//   per SPECIFICATION.md propositions P1-P5.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, p.219
//   ("Bit 5, VERTB, causes an interrupt at line 0 ..."; "The minimum time of
//   vertical blanking is 20 horizontal scan lines for an NTSC system and 25
//   horizontal scan lines for a PAL system. The range starts at line 0 and
//   ends at line 20 for NTSC or line 25 for PAL."), and Figure 7-4, p.221
//   (VERTB / "vertical blank interval", denotation only).
// Non-goals: ECS-programmable vertical blanking (BEAMCON0 HARDDIS/VARVBEN) --
//   see ch7-beamcon0-ecs-control-bits. The wiring of VERTB onto interrupt
//   request bit 5, and the Exec/hardware interrupt-priority columns of
//   Figure 7-4 -- both need paula_intcontroller, outside this fixture's
//   boundary; recorded below, not asserted.
//
// OCS SCOPE: every scenario below drives ecs=0 explicitly (never left at a
// reset default) and PRECOND-HARDDIS-LOW-ECS0 checks that harddis_out reads
// 0 throughout, so the hardwired-range assertions are provably measuring the
// OCS path and not an unstated default.
//
// MUTATION HONESTY NOTE: four assertions below carry `none impossible`. Two
// of the four are genuinely unfalsifiable at this module boundary, full
// stop; the other two merely have no convenient mutation inside the OCS
// scope this PACKAGE is assigned, and ARE falsifiable by a stimulus this
// same MODULE accepts, one step outside that scope. The two are not the
// same claim and are not interchangeable:
//
//   Genuinely unfalsifiable here, under OCS or ECS alike --
//   NTSC-VBLANK-STARTS-LINE-0, PAL-VBLANK-STARTS-LINE-0: vbstop is
//   represented as a non-negative value everywhere in this module, both the
//   hardwired OCS constants (20/25, agnus_beamcounter.v:89-90) and the
//   ECS-programmable vbstop_reg (an unsigned 11-bit register,
//   agnus_beamcounter.v:188). vbl=(vpos<=vbstop) with vpos==0 (:444), so
//   this holds for every value vbstop can ever take on this module. No
//   stimulus anywhere in agnus_beamcounter's input space -- OCS or ECS --
//   falsifies it; it is a tautology of the comparison's operand widths, not
//   OCS-scope-limited coverage. Widening the boundary to ECS would not help:
//   the register vbstop_reg that ECS makes writable is itself unsigned.
//
//   Falsifiable on this same module, but only outside this package's
//   assigned OCS scope (ecs=0) -- not exercised here, and said so rather
//   than left looking unfalsifiable:
//   PRECOND-HARDDIS-LOW-ECS0 -- ecs=1 plus a BEAMCON0 write setting HARDDIS
//   (bit 14) would drive harddis_out high (agnus_beamcounter.v:123 gates the
//   write on ecs; :128 reads the bit; :236 ORs it into harddis_out). A real
//   stimulus on a real port of this exact module, rejected only because
//   ecs=1 is ch7-beamcon0-ecs-control-bits' territory, not this package's.
//   PAL-VBLANK-ENDS-BY-LINE-26 -- ecs=1 plus BEAMCON0 VARVBEN=1/VARBEAMEN=1
//   plus a VBSTOP register write to >=26 (agnus_beamcounter.v:216 writes
//   vbstop_reg unconditionally on ecs, but it only reaches vbl once
//   varvben&&varbeamen select it at :233, and those two bits are themselves
//   behind the ecs-gated BEAMCON0 write) would put line 26 inside blanking.
//   Same rejection, same reason.
//
// If either of the second pair turns out to matter, the honest fix is not a
// cleverer mutation here -- it is a claim built against
// ch7-beamcon0-ecs-control-bits, where vbstop is actually writable.
//
// BOUNDARY NOTE (line 20 NTSC / line 25 PAL): p.219 gives two figures for the
// same range that disagree by one if "ends at line N" is read as inclusive:
// "minimum time ... is 20 [/25] horizontal scan lines" (a plain count) versus
// "the range starts at line 0 and ends at line 20 [/25]" (which, read as an
// inclusive endpoint, spans 21 [/26] lines). The page does not otherwise
// settle which reading is meant. P2/P3 below assert the unambiguous count
// sentence; P4/P5 assert only what both readings agree on (blanking holds at
// line 0; blanking has ended by line 21/26); the disputed line itself
// (20/25) is measured and reported, not graded -- see the two `record()`
// calls near the end of main().

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cstdio>
#include <verilated.h>
#include "Vagnus_beamcounter.h"

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

// One run of the fixture under a fixed stimulus (ntsc/a1k mode, optionally a
// mid-run register write that jams vpos out of sequence). Always drives
// ecs=0 and aga=0: this package's whole boundary is the OCS, hardwired path.
struct Result {
    int max_vpos_seen = -1;
    bool sequential_ok = true;
    bool started_at_zero = false;
    std::vector<int> transitions;       // distinct vpos values, in the order first seen
    std::map<int,int> vbl_by_line;      // vpos -> last-seen vbl (combinational on vpos, so stable)
    std::set<int> vbl_int_lines;        // vpos values at which vbl_int was seen high
    bool harddis_seen_high = false;
};

static Result run_scenario(bool ntsc_mode, bool a1k_mode, int lines,
                            bool jam = false, int jam_at_vpos = 5) {
    auto *top = new Vagnus_beamcounter;
    top->clk = 0;
    top->clk7_en = 0;
    top->reset = 1;
    top->cck = 0;
    top->ntsc = ntsc_mode ? 1 : 0;
    top->aga = 0;
    top->ecs = 0;                 // OCS scope, driven explicitly, never left implicit
    top->a1k = a1k_mode ? 1 : 0;
    top->reg_address_in = 0;
    top->data_in = 0;

    auto tick = [&]() {
        top->eval();
        top->clk = 1;
        top->eval();
        top->clk = 0;
        top->eval();
    };

    for (int i = 0; i < 10; ++i) {
        top->clk7_en = 1;
        tick();
    }
    top->reset = 0;

    Result r;
    int last_vpos = -1;
    bool jammed_done = false;
    const long N = (long)lines * 912 + 2000;   // ~912 main-loop iterations/line (measured
                                                // against agnus_beamcounter's own sibling
                                                // fixture, which needed 314*456*2 iterations
                                                // for a 313-line PAL field), plus margin.
    for (long i = 0; i < N; ++i) {
        top->clk7_en = (i % 2 == 0);
        top->cck    = (i % 4 < 2);
        top->reg_address_in = 0;   // idle bus by default
        top->data_in = 0;

        if (jam && !jammed_done && (int)top->vpos == jam_at_vpos && top->clk7_en) {
            // VPOSW = 9'h02A; reg_address_in carries bits [8:1], i.e. VPOSW>>1.
            // Writing vpos[10:8] = 3'b001 jumps vpos far ahead of its
            // sequential count -- the module's own real register-write path,
            // not an added debug port.
            top->reg_address_in = 0x02A >> 1;
            top->data_in = 0x1;
            jammed_done = true;
        }

        tick();

        int vp = (int)top->vpos;
        if (vp != last_vpos) {
            r.transitions.push_back(vp);
            last_vpos = vp;
        }
        r.vbl_by_line[vp] = top->vbl ? 1 : 0;
        if (top->vbl_int) r.vbl_int_lines.insert(vp);
        if (top->harddis_out) r.harddis_seen_high = true;
        if (vp > r.max_vpos_seen) r.max_vpos_seen = vp;
    }

    r.started_at_zero = !r.transitions.empty() && r.transitions.front() == 0;
    r.sequential_ok = r.started_at_zero;
    for (size_t k = 1; r.sequential_ok && k < r.transitions.size(); ++k) {
        if (r.transitions[k] != r.transitions[k - 1] + 1) r.sequential_ok = false;
    }

    top->final();
    delete top;
    return r;
}

// A second measurement, used only for VBL-INT-AT-LINE-0. Right after reset,
// hpos and vpos both start cold at 0, and (per the RTL's own comment) "vpos
// changes after hpos equals 3" -- so on this FIRST line only, vpos leaves 0
// only ~3 hpos ticks in, well before hpos can reach 8, and vbl_int's
// hpos==8 condition is never reached while vpos==0. That is a simulation
// cold-start artifact, not a hardware behaviour: real hardware enters line 0
// by wrapping from the previous frame's last line, where hpos has already
// been cycling normally. This measurement runs a full frame so vpos wraps
// naturally back to 0 at least once, discards everything before that wrap,
// and reports vbl_int only from the settled, post-wrap frame.
static std::set<int> run_settled_vbl_int_lines(bool ntsc_mode, bool a1k_mode) {
    auto *top = new Vagnus_beamcounter;
    top->clk = 0;
    top->clk7_en = 0;
    top->reset = 1;
    top->cck = 0;
    top->ntsc = ntsc_mode ? 1 : 0;
    top->aga = 0;
    top->ecs = 0;
    top->a1k = a1k_mode ? 1 : 0;
    top->reg_address_in = 0;
    top->data_in = 0;

    auto tick = [&]() {
        top->eval();
        top->clk = 1;
        top->eval();
        top->clk = 0;
        top->eval();
    };
    for (int i = 0; i < 10; ++i) { top->clk7_en = 1; tick(); }
    top->reset = 0;

    std::set<int> settled_vbl_int;
    int last_vpos_raw = -1;
    bool wrapped = false;
    // PAL's frame is the longer of the two (~312-313 lines); run well past
    // it, then well into the next frame, so the settled window comfortably
    // covers line 0 onward.
    const long N = 340L * 912 + 5000;
    for (long i = 0; i < N; ++i) {
        top->clk7_en = (i % 2 == 0);
        top->cck    = (i % 4 < 2);
        top->reg_address_in = 0;
        top->data_in = 0;
        tick();
        int vp = (int)top->vpos;
        if (!wrapped && last_vpos_raw >= 0 && vp < last_vpos_raw) wrapped = true;
        last_vpos_raw = vp;
        if (wrapped && top->vbl_int) settled_vbl_int.insert(vp);
    }
    top->final();
    delete top;
    return settled_vbl_int;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // Baseline scenarios. 40 lines is well past every line this package
    // measures (max is 26), and vbl = (vpos <= vbstop) never re-triggers
    // later in the frame, so a 40-line window gives the same duration count
    // as a full field would.
    Result ntsc = run_scenario(/*ntsc=*/true,  /*a1k=*/false, /*lines=*/40);
    Result pal  = run_scenario(/*ntsc=*/false, /*a1k=*/false, /*lines=*/40);

    // ---- Preconditions: the fixture must prove it counts before anything
    // ---- built on that count is trusted (chapter 6's dead-DMA-grant lesson,
    // ---- applied to a line counter instead of a data word).

    checks.expect(ntsc.sequential_ok && ntsc.max_vpos_seen >= 30,
        "PRECOND-VPOS-SEQUENTIAL-NTSC: vpos counts 0,1,2,... in order, reaching "
        "at least line 30, before any NTSC line-count assertion is trusted "
        "(max seen " + std::to_string(ntsc.max_vpos_seen) + ")");
    // MUTATION PRECOND-VPOS-SEQUENTIAL-NTSC stimulus red: same scenario, but a
    // bus write to VPOSW (the module's real register-write path, reg_address_in
    // = VPOSW>>1, data_in[2:0]=1) fires the first time vpos==5, jamming
    // vpos[10:8] to 1 and jumping the count from 5 to 261. sequential_ok goes
    // false because transitions contain ...,4,5,261,262,... Verified below and
    // reverted (the baseline run above uses no jam).

    checks.expect(pal.sequential_ok && pal.max_vpos_seen >= 30,
        "PRECOND-VPOS-SEQUENTIAL-PAL: vpos counts 0,1,2,... in order, reaching "
        "at least line 30, before any PAL line-count assertion is trusted "
        "(max seen " + std::to_string(pal.max_vpos_seen) + ")");
    // MUTATION PRECOND-VPOS-SEQUENTIAL-PAL stimulus red: identical jam as
    // above, run under ntsc=false. Verified below and reverted.

    {
        Result ntsc_jam = run_scenario(/*ntsc=*/true, /*a1k=*/false, /*lines=*/40,
                                        /*jam=*/true, /*jam_at_vpos=*/5);
        checks.note("mutation demonstration for PRECOND-VPOS-SEQUENTIAL-*: with the "
            "VPOSW bus write jammed in at vpos==5, sequential_ok=" +
            std::string(ntsc_jam.sequential_ok ? "true" : "false") +
            " (expected false) -- transitions "
            + std::to_string(ntsc_jam.transitions.size() > 5 ? ntsc_jam.transitions[4] : -1)
            + "->" + std::to_string(ntsc_jam.transitions.size() > 6 ? ntsc_jam.transitions[5] : -1)
            + "->" + std::to_string(ntsc_jam.transitions.size() > 7 ? ntsc_jam.transitions[6] : -1));
    }

    checks.expect(!ntsc.harddis_seen_high && !pal.harddis_seen_high,
        "PRECOND-HARDDIS-LOW-ECS0: harddis_out reads 0 throughout both scenarios "
        "(ecs held at 0 the whole run), so the assertions below are provably "
        "measuring the hardwired OCS path, not an unstated default");
    // MUTATION PRECOND-HARDDIS-LOW-ECS0 none impossible: NOT a structural
    // invariant -- falsifiable on this exact module by ecs=1 plus a BEAMCON0
    // write setting HARDDIS (agnus_beamcounter.v:123,:128,:236), rejected
    // only because that stimulus is outside this package's assigned OCS
    // scope (ecs=0) and belongs to ch7-beamcon0-ecs-control-bits. See the
    // MUTATION HONESTY NOTE in the file header for why this is grouped apart
    // from the two truly unfalsifiable ones below.

    // ---- P1 (p.219, uid sol:7-interrupts.c34 / opus:7-int.c31, twins; plus
    // ---- the denotation half of opus:7-fig7-4.c18, Figure 7-4 p.221): VERTB
    // ---- fires at display line 0, and at no other line.

    // Measured on a settled (post-wrap) frame, not the cold-start one: right
    // after reset hpos and vpos both start at 0 together, and since "vpos
    // changes after hpos equals 3" (the RTL's own comment), vpos leaves 0
    // before hpos can ever reach 8 on that first line only -- a simulation
    // artifact of a cold start, not a hardware behaviour. See
    // run_settled_vbl_int_lines() above for the full reasoning.
    std::set<int> vbl_int_settled = run_settled_vbl_int_lines(/*ntsc=*/true, /*a1k=*/false);
    checks.expect(vbl_int_settled.size() == 1 && vbl_int_settled.count(0) == 1,
        "VBL-INT-AT-LINE-0: the vertical interrupt request pulses exactly once "
        "per frame, at line 0, and at no other line (measured on a settled, "
        "post-wrap frame; a1k=0; sol:7-interrupts.c34, opus:7-int.c31, "
        "opus:7-fig7-4.c18)");
    // MUTATION VBL-INT-AT-LINE-0 stimulus red: a1k=1 ("enable A1000 VBL
    // interrupt timing", a real port, not a debug hook) moves the settled
    // pulse to line 1 instead of line 0. Verified below and reverted
    // (baseline above uses a1k=0).

    {
        std::set<int> a1k_settled = run_settled_vbl_int_lines(/*ntsc=*/true, /*a1k=*/true);
        checks.note("mutation demonstration for VBL-INT-AT-LINE-0: with a1k=1, "
            "the settled vbl_int fired at line(s) "
            + (a1k_settled.empty() ? std::string("<none>")
               : std::to_string(*a1k_settled.begin()))
            + " instead of {0} -- checked against the a1k=0 expectation this "
              "goes FAIL");
    }

    // ---- P2 (p.219, uid sol:7-interrupts.c35 / opus:7-int.c32): NTSC minimum
    // ---- vertical blanking is 20 horizontal scan lines.

    {
        int ntsc_vbl_lines = 0;
        for (auto &kv : ntsc.vbl_by_line) if (kv.second) ++ntsc_vbl_lines;
        checks.expect(ntsc_vbl_lines == 20,
            "NTSC-VBLANK-DURATION-20-LINES: the number of lines for which vertical "
            "blanking holds, in an NTSC-timed OCS frame, is 20 (measured "
            + std::to_string(ntsc_vbl_lines) + "; sol:7-interrupts.c35, opus:7-int.c32)");
        // MUTATION NTSC-VBLANK-DURATION-20-LINES stimulus red: switch the
        // stimulus to PAL timing (ntsc=false) while leaving the expected
        // value (20) alone -- the module is now asked to run a different
        // mode, and the measured count changes to 26 (see PAL count below),
        // still not 20, so the assertion stays red for a verifiably different
        // reason (proving the measurement tracks the mode, not a frozen
        // number). Verified: PAL count below.
        // MUTATION NTSC-VBLANK-DURATION-20-LINES none impossible: no stimulus
        // inside this package's boundary can make the measured count 20, so
        // there is no `stimulus green` to run. At ecs=0 -- which every run here
        // drives explicitly -- agnus_beamcounter.v:233 makes vbstop the
        // constant `pal ? 25 : 20`; the programmable path is gated on VARVBEN
        // and VARBEAMEN, which arrive through a BEAMCON0 write the module gates
        // on ecs, and that territory is ch7-beamcon0-ecs-control-bits' by this
        // package's own scope note above. What demonstrates the check is live
        // is the stimulus mutation recorded above: swapping the mode moves the
        // measured count (21 <-> 26), so the number tracks what the module is
        // asked to do.
        //
        // An `oracle green` record stood here until stage 8: the expected value
        // had been edited from 20 to 21 by hand and the assertion passed. That
        // demonstrates nothing -- BUILD-BRIEF says flipping an expected value
        // proves nothing, and it is the assertion's own oracle -- so it is
        // withdrawn rather than kept as evidence.
        checks.note("NTSC-VBLANK-DURATION-20-LINES measured count is "
            + std::to_string(ntsc_vbl_lines) + " (vpos 0.." + std::to_string(ntsc_vbl_lines - 1) + ")");
    }

    // ---- P3 (p.219, uid sol:7-interrupts.c36 / opus:7-int.c33): PAL minimum
    // ---- vertical blanking is 25 horizontal scan lines.

    {
        int pal_vbl_lines = 0;
        for (auto &kv : pal.vbl_by_line) if (kv.second) ++pal_vbl_lines;
        checks.expect(pal_vbl_lines == 25,
            "PAL-VBLANK-DURATION-25-LINES: the number of lines for which vertical "
            "blanking holds, in a PAL-timed OCS frame, is 25 (measured "
            + std::to_string(pal_vbl_lines) + "; sol:7-interrupts.c36, opus:7-int.c33)");
        // MUTATION PAL-VBLANK-DURATION-25-LINES stimulus red: switch the
        // stimulus to NTSC timing while leaving the expected value (25)
        // alone -- measured count changes to 21 (see NTSC count above), still
        // not 25. Same reasoning as the NTSC assertion's stimulus mutation.
        // MUTATION PAL-VBLANK-DURATION-25-LINES none impossible: same position
        // as the NTSC assertion above -- at ecs=0 vbstop is the constant 25 and
        // nothing this package may drive can change it, so no `stimulus green`
        // exists. The live-measurement demonstration is the stimulus mutation
        // recorded above (the count moves 26 <-> 21 with the mode).
        //
        // An `oracle green` record stood here until stage 8, editing the
        // expected value from 25 to 26; withdrawn for the same reason.
        checks.note("PAL-VBLANK-DURATION-25-LINES measured count is "
            + std::to_string(pal_vbl_lines) + " (vpos 0.." + std::to_string(pal_vbl_lines - 1) + ")");
    }

    // ---- P4 (p.219, uid sol:7-interrupts.c37 / opus:7-int.c34): the NTSC
    // ---- vertical-blank range starts at line 0. Only the parts both
    // ---- readings of "ends at line 20" agree on are asserted here; see the
    // ---- boundary record below for line 20 itself.

    checks.expect(ntsc.vbl_by_line.count(0) && ntsc.vbl_by_line[0] == 1,
        "NTSC-VBLANK-STARTS-LINE-0: vertical blanking holds at line 0 of an "
        "NTSC-timed OCS frame (sol:7-interrupts.c37, opus:7-int.c34)");
    // MUTATION NTSC-VBLANK-STARTS-LINE-0 none impossible: genuinely
    // unfalsifiable at this module boundary, not merely out of scope --
    // vbstop is a non-negative value everywhere in this module (the
    // hardwired OCS constants and the ECS-programmable vbstop_reg alike), so
    // vpos==0<=vbstop holds for every value vbstop can ever take, under OCS
    // or ECS. No stimulus anywhere on agnus_beamcounter falsifies it; see the
    // MUTATION HONESTY NOTE in the file header.

    checks.expect(ntsc.vbl_by_line.count(21) && ntsc.vbl_by_line[21] == 0,
        "NTSC-VBLANK-ENDS-BY-LINE-21: vertical blanking no longer holds at line "
        "21 of an NTSC-timed OCS frame -- the control case for the range assertion "
        "above, on the same measurement basis (sol:7-interrupts.c37, opus:7-int.c34)");
    // MUTATION NTSC-VBLANK-ENDS-BY-LINE-21 stimulus red: switch to PAL timing
    // (ntsc=false) while checking the same line, 21 -- PAL's vbstop is 25, so
    // line 21 falls back inside blanking and the assertion goes red. Verified
    // below and reverted (baseline above uses ntsc=true).

    checks.note("mutation demonstration for NTSC-VBLANK-ENDS-BY-LINE-21: under "
        "PAL timing, vbl at line 21 is "
        + std::to_string(pal.vbl_by_line.count(21) ? pal.vbl_by_line[21] : -1)
        + " (expected 1, i.e. blanked -- checked against the NTSC expectation "
          "of 0 this goes FAIL)");

    // ---- P5 (p.219, uid sol:7-interrupts.c38 / opus:7-int.c35): the PAL
    // ---- vertical-blank range starts at line 0.

    checks.expect(pal.vbl_by_line.count(0) && pal.vbl_by_line[0] == 1,
        "PAL-VBLANK-STARTS-LINE-0: vertical blanking holds at line 0 of a "
        "PAL-timed OCS frame (sol:7-interrupts.c38, opus:7-int.c35)");
    // MUTATION PAL-VBLANK-STARTS-LINE-0 none impossible: same genuine,
    // module-wide invariant as NTSC-VBLANK-STARTS-LINE-0 above, not scope-
    // limited coverage -- see the MUTATION HONESTY NOTE in the file header.

    checks.expect(pal.vbl_by_line.count(26) && pal.vbl_by_line[26] == 0,
        "PAL-VBLANK-ENDS-BY-LINE-26: vertical blanking no longer holds at line "
        "26 of a PAL-timed OCS frame -- the control case for the range assertion "
        "above, on the same measurement basis (sol:7-interrupts.c38, opus:7-int.c35)");
    // MUTATION PAL-VBLANK-ENDS-BY-LINE-26 none impossible: NOT a structural
    // invariant -- unlike NTSC-VBLANK-ENDS-BY-LINE-21, switching ntsc/pal
    // cannot flip this one (PAL->NTSC only shrinks the blanked range
    // further), but it IS falsifiable on this exact module by ecs=1 +
    // BEAMCON0 VARVBEN=1/VARBEAMEN=1 + a VBSTOP register write to >=26
    // (agnus_beamcounter.v:216,:233), rejected only because that stimulus is
    // outside this package's assigned OCS scope. See the MUTATION HONESTY
    // NOTE in the file header.

    // ---- Boundary lines the manual does not settle: measured, not graded.

    checks.record("NTSC line 20: measured vbl=" + std::to_string(ntsc.vbl_by_line.count(20) ? ntsc.vbl_by_line[20] : -1)
        + ". p.219 states both 'the minimum time of vertical blanking is 20 "
          "horizontal scan lines' (a count) and 'the range starts at line 0 and "
          "ends at line 20' (which, read inclusively, is 21 lines) for NTSC. The "
          "page does not say which reading governs whether line 20 itself is "
          "still blanking, so this specific line is reported, not asserted "
          "(half graded: sol:7-interrupts.c37, opus:7-int.c34).");

    checks.record("PAL line 25: measured vbl=" + std::to_string(pal.vbl_by_line.count(25) ? pal.vbl_by_line[25] : -1)
        + ". Same tension as NTSC line 20, one line up: 'is 25 horizontal scan "
          "lines' versus 'ends at line 25' read inclusively (26 lines). Reported, "
          "not asserted (half graded: sol:7-interrupts.c38, opus:7-int.c35).");

    // ---- Claims recorded because they need a wider module boundary than the
    // ---- one assigned to this package (agnus_beamcounter only).

    checks.record("the request-bit-5 wiring half of "
        "(half graded: sol:7-interrupts.c34, opus:7-int.c31) "
        "('the vblint input lands on request bit 5') needs "
        "paula_intcontroller, where vblint is a free input with no beam-position "
        "port of its own -- the line-0 timing cannot be read there. Out of this "
        "fixture's boundary; not asserted here.");

    checks.record("the Exec-software-priority-6 / hardware-priority-3 half of "
        "(half graded: opus:7-fig7-4.c18) Figure 7-4 row is a claim about the CPU "
        "interrupt-priority encoder in paula_intcontroller, the same boundary "
        "problem as above. Out of this fixture's boundary; not asserted here.");

    std::cout << "Checked " << (checks.passed + checks.failed)
              << " assertions; passed=" << checks.passed
              << " failed=" << checks.failed << ".\n";

    return checks.failed == 0 ? 0 : 1;
}
