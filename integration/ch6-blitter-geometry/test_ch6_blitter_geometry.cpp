// test_ch6_blitter_geometry.cpp
//
// Integration package ch6-blitter-geometry: see SPECIFICATION.md. Every
// program drives nanomig.v only through the blitter's custom registers
// (integration/harness/nanomig.i) and is graded only against
// peek_logical() and ram_log() -- nanomig_harness.h's ports-only surface.
//
// MUTATION A0-fixture stimulus red withhold the BLTSIZE write
// MUTATION A1-row-masks stimulus red use a different FWM value
// MUTATION A2-plus-one stimulus red add a 2-word destination modulo
// MUTATION A3-embedded-modulo stimulus red use a 20-byte modulo
// MUTATION A4-cookiecut stimulus red use function $AC instead of $CA
// MUTATION A5-read-before-write stimulus red point channel C somewhere else
// MUTATION A6-shift-decomposition stimulus red advance the pointer by 5 words instead of 6
// MUTATION A7a-descending-overlap stimulus red leave the pointers at the block's first word in the descending run
// MUTATION A7b-ascending-overlap stimulus red use descending mode for the same one-row-up move
// MUTATION A8a-dest-grows stimulus red set blit size to 2 instead of 3
// MUTATION A8b-source-larger stimulus red set blit size to 2 instead of 3
// MUTATION A9-minterm-bit stimulus red use LF = $40 instead of $80
// MUTATION A10-minterm-or stimulus red use LF = $80 instead of $C0
// MUTATION A11-word-only stimulus red point the pointer's even part at the next word

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <iostream>

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
constexpr uint32_t kScratch = 0x082000;
constexpr uint32_t kSrc     = kScratch + 0x0000;
constexpr uint32_t kSrc2    = kScratch + 0x0800;
constexpr uint32_t kMaskbuf = kScratch + 0x1000;
constexpr uint32_t kDst     = kScratch + 0x4000;
constexpr uint32_t kDst2    = kScratch + 0x4800;
constexpr uint32_t kBitmap  = kScratch + 0x6000;

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

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%04X", v);
    return buf;
}

bool ok(const Run& r) { return r.loaded > 0 && r.released && r.done.has_value(); }

// -----------------------------------------------------------------------
// A0 -- fixture: registers set up, BLTSIZE last, known result delivered.
// Pages 169, 200. ch6-intro.c20, ch6-keypoints.c1.
void assertion_A0(const std::string& dir) {
    Run normal = run(dir, "fixture");
    bool matches = ok(normal) &&
        W(normal, kDst + 0) == 0x1234 && W(normal, kDst + 2) == 0x5678 &&
        W(normal, kDst + 4) == 0x9ABC && W(normal, kDst + 6) == 0xDEF0;
    checks.expect(matches,
        "A0-fixture p169,200 ch6-intro.c20 ch6-keypoints.c1: "
        "registers set up and BLTSIZE written last deliver the known source "
        "pattern (1234 5678 9ABC DEF0) to the destination");

    Run withheld = run(dir, "fixture_nobltsize");
    bool unchanged = withheld.loaded > 0 && withheld.done.has_value() &&
        W(withheld, kDst + 0) == 0x0000 && W(withheld, kDst + 2) == 0x0000 &&
        W(withheld, kDst + 4) == 0x0000 && W(withheld, kDst + 6) == 0x0000;
    checks.expect(unchanged,
        "A0-fixture p169,200 ch6-keypoints.c1: CONTROL withholding the "
        "BLTSIZE write leaves the destination at its pre-blit seed "
        "(mutation A0-fixture)");
}

