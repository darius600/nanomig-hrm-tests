// aa-fmode-sprite-fetch -- agnus_spritedma conformance test
//
// Test purpose: the AA specification gives FMODE a bit table and then a
//   four-row table of what its sprite pair does -- SPAGEM and SPR32 together
//   choose how far a sprite fetch advances the pointer, 2, 4, 4 or 8 bytes.
//   This drives DMA cycles and measures the advance.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 23 and 24, the FMODE entry
//   and the sprite half of the fetch table it continues. The document prints
//   no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: the bitplane half of the same table -- BPAGEM and BPL32 reach
//   agnus_bitplanedma, a module this package does not build -- the CAS cycles
//   and bus widths the table's other two columns name, and sprite scan
//   doubling, which needs the DMA state machine driven across a display
//   window.
//
// WHAT THE TABLE SAYS AND WHERE THE MODULE SAYS IT. The document's sprite rows
//   are in bytes; the module adds words to a word address:
//
//     case(fmode[3:2])
//       2'b00   : spr_fmode_ptradd = 3'd1;     // 2 bytes
//       2'b11   : spr_fmode_ptradd = 3'd4;     // 8 bytes
//       default : spr_fmode_ptradd = 3'd2;     // 4 bytes
//     endcase
//     assign newptr = address_out[20:1] + spr_fmode_ptradd;        (:158-175)
//
//   so the comparison below doubles the module's word step before setting it
//   against the document's byte figure, and says so in the measurement.
//
// HOW A FETCH IS MADE TO HAPPEN. The pointer bank writes back on `ackdma`
//   (:182, :193), and which sprite it writes is `sprite`, which the beam
//   position selects. The fixture parks hpos in a sprite slot exactly as
//   aa-sprite-dma-addresses does -- that package grades the parking itself --
//   then raises ackdma for one clk7_en cycle and reads address_out again.

#include "Vagnus_spritedma.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static std::string hex(uint32_t v) {
    std::ostringstream o;
    o << "0x" << std::hex << std::uppercase << v;
    return o.str();
}

