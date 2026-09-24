// aa-interrupt-register-map -- paula_intcontroller conformance test
//
// Test purpose: the AA specification's address-ordered register list gives an
//   address, an access type and a chip for each of the four interrupt
//   registers, and a function column that says the two writable ones CLEAR OR
//   SET their bits. This grades the addresses against the decoder, the access
//   letters against what a write at each address actually does, and the
//   clear-or-set convention against the bit that selects it.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6 and 7, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the interrupt priority encoder, the
//   _ipl output, the master-enable gating of intreqena, or any of the eleven
//   hardware interrupt inputs. It grades where four registers are decoded,
//   which direction each one moves data, and what bit 15 selects.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `sol:AA-intenar.c3` and `sol:AA-intreqr.c3` each name one reader's reading
//   of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s7.json lists under `inventories`.
//
// WHY THESE FOUR. They are one module, one register family, one boundary, and
//   all four are declared as a full address in the RTL:
//
//     parameter INTENAR = 9'h01c;    parameter INTENA  = 9'h09a;
//     parameter INTREQR = 9'h01e;    parameter INTREQ  = 9'h09c;
//
//   tools/aa/lot_candidates.py reports where the rest of the gradeable
//   material is; this is one of eleven modules that had no package.
//
// WHAT THE TWO READERS SAID. Both halves of the assigned pair claimed the same
//   four fields for each register, independently: the address, the access
//   letters, the chip column, and the function column. Their wordings differ
//   and the propositions do not. The uids on the graded lines below are the
//   union's, and tools/aa/build_interrupt_lot.py is the lot that carries them.
//
// HOW EACH ADDRESS IS OBSERVED. paula_intcontroller has a read path, which
//   makes this simpler than the Copper package:
//
//     data_out = intenar | intreqr                            (:44)
//     intenar  = {1'b0, intena[14:0]}  when addressed at $01C (:65)
//     intreqr  = {1'b0, intreq[14:0]}  when addressed at $01E (:77)
//
//   so the enable and request registers are each readable at exactly one
//   address, and reading at any third address returns zero from both.
//
// WHY BIT 15 NEVER COMES BACK. Both read paths force it: `{1'b0, ...[14:0]}`.
//   Bit 15 is the clear/set selector on the way in and reads as zero on the
//   way out. Every pattern in this file therefore has bit 15 clear in its
//   expected value, and the assertions compare against a 15-bit quantity. That
//   is not a finding here -- the address list's function column says "clear or
//   set", which is what bit 15 is for, and says nothing about reading it back.
//
// WHY THE INTERRUPT INPUTS ARE HELD LOW. Thirteen of the fifteen intreq bits
//   are `intreq[n] <= tmp[n] | <input>` (:107-133). With every input at zero
//   the OR is the identity and the register holds exactly what was written, so
//   an assertion that reads a bit back is measuring the decoder and not a
//   stray interrupt. A fixture that let any of them float would grade the
//   inputs instead.

#include "Vpaula_intcontroller.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

