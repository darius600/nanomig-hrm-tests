// test_ch6_blitter_geometry.cpp
//
// Integration package ch6-blitter-geometry: see SPECIFICATION.md. Every
// program drives nanomig.v only through the blitter's custom registers
// (integration/harness/nanomig.i) and is graded only against
// peek_logical()/peek_word() and ram_log() -- nanomig_harness.h's
// ports-only surface.
//
// Mutations are separate runs, selected by the NMH_MUTATE environment
// variable naming an assertion's leading label. The normal run (NMH_MUTATE
// unset) never loads a mutant program: its summary counts the fixture,
// preconditions and assertions only.
//
// MUTATION A0-fixture stimulus red NMH_MUTATE=A0-fixture: withhold the BLTSIZE write
// MUTATION A1-row-masks stimulus red NMH_MUTATE=A1-row-masks: a different BLTAFWM value
// MUTATION A2-plus-one stimulus red NMH_MUTATE=A2-plus-one: a 2-word destination modulo
// MUTATION A3-embedded-modulo stimulus red NMH_MUTATE=A3-embedded-modulo: a 20-byte modulo instead of 30
// MUTATION A4-cookiecut stimulus red NMH_MUTATE=A4-cookiecut: function $AC instead of $CA
// MUTATION A5-read-before-write stimulus red NMH_MUTATE=A5-read-before-write: channel C points elsewhere
// MUTATION A6-shift-decomposition stimulus red NMH_MUTATE=A6-shift-decomposition: a 5-word pointer advance instead of 6
// MUTATION A7a-ascending-corrupts stimulus red NMH_MUTATE=A7a-ascending-corrupts: use descending mode instead of ascending
// MUTATION A7a-descending-overlap stimulus red NMH_MUTATE=A7a-descending-overlap: pointers left at the block's first word
// MUTATION A7a-descending-modulo stimulus red NMH_MUTATE=A7a-descending-modulo: BLTDMOD=0 instead of 30
// MUTATION A7b-ascending-overlap stimulus red NMH_MUTATE=A7b-ascending-overlap: descending mode for the same one-row-up move
// MUTATION A8a-dest-grows stimulus red NMH_MUTATE=A8a-dest-grows: blit size 2 instead of 3
// MUTATION A8b-source-larger stimulus red NMH_MUTATE=A8b-source-larger: blit size 2 instead of 3
// MUTATION A8c-arb-mask stimulus red NMH_MUTATE=A8c-arb-mask: $FFFF masks instead of the computed ones
// MUTATION A9-minterm-bit stimulus red NMH_MUTATE=A9-minterm-bit: n=4's slot gets n=3's bit
// MUTATION A9b-not-a stimulus red NMH_MUTATE=A9b-not-a: LF=$30 instead of $0F
// MUTATION A10-minterm-or stimulus red NMH_MUTATE=A10-minterm-or: LF=$80 instead of $C0
// MUTATION A11-word-only stimulus red NMH_MUTATE=A11-word-only: the pointer's even part off by a word
// MUTATION A12-shift-right stimulus red NMH_MUTATE=A12-shift-right: use descending mode instead of ascending
// MUTATION A12-shift-left stimulus red NMH_MUTATE=A12-shift-left: use ascending mode instead of descending
// MUTATION A12-within-word stimulus red NMH_MUTATE=A12-within-word: the naive pointer instead of one word lower
// MUTATION A13-fig61 stimulus red NMH_MUTATE=A13-fig61: start channel A one word later
// MUTATION A14-fig63 stimulus red NMH_MUTATE=A14-fig63: LF=$0F instead of $F0
// MUTATION A15-fig64 stimulus red NMH_MUTATE=A15-fig64: 3 words/row instead of 2
// MUTATION A16-mintdc stimulus red NMH_MUTATE=A16-mintdc: LF=$DE instead of $DC
// MUTATION A17-truthorder stimulus red NMH_MUTATE=A17-truthorder: the top-down reading, LF=$3F
// MUTATION A18-fourplanes stimulus red NMH_MUTATE=A18-fourplanes: only the first plane's blit runs
// MUTATION A19-highbit stimulus red NMH_MUTATE=A19-highbit: bit 20 instead of bit 21
// MUTATION A20-mem512 stimulus red NMH_MUTATE=A20-mem512: memory_config 0 for the 2 MB half too

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <iostream>
#include <fstream>
#include <iterator>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// nanomig.i, restated: the harness side of the contract.
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 3'000'000;

// geom.i, restated: this package's own scratch layout.
constexpr uint32_t kScratch   = 0x082000;
constexpr uint32_t kSrc       = kScratch + 0x0000;
constexpr uint32_t kSrc2      = kScratch + 0x0800;
constexpr uint32_t kMaskbuf   = kScratch + 0x1000;
constexpr uint32_t kDst       = kScratch + 0x4000;
constexpr uint32_t kDst2      = kScratch + 0x4800;
constexpr uint32_t kBitmap    = kScratch + 0x6000;
constexpr uint32_t kDst3      = kScratch + 0xA000;
constexpr uint32_t kPlaneBmp0 = kScratch + 0x20000;
constexpr uint32_t kPlaneBmp1 = kScratch + 0x21000;
constexpr uint32_t kPlaneBmp2 = kScratch + 0x22000;
constexpr uint32_t kPlaneBmp3 = kScratch + 0x23000;

// The assertion call form is fixed by BRIEF.md.
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

Checks checks;

// The mutation environment variable: names the one assertion, by its
// leading label, whose mutant program/config should be used this run.
const char* g_mutate = std::getenv("NMH_MUTATE");

bool mutating(const char* label) {
    return g_mutate && std::string(g_mutate) == label;
}

