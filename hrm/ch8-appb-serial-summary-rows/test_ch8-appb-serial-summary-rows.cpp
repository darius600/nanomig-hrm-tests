// Test purpose: Appendix B's address-order register-summary rows for
//   SERDATR $018, SERDAT $030 and SERPER $032 -- address, access direction
//   and function, each graded as the fact composed from a row cell and its
//   column-heading or R/W-legend partner (SPECIFICATION.md's R1-R9) -- plus
//   ADKCON bit 11 (UARTBRK), named independently by printed p.257 and
//   printed p.265 (SPECIFICATION.md's U1-U4, U2 split into U2a/U2b). Four
//   further claims (ADKCONR's row/ADD/R-W/FUNCTION, printed p.289) are
//   declined with record(), citing the accepted package and uid that
//   already grades each -- see SPECIFICATION.md's "Four claims are already
//   graded elsewhere".
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 257, 265, 287, 289; committed specification (SPECIFICATION.md,
//   this package, commit 2f706f3).
// Fixture boundary: `paula` (paula.v), compiled with everything it itself
//   instantiates -- paula_uart.v, paula_intcontroller.v, paula_floppy.v
//   (and its own paula_floppy_fifo.v), paula_audio.v (and its own
//   paula_audio_channel.v, paula_audio_mixer.v, paula_audio_volume.v).
//
// RTL read for step 2/3, after SPECIFICATION.md was committed (2f706f3):
//   paula.v in full, paula_uart.v in full.
//   - paula.v:67 `input [8:1] reg_address_in`; paula.v:126-127
//     `ADKCON=9'h09e`, `ADKCONR=9'h010`; ADKCON write paula.v:182-195 (SET/
//     CLR convention: data_in[15]=1 ORs data_in[14:0] in, =0 ANDs the
//     complement out); ADKCONR read paula.v:198 `assign adkconr[15:0] =
//     (reg_address_in[8:1]==ADKCONR[8:1]) ? {1'b0,adkcon[14:0]} : 16'h0000`;
//     paula.v:203-217 instantiates paula_uart as `pu1`, `.rga_i
//     (reg_address_in)`, `.data_i(data_in)`, `.data_o(uartdata_out)`,
//     `.uartbrk(adkcon[11])`; paula.v:152 `assign data_out = uartdata_out |
//     intdata_out | diskdata_out | adkconr` -- confirms the four mux arms
//     are mutually exclusive by address for every address this file uses
//     (DMACON/ADKCON/ADKCONR decode only at 0x4B/0x4F/0x08; disk addresses
//     are all outside 0x0C/0x18/0x19; INTENA/INTENAR/INTREQ/INTREQR decode
//     outside 0x0C/0x18/0x19 too).
//   - paula_uart.v:20-22 `REG_SERDAT=9'h030`, `REG_SERDATR=9'h018`,
//     `REG_SERPER=9'h032`; as `[8:1]` (the form `rga_i`/`reg_address_in`
//     actually carries): SERDAT=0x18, SERDATR=0x0C, SERPER=0x19 -- three
//     DISTINCT word addresses, confirming R1/R4/R7's addresses do not
//     collide with each other.
//   - paula_uart.v:45-65: SERPER and SERDAT are write-registers with no
//     read arm anywhere (`serper`/`serdat` are never read onto `data_o`);
//     write-decoded only at `rga_i==REG_SERPER[8:1]` (0x19, serper) and
//     `rga_i==REG_SERDAT[8:1]` (0x18, serdat), each gated by `clk7_en`
//     alone -- there is no separate read/write strobe at this boundary, so
//     a "read" that never advances a `clk7_en`-qualifying clock edge cannot
//     be misread as a write (this package's own `read_reg`, below, never
//     ticks the clock -- the technique already used at this identical
//     boundary by `hrm/ch8-adkcon-disk-control-bits`'s `Rig::read_reg`,
//     read for the pattern only, not for its assertions or expected
//     values).
//   - paula_uart.v:245-260: `serdatr = {rx_ovrun, rx_rbf, tx_tbe, tx_tsre,
//     rx_rxd}` occupies bits [15:11] of `data_o`, bit 10 is a hard 0, bits
//     [9:0] are `rx_data`; `data_o = (rga_i==REG_SERDATR[8:1]) ? {serdatr,
//     1'b0, rx_data} : 16'h0000`. Reset values: `tx_tbe<=1`, `tx_tsre<=1`
//     (paula_uart.v:80-81); `rx_rbf<=0`, `rx_rxd<=1`, `rx_ovrun<=0`
//     (paula_uart.v:150-154); `rx_data` has no reset term, Verilator's
//     default-0. So SERDATR out of reset = {OVRUN=0,RBF=0,TBE=1,TSRE=1,
//     RXD=1} at bits[15:11] = 5'b00111 << 11 = 0x3800, bits[9:0]=0 --
//     confirmed by hand from the RTL text before any build, then confirmed
//     again by this package's own FIXTURE-LIVE-SERDATR-RESET measurement
//     below (MEASURED, not asserted -- R1 is what grades it).
//   - paula_uart.v:135-138 (inside the always-block that also drives
//     `tx_txd` from the TX state machine): `if (uartbrk) tx_txd <= #1
//     1'b0;`, evaluated every `clk7_en` tick AFTER the `case (tx_state)`
//     block, so it overrides whatever the state machine itself wrote that
//     same tick whenever `uartbrk` (=`adkcon[11]`, wired at paula.v:211) is
//     1; clearing bit 11 hands `tx_txd` back to the state machine (idle ->
//     1). `assign txd = tx_txd` (paula_uart.v:255), carried straight to
//     `paula`'s own `txd` output port -- no further logic between
//     `tx_txd` and the pin this file drives/reads.
//   - Write timing: ADKCON's own write (paula.v:182-195) needs one
//     `clk7_en` tick to update `adkcon`; `uartbrk` is a combinational wire
//     off that register, so the *next* `clk7_en` tick inside paula_uart is
//     when `tx_txd` actually moves -- `settle_edges(4)` after each ADKCON
//     write, used throughout U2a/U2b/U4 below, is comfortably more than
//     the two ticks this requires.
//
// METHODOLOGY:
//
// (1) ADDRESS (R1, R4, R7): SERDATR/SERDAT/SERPER decode to three distinct
//   word addresses (0x0C/0x18/0x19) with no other mux arm answering there
//   (see RTL facts above). R1 shows SERDATR answers with live content while
//   SERDAT/SERPER (probed the same way) answer 0x0000; R4/R7 show a value
//   presented at SERDATR's own address (harmless -- 0x0C matches neither
//   write decode) leaves SERDAT's/SERPER's own effect unchanged, while the
//   identical value presented at the claimed address changes it.
//
// (2) DIRECTION (R2, R5, R8), per SPECIFICATION.md's Boundary caveat:
//   graded as the conjunction actually observable.
//   - read-only (R2): the address answers (shared with R1), AND several
//     distinct values *written* (a real `clk7_en` edge, via `write_reg`) at
//     SERDATR's own address change nothing subsequently read there --
//     harmless because 0x0C matches neither write decode's own comparison
//     value.
//   - write-only (R5, R8): a value written to the address reaches state
//     with an observable effect (shared with R4/R7's positive half), AND
//     `read_reg` (which never ticks a `clk7_en` edge, so it cannot itself
//     perform the write this boundary's missing strobe would otherwise
//     risk) at that same address answers 0x0000, not the value just
//     written -- `data_o`'s one nonzero arm for these two addresses is
//     always SERDATR's.
//
// (3) FUNCTION (R3, R6, R9): each cell's prose names what kind of fact the
//   row states, not a bit layout.
//   - R3 (SERDATR, "data and status read"): one read returns both a status
//     component that responds to a transmit (TSRE, paula_uart.v:114-118)
//     and a data component that responds to a receive (rx_data,
//     paula_uart.v:207-212) -- the same address carries both.
//   - R6 (SERDAT, "data and stop bits write"): the TX shifter shifts
//     `serdat` out LSB-first until the shift register empties; bit 8 (the
//     word's own 9th bit) is what makes it empty exactly one shift later --
//     flipping bit 8 alone, low-byte data payload unchanged, changes the
//     transmitted frame's own length/content.
//   - R9 (SERPER, "period and control"): two distinct SERPER values measure
//     two distinct, measurably different bit periods -- a ratio of
//     `clk7_en` ticks to `clk7_en` ticks, no absolute time unit (this
//     slice's own `not constructible` rule for absolute frequency/period;
//     `ntsc` never reaches this boundary at all).
//
// (4) UARTBRK LOCATION (U1, U3): ADKCON bit 11 set/cleared via the SET/CLR
//   write convention reads back at bit 11 of ADKCONR and nowhere else --
//   pure storage, no `txd` involved, independent of U2a/U2b/U4's
//   behavioural measurement. U1 cites p.265's row, U3 cites p.257's row;
//   same code, different page cited, because the manual states the same
//   bit-11 location fact on both pages in different words and this
//   package's own task requires each proposition to name its page rather
//   than collapsing them.
//
// (5) UARTBRK BEHAVIOUR (U2a, U2b, U4): idle-high `txd`, set ADKCON bit 11,
//   `txd` forced low; clear bit 11, `txd` released back to idle-high. U2a
//   cites p.265's "forces a UART break" clause, U2b cites p.265's "(clears
//   TXD)" parenthetical (graded together with `opus:8-adkcon.c26`, whose
//   own `classify.evidence` narrows its graded content to exactly this),
//   U4 cites p.257's independently-worded "Force the transmit pin to
//   zero." All three measure the identical RTL fact (paula_uart.v:136) --
//   the manual states it three times, on two pages, in three clauses, and
//   this package's own task forbids collapsing that into one assertion.
//
// CLAIM COVERAGE: all 24 claims in
//   ch8-appb-serial-summary-rows.jsonl. 20 are graded on the expect() lines
//   below (R1=2, R2=1, R3=1, R4=2, R5=1, R6=1, R7=2, R8=1, R9=1, U1=3,
//   U2a=1, U2b=2, U3=1, U4=1 -- 12+8=20); 4 (`opus:8-regsum.c20`, `c21`,
//   `c22`, `c24`) are declined with record(), citing the accepted package
//   and uid that already grades each -- see SPECIFICATION.md.
//
// CONTROL CASES: R1 (SERDATR answers with live content; SERDAT/SERPER,
//   probed the same way, answer 0x0000) and R4/R7 (a write reaches state at
//   SERDAT/SERPER, has no effect at SERDATR's own address) are each other's
//   control on the same measurement basis, exactly as
//   `ch8-appb-serial-register-rows` (the paula_uart-level sibling) already
//   established for this same three-row shape. U1/U3 (two independently
//   worded pages naming the same bit) and U2a/U2b/U4 (two independently
//   worded pages describing the same observable effect) are each other's
//   control in the same sense.
// FIXTURE-LIVE (no claim id, no page, note()-only per the precedent in
//   `hrm/ch8-adkcon-disk-control-bits` -- a fixture control carries no
//   conformance content, so grading it through expect() would cost this
//   package uid-coverage for nothing): two distinct ADKCON words round-trip
//   through ADKCONR before U1/U3 are trusted; two distinct SERDAT payloads
//   arrive as two distinct, correctly-ordered bit sequences on `txd`
//   before R3/R5/R6/R8/R9/U2a/U2b/U4 are trusted.
//
// MUTATION METHODOLOGY: every `expect()` below is guarded by a
// compile-time `#if MUT_n` (n = 0..13, one per assertion, in the order
// they appear), following `MUT ?=` / `-D$(MUT)=1` in the Makefile, exactly
// the mechanism `hrm/ch8-keyboard-serial-register` and several other
// accepted packages already use. `make MUT=MUT_n test` rebuilds from a
// clean $(OBJ_DIR) (the Makefile's own `test` recipe does this
// unconditionally, so a stale binary from a previous variant is never
// silently reused) and reports that one variant's result; unset/empty MUT
// builds the baseline. Every one of the fourteen was actually run this
// way under WSL/Verilator: edit-by-macro -> `make MUT=MUT_n test` -> confirm
// ONLY that one assertion flips red (checked against the full PASS/FAIL
// listing, not just the tally) -> `make test` (MUT unset) -> confirm back to
// baseline -- before moving to the next one. No oracle text or
// expected-value literal was ever touched; nanomig/ was never touched. The
// per-mutation results are logged at the bottom of this header, filled in
// after each was actually run.
//
// Every mutation below was actually run under WSL/Verilator (`make
// MUT=MUT_n test`), confirmed against the full PASS/FAIL listing (not just
// the tally) to flip ONLY the named assertion, then reverted (`make test`,
// MUT unset) and confirmed back to the 14/0 baseline before the next one.
// Baseline throughout: 14 passed, 0 failed.
//
// MUTATION R1-SERDATR-ADD stimulus red MUT_0: serdat_probe's read address
//   changed from $030 to $018 (SERDATR's own) -> 13/1, only R1-SERDATR-ADD
//   newly red (serdat_probe now reads 0x3800, not 0x0000). Reverted.
// MUTATION R2-SERDATR-READONLY stimulus red MUT_1: after3's read-back
//   address changed from $018 to $030 -> 13/1, only R2-SERDATR-READONLY
//   newly red (after3 now reads SERDAT's structural 0x0000, not SERDATR's
//   0x3800). Reverted.
// MUTATION R3-SERDATR-FUNCTION stimulus red MUT_2: the received byte
//   changed 0xA3 -> 0x5C, the hardcoded expected value (0xA3) left
//   untouched -> 13/1, only R3-SERDATR-FUNCTION newly red (the received
//   byte no longer matches). Reverted.
// MUTATION R4-SERDAT-ADD stimulus red MUT_3: the claimed-address write's
//   target changed from SERDAT to SERPER -> 13/1, only R4-SERDAT-ADD newly
//   red (TSRE stays set -- no transmission starts at the wrong address).
//   Reverted.
// MUTATION R5-SERDAT-WRITEONLY stimulus red MUT_4: the readback's target
//   address changed from $030 to $018 -> 13/1, only R5-SERDAT-WRITEONLY
//   newly red (readback now returns live SERDATR content, 0x3800, not
//   0x0000). Reverted.
// MUTATION R6-SERDAT-FUNCTION stimulus red MUT_5: hn's SERDAT payload
//   changed 0x0055 -> 0x0155 (collapsing it onto hs's own value) -> 13/1,
//   only R6-SERDAT-FUNCTION newly red (bits_nostop no longer differs from
//   bits_stop). Reverted.
// MUTATION R7-SERPER-ADD stimulus red MUT_6: h_right's write target
//   changed from SERPER to SERDATR (duplicating h_wrong's own target) ->
//   13/1, only R7-SERPER-ADD newly red (ticks_right collapses to the
//   wrong-address baseline, no longer categorically different). Reverted.
// MUTATION R8-SERPER-WRITEONLY stimulus red MUT_7: readback's target
//   address changed from $032 to $018 -> 13/1, only R8-SERPER-WRITEONLY
//   newly red (readback now returns live SERDATR content, not 0x0000).
//   Reverted.
// MUTATION R9-SERPER-FUNCTION stimulus red MUT_8: h5v's SERPER value
//   changed from 5 to 2 (collapsing it onto h2v's own stimulus) -> 13/1,
//   only R9-SERPER-FUNCTION newly red (t5 no longer greater than t2).
//   Reverted.
// MUTATION U1-UARTBRK-LOCATION-P265 stimulus red MUT_9: the SET write's
//   field changed from bit 11 (0x0800) to bit 10 (0x0400, WORDSYNC) -> 13/1,
//   only U1-UARTBRK-LOCATION-P265 newly red (ADKCONR bit 11 never sets).
//   Reverted.
// MUTATION U2A-UARTBRK-FORCES-BREAK-P265 stimulus red MUT_10: the SET
//   write's field changed from bit 11 to bit 10 -> 13/1, only
//   U2A-UARTBRK-FORCES-BREAK-P265 newly red (txd never forced low).
//   Reverted.
// MUTATION U2B-UARTBRK-CLEARS-TXD-P265 stimulus red MUT_11: the SET
//   write's field changed from bit 11 to bit 10 -> 13/1, only
//   U2B-UARTBRK-CLEARS-TXD-P265 newly red (txd never forced low).
//   Reverted.
// MUTATION U3-UARTBRK-LOCATION-P257 stimulus red MUT_12: the SET write's
//   field changed from bit 11 to bit 10 -> 13/1, only
//   U3-UARTBRK-LOCATION-P257 newly red (ADKCONR bit 11 never sets).
//   Reverted.
// MUTATION U4-UARTBRK-FORCES-ZERO-P257 stimulus red MUT_13: the SET
//   write's field changed from bit 11 to bit 10 -> 13/1, only
//   U4-UARTBRK-FORCES-ZERO-P257 newly red (txd never forced low).
//   Reverted.
//
// Final rebuild after the last revert: 14 passed, 0 failed, confirmed
// again immediately before this comment was written; every code line
// outside this header comment is unchanged from the pre-mutation version.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <verilated.h>
#include "Vpaula.h"

// Replayable mutations: `make MUT=MUT_n test` rebuilds with mutation n's
// stimulus substituted in place of the baseline at that one site; unset
// MUT builds the unmutated baseline. Fourteen sites, MUT_0 through MUT_13,
// one per assertion below, in the order they appear.
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

namespace {

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

// Register word addresses (reg_address_in[8:1] -- the printed $DFFxxx byte
// offset shifted right by one), read directly from paula.v's own
// ADKCON/ADKCONR parameters and paula_uart.v's own REG_SERDAT/REG_SERDATR/
// REG_SERPER parameters, both [8:1]-sliced the same way.
constexpr int A_NONE    = 0x00;
constexpr int A_SERDATR = 0x0C;  // $018 >> 1
constexpr int A_SERDAT  = 0x18;  // $030 >> 1
constexpr int A_SERPER  = 0x19;  // $032 >> 1
constexpr int A_ADKCON  = 0x4F;  // $09E >> 1
constexpr int A_ADKCONR = 0x08;  // $010 >> 1

constexpr uint16_t SETCLR = 0x8000;

class Harness {
public:
    explicit Harness() : dut(new Vpaula) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = A_NONE;
        dut->data_in = 0;
        dut->rxd = 1;  // idle-high, matches real RS232 idle
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 1;
        dut->direc = 0;
        dut->_sel = 0xF;           // all drives deselected (active low)
        dut->side = 0;
        dut->_motor = 1;           // inactive; not consulted by this package's data path
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        settle_edges(4);
        dut->reset = 0;
        settle_edges(4);
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
        dut->clk7_en  = (clk7_cycle == 0) ? 1 : 0;
        dut->clk7n_en = (clk7_cycle == 2) ? 1 : 0;
    }

    // Advance until exactly n qualifying (clk7_en-active) rising edges have
    // been applied.
    void settle_edges(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    // One qualifying-edge write, then the address is withdrawn so no
    // further qualifying edge repeats it.
    void write_reg(int addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle_edges(1);
        dut->reg_address_in = A_NONE;
        dut->data_in = 0;
        settle_edges(2);
    }

    // Presents an address on reg_address_in and samples data_out purely
    // combinationally -- NO clk7_en-qualifying edge is ever advanced here,
    // so this can never itself perform the write this boundary's missing
    // read/write strobe would otherwise risk at a write-only address
    // (SERDAT/SERPER): a write only ever latches on a clk7_en edge
    // (paula_uart.v:45-65), and this function never produces one.
    uint16_t read_reg(int addr) {
        dut->reg_address_in = addr;
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = A_NONE;
        dut->eval();
        return v;
    }

    // Triggers a transmission of serdat=0x0100 (all 8 data bits 0, bit 8 --
    // the stop-bit control -- set) and measures the clk7_en-tick distance
    // between the start bit's falling edge and the following rising edge.
    // Because every data bit is 0, txd stays low for exactly 9 bit periods
    // (the start bit plus the 8 zero data bits) before rising for the stop
    // control bit -- so this distance, divided by 9, is one bit period in
    // clk7_en ticks. SERPER must already be set before calling this.
    int measure_period_ticks() {
        write_reg(A_SERDAT, 0x0100);
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
    // cell's mid-point) starting at the start bit's falling edge.
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
    // for byte b, timed at bit_width_ticks clk7_en ticks per bit, then holds
    // rxd idle-high for a further bit_width_ticks so the RX state machine's
    // own post-stop-bit sampling point (paula_uart.v's RX_SHIFT, which
    // samples and commits rx_data on the tick its counter reaches zero) has
    // definitely passed before this returns. paula/paula_uart expose no
    // top-level rxint port to poll directly (rxint is internal, wired only
    // to paula_intcontroller inside paula.v) -- R3 reads the committed
    // rx_data back through SERDATR itself afterward, which is the
    // conformance-relevant observation anyway.
    void receive_byte(uint8_t b, int bit_width_ticks, int timeout_ticks) {
        std::vector<int> frame;
        frame.push_back(0);
        for (int i = 0; i < 8; ++i) frame.push_back((b >> i) & 1);
        frame.push_back(1);
        size_t idx = 0;
        int in_bit = 0;
        int extra = bit_width_ticks;
        for (int i = 0; i < timeout_ticks; ++i) {
            if (dut->clk7_en) {
                dut->rxd = (idx < frame.size()) ? frame[idx] : 1;
                if (idx < frame.size()) {
                    if (++in_bit >= bit_width_ticks) {
                        in_bit = 0;
                        ++idx;
                    }
                } else if (extra > 0) {
                    --extra;
                } else {
                    break;
                }
            }
            tick();
        }
        dut->rxd = 1;
    }

    Vpaula* dut;
    int clk7_cycle = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= FIXTURE-LIVE-ADKCON (no page) =================
    {
        Harness h;
        h.write_reg(A_ADKCON, SETCLR | 0x0055);
        uint16_t v1 = h.read_reg(A_ADKCONR);
        h.write_reg(A_ADKCON, 0x0055);          // CLR the first word
        h.write_reg(A_ADKCON, SETCLR | 0x00AA);
        uint16_t v2 = h.read_reg(A_ADKCONR);
        checks.note("FIXTURE-LIVE-ADKCON: ADKCON<=SET 0x0055 -> ADKCONR=" + hex16(v1) +
                    "; ADKCON<=SET 0x00AA (after clearing the first) -> ADKCONR=" + hex16(v2) +
                    " -- two distinct, known words both arrive before U1/U3 depend on this path.");
    }

    // ================= FIXTURE-LIVE-SERIAL (no page) =================
    {
        Harness h;
        h.write_reg(A_SERPER, 10);
        int width = h.measure_period_ticks() / 9;
        Harness h55;
        h55.write_reg(A_SERPER, 10);
        h55.write_reg(A_SERDAT, 0x0155);  // data 0x55, bit8=1 (stop)
        auto bits55 = h55.capture_tx(width, 10, 4000);
        Harness haa;
        haa.write_reg(A_SERPER, 10);
        haa.write_reg(A_SERDAT, 0x01AA);  // data 0xAA, bit8=1 (stop)
        auto bitsaa = haa.capture_tx(width, 10, 4000);
        auto vec_to_str = [](const std::vector<int>& v) {
            std::string s;
            for (int b : v) s += std::to_string(b);
            return s;
        };
        checks.note("FIXTURE-LIVE-SERIAL: calibrated bit width " + std::to_string(width) +
                    " clk7_en ticks at SERPER=10; SERDAT=0x0155 (0x55) captured " +
                    vec_to_str(bits55) + ", SERDAT=0x01AA (0xAA) captured " + vec_to_str(bitsaa) +
                    " -- two distinct, correctly start/LSB-first-data/stop-ordered bit "
                    "sequences before R3/R5/R6/R8/R9/U2a/U2b/U4 depend on this path.");
    }

    // ============================================================
    // R1 -- SERDATR is at chip-register offset $018
    // ============================================================
    {
        Harness h;
        uint16_t serdatr_val = h.read_reg(A_SERDATR);
#if MUT_0
        uint16_t serdat_probe = h.read_reg(A_SERDATR);  // MUTATION: probing SERDATR's own address instead of SERDAT's
#else
        uint16_t serdat_probe = h.read_reg(A_SERDAT);
#endif
        uint16_t serper_probe = h.read_reg(A_SERPER);
        checks.note("R1: SERDATR($018/0x0C)=" + hex16(serdatr_val) + "; SERDAT($030/0x18)=" +
                    hex16(serdat_probe) + "; SERPER($032/0x19)=" + hex16(serper_probe) + ".");
        checks.expect(
            serdatr_val == 0x3800 && serdat_probe == 0x0000 && serper_probe == 0x0000,
            "R1-SERDATR-ADD: addressing $018 answers with live register content "
            "(0x3800: OVRUN=0,RBF=0,TBE=1,TSRE=1,RXD=1, out of reset); addressing $030 "
            "or $032 the same way answers 0x0000 -- this fixture decodes $018, uniquely "
            "among these three rows, as a readable address. 'SERDATR      *018   R    "
            "       P    Serial port data and status read' (p.289, ADD cell), composed "
            "with the column heading 'NAME        ADD     R/W   CHIP    FUNCTION' "
            "(p.289). opus:8-regsum.c2, opus:8-regsum.c3.");
    }

    // ============================================================
    // R2 -- SERDATR is read-only
    // ============================================================
    {
        Harness h;
        uint16_t before = h.read_reg(A_SERDATR);
        h.write_reg(A_SERDATR, 0xFFFF);
        uint16_t after1 = h.read_reg(A_SERDATR);
        h.write_reg(A_SERDATR, 0xAAAA);
        uint16_t after2 = h.read_reg(A_SERDATR);
        h.write_reg(A_SERDATR, 0x1234);
#if MUT_1
        uint16_t after3 = h.read_reg(A_SERDAT);  // MUTATION: reading back the wrong address
#else
        uint16_t after3 = h.read_reg(A_SERDATR);
#endif
        checks.note("R2: SERDATR reads " + hex16(before) + ", " + hex16(after1) + ", " +
                    hex16(after2) + ", " + hex16(after3) +
                    " across four presentations differing only in what was written at $018.");
        checks.expect(
            before == 0x3800 && after1 == before && after2 == before && after3 == before,
            "R2-SERDATR-READONLY: $018 answers with live content (the read half); "
            "writing three different values while addressing $018 (0xFFFF, 0xAAAA, "
            "0x1234) changes nothing subsequently read there -- no value written at "
            "this address reaches any state, because $018 matches neither write "
            "decode's own comparison value ($030, $032). 'SERDATR      *018   R' "
            "(p.289, R/W cell), composed with the legend 'W=write-only; R=read-only,' "
            "(p.287, R half). opus:8-regsum.c4.");
    }

    // ============================================================
    // R3 -- SERDATR's function is serial port data and status read
    // ============================================================
    {
        Harness h;
        uint16_t reset_val = h.read_reg(A_SERDATR);
        h.write_reg(A_SERPER, 5);
        h.write_reg(A_SERDAT, 0x0155);  // triggers a transmission
        h.settle_edges(2);
        uint16_t after_write = h.read_reg(A_SERDATR);
#if MUT_2
        h.receive_byte(0x5C, 2 * 5 + 2, 3000);  // MUTATION: a different byte sent...
        uint16_t after_rx = h.read_reg(A_SERDATR);
        bool data_ok = (after_rx & 0x00FF) == 0xA3;  // ...while still comparing against 0xA3
#else
        h.receive_byte(0xA3, 2 * 5 + 2, 3000);
        uint16_t after_rx = h.read_reg(A_SERDATR);
        bool data_ok = (after_rx & 0x00FF) == 0xA3;
#endif
        checks.note("R3: SERDATR before activity=" + hex16(reset_val) + "; right after a SERDAT "
                    "write=" + hex16(after_write) + " (TSRE bit 0x1000 " +
                    std::string((after_write & 0x1000) ? "still set" : "cleared") +
                    "); SERDATR after the receive=" + hex16(after_rx) +
                    " (low byte " + hex16(after_rx & 0x00FF) + ").");
        checks.expect(
            reset_val == 0x3800 && (after_write & 0x1000) == 0 && data_ok,
            "R3-SERDATR-FUNCTION: one address carries both a status component and a "
            "data component. Status: a SERDAT write clears TSRE (bit 12, mask 0x1000) "
            "by the very next SERDATR read. Data: receiving byte 0xA3 over rxd (start, "
            "8 data bits LSB-first, stop, timed at SERPER's own reload) makes the same "
            "byte appear in SERDATR's low 8 bits. 'SERDATR      *018   R           P   "
            " Serial port data and status read' (p.289, FUNCTION cell), composed with "
            "the column heading naming FUNCTION as the fifth column. opus:8-regsum.c6.");
    }

    // ============================================================
    // R4 -- SERDAT is at chip-register offset $030
    // ============================================================
    {
        Harness h;
        uint16_t tsre_before = h.read_reg(A_SERDATR) & 0x1000;
        h.write_reg(A_SERDATR, 0x0155);  // wrong address for a SERDAT write (harmless: 0x0C matches no write decode)
        uint16_t tsre_wrong = h.read_reg(A_SERDATR) & 0x1000;
#if MUT_3
        h.write_reg(A_SERPER, 0x0155);  // MUTATION: the claimed-address write's target changed to SERPER
#else
        h.write_reg(A_SERDAT, 0x0155);  // the claimed address
#endif
        uint16_t tsre_right = h.read_reg(A_SERDATR) & 0x1000;
        checks.note("R4: TSRE (mask 0x1000) before any write=" + hex16(tsre_before) +
                    "; after presenting 0x0155 at $018 (wrong address)=" + hex16(tsre_wrong) +
                    "; after presenting the identical value at the claimed address=" +
                    hex16(tsre_right) + ".");
        checks.expect(
            tsre_before == 0x1000 && tsre_wrong == 0x1000 && tsre_right == 0x0000,
            "R4-SERDAT-ADD: presenting 0x0155 at $018 (SERDATR's own address, not "
            "SERDAT's) leaves TSRE set -- no transmission starts; presenting the "
            "identical value at $030 clears TSRE -- a transmission starts. Only $030 "
            "decodes as SERDAT's write target. 'SERDAT       *030   W           P    "
            "Serial port data and stop bits write' (p.289, ADD cell), composed with "
            "the column heading naming ADD as the address column (p.289). "
            "opus:8-regsum.c8, opus:8-regsum.c9.");
    }

    // ============================================================
    // R5 -- SERDAT is write-only
    // ============================================================
    {
        Harness h;
        h.write_reg(A_SERPER, 10);
        int width = h.measure_period_ticks() / 9;
        Harness h2;
        h2.write_reg(A_SERPER, 10);
        h2.write_reg(A_SERDAT, 0x0155);
        auto bits = h2.capture_tx(width, 10, 4000);
#if MUT_4
        uint16_t readback = h2.read_reg(A_SERDATR);  // MUTATION: reading back the wrong address
#else
        uint16_t readback = h2.read_reg(A_SERDAT);
#endif
        std::vector<int> expected = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
        checks.note("R5: SERDAT=0x0155 transmitted " + std::to_string(bits.size()) +
                    " sampled bit(s); reading $030 back afterward answers " + hex16(readback) + ".");
        checks.expect(
            bits == expected && readback == 0x0000,
            "R5-SERDAT-WRITEONLY: writing 0x0155 to $030 has an observable effect "
            "(transmits start,0x55 LSB-first,stop = 0,1,0,1,0,1,0,1,0,1 on txd); "
            "presenting $030 again with read_reg (no clk7_en edge -- cannot itself "
            "perform a write) answers 0x0000, not the previously written 0x0155 -- "
            "data_o's one nonzero arm is $018, so $030 never answers a read. "
            "'SERDAT       *030   W' (p.289, R/W cell), composed with the legend "
            "'W=write-only; R=read-only,' (p.287, W half). opus:8-regsum.c10.");
    }

    // ============================================================
    // R6 -- SERDAT's function is serial port data and stop bits write
    // ============================================================
    {
        Harness h;
        h.write_reg(A_SERPER, 10);
        int width = h.measure_period_ticks() / 9;
        Harness hs;
        hs.write_reg(A_SERPER, 10);
        hs.write_reg(A_SERDAT, 0x0155);  // bit8=1: 8 data bits + explicit stop
        auto bits_stop = hs.capture_tx(width, 12, 4000);
        Harness hn;
        hn.write_reg(A_SERPER, 10);
#if MUT_5
        hn.write_reg(A_SERDAT, 0x0155);  // MUTATION: collapsed onto hs's own value (no bit-8 difference)
#else
        hn.write_reg(A_SERDAT, 0x0055);  // bit8=0: same low byte, no stop-control bit
#endif
        auto bits_nostop = hn.capture_tx(width, 12, 4000);
        std::vector<int> expect_stop   = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1};
        std::vector<int> expect_nostop = {0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1};
        auto vec_to_str = [](const std::vector<int>& v) {
            std::string s;
            for (int b : v) s += std::to_string(b);
            return s;
        };
        checks.note("R6: SERDAT=0x0155 (bit8=1) samples " + vec_to_str(bits_stop) +
                    "; second SERDAT write samples " + vec_to_str(bits_nostop) + ".");
        checks.expect(
            bits_stop == expect_stop && bits_nostop == expect_nostop && bits_stop != bits_nostop,
            "R6-SERDAT-FUNCTION: SERDAT's written word supplies both a data payload "
            "and stop-bit control in one write. Flipping only bit 8 (0x0155 -> "
            "0x0055), the low-byte data payload (0x55) unchanged, changes the "
            "transmitted frame's own content (diverging at cell 8) -- bit 8 is what "
            "the shift register treats as the frame's own stop control, not part of "
            "the data payload. 'SERDAT       *030   W           P    Serial port "
            "data and stop bits write' (p.289, FUNCTION cell), composed with the "
            "column heading naming FUNCTION as the fifth column. opus:8-regsum.c12.");
    }

    // ============================================================
    // R7 -- SERPER is at chip-register offset $032
    // ============================================================
    {
        Harness h_wrong;
        h_wrong.write_reg(A_SERDATR, 20);  // wrong address for SERPER (harmless)
        int ticks_wrong = h_wrong.measure_period_ticks();
        Harness h_right;
#if MUT_6
        h_right.write_reg(A_SERDATR, 20);  // MUTATION: the claimed-address write's target changed to SERDATR too
#else
        h_right.write_reg(A_SERPER, 20);  // claimed address
#endif
        int ticks_right = h_right.measure_period_ticks();
        checks.note("R7: presenting SERPER=20 at $018 (wrong address) measures " +
                    std::to_string(ticks_wrong) + " ticks over 9 bit periods; at the "
                    "claimed address measures " + std::to_string(ticks_right) + " ticks.");
        checks.expect(
            ticks_wrong > 0 && ticks_right > 0 && ticks_right > ticks_wrong * 5,
            "R7-SERPER-ADD: presenting 20 at $018 (SERDATR's own address, not "
            "SERPER's) leaves the bit period at its unaffected reset-time value; "
            "presenting the identical value at $032 changes it categorically. Only "
            "$032 decodes as SERPER's write target. 'SERPER       *032' (p.289, ADD "
            "cell), composed with the column heading naming ADD as the address "
            "column (p.289). opus:8-regsum.c14, opus:8-regsum.c15.");
    }

    // ============================================================
    // R8 -- SERPER is write-only
    // ============================================================
    {
        Harness h5;
        h5.write_reg(A_SERPER, 5);
        int ticks5 = h5.measure_period_ticks();
        Harness h20;
        h20.write_reg(A_SERPER, 20);
        int ticks20 = h20.measure_period_ticks();
        Harness hro;
        hro.write_reg(A_SERPER, 0x1234);
#if MUT_7
        uint16_t readback = hro.read_reg(A_SERDATR);  // MUTATION: reading back the wrong address
#else
        uint16_t readback = hro.read_reg(A_SERPER);
#endif
        checks.note("R8: SERPER=5 measures " + std::to_string(ticks5) +
                    " ticks; SERPER=20 measures " + std::to_string(ticks20) +
                    " ticks; reading $032 back after writing 0x1234 answers " + hex16(readback) + ".");
        checks.expect(
            ticks20 > ticks5 * 2 && readback == 0x0000,
            "R8-SERPER-WRITEONLY: writing SERPER=5 vs. SERPER=20 changes the "
            "measured bit period -- the write reaches state; presenting $032 again "
            "with read_reg (no clk7_en edge) answers 0x0000, not the previously "
            "written value -- data_o's one nonzero arm is $018, so $032 never "
            "answers a read. 'SERPER       *032   W' (p.289, R/W cell), composed "
            "with the legend 'W=write-only; R=read-only,' (p.287, W half). "
            "opus:8-regsum.c16.");
    }

    // ============================================================
    // R9 -- SERPER's function is serial port period and control
    // ============================================================
    {
        Harness h2v;
        h2v.write_reg(A_SERPER, 2);
        int t2 = h2v.measure_period_ticks();
        Harness h5v;
#if MUT_8
        h5v.write_reg(A_SERPER, 2);  // MUTATION: the second stimulus collapsed onto the first
#else
        h5v.write_reg(A_SERPER, 5);
#endif
        int t5 = h5v.measure_period_ticks();
        checks.note("R9: SERPER=2 measures " + std::to_string(t2) + " ticks over 9 bit "
                    "periods; second SERPER write measures " + std::to_string(t5) + " ticks.");
        checks.expect(
            t2 > 0 && t5 > 0 && t5 > t2,
            "R9-SERPER-FUNCTION: a larger SERPER value produces a measurably longer "
            "bit period -- a dimensionless ratio of clk7_en cycles to clk7_en "
            "cycles, no absolute time unit asserted. 'SERPER       *032   W        "
            "   P    Serial port period and control' (p.289, FUNCTION cell), "
            "composed with the column heading naming FUNCTION as the fifth column. "
            "opus:8-regsum.c18.");
    }

    // ============================================================
    // U1 -- ADKCON bit 11 is named/located as UARTBRK (p.265)
    // ============================================================
    {
        Harness h;
#if MUT_9
        h.write_reg(A_ADKCON, SETCLR | 0x0400);  // MUTATION: bit 10 (WORDSYNC) instead of bit 11
#else
        h.write_reg(A_ADKCON, SETCLR | 0x0800);  // SET bit 11 (UARTBRK)
#endif
        uint16_t set_val = h.read_reg(A_ADKCONR);
        h.write_reg(A_ADKCON, 0x0800);  // CLR bit 11
        uint16_t clr_val = h.read_reg(A_ADKCONR);
        checks.note("U1: ADKCONR after SET=" + hex16(set_val) + ", after CLR bit 11=" + hex16(clr_val) + ".");
        checks.expect(
            (set_val & 0x0800) != 0 && (clr_val & 0x0800) == 0,
            "U1-UARTBRK-LOCATION-P265: ADKCON bit 11, set via the SET/CLR write "
            "convention, reads back set at bit 11 of ADKCONR; clearing it reads "
            "back clear -- pure storage, independent of any txd behaviour. "
            "'11     UARTBRK        Forces a UART break (clears TXD) if true.' "
            "(p.265). opus:8-adkcon.c25, sol:8-adkcon.c22, sol:8-adkcon.c23.");
    }

    // ============================================================
    // U2a -- UARTBRK, true, forces a UART break (p.265)
    // ============================================================
    {
        Harness h;
        bool idle_high = h.dut->txd == 1;
#if MUT_10
        h.write_reg(A_ADKCON, SETCLR | 0x0400);  // MUTATION: bit 10 (WORDSYNC) instead of bit 11
#else
        h.write_reg(A_ADKCON, SETCLR | 0x0800);  // SET bit 11
#endif
        h.settle_edges(4);
        bool forced_low = h.dut->txd == 0;
        h.write_reg(A_ADKCON, 0x0800);  // CLR bit 11
        h.settle_edges(4);
        bool released_high = h.dut->txd == 1;
        checks.note("U2a: txd idle=" + std::string(idle_high ? "1" : "0") +
                    ", after SET bit 11=" + std::string(forced_low ? "0(forced)" : "1(not forced)") +
                    ", after CLR bit 11=" + std::string(released_high ? "1(released)" : "0(stuck)") + ".");
        checks.expect(idle_high && forced_low && released_high,
            "U2A-UARTBRK-FORCES-BREAK-P265: after reset txd idles high; setting "
            "ADKCON bit 11 forces a UART break, observed as txd forced low; "
            "clearing bit 11 releases it back to idle-high. '11     UARTBRK        "
            "Forces a UART break (clears TXD) if true.' (p.265). sol:8-adkcon.c24.");
    }

    // ============================================================
    // U2b -- UARTBRK, true, clears TXD (p.265)
    // ============================================================
    {
        Harness h;
        bool idle_high = h.dut->txd == 1;
#if MUT_11
        h.write_reg(A_ADKCON, SETCLR | 0x0400);  // MUTATION: bit 10 (WORDSYNC) instead of bit 11
#else
        h.write_reg(A_ADKCON, SETCLR | 0x0800);  // SET bit 11
#endif
        h.settle_edges(4);
        bool forced_low = h.dut->txd == 0;
        h.write_reg(A_ADKCON, 0x0800);  // CLR bit 11
        h.settle_edges(4);
        bool released_high = h.dut->txd == 1;
        checks.note("U2b: txd idle=" + std::string(idle_high ? "1" : "0") +
                    ", after SET bit 11=" + std::string(forced_low ? "0(cleared)" : "1(not cleared)") +
                    ", after CLR bit 11=" + std::string(released_high ? "1(released)" : "0(stuck)") + ".");
        checks.expect(idle_high && forced_low && released_high,
            "U2B-UARTBRK-CLEARS-TXD-P265: after reset txd idles high; setting "
            "ADKCON bit 11 clears TXD, observed as txd forced low; clearing bit 11 "
            "releases it back to idle-high. '11     UARTBRK        Forces a UART "
            "break (clears TXD) if true.' (p.265, the parenthetical). "
            "sol:8-adkcon.c25, opus:8-adkcon.c26.");
    }

    // ============================================================
    // U3 -- the UARTBRK field is located at ADKCON bit 11 (p.257)
    // ============================================================
    {
        Harness h;
#if MUT_12
        h.write_reg(A_ADKCON, SETCLR | 0x0400);  // MUTATION: bit 10 (WORDSYNC) instead of bit 11
#else
        h.write_reg(A_ADKCON, SETCLR | 0x0800);  // SET bit 11 (UARTBRK)
#endif
        uint16_t set_val = h.read_reg(A_ADKCONR);
        h.write_reg(A_ADKCON, 0x0800);  // CLR bit 11
        uint16_t clr_val = h.read_reg(A_ADKCONR);
        checks.note("U3: ADKCONR after SET=" + hex16(set_val) + ", after CLR bit 11=" + hex16(clr_val) + ".");
        checks.expect(
            (set_val & 0x0800) != 0 && (clr_val & 0x0800) == 0,
            "U3-UARTBRK-LOCATION-P257: ADKCON bit 11, set via the SET/CLR write "
            "convention, reads back set at bit 11 of ADKCONR; clearing it reads "
            "back clear -- the same bit position U1 established for p.265, named "
            "again independently by p.257's own bit table. '11      UARTBRK  Force "
            "the transmit pin to zero.' (p.257). sol:8-adkcon.c6.");
    }

    // ============================================================
    // U4 -- UARTBRK forces the transmit pin to zero (p.257)
    // ============================================================
    {
        Harness h;
        bool idle_high = h.dut->txd == 1;
#if MUT_13
        h.write_reg(A_ADKCON, SETCLR | 0x0400);  // MUTATION: bit 10 (WORDSYNC) instead of bit 11
#else
        h.write_reg(A_ADKCON, SETCLR | 0x0800);  // SET bit 11
#endif
        h.settle_edges(4);
        bool forced_low = h.dut->txd == 0;
        h.write_reg(A_ADKCON, 0x0800);  // CLR bit 11
        h.settle_edges(4);
        bool released_high = h.dut->txd == 1;
        checks.note("U4: txd idle=" + std::string(idle_high ? "1" : "0") +
                    ", after SET bit 11=" + std::string(forced_low ? "0(forced)" : "1(not forced)") +
                    ", after CLR bit 11=" + std::string(released_high ? "1(released)" : "0(stuck)") + ".");
        checks.expect(idle_high && forced_low && released_high,
            "U4-UARTBRK-FORCES-ZERO-P257: after reset txd idles high; setting "
            "ADKCON bit 11 forces the transmit pin to zero; clearing bit 11 "
            "releases it back to idle-high. '11      UARTBRK  Force the transmit "
            "pin to zero.' (p.257). sol:8-adkcon.c8.");
    }

    // ============================================================
    // ADKCONR row/ADD/R-W/FUNCTION (p.289) -- already graded, slice s2.
    // See SPECIFICATION.md, "Four claims are already graded elsewhere".
    // ============================================================
    checks.record(
        "ADKCONR-ROW [not re-graded]: 'ADKCONR      *010   R           P    Audio, "
        "disk control register read' (p.289, row). Already graded in the accepted "
        "slice-s2 lineage: opus:8-appB-adkconr.c1 ('ADKCONR sits at offset 010'), "
        "graded on the expect() line labelled ADKCONR-ADDRESS-READONLY-AND-CHIP in "
        "hrm/ch5-audio-dmacon-adkcon (test_paula.cpp). This claim's own "
        "classify.evidence reduces 'there is a row' to the same content as the ADD "
        "cell, so it cites the same accepted uid as ADKCONR-ADD below. "
        "opus:8-regsum.c20 declined here. Not re-measured by this package.");
    checks.record(
        "ADKCONR-ADD [not re-graded]: 'ADKCONR      *010   R           P    Audio, "
        "disk control register read' (p.289, ADD cell). Already graded in the "
        "accepted slice-s2 lineage: opus:8-appB-adkconr.c1 ('ADKCONR sits at offset "
        "010'), graded on the expect() line labelled "
        "ADKCONR-ADDRESS-READONLY-AND-CHIP in hrm/ch5-audio-dmacon-adkcon "
        "(test_paula.cpp). opus:8-regsum.c21 declined here, citing that accepted "
        "uid. Not re-measured by this package.");
    checks.record(
        "ADKCONR-READONLY [not re-graded]: 'ADKCONR      *010   R           P    "
        "Audio, disk control register read' (p.289, R/W cell). Already graded in "
        "the accepted slice-s2 lineage: opus:8-appB-adkconr.c3 ('ADKCONR is "
        "read-only'), graded on the same expect() line labelled "
        "ADKCONR-ADDRESS-READONLY-AND-CHIP in hrm/ch5-audio-dmacon-adkcon "
        "(test_paula.cpp). opus:8-regsum.c22 declined here, citing that accepted "
        "uid. Not re-measured by this package.");
    checks.record(
        "ADKCONR-FUNCTION [not re-graded]: 'ADKCONR      *010   R           P    "
        "Audio, disk control register read' (p.289, FUNCTION cell). Already graded "
        "in the accepted slice-s2 lineage: opus:8-appB-adkconr.c5 ('ADKCONR's "
        "function is the audio, disk control register read'), graded on the "
        "expect() line labelled A2-ADKCONR-DISK-READ in "
        "hrm/ch8-adkcon-disk-control-bits (test_ch8-adkcon-disk-control-bits.cpp). "
        "opus:8-regsum.c24 declined here, citing that accepted uid. Not re-measured "
        "by this package.");

    // A SUMMARY THE COVERAGE LEDGER CAN READ.
    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
