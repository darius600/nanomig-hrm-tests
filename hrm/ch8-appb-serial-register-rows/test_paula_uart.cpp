// Test purpose: Appendix B's address-order rows for SERDATR $018, SERDAT
//   $030 and SERPER $032 -- address, access direction and function, each
//   graded as the fact composed from a row cell and its column-heading or
//   R/W-legend partner (SPECIFICATION.md's P1-P9).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed pages 287, 289
//   (Appendix B legend and register rows).
// Non-goals: bit-field layouts of SERDATR/SERPER beyond what P3/P9 need,
//   nine-bit receive mode, overrun/break handling, CHIP=Paula ownership (no
//   claim in this 18-record file restates it -- see SPECIFICATION.md) and
//   the leading "*" mark on each row (same reason).
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: nanomig/src/minimig-aga/paula_uart.v:4-17 module header -- ports
//   clk, clk7_en, reset, rga_i[7:0], data_i[15:0], data_o[15:0], uartbrk,
//   rbfmirror, txint, rxint, txd, rxd. No rd/hwr/lwr strobe and no chip
//   select: a cycle is fully described by rga_i + data_i, and read vs.
//   write are told apart only by which of data_o / internal state responds
//   -- this package's own Boundary caveat (SPECIFICATION.md), applied
//   below to every direction proposition.
//   paula_uart.v was read directly by this agent only AFTER
//   SPECIFICATION.md was committed (commit a9874c5); the sibling package
//   hrm/ch8-uart-frame-and-status's Makefile was read earlier, before the spec, only to
//   see how this fixture is *built* (verilator invocation), per this
//   package's own task -- not for its port list or its assertions, which
//   are that package's own RTL_REGRESSION content and are not reused here.
//
// METHODOLOGY:
//
// (1) ADDRESS (P1, P4, P7): shown by contrast within this module's own
//   three rows. paula_uart.v:258's data_o mux has exactly one nonzero arm,
//   gated on rga_i==REG_SERDATR[8:1] ($018); presenting the other two
//   addresses the same way answers 0x0000 (P1). paula_uart.v:52/61's write
//   decodes are each gated on one specific rga_i value ($032/$030); P4/P7
//   present the SAME data value at the wrong row's address first (no
//   effect) and then at the claimed address (an effect), so the effect is
//   tied to the address, not merely to "some write happened."
//
// (2) DIRECTION (P2, P5, P8), per SPECIFICATION.md's Boundary caveat: graded
//   as the conjunction actually observable, not the manual's letter
//   asserted directly.
//   - read-only (P2): the address answers (shared with P1), AND presenting
//     several distinct values on data_i while addressing $018 changes
//     nothing subsequently read there or at any decoded address (data_i
//     never reaches state at $018 because $018 equals neither write
//     decode's own comparison value).
//   - write-only (P5, P8): a value presented at the address reaches state
//     with an observable effect (shared with P4/P7's positive half), AND
//     presenting the row's own address again to "read" it back (itself
//     simultaneously another write, at this boundary) answers 0x0000, not
//     the value just written -- data_o's one nonzero arm never matches
//     $030 or $032.
//
// (3) FUNCTION (P3, P6, P9): each cell's prose names what kind of fact the
//   row states, not a bit layout, so each is graded on the shape the prose
//   names, not on a hand-derived expected bit pattern:
//   - P3 (SERDATR, "data and status read"): one read returns both a status
//     component that responds to a transmit (TBE, paula_uart.v:139) and a
//     data component that responds to a receive (rx_data, paula_uart.v:
//     215-219) -- the same address carries both kinds of fact.
//   - P6 (SERDAT, "data and stop bits write"): the written word supplies
//     both a data payload and stop-bit control in one write. paula_uart.v's
//     TX shifter (l.107-129) shifts out serdat's bits LSB-first until the
//     shift register empties; bit 8 (the byte's own 9th bit) is what makes
//     it empty exactly after the 8 data bits plus one more "1" bit --
//     flipping bit 8 alone, data payload unchanged, changes the frame's own
//     length.
//   - P9 (SERPER, "period and control"): two distinct SERPER values measure
//     two distinct, measurably different bit periods. This package's own
//     `ratio-or-absolute` gate: only a ratio of clk7_en ticks to clk7_en
//     ticks is asserted, no absolute time unit.
//
// CLAIM COVERAGE (all 18 claims in
//   ch8-appb-serial-register-rows.jsonl, per SPECIFICATION.md's P1-P9):
//   every uid is cited on a graded expect() line; none is declined.
//   Composing-partner uids opus:8-appB.c7 (column heading), opus:8-appB.c3
//   (W=write-only legend) and opus:8-appB.c4 (R=read-only legend) are
//   outside this package's own 18-record file but are cited alongside the
//   row uid on every assertion they compose with, per this package's own
//   build brief and matching the accepted `sol:7-appA-legend.c5` /
//   `opus:7-appA-pre.c10` joint-citation practice in
//   hrm/ch7-appb-beamcounter-marks.
//
// CONTROL CASES: P1 (SERDATR answers $018 with live content, $030/$032
//   answer 0x0000) and P4/P7 (a write reaches state at $030/$032, has no
//   effect at the wrong address) are each other's control on the SAME
//   measurement basis: P1 shows the read path is not stuck-at-zero, which
//   is what makes P5's/P8's "$030/$032 answer 0x0000" a meaningful "no,"
//   not a dead bus; P4/P7 show the write path is not stuck-at-no-effect,
//   which is what makes P2's "poison at $018 changes nothing" a meaningful
//   "no," not a dead input. No synthetic fourth register is needed; the
//   three documented rows already span both directions.
//   FIXTURE-DELIVERS-KNOWN-VALUE additionally proves the write-then-read/
//   write-then-transmit pipe is alive on this package's own independently
//   built obj_dir before anything else relies on it (two distinct SERDAT
//   payloads arrive as two distinct, correctly-ordered bit sequences).
//
// PER-ASSERTION MUTATION RECORDS. Every entry below was actually run by
//   this agent under WSL/Verilator: edit -> rebuild -> run -> confirm ONLY
//   the named assertion flips red (checked against the full 10-line PASS/
//   FAIL listing, not just the tally) -> revert -> rebuild -> confirm back
//   to baseline -- before moving to the next one. No oracle text or
//   expected-value literal was ever touched; nanomig/ was never touched.
//   Baseline throughout: 10 passed, 0 failed, exit code 0.
//
// MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: h55's SERDAT payload
//   changed 0x0155 -> 0x0100 (data 0x55 -> 0x00, bit 8 unchanged) -> 9
//   passed, 1 failed; only FIXTURE-DELIVERS-KNOWN-VALUE newly red (bits55
//   no longer matches the 0x55 LSB-first pattern). Reverted: 10/0.
// MUTATION P1-SERDATR-ADD stimulus red: serdatr_val's read address changed
//   A_SERDATR -> A_SERDAT -> 9/1; only P1-SERDATR-ADD newly red (now reads
//   0x0000, not 0x3800). Reverted: 10/0.
// MUTATION P2-SERDATR-READ-ONLY stimulus red: after1's presented address
//   changed A_SERDATR -> A_SERDAT -> 9/1; only P2-SERDATR-READ-ONLY newly
//   red (after1 no longer equals before). Reverted: 10/0.
// MUTATION P3-SERDATR-FUNCTION stimulus red: the received byte changed
//   0xA3 -> 0x5C, the hardcoded expected value (0xA3) left untouched ->
//   9/1; only P3-SERDATR-FUNCTION newly red (the received byte no longer
//   matches). Reverted: 10/0.
// MUTATION P4-SERDAT-ADD stimulus red: the claimed-address write's target
//   changed A_SERDAT -> A_SERPER -> 9/1; only P4-SERDAT-ADD newly red
//   (TSRE stays set -- no transmission starts at the wrong address).
//   Reverted: 10/0.
// MUTATION P5-SERDAT-WRITE-ONLY stimulus red: the readback's target
//   address changed A_SERDAT -> A_SERDATR -> 9/1; only P5-SERDAT-WRITE-ONLY
//   newly red (readback now returns live SERDATR content, not 0x0000).
//   Reverted: 10/0.
// MUTATION P6-SERDAT-FUNCTION stimulus red: hs's SERDAT payload changed
//   0x0155 -> 0x0055 (collapsing it onto hn's own value) -> 9/1; only
//   P6-SERDAT-FUNCTION newly red (bits_stop no longer matches expect_stop).
//   Reverted: 10/0.
// MUTATION P7-SERPER-ADD stimulus red: h_right's write target changed
//   A_SERPER -> A_SERDATR -> 9/1; only P7-SERPER-ADD newly red
//   (ticks_right collapses to the wrong-address baseline, 18 vs. 18, no
//   longer categorically different). Reverted: 10/0.
// MUTATION P8-SERPER-WRITE-ONLY stimulus red: hro's readback target
//   changed A_SERPER -> A_SERDATR -> 9/1; only P8-SERPER-WRITE-ONLY newly
//   red (readback now returns live SERDATR content, not 0x0000).
//   Reverted: 10/0.
// MUTATION P9-SERPER-FUNCTION stimulus red: h5v's write target changed
//   A_SERPER -> A_SERDATR -> 9/1; only P9-SERPER-FUNCTION newly red (t5
//   collapses to the wrong-address baseline, no longer greater than t2).
//   Reverted: 10/0.
//
// Final rebuild after the last revert: 10 passed, 0 failed, exit code 0,
//   confirmed again immediately before this comment was written; every
//   code line outside this header comment is unchanged from the
//   pre-mutation version.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vpaula_uart.h"

