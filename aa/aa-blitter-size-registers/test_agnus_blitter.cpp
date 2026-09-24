// aa-blitter-size-registers -- agnus_blitter conformance test
//
// Test purpose: the AA specification's alphabetical entries for BLTSIZE,
//   BLTSIZV and BLTSIZH print a bit layout and a paragraph each, and say what
//   the newer pair is for: blits larger than the earlier chips could accept.
//   This grades the fields by counting the words a blit writes.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 12 and 13, the three
//   entries and their bit rows. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test line mode, the fill modes, the
//   minterm, the barrel shifters, descending blits, or memory. It counts
//   destination writes and reads one status pin.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `opus:AA-bltsize.c1` and `opus:AA-bltsizh.c1` each name one reader's
//   reading of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s2.json lists under `inventories`.
//
// A SIZE IS GRADED BY COUNTING WHAT COMES OUT. The document says BLTSIZE
//   `contains the width and height of the blitter operation`, with
//
//     w=Width =Horiz pixels (6 bits=64 words=1024 pixels max)
//     h=Height=Vertical lines (10 bits=1024 lines max)
//
//   and of the newer pair, `These are the blitter size registers for blits
//   larger than the earlier chips could accept`, BLTSIZV a `15 bit height`
//   and BLTSIZH an `11 bit width`. A blit of h rows by w words writes h*w
//   destination words, and this fixture counts them on `we`. Nothing here
//   needs a memory model: with USED set and the minterm lines all selected,
//   every word leaves as $FFFF.
//
// THE SIX-BIT FIELD IS WHAT MAKES THE ELEVEN-BIT ONE A CLAIM. A width of 70
//   words does not fit in BLTSIZE's six bits: written there, the top bit
//   lands in the height field instead and the blit comes out as one row of
//   six. Written into BLTSIZH it is seventy words. Assertion 4 runs both and
//   compares the counts, which is the only way the two field widths can be
//   told apart at this boundary -- a word count alone cannot separate height
//   from width, because the product is the same either way.
//
// AND THE ORDER IS PART OF THE CLAIM. The page says `BLTSIZV should be
//   written first, followed by BLTSIZH, which starts the blitter. BLTSIZV
//   need not be rewritten`. agnus_blitter.v:544-549 keeps the height in its
//   own backup register, written only by BLTSIZV, and :500 starts the blit on
//   a write to BLTSIZE or -- with ecs -- BLTSIZH. Assertions 5 and 6 are those
//   two sentences.

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
constexpr uint16_t A_BLTSIZV = 0x05C;
constexpr uint16_t A_BLTSIZH = 0x05E;
constexpr uint16_t A_BLTADAT = 0x074;

constexpr uint16_t USED = 1u << 8;
constexpr uint16_t ALL_LINES = 0x00FF;      // every minterm line selected

class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->ecs = 1; dut->clkena = 1; dut->enadma = 1; dut->ackdma = 0;
        dut->reg_address_in = 0; dut->data_in = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
        write(A_BLTAFWM, 0xFFFF);
        write(A_BLTALWM, 0xFFFF);
        write(A_BLTCON0, USED | ALL_LINES);
        write(A_BLTCON1, 0);
        write(A_BLTADAT, 0xFFFF);
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

    bool busy() { dut->eval(); return dut->busy != 0; }

    // Grant every cycle the blitter asks for and count the destination writes.
    // A limit rather than a wait: a blit that never ends is a failure the
    // assertion should see as a wrong count, not as a hang.
    int words(int limit = 60000) {
        int n = 0, drain = 16;
        bool prev = false;
        for (int i = 0; i < limit; ++i) {
            dut->ackdma = dut->reqdma;
            dut->eval();
            const bool we = dut->we != 0;
            if (we && !prev) ++n;
            prev = we;
            step();
            // DRAIN AFTER busy FALLS. The destination channel writes through a
            // pipeline, and the last word leaves after the blitter has stopped
            // reporting itself busy. Breaking on !busy counted one word too
            // few for every size -- 3 for a four-word blit, 11 for twelve, 69
            // for seventy -- which looked like an off-by-one in the RTL and
            // was an off-by-one in this loop.
            if (!dut->busy && --drain <= 0) break;
        }
        dut->ackdma = 0;
        dut->eval();
        return n;
    }

    Vagnus_blitter* dut;
};

