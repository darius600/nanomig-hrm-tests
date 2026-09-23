// aa-blitter-masks-and-data -- agnus_blitter conformance test
//
// Test purpose: the AA specification's alphabetical entries for BLTAFWM,
//   BLTALWM, BLTxDAT and BLTDDAT describe two masks and two kinds of data
//   register. This grades what the masks do, which word each reaches, and the
//   two sentences about who loads the data registers.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 12 and 13. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test line mode, the fill modes, the
//   minterm truth table, the barrel shifters, descending blits, or memory.
//
// THE MASKS ARE GRADED AS EQUIVALENCES, FOR THE SAME REASON THE SHIFTERS WERE.
//   The document says of BLTAFWM and BLTALWM:
//
//     The patterns in these two registers are "anded" with the first and last
//     words of each horizontal line of Source A.
//     A zero in any bit overides data from Source A.
//
//   To show that, the output must depend on source A -- and which minterm
//   lines make it depend on A is not something this document prints. So each
//   assertion compares a masked run against a run with the A DATA REGISTER
//   pre-ANDed by the same pattern, sweeps all eight lines, and skips the
//   line/word pairs where moving A does not move the output. What is asserted
//   is an equality that holds whatever the minterm means.
//
// AND WHICH WORD EACH MASK REACHES IS THE OTHER HALF. A four-word row has a
//   first word, a last word and two in between. Assertion 2 shows the first
//   mask changes word 0 and leaves words 1 to 3; assertion 3 shows the last
//   mask changes word 3 and leaves words 0 to 2. A fixture that captured only
//   one word could not tell the two registers apart at all -- which is why
//   this one keeps the whole row.
//
// THE DUMMY-ADDRESS HALF OF BLTDDAT IS DECLINED, AND THE REASON IS MEASURED.
//   The page says `This is a dummy address and cannot be read by the micro.`
//   A first version asserted that by writing $000 and reading it back -- and
//   the assertion survived its own mutation, because agnus_blitter answers NO
//   read at any address: `data_out` carries the blit result, not a register
//   file. $000 is not distinguishable from $044 here, so the claim is recorded
//   rather than graded. What assertion 5 grades instead is the other half of
//   the same entry: the result leaves automatically, with nothing written at
//   $000 at all.

#include "Vagnus_blitter.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

constexpr uint16_t A_BLTDDAT = 0x000;
constexpr uint16_t A_BLTCON0 = 0x040;
constexpr uint16_t A_BLTCON1 = 0x042;
constexpr uint16_t A_BLTAFWM = 0x044;
constexpr uint16_t A_BLTALWM = 0x046;
constexpr uint16_t A_BLTSIZE = 0x058;
constexpr uint16_t A_BLTCDAT = 0x070;
constexpr uint16_t A_BLTBDAT = 0x072;
constexpr uint16_t A_BLTADAT = 0x074;

constexpr uint16_t USED = 1u << 8;
constexpr uint16_t SIZE_ONE_ROW_FOUR = (1u << 6) | 4u;

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

    // Every destination word of the row, in order. The drain after busy falls
    // is not optional: the last word leaves the pipeline afterwards.
    std::vector<uint16_t> row(int limit = 4000) {
        std::vector<uint16_t> out;
        bool prev = false;
        int drain = 16;
        for (int i = 0; i < limit; ++i) {
            dut->ackdma = dut->reqdma;
            dut->eval();
            const bool we = dut->we != 0;
            if (we && !prev) out.push_back((uint16_t)dut->data_out);
            prev = we;
            step();
            if (!dut->busy && --drain <= 0) break;
        }
        dut->ackdma = 0;
        dut->eval();
        return out;
    }

    Vagnus_blitter* dut;
};

