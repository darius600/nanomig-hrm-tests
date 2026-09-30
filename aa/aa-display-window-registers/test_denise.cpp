// aa-display-window-registers -- denise conformance test
//
// Test purpose: the AA specification's alphabetical entries for DIWSTRT,
//   DIWSTOP and DIWHIGH give the display window its horizontal limits and,
//   with DIWHIGH, a ninth bit for each. This grades those limits at the module
//   that compares them against its own beam counter.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 21 and 22. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the vertical limits, the bitplane
//   fetch, sprites, HAM, or the display data fetch registers -- DDFSTRT and
//   DDFSTOP are decoded in agnus_bitplanedma, a different module.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `opus:AA-diw.c7` and `opus:AA-diwstrt.c1` each name one reader's reading
//   of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s4.json lists under `inventories`.
//
// THE WINDOW IS MADE VISIBLE BY BLANKING THE BORDER, NOT BY DRAWING ANYTHING.
//   With no bitplane data the colour inside the window and the border colour
//   are the same entry, so a fixture that only looked at the colour would see
//   one flat line. denise.v:515 gives the way through:
//
//     t_blank = ecs & ecsena & brdrblnk & (~window_del | ~display_ena)
//     {red,green,blue} = t_blank ? 24'h000000 : out_rgb
//
//   So with ECSENA (BPLCON0 bit 0) and BRDRBLNK (BPLCON3 bit 5) set, and a
//   non-zero COLOR00 in the table, the video output is that colour INSIDE the
//   window and black outside it. The edges are then countable.
//
//   `display_ena` is the fourth term and it is not automatic: denise.v:146-151
//   sets it on a write to BPL1DAT and clears it on reset or `blank`. This
//   fixture writes BPL1DAT once per line, which is what a real scanline does.
//
// THE BEAM COUNTER IS INTERNAL. denise.v:136-141 counts hpos on every
//   clk7_en and resets it to 2 on `strhor`, so the fixture pulses strhor to
//   start a line and then steps. Nothing here drives hpos directly.
//
// AND THE TWO REGISTERS DO NOT CARRY THE SAME NINTH BIT. denise.v:295-320: a
//   write to DIWSTRT forces hdiwstrt[8] to 0, a write to DIWSTOP forces
//   hdiwstop[8] to 1, and only DIWHIGH -- gated on `ecs` -- sets either from
//   data. That asymmetry is what makes the window's right edge default to the
//   far half of the line, and it is why assertion 4 writes DIWHIGH last.
//
//   ONE DIWHIGH WRITE CARRIES BOTH NINTH BITS, data_in[5] for the start and
//   data_in[13] for the stop, and a write that sets only one of them moves
//   the OTHER to zero. Writing bit 5 alone would
//   measure a line lit from position 0: hdiwstop[8] would go to 0, putting
//   the stop at 192 and the start at 320, so the window would open near the
//   end of one line and stay open at the beginning of the next -- the
//   register behaving as the page describes it, not a fixture misreading it.
//
// THE CHIP-SET CONDITION ON DIWHIGH CANNOT BE SHOWN HERE EITHER. The write
//   is gated on `ecs`, and so is the border blanking this package sees the
//   window through: `t_blank = ecs & ecsena & brdrblnk & ...`. On the older
//   chip set nothing is blanked, so the whole line is lit and the first lit
//   position is 0 whatever the limits are. An assertion that compared the
//   two chip sets passed -- and was measuring the blanking, not the bit.
//   Assertion 4 compares a DIWHIGH write against no DIWHIGH write on the
//   same chip set instead, and the lot declines the chip-set claim.

#include "Vdenise.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

constexpr uint16_t A_DIWSTRT = 0x08E;
constexpr uint16_t A_DIWSTOP = 0x090;
constexpr uint16_t A_DIWHIGH = 0x1E4;
constexpr uint16_t A_BPLCON0 = 0x100;
constexpr uint16_t A_BPLCON2 = 0x104;
constexpr uint16_t A_BPLCON3 = 0x106;
constexpr uint16_t A_BPL1DAT = 0x110;
constexpr uint16_t A_COLOR00 = 0x180;

constexpr uint16_t ECSENA   = 1u << 0;    // BPLCON0
constexpr uint16_t BRDRBLNK = 1u << 5;    // BPLCON3

