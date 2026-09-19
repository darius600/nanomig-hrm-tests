// Test purpose: HRM conformance for ch8-serper-baud-divider -- SERPER
// ($DFF032, write-only), the 15-bit RATE divider field, the LONG control
// bit, and the register-summary/appendix rows that restate the same facts,
// at the `paula_uart` boundary. Per SPECIFICATION.md: 73 claims, reduced to
// eight testable propositions (A-H), all expected GREEN.
//
// Primary oracle: the manual quotations cited inline (see
// SPECIFICATION.md's Q1-Q12), not the RTL. Every numeric prediction is
// `(N+1) x U` or `2^k x U`, U=2 enables/colour-clock, derived from the
// manual's own N+1 sentence (p.255/283) plus exactly one externally
// disclosed calibration fact (hrm/ch8-uart-frame-and-status/test_paula_uart.cpp:87-88's
// already-accepted 22-enables-for-SERPER=10 case) -- never a constant read
// out of paula_uart.v's own counter arithmetic. See SPECIFICATION.md,
// "Calibration".
//
// Boundary (see SPECIFICATION.md): no claim here is graded against an
// absolute microsecond value, a bits/second baud rate, or an NTSC/PAL
// distinction -- `ntsc` never reaches `paula_uart`, and the design's one
// colour clock carries the PAL constant only as a comment
// (amiga_clk.v:11), so no port here can construct either 279.36ns or
// 281.94ns. Only the dimensionless ratio and the binary place value of
// each RATE bit are graded.
//
// Fixture hazards (see SPECIFICATION.md): (1) paula_uart has no read/write
// strobe -- reading $030/$032 the ordinary way (holding rga_i through a
// clk7_en-qualifying edge) WRITES data_i into that register. This file's
// read_reg_safe() never advances a qualifying edge; data_o is a
// continuous `assign` (paula_uart.v:258), so it can be read by evaluating
// combinational logic alone. (2) `serper` has no reset term
// (paula_uart.v:49-55) -- every trial below that cares about SERPER's
// value constructs a fresh Rig (and therefore a fresh Vpaula_uart) and
// calls do_reset() exactly once, before its first write.
//
// Non-goals: SERDAT's own transmit content/stop-bit semantics
// (ch8-serdat-transmit), SERDATR's own read semantics and the two source
// conflicts on its bits 13/8 (ch8-serdatr-read-word), and ADKCON/INTREQ
// (ch8-adkcon-and-interrupt-relations, invisible at this boundary) are
// siblings' business, not re-graded here even where this file's own
// fixture happens to exercise SERDAT or SERDATR as plumbing.
//
// Test class: HRM_CONFORMANCE
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// MUTATION METHODOLOGY: one `#if MUT_<X>` compile-time flag per graded
// assertion (A-H), built via `make MUT=MUT_<X> test` (Makefile: `MUT ?=` /
// `-CFLAGS -D$(MUT)=1`, `rm -rf obj_dir` inside the `test` recipe itself,
// so a stale binary from a different MUT value is never reused). Unset
// MUT builds the baseline. All eight assertions are expected green at
// baseline, so all eight mutations are "stimulus red": each changes what
// the rig asks the module to do while leaving every expected value
// (derived once, from the manual + calibration, in SPECIFICATION.md)
// alone. See the bottom of this file for the measured MUTATION log,
// filled in after each variant was actually built and run under WSL.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "Vpaula_uart.h"
#include "verilated.h"

#ifndef MUT_A
#define MUT_A 0
#endif
#ifndef MUT_B
#define MUT_B 0
#endif
#ifndef MUT_C
#define MUT_C 0
#endif
#ifndef MUT_D
#define MUT_D 0
#endif
#ifndef MUT_E
#define MUT_E 0
#endif
#ifndef MUT_F
#define MUT_F 0
#endif
#ifndef MUT_G
#define MUT_G 0
#endif
#ifndef MUT_H
#define MUT_H 0
#endif

