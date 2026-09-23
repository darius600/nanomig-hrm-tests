// aa-copper-instruction-format -- agnus_copper conformance test
//
// Test purpose: the AA specification's alphabetical entry for COPINS prints
//   the Coprocessor's whole instruction encoding as one four-column bit
//   table -- MOVE's two words, WAIT's two, SKIP's two -- and the COPCON entry
//   describes CDANG. This grades the encoding at the module that executes it.
//   It is the complement of aa-copper-register-map, which grades where the
//   eight registers are decoded and names the instruction set as a non-goal.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 19 to 22. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the Copper's DMA priority against the
//   other channels, its cycle counts, the display it drives, or the data half
//   of a MOVE.
//
// AND IT DOES NOT ASSERT THE CHIP-SET SPLIT IN THE DANGER RANGE. agnus_copper
//   blocks a wider range on the older chip set -- `ir2[8:6]==3'b000 && !ecs`
//   beside the CDANG test -- and a fifth assertion measured exactly that
//   before it was removed. The AA entry says only that CDANG `allows the
//   Coprocessor access to all Registers`; it prints no chip-set qualifier, so
//   asserting one would be this package claiming more than its oracle does.
//
// THE DATA HALF OF A MOVE IS NOT OBSERVABLE HERE AND THE LOT SAYS SO.
//   agnus_copper has no data output port: `reg_address_out` and `address_out`
//   are the only two. RD15 to RD00 -- the second column of the table -- reach
//   the bus from the instruction register at the top level, not from this
//   module. Every claim about them is declined with that named.
//
// WHAT THE BIT TABLE SAYS, IN THE COLUMNS THIS BOUNDARY CAN SHOW:
//
//     00   0      RD00    1      0       1      1
//          ^MOVE          ^WAIT  ^WAIT   ^SKIP  ^SKIP
//          word1          word1  word2   word1  word2
//
//   Bit 0 of the first word separates a MOVE from a WAIT or a SKIP, and bit 0
//   of the second separates the WAIT from the SKIP. Bits 8 to 1 of a MOVE's
//   first word are DA8 to DA1, the destination register address.
//
// HOW AN INSTRUCTION IS FED WITHOUT MEMORY. The Copper fetches its program
//   over DMA, and this fixture models no memory -- so it writes COPINS, the
//   register the fetch itself targets. agnus_copper.v:184-187 is
//
//     if (reg_address_in[8:1]==COPINS[8:1]) begin ir1[15:1] <= ir2[15:1];
//                                                 ir2[15:0] <= data_in; end
//
//   AND THE WORD CARRYING THE DESTINATION HAS TO BE WRITTEN SECOND. The MOVE
//   puts `ir2[8:1]` on the bus (:216), and ir2 is whatever arrived last. This
//   fixture fed the pair in the document's own order first and measured a
//   Copper that executed nothing; reversing the two writes makes every
//   assertion below pass. The module's own comment at :211 raises exactly that
//   question about itself -- `IR2? is this a bug? then check how ir1/ir2 are
//   loaded in this design` -- and this package does not answer it. What it
//   grades is what the instruction ENCODES, not which register holds it while
//   it runs.
//
// THE MODULE IS BEAM-DRIVEN AND THE FIXTURE HAS TO BE TOO. `clk_ena =
//   hpos[0]`, `enable = ~bus_blk & bus_ena & clk_ena`, and `dma_ack = ackdma &
//   enable`. A fixture that holds hpos still reads as a dead decoder, which is
//   why every step here advances it.

#include "Vagnus_copper.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