class Harness {
public:
    explicit Harness(bool ecs = true) : dut(new Vdenise) {
        dut->clk = 0; dut->clk7_en = 1;
        dut->c1 = 0; dut->c3 = 0; dut->cck = 0;
        dut->reset = 1; dut->strhor = 0; dut->blank = 0;
        dut->a1k = 0; dut->ecs = ecs ? 1 : 0; dut->aga = 1;
        dut->chip48 = 0;
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

    // Everything a line needs: the border blanked, a colour in entry 0, and
    // the display enabled by the write a real scanline makes anyway.
    void arm(uint16_t colour) {
        write(A_BPLCON2, 0);              // rdram low: the colour table takes writes
        write(A_BPLCON3, BRDRBLNK);
        write(A_BPLCON0, ECSENA);
        write(A_COLOR00, colour);
        write(A_BPL1DAT, 0x0000);         // sets display_ena, plane data all zero
    }

    // One line: strhor puts hpos back to 2, then every step advances it.
    // Returns, for each hpos from 2 upward, whether the video output is lit.
    //
    // A SETTLING LINE RUNS FIRST AND IT IS NOT OPTIONAL. `window` is a latch:
    // set when hpos reaches hdiwstrt, cleared when it reaches hdiwstop, and
    // untouched by strhor. Measured straight after the register writes it is
    // still whatever the previous activity left it, and reading it that way
    // would show a line lit from position 0 for every start value.
    std::vector<bool> line(int pixels = 520) {
        for (int settle = 0; settle < 2; ++settle) {
            dut->strhor = 1;
            step();
            dut->strhor = 0;
            if (settle == 0) for (int i = 0; i < pixels; ++i) step();
        }
        std::vector<bool> lit;
        lit.reserve(pixels);
        for (int i = 0; i < pixels; ++i) {
            step();
            dut->eval();
            lit.push_back((dut->red | dut->green | dut->blue) != 0);
        }
        return lit;
    }

    Vdenise* dut;
};

// The first and last index at which the line is lit, or -1.
static int first_lit(const std::vector<bool>& v) {
    for (size_t i = 0; i < v.size(); ++i) if (v[i]) return (int)i;
    return -1;
}
static int last_lit(const std::vector<bool>& v) {
    for (int i = (int)v.size() - 1; i >= 0; --i) if (v[i]) return i;
    return -1;
}
static int count_lit(const std::vector<bool>& v) {
    int n = 0;
    for (bool b : v) if (b) ++n;
    return n;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    constexpr uint16_t COLOUR = 0x0F0F;
    constexpr uint16_t START_A = 0x0040;    // low byte is the horizontal start
    constexpr uint16_t START_B = 0x0060;
    constexpr uint16_t STOP_A  = 0x00C0;
    constexpr uint16_t STOP_B  = 0x00D0;   // $1D0 = 464, inside the line

    // MUTATION. Each guard below tests its own assertion's label and moves
    // ONE thing. MUTATE=<label> selects exactly one.
    const uint16_t m_blnk  = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? 0u : BRDRBLNK;
    const uint16_t m_start = mutating("DIWSTRT-SETS-THE-LEFT-EDGE") ? START_A : START_B;
    const uint16_t m_stop  = mutating("DIWSTOP-SETS-THE-RIGHT-EDGE") ? STOP_A : STOP_B;
    const uint16_t m_high  = mutating("DIWHIGH-IS-THE-NINTH-BIT")
                                  ? 0u : (uint16_t)((1u << 5) | (1u << 13));

    std::cout << "aa-display-window-registers -- AA_CONFORMANCE at denise\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "others must stay on their baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // assertion's own id leaves the border-blank bit clear, so the whole
        // line carries the colour and no edge can be seen at all.
        h.write(A_BPLCON2, 0);
        h.write(A_BPLCON3, m_blnk);
        h.write(A_BPLCON0, ECSENA);
        h.write(A_COLOR00, COLOUR);
        h.write(A_BPL1DAT, 0x0000);
        h.write(A_DIWSTRT, START_A);
        h.write(A_DIWSTOP, STOP_A);
        const std::vector<bool> v = h.line();
        const int n = count_lit(v);
        c.note("over a line of " + std::to_string(v.size()) + " positions, " +
               std::to_string(n) + " are lit, from " +
               std::to_string(first_lit(v)) + " to " +
               std::to_string(last_lit(v)));
        c.expect(n > 0 && n < (int)v.size(),
                 "FIXTURE-DELIVERS-KNOWN-VALUE: with the border blanked the "
                 "line is lit for part of its length and dark for the rest, so "
                 "the window has two edges this fixture can count");
    }

