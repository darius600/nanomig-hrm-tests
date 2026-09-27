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
// samples, and checks PC0-PC4 (plus the PC2w width precondition) from the
// same trace plus the RAM-port log.
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
//   NMH_MUTATE=CR9  prog_cr9.bin  (RESULTS stores $DEAD instead of $BEEF)
//   NMH_MUTATE=CR10 prog_cr10.bin (the program never writes MARKER)
//
// MUTATION P1 stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- bit-3 letters G,I,K mismatch
// MUTATION P2 stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- bit-3 letters M,O,Q mismatch
// MUTATION P3 stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- bit-3 letters S,U,W mismatch
// MUTATION P1 stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- bit-2 letters H,J,L mismatch
// MUTATION P2 stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- bit-2 letters N,P,R mismatch
// MUTATION P3 stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- bit-2 letters T,V,X mismatch
// MUTATION P4 stimulus red NMH_MUTATE=CR3: band-4 COLOR16-31 = S0-c18 instead of S0x -- colour 18's samples mismatch
// MUTATION PC1 stimulus red NMH_MUTATE=CR4: BPLCON0=$9000 (HIRES) instead of $1040 (SHRES) -- pattern runs of two instead of one
// MUTATION PC2w stimulus green NMH_MUTATE=CR4: already red at baseline (p.101/p.124/p.128 predict all 32 samples lit, only 16 measure); the same width comparator passes 32-of-32 lit under HIRES, where a low-resolution pixel is one Hires sample pair too
// MUTATION PC2 stimulus red NMH_MUTATE=CR5: DMACON=$8380 (SPREN not set) instead of $83A0
// MUTATION PC3 stimulus red NMH_MUTATE=CR6: band-1 WAIT=$5C07 (line 92) instead of $5807 (line 88)
// MUTATION PC0 stimulus red NMH_MUTATE=CR7: chipset_config selects OCS instead of ECS
// MUTATION PC4 stimulus red NMH_MUTATE=CR8: MARKER written after 1 frame start instead of 6
// MUTATION FIXTURE stimulus red NMH_MUTATE=CR9: RESULTS stores $DEAD instead of $BEEF
// MUTATION RUN stimulus red NMH_MUTATE=CR10: the program never writes MARKER (loops before the write)

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

constexpr uint64_t kBootBudget    = 4'000'000;
constexpr uint64_t kChunk         = 200'000;
constexpr uint64_t kMaxAfterBoot  = 16'000'000;
constexpr int kFramesAfterMarker  = 2;

enum class Scenario { Base, CR1, CR2, CR3, CR4, CR5, CR6, CR7, CR8, CR9, CR10 };

