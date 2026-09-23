// aa-interrupt-entry-bits -- paula_intcontroller conformance test
//
// Test purpose: the AA specification names the three interrupt registers
//   twice -- once in the address-ordered list on p.7 and once as alphabetical
//   entries on p.24 -- and the entries add prose the list does not carry:
//   that a write clears or sets bits rather than replacing them, and that
//   loading an arbitrary word therefore takes two writes. This grades what
//   the ENTRIES say.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 24, the INTENA, INTREQ and
//   INTREQR entries. The document prints no page numbers of its own; the
//   corpus numbering is the PDF's.
// Non-goals: the interrupt lines themselves -- that an enabled request raises
//   _int2 to _int6 is about ports this package does not sample -- and any
//   comparison between the two registers' bit maps.
//
// WHY THIS IS A SECOND PACKAGE ON A MODULE THAT ALREADY HAS ONE.
//   `aa/aa-interrupt-register-map` grades the same module from slice 7's
//   address-ordered list, and its assertions and this one's overlap in what
//   they measure. They cannot be one package, because a uid is
//   `<reader>:<id>` and each reader numbers its claims per register WITHIN
//   ITS SLICE: `sol:AA-intena.c2` is the address list's access column on p.7
//   in slice 7 and the alphabetical entry's access column on p.24 here. One
//   string, two claims. check_uid_unambiguous.py measured 948 such uids
//   across the six unions, and not one of them names the same claim twice.
//   Citing both from one package would print a uid the ledger cannot resolve.
//
// WHAT THE MODULE DOES. Bit 15 of the written word decides the direction and
//   the remaining fifteen bits are a mask:
//
//     if (data_in[15]) intena[14:0] <= intena[14:0] |  data_in[14:0];
//     else             intena[14:0] <= intena[14:0] & ~data_in[14:0];
//
//   so a write neither replaces the register nor ignores the bits it does
//   not name -- which is exactly the entry's "clears or sets", and exactly
//   why an arbitrary word needs one write of each kind.

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

constexpr uint16_t A_INTENAR = 0x01C;
constexpr uint16_t A_INTREQR = 0x01E;
constexpr uint16_t A_INTENA  = 0x09A;
constexpr uint16_t A_INTREQ  = 0x09C;

constexpr uint16_t SET   = 0x8000;   // bit 15 high: the mask sets
constexpr uint16_t CLEAR = 0x0000;   // bit 15 low: the mask clears

class Harness {
public:
    Harness() : dut(new Vpaula_intcontroller) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->reg_address_in = 0; dut->data_in = 0;
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

    // Both read paths are combinational on reg_address_in, so the address is
    // held while data_out is sampled and released straight after.
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
    const uint16_t m_ena  = mutating("INTENA-ENTRY-IS-AT-09A") ? A_INTREQ  : A_INTENA;    // 2
    const uint16_t m_req  = mutating("INTREQ-ENTRY-IS-AT-09C") ? A_INTENA  : A_INTREQ;    // 3
    const uint16_t m_reqr = mutating("INTREQR-ENTRY-IS-AT-01E") ? A_INTENAR : A_INTREQR;  // 4
    const uint16_t m_set  = mutating("SET-CLR-DECIDES-WHICH-WAY-A-WRITE-LANDS") ? CLEAR : SET; // 5
    const uint16_t m_two  = mutating("LOADING-A-WORD-TAKES-A-SET-AND-A-CLEAR") ? SET : CLEAR;  // 6
    const uint16_t m_wr   = mutating("INTREQR-ENTRY-TAKES-NO-WRITES") ? A_INTREQ : A_INTREQR;  // 7
    const uint16_t m_live = mutating("FIXTURE-REACHES-THE-ENABLE-REGISTER")
                                 ? (uint16_t)0x0000u : (uint16_t)0x8000u;  // 1

    constexpr uint16_t MASK_A = 0x0140;   // two bits, well apart
    constexpr uint16_t MASK_B = 0x2002;   // two others, one of them high

    std::cout << "aa-interrupt-entry-bits -- AA_CONFORMANCE at "
                 "paula_intcontroller\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, "
                      "every other assertion must keep its baseline verdict\n";

