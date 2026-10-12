// test_ch4_shres_sprite_a.cpp -- ch4-shres-sprite-a. See SPECIFICATION.md
// for every assertion's proposition, program, observation and expected
// value. This file only implements what SPECIFICATION.md already
// committed to. The sprite/bitplane/Copper-list *data* is poked directly
// into chip RAM by this file; prog.s and its mutation wrappers only perform
// the CPU register writes of section 3.3.
//
// One evaluation serves the normal run and every NMH_MUTATE selector: each
// run prints the same lines in the same order (prog.bin, cpu_nrst_out, DONE,
// fixture, R1, PC6, PC1-PC4, PC5-BOUNDS, PC5, R5, P1-P4). A line whose
// precondition did not print PASS prints `NOT GRADED: <id> [<precondition>]`
// instead of PASS or FAIL. R1 takes its own HIRES capture (prog_c1.bin) in
// every run. The records below name every line that changes, per core.
//
// MUTATION PC1 stimulus red NMH_MUTATE=C1: BPLCON0=$9000 (HIRES, SHRES=0) instead of $1040 -- the pattern spacing becomes 16 visible pixels instead of 8 -> FAIL PC1; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION PC2 stimulus red NMH_MUTATE=C2: sprite 1's own terminator word (p.108: both terminator words should be $0000) is poked to $BAD1 instead -- the terminator lies beyond CTL's VSTOP, so it is never displayed, and sprite 1's population (294 logical reads per graded frame) stays fully fetched -> FAIL PC2; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION PC3 stimulus red NMH_MUTATE=C3: SPR2PTH (step 5 and the Copper list) points at a separately addressed, non-null structure for sprite 2 ($2C00, sprite 0's own POS, CTL and pairs) instead of sprite 2's null structure -- sprite 2 is no longer idle and the structure is hidden under sprite 0: null2 is read 0 times of 2, the new structure 294 times, sprite 0's and sprite 1's structures stay at 294 reads (PC2 stays green), R5 and PC4 stay green -> FAIL PC3; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION PC4 stimulus red NMH_MUTATE=C4: band 2's WAIT changed from $8B41 to $9141 (the fifth colour-17 line of band 2) -> FAIL PC4; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION PC5 stimulus red NMH_MUTATE=C5: the COLOR00-COLOR15 <- B4 MOVEs after WAIT $C541 are left out -> FAIL PC5; NOT GRADED P1; fx68k 15 passed, 1 failed, 1 lines not graded; TG68K 15 passed, 1 failed, 1 lines not graded
// MUTATION PC6 stimulus red NMH_MUTATE=C6: three frame waits after step 7 instead of four -- 4 Copper restarts, 1 graded frame, and the lines that grade each frame still read PASS on it -> FAIL PC6; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION PC5-BOUNDS stimulus red NMH_MUTATE=C13: COLOR00 in REF is $0000 instead of $0AAA -- the pattern's off samples and the border are zero, so the nonzero extent shrinks to the lit pattern and zero samples lie inside it -> FAIL PC5-BOUNDS; NOT GRADED PC5 P1; fx68k 14 passed, 1 failed, 2 lines not graded; TG68K 14 passed, 1 failed, 2 lines not graded
// MUTATION R1 stimulus red NMH_MUTATE=R1: band 1's COLOR16-COLOR31 swap G and H in the anchor capture's Copper list only (C9's data) -- group 3 of the anchor capture stops matching the section 3.5 U values; the run's own capture is untouched -> FAIL R1; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION R1 stimulus red NMH_MUTATE=R1S: the anchor capture runs prog.bin (SuperHires) instead of prog_c1.bin -- its pattern-line spacing measures 8 instead of 16; the run's own capture is untouched -> FAIL R1; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION R5 stimulus red NMH_MUTATE=C7: the colour-17 lines of band 0 ($53-$5A), both sprites, get low=$FF00/high=$0000 instead of $FFFF/$0000 -- group 0's runs narrow to 8 visible pixels while every other group's stay equal, so the equal-length clause goes red; p.101 permits the shape change, so this is not a P2 contrast -> FAIL R5; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION R5 stimulus red NMH_MUTATE=C14: the colour-18 lines of band 0 ($5C-$63) of both sprites get both data words $0000 -- one group disappears (14 groups), both 294-word structures stay fully fetched and every Copper marker line is untouched -> FAIL R5; NOT GRADED PC5 P1 P2 P3 P4; fx68k 11 passed, 1 failed, 5 lines not graded; TG68K 11 passed, 1 failed, 5 lines not graded
// MUTATION R5 stimulus red NMH_MUTATE=C15: every data word of sprite 1 is $0000 (POS and CTL stay) -- sprite 1's 294-word structure stays fully fetched and shows nothing, so every group line has one run (the run-count clause) -> FAIL R5; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION R5 stimulus red NMH_MUTATE=C16: every sprite data word keeps only its bit 0 -- every run is one sample wide, all runs stay equal and every line has two, so only the interior-pixel clause goes red -> FAIL R5; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION R5 stimulus red NMH_MUTATE=C17: after the second frame wait the program stores a mark word and the test changes every data word of both sprites to its upper byte -- the first graded frame has 16-sample runs, the second 8-sample runs, each frame uniform, so only the cross-frame equal-length clause goes red; PC2, PC3 and PC4 take the data prepared for the frame they grade -> FAIL R5; NOT GRADED P1 P2 P3 P4; fx68k 12 passed, 1 failed, 4 lines not graded; TG68K 12 passed, 1 failed, 4 lines not graded
// MUTATION P1 stimulus red NMH_MUTATE=C8: band 4's COLOR16-COLOR31 reload (after WAIT $C541) uses band 0's values instead of band 3's -- groups 12-14 stop matching band 3 (PC5 and R5 stay green: neither depends on COLOR16-COLOR31 or on colour content) -> FAIL P1 P2 P3 P4; NOT GRADED none; fx68k 13 passed, 4 failed, 0 lines not graded; TG68K 13 passed, 4 failed, 0 lines not graded
// MUTATION P2 stimulus red NMH_MUTATE=C8: group 12 (band 4, colour 17) belongs to P2's own group set (0,3,6,9,12), so C8 prints red here too -> FAIL P1 P2 P3 P4; NOT GRADED none; fx68k 13 passed, 4 failed, 0 lines not graded; TG68K 13 passed, 4 failed, 0 lines not graded
// MUTATION P3 stimulus red NMH_MUTATE=C8: group 13 (band 4, colour 18) belongs to P3's own group set (1,4,7,10,13), so C8 prints red here too -> FAIL P1 P2 P3 P4; NOT GRADED none; fx68k 13 passed, 4 failed, 0 lines not graded; TG68K 13 passed, 4 failed, 0 lines not graded
// MUTATION P4 stimulus red NMH_MUTATE=C8: group 14 (band 4, colour 19) belongs to P4's own group set (2,5,8,11,14), so C8 prints red here too -> FAIL P1 P2 P3 P4; NOT GRADED none; fx68k 13 passed, 4 failed, 0 lines not graded; TG68K 13 passed, 4 failed, 0 lines not graded
// MUTATION P1 stimulus red NMH_MUTATE=C9: band 1's COLOR16-COLOR31 reload swaps G and H -- group 3 alone stops matching, and P1 grades all fifteen groups -> FAIL P1 P2; NOT GRADED none; fx68k 15 passed, 2 failed, 0 lines not graded; TG68K 15 passed, 2 failed, 0 lines not graded
// MUTATION P2 stimulus red NMH_MUTATE=C9: group 3 also belongs to P2's own group set (0,3,6,9,12) -> FAIL P1 P2; NOT GRADED none; fx68k 15 passed, 2 failed, 0 lines not graded; TG68K 15 passed, 2 failed, 0 lines not graded
// MUTATION P1 stimulus red NMH_MUTATE=C10: band 2's COLOR16-COLOR31 reload swaps O and P -- group 7 alone stops matching, and P1 grades all fifteen groups -> FAIL P1 P3; NOT GRADED none; fx68k 15 passed, 2 failed, 0 lines not graded; TG68K 15 passed, 2 failed, 0 lines not graded
// MUTATION P3 stimulus red NMH_MUTATE=C10: group 7 also belongs to P3's own group set (1,4,7,10,13) -> FAIL P1 P3; NOT GRADED none; fx68k 15 passed, 2 failed, 0 lines not graded; TG68K 15 passed, 2 failed, 0 lines not graded
// MUTATION P1 stimulus red NMH_MUTATE=C11: band 2's COLOR16-COLOR31 reload swaps W and X -- group 8 alone stops matching, and P1 grades all fifteen groups -> FAIL P1 P4; NOT GRADED none; fx68k 15 passed, 2 failed, 0 lines not graded; TG68K 15 passed, 2 failed, 0 lines not graded
// MUTATION P4 stimulus red NMH_MUTATE=C11: group 8 also belongs to P4's own group set (2,5,8,11,14) -> FAIL P1 P4; NOT GRADED none; fx68k 15 passed, 2 failed, 0 lines not graded; TG68K 15 passed, 2 failed, 0 lines not graded
// MUTATION fixture stimulus red NMH_MUTATE=C12: the program stores $DEAD instead of $BEEF at result+2 -> FAIL fixture; NOT GRADED none; fx68k 16 passed, 1 failed, 0 lines not graded; TG68K 16 passed, 1 failed, 0 lines not graded
// MUTATION DONE stimulus red NMH_MUTATE=NODONE: the program stops in an endless loop right before its DONE store, so every line that grades the run prints NOT GRADED while R1, which has its own capture, still reads PASS -> FAIL DONE; NOT GRADED PC6 PC1 PC2 PC3 PC4 PC5-BOUNDS PC5 R5 P1 P2 P3 P4; fx68k 4 passed, 1 failed, 12 lines not graded; TG68K 4 passed, 1 failed, 12 lines not graded
// MUTATION prog.bin stimulus red NMH_MUTATE=NOPROG: no program image of that name exists, so nothing runs and every line that depends on the run prints NOT GRADED while R1, which has its own capture, still reads PASS -> FAIL prog.bin; NOT GRADED cpu_nrst_out DONE fixture PC6 PC1 PC2 PC3 PC4 PC5-BOUNDS PC5 R5 P1 P2 P3 P4; fx68k 1 passed, 1 failed, 15 lines not graded; TG68K 1 passed, 1 failed, 15 lines not graded
// MUTATION cpu_nrst_out none impossible: unfalsifiable at this package's boundary -- the release happens in boot(), before the program's first instruction runs, and no stimulus of this package (the program image, the chip-RAM data, the Copper list) is an input of it; every selector above prints it PASS
//
//

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- the assertion call form: expect grades, note measures, record declines ----
struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    // A line whose precondition does not hold is neither PASS nor FAIL: it
    // prints its own leading id (never the claim ids of its label) and the
    // precondition that did not hold.
    void not_graded(const std::string& label, const std::string& why) {
        std::string id = label.substr(0, label.find(' '));
        if (!id.empty() && id.back() == ':') id.pop_back();
        std::cout << "RECORDED: NOT GRADED: " << id << " [" << why << "]\n";
        ++ungraded;
    }
    int passed = 0, failed = 0, ungraded = 0;
};

