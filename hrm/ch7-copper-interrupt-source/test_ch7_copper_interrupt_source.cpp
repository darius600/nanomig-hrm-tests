// Test purpose: HRM conformance for ch7-copper-interrupt-source -- whether
// the Copper's own interrupt condition sets bit 4 (COPER) of
// paula_intcontroller's interrupt-request register, and only bit 4, when it
// travels the Copper's real register-write path (agnus_copper ->
// paula_intcontroller); and whether that same condition, carried through
// the same real path, reaches the CPU as 68000 hardware level 3 (_ipl's
// active-low encoding of it), per SPECIFICATION.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: the manual quotations cited inline (sol:7-interrupts.c41,
// printed p.219; opus:7-fig7-4.c15, printed p.221;
// opus:ch2-the-copper-and-the-680x0.c3, printed p.35), not the RTL. A red
// result is a valid, finished outcome.
// Non-goals: whole-system exclusivity of bit 4 to the Copper, the convention
// half of "reserved for identifying" (not a runtime behaviour), and the Exec
// software-priority-5 half of opus:7-fig7-4.c15 -- all out of this
// package's two claims; see SPECIFICATION.md "Not turned into assertions."
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// The residual (see paula_intcontroller.v:114, 'intreq[4] <= tmp[4];' -- the
// generic SET/CLR mask and nothing else): the hardware cannot distinguish a
// Copper write to INTREQ from a CPU write to the same address; the register
// bus multiplexes every DMA channel onto one reg_address_out (agnus.v's
// channel-priority mux), and the real 8364 has no bus-master-identity
// discriminator either. The manual's verb is "reserved", a convention about
// who uses the bit, not a hardware property. So a green on P1 below says the
// Copper's own interrupt condition reaches bit 4 when it travels the
// Copper's real write path -- it does NOT say that bit 4 identifies the
// Copper as opposed to some other writer. Those are different sentences;
// do not conflate them when reading this file's result.
//
// Fixture: rig_copper_intreq.v (test-only glue in this package directory,
// never nanomig/) instantiates the unmodified agnus_copper and
// paula_intcontroller RTL and forwards the Copper's real reg_address_out and
// the shared chip-data-bus word to the interrupt controller only at the
// cycle the Copper is actually granted the bus -- see that file's header.
//
// MUTATION P1 stimulus red: word2 of the MOVE to INTREQ changed from 0x8010
// to 0x8000 (SET-nothing), expected value left unchanged. P1 went red (bit 4
// stayed clear, before=after=0x0000). Reverted, rebuilt, back to green.
// MUTATION CONTROL stimulus red: the control's readback moved from before
// the Copper's writes to after them, expected value left unchanged. CONTROL
// went red (bit 4 read 1). Reverted, rebuilt, back to green.
// MUTATION P2 stimulus red: word2 of the MOVE to INTENA changed from 0xC010
// to 0xC000 (drops intena[4], COPER's own enable bit; intena[14], master
// enable, stays set), expected value left unchanged. P1 and both CONTROLs
// stayed green (intreq[4] itself, and the pre-stimulus idle read, do not
// depend on intena); P2 alone went red (_ipl read 7, the idle value,
// instead of 4 -- bit 4 was still set in INTREQR but never reached the
// priority encoder because it was masked off). 3 passed, 1 failed. Reverted,
// rebuilt, back to green (4 passed, 0 failed).
//
// opus:ch2-the-copper-and-the-680x0.c3 (p.35, "use the Copper MOVE
// instruction to store a 1 into the following bits of INTREQ" to interrupt
// the 680x0): this claim's two halves -- the MOVE-stores-a-1-into-INTREQ
// half, and the resulting interrupt reaching the 680x0 -- are exactly what
// P1 and P2 already measure on the same stimulus, so the id is added to
// both existing labels rather than a new assertion; no new proposition,
// control or mutation. See SPECIFICATION.md's "opus:ch2-the-copper-and-
// the-680x0.c3" section for the full reading.

#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vrig_copper_intreq.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

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

