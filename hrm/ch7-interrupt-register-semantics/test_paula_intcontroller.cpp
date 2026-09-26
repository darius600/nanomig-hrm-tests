// Test purpose: HRM conformance package for chapter 7's own reading (printed
//   pp.217-218, plus Figure 7-4 p.221) of Paula's interrupt controller: the
//   four register addresses, the SET/CLR write convention, bit 15's "no
//   meaning" on the read-only addresses, the INTEN master gate, and
//   write-to-clear stickiness. 41 claims, from
//   work/ch7-build/ch7-interrupt-register-semantics.jsonl. Full derivation
//   in SPECIFICATION.md (committed as 4fbfa90 before this file was written).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.217-218 (chapter 7 body), corroborated by appendix A p.280 and Figure
//   7-4 p.221 for the INTEN master-gate claim group, and by appendix A p.280
//   alone (its "not automatically reset ... writing to this address"
//   sentence) for P7-STICKY-PERSISTS's persistence half - a citation-mapping
//   repair, see the MUTATION METHODOLOGY note below.
// Fixture boundary: paula_intcontroller.v ONLY. No other module is
//   instantiated anywhere in this file.
// Non-goals: anything outside the `paula_intcontroller` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch7-build/ch7-interrupt-register-semantics.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
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
//   meaning in the read-only addresses") does not name a constant and does
//   not promise the value is stable across write histories either - a bit
//   declared meaningless is not thereby declared constant. P10 below only
//   `note`s the measured constant (both `expect()`s that once compared
//   stability across two write histories were DEMOTED to `record()` in the
//   independent-validation repair; see MUTATION METHODOLOGY below).
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup - not a compile-time #ifdef - so one build serves the real run
//   (MUTATE unset) and all 26 mutation runs (MUTATE=<id>, one id per
//   assertion). Each mutation changes what the module is ASKED TO DO (a
//   written value, a selected address, a skipped stimulus step) and leaves
//   the `expect()` call's own expected-value code untouched, per BUILD-BRIEF.
//   Every one of the 26 below was run with its own id and confirmed red, then
//   reverted (MUTATE unset) and confirmed green again; results are logged
//   next to each assertion and summarised in the build report.
//
//   Repaired against an independent validation
//   (work/2026-09-13_ch7_sol_validation_B1.md, findings 1/3/4/5/7): P4, P6's
//   write-only readback and both P10 bit15-stability checks were DEMOTED
//   (they graded a read value or a stability guarantee the manual never
//   specifies for a write-only or "no meaning" address - pp.217-218, 263-264
//   - so they are now `note()`+`record()`, reason_kind source-silent, with
//   their MUTATE ids and mutation records retired along with them).
//   P16-ONE-WRITE-RESULT was PROMOTED from `note()` to a graded `expect()`
//   (the two-write assertion alone had no failing predicate for a device
//   that reached the mixed target in one write - finding 1) and gained its
//   own MUTATE=P16_ONE record below. P3-FOUR-DISTINCT-ADDRESSES was
//   rewritten (finding 4): it no longer probes an unrelated, manual-silent
//   address and instead shows the two register pairs hold independent,
//   non-aliased content. P2, P3, both A0 pairs, both P5, both P8, P14 and
//   P15 each gained an explicit all-clear write (CLR encoding, P13) before
//   the equality check they already made, so exact-equality no longer
//   silently assumes an undocumented reset value of zero (finding 5) - same
//   assertions, same expected numbers, grounded precondition. P7-STICKY-
//   PERSISTS's citation was remapped from p.218 to p.280, which is what
//   actually states the persistence half (finding 7).
//
// MUTATION RECORDS. One per assertion, in BUILD-BRIEF's fixed grammar so the
//   gate can pair them. The switch is the MUTATE environment variable
//   described above; the trailing (MUTATE=<id>) names it. Every colour below
//   was MEASURED -- each id was run against this build and the assertion it
//   turns red was read off that run, not taken from the mutation's intent.
//   Baseline with MUTATE unset: 26 passed, 0 failed.
//
//   Three ids redden a neighbouring assertion as well, which is recorded here
//   rather than tidied away: P18_CTRL also reddens P18b-MASTER-GATE-OPENS,
//   P5A also reddens P5-INTENAR-READONLY, and P8A also reddens
//   P8-INTREQR-READONLY. Each is still the sole record for the assertion it
//   is named against.
//
// MUTATION A0-LIVENESS-INTENA-FIRST stimulus red: a different first word (0x00AA) is written to INTENA (MUTATE=A0_ENA1)
// MUTATION A0-LIVENESS-INTENA-SECOND stimulus red: the stale first word is replayed as the second (MUTATE=A0_ENA2)
// MUTATION A0-LIVENESS-INTREQ-FIRST stimulus red: a different first word (0x0041) is written to INTREQ (MUTATE=A0_REQ1)
// MUTATION A0-LIVENESS-INTREQ-SECOND stimulus red: the stale first word is replayed as the second (MUTATE=A0_REQ2)
// MUTATION P1-MASK-ENABLE-CLEAR stimulus red: bit 3 is also enabled early, before the cleared-mask read (MUTATE=P1)
// MUTATION P1-MASK-ENABLE-SET stimulus red: bit 3 is never actually enabled (MUTATE=P1B)
// MUTATION P2-TWO-REGISTERS-INDEPENDENT stimulus red: INTENAR is re-read and treated as the request register (MUTATE=P2)
// MUTATION P3-FOUR-DISTINCT-ADDRESSES stimulus red: the request pair's read address is secretly read and called the enable pair's (MUTATE=P3)
// MUTATION P5-INTENAR-MIRRORS stimulus red: a different word (0x001F) is written than the one checked for (MUTATE=P5A)
// MUTATION P5-INTENAR-READONLY stimulus red: the spurious cycle is issued at INTENA, the real write address (MUTATE=P5B)
// MUTATION P6-INTREQ-FORCE-SOFTWARE stimulus red: the bus write is skipped entirely (MUTATE=P6_FORCE)
// MUTATION P7-STICKY-PERSISTS stimulus red: a clear is sneaked in before the idle stretch ends (MUTATE=P7_PERSIST)
// MUTATION P7-STICKY-CLEARS stimulus red: the explicit clear write is skipped (MUTATE=P7_CLEARS)
// MUTATION P8-INTREQR-MIRRORS stimulus red: a different word (0x0041) is written than the one checked for (MUTATE=P8A)
// MUTATION P8-INTREQR-READONLY stimulus red: the spurious cycle is issued at INTREQ, the real write address (MUTATE=P8B)
// MUTATION P11-BIT15-SETCLR-ALSO-INTREQ stimulus red: the CLR half of the pair is never issued (MUTATE=P11)
// MUTATION P12-SET-ENCODING stimulus red: CLR is sent where the assertion asks for SET (MUTATE=P12)
// MUTATION P13-CLR-ENCODING stimulus red: SET is sent again where the assertion asks for CLR (MUTATE=P13)
// MUTATION P14-MULTI-BIT-SELECTORS stimulus red: bit 12 is dropped from the multi-bit write (MUTATE=P14)
// MUTATION P15-ZERO-POSITIONS-UNAFFECTED stimulus red: a different bit is selected than the one checked for (MUTATE=P15)
// MUTATION P16-ONE-WRITE-RESULT stimulus red: bit3 is never written by either the establishing step or the one-write attempt's own selector, so an untouched bit3 trivially reads as "cleared" (MUTATE=P16_ONE)
// MUTATION P16-TWO-WRITES-ACHIEVE-MIX stimulus red: the corrective second write is skipped (MUTATE=P16)
// MUTATION P17-INTEN-IS-NOT-AN-ORDINARY-ENABLE stimulus red: the ordinary bit is also enabled (MUTATE=P17)
// MUTATION P18-CONTROL-SOURCE-REACHES-OUTPUT stimulus red: the ordinary source is never actually enabled (MUTATE=P18_CTRL)
// MUTATION P18a-MASTER-GATE-CLOSES stimulus red: the master bit is never actually cleared (MUTATE=P18_CLOSE)
// MUTATION P18b-MASTER-GATE-OPENS stimulus red: the master bit is never set back (MUTATE=P18_OPEN)
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
        // Establish a known all-clear state explicitly (CLR encoding, P13)
        // before checking exact equality below, instead of relying on an
        // undocumented reset value for the bits outside the pattern written.
        write_reg(INTENA, CLR | 0x7FFF);
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
        // Establish a known all-clear state explicitly (CLR encoding, P13)
        // before checking exact equality below, instead of relying on an
        // undocumented reset value for the bits outside the pattern written.
        write_reg(INTREQ, CLR | 0x7FFF);
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
            "P1-MASK-ENABLE-CLEAR (p.217, sol:7-interrupts.c5, opus:7-int.c6): bit3 pending with bit3's own enable clear leaves the interrupt output inactive even though the master enable is set");

        bool enable_bit3 = !mutating("P1B");  // MUTATION: never actually enable bit3
        if (enable_bit3) write_reg(INTENA, SET | (1u << 3));
        settle(3);
        checks.expect(dut->_ipl != 7,
            "P1-MASK-ENABLE-SET (p.217, sol:7-interrupts.c5, opus:7-int.c6): enabling bit3 releases the still-pending request to the interrupt output");
    }

    // ========================================================================
    // P2/P3 -- Two independent registers, four distinct addresses. printed
    // p.217: "There are two interrupt registers, interrupt enable (mask) and
    // interrupt request (status)." / "Each register has both a read and a
    // write address." Cited: sol:7-interrupts.c10/c11, opus:7-int.c9.
    // ========================================================================
    do_reset();
    {
        // Establish a known all-clear state on both registers explicitly (via
        // the already-tested CLR encoding, P13) before checking exact
        // equality below, instead of relying on an undocumented reset value.
        write_reg(INTENA, CLR | 0x7FFF);
        write_reg(INTREQ, CLR | 0x7FFF);

        write_reg(INTENA, SET | 0x0021);  // nonzero pattern in the enable register only
        uint16_t enar = read_reg(INTENAR);
        int req_addr = mutating("P2") ? INTENAR : INTREQR;  // MUTATION: re-read INTENAR and call it the request register
        uint16_t reqr = read_reg(req_addr);
        checks.expect(enar == 0x0021 && reqr == 0x0000,
            "P2-TWO-REGISTERS-INDEPENDENT (p.217, sol:7-interrupts.c10, opus:7-int.c9): writing only the enable register, from an explicit all-clear baseline, leaves the request register's own readback at 0");

        // P3 does not probe an unrelated, manual-silent address (that proved
        // nothing about the four NAMED addresses). Instead it writes a
        // second, distinct known pattern into the request pair and confirms
        // each pair's own read address reflects only its own pair's write --
        // the actual test of "four distinct addresses" printed p.217 claims.
        write_reg(INTREQ, SET | 0x0044);
        int enar_addr = mutating("P3") ? INTREQR : INTENAR;  // MUTATION: secretly read the request pair's read address and call it the enable pair's
        uint16_t enar2 = read_reg(enar_addr);
        uint16_t reqr2 = read_reg(INTREQR);
        // P20/P21 (SPECIFICATION.md): appendix B's own address-order rows for
        // INTENAR ($01C) and INTREQR ($01E), opus:B-intenar.addr and
        // opus:B-intreqr.addr. This assertion already reads INTENAR at its
        // own $01C address and INTREQR at its own $01E address -- the RTL's
        // INTENAR/INTREQR parameters (paula_intcontroller.v:27-28) -- which
        // is the whole of what each of those two claims states, so they are
        // cited here rather than reasserted.
        checks.expect(enar2 == 0x0021 && reqr2 == 0x0044,
            "P3-FOUR-DISTINCT-ADDRESSES (p.217, sol:7-interrupts.c11, opus:7-int.c9; also grades appendix B's reoffered address rows sol:7-appB-intena.c1, sol:7-appB-intenar.c1, sol:7-appB-intreq.c1, sol:7-appB-intreqr.c1, opus:B-intenar.addr, opus:B-intreqr.addr): after writing distinct known patterns into the two write addresses, each pair's own read address still reflects only its own pair's write -- the four named addresses decode to independent storage, not a shared one");
    }

    // ========================================================================
    // P4 -- INTENA: write-only. printed p.217: "INTENA ... write only. Sets
    // or clears specific bits of INTENA." Cited: sol:7-interrupts.c12,
    // opus:7-int.c10.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | 0x0021);
        uint16_t val = read_reg(INTENA);
        char val_buf[8];
        std::snprintf(val_buf, sizeof val_buf, "0x%04X", val);
        checks.note(std::string("P4-INTENA-WRITE-ONLY-MEASURED: reading INTENA's own write address after a SET write returns ") + val_buf);
        checks.record(std::string(
            "P4-INTENA-WRITE-ONLY (p.217, sol:7-interrupts.c12, opus:7-int.c10; also carries appendix B's reoffered "
            "sol:7-appB-intena.c2, the same write-only access-direction claim): not asserted -- 'write only' (p.217) fixes the "
            "access direction, not a read value, and printed pp.263-264 warns that reading a write-only address 'may trash the "
            "register and crash the system' (undefined), not that it returns ") + val_buf + " or any other particular constant. "
            "reason_kind source-silent.");
    }

    // ========================================================================
    // P5 -- INTENAR: read-only, mirrors INTENA. printed p.217: "INTENAR ...
    // read only. Reads contents of INTENA." Cited: sol:7-interrupts.c13,
    // opus:7-int.c11.
    // ========================================================================
    do_reset();
    {
        // Establish a known all-clear state explicitly (CLR encoding, P13)
        // before checking exact equality below, instead of relying on an
        // undocumented reset value for the bits outside the pattern written.
        write_reg(INTENA, CLR | 0x7FFF);
        uint16_t stim = mutating("P5A") ? 0x001F : 0x0079;  // MUTATION: write a different word than the one checked for
        write_reg(INTENA, SET | stim);
        uint16_t mirrored = read_reg(INTENAR);
        checks.expect(mirrored == 0x0079,
            "P5-INTENAR-MIRRORS (p.217, sol:7-interrupts.c13, opus:7-int.c11; also grades appendix B's reoffered sol:7-appB-intenar.c3, sol:7-appB-intenar.c4 -- function and chip: this measured content IS the interrupt-enable bits INTENAR reads, and chip ownership is structural, this file's only DUT being paula_intcontroller.v with no other chip's decode logic present): INTENAR returns exactly what was last written to INTENA");

        int spurious_addr = mutating("P5B") ? INTENA : INTENAR;  // MUTATION: issue the spurious cycle at the real write address instead
        while (!dut->clk7_en) tick();
        dut->reg_address_in = spurious_addr;
        dut->data_in = SET | 0x7FFF;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        uint16_t after = read_reg(INTENAR);
        checks.expect(after == 0x0079,
            "P5-INTENAR-READONLY (p.217, sol:7-interrupts.c13, opus:7-int.c11; also grades appendix B's reoffered sol:7-appB-intenar.c2, the same read-only access-direction claim): a write-shaped bus cycle at INTENAR's address changes nothing in the stored enable content");
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
            "P6-INTREQ-FORCE-SOFTWARE (p.218, sol:7-interrupts.c14, sol:7-interrupts.c15, opus:7-int.c12): a bus write alone, no hardware pin involved, sets a pending request bit");

        uint16_t wo = read_reg(INTREQ);
        char wo_buf[8];
        std::snprintf(wo_buf, sizeof wo_buf, "0x%04X", wo);
        checks.note(std::string("P6-INTREQ-WRITE-ONLY-MEASURED: reading INTREQ's own write address returns ") + wo_buf);
        checks.record(std::string(
            "P6-INTREQ-WRITE-ONLY (p.218, half graded: sol:7-interrupts.c14, opus:7-int.c12); also carries appendix B's reoffered "
            "sol:7-appB-intreq.c2, the same write-only access-direction claim): not asserted -- 'write only' (p.218) fixes the "
            "access direction, not a read value, and printed pp.263-264 warns that reading a write-only address is undefined "
            "('may trash the register and crash the system'), not that it returns ") + wo_buf + " or any other particular constant. "
            "reason_kind source-silent.");
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
            "P7-STICKY-PERSISTS (p.280: 'these status bits are not automatically reset when the interrupt is serviced ... must be reset when desired by writing to this address' -- corrected citation; p.218's sol:7-interrupts.c16/opus:7-int.c13 extract only the clearing sentence and remain cited on P7-STICKY-CLEARS below, not this persistence half; no uid in this package's own 41-claim ledger covers the p.280 sentence): a pending request bit survives 100 idle clocks with no clearing write");

        bool do_clear = !mutating("P7_CLEARS");  // MUTATION: skip the explicit clear write
        if (do_clear) write_reg(INTREQ, CLR | (1u << 4));
        uint16_t after_clear = read_reg(INTREQR);
        checks.expect(((after_clear >> 4) & 1u) == 0u,
            "P7-STICKY-CLEARS (p.218, sol:7-interrupts.c16, opus:7-int.c13): an explicit CLR write to INTREQ naming that bit clears it");
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
        // Establish a known all-clear state explicitly (CLR encoding, P13)
        // before checking exact equality below, instead of relying on an
        // undocumented reset value for the bits outside the pattern written.
        write_reg(INTREQ, CLR | 0x7FFF);
        uint16_t stim = mutating("P8A") ? 0x0041 : 0x0014;  // MUTATION: write a different word than the one checked for
        write_reg(INTREQ, SET | stim);
        uint16_t mirrored = read_reg(INTREQR);
        checks.expect(mirrored == 0x0014,
            "P8-INTREQR-MIRRORS (p.218, sol:7-interrupts.c17, sol:7-interrupts.c18, opus:7-int.c14; also grades appendix B's reoffered sol:7-appB-intreqr.c3, sol:7-appB-intreqr.c4 -- function and chip: this measured content IS the interrupt-request bits INTREQR reads, and chip ownership is structural, this file's only DUT being paula_intcontroller.v): INTREQR returns exactly the pending bits established by request writes");

        int spurious_addr = mutating("P8B") ? INTREQ : INTREQR;  // MUTATION: issue the spurious cycle at the real write address instead
        while (!dut->clk7_en) tick();
        dut->reg_address_in = spurious_addr;
        dut->data_in = SET | 0x7FFF;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        uint16_t after = read_reg(INTREQR);
        checks.expect(after == 0x0014,
            "P8-INTREQR-READONLY (p.218, sol:7-interrupts.c17, opus:7-int.c14; also grades appendix B's reoffered sol:7-appB-intreqr.c2, the same read-only access-direction claim): a write-shaped bus cycle at INTREQR's address changes nothing in the stored request content");
    }

    // ========================================================================
    // P9 -- bit-position correspondence between INTREQ and INTENA. printed
    // p.218: "The bit positions in the interrupt request register correspond
    // directly to those same positions in the interrupt enable register."
    // NOT reasserted here: sol:7-interrupts.c19's own note says
    // hrm/ch5-audio-interrupt's assertion 3 already tests this exact fact.
    // Declined with that reason, not graded -- a ledger cross-reference, the
    // same load-bearing distinction as P19's disputed half.
    // ========================================================================
    checks.record(
        "P9-BIT-POSITION-CORRESPONDENCE (p.218, sol:7-interrupts.c19, opus:7-int.c15): "
        "not tested by this package -- hrm/ch5-audio-interrupt's SHARED-BIT-LAYOUT-INTENA-INTREQ "
        "assertion already covers this exact proposition against the same module; declined here "
        "to avoid a duplicate assertion, not for lack of a way to test it.");

    // ========================================================================
    // P10 -- bit 15 has no meaning on the read-only addresses. printed p.218:
    // "bit 15 has no meaning in the read-only addresses." An EXPERIMENT, not
    // a claim (see header note): measure the actual constant only. Demoted
    // (independent-validation repair, finding 3) from an `expect()` that
    // compared the constant across two write histories for stability -- "no
    // meaning" rules out tracking stored content but does not promise a
    // stable value either, so nothing here is graded.
    // Cited: sol:7-interrupts.c20, opus:7-int.c16.
    // ========================================================================
    do_reset();
    {
        write_reg(INTENA, SET | 0x0011);
        uint16_t enar_a = read_reg(INTENAR);
        write_reg(INTENA, CLR | 0x0011);
        uint16_t enar_b = read_reg(INTENAR);
        checks.note(std::string("P10-BIT15-MEASURED-ENA: INTENAR bit15 reads ")
            + (((enar_a >> 15) & 1u) ? "1" : "0") + " after a SET write and "
            + (((enar_b >> 15) & 1u) ? "1" : "0") + " after a CLR write");
        checks.record(
            "P10-BIT15-NO-MEANING-ENA (p.218, sol:7-interrupts.c20, opus:7-int.c16): not asserted -- 'bit 15 has no meaning in "
            "the read-only addresses' rules out bit15 tracking stored content, but does not promise bit15 is STABLE across "
            "different write histories either; a bit the manual declares meaningless is not thereby declared constant. "
            "reason_kind source-silent.");

        write_reg(INTREQ, SET | 0x0011);
        uint16_t reqr_a = read_reg(INTREQR);
        write_reg(INTREQ, CLR | 0x0011);
        uint16_t reqr_b = read_reg(INTREQR);
        checks.note(std::string("P10-BIT15-MEASURED-REQ: INTREQR bit15 reads ")
            + (((reqr_a >> 15) & 1u) ? "1" : "0") + " after a SET write and "
            + (((reqr_b >> 15) & 1u) ? "1" : "0") + " after a CLR write");
        checks.record(
            "P10-BIT15-NO-MEANING-REQ (p.218, sol:7-interrupts.c20, opus:7-int.c16): not asserted, same reason as "
            "P10-BIT15-NO-MEANING-ENA above -- 'no meaning' does not promise a stable value either. reason_kind source-silent.");
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
            "P11-BIT15-SETCLR-ALSO-INTREQ (p.218, sol:7-interrupts.c21, opus:7-int.c17; also grades appendix B's reoffered sol:7-appB-intreq.c3, sol:7-appB-intreq.c4 -- function and chip: this SET/CLR behaviour IS the clear/set interrupt-request write port appendix B names, and chip ownership is structural, this file's only DUT being paula_intcontroller.v): bit15 selects SET vs CLR on INTREQ too, the same convention already shown on INTENA");
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
            "P12-SET-ENCODING (p.218, sol:7-interrupts.c22, opus:7-int.c18; also grades appendix B's reoffered sol:7-appB-intena.c3, sol:7-appB-intena.c4 -- chip and, together with P13-CLR-ENCODING below, function: this SET half plus P13's CLR half together are the clear/set interrupt-enable write port appendix B names, and chip ownership is structural, this file's only DUT being paula_intcontroller.v): bit15=1 with selector bit6=1 sets stored bit6");
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
            "P13-CLR-ENCODING (p.218, sol:7-interrupts.c23, opus:7-int.c19; also grades appendix B's reoffered sol:7-appB-intena.c4, function: this CLR half plus P12-SET-ENCODING's SET half above together are the clear/set interrupt-enable write port appendix B names): bit15=0 with selector bit6=1 clears stored bit6");
    }

    // ========================================================================
    // P14 -- Positions 14-0 are bit selectors; several may be selected at
    // once. printed p.218: "Positions 14-0 are bit selectors." Cited:
    // sol:7-interrupts.c24, opus:7-int.c20.
    // ========================================================================
    do_reset();
    {
        // Establish a known all-clear state explicitly (CLR encoding, P13)
        // before checking exact equality below, instead of relying on an
        // undocumented reset value for the bits outside the mask written.
        write_reg(INTENA, CLR | 0x7FFF);
        const uint16_t mask = (1u << 1) | (1u << 3) | (1u << 6) | (1u << 9) | (1u << 12);
        uint16_t stim = mutating("P14")
            ? (uint16_t)((1u << 1) | (1u << 3) | (1u << 6) | (1u << 9))  // MUTATION: drop bit12 from the write
            : mask;
        write_reg(INTENA, SET | stim);
        uint16_t val = read_reg(INTENAR);
        checks.expect(val == mask,
            "P14-MULTI-BIT-SELECTORS (p.218, sol:7-interrupts.c24, opus:7-int.c20): several bits 14-0 set to 1 in one write select and set all of them together, none other");
    }

    // ========================================================================
    // P15 -- Positions written 0 are unaffected (precondition P12-P14 depend
    // on). printed p.218: "Positions 14-0 that have 0 value will not be
    // affected when you do the write." Cited: sol:7-interrupts.c25,
    // opus:7-int.c21.
    // ========================================================================
    do_reset();
    {
        // Establish a known all-clear state explicitly (CLR encoding, P13)
        // before checking exact equality below, instead of relying on an
        // undocumented reset value for the bits outside base|bit4.
        write_reg(INTENA, CLR | 0x7FFF);
        const uint16_t base = (1u << 2) | (1u << 7) | (1u << 11);
        write_reg(INTENA, SET | base);
        uint16_t second_bit = mutating("P15") ? (1u << 5) : (1u << 4);  // MUTATION: select a different bit than the one checked for
        write_reg(INTENA, SET | second_bit);
        uint16_t val = read_reg(INTENAR);
        checks.expect(val == (base | (1u << 4)),
            "P15-ZERO-POSITIONS-UNAFFECTED (p.218, sol:7-interrupts.c25, opus:7-int.c21): a write that leaves positions 14-0 at 0 does not disturb bits an earlier write established");
    }

    // ========================================================================
    // P16 -- Setting some bits and clearing others in one operation takes two
    // writes. printed p.218: "If you want to set some bits and clear others,
    // you will have to write this register twice." Cited: opus:7-int.c22 (no
    // sol counterpart for this specific claim).
    // ========================================================================
    do_reset();
    {
        // MUTATION: bit3 is never actually written by either the establishing
        // step or the one-write attempt's own selector, so an untouched bit3
        // would trivially read as "cleared" after the one-write attempt --
        // the very false positive P16-ONE-WRITE-RESULT exists to catch.
        bool touch_bit3 = !mutating("P16_ONE");
        if (touch_bit3) write_reg(INTENA, SET | (1u << 3));  // establish bit3=1, bit1=0
        // Naive one-write attempt at "set bit1 AND clear bit3": bit15 is one
        // shared value for the whole write (P11-P13), so this cannot reach
        // the mixed target.
        uint16_t one_write_stim = touch_bit3 ? (SET | (1u << 1) | (1u << 3)) : (uint16_t)(SET | (1u << 1));
        write_reg(INTENA, one_write_stim);
        uint16_t after_one_write = read_reg(INTENAR);
        bool mixed_reached_in_one = (((after_one_write >> 1) & 1u) == 1u) && (((after_one_write >> 3) & 1u) == 0u);
        checks.note(std::string("P16-ONE-WRITE-RESULT: after a single SET write naming both bit1 and bit3, the mixed target (bit1 set, bit3 cleared) was ")
            + (mixed_reached_in_one ? "reached (unexpected)" : "NOT reached, as bit15's single value applies to both selected bits alike"));
        checks.expect(!mixed_reached_in_one,
            "P16-ONE-WRITE-RESULT (p.218, opus:7-int.c22): a single SET write naming both bit1 and bit3 does not reach the mixed target (bit1 set, bit3 cleared) -- the failing predicate the manual's 'you will have to write this register twice' consequence requires");

        bool do_second_write = !mutating("P16");  // MUTATION: skip the corrective second write
        if (do_second_write) write_reg(INTENA, CLR | (1u << 3));  // second write: now clear bit3 (bit1 already set)
        uint16_t after_two_writes = read_reg(INTENAR);
        checks.expect(((after_two_writes >> 1) & 1u) == 1u && ((after_two_writes >> 3) & 1u) == 0u,
            "P16-TWO-WRITES-ACHIEVE-MIX (p.218, opus:7-int.c22): a second write reaches the set-bit1-and-clear-bit3 target that one write could not");
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
            "P17-INTEN-IS-NOT-AN-ORDINARY-ENABLE (p.218, sol:7-interrupts.c26, opus:7-int.c23): bit14 set alone, with an ordinary source's own enable still clear, does not release that source");
    }

    // ========================================================================
    // P18 -- INTEN as master gate over every other enable (the undisputed
    // half of the P19 conflict group). printed p.218: "If this bit is a 0,
    // it disables all other interrupts." Cited: sol:7-interrupts.c27,
    // opus:7-int.c24. Appendix A p.280 and Figure 7-4 p.221's "special
    // (master enable)" gloss say the same thing, but both of those claim
    // rows are classified report-only as a whole (they carry the disputed
    // "no request" / hardware-priority-6 half too) -- their uids are cited
    // on P19's record() line, which tells the truth about the whole row,
    // not split across an expect() and a record().
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
            "P18a-MASTER-GATE-CLOSES (p.218, sol:7-interrupts.c27, opus:7-int.c24): clearing INTEN alone silences the still-enabled, still-pending ordinary source");

        bool set_master = !mutating("P18_OPEN");  // MUTATION: never set the master bit back
        if (set_master) write_reg(INTENA, SET | (1u << 14));
        settle(3);
        checks.expect(dut->_ipl != 7,
            "P18b-MASTER-GATE-OPENS (p.218, sol:7-interrupts.c27, opus:7-int.c24): setting INTEN alone releases the same already-enabled, already-pending source");
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
            "P19-INTEN-SELF-REQUEST-DISPUTED (pp.218/280/221, sol:7-interrupts.c28, opus:7-int.c25, "
            "sol:7-appA-int.c10, opus:7-appA-int.c6, opus:7-fig7-4.c45): printed p.218 and appendix A p.280 say "
            "INTEN 'creates no interrupt request'; "
            "Figure 7-4 p.221 places it at hardware priority 6. Measured: setting bit14 alone in both INTENA and INTREQ, "
            "with nothing else enabled or pending, drives _ipl=") + level_buf +
            " (7=inactive; any other value means the RTL treats INTEN as its own request source, matching Figure 7-4's "
            "reading rather than the prose's). Not graded -- the manual contradicts itself here.");
    }

    // ========================================================================
    // APPENDIX B REOFFER -- the four opus:7-appB bundled rows (c21/c23/c31/c32)
    // each restate ONE appendix-B print row as a single claim id spanning four
    // clauses: address, access direction, chip, and function. Every one of
    // those four clauses does have a home above (address on P3; access
    // direction on P4/P5/P6/P8; function on P12+P13/P11/P5-MIRRORS/
    // P8-MIRRORS; chip riding structurally on those same function lines,
    // since this file's only DUT is paula_intcontroller.v with no other
    // chip's decode logic present) -- but no SINGLE assertion in this file
    // grades the four-clause conjunction the printed row states as one
    // sentence. Per this suite's convention that a multi-clause id is not
    // partially kept on a citation that only covers part of it (the same
    // shape as hrm/ch3-register-map-denise's opus:7-appB.c34 and
    // hrm/ch5-audio-location-pointer's COVERAGE REPAIR 2), each bundled id
    // below is declined, not weakened onto one of its own clauses' lines.
    // ========================================================================
    checks.record(
        "APPB-BUNDLE-INTENA opus:7-appB.c31 (p.290): not asserted as the bundled row it is -- its four clauses each "
        "have a home (address: P3-FOUR-DISTINCT-ADDRESSES; access direction: P4-INTENA-WRITE-ONLY, declined there for the "
        "same reason; function and chip: P12-SET-ENCODING/P13-CLR-ENCODING) but no one assertion grades all four together. "
        "Per this suite's multi-clause convention the bundled id is declined, not partially kept.");
    checks.record(
        "APPB-BUNDLE-INTENAR opus:7-appB.c21 (p.289): not asserted as the bundled row it is -- its four clauses each "
        "have a home (address: P3-FOUR-DISTINCT-ADDRESSES; access direction: P5-INTENAR-READONLY; function and chip: "
        "P5-INTENAR-MIRRORS) but no one assertion grades all four together. Per this suite's multi-clause convention the "
        "bundled id is declined, not partially kept.");
    checks.record(
        "APPB-BUNDLE-INTREQ opus:7-appB.c32 (p.290): not asserted as the bundled row it is -- its four clauses each "
        "have a home (address: P3-FOUR-DISTINCT-ADDRESSES; access direction: P6-INTREQ-WRITE-ONLY, declined there for the "
        "same reason; function and chip: P11-BIT15-SETCLR-ALSO-INTREQ) but no one assertion grades all four together. Per "
        "this suite's multi-clause convention the bundled id is declined, not partially kept.");
    checks.record(
        "APPB-BUNDLE-INTREQR opus:7-appB.c23 (p.289): not asserted as the bundled row it is -- its four clauses each "
        "have a home (address: P3-FOUR-DISTINCT-ADDRESSES; access direction: P8-INTREQR-READONLY; function and chip: "
        "P8-INTREQR-MIRRORS) but no one assertion grades all four together. Per this suite's multi-clause convention the "
        "bundled id is declined, not partially kept.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    delete dut;
    return checks.failed == 0 ? 0 : 1;
}
