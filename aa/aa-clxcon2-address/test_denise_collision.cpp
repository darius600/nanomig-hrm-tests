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
// $010 is ADKCONR's address on the same list -- a real neighbour, not an
// invented one, so the inverse control asks about a word the document defines.
constexpr uint16_t A_NEIGHBOUR = 0x010;
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
    // red, and no guard touches the comparison itself -- each swaps the ADDRESS
    // WRITTEN, which is the proposition under test in all three cases.
    //
    // Measured 21.09.2026: the first version of this file guarded only the
    // first assertion, and `make mutate` came back "2 passed, 1 failed". A
    // mutation run that leaves assertions green is not a weaker gate, it is a
    // gate that has already failed -- those two were not shown capable of going
    // red at all. One guard per assertion, and `make mutate` must report
    // 0 passed.
    const uint16_t addr_under_test = mutate ? A_BPLCON4 : A_CLXCON2;
    // Section 2 is the inverse control: it writes the address the document's
    // other half gives and expects nothing to happen. Swapping it for the
    // address that DOES decode makes something happen, and the control fails.
    const uint16_t control_addr    = mutate ? A_CLXCON2 : A_BPLCON4;
    // Section 3 clears CLXCON2 with a CLXCON write. Writing CLXDAT instead
    // clears nothing, so the register stays armed and the assertion fails.
    // CLXDAT is a read-and-clear data register, not a control one, so this is
    // a write the module accepts and ignores rather than an invalid address.
    const uint16_t clearing_addr   = mutate ? A_CLXDAT : A_CLXCON;
    // Section 5 reads one register away; section 6 writes one register away.
    const uint16_t A_CLXDAT_UT     = mutate ? A_NEIGHBOUR : A_CLXDAT;
    const uint16_t A_CLXCON_UT     = mutate ? 0x09A       : A_CLXCON;

    // No claim id on this line. A uid on a RECORDED line means the claim was
    // declined at this boundary, and the coverage ledger reads it that way --
    // it reported opus:AA-clxcon2.c1 as asserted AND declined at once, because
    // this record carried the same id the assertion below grades. The oracle
    // statement belongs here; the claim it supports is cited where it is
    // graded, and nowhere else.
    c.record("oracle: AA specification, PDF p. 8 states CLXCON2 at $10E; "
             "PDF p. 18 states the same register at $10C, which p. 8 and "
             "p. 16 both give to BPLCON4");

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
        // MUTATION CLXCON2-AT-10E stimulus red MUTATE=1 writes $10C instead
        // of $10E, so the bitplane match does not change and the assertion
        // that CLXCON2 decodes at $10E goes red.
        c.expect(before && !after,
                 "CLXCON2-AT-10E: a write to $10E changes the bitplane match, "
                 "so CLXCON2 is decoded at $10E as the address list states "
                 "[claims: opus:AA-clxcon2.c1]");
    }

    // --- 2. The other address the document gives, as an inverse control.
    {
        Harness h(true);
        const bool before = h.collides(0x7F);
        h.write(control_addr, 0x00C3);
        const bool after = h.collides(0x7F);
        // MUTATION CLXCON2-NOT-AT-10C stimulus red MUTATE=1 writes $10E
        // instead of $10C, so the inverse control does see a change and goes
        // red -- the control is only a control while $10C is inert.
        c.expect(before && after,
                 "CLXCON2-NOT-AT-10C: a write to $10C does NOT change the "
                 "bitplane match, so the alphabetical entry's address is not "
                 "where CLXCON2 lives [claims: opus:AA-clxcon2.c2]");
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
        h.write(clearing_addr, 0x0FFF);
        const bool after_clxcon = h.collides(0x7F);
        c.note(std::string("with CLXCON2 armed, bpldata=0x7F collides: ")
               + (armed ? "yes" : "no"));
        c.note(std::string("after a CLXCON write, bpldata=0x7F collides: ")
               + (after_clxcon ? "yes" : "no"));
        // MUTATION CLXCON2-CLEARED-BY-CLXCON stimulus red MUTATE=1 writes
        // CLXDAT instead of CLXCON. CLXDAT clears nothing in CLXCON2, so the
        // register stays armed and the clearing assertion goes red.
        c.expect(!armed && after_clxcon,
                 "CLXCON2-CLEARED-BY-CLXCON: a write to CLXCON clears CLXCON2, "
                 "as p. 4 states [claims: opus:AA-clxcon2.c3]");
    }

    // --- 5. CLXDAT at $00E, the one address this module answers a READ at.
    {
        Harness h(true);
        (void)h.collides(0x7F);                 // prime, as section 3 explains
        h.dut->bpldata = 0x7F;
        h.step();
        // data_out = reg_address_in==CLXDAT ? {1'b1, clxdat[14:0]} : 16'd0
        // (denise_collision.v:117), so bit 15 is set on a real read and the
        // whole word is zero anywhere else. That makes the address falsifiable
        // without needing to know what collided.
        h.dut->reg_address_in = (A_CLXDAT_UT >> 1) & 0xFF;
        h.dut->eval();
        const uint16_t at_clxdat = h.dut->data_out;
        h.dut->reg_address_in = (A_NEIGHBOUR >> 1) & 0xFF;
        h.dut->eval();
        const uint16_t at_neighbour = h.dut->data_out;
        h.dut->reg_address_in = 0;
        h.step(); h.step();
        c.note("data_out reading at $00E: 0x" + std::to_string(at_clxdat)
               + " (bit 15 " + (((at_clxdat >> 15) & 1) ? "set" : "clear")
               + "), at $010: 0x" + std::to_string(at_neighbour));
        // MUTATION CLXDAT-AT-00E stimulus red MUTATE=1 reads at $010 instead of
        // $00E, so the read that should answer returns the neighbour's silence.
        c.expect(((at_clxdat >> 15) & 1) == 1 && at_neighbour == 0,
                 "CLXDAT-AT-00E: this module answers a read at $00E and is "
                 "silent at $010, so CLXDAT is decoded where the address list "
                 "puts it and the list's `R` is the access it has "
                 "[claims: opus:AA-clxdat.c1]");
    }

    // --- 6. CLXCON at $098, reached through what a write there does.
    {
        Harness h(true);
        (void)h.collides(0x7F);
        h.write(A_CLXCON2, 0x00C3);             // arm planes 7 and 8
        const bool armed = h.collides(0x7F);
        h.write(A_CLXCON_UT, 0x0FFF);
        const bool after = h.collides(0x7F);
        c.note(std::string("with CLXCON2 armed: collides ")
               + (armed ? "yes" : "no") + "; after a write at $098: "
               + (after ? "yes" : "no"));
        // MUTATION CLXCON-AT-098 stimulus red MUTATE=1 writes at $09A instead
        // of $098, so nothing clears and the armed state survives.
        c.expect(!armed && after,
                 "CLXCON-AT-098: a write at $098 clears CLXCON2 and a write at "
                 "$09A does not, so CLXCON is decoded where the address list "
                 "puts it [claims: opus:AA-clxcon.c1]");
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
