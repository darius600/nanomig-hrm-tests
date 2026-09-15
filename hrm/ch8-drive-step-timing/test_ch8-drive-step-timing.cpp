// Test purpose: Grade the Chinon access/read/write timing diagrams (Figures
//   8-7, 8-8, 8-9, pp.242-243) and the DSKDIREC seek-direction text (p.245)
//   against paula_floppy's own step-acceptance mechanism.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 242-245; SPECIFICATION.md in this directory.
// Non-goals: absolute microsecond/millisecond values (the pages are printed
//   "with a Chinon drive" and warn the figures change with other vendors'
//   drives -- only the dimensionless content each claim reduces to is
//   graded); the MFM/SD-card data path; DMA; interrupts.
//
// MUTATION RATIO-A oracle green: expected ratio changed from 6 (the printed
//   18ms:3ms) to 1 (what the RTL actually produces) -> PASS, confirming the
//   comparator is sensitive and the baseline red is a genuine measurement.
//   Reverted.
// MUTATION RATIO-B oracle green: expected relationship changed from "equal"
//   to "differ by exactly 1 tick" (0 vs 1, what the RTL actually produces)
//   -> PASS. Reverted.
// MUTATION SEL-GATING stimulus red: the second step attempt changed to run
//   deselected too (drive reselected only afterward, to read _track0
//   without its deselected-mask -- paula_floppy.v:666 forces _track0
//   deasserted whenever no drive is selected, independent of dsktrack, so
//   the read-back has to happen while selected even when the step itself
//   does not) -> FAIL as expected (dsktrack was never actually incremented).
//   Reverted.
// MUTATION PULSE-WIDTH-FLOOR stimulus red: do_step's low phase changed from
//   1 tick to 0 (the call that lowers _step is skipped, so _step never
//   transitions) -> FAIL as expected (no rising edge is ever produced, so
//   the module never accepts the step). Reverted.
// MUTATION TRACK00-POLARITY stimulus red: the outward step that is supposed
//   to leave track 0 is skipped -> FAIL as expected (the trial never leaves
//   track 0, so the "TRACK00 deasserts once off track 0" half of the
//   assertion is checked against a state that never occurred). Reverted.
// MUTATION STEP-POLARITY stimulus red: _step is never raised back to 1 after
//   being lowered (the module is never given a rising edge at all) -> FAIL
//   as expected (track never advances, on the rising edge or otherwise).
//   Reverted.
// MUTATION DIREC-PROTOCOL stimulus red: direc left at 0 for both the first
//   AND the second step (instead of 0 then 1), so the second step increments
//   again instead of decrementing -> FAIL as expected (track ends up at 2,
//   not back at 0). Reverted.
// MUTATION FIXTURE-LIVE stimulus red: the outward step is skipped entirely
//   -> FAIL as expected (_track0 never leaves its reset value, so the rig
//   cannot be shown to deliver a second, distinct value). Reverted.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vpaula_floppy.h"

namespace {

// -----------------------------------------------------------------------
// Rig. paula_floppy's `dsktrack` array carries no reset term
// (paula_floppy.v:671-680: the increment/decrement always-block has no
// `if (reset)` branch at all), so a reused Verilated model silently carries
// head position from a previous trial into the next one. Every trial in this
// file allocates its own fresh Vpaula_floppy and throws it away afterwards --
// this is not an optimisation to skip.
// -----------------------------------------------------------------------
class Trial {
public:
    Trial() : dut(new Vpaula_floppy) {
        dut->clk = 0;
        dut->clk7_en = 1;   // held high throughout (see note below)
        dut->clk7n_en = 0;  // unused anywhere in paula_floppy.v's body
        dut->reset = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->enable = 0;
        dut->reg_address_in = 0xFF;  // an idle register-select value
        dut->data_in = 0;

        dut->_step = 1;   // idle high; active low
        dut->direc = 0;
        dut->_sel = 0xF;  // all four drives deselected; active low
        dut->side = 0;
        dut->_motor = 1;  // motor off; active low
        dut->wordsync = 0;

        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;

        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;

        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }

