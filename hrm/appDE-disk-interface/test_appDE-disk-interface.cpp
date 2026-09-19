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
//   used below was read from figures/p334.png directly (reproduced here,
//   confirmed against the render before this file was written) rather than
//   from p334.txt. p.335 carries no substantive binding displacement
//   (corpus-defects.md's per-page table) and is quoted from p335.txt.
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
// THIS PACKAGE'S OWNED CONDITION (build brief, "YOU OWN A DECLINE WHOSE
//   PREMISE HAS EXPIRED"): hrm/ch8-drive-sense-lines declined its P-READY
//   proposition (opus:8-cia.c7) because "p.244 declines to say what _ready
//   means with the motor off." p.334 pin 1 (RDY*) states both branches p.244
//   was silent on: "If motor on, indicates disk installed and up to speed.
//   If motor not on, identification mode." That prior decline's premise does
//   not carry to a claim built on p.334, so sol:E-disk.c26 and
//   opus:E-extdisk-iface.c4 (below) are GRADED, not declined against it.
//   MEASURED, not adjudicated: paula_floppy.v:688-694 (quoted in full in the
//   MECHANISM section) derives _ready from _sel[3:0] and the configured
//   drive count alone -- no _motor term anywhere in its cone -- so neither
//   printed branch is distinguishable there. Whether P-READY should be
//   re-opened is the build stage's call and is not decided by this file;
//   what this file does is measure and grade the p.334 claim on its own
//   terms. The deeper 32-bit serial identification PROTOCOL p.335 describes
//   (shift register, bit counter, motor-transition trigger) is carried by a
//   sibling package's claims (sol:E-disk-id.c1 / opus:E-extdisk-id.c2, per
//   classify.note on E-disk.c26) -- NOT by this package's 26, which hold
//   only the RDY* pin-level claims (below) and the device-count claims
//   (sol:E-disk.c51 / opus:E-extdisk-id.c16, DEVICE-LIMITS below).
//
// RTL READ, IN ORDER, BEFORE ANY EXPECT/RECORD BELOW WAS WRITTEN:
//   1. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   2. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file, to confirm
//      it instantiates nothing else and the two-file build closes (it does;
//      the only sub-instance in paula_floppy.v is this fifo, `db1`).
//   3. nanomig/src/minimig-aga/minimig.v:376-387,489-548,607-651 -- the CIA
//      join, to confirm (per the build brief's "different axis" instruction)
//      that no 23-pin connector or pin-numbered boundary exists anywhere in
//      the pinned tree: the disk lines are named single-bit wires, and
//      hrm/ch8-cia-disk-signal-bindings, hrm/ch8-drive-select-motor-led and
//      hrm/ch8-drive-sense-lines all grade this same RTL at that CIA-bit
//      join, never at a connector pin ordinal.
//   4. hrm/ch8-drive-sense-lines/test_ch8-drive-sense-lines.cpp -- full
//      file, for this suite's Checks/mutating()/Rig/MUTATION-comment
//      boilerplate shape, and for its own confirmation of the _ready,
//      _track0, _wprot and _change equations (re-verified independently
//      against paula_floppy.v here, not trusted from that file).
//   5. hrm/ch8-drive-select-motor-led/test_ch8-drive-select-motor-led.cpp --
//      full file, for the select_with_motor()/peek_index() idiom (reading a
//      latched motor_on[n] through the combinational `index` output with no
//      clock edge) and the DSKLEN double-write/arm-DMA idiom, both reused
//      below adapted to this file's own clk7_en-held-high Rig.
//   6. hrm/ch8-cia-disk-signal-bindings/test_minimig.cpp -- read for its
//      confirmation of the minimig.v join (no connector, no pin ordinal)
//      and of exactly which pins/bits it and its siblings already grade.
//
// MECHANISM, confirmed by reading paula_floppy.v myself (line numbers below
//   are this package's own verification, not copied from another file):
//
//   Port list (paula_floppy.v:83-93): inputs _step, direc, _sel[3:0], side,
//   _motor; outputs _track0, _change, _ready, _wprot, index. No connector,
//   no pin number, anywhere in this module or in minimig.v's instantiation
//   of it (minimig.v:376-387,517-525) -- confirmed directly, matching the
//   build brief's "different axis" instruction: every DB23 pin ordinal a
//   p.334 claim below cites has no RTL counterpart and is recorded as such.
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
//   reset. With floppy_drives=0 (1 drive configured), every term but the
//   last collapses to a constant 1, so _ready reduces to exactly _sel[0].
//   With floppy_drives=N (0-3), _ready reduces to the OR (via De Morgan,
//   since _sel is active-low and the whole expression is AND-of-ORs) of
//   _sel[0..N] being selected -- i.e. exactly N+1 of the four select lines
//   are recognized; MEASURED directly in DEVICE-LIMITS below, not merely
//   derived algebraically. `_motor` does not appear anywhere in this
//   expression, and does not appear anywhere in `motor_on`'s cone either.
//   The module's own comment, quoted above, says so in its own words:
//   "not implemented - _ready is active when _sel is active". MEASURED in
//   P-READY-MOTOR-INVARIANT below: toggling _motor while holding _sel fixed
//   changes nothing.
//
//   drive motor-on latch (paula_floppy.v:618-622,624-659), one of four
//   identical blocks quoted for drive 0:
//     always @(posedge clk) begin if (clk7_en) begin
//       if (reset) motor_on[0] <= 0;
//       else if (!_sel[0] && _sel_del[0]) motor_on[0] <= ~_motor;
//     end end
//   `_sel_del <= _sel` every clk7_en tick, no reset term of its own. The
//   RESET clause (`motor_on[n]<=0`) is the "reset their motor-on flip-flops"
//   half of p.334 pin 10's (DRESB*) and pin 11's (CHNG*, "When below 3.75V")
//   note; MEASURED (GREEN) in RESET-DUTY below.
//
//   _wprot (paula_floppy.v:664,969):
//     assign disk_writable = floppy_wrprot ? 4'b0000 : 4'b1111;
//     assign _wprot = &(_sel | disk_writable);
//   Purely combinational: no flip-flop anywhere in its cone. The "set their
//   write-protect flip-flops" half of the SAME pin-10/pin-11 reset note
//   therefore has no flop to be set -- MEASURED (RED) in RESET-DUTY below.
//   With a drive selected, _wprot reads protected iff floppy_wrprot=1;
//   with NO drive selected (_sel=4'b1111), the reduction-AND collapses to 1
//   regardless of disk_writable, so _wprot reads unprotected -- the
//   "asserted by a SELECTED, write-protected disk" qualifier p.334 pin 14
//   adds -- MEASURED (GREEN) in WPRO-SELECT-GATE below.
//
//   _change / _disk_change (paula_floppy.v:608-612,662):
//     assign _change = &(_sel | _disk_change);
//     _disk_change <= (_disk_change | ~_sel & {4{_step}} & ~{4{_step_del}}
//                       & disk_present) & ~({4{reset}} | ~disk_present);
//   The `~({4{reset}}|...)` term forces _disk_change to 0 (CHNG* asserted,
//   "changed") whenever reset is active, independent of disk_present or any
//   prior stable state -- the "set at power up" half of p.334 pin 11's note
//   that no accepted package grades -- MEASURED (GREEN) in
//   CHANGE-SET-AT-POWERUP below.
//
//   side (paula_floppy.v:88,669): `assign track = {dsktrack[sel], ~side};`
//   -- side is inverted into the LSB of `track`, which paula_floppy.v:283
//   (`sdc_sector <= track * 11 + fd_dma_wr_sec_next;`) latches into the
//   externally-visible `sdc_sector` output the moment a disk DMA read is
//   armed. With dsktrack[sel]==0 (a freshly reset, unstepped drive),
//   side=0 gives track=1 (sdc_sector=11) and side=1 gives track=0
//   (sdc_sector=0) -- the active-low sense p.334 pin 13 states ("Side 1 if
//   active, side 0 if inactive") -- MEASURED (GREEN) in SIDE-POLARITY below.
//
//   DSKLEN/dmaen (paula_floppy.v:711-745) and the DMA-read trigger
//   (paula_floppy.v:263-288): reused from
//   hrm/ch8-drive-select-motor-led's write_reg()/arm_dma() idiom, adapted to
//   this file's clk7_en-held-high Rig (a single held write_reg() call spans
//   several clk7_en ticks, so the "two consecutive writes with dsklen[15]
//   set" dmaen requires -- paula_floppy.v:743 -- is satisfied within one
//   call: the first tick's non-blocking dsklen[15]<=1 is visible to the
//   SAME held write's later ticks).
//
// NOT GRADABLE ANYWHERE IN THIS PACKAGE (recorded via record(), never
//   graded): the DB23 pin ordinal itself (no connector, no pin number,
//   anywhere in the pinned tree -- confirmed above); the 1.4 usec MTRXD*
//   setup/hold figures and any other absolute real-time interval (this RTL
//   counts clk7_en ticks with no period attached at any boundary, the same
//   reason hrm/ch8-drive-step-timing declines absolute figures); DIRB's
//   "towards center of disk" physical geometry (no RTL boundary represents
//   physical direction, only the track-ordinal half, which is already
//   graded); the pin's printed drive-class letters (I/O, OC, O) and the
//   daisy-chain shift footnote (carried by a sibling package's claims, not
//   this package's).
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- one build serves the real run (MUTATE unset) and every
//   mutation run (MUTATE=<id>, one id per assertion). Ids are the claim's
//   own `classify.uid` for the 12 graded claims (this suite's convention)
//   and a plain uppercase id for FIXTURE-LIVE, which carries no manual
//   citation. Two of the six graded topics (P-READY-MOTOR-INVARIANT,
//   RESET-DUTY) are RED at baseline (real findings); per this suite's rule
//   against mutating an already-failing check with a no-op, each substitutes
//   a DIFFERENT genuine stimulus change through the IDENTICAL comparator and
//   the SAME label, flipping baseline RED to mutated GREEN. The remaining
//   four graded topics and FIXTURE-LIVE are GREEN at baseline and flip to
//   RED by omitting the stimulus event the proposition requires.
//
// MUTATION RUN LOG (measured, WSL/Verilator 5.052): baseline (MUTATE unset)
//   13 checks.expect() calls, 9 passed / 4 failed (the 4 failures are
//   FIXTURE-LIVE's control PASS is not among them -- the 4 baseline FAILs
//   are sol:E-disk.c26, opus:E-extdisk-iface.c4, sol:E-disk.c35,
//   opus:E-extdisk-iface.c13, the two RED findings, each carrying two
//   uids). Each of the 13 MUTATE=<id> runs (FIXTURE-LIVE, sol:E-disk.c26,
//   opus:E-extdisk-iface.c4, sol:E-disk.c35, opus:E-extdisk-iface.c13,
//   sol:E-disk.c36, opus:E-extdisk-iface.c14, sol:E-disk.c38,
//   opus:E-extdisk-iface.c16, sol:E-disk.c39, opus:E-extdisk-iface.c17,
//   sol:E-disk.c51, opus:E-extdisk-id.c16) was executed and confirmed, by
//   diffing the full PASS/FAIL-prefixed output against baseline, to move
//   EXACTLY the targeted label's own line (and no other) -- FIXTURE-LIVE
//   PASS->FAIL (8 passed/5 failed); sol:E-disk.c26, opus:E-extdisk-iface.c4,
//   sol:E-disk.c35, opus:E-extdisk-iface.c13 each FAIL->PASS individually
//   (10 passed/3 failed per run); sol:E-disk.c36, opus:E-extdisk-iface.c14,
//   sol:E-disk.c38, opus:E-extdisk-iface.c16, sol:E-disk.c39,
//   opus:E-extdisk-iface.c17, sol:E-disk.c51, opus:E-extdisk-id.c16 each
//   PASS->FAIL individually (8 passed/5 failed per run). All 13 runs then
//   reverted (MUTATE unset) and reconfirmed back to 9/4.

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
// hrm/ch8-drive-select-motor-led): dsktrack (paula_floppy.v:671-680) and
// step_ena_cnt (paula_floppy.v:594-603) have no reset term, and dmaen/dsklen
// state persists once armed. Every test block below constructs its own
// fresh Rig; none is reused across trials.
struct Rig {
    Vpaula_floppy* d;

