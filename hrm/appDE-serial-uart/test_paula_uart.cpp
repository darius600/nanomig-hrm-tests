// Test purpose: grade the 6 claims of appDE-serial-uart (filter
//   classify.package == "appDE-serial-uart" in
//   work/2026-09-17_appDE_packages.jsonl) at the `paula_uart` module
//   boundary. Four claims restate p.326's SERIAL INTERFACE CONNECTOR PIN
//   ASSIGNMENT (J6): NAME=TXD DIR=O and NAME=RXD DIR=I. Two claims restate
//   p.327: the modem control lines (CTS, RTS, DTR, DSR, CD) have no hardware
//   effect on TXD/RXD and are completely asynchronous to them.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.326-327 (appendix E). p.326's NAME/DIR/NOTES table must be read from
//   figures/p326.png -- the text layer displaces the NOTES column by the
//   same one-row-and-growing pattern as p.318, binding "Receive data" to
//   TXD's row and "Clear to send" to RXD's row (appE/corpus-defects.md,
//   "p.326 -- ... TXD and RXD are inverted a SECOND time"). p.327's
//   asynchrony sentence sits in prose, not a table, and reads clean in the
//   text layer -- checked directly against p327.txt for this file, no
//   displacement between "The modem control lines have no hardware affect
//   on" and "and are completely asynchronous to TXD and RXD" (that sentence
//   is NOT among the shifted rows corpus-defects.md lists for p.327, which
//   names only the electrical-characteristics table below it).
// Claims file: work/2026-09-17_appDE_packages.jsonl, filtered
//   classify.package == "appDE-serial-uart":
//     sol:E-rs232.c155        p.326 TXD DIR=O
//     sol:E-rs232.c156        p.326 RXD DIR=I
//     sol:E-rs232.c171        p.327 modem-control asynchrony
//     opus:E-serial-iface.c8  p.326 TXD DIR=O (second reader, same fact)
//     opus:E-serial-iface.c9  p.326 RXD DIR=I (second reader, same fact)
//     opus:E-serial-timing.c6 p.327 modem-control asynchrony (second reader)
//   Accounting (counted directly from this file, not assumed from the
//   brief): 4 uids graded via expect() in 2 assertions (TXD-IS-OUTPUT pairs
//   sol:E-rs232.c155 with opus:E-serial-iface.c8; RXD-IS-INPUT pairs
//   sol:E-rs232.c156 with opus:E-serial-iface.c9 -- each pair states the
//   same proposition from the same render, one per reader). 2 uids declined
//   via record() (sol:E-rs232.c171, opus:E-serial-timing.c6 -- see
//   MODEM-CONTROL-PORTS and the HALT reasoning below). 0 uids unaddressed.
// Non-goals: anything outside the `paula_uart` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// Decline scope checked against every existing package this claim set could
// collide with, by opening each one's own test source and reading its
// expect()/record() labels (not just its classify.note):
//   hrm/ch8-uart-frame-and-status                  -- baseline transmit/receive behaviour
//     via SERDAT/SERPER, never the connector's NAME/DIR/NOTES table.
//   hrm/ch8-serial-interface         -- 106 claims from HRM chapter 8's own
//     prose (pp.255-259), all register-level (SERPER/SERDAT/SERDATR
//     content, timing, status bits). Grepped for "p.326", "p.318", "DIR
//     column", "output", "connector": no match. Its own header comment says
//     outright: "none of them grades the DIR column itself -- that TXD is
//     the OUTPUT."
//   hrm/ch8-appb-serial-register-rows, hrm/ch8-appb-serial-summary-rows --
//     appendix B's SERDAT/SERDATR/SERPER/ADKCON bit tables (UARTBRK forcing
//     TXD low, SERDATR's own RXD status bit). Grepped the same way: no
//     match. A register bit's behaviour is a different axis from a
//     connector pin's direction, not a narrower statement of it -- the same
//     shape the build brief documents for chapter 8 vs. appendix E at the
//     disk connector (p.320).
//   hrm/ch1-cia-timers (test_ciaa.cpp) -- declines p.6's claim that the 8520
//     provides "modem control" as one of six named duties, on the grounds
//     that ciaa/ciab alone expose only generic ports and cannot see
//     minimig.v's wiring. Different proposition (an abstract duty name on
//     p.6, at a different top) from this package's p.327 claim (a specific
//     no-coupling assertion about TXD/RXD) -- not re-graded, not a
//     collision, and its own text says outright that CD/CTS/DSR/RI/DTR/RTS
//     "have no assertion anywhere in this suite" at the time it was
//     written. Confirmed still true for the TXD/RXD-coupling question
//     specifically: grep of hrm/ for cts, rts, dtr, dsr, cd, ri touching
//     paula_uart or txd/rxd, before writing this file, found nothing.
// So neither p.326 claim pair is a decline (a different axis -- connector
// pin direction vs. register content/timing -- not a narrower statement of
// anything already graded), and neither p.327 claim is a decline either
// (new here, not graded anywhere in hrm/).
//
// RTL facts, read fresh from the pinned submodule
// (d6f7df957fad655bc07f6a381e5094a9edff0be0, confirmed via `git submodule
// status` before writing this file) for this package, not copied from the
// claims file's classify.evidence field:
//   - paula_uart.v:4-17's full port list: clk, clk7_en, reset, rga_i,
//     data_i, data_o (output), uartbrk, rbfmirror, txint (output), rxint
//     (output), txd (output), rxd (input). Twelve names; none is
//     cts/rts/dtr/dsr/cd/ri.
//   - paula_uart.v:255 `assign txd = tx_txd;` -- txd is a continuous,
//     unconditional read of an internal register the testbench never
//     writes to.
//   - paula_uart.v's RX_IDLE/RX_START/RX_SHIFT block samples rxd into a
//     2-stage synchroniser (rxd_sync) and one further register (rx_rxd),
//     which is bit 0 of `serdatr` and lands at data_o[11] when SERDATR is
//     addressed (`assign serdatr = {rx_ovrun,rx_rbf,tx_tbe,tx_tsre,
//     rx_rxd};` then `data_o = {serdatr,1'b0,rx_data}` -- serdatr occupies
//     data_o[15:11], rx_rxd is its low bit, so data_o[11]).
//   - paula.v (the module that instantiates paula_uart as `pu1`,
//     paula.v:203-215) carries the same absence: its own port list
//     (paula.v:60-90) names txd/rxd and nothing modem-control at all.
//   - minimig.v:189-197 is the first place cts/rts/dtr/dsr/cd/ri and
//     txd/rxd coexist as names in one module. minimig.v:648-649 wires
//     cts/rts/dtr/dsr/cd/ri exclusively into the ciab instance's
//     porta_in/porta_out (CIA-B port A: `.porta_in({cd,cts,dsr,
//     ri&_joy3[4],1'b1,_joy4[4]})`, `.porta_out({dtr,rts})`), and
//     minimig.v's paula instantiation wires only `.txd(txd)`/`.rxd(rxd)` --
//     an entirely separate pair of nets, sharing no signal with the ciab
//     instantiation anywhere in the file.
//
// MUTATION machine-readable records, one per expect() above it in the file:
// MUTATION TXD-IS-OUTPUT stimulus red the SERDAT write that starts a transmission is skipped, so tx_tbe never clears and txd never leaves its idle-high level on its own MUT_0
// MUTATION RXD-IS-INPUT stimulus red rxd is left at its idle level (1) throughout instead of being driven low, so the SERDATR RXD status bit never reads anything but its reset value MUT_1