Scenario parse_scenario(const std::string& v) {
    if (v == "CR1") return Scenario::CR1;
    if (v == "CR2") return Scenario::CR2;
    if (v == "CR3") return Scenario::CR3;
    if (v == "CR4") return Scenario::CR4;
    if (v == "CR5") return Scenario::CR5;
    if (v == "CR6") return Scenario::CR6;
    if (v == "CR7") return Scenario::CR7;
    if (v == "CR8") return Scenario::CR8;
    if (v == "CR9") return Scenario::CR9;
    if (v == "CR10") return Scenario::CR10;
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
        case Scenario::CR9: return "prog_cr9.bin";
        case Scenario::CR10: return "prog_cr10.bin";
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

// ---- sprite descriptor mirror (sprb_data.i's SPRDESC, literal) -----------
// header (POS, CTL) then 50 source lines' low/high colour-descriptor word
// pairs (identical for both sprites -- only POS/CTL differ), then a
// two-word terminator: 2 + 100 + 2 = kSprWords.
std::vector<uint16_t> build_expected_sprdesc(uint16_t pos, uint16_t ctl) {
    std::vector<uint16_t> t;
    t.push_back(pos); t.push_back(ctl);
    auto add = [&](uint16_t lo, uint16_t hi) { t.push_back(lo); t.push_back(hi); };
    for (int band = 0; band < 5; ++band) {
        add(0xFFFF, 0x0000); add(0xFFFF, 0x0000); // L1, L1
        add(0x0000, 0xFFFF); add(0x0000, 0xFFFF); // L2, L2
        add(0xFFFF, 0xFFFF); add(0xFFFF, 0xFFFF); // L3, L3
        add(0xD9D1, 0x2F94); add(0xD9D1, 0x2F94); // LA, LA
        add(0x0000, 0x0000); add(0x0000, 0x0000); // gap, gap
    }
    t.push_back(0x0000); t.push_back(0x0000); // terminator
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

// v0: first output line of the graded frame whose longest run of consecutive
// lit samples exceeds 4 (SPECIFICATION.md 4.3): the bitplane pattern of 3.3
// alone gives at most 1 (SuperHires, p.300), 2 (Hires, p.271) or 4 (lores,
// p.271/p.300) samples, so a longer run identifies the sprite band under
// every configuration this package's contrasts use, independently of the
// width PC2w checks. a0: start of that run. a1: first lit sample after that
// run ends (SPECIFICATION.md 4.4).
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
        if (best > 4) {
            an.v0_idx = (int)li; an.a0 = beststart; an.found = true;
            size_t off = an.a0 + best;
            while (off < len && !line_lit(v, line, off)) ++off;
            if (off >= len) an.found = false;
            an.a1 = off;
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
// These ids are withdrawn (1.1, SPECIFICATION.md section 6): the page does
// not say which register a sprite pixel reads, so a letter check with all
// positions written alike cannot establish one register bit. They are
// printed only on a RECORDED line, never on the graded PASS/FAIL line.
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

    // ---- PC0: chip identification, measured from the design --------------
    // (defect 7: chipset_config/memory_config are the harness's own inputs,
    // not a measurement; the program reads VPOSR and DENISEID instead.)
    auto vposr = h.peek_logical(kResults + 2);
    auto deniseid = h.peek_logical(kResults + 4);
    bool pc0_vposr_ok = vposr.has_value() && (((*vposr >> 8) & 0x7F) == 0x20);
    bool pc0_denise_ok = deniseid.has_value() && ((*deniseid & 0xFF) == 0xFC);
    bool pc0_ok = pc0_vposr_ok && pc0_denise_ok && cfg.memory_config == 0x03;
    c.expect(pc0_ok, "PC0: chip identification measured from the design -- VPOSR bits 14-8 = $20 "
             "(p.299 \"8368 (hr) ... = 20 for PAL\") and DENISEID bits 7-0 = $FC (p.299 \"The enhanced "
             "HighRes Denise (8373) will return $FC in the lower 8 bits\"), with 2 MB Chip memory "
             "(VPOSR=" + (vposr ? hex4(*vposr) : std::string("--")) +
             ", DENISEID=" + (deniseid ? hex4(*deniseid) : std::string("--")) +
             ", memory_config=0x" + hex4(cfg.memory_config) + ")");

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
        c.note("anchor: v0 not found (no output line with a run of more than 4 consecutive lit samples)");
    c.note("d measurement: run_active=" + std::to_string(dm.run_active) + " run_gap=" + std::to_string(dm.run_gap) +
           " -> d=" + std::to_string(dm.d) + (dm.ok ? " (consistent)" : " (INCONSISTENT or not measurable)"));

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
              "graded frame alternate lit/dark with runs of exactly one sample (" +
              std::to_string(pc1_checked) + " transitions checked); that BPLCON0 is written only once (step 3) "
              "and the graded pass's Copper fetches contain no MOVE to $0100 is established by PC3's own exact "
              "content match, not repeated here");

    // ---- PC2w: sprite pixel width (precondition; gates only the LA-line
    // grading of P1-P4 below). Expected to fail against the printed page --
    // that failure is a result, not something to repair (REPAIR-BRIEF.md).
    bool pc2w_ok = have_anchor;
    long pc2w_lit = 0, pc2w_dark = 0, pc2w_checked = 0;
    if (have_anchor) {
        for (int band = 0; band < 5; ++band) {
            for (int lineInBand = 0; lineInBand < 6; ++lineInBand) { // L1,L1,L2,L2,L3,L3
                int n = 80 + 10 * band + lineInBand;
                for (int copy = 0; copy < dm.d; ++copy) {
                    auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                    if (!span) { pc2w_ok = false; continue; }
                    for (int sprite = 0; sprite < 2; ++sprite) {
                        size_t anchor_pos = sprite == 0 ? anchor.a0 : anchor.a1;
                        for (size_t off = 0; off < 32; ++off) {
                            ++pc2w_checked;
                            auto samp = nmh::sample_at(video, *span, anchor_pos + off);
                            bool lit = samp && !dark(mask(*samp));
                            if (lit) ++pc2w_lit; else ++pc2w_dark;
                            if (!lit) pc2w_ok = false;
                        }
                    }
                }
            }
        }
    }
    c.expect(pc2w_ok, "PC2w: on every L1/L2/L3 line of the graded frame, sprite 0 is lit exactly over samples "
             "a0..a0+31 and sprite 1 over a1..a1+31 (p.101 \"Sprites, however, are independent of playfield "
             "modes of display, so changing the resolution or interlace mode of the playfield has no effect "
             "on the size or resolution of a sprite\"; p.124 \"The shift occurs once during each low "
             "resolution pixel time\"; p.128 \"The sprite resolution is independent of the bitplane "
             "resolution\") (" + std::to_string(pc2w_lit) + " lit, " + std::to_string(pc2w_dark) +
             " dark of " + std::to_string(pc2w_checked) + " checked)");
    c.record("PC2w page basis: p.101 \"Sprites, however, are independent of playfield modes of display, so "
              "changing the resolution or interlace mode of the playfield has no effect on the size or "
              "resolution of a sprite.\"; p.124 \"The shift occurs once during each low resolution pixel time "
              "and continues until all 16 bits have been transferred to the display circuitry.\"; p.128 \"The "
              "sprite resolution is independent of the bitplane resolution.\" A red here is the result the "
              "page predicts being measured, not a defect to repair; PC2w gates only the LA-line grading of "
              "P1-P4, and PC2(a)/(c)/(d)/(e) do not depend on it.");

    // ---- P1-P4: per-letter masked-colour grading -----------------------
    // Uniform lines (L1/L2/L3) are graded width-free: every LIT sample in
    // [a0,a1) (sprite 0) / [a1,a1+(a1-a0)) (sprite 1) is graded against the
    // band's expected value, whatever the measured width turns out to be
    // (SPECIFICATION.md section 6). LA lines need the pixel-exact a0+2i
    // mapping (4.4) and are graded only while PC2w holds.
    std::array<bool, kNumLetters> letter_ok; letter_ok.fill(true);
    std::array<long, kNumLetters> letter_pop; letter_pop.fill(0);
    bool p4_ok = true; long p4_pop = 0;

    // PC2(e): population and offset-consistency invariant across the same
    // uniform-line scan (replaces a fixed sample count, which would assume
    // the very width PC2w tests).
    bool pop_nonempty_ok = true; long pop_checked = 0;
    bool offsets_consistent_ok = true; long offsets_checked = 0;
    std::optional<std::vector<size_t>> ref_offsets;

    size_t win_len = have_anchor ? (anchor.a1 - anchor.a0) : 0;
    if (have_anchor && win_len > 0) {
        for (int band = 0; band < 5; ++band) {
            for (int lineInBand = 0; lineInBand < 6; ++lineInBand) { // L1,L1,L2,L2,L3,L3
                int n = 80 + 10 * band + lineInBand;
                int value = pixel_value(lineInBand, 0);
                RGB3 exp = expected_v(band, value);
                for (int sprite = 0; sprite < 2; ++sprite) {
                    size_t anchor_pos = sprite == 0 ? anchor.a0 : anchor.a1;
                    for (int copy = 0; copy < dm.d; ++copy) {
                        auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                        std::vector<size_t> lit_offsets;
                        if (span) {
                            for (size_t off = 0; off < win_len; ++off) {
                                auto samp = nmh::sample_at(video, *span, anchor_pos + off);
                                if (samp && !dark(mask(*samp))) {
                                    lit_offsets.push_back(off);
                                    RGB3 act = mask(*samp);
                                    if (band < 4) grade_letters_bit(value, exp, act, letter_ok, letter_pop);
                                    else { p4_ok = p4_ok && eq(act, exp); ++p4_pop; }
                                }
                            }
                        }
                        ++pop_checked;
                        if (lit_offsets.empty()) pop_nonempty_ok = false;
                        ++offsets_checked;
                        if (!ref_offsets) ref_offsets = lit_offsets;
                        else if (*ref_offsets != lit_offsets) offsets_consistent_ok = false;
                    }
                }
            }
        }
    }

    // LA lines: mixed pixel values need the pixel-exact a0+2i/a1+2i mapping
    // (4.4), so they are graded only while PC2w holds; otherwise recorded.
    if (have_anchor && pc2w_ok) {
        for (int band = 0; band < 5; ++band) {
            for (int lineInBand = 6; lineInBand <= 7; ++lineInBand) { // LA, LA
                int n = 80 + 10 * band + lineInBand;
                for (int pixel = 0; pixel < 16; ++pixel) {
                    int value = pixel_value(lineInBand, pixel);
                    if (value == 0) continue;
                    RGB3 exp = expected_v(band, value);
                    for (int sprite = 0; sprite < 2; ++sprite) {
                        size_t anchor_pos = sprite == 0 ? anchor.a0 : anchor.a1;
                        for (int copy = 0; copy < dm.d; ++copy) {
                            auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                            for (int s01 = 0; s01 < 2; ++s01) {
                                std::optional<nmh::VideoSample> samp;
                                if (span) samp = nmh::sample_at(video, *span, anchor_pos + 2 * pixel + s01);
                                RGB3 act = samp ? mask(*samp) : RGB3{0xFF, 0xFF, 0xFF};
                                if (band < 4) grade_letters_bit(value, exp, act, letter_ok, letter_pop);
                                else { p4_ok = p4_ok && eq(act, exp); ++p4_pop; }
                            }
                        }
                    }
                }
            }
        }
    } else if (have_anchor) {
        c.record("P1-P4 LA lines (bands 0-4, pixels of value 01/10/11): not graded this run -- PC2w "
                  "(p.101/p.124/p.128, the sprite-pixel-width precondition) failed, so the exact "
                  "pixel-to-sample position the mixed-value LA lines need cannot be trusted.");
    }

    bool pc2e_ok = have_anchor && pop_nonempty_ok && offsets_consistent_ok;

    // ---- PC2: sprite frame (chip-bus content, guard bands, population) -----
    std::vector<uint16_t> s0d, s1d; std::vector<uint64_t> s0c, s1c; std::vector<bool> s0s, s1s;
    bool spr0_seq = false, spr1_seq = false;
    if (graded_frame) {
        spr0_seq = check_full_traversal(colog, kSpr0WordBase, kSprWords, graded_frame->begin, graded_frame->end, s0d, s0c, s0s);
        spr1_seq = check_full_traversal(colog, kSpr1WordBase, kSprWords, graded_frame->begin, graded_frame->end, s1d, s1c, s1s);
    }
    auto exp_s0 = build_expected_sprdesc(0x5060, 0x8200);
    auto exp_s1 = build_expected_sprdesc(0x506C, 0x8201);
    bool spr0_content_ok = spr0_seq, spr1_content_ok = spr1_seq;
    int spr0_mismatches = 0, spr1_mismatches = 0;
    if (spr0_seq) for (int i = 0; i < kSprWords; ++i) if (s0d[i] != exp_s0[i]) { spr0_content_ok = false; ++spr0_mismatches; }
    if (spr1_seq) for (int i = 0; i < kSprWords; ++i) if (s1d[i] != exp_s1[i]) { spr1_content_ok = false; ++spr1_mismatches; }
    bool pc2a_ok = spr0_seq && spr1_seq && spr0_content_ok && spr1_content_ok;

    bool pc2c_ok = have_anchor;
    long pc2c_checked = 0;
    bool pc2d_ok = have_anchor;
    long pc2d_checked = 0;
    if (have_anchor) {
        pc2c_ok = (anchor.a1 > anchor.a0) && (anchor.a1 - anchor.a0 >= 33);
        for (int n = 80; n <= 129; ++n) for (int copy = 0; copy < dm.d; ++copy) {
            auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
            if (!span) { pc2c_ok = false; continue; }
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

    bool pc2_ok = pc2a_ok && pc2c_ok && pc2d_ok && pc2e_ok;
    c.expect(pc2_ok, "PC2: sprite frame -- (a) sprite 0/1 fetch control+50 data pairs in address order, each "
             "word equal to 3.2's own POS/CTL/SPRDESC/terminator values (" +
             std::to_string(spr0_mismatches) + "/" + std::to_string(spr1_mismatches) +
             " content mismatches sprite0/1), (c) guard bands dark and a1-a0>=33 (" +
             std::to_string(pc2c_checked) + " samples), (d) lines 70-79/130-131 fully dark (" +
             std::to_string(pc2d_checked) + " samples), (e) every uniform line of every band/sprite/copy "
             "shows at least one lit sample and the same set of lit offsets every time (" +
             std::to_string(pop_checked) + " line-instances, " + std::to_string(offsets_checked) +
             " offset-set comparisons)");

    // ---- PC3: register contents in place, and as specified -----------------
    // Graded over the Copper PASS that carries the graded frame, not the
    // vsync-bounded frame itself: the Copper restarts every vertical
    // blanking interval (p.25, p.30), so the pass starts before the frame
    // and ends after it.
    bool cr1 = (sc == Scenario::CR1), cr2 = (sc == Scenario::CR2), cr3 = (sc == Scenario::CR3), cr6 = (sc == Scenario::CR6);
    auto expected_cop = build_expected_coptab(cr1, cr2, cr3, cr6);

    std::optional<uint64_t> pass_start, pass_end;
    if (graded_frame) {
        for (auto& a : colog)
            if (!a.is_write && a.word_addr == kCoplistWordBase && a.cycle <= graded_frame->begin)
                if (!pass_start || a.cycle > *pass_start) pass_start = a.cycle;
        if (pass_start)
            for (auto& a : colog)
                if (!a.is_write && a.word_addr == kCoplistWordBase && a.cycle > *pass_start)
                    if (!pass_end || a.cycle < *pass_end) pass_end = a.cycle;
    }
    if (pass_start && pass_end)
        c.note("Copper pass for PC3: [" + std::to_string(*pass_start) + "," + std::to_string(*pass_end) + ")");
    else
        c.note("Copper pass for PC3: not found (no two successive fetches of list word 0 around the graded frame)");

    std::vector<uint16_t> cop_data; std::vector<uint64_t> cop_cyc; std::vector<bool> cop_seen;
    bool pc3_seq_ok = false;
    if (pass_start && pass_end)
        pc3_seq_ok = check_full_traversal(colog, kCoplistWordBase, kCopWords, *pass_start, *pass_end,
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
    c.expect(pc3_ok, "PC3: over the Copper pass containing the graded frame, the Copper fetches every one of "
             "the " + std::to_string(kCopWords) + " instruction words of the list in order and the MOVE data "
             "words equal those of 3.3/3.5 (" + std::to_string(content_mismatches) + " content mismatches); "
             "for bands 1-4, the band's 32-word colour group fetches strictly between the previous band's "
             "last and this band's first sprite data fetch; section T's two colour groups and the dark-set "
             "group all fetch before line 80's first sprite data fetch");

    // ---- PC4: run length -----------------------------------------------
    // A complete pass is every one of the list's 306 words, word 0 to its
    // last WAIT (word 305), fetched once each in address order between two
    // successive fetches of word 0.
    long pass_count = 0;
    if (graded_frame) {
        std::vector<uint64_t> word0_cycles;
        for (auto& a : colog)
            if (!a.is_write && a.word_addr == kCoplistWordBase && a.cycle < graded_frame->begin)
                word0_cycles.push_back(a.cycle);
        std::sort(word0_cycles.begin(), word0_cycles.end());
        for (size_t i = 0; i + 1 < word0_cycles.size(); ++i) {
            std::vector<uint16_t> pd; std::vector<uint64_t> pc; std::vector<bool> ps;
            if (check_full_traversal(colog, kCoplistWordBase, kCopWords, word0_cycles[i], word0_cycles[i + 1], pd, pc, ps))
                ++pass_count;
        }
    }
    bool pc4_ok = marker_seen && graded_frame.has_value() && pass_count >= 3;
    c.expect(pc4_ok, "PC4: MARKER received $A5A5, the graded frame is complete, and " + std::to_string(pass_count) +
             " complete Copper passes (word 0 to word 305 fetched once each, in order, >= 3 required) precede it");

    // ---- P1-P4 -----------------------------------------------------------
    for (int i = 0; i < 18; ++i) {
        auto& m = kLetterMeta[i];
        std::string label = "P" + std::to_string(m.value) + " colour" + std::to_string(16 + m.value) + " " +
                             std::string(m.comp) + " (letter " + std::string(1, m.name) + ") p302 -- every "
                             "graded lit sample of colour" + std::to_string(16 + m.value) +
                             " (bands 0-3, uniform L1/L2/L3 lines width-free plus LA lines while PC2w holds) "
                             "matches the band-predicted value in this bit";
        if (have_anchor) c.expect(letter_ok[L_G + i], label);
        else c.record("not graded (no v0/d anchor found this run): " + label);
        c.record(std::string("letter ") + m.name + " ids (withdrawn, 1.1): " + m.ids + " -- the page does not "
                 "say which register a sprite pixel reads, so a letter check with all positions written "
                 "alike cannot establish one register bit.");
    }
    {
        std::string label = "P4 colour16 letters A-F p302 -- band 4's colours 17-19 match S0's G-X-predicted "
            "values exactly in every graded lit sample (colour16's letters A-F do not show), one joint "
            "verdict for all 24 ids";
        if (have_anchor) c.expect(p4_ok, label);
        else c.record("not graded (no v0/d anchor found this run): " + label);
        c.record(std::string("P4 colour16 letters A-F ids (withdrawn, 1.1): ") + kP4Ids + " -- the page does "
                 "not say which register a sprite pixel reads, so a letter check with all positions written "
                 "alike cannot establish one register bit.");
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
