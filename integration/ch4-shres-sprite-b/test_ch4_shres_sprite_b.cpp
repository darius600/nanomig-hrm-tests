// test_ch4_shres_sprite_b.cpp
//
// ch4-shres-sprite-b: see SPECIFICATION.md.
//
// One 68000 program (prog_main.s, or a mutant wrapper selected by
// NMH_MUTATE) drives sprites 0 and 1 through the SuperHires colour
// encoding of p.302 for four bands of pattern lines, plus two pattern
// regions used only to confirm SuperHires is in effect (PC1). This test
// finds the graded frame from the video trace alone (v0/a0/a1/d,
// SPECIFICATION.md section 4), grades P1-P4 per letter from masked
// samples, and checks PC0-PC4 from the same trace plus the RAM-port log.
//
// NMH_MUTATE selects one contrast in place of the baseline stimulus; none
// is exercised by a plain `make test`.
//   NMH_MUTATE=CR1  prog_cr1.bin  (section-T COLOR16-31 = S2 instead of S0)
//   NMH_MUTATE=CR2  prog_cr2.bin  (section-T COLOR16-31 = S1 instead of S0)
//   NMH_MUTATE=CR3  prog_cr3.bin  (band-4 COLOR16-31 = S0-c18 instead of S0x)
//   NMH_MUTATE=CR4  prog_cr4.bin  (BPLCON0 = $9000 instead of $1040)
//   NMH_MUTATE=CR5  prog_cr5.bin  (DMACON = $8380, SPREN not set)
//   NMH_MUTATE=CR6  prog_cr6.bin  (band-1 WAIT = $5C07 instead of $5807)
//   NMH_MUTATE=CR7  prog_main.bin (chipset_config selects OCS instead of ECS)
//   NMH_MUTATE=CR8  prog_cr8.bin  (MARKER written after 1 frame start, not 6)
//
// MUTATION P1-letters-bit3 stimulus red S2 instead of S0 in band 0, via NMH_MUTATE=CR1
// MUTATION P1-letters-bit2 stimulus red S1 instead of S0 in band 0, via NMH_MUTATE=CR2
// MUTATION P4-colour18 stimulus red S0-c18 instead of S0x in band 4, via NMH_MUTATE=CR3
// MUTATION PC1 stimulus red BPLCON0=$9000 (HIRES) instead of $1040 (SHRES), via NMH_MUTATE=CR4
// MUTATION PC2 stimulus red DMACON=$8380 (SPREN clear) instead of $83A0, via NMH_MUTATE=CR5
// MUTATION PC3 stimulus red band-1 WAIT=$5C07 (line 92) instead of $5807 (line 88), via NMH_MUTATE=CR6
// MUTATION PC0 stimulus red chipset_config selects OCS instead of ECS, via NMH_MUTATE=CR7
// MUTATION PC4 stimulus red FRAME_COUNT=1 instead of 6, via NMH_MUTATE=CR8

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <iostream>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

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

// ---- constants mirroring sprb.i / nanomig.i --------------------------
constexpr uint32_t kResults      = 0x0C0000;
constexpr uint32_t kMarker       = 0x011500;
constexpr uint16_t kMarkerVal    = 0xA5A5;
constexpr uint32_t kCoplist      = 0x010000;
constexpr uint32_t kSpr0Dat      = 0x011000;
constexpr uint32_t kSpr1Dat      = 0x011200;

constexpr uint32_t kCoplistWordBase = kCoplist / 2;
constexpr uint32_t kSpr0WordBase    = kSpr0Dat / 2;
constexpr uint32_t kSpr1WordBase    = kSpr1Dat / 2;
constexpr int kCopWords  = 306;
constexpr int kSprWords  = 104;   // POS,CTL + 100 descriptor words + 2 terminator

constexpr int kOffColor0_15a      = 36;
constexpr int kOffColor16_31_T    = 68;
constexpr int kOffColor0_15dark   = 102;
constexpr int kOffColor16_31_S1   = 136;
constexpr int kOffColor16_31_S2   = 170;
constexpr int kOffColor16_31_S3   = 204;
constexpr int kOffColor16_31_band4 = 238;
constexpr int kOffWaitFinal       = 304;

constexpr uint64_t kBootBudget    = 4'000'000;
constexpr uint64_t kChunk         = 200'000;
constexpr uint64_t kMaxAfterBoot  = 16'000'000;
constexpr int kFramesAfterMarker  = 2;

enum class Scenario { Base, CR1, CR2, CR3, CR4, CR5, CR6, CR7, CR8 };

Scenario parse_scenario(const std::string& v) {
    if (v == "CR1") return Scenario::CR1;
    if (v == "CR2") return Scenario::CR2;
    if (v == "CR3") return Scenario::CR3;
    if (v == "CR4") return Scenario::CR4;
    if (v == "CR5") return Scenario::CR5;
    if (v == "CR6") return Scenario::CR6;
    if (v == "CR7") return Scenario::CR7;
    if (v == "CR8") return Scenario::CR8;
    return Scenario::Base;
}

std::string bin_for(Scenario sc) {
    switch (sc) {
        case Scenario::CR1: return "prog_cr1.bin";
        case Scenario::CR2: return "prog_cr2.bin";
        case Scenario::CR3: return "prog_cr3.bin";
        case Scenario::CR4: return "prog_cr4.bin";
        case Scenario::CR5: return "prog_cr5.bin";
        case Scenario::CR6: return "prog_cr6.bin";
        case Scenario::CR8: return "prog_cr8.bin";
        default: return "prog_main.bin"; // Base and CR7 share the baseline program
    }
}

std::string hex4(uint16_t v) { char b[8]; std::snprintf(b, sizeof b, "%04X", v); return b; }

