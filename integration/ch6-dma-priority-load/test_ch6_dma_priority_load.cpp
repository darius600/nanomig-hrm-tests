// test_ch6_dma_priority_load.cpp -- ch6-dma-priority-load. See SPECIFICATION.md for every proposition,
// precondition, program and measured value; this file implements what it commits to.
//
// NMH_MUTATE=<label> replaces one run by its contrasting case (a different loaded image or program) and
// evaluates the group of that label with the unchanged comparators; the normal run never runs a mutant.
// Both cores gave the same result for every record below.
//
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXTURE: prog_load_fixture_mut.s stores $DEAD instead of $BEEF; FIXTURE reads FAIL
// MUTATION FIG69 none impossible NMH_MUTATE=-: unfalsifiable at this boundary: the four refresh cycles per line, 16 clk_sys apart and 1816 clk_sys per line, do not depend on anything the program or the loaded image sets (the NTSC run measured the same pattern)
// MUTATION C1 stimulus red NMH_MUTATE=C1: prog_load_nop.s has one NOP before S3 in the BLT run; C1 reads FAIL (first difference at a program fetch, before the named access); P1 not graded; C2, C3, C4, C6, C7 stay PASS
// MUTATION C2 stimulus red NMH_MUTATE=C2: the BLT program writes DMACON $83FF while $87FF is declared; C2 reads FAIL; P1 not graded; C1, C3, C4, C6, C7 stay PASS
// MUTATION C3 stimulus red NMH_MUTATE=C3: the BLT run with CPU = LOOP; C3 reads FAIL; P1 not graded; the other C lines stay PASS
// MUTATION C4 stimulus red NMH_MUTATE=C4: the last stream MOVE of the BLT run replaced by the WAIT $F801,$FF00; C4 reads FAIL (C3 reads FAIL as well); P1 not graded
// MUTATION C5 stimulus red NMH_MUTATE=C5: prog_load_c5.s (a poll for two vertical blanks between S4.10 and S4.11, INTREQ <- $0010 before STOP) for REF and CPU; C5 reads FAIL; C3 and P2-b read FAIL as well (REF's own polling accesses fall in field F); P2 not graded
// MUTATION C6 stimulus red NMH_MUTATE=C6: the CPU run writes the handler address to $6E; C6 reads FAIL (C1 reads FAIL as well: the write to $6E differs from the reference run's write to $6C before the named access); P2 not graded
// MUTATION C7 stimulus red NMH_MUTATE=C7: the BLT run with NTSC selected; C7 reads FAIL (C1 and C3 read FAIL as well); P1 not graded
// MUTATION P1 stimulus red NMH_MUTATE=P1: BLT with item 5 = $5101,$FF00; P1 reads FAIL through the Copper's fetch times (first mismatch: element 0, cycle 2988969 against 2987153 on both cores; item 5's first word is exempt from the data comparison); P1-a, P1-b and every C line stay PASS
// MUTATION P1-a stimulus red NMH_MUTATE=P1-a: BLT with item 4 data $0100 (4 rows); P1-a reads FAIL; P1 not graded
// MUTATION P1-b stimulus red NMH_MUTATE=P1-b: BLT-NOCOP with BLIT = NO-OP (no blit in the unopposed run); P1-b reads FAIL; P1 not graded
// MUTATION P2 stimulus red NMH_MUTATE=P2: CPU with item 5 = $5101,$FF00; P2 reads FAIL through the fetch times (first mismatch: element 54, cycle 2988969 against 2987153); P2-a, P2-b and every C line stay PASS
// MUTATION P2-a stimulus red NMH_MUTATE=P2-a: CPU with CPU = STOP (REF itself); P2-a reads FAIL; P2 not graded
// MUTATION P2-b stimulus red NMH_MUTATE=P2-b: CPU-NOCOP with CPU = STOP; P2-b reads FAIL; P2 not graded
// MUTATION P3 stimulus red NMH_MUTATE=P3: REF with item 5 = $5101,$FF00 against NOFIX; P3 reads FAIL through the fetch times (first mismatch: element 54, cycle 2988969 against 2987153); P3-a, P3-b and every C line stay PASS
// MUTATION P3-a stimulus red NMH_MUTATE=P3-a: REF with FIX = none; P3-a reads FAIL; P3 not graded
// MUTATION P3-b stimulus red NMH_MUTATE=P3-b: NOFIX with FIX = lowres; P3-b reads FAIL; P3 not graded
// MUTATION P4 stimulus red NMH_MUTATE=P4: BLT with AUD0PER = 200; P4 reads FAIL for audio channel 0 only (22 fetches against 34, first at cycle 2960001 against 2961817); this shows the comparator's sensitivity, no page predicts the delay; P4-a, P4-b and every C line stay PASS
// MUTATION P4-a stimulus red NMH_MUTATE=P4-a: BLT and REF with DDFSTRT = $0030; P4-a reads FAIL; P4 not graded
// MUTATION P4-b stimulus red NMH_MUTATE=P4-b: BLT-NOFIX with BLIT = NO-OP; P4-b reads FAIL; P4 not graded
// MUTATION P5a stimulus red NMH_MUTATE=P5a: REF with AUD0PER = 200 against NOCOP; P5a reads FAIL for audio channel 0 only (3 fetches against 5); P5b reads FAIL as well since REF is also its reference; sensitivity only, no page predicts the delay
// MUTATION P5b stimulus red NMH_MUTATE=P5b: CPU with AUD0PER = 200 against REF; P5b reads FAIL for audio channel 0 only (141 fetches against 227); sensitivity only, no page predicts the delay
// MUTATION P5-a stimulus red NMH_MUTATE=P5-a: REF and CPU with FIX = none; P5-a reads FAIL; P5a and P5b not graded
// MUTATION P6a stimulus red NMH_MUTATE=P6a: HIRES-LOAD with BPL1MOD = 2; P6a reads FAIL (first mismatch: element 2, address $4A668 against $4A640); P6-a, P6-b, P6-c and every C line stay PASS
// MUTATION P6b stimulus red NMH_MUTATE=P6b: HIRES-LOAD with item 5 = $E001,$FF00 and item 4 data $0040; P6-b reads FAIL (necessarily, as nothing is then pending across a gap) and so does P6-a; P6a and P6b not graded
// MUTATION P6-a stimulus red NMH_MUTATE=P6-a: HIRES-REF with BPLCON0 = $8200 (no planes); P6-a reads FAIL (P6-c reads FAIL as well: HIRES-REF has no bitplane fetches); P6a not graded
// MUTATION P6-b stimulus red NMH_MUTATE=P6-b: the contrast of P6b (HIRES-LOAD with item 5 = $E001,$FF00 and item 4 data $0040); P6-b reads FAIL, P6-a reads FAIL as well
// MUTATION P6-c stimulus red NMH_MUTATE=P6-c: BLT-NOFIX with BLIT = NO-OP; P6-c reads FAIL; P6a not graded

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

constexpr uint32_t kK0 = 0x10000, kL1 = 0x10100;
constexpr uint32_t kBA = 0x20000, kBD = 0x28000;
constexpr uint32_t kP0 = 0x40000, kS0 = 0x60000, kAU0 = 0x62000, kDK0 = 0x66000;
constexpr uint32_t kVT = 0x0C1000;
constexpr uint32_t kResults = 0x0C0000, kDoneAddr = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint32_t kRVec = kResults, kRDma = kResults + 4, kRFix = kResults + 6, kRMark = kResults + 8;
constexpr uint32_t kA2 = kResults + 0x200;
constexpr uint32_t kHandler = 0x080800;
constexpr uint32_t kCodeLo = 0x080400, kCodeHi = 0x080800;
constexpr uint64_t kBootBudget = 8'000'000;
constexpr uint64_t kRunBudget = 6'000'000;

// byte offsets inside L1 (SPECIFICATION.md 1.6)
constexpr uint32_t kItem3 = 96, kItem4 = 100, kItem5 = 104, kItem6 = 108, kItem7 = 1132, kItem8 = 1136;

enum Ch { K = 0, BA, BD, PL1, PL2, PL3, PL4, SP0, SP1, SP2, SP3, SP4, SP5, SP6, SP7,
          AU0, AU1, AU2, AU3, DK, OTH, NCH };
constexpr int kFirstFixed = PL1, kLastFixed = DK;   // 17 channels
constexpr uint32_t kMaskBlit = (1u << BA) | (1u << BD);
constexpr uint32_t kMaskPlanes = (1u << PL1) | (1u << PL2) | (1u << PL3) | (1u << PL4);
constexpr uint32_t kMaskK = 1u << K;
constexpr uint32_t kMaskFixed = ((1u << (kLastFixed + 1)) - 1) & ~((1u << kFirstFixed) - 1);
constexpr uint32_t kMaskOth = 1u << OTH;

const char* ch_name(int c) {
    static const char* n[] = {"K", "BA", "BD", "PL1", "PL2", "PL3", "PL4", "SP0", "SP1", "SP2", "SP3",
                              "SP4", "SP5", "SP6", "SP7", "AU0", "AU1", "AU2", "AU3", "DK", "OTH"};
    return n[c];
}

int chan_of(uint32_t wa, bool hires) {
    uint32_t b = wa * 2;
    if (b >= kK0 && b < kK0 + 0x1000) return K;
    if (b >= kBA && b < kBA + 0x1000) return BA;
    if (b >= kBD && b < kBD + 0x1000) return BD;
    if (b >= kP0 && b < kP0 + 0x14000) {
        uint32_t sz = hires ? 0x5000 : 0x2800;
        uint32_t p = (b - kP0) / sz;
        return p < 4 ? PL1 + int(p) : OTH;
    }
    if (b >= kS0 && b < kS0 + 0x2000) return SP0 + int((b - kS0) / 0x400);
    if (b >= kAU0 && b < kAU0 + 0x4000) return AU0 + int((b - kAU0) / 0x1000);
    if (b >= kDK0 && b < kDK0 + 0x4000) return DK;
    return OTH;
}

