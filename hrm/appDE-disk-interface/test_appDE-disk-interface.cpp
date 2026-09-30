// Test purpose: HRM conformance package for appendix E's External Disk
//   Interface Connector (J7), pp.334-335 -- the 23-pin DB23S connector's pin
//   assignment table (p.334) and its identification-mode/limitations text
//   (p.335) -- graded against paula_floppy (this suite's only model of a
//   physical Amiga floppy drive). 26 claims, from
//   work/2026-09-17_appDE_packages.jsonl (classify.package ==
//   "appDE-disk-interface").
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed
//   pp.334-335. p.334's NOTES column is severely displaced in the text layer
//   (docs/references/amiga-hrm-3rd/appE/corpus-defects.md, "p.334 --
//   EXTERNAL DISK CONNECTOR PIN ASSIGNMENT J7"), so every pin/note pairing
//   used below is read from figures/p334.png, not from p334.txt. p.335
//   carries no substantive binding displacement (corpus-defects.md's
//   per-page table) and is quoted from p335.txt.
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// RDY* (p.334 pin 1). p.244 does not say what _ready means with the motor
//   off, and hrm/ch8-drive-sense-lines declines its P-READY proposition
//   (opus:8-cia.c7) on that ground. p.334 pin 1 states both branches: "If
//   motor on, indicates disk installed and up to speed. If motor not on,
//   identification mode." sol:E-disk.c26 and opus:E-extdisk-iface.c4 are
//   therefore graded here against p.334, in P-READY-MOTOR-INVARIANT below:
//   the motor is latched on at a select edge with no disk installed, and
//   the printed motor-on branch then predicts RDY* deasserted. The 32-bit
//   serial identification protocol p.335 describes is not among this
//   package's 26 claims (its own claims, sol:E-disk-id.c1 and
//   opus:E-extdisk-id.c2, are classified not-asserted);
//   hrm/appH-device-id-protocol runs the printed procedure and records what
//   it reads. This package's 26 hold the pin-level claims of p.334 and the
//   device-count claims of p.335 (sol:E-disk.c51 / opus:E-extdisk-id.c16,
//   DEVICE-LIMITS below).
//
// FIXTURE BOUNDARY. paula_floppy's disk-side ports are named single-bit
//   lines: inputs _step, direc, _sel[3:0], side, _motor; outputs _track0,
//   _change, _ready, _wprot, index. No port carries a DB23 pin number, so
//   the pin ordinal a p.334 claim cites has no counterpart at this boundary
//   and is recorded as such; hrm/ch8-cia-disk-signal-bindings,
//   hrm/ch8-drive-select-motor-led and hrm/ch8-drive-sense-lines grade the
//   same lines at their CIA port bits. Each block below states what it
//   measures and on which page it rests.
//
// NOT GRADABLE ANYWHERE IN THIS PACKAGE (recorded via record(), never
//   graded): the DB23 pin ordinal itself (no port carries one, see FIXTURE
//   BOUNDARY above); the 1.4 usec MTRXD* setup/hold figures and any other
//   absolute real-time interval (the fixture counts clk7_en ticks and no
//   port carries a clock period, the same reason hrm/ch8-drive-step-timing
//   declines absolute figures); DIRB's "towards center of disk" physical
//   geometry (no port represents physical direction, only the
//   track-ordinal half, which is already graded); the pins' printed
//   drive-class letters (I/O, OC, O); and the daisy-chain shift footnote,
//   a separate proposition with claims of its own (sol:E-disk.c49 /
//   opus:E-extdisk-iface.c27, classified not-asserted).
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- one build serves the real run (MUTATE unset) and every
//   mutation run (MUTATE=<id>, one id per assertion). Ids are the claim's
//   own `classify.uid` for the corpus-cited claims (this suite's
//   convention) and a plain uppercase id for FIXTURE-LIVE, RESET-NO-WRITE,
//   FIXTURE-LIVE-DRIVE1 and P-READY-MOTOR-INVARIANT-DRIVE1, which carry no
//   classify.uid of their own -- each still cites its page, it is simply
//   not a reader's reading of one corpus sentence. sol:E-disk.c26 and
//   opus:E-extdisk-iface.c4 read RED at baseline: p.334 pin 1 and p.369
//   predict RDY* deasserted for a drive whose motor is genuinely latched on
//   with no disk installed, and paula_floppy reads it asserted instead.
//   P-READY-MOTOR-INVARIANT-DRIVE1 corroborates the same defect on drive 1
//   (NM-HRM-080's own "What weakens it" -- the graded reds ran on drive 0
//   only) and is RED at baseline for the identical reason. Per this suite's
//   rule against mutating an already-failing check with a no-op, each of
//   these three substitutes a DIFFERENT genuine stimulus (deselecting the
//   drive after the motor has been genuinely latched, rather than leaving
//   it latched) through the IDENTICAL comparator and the SAME label,
//   flipping baseline RED to mutated GREEN. The motor-latch precondition is
//   measured through peek_index() in every branch of every one of these
//   three, not assumed constant.
//   The remaining thirteen graded checks are GREEN at baseline and flip to
//   RED by omitting the stimulus event the proposition requires.
//
// MUTATION RUN LOG (WSL/Verilator 5.052): baseline (MUTATE unset) 16
//   checks.expect() calls, 13 passed / 3 failed. The 3 baseline FAILs are
//   sol:E-disk.c26, opus:E-extdisk-iface.c4 (p.334 pin 1 RDY*, drive 0 --
//   see P-READY-MOTOR-INVARIANT below for what is measured and which page
//   it rests on) and P-READY-MOTOR-INVARIANT-DRIVE1 (the same measurement
//   on drive 1). Each of the 13 MUTATE=<id> runs targeting a baseline-GREEN
//   assertion, diffed line by line against the baseline's PASS/FAIL output,
//   moves EXACTLY the targeted label's own line and no other: FIXTURE-LIVE,
//   FIXTURE-LIVE-DRIVE1, sol:E-disk.c35, opus:E-extdisk-iface.c13,
//   RESET-NO-WRITE, sol:E-disk.c36, opus:E-extdisk-iface.c14,
//   sol:E-disk.c38, opus:E-extdisk-iface.c16, sol:E-disk.c39,
//   opus:E-extdisk-iface.c17, sol:E-disk.c51 and opus:E-extdisk-id.c16 each
//   PASS->FAIL (12 passed / 4 failed per run); sol:E-disk.c26,
//   opus:E-extdisk-iface.c4 and P-READY-MOTOR-INVARIANT-DRIVE1 each
//   FAIL->PASS (14 passed / 2 failed per run), the other two baseline-RED
//   lines unaffected in every case.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
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

// RIG HAZARD (same as hrm/ch8-drive-sense-lines and
// hrm/ch8-drive-select-motor-led): the drive's head position and its
// step-rate counter survive a reset, and an armed disk DMA stays armed.
// Every test block below constructs its own fresh Rig; none is reused
// across trials.
struct Rig {
    Vpaula_floppy* d;

