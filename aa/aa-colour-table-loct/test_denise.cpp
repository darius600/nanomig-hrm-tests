// aa-colour-table-loct -- denise conformance test
//
// Test purpose: the AA specification's alphabetical entry for COLORxx prints
//   the colour registers TWICE -- once for LOCT=0 and once for LOCT=1 -- and
//   describes two sets of registers selected by one bit. This grades that
//   mechanism, which is the whole of AGA's 24-bit colour. It is the
//   complement of aa-lisa-colour-registers, which grades where the table is
//   decoded and what opens the readback path.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 19 and 20. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test HAM, the sprites, the genlock ZD pin,
//   dual playfield priority, or what any colour looks like on a screen.
//
// WHAT THE ENTRY SAYS, IN ITS OWN TWO ROWS:
//
//     LOCT=0   T X X X  R7 R6 R5 R4  G7 G6 G5 G4  B7 B6 B5 B4
//     LOCT=1   X X X X  R3 R2 R1 R0  G3 G2 G1 G0  B3 B2 B1 B0
//
//   and in prose: `There are actually two sets of color registers, selection
//   of which is controlled by the LOCT register bit`, and -- the sentence
//   this package exists for -- `The low order set of registers is also
//   selected simultaneously` when LOCT is low, `so that this provides
//   compatibility with old software`.
//
//   So a write with LOCT clear must reach BOTH halves, and a write with LOCT
//   set must reach only the low one. That asymmetry is the claim: a
//   twelve-bit program writing the table on an AGA machine gets a colour
//   whose low nibbles match its high ones, rather than whatever was there.
//
// AND THE RTL PUTS IT IN ONE LINE. denise_colortable.v:33 is
//
//     wire [2-1:0] wr_ws = loct ? 2'b01 : 2'b11;
//
//   -- two write strobes with LOCT low, one with it high -- and denise.v:251
//   is `assign loct = bplcon3[9] & aga`, so the bit only exists on AGA. The
//   readback follows the same bit: denise.v:222 hands back the low nibbles
//   when LOCT is set and the high ones when it is clear.
//
// THE CHIP-SET CONDITION ON LOCT CANNOT BE SHOWN HERE, AND THE REASON IS
//   THE READBACK. `loct = bplcon3[9] & aga` is AGA-gated, and so is the
//   only way to see a colour at this boundary: `rdram = bplcon2[8] & aga`.
//   A fixture built without AGA reads zero from every colour address, so
//   the bit's own chip-set condition cannot be separated from the
//   readback's. An assertion that tried measured a high half of 0x0 on the
//   older chip set -- not the value it expected, and not evidence of
//   anything. The lot declines that claim and names this.
//
// READING A COLOUR ADDRESS WITH THE READBACK BIT CLEAR IS A WRITE. That trap
//   cost aa-lisa-colour-registers an assertion: `wr_en` is `address decodes &&
//   clk7_en && !rdram`, and nothing about it asks whether the bus is writing.
//   Every read here sets BPLCON2 bit 8 first.

#include "Vdenise.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

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

constexpr uint16_t A_BPLCON2 = 0x104;
constexpr uint16_t A_BPLCON3 = 0x106;
constexpr uint16_t A_COLOR00 = 0x180;

constexpr uint16_t RDRAM = 1u << 8;      // BPLCON2: read the colour table
constexpr uint16_t LOCT  = 1u << 9;      // BPLCON3: the low-order register set

static uint16_t colour_addr(int n) { return (uint16_t)(A_COLOR00 + 2 * n); }

