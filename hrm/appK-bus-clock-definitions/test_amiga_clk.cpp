// Test purpose: p.393 "CLOCK SIGNALS" (Zorro II) and p.410 "CLOCK SIGNALS"
//   (Zorro III), amiga_clk.v alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 393, 410
// Non-goals: anything outside the `amiga_clk` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-bus-clock-definitions.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// appK-bus-clock-definitions -- p.393 "CLOCK SIGNALS" (Zorro II) and p.410
// "CLOCK SIGNALS" (Zorro III), amiga_clk.v alone. 31 claims, from
// hrm-testpipeline/work/appK-build/appK-bus-clock-definitions.jsonl. See
// SPECIFICATION.md for the full per-claim rationale; this header only
// restates the RTL facts and the mutation plan.
//
// RTL FACTS (amiga_clk.v, pinned submodule, read in full -- 67 lines, no
// submodule instantiations):
//
//   :6-13  ports -- clk_28 (in), clk7_en, clk7n_en, c1, c3, cck, eclk[9:0]
//          (out), reset_n (in, async, active low). No video-mode input.
//   :19    reg [1:0] clk7_cnt = 2'b10;  -- free-running divider off clk_28
//   :24-29 reset branch: clk7_cnt<=2'b10, clk7_en_reg<=1, clk7n_en_reg<=1,
//          cck<=1, shifter<=1. c1/c3 are NOT in this always block -- they
//          have no reset input of their own at all.
//   :31-33 clk7_cnt<=clk7_cnt+1; clk7_en_reg<=(clk7_cnt==0);
//          clk7n_en_reg<=(clk7_cnt==2) -- both one clk_28 cycle delayed.
//   :34-38 on clk7_cnt==1: cck<=~cck (once per 4-edge period -- cck's own
//          full period is 8 edges); shifter rotates one position.
//   :42    wire clk_7 = clk7_cnt[1];  -- clk_28/4, a 4-edge-period signal.
//   :43-44 clk7_en = clk7_en_reg; clk7n_en = clk7n_en_reg -- the module's
//          own rising/falling markers for the internal clk_7 (there is no
//          "7M" port to read directly).
//   :58    c3 <= clk_7;   -- one clk_28 edge (90 degrees of clk_7) later.
//   :61    c1 <= ~c3;     -- ends up back in phase with clk_7 itself, not a
//          half-rate divided copy of it (measured below).
//   :64    eclk = shifter -- ten-bit one-hot, one bit set at all times.
//
// MEASURED, not assumed (see SPECIFICATION.md "What each claim asserts"):
// c1 and c3 both run at clk_28/4 -- the SAME ratio as clk_7/7M itself, not
// clk_28/8 (the 3.58/3.55 MHz-class ratio /C1 and /C3 are printed as). The
// signal in this design that actually has the 3.5x MHz-class period is
// cck, under neither printed name. c3 is delayed from c1 by exactly one
// clk_28 edge (90 degrees) -- CDAC's printed definition on p.410, exactly,
// even though c3 fails /C3's own frequency claim on the same two pages.
//
// WHY THE FIFTEEN /C1 AND /C3 UIDS ARE DECLINED. Their measurements are
// still made and printed, in the note() lines above the grading block; what
// those numbers cannot be is bound to /C1 or /C3.
//
//   /C1 and /C3 are pins on the Zorro expansion connector. NanoMig has no
//   such connector, so it has no /C1 and no /C3. The ports named c1 and c3
//   on this module are not those pins under another spelling, and three
//   places in the design say so, none of them written for this test:
//   amiga_clk.v:47-54 draws c1 tracking clk_7 edge for edge with c3 90
//   degrees behind it; denise_bitplanes.v:222 says "c1 is the 7MHz clock,
//   c3 the same clock shifted 90deg"; minimig_sram_bridge.v:46 labels its
//   c1 waveform "(7.09 MHz)". By the design's own account c1 and c3 are 7M
//   and CDAC -- which is exactly what this fixture measures them to be.
//   A binding of /C1 and /C3 to c1 and c3 would rest on the name alone, and
//   the name is Minimig's, not the manual's.
//
//   So the question the discipline asks -- not "does this pass?" but "does
//   the printed page require what this asserts?" -- answers no. p.393 does
//   not require that a Verilog net called c1 run at 3.58 MHz. It requires
//   that a bus pin called /C1 do so, and there is no such pin here.
//
// The /C1 and /C3 claims are NOT bound to cck either. cck does have the
// printed 3.55 MHz rate and the port comment "colour clock output (3.54
// MHz)" -- but no claim on either page names it, the page describes a
// QUADRATURE PAIR while this design carries one colour-clock phase and no
// second one, and choosing that binding after seeing the result is the move
// this project exists to refuse. cck is measured and noted; never graded.
//
// 7M and CDAC are graded, and the line between them and /C1, /C3 is a
// measured one rather than a preference: the design names those two.
// clk7_en's port comment is "7MHz output clock enable"; c3's is "delayed by
// 90 degrees", which is CDAC's printed definition word for word. Nothing in
// the design names anything /C1 or /C3.
//
// MUTATION PLAN. This module's only inputs are clk_28 (the reference the
// harness measures IN, not a stimulus) and reset_n. One family: every
// graded uid is baseline-PASS and is turned red
// by stimulus via MUT_RESETHOLD (make test MUT=RESETHOLD) -- reset_n is
// never released, so clk7_en/clk7n_en freeze high (:26-27, forced, never
// computed from clk7_cnt again) and c1/c3/cck freeze at reset-forced
// constants a cycle or two later, and every period/pulse measurement below
// finds no transitions. Declined uids carry no MUTATION line.
//
// MUTATION sol:K-0135 stimulus red MUT_RESETHOLD (clk7_en frozen high, no pulses)
// MUTATION opus:K-0142 stimulus red (same MUT_RESETHOLD as sol:K-0135)
// MUTATION sol:K-0411 stimulus red (same MUT_RESETHOLD as sol:K-0135)
// MUTATION opus:K-0421 stimulus red (same MUT_RESETHOLD as sol:K-0135)
// MUTATION sol:K-0407 stimulus red (same MUT_RESETHOLD as sol:K-0135 -- c3 freezes)
// MUTATION sol:K-0408 stimulus red (same MUT_RESETHOLD -- both c1 and c3 freeze, no offset)
// MUTATION opus:K-0418 stimulus red (same MUT_RESETHOLD as sol:K-0407/K-0408)