// ---- a run's definition ---------------------------------------------------
struct Spec {
    std::string bin = "prog_load.bin";
    int fix = 0;                 // 0 lowres, 1 none, 2 hires
    bool blit = false;           // item 4 writes BLTSIZE
    uint16_t blt_data = 0x0800;
    uint16_t item5 = 0x5001;     // first word of the WAIT that starts the stream
    bool loop = false;           // S5 variant
    uint16_t aud0per = 124;
    uint16_t bpl1mod = 0;
    uint16_t dmacon = 0;         // 0: from fix
    uint16_t dmacon_decl = 0;    // the value the test declares (0: the written one)
    uint16_t bplcon0 = 0;        // 0: from fix
    uint16_t ddfstrt = 0;        // 0: from fix
    bool ntsc = false;
    bool last_wait = false;      // last stream MOVE replaced by a WAIT
    bool vec_wrong = false;      // handler address written to $6E
    bool hires() const { return fix == 2; }
    uint16_t dmacon_v() const {
        if (dmacon) return dmacon;
        return fix == 0 ? 0x87FF : fix == 1 ? 0x86C0 : 0x87C0;
    }
    uint16_t bplcon0_v() const { return bplcon0 ? bplcon0 : (fix == 2 ? 0xC200 : 0x4200); }
    uint16_t ddfstrt_v() const { return ddfstrt ? ddfstrt : (fix == 2 ? 0x003C : 0x0038); }
    uint16_t ddfstop_v() const { return fix == 2 ? 0x00D4 : 0x00D0; }
};

Spec mk(int fix, bool blit, int stream_line, bool loop) {
    Spec s;
    s.fix = fix; s.blit = blit; s.loop = loop;
    s.item5 = uint16_t((stream_line << 8) | 1);
    return s;
}

uint32_t plane_base(const Spec& s, int p) { return kP0 + uint32_t(p) * (s.hires() ? 0x5000 : 0x2800); }
// words per plane and line from the data-fetch registers (p.60: DDFSTRT = DDFSTOP - 8 * (words - 1) low
// resolution, DDFSTRT = DDFSTOP - 4 * (words - 2) high resolution); 20 and 40 at the normal values
uint32_t line_words(const Spec& s) {
    uint32_t span = uint32_t(s.ddfstop_v()) - uint32_t(s.ddfstrt_v());
    return s.hires() ? span / 4 + 2 : span / 8 + 1;
}
uint32_t line_bytes(const Spec& s) { return 2 * line_words(s); }

struct Img {
    nmh::NanomigHarness& h;
    void w(uint32_t byte_addr, uint16_t v) { h.poke_word(byte_addr / 2, v); }
    void l(uint32_t byte_addr, uint32_t v) { w(byte_addr, uint16_t(v >> 16)); w(byte_addr + 2, uint16_t(v)); }
};

uint16_t pat(uint32_t byte_addr, uint16_t c) { return uint16_t((byte_addr >> 1) ^ c); }

void place_image(nmh::NanomigHarness& h, const Spec& s) {
    Img m{h};
    for (uint32_t i = 0; i < 2048; ++i) m.w(kBA + 2 * i, pat(kBA + 2 * i, 0x3C5A));
    for (uint32_t a = kP0; a < kP0 + 0x14000; a += 2) m.w(a, pat(a, 0x96C3));
    for (uint32_t x = 0; x < 8; ++x) {
        uint32_t b = kS0 + 0x400 * x;
        m.w(b, uint16_t(0x3000 + 0x30 + 8 * x));
        m.w(b + 2, 0xF000);
        for (uint32_t i = 0; i < 192; ++i) {
            m.w(b + 4 + 4 * i, pat(b + 4 + 4 * i, 0x5AA5));
            m.w(b + 6 + 4 * i, pat(b + 6 + 4 * i, 0xA55A));
        }
        m.w(b + 4 + 4 * 192, 0);
        m.w(b + 6 + 4 * 192, 0);
    }
    for (uint32_t x = 0; x < 4; ++x)
        for (uint32_t i = 0; i < 2048; ++i) {
            uint32_t a = kAU0 + 0x1000 * x + 2 * i;
            m.w(a, pat(a, uint16_t(0x1111 * (x + 1))));
        }
    for (uint32_t i = 0; i < 8000; ++i) m.w(kDK0 + 2 * i, pat(kDK0 + 2 * i, 0xC3A5));
    // Copper lists
    m.w(kK0, 0xFFFF); m.w(kK0 + 2, 0xFFFE);
    uint32_t a = kL1;
    auto mv = [&](uint16_t reg, uint16_t data) { m.w(a, reg); m.w(a + 2, data); a += 4; };
    for (uint32_t x = 0; x < 8; ++x) {
        uint32_t sp = kS0 + 0x400 * x;
        mv(uint16_t(0x120 + 4 * x), uint16_t(sp >> 16));
        mv(uint16_t(0x122 + 4 * x), uint16_t(sp));
    }
    for (int p = 0; p < 4; ++p) {
        uint32_t pb = plane_base(s, p);
        mv(uint16_t(0x0E0 + 4 * p), uint16_t(pb >> 16));
        mv(uint16_t(0x0E2 + 4 * p), uint16_t(pb));
    }
    mv(0x4001, 0xFF00);                                   // item 3
    mv(s.blit ? 0x0058 : 0x01FE, s.blt_data);             // item 4
    mv(s.item5, 0xFF00);                                  // item 5
    for (uint32_t i = 0; i < 256; ++i) {
        if (s.last_wait && i == 255) mv(0xF801, 0xFF00);
        else mv(0x01FE, uint16_t(i));
    }
    mv(0xF801, 0xFF00);                                   // item 7
    mv(0x009C, 0x8010);                                   // item 8
    mv(0xFFFF, 0xFFFE);                                   // item 9
    // variant table
    m.w(kVT + 0, s.loop ? 1 : 0);
    m.w(kVT + 2, s.dmacon_v());
    m.w(kVT + 4, s.bplcon0_v());
    m.w(kVT + 6, s.ddfstrt_v());
    m.w(kVT + 8, s.ddfstop_v());
    m.w(kVT + 10, s.bpl1mod);
    m.w(kVT + 12, 0);
    m.w(kVT + 14, s.aud0per);
    m.l(kVT + 16, s.vec_wrong ? 0x6E : 0x6C);
    for (int p = 0; p < 4; ++p) m.l(kVT + 20 + 4 * p, plane_base(s, p));
}

// ---- a run --------------------------------------------------------------------
struct Run {
    Spec spec;
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> raw;   // the whole port log
    std::vector<nmh::RamAccess> acc;   // after removing R
    std::vector<uint8_t> ch;           // channel of each acc
    uint16_t r_vec_hi = 0, r_vec_lo = 0, r_dma = 0, r_fix = 0, r_mark = 0;
    bool hasF = false;
    uint64_t fs = 0, fe = 0;           // field F
    std::vector<std::pair<uint64_t, uint32_t>> keys;   // sorted (cycle, word address) of raw
    uint64_t base = 0;                 // cycle of refv[0]
    std::vector<uint8_t> refv;         // the `refresh` output, one entry per clk_sys cycle
    std::vector<uint64_t> redges;      // cycles at which `refresh` rises
    bool refresh_at(uint64_t cycle) const { return cycle >= base && cycle - base < refv.size() && refv[cycle - base]; }
    bool ok() const { return loaded && released && done.has_value(); }
};

std::string g_dir;

Run execute(const Spec& s) {
    Run r;
    r.spec = s;
    nmh::Config cfg;
    if (s.ntsc) cfg.chipset_config |= 0x02;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(g_dir + "/" + s.bin) > 0;
    if (!r.loaded) return r;
    place_image(h, s);
    r.released = h.boot(kBootBudget).cpu_released;
    // step one clk_sys cycle at a time so that the `refresh` output is sampled with every access
    r.base = h.cycles();
    size_t seen = h.ram_log().size();
    while (h.cycles() - r.base < kRunBudget && !r.done) {
        h.run_cycles(1);
        r.refv.push_back(h.ports()->refresh ? 1 : 0);
        const auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen)
            if (lg[seen].is_write && lg[seen].word_addr * 2 == kDoneAddr && lg[seen].data == kDoneValue) r.done = lg[seen].cycle;
    }
    for (size_t i = 0; i < r.refv.size(); ++i)
        if (r.refv[i] && (i == 0 || !r.refv[i - 1])) r.redges.push_back(r.base + i);
    r.raw = h.ram_log();
    auto pk = [&](uint32_t a) { auto v = h.peek_logical(a); return v ? *v : uint16_t(0); };
    r.r_vec_hi = pk(kRVec); r.r_vec_lo = pk(kRVec + 2);
    r.r_dma = pk(kRDma); r.r_fix = pk(kRFix); r.r_mark = pk(kRMark);
    return r;
}

// refresh: (cycle, word address) pairs of NOFIX's field-F accesses outside K
using RSet = std::vector<std::pair<uint64_t, uint32_t>>;   // sorted

void find_field(Run& r) {
    r.hasF = false;
    const uint32_t w0 = kL1 / 2, w1 = (kL1 + kItem8 + 2) / 2;
    bool have0 = false;
    for (const auto& a : r.raw) {
        if (a.is_write) continue;
        if (!have0 && a.word_addr == w0) { r.fs = a.cycle; have0 = true; continue; }
        if (have0 && a.word_addr == w1) { r.fe = a.cycle; r.hasF = true; break; }
    }
}

void analyze(Run& r, const RSet* R) {
    find_field(r);
    if (r.keys.empty()) {
        for (const auto& a : r.raw) r.keys.push_back({a.cycle, a.word_addr});
        std::sort(r.keys.begin(), r.keys.end());
    }
    r.acc.clear(); r.ch.clear();
    // an access held over several bus periods (identical entries 8 clk_sys apart) is one access
    std::vector<bool> held = nmh::held_continuations(r.raw, 8);
    for (size_t ri = 0; ri < r.raw.size(); ++ri) {
        const auto& a = r.raw[ri];
        if (held[ri]) continue;
        if (R && std::binary_search(R->begin(), R->end(), std::make_pair(a.cycle, a.word_addr))) continue;
        r.acc.push_back(a);
        r.ch.push_back(uint8_t(chan_of(a.word_addr, r.spec.hires())));
    }
}

RSet make_R(const Run& nofix) {
    RSet R;
    if (!nofix.hasF) return R;
    for (const auto& a : nofix.raw) {
        if (a.cycle < nofix.fs || a.cycle > nofix.fe) continue;
        if (!nofix.refresh_at(a.cycle)) continue;
        R.push_back({a.cycle, a.word_addr});
    }
    std::sort(R.begin(), R.end());
    R.erase(std::unique(R.begin(), R.end()), R.end());
    return R;
}

// the refresh groups of a run: clusters of rising edges of `refresh`
std::vector<std::vector<uint64_t>> refresh_groups(const Run& r) {
    std::vector<std::vector<uint64_t>> g;
    for (uint64_t e : r.redges) {
        if (g.empty() || e - g.back().back() > 64) g.emplace_back();
        g.back().push_back(e);
    }
    return g;
}

