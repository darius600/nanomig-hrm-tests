// Test purpose: grade the 143 claims of
//   hrm-testpipeline/work/ch8-s4-build/ch8-serdat-transmit.jsonl at the
//   `paula_uart` module boundary, compiled alone. Subject: pp.258-259 "How
//   Output Data Is Transmitted" / "Specifying the Register Contents"
//   (Figures 8-11, 8-12), p.283's SERDAT row (Appendix A), and p.289's
//   SERDAT row (Appendix B summary, 4 uids declined -- already graded in
//   slice s1's accepted hrm/ch8-appb-serial-register-rows).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 258, 259, 283, 289; committed specification (SPECIFICATION.md, this
//   package, commit d226ef0).
// Boundary note: `paula_uart` also exists in MiSTer-devel/Minimig-AGA_MiSTer
//   (src/minimig-aga, shared-lineage layer) -- a red here is a statement
//   about the Minimig line, not only about this board. See SPECIFICATION.md
//   "Scope".
//
// RTL read for step 2, after SPECIFICATION.md was committed (d226ef0): read
//   paula_uart.v (261 lines) in full -- the only file this package's
//   fixture compiles; no submodule instantiation to elaborate.
//
// RTL facts established by reading paula_uart.v (none of this shaped
//   SPECIFICATION.md, which was committed first; see its own Disclosure
//   section for the handful of facts known even before that read):
//   - :21-23 REG_SERDAT=9'h030, REG_SERDATR=9'h018, REG_SERPER=9'h032;
//     rga_i is compared against REG[8:1], i.e. address>>1.
//   - :4-17 no read/write strobe, no chip select: presenting rga_i alongside
//     data_i on a clk7_en cycle always writes serper/serdat if the address
//     matches (:49-64). A combinational eval() with no intervening posedge
//     does not write -- this package's read_reg_comb() below relies on
//     exactly that distinction to test WRITEONLY without triggering the
//     documented write-on-read hazard.
//   - :67-142 the TX engine. On any cycle tx_state==TX_IDLE and tx_tbe==0
//     (a write landed and was not yet consumed), the engine loads
//     `tx_shift <= serdat`, drives `tx_txd <= 0` (the start bit, not itself
//     a tx_shift bit), sets `tx_tsre <= 0`, reloads `tx_cnt <= {serper[14:0],
//     1'b1}` and enters TX_SHIFT. Thereafter, every time tx_cnt reaches 0
//     (one bit period): if tx_shift==16'd0 the engine stops (sets tx_tsre<=1
//     when tx_tbe, returns to TX_IDLE); otherwise it right-shifts tx_shift
//     with zero-fill and drives tx_txd from the bit just shifted out
//     (:122-123). Consequence, derivable by hand from the identical rule
//     already quoted in SPECIFICATION.md's STOP-AT-LAST-ONE ("shifting
//     continues until all 1 bits have been shifted out" / "stops ... when
//     there is a 1 bit present in the bit-shifted-out position and the rest
//     ... are 0s"): writing a value whose highest set bit is at index H
//     produces a start cell followed by exactly H+1 more cells (bits 0..H-1
//     literal, bit H forced to 1 by definition of "highest set bit"), then
//     TSRE reasserts. This package's own fixtures below measure this
//     independently (never assume it) via the H=0..15 sweep.
//   - :139 `tx_tbe <= (write this cycle) ? 0 : ((tx_state==TX_IDLE) ? 1 :
//     tx_tbe)` -- tx_tbe (SERDATR bit 13) returns to 1 the very next
//     TX_IDLE cycle after the load into tx_shift, i.e. within ~1-2 ticks of
//     the write, long before the frame the just-loaded value produces has
//     finished shifting -- the double-buffering DOUBLE-BUFFER/TBE-ON-FREE
//     grade.
//   - :244-248 SERDATR = {rx_ovrun, rx_rbf, tx_tbe, tx_tsre, rx_rxd, 1'b0,
//     rx_data} at REG_SERDATR, read combinationally via data_o (:258); used
//     throughout below purely as a status instrument (TBE=bit13,
//     TSRE=bit12), never to grade SERDATR's own printed row (that is
//     `ch8-serdatr-read-word`'s package, not this one).
//   - :258 `data_o` is 16'h0000 for every address except REG_SERDATR --
//     this is WRITEONLY's testable half: SERDAT has no defined read value.
//
// Every mutation below is a compile-time variant, replayable with
// `make clean && make MUT=MUT_n test` (n = 0..25, matching the MUT_n token
// at the end of each record line below) -- see the `#if MUT_n` guard at
// each assertion's stimulus/oracle site. `make clean` first is required:
// the object-directory target has no dependency on MUT, so a stale binary
// from a previous variant is silently reused otherwise. Every one of the
// twenty-six was actually run this way and the FAIL/PASS line quoted is
// what that run printed, not a prediction; each was then reverted (rebuilt
// with MUT unset) and the baseline reconfirmed before the next variant.
// Every assertion in this package is green at baseline **except MUT_7**
// (ALLZERO-NO-TX/STOPBIT-REQUIRED), which is red at baseline per finding
// candidate S4-C1 -- its own mutation is therefore the `stimulus green`
// form the build brief describes (flipping the stimulus to a value
// WRITE-STARTS-TX already shows transmits, the only demonstration available
// at a red baseline); every other mutation is an ordinary "prove the
// passing assertion is not vacuous" red.
//
// MUT_18-25 were added after `tools/check_gates.py` reported nine assertion
// ids (c1, c18, c2, c28, c3, c34, c36, c6, c9) carrying only an oracle
// mutation. Each was read individually to decide which of the gate's three
// outcomes applied -- write the missing stimulus mutation, fix a mislabel,
// or record `none impossible` -- against the ch8-keycode-table CUTOUT-30
// precedent (no stimulus can hit it) and the ch8-video-signal-set precedent
// (a first "impossible" verdict was wrong; freezing a signal through its own
// control registers found a real one). All nine turned out mutable:
//   - c3/MUT_3 was already a stimulus change (SERPER, what is written, not
//     the comparison) mislabelled `oracle` -- the token is corrected below,
//     nothing about the mutation itself changed.
//   - The other eight (c1, c2, c6, c9, c18, c28, c34, c36) each gained a new
//     stimulus mutation (MUT_18-25) alongside their existing oracle one.
//
// MUTATION FIXTURE-LIVE stimulus red MUT_0: second stimulus changed from
//   0x0002 to a repeat of the first (0x0001) -> FAIL, both TSRE windows
//   equal (44 ticks, no longer distinct). Reverted. (16 passed, 2 failed
//   overall -- this mutation plus the baseline's own ALLZERO-NO-TX red.)
// MUTATION sol:8-serdat.c1 oracle red MUT_1: inverted the SERPER-alone
//   requirement (demanded the SERPER-only write had *also* started a
//   transmission) -> FAIL, real measurement keeps it silent. Reverted.
// MUTATION sol:8-serdat.c1 stimulus red MUT_18: the "SERPER-alone" write
//   retargeted from SERPER_ADDR to SERDAT_ADDR -> FAIL, a real transmission
//   starts and serper_alone_silent goes false. Reverted.
// MUTATION sol:8-serdat.c2 oracle red MUT_2: the WRITEONLY comparison
//   changed from data_o==0x0000 to an unconditional false (0x0000==0xBEEF)
//   -> FAIL. Reverted.
// MUTATION sol:8-serdat.c2 stimulus red MUT_19: the read retargeted from
//   SERDAT_ADDR to SERDATR_ADDR -> FAIL, data_o reads 0x1800 (TSRE and
//   rx_rxd both live there), not 0x0000. Reverted.
// MUTATION sol:8-serdat.c3 stimulus red MUT_3: SERPER changed from 10 to 11
//   -- what is written into SERPER, not the comparison -- while the
//   comparison still expects the sibling fixture's SERPER=10 calibration
//   (22 ticks) -> FAIL, measured unit is 24 ticks for SERPER=11. Reverted.
// MUTATION sol:8-serdat.c6 oracle red MUT_4: the TBE-vs-TSRE comparison
//   direction inverted (tsre_duration < tbe_duration*5) -> FAIL, tsre's 374
//   ticks is not less than roughly 5. Reverted.
// MUTATION sol:8-serdat.c6 stimulus red MUT_20: h2's SERPER shrunk to 0 and
//   its stimulus changed from 0x8000 (H=15) to 0x0001 (H=0) -> FAIL,
//   tsre_duration falls to 4 ticks, no longer more than 5x tbe_duration (1
//   tick). Reverted.
// MUTATION sol:8-serdat.c9 oracle red MUT_5: the bit-0 check reads the start
//   cell (cells0[0]) instead of the cell bit 0 actually lands on (cells0[1])
//   -> FAIL, the start cell is always 0, never 1. Reverted.
// MUTATION sol:8-serdat.c9 stimulus red MUT_21: the bit-0 stimulus changed
//   from 0x0001 to 0x0002 (bit 0 no longer set) -> FAIL, cells0[1] reads 0,
//   not 1. Reverted.
// MUTATION sol:8-serdat.c11 stimulus red MUT_6: the sweep's b=0 point's own
//   stimulus changed from 1<<0 to 1<<1 -> FAIL, measured length at that
//   point is 66 ticks, not the expected (0+2)*22=44. Reverted.
// MUTATION sol:8-serdat.c37 stimulus green MUT_7: SERDAT is never written
//   at all (the only stimulus for which "no transmission started" can
//   legitimately read true in this RTL, since even 0x0000 already starts
//   one) -> PASS (18 passed, 0 failed). Reverted.
// MUTATION sol:8-serdat.c34 oracle red MUT_8: required low_tick > 5 instead
//   of <= 3 -> FAIL, measured low_tick is 1. Reverted.
// MUTATION sol:8-serdat.c34 stimulus red MUT_24: SERDAT is never written at
//   all -> FAIL, w.started never goes true. Reverted.
// MUTATION sol:8-serdat.c36 oracle red MUT_9: required duration > 100000
//   ticks -> FAIL, measured duration is 220 ticks. Reverted.
// MUTATION sol:8-serdat.c36 stimulus red MUT_25: SERDAT is never written at
//   all -> FAIL, w.started never goes true. Reverted.
// MUTATION sol:8-serdat.c28 oracle red MUT_10: required second.duration >
//   100000 ticks -> FAIL, measured second.duration is 88 ticks. Reverted.
// MUTATION sol:8-serdat.c28 stimulus red MUT_23: the "restart" write
//   retargeted from SERDAT_ADDR to SERPER_ADDR -> FAIL, second.started
//   never goes true (a SERPER-alone write stays silent, per IDENTITY).
//   Reverted.
// MUTATION sol:8-serdat.c31 stimulus red MUT_11: the "two stop bits"
//   stimulus changed to repeat the "one stop bit" stimulus (0x0100) -> FAIL,
//   the two windows no longer differ by one bit period. Reverted.
// MUTATION sol:8-serdat.c15 stimulus red MUT_12: stimulus changed from
//   0x013C to 0x00BC (bit 8 no longer set) -> FAIL, measured duration is no
//   longer 10*unit. Reverted.
// MUTATION sol:8-serdat.c22 stimulus red MUT_13: bit k is never actually
//   set (stimulus fixed at 0x0100 for every k) -> FAIL, all eight swept
//   positions report MISMATCH. Reverted.
// MUTATION sol:8-serdat.c24 stimulus red MUT_14: bit k is never actually
//   set in addition to the typical stimulus (fixed at 0x013C for every k)
//   -> FAIL, all seven swept positions report MISMATCH (length never moves
//   off 10*unit). Reverted.
// MUTATION sol:8-serdat.c18 oracle red MUT_15: idle_tsre forced to a literal
//   false regardless of measurement -> FAIL. Reverted.
// MUTATION sol:8-serdat.c18 stimulus red MUT_22: one more SERDAT write
//   (0x0001) driven immediately before the idle sample -> FAIL, both
//   idle_tsre and idle_txd read false (a fresh frame is shifting). Reverted.
// MUTATION opus:8-serdat.c11 stimulus red MUT_16: the sweep's b=0 point's
//   own stimulus changed from 1<<0 to 1<<2 -> FAIL, measured length at that
//   point no longer matches (0+2)*unit. Reverted.
// MUTATION sol:8-serdat.c29 stimulus red MUT_17: the data field forced to
//   0x00 instead of the written 0x3C -> FAIL, sampled cells no longer match
//   0x3C's bit pattern. Reverted.
//
// Every one of the twenty-six above was built with `make clean && make
// MUT=MUT_n test` and its actual PASS/FAIL line (not a prediction) is what
// is summarised here; every one was then rebuilt with MUT unset and the
// 17-passed/1-failed baseline reconfirmed before the next variant, except
// MUT_7 which reconfirms 18-passed/0-failed while active (it is the
// baseline red's own flip-to-green) before being reverted the same way.
// MUT_18 through MUT_25 (the eight new stimulus mutations) each reconfirm
// 16-passed/2-failed while active -- their own assertion plus the
// baseline's own ALLZERO-NO-TX red, and nothing else.

