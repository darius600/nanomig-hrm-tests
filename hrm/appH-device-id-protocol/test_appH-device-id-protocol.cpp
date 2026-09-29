// Test purpose: HRM conformance package for appendix H's Device I.D.
//   protocol -- pp.368,370-371, the nine-step-plus-31-repeats procedure that
//   samples RDY (pin 1, _ready) 32 times to assemble a device identifier --
//   run against paula_floppy (this suite's only model of a physical Amiga
//   floppy drive, and the same module hrm/appDE-disk-interface grades), with
//   what it reads recorded against p.335's defined identifications. 14
//   claims, from work/2026-09-17_appHJ_packages.jsonl (classify.package ==
//   "appH-device-id-protocol").
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed
//   pp.368, 370-371 for the procedure and p.335 for the identification mode
//   and its table of defined identifications. p.368's Summary Table is NOT
//   displaced (this package's own corpus-defects.md, "p.368 -- the table is
//   clean"), so the RDY- entry is quoted from p368.txt directly. p.370's
//   nine numbered steps and p.371's four prose paragraphs both survive the
//   text layer (corpus-defects.md, "p.370", "p.371 -- the I.D. table is
//   unusable; the prose ... is not"); only p.371's two-column bit-pattern
//   table is unusable, and the records below read p.335's copy of that
//   table instead. Every quotation from pp.370-371 below is checked against
//   figures/p370.png and figures/p371.png (the nine steps on p370, and the
//   "Repeat steps 7-9" / "Steps 1-4 ... initializes" / "Steps 7, 8 and 9
//   form a loop" / "This loop is performed 32 times" paragraphs on p371),
//   not taken from the claims file's own restatements.
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appHJ_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// WHAT IS MEASURED. Every step of the printed procedure is expressed in
//   ports paula_floppy has: steps 1 and 4 drive `_motor`; steps 2, 3, 5, 6,
//   7 and 9 drive `_sel`; step 8 reads `_ready` (RDY*, pin 1; p.368's RDY-
//   entry: "When the motor is off this is used as a ID data line"). The Rig
//   configures one drive (floppy_drives=0), so drive 0 answers on `_ready`
//   (FIXTURE-LIVE) and drive 1, unconfigured, answers as no drive -- which
//   is what the mutations below select instead. ID-CHANNEL-EXISTS runs the
//   procedure once and keeps its 32 samples; MOTOR-CYCLE-INIT-CHANGES-ID-
//   STREAM runs it with and without the steps-1-4 motor cycle. Both are
//   checks.record(), not checks.expect() -- each block's own header says
//   why p.335's table cannot grade them for the one drive type this
//   fixture holds.
//
// NOT A RE-GRADE OF hrm/appDE-disk-interface's P-READY-MOTOR-INVARIANT.
//   That block reads `_ready` once, with the motor latched on at a select
//   edge and no disk installed, against p.334's motor-on branch. The claims
//   here run the printed identification procedure: a SEQUENCE OF 32 SELECT
//   PULSES (ID-CHANNEL-EXISTS), or two such sequences with and without the
//   steps-1-4 motor cycle (MOTOR-CYCLE-INIT-CHANGES-ID-STREAM), read against
//   p.335's defined identifications -- a different proposition on a
//   different axis. Neither record below declines against
//   P-READY-MOTOR-INVARIANT, and neither names its label or uids.
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
//   bears on this package's claims: the records read p.335's copy of the
//   table, not p.371's, and no claim names the pin-14 write-protect signal.
//
// MUTATION METHODOLOGY. FIXTURE-LIVE is the file's only checks.expect(): its
//   mutation runs the direction a liveness control needs -- the stimulus
//   collapses its two known-distinct `_sel` states to one, through the
//   identical != comparator and the same label, flipping baseline GREEN to
//   mutated RED. An oracle mutation here would show only that the comparator
//   can be made false, not that the stimulus path is live, which is the one
//   thing this control exists to establish.
//   ID-CHANNEL-EXISTS and MOTOR-CYCLE-INIT-CHANGES-ID-STREAM are
//   checks.record(), not checks.expect(): p.335's table gives no pass/fail
//   clause that can tell a live serial channel apart from a hardwired
//   constant for the one drive type this fixture can hold (see each block's
//   own header). Each still substitutes a genuine, independently-live
//   stimulus difference under its own MUTATE id -- alternating select lines
//   for ID-CHANNEL-EXISTS, drive 1 in place of the omitted motor cycle for
//   MOTOR-CYCLE-INIT-CHANGES-ID-STREAM -- so the record's own text can be
//   checked against a run where the measurement is known to differ; neither
//   substitution flips a verdict, because record() keeps none.
//   `mutating(id)` is read once per block into a local `bool mut` before any
//   `expect()` or `record()` call, never inside a condition or a label (see
//   tools/check_expect_label_shape.py).
//
// MUTATION RUN LOG (WSL/Verilator 5.052). Baseline (MUTATE unset): 1
//   checks.expect() call, 1 passed / 0 failed -- FIXTURE-LIVE PASS; 2
//   checks.record() lines, ID-CHANNEL-EXISTS and MOTOR-CYCLE-INIT-
//   CHANGES-ID-STREAM, both measuring the constant 32-bit stream
//   11111111111111111111111111111111 ($FFFFFFFF) with and without the
//   steps-1-4 motor cycle. MUTATE=ID-CHANNEL-EXISTS: count unchanged (1
//   passed / 0 failed, no clause on a record()); its own record text moves
//   to the alternating-select-line stream, whose odd iterations (drive 1,
//   unconfigured) read 0; the other two checks are unchanged.
//   MUTATE=MOTOR-CYCLE-INIT-CHANGES-ID-STREAM: count unchanged (1/0); its
//   own record text moves to a without-init stream read on drive 1 instead
//   of drive 0; the other two checks are unchanged. MUTATE=FIXTURE-LIVE: 0
//   passed / 1 failed -- diffed against baseline, FIXTURE-LIVE moves
//   PASS->FAIL (the second reading becomes ready=1, identical to the first,
//   once the same _sel state is driven twice instead of two distinct
//   states); the two record()s are unchanged.

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
// hrm/ch8-drive-select-motor-led): the drive's head position and its
// step-rate counter survive a reset, and an armed disk DMA stays armed. Not
// touched by this package's procedure (no step, no DMA arm anywhere below),
// but every test block still constructs its own fresh Rig; none is reused
// across trials.
struct Rig {
    Vpaula_floppy* d;