    // --- 1. The fixture reaches the enable register -------------------------
    {
        Harness h;
        // MUTATION FIXTURE-REACHES-THE-ENABLE-REGISTER stimulus red:
        // MUTATE=<id> drops bit 15, so the same mask clears bits that were
        // already clear and the write leaves no trace to find. MEASURED
        // (WSL/Verilator 5.052, run alone against this id): FAIL FIXTURE-
        // REACHES-THE-ENABLE-REGISTER, all six other assertions PASS,
        // summary "6 passed, 1 failed" (baseline "7 passed, 0 failed").
        // Only the named assertion moved.
        h.write(A_INTENA, (uint16_t)(m_live | MASK_A));
        const uint16_t got = h.read(A_INTENAR);
        c.note("after one INTENA write of " + hex(m_live | MASK_A) +
               ", INTENAR reads " + hex(got));
        c.expect((got & MASK_A) == MASK_A,
                 "FIXTURE-REACHES-THE-ENABLE-REGISTER: a write at $09A shows "
                 "up at $01C, so every assertion below is against a register "
                 "this fixture wrote [no page -- infrastructure]");
    }

    // --- 2. INTENA is the register at $09A ----------------------------------
    {
        Harness h;
        // MUTATION INTENA-ENTRY-IS-AT-09A stimulus red: MUTATE=<id> writes
        // the request register's address instead, which leaves the enable
        // register clear. MEASURED (WSL/Verilator 5.052, run alone against
        // this id): FAIL INTENA-ENTRY-IS-AT-09A, all six other assertions
        // PASS, summary "6 passed, 1 failed". Only the named assertion
        // moved.
        h.write(m_ena, (uint16_t)(SET | MASK_B));
        const uint16_t got = h.read(A_INTENAR);
        c.note("INTENAR reads " + hex(got) + " after the write");
        c.expect((got & MASK_B) == MASK_B,
                 "INTENA-ENTRY-IS-AT-09A: the address the entry gives INTENA "
                 "is the one whose writes reach the enable register [p.24] "
                 "[claims: opus:AA-intena.c1 sol:AA-intena.c1]");
    }

    // --- 3. INTREQ is the register at $09C ----------------------------------
    {
        Harness h;
        // MUTATION INTREQ-ENTRY-IS-AT-09C stimulus red: MUTATE=<id> writes
        // the enable register's address instead, so the request register
        // keeps its reset value. MEASURED (WSL/Verilator 5.052, run alone
        // against this id): FAIL INTREQ-ENTRY-IS-AT-09C, all six other
        // assertions PASS, summary "6 passed, 1 failed". Only the named
        // assertion moved.
        h.write(m_req, (uint16_t)(SET | MASK_A));
        const uint16_t got = h.read(A_INTREQR);
        c.note("INTREQR reads " + hex(got) + " after the write");
        c.expect((got & MASK_A) == MASK_A,
                 "INTREQ-ENTRY-IS-AT-09C: the address the entry gives INTREQ "
                 "is the one whose writes reach the request register [p.24] "
                 "[claims: opus:AA-intreq.c1 sol:AA-interrupts.c1/c2 sol:AA-intreq.c1]");
    }

    // --- 4. INTREQR is the register at $01E ---------------------------------
    {
        Harness h;
        h.write(A_INTREQ, (uint16_t)(SET | MASK_B));
        // MUTATION INTREQR-ENTRY-IS-AT-01E stimulus red: MUTATE=<id> reads
        // the enable register's reader instead, which was never written.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // INTREQR-ENTRY-IS-AT-01E, all six other assertions PASS, summary
        // "6 passed, 1 failed". Only the named assertion moved.
        const uint16_t got = h.read(m_reqr);
        c.note("the address the entry gives INTREQR reads " + hex(got));
        c.expect((got & MASK_B) == MASK_B,
                 "INTREQR-ENTRY-IS-AT-01E: the address the entry gives INTREQR "
                 "is the one that reads back what a write to INTREQ put there "
                 "[p.24] [claims: opus:AA-intreqr.c1/c5 sol:AA-intreqr.c1/c2]");
    }

    // --- 5. A write sets or clears, and does not replace --------------------
    {
        Harness h;
        h.write(A_INTENA, (uint16_t)(SET | MASK_A));
        // MUTATION SET-CLR-DECIDES-WHICH-WAY-A-WRITE-LANDS stimulus red:
        // MUTATE=<id> clears bit 15 in the second write, so the second mask
        // clears instead of setting and the two bits never stand together.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // SET-CLR-DECIDES-WHICH-WAY-A-WRITE-LANDS, all six other assertions
        // PASS, summary "6 passed, 1 failed". Only the named assertion
        // moved.
        h.write(A_INTENA, (uint16_t)(m_set | MASK_B));
        const uint16_t both = h.read(A_INTENAR);
        h.write(A_INTENA, (uint16_t)(CLEAR | MASK_A));
        const uint16_t left = h.read(A_INTENAR);
        c.note("two set writes leave " + hex(both) + "; clearing the first "
               "mask leaves " + hex(left));
        c.expect((both & (MASK_A | MASK_B)) == (MASK_A | MASK_B) &&
                 (left & MASK_A) == 0 && (left & MASK_B) == MASK_B,
                 "SET-CLR-DECIDES-WHICH-WAY-A-WRITE-LANDS: a write to the "
                 "enable register sets or clears the bits its mask names and "
                 "leaves the rest standing, which is what the entry means by "
                 "clearing or setting rather than loading [p.24] "
                 "[claims: opus:AA-intena.c4]");
    }

