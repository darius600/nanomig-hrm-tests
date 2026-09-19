// Test purpose: assert printed p.12's claim that "the 8520 I/O chips provide
//   timers for developers who are bypassing the operating system" -- at this
//   fixture's boundary, that means `ciaa` (one of the two 8520s on this
//   board) must own a timer a developer can load with a count, start, and
//   see reach a detectable completion after real clock ticks -- at the
//   `ciaa` module boundary, compiled alone with its CIA submodules. This
//   file also carries the package's one declined claim (p.6, the six-duty
//   CIA enumeration): see the RECORDED block at the end of main().
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 12; committed specification 069192a (SPECIFICATION.md, this
//   package).
// Requirement IDs: PROP-TIMER, per SPECIFICATION.md's claim inventory.
//   Claims file: hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl,
//   filtered to classify.package == "ch1-cia-timers" (4 claims; 2 carried
//   on PROP-TIMER here and in test_ciab.cpp -- same proposition, graded
//   once per physical chip -- 2 declined, on the RECORDED line below).
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in
//   `work/2026-09-16_ch1_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// Order-of-work disclosure (carried forward from SPECIFICATION.md, restated
//   here because it bears on how this file's assertion was designed): the
//   task instructions that assigned this package pre-exposed RTL detail for
//   the timer claim -- "A classifier measured ciaa.v:331/351, ciab.v:195/
//   215, the counter at cia_timera.v:17, and eclk at :31" -- before
//   SPECIFICATION.md was written, the same situation hrm/ch8-parallel-
//   port-direction and hrm/ch8-ciabprb-disk-control-bits both disclosed for
//   themselves. SPECIFICATION.md's PROP-TIMER wording nonetheless rests
//   only on extract.quote/classify.restatement and the p.12 text -- it
//   would read identically had the module name and line numbers never been
//   mentioned. The RTL facts below were independently re-verified by this
//   builder (Read, not re-typed from what was pre-exposed) after
//   SPECIFICATION.md's commit, as step 2.
//
// RTL read for step 2, after SPECIFICATION.md was committed (069192a):
//   ciaa.v (388 lines, pinned submodule commit
//   d6f7df957fad655bc07f6a381e5094a9edff0be0) in full; cia_timera.v (112
//   lines) in full; cia_int.v (70 lines) in full; cia_timerb.v/cia_timerd.v
//   not opened for content, only present so Verilator can elaborate `ciaa`.
//   Also read, per the task instructions naming it as this package's
//   control-pattern sibling: hrm/ch8-parallel-port-direction's Makefile and
//   test file (Checks class, Harness bus-timing protocol for `ciaa`
//   specifically -- reused below verbatim as verified RTL-interface
//   plumbing, not as a source of this package's own assertions) and
//   hrm/ch8-disk-index-interrupt-path's arm_timer_a_underflow() (the
//   TALO/TAHI/CRA force-load idiom, confirming the register-select values
//   used below).
//
// RTL facts established by that reading (none of this shaped
//   SPECIFICATION.md, which was committed first):
//   - ciaa.v:130-150 (address decoder, rs[3:0]): pra=0x0, prb=0x1, ddra=0x2,
//     ddrb=0x3, talo=0x4, tahi=0x5, tblo=0x6, tbhi=0x7, sdr=0xC, icrs=0xD,
//     cra=0xE, crb=0xF -- never by name or bus address, `ciaa` alone has no
//     address decoder for $BFE001, that is gary.v's job, out of boundary
//     (irrelevant to PROP-TIMER, which needs no CPU address at all).
//   - ciaa.v:331 (`cia_timera tmra`) and cia_timera.v itself: a 16-bit
//     down-counter (`reg [15:0] tmr`, :18) with an 8-bit latch pair
//     (tmll/tmlh, written at TALO/TAHI); writing TAHI while the timer is
//     stopped registers `thi_load` (:76) which reloads `tmr` from the latch
//     one further clk7_en edge later (:80,:87-88) -- so TALO then TAHI is
//     enough to load the counter, no explicit force-load strobe needed.
//   - cia_timera.v:31-32, `assign count = eclk;` -- `eclk` is a plain
//     LEVEL enable, not an edge-detected input: held at 1, every following
//     clk7_en-qualified clock edge with `start` set decrements `tmr` by
//     exactly 1 (:83-91), confirming the evidence's "count enable is tied
//     to eclk" precisely.
//   - Control register CRA (cia_timera.v:34-45, `tmcr[6:0]`): bit0 = start,
//     bit3 = oneshot, bit4 = force-load strobe (transient, not stored in
//     `tmcr`). Writing CRA=0x01 (start=1, oneshot=0, no force-load) starts
//     continuous counting from whatever is currently latched into `tmr`.
//   - `zero` (:94, `~|tmr`) and `underflow` (:97, `zero & start & count`)
//     are combinational; `underflow` also drives `reload` (:80), so in
//     continuous mode (oneshot=0) the counter auto-reloads from the latch
//     and keeps running past its first underflow -- not exercised further
//     here, one underflow is enough for this claim.
//   - `irq` (cia_timera.v:103, `assign irq = underflow;`) feeds `ta` at
//     ciaa.v:345, into `cia_int` (ciaa.v:311-326). cia_int.v:45
//     (`icr[0] <= ta`, read-clear form) / :53 (`icr[0] <= icr[0]|ta`,
//     sticky form) latches Timer A's underflow into ICR bit 0 on the very
//     next clk7_en edge; cia_int.v:31-34 (mask write, bit7=1 ORs the named
//     bits into `icrmask`) and :62-66 (`irq = |(icrmask & icr)`,
//     combinational) mean writing ICR=0x81 (set-mode, bit0=TA) before
//     starting the timer makes ciaa's own top-level `irq` output pin go
//     high, unprompted by any read, the same clock edge Timer A's
//     underflow latches -- a real, externally observable completion
//     signal, exactly what "provide timers" needs to mean at this
//     boundary.
//   - ciaa.v:79-104 (port list): no signal here is specific to "developers
//     bypassing the operating system" -- that phrase names who uses the
//     timer (a programmer talking to these registers directly, the same
//     register interface this harness itself uses), not a distinct RTL
//     signal, and is not separately gradable; the OS-bypass framing is
//     read as the reason the manual mentions timers here at all, not as an
//     additional condition this fixture must show.
//   - Independently re-confirmed while this file was open (bearing on the
//     RECORDED line below, not on PROP-TIMER): ciaa.v:82-103's port list
//     declares no `portb_out` output at all; :267 (`//reg [7:0]
//     regportb;`) and the write block at :270-279 are commented out; :114
//     (`wire [7:0] portb_out;`) is declared and never assigned. Same
//     finding hrm/ch8-parallel-port-direction already grades under
//     opus:8-par.c1/sol:8-par.c1 (FAIL) -- not re-graded here.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag (`MUT ?=` in the Makefile), never a runtime string compare on
//   the label -- the label passed to expect() carries no `MUT_*` token, per
//   the build brief's warning about the gate mis-pairing a `-D` name typed
//   into a label. Every mutation was actually built and run (WSL Verilator,
//   `make MUT=<NAME> test`, full `rm -rf obj_dir_ciaa obj_dir_ciab`
//   rebuild) and the PASS/FAIL lines and tick counts quoted below are what
//   that run printed, not predictions -- filled in after building, per this
//   suite's rule against writing a record before taking the measurement.
//
// Baseline (no MUT, WSL Verilator 5.052): "2 passed, 0 failed" --
//   FIXTURE-LIVE-CIAA PASS, CIAA-TIMER-PROVIDED PASS (a real, working
//   feature -- no defect is alleged here). Measured: a Timer A load of 2
//   reaches irq after exactly 2 ticks, a load of 10 after exactly 10 --
//   this build's `tmr` decrements by exactly 1 per qualifying clk7_en
//   edge with `start`/`eclk` set, so ticks-to-underflow equals the loaded
//   count exactly, not merely ">1 tick and increasing" as the graded
//   condition (deliberately looser, to not assert a duration p.12 never
//   states) requires.
//
// MUTATION FIXTURE-LIVE-CIAA stimulus red: MUT_FIXLIVE_CIAA repeats the
//   first DDRA word (0x55) instead of writing a distinct second word
//   (0xAA) -> the two readings can no longer be told apart -> measured
//   "1 passed, 1 failed" (FIXTURE-LIVE-CIAA FAIL, CIAA-TIMER-PROVIDED
//   unaffected PASS). Reverted (baseline is MUT_FIXLIVE_CIAA=0).
// MUTATION CIAA-TIMER-PROVIDED stimulus red: MUT_NOTICK_CIAA holds `eclk`
//   at 0 for the whole run instead of 1, so `count` (cia_timera.v:32) never
//   qualifies and `tmr` never decrements from either loaded value -> Timer
//   A never underflows, ICR bit 0 never latches, `irq` never asserts ->
//   measured "1 passed, 1 failed" (both ticks_to_underflow() calls return
//   -1, "MEASURED: ... after -1 clock tick(s) ... after -1 tick(s)",
//   CIAA-TIMER-PROVIDED FAIL, FIXTURE-LIVE-CIAA unaffected PASS). Reverted
//   (baseline is MUT_NOTICK_CIAA=0).

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_FIXLIVE_CIAA
#define MUT_FIXLIVE_CIAA 0
#endif
#ifndef MUT_NOTICK_CIAA
#define MUT_NOTICK_CIAA 0
#endif

namespace {

constexpr uint8_t RS_PRA  = 0x0;
constexpr uint8_t RS_DDRA = 0x2;
constexpr uint8_t RS_PRB  = 0x1;
constexpr uint8_t RS_DDRB = 0x3;
constexpr uint8_t RS_TALO = 0x4;
constexpr uint8_t RS_TAHI = 0x5;
constexpr uint8_t RS_ICR  = 0xD;
constexpr uint8_t RS_CRA  = 0xE;

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Bus-timing protocol for ciaa.v, reused verbatim from
// hrm/ch8-parallel-port-direction's own Harness (this package's named
// control-pattern sibling): regporta/ddrporta/ddrportb/timer registers are
// posedge-clk, clk7_en-gated, so a write needs one tick to latch;
// pa_out/pb_out/porta_out/irq are combinational and need only eval().
class Harness {
public:
    Harness() : dut(new Vciaa) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->clk7n_en = 1;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0;
        dut->portb_in = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint8_t rs, uint8_t data) {
        dut->aen = 1;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        tick();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        tick();
    }

    uint8_t read_reg(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE-CIAA: this exact build of `ciaa` must be shown able to
// deliver a known value before anything PROP-TIMER depends on is trusted.
// Uses CIAADDRA -- a plain port register with no connection to the timer
// path -- so this precondition shares no mechanism with what PROP-TIMER
// grades.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h) {
    h.write_reg(RS_DDRA, 0x55);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, MUT_FIXLIVE_CIAA ? 0x55 : 0xAA);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0x55 && r2 == 0xAA && r1 != r2;
}

// ---------------------------------------------------------------------
// Loads `count` into Timer A's latch (TALO then TAHI), unmasks Timer A's
// interrupt in ICR, starts the timer in continuous mode (CRA=0x01), then
// ticks the shared clock until ciaa's own `irq` output pin reads high.
// Returns the number of ticks that were needed (>=1), or -1 if `irq` never
// asserted within `max_ticks` -- the fixture is never asked to count
// forever. `eclk` is held at 1 for the whole run (a level enable, per
// cia_timera.v:32) unless MUT_NOTICK_CIAA holds it at 0 instead.
// ---------------------------------------------------------------------
int ticks_to_underflow(Harness& h, uint16_t count, int max_ticks) {
    h.dut->eclk = MUT_NOTICK_CIAA ? 0 : 1;
    h.write_reg(RS_TALO, static_cast<uint8_t>(count & 0xFF));
    h.write_reg(RS_TAHI, static_cast<uint8_t>((count >> 8) & 0xFF));
    h.write_reg(RS_ICR, 0x81);   // unmask Timer A (bit7 set-mode, bit0 TA)
    h.write_reg(RS_CRA, 0x01);   // start, continuous mode, no force-load
    for (int i = 1; i <= max_ticks; ++i) {
        h.tick();
        if (h.dut->irq) return i;
    }
    return -1;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h),
            "FIXTURE-LIVE-CIAA: two distinct, known CIAADDRA bytes (0x55, "
            "0xAA) both arrive and read back correctly and distinctly "
            "before any data-dependent assertion is graded "
            "[no page -- infrastructure]");
    }

    int t_small, t_large;
    {
        Harness h;
        t_small = ticks_to_underflow(h, /*count=*/2, /*max_ticks=*/8);
    }
    {
        Harness h;
        t_large = ticks_to_underflow(h, /*count=*/10, /*max_ticks=*/16);
    }
    checks.note("CIAA-TIMER measured: a Timer A load of 2 reached a live, "
                "unmasked irq after " + std::to_string(t_small) +
                " clock tick(s) (-1 = never, within the bound given); a "
                "load of 10 reached it after " + std::to_string(t_large) +
                " tick(s).");
    checks.expect(t_small >= 1 && t_large >= 1 && t_large > t_small,
        "CIAA-TIMER-PROVIDED (p.12, sol:1-guidelines.c46, "
        "opus:1-guidelines.c46): ciaa owns a timer a developer talking to "
        "its registers directly can load with a count, start, and see "
        "reach a detectable completion (ciaa's own irq pin, unmasked "
        "through ICR) after real clock ticks -- not on the write itself "
        "(t_small/t_large both >=1 tick) and proportionally to the loaded "
        "count, not a fixed-latency pulse (a load of 10 takes measurably "
        "longer than a load of 2) [p.12 \"The 8520 I/O chips provide "
        "timers for developers who are bypassing the operating system\"]");

    checks.record(
        "sol:1-components.c125, opus:1-components.c125 (p.6, quote 'These "
        "include modem control, disk status sensing, disk motor and "
        "stepping control, ROM enable, parallel input/output interface, "
        "and keyboard interface'): DECLINED at this package's boundary "
        "(ciaa, ciab alone). Both chips expose only generic 8-bit ports "
        "(porta/portb with their own ddr registers) and a generic serial "
        "shift register -- nothing here carries a name distinguishing "
        "'modem control' from any other duty; that binding is a "
        "minimig.v-level wiring fact one level above this package's "
        "declared tops, the same boundary hrm/ch8-ciabprb-disk-control-"
        "bits's own specification already argued for 16 of its 30 claims, "
        "and hrm/ch8-cia-disk-port-windows independently reached for its "
        "own two-ports claim. Four of the six named duties already have a "
        "specific, named-signal assertion elsewhere in this suite, at a "
        "top that can see the wiring this package's cannot -- not the "
        "same page-6 sentence, so not re-graded, cited here only for "
        "orientation: disk status sensing and disk motor/stepping control "
        "at hrm/ch8-cia-disk-signal-bindings; keyboard interface at "
        "hrm/ch8-keyboard-serial-register; parallel input/output "
        "interface at hrm/ch8-parallel-port-direction (opus:8-par.c1, "
        "sol:8-par.c1 -- found FAIL there: ciaa's Port B write path is "
        "entirely commented out, confirmed again independently in this "
        "file's own ciaa.v reading above -- see the header comment's RTL "
        "facts list). The remaining two -- ROM "
        "enable and modem control -- have no assertion anywhere in this "
        "suite (grep of hrm/ before writing this file found none) and are "
        "out of reach at this boundary regardless: ROM enable is realized "
        "entirely outside the CIA module (board-level overlay logic, not "
        "a ciaa/ciab port at all; hrm/ch7-overlay-source grades a related "
        "but different proposition, on a different page, about the OVL "
        "bit once it reaches the overlay logic, and is not re-graded "
        "here); modem control's specific signals (CD/CTS/DSR/RI/DTR/RTS) "
        "are a minimig.v-level wiring fact this package's two tops cannot "
        "see. See SPECIFICATION.md's 'Claims not assigned an assertion' "
        "section for the full argument.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