// -----------------------------------------------------------------------
// A1 -- BLTAFWM/BLTALWM mask the first/last word of every raster row.
// Page 169. 6-intro.c13, 6-intro.c14, ch6-intro.c12, ch6-intro.c13.
void assertion_A1(const std::string& dir) {
    Run normal = run(dir, "rowmask");
    bool matches = ok(normal) &&
        W(normal, kDst + 0) == 0x00FF && W(normal, kDst + 2) == 0xFFFF && W(normal, kDst + 4) == 0xFF00 &&
        W(normal, kDst + 6) == 0x00FF && W(normal, kDst + 8) == 0xFFFF && W(normal, kDst + 10) == 0xFF00;
    checks.expect(matches,
        "A1-row-masks p169 6-intro.c13 6-intro.c14 ch6-intro.c12 ch6-intro.c13: "
        "on both rows of a 3-word/row, 2-row copy, word 0 is FWM-masked "
        "(00FF), word 1 is unmasked (FFFF) and word 2 is LWM-masked (FF00)");

    Run mutated = run(dir, "rowmask_wrongfwm");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        W(mutated, kDst + 0) != 0x00FF;
    checks.expect(diverges,
        "A1-row-masks p169 ch6-intro.c12: CONTROL a different BLTAFWM value "
        "changes word 0 of row 0 away from the value the check expects "
        "(mutation A1-row-masks)");
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
    Run normal = run(dir, "rastercont");
    bool contiguous = ok(normal) && contiguous_run(*normal.h, kDst, 21);
    checks.expect(contiguous,
        "A2-plus-one p171 ch6-dma.c30 ch6-dma.c31: with BLTAMOD=BLTDMOD=0, "
        "the 21 destination writes of a 7-word/row, 3-row copy form one "
        "unbroken run of consecutive word addresses, no gap at either row "
        "boundary");

    Run mutated = run(dir, "rastercont_mod");
    bool gap = mutated.loaded > 0 && mutated.done.has_value() &&
        !contiguous_run(*mutated.h, kDst, 21);
    checks.expect(gap,
        "A2-plus-one p171 ch6-dma.c30: CONTROL a 2-word destination modulo "
        "breaks the unbroken-run check (mutation A2-plus-one)");
}

// -----------------------------------------------------------------------
// A3 -- embedded-rectangle modulo arithmetic: 2*width(words) + modulo
// (bytes) = row byte width. Page 172. 6-dma-channels.c65.
bool embedded_rect_ok(const Run& r) {
    if (!ok(r)) return false;
    // 3 rows of 40 bytes (20 words); rectangle at byte column 12, 5 words
    // (10 bytes) wide.
    static const uint16_t kSrcWords[15] = {
        0x1111, 0x2222, 0x3333, 0x4444, 0x5555,
        0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA,
        0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF,
    };
    for (int row = 0; row < 3; ++row) {
        for (int word = 0; word < 20; ++word) {
            uint32_t addr = kBitmap + row * 40 + word * 2;
            uint16_t v = W(r, addr);
            bool inRect = word >= 6 && word < 11;  // bytes 12..21
            if (inRect) {
                if (v != kSrcWords[row * 5 + (word - 6)]) return false;
            } else {
                if (v != 0xDEAD) return false;
            }
        }
    }
    return true;
}

void assertion_A3(const std::string& dir) {
    Run normal = run(dir, "embedmod");
    checks.expect(embedded_rect_ok(normal),
        "A3-embedded-modulo p172 6-dma-channels.c65: with BLTDMOD=30 "
        "(2*5 words + 30 bytes = 40 bytes), a copy into the embedded "
        "5-word rectangle of a 40-byte/row, 3-row bitmap lands the source "
        "on every row's rectangle and leaves the 12-byte left / 18-byte "
        "right margins at their sentinel on every row");

    Run mutated = run(dir, "embedmod_wrongmod");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() && !embedded_rect_ok(mutated);
    checks.expect(diverges,
        "A3-embedded-modulo p172 6-dma-channels.c65: CONTROL a 20-byte "
        "modulo instead of 30 lands rows at the wrong offset and the "
        "margin check fails (mutation A3-embedded-modulo)");
}