// Figure 6-9 anchor on NOFIX's field F: four refresh cycles per line, on every other memory cycle
// (16 clk_sys apart), the lines 227 memory cycles (1816 clk_sys) apart
struct Anchor { bool ok = false; size_t lines = 0; std::string text; };
Anchor fig69(const Run& r) {
    Anchor a;
    if (!r.hasF) { a.text = "no field F"; return a; }
    auto g = refresh_groups(r);
    std::vector<std::vector<uint64_t>> in;
    for (auto& x : g) if (x.front() >= r.fs && x.back() <= r.fe) in.push_back(x);
    if (in.size() < 3) { a.text = "too few refresh groups"; return a; }
    a.ok = true;
    for (size_t i = 1; i + 1 < in.size(); ++i) {
        if (in[i].size() != 4) { a.ok = false; a.text = "line group " + std::to_string(i) + " has " + std::to_string(in[i].size()) + " refresh cycles"; return a; }
        for (size_t j = 1; j < 4; ++j)
            if (in[i][j] - in[i][j - 1] != 16) { a.ok = false; a.text = "refresh cycles of group " + std::to_string(i) + " are not 16 clk_sys apart"; return a; }
        if (in[i][0] - in[i - 1][0] != 1816) { a.ok = false; a.text = "line period " + std::to_string(in[i][0] - in[i - 1][0]) + " clk_sys, not 1816"; return a; }
    }
    a.lines = in.size() - 2;
    return a;
}

// ---- traces and intervals -------------------------------------------------------
struct Iv { uint64_t lo = 0, hi = 0; bool ok = false; };

using Tup = std::tuple<uint64_t, uint32_t, uint16_t, bool>;

std::vector<Tup> trace(const Run& r, uint32_t mask, Iv w) {
    std::vector<Tup> t;
    for (size_t i = 0; i < r.acc.size(); ++i) {
        if (!(mask & (1u << r.ch[i]))) continue;
        const auto& a = r.acc[i];
        if (a.cycle < w.lo || a.cycle > w.hi) continue;
        t.push_back({a.cycle, a.word_addr, a.data, a.is_write});
    }
    return t;
}

size_t count_in(const Run& r, uint32_t mask, Iv w) {
    size_t n = 0;
    for (size_t i = 0; i < r.acc.size(); ++i)
        if ((mask & (1u << r.ch[i])) && r.acc[i].cycle >= w.lo && r.acc[i].cycle <= w.hi) ++n;
    return n;
}

bool is_item6(const nmh::RamAccess& a) {
    uint32_t b = a.word_addr * 2;
    return !a.is_write && b >= kL1 + kItem6 && b < kL1 + kItem7;
}

Iv stream_span(const Run& r) {
    Iv w;
    if (!r.hasF) return w;
    bool any = false;
    for (const auto& a : r.acc) {
        if (a.cycle < r.fs || a.cycle > r.fe || !is_item6(a)) continue;
        if (!any) { w.lo = a.cycle; any = true; }
        w.hi = a.cycle;
    }
    w.ok = any;
    return w;
}

Iv blit_span(const Run& r) {
    Iv w;
    if (!r.hasF) return w;
    bool any = false;
    for (size_t i = 0; i < r.acc.size(); ++i) {
        if (r.acc[i].cycle < r.fs || !(kMaskBlit & (1u << r.ch[i]))) continue;
        if (!any) { w.lo = r.acc[i].cycle; any = true; }
        w.hi = r.acc[i].cycle;
    }
    w.ok = any;
    return w;
}

Iv field_span(const Run& r) { Iv w; w.lo = r.fs; w.hi = r.fe; w.ok = r.hasF; return w; }

// S_pre: from the fetch of item 4's second word to the first item-6 fetch, in field F
Iv pre_span(const Run& r) {
    Iv w;
    if (!r.hasF) return w;
    bool have = false;
    for (const auto& a : r.acc) {
        if (a.cycle < r.fs || a.cycle > r.fe || a.is_write) continue;
        if (!have && a.word_addr * 2 == kL1 + kItem4 + 2) { w.lo = a.cycle; have = true; continue; }
        if (have && is_item6(a)) { w.hi = a.cycle; w.ok = true; break; }
    }
    return w;
}

// "at least one fetch" of a fixed channel: two fetches at consecutive word addresses
bool has_fetch(const Run& r, int c, Iv w) {
    std::vector<uint32_t> wa;
    for (size_t i = 0; i < r.acc.size(); ++i)
        if (r.ch[i] == c && r.acc[i].cycle >= w.lo && r.acc[i].cycle <= w.hi) wa.push_back(r.acc[i].word_addr);
    std::sort(wa.begin(), wa.end());
    for (size_t i = 1; i < wa.size(); ++i)
        if (wa[i] == wa[i - 1] + 1) return true;
    return false;
}

// ---- display lines ----------------------------------------------------------------
struct Lines {
    bool ok = false;               // frame check passed for lines 0..kmax
    int kmax = -1;
    std::string why;
    std::vector<uint64_t> lo, hi;  // I_k
};

Lines display_lines(const Run& r) {
    Lines L;
    const Spec& s = r.spec;
    if (!r.hasF) { L.why = "no field F"; return L; }
    const uint32_t w = line_bytes(s);
    const uint32_t words = w / 2;
    std::map<int, std::vector<std::tuple<uint64_t, int, uint32_t>>> byline;   // k -> (cycle, plane, word index)
    for (size_t i = 0; i < r.acc.size(); ++i) {
        int c = r.ch[i];
        if (!(kMaskPlanes & (1u << c))) continue;
        const auto& a = r.acc[i];
        if (a.cycle < r.fs) continue;
        int p = c - PL1;
        uint32_t off = a.word_addr * 2 - plane_base(s, p);
        uint32_t mod = (p % 2 == 0) ? s.bpl1mod : 0;
        uint32_t k = off / (w + mod), rr = off % (w + mod);
        if (rr >= w) continue;
        byline[int(k)].push_back({a.cycle, p, rr / 2});
    }
    // lines whose successor starts before the end of field F
    std::vector<int> ks;
    for (auto& kv : byline) ks.push_back(kv.first);
    if (ks.empty()) { L.why = "no bitplane fetches"; return L; }
    int top = -1;
    for (int k : ks) {
        auto nx = byline.find(k + 1);
        if (nx == byline.end()) break;
        uint64_t st = ~0ull;
        for (auto& t : nx->second) st = std::min(st, std::get<0>(t));
        if (st <= r.fe) top = k; else break;
    }
    L.kmax = top;
    L.lo.assign(size_t(top + 2), 0); L.hi.assign(size_t(top + 2), 0);
    for (int k = 0; k <= top + 1; ++k) {
        auto it = byline.find(k);
        if (it == byline.end()) { L.why = "line " + std::to_string(k) + " has no fetches"; return L; }
        uint64_t lo = ~0ull, hi = 0;
        std::set<std::pair<int, uint32_t>> seen;
        for (auto& t : it->second) {
            lo = std::min(lo, std::get<0>(t)); hi = std::max(hi, std::get<0>(t));
            seen.insert({std::get<1>(t), std::get<2>(t)});
        }
        L.lo[size_t(k)] = lo; L.hi[size_t(k)] = hi;
        if (k <= top && seen.size() != size_t(4 * words)) {
            L.why = "line " + std::to_string(k) + " has " + std::to_string(seen.size()) + " of " +
                    std::to_string(4 * words) + " words";
            return L;
        }
        if (k > 0 && L.lo[size_t(k)] <= L.hi[size_t(k - 1)]) {
            L.why = "line " + std::to_string(k - 1) + " ends after line " + std::to_string(k) + " starts";
            return L;
        }
    }
    // I_k lies on beam line $2C+k: line n of the field is the n-th group of refresh cycles from the field's line 0
    {
        auto g = refresh_groups(r);
        int g0 = -1;
        for (size_t i = 0; i < g.size(); ++i) if (g[i].front() <= r.fs) g0 = int(i);
        if (g0 < 0) { L.why = "no refresh group at the start of field F"; return L; }
        for (int k = 0; k <= top + 1; ++k) {
            size_t gi = size_t(g0 + 0x2C + k);
            if (gi + 1 >= g.size() || !(g[gi].front() <= L.lo[size_t(k)] && L.lo[size_t(k)] < g[gi + 1].front())) {
                L.why = "display line " + std::to_string(k) + " is not on beam line $2C+k";
                return L;
            }
        }
    }
    L.ok = true;
    return L;
}


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

std::string hx(uint32_t v) { char b[16]; std::snprintf(b, sizeof b, "%X", v); return b; }
std::string num(uint64_t v) { return std::to_string(v); }

// ---- the runs of one process ----------------------------------------------------
struct Ctx {
    std::map<std::string, Spec> specs;
    std::map<std::string, Run> runs;
    RSet R;
    Anchor anchor;
    bool haveR = false;

    void ensureR() {
        if (haveR) return;
        Run r = execute(specs.at("NOFIX"));
        analyze(r, nullptr);
        R = make_R(r);
        anchor = fig69(r);
        analyze(r, &R);
        runs["NOFIX"] = std::move(r);
        haveR = true;
    }
    Run& get(const std::string& n) {
        auto it = runs.find(n);
        if (it != runs.end()) return it->second;
        ensureR();
        it = runs.find(n);
        if (it != runs.end()) return it->second;
        Run r = execute(specs.at(n));
        analyze(r, &R);
        return runs[n] = std::move(r);
    }
};

// ---- common preconditions, per run and per pair -------------------------------------
bool run_ok(const Run& r) { return r.ok() && r.hasF; }

