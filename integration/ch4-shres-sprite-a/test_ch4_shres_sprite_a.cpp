// test_ch4_shres_sprite_a.cpp -- ch4-shres-sprite-a. See SPECIFICATION.md
// for every assertion's proposition, program, observation and expected
// value. This file only implements what SPECIFICATION.md already
// committed to. The sprite/bitplane/Copper-list *data* is poked directly
// into chip RAM by this file; prog.s and its mutation wrappers only perform
// the CPU register writes of section 3.3.
//
// NMH_MUTATE selects a mutation in place of the normal run, one at a time;
// none is exercised by a plain `make test`. The normal run itself also
// captures C1 (prog_c1.bin) once, internally, to assert the R1/R2 anchor
// (SPECIFICATION.md 3.6 R1) before P1-P4 are graded; that capture is
// separate from the NMH_MUTATE=C1 replay below, which grades PC1's own
// mutation record.
//
// MUTATION PC1 stimulus red NMH_MUTATE=C1: BPLCON0=$9000 (HIRES, SHRES=0) instead of $1040 -- pattern spacing becomes 16 visible pixels instead of 8 -- measured: fx68k FAIL, TG68K FAIL
// MUTATION PC2 stimulus red NMH_MUTATE=C2: sprite 1's own terminator word (p.108: both terminator words should be $0000) is poked to $BAD1 instead -- the terminator lies beyond CTL's VSTOP, so it is never displayed and sprite 1's own measured population (all 294 words) stays fully fetched -- measured: fx68k FAIL, TG68K FAIL
// MUTATION PC3 stimulus red NMH_MUTATE=C3: SPR2PTH (step 5 and the Copper list) points at sprite 0's data instead of its own null2 structure -- null2's own two words go unread and sprite 0's own words are read more than once -- measured: fx68k FAIL, TG68K FAIL
// MUTATION PC4 stimulus red NMH_MUTATE=C4: band 2's WAIT changed from $8B41 to $9141 (the fifth colour-17 line of band 2) -- measured: fx68k FAIL, TG68K FAIL
// MUTATION PC5 stimulus red NMH_MUTATE=C5: the COLOR00-COLOR15 <- B4 MOVEs after WAIT $C541 are left out -- measured: fx68k FAIL, TG68K FAIL
// MUTATION PC6 stimulus red NMH_MUTATE=C6: three frame waits after step 7 instead of four, so fewer than two graded frames occur (measured: 1) while the measured frame population stays nonempty -- measured: fx68k FAIL, TG68K FAIL
// MUTATION R5 stimulus red NMH_MUTATE=C7: colour-17 lines of band 0 ($53-$5A), both sprites, get low=$FF00/high=$0000 instead of $FFFF/$0000 -- group 0's own runs narrow to 8 visible pixels while every other group's runs stay at their own (equal) length, breaking the equal-length guard inside R5 (section 3.7); p.101 permits the shape change, so this is not a P2 contrast (C9 already tests P2's own colour clause) -- P1 and P2 stay green -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P1 stimulus red NMH_MUTATE=C8: band 4's COLOR16-COLOR31 reload (after WAIT $C541) uses band 0's values instead of band 3's -- groups 12-14 stop matching band 3 (PC5 and R5 stay green: neither depends on COLOR16-COLOR31 or on colour content) -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P2 stimulus red NMH_MUTATE=C8: group 12 (band 4, colour 17) belongs to P2's own group set (0,3,6,9,12), so C8 prints red here too -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P3 stimulus red NMH_MUTATE=C8: group 13 (band 4, colour 18) belongs to P3's own group set (1,4,7,10,13), so C8 prints red here too -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P4 stimulus red NMH_MUTATE=C8: group 14 (band 4, colour 19) belongs to P4's own group set (2,5,8,11,14), so C8 prints red here too -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P1 stimulus red NMH_MUTATE=C9: band 1's COLOR16-COLOR31 reload swaps G and H -- group 3 alone stops matching, and P1 grades all fifteen groups -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P2 stimulus red NMH_MUTATE=C9: group 3 also belongs to P2's own group set (0,3,6,9,12) -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P1 stimulus red NMH_MUTATE=C10: band 2's COLOR16-COLOR31 reload swaps O and P -- group 7 alone stops matching, and P1 grades all fifteen groups -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P3 stimulus red NMH_MUTATE=C10: group 7 also belongs to P3's own group set (1,4,7,10,13) -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P1 stimulus red NMH_MUTATE=C11: band 2's COLOR16-COLOR31 reload swaps W and X -- group 8 alone stops matching, and P1 grades all fifteen groups -- measured: fx68k FAIL, TG68K FAIL
// MUTATION P4 stimulus red NMH_MUTATE=C11: group 8 also belongs to P4's own group set (2,5,8,11,14) -- measured: fx68k FAIL, TG68K FAIL
// MUTATION fixture stimulus red NMH_MUTATE=C12: the program stores $DEAD instead of $BEEF at result+2 -- measured: fx68k FAIL, TG68K FAIL
//
// prog.bin loaded, cpu_nrst_out released within the boot budget: not
// exercised, by a scope decision -- both are harness/boot outcomes settled
// before the program's first instruction executes; every mutation in this
// package changes only program or data content the CPU or chipset reads
// afterward, so none of this package's own stimulus can make either check
// fail without corrupting the build itself.
// the program's DONE mark was written: not exercised, by a scope decision
// -- every contrast in this package changes a register, a pointer or a
// colour value the chipset reads, never the CPU's own control flow, so
// none of them can stop the program from reaching its own DONE store.
// R1 (p.301 p.300 anchor): not exercised, by a scope decision -- R1
// calibrates this package's own channel/bit-order reading technique using
// the same register loads (section 3.5) every proposition already depends
// on; no stimulus in this package's own space can selectively break the
// anchor without equally breaking the propositions it calibrates. Its own
// premise (that the internal capture is HIRES) is nonetheless positively
// confirmed: NMH_MUTATE=C1 measures the same capture technique's
// pattern-line spacing at 16 visible pixels, printed above.

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

