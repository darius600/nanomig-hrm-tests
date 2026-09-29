// appF-cia-timer-modes -- CRB control-bit conformance test, cia_timerb
// fixture.
//
// Test purpose: grade printed pages 349-350 and 354's claims about CRB's
//   control bits -- START, RUNMODE, LOAD, INMODE -- against `cia_timerb`
//   (cia_timerb.v), the module that reads and acts on these bits, compiled
//   alone. SPECIFICATION.md (committed b64c416) records the claim
//   inventory and proposition grouping in full; this file carries it out.
//   This package has two fixtures -- this is the second, mirroring
//   `hrm/ch1-cia-timers`'s own two-fixture (ciaa/ciab) shape -- because CRA
//   and CRB act on two different, independently-instantiated modules.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 349, 350, 354; committed specification b64c416
//   (SPECIFICATION.md, this package).
// Requirement IDs: CRB-START, CRB-RUNMODE, CRB-LOAD, CRB-INMODE-ARITY-00-01,
//   CRB-INMODE-ARITY-10-11, CRB-INMODE-CODE-00, CRB-INMODE-CODE-10,
//   CRA/CRB-RESET (CRB half), per SPECIFICATION.md's claim inventory table
//   (row 13 split into a graded half and a declined half; rows 9-15
//   otherwise). This file's 7 graded assertions carry 18 of this
//   package's 56 uids (2 of the 18 --
//   sol:F-signals.c89/opus:F-signals.c20 -- are also graded in
//   test_cia_timera.cpp, once per physical timer, the same pattern
//   hrm/ch1-cia-timers uses for its own both-chips claim); the other 38
//   (CRA-*) are graded there.
// Non-goals: anything outside the `cia_timerb` fixture this file builds -- this
//   package Verilates two independent tops from two sources, and this is
//   one of them -- and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision, not that the
//   module is correct and not that the chapter is covered.
//
// Order-of-work disclosure: see test_cia_timera.cpp's header, which
//   applies identically here (both files were designed from the same
//   SPECIFICATION.md pass and the same direct reading of cia_timera.v/
//   cia_timerb.v before that document was drafted).
//
// RTL facts, independently read from cia_timerb.v (107 lines, pinned
//   submodule commit 4495f86fbb0611e53649165faa36f1125e062f18):
//   - Same three-register shape as cia_timera (tlo/thi/tcr, each its own
//     select line; data_out combinational, cia_timerb.v:101-103) plus one
//     extra input this module alone has: `tmra_ovf` (Timer A's underflow,
//     cia_timerb.v:13) and no `spmode` output (this module has none).
//   - `assign count = tmcr[6] ? tmra_ovf : eclk;` (cia_timerb.v:31) -- the
//     pre-exposed line from the task instructions, independently
//     re-confirmed here by direct read: bit 6 alone selects the source;
//     bit 5 (stored at line 39, `data_in[6:5]`, read back at line 103,
//     `{1'b0,tmcr[6:0]}`) is never a term in this expression or anywhere
//     else the file's `tmcr[` occurrences appear (lines 37, 39, 41, 43,
//     51, 52, 103) -- the same "stored, read back, never consulted" shape
//     CRA-INMODE's bit 5 has, one register over.
//   - `tmcr[6:0] <= {data_in[6:5],1'b0,data_in[3:0]};` (line 39) is
//     BYTE-FOR-BYTE the same masking CRA's tcr write uses -- bit 4 forced
//     0, bit 7 of the written byte (`data_in[7]`) never read at all. So AT
//     THIS MODULE'S OWN BOUNDARY, writing 0xFF and reading back gives 0x6F
//     (bits 7 and 4 both masked), the SAME value CRA gives for the same
//     write -- not the 0xEF the pre-exposed ciab-level measurement reports,
//     because ciab ORs cia_timerd's separately-stored ALARM bit back onto
//     bit 7 (ciab.v:69, outside this fixture's boundary and outside this
//     package's claims, since ALARM has no uid of its own here).
//   - Otherwise identical in shape to cia_timera.v: same reset behaviour
//     (tmcr<=0, tmll/tmlh<=8'hFF, tmr<=16'hFFFF), same forceload/reload/
//     thi_load/underflow wiring, same one-shot auto-clear of `start`.
//
// Every mutation below was actually built as a `-D` variant (WSL Verilator
//   5.052, `make MUT=<NAME> test`, full rebuild via the shared Makefile)
//   and the PASS/FAIL lines and measured values quoted are what that run
//   printed, not predictions.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>

