// Test purpose: grade Appendix F's Interrupt Control Register (ICR)
//   description (pp.346-347, plus the worked example on p.352) against
//   `cia_int`, the CIA interrupt controller, compiled ALONE (it is a leaf
//   module -- no sub-instantiation, confirmed by reading all 70 lines of
//   cia_int.v). 70 claims filtered from
//   hrm-testpipeline/work/2026-09-17_appF_packages.jsonl, classify.package
//   == "appF-cia-interrupt-control": 56 graded below (PASS in every case --
//   no known defect in THIS package's own claim set), 14 declined on
//   RECORDED lines, none dropped.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 346, 347, 352; committed specification (SPECIFICATION.md, this
//   package).
// Claims file: hrm-testpipeline/work/2026-09-17_appF_packages.jsonl,
//   filtered to classify.package == "appF-cia-interrupt-control" (70 claims).
// Non-goals: anything outside the `cia_int` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appF_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// ===========================================================================
// Order-of-work disclosure
// ===========================================================================
// The task instructions that assigned this package pre-exposed RTL detail
// ahead of step 1 -- cia_int.v:18-19/22/29/32/34/57, ciaa.v:321, ciab.v:186 --
// the same situation hrm/ch1-cia-timers, hrm/ch8-parallel-port-direction and
// others have each disclosed for themselves. SPECIFICATION.md was written
// from extract.quote/classify.restatement and the printed pages alone; its
// claim inventory and grading boundaries would read identically had none of
// those citations been mentioned. What the pre-exposure did supply: which
// lines to re-verify first, and confidence that the MASK/DATA split (new to
// this package's own claim set, but depended on by two already-accepted
// packages) and the FLAG level-vs-edge question were real and worth a
// dedicated fixture. No expected value below was chosen, adjusted, or
// narrowed because of it -- every comparison traces to a printed sentence,
// re-derived by this builder reading cia_int.v/ciaa.v/ciab.v directly after
// SPECIFICATION.md was committed.
//
// ===========================================================================
// RTL facts (independently re-verified by this builder, Read in full)
// ===========================================================================
// cia_int.v (70 lines, pinned submodule d6f7df957fad655bc07f6a381e5094a9edff0be0):
//   Ports: clk, clk7_en, wr, reset, icrs, ta, tb, alrm, flag, ser, data_in
//   [7:0], data_out[7:0], irq. No address decoder, no rs/rd -- `icrs` IS the
//   select, `wr` alone distinguishes write from read.
//   :18-19 -- `reg [4:0] icr` and `reg [4:0] icrmask`, two DISTINCT 5-bit
//     registers behind the one select `icrs`.
//   :22 -- `data_out = icrs && !wr ? {irq,2'b00,icr[4:0]} : 0`. Reads ONLY
//     ever surface `icr` (+ `irq`, +2 hardcoded zero bits) -- `icrmask` has
//     no path to `data_out` at all, anywhere in the file.
//   :29-35 -- `icrs && wr` routes exclusively to `icrmask[4:0]`, via
//     `data_in[4:0]` only (bits 6:5 of data_in are never read by this
//     block) -- OR when data_in[7], AND-NOT when !data_in[7].
//   :43-50 -- on a read cycle (`icrs && !wr`), `icr[i] <= source_i` (the
//     RAW CURRENT LEVEL, not zero, and not an OR) -- if the corresponding
//     source is still asserted at the read's clock edge, the bit does NOT
//     clear. Every read in this file's own harness uses a genuine one-tick
//     pulse (deasserted before the read), matching how every other package
//     in this suite drives ta/tb/alrm (ta from cia_timera.v's combinational
//     `underflow`, self-clearing after one reload tick) -- p.346's "cleared
//     on a read" is graded fairly, against the documented normal-use shape,
//     not against a level held through the read.
//   :52-58 (the `else`, i.e. NOT a read cycle -- includes writes and idle
//     bus cycles) -- `icr[i] <= icr[i] | source_i`, sticky OR, unconditional
//     on any 1-to-0 or 0-to-1 transition, or a sustained level.
//   :62-66 -- `irq = OR over i of (icrmask[i] & icr[i])`, purely
//     combinational.
// ciaa.v:321 -- `.flag(1'b0)` -- CIA-A's `cia_int` instance has its FLAG
//   input tied to a constant; ciaa can never see a Flag interrupt.
// ciab.v:185 -- `.flag(flag)` -- ciab's FLAG is a real, live top-level port.
// ciaa.v:322 -- `.ser(keystrobe | ser_tx_irq)` -- ciaa's SER is real.
// ciab.v:186 -- `.ser(1'b0)` -- ciab's SER input is tied to a constant.
// So p.346's "there are five sources... Serial port full/empty... Flag" is
// true of EVERY 8520 (and of `cia_int` itself, this package's own top,
// which ties nothing and exposes all five as real ports) but NOT true of
// either physical chip instance in this design: ciaa is missing Flag,
// ciab is missing SER. That tie-off is a fact about ciaa.v/ciab.v, one
// level above this package's declared top (`cia_int`) -- reported here
// (see the MEASURED block near the end of main()) because the task
// instructions specifically asked for it, not graded, because `cia_int`'s
// own boundary has no tie-off to grade against.
//
// FLAG level-vs-edge (measured, reported, NOT adjudicated -- no claim in
//   THIS package's 70-claim inventory cites p.341; that page belongs to a
//   different appendix-F package (I/O ports / pin bindings), so there is no
//   uid here to put on a graded line):
//   cia_int.v:57 is `icr[4] <= icr[4] | flag;` -- read again above, in
//   context, as part of the same sticky-OR block that also handles ta/tb/
//   alrm/ser identically. No edge-detect register (no `flag_d`, no
//   `_del`-suffixed signal, nothing) exists anywhere in this 70-line file
//   for `flag` or for any other source. ciab.v (the only place FLAG is a
//   real signal) was also checked: `flag` is a plain module input,
//   connected straight through at ciab.v:185 with nothing between the port
//   and `cia_int`'s `flag` input -- no edge-detect wrapper there either
//   (contrast `tick_del`, ciab.v:39, "required for edge detection" -- built
//   for TOD's `tick` input, and named as such; no equivalent exists for
//   `flag`). p.341: "FLAG is a negative edge-sensitive input... Any
//   negative transition on FLAG will set the FLAG interrupt bit." Measured
//   below (see PROP-FLAG-LEVEL-VS-EDGE): holding `flag` at a sustained HIGH
//   level with ZERO negative transitions across the observation window
//   still sets icr[4] -- the opposite of what an edge-sensitive input
//   requires (a transition, and specifically a negative one). Whether this
//   is a defect is for Amiga/RTL specialists, per this suite's own division
//   of labour; this package's job is to measure and report it, which the
//   note()-only block near the end of main() does.
//
// ===========================================================================
// Harness design notes
// ===========================================================================
// Unlike ciaa/ciab (which also expose independent DDR/port registers),
// `cia_int` has exactly one functional register pair -- `icr` (read-only
// DATA) and `icrmask` (write-only MASK) -- and no third, unrelated
// register. FIXTURE-LIVE below therefore necessarily shares its pulse
// mechanism with the claims it precedes; this is disclosed rather than
// hidden, and is a property of this module's small port list, not an
// oversight.
//
// `icrmask` has NO read path (cia_int.v:22 only ever reads `icr`) and this
// package adds no debug port (forbidden by the build brief). Every
// assertion below that needs to know `icrmask`'s contents reads it back
// through the `irq` side-channel instead: `probe_mask_bit()` clears `icr`
// to all-zero, pulses exactly one source, and peeks -- with every other
// icr bit known to be 0, `irq == icrmask[that bit]` exactly, by
// cia_int.v:62-66's OR-tree. This is the only observation this file makes
// that was not already available at a real CPU-visible boundary (`data_out`,
// `irq`) -- it composes existing pins, it does not add one.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag (`MUT ?=` in the Makefile), never a runtime string compare on
//   the label. Every mutation was actually built and run (WSL Verilator,
//   `make MUT=<NAME> test`, full `rm -rf obj_dir` rebuild) and the numbers
//   quoted in the per-assertion `// MUTATION` comments below are what that
//   run printed, filled in after building, per this suite's rule against
//   writing a record before taking the measurement.
//
// Baseline (no MUT, WSL Verilator 5.052): "10 passed, 0 failed" -- every
//   one of the 10 assertions below PASSes; this package's own claim set
//   reaches no known defect in `cia_int` (the one disclosed defect, FLAG
//   level-vs-edge, is measured near the end of main() but not graded --
//   see the header note above for why there is no uid to grade it against).