namespace {
// Per-assertion mutation selector, the same form the rest of the suite uses:
// MUTATE names ONE assertion's own label, and only that assertion's stimulus
// site changes. Every OTHER assertion stays on its real stimulus and is a
// live control on each run.
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

// Printing a value with std::to_string under an "0x" label prints decimal
// digits behind a hexadecimal sign. aa-copper-register-map lost an hour to
// that; this package borrows its repair.
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

constexpr uint16_t A_COPCON  = 0x02E;
constexpr uint16_t A_COPINS  = 0x08C;

class Harness {
public:
    explicit Harness(bool ecs = true) : dut(new Vagnus_copper), hpos(0x40) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->ecs = ecs ? 1 : 0;
        dut->ackdma = 0; dut->enadma = 1; dut->sof = 0; dut->blit_busy = 0;
        dut->vpos = 0x20; dut->data_in = 0; dut->reg_address_in = 0;
        for (int i = 0; i < 16; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 8; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() {
        dut->hpos = hpos & 0x1FF;
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        hpos = (hpos + 1) & 0x1FF;
        if (hpos == 0) hpos = 0x40;         // stay clear of $E1/$E2
    }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    // Feed one instruction pair the way a fetch would leave it.
    void feed(uint16_t word1, uint16_t word2) {
        // ir1 <= ir2, ir2 <= data_in on every COPINS write, and the MOVE puts
        // ir2[8:1] on the bus -- so the word carrying the destination has to
        // be the SECOND one written. The module's own comment at :211 raises
        // exactly that question about itself.
        write(A_COPINS, word2);
        write(A_COPINS, word1);
    }

    uint8_t reg_address_out() const { return dut->reg_address_out & 0xFF; }

    // Does the Copper put `want` on reg_address_out while it runs?
    //
    // The bus alternates: COPINS on the fetch, then the instruction's own
    // address on the MOVE, then COPINS again. A scan that stopped at the first
    // value other than idle would stop on the FETCH every time and never see
    // the MOVE.
    bool emits(uint8_t want, int ccks = 80) {
        bool seen = false;
        dut->ackdma = 1;
        for (int i = 0; i < ccks && !seen; ++i) {
            step();
            if (reg_address_out() == want) seen = true;
        }
        dut->ackdma = 0;
        for (int i = 0; i < 4; ++i) step();
        return seen;
    }

    Vagnus_copper* dut;
    uint32_t hpos;
};

// A MOVE whose destination is `word_addr`: first word carries DA8-DA1 with
// bit 0 clear, second word carries the data.
static bool move_reaches(Harness& h, uint16_t word_addr, uint16_t data,
                         uint16_t bit0 = 0) {
    h.feed((uint16_t)((word_addr & 0x1FE) | bit0), data);
    return h.emits((uint8_t)((word_addr >> 1) & 0xFF));
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    // A destination well above $080, so CDANG never decides it.
    constexpr uint16_t A_HIGH  = 0x180;      // COLOR00
    constexpr uint16_t A_OTHER = 0x182;      // its neighbour
    constexpr uint16_t A_LOW   = 0x040;      // BLTCON0, below the danger line

    // MUTATION. Each guard below tests its own assertion's label and moves
    // ONE thing. MUTATE=<label> selects exactly one.
    const uint16_t m_dest  = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? A_OTHER : A_HIGH;
    const uint16_t m_bit0  = mutating("MOVE-IS-BIT-0-CLEAR") ? 0u : 1u;
    const uint16_t m_cdang = mutating("CDANG-OPENS-THE-LOW-REGISTERS") ? 0u : 0x0002u;

    std::cout << "aa-copper-instruction-format -- AA_CONFORMANCE at agnus_copper\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "others must stay on their baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // assertion's own id looks for the neighbouring register's address
        // instead of the one fed.
        h.feed(A_HIGH & 0x1FE, 0x1234);
        const bool seen = h.emits((uint8_t)((m_dest >> 1) & 0xFF));
        c.note(std::string("a MOVE to ") + hex(A_HIGH) + " put " +
               hex((A_HIGH >> 1) & 0xFF) + " on the register address bus: " +
               (seen ? "yes" : "no"));
        c.expect(seen,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: an instruction pair written "
                 "into COPINS is executed as a MOVE and its destination "
                 "reaches the register address bus, so every assertion below "
                 "is against an instruction this fixture fed");
    }