// -----------------------------------------------------------------------
// A4 -- cookie-cut mask is one bitplane regardless of background depth,
// function $CA. Page 176, 183. 6-function-generator.c40, ch6-minterms.c9,
// ch6-copy.c6.
void assertion_A4(const std::string& dir) {
    Run normal = run(dir, "cookiecut");
    bool run1 = ok(normal) &&
        W(normal, kDst + 0) == 0xA5A5 && W(normal, kDst + 2) == 0xA5A5 &&
        W(normal, kDst + 4) == 0xA5A5 && W(normal, kDst + 6) == 0xA5A5;
    bool run2 = ok(normal) &&
        W(normal, kDst2 + 0) == 0xA3A3 && W(normal, kDst2 + 2) == 0xA3A3 &&
        W(normal, kDst2 + 4) == 0xA3A3 && W(normal, kDst2 + 6) == 0xA3A3;
    checks.note("A4 actual DST words=" + hex4(W(normal, kDst + 0)) + " " +
        hex4(W(normal, kDst + 2)) + " " + hex4(W(normal, kDst + 4)) + " " +
        hex4(W(normal, kDst + 6)) + "; DST2 words=" + hex4(W(normal, kDst2 + 0)) +
        " " + hex4(W(normal, kDst2 + 2)) + " " + hex4(W(normal, kDst2 + 4)) +
        " " + hex4(W(normal, kDst2 + 6)) + " (expected A5A5.. and A3A3..)");
    checks.expect(run1 && run2,
        "A4-cookiecut p176,183 6-function-generator.c40 ch6-minterms.c9 "
        "ch6-copy.c6: with LF=$CA, D=(A AND B) OR (NOT A AND C); the same "
        "mask (A=F0F0) and object (B=AAAA) cut correctly against two "
        "different backgrounds (C=5555 giving A5A5, C=3333 giving A3A3) "
        "without the mask itself changing");

    Run mutated = run(dir, "cookiecut_wronglf");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        (W(mutated, kDst + 0) != 0xA5A5 || W(mutated, kDst2 + 0) != 0xA3A3);
    checks.expect(diverges,
        "A4-cookiecut p176,183 ch6-copy.c6: CONTROL function $AC instead "
        "of $CA no longer matches the mask?object:background reference "
        "(mutation A4-cookiecut)");
}

// -----------------------------------------------------------------------
// A5 -- the destination background is fetched (read) before it is
// written. Page 176. ch6-minterms.c12.
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

void assertion_A5(const std::string& dir) {
    Run normal = run(dir, "cookiecut");
    bool rbw = ok(normal) && read_before_write_all(*normal.h, kDst, 4);
    checks.expect(rbw,
        "A5-read-before-write p176 ch6-minterms.c12: on the RAM port, a "
        "read of every destination word (channel C, pointed at the same "
        "address as D) precedes its write");

    Run mutated = run(dir, "cookiecut_celsewhere");
    bool no_preceding_read = mutated.loaded > 0 && mutated.done.has_value() &&
        !read_before_write_all(*mutated.h, kDst, 4);
    checks.expect(no_preceding_read,
        "A5-read-before-write p176 ch6-minterms.c12: CONTROL pointing "
        "channel C elsewhere leaves no read at the destination address "
        "before its write (mutation A5-read-before-write)");
}

