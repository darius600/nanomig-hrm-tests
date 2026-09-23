// aa-controller-port-bit-tables -- userio conformance test
//
// Test purpose: the AA specification's alphabetical entries for JOY0DAT,
//   JOY1DAT, JOYTEST and POTINP print each register as a bit table, and the
//   joystick tables print the same register twice -- once as eight-bit
//   counters and once with the two lowest bits of each marked xx. This grades
//   the layout at the module that holds the counters. It is the complement of
//   aa-controller-port-registers, which grades where the seven registers are
//   decoded.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 25 to 27. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the quadrature decoding, the pot
//   capacitor timing, the keyboard, or the four-bit bidirectional port POTGO
//   shares with the pot pins.
//
// THE TABLE IS Y ABOVE X AND JOYTEST IS HOW IT IS FILLED. The entry prints
//
//     JOY0DAT   Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  X7 X6 X5 X4 X3 X2 X1 X0
//
//   and the JOYTEST entry says `Write to all 4 Joystick-mouse counters at
//   once.` So one write at $036 puts a known word into both ports' counters,
//   and reading $00A back shows which half went where. No quadrature motion
//   is needed, which is this package's first non-goal.
//
// THE SECOND TABLE IS THE SAME REGISTER WITH FOUR BITS MASKED, AND IT MEANS
//   IT. The entry prints JOY0DAT a second time as
//
//     JOY0DAT   Y7 Y6 Y5 Y4 Y3 Y2 xx xx  X7 X6 X5 X4 X3 X2 xx xx
//
//   and this RTL masks exactly those four. MEASURED: all ones written through
//   JOYTEST come back as $FCFC. The first version of assertion 4 asserted the
//   opposite -- that the marks described what a joystick drives rather than
//   what the register keeps -- and went red against a register that was
//   following the document. The first three assertions use $5C3C for the same
//   reason: a word whose masked bits are already clear survives the round
//   trip, and $5A3C does not.

#include "Vuserio.h"
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

constexpr uint16_t A_JOY0DAT = 0x00A;
constexpr uint16_t A_JOY1DAT = 0x00C;
constexpr uint16_t A_POTINP  = 0x016;
constexpr uint16_t A_JOYTEST = 0x036;

class Harness {
public:
    Harness() : dut(new Vuserio) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->pot_cnt_en = 0;
        dut->_fire0_dat = 1; dut->_fire1_dat = 1;
        dut->_joy1 = 0xFFFF; dut->_joy2 = 0xFFFF;   // active low: nothing pressed
        dut->joy_ana1 = 0; dut->joy_ana2 = 0;
        // mouse_btn is active HIGH; zero is nothing pressed. The sibling
        // package lost an assertion to holding it at seven.
        dut->mouse_btn = 0; dut->kms_level = 0;
        dut->kbd_mouse_type = 0; dut->kbd_mouse_data = 0;
        dut->reg_address_in = 0; dut->data_in = 0;
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

    uint16_t read(uint16_t word_addr) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = (uint16_t)dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    Vuserio* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    constexpr uint16_t WORD = 0x5C3C;      // distinct halves, and a word
                                       // that survives the mask below

    // MUTATION. Each guard below tests its own assertion's label and moves
    // ONE thing; MUTATE=<label> selects exactly one.
    const uint16_t m_port = mutating("JOYTEST-FILLS-BOTH-PORTS") ? A_POTINP : A_JOY1DAT;      // 3: wrong register
    const uint16_t m_all  = mutating("JOYDAT-MASKS-THE-TWO-LOWEST-BITS-OF-EACH-AXIS")
                                 ? (uint16_t)0x0000u : (uint16_t)0xFFFFu;         // 4: nothing set