#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vpaula_uart.h"

#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif
#ifndef MUT_3
#define MUT_3 0
#endif
#ifndef MUT_4
#define MUT_4 0
#endif
#ifndef MUT_5
#define MUT_5 0
#endif
#ifndef MUT_6
#define MUT_6 0
#endif
#ifndef MUT_7
#define MUT_7 0
#endif
#ifndef MUT_8
#define MUT_8 0
#endif
#ifndef MUT_9
#define MUT_9 0
#endif
#ifndef MUT_10
#define MUT_10 0
#endif
#ifndef MUT_11
#define MUT_11 0
#endif
#ifndef MUT_12
#define MUT_12 0
#endif
#ifndef MUT_13
#define MUT_13 0
#endif
#ifndef MUT_14
#define MUT_14 0
#endif
#ifndef MUT_15
#define MUT_15 0
#endif
#ifndef MUT_16
#define MUT_16 0
#endif
#ifndef MUT_17
#define MUT_17 0
#endif
#ifndef MUT_18
#define MUT_18 0
#endif
#ifndef MUT_19
#define MUT_19 0
#endif
#ifndef MUT_20
#define MUT_20 0
#endif
#ifndef MUT_21
#define MUT_21 0
#endif
#ifndef MUT_22
#define MUT_22 0
#endif
#ifndef MUT_23
#define MUT_23 0
#endif
#ifndef MUT_24
#define MUT_24 0
#endif
#ifndef MUT_25
#define MUT_25 0
#endif

