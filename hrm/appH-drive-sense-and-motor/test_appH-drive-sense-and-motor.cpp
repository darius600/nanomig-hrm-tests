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
// NOT A RE-GRADE, CHECKED AGAINST EACH NAMED PROPOSITION'S OWN LABEL, READ
//   IN ITS SOURCE:
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
//     THE WHOLE TIME and never drives a step while DESELECTED at all.
//     p.369's own sentence adds a conjunct P-CHANGE-LATCH's source page
//     (p.244, "inserted AND a step pulse") does not carry: "the system
//     must select the drive, AND step the head" -- CHANGE-CLEAR-NEEDS-SELECT
//     grades exactly that added conjunct (a step while DESELECTED must not
//     clear the latch, only a step while SELECTED does), and
//     CHANGE-NO-CLEAR-NO-DISK grades a second conjunct neither
//     P-CHANGE-ON-REMOVE nor P-CHANGE-LATCH ever measures (disk absence
//     held across the clearing attempt). Different, narrower propositions.
//   - ch8-drive-sense-lines' opus:8-cia.c7 (P-READY, motor OFF, declined
//     there because p.244 does not define _ready with the motor off) and
//     appDE-disk-interface's P-READY-MOTOR-INVARIANT (the motor latched on
//     at a select edge, no disk installed, graded against p.334) both hold
//     disk presence fixed. RDY-DISK-PRESENCE below toggles DISK PRESENCE
//     (sdc_image_mounted/sdc_image_size) with selection AND motor BOTH
//     held fixed -- a third, independently drivable axis neither prior
//     assertion moves. See RDY-DISK-PRESENCE's own comment below.
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
//     (declined: absolute "1 us min.", no real-time unit at the fixture's
//     boundary) are all about STEPB*/DIRB*, signals no claim in this
//     package cites at all. Different signals entirely.
//   - hrm/ch8-dsklen-dma-arming reads `sdc_wr` in its
//     A8-WRITE-DIRECTION-BIT-AND-PREVENTION block, as one of three signals
//     proving a SINGLE DSKLEN write starts no DMA at all, and
//     hrm/ch8-disk-controller-overview reads the sibling `sdc_rd` as its
//     own central mechanism. ch8-dsklen-dma-arming's block never varies
//     `_sel` and never grades which drive's `sdc_wr` bit is set -- it is
//     about WHETHER a DMA arms at all (a single vs. double DSKLEN write),
//     not about SELECTION gating a per-drive request. Different
//     proposition; WRITE-DATA-SELECT-GATE below is the only assertion in
//     the suite that grades `sdc_wr`'s per-drive selectivity.
//
// WRITE STIMULUS. WRITE-DATA-SELECT-GATE drives a complete write-DMA
//   transfer through the DSKLEN register and DSKDAT bus writes, never an
//   internal signal: the sync word 0x4489 followed by 544 zero words (see
//   push_zero_sector()). With every word after the sync zero, the sector's
//   header and payload checksums are zero and its track field is 0, the
//   track a freshly reset, unstepped drive is on with SIDEB* inactive
//   (side 0, p.334 pin 13), so exact cycle alignment among the zero words
//   does not matter. The fixture then raises its per-drive write request,
//   `sdc_wr`, for the drive selected when the transfer completes and keeps
//   it up (this Rig never raises sdc_busy), so showing that the request
//   follows selection takes TWO complete transfers with different drives
//   selected.
//
// NOT GRADABLE ANYWHERE IN THIS PACKAGE (recorded via note(), never
//   graded): the RATE half of INDEX- ("once for each revolution"): no port
//   of the fixture carries a revolution time to grade it against; MTRXD-'s
//   printed 1.4 microsecond setup/hold figures (absolute real-time
//   intervals; the fixture counts clk7_en ticks and no port carries a
//   clock period, the same reason hrm/ch8-drive-step-timing declines its
//   own absolute figures); the appendix's own tension between "MTRXD- ...
//   for all four disk drives" and its connector table naming only three
//   EXTERNAL select lines (SEL1B-/SEL2B-/SEL3B-) -- recorded, not
//   resolved, same as ch8-drive-select-motor-led/appDE precedent for this
//   exact tension.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- one build serves the real run (MUTATE unset) and every
//   mutation run (MUTATE=<id>, one id per assertion). Per this package's
//   convention (matching hrm/ch8-drive-step-timing), each assertion's id
//   is its PROPOSITION NAME, not a claim uid, and each proposition's TWO
//   claim uids (one opus, one sol) are cited together in a `[claims: ...]`
//   suffix at the end of the label -- both count as ASSERTED by the graded
//   line. Every proposition below is GREEN at baseline except
//   RDY-DISK-PRESENCE and RDY-DISK-PRESENCE-DRIVE1, both RED (NM-HRM-080,
//   drive 0 and drive 1); each one's mutation substitutes a different,
//   independently-live stimulus (deselection, proven live by FIXTURE-LIVE
//   and FIXTURE-LIVE-DRIVE1 respectively) through the IDENTICAL comparator
//   and the SAME label, per this suite's rule against mutating an
//   already-failing check with a no-op.
//
// MUTATION RUN LOG (WSL/Verilator 5.052): baseline (MUTATE unset) 10
//   checks.expect() calls, 8 passed / 2 failed -- the 2 failures are
//   RDY-DISK-PRESENCE and RDY-DISK-PRESENCE-DRIVE1. Each of
//   MUTATE=FIXTURE-LIVE, FIXTURE-LIVE-DRIVE1, INDEX-SEL-MOTOR-GATE,
//   WRITE-DATA-SELECT-GATE, CHANGE-CLEAR-NEEDS-SELECT,
//   CHANGE-NO-CLEAR-NO-DISK, MTRXD-FOUR-INDEPENDENT-LATCHES-P369 and
//   MTRXD-FOUR-INDEPENDENT-LATCHES-P370, diffed line by line against the
//   baseline's PASS/FAIL output, moves EXACTLY the targeted label's own
//   line (and no other) from PASS to FAIL (7 passed / 3 failed per run,
//   the RDY-DISK-PRESENCE pair's baseline failures unaffected).
//   MUTATE=RDY-DISK-PRESENCE moves exactly that line from FAIL to PASS (9
//   passed / 1 failed, RDY-DISK-PRESENCE-DRIVE1 unaffected); MUTATE=
//   RDY-DISK-PRESENCE-DRIVE1 moves exactly that line from FAIL to PASS (9
//   passed / 1 failed, RDY-DISK-PRESENCE unaffected). Nothing else moves in
//   either run.

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
// and hrm/ch8-drive-select-motor-led): the drive's head position and its
// step-rate counter survive a reset, and an armed disk DMA stays armed.
// Every test block below constructs its own fresh Rig; none is reused
// across trials.
struct Rig {
    Vpaula_floppy* d;

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
        d->side = 1;    // SIDEB* inactive: side 0 (p.334 pin 13), see WRITE STIMULUS
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
    // GENUINE turn-on edge, in p.369's order: deselect first and settle one
    // tick, THEN set the motor level and select -- the following tick() is
    // the off-to-on SELxB- transition that p.369 has clock MTRXD- into the
    // drive's motor flip-flop.
    void select_with_motor(int n, bool motor_on_level) {
        deselect_all();
        tick();
        d->_motor = motor_on_level ? 0 : 1;  // DSKMOTOR* is active low
        select(n);
        tick();
    }