// ---- the fixed assertion call form (BRIEF.md) -----------------------------
struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// ---- addresses (ch4.i, restated) ------------------------------------------
constexpr uint32_t kResults  = 0x0C0000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint32_t kSpr0Data = 0x002000;
constexpr uint32_t kSpr1Data = 0x002400;
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
// bitplane is fetched) is $0888 -- U=(2,2,2), one of the two backgrounds
// "background-like" already covers (section 3.6 R5) -- instead of $0000.
// This is PC5's own all-nonblack calibration anchor (SPECIFICATION.md
// 3.8, section 6): with every sample of the pattern line and its
// surrounding border genuinely nonzero, the display window's own first and
// last nonzero sample (measure_window()) can only be the true hardware
// blanking edge, never a REF pixel that happens to carry U=(0,0,0).
const uint16_t kRef[16] = {
    0x0888,0x0CCC,0x0000,0x0000,0x0333,0x0FFF,0x0333,0x0333,
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

// mutate_c7: band 0's colour-17 lines (global pair index 3..10) get
// low=$FF00/high=$0000 instead of $FFFF/$0000 (SPECIFICATION.md 3.8, C7).
std::vector<std::pair<uint16_t,uint16_t>> expected_pairs(bool mutate_c7) {
    std::vector<std::pair<uint16_t,uint16_t>> pairs;
    pairs.reserve(145);
    for (int b = 0; b < 5; ++b)
        for (int off = 0; off < 29; ++off) {
            int idx = b*29+off;
            if (mutate_c7 && idx >= 3 && idx <= 10) pairs.push_back({0xFF00,0x0000});
            else pairs.push_back(line_pattern(off));
        }
    return pairs;
}

// ---- mutation ids ----------------------------------------------------------

enum class Mut { NONE, C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12 };

std::optional<Mut> parse_mut(const std::string& s) {
    if (s.empty())  return Mut::NONE;
    if (s=="C1")  return Mut::C1;
    if (s=="C2")  return Mut::C2;
    if (s=="C3")  return Mut::C3;
    if (s=="C4")  return Mut::C4;
    if (s=="C5")  return Mut::C5;
    if (s=="C6")  return Mut::C6;
    if (s=="C7")  return Mut::C7;
    if (s=="C8")  return Mut::C8;
    if (s=="C9")  return Mut::C9;
    if (s=="C10") return Mut::C10;
    if (s=="C11") return Mut::C11;
    if (s=="C12") return Mut::C12;
    return std::nullopt;
}

std::string prog_name_for(Mut m) {
    switch (m) {
        case Mut::C1:  return "prog_c1";
        case Mut::C3:  return "prog_c3";
        case Mut::C6:  return "prog_c6";
        case Mut::C12: return "prog_c12";
        default:       return "prog"; // C2, C4, C5, C7-C11: data-only contrasts
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
    pushl(0x128, (mut==Mut::C3) ? kSpr0Data : kNull2);        // SPR2PTH/L
    pushl(0x12C, kNull3);                                     // SPR3PTH/L
    pushl(0x130, kNull4);                                     // SPR4PTH/L
    pushl(0x134, kNull5);                                     // SPR5PTH/L
    pushl(0x138, kNull6);                                     // SPR6PTH/L
    pushl(0x13C, kNull7);                                     // SPR7PTH/L

    for (int i=0;i<16;++i) push(0x180+2*i, kRef[i]);          // COLOR00-15 <- REF
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
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::VideoSample> video;
    std::optional<uint16_t> deniseid, fixture;
    std::vector<uint16_t> cop_words;
    std::vector<std::pair<uint16_t,uint16_t>> pairs;
    std::map<std::string,size_t> mark;
};

Capture run_case(const std::string& dir, Mut mut) {
    Capture c;
    nmh::Config cfg; // defaults: ECS, 2.0 MB chip RAM, 68000 behaviour (SPECIFICATION.md 3.1)
    nmh::NanomigHarness h(cfg);

    poke_bitplane_buffer(h, kBplBuf, kBplWords);
    c.pairs = expected_pairs(mut==Mut::C7);
    poke_sprite_structure(h, kSpr0Data, 0x5050, 0xE100, c.pairs);
    poke_sprite_structure(h, kSpr1Data, 0x5060, 0xE100, c.pairs);
    for (uint32_t na : kNullAddrs) poke_null_structure(h, na);

    // C2 (PC2's own contrast, SPECIFICATION.md 3.8): sprite 1's structure
    // stays fully fetched -- only its own terminator word (p.108: "the
    // value 0 should be used for both words when ending the usage of a
    // sprite") is poked to a nonzero, page-incorrect value. The terminator
    // lies beyond CTL's VSTOP ($E1), so this pixel is never displayed and
    // no colour check depends on it; only PC2's own exact-value clause
    // sees the mismatch, and sprite 1's measured population (all 294
    // words) stays fully populated.
    if (mut == Mut::C2) {
        uint32_t term_word = word_addr(kSpr1Data) + 2 + (uint32_t)(2*c.pairs.size()) + 1;
        h.poke_word(term_word, 0xBAD1);
    }

    auto cb = build_copper(mut);
    c.cop_words = cb.words;
    c.mark = cb.mark;
    for (size_t i=0;i<cb.words.size();++i) h.poke_word(word_addr(kCopList)+(uint32_t)i, cb.words[i]);

    c.loaded = h.load_program(dir + "/" + prog_name_for(mut) + ".bin") > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    c.log = h.ram_log();
    c.video = h.video();
    // The fixture is read before anything else is read (BRIEF.md: "the
    // fixture proves it can deliver a known value before anything reads
    // one"), including the DENISEID readback recorded just below.
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
    bool run_length_ok=true;         // equal-length guard (p.101: "Sprites are 16 pixels wide")
    std::array<bool,15> colour_ok;   // per group: every interior pixel matches expected_u(group)
    std::array<bool,15> group_present{};
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
    bool structural_ok() const { return groups_count_ok && runs_per_line_ok && run_length_ok; }
    bool group_ok_all() const { for (bool b: colour_ok) if(!b) return false; return true; }
    bool all_ok() const { return structural_ok() && group_ok_all(); }
};

ColourCheck colour_check(const std::vector<nmh::VideoSample>& video, const FrameAnalysis& fa) {
    ColourCheck cc;
    cc.groups_count_ok = (fa.groups.size()==15);
    if (!cc.groups_count_ok) cc.detail = "expected 15 groups, found " + std::to_string(fa.groups.size());

    size_t ngroups = std::min<size_t>(fa.groups.size(), 15);
    for (size_t g=0; g<ngroups; ++g) {
        cc.group_present[g] = true;
        U want = expected_u((int)g);
        for (auto& line : fa.groups[g].lines) {
            auto runs = find_runs(video, line);
            if (runs.size() != 2) { cc.runs_per_line_ok=false; continue; }
            std::sort(runs.begin(), runs.end(), [](const Run&a, const Run&b){ return a.start<b.start; });
            for (auto& r : runs) {
                size_t len = r.end-r.start;
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

// ---- PC5's window (SPECIFICATION.md 3.8, section 6: "PC5's window") -------
//
// The window is DIWSTRT to DIWSTOP as the program sets them (p.59), less the
// one pixel next to each end of a run. Its bounds (as an offset from any
// line's own begin -- constant, since DIWSTRT/DIWSTOP are set once in step 4
// and never rewritten) are found from the pattern line above the sprite
// region: PC1's own marker, anchored independently of every sprite and
// colour register any proposition in section 4 grades (rule: an anchor never
// depends on the clause under test). Its first and last sample outside the
// video port's own blanked output (nanomig_harness.h's VideoSample, already
// blanked during hbl/vbl) bracket the display window.
struct Window { size_t lo_off=0, hi_off=0; bool ok=false; };

Window measure_window(const std::vector<nmh::VideoSample>& video, const FrameAnalysis& fa) {
    Window w;
    for (auto it = fa.pre_region.rbegin(); it != fa.pre_region.rend(); ++it) {
        if (!it->is_pattern_line) continue;
        size_t begin = it->span.begin, end = it->span.end;
        size_t lo = begin, hi = end;
        while (lo < hi && !(video[lo].r || video[lo].g || video[lo].b)) ++lo;
        while (hi > lo && !(video[hi-1].r || video[hi-1].g || video[hi-1].b)) --hi;
        if (hi <= lo) continue;
        w.lo_off = lo - begin;
        w.hi_off = hi - begin;
        w.ok = true;
        break;
    }
    return w;
}

// PC5: within the window, outside however many runs the line has (0, 1 or 2
// -- generalised so a precondition contrast that removes a run, such as C2,
// does not itself break this check), every visible pixel, apart from the one
// pixel next to each run's own ends, must show the group's expected
// background.
bool pc5_group_ok(const std::vector<nmh::VideoSample>& video, const GroupLines& gl, U want_bg,
                   const Window& w, std::string* dbg=nullptr) {
    if (!w.ok) { if (dbg && dbg->empty()) *dbg = "no window anchor (no pattern line found)"; return false; }
    for (auto& line : gl.lines) {
        size_t wlo = line.begin + w.lo_off;
        size_t whi = line.begin + w.hi_off;
        if (whi > line.end) whi = line.end;
        auto runs = find_runs(video, line);
        std::sort(runs.begin(), runs.end(), [](const Run&a,const Run&b){ return a.start<b.start; });
        size_t cur = wlo;
        for (size_t ri=0; ri<=runs.size(); ++ri) {
            size_t seg_hi = (ri<runs.size()) ? (runs[ri].start>0 ? runs[ri].start-1 : runs[ri].start) : whi;
            size_t seg_lo = std::max(cur, wlo);
            seg_hi = std::min(seg_hi, whi);
            for (size_t i=seg_lo; i<seg_hi; ++i) {
                U u = sample_u(video[i]);
                if (!(u==want_bg)) {
                    if (dbg && dbg->empty())
                        *dbg = "pixel " + std::to_string(i) + " U=(" + std::to_string(u.r) + "," + std::to_string(u.g) +
                               "," + std::to_string(u.b) + ") want=(" + std::to_string(want_bg.r) + "," +
                               std::to_string(want_bg.g) + "," + std::to_string(want_bg.b) + ") window=[" +
                               std::to_string(wlo) + "," + std::to_string(whi) + ")";
                    return false;
                }
            }
            if (ri<runs.size()) cur = runs[ri].end+1;
        }
    }
    return true;
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

size_t count_reads(const std::vector<nmh::RamAccess>& log, uint32_t wa, uint16_t value,
                    uint64_t fbegin, uint64_t fend) {
    size_t n=0;
    for (auto& a : log) {
        if (a.is_write || a.word_addr!=wa || a.data!=value) continue;
        if (a.cycle<fbegin || a.cycle>=fend) continue;
        ++n;
    }
    return n;
}

// every offset in [0,expected.size()) fetched at least once, correctly, in range.
bool all_fetched(const std::vector<nmh::RamAccess>& log, uint32_t base_word,
                  const std::vector<uint16_t>& expected, uint64_t fbegin, uint64_t fend) {
    for (size_t off=0; off<expected.size(); ++off)
        if (!first_read_cycle(log, base_word+(uint32_t)off, expected[off], fbegin, fend)) return false;
    return true;
}

// PC2 check 1 (SPECIFICATION.md 3.8): every offset of a structure fetched
// exactly once, at its own address, with the prepared value, in exactly
// the offset order Table 4-1 lays the structure out -- and no other read
// of any word inside the structure's own address range. Reads that share
// one bus cycle stretched by wait states collapse to one logical access
// first (nmh::coalesce), so a multi-cycle fetch is never counted twice.
bool exact_ordered_fetch(const std::vector<nmh::RamAccess>& log, uint32_t base_word,
                          const std::vector<uint16_t>& expected, uint64_t fbegin, uint64_t fend) {
    std::vector<nmh::RamAccess> reads;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.cycle<fbegin || a.cycle>=fend) continue;
        if (a.word_addr<base_word || a.word_addr>=base_word+(uint32_t)expected.size()) continue;
        reads.push_back(a);
    }
    std::sort(reads.begin(), reads.end(), [](const nmh::RamAccess&a, const nmh::RamAccess&b){ return a.cycle<b.cycle; });
    auto coalesced = nmh::coalesce(reads);
    if (coalesced.size() != expected.size()) return false;
    for (size_t i=0;i<expected.size();++i)
        if (coalesced[i].word_addr!=base_word+(uint32_t)i || coalesced[i].data!=expected[i]) return false;
    return true;
}

struct Range { uint64_t lo=kNoCycle, hi=0; bool any=false; bool all=false; };

// PC4 check 1 (SPECIFICATION.md 3.8): every marked Copper word in
// [off_lo,off_hi) must be fetched (r.all), not merely any one of them
// (the previous r.any let one fetched word stand in for an entire reload).
Range chunk_range(const std::vector<nmh::RamAccess>& log, uint32_t base_word,
                   const std::vector<uint16_t>& expected, size_t off_lo, size_t off_hi,
                   uint64_t fbegin, uint64_t fend) {
    Range r;
    bool complete = off_hi > off_lo;
    for (size_t off=off_lo; off<off_hi; ++off) {
        auto c = first_read_cycle(log, base_word+(uint32_t)off, expected[off], fbegin, fend);
        if (!c) { complete = false; continue; }
        r.any = true;
        r.lo = std::min(r.lo, *c);
        r.hi = std::max(r.hi, *c);
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

    auto top = chunk_range(c.log, cop0, c.cop_words, 0, c.mark.at("top_end"), f.begin, f.end);
    auto bpl = first_bitplane_cycle(c.log, f.begin, f.end);
    bool a_ok = top.all && bpl && top.hi < *bpl;

    auto b0 = chunk_range(c.log, cop0, c.cop_words, c.mark.at("b0_start"), c.mark.at("b0_end"), f.begin, f.end);
    auto line50 = sprite_line_low_cycle(c.log, kSpr0Data, c.pairs, 0x50, f.begin, f.end);
    bool b_ok = b0.all && line50 && b0.hi < *line50;

    bool c_ok = true;
    // Band 4's chunk starts at "b4_start", not "band4_start": that range
    // also covers the COLOR00-COLOR15<-B4 MOVEs, which band 4 alone carries
    // between the same two sprite-line fetches (SPECIFICATION.md 3.8, PC4c).
    static const char* starts[5] = {nullptr,"band1_start","band2_start","band3_start","b4_start"};
    static const char* ends[5]   = {nullptr,"band1_end","band2_end","band3_end","band4_end"};
    for (int b=1;b<=4;++b) {
        auto chunk = chunk_range(c.log, cop0, c.cop_words, c.mark.at(starts[b]), c.mark.at(ends[b]), f.begin, f.end);
        uint32_t s_b = 0x50 + 29*b;
        auto lo = sprite_line_low_cycle(c.log, kSpr0Data, c.pairs, s_b,   f.begin, f.end);
        auto hi = sprite_line_low_cycle(c.log, kSpr0Data, c.pairs, s_b+2, f.begin, f.end);
        if (!(chunk.all && lo && hi && *lo < chunk.lo && chunk.hi < *hi)) c_ok = false;
    }
    return a_ok && b_ok && c_ok;
}

// ---- R1/R2's own HIRES-spacing measurement (SPECIFICATION.md 3.6, 3.8) ----
// Shared by the normal run's internal R1 anchor and the C1 mutation replay:
// both need to establish -- not merely assume -- that a capture taken with
// BPLCON0 = $9000 actually is HIRES, by measuring the pattern line's own
// spacing (p.300: SuperHires's 35ns rate is "twice the horizontal
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

// ---- assertion labels shared between the normal run and its mutations -----
// (BRIEF.md: a mutation's record names the assertion-label its own expect()
// uses "exactly". C7-C11 replay these same strings through the same
// computation the normal run uses, expecting the opposite polarity.)

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
const char* kPc5Label =
    "PC5 p.302 p.59: band 4's reload of COLOR00-COLOR15 took effect (background U=(10,10,10) on "
    "groups 12-14, U=(00,00,00) on groups 0-11, within the display window, every graded frame)";
// R5 (SPECIFICATION.md 3.6, 3.8): the group/run structure itself, graded
// once for the whole frame, separately from any single group's colour
// (P1-P4, which grade only their own groups' colour_ok). A structural
// defect (a missing group, a stray run, an unequal run length) must not by
// itself make an unrelated colour proposition go red.
const char* kR5Label =
    "R5 p.101 p.302: the sprite region has exactly 15 groups, every group line has exactly two runs, "
    "and every run of every graded group line is the same length as every other (p.101: \"Sprites are "
    "16 pixels wide\")";

// ---- the colour check, evaluated across every graded frame -----------------
// Shared by the normal run and by the mutations that replay P1-P4 as
// "stimulus red" (C7-C11, SPECIFICATION.md 3.8 and section 4).

struct Verdict {
    bool any_frame=false;
    bool p1=true, p2=true, p3=true, p4=true, pc5=true, r5=true;
    int run_len_lo=-1, run_len_hi=-1; // R2's own SuperHires run-width measurement (ungraded)
    std::array<int,15> r_lo, r_hi, g_lo, g_hi, b_lo, b_hi;
};

Verdict evaluate(const Capture& c, const GradedRun& g) {
    Verdict v;
    v.r_lo.fill(3); v.g_lo.fill(3); v.b_lo.fill(3);
    v.r_hi.fill(0); v.g_hi.fill(0); v.b_hi.fill(0);
    for (auto& f : g.frames) {
        FrameAnalysis fa = analyze_frame(c.video, f);
        if (!fa.ok) { v.p1=v.p2=v.p3=v.p4=v.pc5=v.r5=false; continue; }
        v.any_frame = true;
        ColourCheck cc = colour_check(c.video, fa);
        // R5 grades the structure (group count, run count, run length) on
        // its own; P1-P4 grade only their own groups' colour_ok, so a
        // structural defect in one group never makes an unrelated colour
        // proposition go red (SPECIFICATION.md 3.6 R5, 3.8).
        if (!cc.structural_ok()) v.r5 = false;
        if (cc.run_len_min>=0) {
            v.run_len_lo = (v.run_len_lo<0) ? cc.run_len_min : std::min(v.run_len_lo, cc.run_len_min);
            v.run_len_hi = std::max(v.run_len_hi, cc.run_len_max);
        }
        for (int g0=0; g0<15; ++g0)          if (!cc.colour_ok[g0]) v.p1=false;
        for (int g0 : {0,3,6,9,12})          if (!cc.colour_ok[g0]) v.p2=false;
        for (int g0 : {1,4,7,10,13})         if (!cc.colour_ok[g0]) v.p3=false;
        for (int g0 : {2,5,8,11,14})         if (!cc.colour_ok[g0]) v.p4=false;
        for (int g0=0; g0<15; ++g0) {
            v.r_lo[g0]=std::min(v.r_lo[g0], cc.r_lo[g0]); v.r_hi[g0]=std::max(v.r_hi[g0], cc.r_hi[g0]);
            v.g_lo[g0]=std::min(v.g_lo[g0], cc.g_lo[g0]); v.g_hi[g0]=std::max(v.g_hi[g0], cc.g_hi[g0]);
            v.b_lo[g0]=std::min(v.b_lo[g0], cc.b_lo[g0]); v.b_hi[g0]=std::max(v.b_hi[g0], cc.b_hi[g0]);
        }

        Window w = measure_window(c.video, fa);
        if (fa.groups.size()==15 && w.ok) {
            for (int gi=0; gi<15; ++gi)
                if (!pc5_group_ok(c.video, fa.groups[gi], expected_background(gi), w)) v.pc5=false;
        } else v.pc5=false;
    }
    if (!v.any_frame) v.p1=v.p2=v.p3=v.p4=v.pc5=false;
    return v;
}

// ---- the normal run ---------------------------------------------------------

void run_normal(const std::string& dir, Checks& checks) {
    Capture c = run_case(dir, Mut::NONE);

    checks.expect(c.loaded, "prog.bin loaded");
    checks.expect(c.released, "cpu_nrst_out released within the boot budget");
    checks.expect(c.done.has_value(), "the program's DONE mark was written");
    checks.expect(c.fixture.has_value() && *c.fixture==0xBEEF,
                  "fixture: the program-stored constant $BEEF was read back before anything else is graded");

    if (c.deniseid)
        checks.note("DENISEID = " + std::to_string(*c.deniseid) + " (p.299, recorded only)");

    if (!c.loaded || !c.done) return;

    GradedRun g = find_graded(c);
    checks.expect(g.frames.size()>=2,
                  "PC6 p.25 p.30: at least two graded frames (R3) -- " + std::to_string(g.restarts.size()) +
                  " Copper restart(s) observed, " + std::to_string(g.frames.size()) + " graded frame(s)");
    if (g.frames.empty()) return;

    // PC1: a pattern line above the sprite region, spacing 8 visible pixels, every frame.
    bool pc1 = true;
    for (auto& f : g.frames) {
        FrameAnalysis fa = analyze_frame(c.video, f);
        if (!fa.ok) { pc1=false; continue; }
        int npat=0; bool spacing_ok=true;
        for (auto& la : fa.pre_region) if (la.is_pattern_line) { ++npat; if (la.pattern_spacing!=8) spacing_ok=false; }
        if (npat<1 || !spacing_ok) pc1=false;
    }
    checks.expect(pc1, "PC1 p.300: SuperHires is in effect (every graded frame has a pattern line above the "
                       "sprite region, spacing 8 visible pixels)");

    // PC2: sprites 0 and 1 fetch exactly the prepared data, every frame.
    bool pc2 = true;
    std::vector<uint16_t> spr0_expected, spr1_expected;
    auto push_struct = [](std::vector<uint16_t>& v, uint16_t pos, uint16_t ctl,
                           const std::vector<std::pair<uint16_t,uint16_t>>& pairs) {
        v.push_back(pos); v.push_back(ctl);
        for (auto& p : pairs) { v.push_back(p.first); v.push_back(p.second); }
        v.push_back(0); v.push_back(0);
    };
    push_struct(spr0_expected, 0x5050, 0xE100, c.pairs);
    push_struct(spr1_expected, 0x5060, 0xE100, c.pairs);
    for (auto& f : g.frames) {
        if (!exact_ordered_fetch(c.log, word_addr(kSpr0Data), spr0_expected, f.begin, f.end)) pc2=false;
        if (!exact_ordered_fetch(c.log, word_addr(kSpr1Data), spr1_expected, f.begin, f.end)) pc2=false;
    }
    checks.expect(pc2, "PC2 p.107 p.108: sprites 0 and 1 fetch POS, CTL and all 145 data pairs ($50-$E0) with "
                       "the prepared values plus the terminator, exactly once each, each at its own fixed "
                       "offset in Table 4-1's own order, with no other read of either structure's own address "
                       "range, every graded frame");

    // PC3: sprites 2-7 idle. Each of the six is given its own safe-null
    // structure (ch4.i), so each of the six must show the exact same
    // two-word fetch PC2 requires of a real sprite; sprite 0 and sprite 1's
    // own structures must not be over-fetched (the signature a second
    // sprite channel reading one of them, instead of its own null, would
    // leave -- see C3 below).
    bool pc3 = true;
    for (auto& f : g.frames) {
        for (uint32_t na : kNullAddrs)
            if (!exact_ordered_fetch(c.log, word_addr(na), {0,0}, f.begin, f.end)) pc3=false;
        for (size_t off=0; off<spr0_expected.size(); ++off)
            if (count_reads(c.log, word_addr(kSpr0Data)+(uint32_t)off, spr0_expected[off], f.begin, f.end) > 1) pc3=false;
        for (size_t off=0; off<spr1_expected.size(); ++off)
            if (count_reads(c.log, word_addr(kSpr1Data)+(uint32_t)off, spr1_expected[off], f.begin, f.end) > 1) pc3=false;
    }
    checks.expect(pc3, "PC3 p.114 p.115: sprites 2 to 7 are idle (each of their six independently identifiable "
                       "null structures' two words is read exactly once, and no word of sprite 0's or sprite "
                       "1's own structure is read more than once)");

    // PC4: Copper colour loads fall between the displayed sprite pixels.
    bool pc4 = true;
    for (auto& f : g.passes) if (!pc4_frame_ok(c, f)) pc4=false;
    checks.expect(pc4, "PC4 p.126 p.23: the Copper's colour-register MOVEs fall between sprite 0's own pair "
                       "fetches on the bus (top-of-list before the first bitplane fetch, the B0 reload before "
                       "line $50, each band's reload between its own line and two lines later)");

    // R1/R2 anchor: capture C1 (HIRES) inside the normal run and assert that
    // COLOR17-COLOR19, read as ordinary registers, already show the section
    // 3.5 U values, before P1-P4 are graded (SPECIFICATION.md 3.6 R1).
    Capture c1 = run_case(dir, Mut::C1);
    bool anchor_ok = c1.loaded && c1.done.has_value();
    SpacingMeasure c1_spacing;
    if (anchor_ok) {
        GradedRun g1 = find_graded(c1);
        anchor_ok = !g1.frames.empty();
        for (auto& f : g1.frames) {
            FrameAnalysis fa = analyze_frame(c1.video, f);
            if (!fa.ok || !colour_check(c1.video, fa).all_ok()) anchor_ok = false;
        }
        // Establish the premise that this capture is actually HIRES (p.300)
        // before trusting its colour check as the R1 anchor.
        c1_spacing = measure_pattern_spacing(c1.video, g1.frames);
        if (!c1_spacing.any || c1_spacing.min_spacing!=16 || c1_spacing.max_spacing!=16) anchor_ok = false;
    }
    checks.expect(anchor_ok, "R1 p.301 p.300 (anchor): with HIRES (SHRES=0) instead of SuperHires -- "
                             "established by a measured pattern-line spacing of 16 visible pixels, twice the "
                             "SuperHires 8-pixel spacing -- COLOR17-COLOR19 read as ordinary 0x0RGB registers "
                             "already show the section 3.5 U values for every group, establishing the channel "
                             "and bit order before P1-P4 are graded");
    if (c1_spacing.any)
        checks.note("R1/R2 p.300 anchor: the internal HIRES capture's pattern-line spacing measures " +
                    std::to_string(c1_spacing.min_spacing) + "-" + std::to_string(c1_spacing.max_spacing) +
                    " visible pixels");

    // colour check (section 3.7), shared by P1-P4, plus PC5 (band 4's background).
    Verdict v = evaluate(c, g);

    checks.expect(v.pc5, kPc5Label);
    checks.expect(v.r5, kR5Label);
    checks.expect(v.p1, kP1Label);
    checks.expect(v.p2, kP2Label);
    checks.expect(v.p3, kP3Label);
    checks.expect(v.p4, kP4Label);

    if (v.run_len_hi>=0)
        checks.note("R2 p.101 (ungraded): the SuperHires sprite run width measures " +
                    std::to_string(v.run_len_lo) + "-" + std::to_string(v.run_len_hi) + " visible pixels");

    checks.record("CH4-appC-color.c1 (p.302): P1 loads all sixteen upper registers (COLOR16-COLOR31)"
                  "together and observes only colours 17-19 (COLOR17-COLOR19), so it cannot show that all "
                  "sixteen registers participate in the SuperHires scheme; the per-register decode is graded "
                  "at the unit level, in hrm/ch3-shres-sprite-colour-encoding. P1 keeps grading what it "
                  "observes: that reloading COLOR16-COLOR31 changes the displayed colours as the scheme "
                  "predicts, and reloading COLOR00-COLOR15 does not.");

    // Bits 1-0 of every graded pixel, per group (section 3.7): recorded, never graded.
    for (int gi=0; gi<15; ++gi) {
        checks.note("group " + std::to_string(gi) + " bits1-0 (section 3.7, ungraded): R=[" +
                    std::to_string(v.r_lo[gi]) + "," + std::to_string(v.r_hi[gi]) + "] G=[" +
                    std::to_string(v.g_lo[gi]) + "," + std::to_string(v.g_hi[gi]) + "] B=[" +
                    std::to_string(v.b_lo[gi]) + "," + std::to_string(v.b_hi[gi]) + "]");
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

// ---- contrasts (C1-C11), one at a time under NMH_MUTATE -------------------

void run_contrast(const std::string& dir, Mut mut, Checks& checks) {
    Capture c = run_case(dir, mut);
    checks.expect(c.loaded, "mutant program loaded");
    checks.expect(c.done.has_value(), "the mutant program's DONE mark was written");
    if (!c.loaded || !c.done) return;

    GradedRun g = find_graded(c);

    if (mut == Mut::C6) {
        checks.expect(g.frames.size()>=2,
                      "PC6 p.25 p.30: at least two graded frames (R3) -- " + std::to_string(g.restarts.size()) +
                      " Copper restart(s) observed, " + std::to_string(g.frames.size()) + " graded frame(s)");
        return;
    }
    if (g.frames.empty()) {
        checks.record("NMH_MUTATE=" + std::string("(this run)") + ": no graded frame was found, the mutation's "
                       "own check cannot be evaluated");
        return;
    }

    if (mut == Mut::C1) {
        bool pc1 = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok) { pc1=false; continue; }
            int npat=0; bool spacing_ok=true;
            for (auto& la : fa.pre_region) if (la.is_pattern_line) { ++npat; if (la.pattern_spacing!=8) spacing_ok=false; }
            if (npat<1 || !spacing_ok) pc1=false;
        }
        checks.expect(pc1, "PC1 p.300: SuperHires is in effect (every graded frame has a pattern line above the "
                           "sprite region, spacing 8 visible pixels)");

        // The page-300 HIRES spacing itself (R1/R2's own anchor,
        // SPECIFICATION.md 3.6 R2, 3.8): the same capture's pattern line
        // must measure 16 visible pixels, twice the SuperHires spacing PC1
        // grades above. This is a genuine assertion, not only PC1's own
        // negative ("spacing is not 8").
        SpacingMeasure sp = measure_pattern_spacing(c.video, g.frames);
        checks.expect(sp.any && sp.min_spacing==16 && sp.max_spacing==16,
                      "R1/R2 p.300: in this HIRES capture, the pattern line's spacing measures 16 visible "
                      "pixels (twice the SuperHires 8-pixel spacing PC1 grades)");
        if (sp.any)
            checks.note("R1/R2 p.300: measured pattern-line spacing " + std::to_string(sp.min_spacing) + "-" +
                        std::to_string(sp.max_spacing) + " visible pixels");

        bool colour_still_ok = true;
        int run_lo=-1, run_hi=-1;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok) { colour_still_ok=false; continue; }
            ColourCheck cc = colour_check(c.video, fa);
            if (!cc.all_ok()) colour_still_ok=false;
            if (cc.run_len_min>=0) {
                run_lo = (run_lo<0) ? cc.run_len_min : std::min(run_lo, cc.run_len_min);
                run_hi = std::max(run_hi, cc.run_len_max);
            }
        }
        checks.note(std::string("R1/R2 anchor: with HIRES (SHRES=0), the colour check itself still reads ") +
                    (colour_still_ok ? "PASS" : "FAIL") +
                    " -- COLOR17-19 read as ordinary 0x0RGB registers already carry the key's bits (SPECIFICATION.md 1.2, 3.6 R1)");
        if (run_hi>=0)
            checks.note("R2 p.101 (anchor, ungraded): in this HIRES capture, sprite runs measure " +
                        std::to_string(run_lo) + "-" + std::to_string(run_hi) + " visible pixels");
        return;
    }
    if (mut == Mut::C2) {
        // Re-run PC2's own check (SPECIFICATION.md 3.8): sprite 1's own
        // terminator word was poked to a page-incorrect nonzero value
        // (run_case()), so the same exact-value check that reads PASS in
        // the normal run reads FAIL here, through the value clause alone --
        // the measured population (both structures, all 294 words each)
        // stays fully live.
        std::vector<uint16_t> spr0_expected, spr1_expected;
        auto push_struct = [](std::vector<uint16_t>& v, uint16_t pos, uint16_t ctl,
                               const std::vector<std::pair<uint16_t,uint16_t>>& pairs) {
            v.push_back(pos); v.push_back(ctl);
            for (auto& p : pairs) { v.push_back(p.first); v.push_back(p.second); }
            v.push_back(0); v.push_back(0);
        };
        push_struct(spr0_expected, 0x5050, 0xE100, c.pairs);
        push_struct(spr1_expected, 0x5060, 0xE100, c.pairs);
        bool pc2_spr0 = true, pc2_spr1 = true;
        for (auto& f : g.frames) {
            if (!exact_ordered_fetch(c.log, word_addr(kSpr0Data), spr0_expected, f.begin, f.end)) pc2_spr0=false;
            if (!exact_ordered_fetch(c.log, word_addr(kSpr1Data), spr1_expected, f.begin, f.end)) pc2_spr1=false;
        }
        checks.expect(pc2_spr0 && pc2_spr1,
                      "PC2 p.107 p.108: sprites 0 and 1 fetch POS, CTL and all 145 data pairs ($50-$E0) with "
                      "the prepared values plus the terminator, exactly once each, each at its own fixed "
                      "offset in Table 4-1's own order, with no other read of either structure's own address "
                      "range, every graded frame");
        checks.note(std::string("sprite 0's own fetch stays ") + (pc2_spr0 ? "PASS" : "FAIL") +
                    ", sprite 1's own fetch is " + (pc2_spr1 ? "PASS" : "FAIL") +
                    " (its poked terminator word no longer matches the expected $0000)");
        // PC5 must not depend on this precondition (SPECIFICATION.md 3.8):
        // the wrong word sits beyond sprite 1's own VSTOP, so it is never
        // displayed, and PC5's own window-based background check is
        // unaffected.
        Verdict v = evaluate(c, g);
        checks.note(std::string("PC5 stays ") + (v.pc5 ? "PASS" : "FAIL") +
                    " under this mutation (the wrong word is never displayed)");
        return;
    }
    if (mut == Mut::C3) {
        // Re-run PC3's own check (SPECIFICATION.md 3.8): under this mutation
        // sprite 2's channel fetches sprite 0's structure instead of its own
        // null2 structure, so null2's own two words go unread and sprite 0's
        // own words are read more than once per graded frame; the same
        // check that reads PASS in the normal run reads FAIL here.
        std::vector<uint16_t> spr0_expected;
        auto push_struct = [](std::vector<uint16_t>& v, uint16_t pos, uint16_t ctl,
                               const std::vector<std::pair<uint16_t,uint16_t>>& pairs) {
            v.push_back(pos); v.push_back(ctl);
            for (auto& p : pairs) { v.push_back(p.first); v.push_back(p.second); }
            v.push_back(0); v.push_back(0);
        };
        push_struct(spr0_expected, 0x5050, 0xE100, c.pairs);
        bool pc3 = true;
        for (auto& f : g.frames) {
            for (uint32_t na : kNullAddrs)
                if (!exact_ordered_fetch(c.log, word_addr(na), {0,0}, f.begin, f.end)) pc3=false;
            for (size_t off=0; off<spr0_expected.size(); ++off)
                if (count_reads(c.log, word_addr(kSpr0Data)+(uint32_t)off, spr0_expected[off], f.begin, f.end) > 1) pc3=false;
        }
        checks.expect(pc3, "PC3 p.114 p.115: sprites 2 to 7 are idle (each of their six independently "
                           "identifiable null structures' two words is read exactly once, and no word of "
                           "sprite 0's or sprite 1's own structure is read more than once)");
        Verdict v = evaluate(c, g);
        checks.note(std::string("the displayed colour-match is ") + (v.p1 ? "unchanged" : "changed") +
                    " -- sprite 0 has priority over sprite 2 (p.115)");
        return;
    }
    if (mut == Mut::C4) {
        // Re-run PC4's own check: band 2's WAIT now lands inside sprite 0's
        // own pair fetches, so the same check that reads PASS in the normal
        // run reads FAIL here.
        bool pc4 = true;
        for (auto& f : g.passes) if (!pc4_frame_ok(c, f)) pc4 = false;
        checks.expect(pc4, "PC4 p.126 p.23: the Copper's colour-register MOVEs fall between sprite 0's own pair "
                           "fetches on the bus (top-of-list before the first bitplane fetch, the B0 reload before "
                           "line $50, each band's reload between its own line and two lines later)");
        return;
    }
    if (mut == Mut::C5) {
        // Re-run PC5's own check: leaving out the COLOR00-COLOR15<-B4 MOVEs
        // means band 4's background never reaches (10,10,10), so the same
        // check that reads PASS in the normal run reads FAIL here.
        Verdict v = evaluate(c, g);
        checks.expect(v.pc5, kPc5Label);
        checks.note(std::string("the colour-match clause of the colour check itself still reads ") +
                    (v.p1 ? "PASS" : "FAIL"));
        return;
    }
    if (mut == Mut::C7) {
        // R5's own contrast (SPECIFICATION.md 3.8, not P2's: p.101 permits
        // any shape within the 16-pixel width, so narrowing group 0's own
        // runs is not a page-opposite case for P2's colour clause; C9
        // already tests that). Group 0's runs narrow to 8 samples while
        // every other group's stay at their own (equal) length, so the
        // equal-length guard inside R5 goes red through that clause alone,
        // replayed with the SAME expect() the normal run uses. P1-P4 do not
        // depend on R5 (each grades only its own groups' colour), so they
        // stay green here; printed to show that the mutation is scoped to
        // R5 alone.
        Verdict v = evaluate(c, g);
        checks.expect(v.r5, kR5Label);
        checks.expect(v.p1, kP1Label);
        checks.expect(v.p2, kP2Label);
        return;
    }
    if (mut == Mut::C8) {
        // Band 4's reload uses band 0's values instead of band 3's, so
        // groups 12-14 (all three of band 4's colours) stop matching band
        // 3. Group 12 belongs to P2's own group set, group 13 to P3's and
        // group 14 to P4's, and P1 grades all fifteen groups, so this
        // mutation prints red on P1, P2, P3 and P4 alike -- every affected
        // line, not only P1's own. PC5 (the lower-register background) does
        // not depend on COLOR16-COLOR31, so it stays green, and R5 (the
        // group/run structure) is unaffected too.
        Verdict v = evaluate(c, g);
        checks.expect(v.p1, kP1Label);
        checks.expect(v.p2, kP2Label);
        checks.expect(v.p3, kP3Label);
        checks.expect(v.p4, kP4Label);
        checks.expect(v.pc5, kPc5Label);
        checks.expect(v.r5, kR5Label);
        return;
    }
    if (mut == Mut::C9) {
        // Band 1's G/H swap breaks group 3 alone. Group 3 belongs to both
        // P1's group set (all fifteen) and P2's own (0,3,6,9,12), so this
        // mutation prints red on both, not on P2 alone.
        Verdict v = evaluate(c, g);
        checks.expect(v.p1, kP1Label);
        checks.expect(v.p2, kP2Label);
        return;
    }
    if (mut == Mut::C10) {
        // Band 2's O/P swap breaks group 7 alone, which belongs to both
        // P1's group set and P3's own (1,4,7,10,13).
        Verdict v = evaluate(c, g);
        checks.expect(v.p1, kP1Label);
        checks.expect(v.p3, kP3Label);
        return;
    }
    if (mut == Mut::C11) {
        // Band 2's W/X swap breaks group 8 alone, which belongs to both
        // P1's group set and P4's own (2,5,8,11,14).
        Verdict v = evaluate(c, g);
        checks.expect(v.p1, kP1Label);
        checks.expect(v.p4, kP4Label);
        return;
    }
    if (mut == Mut::C12) {
        // The fixture check's own contrast (SPECIFICATION.md 3.8): the
        // program stores $DEAD instead of $BEEF, so the same expect() the
        // normal run uses goes red on the value clause alone.
        checks.expect(c.fixture.has_value() && *c.fixture==0xBEEF,
                      "fixture: the program-stored constant $BEEF was read back before anything else is "
                      "graded");
        return;
    }
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
    if (!mut) {
        checks.record("unrecognised NMH_MUTATE value: " + mutate);
    } else if (*mut == Mut::NONE) {
        run_normal(dir, checks);
    } else {
        run_contrast(dir, *mut, checks);
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed==0 ? 0 : 1;
}
