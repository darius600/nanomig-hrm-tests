// aa-clxcon2-address -- denise_collision conformance test
//
// Test purpose: where CLXCON2 is decoded, and what clears it. The AA
//   specification states CLXCON2's address TWICE AND DIFFERENTLY -- its
//   address-ordered register list (PDF p. 8) gives $10E, its alphabetical
//   entry (PDF p. 18) gives $10C. $10C is BPLCON4's address, printed as such
//   on the same document's pp. 8 and 16. This package measures which one the
//   RTL implements, and grades the two further CLXCON2 statements the
//   document makes: that it is gated on AGA, and that a write to CLXCON
//   clears it.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 4, 8, 18. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the collision matrix itself -- which
//   sprite-group and bitplane pairs set which CLXDAT bit -- nor CLXDAT's
//   read-and-clear behaviour, nor CLXCON. It reaches CLXCON2 only through
//   the one effect that makes it observable at this boundary: bitplanes 7
//   and 8 joining the bitplane match.
//
// HOW CLXCON2 IS OBSERVED. The module exposes no read path for it. It reaches
//   the outside only through the bitplane match:
//
//     bm = (bpldata ^ ~{clxcon2[1:0], clxcon[5:0]}) | ~{clxcon2[7:6], clxcon[11:6]}
//
//   clxcon2[7:6] are the enables for bitplanes 7 and 8, clxcon2[1:0] their
//   match values. With clxcon2 zero, the two top enables are zero, `~` forces
//   bm[7:6] high, and bit 7 of bpldata cannot affect a collision. With
//   clxcon2 = $00C3 -- both enabled, both matching 1 -- it can. That
//   difference is the measurement.
//
// THE ORACLE DISAGREES WITH ITSELF AND THIS PACKAGE DOES NOT RESOLVE IT.
//   A red here would not mean the RTL is wrong; it would mean the RTL follows
//   the other half of the document. Commodore's own custom.h 39.1 -- the
//   shipping-era header, a year after this PRELIMINARY text -- puts CLXCON2 at
//   $10E and BPLCON4 at $10C, which is evidence about the document, not about
//   NanoMig. What is asked of a reader is in the finding, not here.

#include "Vdenise_collision.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
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

// Word addresses as the module sees them: reg_address_in is [8:1].
constexpr uint16_t A_CLXDAT  = 0x00E;
constexpr uint16_t A_CLXCON  = 0x098;
constexpr uint16_t A_CLXCON2 = 0x10E;   // the address list, p. 8
constexpr uint16_t A_BPLCON4 = 0x10C;   // the alphabetical entry's address, p. 18

class Harness {
public:
    explicit Harness(bool aga) : dut(new Vdenise_collision) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->aga = aga ? 1 : 0;
        dut->reg_address_in = 0; dut->data_in = 0;
        dut->dblpf = 0; dut->bpldata = 0; dut->nsprite = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
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

    // Drive bitplane data and report whether an odd-to-even collision latched.
    bool collides(uint8_t bpldata) {
        dut->bpldata = bpldata;
        dut->reg_address_in = 0;
        step();
        dut->reg_address_in = (A_CLXDAT >> 1) & 0xFF;
        dut->eval();
        const bool cl0 = (dut->data_out & 0x0001) != 0;
        // Reading CLXDAT clears it on the cycle after the read ends -- when
        // clxdat_read has gone low and its delayed copy is still high. That is
        // what makes each measurement independent of the one before, and it
        // only works if the stimulus is removed FIRST: with bpldata still
        // driven, the two settling cycles after the clear re-latch the very
        // collision that was just cleared, and every later measurement reads
        // the first one's result. Measured 21.09.2026 -- it made a correct RTL
        // look as though a write to $10E did nothing.
        step();
        dut->reg_address_in = 0;
        dut->bpldata = 0;
        step(); step();
        return cl0;
    }

    Vdenise_collision* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() below is guarded so that MUTATE=1 forces it
    // red, and the guard never touches the comparison itself -- it changes
    // the address written, which is exactly the proposition under test.
    const uint16_t addr_under_test = mutate ? A_BPLCON4 : A_CLXCON2;

    c.record("oracle: AA specification, PDF p. 8 states CLXCON2 at $10E; "
             "PDF p. 18 states the same register at $10C, which p. 8 and "
             "p. 16 both give to BPLCON4 [AA-clxcon2.c1]");

    // --- 1. Is CLXCON2 decoded where the address list says it is?
    {
        Harness h(true);
        const bool before = h.collides(0x7F);   // bit 7 low
        h.write(addr_under_test, 0x00C3);       // enable and match planes 7,8
        const bool after = h.collides(0x7F);
        c.note(std::string("with clxcon2 unwritten, bpldata=0x7F collides: ")
               + (before ? "yes" : "no"));
        c.note(std::string("after writing $00C3 to $") +
               (mutate ? "10C" : "10E") + ", bpldata=0x7F collides: "
               + (after ? "yes" : "no"));
        c.expect(before && !after,
                 "a write to $10E changes the bitplane match, so CLXCON2 is "
                 "decoded at $10E as the address list states [AA-clxcon2.c1]");
    }

    // --- 2. The other address the document gives, as an inverse control.
    {
        Harness h(true);
        const bool before = h.collides(0x7F);
        h.write(A_BPLCON4, 0x00C3);
        const bool after = h.collides(0x7F);
        c.expect(before && after,
                 "a write to $10C does NOT change the bitplane match, so the "
                 "alphabetical entry's address is not where CLXCON2 lives "
                 "[AA-clxcon2.c2]");
    }

    // --- 3. "CLXCON2 is reset by a write to CLXCON" -- PDF p. 4, verbatim.
    {
        Harness h(true);
        // PRIME THE HARNESS. clxdat has no reset in the RTL and it accumulates
        // every cycle it is not being cleared, so the first measurement in a
        // fresh harness reads whatever has gathered since construction. One
        // discarded measurement establishes the read-clear cycle. Without it
        // this section graded red against a correct RTL -- see the note on
        // collides() above; both were the harness, not the module.
        (void)h.collides(0x7F);
        h.write(A_CLXCON2, 0x00C3);
        const bool armed = h.collides(0x7F);
        h.write(A_CLXCON, 0x0FFF);
        const bool after_clxcon = h.collides(0x7F);
        c.note(std::string("with CLXCON2 armed, bpldata=0x7F collides: ")
               + (armed ? "yes" : "no"));
        c.note(std::string("after a CLXCON write, bpldata=0x7F collides: ")
               + (after_clxcon ? "yes" : "no"));
        c.expect(!armed && after_clxcon,
                 "a write to CLXCON clears CLXCON2, as p. 4 states "
                 "[AA-clxcon2.c3]");
    }

    // --- 4. The RTL gates CLXCON2 on AGA. The document does not say this.
    {
        Harness h(false);
        const bool before = h.collides(0x7F);
        h.write(A_CLXCON2, 0x00C3);
        const bool after = h.collides(0x7F);
        c.record("the AA document states no chipset gate on CLXCON2 writes; "
                 "this is measured from the RTL, not graded against the page "
                 "[AA-clxcon2.n1]");
        c.note(std::string("with aga=0, a write to $10E changes the match: ")
               + ((before != after) ? "yes" : "no"));
    }

    std::cout << "\naa-clxcon2-address: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed ? 1 : 0;
}
