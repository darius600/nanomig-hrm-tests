// test_ch4_shres_sprite_b.cpp
//
// ch4-shres-sprite-b: see SPECIFICATION.md.
//
// One 68000 program (prog_main.s, or a mutant wrapper selected by
// NMH_MUTATE) drives sprites 0 and 1 through the SuperHires colour
// encoding of p.302 for four bands of pattern lines, plus two pattern
// regions used only to confirm SuperHires is in effect (PC1). This test
// finds the graded frame from the video trace alone (v0/a0/a1/d,
// SPECIFICATION.md section 4), grades that finding itself (REF) and the
// sample unit (SU) before using them, grades P1-P4 per letter from masked
// samples in two lines per letter (uniform lines; LA lines, graded only
// while PC2w holds), and checks PC0-PC4 (plus the PC2w width precondition)
// from the same trace plus the RAM-port log. A line whose precondition is
// not met prints NOT GRADED, never PASS or FAIL.
//
// NMH_MUTATE selects one contrast in place of the baseline stimulus; none
// is exercised by a plain `make test`.
//   NMH_MUTATE=CR1  prog_cr1.bin  (section-T COLOR16-31 = S2 instead of S0)
//   NMH_MUTATE=CR2  prog_cr2.bin  (section-T COLOR16-31 = S1 instead of S0)
//   NMH_MUTATE=CR3  prog_cr3.bin  (band-4 COLOR16-31 = S0-c18 instead of S0x)
//   NMH_MUTATE=CR4  prog_cr4.bin  (BPLCON0 = $9000 instead of $1040)
//   NMH_MUTATE=CR5  prog_cr5.bin  (sprite descriptor terminator = $0001,$0000 instead of $0000,$0000)
//   NMH_MUTATE=CR6  prog_cr6.bin  (band-1 WAIT = $5C07 instead of $5807)
//   NMH_MUTATE=CR7  prog_cr7.bin  (the program stores INTENAR instead of DENISEID at RESULTS+4)
//   NMH_MUTATE=CR8  prog_cr8.bin  (MARKER written after 3 frame starts, not 6)
//   NMH_MUTATE=CR9  prog_cr9.bin  (RESULTS stores $DEAD instead of $BEEF)
//   NMH_MUTATE=CR10 prog_cr10.bin (the program never writes MARKER, but still reaches DONE)
//   NMH_MUTATE=CR11 prog_cr11.bin (every COLOR16-31 data word & $F333: no lit sprite sample)
//   NMH_MUTATE=CR12 prog_cr12.bin (the program stores INTENAR instead of VPOSR at RESULTS+2)
//   NMH_MUTATE=CR13 prog_cr13.bin (SPR1POS = $506E, HSTART 221, instead of $506C, HSTART 217)
//
// Every record below states what fx68k and TG68K each produced running
// `NMH_MUTATE=CRn ./Vnanomig <package directory>` in obj_dir_<core>.
//
// A line whose precondition is not met prints "RECORDED: NOT GRADED: ..." and no PASS/FAIL. The baseline (make -B test) is 28 passed, 1 failed on fx68k and on TG68K: the only red is PC2w; the 19 LA lines (P1-LA to P4-LA) are NOT GRADED because PC2w is red. "P-private" below is a build of this package against a private copy of denise.v outside the repository (spres_d = 2'b01 whenever BPLCON2/BPLCON3 select no sprite resolution, so a sprite pixel is one low-resolution pixel in SuperHires too, the reading of p.101 and p.124): there PC2w passes, every line is graded and the baseline is 48 passed, 0 failed on both cores.
// MUTATION P1 stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- letters G,I,K mismatch, H,J,L unaffected -- measured fx68k: FAIL via G,I,K red, H,J,L green (19 passed, 10 failed overall: these three, P2/P3 below and PC2w; the 19 LA lines NOT GRADED); TG68K: same
// MUTATION P2 stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- letters M,O,Q mismatch, N,P,R unaffected -- measured fx68k: FAIL via M,O,Q red, N,P,R green; TG68K: same
// MUTATION P3 stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- letters S,U,W mismatch, T,V,X unaffected -- measured fx68k: FAIL via S,U,W red, T,V,X green; TG68K: same
// MUTATION P1 stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- S0 $48C to S1 $8C0 changes G,H,J,K,L, I unaffected -- measured fx68k: FAIL via G,H,J,K,L red, I green (14 passed, 15 failed overall: these five, P2/P3 below and PC2w); TG68K: same
// MUTATION P2 stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- S0 $8C0 to S1 $C04 changes N,O,P,R, M,Q unaffected -- measured fx68k: FAIL via N,O,P,R red, M,Q green; TG68K: same
// MUTATION P3 stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- S0 $C04 to S1 $048 changes S,T,V,W,X, U unaffected -- measured fx68k: FAIL via S,T,V,W,X red, U green; TG68K: same
// MUTATION P4 stimulus red NMH_MUTATE=CR3: band-4 COLOR16-31 = S0-c18 instead of S0x -- colour 18's samples mismatch -- measured fx68k: FAIL via P4 colour-18 mismatch, PC0-PC4 green (27 passed, 2 failed overall: P4, baseline PC2w); TG68K: same
// MUTATION P1-LA stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- on the repository RTL every LA line is NOT GRADED (PC2w red), so this record is measured on P-private -- measured P-private fx68k: P1-LA letters G,I,K red, H,J,L green, and the same nine letters red on the uniform-line lines (30 passed, 18 failed overall: the 18 letter lines); TG68K: same
// MUTATION P2-LA stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- measured P-private fx68k: P2-LA letters M,O,Q red, N,P,R green; TG68K: same
// MUTATION P3-LA stimulus red NMH_MUTATE=CR1: section-T COLOR16-31 = S2 instead of S0 in band 0 -- measured P-private fx68k: P3-LA letters S,U,W red, T,V,X green; TG68K: same
// MUTATION P1-LA stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- measured P-private fx68k: P1-LA letters G,H,J,K,L red, I green (20 passed, 28 failed overall: the 28 letter lines of both populations); TG68K: same
// MUTATION P2-LA stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- measured P-private fx68k: P2-LA letters N,O,P,R red, M,Q green; TG68K: same
// MUTATION P3-LA stimulus red NMH_MUTATE=CR2: section-T COLOR16-31 = S1 instead of S0 in band 0 -- measured P-private fx68k: P3-LA letters S,T,V,W,X red, U green; TG68K: same
// MUTATION P4-LA stimulus red NMH_MUTATE=CR3: band-4 COLOR16-31 = S0-c18 instead of S0x -- measured P-private fx68k: P4-LA FAIL together with P4 (46 passed, 2 failed overall: these two); TG68K: same
// MUTATION PC1 stimulus red NMH_MUTATE=CR4: BPLCON0=$9000 (HIRES) instead of $1040 (SHRES) -- pattern runs of two instead of one -- measured fx68k: FAIL via PC1 only; PC2w flips PASS; the 38 P lines (both populations) NOT GRADED because PC1 is not met (9 passed, 1 failed overall, PC1 the only red); TG68K: same
// MUTATION PC2w stimulus green NMH_MUTATE=CR4: already red at baseline (p.101/p.124 predict all 32 samples lit, only 16 measure); the same width-plus-boundary comparator passes 3840/3840 lit and 240/240 boundary samples dark under HIRES, where a low-resolution pixel is one Hires sample pair too -- measured fx68k: PASS; TG68K: same; P-private: PASS with the same counts at baseline and under CR4
// MUTATION PC2 stimulus red NMH_MUTATE=CR5: sprite descriptor terminator = $0001,$0000 instead of $0000,$0000 -- content mismatch after the last graded line, anchor (a0=269, a1=319) and both populations stay live -- measured fx68k: FAIL via PC2(a) content mismatch (1/1 words sprite0/sprite1), PC0/SU/PC1/PC3/PC4 green, the 38 P lines NOT GRADED because PC2 is not met (8 passed, 2 failed overall: PC2, baseline PC2w); TG68K: same
// MUTATION PC3 stimulus red NMH_MUTATE=CR6: band-1 WAIT=$5C07 (line 92) instead of $5807 -- band 1's colour group is fetched after the data fetch of line 90 -- measured fx68k: FAIL via PC3 (ordering) only; the 38 P lines NOT GRADED because PC3 is not met (8 passed, 2 failed overall: PC3, baseline PC2w); TG68K: same
// MUTATION PC0 stimulus red NMH_MUTATE=CR7: the program stores INTENAR (reads $0000 there) instead of DENISEID at RESULTS+4 -- measured fx68k: FAIL via PC0's DENISEID clause (VPOSR=A000, DENISEID=0000), the VPOSR clause and every other precondition green, the 38 P lines NOT GRADED because PC0 is not met (8 passed, 2 failed overall: PC0, baseline PC2w); TG68K: same
// MUTATION PC0 stimulus red NMH_MUTATE=CR12: the program stores INTENAR (reads $0000 there) instead of VPOSR at RESULTS+2 -- measured fx68k: FAIL via PC0's VPOSR clause (VPOSR=0000, DENISEID=FFFC), the DENISEID clause and every other precondition green, the 38 P lines NOT GRADED because PC0 is not met (8 passed, 2 failed overall: PC0, baseline PC2w); TG68K: same
// MUTATION PC4 stimulus red NMH_MUTATE=CR8: MARKER written after 3 frame starts instead of 6 -- one complete, populated graded frame preceded by 1 complete Copper pass (< 3 required) -- measured fx68k: FAIL via PC4's pass count only, PC0-PC3 green, the 38 P lines NOT GRADED because PC4 is not met (8 passed, 2 failed overall: PC4, baseline PC2w); TG68K: same
// MUTATION FIXTURE stimulus red NMH_MUTATE=CR9: RESULTS stores $DEAD instead of $BEEF -- measured fx68k: FAIL via FIXTURE; PC0, which reads RESULTS+2/+4, and the 38 P lines depending on it are NOT GRADED (7 passed, 2 failed overall: FIXTURE, baseline PC2w); TG68K: same
// MUTATION RUN stimulus red NMH_MUTATE=CR10: the program never writes MARKER, but still writes nanomig.i's DONE_ADDR/DONE_VALUE as an independent completion witness -- measured fx68k: FAIL via RUN (marker never seen) only; every other line keeps its baseline verdict, PC4 included (4 complete passes) (27 passed, 2 failed overall: RUN, baseline PC2w); TG68K: same
// MUTATION SU stimulus red NMH_MUTATE=CR13: sprite 1's POS word = $506E (HSTART 221) instead of $506C (HSTART 217), the test's sprite-word mirror following -- a1-a0 = 58 instead of 50 -- measured fx68k: FAIL via SU only; PC1 and PC2w NOT GRADED because SU is not met, the 38 P lines NOT GRADED because PC1 is not met; REF/PC0/PC2/PC3/PC4 green (7 passed, 1 failed overall: SU); TG68K: same; P-private: the same lines move the same way (7 passed, 1 failed)
// MUTATION REF stimulus red NMH_MUTATE=CR11: every COLOR16-31 data word of the Copper list AND $F333 (bits 3-2 of each component cleared), the test's Copper-list mirror following -- sprite DMA and the list structure unchanged but no sprite sample is lit after the 4.1 mask, so no >4-sample run exists -- measured fx68k: FAIL via REF only (v0 not found); SU, PC1, PC2w, PC2 and the 38 P lines NOT GRADED because REF is not met; RUN/FIXTURE/PC0/PC3/PC4 green, and a MEASURED line gives PC2(a) alone as met (5 passed, 1 failed overall: REF); TG68K: same

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
constexpr uint32_t kDoneAddr     = 0x0C0FFE; // nanomig.i DONE_ADDR
constexpr uint16_t kDoneValue    = 0x600D;   // nanomig.i DONE_VALUE

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