    std::cout << "aa-controller-port-bit-tables -- AA_CONFORMANCE at userio\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "others must stay on their baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // assertion's own id writes the two halves the other way round, so
        // the word read back is not the one compared against.
        const uint16_t word1 = mutating("FIXTURE-DELIVERS-KNOWN-VALUE")
                                    ? (uint16_t)0x3C5Cu : WORD;
        h.write(A_JOYTEST, word1);
        const uint16_t got = h.read(A_JOY0DAT);
        c.note("after one JOYTEST write of " + hex(WORD) +
               ", JOY0DAT reads " + hex(got));
        c.expect(got == WORD,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a word written at $036 comes "
                 "back whole from $00A, so every assertion below is against a "
                 "counter value this fixture put there");
    }

    // --- 2. The table is Y above X ------------------------------------------
    {
        Harness h;
        // MUTATION JOYDAT-IS-Y-ABOVE-X stimulus red: selecting this
        // assertion's own id writes the two halves the other way round, so
        // the high byte carries what the low byte is compared against and
        // neither half matches.
        const uint16_t word2 = mutating("JOYDAT-IS-Y-ABOVE-X")
                                    ? (uint16_t)0x3C5Cu : WORD;
        h.write(A_JOYTEST, word2);
        const uint16_t got = h.read(A_JOY0DAT);
        c.note("the high byte reads " + hex((got >> 8) & 0xFF) +
               " and the low byte " + hex(got & 0xFF));
        c.expect(((got >> 8) & 0xFF) == 0x5C && (got & 0xFF) == 0x3C,
                 "JOYDAT-IS-Y-ABOVE-X: the high byte of $00A carries the value "
                 "written into the vertical counter and the low byte the "
                 "horizontal one, which is the Y7-Y0 over X7-X0 the AA table "
                 "prints for JOY0DAT [p.25] [claims: sol:AA-joy0dat.c1/c2/c3/c4 sol:AA-joy0dat-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16 sol:AA-joy1dat.c1/c2/c3/c4 sol:AA-joy1dat-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16 sol:AA-joydat.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14 sol:AA-joytest-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28]");
    }

    // --- 3. And JOYTEST fills both ports -------------------------------------
    {
        Harness h;
        h.write(A_JOYTEST, WORD);
        const uint16_t port0 = h.read(A_JOY0DAT);
        // MUTATION JOYTEST-FILLS-BOTH-PORTS stimulus red: selecting this
        // assertion's own id reads the pot pin register instead of the
        // second port, which answers with something else entirely.
        const uint16_t port1 = h.read(m_port);
        c.note("after one write, port 0 reads " + hex(port0) +
               " and port 1 reads " + hex(port1));
        c.expect(port0 == WORD && port1 == WORD,
                 "JOYTEST-FILLS-BOTH-PORTS: one word written at $036 comes "
                 "back from both $00A and $00C, which is what the AA entry "
                 "means by writing all four joystick-mouse counters at once "
                 "[p.26] [claims: sol:AA-joytest.c1/c2/c3/c4]");
    }

    // --- 4. What the second table's masked bits actually do -----------------
    {
        Harness h;
        // MUTATION JOYDAT-MASKS-THE-TWO-LOWEST-BITS-OF-EACH-AXIS stimulus red:
        // selecting this assertion's own id writes zero, so no bit is set
        // that the mask could clear and the read-back cannot show which
        // four it clears.
        h.write(A_JOYTEST, m_all);
        const uint16_t got = h.read(A_JOY0DAT);
        c.note("after writing all ones, JOY0DAT reads " + hex(got) +
               "; the second AA table marks bits 9, 8, 1 and 0 as xx");
        c.expect(got == 0xFCFC,
                 "JOYDAT-MASKS-THE-TWO-LOWEST-BITS-OF-EACH-AXIS: writing every "
                 "bit through JOYTEST leaves bits 9, 8, 1 and 0 clear, which is "
                 "exactly the four the AA entry's second table marks xx -- so "
                 "that table is a statement about the register and not about "
                 "what a joystick happens to drive [p.25] "
                 "[claims: sol:AA-joytest-bits.c13/c14/c15/c16/c29/c30/c31/c32]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about the "
             "four-bit bidirectional port POTGO shares with the pot pins, or "
             "about the keyboard and mouse lines. Those reach userio on pins "
             "this package holds idle. [claims: sol:AA-potinp.c6]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about the "
             "pot counters' analogue behaviour -- the capacitor dump, the charge "
             "time, the value a paddle returns. userio counts while an enable is "
             "held, and this fixture models no charge. [claims: "
             "sol:AA-potgo.c1/c2/c4 sol:AA-potinp.c1/c2/c4/c7]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about the "
             "quadrature decoding that turns pin motion into counter steps. This "
             "package writes the counters through JOYTEST instead, which is its "
             "first non-goal. [claims: sol:AA-mouse-serial.c2/c4/c6/c8]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of userio carries a chip "
             "identity. [claims: opus:AA-adkcon.c1/c2 "
             "opus:AA-adkcon-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c38/c39/c40 "
             "opus:AA-adkconr.c1/c2/c3 "
             "opus:AA-audxdat.c1/c2/c3/c5/c6/c7/c8/c9/c10/c11/c12 "
             "opus:AA-audxlc.c1/c2 opus:AA-audxlcl.c1/c2/c3/c5 "
             "opus:AA-audxlen.c1/c2/c3/c4 "
             "opus:AA-audxvol.c1/c2/c3/c4/c5/c6/c7/c8/c9 "
             "opus:AA-joy0dat.c1/c2/c3 opus:AA-joy1dat.c1/c2/c3 "
             "opus:AA-joydat.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c38/c39/c40/c41/c42/c43/c44/c45/c46/c47/c48/c49/c50/c51/c52/c53/c54/c55/c56/c57/c58/c59 "
             "opus:AA-joytest.c1/c2/c3/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37]");
    c.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "somewhere in this design but not at the boundary this package "
             "drives, and no other slice 5 lot claims it. Recorded so the slice "
             "closes with nothing unclassified. [claims: "
             "sol:AA-audxlcl.c1/c2/c3/c4 "
             "sol:AA-interrupts.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18 "
             "sol:AA-joystick.c1/c2/c3/c4 sol:AA-mouse-serial.c1/c3/c5/c7 "
             "sol:AA-potinp-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10 "
             "sol:AA-refptr.c1/c2/c3/c6/c7/c8 "
             "sol:AA-serdat-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16 "
             "sol:AA-serdatr-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16 "
             "sol:AA-serper.c1/c2/c4/c6/c7 sol:AA-serper-bits.c1/c2]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- a mark, a legend, or a disagreement between its two "
             "statements of a register. [claims: opus:AA-adkcon.c3/c4 "
             "opus:AA-adkconr.c4/c5 opus:AA-audxdat.c4 "
             "opus:AA-audxlch.c1/c2/c3/c4/c5/c6 opus:AA-audxlcl.c4 "
             "opus:AA-audxper.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11 "
             "opus:AA-intenar.c1/c2/c3/c4/c5 "
             "opus:AA-intenar-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c38/c39/c40/c41/c42/c43 "
             "opus:AA-joy0dat.c4/c5 opus:AA-joy1dat.c4/c5 opus:AA-joytest.c4 "
             "opus:AA-legend.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18 "
             "opus:AA-lisaid.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21 "
             "opus:AA-pot0dat.c1/c2/c3/c4/c5/c6 "
             "opus:AA-pot1dat.c1/c2/c3/c4/c5/c6 "
             "opus:AA-potdat.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c38/c39/c40/c41/c42/c43/c44/c45 "
             "opus:AA-potgo.c1/c2/c3/c4/c5 "
             "opus:AA-potinp.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26 "
             "opus:AA-refptr.c1/c2/c3/c4/c5/c6/c7 "
             "opus:AA-serdat.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28 "
             "opus:AA-serdatr.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28 "
             "opus:AA-serper.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11 "
             "opus:AA-sprhdat.c1/c2/c3/c4/c5/c6 opus:AA-sprhpt.c1/c2 "
             "opus:AA-sprhpth.c1/c2/c3/c4/c5/c6 "
             "opus:AA-sprhptl.c1/c2/c3/c4/c5/c6 sol:AA-adkcon.c3 "
             "sol:AA-adkconr.c3/c5 sol:AA-audio-location.c1/c2/c3 "
             "sol:AA-audxdat.c3/c4 sol:AA-audxlch.c1/c2/c3/c4/c5/c6/c7 "
             "sol:AA-audxlcl.c5 sol:AA-audxlen.c3 "
             "sol:AA-audxper.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11 "
             "sol:AA-audxvol.c3 sol:AA-intenar.c3/c4 sol:AA-joy0dat.c5 "
             "sol:AA-joy1dat.c5 sol:AA-joytest.c5 "
             "sol:AA-lisaid.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18 "
             "sol:AA-pot-connectors.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12 "
             "sol:AA-pot0dat.c1/c2/c3/c4/c5/c6 "
             "sol:AA-pot0dat-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16 "
             "sol:AA-pot1dat.c1/c2/c3/c4/c5/c6 "
             "sol:AA-pot1dat-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16 "
             "sol:AA-potdat.c1/c2/c3/c4/c5 sol:AA-potgo.c3/c5 "
             "sol:AA-potinp.c3/c5 sol:AA-refptr.c4/c5 sol:AA-serdat.c3/c5 "
             "sol:AA-serdatr.c3/c5 sol:AA-serper.c3/c5 "
             "sol:AA-sprhdat.c1/c2/c3/c4/c5/c6/c7/c8 sol:AA-sprhpt.c1/c2 "
             "sol:AA-sprhpth.c1/c2/c3/c4/c5/c6 sol:AA-sprhptl.c1/c2/c3/c4/c5/c6]");
    c.record(
             "DECLINED at this boundary (outside-uut): The register is decoded "
             "in agnus or one of its sub-modules and graded there by its own AA "
             "package. [claims: sol:AA-audxdat.c1/c2/c5/c6/c7/c8/c9/c10/c11 "
             "sol:AA-audxlen.c1/c2/c4 sol:AA-audxvol.c1/c2/c4/c5/c6/c7]");
    c.record(
             "DECLINED at this boundary (outside-uut): The register is decoded "
             "in paula or one of its sub-modules -- paula_uart, "
             "paula_intcontroller, paula_floppy -- and graded there by its own "
             "AA package. [claims: "
             "sol:AA-adkcon.c1/c2/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30 "
             "sol:AA-adkconr.c1/c2/c4 sol:AA-intenar.c1/c2 "
             "sol:AA-serdat.c1/c2/c4/c6/c7/c8/c9/c10 "
             "sol:AA-serdatr.c1/c2/c4/c6/c7/c8]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