    ~Trial() {
        dut->final();
        delete dut;
    }

    Trial(const Trial&) = delete;
    Trial& operator=(const Trial&) = delete;

    // One clk7_en-enabled clock edge. clk7_en is held permanently high
    // (clk7n_en is a port on this module but is never read anywhere in
    // paula_floppy.v's body) -- so one call here is exactly one
    // "clk7_en tick" in the sense the claims file measures ticks in, with no
    // 4-phase division to reproduce.
    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    void ticks(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    // Select drive 0 (active low: one bit per drive, 4'hE = ~0001). Settles
    // the combinational outputs (`_selx`, `_track0`) immediately, so a
    // caller reading `track0()` right after does not see a stale value from
    // before the input change.
    void select_drive0() {
        dut->_sel = 0xE;
        dut->eval();
    }
    void deselect_all() {
        dut->_sel = 0xF;
        dut->eval();
    }

    // Idle long enough for step_ena_cnt (paula_floppy.v:594-603, a 9-bit
    // saturating counter with no reset term of its own either) to saturate,
    // i.e. for step_ena to read 1, so the NEXT accepted-edge attempt is not
    // itself rejected by the spacing floor under test elsewhere in this
    // file. 512 ticks covers the counter's full 9-bit range with margin.
    void saturate_step_ena() { ticks(512); }

    // Drives one _step pulse: low for `low_ticks` ticks (>=1, so a rising
    // edge exists to detect), then high. `direc` is set once, before the
    // low phase, and held through the edge -- ample setup, used by every
    // assertion except RATIO-B's explicit zero-setup probe.
    void do_step(bool direc_val, int low_ticks = 2) {
        dut->direc = direc_val ? 1 : 0;
        dut->_step = 0;
        for (int i = 0; i < low_ticks; ++i) tick();
        dut->_step = 1;
        tick();  // the rising edge is processed on this tick
        tick();  // settle
    }

    bool track0() {
        dut->eval();  // settle combinational outputs against current inputs
        return dut->_track0 == 0;  // active low
    }

    Vpaula_floppy* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        if (condition) {
            std::cout << "PASS: " << label << "\n";
            ++passed;
        } else {
            std::cout << "FAIL: " << label << "\n";
            ++failed;
        }
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

    int passed = 0;
    int failed = 0;
};

// ---------------------------------------------------------------------
// RATIO-A / RATIO-B measurement helpers
// ---------------------------------------------------------------------

// Whether a step separated from the previous accepted edge by exactly `gap`
// idle ticks is itself accepted, measured at burst "position" 1 (the first
// inter-edge gap after the rig starts) or position 2 (the second inter-edge
// gap in the same burst, immediately following a first gap that is known-
// accepted because it is generous). `_track0` is the only external read-back
// of head position (a single bit: "at track 0 or not"), so every probe here
// is arranged to cross the track-0 boundary -- that is the only way a 1-bit
// signal can report accept/reject of a specific edge.
bool gap_accepted(int position, int gap) {
    Trial t;
    t.select_drive0();
    t.saturate_step_ena();

    // Edge 1: direc=0, track 0 -> 1 (leaves track 0). Trivially accepted:
    // step_ena has just been saturated with nothing to reset it since.
    t.do_step(/*direc_val=*/false);

    if (position == 1) {
        // Probe: direc=1 would return 1 -> 0 (back to track 0) iff accepted.
        t.ticks(gap);
        t.do_step(/*direc_val=*/true);
        return t.track0();  // asserted (0) => the probed edge was accepted
    }

    // position == 2: fix the first gap generously (always accepted), then
    // probe the SECOND gap the same way, crossing track 0 a second time.
    t.ticks(600);
    t.do_step(/*direc_val=*/true);  // track 1 -> 0, fixed-generous gap
    // Sanity: this fixed gap must itself have been accepted, or the probe
    // below is meaningless. Not asserted here (return would be wrong either
    // way); checked by the caller instead.
    if (!t.track0()) return false;

    t.ticks(gap);
    t.do_step(/*direc_val=*/false);  // track 0 -> 1 iff the probed edge lands
    return !t.track0();              // deasserted (1) => probed edge accepted
}

// Binary search over [1, hi] for the smallest gap that is accepted. Assumes
// (and does not itself re-verify per call) that acceptance is monotonic in
// gap -- the counter this measures is a simple saturating up-counter with a
// single threshold, so once a gap is long enough every longer gap is too.
int min_accepted_gap(int position, int hi = 600) {
    if (!gap_accepted(position, hi)) return -1;  // never accepted at all
    int lo = 1;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (gap_accepted(position, mid)) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }
    return lo;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    std::cout << "Starting ch8-drive-step-timing...\n";