    // --- 2. The destination address is the first word's bits 8 to 1 ---------
    {
        // MUTATION MOVE-DESTINATION-IS-DA8-TO-DA1 stimulus red: selecting
        // this assertion's own id looks for the neighbouring register's
        // address, two apart from the one the instruction names.
        bool ok = true;
        std::ostringstream o;
        for (uint16_t a : {(uint16_t)0x100, (uint16_t)0x180, (uint16_t)0x1FE}) {
            Harness h;
            const uint16_t want = mutating("MOVE-DESTINATION-IS-DA8-TO-DA1")
                                       ? (uint16_t)(a ^ 0x0002) : a;
            h.feed(a & 0x1FE, 0x5555);
            const bool seen = h.emits((uint8_t)((want >> 1) & 0xFF));
            if (!seen) {
                ok = false;
                o << (o.tellp() ? ", " : "") << hex(a);
            }
        }
        c.note(std::string("three MOVE destinations reached the bus: ") +
               (ok ? "all of them" : "missing " + o.str()));
        c.expect(ok,
                 "MOVE-DESTINATION-IS-DA8-TO-DA1: the register a MOVE writes "
                 "is the one named by bits 8 to 1 of its first word, which is "
                 "the DA8-DA1 column the AA bit table prints for a MOVE "
                 "[p.20] [claims: opus:AA-copins.c1/c2/c4/c6 sol:AA-copins.c1/c2/c3/c4/c5/c6/c8 opus:AA-copins.d1 opus:AA-copins.d2 opus:AA-copins.d3 opus:AA-copins.d4 opus:AA-copins.d5 opus:AA-copins.d6 opus:AA-copins.d7 opus:AA-copins.d8 opus:AA-copins.move-ir1-b01 opus:AA-copins.move-ir1-b02 opus:AA-copins.move-ir1-b03 opus:AA-copins.move-ir1-b04 opus:AA-copins.move-ir1-b05 opus:AA-copins.move-ir1-b06 opus:AA-copins.move-ir1-b07 opus:AA-copins.move-ir1-b08 opus:AA-copins.move-ir1-b09 opus:AA-copins.move-ir1-b10 opus:AA-copins.move-ir1-b11 opus:AA-copins.move-ir1-b12 opus:AA-copins.move-ir1-b13 opus:AA-copins.move-ir1-b14 opus:AA-copins.move-ir1-b15]");
    }

    // --- 3. Bit 0 of the first word separates a MOVE from a WAIT ------------
    {
        Harness h;
        // MUTATION MOVE-IS-BIT-0-CLEAR stimulus red: selecting this
        // assertion's own id feeds bit 0 clear in the half that must not
        // move, so a MOVE happens after all.
        const bool as_move = move_reaches(h, A_HIGH, 0x1234, 0);
        Harness g;
        const bool as_wait = move_reaches(g, A_HIGH, 0x0000, m_bit0);
        c.note(std::string("with bit 0 of the first word clear the MOVE runs: ") +
               (as_move ? "yes" : "no") + "; with it set: " +
               (as_wait ? "yes" : "no"));
        c.expect(as_move && !as_wait,
                 "MOVE-IS-BIT-0-CLEAR: the same destination reaches the "
                 "register address bus when bit 0 of the first word is clear "
                 "and does not when it is set, which is the row the AA bit "
                 "table prints as 0 for a MOVE and 1 for a WAIT or SKIP "
                 "[p.20] [claims: opus:AA-copins.move-ir1-b00]");
    }