// ---- Copper list mirror (sprb.i's COPTAB, literal) --------------------
std::vector<uint16_t> build_expected_coptab(bool cr1, bool cr2, bool cr3, bool cr6) {
    std::vector<uint16_t> t;
    auto add = [&](std::initializer_list<uint16_t> vals) { for (auto v : vals) t.push_back(v); };

    add({0x0120,0x0001, 0x0122,0x1000});
    add({0x0124,0x0001, 0x0126,0x1200});
    add({0x0128,0x0001, 0x012A,0x1400});
    add({0x012C,0x0001, 0x012E,0x1400});
    add({0x0130,0x0001, 0x0132,0x1400});
    add({0x0134,0x0001, 0x0136,0x1400});
    add({0x0138,0x0001, 0x013A,0x1400});
    add({0x013C,0x0001, 0x013E,0x1400});
    add({0x00E0,0x0002, 0x00E2,0x0000});

    add({0x0180,0x0000, 0x0182,0x0CCC, 0x0184,0x0000, 0x0186,0x0000});
    add({0x0188,0x0333, 0x018A,0x0FFF, 0x018C,0x0333, 0x018E,0x0333});
    add({0x0190,0x0000, 0x0192,0x0CCC, 0x0194,0x0000, 0x0196,0x0000});
    add({0x0198,0x0000, 0x019A,0x0CCC, 0x019C,0x0000, 0x019E,0x0000});

    if (cr1) {
        add({0x01A0,0x0AF0, 0x01A2,0x0E34, 0x01A4,0x0278, 0x01A6,0x06BC});
        add({0x01A8,0x0BC1, 0x01AA,0x0F05, 0x01AC,0x0349, 0x01AE,0x078D});
        add({0x01B0,0x08D2, 0x01B2,0x0C16, 0x01B4,0x005A, 0x01B6,0x049E});
        add({0x01B8,0x09E3, 0x01BA,0x0D27, 0x01BC,0x016B, 0x01BE,0x05AF});
    } else if (cr2) {
        add({0x01A0,0x05AF, 0x01A2,0x09E3, 0x01A4,0x0D27, 0x01A6,0x016B});
        add({0x01A8,0x06BC, 0x01AA,0x0AF0, 0x01AC,0x0E34, 0x01AE,0x0278});
        add({0x01B0,0x078D, 0x01B2,0x0BC1, 0x01B4,0x0F05, 0x01B6,0x0349});
        add({0x01B8,0x049E, 0x01BA,0x08D2, 0x01BC,0x0C16, 0x01BE,0x005A});
    } else {
        add({0x01A0,0x005A, 0x01A2,0x049E, 0x01A4,0x08D2, 0x01A6,0x0C16});
        add({0x01A8,0x016B, 0x01AA,0x05AF, 0x01AC,0x09E3, 0x01AE,0x0D27});
        add({0x01B0,0x0278, 0x01B2,0x06BC, 0x01B4,0x0AF0, 0x01B6,0x0E34});
        add({0x01B8,0x0349, 0x01BA,0x078D, 0x01BC,0x0BC1, 0x01BE,0x0F05});
    }

    add({0x3A07,0xFFFE});

    add({0x0180,0x0000, 0x0182,0x0000, 0x0184,0x0000, 0x0186,0x0000});
    add({0x0188,0x0000, 0x018A,0x0000, 0x018C,0x0000, 0x018E,0x0000});
    add({0x0190,0x0000, 0x0192,0x0000, 0x0194,0x0000, 0x0196,0x0000});
    add({0x0198,0x0000, 0x019A,0x0000, 0x019C,0x0000, 0x019E,0x0000});

    if (cr6) add({0x5C07,0xFFFE}); else add({0x5807,0xFFFE});

    add({0x01A0,0x05AF, 0x01A2,0x09E3, 0x01A4,0x0D27, 0x01A6,0x016B});
    add({0x01A8,0x06BC, 0x01AA,0x0AF0, 0x01AC,0x0E34, 0x01AE,0x0278});
    add({0x01B0,0x078D, 0x01B2,0x0BC1, 0x01B4,0x0F05, 0x01B6,0x0349});
    add({0x01B8,0x049E, 0x01BA,0x08D2, 0x01BC,0x0C16, 0x01BE,0x005A});

    add({0x6207,0xFFFE});

    add({0x01A0,0x0AF0, 0x01A2,0x0E34, 0x01A4,0x0278, 0x01A6,0x06BC});
    add({0x01A8,0x0BC1, 0x01AA,0x0F05, 0x01AC,0x0349, 0x01AE,0x078D});
    add({0x01B0,0x08D2, 0x01B2,0x0C16, 0x01B4,0x005A, 0x01B6,0x049E});
    add({0x01B8,0x09E3, 0x01BA,0x0D27, 0x01BC,0x016B, 0x01BE,0x05AF});

    add({0x6C07,0xFFFE});

    add({0x01A0,0x0F05, 0x01A2,0x0349, 0x01A4,0x078D, 0x01A6,0x0BC1});
    add({0x01A8,0x0C16, 0x01AA,0x005A, 0x01AC,0x049E, 0x01AE,0x08D2});
    add({0x01B0,0x0D27, 0x01B2,0x016B, 0x01B4,0x05AF, 0x01B6,0x09E3});
    add({0x01B8,0x0E34, 0x01BA,0x0278, 0x01BC,0x06BC, 0x01BE,0x0AF0});

    add({0x7607,0xFFFE});

    if (cr3) {
        add({0x01A0,0x005A, 0x01A2,0x049E, 0x01A4,0x041E, 0x01A6,0x0C16});
        add({0x01A8,0x016B, 0x01AA,0x05AF, 0x01AC,0x052F, 0x01AE,0x0D27});
        add({0x01B0,0x014B, 0x01B2,0x058F, 0x01B4,0x050F, 0x01B6,0x0D07});
        add({0x01B8,0x0349, 0x01BA,0x078D, 0x01BC,0x070D, 0x01BE,0x0F05});
    } else {
        add({0x01A0,0x0FA5, 0x01A2,0x07AD, 0x01A4,0x0BE1, 0x01A6,0x0F25});
        add({0x01A8,0x0DA7, 0x01AA,0x05AF, 0x01AC,0x09E3, 0x01AE,0x0D27});
        add({0x01B0,0x0EB4, 0x01B2,0x06BC, 0x01B4,0x0AF0, 0x01B6,0x0E34});
        add({0x01B8,0x0F85, 0x01BA,0x078D, 0x01BC,0x0BC1, 0x01BE,0x0F05});
    }

    add({0x8407,0xFFFE});

    add({0x0180,0x0000, 0x0182,0x0CCC, 0x0184,0x0000, 0x0186,0x0000});
    add({0x0188,0x0333, 0x018A,0x0FFF, 0x018C,0x0333, 0x018E,0x0333});
    add({0x0190,0x0000, 0x0192,0x0CCC, 0x0194,0x0000, 0x0196,0x0000});
    add({0x0198,0x0000, 0x019A,0x0CCC, 0x019C,0x0000, 0x019E,0x0000});

    add({0xFFFF,0xFFFE});

    return t;
}

