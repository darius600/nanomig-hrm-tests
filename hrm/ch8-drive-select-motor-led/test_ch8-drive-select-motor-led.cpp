// Test purpose: HRM conformance package for the printed-page claim (p.244,
//   DSKMOTOR*) that a floppy drive latches the motor line at its own select
//   edge, holds that latch until the next such edge (including while
//   deselected), and that DSKMOTOR* also drives the drive's front activity
//   light -- against paula_floppy (this suite's only model of a physical
//   Amiga floppy drive). 8 claims, from
//   work/ch8-s2-build/ch8-drive-select-motor-led.jsonl. Full derivation in
//   SPECIFICATION.md (committed as 0ae4b60, before this file or
//   paula_floppy.v was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.244 ("PB7
//   DSKMOTOR* ... Each drive will latch the motor signal at the time its
//   select signal turns on. The disk drive motor will stay in this state
//   until the next time select turns on. DSKMOTOR* also controls the
//   activity light on the front of the disk drive. ... The drive will
//   \"remember\" the state of its motor when it is not selected.").
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (0ae4b60):
//   1. $(NANOMIG)/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   2. $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v -- full file, to
//      confirm it instantiates nothing else and the two-file build closes
//      (it does; the only sub-instance in paula_floppy.v is this fifo).
//   3. hrm/paula_floppy/test_paula_floppy.cpp and its Makefile -- an
//      existing RTL_REGRESSION package against the same module (its own
//      header disclaims HRM-conformance status), read only for the
//      clk/clk7_en clocking convention, default port-init values and the
//      DSKLEN double-write idiom -- not for any conformance fact.
//   4. hrm/ch8-ciaapra-address-window/test_ch8-ciaapra-address-window.cpp --
//      full file, for this suite's Checks/mutating()/MUTATION-comment
//      boilerplate shape.
//
// MECHANISM, confirmed by reading paula_floppy.v myself (SPECIFICATION.md
//   had only the classify stage's disclosed, not-yet-verified citations):
//   - motor_on[3:0] (paula_floppy.v:176, four separate always blocks at
//     624-659) is written ONLY by `reset` (clears to 0) and by
//     `!_sel[n] && _sel_del[n]` -- the falling edge of drive n's OWN _sel
//     bit (active low; _sel_del is _sel delayed by one clk7_en tick,
//     paula_floppy.v:618-622) -- which latches `~_motor`. No other writer
//     touches any bit of motor_on. This is P1 (latch at own edge) and P2
//     (unchanged between edges of the SAME drive) at once, confirmed by
//     direct reading, not merely disclosed evidence.
//   - `index` (paula_floppy.v:576) is combinational:
//     `|(~_sel & motor_on) & ~|rpm_pulse_cnt & sof`. It can be sampled by
//     driving `_sel`+`sof` and calling eval() WITHOUT a clock edge, which
//     is how this file peeks at a drive's motor_on without ever risking a
//     spurious latch edge while reading it (`rpm_pulse_cnt`,
//     paula_floppy.v:563-573, only advances on a clk7_en edge where `sof`
//     was asserted AT that edge; Verilator zero-initialises it, so it
//     reads 0 for the whole run as long as `sof` is never asserted across
//     an actual tick(), which the peek helper below guarantees).
//   - `fdd_led` (paula_floppy.v:972) is `(dskstate!=DISKDMA_IDLE)` --
//     entirely a function of the disk-DMA state machine
//     (paula_floppy.v:994-1037, driven by dmaen/lenzero/enable); `_motor`
//     and `motor_on` do not appear anywhere in its cone. Confirms finding
//     candidate C16.
//   - Arming DMA needs the DSKLEN double write (paula_floppy.v:742-743,
//     confirmed): `dmaen <= data_in[15] & dsklen[15]` reads dsklen[15]
//     BEFORE this same-cycle write updates it (non-blocking assignment),
//     so dmaen goes high only on the SECOND consecutive write with bit 15
//     set. Once dmaen&&!lenzero&&enable, dskstate leaves DISKDMA_IDLE on
//     the next clk7_en edge; this package never feeds the SD-card
//     interface at all, because that IDLE->ACTIVE transition does not wait
//     on sdc_busy/sdc_rd being serviced.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- one build serves the real run (MUTATE unset) and every
//   mutation run (MUTATE=<id>, one id per assertion). Ids are the claim's
//   own `classify.uid` for A1-A8 (this suite's chapter-8 convention for
//   claims labelled by uid -- see tools/check_mutation_labels.py's own
//   comment on the colon-carrying id shape) and a plain uppercase id for
//   A0/CTRL-DMA, which carry no manual citation.
//   A5/A6 (opus:8-cia.c26, sol:8-cia.c24) are RED at baseline (the
//   expected finding, C16), so per this suite's rule against mutating an
//   already-failing check, their MUTATE run does not flip a stimulus red
//   -- it substitutes a DIFFERENT mechanism (disk DMA, proven live
//   elsewhere in this same run by CTRL-DMA) through the IDENTICAL
//   fdd_led==1 comparator and the SAME label, and that run is GREEN.
//
// MUTATION RUN LOG (measured -- see the report accompanying this commit for
//   the full per-assertion baseline/mutated pass-fail table):
//   Baseline (MUTATE unset): 10 checks.expect() calls, 8 passed, 2 failed
//   (opus:8-cia.c26, sol:8-cia.c24 -- the expected C16 finding). Each of
//   MUTATE=A0-FIXTURE-LIVENESS-INDEX-PATH, opus:8-cia.c24, sol:8-cia.c22,
//   opus:8-cia.c25, sol:8-cia.c23, opus:8-cia.c28, sol:8-cia.c26,
//   CTRL-DMA-DRIVES-FDD-LED was run and confirmed 7 passed / 3 failed (only
//   the targeted label's line changed from PASS to FAIL among the 8
//   baseline-green checks, the 2 baseline-red ones unaffected), then
//   reverted and confirmed back to 8/2. MUTATE=opus:8-cia.c26 and
//   MUTATE=sol:8-cia.c24 were each run and confirmed 9 passed / 1 failed
//   (only that one label flipped from FAIL to PASS, nothing else moved),
//   then reverted and confirmed back to 8/2.

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

