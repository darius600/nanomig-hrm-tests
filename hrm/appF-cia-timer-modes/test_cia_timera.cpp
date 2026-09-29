// appF-cia-timer-modes -- CRA control-bit conformance test, cia_timera
// fixture.
//
// Test purpose: grade printed pages 345, 348, 349, 352 and 354's claims
//   about CRA's control bits -- START, RUNMODE, LOAD, INMODE, SPMODE, the
//   unused bit 7, the p.352 worked timing example, and the p.354 reset
//   paragraph -- against `cia_timera` (cia_timera.v), the module that reads
//   and acts on these bits, compiled alone. SPECIFICATION.md (committed
//   b64c416, before this file or cia_timera.v/cia_timerb.v were opened for
//   the second time -- see its own disclosure section) records the claim
//   inventory and proposition grouping in full; this file carries it out.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 345, 348, 349, 352, 354; committed specification b64c416
//   (SPECIFICATION.md, this package).
// Requirement IDs: CRA-START, CRA-RUNMODE, CRA-LOAD, CRA-INMODE,
//   CRA-SPMODE, CRA-BIT7-UNUSED, CRA-EXAMPLE-SETUP-MASK,
//   CRA-EXAMPLE-BYTE-ORDER, CRA-RESET, per SPECIFICATION.md's claim
//   inventory table (rows 1-9). This file's 9 assertions carry 38 of this
//   package's 56 uids; the other 18 (CRB-*) are graded in test_cia_timerb.cpp.
//   Claims file: hrm-testpipeline/work/2026-09-17_appF_packages.jsonl,
//   filtered to classify.package == "appF-cia-timer-modes".
// Non-goals: anything outside the `cia_timera` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appF_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// Order-of-work disclosure (carried forward from SPECIFICATION.md): the
//   task instructions that assigned this package pre-exposed RTL detail
//   (cia_timerb.v:31 in full, CRA/CRB read-back values, INMODE deltas) and
//   every claim's own classify.evidence field carries extensive file:line
//   citations -- the same situation every prior appendix-F package
//   disclosed. Beyond that, this builder read cia_timera.v and
//   cia_timerb.v in full BEFORE drafting SPECIFICATION.md (to group 56
//   claims into 15 propositions responsibly), also disclosed there. What
//   neither pre-exposure supplied: the concrete register values, tick
//   counts, mutation implementations and PASS/FAIL/RECORDED counts below,
//   all established after SPECIFICATION.md was committed, by running this
//   file under WSL Verilator, not predicted.
//
// RTL facts, independently read from cia_timera.v (112 lines, pinned
//   submodule commit 4495f86fbb0611e53649165faa36f1125e062f18):
//   - Three registers only: tlo/thi (latch bytes, cia_timera.v:57-71) and
//     tcr (control register, cia_timera.v:34-45), each its own 1-bit select
//     line (no shared address bus at this boundary) -- a write needs
//     select&&wr held for one posedge clk (clk7_en-gated); data_out is
//     driven combinationally by (~wr & select) (cia_timera.v:106-108), so a
//     read needs no clock edge, only eval().
//   - `assign count = eclk;` (cia_timera.v:32) is UNCONDITIONAL -- no
//     tmcr[5] (INMODE) term anywhere in this expression, and grep for
//     `tmcr[` in this file returns lines 38, 40, 42, 44, 52, 53, 54, 108
//     only: bit 5 is stored (line 40, as part of `data_in[6:5]`) and read
//     back (line 108) and consulted nowhere else. CRA-INMODE's RED baseline
//     follows directly from this line, not from the pre-exposed CRB
//     citation, which is about a different module.
//   - `tmcr` is declared `[6:0]` (cia_timera.v:21) -- seven bits. Bit 4 is
//     hardwired to `1'b0` at the write (line 40, `{data_in[6:5],1'b0,
//     data_in[3:0]}`) regardless of `data_in[4]`; there is no bit 7 in the
//     register at all -- the read path prepends a literal `1'b0`
//     (`{1'b0,tmcr[6:0]}`, line 108) to make an 8-bit byte.
//   - `forceload <= tcr & wr & data_in[4];` (line 49, registered one cycle
//     after the write) and `reload = thi_load | forceload | underflow`
//     (line 80) -- the LOAD strobe and a THI write are two independent
//     paths to the same reload, and `underflow` ALSO reloads unconditionally
//     of RUNMODE, every underflow (line 80 has no `oneshot` term) -- what
//     RUNMODE actually gates is the SEPARATE auto-clear of `start`
//     (`else if (underflow && oneshot) tmcr[0] <= 1'd0;`, line 44) and the
//     auto-SET of `start` on a THI write (`else if (thi_load && oneshot)
//     tmcr[0] <= 1'd1;`, line 42) -- so after a one-shot underflow the
//     counter is already reloaded and waiting; only `start` needs
//     re-setting to resume, which is exactly what p.352's worked example
//     does (`bset.b #0,ciacra`, no TALO/TAHI rewrite).
//   - `thi_load <= thi & wr & (~start | oneshot);` (line 76) is why
//     CRA-RUNMODE's fixture writes CRA (to set RUNMODE) BEFORE the TLO/THI
//     latch writes when it wants the THI write to auto-start: `oneshot`
//     is read at the SAME cycle as the THI write, from whatever `tmcr[3]`
//     already holds.
//   - reset (cia_timera.v:37-38, 59-60, 67-68, 85-86): `tmcr[6:0] <= 0`,
//     `tmll <= 8'hFF`, `tmlh <= 8'hFF`, and (not stated on p.354, noted
//     below rather than graded) `tmr <= 16'hFFFF` too.
//
// Every mutation below was actually built as a `-D` variant (WSL Verilator
//   5.052, `make MUT=<NAME> test`, full `rm -rf obj_dir_cia_timera
//   obj_dir_cia_timerb` rebuild via the shared Makefile) and the PASS/FAIL
//   lines and measured values quoted are what that run printed, not
//   predictions -- filled in after building, per this suite's rule against
//   writing a record before taking the measurement.
//
// Baseline (no MUT, WSL Verilator 5.052), both fixtures together (this file
//   and test_cia_timerb.cpp, run by the shared Makefile's `test` target):
//   cia_timera -- "9 passed, 1 failed" (FAIL: CRA-INMODE, a real defect,
//   not a fixture problem -- see its own comment block); cia_timerb --
//   "7 passed, 1 failed" (FAIL: CRB-INMODE-ARITY, confirming the
//   pre-measured CRB5 finding). 18 expect() lines total (2 fixture-liveness
//   + 16 content assertions), carrying all 56 of this package's uids (2 of
//   them, the RESET pair, appear on two lines -- once per physical timer).
//   `make test` exits 1 because of the two real FAILs; this is the
//   package's actual, honest result, not a build failure.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vcia_timera.h"