// Picks the mutant program in place of the normal one when NMH_MUTATE
// names this assertion; otherwise the normal program. The normal run
// (NMH_MUTATE unset) never selects a mutant.
std::string pick(const char* label, const char* normal, const char* mutant) {
    return mutating(label) ? mutant : normal;
}

struct Run {
    std::unique_ptr<nmh::NanomigHarness> h;
    size_t loaded = 0;
    bool released = false;
    std::optional<uint64_t> done;
};

Run run(const std::string& dir, const std::string& name) {
    Run r;
    nmh::Config cfg;
    r.h = std::make_unique<nmh::NanomigHarness>(cfg);
    r.loaded = r.h->load_program(dir + "/" + name + ".bin");
    if (!r.loaded) return r;
    r.released = r.h->boot(kBootBudget).cpu_released;
    r.done = r.h->run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    return r;
}

uint16_t W(const Run& r, uint32_t byte_addr) {
    auto v = r.h->peek_logical(byte_addr);
    return v.value_or(0x0000);
}

bool ok(const Run& r) { return r.loaded > 0 && r.released && r.done.has_value(); }

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%04X", v);
    return buf;
}

// ---- memory_config = 0 (512 KB chip RAM) support --------------------
//
// minimig_bankmapper.v folds chip1/chip2/chip3 onto chip0's bank under
// memory_config 0 (bank_r[3:0] = {0,0,0, chip3|chip2|chip1|chip0}), so any
// logical chip-RAM address collapses to its offset within the first
// 512 KB block: physical = logical - 0x080000 for anything in
// [0x080000, 0x100000). load_program()/peek_logical() assume the 2 MB
// identity map (physical == logical) and cannot be used under this
// config; poke_word()/peek_word() (physical, harness-public) can.
constexpr uint32_t kChipCollapseBase = 0x080000;

size_t load_program_collapsed(nmh::NanomigHarness& h, const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return 0;
    std::vector<uint8_t> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (b.empty()) return 0;
    if (b.size() & 1) b.push_back(0);
    uint32_t phys_base = (nmh::kProgramOrg - kChipCollapseBase) / 2;
    for (size_t i = 0; i < b.size(); i += 2)
        h.poke_word(phys_base + i / 2, uint16_t((b[i] << 8) | b[i + 1]));
    h.poke_long(nmh::kKickShadowWordBase + 0, nmh::kProgramStack);
    h.poke_long(nmh::kKickShadowWordBase + 2, nmh::kProgramOrg);
    return b.size();
}

// -----------------------------------------------------------------------
// A0 -- fixture: registers set up, BLTSIZE last, known result delivered.
// Pages 169, 200. ch6-intro.c20, ch6-keypoints.c1.
void assertion_A0(const std::string& dir) {
    Run r = run(dir, pick("A0-fixture", "fixture", "fixture_nobltsize"));
    bool matches = r.loaded > 0 && r.done.has_value() &&
        W(r, kDst + 0) == 0x1234 && W(r, kDst + 2) == 0x5678 &&
        W(r, kDst + 4) == 0x9ABC && W(r, kDst + 6) == 0xDEF0;
    checks.expect(matches,
        "A0-fixture p169,200 ch6-intro.c20 ch6-keypoints.c1: "
        "registers set up and BLTSIZE written last deliver the known source "
        "pattern (1234 5678 9ABC DEF0) to the destination");
}

// -----------------------------------------------------------------------
// A1 -- BLTAFWM/BLTALWM mask the first/last word of every raster row.
// Page 169. 6-intro.c13, 6-intro.c14, ch6-intro.c12, ch6-intro.c13.
void assertion_A1(const std::string& dir) {
    Run r = run(dir, pick("A1-row-masks", "rowmask", "rowmask_wrongfwm"));
    bool matches = ok(r) &&
        W(r, kDst + 0) == 0x00FF && W(r, kDst + 2) == 0xFFFF && W(r, kDst + 4) == 0xFF00 &&
        W(r, kDst + 6) == 0x00FF && W(r, kDst + 8) == 0xFFFF && W(r, kDst + 10) == 0xFF00;
    checks.expect(matches,
        "A1-row-masks p169 6-intro.c13 6-intro.c14 ch6-intro.c12 ch6-intro.c13: "
        "on both rows of a 3-word/row, 2-row copy, word 0 is FWM-masked "
        "(00FF), word 1 is unmasked (FFFF) and word 2 is LWM-masked (FF00)");
}

// -----------------------------------------------------------------------
// A2 -- zero-modulo raster addressing is a "plus one" continuation.
// Page 171. ch6-dma.c30, ch6-dma.c31.
bool contiguous_run(const nmh::NanomigHarness& h, uint32_t base_byte_addr, size_t count) {
    uint32_t base_word = base_byte_addr / 2;
    auto log = nmh::coalesce(h.ram_log());
    std::vector<uint32_t> writes;
    for (const auto& a : log)
        if (a.is_write && a.word_addr >= base_word && a.word_addr < base_word + count + 8)
            writes.push_back(a.word_addr);
    if (writes.size() != count) return false;
    for (size_t i = 0; i < writes.size(); ++i)
        if (writes[i] != base_word + i) return false;
    return true;
}

void assertion_A2(const std::string& dir) {
    Run r = run(dir, pick("A2-plus-one", "rastercont", "rastercont_mod"));
    bool contiguous = ok(r) && contiguous_run(*r.h, kDst, 21);
    checks.expect(contiguous,
        "A2-plus-one p171 ch6-dma.c30 ch6-dma.c31: with BLTAMOD=BLTDMOD=0, "
        "the 21 destination writes of a 7-word/row, 3-row copy form one "
        "unbroken run of consecutive word addresses, no gap at either row "
        "boundary");
}