// One four-word row with the three sources held and the two masks given.
static std::vector<uint16_t> blit(uint16_t lf, uint16_t afwm, uint16_t alwm,
                                  uint16_t a = SRC_A) {
    Harness h;
    h.write(A_BLTCON0, USED | lf);
    h.write(A_BLTCON1, 0);
    h.write(A_BLTAFWM, afwm);
    h.write(A_BLTALWM, alwm);
    h.write(A_BLTADAT, a);
    h.write(A_BLTBDAT, SRC_B);
    h.write(A_BLTCDAT, SRC_C);
    h.write(A_BLTSIZE, SIZE_ONE_ROW_FOUR);
    return h.row();
}

static std::string show(const std::vector<uint16_t>& r) {
    std::ostringstream o;
    for (size_t i = 0; i < r.size(); ++i) o << (i ? " " : "") << hex(r[i]);
    return o.str();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    const uint16_t MASK = 0x0FF0;

    std::cout << "aa-blitter-masks-and-data -- AA_CONFORMANCE at agnus_blitter\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // id closes the first-word mask, so word 0 differs from the rest.
        const uint16_t m_fix = mutating("FIXTURE-DELIVERS-KNOWN-VALUE")
                                    ? 0x0000u : 0xFFFFu;
        const std::vector<uint16_t> r = blit(0x0F, m_fix, 0xFFFF);
        const bool flat = r.size() == 4 && r[0] == r[1] && r[1] == r[2] &&
                          r[2] == r[3];
        c.note("four words with both masks open: " + show(r));
        c.expect(flat,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: with both masks all ones a "
                 "four-word row leaves four identical words, so any word that "
                 "differs below was changed by a mask this fixture wrote");
    }

    // --- 2. BLTAFWM reaches the first word and no other ---------------------
    {
        // MUTATION BLTAFWM-ANDS-THE-FIRST-WORD-ONLY stimulus red: selecting
        // this id puts the pattern in the LAST word mask instead, so word 0
        // keeps the whole source and word 3 is the one that moves.
        const bool m_first = mutating("BLTAFWM-ANDS-THE-FIRST-WORD-ONLY");
        int tried = 0, vacuous = 0;
        bool ok = true;
        std::ostringstream o;
        for (int bit = 0; bit < 8; ++bit) {
            const uint16_t LF = (uint16_t)(1u << bit);
            const std::vector<uint16_t> open_  = blit(LF, 0xFFFF, 0xFFFF);
            const std::vector<uint16_t> masked = blit(LF, m_first ? 0xFFFF : MASK,
                                                      m_first ? MASK : 0xFFFF);
            const std::vector<uint16_t> pre    = blit(LF, 0xFFFF, 0xFFFF,
                                                      (uint16_t)(SRC_A & MASK));
            if (open_.size() != 4 || masked.size() != 4 || pre.size() != 4) {
                ok = false; continue;
            }
            if (pre[0] == open_[0]) { ++vacuous; continue; }
            ++tried;
            const bool first_right = masked[0] == pre[0];
            const bool rest_untouched = masked[1] == open_[1] &&
                                        masked[2] == open_[2] &&
                                        masked[3] == open_[3];
            if (!(first_right && rest_untouched)) {
                ok = false;
                o << (o.tellp() ? ", " : "") << "LF" << bit << " " << show(masked);
            }
        }
        c.note("BLTAFWM: " + std::to_string(tried) + " lines where the mask can "
               "show, " + std::to_string(vacuous) + " where it cannot" +
               (ok ? "; the first word matched a pre-ANDed source and words 1 "
                     "to 3 were untouched every time"
                   : "; mismatches: " + o.str()));
        c.expect(ok && tried > 0,
                 "BLTAFWM-ANDS-THE-FIRST-WORD-ONLY: the pattern at $044 gives "
                 "the first word of the row what pre-ANDing source A would give "
                 "it, and leaves the other three alone, which is what the AA "
                 "entry means by a first word mask for source A [p.12] "
                 "[claims: opus:AA-bltafwm.c1/c2/c5 sol:AA-bltafwm.c1/c2/c5]");
    }

    // --- 3. And BLTALWM reaches the last -------------------------------------
    {
        // MUTATION BLTALWM-ANDS-THE-LAST-WORD-ONLY stimulus red: selecting
        // this id puts the pattern in the FIRST word mask instead, so the
        // word this assertion checks is untouched and another one moves.
        const bool m_last = mutating("BLTALWM-ANDS-THE-LAST-WORD-ONLY");
        int tried = 0, vacuous = 0;
        bool ok = true;
        std::ostringstream o;
        for (int bit = 0; bit < 8; ++bit) {
            const uint16_t LF = (uint16_t)(1u << bit);
            const std::vector<uint16_t> open_  = blit(LF, 0xFFFF, 0xFFFF);
            const std::vector<uint16_t> masked = blit(LF, m_last ? MASK : 0xFFFF,
                                                      m_last ? 0xFFFF : MASK);
            const std::vector<uint16_t> pre    = blit(LF, 0xFFFF, 0xFFFF,
                                                      (uint16_t)(SRC_A & MASK));
            if (open_.size() != 4 || masked.size() != 4 || pre.size() != 4) {
                ok = false; continue;
            }
            if (pre[0] == open_[0]) { ++vacuous; continue; }
            ++tried;
            const bool last_right = masked[3] == pre[0];
            const bool rest_untouched = masked[0] == open_[0] &&
                                        masked[1] == open_[1] &&
                                        masked[2] == open_[2];
            if (!(last_right && rest_untouched)) {
                ok = false;
                o << (o.tellp() ? ", " : "") << "LF" << bit << " " << show(masked);
            }
        }
        c.note("BLTALWM: " + std::to_string(tried) + " lines where the mask can "
               "show, " + std::to_string(vacuous) + " where it cannot" +
               (ok ? "; the last word matched a pre-ANDed source and words 0 to "
                     "2 were untouched every time"
                   : "; mismatches: " + o.str()));
        c.expect(ok && tried > 0,
                 "BLTALWM-ANDS-THE-LAST-WORD-ONLY: the pattern at $046 gives "
                 "the last word of the row what pre-ANDing source A would give "
                 "it, and leaves the first three alone, which is what the AA "
                 "entry means by a last word mask for source A [p.12] "
                 "[claims: opus:AA-bltalwm.c1/c2 sol:AA-bltalwm.c1/c2/c5]");
    }

    // --- 4. A zero in any bit overrides source A -----------------------------
    {
        // MUTATION A-ZERO-OVERRIDES-SOURCE-A stimulus red: selecting this id
        // opens the mask instead of closing it, so the first word keeps
        // source A.
        const uint16_t m_zero = mutating("A-ZERO-OVERRIDES-SOURCE-A")
                                     ? 0xFFFFu : 0x0000u;
        int tried = 0;
        bool ok = true;
        for (int bit = 0; bit < 8; ++bit) {
            const uint16_t LF = (uint16_t)(1u << bit);
            const std::vector<uint16_t> zeroed = blit(LF, m_zero, 0xFFFF);
            const std::vector<uint16_t> a_zero = blit(LF, 0xFFFF, 0xFFFF, 0x0000);
            const std::vector<uint16_t> open_  = blit(LF, 0xFFFF, 0xFFFF);
            if (zeroed.size() != 4 || a_zero.size() != 4) { ok = false; continue; }
            if (a_zero[0] == open_[0]) continue;      // this line ignores A
            ++tried;
            if (zeroed[0] != a_zero[0]) ok = false;
        }
        c.note("with the first-word mask all zeros, word 0 matched a source A "
               "of zero on " + std::to_string(tried) + " of the lines that "
               "depend on A");
        c.expect(ok && tried > 0,
                 "A-ZERO-OVERRIDES-SOURCE-A: a mask of all zeros gives the "
                 "first word exactly what a source A of zero would give it, "
                 "which is what the AA entry means by a zero in any bit "
                 "overriding data from source A [p.12] "
                 "[claims: opus:AA-bltafwm.c6]");
    }

    // --- 5. BLTDDAT carries the result out, automatically -------------------
    {
        // MUTATION BLTDDAT-CARRIES-THE-RESULT stimulus red: selecting this id
        // clears USED, so the blit runs and nothing leaves at all.
        const bool m_no_used = mutating("BLTDDAT-CARRIES-THE-RESULT");
        Harness h;
        h.write(A_BLTCON0, (m_no_used ? 0u : USED) | 0x0Fu);
        h.write(A_BLTCON1, 0);
        h.write(A_BLTAFWM, 0xFFFF);
        h.write(A_BLTALWM, 0xFFFF);
        h.write(A_BLTADAT, SRC_A);
        h.write(A_BLTBDAT, SRC_B);
        h.write(A_BLTCDAT, SRC_C);
        h.write(A_BLTSIZE, SIZE_ONE_ROW_FOUR);
        const std::vector<uint16_t> r = h.row();
        c.note("a four-word row with nothing written at $000 leaves " +
               std::to_string(r.size()) + " word(s): " + show(r));
        c.expect(r.size() == 4,
                 "BLTDDAT-CARRIES-THE-RESULT: four words leave the blitter for "
                 "a four-word row without the processor writing the "
                 "destination register at all, which is what the AA entry "
                 "means by a register that holds the result of each word and a "
                 "transfer that is automatic during Blitter operation [p.12] "
                 "[claims: opus:AA-bltddat.c1/c6/c8 sol:AA-bltddat.c1/c5/c7/c8]");
    }

    // --- 6. The source data registers can be preloaded by the processor ------
    {
        // MUTATION BLTXDAT-CAN-BE-PRELOADED stimulus red: selecting this id
        // preloads the same word twice, so no difference can show.
        const uint16_t m_pre = mutating("BLTXDAT-CAN-BE-PRELOADED")
                                    ? SRC_A : (uint16_t)0x0F0Fu;
        int moved = 0;
        for (int bit = 0; bit < 8; ++bit) {
            const uint16_t LF = (uint16_t)(1u << bit);
            const std::vector<uint16_t> one = blit(LF, 0xFFFF, 0xFFFF, SRC_A);
            const std::vector<uint16_t> two = blit(LF, 0xFFFF, 0xFFFF, m_pre);
            if (one.size() == 4 && two.size() == 4 && one[0] != two[0]) ++moved;
        }
        c.note("a word written into the A data register by the processor "
               "changed the output on " + std::to_string(moved) +
               " of the eight minterm lines");
        c.expect(moved > 0,
                 "BLTXDAT-CAN-BE-PRELOADED: a word the processor writes at "
                 "$074 reaches the blit with no DMA fetch behind it, which is "
                 "what the AA entry means by a source register that may also "
                 "be preloaded by the microprocessor [p.12,13] "
                 "[claims: opus:AA-bltxdat.c1/c2/c6/c8 sol:AA-bltxdat.c1/c2/c4/c6]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary: the blitter's pointer and modulo "
             "registers. They are decoded in agnus_blitter_adrgen, a module of "
             "its own, and graded there by aa-blitter-pointer-map [claims: "
             "opus:AA-bltxmod.c1/c2/c3/c4/c5/c6/c7 "
             "opus:AA-bltxpth.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11 "
             "opus:AA-bltxptl.c1/c2/c3/c4/c5/c6/c7 sol:AA-bltamod.c1 "
             "sol:AA-bltapth.c1 sol:AA-bltaptl.c1 sol:AA-bltbmod.c1 "
             "sol:AA-bltbpth.c1 sol:AA-bltbptl.c1 sol:AA-bltcmod.c1 "
             "sol:AA-bltcpth.c1 sol:AA-bltcptl.c1 sol:AA-bltdmod.c1 "
             "sol:AA-bltdpth.c1 sol:AA-bltdptl.c1 "
             "sol:AA-bltxmod.c1/c2/c3/c4/c5/c6/c7/c8 sol:AA-bltxpt.c1/c2/c3/c4 "
             "sol:AA-bltxpth.c1/c2/c3/c4 sol:AA-bltxptl.c1/c2/c3/c4]");
    c.record(
             "DECLINED at this boundary: the rows in which the document explains "
             "its own notation. No signal carries a legend [claims: "
             "opus:AA-legend.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19]");
    c.record(
             "DECLINED at this boundary: the chip column. No port of "
             "agnus_blitter carries a chip identity [claims: opus:AA-bltafwm.c3 "
             "opus:AA-bltalwm.c3 opus:AA-bltcon0.c3 opus:AA-bltcon1.c3 "
             "opus:AA-bltddat.c3 opus:AA-bltfield.c23 opus:AA-bltxdat.c3 "
             "sol:AA-bltafwm.c3 sol:AA-bltalwm.c3 sol:AA-bltcon0.c3 "
             "sol:AA-bltcon1.c3/c54 sol:AA-bltddat.c2 sol:AA-bltsizext.c1 "
             "sol:AA-bltxdat.c3]");
    c.record(
             "DECLINED at this boundary: the claims that the document's two "
             "statements of a register differ from each other [claims: "
             "opus:AA-bltafwm.c4 opus:AA-bltalwm.c4 opus:AA-bltcon0.c4 "
             "opus:AA-bltcon1.c4/c5 opus:AA-bltddat.c2/c4/c5 opus:AA-bltmode.c1 "
             "opus:AA-bltxdat.c4/c5 sol:AA-bltadat.c1 sol:AA-bltafwm.c4 "
             "sol:AA-bltalwm.c4 sol:AA-bltbdat.c1 sol:AA-bltcdat.c1 "
             "sol:AA-bltcon0.c4 sol:AA-bltcon1.c4 sol:AA-bltddat.c3/c4]");
    c.record(
             "DECLINED at this boundary: line mode and the fill modes, which are "
             "this package's non-goals [claims: opus:AA-bltafwm.c7 "
             "opus:AA-bltfield.c13/c14/c15/c16/c18/c19/c20 sol:AA-bltcon1.c5 "
             "sol:AA-bltmask.c2 opus:AA-bltcon1.line00val]");
    c.record(
             "DECLINED at this boundary: the claim that $000 cannot be read by "
             "the processor. MEASURED: agnus_blitter answers no read at ANY "
             "address -- data_out carries the blit result, not a register file "
             "-- so $000 not being readable is not distinguishable from $044 not "
             "being readable. A first version of this package asserted it and "
             "the assertion survived its own mutation [claims: "
             "opus:AA-bltddat.c7 sol:AA-bltddat.c6]");
    c.record(
             "DECLINED at this boundary: the sentences about a source register "
             "being loaded by the blitter's own DMA channel. This fixture grants "
             "the handshake and models no memory behind it, so a fetched word "
             "and a preloaded one cannot be told apart [claims: "
             "opus:AA-bltxdat.c7 sol:AA-bltxdat.c5]");
    c.record(
             "DECLINED at this boundary: registers of the blitter this package "
             "does not exercise, BLTCON0L among them -- the minterm lines it "
             "carries are graded through BLTCON0 by "
             "aa-blitter-function-and-shift [claims: opus:AA-bltcon0.c1/c2 "
             "opus:AA-bltcon0l.c1/c2/c3/c4/c5/c6/c7 "
             "opus:AA-bltcon1.c1/c2/c6/c7/c8 "
             "opus:AA-bltfield.c9/c10/c11/c12/c17/c21/c22 opus:AA-bltmode.c2 "
             "sol:AA-bltcon.c1/c2/c3/c4 sol:AA-bltcon0.c1/c2 "
             "sol:AA-bltcon0l.c1/c2/c3/c4/c5 "
             "sol:AA-bltcon1.c1/c2/c40/c41/c42/c43/c44/c45/c46/c47/c48/c49/c50/c51/c52/c53 "
             "sol:AA-bltmask.c1/c3 sol:AA-bltsizext.c2/c3/c4 "
             "opus:AA-bltcon1.area00val]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