#include <verilated.h>
#include "Vcia_timerb.h"

#ifndef MUT_FIXLIVE_TIMERB
#define MUT_FIXLIVE_TIMERB 0
#endif
#ifndef MUT_CRBSTART_NOSTART
#define MUT_CRBSTART_NOSTART 0
#endif
#ifndef MUT_CRBRUNMODE_FORCEONESHOT
#define MUT_CRBRUNMODE_FORCEONESHOT 0
#endif
#ifndef MUT_CRBLOAD_NOSTROBE
#define MUT_CRBLOAD_NOSTROBE 0
#endif
#ifndef MUT_CRBRESET_SKIP
#define MUT_CRBRESET_SKIP 0
#endif
#ifndef MUT_CRBINMODE_CODE00_WRONGCODE
#define MUT_CRBINMODE_CODE00_WRONGCODE 0
#endif
#ifndef MUT_CRBINMODE_CODE10_WRONGCODE
#define MUT_CRBINMODE_CODE10_WRONGCODE 0
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

class Harness {
public:
    Harness() : dut(new Vcia_timerb) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->wr = 0;
        dut->reset = 1;
        dut->tlo = 0;
        dut->thi = 0;
        dut->tcr = 0;
        dut->data_in = 0;
        dut->eclk = 0;
        dut->tmra_ovf = 0;
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
    void load_count(uint16_t count) {
        write_tlo(static_cast<uint8_t>(count & 0xFF));
        write_thi(static_cast<uint8_t>((count >> 8) & 0xFF));
    }
    uint16_t read_counter() {
        const uint8_t lo = read_tlo();
        const uint8_t hi = read_thi();
        return static_cast<uint16_t>((static_cast<uint16_t>(hi) << 8) | lo);
    }
    // Reads the counter, ticks eclk high for n cycles (tmra_ovf low
    // throughout), reads the counter again, and returns how much it
    // decreased. A loaded count large relative to n (this file uses 0x00FF
    // against n=10) means the counter is nowhere near underflowing, so this
    // measures the count SOURCE directly rather than relying on an
    // underflow event that n stimulus pulses cannot reach.
    int delta_after_eclk(int n) {
        const uint16_t before = read_counter();
        dut->eclk = 1;
        dut->tmra_ovf = 0;
        for (int i = 0; i < n; ++i) tick();
        dut->eclk = 0;
        const uint16_t after = read_counter();
        return static_cast<int>(before) - static_cast<int>(after);
    }
    // Same, but the stimulus is n one-tick-high pulses on tmra_ovf (eclk
    // held low throughout).
    int delta_after_tmraovf(int n) {
        const uint16_t before = read_counter();
        dut->eclk = 0;
        for (int i = 0; i < n; ++i) {
            dut->tmra_ovf = 1;
            tick();
            dut->tmra_ovf = 0;
            tick();
        }
        const uint16_t after = read_counter();
        return static_cast<int>(before) - static_cast<int>(after);
    }
    // Ticks eclk held high for `n` cycles (tmra_ovf held low throughout),
    // returning how many ticks observed irq==1 immediately after.
    int count_underflow_pulses_eclk(int n) {
        dut->eclk = 1;
        dut->tmra_ovf = 0;
        int pulses = 0;
        for (int i = 0; i < n; ++i) {
            tick();
            if (dut->irq) ++pulses;
        }
        return pulses;
    }
    // Pulses tmra_ovf high for exactly one tick, n times (eclk held low
    // throughout), returning how many of those pulses were seen as an
    // underflow.
    int count_underflow_pulses_tmraovf(int n) {
        dut->eclk = 0;
        int pulses = 0;
        for (int i = 0; i < n; ++i) {
            dut->tmra_ovf = 1;
            tick();
            if (dut->irq) ++pulses;
            dut->tmra_ovf = 0;
            tick();
        }
        return pulses;
    }