// Sprite control words of the unmutated stimulus (SPECIFICATION.md 3.2) and
// sprite 1's POS word under CR13; HSTART = POS bits 7-0 x 2 + CTL bit 0.
constexpr uint16_t kS0Pos = 0x5060, kS0Ctl = 0x8200;
constexpr uint16_t kS1PosBase = 0x506C, kS1PosCr13 = 0x506E, kS1CtlBase = 0x8201;
constexpr long hstart_of(uint16_t pos, uint16_t ctl) { return ((pos & 0xFF) << 1) | (ctl & 1); }

enum class Scenario { Base, CR1, CR2, CR3, CR4, CR5, CR6, CR7, CR8, CR9, CR10, CR11, CR12, CR13 };

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
    if (v == "CR11") return Scenario::CR11;
    if (v == "CR12") return Scenario::CR12;
    if (v == "CR13") return Scenario::CR13;
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
        case Scenario::CR7: return "prog_cr7.bin";
        case Scenario::CR8: return "prog_cr8.bin";
        case Scenario::CR9: return "prog_cr9.bin";
        case Scenario::CR10: return "prog_cr10.bin";
        case Scenario::CR11: return "prog_cr11.bin";
        case Scenario::CR12: return "prog_cr12.bin";
        case Scenario::CR13: return "prog_cr13.bin";
        default: return "prog_main.bin";
    }
}

