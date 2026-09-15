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
//   18ms-reversal : 3ms-same-direction pair, p.245:32 and p.245:37-39) to 1
//   (what the RTL actually produces for a genuine reversal-gap measurement
//   against a genuine same-direction-gap measurement) -> PASS, confirming
//   the comparator is sensitive and the baseline red is a genuine
//   measurement, not a broken comparator. Reverted.
// RATIO-B carries no MUTATION record: it is declined via checks.record(),
//   not graded through expect(), so there is no assertion label to mutate.
//   See the record() call in main() for the reason.
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
#include <functional>
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
// RATIO-A measurement helpers
//
// p.245:37-39 ("When reversing directions, a minimum of 18 milliseconds
// delay is required from the last step pulse") and p.245:32 ("the next
// track within 3 milliseconds") name two DIFFERENT cases: a direction
// REVERSAL and a SAME-DIRECTION continuation. Figure 8-8 prints them as two
// arrows in the same frame (18ms, 18ms, 3ms), a 6:1 ratio. A sound
// measurement needs one genuine reversal gap and one genuine same-direction
// gap, both read through the same observable.
//
// `_track0` is the only external read-back of head position, and it is a
// single bit that is only meaningful "at track 0 or not"
// (paula_floppy.v:653: `_track0 = &(_selx | _dsktrack0)`), so any probe has
// to be arranged so its accept/reject outcome shows up as a track-0
// crossing. A reversal probe can be read directly this way (it crosses back
// toward track 0). A same-direction probe cannot -- it moves the head
// FURTHER from track 0 -- so it is folded back onto the boundary with one
// extra, generously-spaced reversal step after it.
// ---------------------------------------------------------------------

// REVERSAL case (p.245:37-39, "18 ms min" in Figure 8-8). Edge 1 (direc=
// false, guaranteed accepted -- step_ena was just saturated with nothing to
// reset it) leaves track 0. The probe, `gap` ticks later, is direc=true: a
// genuine reversal. `_track0` is read directly: asserted (0) means the
// probed edge decremented the track back to 0, i.e. was accepted.
bool gap_accepted_reversal(int gap) {
    Trial t;
    t.select_drive0();
    t.saturate_step_ena();
    t.do_step(/*direc_val=*/false);   // track 0 -> 1, trivially accepted
    t.ticks(gap);
    t.do_step(/*direc_val=*/true);    // PROBE: reversal (direc flips)
    return t.track0();                // asserted (0) => probed edge accepted
}

// SAME-DIRECTION case (p.245:32, "3 ms min" -- "the next track within 3
// milliseconds"). Edge 1 (direc=false, guaranteed accepted) leaves track 0.
// The probe, `gap` ticks later, is ALSO direc=false: the same direction as
// edge 1, exactly the case RATIO-A's original form never drove. If accepted
// it moves track 1 -> 2; if rejected, track stays at 1 -- a difference
// `_track0` cannot see directly, since both land off track 0.
//
// Folded back with one more step, direc=true, spaced 600 ticks after the
// probe (well past the ~253-tick floor measured elsewhere in this file, so
// it is accepted regardless of the probe's outcome or of which earlier edge
// last reset step_ena_cnt -- paula_floppy.v:581-590's reset-on-rising-edge
// is not gated by selection or by whether the earlier edge actually moved
// the track). This third edge is itself a reversal, decrementing; that is
// never blocked by the track-0 floor here because the head is at track 1 or
// 2, never at 0, when it fires (paula_floppy.v:658-667: the decrement guard
// is `_dsktrack0 && direc`, and `_dsktrack0` only deasserts AT track 0).
//
//   edge 1 (direc=false, guaranteed accepted):              track 0 -> 1
//   PROBE  (direc=false, SAME direction, `gap` ticks later): 1 -> 2 iff
//                                                             accepted
//   edge 3 (direc=true, 600-tick gap, guaranteed accepted):  2 -> 1 (probe
//                                                             was accepted)
//                                                          or 1 -> 0 (probe
//                                                             was rejected)
bool gap_accepted_same_direction(int gap) {
    Trial t;
    t.select_drive0();
    t.saturate_step_ena();
    t.do_step(/*direc_val=*/false);   // track 0 -> 1, trivially accepted
    if (t.track0()) return false;     // edge 1 did not leave track 0: a rig
                                       // fault, not a measurement of the
                                       // probe -- reported as "rejected"
                                       // would be wrong, but this can only
                                       // be reached if do_step itself is
                                       // broken, which FIXTURE-LIVE guards.
    t.ticks(gap);
    t.do_step(/*direc_val=*/false);   // PROBE: SAME direction as edge 1
    t.ticks(600);
    t.do_step(/*direc_val=*/true);    // fold-back reversal, generously spaced
    return !t.track0();               // deasserted (1) => probed edge accepted
}

