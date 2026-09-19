// appF-cia-serial-shift-register -- ciaa conformance test
//
// Test purpose: grade p.345's "Serial Shift Register (SDR)" output-mode
//   paragraphs against `ciaa` (ciaa.v's sdr_latch/ser_tx_run/ser_tx_cnt
//   logic + its CIA submodules). SPECIFICATION.md records the claim
//   inventory (27 uids: 15 already graded elsewhere, declined on record()
//   lines; 12 fresh, across 6 propositions, graded below) and the structural
//   finding this page's own SDR mechanism rests on: `ciaa` has no SP/CNT pin
//   and no bit-serial shifter at all -- `sdr_latch` is a flat 8-bit parallel
//   register, and "shifting" is entirely a byte-timing abstraction built
//   from `ser_tx_cnt` counting Timer A underflow pulses (`tmra_ovf`). Every
//   assertion below is designed around that measured structure, not around a
//   literal SP/CNT signal that does not exist.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 345.
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim outside the 35 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 35 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM COVERAGE: 15 of 27 uids are declined on record() lines (duplicate
//   citations, see SPECIFICATION.md). The remaining 12 uids (6 propositions
//   x sol/opus) are graded with expect() below: TIMER-A-IS-GENERATOR
//   (sol:F-sdr.c7, opus:F-sdr.c7), HALF-RATE-SHIFT (sol:F-sdr.c8,
//   opus:F-sdr.c8), MAX-BAUD-CEILING (sol:F-sdr.c9, opus:F-sdr.c9),
//   SDR-WRITE-STARTS-TX (sol:F-sdr.c12, opus:F-sdr.c11),
//   EIGHT-PULSES-INTERRUPT (sol:F-sdr.c16, opus:F-sdr.c15),
//   RELOAD-CONTINUES (sol:F-sdr.c17, opus:F-sdr.c16).
//
// `tmra_ovf`/`ser_tx_cnt`/`ser_tx_run` are internal `ciaa.v` wires, not
// ports -- not readable from this black-box fixture. Every measurement
// below instead uses only `ciaa`'s real ports: TALO/TAHI readback (rs=0x4/
// 0x5, to watch Timer A's own down-counter directly) and the top-level
// `irq` pin (armed once via an ICR mask write, then polled -- never read
// back, since an ICR *read* clears its latched bits (cia_int.v:43-50) and
// would corrupt the very state being measured).
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag (`MUT ?=` in the Makefile). Every mutation was actually built
//   and run (WSL Verilator, `make MUT=<NAME> test`, full `rm -rf obj_dir`
//   rebuild); see the MUTATION comment after each assertion.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif
#ifndef MUT_NO_TIMERA_ISOLATION
#define MUT_NO_TIMERA_ISOLATION 0
#endif
#ifndef MUT_SKIP_SDR_WRITE
#define MUT_SKIP_SDR_WRITE 0
#endif
#ifndef MUT_RESTART_ON_RELOAD_CHECK
#define MUT_RESTART_ON_RELOAD_CHECK 0
#endif

namespace {

constexpr uint8_t RS_TALO = 0x4;
constexpr uint8_t RS_TAHI = 0x5;
constexpr uint8_t RS_TBLO = 0x6;
constexpr uint8_t RS_TBHI = 0x7;
constexpr uint8_t RS_SDR  = 0xC;
constexpr uint8_t RS_ICR  = 0xD;
constexpr uint8_t RS_CRA  = 0xE;
constexpr uint8_t RS_CRB  = 0xF;
constexpr uint8_t RS_DDRA = 0x2;

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

// Bus-timing protocol for ciaa.v, the same as this batch's other `ciaa`
// packages.
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
        ++elapsed;
    }

    Vciaa* dut;
    // Every posedge this Harness has issued since construction, including
    // ones spent inside write_reg()/read_reg() -- used instead of a
    // caller-local loop counter so a register write nested inside a
    // tick-counting loop (e.g. RELOAD-CONTINUES's mid-transmission SDR
    // rewrite) cannot desynchronize "ticks elapsed" from "clock edges the
    // DUT actually saw". Includes the constructor's own reset ticks, which
    // is harmless: every measurement below compares elapsed against either
    // another elapsed value from the same Harness or a value fitted from
    // other Harness instances built the same way (the constant offset
    // cancels).
    int elapsed = 0;
};

