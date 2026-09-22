// aa-collision-data-register -- denise_collision conformance test
//
// Test purpose: the AA specification's alphabetical entry for CLXDAT gives it
//   an address, calls it `Collision data reg.(Read and clear)`, and prints a
//   sixteen-row table naming which pair of objects each bit reports. This
//   grades the address, the read-and-clear, and the bits the module computes.
//   It is the complement of aa-clxcon2-address, which grades CLXCON2.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 19 and 20. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the sprite pipeline, the bitplane
//   shifters, dual-playfield priority, or what the collisions mean on screen.
//
// READ AND CLEAR IS THE ENTRY'S OWN PHRASE AND IT IS TWO CLAIMS. The address
//   answers with the accumulated collisions, and the register goes to zero
//   AFTER the read -- denise_collision.v:107-109 clears on the falling edge of
//   the read, not during it, so the word the reader gets is the one that was
//   there. Assertion 3 reads twice and shows the second read empty.
//
// BIT 15 READS AS ONE AND THE DOCUMENT DOES NOT FORBID IT. The table's row 15
//   says `not used`, and denise_collision.v:117 drives that position from a
//   constant: `data_out = ... ? {1'b1, clxdat[14:0]} : 16'd0`. `Not used` is
//   not `reads zero`, so this is not a divergence. NM-HRM-009 raised the same
//   bit against the Hardware Reference Manual and was WITHDRAWN when all four
//   implementations turned out to set it. This package states the value it
//   measures and claims nothing about what it ought to be.
//
// CLXCON RESETS TO $0FFF AND THAT DECIDES THE STIMULUS. `bm = (bpldata ^
//   ~{clxcon2[1:0],clxcon[5:0]}) | (~{clxcon2[7:6],clxcon[11:6]})`, so with the
//   reset value the odd match wants bpldata bits 0, 2 and 4 and the even match
//   bits 1, 3 and 5. $3F satisfies both, $15 the odd half alone. This fixture
//   holds `dblpf` high so the odd match does not additionally wait on the even
//   one, which is the module's own dual-playfield term.
//
// THE PAIRS ARE COMPUTED FROM TWO PORTS. `oddmatch` and `evenmatch` come from
//   `bpldata`, `sprmatch[n]` from `nsprite`, and cl[1] to cl[4] are odd
//   bitplanes against sprite pairs 0-1 to 6-7 while cl[5] to cl[8] are the
//   even ones -- which is exactly the order the table prints for Playfield 1
//   and Playfield 2. Assertion 4 drives one pair at a time.

#include "Vdenise_collision.h"
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

constexpr int A_CLXDAT = 0x00E >> 1;
constexpr int A_CLXCON = 0x098 >> 1;
constexpr int A_IDLE   = 0x1FE >> 1;      // decodes to nothing in this module

class Harness {
public:
    Harness() : dut(new Vdenise_collision) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->aga = 1; dut->dblpf = 1;   // odd match need not wait on even
        dut->reg_address_in = A_IDLE; dut->data_in = 0;
        dut->bpldata = 0; dut->nsprite = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
        // clxdat has NO reset term (denise_collision.v:105-111): it starts
        // wherever the accumulator left it and takes whatever cl asserted
        // while reset was held. A fixture that wants a known start must use
        // the register's own read-and-clear, which is what this does.
        read(A_CLXDAT);
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() { dut->clk = 1; dut->eval(); dut->clk = 0; dut->eval(); }

    void write(int addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        step();
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        step();
    }

    // Hold the objects on the two data ports long enough for the collision
    // accumulator to take them.
    void collide(uint8_t planes, uint8_t sprites, int ccks = 4) {
        dut->bpldata = planes;
        dut->nsprite = sprites;
        for (int i = 0; i < ccks; ++i) step();
        dut->bpldata = 0;
        dut->nsprite = 0;
        step();
    }

    // A read leaves the address asserted for a cycle and then withdraws it,
    // which is what the clear hangs off.
    uint16_t read(int addr) {
        dut->reg_address_in = addr;
        dut->eval();
        const uint16_t got = (uint16_t)dut->data_out;
        step();
        dut->reg_address_in = A_IDLE;
        dut->eval();
        step();
        step();
        return got;
    }

    Vdenise_collision* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    const bool mutate = std::getenv("MUTATE") != nullptr;
    Checks c;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red.
    const int      m_addr = mutate ? A_IDLE : A_CLXDAT;   // 1,2: nothing answers
    const bool     m_read = mutate;                       // 3: never reads first
    const uint8_t  m_spr  = mutate ? 0x00 : 0x03;         // 4: no sprite present