    // --- 6. So an arbitrary word takes two writes ---------------------------
    {
        Harness h;
        h.write(A_INTREQ, (uint16_t)(SET | 0x7FFF));      // everything on
        const uint16_t one = h.read(A_INTREQR);
        // MUTATION LOADING-A-WORD-TAKES-A-SET-AND-A-CLEAR stimulus red:
        // MUTATE=<id> makes the second write another SET, so the bits that
        // should have been cleared stay standing and one write looks enough.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // LOADING-A-WORD-TAKES-A-SET-AND-A-CLEAR, all six other assertions
        // PASS, summary "6 passed, 1 failed". Only the named assertion
        // moved.
        h.write(A_INTREQ, (uint16_t)(m_two | (uint16_t)(~MASK_A & 0x7FFF)));
        const uint16_t two = h.read(A_INTREQR);
        c.note("after one set-everything write INTREQR reads " + hex(one) +
               "; after the clearing write that follows it reads " + hex(two));
        c.expect((one & 0x7FFF) != (MASK_A & 0x7FFF) &&
                 (two & 0x7FFF) == (MASK_A & 0x7FFF),
                 "LOADING-A-WORD-TAKES-A-SET-AND-A-CLEAR: no single write "
                 "leaves the request register holding an arbitrary word -- it "
                 "took one write to set and one to clear, exactly as the entry "
                 "says [p.24] [claims: opus:AA-intreq.c4 opus:AA-intreqr.c7 sol:AA-interrupts.c4]");
    }

    // --- 7. And the reader takes no writes ----------------------------------
    {
        Harness h;
        h.write(A_INTREQ, (uint16_t)(SET | MASK_A));
        const uint16_t before = h.read(A_INTREQR);
        // MUTATION INTREQR-ENTRY-TAKES-NO-WRITES stimulus red: MUTATE=<id>
        // aims the clearing write at INTREQ itself, which does change the
        // register, so a writable reader would look the same as this.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // INTREQR-ENTRY-TAKES-NO-WRITES, all six other assertions PASS,
        // summary "6 passed, 1 failed". Only the named assertion moved.
        h.write(m_wr, (uint16_t)(CLEAR | MASK_A));
        const uint16_t after = h.read(A_INTREQR);
        c.note("INTREQR reads " + hex(before) + " before a write to its own "
               "address and " + hex(after) + " after");
        c.expect(before == after && (before & MASK_A) == MASK_A,
                 "INTREQR-ENTRY-TAKES-NO-WRITES: the address the entry marks "
                 "read-only does not change the register when written, so the "
                 "R in its access column is the register's behaviour and not "
                 "only the document's notation [p.24] "
                 "[claims: opus:AA-intreqr.c2]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about a "
             "request bit with its enable set raising a processor interrupt. The "
             "int lines are ports of this module, but no assertion in this "
             "package samples them; it grades where the four registers are "
             "decoded and how a write to them lands. [claims: opus:AA-intreqr.c6 "
             "sol:AA-interrupts.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is that the "
             "register is write-only. The package grades the two READABLE "
             "registers being read-only, which is a reachable measurement; that "
             "a write-only address answers no read is the same silence any "
             "undecoded address gives. [claims: opus:AA-intena.c2 "
             "opus:AA-intreq.c2 sol:AA-intena.c2 sol:AA-intreq.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is that the "
             "request register's bit assignments are identical to the enable "
             "register's. Nothing here compares the two bit maps against each "
             "other; the assertions write one register and read its own reader. "
             "[claims: opus:AA-intreqr.c8 sol:AA-interrupts.c5]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of paula_intcontroller "
             "carries a chip identity. [claims: opus:AA-intena.c3 "
             "opus:AA-intreq.c3 opus:AA-intreqr.c3 sol:AA-intena.c3 "
             "sol:AA-intreq.c3 sol:AA-intreqr.c3]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- the ~ mark and where it is and is not printed, or a "
             "conflict between two of its statements. [claims: "
             "opus:AA-intreqr.c4 sol:AA-intreqr.c4]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