namespace {

// Register addresses, as rga_i (the $DFFxxx word offset, addr>>1) --
// paula_uart.v's own localparams REG_SERDAT=9'h030, REG_SERDATR=9'h018,
// REG_SERPER=9'h032, each right-shifted by 1 at the call site
// (paula_uart.v:52,61,258 all compare against rga_i, an 8-bit value).
constexpr uint8_t RGA_IDLE    = 0x00;  // matches no decode; safe park address
constexpr uint8_t RGA_SERDATR = 0x0C;  // $018 >> 1
constexpr uint8_t RGA_SERDAT  = 0x18;  // $030 >> 1
constexpr uint8_t RGA_SERPER  = 0x19;  // $032 >> 1
constexpr uint8_t RGA_034     = 0x1A;  // $034 (POTGO) -- decoy, assertion A
constexpr uint8_t RGA_02E     = 0x17;  // $02E (COPCON) -- decoy, assertion A
constexpr uint8_t RGA_036     = 0x1B;  // $036 (JOYTEST) -- decoy, assertion A
constexpr uint8_t RGA_1FE     = 0xFF;  // $1FE -- decoy, assertion A

std::string hex4(uint32_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "0x%04X", (unsigned)(v & 0xFFFF));
    return std::string(b);
}

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

// A fresh Vpaula_uart per trial -- `serper` has no reset term
// (paula_uart.v:49-55), so a reused model would carry a previous trial's
// period, exactly the hazard the build brief names.
struct Rig {
    Vpaula_uart* dut;
    int clk7_cycle = 0;

    Rig() { dut = new Vpaula_uart; }
    ~Rig() { delete dut; }