    // -----------------------------------------------------------------
    // A8 FIXTURE-LIVE -- the rig must be shown to deliver a distinct word
    // before anything above it is trusted. Two distinct stimulus sequences
    // (idle vs. one outward step) must produce two distinct, correct
    // _track0 read-backs.
    // -----------------------------------------------------------------
    {
        Trial t;
        t.select_drive0();
        t.saturate_step_ena();
        const bool value_a = t.track0();  // expect asserted: fresh dsktrack==0
        t.do_step(/*direc_val=*/false);   // outward step
        const bool value_b = t.track0();  // expect deasserted: left track 0
        checks.expect(value_a == true && value_b == false && value_a != value_b,
                      "FIXTURE-LIVE: the rig delivers two distinct, correct "
                      "_track0 read-backs (track0 before an outward step, "
                      "not-track0 after) -- proves the stimulus-to-observable "
                      "path is live before any claim below is graded");
    }

    // -----------------------------------------------------------------
    // A7 DIREC-PROTOCOL -- control case. Same rig and mechanism as the RED
    // ratio assertions below, grading a proposition the RTL does implement.
    // p.245: "Zero implies seek towards the center spindle. Track zero is
    // at the outside of the disk." Composed: direc=0 increases the track
    // count (leaves track 0); direc=1 decreases it (returns to track 0).
    // -----------------------------------------------------------------
    {
        Trial t;
        t.select_drive0();
        t.saturate_step_ena();
        const bool at_track0_initially = t.track0();
        t.do_step(/*direc_val=*/false);  // direc=0: expect track 0 -> 1
        const bool left_track0_on_direc0 = !t.track0();
        // A step edge resets step_ena_cnt regardless of whether it was
        // accepted, so the spacing floor must be earned again before the
        // second edge can be judged on `direc` alone.
        t.saturate_step_ena();
        t.do_step(/*direc_val=*/true);  // direc=1: expect track 1 -> 0
        const bool returned_on_direc1 = t.track0();
        checks.expect(
            at_track0_initially && left_track0_on_direc0 && returned_on_direc1,
            "DIREC-PROTOCOL: direc=0 increases the track count (leaves "
            "track 0), direc=1 decreases it (returns to track 0) "
            "[claims: opus:8-cia.c46, sol:8-cia.c40]");
    }

    // -----------------------------------------------------------------
    // A5 TRACK00-POLARITY -- p.242/243: TRACK 00 is depicted active-low,
    // asserted while the head is at track zero.
    // -----------------------------------------------------------------
    {
        Trial t;
        t.select_drive0();
        t.saturate_step_ena();
        const bool asserted_at_track0 = t.track0();
        t.do_step(/*direc_val=*/false);  // leave track 0
        const bool deasserted_off_track0 = !t.track0();
        checks.expect(
            asserted_at_track0 && deasserted_off_track0,
            "TRACK00-POLARITY: _track0 reads asserted (low) at track 0 and "
            "deasserted (high) once stepped away from it "
            "[claims: sol:8-fig8-8-text.c4, sol:8-fig8-8.c27]");
    }

