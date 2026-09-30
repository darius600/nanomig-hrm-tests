// Test purpose: HRM conformance package for the four drive-side sense lines
//   printed on p.244 -- DSKRDY* (PA5), DSKTRACK0* (PA4), DSKPROT* (PA3),
//   DSKCHANGE* (PA2) -- their polarity, their stated preconditions, and the
//   outward-step block at track zero, against paula_floppy (this suite's
//   only model of a physical Amiga floppy drive). 10 claims, from
//   work/ch8-s2-build/ch8-drive-sense-lines.jsonl. Full derivation in
//   SPECIFICATION.md (committed before this file or
//   paula_floppy.v was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.244 (PA5
//   DSKRDY*, PA4 DSKTRACK0*, PA3 DSKPROT*, PA2 DSKCHANGE*; full quotations
//   in SPECIFICATION.md).
// This package's specific subject, named by the build task:
//   C4 -- opus:8-cia.c7 (_ready with the motor off): p.244, three lines
//         below the sentence this claim cites, reads: "This signal is only valid
//         when the motor is ON, at other times configuration information
//         may obscure the meaning of this input." The page itself declines
//         to say what _ready means with the motor off, so a check requiring
//         a particular value there would test a case the page expressly refuses
//         to define. DECLINED via record(), not graded through expect() --
//         see the opus:8-cia.c7 block below. The RTL fact this claim
//         rests on (paula_floppy.v:688-694 never consults _motor) is real
//         and is kept as a note(), not a finding against this page.
//   C5 -- _track0 latency is 0 ticks, purely combinational; no claim among
//         this package's 10 cites the "4 ms max" figure, so it is measured
//         and reported via note(), not graded.
//   C17 -- _track0 is masked (forced deasserted) whenever no drive is
//         selected, regardless of head position. Every read below selects
//         first; SEL-MASKS-TRACK0 grades the masking itself.
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-drive-sense-lines.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED:
//   1. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   2. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file, to confirm
//      it instantiates nothing else and the two-file build closes (it does;
//      the only sub-instance in paula_floppy.v is this fifo, `db1`).
//
// MECHANISM, confirmed by reading paula_floppy.v myself (SPECIFICATION.md
//   had only the classify stage's disclosed, not-yet-verified citations):
//
//   _ready (paula_floppy.v:688-694):
//     assign _ready = (_sel[3] | ~(drives[1]&drives[0]))
//                    & (_sel[2] | ~drives[1])
//                    & (_sel[1] | ~(drives[1]|drives[0]))
//                    & (_sel[0]);
//   `drives` (paula_floppy.v:556-558) latches from the `floppy_drives` input
//   only at reset. With floppy_drives=0 (one drive configured, this
//   fixture's setting), every term but the last collapses to a constant 1,
//   so `_ready` reduces to exactly `_sel[0]` -- purely a function of
//   selection. `_motor` does not appear anywhere in this expression, and
//   does not appear anywhere in `motor_on`'s cone either (motor_on feeds
//   only `index` and `syncint`, neither read by this package). The file's
//   own changelog (line 54) documents this as deliberate: "2008-09-24 -
//   incompatibility found: _READY signal should respond to _SELx even when
//   the motor is off" -- a genuine, deliberate compatibility choice in the
//   implementation, and an interesting measured fact, but NOT a conformance
//   finding against p.244: that page's own next sentence declines to define
//   _ready's value with the motor off at all, so this RTL behaviour cannot
//   contradict a value the page never printed. Recorded as a decline, not
//   graded.
//
//   _track0 (paula_floppy.v:666,683,671-680):
//     assign _selx = &_sel[3:0];                    // true iff NO drive selected
//     assign _dsktrack0 = ~(dsktrack[sel]==0);       // true iff selected drive NOT at 0
//     assign _track0 = &(_selx | _dsktrack0);        // = _selx | _dsktrack0 (scalar)
//   So _track0 reads deasserted (1) whenever no drive is selected at all
//   (_selx=1), REGARDLESS of dsktrack -- this is C17's masking, confirmed
//   exactly. It also reads deasserted whenever the selected drive's counter
//   is nonzero. It reads asserted (0) only when some drive is selected AND
//   that drive's dsktrack==0.
//   `dsktrack` (paula_floppy.v:671-680) updates synchronously, gated by
//   `!_selx && _step && !_step_del && step_ena`:
//     - direc=0, not at last track (dsktracklast, ==82): increment.
//     - else if direc=1 AND _dsktrack0 (i.e. NOT currently at track 0):
//       decrement.
//   At track 0, _dsktrack0 is false, so a direc=1 (outward/decrement)
//   attempt matches neither branch: the counter simply does not move.
//   This is the refusal C15 grades -- confirmed by direct reading, not
//   merely disclosed evidence.
//   `dsktrack` has NO reset term anywhere in this always block (confirmed
//   by reading it myself) -- Verilator zero-initialises it, so a FRESH
//   model starts at track 0, but a reused one would carry stale position.
//   `step_ena_cnt`/`step_ena` (paula_floppy.v:594-603) ALSO has no reset
//   term; step_ena = step_ena_cnt[8] (>=256 clk7_en ticks since the last
//   accepted-or-attempted edge). Since every trial below allocates a fresh
//   model, step_ena_cnt starts zero-initialised at 0 and must saturate
//   before the first step in each trial.
//   Because _track0 is a continuous assign off a synchronously-updated
//   register with no extra pipeline stage, its latency relative to the
//   clk7_en tick that updates dsktrack is 0 additional ticks -- measured
//   directly in the C5 block below (finding candidate C5; no claim among
//   this package's 10 cites the "4 ms max" figure this candidate is about,
//   so it is reported via note(), not graded).
//
//   _wprot (paula_floppy.v:664,969):
//     assign disk_writable = floppy_wrprot ? 4'b0000 : 4'b1111;
//     assign _wprot = &(_sel | disk_writable);
//   With the drive selected and floppy_wrprot=1, _wprot reads asserted
//   (protected); with floppy_wrprot=0, deasserted, regardless of selection.
//
//   _change / _disk_change (paula_floppy.v:610,662,959-966):
//     assign _change = &(_sel | _disk_change);   // = _disk_change[sel] when selected
//     _disk_change <= (_disk_change | ~_sel & {4{_step}} & ~{4{_step_del}}
//                       & disk_present) & ~({4{reset}} | ~disk_present);
//   `_disk_change[i]` is an internally-active-HIGH "stable" latch (1 =
//   no pending change, so _change reads deasserted when selected): it is
//   forced to 0 (asserted/"changed") whenever reset is active OR
//   disk_present[i] is false (image absent, drops via `disk_present[i] <=
//   |sdc_image_size`, paula_floppy.v:959-966, clocked on every plain `clk`
//   edge with no clk7_en gate); it is set/held at 1 only while NOT reset,
//   disk present, and (it was already 1, OR a genuine _step rising edge just
//   occurred while that drive was selected AND disk_present). Reinsertion
//   alone (disk_present returning to 1 with no fresh step edge) does NOT
//   set the OR-term, confirmed by direct reading -- matching the page's
//   compound "inserted AND a step pulse" condition exactly, not either half
//   alone.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- one build serves the real run (MUTATE unset) and every
//   mutation run (MUTATE=<id>, one id per assertion). Ids are the claim's
//   own `classify.uid` for the 9 remaining claim-based assertions (this
//   suite's chapter-8 convention) and a plain uppercase id for FIXTURE-LIVE,
//   READY-SEL-CONTROL and SEL-MASKS-TRACK0, which carry no manual citation.
//   `opus:8-cia.c7` (C4) carries NO `expect()` and NO mutation record: it is
//   DECLINED via `record()` (see that block below), per this suite's rule
//   that `record()` is the only place a declined uid may appear and a uid on
//   a graded line counts as ASSERTED regardless of the words around it.
//   `opus:8-cia.c7` requires a value p.244 itself declines to define with the
//   motor off, so grading it would test more than the page states -- a
//   decline, not a red finding, so it carries no graded check or mutation;
//   READY-SEL-CONTROL alone remains as the live proof that
//   `_ready`'s port is not dead or stuck.
//
// MUTATION RUN LOG (measured, WSL/Verilator 5.052 -- see the report
//   accompanying this commit for the full per-assertion table):
//   Baseline (MUTATE unset): 12 checks.expect() calls, 12 passed, 0 failed,
//   plus 1 RECORDED decline (opus:8-cia.c7). Each of MUTATE=FIXTURE-LIVE,
//   READY-SEL-CONTROL, opus:8-cia.c12, sol:8-cia.c10, SEL-MASKS-TRACK0,
//   opus:8-cia.c15, sol:8-cia.c15, opus:8-cia.c17, opus:8-cia.c19,
//   sol:8-cia.c18, opus:8-cia.c20, sol:8-cia.c19 was run and confirmed
//   11 passed / 1 failed (only the targeted label's line flipped from PASS
//   to FAIL among the 12 baseline-green checks). All 12 runs, individually,
//   moved exactly one line relative to baseline -- no cross-contamination
//   between assertions, confirming each comparator is genuinely sensitive to
//   its own targeted stimulus.

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