// -----------------------------------------------------------------------
// A3 -- embedded-rectangle modulo arithmetic and figure 6-2 (p.173): the
// rectangle sits at row 13, byte column 12, in a 40-byte/row, 18-row
// bitmap -- rows 0-12 are never touched, rows 13-17 hold the rectangle
// with the 12-byte left / 18-byte right margins at their sentinel.
// 6-dma-channels.c65, c73, c74, c75, c76, ch6-dma.f5, f7, f8.
const uint16_t kRectWords[25] = {
    0x1111, 0x2222, 0x3333, 0x4444, 0x5555,
    0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA,
    0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF,
    0x1234, 0x2345, 0x3456, 0x4567, 0x5678,
    0x6789, 0x789A, 0x89AB, 0x9ABC, 0xABCD,
};

bool embedded_rect_ok(const Run& r) {
    if (!ok(r)) return false;
    for (int row = 0; row < 18; ++row) {
        for (int word = 0; word < 20; ++word) {
            uint32_t addr = kBitmap + row * 40 + word * 2;
            uint16_t v = W(r, addr);
            bool inRect = row >= 13 && word >= 6 && word < 11;  // rows 13-17, bytes 12-21
            if (inRect) {
                int rectRow = row - 13;
                if (v != kRectWords[rectRow * 5 + (word - 6)]) return false;
            } else {
                if (v != 0xDEAD) return false;
            }
        }
    }
    return true;
}

void assertion_A3(const std::string& dir) {
    Run r = run(dir, pick("A3-embedded-modulo", "embedmod", "embedmod_wrongmod"));
    checks.expect(embedded_rect_ok(r),
        "A3-embedded-modulo p172,173 6-dma-channels.c65 6-dma-channels.c73 "
        "6-dma-channels.c74 6-dma-channels.c75 6-dma-channels.c76 ch6-dma.f5 "
        "ch6-dma.f7 ch6-dma.f8: with BLTDMOD=30 (2*5 words + 30 bytes = 40 "
        "bytes), a copy into figure 6-2's own position -- the embedded "
        "5-word rectangle at row 13, byte column 12, of a 40-byte/row, "
        "18-row bitmap -- lands the source on rows 13-17's rectangle and "
        "leaves the 12-byte left / 18-byte right margins, and rows 0-12, "
        "at their sentinel");
}

// -----------------------------------------------------------------------
// A4 -- cookie-cut mask is one bitplane regardless of background depth,
// function $CA. Page 176, 183. 6-function-generator.c40, ch6-minterms.c9,
// ch6-copy.c6.
void assertion_A4(const std::string& dir) {
    Run r = run(dir, pick("A4-cookiecut", "cookiecut", "cookiecut_wronglf"));
    bool run1 = ok(r) &&
        W(r, kDst + 0) == 0xA5A5 && W(r, kDst + 2) == 0xA5A5 &&
        W(r, kDst + 4) == 0xA5A5 && W(r, kDst + 6) == 0xA5A5;
    bool run2 = ok(r) &&
        W(r, kDst2 + 0) == 0xA3A3 && W(r, kDst2 + 2) == 0xA3A3 &&
        W(r, kDst2 + 4) == 0xA3A3 && W(r, kDst2 + 6) == 0xA3A3;
    checks.expect(run1 && run2,
        "A4-cookiecut p176,183 6-function-generator.c40 ch6-minterms.c9 "
        "ch6-copy.c6: with LF=$CA, D=(A AND B) OR (NOT A AND C); the same "
        "mask (A=F0F0) and object (B=AAAA) cut correctly against two "
        "different backgrounds (C=5555 giving A5A5, C=3333 giving A3A3) "
        "without the mask itself changing");
}

// -----------------------------------------------------------------------
// A5 -- the destination background is fetched (read) before it is
// written, and the write covers the whole word (both byte lanes), so the
// unmodified background bits in DST (0505 out of A5A5) can only be kept
// because the fetch happened. Page 176. ch6-minterms.c12.
bool read_before_write_all(const nmh::NanomigHarness& h, uint32_t base_byte_addr, size_t count) {
    uint32_t base_word = base_byte_addr / 2;
    const auto& log = h.ram_log();
    for (size_t i = 0; i < count; ++i) {
        uint32_t word_addr = base_word + i;
        bool found_read = false;
        for (const auto& a : log) {
            if (a.word_addr != word_addr) continue;
            if (a.is_write) return false;  // write seen before any read
            found_read = true;
            break;
        }
        if (!found_read) return false;
    }
    return true;
}

bool full_word_writes(const nmh::NanomigHarness& h, uint32_t base_byte_addr, size_t count) {
    uint32_t base_word = base_byte_addr / 2;
    bool saw_any = false;
    for (const auto& a : h.ram_log()) {
        if (!a.is_write || a.word_addr < base_word || a.word_addr >= base_word + count) continue;
        saw_any = true;
        if (!(a.upper_byte && a.lower_byte)) return false;
    }
    return saw_any;
}

void assertion_A5(const std::string& dir) {
    Run r = run(dir, pick("A5-read-before-write", "cookiecut", "cookiecut_celsewhere"));
    bool background_kept = ok(r) && W(r, kDst + 0) == 0xA5A5;
    bool rbw = ok(r) && read_before_write_all(*r.h, kDst, 4);
    bool wholeword = ok(r) && full_word_writes(*r.h, kDst, 4);
    checks.expect(background_kept && rbw && wholeword,
        "A5-read-before-write p176 ch6-minterms.c12: because there is no "
        "way to write only a portion of a word, the background must be "
        "fetched first -- a read of every destination word (channel C, "
        "pointed at the same address as D) precedes a full-word write "
        "(both byte lanes), and the unmodified background bits survive "
        "(DST=A5A5, its 0505 half from the true C=5555 background)");
}