// Printing a value with std::to_string under an "0x" label prints the DECIMAL
// digits. The Copper package's first run reported "0x54898" for 0xD672 and
// sent a reader hunting a decoder fault that did not exist.
static std::string hex(uint32_t v) {
    std::ostringstream o;
    o << "0x" << std::hex << std::uppercase << v;
    return o.str();
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

// Word addresses as the AA address list prints them, read from the corpus.
constexpr uint16_t A_INTENAR = 0x01C;
constexpr uint16_t A_INTREQR = 0x01E;
constexpr uint16_t A_INTENA  = 0x09A;
constexpr uint16_t A_INTREQ  = 0x09C;

// Bit 15 of the data word selects set (1) or clear (0).
constexpr uint16_t SET   = 0x8000;
constexpr uint16_t CLEAR = 0x0000;

class Harness {
public:
    Harness() : dut(new Vpaula_intcontroller) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->reg_address_in = 0; dut->data_in = 0;
        // Every hardware interrupt input low -- see the header note.
        dut->rxint = 0; dut->txint = 0; dut->vblint = 0;
        dut->int2 = 0; dut->int3 = 0; dut->int6 = 0;
        dut->blckint = 0; dut->syncint = 0; dut->audint = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() {
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
    }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    // Both read paths are combinational on reg_address_in, so the address has
    // to be held while data_out is sampled -- and then released, or the next
    // write lands with two addresses asserted at once.
    uint16_t read(uint16_t word_addr) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    Vpaula_intcontroller* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    // Per-assertion mutation selector: MUTATE names ONE assertion's own
    // label, and only that assertion's stimulus site changes.
    //
    // NO GUARD SWAPS A PAIR SYMMETRICALLY. The Copper package learned this the
    // expensive way: exchanging both halves of a mirrored pair writes both and
    // the value arrives by the other route, leaving four of six assertions
    // green under mutation. Here INTENA/INTENAR and INTREQ/INTREQR are exactly
    // such a mirror, so each guard breaks one side and leaves the other alone.
    //
    // EVERY PATTERN HAS BITS SET BELOW 15 AND IS DISTINCT FROM EVERY OTHER, so
    // a misdirected write cannot land the expected value by accident and a
    // register that keeps its reset value of zero is visibly wrong.
    //
    // INTENA-CLEARS-OR-SETS and INTREQ-CLEARS-OR-SETS used to share one
    // `m_clear` flag (kept as SET vs CLEAR by the boolean switch); split into
    // m_clear_ena and m_clear_req so each assertion's own id selects only its
    // own second write and cannot disturb the other's.
    const uint16_t m_ena_w    = mutating("INTENA-AT-09A") ? A_INTREQ : A_INTENA;      // 1
    const uint16_t m_ena_not  = mutating("INTENA-NOT-AT-09C") ? A_INTENA : A_INTREQ;  // 2
    const uint16_t m_enar_r   = mutating("INTENAR-AT-01C") ? A_INTREQR : A_INTENAR;   // 3
    const uint16_t m_req_w    = mutating("INTREQ-AT-09C") ? A_INTENA : A_INTREQ;      // 4
    const uint16_t m_reqr_r   = mutating("INTREQR-AT-01E") ? A_INTENAR : A_INTREQR;   // 5
    const uint16_t m_clear_ena = mutating("INTENA-CLEARS-OR-SETS") ? SET : CLEAR;     // 6
    const uint16_t m_clear_req = mutating("INTREQ-CLEARS-OR-SETS") ? SET : CLEAR;     // 7
    const uint16_t m_ro_ena   = mutating("INTENAR-IS-READ-ONLY") ? A_INTENA : A_INTENAR; // 8
    const uint16_t m_ro_req   = mutating("INTREQR-IS-READ-ONLY") ? A_INTREQ : A_INTREQR; // 9
    const uint16_t m_live     = mutating("FIXTURE-DELIVERS-KNOWN-VALUE")
                                     ? A_INTREQ : A_INTENA;                            // liveness

    c.record("oracle: AA specification, PDF pp. 6-7, the address-ordered "
             "register list. INTENAR $01C R P, INTREQR $01E R P, INTENA $09A "
             "W P, INTREQ $09C W P [no page -- the package's own oracle line]");

    // The chip column is not observable here and is recorded rather than
    // asserted. This module IS inside Paula by where its file sits, which is a
    // fact about the source tree and not a behaviour any stimulus can make
    // this boundary reveal. Citing the uid on a record() line is what marks
    // the claim DECLINED at this boundary rather than covered by it.
    c.record("DECLINED at this boundary: the list's chip column gives all four "
             "registers to Paula. paula_intcontroller is a Paula module by "
             "where its source sits, which no stimulus at this boundary can "
             "show [claims: sol:AA-intenar.c3 sol:AA-intreqr.c3 sol:AA-intena.c3 "
             "sol:AA-intreq.c3]");
    c.record("DECLINED at this boundary: the list marks INTENAR and INTREQR "
             "with `~`, which the p.6 legend defines as not writable by the "
             "Coprocessor unless COPCON bit 1 is set. That restriction is "
             "enforced in agnus_copper and is graded by aa-copper-register-map "
             "[claims: sol:AA-intenar.c4 sol:AA-intreqr.c4]");

    // --- FIXTURE LIVENESS, before any address assertion leans on it --------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: MUTATE=<id>
        // writes the pattern at $09C, so intena keeps its reset value of
        // zero. MEASURED (WSL/Verilator 5.052, run alone against this id):
        // FAIL FIXTURE-DELIVERS-KNOWN-VALUE, all nine other assertions
        // PASS, summary "9 passed, 1 failed" (baseline "10 passed, 0
        // failed"). Only the named assertion moved.
        h.write(m_live, SET | 0x2AAA);
        const uint16_t got = h.read(A_INTENAR);
        c.note("a $2AAA pattern set through INTENA and read at $01C arrives as "
               + hex(got) + ", expected " + hex(0x2AAA));
        c.expect(got == 0x2AAA,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a write at one address and a "
                 "read at another does move a distinct pattern through this "
                 "module, so a later assertion that reads nothing means the "
                 "decoder and not the fixture [no page -- infrastructure]");
    }

    // --- 1. INTENA is decoded at $09A --------------------------------------
    {
        Harness h;
        // MUTATION INTENA-AT-09A stimulus red: MUTATE=<id> writes at $09C,
        // so the enable register is never touched and reads back zero.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // INTENA-AT-09A, all nine other assertions PASS, summary "9 passed,
        // 1 failed". Only the named assertion moved.
        h.write(m_ena_w, SET | 0x1234);
        const uint16_t got = h.read(A_INTENAR);
        c.note("set $1234 at $09A, read $01C -> " + hex(got));
        c.expect(got == 0x1234,
                 "INTENA-AT-09A: a set-write at $09A reaches the interrupt "
                 "enable register, which is the address the AA list prints for "
                 "INTENA [p.7] [claims: sol:AA-intena.c1 opus:AA-intena.c1 sol:AA-intena.c2 opus:AA-intena.c2]");
    }

    // --- 2. a write at the neighbouring address does NOT reach intena ------
    {
        Harness h;
        // MUTATION INTENA-NOT-AT-09C stimulus red: MUTATE=<id> sends the
        // write to $09A after all, so the enable register does change and
        // the assertion's "unchanged" reading fails. MEASURED (WSL/
        // Verilator 5.052, run alone against this id): FAIL INTENA-NOT-AT-
        // 09C, all nine other assertions PASS, summary "9 passed, 1
        // failed". Only the named assertion moved.
        h.write(m_ena_not, SET | 0x0555);
        const uint16_t got = h.read(A_INTENAR);
        c.note("set $0555 at $09C, read $01C -> " + hex(got) + ", expected 0x0");
        c.expect(got == 0x0000,
                 "INTENA-NOT-AT-09C: a set-write at $09C leaves the interrupt "
                 "enable register alone, so $09A names it and the next address "
                 "up does not [p.7] [claims: sol:AA-intena.c1 opus:AA-intena.c1]");
    }

    // --- 3. INTENAR is readable at $01C and nowhere else --------------------
    {
        Harness h;
        h.write(A_INTENA, SET | 0x0F0F);
        // MUTATION INTENAR-AT-01C stimulus red: MUTATE=<id> reads at $01E,
        // which returns the request register -- zero -- instead. MEASURED
        // (WSL/Verilator 5.052, run alone against this id): FAIL INTENAR-
        // AT-01C, all nine other assertions PASS, summary "9 passed, 1
        // failed". Only the named assertion moved.
        const uint16_t got = h.read(m_enar_r);
        const uint16_t elsewhere = h.read(0x040);
        c.note("enable register read at $01C -> " + hex(got)
               + ", at an unrelated address -> " + hex(elsewhere));
        c.expect(got == 0x0F0F && elsewhere == 0x0000,
                 "INTENAR-AT-01C: the interrupt enable register reads back at "
                 "$01C and at no unrelated address, which is the address and "
                 "the R access the AA list prints for INTENAR [p.6] [claims: "
                 "sol:AA-intenar.c1 sol:AA-intenar.c2 opus:AA-intenar.c1]");
    }

    // --- 4. INTREQ is decoded at $09C --------------------------------------
    {
        Harness h;
        // MUTATION INTREQ-AT-09C stimulus red: MUTATE=<id> writes at $09A,
        // so the request register is never touched and reads back zero.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // INTREQ-AT-09C, all nine other assertions PASS, summary "9 passed,
        // 1 failed". Only the named assertion moved.
        h.write(m_req_w, SET | 0x0AAA);
        const uint16_t got = h.read(A_INTREQR);
        c.note("set $0AAA at $09C, read $01E -> " + hex(got));
        c.expect(got == 0x0AAA,
                 "INTREQ-AT-09C: a set-write at $09C reaches the interrupt "
                 "request register, which is the address the AA list prints "
                 "for INTREQ [p.7] [claims: sol:AA-intreq.c1 opus:AA-intreq.c1 sol:AA-intreq.c2 opus:AA-intreq.c2]");
    }

    // --- 5. INTREQR is readable at $01E and nowhere else --------------------
    {
        Harness h;
        h.write(A_INTREQ, SET | 0x1111);
        // MUTATION INTREQR-AT-01E stimulus red: MUTATE=<id> reads at $01C,
        // which returns the enable register -- zero -- instead. MEASURED
        // (WSL/Verilator 5.052, run alone against this id): FAIL INTREQR-
        // AT-01E, all nine other assertions PASS, summary "9 passed, 1
        // failed". Only the named assertion moved.
        const uint16_t got = h.read(m_reqr_r);
        const uint16_t elsewhere = h.read(0x040);
        c.note("request register read at $01E -> " + hex(got)
               + ", at an unrelated address -> " + hex(elsewhere));
        c.expect(got == 0x1111 && elsewhere == 0x0000,
                 "INTREQR-AT-01E: the interrupt request register reads back at "
                 "$01E and at no unrelated address, which is the address and "
                 "the R access the AA list prints for INTREQR [p.6] [claims: "
                 "sol:AA-intreqr.c1 sol:AA-intreqr.c2 opus:AA-intreqr.c1]");
    }

    // --- 6. INTENA clears as well as sets, and bit 15 decides ---------------
    {
        Harness h;
        h.write(A_INTENA, SET | 0x7FFF);
        const uint16_t all = h.read(A_INTENAR);
        // MUTATION INTENA-CLEARS-OR-SETS stimulus red: MUTATE=<id> sends the
        // second write with bit 15 SET, so it ORs the same bits in again and
        // clears nothing. MEASURED (WSL/Verilator 5.052, run alone against
        // this id): FAIL INTENA-CLEARS-OR-SETS, all nine other assertions
        // PASS (INTREQ-CLEARS-OR-SETS included, confirming the split from
        // the shared m_clear left it unaffected), summary "9 passed, 1
        // failed". Only the named assertion moved.
        h.write(A_INTENA, m_clear_ena | 0x0F0F);
        const uint16_t got = h.read(A_INTENAR);
        c.note("after setting $7FFF then writing $0F0F with bit 15 clear, the "
               "enable register reads " + hex(got) + ", expected "
               + hex(0x7FFF & ~0x0F0F) + " (was " + hex(all) + ")");
        c.expect(all == 0x7FFF && got == (uint16_t)(0x7FFF & ~0x0F0F),
                 "INTENA-CLEARS-OR-SETS: bit 15 of the data word selects set "
                 "from clear at $09A, which is what the AA list's function "
                 "column states for INTENA [p.7] [claims: sol:AA-intena.c4 "
                 "opus:AA-intena.c4]");
    }

    // --- 7. INTREQ clears as well as sets -----------------------------------
    {
        Harness h;
        h.write(A_INTREQ, SET | 0x7FFF);
        const uint16_t all = h.read(A_INTREQR);
        // MUTATION INTREQ-CLEARS-OR-SETS stimulus red: MUTATE=<id> sends the
        // second write with bit 15 SET, so nothing is cleared. MEASURED
        // (WSL/Verilator 5.052, run alone against this id): FAIL INTREQ-
        // CLEARS-OR-SETS, all nine other assertions PASS (INTENA-CLEARS-OR-
        // SETS included, confirming the split from the shared m_clear left
        // it unaffected), summary "9 passed, 1 failed". Only the named
        // assertion moved.
        h.write(A_INTREQ, m_clear_req | 0x30C3);
        const uint16_t got = h.read(A_INTREQR);
        c.note("after setting $7FFF then writing $30C3 with bit 15 clear, the "
               "request register reads " + hex(got) + ", expected "
               + hex(0x7FFF & ~0x30C3) + " (was " + hex(all) + ")");
        c.expect(all == 0x7FFF && got == (uint16_t)(0x7FFF & ~0x30C3),
                 "INTREQ-CLEARS-OR-SETS: bit 15 of the data word selects set "
                 "from clear at $09C, which is what the AA list's function "
                 "column states for INTREQ [p.7] [claims: sol:AA-intreq.c4 "
                 "opus:AA-intreq.c4]");
    }

    // --- 8. INTENAR is read-only ---------------------------------------------
    {
        Harness h;
        h.write(A_INTENA, SET | 0x2468);
        // MUTATION INTENAR-IS-READ-ONLY stimulus red: MUTATE=<id> sends the
        // second write to $09A, which DOES change the register, so
        // "unchanged" fails. MEASURED (WSL/Verilator 5.052, run alone
        // against this id): FAIL INTENAR-IS-READ-ONLY, all nine other
        // assertions PASS, summary "9 passed, 1 failed". Only the named
        // assertion moved.
        h.write(m_ro_ena, SET | 0x1357);
        const uint16_t got = h.read(A_INTENAR);
        c.note("after setting $2468 at $09A and writing $1357 at $01C, the "
               "enable register reads " + hex(got) + ", expected " + hex(0x2468));
        c.expect(got == 0x2468,
                 "INTENAR-IS-READ-ONLY: a write at $01C does not change the "
                 "interrupt enable register, which is the R the AA list prints "
                 "for INTENAR [p.6] [claims: sol:AA-intenar.c2 "
                 "opus:AA-intenar.c2]");
    }

    // --- 9. INTREQR is read-only ---------------------------------------------
    {
        Harness h;
        h.write(A_INTREQ, SET | 0x2468);
        // MUTATION INTREQR-IS-READ-ONLY stimulus red: MUTATE=<id> sends the
        // second write to $09C, which DOES change the register. MEASURED
        // (WSL/Verilator 5.052, run alone against this id): FAIL INTREQR-
        // IS-READ-ONLY, all nine other assertions PASS, summary "9 passed,
        // 1 failed". Only the named assertion moved.
        h.write(m_ro_req, SET | 0x1357);
        const uint16_t got = h.read(A_INTREQR);
        c.note("after setting $2468 at $09C and writing $1357 at $01E, the "
               "request register reads " + hex(got) + ", expected " + hex(0x2468));
        c.expect(got == 0x2468,
                 "INTREQR-IS-READ-ONLY: a write at $01E does not change the "
                 "interrupt request register, which is the R the AA list "
                 "prints for INTREQR [p.6] [claims: sol:AA-intreqr.c2 "
                 "opus:AA-intreqr.c2]");
    }

    // Measured, not graded: the read paths force bit 15 low. The address list
    // says nothing about reading bit 15 back, so there is no claim to grade
    // and no finding to file -- it is here so the next reader of this file
    // does not have to rediscover why every expected value is 15 bits.
    {
        Harness h;
        h.write(A_INTENA, SET | 0x7FFF);
        c.note("bit 15 of the enable read-back is "
               + std::to_string((h.read(A_INTENAR) >> 15) & 1)
               + " after setting every writable bit; both read paths are "
                 "declared {1'b0, reg[14:0]}");
    }

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
