// test_ch6_blitter_line_fill.cpp
//
// ch6-blitter-line-fill: see SPECIFICATION.md.
//
// Topic 1 (page 190-191): the line-mode dx/dy setup recipe is CPU
// arithmetic, run by prog_line.s and read back from RESULTS.
// Topic 2 (page 182, 184, 200): area fill requires descending mode, tested
// by running the same 5-word/3-marker boundary row through the blitter's
// inclusive area-fill logic once descending, once ascending, and reading
// back the two marker-free interior words (word1, word3) -- see
// SPECIFICATION.md for the by-hand trace of agnus_blitter_fill.v's carry
// chain that the expected $0000/$FFFF values come from.
//
// NMH_MUTATE=LINE loads prog_line_mut.bin (different EP1 endpoints) in
// place of prog_line.bin. NMH_MUTATE=FILL_DESC loads prog_fill_desc_mut.bin
// (middle marker moved into word1) in place of prog_fill_desc.bin. Neither
// is exercised by a plain `make test`; they are re-runs used to confirm the
// MUTATION lines recorded below.
//
// MUTATION LINE-RAW-DELTA stimulus red endpoints changed from (10,5)-(3,40) to (0,0)-(6,2), via NMH_MUTATE=LINE
// MUTATION LINE-NORMALIZE stimulus red endpoints changed from (10,5)-(3,40) to (0,0)-(6,2), via NMH_MUTATE=LINE
// MUTATION FILL-DESC-CORRECT stimulus red middle marker moved from word 2 to word 1, via NMH_MUTATE=FILL_DESC

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
    std::vector<std::optional<uint16_t>> words;
};

Run run_program(const std::string& bin, size_t result_words) {
    Run r;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin);
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    for (size_t i = 0; i < result_words; ++i) r.words.push_back(h.peek_logical(kResults + 2 * i));
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

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks c;

    const char* mutate = std::getenv("NMH_MUTATE");
    const bool mutate_line       = mutate && std::string(mutate) == "LINE";
    const bool mutate_fill_desc  = mutate && std::string(mutate) == "FILL_DESC";

    // ---- Topic 1: line-mode dx/dy (page 190-191) ----
    {
        const std::string bin = dir + (mutate_line ? "/prog_line_mut.bin" : "/prog_line.bin");
        Run r = run_program(bin, 9);  // fixture + EP1 dx0,dy0,dx_n,dy_n + EP2 dx0,dy0,dx_n,dy_n
        const bool loaded = ok(r);

        c.expect(loaded && has(r, 0) && w(r, 0) == 0xBEEF,
            "FIXTURE line: prog_line stores a known constant (BEEF) first, read back before any computed value is trusted");

        c.expect(loaded && has(r, 1) && has(r, 2) && w(r, 1) == 7 && w(r, 2) == 35,
            "6-line-mode.c39 6-line-mode.c40 ch6-line.c12 ch6-line.c13 p190: "
            "before normalization dx0=abs(x2-x1)=7 and dy0=abs(y2-y1)=35 for endpoints (10,5)-(3,40)");

        c.expect(loaded && has(r, 3) && has(r, 4) && w(r, 3) == 35 && w(r, 4) == 7,
            "6-line-mode.c41 6-line-mode.c42 ch6-line.c14 ch6-line.c15 ch6-line.c16 p191: "
            "normalized dx_n=max(dx0,dy0)=35 and dy_n=min(dx0,dy0)=7, swapped from the raw 7,35");

        c.expect(loaded && has(r, 5) && has(r, 6) && has(r, 7) && has(r, 8) &&
                 w(r, 5) == 50 && w(r, 6) == 10 && w(r, 7) == 50 && w(r, 8) == 10,
            "CONTROL LINE-NORMALIZE-NOOP: endpoints (0,0)-(50,10) are already ordered (dx0=50 > dy0=10), "
            "so normalization leaves dx_n=50, dy_n=10 unchanged -- same comparator, expected PASS");

        if (loaded)
            c.note("line: dx0=" + hex4(w(r,1)) + " dy0=" + hex4(w(r,2)) +
                    " dx_n=" + hex4(w(r,3)) + " dy_n=" + hex4(w(r,4)) +
                    " (EP2 dx0=" + hex4(w(r,5)) + " dy0=" + hex4(w(r,6)) +
                    " dx_n=" + hex4(w(r,7)) + " dy_n=" + hex4(w(r,8)) + ")");
    }

    // ---- Topic 2: area fill requires descending mode (pages 182, 184, 200) ----
    {
        Run r = run_program(dir + "/prog_fill_plain.bin", 5);
        const bool loaded = ok(r);
        c.expect(loaded && has(r,0) && has(r,1) && has(r,2) && has(r,3) && has(r,4) &&
                 w(r,0) == 0x0100 && w(r,1) == 0x0000 && w(r,2) == 0x0100 &&
                 w(r,3) == 0x0000 && w(r,4) == 0x0100,
            "FIXTURE fill: a plain, fill-disabled blit reproduces the 5-word/3-marker row exactly "
            "(0100 0000 0100 0000 0100) before any fill assertion is trusted");
        if (loaded)
            c.note("fill plain copy: " + hex4(w(r,0)) + " " + hex4(w(r,1)) + " " + hex4(w(r,2)) +
                    " " + hex4(w(r,3)) + " " + hex4(w(r,4)));
    }
    {
        const std::string bin = dir + (mutate_fill_desc ? "/prog_fill_desc_mut.bin" : "/prog_fill_desc.bin");
        Run r = run_program(bin, 2);
        const bool loaded = ok(r);
        c.expect(loaded && has(r,0) && has(r,1) && w(r,0) == 0x0000 && w(r,1) == 0xFFFF,
            "6-descending-mode.c10 ch6-desc.c13 6-area-fill.c7 ch6-fill.c6 p182 p184: "
            "an area-fill blit of the 5-word/3-marker row run in descending mode reads back "
            "word1=0000, word3=FFFF");
        if (loaded)
            c.note("fill descending: word1=" + hex4(w(r,0)) + " word3=" + hex4(w(r,1)));
    }
    {
        Run r = run_program(dir + "/prog_fill_asc.bin", 2);
        const bool loaded = ok(r);
        c.expect(loaded && has(r,0) && has(r,1) && !(w(r,0) == 0x0000 && w(r,1) == 0xFFFF),
            "6-blitter-key-points.c18 ch6-keypoints.c20 p200: the identical blit run in ascending mode "
            "instead does not reproduce the descending-mode result (word1=0000, word3=FFFF)");
        if (loaded)
            c.note("fill ascending: word1=" + hex4(w(r,0)) + " word3=" + hex4(w(r,1)));
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