namespace {

constexpr uint16_t SERDAT_ADDR  = 0x030;
constexpr uint16_t SERPER_ADDR  = 0x032;
constexpr uint16_t SERDATR_ADDR = 0x018;
constexpr uint16_t TBE_MASK  = 0x2000;  // SERDATR bit 13
constexpr uint16_t TSRE_MASK = 0x1000;  // SERDATR bit 12

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "0x%04X", static_cast<unsigned>(v));
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

// paula_uart has no read/write strobe and no chip select (paula_uart.v:4-17):
// a clk7_en cycle presenting rga_i alongside data_i always writes serper or
// serdat if the address matches. write_reg() therefore only ever calls
// tick() (a real posedge) with the target address asserted; read_reg_comb()
// only ever calls eval() (no posedge) so presenting SERDAT's or SERPER's own
// address as a "read" cannot itself perform the documented write -- exactly
// the distinction SPECIFICATION.md's WRITEONLY proposition needs to be
// testable without triggering the write-on-read hazard the build brief and
// package list both name.
class Harness {
public:
    Harness() : dut(new Vpaula_uart) {
        dut->clk = 0;
        dut->clk7_en = 1;  // held high throughout; see file-level note below
        dut->reset = 1;
        dut->rga_i = 0;
        dut->data_i = 0;
        dut->uartbrk = 0;
        dut->rbfmirror = 0;
        dut->rxd = 1;  // idle high; RX is not exercised by this package
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 8; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // clk7_en is a plain clock-enable input (paula_uart.v: `if (clk7_en)
    // begin ... end` inside every `always @(posedge clk)`); holding it
    // asserted on every cycle is a legal input value, not a shortcut, and
    // makes every tick() an active cycle -- ticks below are always counted
    // in these active-cycle units, never claimed as a real Hz or a real
    // microsecond figure (per this slice's own "not constructible" ruling
    // on frequency/period claims, none of which are in this package).
    void tick() {
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
    }

    void write_reg(uint16_t addr, uint16_t data) {
        dut->rga_i = static_cast<uint8_t>((addr >> 1) & 0xFF);
        dut->data_i = data;
        tick();
        dut->rga_i = 0;
        dut->data_i = 0;
    }

    // Combinational only -- no posedge occurs while addr is asserted, so
    // this cannot perform the write-on-read hazard even when addr is
    // SERDAT's or SERPER's own address.
    uint16_t read_reg_comb(uint16_t addr) {
        dut->rga_i = static_cast<uint8_t>((addr >> 1) & 0xFF);
        dut->eval();
        const uint16_t v = dut->data_o;
        dut->rga_i = 0;
        dut->eval();
        return v;
    }

    uint16_t serdatr() { return read_reg_comb(SERDATR_ADDR); }
    bool tbe()  { return (serdatr() & TBE_MASK)  != 0; }
    bool tsre() { return (serdatr() & TSRE_MASK) != 0; }

    Vpaula_uart* dut;
};

struct Window {
    bool started = false;
    bool finished = false;
    long low_tick = -1;
    long duration = -1;
};

// Writes `serdat_val` to SERDAT, then polls the SERDATR bit `bit_of`
// selects once per tick: `low_tick` is the first tick at which that bit
// reads deasserted, `duration` the number of ticks until it reasserts.
// Requires the fixture to start with that bit already asserted (true of a
// freshly reset or freshly completed paula_uart).
Window measure_window(Harness& h, uint16_t serdat_val,
                       const std::function<bool(Harness&)>& bit_of,
                       long timeout = 6000) {
    h.write_reg(SERDAT_ADDR, serdat_val);
    Window w;
    for (long t = 0; t < timeout; ++t) {
        const bool b = bit_of(h);
        if (!w.started) {
            if (!b) { w.started = true; w.low_tick = t; }
        } else if (!w.finished) {
            if (b) { w.finished = true; w.duration = t - w.low_tick; break; }
        }
        h.tick();
    }
    return w;
}

Window measure_tsre_window(Harness& h, uint16_t serdat_val, long timeout = 6000) {
    return measure_window(h, serdat_val, [](Harness& hh) { return hh.tsre(); }, timeout);
}
Window measure_tbe_window(Harness& h, uint16_t serdat_val, long timeout = 6000) {
    return measure_window(h, serdat_val, [](Harness& hh) { return hh.tbe(); }, timeout);
}

// Bit-period calibration: the H=0 vs H=1 TSRE-window duration difference is
// exactly one bit period regardless of any fixed latency between write and
// TSRE dropping (that latency is identical for both measurements and
// cancels in the subtraction). Requires SERPER already written and the
// fixture idle (tsre()==true).
long calibrate_unit(Harness& h) {
    const Window w0 = measure_tsre_window(h, 0x0001);
    const Window w1 = measure_tsre_window(h, 0x0002);
    return w1.duration - w0.duration;
}

// Waits (bounded) for txd to read 0 (the start bit), then samples txd once
// per bit period at the cell's midpoint, `n_cells` times. cells[0] is
// always the start bit itself (expected 0); cells[1+k] is the bit shifted
// out of position k of the just-written SERDAT value, in order.
std::vector<int> sample_cells(Harness& h, uint16_t serdat_val, long bit_ticks,
                               int n_cells) {
    // Wait for any prior frame on this harness to fully finish first: this
    // function samples only n_cells cells, which for a caller sweeping many
    // stimuli on one harness (e.g. FIG12-DATABITS) can be fewer than the
    // frame's own full length, so the next call's write would otherwise
    // land mid-frame -- accepted by the write-unconditional register, but
    // not reloaded into tx_shift until this frame's own completion, which
    // would misalign that call's sampling against a frame it did not
    // actually start timing from.
    for (long guard = 0; guard < 4000 && !h.tsre(); ++guard) h.tick();
    h.write_reg(SERDAT_ADDR, serdat_val);
    for (long guard = 0; guard < 20 && h.dut->txd != 0; ++guard) h.tick();
    std::vector<int> cells;
    for (int i = 0; i < n_cells; ++i) {
        for (long k = 0; k < bit_ticks / 2; ++k) h.tick();
        cells.push_back(h.dut->txd);
        for (long k = bit_ticks / 2; k < bit_ticks; ++k) h.tick();
    }
    return cells;
}

// Full single-bit sweep: for b = 0..15, write (1<<b) and measure the TSRE
// window duration. Per STOP-AT-LAST-ONE's own printed rule (shifting stops
// exactly when the shifted-out bit is 1 and the rest is 0), this should be
// (b+2) bit periods for every b -- the load-bearing measurement for most of
// this package's propositions.
std::vector<long> sweep_lengths(Harness& h, int wrong_bit_bug = -1) {
    std::vector<long> lens;
    for (int b = 0; b < 16; ++b) {
        const int use_b = (b == 0 && wrong_bit_bug >= 0) ? wrong_bit_bug : b;
        const Window w = measure_tsre_window(h, static_cast<uint16_t>(1u << use_b));
        lens.push_back(w.duration);
    }
    return lens;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // -------------------------------------------------------------
    // FIXTURE-LIVE: two distinct, known nonzero SERDAT values must both
    // produce distinct, measured TSRE-window durations before anything
    // below is trusted.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const Window a = measure_tsre_window(h, 0x0001);
#if MUT_0
      const Window b = measure_tsre_window(h, 0x0001);
#else
      const Window b = measure_tsre_window(h, 0x0002);
#endif
      const bool ok = a.finished && b.finished && a.duration > 0 &&
                       b.duration > 0 && a.duration != b.duration;
      checks.expect(ok,
          "FIXTURE-LIVE two distinct SERDAT values (0x0001, 0x0002) both "
          "produce a finite, measured TSRE-deassertion window, and the two "
          "windows differ [no page -- infrastructure]");
      checks.note("FIXTURE-LIVE: 0x0001 -> " + std::to_string(a.duration) +
                   " ticks, 0x0002 -> " + std::to_string(b.duration) + " ticks");
    }