#ifndef MUT_FIXLIVE_TIMERA
#define MUT_FIXLIVE_TIMERA 0
#endif
#ifndef MUT_CRASTART_NOSTART
#define MUT_CRASTART_NOSTART 0
#endif
#ifndef MUT_CRARUNMODE_FORCEONESHOT
#define MUT_CRARUNMODE_FORCEONESHOT 0
#endif
#ifndef MUT_CRALOAD_NOSTROBE
#define MUT_CRALOAD_NOSTROBE 0
#endif
#ifndef MUT_CRASPMODE_NOSET
#define MUT_CRASPMODE_NOSET 0
#endif
#ifndef MUT_CRARESET_SKIP
#define MUT_CRARESET_SKIP 0
#endif
#ifndef MUT_EXSETUP_NOMASK
#define MUT_EXSETUP_NOMASK 0
#endif
#ifndef MUT_EXORDER_REVERSE
#define MUT_EXORDER_REVERSE 0
#endif

namespace {

std::string hex8(unsigned v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%02X", v & 0xFFu);
    return std::string(buf);
}

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

// cia_timera has no shared address bus -- tlo/thi/tcr are each their own
// 1-bit register-select line. A write needs select&&wr held for one
// clk7_en-gated posedge; data_out is combinational (cia_timera.v:106-108),
// so a read needs only eval(), no clock edge.
class Harness {
public:
    Harness() : dut(new Vcia_timera) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->wr = 0;
        dut->reset = 1;
        dut->tlo = 0;
        dut->thi = 0;
        dut->tcr = 0;
        dut->data_in = 0;
        dut->eclk = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    void write_tcr(uint8_t v) {
        dut->tcr = 1; dut->wr = 1; dut->data_in = v;
        tick();
        dut->tcr = 0; dut->wr = 0; dut->data_in = 0;
        tick();
    }
    void write_tlo(uint8_t v) {
        dut->tlo = 1; dut->wr = 1; dut->data_in = v;
        tick();
        dut->tlo = 0; dut->wr = 0; dut->data_in = 0;
        tick();
    }
    void write_thi(uint8_t v) {
        dut->thi = 1; dut->wr = 1; dut->data_in = v;
        tick();
        dut->thi = 0; dut->wr = 0; dut->data_in = 0;
        tick();
    }
    uint8_t read_tcr() {
        dut->tcr = 1; dut->wr = 0;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->tcr = 0;
        dut->eval();
        return v;
    }
    uint8_t read_tlo() {
        dut->tlo = 1; dut->wr = 0;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->tlo = 0;
        dut->eval();
        return v;
    }
    uint8_t read_thi() {
        dut->thi = 1; dut->wr = 0;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->thi = 0;
        dut->eval();
        return v;
    }
    // Loads `count` into the latch via TLO then TAHI, matching p.352's own
    // printed order. With CRA's RUNMODE bit already set to one-shot before
    // this call, the THI write also auto-starts (cia_timera.v:41-42,76).
    void load_count(uint16_t count) {
        write_tlo(static_cast<uint8_t>(count & 0xFF));
        write_thi(static_cast<uint8_t>((count >> 8) & 0xFF));
    }
    // Ticks eclk held high for `n` cycles, returning how many of those
    // ticks observed irq==1 immediately after (underflow pulses seen in
    // this window).
    int count_underflow_pulses(int n) {
        dut->eclk = 1;
        int pulses = 0;
        for (int i = 0; i < n; ++i) {
            tick();
            if (dut->irq) ++pulses;
        }
        return pulses;
    }