static std::string dec(int v) {
    std::ostringstream o;
    o << v;
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

constexpr uint16_t A_SPRPTBASE = 0x120;   // SPR0PTH; +4 per sprite, +2 for PTL
constexpr uint16_t A_FMODE     = 0x1FC;
constexpr uint16_t A_BPLCON0   = 0x100;   // a neighbour FMODE must not answer for

static uint16_t sprpth_addr(int s) { return A_SPRPTBASE + 4 * s; }
static uint16_t sprptl_addr(int s) { return A_SPRPTBASE + 4 * s + 2; }

class Harness {
public:
    Harness() : dut(new Vagnus_spritedma) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->aga = 1; dut->ecs = 1;
        dut->ackdma = 0; dut->vbl = 0; dut->vblend = 0;
        dut->hpos = 0; dut->vpos = 0x40;
        dut->reg_address_in = 0; dut->data_in = 0;
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

    // Solve sprite[2:0] = {hpos[5]^hpos[4], ~hpos[4], hpos[3]} for hpos, as
    // aa-sprite-dma-addresses does, so the bank selects the sprite we mean.
    static uint16_t hpos_for(int s) {
        const int b3 = s & 1;
        const int b4 = ((s >> 1) & 1) ? 0 : 1;
        const int b5 = (((s >> 2) & 1) ^ b4) & 1;
        uint16_t h = 0x001 | (b3 << 3) | (b4 << 4) | (b5 << 5);
        while ((h >> 1) < 0x18 || (h >> 1) > 0x38) h += 0x40;
        return h & 0x1FF;
    }

    void select(int s) {
        dut->hpos = hpos_for(s);
        for (int i = 0; i < 4; ++i) step();
        dut->eval();
    }

    uint32_t pointer(int s) { select(s); return dut->address_out & 0xFFFFF; }

    void set_pointer(int s, uint32_t word_address) {
        write(sprpth_addr(s), (uint16_t)((word_address >> 15) & 0x1F));
        write(sprptl_addr(s), (uint16_t)((word_address << 1) & 0xFFFE));
    }

    // One granted DMA cycle for the sprite the beam has selected.
    void fetch(int s) {
        select(s);
        dut->ackdma = 1;
        step();
        dut->ackdma = 0;
        step();
        dut->eval();
    }

    // How far one fetch moves the pointer, in BYTES, which is the unit the
    // document's table prints.
    int advance_bytes(int s, uint16_t fmode) {
        write(A_FMODE, fmode);
        set_pointer(s, 0x01000);
        const uint32_t before = pointer(s);
        fetch(s);
        const uint32_t after = pointer(s);
        return (int)(after - before) * 2;
    }

    Vagnus_spritedma* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard moves ONE thing: the address a word is written to, or which bits
    // of the written word carry the sprite pair.
    const uint16_t m_fmode = mutate ? A_BPLCON0 : A_FMODE;   // 2
    const int      m_shift = mutate ? 0 : 2;                 // 3: the pair's place
    const uint16_t m_high  = mutate ? (uint16_t)0x000Cu      // 4: unused bits
                                    : (uint16_t)0x3FF0u;
    const uint16_t m_low   = mutate ? (uint16_t)0x000Cu      // 5: the other pair
                                    : (uint16_t)0x0003u;
    const uint16_t m_idle  = mutate ? (uint16_t)0x000Cu      // 1: liveness
                                    : (uint16_t)0x0000u;

    std::cout << "aa-fmode-sprite-fetch -- AA_CONFORMANCE at agnus_spritedma\n";
    std::cout << (mutate ? "MUTATE=1: every assertion below must go red\n" : "");

    // The document's four sprite rows, in bytes, SPAGEM:SPR32 = 00, 01, 10, 11.
    const int WANT[4] = {2, 4, 4, 8};

    // --- 1. The fixture moves a pointer at all ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-ADVANCES-A-POINTER stimulus red MUTATE=1 sets
        // the sprite pair before the fetch, so the pointer moves by the
        // table's widest step and not by the cleared register's word.
        const int got = h.advance_bytes(3, m_idle);
        c.note("with FMODE cleared, one granted DMA cycle moves sprite 3's "
               "pointer by " + dec(got) + " bytes");
        c.expect(got == 2,
                 "FIXTURE-ADVANCES-A-POINTER: a granted DMA cycle moves the "
                 "selected sprite's pointer, so every row measured below is "
                 "measured against a fetch this fixture caused "
                 "[no page -- infrastructure]");
    }

    // --- 2. FMODE is the register at $1FC ----------------------------------
    {
        Harness h;
        // MUTATION FMODE-IS-DECODED-AT-1FC stimulus red MUTATE=1 writes the
        // same word to BPLCON0's address instead, which this module ignores,
        // so the fetch keeps the cleared register's 2 bytes.
        h.write(m_fmode, 0x000C);           // SPAGEM and SPR32 both set
        h.set_pointer(2, 0x01000);
        const uint32_t before = h.pointer(2);
        h.fetch(2);
        const int got = (int)(h.pointer(2) - before) * 2;
        c.note("after writing $000C at " + hex(mutate ? A_BPLCON0 : A_FMODE) +
               ", a sprite fetch advances by " + dec(got) + " bytes");
        c.expect(got == 8,
                 "FMODE-IS-DECODED-AT-1FC: the word that changes the sprite "
                 "fetch step is the one written at $1FC, which is where the "
                 "AA entry puts FMODE [p.23] [claims: opus:AA-fmode.c3 sol:AA-fmode.c4]");
    }

    // --- 3. The four rows of the sprite fetch table -------------------------
    {
        Harness h;
        int got[4] = {0, 0, 0, 0};
        bool all = true;
        for (int row = 0; row < 4; ++row) {
            // MUTATION SPRITE-FETCH-ADVANCES-AS-THE-TABLE-SAYS stimulus red
            // MUTATE=1 puts the pair in bits 1 and 0, where the bitplane pair
            // lives, so every row reads the cleared register's 2 bytes.
            got[row] = h.advance_bytes(1, (uint16_t)(row << m_shift));
            if (got[row] != WANT[row]) all = false;
        }
        c.note("SPAGEM:SPR32 = 00, 01, 10, 11 advance the sprite pointer by " +
               dec(got[0]) + ", " + dec(got[1]) + ", " + dec(got[2]) + " and " +
               dec(got[3]) + " bytes; the table gives 2, 4, 4 and 8");
        c.expect(all,
                 "SPRITE-FETCH-ADVANCES-AS-THE-TABLE-SAYS: all four rows of "
                 "the sprite half of FMODE's fetch table hold -- the pair "
                 "steps the pointer 2, 4, 4 and 8 bytes, and the two middle "
                 "rows are the same step by two different routes [p.24] "
                 "[claims: opus:AA-fmode.c11/c31/c32/c33/c34 sol:AA-fmode.c13 sol:AA-fmode-sprite-fetch.c1/c4/c7/c10]");
    }

    // --- 4. And the ten bits between the two pairs do nothing here ----------
    {
        Harness h;
        // MUTATION FMODE-BITS-13-TO-4-DO-NOTHING stimulus red MUTATE=1 writes
        // the sprite pair instead of the unused bits, so the step changes and
        // the assertion reads a register that was not left alone.
        const int got = h.advance_bytes(5, m_high);
        c.note("with FMODE = " + hex(m_high) + " the sprite fetch advances by " +
               dec(got) + " bytes, where a cleared FMODE advances by 2");
        c.expect(got == 2,
                 "FMODE-BITS-13-TO-4-DO-NOTHING: writing every bit the AA bit "
                 "table marks unused leaves the sprite fetch where a cleared "
                 "FMODE leaves it [p.23] [claims: opus:AA-fmode.c14 sol:AA-fmode.c11]");
    }

    // --- 5. The low pair is the bitplanes', not the sprites' ----------------
    {
        Harness h;
        // MUTATION SPRITE-FETCH-IGNORES-THE-BITPLANE-PAIR stimulus red
        // MUTATE=1 writes the sprite pair instead, so the step moves and the
        // two pairs are no longer told apart.
        const int got = h.advance_bytes(6, m_low);
        c.note("with BPAGEM and BPL32 both set and the sprite pair clear, a "
               "sprite fetch advances by " + dec(got) + " bytes");
        c.expect(got == 2,
                 "SPRITE-FETCH-IGNORES-THE-BITPLANE-PAIR: bits 1 and 0 size "
                 "the bitplane data the AA entry names BPL1DAT to BPL8DAT and "
                 "leave the sprite fetch alone, so the entry's two pairs are "
                 "two pairs here as well [p.23] [claims: opus:AA-fmode.c1/c2/c13/c15/c16/c17/c18 sol:AA-fmode.c1/c15]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (absent): The claim is about read or "
             "write access to the register. This module has no data output port, "
             "so no read of any address is possible here and write-only-ness "
             "cannot be distinguished from silence. [claims: opus:AA-fmode.c4 "
             "sol:AA-fmode.c5]");
    c.record(
             "DECLINED at this boundary (absent): The claim is about the memory "
             "cycle or the bus width a fetch uses -- normal or double CAS, 16, "
             "32 or 64 bits. This module drives a chip address and a DMA "
             "request; no port of it carries a memory cycle or a bus width. "
             "[claims: "
             "opus:AA-fmode.c10/c12/c19/c20/c21/c22/c23/c24/c25/c26/c35/c36/c37/c38/c39/c40/c41/c42 "
             "sol:AA-fmode.c12/c14 "
             "sol:AA-fmode-bitplane-fetch.c2/c3/c5/c6/c8/c9/c11/c12 "
             "sol:AA-fmode-sprite-fetch.c2/c3/c5/c6/c8/c9/c11/c12]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about scan "
             "doubling. SSCAN2 and BSCAN2 decide whether a DMA happens on a "
             "line, which the state machine settles inside a display window "
             "against a sprite's own SPRxPOS; this fixture grants single cycles "
             "and does not run a window. [claims: opus:AA-bscan2.c1/c2/c3/c4 "
             "opus:AA-fmode.c8/c9 opus:AA-scandouble.c1/c2 "
             "opus:AA-sscan2.c1/c2/c3/c4/c5 sol:AA-fmode.c2/c3/c9/c10 "
             "sol:AA-scan-doubling.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of agnus_spritedma carries a "
             "chip identity. [claims: opus:AA-fmode.c7 sol:AA-fmode.c8]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- the revision mark and what the legend makes of it, a "
             "conflict between two of its statements, or a misprint. [claims: "
             "opus:AA-fmode.c5/c6 sol:AA-fmode.c6/c7]");
    c.record(
             "DECLINED at this boundary (outside-uut): The claim is about the "
             "bitplane half of the fetch table or about BPAGEM and BPL32 "
             "themselves. Those bits reach agnus_bitplanedma; this module holds "
             "FMODE but uses only its sprite pair. [claims: "
             "sol:AA-fmode-bitplane-fetch.c1/c4/c7/c10]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