#include <cstdint>
#include <iostream>
#include <string>
#include "Vpaula_uart.h"
#include "verilated.h"

#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif

namespace {

constexpr int ADDR_SERDATR = 0x018;
constexpr int ADDR_SERDAT  = 0x030;
constexpr int ADDR_SERPER  = 0x032;
constexpr uint16_t BIT_RXD = 0x0800;  // data_o[11], serdatr's rx_rxd bit

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

// Bus-timing protocol for paula_uart.v, the same one verified for this exact
// fixture by hrm/ch8-uart-frame-and-status (this slice's own calibration reference) and
// reused unchanged by hrm/ch8-serial-interface: posedge-clk registers gated
// by clk7_en, so rga_i/data_i must be held stable across one full
// clk7_en-active cycle for a write to land; data_o is a combinational read
// mux and needs no extra settling beyond that same cycle.
class Harness {
public:
    Harness() : dut(new Vpaula_uart) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->rga_i = 0;
        dut->data_i = 0;
        dut->uartbrk = 0;
        dut->rbfmirror = 0;
        dut->rxd = 1;  // idle high
        for (int i = 0; i < 32; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();
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
        clk7_phase = (clk7_phase + 1) % 4;
        dut->clk7_en = (clk7_phase == 0);
    }

    void settle(int clk7_beats) {
        for (int c = 0; c < clk7_beats; ) {
            tick();
            if (dut->clk7_en) ++c;
        }
    }

    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->rga_i = (addr >> 1) & 0xFF;
        dut->data_i = data;
        tick(); tick(); tick(); tick();
        dut->rga_i = 0;
        dut->data_i = 0;
    }

