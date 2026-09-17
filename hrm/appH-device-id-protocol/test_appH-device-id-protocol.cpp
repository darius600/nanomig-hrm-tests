// Test purpose: HRM conformance package for appendix H's Device I.D. protocol
//   -- pp.368,370-371, the nine-step-plus-31-repeats procedure that samples
//   RDY (pin 1, _ready) 32 times to assemble a device identifier -- graded
//   against paula_floppy (this suite's only model of a physical Amiga floppy
//   drive, and the same module hrm/appDE-disk-interface grades one page
//   earlier). 14 claims, from
//   work/2026-09-17_appHJ_packages.jsonl (classify.package ==
//   "appH-device-id-protocol").
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed
//   pp.368, 370-371. p.368's Summary Table is NOT displaced (this package's
//   own corpus-defects.md, "p.368 -- the table is clean"), so the RDY- entry
//   is quoted from p368.txt directly. p.370's nine numbered steps and p.371's
//   four prose paragraphs both survive the text layer (corpus-defects.md,
//   "p.370", "p.371 -- the I.D. table is unusable; the prose ... is not");
//   only p.371's two-column bit-pattern table is unusable and irrelevant here
//   (this package's 14 claims name none of its rows). Every quotation below
//   was independently confirmed against figures/p370.png and figures/p371.png
//   before this file was written (both reproduced in full: the nine steps on
//   p370, and the "Repeat steps 7-9" / "Steps 1-4 ... initializes" / "Steps 7,
//   8 and 9 form a loop" / "This loop is performed 32 times" paragraphs on
//   p371), not trusted from the claims file's own restatements.
//
// THIS IS THE PACKAGE APPENDICES H AND J WERE READ FOR. Appendix E's lot
//   declined 21 paula_floppy claims (sol:E-disk-id.c1..c21) at CLASSIFICATION
//   because "the external-device identification shift register and its
//   serial output on RDY* -- ... no shift register or bit counter exists
//   anywhere under src/minimig-aga". That is correct about the RTL (confirmed
//   again below, independently) and is a statement about the MECHANISM.
//   Classification asks a different question: can a fixture drive this and
//   observe it at a boundary. It can -- every step of the printed procedure
//   is expressed in ports paula_floppy has: steps 1 and 4 drive `_motor`;
//   steps 2, 3, 5, 6, 7 and 9 drive `_sel`; step 8 reads `_ready`. The missing
//   mechanism is what the measurement REPORTS -- a red -- not a reason no
//   assertion can be built. hrm/appDE-disk-interface took exactly that route
//   for the same port one page earlier: its P-READY-MOTOR-INVARIANT is graded
//   RED at baseline rather than declined, on the same _ready equation. This
//   package follows the same precedent for the deeper 32-bit protocol.
//
// NOT A RE-GRADE OF hrm/appDE-disk-interface's P-READY-MOTOR-INVARIANT. That
//   assertion's label reads "with selection held fixed, _ready must read
//   differently for motor-on than for motor-off to realise either printed
//   branch" -- ONE `_motor` toggle, ONE `!=` comparison, selection fixed.
//   Every claim graded below states a comparator over a SEQUENCE OF 32 SELECT
//   PULSES (ID-CHANNEL-EXISTS) or over TWO FULL SUCH SEQUENCES, with and
//   without the steps-1-4 motor cycle
//   (MOTOR-CYCLE-INIT-CHANGES-ID-STREAM) -- a different proposition on a
//   different axis. appDE-disk-interface's own header agrees in its own
//   words: "The deeper 32-bit serial identification PROTOCOL p.335 describes
//   (shift register, bit counter, motor-transition trigger) is carried by a
//   sibling package's claims ... NOT by this package's 26". This package is
//   that sibling. Neither assertion below declines against
//   P-READY-MOTOR-INVARIANT, and neither names its label or uids.
//
// RTL READ, IN ORDER, BEFORE ANY EXPECT/RECORD BELOW WAS WRITTEN:
//   1. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines),
//      independently, not trusted from hrm/appDE-disk-interface's own header.
//   2. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file, confirming
//      it instantiates nothing else (the only sub-instance in paula_floppy.v
//      is this fifo, `db1`), so the two-file build closes.
//   3. hrm/appDE-disk-interface/test_appDE-disk-interface.cpp -- full file,
//      the build brief's named model: its Rig (that file's lines 225-378)
//      instantiates Vpaula_floppy, drives `_sel` and `_motor`, reads
//      `_ready`, and carries a FIXTURE-LIVE check proving that path live.
//      This package's Rig below is its own (a fresh struct in this file, not
//      an #include of that one -- packages build independently), adapted
//      from that shape to this package's own procedure.
//
// MECHANISM, confirmed by reading paula_floppy.v myself (line numbers below
//   are this package's own verification):
//
//   Port list (paula_floppy.v:83-93): inputs _step, direc, _sel[3:0], side,
//   _motor; outputs _track0, _change, _ready, _wprot, index -- the same ports
//   hrm/appDE-disk-interface reads. `_ready` is pin 1 / RDY* (p.367 Summary
//   Table, p.334 in appendix E; this appendix's own p.368 RDY- entry: "When
//   the motor is off this is used as a ID data line").
//
//   _ready (paula_floppy.v:688-694), quoted in full:
//     // drive _ready signal control
//     // Amiga DD drive activates _ready whenever _sel is active and motor is off
//     // or whenever _sel is active, motor is on and there is a disk inserted (not implemented - _ready is active when _sel is active)
//     assign _ready   = (_sel[3] | ~(drives[1] & drives[0]))
//             & (_sel[2] | ~drives[1])
//             & (_sel[1] | ~(drives[1] | drives[0]))
//             & (_sel[0]);
//   `drives` (paula_floppy.v:554-558) latches from `floppy_drives` only at
//   reset, and this package's Rig fixes `floppy_drives=0` throughout (one
//   drive configured), which reduces every term but the last to a constant 1
//   -- MEASURED directly, not merely derived: with `floppy_drives=0`,
//   `_ready` collapses to exactly `_sel[0]`, confirmed below in FIXTURE-LIVE
//   and used deliberately as this package's mutation vector (selecting a
//   DIFFERENT physical line, `_sel[1]`, reads back a genuinely different,
//   constant `_ready`, since with this config that line selects no configured
//   drive). `_motor` does not appear anywhere in this expression, and does
//   not appear anywhere in `motor_on`'s cone either (paula_floppy.v:624-659,
//   the four motor_on flip-flops, feed nothing this module reads back on
//   `_ready`). The module's own comment, quoted above, says so in its own
//   words: "not implemented - _ready is active when _sel is active". There is
//   no shift register and no bit counter anywhere in this file: `_ready` is
//   pure combinational logic of `_sel[3:0]` and the reset-latched `drives`
//   alone, so 32 samples taken with the SAME `_sel` pattern on every
//   iteration -- exactly what the printed procedure drives, one select line
//   pulsed low/high 32 times -- read back the SAME value 32 times. That is
//   MEASURED below, not assumed: ID-CHANNEL-EXISTS and
//   MOTOR-CYCLE-INIT-CHANGES-ID-STREAM are both RED at baseline, and both are
//   the real, previously-unmeasured finding this appendix's reading exists to
//   report.
//
// A CORRECTION TO APPENDIX E'S OWN DECLINE EVIDENCE. Its absent_signal text
//   said a grep for `ident|drive_id|id_reg|shift_reg` "returns nothing
//   anywhere under nanomig/src". Re-run myself, 17.09.2026, against the same
//   pinned submodule (d6f7df957fad655bc07f6a381e5094a9edff0be0):
//     grep -rniE 'ident|drive_id|id_reg|shift_reg' nanomig/src/minimig-aga
//       -> 4 matches, all "identically"/"identical"/"INDIRECT" (an English
//          word inside a comment or licence header; denise_bitplanes.v:251,
//          denise_bitplane_shifter.v:26, denise_bitplanes.v:340, ide.v:19)
//     grep -rniE 'ident|drive_id|id_reg|shift_reg' nanomig/src
//       -> 22 matches tree-wide, same character: no register, no bit counter,
//          no serial channel among any of them (the two additional hits under
//          src/misc, sysctrl.v:184,210, are "identify"/"identifies" in a
//          comment).
//   The CONCLUSION holds -- no identification shift register or bit counter
//   exists anywhere in the pinned tree, confirmed independently above by
//   reading _ready's own equation rather than by this grep -- but the
//   MEASUREMENT AS WRITTEN in the appendix E decline does not: it returns 4
//   matches under src/minimig-aga and 22 tree-wide, not nothing. Recorded
//   here, per the build brief, so the absence is stated correctly rather than
//   repeated.
//
// NOT GRADABLE ANYWHERE IN THIS PACKAGE (recorded via record(), never
//   graded): none of this package's 14 claims state a defect this file needs
//   to decline against. The trailing-hyphen inconsistency corpus-defects.md
//   notes (RDY- in the Summary Table against RDY in Device I.D. step 8;
//   SELxB- against SELxB in the same procedure) names the SAME signal both
//   ways and binds no claim differently either way -- sol:H-076/opus:AHJ-103
//   and sol:H-085/opus:AHJ-111's own restatements already note the missing
//   hyphen; it is not a contradiction requiring "grade neither" (NM-HRM-062's
//   precedent is for two DIFFERENT printed values under one label, not a
//   typographic variant of one name). p.371's own self-contradictory
//   "Reserved ($x000 x000)" / "Reserved ($xFFF xFFF)" row pair and the
//   WRPRO-/WPRO- naming split are both real (corpus-defects.md) but neither
//   is named by any of this package's 14 claims -- none references a specific
//   32-bit ID pattern from the table or the pin-14 write-protect signal -- so
//   neither is recorded here as a decline; both are out of scope for this
//   file's own claims.
//
// MUTATION METHODOLOGY. ID-CHANNEL-EXISTS and MOTOR-CYCLE-INIT-CHANGES-ID-
//   STREAM are RED at baseline -- the real, previously-unmeasured finding
//   this appendix's reading exists to report. Per this suite's rule against
//   mutating an already-failing check with a no-op, each substitutes a
//   DIFFERENT genuine, independently-live stimulus change through the
//   IDENTICAL comparator and the SAME label, flipping baseline RED to
//   mutated GREEN -- proving the comparator itself is sensitive to a real
//   difference, not merely broken in a way that always reads red.
//   FIXTURE-LIVE is GREEN at baseline (it is the liveness control itself),
//   so its mutation runs the other direction: the stimulus collapses its two
//   known-distinct `_sel` states to one, through the identical != comparator
//   and the same label, flipping baseline GREEN to mutated RED -- an oracle
//   mutation here would show only that the comparator can be made false, not
//   that the stimulus path is live, which is the one thing this control
//   exists to establish. `mutating(id)` is read once per block into a local
//   `bool mut` before any `expect()` call, never inside a condition or a
//   label (see tools/check_expect_label_shape.py).
//
// MUTATION RUN LOG (measured, WSL/Verilator 5.052, 17.09.2026). Baseline
//   (MUTATE unset): 3 checks.expect() calls, 1 passed / 2 failed -- FIXTURE-
//   LIVE PASS, ID-CHANNEL-EXISTS FAIL, MOTOR-CYCLE-INIT-CHANGES-ID-STREAM
//   FAIL (the two real findings). MUTATE=ID-CHANNEL-EXISTS: 2 passed / 1
//   failed -- diffed against baseline, ONLY ID-CHANNEL-EXISTS moves,
//   FAIL->PASS (samples become 10101010101010101010101010101010, alternating
//   as the substituted stimulus predicts); FIXTURE-LIVE and MOTOR-CYCLE-INIT-
//   CHANGES-ID-STREAM are unchanged. MUTATE=MOTOR-CYCLE-INIT-CHANGES-ID-
//   STREAM: 2 passed / 1 failed -- diffed against baseline, ONLY that
//   assertion moves, FAIL->PASS (without-init sequence becomes constant 0
//   instead of constant 1, on the substituted drive-1 trial); the other two
//   are unchanged. MUTATE=FIXTURE-LIVE: 0 passed / 3 failed -- diffed against
//   baseline, ONLY FIXTURE-LIVE moves, PASS->FAIL (second reading becomes
//   ready=1, identical to the first, once the same _sel state is driven
//   twice instead of two distinct states); ID-CHANNEL-EXISTS and MOTOR-
//   CYCLE-INIT-CHANGES-ID-STREAM are unchanged. All three runs then reverted
//   (MUTATE unset) and reconfirmed back to 1 passed / 2 failed.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "Vpaula_floppy.h"
#include "verilated.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

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