    // Same divide-by-4 clk7_en cadence as hrm/ch8-uart-frame-and-status/test_paula_uart.cpp
    // (the calibration source this package's numbers are checked against):
    // one clk7_en-qualifying rising edge every 4 clk edges.
    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    }

    // Used only by assertion E: toggles clk without ever letting clk7_en
    // assert, to show that raw clk alone (no colour-clock enable) advances
    // nothing.
    void tick_no_clk7_en() {
        dut->clk7_en = 0;
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    void do_reset() {
        clk7_cycle = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->rga_i = RGA_IDLE;
        dut->data_i = 0;
        dut->uartbrk = 0;
        dut->rbfmirror = 0;
        dut->rxd = 1;  // idle high
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    }

    // Advance until exactly n clk7_en-qualifying rising edges have
    // occurred (clk7_en, as read here, is the value that will gate the
    // UPCOMING tick()'s rising edge, per this cadence's own generation).
    void settle_edges(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    // One qualifying-edge write, then rga_i is parked back on RGA_IDLE so
    // no later qualifying edge repeats it, and so a later SAFE read of a
    // different address is never preceded by a lingering write-matching
    // rga_i.
    void write_reg(uint8_t rga, uint16_t data) {
        dut->rga_i = rga;
        dut->data_i = data;
        settle_edges(1);
        dut->rga_i = RGA_IDLE;
        dut->data_i = 0;
        settle_edges(1);
    }

    // SAFE READ: data_o is a continuous `assign` (paula_uart.v:258), pure
    // combinational logic off rga_i and the module's internal registers --
    // no clock edge is needed to resolve it. This peeks it by setting
    // rga_i and calling eval() alone, WITHOUT ever advancing a
    // clk7_en-qualifying posedge, so it cannot trigger the "reading is a
    // write" fixture hazard disclosed in the build brief (an ordinary
    // helper that ticks through a qualifying edge while rga_i points at
    // $030/$032 WOULD write data_i into that register on that same edge,
    // since paula_uart has no read/write strobe).
    uint16_t read_reg_safe(uint8_t rga) {
        dut->rga_i = rga;
        dut->eval();
        uint16_t v = (uint16_t)dut->data_o;
        dut->rga_i = RGA_IDLE;
        dut->eval();
        return v;
    }
};

// Triggers a transmission (SERDAT <= 0x5555, a start bit then a perfectly
// alternating 16-bit pattern chosen so every consecutive shifted bit
// differs from the one before it -- guaranteeing a txd transition at
// every single bit-cell boundary, with no risk of two equal consecutive
// bits hiding a transition) and returns the number of clk7_en enables
// between the FIRST transition (the start bit falling) and the SECOND
// (the first data bit) -- exactly one bit cell. Assumes SERPER is already
// in whatever state the caller wants (possibly written through a decoy
// address, for assertion A).
int measure_tx_cell_now(Rig& rig) {
    // The SERDAT write and the txd monitor share ONE loop, deliberately:
    // a separate write_reg() call before monitoring starts would consume
    // the address-write edge (which only clears tx_tbe) AND the following
    // edge (TX_IDLE->TX_SHIFT, txd's actual 1->0 transition,
    // paula_uart.v:91-104 -- tx_tbe is checked with its PRE-edge value, so
    // the state transition lags the write by one qualifying edge) inside
    // its own settle_edges() bookkeeping, before last_txd could be
    // initialised from the true pre-transition value -- silently
    // discarding the first transition and undercounting every absolute
    // cell length by exactly one enable (found the hard way: the first
    // version of this rig measured 21 for SERPER=10, not the calibrated
    // 22, matching the very hazard the build brief names). Folding the
    // trigger into the monitor loop means last_txd is read from the
    // actual current state before any qualifying edge in this function
    // has run, so both transitions are caught.
    rig.dut->rga_i = RGA_SERDAT;
    rig.dut->data_i = 0x5555;

    long enable_counter = 0;
    int last_txd = (int)rig.dut->txd;
    std::vector<long> transition_at;
    bool parked = false;
    while (enable_counter < 250000 && transition_at.size() < 2) {
        bool q = rig.dut->clk7_en;
        rig.tick();
        if (q) {
            enable_counter++;
            if (!parked) {
                rig.dut->rga_i = RGA_IDLE;
                rig.dut->data_i = 0;
                parked = true;
            }
            int cur = (int)rig.dut->txd;
            if (cur != last_txd) transition_at.push_back(enable_counter);
            last_txd = cur;
        }
    }
    if (transition_at.size() < 2) return -1;
    return (int)(transition_at[1] - transition_at[0]);
}

int measure_tx_cell(Rig& rig, uint16_t serper_value) {
    rig.write_reg(RGA_SERPER, serper_value);
    return measure_tx_cell_now(rig);
}

// Drives one start bit (rxd low for exactly one calibrated bit-cell
// period at N, comfortably longer than RX_START's own half-period sample
// point) then releases rxd to idle (mark, 1) for the remainder of the
// reception -- content after the start bit does not affect WHEN rxint
// fires (only LONG/bit 15 does; the marker that decides the round rx_shift
// terminates on is the start-bit indicator and, when LONG=0, LONG's own
// value, both structural, not data-dependent -- see SPECIFICATION.md,
// assertion G). Returns the number of clk7_en enables from the driven
// start bit to the rxint pulse.
int measure_rx_elapsed_to_rxint(Rig& rig, uint16_t n, bool longbit) {
    uint16_t serper_val = (uint16_t)((n & 0x7FFF) | (longbit ? 0x8000 : 0));
    rig.write_reg(RGA_SERPER, serper_val);
    rig.dut->rxd = 1;
    rig.settle_edges(4);

    const long cell = 2L * (n + 1);
    rig.dut->rxd = 0;  // drive the start bit now; count from here

    long enable_counter = 0;
    long start_edge = -1;
    bool started = false;
    bool released = false;
    long guard = 0;
    while (guard < 400000) {
        bool q = rig.dut->clk7_en;
        rig.tick();
        if (q) {
            enable_counter++;
            guard++;
            if (!started) { started = true; start_edge = enable_counter; }
            if (!released && (enable_counter - start_edge) >= cell) {
                rig.dut->rxd = 1;
                released = true;
            }
            if (rig.dut->rxint) return (int)(enable_counter - start_edge);
        }
    }
    return -1;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ========================================================================
    // LIVENESS / CALIBRATION -- no claim uid, MEASURED not GRADED. Checks the
    // rig against the disclosed known case before anything below trusts a
    // number derived from it (build brief: "INSTRUMENT CHECKED against a
    // known case first" -- classify's own note that a first rig measured 21
    // and was wrong is the reason this runs first, unconditionally).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        int calib = measure_tx_cell(rig, 10);
        checks.note("LIVENESS/CALIBRATION: SERPER=10 (0x000A) transmitting 0x5555 gives a txd bit cell of " +
            std::to_string(calib) + " clk7_en enables -- must equal 22 to match the already-accepted known case "
            "at hrm/ch8-uart-frame-and-status/test_paula_uart.cpp:87-88. U (enables per colour clock) = 22/(10+1) = 2, the sole "
            "derived constant behind every numeric prediction below (SPECIFICATION.md, Calibration). No claim "
            "uid: this is a fixture check, not conformance evidence.");
    }

    // ========================================================================
    // A -- SERPER-ADDRESS-DECODE (opus:8-serper.c3, sol:8-serper.c16,
    // sol:8-serper.c29; p.283/289, "SERPER 032 W P Serial port period and
    // control"). SERPER is decoded uniquely at $032: writing N=10 there and
    // transmitting gives the calibrated 22-enable cell; the identical write
    // to seven other addresses, each on a fresh model, leaves the cell at
    // its reset value (N=0, 2 enables).
    // ========================================================================
    {
        struct AddrCase { uint8_t rga; const char* label; bool is_target; };
        AddrCase cases[] = {
            {RGA_SERPER, "$032(SERPER)", true},
            {RGA_SERDAT, "$030(SERDAT)", false},
            {RGA_034,    "$034(POTGO)", false},
            {RGA_SERDATR,"$018(SERDATR)", false},
            {RGA_02E,    "$02E(COPCON)", false},
            {RGA_036,    "$036(JOYTEST)", false},
            {RGA_IDLE,   "$000", false},
            {RGA_1FE,    "$1FE", false},
        };
        bool all_ok = true;
        std::string detail;
        for (auto& c : cases) {
            uint8_t rga = c.rga;
#if MUT_A
            if (c.is_target) rga = RGA_034;  // write the "10" to $034 instead of $032
#endif
            Rig rig;
            rig.do_reset();
            rig.write_reg(rga, 10);
            int cell = measure_tx_cell_now(rig);
            bool ok = c.is_target ? (cell == 22) : (cell == 2);
            all_ok &= ok;
            detail += std::string(c.label) + ":" + std::to_string(cell) + " ";
        }
        checks.note("A-STIMULUS: cell (enables) per address after writing 10 there -- " + detail);
        checks.expect(all_ok,
            "A-SERPER-ADDRESS-DECODE (p.283/289 'SERPER 032 W P Serial port period and control'): writing N=10 "
            "to $032 loads the divider (cell=22, matching the (N+1) calibration); the identical write to $030, "
            "$034, $018, $02E, $036, $000 and $1FE, each on a fresh model, leaves the divider at its reset value "
            "(cell=2) -- SERPER is decoded uniquely at $032 [claims: opus:8-serper.c3, sol:8-serper.c16, "
            "sol:8-serper.c29]");
    }

    // ========================================================================
    // B -- SERPER-WRITE-ONLY (opus:8-serper.c4, sol:8-serper.c12,
    // sol:8-serper.c17, sol:8-serper.c30; p.256/283/289 "W"/"write-only",
    // composed with the legend at appB3/p287.txt:30, "W=write-only"). Four
    // distinct words written to $032 in turn, each followed by a SAFE read
    // of $032: every read returns 0x0000. Control, same run: a SAFE read of
    // $018 (SERDATR) returns a nonzero value -- the read path itself is
    // live, so "$032 never reads back" is SERPER's own write-only property,
    // not a broken read bus.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        const uint16_t write_vals[] = {0xBEEF, 0x0000, 0xFFFF, 0x800A};
        bool all_zero = true;
        std::string detail;
        for (uint16_t v : write_vals) {
            rig.write_reg(RGA_SERPER, v);
            uint16_t r = rig.read_reg_safe(RGA_SERPER);
            if (r != 0x0000) all_zero = false;
            detail += "wrote " + hex4(v) + " -> read " + hex4(r) + "; ";
        }
        uint8_t control_addr = RGA_SERDATR;
#if MUT_B
        control_addr = RGA_SERPER;  // redirect the control read to $032 too
#endif
        uint16_t control_read = rig.read_reg_safe(control_addr);
        checks.note("B-STIMULUS: " + detail + "control read at rga=0x" +
            std::to_string((unsigned)control_addr) + " -> " + hex4(control_read));
        checks.expect(all_zero && control_read != 0x0000,
            "B-SERPER-WRITE-ONLY (p.283/289 'SERPER 032 W ...'; p.256 'the write-only SERPER register'; "
            "appB3/p287.txt:30 'W=write-only'): four distinct words written to $032 always read back 0x0000 "
            "(" + detail + "), while the identical SAFE-read mechanism returns a nonzero value at $018 (SERDATR) "
            "in the same run (control: the read path itself is live) [claims: opus:8-serper.c4, "
            "sol:8-serper.c12, sol:8-serper.c17, sol:8-serper.c30]");
    }

    // ========================================================================
    // C -- SERPER-FUNCTION-PERIOD-AND-CONTROL (opus:8-serper.c6,
    // sol:8-serper.c19, sol:8-serper.c32; p.283/289 "Serial port period and
    // control"). The claimed function is a compound of two mechanisms this
    // package proves independently elsewhere (D, G): graded here as the
    // conjunction of a period sub-check (SERPER=10 gives the calibrated
    // 22-enable cell) and a control sub-check (toggling LONG shifts the
    // receive-completion event by exactly one bit cell), both measured
    // fresh in this block.
    // ========================================================================
    {
        int period_cell;
        {
            Rig rig;
            rig.do_reset();
#if MUT_C
            // Period sub-check stimulus: SERPER is never written (stays at
            // its reset value 0) while the check still expects the
            // SERPER=10-derived cell count.
            period_cell = measure_tx_cell_now(rig);
#else
            period_cell = measure_tx_cell(rig, 10);
#endif
        }
        bool period_ok = (period_cell == 22);

        int e0, e1;
        {
            Rig rig0;
            rig0.do_reset();
            e0 = measure_rx_elapsed_to_rxint(rig0, 10, false);
        }
        {
            Rig rig1;
            rig1.do_reset();
            e1 = measure_rx_elapsed_to_rxint(rig1, 10, true);
        }
        bool control_ok = (e0 > 0) && (e1 > 0) && ((e1 - e0) == 22);

        checks.note("C-STIMULUS: period sub-check cell=" + std::to_string(period_cell) +
            " (expect 22); control sub-check elapsed enables LONG=0:" + std::to_string(e0) +
            " LONG=1:" + std::to_string(e1));
        checks.expect(period_ok && control_ok,
            "C-SERPER-FUNCTION-PERIOD-AND-CONTROL (p.283 'SERPER 032 W P Serial port period and control'; "
            "p.289 same row): both named halves of the function hold -- the period mechanism (SERPER=10 gives "
            "cell=" + std::to_string(period_cell) + ", expected 22) and the control-bit mechanism (LONG shifts "
            "the receive-completion event by exactly one calibrated bit cell, " + std::to_string(e0) + " vs " +
            std::to_string(e1) + " enables) [claims: opus:8-serper.c6, sol:8-serper.c19, sol:8-serper.c32]");
    }

    // ========================================================================
    // D -- SERPER-RATIO-N-PLUS-ONE (opus:8-serper.c9, opus:8-serper.c16,
    // sol:8-serper.c1, sol:8-serper.c6, sol:8-serper.c7, sol:8-serper.c22,
    // plus 15 per-bit "contributes to RATE's formula" claims -- 21 uids;
    // p.255 "N+1 color clocks..."; p.283 "the baud rate is 1 bit every
    // (N+1)*.2794 microseconds" / "RATE ... 1/((N+1)*.2794 microsec.)").
    // Dimensionless only (SPECIFICATION.md, Boundary): period(N) = (N+1) x
    // 2 enables, swept over a representative set of N.
    // ========================================================================
    {
        struct NCase { uint16_t n; const char* label; };
        static const NCase ncases[] = {
            {0, "N=0"}, {1, "N=1"}, {2, "N=2"}, {3, "N=3"}, {10, "N=10"},
            {128, "N=128"}, {16384, "N=16384"}, {32767, "N=32767"},
        };
        bool all_ok = true;
        std::string detail;
        for (auto& c : ncases) {
            uint16_t n = c.n;
#if MUT_D
            if (c.n == 10) n = 9;  // write N=9 instead of N=10
#endif
            Rig rig;
            rig.do_reset();
            int cell = measure_tx_cell(rig, n);
            int expected = 2 * (c.n + 1);  // oracle always keyed to the LABEL's N, not the (possibly mutated) stimulus
            bool ok = (cell == expected);
            all_ok &= ok;
            detail += std::string(c.label) + ":" + std::to_string(cell) + "(exp " + std::to_string(expected) + ") ";
        }
        checks.note("D-STIMULUS: " + detail);
        checks.expect(all_ok,
            "D-SERPER-RATIO-N-PLUS-ONE (p.255 'If the SERPER divisor is set to the number N, then N+1 color "
            "clocks occur between samples ... or between transmissions'; p.283 'If this number is N, then the "
            "baud rate is 1 bit every (N+1)*.2794 microseconds' / '14-00 RATE Defines baud rate=1/((N+1)*.2794 "
            "microsec.)', dimensionless part only): period(N) = (N+1) x 2 enables holds across N in "
            "{0,1,2,3,10,128,16384,32767} -- " + detail + "[claims: opus:8-serper.c9, opus:8-serper.c16, "
            "sol:8-serper.c1, sol:8-serper.c6, sol:8-serper.c7, sol:8-serper.c22, sol:8-serper.c28, "
            "sol:8-serper.c52, sol:8-serper.c55, sol:8-serper.c58, sol:8-serper.c61, sol:8-serper.c64, "
            "sol:8-serper.c67, sol:8-serper.c70, sol:8-serper.c73, sol:8-serper.c76, sol:8-serper.c79, "
            "sol:8-serper.c82, sol:8-serper.c85, sol:8-serper.c88, sol:8-serper.c91]");
    }

    // ========================================================================
    // E -- SERPER-TIMEBASE-IS-COLOUR-CLOCK (sol:8-serper.c3; p.255 "All
    // timing is done on the basis of a 'color clock'"). With a transmission
    // started, raw clk alone -- advanced for an extended window with
    // clk7_en held permanently deasserted -- produces zero txd transitions.
    // Rig sanity is established once, up front, by LIVENESS/CALIBRATION,
    // not folded into this assertion's own condition.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(RGA_SERPER, 10);
        rig.write_reg(RGA_SERDAT, 0x5555);

        int guard = 0;
        while (rig.dut->txd != 0 && guard < 2000) {
            rig.tick();
            guard++;
        }
        bool started = (rig.dut->txd == 0);
        int level_at_start = (int)rig.dut->txd;

        bool changed = false;
        for (int i = 0; i < 2000; i++) {
#if MUT_E
            rig.tick();  // clk7_en allowed to pulse normally during this window
#else
            rig.tick_no_clk7_en();
#endif
            if ((int)rig.dut->txd != level_at_start) { changed = true; break; }
        }
        checks.note("E-STIMULUS: transmission started=" + std::string(started ? "yes" : "no") +
            "; txd changed during the window=" + std::string(changed ? "yes" : "no"));
        checks.expect(!changed,
            "E-SERPER-TIMEBASE-IS-COLOUR-CLOCK (p.255 'All timing is done on the basis of a \"color clock\"'): "
            "with a transmission in progress, 2000 raw clk edges with clk7_en held permanently deasserted "
            "produce zero txd transitions -- nothing advances except on a clk7_en (colour-clock) enable "
            "[claims: sol:8-serper.c3]");
    }

    // ========================================================================
    // F -- SERPER-BIT15-OUTSIDE-PERIOD (opus:8-serper.c8, opus:8-serper.c15,
    // sol:8-serper.c21; p.283 "a 15-bit number defining the serial port baud
    // rate" / "14-00 RATE"). The period-controlling number is exactly 15
    // bits (14-0): with N held at 10, the transmitted bit cell is identical
    // with bit 15 clear and with bit 15 set.
    // ========================================================================
    {
        int cell_clear, cell_set;
        {
            Rig rig;
            rig.do_reset();
            cell_clear = measure_tx_cell(rig, 10);  // 0x000A
        }
        {
            Rig rig;
            rig.do_reset();
            uint16_t n2 = 10;
#if MUT_F
            n2 = 11;  // the bit15-set trial uses a different N
#endif
            cell_set = measure_tx_cell(rig, (uint16_t)(0x8000 | n2));
        }
        checks.note("F-STIMULUS: cell with bit15 clear=" + std::to_string(cell_clear) +
            ", bit15 set=" + std::to_string(cell_set));
        checks.expect(cell_clear == 22 && cell_set == 22,
            "F-SERPER-BIT15-OUTSIDE-PERIOD (p.283 'This register contains the control bit LONG ... and a "
            "15-bit number defining the serial port baud rate'; '14-00 RATE Defines baud rate...'): with N=10 "
            "held fixed, the transmitted bit cell is identical (22 enables) with bit 15 clear (0x000A) and set "
            "(0x800A) -- the divider is exactly 15 bits wide [claims: opus:8-serper.c8, opus:8-serper.c15, "
            "sol:8-serper.c21]");
    }

    // ========================================================================
    // G -- SERPER-LONG-BIT15-CONTROL (opus:8-serper.c7, opus:8-serper.c13,
    // sol:8-serper.c11, sol:8-serper.c13, sol:8-serper.c14, sol:8-serper.c20,
    // sol:8-serper.c23, sol:8-serper.c25; p.256/283). Bit 15 (LONG) is a
    // real control bit: nine data bits (LONG=1) takes exactly one
    // calibrated bit-cell period longer to receive than eight (LONG=0),
    // measured directly as an elapsed-enable delta -- not by reading the RX
    // shift register's internal round index.
    // ========================================================================
    {
        int e0, e1;
        {
            Rig rig0;
            rig0.do_reset();
            e0 = measure_rx_elapsed_to_rxint(rig0, 10, false);
        }
        {
            Rig rig1;
            rig1.do_reset();
            bool longbit = true;
#if MUT_G
            longbit = false;  // collapse the intended contrast
#endif
            e1 = measure_rx_elapsed_to_rxint(rig1, 10, longbit);
        }
        checks.note("G-STIMULUS: elapsed enables to rxint -- LONG=0: " + std::to_string(e0) +
            ", LONG=1 trial: " + std::to_string(e1));
        checks.expect(e0 > 0 && e1 > 0 && (e1 - e0) == 22,
            "G-SERPER-LONG-BIT15-CONTROL (p.256 'Receive mode is set by bit 15 of the write-only SERPER "
            "register. Bit 15 is a 1 if you chose nine data bits ... and a 0 if you chose eight data bits'; "
            "p.283 '15 LONG Defines serial receive as 9-bit word'; 'This register contains the control bit "
            "LONG'): nine data bits (LONG=1) takes exactly one calibrated bit-cell period (22 enables) longer "
            "to complete than eight data bits (LONG=0) -- elapsed " + std::to_string(e0) + " vs " +
            std::to_string(e1) + " enables [claims: opus:8-serper.c7, opus:8-serper.c13, sol:8-serper.c11, "
            "sol:8-serper.c13, sol:8-serper.c14, sol:8-serper.c20, sol:8-serper.c23, sol:8-serper.c25]");
    }

    // ========================================================================
    // H -- SERPER-RATE-BITFIELD-POSITION (15 "bit N is a baud-rate-divider
    // bit" uids, p.255, plus 15 "one RATE-field position is bit N" uids,
    // p.283 -- 30 uids). Each of the 15 RATE bits acts at its own
    // independent binary place value: isolating bit k alone moves the bit
    // cell by exactly 2^k x 2 enables versus the N=0 baseline, for every
    // k=0..14.
    // ========================================================================
    {
        int baseline;
        {
            Rig rig;
            rig.do_reset();
            baseline = measure_tx_cell(rig, 0);
        }
        bool all_ok = true;
        std::string detail;
        for (int k = 0; k <= 14; k++) {
            int written_k = k;
#if MUT_H
            if (k == 7) written_k = 8;  // write bit 8 instead of bit 7
#endif
            uint16_t n_written = (uint16_t)(1u << written_k);
            Rig rig;
            rig.do_reset();
            int cell = measure_tx_cell(rig, n_written);
            int delta = cell - baseline;
            int expected_delta = (1 << k) * 2;  // oracle always keyed to the LABEL's own bit k
            bool ok = (delta == expected_delta);
            all_ok &= ok;
            detail += "bit" + std::to_string(k) + ":delta=" + std::to_string(delta) +
                      "(exp " + std::to_string(expected_delta) + ") ";
        }
        checks.note("H-STIMULUS: baseline(N=0)=" + std::to_string(baseline) + "; " + detail);
        checks.expect(all_ok,
            "H-SERPER-RATE-BITFIELD-POSITION (p.255 'Bits 14-0 of SERPER are the baud-rate divider bits'; "
            "p.283 '14-00 RATE Defines baud rate=1/((N+1)*.2794 microsec.)'): isolating each RATE bit k "
            "(k=0..14) alone moves the bit cell by exactly 2^k x 2 enables versus the N=0 baseline, for every "
            "one of the 15 bits -- " + detail + "[claims: sol:8-serper.c2, sol:8-serper.c36, sol:8-serper.c37, "
            "sol:8-serper.c38, sol:8-serper.c39, sol:8-serper.c40, sol:8-serper.c41, sol:8-serper.c42, "
            "sol:8-serper.c43, sol:8-serper.c44, sol:8-serper.c45, sol:8-serper.c46, sol:8-serper.c47, "
            "sol:8-serper.c48, sol:8-serper.c49, sol:8-serper.c26, sol:8-serper.c50, sol:8-serper.c53, "
            "sol:8-serper.c56, sol:8-serper.c59, sol:8-serper.c62, sol:8-serper.c65, sol:8-serper.c68, "
            "sol:8-serper.c71, sol:8-serper.c74, sol:8-serper.c77, sol:8-serper.c80, sol:8-serper.c83, "
            "sol:8-serper.c86, sol:8-serper.c89]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION run log -- each variant actually built and run under WSL
// (`make MUT=MUT_<X> test`), one at a time (obj_dir is shared per build,
// so variants cannot run concurrently). Baseline: 8 passed, 0 failed.
// Every one of the eight variants below produced exactly 7 passed, 1
// failed -- the failing label is named on its own line; no other
// assertion's PASS/FAIL changed in any of the eight runs (verified by
// diffing each run's full PASS/FAIL block against baseline).
//
// MUTATION A-SERPER-ADDRESS-DECODE stimulus red: the "10" meant for $032
// was written to $034 instead (decoy address), while the check still
// expects the $032-addressed trial to show the loaded period (cell=22) --
// $032 itself was left at its reset value (cell=2) under this mutation,
// so the check correctly went red (MUT=MUT_A).
// MUTATION B-SERPER-WRITE-ONLY stimulus red: the control read (meant to
// prove the SAFE-read mechanism can return nonzero, at $018/SERDATR) was
// redirected to $032 too, so the control leg also read 0x0000 and no
// longer demonstrated a live nonzero read anywhere (MUT=MUT_B).
// MUTATION C-SERPER-FUNCTION-PERIOD-AND-CONTROL stimulus red: the period
// sub-check's SERPER write was skipped (SERPER stayed at its reset value,
// 0) while the check still expected the SERPER=10-derived cell count (22)
// -- measured cell=2 instead, period_ok false (MUT=MUT_C).
// MUTATION D-SERPER-RATIO-N-PLUS-ONE stimulus red: the N=10 trial wrote 9
// instead, while the check still expected the N=10-derived cell count (22)
// -- measured cell=20 (matching N=9's own (N+1)x2), mismatch against the
// N=10 oracle (MUT=MUT_D).
// MUTATION E-SERPER-TIMEBASE-IS-COLOUR-CLOCK stimulus red: clk7_en was
// allowed to pulse normally (ordinary tick()) during the window that
// should have held it permanently deasserted, so txd changed during that
// window as expected under the mutation, contradicting the "raw clk alone
// advances nothing" condition (MUT=MUT_E).
// MUTATION F-SERPER-BIT15-OUTSIDE-PERIOD stimulus red: the bit15-set trial
// used N=11 instead of N=10, so its cell (24) differs from the bit15-clear
// trial's cell (22) for a reason unrelated to bit 15 -- the "identical
// cell regardless of bit 15" check correctly went red (MUT=MUT_F).
// MUTATION G-SERPER-LONG-BIT15-CONTROL stimulus red: the LONG=1 trial
// never actually set bit 15 (collapsed to LONG=0 too), so both trials'
// elapsed-enables-to-rxint were identical (211 vs 211, delta 0 instead of
// 22) -- the intended contrast was collapsed (MUT=MUT_G).
// MUTATION H-SERPER-RATE-BITFIELD-POSITION stimulus red: the k=7 trial
// wrote bit 8 (0x0100) instead of bit 7 (0x0080), so its measured delta
// (512) was checked against bit 7's own oracle (256) and mismatched, while
// all fourteen other bits' deltas remained exactly correct (MUT=MUT_H).
//
// No oracle/expected value was changed by any of the eight mutations --
// only what the rig asked paula_uart to do (which address, which N, which
// bit, whether clk7_en was allowed to pulse). Every mutation was rebuilt
// with no flags afterward and reconfirmed against the clean 8-passed/
// 0-failed baseline before this log was written.