// C2: the DMACON bits read back; BLTPRI reads 1 in every run that blits
bool c2_run(const Run& r) {
    if (!run_ok(r)) return false;
    uint16_t want = (r.spec.dmacon_decl ? r.spec.dmacon_decl : r.spec.dmacon_v()) & 0x7FF;
    bool bits = (r.r_dma & 0x7FF) == want;
    bool nasty = !r.spec.blit || (r.r_dma & 0x400);
    return bits && nasty;
}
// C3: in a STOP run, the 68000 makes no access during field F
size_t cpu_in_field(const Run& r) {
    size_t n = 0;
    for (size_t i = 0; i < r.acc.size(); ++i)
        if (r.ch[i] == OTH && r.acc[i].cycle >= r.fs && r.acc[i].cycle <= r.fe) ++n;
    return n;
}
bool c3_run(const Run& r) { return run_ok(r) && cpu_in_field(r) == 0; }
// C4: the 512 words of item 6 are fetched in field F, ascending, with the data placed there
bool c4_run(const Run& r) {
    if (!run_ok(r)) return false;
    uint32_t next = 0;
    for (const auto& a : r.acc) {
        if (a.cycle < r.fs || a.cycle > r.fe || !is_item6(a)) continue;
        if (next >= 512) return false;
        uint32_t wa = (kL1 + kItem6) / 2 + next;
        if (a.word_addr != wa) return false;
        uint16_t want = (next % 2 == 0) ? uint16_t(0x01FE) : uint16_t(next / 2);
        if (a.data != want) return false;
        ++next;
    }
    return next == 512;
}
// C5: the 68000 reads the CPU variant word before field F starts
bool c5_run(const Run& r) {
    if (!run_ok(r)) return false;
    for (const auto& a : r.raw)
        if (!a.is_write && a.word_addr * 2 == kVT) return a.cycle < r.fs;
    return false;
}
// C6: the vector word read back
bool c6_run(const Run& r) { return r.ok() && r.r_vec_hi == (kHandler >> 16) && r.r_vec_lo == (kHandler & 0xFFFF); }
// C7: every refresh access of R appears in the run
bool c7_run(const Run& r, const RSet& R) {
    if (!r.ok() || R.empty()) return false;
    for (const auto& e : R)
        if (!std::binary_search(r.keys.begin(), r.keys.end(), e)) return false;
    return true;
}

// the words whose read may differ between two runs by construction
std::vector<uint32_t> diff_reads(const Spec& a, const Spec& b) {
    std::vector<uint32_t> v;
    if (a.loop != b.loop) v.push_back(kVT + 0);
    if (a.dmacon_v() != b.dmacon_v()) v.push_back(kVT + 2);
    if (a.bplcon0_v() != b.bplcon0_v()) v.push_back(kVT + 4);
    if (a.ddfstrt_v() != b.ddfstrt_v()) v.push_back(kVT + 6);
    if (a.ddfstop_v() != b.ddfstop_v()) v.push_back(kVT + 8);
    if (a.bpl1mod != b.bpl1mod) v.push_back(kVT + 10);
    if (a.aud0per != b.aud0per) v.push_back(kVT + 14);
    if (a.blit != b.blit) v.push_back(kL1 + kItem4);
    if (a.blt_data != b.blt_data) v.push_back(kL1 + kItem4 + 2);
    if (a.item5 != b.item5) v.push_back(kL1 + kItem5);
    if (a.last_wait != b.last_wait) v.push_back(kL1 + kItem6 + 4 * 255);
    return v;
}

// C1: identical port traces up to the first access at which the runs differ by construction
bool c1_pair(const Run& a, const Run& b, std::string& why) {
    if (!a.ok() || !b.ok()) { why = "a run did not finish"; return false; }
    std::vector<uint32_t> allowed = diff_reads(a.spec, b.spec);
    size_t n = std::min(a.raw.size(), b.raw.size());
    for (size_t j = 0; j < n; ++j) {
        const auto& x = a.raw[j];
        const auto& y = b.raw[j];
        if (x.cycle == y.cycle && x.word_addr == y.word_addr && x.data == y.data && x.is_write == y.is_write) continue;
        bool same_access = x.cycle == y.cycle && x.word_addr == y.word_addr && !x.is_write && !y.is_write;
        bool listed = std::find(allowed.begin(), allowed.end(), x.word_addr * 2) != allowed.end();
        if (same_access && listed) return true;
        why = "first difference at entry " + num(j) + ": cycle " + num(x.cycle) + "/" + num(y.cycle) +
              ", address $" + hx(x.word_addr * 2) + "/$" + hx(y.word_addr * 2);
        return false;
    }
    if (a.raw.size() == b.raw.size()) return true;   // the runs do not differ at all
    why = "one trace is a prefix of the other";
    return false;
}

struct CmpRes { bool eq = false; size_t na = 0, nb = 0; std::string first; };

CmpRes cmp_traces(const std::vector<Tup>& a, const std::vector<Tup>& b, const std::vector<uint32_t>& ign = {}) {
    CmpRes r;
    r.na = a.size(); r.nb = b.size();
    size_t n = std::min(a.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        bool same = std::get<0>(a[i]) == std::get<0>(b[i]) && std::get<1>(a[i]) == std::get<1>(b[i]) &&
                    std::get<3>(a[i]) == std::get<3>(b[i]) &&
                    (std::get<2>(a[i]) == std::get<2>(b[i]) || std::find(ign.begin(), ign.end(), std::get<1>(a[i]) * 2) != ign.end());
        if (!same) {
            r.first = "element " + num(i) + ": cycle " + num(std::get<0>(a[i])) + "/" + num(std::get<0>(b[i])) +
                      ", address $" + hx(std::get<1>(a[i]) * 2) + "/$" + hx(std::get<1>(b[i]) * 2) +
                      ", data $" + hx(std::get<2>(a[i])) + "/$" + hx(std::get<2>(b[i]));
            return r;
        }
    }
    if (a.size() != b.size()) { r.first = "element counts differ"; return r; }
    r.eq = true;
    return r;
}

bool is_loop_access(const Run& r, size_t i) {
    if (r.ch[i] != OTH) return false;
    uint32_t b = r.acc[i].word_addr * 2;
    return (b >= kCodeLo && b < kCodeHi) || b == kA2;
}
size_t loop_count(const Run& r, Iv w) {
    size_t n = 0;
    for (size_t i = 0; i < r.acc.size(); ++i)
        if (is_loop_access(r, i) && r.acc[i].cycle >= w.lo && r.acc[i].cycle <= w.hi) ++n;
    return n;
}
// demand precondition: in the unopposed run an access of the kind `un` lies in the memory cycle (8 clk_sys) of an
// access of the kind `ref` of the reference run, inside the interval
template <class UN, class RF>
size_t demand_count(const Run& unopp, UN un, const Run& refr, RF rf, Iv w) {
    std::set<uint64_t> times;
    for (size_t i = 0; i < refr.acc.size(); ++i)
        if (refr.acc[i].cycle >= w.lo && refr.acc[i].cycle <= w.hi && rf(refr, i)) times.insert(refr.acc[i].cycle / 8);
    size_t n = 0;
    for (size_t i = 0; i < unopp.acc.size(); ++i)
        if (times.count(unopp.acc[i].cycle / 8) && un(unopp, i)) ++n;
    return n;
}
bool any_loop(const Run& r, uint64_t lo, uint64_t hi_excl) {
    for (size_t i = 0; i < r.acc.size(); ++i)
        if (is_loop_access(r, i) && r.acc[i].cycle >= lo && r.acc[i].cycle < hi_excl) return true;
    return false;
}

// P2-a: the 68000 is busy
bool p2a_run(const Run& c, std::string& why) {
    if (!run_ok(c)) { why = "run"; return false; }
    Lines L = display_lines(c);
    if (!L.ok) { why = "display-line frame: " + L.why; return false; }
    Iv ws = stream_span(c);
    if (!ws.ok) { why = "no stream"; return false; }
    if (!any_loop(c, c.fs, L.lo[0])) { why = "no loop access before the first bitplane fetch"; return false; }
    int before = -1, after = -1;
    for (int k = 0; k <= L.kmax; ++k) {
        uint64_t lo = L.lo[size_t(k)], hi = L.lo[size_t(k + 1)];
        bool overlap = lo <= ws.hi && hi > ws.lo;
        if (hi <= ws.lo) before = k;
        if (lo >= ws.hi && after < 0) after = k;
        if (!overlap && !any_loop(c, lo, hi)) { why = "display line " + num(uint64_t(k)) + " has no loop access"; return false; }
    }
    if (before < 0 || after < 0) { why = "no display line before or after the stream"; return false; }
    bool marker = false;
    for (const auto& a : c.raw)
        if (a.is_write && a.word_addr * 2 == kRMark && a.data == 1 && a.cycle > c.fe) { marker = true; break; }
    if (!marker) { why = "no loop-exit marker after field F"; return false; }
    return true;
}

bool fixed_all(const Run& r, Iv w, std::string& missing) {
    bool ok = true;
    for (int c = kFirstFixed; c <= kLastFixed; ++c)
        if (!has_fetch(r, c, w)) { ok = false; missing += std::string(missing.empty() ? "" : ",") + ch_name(c); }
    return ok;
}

void not_graded(Checks& chk, const std::string& id, const std::string& unmet) {
    chk.record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
}

// aggregate of the common preconditions over the comparisons evaluated in this process
struct CAgg {
    bool ok[8];
    int n[8];
    std::string bad[8];
    CAgg() { for (int i = 0; i < 8; ++i) { ok[i] = true; n[i] = 0; } }
    void add(int c, bool v, const std::string& what) {
        ++n[c];
        if (!v) { ok[c] = false; bad[c] += (bad[c].empty() ? "" : "; ") + what; }
    }
};
CAgg g_c;

const char* kClaimsP1 = "sol:COP2-copins-arbitration.c12,opus:ch2-appa-copins.c131,opus:ch2-about-the-copper.c28,opus:ch2-about-the-copper.c17.s2";
const char* kClaimsP2 = "sol:COP2-copins-arbitration.c14,opus:ch2-appa-copins.c132,opus:ch2-about-the-copper.c27,opus:ch2-about-the-copper.c17.s2";
const char* kClaimsP3 = "opus:ch2-about-the-copper.c29,opus:ch2-appa-copins.c133,sol:COP2-copins-arbitration.c12,sol:COP2-copins-arbitration.c14,opus:ch2-appa-copins.c131,opus:ch2-appa-copins.c132";
const char* kClaimsP4 = "sol:6-blitter-priority.c4,sol:6-blitter-priority.c5,opus:ch6-sysdma.c55,sol:6-system-dma.c5,sol:6-system-dma.c6,sol:6-system-dma.c8,opus:ch6-sysdma.c2,opus:ch6-sysdma.c3,opus:ch6-sysdma.c5";
const char* kClaimsP5a = "sol:6-system-dma.c5,sol:6-system-dma.c6,sol:6-system-dma.c8,sol:6-system-dma.c11,opus:ch6-sysdma.c2,opus:ch6-sysdma.c3,opus:ch6-sysdma.c5,opus:ch6-sysdma.c9";
const char* kClaimsP5b = "sol:6-system-dma.c5,sol:6-system-dma.c6,sol:6-system-dma.c8,opus:ch6-sysdma.c2,opus:ch6-sysdma.c3,opus:ch6-sysdma.c5";
const char* kClaimsP6a = "sol:6-system-dma.c11,opus:ch6-sysdma.c9,opus:ch6-sysdma.c55";