// -----------------------------------------------------------------------
// A6 -- a shift over 15 bits is a whole-word destination-pointer
// displacement plus a residual barrel shift. Page 179.
// 6-shifts-and-masks.c10/c11/c12/c13, ch6-shifts.c10/c11/c12.
void assertion_A6(const std::string& dir) {
    Run r = run(dir, pick("A6-shift-decomposition", "shift100", "shift100_wrongadvance"));
    bool matches = ok(r) &&
        W(r, kDst + 12) == 0x0111 && W(r, kDst + 14) == 0x1222 &&
        W(r, kDst + 16) == 0x2333;
    checks.expect(matches,
        "A6-shift-decomposition p179 6-shifts-and-masks.c10 "
        "6-shifts-and-masks.c11 6-shifts-and-masks.c12 6-shifts-and-masks.c13 "
        "ch6-shifts.c10 ch6-shifts.c11 ch6-shifts.c12: a 100-bit right "
        "shift (6-word/12-byte destination-pointer advance plus a 4-bit "
        "residual shift, ASH=4) of source words 1111 2222 3333 gives "
        "destination words 0111 1222 2333, the reference computed "
        "independently of the RTL");
}

// -----------------------------------------------------------------------
// A7a -- descending mode moves an overlapping image down correctly.
// Page 182. ch6-desc.c1 (ascending overwrites); 6-descending-mode.c9,
// ch6-desc.c10 (descending pointers at the last word); ch6-desc.c11
// (same modulo as ascending, exercised with a real nonzero modulo).
bool clean_shift(const Run& r) {
    return ok(r) &&
        W(r, kSrc + 4) == 0x1111 && W(r, kSrc + 6) == 0x2222 &&
        W(r, kSrc + 8) == 0x3333 && W(r, kSrc + 10) == 0x4444 &&
        W(r, kSrc + 12) == 0x5555 && W(r, kSrc + 14) == 0x6666;
}

void assertion_A7a(const std::string& dir) {
    // p.182: "we overwrite the second row before we get a chance to copy
    // it" -- every later row ends up holding row 0's own data.
    Run asc = run(dir, pick("A7a-ascending-corrupts", "overlap_down_asc", "overlap_down_desc"));
    bool corrupt_pattern = ok(asc) &&
        W(asc, kSrc + 4) == 0x1111 && W(asc, kSrc + 6) == 0x2222 &&
        W(asc, kSrc + 8) == 0x1111 && W(asc, kSrc + 10) == 0x2222 &&
        W(asc, kSrc + 12) == 0x1111 && W(asc, kSrc + 14) == 0x2222;
    checks.expect(corrupt_pattern,
        "A7a-ascending-corrupts p182 ch6-desc.c1: moving a 3-row block one "
        "row down in place with a standard (ascending) copy overwrites the "
        "second row before it is copied -- rows 1-3 all read back as row "
        "0's own data (1111 2222 x3), the value the page predicts");

    Run desc = run(dir, pick("A7a-descending-overlap", "overlap_down_desc", "overlap_down_desc_bad"));
    checks.expect(clean_shift(desc),
        "A7a-descending-overlap p182 6-descending-mode.c9 ch6-desc.c10: "
        "descending mode, pointers at each block's last word, matches a "
        "clean one-row-down shift");

    Run descmod = run(dir, pick("A7a-descending-modulo", "descmod", "descmod_zeromod"));
    checks.expect(embedded_rect_ok(descmod),
        "A7a-descending-modulo p182 ch6-desc.c11: a descending copy of "
        "A3's embedded rectangle with the same positive BLTDMOD=30 as the "
        "ascending run -- not a zero modulo, which p.182 says is "
        "subtracted rather than added but never exercises the clause -- "
        "lands the same rectangle and leaves the same margins untouched");
}

// -----------------------------------------------------------------------
// A7b -- control: ascending mode moves an overlapping image up correctly,
// on the same measurement basis as A7a. p.183's "a lower memory address
// is higher on the display" is a separate, display-dependent clause of
// ch6-copy.c10 that this memory-only control does not measure (declined
// below); this control itself carries no claim id.
void assertion_A7b(const std::string& dir) {
    Run r = run(dir, pick("A7b-ascending-overlap", "overlap_up_asc", "overlap_up_desc"));
    bool matches = ok(r) &&
        W(r, kSrc + 0) == 0xAAAA && W(r, kSrc + 2) == 0xBBBB &&
        W(r, kSrc + 4) == 0xCCCC && W(r, kSrc + 6) == 0xDDDD &&
        W(r, kSrc + 8) == 0xEEEE && W(r, kSrc + 10) == 0xFFFF;
    checks.expect(matches,
        "A7b-ascending-overlap p183: CONTROL ascending copy of an "
        "overlapping one-row-up move (destination at the lower address) "
        "matches a clean shift -- the package's control case, on the same "
        "overlap/measurement basis as A7a");
}

// -----------------------------------------------------------------------
// A8a -- arbitrary-region copy, destination spans more words than
// source. Page 183. 6-copying-arbitrary-regions.c1, ch6-copy.c1,
// ch6-copy.c2, ch6-copy.c13, ch6-copy.c14, ch6-copy.c4, ch6-copy.c5.
void assertion_A8a(const std::string& dir) {
    Run r = run(dir, pick("A8a-dest-grows", "arbgrow", "arbgrow_wrongsize"));
    bool matches = ok(r) &&
        W(r, kDst + 0) == 0x000F && W(r, kDst + 2) == 0xFFFF &&
        W(r, kDst + 4) == 0xE000;
    checks.expect(matches,
        "A8a-dest-grows p183 6-copying-arbitrary-regions.c1 ch6-copy.c1 "
        "ch6-copy.c2 ch6-copy.c13 ch6-copy.c14 ch6-copy.c4 ch6-copy.c5: a "
        "23-bit-wide pattern (2 source words) shifted right 12 bits spans "
        "3 destination words (000F FFFF E000, the reference computed "
        "independently of the RTL); blit size is the larger span 3, ASH=0, "
        "and BLTAFWM/BLTALWM (000F/E000) mask channel A as if masking the "
        "destination's own edges, clipping the real neighbour word fetched "
        "past the true source out of the trailing word");
}