#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vcia_int.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif
#ifndef MUT_SOURCES_NOPULSE
#define MUT_SOURCES_NOPULSE 0
#endif
#ifndef MUT_SPLIT_ASSUME_ECHO
#define MUT_SPLIT_ASSUME_ECHO 0
#endif
#ifndef MUT_SPLIT_WITHHOLD_WRITE
#define MUT_SPLIT_WITHHOLD_WRITE 0
#endif
#ifndef MUT_GATES_SKIPMASK
#define MUT_GATES_SKIPMASK 0
#endif
#ifndef MUT_READCLEAR_NOTICK
#define MUT_READCLEAR_NOTICK 0
#endif
#ifndef MUT_PROTOCOL_SKIP_STEP2
#define MUT_PROTOCOL_SKIP_STEP2 0
#endif
#ifndef MUT_EXAMPLE_SKIP_SET
#define MUT_EXAMPLE_SKIP_SET 0
#endif
#ifndef MUT_CLEARALL_SKIP
#define MUT_CLEARALL_SKIP 0
#endif
#ifndef MUT_RESERVED_NOOP
#define MUT_RESERVED_NOOP 0
#endif
#ifndef MUT_DONTCARE_ASSUME_LIVE
#define MUT_DONTCARE_ASSUME_LIVE 0
#endif

namespace {

std::string hex8(uint32_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%02X", (unsigned)(v & 0xFF));
    return std::string(buf);
}
std::string b2s(bool b) { return b ? "1" : "0"; }

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
Checks checks;

// ---------------------------------------------------------------------
// Harness: cia_int alone. `icrs` is the whole address decoder at this
// boundary (no rs/aen) -- `wr` alone distinguishes a write cycle
// (icrs&&wr -> icrmask) from a read cycle (icrs&&!wr -> icr's read-clear
// branch); icrs==0 means the bus is idle (still runs the sticky-OR "keep
// latched" branch, cia_int.v:52-58, so a source pulse latches into `icr`
// even while nobody is reading ICR -- exactly how ta/tb/alrm reach it from
// the real timers elsewhere in this design).
// ---------------------------------------------------------------------
class Harness {
public:
    Harness() : dut(new Vcia_int) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->wr = 0;
        dut->reset = 1;
        dut->icrs = 0;
        dut->ta = 0;
        dut->tb = 0;
        dut->alrm = 0;
        dut->flag = 0;
        dut->ser = 0;
        dut->data_in = 0;
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

    // Non-clocked peek: samples data_out combinationally with icrs
    // asserted, then deasserts. No clk edge crosses this call, so `icr`/
    // `icrmask` are left exactly as they were -- this cannot itself clear
    // a latched interrupt (cia_int.v:43-50's clear branch is
    // clocked/posedge, not combinational).
    uint8_t peek() {
        dut->icrs = 1;
        dut->wr = 0;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->icrs = 0;
        dut->eval();
        return v;
    }

    void write_mask(uint8_t byte) {
        dut->icrs = 1;
        dut->wr = 1;
        dut->data_in = byte;
        tick();
        dut->icrs = 0;
        dut->wr = 0;
        dut->data_in = 0;
        tick();
    }