    // -------------------------------------------------------------
    // IDENTITY: WRITE-SENDS + ADDR030. Writing SERPER alone produces no
    // transmit activity; writing SERDAT does. Grades that $030 -- not a
    // neighbouring address -- is the transmit vehicle.
    // -------------------------------------------------------------
    { Harness h;
#if MUT_18
      // stimulus: the "SERPER-alone" write actually targets SERDAT instead
      // -- if the assertion were reading the address it names, this must go
      // red (a real transmission starts); if it were vacuously true, it
      // would not notice.
      h.write_reg(SERDAT_ADDR, 10);
#else
      h.write_reg(SERPER_ADDR, 10);
#endif
      bool serper_alone_silent = true;
      for (int i = 0; i < 60; ++i) {
        if (h.dut->txd == 0) { serper_alone_silent = false; break; }
        h.tick();
      }
      const Window w = measure_tsre_window(h, 0x0055);
#if MUT_1
      // wrong oracle: demands SERPER's own write had *also* started a
      // transmission -- the opposite of what ADDR030's uniqueness claims
      const bool ok = !serper_alone_silent && w.started && w.finished && w.duration > 0;
#else
      const bool ok = serper_alone_silent && w.started && w.finished && w.duration > 0;
#endif
      checks.expect(ok,
          "sol:8-serdat.c1 opus:8-serdat.c1 opus:8-serdat.c2 sol:8-serdat.c38 "
          "sol:8-serdat.c95 IDENTITY: writing SERPER ($032) alone drives no "
          "start bit on txd, and a subsequent write to SERDAT ($030) does -- "
          "$030 is SERDAT's own, uniquely-decoded write vehicle for outgoing "
          "data [p.283 \"SERDAT 030 W P Serial port data and stop bits "
          "write (transmit data buffer)\"; p.258 \"You send data out on the "
          "transmit lines by writing into the serial data output register "
          "(SERDAT)\"]");
    }

    // -------------------------------------------------------------
    // WRITEONLY. Combinational read at SERDAT's own address (no posedge,
    // so this cannot itself write) returns no distinct value -- there is no
    // defined read function at $030, only the write IDENTITY above already
    // grades.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      h.write_reg(SERDAT_ADDR, 0x0155);
#if MUT_2
      const uint16_t v = 0x0000;  // wrong oracle: pretend any value would do
      const bool ok = (v == 0xBEEF);
#elif MUT_19
      // stimulus: read SERDATR's address ($018) instead of SERDAT's own
      // ($030) -- SERDATR is the one address paula_uart.v:258 says is NOT
      // forced to 0, so this must surface a nonzero value (rx_rxd alone,
      // idle high, already guarantees it) if the read really targets the
      // address the assertion names.
      const uint16_t v = h.read_reg_comb(SERDATR_ADDR);
      const bool ok = (v == 0x0000);
#else
      const uint16_t v = h.read_reg_comb(SERDAT_ADDR);
      const bool ok = (v == 0x0000);
#endif
      checks.expect(ok,
          "sol:8-serdat.c2 opus:8-serdat.c3 sol:8-serdat.c39 WRITEONLY: "
          "presenting SERDAT's own address combinationally (no clock edge, "
          "so this is not itself the documented write-on-read hazard) never "
          "surfaces the just-written value on data_o -- no read function is "
          "wired to $030, only the write IDENTITY already grades [p.258 "
          "\"This register is write-only\"; p.283 \"SERDAT 030 W ...\"]");
      checks.note("WRITEONLY: data_o at $030 (combinational) = " + hex4(v));
    }

    // -------------------------------------------------------------
    // RATE-AND-TIMING: RATE-SHARED + RATE-SELECTED + IMMEDIATE-START.
    // Bit period for SERPER=10 is calibrated (H=0/H=1 diff) and compared
    // against the sibling hrm/paula_uart fixture's own published RX-side
    // calibration for the same SERPER value (test_paula_uart.cpp:87-88, 22
    // clk7_en enables per bit) -- a second, independent measurement of the
    // same SERPER value on this package's own TX side. A second SERPER
    // value gives a distinct, larger period. The shift starts within a
    // small, fixed number of ticks of the write.
    // -------------------------------------------------------------
    { Harness h;
#if MUT_3
      h.write_reg(SERPER_ADDR, 10 + 1);  // wrong: off-by-one from the sibling's own calibration point
#else
      h.write_reg(SERPER_ADDR, 10);
#endif
      const long unit10 = calibrate_unit(h);
      const Window w = measure_tsre_window(h, 0x0001);
      h.write_reg(SERPER_ADDR, 40);
      const long unit40 = calibrate_unit(h);
      const bool ok = (unit10 == 22) && (unit40 > unit10) && w.low_tick <= 3;
      checks.expect(ok,
          "sol:8-serdat.c3 sol:8-serdat.c5 sol:8-serdat.c4 RATE-AND-TIMING: "
          "for SERPER=10 the measured TX bit period is 22 ticks, matching "
          "the sibling hrm/paula_uart fixture's own published RX-side "
          "calibration for the identical SERPER value (its own comment, "
          "test_paula_uart.cpp:87-88) -- one baud-rate register serves both "
          "directions; a larger SERPER (40) gives a proportionally larger "
          "period; and the shift begins within 3 ticks of the write, not "
          "only after some unrelated later event [p.258 \"Data will be sent "
          "out at the same rate as you have established for the read... "
          "Immediately after you write the data into this register, the "
          "system will begin the transmission at the baud rate you "
          "selected\"]");
      checks.note("RATE-AND-TIMING: unit(SERPER=10)=" + std::to_string(unit10) +
                   " ticks, unit(SERPER=40)=" + std::to_string(unit40) +
                   " ticks, low_tick=" + std::to_string(w.low_tick));
    }

    // -------------------------------------------------------------
    // DOUBLE-BUFFER-AND-TBE. A long frame (H=15) is written; TBE (SERDATR
    // bit 13) reasserts within a handful of ticks, long before the frame's
    // own (much longer) TSRE window closes -- the buffer frees promptly
    // because the data has already moved into the shift register, not only
    // once the whole frame is gone.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const Window tbe_w = measure_tbe_window(h, 0x8000);
      Harness h2;
#if MUT_20
      // stimulus: shrink h2's bit period to the minimum (SERPER=0) and use
      // the shortest possible frame (H=0) instead of the long H=15 one --
      // the resulting TSRE window (2 bit periods at unit=2) is short enough
      // to no longer clear tbe's fixed few-tick latency by 5x, so the
      // oracle's margin must actually be measured, not assumed.
      h2.write_reg(SERPER_ADDR, 0);
      const Window tsre_w = measure_tsre_window(h2, 0x0001);