    // --- 2. DIWSTRT sets the left edge --------------------------------------
    {
        Harness h;
        h.arm(COLOUR);
        h.write(A_DIWSTRT, START_A);
        h.write(A_DIWSTOP, STOP_A);
        const int edge_a = first_lit(h.line());
        Harness g;
        g.arm(COLOUR);
        // MUTATION DIWSTRT-SETS-THE-LEFT-EDGE stimulus red: selecting this
        // assertion's own id writes the same start twice, so the edge
        // cannot move and the difference the assertion is about disappears.
        g.write(A_DIWSTRT, m_start);
        g.write(A_DIWSTOP, STOP_A);
        const int edge_b = first_lit(g.line());
        c.note("start $" + std::to_string(START_A & 0xFF) + " lights from " +
               std::to_string(edge_a) + ", start $" +
               std::to_string(START_B & 0xFF) + " from " +
               std::to_string(edge_b));
        c.expect(edge_a >= 0 && edge_b >= 0 &&
                 (edge_b - edge_a) == (int)((START_B & 0xFF) - (START_A & 0xFF)),
                 "DIWSTRT-SETS-THE-LEFT-EDGE: moving the value written at $08E "
                 "moves the first lit position by the same number of places, "
                 "which is what the AA entry means by the display window's "
                 "horizontal start [p.21] [claims: opus:AA-diw.c7 opus:AA-diwstrt.c1/c2/c6 sol:AA-diwstrt.c1/c2/c3/c6]");
    }

    // --- 3. DIWSTOP sets the right edge -------------------------------------
    {
        Harness h;
        h.arm(COLOUR);
        h.write(A_DIWSTRT, START_A);
        h.write(A_DIWSTOP, STOP_A);
        const int edge_a = last_lit(h.line());
        Harness g;
        g.arm(COLOUR);
        g.write(A_DIWSTRT, START_A);
        // MUTATION DIWSTOP-SETS-THE-RIGHT-EDGE stimulus red: selecting this
        // assertion's own id writes the same stop twice, so the right edge
        // cannot move.
        g.write(A_DIWSTOP, m_stop);
        const int edge_b = last_lit(g.line());
        c.note("stop $" + std::to_string(STOP_A & 0xFF) + " goes dark after " +
               std::to_string(edge_a) + ", stop $" +
               std::to_string(STOP_B & 0xFF) + " after " +
               std::to_string(edge_b));
        c.expect(edge_a >= 0 && edge_b >= 0 &&
                 (edge_b - edge_a) == (int)((STOP_B & 0xFF) - (STOP_A & 0xFF)),
                 "DIWSTOP-SETS-THE-RIGHT-EDGE: moving the value written at $090 "
                 "moves the last lit position by the same number of places, "
                 "which is what the AA entry means by the display window's "
                 "horizontal stop [p.21] [claims: opus:AA-diw.c5 opus:AA-diwstop.c1/c2/c6 sol:AA-diwstop.c1/c2/c3/c6 opus:AA-diw.use-b00 opus:AA-diw.use-b01 opus:AA-diw.use-b02 opus:AA-diw.use-b03 opus:AA-diw.use-b04 opus:AA-diw.use-b05 opus:AA-diw.use-b06 opus:AA-diw.use-b07]");
    }