// ====================================================================================
// P1 -- the Copper over the blitter
// ====================================================================================
void gP1(Ctx& cx, Checks& chk) {
    Run& ref = cx.get("REF");
    Run& blt = cx.get("BLT");
    Run& nc = cx.get("BLT-NOCOP");
    std::string why;
    bool c1 = c1_pair(blt, ref, why); g_c.add(1, c1, "P1 " + why);
    bool c2 = c2_run(blt) && c2_run(ref); g_c.add(2, c2, "P1");
    bool c3 = c3_run(blt) && c3_run(ref); g_c.add(3, c3, "P1");
    bool c4 = c4_run(blt) && c4_run(ref); g_c.add(4, c4, "P1");
    bool c6 = c6_run(blt) && c6_run(ref); g_c.add(6, c6, "P1");
    bool c7 = c7_run(blt, cx.R) && c7_run(ref, cx.R); g_c.add(7, c7, "P1");
    Iv wb = blit_span(blt), ws = stream_span(blt), wr = stream_span(ref);
    bool p1a = wb.ok && ws.ok && wb.lo < ws.lo && wb.hi > ws.hi;
    size_t n_dem = 0;
    if (wr.ok) n_dem = demand_count(nc, [](const Run& q, size_t i) { return (kMaskBlit & (1u << q.ch[i])) != 0; },
                                    ref, [](const Run& q, size_t i) { return is_item6(q.acc[i]); }, wr);
    bool p1b = wr.ok && n_dem > 0;
    std::string a_dem = num(n_dem);
    std::string sa = num(wb.lo), sb = num(wb.hi), sc = num(ws.lo), sd = num(ws.hi);
    chk.expect(p1a,
        "P1-a [p19,p20,p198]: the blit covers the stream -- the first access to BA or BD (cycle " + sa +
        ") precedes the first item-6 fetch (cycle " + sc + ") and the last (cycle " + sb + ") follows the last item-6 fetch (cycle " + sd +
        ") [not graded -- precondition, no claim uid]");
    chk.expect(p1b,
        "P1-b [p19,p198]: the blitter demanded the Copper's slots -- in BLT-NOCOP (the Copper waiting) " + a_dem +
        " accesses to BA and BD lie at the times of REF's item-6 fetches [not graded -- precondition, no claim uid]");
    bool gate = c1 && c2 && c3 && c4 && c6 && c7 && p1a && p1b;
    auto ta = trace(blt, kMaskK, wb), tb = trace(ref, kMaskK, wb);
    CmpRes r = cmp_traces(ta, tb, {kL1 + kItem4, kL1 + kItem5});
    std::string na = num(r.na), nb = num(r.nb);
    if (gate) {
        chk.expect(r.eq,
            std::string("P1 [p275,p20,p19] ") + kClaimsP1 + ": while a blit with the blitter-nasty bit set runs, every one of the " + na +
            " Copper fetches in the blit's span happens at the same time, from the same address, with the same data as in the run without the blit (" +
            nb + " fetches there)");
        if (!r.eq) chk.note("P1 first mismatch: " + r.first);
    } else {
        not_graded(chk, "P1", std::string("C1=") + (c1 ? "1" : "0") + " C2=" + (c2 ? "1" : "0") + " C3=" + (c3 ? "1" : "0") + " C4=" + (c4 ? "1" : "0") +
                   " C6=" + (c6 ? "1" : "0") + " C7=" + (c7 ? "1" : "0") + " P1-a=" + (p1a ? "1" : "0") + " P1-b=" + (p1b ? "1" : "0"));
        chk.note(std::string("P1 comparator, not graded: ") + (r.eq ? "equal" : "different") + " (" + na + " against " + nb + " fetches) " + r.first);
    }
}

// ====================================================================================
// P2 -- the Copper over the 68000
// ====================================================================================
void gP2(Ctx& cx, Checks& chk) {
    Run& ref = cx.get("REF");
    Run& cpu = cx.get("CPU");
    Run& nc = cx.get("CPU-NOCOP");
    std::string why, why2;
    bool c1 = c1_pair(cpu, ref, why); g_c.add(1, c1, "P2 " + why);
    bool c2 = c2_run(cpu) && c2_run(ref); g_c.add(2, c2, "P2");
    bool c3 = c3_run(ref); g_c.add(3, c3, "P2");
    bool c4 = c4_run(cpu) && c4_run(ref); g_c.add(4, c4, "P2");
    bool c5 = c5_run(cpu); g_c.add(5, c5, "P2");
    bool c6 = c6_run(ref) && c6_run(cpu); g_c.add(6, c6, "P2");
    bool c7 = c7_run(cpu, cx.R) && c7_run(ref, cx.R); g_c.add(7, c7, "P2");
    bool p2a = p2a_run(cpu, why2);
    Iv wr = stream_span(ref);
    size_t n_dem = 0;
    if (wr.ok) n_dem = demand_count(nc, [](const Run& q, size_t i) { return is_loop_access(q, i); },
                                    ref, [](const Run& q, size_t i) { return is_item6(q.acc[i]); }, wr);
    bool p2b = wr.ok && n_dem > 0;
    std::string a_dem = num(n_dem);
    chk.expect(p2a,
        "P2-a [p19,p20,p196]: the 68000 is busy -- loop accesses before the first bitplane fetch, in every display line outside the stream, in the lines just before and after it, and the loop-exit marker follows field F" +
        std::string(p2a ? "" : " (unmet: " + why2 + ")") + " [not graded -- precondition, no claim uid]");
    chk.expect(p2b,
        "P2-b [p19,p20]: the 68000 demanded the Copper's slots -- in CPU-NOCOP (the Copper waiting) " + a_dem +
        " loop accesses lie at the times of REF's item-6 fetches [not graded -- precondition, no claim uid]");
    bool gate = c1 && c2 && c3 && c4 && c5 && c6 && c7 && p2a && p2b;
    Iv wf = field_span(cpu);
    auto ta = trace(cpu, kMaskK, wf), tb = trace(ref, kMaskK, wf);
    CmpRes r = cmp_traces(ta, tb, {kL1 + kItem5});
    std::string na = num(r.na), nb = num(r.nb);
    if (gate) {
        chk.expect(r.eq,
            std::string("P2 [p275,p20,p19] ") + kClaimsP2 + ": while the 68000 runs a loop of Chip-memory accesses, every one of the " + na +
            " Copper fetches of field F happens at the same time, from the same address, with the same data as when the 68000 is stopped (" + nb + " fetches there)");
        if (!r.eq) chk.note("P2 first mismatch: " + r.first);
    } else {
        not_graded(chk, "P2", std::string("C1=") + (c1 ? "1" : "0") + " C2=" + (c2 ? "1" : "0") + " C3=" + (c3 ? "1" : "0") + " C4=" + (c4 ? "1" : "0") +
                   " C5=" + (c5 ? "1" : "0") + " C6=" + (c6 ? "1" : "0") + " C7=" + (c7 ? "1" : "0") + " P2-a=" + (p2a ? "1" : "0") + " P2-b=" + (p2b ? "1" : "0"));
        chk.note(std::string("P2 comparator, not graded: ") + (r.eq ? "equal" : "different") + " (" + na + " against " + nb + " fetches) " + r.first);
    }
}

// ====================================================================================
// P3 -- no collision with disk, audio, sprites, a four-plane low-resolution display
// ====================================================================================
void gP3(Ctx& cx, Checks& chk) {
    Run& ref = cx.get("REF");
    Run& nof = cx.get("NOFIX");
    std::string why;
    bool c1 = c1_pair(ref, nof, why); g_c.add(1, c1, "P3 " + why);
    bool c2 = c2_run(ref) && c2_run(nof); g_c.add(2, c2, "P3");
    bool c3 = c3_run(ref) && c3_run(nof); g_c.add(3, c3, "P3");
    bool c4 = c4_run(ref) && c4_run(nof); g_c.add(4, c4, "P3");
    bool c6 = c6_run(ref) && c6_run(nof); g_c.add(6, c6, "P3");
    bool c7 = c7_run(ref, cx.R) && c7_run(nof, cx.R); g_c.add(7, c7, "P3");
    // P3-a
    Iv ws = stream_span(ref);
    std::string missing, why3;
    bool p3a = ws.ok && fixed_all(ref, ws, missing);
    if (!p3a) why3 = "channels without fetches: " + missing;
    if (p3a) {
        Lines L = display_lines(ref);
        if (!L.ok) { p3a = false; why3 = "display-line frame: " + L.why; }
        else {
            std::vector<uint64_t> st, pl;
            for (size_t i = 0; i < ref.acc.size(); ++i) {
                if (ref.acc[i].cycle < ref.fs || ref.acc[i].cycle > ref.fe) continue;
                if (is_item6(ref.acc[i])) st.push_back(ref.acc[i].cycle);
                if (kMaskPlanes & (1u << ref.ch[i])) pl.push_back(ref.acc[i].cycle);
            }
            int lines = 0;
            for (int k = 0; k <= L.kmax && p3a; ++k) {
                uint64_t lo = L.lo[size_t(k)], hi = L.hi[size_t(k)];
                if (!(lo <= ws.hi && hi >= ws.lo)) continue;
                ++lines;
                bool found = false;
                for (size_t i = 0; i + 1 < st.size() && !found; ++i) {
                    if (st[i] < lo || st[i + 1] > hi) continue;
                    auto it = std::upper_bound(pl.begin(), pl.end(), st[i]);
                    if (it != pl.end() && *it < st[i + 1]) found = true;
                }
                if (!found) { p3a = false; why3 = "display line " + num(uint64_t(k)) + " has no bitplane fetch between two stream fetches"; }
            }
            if (lines == 0) { p3a = false; why3 = "no display line overlaps the stream"; }
        }
    }
    // P3-b: the reference is quiet (raw classification of NOFIX's field F)
    size_t quiet = 0;
    for (const auto& a : nof.raw) {
        if (a.cycle < nof.fs || a.cycle > nof.fe) continue;
        int c = chan_of(a.word_addr, false);
        if (c >= PL1 && c <= DK) ++quiet;
    }
    bool p3b = run_ok(nof) && quiet == 0;
    std::string qn = num(quiet);
    chk.expect(p3a,
        std::string("P3-a [p20,p196,p275]: the channels ran while the Copper fetched -- over the stream's interval in REF each of the 17 fixed channels has fetches at consecutive addresses and every display line that the stream overlaps has a bitplane fetch between two stream fetches") +
        (p3a ? "" : " (unmet: " + why3 + ")") + " [not graded -- precondition, no claim uid]");
    chk.expect(p3b,
        "P3-b [p20,p275]: the reference is quiet -- NOFIX has " + qn + " accesses to bitplane, sprite, audio or disk data in field F [not graded -- precondition, no claim uid]");
    bool gate = c1 && c2 && c3 && c4 && c6 && c7 && p3a && p3b;
    Iv wf = field_span(ref);
    auto ta = trace(ref, kMaskK, wf), tb = trace(nof, kMaskK, wf);
    CmpRes r = cmp_traces(ta, tb, {kL1 + kItem5});
    std::string na = num(r.na), nb = num(r.nb);
    if (gate) {
        chk.expect(r.eq,
            std::string("P3 [p20,p275,p196] ") + kClaimsP3 + ": disk, audio and sprite DMA and a four-plane low-resolution display never delay the Copper -- all " + na +
            " Copper fetches of field F happen at the same time, from the same address, with the same data as without them (" + nb + " fetches there)");
        if (!r.eq) chk.note("P3 first mismatch: " + r.first);
    } else {
        not_graded(chk, "P3", std::string("C1=") + (c1 ? "1" : "0") + " C2=" + (c2 ? "1" : "0") + " C3=" + (c3 ? "1" : "0") + " C4=" + (c4 ? "1" : "0") +
                   " C6=" + (c6 ? "1" : "0") + " C7=" + (c7 ? "1" : "0") + " P3-a=" + (p3a ? "1" : "0") + " P3-b=" + (p3b ? "1" : "0"));
        chk.note(std::string("P3 comparator, not graded: ") + (r.eq ? "equal" : "different") + " (" + na + " against " + nb + " fetches) " + r.first);
    }
}