enum class St { PASS, FAIL, NG };

// Prints `label` as PASS or FAIL, or as NOT GRADED with `ng_reason` when the
// reason is nonempty (then `ok` is never read).
St grade(Checks& ck, const std::string& label, const std::string& ng_reason, bool ok) {
    if (!ng_reason.empty()) { ck.not_graded(label, ng_reason); return St::NG; }
    ck.expect(ok, label);
    return ok ? St::PASS : St::FAIL;
}

// The ids of the lines in `deps` that did not print PASS, space separated.
std::string not_holding(std::initializer_list<std::pair<const char*, St>> deps) {
    std::string s;
    for (auto& d : deps)
        if (d.second != St::PASS) { if (!s.empty()) s += " "; s += d.first; }
    return s;
}

// ---- addresses (ch4.i, restated) ------------------------------------------
constexpr uint32_t kResults  = 0x0C0000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint32_t kSpr0Data = 0x002000;
constexpr uint32_t kSpr1Data = 0x002400;
// C3 only: a non-null structure for sprite 2 (SPECIFICATION.md 3.2)
constexpr uint32_t kSpr2Data = 0x002C00;
// C17 only: the program's mark word before the test changes the sprite data
constexpr uint32_t kMarkAddr  = 0x0C0FFC;
constexpr uint16_t kMarkValue = 0x17C1;
// sprites 2-7's own safe null structures (ch4.i, restated): each
// independently identifiable, so PC3 can require every one of the six to
// be read on its own address (p.114).
constexpr uint32_t kNull2 = 0x002800;
constexpr uint32_t kNull3 = 0x002804;
constexpr uint32_t kNull4 = 0x002808;
constexpr uint32_t kNull5 = 0x00280C;
constexpr uint32_t kNull6 = 0x002810;
constexpr uint32_t kNull7 = 0x002814;
const uint32_t kNullAddrs[6] = { kNull2, kNull3, kNull4, kNull5, kNull6, kNull7 };
constexpr uint32_t kBplBuf   = 0x010000;
constexpr uint32_t kCopList  = 0x020000;
constexpr size_t   kBplWords = 32768;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 10'000'000;

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

// ---- section 3.5 tables, transcribed literally from SPECIFICATION.md -----

