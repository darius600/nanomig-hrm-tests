// aa-blitter-function-and-shift -- agnus_blitter conformance test
//
// Test purpose: the AA specification's alphabetical entry for BLTCON0 and
//   BLTCON1 prints a bit table with an area-mode and a line-mode column, and a
//   legend naming four fields: ASH3-0, BSH3-0, USEA/USEB/USEC/USED and LF7-0.
//   This grades what those fields do. It is the complement of
//   aa-blitter-control-registers, which grades the addresses and names the
//   minterm, the shifters and the fill modes as non-goals.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 14, the BLTCON0/BLTCON1 bit
//   table and the field legend below it. The document prints no page numbers
//   of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test line mode, the fill modes (EFE, IFE,
//   FCI), descending blits, BLTCON0L, the DMA slot handshake beyond granting
//   it, or memory. Nothing here reads or writes chip RAM.
//
// THE DOCUMENT NAMES THE MINTERM LINES AND NEVER PRINTS THE TRUTH TABLE.
//   The legend reads `LF7-0   Logic function minterm select lines`, and the bit
//   rows name LF7 down to LF0 -- but nowhere in these 31 pages does the
//   specification say which combination of A, B and C the line LFn selects.
//   MEASURED: a search for a minterm truth table over all seven slice unions
//   returns the register rows and the legend, and no mapping.
//
//   So this package does NOT assert "minterm $F0 means D = A". That mapping
//   would have to come from the RTL, and the RTL is not an oracle. What the
//   legend does support, without any mapping at all, is arithmetic on the SET
//   of lines:
//
//     - select none of the eight and no combination can produce a 1, so D is
//       zero whatever A, B and C are;
//     - select all eight and every combination produces a 1, so D is all ones;
//     - select exactly one and the result is whichever source bits land in
//       that one combination -- so the eight single-line results must be
//       pairwise DISJOINT and must together cover every bit.
//
//   Assertions 1 and 2 are those three sentences. They are strong -- a
//   generator that ORed the lines together, or ignored one of them, fails
//   every part -- and they rest on nothing the document does not print.
//
// THE SOURCES ARE CHOSEN SO EVERY COMBINATION APPEARS. A = $FF00, B = $F0F0,
//   C = $CCCC puts each of the eight (A,B,C) patterns into exactly two of the
//   sixteen bit positions. That is what makes "pairwise disjoint, union is all
//   ones" a measurement rather than a coincidence.
//
// THE SHIFT IS GRADED AS AN EQUIVALENCE, BECAUSE THE PAGE DOES NOT PRINT A
//   DIRECTION. The legend says `ASH3-0  Shift value of A source` and stops
//   there. So assertions 3 and 4 assert what that sentence does support:
//   setting the field to k produces the same word as pre-shifting the source
//   itself by k places, in whichever direction the shifter runs.
//
//   AND THEY SWEEP ALL EIGHT LINES RATHER THAN PICKING ONE. The first version
//   picked LF = $5A because it looked arbitrary. It is not: $5A selects a
//   function of A and C alone, so the B half of the same test compared two
//   identical words four times and called it a pass. Without the truth table
//   -- which this document does not print -- no constant can be KNOWN to
//   depend on the source under test. Both assertions now run every single
//   line, count the line/shift pairs where moving the source does not move
//   the output, skip those as proving nothing, and require the equivalence of
//   all the rest. Measured: 24 of 32 pairs are live for A, 32 of 32 for B.
//
// AND THAT IS WHERE BIT 12 MATTERS. The bit table's row for bit 12 reads
//
//     12   ASA0        BSH0        12   ASH0        BSH0
//
//   -- `ASA0` in the area column where every neighbour reads ASH3, ASH2, ASH1,
//   and where the same row's line column reads ASH0. The corpus tags that line
//   `[3/3] ocr+guide+model`: all three independent readings produced it, so it
//   is what the page prints, and both blind readers flagged it independently
//   -- sol as `MISPRINT`, opus by naming the line column against it.
//   Assertion 3 includes k = 1, which is bit 12 alone, and shows the bit is
//   the low bit of the shift value the legend names ASH3-0. Filed as
//   NM-AA-003.

#include "Vagnus_blitter.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