// RIG HAZARD (same as hrm/appDE-disk-interface, hrm/ch8-drive-sense-lines and
// hrm/ch8-drive-select-motor-led): dsktrack and step_ena_cnt have no reset
// term, and dmaen/dsklen state persists once armed. Not touched by this
// package's procedure (no step, no DMA arm anywhere below), but every test
// block still constructs its own fresh Rig; none is reused across trials.
struct Rig {
    Vpaula_floppy* d;

    // floppy_drives is fixed at 0 (one drive configured) for every trial in
    // this file: with drives=0, _ready (paula_floppy.v:688-694) reduces
    // algebraically to exactly _sel[0] -- MEASURED directly in FIXTURE-LIVE
    // below, not merely derived -- which is what lets this package's
    // mutations substitute "select a different physical line" as a genuine,
    // independently-live stimulus difference (see file header).
    explicit Rig() {
        d = new Vpaula_floppy;
        d->clk = 0;
        d->clk7_en = 1;   // held high throughout, same idiom as
        d->clk7n_en = 0;  // hrm/appDE-disk-interface's Rig
        d->reset = 1;
        d->ntsc = 0;
        d->sof = 0;
        d->enable = 0;
        d->reg_address_in = 0xFF;  // an idle register-select value
        d->data_in = 0;

        d->_step = 1;   // idle high; active low; unused by this package
        d->direc = 0;
        d->_sel = 0xF;  // all four drives deselected; active low
        d->side = 0;
        d->_motor = 1;  // motor off; active low (step 4's resting state)

        d->wordsync = 0;
        d->floppy_drives = 0;
        d->floppy_wrprot = 0;

        d->sdc_image_mounted = 0;
        d->sdc_image_size = 0;
        d->sdc_busy = 0;
        d->sdc_done = 0;
        d->sdc_byte_in_strobe = 0;
        d->sdc_byte_addr = 0;
        d->sdc_byte_in_data = 0;

        d->eval();
        do_reset();
    }
    ~Rig() { d->final(); delete d; }