// REF's entry 0 (bitplane colour 0, and the border colour whenever no
// bitplane is fetched) is $0AAA: letters a=1 b=0 c=1 d=0 e=1 f=0 in row 00 of
// p.301 ("a b a b c d c d e f e f", bits 11..0), so bits 11 and 9 are both a,
// 10 and 8 both b, and so on -- U=(2,2,2), one of the two backgrounds
// "background-like" already covers (section 3.6 R5). This is PC5-BOUNDS's own
// all-nonblack anchor (SPECIFICATION.md 3.8, section 6): with every sample of
// the pattern line and of its surrounding border genuinely nonzero, the first
// and last nonzero sample (measure_window()) can only be the end of the
// video port's blanked output, never a REF pixel that happens to carry
// U=(0,0,0).
const uint16_t kRef[16] = {
    0x0AAA,0x0CCC,0x0000,0x0000,0x0333,0x0FFF,0x0333,0x0333,
    0x0000,0x0CCC,0x0000,0x0000,0x0000,0x0CCC,0x0000,0x0000
};
const uint16_t kB0[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
const uint16_t kB4[16] = {
    0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA,
    0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA,0x0AAA
};

// COLOR16..COLOR31, bands 0-3 (band 4 reuses band 3, SPECIFICATION.md 3.5)
const uint16_t kBandUpper[4][16] = {
    { 0x05AF,0x016B,0x0D27,0x09E3,0x049E,0x005A,0x0C16,0x08D2,
      0x078D,0x0349,0x0F05,0x0BC1,0x06BC,0x0278,0x0E34,0x0AF0 },
    { 0x0AF0,0x06BC,0x0278,0x0E34,0x09E3,0x05AF,0x016B,0x0D27,
      0x08D2,0x049E,0x005A,0x0C16,0x0BC1,0x078D,0x0349,0x0F05 },
    { 0x0F05,0x0BC1,0x078D,0x0349,0x0E34,0x0AF0,0x06BC,0x0278,
      0x0D27,0x09E3,0x05AF,0x016B,0x0C16,0x08D2,0x049E,0x005A },
    { 0x005A,0x0C16,0x08D2,0x049E,0x0349,0x0F05,0x0BC1,0x078D,
      0x0278,0x0E34,0x0AF0,0x06BC,0x016B,0x0D27,0x09E3,0x05AF },
};

// P2's contrast C9: band 1, G/H swapped (SPECIFICATION.md 4, P2.6)
const uint16_t kC9Band1[16] = {
    0x0AF0,0x0ABC,0x0278,0x0E34,0x0AE3,0x0AAF,0x026B,0x0E27,
    0x08D2,0x089E,0x005A,0x0C16,0x0BC1,0x0B8D,0x0349,0x0F05
};
// P3's contrast C10: band 2, O/P swapped (SPECIFICATION.md 4, P3.6)
const uint16_t kC10Band2[16] = {
    0x0F05,0x0BC1,0x074D,0x0349,0x0E34,0x0AF0,0x067C,0x0278,
    0x0D17,0x09D3,0x055F,0x015B,0x0C16,0x08D2,0x045E,0x005A
};
// P4's contrast C11: band 2, W/X swapped (SPECIFICATION.md 4, P4.6)
const uint16_t kC11Band2[16] = {
    0x0F05,0x0BC1,0x078D,0x0345,0x0E34,0x0AF0,0x06BC,0x0274,
    0x0D27,0x09E3,0x05AF,0x0167,0x0C15,0x08D1,0x049D,0x0055
};

// ---- section 3.2's sprite line layout -------------------------------------

std::pair<uint16_t,uint16_t> line_pattern(int off) {
    if (off <= 2)                 return {0x0000,0x0000};
    if (off >= 3 && off <= 10)    return {0xFFFF,0x0000}; // colour 17
    if (off == 11)                return {0x0000,0x0000};
    if (off >= 12 && off <= 19)   return {0x0000,0xFFFF}; // colour 18
    if (off == 20)                return {0x0000,0x0000};
    /* 21..28 */                  return {0xFFFF,0xFFFF}; // colour 19
}

using Pairs = std::vector<std::pair<uint16_t,uint16_t>>;

// C7: band 0's colour-17 lines (global pair index 3..10) get low=$FF00 /
// high=$0000 instead of $FFFF/$0000 (SPECIFICATION.md 3.8, C7).
// C14: band 0's colour-18 lines (global pair index 12..19) are transparent.
// C16: every data word keeps only its bit 0, so every run is one sample wide.
Pairs expected_pairs(int mode) {
    Pairs pairs;
    pairs.reserve(145);
    for (int b = 0; b < 5; ++b)
        for (int off = 0; off < 29; ++off) {
            int idx = b*29+off;
            if (mode == 7 && idx >= 3 && idx <= 10) pairs.push_back({0xFF00,0x0000});
            else if (mode == 14 && idx >= 12 && idx <= 19) pairs.push_back({0x0000,0x0000});
            else {
                auto p = line_pattern(off);
                if (mode == 16) { p.first &= 0x0001; p.second &= 0x0001; }
                pairs.push_back(p);
            }
        }
    return pairs;
}

// C17's later data: every data word keeps only its upper byte.
Pairs upper_byte_pairs(const Pairs& in) {
    Pairs out;
    for (auto& p : in) out.push_back({(uint16_t)(p.first & 0xFF00), (uint16_t)(p.second & 0xFF00)});
    return out;
}

// ---- mutation ids ----------------------------------------------------------

// C1-C16 and R1/R1S are the stimulus selectors of the header records; NODONE
// and NOPROG are the program-level ones.
enum class Mut { NONE, C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12,C13,C14,C15,C16,C17, R1,R1S, NODONE, NOPROG };

std::optional<Mut> parse_mut(const std::string& s) {
    static const std::map<std::string,Mut> m = {
        {"",Mut::NONE},{"C1",Mut::C1},{"C2",Mut::C2},{"C3",Mut::C3},{"C4",Mut::C4},{"C5",Mut::C5},
        {"C6",Mut::C6},{"C7",Mut::C7},{"C8",Mut::C8},{"C9",Mut::C9},{"C10",Mut::C10},{"C11",Mut::C11},
        {"C12",Mut::C12},{"C13",Mut::C13},{"C14",Mut::C14},{"C15",Mut::C15},{"C16",Mut::C16},{"C17",Mut::C17},
        {"R1",Mut::R1},{"R1S",Mut::R1S},{"NODONE",Mut::NODONE},{"NOPROG",Mut::NOPROG},
    };
    auto it = m.find(s);
    if (it == m.end()) return std::nullopt;
    return it->second;
}

std::string prog_name_for(Mut m) {
    switch (m) {
        case Mut::C1:     return "prog_c1";
        case Mut::C3:     return "prog_c3";
        case Mut::C6:     return "prog_c6";
        case Mut::C12:    return "prog_c12";
        case Mut::C17:    return "prog_c17";
        case Mut::NODONE: return "prog_nodone";
        case Mut::NOPROG: return "prog_missing"; // no such image exists
        default:          return "prog"; // data-only contrasts and the anchor selectors
    }
}

// ---- Copper list (section 3.4), one static word sequence per mutation ----

struct CopperBuild {
    std::vector<uint16_t> words;
    std::map<std::string,size_t> mark;
};

CopperBuild build_copper(Mut mut) {
    CopperBuild cb;
    auto& w = cb.words;
    auto push  = [&](uint16_t a, uint16_t b) { w.push_back(a); w.push_back(b); };
    auto pushl = [&](uint16_t regH, uint32_t ptr) {
        push(regH, (uint16_t)(ptr>>16));
        push((uint16_t)(regH+2), (uint16_t)(ptr & 0xFFFF));
    };

    pushl(0x0E0, kBplBuf);                                   // BPL1PTH/L
    pushl(0x120, kSpr0Data);                                 // SPR0PTH/L
    pushl(0x124, kSpr1Data);                                  // SPR1PTH/L
    pushl(0x128, (mut==Mut::C3) ? kSpr2Data : kNull2);        // SPR2PTH/L (C3: a non-null structure)
    pushl(0x12C, kNull3);                                     // SPR3PTH/L
    pushl(0x130, kNull4);                                     // SPR4PTH/L
    pushl(0x134, kNull5);                                     // SPR5PTH/L
    pushl(0x138, kNull6);                                     // SPR6PTH/L
    pushl(0x13C, kNull7);                                     // SPR7PTH/L

    for (int i=0;i<16;++i) push(0x180+2*i, (i==0 && mut==Mut::C13) ? 0x0000 : kRef[i]); // COLOR00-15 <- REF (C13: COLOR00 = $0000)
    cb.mark["band0_start"] = w.size();
    for (int i=0;i<16;++i) push(0x1A0+2*i, kBandUpper[0][i]); // COLOR16-31 <- band0
    cb.mark["top_end"] = w.size();

    push(0x4041, 0xFFFE);

    cb.mark["b0_start"] = w.size();
    for (int i=0;i<16;++i) push(0x180+2*i, kB0[i]);           // COLOR00-15 <- B0
    cb.mark["b0_end"] = w.size();

    push(0x6E41, 0xFFFE);

    const uint16_t* band1 = (mut==Mut::C9) ? kC9Band1 : kBandUpper[1];
    cb.mark["band1_start"] = w.size();
    for (int i=0;i<16;++i) push(0x1A0+2*i, band1[i]);         // COLOR16-31 <- band1
    cb.mark["band1_end"] = w.size();

    push(mut==Mut::C4 ? 0x9141 : 0x8B41, 0xFFFE);

    const uint16_t* band2 = (mut==Mut::C10) ? kC10Band2 : (mut==Mut::C11) ? kC11Band2 : kBandUpper[2];
    cb.mark["band2_start"] = w.size();
    for (int i=0;i<16;++i) push(0x1A0+2*i, band2[i]);         // COLOR16-31 <- band2
    cb.mark["band2_end"] = w.size();

    push(0xA841, 0xFFFE);

    cb.mark["band3_start"] = w.size();
    for (int i=0;i<16;++i) push(0x1A0+2*i, kBandUpper[3][i]); // COLOR16-31 <- band3
    cb.mark["band3_end"] = w.size();

    push(0xC541, 0xFFFE);

    cb.mark["b4_start"] = w.size();
    if (mut != Mut::C5)
        for (int i=0;i<16;++i) push(0x180+2*i, kB4[i]);       // COLOR00-15 <- B4
    cb.mark["b4_end"] = w.size();

    const uint16_t* band4 = (mut==Mut::C8) ? kBandUpper[0] : kBandUpper[3];
    cb.mark["band4_start"] = w.size();
    for (int i=0;i<16;++i) push(0x1A0+2*i, band4[i]);         // COLOR16-31 <- band4
    cb.mark["band4_end"] = w.size();

    push(0xFFFF, 0xFFFE);                                     // end of list
    return cb;
}

// ---- memory setup ----------------------------------------------------------

void poke_sprite_structure(nmh::NanomigHarness& h, uint32_t base, uint16_t pos, uint16_t ctl,
                            const std::vector<std::pair<uint16_t,uint16_t>>& pairs) {
    uint32_t w = word_addr(base);
    h.poke_word(w++, pos);
    h.poke_word(w++, ctl);
    for (auto& p : pairs) { h.poke_word(w++, p.first); h.poke_word(w++, p.second); }
    h.poke_word(w++, 0); h.poke_word(w++, 0);
}

void poke_null_structure(nmh::NanomigHarness& h, uint32_t base) {
    uint32_t w = word_addr(base);
    h.poke_word(w++, 0); h.poke_word(w++, 0);
}

void poke_bitplane_buffer(nmh::NanomigHarness& h, uint32_t base, size_t words) {
    uint32_t w = word_addr(base);
    for (size_t i=0;i<words;++i) h.poke_word(w+(uint32_t)i, 0xFF00);
}

// ---- one captured run --------------------------------------------------

struct Capture {
    bool loaded=false, released=false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;      // one entry per RAM-port sample
    uint64_t period = 0;                  // measured clk_sys cycles between consecutive RAM-port samples
    std::vector<nmh::VideoSample> video;
    std::optional<uint16_t> deniseid, fixture;
    std::vector<uint16_t> cop_words;
    // the data words prepared in sprite 0 and sprite 1 (pairs0/pairs1), and, in a
    // run that changes them part-way (C17), the data words after the change
    // (pairs0_late/pairs1_late) with the cycle of the program's mark word
    Pairs pairs0, pairs1, pairs0_late, pairs1_late;
    std::optional<uint64_t> cutover;
    std::map<std::string,size_t> mark;
    // The data a graded frame or Copper pass [begin,end) fetches: the change is
    // made at the top of a frame, before that frame's sprite lines, so a span
    // whose midpoint lies after the mark word reads the later data.
    bool late(nmh::Span f) const { return cutover && *cutover < (f.begin + f.end) / 2; }
    const Pairs& p0(nmh::Span f) const { return late(f) ? pairs0_late : pairs0; }
    const Pairs& p1(nmh::Span f) const { return late(f) ? pairs1_late : pairs1; }
};

// `copper_mut` selects the Copper list independently of `mut` and `prog` the
// program image, so the R1 selectors can change the anchor capture alone.
Capture run_case(const std::string& dir, Mut mut, std::optional<Mut> copper_mut = std::nullopt,
                 const std::string& prog = "") {
    Capture c;
    nmh::Config cfg; // defaults: ECS, 2.0 MB chip RAM, 68000 behaviour (SPECIFICATION.md 3.1)
    nmh::NanomigHarness h(cfg);

    poke_bitplane_buffer(h, kBplBuf, kBplWords);
    int mode = mut==Mut::C7 ? 7 : mut==Mut::C14 ? 14 : mut==Mut::C16 ? 16 : 0;
    c.pairs0 = expected_pairs(mode);
    c.pairs1 = (mut==Mut::C15) ? Pairs(c.pairs0.size(), {0x0000,0x0000}) : c.pairs0; // C15: sprite 1 transparent
    c.pairs0_late = c.pairs0; c.pairs1_late = c.pairs1;
    if (mut == Mut::C17) { c.pairs0_late = upper_byte_pairs(c.pairs0); c.pairs1_late = upper_byte_pairs(c.pairs1); }
    poke_sprite_structure(h, kSpr0Data, 0x5050, 0xE100, c.pairs0);
    poke_sprite_structure(h, kSpr1Data, 0x5060, 0xE100, c.pairs1);
    for (uint32_t na : kNullAddrs) poke_null_structure(h, na);
    // C3: sprite 2 gets a real structure of its own, the same position and
    // pixels as sprite 0 (SPECIFICATION.md 3.2), so it is fetched in full and
    // hidden under sprite 0.
    if (mut == Mut::C3) poke_sprite_structure(h, kSpr2Data, 0x5050, 0xE100, c.pairs0);

    // C2 (PC2's own contrast, SPECIFICATION.md 3.8): sprite 1's structure
    // stays fully fetched -- only its own terminator word (p.108: "the
    // value 0 should be used for both words when ending the usage of a
    // sprite") is poked to a nonzero, page-incorrect value. The terminator
    // lies beyond CTL's VSTOP ($E1), so this pixel is never displayed and
    // no colour check depends on it; only PC2's own exact-value clause
    // sees the mismatch, and sprite 1's measured population (all 294
    // words) stays fully populated.
    if (mut == Mut::C2) {
        uint32_t term_word = word_addr(kSpr1Data) + 2 + (uint32_t)(2*c.pairs1.size()) + 1;
        h.poke_word(term_word, 0xBAD1);
    }

    auto cb = build_copper(copper_mut.value_or(mut));
    c.cop_words = cb.words;
    c.mark = cb.mark;
    for (size_t i=0;i<cb.words.size();++i) h.poke_word(word_addr(kCopList)+(uint32_t)i, cb.words[i]);

    c.loaded = h.load_program(dir + "/" + (prog.empty() ? prog_name_for(mut) : prog) + ".bin") > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    if (mut == Mut::C17) {
        // C17: the program stores its mark word after the second frame wait;
        // the sprite data words of both sprites then change to their later
        // value, before the next frame's sprite lines are fetched.
        c.cutover = h.run_until_write(kMarkAddr, kMarkValue, kBootBudget + kRunBudget);
        if (c.cutover) {
            for (uint32_t base : {kSpr0Data, kSpr1Data}) {
                const Pairs& pl = (base == kSpr0Data) ? c.pairs0_late : c.pairs1_late;
                uint32_t w = word_addr(base) + 2;
                for (auto& p : pl) { h.poke_word(w++, p.first); h.poke_word(w++, p.second); }
            }
        }
    }
    c.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    c.log = h.ram_log();
    // The smallest spacing between consecutive RAM-port samples is one bus
    // period (the measured spacing nmh::coalesce_adjacent takes): entries
    // that continue a bus cycle held by wait states (same address, direction,
    // data and byte enables, exactly one period later) are one logical access.
    for (size_t i = 1; i < c.log.size(); ++i) {
        uint64_t d = c.log[i].cycle - c.log[i-1].cycle;
        if (d > 0 && (c.period == 0 || d < c.period)) c.period = d;
    }
    c.video = h.video();
    // The fixture is read before anything else is read: it proves the
    // fixture can deliver a known value, including the DENISEID readback
    // recorded just below.
    c.fixture  = h.peek_logical(kResults + 2);
    c.deniseid = h.peek_logical(kResults + 0);
    return c;
}

// ---- pixels, U values, background-like (section 3.6, R1) ------------------

struct U { int r,g,b; };
bool operator==(const U& a, const U& b) { return a.r==b.r && a.g==b.g && a.b==b.b; }
U sample_u(const nmh::VideoSample& s) { return { (s.r>>2)&3, (s.g>>2)&3, (s.b>>2)&3 }; }
bool is_background_like(const U& u) {
    return (u.r==0 && u.g==0 && u.b==0) || (u.r==2 && u.g==2 && u.b==2);
}
const U kBg0 = {0,0,0}, kBg2 = {2,2,2};

// section 3.5's "Expected upper bits" table, indexed by group (0..14):
// group = 3*band+colour_idx (colour_idx 0=17,1=18,2=19); band 4 reuses band 3.
U expected_u(int group) {
    int band = group/3, ci = group%3;
    int tb = band>3 ? 3 : band;
    // v = (b + k) mod 4 per component, per SPECIFICATION.md 3.5's letter formulas,
    // restated here only to build the SAME table already given in 3.5's "Expected
    // upper bits" -- the literal (band,colour) -> U mapping, transcribed:
    static const U table[4][3] = {
        { {0,1,2}, {3,0,1}, {2,3,0} }, // band0: colour17,18,19
        { {1,2,3}, {0,1,2}, {3,0,1} }, // band1
        { {2,3,0}, {1,2,3}, {0,1,2} }, // band2
        { {3,0,1}, {2,3,0}, {1,2,3} }, // band3
    };
    return table[tb][ci];
}
U expected_background(int group) { return group <= 11 ? kBg0 : kBg2; }

// ---- line-level analysis (R5) ----------------------------------------------

struct LineAnalysis {
    nmh::Span span;
    std::vector<size_t> rising_edges; // absolute indices
    bool is_pattern_line=false;
    int pattern_spacing=0;
    bool fully_background=true;
};

LineAnalysis analyze_line(const std::vector<nmh::VideoSample>& video, nmh::Span line) {
    LineAnalysis la; la.span = line;
    std::vector<bool> bg(line.end-line.begin);
    for (size_t i=line.begin;i<line.end;++i) {
        U u = sample_u(video[i]);
        bool b = is_background_like(u);
        bg[i-line.begin] = b;
        if (!b) la.fully_background = false;
    }
    for (size_t i=1;i<bg.size();++i) if (!bg[i] && bg[i-1]) la.rising_edges.push_back(line.begin+i);

    size_t best_len = la.rising_edges.empty() ? 0 : 1, best_step = 0;
    if (la.rising_edges.size() >= 2) {
        size_t cur_len=1, cur_step=0;
        for (size_t i=1;i<la.rising_edges.size();++i) {
            size_t step = la.rising_edges[i]-la.rising_edges[i-1];
            cur_len = (i>=2 && step==cur_step) ? cur_len+1 : 2;
            cur_step = step;
            if (cur_len>best_len) { best_len=cur_len; best_step=cur_step; }
        }
    }
    la.is_pattern_line = best_len>=9;
    la.pattern_spacing = (int)best_step;
    return la;
}

struct Run { size_t start,end; }; // [start,end) absolute video indices

std::vector<Run> find_runs(const std::vector<nmh::VideoSample>& video, nmh::Span line) {
    std::vector<Run> runs;
    bool in=false; size_t s=0;
    for (size_t i=line.begin;i<line.end;++i) {
        bool bg = is_background_like(sample_u(video[i]));
        if (!bg && !in) { in=true; s=i; }
        if (bg && in)   { in=false; runs.push_back({s,i}); }
    }
    if (in) runs.push_back({s,line.end});
    return runs;
}

// ---- per-frame structure (section 3.6 R5) ----------------------------------

struct GroupLines { std::vector<nmh::Span> lines; };

struct FrameAnalysis {
    bool ok=false;
    std::string fail_reason;
    std::vector<LineAnalysis> pre_region;   // lines "above the sprite region"
    std::vector<GroupLines> groups;         // maximal group-line blocks (should be 15)
};

FrameAnalysis analyze_frame(const std::vector<nmh::VideoSample>& video, nmh::Span frame) {
    FrameAnalysis fa;
    auto all_lines = nmh::find_lines(video);
    std::vector<nmh::Span> lines;
    for (auto& l : all_lines) if (l.begin>=frame.begin && l.end<=frame.end) lines.push_back(l);

    std::vector<LineAnalysis> analyses;
    analyses.reserve(lines.size());
    for (auto& l : lines) analyses.push_back(analyze_line(video, l));

    int first_pattern = -1;
    for (size_t i=0;i<analyses.size();++i) if (analyses[i].is_pattern_line) { first_pattern=(int)i; break; }
    if (first_pattern < 0) { fa.fail_reason = "no pattern line found in frame"; return fa; }

    int region_start = -1;
    for (size_t i=first_pattern+1; i<analyses.size(); ) {
        if (!analyses[i].fully_background) { ++i; continue; }
        size_t j=i;
        while (j<analyses.size() && analyses[j].fully_background) ++j;
        if (j-i >= 4) { region_start = (int)i; break; }
        i=j;
    }
    if (region_start < 0) { fa.fail_reason = "no >=4-line background block found after the first pattern line"; return fa; }

    for (int i=0;i<region_start;++i) fa.pre_region.push_back(analyses[i]);

    bool in_group=false;
    for (size_t i=region_start;i<lines.size();++i) {
        bool gline = !analyses[i].fully_background;
        if (gline && !in_group) { fa.groups.push_back({}); in_group=true; }
        if (gline) fa.groups.back().lines.push_back(lines[i]);
        if (!gline) in_group=false;
    }

    fa.ok = true;
    return fa;
}

// ---- the colour check (section 3.7) ---------------------------------------

struct ColourCheck {
    bool groups_count_ok=false;
    bool runs_per_line_ok=true;
    bool interior_ok=true;           // every run of every group line has an interior pixel to grade
    bool run_length_ok=true;         // equal-length guard (p.101: "Sprites are 16 pixels wide")
    std::array<bool,15> colour_ok;   // per group: every interior pixel matches expected_u(group)
    int run_len_min=-1, run_len_max=-1; // observed run lengths this frame, samples
    // bits 1-0 of every graded pixel, per group per channel (section 3.7's own
    // logged, ungraded record): the smallest and largest value observed.
    std::array<int,15> r_lo, r_hi, g_lo, g_hi, b_lo, b_hi;
    std::string detail;
    ColourCheck() {
        colour_ok.fill(true);
        r_lo.fill(3); g_lo.fill(3); b_lo.fill(3);
        r_hi.fill(0); g_hi.fill(0); b_hi.fill(0);
    }
    // R5: 15 groups, two runs on every group line, every run with an
    // interior pixel, every run the same length.
    bool structural_ok() const { return groups_count_ok && runs_per_line_ok && interior_ok && run_length_ok; }
    bool group_ok_all() const { for (bool b: colour_ok) if(!b) return false; return true; }
    bool all_ok() const { return structural_ok() && group_ok_all(); }
};

ColourCheck colour_check(const std::vector<nmh::VideoSample>& video, const FrameAnalysis& fa) {
    ColourCheck cc;
    cc.groups_count_ok = (fa.groups.size()==15);
    if (!cc.groups_count_ok) cc.detail = "expected 15 groups, found " + std::to_string(fa.groups.size());

    size_t ngroups = std::min<size_t>(fa.groups.size(), 15);
    for (size_t g=0; g<ngroups; ++g) {
        U want = expected_u((int)g);
        for (auto& line : fa.groups[g].lines) {
            auto runs = find_runs(video, line);
            if (runs.size() != 2) { cc.runs_per_line_ok=false; continue; }
            std::sort(runs.begin(), runs.end(), [](const Run&a, const Run&b){ return a.start<b.start; });
            for (auto& r : runs) {
                size_t len = r.end-r.start;
                if (len < 3) cc.interior_ok = false; // no interior pixel: nothing of this run is graded
                if (cc.run_len_min<0 || (int)len<cc.run_len_min) cc.run_len_min=(int)len;
                if ((int)len>cc.run_len_max) cc.run_len_max=(int)len;
                for (size_t i=r.start+1; i+1<r.end; ++i) {
                    U u = sample_u(video[i]);
                    if (!(u==want)) cc.colour_ok[g]=false;
                    int br = video[i].r & 3, bgc = video[i].g & 3, bb = video[i].b & 3;
                    if (br<cc.r_lo[g]) cc.r_lo[g]=br;
                    if (br>cc.r_hi[g]) cc.r_hi[g]=br;
                    if (bgc<cc.g_lo[g]) cc.g_lo[g]=bgc;
                    if (bgc>cc.g_hi[g]) cc.g_hi[g]=bgc;
                    if (bb<cc.b_lo[g]) cc.b_lo[g]=bb;
                    if (bb>cc.b_hi[g]) cc.b_hi[g]=bb;
                }
            }
        }
    }
    // Every run of every graded group line the same length as every other
    // (p.101: "Sprites are 16 pixels wide"; SPECIFICATION.md 3.7). No page
    // states the width of a sprite pixel in SuperHires mode, so this grades
    // only the equality, not a number of visible pixels of its own.
    cc.run_length_ok = (cc.run_len_min>=0) && (cc.run_len_min==cc.run_len_max);
    return cc;
}

// ---- PC5-BOUNDS: the visible extent of a line (SPECIFICATION.md 3.8, section 6)
//
// The video port blanks the line outside its visible extent (nanomig_harness.h
// VideoSample is already blanked during hbl/vbl), so a REF pattern line, whose
// every visible sample is nonzero, has its first and last nonzero sample at
// the two ends of that extent. Blanking is a property of the line position,
// so the same extent applies to every other line of the frame (constant,
// since DIWSTRT/DIWSTOP are set once in step 4 and never rewritten). The
// extent is measured on the pattern line above the sprite region, anchored
// independently of every sprite and colour register any proposition in
// section 4 grades.
struct Window {
    size_t lo_off=0, hi_off=0;          // visible extent [lo_off,hi_off) from the line's own begin
    size_t lit_lo_off=0, lit_hi_off=0;  // the pattern's own lit extent [lit_lo_off,lit_hi_off)
    size_t line_len=0;                  // samples in the pattern line itself
    bool found=false;                   // a pattern line with a nonzero sample exists
    bool nonblack_inside=false;         // no zero sample inside [lo_off,hi_off)
    bool border_is_bg=false;            // outside the lit extent, inside the visible extent: U=(10,10,10), both sides nonempty
    bool ok() const { return found && nonblack_inside && border_is_bg; }
};

Window measure_window(const std::vector<nmh::VideoSample>& video, const FrameAnalysis& fa) {
    Window w;
    for (auto it = fa.pre_region.rbegin(); it != fa.pre_region.rend(); ++it) {
        if (!it->is_pattern_line) continue;
        size_t begin = it->span.begin, end = it->span.end;
        size_t lo = begin, hi = end;
        while (lo < hi && !(video[lo].r || video[lo].g || video[lo].b)) ++lo;
        while (hi > lo && !(video[hi-1].r || video[hi-1].g || video[hi-1].b)) --hi;
        if (hi <= lo) continue;
        w.found = true;
        w.line_len = end - begin;
        w.lo_off = lo - begin;
        w.hi_off = hi - begin;
        w.nonblack_inside = true;
        for (size_t i=lo; i<hi; ++i)
            if (!(video[i].r || video[i].g || video[i].b)) w.nonblack_inside = false;
        size_t lit_lo = hi, lit_hi = lo;
        for (size_t i=lo; i<hi; ++i)
            if (!is_background_like(sample_u(video[i]))) { if (lit_lo==hi) lit_lo = i; lit_hi = i+1; }
        w.lit_lo_off = lit_lo - begin;
        w.lit_hi_off = lit_hi - begin;
        w.border_is_bg = lit_lo > lo && hi > lit_hi;
        for (size_t i=lo; i<lit_lo && i<hi; ++i) if (!(sample_u(video[i])==kBg2)) w.border_is_bg = false;
        for (size_t i=lit_hi; i<hi; ++i)          if (!(sample_u(video[i])==kBg2)) w.border_is_bg = false;
        break;
    }
    return w;
}

// PC5: on every line of the group, every visible sample of the line (the
// extent of w), outside however many runs the line has (0, 1 or 2 --
// generalised so a contrast that removes a run does not itself break this
// check), apart from the one sample next to each run's own ends, must show
// the group's expected background (p.46: the background colour "is also
// displayed outside the defined display window, in the border area"). Every
// line must contribute at least one compared sample, or the group is
// reported as unpopulated.
struct Pc5Group { bool ok=true; bool populated=true; };

Pc5Group pc5_group(const std::vector<nmh::VideoSample>& video, const GroupLines& gl, U want_bg, const Window& w) {
    Pc5Group r;
    for (auto& line : gl.lines) {
        size_t wlo = line.begin + w.lo_off;
        size_t whi = line.begin + w.hi_off;
        if (whi > line.end) whi = line.end;
        auto runs = find_runs(video, line);
        std::sort(runs.begin(), runs.end(), [](const Run&a,const Run&b){ return a.start<b.start; });
        size_t cur = wlo, graded = 0;
        for (size_t ri=0; ri<=runs.size(); ++ri) {
            size_t seg_hi = (ri<runs.size()) ? (runs[ri].start>0 ? runs[ri].start-1 : runs[ri].start) : whi;
            size_t seg_lo = std::max(cur, wlo);
            seg_hi = std::min(seg_hi, whi);
            for (size_t i=seg_lo; i<seg_hi; ++i) {
                ++graded;
                if (!(sample_u(video[i])==want_bg)) r.ok = false;
            }
            if (ri<runs.size()) cur = runs[ri].end+1;
        }
        if (graded == 0) r.populated = false;
    }
    if (gl.lines.empty()) r.populated = false;
    return r;
}

// ---- RAM-log helpers (PC2, PC3, PC4) ---------------------------------------

uint64_t kNoCycle = UINT64_MAX;

// first in-range read whose data matches `value` at word address `wa`.
std::optional<uint64_t> first_read_cycle(const std::vector<nmh::RamAccess>& log, uint32_t wa, uint16_t value,
                                          uint64_t fbegin, uint64_t fend) {
    for (auto& a : log) {
        if (a.is_write || a.word_addr!=wa || a.data!=value) continue;
        if (a.cycle<fbegin || a.cycle>=fend) continue;
        return a.cycle;
    }
    return std::nullopt;
}

// The reads of [base_word, base_word+nwords) inside [fbegin,fend), in time
// order, as logical accesses: entries that continue a bus cycle held by wait
// states (same address, data and byte enables, exactly one measured bus
// period later: nmh::coalesce_adjacent) are one access, and two reads of the
// same word at any other spacing stay two.
std::vector<nmh::RamAccess> logical_reads(const Capture& c, uint32_t base_word, size_t nwords,
                                           uint64_t fbegin, uint64_t fend) {
    std::vector<nmh::RamAccess> reads;
    for (auto& a : c.log) {
        if (a.is_write) continue;
        if (a.cycle<fbegin || a.cycle>=fend) continue;
        if (a.word_addr<base_word || a.word_addr>=base_word+(uint32_t)nwords) continue;
        reads.push_back(a);
    }
    std::sort(reads.begin(), reads.end(), [](const nmh::RamAccess&a, const nmh::RamAccess&b){ return a.cycle<b.cycle; });
    return c.period ? nmh::coalesce_adjacent(reads, c.period) : reads;
}

// No word of the structure is read more than once (a read of a word with
// the prepared value counts once per logical access).
bool read_at_most_once(const Capture& c, uint32_t base_word, const std::vector<uint16_t>& expected,
                        uint64_t fbegin, uint64_t fend) {
    std::vector<size_t> n(expected.size(), 0);
    for (auto& a : logical_reads(c, base_word, expected.size(), fbegin, fend)) {
        size_t off = a.word_addr - base_word;
        if (a.data == expected[off] && ++n[off] > 1) return false;
    }
    return true;
}

// PC2 check 1 (SPECIFICATION.md 3.8): every offset of a structure fetched
// exactly once, at its own address, with the prepared value, in exactly
// the offset order Table 4-1 lays the structure out -- and no other read
// of any word inside the structure's own address range.
bool exact_ordered_fetch(const Capture& c, uint32_t base_word,
                          const std::vector<uint16_t>& expected, uint64_t fbegin, uint64_t fend) {
    auto reads = logical_reads(c, base_word, expected.size(), fbegin, fend);
    if (reads.size() != expected.size()) return false;
    for (size_t i=0;i<expected.size();++i)
        if (reads[i].word_addr!=base_word+(uint32_t)i || reads[i].data!=expected[i]) return false;
    return true;
}

struct Range { uint64_t lo=kNoCycle, hi=0; bool any=false; bool all=false; };

// PC4 check 1 (SPECIFICATION.md 3.8): the logical reads of the chunk's own
// address range in [fbegin,fend) are exactly the chunk's marked words, each
// once, in list order, with the prepared values (r.all): a missing, a
// duplicate, an extra or an out-of-order read makes r.all false. lo and hi
// bound every one of those reads, not the first match of each word.
Range chunk_range(const Capture& c, uint32_t base_word,
                   const std::vector<uint16_t>& expected, size_t off_lo, size_t off_hi,
                   uint64_t fbegin, uint64_t fend) {
    Range r;
    if (off_hi <= off_lo) return r;
    const size_t n = off_hi - off_lo;
    auto reads = logical_reads(c, base_word+(uint32_t)off_lo, n, fbegin, fend);
    bool complete = reads.size() == n;
    for (size_t i=0; i<reads.size(); ++i) {
        if (i >= n || reads[i].word_addr != base_word+(uint32_t)(off_lo+i) || reads[i].data != expected[off_lo+i])
            complete = false;
        r.any = true;
        r.lo = std::min(r.lo, reads[i].cycle);
        r.hi = std::max(r.hi, reads[i].cycle);
    }
    r.all = complete;
    return r;
}

std::optional<uint64_t> sprite_line_low_cycle(const std::vector<nmh::RamAccess>& log, uint32_t sprite_base,
                                               const std::vector<std::pair<uint16_t,uint16_t>>& pairs,
                                               uint32_t line, uint64_t fbegin, uint64_t fend) {
    uint32_t idx = line - 0x50;
    if (idx >= pairs.size()) return std::nullopt;
    uint32_t off = 2 + 2*idx; // header (POS,CTL) then pairs
    return first_read_cycle(log, word_addr(sprite_base)+off, pairs[idx].first, fbegin, fend);
}

std::optional<uint64_t> first_bitplane_cycle(const std::vector<nmh::RamAccess>& log, uint64_t fbegin, uint64_t fend) {
    uint32_t base = word_addr(kBplBuf);
    std::optional<uint64_t> best;
    for (auto& a : log) {
        if (a.is_write || a.data!=0xFF00) continue;
        if (a.word_addr<base || a.word_addr>=base+kBplWords) continue;
        if (a.cycle<fbegin || a.cycle>=fend) continue;
        if (!best || a.cycle<*best) best = a.cycle;
    }
    return best;
}

// ---- graded frames (R3) -----------------------------------------------------

struct GradedRun {
    std::vector<uint64_t> restarts;    // cycles of Copper-list-start refetches
    std::vector<nmh::Span> frames;     // graded frames (fully between the 2nd restart and DONE, R3)
    std::vector<nmh::Span> passes;     // graded Copper passes: [restarts[k],restarts[k+1]), k>=1, both <= DONE
};

GradedRun find_graded(const Capture& c) {
    GradedRun g;
    uint32_t cop0 = word_addr(kCopList);
    for (auto& a : c.log) {
        if (a.is_write || a.word_addr!=cop0 || a.data!=0x00E0) continue;
        if (g.restarts.empty() || a.cycle != g.restarts.back()) g.restarts.push_back(a.cycle);
    }
    if (g.restarts.size()<2 || !c.done) return g;
    uint64_t start = g.restarts[1];
    uint64_t end   = *c.done;
    for (auto& f : nmh::find_frames(c.video))
        if (f.begin>=start && f.end<=end) g.frames.push_back(f);
    // A "pass" is bounded by two consecutive Copper restarts, which is what
    // PC4's own bus-timing clauses are relative to; a video frame's vsync
    // edge does not fall exactly on the Copper's own restart cycle, so using
    // find_frames() spans for this would occasionally straddle two passes.
    for (size_t k=1; k+1<g.restarts.size(); ++k)
        if (g.restarts[k]>=start && g.restarts[k+1]<=end)
            g.passes.push_back({(size_t)g.restarts[k], (size_t)g.restarts[k+1]});
    return g;
}

// ---- PC4: Copper-vs-sprite bus timing --------------------------------------

bool pc4_frame_ok(const Capture& c, nmh::Span f) {
    uint32_t cop0 = word_addr(kCopList);

    auto top = chunk_range(c, cop0, c.cop_words, 0, c.mark.at("top_end"), f.begin, f.end);
    auto bpl = first_bitplane_cycle(c.log, f.begin, f.end);
    bool a_ok = top.all && bpl && top.hi < *bpl;

    auto b0 = chunk_range(c, cop0, c.cop_words, c.mark.at("b0_start"), c.mark.at("b0_end"), f.begin, f.end);
    auto line50 = sprite_line_low_cycle(c.log, kSpr0Data, c.p0(f), 0x50, f.begin, f.end);
    bool b_ok = b0.all && line50 && b0.hi < *line50;

    bool c_ok = true;
    // Band 4's chunk starts at "b4_start", not "band4_start": that range
    // also covers the COLOR00-COLOR15<-B4 MOVEs, which band 4 alone carries
    // between the same two sprite-line fetches (SPECIFICATION.md 3.8, PC4c).
    static const char* starts[5] = {nullptr,"band1_start","band2_start","band3_start","b4_start"};
    static const char* ends[5]   = {nullptr,"band1_end","band2_end","band3_end","band4_end"};
    for (int b=1;b<=4;++b) {
        auto chunk = chunk_range(c, cop0, c.cop_words, c.mark.at(starts[b]), c.mark.at(ends[b]), f.begin, f.end);
        uint32_t s_b = 0x50 + 29*b;
        auto lo = sprite_line_low_cycle(c.log, kSpr0Data, c.p0(f), s_b,   f.begin, f.end);
        auto hi = sprite_line_low_cycle(c.log, kSpr0Data, c.p0(f), s_b+2, f.begin, f.end);
        if (!(chunk.all && lo && hi && *lo < chunk.lo && chunk.hi < *hi)) c_ok = false;
    }
    return a_ok && b_ok && c_ok;
}

// ---- R1/R2's own HIRES-spacing measurement (SPECIFICATION.md 3.6, 3.8) ----
// R1's anchor capture has to establish -- not merely assume -- that a capture
// taken with BPLCON0 = $9000 actually is HIRES, by measuring the pattern
// line's own spacing (p.300: SuperHires's 35ns rate is "twice the horizontal
// resolution of Hires mode", so an 8-visible-pixel SuperHires spacing
// becomes 16 under HIRES).

struct SpacingMeasure { bool any=false; int min_spacing=0, max_spacing=0; };

SpacingMeasure measure_pattern_spacing(const std::vector<nmh::VideoSample>& video,
                                        const std::vector<nmh::Span>& frames) {
    SpacingMeasure sm;
    for (auto& f : frames) {
        FrameAnalysis fa = analyze_frame(video, f);
        if (!fa.ok) continue;
        for (auto& la : fa.pre_region) {
            if (!la.is_pattern_line) continue;
            if (!sm.any) { sm.min_spacing = sm.max_spacing = la.pattern_spacing; sm.any = true; }
            else {
                sm.min_spacing = std::min(sm.min_spacing, la.pattern_spacing);
                sm.max_spacing = std::max(sm.max_spacing, la.pattern_spacing);
            }
        }
    }
    return sm;
}

// ---- assertion labels -------------------------------------------------------
// A mutation record names the leading id of the label its own expect() prints.
// Every selector runs the same evaluation, so every selector prints these same
// labels.

const char* kProgLabel = "prog.bin loaded";
const char* kCpuLabel  = "cpu_nrst_out released within the boot budget";
const char* kDoneLabel = "DONE: the program's DONE mark was written";
const char* kFixLabel  =
    "fixture: the program-stored constant $BEEF was read back before anything else is graded";
const char* kR1Label =
    "R1 p.301 p.300 (anchor): with HIRES (SHRES=0) instead of SuperHires -- established by a measured "
    "pattern-line spacing of 16 visible pixels, twice the SuperHires 8-pixel spacing -- COLOR17-COLOR19 "
    "read as ordinary 0x0RGB registers already show the section 3.5 U values for every group, "
    "establishing the channel and bit order (and the 8-pixel reading PC1 rests on) before PC1 and "
    "P1-P4 are graded";
const char* kPc1Label =
    "PC1 p.300: SuperHires is in effect (every graded frame has a pattern line above the sprite "
    "region, spacing 8 visible pixels, half of the 16 R1 measures with SHRES=0)";
const char* kPc2Label =
    "PC2 p.107 p.108: sprites 0 and 1 fetch POS, CTL and all 145 data pairs ($50-$E0) with "
    "the prepared values plus the terminator, exactly once each, each at its own fixed "
    "offset in Table 4-1's own order, with no other read of either structure's own address "
    "range, every graded frame";
const char* kPc3Label =
    "PC3 p.114 p.115: sprites 2 to 7 are idle (each of their six independently identifiable "
    "null structures' two words is read exactly once, and no word of sprite 0's or sprite "
    "1's own structure is read more than once)";
const char* kPc5BoundsLabel =
    "PC5-BOUNDS p.46 p.59: the visible extent of the REF pattern line above the sprite region (its "
    "first to its last nonzero sample) is the same in every graded frame, holds no zero sample, "
    "and shows U=(10,10,10), COLOR00's own colour (p.46), on every sample before the pattern's "
    "first and after its last lit sample";
const char* kPc5Label =
    "PC5 p.302 p.46 p.301: band 4's reload of COLOR00-COLOR15 took effect: on every line of groups "
    "12-14 every visible sample of the line outside the two runs (the whole extent PC5-BOUNDS measures, "
    "border area included: p.46, \"also displayed outside the defined display window, in the border "
    "area\"), apart from the sample next to each run end, shows U=(10,10,10), on groups 0-11 "
    "U=(00,00,00), every graded frame";
// R5 (SPECIFICATION.md 3.6, 3.8): the group/run structure itself, graded
// once for the whole frame, separately from any single group's colour
// (P1-P4, which grade only their own groups' colour). A structural defect
// (a missing group, a stray run, an unequal run length, a run with no
// interior pixel) must not by itself make an unrelated colour proposition
// go red: it makes them NOT GRADED.
const char* kR5Label =
    "R5 p.101 p.302: the sprite region has exactly 15 groups, every group line has exactly two runs, "
    "every run has an interior pixel, and every run of every graded group line is the same length as "
    "every other (p.101: \"Sprites are 16 pixels wide\")";
const char* kP1Label =
    "P1 p.302: SuperHires sprite colours are held in COLOR16-COLOR31, not in "
    "the lower sixteen registers (groups 0-11 track their band, groups 12-14 match band 3's "
    "colours unchanged by band 4's own reload of COLOR00-COLOR15)";
const char* kP2Label =
    "P2 p.302 APPC-sprkeykey.c17r APPC-sprkeykey.c17g APPC-sprkeykey.c17b "
    "CH4-appC-color.c5 CH4-appC-color.c6 CH4-appC-color.c7: colour 17 is displayed with red "
    "bits3-2=GH, green bits3-2=IJ, blue bits3-2=KL on groups 0,3,6,9,12";
const char* kP3Label =
    "P3 p.302 APPC-sprkeykey.c18r APPC-sprkeykey.c18g APPC-sprkeykey.c18b "
    "CH4-appC-color.c8 CH4-appC-color.c9 CH4-appC-color.c10: colour 18 is displayed with red "
    "bits3-2=MN, green bits3-2=OP, blue bits3-2=QR on groups 1,4,7,10,13";
const char* kP4Label =
    "P4 p.302 APPC-sprkeykey.c19r APPC-sprkeykey.c19g APPC-sprkeykey.c19b "
    "CH4-appC-color.c11 CH4-appC-color.c12 CH4-appC-color.c13: colour 19 is displayed with red "
    "bits3-2=ST, green bits3-2=UV, blue bits3-2=WX on groups 2,5,8,11,14";

// ---- the evaluation of every graded frame ---------------------------------

struct Verdict {
    bool structure_ok = true;                // R5, every graded frame
    std::array<bool,15> colour_ok;           // per group, every graded frame (P1-P4)
    bool bounds_ok = true;                   // PC5-BOUNDS
    bool bounds_set = false;
    size_t lo=0, hi=0, lit_lo=0, lit_hi=0, line_len=0;   // the measured extent, first graded frame
    bool pc5_populated = true, pc5_ok = true;
    int run_len_lo=-1, run_len_hi=-1;        // R2's own SuperHires run-width measurement (ungraded)
    std::vector<std::pair<int,int>> frame_runs; // per graded frame: smallest and largest run length
    std::array<int,15> r_lo, r_hi, g_lo, g_hi, b_lo, b_hi;
    Verdict() {
        colour_ok.fill(true);
        r_lo.fill(3); g_lo.fill(3); b_lo.fill(3);
        r_hi.fill(0); g_hi.fill(0); b_hi.fill(0);
    }
};

Verdict evaluate(const Capture& c, const GradedRun& g) {
    Verdict v;
    for (auto& f : g.frames) {
        FrameAnalysis fa = analyze_frame(c.video, f);
        if (!fa.ok) { v.structure_ok = false; v.bounds_ok = false; v.pc5_populated = false; continue; }
        ColourCheck cc = colour_check(c.video, fa);
        if (!cc.structural_ok()) v.structure_ok = false;
        v.frame_runs.push_back({cc.run_len_min, cc.run_len_max});
        if (cc.run_len_min>=0) {
            v.run_len_lo = (v.run_len_lo<0) ? cc.run_len_min : std::min(v.run_len_lo, cc.run_len_min);
            v.run_len_hi = std::max(v.run_len_hi, cc.run_len_max);
        }
        for (int g0=0; g0<15; ++g0) {
            if (!cc.colour_ok[g0]) v.colour_ok[g0] = false;
            v.r_lo[g0]=std::min(v.r_lo[g0], cc.r_lo[g0]); v.r_hi[g0]=std::max(v.r_hi[g0], cc.r_hi[g0]);
            v.g_lo[g0]=std::min(v.g_lo[g0], cc.g_lo[g0]); v.g_hi[g0]=std::max(v.g_hi[g0], cc.g_hi[g0]);
            v.b_lo[g0]=std::min(v.b_lo[g0], cc.b_lo[g0]); v.b_hi[g0]=std::max(v.b_hi[g0], cc.b_hi[g0]);
        }

        Window w = measure_window(c.video, fa);
        if (!w.ok()) v.bounds_ok = false;
        if (w.found) {
            if (!v.bounds_set) { v.bounds_set = true; v.lo=w.lo_off; v.hi=w.hi_off; v.lit_lo=w.lit_lo_off; v.lit_hi=w.lit_hi_off; v.line_len=w.line_len; }
            else if (v.lo!=w.lo_off || v.hi!=w.hi_off) v.bounds_ok = false;
        }
        if (fa.groups.size()==15 && w.ok()) {
            for (int gi=0; gi<15; ++gi) {
                Pc5Group r = pc5_group(c.video, fa.groups[gi], expected_background(gi), w);
                if (!r.ok) v.pc5_ok = false;
                if (!r.populated) v.pc5_populated = false;
            }
        } else v.pc5_populated = false;
    }
    if (!v.bounds_set) v.bounds_ok = false;
    // R5's equal-length clause holds across all graded frames together, not per frame only
    if (v.run_len_lo >= 0 && v.run_len_lo != v.run_len_hi) v.structure_ok = false;
    return v;
}

// ---- R1's anchor capture ----------------------------------------------------
// An internal HIRES capture (prog_c1.bin), taken in every run. NMH_MUTATE=R1
// swaps G and H of band 1's COLOR16-COLOR31 in this capture's Copper list
// only; NMH_MUTATE=R1S runs the SuperHires program in this capture only. The
// run's own capture is untouched by both.

struct Anchor {
    bool ok = false;
    SpacingMeasure spacing;
    int run_lo=-1, run_hi=-1;
};

Anchor run_anchor(const std::string& dir, Mut sel) {
    Anchor a;
    std::optional<Mut> copper;
    std::string prog;
    if (sel == Mut::R1)  copper = Mut::C9;
    if (sel == Mut::R1S) prog = "prog";
    Capture c1 = run_case(dir, Mut::C1, copper, prog);
    a.ok = c1.loaded && c1.done.has_value();
    if (!a.ok) return a;
    GradedRun g1 = find_graded(c1);
    a.ok = !g1.frames.empty();
    for (auto& f : g1.frames) {
        FrameAnalysis fa = analyze_frame(c1.video, f);
        if (!fa.ok) { a.ok = false; continue; }
        ColourCheck cc = colour_check(c1.video, fa);
        if (!cc.all_ok()) a.ok = false;
        if (cc.run_len_min>=0) {
            a.run_lo = (a.run_lo<0) ? cc.run_len_min : std::min(a.run_lo, cc.run_len_min);
            a.run_hi = std::max(a.run_hi, cc.run_len_max);
        }
    }
    // Establish the premise that this capture is actually HIRES (p.300)
    // before trusting its colour check as the R1 anchor.
    a.spacing = measure_pattern_spacing(c1.video, g1.frames);
    if (!a.spacing.any || a.spacing.min_spacing!=16 || a.spacing.max_spacing!=16) a.ok = false;
    return a;
}

// ---- one run: the same evaluation for the normal run and every selector ----

std::string join_not_holding(const std::string& ids) {
    return ids.empty() ? "" : "precondition not holding: " + ids;
}

size_t reads_in(const Capture& c, uint32_t base_byte, size_t nwords, nmh::Span f) {
    return logical_reads(c, word_addr(base_byte), nwords, f.begin, f.end).size();
}

void run_all(const std::string& dir, Mut mut, Checks& checks) {
    Capture c = run_case(dir, mut);
    const bool loaded = c.loaded;
    const bool ran = c.loaded && c.done.has_value();
    const std::string no_img = "no program image was loaded (prog.bin)";

    grade(checks, kProgLabel, "", loaded);
    grade(checks, kCpuLabel, loaded ? "" : no_img, c.released);
    grade(checks, kDoneLabel, loaded ? "" : no_img, c.done.has_value());
    grade(checks, kFixLabel, loaded ? "" : no_img, c.fixture.has_value() && *c.fixture==0xBEEF);
    if (c.deniseid)
        checks.note("DENISEID = " + std::to_string(*c.deniseid) + " (p.299, recorded only)");

    // R1 comes before every line that rests on it (PC1's 8 visible pixels, P1-P4).
    Anchor an = run_anchor(dir, mut);
    St s_r1 = grade(checks, kR1Label, "", an.ok);
    if (an.spacing.any)
        checks.note("R1/R2 p.300 anchor: the anchor capture's pattern-line spacing measures " +
                    std::to_string(an.spacing.min_spacing) + "-" + std::to_string(an.spacing.max_spacing) +
                    " visible pixels");
    if (an.run_hi >= 0)
        checks.note("R2 p.101 (anchor, ungraded): in the anchor capture, sprite runs measure " +
                    std::to_string(an.run_lo) + "-" + std::to_string(an.run_hi) + " visible pixels");

    const std::string no_run = ran ? "" : (loaded ? "the program did not reach its DONE mark (DONE)" : no_img);
    GradedRun g = ran ? find_graded(c) : GradedRun{};
    const std::string no_frames = !no_run.empty() ? no_run : (g.frames.empty() ? "no graded frame (PC6)" : "");
    if (loaded) {
        size_t logical = c.log.empty() ? 0 : (c.period ? nmh::coalesce_adjacent(c.log, c.period).size() : c.log.size());
        checks.note("RAM-port log: " + std::to_string(c.log.size()) + " samples, " + std::to_string(logical) +
                    " logical accesses after collapsing wait-state repeats, bus period " +
                    std::to_string(c.period) + " clk_sys cycles (measured)");
    }

    St s_pc6 = grade(checks,
        "PC6 p.25 p.30: at least two graded frames (R3) -- " + std::to_string(g.restarts.size()) +
        " Copper restart(s) observed, " + std::to_string(g.frames.size()) + " graded frame(s)",
        no_run, g.frames.size()>=2);

    // PC1: a pattern line above the sprite region, spacing 8 visible pixels, every frame.
    bool pc1 = true;
    for (auto& f : g.frames) {
        FrameAnalysis fa = analyze_frame(c.video, f);
        if (!fa.ok) { pc1=false; continue; }
        int npat=0; bool spacing_ok=true;
        for (auto& la : fa.pre_region) if (la.is_pattern_line) { ++npat; if (la.pattern_spacing!=8) spacing_ok=false; }
        if (npat<1 || !spacing_ok) pc1=false;
    }
    St s_pc1 = grade(checks, kPc1Label, no_frames, pc1);

    // PC2: sprites 0 and 1 fetch exactly the prepared data, every frame.
    auto push_struct = [](uint16_t pos, uint16_t ctl, const Pairs& pairs) {
        std::vector<uint16_t> v;
        v.push_back(pos); v.push_back(ctl);
        for (auto& p : pairs) { v.push_back(p.first); v.push_back(p.second); }
        v.push_back(0); v.push_back(0);
        return v;
    };
    // The data each graded frame fetches is the data prepared for it (C17 changes it between frames).
    auto spr0_exp = [&](nmh::Span f) { return push_struct(0x5050, 0xE100, c.p0(f)); };
    auto spr1_exp = [&](nmh::Span f) { return push_struct(0x5060, 0xE100, c.p1(f)); };
    bool pc2_s0 = true, pc2_s1 = true;
    for (auto& f : g.frames) {
        if (!exact_ordered_fetch(c, word_addr(kSpr0Data), spr0_exp(f), f.begin, f.end)) pc2_s0=false;
        if (!exact_ordered_fetch(c, word_addr(kSpr1Data), spr1_exp(f), f.begin, f.end)) pc2_s1=false;
    }
    St s_pc2 = grade(checks, kPc2Label, no_frames, pc2_s0 && pc2_s1);
    if (no_frames.empty()) {
        std::string s = std::string("PC2 detail (ungraded): sprite 0 ") + (pc2_s0 ? "PASS" : "FAIL") +
                        ", sprite 1 " + (pc2_s1 ? "PASS" : "FAIL") + "; logical reads per graded frame";
        for (size_t fi=0; fi<g.frames.size(); ++fi) {
            const auto& f = g.frames[fi];
            s += "; frame " + std::to_string(fi) + ": sprite 0 " +
                 std::to_string(reads_in(c, kSpr0Data, spr0_exp(f).size(), f)) + "/" +
                 std::to_string(spr0_exp(f).size()) + ", sprite 1 " +
                 std::to_string(reads_in(c, kSpr1Data, spr1_exp(f).size(), f)) + "/" +
                 std::to_string(spr1_exp(f).size());
            for (int i=0; i<6; ++i)
                s += ", null" + std::to_string(i+2) + " " + std::to_string(reads_in(c, kNullAddrs[i], 2, f)) + "/2";
            s += ", sprite-2 structure " + std::to_string(reads_in(c, kSpr2Data, 294, f)) + "/294";
        }
        checks.note(s);
    }

    // PC3: sprites 2-7 idle. Each of the six is given its own safe-null
    // structure (ch4.i), so each of the six must show the exact same
    // two-word fetch PC2 requires of a real sprite; sprite 0 and sprite 1's
    // own structures must not be over-fetched.
    bool pc3 = true;
    for (auto& f : g.frames) {
        for (uint32_t na : kNullAddrs)
            if (!exact_ordered_fetch(c, word_addr(na), {0,0}, f.begin, f.end)) pc3=false;
        if (!read_at_most_once(c, word_addr(kSpr0Data), spr0_exp(f), f.begin, f.end)) pc3=false;
        if (!read_at_most_once(c, word_addr(kSpr1Data), spr1_exp(f), f.begin, f.end)) pc3=false;
    }
    St s_pc3 = grade(checks, kPc3Label, no_frames, pc3);

    // PC4: Copper colour loads fall between the displayed sprite pixels, on a
    // nonempty set of graded Copper passes covering the graded frames.
    bool pc4 = true;
    for (auto& f : g.passes) if (!pc4_frame_ok(c, f)) pc4=false;
    std::string pc4_ng = no_frames;
    if (pc4_ng.empty() && (g.passes.empty() || g.passes.size() < g.frames.size()))
        pc4_ng = "fewer graded Copper passes than graded frames";
    St s_pc4 = grade(checks,
        "PC4 p.126 p.23: the Copper's colour-register MOVEs fall between sprite 0's own pair "
        "fetches on the bus (top-of-list before the first bitplane fetch, the B0 reload before "
        "line $50, each band's reload between its own line and two lines later), on " +
        std::to_string(g.passes.size()) + " graded Copper pass(es) for " + std::to_string(g.frames.size()) +
        " graded frame(s)", pc4_ng, pc4);

    Verdict v;
    if (no_frames.empty()) v = evaluate(c, g);

    St s_bounds = grade(checks, kPc5BoundsLabel, no_frames, v.bounds_ok);
    if (no_frames.empty() && v.bounds_set)
        checks.note("PC5-BOUNDS: the visible extent of the pattern line measures [" + std::to_string(v.lo) + "," +
                    std::to_string(v.hi) + ") of its " + std::to_string(v.line_len) + " samples, " + std::to_string(v.hi - v.lo) +
                    " samples, and its lit pattern [" + std::to_string(v.lit_lo) + "," + std::to_string(v.lit_hi) + ")");

    std::string pc5_ng = no_frames;
    if (pc5_ng.empty() && s_bounds != St::PASS) pc5_ng = "precondition not holding: PC5-BOUNDS";
    if (pc5_ng.empty() && !v.pc5_populated)
        pc5_ng = "the sprite region does not show 15 groups with a compared sample on every group line";
    St s_pc5 = grade(checks, kPc5Label, pc5_ng, v.pc5_ok);

    St s_r5 = grade(checks, kR5Label, no_frames, v.structure_ok);
    if (no_frames.empty()) {
        std::string s = "R5 detail (ungraded): run length in samples per graded frame (smallest-largest)";
        for (size_t fi=0; fi<v.frame_runs.size(); ++fi)
            s += "; frame " + std::to_string(fi) + ": " + std::to_string(v.frame_runs[fi].first) + "-" +
                 std::to_string(v.frame_runs[fi].second);
        checks.note(s);
    }

    // P1-P4 grade their own colour clauses only when every precondition of
    // SPECIFICATION.md 4 item 5 holds, and only on their whole group sets.
    std::string base = not_holding({{"R1",s_r1},{"PC6",s_pc6},{"PC1",s_pc1},{"PC2",s_pc2},{"PC3",s_pc3},
                                    {"PC4",s_pc4},{"R5",s_r5}});
    std::string p1need = base;
    {
        std::string extra = not_holding({{"PC5-BOUNDS",s_bounds},{"PC5",s_pc5}});
        if (!extra.empty()) p1need += (p1need.empty() ? "" : " ") + extra;
    }
    auto set_ok = [&](std::initializer_list<int> gs) { for (int x : gs) if (!v.colour_ok[x]) return false; return true; };
    bool p1 = true; for (int x=0; x<15; ++x) if (!v.colour_ok[x]) p1 = false;
    grade(checks, kP1Label, join_not_holding(p1need), p1);
    grade(checks, kP2Label, join_not_holding(base), set_ok({0,3,6,9,12}));
    grade(checks, kP3Label, join_not_holding(base), set_ok({1,4,7,10,13}));
    grade(checks, kP4Label, join_not_holding(base), set_ok({2,5,8,11,14}));

    if (v.run_len_hi>=0)
        checks.note("R2 p.101 (ungraded): the sprite run width of this run measures " +
                    std::to_string(v.run_len_lo) + "-" + std::to_string(v.run_len_hi) + " visible pixels");

    checks.record("CH4-appC-color.c1 (p.302): P1 loads all sixteen upper registers (COLOR16-COLOR31) "
                  "together and observes only colours 17-19 (COLOR17-COLOR19), so it cannot show that all "
                  "sixteen registers participate in the SuperHires scheme; the per-register decode is graded "
                  "at the unit level, in hrm/ch3-shres-sprite-colour-encoding. P1 keeps grading what it "
                  "observes: that reloading COLOR16-COLOR31 changes the displayed colours as the scheme "
                  "predicts, and reloading COLOR00-COLOR15 does not.");

    // Bits 1-0 of every graded pixel, per group (section 3.7): recorded, never graded.
    if (no_frames.empty()) {
        for (int gi=0; gi<15; ++gi) {
            checks.note("group " + std::to_string(gi) + " bits1-0 (section 3.7, ungraded): R=[" +
                        std::to_string(v.r_lo[gi]) + "," + std::to_string(v.r_hi[gi]) + "] G=[" +
                        std::to_string(v.g_lo[gi]) + "," + std::to_string(v.g_hi[gi]) + "] B=[" +
                        std::to_string(v.b_lo[gi]) + "," + std::to_string(v.b_hi[gi]) + "]");
        }
    }

    // ---- claims this package declines, restated from SPECIFICATION.md ------
    checks.record("APPC-sprkeykey.c16r, APPC-sprkeykey.c16g, APPC-sprkeykey.c16b, CH4-appC-color.c2, "
                   "CH4-appC-color.c3, CH4-appC-color.c4 (p.302, 6 claims): colour 16 is never shown by any "
                   "sprite pixel per pp.103,108 and Table 4-6; no page states a mechanism to reach it, so no "
                   "proposition is made (SPECIFICATION.md 1.1)");
    checks.record("APPC-sprtab.r10b00 to APPC-sprtab.r17b11, CH4-appC-color.c18-c29, c34-c45, c50-c61, "
                   "c66-c77, c82-c93, c98-c109, c114-c125, c130-c141 (192 claims): each names one bit of one "
                   "cell of the page-302 table; no page says which register the display reads for a pixel or "
                   "which bits reach it, so no single cell can be graded on its own -- used as stimulus only "
                   "(SPECIFICATION.md 1.2)");
    checks.record("CH4-appC-color.c14-c17, c30-c33, c46-c49, c62-c65, c78-c81, c94-c97, c110-c113, c126-c129 "
                   "(32 claims): the table's dot bits (bits 15-12 of each register); no page states a value for "
                   "them (SPECIFICATION.md 1.3)");
    checks.record("the dash half of every key claim's component (APPC-sprkeykey.*, CH4-appC-color.c2-c13): "
                   "a bit the key does not encode predicts nothing a test can see; masked and logged, never "
                   "graded (SPECIFICATION.md 1.4). The letter half of colour 17/18/19's claims is graded under "
                   "P2/P3/P4.");
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc>1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));

    const char* env = std::getenv("NMH_MUTATE");
    std::string mutate = env ? env : "";
    std::printf("NMH_MUTATE: %s\n", mutate.empty() ? "(none)" : mutate.c_str());

    Checks checks;
    auto mut = parse_mut(mutate);
    if (!mut) checks.record("unrecognised NMH_MUTATE value: " + mutate);
    else      run_all(dir, *mut, checks);

    std::printf("%d lines not graded\n", checks.ungraded);
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed==0 ? 0 : 1;
}