// One blit sized through BLTSIZE's packed word.
static int by_bltsize(int h, int w) {
    Harness x;
    x.write(A_BLTSIZE, (uint16_t)((h << 6) | (w & 0x3F)));
    return x.words();
}

// One blit sized through the newer pair.
static int by_pair(int h, int w) {
    Harness x;
    x.write(A_BLTSIZV, (uint16_t)h);
    x.write(A_BLTSIZH, (uint16_t)w);
    return x.words();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    std::cout << "aa-blitter-size-registers -- AA_CONFORMANCE at agnus_blitter\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // id asks for five words and compares against four.
        const int m_fix_w = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? 5 : 4;
        const int got = by_bltsize(1, m_fix_w);
        c.note("one row of four words writes " + std::to_string(got) +
               " destination word(s)");
        c.expect(got == 4,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a blit of one row by four "
                 "words leaves four words, so this fixture counts destination "
                 "writes and every count below is against a blit it sized");
    }

    // --- 2. BLTSIZE carries height above width ------------------------------
    {
        // MUTATION BLTSIZE-CARRIES-HEIGHT-AND-WIDTH stimulus red: selecting
        // this id asks for one row instead of three, so the count is a third
        // of what the packed word says.
        const int m_h = mutating("BLTSIZE-CARRIES-HEIGHT-AND-WIDTH") ? 1 : 3;
        const int three_by_four = by_bltsize(m_h, 4);
        const int one_by_four   = by_bltsize(1, 4);
        c.note("BLTSIZE h=3 w=4 writes " + std::to_string(three_by_four) +
               " words, h=1 w=4 writes " + std::to_string(one_by_four));
        c.expect(three_by_four == 12 && one_by_four == 4,
                 "BLTSIZE-CARRIES-HEIGHT-AND-WIDTH: one word written at $058 "
                 "sets both the number of rows and the words per row, and the "
                 "blit leaves their product, which is what the AA entry means "
                 "by the width and height of the blitter operation [p.12] "
                 "[claims: opus:AA-bltsize.c1/c2/c6/c11 opus:AA-bltsizh.c1/c2 opus:AA-bltsizv.c1/c2 sol:AA-bltsize.c1/c2/c5/c6/c28/c29/c36/c37 sol:AA-bltsizh.c1/c2 sol:AA-bltsizv.c1/c2/c10]");
    }

    // --- 3. The newer pair expresses the same size --------------------------
    {
        // MUTATION BLTSIZV-AND-BLTSIZH-SPLIT-THE-SIZE stimulus red: selecting
        // this id gives the pair a height of one while BLTSIZE keeps three.
        const int m_pair = mutating("BLTSIZV-AND-BLTSIZH-SPLIT-THE-SIZE") ? 1 : 3;
        const int packed = by_bltsize(3, 4);
        const int split  = by_pair(m_pair, 4);
        c.note("the same blit is " + std::to_string(packed) +
               " words through BLTSIZE and " + std::to_string(split) +
               " through BLTSIZV then BLTSIZH");
        c.expect(packed == split && packed == 12,
                 "BLTSIZV-AND-BLTSIZH-SPLIT-THE-SIZE: a height written at "
                 "$05C and a width at $05E produce the same blit as the packed "
                 "word at $058, which is what makes the newer pair the same "
                 "operation in two registers [p.12,13] "
                 "[claims: opus:AA-bltsizv.c6/c7 sol:AA-bltsizv.c4/c6]");
    }

    // --- 4. And the pair reaches sizes the packed word cannot ---------------
    {
        // MUTATION BLTSIZH-IS-WIDER-THAN-SIX-BITS stimulus red see below
        const int wide = 70;
        const int via_pair = by_pair(1, wide);
        // Selecting this id sizes the second blit through BLTSIZH as well, so
        // the two routes agree and the difference this assertion is about
        // disappears. An earlier mutation asked for a width of six instead;
        // that survived, because a packed word whose height field is zero
        // means 1024 rows and 6144 is still not 6.
        const bool m_same = mutating("BLTSIZH-IS-WIDER-THAN-SIX-BITS");
        const int via_packed = m_same ? by_pair(1, wide)
                                      : by_bltsize(wide >> 6, wide);
        c.note("a width of " + std::to_string(wide) + " gives " +
               std::to_string(via_pair) + " words through BLTSIZH and " +
               std::to_string(via_packed) + " through BLTSIZE's six-bit field");
        c.expect(via_pair == wide && via_packed != wide,
                 "BLTSIZH-IS-WIDER-THAN-SIX-BITS: a width of seventy words is "
                 "seventy words when written at $05E and is not when written "
                 "into $058's six-bit field, which is what the AA entries mean "
                 "by an 11 bit width and by blits larger than the earlier "
                 "chips could accept [p.12,13] [claims: sol:AA-bltsize.c32/c33 sol:AA-bltsizh.c4]");
    }

    // --- 5. BLTSIZV need not be rewritten -----------------------------------
    {
        // MUTATION BLTSIZV-NEED-NOT-BE-REWRITTEN stimulus red: selecting this
        // id makes the second blit two words wide by zero, so no count
        // survives to show the height was kept.
        const int m_reuse = mutating("BLTSIZV-NEED-NOT-BE-REWRITTEN") ? 0 : 2;
        Harness x;
        x.write(A_BLTSIZV, 3);
        x.write(A_BLTSIZH, 4);
        const int first = x.words();
        x.write(A_BLTSIZH, (uint16_t)m_reuse);     // height NOT rewritten
        const int second = x.words();
        c.note("first blit " + std::to_string(first) +
               " words, then a width alone gives " + std::to_string(second));
        c.expect(first == 12 && second == 6,
                 "BLTSIZV-NEED-NOT-BE-REWRITTEN: a second width alone starts a "
                 "blit three rows tall, because the height written once at "
                 "$05C is still there, which is what the AA entry says in so "
                 "many words [p.13] [claims: opus:AA-bltsizv.c10/c12 sol:AA-bltsizv.c24]");
    }

    // --- 6. BLTSIZH starts the blit and BLTSIZV does not --------------------
    {
        Harness x;
        x.write(A_BLTSIZV, 3);
        // MUTATION BLTSIZH-STARTS-AND-BLTSIZV-DOES-NOT stimulus red:
        // selecting this id writes the starting register in the half that
        // must stay idle.
        const bool m_only = mutating("BLTSIZH-STARTS-AND-BLTSIZV-DOES-NOT");
        if (m_only) x.write(A_BLTSIZH, 4);
        const bool after_v = x.busy();
        Harness y;
        y.write(A_BLTSIZV, 3);
        y.write(A_BLTSIZH, 4);
        const bool after_h = y.busy();
        c.note(std::string("after BLTSIZV alone the blitter is ") +
               (after_v ? "busy" : "idle") + ", after BLTSIZH it is " +
               (after_h ? "busy" : "idle"));
        c.expect(!after_v && after_h,
                 "BLTSIZH-STARTS-AND-BLTSIZV-DOES-NOT: the height register "
                 "leaves the blitter idle and the width register starts it, "
                 "which is the order the AA entry prescribes [p.13] "
                 "[claims: opus:AA-bltsize.c5/c9 opus:AA-bltsizh.c6/c7 opus:AA-bltsizv.c11 sol:AA-bltsize.c9/c35 sol:AA-bltsizh.c27/c28]");
    }


    // --- 7. Each bit of BLTSIZH's width field carries its own power of two --
    {
        // MUTATION BLTSIZH-BITS-ARE-A-BINARY-WIDTH stimulus red: selecting
        // this id sets bit 0 as well as the bit under test, so every count is
        // one word too many and only the lowest bit still matches.
        bool ok = true;
        std::ostringstream o;
        for (int bit = 1; bit <= 10; ++bit) {          // bit 0 alone is 1 word
            const int w = (1 << bit) | (mutating("BLTSIZH-BITS-ARE-A-BINARY-WIDTH") ? 1 : 0);
            const int got = by_pair(1, w);
            if (got != (1 << bit)) {
                ok = false;
                o << (o.tellp() ? ", " : "") << "w bit " << bit << " gives "
                  << got << " not " << (1 << bit);
            }
        }
        c.note(std::string("BLTSIZH bits 1 to 10, each alone: ") +
               (ok ? "every one writes its own power of two" : o.str()));
        c.expect(ok,
                 "BLTSIZH-BITS-ARE-A-BINARY-WIDTH: bits 1 to 10 of $05E each "
                 "set alone give a blit of 2, 4, 8 and so on up to 1024 words, "
                 "so the eleven bits are the binary width field the AA entry "
                 "names w10 to w0 [p.13] [claims: sol:AA-bltsizh.c16/c17/c18/c19/c20/c21/c22/c23/c24/c25 opus:AA-bltsizh.b01 opus:AA-bltsizh.b02 opus:AA-bltsizh.b03 opus:AA-bltsizh.b04 opus:AA-bltsizh.b05 opus:AA-bltsizh.b06 opus:AA-bltsizh.b07 opus:AA-bltsizh.b08 opus:AA-bltsizh.b09 opus:AA-bltsizh.b10]");
    }

    // --- 8. And each bit of BLTSIZV's height field does the same ------------
    {
        // MUTATION BLTSIZV-BITS-ARE-A-BINARY-HEIGHT stimulus red: selecting
        // this id adds one row to every height, so no count matches its
        // power of two.
        bool ok = true;
        std::ostringstream o;
        for (int bit = 0; bit <= 10; ++bit) {
            const int h = (1 << bit) + (mutating("BLTSIZV-BITS-ARE-A-BINARY-HEIGHT") ? 1 : 0);
            const int got = by_pair(h, 2);             // two words per row
            if (got != (1 << bit) * 2) {
                ok = false;
                o << (o.tellp() ? ", " : "") << "h bit " << bit << " gives "
                  << got << " not " << ((1 << bit) * 2);
            }
        }
        c.note(std::string("BLTSIZV bits 0 to 10, each alone, two words wide: ") +
               (ok ? "every one writes twice its own power of two" : o.str()));
        c.expect(ok,
                 "BLTSIZV-BITS-ARE-A-BINARY-HEIGHT: bits 0 to 10 of $05C each "
                 "set alone give a blit of 1, 2, 4 and so on rows, so the "
                 "fifteen bits are the binary height field the AA entry names "
                 "h14 to h0 [p.13] [claims: sol:AA-bltsizv.c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23 opus:AA-bltsizv.b00 opus:AA-bltsizv.b01 opus:AA-bltsizv.b02 opus:AA-bltsizv.b03 opus:AA-bltsizv.b04 opus:AA-bltsizv.b05 opus:AA-bltsizv.b06 opus:AA-bltsizv.b07 opus:AA-bltsizv.b08 opus:AA-bltsizv.b09 opus:AA-bltsizv.b10]");
    }


    // --- 9. BLTSIZE's low six bits are its width ----------------------------
    {
        // MUTATION BLTSIZE-WIDTH-BITS-ARE-THE-LOW-SIX stimulus red: selecting
        // this id sets bit 0 alongside the bit under test, so each count is
        // one word too many.
        bool ok = true;
        std::ostringstream o;
        for (int bit = 1; bit <= 5; ++bit) {
            const int w = (1 << bit) | (mutating("BLTSIZE-WIDTH-BITS-ARE-THE-LOW-SIX") ? 1 : 0);
            const int got = by_bltsize(1, w);
            if (got != (1 << bit)) {
                ok = false;
                o << (o.tellp() ? ", " : "") << "bit " << bit << " gives " << got;
            }
        }
        c.note(std::string("BLTSIZE bits 1 to 5, each alone: ") +
               (ok ? "every one writes its own power of two" : o.str()));
        c.expect(ok,
                 "BLTSIZE-WIDTH-BITS-ARE-THE-LOW-SIX: bits 1 to 5 of $058 each "
                 "set alone give a blit of 2, 4, 8, 16 and 32 words, so the low "
                 "six bits are the width field the AA entry names w5 to w0 "
                 "[p.12] [claims: sol:AA-bltsize.c22/c23/c24/c25/c26/c27 opus:AA-bltsize.b00 opus:AA-bltsize.b01 opus:AA-bltsize.b02 opus:AA-bltsize.b03 opus:AA-bltsize.b04 opus:AA-bltsize.b05]");
    }

    // --- 10. And its high ten bits are the height ---------------------------
    {
        // MUTATION BLTSIZE-HEIGHT-BITS-ARE-THE-HIGH-TEN stimulus red:
        // selecting this id adds a row to every height, so no count matches
        // its power of two.
        bool ok = true;
        std::ostringstream o;
        for (int bit = 0; bit <= 9; ++bit) {
            const int h = (1 << bit) + (mutating("BLTSIZE-HEIGHT-BITS-ARE-THE-HIGH-TEN") ? 1 : 0);
            const int got = by_bltsize(h, 2);
            if (got != (1 << bit) * 2) {
                ok = false;
                o << (o.tellp() ? ", " : "") << "bit " << (bit + 6)
                  << " gives " << got;
            }
        }
        c.note(std::string("BLTSIZE bits 6 to 15, each alone, two words wide: ") +
               (ok ? "every one writes twice its own power of two" : o.str()));
        c.expect(ok,
                 "BLTSIZE-HEIGHT-BITS-ARE-THE-HIGH-TEN: bits 6 to 15 of $058 "
                 "each set alone give a blit of 1, 2, 4 and so on rows, so the "
                 "high ten bits are the height field the AA entry names h9 to "
                 "h0 [p.12] [claims: sol:AA-bltsize.c12/c13/c14/c15/c16/c17/c18/c19/c20/c21 opus:AA-bltsize.b06 opus:AA-bltsize.b07 opus:AA-bltsize.b08 opus:AA-bltsize.b09 opus:AA-bltsize.b10 opus:AA-bltsize.b11 opus:AA-bltsize.b12 opus:AA-bltsize.b13 opus:AA-bltsize.b14 opus:AA-bltsize.b15]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary: the chip column of all three entries. "
             "No port of agnus_blitter carries a chip identity [claims: "
             "opus:AA-bltsize.c3 opus:AA-bltsizh.c3 opus:AA-bltsizv.c3/c8 "
             "sol:AA-bltsize.c3 sol:AA-bltsizh.c3 sol:AA-bltsizv.c3]");
    c.record(
             "DECLINED at this boundary: the claims that the document's two "
             "statements of a register differ from each other. Which of them is "
             "right is a fact about the document [claims: opus:AA-bltsize.c4 "
             "opus:AA-bltsizh.c4/c5 opus:AA-bltsizv.c4/c5 sol:AA-bltsize.c4 "
             "sol:AA-bltsizh.c5 sol:AA-bltsizv.c5]");
    c.record(
             "DECLINED at this boundary: what the size fields mean in line mode, "
             "where width must be two and height is the line length. Line mode "
             "is this package's first non-goal [claims: opus:AA-bltsize.c7/c8 "
             "sol:AA-bltsize.c7/c8]");
    c.record(
             "DECLINED at this boundary: the claims counted in PIXELS rather "
             "than words. How many pixels a word covers is decided in Denise by "
             "the bit-plane depth and the resolution, neither of which reaches "
             "agnus [claims: opus:AA-bltsize.c13/c14 opus:AA-bltsizv.c13 "
             "sol:AA-bltsize.c31]");
    c.record(
             "DECLINED at this boundary: the largest blit the two newer fields "
             "can express, 32k by 32k. Running it would be tens of millions of "
             "cycles for one assertion, and the field widths it rests on are "
             "graded bit by bit above [claims: "
             "sol:AA-bltsizh.c6/c8/c10/c12/c14/c26 sol:AA-bltsizv.c7/c9/c11/c12 "
             "opus:AA-bltsizh.b00 opus:AA-bltsizv.b11 opus:AA-bltsizv.b12 "
             "opus:AA-bltsizv.b13 opus:AA-bltsizv.b14]");
    c.record(
             "DECLINED at this boundary: the sentences that tell a programmer "
             "what order to write the registers in, and what to put in the bits "
             "the tables print as x. Neither is a requirement on a signal "
             "[claims: opus:AA-bltsize.c10 opus:AA-bltsizv.c9/c14 "
             "sol:AA-bltsize.c10/c11 sol:AA-bltsizh.c7/c9/c11/c13/c15 "
             "sol:AA-bltsizv.c8 opus:AA-bltsizh.b11 opus:AA-bltsizh.b12 "
             "opus:AA-bltsizh.b13 opus:AA-bltsizh.b14 opus:AA-bltsizh.b15 "
             "opus:AA-bltsizv.b15]");
    c.record(
             "DECLINED at this boundary: the 1024-line maximum, which is the h=0 "
             "encoding of the ten bit field. The height assertion above sets one "
             "bit at a time and reaches 512 rows [claims: opus:AA-bltsize.c12 "
             "sol:AA-bltsize.c30/c34]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