// RIG HAZARD (SPECIFICATION.md, build task): dsktrack (paula_floppy.v:671-
// 680) and step_ena_cnt (paula_floppy.v:594-603) both have no reset term, so
// a reused Verilated model would carry stale head position and stale
// step-spacing state forward. Rig allocates a FRESH Vpaula_floppy per
// instance; every test block below constructs its own Rig and none is
// reused for a second trial.
struct Rig {
    Vpaula_floppy* d;

    Rig() {
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
        d->floppy_drives = 0;  // one drive configured -- keeps the _ready
                                // equation a pure function of _sel[0] alone
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

    // One clk7_en-enabled clock edge. clk7_en is held permanently high, so
    // one call here is exactly one "clk7_en tick" in the sense the claims
    // file measures ticks in.
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

    // Select drive n (active low: one bit per drive). Settles the
    // combinational outputs (_selx, _track0, _ready, _wprot, _change)
    // immediately.
    void select(int n) {
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        d->eval();
    }
    void deselect_all() { d->_sel = 0xF; d->eval(); }

    // step_ena_cnt (paula_floppy.v:594-603) is a 9-bit saturating counter,
    // no reset term of its own, gating every accepted _step rising edge
    // behind a minimum of 256 clk7_en ticks since the last edge -- ACCEPTED
    // OR NOT. Idle long enough for it to saturate before a step this file
    // needs accepted.
    void saturate_step_ena() { ticks(300); }

    // One _step pulse: low for one tick (so _step_del catches down to 0,
    // producing a genuine detectable edge), then high for two ticks (the
    // rising edge is processed on the first of these, per paula_floppy.v's
    // non-blocking _step_del assignment; the second is settle margin).
    // direc is set before the low phase and held through the edge.
    void do_step(bool direc_outward) {
        d->direc = direc_outward ? 1 : 0;  // direc=1 decrements (outward,
                                            // per paula_floppy.v:676); direc=0
                                            // increments (paula_floppy.v:674)
        d->_step = 0;
        tick();
        d->_step = 1;
        tick();  // the rising edge is processed on this tick
        tick();  // settle
    }

    bool track0() { d->eval(); return d->_track0 == 0; }  // active low
    bool ready()  { d->eval(); return d->_ready == 0; }   // active low
    bool wprot()  { d->eval(); return d->_wprot == 0; }   // active low
    bool change() { d->eval(); return d->_change == 0; }  // active low

    // Mount an image on drive n: disk_present[n] latches to |sdc_image_size
    // (paula_floppy.v:959-966) on the next plain `clk` edge while
    // sdc_image_mounted[n] is held high -- that block is NOT clk7_en-gated.
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
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    std::cout << "Starting ch8-drive-sense-lines...\n";

    // ========================================================================
    // FIXTURE-LIVE -- no claim id, no manual page: this suite's own
    // requirement that the fixture prove it can deliver a known, distinct
    // word before any read-based assertion below is trusted. Measured on
    // _wprot, the simplest single-cycle case of the shared sense-line output
    // mux every one of this package's four ports uses.
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
    // READY-SEL-CONTROL -- this package's mandatory control case, on the
    // SAME _ready port/comparator the declined opus:8-cia.c7 citation below
    // reads: proof that _ready is a live signal that responds to something
    // real (selection), so that citation's decline is about the page's own
    // silence on the motor-off case, not about a dead or stuck port.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("READY-SEL-CONTROL");
        r.deselect_all();
        bool ready_deselected = r.ready();
        // MUTATION: never actually select the drive -- the "asserts once
        // selected" half is never exercised.
        if (!mut) r.select(0);
        bool ready_selected = r.ready();
        checks.note("READY-SEL-CONTROL: deselected -> _ready asserted=" + std::to_string(ready_deselected) +
                    ", " + std::string(mut ? "still deselected (mutated)" : "selected") +
                    " -> _ready asserted=" + std::to_string(ready_selected));
        checks.expect(!ready_deselected && ready_selected,
            "READY-SEL-CONTROL (control case, no manual citation, same _ready port/comparator the declined "
            "motor-off claim below reads): deselecting every drive deasserts _ready and selecting a configured "
            "drive asserts it -- paula_floppy.v:688-694 -- proving _ready is live and responds to selection, so "
            "that claim's decline (below) is about the page's own silence on the motor-off case, not a dead or "
            "stuck port");
    }