    // Read drive n's motor latch through the `index` output, FORCING _sel
    // to select n and raising sof during the read, with NO clock edge, so
    // the read itself changes no latched state (tick() never raises sof).
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

    // One register write, held on the bus for four clk7_en ticks. For
    // DSKLEN, which must be written twice with bit 15 set to start a disk
    // DMA, one held call is enough: WRITE-DATA-SELECT-GATE's transfers
    // complete and raise sdc_wr (its MEASURED line).
    void write_reg(int addr, uint16_t data) {
        d->reg_address_in = (uint8_t)((addr >> 1) & 0xFF);
        d->data_in = data;
        ticks(4);
        d->reg_address_in = 0xFF;
        d->data_in = 0;
    }

    // Arm a disk DMA WRITE (DSKLEN bit 14 set) on whichever drive is
    // currently selected, with a length longer than the transfer
    // push_zero_sector() pushes, so the DMA stays on for all of it.
    void arm_dma_write(int length) {
        d->enable = 1;
        write_reg(0x024, (uint16_t)(0x8000 | 0x4000 | (length & 0x3FFF)));
        ticks(20);
    }

    // One DSKDAT bus write, held for EXACTLY one clk7_en tick, so exactly
    // one word enters the disk FIFO; holding the address longer would push
    // the SAME data_in value again on every held tick.
    void push_dskdat_word(uint16_t w) {
        d->reg_address_in = 0x13;  // DSKDAT (9'h026) >> 1
        d->data_in = w;
        tick();
        d->reg_address_in = 0xFF;
        d->data_in = 0;
    }