// register addresses, as [8:1] -- see paula_intcontroller.v:27-30
static const uint8_t INTENAR_ADDR = 0x0E; // 9'h01c[8:1]
static const uint8_t INTREQR_ADDR = 0x0F; // 9'h01e[8:1]
static const uint8_t INTENA_ADDR  = 0x4D; // 9'h09a[8:1]
static const uint8_t INTREQ_ADDR  = 0x4E; // 9'h09c[8:1]
static const uint8_t IDLE_ADDR    = 0xFF; // agnus_copper.v's own "no register selected" value

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    Vrig_copper_intreq* top = new Vrig_copper_intreq;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("trace.vcd");
    int t = 0;
    Checks checks;

    top->clk = 0;
    top->clk7_en = 1;
    top->reset = 1;
    top->ecs = 0;
    top->ackdma = 0;
    top->enadma = 0;
    top->sof = 0;
    top->blit_busy = 1; // keep skip_flag from ever being asserted; this fixture issues no WAIT/SKIP
    top->vpos = 0;
    top->hpos = 5;
    top->chip_data = 0;
    top->paula_reg_address_in = IDLE_ADDR;

    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval(); if (tfp) tfp->dump(t++);
        top->clk = 0; top->eval(); if (tfp) tfp->dump(t++);
        top->hpos++;
        if (top->hpos > 227) top->hpos = 0;
        top->eval();
    };

    auto wait_reqdma = [&](int timeout = 80) {
        while (top->reqdma == 0 && timeout > 0) { tick(); timeout--; }
        return timeout > 0;
    };

    // Deliver one instruction word: wait for the Copper to request the bus,
    // grant it for exactly one cycle while presenting `word` on the shared
    // chip data bus, and forward the Copper's own reg_address_out to
    // paula_intcontroller for that same cycle -- the real-boundary path this
    // package's fixture exists to exercise, not a shortcut around it.
    auto provide_word = [&](uint16_t word) -> bool {
        if (!wait_reqdma()) { std::cout << "FAIL: timeout waiting for reqdma\n"; return false; }
        top->ackdma = 1;
        top->chip_data = word;
        top->paula_reg_address_in = top->copper_reg_address_out;
        top->eval();
        tick();
        top->ackdma = 0;
        top->chip_data = 0;
        top->paula_reg_address_in = IDLE_ADDR;
        top->eval();
        return true;
    };

    // Pure combinational readback of INTREQR: no clock edge, no side effect
    // on the Copper's state. data_out is {1'b0, intreq[14:0]} at this
    // address (paula_intcontroller.v:73-80).
    auto read_intreqr = [&]() -> uint16_t {
        uint8_t saved = top->paula_reg_address_in;
        top->paula_reg_address_in = INTREQR_ADDR;
        top->eval();
        uint16_t v = top->paula_data_out;
        top->paula_reg_address_in = saved;
        top->eval();
        return v;
    };

    // --- bring up the fixture ---
    for (int i = 0; i < 10; i++) tick();
    top->reset = 0;

    // pass the RESET state's dummy DMA cycle (agnus_copper.v's RESET case)
    if (!wait_reqdma()) { std::cout << "FAIL: timeout leaving RESET\n"; top->final(); return 1; }
    top->ackdma = 1;
    top->eval();
    tick();
    top->ackdma = 0;
    top->eval();

    // --- CONTROL: absent any Copper write, COPER reads clear ---
    uint16_t before = read_intreqr();
    checks.expect((before & 0x0010) == 0,
                  "CONTROL (fixture-liveness baseline, same measurement basis as P1): "
                  "before any Copper write, INTREQR bit 4 (COPER) reads 0");

    // MOVE INTENA,#$C010 -- sets the master-enable bit and COPER's own enable
    // bit, so the CPU interrupt-priority-encoder value from the step-3
    // attack-pass measurement can be reproduced below. This write does not
    // touch INTREQ; P1 does not depend on it.
    provide_word(static_cast<uint16_t>(INTENA_ADDR) << 1); // word1: INTENA's own address, MOVE-encoded
    provide_word(0xC010);                                  // word2: SET intena[14] and intena[4]

    // CONTROL for P2, same measurement basis (_ipl) as P2 below: COPER is
    // now enabled but its interrupt condition has not yet been presented,
    // so no source is pending and the priority encoder reports the idle
    // value 7 (paula_intcontroller.v's all-zero/default casez branch).
    // Same one-cycle settle as P2 needs (see the comment at P2's tick()).
    tick();
    int ipl_idle = top->_ipl;
    char ipl_idle_str[4];
    snprintf(ipl_idle_str, sizeof ipl_idle_str, "%d", ipl_idle);
    checks.expect(ipl_idle == 7,
        "CONTROL for P2 (same measurement basis as P2): with COPER's own "
        "enable bit set but its interrupt condition not yet presented, "
        "_ipl=" + std::string(ipl_idle_str) + " (idle, no source pending) -- expected 7");

    // MOVE INTREQ,#$8010 -- the claim's actual stimulus: the Copper's own
    // interrupt condition (SPECIFICATION.md P1), travelling the Copper's
    // real register-write path into paula_intcontroller.
    provide_word(static_cast<uint16_t>(INTREQ_ADDR) << 1); // word1: INTREQ's own address, MOVE-encoded
    provide_word(0x8010);                                  // word2: SET bit 4 (COPER)

    uint16_t after = read_intreqr();

    char before_hex[8], after_hex[8];
    snprintf(before_hex, sizeof before_hex, "%04x", before);
    snprintf(after_hex, sizeof after_hex, "%04x", after);

    bool bit4_transitioned    = ((before & 0x0010) == 0) && ((after & 0x0010) != 0);
    bool other_bits_unchanged = ((before & static_cast<uint16_t>(~0x0010)) ==
                                  (after  & static_cast<uint16_t>(~0x0010)));

    checks.expect(bit4_transitioned && other_bits_unchanged,
        "P1 (sol:7-interrupts.c41 p.219, opus:ch2-the-copper-and-the-680x0.c3, "
        "opus:7-fig7-4.c15 p.221): a Copper MOVE "
        "asserting COPER travelling the Copper's real register-write path into "
        "paula_intcontroller sets bit 4, and only bit 4, of INTREQR -- before=0x"
        + std::string(before_hex) + " after=0x" + std::string(after_hex));

    // Let the pipelined priority encoder settle one more cycle before
    // sampling _ipl (paula_intcontroller.v:150-172 reads intreqena, itself
    // derived combinationally from the same-edge intreq/intena update, so
    // its own posedge lags that update by one cycle).
    tick();
    int ipl = top->_ipl;
    char ipl_str[4];
    snprintf(ipl_str, sizeof ipl_str, "%d", ipl);
    checks.expect(ipl == 4,
        "P2 (opus:ch2-the-copper-and-the-680x0.c3, opus:7-fig7-4.c15 p.221, hardware-priority half -- pp.219+221 "
        "together give the same numeral: p.219 states plainly 'Bit 4, COPER, "
        "is used by the Copper to issue a level 3 interrupt', and Figure 7-4 "
        "places COPER at hardware priority 3; that p.219 sentence's own claim "
        "UID, 7-interrupts.c40, belongs to a different package and is not "
        "cited here -- only opus:7-fig7-4.c15, already this package's claim, "
        "is): the same Copper MOVE that sets bit 4 above also reaches the CPU "
        "as 68000 hardware level 3 -- _ipl=" + std::string(ipl_str) +
        " is the active-low encoding of level 3 (paula_intcontroller.v's "
        "priority encoder: intreqena[4] alone drives _ipl<=4) -- expected 4");

    // --- claims recorded, not asserted ---
    checks.record(
        "(half graded: sol:7-interrupts.c41) (p.219), convention half: a set COPER bit *meaning* "
        "\"the Copper did it\" to a programmer is not a runtime behaviour; no "
        "stimulus/observation pair grades it");
    checks.record(
        "(half graded: sol:7-interrupts.c41) (p.219), whole-system-exclusivity half: this fixture "
        "is exactly agnus_copper + paula_intcontroller; showing no other source "
        "anywhere in the full chip can ever set bit 4 needs every other candidate "
        "source's RTL, a wider boundary than assigned -- matches "
        "paula_intcontroller.v:114, 'intreq[4] <= tmp[4];' (the generic SET/CLR "
        "mask and nothing else: bit 4 has no dedicated wire of its own, so "
        "nothing in this fixture distinguishes a Copper write from a CPU write "
        "to the same address)");
    checks.record(
        "(half graded: opus:7-fig7-4.c15) (p.221), Exec-software-priority half only: Figure 7-4's "
        "row also carries Exec software priority 5 for COPER, exec.library's "
        "server-chain numbering -- a distinct proposition from both naming (P1) "
        "and the hardware-priority-3 value that reaches the CPU (P2, now "
        "asserted above), out of this package's two claims");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";

    top->final();
    tfp->close();
    delete top;
    delete tfp;
    return checks.failed == 0 ? 0 : 1;
}