// Binary search over [1, hi] for the smallest gap `fn` accepts. Assumes (and
// does not itself re-verify per call) that acceptance is monotonic in gap --
// the counter this measures is a simple saturating up-counter with a single
// threshold, so once a gap is long enough every longer gap is too.
int min_accepted_gap(const std::function<bool(int)>& fn, int hi = 600) {
    if (!fn(hi)) return -1;  // never accepted at all
    int lo = 1;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (fn(mid)) {
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
    // RATIO-B -- DECLINED, not graded. p.243 figure legend: "t: 1 us min."
    // is assigned to every interval marked only `t` (the DIRECTION SELECT
    // row's several arrows, the DRIVE-SELECT-to-first-STEP arrow, and --
    // read across into the paired read-timing frame -- the STEP
    // pulse-width arrow). The withdrawn form of this assertion compared a
    // declared "0 tick" DIRECTION setup against a declared "1 tick" STEP
    // pulse floor and called the inequality a finding. Both numbers below
    // are still measured and printed, but neither is a measurement of a
    // physical or RTL-internal interval of the kind the page's "1 us min."
    // names, for reasons explained in the record() below -- so no expect()
    // grades them.
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
                    " tick(s) (zero_setup_direc_took_effect=" +
                    (zero_setup_direc_took_effect ? std::string("true") : std::string("false")) +
                    "); _step minimum low width = " +
                    std::to_string(step_width_floor_ticks) +
                    " tick(s) (zero_width_never_accepted=" +
                    (zero_width_never_accepted ? std::string("true") : std::string("false")) +
                    ") -- both figures are testbench/simulation bookkeeping, "
                    "not measured physical intervals; see the RATIO-B "
                    "record() below");
        checks.record(
            "RATIO-B declined: the two numbers above are not two measured "
            "floors of the same kind, and their comparison does not become "
            "a valid dimensionless equality just because both sides are "
            "integers. This is a no-timing Verilator run: eval() has no "
            "setup/hold model at all, so changing direc immediately before "
            "the same rising-edge eval() and having the module read the "
            "new value shows only that this simulation enforces NO direc "
            "setup requirement whatsoever -- an absence, not a measured "
            "zero-tick floor. Never lowering _step (the width-floor probe) "
            "produces no rising edge for the module to detect at all "
            "(paula_floppy.v:574-579's _step_del compare needs an actual "
            "low-then-high transition); that proves only that no edge "
            "occurred, not that 1 tick is a measured minimum PULSE WIDTH -- "
            "1 tick is simply the coarsest interval a discrete clk7_en-tick "
            "edge detector can register, true of any edge-triggered input "
            "regardless of what physical pulse width it actually requires. "
            "The page's own value for this family, \"t: 1 us min.\", is an "
            "ABSOLUTE interval, and paula_floppy.v ties no constant or "
            "counter to a real-time unit anywhere reachable from this "
            "fixture (step_ena_cnt's ~253-tick floor, measured under "
            "RATIO-A, is the only calibrated-looking number here and nothing "
            "connects it to 1 us either) -- so there is no genuine "
            "dimensionless relation of the RATIO-A kind hiding in this pair: "
            "unlike the 18ms:3ms ratio, which strips to a comparison of two "
            "things the module actually implements (two spacing floors on "
            "the same counter), 'direc setup' and 'STEP pulse width' are not "
            "the same kind of quantity in this design at all -- one is the "
            "absence of a modelled constraint, the other is a detector's "
            "time-step granularity. Declined rather than graded "
            "[claims: sol:8-fig8-8.c1, opus:8-fig8-8.c8, opus:8-fig8-8.c3, "
            "sol:8-fig8-8.c17, sol:8-fig8-8.c29]");
    }

    // -----------------------------------------------------------------
    // RATIO-A -- the package's headline (finding candidate C3). Figure 8-8's
    // access-timing diagram prints two different step-to-step minima in the
    // same frame: 18 ms (twice, on direction REVERSAL, p.245:37-39) and
    // 3 ms (once, on a SAME-DIRECTION continuation, p.245:32), a ratio of
    // 6:1. The withdrawn form of this assertion measured a direction
    // reversal twice (position 1: direc false-then-true; position 2:
    // direc false-then-true-then-probe-false -- the probe in BOTH cases
    // follows a direction change) and called the two identical 253-tick
    // results "the 18 ms position" and "the 3 ms position" without ever
    // driving the same-direction case the page names. This form drives
    // both cases for real: gap_accepted_reversal probes a genuine
    // direction reversal directly against _track0; gap_accepted_same_
    // direction probes a genuine same-direction continuation, folded back
    // onto the track-0 boundary so _track0 can still report it (see the
    // helper's comment above for why the fold-back is needed and why it
    // does not itself bias the probed measurement).
    // -----------------------------------------------------------------
    {
        const int t_reversal = min_accepted_gap(gap_accepted_reversal);
        const int t_same_direction = min_accepted_gap(gap_accepted_same_direction);
        checks.note("minimum accepted step-to-step gap, direction REVERSAL "
                    "(p.245:37-39, \"18 ms\" in Figure 8-8) = " +
                    std::to_string(t_reversal) +
                    " tick(s); SAME-DIRECTION continuation (p.245:32, "
                    "\"3 ms\" in Figure 8-8) = " +
                    std::to_string(t_same_direction) + " tick(s); printed "
                    "ratio 6:1, measured ratio " + std::to_string(t_reversal) +
                    ":" + std::to_string(t_same_direction));
        checks.expect(
            t_reversal > 0 && t_same_direction > 0 &&
                t_reversal == 6 * t_same_direction,
            "RATIO-A: the printed 18ms-reversal and 3ms-same-direction "
            "step-to-step minima (p.245:32, p.245:37-39; Figure 8-8) are "
            "in a 6:1 ratio -- measured against paula_floppy's single "
            "step_ena_cnt spacing counter (paula_floppy.v:581-590), which "
            "resets on every accepted _step rising edge regardless of "
            "selection (paula_floppy.v:658-667 gates the TRACK update on "
            "selection and direction, but the counter reset at 581-590 "
            "carries no such gate) and so gates a reversal and a "
            "same-direction edge alike "
            "[claims: opus:8-time.c15, opus:8-time.c16, opus:8-time.c17, "
            "opus:8-fig8-8.c4, opus:8-fig8-8.c5, opus:8-fig8-8.c6, "
            "sol:8-fig8-8-text.c1, sol:8-fig8-8-text.c2, sol:8-fig8-8.c2, "
            "sol:8-fig8-8.c3, sol:8-fig8-8.c4, sol:8-fig8-8.c19, "
            "sol:8-fig8-8.c21, sol:8-fig8-8.c23]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