// -----------------------------------------------------------------------
// A8b -- arbitrary-region copy, source spans more words than the
// destination's true data. Page 183. 6-copying-arbitrary-regions.c2,
// ch6-copy.c3, ch6-copy.c4.
void assertion_A8b(const std::string& dir) {
    Run r = run(dir, pick("A8b-source-larger", "arbshrink", "arbshrink_wrongsize"));
    bool matches = ok(r) &&
        W(r, kDst + 0) == 0x0000 && W(r, kDst + 2) == 0xFFFF &&
        W(r, kDst + 4) == 0xF000;
    checks.expect(matches,
        "A8b-source-larger p183 6-copying-arbitrary-regions.c2 ch6-copy.c3 "
        "ch6-copy.c4: a 3-word source (20 true bits) shifted right 2 bits "
        "compacts its meaningful content into the second and third "
        "destination words (0000 FFFF F000, the reference computed "
        "independently of the RTL); blit size is still the larger span, 3 "
        "(the source's own), not the 2 the true data alone would need");
}

// -----------------------------------------------------------------------
// A8c -- arbitrary-region copy's A-channel row mask: disabled, all-ones,
// shifted the same as source B (ch6-copy.c11), first/last masks set as
// if masking B's own edges (ch6-copy.c12). Page 183.
void assertion_A8c(const std::string& dir) {
    Run r = run(dir, pick("A8c-arb-mask", "arbmask", "arbmask_nomask"));
    bool matches = ok(r) &&
        W(r, kDst + 0) == 0x5555 && W(r, kDst + 2) == 0xFFFF && W(r, kDst + 4) == 0xF555;
    checks.expect(matches,
        "A8c-arb-mask p183 ch6-copy.c11 ch6-copy.c12: channel A disabled, "
        "preloaded all-ones, ASH=BSH=2 (the same shift as source B), "
        "BLTAFWM=$0003/BLTALWM=$C000 (B's own shifted edge shape); against "
        "background C=D=5555 the cookie-cut result is 5555 FFFF F555, the "
        "reference computed independently of the RTL");
}

// -----------------------------------------------------------------------
// A9 -- minterm n is bit n of the LF byte, for every n = 0..7
// individually. Page 178. ch6-venn.c2.
void assertion_A9(const std::string& dir) {
    Run r = run(dir, pick("A9-minterm-bit", "mintper_n", "mintper_n_swap"));
    // Program order (DST+0..DST+14): n=7,6,5,4,3,2,1,0.
    static const uint16_t expected_by_slot[8] = {
        0xC000, 0x3000, 0x0C00, 0x0300, 0x00C0, 0x0030, 0x000C, 0x0003,
    };
    bool matches = ok(r);
    for (int slot = 0; slot < 8; ++slot)
        matches = matches && W(r, kDst + slot * 2) == expected_by_slot[slot];
    checks.expect(matches,
        "A9-minterm-bit p178 ch6-venn.c2: with A=FF00, B=F0F0, C=CCCC, LF="
        "1<<n for each n=7..0 gives C000 3000 0C00 0300 00C0 0030 000C "
        "0003 at DST+0..DST+14, the minterm-number-order truth table read "
        "bit by bit, not just at n=7 and as a group for n=0-3");
}

// -----------------------------------------------------------------------
// A9b -- the inverse of a source, NOT A, is minterms 0-3. Page 179.
// ch6-venn.c8.
void assertion_A9b(const std::string& dir) {
    Run r = run(dir, pick("A9b-not-a", "mintbit", "mintbit_wrongbit"));
    bool matches = ok(r) && W(r, kDst) == 0x00FF;
    checks.expect(matches,
        "A9b-not-a p179 ch6-venn.c8: with A=FF00, B=F0F0, C=CCCC, LF=$0F "
        "(minterms 0-3) gives NOT A = 00FF, whatever B and C are");
}

// -----------------------------------------------------------------------
// A10 -- minterms combine by OR-ing their LF bits. Page 178, 179.
// ch6-venn.c5, ch6-venn.c11.
void assertion_A10(const std::string& dir) {
    Run r = run(dir, pick("A10-minterm-or", "mintor", "mintor_missing"));
    bool matches = ok(r) && W(r, kDst) == 0xF000;
    checks.expect(matches,
        "A10-minterm-or p178,179 ch6-venn.c5 ch6-venn.c11: with A=FF00, "
        "B=F0F0, C=CCCC, LF=$C0 (minterms 7 and 6 OR'ed together) gives A "
        "AND B, regardless of C = F000");
}

// -----------------------------------------------------------------------
// A11 -- the blitter operates on words: a pointer's low bit is ignored.
// Page 172. 6-dma-channels.c79.
void assertion_A11(const std::string& dir) {
    Run r = run(dir, pick("A11-word-only", "wordonly", "wordonly_wrongword"));
    bool matches = ok(r) && W(r, kDst) == 0xABCD;
    checks.expect(matches,
        "A11-word-only p172 6-dma-channels.c79: BLTAPT set to SRC+1 (an "
        "odd byte address) fetches the word at the even, LSB-truncated "
        "address (ABCD), not the next word");
}