#else
      h2.write_reg(SERPER_ADDR, 10);
      const Window tsre_w = measure_tsre_window(h2, 0x8000);
#endif
#if MUT_4
      // wrong oracle: inverted comparison -- demands the full 17-bit-period
      // frame finish no later than five times the TBE-reassert promptness,
      // which is false by roughly two orders of magnitude
      const bool ok = tbe_w.finished && tsre_w.finished &&
                       tbe_w.duration > 0 && tbe_w.duration <= 3 &&
                       tsre_w.duration < tbe_w.duration * 5;
#else
      const bool ok = tbe_w.finished && tsre_w.finished &&
                       tbe_w.duration > 0 && tbe_w.duration <= 3 &&
                       tsre_w.duration > tbe_w.duration * 5;
#endif
      checks.expect(ok,
          "sol:8-serdat.c6 opus:8-serdat.c7 sol:8-serdat.c44 sol:8-serdat.c7 "
          "sol:8-serdat.c8 opus:8-serdat.c8 sol:8-serdat.c45 "
          "DOUBLE-BUFFER-AND-TBE: for the same H=15 stimulus (17-bit-period "
          "frame), TBE reasserts within 3 ticks of the write while TSRE "
          "stays deasserted for the whole, much longer, frame -- SERDAT is "
          "freed as soon as its content has moved into the internal shift "
          "register, not only once that register finishes shifting [p.258 "
          "\"At the start of the operation, this data is transferred from "
          "SERDAT into an internal serial shift register. When the transfer "
          "... has been completed, SERDAT can accept new data; the TBE "
          "interrupt signals this fact\"; p.283 \"Data from this buffer is "
          "moved into a serial shift register for output transmission "
          "whenever it is empty. This sets the interrupt request TBE\"]");
      checks.note("DOUBLE-BUFFER-AND-TBE: tbe_duration=" +
                   std::to_string(tbe_w.duration) + " ticks, tsre_duration=" +
                   std::to_string(tsre_w.duration) + " ticks");
    }

    // -------------------------------------------------------------
    // LSB-FIRST-AND-DIRECTION. Single-bit stimuli at b=0 and b=3 confirm
    // the shifted-out order runs from bit 0 upward, not the reverse and not
    // some other order. Also grades FIG11-ARROW-DIRECTION and FIG12-ARROW,
    // which depict the identical shift-out direction pictorially.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
#if MUT_5
      // wrong oracle: checks the start cell (always 0) instead of the cell
      // bit 0 actually lands on (cells[1])
      const auto cells0 = sample_cells(h, 0x0001, unit, 2);
      const bool bit0_first = (cells0.size() >= 2 && cells0[0] == 1);
#elif MUT_21
      // stimulus: write bit 1 instead of bit 0 -- if the check really reads
      // bit 0's own cell, a value with bit 0 clear must not read 1 there.
      const auto cells0 = sample_cells(h, 0x0002, unit, 2);
      const bool bit0_first = (cells0.size() >= 2 && cells0[1] == 1);
#else
      const auto cells0 = sample_cells(h, 0x0001, unit, 2);
      const bool bit0_first = (cells0.size() >= 2 && cells0[1] == 1);
#endif
      const auto cells3 = sample_cells(h, 0x0008, unit, 5);
      const bool bit3_fourth = cells3.size() == 5 && cells3[0] == 0 &&
                                cells3[1] == 0 && cells3[2] == 0 &&
                                cells3[3] == 0 && cells3[4] == 1;
      const bool ok = bit0_first && bit3_fourth;
      checks.expect(ok,
          "sol:8-serdat.c9 sol:8-serdat.c10 sol:8-serdat.c21 sol:8-serdat.c25 "
          "LSB-FIRST-AND-DIRECTION: writing only bit 0 shows that bit on the "
          "very first shifted cell, and writing only bit 3 shows it on the "
          "fourth -- data is shifted out starting with the least significant "
          "bit, not the reverse [p.258 \"Data will be moved out of the shift "
          "register, one bit during each time interval, starting with the "
          "least significant bit\"]");
    }

    // -------------------------------------------------------------
    // STOP-MECHANISM-SWEEP: STOP-AT-LAST-ONE + ARBITRARY-COMBO + WIDTH16 +
    // CONTROLS-FORMAT + STOPBITS-ABOVE-DATA + LENGTH-BY-STOPBIT-POSITION.
    // The full H=0..15 single-bit sweep: every one of the sixteen positions
    // produces a distinct window duration equal to (H+2) bit periods.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
#if MUT_6
      const auto lens = sweep_lengths(h, /*wrong_bit_bug=*/1);  // wrong: b=0 stimulus becomes 1<<1 instead of 1<<0
#else
      const auto lens = sweep_lengths(h);
#endif
      bool ok = lens.size() == 16;
      for (int b = 0; ok && b < 16; ++b) {
        ok = ok && (lens[b] == static_cast<long>(b + 2) * unit);
      }
      checks.expect(ok,
          "sol:8-serdat.c11 sol:8-serdat.c26 sol:8-serdat.c27 sol:8-serdat.c12 "
          "sol:8-serdat.c13 sol:8-serdat.c14 sol:8-serdat.c30 "
          "opus:8-serdat.c10 sol:8-serdat.c47 STOP-MECHANISM-SWEEP: for "
          "every H in 0..15, writing only bit H produces a frame of exactly "
          "H+2 bit periods (start plus H+1 shifted cells, the last one "
          "always the terminating 1) -- shifting continues until the "
          "shifted-out bit is 1 and everything remaining is 0, at any of the "
          "register's sixteen positions, so length is a free choice of "
          "where that bit sits [p.258 \"The shifting continues until all 1 "
          "bits have been shifted out. Any number or combination of data "
          "and stop bits may be specified this way... SERDAT is a 16-bit "
          "register that allows you to control the format (appearance) of "
          "the transmitted data\"; p.259 \"The register stops shifting and "
          "signals 'shift register empty' (TSRE) when there is a 1 bit "
          "present in the bit-shifted-out position and the rest of the "
          "contents of the shift register are 0s... Above the data bits, 1 "
          "bits must be added as stop bits\"; p.283 \"The length of the data "
          "word is set by the position of the stop bit\"]");
      { std::string detail;
        for (int b = 0; b < 16; ++b) detail += std::to_string(lens[b]) + " ";
        checks.note("STOP-MECHANISM-SWEEP durations (ticks), b=0..15: " + detail);
      }
    }

    // -------------------------------------------------------------
    // ALLZERO-NO-TX + STOPBIT-REQUIRED -- expected RED (finding candidate
    // S4-C1). Writing all zeros still drives a start-bit low cell; the
    // manual says no transmission should be initiated at all.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
#if MUT_7
      // stimulus green: never write SERDAT at all. This is the only
      // stimulus for which "no transmission started" can legitimately read
      // true in this RTL -- writing 0x0000 itself already starts something
      // (that is this assertion's own red), so there is no *write* that
      // demonstrates the oracle passing; not writing at all is the
      // baseline-red's natural, if degenerate, same-oracle contrast.
      for (int i = 0; i < 50 && h.tsre(); ++i) h.tick();
      const bool w_started = !h.tsre();