namespace {

// The fixed assertion form, verbatim from BUILD-BRIEF.md.
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

std::string hex16(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%04X", v);
    return std::string(buf);
}

// Register byte addresses (printed p.289). rga_i carries address bits
// [8:1]; a custom-chip byte address is always even, so the port value is
// the byte address shifted right by one -- read directly from
// paula_uart.v:21-23's own REG_SERDAT/REG_SERDATR/REG_SERPER localparams
// and their [8:1] comparisons.
constexpr int A_SERDATR = 0x018;
constexpr int A_SERDAT  = 0x030;
constexpr int A_SERPER  = 0x032;

class Harness {
public:
    explicit Harness() : dut(new Vpaula_uart) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->rga_i = 0;
        dut->data_i = 0;
        dut->uartbrk = 0;
        dut->rbfmirror = 0;
        dut->rxd = 1;  // idle high
        for (int i = 0; i < 40; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 40; ++i) tick();
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
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0) ? 1 : 0;
    }

    // Presents rga_i=addr, data_i=data for exactly one clk7_en-active edge
    // (the one stimulus shape this boundary has -- no separate strobe),
    // then releases both lines. Returns data_o sampled combinationally
    // right after that edge, while rga_i is still held at addr -- the same
    // single bus cycle serves as "read" or "write" depending only on which
    // row's address was presented.
    uint16_t present(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->rga_i = static_cast<uint8_t>((addr >> 1) & 0xFF);
        dut->data_i = data;
        tick();  // the clk7_en edge that samples rga_i/data_i
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_o);
        tick(); tick(); tick();  // let clk7_en cycle back around
        dut->rga_i = 0;
        dut->data_i = 0;
        dut->eval();
        return v;
    }

    // Triggers a transmission of serdat=0x0100 (all 8 data bits 0, bit 8 --
    // the stop-bit control -- set) and measures the clk7_en-tick distance
    // between the start bit's falling edge and the following rising edge.
    // Because every data bit is 0, txd stays low for exactly 9 bit periods
    // (the start bit plus the 8 zero data bits) before rising for the stop
    // bit -- so this distance, divided by 9, is one bit period in
    // clk7_en ticks, independent of SERPER's value. SERPER must already be
    // set (or deliberately left unset) before calling this.
    int measure_period_ticks() {
        present(A_SERDAT, 0x0100);
        int last = 1, fall_at = -1, rise_at = -1, t = 0;
        for (int i = 0; i < 20000 && rise_at < 0; ++i) {
            if (dut->clk7_en) {
                int cur = dut->txd;
                if (fall_at < 0 && last == 1 && cur == 0) fall_at = t;
                else if (fall_at >= 0 && last == 0 && cur == 1) rise_at = t;
                last = cur;
                ++t;
            }
            tick();
        }
        return (fall_at >= 0 && rise_at >= 0) ? (rise_at - fall_at) : -1;
    }

    // Captures a transmitted frame, sampling txd once per bit cell (at the
    // cell's mid-point) starting at the start bit's falling edge. Stops
    // after max_bits samples or once idle high has held for a full extra
    // bit cell with no further transition.
    std::vector<int> capture_tx(int bit_width_ticks, int max_bits, int max_ticks) {
        std::vector<int> bits;
        bool started = false;
        int last = 1, since_start = 0;
        for (int i = 0; i < max_ticks && static_cast<int>(bits.size()) < max_bits; ++i) {
            if (dut->clk7_en) {
                int cur = dut->txd;
                if (!started && last == 1 && cur == 0) {
                    started = true;
                    since_start = 0;
                }
                if (started) {
                    if (since_start % bit_width_ticks == bit_width_ticks / 2) {
                        bits.push_back(cur);
                    }
                    ++since_start;
                }
                last = cur;
            }
            tick();
        }
        return bits;
    }

    // Drives rxd with a start(0) + 8 data bits (LSB-first) + stop(1) frame
    // for byte b, timed at bit_width_ticks clk7_en ticks per bit -- SERPER's
    // own reload value, read directly from paula_uart.v:172/186/207's
    // {serper[14:0],1'b1} expression (2*SERPER+2 ticks per bit; the same
    // reload feeds both the RX and TX period counters). Returns true if
    // rxint was observed to pulse before the timeout.
    bool receive_byte(uint8_t b, int bit_width_ticks, int timeout_ticks) {
        std::vector<int> frame;
        frame.push_back(0);
        for (int i = 0; i < 8; ++i) frame.push_back((b >> i) & 1);
        frame.push_back(1);
        size_t idx = 0;
        int in_bit = 0;
        bool seen_irq = false;
        for (int i = 0; i < timeout_ticks; ++i) {
            if (dut->clk7_en) {
                dut->rxd = (idx < frame.size()) ? frame[idx] : 1;
                if (dut->rxint) seen_irq = true;
                if (idx < frame.size() && ++in_bit >= bit_width_ticks) {
                    in_bit = 0;
                    ++idx;
                }
            }
            tick();
            if (seen_irq && idx >= frame.size()) break;
        }
        dut->rxd = 1;
        return seen_irq;
    }

    Vpaula_uart* dut;
    int clk7_cycle = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= FIXTURE-DELIVERS-KNOWN-VALUE (no page) =================
    {
        Harness h;
        h.present(A_SERPER, 10);
        int width = h.measure_period_ticks() / 9;  // calibrate from the module itself
        Harness h55;
        h55.present(A_SERPER, 10);
        h55.present(A_SERDAT, 0x0155);  // data 0x55, bit8=1 (stop)
        auto bits55 = h55.capture_tx(width, 10, 4000);
        Harness haa;
        haa.present(A_SERPER, 10);
        haa.present(A_SERDAT, 0x01AA);  // data 0xAA, bit8=1 (stop)
        auto bitsaa = haa.capture_tx(width, 10, 4000);
        checks.note("Calibrated bit width " + std::to_string(width) +
                    " clk7_en ticks at SERPER=10; SERDAT=0x0155 (data 0x55) "
                    "and SERDAT=0x01AA (data 0xAA) each captured " +
                    std::to_string(bits55.size()) + " and " +
                    std::to_string(bitsaa.size()) + " bit(s) respectively.");
        std::vector<int> expect55 = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
        std::vector<int> expectaa = {0, 0, 1, 0, 1, 0, 1, 0, 1, 1};
        checks.expect(
            bits55 == expect55 && bitsaa == expectaa && bits55 != bitsaa,
            "FIXTURE-DELIVERS-KNOWN-VALUE: two distinct SERDAT payloads "
            "(0x55, 0xAA), written through this package's own independently "
            "built obj_dir, arrive on txd as two distinct, correctly "
            "start/LSB-first-data/stop-ordered bit sequences -- the "
            "write-then-transmit path actually delivers a known, distinct "
            "word before any FUNCTION/direction assertion below depends on "
            "it.");
    }

    // ============================================================
    // SERDATR $018 -- P1 (address), P2 (direction), P3 (function)
    // ============================================================
    {
        Harness h;
        uint16_t serdatr_val = h.present(A_SERDATR, 0xFFFF);
        uint16_t serdat_probe = h.present(A_SERDAT, 0x0000);
        uint16_t serper_probe = h.present(A_SERPER, 0x0000);
        checks.note("Out of reset: SERDATR($018) reads " + hex16(serdatr_val) +
                    "; SERDAT($030) and SERPER($032), read the same way, "
                    "read " + hex16(serdat_probe) + " and " +
                    hex16(serper_probe) + ".");
        checks.expect(
            serdatr_val == 0x3800 && serdat_probe == 0x0000 && serper_probe == 0x0000,
            "P1-SERDATR-ADD: addressing $018 answers with live register "
            "content (0x3800: OVRUN=0, RBF=0, TBE=1, TSRE=1, RXD=1, out of "
            "reset); addressing $030 or $032 the same way answers 0x0000 -- "
            "this module decodes $018, uniquely among these three rows, as "
            "a readable address. 'SERDATR      *018' (p.289, ADD cell), "
            "composed with the column heading 'NAME        ADD     R/W   "
            "CHIP    FUNCTION' (p.289) naming ADD as the address column. "
            "opus:8-appB.c33, sol:8-appB.c32, composing partner opus:8-appB.c7.");
    }
    {
        Harness h;
        uint16_t before = h.present(A_SERDATR, 0x0000);
        uint16_t after1 = h.present(A_SERDATR, 0xFFFF);
        uint16_t after2 = h.present(A_SERDATR, 0xAAAA);
        uint16_t after3 = h.present(A_SERDATR, 0x1234);
        checks.note("SERDATR($018) reads " + hex16(before) + ", " +
                    hex16(after1) + ", " + hex16(after2) + ", " +
                    hex16(after3) + " across four presentations differing "
                    "only in data_i.");
        checks.expect(
            before == 0x3800 && after1 == before && after2 == before && after3 == before,
            "P2-SERDATR-READ-ONLY: $018 answers with live content (the read "
            "half); presenting three different values on the data input "
            "while addressing $018 (0xFFFF, 0xAAAA, 0x1234) changes nothing "
            "subsequently read there -- no value presented at this address "
            "reaches any state, because $018 matches neither write decode's "
            "own comparison value ($030, $032). 'SERDATR      *018   R' "
            "(p.289, R/W cell), composed with the legend "
            "'W=write-only; R=read-only,' (p.287, R half). "
            "opus:8-appB.c34, sol:8-appB.c33, composing partner opus:8-appB.c4.");
    }
    {
        Harness h;
        uint16_t reset_val = h.present(A_SERDATR, 0x0000);
        h.present(A_SERPER, 5);
        h.present(A_SERDAT, 0x0155);  // triggers a transmission
        uint16_t after_write = h.present(A_SERDATR, 0x0000);
        bool rx_ok = h.receive_byte(0xA3, 2 * 5 + 2, 3000);
        uint16_t after_rx = h.present(A_SERDATR, 0x0000);
        checks.note("SERDATR before any activity: " + hex16(reset_val) +
                    "; right after a SERDAT write: " + hex16(after_write) +
                    " (TSRE bit 0x1000 " +
                    std::string((after_write & 0x1000) ? "still set" : "cleared") +
                    "; note TBE bit 0x2000 was found, while building this "
                    "test, to bounce back to 1 one clk7_en cycle after the "
                    "write -- paula_uart.v:139's own bottom-of-block "
                    "assignment reasserts it whenever tx_state reads as "
                    "TX_IDLE, which it still does for one more cycle after "
                    "the load -- so TSRE, not TBE, is the status bit this "
                    "assertion reads; not itself a claim in this package's "
                    "18); rxint observed during receive_byte(0xA3): " +
                    std::string(rx_ok ? "yes" : "no") +
                    "; SERDATR after the receive: " + hex16(after_rx) +
                    " (low byte " + hex16(after_rx & 0x00FF) + ").");
        checks.expect(
            reset_val == 0x3800 && (after_write & 0x1000) == 0 && rx_ok &&
                (after_rx & 0x00FF) == 0xA3,
            "P3-SERDATR-FUNCTION: one address carries both a status "
            "component and a data component. Status: a SERDAT write clears "
            "TSRE (bit 12, mask 0x1000) in the very next SERDATR read. "
            "Data: receiving byte 0xA3 over rxd (start, 8 data bits "
            "LSB-first, stop, timed at SERPER's own reload) makes the same "
            "byte appear in SERDATR's low 8 bits. 'SERDATR      *018   R   "
            "        P    Serial port data and status read' (p.289, "
            "FUNCTION cell), composed with the column heading naming "
            "FUNCTION as the fifth column. opus:8-appB.c36, sol:8-appB.c35, "
            "composing partner opus:8-appB.c7.");
    }

    // ============================================================
    // SERDAT $030 -- P4 (address), P5 (direction), P6 (function)
    // ============================================================
    {
        Harness h;
        uint16_t tsre_before = h.present(A_SERDATR, 0x0000) & 0x1000;
        h.present(A_SERDATR, 0x0155);  // wrong address for a SERDAT write
        uint16_t tsre_wrong = h.present(A_SERDATR, 0x0000) & 0x1000;
        h.present(A_SERDAT, 0x0155);  // the claimed address
        uint16_t tsre_right = h.present(A_SERDATR, 0x0000) & 0x1000;
        checks.note("TSRE (mask 0x1000) before any write: " + hex16(tsre_before) +
                    "; after presenting 0x0155 at $018 (wrong address): " +
                    hex16(tsre_wrong) + "; after presenting the identical "
                    "value at $030 (claimed address): " + hex16(tsre_right) + ".");
        checks.expect(
            tsre_before == 0x1000 && tsre_wrong == 0x1000 && tsre_right == 0x0000,
            "P4-SERDAT-ADD: presenting 0x0155 at $018 (SERDATR's own "
            "address, not SERDAT's) leaves TSRE set -- no transmission "
            "starts; presenting the identical value at $030 clears TSRE -- "
            "a transmission starts. Only $030 decodes as SERDAT's write "
            "target. 'SERDAT       *030' (p.289, ADD cell), composed with "
            "the column heading naming ADD as the address column (p.289). "
            "opus:8-appB.c38, sol:8-appB.c37, composing partner opus:8-appB.c7.");
    }
    {
        Harness h;
        h.present(A_SERPER, 10);
        int width = h.measure_period_ticks() / 9;
        Harness h2;
        h2.present(A_SERPER, 10);
        h2.present(A_SERDAT, 0x0155);
        auto bits = h2.capture_tx(width, 10, 4000);
        uint16_t readback = h2.present(A_SERDAT, 0x0000);
        std::vector<int> expected = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
        checks.note("SERDAT=0x0155 transmitted " + std::to_string(bits.size()) +
                    " sampled bit(s); reading $030 back afterward answers " +
                    hex16(readback) + ".");
        checks.expect(
            bits == expected && readback == 0x0000,
            "P5-SERDAT-WRITE-ONLY: writing 0x0155 to $030 has an observable "
            "effect (transmits start, 0x55 LSB-first, stop = "
            "0,1,0,1,0,1,0,1,0,1 on txd); presenting $030 again to read it "
            "back (data_i=0) answers 0x0000, not the previously written "
            "0x0155 -- data_o's one nonzero arm is $018, so $030 never "
            "answers a read regardless of what was written there. "
            "'SERDAT       *030   W' (p.289, R/W cell), composed with the "
            "legend 'W=write-only; R=read-only,' (p.287, W half). "
            "opus:8-appB.c39, sol:8-appB.c38, composing partner opus:8-appB.c3.");
    }
    {
        Harness h;
        h.present(A_SERPER, 10);
        int width = h.measure_period_ticks() / 9;
        Harness hs;
        hs.present(A_SERPER, 10);
        hs.present(A_SERDAT, 0x0155);  // bit8=1: 8 data bits + explicit stop
        auto bits_stop = hs.capture_tx(width, 12, 4000);
        Harness hn;
        hn.present(A_SERPER, 10);
        hn.present(A_SERDAT, 0x0055);  // bit8=0: same low byte, no stop bit
        auto bits_nostop = hn.capture_tx(width, 12, 4000);
        // capture_tx always samples exactly 12 fixed bit-cells (it does not
        // stop early at idle), so the two frames are compared by CONTENT,
        // not length. With bit8=1 the shift register empties after 9
        // shifts (8 data bits + the stop-control bit), giving one distinct
        // stop-level cell before idle; with bit8=0 (same low byte 0x55) it
        // empties one shift sooner (7 data-bit shifts, since bit 7 of 0x55
        // is itself 0 and needs no separate shift to reach zero), so cell 8
        // is already idle-high instead of the data/stop pattern -- the two
        // 12-cell captures must differ starting at that cell.
        std::vector<int> expect_stop   = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1};
        std::vector<int> expect_nostop = {0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1};
        auto vec_to_str = [](const std::vector<int>& v) {
            std::string s;
            for (int b : v) s += std::to_string(b);
            return s;
        };
        checks.note("SERDAT=0x0155 (bit8=1) samples " + vec_to_str(bits_stop) +
                    "; SERDAT=0x0055 (bit8=0, same low byte 0x55) samples " +
                    vec_to_str(bits_nostop) + " (12 fixed bit-cells each).");
        checks.expect(
            bits_stop == expect_stop && bits_nostop == expect_nostop &&
                bits_stop != bits_nostop,
            "P6-SERDAT-FUNCTION: SERDAT's written word supplies both a data "
            "payload and stop-bit control in one write. Flipping only bit 8 "
            "(0x0155 -> 0x0055), the low-byte data payload (0x55) "
            "unchanged, changes the transmitted frame's own content (cells "
            "0-11: " + vec_to_str(expect_stop) + " with bit8=1 vs. " +
            vec_to_str(expect_nostop) + " with bit8=0, diverging at cell "
            "8) -- bit 8 is what the shift register treats as the frame's "
            "own stop control, not part of the data payload. "
            "'SERDAT       *030   W           P    Serial port data and "
            "stop bits write' (p.289, FUNCTION cell), composed with the "
            "column heading naming FUNCTION as the fifth column. "
            "opus:8-appB.c41, sol:8-appB.c40, composing partner opus:8-appB.c7.");
    }

    // ============================================================
    // SERPER $032 -- P7 (address), P8 (direction), P9 (function)
    // ============================================================
    {
        Harness h_wrong;
        h_wrong.present(A_SERDATR, 20);  // wrong address for SERPER
        int ticks_wrong = h_wrong.measure_period_ticks();
        Harness h_right;
        h_right.present(A_SERPER, 20);  // claimed address
        int ticks_right = h_right.measure_period_ticks();
        checks.note("Presenting SERPER=20 at $018 (wrong address) then "
                    "transmitting measures " + std::to_string(ticks_wrong) +
                    " clk7_en ticks between the start-bit falling edge and "
                    "the stop-bit rising edge; presenting the identical "
                    "value at $032 (claimed address) measures " +
                    std::to_string(ticks_right) + " ticks.");
        checks.expect(
            ticks_wrong > 0 && ticks_right > 0 && ticks_right > ticks_wrong * 5,
            "P7-SERPER-ADD: presenting 20 at $018 (SERDATR's own address, "
            "not SERPER's) leaves the bit period at its unaffected, "
            "reset-time value (measured " + std::to_string(ticks_wrong) +
            " ticks over 9 bit periods); presenting the identical value at "
            "$032 changes it to " + std::to_string(ticks_right) +
            " ticks -- categorically different, not a rounding coincidence. "
            "Only $032 decodes as SERPER's write target. "
            "'SERPER       *032' (p.289, ADD cell), composed with the "
            "column heading naming ADD as the address column (p.289). "
            "opus:8-appB.c43, sol:8-appB.c42, composing partner opus:8-appB.c7.");
    }
    {
        Harness h5;
        h5.present(A_SERPER, 5);
        int ticks5 = h5.measure_period_ticks();
        Harness h20;
        h20.present(A_SERPER, 20);
        int ticks20 = h20.measure_period_ticks();
        Harness hro;
        hro.present(A_SERPER, 0x1234);
        uint16_t readback = hro.present(A_SERPER, 0x0000);
        checks.note("SERPER=5 measures " + std::to_string(ticks5) +
                    " ticks over 9 bit periods; SERPER=20 measures " +
                    std::to_string(ticks20) + " ticks; reading $032 back "
                    "after writing 0x1234 answers " + hex16(readback) + ".");
        checks.expect(
            ticks20 > ticks5 * 2 && readback == 0x0000,
            "P8-SERPER-WRITE-ONLY: writing SERPER=5 vs. SERPER=20 changes "
            "the measured bit period (" + std::to_string(ticks5) + " vs. " +
            std::to_string(ticks20) + " ticks) -- the write reaches state; "
            "presenting $032 again to read it back (data_i=0, after writing "
            "0x1234) answers 0x0000, not the previously written value -- "
            "data_o's one nonzero arm is $018, so $032 never answers a "
            "read. 'SERPER       *032   W' (p.289, R/W cell), composed with "
            "the legend 'W=write-only; R=read-only,' (p.287, W half). "
            "opus:8-appB.c44, sol:8-appB.c43, composing partner opus:8-appB.c3.");
    }
    {
        Harness h2v;
        h2v.present(A_SERPER, 2);
        int t2 = h2v.measure_period_ticks();
        Harness h5v;
        h5v.present(A_SERPER, 5);
        int t5 = h5v.measure_period_ticks();
        double width2 = t2 / 9.0, width5 = t5 / 9.0;
        checks.note("SERPER=2 gives a bit width of " + std::to_string(width2) +
                    " clk7_en ticks; SERPER=5 gives " + std::to_string(width5) +
                    " ticks (each measured over 9 bit periods, " +
                    std::to_string(t2) + " and " + std::to_string(t5) +
                    " ticks total).");
        checks.expect(
            t2 > 0 && t5 > 0 && t5 > t2,
            "P9-SERPER-FUNCTION: a larger SERPER value produces a "
            "measurably longer bit period (SERPER=2: " + std::to_string(width2) +
            " clk7_en ticks/bit; SERPER=5: " + std::to_string(width5) +
            " ticks/bit) -- a dimensionless ratio of clk7_en cycles to "
            "clk7_en cycles, no absolute time unit asserted. "
            "'SERPER       *032   W           P    Serial port period and "
            "control' (p.289, FUNCTION cell), composed with the column "
            "heading naming FUNCTION as the fifth column. "
            "opus:8-appB.c46, sol:8-appB.c45, composing partner opus:8-appB.c7.");
    }

    // A SUMMARY THE COVERAGE LEDGER CAN READ.
    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