    // -----------------------------------------------------------------
    // A6 STEP-POLARITY -- p.243: STEP is depicted as an active-low pulse.
    // The actionable event is the RISING edge (the pulse's end), not the
    // low level by itself.
    // -----------------------------------------------------------------
    {
        Trial t;
        t.select_drive0();
        t.saturate_step_ena();
        t.dut->direc = 0;
        t.dut->_step = 0;
        for (int i = 0; i < 4; ++i) t.tick();  // held low; no rising edge yet
        const bool unchanged_while_low = t.track0();
        t.dut->_step = 1;  // rising edge now
        t.tick();
        t.tick();
        const bool changed_on_rising_edge = !t.track0();
        checks.expect(
            unchanged_while_low && changed_on_rising_edge,
            "STEP-POLARITY: holding _step low produces no motion; the "
            "track only advances on the rising edge that ends the "
            "active-low pulse [claims: sol:8-fig8-8.c26]");
    }

    // -----------------------------------------------------------------
    // A4 PULSE-WIDTH-FLOOR -- Figure 8-7 (write timing) and the read-timing
    // diagram both print the STEP pulse width as "1 us min": a chip
    // requirement with a floor sense. Already measured and true at this
    // fixture: the module's own minimum accepted low width is 1 tick.
    // -----------------------------------------------------------------
    {
        Trial t;
        t.select_drive0();
        t.saturate_step_ena();
        t.do_step(/*direc_val=*/false, /*low_ticks=*/1);
        checks.expect(
            !t.track0(),
            "PULSE-WIDTH-FLOOR: a _step pulse held low for exactly 1 tick "
            "is accepted -- the module's minimum accepted low width is "
            "satisfiable at 1 tick, matching the printed floor sense "
            "[claims: sol:8-fig8-7.c3, sol:8-fig8-9-text.c1]");
    }

    // -----------------------------------------------------------------
    // A3 SEL-GATING -- p.242/243: DRIVE SELECT is depicted active-low and
    // precedes STEP; a step is only meaningful while the target drive is
    // selected. paula_floppy.v:673 gates the whole increment/decrement
    // block on `!_selx`.
    // -----------------------------------------------------------------
    {
        Trial t;
        t.deselect_all();
        t.saturate_step_ena();
        t.do_step(/*direc_val=*/false);  // attempted while deselected

        // paula_floppy.v:598-599: the step_ena_cnt reset on a rising edge is
        // NOT itself gated by _selx, so the deselected attempt above still
        // consumed the floor and it must be earned again before the second,
        // selected attempt can be judged on selection alone.
        t.saturate_step_ena();

        // paula_floppy.v:666: _track0 = &(_selx | _dsktrack0) reads
        // deasserted whenever the drive is deselected, REGARDLESS of head
        // position -- a deselected drive does not drive its status line.
        // Read the observable only after selecting, so it reflects dsktrack
        // rather than the deselected mask.
        t.select_drive0();
        const bool rejected_while_deselected = t.track0();  // still at 0
        t.do_step(/*direc_val=*/false);  // now selected
        const bool accepted_once_selected = !t.track0();

        checks.expect(
            rejected_while_deselected && accepted_once_selected,
            "SEL-GATING: a step is ignored while all drives are deselected "
            "and accepted once the drive is selected -- DRIVE SELECT "
            "(active low) gates STEP acceptance "
            "[claims: opus:8-time.c21, opus:8-fig8-8.c10, sol:8-fig8-8.c7, "
            "sol:8-fig8-8.c6]");
    }