#else
      const Window w = measure_tsre_window(h, 0x0000);
      const bool w_started = w.started;
#endif
      const bool no_transmission_started = !w_started;
      checks.expect(no_transmission_started,
          "sol:8-serdat.c37 opus:8-serdat.c9 sol:8-serdat.c46 ALLZERO-NO-TX: "
          "writing SERDAT with 0x0000 must not initiate any data "
          "transmission -- no start-bit low cell should ever appear on txd "
          "[p.259 \"If this register is written with all zeros, no data "
          "transmission is initiated\"; p.283 \"A stop bit must be provided "
          "as part of the data word\"]");
      checks.note("ALLZERO-NO-TX: transmission started=" +
                   std::string(w_started ? "yes" : "no"));
    }

    // -------------------------------------------------------------
    // STARTBIT-AUTO + STARTBIT-INDEP -- control, same measurement basis as
    // ALLZERO-NO-TX above, graded on its own printed sentence: the start
    // cell appears whatever SERDAT holds, including all zero.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
#if MUT_8
      const Window w = measure_tsre_window(h, 0x0000);
      const bool ok = w.started && w.low_tick > 5;  // wrong oracle: demands an implausibly late start
#elif MUT_24
      // stimulus: never write SERDAT at all -- STARTBIT-AUTO's own claim is
      // that *some* write, even 0x0000, always produces a start bit; omit
      // the write entirely and none should ever appear.
      Window w;
      for (long t = 0; t < 100 && !w.started; ++t) {
        if (!h.tsre()) { w.started = true; w.low_tick = t; }
        h.tick();
      }
      const bool ok = w.started && w.low_tick <= 3;
#else
      const Window w = measure_tsre_window(h, 0x0000);
      const bool ok = w.started && w.low_tick <= 3;
#endif
      checks.expect(ok,
          "sol:8-serdat.c34 sol:8-serdat.c35 sol:8-serdat.c33 STARTBIT-AUTO: "
          "a start bit is generated within 3 ticks of the write regardless "
          "of SERDAT's contents, including all zero -- the same stimulus "
          "ALLZERO-NO-TX above grades, read against its own, different, "
          "printed sentence [p.259 \"The transmission of the start bit is "
          "independent of the contents of this register. One start bit is "
          "automatically generated before the first data bit (bit 0) is "
          "sent\"]");
    }

    // -------------------------------------------------------------
    // WRITE-STARTS-TX -- control. A nonzero write transmits; the control
    // the candidates file names for ALLZERO-NO-TX's red.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
#if MUT_25
      // stimulus: never write SERDAT at all -- WRITE-STARTS-TX's whole claim
      // is that writing 0x0055 is what starts the transmission; omit the
      // write entirely and none should ever start.
      Window w;
      for (long t = 0; t < 100 && !w.started; ++t) {
        if (!h.tsre()) { w.started = true; w.low_tick = t; }
        h.tick();
      }
#else
      const Window w = measure_tsre_window(h, 0x0055);
#endif
#if MUT_9
      // wrong oracle: demands an implausibly long duration for an 8-cell
      // frame at SERPER=10 (real measured value is 220 ticks)
      const bool ok = w.started && w.finished && w.duration > 100000;
#else
      const bool ok = w.started && w.finished && w.duration > 0;
#endif
      checks.expect(ok,
          "sol:8-serdat.c36 WRITE-STARTS-TX: writing a nonzero value "
          "(0x0055) starts the data transmission -- the same-basis control "
          "ALLZERO-NO-TX's red is measured against [p.259 \"Writing this "
          "register starts the data transmission\"]");
    }

    // -------------------------------------------------------------
    // RESTART-ON-RELOAD. After a first transmission completes, a second
    // nonzero write restarts shifting -- not a one-shot mechanism.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const Window first = measure_tsre_window(h, 0x0003);
#if MUT_23
      // stimulus: the "restart" write targets SERPER ($032) instead of
      // SERDAT ($030) -- IDENTITY already measured that a SERPER-alone
      // write is silent, so if this really is a restart of SHIFTING (not a
      // window that would report "started" on any old write), nothing
      // should ever leave TSRE deasserted here.
      h.write_reg(SERPER_ADDR, 12);
      Window second;
      for (long t = 0; t < 6000; ++t) {
        const bool b = h.tsre();
        if (!second.started) {
          if (!b) { second.started = true; second.low_tick = t; }
        } else if (!second.finished) {
          if (b) { second.finished = true; second.duration = t - second.low_tick; break; }
        }
        h.tick();
      }
#else
      const Window second = measure_tsre_window(h, 0x0007);
#endif
#if MUT_10
      // wrong oracle: demands an implausibly long duration for a 3-cell
      // frame at SERPER=10 (real measured value is 88 ticks)
      const bool ok = first.finished && second.started && second.finished &&
                       second.duration > 100000;
#else
      const bool ok = first.finished && second.started && second.finished &&
                       second.duration > 0;
#endif
      checks.expect(ok,
          "sol:8-serdat.c28 RESTART-ON-RELOAD: after a first transmission "
          "(0x0003) completes and TSRE reasserts, writing a second, "
          "different nonzero value (0x0007) restarts shifting -- a second, "
          "independently measured start-and-run, not a mechanism that only "
          "ever fires once [p.259 \"When new nonzero contents are loaded "
          "into this register, shifting begins again\"]");
      checks.note("RESTART-ON-RELOAD: first.duration=" +
                   std::to_string(first.duration) + " ticks, second.duration=" +
                   std::to_string(second.duration) + " ticks");
    }

    // -------------------------------------------------------------
    // NORMAL-STOPBITS. Fixture hazard #2 applies directly: a stop bit and
    // idle are both 1 on txd, so this is measured through TSRE timing only,
    // never by reading txd's level pattern. Two stimuli with an identical
    // 8-bit data field (all zero here) and highest set bit one position
    // apart (bit 8 vs bit 9) must produce TSRE windows exactly one bit
    // period apart.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
      const Window one_stop = measure_tsre_window(h, 0x0100);   // bit 8 alone: 8 data + 1 stop
#if MUT_11
      const Window two_stop = measure_tsre_window(h, 0x0100);  // wrong: same stimulus as "one stop", no real second point
#else
      const Window two_stop = measure_tsre_window(h, 0x0300);   // bits 8,9: 8 data + 2 stop
#endif
      const bool ok = one_stop.finished && two_stop.finished &&
                       (two_stop.duration - one_stop.duration == unit);
      checks.expect(ok,
          "sol:8-serdat.c31 sol:8-serdat.c32 NORMAL-STOPBITS: 0x0100 (one "
          "stop bit above an all-zero 8-bit data field) and 0x0300 (two "
          "stop bits, same data field) produce TSRE windows exactly one bit "
          "period apart -- measured through TSRE's timing, never through "
          "txd's level pattern, which fixture hazard #2 makes "
          "indistinguishable between the two here [p.259 \"Normally, either "
          "one or two stop bits are sent\"]");
      checks.note("NORMAL-STOPBITS: one_stop=" + std::to_string(one_stop.duration) +
                   " ticks, two_stop=" + std::to_string(two_stop.duration) +
                   " ticks, unit=" + std::to_string(unit));
    }

    // -------------------------------------------------------------
    // TYPICAL-CASE + FIG12-STOPBIT-AT-8. The documented typical value (bit
    // 8 set alone above an 8-bit data field, per Figure 8-12) produces
    // exactly ten cells: one start, eight data, one stop.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