std::string hex8(uint8_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%02X", v);
    return std::string(buf);
}

bool fixture_live(Harness& h) {
    h.write_reg(RS_DDRA, 0x55);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, MUT_FIXLIVE ? 0x55 : 0xAA);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0x55 && r2 == 0xAA && r1 != r2;
}

// Arms output mode (CRA bit6=spmode) and Timer A's latch, unmasks ICR's
// SER bit (bit3) in set-mode so the top-level `irq` pin reflects
// ser_tx_irq directly, starts Timer A, writes SDR (the transmission
// trigger), then ticks (eclk held at 1 throughout) until `irq` reads high
// or `max_ticks` is exhausted. Returns the 1-based tick index `irq` first
// read high, or -1. Never reads ICR (a read clears its latched bits --
// cia_int.v:43-50 -- which would corrupt the very state this measures).
int ticks_to_ser_complete(Harness& h, uint16_t ta_load, uint8_t sdr_byte, int max_ticks) {
    h.write_reg(RS_TALO, static_cast<uint8_t>(ta_load & 0xFF));
    h.write_reg(RS_TAHI, static_cast<uint8_t>((ta_load >> 8) & 0xFF));
    h.write_reg(RS_ICR, 0x88);   // set-mode (bit7=1), unmask SER (bit3)
    h.write_reg(RS_CRA, 0x41);   // spmode=1 (bit6), start=1 (bit0)
    h.dut->eclk = 1;
    if (!MUT_SKIP_SDR_WRITE)
        h.write_reg(RS_SDR, sdr_byte);
    const int budget_start = h.elapsed;
    while (h.elapsed - budget_start <= max_ticks) {
        h.tick();
        if (h.dut->irq) return h.elapsed;
    }
    return -1;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // FIXTURE-LIVENESS
    // =====================================================================
    {
        Harness h;
        checks.expect(fixture_live(h),
            "FIXTURE-LIVENESS: two distinct, known CIAADDRA bytes (0x55, "
            "0xAA) both arrive and read back correctly and distinctly "
            "before any data-dependent assertion is graded [no page -- "
            "infrastructure]");
    }
    // MUTATION FIXTURE-LIVENESS stimulus red MUT_FIXLIVE: repeat the first CIAADDRA byte (0x55) instead of writing a distinct second one (0xAA) -- the two readings can no longer be told apart -- baseline PASS goes red.

    // =====================================================================
    // Declined claims (record() only -- 15 of 27 uids; see SPECIFICATION.md's
    // duplicate-grading check for each citation verified against the cited
    // package's own test source).
    // =====================================================================

    checks.record(
        "WIDTH-8BIT (p.345, ch8-keyboard-serial-register, untagged assertion "
        "WIDTH-8BIT, opus:8-appG.c13/sol:8-appG.c20/opus:8-appG.c142/"
        "sol:8-appG.c170): \"The serial port is a buffered, 8-bit "
        "synchronous shift register.\" -- already measured at this "
        "identical ciaa boundary: sweeping the presented byte produces "
        "256 distinct register readbacks, i.e. the register really is 8 "
        "independent bits. Re-grading here would test the identical "
        "mechanism. Declined in full, not re-asserted. [claims: "
        "sol:F-sdr.c1, opus:F-sdr.c1]");

    checks.record(
        "KEYBOARD-USES-ONE-REGISTER (p.345, ch8-keyboard-serial-register "
        "PROP-1, opus:8-kbd.c1/sol:8-kbd.c1): \"In the Amiga system one "
        "shift register is used for the keyboard, and the other is "
        "unassigned.\" -- PROP-1 already grades, at this identical ciaa "
        "boundary, that the keyboard reaches the system through this "
        "register. The 'and the other is unassigned' half is a claim "
        "about ciab's shift register, out of this package's ciaa-only "
        "SPECIFICATION.md scope (see there) and not separately "
        "constructible as a distinct RTL requirement -- 'unassigned' names "
        "the absence of a binding, not a behaviour a fixture reads. "
        "Declined in full, not re-asserted. [claims: sol:F-sdr.c3, "
        "opus:F-sdr.c3]");

    checks.record(
        "INPUT-MODE-EIGHT-PULSES (p.345, ch8-keyboard-serial-register "
        "PROP-1 and PROP-2, opus:8-kbd.c1/sol:8-kbd.c1 and "
        "opus:8-kbd.c12/sol:8-kbd.c9): \"In input mode, data on the SP pin "
        "is shifted into the shift register on the rising edge of the "
        "signal applied to the CNT pin. After eight CNT pulses, the data "
        "in the shift register is dumped into the serial data register "
        "and an interrupt is generated.\" -- PROP-2's own specification "
        "already reads this phrase the same way this package would: "
        "'there is no bit counter here, so the sentence's phrase cannot "
        "be read as after eight individually clocked bits, only as after "
        "the byte-latch event' -- ciaa has no SP/CNT pin and no bit-serial "
        "input path at all (keycodes arrive as a whole byte on "
        "kbd_mouse_data via keystrobe, ciaa.v:186), so the CNT-pulse-count "
        "half of this input-mode sentence is not constructible, the same "
        "finding this package's own SPECIFICATION.md records independently "
        "for the page's MSB-first sentence. Declined in full, not "
        "re-asserted. [claims: sol:F-sdr.c6, opus:F-sdr.c6]");

    checks.record(
        "SDR-TABLE-ROWS (p.346, ch8-keyboard-serial-register PROP-1 "
        "[register identity at rs==4'hC, opus:8-kbd.c1/sol:8-kbd.c1] and "
        "untagged assertion WIDTH-8BIT [256 distinct readbacks, implying "
        "every bit position is independent, opus:8-appG.c13/sol:8-appG.c20/"
        "opus:8-appG.c142/sol:8-appG.c170]): the REG NAME table's SDR row "
        "(\"S7 S6 S5 S4 S3 S2 SI SO / C SDR\") -- each "
        "column restates, at one specific bit position, the same register-"
        "identity-and-width fact PROP-1/WIDTH-8BIT already grade as a "
        "whole. Re-grading any position here would test the identical "
        "mechanism. Declined in full, not re-asserted. [claims: "
        "sol:F-sdr.c24, sol:F-sdr.c25, sol:F-sdr.c26, sol:F-sdr.c27, "
        "sol:F-sdr.c28, sol:F-sdr.c29, sol:F-sdr.c30, sol:F-sdr.c31, "
        "opus:F-sdr.c23]");

    checks.note("Measured, not graded: p.345's \"SDR data is shifted out MSB first. Serial "
                "input data should appear in this same format\" carries no uid in this "
                "package -- both readers marked it status:not-asserted, reason absent: "
                "there is no bit-serial output path to observe an order on. Flagged for "
                "this build anyway: this sentence is the premise hrm/ch8-keyboard-serial-"
                "register's own bit map rests on. Appendix G states the keyboard's "
                "transmit ORDER (6-5-4-3-2-1-0-7); only this p.345 sentence states that "
                "the register fills MSB-first, which is what turns that order into the "
                "specific bit positions ciaa.v:186's `sdr_latch[7:0] <= "
                "~{kbd_mouse_data[6:0],kbd_mouse_data[7]}` lands them at -- confirmed "
                "read directly. Chapter 8 graded the composite permutation without this "
                "page. Reported per the build brief's instruction, adjudicated by nobody "
                "here.");

    // =====================================================================
    // Calibration (not itself a claim): two ticks_to_ser_complete() runs at
    // different Timer A latch values give two points on the line
    // ticks(L) = C + K*(L+1), where K = the number of Timer A underflow
    // pulses a full 8-bit transmission needs and C is fixed per-call setup
    // overhead (identical write_reg sequence both times). Solving:
    //   K = (ticks(L2) - ticks(L1)) / ((L2+1) - (L1+1))
    // HALF-RATE-SHIFT and MAX-BAUD-CEILING both read K and the directly-
    // measured minimum underflow period below; neither assumes K=16 --
    // the code computes it from these two measurements.
    // =====================================================================
    int ticks_L1, ticks_L4;
    {
        Harness h;
        ticks_L1 = ticks_to_ser_complete(h, /*ta_load=*/1, 0xA5, /*max_ticks=*/200);
    }
    {
        Harness h;
        ticks_L4 = ticks_to_ser_complete(h, /*ta_load=*/4, 0xA5, /*max_ticks=*/200);
    }
    const int K = (ticks_L1 > 0 && ticks_L4 > 0) ? (ticks_L4 - ticks_L1) / (4 - 1) : -1;
    checks.note("Calibration: ticks-to-completion at Timer A latch=1 is " +
                std::to_string(ticks_L1) + ", at latch=4 is " + std::to_string(ticks_L4) +
                " -> K (Timer A underflow pulses per completed transmission) = " +
                std::to_string(K) + ".");

    // Directly measure Timer A's own minimum underflow period (latch=1),
    // reading TALO (rs=0x4) after every tick -- no serial/output-mode
    // machinery involved, just the plain down-counter every appF-cia-*
    // package in this batch already relies on.
    int min_underflow_period = -1;
    {
        Harness h;
        h.write_reg(RS_TALO, 1);
        h.write_reg(RS_TAHI, 0);
        h.write_reg(RS_CRA, 0x01);  // start=1, spmode=0 -- plain counting, no serial logic
        h.dut->eclk = 1;
        int last_reload_tick = -1;
        uint8_t prev = h.read_reg(RS_TALO);
        for (int i = 1; i <= 8 && min_underflow_period < 0; ++i) {
            h.tick();
            const uint8_t cur = h.read_reg(RS_TALO);
            if (cur > prev) {  // TALO climbed back up -- a reload (underflow) just happened
                if (last_reload_tick > 0) min_underflow_period = i - last_reload_tick;
                last_reload_tick = i;
            }
            prev = cur;
        }
    }
    checks.note("Calibration: Timer A's own minimum underflow period (latch=1, plain "
                "counting mode) is " + std::to_string(min_underflow_period) + " clock tick(s).");

    // =====================================================================
    // TIMER-A-IS-GENERATOR
    // =====================================================================
    {
        Harness h;
        h.write_reg(RS_TALO, 1);
        h.write_reg(RS_TAHI, 0);
        h.write_reg(RS_TBLO, 1);
        h.write_reg(RS_TBHI, 0);
        h.write_reg(RS_ICR, 0x88);
        h.write_reg(RS_CRA, 0x40);  // spmode=1, start=0 -- Timer A NOT running
        h.write_reg(RS_CRB, 0x01);  // Timer B running fast (irrelevant if Timer A isolation holds)
        h.dut->eclk = 1;
        h.write_reg(RS_SDR, 0x3C);
        bool fired_with_only_b_running = false;
        for (int i = 0; i < 100 && !fired_with_only_b_running; ++i) {
            h.tick();
            if (h.dut->irq) fired_with_only_b_running = true;
        }
        if (!MUT_NO_TIMERA_ISOLATION)
            h.write_reg(RS_CRA, 0x41);  // now start Timer A too
        int after = -1;
        for (int i = 1; i <= 60; ++i) {
            h.tick();
            if (h.dut->irq) { after = i; break; }
        }
        checks.note("TIMER-A-IS-GENERATOR: with only Timer B running (100 ticks, far more "
                    "than the ~" + std::to_string(K > 0 ? K : 16) + " pulses a transmission "
                    "needs), fired=" + std::to_string(fired_with_only_b_running) +
                    "; after starting Timer A, completed after " + std::to_string(after) +
                    " further tick(s).");
        checks.expect(!fired_with_only_b_running && after > 0,
            "TIMER-A-IS-GENERATOR (p.345, sol:F-sdr.c7, opus:F-sdr.c7): \"In "
            "the output mode, Timer A is used as the baud rate generator.\" "
            "-- with Timer A stopped, transmission never completes no "
            "matter how long an independently-running Timer B ticks; "
            "starting Timer A specifically is what lets it complete.");
    }
    // MUTATION TIMER-A-IS-GENERATOR stimulus red MUT_NO_TIMERA_ISOLATION: skip starting Timer A in the second phase too (CRA's start bit is never written) -- Timer A never runs at all in this variant, so `after` stays -1 and the `after > 0` conjunct goes red.

    // =====================================================================
    // HALF-RATE-SHIFT
    // =====================================================================
    checks.expect(K == 16,
        "HALF-RATE-SHIFT (p.345, sol:F-sdr.c8, opus:F-sdr.c8): \"Data is "
        "shifted out on the SP pin at 1/2 the underflow rate of Timer A.\" "
        "-- operationalized as: completing an 8-bit transmission takes "
        "exactly twice as many Timer A underflow pulses as there are bits "
        "(K=16 for 8 bits), measured from two independent ticks-to-"
        "completion runs at different Timer A latch values (calibration "
        "above) rather than assumed.");
    // MUTATION HALF-RATE-SHIFT stimulus red MUT_SKIP_SDR_WRITE: (shared flag, see SDR-WRITE-STARTS-TX) never write SDR in either calibration run -- both ticks_to_ser_complete() calls return -1, so K's own guard clause forces K=-1, and the K == 16 conjunct goes red.

    // =====================================================================
    // MAX-BAUD-CEILING
    // =====================================================================
    const int min_bit_period = (min_underflow_period > 0) ? min_underflow_period * 2 : -1;
    checks.note("MAX-BAUD-CEILING: minimum underflow period=" + std::to_string(min_underflow_period) +
                " tick(s); combined with HALF-RATE-SHIFT's 2-underflows-per-bit factor, "
                "minimum achievable bit period=" + std::to_string(min_bit_period) +
                " tick(s) -- i.e. a maximum baud rate of (Timer A clock)/4.");
    checks.expect(min_underflow_period == 2 && K == 16,
        "MAX-BAUD-CEILING (p.345, sol:F-sdr.c9, opus:F-sdr.c9): \"The "
        "maximum baud rate possible is 02 divided by 4[, but the maximum "
        "usable baud rate will be determined by line loading and the "
        "speed at which the receiver responds to input data].\" -- "
        "operationalized as the two RTL-measured factors the printed "
        "ceiling is built from: Timer A's own fastest legal (non-zero-"
        "latch) underflow period is 2 Timer-A-clock ticks (the smallest "
        "nonzero down-counter latch, 1, takes 1 tick to reach zero plus 1 "
        "tick for the zero-held underflow edge), and HALF-RATE-SHIFT's own "
        "2-underflows-per-bit factor -- together giving a minimum bit "
        "period of 4 Timer-A-clock ticks, i.e. a ceiling of (Timer A "
        "clock)/4, matching the printed \"02 divided by 4\" numerically. "
        "The \"line loading\"/\"receiver response speed\" clause names "
        "external analog factors with no RTL counterpart at this boundary "
        "and is not graded.");
    // MUTATION MAX-BAUD-CEILING stimulus red MUT_SKIP_SDR_WRITE: (shared flag, see SDR-WRITE-STARTS-TX and HALF-RATE-SHIFT) never write SDR in either calibration run -- K is forced to -1 by the same guard clause, so the K == 16 conjunct goes red (min_underflow_period is unaffected by this flag, since it comes from a separate run that never touches SDR either way -- confirming the red result traces to K, the factor this flag actually perturbs).

    // =====================================================================
    // SDR-WRITE-STARTS-TX
    // =====================================================================
    {
        Harness h;
        h.write_reg(RS_TALO, 1);
        h.write_reg(RS_TAHI, 0);
        h.write_reg(RS_ICR, 0x88);
        h.write_reg(RS_CRA, 0x41);  // spmode + start -- Timer A running, but SDR untouched
        h.dut->eclk = 1;
        bool fired_without_write = false;
        for (int i = 0; i < 100 && !fired_without_write; ++i) {
            h.tick();
            if (h.dut->irq) fired_without_write = true;
        }
        int after = -1;
        if (!MUT_SKIP_SDR_WRITE) {
            h.write_reg(RS_SDR, 0xA5);
            for (int i = 1; i <= 40; ++i) {
                h.tick();
                if (h.dut->irq) { after = i; break; }
            }
        }
        checks.note("SDR-WRITE-STARTS-TX: with Timer A running in output mode but SDR "
                    "never written, 100 ticks (far more than K=" + std::to_string(K) +
                    " pulses' worth) produced fired=" + std::to_string(fired_without_write) +
                    "; after writing SDR, completed after " + std::to_string(after) +
                    " further tick(s).");
        checks.expect(!fired_without_write && after > 0,
            "SDR-WRITE-STARTS-TX (p.345, sol:F-sdr.c12, opus:F-sdr.c11): "
            "\"Transmission will start following a write to the serial "
            "data register.\" -- Timer A running in output mode, alone, "
            "never completes a transmission; writing SDR is what starts "
            "it, after which it completes on the same schedule the "
            "calibration above establishes.");
    }
    // MUTATION SDR-WRITE-STARTS-TX stimulus red MUT_SKIP_SDR_WRITE: never write SDR at all -- `after` stays -1, so the `after > 0` conjunct goes red (this also drives the shared ticks_to_ser_complete() helper's two calibration runs to -1/-1 when this flag is set, which is why this variant is built and run separately from the other MUT_* flags, never combined with them).

    // =====================================================================
    // EIGHT-PULSES-INTERRUPT
    // =====================================================================
    {
        Harness h;
        constexpr uint16_t TA_LOAD = 6;
        int completed = ticks_to_ser_complete(h, TA_LOAD, 0x5A, /*max_ticks=*/200);
        checks.note("EIGHT-PULSES-INTERRUPT: Timer A latch=" + std::to_string(TA_LOAD) +
                    ", completed after " + std::to_string(completed) + " tick(s).");
        // Cross-check against the calibration line ticks(L) = C + K*(L+1),
        // C solved from the latch=1 run above: C = ticks_L1 - K*2.
        const int C = (K > 0 && ticks_L1 > 0) ? ticks_L1 - K * 2 : -1;
        const int predicted = (K > 0 && ticks_L1 > 0) ? C + K * (static_cast<int>(TA_LOAD) + 1) : -1;
        checks.note("EIGHT-PULSES-INTERRUPT: predicted completion tick from the calibration "
                    "line (C=" + std::to_string(C) + ", K=" + std::to_string(K) + ") is " +
                    std::to_string(predicted) + ".");
        checks.expect(completed > 0 && completed == predicted,
            "EIGHT-PULSES-INTERRUPT (p.345, sol:F-sdr.c16, opus:F-sdr.c15): "
            "\"After eight CNT pulses, an interrupt is generated to "
            "indicate that more data can be sent.\" -- a third, "
            "independent Timer A latch value's measured completion tick "
            "matches the calibration line ticks(L)=C+K*(L+1) exactly, "
            "confirming the interrupt fires at the K-th Timer-A-underflow "
            "boundary (K=16 underflow pulses = \"eight CNT pulses\", per "
            "HALF-RATE-SHIFT's own 2-underflows-per-CNT-pulse finding) at "
            "this third, previously-unused configuration too, not merely "
            "at the two points K was fitted from.");
    }
    // MUTATION EIGHT-PULSES-INTERRUPT stimulus red MUT_SKIP_SDR_WRITE: (shared flag, see SDR-WRITE-STARTS-TX) never write SDR -- ticks_to_ser_complete() returns -1 for this assertion's own run too, so `completed > 0` goes red; the same flag also drives the K/C calibration runs to -1/-1, so `predicted` is also -1 -- both conjuncts fail together, confirming this assertion is not vacuously true when the underlying mechanism never fires.

    // =====================================================================
    // RELOAD-CONTINUES
    // =====================================================================
    {
        Harness h;
        constexpr uint16_t TA_LOAD = 4;
        h.write_reg(RS_TALO, static_cast<uint8_t>(TA_LOAD));
        h.write_reg(RS_TAHI, 0);
        h.write_reg(RS_ICR, 0x88);
        h.write_reg(RS_CRA, 0x41);
        h.dut->eclk = 1;
        h.write_reg(RS_SDR, 0x11);  // first byte -- starts transmission

        const int C = (K > 0 && ticks_L1 > 0) ? ticks_L1 - K * 2 : -1;
        const int predicted_first = (K > 0 && ticks_L1 > 0) ? C + K * (static_cast<int>(TA_LOAD) + 1) : -1;
        // Reload at roughly the halfway point -- well before predicted_first.
        // An ABSOLUTE h.elapsed target, comparable directly to predicted_first
        // and to what ticks_to_ser_complete() returns (also h.elapsed, not a
        // caller-local loop index) -- see the Harness::elapsed comment for why
        // a loop-local counter would be thrown off by the reload's own write.
        const int reload_at = (MUT_RESTART_ON_RELOAD_CHECK || predicted_first <= 0) ? -1 : predicted_first / 2;

        int completed = -1;
        bool reloaded = false;
        const int budget_start = h.elapsed;
        while (h.elapsed - budget_start <= 400) {
            h.tick();
            if (!reloaded && reload_at > 0 && h.elapsed >= reload_at) {
                h.write_reg(RS_SDR, 0x99);  // reload mid-transmission with new data
                reloaded = true;
            }
            if (h.dut->irq) { completed = h.elapsed; break; }
        }
        const uint8_t sdr_after = h.read_reg(RS_SDR);

        checks.note("RELOAD-CONTINUES: latch=" + std::to_string(TA_LOAD) + ", first-byte "
                    "predicted completion=" + std::to_string(predicted_first) + ", reloaded "
                    "at tick=" + std::to_string(reload_at) + ", actual completion=" +
                    std::to_string(completed) + ", SDR reads back 0x" + hex8(sdr_after) +
                    " after completion (0x99 = the reload's byte, 0x11 = the original).");
        checks.expect(
            completed == predicted_first && sdr_after == 0x99,
            "RELOAD-CONTINUES (p.345, sol:F-sdr.c17, opus:F-sdr.c16): \"If "
            "the serial data register was reloaded with new information "
            "prior to this interrupt, the new data will automatically be "
            "loaded into the shift register and transmission will "
            "continue.\" -- writing SDR again partway through an "
            "in-progress transmission does not restart the pulse count "
            "(completion still lands exactly where the first write alone "
            "predicted -- transmission \"continued\", not restarted), and "
            "the reloaded byte is what SDR holds once transmission "
            "completes.");
    }
    // MUTATION RELOAD-CONTINUES stimulus red MUT_RESTART_ON_RELOAD_CHECK: skip the mid-transmission reload entirely (reload_at forced to -1, so SDR is never rewritten to 0x99) -- sdr_after stays 0x11 (the original byte), so the sdr_after == 0x99 conjunct goes red.

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