// RIG HAZARD (SPECIFICATION.md, build brief): dsktrack (paula_floppy.v:671-
// 680) has no reset term, so a Verilated model reused across trials would
// carry stale head position forward. Rig allocates a FRESH Vpaula_floppy
// per instance; every test block below constructs its own Rig and none is
// reused for a second trial.
struct Rig {
    Vpaula_floppy* d;
    int clk_cycle = 0;

    Rig() {
        d = new Vpaula_floppy;
        d->clk = 0; d->clk7_en = 0; d->clk7n_en = 0;
        d->reset = 1; d->ntsc = 0; d->sof = 0; d->enable = 0;
        d->reg_address_in = 0; d->data_in = 0;
        d->_step = 1; d->direc = 1; d->_sel = 0xF; d->side = 0; d->_motor = 1;
        d->wordsync = 0;
        d->floppy_drives = 3;  // 4 drives configured; irrelevant to every port this file grades
        d->floppy_wrprot = 0;
        d->sdc_image_mounted = 0; d->sdc_image_size = 0;
        d->sdc_busy = 0; d->sdc_done = 0;
        d->sdc_byte_in_strobe = 0; d->sdc_byte_addr = 0; d->sdc_byte_in_data = 0;
        d->eval();
    }
    ~Rig() { delete d; }

    void tick() {
        d->eval();
        d->clk = 1; d->eval();
        d->clk = 0; d->eval();
        clk_cycle = (clk_cycle + 1) % 4;
        d->clk7_en  = (clk_cycle == 0);
        d->clk7n_en = (clk_cycle == 2);
    }
    void settle(int n = 8) { for (int i = 0; i < n; i++) tick(); }

    void do_reset() {
        clk_cycle = 0;
        d->clk = 0; d->clk7_en = 0; d->clk7n_en = 0;
        d->reset = 1; settle(16);
        d->reset = 0; settle(16);
    }