    // ========================================================================
    // opus:8-cia.c7 (p.244) -- P-READY, motor OFF. DECLINED, not
    // graded: finding candidate C4 would test MORE than its cited page
    // states. p.244, three lines below the sentence this claim cites, reads:
    // "This signal is only valid when the motor is ON, at other times
    // configuration information may obscure the meaning of this input." The
    // page expressly declines to define _ready's value with the motor off,
    // so a check requiring a particular value in that state is not a
    // conformance test of anything the page prints. Per this suite's rule,
    // `record()` is the only place a declined uid may appear -- this uid is
    // no longer on any `expect()` line and carries no mutation record.
    // ========================================================================
    {
        Rig r;
        r.d->_motor = 1;  // active low: 1 = motor OFF
        r.select(0);
        bool ready_state = r.ready();
        checks.note("opus:8-cia.c7: drive0 selected, _motor=OFF -> _ready asserted(ready)=" +
                    std::to_string(ready_state) + " -- measured for interest only, not graded");
        checks.record(
            "opus:8-cia.c7 (p.244): the cited sentence ('The drive will pull this line low when the motor is "
            "known to be rotating at full speed.') is followed, three lines later on the same printed page, by "
            "'This signal is only valid when the motor is ON, at other times configuration information may "
            "obscure the meaning of this input.' The page itself declines to define _ready's value with the "
            "motor off, so a check requiring _ready deasserted in that state tests a case the page expressly "
            "refuses to specify -- more than the page states. Declined, not graded. Measured for interest only: "
            "paula_floppy.v:688-694 computes _ready from _sel and the configured drive count alone (no _motor "
            "term anywhere in its cone; drive0 selected, motor off, reads " + std::string(!ready_state ? "NOT " : "") +
            "asserted here), and the module's own 2008-09-24 changelog entry documents this as a deliberate "
            "compatibility choice ('_READY signal should respond to _SELx even when the motor is off'), not an "
            "oversight -- a real, interesting fact about NanoMig, but not a conformance failure against a page "
            "that names no expected value for this case.");
    }