// ---- pixel data (SPECIFICATION.md 3.2) --------------------------------
constexpr int kLA[16] = {1,1,2,1,3,2,2,3,3,1,0,3,0,2,0,1}; // "1121322331030201"

int pixel_value(int line_in_band, int pixel) {
    if (line_in_band < 2) return 1;      // L1
    if (line_in_band < 4) return 2;      // L2
    if (line_in_band < 6) return 3;      // L3
    if (line_in_band < 8) return kLA[pixel]; // LA
    return 0;                             // gap
}

// ---- expected masked colours per band/value (SPECIFICATION.md section 6) --
struct RGB3 { uint8_t r, g, b; };
RGB3 unpack(uint16_t v) { return { uint8_t((v>>8)&0xF), uint8_t((v>>4)&0xF), uint8_t(v&0xF) }; }
RGB3 mask(const nmh::VideoSample& s) { return { uint8_t(s.r & 0xC), uint8_t(s.g & 0xC), uint8_t(s.b & 0xC) }; }
bool dark(const RGB3& c) { return c.r==0 && c.g==0 && c.b==0; }
bool eq(const RGB3& a, const RGB3& b) { return a.r==b.r && a.g==b.g && a.b==b.b; }

// index 0=colour17,1=colour18,2=colour19; band 4 uses the same row as band 0 (S0x).
RGB3 expected_v(int band, int value) {
    static const uint16_t table[4][3] = {
        {0x48C, 0x8C0, 0xC04}, // band 0 (S0)
        {0x8C0, 0xC04, 0x048}, // band 1 (S1)
        {0xC04, 0x048, 0x48C}, // band 2 (S2)
        {0x048, 0x48C, 0x8C0}, // band 3 (S3)
    };
    int b = (band == 4) ? 0 : band;
    return unpack(table[b][value-1]);
}

// ---- letters ------------------------------------------------------------
enum Letter { L_A,L_B,L_C,L_D,L_E,L_F,L_G,L_H,L_I,L_J,L_K,L_L,
              L_M,L_N,L_O,L_P,L_Q,L_R,L_S,L_T,L_U,L_V,L_W,L_X, kNumLetters };

void grade_letters_bit(int value, const RGB3& exp, const RGB3& act,
                        std::array<bool,kNumLetters>& ok, std::array<long,kNumLetters>& pop) {
    auto bit = [](uint8_t x, int b) { return (x >> b) & 1; };
    int base = (value - 1) * 6 + 6; // value1->6(G), value2->12(M), value3->18(S)
    bool m[6] = {
        bit(act.r,3)==bit(exp.r,3), bit(act.r,2)==bit(exp.r,2),
        bit(act.g,3)==bit(exp.g,3), bit(act.g,2)==bit(exp.g,2),
        bit(act.b,3)==bit(exp.b,3), bit(act.b,2)==bit(exp.b,2),
    };
    for (int i = 0; i < 6; ++i) { ok[base+i] = ok[base+i] && m[i]; pop[base+i]++; }
}

// ---- video/frame analysis (SPECIFICATION.md section 4) -----------------

bool line_lit(const std::vector<nmh::VideoSample>& v, nmh::Span line, size_t off) {
    if (auto s = nmh::sample_at(v, line, off)) { RGB3 m = mask(*s); return !dark(m); }
    return false;
}

std::vector<nmh::Span> lines_in(const std::vector<nmh::Span>& all, nmh::Span frame) {
    std::vector<nmh::Span> out;
    for (auto& l : all) if (l.begin >= frame.begin && l.end <= frame.end) out.push_back(l);
    return out;
}

struct Anchor { int v0_idx = -1; size_t a0 = 0, a1 = 0; bool found = false; };

// v0: first output line of the graded frame with a run of >=32 consecutive
// lit samples (SPECIFICATION.md 4.3). a0: start of that run. a1: first lit
// sample after a0+31 (4.4).
Anchor find_anchor(const std::vector<nmh::VideoSample>& v, const std::vector<nmh::Span>& lif) {
    Anchor an;
    for (size_t li = 0; li < lif.size(); ++li) {
        nmh::Span line = lif[li];
        size_t len = line.end - line.begin;
        size_t run = 0, best = 0, beststart = 0, curstart = 0;
        for (size_t off = 0; off < len; ++off) {
            if (line_lit(v, line, off)) {
                if (run == 0) curstart = off;
                if (++run > best) { best = run; beststart = curstart; }
            } else run = 0;
        }
        if (best >= 32) {
            an.v0_idx = (int)li; an.a0 = beststart; an.found = true;
            size_t off = an.a0 + 32;
            while (off < len && !line_lit(v, line, off)) ++off;
            if (off >= len) an.found = false;
            an.a1 = off;
            break;
        }
    }
    return an;
}

// Diagnostic only (see the report): same search as find_anchor but with a
// lower run threshold, to locate the sprite band even if the true run
// width differs from the 32 that SPECIFICATION.md 4.3/4.4 assume. a1 here
// is the END of the first run found (a0+width), not the second block.
Anchor find_anchor16(const std::vector<nmh::VideoSample>& v, const std::vector<nmh::Span>& lif) {
    Anchor an;
    for (size_t li = 0; li < lif.size(); ++li) {
        nmh::Span line = lif[li];
        size_t len = line.end - line.begin;
        size_t run = 0, best = 0, beststart = 0, curstart = 0;
        for (size_t off = 0; off < len; ++off) {
            if (line_lit(v, line, off)) {
                if (run == 0) curstart = off;
                if (++run > best) { best = run; beststart = curstart; }
            } else run = 0;
        }
        if (best >= 16) {
            an.v0_idx = (int)li; an.a0 = beststart; an.a1 = beststart + best; an.found = true;
            break;
        }
    }
    return an;
}

bool window_active(const std::vector<nmh::VideoSample>& v, nmh::Span line, size_t lo, size_t hi) {
    size_t len = line.end - line.begin;
    if (hi > len) hi = len;
    for (size_t off = lo; off < hi; ++off) if (line_lit(v, line, off)) return true;
    return false;
}