// ====================================================================================
// P4 -- the fixed channels during a blitter-nasty blit
// ====================================================================================
void gP4(Ctx& cx, Checks& chk) {
    Run& ref = cx.get("REF");
    Run& blt = cx.get("BLT");
    Run& nf = cx.get("BLT-NOFIX");
    std::string why;
    bool c1 = c1_pair(blt, ref, why); g_c.add(1, c1, "P4 " + why);
    bool c2 = c2_run(blt) && c2_run(ref); g_c.add(2, c2, "P4");
    bool c3 = c3_run(blt) && c3_run(ref); g_c.add(3, c3, "P4");
    bool c4 = c4_run(blt) && c4_run(ref); g_c.add(4, c4, "P4");
    bool c6 = c6_run(blt) && c6_run(ref); g_c.add(6, c6, "P4");
    bool c7 = c7_run(blt, cx.R) && c7_run(ref, cx.R); g_c.add(7, c7, "P4");
    Iv wb = blit_span(blt);
    // P4-a
    std::string missing, why4;
    bool p4a = wb.ok && fixed_all(ref, wb, missing);
    if (!p4a) why4 = wb.ok ? "channels without fetches: " + missing : "no blit span";
    int lines_in = 0;
    if (p4a) {
        Lines L = display_lines(ref);
        if (!L.ok) { p4a = false; why4 = "display-line frame: " + L.why; }
        else {
            for (int k = 0; k <= L.kmax && p4a; ++k) {
                uint64_t lo = L.lo[size_t(k)], hi = L.lo[size_t(k + 1)];
                if (lo < wb.lo || hi > wb.hi) continue;
                ++lines_in;
                Iv w; w.lo = lo; w.hi = hi - 1; w.ok = true;
                for (int c = PL1; c <= PL4 && p4a; ++c)
                    if (count_in(ref, 1u << c, w) == 0) { p4a = false; why4 = std::string(ch_name(c)) + " has no fetch in display line " + num(uint64_t(k)); }
                for (int c = SP0; c <= SP7 && p4a; ++c)
                    if (count_in(ref, 1u << c, w) == 0) { p4a = false; why4 = std::string(ch_name(c)) + " has no fetch in display line " + num(uint64_t(k)); }
            }
            if (lines_in == 0) { p4a = false; why4 = "no display line inside the blit"; }
        }
    }
    // P4-b
    Iv sp = pre_span(blt);
    size_t n_dem = 0;
    if (sp.ok) n_dem = demand_count(nf, [](const Run& q, size_t i) { return (kMaskBlit & (1u << q.ch[i])) != 0; },
                                    ref, [](const Run& q, size_t i) { return (kMaskFixed & (1u << q.ch[i])) != 0; }, sp);
    bool p4b = sp.ok && n_dem > 0;
    std::string a_dem = num(n_dem), li = num(uint64_t(lines_in));
    chk.expect(p4a,
        "P4-a [p194,p198,p80,p82]: the channels ran during the blit -- in REF over the blit's span each of the 17 fixed channels has fetches at consecutive addresses and in each of the " + li +
        " display lines inside it every plane and every sprite (sprite 7 among them) has fetches" + (p4a ? "" : " (unmet: " + why4 + ")") +
        " [not graded -- precondition, no claim uid]");
    chk.expect(p4b,
        "P4-b [p198]: the blitter demanded their slots -- over lines $40 to $4F, in BLT-NOFIX " + a_dem +
        " accesses to BA and BD lie at the times of REF's fixed-channel fetches [not graded -- precondition, no claim uid]");
    bool gate = c1 && c2 && c3 && c4 && c6 && c7 && p4a && p4b;
    bool all = true;
    std::string bad;
    size_t total = 0;
    for (int c = kFirstFixed; c <= kLastFixed; ++c) {
        auto ta = trace(blt, 1u << c, wb), tb = trace(ref, 1u << c, wb);
        CmpRes r = cmp_traces(ta, tb);
        total += r.na;
        if (!r.eq) { all = false; bad += std::string(bad.empty() ? "" : "; ") + ch_name(c) + " (" + num(r.na) + " against " + num(r.nb) + "): " + r.first; }
    }
    std::string tn = num(total);
    if (gate) {
        chk.expect(all,
            std::string("P4 [p198,p194] ") + kClaimsP4 + ": while a blit with the blitter-nasty bit set runs, disk, audio, sprite and bitplane DMA keep every one of their slots -- all " + tn +
            " fetches of the 17 channels in the blit's span happen at the same time, from the same address, with the same data as without the blit");
        if (!all) chk.note("P4 mismatches: " + bad);
    } else {
        not_graded(chk, "P4", std::string("C1=") + (c1 ? "1" : "0") + " C2=" + (c2 ? "1" : "0") + " C3=" + (c3 ? "1" : "0") + " C4=" + (c4 ? "1" : "0") +
                   " C6=" + (c6 ? "1" : "0") + " C7=" + (c7 ? "1" : "0") + " P4-a=" + (p4a ? "1" : "0") + " P4-b=" + (p4b ? "1" : "0"));
        chk.note(std::string("P4 comparator, not graded: ") + (all ? "all channels equal" : "different: " + bad));
    }
}

// ====================================================================================
// P5 -- the fixed channels against the Copper and the 68000
// ====================================================================================
void gP5(Ctx& cx, Checks& chk) {
    Run& ref = cx.get("REF");
    Run& noc = cx.get("NOCOP");
    Run& cpu = cx.get("CPU");
    // (a)
    std::string why;
    bool ac1 = c1_pair(ref, noc, why); g_c.add(1, ac1, "P5a " + why);
    bool ac2 = c2_run(ref) && c2_run(noc); g_c.add(2, ac2, "P5a");
    bool ac3 = c3_run(ref) && c3_run(noc); g_c.add(3, ac3, "P5a");
    bool ac6 = c6_run(ref) && c6_run(noc); g_c.add(6, ac6, "P5a");
    bool ac7 = c7_run(ref, cx.R) && c7_run(noc, cx.R); g_c.add(7, ac7, "P5a");
    bool ac4 = c4_run(ref); g_c.add(4, ac4, "P5a");
    // (b)
    std::string why2, why3;
    bool bc1 = c1_pair(cpu, ref, why2); g_c.add(1, bc1, "P5b " + why2);
    bool bc2 = c2_run(cpu) && c2_run(ref); g_c.add(2, bc2, "P5b");
    bool bc7 = c7_run(cpu, cx.R) && c7_run(ref, cx.R); g_c.add(7, bc7, "P5b");
    bool bc3 = c3_run(ref); g_c.add(3, bc3, "P5b");
    bool bc6 = c6_run(ref); g_c.add(6, bc6, "P5b");
    bool bc5 = c5_run(cpu); g_c.add(5, bc5, "P5b");
    bool p2a = p2a_run(cpu, why3);
    Iv ws = stream_span(ref);
    Iv wf = field_span(cpu);
    std::string miss_a, miss_b;
    bool p5a_a = ws.ok && fixed_all(ref, ws, miss_a);
    bool p5a_b = wf.ok && fixed_all(cpu, wf, miss_b);
    chk.expect(p5a_a && p5a_b,
        std::string("P5-a [p194]: the channels ran -- each of the 17 fixed channels has fetches at consecutive addresses in REF over the Copper's stream interval") +
        (p5a_a ? "" : " (unmet: " + miss_a + ")") + " and in CPU over field F" + (p5a_b ? "" : " (unmet: " + miss_b + ")") +
        " [not graded -- precondition, no claim uid]");
    bool all_a = true; std::string bad_a; size_t tot_a = 0;
    bool all_b = true; std::string bad_b; size_t tot_b = 0; std::string planes_note;
    for (int c = kFirstFixed; c <= kLastFixed; ++c) {
        CmpRes ra = cmp_traces(trace(ref, 1u << c, ws), trace(noc, 1u << c, ws));
        tot_a += ra.na;
        if (!ra.eq) { all_a = false; bad_a += std::string(bad_a.empty() ? "" : "; ") + ch_name(c) + " (" + num(ra.na) + " against " + num(ra.nb) + "): " + ra.first; }
        CmpRes rb = cmp_traces(trace(cpu, 1u << c, wf), trace(ref, 1u << c, wf));
        if (kMaskPlanes & (1u << c)) {
            planes_note += std::string(planes_note.empty() ? "" : ", ") + ch_name(c) + " " + num(rb.na) + " against " + num(rb.nb) + (rb.eq ? " equal" : " different");
            continue;
        }
        tot_b += rb.na;
        if (!rb.eq) { all_b = false; bad_b += std::string(bad_b.empty() ? "" : "; ") + ch_name(c) + " (" + num(rb.na) + " against " + num(rb.nb) + "): " + rb.first; }
    }
    std::string ta = num(tot_a), tb = num(tot_b);
    chk.note("P5b bitplanes, measured and not graded: " + planes_note);
    bool gate_a = ac1 && ac2 && ac3 && ac4 && ac6 && ac7 && p5a_a;
    bool gate_b = bc1 && bc2 && bc3 && bc5 && bc6 && bc7 && p2a && p5a_b;
    if (gate_a) {
        chk.expect(all_a,
            std::string("P5a [p194] ") + kClaimsP5a + ": while the Copper runs a stream of MOVEs, disk, audio, sprite and bitplane DMA keep every one of their slots -- all " + ta +
            " fetches of the 17 channels in the stream's interval happen at the same time, from the same address, with the same data as with the Copper waiting");
        if (!all_a) chk.note("P5a mismatches: " + bad_a);
    } else {
        not_graded(chk, "P5a", std::string("C1=") + (ac1 ? "1" : "0") + " C2=" + (ac2 ? "1" : "0") + " C3=" + (ac3 ? "1" : "0") + " C4=" + (ac4 ? "1" : "0") +
                   " C6=" + (ac6 ? "1" : "0") + " C7=" + (ac7 ? "1" : "0") + " P5-a(a)=" + (p5a_a ? "1" : "0"));
        chk.note(std::string("P5a comparator, not graded: ") + (all_a ? "all channels equal" : "different: " + bad_a));
    }
    if (gate_b) {
        chk.expect(all_b,
            std::string("P5b [p194] ") + kClaimsP5b + ": while the 68000 runs a loop of Chip-memory accesses, disk, audio, sprite and bitplane DMA keep every one of their slots -- all " + tb +
            " fetches of disk, audio and sprite DMA in field F happen at the same time, from the same address, with the same data as with the 68000 stopped");
        if (!all_b) chk.note("P5b mismatches: " + bad_b);
    } else {
        not_graded(chk, "P5b", std::string("C1=") + (bc1 ? "1" : "0") + " C2=" + (bc2 ? "1" : "0") + " C3=" + (bc3 ? "1" : "0") + " C5=" + (bc5 ? "1" : "0") +
                   " C6=" + (bc6 ? "1" : "0") + " C7=" + (bc7 ? "1" : "0") + " P2-a=" + (p2a ? "1" : "0") + " P5-a(b)=" + (p5a_b ? "1" : "0"));
        chk.note(std::string("P5b comparator, not graded: ") + (all_b ? "all channels equal" : "different: " + bad_b));
    }
}