    // ========================================================================
    // opus:8-cia.c12 / sol:8-cia.c10 (p.244, twins) -- P-TRACK0-ZERO.
    // Digital half: selected drive's dsktrack==0 -> _track0 asserted.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("opus:8-cia.c12");
        r.select(0);
        bool at_zero_initially = r.track0();
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/false);  // increment: leaves track 0
        bool left_zero = !r.track0();
        r.saturate_step_ena();
        // MUTATION: step away AGAIN instead of back toward zero, so the
        // drive never returns to track 0 and the final read is checked
        // against a state that never occurred.
        r.do_step(/*direc_outward=*/mut ? false : true);
        bool returned_to_zero = r.track0();
        checks.note("opus:8-cia.c12: at start=" + std::to_string(at_zero_initially) +
                    ", after outward step (left zero)=" + std::to_string(left_zero) +
                    ", after " + std::string(mut ? "a second outward (mutated)" : "inward") +
                    " step=" + std::to_string(returned_to_zero));
        checks.expect(at_zero_initially && left_zero && returned_to_zero,
            "opus:8-cia.c12 (p.244, quote 'The drive will pull this line low when the disk heads are positioned "
            "over track zero.'): with the drive selected, _track0 reads asserted at dsktrack==0, deasserted once "
            "stepped away, and reasserted once stepped back -- paula_floppy.v:666 (_track0=&(_selx|_dsktrack0)) "
            "with :683 (_dsktrack0=~(dsktrack[sel]==0))");
    }
    {
        Rig r;
        bool mut = mutating("sol:8-cia.c10");
        r.select(0);
        bool at_zero_initially = r.track0();
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/false);
        bool left_zero = !r.track0();
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/mut ? false : true);
        bool returned_to_zero = r.track0();
        checks.note("sol:8-cia.c10: twin of opus:8-cia.c12, same measurement -> at start=" +
                    std::to_string(at_zero_initially) + ", left zero=" + std::to_string(left_zero) +
                    ", final=" + std::to_string(returned_to_zero));
        checks.expect(at_zero_initially && left_zero && returned_to_zero,
            "sol:8-cia.c10 (p.244, quote 'The drive will pull this line low when the disk heads are positioned "
            "over track zero.'): twin of opus:8-cia.c12 (same printed quote, independent reader) -- same "
            "measurement, graded independently under its own uid");
    }

    // ========================================================================
    // C5 (informational -- no claim among this package's 10 cites the "4 ms
    // max" figure this candidate is about; measured and reported, not
    // graded). _track0 is purely combinational off dsktrack, so its latency
    // relative to the clk7_en tick that updates dsktrack is measured here.
    // ========================================================================
    {
        Rig r;
        r.select(0);
        r.saturate_step_ena();
        r.d->direc = 0;  // increment, leaves track 0
        r.d->_step = 0;
        r.tick();
        r.d->_step = 1;
        r.tick();  // the state-changing tick: dsktrack increments AND
                   // _track0 (purely combinational) reflects it within this
                   // SAME tick -- no extra tick before reading it below.
        bool track0_immediately_after_state_change = r.track0();
        checks.note("C5: _track0 read immediately after the SAME clk7_en tick that updated dsktrack (no extra "
                    "settle tick) already reads deasserted=" +
                    std::to_string(!track0_immediately_after_state_change) +
                    " -- measured latency 0 ticks; the printed '4 ms max' to TRACK 00 has no mechanism in this "
                    "RTL to bound (finding candidate C5)");
    }

    // ========================================================================
    // SEL-MASKS-TRACK0 -- C17's masking property: real, measured, but not
    // printed on the page, so it carries no citation. With the drive
    // genuinely at track zero, deselecting it is expected to read _track0
    // deasserted anyway (the mask, not the track-zero state).
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("SEL-MASKS-TRACK0");
        r.select(0);
        bool asserted_while_selected_at_zero = r.track0();
        // MUTATION: never actually deselect -- stays selected, so the
        // masking condition (_selx) is never true and this run cannot
        // distinguish the mask from the genuine track-zero state.
        if (!mut) r.deselect_all();
        bool reading_after_maybe_deselect = r.track0();
        r.select(0);
        bool reasserted_once_reselected = r.track0();
        checks.note("SEL-MASKS-TRACK0: selected at zero=" + std::to_string(asserted_while_selected_at_zero) +
                    ", " + std::string(mut ? "still selected (mutated)" : "deselected") +
                    " at zero -> _track0 asserted=" + std::to_string(reading_after_maybe_deselect) +
                    ", reselected -> _track0 asserted=" + std::to_string(reasserted_once_reselected));
        checks.expect(asserted_while_selected_at_zero && !reading_after_maybe_deselect && reasserted_once_reselected,
            "SEL-MASKS-TRACK0 (C17, real property, no manual citation): with the drive genuinely at track zero, "
            "deselecting it reads _track0 deasserted anyway -- the deselected mask, not the track-zero state -- "
            "and reselecting reads it reasserted -- paula_floppy.v:666 (_track0=&(_selx|_dsktrack0)), _selx="
            "&_sel[3:0] forces the deasserted reading whenever no drive is selected, independent of dsktrack");
    }

    // ========================================================================
    // opus:8-cia.c15 / sol:8-cia.c15 (p.244, twins) -- P-TRACK0-REFUSE-
    // OUTWARD. An outward (decrement) step attempted genuinely at track
    // zero leaves the counter at zero; the same decrement direction is
    // shown, in the same trial, to work normally away from the boundary.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("opus:8-cia.c15");
        r.select(0);
        r.saturate_step_ena();
        // MUTATION: move away from zero FIRST, so the "genuinely at track
        // zero" precondition this proposition requires is never true when
        // the outward step is attempted below.
        if (mut) {
            r.do_step(/*direc_outward=*/false);
            r.saturate_step_ena();
        }
        bool at_zero_before_attempt = r.track0();
        r.do_step(/*direc_outward=*/true);  // outward/decrement attempt
        bool still_at_zero_after_attempt = r.track0();
        // Control half, same trial: from a genuine non-zero position, the
        // same decrement direction DOES work -- rules out a dead/always-
        // refusing decrement path.
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/false);  // away from zero again
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/true);   // decrement back
        bool decrement_works_away_from_zero = r.track0();
        checks.note("opus:8-cia.c15: at zero before outward attempt=" + std::to_string(at_zero_before_attempt) +
                    std::string(mut ? " (mutated: moved away first)" : "") +
                    ", after outward attempt=" + std::to_string(still_at_zero_after_attempt) +
                    ", decrement from a genuine non-zero position=" +
                    std::to_string(decrement_works_away_from_zero));
        checks.expect(at_zero_before_attempt && still_at_zero_after_attempt && decrement_works_away_from_zero,
            "opus:8-cia.c15 (p.244, quote 'Software must not attempt to step outwards when this signal is "
            "active. ... All new drives must refuse to step outward in this condition.'): an outward (decrement) "
            "step attempted while genuinely at track zero leaves the counter at zero -- paula_floppy.v:673-680, "
            "the decrement arm `else if (_dsktrack0 && direc) dsktrack[sel]<=dsktrack[sel]-1` is gated by "
            "_dsktrack0 (false at track zero) -- while the same decrement direction is shown, in the same trial, "
            "to work normally from a genuine non-zero position, ruling out a simply broken decrement path");
    }
    {
        Rig r;
        bool mut = mutating("sol:8-cia.c15");
        r.select(0);
        r.saturate_step_ena();
        if (mut) {
            r.do_step(/*direc_outward=*/false);
            r.saturate_step_ena();
        }
        bool at_zero_before_attempt = r.track0();
        r.do_step(/*direc_outward=*/true);
        bool still_at_zero_after_attempt = r.track0();
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/false);
        r.saturate_step_ena();
        r.do_step(/*direc_outward=*/true);
        bool decrement_works_away_from_zero = r.track0();
        checks.note("sol:8-cia.c15: twin of opus:8-cia.c15, same measurement -> before=" +
                    std::to_string(at_zero_before_attempt) + ", after attempt=" +
                    std::to_string(still_at_zero_after_attempt) + ", control=" +
                    std::to_string(decrement_works_away_from_zero));
        checks.expect(at_zero_before_attempt && still_at_zero_after_attempt && decrement_works_away_from_zero,
            "sol:8-cia.c15 (p.244, quote 'All new drives must refuse to step outward in this condition.'): twin "
            "of opus:8-cia.c15 (same printed quote, independent reader) -- same measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // opus:8-cia.c17 (p.244) -- P-WPROT. DSKPROT* polarity.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("opus:8-cia.c17");
        r.select(0);
        r.d->floppy_wrprot = 0;
        bool wprot_off = r.wprot();
        // MUTATION: never assert floppy_wrprot at all -- the "protected"
        // half of this polarity claim is never exercised.
        r.d->floppy_wrprot = mut ? 0 : 1;
        bool wprot_on = r.wprot();
        checks.note("opus:8-cia.c17: floppy_wrprot=0 -> _wprot asserted(protected)=" + std::to_string(wprot_off) +
                    "; floppy_wrprot=" + std::string(mut ? "0(mutated)" : "1") +
                    " -> _wprot asserted(protected)=" + std::to_string(wprot_on));
        checks.expect(!wprot_off && wprot_on,
            "opus:8-cia.c17 (p.244, quote 'PA3 DSKPROT* Disk is write protected (active low).'): with the drive "
            "selected, _wprot reads deasserted while floppy_wrprot=0 and asserted (low) while floppy_wrprot=1 -- "
            "paula_floppy.v:664 (_wprot=&(_sel|disk_writable)) with :969 "
            "(disk_writable=floppy_wrprot?0000:1111)");
    }

    // ========================================================================
    // opus:8-cia.c19 / sol:8-cia.c18 (p.244, twins) -- P-CHANGE-ON-REMOVE.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("opus:8-cia.c19");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(/*direc_outward=*/false);  // establish a stable baseline
        bool stable_baseline = !r.change();
        // MUTATION: never actually drop the image -- the "removed" half is
        // never exercised.
        if (!mut) r.unmount(0);
        bool changed_after_removal = r.change();
        checks.note("opus:8-cia.c19: stable baseline (not changed)=" + std::to_string(stable_baseline) +
                    ", after " + std::string(mut ? "NOT removing (mutated)" : "removing") +
                    " the image, _change asserted=" + std::to_string(changed_after_removal));
        checks.expect(stable_baseline && changed_after_removal,
            "opus:8-cia.c19 (p.244, quote 'PA2 DSKCHANGE* Disk has been removed from the drive. The signal goes "
            "low whenever a disk is removed.'): with the drive selected, _change reads deasserted once a "
            "mount+step establishes a stable baseline, and asserted once the image is removed (sdc_image_size "
            "dropped to 0) -- paula_floppy.v:610 (_disk_change latch) with :662 "
            "(_change=&(_sel|_disk_change)) and :965 (disk_present<=|sdc_image_size)");
    }
    {
        Rig r;
        bool mut = mutating("sol:8-cia.c18");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(/*direc_outward=*/false);
        bool stable_baseline = !r.change();
        if (!mut) r.unmount(0);
        bool changed_after_removal = r.change();
        checks.note("sol:8-cia.c18: twin of opus:8-cia.c19, same measurement -> baseline=" +
                    std::to_string(stable_baseline) + ", after removal=" + std::to_string(changed_after_removal));
        checks.expect(stable_baseline && changed_after_removal,
            "sol:8-cia.c18 (p.244, quote 'The signal goes low whenever a disk is removed.'): twin of "
            "opus:8-cia.c19 (same printed quote, independent reader) -- same measurement, graded independently "
            "under its own uid");
    }

    // ========================================================================
    // opus:8-cia.c20 / sol:8-cia.c19 (p.244, twins) -- P-CHANGE-LATCH.
    // Compound condition: reinsertion ALONE must NOT clear the latch; only
    // reinsertion AND a following step pulse clears it.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("opus:8-cia.c20");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(/*direc_outward=*/false);
        r.unmount(0);
        bool asserted_after_removal = r.change();
        r.mount(0, 1000);  // reinsert -- insertion ALONE
        bool still_asserted_after_reinsert_alone = r.change();
        // MUTATION: never send the qualifying step pulse after reinsertion
        // -- the "clears" half of this compound claim is never exercised.
        if (!mut) r.do_step(/*direc_outward=*/false);
        bool cleared_after_step = !r.change();
        checks.note("opus:8-cia.c20: asserted after removal=" + std::to_string(asserted_after_removal) +
                    ", still asserted after reinsert ALONE=" +
                    std::to_string(still_asserted_after_reinsert_alone) + ", cleared after " +
                    std::string(mut ? "NO step pulse (mutated)" : "the qualifying step pulse") + "=" +
                    std::to_string(cleared_after_step));
        checks.expect(asserted_after_removal && still_asserted_after_reinsert_alone && cleared_after_step,
            "opus:8-cia.c20 (p.244, quote 'It remains low until a disk is inserted AND a step pulse is "
            "received.'): with the drive selected, _change stays asserted through reinsertion alone and clears "
            "only once a step pulse follows -- paula_floppy.v:610, the OR-term "
            "`~_sel&_step&~_step_del&disk_present` requires both a rising _step edge and disk_present together "
            "to set the stable state back");
    }
    {
        Rig r;
        bool mut = mutating("sol:8-cia.c19");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(/*direc_outward=*/false);
        r.unmount(0);
        bool asserted_after_removal = r.change();
        r.mount(0, 1000);
        bool still_asserted_after_reinsert_alone = r.change();
        if (!mut) r.do_step(/*direc_outward=*/false);
        bool cleared_after_step = !r.change();
        checks.note("sol:8-cia.c19: twin of opus:8-cia.c20, same measurement -> after removal=" +
                    std::to_string(asserted_after_removal) + ", reinsert alone=" +
                    std::to_string(still_asserted_after_reinsert_alone) + ", cleared=" +
                    std::to_string(cleared_after_step));
        checks.expect(asserted_after_removal && still_asserted_after_reinsert_alone && cleared_after_step,
            "sol:8-cia.c19 (p.244, quote 'It remains low until a disk is inserted AND a step pulse is "
            "received.'): twin of opus:8-cia.c20 (same printed quote, independent reader) -- same measurement, "
            "graded independently under its own uid");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION FIXTURE-LIVE stimulus red: word 2 driven identical to word 1 instead of its complement, so the two readings are indistinguishable (MUTATE=FIXTURE-LIVE).
// MUTATION READY-SEL-CONTROL stimulus red: the drive is never actually selected, so the "asserts once selected" half is never exercised (MUTATE=READY-SEL-CONTROL).
// opus:8-cia.c7 -- DECLINED via record(), not expect(); no mutation record (not graded through expect()) -- see the header's note above.
// MUTATION opus:8-cia.c12 stimulus red: the second step is redirected to move away from zero AGAIN instead of back toward it, so the drive never returns to track 0 and the final read is checked against a state that never occurred (MUTATE=opus:8-cia.c12).
// MUTATION sol:8-cia.c10 stimulus red: same redirection as opus:8-cia.c12, independent uid (MUTATE=sol:8-cia.c10).
// MUTATION SEL-MASKS-TRACK0 stimulus red: the drive is never actually deselected, so the masking condition (_selx) is never true and the run cannot distinguish the mask from the genuine track-zero state (MUTATE=SEL-MASKS-TRACK0).
// MUTATION opus:8-cia.c15 stimulus red: the drive is moved away from zero BEFORE the outward-step attempt, so the "genuinely at track zero" precondition this proposition requires is never true when the attempt is made (MUTATE=opus:8-cia.c15).
// MUTATION sol:8-cia.c15 stimulus red: same redirection as opus:8-cia.c15, independent uid (MUTATE=sol:8-cia.c15).
// MUTATION opus:8-cia.c17 stimulus red: floppy_wrprot is never asserted at all, so the "protected" half of this polarity claim is never exercised (MUTATE=opus:8-cia.c17).
// MUTATION opus:8-cia.c19 stimulus red: the image is never actually dropped, so the "removed" half of this claim is never exercised (MUTATE=opus:8-cia.c19).
// MUTATION sol:8-cia.c18 stimulus red: same omission as opus:8-cia.c19, independent uid (MUTATE=sol:8-cia.c18).
// MUTATION opus:8-cia.c20 stimulus red: the qualifying step pulse after reinsertion is skipped, so the "clears" half of this compound claim is never exercised (MUTATE=opus:8-cia.c20).
// MUTATION sol:8-cia.c19 stimulus red: same omission as opus:8-cia.c20, independent uid (MUTATE=sol:8-cia.c19).