    Vcia_int* dut;
};

void set_ta(Harness& h, uint8_t v)   { h.dut->ta = v; }
void set_tb(Harness& h, uint8_t v)   { h.dut->tb = v; }
void set_alrm(Harness& h, uint8_t v) { h.dut->alrm = v; }
void set_ser(Harness& h, uint8_t v)  { h.dut->ser = v; }
void set_flag(Harness& h, uint8_t v) { h.dut->flag = v; }
typedef void (*Setter)(Harness&, uint8_t);

// Pulses exactly one named source high for one tick, then low for one
// tick, via the "keep latched" OR path (cia_int.v:52-58, icrs held low
// throughout), and returns the fresh-fixture DATA peek afterward.
uint8_t pulse_and_read(Setter setter) {
    Harness h;
    if (!MUT_SOURCES_NOPULSE) setter(h, 1);
    h.tick();
    setter(h, 0);
    h.tick();
    return h.peek();
}

// Reads `icrmask[bit]` back through the `irq` side-channel (see the header
// comment). PRECONDITION: `icr` is fully clear (every bit 0) -- true right
// after Harness() (reset clears it) and after every prior call to this
// function (it restores the precondition before returning), so calls can
// be chained back-to-back on one Harness.
bool probe_mask_bit(Harness& h, Setter setter) {
    setter(h, 1);
    h.tick();
    setter(h, 0);
    h.tick();
    const uint8_t v = h.peek();
    const bool irq_bit = (v & 0x80) != 0;
    // Restore the precondition: a clocked read with every source at 0
    // latches icr[i] <= 0 for all i (cia_int.v:43-50).
    h.dut->icrs = 1;
    h.dut->wr = 0;
    h.dut->eval();
    h.tick();
    h.dut->icrs = 0;
    h.dut->wr = 0;
    h.dut->eval();
    return irq_bit;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // =======================================================================
    // FIXTURE-LIVE: this exact build of `cia_int` must be shown able to
    // deliver two distinct, known values before anything else is trusted.
    // See the header note on why this necessarily shares its pulse
    // mechanism with PROP-INTERRUPT-SOURCES -- cia_int has no third,
    // independent register to use instead.
    // MUTATION FIXTURE-LIVE stimulus red: MUT_FIXLIVE also pulses `ta` in
    //   the "no pulse" case, so both readings become 0x01 -- no longer
    //   distinct -> measured "9 passed, 1 failed" (FIXTURE-LIVE FAIL,
    //   nothing else affected). Reverted (baseline is MUT_FIXLIVE=0).
    // =======================================================================
    {
        const uint8_t r1 = pulse_and_read(set_ta);
        uint8_t r2;
        {
            Harness h;
            if (MUT_FIXLIVE) set_ta(h, 1);
            h.tick();
            set_ta(h, 0);
            h.tick();
            r2 = h.peek();
        }
        checks.note("FIXTURE-LIVE: pulsed reading=" + hex8(r1) + ", unpulsed reading=" + hex8(r2));
        checks.expect(r1 == 0x01 && r2 == 0x00 && r1 != r2,
            "FIXTURE-LIVE: two distinct, known DATA readings (a genuine TA "
            "pulse vs. none) both arrive and read back correctly and "
            "distinctly before any data-dependent claim is graded "
            "[no page -- infrastructure]");
    }

    // =======================================================================
    // PROP-INTERRUPT-SOURCES (p.346 "There are five sources of interrupts
    // on the 8520:" and the five bulleted items; p.352's busy-wait example
    // polling ICR bit 0). Each of the four inputs this fixture can pulse
    // independently of a timer/serial submodule -- ta, tb, alrm, ser --
    // reaches its own, exclusive DATA bit (0,1,2,3), matching both the
    // source list's order and the READ map's bit names (D0=TA, D1=TB,
    // D2=ALRM, D3=SP); all five sources (adding flag) are independently,
    // simultaneously live.
    // uids: opus:F-icr.c1, sol:F-icr.c1, opus:F-icr.c2, sol:F-icr.c2,
    //   opus:F-icr.c3, sol:F-icr.c3, opus:F-icr.c4, sol:F-icr.c4,
    //   opus:F-icr.c5, sol:F-example.c17, opus:F-example.c23,
    //   sol:F-icr.c31, sol:F-icr.c32, sol:F-icr.c33, sol:F-icr.c34.
    //   (sol:F-icr.c5/opus:F-icr.c6, the Flag-source claims, are declined
    //   below -- already graded at hrm/ch8-disk-index-interrupt-path.)
    // MUTATION PROP-INTERRUPT-SOURCES stimulus red: MUT_SOURCES_NOPULSE
    //   holds every source pin at 0 throughout (the pulse functions never
    //   assert their target) -> every reading drops to 0x00 -> measured
    //   "8 passed, 2 failed" -- PROP-INTERRUPT-SOURCES FAILs as intended,
    //   and FIXTURE-LIVE also FAILs alongside it (disclosed above and in
    //   SPECIFICATION.md: FIXTURE-LIVE's own pulsed reading is
    //   `pulse_and_read(set_ta)`, the same helper, gated by the same flag
    //   -- this module's small port list leaves no third, independent
    //   register to keep the two apart). Reverted (baseline is
    //   MUT_SOURCES_NOPULSE=0).
    // =======================================================================
    {
        uint8_t v_five;
        {
            Harness h;
            if (!MUT_SOURCES_NOPULSE) {
                set_ta(h, 1); set_tb(h, 1); set_alrm(h, 1); set_ser(h, 1); set_flag(h, 1);
            }
            h.tick();
            set_ta(h, 0); set_tb(h, 0); set_alrm(h, 0); set_ser(h, 0); set_flag(h, 0);
            h.tick();
            v_five = h.peek();
        }
        const uint8_t v_ta = pulse_and_read(set_ta);
        const uint8_t v_tb = pulse_and_read(set_tb);
        const uint8_t v_alrm = pulse_and_read(set_alrm);
        const uint8_t v_ser = pulse_and_read(set_ser);
        checks.note("PROP-INTERRUPT-SOURCES measured: five-at-once=" + hex8(v_five) +
                    " TA-alone=" + hex8(v_ta) + " TB-alone=" + hex8(v_tb) +
                    " ALRM-alone=" + hex8(v_alrm) + " SER-alone=" + hex8(v_ser) +
                    " (mask held at its reset value, 0, throughout -- IR/bit7 "
                    "must read 0 in every case)");
        const bool ok = v_five == 0x1F && v_ta == 0x01 && v_tb == 0x02 &&
                        v_alrm == 0x04 && v_ser == 0x08;
        checks.expect(ok,
            "PROP-INTERRUPT-SOURCES (opus:F-icr.c1, sol:F-icr.c1, "
            "opus:F-icr.c2, sol:F-icr.c2, opus:F-icr.c3, sol:F-icr.c3, "
            "opus:F-icr.c4, sol:F-icr.c4, opus:F-icr.c5, sol:F-example.c17, "
            "opus:F-example.c23, sol:F-icr.c31, sol:F-icr.c32, "
            "sol:F-icr.c33, sol:F-icr.c34): the 8520 has five interrupt "
            "sources (Timer A underflow, Timer B underflow, TOD alarm, "
            "serial port full/empty, Flag); at `cia_int`'s own boundary "
            "each of the four non-Flag sources is a distinct, correctly "
            "positioned, exclusively-set DATA bit (TA=D0, TB=D1, ALRM=D2, "
            "SP=D3), and all five together are simultaneously, "
            "independently live [p.346 \"There are five sources of "
            "interrupts on the 8520\" + bulleted list; p.347 READ map D3-D0]");
    }

    // =======================================================================
    // PROP-MASK-DATA-SPLIT -- p.346: "The interrupt control register
    // consists of a write-only MASK register and a read-only DATA
    // register." NEW to this package's own claim set (not already graded
    // by any accepted package), and specifically what hrm/ch1-cia-timers
    // and hrm/ch8-disk-index-interrupt-path both DEPEND on (each writes a
    // set/clear byte to ICR and reads a data byte back from the SAME
    // address) without ever having had this page.
    // uids: sol:F-icr.c6, sol:F-icr.c7, sol:F-icr.c8, opus:F-icr.c7,
    //   opus:F-icr.c8.
    // MUTATION PROP-MASK-DATA-SPLIT oracle red: MUT_SPLIT_ASSUME_ECHO
    //   changes only the EXPECTED value after the write -- from 0x00 (what
    //   the read-only DATA register, driven by icr not icrmask, actually
    //   shows) to 0x9F (what a wrongly-assumed single read/write register
    //   at one address would echo back). The real RTL does not echo ->
    //   measured "9 passed, 1 failed". Reverted (baseline is
    //   MUT_SPLIT_ASSUME_ECHO=0).
    // MUTATION PROP-MASK-DATA-SPLIT stimulus red: MUT_SPLIT_WITHHOLD_WRITE
    //   withholds the `write_mask(0x9F)` call entirely -- icrmask stays at
    //   its reset value, 0, so the later TA pulse no longer has anything
    //   enabling it: after_pulse and after_read_then_pulse both read 0x01
    //   (DATA bit0 latches; IR stays 0, unmasked) instead of the fixed,
    //   unmutated comparison target 0x81 -> measured "9 passed, 1 failed".
    //   This is the demonstration the oracle mutation above cannot give:
    //   that the "write really took effect" half of the claim depends on
    //   the write actually happening, not just on the comparator being
    //   satisfiable. Reverted (baseline is MUT_SPLIT_WITHHOLD_WRITE=0).
    // =======================================================================
    {
        Harness h;
        if (!MUT_SPLIT_WITHHOLD_WRITE) h.write_mask(0x9F);  // write-only MASK: arbitrary nonzero pattern (set-mode, all 5 bits)
        const uint8_t after_write = h.peek();  // read-only DATA at the SAME address
        // Part B: prove the write really took effect on MASK (not silently
        // dropped) -- pulse TA (bit0, included in 0x9F) and confirm it now
        // drives IR/irq, even though the write itself was never echoed.
        set_ta(h, 1); h.tick(); set_ta(h, 0); h.tick();
        const uint8_t after_pulse = h.peek();
        // Part C: a clocked (destructive) DATA read must not disturb what
        // the write configured -- MASK and DATA are separate registers,
        // not one register toggled by a read/write strobe.
        h.dut->icrs = 1; h.dut->wr = 0; h.dut->eval();
        h.tick();
        h.dut->icrs = 0; h.dut->wr = 0; h.dut->eval();
        set_ta(h, 1); h.tick(); set_ta(h, 0); h.tick();
        const uint8_t after_read_then_pulse = h.peek();

        const uint8_t want_after_write = MUT_SPLIT_ASSUME_ECHO ? 0x9F : 0x00;
        checks.note("PROP-MASK-DATA-SPLIT measured: write 0x9F, immediate DATA "
                    "peek=" + hex8(after_write) + "; TA pulse then peek=" +
                    hex8(after_pulse) + "; a clearing DATA read, then a second "
                    "TA pulse, then peek=" + hex8(after_read_then_pulse));
        const bool ok = after_write == want_after_write && after_pulse == 0x81 &&
                        after_read_then_pulse == 0x81;
        checks.expect(ok,
            "PROP-MASK-DATA-SPLIT (sol:F-icr.c6, sol:F-icr.c7, sol:F-icr.c8, "
            "opus:F-icr.c7, opus:F-icr.c8): one address (`icrs`) reaches two "
            "distinct registers -- writing never reads back as an echo "
            "(immediate peek after the write is 0x00, not 0x9F), yet the "
            "write demonstrably took effect on a real, separate MASK "
            "register (it gates TA's contribution to IR both before and "
            "after an intervening DATA-clearing read) [p.346 \"...consists "
            "of a write-only MASK register and a read-only DATA register\"]");
    }

    // =======================================================================
    // PROP-MASK-GATES-IRQ -- p.346: "Any interrupt will set the "
    // corresponding bit in the DATA register [regardless of MASK]" and
    // p.347: "If an interrupt is to occur based on a particular condition,
    // then that corresponding MASK bit must be a 1." Also grades the WRITE
    // map's non-overlap bit names (D3=SP, D2=ALRM, D1=TB, D0=TA) by
    // confirming a mask write that names exactly one of those bits gates
    // exactly that source's contribution to IR.
    // uids: sol:F-icr.c9, opus:F-icr.c9, sol:F-icr.c23, opus:F-icr.c23,
    //   sol:F-icr.c39, sol:F-icr.c40, sol:F-icr.c41, sol:F-icr.c42.
    // MUTATION PROP-MASK-GATES-IRQ stimulus red: MUT_GATES_SKIPMASK skips
    //   the mask-enabling write in the "masked" half of each of the 4
    //   sub-cases -> IR never rises even though the DATA bit still latches
    //   -> measured "9 passed, 1 failed". Reverted (baseline is
    //   MUT_GATES_SKIPMASK=0).
    // =======================================================================
    {
        struct Case { const char* nm; uint8_t bit; uint8_t setbyte; Setter setter; };
        const Case cases[4] = {
            {"TA",  0x01, 0x81, set_ta},
            {"TB",  0x02, 0x82, set_tb},
            {"ALRM",0x04, 0x84, set_alrm},
            {"SP",  0x08, 0x88, set_ser},
        };
        bool ok = true;
        std::string measured;
        for (const auto& c : cases) {
            uint8_t unmasked, masked;
            {
                Harness h;
                c.setter(h, 1); h.tick(); c.setter(h, 0); h.tick();
                unmasked = h.peek();
            }
            {
                Harness h;
                if (!MUT_GATES_SKIPMASK) h.write_mask(c.setbyte);
                c.setter(h, 1); h.tick(); c.setter(h, 0); h.tick();
                masked = h.peek();
            }
            measured += std::string(c.nm) + ":unmasked=" + hex8(unmasked) +
                        ",masked=" + hex8(masked) + " ";
            ok = ok && (unmasked == c.bit) && (masked == (uint8_t)(0x80 | c.bit));
        }
        checks.note("PROP-MASK-GATES-IRQ measured: " + measured);
        checks.expect(ok,
            "PROP-MASK-GATES-IRQ (sol:F-icr.c9, opus:F-icr.c9, sol:F-icr.c23, "
            "opus:F-icr.c23, sol:F-icr.c39, sol:F-icr.c40, sol:F-icr.c41, "
            "sol:F-icr.c42): each of TA/TB/ALRM/SP sets its own DATA bit "
            "whether or not its MASK bit is 1 (IR/irq stays inactive when "
            "unmasked), and drives IR/irq active once its own, specifically "
            "named MASK bit (D0=TA, D1=TB, D2=ALRM, D3=SP in the WRITE map) "
            "is set to 1 [p.346 \"Any interrupt will set the corresponding "
            "bit in the DATA register\"; p.347 \"...that corresponding MASK "
            "bit must be a 1\"]");
    }

    // =======================================================================
    // PROP-READ-CLEAR -- p.346: "When you read the DATA register, its "
    // contents are cleared (set to 0), and the IRQ line returns to a high "
    // state." Graded against a genuine one-tick pulse (ta), the normal-use
    // shape every other package in this suite drives interrupt sources
    // with -- see the header note on cia_int.v:43-50 for why a sustained
    // level is a different, separately-measured question (FLAG, below).
    // uids: sol:F-icr.c12, opus:F-icr.c12.
    // MUTATION PROP-READ-CLEAR stimulus red: MUT_READCLEAR_NOTICK skips
    //   the one clk tick that runs cia_int.v:43-50's clear branch (the
    //   "read" becomes a peek in all but name) -> the second reading stays
    //   0x81 instead of dropping to 0x00 -> measured "9 passed, 1 failed".
    //   Reverted (baseline is MUT_READCLEAR_NOTICK=0).
    // =======================================================================
    {
        Harness h;
        h.write_mask(0x81);  // unmask TA
        set_ta(h, 1); h.tick(); set_ta(h, 0); h.tick();
        const uint8_t before = h.peek();
        const bool irq_before = h.dut->irq;

        h.dut->icrs = 1; h.dut->wr = 0; h.dut->eval();
        const uint8_t sampled = h.dut->data_out;
        if (!MUT_READCLEAR_NOTICK) h.tick();
        h.dut->icrs = 0; h.dut->wr = 0; h.dut->eval();
        const uint8_t after = h.peek();
        const bool irq_after = h.dut->irq;

        checks.note("PROP-READ-CLEAR measured: before=" + hex8(before) +
                    " (irq=" + b2s(irq_before) + "), clocked-read sampled=" +
                    hex8(sampled) + ", after=" + hex8(after) + " (irq=" +
                    b2s(irq_after) + ")");
        const bool ok = before == 0x81 && irq_before && sampled == 0x81 &&
                        after == 0x00 && !irq_after;
        checks.expect(ok,
            "PROP-READ-CLEAR (sol:F-icr.c12, opus:F-icr.c12): reading the "
            "DATA register clears its contents to 0 and the DUT's own `irq` "
            "output (active-high at this boundary -- ciaa.v:325 wires it "
            "straight through with no inversion) returns inactive, for a "
            "genuine, already-deasserted-by-the-time-of-the-read source "
            "pulse [p.346 \"When you read the DATA register, its contents "
            "are cleared (set to 0), and the IRQ line returns to a high "
            "state\"]");
    }

    // =======================================================================
    // PROP-SETCLEAR-PROTOCOL -- p.346-347's set/clear-bit write protocol:
    // bit 7 (S/C) selects direction; each 1-bit elsewhere in the written
    // byte selects a MASK bit to affect; each 0-bit leaves the
    // corresponding MASK bit at its CURRENT state (not merely "still 0" --
    // the discriminating test below starts several bits already at 1).
    // uids: sol:F-icr.c15, opus:F-icr.c15, sol:F-icr.c16, opus:F-icr.c16,
    //   sol:F-icr.c18, opus:F-icr.c18, sol:F-icr.c19, opus:F-icr.c19,
    //   sol:F-icr.c20, opus:F-icr.c20, sol:F-icr.c21, opus:F-icr.c21,
    //   sol:F-icr.c22, opus:F-icr.c22.
    // MUTATION PROP-SETCLEAR-PROTOCOL stimulus red: MUT_PROTOCOL_SKIP_STEP2
    //   skips the 0x84 write (step 2) entirely -> TA stays the only
    //   enabled bit going into step 3 instead of TA+ALRM -> step 3's
    //   ALRM-preserved sub-check (expected true) reads false -> measured
    //   "9 passed, 1 failed". Reverted (baseline is
    //   MUT_PROTOCOL_SKIP_STEP2=0).
    // =======================================================================
    {
        Harness h;
        h.write_mask(0x81);  // step 1: set-mode, select bit0 (TA) only
        const bool s1_ta = probe_mask_bit(h, set_ta);
        const bool s1_tb = probe_mask_bit(h, set_tb);
        const bool s1_alrm = probe_mask_bit(h, set_alrm);
        const bool s1_ser = probe_mask_bit(h, set_ser);
        const bool s1_flag = probe_mask_bit(h, set_flag);

        if (!MUT_PROTOCOL_SKIP_STEP2) h.write_mask(0x84);  // step 2: set-mode, select bit2 (ALRM) only
        const bool s2_ta = probe_mask_bit(h, set_ta);
        const bool s2_tb = probe_mask_bit(h, set_tb);
        const bool s2_alrm = probe_mask_bit(h, set_alrm);
        const bool s2_ser = probe_mask_bit(h, set_ser);
        const bool s2_flag = probe_mask_bit(h, set_flag);

        h.write_mask(0x03);  // step 3: clear-mode, select bits0,1 (TA,TB)
        const bool s3_ta = probe_mask_bit(h, set_ta);
        const bool s3_tb = probe_mask_bit(h, set_tb);
        const bool s3_alrm = probe_mask_bit(h, set_alrm);
        const bool s3_ser = probe_mask_bit(h, set_ser);
        const bool s3_flag = probe_mask_bit(h, set_flag);

        checks.note("PROP-SETCLEAR-PROTOCOL measured: after 0x81 -> TA=" +
            b2s(s1_ta) + " TB=" + b2s(s1_tb) + " ALRM=" + b2s(s1_alrm) +
            " SER=" + b2s(s1_ser) + " FLAG=" + b2s(s1_flag) +
            "; after 0x84 -> TA=" + b2s(s2_ta) + " TB=" + b2s(s2_tb) +
            " ALRM=" + b2s(s2_alrm) + " SER=" + b2s(s2_ser) + " FLAG=" +
            b2s(s2_flag) + "; after 0x03 -> TA=" + b2s(s3_ta) + " TB=" +
            b2s(s3_tb) + " ALRM=" + b2s(s3_alrm) + " SER=" + b2s(s3_ser) +
            " FLAG=" + b2s(s3_flag));

        const bool ok =
            s1_ta && !s1_tb && !s1_alrm && !s1_ser && !s1_flag &&
            s2_ta && !s2_tb && s2_alrm && !s2_ser && !s2_flag &&
            !s3_ta && !s3_tb && s3_alrm && !s3_ser && !s3_flag;
        checks.expect(ok,
            "PROP-SETCLEAR-PROTOCOL (sol:F-icr.c15, opus:F-icr.c15, "
            "sol:F-icr.c16, opus:F-icr.c16, sol:F-icr.c18, opus:F-icr.c18, "
            "sol:F-icr.c19, opus:F-icr.c19, sol:F-icr.c20, opus:F-icr.c20, "
            "sol:F-icr.c21, opus:F-icr.c21, sol:F-icr.c22, opus:F-icr.c22): "
            "bit 7 of a MASK write selects direction (1=set, 0=clear); each "
            "1-bit among the rest selects a MASK bit to affect; each 0-bit "
            "leaves its MASK bit at whatever it already was -- writing "
            "0x84 (set ALRM) after 0x81 (set TA) leaves TA on; writing 0x03 "
            "(clear TA+TB) afterward leaves ALRM on -- bits are affected "
            "independently, never as a full overwrite [p.346 \"You can set "
            "or clear one or more bits of the MASK register without "
            "affecting the current state of any of the other bits\"; p.347 "
            "\"...causes the MASK register bit to remain in its current "
            "state\" / \"...causes no change...\"]");
    }

    // =======================================================================
    // PROP-EXAMPLE-SEQUENCE -- p.347's worked example: "Suppose you want to
    // set the Timer A interrupt bit... but want to be sure that all other
    // interrupts are cleared" -- the two-instruction idiom (clear bits 6-1,
    // then set bit 0) reaches that goal regardless of the starting MASK
    // state (this fixture deliberately starts with TA OFF and everything
    // else ON, the opposite of the goal, to make the idiom do real work).
    // uids: sol:F-icr.c24, opus:F-icr.c24, sol:F-icr.c25, opus:F-icr.c26,
    //   sol:F-icr.c26, opus:F-icr.c27.
    // MUTATION PROP-EXAMPLE-SEQUENCE stimulus red: MUT_EXAMPLE_SKIP_SET
    //   skips the final 0x81 write (the "set TA" instruction) -> TA stays
    //   off instead of becoming the one enabled bit -> measured "9 passed,
    //   1 failed". Reverted (baseline is MUT_EXAMPLE_SKIP_SET=0).
    // =======================================================================
    {
        Harness h;
        h.write_mask(0x9E);  // baseline: set-mode, select bits4,3,2,1 (not bit0) -- TA off, rest on
        const bool base_ta = probe_mask_bit(h, set_ta);
        const bool base_rest_on = probe_mask_bit(h, set_tb) && probe_mask_bit(h, set_alrm) &&
                                   probe_mask_bit(h, set_ser) && probe_mask_bit(h, set_flag);

        h.write_mask(0x7E);  // "MSB is 0, means clear any bit whose value is 1..." -- clear bits4,3,2,1
        const bool ac_ta = probe_mask_bit(h, set_ta);
        const bool ac_tb = probe_mask_bit(h, set_tb);
        const bool ac_alrm = probe_mask_bit(h, set_alrm);
        const bool ac_ser = probe_mask_bit(h, set_ser);
        const bool ac_flag = probe_mask_bit(h, set_flag);

        if (!MUT_EXAMPLE_SKIP_SET) h.write_mask(0x81);  // "MSB is 1, means set..." -- set bit0
        const bool final_ta = probe_mask_bit(h, set_ta);
        const bool final_rest_off = !probe_mask_bit(h, set_tb) && !probe_mask_bit(h, set_alrm) &&
                                     !probe_mask_bit(h, set_ser) && !probe_mask_bit(h, set_flag);

        checks.note("PROP-EXAMPLE-SEQUENCE measured: baseline(0x9E) TA=" +
            b2s(base_ta) + " rest-all-on=" + b2s(base_rest_on) +
            "; after CLEAR(0x7E) TA=" + b2s(ac_ta) + " TB=" + b2s(ac_tb) +
            " ALRM=" + b2s(ac_alrm) + " SER=" + b2s(ac_ser) + " FLAG=" +
            b2s(ac_flag) + "; after SET(0x81) TA=" + b2s(final_ta) +
            " rest-all-off=" + b2s(final_rest_off));

        const bool ok = !base_ta && base_rest_on &&
                        !ac_ta && !ac_tb && !ac_alrm && !ac_ser && !ac_flag &&
                        final_ta && final_rest_off;
        checks.expect(ok,
            "PROP-EXAMPLE-SEQUENCE (sol:F-icr.c24, opus:F-icr.c24, "
            "sol:F-icr.c25, opus:F-icr.c26, sol:F-icr.c26, opus:F-icr.c27): "
            "the worked example's two-instruction idiom -- clear-mode "
            "0b01111110 then set-mode 0b10000001 -- reaches exactly 'Timer "
            "A enabled, every other source disabled' regardless of the "
            "starting MASK state (measured from the opposite starting "
            "point: TA off, everything else on) [p.347 \"Suppose you want "
            "to set the Timer A interrupt bit..., but want to be sure that "
            "all other interrupts are cleared\"; \"MSB is 0, means clear "
            "any bit...\"; \"MSB is 1, means set any bit...(do not change "
            "any values wherein the written value bit is a zero)\"]");
    }

    // =======================================================================
    // PROP-ICR-CLEAR-ALL -- p.352's setup code: "move.b #%01111111,ciaicr
    // (a4) /Clear all 8520 interrupts". A single clear-mode write with
    // every real mask bit selected (0x7F; bits 6-5 of the byte are along
    // for the ride, see PROP-WRITEMAP-DONTCARE) truly disables all five.
    // uids: sol:F-example.c12, opus:F-example.c18.
    // MUTATION PROP-ICR-CLEAR-ALL stimulus red: MUT_CLEARALL_SKIP skips
    //   the 0x7F write -> all 5 mask bits stay on from the 0x9F baseline
    //   instead of clearing -> measured "9 passed, 1 failed". Reverted
    //   (baseline is MUT_CLEARALL_SKIP=0).
    // =======================================================================
    {
        Harness h;
        h.write_mask(0x9F);  // baseline: set-mode, select all 5 -> mask=0x1F
        const bool base_all_on = probe_mask_bit(h, set_ta) && probe_mask_bit(h, set_tb) &&
                                  probe_mask_bit(h, set_alrm) && probe_mask_bit(h, set_ser) &&
                                  probe_mask_bit(h, set_flag);
        if (!MUT_CLEARALL_SKIP) h.write_mask(0x7F);  // "Clear all 8520 interrupts"
        const bool after_ta = probe_mask_bit(h, set_ta);
        const bool after_tb = probe_mask_bit(h, set_tb);
        const bool after_alrm = probe_mask_bit(h, set_alrm);
        const bool after_ser = probe_mask_bit(h, set_ser);
        const bool after_flag = probe_mask_bit(h, set_flag);

        checks.note("PROP-ICR-CLEAR-ALL measured: baseline(0x9F) all-on=" +
            b2s(base_all_on) + "; after 0x7F -> TA=" + b2s(after_ta) +
            " TB=" + b2s(after_tb) + " ALRM=" + b2s(after_alrm) + " SER=" +
            b2s(after_ser) + " FLAG=" + b2s(after_flag));
        const bool ok = base_all_on && !after_ta && !after_tb && !after_alrm &&
                        !after_ser && !after_flag;
        checks.expect(ok,
            "PROP-ICR-CLEAR-ALL (sol:F-example.c12, opus:F-example.c18): "
            "writing %01111111 to the ICR mask, from a baseline with every "
            "real mask bit on, leaves all five disabled -- \"clear all "
            "8520 interrupts\" is exact, not approximate, at this boundary "
            "[p.352 \"move.b #%01111111,ciaicr(a4) /Clear all 8520 "
            "interrupts\"]");
    }

    // =======================================================================
    // PROP-READMAP-RESERVED-ZERO -- p.347 READ map: D6 and D5 both print
    // as a literal "0", not a source name and not "x". cia_int.v:22 hard-
    // codes them (`2'b00`) directly in the data_out expression -- no
    // register, no input, nothing reachable feeds them.
    // uids: sol:F-icr.c28, sol:F-icr.c29.
    // MUTATION PROP-READMAP-RESERVED-ZERO stimulus impossible:
    //   MUT_RESERVED_NOOP changes nothing -- no stimulus this file (or any
    //   other) can drive reaches cia_int.v:22's literal `2'b00`; it is not
    //   a register, not a wire fed by any input, a constant in the
    //   expression itself. Built and run anyway, at the same maximal-
    //   entropy stimulus (all 5 sources pulsed, all 5 mask bits on) used
    //   below: identical output, 0x9F, every time -- confirmed, not
    //   asserted.
    // =======================================================================
    {
        Harness h;
        h.write_mask(0x9F);
        set_ta(h, 1); set_tb(h, 1); set_alrm(h, 1); set_ser(h, 1); set_flag(h, 1);
        h.tick();
        set_ta(h, 0); set_tb(h, 0); set_alrm(h, 0); set_ser(h, 0); set_flag(h, 0);
        h.tick();
        const uint8_t v = h.peek();
        (void)MUT_RESERVED_NOOP;  // no branch depends on this -- see the comment above
        checks.note("PROP-READMAP-RESERVED-ZERO measured: maximal-entropy "
                    "state (all 5 sources pulsed, all 5 mask bits on) "
                    "data_out=" + hex8(v) + " -- bits[6:5] must read 0");
        const bool ok = (v & 0x60) == 0x00 && v == 0x9F;
        checks.expect(ok,
            "PROP-READMAP-RESERVED-ZERO (sol:F-icr.c28, sol:F-icr.c29): the "
            "READ map's D6 and D5 positions read a fixed 0 under the "
            "highest-entropy stimulus this fixture can produce [p.347 READ "
            "map: \"D ICR  IR 0 0 FLG SP ... ALRM TB TA\"]");
    }

    // =======================================================================
    // PROP-WRITEMAP-DONTCARE -- p.347 WRITE map: D6 and D5 both print as
    // "x" (don't care), where p.346's own prose says "In bits 6-0, you "
    // yourself form a mask" (declined below, as loose prose -- see the
    // RECORDED block). cia_int.v:29-34 slices `data_in[4:0]` only into
    // both the OR and AND-NOT paths -- bits 6 and 5 of a mask write are
    // never read by this module, by anyone.
    // uids: sol:F-icr.c36, sol:F-icr.c37.
    // MUTATION PROP-WRITEMAP-DONTCARE oracle red: MUT_DONTCARE_ASSUME_LIVE
    //   swaps the expected outcome for the D6/D5-alone writes from
    //   "unaffected" to "ALRM cleared", the prediction a (wrong) belief
    //   that D6/D5 alias a real mask bit would make -- the real RTL leaves
    //   ALRM on either way, so comparing against the wrong belief reds two
    //   of the ten sub-checks -> measured "9 passed, 1 failed" (one
    //   `expect()` still carries both flipped sub-checks in its single
    //   boolean AND). Reverted (baseline is MUT_DONTCARE_ASSUME_LIVE=0).
    // MUTATION PROP-WRITEMAP-DONTCARE none impossible: no stimulus can
    //   falsify the D6/D5-alone sub-checks specifically (as opposed to the
    //   baseline-all-on precondition, which any withheld write would
    //   trivially break without testing this claim at all) -- cia_int.v:
    //   29-34 slices `data_in[4:0]` in both the OR and AND-NOT paths, so
    //   data_in[6] and data_in[5] are not read by ANY wire in this module,
    //   the same "stored/consulted nowhere" shape appF-cia-timer-modes
    //   recorded none/impossible for CRA-INMODE and CRB-INMODE-ARITY (tmcr[5]
    //   reaching no logic in cia_timera.v/cia_timerb.v). A stimulus that
    //   skips the baseline 0x9F write would flip the assertion red, but only
    //   by breaking its precondition (no bit is on to test preservation of),
    //   not by exercising D6/D5 -- recorded as none/impossible rather than
    //   substituting that unrelated demonstration for this one.
    // =======================================================================
    {
        Harness h;
        h.write_mask(0x9F);  // baseline: all 5 real mask bits on
        const bool base_ok = probe_mask_bit(h, set_ta) && probe_mask_bit(h, set_tb) &&
                              probe_mask_bit(h, set_alrm) && probe_mask_bit(h, set_ser) &&
                              probe_mask_bit(h, set_flag);

        h.write_mask(0x40);  // clear-mode, D6 selected ALONE, nothing else
        const bool d6_ta = probe_mask_bit(h, set_ta);
        const bool d6_tb = probe_mask_bit(h, set_tb);
        const bool d6_alrm_raw = probe_mask_bit(h, set_alrm);
        const bool d6_alrm = MUT_DONTCARE_ASSUME_LIVE ? !d6_alrm_raw : d6_alrm_raw;
        const bool d6_ser = probe_mask_bit(h, set_ser);
        const bool d6_flag = probe_mask_bit(h, set_flag);

        h.write_mask(0x20);  // clear-mode, D5 selected ALONE, nothing else
        const bool d5_ta = probe_mask_bit(h, set_ta);
        const bool d5_tb = probe_mask_bit(h, set_tb);
        const bool d5_alrm_raw = probe_mask_bit(h, set_alrm);
        const bool d5_alrm = MUT_DONTCARE_ASSUME_LIVE ? !d5_alrm_raw : d5_alrm_raw;
        const bool d5_ser = probe_mask_bit(h, set_ser);
        const bool d5_flag = probe_mask_bit(h, set_flag);

        checks.note("PROP-WRITEMAP-DONTCARE measured: baseline(0x9F) "
            "all-on=" + b2s(base_ok) + "; after 0x40 (D6 alone) -> TA=" +
            b2s(d6_ta) + " TB=" + b2s(d6_tb) + " ALRM=" + b2s(d6_alrm_raw) +
            " SER=" + b2s(d6_ser) + " FLAG=" + b2s(d6_flag) +
            "; after 0x20 (D5 alone) -> TA=" + b2s(d5_ta) + " TB=" +
            b2s(d5_tb) + " ALRM=" + b2s(d5_alrm_raw) + " SER=" +
            b2s(d5_ser) + " FLAG=" + b2s(d5_flag));

        const bool ok = base_ok &&
            d6_ta && d6_tb && d6_alrm && d6_ser && d6_flag &&
            d5_ta && d5_tb && d5_alrm && d5_ser && d5_flag;
        checks.expect(ok,
            "PROP-WRITEMAP-DONTCARE (sol:F-icr.c36, sol:F-icr.c37): a mask "
            "write that selects ONLY D6, or ONLY D5, changes none of the "
            "five real MASK bits -- the WRITE map's \"x\" at those two "
            "positions is exact: cia_int.v:29-34 never reads data_in[6] or "
            "data_in[5] [p.347 WRITE map: \"S/C x x FLG SP ALRM TB TA\"]");
    }

    // =======================================================================
    // FLAG level-vs-edge -- MEASURED and reported, NOT graded (no claim in
    // this package's 70-claim inventory cites p.341 -- it belongs to a
    // different appendix-F package). Confirms cia_int.v:57 independently,
    // per the task instructions' specific request. No MUTATION record: this
    // block carries no expect().
    // =======================================================================
    {
        // Condition (a): a genuine negative transition (0->1->0). Both an
        // edge-sensitive-to-negative-transitions reading and a level-OR
        // reading agree the bit should set here -- not discriminating on
        // its own, included for contrast with (b).
        Harness h;
        set_flag(h, 1);
        h.tick();
        set_flag(h, 0);
        h.tick();
        const uint8_t after_pulse = h.peek();

        // Condition (b): flag transitions LOW->HIGH once (a POSITIVE edge)
        // and is then HELD high for several ticks -- zero negative
        // transitions occur anywhere in this window.
        Harness h2;
        set_flag(h2, 1);
        for (int i = 0; i < 5; ++i) h2.tick();  // held high, never returns low
        const uint8_t held_high = h2.peek();

        checks.note("FLAG level-vs-edge measured (cia_int.v:57, p.341): "
            "a genuine negative-transition pulse (flag 0->1->0) -> DATA " +
            hex8(after_pulse) + " (FLG bit set, as expected either way); "
            "flag held HIGH for 5 ticks with ZERO negative transitions in "
            "the window -> DATA " + hex8(held_high) + " -- the FLG bit "
            "(0x10) is " + std::string((held_high & 0x10) ? "SET" : "clear") +
            " despite no negative transition ever occurring. cia_int.v:57 "
            "is `icr[4] <= icr[4] | flag;`, a level-sensitive OR with no "
            "edge-detect register anywhere in this file or in ciab.v's "
            "wiring of the real `flag` port (ciab.v:185, straight through, "
            "no `_del`-style edge-detect signal, contrast ciab.v:39's "
            "`tick_del`, built and named for that exact purpose for TOD's "
            "`tick` input). p.341: \"FLAG is a negative edge-sensitive "
            "input... Any negative transition on FLAG will set the FLAG "
            "interrupt bit.\" Not graded here -- p.341 is outside this "
            "package's 70-claim inventory; adjudicating whether this is a "
            "defect is for Amiga/RTL specialists, per this suite's own "
            "division of labour.");
    }

    // =======================================================================
    // RECORDED -- declined claims (14 of 70)
    // =======================================================================

    checks.record(
        "ALREADY GRADED, declined (10 uids): sol:F-icr.c5, opus:F-icr.c6 "
        "(p.346 \"-Flag\", the Flag interrupt source), sol:F-icr.c10, "
        "opus:F-icr.c10 (p.346, an enabled interrupt sets IR and pulls IRQ "
        "low), sol:F-icr.c27 (p.347 READ map, D7=IR), sol:F-icr.c30 (p.347 "
        "READ map, D4=FLG), sol:F-icr.c35 (p.347 WRITE map, D7=S/C), "
        "sol:F-icr.c38 (p.347 WRITE map, D4=FLG), opus:F-icr.c28 (p.347 "
        "READ map, the WHOLE row D7..D0), opus:F-icr.c29 (p.347 WRITE map, "
        "the WHOLE row D7..D0) -- all ten are graded, or (for the two "
        "whole-row opus claims) partly graded, at hrm/ch8-disk-index-"
        "interrupt-path's PROP-FLAG-PIN (uids opus:8-cia.c56, "
        "opus:8-dskint.c9, sol:8-int.c9), which writes 0x90 to `ciab`'s "
        "ICR and requires the read to return exactly 0x90 -- bit 7 (IR) "
        "set together with bit 4 (FLG) set. opus:F-icr.c28 and "
        "opus:F-icr.c29 each name a WHOLE 8-bit row (all of D7..D0, not "
        "one bit); D7 and D4 of each are the two positions "
        "PROP-FLAG-PIN already grades, and the classify-time note for both "
        "explicitly says 'decline THOSE TWO rather than re-grade' -- but "
        "a uid on a graded line counts as asserted in full, whatever the "
        "prose around it says (per this suite's own rule), so putting "
        "either whole-row claim on a PASS/FAIL line would silently "
        "re-grade the two already-decided bits under a broader label. "
        "Declined whole; the six NEW bit positions each whole-row claim "
        "also names (D6, D5, D3, D2, D1, D0) are each separately graded "
        "above, on their own PASS lines, at their own finer-grained "
        "sol-reader uids -- not re-listed here, so this decline does not "
        "also name a claim graded elsewhere in this same file -- nothing "
        "the whole-row claims name is lost, only the two-bit overlap is "
        "not re-decided. New beyond the "
        "already-graded fact itself, for sol:F-icr.c5/opus:F-icr.c6 "
        "specifically: p.346 makes Flag a source of EVERY 8520, while "
        "ciaa.v:321 ties CIA-A's `flag` input to a constant 1'b0 -- "
        "PROP-FLAG-PIN already measures Flag at `ciab` (where it is real); "
        "`cia_int` itself (this package's own top) ties nothing, so the "
        "source-list claim is separately, honestly gradable there too, "
        "and is graded above, in PROP-INTERRUPT-SOURCES's five-at-once "
        "sub-check, without re-citing these two declined uids.");

    checks.record(
        "LOOSE PROSE vs. precise table, declined (2 uids): sol:F-icr.c17, "
        "opus:F-icr.c17 (p.346, \"In bits 6-0, you yourself form a mask "
        "that specifies which of the bits you wish to affect\"). p.347's "
        "own WRITE map prints D6 and D5 as \"x\" (don't care), and "
        "cia_int.v:29-34 agrees exactly -- data_in[6] and data_in[5] are "
        "never read by the write path (graded above, PASS, in "
        "PROP-WRITEMAP-DONTCARE). p.346's prose bundles those two dead "
        "bits into a uniform \"bits 6-0\" range as if all seven "
        "participated in forming the mask, where only five (bits 4-0) "
        "actually do. Grading the prose literally would either unfairly "
        "fail a plausible rounding/imprecision in the manual's own words, "
        "or require deciding how literally \"6-0\" is meant to be read -- "
        "not this package's call to make (Amiga/RTL specialists', per this "
        "suite's own division of labour, the same one the build brief "
        "invokes for the appendix's printed-pattern/prose conflict at "
        "NM-HRM-062). Measured and recorded per the task instructions' "
        "explicit direction for this exact pair: \"Record both, grade the "
        "one the RTL can answer and say why\" -- the table version is "
        "graded above; the prose is recorded here, not graded.");

    checks.record(
        "OUT OF BOUNDARY, declined (2 uids): sol:F-icr.c11, opus:F-icr.c11 "
        "(p.346, \"In a multichip system, the IR bit can be polled to "
        "detect which chip has generated an interrupt request\"). This "
        "fixture instantiates exactly one `cia_int` -- there is no second "
        "chip, no shared IR-polling bus, and no arbitration logic to "
        "measure a multichip system against. `cia_int`'s own IR bit "
        "(graded above, PROP-MASK-GATES-IRQ and others) behaves correctly "
        "for a single instance; whether N independent instances' IR bits "
        "can be usefully polled together is a system-integration fact "
        "about how multiple chips' outputs are wired and read, one level "
        "above any single `cia_int`/`ciaa`/`ciab` top this suite builds, "
        "the same boundary hrm/ch1-cia-timers' own SPECIFICATION.md "
        "already argued for ROM enable and modem control (facts realized "
        "or wired entirely outside the CIA module's own boundary).");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