namespace {
// Per-assertion mutation selector: MUTATE names ONE assertion's own label,
// and only that assertion's stimulus site changes.
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

// Word addresses as the AA register list prints them, read from the corpus.
constexpr uint16_t A_BLTCON0 = 0x040;
constexpr uint16_t A_BLTCON1 = 0x042;
constexpr uint16_t A_BLTAFWM = 0x044;
constexpr uint16_t A_BLTALWM = 0x046;
constexpr uint16_t A_BLTSIZE = 0x058;
constexpr uint16_t A_BLTCDAT = 0x070;
constexpr uint16_t A_BLTBDAT = 0x072;
constexpr uint16_t A_BLTADAT = 0x074;

constexpr uint16_t USED = 1u << 8;          // BLTCON0 bit 8, the legend's USED
constexpr int      ASH_SHIFT = 12;          // BLTCON0 bits 15-12
constexpr int      BSH_SHIFT = 12;          // BLTCON1 bits 15-12

// Four words in one row: a one-word blit never writes, because the D channel
// only requests once the pipeline is full.
constexpr uint16_t SIZE_FOUR_WORDS = (1u << 6) | 4u;

// Each of the eight (A,B,C) patterns lands in exactly two bit positions.
constexpr uint16_t SRC_A = 0xFF00;
constexpr uint16_t SRC_B = 0xF0F0;
constexpr uint16_t SRC_C = 0xCCCC;

class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->ecs = 1; dut->clkena = 1; dut->enadma = 1; dut->ackdma = 0;
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

    // Grant every cycle the blitter asks for and capture the FIRST D word as
    // it leaves. First, not last: the A channel's first word is the one the
    // first-word mask acts on, and this fixture opens both masks so the word
    // it captures is the function generator's own output.
    // Returns -1 if the blit never wrote, which an assertion should see rather
    // than a hang.
    int run(int limit = 400) {
        int got = -1;
        for (int i = 0; i < limit; ++i) {
            dut->ackdma = dut->reqdma;
            dut->eval();
            if (dut->we && got < 0) got = dut->data_out;
            step();
            if (!dut->busy) break;
        }
        dut->ackdma = 0;
        dut->eval();
        return got;
    }

    Vagnus_blitter* dut;
};