    Rig(const Rig&) = delete;
    Rig& operator=(const Rig&) = delete;

    void tick() {
        d->eval();
        d->clk = 1;
        d->eval();
        d->clk = 0;
        d->eval();
    }
    void ticks(int n) { for (int i = 0; i < n; i++) tick(); }

    void do_reset() {
        d->reset = 1; ticks(16);
        d->reset = 0; ticks(16);
    }

    // Step 2/5/7: "Drive SELxB- low: Activate drive select x."
    void select(int n) {
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        d->eval();
    }
    // Step 3/6/9: "Drive SELxB- high: Deactivate drive select x."
    void deselect_all() { d->_sel = 0xF; d->eval(); }

    // Step 1: "Drive MTRXD- low: Turn on the disk drive motor."
    void motor_on()  { d->_motor = 0; d->eval(); }
    // Step 4: "Drive MTRXD- high: Turn off disk drive motor."
    void motor_off() { d->_motor = 1; d->eval(); }

    // Step 8: "Read and save state of RDY." Active-low, same convention as
    // hrm/appDE-disk-interface's ready().
    bool ready() { d->eval(); return d->_ready == 0; }
};

// Steps 1-9 plus the printed 31 repeats (steps 7-9), 32 total iterations of
// the read loop, exactly as p.370-371 print them. `drive` selects which
// physical _sel line steps 2/5/7 pulse (always the SAME one across all 32
// iterations, per the printed procedure -- "activate drive select x" names
// one x throughout). `do_init` gates steps 1-4 (the motor on/off cycle);
// when false, the procedure starts at step 5, matching the header's use of
// this same function for MOTOR-CYCLE-INIT-CHANGES-ID-STREAM's two-run
// comparison. Returns the 32 saved RDY states, most-significant (first-read)
// bit at index 0, exactly as steps 7-9's loop and the "Repeat steps 7 through
// 9 ... The most significant bit is read first" paragraph describe.
std::vector<bool> run_device_id_procedure(Rig& r, int drive, bool do_init) {
    if (do_init) {
        r.motor_on();     r.tick();  // step 1
        r.select(drive);  r.tick();  // step 2
        r.deselect_all(); r.tick();  // step 3
        r.motor_off();    r.tick();  // step 4
    }
    r.select(drive);  r.tick();      // step 5
    r.deselect_all(); r.tick();      // step 6

    std::vector<bool> samples;
    samples.reserve(32);
    for (int i = 0; i < 32; i++) {
        r.select(drive);             // step 7
        r.tick();
        samples.push_back(r.ready());  // step 8
        r.deselect_all();            // step 9
        r.tick();
    }
    return samples;
}

