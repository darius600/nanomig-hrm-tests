// test_ch6_blitter_line_fill.cpp
//
// ch6-blitter-line-fill: see SPECIFICATION.md.
//
// Topic 1 (page 189-191): prog_line.s runs a real line-mode blit for
// endpoints (10,5)-(3,40) and leaves the drawn line in a 36-word row
// buffer (LINE_ROW0, one word per row y=5..40, bit index = x); the harness
// reads all 36 rows back with peek_logical and grades only what page 189
// and 191 fix -- first pixel, last pixel, one pixel per row, x falling by
// 0 or 1 -- never an exact intermediate position.
//
// Topic 2 (page 182, 184, 200): prog_fill_desc.s / prog_fill_asc.s run the
// same 5-word/3-marker boundary row through the blitter's inclusive
// area-fill logic once descending, once ascending, into a destination that
// starts out holding the sentinel $A5A5 (not zero) so a precondition check
// can tell the blit actually wrote before the graded words are trusted.
// See SPECIFICATION.md for the page-derived (not RTL-derived) fill
// direction. CONTROL-FILL-DESC carries no claim ids -- it is the control;
// only the ascending run can falsify "descending mode is necessary", so
// the p.182/p.184 ids sit on the ascending assertion together with p.200's.
//
// NMH_MUTATE selects a mutated program in place of its normal counterpart;
// none is exercised by a plain `make test`. Re-runs used to confirm the
// MUTATION lines below:
//   NMH_MUTATE=LINE          prog_line_mut.bin (unnormalized registers)
//   NMH_MUTATE=LINE_FIXTURE  prog_line_fixture_mut.bin (wrong constant)
//   NMH_MUTATE=FILL_FIXTURE  prog_fill_plain_mut.bin (word4 marker dropped)
//   NMH_MUTATE=FILL_DESC     prog_fill_desc_mut.bin (marker moved to word1)
//   NMH_MUTATE=FILL_ASC      prog_fill_asc_mut.bin (word0 marker dropped)
//
// MUTATION 6-line-mode.c39 stimulus red unnormalized deltas fed into BLTAPT/BLTAMOD/BLTBMOD/BLTSIZE, height becomes 8 instead of 36, via NMH_MUTATE=LINE
// MUTATION FIXTURE-LINE stimulus red stored constant changed to DEAD instead of BEEF, via NMH_MUTATE=LINE_FIXTURE
// MUTATION FIXTURE-FILL stimulus red word4's marker dropped, via NMH_MUTATE=FILL_FIXTURE
// MUTATION CONTROL-FILL-DESC stimulus red middle marker moved from word 2 to word 1, via NMH_MUTATE=FILL_DESC
// MUTATION 6-descending-mode.c10 stimulus red word0's marker dropped, ascending scan coincides with the descending pattern, via NMH_MUTATE=FILL_ASC
//
// PRECONDITION-FILL-DESC and PRECONDITION-FILL-ASC (the sentinel-overwritten
// checks) carry no dedicated mutation: not exercised, by a scope decision.
// Their own stimulus is identical to CONTROL-FILL-DESC's and
// 6-descending-mode.c10's, whose mutations (marker moved / dropped) change
// which fill pattern comes out, not whether the blit writes at all; a
// mutation isolating "the blit completed without ever writing channel D"
// (for example clearing BLTCON0's USED bit) is out of scope for this
// repair.

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// nanomig.i, restated: the harness side of the contract.
constexpr uint32_t kResults    = 0x0C0000;
constexpr uint32_t kDoneAddr   = 0x0C0FFE;
constexpr uint16_t kDoneValue  = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 4'000'000;

// prog_line.s's row buffer: 36 words, one per row y=5..40.
constexpr uint32_t kLineRow0 = 0x082400;
constexpr int kLineRows = 36;

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

struct Run {
    size_t loaded = 0;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<std::optional<uint16_t>> words;  // one entry per requested address, in order
};

Run run_program(const std::string& bin, const std::vector<uint32_t>& addrs) {
    Run r;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin);
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    for (uint32_t a : addrs) r.words.push_back(h.peek_logical(a));
    return r;
}

