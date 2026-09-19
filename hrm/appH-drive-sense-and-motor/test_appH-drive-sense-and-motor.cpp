// Test purpose: HRM conformance package for appendix H's External Disk
//   Connector "Signals When Driving a Disk" prose, pp.368-370 -- RDY*'s
//   motor+disk-presence claim, INDEX*'s per-selected-drive gating, the
//   DKWDB*/DKWEB* write-data selection gate, the CHNG* disk-change latch's
//   select+step clearing condition (and its no-diskette refusal), and
//   MTRXD*'s four independent per-drive motor latches -- against
//   paula_floppy (this suite's only model of a physical Amiga floppy drive).
//   14 claims, from work/2026-09-17_appHJ_packages.jsonl (classify.package
//   == "appH-drive-sense-and-motor").
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed
//   pp.368-370. p.368's DAMAGE header (corpus-defects.md) says the Summary
//   Table BINDS CORRECTLY (character damage only, on pins this package does
//   not cite) and the "Signals When Driving a Disk" prose at the bottom of
//   the page is clean; p.369 is prose with one broken signal name (E)KWEB-
//   for DKWEB-, inside the DKWDB- entry, not inside DKWEB-'s own -- this
//   package cites the DKWDB-/DKWEB- pairing as a whole and does not depend
//   on that one damaged token); p.370 is prose and a numbered procedure,
//   with three damaged bullet glyphs above the (intact) numbered steps --
//   none of it inside the MTRXD- sentence this package cites. All three
//   pages quoted below are quoted from the text layer directly (p368.txt,
//   p369.txt, p370.txt), matching corpus-defects.md's own "read the render
//   only where told to" rule.
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
// RTL READ, IN ORDER, BEFORE ANY EXPECT/RECORD BELOW WAS WRITTEN:
//   1. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   2. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (2048x16
//      fifo, no reset-hazard of its own), to confirm the two-file build
//      closes.
//   3. hrm/appDE-disk-interface/test_appDE-disk-interface.cpp -- full
//      file, for this suite's Checks/mutating()/Rig/MUTATION-comment
//      boilerplate and the select()/select_with_motor()/peek_index()/
//      mount()/unmount()/do_step()/write_reg()/arm_dma_read() idioms,
//      several of which are reused here verbatim.
//   4. hrm/ch8-drive-sense-lines/test_ch8-drive-sense-lines.cpp -- full
//      file, read for its P-CHANGE-ON-REMOVE/P-CHANGE-LATCH/P-TRACK0-ZERO
//      labels (opened per the build brief's instruction before deciding
//      whether this package's claims are re-grades).
//   5. hrm/ch8-drive-select-motor-led/test_ch8-drive-select-motor-led.cpp
//      -- full file, read for its A1-A4/A7-A8 motor-latch labels (the
//      accepted two-drive distinctness measurement this package's MTRXD
//      claims must go wider than, not re-state).
//   6. hrm/ch8-drive-step-timing/test_ch8-drive-step-timing.cpp -- full
//      file, read for its SEL-GATING, DIREC-PROTOCOL and PULSE-WIDTH-FLOOR
//      labels (none of which shares a proposition with this package's
//      claims -- see NOT A RE-GRADE below).
//
// NOT A RE-GRADE, CHECKED AGAINST EACH NAMED PROPOSITION'S OWN LABEL, READ
//   IN ITS SOURCE (build brief: "decline only where a prior assertion
//   grades the SAME proposition"):
//   - appDE-disk-interface's RESET-DUTY (pin 10 DRESB*, a system-reset
//     compound: motor-on flip-flop clear + write-protect flip-flop set)
//     is about the RESET input; none of this package's 14 claims mention
//     reset at all. Different signal, different proposition.
//   - appDE-disk-interface's WPRO-SELECT-GATE (pin 14 WPRO*, the SELECTED
//     qualifier on write-protect) and SIDE-POLARITY (pin 13 SIDEB*) are
//     about _wprot and side/track, neither of which any claim in this
//     package's 14 reads or cites. Different signal, different
//     proposition.
//   - appDE-disk-interface's CHANGE-SET-AT-POWERUP (pin 11 CHNG*, the
//     "set at power up" half) reads _change through a RESET pulse. This
//     package's CHANGE-CLEAR-NEEDS-SELECT and CHANGE-NO-CLEAR-NO-DISK
//     read _change through SELECT/STEP/mount/unmount and never assert
//     reset -- the SETTING half (at power-up) versus the CLEARING half
//     (select+step, and its no-diskette refusal) are different halves of
//     the same latch, not the same proposition.
//   - ch8-drive-sense-lines' P-CHANGE-ON-REMOVE (_change asserts once an
//     inserted, stable disk is removed) and P-CHANGE-LATCH (_change stays
//     asserted through reinsertion ALONE and clears only once a step
//     pulse follows, drive held SELECTED throughout) are both read here.
//     P-CHANGE-LATCH's own label, quoted from its source: "with the drive
//     selected, _change stays asserted through reinsertion alone and
//     clears only once a step pulse follows". It holds the drive selected
//     THE WHOLE TIME and never drives a step while DESELECTED at all --
//     the ~_sel term in the clearing arm (paula_floppy.v:610) is never
//     exercised there. p.369's own sentence adds a conjunct P-CHANGE-LATCH's
//     source page (p.244, "inserted AND a step pulse") does not carry:
//     "the system must select the drive, AND step the head" -- CHANGE-
//     CLEAR-NEEDS-SELECT grades exactly that added conjunct (a step while
//     DESELECTED must not clear the latch, only a step while SELECTED
//     does), and CHANGE-NO-CLEAR-NO-DISK grades a second conjunct neither
//     P-CHANGE-ON-REMOVE nor P-CHANGE-LATCH ever measures (disk absence
//     held across the clearing attempt). Different, narrower propositions.
//   - ch8-drive-sense-lines' opus:8-cia.c7 (P-READY, motor OFF, declined
//     there as untestable because p.244 itself refuses to define _ready
//     with the motor off) and appDE-disk-interface's re-opened
//     P-READY-MOTOR-INVARIANT (RED: _ready has no _motor term, measured
//     by toggling _motor with selection fixed) are both about the MOTOR
//     axis. RDY-DISK-PRESENCE below toggles DISK PRESENCE
//     (sdc_image_mounted/sdc_image_size) with selection AND motor BOTH
//     held fixed -- a third, independently drivable axis neither prior
//     assertion moves. See RDY-DISK-PRESENCE's own comment below for the
//     full argument.
//   - ch8-drive-select-motor-led's P1/P2 (opus:8-cia.c24/c28,
//     sol:8-cia.c22/c26 and siblings) grade the motor latch's
//     latch-at-own-edge and remembers-when-deselected properties on
//     drives 0 and 1 -- TWO drives, establishing the per-drive-latch
//     mechanism exists. p.369/p.370's claims below say the mechanism
//     serves ALL FOUR drives from the ONE shared MTRXD- line. Two is not
//     four: MTRXD-FOUR-INDEPENDENT-LATCHES-P369/P370 below latch four
//     DIFFERENT, alternating levels onto all four drives in one trial and
//     read all four back distinctly -- wider than the accepted pair.
//   - ch8-drive-step-timing's SEL-GATING (STEP acceptance gated by
//     selection), DIREC-PROTOCOL (direc polarity) and PULSE-WIDTH-FLOOR
//     (declined: absolute "1 us min.", no real-time unit in this RTL) are
//     all about STEPB*/DIRB*, signals no claim in this package cites at
//     all. Different signals entirely.
//   - CORRECTED AFTER CHECKING (the classifier's own evidence for
//     opus:AHJ-072/sol:H-048 said "grep ... finds them only in generated
//     obj_dir files", and that is now FALSE): a fresh grep for `sdc_wr`
//     across every test_*.cpp under nanomig-hrm-tests/hrm finds it read in
//     hrm/ch8-dsklen-dma-arming (its A8-WRITE-DIRECTION-BIT-AND-PREVENTION
//     block, checking `sdc_wr==0` as one of three signals proving a SINGLE
//     DSKLEN write starts no DMA at all) and hrm/ch8-disk-controller-
//     overview reads the sibling `sdc_rd` extensively as its own central
//     mechanism. Read directly: ch8-dsklen-dma-arming's block never varies
//     `_sel` and never grades which drive's `sdc_wr` bit is set -- it is
//     about WHETHER a DMA arms at all (a single vs. double DSKLEN write),
//     not about SELECTION gating a per-drive request. Different
//     proposition; WRITE-DATA-SELECT-GATE below is still the first
//     assertion anywhere in the suite to grade `sdc_wr`'s per-drive
//     selectivity.
//
// MECHANISM, confirmed by reading paula_floppy.v myself (line numbers
//   below are this package's own verification):
//
//   _ready (paula_floppy.v:688-694), quoted in full:
//     // drive _ready signal control
//     // Amiga DD drive activates _ready whenever _sel is active and motor is off
//     // or whenever _sel is active, motor is on and there is a disk inserted (not implemented - _ready is active when _sel is active)
//     assign _ready   = (_sel[3] | ~(drives[1] & drives[0]))
//             & (_sel[2] | ~drives[1])
//             & (_sel[1] | ~(drives[1] | drives[0]))
//             & (_sel[0]);
//   With floppy_drives=0 (this file's Rig default, one drive configured),
//   every term but the last collapses to a constant 1, so `_ready` reduces
//   to exactly `_sel[0]`. Neither `_motor` nor `disk_present` appears
//   anywhere in this expression's cone -- confirmed directly, not merely
//   disclosed: `disk_present` (paula_floppy.v:961, updated :963-966) feeds
//   only `_disk_change` (paula_floppy.v:610), never `_ready`.
//
//   index / motor_on (paula_floppy.v:576,618-659):
//     assign index = |(~_sel & motor_on) & ~|rpm_pulse_cnt & sof;
//     always @(posedge clk) begin if (clk7_en) begin
//       if (reset) motor_on[n] <= 0;
//       else if (!_sel[n] && _sel_del[n]) motor_on[n] <= ~_motor;
//     end end                                             // n = 0..3
//   `index` requires BOTH a selected drive (~_sel) AND that drive's
//   latched motor_on, ANDed with rpm_pulse_cnt==0 and sof -- this
//   package's Rig drives sof itself when reading, so rpm_pulse_cnt==0 is
//   guaranteed (it is also the model's reset value). The GATING condition
//   (selected AND motor-on, vs. deselected, vs. selected-but-motor-off)
//   is exactly what INDEX-SEL-MOTOR-GATE below measures. The RATE ("once
//   for each revolution") is NOT gradable: `rpm_pulse_cnt`
//   (paula_floppy.v:563-573) counts `sof` pulses against an RTL-internal
//   constant (11, or 9 for ntsc) with no revolution-time reference at any
//   boundary this fixture can reach -- using it as the oracle for a
//   printed rate would be an RTL-derived oracle value, so it is reported
//   via note(), never graded.
//
//   sdc_wr (paula_floppy.v:410, inside the write-DMA completion state
//   machine, `fd_dma_buf_wr_state` case 4'd7):
//     4'd7: begin
//        if (!sdc_busy) begin
//           sdc_wr <= ~_sel;
//           fd_dma_buf_wr_state <= 4'd8;
//        end
//     end
//   Reached only once a full write-DMA transfer completes with a matching
//   header/payload checksum (the `cpu_wr_state` machine, paula_floppy.v:
//   781-903, gates entry via `if(cpu_wr_state==4'd0)` after
//   `wr_sector_csum` reads zero, paula_floppy.v:388-402). `sdc_wr` is
//   registered, not a continuous function of `_sel` -- it captures
//   whichever drive was selected AT COMPLETION and holds that value
//   (state 4'd8 only clears it once `sdc_busy` goes high, which this
//   Rig's `sdc_busy=0` throughout never does), so demonstrating "only the
//   selected drive is requested, and a different selection moves the
//   request with it" needs TWO independent, complete transfers with
//   different drives selected -- exactly what WRITE-DATA-SELECT-GATE does
//   below. The transfer is driven honestly through the DSKLEN register
//   and the DSKDAT bus-write path (`buswr & dmaon`, paula_floppy.v:771),
//   not by poking any internal signal directly: every word fed is 0x0000
//   except the FIRST, which must equal `dsksync` (default 0x4489 after
//   reset, paula_floppy.v:705) to satisfy the sync-marker check
//   (paula_floppy.v:795). Because every word after the sync is uniformly
//   zero, the header-checksum, track-match and payload-checksum
//   verifications (paula_floppy.v:807-895) all pass trivially (an XOR
//   accumulator seeded at 0 and XORed with 0 throughout stays 0, and the
//   decoded track field is 0, matching a freshly-reset, unstepped drive
//   with `side=1` so that `track={dsktrack[sel],~side}` reads 0) --
//   exact cycle alignment among the trailing zero words therefore does
//   not matter, only the leading sync word's exact value and count do.
//
//   disk-change latch (paula_floppy.v:608-612,662):
//     assign _change = &(_sel | _disk_change);
//     _disk_change <= (_disk_change | ~_sel & {4{_step}} & ~{4{_step_del}}
//                       & disk_present) & ~({4{reset}} | ~disk_present);
//   The clearing (OR) arm carries BOTH `~_sel` (drive selected) AND
//   `disk_present` (diskette present) as separate factors alongside the
//   `_step` rising edge -- CHANGE-CLEAR-NEEDS-SELECT measures the `~_sel`
//   factor (a step while deselected must not clear the latch) and
//   CHANGE-NO-CLEAR-NO-DISK measures the `disk_present` factor (a
//   select+step attempt with no diskette mounted must not clear it,
//   confirmed independently by the trailing `~(...|~disk_present)` term
//   also forcing the latch to its changed state whenever `disk_present`
//   is false, regardless of the OR arm).
//
// NOT GRADABLE ANYWHERE IN THIS PACKAGE (recorded via note(), never
//   graded): the RATE half of INDEX- ("once for each revolution",
//   rpm_pulse_cnt has no revolution-time reference); MTRXD-'s printed 1.4
//   microsecond setup/hold figures (absolute real-time intervals; this
//   RTL counts clk7_en ticks with no period attached at any boundary,
//   the same reason hrm/ch8-drive-step-timing declines its own absolute
//   figures); the appendix's own tension between "MTRXD- ... for all four
//   disk drives" and its connector table naming only three EXTERNAL
//   select lines (SEL1B-/SEL2B-/SEL3B-) -- recorded, not resolved, same
//   as ch8-drive-select-motor-led/appDE precedent for this exact tension.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- one build serves the real run (MUTATE unset) and every
//   mutation run (MUTATE=<id>, one id per assertion). Per this package's
//   own convention (matching hrm/ch8-drive-step-timing, and the build
//   brief's instruction to put uids at the END of the label): each
//   assertion's id is its PROPOSITION NAME, not a claim uid, and each
//   proposition's TWO claim uids (one opus, one sol) are cited together
//   in a `[claims: ...]` suffix at the end of the label -- both count as
//   ASSERTED by the graded line either way, per the build brief's own
//   rule. Every proposition below is expected GREEN except
//   RDY-DISK-PRESENCE, which is expected RED (a genuine finding); its
//   mutation substitutes a different, independently-live stimulus
//   (deselection, proven live by FIXTURE-LIVE below) through the
//   IDENTICAL comparator and the SAME label, per this suite's rule
//   against mutating an already-failing check with a no-op.
//
// MUTATION RUN LOG (measured, WSL/Verilator 5.052): baseline (MUTATE
//   unset) 8 checks.expect() calls, 7 passed / 1 failed -- the 1 failure
//   is RDY-DISK-PRESENCE, the expected finding. Each of MUTATE=
//   FIXTURE-LIVE, INDEX-SEL-MOTOR-GATE, WRITE-DATA-SELECT-GATE,
//   CHANGE-CLEAR-NEEDS-SELECT, CHANGE-NO-CLEAR-NO-DISK,
//   MTRXD-FOUR-INDEPENDENT-LATCHES-P369 and
//   MTRXD-FOUR-INDEPENDENT-LATCHES-P370 was executed and confirmed, by
//   diffing the full PASS/FAIL-prefixed output against baseline, to move
//   EXACTLY the targeted label's own line (and no other) from PASS to
//   FAIL (6 passed / 2 failed per run, RDY-DISK-PRESENCE's baseline
//   failure unaffected). MUTATE=RDY-DISK-PRESENCE was executed and
//   confirmed to move exactly that line from FAIL to PASS (8 passed / 0
//   failed), nothing else moving. All 8 runs were then reconfirmed back
//   to baseline (MUTATE unset, 7/1) by construction (the mutation guard
//   only fires when MUTATE matches).

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

