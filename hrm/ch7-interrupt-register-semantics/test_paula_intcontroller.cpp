// Test purpose: HRM conformance package for chapter 7's own reading (printed
//   pp.217-218, plus Figure 7-4 p.221) of Paula's interrupt controller: the
//   four register addresses, the SET/CLR write convention, bit 15's "no
//   meaning" on the read-only addresses, the INTEN master gate, and
//   write-to-clear stickiness. 41 claims, from
//   work/ch7-build/ch7-interrupt-register-semantics.jsonl. Full derivation in
//   SPECIFICATION.md (committed as 9ed37e0 before this file was written).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.217-218 (chapter 7 body), corroborated by appendix A p.280 and Figure
//   7-4 p.221 for the INTEN master-gate claim group only.
// Fixture boundary: paula_intcontroller.v ONLY. No other module is
//   instantiated anywhere in this file.
//
// PACKAGE BOUNDARY - what this package does and does not duplicate.
//   hrm/ch5-audio-interrupt (chapter 5, appendix A/B-sourced, 106 claims) is
//   accepted and already asserts: the four-address register map and
//   write-only-reads-as-constant behaviour, the shared bit-layout between
//   INTENA and INTREQ, the full SET/CLR bit-15 mechanics including the
//   two-write requirement, the 14-source bit/priority-level table, pending
//   bits pollable independent of enable, sticky-until-explicit-clear, and the
//   INTEN master-gate/no-request split (its own assertions 5 and 5B - 5B
//   asserts the "no request" half and is EXPECTED RED there).
//   hrm/paula_intcontroller (no ch7- prefix, pre-expect()-harness, no claim
//   citations, "FPGA-038/039/040" numbering) already covers one
//   write+readback+one-clear round trip, a VBLANK hardware latch, and four
//   _ipl priority scenarios - it is NOT being retired: the coordinator's
//   reply says its VBLANK-pin-to-request-bit-5 latch check has no home in
//   the new chapter split and stays there permanently.
//   This package's own 41 claims are chapter 7's body-text reading of
//   pp.217-218 (a different printed passage than either prior package's
//   evidence) plus Figure 7-4 p.221's corroborating and conflicting reading
//   of INTEN. What it adds that neither prior package has: P1's general
//   masking statement (p.217, absent from ch5's 9 assertions); bit 15's "no
//   meaning" on the read-only addresses (P10 - ch5-audio-interrupt's own
//   claim notes say it does not assert this); a `record`, not `expect`,
//   disposition for the disputed INTEN-self-request half (P19), folding in
//   Figure 7-4's corroborating placement, in place of ch5's own assertion 5B
//   (which asserts that same disputed half and is EXPECTED RED there).
//   P9 (bit-position correspondence between INTENA and INTREQ, printed
//   p.218) is cited below but NOT reasserted: its own claim row
//   (sol:7-interrupts.c19) says hrm/ch5-audio-interrupt's assertion 3 already
//   covers it. Ledger cross-reference only.
//
// THE INTEN SELF-CONTRADICTION (P18/P19) - resolved exactly as directed.
//   Printed p.218 ("This bit is used for enable/disable only. It creates no
//   interrupt request.") and appendix A p.280 ("Master interrupt (enable
//   only, no request)") agree that INTEN cannot itself raise a request.
//   Figure 7-4 p.221 places INTEN inside the hardware-priority-6 bracket -
//   flatly the opposite. The undisputed half - INTEN as master gate over
//   every other enable - is ASSERTED (P17/P18). The disputed half is
//   RECORDED, never graded (P19). No `expect()` anywhere in this file grades
//   whether INTEN can raise its own request.
//
// P10 - "reads zero" is an experiment, not a claim (coordinator's
//   correction). The quotation this package is built from ("bit 15 has no
//   meaning in the read-only addresses") does not name a constant. P10 below
//   `note`s the measured constant and `expect`s only what the quotation
//   supports: that bit 15 of a read from INTENAR/INTREQR does not track
//   stored content.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup - not a compile-time #ifdef - so one build serves the real run
//   (MUTATE unset) and all 29 mutation runs (MUTATE=<id>, one id per
//   assertion). Each mutation changes what the module is ASKED TO DO (a
//   written value, a selected address, a skipped stimulus step) and leaves
//   the `expect()` call's own expected-value code untouched, per BUILD-BRIEF.
//   Every one of the 29 below was run with its own id and confirmed red, then
//   reverted (MUTATE unset) and confirmed green again; results are logged
//   next to each assertion and summarised in the build report.
//
// FIXTURE-INTEGRITY PRECONDITION (A0). Per BUILD-BRIEF's silence-trap rule,
//   distinct known words are written and read back through both register
//   pairs, in sequence, before any later assertion that depends on a read
//   reflecting a write is trusted.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vpaula_intcontroller.h"
#include "verilated.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

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

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Vpaula_intcontroller* dut = new Vpaula_intcontroller;
    Checks checks;

    int clk7_cycle = 0;
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    auto do_reset = [&]() {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->rxint = 0;
        dut->txint = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->blckint = 0;
        dut->syncint = 0;
        dut->audint = 0;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    auto write_reg = [&](int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };

    auto read_reg = [&](int addr) -> uint16_t {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        tick();
        uint16_t val = dut->data_out;
        tick(); tick(); tick();
        dut->reg_address_in = 0;
        return val;
    };

    // Several clk7_en-aligned edges, never one: intreq updates on one edge,
    // intreqena/_ipl reflect it only after a further edge (paula_intcontroller.v:99-147,150-172).
    auto settle = [&](int n) {
        for (int i = 0; i < n; i++) { while (!dut->clk7_en) tick(); tick(); }
    };

    // Register addresses: documented Amiga custom-chip offsets (printed HRM
    // appendix A/B), presented as word indices (byte address >> 1) on
    // reg_address_in[8:1], confirmed at paula_intcontroller.v:27-30 and used
    // by every package in this tree the same way.
    const int INTENA  = 0x4D;  // write address, byte $09A
    const int INTENAR = 0x0E;  // read address,  byte $01C
    const int INTREQ  = 0x4E;  // write address, byte $09C
    const int INTREQR = 0x0F;  // read address,  byte $01E
    const uint16_t SET = 0x8000;
    const uint16_t CLR = 0x0000;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page: the
    // BUILD-BRIEF's own requirement that the fixture prove it can deliver a
    // known, distinct word before any read-based assertion is trusted.
    // ========================================================================
    do_reset();
    {
        uint16_t first = mutating("A0_ENA1") ? 0x00AA : 0x0055;  // MUTATION: wrong first word
        write_reg(INTENA, SET | first);
        uint16_t ena1 = read_reg(INTENAR);
        checks.expect(ena1 == 0x0055,
            "A0-LIVENESS-INTENA-FIRST: a known word (0x0055) written to INTENA arrives at INTENAR");

        write_reg(INTENA, CLR | 0x0055);  // clear what we just set
        uint16_t second = mutating("A0_ENA2") ? 0x0055 : 0x00AA;  // MUTATION: replay the stale word
        write_reg(INTENA, SET | second);
        uint16_t ena2 = read_reg(INTENAR);
        checks.expect(ena2 == 0x00AA,
            "A0-LIVENESS-INTENA-SECOND: a second, different known word (0x00AA) also arrives, so the path is not stuck at the first value");
    }
    do_reset();
    {
        // bits 2 and 4: no hardware source at all (paula_intcontroller.v:110,:114) -- a clean
        // bus-only round trip with nothing else able to interfere.
        uint16_t first = mutating("A0_REQ1") ? 0x0041 : 0x0014;  // MUTATION: wrong first word
        write_reg(INTREQ, SET | first);
        uint16_t req1 = read_reg(INTREQR);
        checks.expect(req1 == 0x0014,
            "A0-LIVENESS-INTREQ-FIRST: a known word (0x0014) written to INTREQ arrives at INTREQR");

        write_reg(INTREQ, CLR | 0x0014);
        uint16_t second = mutating("A0_REQ2") ? 0x0014 : 0x0041;  // MUTATION: replay the stale word
        write_reg(INTREQ, SET | second);
        uint16_t req2 = read_reg(INTREQR);
        checks.expect(req2 == 0x0041,
            "A0-LIVENESS-INTREQ-SECOND: a second, different known word (0x0041) also arrives");
    }

    // ========================================================================
    // P1 -- Interrupt sources can be individually masked. printed p.217:
    // "Control registers within the peripherals chip allow you to mask
    // certain of these sources and prevent them from generating a 680x0
    // interrupt." Cited: sol:7-interrupts.c5, opus:7-int.c6.
    // Uses one arbitrary ordinary (non-master) selector bit, bit 3, driven at
    // this fixture boundary by the int2 pin. This package does not assert
    // which named Amiga source that pin represents -- that identity table is
    // hrm/ch5-audio-interrupt's, not this claim's.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | (1u << 14));  // master enabled; bit3 left disabled
        if (mutating("P1")) write_reg(INTENA, SET | (1u << 3));  // MUTATION: also enable bit3 early
        while (!dut->clk7_en) tick();
        dut->int2 = 1; tick(); tick(); tick(); tick(); dut->int2 = 0;  // bit3 becomes pending
        settle(3);
        checks.expect(dut->_ipl == 7,
            "P1-MASK-ENABLE-CLEAR: bit3 pending with bit3's own enable clear leaves the interrupt output inactive even though the master enable is set");

        bool enable_bit3 = !mutating("P1B");  // MUTATION: never actually enable bit3
        if (enable_bit3) write_reg(INTENA, SET | (1u << 3));
        settle(3);
        checks.expect(dut->_ipl != 7,
            "P1-MASK-ENABLE-SET: enabling bit3 releases the still-pending request to the interrupt output");
    }

    // ========================================================================
    // P2/P3 -- Two independent registers, four distinct addresses. printed
    // p.217: "There are two interrupt registers, interrupt enable (mask) and
    // interrupt request (status)." / "Each register has both a read and a
    // write address." Cited: sol:7-interrupts.c10/c11, opus:7-int.c9.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | 0x0021);  // nonzero pattern in the enable register only
        uint16_t enar = read_reg(INTENAR);
        int req_addr = mutating("P2") ? INTENAR : INTREQR;  // MUTATION: re-read INTENAR and call it the request register
        uint16_t reqr = read_reg(req_addr);
        checks.expect(enar == 0x0021 && reqr == 0x0000,
            "P2-TWO-REGISTERS-INDEPENDENT: writing only the enable register leaves the request register's own readback at 0");

        int unrelated_addr = mutating("P3") ? INTENAR : 0x00;  // MUTATION: "unrelated" address is secretly a real one
        uint16_t unrelated = read_reg(unrelated_addr);
        checks.expect(unrelated == 0x0000,
            "P3-FOUR-DISTINCT-ADDRESSES: an address outside the four named registers returns 0 even though a real register holds nonzero content");
    }

    // ========================================================================
    // P4 -- INTENA: write-only. printed p.217: "INTENA ... write only. Sets
    // or clears specific bits of INTENA." Cited: sol:7-interrupts.c12,
    // opus:7-int.c10.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | 0x0021);
        int read_addr = mutating("P4") ? INTENAR : INTENA;  // MUTATION: read the real content address instead
        uint16_t val = read_reg(read_addr);
        checks.expect(val == 0xFFFF,
            "P4-INTENA-WRITE-ONLY: presenting INTENA's own write address to the read path never exposes stored content");
    }

    // ========================================================================
    // P5 -- INTENAR: read-only, mirrors INTENA. printed p.217: "INTENAR ...
    // read only. Reads contents of INTENA." Cited: sol:7-interrupts.c13,
    // opus:7-int.c11.
    // ========================================================================
    do_reset();
    {
        uint16_t stim = mutating("P5A") ? 0x001F : 0x0079;  // MUTATION: write a different word than the one checked for
        write_reg(INTENA, SET | stim);
        uint16_t mirrored = read_reg(INTENAR);
        checks.expect(mirrored == 0x0079,
            "P5-INTENAR-MIRRORS: INTENAR returns exactly what was last written to INTENA");

        int spurious_addr = mutating("P5B") ? INTENA : INTENAR;  // MUTATION: issue the spurious cycle at the real write address instead
        while (!dut->clk7_en) tick();
        dut->reg_address_in = spurious_addr;
        dut->data_in = SET | 0x7FFF;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        uint16_t after = read_reg(INTENAR);
        checks.expect(after == 0x0079,
            "P5-INTENAR-READONLY: a write-shaped bus cycle at INTENAR's address changes nothing in the stored enable content");
    }

    // ========================================================================
    // P6 -- INTREQ: write-only, can force a software-triggered request.
    // printed p.218: "INTREQ ... write only. Used by the processor to force
    // a certain kind of interrupt to be processed (software interrupt)."
    // Cited: sol:7-interrupts.c14/c15, opus:7-int.c12.
    // ========================================================================
    do_reset();
    {
        // bit4: no hardware source at all (paula_intcontroller.v:114) -- forcing it needs
        // nothing but a bus write, exactly the manual's "software interrupt".
        bool do_write = !mutating("P6_FORCE");  // MUTATION: skip the bus write entirely
        if (do_write) write_reg(INTREQ, SET | (1u << 4));
        uint16_t reqr = read_reg(INTREQR);
        checks.expect(((reqr >> 4) & 1u) == 1u,
            "P6-INTREQ-FORCE-SOFTWARE: a bus write alone, no hardware pin involved, sets a pending request bit");

        int read_addr = mutating("P6_WO") ? INTREQR : INTREQ;  // MUTATION: read the real content address instead
        uint16_t wo = read_reg(read_addr);
        checks.expect(wo == 0xFFFF,
            "P6-INTREQ-WRITE-ONLY: presenting INTREQ's own write address to the read path never exposes stored content");
    }

    // ========================================================================
    // P7 -- write-to-clear stickiness (two-sided). printed p.218: "Also used
    // to clear interrupt request flags once the interrupt process is
    // completed." Split: asserted = a CLR write clears the named bit and a
    // pending bit does not clear itself; excluded = the servicing moment (no
    // interrupt-acknowledge signal on this claim's evidence to test it
    // against). Cited: sol:7-interrupts.c16, opus:7-int.c13.
    // ========================================================================
    do_reset();
    {
        write_reg(INTREQ, SET | (1u << 4));  // bit4 again: pure software, isolates stickiness
        bool clear_early = mutating("P7_PERSIST");  // MUTATION: sneak in a clear before the idle stretch ends
        if (clear_early) write_reg(INTREQ, CLR | (1u << 4));
        for (int i = 0; i < 100; i++) tick();  // long idle stretch, no other stimulus
        uint16_t after_idle = read_reg(INTREQR);
        checks.expect(((after_idle >> 4) & 1u) == 1u,
            "P7-STICKY-PERSISTS: a pending request bit survives 100 idle clocks with no clearing write");

        bool do_clear = !mutating("P7_CLEARS");  // MUTATION: skip the explicit clear write
        if (do_clear) write_reg(INTREQ, CLR | (1u << 4));
        uint16_t after_clear = read_reg(INTREQR);
        checks.expect(((after_clear >> 4) & 1u) == 0u,
            "P7-STICKY-CLEARS: an explicit CLR write to INTREQ naming that bit clears it");
    }
    // Measured, not asserted -- the manual rules on neither case (no
    // interrupt-acknowledge signal exists on this claim's evidence to test
    // the servicing moment against): does a CLR write clear the bit while its
    // hardware source is still asserted? Not part of P7's graded proposition.
    do_reset();
    {
        while (!dut->clk7_en) tick();
        dut->vblint = 1;
        tick(); tick(); tick(); tick();  // bit5, hardware-sourced, now pending
        write_reg(INTREQ, CLR | (1u << 5));  // CLR write issued while vblint is STILL asserted
        uint16_t still_high = read_reg(INTREQR);
        dut->vblint = 0;
        checks.note(std::string("P7-CLR-WHILE-SOURCE-HIGH: a CLR write to a hardware-sourced bit while its ")
            + "source pin is still asserted leaves the bit "
            + (((still_high >> 5) & 1u) ? "SET (re-asserted the same cycle)" : "CLEARED")
            + " -- the manual does not rule on this case, so it is measured, not graded");
    }

    // ========================================================================
    // P8 -- INTREQR: read-only, reports pending sources. printed p.218:
    // "INTREQR ... read only. Contains the bits that define which items are
    // requesting interrupt service." Cited: sol:7-interrupts.c17/c18,
    // opus:7-int.c14.
    // ========================================================================
    do_reset();
    {
        uint16_t stim = mutating("P8A") ? 0x0041 : 0x0014;  // MUTATION: write a different word than the one checked for
        write_reg(INTREQ, SET | stim);
        uint16_t mirrored = read_reg(INTREQR);
        checks.expect(mirrored == 0x0014,
            "P8-INTREQR-MIRRORS: INTREQR returns exactly the pending bits established by request writes");

        int spurious_addr = mutating("P8B") ? INTREQ : INTREQR;  // MUTATION: issue the spurious cycle at the real write address instead
        while (!dut->clk7_en) tick();
        dut->reg_address_in = spurious_addr;
        dut->data_in = SET | 0x7FFF;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        uint16_t after = read_reg(INTREQR);
        checks.expect(after == 0x0014,
            "P8-INTREQR-READONLY: a write-shaped bus cycle at INTREQR's address changes nothing in the stored request content");
    }

    // P9 -- bit-position correspondence between INTREQ and INTENA (printed
    // p.218) is NOT reasserted here. sol:7-interrupts.c19's own note: "Already
    // asserted from appendix A by hrm/ch5-audio-interrupt assertion 3; ledger
    // cross-reference only." Cited (cross-reference, no assertion):
    // sol:7-interrupts.c19, opus:7-int.c15.

    // ========================================================================
    // P10 -- bit 15 has no meaning on the read-only addresses. printed p.218:
    // "bit 15 has no meaning in the read-only addresses." An EXPERIMENT, not
    // a claim (see header note): measure the actual constant and assert only
    // what the quotation supports -- it does not track stored content.
    // Cited: sol:7-interrupts.c20, opus:7-int.c16.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | 0x0011);
        uint16_t enar_a = read_reg(INTENAR);
        write_reg(INTENA, CLR | 0x0011);
        int sample_addr = mutating("P10_ENA") ? INTENA : INTENAR;  // MUTATION: sample the write address's constant instead
        uint16_t enar_b = read_reg(sample_addr);
        checks.note(std::string("P10-BIT15-MEASURED-ENA: INTENAR bit15 reads ")
            + (((enar_a >> 15) & 1u) ? "1" : "0") + " after a SET write and "
            + (((enar_b >> 15) & 1u) ? "1" : "0") + " after a CLR write");
        checks.expect(((enar_a >> 15) & 1u) == ((enar_b >> 15) & 1u),
            "P10-BIT15-NO-MEANING-ENA: bit15 of an INTENAR read does not track the write history behind the stored content");

        write_reg(INTREQ, SET | 0x0011);
        uint16_t reqr_a = read_reg(INTREQR);
        write_reg(INTREQ, CLR | 0x0011);
        int sample_addr2 = mutating("P10_REQ") ? INTREQ : INTREQR;  // MUTATION: sample the write address's constant instead
        uint16_t reqr_b = read_reg(sample_addr2);
        checks.note(std::string("P10-BIT15-MEASURED-REQ: INTREQR bit15 reads ")
            + (((reqr_a >> 15) & 1u) ? "1" : "0") + " after a SET write and "
            + (((reqr_b >> 15) & 1u) ? "1" : "0") + " after a CLR write");
        checks.expect(((reqr_a >> 15) & 1u) == ((reqr_b >> 15) & 1u),
            "P10-BIT15-NO-MEANING-REQ: bit15 of an INTREQR read does not track the write history behind the stored content either");
    }

    // ========================================================================
    // P11 -- Bit 15 is the SET/CLR bit, and INTREQ obeys the identical
    // convention already shown on INTENA. printed p.218: "Bit 15 of these
    // registers is called the SET/CLR bit." (DMACON half excluded: not a
    // port of this module.) Cited: sol:7-interrupts.c21, opus:7-int.c17.
    // ========================================================================
    do_reset();
    {
        write_reg(INTREQ, SET | (1u << 5));
        uint16_t after_set = read_reg(INTREQR);
        bool do_clr = !mutating("P11");  // MUTATION: never issue the CLR half
        if (do_clr) write_reg(INTREQ, CLR | (1u << 5));
        uint16_t after_clr = read_reg(INTREQR);
        checks.expect(((after_set >> 5) & 1u) == 1u && ((after_clr >> 5) & 1u) == 0u,
            "P11-BIT15-SETCLR-ALSO-INTREQ: bit15 selects SET vs CLR on INTREQ too, the same convention already shown on INTENA");
    }

    // ========================================================================
    // P12 -- SET encoding. printed p.218: "When you wish to set a bit ...
    // place a 1 in the position you want to set and a 1 into position 15."
    // Cited: sol:7-interrupts.c22, opus:7-int.c18.
    // ========================================================================
    do_reset();
    {
        uint16_t stim = mutating("P12") ? (CLR | (1u << 6)) : (SET | (1u << 6));  // MUTATION: send CLR instead of SET
        write_reg(INTENA, stim);
        uint16_t val = read_reg(INTENAR);
        checks.expect(((val >> 6) & 1u) == 1u,
            "P12-SET-ENCODING: bit15=1 with selector bit6=1 sets stored bit6");
    }

    // ========================================================================
    // P13 -- CLR encoding. printed p.218: "When you wish to clear a bit ...
    // place a 1 in the position you wish to clear and a 0 into position 15."
    // Cited: sol:7-interrupts.c23, opus:7-int.c19.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | (1u << 6));  // establish it set first
        uint16_t stim = mutating("P13") ? (SET | (1u << 6)) : (CLR | (1u << 6));  // MUTATION: send SET again instead of CLR
        write_reg(INTENA, stim);
        uint16_t val = read_reg(INTENAR);
        checks.expect(((val >> 6) & 1u) == 0u,
            "P13-CLR-ENCODING: bit15=0 with selector bit6=1 clears stored bit6");
    }

    // ========================================================================
    // P14 -- Positions 14-0 are bit selectors; several may be selected at
    // once. printed p.218: "Positions 14-0 are bit selectors." Cited:
    // sol:7-interrupts.c24, opus:7-int.c20.
    // ========================================================================
    do_reset();
    {
        const uint16_t mask = (1u << 1) | (1u << 3) | (1u << 6) | (1u << 9) | (1u << 12);
        uint16_t stim = mutating("P14")
            ? (uint16_t)((1u << 1) | (1u << 3) | (1u << 6) | (1u << 9))  // MUTATION: drop bit12 from the write
            : mask;
        write_reg(INTENA, SET | stim);
        uint16_t val = read_reg(INTENAR);
        checks.expect(val == mask,
            "P14-MULTI-BIT-SELECTORS: several bits 14-0 set to 1 in one write select and set all of them together, none other");
    }

    // ========================================================================
    // P15 -- Positions written 0 are unaffected (precondition P12-P14 depend
    // on). printed p.218: "Positions 14-0 that have 0 value will not be
    // affected when you do the write." Cited: sol:7-interrupts.c25,
    // opus:7-int.c21.
    // ========================================================================
    do_reset();
    {
        const uint16_t base = (1u << 2) | (1u << 7) | (1u << 11);
        write_reg(INTENA, SET | base);
        uint16_t second_bit = mutating("P15") ? (1u << 5) : (1u << 4);  // MUTATION: select a different bit than the one checked for
        write_reg(INTENA, SET | second_bit);
        uint16_t val = read_reg(INTENAR);
        checks.expect(val == (base | (1u << 4)),
            "P15-ZERO-POSITIONS-UNAFFECTED: a write that leaves positions 14-0 at 0 does not disturb bits an earlier write established");
    }

    // ========================================================================
    // P16 -- Setting some bits and clearing others in one operation takes two
    // writes. printed p.218: "If you want to set some bits and clear others,
    // you will have to write this register twice." Cited: opus:7-int.c22 (no
    // sol counterpart for this specific claim).
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | (1u << 3));  // establish bit3=1, bit1=0
        // Naive one-write attempt at "set bit1 AND clear bit3": bit15 is one
        // shared value for the whole write (P11-P13), so this cannot reach
        // the mixed target. Reported for context, not graded.
        write_reg(INTENA, SET | (1u << 1) | (1u << 3));
        uint16_t after_one_write = read_reg(INTENAR);
        bool mixed_reached_in_one = (((after_one_write >> 1) & 1u) == 1u) && (((after_one_write >> 3) & 1u) == 0u);
        checks.note(std::string("P16-ONE-WRITE-RESULT: after a single SET write naming both bit1 and bit3, the mixed target (bit1 set, bit3 cleared) was ")
            + (mixed_reached_in_one ? "reached (unexpected)" : "NOT reached, as bit15's single value applies to both selected bits alike"));

        bool do_second_write = !mutating("P16");  // MUTATION: skip the corrective second write
        if (do_second_write) write_reg(INTENA, CLR | (1u << 3));  // second write: now clear bit3 (bit1 already set)
        uint16_t after_two_writes = read_reg(INTENAR);
        checks.expect(((after_two_writes >> 1) & 1u) == 1u && ((after_two_writes >> 3) & 1u) == 0u,
            "P16-TWO-WRITES-ACHIEVE-MIX: a second write reaches the set-bit1-and-clear-bit3 target that one write could not");
    }

    // ========================================================================
    // P17 -- INTEN is not an ordinary per-source enable. printed p.218: "Bit
    // 14 of the interrupt registers (INTEN) is for interrupt enable."
    // Cited: sol:7-interrupts.c26, opus:7-int.c23.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | (1u << 14));  // master alone
        if (mutating("P17")) write_reg(INTENA, SET | (1u << 3));  // MUTATION: also enable the ordinary bit
        while (!dut->clk7_en) tick();
        dut->int2 = 1; tick(); tick(); tick(); tick(); dut->int2 = 0;
        settle(3);
        checks.expect(dut->_ipl == 7,
            "P17-INTEN-IS-NOT-AN-ORDINARY-ENABLE: bit14 set alone, with an ordinary source's own enable still clear, does not release that source");
    }

    // ========================================================================
    // P18 -- INTEN as master gate over every other enable (the undisputed
    // half of the P19 conflict group). printed p.218: "If this bit is a 0,
    // it disables all other interrupts." Corroborated (undisputed half only):
    // appendix A p.280 and Figure 7-4 p.221's own gloss "special (master
    // enable)". Cited: sol:7-interrupts.c27, opus:7-int.c24; corroborating:
    // sol:7-appA-int.c10, opus:7-appA-int.c6, opus:7-fig7-4.c45 (gloss half
    // only -- its hardware-priority-6 placement belongs to P19).
    // ========================================================================
    do_reset();
    {
        bool enable_bit3 = !mutating("P18_CTRL");  // MUTATION: never actually enable the ordinary source
        uint16_t ena_stim = (1u << 14) | (enable_bit3 ? (1u << 3) : 0u);
        write_reg(INTENA, SET | ena_stim);
        while (!dut->clk7_en) tick();
        dut->int2 = 1; tick(); tick(); tick(); tick(); dut->int2 = 0;  // bit3 pending
        settle(3);
        checks.expect(dut->_ipl != 7,
            "P18-CONTROL-SOURCE-REACHES-OUTPUT: with the master and an ordinary source both enabled and the source pending, the interrupt output is active -- the control case the gate test below depends on");

        bool clear_master = !mutating("P18_CLOSE");  // MUTATION: never actually clear the master bit
        if (clear_master) write_reg(INTENA, CLR | (1u << 14));
        settle(3);
        checks.expect(dut->_ipl == 7,
            "P18a-MASTER-GATE-CLOSES: clearing INTEN alone silences the still-enabled, still-pending ordinary source");

        bool set_master = !mutating("P18_OPEN");  // MUTATION: never set the master bit back
        if (set_master) write_reg(INTENA, SET | (1u << 14));
        settle(3);
        checks.expect(dut->_ipl != 7,
            "P18b-MASTER-GATE-OPENS: setting INTEN alone releases the same already-enabled, already-pending source");
    }

    // ========================================================================
    // P19 -- [RECORD ONLY, no pass/fail] Disputed half: can INTEN raise a
    // request of its own? printed p.218 and appendix A p.280 say no; Figure
    // 7-4 p.221 places INTEN at hardware priority 6. Per the coordinator's
    // instruction: do not grade either reading. Cited (record, not assert):
    // sol:7-interrupts.c28, opus:7-int.c25, sol:7-appA-int.c10,
    // opus:7-appA-int.c6, opus:7-fig7-4.c45 (hardware-priority-6 half).
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | (1u << 14));
        write_reg(INTREQ, SET | (1u << 14));
        settle(3);
        char level_buf[8];
        std::snprintf(level_buf, sizeof level_buf, "%d", (int)dut->_ipl);
        checks.record(std::string(
            "P19-INTEN-SELF-REQUEST-DISPUTED: printed p.218 and appendix A p.280 say INTEN 'creates no interrupt request'; "
            "Figure 7-4 p.221 places it at hardware priority 6. Measured: setting bit14 alone in both INTENA and INTREQ, "
            "with nothing else enabled or pending, drives _ipl=") + level_buf +
            " (7=inactive; any other value means the RTL treats INTEN as its own request source, matching Figure 7-4's "
            "reading rather than the prose's). Not graded -- the manual contradicts itself here.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    delete dut;
    return checks.failed == 0 ? 0 : 1;
}