// -----------------------------------------------------------------------
// A12 -- ascending mode shifts right, descending shifts left (ch6-copy.c7,
// ch6-copy.c8); the shift value is the bit position of the leftmost edge
// within a word, not an absolute shift (ch6-copy.c9). Page 182, 183, 200.
void assertion_A12(const std::string& dir) {
    Run r_asc = run(dir, pick("A12-shift-right", "shiftdir_asc", "shiftdir_desc"));
    bool right = ok(r_asc) &&
        W(r_asc, kDst + 0) == 0x0111 && W(r_asc, kDst + 2) == 0x1222 && W(r_asc, kDst + 4) == 0x2333;
    checks.expect(right,
        "A12-shift-right p182,200 ch6-copy.c7: ascending mode with ASH=4 "
        "shifts source 1111 2222 3333 right, giving 0111 1222 2333");

    Run r_desc = run(dir, pick("A12-shift-left", "shiftdir_desc", "shiftdir_asc"));
    bool left = ok(r_desc) &&
        W(r_desc, kDst + 0) == 0x1112 && W(r_desc, kDst + 2) == 0x2223 && W(r_desc, kDst + 4) == 0x3330;
    checks.expect(left,
        "A12-shift-left p182,200 ch6-copy.c8: descending mode (pointers at "
        "the last word) with the same ASH=4 shifts the same source left, "
        "giving 1112 2223 3330");

    Run r_c9 = run(dir, pick("A12-within-word", "shiftc9", "shiftc9_noptr"));
    bool within_word = ok(r_c9) &&
        W(r_c9, kDst - 2) == 0x0001 && W(r_c9, kDst + 0) == 0x1112 &&
        W(r_c9, kDst + 2) == 0x2223 && W(r_c9, kDst + 4) == 0x3330;
    checks.expect(within_word,
        "A12-within-word p183 ch6-copy.c9: source 1111 2222 3333 has its "
        "own left edge at a word boundary; moving it four pixels left "
        "lands that edge at bit 12 of the word before -- ASH=12 (a "
        "rightward within-word shift) with BLTDPT one word lower, blit "
        "size 4 (p.183: the larger of the two spans) and BLTALWM=$0000 "
        "(the 4th, past-the-source word contributes nothing of its own) "
        "gives 0001 1112 2223 3330, the same leftward result, showing the "
        "shift value is the leftmost edge's bit position, not an absolute "
        "shift");
}

// -----------------------------------------------------------------------
// A13 -- figure 6-1 (p.171) as a blit: word addresses increase by one
// across a row and across the row boundary. 6-dma-channels.c30, c31, c32,
// ch6-dma.f1, f2, f3.
bool reads_exact_range(const nmh::NanomigHarness& h, uint32_t first_word, uint32_t last_word) {
    auto log = nmh::coalesce(h.ram_log());
    std::vector<uint32_t> reads;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= first_word && a.word_addr <= last_word + 8)
            reads.push_back(a.word_addr);
    size_t expected_count = last_word - first_word + 1;
    if (reads.size() != expected_count) return false;
    for (size_t i = 0; i < reads.size(); ++i)
        if (reads[i] != first_word + i) return false;
    return true;
}

void assertion_A13(const std::string& dir) {
    Run r = run(dir, pick("A13-fig61", "fig61", "fig61_wrongstart"));
    bool matches = ok(r) && reads_exact_range(*r.h, 20, 61);
    checks.expect(matches,
        "A13-fig61 p171 6-dma-channels.c30 6-dma-channels.c31 "
        "6-dma-channels.c32 ch6-dma.f1 ch6-dma.f2 ch6-dma.f3: a blit with "
        "BLTAPT at word address 20, width 7, height 6 reads channel A at "
        "exactly word addresses 20 through 61 (42 reads), consecutive "
        "within each row and across every row boundary, matching figure "
        "6-1's own grid and its 20..61 extent");
}

// -----------------------------------------------------------------------
// A14 -- figure 6-3 (p.178): the A circle is minterms 7,6,5,4 (LF=$F0,
// D=A); Table 6-1 (p.177) gives LF=$CC (D=B) and LF=$AA (D=C) for the B
// and C circles. ch6-venn.f-labels.
void assertion_A14(const std::string& dir) {
    Run r = run(dir, pick("A14-fig63", "fig63", "fig63_wronglf"));
    bool matches = ok(r) &&
        W(r, kDst) == 0xFF00 && W(r, kDst2) == 0xF0F0 && W(r, kDst3) == 0xCCCC;
    checks.expect(matches,
        "A14-fig63 p177,178 ch6-venn.f-labels: LF=$F0 gives D=A=FF00 (the "
        "A circle), LF=$CC gives D=B=F0F0 (the B circle), LF=$AA gives "
        "D=C=CCCC (the C circle), confirming the diagram has exactly these "
        "three labelled circles");
}

// -----------------------------------------------------------------------
// A15 -- figure 6-4 (p.181) extraction recipe: channel B reads exactly
// two consecutive source words per row and D writes two per row.
// 6-shifts-and-masks.c32, ch6-shifts.f1. The figure's printed final rows
// conflict with its own drawn mask by one bit; that is recorded, not
// graded (see fig64.s).
size_t count_accesses(const nmh::NanomigHarness& h, uint32_t base_byte_addr, size_t bound, bool want_write) {
    uint32_t base_word = base_byte_addr / 2;
    auto log = nmh::coalesce(h.ram_log());
    size_t n = 0;
    for (const auto& a : log)
        if (a.is_write == want_write && a.word_addr >= base_word && a.word_addr < base_word + bound)
            ++n;
    return n;
}

void assertion_A15(const std::string& dir) {
    Run r = run(dir, pick("A15-fig64", "fig64", "fig64_wrongwidth"));
    size_t b_reads = ok(r) ? count_accesses(*r.h, kSrc, 12, false) : 0;
    size_t d_writes = ok(r) ? count_accesses(*r.h, kDst, 12, true) : 0;
    checks.expect(ok(r) && b_reads == 6 && d_writes == 6,
        "A15-fig64 p181 6-shifts-and-masks.c32 ch6-shifts.f1: over 3 rows, "
        "channel B reads exactly 6 source words (2/row) and channel D "
        "writes exactly 6 destination words (2/row)");
    checks.record(
        "A15-fig64: the figure's drawn second-word mask is 7 bits "
        "(11111110 00000000, matching a 23-bit extraction) but its printed "
        "final rows take 8 bits from the source (row 1 \"00000000 "
        "11111111\", where the drawn second-word mask $FE00, applied to "
        "the figure's row 1 over a destination of ones, gives \"00000001 "
        "11111111\") -- a one-bit conflict between the drawing and the "
        "printed numbers that this package does not resolve, so the "
        "recipe is graded structurally (reads/writes per row), not "
        "against those exact final-row values");
}