// RIG HAZARD (same as hrm/appDE-disk-interface, hrm/ch8-drive-sense-lines
// and hrm/ch8-drive-select-motor-led): dsktrack (paula_floppy.v:671-680)
// and step_ena_cnt (paula_floppy.v:594-603) have no reset term, and
// dmaen/dsklen state persists once armed. Every test block below
// constructs its own fresh Rig; none is reused across trials.
struct Rig {
    Vpaula_floppy* d;

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
        d->side = 1;    // track={dsktrack[sel],~side} reads 0 at reset with side=1
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

    void select(int n) {
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        d->eval();
    }
    void deselect_all() { d->_sel = 0xF; d->eval(); }

    void do_step(bool direc_outward) {
        d->direc = direc_outward ? 1 : 0;
        d->_step = 0;
        tick();
        d->_step = 1;
        tick();
        tick();
    }

    bool ready()  { d->eval(); return d->_ready == 0; }   // active low
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

    // Read drive n's latched motor_on through `index` (paula_floppy.v:576),
    // FORCING _sel to select n during the read, with NO clock edge: eval()
    // alone drives this combinational output, so motor_on/_sel_del/
    // rpm_pulse_cnt are untouched by the act of reading.
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

    // Read `index` at whatever `_sel` currently is, WITHOUT forcing
    // selection -- needed to measure the deselected-masking half of
    // INDEX-SEL-MOTOR-GATE, which peek_index() above cannot see (it
    // always selects the drive it reads).
    bool read_index_now() {
        d->sof = 1;
        d->eval();
        bool v = d->index;
        d->sof = 0;
        d->eval();
        return v;
    }