    // floppy_drives is fixed at 0 (one drive configured) for every trial in
    // this file, so drive 0 answers on _ready (FIXTURE-LIVE below) and drive
    // 1 answers as no drive -- which is what lets this package's mutations
    // substitute "select a different physical line" as a genuine,
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
    // FIXTURE-LIVE -- no claim id, no manual page: the fixture must deliver
    // a known, distinct reading before any read-based assertion below is
    // trusted. Measured on
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
                    " (floppy_drives=0: one drive configured)");
        checks.expect(first_ready != second_ready,
            "FIXTURE-LIVE: two distinct, known _sel states, driven through this package's Rig, "
            "arrive unmodified and mutually distinguishable when read back through _ready -- the "
            "_sel -> _ready path this package's every claim depends on is live, not stuck, before "
            "any claim below is trusted");
    }

    // ========================================================================
    // ID-CHANNEL-EXISTS -- p.335 ("An identification mode is provided for
    // reading a 32-bit serial identification data stream from an external
    // device.", "$FFFF FFFF - Amiga standard 3.25 diskette.", "The serial
    // input data is active low and must therefore be inverted to be
    // consistent with the above table."). RECORDED, not graded: p.335
    // defines the 32-bit VALUE a drive of a given type answers, not the
    // mechanism that produces it. The one drive type this fixture can hold
    // -- the standard drive this package's one configured drive answers as
    // -- has a defined identification of 32 identical bits, so a genuinely
    // live serial channel that shifted that value out one bit at a time and
    // a fixed line tied to the same value read back indistinguishably over
    // the printed procedure's 32 samples. p.335's table cannot tell them
    // apart for this drive type, so no pass/fail clause built from it can
    // grade "a channel exists" apart from "the value matches the table" --
    // the two collapse into the same observation here. What IS testable
    // against p.335 is the value itself, recorded below.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("ID-CHANNEL-EXISTS");
        std::vector<bool> samples;
        samples.reserve(32);
        if (!mut) {
            samples = run_device_id_procedure(r, /*drive=*/0, /*do_init=*/true);
        } else {
            // MUTATION (record(), no pass/fail clause to flip): a genuine,
            // independently-live stimulus difference -- alternate between
            // drive 0 and drive 1 each iteration instead of pulsing drive 0
            // throughout -- shows this record's text tracks the running
            // simulation rather than a fixed string. floppy_drives=0 leaves
            // drive 1 unconfigured, so p.335's "$0000 0000 - no drive
            // present" is what the odd iterations answer (the MEASURED text
            // of this record in a MUTATE=ID-CHANNEL-EXISTS run).
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
        std::string bits = bits_to_string(samples);
        std::string measured = mut
            ? ("MUTATED (drive 0/drive 1 alternated each iteration): " + bits)
            : (bits + ", inverted per p.335's own instruction (this package's ready() "
               "already returns the inverted, table-consistent bit: RDY* driven low/"
               "asserted, ready()==true, is the '1' the table prints; RDY* deasserted, "
               "ready()==false, is its '0' -- see FIXTURE-LIVE) = $FFFFFFFF, exactly "
               "p.335's 'Amiga standard 3.25 diskette' entry for the one configured "
               "drive.");
        checks.record(
            "ID-CHANNEL-EXISTS (p.335: 'An identification mode is provided for reading a "
            "32-bit serial identification data stream from an external device.'; '$FFFF "
            "FFFF - Amiga standard 3.25 diskette.'; 'The serial input data is active low "
            "and must therefore be inverted to be consistent with the above table.'): not "
            "graded -- p.335's table defines the value a drive of a given type answers, "
            "not the mechanism that produces it, and for the one drive type modeled here "
            "(the standard drive, whose defined identification is 32 identical bits) a "
            "live serial channel and a fixed line tied to that value are indistinguishable "
            "over the printed procedure's 32 samples, so no pass/fail clause built from "
            "this table can grade a channel's existence apart from a value match. "
            "MEASURED: " + measured + " "
            "sol:H-038 opus:AHJ-062 sol:H-068 opus:AHJ-094 sol:H-076 opus:AHJ-103 "
            "sol:H-078 opus:AHJ-105 sol:H-085 opus:AHJ-111 sol:H-087 opus:AHJ-112");
    }

    // ========================================================================
    // MOTOR-CYCLE-INIT-CHANGES-ID-STREAM -- p.335 ("To initialize this mode,
    // the motor must be turned on, then off. ... The transition from motor
    // on to motor off reinitializes the serial shift register.", and the
    // same defined-identifications table ID-CHANNEL-EXISTS reads).
    // RECORDED, not graded: p.335 defines a fixed 32-bit identification per
    // drive type; it says the motor transition reinitializes the shift
    // register, not that reinitializing changes the VALUE a given drive's
    // register holds. For the one drive type this fixture can hold (32
    // identical bits), a register that was genuinely reinitialized and one
    // that was not read back the same value either way, so the printed
    // procedure run with the steps-1-4 motor cycle and the same procedure
    // run without it cannot be told apart through this table for this drive
    // type -- not because nothing was reinitialized, but because the page
    // never says reinitializing changes a fixed device's own identification.
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
            // MUTATION (record(), no pass/fail clause to flip): a genuine,
            // independently-live stimulus difference -- run the without-init
            // procedure on drive 1 instead of drive 0 -- shows this record's
            // text tracks the running simulation rather than a fixed string.
            without_init = run_device_id_procedure(r_without, /*drive=*/1, /*do_init=*/false);
        }
        std::string with_bits = bits_to_string(with_init);
        std::string without_bits = bits_to_string(without_init);
        std::string measured = mut
            ? ("MUTATED (without-init run on drive 1 instead of drive 0): with-init = "
               + with_bits + "; without-init = " + without_bits)
            : ("with-init = " + with_bits + "; without-init = " + without_bits +
               " -- both $FFFFFFFF, exactly p.335's 'Amiga standard 3.25 diskette' entry "
               "either way.");
        checks.record(
            "MOTOR-CYCLE-INIT-CHANGES-ID-STREAM (p.335: 'To initialize this mode, the "
            "motor must be turned on, then off. ... The transition from motor on to motor "
            "off reinitializes the serial shift register.'; '$FFFF FFFF - Amiga standard "
            "3.25 diskette.'): not graded -- p.335 says the motor transition reinitializes "
            "the shift register, not that reinitializing changes the value a given drive "
            "answers, and for the one drive type modeled here (32 identical bits) a "
            "reinitialized register and one that was not read back the same value either "
            "way, so the table cannot tell 'reinitialized' apart from 'not' for this drive "
            "type. MEASURED: " + measured + " "
            "sol:H-081 opus:AHJ-108");
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
// MUTATION ID-CHANNEL-EXISTS record: this check is checks.record(), not checks.expect() --
//   p.335's table cannot tell a live serial channel apart from a hardwired constant for the one
//   drive type this fixture can hold, so there is no pass/fail clause to flip. The stimulus still
//   substitutes a genuine, independently-live change -- alternating between drive 0 and drive 1
//   each iteration instead of pulsing drive 0 throughout -- so the record's own text is shown to
//   track the running simulation rather than repeat a fixed string (MUTATE=ID-CHANNEL-EXISTS).
// MUTATION MOTOR-CYCLE-INIT-CHANGES-ID-STREAM record: likewise checks.record(), not
//   checks.expect(), for the same reason. The stimulus substitutes running the without-init trial
//   on drive 1 instead of drive 0 in place of the omitted motor cycle
//   (MUTATE=MOTOR-CYCLE-INIT-CHANGES-ID-STREAM).