    // drives_cfg is the floppy_drives input, which the fixture samples at
    // reset only. 0 (one drive configured) is every block's default; the
    // select-line block, the drive 1-3 reset replays and DEVICE-LIMITS pass
    // 3 (all four select lines live, as DEVICE-LIMITS measures).
    explicit Rig(uint8_t drives_cfg = 0) {
        d = new Vpaula_floppy;
        d->clk = 0;
        d->clk7_en = 1;   // held high throughout
        d->clk7n_en = 0;
        d->reset = 1;
        d->ntsc = 0;
        d->sof = 0;
        d->enable = 0;
        d->reg_address_in = 0xFF;  // an idle register-select value
        d->data_in = 0;

        d->_step = 1;   // idle high; active low
        d->direc = 0;
        d->_sel = 0xF;  // all four drives deselected; active low
        d->side = 0;
        d->_motor = 1;  // motor off; active low

        d->wordsync = 0;
        d->floppy_drives = drives_cfg;
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
    // Same shape as do_reset(), callable mid-test after the model has
    // already been driven into a chosen state (RESET-DUTY,
    // CHANGE-SET-AT-POWERUP).
    void pulse_reset() { do_reset(); }

    void select(int n) {
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        d->eval();
    }
    void deselect_all() { d->_sel = 0xF; d->eval(); }

    void saturate_step_ena() { ticks(300); }

    void do_step(bool direc_outward) {
        d->direc = direc_outward ? 1 : 0;
        d->_step = 0;
        tick();
        d->_step = 1;
        tick();
        tick();
    }

    bool track0() { d->eval(); return d->_track0 == 0; }  // active low
    bool ready()  { d->eval(); return d->_ready == 0; }   // active low
    bool wprot()  { d->eval(); return d->_wprot == 0; }   // active low
    bool change() { d->eval(); return d->_change == 0; }  // active low

    void mount(int n, uint32_t size) {
        d->sdc_image_mounted = (uint8_t)(1u << n);
        d->sdc_image_size = size;
        ticks(4);
        d->sdc_image_mounted = 0;
    }
    void unmount(int n) {
        d->sdc_image_mounted = (uint8_t)(1u << n);
        d->sdc_image_size = 0;
        ticks(4);
        d->sdc_image_mounted = 0;
    }

    // Select drive n with _motor driven to the requested level across a
    // GENUINE turn-on edge: deselect first and settle one tick, THEN set
    // the motor level and select -- the following tick() is the select
    // edge at which p.334 pin 8 has the drive clock MTRXD* into its
    // motor-on flip-flop.
    void select_with_motor(int n, bool motor_on_level) {
        deselect_all();
        tick();
        d->_motor = motor_on_level ? 0 : 1;  // DSKMOTOR* is active low
        select(n);
        tick();
    }

    // Read drive n's motor latch through the `index` output: select drive
    // n, raise sof, eval() and read, then restore -- with NO clock edge, so
    // the read itself changes no latched state (tick() never raises sof).
    // index reads 1 for a selected drive whose motor is latched on and 0
    // for one whose motor is latched off, as the MTRXD* block below
    // measures.
    bool peek_index(int n) {
        uint8_t saved_sel = d->_sel;
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        d->sof = 1;
        d->eval();
        bool v = d->index;
        d->sof = 0;
        d->_sel = saved_sel;
        d->eval();
        return v;
    }

    // One register write, held on the bus for four clk7_en ticks. For
    // DSKLEN, which must be written twice with bit 15 set to start a disk
    // DMA, one held call is enough: SIDE-POLARITY's armed read captures
    // sdc_sector and RESET-NO-WRITE's armed write raises dmal and fdd_led
    // (both in their MEASURED lines).
    void write_reg(int addr, uint16_t data) {
        d->reg_address_in = (uint8_t)((addr >> 1) & 0xFF);
        d->data_in = data;
        ticks(4);
        d->reg_address_in = 0xFF;
        d->data_in = 0;
    }

    // Arm a disk DMA READ on whichever drive is currently selected and run
    // long enough for the armed read to put its sector request on
    // `sdc_sector`.
    void arm_dma_read() {
        d->enable = 1;
        write_reg(0x024, 0x800A);  // DSKLEN: length 10 (0xA), bit14=0 (read), bit15=1 (dma enable)
        ticks(20);
    }

    // Arm a disk WRITE DMA on whichever drive is currently selected and run
    // long enough for the disk-dma state machine to leave idle -- the point
    // at which the DMA request (dmal) and the drive light (fdd_led) both go
    // live for a write, the same idiom as arm_dma_read() with the write bit
    // set.
    void arm_write_dma() {
        d->enable = 1;
        write_reg(0x024, 0xC00A);  // DSKLEN: length 10 (0xA), bit14=1 (write), bit15=1 (dma enable)
        ticks(20);
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    std::cout << "Starting appDE-disk-interface...\n";

    // ========================================================================
    // FIXTURE-LIVE -- no claim id, no manual page: the fixture must deliver
    // a known, distinct word before any read-based assertion below is
    // trusted. Measured on _wprot, one of the sense-line outputs several of
    // this package's graded claims read.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("FIXTURE-LIVE");
        r.select(0);
        r.d->floppy_wrprot = 0;
        bool v1 = r.wprot();
        // MUTATION: drive word 2 IDENTICAL to word 1 instead of its
        // complement, so the two readings cannot be told apart.
        r.d->floppy_wrprot = mut ? 0 : 1;
        bool v2 = r.wprot();
        checks.note("FIXTURE-LIVE: _wprot word1(floppy_wrprot=0)=" + std::to_string(v1) +
                    ", word2(floppy_wrprot=" + std::string(mut ? "0,mutated" : "1") + ")=" + std::to_string(v2));
        checks.expect(v1 == false && v2 == true && v1 != v2,
            "FIXTURE-LIVE: two distinct, known floppy_wrprot levels, driven through the selected drive, arrive "
            "unmodified and mutually distinguishable when read back through _wprot -- the sense-line output path "
            "is live, not stuck, before any claim below is trusted");
    }

    // ========================================================================
    // P-READY-MOTOR-INVARIANT -- sol:E-disk.c26 / opus:E-extdisk-iface.c4
    // (p.334 pin 1, RDY*, figure-verified: "If motor on, indicates disk
    // installed and up to speed. If motor not on, identification mode.").
    // Graded here against p.334, which states the branch p.244 leaves open
    // (see file header). The motor latches only at a select edge (p.334 pin 8, p.369, p.244); each block
    // below drives a genuine off-to-on transition of the select line
    // through select_with_motor(), with no disk mounted, and reads _ready
    // once the latch has taken. With no disk installed the printed
    // motor-on branch ("disk installed and up to speed") does not apply,
    // so p.334 and p.369 predict the line deasserted. Whatever paula_floppy
    // reports here is the measurement.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("sol:E-disk.c26");
        // no mount() call anywhere in this block: no disk is installed
        // throughout.
        bool motor_latched_on;
        if (!mut) {
            r.select_with_motor(0, /*motor_on_level=*/true);  // a genuine
                // off-to-on transition of the select line, the edge p.334
                // pin 8 and p.369 require to latch the motor on
            motor_latched_on = r.peek_index(0);
        } else {
            // MUTATION (rule against mutating an already-failing check):
            // substitute a genuine, independently-live stimulus change
            // (deselecting the drive after the motor was genuinely
            // latched) through the IDENTICAL !ready() comparator and the
            // same label, proving the comparator is sensitive to a real
            // difference. The precondition term is measured here too, not
            // assumed: latch it for real, read it back, then deselect.
            r.select_with_motor(0, /*motor_on_level=*/true);
            motor_latched_on = r.peek_index(0);
            r.deselect_all();
        }
        bool ready_state = r.ready();
        checks.note("sol:E-disk.c26: drive0, no disk, " +
                    std::string(mut ? "motor latched on then deselected (mutated substitute), motor latch="
                                     : "motor latched on at a genuine select edge, motor latch=") +
                    std::to_string(motor_latched_on) +
                    " -> ready=" + std::to_string(ready_state));
        checks.expect(motor_latched_on && !ready_state,
            "sol:E-disk.c26 (p.334, figure, pin 1 RDY*: 'If motor on, indicates disk installed and up to speed. "
            "If motor not on, identification mode.'; p.334 pin 8 and p.369 on the select-edge motor latch): the "
            "page's own precondition, measured -- the motor latch read back through peek_index(0) right after "
            "select_with_motor() -- must itself be on before this can mean anything; with "
            "the motor genuinely latched on at a select edge and no disk in the drive, the printed motor-on "
            "branch requires a disk installed and up to speed for RDY* to read asserted; with none installed the "
            "page predicts it deasserted");
    }
    {
        Rig r;
        bool mut = mutating("opus:E-extdisk-iface.c4");
        bool motor_latched_on;
        if (!mut) {
            r.select_with_motor(0, true);
            motor_latched_on = r.peek_index(0);
        } else {
            r.select_with_motor(0, true);
            motor_latched_on = r.peek_index(0);
            r.deselect_all();
        }
        bool ready_state = r.ready();
        checks.note("opus:E-extdisk-iface.c4: twin of sol:E-disk.c26, same measurement -> " +
                    std::string(mut ? "motor latched on then deselected (mutated substitute), motor latch="
                                     : "motor latched on at a genuine select edge, motor latch=") +
                    std::to_string(motor_latched_on) +
                    " -> ready=" + std::to_string(ready_state));
        checks.expect(motor_latched_on && !ready_state,
            "opus:E-extdisk-iface.c4 (p.334, figure, pin 1 RDY*: 'With the motor on it indicates a disk installed "
            "and up to speed; with the motor off it is identification mode.'): twin of sol:E-disk.c26 (same "
            "printed pin, independent reader) -- same measurement, graded independently under its own uid, with "
            "the motor latch read back through peek_index(0) as its own precondition term");
    }