bool all_identical(const std::vector<bool>& v) {
    for (size_t i = 1; i < v.size(); i++) if (v[i] != v[0]) return false;
    return true;
}

std::string bits_to_string(const std::vector<bool>& v) {
    std::string s;
    for (bool b : v) s += b ? '1' : '0';
    return s;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    std::cout << "Starting appH-device-id-protocol...\n";

    // ========================================================================
    // FIXTURE-LIVE -- no claim id, no manual page: the build brief's own
    // requirement that the fixture prove it can deliver a known, distinct
    // reading before any read-based assertion below is trusted. Measured on
    // `_ready` itself (this package's only observable), through `_sel` (this
    // package's only stimulus that reaches it, with floppy_drives=0): two
    // distinct, known `_sel` states must read back distinguishably.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("FIXTURE-LIVE");
        r.select(0);
        bool first_ready = r.ready();
        // MUTATION: drive the SAME _sel state again instead of the second,
        // distinct one (deselect_all()), so the two readings collapse to one
        // known state and cannot be told apart -- a genuine, independently-
        // live stimulus difference (select(0) vs deselect_all() are the two
        // states FIXTURE-LIVE itself exists to distinguish), through the
        // IDENTICAL != comparator and the SAME label.
        if (!mut) {
            r.deselect_all();
        } else {
            r.select(0);
        }
        bool second_ready = r.ready();
        checks.note("FIXTURE-LIVE: drive0 selected -> ready=" + std::to_string(first_ready) +
                    "; second reading (" + std::string(mut ? "drive0 selected again, mutated"
                                                             : "all deselected") + ") -> ready=" +
                    std::to_string(second_ready) +
                    " (floppy_drives=0, so _ready reduces to exactly _sel[0] -- "
                    "paula_floppy.v:688-694)");
        checks.expect(first_ready != second_ready,
            "FIXTURE-LIVE: two distinct, known _sel states, driven through this package's Rig, "
            "arrive unmodified and mutually distinguishable when read back through _ready -- the "
            "_sel -> _ready path this package's every claim depends on is live, not stuck, before "
            "any claim below is trusted");
    }

    // ========================================================================
    // ID-CHANNEL-EXISTS -- p.368 (Summary Table, RDY-: "When the motor is off
    // this is used as a ID data line"), p.370 ("This interface supports a
    // method of establishing the type of disk(s) attached", step 8: "Read and
    // save state of RDY"), p.371 ("Repeat steps 7 through 9, 31 more times
    // for a total of 32 iterations, in order to read 32 bits of data",
    // "Steps 7, 8 and 9 form a loop where (7) the SELxB signal is driven
    // active (low), (8) the serial input data is read on RDY (pin 1) and (9)
    // the SELxB signal is again driven high (inactive)", "This loop is
    // performed 32 times, once for each of the bits in the input stream that
    // comprise the device I.D."). Six distinct printed sentences, each read
    // twice (sol + opus) = 12 claims, all reducible to ONE testable boundary
    // fact: running the printed procedure once and sampling RDY at step 8 on
    // all 32 iterations must NOT yield a fixed function of `_sel` alone --
    // if it does, no data is being delivered and every one of these six
    // sentences (an "ID data line", a "method of establishing device type",
    // a step that "reads and saves" a changing state, "32 bits of data",
    // "serial input data", "the device I.D.") is unrealised. THE SHAPE OF THE
    // ASSERTION THE CLASSIFIER PROPOSED, built as specified: run the nine
    // steps plus the 31 repeats, collect the 32 RDY samples, require they are
    // not a fixed function of _sel alone.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("ID-CHANNEL-EXISTS");
        // MUTATION: the printed procedure pulses the SAME select line (drive
        // 0) on all 32 iterations. Substitute a genuine, independently-live
        // stimulus difference -- alternate between drive 0 and drive 1 each
        // iteration -- through the IDENTICAL all_identical() comparator and
        // the SAME label. With floppy_drives=0, drive 1 selects no configured
        // drive, so _ready reads back a genuinely different constant on
        // those iterations (MEASURED in FIXTURE-LIVE above: _ready ==
        // _sel[0] exactly), proving the comparator is sensitive to a real
        // difference and not merely broken in a way that always reads red.
        std::vector<bool> samples;
        samples.reserve(32);
        if (!mut) {
            samples = run_device_id_procedure(r, /*drive=*/0, /*do_init=*/true);
        } else {
            r.motor_on();     r.tick();
            r.select(0);      r.tick();
            r.deselect_all(); r.tick();
            r.motor_off();    r.tick();
            r.select(0);      r.tick();
            r.deselect_all(); r.tick();
            for (int i = 0; i < 32; i++) {
                int d_idx = (i % 2 == 0) ? 0 : 1;  // alternate select line
                r.select(d_idx);
                r.tick();
                samples.push_back(r.ready());
                r.deselect_all();
                r.tick();
            }
        }
        checks.note("ID-CHANNEL-EXISTS: 32 RDY samples over the printed procedure ("
                    + std::string(mut ? "mutated: alternating _sel[0]/_sel[1] each iteration"
                                       : "as printed: _sel[0] pulsed identically every iteration")
                    + ") = " + bits_to_string(samples));
        checks.expect(!all_identical(samples),
            "ID-CHANNEL-EXISTS (p.368 RDY-: 'When the motor is off this is used as a ID data "
            "line'; p.370: 'This interface supports a method of establishing the type of "
            "disk(s) attached', step 8: 'Read and save state of RDY'; p.371: 'Repeat steps 7 "
            "through 9, 31 more times for a total of 32 iterations, in order to read 32 bits of "
            "data', 'Steps 7, 8 and 9 form a loop where (7) the SELxB signal is driven active "
            "(low), (8) the serial input data is read on RDY (pin 1) and (9) the SELxB signal is "
            "again driven high (inactive)', 'This loop is performed 32 times, once for each of "
            "the bits in the input stream that comprise the device I.D.'): running the printed "
            "nine-step-plus-31-repeat procedure once and sampling RDY at step 8 on all 32 "
            "iterations must not yield a fixed function of _sel alone, or no identification data "
            "is being delivered -- a different proposition, on a different axis (a sequence of "
            "32 select pulses, not one _motor toggle with selection fixed), from "
            "hrm/appDE-disk-interface's P-READY-MOTOR-INVARIANT. MEASURED red: "
            "paula_floppy.v:688-694 makes _ready pure combinational logic of _sel[3:0] and the "
            "reset-latched drive count alone (module's own comment: 'not implemented - _ready is "
            "active when _sel is active'), no shift register or bit counter exists anywhere in "
            "the pinned tree (grep -rniE 'ident|drive_id|id_reg|shift_reg': 4 matches under "
            "src/minimig-aga, 22 tree-wide, every one the English word 'identical'/'identifies'/"
            "'INDIRECT'), so pulsing the same _sel line 32 times reads the same value 32 times: "
            "no device identifier is ever assembled. "
            "sol:H-038 opus:AHJ-062 sol:H-068 opus:AHJ-094 sol:H-076 opus:AHJ-103 "
            "sol:H-078 opus:AHJ-105 sol:H-085 opus:AHJ-111 sol:H-087 opus:AHJ-112");
    }

    // ========================================================================
    // MOTOR-CYCLE-INIT-CHANGES-ID-STREAM -- p.371 ("Steps 1 through 4 in the
    // algorithm above turn on and off the disk drive motor. This initializes
    // the serial shift register. After initialization, the SELxB signal is
    // driven (first active then) inactive as in steps 5 and 6."). A
    // DIFFERENT axis from ID-CHANNEL-EXISTS: not whether one run's 32 samples
    // vary, but whether PERFORMING steps 1-4 changes what the subsequent
    // steps 5-9 read back at all, i.e. whether the motor cycle actually
    // initialises anything. Two runs, one comparator: the 32-sample sequence
    // WITH the steps-1-4 motor cycle must differ from the SAME steps 5-9
    // procedure run WITHOUT it.
    // ========================================================================
    {
        Rig r_with;
        Rig r_without;
        bool mut = mutating("MOTOR-CYCLE-INIT-CHANGES-ID-STREAM");
        std::vector<bool> with_init = run_device_id_procedure(r_with, /*drive=*/0, /*do_init=*/true);
        std::vector<bool> without_init;
        if (!mut) {
            without_init = run_device_id_procedure(r_without, /*drive=*/0, /*do_init=*/false);
        } else {
            // MUTATION: the printed stimulus difference (omit steps 1-4) cannot
            // move _ready at all (no _motor term anywhere in its cone -- see
            // ID-CHANNEL-EXISTS). Substitute a genuine, independently-live
            // stimulus difference in its place -- run the WITHOUT-init
            // procedure on drive 1 instead of drive 0 -- through the IDENTICAL
            // != comparator and the SAME label, proving the comparator is
            // sensitive to a real difference.
            without_init = run_device_id_procedure(r_without, /*drive=*/1, /*do_init=*/false);
        }
        checks.note("MOTOR-CYCLE-INIT-CHANGES-ID-STREAM: with-init sequence = "
                    + bits_to_string(with_init) + "; without-init sequence ("
                    + std::string(mut ? "mutated: drive1 substituted for the omitted motor cycle"
                                       : "as printed: steps 1-4 simply omitted")
                    + ") = " + bits_to_string(without_init));
        checks.expect(with_init != without_init,
            "MOTOR-CYCLE-INIT-CHANGES-ID-STREAM (p.371: 'Steps 1 through 4 in the algorithm "
            "above turn on and off the disk drive motor. This initializes the serial shift "
            "register. After initialization, the SELxB signal is driven (first active then) "
            "inactive as in steps 5 and 6.'): if the steps-1-4 motor on/off cycle initialises a "
            "serial shift register as printed, the 32-sample sequence steps 5-9 read back "
            "afterward must differ from the same steps-5-9 procedure run without that cycle -- "
            "a different proposition, on a different axis (two full 32-pulse sequences compared "
            "against each other), from hrm/appDE-disk-interface's P-READY-MOTOR-INVARIANT (one "
            "_motor toggle, one !=, selection held fixed). MEASURED red: paula_floppy.v:688-694 "
            "has no _motor term anywhere in its cone (module's own comment: 'not implemented - "
            "_ready is active when _sel is active'), and motor_on's four flip-flops "
            "(paula_floppy.v:624-659) feed nothing _ready reads, so the two sequences are "
            "identical regardless of whether steps 1-4 ran: the motor cycle initialises nothing "
            "observable at this boundary. sol:H-081 opus:AHJ-108");
    }

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";

    return checks.failed == 0 ? 0 : 1;
}