    uint16_t read_reg(int addr) {
        while (!dut->clk7_en) tick();
        dut->rga_i = (addr >> 1) & 0xFF;
        tick(); tick(); tick(); tick();
        uint16_t val = dut->data_o;
        dut->rga_i = 0;
        return val;
    }

    Vpaula_uart* dut;
    int clk7_phase = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ===============================================================
    // TXD-IS-OUTPUT (p.326, DIR=O): sol:E-rs232.c155, opus:E-serial-iface.c8
    // ===============================================================
    {
        Harness h;
        const bool idle_high = (h.dut->txd == 1);

        // Try to force the pin from the testbench side: write 0 straight
        // into the Verilator-generated field, then step one clk edge.
        // paula_uart.v's `assign txd = tx_txd;` means txd is wired straight
        // from an internal register this testbench never writes -- a bare
        // eval() with no input change is not enough to observe this
        // (Verilator's incremental scheduler can leave a combinationally
        // stale field exactly as the testbench poked it when nothing it
        // tracks has changed, which is a property of the simulator's
        // change-detection, not of the RTL -- measured directly: it reads
        // the poked 0 back with no clk step). Stepping clk forces a real
        // recompute of the reachable combinational cone, and if that
        // recompute restores the DUT's own value, the forced write did not
        // stick -- which is what an OUTPUT means at this boundary. RXD
        // below is exercised the opposite way, for contrast.
        h.dut->txd = 0;
        h.tick();
        const bool override_rejected = (h.dut->txd == 1);

#if MUT_0
        // MUTATION MUT_0: no SERDAT write below -- see the record above.
#else
        h.write_reg(ADDR_SERPER, 10);
        h.write_reg(ADDR_SERDAT, 0x0155);
#endif
        bool saw_txd_low = false;
        for (int i = 0; i < 4000 && !saw_txd_low; ++i) {
            h.tick();
            if (h.dut->clk7_en && h.dut->txd == 0) saw_txd_low = true;
        }

        checks.note("TXD-IS-OUTPUT: idle_high=" + std::to_string(idle_high) +
                    " override_rejected=" + std::to_string(override_rejected) +
                    " self_driven_low_seen=" + std::to_string(saw_txd_low));
        checks.expect(idle_high && override_rejected && saw_txd_low,
            "TXD-IS-OUTPUT: p.326's SERIAL INTERFACE CONNECTOR PIN "
            "ASSIGNMENT (J6) table binds NAME=TXD to DIR=O, an output the "
            "connector receives FROM the Amiga -- and paula_uart's txd is "
            "exactly that: idle high with nothing driven onto it, a forced "
            "external write to the port is discarded by the module's own "
            "unconditional drive rather than sticking, and it goes low on "
            "its own once a transmission starts -- content the module "
            "produces, not content supplied to it [p.326 SERIAL INTERFACE "
            "CONNECTOR PIN ASSIGNMENT (J6), row NAME=TXD DIR=O "
            "NOTES=\"Transmit data\" -- figures/p326.png, read from the "
            "render because the text layer displaces this row's NOTES "
            "entry onto \"Receive data\", the same one-row displacement as "
            "p.318 (appE/corpus-defects.md)] sol:E-rs232.c155 "
            "opus:E-serial-iface.c8");
    }

    // ===============================================================
    // RXD-IS-INPUT (p.326, DIR=I): sol:E-rs232.c156, opus:E-serial-iface.c9
    // ===============================================================
    {
        Harness h;
        h.dut->rxd = 1;
        h.settle(8);
        const uint16_t idle_word = h.read_reg(ADDR_SERDATR);
        const bool idle_high_seen = (idle_word & BIT_RXD) != 0;

#if MUT_1
        // MUTATION MUT_1: rxd stays at 1 -- see the record above.
#else
        h.dut->rxd = 0;
        h.settle(8);
#endif
        const uint16_t low_word = h.read_reg(ADDR_SERDATR);
        const bool low_seen = (low_word & BIT_RXD) == 0;

        h.dut->rxd = 1;
        h.settle(8);
        const uint16_t back_word = h.read_reg(ADDR_SERDATR);
        const bool back_high_seen = (back_word & BIT_RXD) != 0;

        checks.note("RXD-IS-INPUT: idle_high_seen=" + std::to_string(idle_high_seen) +
                    " low_seen=" + std::to_string(low_seen) +
                    " back_high_seen=" + std::to_string(back_high_seen));
        checks.expect(idle_high_seen && low_seen && back_high_seen,
            "RXD-IS-INPUT: p.326's SERIAL INTERFACE CONNECTOR PIN "
            "ASSIGNMENT (J6) table binds NAME=RXD to DIR=I, an input the "
            "Amiga receives FROM the connector -- and paula_uart's rxd is "
            "exactly that: SERDATR's own RXD status bit (data_o[11]) "
            "tracks whatever level this testbench imposes on the rxd port "
            "-- high, then low, then high again -- each transition "
            "reaching the readable register a few clk7_en beats after the "
            "pin changes, content supplied INTO the module, the mirror "
            "image of TXD-IS-OUTPUT above [p.326 SERIAL INTERFACE "
            "CONNECTOR PIN ASSIGNMENT (J6), row NAME=RXD DIR=I "
            "NOTES=\"Receive data\" -- figures/p326.png, read from the "
            "render because the text layer displaces this row's NOTES "
            "entry onto \"Clear to send\" (appE/corpus-defects.md)] "
            "sol:E-rs232.c156 opus:E-serial-iface.c9");
    }