    // DSKLEN double-write idiom (paula_floppy.v:711-745): holding the
    // register address/data for several clk7_en ticks lets the SAME held
    // write's first tick (dsklen[15]<=data_in[15], non-blocking) become
    // the "already 1" dsklen[15] its own later ticks read back for
    // dmaen<=data_in[15]&dsklen[15] -- one held write_reg() call is
    // enough to latch dmaen (confirmed working in hrm/appDE-disk-interface).
    void write_reg(int addr, uint16_t data) {
        d->reg_address_in = (uint8_t)((addr >> 1) & 0xFF);
        d->data_in = data;
        ticks(4);
        d->reg_address_in = 0xFF;
        d->data_in = 0;
    }

    // Arm a disk DMA WRITE (dsklen[14]=1) on whichever drive is currently
    // selected, with a generous length so `dmaon` (paula_floppy.v:1011,
    // = ~lenzero while dsklen[14]=1) stays high for the whole transfer
    // pushed by push_dskdat_word() below.
    void arm_dma_write(int length) {
        d->enable = 1;
        write_reg(0x024, (uint16_t)(0x8000 | 0x4000 | (length & 0x3FFF)));
        ticks(20);
    }

    // One DSKDAT bus write, held for EXACTLY one clk7_en tick, so exactly
    // one word enters the fifo (paula_floppy.v:771, fifo_wr = ... |
    // (buswr & dmaon); holding the address longer would push the SAME
    // data_in value on every held tick).
    void push_dskdat_word(uint16_t w) {
        d->reg_address_in = 0x13;  // DSKDAT (9'h026) >> 1
        d->data_in = w;
        tick();
        d->reg_address_in = 0xFF;
        d->data_in = 0;
    }