    // drives_cfg latches into `drives` (paula_floppy.v:554-558) at reset
    // only -- DEVICE-LIMITS is the only block that needs a non-default
    // value (0 keeps _ready a pure function of _sel[0] alone, matching
    // every other block's single-drive assumption).
    explicit Rig(uint8_t drives_cfg = 0) {
        d = new Vpaula_floppy;
        d->clk = 0;
        d->clk7_en = 1;   // held high throughout -- clk7n_en is a port on
        d->clk7n_en = 0;  // this module but is never read anywhere in its body
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
    // GENUINE turn-on edge: deselect first and settle one tick (so
    // _sel_del reads deselected), THEN set the motor level and select --
    // the following tick() is the edge paula_floppy.v:625-658 latches on
    // (!_sel[n] && _sel_del[n]).
    void select_with_motor(int n, bool motor_on_level) {
        deselect_all();
        tick();
        d->_motor = motor_on_level ? 0 : 1;  // DSKMOTOR* is active low
        select(n);
        tick();
    }

    // Read drive n's latched motor_on through `index`
    // (paula_floppy.v:576, index = |(~_sel & motor_on) & ~|rpm_pulse_cnt &
    // sof), with NO clock edge: eval() alone drives this combinational
    // output, so motor_on/_sel_del/rpm_pulse_cnt are untouched by the act
    // of reading (rpm_pulse_cnt only advances on a clk7_en edge where sof
    // was asserted AT that edge, which tick() never does).
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

    // DSKLEN double-write idiom (paula_floppy.v:711-745): holding the
    // register address/data for several clk7_en ticks lets the SAME held
    // write's first tick (dsklen[15]<=data_in[15], non-blocking) become the
    // "already 1" dsklen[15] its own later ticks read back for
    // dmaen<=data_in[15]&dsklen[15] -- one held write_reg() call is enough
    // to latch dmaen, confirmed by CTRL-DMA-SIDE below actually producing a
    // DMA-read sdc_sector write.
    void write_reg(int addr, uint16_t data) {
        d->reg_address_in = (uint8_t)((addr >> 1) & 0xFF);
        d->data_in = data;
        ticks(4);
        d->reg_address_in = 0xFF;
        d->data_in = 0;
    }

    // Arm a disk DMA READ on whichever drive is currently selected and run
    // long enough for the state machine to reach the point where it
    // captures `track` into `sdc_sector` (paula_floppy.v:283, fires once as
    // fd_dma_buf_wr_state leaves state 0).
    void arm_dma_read() {
        d->enable = 1;
        write_reg(0x024, 0x800A);  // DSKLEN: length 10 (0xA), bit14=0 (read), bit15=1 (dma enable)
        ticks(20);
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    std::cout << "Starting appDE-disk-interface...\n";

    // ========================================================================
    // FIXTURE-LIVE -- no claim id, no manual page: the build brief's own
    // requirement that the fixture prove it can deliver a known, distinct
    // word before any read-based assertion below is trusted. Measured on
    // _wprot, the shared sense-line output mux several of this package's
    // graded claims read.
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
    // THE OWNED CONDITION: graded here, NOT declined against
    // hrm/ch8-drive-sense-lines' opus:8-cia.c7 (see file header). Both
    // printed branches require _ready to distinguish the motor state;
    // paula_floppy.v:688-694 has no _motor term at all, so toggling _motor
    // (selection held fixed) must change nothing -- RED at baseline, the
    // expected, previously-unmeasurable finding.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("sol:E-disk.c26");
        r.select(0);
        r.d->_motor = 1;  // motor OFF
        bool ready_state1 = r.ready();
        bool ready_state2;
        if (!mut) {
            r.d->_motor = 0;  // motor ON -- the real stimulus p.334 says should matter
            ready_state2 = r.ready();
        } else {
            // MUTATION (rule against mutating an already-failing check):
            // substitute a genuine, independently-live stimulus change
            // (deselecting the drive) for the motor toggle, through the
            // IDENTICAL != comparator and the same label, proving the
            // comparator itself is sensitive to a real difference.
            r.deselect_all();
            ready_state2 = r.ready();
        }
        checks.note("sol:E-disk.c26: drive0 selected, motor OFF -> ready=" + std::to_string(ready_state1) +
                    "; then " + std::string(mut ? "deselected (mutated substitute)" : "motor ON, still selected") +
                    " -> ready=" + std::to_string(ready_state2));
        checks.expect(ready_state1 != ready_state2,
            "sol:E-disk.c26 (p.334, figure, pin 1 RDY*: 'If motor on, indicates disk installed and up to speed. "
            "If motor not on, identification mode.'): with selection held fixed, _ready must read differently for "
            "motor-on than for motor-off to realise either printed branch -- paula_floppy.v:688-694 derives "
            "_ready from _sel[3:0] and the configured drive count alone (no _motor term anywhere in its cone; the "
            "module's own comment there reads 'not implemented - _ready is active when _sel is active'), so "
            "toggling only _motor changes nothing -- neither branch is realised. Re-opening "
            "hrm/ch8-drive-sense-lines' declined P-READY (opus:8-cia.c7, whose stated reason -- p.244's silence on "
            "the motor-off case -- p.334 ends) is the build stage's call and is not decided here; this is measured "
            "and graded on its own terms");
    }
    {
        Rig r;
        bool mut = mutating("opus:E-extdisk-iface.c4");
        r.select(0);
        r.d->_motor = 1;
        bool ready_state1 = r.ready();
        bool ready_state2;
        if (!mut) {
            r.d->_motor = 0;
            ready_state2 = r.ready();
        } else {
            r.deselect_all();
            ready_state2 = r.ready();
        }
        checks.note("opus:E-extdisk-iface.c4: twin of sol:E-disk.c26, same measurement -> ready1=" +
                    std::to_string(ready_state1) + ", ready2=" + std::to_string(ready_state2));
        checks.expect(ready_state1 != ready_state2,
            "opus:E-extdisk-iface.c4 (p.334, figure, pin 1 RDY*: 'With the motor on it indicates a disk installed "
            "and up to speed; with the motor off it is identification mode.'): twin of sol:E-disk.c26 (same "
            "printed pin, independent reader) -- same measurement, graded independently under its own uid");
    }

    // ========================================================================
    // Already-graded select/motor/step mechanisms, p.334, DECLINED as
    // re-grades of accepted chapter-8 packages -- the SAME RTL mechanism,
    // reached here through a connector pin index instead of a CIA port bit
    // (Lot C's "different axis, not a narrower statement", applied fresh
    // per pin below). The DB23 pin ordinal itself has no RTL counterpart in
    // any case (confirmed in the header's RTL READ) and is recorded as
    // such alongside each re-grade.
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
            "time is 1.4 usec.'): the clocked-by-SELxB* mechanism (paula_floppy.v:618-622,624-659, "
            "!_sel[n]&&_sel_del[n] latching ~_motor at drive n's own edge, confirmed above) is already graded -- "
            "hrm/ch8-drive-select-motor-led's P1/P2/P4 (six uids) grade exactly this at `paula_floppy` from "
            "p.244. Decline as a re-grade. The 1.4 usec setup/hold figures are absolute real-time intervals this "
            "RTL cannot bound (it counts clk7_en ticks with no period attached, the same reason "
            "hrm/ch8-drive-step-timing declines absolute figures), and the DB23 pin ordinal has no RTL "
            "counterpart at all (confirmed in the header's RTL READ: no connector, no pin number, anywhere in "
            "the pinned tree) -- neither is gradable, whatever the mechanism's grading status.");
        checks.record(
            "opus:E-extdisk-iface.c11 (p.334, figure, pin 8 MTRXD*, OC): twin of sol:E-disk.c33 (same printed "
            "pin, independent reader) -- same reasons, declined independently under its own uid.");
    }

    // -- pins 9/20/21 SEL2B*/SEL3B*/SEL1B*, OC: per-drive select lines.
    //    sol:E-disk.c34/c45/c46, opus:E-extdisk-iface.c12/c23/c24.
    {
        // `sel` (paula_floppy.v:165) carries a verilator-public attribute in
        // the source but is not exposed as a top-level model member by this
        // Verilator version, so the confirmation reads the priority
        // encoder's effect through `_ready` instead (floppy_drives=3 makes
        // _ready track ANY of the four _sel bits, paula_floppy.v:688-694).
        Rig r(3);
        r.select(1); bool sel_is_1 = r.ready(); r.deselect_all();
        r.select(2); bool sel_is_2 = r.ready(); r.deselect_all();
        r.select(3); bool sel_is_3 = r.ready(); r.deselect_all();
        checks.note("E-disk.c34/c45/c46 and twins: own confirmation of the priority encoder (paula_floppy.v:615) "
                    "via _ready (floppy_drives=3) -- _sel[1] alone asserts ready: " + std::to_string(sel_is_1) +
                    "; _sel[2] alone asserts ready: " + std::to_string(sel_is_2) +
                    "; _sel[3] alone asserts ready: " + std::to_string(sel_is_3));
        checks.record(
            "sol:E-disk.c34 (p.334, figure, pin 9 SEL2B*, OC: 'Select drive 2.*'): the select-line-to-drive-number "
            "binding (paula_floppy.v:615, confirmed above) is already graded -- "
            "hrm/ch8-cia-disk-signal-bindings grades PB5 DSKSEL2* at the minimig.v:650 join and "
            "hrm/ch8-drive-select-motor-led grades per-drive select behaviour at `paula_floppy`. Decline as a "
            "re-grade. The DB23 pin ordinal has no RTL counterpart (confirmed in the header's RTL READ). The "
            "daisy-chain shift footnote printed under this table is a separate proposition, carried by a sibling "
            "package's claims (sol:E-disk.c49 / opus:E-extdisk-iface.c27 per classify.split), not this uid.");
        checks.record("opus:E-extdisk-iface.c12 (p.334, figure, pin 9 SEL2B*, OC): twin of sol:E-disk.c34 -- same "
                       "reasons, declined independently under its own uid.");
        checks.record(
            "sol:E-disk.c45 (p.334, figure, pin 20 SEL3B*, OC: 'Select drive 3. *'): same mechanism, confirmed "
            "above for _sel[3] -- hrm/ch8-cia-disk-signal-bindings grades PB6 DSKSEL3* at the minimig.v:650 join. "
            "Decline as a re-grade. DB23 pin ordinal not gradable; daisy-chain footnote carried elsewhere.");
        checks.record("opus:E-extdisk-iface.c23 (p.334, figure, pin 20 SEL3B*, OC): twin of sol:E-disk.c45 -- same "
                       "reasons, declined independently under its own uid.");
        checks.record(
            "sol:E-disk.c46 (p.334, figure, pin 21 SEL1B*, OC: 'Select drive 1. *'): same mechanism, confirmed "
            "above for _sel[1] -- hrm/ch8-cia-disk-signal-bindings grades PB4 DSKSEL1* at the minimig.v:650 join. "
            "Decline as a re-grade. DB23 pin ordinal not gradable; daisy-chain footnote carried elsewhere.");
        checks.record("opus:E-extdisk-iface.c24 (p.334, figure, pin 21 SEL1B*, OC): twin of sol:E-disk.c46 -- same "
                       "reasons, declined independently under its own uid.");
    }

    // ========================================================================
    // RESET-DUTY -- sol:E-disk.c35 / opus:E-extdisk-iface.c13 (p.334 pin 10
    // DRESB*, OC, figure-verified: "Amiga system reset. Drives should reset "
    // "their motor-on flip-flops and set their write-protect flip-flops.").
    // NEW HERE (no accepted package grades a reset duty on the drive
    // model). Compound proposition, two independently-measured halves:
    // motor-on reset is real (GREEN); the write-protect "flip-flop" does
    // not exist (_wprot is combinational, RED) -- the conjunction is RED
    // overall.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("sol:E-disk.c35");
        r.select_with_motor(0, /*motor_on_level=*/true);
        bool motor_on_before_reset = r.peek_index(0);
        r.d->floppy_wrprot = mut ? 1 : 0;  // baseline: never request protect, so a "reset SETS it" claim is
                                            // tested honestly; mutated: hold protect requested throughout
        r.pulse_reset();
        bool motor_on_after_reset = r.peek_index(0);
        bool wprot_after_reset = r.wprot();  // drive0 still selected from select_with_motor(); reset doesn't
                                              // touch _sel
        bool motor_half = motor_on_before_reset && !motor_on_after_reset;
        checks.note("sol:E-disk.c35: motor_on latched before reset=" + std::to_string(motor_on_before_reset) +
                    ", after reset=" + std::to_string(motor_on_after_reset) + " (motor half " +
                    std::string(motor_half ? "GREEN" : "RED") + "); _wprot after reset (floppy_wrprot=" +
                    std::string(mut ? "1,mutated" : "0") + ")=" + std::to_string(wprot_after_reset) +
                    " (write-protect half " + std::string(wprot_after_reset ? "GREEN" : "RED") + ")");
        checks.expect(motor_half && wprot_after_reset,
            "sol:E-disk.c35 (p.334, figure, pin 10 DRESB*, OC: 'Amiga system reset. Drives should reset their "
            "motor-on flip-flops and set their write-protect flip-flops.'): reset clears the latched motor-on "
            "state (paula_floppy.v:625-658, 'if (reset) motor_on[n] <= 0;') -- real, measured GREEN -- but "
            "_wprot (paula_floppy.v:664, '&(_sel|disk_writable)') is purely combinational with no flip-flop and "
            "no reset term anywhere in its cone, so reset does not, and cannot, set a write-protect flop -- "
            "measured RED with floppy_wrprot held at its non-protecting level across the reset pulse. The "
            "compound requirement is RED overall on its write-protect half");
    }
    {
        Rig r;
        bool mut = mutating("opus:E-extdisk-iface.c13");
        r.select_with_motor(0, true);
        bool motor_on_before_reset = r.peek_index(0);
        r.d->floppy_wrprot = mut ? 1 : 0;
        r.pulse_reset();
        bool motor_on_after_reset = r.peek_index(0);
        bool wprot_after_reset = r.wprot();
        bool motor_half = motor_on_before_reset && !motor_on_after_reset;
        checks.note("opus:E-extdisk-iface.c13: twin of sol:E-disk.c35, same measurement -> motor half=" +
                    std::string(motor_half ? "GREEN" : "RED") + ", wprot half=" +
                    std::string(wprot_after_reset ? "GREEN" : "RED"));
        checks.expect(motor_half && wprot_after_reset,
            "opus:E-extdisk-iface.c13 (p.334, figure, pin 10 DRESB*, OC: 'Amiga system reset; drives should "
            "reset their motor-on flip-flops and set their write-protect flip-flops.'): twin of sol:E-disk.c35 "
            "(same printed pin, independent reader) -- same measurement, graded independently under its own uid");
    }

    // ========================================================================
    // CHANGE-SET-AT-POWERUP -- sol:E-disk.c36 / opus:E-extdisk-iface.c14
    // (p.334 pin 11 CHNG*, I/O, figure-verified: "Note: Nominally used as an "
    // "open collector input. Drive's change flop is set at power up or when "
    // "no disk is not installed. Flop is reset when drive is selected and "
    // "the head stepped, but only if a disk is installed."). Two of three
    // halves (change-on-remove, change-latch) are already graded by
    // hrm/ch8-drive-sense-lines and declined below via note only (their own
    // uids are not repeated here -- this package's uid grades the "set at "
    // "power up" half specifically, which no accepted package covers).
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
            "NEW here: from a genuinely stable baseline (mount+select+step), reset alone forces _disk_change back "
            "to its 'changed' state -- paula_floppy.v:608-612, the `~({4{reset}}|~disk_present)` term zeroes "
            "_disk_change whenever reset is active, independent of disk_present or any prior stable value");
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
    // (hrm/ch8-cia-disk-signal-bindings, minimig.v:650); NEW here is the
    // polarity AT THE DRIVE MODEL, measured through `track`'s effect on the
    // externally-visible sdc_sector.
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
            "dsktrack[sel]==0 (freshly reset, unstepped), driving `side` to its two levels before arming a disk "
            "DMA read produces two DIFFERENT captures of `sdc_sector` (paula_floppy.v:669, "
            "`track={dsktrack[sel],~side}`, latched at paula_floppy.v:283) -- side is inverted into track's LSB "
            "and does select between two distinct physical halves, the active-low sense this pin states");
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
            "selected and asserted once the drive is selected -- paula_floppy.v:664 (`_wprot=&(_sel|"
            "disk_writable)`), the reduction-AND collapses to 1 (deasserted) whenever _sel=4'b1111 regardless of "
            "disk_writable -- the SELECTED qualifier this page adds over p.244's bare polarity "
            "(hrm/ch8-drive-sense-lines P-WPROT, already graded and not re-tested by this uid)");
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
            "(paula_floppy.v:666,683, confirmed above) are already graded -- hrm/ch8-drive-sense-lines' "
            "P-TRACK0-ZERO and SEL-MASKS-TRACK0 grade exactly this at `paula_floppy` from p.244. Decline as a "
            "re-grade. The DB23 pin ordinal has no RTL counterpart.");
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
            "direction indicated by DIRB.'): the one-cylinder-per-pulse mechanism (paula_floppy.v:671-680, "
            "confirmed above) is already graded -- hrm/ch8-drive-step-timing's A3/A4/A6 grade this at "
            "`paula_floppy` and hrm/ch8-cia-disk-signal-bindings grades PB0 DSKSTEP* at the minimig.v:650 join. "
            "Decline as a re-grade. The DB23 pin ordinal and the OC drive class have no RTL counterpart.");
        checks.record("opus:E-extdisk-iface.c21 (p.334, figure, pin 18 STEPB*, OC): twin of sol:E-disk.c43 -- "
                       "same reasons, declined independently under its own uid.");
        checks.record(
            "sol:E-disk.c44 (p.334, figure, pin 19 DIRB, OC: 'Direction to step the head. Inactive to step "
            "towards center of disk (higher-numbered tracks).'): the direction-to-track-ordinal mapping "
            "(paula_floppy.v:673-677, confirmed above -- inactive direc increments, matching 'higher-numbered "
            "tracks') is already graded -- hrm/ch8-drive-step-timing's A7 DIREC-PROTOCOL and "
            "hrm/ch8-cia-disk-signal-bindings' PB1 DSKDIREC join grade this. Decline as a re-grade. 'Towards "
            "center of disk' is a physical-geometry statement with no RTL counterpart at any boundary in this "
            "tree (confirmed in the header's RTL READ), and the DB23 pin ordinal has none either -- neither is "
            "gradable, whatever the ordinal mapping's grading status.");
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
    // p.351 port table -- not an RTL fact) is realised when `drives` is
    // configured to its maximum; "specific implementations may support "
    // "fewer" is realised when it is configured to its minimum.
    // ========================================================================
    {
        bool mut = mutating("sol:E-disk.c51");
        Rig full(3);  // floppy_drives=3 -- MEASURED (paula_floppy.v:688-694):
                       // reduces _ready to the OR of all four _sel bits
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
            "implementations may support fewer external devices.'): at the maximum configuration, all four "
            "_sel[3:0] slots (paula_floppy.v:86, dsktrack[3:0] at :129) independently assert _ready when "
            "selected -- the structural ceiling of 4 total drive slots, of which the manual's own port table "
            "(p.337/p.351) names one internal and three external, a distinction this RTL itself does not draw; "
            "at the minimum configuration, only drive 0 does, the remaining three (the external slots by that "
            "same convention) do not -- paula_floppy.v:688-694's `drives` term, latched from `floppy_drives` "
            "only at reset, is exactly the 'specific implementations may support fewer' half");
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
// MUTATION sol:E-disk.c26 stimulus green: this assertion is RED at baseline (the reopened P-READY finding, _ready has no _motor term), so per this suite's rule against mutating an already-failing check, the stimulus substitutes a genuine, independently-live change (deselecting the drive) for the motor toggle, through the identical != comparator and the same label, proving the comparator is sensitive to a real difference (MUTATE=sol:E-disk.c26).
// MUTATION opus:E-extdisk-iface.c4 stimulus green: same substitute as sol:E-disk.c26, independent uid (MUTATE=opus:E-extdisk-iface.c4).
// sol:E-disk.c33, opus:E-extdisk-iface.c11, sol:E-disk.c34, opus:E-extdisk-iface.c12, sol:E-disk.c45, opus:E-extdisk-iface.c23, sol:E-disk.c46, opus:E-extdisk-iface.c24, sol:E-disk.c40, opus:E-extdisk-iface.c18, sol:E-disk.c43, opus:E-extdisk-iface.c21, sol:E-disk.c44, opus:E-extdisk-iface.c22 -- all DECLINED via record(), not expect(); no mutation records (not graded through expect()).
// MUTATION sol:E-disk.c35 stimulus green: this assertion is RED at baseline (the write-protect half, _wprot has no flip-flop), so per the rule against mutating an already-failing check, floppy_wrprot is held at its PROTECTING level throughout instead of its non-protecting level, so the write-protect half reads protected after reset for an uninteresting reason (the live input, not a reset-set flop) -- flips the compound check green through the identical wprot() comparator and the same label, confirming the comparator responds to real stimulus (MUTATE=sol:E-disk.c35).
// MUTATION opus:E-extdisk-iface.c13 stimulus green: same substitute as sol:E-disk.c35, independent uid (MUTATE=opus:E-extdisk-iface.c13).
// MUTATION sol:E-disk.c36 stimulus red: the reset pulse is skipped entirely, so the "set at power up" half is never exercised and the stable baseline simply persists unchanged (MUTATE=sol:E-disk.c36).
// MUTATION opus:E-extdisk-iface.c14 stimulus red: same omission as sol:E-disk.c36, independent uid (MUTATE=opus:E-extdisk-iface.c14).
// MUTATION sol:E-disk.c38 stimulus red: the second trial's `side` is held at the SAME level as the first instead of the opposite one, so the two sdc_sector captures cannot be told apart (MUTATE=sol:E-disk.c38).
// MUTATION opus:E-extdisk-iface.c16 stimulus red: same omission as sol:E-disk.c38, independent uid (MUTATE=opus:E-extdisk-iface.c16).
// MUTATION sol:E-disk.c39 stimulus red: the drive is never actually selected, so the "asserted when selected" half is never exercised and the deselected reading is compared against itself (MUTATE=sol:E-disk.c39).
// MUTATION opus:E-extdisk-iface.c17 stimulus red: same omission as sol:E-disk.c39, independent uid (MUTATE=opus:E-extdisk-iface.c17).
// MUTATION sol:E-disk.c51 stimulus red: the second trial's floppy_drives is left at the SAME maximum configuration instead of being lowered, so the "specific implementations may support fewer" half is never exercised and all four drives are recognized in both trials (MUTATE=sol:E-disk.c51).
// MUTATION opus:E-extdisk-id.c16 stimulus red: same omission as sol:E-disk.c51, independent uid (MUTATE=opus:E-extdisk-id.c16).