    Vcia_timera* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // FIXTURE-LIVE-TIMERA: this exact build of cia_timera must be shown
    // able to deliver a known value before anything below is trusted.
    // cia_timera has only three registers (tlo, thi, tcr), all three
    // implicated by some assertion below -- there is no register at this
    // boundary independent of every claim here. This check is therefore
    // the plain combinational tcr write/read path with two distinct byte
    // patterns that avoid the two known-always-zero bit positions (4, 7) --
    // a check of the bus plumbing every other assertion also depends on,
    // not of the time-dependent (ticking/counting) behaviour under test
    // elsewhere in this file. [no page -- infrastructure]
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x21);  // bits 5,0
        const uint8_t r1 = h.read_tcr();
        h.write_tcr(MUT_FIXLIVE_TIMERA ? 0x21 : 0x0C);  // bits 3,2
        const uint8_t r2 = h.read_tcr();
        checks.note("FIXTURE-LIVE-TIMERA: wrote 0x21 -> read " + hex8(r1) + "; wrote " +
            (MUT_FIXLIVE_TIMERA ? std::string("0x21 (mutated, repeat)") : std::string("0x0C")) +
            " -> read " + hex8(r2) + ".");
        checks.expect(r1 == 0x21 && r2 == 0x0C && r1 != r2,
            "FIXTURE-LIVE-TIMERA: two distinct, known CRA byte patterns "
            "(0x21, 0x0C) both arrive and read back correctly and "
            "distinctly through the tcr write/read path before any "
            "data-dependent assertion is graded [no page -- infrastructure]");
    }
    // MUTATION FIXTURE-LIVE-TIMERA stimulus red: MUT_FIXLIVE_TIMERA repeats the first CRA word (0x21) instead of writing a distinct second word (0x0C) -- the two readings can no longer be told apart.

    // =====================================================================
    // CRA-START (p.348 "0 START 1 = start Timer A, 0 = stop Timer A. This
    // bit is automatically reset (= 0) when underflow occurs during
    // one-shot mode."; p.349's bitmap repeats the label; p.352's worked
    // example writes and re-writes this bit to restart the timer after a
    // one-shot underflow).
    // uids: sol:F-cra.c3, opus:F-cra.c3, sol:F-cra.c18, sol:F-example.c19,
    //       opus:F-example.c26
    // =====================================================================
    {
        // Uses a loaded count of 50 (well clear of underflow within 3
        // ticks in either phase) and reads the COUNTER VALUE before/after
        // each 3-tick phase, rather than counting underflow pulses (an
        // earlier draft counted pulses here with a count of 5 over 3
        // ticks, which can never underflow either way and so could never
        // observe the started phase actually moving the counter).
        Harness h;
        h.write_tcr(0x00);          // start=0, oneshot=0
        h.load_count(50);           // reload happens on the thi write regardless of start
        const uint8_t before = h.read_tlo();
        h.count_underflow_pulses(3);
        const uint8_t after_stopped = h.read_tlo();
        h.write_tcr(MUT_CRASTART_NOSTART ? 0x00 : 0x01);  // start=1 (unless mutated)
        h.count_underflow_pulses(3);
        const uint8_t after_started = h.read_tlo();
        checks.note("CRA-START stopped-vs-started: tlo=" + hex8(before) +
            " before ticking; after 3 ticks with start=0, tlo=" + hex8(after_stopped) +
            " (expect unchanged); after write_tcr(start=1) and 3 more ticks, tlo=" +
            hex8(after_started) + " (expect 3 less, unless mutated).");
        const bool gates_counting = (before == after_stopped) &&
                                     (static_cast<int>(after_stopped) - static_cast<int>(after_started) > 0);

        // one-shot auto-clear and restart-after-underflow, and continuous non-clear
        Harness h2;
        h2.write_tcr(0x08);         // oneshot=1, start=0 -- set BEFORE the thi write
        h2.load_count(2);           // thi write auto-starts because oneshot is already 1
        const uint8_t cra_after_load = h2.read_tcr();
        const int p1 = h2.count_underflow_pulses(2);   // expect exactly one pulse, at tick 2
        h2.tick();                                     // latch the auto-clear/reload
        const uint8_t cra_after_underflow = h2.read_tcr();
        const int p2 = h2.count_underflow_pulses(4);    // expect zero more pulses (stopped)
        h2.write_tcr(0x09);                             // restart, matching p.352's bset.b
        const uint8_t cra_after_restart = h2.read_tcr();
        const int p3 = h2.count_underflow_pulses(2);    // expect one more pulse

        Harness h3;
        h3.write_tcr(0x00);         // oneshot=0, start=0
        h3.load_count(2);           // reload only, no auto-start (continuous)
        const uint8_t cra_before_start_h3 = h3.read_tcr();
        h3.write_tcr(0x01);         // explicit start, continuous
        const int c1 = h3.count_underflow_pulses(2);
        h3.tick();
        const uint8_t cra_after_underflow_h3 = h3.read_tcr();
        const int c2 = h3.count_underflow_pulses(4);

        checks.note("CRA-START one-shot: CRA=" + hex8(cra_after_load) +
            " after load (expect 0x09, auto-started); pulses in first 2 ticks=" +
            std::to_string(p1) + "; CRA=" + hex8(cra_after_underflow) +
            " after the underflow tick (expect 0x08, auto-cleared); pulses in next 4 ticks=" +
            std::to_string(p2) + " (expect 0); CRA=" + hex8(cra_after_restart) +
            " after bset.b-style restart (expect 0x09); pulses in next 2 ticks=" +
            std::to_string(p3) + " (expect 1). CRA-START continuous: CRA=" + hex8(cra_before_start_h3) + " after load (expect 0x00); pulses=" +
            std::to_string(c1) + "; CRA=" + hex8(cra_after_underflow_h3) +
            " after the underflow tick (expect 0x01, NOT cleared); pulses in next 4 ticks=" +
            std::to_string(c2) + " (expect >=1, continuous keeps running).");

        const bool oneshot_ok = cra_after_load == 0x09 && p1 == 1 &&
            cra_after_underflow == 0x08 && p2 == 0 &&
            cra_after_restart == 0x09 && p3 == 1;
        const bool continuous_ok = cra_before_start_h3 == 0x00 && c1 == 1 &&
            cra_after_underflow_h3 == 0x01 && c2 >= 1;

        checks.expect(gates_counting && oneshot_ok && continuous_ok,
            "CRA-START (p.348 sol:F-cra.c3/opus:F-cra.c3, p.349 sol:F-cra.c18, "
            "p.352 sol:F-example.c19/opus:F-example.c26): bit 0 starts/stops "
            "Timer A's counting (a latched count does not move while the bit "
            "is 0 and does while it is 1); in one-shot mode the bit "
            "auto-clears on the underflow that follows and the timer can be "
            "restarted by re-setting the bit alone, without rewriting "
            "TALO/TAHI, because the reload already happened at the underflow; "
            "in continuous mode the bit does not auto-clear and the timer "
            "keeps underflowing unattended");
    }
    // MUTATION CRA-START stimulus red: MUT_CRASTART_NOSTART withholds the write_tcr(0x01) that sets START in the stopped-vs-started half, so `before==after_stopped` still holds but tlo also stays unchanged over the "started" 3 ticks instead of decreasing (gates_counting's after_stopped>after_started clause goes false) -- verified this collapses the assertion to FAIL (9 passed -> 8 passed, 2 failed), not merely to a differently-true condition.

    // =====================================================================
    // CRA-RUNMODE (p.348 "3 RUNMODE 1 = one-shot mode, 0 = continuous
    // mode."; p.345 "To begin transmission, you must first set up Timer A
    // in continuous mode, and start the timer." -- continuous mode is what
    // sustains the repeated underflow train a serial baud clock needs;
    // p.349's bitmap repeats the label).
    // uids: sol:F-cra.c6, opus:F-cra.c6, sol:F-sdr.c11, opus:F-sdr.c10,
    //       sol:F-cra.c15
    // =====================================================================
    {
        Harness h_one;
        h_one.write_tcr(0x08);      // oneshot=1, start=0 (set before load so thi write auto-starts)
        h_one.load_count(MUT_CRARUNMODE_FORCEONESHOT ? 2 : 2);
        const int oneshot_pulses = h_one.count_underflow_pulses(8);

        Harness h_cont;
        h_cont.write_tcr(MUT_CRARUNMODE_FORCEONESHOT ? 0x08 : 0x00);  // mutation: force oneshot on the "continuous" arm too
        h_cont.load_count(2);
        if (!MUT_CRARUNMODE_FORCEONESHOT) h_cont.write_tcr(0x01);     // explicit start, continuous
        const int continuous_pulses = h_cont.count_underflow_pulses(8);

        checks.note("CRA-RUNMODE: over 8 ticks with a count-of-2 latch, "
            "one-shot mode produced " + std::to_string(oneshot_pulses) +
            " underflow pulse(s); continuous mode produced " +
            std::to_string(continuous_pulses) + " underflow pulse(s).");
        checks.expect(oneshot_pulses == 1 && continuous_pulses >= 2 &&
                       continuous_pulses > oneshot_pulses,
            "CRA-RUNMODE (p.348 sol:F-cra.c6/opus:F-cra.c6, p.349 sol:F-cra.c15, "
            "p.345 sol:F-sdr.c11/opus:F-sdr.c10): with bit 3 set (one-shot), "
            "Timer A underflows exactly once and then stops without further "
            "register writes; with bit 3 clear (continuous), it keeps "
            "reloading and underflowing on its own -- continuous mode is the "
            "one capable of sustaining the repeated clock p.345's serial "
            "transmission precondition depends on, one-shot is not");
    }
    // MUTATION CRA-RUNMODE stimulus red: MUT_CRARUNMODE_FORCEONESHOT makes the "continuous" arm also write CRA=0x08 (oneshot=1) instead of 0x00/0x01, so both arms produce exactly 1 pulse over the 8-tick window and continuous_pulses > oneshot_pulses no longer holds.

    // =====================================================================
    // CRA-LOAD (p.348 "4 LOAD 1 = force load (this is a strobe input, "
    // "there is no data storage; bit 4 will always read back a zero and "
    // "writing a 0 has no effect.)"; p.349's bitmap repeats the label and
    // its wrapped "(strobe)" tail).
    // uids: sol:F-cra.c7, opus:F-cra.c7, sol:F-cra.c14, opus:F-cra.c14
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x00);
        h.load_count(0x0040);
        h.write_tcr(0x01);              // start, continuous
        h.count_underflow_pulses(0);    // no-op, keeps symmetry with the narrative below
        for (int i = 0; i < 5; ++i) h.count_underflow_pulses(1);
        const uint8_t after_5_ticks = h.read_tlo();

        const uint8_t strobe_value = MUT_CRALOAD_NOSTROBE ? 0x01 : 0x11;  // bit4 withheld under mutation
        h.write_tcr(strobe_value);
        const uint8_t after_strobe_tlo = h.read_tlo();
        const uint8_t after_strobe_tcr = h.read_tcr();

        // Writing 0 to bit 4 while running must not itself force a reload.
        // eclk is dropped to 0 first so the two ticks write_tcr() itself
        // takes do not also decrement the counter (start is still 1 here)
        // -- that residual-eclk decrement, not a RUNMODE/LOAD defect, is
        // what an earlier draft of this fixture measured before this fix.
        h.dut->eclk = 0;
        const uint8_t before_noop = h.read_tlo();
        h.write_tcr(0x01);              // bit4=0, start=1 unchanged
        const uint8_t after_noop = h.read_tlo();

        checks.note("CRA-LOAD: tlo=" + hex8(after_5_ticks) +
            " after 5 ticks from a 0x40 load (expect 0x3B); after writing "
            "CRA=" + hex8(strobe_value) + " tlo=" +
            hex8(after_strobe_tlo) + " (expect 0x40 unless mutated) "
            "and CRA reads " + hex8(after_strobe_tcr) +
            " (expect 0x01, bit4 never returns); with eclk dropped to 0, a "
            "bit4=0 write left tlo at " +
            hex8(before_noop) + " -> " + hex8(after_noop) +
            " (expect unchanged).");
        checks.expect(after_5_ticks == 0x3B &&
                       after_strobe_tlo == 0x40 &&
                       (after_strobe_tcr & 0x10) == 0 &&
                       before_noop == after_noop,
            "CRA-LOAD (p.348 sol:F-cra.c7/opus:F-cra.c7, p.349 sol:F-cra.c14/"
            "opus:F-cra.c14): bit 4 is a force-load strobe that reloads the "
            "counter from the latch on demand while running (0x3B -> 0x40), "
            "the bit itself never returns on a read of CRA regardless of "
            "what was written, and writing a 0 there leaves the counter "
            "alone");
    }
    // MUTATION CRA-LOAD stimulus red: MUT_CRALOAD_NOSTROBE writes CRA=0x01 (bit4 clear) instead of 0x11 at the strobe step, so no reload happens and tlo continues decrementing from 0x3B instead of jumping back to 0x40.

    // =====================================================================
    // CRA-INMODE (p.348 "5 INMODE 1 = Timer A counts positive CNT "
    // "transitions, 0 = Timer A counts 02 pulses."). cia_timera's complete
    // port list has no CNT-like input -- only eclk -- so the "positive CNT
    // transitions" half names a pin this module does not have. What IS
    // testable without inventing that pin: does writing bit 5 change the
    // counter's behaviour AT ALL. cia_timera.v:32 (`assign count = eclk;`)
    // has no tmcr[5] term, so the answer, measured, is no.
    // uids: sol:F-cra.c8, opus:F-cra.c8
    // =====================================================================
    {
        // start MUST be 0 at the moment of the THI write for the reload to
        // happen at all (thi_load <= thi & wr & (~start | oneshot),
        // cia_timera.v:76 -- with start already 1 and oneshot 0, ~start|
        // oneshot is 0 and the load silently does not happen). So: load
        // first with start=0, THEN set start separately.
        Harness h0;
        h0.write_tcr(0x00);   // bit5=0, start=0
        h0.load_count(5);
        h0.write_tcr(0x01);   // NOW start=1, bit5=0, continuous
        const int pulses_bit5_0 = h0.count_underflow_pulses(11);
        const uint8_t remaining_bit5_0 = h0.read_tlo();

        Harness h1;
        h1.write_tcr(0x00);   // bit5=1 will be set after the load, same as h0
        h1.load_count(5);     // same latch, same tick count as h0
        h1.write_tcr(0x21);   // NOW start=1, bit5=1, continuous
        const int pulses_bit5_1 = h1.count_underflow_pulses(11);
        const uint8_t remaining_bit5_1 = h1.read_tlo();

        checks.note("CRA-INMODE: with bit5=0 (\"02 pulses\"), 11 eclk ticks "
            "from a count of 5 gave " + std::to_string(pulses_bit5_0) +
            " underflow pulse(s), remaining count " + hex8(remaining_bit5_0) + "; with bit5=1 (\"positive "
            "CNT transitions\", same eclk stimulus, no CNT pin exists to "
            "drive separately), the SAME 11 ticks gave " +
            std::to_string(pulses_bit5_1) + " underflow pulse(s), remaining "
            "count " + hex8(remaining_bit5_1) + ".");
        checks.expect(pulses_bit5_0 != pulses_bit5_1 ||
                       remaining_bit5_0 != remaining_bit5_1,
            "CRA-INMODE (p.348 sol:F-cra.c8/opus:F-cra.c8): writing bit 5 "
            "changes Timer A's count-source behaviour, as the two printed "
            "modes require -- MEASURED FALSE: cia_timera.v:32 assigns "
            "`count = eclk` unconditionally, with no tmcr[5] term anywhere "
            "in the file (grep-confirmed), so bit 5 is stored and read back "
            "and never consulted -- the identical eclk stimulus produces "
            "the identical pulse count and remaining value whether bit 5 is "
            "0 or 1. Not merely 'behaves as fewer than the printed variety', "
            "as CRB's INMODE does one page over -- for CRA, bit 5 selects "
            "between nothing and nothing; every observable mode is the same "
            "one mode");
    }
    // MUTATION CRA-INMODE none impossible: green at baseline is not reachable and there is no mutation to attempt -- the assertion is already RED at baseline, and no legitimate stimulus can make it PASS, because cia_timera.v has no logic anywhere that reads tmcr[5]. Per this suite's existing convention for an unfalsifiable-by-stimulus red (agnus_copper_locreg_width's RM2/RM4-6, LW1/LW3-5 comments), recorded as none/impossible rather than invented.

    // =====================================================================
    // CRA-SPMODE (p.348 "6 SPMODE 1 = Serial port=output (CNT is the "
    // "source of the shift clock), 0 = Serial port=input (external shift "
    // "clock is required)"; p.345 "A control bit selects input or output "
    // "mode."; p.349's bitmap repeats the label).
    // uids: sol:F-sdr.c2, opus:F-sdr.c2, sol:F-cra.c9, opus:F-cra.c9,
    //       sol:F-cra.c12
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x00);
        const bool spmode_0 = h.dut->spmode;
        h.write_tcr(MUT_CRASPMODE_NOSET ? 0x00 : 0x40);   // bit6=1 unless mutated
        const bool spmode_1 = h.dut->spmode;
        h.write_tcr(0x00);
        const bool spmode_back_to_0 = h.dut->spmode;

        checks.note("CRA-SPMODE: spmode reads " + std::string(spmode_0 ? "1" : "0") +
            " with bit6=0, " + std::string(spmode_1 ? "1" : "0") +
            " after writing bit6=1, and " + std::string(spmode_back_to_0 ? "1" : "0") +
            " after clearing it again.");
        checks.note(
            "CRA-SPMODE's two printed parenthetical clauses -- 'CNT is the "
            "source of the shift clock' (bit6=1) and 'external shift clock "
            "is required' (bit6=0) -- both name a CNT-like pin. cia_timera's "
            "complete port list (clk, clk7_en, wr, reset, tlo, thi, tcr, "
            "data_in, data_out, eclk, tmra_ovf, spmode, irq) has no such "
            "pin; neither does ciaa's (checked: no `cnt`/`sp` port anywhere "
            "in src/minimig-aga or src/misc naming an 8520 CNT pin). Not "
            "separately declined -- this is the unconstructible half of "
            "the SAME compound claim the expect() line below grades the "
            "other (mode-select) half of; the uids stay on that graded "
            "line, per the rule that a uid may not appear on both a graded "
            "and a RECORDED line.");
        checks.expect(spmode_0 == false && spmode_1 == true && spmode_back_to_0 == false,
            "CRA-SPMODE (p.345 sol:F-sdr.c2/opus:F-sdr.c2, p.348 sol:F-cra.c9/"
            "opus:F-cra.c9, p.349 sol:F-cra.c12): bit 6 is a real, connected "
            "mode-select control -- cia_timera's own `spmode` output tracks "
            "it directly and reversibly (0/1/0 as the bit is written "
            "0/1/0); the parenthetical CNT-pin clauses are recorded above, "
            "not graded here");
    }
    // MUTATION CRA-SPMODE stimulus red: MUT_CRASPMODE_NOSET withholds the write_tcr(0x40) that sets bit6, so spmode_1 stays 0 instead of becoming 1.

    // =====================================================================
    // CRA-BIT7-UNUSED (p.348 "7 UNUSED"; p.349 "All unused register bits "
    // "are unaffected by a write and forced to 0 on a read.", printed once
    // in CRA's own bitmap context; the printed bracket over the CRA bitmap
    // partitions bit 7 out of "Timer A Variables").
    // uids: sol:F-cra.c10, opus:F-cra.c10, sol:F-cra.c11, opus:F-cra.c15,
    //       sol:F-cra.c19, opus:F-cra.c16
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0xFF);
        const uint8_t r1 = h.read_tcr();
        h.write_tcr(0x80);
        const uint8_t r2 = h.read_tcr();
        h.write_tcr(0xAA);
        const uint8_t r3 = h.read_tcr();
        h.write_tcr(0x55);
        const uint8_t r4 = h.read_tcr();

        checks.note("CRA-BIT7-UNUSED: wrote 0xFF -> read " + hex8(r1) +
            "; wrote 0x80 -> read " + hex8(r2) +
            "; wrote 0xAA -> read " + hex8(r3) +
            "; wrote 0x55 -> read " + hex8(r4) + ".");
        const bool bit7_never_returns = (r1 & 0x80) == 0 && (r2 & 0x80) == 0 &&
                                         (r3 & 0x80) == 0 && (r4 & 0x80) == 0;
        checks.expect(bit7_never_returns,
            "CRA-BIT7-UNUSED (p.348 sol:F-cra.c10/opus:F-cra.c10, p.349 "
            "sol:F-cra.c11/opus:F-cra.c15/sol:F-cra.c19/opus:F-cra.c16): "
            "bit 7 has no storage in `tmcr` (declared [6:0]) -- a write "
            "with bit 7 set never reappears on a read, for any other bits' "
            "value, matching p.349's general 'unused bits ... forced to 0 "
            "on a read' rule and p.349's bracket, which places bit 7 "
            "outside the 'Timer A Variables' span");
    }
    // MUTATION CRA-BIT7-UNUSED none impossible: green at baseline (bit7 always reads 0, for every value written) and no legitimate stimulus makes it flip, because cia_timera.v:21 gives tmcr only 7 bits -- there is no storage element for bit 7 to mutate into returning 1.

    // =====================================================================
    // CRA-EXAMPLE-SETUP-MASK (p.352: "move.b ciacra(a4),d0 / and.b
    // #%11000000,d0 / or.b #%00001000,d0 / move.b d0,ciacra(a4)" -- the
    // read-modify-write that sets RUNMODE while preserving bits 7:6).
    // uids: sol:F-example.c11, opus:F-example.c14, opus:F-example.c15,
    //       opus:F-example.c16, opus:F-example.c17
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x41);              // a prior CRA state: SPMODE set (bit6, inside the
                                         // mask) AND START set (bit0, outside the mask) --
                                         // chosen so the AND step has something to remove;
                                         // 0x40 alone (an earlier draft's value) has no bit
                                         // outside the mask, so skipping the AND made no
                                         // observable difference and the mutation below did
                                         // not flip -- caught by actually running it, not
                                         // asserted from inspection.
        const uint8_t d0_read = h.read_tcr();
        const uint8_t masked = MUT_EXSETUP_NOMASK ? d0_read : (d0_read & 0xC0);
        const uint8_t to_write = masked | 0x08;
        h.write_tcr(to_write);
        const uint8_t readback = h.read_tcr();

        checks.note("CRA-EXAMPLE-SETUP-MASK: CRA read as " + hex8(d0_read) +
            "; AND #%11000000 -> " + hex8(masked) +
            "; OR #%00001000 -> " + hex8(to_write) +
            "; written back and read as " + hex8(readback) + ".");
        checks.expect(readback == 0x48,
            "CRA-EXAMPLE-SETUP-MASK (p.352 sol:F-example.c11, opus:F-example."
            "c14/c15/c16/c17): the printed AND-then-OR read-modify-write "
            "sets RUNMODE (bit 3) to one-shot while carrying bit 6 (SPMODE) "
            "through from whatever CRA already held and dropping bit 0 "
            "(START, outside the %11000000 mask) rather than carrying it "
            "along by accident -- the written-back value reads exactly "
            "0x48 (bit6 | bit3), not 0x49");
    }
    // MUTATION CRA-EXAMPLE-SETUP-MASK stimulus red: MUT_EXSETUP_NOMASK skips the AND #%11000000 step (ORs the OR-tail directly onto the unmasked read value, which still carries bit0/START) -- readback becomes 0x49 instead of 0x48.

    // =====================================================================
    // CRA-EXAMPLE-BYTE-ORDER (p.352: ";--- Set time (low byte THEN high "
    // "byte)" / "move.b #(TIME&$FF),ciatalo(a4)" / "move.b #(TIME>>8),"
    // "ciatahi(a4)").
    // uids: sol:F-example.c13, sol:F-example.c15, sol:F-example.c16,
    //       opus:F-example.c19, opus:F-example.c21, opus:F-example.c22
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x08);  // one-shot mode, matching p.352's own setup, before the latch writes
        const uint8_t lo = 0x64, hi = 0x08;  // TIME=2148=0x0864, same constant the page uses
        if (MUT_EXORDER_REVERSE) {
            h.write_thi(hi);
            h.write_tlo(lo);
        } else {
            h.write_tlo(lo);
            h.write_thi(hi);
        }
        const uint8_t tlo_read = h.read_tlo();
        const uint8_t thi_read = h.read_thi();

        checks.note(std::string("CRA-EXAMPLE-BYTE-ORDER: wrote ") +
            (MUT_EXORDER_REVERSE ? "TAHI then TALO (reversed)" : "TALO then TAHI (printed order)") +
            "; tlo reads " + hex8(tlo_read) +
            ", thi reads " + hex8(thi_read) + ".");
        checks.expect(tlo_read == 0x64 && thi_read == 0x08,
            "CRA-EXAMPLE-BYTE-ORDER (p.352 sol:F-example.c13/c15/c16, "
            "opus:F-example.c19/c21/c22): writing the low byte before the "
            "high byte, the order p.352 prints, loads both correctly "
            "(tlo=0x64, thi=0x08) because the high-byte write is what "
            "triggers the load (cia_timera.v:74-76) -- writing them in the "
            "opposite order loses the low byte to the stale reset value");
    }
    // MUTATION CRA-EXAMPLE-BYTE-ORDER stimulus red: MUT_EXORDER_REVERSE writes TAHI before TALO -- the load latches the high byte correctly but the low byte is still the reset-default 0xFF at that moment, so tlo reads 0xFF instead of 0x64.

    // =====================================================================
    // CRA/CRB-RESET, CRA half (p.354: "The timer control registers are set
    // to zero and the timer latches to all ones.").
    // uids: sol:F-signals.c89, opus:F-signals.c20 (also graded in
    //       test_cia_timerb.cpp, CRB half)
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x2D);            // a recognisable, storable CRA value
        h.write_tlo(0x33);
        h.write_thi(0x44);
        const uint8_t cra_before = h.read_tcr();

        if (!MUT_CRARESET_SKIP) {
            h.dut->reset = 1;
            h.tick();
            h.tick();
            h.dut->reset = 0;
            h.tick();
        }
        const uint8_t cra_after = h.read_tcr();
        // Force a load to read the LATCH (a plain tlo/thi read returns the
        // counter, cia_timera.v:106-107, not the latch directly).
        h.write_tcr(0x10);            // bit4=1, force load, all else 0
        const uint8_t tlo_after_load = h.read_tlo();
        const uint8_t thi_after_load = h.read_thi();

        checks.note("CRA/CRB-RESET (CRA half): CRA=" + hex8(cra_before) +
            " before reset; " + hex8(cra_after) + " after reset "
            "(expect 0x00); latch bytes, read via a forced load after "
            "reset, are " + hex8(tlo_after_load) + "/" + hex8(thi_after_load) + " (expect 0xFF/0xFF).");
        checks.expect(cra_after == 0x00 && tlo_after_load == 0xFF && thi_after_load == 0xFF,
            "CRA-RESET (CRA half of CRA/CRB-RESET; p.354 sol:F-signals.c89/"
            "opus:F-signals.c20): reset clears CRA to 0 and sets both "
            "timer-A latch bytes to all ones");
    }
    // MUTATION CRA-RESET stimulus red: MUT_CRARESET_SKIP never asserts reset, so CRA stays at the previously-written 0x2D (masked: 0x2D itself has no bit4/bit7 set, so it reads back unchanged) instead of clearing to 0, and the latch bytes stay at 0x33/0x44 instead of 0xFF/0xFF.

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