// One four-word blit with the three sources held and nothing fetched.
// `con0_high` carries ASH and USED; `lf` is the eight minterm select lines.
static int blit(uint16_t lf, uint16_t con0_high, uint16_t con1 = 0,
                uint16_t a = SRC_A, uint16_t b = SRC_B, uint16_t c = SRC_C) {
    Harness h;
    h.write(A_BLTAFWM, 0xFFFF);        // both masks open: the captured word is
    h.write(A_BLTALWM, 0xFFFF);        // the generator's output, not a mask's
    // THE CONTROL REGISTERS GO FIRST AND THE ORDER IS NOT COSMETIC. The B
    // channel is shifted into its holding register only when BLTBDAT is
    // written (agnus_blitter.v:411-412), so a BSH written afterwards never
    // reaches the word. The first version of this fixture wrote the data
    // first and measured a shift field that did nothing -- which is what
    // programming the blitter control-first avoids on real hardware too.
    h.write(A_BLTCON0, con0_high | lf);
    h.write(A_BLTCON1, con1);
    h.write(A_BLTADAT, a);
    h.write(A_BLTBDAT, b);
    h.write(A_BLTCDAT, c);
    h.write(A_BLTSIZE, SIZE_FOUR_WORDS);
    return h.run();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    std::cout << "aa-blitter-function-and-shift -- AA_CONFORMANCE at agnus_blitter\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red\n";

    // --- 1. No line selected is zero, every line selected is all ones -------
    {
        // MUTATION LF-NONE-IS-ZERO-ALL-IS-ONES stimulus red: selecting this
        // id selects one line in the run that must select none, so the word
        // is no longer zero.
        const uint16_t m_none = mutating("LF-NONE-IS-ZERO-ALL-IS-ONES")
                                     ? 0x0001u : 0x0000u;
        const int none = blit(m_none, USED);
        const int all  = blit(0x00FF, USED);
        c.note("with no minterm line selected D reads " + hex(none) +
               ", with all eight " + hex(all));
        c.expect(none == 0x0000 && all == 0xFFFF,
                 "LF-NONE-IS-ZERO-ALL-IS-ONES: selecting none of the eight "
                 "lines makes every bit zero and selecting all eight makes "
                 "every bit one, whatever the three sources hold, which is "
                 "what the AA legend's `Logic function minterm select lines` "
                 "requires of the set without naming any one of them [p.14] "
                 "[claims: sol:AA-bltcon0.c42]");
    }

    // --- 2. The eight lines are disjoint and cover every bit ----------------
    {
        // MUTATION LF-LINES-ARE-DISJOINT-AND-COMPLETE stimulus red: selecting
        // this id gives the first run two lines instead of one, so two
        // results overlap and the union is no longer a partition.
        const uint16_t m_bit = mutating("LF-LINES-ARE-DISJOINT-AND-COMPLETE")
                                    ? 0x0003u : 0x0001u;
        int part[8];
        uint16_t seen = 0, overlap = 0;
        bool read_all = true;
        for (int i = 0; i < 8; ++i) {
            part[i] = blit(i == 0 ? m_bit : (uint16_t)(1u << i), USED);
            if (part[i] < 0) { read_all = false; break; }
            overlap |= (uint16_t)(seen & (uint16_t)part[i]);
            seen |= (uint16_t)part[i];
        }
        std::ostringstream o;
        for (int i = 0; i < 8; ++i) o << (i ? " " : "") << hex(part[i]);
        c.note("one line at a time: " + o.str() + "; union " + hex(seen) +
               ", overlap " + hex(overlap));
        c.expect(read_all && overlap == 0x0000 && seen == 0xFFFF,
                 "LF-LINES-ARE-DISJOINT-AND-COMPLETE: the eight lines taken "
                 "one at a time produce eight results that share no bit and "
                 "together cover all sixteen, which is what makes them minterm "
                 "SELECT lines rather than eight ways to say the same thing "
                 "[p.14] [claims: opus:AA-bltfield.c8]");
    }

    // --- 3. ASH3-0 is the shift value of the A source ----------------------
    {
        // MUTATION ASH-BITS-ARE-THE-SHIFT-VALUE-OF-A stimulus red: selecting
        // this id leaves the field at zero for every k, so the shifted run
        // equals the unshifted one while the pre-shift it is compared
        // against does not.
        //
        // EVERY LINE IS TRIED RATHER THAN A CHOSEN ONE. The first version of
        // this assertion picked LF = $5A "arbitrarily" and it graded B as
        // vacuous, because $5A happens to select a function of A and C alone.
        // A constant picked without the truth table -- which this document
        // does not print -- cannot be known to depend on the source under
        // test. So all eight single lines are run, the ones whose output does
        // not move when the source moves are counted and skipped, and the
        // rest must all show the equivalence.
        int tried = 0, vacuous = 0;
        bool ok = true;
        std::ostringstream o;
        for (int bit = 0; bit < 8; ++bit) {
            const uint16_t LF = (uint16_t)(1u << bit);
            const int plain = blit(LF, USED);
            for (int k : {1, 2, 4, 8}) {
                const int kk = mutating("ASH-BITS-ARE-THE-SHIFT-VALUE-OF-A") ? 0 : k;
                const int by_fld = blit(LF, (uint16_t)(kk << ASH_SHIFT) | USED);
                const int pre_r = blit(LF, USED, 0, (uint16_t)(SRC_A >> k));
                const int pre_l = blit(LF, USED, 0, (uint16_t)(SRC_A << k));
                if (pre_r == plain && pre_l == plain) { ++vacuous; continue; }
                ++tried;
                if (by_fld != pre_r && by_fld != pre_l) {
                    ok = false;
                    o << (o.tellp() ? ", " : "") << "LF" << bit << " k=" << k
                      << " field " << hex(by_fld) << " preR " << hex(pre_r);
                }
            }
        }
        c.note("A shift: " + std::to_string(tried) + " line/shift pairs where "
               "the source moves the output, " + std::to_string(vacuous) +
               " where it does not and which prove nothing" +
               (ok ? "; every one of the first group matched a pre-shift"
                   : "; mismatches: " + o.str()));
        c.expect(ok && tried > 0,
                 "ASH-BITS-ARE-THE-SHIFT-VALUE-OF-A: for every minterm line "
                 "whose output depends on the A source, writing 1, 2, 4 or 8 "
                 "into bits 15-12 produces the same word as shifting the A "
                 "source itself by that many places, so the four bits are the "
                 "field the AA legend calls ASH3-0 -- bit 12 included, which "
                 "the area column of the same table spells ASA0 [p.14] "
                 "[claims: sol:AA-bltcon0.c5/c7/c9/c11/c37 "
                 "opus:AA-bltcon0.area15 opus:AA-bltcon0.area14 "
                 "opus:AA-bltcon0.area13 opus:AA-bltcon0.area12 "
                 "opus:AA-bltfield.c1]");
    }

    // --- 4. BSH3-0 is the shift value of the B source ------------------------
    {
        // MUTATION BSH-BITS-ARE-THE-SHIFT-VALUE-OF-B stimulus red: selecting
        // this id leaves BLTCON1's field at zero, so nothing moves.
        int tried = 0, vacuous = 0;
        bool ok = true;
        std::ostringstream o;
        for (int bit = 0; bit < 8; ++bit) {
            const uint16_t LF = (uint16_t)(1u << bit);
            const int plain = blit(LF, USED, 0);
            for (int k : {1, 2, 4, 8}) {
                const int kk = mutating("BSH-BITS-ARE-THE-SHIFT-VALUE-OF-B") ? 0 : k;
                const int by_fld = blit(LF, USED, (uint16_t)(kk << BSH_SHIFT));
                const int pre_r = blit(LF, USED, 0, SRC_A, (uint16_t)(SRC_B >> k));
                const int pre_l = blit(LF, USED, 0, SRC_A, (uint16_t)(SRC_B << k));
                if (pre_r == plain && pre_l == plain) { ++vacuous; continue; }
                ++tried;
                if (by_fld != pre_r && by_fld != pre_l) {
                    ok = false;
                    o << (o.tellp() ? ", " : "") << "LF" << bit << " k=" << k
                      << " field " << hex(by_fld) << " preR " << hex(pre_r);
                }
            }
        }
        c.note("B shift: " + std::to_string(tried) + " line/shift pairs where "
               "the source moves the output, " + std::to_string(vacuous) +
               " where it does not" +
               (ok ? "; every one of the first group matched a pre-shift"
                   : "; mismatches: " + o.str()));
        c.expect(ok && tried > 0,
                 "BSH-BITS-ARE-THE-SHIFT-VALUE-OF-B: for every minterm line "
                 "whose output depends on the B source, writing 1, 2, 4 or 8 "
                 "into BLTCON1 bits 15-12 produces the same word as shifting "
                 "the B source itself, which is the field the AA legend calls "
                 "BSH3-0 [p.14] [claims: sol:AA-bltcon1.c6/c8/c10/c12/c38 "
                 "opus:AA-bltcon1.area15 opus:AA-bltcon1.area14 "
                 "opus:AA-bltcon1.area13 opus:AA-bltcon1.area12 "
                 "opus:AA-bltfield.c2]");
    }

    // --- 5. USED decides whether a destination word is written at all -------
    {
        // MUTATION USED-DECIDES-WHETHER-D-IS-WRITTEN stimulus red: selecting
        // this id leaves USED set in the run that must not write, so a word
        // comes out.
        const uint16_t m_used = mutating("USED-DECIDES-WHETHER-D-IS-WRITTEN")
                                     ? USED : 0u;
        const int with_d = blit(0x00FF, USED);
        const int no_d   = blit(0x00FF, m_used);
        c.note(std::string("with USED set the blit writes ") + hex(with_d) +
               ", with it clear it writes " +
               (no_d < 0 ? std::string("nothing at all") : hex(no_d)));
        c.expect(with_d == 0xFFFF && no_d < 0,
                 "USED-DECIDES-WHETHER-D-IS-WRITTEN: with bit 8 clear the same "
                 "blit runs and produces no destination write, which is what "
                 "the AA legend means by a mode control bit to use Destination "
                 "D [p.14] [claims: sol:AA-bltcon0.c41/c19 opus:AA-bltfield.c7 opus:AA-bltcon0.area08]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary: the area-mode column names BLTCON0 bits "
             "7 to 0 LF7 down to LF0, and the legend calls the eight of them "
             "logic function minterm select lines. The document never prints "
             "which combination of A, B and C any one line selects -- MEASURED "
             "across all seven slice unions, which return the register rows and "
             "the legend and no truth table. One line's name is not separable "
             "from the other seven here; the set is graded above [claims: "
             "sol:AA-bltcon0.c21/c23/c25/c27/c29/c31/c33/c35 "
             "opus:AA-bltcon0.area00 opus:AA-bltcon0.area01 "
             "opus:AA-bltcon0.area02 opus:AA-bltcon0.area03 "
             "opus:AA-bltcon0.area04 opus:AA-bltcon0.area05 "
             "opus:AA-bltcon0.area06 opus:AA-bltcon0.area07]");
    c.record(
             "DECLINED at this boundary: every cell of the line-mode column, and "
             "the area-mode cells that only mean something in line mode. Line "
             "mode is this package's first non-goal and nothing here writes "
             "BLTCON1 bit 0 [claims: opus:AA-bltfield.c3 "
             "sol:AA-bltcon0.c6/c8/c10/c12/c14/c16/c18/c20/c22/c24/c26/c28/c30/c32/c34/c36 "
             "sol:AA-bltcon1.c7/c9/c11/c13/c15/c17/c19/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c39 "
             "opus:AA-bltcon0.line00 opus:AA-bltcon0.line01 "
             "opus:AA-bltcon0.line02 opus:AA-bltcon0.line03 "
             "opus:AA-bltcon0.line04 opus:AA-bltcon0.line05 "
             "opus:AA-bltcon0.line06 opus:AA-bltcon0.line07 "
             "opus:AA-bltcon0.line08 opus:AA-bltcon0.line09 "
             "opus:AA-bltcon0.line10 opus:AA-bltcon0.line11 "
             "opus:AA-bltcon0.line12 opus:AA-bltcon0.line13 "
             "opus:AA-bltcon0.line14 opus:AA-bltcon0.line15 "
             "opus:AA-bltcon1.area00 opus:AA-bltcon1.area01 "
             "opus:AA-bltcon1.area02 opus:AA-bltcon1.area03 "
             "opus:AA-bltcon1.area04 opus:AA-bltcon1.area05 "
             "opus:AA-bltcon1.area06 opus:AA-bltcon1.area07 "
             "opus:AA-bltcon1.line00 opus:AA-bltcon1.line01 "
             "opus:AA-bltcon1.line02 opus:AA-bltcon1.line03 "
             "opus:AA-bltcon1.line04 opus:AA-bltcon1.line05 "
             "opus:AA-bltcon1.line06 opus:AA-bltcon1.line07 "
             "opus:AA-bltcon1.line08 opus:AA-bltcon1.line09 "
             "opus:AA-bltcon1.line10 opus:AA-bltcon1.line11 "
             "opus:AA-bltcon1.line12 opus:AA-bltcon1.line13 "
             "opus:AA-bltcon1.line14 opus:AA-bltcon1.line15]");
    c.record(
             "DECLINED at this boundary: USEA, USEB and USEC decide whether a "
             "channel is fetched from memory. This fixture holds the three "
             "sources in their data registers and models no memory, so it cannot "
             "tell a fetched source from a held one [claims: "
             "opus:AA-bltfield.c4/c5/c6 sol:AA-bltcon0.c13/c15/c17/c38/c39/c40 "
             "opus:AA-bltcon0.area09 opus:AA-bltcon0.area10 "
             "opus:AA-bltcon0.area11]");
    c.record(
             "DECLINED at this boundary: the area-mode column prints a fixed "
             "value for BLTCON1 bits 11 to 8 rather than naming a field. A "
             "constant printed in a table is not a requirement on a signal "
             "[claims: sol:AA-bltcon1.c14/c16/c18/c20 opus:AA-bltcon1.area08 "
             "opus:AA-bltcon1.area09 opus:AA-bltcon1.area10 "
             "opus:AA-bltcon1.area11]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
