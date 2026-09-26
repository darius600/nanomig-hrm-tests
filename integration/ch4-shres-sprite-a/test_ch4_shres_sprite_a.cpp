// test_ch4_shres_sprite_a.cpp -- ch4-shres-sprite-a. See SPECIFICATION.md
// for every assertion's proposition, program, observation and expected
// value. This file only implements what SPECIFICATION.md already
// committed to. The sprite/bitplane/Copper-list *data* is poked directly
// into chip RAM by this file (the same technique the ch6 packages use for
// their own DMA data areas); prog.s and its mutation wrappers only perform
// the CPU register writes of section 3.3.
//
// NMH_MUTATE selects a mutation in place of the normal run, one at a time;
// none is exercised by a plain `make test`.
//
// MUTATION PC1 stimulus red NMH_MUTATE=C1: BPLCON0=$9000 (HIRES, SHRES=0) instead of $1040 -- pattern spacing becomes 16 visible pixels instead of 8
// MUTATION PC2 stimulus red NMH_MUTATE=C2: SPR1PTH (step 5 and the Copper list) points at the null structure instead of sprite 1's data
// MUTATION PC3 stimulus red NMH_MUTATE=C3: SPR2PTH (step 5 and the Copper list) points at sprite 0's data instead of the null structure
// MUTATION PC4 stimulus red NMH_MUTATE=C4: band 2's WAIT changed from $8B41 to $9141 (the fifth colour-17 line of band 2)
// MUTATION PC5 stimulus red NMH_MUTATE=C5: the COLOR00-COLOR15 <- B4 MOVEs after WAIT $C541 are left out
// MUTATION PC6 stimulus red NMH_MUTATE=C6: DONE right after step 7, no four-frame wait, so there is no graded frame
//
// P2, P3 and P4 already read FAIL in the normal run, through the run-length
// clause of section 3.7 alone (see SPECIFICATION.md section 6): every
// SuperHires sprite run measures half its expected width on this RTL. Their
// own natural check cannot show C7-C11's own, additional effect on top of
// that pre-existing redness, so C7-C11 isolate the colour-match clause (or,
// for C7, the run-length clause on one group alone) with a live, unmutated
// group from the same run as its comparator -- the same technique ch6 uses
// for an assertion that is already red (see ch6-blitter-cpu-sharing's own
// A8/A8-SHARE). Each prints PASS when the isolated clause shows exactly the
// row's own predicted difference.
// MUTATION P2 stimulus isolated (run-length clause) NMH_MUTATE=C7: colour-17 lines of band 0 ($53-$5A), both sprites, get low=$FF00/high=$0000 instead of $FFFF/$0000 -- group 0's own run narrows relative to group 3 (same colour, band 1, live and unmutated)
// MUTATION P1 stimulus isolated (colour-match clause) NMH_MUTATE=C8: band 4's COLOR16-COLOR31 reload (after WAIT $C541) uses band 0's values instead of band 3's -- groups 12-14 stop matching band 3 (PC5 stays green: it does not depend on COLOR16-COLOR31)
// MUTATION P2 stimulus isolated (colour-match clause) NMH_MUTATE=C9: band 1's COLOR16-COLOR31 reload swaps G and H -- group 3 alone stops matching, groups 0/6/9/12 (same colour, other bands) still match
// MUTATION P3 stimulus isolated (colour-match clause) NMH_MUTATE=C10: band 2's COLOR16-COLOR31 reload swaps O and P -- group 7 alone stops matching, groups 1/4/10/13 still match
// MUTATION P4 stimulus isolated (colour-match clause) NMH_MUTATE=C11: band 2's COLOR16-COLOR31 reload swaps W and X -- group 8 alone stops matching, groups 2/5/11/14 still match

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

constexpr uint32_t kSpr0Data = 0x082000;
constexpr uint32_t kSpr1Data = 0x082400;
constexpr uint32_t kNullData = 0x082800;
constexpr uint32_t kBplBuf   = 0x090000;
constexpr uint32_t kCopList  = 0x0A0000;
constexpr size_t   kBplWords = 32768;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 10'000'000;

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

// ---- section 3.5 tables, transcribed literally from SPECIFICATION.md -----