    // --- 4. DIWHIGH carries the ninth bit of each limit ---------------------
    {
        Harness h(true);
        h.arm(COLOUR);
        h.write(A_DIWSTRT, START_A);
        h.write(A_DIWSTOP, STOP_A);
        const int without = first_lit(h.line());
        Harness g(true);
        g.arm(COLOUR);
        g.write(A_DIWSTRT, START_A);
        g.write(A_DIWSTOP, STOP_A);
        // MUTATION DIWHIGH-IS-THE-NINTH-BIT stimulus red: selecting this
        // assertion's own id writes zero into DIWHIGH instead, which leaves
        // both ninth bits where the two earlier writes put them and moves
        // no edge.
        g.write(A_DIWHIGH, m_high);
        const int with_high = first_lit(g.line());
        c.note("without a DIWHIGH write the line lights from " +
               std::to_string(without) + "; with both ninth bits set, from " +
               std::to_string(with_high));
        c.expect(without == (int)(START_A & 0xFF) && with_high == without + 256,
                 "DIWHIGH-IS-THE-NINTH-BIT: a write at $1E4 moves the window's "
                 "start up by exactly 256 places, which is the ninth bit "
                 "DIWSTRT's eight cannot carry and the AA entry gives to "
                 "DIWHIGH [p.22] [claims: opus:AA-diw.c6 opus:AA-diwhigh.c1/c2/c5/c7/c8/c9/c10/c11/c12/c13/c14 opus:AA-diwhigh.bit-b02 opus:AA-diwhigh.bit-b03 opus:AA-diwhigh.bit-b04 opus:AA-diwhigh.bit-b05 opus:AA-diwhigh.bit-b06 opus:AA-diwhigh.bit-b07 opus:AA-diwhigh.bit-b10 opus:AA-diwhigh.bit-b11 opus:AA-diwhigh.bit-b12 opus:AA-diwhigh.bit-b13 opus:AA-diwhigh.bit-b14 opus:AA-diwhigh.bit-b15]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of denise carries a chip "
             "identity. [claims: opus:AA-diwhigh.c3/c15 opus:AA-diwstop.c3 "
             "opus:AA-diwstrt.c3 opus:AA-dmaconr.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "somewhere in this design but not at a boundary this package "
             "drives, and no other slice 4 lot claims it. It is recorded here so "
             "the slice closes with nothing unclassified. [claims: "
             "opus:AA-clxcon-cont.c1/c2/c3 opus:AA-copjmp.c5/c6 opus:AA-coplc.c1 "
             "opus:AA-dmaconr.c1/c2/c4 sol:AA-collision-control.c1/c2/c3 "
             "sol:AA-copins-encoding.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c38/c39/c40/c41/c42/c43/c44/c45/c46/c47/c48/c49/c50/c51/c52/c53/c54/c55/c56/c57/c58/c59/c60/c61/c62/c63/c64/c65/c66/c67/c68/c69/c70/c71/c72/c73/c74/c75/c76/c77/c78/c79/c80/c81/c82/c83/c84/c85/c86/c87/c88/c89/c90/c91/c92/c93/c94/c95/c96/c97/c98/c99/c100/c101/c102/c103/c104/c105/c106/c107/c108/c109/c110 "
             "sol:AA-copjmp.c1/c2 sol:AA-coploc.c1 "
             "sol:AA-ddf-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17 "
             "sol:AA-display-window.c1/c2/c3/c4 "
             "sol:AA-display-window-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16 "
             "sol:AA-dmacon-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20 "
             "sol:AA-dmaconr.c6]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- a mark, a legend, or a disagreement between its two "
             "statements of a register. No signal carries any of those. [claims: "
             "opus:AA-diwhigh.c4/c6 opus:AA-dmaconr.c5 "
             "opus:AA-legend.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11 "
             "sol:AA-diwhigh.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10 "
             "sol:AA-diwhigh-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20 "
             "sol:AA-dmaconr.c1/c2/c3/c4/c5]");
    c.record(
             "DECLINED at this boundary (outside-uut): DDFSTRT and DDFSTOP are "
             "decoded in agnus_bitplanedma, a module of its own, and decide when "
             "the bitplane fetch starts rather than what denise displays. "
             "[claims: opus:AA-ddf.c5/c6/c7/c8/c10 "
             "opus:AA-ddfstop.c1/c2/c3/c4/c9 opus:AA-ddfstrt.c1/c2/c3/c4/c9 "
             "sol:AA-ddf.c1/c2/c3/c4/c5/c6 sol:AA-ddfstop.c1/c2/c3/c4/c5/c6/c7 "
             "sol:AA-ddfstrt.c1/c2/c3/c4/c5/c6/c7/c8 opus:AA-ddf.use-b00 "
             "opus:AA-ddf.use-b01 opus:AA-ddf.use-b02 opus:AA-ddf.use-b03 "
             "opus:AA-ddf.use-b04 opus:AA-ddf.use-b05 opus:AA-ddf.use-b06 "
             "opus:AA-ddf.use-b07 opus:AA-ddf.use-b08 opus:AA-ddf.use-b09 "
             "opus:AA-ddf.use-b10 opus:AA-ddf.use-b11 opus:AA-ddf.use-b12 "
             "opus:AA-ddf.use-b13 opus:AA-ddf.use-b14 opus:AA-ddf.use-b15 "
             "opus:AA-ddfstop.t1 opus:AA-ddfstop.t2 opus:AA-ddfstop.t3 "
             "opus:AA-ddfstrt.t1 opus:AA-ddfstrt.t2 opus:AA-ddfstrt.t3 "
             "opus:AA-ddfstrt.t4]");
    c.record(
             "DECLINED at this boundary (outside-uut): The claim is about the "
             "window's VERTICAL limits. denise compares only the horizontal ones "
             "against its own hpos; the vertical window is decided in agnus and "
             "reaches denise as the blank input. [claims: opus:AA-diwstop.c4/c5 "
             "opus:AA-diwstrt.c4/c5 sol:AA-diwstop.c4/c5 sol:AA-diwstrt.c4/c5 "
             "opus:AA-diw.use-b08 opus:AA-diw.use-b09 opus:AA-diw.use-b10 "
             "opus:AA-diw.use-b11 opus:AA-diw.use-b12 opus:AA-diw.use-b13 "
             "opus:AA-diw.use-b14 opus:AA-diw.use-b15 opus:AA-diwhigh.bit-b00 "
             "opus:AA-diwhigh.bit-b01 opus:AA-diwhigh.bit-b08 "
             "opus:AA-diwhigh.bit-b09]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