// -----------------------------------------------------------------------
// A6 -- a shift over 15 bits is a whole-word destination-pointer
// displacement plus a residual barrel shift. Page 179.
// 6-shifts-and-masks.c10/c11/c12/c13, ch6-shifts.c10/c11/c12.
void assertion_A6(const std::string& dir) {
    Run normal = run(dir, "shift100");
    bool matches = ok(normal) &&
        W(normal, kDst + 12) == 0x0111 && W(normal, kDst + 14) == 0x1222 &&
        W(normal, kDst + 16) == 0x2333;
    checks.expect(matches,
        "A6-shift-decomposition p179 6-shifts-and-masks.c10 "
        "6-shifts-and-masks.c11 6-shifts-and-masks.c12 6-shifts-and-masks.c13 "
        "ch6-shifts.c10 ch6-shifts.c11 ch6-shifts.c12: a 100-bit right "
        "shift (6-word/12-byte destination-pointer advance plus a 4-bit "
        "residual shift, ASH=4) of source words 1111 2222 3333 gives "
        "destination words 0111 1222 2333, the reference computed "
        "independently of the RTL");

    Run mutated = run(dir, "shift100_wrongadvance");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        (W(mutated, kDst + 12) != 0x0111 || W(mutated, kDst + 14) != 0x1222 ||
         W(mutated, kDst + 16) != 0x2333);
    checks.expect(diverges,
        "A6-shift-decomposition p179 6-shifts-and-masks.c11: CONTROL a "
        "5-word pointer advance instead of 6 no longer matches the 100-bit "
        "shift reference (mutation A6-shift-decomposition)");
}

// -----------------------------------------------------------------------
// A7a -- descending mode moves an overlapping image down correctly.
// Page 182. 6-descending-mode.c9, ch6-desc.c1, ch6-desc.c10, ch6-desc.c11.
void assertion_A7a(const std::string& dir) {
    Run asc = run(dir, "overlap_down_asc");
    bool asc_clean = ok(asc) &&
        W(asc, kSrc + 4) == 0x1111 && W(asc, kSrc + 6) == 0x2222 &&
        W(asc, kSrc + 8) == 0x3333 && W(asc, kSrc + 10) == 0x4444 &&
        W(asc, kSrc + 12) == 0x5555 && W(asc, kSrc + 14) == 0x6666;
    checks.expect(ok(asc) && !asc_clean,
        "A7a-ascending-corrupts p182 6-descending-mode.c9 ch6-desc.c1 "
        "ch6-desc.c10 ch6-desc.c11: moving a 3-row block one row down in "
        "place with a standard (ascending) copy does not match a clean "
        "one-row-down shift -- the later rows are overwritten before they "
        "are copied");

    Run desc = run(dir, "overlap_down_desc");
    bool desc_clean = ok(desc) &&
        W(desc, kSrc + 4) == 0x1111 && W(desc, kSrc + 6) == 0x2222 &&
        W(desc, kSrc + 8) == 0x3333 && W(desc, kSrc + 10) == 0x4444 &&
        W(desc, kSrc + 12) == 0x5555 && W(desc, kSrc + 14) == 0x6666;
    checks.expect(desc_clean,
        "A7a-descending-overlap p182 6-descending-mode.c9 ch6-desc.c1 "
        "ch6-desc.c10 ch6-desc.c11: descending mode, pointers at each "
        "block's last word, the same modulo and blit size as the ascending "
        "run, matches a clean one-row-down shift");

    Run bad = run(dir, "overlap_down_desc_bad");
    bool bad_clean = ok(bad) &&
        W(bad, kSrc + 4) == 0x1111 && W(bad, kSrc + 6) == 0x2222 &&
        W(bad, kSrc + 8) == 0x3333 && W(bad, kSrc + 10) == 0x4444 &&
        W(bad, kSrc + 12) == 0x5555 && W(bad, kSrc + 14) == 0x6666;
    checks.expect(ok(bad) && !bad_clean,
        "A7a-descending-overlap p182 ch6-desc.c10: CONTROL DESC=1 with the "
        "pointers left at the block's first word no longer matches the "
        "clean shift (mutation A7a-descending-overlap)");
}