    // ===============================================================
    // p.327: modem-control lines vs. TXD/RXD -- MODEM-CONTROL-PORTS,
    // HALT: sol:E-rs232.c171, opus:E-serial-timing.c6
    // ===============================================================
    {
        checks.note(
            "MODEM-CONTROL-PORTS: paula_uart.v's full port list "
            "(paula_uart.v:4-17) is clk, clk7_en, reset, rga_i, data_i, "
            "data_o, uartbrk, rbfmirror, txint, rxint, txd, rxd -- twelve "
            "names, none of them cts, rts, dtr, dsr, cd or ri. paula.v "
            "(paula_uart's own instantiator, as pu1) carries the same "
            "absence in its own port list. The six modem-control names "
            "first appear together with txd/rxd only at minimig.v, which "
            "wires cts/rts/dtr/dsr/cd/ri exclusively into the ciab "
            "instance's porta_in/porta_out (CIA-B port A) and wires "
            "paula's txd/rxd as an entirely separate pair of nets, sharing "
            "no wire with the ciab instantiation anywhere in the file.");
        checks.record(
            "sol:E-rs232.c171, opus:E-serial-timing.c6 (p.327, quote \"The "
            "modem control lines have no hardware affect on and are "
            "completely asynchronous to TXD and RXD.\"): DECLINED at this "
            "package's declared top (paula_uart alone). MODEM-CONTROL-"
            "PORTS above shows paula_uart's port list carries no wire for "
            "any modem-control signal at all, so there is no stimulus at "
            "this boundary that could even in principle be driven to test "
            "the claim -- an expect() here would grade the absence of a "
            "port, not the absence of a hardware effect: it would read "
            "PASS against a hypothetical mutant that added a real coupling "
            "wire just as readily as against the baseline, because the "
            "assertion would never touch RTL capable of carrying one. That "
            "is a vacuous truth, not a measurement, and the build brief's "
            "own guidance is to say so rather than grade it. The RTL fact "
            "that actually backs the manual's claim sits one level up, at "
            "minimig.v, and is confirmed there by MODEM-CONTROL-PORTS: "
            "CTS/RTS/DTR/DSR/CD/RI terminate at CIA-B port A and TXD/RXD "
            "terminate at paula_uart, and the two groups share no wire "
            "anywhere minimig.v instantiates both -- so the coupling the "
            "manual denies does not exist to be exercised, and a live "
            "simulation of that fact would need a fixture at least as wide "
            "as minimig (or a ciab+paula_uart harness wired the way "
            "minimig.v wires them), not this package's declared "
            "paula_uart-alone top; adding a port to paula_uart to make the "
            "claim driveable would be changing the RTL under test, which "
            "the build brief separately rules out. HALT: not constructible "
            "at this top, not graded here. Not graded anywhere else in "
            "hrm/ either: a grep of hrm/ for cts, rts, dtr, dsr, cd or ri "
            "touching paula_uart or txd/rxd, run before writing this file, "
            "found nothing. hrm/ch1-cia-timers declines a different "
            "proposition at a different top -- p.6's abstract naming of "
            "\"modem control\" as one of the 8520's six duties, declined "
            "there because ciaa/ciab alone cannot see minimig.v's wiring "
            "-- and its own text says CD/CTS/DSR/RI/DTR/RTS \"have no "
            "assertion anywhere in this suite\"; that record does not "
            "reach this page's specific TXD/RXD no-coupling claim and is "
            "not re-graded or re-declined against here, only cited for "
            "orientation.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