// -----------------------------------------------------------------------
// A16 -- ch6-minterms.c8: p.175's worked example ("A and not C or B")
// names five minterms at BLTCON0 bit positions 6, 4, 7, 3, 2 -- LF=$DC.
void assertion_A16(const std::string& dir) {
    Run r = run(dir, pick("A16-mintdc", "mintdc", "mintdc_wrong"));
    bool matches = ok(r) && W(r, kDst) == 0xF3F0;
    checks.expect(matches,
        "A16-mintdc p175 ch6-minterms.c8: A=FF00, B=F0F0, C=CCCC with "
        "LF=$DC (bit positions 6,4,7,3,2 -- the worked example's five "
        "minterms) gives F3F0, the reference computed independently of "
        "the RTL");
}

// -----------------------------------------------------------------------
// A17 -- p.174's own worked example: D=A+B read bottom-up is LF=$FC.
// 6-function-generator.c24, ch6-funcgen.c6.
void assertion_A17(const std::string& dir) {
    Run r = run(dir, pick("A17-truthorder", "truthorder", "truthorder_topdown"));
    bool matches = ok(r) && W(r, kDst) == 0xFFF0;
    checks.expect(matches,
        "A17-truthorder p174 6-function-generator.c24 ch6-funcgen.c6: "
        "A=FF00, B=F0F0, D=A+B read from the bottom of the truth table up "
        "is LF=$FC, giving FFF0 -- the top-down (bit-reversed) reading "
        "would give a different function entirely");
}

// -----------------------------------------------------------------------
// A18 -- moving a sixteen-colour image needs four separate copy
// operations, one per plane. Page 171. 6-dma-channels.c36, ch6-dma.c34.
// Four separate plane bitmaps (not four adjacent words): a single blit's
// own row modulo can only step within the one bitmap it was given, never
// into a different plane's separate buffer, so one blit moves at most one
// plane's image.
void assertion_A18(const std::string& dir) {
    Run r = run(dir, pick("A18-fourplanes", "fourplanes", "fourplanes_only1"));
    auto plane_ok = [&](uint32_t bmp) {
        return W(r, bmp + 80 + 0) == 0x1111 && W(r, bmp + 80 + 2) == 0x2222 &&
               W(r, bmp + 80 + 4) == 0x3333 && W(r, bmp + 80 + 6) == 0x4444 &&
               W(r, bmp + 80 + 8) == 0x5555 && W(r, bmp + 80 + 10) == 0x6666 &&
               W(r, bmp + 80 + 12) == 0x7777;
    };
    bool matches = ok(r) && plane_ok(kPlaneBmp0) && plane_ok(kPlaneBmp1) &&
        plane_ok(kPlaneBmp2) && plane_ok(kPlaneBmp3);
    checks.expect(matches,
        "A18-fourplanes p171 6-dma-channels.c36 ch6-dma.c34: four separate "
        "plane bitmaps (40 bytes x 12 rows each), a 7-word x 6-row image "
        "embedded at row 2 of each; one blit's row modulo (40-2*7=26 "
        "bytes) only steps within its own bitmap, so moving all four "
        "planes' images takes four separate blits, one pointer pair each "
        "-- all four bitmaps show the image only once all four blits have "
        "run");
}

// -----------------------------------------------------------------------
// A19 -- some of a pointer's most significant bits are ignored, because
// only Chip memory is accessible, and a bigger Chip ignores fewer of
// them (p.170). The harness's configured Chip memory is 2 MB
// (Config.memory_config defaults to 0x03; nanomig_harness.h), i.e. bits
// ignored = 32 - log2(chip size) = 32 - 21 = 11 -- so bit 20 is the top
// of the honoured range and bit 21 is the lowest ignored bit, for this
// configured size. (Appendix C, p.309, "BLTxPTH ... (high 5 bits, was 3
// bits)" corroborates the same bit 20/21 edge, but only because the
// harness runs the Enhanced Chip Set: Config.chipset_config defaults to
// 0x08, and p.309's "high 5 bits" is stated to hold "when the Enhanced
// Chip Set is present" -- under OCS the same page would predict bit 20
// ignored too.) Page 170, 309. 6-dma-channels.c21, ch6-dma.c19.
void assertion_A19(const std::string& dir) {
    Run r = run(dir, pick("A19-highbit", "highbit", "highbit_bit20"));
    bool matches = ok(r) &&
        W(r, kDst + 0) == 0x1234 && W(r, kDst + 2) == 0x5678 &&
        W(r, kDst + 4) == 0x9ABC && W(r, kDst + 6) == 0xDEF0;
    checks.expect(matches,
        "A19-highbit p170 6-dma-channels.c21 ch6-dma.c19: with the "
        "harness's configured 2 MB of Chip memory, BLTDPT set to DST with "
        "bit 21 also set (DST|$00200000, above the top of the honoured "
        "range for 2 MB) still writes at DST -- corroborated by Appendix "
        "C, p.309, for the harness's configured ECS chipset");
}