    // -----------------------------------------------------------------
    // A2 RATIO-B -- p.243 figure legend: "t: 1 us min." is assigned to
    // every interval marked only `t` (the DIRECTION SELECT row's several
    // arrows, the DRIVE-SELECT-to-first-STEP arrow, and -- read across into
    // the paired read-timing frame -- the STEP pulse-width arrow): a single
    // shared floor. Already measured and true at this fixture: `direc`
    // setup before an accepted edge = 0 ticks; `_step` minimum low width =
    // 1 tick. Not equal.
    // -----------------------------------------------------------------
    {
        // direc setup = 0 ticks: change direc in the SAME tick as the
        // rising edge (no prior setup at all) and confirm the module still
        // reads the NEW value, not a stale one held during the low phase.
        Trial t;
        t.select_drive0();
        t.saturate_step_ena();
        t.dut->direc = 1;   // "wrong" value, held during the low phase
        t.dut->_step = 0;
        t.tick();
        t.tick();
        t.dut->direc = 0;   // changed with zero ticks of prior setup
        t.dut->_step = 1;   // rising edge, same tick
        t.tick();
        t.tick();
        const bool zero_setup_direc_took_effect = !t.track0();  // acted as direc=0 (increment)

        // step width floor = 1 tick: a pulse with NO low phase at all (no
        // low-to-high transition is ever produced) is never accepted, in
        // the same trial, from the same track-0 starting point re-measured
        // fresh.
        Trial t2;
        t2.select_drive0();
        t2.saturate_step_ena();
        t2.dut->direc = 0;
        // _step is never lowered: no edge exists to detect.
        for (int i = 0; i < 8; ++i) t2.tick();
        const bool zero_width_never_accepted = t2.track0();  // stayed at track 0

        const int direc_setup_ticks = 0;
        const int step_width_floor_ticks = 1;
        checks.note("direc setup = " + std::to_string(direc_setup_ticks) +
                    " tick(s); _step minimum low width = " +
                    std::to_string(step_width_floor_ticks) + " tick(s)");
        checks.expect(
            zero_setup_direc_took_effect && zero_width_never_accepted &&
                (direc_setup_ticks == step_width_floor_ticks),
            "RATIO-B: every `t`-labelled interval in the figure (DIRECTION "
            "SELECT row, DRIVE-SELECT-to-STEP, and the read-frame STEP "
            "pulse width) shares one printed floor -- measured: direc "
            "setup and the STEP pulse-width floor are NOT the same number "
            "of ticks "
            "[claims: sol:8-fig8-8.c1, opus:8-fig8-8.c8, opus:8-fig8-8.c3, "
            "sol:8-fig8-8.c17, sol:8-fig8-8.c29]");
    }

    // -----------------------------------------------------------------
    // A1 RATIO-A -- the package's headline (finding candidate C3). Figure
    // 8-8's access-timing diagram prints two different step-to-step minima
    // in the same frame: 18 ms (twice) and 3 ms (once), a ratio of 6:1.
    // -----------------------------------------------------------------
    {
        const int t_wide = min_accepted_gap(1);
        const int t_close = min_accepted_gap(2);
        checks.note("minimum accepted step-to-step gap, burst position 1 "
                    "(\"18 ms\" position) = " + std::to_string(t_wide) +
                    " tick(s); position 2 (\"3 ms\" position) = " +
                    std::to_string(t_close) + " tick(s); printed ratio 6:1, "
                    "measured ratio " + std::to_string(t_wide) + ":" +
                    std::to_string(t_close));
        checks.expect(
            t_wide > 0 && t_close > 0 && t_wide == 6 * t_close,
            "RATIO-A: the two step-to-step minima printed in the same "
            "frame (18 ms, 18 ms, 3 ms) are in a 6:1 ratio -- measured "
            "against paula_floppy's single step_ena_cnt threshold "
            "(paula_floppy.v:594-603), which gates every accepted edge "
            "alike regardless of position in a burst "
            "[claims: opus:8-time.c15, opus:8-time.c16, opus:8-time.c17, "
            "opus:8-fig8-8.c4, opus:8-fig8-8.c5, opus:8-fig8-8.c6, "
            "sol:8-fig8-8-text.c1, sol:8-fig8-8-text.c2, sol:8-fig8-8.c2, "
            "sol:8-fig8-8.c3, sol:8-fig8-8.c4, sol:8-fig8-8.c19, "
            "sol:8-fig8-8.c21, sol:8-fig8-8.c23]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
