// aa-bplcon-bits-at-denise -- denise conformance test
//
// Test purpose: the AA specification gives BPLCON0, BPLCON2 and BPLCON3 a bit
//   table each, and a few of those bits reach a port of this module or decide
//   whether the colour table can be read at all. This grades those: HIRES and
//   SHRES, which are outputs; RDRAM, which opens the read path; and the three
//   BANK bits, which choose which block of thirty-two entries a colour write
//   lands in.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 19 to 21, the BPLCON0,
//   BPLCON2 and BPLCON3 entries and their bit tables. The document prints no
//   page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: every BPLCON field that decides what a pixel looks like -- BPU,
//   HAM, DPF, the playfield offsets, the sprite resolution, the border bits --
//   which needs bitplane data clocked through a display window, and the
//   BPLCON1 scroll, which lives in denise_bitplanes.
//
// WHY THESE FOUR AND NOT THE OTHERS. `hires` and `shres` are ports of this
//   module (denise.v:180, :187), so two of BPLCON0's bit rows can be read
//   straight off the boundary. RDRAM and BANK are different: they are not
//   ports, but they decide what a BUS READ of a colour address returns --
//
//     assign rdram = bplcon2[8] & aga;                              (:219)
//     assign bank  = bplcon3[15:13] & {3{aga}};                     (:249)
//     wire wr_adr = {bank[2:0], reg_address_in[5:1]};   (colortable:30)
//     wire wr_en  = COLORBASE match && clk7_en && !rdram;           (:31)
//
//   so the table is written with RDRAM low and read with it high, and the
//   bank picks the thirty-two-entry block at both ends. That makes both bits
//   measurable with nothing but register writes and one read.
//
// THE READ PATH IS ALSO THE WRITE INHIBIT. `wr_en` has `!rdram` in it, so the
//   same bit that opens the read closes the write. An assertion that set
//   RDRAM, wrote a colour and read it back would measure nothing -- the write
//   would never land. Every write below is done with RDRAM low.

#include "Vdenise.h"
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

constexpr uint16_t A_BPLCON0 = 0x100;
constexpr uint16_t A_BPLCON1 = 0x102;   // a neighbour BPLCON0 must not answer for
constexpr uint16_t A_BPLCON2 = 0x104;
constexpr uint16_t A_BPLCON3 = 0x106;
constexpr uint16_t A_COLORBASE = 0x180;

constexpr uint16_t HIRES = 1u << 15;    // BPLCON0 bit 15
constexpr uint16_t SHRES = 1u << 6;     // BPLCON0 bit 6
constexpr uint16_t RDRAM = 1u << 8;     // BPLCON2 bit 8
constexpr int      BANK_SHIFT = 13;     // BPLCON3 bits 15 to 13

static uint16_t colour_addr(int entry) {
    return (uint16_t)(A_COLORBASE + 2 * (entry & 31));
}