    // Push one full write-DMA sector stream: a sync word (must equal
    // dsksync, default 0x4489 after reset) followed by 544 zero words --
    // 24 header/header-checksum words, 4 payload-checksum-preamble words,
    // 512 payload words and 4 margin words to flush cpu_wr_state's final
    // state (paula_floppy.v:884-895 needs two more stbdat pulses after the
    // 512th payload word to register "sector received" and return to
    // state 0). Every word past the sync is uniformly zero, so no exact
    // cycle alignment among them matters -- see the MECHANISM comment
    // above the file header for why this makes the header/track/payload
    // checksum verifications pass trivially.
    void push_zero_sector(uint16_t sync_word = 0x4489) {
        push_dskdat_word(sync_word);
        for (int i = 0; i < 24; i++) push_dskdat_word(0x0000);
        for (int i = 0; i < 4; i++)  push_dskdat_word(0x0000);
        for (int i = 0; i < 512; i++) push_dskdat_word(0x0000);
        for (int i = 0; i < 4; i++)  push_dskdat_word(0x0000);
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    std::cout << "Starting appH-drive-sense-and-motor...\n";

    // ========================================================================
    // FIXTURE-LIVE -- no claim id, no manual page: the build brief's own
    // requirement that the fixture prove it can deliver a known, distinct
    // word before any read-based assertion below is trusted. Measured on
    // `_ready` responding to selection -- the same port RDY-DISK-PRESENCE
    // reads below, and the same "genuinely live, different stimulus" fact
    // that assertion's mutation substitutes for disk presence.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("FIXTURE-LIVE");
        r.deselect_all();
        bool ready_deselected = r.ready();
        // MUTATION: never actually select the drive -- the "asserts once
        // selected" half is never exercised.
        if (!mut) r.select(0);
        bool ready_selected = r.ready();
        checks.note("FIXTURE-LIVE: deselected -> _ready asserted=" + std::to_string(ready_deselected) +
                    ", " + std::string(mut ? "still deselected (mutated)" : "selected") +
                    " -> _ready asserted=" + std::to_string(ready_selected));
        checks.expect(!ready_deselected && ready_selected,
            "FIXTURE-LIVE: deselecting every drive deasserts _ready and selecting a configured drive asserts "
            "it -- paula_floppy.v:688-694 -- the sense-line read path is live, not stuck, and selection is a "
            "genuinely live, independent stimulus before any claim below is trusted");
    }