const uint16_t kRef[16] = {
    0x0000,0x0CCC,0x0000,0x0000,0x0333,0x0FFF,0x0333,0x0333,
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

enum class Mut { NONE, C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11 };

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
    return std::nullopt;
}

std::string prog_name_for(Mut m) {
    switch (m) {
        case Mut::C1: return "prog_c1";
        case Mut::C2: return "prog_c2";
        case Mut::C3: return "prog_c3";
        case Mut::C6: return "prog_c6";
        default:      return "prog";
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
    pushl(0x124, (mut==Mut::C2) ? kNullData : kSpr1Data);     // SPR1PTH/L
    pushl(0x128, (mut==Mut::C3) ? kSpr0Data : kNullData);     // SPR2PTH/L
    pushl(0x12C, kNullData);
    pushl(0x130, kNullData);
    pushl(0x134, kNullData);
    pushl(0x138, kNullData);
    pushl(0x13C, kNullData);

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
    poke_null_structure(h, kNullData);

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
    c.deniseid = h.peek_logical(kResults + 0);
    c.fixture  = h.peek_logical(kResults + 2);
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
    bool run_length_ok=true;
    std::array<bool,15> colour_ok; // per group: every interior pixel matches expected_u(group)
    std::array<bool,15> group_present{};
    std::array<int,15>  run_len_min; // per group: shortest run (of either sprite) observed, samples
    std::string detail;
    ColourCheck() { colour_ok.fill(true); run_len_min.fill(-1); }
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
                if (len<31 || len>33) cc.run_length_ok=false;
                if (cc.run_len_min[g]<0 || (int)len<cc.run_len_min[g]) cc.run_len_min[g]=(int)len;
                for (size_t i=r.start+1; i+1<r.end; ++i) {
                    U u = sample_u(video[i]);
                    if (!(u==want)) cc.colour_ok[g]=false;
                }
            }
        }
    }
    return cc;
}