    // ========================================================================
    // FIXTURE-LIVE-DRIVE1 -- no claim id, no manual page: P-READY-MOTOR-
    // INVARIANT-DRIVE1 below reads drive 1, whose _ready term only responds
    // to selection once floppy_drives configures an extra drive present
    // (the drive-count input, sampled at reset). Measured the same way
    // FIXTURE-LIVE above measured drive 0: before trusting a read on drive
    // 1, show selecting and deselecting it changes _ready at all.
    // ========================================================================
    {
        Rig r(1);  // floppy_drives=1: one extra drive configured, drive 1 present
        bool mut = mutating("FIXTURE-LIVE-DRIVE1");
        r.deselect_all();
        bool ready_deselected = r.ready();
        // MUTATION: never actually select drive 1 -- the "asserts once
        // selected" half is never exercised.
        if (!mut) r.select(1);
        bool ready_selected = r.ready();
        checks.note("FIXTURE-LIVE-DRIVE1: deselected -> _ready asserted=" + std::to_string(ready_deselected) +
                    ", " + std::string(mut ? "still deselected (mutated)" : "drive 1 selected") +
                    " -> _ready asserted=" + std::to_string(ready_selected));
        checks.expect(!ready_deselected && ready_selected,
            "FIXTURE-LIVE-DRIVE1: with floppy_drives configuring drive 1 present, deselecting every drive "
            "deasserts _ready and selecting drive 1 asserts it -- drive 1's sense-line read path is live "
            "before P-READY-MOTOR-INVARIANT-DRIVE1 below is trusted");
    }

    // ========================================================================
    // P-READY-MOTOR-INVARIANT-DRIVE1 -- no classify.uid of its own (same
    // convention as FIXTURE-LIVE and RESET-NO-WRITE above): corroborates
    // sol:E-disk.c26 / opus:E-extdisk-iface.c4 (p.334 pin 1, RDY*) on drive
    // 1, per the followup NM-HRM-080 names -- pp.334/368-370 describe
    // drives beyond drive 0, and P-READY-MOTOR-INVARIANT above only
    // exercises drive 0. Same setup: motor latched on at a genuine select
    // edge, no disk mounted; p.334's motor-on branch then predicts RDY*
    // deasserted. floppy_drives=1 (sampled at reset) is what makes drive 1
    // exist for this fixture; FIXTURE-LIVE-DRIVE1 above shows it does.
    // ========================================================================
    {
        Rig r(1);  // floppy_drives=1: drive 1 present, see FIXTURE-LIVE-DRIVE1
        bool mut = mutating("P-READY-MOTOR-INVARIANT-DRIVE1");
        // Read back right after select_with_motor(), RESET-DUTY's own
        // technique (peek_index(n), no clock edge) -- the page's own
        // sentence starts with the motor condition, and this block
        // MEASURES the latch in every branch, not just states it as
        // literal text.
        bool motor_latched_on;
        if (!mut) {
            r.select_with_motor(1, /*motor_on_level=*/true);
            motor_latched_on = r.peek_index(1);
        } else {
            // MUTATION (rule against mutating an already-failing check):
            // substitute a genuine, independently-live stimulus change
            // (deselecting drive 1 after the motor was genuinely latched)
            // through the IDENTICAL !ready() comparator and the same
            // label. The precondition term is measured here too, not
            // passed in as a constant: latch it for real, read it back,
            // then deselect.
            r.select_with_motor(1, /*motor_on_level=*/true);
            motor_latched_on = r.peek_index(1);
            r.deselect_all();
        }
        bool ready_state = r.ready();
        checks.note("P-READY-MOTOR-INVARIANT-DRIVE1: drive1, no disk, " +
                    std::string(mut ? "motor latched on then deselected (mutated substitute), motor latch="
                                     : "motor latched on at a genuine select edge, motor latch=") +
                    std::to_string(motor_latched_on) +
                    " -> ready=" + std::to_string(ready_state));
        checks.expect(motor_latched_on && !ready_state,
            "P-READY-MOTOR-INVARIANT-DRIVE1 (p.334, figure, pin 1 RDY*, corroborating sol:E-disk.c26 / "
            "opus:E-extdisk-iface.c4 on drive 1): the page's own precondition, measured -- the motor latch "
            "read back through peek_index(1) right after select_with_motor() -- must itself be on before this "
            "can mean anything; with drive 1 configured present, the motor genuinely latched on at a select "
            "edge and no disk in the drive, the printed motor-on branch then requires a disk installed and up "
            "to speed for RDY* to read asserted; with none installed the page predicts it deasserted on drive "
            "1 as on drive 0");
    }