    // ========================================================================
    // RDY-DISK-PRESENCE -- p.368 RDY-: "When a disk drive's motor is on,
    // this line indicates the selected disk is installed and rotating at
    // speed." [claims: opus:AHJ-060, sol:H-036]
    //
    // A THIRD AXIS, not a re-grade: hrm/ch8-drive-sense-lines' opus:8-cia.c7
    // was declined because p.244 itself refuses to define _ready with the
    // motor off. hrm/appDE-disk-interface's P-READY-MOTOR-INVARIANT is
    // GRADED RED by toggling _motor alone, selection held fixed. Neither
    // ever varies disk presence. This page's motor-ON branch names DISK
    // PRESENCE as what the line indicates ("installed"), and disk presence
    // (sdc_image_mounted/sdc_image_size -> disk_present, paula_floppy.v:
    // 959-966) is a THIRD, independently drivable axis this assertion
    // moves with selection AND motor BOTH held fixed -- an axis no
    // accepted assertion anywhere in the suite moves against _ready.
    // EXPECT RED: paula_floppy.v:688-694 (quoted in full in the file
    // header) has no disk_present term anywhere in its cone.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("RDY-DISK-PRESENCE");
        r.select(0);
        r.d->_motor = 0;  // motor ON throughout, per the cited branch
        r.unmount(0);     // disk absent
        bool ready_absent = r.ready();
        bool ready_present;
        if (!mut) {
            r.mount(0, 1000);  // disk present -- the real stimulus p.368 says should matter
            ready_present = r.ready();
        } else {
            // MUTATION (rule against mutating an already-failing check):
            // substitute a genuine, independently-live stimulus change
            // (deselecting the drive, proven live by FIXTURE-LIVE above)
            // for the disk-presence toggle, through the IDENTICAL !=
            // comparator and the same label.
            r.deselect_all();
            ready_present = r.ready();
        }
        checks.note("RDY-DISK-PRESENCE: drive0 selected, motor ON, disk absent -> ready=" +
                    std::to_string(ready_absent) + "; then " +
                    std::string(mut ? "deselected (mutated substitute)" : "disk mounted, still selected") +
                    " -> ready=" + std::to_string(ready_present));
        checks.expect(ready_absent != ready_present,
            "RDY-DISK-PRESENCE (p.368, RDY-: 'When a disk drive's motor is on, this line indicates the "
            "selected disk is installed and rotating at speed.'): with selection and motor both held fixed "
            "(motor ON, drive selected throughout), _ready must read differently for disk-absent than for "
            "disk-present to realise the printed claim -- paula_floppy.v:688-694 derives _ready from _sel[3:0] "
            "and the configured drive count alone, with no disk_present term anywhere in its cone (disk_present "
            "feeds only _disk_change, paula_floppy.v:610, confirmed by direct reading) -- so toggling only disk "
            "presence changes nothing. This is a THIRD axis, distinct from the accepted motor-toggle finding and "
            "from the page-244 claim declined for the motor's OWN off-state being undefined: here selection and "
            "motor are both fixed and only disk presence varies, which no accepted assertion moves against "
            "_ready [claims: opus:AHJ-060, sol:H-036]");
    }

    // ========================================================================
    // INDEX-SEL-MOTOR-GATE -- p.369 INDEX-(Pin #22): "A selected drive
    // pulses this signal low once for each revolution of its motor."
    // [claims: opus:AHJ-065, sol:H-040]
    //
    // The GATING condition graded here (selected AND motor-on can pulse;
    // deselected cannot; selected-but-motor-off cannot) is not read by any
    // accepted package: hrm/ch8-disk-index-interrupt-path grades only what
    // happens to a pulse DOWNSTREAM of `index`, and hrm/ch8-drive-select-
    // motor-led reads `index` only as an uncited fixture-liveness probe.
    // The RATE ("once for each revolution") is measured and reported
    // separately via note(), not graded -- see NOT GRADABLE in the file
    // header.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("INDEX-SEL-MOTOR-GATE");
        r.select_with_motor(0, /*motor_on_level=*/true);
        bool pulses_selected_motor_on = r.read_index_now();
        Rig r2;
        r2.select_with_motor(0, /*motor_on_level=*/false);
        bool no_pulse_motor_off = !r2.read_index_now();
        r.select_with_motor(0, /*motor_on_level=*/true);  // reselect r's drive0, motor back ON
        // MUTATION: never actually deselect -- the masking condition
        // (~_sel in the index gate) is never exercised.
        if (!mut) r.deselect_all();
        bool no_pulse_deselected = !r.read_index_now();
        checks.note("INDEX-SEL-MOTOR-GATE: selected+motor-on pulses=" + std::to_string(pulses_selected_motor_on) +
                    "; selected+motor-off pulses=" + std::to_string(!no_pulse_motor_off) + "; " +
                    std::string(mut ? "still selected (mutated)" : "deselected") + "+motor-on pulses=" +
                    std::to_string(!no_pulse_deselected));
        checks.expect(pulses_selected_motor_on && no_pulse_motor_off && no_pulse_deselected,
            "INDEX-SEL-MOTOR-GATE (p.369, INDEX-(Pin #22): 'A selected drive pulses this signal low once for "
            "each revolution of its motor.'): paula_floppy.v:576 (index=|(~_sel&motor_on)&~|rpm_pulse_cnt&sof) "
            "pulses only for a SELECTED drive whose latched motor_on is set -- a selected, motor-running drive "
            "pulses; the same drive deselected does not; the same drive selected with its motor latched off "
            "does not. The RATE ('once for each revolution') is not graded here -- see the MEASURED note below "
            "[claims: opus:AHJ-065, sol:H-040]");
        checks.note("INDEX rate (not graded): rpm_pulse_cnt (paula_floppy.v:563-573) counts `sof` pulses against "
                    "an RTL-internal constant (11, or 9 for ntsc) with no revolution-time reference at any "
                    "boundary this fixture can reach -- using it as the oracle for 'once per revolution' would "
                    "be an RTL-derived oracle value");
    }

    // ========================================================================
    // WRITE-DATA-SELECT-GATE -- p.369 DKWDB-(Pin #17)/DKWEB-(Pin #16): "The
    // system drives write data to all disks via this signal. The data is
    // only written when DKWEB- is active (low). Data is written only to
    // selected drives." [claims: opus:AHJ-072, sol:H-048]
    //
    // ch8-dsklen-dma-arming reads `sdc_wr` too, but only as a control
    // proving a SINGLE DSKLEN write arms no DMA at all -- it never varies
    // `_sel` and never grades which drive's bit is set (see the "CORRECTED
    // AFTER CHECKING" note in the file header: the classifier's own "grep
    // finds it only in generated obj_dir files" evidence was wrong). This
    // block is still the first in the suite to grade `sdc_wr`'s per-drive
    // selectivity. A full write-DMA transfer is driven honestly through
    // DSKLEN/DSKDAT (see MECHANISM in the file header for why an all-zero
    // payload after the sync word passes the header/track/payload checksum
    // checks trivially) so that paula_floppy.v:410 (`sdc_wr <= ~_sel`)
    // actually fires.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("WRITE-DATA-SELECT-GATE");
        r.select(0);
        r.arm_dma_write(700);
        r.push_zero_sector();
        r.ticks(300);  // settle/drain margin
        bool bit0_set = (r.d->sdc_wr >> 0) & 1;
        bool only_bit0 = (r.d->sdc_wr & ~0x1) == 0;

        Rig r2;
        // MUTATION: select the SAME drive as the first trial instead of a
        // different one, so "selecting a different drive must move the
        // request with it" is never exercised.
        r2.select(mut ? 0 : 1);
        r2.arm_dma_write(700);
        r2.push_zero_sector();
        r2.ticks(300);
        bool bit1_set = (r2.d->sdc_wr >> 1) & 1;
        bool only_bit1 = (r2.d->sdc_wr & ~0x2) == 0;

        checks.note("WRITE-DATA-SELECT-GATE: trial1 (drive0 selected) sdc_wr=0x" +
                    std::to_string((unsigned)r.d->sdc_wr) + "; trial2 (" +
                    std::string(mut ? "drive0 selected, mutated" : "drive1 selected") + ") sdc_wr=0x" +
                    std::to_string((unsigned)r2.d->sdc_wr));
        checks.expect(bit0_set && only_bit0 && bit1_set && only_bit1,
            "WRITE-DATA-SELECT-GATE (p.369, DKWDB-(Pin #17)/DKWEB-(Pin #16): 'The system drives write data to "
            "all disks via this signal. ... Data is written only to selected drives.'): a complete write-DMA "
            "transfer with drive 0 selected requests a write for drive 0 alone (sdc_wr==4'b0001), and the same "
            "transfer with drive 1 selected instead requests drive 1 alone (sdc_wr==4'b0010) -- "
            "paula_floppy.v:410 (`sdc_wr <= ~_sel`, inside the write-completion state machine) captures exactly "
            "the selected slot and moves with selection, never touching an unselected drive's bit "
            "[claims: opus:AHJ-072, sol:H-048]");
    }

    // ========================================================================
    // CHANGE-CLEAR-NEEDS-SELECT -- p.369 CHNG-(Pin #11): "To reset the
    // latch, the system must select the drive, and step the head."
    // [claims: opus:AHJ-076, sol:H-052]
    //
    // NARROWER than the accepted ch8-drive-sense-lines' P-CHANGE-LATCH,
    // whose own label (read in its source) holds the drive SELECTED
    // throughout and never drives a step while deselected: "with the
    // drive selected, _change stays asserted through reinsertion alone
    // and clears only once a step pulse follows". p.369 adds a conjunct
    // that label never tests -- select THE DRIVE, not merely step it.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("CHANGE-CLEAR-NEEDS-SELECT");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false);
        bool stable_baseline = !r.change();
        r.unmount(0);
        r.mount(0, 1000);  // reinsert -- reinsertion alone does not clear (P-CHANGE-LATCH, already graded)
        bool changed_before_attempt = r.change();
        r.deselect_all();
        // MUTATION: select the drive before this "deselected" step
        // attempt, defeating the very condition under test.
        if (mut) r.select(0);
        r.do_step(false);
        // _change is itself MASKED while deselected (paula_floppy.v:662,
        // &(_sel|_disk_change) reads deasserted whenever no drive is
        // selected, the same masking SEL-MASKS-TRACK0 measures for
        // _track0) -- reselect (WITHOUT another step) before reading, so
        // the read reports the latch's true internal state rather than
        // the deselected mask. Reselecting alone cannot itself clear the
        // latch: _step stays idle-high with _step_del already high, so
        // `_step && !_step_del` (the clearing arm's edge term) stays false.
        if (!mut) r.select(0);
        bool still_changed_after_deselected_step = r.change();
        r.select(0);
        r.do_step(false);
        bool cleared_after_select_and_step = !r.change();
        checks.note("CHANGE-CLEAR-NEEDS-SELECT: baseline stable=" + std::to_string(stable_baseline) +
                    ", changed before attempt=" + std::to_string(changed_before_attempt) + ", still changed "
                    "after step " + std::string(mut ? "while selected (mutated)" : "while deselected") + "=" +
                    std::to_string(still_changed_after_deselected_step) + ", cleared after select+step=" +
                    std::to_string(cleared_after_select_and_step));
        checks.expect(stable_baseline && changed_before_attempt && still_changed_after_deselected_step &&
                          cleared_after_select_and_step,
            "CHANGE-CLEAR-NEEDS-SELECT (p.369, CHNG-(Pin #11): 'To reset the latch, the system must select the "
            "drive, and step the head.'): from a changed latch with the disk reinserted, a step pulse while ALL "
            "drives are deselected does not clear it, and the identical step pulse once the drive is selected "
            "does -- paula_floppy.v:610, the clearing arm's `~_sel` factor gates the `_step` rising edge "
            "independently of `disk_present`, the conjunct p.369 adds over the accepted P-CHANGE-LATCH reading "
            "(which holds the drive selected throughout and never drives a step while deselected) "
            "[claims: opus:AHJ-076, sol:H-052]");
    }

    // ========================================================================
    // CHANGE-NO-CLEAR-NO-DISK -- p.369 CHNG-(Pin #11): "Of course, the
    // latch will not reset if there is no diskette installed."
    // [claims: opus:AHJ-077, sol:H-053]
    //
    // NARROWER than the accepted P-CHANGE-LATCH, whose trial reinserts the
    // image before stepping -- the no-diskette case this sentence states
    // is never measured there.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("CHANGE-NO-CLEAR-NO-DISK");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false);
        bool stable_baseline = !r.change();
        r.unmount(0);  // disk absent -- changed, and stays changed per the latch's own trailing term
        bool changed_after_removal = r.change();
        // MUTATION: reinsert the disk before the clear attempt, defeating
        // the "no diskette" condition under test.
        if (mut) r.mount(0, 1000);
        r.do_step(false);  // drive still selected from above
        bool still_changed_no_disk_attempt = r.change();
        r.mount(0, 1000);  // control half, same trial: WITH a disk, select+step genuinely clears
        r.do_step(false);
        bool cleared_with_disk_present = !r.change();
        checks.note("CHANGE-NO-CLEAR-NO-DISK: baseline stable=" + std::to_string(stable_baseline) +
                    ", changed after removal=" + std::to_string(changed_after_removal) + ", still changed after "
                    "select+step " + std::string(mut ? "with disk reinserted (mutated)" : "with no disk present") +
                    "=" + std::to_string(still_changed_no_disk_attempt) + ", cleared once a disk is present=" +
                    std::to_string(cleared_with_disk_present));
        checks.expect(stable_baseline && changed_after_removal && still_changed_no_disk_attempt &&
                          cleared_with_disk_present,
            "CHANGE-NO-CLEAR-NO-DISK (p.369, CHNG-(Pin #11): 'Of course, the latch will not reset if there is no "
            "diskette installed.'): with the drive selected and no diskette mounted, a step pulse does not clear "
            "the changed latch, while the identical select+step sequence WITH a disk present does -- "
            "paula_floppy.v:610's trailing `~({4{reset}}|~disk_present)` term holds `_disk_change` at its "
            "changed value for as long as no image is mounted, independent of the `_step`/`~_sel` clearing arm -- "
            "the no-diskette case the accepted P-CHANGE-LATCH reading never measures (its trial reinserts the "
            "image before stepping) [claims: opus:AHJ-077, sol:H-053]");
    }

    // ========================================================================
    // MTRXD-FOUR-INDEPENDENT-LATCHES-P369 -- p.369 MTRXD-(Pin #8): "This is
    // the motor control line for all four disk drives."
    // [claims: opus:AHJ-078, sol:H-054]
    //
    // WIDER than the accepted ch8-drive-select-motor-led reading, whose
    // own label (read in its source) grades exactly TWO drives:
    // "drive0's latch reflects _motor as driven at drive0's OWN _sel[0]
    // turn-on edge, and drive1's latch independently reflects the
    // DIFFERENT _motor level driven later at drive1's own _sel[1]
    // turn-on edge" -- it does not establish four. This block latches
    // four DIFFERENT, alternating levels onto all four drives in one
    // trial and reads all four back distinctly.
    //
    // RECORDED, NOT RESOLVED (matching the classifier's own note): the
    // appendix says MTRXD- serves "all four disk drives" while its own
    // connector table names only three EXTERNAL select lines (SEL1B-,
    // SEL2B-, SEL3B- at pins 21, 9 and 20) -- this package records the
    // tension and grades the four independent latches, which is what the
    // RTL boundary presents.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("MTRXD-FOUR-INDEPENDENT-LATCHES-P369");
        r.select_with_motor(0, false);
        r.select_with_motor(1, true);
        r.select_with_motor(2, false);
        // MUTATION: give drive3 the SAME level as drive2 instead of the
        // opposite one, so the four-way distinctness this assertion
        // grades is never exercised.
        r.select_with_motor(3, mut ? false : true);
        bool d0 = r.peek_index(0);
        bool d1 = r.peek_index(1);
        bool d2 = r.peek_index(2);
        bool d3 = r.peek_index(3);
        checks.note("MTRXD-FOUR-INDEPENDENT-LATCHES-P369: latched OFF,ON,OFF," +
                    std::string(mut ? "OFF(mutated)" : "ON") + " -> read back " + std::to_string(d0) + "," +
                    std::to_string(d1) + "," + std::to_string(d2) + "," + std::to_string(d3));
        checks.expect(d0 == false && d1 == true && d2 == false && d3 == true,
            "MTRXD-FOUR-INDEPENDENT-LATCHES-P369 (p.369, MTRXD-(Pin #8): 'This is the motor control line for all "
            "four disk drives.'): four alternating motor levels (OFF,ON,OFF,ON), each latched onto a different "
            "drive at that drive's OWN _sel turn-on edge (paula_floppy.v:618-659, four structurally identical "
            "always-blocks, one _motor input), read back distinctly through all four drives at once -- one "
            "shared cable line, four independently held motor states, wider than the accepted two-drive "
            "reading. RECORDED, not resolved: the appendix's own connector table names only three EXTERNAL "
            "select lines (SEL1B-/SEL2B-/SEL3B-) against this sentence's 'all four' "
            "[claims: opus:AHJ-078, sol:H-054]");
    }

    // ========================================================================
    // MTRXD-FOUR-INDEPENDENT-LATCHES-P370 -- p.370: "Thus, the system can
    // control all four motors using only one signal on the cable
    // (MTRXD-)." [claims: opus:AHJ-086, sol:H-061]
    //
    // Same RTL mechanism and same argument as the P369 block above,
    // measured independently for its own printed sentence on the
    // following page (a different reader, a different page citation) --
    // matching this suite's twin convention of grading each printed
    // citation under its own uid rather than reusing one measurement's
    // result for two claims.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("MTRXD-FOUR-INDEPENDENT-LATCHES-P370");
        r.select_with_motor(0, false);
        r.select_with_motor(1, true);
        r.select_with_motor(2, false);
        r.select_with_motor(3, mut ? false : true);
        bool d0 = r.peek_index(0);
        bool d1 = r.peek_index(1);
        bool d2 = r.peek_index(2);
        bool d3 = r.peek_index(3);
        checks.note("MTRXD-FOUR-INDEPENDENT-LATCHES-P370: twin of P369, same measurement -> " +
                    std::to_string(d0) + "," + std::to_string(d1) + "," + std::to_string(d2) + "," +
                    std::to_string(d3));
        checks.expect(d0 == false && d1 == true && d2 == false && d3 == true,
            "MTRXD-FOUR-INDEPENDENT-LATCHES-P370 (p.370: 'Thus, the system can control all four motors using "
            "only one signal on the cable (MTRXD-).'): twin measurement of MTRXD-FOUR-INDEPENDENT-LATCHES-P369 "
            "above (same RTL mechanism, independent printed citation on the following page) -- four alternating "
            "motor levels latched onto all four drives at their own edges and read back distinctly "
            "[claims: opus:AHJ-086, sol:H-061]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION FIXTURE-LIVE stimulus red: the drive is never actually selected, so the "asserts once selected" half is never exercised (MUTATE=FIXTURE-LIVE).
// MUTATION RDY-DISK-PRESENCE stimulus green: this assertion is RED at baseline (disk presence has no term in _ready's cone), so per the rule against mutating an already-failing check, the stimulus substitutes a genuine, independently-live change (deselecting the drive) for the disk-presence toggle, through the identical != comparator and the same label (MUTATE=RDY-DISK-PRESENCE).
// MUTATION INDEX-SEL-MOTOR-GATE stimulus red: the deselection step is skipped, so the drive stays selected and the masking condition (~_sel in the index gate) is never exercised (MUTATE=INDEX-SEL-MOTOR-GATE).
// MUTATION WRITE-DATA-SELECT-GATE stimulus red: the second trial selects the SAME drive as the first instead of a different one, so "selecting a different drive must move the request with it" is never exercised (MUTATE=WRITE-DATA-SELECT-GATE).
// MUTATION CHANGE-CLEAR-NEEDS-SELECT stimulus red: the drive is selected BEFORE the "deselected step" attempt, defeating the very condition under test (MUTATE=CHANGE-CLEAR-NEEDS-SELECT).
// MUTATION CHANGE-NO-CLEAR-NO-DISK stimulus red: the disk is reinserted BEFORE the "no diskette" clear attempt, defeating the condition under test (MUTATE=CHANGE-NO-CLEAR-NO-DISK).
// MUTATION MTRXD-FOUR-INDEPENDENT-LATCHES-P369 stimulus red: drive3 is latched to the SAME level as drive2 instead of the opposite one, so the four-way distinctness this assertion grades is never exercised (MUTATE=MTRXD-FOUR-INDEPENDENT-LATCHES-P369).
// MUTATION MTRXD-FOUR-INDEPENDENT-LATCHES-P370 stimulus red: same redirection as the P369 block, independent id (MUTATE=MTRXD-FOUR-INDEPENDENT-LATCHES-P370).