// PC5: background pixels outside the two runs (excluding the pixel next to
// each run's ends), per group, must show the group's expected background.
// Scans only the interior of the GAP between the two runs (excluding the
// pixel next to each run's own ends, section 3.7's own exclusion). Outside
// the two runs, but still on the same line, is hblank-adjacent territory the
// design forces to (0,0,0) regardless of any colour register (nanomig.v's
// blanking override); that would misread as "background" for groups 0-11
// (whose own expected background happens to be (0,0,0)) but wrongly so for
// groups 12-14 (expected (2,2,2)). The gap strictly between two confirmed
// visible runs is not blanking by construction, so it is the only region
// this check reads.
bool pc5_group_ok(const std::vector<nmh::VideoSample>& video, const GroupLines& gl, U want_bg, std::string* dbg=nullptr) {
    for (auto& line : gl.lines) {
        auto runs = find_runs(video, line);
        if (runs.size()!=2) { if (dbg && dbg->empty()) *dbg = "runs!=2 (" + std::to_string(runs.size()) + ")"; return false; }
        std::sort(runs.begin(), runs.end(), [](const Run&a,const Run&b){ return a.start<b.start; });
        size_t gap_lo = runs[0].end+1, gap_hi = runs[1].start>0? runs[1].start-1 : runs[1].start;
        for (size_t i=gap_lo;i<gap_hi;++i) {
            U u = sample_u(video[i]);
            if (!(u==want_bg)) {
                if (dbg && dbg->empty())
                    *dbg = "pixel " + std::to_string(i) + " U=(" + std::to_string(u.r) + "," + std::to_string(u.g) +
                           "," + std::to_string(u.b) + ") want=(" + std::to_string(want_bg.r) + "," +
                           std::to_string(want_bg.g) + "," + std::to_string(want_bg.b) + ") gap=[" +
                           std::to_string(gap_lo) + "," + std::to_string(gap_hi) + ") run0=[" +
                           std::to_string(runs[0].start) + "," + std::to_string(runs[0].end) + ") run1=[" +
                           std::to_string(runs[1].start) + "," + std::to_string(runs[1].end) + ")";
                return false;
            }
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

struct Range { uint64_t lo=kNoCycle, hi=0; bool any=false; };

Range chunk_range(const std::vector<nmh::RamAccess>& log, uint32_t base_word,
                   const std::vector<uint16_t>& expected, size_t off_lo, size_t off_hi,
                   uint64_t fbegin, uint64_t fend) {
    Range r;
    for (size_t off=off_lo; off<off_hi; ++off) {
        auto c = first_read_cycle(log, base_word+(uint32_t)off, expected[off], fbegin, fend);
        if (!c) continue;
        r.any = true;
        r.lo = std::min(r.lo, *c);
        r.hi = std::max(r.hi, *c);
    }
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
    bool a_ok = top.any && bpl && top.hi < *bpl;

    auto b0 = chunk_range(c.log, cop0, c.cop_words, c.mark.at("b0_start"), c.mark.at("b0_end"), f.begin, f.end);
    auto line50 = sprite_line_low_cycle(c.log, kSpr0Data, c.pairs, 0x50, f.begin, f.end);
    bool b_ok = b0.any && line50 && b0.hi < *line50;

    bool c_ok = true;
    static const char* starts[5] = {nullptr,"band1_start","band2_start","band3_start","band4_start"};
    static const char* ends[5]   = {nullptr,"band1_end","band2_end","band3_end","band4_end"};
    for (int b=1;b<=4;++b) {
        auto chunk = chunk_range(c.log, cop0, c.cop_words, c.mark.at(starts[b]), c.mark.at(ends[b]), f.begin, f.end);
        uint32_t s_b = 0x50 + 29*b;
        auto lo = sprite_line_low_cycle(c.log, kSpr0Data, c.pairs, s_b,   f.begin, f.end);
        auto hi = sprite_line_low_cycle(c.log, kSpr0Data, c.pairs, s_b+2, f.begin, f.end);
        if (!(chunk.any && lo && hi && *lo < chunk.lo && chunk.hi < *hi)) c_ok = false;
    }
    return a_ok && b_ok && c_ok;
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
        if (!all_fetched(c.log, word_addr(kSpr0Data), spr0_expected, f.begin, f.end)) pc2=false;
        if (!all_fetched(c.log, word_addr(kSpr1Data), spr1_expected, f.begin, f.end)) pc2=false;
    }
    checks.expect(pc2, "PC2 p.107 p.108: sprites 0 and 1 fetch POS, CTL and all 145 data pairs ($50-$E0) with "
                       "the prepared values plus the terminator, every graded frame");

    // PC3: sprites 2-7 idle (only the null structure's two words are attributable to them).
    bool pc3 = true;
    for (auto& f : g.frames) {
        if (count_reads(c.log, word_addr(kNullData)+0, 0, f.begin, f.end)==0) pc3=false;
        if (count_reads(c.log, word_addr(kNullData)+1, 0, f.begin, f.end)==0) pc3=false;
        // sprite 0's own structure must not be over-fetched (the signature a second
        // sprite channel sharing it would leave, see C3 below).
        for (size_t off=0; off<spr0_expected.size(); ++off)
            if (count_reads(c.log, word_addr(kSpr0Data)+(uint32_t)off, spr0_expected[off], f.begin, f.end) > 1) pc3=false;
    }
    checks.expect(pc3, "PC3 p.114 p.115: sprites 2 to 7 are idle (their DMA reads only the null structure's two "
                       "words; sprite 0's own data is not additionally fetched)");

    // PC4: Copper colour loads fall between the displayed sprite pixels.
    bool pc4 = true;
    for (auto& f : g.passes) if (!pc4_frame_ok(c, f)) pc4=false;
    checks.expect(pc4, "PC4 p.126 p.23: the Copper's colour-register MOVEs fall between sprite 0's own pair "
                       "fetches on the bus (top-of-list before the first bitplane fetch, the B0 reload before "
                       "line $50, each band's reload between its own line and two lines later)");

    // colour check (section 3.7), shared by P1-P4, plus PC5 (band 4's background).
    bool p1=true, p2=true, p3=true, p4=true, pc5=true;
    for (auto& f : g.frames) {
        FrameAnalysis fa = analyze_frame(c.video, f);
        if (!fa.ok) { p1=p2=p3=p4=pc5=false; continue; }
        ColourCheck cc = colour_check(c.video, fa);
        if (!cc.structural_ok()) { p1=p2=p3=p4=false; }
        for (int g0=0; g0<3;  ++g0) if (!cc.colour_ok[g0])   p1=false;
        for (int g0=3; g0<6;  ++g0) if (!cc.colour_ok[g0])   p1=false;
        for (int g0=6; g0<9;  ++g0) if (!cc.colour_ok[g0])   p1=false;
        for (int g0=9; g0<15; ++g0) if (!cc.colour_ok[g0])   p1=false;
        for (int g0 : {0,3,6,9,12})  if (!cc.colour_ok[g0])  p2=false;
        for (int g0 : {1,4,7,10,13}) if (!cc.colour_ok[g0])  p3=false;
        for (int g0 : {2,5,8,11,14}) if (!cc.colour_ok[g0])  p4=false;

        if (fa.groups.size()==15) {
            for (int gi=0; gi<15; ++gi)
                if (!pc5_group_ok(c.video, fa.groups[gi], expected_background(gi))) pc5=false;
        } else pc5=false;
    }
    checks.expect(pc5, "PC5 p.302: band 4's reload of COLOR00-COLOR15 took effect (background U=(10,10,10) on "
                       "groups 12-14, U=(00,00,00) on groups 0-11, every graded frame)");

    checks.expect(p1, "P1 p.302 CH4-appC-color.c1: SuperHires sprite colours are held in COLOR16-COLOR31, not in "
                       "the lower sixteen registers (groups 0-11 track their band, groups 12-14 match band 3's "
                       "colours unchanged by band 4's own reload of COLOR00-COLOR15)");
    checks.expect(p2, "P2 p.302 APPC-sprkeykey.c17r APPC-sprkeykey.c17g APPC-sprkeykey.c17b "
                       "CH4-appC-color.c5 CH4-appC-color.c6 CH4-appC-color.c7: colour 17 is displayed with red "
                       "bits3-2=GH, green bits3-2=IJ, blue bits3-2=KL on groups 0,3,6,9,12");
    checks.expect(p3, "P3 p.302 APPC-sprkeykey.c18r APPC-sprkeykey.c18g APPC-sprkeykey.c18b "
                       "CH4-appC-color.c8 CH4-appC-color.c9 CH4-appC-color.c10: colour 18 is displayed with red "
                       "bits3-2=MN, green bits3-2=OP, blue bits3-2=QR on groups 1,4,7,10,13");
    checks.expect(p4, "P4 p.302 APPC-sprkeykey.c19r APPC-sprkeykey.c19g APPC-sprkeykey.c19b "
                       "CH4-appC-color.c11 CH4-appC-color.c12 CH4-appC-color.c13: colour 19 is displayed with red "
                       "bits3-2=ST, green bits3-2=UV, blue bits3-2=WX on groups 2,5,8,11,14");

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
    checks.record("the lower two bits of every key claim's component (APPC-sprkeykey.*, CH4-appC-color.c2-c13): "
                   "p.302 marks them '--' and gives no expected value; masked and logged, never graded "
                   "(SPECIFICATION.md 1.4). The letter half of colour 17/18/19's claims is graded under P2/P3/P4.");
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
        bool colour_still_ok = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok) { colour_still_ok=false; continue; }
            ColourCheck cc = colour_check(c.video, fa);
            if (!cc.all_ok()) colour_still_ok=false;
        }
        checks.note(std::string("R1/R2 anchor: with HIRES (SHRES=0), the colour check itself still reads ") +
                    (colour_still_ok ? "PASS" : "FAIL") +
                    " -- COLOR17-19 read as ordinary 0x0RGB registers already carry the key's bits (SPECIFICATION.md 1.2, 3.6 R1)");
        return;
    }
    if (mut == Mut::C2) {
        std::vector<uint16_t> spr0_expected, spr1_expected;
        auto push_struct = [](std::vector<uint16_t>& v, uint16_t pos, uint16_t ctl,
                               const std::vector<std::pair<uint16_t,uint16_t>>& pairs) {
            v.push_back(pos); v.push_back(ctl);
            for (auto& p : pairs) { v.push_back(p.first); v.push_back(p.second); }
            v.push_back(0); v.push_back(0);
        };
        push_struct(spr0_expected, 0x5050, 0xE100, c.pairs);
        push_struct(spr1_expected, 0x5060, 0xE100, c.pairs);
        bool pc2 = true;
        for (auto& f : g.frames) {
            if (!all_fetched(c.log, word_addr(kSpr0Data), spr0_expected, f.begin, f.end)) pc2=false;
            if (!all_fetched(c.log, word_addr(kSpr1Data), spr1_expected, f.begin, f.end)) pc2=false;
        }
        checks.expect(pc2, "PC2 p.107 p.108: sprites 0 and 1 fetch POS, CTL and all 145 data pairs ($50-$E0) with "
                           "the prepared values plus the terminator, every graded frame");
        return;
    }
    if (mut == Mut::C3) {
        // Re-run PC3's own check (SPECIFICATION.md 3.8): under this mutation
        // sprite 2's channel also fetches sprite 0's structure, so sprite 0's
        // own words are read more than once per graded frame and the same
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
            if (count_reads(c.log, word_addr(kNullData)+0, 0, f.begin, f.end)==0) pc3=false;
            if (count_reads(c.log, word_addr(kNullData)+1, 0, f.begin, f.end)==0) pc3=false;
            for (size_t off=0; off<spr0_expected.size(); ++off)
                if (count_reads(c.log, word_addr(kSpr0Data)+(uint32_t)off, spr0_expected[off], f.begin, f.end) > 1) pc3=false;
        }
        checks.expect(pc3, "PC3 p.114 p.115: sprites 2 to 7 are idle (their DMA reads only the null structure's "
                           "two words; sprite 0's own data is not additionally fetched)");
        bool colour_unchanged = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok || !colour_check(c.video, fa).group_ok_all()) colour_unchanged=false;
        }
        checks.note(std::string("the displayed colour-match is ") + (colour_unchanged?"unchanged":"changed") +
                    " -- sprite 0 has priority over sprite 2 (p.115); the run-length clause of section 3.7 is "
                    "separately red in every SuperHires run of this package, see PC5/P1-P4 in the normal run");
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
        bool pc5 = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok || fa.groups.size()!=15) { pc5=false; continue; }
            for (int gi=0; gi<15; ++gi) if (!pc5_group_ok(c.video, fa.groups[gi], expected_background(gi))) pc5 = false;
        }
        checks.expect(pc5, "PC5 p.302: band 4's reload of COLOR00-COLOR15 took effect (background U=(10,10,10) on "
                           "groups 12-14, U=(00,00,00) on groups 0-11, every graded frame)");
        bool colour_still_ok = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok || !colour_check(c.video, fa).group_ok_all()) colour_still_ok = false;
        }
        checks.note(std::string("the colour-match clause of the colour check itself still reads ") +
                    (colour_still_ok?"PASS":"FAIL") + " (the run-length clause is separately red in every "
                    "SuperHires run of this package, see PC5/P1-P4 in the normal run)");
        return;
    }
    if (mut == Mut::C7) {
        // C7 narrows band 0's colour-17 run only; it does not change any
        // displayed colour, so it is isolated by comparing the shortest run
        // measured for group 0 against group 3 (the same colour, band 1,
        // untouched by this mutation) in the same run: a live, unchanged
        // comparator.
        int len0 = -1, len3 = -1;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok || fa.groups.size()<4) continue;
            ColourCheck cc = colour_check(c.video, fa);
            if (cc.run_len_min[0]>=0 && (len0<0 || cc.run_len_min[0]<len0)) len0 = cc.run_len_min[0];
            if (cc.run_len_min[3]>=0 && (len3<0 || cc.run_len_min[3]<len3)) len3 = cc.run_len_min[3];
        }
        checks.expect(len0>=0 && len3>=0 && len0<len3,
                      "P2 p.302 (run-length clause, section 3.7): group 0 narrows to " + std::to_string(len0) +
                      " samples while group 3, the same colour on band 1, stays at " + std::to_string(len3) +
                      " samples in the same run");
        return;
    }
    if (mut == Mut::C8) {
        // Isolated from the run-length clause (separately red in every
        // SuperHires run of this package, see the normal run PC5/P1-P4):
        // band 4 (groups 12-14) must equal band 3 (groups 9-11), not band 0.
        bool colour_ok = true;
        bool pc5 = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok) { colour_ok=false; pc5=false; continue; }
            ColourCheck cc = colour_check(c.video, fa);
            for (int g0=9; g0<15; ++g0) if (!cc.colour_ok[g0]) colour_ok=false;
            if (fa.groups.size()==15)
                for (int gi=12; gi<15; ++gi) if (!pc5_group_ok(c.video, fa.groups[gi], kBg2)) pc5=false;
        }
        checks.expect(!colour_ok, "P1 p.302 CH4-appC-color.c1 (colour-match clause): groups 12-14 no longer equal "
                                  "band 3, colours (band 0, colours were substituted)");
        checks.expect(pc5, "PC5 p.302: band 4, reload of COLOR00-COLOR15 still took effect (background still "
                           "U=(10,10,10) on groups 12-14)");
        return;
    }
    if (mut == Mut::C9) {
        bool colour_ok = true;
        bool others_ok = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok) { colour_ok=false; others_ok=false; continue; }
            ColourCheck cc = colour_check(c.video, fa);
            if (!cc.colour_ok[3]) colour_ok=false;
            for (int g0 : {0,6,9,12}) if (!cc.colour_ok[g0]) others_ok=false;
        }
        checks.expect(!colour_ok, "P2 p.302 APPC-sprkeykey.c17r APPC-sprkeykey.c17g APPC-sprkeykey.c17b "
                           "CH4-appC-color.c5 CH4-appC-color.c6 CH4-appC-color.c7 (colour-match clause): group 3 "
                           "(band 1) no longer shows red bits3-2=GH after the G/H swap");
        checks.note(std::string("groups 0, 6, 9, 12 (colour 17, other bands) colour-match ") +
                    (others_ok?"still PASS":"also FAIL") + " under this mutation");
        return;
    }
    if (mut == Mut::C10) {
        bool colour_ok = true;
        bool others_ok = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok) { colour_ok=false; others_ok=false; continue; }
            ColourCheck cc = colour_check(c.video, fa);
            if (!cc.colour_ok[7]) colour_ok=false;
            for (int g0 : {1,4,10,13}) if (!cc.colour_ok[g0]) others_ok=false;
        }
        checks.expect(!colour_ok, "P3 p.302 APPC-sprkeykey.c18r APPC-sprkeykey.c18g APPC-sprkeykey.c18b "
                           "CH4-appC-color.c8 CH4-appC-color.c9 CH4-appC-color.c10 (colour-match clause): group 7 "
                           "(band 2) no longer shows green bits3-2=OP after the O/P swap");
        checks.note(std::string("groups 1, 4, 10, 13 (colour 18, other bands) colour-match ") +
                    (others_ok?"still PASS":"also FAIL") + " under this mutation");
        return;
    }
    if (mut == Mut::C11) {
        bool colour_ok = true;
        bool others_ok = true;
        for (auto& f : g.frames) {
            FrameAnalysis fa = analyze_frame(c.video, f);
            if (!fa.ok) { colour_ok=false; others_ok=false; continue; }
            ColourCheck cc = colour_check(c.video, fa);
            if (!cc.colour_ok[8]) colour_ok=false;
            for (int g0 : {2,5,11,14}) if (!cc.colour_ok[g0]) others_ok=false;
        }
        checks.expect(!colour_ok, "P4 p.302 APPC-sprkeykey.c19r APPC-sprkeykey.c19g APPC-sprkeykey.c19b "
                           "CH4-appC-color.c11 CH4-appC-color.c12 CH4-appC-color.c13 (colour-match clause): group 8 "
                           "(band 2) no longer shows blue bits3-2=WX after the W/X swap");
        checks.note(std::string("groups 2, 5, 11, 14 (colour 19, other bands) colour-match ") +
                    (others_ok?"still PASS":"also FAIL") + " under this mutation");
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