// ====================================================================================
// P6 -- four high-resolution planes against the Copper and the blitter
// ====================================================================================
void gP6(Ctx& cx, Checks& chk) {
    Run& h0 = cx.get("HIRES-REF");
    Run& h1 = cx.get("HIRES-LOAD");
    Run& nf = cx.get("BLT-NOFIX");
    std::string why;
    bool c1 = c1_pair(h1, h0, why); g_c.add(1, c1, "P6 " + why);
    bool c2 = c2_run(h1) && c2_run(h0); g_c.add(2, c2, "P6");
    bool c3 = c3_run(h1) && c3_run(h0); g_c.add(3, c3, "P6");
    bool c4 = c4_run(h1); g_c.add(4, c4, "P6");
    bool c6 = c6_run(h1) && c6_run(h0); g_c.add(6, c6, "P6");
    bool c7 = c7_run(h1, cx.R) && c7_run(h0, cx.R); g_c.add(7, c7, "P6");
    Iv ss = stream_span(h1), bs = blit_span(h1);
    Iv wh;
    if (ss.ok && bs.ok) { wh.lo = std::min(ss.lo, bs.lo); wh.hi = std::max(ss.hi, bs.hi); wh.ok = true; }
    Lines L0 = display_lines(h0), L1 = display_lines(h1);
    // P6-a
    bool p6a = wh.ok && L0.ok;
    std::string why6a = !wh.ok ? "no stream or blit in HIRES-LOAD" : !L0.ok ? "display-line frame: " + L0.why : "";
    int lines_in = 0;
    if (p6a) {
        for (int k = 0; k <= L0.kmax; ++k)
            if (L0.lo[size_t(k)] >= wh.lo && L0.hi[size_t(k)] <= wh.hi) ++lines_in;
        if (lines_in == 0) { p6a = false; why6a = "no display line inside the interval"; }
        size_t stray = 0;
        for (size_t i = 0; i < h0.acc.size(); ++i) {
            const auto& a = h0.acc[i];
            if (a.cycle < wh.lo || a.cycle > wh.hi) continue;
            if (is_item6(a) || (kMaskBlit & (1u << h0.ch[i]))) ++stray;
        }
        if (stray != 0) { p6a = false; why6a = num(stray) + " item-6, BA or BD accesses in HIRES-REF"; }
    }
    // P6-b: the span in which both requesters are pending, and the gaps inside it
    Iv pend;
    if (ss.ok && bs.ok) { pend.lo = std::max(ss.lo, bs.lo); pend.hi = std::min(ss.hi, bs.hi); pend.ok = pend.lo < pend.hi; }
    std::vector<int> gaps;
    if (pend.ok && L1.ok)
        for (int k = 0; k <= L1.kmax; ++k)
            if (L1.hi[size_t(k)] > pend.lo && L1.lo[size_t(k + 1)] < pend.hi) gaps.push_back(k);
    bool p6b = pend.ok && L1.ok && gaps.size() >= 3;
    std::string why6b = !wh.ok ? "no stream or blit" : !L1.ok ? "display-line frame: " + L1.why : !pend.ok ? "the stream and the blit are never pending together" : gaps.size() < 3 ? "fewer than three gaps in the pending span" : "";
    // P6-c
    Iv sp = pre_span(h1);
    size_t n_dem = 0;
    if (sp.ok) n_dem = demand_count(nf, [](const Run& q, size_t i) { return (kMaskBlit & (1u << q.ch[i])) != 0; },
                                    h0, [](const Run& q, size_t i) { return (kMaskPlanes & (1u << q.ch[i])) != 0; }, sp);
    bool p6c = sp.ok && n_dem > 0;
    std::string a_dem = num(n_dem), li = num(uint64_t(lines_in));
    chk.expect(p6a,
        "P6-a [p197,p60]: the display ran and the reference shows it alone -- in HIRES-REF the " + li +
        " display lines inside the interval have fetches from each of the four planes at their addresses and no access there is attributed to item 6, BA or BD" +
        (p6a ? "" : " (unmet: " + why6a + ")") + " [not graded -- precondition, no claim uid]");
    chk.expect(p6b,
        std::string("P6-b [p20,p197,p275]: the Copper and the blitter are pending together across at least three gaps between display lines (" + num(uint64_t(gaps.size())) + " gaps in the span)") +
        (p6b ? "" : " (unmet: " + why6b + ")") + " [not graded -- precondition, no claim uid]");
    chk.expect(p6c,
        "P6-c [p198]: the blitter demanded the display's slots -- over lines $40 to $4F, in BLT-NOFIX " + a_dem +
        " accesses to BA and BD lie at the times of HIRES-REF's bitplane fetches [not graded -- precondition, no claim uid]");
    // (a)
    auto ta = trace(h1, kMaskPlanes, wh), tb = trace(h0, kMaskPlanes, wh);
    CmpRes ra = cmp_traces(ta, tb);
    std::string na = num(ra.na), nb = num(ra.nb);
    bool gate_a = c1 && c2 && c3 && c4 && c6 && c7 && p6a && p6b && p6c;
    if (gate_a) {
        chk.expect(ra.eq,
            std::string("P6a [p194,p275,p198,p197,p186] ") + kClaimsP6a + ": in a four-plane high-resolution display neither the Copper nor a blitter-nasty blit takes a slot from bitplane DMA -- all " + na +
            " bitplane fetches while the stream and the blit run happen at the same time, from the same address, with the same data as without them (" + nb + " fetches there)");
        if (!ra.eq) chk.note("P6a first mismatch: " + ra.first);
    } else {
        not_graded(chk, "P6a", std::string("C1=") + (c1 ? "1" : "0") + " C2=" + (c2 ? "1" : "0") + " C3=" + (c3 ? "1" : "0") + " C4=" + (c4 ? "1" : "0") +
                   " C6=" + (c6 ? "1" : "0") + " C7=" + (c7 ? "1" : "0") + " P6-a=" + (p6a ? "1" : "0") + " P6-b=" + (p6b ? "1" : "0") + " P6-c=" + (p6c ? "1" : "0"));
        chk.note(std::string("P6a comparator, not graded: ") + (ra.eq ? "equal" : "different") + " (" + na + " against " + nb + " fetches) " + ra.first);
    }
    // (b)
    bool gate_b = c1 && c2 && c3 && c4 && c6 && c7 && p6b;
    bool gaps_ok = p6b;
    std::string gapdesc;
    if (p6b) {
        for (int k : gaps) {
            uint64_t lo = L1.hi[size_t(k)], hi = L1.lo[size_t(k + 1)];
            size_t nc = 0, nbl = 0;
            for (size_t i = 0; i < h1.acc.size(); ++i) {
                const auto& a = h1.acc[i];
                if (a.cycle <= lo || a.cycle >= hi) continue;
                if (is_item6(a)) ++nc;
                if (kMaskBlit & (1u << h1.ch[i])) ++nbl;
            }
            if (nc == 0 || nbl == 0) { gaps_ok = false; gapdesc += (gapdesc.empty() ? "" : ", ") + std::string("gap after line ") + num(uint64_t(k)) + ": " + num(nc) + " item-6 fetches and " + num(nbl) + " blitter accesses"; }
        }
        if (gapdesc.empty()) gapdesc = "every one of the " + num(uint64_t(gaps.size())) + " gaps holds both";
    }
    if (gate_b) {
        chk.expect(gaps_ok,
            "P6b [p197,p275,p23]: in a four-plane high-resolution display both the Copper and the blitter still get slots between the lines' data fetches -- every gap inside the span in which both are pending holds at least one item-6 fetch and one blitter access (" +
            gapdesc + ") [not graded -- no claim uid]");
    } else {
        not_graded(chk, "P6b", std::string("C1=") + (c1 ? "1" : "0") + " C2=" + (c2 ? "1" : "0") + " C3=" + (c3 ? "1" : "0") + " C4=" + (c4 ? "1" : "0") +
                   " C6=" + (c6 ? "1" : "0") + " C7=" + (c7 ? "1" : "0") + " P6-b=" + (p6b ? "1" : "0") + " frame=" + (L1.ok ? "1" : "0"));
        chk.note(std::string("P6b comparator, not graded: ") + (gaps_ok ? "every gap holds both" : "a gap lacks one or the gaps are undefined") + " " + gapdesc);
    }
    // reported, not graded
    if (wh.ok && L0.ok) {
        size_t in_c = 0, in_b = 0;
        for (size_t i = 0; i < h1.acc.size(); ++i) {
            const auto& a = h1.acc[i];
            if (a.cycle < wh.lo || a.cycle > wh.hi) continue;
            bool c = is_item6(a), b = (kMaskBlit & (1u << h1.ch[i])) != 0;
            if (!c && !b) continue;
            for (int k = 0; k <= L0.kmax; ++k)
                if (a.cycle > L0.lo[size_t(k)] && a.cycle < L0.hi[size_t(k)]) { if (c) ++in_c; if (b) ++in_b; break; }
        }
        uint64_t dmin = ~0ull, dmax = 0;
        for (int k = 0; k <= L0.kmax; ++k)
            if (L0.lo[size_t(k)] >= wh.lo && L0.hi[size_t(k)] <= wh.hi) {
                uint64_t d = L0.hi[size_t(k)] - L0.lo[size_t(k)];
                dmin = std::min(dmin, d); dmax = std::max(dmax, d);
            }
        chk.note("P6 reported, not graded: within the interval " + num(wh.lo) + ".." + num(wh.hi) + " HIRES-LOAD has " + num(in_c) + " item-6 fetches and " + num(in_b) +
                 " blitter accesses strictly inside a display line's fetch interval; the fetch interval of a display line lasts " + num(dmin) + " to " + num(dmax) +
                 " clk_sys cycles in HIRES-REF");
    }
}