class Harness {
public:
    Harness() : dut(new Vdenise) {
        dut->clk = 0; dut->clk7_en = 1;
        dut->aga = 1; dut->ecs = 1;
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

    // Put one colour into one bank, with the read path closed so it lands.
    void put(int bank, int entry, uint16_t value) {
        write(A_BPLCON2, 0);
        write(A_BPLCON3, (uint16_t)((bank & 7) << BANK_SHIFT));
        write(colour_addr(entry), value);
    }

    // Take it back out, with the read path open.
    uint16_t get(int bank, int entry, uint16_t bplcon2 = RDRAM) {
        write(A_BPLCON3, (uint16_t)((bank & 7) << BANK_SHIFT));
        write(A_BPLCON2, bplcon2);
        const uint16_t v = read(colour_addr(entry));
        write(A_BPLCON2, 0);
        return v;
    }

    Vdenise* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard moves ONE thing: an address written, a bit written, or the bank a
    // colour is read from.
    const uint16_t m_con0 = mutate ? A_BPLCON1 : A_BPLCON0;   // 2, 3
    const uint16_t m_rd   = mutate ? RDRAM     : 0u;          // 4
    const int      m_bank = mutate ? 2         : 5;           // 5
    const uint16_t m_live = mutate ? (uint16_t)0u : RDRAM;    // 1

    constexpr uint16_t COL_A = 0x0F3C;
    constexpr uint16_t COL_B = 0x0A55;

    std::cout << "aa-bplcon-bits-at-denise -- AA_CONFORMANCE at denise\n";
    std::cout << (mutate ? "MUTATE=1: every assertion below must go red\n" : "");

    // --- 1. The fixture puts a colour in and gets it back -------------------
    {
        Harness h;
        h.put(0, 3, COL_A);
        // MUTATION FIXTURE-REACHES-THE-COLOUR-TABLE stimulus red MUTATE=1
        // reads with RDRAM low, which is the state the table takes writes in
        // and answers no reads in, so nothing comes back.
        const uint16_t got = h.get(0, 3, m_live);
        c.note("colour entry 3 of bank 0 was written " + hex(COL_A) +
               " and reads back " + hex(got));
        c.expect((got & 0x0FFF) == (COL_A & 0x0FFF),
                 "FIXTURE-REACHES-THE-COLOUR-TABLE: a colour written at $186 "
                 "comes back from $186, so every assertion below is against a "
                 "table this fixture wrote [no page -- infrastructure]");
    }

    // --- 2. HIRES is bit 15 of BPLCON0 --------------------------------------
    {
        Harness h;
        const bool before = h.dut->hires != 0;
        // MUTATION HIRES-IS-BPLCON0-BIT-15 stimulus red MUTATE=1 writes
        // BPLCON1's address instead, so the bit never reaches BPLCON0 and the
        // output stays where reset left it.
        h.write(m_con0, HIRES);
        const bool after = h.dut->hires != 0;
        c.note(std::string("hires reads ") + (before ? "1" : "0") +
               " after reset and " + (after ? "1" : "0") +
               " after bit 15 is written");
        c.expect(!before && after,
                 "HIRES-IS-BPLCON0-BIT-15: the high-resolution select the AA "
                 "bit table puts at bit 15 of BPLCON0 is the bit this module "
                 "puts on its `hires` output, and reset leaves it clear as the "
                 "table's RST_ column says [p.19] [claims: opus:AA-hires.c1 opus:AA-spres.c7 sol:AA-bplcon0.c1/c2/c3/c6/c26 sol:AA-spres.c3 opus:AA-bplcon0.b15 opus:AA-bplcon0.h1]");
    }

    // --- 3. SHRES is bit 6 --------------------------------------------------
    {
        Harness h;
        const bool before = h.dut->shres != 0;
        // MUTATION SHRES-IS-BPLCON0-BIT-6 stimulus red MUTATE=1 writes
        // BPLCON1's address, so the bit never reaches BPLCON0 and the
        // output never rises.
        h.write(m_con0, SHRES);
        const bool after = h.dut->shres != 0;
        h.write(A_BPLCON0, 0);
        const bool cleared = h.dut->shres != 0;
        c.note(std::string("shres reads ") + (before ? "1" : "0") +
               " after reset, " + (after ? "1" : "0") +
               " after bit 6 is written and " + (cleared ? "1" : "0") +
               " after the register is cleared");
        c.expect(!before && after && !cleared,
                 "SHRES-IS-BPLCON0-BIT-6: the super-hires select the AA bit "
                 "table puts at bit 6 is the bit this module puts on its "
                 "`shres` output, and it follows the register both ways "
                 "[p.19] [claims: opus:AA-bplcon1.c2 opus:AA-clock.c1 opus:AA-coercion.c2 opus:AA-colortable.c3 opus:AA-ham.c3/c30 opus:AA-pf1h.c2 opus:AA-shres.c1 opus:AA-spres.c1/c3/c5 sol:AA-bplcon0.c18/c35 sol:AA-bplcon1.c2/c11 sol:AA-clocking.c1 sol:AA-ehb.c2 sol:AA-ham6.c1 sol:AA-ham8.c7 sol:AA-palette.c15 sol:AA-spres.c1 sol:AA-sprites.c2/c6/c18 opus:AA-bplcon0.b06]");
    }

    // --- 4. RDRAM is what opens the colour table to a read ------------------
    {
        Harness h;
        h.put(0, 7, COL_B);
        const uint16_t open = h.get(0, 7, RDRAM);
        // MUTATION RDRAM-IS-BPLCON2-BIT-8 stimulus red MUTATE=1 leaves RDRAM
        // set for the closed trial as well, so both reads answer and the bit
        // distinguishes nothing.
        const uint16_t shut = h.get(0, 7, m_rd);
        c.note("with BPLCON2 bit 8 set the colour address reads " + hex(open) +
               "; with it clear it reads " + hex(shut));
        c.expect((open & 0x0FFF) == (COL_B & 0x0FFF) && shut == 0,
                 "RDRAM-IS-BPLCON2-BIT-8: the bit the AA entry calls RDRAM is "
                 "what makes a colour address readable at all -- with it clear "
                 "the same address answers nothing [p.20] "
                 "[claims: opus:AA-rdram.c1/c2 sol:AA-bplcon2.c2/c3/c15 opus:AA-bplcon2.b08 opus:AA-bplcon2.h1]");
    }

    // --- 5. BANK chooses which thirty-two entries a write lands in ----------
    {
        Harness h;
        h.put(2, 5, COL_A);
        // MUTATION BANK-IS-BPLCON3-BITS-15-TO-13 stimulus red MUTATE=1
        // sends the second colour to the FIRST bank, so one bank holds
        // both writes and the other holds neither.
        h.put(m_bank, 5, COL_B);
        const uint16_t two = h.get(2, 5);
        const uint16_t other = h.get(5, 5);
        c.note("entry 5 reads " + hex(two) + " in bank 2 and " + hex(other) +
               " in bank 5, after one write to each");
        c.expect((two & 0x0FFF) == (COL_A & 0x0FFF) &&
                 (other & 0x0FFF) == (COL_B & 0x0FFF),
                 "BANK-IS-BPLCON3-BITS-15-TO-13: the three bits the AA bit "
                 "table calls BANK2 to BANK0 choose which block of thirty-two "
                 "colour entries one colour address reaches, so the same "
                 "address holds a different colour in each bank [p.21] "
                 "[claims: opus:AA-bank.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10 sol:AA-bplcon3.c3 sol:AA-palette.c14 sol:AA-palette-bank.c1/c2/c3/c4/c5/c6/c7/c8 opus:AA-bplcon3.b13 opus:AA-bplcon3.b14 opus:AA-bplcon3.b15 opus:AA-bplcon3.h1]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (absent): The claim is about a chip pin "
             "-- ZD, SOG, BLANK, the genlock clock, the light pen, the mouse "
             "lines or the R(7:0) bypass path. denise has `red`, `green`, "
             "`blue`, `hires` and `shres` and no such pin. [claims: "
             "opus:AA-blank.c1 opus:AA-bypass.c1 opus:AA-gaud.c1/c2 "
             "opus:AA-genlock.c1 opus:AA-lpen.c1 opus:AA-mouse.c1/c2/c3 "
             "opus:AA-sogen.c1/c3 opus:AA-zdbpen.c1/c2/c3 opus:AA-zdclken.c1 "
             "opus:AA-zdcten.c1/c3 sol:AA-genlock.c1/c2/c3/c4/c5/c6 "
             "sol:AA-mouse.c1/c2/c3/c4/c5/c6/c7/c8]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about LOCT "
             "and the two halves of a colour entry. That is graded at this same "
             "boundary by aa-colour-table-loct, whose lot is a different slice's "
             "reading; this package has no assertion that writes both halves. "
             "[claims: opus:AA-colortable.c2 opus:AA-loct.c2/c4 "
             "opus:AA-palette.c1/c2/c3/c4 sol:AA-bplcon3.c8/c18 "
             "sol:AA-palette.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13 "
             "opus:AA-bplcon3.b09 opus:AA-bplcon3.b09.rst]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about read "
             "or write access to the register. denise answers reads at the "
             "collision, chip-id and colour addresses only, so a write-only "
             "control register reads as the same zero an undecoded address reads "
             "as. [claims: sol:AA-bplcon0.c4 sol:AA-bplcon2.c4 sol:AA-bplcon3.c4 "
             "sol:AA-bplcon4.c5 opus:AA-bplcon0.h2 opus:AA-bplcon2.h2 "
             "opus:AA-bplcon3.h2 opus:AA-bplcon4.h2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about the "
             "display window. DIWHIGH is graded at this same boundary by "
             "aa-display-window-registers, whose lot is a different slice's "
             "reading, and the window itself is only visible while a beam runs. "
             "[claims: opus:AA-diwhigh.c1/c2 sol:AA-diwhigh.c1/c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about what "
             "RST_ low leaves in a register whose bits this package cannot see. "
             "Reset is driven here, but a bit's reset value is only readable "
             "through the field it controls. [claims: opus:AA-rst.c1/c2/c4 "
             "sol:AA-bplcon0.c36/c37/c41 sol:AA-bplcon2.c29/c30 "
             "sol:AA-bplcon3.c12/c13/c14/c15/c16/c17/c21/c22/c23/c24/c27/c29 "
             "sol:AA-bplcon4.c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23 "
             "sol:AA-reset.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11 "
             "opus:AA-bplcon0.b00.rst opus:AA-bplcon0.b04.rst "
             "opus:AA-bplcon0.b05.rst opus:AA-bplcon2.b07.rst "
             "opus:AA-bplcon2.b08.rst opus:AA-bplcon3.b00.rst "
             "opus:AA-bplcon3.b01.rst opus:AA-bplcon3.b02.rst "
             "opus:AA-bplcon3.b04.rst opus:AA-bplcon3.b05.rst "
             "opus:AA-bplcon3.b06.rst opus:AA-bplcon3.b07.rst "
             "opus:AA-bplcon3.b10.rst opus:AA-bplcon3.b11.rst "
             "opus:AA-bplcon3.b12.rst opus:AA-bplcon3.b13.rst "
             "opus:AA-bplcon3.b14.rst opus:AA-bplcon3.b15.rst "
             "opus:AA-bplcon4.b00.rst opus:AA-bplcon4.b01.rst "
             "opus:AA-bplcon4.b02.rst opus:AA-bplcon4.b03.rst "
             "opus:AA-bplcon4.b04.rst opus:AA-bplcon4.b05.rst "
             "opus:AA-bplcon4.b06.rst opus:AA-bplcon4.b07.rst "
             "opus:AA-bplcon4.b08.rst opus:AA-bplcon4.b09.rst "
             "opus:AA-bplcon4.b10.rst opus:AA-bplcon4.b11.rst "
             "opus:AA-bplcon4.b12.rst opus:AA-bplcon4.b13.rst "
             "opus:AA-bplcon4.b14.rst opus:AA-bplcon4.b15.rst]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about what "
             "a pixel looks like -- the number of bitplanes, hold-and-modify, "
             "dual playfield, the playfield offsets and priorities, "
             "extra-half-brite, the border bits, sprite resolution or the sprite "
             "colour maps. Those are settled while bitplane data is clocked "
             "through a display window against a beam; this package writes "
             "registers and reads one bus address. [claims: "
             "opus:AA-bitplanes.c1/c3/c4/c5 opus:AA-bplam.c1/c2 "
             "opus:AA-bplcon0.c0/c1 opus:AA-bplcon3.c1 opus:AA-bplcon4.c1/c2/c3 "
             "opus:AA-bpu.c1 opus:AA-brdntran.c1 opus:AA-brdrblnk.c1 "
             "opus:AA-brdrsprt.c1 opus:AA-color.c1 opus:AA-dpf.c1/c2/c3 "
             "opus:AA-ecsena.c1/c2/c4/c5/c6/c7 opus:AA-ehb.c1/c3/c4/c5/c6 "
             "opus:AA-esprm.c1 "
             "opus:AA-ham.c1/c2/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c31/c32/c33 "
             "opus:AA-killehb.c1 opus:AA-osprm.c1/c2 opus:AA-pf1p.c1 "
             "opus:AA-pf2of.c1/c2/c3/c4/c5/c6/c8/c9/c10/c11 opus:AA-pf2p.c1 "
             "opus:AA-pf2pri.c1 opus:AA-spres.c2/c4/c6 opus:AA-sprites.c1/c2 "
             "opus:AA-uhres.c1/c3 sol:AA-bitplanes.c1/c2/c3 sol:AA-bplcon.c1 "
             "sol:AA-bplcon0.c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c20/c21/c22/c23/c24/c27/c28/c29/c30/c31/c32/c33/c34/c38/c39/c40 "
             "sol:AA-bplcon2.c1/c6/c9/c10/c11/c12/c13/c14/c16/c17/c18/c19/c21/c23/c24/c25/c26/c27/c28/c31/c32/c33/c34/c35/c36/c37 "
             "sol:AA-bplcon3.c1/c2/c6/c7/c9/c10/c19/c25 "
             "sol:AA-bplcon4.c1/c2/c3/c4 sol:AA-dual-playfield.c1/c2/c4 "
             "sol:AA-ecsena.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10 "
             "sol:AA-ehb.c1/c3/c4/c5/c6 sol:AA-ham.c1/c2 "
             "sol:AA-ham6.c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12 "
             "sol:AA-ham8.c1/c2/c3/c4/c5/c6/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17 "
             "sol:AA-pf2of.c1/c2/c3/c4/c5/c6/c7/c8 sol:AA-spres.c2 "
             "sol:AA-sprites.c1/c3/c4/c5/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c19 "
             "opus:AA-bplcon0.b00 opus:AA-bplcon0.b01 opus:AA-bplcon0.b02 "
             "opus:AA-bplcon0.b03 opus:AA-bplcon0.b04 opus:AA-bplcon0.b05 "
             "opus:AA-bplcon0.b07 opus:AA-bplcon0.b08 opus:AA-bplcon0.b09 "
             "opus:AA-bplcon0.b10 opus:AA-bplcon0.b11 opus:AA-bplcon0.b12 "
             "opus:AA-bplcon0.b13 opus:AA-bplcon0.b14 opus:AA-bplcon2.b00 "
             "opus:AA-bplcon2.b01 opus:AA-bplcon2.b02 opus:AA-bplcon2.b03 "
             "opus:AA-bplcon2.b04 opus:AA-bplcon2.b05 opus:AA-bplcon2.b06 "
             "opus:AA-bplcon2.b07 opus:AA-bplcon2.b09 opus:AA-bplcon2.b10 "
             "opus:AA-bplcon2.b11 opus:AA-bplcon2.b12 opus:AA-bplcon2.b13 "
             "opus:AA-bplcon2.b14 opus:AA-bplcon2.b15 opus:AA-bplcon3.b00 "
             "opus:AA-bplcon3.b01 opus:AA-bplcon3.b02 opus:AA-bplcon3.b03 "
             "opus:AA-bplcon3.b04 opus:AA-bplcon3.b05 opus:AA-bplcon3.b06 "
             "opus:AA-bplcon3.b07 opus:AA-bplcon3.b08 opus:AA-bplcon3.b10 "
             "opus:AA-bplcon3.b11 opus:AA-bplcon3.b12 opus:AA-bplcon4.b00 "
             "opus:AA-bplcon4.b01 opus:AA-bplcon4.b02 opus:AA-bplcon4.b03 "
             "opus:AA-bplcon4.b04 opus:AA-bplcon4.b05 opus:AA-bplcon4.b06 "
             "opus:AA-bplcon4.b07 opus:AA-bplcon4.b08 opus:AA-bplcon4.b09 "
             "opus:AA-bplcon4.b10 opus:AA-bplcon4.b11 opus:AA-bplcon4.b12 "
             "opus:AA-bplcon4.b13 opus:AA-bplcon4.b14 opus:AA-bplcon4.b15 "
             "opus:AA-bplcon4.h1]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of denise carries a chip "
             "identity. [claims: opus:AA-bitplanes.c2 opus:AA-bpl1mod.c3 "
             "opus:AA-bpl2mod.c3 opus:AA-clxdat.c1 opus:AA-colortable.c1 "
             "opus:AA-ham.c4 opus:AA-legend.c4/c5/c6/c11 opus:AA-sogen.c2 "
             "opus:AA-uhres.c2 sol:AA-bpl1mod.c3 sol:AA-bpl2mod.c3 "
             "sol:AA-bplcon0.c5/c17 sol:AA-bplcon1.c8 sol:AA-bplcon2.c5 "
             "sol:AA-bplcon3.c5 sol:AA-bplcon4.c6 opus:AA-bplcon0.h3 "
             "opus:AA-bplcon1.h3 opus:AA-bplcon2.h3 opus:AA-bplcon3.h3 "
             "opus:AA-bplcon4.h3]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- a misprint, a conflict between two of its statements, "
             "a legend mark, or a recommendation to software about an unused "
             "cell. [claims: opus:AA-bittable.c2 opus:AA-bpl1mod.c4 "
             "opus:AA-bpl2mod.c4 opus:AA-brdrsprt.c2 opus:AA-clxcon2.c3/c4 "
             "opus:AA-ecsena.c3 opus:AA-ehb.c2 opus:AA-ham.c16 "
             "opus:AA-legend.c15/c16/c18 opus:AA-loct.c1/c3 opus:AA-pf2of.c7 "
             "opus:AA-rst.c3 opus:AA-zdbpsel.c1 opus:AA-zdcten.c2 "
             "sol:AA-bplcon0.c25 sol:AA-bplcon1.c14 sol:AA-bplcon2.c7/c8/c20/c22 "
             "sol:AA-bplcon3.c11/c20/c26/c28 sol:AA-bplcon4.c7 sol:AA-clxcon2.c6 "
             "sol:AA-video.c2 opus:AA-bplcon0.h4 opus:AA-bplcon1.h4 "
             "opus:AA-bplcon2.b15.rec opus:AA-bplcon2.h4 opus:AA-bplcon3.b03.rec "
             "opus:AA-bplcon3.b08.rec opus:AA-bplcon3.h4 opus:AA-bplcon4.h4]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is an overview "
             "sentence about the chip set -- what was added, what it is for, "
             "what a mode is intended to achieve, or how the document is laid "
             "out. It states no requirement a signal could meet. [claims: "
             "opus:AA-bittable.c1 opus:AA-clock.c2/c3 opus:AA-coercion.c1/c3/c4 "
             "opus:AA-comparators.c1 opus:AA-databus.c1/c2/c3/c4 "
             "opus:AA-legend.c1/c2/c3/c7/c8/c9/c10/c17 sol:AA-clocking.c2/c3/c4 "
             "sol:AA-overview.c1/c2/c3/c4/c5 sol:AA-position-resolution.c2/c3 "
             "sol:AA-scan-coercion.c1 sol:AA-video.c1/c3/c4]");
    c.record(
             "DECLINED at this boundary (outside-uut): BPLCON1's scroll fields "
             "are consumed in denise_bitplanes, which is a module of its own and "
             "has its own AA package. [claims: opus:AA-bplcon1.c1/c3/c4/c5/c6 "
             "opus:AA-bypass.c2 opus:AA-pf1h.c1/c3 opus:AA-pf2h.c1 "
             "opus:AA-scroll.c1/c2 sol:AA-bplcon0.c19 "
             "sol:AA-bplcon1.c1/c3/c4/c5/c6/c7/c9/c10/c12/c13/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30 "
             "sol:AA-position-resolution.c1 sol:AA-scroll.c1 opus:AA-bplcon1.b00 "
             "opus:AA-bplcon1.b01 opus:AA-bplcon1.b02 opus:AA-bplcon1.b03 "
             "opus:AA-bplcon1.b04 opus:AA-bplcon1.b05 opus:AA-bplcon1.b06 "
             "opus:AA-bplcon1.b07 opus:AA-bplcon1.b08 opus:AA-bplcon1.b08.rst "
             "opus:AA-bplcon1.b09 opus:AA-bplcon1.b09.rst opus:AA-bplcon1.b10 "
             "opus:AA-bplcon1.b10.rst opus:AA-bplcon1.b11 "
             "opus:AA-bplcon1.b11.rst opus:AA-bplcon1.b12 "
             "opus:AA-bplcon1.b12.rst opus:AA-bplcon1.b13 "
             "opus:AA-bplcon1.b13.rst opus:AA-bplcon1.b14 "
             "opus:AA-bplcon1.b14.rst opus:AA-bplcon1.b15 "
             "opus:AA-bplcon1.b15.rst opus:AA-bplcon1.h1 opus:AA-bplcon1.h2]");
    c.record(
             "DECLINED at this boundary (outside-uut): The bitplane modulo "
             "registers are decoded in agnus_bitplanedma, not in denise. "
             "[claims: opus:AA-bitplanes.c6 opus:AA-bpl1mod.c1/c2/c5/c6/c7/c8 "
             "opus:AA-bpl2mod.c1/c2 opus:AA-legend.c12/c13/c14 "
             "sol:AA-bpl1mod.c1/c2 sol:AA-bpl2mod.c1/c2 "
             "sol:AA-bplmod.c1/c2/c3/c4/c5/c6/c7/c8 sol:AA-dual-playfield.c3]");
    c.record(
             "DECLINED at this boundary (outside-uut): The register is decoded "
             "in a sub-module of denise -- denise_collision for the collision "
             "pair, the chip-id block for LISAID -- and each is graded there by "
             "its own AA package. [claims: opus:AA-clxcon2.c1/c2 "
             "opus:AA-lisaid.c1 sol:AA-clxcon2.c1/c2/c3/c4/c5 sol:AA-clxdat.c1]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