bool has(const Run& r, size_t i) { return i < r.words.size() && r.words[i].has_value(); }
uint16_t w(const Run& r, size_t i) { return has(r, i) ? *r.words[i] : 0xDEAD; }
bool ok(const Run& r) { return r.loaded > 0 && r.done.has_value(); }

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "%04X", v);
    return std::string(buf);
}

// Analyzes the 36 row words (r.words[1..36], r.words[0] is the fixture).
// Grades only what page 189/191 fix: exactly one bit per row, the first
// row's bit at x=10, the last row's bit at x=3, and x never rising and
// never falling by more than one between consecutive rows. Never checks an
// exact intermediate position.
struct LineAnalysis {
    bool have_all = false;
    bool all_single_bit = false;
    bool first_ok = false;
    bool last_ok = false;
    bool monotonic_ok = false;
    int first_bit = -1;
    int last_bit = -1;
    int max_step = -1;
    int bad_row = -1;  // first row whose word does not carry exactly one bit, else -1
    bool complete() const { return have_all && all_single_bit && first_ok && last_ok && monotonic_ok; }
};

LineAnalysis analyze_line(const Run& r) {
    LineAnalysis a;
    if (r.words.size() < 1 + kLineRows) return a;
    std::vector<int> bits(kLineRows, -1);
    a.have_all = true;
    a.all_single_bit = true;
    for (int i = 0; i < kLineRows; ++i) {
        if (!has(r, 1 + i)) { a.have_all = false; break; }
        uint16_t v = w(r, 1 + i);
        int pc = 0, idx = -1;
        for (int b = 0; b < 16; ++b) if (v & (1 << b)) { ++pc; idx = b; }
        bits[i] = idx;
        if (pc != 1 && a.bad_row < 0) a.bad_row = i;
        if (pc != 1) a.all_single_bit = false;
    }
    if (!a.have_all) return a;
    a.first_bit = bits[0];
    a.last_bit = bits[kLineRows - 1];
    a.first_ok = (a.first_bit == 10);
    a.last_ok = (a.last_bit == 3);
    a.monotonic_ok = true;
    a.max_step = 0;
    for (int i = 0; i < kLineRows - 1; ++i) {
        if (bits[i] < 0 || bits[i + 1] < 0) { a.monotonic_ok = false; continue; }
        int step = bits[i] - bits[i + 1];
        if (step < 0 || step > 1) a.monotonic_ok = false;
        if (step > a.max_step) a.max_step = step;
    }
    return a;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks c;

    const char* mutate = std::getenv("NMH_MUTATE");
    const std::string mv = mutate ? mutate : "";
    const bool mutate_line          = mv == "LINE";
    const bool mutate_line_fixture  = mv == "LINE_FIXTURE";
    const bool mutate_fill_fixture  = mv == "FILL_FIXTURE";
    const bool mutate_fill_desc     = mv == "FILL_DESC";
    const bool mutate_fill_asc      = mv == "FILL_ASC";

    // ---- Topic 1: the line-mode claims (page 189-191) ----
    {
        std::string bin = dir + "/prog_line.bin";
        if (mutate_line) bin = dir + "/prog_line_mut.bin";
        if (mutate_line_fixture) bin = dir + "/prog_line_fixture_mut.bin";

        std::vector<uint32_t> addrs;
        addrs.push_back(kResults);  // fixture
        for (int i = 0; i < kLineRows; ++i) addrs.push_back(kLineRow0 + 2 * i);

        Run r = run_program(bin, addrs);
        const bool loaded = ok(r);

        c.expect(loaded && has(r, 0) && w(r, 0) == 0xBEEF,
            "FIXTURE-LINE: prog_line stores a known constant (BEEF) first, read back before the drawn line is trusted");

        LineAnalysis a = analyze_line(r);
        c.expect(loaded && a.complete(),
            "6-line-mode.c39 6-line-mode.c40 ch6-line.c12 ch6-line.c13 "
            "6-line-mode.c41 6-line-mode.c42 ch6-line.c14 ch6-line.c15 ch6-line.c16 "
            "p189 p190 p191: a line-mode blit for endpoints (10,5)-(3,40) draws exactly one "
            "pixel in every row y=5..40 (36 rows), the first at x=10, the last at x=3, x falling "
            "by 0 or 1 between consecutive rows (interior positions not graded)");

        if (loaded && a.have_all)
            c.note("line: first_x=" + std::to_string(a.first_bit) + " last_x=" + std::to_string(a.last_bit) +
                    " max_step=" + std::to_string(a.max_step) +
                    " all_single_bit=" + (a.all_single_bit ? "yes" : "no") +
                    (a.bad_row >= 0 ? (" bad_row=" + std::to_string(a.bad_row)) : ""));
        else if (loaded)
            c.note("line: could not read all 36 rows");
        if (loaded) {
            std::string dump;
            for (int i = 0; i < kLineRows; ++i) dump += hex4(w(r, 1 + i)) + " ";
            c.note("line raw rows: " + dump);
        }
    }

    // ---- Topic 2: area fill requires descending mode (pages 182, 184, 200) ----
    constexpr uint16_t kSentinel = 0xA5A5;
    {
        const std::string bin = mutate_fill_fixture ? dir + "/prog_fill_plain_mut.bin" : dir + "/prog_fill_plain.bin";
        Run r = run_program(bin, {kResults, kResults + 2, kResults + 4, kResults + 6, kResults + 8});
        const bool loaded = ok(r);
        c.expect(loaded && has(r,0) && has(r,1) && has(r,2) && has(r,3) && has(r,4) &&
                 w(r,0) == 0x0100 && w(r,1) == 0x0000 && w(r,2) == 0x0100 &&
                 w(r,3) == 0x0000 && w(r,4) == 0x0100,
            "FIXTURE-FILL: a plain, fill-disabled blit reproduces the 5-word/3-marker row exactly "
            "(0100 0000 0100 0000 0100) into a destination that started as the sentinel A5A5, "
            "before any fill assertion is trusted");
        if (loaded)
            c.note("fill plain copy: " + hex4(w(r,0)) + " " + hex4(w(r,1)) + " " + hex4(w(r,2)) +
                    " " + hex4(w(r,3)) + " " + hex4(w(r,4)));
    }
    {
        const std::string bin = mutate_fill_desc ? dir + "/prog_fill_desc_mut.bin" : dir + "/prog_fill_desc.bin";
        Run r = run_program(bin, {kResults, kResults + 2});
        const bool loaded = ok(r);

        c.expect(loaded && has(r,0) && has(r,1) && w(r,0) != kSentinel && w(r,1) != kSentinel,
            "PRECONDITION-FILL-DESC: word1 and word3 no longer read back as the A5A5 sentinel the "
            "destination started with -- the descending blit actually wrote them");

        c.expect(loaded && has(r,0) && has(r,1) && w(r,0) == 0x0000 && w(r,1) == 0xFFFF,
            "CONTROL-FILL-DESC: a fill blit run in descending mode reads back word1=0000, "
            "word3=FFFF (control, same measurement basis as the ascending check, carries no claim id)");
        if (loaded)
            c.note("fill descending: word1=" + hex4(w(r,0)) + " word3=" + hex4(w(r,1)));
    }
    {
        const std::string bin = mutate_fill_asc ? dir + "/prog_fill_asc_mut.bin" : dir + "/prog_fill_asc.bin";
        Run r = run_program(bin, {kResults, kResults + 2});
        const bool loaded = ok(r);

        c.expect(loaded && has(r,0) && has(r,1) && w(r,0) != kSentinel && w(r,1) != kSentinel,
            "PRECONDITION-FILL-ASC: word1 and word3 no longer read back as the A5A5 sentinel the "
            "destination started with -- the ascending blit actually wrote them");

        c.expect(loaded && has(r,0) && has(r,1) && !(w(r,0) == 0x0000 && w(r,1) == 0xFFFF),
            "6-descending-mode.c10 ch6-desc.c13 6-area-fill.c7 ch6-fill.c6 "
            "6-blitter-key-points.c18 ch6-keypoints.c20 p182 p184 p200: the identical blit run in "
            "ascending mode does not reproduce the descending-mode result (word1=0000, word3=FFFF) "
            "-- only the ascending run can falsify \"descending mode is necessary\"");
        if (loaded)
            c.note("fill ascending: word1=" + hex4(w(r,0)) + " word3=" + hex4(w(r,1)));
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