struct DMeasure { int d = 0; bool ok = false; long run_active = 0, run_gap = 0; };

// d: output lines per source line, measured (not assumed) from the first
// active-then-gap boundary starting at v0 (band 0's 8 non-gap lines then
// 2 gap lines, SPECIFICATION.md 3.6), cross-checked against each other.
DMeasure measure_d(const std::vector<nmh::VideoSample>& v, const std::vector<nmh::Span>& lif, const Anchor& an) {
    DMeasure dm;
    if (!an.found) return dm;
    size_t lo = an.a0 >= 4 ? an.a0 - 4 : 0;
    size_t hi = an.a1 + 36;
    long i = an.v0_idx, n = (long)lif.size();
    long run_active = 0;
    while (i < n && window_active(v, lif[i], lo, hi)) { ++run_active; ++i; }
    long run_gap = 0;
    while (i < n && !window_active(v, lif[i], lo, hi)) { ++run_gap; ++i; }
    dm.run_active = run_active; dm.run_gap = run_gap;
    if (run_active > 0 && run_active % 8 == 0) {
        int d1 = (int)(run_active / 8);
        if (d1 > 0 && run_gap == 2 * d1) { dm.d = d1; dm.ok = true; }
    }
    return dm;
}

std::optional<nmh::Span> src_line_copy(const std::vector<nmh::Span>& lif, int v0_idx, int d, int n, int copy) {
    long idx = (long)v0_idx + (long)d * (n - 80) + copy;
    if (idx < 0 || idx >= (long)lif.size()) return std::nullopt;
    return lif[idx];
}

// ---- RAM-port log analysis (SPECIFICATION.md PC2/PC3/PC4) ---------------

// Within [lo,hi), checks that coalesced reads at word addresses
// [base,base+count) occur, each exactly once, in strictly ascending
// address order -- one full Copper-list or sprite-descriptor traversal.
bool check_full_traversal(const std::vector<nmh::RamAccess>& log, uint32_t base, int count,
                           uint64_t lo, uint64_t hi,
                           std::vector<uint16_t>& data_out, std::vector<uint64_t>& cycle_out,
                           std::vector<bool>& seen_out) {
    std::vector<const nmh::RamAccess*> hits;
    for (auto& a : log)
        if (!a.is_write && a.cycle >= lo && a.cycle < hi && a.word_addr >= base && a.word_addr < base + (uint32_t)count)
            hits.push_back(&a);
    std::sort(hits.begin(), hits.end(), [](const nmh::RamAccess* x, const nmh::RamAccess* y) { return x->cycle < y->cycle; });
    data_out.assign(count, 0); cycle_out.assign(count, 0); seen_out.assign(count, false);
    int expect_next = 0;
    bool order_ok = true;
    for (auto* h : hits) {
        int off = (int)(h->word_addr - base);
        if (off != expect_next) { order_ok = false; continue; }
        data_out[off] = h->data; cycle_out[off] = h->cycle; seen_out[off] = true;
        ++expect_next;
    }
    return order_ok && expect_next == count;
}

std::optional<std::pair<uint64_t,uint64_t>> minmax_cycle(const std::vector<uint64_t>& cyc, const std::vector<bool>& seen,
                                                          int base, int count) {
    uint64_t lo = UINT64_MAX, hi = 0; bool any = false;
    for (int i = base; i < base + count; ++i) {
        if (!seen[i]) return std::nullopt;
        any = true;
        if (cyc[i] < lo) lo = cyc[i];
        if (cyc[i] > hi) hi = cyc[i];
    }
    if (!any) return std::nullopt;
    return std::make_pair(lo, hi);
}

// Sprite descriptor word offset (from the sprite's own base) of source
// line n's low (high=false) or high (high=true) colour-descriptor word
// (SPECIFICATION.md 4.5): header (POS,CTL) is 2 words, then 2 words/line.
int sprite_line_offset(int n, bool high) { return 2 + 2 * (n - 80) + (high ? 1 : 0); }