    Vcia_timerb* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // FIXTURE-LIVE-TIMERB: mirrors FIXTURE-LIVE-TIMERA in test_cia_timera.
    // cia_timerb has the same three-register boundary, all three
    // implicated by some assertion below. [no page -- infrastructure]
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x21);
        const uint8_t r1 = h.read_tcr();
        h.write_tcr(MUT_FIXLIVE_TIMERB ? 0x21 : 0x0C);
        const uint8_t r2 = h.read_tcr();
        checks.note("FIXTURE-LIVE-TIMERB: wrote 0x21 -> read " + hex8(r1) + "; wrote " +
            (MUT_FIXLIVE_TIMERB ? std::string("0x21 (mutated, repeat)") : std::string("0x0C")) +
            " -> read " + hex8(r2) + ".");
        checks.expect(r1 == 0x21 && r2 == 0x0C && r1 != r2,
            "FIXTURE-LIVE-TIMERB: two distinct, known CRB byte patterns "
            "(0x21, 0x0C) both arrive and read back correctly and "
            "distinctly through the tcr write/read path before any "
            "data-dependent assertion is graded [no page -- infrastructure]");
    }
    // MUTATION FIXTURE-LIVE-TIMERB stimulus red: MUT_FIXLIVE_TIMERB repeats the first CRB word (0x21) instead of writing a distinct second word (0x0C).

    // =====================================================================
    // CRB-START (p.349 "0 START 1 start Timer B, 0 = stop Timer B. This
    // bit is automatically reset (= 0) when underflow occurs during
    // one-shot mode."; p.350's bitmap repeats the label).
    // uids: sol:F-crb.c2, opus:F-crb.c1, sol:F-crb.c15
    // =====================================================================
    {
        // Uses a loaded count of 50 (well clear of underflow within 3
        // ticks in either phase) and reads the COUNTER VALUE before/after
        // each 3-tick phase, rather than counting underflow pulses -- see
        // test_cia_timera.cpp's CRA-START comment for why (a count of 5
        // over 3 ticks can never underflow either way).
        Harness h;
        h.write_tcr(0x00);
        h.load_count(50);
        const uint8_t before = h.read_tlo();
        h.count_underflow_pulses_eclk(3);
        const uint8_t after_stopped = h.read_tlo();
        h.write_tcr(MUT_CRBSTART_NOSTART ? 0x00 : 0x01);
        h.count_underflow_pulses_eclk(3);
        const uint8_t after_started = h.read_tlo();
        const bool gates_counting = (before == after_stopped) &&
                                     (static_cast<int>(after_stopped) - static_cast<int>(after_started) > 0);

        Harness h2;
        h2.write_tcr(0x08);         // oneshot=1, start=0
        h2.load_count(2);           // thi write auto-starts (oneshot already 1)
        const uint8_t crb_after_load = h2.read_tcr();
        const int p1 = h2.count_underflow_pulses_eclk(2);
        h2.tick();
        const uint8_t crb_after_underflow = h2.read_tcr();
        const int p2 = h2.count_underflow_pulses_eclk(4);
        h2.write_tcr(0x09);         // restart, no latch rewrite
        const int p3 = h2.count_underflow_pulses_eclk(2);

        Harness h3;
        h3.write_tcr(0x00);
        h3.load_count(2);
        h3.write_tcr(0x01);         // explicit start, continuous
        const int c1 = h3.count_underflow_pulses_eclk(2);
        h3.tick();
        const uint8_t crb_after_underflow_h3 = h3.read_tcr();
        const int c2 = h3.count_underflow_pulses_eclk(4);

        checks.note("CRB-START: gates_counting=" + std::string(gates_counting ? "true" : "false") +
            " (tlo before=" + hex8(before) + ", after 3 stopped ticks=" + hex8(after_stopped) +
            ", after 3 more started ticks=" + hex8(after_started) +
            "). One-shot: CRB=" + hex8(crb_after_load) +
            " after load (expect 0x09); pulses in first 2 ticks=" +
            std::to_string(p1) + "; CRB=" + hex8(crb_after_underflow) +
            " after underflow tick (expect 0x08); pulses in next 4=" +
            std::to_string(p2) + " (expect 0); pulses after bset-style restart=" +
            std::to_string(p3) + " (expect 1). Continuous: pulses=" +
            std::to_string(c1) + "; CRB=" + hex8(crb_after_underflow_h3) +
            " after underflow (expect 0x01, not cleared); pulses in next 4=" +
            std::to_string(c2) + " (expect >=1).");

        const bool oneshot_ok = crb_after_load == 0x09 && p1 == 1 &&
            crb_after_underflow == 0x08 && p2 == 0 && p3 == 1;
        const bool continuous_ok = c1 == 1 && crb_after_underflow_h3 == 0x01 && c2 >= 1;

        checks.expect(gates_counting && oneshot_ok && continuous_ok,
            "CRB-START (p.349 sol:F-crb.c2/opus:F-crb.c1, p.350 "
            "sol:F-crb.c15): bit 0 starts/stops Timer B's counting; in "
            "one-shot mode it auto-clears on the following underflow and "
            "the timer restarts by re-setting the bit alone; in continuous "
            "mode it does not auto-clear and Timer B keeps underflowing "
            "unattended -- the same mechanism CRA-START grades for Timer A");
    }
    // MUTATION CRB-START stimulus red: MUT_CRBSTART_NOSTART withholds the write_tcr(0x01) that sets START in the stopped-vs-started half, so tlo also stays unchanged over the "started" 3 ticks instead of decreasing (gates_counting's after_stopped>after_started clause goes false) -- verified this collapses the assertion to FAIL (7 passed -> 6 passed, 2 failed).

    // =====================================================================
    // CRB-RUNMODE (p.349 "3 RUNMODE 1 one-shot mode, 0 = continuous
    // mode,"; p.350's bitmap repeats the label).
    // uids: sol:F-crb.c5, opus:F-crb.c4, sol:F-crb.c12
    // =====================================================================
    {
        Harness h_one;
        h_one.write_tcr(0x08);
        h_one.load_count(2);
        const int oneshot_pulses = h_one.count_underflow_pulses_eclk(8);

        Harness h_cont;
        h_cont.write_tcr(MUT_CRBRUNMODE_FORCEONESHOT ? 0x08 : 0x00);
        h_cont.load_count(2);
        if (!MUT_CRBRUNMODE_FORCEONESHOT) h_cont.write_tcr(0x01);
        const int continuous_pulses = h_cont.count_underflow_pulses_eclk(8);

        checks.note("CRB-RUNMODE: over 8 ticks with a count-of-2 latch, "
            "one-shot mode produced " + std::to_string(oneshot_pulses) +
            " underflow pulse(s); continuous mode produced " +
            std::to_string(continuous_pulses) + " underflow pulse(s).");
        checks.expect(oneshot_pulses == 1 && continuous_pulses >= 2 &&
                       continuous_pulses > oneshot_pulses,
            "CRB-RUNMODE (p.349 sol:F-crb.c5/opus:F-crb.c4, p.350 "
            "sol:F-crb.c12): bit 3 selects one-shot (underflows once, "
            "stops) against continuous (keeps reloading and underflowing) "
            "for Timer B, the same distinction CRA-RUNMODE grades for "
            "Timer A");
    }
    // MUTATION CRB-RUNMODE stimulus red: MUT_CRBRUNMODE_FORCEONESHOT makes the "continuous" arm also write CRB=0x08 (oneshot=1), collapsing continuous_pulses to 1 as well.

    // =====================================================================
    // CRB-LOAD (p.349 "4 LOAD 1 force load (this is a strobe input, there "
    // "is no data storage; bit 4 will always read back a zero and writing "
    // "a 0 has no effect.)"; p.350's bitmap repeats the label, its wrapped "
    // "(strobe)" tail, its general "unused bits ... forced to 0" sentence "
    // "(CRB's own instance of that rule is bit 4 -- there is no separate "
    // "unused CRB bit the way CRA has bit 7), and its printed bracket over "
    // "\"Timer B Variables\", read here as confirming that bits 0-6 are "
    // "this module's own and bit 7 is not (bit 7's storage, ALARM, lives "
    // "in cia_timerd, out of this package's scope since ALARM has no claim "
    // "of its own here -- see SPECIFICATION.md).
    // uids: sol:F-crb.c6, opus:F-crb.c5, sol:F-crb.c11, sol:F-crb.c16,
    //       opus:F-crb.c18, opus:F-crb.c17
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x00);
        h.load_count(0x0040);
        h.write_tcr(0x01);
        for (int i = 0; i < 5; ++i) h.count_underflow_pulses_eclk(1);
        const uint8_t after_5_ticks = h.read_tlo();

        const uint8_t strobe_value = MUT_CRBLOAD_NOSTROBE ? 0x01 : 0x11;
        h.write_tcr(strobe_value);
        const uint8_t after_strobe_tlo = h.read_tlo();
        const uint8_t after_strobe_tcr = h.read_tcr();

        // eclk dropped to 0 first -- see test_cia_timera.cpp's CRA-LOAD
        // comment for why (write_tcr()'s own two ticks would otherwise
        // also decrement the counter, since start is still 1 here).
        h.dut->eclk = 0;
        const uint8_t before_noop = h.read_tlo();
        h.write_tcr(0x01);
        const uint8_t after_noop = h.read_tlo();

        // bit7-at-this-boundary check, folded in per SPECIFICATION.md
        Harness h2;
        h2.write_tcr(0xFF);
        const uint8_t all_ones_readback = h2.read_tcr();

        checks.note("CRB-LOAD: tlo=" + hex8(after_5_ticks) +
            " after 5 ticks from a 0x40 load (expect 0x3B); after writing "
            "CRB=" + hex8(strobe_value) + " tlo=" +
            hex8(after_strobe_tlo) + " (expect 0x40 unless "
            "mutated), CRB reads " + hex8(after_strobe_tcr) +
            " (expect bit4 clear); with eclk dropped to 0, a bit4=0 write "
            "left tlo at " +
            hex8(before_noop) + " -> " + hex8(after_noop) +
            " (expect unchanged); a separate fixture wrote CRB=0xFF and "
            "read back " + hex8(all_ones_readback) +
            " (expect 0x6F at cia_timerb's OWN boundary -- bits 7 and 4 "
            "both masked, the same pair CRA masks, NOT the 0xEF a full-ciab "
            "measurement would show, since bit 7's real storage, ALARM, "
            "lives in cia_timerd, outside this module).");
        checks.expect(after_5_ticks == 0x3B &&
                       after_strobe_tlo == 0x40 &&
                       (after_strobe_tcr & 0x10) == 0 &&
                       before_noop == after_noop &&
                       all_ones_readback == 0x6F,
            "CRB-LOAD (p.349 sol:F-crb.c6/opus:F-crb.c5, p.350 sol:F-crb.c11/"
            "opus:F-crb.c17, general-rule sol:F-crb.c16/opus:F-crb.c18): "
            "bit 4 is a force-load strobe (0x3B -> 0x40), never returns on "
            "a read regardless of what was written, a 0-write leaves the "
            "counter alone, and -- at cia_timerb's own boundary, where "
            "this package's claims are scoped -- bit 4 is the ONLY bit "
            "that never returns: bit 7 is also forced to 0 here, but for a "
            "narrower reason (this module has no storage for it at all, "
            "not that it is a defined 'unused' position), confirming the "
            "printed bracket's implicit partition that bits 0-6 are "
            "cia_timerb's own 'Timer B Variables' and bit 7 is not");
    }
    // MUTATION CRB-LOAD stimulus red: MUT_CRBLOAD_NOSTROBE writes CRB=0x01 (bit4 clear) instead of 0x11 at the strobe step, so no reload happens and tlo continues decrementing from 0x3B instead of jumping back to 0x40.

    // =====================================================================
    // CRB-INMODE-ARITY-00-01 (p.349 "6,5 INMODE Bits CRB6 and CRB5 select
    // one of four possible input modes for Timer B" and its four-row
    // table, codes 00 = 02 pulses, 01 = positive CNT transitions). Split
    // from the combined CRB-INMODE-ARITY assertion (NM-HRM-078): this half
    // needs no CNT pin, because codes 00 and 01 differ only in whether
    // eclk pulses should count at all -- the SAME eclk stimulus this
    // fixture already applies is enough to show that difference, or its
    // absence. The 10/11 half below needs CNT held low for part of the
    // count, which this fixture cannot supply; see that assertion.
    // This is the pre-exposed finding named in the task instructions,
    // confirmed and reported here, not adjudicated. uid: opus:F-crb.c6
    // =====================================================================
    {
        // A loaded count of 0x00FF (255) against only 10 stimulus pulses
        // never reaches underflow -- deliberately, so the SOURCE itself is
        // what gets measured (via the decrease in the counter's own
        // value), not an underflow event 10 pulses cannot produce. start
        // MUST be 0 at the moment of the THI write (inside load_count) for
        // the reload to happen at all -- see test_cia_timera.cpp's
        // CRA-INMODE comment for the same fact one register over. So:
        // write the code bits with start=0, load, THEN set start=1
        // separately -- for EACH stimulus run, since start never
        // auto-clears here (oneshot=0 throughout).
        auto measure = [](Harness& h, uint8_t code2) -> std::pair<int,int> {
            h.write_tcr(static_cast<uint8_t>(code2 << 5));           // bits6:5=code, start=0
            h.load_count(0x00FF);
            h.write_tcr(static_cast<uint8_t>((code2 << 5) | 0x01));  // NOW start=1, continuous
            const int via_eclk = h.delta_after_eclk(10);
            h.write_tcr(static_cast<uint8_t>(code2 << 5));           // start=0 again before reloading
            h.load_count(0x00FF);
            h.write_tcr(static_cast<uint8_t>((code2 << 5) | 0x01));
            const int via_tmraovf = h.delta_after_tmraovf(10);
            return {via_eclk, via_tmraovf};
        };
        Harness h00; auto m00 = measure(h00, 0b00);
        Harness h01; auto m01 = measure(h01, 0b01);

        checks.note("CRB-INMODE-ARITY-00-01: (delta via eclk, delta via "
            "tmra_ovf) per code, 10 pulses of each stimulus against a "
            "loaded count of 255: code00=(" + std::to_string(m00.first) + "," + std::to_string(m00.second) +
            ") code01=(" + std::to_string(m01.first) + "," + std::to_string(m01.second) + ").");
        const bool codes_distinct = (m00 != m01);
        checks.expect(codes_distinct,
            "CRB-INMODE-ARITY-00-01 (p.349 opus:F-crb.c6): CRB6,CRB5=00 "
            "and =01 must select distinguishable input modes for Timer B "
            "(02 pulses against positive CNT transitions) -- MEASURED "
            "FALSE: codes 00 and 01 give the identical response to the "
            "identical eclk and tmra_ovf stimuli, so writing bit 5 (CRB5) "
            "alone changes nothing measurable at this boundary. "
            "Confirming and reporting the pre-measured finding named in "
            "the task instructions (deltas 10/10 for both codes with "
            "Timer A stopped) against this package's own, independently "
            "built fixture; not adjudicated here -- see the report for "
            "what belongs to Amiga/RTL specialists");
    }
    // MUTATION CRB-INMODE-ARITY-00-01 none impossible: red at baseline and no legitimate stimulus makes it green -- nothing this fixture can drive distinguishes codes 00 and 01, and reaching a different result would require changing the submodule under test, which this repair does not touch.

    // =====================================================================
    // CRB-INMODE-ARITY-10-11 (p.349 "6,5 INMODE Bits CRB6 and CRB5 select
    // one of four possible input modes for Timer B" and its four-row
    // table, codes 10 = Timer A underflow pulses, 11 = Timer A underflow
    // pulses while CNT pin is held high). Grading codes 10 and 11 apart
    // needs CNT driven low for part of the count: p.349 makes mode 11
    // count Timer A's underflows only while CNT is high, so with CNT held
    // high throughout -- the only level a fixture with no CNT input can
    // offer -- mode 11 counts every underflow mode 10 counts, and the two
    // ARE the same observable behaviour by the page's own words, not a
    // defect to measure. cia_timerb's complete port list (clk, clk7_en,
    // wr, reset, tlo, thi, tcr, data_in, data_out, eclk, tmra_ovf, irq)
    // has no CNT-like input at all, and no module instantiating it in
    // this pinned design carries one either (the same absence
    // test_cia_timera.cpp's CRA-INMODE and CRA-SPMODE already record).
    // This half cannot be driven at this boundary.
    // uid: opus:F-crb.c6 (half graded above, as CRB-INMODE-ARITY-00-01)
    // =====================================================================
    {
        auto measure = [](Harness& h, uint8_t code2) -> std::pair<int,int> {
            h.write_tcr(static_cast<uint8_t>(code2 << 5));
            h.load_count(0x00FF);
            h.write_tcr(static_cast<uint8_t>((code2 << 5) | 0x01));
            const int via_eclk = h.delta_after_eclk(10);
            h.write_tcr(static_cast<uint8_t>(code2 << 5));
            h.load_count(0x00FF);
            h.write_tcr(static_cast<uint8_t>((code2 << 5) | 0x01));
            const int via_tmraovf = h.delta_after_tmraovf(10);
            return {via_eclk, via_tmraovf};
        };
        Harness h10; auto m10 = measure(h10, 0b10);
        Harness h11; auto m11 = measure(h11, 0b11);
        checks.note("CRB-INMODE-ARITY-10-11: with no CNT input to hold "
            "low, (delta via eclk, delta via tmra_ovf) for codes 10 and "
            "11 are code10=(" + std::to_string(m10.first) + "," + std::to_string(m10.second) +
            ") code11=(" + std::to_string(m11.first) + "," + std::to_string(m11.second) +
            ") -- identical, which p.349 predicts too while CNT stays "
            "high; not informative about whether mode 11 stops counting "
            "with CNT low, which nothing at this boundary can drive.");
        checks.record(
            "CRB-INMODE-ARITY-10-11 (p.349, half graded: opus:F-crb.c6): "
            "DECLINED. Distinguishing codes 10 and 11 requires holding "
            "CNT low for part of the count and confirming mode 11 stops "
            "counting Timer A's underflows there while mode 10 keeps "
            "counting them -- cia_timerb has no CNT pin to drive low, and "
            "neither does any module instantiating it in this pinned "
            "design. Grading cannot proceed without adding a port, which "
            "this package does not do; the other half of the same claim, "
            "codes 00 and 01, is graded above as CRB-INMODE-ARITY-00-01 "
            "and carries the red.");
    }

    // =====================================================================
    // CRB-INMODE-CODE-00 (p.349, row "0  0  Timer B counts 02 pulses").
    // uid: opus:F-crb.c7
    // =====================================================================
    {
        Harness h;
        const uint8_t code = MUT_CRBINMODE_CODE00_WRONGCODE ? 0b10 : 0b00;
        h.write_tcr(static_cast<uint8_t>(code << 5));           // start=0 for the load
        h.load_count(0x00FF);
        h.write_tcr(static_cast<uint8_t>((code << 5) | 0x01));  // NOW start=1
        const int via_eclk = h.delta_after_eclk(10);
        h.write_tcr(static_cast<uint8_t>(code << 5));
        h.load_count(0x00FF);
        h.write_tcr(static_cast<uint8_t>((code << 5) | 0x01));
        const int via_tmraovf = h.delta_after_tmraovf(10);

        checks.note("CRB-INMODE-CODE-00: with code=" + std::to_string(code) +
            "(binary), 10 eclk pulses gave a counter delta of " + std::to_string(via_eclk) +
            " (expect 10 unless mutated), 10 tmra_ovf pulses gave a delta of " +
            std::to_string(via_tmraovf) + " (expect 0).");
        checks.expect(via_eclk == 10 && via_tmraovf == 0,
            "CRB-INMODE-CODE-00 (p.349 opus:F-crb.c7): with CRB6,CRB5=00, "
            "Timer B counts phi-2 (eclk) pulses and ignores tmra_ovf "
            "entirely");
    }
    // MUTATION CRB-INMODE-CODE-00 stimulus red: MUT_CRBINMODE_CODE00_WRONGCODE writes code=10 instead of 00, so via_eclk collapses from 10 to 0 (bit6=1 now selects tmra_ovf) and via_tmraovf becomes 10 instead of 0 -- verified measured "delta of 0 (expect 10 unless mutated) ... delta of 10 (expect 0)", FAIL.

    // =====================================================================
    // CRB-INMODE-CODE-10 (p.349, row "1  0  Timer B counts Timer A
    // underflow pulses"; p.350's bitmap repeats the "10-Timer A" row and
    // its wrapped "(strobe)"/"shot" tail, belonging to LOAD/RUNMODE and
    // not re-cited there).
    // uids: opus:F-crb.c9, opus:F-crb.c15
    // =====================================================================
    {
        Harness h;
        const uint8_t code = MUT_CRBINMODE_CODE10_WRONGCODE ? 0b00 : 0b10;
        h.write_tcr(static_cast<uint8_t>(code << 5));           // start=0 for the load
        h.load_count(0x00FF);
        h.write_tcr(static_cast<uint8_t>((code << 5) | 0x01));  // NOW start=1
        const int via_eclk = h.delta_after_eclk(10);
        h.write_tcr(static_cast<uint8_t>(code << 5));
        h.load_count(0x00FF);
        h.write_tcr(static_cast<uint8_t>((code << 5) | 0x01));
        const int via_tmraovf = h.delta_after_tmraovf(10);

        checks.note("CRB-INMODE-CODE-10: with code=" + std::to_string(code) +
            "(binary), 10 eclk pulses gave a counter delta of " + std::to_string(via_eclk) +
            " (expect 0), 10 tmra_ovf pulses gave a delta of " +
            std::to_string(via_tmraovf) + " (expect 10 unless mutated).");
        checks.expect(via_eclk == 0 && via_tmraovf == 10,
            "CRB-INMODE-CODE-10 (p.349 opus:F-crb.c9, p.350 opus:F-crb.c15): "
            "with CRB6,CRB5=10, Timer B counts Timer A's real underflow "
            "pulses (driven here directly on cia_timerb's own `tmra_ovf` "
            "input) and ignores eclk entirely");
    }
    // MUTATION CRB-INMODE-CODE-10 stimulus red: MUT_CRBINMODE_CODE10_WRONGCODE writes code=00 instead of 10, so via_tmraovf collapses from 10 to 0 (bit6=0 now selects eclk) and via_eclk becomes 10 instead of 0 -- verified measured "delta of 10 (expect 0) ... delta of 0 (expect 10 unless mutated)", FAIL.

    // =====================================================================
    // CRA/CRB-RESET, CRB half (p.354, same sentence CRA-RESET grades in
    // test_cia_timera.cpp).
    // uids: sol:F-signals.c89, opus:F-signals.c20
    // =====================================================================
    {
        Harness h;
        h.write_tcr(0x2D);
        h.write_tlo(0x33);
        h.write_thi(0x44);
        const uint8_t crb_before = h.read_tcr();

        if (!MUT_CRBRESET_SKIP) {
            h.dut->reset = 1;
            h.tick();
            h.tick();
            h.dut->reset = 0;
            h.tick();
        }
        const uint8_t crb_after = h.read_tcr();
        h.write_tcr(0x10);
        const uint8_t tlo_after_load = h.read_tlo();
        const uint8_t thi_after_load = h.read_thi();

        checks.note("CRA/CRB-RESET (CRB half): CRB=" + hex8(crb_before) +
            " before reset; " + hex8(crb_after) + " after reset "
            "(expect 0x00); latch bytes after a forced load are " + hex8(tlo_after_load) + "/" + hex8(thi_after_load) +
            " (expect 0xFF/0xFF).");
        checks.expect(crb_after == 0x00 && tlo_after_load == 0xFF && thi_after_load == 0xFF,
            "CRB-RESET (CRB half of CRA/CRB-RESET; p.354 sol:F-signals.c89/"
            "opus:F-signals.c20): reset clears CRB to 0 and sets both "
            "timer-B latch bytes to all ones");
    }
    // MUTATION CRB-RESET stimulus red: MUT_CRBRESET_SKIP never asserts reset, so CRB stays at 0x2D and the latch bytes stay at 0x33/0x44 instead of resetting.

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