// -----------------------------------------------------------------------
// A7b -- control: ascending mode moves an overlapping image up correctly.
// Page 183. ch6-copy.c10.
void assertion_A7b(const std::string& dir) {
    Run asc = run(dir, "overlap_up_asc");
    bool asc_clean = ok(asc) &&
        W(asc, kSrc + 0) == 0xAAAA && W(asc, kSrc + 2) == 0xBBBB &&
        W(asc, kSrc + 4) == 0xCCCC && W(asc, kSrc + 6) == 0xDDDD &&
        W(asc, kSrc + 8) == 0xEEEE && W(asc, kSrc + 10) == 0xFFFF;
    checks.expect(asc_clean,
        "A7b-ascending-overlap p183 ch6-copy.c10: CONTROL ascending copy "
        "of an overlapping one-row-up move (destination at the lower "
        "address) matches a clean shift -- the package's control case, on "
        "the same overlap/measurement basis as A7a");

    Run desc = run(dir, "overlap_up_desc");
    bool desc_clean = ok(desc) &&
        W(desc, kSrc + 0) == 0xAAAA && W(desc, kSrc + 2) == 0xBBBB &&
        W(desc, kSrc + 4) == 0xCCCC && W(desc, kSrc + 6) == 0xDDDD &&
        W(desc, kSrc + 8) == 0xEEEE && W(desc, kSrc + 10) == 0xFFFF;
    checks.expect(ok(desc) && !desc_clean,
        "A7b-ascending-overlap p183 ch6-copy.c10: CONTROL descending mode "
        "on the same one-row-up move reads an already-overwritten row and "
        "no longer matches the clean shift (mutation A7b-ascending-overlap)");
}

// -----------------------------------------------------------------------
// A8a -- arbitrary-region copy, destination spans more words than
// source. Page 183. 6-copying-arbitrary-regions.c1, ch6-copy.c1,
// ch6-copy.c2, ch6-copy.c13, ch6-copy.c14, ch6-copy.c4, ch6-copy.c9,
// ch6-copy.c5.
void assertion_A8a(const std::string& dir) {
    Run normal = run(dir, "arbgrow");
    bool matches = ok(normal) &&
        W(normal, kDst + 0) == 0x000F && W(normal, kDst + 2) == 0xFFFF &&
        W(normal, kDst + 4) == 0xE000;
    checks.expect(matches,
        "A8a-dest-grows p183 6-copying-arbitrary-regions.c1 ch6-copy.c1 "
        "ch6-copy.c2 ch6-copy.c13 ch6-copy.c14 ch6-copy.c4 ch6-copy.c9 "
        "ch6-copy.c5: a 23-bit-wide pattern (2 source words) shifted right "
        "12 bits spans 3 destination words (000F FFFF E000, the reference "
        "computed independently of the RTL); blit size is the larger span "
        "3, ASH=0, and BLTAFWM/BLTALWM (000F/E000) mask channel A as if "
        "masking the destination's own edges, clipping the real neighbour "
        "word fetched past the true source out of the trailing word");

    Run mutated = run(dir, "arbgrow_wrongsize");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        W(mutated, kDst + 4) != 0xE000;
    checks.expect(diverges,
        "A8a-dest-grows p183 ch6-copy.c4: CONTROL blit size 2 instead of 3 "
        "never writes the third destination word (mutation A8a-dest-grows)");
}

// -----------------------------------------------------------------------
// A8b -- arbitrary-region copy, source spans more words than the
// destination's true data. Page 183. 6-copying-arbitrary-regions.c2,
// ch6-copy.c3, ch6-copy.c11, ch6-copy.c12, ch6-copy.c4, ch6-copy.c5.
void assertion_A8b(const std::string& dir) {
    Run normal = run(dir, "arbshrink");
    bool matches = ok(normal) &&
        W(normal, kDst + 0) == 0x0000 && W(normal, kDst + 2) == 0xFFFF &&
        W(normal, kDst + 4) == 0xF000;
    checks.expect(matches,
        "A8b-source-larger p183 6-copying-arbitrary-regions.c2 ch6-copy.c3 "
        "ch6-copy.c11 ch6-copy.c12 ch6-copy.c4 ch6-copy.c5: a 3-word source "
        "(20 true bits) shifted right 2 bits compacts its meaningful "
        "content into the second and third destination words (0000 FFFF "
        "F000, the reference computed independently of the RTL); the A "
        "channel fetches the same source as B with the same shift value "
        "(ASH=BSH=2) so D=A AND B reproduces B masked as if masking the B "
        "source data; blit size is still the larger span, 3 (the source's "
        "own), not the 2 the true data alone would need");

    Run mutated = run(dir, "arbshrink_wrongsize");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        W(mutated, kDst + 4) != 0xF000;
    checks.expect(diverges,
        "A8b-source-larger p183 ch6-copy.c4: CONTROL blit size 2 instead "
        "of 3 never runs the third fetch that produces the second real "
        "destination word (mutation A8b-source-larger)");
}