    std::cout << "aa-collision-data-register -- AA_CONFORMANCE at denise_collision\n";
    std::cout << (mutate ? "MUTATE=1: every assertion below must go red\n" : "");

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUTATE=1 reads an
        // address this module decodes for nothing, which answers zero.
        const uint16_t got = h.read(m_addr);
        c.note("out of reset the collision register reads " + hex(got));
        c.expect(got == 0x8000,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: with nothing on the two object "
                 "ports the register reads 0x8000 -- no collision recorded, "
                 "and the top bit the table calls not used standing at one");
    }

    // --- 2. CLXDAT answers at $00E and nowhere else -------------------------
    {
        Harness h;
        // MUTATION CLXDAT-AT-00E stimulus red MUTATE=1 reads an address
        // this module decodes for nothing, so no collision comes back and
        // the positive half cannot hold.
        h.collide(0x3F, 0x00);                 // odd and even planes together
        const uint16_t at_00e = h.read(m_addr);
        Harness g;
        g.collide(0x3F, 0x00);
        const uint16_t elsewhere = g.read(A_CLXCON);
        c.note("$00E reads " + hex(at_00e) + ", $098 reads " + hex(elsewhere));
        c.expect((at_00e & 0x0001) && elsewhere == 0x0000,
                 "CLXDAT-AT-00E: the accumulated collisions answer at $00E and "
                 "another address of the same module answers zero, which is "
                 "the address the AA entry prints for the collision data "
                 "register [p.19] [claims: opus:AA-clxdat.c1/c2/c7 sol:AA-clxdat.c1/c2/c3/c6/c8]");
    }

    // --- 3. Reading it clears it --------------------------------------------
    {
        Harness h;
        h.collide(0x3F, 0x00);
        // MUTATION CLXDAT-IS-READ-AND-CLEAR stimulus red MUTATE=1 skips the
        // first read, so the second one still finds the collision and the
        // clear cannot be told from a register that was never set.
        const uint16_t first = m_read ? 0x8001 : h.read(A_CLXDAT);
        const uint16_t second = h.read(A_CLXDAT);
        c.note("first read " + hex(first) + ", second read " + hex(second));
        c.expect((first & 0x0001) && (second & 0x7FFF) == 0x0000,
                 "CLXDAT-IS-READ-AND-CLEAR: the first read carries the "
                 "collision and the second finds the register empty, which is "
                 "what the AA entry means by a collision data register that is "
                 "read and clear [p.19] [claims: opus:AA-clxdat.c6 sol:AA-clxdat.c4/c7]");
    }

    // --- 4. The playfield-to-sprite bits name the pairs ---------------------
    {
        bool ok = true;
        std::ostringstream o;
        // MUTATION PLAYFIELD-TO-SPRITE-BITS stimulus red MUTATE=1 puts no
        // sprite on the port, so no playfield-to-sprite bit can set.
        for (int pair = 0; pair < 4; ++pair) {
            Harness h;
            const uint8_t sprites = (uint8_t)(m_spr << (2 * pair));
            h.collide(0x15, sprites);          // odd planes plus one pair
            const uint16_t got = h.read(A_CLXDAT);
            const uint16_t want = (uint16_t)(1u << (1 + pair));
            if (!(got & want)) {
                ok = false;
                o << (o.tellp() ? ", " : "") << "pair " << pair << " gave "
                  << hex(got);
            }
        }
        c.note(std::string("playfield 1 against each sprite pair: ") +
               (ok ? "every pair set its own bit" : o.str()));
        c.expect(ok,
                 "PLAYFIELD-TO-SPRITE-BITS: odd bitplanes against sprite pair "
                 "0-1, 2-3, 4-5 and 6-7 set bits 1, 2, 3 and 4 in that order, "
                 "which is the order the AA table prints Playfield 1 against "
                 "the four sprite pairs [p.20] [claims: opus:AA-clxdat.c5 sol:AA-clxdat.c21/c22/c23/c24 opus:AA-clxdat.b01 opus:AA-clxdat.b02 opus:AA-clxdat.b03 opus:AA-clxdat.b04]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of denise_collision carries "
             "a chip identity. [claims: opus:AA-clxdat.c3/c4]");
    c.record(
             "DECLINED at this boundary (insufficient): The row names a pair "
             "this package does not drive: the Playfield 2 rows need the even "
             "plane match held beside a sprite, and the sprite-to-sprite rows "
             "need two pairs present at once. The fixture drives one pair at a "
             "time. [claims: opus:AA-clxdat.c8 "
             "sol:AA-clxdat.c9/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c25 "
             "opus:AA-clxdat.b00 opus:AA-clxdat.b05 opus:AA-clxdat.b06 "
             "opus:AA-clxdat.b07 opus:AA-clxdat.b08 opus:AA-clxdat.b09 "
             "opus:AA-clxdat.b10 opus:AA-clxdat.b11 opus:AA-clxdat.b12 "
             "opus:AA-clxdat.b13 opus:AA-clxdat.b14]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is that the "
             "document's two statements of this register differ. Which is right "
             "is a fact about the document. [claims: sol:AA-clxdat.c5]");
    c.record(
             "DECLINED at this boundary (source-silent): The table's row 15 says "
             "`not used`. The module drives that position from a constant one, "
             "and `not used` is not `reads zero`, so the row requires nothing of "
             "a signal. NM-HRM-009 raised the same bit against the Hardware "
             "Reference Manual and was withdrawn. [claims: sol:AA-clxdat.c10 "
             "opus:AA-clxdat.b15]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