    // ========================================================================
    // Already-graded select/motor/step mechanisms, p.334, DECLINED as
    // re-grades of accepted chapter-8 packages -- the same behaviour those
    // packages grade at a CIA port bit, reached here through a connector
    // pin instead, confirmed per pin below. The DB23 pin ordinal itself has
    // no counterpart at the fixture's boundary (see the file header's
    // FIXTURE BOUNDARY) and is recorded as such alongside each re-grade.
    // ========================================================================

    // -- pin 8 MTRXD*, OC: motor-on flip-flop clock mechanism, plus the
    //    1.4 usec setup/hold figures (absolute time, no counterpart).
    //    sol:E-disk.c33 / opus:E-extdisk-iface.c11.
    {
        Rig r;
        r.select_with_motor(0, true);
        bool latched_on = r.peek_index(0);
        r.select_with_motor(0, false);
        bool latched_off = r.peek_index(0);
        checks.note("E-disk.c33/E-extdisk-iface.c11: own confirmation of the clocked-at-select-edge motor latch -- "
                    "drive0 latched ON -> index=" + std::to_string(latched_on) +
                    ", then re-latched OFF at its own next edge -> index=" + std::to_string(latched_off));
        checks.record(
            "sol:E-disk.c33 (p.334, figure, pin 8 MTRXD*, OC: 'Motor on data, clocked into drive's motor-on "
            "flip-flop by the active transition of SELxB*. Guaranteed setup time is 1.4 usec. Guaranteed hold "
            "time is 1.4 usec.'): the clocked-by-SELxB* mechanism (the motor level taken at drive n's own "
            "select edge, confirmed above through index) is already graded -- "
            "hrm/ch8-drive-select-motor-led's P1/P2/P4 (six uids) grade exactly this at `paula_floppy` from "
            "p.244. Decline as a re-grade. The 1.4 usec setup/hold figures are absolute real-time intervals this "
            "fixture cannot bound (it counts clk7_en ticks and no port carries a clock period, the same reason "
            "hrm/ch8-drive-step-timing declines absolute figures), and the DB23 pin ordinal has no counterpart "
            "at the fixture's boundary (no port carries a pin number) -- neither is gradable, whatever the "
            "mechanism's grading status.");
        checks.record(
            "opus:E-extdisk-iface.c11 (p.334, figure, pin 8 MTRXD*, OC): twin of sol:E-disk.c33 (same printed "
            "pin, independent reader) -- same reasons, declined independently under its own uid.");
    }

    // -- pins 9/20/21 SEL2B*/SEL3B*/SEL1B*, OC: per-drive select lines.
    //    sol:E-disk.c34/c45/c46, opus:E-extdisk-iface.c12/c23/c24.
    {
        // No port reports which drive is selected, so the confirmation reads
        // each select line's effect through `_ready`, with floppy_drives=3
        // (all four select lines live, as DEVICE-LIMITS measures).
        Rig r(3);
        r.select(1); bool sel_is_1 = r.ready(); r.deselect_all();
        r.select(2); bool sel_is_2 = r.ready(); r.deselect_all();
        r.select(3); bool sel_is_3 = r.ready(); r.deselect_all();
        checks.note("E-disk.c34/c45/c46 and twins: own confirmation of the select-line-to-drive binding "
                    "via _ready (floppy_drives=3) -- _sel[1] alone asserts ready: " + std::to_string(sel_is_1) +
                    "; _sel[2] alone asserts ready: " + std::to_string(sel_is_2) +
                    "; _sel[3] alone asserts ready: " + std::to_string(sel_is_3));
        checks.record(
            "sol:E-disk.c34 (p.334, figure, pin 9 SEL2B*, OC: 'Select drive 2.*'): the select-line-to-drive-number "
            "binding (confirmed above) is already graded -- "
            "hrm/ch8-cia-disk-signal-bindings grades PB5 DSKSEL2* at its CIA port bit and "
            "hrm/ch8-drive-select-motor-led grades per-drive select behaviour at `paula_floppy`. Decline as a "
            "re-grade. The DB23 pin ordinal has no counterpart at the fixture's boundary. The daisy-chain shift "
            "footnote printed under this table is a separate proposition with claims of its own "
            "(sol:E-disk.c49 / opus:E-extdisk-iface.c27, classified not-asserted), not this uid.");
        checks.record("opus:E-extdisk-iface.c12 (p.334, figure, pin 9 SEL2B*, OC): twin of sol:E-disk.c34 -- same "
                       "reasons, declined independently under its own uid.");
        checks.record(
            "sol:E-disk.c45 (p.334, figure, pin 20 SEL3B*, OC: 'Select drive 3. *'): same mechanism, confirmed "
            "above for _sel[3] -- hrm/ch8-cia-disk-signal-bindings grades PB6 DSKSEL3* at its CIA port bit. "
            "Decline as a re-grade. DB23 pin ordinal not gradable; daisy-chain footnote a separate proposition.");
        checks.record("opus:E-extdisk-iface.c23 (p.334, figure, pin 20 SEL3B*, OC): twin of sol:E-disk.c45 -- same "
                       "reasons, declined independently under its own uid.");
        checks.record(
            "sol:E-disk.c46 (p.334, figure, pin 21 SEL1B*, OC: 'Select drive 1. *'): same mechanism, confirmed "
            "above for _sel[1] -- hrm/ch8-cia-disk-signal-bindings grades PB4 DSKSEL1* at its CIA port bit. "
            "Decline as a re-grade. DB23 pin ordinal not gradable; daisy-chain footnote a separate proposition.");
        checks.record("opus:E-extdisk-iface.c24 (p.334, figure, pin 21 SEL1B*, OC): twin of sol:E-disk.c46 -- same "
                       "reasons, declined independently under its own uid.");
    }