    // --- 4. CDANG opens the registers below the danger line -----------------
    {
        Harness h;
        // MUTATION CDANG-OPENS-THE-LOW-REGISTERS stimulus red: selecting
        // this assertion's own id leaves the danger bit clear in the half
        // that must open, so the low register is blocked in both runs.
        h.write(A_COPCON, m_cdang);
        const bool with_cdang = move_reaches(h, A_LOW, 0x1234, 0);
        Harness g;
        g.write(A_COPCON, 0x0000);
        const bool without = move_reaches(g, A_LOW, 0x1234, 0);
        c.note(std::string("a MOVE to ") + hex(A_LOW) +
               " reaches the bus with CDANG set: " +
               (with_cdang ? "yes" : "no") + ", with it clear: " +
               (without ? "yes" : "no"));
        c.expect(with_cdang && !without,
                 "CDANG-OPENS-THE-LOW-REGISTERS: a MOVE below the danger line "
                 "reaches the register address bus only while COPCON bit 1 is "
                 "set, which is what the AA entry means by a one bit register "
                 "that allows the Coprocessor access to all registers "
                 "[p.19] [claims: opus:AA-copcon.c8/c10]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary: RD15 to RD00, the data half of a MOVE. "
             "agnus_copper has two outputs, reg_address_out and address_out, and "
             "no data port: the instruction's data reaches the bus from the "
             "instruction register at the top level [claims: "
             "opus:AA-copins.move-ir2-b00 opus:AA-copins.move-ir2-b01 "
             "opus:AA-copins.move-ir2-b02 opus:AA-copins.move-ir2-b03 "
             "opus:AA-copins.move-ir2-b04 opus:AA-copins.move-ir2-b05 "
             "opus:AA-copins.move-ir2-b06 opus:AA-copins.move-ir2-b07 "
             "opus:AA-copins.move-ir2-b08 opus:AA-copins.move-ir2-b09 "
             "opus:AA-copins.move-ir2-b10 opus:AA-copins.move-ir2-b11 "
             "opus:AA-copins.move-ir2-b12 opus:AA-copins.move-ir2-b13 "
             "opus:AA-copins.move-ir2-b14 opus:AA-copins.move-ir2-b15]");
    c.record(
             "DECLINED at this boundary: the WAIT and SKIP columns of the table "
             "and the sentences about them. Their comparison runs inside this "
             "module, but showing one needs the Copper started from a loaded "
             "pointer and a beam walked past the compare value [claims: "
             "opus:AA-copcon.c1/c2/c7 opus:AA-copins.c7/c8/c9/c10 "
             "opus:AA-copper.c4/c5/c10/c12/c13/c15 sol:AA-copins.c9/c10/c11 "
             "sol:AA-copper.c4/c5/c7/c10/c11/c13/c14 opus:AA-copins.d10 "
             "opus:AA-copins.d11 opus:AA-copins.d12 opus:AA-copins.d13 "
             "opus:AA-copins.d9 opus:AA-copins.skip-ir1-b00 "
             "opus:AA-copins.skip-ir1-b01 opus:AA-copins.skip-ir1-b02 "
             "opus:AA-copins.skip-ir1-b03 opus:AA-copins.skip-ir1-b04 "
             "opus:AA-copins.skip-ir1-b05 opus:AA-copins.skip-ir1-b06 "
             "opus:AA-copins.skip-ir1-b07 opus:AA-copins.skip-ir1-b08 "
             "opus:AA-copins.skip-ir1-b09 opus:AA-copins.skip-ir1-b10 "
             "opus:AA-copins.skip-ir1-b11 opus:AA-copins.skip-ir1-b12 "
             "opus:AA-copins.skip-ir1-b13 opus:AA-copins.skip-ir1-b14 "
             "opus:AA-copins.skip-ir1-b15 opus:AA-copins.skip-ir2-b00 "
             "opus:AA-copins.skip-ir2-b01 opus:AA-copins.skip-ir2-b02 "
             "opus:AA-copins.skip-ir2-b03 opus:AA-copins.skip-ir2-b04 "
             "opus:AA-copins.skip-ir2-b05 opus:AA-copins.skip-ir2-b06 "
             "opus:AA-copins.skip-ir2-b07 opus:AA-copins.skip-ir2-b08 "
             "opus:AA-copins.skip-ir2-b09 opus:AA-copins.skip-ir2-b10 "
             "opus:AA-copins.skip-ir2-b11 opus:AA-copins.skip-ir2-b12 "
             "opus:AA-copins.skip-ir2-b13 opus:AA-copins.skip-ir2-b14 "
             "opus:AA-copins.skip-ir2-b15 opus:AA-copins.wait-ir1-b00 "
             "opus:AA-copins.wait-ir1-b01 opus:AA-copins.wait-ir1-b02 "
             "opus:AA-copins.wait-ir1-b03 opus:AA-copins.wait-ir1-b04 "
             "opus:AA-copins.wait-ir1-b05 opus:AA-copins.wait-ir1-b06 "
             "opus:AA-copins.wait-ir1-b07 opus:AA-copins.wait-ir1-b08 "
             "opus:AA-copins.wait-ir1-b09 opus:AA-copins.wait-ir1-b10 "
             "opus:AA-copins.wait-ir1-b11 opus:AA-copins.wait-ir1-b12 "
             "opus:AA-copins.wait-ir1-b13 opus:AA-copins.wait-ir1-b14 "
             "opus:AA-copins.wait-ir1-b15 opus:AA-copins.wait-ir2-b00 "
             "opus:AA-copins.wait-ir2-b01 opus:AA-copins.wait-ir2-b02 "
             "opus:AA-copins.wait-ir2-b03 opus:AA-copins.wait-ir2-b04 "
             "opus:AA-copins.wait-ir2-b05 opus:AA-copins.wait-ir2-b06 "
             "opus:AA-copins.wait-ir2-b07 opus:AA-copins.wait-ir2-b08 "
             "opus:AA-copins.wait-ir2-b09 opus:AA-copins.wait-ir2-b10 "
             "opus:AA-copins.wait-ir2-b11 opus:AA-copins.wait-ir2-b12 "
             "opus:AA-copins.wait-ir2-b13 opus:AA-copins.wait-ir2-b14 "
             "opus:AA-copins.wait-ir2-b15]");
    c.record(
             "DECLINED at this boundary: the claims counting bus cycles per "
             "instruction. This fixture grants every slot the Copper asks for "
             "rather than modelling the allocation it shares with the other DMA "
             "channels, so a cycle count here would be the fixture's [claims: "
             "opus:AA-copins.c5 opus:AA-copper.c1/c2/c3/c6/c7/c8/c9 "
             "sol:AA-copins.c7 sol:AA-copper.c1/c2/c3/c6/c8/c9]");
    c.record(
             "DECLINED at this boundary: the sentences telling a programmer what "
             "to initialise before starting Coprocessor DMA. The module starts "
             "when it is strobed, whatever was initialised first [claims: "
             "opus:AA-copper.c11/c14 sol:AA-copper.c12/c15/c16/c17/c18]");
    c.record(
             "DECLINED at this boundary: the chip column. No port of "
             "agnus_copper carries a chip identity [claims: "
             "opus:AA-cop1lch.c3/c5/c6/c7 opus:AA-cop1lcl.c3 "
             "opus:AA-cop2lch.c3/c5/c6 opus:AA-cop2lcl.c3 "
             "opus:AA-copcon.c3/c4/c11 opus:AA-copins.c3 opus:AA-copjmp1.c3/c4 "
             "opus:AA-copjmp2.c3/c4 sol:AA-cop1lch.c1/c2/c3/c4/c5 "
             "sol:AA-cop2lch.c1/c2/c3/c4/c5 "
             "sol:AA-copcon.c1/c2/c3/c5/c6/c7/c8/c9/c10/c11]");
    c.record(
             "DECLINED at this boundary: the claims that the document's two "
             "statements of a register differ from each other [claims: "
             "opus:AA-copcon.c5/c6/c9 sol:AA-cop1lch.c6 sol:AA-cop1lcl.c5 "
             "sol:AA-cop2lch.c6 sol:AA-cop2lcl.c5 sol:AA-copcon.c4]");
    c.record(
             "DECLINED at this boundary: the pointer and jump registers, which "
             "aa-copper-register-map grades at this same boundary through the "
             "chip address a strobe loads. A record here would be a second home "
             "on one measurement [claims: opus:AA-cop1lch.c1/c2/c4 "
             "opus:AA-cop1lcl.c1/c2/c4/c5 opus:AA-cop2lch.c1/c2/c4/c7 "
             "opus:AA-cop2lcl.c1/c2/c4/c5 opus:AA-copjmp1.c1/c2 "
             "opus:AA-copjmp2.c1/c2 sol:AA-cop1lcl.c1/c2/c3/c4 "
             "sol:AA-cop2lcl.c1/c2/c3/c4 sol:AA-copjmp1.c1/c2/c3/c4 "
             "sol:AA-copjmp2.c1/c2/c3/c4]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