#include "Vamiga_clk.h"
#include "verilated.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdint>

static Vamiga_clk* top;
static vluint64_t sim_time = 0;
static int passed = 0, failed = 0;

// ---- the fixed harness (character-identical across the suite) ----
void expect(bool cond, const std::string& label) {
    if (cond) { std::cout << "PASS: " << label << "\n"; passed++; }
    else      { std::cout << "FAIL: " << label << "\n"; failed++; }
}
void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// ---- clocking ----
void tick() {
    top->clk_28 = 0; top->eval(); sim_time++;
    top->clk_28 = 1; top->eval(); sim_time++;
}
void settle(int n) { for (int i = 0; i < n; i++) tick(); }

// ---- trace capture: one sample per tick, all five outputs together ----
struct Sample {
    bool c1, c3, cck, clk7_en, clk7n_en;
    uint32_t eclk;
};

std::vector<int> all_transitions(const std::vector<Sample>& tr, bool Sample::*f) {
    std::vector<int> v;
    for (size_t i = 1; i < tr.size(); i++)
        if ((tr[i-1].*f) != (tr[i].*f)) v.push_back((int)i);
    return v;
}
std::vector<int> rising_edges(const std::vector<Sample>& tr, bool Sample::*f) {
    std::vector<int> v;
    for (size_t i = 1; i < tr.size(); i++)
        if (!(tr[i-1].*f) && (tr[i].*f)) v.push_back((int)i);
    return v;
}
// Steady-state period in clk_28 edges: distance between the 1st and 3rd
// captured transition (two same-direction transitions), skipping nothing
// else since the trace is captured well after reset settle.
int period_of(const std::vector<int>& transitions) {
    if (transitions.size() < 3) return -1;
    return transitions[2] - transitions[0];
}
// Fraction of `markers` (rising-edge ticks of a pulse signal) coinciding
// with a transition of `target` at the SAME clk_28 edge or the very next
// one (a 2-tick window -- wide enough to catch either lag a one-register
// hop away from the raw divider can produce, tight enough that an
// unrelated signal cannot match by coincidence at this module's 4-edge
// period). Checked both ways (against clk7_en and against clk7n_en) so the
// actual lag, whatever it is, is measured rather than assumed.
double follows_fraction(const std::vector<int>& markers, const std::vector<int>& target_transitions) {
    if (markers.empty()) return -1.0;
    std::set<int> tset(target_transitions.begin(), target_transitions.end());
    int hit = 0;
    for (int m : markers) if (tset.count(m) || tset.count(m + 1)) hit++;
    return (double)hit / (double)markers.size();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vamiga_clk;

    top->clk_28 = 0;
    top->reset_n = 0; // assert reset (active low)
    top->eval();
    settle(5); // run a few cycles held in reset

#ifndef MUT_RESETHOLD
    top->reset_n = 1; // release
    settle(12); // let the divider reach steady periodic state before capture
#else
    settle(12); // mutation: reset_n never released, everything stays frozen
#endif

    note("boundary: amiga_clk has no video-mode input (port list is clk_28, "
         "clk7_en, clk7n_en, c1, c3, cck, eclk[9:0], reset_n -- confirmed by "
         "reading amiga_clk.v in full) -- so every printed NTSC/PAL pair "
         "(3.58/3.55 MHz, 7.16/7.09 MHz) is graded here as a ratio to clk_28 "
         "(the same hardware divider either way), never as one selected "
         "absolute-Hz figure; which of the two numerals applies depends on "
         "the board's crystal, outside this module's boundary.");

    // =====================================================================
    // Apparatus liveness -- proven before any claim is graded, carries no uid.
    // =====================================================================
    {
        std::vector<Sample> probe;
        for (int i = 0; i < 20; i++) {
            tick();
            probe.push_back({top->c1 != 0, top->c3 != 0, top->cck != 0,
                              top->clk7_en != 0, top->clk7n_en != 0,
                              (uint32_t)top->eclk});
        }
        auto pulses = rising_edges(probe, &Sample::clk7_en);
        note("apparatus check: clk7_en rising edges in 20 post-reset ticks = " +
             std::to_string(pulses.size()));
        expect(pulses.size() >= 3,
               "clk7_en pulses periodically after reset release "
               "[not graded -- apparatus check, no claim uid]");
    }

    // =====================================================================
    // Main trace: 200 ticks (50 periods of the 4-edge divider), captured in
    // one continuous run so every claim below reads the same measurement.
    // =====================================================================
    std::vector<Sample> tr;
    const int TRACE_LEN = 200;
    for (int i = 0; i < TRACE_LEN; i++) {
        tick();
        tr.push_back({top->c1 != 0, top->c3 != 0, top->cck != 0,
                      top->clk7_en != 0, top->clk7n_en != 0,
                      (uint32_t)top->eclk});
    }

    auto c1_tr   = all_transitions(tr, &Sample::c1);
    auto c3_tr   = all_transitions(tr, &Sample::c3);
    auto cck_tr  = all_transitions(tr, &Sample::cck);
    auto en_rise  = rising_edges(tr, &Sample::clk7_en);
    auto nen_rise = rising_edges(tr, &Sample::clk7n_en);

    int c1_period  = period_of(c1_tr);
    int c3_period  = period_of(c3_tr);
    int cck_period = period_of(cck_tr);
    int sevenm_period = (en_rise.size() >= 2) ? (en_rise[1] - en_rise[0]) : -1;

    note("c1 period = " + std::to_string(c1_period) + " clk_28 edges (" +
         std::to_string(c1_tr.size()) + " transitions in " + std::to_string(TRACE_LEN) + " ticks)");
    note("c3 period = " + std::to_string(c3_period) + " clk_28 edges (" +
         std::to_string(c3_tr.size()) + " transitions in " + std::to_string(TRACE_LEN) + " ticks)");
    note("cck period = " + std::to_string(cck_period) + " clk_28 edges (" +
         std::to_string(cck_tr.size()) + " transitions in " + std::to_string(TRACE_LEN) + " ticks)");
    note("7M reference period (clk7_en pulse-to-pulse) = " + std::to_string(sevenm_period) +
         " clk_28 edges (" + std::to_string(en_rise.size()) + " pulses in " + std::to_string(TRACE_LEN) + " ticks)");

    // Phase offset: clk_28 edges from each c1 transition to the next c3
    // transition (counted, not assumed -- averaged/checked for constancy
    // across the whole trace).
    int phase_offset = -1;
    bool phase_constant = true;
    {
        std::vector<int> offsets;
        size_t n = std::min(c1_tr.size(), c3_tr.size());
        for (size_t i = 0; i < n; i++) offsets.push_back(c3_tr[i] - c1_tr[i]);
        if (!offsets.empty()) {
            phase_offset = offsets[0];
            for (int o : offsets) if (o != phase_offset) phase_constant = false;
        }
        note("c1-to-c3 transition offset: " + std::to_string(offsets.size()) +
             " pairs measured, offset = " + std::to_string(phase_offset) +
             " clk_28 edges each" + (phase_constant ? " (constant)" : " (NOT constant)") +
             " = " + std::to_string(phase_offset) + "/" + std::to_string(c1_period) +
             " of a period, " + std::to_string(phase_offset * 360 / (c1_period > 0 ? c1_period : 1)) +
             " degrees, " + std::to_string(phase_offset) + " x 35.2ns at 28.375160MHz clk_28");
    }

    double c1_follows_rise  = follows_fraction(en_rise, c1_tr);
    double c1_follows_fall  = follows_fraction(nen_rise, c1_tr);
    double c3_follows_rise  = follows_fraction(en_rise, c3_tr);
    double c3_follows_fall  = follows_fraction(nen_rise, c3_tr);
    double cck_follows_rise = follows_fraction(en_rise, cck_tr);
    double cck_follows_fall = follows_fraction(nen_rise, cck_tr);

    note("c1 transitions following clk7_en (rising marker) pulses: " + std::to_string(c1_follows_rise) +
         "; following clk7n_en (falling marker) pulses: " + std::to_string(c1_follows_fall));
    note("c3 transitions following clk7_en (rising marker) pulses: " + std::to_string(c3_follows_rise) +
         "; following clk7n_en (falling marker) pulses: " + std::to_string(c3_follows_fall));
    note("cck transitions following clk7_en (rising marker) pulses: " + std::to_string(cck_follows_rise) +
         "; following clk7n_en (falling marker) pulses: " + std::to_string(cck_follows_fall) +
         " -- oracle-green control for the sync-exclusivity comparator");

    // eclk one-hot invariant, checked on every sample -- backs the record()
    // decisions for the four E-clock duty uids below.
    {
        int violations = 0, popcount_sum = 0;
        std::set<int> positions_seen;
        for (auto& s : tr) {
            int pc = 0, pos = -1;
            for (int b = 0; b < 10; b++) if (s.eclk & (1u << b)) { pc++; pos = b; }
            popcount_sum += pc;
            if (pc != 1) violations++; else positions_seen.insert(pos);
        }
        note("eclk one-hot check: " + std::to_string(violations) + "/" + std::to_string(tr.size()) +
             " samples did NOT have exactly one bit set; distinct set-bit positions observed: " +
             std::to_string(positions_seen.size()) + "/10 -- eclk is a rotating ten-phase enable "
             "vector, never a two-level signal, at this port");
    }

    // =====================================================================
    // GRADED -- 7 uids, all PASS. The fifteen /C1 and /C3 uids are declined
    // below; see the note at the head of this file on why. Their
    // measurements are printed above.
    // =====================================================================

    // --- 7M frequency (four uids, same measurement) ---
    expect(sevenm_period == 4,
           "sol:K-0135 7M = 7.16/7.09 MHz: clk7_en pulse period = " + std::to_string(sevenm_period) +
           " clk_28 edges (clk_28/4 expected)");
    expect(sevenm_period == 4,
           "opus:K-0142 7M = 7.16/7.09 MHz (same measurement as sol:K-0135)");
    expect(sevenm_period == 4,
           "sol:K-0411 7M = 7.16/7.09 MHz (p.410, same measurement as sol:K-0135)");
    expect(sevenm_period == 4,
           "opus:K-0421 7M = 7.16/7.09 MHz (p.410, same measurement as sol:K-0135)");

    // --- CDAC frequency and phase (p.410 only) ---
    // The `sevenm_period > 0` term is not decoration. Both period_of() and the
    // 7M reference return -1 when the trace holds too few transitions to
    // measure, and `c3_period == sevenm_period` alone is satisfied by -1 == -1
    // -- so under MUT_RESETHOLD, where nothing transitions at all, the
    // equality alone would pass vacuously.
    expect(sevenm_period > 0 && c3_period == sevenm_period,
           "sol:K-0407 CDAC = 7.16/7.09 MHz: c3 period = " + std::to_string(c3_period) +
           " clk_28 edges, 7M reference period = " + std::to_string(sevenm_period) +
           " (equal, and both actually measured, expected)");
    expect(phase_constant && phase_offset == 1,
           "sol:K-0408 CDAC trails 7M by 90 degrees (~35ns): c1-to-c3 offset = " +
           std::to_string(phase_offset) + " clk_28 edge(s) (1 expected = 35.2ns at 28.375160MHz = 90 "
           "degrees of a " + std::to_string(c1_period) + "-edge period)");
    expect((c3_period == sevenm_period) && phase_constant && phase_offset == 1,
           "opus:K-0418 CDAC = 7.16/7.09 MHz AND trails 7M by 90 degrees/~35ns (combined, same "
           "measurements as sol:K-0407/sol:K-0408)");

    // =====================================================================
    // DECLINED -- 24 uids, grouped by reason. No expect() for these.
    // =====================================================================

    // --- /C1 and /C3: the printed subject is not in this design (15 uids) ---
    record("sol:K-0122, sol:K-0125 -- declined: /C1 and /C3 are pins on the Zorro expansion "
           "connector, and NanoMig has no such connector, so the existence these two headings "
           "assert has no subject here. The ports named c1 and c3 on this module are not those "
           "pins under another spelling: amiga_clk.v:47-54 draws c1 tracking clk_7 edge for edge "
           "with c3 90 degrees behind it, denise_bitplanes.v:222 calls c1 'the 7MHz clock, c3 the "
           "same clock shifted 90deg', and minimig_sram_bridge.v:46 labels its c1 waveform "
           "'(7.09 MHz)'. Measured here and printed above: c1 period = " + std::to_string(c1_period) +
           " clk_28 edges, c3 period = " + std::to_string(c3_period) + ", both equal to 7M's own " +
           std::to_string(sevenm_period) + ". See the note at the head of this file on why the "
           "fifteen /C1 and /C3 uids are declined.");
    record("sol:K-0123, sol:K-0126, sol:K-0402, sol:K-0405 -- declined, same reason as "
           "sol:K-0122/sol:K-0125: no /C1 or /C3 exists in this design to carry the printed 3.58/3.55 "
           "MHz. Measured for the record: c1 and c3 both run at clk_28/" + std::to_string(sevenm_period) +
           ", the 7M ratio, not clk_28/" + std::to_string(2 * sevenm_period) + ". The signal in this "
           "design with the printed 3.5x MHz-class period is cck (measured above: period = " +
           std::to_string(cck_period) + " edges; port comment 'colour clock output (3.54 MHz)'), and "
           "cck is NOT graded under either printed name -- no claim on either page names it, the page "
           "describes a quadrature PAIR while this design carries one colour-clock phase, and binding "
           "a claim to a signal chosen after seeing the result is not a reading.");
    record("sol:K-0124, sol:K-0127, sol:K-0403, sol:K-0406 -- declined, same reason as "
           "sol:K-0122/sol:K-0125: the edge-synchronisation claims are about /C1 and /C3, which this "
           "design does not have. Measured for the record: c1 follows the falling marker on " +
           std::to_string(c1_follows_fall * 100) + "% of pulses and the rising marker on " +
           std::to_string(c1_follows_rise * 100) + "%, c3 follows rising on " +
           std::to_string(c3_follows_rise * 100) + "% and falling on " +
           std::to_string(c3_follows_fall * 100) + "% -- both follow both markers, which is what a "
           "quadrature pair at 7M's own rate does and not what a half-rate clock synched to one edge "
           "does.");
    record("opus:K-0136, opus:K-0137, opus:K-0416, opus:K-0417 -- declined, same reason as "
           "sol:K-0122/sol:K-0125: this reader bundles the frequency and the edge-synchronisation "
           "halves of the same two printed entries into one uid, and both halves are about /C1 and "
           "/C3. Measurements as for sol:K-0123 and sol:K-0124 above.");
    record("sol:K-0404 -- declined, same reason as sol:K-0122/sol:K-0125: 'the complementary "
           "half-rate bus clock' is a claim about /C3, and there is no /C3 here. Measured for the "
           "record: c3:c1 period ratio " + std::to_string(c3_period) + ":" + std::to_string(c1_period) +
           " (1:1, not the 2:1 a half-rate clock would give) and a constant c1-to-c3 offset of " +
           std::to_string(phase_offset) + " clk_28 edge(s), a quarter period and not a half. That "
           "offset is 90 degrees, which is CDAC's printed definition -- and it is why c3 PASSES the "
           "three CDAC assertions above on the very same measurement. (Their uids are deliberately "
           "not repeated here: a uid named on a record line counts as declined, and those three are "
           "graded.)");

    // --- E clock: outside this module's boundary, or not a level (9 uids) ---
    record("sol:K-0130, sol:K-0131, opus:K-0139 -- declined: both assert something outside "
           "amiga_clk's own boundary -- K-0130/opus:K-0139 name the E clock's SOURCE ('the "
           "68000 generated E clock'), and amiga_clk has no CPU-originated input at all (its "
           "only inputs are clk_28 and reset_n); K-0131/opus:K-0139 name the CONSUMERS "
           "('used for 6800 family peripherals ... and 6502 peripherals'), and no peripheral "
           "(ciaa, ciab) is instantiated in this single-top-module fixture. eclk's own "
           "liveness is measured above regardless (clk7_en pulses periodically; eclk is "
           "confirmed one-hot below) -- the decline is not silent about what the module does show.");
    record("sol:K-0409, opus:K-0419 -- declined, same reason as sol:K-0130/sol:K-0131/opus:K-0139 "
           "(p.410's restatement of the same E-clock identity/consumer claim): no CPU input, no "
           "peripheral instance at amiga_clk's own boundary.");
    record("sol:K-0133, opus:K-0140 -- declined: 'four 7M clocks high, six clocks low' describes "
           "a two-level signal. eclk is a ten-bit one-hot vector (measured above: every sample in "
           "a " + std::to_string(TRACE_LEN) + "-tick trace had exactly one bit set, all 10 "
           "positions observed rotating through) -- not a level, so a duty cycle is not directly "
           "observable at this port without inventing a bit-selection convention this module does "
           "not supply.");
    record("sol:K-0410, opus:K-0420 -- declined, same reason as sol:K-0133/opus:K-0140 (p.410's "
           "restatement of the same E-clock duty claim): eclk is one-hot, not a level.");

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