    // Push one full write-DMA sector stream: the sync word (0x4489, the
    // value the fixture matches after reset) followed by 544 zero words --
    // 24 header/header-checksum words, 4 payload-checksum-preamble words,
    // 512 payload words and 4 margin words so the fixture registers the
    // complete sector. Every word past the sync is zero, so no exact cycle
    // alignment among them matters -- see WRITE STIMULUS in the file
    // header.
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
    // FIXTURE-LIVE -- no claim id, no manual page: the fixture must deliver
    // a known, distinct word before any read-based assertion below is
    // trusted. Measured on
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
            "it -- the sense-line read path is live, not stuck, and selection is a "
            "genuinely live, independent stimulus before any claim below is trusted");
    }

    // ========================================================================
    // RDY-DISK-PRESENCE -- p.368 RDY-: "When a disk drive's motor is on,
    // this line indicates the selected disk is installed and rotating at
    // speed." [claims: opus:AHJ-060, sol:H-036]
    //
    // A THIRD AXIS, not a re-grade: hrm/ch8-drive-sense-lines' opus:8-cia.c7
    // is declined because p.244 does not define _ready with the motor off.
    // hrm/appDE-disk-interface's P-READY-MOTOR-INVARIANT latches the motor
    // on at a select edge and reads RDY* with no disk installed, against
    // p.334. Neither varies disk presence. This page's motor-ON branch
    // names DISK PRESENCE as what the line indicates ("installed"), and
    // disk presence (a mounted or unmounted image, sdc_image_mounted/
    // sdc_image_size) is a THIRD, independently drivable axis this
    // assertion moves with selection AND motor BOTH held fixed -- an axis
    // no accepted assertion anywhere in the suite moves against _ready.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("RDY-DISK-PRESENCE");
        r.select_with_motor(0, /*motor_on_level=*/true);  // motor latched ON at a genuine select edge, p.369's order
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
            "disk-present to realise the printed claim -- with the motor on, p.368 has the line indicate "
            "whether a disk is installed, so removing or installing one must change it. This is a THIRD axis, "
            "distinct from hrm/appDE-disk-interface's no-disk reading of p.334 and from the page-244 claim "
            "declined for the motor's OWN off-state being undefined: here selection and motor are both fixed "
            "and only disk presence varies, which no accepted assertion moves against "
            "_ready [claims: opus:AHJ-060, sol:H-036]");
    }

    // ========================================================================
    // FIXTURE-LIVE-DRIVE1 -- no claim id, no manual page: RDY-DISK-PRESENCE-
    // DRIVE1 below reads drive 1, whose _ready term only responds to
    // selection once floppy_drives configures an extra drive present (the
    // drive-count input, sampled at reset). Measured the same way
    // FIXTURE-LIVE measured drive 0 above: before trusting a read on drive
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
            "before RDY-DISK-PRESENCE-DRIVE1 below is trusted");
    }

    // ========================================================================
    // RDY-DISK-PRESENCE-DRIVE1 -- p.368 RDY-, same sentence as
    // RDY-DISK-PRESENCE above, corroborated on drive 1: pp.334/368-370
    // describe drives beyond drive 0 (the external drives 1-3), and
    // RDY-DISK-PRESENCE alone only exercises drive 0 -- the followup this
    // package answers asks for the same grading on drive 1. floppy_drives=1
    // (sampled at reset, see the Rig constructor) is what makes drive 1
    // exist for this fixture; FIXTURE-LIVE-DRIVE1 above shows it does.
    // [claims: opus:AHJ-060, sol:H-036]
    // ========================================================================
    {
        Rig r(1);  // floppy_drives=1: drive 1 present, see FIXTURE-LIVE-DRIVE1
        bool mut = mutating("RDY-DISK-PRESENCE-DRIVE1");
        r.select_with_motor(1, /*motor_on_level=*/true);
        r.unmount(1);     // disk absent
        bool ready_absent = r.ready();
        bool ready_present;
        if (!mut) {
            r.mount(1, 1000);  // disk present -- the real stimulus p.368 says should matter
            ready_present = r.ready();
        } else {
            // MUTATION (rule against mutating an already-failing check):
            // substitute a genuine, independently-live stimulus change
            // (deselecting the drive, proven live by FIXTURE-LIVE-DRIVE1
            // above) for the disk-presence toggle, through the IDENTICAL !=
            // comparator and the same label.
            r.deselect_all();
            ready_present = r.ready();
        }
        checks.note("RDY-DISK-PRESENCE-DRIVE1: drive1 selected, motor ON, disk absent -> ready=" +
                    std::to_string(ready_absent) + "; then " +
                    std::string(mut ? "deselected (mutated substitute)" : "disk mounted, still selected") +
                    " -> ready=" + std::to_string(ready_present));
        checks.expect(ready_absent != ready_present,
            "RDY-DISK-PRESENCE-DRIVE1 (p.368, RDY-: 'When a disk drive's motor is on, this line indicates the "
            "selected disk is installed and rotating at speed.'): with drive 1 configured present, selection "
            "and motor both held fixed (motor ON, drive 1 selected throughout), _ready must read differently "
            "for disk-absent than for disk-present to realise the printed claim on a drive other than drive 0 "
            "[claims: opus:AHJ-060, sol:H-036]");
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
    // The RATE ("once for each revolution") is reported separately via
    // note(), not graded -- see NOT GRADABLE in the file header.
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
        // MUTATION: never actually deselect -- the "selected drive"
        // condition of p.369's sentence is never exercised.
        if (!mut) r.deselect_all();
        bool no_pulse_deselected = !r.read_index_now();
        checks.note("INDEX-SEL-MOTOR-GATE: selected+motor-on pulses=" + std::to_string(pulses_selected_motor_on) +
                    "; selected+motor-off pulses=" + std::to_string(!no_pulse_motor_off) + "; " +
                    std::string(mut ? "still selected (mutated)" : "deselected") + "+motor-on pulses=" +
                    std::to_string(!no_pulse_deselected));
        checks.expect(pulses_selected_motor_on && no_pulse_motor_off && no_pulse_deselected,
            "INDEX-SEL-MOTOR-GATE (p.369, INDEX-(Pin #22): 'A selected drive pulses this signal low once for "
            "each revolution of its motor.'): index pulses only for a SELECTED drive whose motor is latched "
            "on -- a selected, motor-running drive "
            "pulses; the same drive deselected does not; the same drive selected with its motor latched off "
            "does not. The RATE ('once for each revolution') is not graded here -- see the MEASURED note below "
            "[claims: opus:AHJ-065, sol:H-040]");
        checks.note("INDEX rate (not graded): no port of this fixture carries a revolution time, so 'once for "
                    "each revolution' has nothing at this boundary to be graded against -- a rate taken from the "
                    "fixture itself would make the fixture its own oracle");
    }

    // ========================================================================
    // WRITE-DATA-SELECT-GATE -- p.369 DKWDB-(Pin #17)/DKWEB-(Pin #16): "The
    // system drives write data to all disks via this signal. The data is
    // only written when DKWEB- is active (low). Data is written only to
    // selected drives." [claims: opus:AHJ-072, sol:H-048]
    //
    // ch8-dsklen-dma-arming reads `sdc_wr` too, but only as a control
    // proving a SINGLE DSKLEN write arms no DMA at all -- it never varies
    // `_sel` and never grades which drive's bit is set (see the file
    // header). This block is the only one in the suite that grades
    // `sdc_wr`'s per-drive selectivity. A full write-DMA transfer is driven
    // through DSKLEN/DSKDAT (see WRITE STIMULUS in the file header) so that
    // the fixture completes the write and raises its request on `sdc_wr`.
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
            "transfer with drive 1 selected instead requests drive 1 alone (sdc_wr==4'b0010) -- the write "
            "request goes to exactly the selected drive and moves with selection, never touching an unselected "
            "drive's bit "
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
        // _change reads deasserted whenever no drive is selected (the same
        // masking SEL-MASKS-TRACK0 measures for _track0) -- reselect
        // (WITHOUT another step) before reading, so the read reports the
        // latch rather than the deselected output. Reselecting is not a
        // step: _step stays at its idle level, so by p.369 it cannot clear
        // the latch itself.
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
            "does -- the 'select the drive' conjunct p.369 adds over the accepted P-CHANGE-LATCH reading "
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
        r.unmount(0);  // disk absent -- changed, and by p.369 it stays changed while no diskette is installed
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
    // RECORDED, NOT RESOLVED: the appendix says MTRXD- serves "all four
    // disk drives" while its own connector table names only three
    // EXTERNAL select lines (SEL1B-, SEL2B-, SEL3B- at pins 21, 9 and 20)
    // -- this package records the tension and grades the four independent
    // latches, which is what the fixture's boundary presents (four select
    // lines, one motor line).
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
            "drive at that drive's OWN _sel turn-on edge, read back distinctly through all four drives at once -- one "
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
    // Same mechanism and same argument as the P369 block above,
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
            "above (same mechanism, independent printed citation on the following page) -- four alternating "
            "motor levels latched onto all four drives at their own edges and read back distinctly "
            "[claims: opus:AHJ-086, sol:H-061]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION FIXTURE-LIVE stimulus red: the drive is never actually selected, so the "asserts once selected" half is never exercised (MUTATE=FIXTURE-LIVE).