// -----------------------------------------------------------------------
// A20 -- 512 KB Chip RAM ignores 13 pointer bits (6-dma-channels.c22,
// ch6-dma.c20); more Chip memory ignores fewer (6-dma-channels.c23,
// ch6-dma.c21). p.170: the pointer is composed of two words (32 bits);
// on 512 KB Chip memory the most significant 13 are ignored, leaving 19
// -- i.e. the pointer wraps modulo $80000. Both chip sizes are graded in
// the same, normal run: under 512 KB, DST=$086000 wraps to $006000 (bit
// 19 dropped), DST+$040000=$0C6000 wraps to $046000 (bit 18 kept, so 13
// bits, not 14, are what is ignored), and DST+$100002=$186002 wraps to
// $006002 (bit 20 dropped too); under 2 MB, DST is honoured directly,
// unwrapped. (The CPU-side program placement under 512 KB -- physical
// $000400 for logical $080400 -- is harness plumbing the page does not
// describe, not part of the oracle; load_program()/peek_logical() assume
// the 2 MB identity map and cannot be used for it, so this case pokes the
// assembled program directly with poke_word()/peek_word(), a harness gap,
// not a change to the harness.)
void assertion_A20(const std::string& dir) {
    // 512 KB: three pointers distinguish 13 ignored bits from 14, or from
    // just bit 19.
    nmh::Config cfg0;
    cfg0.memory_config = 0x00;
    auto h0 = std::make_unique<nmh::NanomigHarness>(cfg0);
    size_t loaded0 = load_program_collapsed(*h0, dir + "/mem512.bin");
    h0->boot(kBootBudget);
    auto done0 = h0->run_until_write(kDoneAddr - kChipCollapseBase, kDoneValue, kBootBudget + kRunBudget);
    bool half_512k = loaded0 > 0 && done0.has_value() &&
        h0->peek_word(0x006000 / 2) == 0xAAAA &&
        h0->peek_word(0x046000 / 2) == 0xBBBB &&
        h0->peek_word(0x006002 / 2) == 0xCCCC;

    // 2 MB: the same program, DST honoured directly. The mutation swaps
    // this half to memory_config 0 too, so DST (unwrapped) is never
    // written and this half goes red on its own.
    bool mutated_2mb = mutating("A20-mem512");
    nmh::Config cfg3;
    cfg3.memory_config = mutated_2mb ? 0x00 : 0x03;
    auto h3 = std::make_unique<nmh::NanomigHarness>(cfg3);
    size_t loaded3;
    std::optional<uint64_t> done3;
    if (mutated_2mb) {
        loaded3 = load_program_collapsed(*h3, dir + "/mem512.bin");
        h3->boot(kBootBudget);
        done3 = h3->run_until_write(kDoneAddr - kChipCollapseBase, kDoneValue, kBootBudget + kRunBudget);
    } else {
        loaded3 = h3->load_program(dir + "/mem512.bin");
        h3->boot(kBootBudget);
        done3 = h3->run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    }
    bool half_2mb = loaded3 > 0 && done3.has_value() && h3->peek_word(kDst / 2) == 0xAAAA;

    checks.expect(half_512k && half_2mb,
        "A20-mem512 p170 6-dma-channels.c22 6-dma-channels.c23 ch6-dma.c20 "
        "ch6-dma.c21: a 32-bit pointer wraps modulo $80000 under 512 KB "
        "Chip memory (the top 13 of its 32 bits ignored) -- DST=$086000 "
        "wraps to $006000, DST+$040000=$0C6000 wraps to $046000 (bit 18 "
        "kept, so exactly 13 bits, not 14), DST+$100002=$186002 wraps to "
        "$006002 (bit 20 dropped too) -- while under 2 MB Chip memory the "
        "same DST is honoured directly, unwrapped: a larger chip ignores "
        "fewer bits");
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);

    assertion_A0(dir);
    assertion_A1(dir);
    assertion_A2(dir);
    assertion_A3(dir);
    assertion_A4(dir);
    assertion_A5(dir);
    assertion_A6(dir);
    assertion_A7a(dir);
    assertion_A7b(dir);
    assertion_A8a(dir);
    assertion_A8b(dir);
    assertion_A8c(dir);
    assertion_A9(dir);
    assertion_A9b(dir);
    assertion_A10(dir);
    assertion_A11(dir);
    assertion_A12(dir);
    assertion_A13(dir);
    assertion_A14(dir);
    assertion_A15(dir);
    assertion_A16(dir);
    assertion_A17(dir);
    assertion_A18(dir);
    assertion_A19(dir);
    assertion_A20(dir);

    if (!g_mutate) {
        checks.record(
            "ch6-copy.c10's display clause (\"a lower memory address is "
            "higher on the display\") is not measured by A7b, which is "
            "memory-only: settling it needs a bitplane fixture (BPLCON0 "
            "BPU>=1, DDFSTRT/DDFSTOP, video()) showing lower-address rows "
            "on earlier r/g/b lines -- not built in this package; scope "
            "decision, not unfalsifiable.");
        checks.record(
            "6-memory-layout.c5-c9, ch6-memlayout.c3-c7 (a 320x200 16-color "
            "screen's four-bitplane, 8000-byte, 200-row, 40-byte/20-word "
            "layout) are display-memory arithmetic: bitplane DMA addresses "
            "on the RAM port, and r/g/b, do encode both resolution and "
            "colour depth, so this is not unfalsifiable -- but settling it "
            "needs a display fixture (BPLCON0 BPU=4 lowres, DDFSTRT=$0038, "
            "DDFSTOP=$00D0, 200 lines, BPLxMOD=0: each plane fetched as 20 "
            "words/line over 200 lines, one contiguous 8000-byte range; "
            "pixel values 0-15 showing 16 distinct colours) not built in "
            "this package; scope decision.");
        checks.record(
            "6-dma-channels.c35, ch6-dma.c33 (\"a 16-color image needs four "
            "bitplanes\") is the same display-fixture half of the bit-depth "
            "corollary as the memory-layout ids above; A18 grades the "
            "\"four copy operations\" half without a display fixture. "
            "Scope decision, not unfalsifiable.");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