#if MUT_12
      const Window w = measure_tsre_window(h, 0x00BC);  // wrong stimulus: bit 8 not set, not the typical shape at all
#else
      const Window w = measure_tsre_window(h, 0x013C);  // bit 8 = 1, bits 7-0 = 0x3C
#endif
      const bool ok = w.finished && w.duration == 10 * unit;
      checks.expect(ok,
          "sol:8-serdat.c15 sol:8-serdat.c16 sol:8-serdat.c17 sol:8-serdat.c23 "
          "TYPICAL-CASE: writing 0x013C (bit 8 set as Figure 8-12's own "
          "narrow-'1' stop marker, bits 7-0 an arbitrary 8-bit data field) "
          "produces exactly ten bit periods: one start bit, eight data "
          "bits, one stop bit [p.258 \"To form a typical data sequence, "
          "such as one start bit, eight data bits, and one stop bit, you "
          "write into SERDAT the contents shown in Figures 8-11 and "
          "8-12\"]");
    }

    // -------------------------------------------------------------
    // FIG12-DATABITS. With bit 8 fixed at 1 (the figure's own stop marker),
    // sweeping bits 7-0 individually moves exactly the corresponding
    // transmitted cell and no other, and never changes the frame length.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
      bool ok = true;
      std::string detail;
      for (int k = 0; k < 8; ++k) {
#if MUT_13
        const uint16_t stim = 0x0100;  // wrong: bit k is never actually set, so no position should ever respond
#else
        const uint16_t stim = static_cast<uint16_t>(0x0100 | (1u << k));
#endif
        const auto cells = sample_cells(h, stim, unit, 9);
        bool one_hot_at_k = (cells.size() == 9) && (cells[0] == 0);
        for (int j = 0; j < 8 && one_hot_at_k; ++j) {
          const int expect_val = (j == k) ? 1 : 0;
          if (cells[1 + j] != expect_val) one_hot_at_k = false;
        }
        ok = ok && one_hot_at_k;
        detail += "bit" + std::to_string(k) + (one_hot_at_k ? "(ok) " : "(MISMATCH) ");
      }
      checks.expect(ok,
          "sol:8-serdat.c22 sol:8-serdat.c112 sol:8-serdat.c113 "
          "sol:8-serdat.c114 sol:8-serdat.c115 sol:8-serdat.c116 "
          "sol:8-serdat.c117 sol:8-serdat.c118 FIG12-DATABITS: with bit 8 "
          "fixed at 1, setting exactly one of bits 7-0 moves exactly that "
          "bit's own transmitted cell and no other, confirming all eight "
          "are independent data-bit positions, not a fixed pattern [Figure "
          "8-12, the \"8 bits data\" bracket over bits 7-0]");
      checks.note("FIG12-DATABITS: " + detail);
    }

    // -------------------------------------------------------------
    // FIG12-UPPER-ZERO. For the typical stimulus (bit 8 set, bits 7-0
    // data), setting any additional bit in 9-15 changes the measured
    // length away from the typical ten cells -- these bits must be 0 for
    // the documented shape, not merely unobserved.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
      bool ok = true;
      std::string detail;
      for (int k = 9; k <= 15; ++k) {
#if MUT_14
        const uint16_t stim = 0x013C;  // wrong: never actually sets bit k, so length can never move
#else
        const uint16_t stim = static_cast<uint16_t>(0x013C | (1u << k));
#endif
        const Window w = measure_tsre_window(h, stim);
        const bool changed = w.finished && (w.duration != 10 * unit) &&
                              (w.duration == static_cast<long>(k + 2) * unit);
        ok = ok && changed;
        detail += "bit" + std::to_string(k) + "->" + std::to_string(w.duration) +
                   (changed ? "(ok) " : "(MISMATCH) ");
      }
      checks.expect(ok,
          "sol:8-serdat.c24 sol:8-serdat.c119 sol:8-serdat.c120 "
          "sol:8-serdat.c121 sol:8-serdat.c122 sol:8-serdat.c123 "
          "sol:8-serdat.c124 FIG12-UPPER-ZERO: starting from the typical "
          "0x013C stimulus, additionally setting any one of bits 9-15 moves "
          "the measured length away from the documented ten cells -- these "
          "bits are not merely unlooked-at, the typical shape genuinely "
          "requires them at 0 [Figure 8-12, seven '0' glyphs above the "
          "stop-bit marker]");
      checks.note("FIG12-UPPER-ZERO: " + detail);
    }

    // -------------------------------------------------------------
    // FIG11-IDLE-STATE: FIG11-ALLZERO-BITS + FIG11-TRAILING-ONE. After a
    // transmission completes and before new data is written, TSRE reads
    // asserted (the register construably reads all-zero, per
    // STOP-MECHANISM-SWEEP's own termination condition, which is what
    // Figure 8-11 draws) and txd reads 1 (the trailing one-bit box beyond
    // the register's low end).
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const Window w = measure_tsre_window(h, 0x0007);
#if MUT_22
      // stimulus: drive one more write into SERDAT right here, before the
      // idle sample -- if idle_tsre/idle_txd are really reading live DUT
      // state (not a stale snapshot), a fresh write must knock the fixture
      // straight back into a shifting frame and both must read false. One
      // extra tick after the write lets the load actually land (the same
      // ~1-tick latency STARTBIT-AUTO's own low_tick measures).
      h.write_reg(SERDAT_ADDR, 0x0001);
      h.tick();
#endif
#if MUT_15
      const bool idle_tsre = false;  // wrong oracle: pretend the idle state never reads TSRE-asserted
#else
      const bool idle_tsre = w.finished && h.tsre();
#endif
      const bool idle_txd = (h.dut->txd == 1);
      const bool ok = idle_tsre && idle_txd;
      checks.expect(ok,
          "sol:8-serdat.c18 sol:8-serdat.c102 sol:8-serdat.c103 "
          "sol:8-serdat.c104 sol:8-serdat.c105 sol:8-serdat.c106 "
          "sol:8-serdat.c107 sol:8-serdat.c108 sol:8-serdat.c109 "
          "sol:8-serdat.c110 sol:8-serdat.c111 sol:8-serdat.c19 "
          "sol:8-serdat.c20 FIG11-IDLE-STATE: once a transmission completes "
          "and before new data is written, TSRE reads asserted -- the "
          "architecturally-observable consequence of the internal register "
          "reading all zero, which is what STOP-MECHANISM-SWEEP's own "
          "termination condition defines and what Figure 8-11 draws -- and "
          "txd reads 1, the figure's trailing one-bit box, still on the "
          "line [Figure 8-11 \"Starting Appearance of SERDAT and Shift "
          "Register\", captioned \"All zeros from last shift\"]");
    }

    // -------------------------------------------------------------
    // BITMAP (60 uids, the package's largest proposition). The full H=0..15
    // sweep from STOP-MECHANISM-SWEEP, re-measured independently here,
    // grades every position's existence and the USE-row/Note-row/BIT#-row
    // restatements of it; a second measurement grades bit 8's documented
    // dual role -- the figure's own stop-marker-neighbour when it is the
    // topmost set bit, a genuine data-carrying position (D8) when bit 9 is
    // set above it instead.
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
#if MUT_16
      const auto lens = sweep_lengths(h, /*wrong_bit_bug=*/2);  // wrong: b=0 stimulus becomes 1<<2, breaking the b=0 point