// MUTATION FIXTURE-LIVE stimulus red: this check is GREEN (PASS) at baseline, so an oracle
//   mutation would show only that its != comparator can be made false, not that the _sel -> _ready
//   stimulus path is live -- the one thing a liveness control exists to establish. The stimulus
//   substitutes driving the SAME _sel state twice (select(0) again) for the second, distinct state
//   (deselect_all()), so the two known-distinct selections collapse to one and cannot be told
//   apart, through the identical != comparator and the same label, flipping baseline GREEN to
//   mutated RED (MUTATE=FIXTURE-LIVE).
// MUTATION ID-CHANNEL-EXISTS stimulus green: this assertion is RED at baseline (the real
//   finding: no identification shift register exists, so pulsing one _sel line 32 times reads
//   the same value 32 times), so per this suite's rule against mutating an already-failing check
//   with a no-op, the stimulus substitutes a genuine, independently-live change -- alternating
//   between _sel[0] and _sel[1] each iteration instead of pulsing _sel[0] throughout -- through
//   the identical all_identical() comparator and the same label, flipping baseline RED to
//   mutated GREEN and proving the comparator is sensitive to a real difference (MUTATE=ID-CHANNEL-EXISTS).
// MUTATION MOTOR-CYCLE-INIT-CHANGES-ID-STREAM stimulus green: this assertion is RED at baseline
//   (no _motor term anywhere in _ready's cone, so the with-init and without-init sequences are
//   identical), so per the same rule, the stimulus substitutes running the without-init trial on
//   drive 1 instead of drive 0 in place of the omitted motor cycle, through the identical !=
//   comparator and the same label, flipping baseline RED to mutated GREEN
//   (MUTATE=MOTOR-CYCLE-INIT-CHANGES-ID-STREAM).