int real_main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    Ctx cx;
    cx.specs["REF"] = mk(0, false, 0x50, false);
    cx.specs["BLT"] = mk(0, true, 0x50, false);
    cx.specs["CPU"] = mk(0, false, 0x50, true);
    cx.specs["NOCOP"] = mk(0, false, 0xE0, false);
    cx.specs["NOFIX"] = mk(1, false, 0x50, false);
    cx.specs["BLT-NOCOP"] = mk(0, true, 0xE0, false);
    cx.specs["BLT-NOFIX"] = mk(1, true, 0x50, false);
    cx.specs["CPU-NOCOP"] = mk(0, false, 0xE0, true);
    cx.specs["HIRES-REF"] = mk(2, false, 0xE0, false);
    cx.specs["HIRES-LOAD"] = mk(2, true, 0x50, false);

    // the stimulus mutations: each replaces the named run by its contrasting case
    bool g1 = true, g2 = true, g3 = true, g4 = true, g5 = true, g6 = true, fixture = true;
    if (!mv.empty()) {
        g1 = g2 = g3 = g4 = g5 = g6 = fixture = false;
        Spec& REF = cx.specs["REF"]; Spec& BLT = cx.specs["BLT"]; Spec& CPU = cx.specs["CPU"];
        Spec& NOFIX = cx.specs["NOFIX"]; Spec& H0 = cx.specs["HIRES-REF"]; Spec& H1 = cx.specs["HIRES-LOAD"];
        if (mv == "FIXTURE") { fixture = true; REF.bin = "prog_load_fixture_mut.bin"; }
        else if (mv == "C1") { g1 = true; BLT.bin = "prog_load_nop.bin"; }
        else if (mv == "C7") { g1 = true; BLT.ntsc = true; }
        else if (mv == "C2") { g1 = true; BLT.dmacon = 0x83FF; BLT.dmacon_decl = 0x87FF; }
        else if (mv == "C3") { g1 = true; BLT.loop = true; }
        else if (mv == "C4") { g1 = true; BLT.last_wait = true; }
        else if (mv == "C5") { g2 = true; CPU.bin = "prog_load_c5.bin"; REF.bin = "prog_load_c5.bin"; }
        else if (mv == "C6") { g2 = true; CPU.vec_wrong = true; }
        else if (mv == "P1") { g1 = true; BLT.item5 = 0x5101; }
        else if (mv == "P1-a") { g1 = true; BLT.blt_data = 0x0100; }
        else if (mv == "P1-b") { g1 = true; cx.specs["BLT-NOCOP"].blit = false; }
        else if (mv == "P2") { g2 = true; CPU.item5 = 0x5101; }
        else if (mv == "P2-a") { g2 = true; CPU.loop = false; }
        else if (mv == "P2-b") { g2 = true; cx.specs["CPU-NOCOP"].loop = false; }
        else if (mv == "P3") { g3 = true; REF.item5 = 0x5101; }
        else if (mv == "P3-a") { g3 = true; REF.fix = 1; }
        else if (mv == "P3-b") { g3 = true; NOFIX.fix = 0; }
        else if (mv == "P4") { g4 = true; BLT.aud0per = 200; }
        else if (mv == "P4-a") { g4 = true; BLT.ddfstrt = 0x0030; REF.ddfstrt = 0x0030; }
        else if (mv == "P4-b") { g4 = true; cx.specs["BLT-NOFIX"].blit = false; }
        else if (mv == "P5a") { g5 = true; REF.aud0per = 200; }
        else if (mv == "P5b") { g5 = true; CPU.aud0per = 200; }
        else if (mv == "P5-a") { g5 = true; REF.fix = 1; CPU.fix = 1; }
        else if (mv == "P6a") { g6 = true; H1.bpl1mod = 2; }
        else if (mv == "P6b" || mv == "P6-b") { g6 = true; H1.item5 = 0xE001; H1.blt_data = 0x0040; }
        else if (mv == "P6-a") { g6 = true; H0.bplcon0 = 0x8200; }
        else if (mv == "P6-c") { g6 = true; cx.specs["BLT-NOFIX"].blit = false; }
        else { std::printf("unknown NMH_MUTATE %s\n", mv.c_str()); return 2; }
    }

    // ---- FIXTURE: the program stores a constant; it must read back before anything else is read
    {
        Run& r = cx.get("REF");
        checks.expect(r.ok() && r.r_fix == 0xBEEF,
            "FIXTURE: a constant stored by the program ($BEEF) reads back from Chip memory after the run [not graded -- apparatus check, no claim uid]");
    }
    if (cx.haveR) {
        std::string ln = num(uint64_t(cx.anchor.lines));
        checks.expect(cx.anchor.ok,
            "FIG69 [p194,p195,p23]: the Figure 6-9 anchor -- in each of the " + ln + " full lines of NOFIX's field F the `refresh` output is high in four bus cycles, 16 clk_sys apart (every other memory cycle), and the lines are 1816 clk_sys (227 colour clocks) apart" +
            (cx.anchor.ok ? "" : " (unmet: " + cx.anchor.text + ")") + " [not graded -- precondition, no claim uid]");
    }
    if (g1) gP1(cx, checks);
    if (g2) gP2(cx, checks);
    if (g3) gP3(cx, checks);
    if (g4) gP4(cx, checks);
    if (g5) gP5(cx, checks);
    if (g6) gP6(cx, checks);

    // ---- the common preconditions, over the comparisons evaluated above
    if (!(mv == "FIXTURE")) {
        const char* names[8] = {"", "C1", "C2", "C3", "C4", "C5", "C6", "C7"};
        const char* texts[8] = {"",
            "alignment -- the two runs of each comparison have identical port traces up to the first access at which they differ by construction",
            "DMA settings took -- DMACONR bits 10-0 read back as written, and BLTPRI reads 1 in every run that blits",
            "the 68000 is quiet -- in a STOP run no 68000 access lies within field F",
            "the stream is complete -- all 512 item-6 words are fetched in field F with the placed data, in ascending address order",
            "setup over -- the 68000 reads the CPU variant word before field F starts",
            "vector in place -- the vector word read back equals the handler address",
            "refresh unchanged -- every refresh access of NOFIX appears at the same time and address"};
        for (int c = 1; c <= 7; ++c) {
            if (g_c.n[c] == 0) continue;
            std::string cn = num(uint64_t(g_c.n[c]));
            std::string bad = g_c.bad[c];
            checks.expect(g_c.ok[c],
                std::string(names[c]) + " [not graded -- precondition, no claim uid]: " + texts[c] + " (" + cn + " evaluation(s)" +
                (g_c.ok[c] ? "" : "; unmet: " + bad) + ")");
        }
    }

    // ---- the setup as the specification orders it (COP1LC = L1 written at S4.7), measured, not graded
    if (mv.empty()) {
        Spec ls = mk(0, false, 0x50, false);
        ls.bin = "prog_load_literal.bin";
        Run lr = execute(ls);
        analyze(lr, nullptr);
        uint64_t dm = 0, firstk = 0, firstv = 0;
        uint32_t kaddr = 0;
        for (const auto& a : lr.raw) {
            if (!a.is_write && a.word_addr * 2 == kVT + 2 && !dm) dm = a.cycle;
            if (!a.is_write && a.word_addr * 2 == kVT && !firstv) firstv = a.cycle;
            if (dm && !firstk && chan_of(a.word_addr, false) == K) { firstk = a.cycle; kaddr = a.word_addr * 2; }
        }
        size_t sprites = 0;
        for (size_t i = 0; i < lr.acc.size(); ++i)
            if (lr.hasF && lr.ch[i] >= SP0 && lr.ch[i] <= SP7 && lr.acc[i].cycle >= lr.fs && lr.acc[i].cycle <= lr.fe) ++sprites;
        checks.note("SETUP-ORDER, COP1LC = L1 written at S4.7 (prog_load_literal.s): the first Copper fetch after the CPU's read of the DMACON value is at address $" + hx(kaddr) +
                    " at cycle " + num(firstk) + " (" + num(firstk - dm) + " clk_sys after that read; L0 is $" + hx(kL1 - 0x100) + ", L1 is $" + hx(kL1) +
                    "); field F starts at cycle " + num(lr.fs) + ", the CPU variant word is read at cycle " + num(firstv) + ", sprite DMA has " + num(sprites) +
                    " fetches in field F");
    }

    // ---- claims not graded
    if (mv.empty()) {
        checks.record(
            "P6-LOCKOUT-WITHDRAWN [p197,p275] sol:6-system-dma.c73,opus:ch6-sysdma.c43: withdrawn -- the claims bundle the lock-out of the Copper and the blitter during the display, and the pages disagree on it "
            "(p.197 says the four-plane high-resolution display locks them out, p.275 says the display uses only the cycles the Copper does not request); the package grades only the ordering every page agrees on (P6a) and "
            "the slots between the lines' fetches (P6b), and prints the counts inside the fetches as measurements");
    }
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}

}  // namespace

int main(int argc, char** argv) { return real_main(argc, argv); }