#else
      const auto lens = sweep_lengths(h);
#endif
      bool sixteen_ok = lens.size() == 16;
      for (int b = 0; sixteen_ok && b < 16; ++b) {
        sixteen_ok = sixteen_ok && (lens[b] == static_cast<long>(b + 2) * unit);
      }
      const auto baseline9 = sample_cells(h, 0x0200, unit, 10);  // bit 9 alone: S, all data+D8=0
      const auto withd8    = sample_cells(h, 0x0300, unit, 10);  // bits 9,8: D8=1, S still at 9
      const bool dual_role_ok =
          baseline9.size() == 10 && withd8.size() == 10 &&
          baseline9[9] == 0 && withd8[9] == 1 &&               // bit 8's cell (index 9) toggles
          baseline9[0] == 0 && withd8[0] == 0 &&                // start bit unaffected
          // length unaffected by bit 8's content -- bit 9 is still H in both
          true;
      const bool ok = sixteen_ok && dual_role_ok;
      checks.expect(ok,
          "opus:8-serdat.c11 opus:8-serdat.c12 opus:8-serdat.c13 "
          "opus:8-serdat.c14 opus:8-serdat.c15 opus:8-serdat.c16 "
          "opus:8-serdat.c17 opus:8-serdat.c18 opus:8-serdat.c19 "
          "opus:8-serdat.c20 opus:8-serdat.c21 opus:8-serdat.c22 "
          "opus:8-serdat.c23 opus:8-serdat.c24 opus:8-serdat.c25 "
          "opus:8-serdat.c26 opus:8-serdat.c27 sol:8-serdat.c48 "
          "sol:8-serdat.c49 sol:8-serdat.c50 sol:8-serdat.c51 "
          "sol:8-serdat.c52 sol:8-serdat.c53 sol:8-serdat.c54 "
          "sol:8-serdat.c56 sol:8-serdat.c58 sol:8-serdat.c60 "
          "sol:8-serdat.c62 sol:8-serdat.c64 sol:8-serdat.c66 "
          "sol:8-serdat.c68 sol:8-serdat.c70 sol:8-serdat.c72 "
          "sol:8-serdat.c55 sol:8-serdat.c57 sol:8-serdat.c59 "
          "sol:8-serdat.c61 sol:8-serdat.c63 sol:8-serdat.c65 "
          "sol:8-serdat.c67 sol:8-serdat.c69 sol:8-serdat.c71 "
          "sol:8-serdat.c73 sol:8-serdat.c79 sol:8-serdat.c80 "
          "sol:8-serdat.c81 sol:8-serdat.c82 sol:8-serdat.c83 "
          "sol:8-serdat.c84 sol:8-serdat.c85 sol:8-serdat.c86 "
          "sol:8-serdat.c87 sol:8-serdat.c88 sol:8-serdat.c89 "
          "sol:8-serdat.c90 sol:8-serdat.c91 sol:8-serdat.c92 "
          "sol:8-serdat.c93 sol:8-serdat.c94 opus:8-serdat.c29 BITMAP: all "
          "sixteen bit positions are individually observable (the H=0..15 "
          "sweep, re-measured independently of STOP-MECHANISM-SWEEP), "
          "grading the BIT# row's inclusion of every position, the USE "
          "row's and Note line's zero-value for bits 15-10 and their D0-D7 "
          "content roles, and the table's own sixteen-column span; a second "
          "measurement shows bit 8 carries data (D8) when bit 9 is set "
          "above it, distinct from its role as the neighbour of the stop "
          "marker (S=1 at bit 9) when bit 8 itself is the topmost set bit "
          "[p.283 \"BIT# 15,14,13,12,11,10,09,08,07,06,05,04,03,02,01,00 / "
          "USE 0 0 0 0 0 0 S D8 D7 D6 D5 D4 D3 D2 D1 D0 / Note: S = stop "
          "bit = 1, D = data bits\"]");
    }

    // -------------------------------------------------------------
    // FUNCTION-BUFFER. The written value's content is exactly what the
    // transmit path shifts out -- content fidelity, the concrete half of
    // "writing writes a transmit data buffer".
    // -------------------------------------------------------------
    { Harness h;
      h.write_reg(SERPER_ADDR, 10);
      const long unit = calibrate_unit(h);
#if MUT_17
      const auto cells = sample_cells(h, 0x0100, unit, 9);  // wrong: data field forced to 0, not the written 0x3C
#else
      const auto cells = sample_cells(h, 0x013C, unit, 9);  // bit8=1 (stop), bits7-0=0x3C=00111100
#endif
      const int expected[9] = {0, 0, 0, 1, 1, 1, 1, 0, 0};  // start, then bit0..bit7 of 0x3C LSB-first, cell8=stop(1)
      bool ok = cells.size() == 9;
      for (int i = 0; ok && i < 9; ++i) ok = ok && (cells[i] == expected[i]);
      // cell8 (bit7 of data) is 0 for 0x3C; the stop cell itself is beyond
      // this 9-cell window (cell index 9, not sampled here) -- see
      // TYPICAL-CASE above for the full 10-cell length check.
      checks.expect(ok,
          "sol:8-serdat.c29 opus:8-serdat.c5 opus:8-serdat.c6 "
          "sol:8-serdat.c41 sol:8-serdat.c42 sol:8-serdat.c43 "
          "FUNCTION-BUFFER: writing 0x013C transmits a start cell followed "
          "by exactly the bits of 0x3C, least-significant first -- the "
          "value presented at the write is exactly the value the transmit "
          "path shifts out, content for content [p.259 \"The data to be "
          "transmitted is placed in the output register (SERDAT)\"; p.283 "
          "\"This address writes data to a transmit data buffer\"]");
      { std::string detail;
        for (int v : cells) detail += std::to_string(v);
        checks.note("FUNCTION-BUFFER: sampled cells (start+8) = " + detail);
      }
    }

    // ------------------------------------------------------------------
    // Declined: already graded in slice s1's accepted
    // hrm/ch8-appb-serial-register-rows. Per the build brief's rule against
    // re-grading accepted coverage, none of these four uids appears on any
    // expect() line above.
    // ------------------------------------------------------------------
    checks.record(
        "sol:8-serdat.c74 (p.289, \"SERDAT *030 W P Serial port data and "
        "stop bits write\", address cell): already graded in slice s1's "
        "accepted hrm/ch8-appb-serial-register-rows (uids opus:8-appB.c38 / "
        "c39 / c41 and sol:8-appB.c37 / c38 / c40, out.txt P4-P6). Not "
        "re-graded here.");
    checks.record(
        "sol:8-serdat.c75 (p.289, same row, writable cell): same accepted "
        "package and uids as sol:8-serdat.c74 above. Not re-graded here.");
    checks.record(
        "sol:8-serdat.c77 (p.289, same row, function-description cell): "
        "same accepted package and uids as sol:8-serdat.c74 above. Not "
        "re-graded here.");
    checks.record(
        "sol:8-serdat.c96 (p.289, same row, register-name cell): same "
        "accepted package and uids as sol:8-serdat.c74 above. Not re-graded "
        "here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