// ---- claim ids per letter (Appendix A), enum order matches kLetterMeta --
struct LetterMeta { char name; int value; const char* comp; const char* ids; };
const LetterMeta kLetterMeta[18] = {
  {'G',1,"red bit 3",   "APPC-sprtab.r19b11 CH4-appC-color.c162 APPC-sprtab.r1Db11 CH4-appC-color.c226"},
  {'H',1,"red bit 2",   "APPC-sprtab.r19b10 CH4-appC-color.c163 APPC-sprtab.r1Db10 CH4-appC-color.c227"},
  {'I',1,"green bit 3", "APPC-sprtab.r19b07 CH4-appC-color.c166 APPC-sprtab.r1Db07 CH4-appC-color.c230"},
  {'J',1,"green bit 2", "APPC-sprtab.r19b06 CH4-appC-color.c167 APPC-sprtab.r1Db06 CH4-appC-color.c231"},
  {'K',1,"blue bit 3",  "APPC-sprtab.r19b03 CH4-appC-color.c170 APPC-sprtab.r1Db03 CH4-appC-color.c234"},
  {'L',1,"blue bit 2",  "APPC-sprtab.r19b02 CH4-appC-color.c171 APPC-sprtab.r1Db02 CH4-appC-color.c235"},
  {'M',2,"red bit 3",   "APPC-sprtab.r18b09 CH4-appC-color.c148 APPC-sprtab.r19b09 CH4-appC-color.c164 APPC-sprtab.r1Ab11 CH4-appC-color.c178 APPC-sprtab.r1Ab09 CH4-appC-color.c180 APPC-sprtab.r1Bb09 CH4-appC-color.c196 APPC-sprtab.r1Eb11 CH4-appC-color.c242"},
  {'N',2,"red bit 2",   "APPC-sprtab.r18b08 CH4-appC-color.c149 APPC-sprtab.r19b08 CH4-appC-color.c165 APPC-sprtab.r1Ab10 CH4-appC-color.c179 APPC-sprtab.r1Ab08 CH4-appC-color.c181 APPC-sprtab.r1Bb08 CH4-appC-color.c197 APPC-sprtab.r1Eb10 CH4-appC-color.c243"},
  {'O',2,"green bit 3", "APPC-sprtab.r18b05 CH4-appC-color.c152 APPC-sprtab.r19b05 CH4-appC-color.c168 APPC-sprtab.r1Ab07 CH4-appC-color.c182 APPC-sprtab.r1Ab05 CH4-appC-color.c184 APPC-sprtab.r1Bb05 CH4-appC-color.c200 APPC-sprtab.r1Eb07 CH4-appC-color.c246"},
  {'P',2,"green bit 2", "APPC-sprtab.r18b04 CH4-appC-color.c153 APPC-sprtab.r19b04 CH4-appC-color.c169 APPC-sprtab.r1Ab06 CH4-appC-color.c183 APPC-sprtab.r1Ab04 CH4-appC-color.c185 APPC-sprtab.r1Bb04 CH4-appC-color.c201 APPC-sprtab.r1Eb06 CH4-appC-color.c247"},
  {'Q',2,"blue bit 3",  "APPC-sprtab.r18b01 CH4-appC-color.c156 APPC-sprtab.r19b01 CH4-appC-color.c172 APPC-sprtab.r1Ab03 CH4-appC-color.c186 APPC-sprtab.r1Ab01 CH4-appC-color.c188 APPC-sprtab.r1Bb01 CH4-appC-color.c204 APPC-sprtab.r1Eb03 CH4-appC-color.c250"},
  {'R',2,"blue bit 2",  "APPC-sprtab.r18b00 CH4-appC-color.c157 APPC-sprtab.r19b00 CH4-appC-color.c173 APPC-sprtab.r1Ab02 CH4-appC-color.c187 APPC-sprtab.r1Ab00 CH4-appC-color.c189 APPC-sprtab.r1Bb00 CH4-appC-color.c205 APPC-sprtab.r1Eb02 CH4-appC-color.c251"},
  {'S',3,"red bit 3",   "APPC-sprtab.r1Bb11 CH4-appC-color.c194 APPC-sprtab.r1Cb09 CH4-appC-color.c212 APPC-sprtab.r1Db09 CH4-appC-color.c228 APPC-sprtab.r1Eb09 CH4-appC-color.c244 APPC-sprtab.r1Fb11 CH4-appC-color.c258 APPC-sprtab.r1Fb09 CH4-appC-color.c260"},
  {'T',3,"red bit 2",   "APPC-sprtab.r1Bb10 CH4-appC-color.c195 APPC-sprtab.r1Cb08 CH4-appC-color.c213 APPC-sprtab.r1Db08 CH4-appC-color.c229 APPC-sprtab.r1Eb08 CH4-appC-color.c245 APPC-sprtab.r1Fb10 CH4-appC-color.c259 APPC-sprtab.r1Fb08 CH4-appC-color.c261"},
  {'U',3,"green bit 3", "APPC-sprtab.r1Bb07 CH4-appC-color.c198 APPC-sprtab.r1Cb05 CH4-appC-color.c216 APPC-sprtab.r1Db05 CH4-appC-color.c232 APPC-sprtab.r1Eb05 CH4-appC-color.c248 APPC-sprtab.r1Fb07 CH4-appC-color.c262 APPC-sprtab.r1Fb05 CH4-appC-color.c264"},
  {'V',3,"green bit 2", "APPC-sprtab.r1Bb06 CH4-appC-color.c199 APPC-sprtab.r1Cb04 CH4-appC-color.c217 APPC-sprtab.r1Db04 CH4-appC-color.c233 APPC-sprtab.r1Eb04 CH4-appC-color.c249 APPC-sprtab.r1Fb06 CH4-appC-color.c263 APPC-sprtab.r1Fb04 CH4-appC-color.c265"},
  {'W',3,"blue bit 3",  "APPC-sprtab.r1Bb03 CH4-appC-color.c202 APPC-sprtab.r1Cb01 CH4-appC-color.c220 APPC-sprtab.r1Db01 CH4-appC-color.c236 APPC-sprtab.r1Eb01 CH4-appC-color.c252 APPC-sprtab.r1Fb03 CH4-appC-color.c266 APPC-sprtab.r1Fb01"},
  {'X',3,"blue bit 2",  "APPC-sprtab.r1Bb02 CH4-appC-color.c203 APPC-sprtab.r1Cb00 CH4-appC-color.c221 APPC-sprtab.r1Db00 CH4-appC-color.c237 APPC-sprtab.r1Eb00 CH4-appC-color.c253 APPC-sprtab.r1Fb02 CH4-appC-color.c267 APPC-sprtab.r1Fb00"},
};