    // ========================================================================
    // RESET-DUTY -- sol:E-disk.c35 / opus:E-extdisk-iface.c13 (p.334 pin 10
    // DRESB*, OC, figure-verified: "Amiga system reset. Drives should reset "
    // "their motor-on flip-flops and set their write-protect flip-flops.").
    // Split into two independently-measured halves. The motor half is
    // measured directly (the latch read through index) and stays graded
    // under these uids. p.334 pin 10 and p.370 also say a reset sets a
    // write-protect flip-flop, but no page (334, 369, 370, 244) names a
    // line that flip-flop drives or an event that clears it -- p.334 pin 14
    // and p.369 tie WPRO* to the disk's own protection instead -- so that
    // half is recorded below, not graded. RESET-NO-WRITE, further down,
    // grades the one consequence pp.334 and 244 do fix for the reset: a
    // drive does not write while it is held.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("sol:E-disk.c35");
        r.select_with_motor(0, /*motor_on_level=*/true);
        bool motor_on_before_reset = r.peek_index(0);
        if (!mut) r.pulse_reset();
        // MUTATION: omit the reset pulse -- the motor latch is never asked
        // to clear, so the "before" and "after" readings are the same
        // latched state.
        bool motor_on_after_reset = r.peek_index(0);
        bool motor_half = motor_on_before_reset && !motor_on_after_reset;
        bool wprot_after_reset = r.wprot();  // drive0 still selected; recorded below, not graded
        checks.note("sol:E-disk.c35: motor latch (read through index) before reset=" + std::to_string(motor_on_before_reset) +
                    ", after " + std::string(mut ? "NO reset (mutated)" : "reset") + "=" +
                    std::to_string(motor_on_after_reset) + "; _wprot after reset (floppy_wrprot=0, recorded, not "
                    "graded)=" + std::to_string(wprot_after_reset));
        checks.expect(motor_half,
            "sol:E-disk.c35 (p.334, figure, pin 10 DRESB*, OC: 'Amiga system reset. Drives should reset their "
            "motor-on flip-flops...'): the motor half of the reset duty -- a drive latched on before the reset "
            "reads off once the reset is released");
        checks.record(
            "sol:E-disk.c35, write-protect half (p.334 pin 10 and p.370: '...and set their write-protect "
            "flip-flops.'): no page (334, 369, 370, 244) names a line this flip-flop drives or an event that "
            "clears it; p.334 pin 14 and p.369 tie WPRO* to a selected, write-protected disk instead. Recorded, "
            "not graded -- _wprot reads " + std::to_string(wprot_after_reset) + " after this reset with a "
            "writable disk (floppy_wrprot=0)");
    }
    {
        Rig r;
        bool mut = mutating("opus:E-extdisk-iface.c13");
        r.select_with_motor(0, true);
        bool motor_on_before_reset = r.peek_index(0);
        if (!mut) r.pulse_reset();
        bool motor_on_after_reset = r.peek_index(0);
        bool motor_half = motor_on_before_reset && !motor_on_after_reset;
        bool wprot_after_reset = r.wprot();
        checks.note("opus:E-extdisk-iface.c13: twin of sol:E-disk.c35, same measurement -> motor half=" +
                    std::string(motor_half ? "GREEN" : "RED") + "; _wprot after reset (recorded, not graded)=" +
                    std::to_string(wprot_after_reset));
        checks.expect(motor_half,
            "opus:E-extdisk-iface.c13 (p.334, figure, pin 10 DRESB*, OC): twin of sol:E-disk.c35 (same printed "
            "pin, independent reader) -- same measurement, graded independently under its own uid");
        checks.record(
            "opus:E-extdisk-iface.c13, write-protect half: twin of sol:E-disk.c35's record -- same reasons, "
            "recorded independently under its own uid");
    }

    // ========================================================================
    // RESET-NO-WRITE -- package-internal, no classify.uid (same convention
    // as FIXTURE-LIVE, which also carries no manual citation of its own).
    // The one consequence of the reset that p.334 pin 10 and p.244 do fix --
    // "Amiga system reset. Drives should reset their motor-on flip-flops..."
    // and "All drive motors turn off after system reset." -- is that a
    // drive does not write while the reset is held. Grades that
    // consequence directly: with a disk write DMA armed on the selected
    // drive just before the reset, the DMA request (dmal) and the drive
    // light (fdd_led) read off on every one of the 16 reset clocks.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("RESET-NO-WRITE");
        r.select_with_motor(0, true);
        r.mount(0, 1000);
        r.arm_write_dma();
        bool dmal_before = r.d->dmal;
        bool led_before = r.d->fdd_led;
        bool both_off_throughout = true;
        // MUTATION: no-reset -- `reset` is left at 0 for the same 16
        // clocks instead of held at 1, so the armed write runs through
        // them with nothing to stop it; the observation loop below still
        // runs, so it still gets a chance to see dmal/fdd_led live.
        if (!mut) r.d->reset = 1;
        for (int i = 0; i < 16; i++) {
            r.tick();
            if (r.d->dmal || r.d->fdd_led) both_off_throughout = false;
        }
        if (!mut) r.d->reset = 0;
        checks.note("RESET-NO-WRITE: dmal=" + std::to_string(dmal_before) + ", fdd_led=" +
                    std::to_string(led_before) + " before " + std::string(mut ? "the omitted reset" : "reset") +
                    "; both off on all 16 " + std::string(mut ? "unreset" : "reset") + " clocks=" +
                    std::to_string(both_off_throughout));
        checks.expect(dmal_before && led_before && both_off_throughout,
            "RESET-NO-WRITE (p.334, pin 10 DRESB*: 'Amiga system reset. Drives should reset their motor-on "
            "flip-flops...'; p.244: 'All drive motors turn off after system reset.'): a disk write DMA armed on "
            "a selected drive, live just before the reset, then the reset held for 16 clocks -- the DMA request "
            "and the drive light read off on every one of those clocks");
    }

    // -- drives 1, 2 and 3: the same reset duty, replayed on the connector's
    //    own drives (p.334 describes the external connector; drive 0 is not
    //    on it). Not graded under a new uid -- confirmation of the two
    //    measurements above, each already graded on drive 0. A fresh Rig
    //    per drive, per this file's own rule against reusing one across
    //    trials.
    for (int n = 1; n <= 3; n++) {
        Rig r(3);  // floppy_drives=3, all four select lines live
        r.select_with_motor(n, true);
        bool motor_on_before = r.peek_index(n);
        r.pulse_reset();
        bool motor_on_after = r.peek_index(n);
        checks.note("RESET-DUTY drive " + std::to_string(n) + ": motor latch before reset=" +
                    std::to_string(motor_on_before) + ", after=" + std::to_string(motor_on_after));
    }
    for (int n = 1; n <= 3; n++) {
        Rig r(3);
        r.select_with_motor(n, true);
        r.mount(n, 1000);
        r.arm_write_dma();
        bool dmal_before = r.d->dmal;
        bool led_before = r.d->fdd_led;
        bool both_off_throughout = true;
        r.d->reset = 1;
        for (int i = 0; i < 16; i++) {
            r.tick();
            if (r.d->dmal || r.d->fdd_led) both_off_throughout = false;
        }
        r.d->reset = 0;
        checks.note("RESET-NO-WRITE drive " + std::to_string(n) + ": dmal=" + std::to_string(dmal_before) +
                    ", fdd_led=" + std::to_string(led_before) + " before reset; both off on all 16 reset "
                    "clocks=" + std::to_string(both_off_throughout));
    }

    // ========================================================================
    // CHANGE-SET-AT-POWERUP -- sol:E-disk.c36 / opus:E-extdisk-iface.c14
    // (p.334 pin 11 CHNG*, I/O, figure-verified: "Note: Nominally used as an "
    // "open collector input. Drive's change flop is set at power up or when "
    // "no disk is not installed. Flop is reset when drive is selected and "
    // "the head stepped, but only if a disk is installed."). Two of the
    // three halves (change-on-remove, change-latch) are graded by
    // hrm/ch8-drive-sense-lines from p.244 and are not re-tested here; this
    // package's uids grade the "set at power up" half, which no accepted
    // package covers.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("sol:E-disk.c36");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(/*direc_outward=*/false);  // establish a stable baseline (disk present + qualifying step)
        bool stable_before_reset = !r.change();
        if (!mut) r.pulse_reset();
        // MUTATION: skip the reset pulse entirely -- the "set at power up"
        // half is never exercised, and the stable baseline simply persists.
        bool changed_after_reset = r.change();
        checks.note("sol:E-disk.c36: stable baseline before reset (not changed)=" +
                    std::to_string(stable_before_reset) + ", " +
                    std::string(mut ? "NO reset issued (mutated)" : "after reset") + " -> _change asserted=" +
                    std::to_string(changed_after_reset));
        checks.expect(stable_before_reset && changed_after_reset,
            "sol:E-disk.c36 (p.334, figure, pin 11 CHNG*, I/O: drive's change flop is set at power up): the "
            "already-graded halves (change-on-remove, change-latch-until-step -- "
            "hrm/ch8-drive-sense-lines P-CHANGE-ON-REMOVE/P-CHANGE-LATCH, p.244) are not re-tested by this uid. "
            "NEW here: from a genuinely stable baseline (mount+select+step), a reset alone returns _change to its "
            "'changed' state, with the disk still installed");
    }
    {
        Rig r;
        bool mut = mutating("opus:E-extdisk-iface.c14");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false);
        bool stable_before_reset = !r.change();
        if (!mut) r.pulse_reset();
        bool changed_after_reset = r.change();
        checks.note("opus:E-extdisk-iface.c14: twin of sol:E-disk.c36, same measurement -> stable before=" +
                    std::to_string(stable_before_reset) + ", after " +
                    std::string(mut ? "NO reset (mutated)" : "reset") + "=" + std::to_string(changed_after_reset));
        checks.expect(stable_before_reset && changed_after_reset,
            "opus:E-extdisk-iface.c14 (p.334, figure, pin 11 CHNG*, I/O: 'Drive's change flop is set at power up "
            "or when no disk is installed.'): twin of sol:E-disk.c36 (same printed pin, independent reader) -- "
            "same measurement, graded independently under its own uid");
    }

    // ========================================================================
    // SIDE-POLARITY -- sol:E-disk.c38 / opus:E-extdisk-iface.c16 (p.334 pin
    // 13 SIDEB*, O, figure-verified: "Side 1 if active, side 0 if
    // inactive"). The CIA-side binding of DSKSIDE is already graded
    // (hrm/ch8-cia-disk-signal-bindings); NEW here is the side selection AT
    // THE DRIVE MODEL, measured through the sector request an armed disk
    // DMA read puts on sdc_sector.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("sol:E-disk.c38");
        r.d->side = 0;
        r.select(0);
        r.arm_dma_read();
        uint32_t sector_side0 = r.d->sdc_sector;
        Rig r2;
        r2.d->side = mut ? 0 : 1;  // MUTATION: hold side at the SAME level as the first trial instead of
                                    // flipping it, so the two reads cannot be told apart.
        r2.select(0);
        r2.arm_dma_read();
        uint32_t sector_side1 = r2.d->sdc_sector;
        checks.note("sol:E-disk.c38: side=0 -> sdc_sector=" + std::to_string(sector_side0) + "; side=" +
                    std::string(mut ? "0(mutated)" : "1") + " -> sdc_sector=" + std::to_string(sector_side1));
        checks.expect(sector_side0 != sector_side1,
            "sol:E-disk.c38 (p.334, figure, pin 13 SIDEB*, O: 'Side 1 if active, side 0 if inactive.'): with "
            "a freshly reset, unstepped drive, driving `side` to its two levels before arming a disk DMA read "
            "produces two DIFFERENT sector requests on `sdc_sector` -- the line selects between the disk's two "
            "sides; the MEASURED line shows which level selects which");
    }
    {
        Rig r;
        bool mut = mutating("opus:E-extdisk-iface.c16");
        r.d->side = 0;
        r.select(0);
        r.arm_dma_read();
        uint32_t sector_side0 = r.d->sdc_sector;
        Rig r2;
        r2.d->side = mut ? 0 : 1;
        r2.select(0);
        r2.arm_dma_read();
        uint32_t sector_side1 = r2.d->sdc_sector;
        checks.note("opus:E-extdisk-iface.c16: twin of sol:E-disk.c38, same measurement -> sector0=" +
                    std::to_string(sector_side0) + ", sector1=" + std::to_string(sector_side1));
        checks.expect(sector_side0 != sector_side1,
            "opus:E-extdisk-iface.c16 (p.334, figure, pin 13 SIDEB*, O: 'Side 1 if active, side 0 if inactive.'): "
            "twin of sol:E-disk.c38 (same printed pin, independent reader) -- same measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // WPRO-SELECT-GATE -- sol:E-disk.c39 / opus:E-extdisk-iface.c17 (p.334
    // pin 14 WPRO*, I/O, figure-verified: "Asserted by selected,
    // write-protected disk."). The bare polarity is already graded
    // (hrm/ch8-drive-sense-lines P-WPROT, p.244); NEW here is the SELECTED
    // qualifier p.334 adds, which p.244's own wording does not carry.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("sol:E-disk.c39");
        r.d->floppy_wrprot = 1;  // write-protect requested
        r.deselect_all();
        bool wprot_deselected = r.wprot();
        // MUTATION: never actually select -- the "asserted when selected"
        // half is never exercised, and the deselected reading is compared
        // against itself.
        if (!mut) r.select(0);
        bool wprot_maybe_selected = r.wprot();
        checks.note("sol:E-disk.c39: floppy_wrprot=1, deselected -> _wprot asserted=" +
                    std::to_string(wprot_deselected) + ", " +
                    std::string(mut ? "still deselected (mutated)" : "selected") + " -> _wprot asserted=" +
                    std::to_string(wprot_maybe_selected));
        checks.expect(!wprot_deselected && wprot_maybe_selected,
            "sol:E-disk.c39 (p.334, figure, pin 14 WPRO*, I/O: 'Asserted by selected, write-protected disk.'): "
            "with a write-protected disk requested throughout, _wprot reads deasserted while no drive is "
            "selected and asserted once the drive is selected -- the SELECTED qualifier this page adds over "
            "p.244's bare polarity (hrm/ch8-drive-sense-lines P-WPROT grades that polarity; this uid does not "
            "re-test it)");
    }
    {
        Rig r;
        bool mut = mutating("opus:E-extdisk-iface.c17");
        r.d->floppy_wrprot = 1;
        r.deselect_all();
        bool wprot_deselected = r.wprot();
        if (!mut) r.select(0);
        bool wprot_maybe_selected = r.wprot();
        checks.note("opus:E-extdisk-iface.c17: twin of sol:E-disk.c39, same measurement -> deselected=" +
                    std::to_string(wprot_deselected) + ", selected=" + std::to_string(wprot_maybe_selected));
        checks.expect(!wprot_deselected && wprot_maybe_selected,
            "opus:E-extdisk-iface.c17 (p.334, figure, pin 14 WPRO*, I/O: 'Asserted by a selected, "
            "write-protected disk.'): twin of sol:E-disk.c39 (same printed pin, independent reader) -- same "
            "measurement, graded independently under its own uid");
    }

    // ========================================================================
    // Already-graded track-zero and step mechanisms, p.334, DECLINED as
    // re-grades -- confirmed directly below before being declined.
    // ========================================================================

    // -- pin 15 TK0*, I/O: track-zero sense + deselect masking.
    //    sol:E-disk.c40 / opus:E-extdisk-iface.c18.
    {
        Rig r;
        r.select(0);
        bool asserted_selected_at_zero = r.track0();
        r.deselect_all();
        bool masked_when_deselected = !r.track0();
        checks.note("E-disk.c40/E-extdisk-iface.c18: own confirmation -- selected at track0 -> asserted=" +
                    std::to_string(asserted_selected_at_zero) + "; deselected at track0 -> masked(deasserted)=" +
                    std::to_string(masked_when_deselected));
        checks.record(
            "sol:E-disk.c40 (p.334, figure, pin 15 TK0*, I/O: 'Asserted by selected drive when read/write head "
            "is positioned over track 0.'): the track-zero sense and its selection gating "
            "(confirmed above) are already graded -- hrm/ch8-drive-sense-lines' "
            "P-TRACK0-ZERO and SEL-MASKS-TRACK0 grade exactly this at `paula_floppy` from p.244. Decline as a "
            "re-grade. The DB23 pin ordinal has no counterpart at the fixture's boundary.");
        checks.record("opus:E-extdisk-iface.c18 (p.334, figure, pin 15 TK0*, I/O): twin of sol:E-disk.c40 -- same "
                       "reasons, declined independently under its own uid.");
    }

    // -- pins 18/19 STEPB*/DIRB, OC: step-one-cylinder mechanism and
    //    direction-to-track-ordinal mapping.
    //    sol:E-disk.c43/c44, opus:E-extdisk-iface.c21/c22.
    {
        Rig r;
        r.select(0);
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/false);  // direc=0 (inactive): one cylinder away from track 0
        bool left_zero_on_inactive_direc = !r.track0();
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/true);   // direc=1 (active): one cylinder back
        bool returned_on_active_direc = r.track0();
        checks.note("E-disk.c43/c44 and twins: own confirmation -- one step with direc inactive leaves track0=" +
                    std::to_string(left_zero_on_inactive_direc) + "; one step with direc active returns to "
                    "track0=" + std::to_string(returned_on_active_direc));
        checks.record(
            "sol:E-disk.c43 (p.334, figure, pin 18 STEPB*, OC: 'Selected drive steps one cylinder in the "
            "direction indicated by DIRB.'): the one-cylinder-per-pulse mechanism (confirmed above) is already "
            "graded -- hrm/ch8-drive-step-timing's A3/A4/A6 grade this at "
            "`paula_floppy` and hrm/ch8-cia-disk-signal-bindings grades PB0 DSKSTEP* at its CIA port bit. "
            "Decline as a re-grade. The DB23 pin ordinal and the OC drive class have no counterpart at the "
            "fixture's boundary.");
        checks.record("opus:E-extdisk-iface.c21 (p.334, figure, pin 18 STEPB*, OC): twin of sol:E-disk.c43 -- "
                       "same reasons, declined independently under its own uid.");
        checks.record(
            "sol:E-disk.c44 (p.334, figure, pin 19 DIRB, OC: 'Direction to step the head. Inactive to step "
            "towards center of disk (higher-numbered tracks).'): the direction-to-track-ordinal mapping "
            "(confirmed above -- a step with direc inactive moves the head off track 0, toward 'higher-numbered "
            "tracks') is already graded -- hrm/ch8-drive-step-timing's A7 DIREC-PROTOCOL and "
            "hrm/ch8-cia-disk-signal-bindings' PB1 DSKDIREC join grade this. Decline as a re-grade. 'Towards "
            "center of disk' is a physical-geometry statement with no counterpart at the fixture's boundary, and "
            "the DB23 pin ordinal has none either -- neither is gradable, whatever the ordinal mapping's grading "
            "status.");
        checks.record("opus:E-extdisk-iface.c22 (p.334, figure, pin 19 DIRB, OC): twin of sol:E-disk.c44 -- same "
                       "reasons, declined independently under its own uid.");
    }

    // ========================================================================
    // DEVICE-LIMITS -- sol:E-disk.c51 / opus:E-extdisk-id.c16 (p.335: "A "
    // "maximum of 3 external devices may reside on this interface, but "
    // "specific implementations may support fewer external devices."). NEW
    // HERE (no accepted package grades the interface's device-count
    // ceiling). Two halves: the structural ceiling of 4 total select lines
    // (1 conventionally internal + 3 external, per the manual's own p.337/
    // p.351 port table) is realised when floppy_drives is configured to its
    // maximum; "specific implementations may support fewer" is realised
    // when it is configured to its minimum.
    // ========================================================================
    {
        bool mut = mutating("sol:E-disk.c51");
        Rig full(3);  // floppy_drives=3, the maximum configuration
        full.select(0); bool full0 = full.ready(); full.deselect_all();
        full.select(1); bool full1 = full.ready(); full.deselect_all();
        full.select(2); bool full2 = full.ready(); full.deselect_all();
        full.select(3); bool full3 = full.ready(); full.deselect_all();
        bool all_four_recognized = full0 && full1 && full2 && full3;

        Rig fewer(mut ? 3 : 0);  // MUTATION: leave the second trial at the SAME max config instead of
                                  // lowering it, so the "fewer" half is never exercised.
        fewer.select(0); bool few0 = fewer.ready(); fewer.deselect_all();
        fewer.select(1); bool few1 = fewer.ready(); fewer.deselect_all();
        fewer.select(2); bool few2 = fewer.ready(); fewer.deselect_all();
        fewer.select(3); bool few3 = fewer.ready(); fewer.deselect_all();
        bool only_first_recognized = few0 && !few1 && !few2 && !few3;

        checks.note("sol:E-disk.c51: floppy_drives=3 -> ready per drive0..3=" + std::to_string(full0) + "," +
                    std::to_string(full1) + "," + std::to_string(full2) + "," + std::to_string(full3) +
                    "; floppy_drives=" + std::string(mut ? "3(mutated)" : "0") + " -> ready per drive0..3=" +
                    std::to_string(few0) + "," + std::to_string(few1) + "," + std::to_string(few2) + "," +
                    std::to_string(few3));
        checks.expect(all_four_recognized && only_first_recognized,
            "sol:E-disk.c51 (p.335: 'A maximum of 3 external devices may reside on this interface, but specific "
            "implementations may support fewer external devices.'): at the maximum configuration "
            "(floppy_drives=3), all four _sel[3:0] slots independently assert _ready when selected -- the "
            "structural ceiling of 4 total drive slots, of which the manual's own port table (p.337/p.351) names "
            "one internal and three external; at the minimum configuration (floppy_drives=0), only drive 0 does "
            "and the remaining three (the external slots by that same convention) do not -- the 'specific "
            "implementations may support fewer' half");
    }
    {
        bool mut = mutating("opus:E-extdisk-id.c16");
        Rig full(3);
        full.select(0); bool full0 = full.ready(); full.deselect_all();
        full.select(1); bool full1 = full.ready(); full.deselect_all();
        full.select(2); bool full2 = full.ready(); full.deselect_all();
        full.select(3); bool full3 = full.ready(); full.deselect_all();
        bool all_four_recognized = full0 && full1 && full2 && full3;

        Rig fewer(mut ? 3 : 0);
        fewer.select(0); bool few0 = fewer.ready(); fewer.deselect_all();
        fewer.select(1); bool few1 = fewer.ready(); fewer.deselect_all();
        fewer.select(2); bool few2 = fewer.ready(); fewer.deselect_all();
        fewer.select(3); bool few3 = fewer.ready(); fewer.deselect_all();
        bool only_first_recognized = few0 && !few1 && !few2 && !few3;

        checks.note("opus:E-extdisk-id.c16: twin of sol:E-disk.c51, same measurement -> all_four_recognized=" +
                    std::to_string(all_four_recognized) + ", only_first_recognized=" +
                    std::to_string(only_first_recognized));
        checks.expect(all_four_recognized && only_first_recognized,
            "opus:E-extdisk-id.c16 (p.335: 'A maximum of 3 external devices may be on the interface, though "
            "specific implementations may support fewer.'): twin of sol:E-disk.c51 (same printed proposition, "
            "independent reader) -- same measurement, graded independently under its own uid");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION FIXTURE-LIVE stimulus red: word 2 driven identical to word 1 instead of its complement, so the two readings are indistinguishable (MUTATE=FIXTURE-LIVE).
// MUTATION sol:E-disk.c26 stimulus green: this assertion is RED at baseline (p.334 pin 1 and p.369 predict RDY* deasserted for a motor genuinely latched on with no disk; paula_floppy reads it asserted), so per this suite's rule against mutating an already-failing check, the stimulus substitutes a genuine, independently-live change (leaving the drive deselected instead of latching the motor) through the identical !ready() comparator and the same label, proving the comparator is sensitive to a real difference (MUTATE=sol:E-disk.c26).
// MUTATION opus:E-extdisk-iface.c4 stimulus green: same substitute as sol:E-disk.c26, independent uid (MUTATE=opus:E-extdisk-iface.c4).
// MUTATION FIXTURE-LIVE-DRIVE1 stimulus red: drive 1 is never actually selected, so the "asserts once selected" half is never exercised on drive 1 (MUTATE=FIXTURE-LIVE-DRIVE1).
// MUTATION P-READY-MOTOR-INVARIANT-DRIVE1 stimulus green: this assertion is RED at baseline (motor latch measured on, p.334 pin 1 predicts RDY* deasserted for drive 1's motor genuinely latched on with no disk; paula_floppy reads it asserted, same defect as sol:E-disk.c26/opus:E-extdisk-iface.c4 on drive 0), so per this suite's rule against mutating an already-failing check, the stimulus substitutes a genuine, independently-live change (leaving drive 1 deselected instead of latching the motor) through the identical !ready() comparator and the same label -- the motor-latch precondition defaults to true (not applicable) on this branch, since it deliberately never latches the motor at all and tests a different axis (14 passed, 2 failed, MUTATE=P-READY-MOTOR-INVARIANT-DRIVE1).
// sol:E-disk.c33, opus:E-extdisk-iface.c11, sol:E-disk.c34, opus:E-extdisk-iface.c12, sol:E-disk.c45, opus:E-extdisk-iface.c23, sol:E-disk.c46, opus:E-extdisk-iface.c24, sol:E-disk.c40, opus:E-extdisk-iface.c18, sol:E-disk.c43, opus:E-extdisk-iface.c21, sol:E-disk.c44, opus:E-extdisk-iface.c22 -- all DECLINED via record(), not expect(); no mutation records (not graded through expect()). sol:E-disk.c35's and opus:E-extdisk-iface.c13's write-protect halves are RECORDED via record(), not graded through expect() either -- no mutation records for them.
// MUTATION sol:E-disk.c35 stimulus red: only the motor half is graded here (the write-protect half is recorded, not graded, below it); the reset pulse is omitted entirely, so the latched-before/after comparison reads the same latched state twice (MUTATE=sol:E-disk.c35).
// MUTATION opus:E-extdisk-iface.c13 stimulus red: same omission as sol:E-disk.c35, independent uid (MUTATE=opus:E-extdisk-iface.c13).
// MUTATION RESET-NO-WRITE stimulus red: `reset` is left at 0 for the same 16 clocks instead of held at 1, so the write DMA armed just before them keeps dmal and fdd_led live through the observation loop instead of dropping them (MUTATE=RESET-NO-WRITE).
// MUTATION sol:E-disk.c36 stimulus red: the reset pulse is skipped entirely, so the "set at power up" half is never exercised and the stable baseline simply persists unchanged (MUTATE=sol:E-disk.c36).
// MUTATION opus:E-extdisk-iface.c14 stimulus red: same omission as sol:E-disk.c36, independent uid (MUTATE=opus:E-extdisk-iface.c14).
// MUTATION sol:E-disk.c38 stimulus red: the second trial's `side` is held at the SAME level as the first instead of the opposite one, so the two sdc_sector captures cannot be told apart (MUTATE=sol:E-disk.c38).
// MUTATION opus:E-extdisk-iface.c16 stimulus red: same omission as sol:E-disk.c38, independent uid (MUTATE=opus:E-extdisk-iface.c16).
// MUTATION sol:E-disk.c39 stimulus red: the drive is never actually selected, so the "asserted when selected" half is never exercised and the deselected reading is compared against itself (MUTATE=sol:E-disk.c39).
// MUTATION opus:E-extdisk-iface.c17 stimulus red: same omission as sol:E-disk.c39, independent uid (MUTATE=opus:E-extdisk-iface.c17).
// MUTATION sol:E-disk.c51 stimulus red: the second trial's floppy_drives is left at the SAME maximum configuration instead of being lowered, so the "specific implementations may support fewer" half is never exercised and all four drives are recognized in both trials (MUTATE=sol:E-disk.c51).
// MUTATION opus:E-extdisk-id.c16 stimulus red: same omission as sol:E-disk.c51, independent uid (MUTATE=opus:E-extdisk-id.c16).