// MUTATION RDY-DISK-PRESENCE stimulus green: this assertion is RED at baseline (_ready reads the same with and without a disk installed), so per the rule against mutating an already-failing check, the stimulus substitutes a genuine, independently-live change (deselecting the drive) for the disk-presence toggle, through the identical != comparator and the same label (MUTATE=RDY-DISK-PRESENCE).
// MUTATION FIXTURE-LIVE-DRIVE1 stimulus red: drive 1 is never actually selected, so the "asserts once selected" half is never exercised on drive 1 (MUTATE=FIXTURE-LIVE-DRIVE1).
// MUTATION RDY-DISK-PRESENCE-DRIVE1 stimulus green: this assertion is RED at baseline (_ready reads the same on drive 1 with and without a disk installed, same defect as RDY-DISK-PRESENCE), so per the rule against mutating an already-failing check, the stimulus substitutes a genuine, independently-live change (deselecting the drive) for the disk-presence toggle, through the identical != comparator and the same label (MUTATE=RDY-DISK-PRESENCE-DRIVE1).
// MUTATION INDEX-SEL-MOTOR-GATE stimulus red: the deselection step is skipped, so the drive stays selected and the "selected drive" condition is never exercised (MUTATE=INDEX-SEL-MOTOR-GATE).
// MUTATION WRITE-DATA-SELECT-GATE stimulus red: the second trial selects the SAME drive as the first instead of a different one, so "selecting a different drive must move the request with it" is never exercised (MUTATE=WRITE-DATA-SELECT-GATE).
// MUTATION CHANGE-CLEAR-NEEDS-SELECT stimulus red: the drive is selected BEFORE the "deselected step" attempt, defeating the very condition under test (MUTATE=CHANGE-CLEAR-NEEDS-SELECT).
// MUTATION CHANGE-NO-CLEAR-NO-DISK stimulus red: the disk is reinserted BEFORE the "no diskette" clear attempt, defeating the condition under test (MUTATE=CHANGE-NO-CLEAR-NO-DISK).
// MUTATION MTRXD-FOUR-INDEPENDENT-LATCHES-P369 stimulus red: drive3 is latched to the SAME level as drive2 instead of the opposite one, so the four-way distinctness this assertion grades is never exercised (MUTATE=MTRXD-FOUR-INDEPENDENT-LATCHES-P369).
// MUTATION MTRXD-FOUR-INDEPENDENT-LATCHES-P370 stimulus red: same redirection as the P369 block, independent id (MUTATE=MTRXD-FOUR-INDEPENDENT-LATCHES-P370).