class Harness {
public:
    explicit Harness(bool aga = true) : dut(new Vdenise) {
        dut->clk = 0; dut->clk7_en = 1;
        dut->aga = aga ? 1 : 0; dut->ecs = 1;
        dut->reg_address_in = 0; dut->data_in = 0;
        for (int i = 0; i < 8; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() { dut->clk = 1; dut->eval(); dut->clk = 0; dut->eval(); }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    uint16_t read(uint16_t word_addr) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        step();                       // the table's output is a clock behind
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = (uint16_t)dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    // Write one colour entry with the readback path CLOSED, which is the only
    // state in which the table takes writes at all.
    void put(int entry, uint16_t value, bool loct) {
        write(A_BPLCON2, 0);                       // rdram low: writes land
        write(A_BPLCON3, loct ? LOCT : 0);
        write(colour_addr(entry), value);
    }

    // Read one colour entry with the readback path OPEN.
    uint16_t get(int entry, bool loct) {
        write(A_BPLCON3, loct ? LOCT : 0);
        write(A_BPLCON2, RDRAM);
        const uint16_t v = read(colour_addr(entry));
        write(A_BPLCON2, 0);
        return v;
    }

    Vdenise* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    const bool mutate = std::getenv("MUTATE") != nullptr;
    Checks c;

    constexpr int ENTRY = 5;
    constexpr uint16_t HIGH_WRITE = 0x0A73;   // what a 12-bit program writes
    constexpr uint16_t LOW_WRITE  = 0x0159;   // a different low-order word

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red.
    const bool     m_both  = mutate;                      // 1: writes low only
    const bool     m_low   = mutate;                      // 2: writes both
    const uint16_t m_other = mutate ? HIGH_WRITE : LOW_WRITE;  // 3: same word

    std::cout << "aa-colour-table-loct -- AA_CONFORMANCE at denise\n";
    std::cout << (mutate ? "MUTATE=1: every assertion below must go red\n" : "");

    // --- 1. A write with LOCT clear reaches both sets -----------------------
    {
        Harness h;
        // MUTATION LOCT-CLEAR-WRITES-BOTH-SETS stimulus red MUTATE=1 makes the
        // write a LOCT=1 one, so only the low set takes it and the high half
        // keeps whatever was there.
        h.put(ENTRY, HIGH_WRITE, m_both);
        const uint16_t high = h.get(ENTRY, false);
        const uint16_t low  = h.get(ENTRY, true);
        c.note("after one write with LOCT clear: the high set reads " +
               hex(high) + ", the low set reads " + hex(low));
        c.expect((high & 0x0FFF) == (HIGH_WRITE & 0x0FFF) &&
                 (low & 0x0FFF) == (HIGH_WRITE & 0x0FFF),
                 "LOCT-CLEAR-WRITES-BOTH-SETS: one write with the bit clear "
                 "leaves the same twelve bits in both halves of the entry, "
                 "which is what the AA entry means by the low order set being "
                 "selected simultaneously for compatibility with old software "
                 "[p.19,20] [claims: opus:AA-colorxx.c8/c9 sol:AA-colorxx.c9/c11]");
    }

    // --- 2. A write with LOCT set reaches only the low set ------------------
    {
        Harness h;
        h.put(ENTRY, HIGH_WRITE, false);           // both halves now agree
        // MUTATION LOCT-SET-WRITES-ONE-SET stimulus red MUTATE=1 makes the
        // second write a LOCT=0 one, so it lands in both and the high half
        // moves too.
        h.put(ENTRY, LOW_WRITE, !m_low);
        const uint16_t high = h.get(ENTRY, false);
        const uint16_t low  = h.get(ENTRY, true);
        c.note("then a write with LOCT set: the high set reads " + hex(high) +
               ", the low set reads " + hex(low));
        c.expect((high & 0x0FFF) == (HIGH_WRITE & 0x0FFF) &&
                 (low & 0x0FFF) == (LOW_WRITE & 0x0FFF),
                 "LOCT-SET-WRITES-ONE-SET: a write with the bit set changes "
                 "the low order half and leaves the high one, which is the "
                 "two sets of colour registers the AA entry describes and the "
                 "bit that selects between them [p.19,20] "
                 "[claims: opus:AA-colorxx.c6/c10 sol:AA-colorxx.c5]");
    }

    // --- 3. The two halves are read back separately --------------------------
    {
        Harness h;
        // MUTATION THE-TWO-HALVES-READ-BACK-SEPARATELY stimulus red
        // MUTATE=1 writes the same word into both halves, so the two
        // reads agree and the entry cannot be shown to be two registers.
        h.put(ENTRY, HIGH_WRITE, false);
        h.put(ENTRY, m_other, true);
        const uint16_t high = h.get(ENTRY, false);
        const uint16_t low  = h.get(ENTRY, true);
        c.note("with the two halves deliberately different, LOCT=0 reads " +
               hex(high) + " and LOCT=1 reads " + hex(low));
        c.expect((high & 0x0FFF) != (low & 0x0FFF),
                 "THE-TWO-HALVES-READ-BACK-SEPARATELY: the same address "
                 "answers with two different words depending on the bit, so "
                 "the entry really is two registers and not one, which is "
                 "what the AA entry's two printed rows say [p.19,20] "
                 "[claims: opus:AA-colorxx.c1/c2/c4/c5/c7 sol:AA-colorxx.c1/c2/c3/c4/c6/c7/c8/c10/c12/c14]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The T bit drives the ZD "
             "pin for genlocks. de [claims: opus:AA-colorxx.c11/c12/c13 "
             "sol:AA-colorxx.c13/c46/c51 opus:AA-colorxx.loct0-15]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the [claims: opus:AA-colorxx.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The row names one bit of "
             "one colour componen [claims: "
             "sol:AA-colorxx.c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c38/c39/c40/c41/c42/c43/c44/c45 "
             "opus:AA-colorxx.loct0-00 opus:AA-colorxx.loct0-01 "
             "opus:AA-colorxx.loct0-02 opus:AA-colorxx.loct0-03 "
             "opus:AA-colorxx.loct0-04 opus:AA-colorxx.loct0-05 "
             "opus:AA-colorxx.loct0-06 opus:AA-colorxx.loct0-07 "
             "opus:AA-colorxx.loct0-08 opus:AA-colorxx.loct0-09 "
             "opus:AA-colorxx.loct0-10 opus:AA-colorxx.loct0-11 "
             "opus:AA-colorxx.loct0-12 opus:AA-colorxx.loct0-13 "
             "opus:AA-colorxx.loct0-14 opus:AA-colorxx.loct1-00 "
             "opus:AA-colorxx.loct1-01 opus:AA-colorxx.loct1-02 "
             "opus:AA-colorxx.loct1-03 opus:AA-colorxx.loct1-04 "
             "opus:AA-colorxx.loct1-05 opus:AA-colorxx.loct1-06 "
             "opus:AA-colorxx.loct1-07 opus:AA-colorxx.loct1-08 "
             "opus:AA-colorxx.loct1-09 opus:AA-colorxx.loct1-10 "
             "opus:AA-colorxx.loct1-11 opus:AA-colorxx.loct1-12 "
             "opus:AA-colorxx.loct1-13 opus:AA-colorxx.loct1-14 "
             "opus:AA-colorxx.loct1-15]");
    c.record(
             "DECLINED at this boundary (not-hardware): The line explains the "
             "document's own column [claims: sol:AA-colorxx.c47/c48/c49/c50]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