const char* kP4Ids =
  "APPC-sprtab.r18b11 CH4-appC-color.c146 APPC-sprtab.r1Cb11 CH4-appC-color.c210 "
  "APPC-sprtab.r18b10 CH4-appC-color.c147 APPC-sprtab.r1Cb10 CH4-appC-color.c211 "
  "APPC-sprtab.r18b07 CH4-appC-color.c150 APPC-sprtab.r1Cb07 CH4-appC-color.c214 "
  "APPC-sprtab.r18b06 CH4-appC-color.c151 APPC-sprtab.r1Cb06 CH4-appC-color.c215 "
  "APPC-sprtab.r18b03 CH4-appC-color.c154 APPC-sprtab.r1Cb03 CH4-appC-color.c218 "
  "APPC-sprtab.r18b02 CH4-appC-color.c155 APPC-sprtab.r1Cb02 CH4-appC-color.c219";

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    const char* mutate_env = std::getenv("NMH_MUTATE");
    std::string mv = mutate_env ? mutate_env : "";
    Scenario sc = parse_scenario(mv);
    std::printf("MUTATE: %s\n", mv.empty() ? "(none)" : mv.c_str());

    Checks c;

    nmh::Config cfg;
    cfg.chipset_config = (sc == Scenario::CR7) ? 0x00 : 0x08;
    nmh::NanomigHarness h(cfg);

    std::string bin = dir + "/" + bin_for(sc);
    size_t loaded = h.load_program(bin);
    auto boot = h.boot(kBootBudget);

    std::optional<uint64_t> marker_cycle;
    bool marker_seen = false;
    int frames_after = 0;
    bool prev_vs = false;
    size_t video_scan = 0;
    uint64_t elapsed = 0;
    if (loaded && boot.cpu_released) {
        while (elapsed < kMaxAfterBoot) {
            h.run_cycles(kChunk);
            elapsed += kChunk;
            if (!marker_seen) {
                for (auto& a : h.ram_log())
                    if (a.is_write && a.word_addr == kMarker / 2 && a.data == kMarkerVal) {
                        marker_cycle = a.cycle; marker_seen = true; break;
                    }
            }
            const auto& v = h.video();
            for (; video_scan < v.size(); ++video_scan) {
                bool cur = v[video_scan].vs;
                if (cur && !prev_vs && marker_seen) ++frames_after;
                prev_vs = cur;
            }
            if (marker_seen && frames_after >= kFramesAfterMarker) break;
        }
    }
    bool run_ok = loaded > 0 && boot.cpu_released && marker_seen;

    c.expect(run_ok, "RUN: prog " + bin + " loaded (" + std::to_string(loaded) + " bytes), CPU released from "
             "reset (" + std::to_string(boot.cycles_to_release) + " clk_sys cycles), MARKER received $A5A5 "
             "within budget" + (marker_seen ? (" at cycle " + std::to_string(*marker_cycle)) : " -- not seen"));

    // ---- fixture: a program-stored constant, read back first ------------
    auto fixture = h.peek_logical(kResults);
    bool fixture_ok = fixture.has_value() && *fixture == 0xBEEF;
    c.expect(fixture_ok, "FIXTURE: prog stores a known constant (BEEF) at RESULTS before anything else is trusted"
             " (read back " + (fixture ? hex4(*fixture) : std::string("nothing")) + ")");

    // ---- PC0: harness configuration ---------------------------------------
    bool pc0_ok = (cfg.chipset_config & 0x08) != 0 && cfg.memory_config == 0x03;
    c.expect(pc0_ok, "PC0: the harness runs ECS, PAL and 2 MB Chip memory (chipset_config=0x" +
             hex4(cfg.chipset_config) + ", memory_config=0x" + hex4(cfg.memory_config) + ")");

    const auto& video = h.video();
    auto colog = nmh::coalesce(h.ram_log());

    auto frames = nmh::find_frames(video);
    std::optional<nmh::Span> graded_frame;
    if (marker_seen) {
        for (auto& f : frames)
            if (f.end <= *marker_cycle && f.end < video.size())
                if (!graded_frame || f.end > graded_frame->end) graded_frame = f;
    }

    auto all_lines = nmh::find_lines(video);
    std::vector<nmh::Span> lif;
    if (graded_frame) lif = lines_in(all_lines, *graded_frame);

    Anchor anchor = find_anchor(video, lif);
    DMeasure dm = measure_d(video, lif, anchor);
    bool have_anchor = anchor.found && dm.ok && graded_frame.has_value();

    if (graded_frame)
        c.note("graded frame: samples [" + std::to_string(graded_frame->begin) + "," +
               std::to_string(graded_frame->end) + "), " + std::to_string(lif.size()) + " output lines");
    if (anchor.found)
        c.note("anchor: v0_idx=" + std::to_string(anchor.v0_idx) + " a0=" + std::to_string(anchor.a0) +
               " a1=" + std::to_string(anchor.a1) + " (a1-a0=" + std::to_string(anchor.a1 - anchor.a0) + ")");
    else
        c.note("anchor: v0 not found (no output line with a run of >=32 consecutive lit samples)");
    c.note("d measurement: run_active=" + std::to_string(dm.run_active) + " run_gap=" + std::to_string(dm.run_gap) +
           " -> d=" + std::to_string(dm.d) + (dm.ok ? " (consistent)" : " (INCONSISTENT or not measurable)"));

    // Diagnostic for the halt-and-report finding below: a run of >=16 (not
    // >=32) consecutive lit samples locates the same sprite-band line
    // reliably. This measurement is never used to grade a claim.
    Anchor anchor16 = find_anchor16(video, lif);
    if (anchor16.found) {
        nmh::Span line = lif[anchor16.v0_idx];
        size_t off = anchor16.a0 + 16;
        while (off < (line.end - line.begin) && !line_lit(video, line, off)) ++off;
        size_t second_start = off;
        size_t width2 = 0;
        while (line_lit(video, line, second_start + width2)) ++width2;
        c.record("SPECIFICATION.md 4.4's factor of 2 (\"sprite pixel i covers samples a0+2i and "
                  "a0+2i+1\") does not hold here: on graded-frame output line " + std::to_string(anchor16.v0_idx) +
                  " (source line 80, both sprites' L1 line, all 16 pixels = colour 17), sprite 0's lit run is " +
                  std::to_string(anchor16.a1 - anchor16.a0) + " samples wide (offsets " + std::to_string(anchor16.a0) +
                  ".." + std::to_string(anchor16.a1 - 1) + "), not 32; sprite 1's run starts " +
                  std::to_string(second_start - anchor16.a0) + " samples later (matches 4.4's own p.107-derived "
                  "50, i.e. 25 low-res pixels x2) and is also " + std::to_string(width2) +
                  " samples wide, not 32. Width is halved; position is not. P1-P4/PC1-PC3 cannot be graded "
                  "against a 32-sample-wide observation window that the trace does not show -- see the report.");
    }

    // ---- PC1: SuperHires in effect around the sprite band ------------------
    bool pc1_ok = have_anchor;
    long pc1_checked = 0;
    if (have_anchor) {
        auto check_region = [&](int lo, int hi) {
            for (int n = lo; n <= hi; ++n) for (int copy = 0; copy < dm.d; ++copy) {
                auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                if (!span) { pc1_ok = false; continue; }
                for (size_t off = anchor.a0; off < anchor.a0 + 99; ++off) {
                    auto s1 = nmh::sample_at(video, *span, off);
                    auto s2 = nmh::sample_at(video, *span, off + 1);
                    ++pc1_checked;
                    if (!s1 || !s2) { pc1_ok = false; continue; }
                    bool cur = !dark(mask(*s1)), nxt = !dark(mask(*s2));
                    if (cur == nxt) pc1_ok = false;
                }
            }
        };
        check_region(46, 55);
        check_region(140, 160);
    }
    c.expect(pc1_ok, "PC1: SuperHires in effect -- samples a0..a0+99 on source lines 46-55 and 140-160 of the "
              "graded frame alternate lit/dark with runs of exactly one sample, and the graded frame's Copper "
              "fetches contain no MOVE to $0100 (BPLCON0 written only once, in step 3) (" +
              std::to_string(pc1_checked) + " transitions checked)");

    // ---- PC2: sprite frame --------------------------------------------------
    std::vector<uint16_t> s0d, s1d; std::vector<uint64_t> s0c, s1c; std::vector<bool> s0s, s1s;
    bool spr0_seq = false, spr1_seq = false;
    if (graded_frame) {
        spr0_seq = check_full_traversal(colog, kSpr0WordBase, kSprWords, graded_frame->begin, graded_frame->end, s0d, s0c, s0s);
        spr1_seq = check_full_traversal(colog, kSpr1WordBase, kSprWords, graded_frame->begin, graded_frame->end, s1d, s1c, s1s);
    }
    bool pc2a_ok = spr0_seq && spr1_seq;

    bool pc2b_ok = have_anchor, pc2c_ok = have_anchor, pc2d_ok = have_anchor;
    long pc2b_checked = 0, pc2c_checked = 0, pc2d_checked = 0;
    if (have_anchor) {
        pc2c_ok = (anchor.a1 > anchor.a0) && (anchor.a1 - anchor.a0 >= 33);
        for (int n = 80; n <= 129; ++n) for (int copy = 0; copy < dm.d; ++copy) {
            auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
            if (!span) { pc2b_ok = false; pc2c_ok = false; continue; }
            int lineInBand = (n - 80) % 10;
            for (int pixel = 0; pixel < 16; ++pixel) {
                int value = pixel_value(lineInBand, pixel);
                for (int sprite = 0; sprite < 2; ++sprite) {
                    size_t anchor_pos = sprite == 0 ? anchor.a0 : anchor.a1;
                    for (int s01 = 0; s01 < 2; ++s01) {
                        size_t off = anchor_pos + 2 * pixel + s01;
                        auto samp = nmh::sample_at(video, *span, off);
                        ++pc2b_checked;
                        if (!samp) { pc2b_ok = false; continue; }
                        bool is_dark = dark(mask(*samp));
                        if (is_dark != (value == 0)) pc2b_ok = false;
                    }
                }
            }
            auto darkspan = [&](size_t lo, size_t hi) {
                for (size_t off = lo; off < hi; ++off) {
                    ++pc2c_checked;
                    auto samp = nmh::sample_at(video, *span, off);
                    if (!samp || !dark(mask(*samp))) pc2c_ok = false;
                }
            };
            darkspan(anchor.a0 >= 4 ? anchor.a0 - 4 : 0, anchor.a0);
            darkspan(anchor.a0 + 32, anchor.a1);
            darkspan(anchor.a1 + 32, anchor.a1 + 36);
        }
        auto darkregion = [&](int lo, int hi) {
            for (int n = lo; n <= hi; ++n) for (int copy = 0; copy < dm.d; ++copy) {
                auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                if (!span) { pc2d_ok = false; continue; }
                size_t start = anchor.a0 >= 4 ? anchor.a0 - 4 : 0, end = anchor.a1 + 36;
                for (size_t off = start; off < end; ++off) {
                    ++pc2d_checked;
                    auto samp = nmh::sample_at(video, *span, off);
                    if (!samp || !dark(mask(*samp))) pc2d_ok = false;
                }
            }
        };
        darkregion(70, 79);
        darkregion(130, 131);
    }

    // ---- P1-P4: per-letter masked-colour grading (gated on have_anchor) ----
    std::array<bool, kNumLetters> letter_ok; letter_ok.fill(true);
    std::array<long, kNumLetters> letter_pop; letter_pop.fill(0);
    bool p4_ok = true; long p4_pop = 0;

    if (have_anchor) {
        auto grade_pixel = [&](int band, int value, RGB3 exp, size_t anchor_pos, std::optional<nmh::Span> span, int pixel) {
            for (int s01 = 0; s01 < 2; ++s01) {
                std::optional<nmh::VideoSample> samp;
                if (span) samp = nmh::sample_at(video, *span, anchor_pos + 2 * pixel + s01);
                RGB3 act = samp ? mask(*samp) : RGB3{0xFF, 0xFF, 0xFF};
                if (band < 4) grade_letters_bit(value, exp, act, letter_ok, letter_pop);
                else { p4_ok = p4_ok && eq(act, exp); ++p4_pop; }
            }
        };
        for (int band = 0; band < 5; ++band) {
            for (int lineInBand = 0; lineInBand < 10; ++lineInBand) {
                int n = 80 + 10 * band + lineInBand;
                for (int pixel = 0; pixel < 16; ++pixel) {
                    int value = pixel_value(lineInBand, pixel);
                    if (value == 0) continue;
                    RGB3 exp = expected_v(band, value);
                    for (int sprite = 0; sprite < 2; ++sprite) {
                        size_t anchor_pos = sprite == 0 ? anchor.a0 : anchor.a1;
                        for (int copy = 0; copy < dm.d; ++copy) {
                            auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                            grade_pixel(band, value, exp, anchor_pos, span, pixel);
                        }
                    }
                }
            }
        }
    }

    long expect_p1 = 672L * dm.d, expect_p2 = 640L * dm.d, expect_p3 = 640L * dm.d, expect_p4 = 488L * dm.d;
    bool pc2e_ok = have_anchor && letter_pop[L_G] == expect_p1 && letter_pop[L_M] == expect_p2 &&
                   letter_pop[L_S] == expect_p3 && p4_pop == expect_p4;

    bool pc2_ok = pc2a_ok && pc2b_ok && pc2c_ok && pc2d_ok && pc2e_ok;
    c.expect(pc2_ok, "PC2: sprite frame -- (a) sprite 0/1 fetch control+50 data pairs in address order (" +
             std::to_string(pc2a_ok ? 2L * kSprWords : 0) + " words), (b) lit/dark matches the pixel values on "
             "source lines 80-129 (" + std::to_string(pc2b_checked) + " samples), (c) guard bands dark and "
             "a1-a0>=33 (" + std::to_string(pc2c_checked) + " samples), (d) lines 70-79/130-131 fully dark (" +
             std::to_string(pc2d_checked) + " samples), (e) graded sample counts equal P1-P4's stated totals "
             "(P1=" + std::to_string(letter_pop[L_G]) + "/" + std::to_string(expect_p1) +
             ", P2=" + std::to_string(letter_pop[L_M]) + "/" + std::to_string(expect_p2) +
             ", P3=" + std::to_string(letter_pop[L_S]) + "/" + std::to_string(expect_p3) +
             ", P4=" + std::to_string(p4_pop) + "/" + std::to_string(expect_p4) + ")");

    // ---- PC3: register contents in place, and as specified -----------------
    bool cr1 = (sc == Scenario::CR1), cr2 = (sc == Scenario::CR2), cr3 = (sc == Scenario::CR3), cr6 = (sc == Scenario::CR6);
    auto expected_cop = build_expected_coptab(cr1, cr2, cr3, cr6);

    std::vector<uint16_t> cop_data; std::vector<uint64_t> cop_cyc; std::vector<bool> cop_seen;
    bool pc3_seq_ok = false;
    if (graded_frame)
        pc3_seq_ok = check_full_traversal(colog, kCoplistWordBase, kCopWords, graded_frame->begin, graded_frame->end,
                                           cop_data, cop_cyc, cop_seen);

    bool pc3_content_ok = pc3_seq_ok;
    int content_mismatches = 0;
    if (pc3_seq_ok) for (int i = 0; i < kCopWords; ++i) if (cop_data[i] != expected_cop[i]) { pc3_content_ok = false; ++content_mismatches; }

    bool pc3_timing_ok = pc3_seq_ok && spr0_seq && spr1_seq;
    if (pc3_timing_ok) {
        auto last_fetch = [&](int n) -> std::optional<uint64_t> {
            int lo_off = sprite_line_offset(n, false), hi_off = sprite_line_offset(n, true);
            if (!s0s[lo_off] || !s0s[hi_off] || !s1s[lo_off] || !s1s[hi_off]) return std::nullopt;
            return std::max({s0c[lo_off], s0c[hi_off], s1c[lo_off], s1c[hi_off]});
        };
        auto first_fetch = [&](int n) -> std::optional<uint64_t> {
            int lo_off = sprite_line_offset(n, false), hi_off = sprite_line_offset(n, true);
            if (!s0s[lo_off] || !s0s[hi_off] || !s1s[lo_off] || !s1s[hi_off]) return std::nullopt;
            return std::min({s0c[lo_off], s0c[hi_off], s1c[lo_off], s1c[hi_off]});
        };
        auto line80_first = first_fetch(80);
        auto grpT_pattern = minmax_cycle(cop_cyc, cop_seen, kOffColor0_15a, 32);
        auto grpT_color   = minmax_cycle(cop_cyc, cop_seen, kOffColor16_31_T, 32);
        auto grpDark      = minmax_cycle(cop_cyc, cop_seen, kOffColor0_15dark, 32);
        if (!line80_first || !grpT_pattern || !grpT_color || !grpDark) pc3_timing_ok = false;
        else {
            if (!(grpT_pattern->second < *line80_first)) pc3_timing_ok = false;
            if (!(grpT_color->second < *line80_first)) pc3_timing_ok = false;
            if (!(grpDark->second < *line80_first)) pc3_timing_ok = false;
        }
        struct BandCheck { int off, prevlast, nextfirst; };
        BandCheck bands[4] = {
            {kOffColor16_31_S1, 87, 90}, {kOffColor16_31_S2, 97, 100},
            {kOffColor16_31_S3, 107, 110}, {kOffColor16_31_band4, 117, 120},
        };
        for (auto& bc : bands) {
            auto grp = minmax_cycle(cop_cyc, cop_seen, bc.off, 32);
            auto plast = last_fetch(bc.prevlast);
            auto nfirst = first_fetch(bc.nextfirst);
            if (!grp || !plast || !nfirst) { pc3_timing_ok = false; continue; }
            if (!(grp->first > *plast && grp->second < *nfirst)) pc3_timing_ok = false;
        }
    }
    bool pc3_ok = pc3_seq_ok && pc3_content_ok && pc3_timing_ok;
    c.expect(pc3_ok, "PC3: the Copper fetches every one of the " + std::to_string(kCopWords) +
             " instruction words of the list in order and the MOVE data words equal those of 3.3/3.5 (" +
             std::to_string(content_mismatches) + " content mismatches); for bands 1-4, the band's 32-word "
             "colour group fetches strictly between the previous band's last and this band's first sprite "
             "data fetch; section T's two colour groups and the dark-set group all fetch before line 80's "
             "first sprite data fetch");

    // ---- PC4: run length -----------------------------------------------
    long pass_count = 0;
    if (graded_frame)
        for (auto& a : colog)
            if (!a.is_write && a.word_addr == kCoplistWordBase + kOffWaitFinal && a.cycle < graded_frame->begin)
                ++pass_count;
    bool pc4_ok = marker_seen && graded_frame.has_value() && pass_count >= 3;
    c.expect(pc4_ok, "PC4: MARKER received $A5A5, the graded frame is complete, and " + std::to_string(pass_count) +
             " complete Copper passes (>= 3 required) precede it");

    // ---- P1-P4 -----------------------------------------------------------
    for (int i = 0; i < 18; ++i) {
        auto& m = kLetterMeta[i];
        std::string label = "P" + std::to_string(m.value) + " colour" + std::to_string(16 + m.value) + " " +
                             std::string(m.comp) + " (letter " + std::string(1, m.name) + ") p302: " + m.ids +
                             " -- every graded pixel of colour" + std::to_string(16 + m.value) +
                             " (bands 0-3) shows this bit matching the band-predicted value";
        if (have_anchor) c.expect(letter_ok[L_G + i], label);
        else c.record("not graded (PC2's v0/d precondition failed this run): " + label);
    }
    {
        std::string label = std::string("P4 colour16 letters A-F p302: ") + kP4Ids +
            " -- band 4's colours 17-19 match S0's G-X-predicted values exactly in every graded sample "
            "(colour16's letters A-F do not show), one joint verdict for all 24 ids";
        if (have_anchor) c.expect(p4_ok, label);
        else c.record("not graded (PC2's v0/d precondition failed this run): " + label);
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