std::string hex4(uint16_t v) { char b[8]; std::snprintf(b, sizeof b, "%04X", v); return b; }

// ---- Copper list mirror (sprb.i's COPTAB, literal) --------------------
std::vector<uint16_t> build_expected_coptab(bool cr1, bool cr2, bool cr3, bool cr6, bool cr11) {
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

    // CR11: every COLOR16-31 data word has bits 3-2 of each component cleared.
    if (cr11)
        for (size_t i = 0; i + 1 < t.size(); i += 2)
            if (t[i] >= 0x01A0 && t[i] <= 0x01BE && (t[i] & 1) == 0) t[i + 1] &= 0xF333;

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
    cfg.chipset_config = 0x08; // ECS throughout -- Denise stays SuperHires-capable in every scenario
    cfg.memory_config = 0x03; // 2 MB Chip memory
    nmh::NanomigHarness h(cfg);

    std::string bin = dir + "/" + bin_for(sc);
    size_t loaded = h.load_program(bin);
    auto boot = h.boot(kBootBudget);

    // marker_seen/marker_cycle: the program's own MARKER write, which RUN
    // grades directly. done_seen/done_cycle: nanomig.i's own DONE_ADDR
    // write, an independent completion witness a mutant (CR10) reaches
    // without ever writing MARKER. completion_seen/completion_cycle bound
    // the search for the graded frame from whichever of the two the run
    // actually produced, so a run that skips MARKER but still finishes
    // still yields a graded frame for every other check.
    std::optional<uint64_t> marker_cycle;
    bool marker_seen = false;
    std::optional<uint64_t> done_cycle;
    bool done_seen = false;
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
            if (!done_seen) {
                for (auto& a : h.ram_log())
                    if (a.is_write && a.word_addr == kDoneAddr / 2 && a.data == kDoneValue) {
                        done_cycle = a.cycle; done_seen = true; break;
                    }
            }
            bool completion_seen = marker_seen || done_seen;
            const auto& v = h.video();
            for (; video_scan < v.size(); ++video_scan) {
                bool cur = v[video_scan].vs;
                if (cur && !prev_vs && completion_seen) ++frames_after;
                prev_vs = cur;
            }
            if (completion_seen && frames_after >= kFramesAfterMarker) break;
        }
    }
    bool completion_seen = marker_seen || done_seen;
    uint64_t completion_cycle = marker_seen ? *marker_cycle : (done_seen ? *done_cycle : 0);
    bool run_ok = loaded > 0 && boot.cpu_released && marker_seen;

    c.expect(run_ok, "RUN: prog " + bin + " loaded (" + std::to_string(loaded) + " bytes), CPU released from "
             "reset (" + std::to_string(boot.cycles_to_release) + " clk_sys cycles), MARKER received $A5A5 "
             "within budget" + (marker_seen ? (" at cycle " + std::to_string(*marker_cycle)) : " -- not seen"));

    // A line whose precondition is not met is declined, never graded: the
    // record names the line and every precondition that is not met.
    auto not_graded = [&](const std::string& line, const std::string& why) {
        c.record("NOT GRADED: " + line + " -- precondition not met: " + why);
    };
    auto unmet = [](std::initializer_list<std::pair<const char*, bool>> l) {
        std::string s;
        for (auto& p : l) if (!p.second) { if (!s.empty()) s += ", "; s += p.first; }
        return s;
    };

    // ---- fixture: a program-stored constant, read back first ------------
    auto fixture = h.peek_logical(kResults);
    bool fixture_ok = fixture.has_value() && *fixture == 0xBEEF;
    c.expect(fixture_ok, "FIXTURE: prog stores a known constant (BEEF) at RESULTS before anything else is trusted"
             " (read back " + (fixture ? hex4(*fixture) : std::string("nothing")) + ")");

    // ---- PC0: chip identification, measured from the design (the program
    // reads VPOSR and DENISEID back and stores them at RESULTS; the harness's
    // chipset_config/memory_config are its own inputs and are not graded) ---
    auto vposr = h.peek_logical(kResults + 2);
    auto deniseid = h.peek_logical(kResults + 4);
    bool pc0_vposr_ok = vposr.has_value() && (((*vposr >> 8) & 0x7F) == 0x20);
    bool pc0_denise_ok = deniseid.has_value() && ((*deniseid & 0xFF) == 0xFC);
    bool pc0_ok = false;
    if (fixture_ok) {
        pc0_ok = pc0_vposr_ok && pc0_denise_ok;
        c.expect(pc0_ok, "PC0: chip identification measured from the design -- VPOSR bits 14-8 = $20 "
                 "(p.299 \"8368 (hr) ... = 20 for PAL\") and DENISEID bits 7-0 = $FC (p.299 \"The enhanced "
                 "HighRes Denise (8373) will return $FC in the lower 8 bits\") (VPOSR=" +
                 (vposr ? hex4(*vposr) : std::string("--")) +
                 ", DENISEID=" + (deniseid ? hex4(*deniseid) : std::string("--")) + ")");
    } else {
        not_graded("PC0 (chip identification read from RESULTS+2/+4)", "FIXTURE");
    }
    c.record("PC0 configuration (not graded): the harness runs chipset_config=0x" + hex4(cfg.chipset_config) +
             " (ECS) and memory_config=0x" + hex4(cfg.memory_config) + " (2 MB Chip memory); no page "
             "ties the identification registers to the memory size, and this configuration is the test's own input.");

    const auto& video = h.video();
    auto colog = nmh::coalesce(h.ram_log());

    auto frames = nmh::find_frames(video);
    std::optional<nmh::Span> graded_frame;
    if (completion_seen) {
        for (auto& f : frames)
            if (f.end <= completion_cycle && f.end < video.size())
                if (!graded_frame || f.end > graded_frame->end) graded_frame = f;
    }

    auto all_lines = nmh::find_lines(video);
    std::vector<nmh::Span> lif;
    if (graded_frame) lif = lines_in(all_lines, *graded_frame);

    Anchor anchor = find_anchor(video, lif);
    DMeasure dm = measure_d(video, lif, anchor);

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

    // ---- REF: the reference frame itself (SPECIFICATION.md section 4) --
    // v0/a0/a1 (the sprite anchor, located by the page-grounded >4-sample
    // run of 4.3: VSTART=80 is "where the topmost line of the sprite will be
    // positioned", p.107, and the pattern alone reaches at most 1
    // (SuperHires, p.300), 2 (Hires, p.271) or 4 (lores, p.271/p.300)
    // samples) and d (the line-doubler factor, measured, not assumed) are
    // graded here, once, before anything that depends on them. Every later
    // line that uses v0/a0/a1/d prints NOT GRADED when REF is not met.
    bool ref_ok = graded_frame.has_value() && anchor.found && dm.ok;
    c.expect(ref_ok, "REF: reference frame -- the graded frame yields a sprite anchor (v0/a0/a1, the "
             ">4-sample run identified by p.107's VSTART=80 \"where the topmost line of the sprite will be "
             "positioned\" against the pattern-only runs of p.271/p.300) and a consistent line-doubling "
             "factor d, before any check that uses them");

    // ---- SU: the sample unit (SPECIFICATION.md 4.4). Sprite 1 sits 25
    // low-resolution positions right of sprite 0 (HSTART 217 - 192, from the
    // POS/CTL words of the unmutated stimulus, p.107); one position is
    // 140 ns (p.303 SH0 "140 ns increment"); one sample is 70 ns (every
    // second 35 ns SuperHires pixel is shown, p.300). So a1 - a0 must be 50.
    const long kSuHstartDelta = hstart_of(kS1PosBase, kS1CtlBase) - hstart_of(kS0Pos, kS0Ctl);
    const long kSuExpected = kSuHstartDelta * 140 / 70;
    bool su_ok = false;
    if (ref_ok) {
        su_ok = (long)(anchor.a1 - anchor.a0) == kSuExpected;
        c.expect(su_ok, "SU: sample unit -- a1 - a0 = " + std::to_string(kSuExpected) + " samples: HSTART " +
                 std::to_string(hstart_of(kS1PosBase, kS1CtlBase)) + " - " + std::to_string(hstart_of(kS0Pos, kS0Ctl)) +
                 " = " + std::to_string(kSuHstartDelta) + " low-resolution positions (p.107) of 140 ns "
                 "(p.303 SH0 \"140 ns increment\"), at 70 ns per sample (every second 35 ns SuperHires pixel, p.300) "
                 "(a0=" + std::to_string(anchor.a0) + ", a1=" + std::to_string(anchor.a1) + ", a1-a0=" +
                 std::to_string(anchor.a1 - anchor.a0) + ")");
    } else {
        not_graded("SU (sample unit, a1 - a0 = 50)", "REF");
    }

    // ---- PC1: SuperHires in effect around the sprite band ------------------
    bool pc1_ok = false;
    {
        long pc1_checked = 0;
        bool pc1_pass = true;
        if (ref_ok && su_ok) {
            auto check_region = [&](int lo, int hi) {
                for (int n = lo; n <= hi; ++n) for (int copy = 0; copy < dm.d; ++copy) {
                    auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                    if (!span) { pc1_pass = false; continue; }
                    for (size_t off = anchor.a0; off < anchor.a0 + 99; ++off) {
                        auto s1 = nmh::sample_at(video, *span, off);
                        auto s2 = nmh::sample_at(video, *span, off + 1);
                        ++pc1_checked;
                        if (!s1 || !s2) { pc1_pass = false; continue; }
                        bool cur = !dark(mask(*s1)), nxt = !dark(mask(*s2));
                        if (cur == nxt) pc1_pass = false;
                    }
                }
            };
            check_region(46, 55);
            check_region(140, 160);
        }
        if (!(ref_ok && su_ok)) {
            not_graded("PC1 (SuperHires in effect)", unmet({{"REF", ref_ok}, {"SU", su_ok}}));
        } else if (pc1_checked == 0) {
            not_graded("PC1 (SuperHires in effect)", "empty population (0 transitions)");
        } else {
            pc1_ok = pc1_pass;
            c.expect(pc1_ok, "PC1: SuperHires in effect -- samples a0..a0+99 on source lines 46-55 and 140-160 of the "
                      "graded frame alternate lit/dark with runs of exactly one sample (" +
                      std::to_string(pc1_checked) + " transitions checked); that BPLCON0 is written only once (step 3) "
                      "and the graded pass's Copper fetches contain no MOVE to $0100 is established by PC3's own exact "
                      "content match, not repeated here");
        }
    }

    // ---- PC2w: sprite pixel width (precondition; gates only the LA-line
    // grading of P1-P4 below). A sprite is 16 pixels of one low-resolution
    // position each; SU gives the samples per position, (a1 - a0) / 25 = 2,
    // so the width is 16 x 2 = 32 samples. "Exactly" is graded both ways:
    // every in-interval sample must be lit, and the sample immediately
    // outside each edge (a0-1/a0+width, a1-1/a1+width) must be dark, both in
    // the same boolean. Expected to fail against the printed page: that
    // failure is a result, not something to repair.
    bool pc2w_graded = false, pc2w_ok = false;
    long pc2w_lit = 0, pc2w_dark = 0, pc2w_checked = 0;
    long pc2w_boundary_lit = 0, pc2w_boundary_dark = 0, pc2w_boundary_checked = 0;
    long pc2w_width = 0;
    if (ref_ok && su_ok) {
        pc2w_width = 16 * (long)(anchor.a1 - anchor.a0) / kSuHstartDelta;
        bool pass = true;
        for (int band = 0; band < 5; ++band) {
            for (int lineInBand = 0; lineInBand < 6; ++lineInBand) { // L1,L1,L2,L2,L3,L3
                int n = 80 + 10 * band + lineInBand;
                for (int copy = 0; copy < dm.d; ++copy) {
                    auto span = src_line_copy(lif, anchor.v0_idx, dm.d, n, copy);
                    if (!span) { pass = false; continue; }
                    for (int sprite = 0; sprite < 2; ++sprite) {
                        size_t anchor_pos = sprite == 0 ? anchor.a0 : anchor.a1;
                        for (long off = 0; off < pc2w_width; ++off) {
                            ++pc2w_checked;
                            auto samp = nmh::sample_at(video, *span, anchor_pos + (size_t)off);
                            bool lit = samp && !dark(mask(*samp));
                            if (lit) ++pc2w_lit; else ++pc2w_dark;
                            if (!lit) pass = false;
                        }
                        for (long boff : {-1L, pc2w_width}) {
                            ++pc2w_boundary_checked;
                            std::optional<nmh::VideoSample> samp;
                            if (boff >= 0 || anchor_pos >= 1) // guard the a0-1 underflow case
                                samp = nmh::sample_at(video, *span, (size_t)((long)anchor_pos + boff));
                            bool lit = samp && !dark(mask(*samp));
                            if (lit) ++pc2w_boundary_lit; else ++pc2w_boundary_dark;
                            if (lit || !samp) pass = false;
                        }
                    }
                }
            }
        }
        if (pc2w_checked > 0) { pc2w_graded = true; pc2w_ok = pass; }
    }
    if (pc2w_graded) {
        c.expect(pc2w_ok, "PC2w: on every L1/L2/L3 line of the graded frame, sprite 0 is lit exactly over samples "
                 "a0..a0+" + std::to_string(pc2w_width - 1) + " and dark at a0-1/a0+" + std::to_string(pc2w_width) +
                 ", and sprite 1 exactly over a1..a1+" + std::to_string(pc2w_width - 1) + " and dark at a1-1/a1+" +
                 std::to_string(pc2w_width) + " (the width is 16 pixels x (a1-a0)/25 = 2 samples per low-resolution "
                 "pixel, from SU; p.101 \"Sprites, however, are independent of playfield modes of display, so changing "
                 "the resolution or interlace mode of the playfield has no effect on the size or resolution of a "
                 "sprite\"; p.124 \"The shift occurs once during each low resolution pixel time\") (" +
                 std::to_string(pc2w_lit) + " lit, " + std::to_string(pc2w_dark) + " dark of " +
                 std::to_string(pc2w_checked) + " interval samples checked; " + std::to_string(pc2w_boundary_dark) +
                 " of " + std::to_string(pc2w_boundary_checked) + " boundary samples dark as required)");
    } else {
        not_graded("PC2w (sprite pixel width)", unmet({{"REF", ref_ok}, {"SU", su_ok}}));
    }
    c.record("PC2w page basis: p.101 \"Sprites, however, are independent of playfield modes of display, so "
             "changing the resolution or interlace mode of the playfield has no effect on the size or "
             "resolution of a sprite.\"; p.124 \"The shift occurs once during each low resolution pixel time "
             "and continues until all 16 bits have been transferred to the display circuitry.\" A red here is the "
             "result the page predicts being measured, not a defect to repair; PC2w gates only the LA-line "
             "grading of P1-P4, and PC2(a)/(c)/(d)/(e) do not depend on it.");

    // ---- uniform-line scan: P1-P4's uniform-line population and PC2(e) ---
    // Uniform lines (L1/L2/L3) are graded width-free: every LIT sample in
    // [a0,a1) (sprite 0) / [a1,a1+(a1-a0)) (sprite 1) is graded against the
    // band's expected value, whatever the measured width turns out to be
    // (SPECIFICATION.md section 6). LA lines need the pixel-exact a0+2i
    // mapping (4.4) and are graded only while PC2w holds.
    std::array<bool, kNumLetters> letter_ok_u; letter_ok_u.fill(true);
    std::array<long, kNumLetters> letter_pop_u; letter_pop_u.fill(0);
    std::array<bool, kNumLetters> letter_ok_la; letter_ok_la.fill(true);
    std::array<long, kNumLetters> letter_pop_la; letter_pop_la.fill(0);
    bool p4_ok_u = true, p4_ok_la = true; long p4_pop_u = 0, p4_pop_la = 0;

    // PC2(e): population and offset-consistency invariant across the same
    // uniform-line scan (replaces a fixed sample count, which would assume
    // the very width PC2w tests).
    bool pop_nonempty_ok = true; long pop_checked = 0;
    bool offsets_consistent_ok = true; long offsets_checked = 0;
    std::optional<std::vector<size_t>> ref_offsets;

    size_t win_len = ref_ok ? (anchor.a1 - anchor.a0) : 0;
    if (ref_ok && win_len > 0) {
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
                                    if (band < 4) grade_letters_bit(value, exp, act, letter_ok_u, letter_pop_u);
                                    else { p4_ok_u = p4_ok_u && eq(act, exp); ++p4_pop_u; }
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

    // ---- PC2: sprite frame (chip-bus content, guard bands, population) -----
    std::vector<uint16_t> s0d, s1d; std::vector<uint64_t> s0c, s1c; std::vector<bool> s0s, s1s;
    bool spr0_seq = false, spr1_seq = false;
    if (graded_frame) {
        spr0_seq = check_full_traversal(colog, kSpr0WordBase, kSprWords, graded_frame->begin, graded_frame->end, s0d, s0c, s0s);
        spr1_seq = check_full_traversal(colog, kSpr1WordBase, kSprWords, graded_frame->begin, graded_frame->end, s1d, s1c, s1s);
    }
    auto exp_s0 = build_expected_sprdesc(kS0Pos, kS0Ctl);
    auto exp_s1 = build_expected_sprdesc(sc == Scenario::CR13 ? kS1PosCr13 : kS1PosBase, kS1CtlBase);
    bool spr0_content_ok = spr0_seq, spr1_content_ok = spr1_seq;
    int spr0_mismatches = 0, spr1_mismatches = 0;
    if (spr0_seq) for (int i = 0; i < kSprWords; ++i) if (s0d[i] != exp_s0[i]) { spr0_content_ok = false; ++spr0_mismatches; }
    if (spr1_seq) for (int i = 0; i < kSprWords; ++i) if (s1d[i] != exp_s1[i]) { spr1_content_ok = false; ++spr1_mismatches; }
    bool pc2a_ok = spr0_seq && spr1_seq && spr0_content_ok && spr1_content_ok;

    bool pc2_ok = false;
    if (ref_ok) {
        bool pc2c_ok = (anchor.a1 > anchor.a0) && (anchor.a1 - anchor.a0 >= 33);
        long pc2c_checked = 0;
        bool pc2d_ok = true;
        long pc2d_checked = 0;
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
        bool pc2e_ok = pop_nonempty_ok && offsets_consistent_ok;

        pc2_ok = pc2a_ok && pc2c_ok && pc2d_ok && pc2e_ok;
        c.expect(pc2_ok, "PC2: sprite frame -- (a) sprite 0/1 fetch control+50 data pairs in address order, each "
                 "word equal to 3.2's own POS/CTL/SPRDESC/terminator values (" +
                 std::to_string(spr0_mismatches) + "/" + std::to_string(spr1_mismatches) +
                 " content mismatches sprite0/1), (c) guard bands dark and a1-a0>=33 (" +
                 std::to_string(pc2c_checked) + " samples), (d) lines 70-79/130-131 fully dark (" +
                 std::to_string(pc2d_checked) + " samples), (e) every uniform line of every band/sprite/copy "
                 "shows at least one lit sample and the same set of lit offsets every time (" +
                 std::to_string(pop_checked) + " line-instances, " + std::to_string(offsets_checked) +
                 " offset-set comparisons)");
    } else {
        not_graded("PC2 (sprite frame: (a) chip-bus content, (c) guard bands, (d) dark lines, (e) population)", "REF");
        c.note("PC2(a) alone, not graded this run: " + std::string(pc2a_ok ? "sprite 0/1 fetched in order with every word as expected" :
               "sprite fetches or content differ from 3.2 (") + (pc2a_ok ? "" : std::to_string(spr0_mismatches) + "/" +
               std::to_string(spr1_mismatches) + " content mismatches sprite0/1)"));
    }

    // ---- PC3: register contents in place, and as specified -----------------
    // Graded over the Copper pass that carries the graded frame, not the
    // vsync-bounded frame itself: the Copper restarts every vertical
    // blanking interval (p.25, p.30). The pass is the span from the last
    // fetch of the list's word 0 at or before the graded frame's first
    // sample to the next such fetch; it begins before the frame's first
    // sample and ends inside the frame.
    bool cr1 = (sc == Scenario::CR1), cr2 = (sc == Scenario::CR2), cr3 = (sc == Scenario::CR3), cr6 = (sc == Scenario::CR6);
    bool cr11 = (sc == Scenario::CR11);
    auto expected_cop = build_expected_coptab(cr1, cr2, cr3, cr6, cr11);

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
    // successive fetches of word 0. MARKER's receipt is RUN's, not PC4's, so
    // PC4 does not depend on RUN.
    bool pc4_ok = false;
    {
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
        pc4_ok = graded_frame.has_value() && pass_count >= 3;
        c.expect(pc4_ok, "PC4: the graded frame is complete, and " + std::to_string(pass_count) +
                 " complete Copper passes (word 0 to word 305 fetched once each, in order, >= 3 required) precede it");
    }

    // ---- P1-P4: per-letter masked-colour grading -----------------------
    // Every P line is graded only when PC0, REF, PC1, PC2, PC3 and PC4 are
    // met (each graded and passed). Each letter has two lines over disjoint
    // populations: the uniform-line line (graded width-free) and the LA-line
    // line (pixel-exact mapping of 4.4, graded only while PC2w holds). Claim
    // ids sit on the RECORDED lines below, on no PASS/FAIL line: neither
    // population alone is the whole claim, and 1.1 withdraws them anyway.
    bool p_gate = pc0_ok && ref_ok && pc1_ok && pc2_ok && pc3_ok && pc4_ok;
    std::string p_unmet = unmet({{"PC0", pc0_ok}, {"REF", ref_ok}, {"PC1", pc1_ok}, {"PC2", pc2_ok},
                                  {"PC3", pc3_ok}, {"PC4", pc4_ok}});

    // LA lines: mixed pixel values need the pixel-exact a0+2i/a1+2i mapping
    // (4.4), so they are graded only while PC2w holds.
    bool la_gate = p_gate && pc2w_graded && pc2w_ok;
    if (la_gate) {
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
                                if (band < 4) grade_letters_bit(value, exp, act, letter_ok_la, letter_pop_la);
                                else { p4_ok_la = p4_ok_la && eq(act, exp); ++p4_pop_la; }
                            }
                        }
                    }
                }
            }
        }
    }
    std::string la_unmet = p_unmet;
    if (p_gate && !(pc2w_graded && pc2w_ok)) la_unmet = pc2w_graded ? "PC2w (failed)" : "PC2w (not graded)";

    for (int i = 0; i < 18; ++i) {
        auto& m = kLetterMeta[i];
        std::string tag = "P" + std::to_string(m.value);
        std::string what = " colour" + std::to_string(16 + m.value) + " " + std::string(m.comp) + " (letter " +
                           std::string(1, m.name) + ") p302 -- ";
        std::string label = tag + what + "every graded lit sample of colour" + std::to_string(16 + m.value) +
                            " on the uniform L1/L2/L3 lines of bands 0-3 (width-free) matches the band-predicted "
                            "value in this bit (" + std::to_string(letter_pop_u[L_G + i]) + " samples)";
        std::string label_la = tag + "-LA" + what + "every colour" + std::to_string(16 + m.value) +
                               " pixel of the LA lines of bands 0-3 (pixel-exact a0+2i mapping, two samples per "
                               "pixel) matches the band-predicted value in this bit (" +
                               std::to_string(letter_pop_la[L_G + i]) + " samples)";
        if (!p_gate) not_graded(tag + what + "uniform lines", p_unmet);
        else if (letter_pop_u[L_G + i] == 0) not_graded(tag + what + "uniform lines", "empty population");
        else c.expect(letter_ok_u[L_G + i], label);
        if (!la_gate) not_graded(tag + "-LA" + what + "LA lines", la_unmet);
        else if (letter_pop_la[L_G + i] == 0) not_graded(tag + "-LA" + what + "LA lines", "empty population");
        else c.expect(letter_ok_la[L_G + i], label_la);
        c.record(std::string("letter ") + m.name + " ids (withdrawn, 1.1): " + m.ids + " -- the page does not "
                 "say which register a sprite pixel reads, so a letter check with all positions written "
                 "alike cannot establish one register bit.");
    }
    {
        std::string label = "P4 colour16 letters A-F p302 -- band 4's colours 17-19 match S0's G-X-predicted "
            "values exactly in every graded lit sample of the uniform L1/L2/L3 lines (colour16's letters A-F do not "
            "show) (" + std::to_string(p4_pop_u) + " samples)";
        std::string label_la = "P4-LA colour16 letters A-F p302 -- band 4's colours 17-19 match S0's G-X-predicted "
            "values exactly in every pixel of value 01, 10 or 11 of the LA lines (pixel-exact a0+2i mapping) (" +
            std::to_string(p4_pop_la) + " samples)";
        if (!p_gate) not_graded("P4 colour16 letters A-F, uniform lines", p_unmet);
        else if (p4_pop_u == 0) not_graded("P4 colour16 letters A-F, uniform lines", "empty population");
        else c.expect(p4_ok_u, label);
        if (!la_gate) not_graded("P4-LA colour16 letters A-F, LA lines", la_unmet);
        else if (p4_pop_la == 0) not_graded("P4-LA colour16 letters A-F, LA lines", "empty population");
        else c.expect(p4_ok_la, label_la);
        c.record(std::string("P4 colour16 letters A-F ids (withdrawn, 1.1): ") + kP4Ids + " -- the page does "
                 "not say which register a sprite pixel reads, so a letter check with all positions written "
                 "alike cannot establish one register bit.");
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