    // Deselect every drive, then select ONLY drive n with _motor driven to
    // the requested level, held across a full clk7_en period so the
    // turn-on edge this produces genuinely latches motor_on[n]. Always
    // starts from a fully-deselected state so the edge is real, not
    // assumed.
    void select_with_motor(int n, bool motor_on_level) {
        d->_sel = 0xF;
        settle(8);
        d->_motor = motor_on_level ? 0 : 1;   // DSKMOTOR* is active low
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        settle(8);
    }
    void deselect_all() { d->_sel = 0xF; settle(8); }

    // Read drive n's latched motor_on through `index`, with NO clock edge:
    // eval() alone drives the combinational `index` output, so
    // motor_on/_sel_del/rpm_pulse_cnt are all untouched by the act of
    // reading.
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

    void write_reg(int addr, uint16_t data) {
        while (!d->clk7_en) tick();
        d->reg_address_in = addr >> 1;
        d->data_in = data;
        tick(); tick(); tick(); tick();
        d->reg_address_in = 0;
        d->data_in = 0;
    }

    // Arm disk DMA read (DSKLEN double write, paula_floppy.v:742-743) and
    // run long enough for dskstate to leave DISKDMA_IDLE. No SD-card
    // interface signal is driven -- that transition does not wait on
    // sdc_busy/sdc_rd being serviced.
    void arm_dma() {
        d->enable = 1;
        write_reg(0x024, 0x800A);   // DSKLEN: length 10, bit15 set (1st write)
        write_reg(0x024, 0x800A);   // 2nd write -> dmaen latches 1
        settle(40);
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS for the motor_on/index half of this
    // fixture. No claim id, no manual page: the build brief's own
    // requirement that the fixture prove it can deliver a known, distinct
    // word before any read-based assertion (A1-A4, A7-A8) is trusted.
    // ========================================================================
    {
        Rig r;
        r.do_reset();
        r.select_with_motor(0, true);   // word 1: motor ON
        bool v_on = r.peek_index(0);
        // MUTATION: drive word 2 IDENTICAL to word 1 instead of its
        // complement, so the two readings cannot be told apart.
        bool word2_on = mutating("A0-FIXTURE-LIVENESS-INDEX-PATH") ? true : false;
        r.select_with_motor(0, word2_on);
        bool v2 = r.peek_index(0);
        checks.note("A0: drive0 motor_on via index -- word1(ON)=" + std::to_string(v_on) +
                    ", word2(" + std::string(word2_on ? "ON,mutated" : "OFF") + ")=" + std::to_string(v2));
        checks.expect(v_on == true && v2 == false && v_on != v2,
            "A0-FIXTURE-LIVENESS-INDEX-PATH: two distinct, known motor states (ON then OFF), each latched onto "
            "drive0 via its own genuine _sel[0] turn-on edge, arrive unmodified and mutually distinguishable when "
            "read back through `index` -- the motor_on/index half of this fixture's data path is live, not stuck");
    }

    // ========================================================================
    // A1 -- opus:8-cia.c24 (p.244). A2 -- sol:8-cia.c22 (twin of A1, p.244).
    // P1: each drive samples _motor only at its OWN select turn-on edge.
    // ========================================================================
    {
        Rig r;
        r.do_reset();
        bool mut = mutating("opus:8-cia.c24");
        r.select_with_motor(0, false);              // drive0 latches OFF at its own edge
        // MUTATION: skip changing the motor level before drive1's edge, so
        // both drives would latch the SAME value and the distinctness this
        // assertion grades disappears.
        r.select_with_motor(1, mut ? false : true);  // drive1 latches (real: ON) / (mutated: also OFF)
        bool d0 = r.peek_index(0);
        bool d1 = r.peek_index(1);
        checks.note("A1/opus:8-cia.c24: drive0 latched with motor OFF -> index=" + std::to_string(d0) +
                    "; drive1 latched with motor " + std::string(mut ? "OFF(mutated)" : "ON") +
                    " -> index=" + std::to_string(d1));
        checks.expect(d0 == false && d1 == true,
            "opus:8-cia.c24 (p.244, quote 'Each drive will latch the motor signal at the time its select signal "
            "turns on.'): drive0's latch reflects _motor as driven at drive0's OWN _sel[0] turn-on edge, and "
            "drive1's latch independently reflects the DIFFERENT _motor level driven later at drive1's own "
            "_sel[1] turn-on edge -- each drive samples the motor line only at its own edge");
    }
    {
        Rig r;
        r.do_reset();
        bool mut = mutating("sol:8-cia.c22");
        r.select_with_motor(0, false);
        r.select_with_motor(1, mut ? false : true);
        bool d0 = r.peek_index(0);
        bool d1 = r.peek_index(1);
        checks.note("A2/sol:8-cia.c22: twin of A1, same measurement -> index0=" + std::to_string(d0) +
                    " index1=" + std::to_string(d1));
        checks.expect(d0 == false && d1 == true,
            "sol:8-cia.c22 (p.244, quote 'Each drive will latch the motor signal at the time its select signal "
            "turns on.'): twin of opus:8-cia.c24 (same printed quote, independent reader) -- same measurement, "
            "graded independently under its own uid");
    }

    // ========================================================================
    // A3 -- opus:8-cia.c25 (p.244). A4 -- sol:8-cia.c23 (twin of A3, p.244).
    // P2: between two turn-on edges of the SAME drive, the latch cannot
    // change -- proven by toggling _motor freely while deselected.
    // ========================================================================
    {
        Rig r;
        r.do_reset();
        bool mut = mutating("opus:8-cia.c25");
        r.select_with_motor(0, true);      // drive0 latches ON
        r.deselect_all();
        for (int i = 0; i < 6; i++) {
            r.d->_motor = (i % 2) ? 0 : 1; // toggle motor while deselected -- must not touch the latch
            r.settle(8);
        }
        if (mut) {
            // MUTATION: give drive0 a genuine SECOND edge with the OPPOSITE
            // motor level before the read -- exactly the event P2 says is
            // required to change the latch.
            r.select_with_motor(0, false);
        }
        bool v = r.peek_index(0);
        checks.note("A3/opus:8-cia.c25: drive0 latched ON, deselected, motor toggled 6x with no new edge" +
                    std::string(mut ? " THEN a mutated 2nd edge with motor OFF" : "") +
                    " -> index=" + std::to_string(v));
        checks.expect(v == true,
            "opus:8-cia.c25 (p.244, quote 'The disk drive motor will stay in this state until the next time "
            "select turns on.'): between two turn-on edges of drive0's own _sel[0], the latched motor state does "
            "not change no matter how _motor is driven while deselected");
    }
    {
        Rig r;
        r.do_reset();
        bool mut = mutating("sol:8-cia.c23");
        r.select_with_motor(0, true);
        r.deselect_all();
        for (int i = 0; i < 6; i++) {
            r.d->_motor = (i % 2) ? 0 : 1;
            r.settle(8);
        }
        if (mut) r.select_with_motor(0, false);
        bool v = r.peek_index(0);
        checks.note("A4/sol:8-cia.c23: twin of A3, same measurement -> index=" + std::to_string(v));
        checks.expect(v == true,
            "sol:8-cia.c23 (p.244, quote 'The disk drive motor will stay in this state until the next time "
            "select turns on.'): twin of opus:8-cia.c25 (same printed quote, independent reader) -- same "
            "measurement, graded independently under its own uid");
    }

    // ========================================================================
    // A7 -- opus:8-cia.c28 (p.244). A8 -- sol:8-cia.c26 (twin of A7, p.244).
    // P4: an unselected drive remembers its latched motor state -- proven
    // through genuine selection and operation of a DIFFERENT drive in
    // between, not just idle time (distinct from A3/A4's demonstration).
    // ========================================================================
    {
        Rig r;
        r.do_reset();
        bool mut = mutating("opus:8-cia.c28");
        r.select_with_motor(0, true);       // drive0 latches ON
        r.deselect_all();
        // Real activity on a DIFFERENT drive while drive0 stays unselected.
        r.select_with_motor(1, false);
        r.deselect_all();
        if (mut) {
            // MUTATION: reselect drive0 itself -- a genuine second edge for
            // drive0 -- which this proposition says would overwrite what it
            // remembers.
            r.select_with_motor(0, false);
        }
        bool v0 = r.peek_index(0);
        checks.note("A7/opus:8-cia.c28: drive0 latched ON then deselected; drive1 selected+latched OFF and "
                    "deselected in between" +
                    std::string(mut ? "; THEN drive0 mutated with a 2nd edge, motor OFF" : "") +
                    " -> drive0 index=" + std::to_string(v0));
        checks.expect(v0 == true,
            "opus:8-cia.c28 (p.244, quote 'The drive will \"remember\" the state of its motor when it is not "
            "selected.'): drive0's latched motor state survives genuine selection and operation of a DIFFERENT "
            "drive in between, unchanged while drive0 itself is not reselected");
    }
    {
        Rig r;
        r.do_reset();
        bool mut = mutating("sol:8-cia.c26");
        r.select_with_motor(0, true);
        r.deselect_all();
        r.select_with_motor(1, false);
        r.deselect_all();
        if (mut) r.select_with_motor(0, false);
        bool v0 = r.peek_index(0);
        checks.note("A8/sol:8-cia.c26: twin of A7, same measurement -> drive0 index=" + std::to_string(v0));
        checks.expect(v0 == true,
            "sol:8-cia.c26 (p.244, quote 'The drive will \"remember\" the state of its motor when it is not "
            "selected.'): twin of opus:8-cia.c28 (same printed quote, independent reader) -- same measurement, "
            "graded independently under its own uid");
    }

    // ========================================================================
    // A5 -- opus:8-cia.c26 (p.244). A6 -- sol:8-cia.c24 (twin of A5, p.244).
    // P3, this package's headline (finding candidate C16): recovered by the
    // attack stage after classification discarded both as not-hardware.
    // Prescribed fixture: disk DMA idle, drive _sel/_motor through both
    // motor states, read fdd_led. Baseline is RED (expected).
    // ========================================================================
    {
        Rig r;
        r.do_reset();
        bool use_dma = mutating("opus:8-cia.c26");
        bool led_off_state, led_on_state;
        if (!use_dma) {
            r.select_with_motor(0, false);
            led_off_state = r.d->fdd_led;
            r.select_with_motor(0, true);
            led_on_state = r.d->fdd_led;
        } else {
            // MUTATION (substitute, per the rule against mutating an
            // already-red check): the stimulus swaps in disk DMA -- proven
            // live elsewhere in this same run by CTRL-DMA -- through the
            // IDENTICAL fdd_led==1 comparator and the same label.
            r.d->_sel = (uint8_t)(0xF & ~1u);
            r.settle(8);
            led_off_state = r.d->fdd_led;   // before DMA is armed
            r.arm_dma();
            led_on_state = r.d->fdd_led;    // after DMA is armed
        }
        checks.note(std::string("A5/opus:8-cia.c26: fdd_led with motor OFF (DMA idle)=") +
                    std::to_string(led_off_state) + ", fdd_led with " +
                    (use_dma ? "disk DMA active (substitute mechanism)=" : "motor ON (DMA idle)=") +
                    std::to_string(led_on_state));
        checks.expect(led_on_state == true,
            "opus:8-cia.c26 (p.244, quote 'DSKMOTOR* also controls the activity light on the front of the disk "
            "drive.', recovered by the attack stage after classification discarded it as not-hardware): with "
            "disk DMA idle, `fdd_led` follows the selected drive's motor into the ON state -- paula_floppy.v:972 "
            "drives fdd_led purely from dskstate!=DISKDMA_IDLE, so this is expected to fail against the RTL "
            "(finding candidate C16)");
    }
    {
        Rig r;
        r.do_reset();
        bool use_dma = mutating("sol:8-cia.c24");
        bool led_off_state, led_on_state;
        if (!use_dma) {
            r.select_with_motor(0, false);
            led_off_state = r.d->fdd_led;
            r.select_with_motor(0, true);
            led_on_state = r.d->fdd_led;
        } else {
            r.d->_sel = (uint8_t)(0xF & ~1u);
            r.settle(8);
            led_off_state = r.d->fdd_led;
            r.arm_dma();
            led_on_state = r.d->fdd_led;
        }
        checks.note(std::string("A6/sol:8-cia.c24: twin of A5, same measurement -> fdd_led off-state=") +
                    std::to_string(led_off_state) + ", on-state=" + std::to_string(led_on_state));
        checks.expect(led_on_state == true,
            "sol:8-cia.c24 (p.244, quote 'DSKMOTOR* also controls the activity light on the front of the disk "
            "drive.', recovered by the attack stage after classification discarded it as not-hardware): twin of "
            "opus:8-cia.c26 (same printed quote, independent reader) -- same measurement, graded independently "
            "under its own uid");
    }

    // ========================================================================
    // CTRL-DMA -- control case, same fdd_led readout as A5/A6: the attack's
    // own prescribed second half. Not optional -- without it A5/A6's red
    // does not survive the objection that fdd_led is simply dead.
    // ========================================================================
    {
        Rig r;
        r.do_reset();
        r.d->_sel = 0xF;   // no drive selected at all -- isolates DMA as the sole cause
        r.settle(8);
        bool led_before = r.d->fdd_led;
        bool mut = mutating("CTRL-DMA-DRIVES-FDD-LED");
        if (!mut) {
            r.arm_dma();
        } else {
            // MUTATION: never complete the double write -- DMA never arms,
            // dskstate never leaves DISKDMA_IDLE.
            r.d->enable = 1;
            r.write_reg(0x024, 0x800A);   // single write only
            r.settle(40);
        }
        bool led_after = r.d->fdd_led;
        checks.note("CTRL-DMA: fdd_led before=" + std::to_string(led_before) + ", after " +
                    std::string(mut ? "a single DSKLEN write (mutated)" : "the DSKLEN double write") +
                    "=" + std::to_string(led_after));
        checks.expect(led_before == false && led_after == true,
            "CTRL-DMA-DRIVES-FDD-LED (control case, no manual citation, same fdd_led readout as A5/A6 -- the "
            "attack's own prescribed second half): with no drive selected at all, arming disk DMA is what "
            "actually moves fdd_led from 0 to 1 -- proving the port is live and naming DMA, not the motor, as "
            "its real cause");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION A0-FIXTURE-LIVENESS-INDEX-PATH stimulus red: word 2 driven identical to word 1 instead of its complement, so the two readings are indistinguishable (MUTATE=A0-FIXTURE-LIVENESS-INDEX-PATH).
// MUTATION opus:8-cia.c24 stimulus red: drive1's edge redirected to latch the SAME motor level as drive0 instead of the opposite one, so the distinctness this assertion grades disappears (MUTATE=opus:8-cia.c24).
// MUTATION sol:8-cia.c22 stimulus red: same redirection as opus:8-cia.c24, independent uid (MUTATE=sol:8-cia.c22).
// MUTATION opus:8-cia.c25 stimulus red: a genuine second _sel[0] turn-on edge with the opposite motor level is injected before the read, which this proposition says must change the latch (MUTATE=opus:8-cia.c25).
// MUTATION sol:8-cia.c23 stimulus red: same injected edge as opus:8-cia.c25, independent uid (MUTATE=sol:8-cia.c23).
// MUTATION opus:8-cia.c28 stimulus red: drive0 is reselected with the opposite motor level after the other-drive activity, overwriting what it "remembers" (MUTATE=opus:8-cia.c28).
// MUTATION sol:8-cia.c26 stimulus red: same reselection as opus:8-cia.c28, independent uid (MUTATE=sol:8-cia.c26).
// MUTATION opus:8-cia.c26 stimulus green: this assertion is RED at baseline (C16), so per the rule against mutating an already-failing check, the stimulus substitutes disk DMA -- proven live elsewhere in this run by CTRL-DMA -- through the identical fdd_led==1 comparator and the same label in place of the motor (MUTATE=opus:8-cia.c26).
// MUTATION sol:8-cia.c24 stimulus green: same substitute mechanism as opus:8-cia.c26, independent uid (MUTATE=sol:8-cia.c24).
// MUTATION CTRL-DMA-DRIVES-FDD-LED stimulus red: only a single DSKLEN write is issued instead of the double write DMAEN needs, so dmaen never latches and dskstate never leaves DISKDMA_IDLE (MUTATE=CTRL-DMA-DRIVES-FDD-LED).