// -----------------------------------------------------------------------
// A9 -- minterm n is bit n of the LF byte. Page 178, 179. ch6-venn.c2,
// ch6-venn.c8.
void assertion_A9(const std::string& dir) {
    Run normal = run(dir, "mintbit");
    bool matches = ok(normal) && W(normal, kDst) == 0xC000 && W(normal, kDst2) == 0x00FF;
    checks.note("A9 actual DST(LF=$80)=" + hex4(W(normal, kDst)) +
        " DST2(LF=$0F)=" + hex4(W(normal, kDst2)) +
        " (expected C000 and 00FF)");
    checks.expect(matches,
        "A9-minterm-bit p178,179 ch6-venn.c2 ch6-venn.c8: with A=FF00, "
        "B=F0F0, C=CCCC, LF=$80 (minterm 7 alone) gives A AND B AND C = "
        "C000, and LF=$0F (minterms 0-3) gives NOT A = 00FF");

    Run mutated = run(dir, "mintbit_wrongbit");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        W(mutated, kDst) != 0xC000;
    checks.expect(diverges,
        "A9-minterm-bit p178 ch6-venn.c2: CONTROL LF=$40 (minterm 6) "
        "instead of $80 (minterm 7) no longer equals A AND B AND C "
        "(mutation A9-minterm-bit)");
}

// -----------------------------------------------------------------------
// A10 -- minterms combine by OR-ing their LF bits. Page 178, 179.
// ch6-venn.c5, ch6-venn.c11.
void assertion_A10(const std::string& dir) {
    Run normal = run(dir, "mintor");
    bool matches = ok(normal) && W(normal, kDst) == 0xF000;
    checks.expect(matches,
        "A10-minterm-or p178,179 ch6-venn.c5 ch6-venn.c11: with A=FF00, "
        "B=F0F0, C=CCCC, LF=$C0 (minterms 7 and 6 OR'ed together) gives A "
        "AND B, regardless of C = F000");

    Run mutated = run(dir, "mintor_missing");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        W(mutated, kDst) != 0xF000;
    checks.expect(diverges,
        "A10-minterm-or p178 ch6-venn.c5: CONTROL LF=$80 (minterm 7 only) "
        "instead of $C0 loses the AB~C case and no longer equals A AND B "
        "(mutation A10-minterm-or)");
}

// -----------------------------------------------------------------------
// A11 -- the blitter operates on words: a pointer's low bit is ignored.
// Page 172. 6-dma-channels.c79.
void assertion_A11(const std::string& dir) {
    Run normal = run(dir, "wordonly");
    bool matches = ok(normal) && W(normal, kDst) == 0xABCD;
    checks.expect(matches,
        "A11-word-only p172 6-dma-channels.c79: BLTAPT set to SRC+1 (an "
        "odd byte address) fetches the word at the even, LSB-truncated "
        "address (ABCD), not the next word");

    Run mutated = run(dir, "wordonly_wrongword");
    bool diverges = mutated.loaded > 0 && mutated.done.has_value() &&
        W(mutated, kDst) != 0xABCD;
    checks.expect(diverges,
        "A11-word-only p172 6-dma-channels.c79: CONTROL BLTAPT set to "
        "SRC+3 (still odd, truncates to SRC+2) fetches the next word "
        "(1234) instead, which no longer equals ABCD "
        "(mutation A11-word-only)");
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));

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
    assertion_A9(dir);
    assertion_A10(dir);
    assertion_A11(dir);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
