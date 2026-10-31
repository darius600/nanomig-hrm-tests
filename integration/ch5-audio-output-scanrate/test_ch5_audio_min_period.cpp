// test_ch5_audio_min_period.cpp -- see SPECIFICATION-min-period.md.
//
// MP1-MP5: audio DMA at, above and below the minimum period of p.141, in NTSC
// and in PAL. One run (prog_minper.s) plays one period value on channel 0;
// the Chip-memory port log and the per-cycle audio samples of the run are
// graded against the preconditions MC1-MC6 and then against the proposition.
//
// This source is included by test_ch5_audio_all.cpp, which runs it after the
// package's earlier test and prints the one combined summary line. Run on its
// own, a standard is selected by run_min_period(): NTSC and PAL are graded by
// two processes that run at the same time.
//
// How the line and the slots are found: the `refresh` output of nanomig.v is
// sampled once per system clock cycle. A refresh access is a rising edge of
// it; the memory refresh accesses begin every line (Figure 6-9, p.195: four
// refresh slots, two slots apart). A group of edges that lie 4 ticks or less
// apart is a cluster; every cluster must hold four edges at 0, 2, 4 and 6
// slots from its first, or the anchor of F4 fails. A line starts at the first
// edge of a cluster. The criterion "reads below word address $1000" is
// printed as a MEASURED cross-check and never used to grade.
//
// NMH_MUTATE selects the contrasting case of one assertion or precondition.
// Unset, every section runs its normal programs. Selectors:
//   MP1 MP2 MP3 MP4 MP5CH1 MP6LEN MP6GREEN MC1STD MC2DMA MC2ADK MC3LEN MC3OTH MC3A MC3B MC3F4 MC3TIME
//   MC4LEN MC4CPU MC5MARK MC5STOP MC6PER MC6BELOW FIXMP FIXDONE
// A selector changes only the stimulus (the period value, the harness standard,
// the program); the comparator of the line it turns red is the normal one.
//
// Every selector below reached DONE on both cores (FIXDONE excepted: its program has no DONE); each
// MUTATION line states the result measured on fx68k and on TG68K (identical on both unless said).
//
// MUTATION MP1 stimulus red NMH_MUTATE=MP1: the NTSC MP1 comparator run at P = 119 and P = 64 in place of the S_hi values
//   measured: fx68k and TG68K: P = 119 red (next 499 of 503 classified, reuse 0, repeat 2, skip 2); P = 64 red (next 732 of 936 classified,
//   reuse 0, repeat 204); 22 passed, 2 failed
// MUTATION MP2 stimulus red NMH_MUTATE=MP2: the NTSC MP2 comparator run at P = 124 and P = 64
//   measured: fx68k and TG68K: P = 124 red in (a), (b) and (a) and (b) together (next 482 of 482, no sample played again); P = 64 green in
//   all three (next 732 of 936, previous sample again 0, both buffered samples again 204); 25 passed, 3 failed. The P = 124 run is the
//   input for which (b) is red, the P = 64 run the green input of the same comparator.
// MUTATION MP3 stimulus red NMH_MUTATE=MP3: the PAL MP3 comparator run at P = 119 and P = 64
//   measured: fx68k and TG68K: P = 119 red (next 499 of 503, repeat 2, skip 2); P = 64 red (next 732 of 936, repeat 204); 22 passed, 2 failed
// MUTATION MP4 stimulus red NMH_MUTATE=MP4: the PAL MP4 comparator run at P = 124 and P = 64
//   measured: fx68k and TG68K: P = 124 red (next 482 of 482); P = 64 green (next 732 of 936); 23 passed, 1 failed
// MP5 (unfalsifiable at this boundary): no stimulus the pages permit makes one channel take a second slot in a line, so no contrast can
//   violate a channel's bound. NMH_MUTATE=MP5CH1 is a control, not a mutation: NTSC P = 64 with channel 1 also set up on the same table,
//   period 64, DMACON $8203
//   measured: fx68k and TG68K: MP5 green, one read per channel in every line (114 cycles: 1; 130 cycles: 1; 526 reads in 263 lines);
//   MC1, MC2, MC4 with two slot offsets, MC5 green; 15 passed, 0 failed
// MUTATION MP6 stimulus green NMH_MUTATE=MP6GREEN: MP6 is red in the normal run (P = 110, 116, 118), so the same comparator is run on two periods
//   that are green in the normal run: NTSC and PAL, P = 64 and P = 124
//   measured: fx68k and TG68K: MP6 green in both standards (2 runs graded, 7 complete passes in their windows); 42 passed, 0 failed
// MUTATION MP6 stimulus red NMH_MUTATE=MP6LEN: NTSC and PAL P = 64 (green under MP6GREEN) with AUD0LEN <- 32 written once after N_run / 2 lines of the wait
//   measured: fx68k and TG68K: MP6 red in both standards (4 short passes, the first a T that follows T+62 at 35866.0 ticks after the start of the
//   port window; no read off the table, none twice in a row); MC1, MC2, MC4, MC5 green; 28 passed, 2 failed
// MUTATION MC1 stimulus red NMH_MUTATE=MC1STD: P = 124 declared NTSC with the harness PAL, and declared PAL with the harness NTSC
//   measured: fx68k and TG68K: MC1 red in both (R+4 = 312 and 262); the other preconditions of both runs green; 28 passed, 2 failed
// MUTATION MC2 stimulus red NMH_MUTATE=MC2DMA: PAL P = 124 with channel 3 on a second table and DMACON $8209
//   measured: fx68k and TG68K: MC2 red (DMACONR bits 9-0 = 521); MC1, MC4, MC5, MC6 green; the proposition not graded; 14 passed, 1 failed
// MUTATION MC2 stimulus red NMH_MUTATE=MC2ADK: PAL P = 124 with ADKCON $8080 written after step 3
//   measured: fx68k and TG68K: MC2 red (ADKCONR bits 7-0 = 128); MC1, MC4, MC5, MC6 green; 14 passed, 1 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3LEN: both reference runs with AUD0LEN <- 32
//   measured: fx68k and TG68K: MC3 item 2 red in PAL and NTSC (the 256 level runs show 64 distinct levels, the output is live); item 1 green;
//   item 3 not graded; 6 passed, 2 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3OTH: both reference runs with channel 1 playing a copy of the table at T3, volume 64, period 1000, DMACON $8203
//   measured: fx68k and TG68K: MC3 item 1 red in PAL and NTSC (264 level changes of the other output in the run); item 2 and anchors F1 (a), (b), (c) and F4
//   green; 14 passed, 2 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3A: both reference runs with AUD0PER <- 448 written after 753 lines of the wait
//   measured: fx68k and TG68K: MC3 anchor F1 (a) red in PAL and NTSC (largest deviation of a level run from D_ref 7.938 cycles on fx68k,
//   7.969 cycles on TG68K); items 1 and 2 and anchors F1 (b), F1 (c) and F4 green; 14 passed, 2 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3B: both reference runs with AUD0PER <- 224 written after 800 lines of the wait
//   measured: fx68k and TG68K: MC3 anchor F1 (b) red in PAL and NTSC (mean spacing of table fetches over 192 fetches 5012.917 cycles against
//   2 * D_ref = 7152.000, tolerance 9.466 cycles); items 1 and 2 and anchors F1 (a), F1 (c) and F4 green; 14 passed, 2 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3F4: both reference runs with one CPU read of the word at T+64 after 1140 lines of the wait
//   measured: fx68k and TG68K: MC3 anchor F4 red in PAL and NTSC (228 table accesses in the port window at 2 different offsets from the line
//   start; 0 refresh clusters of 1160 on fx68k and of 1161 on TG68K fail the shape check); items 1 and 2 and anchors F1 (a), (b) and (c) green;
//   14 passed, 2 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3TIME: both reference runs with AUD0PER 448 and the declaration left at 447
//   measured: fx68k and TG68K: MC3 anchor F1 (c) red in PAL and NTSC (tick 8.0179 cycles, 895 complete lines of length 226.493 ticks, all outside
//   226.875 ...); anchor F4 green (0 of 1162 clusters on fx68k, 0 of 1163 on TG68K, not four accesses two slots apart); items 1, 2 and anchors
//   (a), (b) green; 14 passed, 2 failed
// MUTATION MC4 stimulus red NMH_MUTATE=MC4LEN: PAL P = 124 with AUD0LEN 65 and the bytes 0 and 2 at T+128
//   measured: fx68k and TG68K: MC4 red (fetches off the table); MC1, MC2, MC5, MC6 green; 14 passed, 1 failed
// MUTATION MC4 stimulus red NMH_MUTATE=MC4CPU: PAL P = 124 with one CPU read of the word at T+64 in the middle of the wait
//   measured: fx68k and TG68K: MC4 red (a read off the channel offset); MC1, MC2, MC5, MC6 green; 14 passed, 1 failed
// MUTATION MC5 stimulus red NMH_MUTATE=MC5MARK: PAL P = 124 without the done-marker store
//   measured: fx68k and TG68K: MC5 red (marker absent); MC1, MC2, MC4, MC6 green; 14 passed, 1 failed
// MUTATION MC5 stimulus red NMH_MUTATE=MC5STOP: PAL P = 64 with DMACON <- $0001 after 200 lines of the wait and a delay of 30 loop passes
//   measured: fx68k, TG68K and TG68K shipped kernel: MC5 red through both of its clauses (no read at the channel offset in the first and last
//   tenth of the port window, the last such read before the output window ends); MC1, MC2, MC4 green (2 wraps in the port window, 0 reads out of
//   order). MC6 green on fx68k and TG68K shipped kernel (largest distance from the grid 0.0000 ticks; 14 passed, 1 failed), red on TG68K
//   (largest distance from the grid 1.0000 ticks; 13 passed, 2 failed): SPECIFICATION-min-period.md 6.1
// MUTATION MC6 stimulus red NMH_MUTATE=MC6PER: PAL with AUD0PER 129 written while 128 is declared
//   measured: fx68k and TG68K: MC6 red (largest distance from the 128-tick grid 64 ticks); MC1, MC2, MC4, MC5 green; 14 passed, 1 failed
// MUTATION MC6 stimulus red NMH_MUTATE=MC6BELOW: PAL with AUD0PER 101 written while 100 is declared, below the minimum
//   measured: fx68k and TG68K: MC6 red (largest distance from the 100-tick grid 50 ticks); MC1, MC2, MC4, MC5 green; MP4 not graded (precondition
//   failed); 14 passed, 1 failed
// MUTATION FIXTURE-CONST stimulus red NMH_MUTATE=FIXMP: PAL P = 124 with the fixture constant $DEAD in place of $BEEF
//   measured: fx68k and TG68K: FIXTURE-CONST red; behind it MC1-MC6 and the proposition not graded (RECORDED); 9 passed, 1 failed
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXDONE: PAL P = 124 run with prog_nodone, a program that never writes the done marker
//   measured: fx68k and TG68K: "reached DONE" red; behind it MC1-MC6 and the proposition not graded (RECORDED); 8 passed, 1 failed

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <iostream>

#include "nanomig_harness.h"

#ifndef NMH_XSTR
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)
#endif

namespace minper {

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

constexpr uint32_t kT        = 0x002000;   // the table, 64 words
constexpr uint32_t kT3       = 0x002200;   // channel 3's table (MC2 contrast)
constexpr uint32_t kTW       = kT / 2;     // word address of T
constexpr uint32_t kT3W      = kT3 / 2;
constexpr uint32_t kResults  = 0x0C0000;
constexpr uint32_t kR        = kResults + 0x100;
constexpr uint32_t kKfix     = kResults + 0x120;
constexpr uint32_t kMpP      = kResults + 0x200;
constexpr uint32_t kMpN      = kResults + 0x202;
constexpr uint32_t kProgWord = 0x080400 / 2;
constexpr uint32_t kResWord  = kResults / 2;
constexpr uint16_t kFix      = 0xBEEF;
constexpr int      kPref     = 447;

std::string S(long v) { return std::to_string(v); }
std::string F(double v, int prec = 3) { char b[64]; std::snprintf(b, sizeof b, "%.*f", prec, v); return b; }

inline int tsettle(int P) { return std::max(128 * P, 14600); }
inline int window(int P)  { return std::max(60000, 454 * P); }
inline int nrun(int P)    { return std::max(330, (int)std::ceil((tsettle(P) + window(P)) / 227.0) + 16); }

// ---------------------------------------------------------------- capture

struct Cap {
    bool loaded = false, done = false;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::AudioSample> audio;
    std::vector<uint8_t> refresh;   // `refresh` output, one entry per clock cycle from refbase
    uint64_t refbase = 0;
    uint16_t r0 = 0, r2 = 0, rmax = 0, rlines = 0, rmark = 0, kfix = 0;
    size_t prog_words = 0;
};

Cap capture(const std::string& dir, const std::string& prog, bool ntsc, int P, int N) {
    nmh::Config c;
    c.chipset_config = ntsc ? 0x0A : 0x08;   // ECS; bit 1 selects NTSC
    nmh::NanomigHarness h(c);
    Cap cap;
    size_t sz = h.load_program(dir + "/" + prog + ".bin");
    cap.loaded = sz > 0;
    if (!cap.loaded) return cap;
    cap.prog_words = sz / 2;
    h.poke_word(kMpP / 2, uint16_t(P));
    h.poke_word(kMpN / 2, uint16_t(N));
    h.boot(8000000);
    cap.refbase = h.cycles();
    const uint64_t budget = (uint64_t)(N + 80) * 2000 + 5000000;
    cap.refresh.reserve(budget / 2);
    for (uint64_t i = 0; i < budget; ++i) {
        h.run_cycles(1);
        cap.refresh.push_back(h.ports()->refresh ? 1 : 0);
        if ((i & 255) == 255 && h.peek_logical(0x0C0FFE).value_or(0) == 0x600D) { cap.done = true; break; }
    }
    auto pk = [&](uint32_t a) { return h.peek_logical(a).value_or(0xFFFF); };
    cap.r0 = pk(kR + 0); cap.r2 = pk(kR + 2); cap.rmax = pk(kR + 4);
    cap.rlines = pk(kR + 6); cap.rmark = pk(kR + 8); cap.kfix = pk(kKfix);
    cap.log = h.ram_log();
    cap.audio = h.audio();
    return cap;
}

// ------------------------------------------------------------------ lines

bool is_cpu(const nmh::RamAccess& a, size_t prog_words) {
    if (a.word_addr >= nmh::kKickShadowWordBase) return true;                       // reset-vector window
    if (a.word_addr >= kProgWord && a.word_addr < kProgWord + prog_words + 64) return true;
    if (a.word_addr >= kResWord && a.word_addr < kResWord + 0x800) return true;     // result page
    return false;
}
bool in_t(uint32_t w)  { return w >= kTW && w < kTW + 64; }
bool in_audio(uint32_t w) { return (w >= kTW && w < kTW + 66) || (w >= kT3W && w < kT3W + 64); }

struct LineSet {
    std::vector<uint64_t> start;
    size_t groups = 0, bad = 0;
};

LineSet make_lines(const Cap& c, double tickc, uint64_t from) {
    LineSet L;
    std::vector<uint64_t> edges;
    for (size_t i = 1; i < c.refresh.size(); ++i)
        if (c.refresh[i] && !c.refresh[i - 1]) edges.push_back(c.refbase + i);
    std::vector<std::vector<uint64_t>> groups;
    for (size_t i = 0; i < edges.size(); ++i) {
        if (i == 0 || double(edges[i] - edges[i - 1]) > 4.0 * tickc) groups.push_back({});
        groups.back().push_back(edges[i]);
    }
    if (!groups.empty()) groups.pop_back();     // the group the capture ended in
    for (const auto& g : groups) {
        bool ok = g.size() == 4;
        for (size_t k = 0; ok && k < g.size(); ++k)
            if (std::fabs(double(g[k] - g[0]) - 2.0 * double(k) * tickc) > 0.5) ok = false;
        if (g.front() >= from) { ++L.groups; if (!ok) ++L.bad; }
        if (ok) L.start.push_back(g.front());
    }
    return L;
}

// MEASURED cross-check only: reads below word address $1000 that are not the CPU's.
size_t low_reads(const Cap& c, uint64_t from) {
    size_t n = 0;
    for (const auto& a : c.log)
        if (!a.is_write && a.cycle >= from && a.word_addr < 0x1000 && !is_cpu(a, c.prog_words)) ++n;
    return n;
}

// Index of the line a cycle lies in, or -1.
long line_of(const LineSet& L, uint64_t cyc) {
    auto it = std::upper_bound(L.start.begin(), L.start.end(), cyc);
    if (it == L.start.begin()) return -1;
    return long(it - L.start.begin()) - 1;
}

// ----------------------------------------------------------------- output

struct Chg { uint64_t t; uint16_t v; };

uint16_t side_val(const nmh::AudioSample& s, bool right) { return right ? s.right : s.left; }

std::vector<Chg> changes(const Cap& c, bool right, uint64_t from) {
    std::vector<Chg> out;
    if (c.audio.size() <= from + 1) return out;
    uint16_t prev = side_val(c.audio[from], right);
    for (size_t i = from + 1; i < c.audio.size(); ++i) {
        uint16_t v = side_val(c.audio[i], right);
        if (v != prev) { out.push_back({i, v}); prev = v; }
    }
    return out;
}

// First read of the table range, or -1.
long first_fetch(const Cap& c) {
    for (const auto& a : c.log) if (!a.is_write && in_t(a.word_addr)) return (long)a.cycle;
    return -1;
}

// ------------------------------------------------------------- reference

struct Ref {
    bool ntsc = false;
    bool ok = false;           // every part of MC3 holds
    bool cal = false;          // the level cycle and the tick are known
    bool right = false;
    double tickc = 8.0;        // system clock cycles per tick
    double D = 0;
    double s = 0;              // output resolution, ticks
    std::vector<uint16_t> cyc; // the level cycle, raw levels in order
    std::map<uint16_t, int> idx;
};

struct PortWin {
    bool ok = false;
    uint64_t f0 = 0, pw0 = 0, pw1 = 0, last_fetch = 0;
};

PortWin port_window(const Cap& c, int declP, double tickc) {
    PortWin p;
    long f0 = first_fetch(c);
    if (f0 < 0) return p;
    p.f0 = (uint64_t)f0;
    double lo = double(p.f0) + double(tsettle(declP)) * tickc;
    bool found = false;
    for (const auto& a : c.log) {
        if (a.is_write || !in_t(a.word_addr)) continue;
        if (!found && double(a.cycle) >= lo) { p.pw0 = a.cycle; found = true; }
        p.last_fetch = a.cycle;
    }
    if (!found) return p;
    p.pw1 = p.pw0 + (uint64_t)(double(window(declP)) * tickc);
    p.ok = p.pw1 <= c.audio.size();
    return p;
}

// Evaluates MC3 on a reference-type capture and prints its lines. declP is the
// period the run declares (447).
Ref eval_ref(Checks& ck, const Cap& c, bool ntsc, int declP, const std::string& tag) {
    Ref r;
    r.ntsc = ntsc;
    const std::string stdname = ntsc ? "NTSC" : "PAL";
    long f0l = first_fetch(c);
    if (f0l < 0) {
        ck.expect(false, "MC3 " + tag + " item 2: the channel-0 output shows exactly 128 distinct levels (no table fetch in the run)");
        ck.record("MC3 " + tag + ": items 1 and 3 not graded, the run holds no table fetch");
        return r;
    }
    uint64_t f0 = (uint64_t)f0l;
    auto cl = changes(c, false, f0), cr = changes(c, true, f0);
    r.right = cr.size() > cl.size();
    const auto& chg = r.right ? cr : cl;
    const auto& oth = r.right ? cl : cr;
    ck.note("MC3 " + tag + ": the output that carries channel 0 is the " + std::string(r.right ? "right" : "left") +
            " one (changes after the first fetch: left " + S((long)cl.size()) + ", right " + S((long)cr.size()) +
            "); not graded");
    if (chg.size() < 20) {
        ck.expect(false, "MC3 " + tag + " item 2: the channel-0 output shows exactly 128 distinct levels, repeating every 128 level runs "
                         "(p.140; " + S((long)chg.size()) + " level changes in the run, too few for a window)");
        ck.expect(oth.empty(), "MC3 " + tag + " item 1: the output that does not carry channel 0 is constant throughout the window");
        ck.record("MC3 " + tag + ": item 3 not graded, no level cycle appears");
        return r;
    }
    // Preliminary tick: median spacing of the changes after the start-up, over 447 ticks.
    std::vector<double> gaps;
    for (size_t i = 10; i + 1 < chg.size() && i < 200; ++i) gaps.push_back(double(chg[i + 1].t - chg[i].t));
    std::sort(gaps.begin(), gaps.end());
    double tick0 = gaps[gaps.size() / 2] / double(declP);
    double lo = double(chg[0].t) + double(tsettle(declP)) * tick0;
    size_t i0 = 0;
    while (i0 < chg.size() && double(chg[i0].t) < lo) ++i0;
    uint64_t wend = chg[i0].t + (uint64_t)(double(window(declP)) * tick0);
    bool item1 = true;
    for (const auto& x : oth) if (x.t >= chg[i0].t && x.t < wend) item1 = false;
    // 256 runs from i0.
    bool enough = i0 + 256 < chg.size();
    bool item2 = false;
    size_t ndist = 0;
    if (enough) {
        std::set<uint16_t> dist;
        for (size_t i = 0; i < 256; ++i) dist.insert(chg[i0 + i].v);
        ndist = dist.size();
        bool per = dist.size() == 128;
        for (size_t i = 0; per && i + 128 < 256; ++i) if (chg[i0 + i].v != chg[i0 + i + 128].v) per = false;
        std::set<uint16_t> first128;
        for (size_t i = 0; i < 128; ++i) first128.insert(chg[i0 + i].v);
        item2 = per && first128.size() == 128;
        for (size_t i = 0; i < 128; ++i) r.cyc.push_back(chg[i0 + i].v);
    }
    if (enough) ck.note("MC3 " + tag + ": the first 256 level runs of the window show " + S((long)ndist) + " distinct levels");
    ck.expect(item1, "MC3 " + tag + " item 1: the output that does not carry channel 0 is constant throughout the window");
    ck.expect(item2, "MC3 " + tag + " item 2: the channel-0 output shows exactly 128 distinct levels, repeating every 128 level runs, each level once per period (p.140)");
    if (!item2) {
        ck.record("MC3 " + tag + ": item 3 not graded, no level cycle appears");
        return r;
    }
    for (size_t i = 0; i < r.cyc.size(); ++i) r.idx[r.cyc[i]] = (int)i;
    // D_ref over 256 runs
    double sum = 0, dmax = 0, dmin = 0;
    std::vector<double> d(256);
    for (size_t i = 0; i < 256; ++i) { d[i] = double(chg[i0 + i + 1].t - chg[i0 + i].t); sum += d[i]; }
    r.D = sum / 256.0;
    r.tickc = r.D / double(declP);
    r.s = 1.0 / r.tickc;
    for (double x : d) { dmax = std::max(dmax, x - r.D); dmin = std::max(dmin, r.D - x); }
    bool a_ok = dmax <= 1.0 + 1e-9 && dmin <= 1.0 + 1e-9;
    ck.note("MC3 " + tag + ": D_ref = " + F(r.D) + " cycles, tick = " + F(r.tickc, 4) + " cycles, largest deviation of a level run from D_ref = " +
            F(std::max(dmax, dmin)) + " cycles, output resolution s = " + F(r.s, 4) + " ticks (one cycle)");
    ck.expect(a_ok, "MC3 " + tag + " anchor F1 (a): every one of 256 level runs lasts D_ref to within one output sample spacing");
    // (b) and (c)
    PortWin pw = port_window(c, declP, r.tickc);
    LineSet L = make_lines(c, r.tickc, f0);
    bool b_ok = false;
    if (pw.ok) {
        std::vector<uint64_t> wraps;
        std::vector<uint64_t> fet;
        uint32_t prev = 0xFFFFFFFF;
        for (const auto& a : c.log) {
            if (a.is_write || !in_t(a.word_addr)) continue;
            if (a.cycle >= pw.pw0 && a.cycle < pw.pw1) {
                fet.push_back(a.cycle);
                if (a.word_addr == kTW && prev == kTW + 63) wraps.push_back(a.cycle);
            }
            prev = a.word_addr;
        }
        if (wraps.size() >= 2) {
            size_t cnt = 0;
            for (uint64_t t : fet) if (t >= wraps.front() && t < wraps.back()) ++cnt;
            double mean = double(wraps.back() - wraps.front()) / double(cnt);
            // The tolerance is the resolution of the measurement. A table fetch lies in the one slot per line
            // that its channel owns (p.141), so the time of a fetch is known to one line, and the time between
            // the first and the last wrap to one line over the cnt fetches between them. D_ref is the span of
            // 256 level runs, each end known to one cycle: 2 / 256 cycles on 2 * D_ref.
            double lmax = 0;
            for (size_t i = 0; i + 1 < L.start.size(); ++i)
                if (L.start[i] >= pw.pw0 && L.start[i + 1] <= pw.pw1) lmax = std::max(lmax, double(L.start[i + 1] - L.start[i]));
            double tol = lmax / double(cnt) + 2.0 / 256.0;
            double diff = std::fabs(mean - 2.0 * r.D);
            b_ok = lmax > 0 && diff <= tol;
            ck.note("MC3 " + tag + ": mean spacing of table fetches over " + S((long)cnt) + " fetches = " + F(mean) + " cycles, 2 * D_ref = " +
                    F(2.0 * r.D) + " cycles, difference " + F(diff) + " cycles, tolerance " + F(tol) + " cycles (longest complete line " + F(lmax, 0) +
                    " cycles over " + S((long)cnt) + " fetches, plus 2 / 256)");
        } else {
            ck.note("MC3 " + tag + ": the port window holds " + S((long)wraps.size()) + " wraps, fewer than the 2 that give a whole pass");
        }
    }
    ck.expect(b_ok, "MC3 " + tag + " anchor F1 (b): the mean spacing of consecutive table fetches over whole passes is 2 * D_ref to within the resolution of the measurement, "
                    "one line over the number of fetches plus 2 / 256 cycle (p.138, p.139, p.165; p.141: one DMA slot per line)");
    // F4: the line frame that F1 (c) measures with is asserted before it is used.
    bool f4_ok = false;
    {
        std::set<long> fo;
        long nf = 0;
        if (pw.ok)
            for (const auto& a : c.log) {
                if (a.is_write || !in_t(a.word_addr) || a.cycle < pw.pw0 || a.cycle >= pw.pw1) continue;
                long li = line_of(L, a.cycle);
                fo.insert(li < 0 ? -1 : (long)(a.cycle - L.start[(size_t)li]));
                ++nf;
            }
        f4_ok = pw.ok && L.groups > 0 && L.bad == 0 && nf > 0 && fo.size() == 1 && *fo.begin() >= 0;
        ck.note("MC3 " + tag + ": F4 anchor: refresh clusters checked " + S((long)L.groups) + ", not four accesses two slots apart " + S((long)L.bad) +
                "; table fetches in the port window " + S(nf) + " at " + S((long)fo.size()) + " different offset(s) from the line start");
    }
    ck.expect(f4_ok, "MC3 " + tag + " anchor F4: every refresh cluster holds four accesses two slots apart and every table fetch of the port window lies at one offset from its line start (Figure 6-9, p.195)");
    bool c_ok = false;
    std::vector<double> ll;
    if (pw.ok) {
        for (size_t i = 0; i + 1 < L.start.size(); ++i)
            if (L.start[i] >= pw.pw0 && L.start[i + 1] <= pw.pw1) ll.push_back(double(L.start[i + 1] - L.start[i]) / r.tickc);
    }
    if (!ll.empty()) {
        double m = 0; for (double x : ll) m += x; m /= double(ll.size());
        // Every complete line lies between the lengths the pages name, to within the output resolution s
        // (one system cycle in ticks): NTSC 227 and 228 (p.23), PAL 227 (p.23) and 227.5 (p.304).
        const double lo = 227.0 - r.s, hi = (ntsc ? 228.0 : 227.5) + r.s;
        size_t n227 = 0, n228 = 0, nalt = 0, nbad = 0;
        for (size_t i = 0; i < ll.size(); ++i) {
            if (ll[i] < lo || ll[i] > hi) ++nbad;
            if (std::fabs(ll[i] - 227.0) <= r.s) ++n227;
            else if (std::fabs(ll[i] - 228.0) <= r.s) ++n228;
            if (i > 0 && std::fabs(ll[i] - ll[i - 1]) > r.s) ++nalt;
        }
        c_ok = nbad == 0;
        ck.note("MC3 " + tag + ": " + S((long)ll.size()) + " complete lines in the port window, mean length " + F(m) + " ticks, " + S((long)nbad) + " outside " + F(lo) + " ... " + F(hi) +
                " ticks; lines of 227 ticks " + S((long)n227) +
                ", of 228 ticks " + S((long)n228) + ", lines whose length differs from the line before " + S((long)nalt) +
                (ntsc ? " (p.23's alternation of 228 and 227 and p.194's 227.5 mean are measured here, not graded; NM-HRM-010 grades them)" : " (reported, not graded)") + "; "
                "refresh clusters checked " + S((long)L.groups) + ", not four accesses two slots apart " + S((long)L.bad) + "; reads below word $1000 from the first fetch on: " +
                S((long)low_reads(c, f0)) + " (cross-check only)");
    }
    ck.expect(c_ok, "MC3 " + tag + " anchor F1 (c): every complete line of the port window is " +
                    std::string(ntsc ? "227 or 228 ticks long (p.23), to within the output resolution s" : "between 227 (p.23) and 227.5 (p.304) ticks long, to within the output resolution s"));
    r.cal = true;
    r.ok = item1 && item2 && a_ok && b_ok && f4_ok && c_ok;
    return r;
}

// ------------------------------------------------------------------- runs

// ------------------------------------------------------------------- MP6

// MP6: channel 0's reads at its slot offset form passes of exactly 64 words,
// T to T+126 in order, each followed by T (p.139, p.147).
struct Mp6 {
    int P = 0;
    bool graded = false;
    std::string why;                 // why it was not graded
    long reads = 0, passes = 0, off = 0, dbl = 0, shortp = 0, longp = 0, gap = 0;
    std::string first_off, first_dbl, first_short, first_long, first_gap;
    bool ok() const { return graded && off == 0 && dbl == 0 && shortp == 0 && longp == 0 && gap == 0; }
};

std::string toff(uint32_t w) {
    long b = 2 * ((long)w - (long)kTW);
    return b >= 0 ? "T+" + S(b) : "T-" + S(-b);
}

void mp6_eval(Mp6& m, const Cap& c, const PortWin& pw, const LineSet& L, const std::set<long>& offsets, double tk) {
    for (long o : offsets) {
        bool havep = false;
        uint32_t prev = 0;
        for (const auto& a : c.log) {
            if (a.is_write || a.cycle < pw.pw0 || a.cycle >= pw.pw1 || is_cpu(a, c.prog_words)) continue;
            long li = line_of(L, a.cycle);
            if (li < 0 || (long)(a.cycle - L.start[(size_t)li]) != o) continue;
            ++m.reads;
            const std::string when = " at " + F(double(a.cycle - pw.pw0) / tk, 1) + " ticks after the start of the port window";
            if (!in_t(a.word_addr)) {
                if (m.off++ == 0) m.first_off = "read of " + toff(a.word_addr) + when;
                havep = false;
                continue;
            }
            if (havep) {
                if (a.word_addr == prev) {
                    if (m.dbl++ == 0) m.first_dbl = toff(a.word_addr) + " read twice in a row" + when;
                } else if (a.word_addr == prev + 1) {
                } else if (prev == kTW + 63 && a.word_addr == kTW) {
                    ++m.passes;
                } else if (a.word_addr == kTW) {
                    if (m.shortp++ == 0) m.first_short = "T follows " + toff(prev) + when;
                } else if (prev == kTW + 63) {
                    if (m.longp++ == 0) m.first_long = toff(a.word_addr) + " follows T+126" + when;
                } else {
                    if (m.gap++ == 0) m.first_gap = toff(a.word_addr) + " follows " + toff(prev) + when;
                }
            }
            prev = a.word_addr;
            havep = true;
        }
    }
    m.graded = true;
}

// One MP6 line per standard; each failing period's evidence is printed by the run itself.
void mp6_summary(Checks& ck, const std::string& sn, const std::vector<Mp6>& rows) {
    std::string fail, nog;
    long ng = 0, np = 0, nf = 0;
    for (const auto& m : rows) {
        if (!m.graded) { nog += (nog.empty() ? "" : ", ") + S(m.P); continue; }
        ++ng; np += m.passes;
        if (!m.ok()) { fail += (fail.empty() ? "" : ", ") + S(m.P); ++nf; }
    }
    ck.note("MP6 " + sn + ": " + S(ng) + " runs graded, " + S(np) + " complete passes in their windows, " + S(nf) + " runs fail");
    if (!nog.empty()) ck.record("MP6 " + sn + ": not graded at P = " + nog + " (a precondition failed); see the lines of those runs");
    if (ng == 0) { ck.record("MP6 " + sn + ": no run graded"); return; }
    ck.expect(nf == 0, "MP6 " + sn + " (p.139, p.147): in every run channel 0's reads at its slot offset form passes of exactly 64 words, T to T+126 in order, "
                       "each followed by T; periods that fail: " + (fail.empty() ? std::string("none") : fail));
}

enum Kind { NONE = 0, ALLNEXT = 1, REUSE = 2 };

struct RunSpec {
    bool declNtsc = true, cfgNtsc = true;
    int P = 128, declP = 128;
    std::string prog = "prog_minper";
    unsigned declDma = 0x0201;
    int nch = 1;
    Kind kind = NONE;
    bool mp5 = false;
    std::string tag;
    std::vector<Mp6>* mp6out = nullptr;   // where the MP6 result of the run goes
};

struct Cls { int n = 0, next = 0, reuse = 0, repeat = 0, skip = 0, unm = 0, after_unm = 0, other = 0; bool ok = false; };

// Each sample period is compared with the sample period immediately before it (section 3.6).
Cls classify(const Cap& c, const Ref& r, bool right, uint64_t t_start, int P, double tickc, int N) {
    Cls k;
    long q = -1;                // the index the immediately preceding sample period carried
    bool have = false;          // q is valid: the preceding sample period was mapped
    bool first = true;          // no sample period has been mapped yet
    for (int j = 0; j < N; ++j) {
        size_t at = (size_t)(double(t_start) + (double(j) + 0.5) * double(P) * tickc);
        if (at >= c.audio.size()) return k;
        auto it = r.idx.find(side_val(c.audio[at], right));
        int cj = it == r.idx.end() ? -1 : it->second;
        if (first) {                          // c_1 is not classified; an unmapped one is counted
            if (cj >= 0) { q = cj; have = true; first = false; }
            else { ++k.n; ++k.unm; }
            continue;
        }
        ++k.n;
        if (cj < 0) { ++k.unm; have = false; continue; }
        if (!have) { ++k.after_unm; q = cj; have = true; continue; }
        int d = ((cj - (int)q) % 128 + 128) % 128;
        if (d == 1) ++k.next;
        else if (d == 0) ++k.reuse;
        else if (d == 127) ++k.repeat;
        else if (d >= 2 && d <= 63) ++k.skip;
        else ++k.other;
        q = cj;
    }
    k.ok = true;
    return k;
}

void run_one(Checks& ck, const std::string& dir, const RunSpec& rs, const Ref& ref) {
    const std::string sn = rs.declNtsc ? "NTSC" : "PAL";
    const std::string tag = rs.tag.empty() ? sn + " P=" + S(rs.P) : rs.tag;
    const int N = nrun(rs.declP);
    struct Mp6Guard {
        std::vector<Mp6>* out; Mp6 m;
        ~Mp6Guard() { if (out) out->push_back(m); }
    } g6{rs.mp6out, Mp6{}};
    g6.m.P = rs.declP;
    g6.m.why = "the run did not reach its checks";
    Cap c = capture(dir, rs.prog, rs.cfgNtsc, rs.P, N);
    bool base = c.loaded && c.done;
    ck.expect(base, "FIXTURE " + tag + ": " + rs.prog + " reached DONE [not graded -- fixture, no claim uid]");
    if (!base) { ck.record(tag + ": the run did not complete; MC1-MC6 and the propositions not graded"); return; }
    bool fx = c.kfix == kFix;
    ck.expect(fx, "FIXTURE-CONST " + tag + ": the program-stored constant reads back [not graded -- fixture, no claim uid]");
    if (!fx) { ck.record(tag + ": the fixture constant did not read back; MC1-MC6 and the propositions not graded"); return; }

    // MC1
    bool mc1 = rs.declNtsc ? (c.rmax == 261 || c.rmax == 262) : (c.rmax == 311 || c.rmax == 312);
    ck.expect(mc1, "MC1 " + tag + ": the largest vertical position recorded (" + S(c.rmax) + ") is " +
              std::string(rs.declNtsc ? "261 or 262" : "311 or 312") + " (p.24, p.40, p.34)");
    // MC2
    bool mc2 = (c.r0 & 0x3FF) == rs.declDma && (c.r2 & 0xFF) == 0;
    ck.expect(mc2, "MC2 " + tag + ": DMACONR bits 9-0 = " + S(c.r0 & 0x3FF) + " equal the declared " + S(rs.declDma) +
              " and ADKCONR bits 7-0 = " + S(c.r2 & 0xFF) + " are 0 (p.278, p.265)");
    if (!ref.cal) { ck.record(tag + ": the " + sn + " reference run gave no level cycle; MC4-MC6 and the propositions not graded"); return; }

    const double tk = ref.tickc;
    const bool below = rs.declNtsc ? rs.declP <= 123 : rs.declP <= 122;   // below the page's minimum
    PortWin pw = port_window(c, rs.declP, tk);
    long f0l = first_fetch(c);
    uint64_t f0 = f0l < 0 ? 0 : (uint64_t)f0l;
    LineSet L = make_lines(c, tk, f0);

    // ---- MC4
    bool mc4 = false, mc4_core = false, mc4_wrap = false;
    std::set<long> allowed;                   // channel 0's slot offset(s) from the line start, in cycles
    long viol_off = 0, viol_order = 0, nwraps = 0, cpu_writes = 0;
    std::map<long, int> offs;
    if (pw.ok) {
        for (const auto& a : c.log) {
            if (a.is_write) { if (in_t(a.word_addr) && a.cycle > f0) ++cpu_writes; continue; }
            if (!in_t(a.word_addr) || a.cycle < pw.pw0 || a.cycle >= pw.pw1) continue;
            long li = line_of(L, a.cycle);
            long off = li < 0 ? -1 : (long)(a.cycle - L.start[(size_t)li]);
            ++offs[off];
        }
        std::vector<std::pair<int, long>> byc;
        for (auto& kv : offs) byc.push_back({kv.second, kv.first});
        std::sort(byc.rbegin(), byc.rend());
        for (int i = 0; i < rs.nch && i < (int)byc.size(); ++i) allowed.insert(byc[(size_t)i].second);
        for (auto& kv : offs) if (!allowed.count(kv.first)) viol_off += kv.second;
        std::map<long, uint32_t> prev;
        for (const auto& a : c.log) {
            if (a.is_write || a.cycle < f0 || is_cpu(a, c.prog_words)) continue;
            long li = line_of(L, a.cycle);
            if (li < 0) continue;
            long off = (long)(a.cycle - L.start[(size_t)li]);
            if (!allowed.count(off)) continue;
            bool inw = a.cycle >= pw.pw0 && a.cycle < pw.pw1;
            auto it = prev.find(off);
            if (inw) {
                if (!in_t(a.word_addr)) ++viol_order;
                else if (it != prev.end()) {
                    uint32_t p = it->second;
                    bool step = (a.word_addr == p + 1 && in_t(p)) || (p == kTW + 63 && a.word_addr == kTW);
                    if (!step) ++viol_order;
                }
                if (a.word_addr == kTW && it != prev.end() && it->second == kTW + 63) ++nwraps;
            }
            prev[off] = a.word_addr;
        }
        std::string os;
        for (long o : allowed) os += " " + S(o) + " cycles (" + F(double(o) / tk, 2) + " ticks)";
        ck.note("MC4 " + tag + ": channel slot offset(s) from the line start:" + os + "; clusters checked " + S((long)L.groups) +
                ", not four refresh accesses two slots apart " + S((long)L.bad) + "; reads off the channel offsets " + S(viol_off) +
                ", out of order or off the table " + S(viol_order) + ", wraps in the window " + S(nwraps) +
                ", CPU writes to the table after the first fetch " + S(cpu_writes));
        mc4_core = viol_off == 0 && cpu_writes == 0 && L.bad == 0 && L.groups > 0 && !allowed.empty();
        mc4_wrap = nwraps >= 1;
        mc4 = mc4_core && mc4_wrap && (below || viol_order == 0);
    } else {
        ck.note("MC4 " + tag + ": no port window");
    }
    ck.expect(mc4, "MC4 " + tag + ": the channel fetches the table at a fixed slot offset" + std::string(below ? "" : ", in address order without a gap") +
                   ", nothing else touches it, and every line starts with four refresh accesses two slots apart (p.147, p.148, p.194, Figure 6-9)");
    if (below)
        ck.note("MC4 " + tag + ": below the minimum the address order without a gap is a result, not a precondition: " + S(viol_order) + " fetches out of order or off the table");

    // ---- output window
    const auto chg = changes(c, ref.right, f0);
    uint64_t t_first = 0, t_start = 0, t_end = 0;
    bool ow = false;
    {
        size_t i = 0;
        while (i < chg.size() && !ref.idx.count(chg[i].v)) ++i;
        if (i < chg.size()) {
            t_first = chg[i].t;
            double lo = double(t_first) + double(tsettle(rs.declP)) * tk;
            size_t j = i;
            while (j < chg.size() && double(chg[j].t) < lo) ++j;
            if (j < chg.size()) {
                t_start = chg[j].t;
                t_end = t_start + (uint64_t)(double(window(rs.declP)) * tk);
                ow = t_end <= c.audio.size();
            }
        }
    }

    // ---- MC5
    // Above the minimum the table clauses count reads of T ... T+126. Below it address
    // order is a result, so they count channel 0's DMA reads at its slot offset, at any address.
    bool mc5 = false, mc5_run = false;
    uint64_t last_fetch = pw.last_fetch;
    auto tfetch = [&](const nmh::RamAccess& a) {
        if (a.is_write) return false;
        if (!below) return in_t(a.word_addr);
        if (a.cycle < f0 || is_cpu(a, c.prog_words)) return false;
        long li = line_of(L, a.cycle);
        return li >= 0 && allowed.count((long)(a.cycle - L.start[(size_t)li])) > 0;
    };
    if (below) {
        last_fetch = 0;
        for (const auto& a : c.log) if (tfetch(a)) last_fetch = a.cycle;
    }
    {
        bool marker = c.rmark == 0xA5C3, lines = c.rlines == N;
        bool tenths = false, lastafter = false;
        std::vector<size_t> wl;
        if (pw.ok)
            for (size_t i = 0; i + 1 < L.start.size(); ++i)
                if (L.start[i] >= pw.pw0 && L.start[i + 1] <= pw.pw1) wl.push_back(i);
        if (pw.ok && !wl.empty()) {
            size_t tenth = std::max<size_t>(1, wl.size() / 10);
            uint64_t a0 = L.start[wl.front()], a1 = L.start[wl[tenth - 1] + 1];
            uint64_t b0 = L.start[wl[wl.size() - tenth]], b1 = L.start[wl.back() + 1];
            bool f = false, l = false;
            for (const auto& a : c.log) {
                if (!tfetch(a)) continue;
                if (a.cycle >= a0 && a.cycle < a1) f = true;
                if (a.cycle >= b0 && a.cycle < b1) l = true;
            }
            tenths = f && l;
        }
        lastafter = ow ? last_fetch > t_end : (pw.ok && last_fetch > pw.pw1);
        mc5_run = marker && lines;
        mc5 = mc5_run && tenths && lastafter;
        ck.note("MC5 " + tag + ": marker " + std::string(marker ? "present" : "absent") + ", R+6 = " + S(c.rlines) + " (N_run " + S(N) +
                "), " + (below ? "reads at the channel offset (any address)" : "table fetches") + " in the first and last tenth of " + S((long)wl.size()) +
                " window lines: " + (tenths ? "yes" : "no") + ", last " + (below ? "such read" : "fetch") + " after the output window: " + (lastafter ? "yes" : "no"));
    }
    ck.expect(mc5, "MC5 " + tag + ": the run is complete: marker $A5C3, " + S(N) + " lines counted, " + (below ? "reads at channel 0's slot offset" : "table fetches") +
                   " in the first and last tenth of the port window, the last " + (below ? "such read" : "fetch") + " after the output window");

    // ---- MP6: every pass restarts at the start of the data (p.139, p.147)
    {
        Mp6& m = g6.m;
        m.why = "";
        const bool pre6 = ref.ok && mc1 && mc2 && mc4_core && mc5_run && pw.ok;
        if (!pre6) {
            m.why = "precondition failed";
        } else {
            mp6_eval(m, c, pw, L, allowed, tk);
            if (!m.ok()) {
                ck.note("MP6 " + tag + ": " + S(m.reads) + " reads at the channel offset, " + S(m.passes) + " complete passes; off the table " + S(m.off) +
                        (m.off ? " (first: " + m.first_off + ")" : "") + "; read twice in a row " + S(m.dbl) + (m.dbl ? " (first: " + m.first_dbl + ")" : "") +
                        "; short passes " + S(m.shortp) + (m.shortp ? " (first: " + m.first_short + ")" : "") + "; long passes " + S(m.longp) +
                        (m.longp ? " (first: " + m.first_long + ")" : "") + "; other gaps " + S(m.gap) + (m.gap ? " (first: " + m.first_gap + ")" : ""));
            }
        }
    }

    // ---- MP5 (port based, MC1-MC5)
    if (rs.mp5) {
        // Per channel: a channel is a slot offset of MC4; the reads at that offset are counted per line.
        long maxl = 0, nl = 0, tot = 0;
        std::map<long, long> chmax;          // per slot offset, the most reads in one line
        uint64_t dur = 0;                    // cycles of the complete lines the reads were counted in
        if (pw.ok) {
            for (size_t i = 0; i + 1 < L.start.size(); ++i) {
                if (!(L.start[i] >= pw.pw0 && L.start[i + 1] <= pw.pw1)) continue;
                std::map<long, long> cnt;
                for (const auto& a : c.log)
                    if (!a.is_write && in_t(a.word_addr) && a.cycle >= L.start[i] && a.cycle < L.start[i + 1])
                        ++cnt[(long)(a.cycle - L.start[i])];
                long lt = 0, ex = 0;       // ex: reads at an offset that is no channel's (counted against the one channel when only one is set up)
                for (auto& kv : cnt) { lt += kv.second; if (!allowed.count(kv.first)) ex += kv.second; }
                for (long o : allowed) chmax[o] = std::max(chmax[o], (cnt.count(o) ? cnt[o] : 0) + (rs.nch == 1 ? ex : 0));
                ++nl; tot += lt;
                dur += L.start[i + 1] - L.start[i];
            }
        }
        std::string per;
        for (auto& kv : chmax) { maxl = std::max(maxl, kv.second); per += " " + S(kv.first) + " cycles: " + S(kv.second) + ";"; }
        const double lticks = nl > 0 ? double(dur) / tk / double(nl) : 0.0;     // mean length of those lines, ticks
        double secs = nl > 0 ? double(dur) / tk * 0.279365e-6 : 1.0;
        ck.note("MP5 " + tag + ": " + S(nl) + " complete lines, most table reads of one channel in one line (per slot offset)" + per + " " + S(tot) +
                " reads in them, 2 * reads / duration of the complete lines = " + F(2.0 * double(tot) / secs, 0) +
                " samples/second (reported, not graded; the figure follows from the " + F(lticks, 1) + "-tick lines of this run: one word per line of 227 ticks is 2 * 3,579,545 / 227 = 31,538, "
                "p.141's 31,469 rests on 227.5-tick lines)");
        if (!(ref.ok && mc1 && mc2 && mc4 && mc5)) {
            ck.record("MP5 " + tag + ": precondition failed (" + std::string(ref.ok ? "" : "MC3 ") + (mc1 ? "" : "MC1 ") + (mc2 ? "" : "MC2 ") +
                      (mc4 ? "" : "MC4 ") + (mc5 ? "" : "MC5 ") + "), not graded; the counts above are information only");
        } else {
            ck.expect(maxl <= 1 && nl > 0, "MP5 " + tag + " (p.141): no channel holds more than one data fetch in a line");
        }
    }

    if (rs.kind == NONE) return;
    const std::string id = rs.declNtsc ? (rs.kind == ALLNEXT ? "MP1" : "MP2") : (rs.kind == ALLNEXT ? "MP3" : "MP4");
    // ---- MC6
    bool mc6 = false;
    double s = ref.s;
    if (ow) {
        bool res = s <= double(rs.declP) / 2.0;
        double dmax = 0, dat = 0; long n = 0;
        for (const auto& x : chg) {
            if (x.t < t_start || x.t >= t_end || x.t > last_fetch) continue;   // after its last fetch the channel has no data to play
            double xt = double(x.t - t_start) / tk;
            double r = std::fmod(xt, double(rs.declP));
            double dd = std::min(r, double(rs.declP) - r);
            if (dd > dmax) { dmax = dd; dat = xt; }
            ++n;
        }
        mc6 = res && dmax <= s + 1e-9;
        ck.note("MC6 " + tag + ": s = " + F(s, 4) + " ticks (P/2 = " + F(double(rs.declP) / 2.0, 1) + "), " + S(n) + " level changes in the window, largest distance from the grid " +
                F(dmax, 4) + " ticks" + (dmax > 0 ? " (at " + F(dat, 1) + " ticks after the start of the output window; last channel fetch at " +
                F(double(last_fetch - t_start) / tk, 1) + " ticks)" : std::string()));
    }
    ck.expect(mc6, "MC6 " + tag + ": the period is in effect and the output resolved: s <= P/2 and every level change lies within s of the P grid (p.140, p.164)");
    const bool pre = ref.ok && mc1 && mc2 && mc4 && mc5 && mc6;
    // Above the minimum MP1 and MP3 are graded when only MC4's address-order clause or MC5's table clauses fail.
    const bool pre_rest = ref.ok && mc1 && mc2 && mc4_core && mc4_wrap && mc5_run && mc6;
    const bool graded = pre || (rs.kind == ALLNEXT && pre_rest);
    if (!pre && graded) {
        ck.record(id + " " + tag + ": " + std::string(mc4 ? "" : "MC4's address-order clause ") + (mc5 ? "" : "MC5's table clauses ") +
                  "failed (the lines above); " + id + " is graded and reported next to that verdict");
    } else if (!pre) {
        ck.record(id + " " + tag + ": precondition failed (" + std::string(ref.ok ? "" : "MC3 ") + (mc1 ? "" : "MC1 ") + (mc2 ? "" : "MC2 ") +
                  (mc4 ? "" : "MC4 ") + (mc5 ? "" : "MC5 ") + (mc6 ? "" : "MC6 ") + "), not graded; the counts below are information only");
    }
    if (!ow) return;
    const int Np = (int)std::floor(double(window(rs.declP)) / double(rs.declP));
    Cls k = classify(c, ref, ref.right, t_start, rs.declP, tk, Np);
    ck.note(id + " " + tag + ": " + S(Np) + " sample periods, classified " + S(k.n) + ": next " + S(k.next) + ", previous sample again (reuse) " + S(k.reuse) + ", both buffered samples again (repeat) " +
            S(k.repeat) + ", skip " + S(k.skip) + ", unmapped " + S(k.unm) + ", after an unmapped period " + S(k.after_unm) + ", other " + S(k.other));
    if (!k.ok) { ck.record(id + " " + tag + ": not graded, the window ran past the capture"); return; }
    if (!graded) return;
    const std::string win = " within W = " + S(window(rs.declP)) + " ticks (a stimulus choice)";
    if (rs.kind == ALLNEXT) {
        std::string sub = (!rs.declNtsc && rs.P == 123) ? " [MP3-123]" : "";
        ck.expect(k.next == k.n && k.n > 0, id + " " + tag + sub + " (p.141): every classified sample period carries the next sample" + win);
    } else if (rs.declNtsc) {
        std::string sub = rs.P <= 113 ? " [MP2-lo]" : " [MP2-hi]";
        ck.expect(k.next < k.n, id + " " + tag + sub + " (a) (p.141): at least one sample period does not carry the next sample" + win);
        const bool again = k.reuse + k.repeat >= 1;
        ck.expect(again, id + " " + tag + sub + " (b) (p.141, p.164): at least one sample period plays a sample already played again where the next was due, "
                  "the previous sample again or both buffered samples again" + win);
        ck.expect(k.next < k.n && again, id + " " + tag + sub + " (a) and (b) (p.141; sol:5-p141.c10, opus:5-rate.c15): at least one sample period does not carry the next sample "
                  "and at least one sample period plays a sample already played again" + win);
    } else {
        ck.expect(k.next < k.n, id + " " + tag + " (p.141): at least one sample period does not carry the next sample" + win);
        ck.note(id + " " + tag + ": previous sample again (reuse) " + S(k.reuse) + ", both buffered samples again (repeat) " + S(k.repeat) + " (PAL reuse and repeat are reported, not graded)");
    }
}

// --------------------------------------------------------------- sections

Ref do_ref(Checks& ck, const std::string& dir, bool ntsc, const std::string& prog = "prog_minper", int P = kPref,
           const std::string& tagsuffix = "") {
    const bool cfgNtsc = ntsc;
    const std::string tag = std::string(ntsc ? "NTSC" : "PAL") + " reference P=" + S(kPref) + tagsuffix;
    Cap c = capture(dir, prog, cfgNtsc, P, nrun(std::max(P, kPref)));
    bool base = c.loaded && c.done;
    ck.expect(base, "FIXTURE " + tag + ": " + prog + " reached DONE [not graded -- fixture, no claim uid]");
    Ref r;
    if (!base) { ck.record(tag + ": the run did not complete; MC3 not graded"); return r; }
    ck.expect(c.kfix == kFix, "FIXTURE-CONST " + tag + ": the program-stored constant reads back [not graded -- fixture, no claim uid]");
    if (c.kfix != kFix) { ck.record(tag + ": the fixture constant did not read back; MC3 not graded"); return r; }
    return eval_ref(ck, c, ntsc, kPref, tag);
}

const int kShi[] = {124, 125, 126, 127, 128, 129, 130, 131, 140, 160, 200, 256, 320, 428};

void records(Checks& ck, bool ntsc) {
    const std::string sn = ntsc ? "NTSC" : "PAL";
    const std::string gaps = "132-139, 141-159, 161-199, 201-255, 257-319, 321-427 and 429-65535";
    if (ntsc) {
        ck.record("p.141 (sol:5-p141.c9, opus:5-rate.c14: 'Therefore, a value of at least 124 must be written into the period register to assure that the audio system DMA will be able to retrieve the next data sample'): "
                  "withdrawn; MP1 grades the fourteen values of S_hi, and the periods " + gaps + " inside 'at least 124' are not tested, so the interval is not established; MP1 is graded without these ids");
        ck.record("p.141 (sol:5-p141.c4, opus:5-rate.c8: 'In order to save buffers, the hardware is designed to handle 28,867 samples/second'): withdrawn; no output shows the reason, "
                  "and no one assertion grades the design maximum on both sides of the threshold; MP1 and MP2 grade the periods at and below it without these ids");
        ck.record("p.141 (sol:5-p141.c3, opus:5-rate.c6: 'The figure of 31,469 is a theoretical maximum', '2 samples/line * 262.5 lines/frame * 59.94 frames/second = 31,469 samples/second'): withdrawn; "
                  "MP5 grades only that no channel holds more than one data fetch in a line, without these ids; the figure itself is not graded: the runs measure 31,538 samples/second on lines of 227 ticks "
                  "(MEASURED in the MP5 lines), because the NTSC lines of the runs are all 227 ticks and the 228-tick lines of p.23, which make the 227.5-tick mean of the 262.5 lines, do not occur (NM-HRM-010)");
        ck.record("p.141 (how often the previous sample is reused below the minimum): the page does not say; only 'at least once' is graded, the counts are reported");
        ck.record("p.141 (what happens to the sample that arrives too late): the page does not say; skips are counted, not graded");
        ck.record("p.141 (the PAL figure of the theoretical maximum): the page gives none; MP5 is NTSC only");
        ck.record("MP5: unfalsifiable at this boundary; p.141 allocates one DMA slot per line to each channel and no stimulus the pages permit makes one channel take a second slot in a line, "
                  "so no contrast can violate a channel's bound; the control run (NMH_MUTATE=MP5CH1) shows that the count is per channel: two channels on one table stay at one read per line each");
    } else {
        ck.record("p.141 (sol:5-p141.c12, opus:5-rate.c18: 'Thus, for PAL systems, a value of at least 123 ticks/sample must be written into the period register'): withdrawn; "
                  "MP4 grades every period 1-122, MP3 grades 123 and S_hi, and the periods " + gaps + " inside 'at least 123' are not tested, so the interval is not established; MP3 is graded without these ids");
        ck.record("p.141 (sol:5-p141.c11, opus:5-rate.c17: '28,867 samples/second is also the maximum sampling rate for PAL systems'): withdrawn; "
                  "the rates above the maximum are graded at every period 1-122 (MP4), the rates at and below it only at 123 and S_hi (MP3); MP3 and MP4 are graded without these ids");
        ck.record("p.141 (PAL): the page prints the limit in the two sentences '28,867 samples/second is also the maximum sampling rate for PAL systems.' and "
                  "'Thus, for PAL systems, a value of at least 123 ticks/sample must be written into the period register.'; the consequence 'the previous sample will be reused' is printed in the NTSC paragraph only, "
                  "so MP4 grades that not every next sample is retrieved in time and the PAL reuse count is MEASURED");
    }
    ck.record("p.164 (the mechanism, not graded): 'DMA requests and restart requests are transferred to Agnus once each horizontal line, and the data comes back about 14 clock cycles later' explains what is measured "
              "below the minimum; it prints no bound that predicts a word in time at " + std::string(ntsc ? "NTSC periods 120-123" : "PAL periods 120-122") + ", so p.141's statement for those periods is graded");
    ck.record(sn + " period 0: the pages do not say what the value 0 does (p.140 'the maximum is 65535', p.159 'its full range 124-65536'); excluded");
}

void run_standard(Checks& ck, const std::string& dir, bool ntsc) {
    records(ck, ntsc);
    Ref ref = do_ref(ck, dir, ntsc);
    std::vector<int> ps;
    if (ntsc) { for (int p = 1; p <= 123; ++p) ps.push_back(p); }
    else      { for (int p = 1; p <= 122; ++p) ps.push_back(p); ps.push_back(123); }
    for (int p : kShi) ps.push_back(p);
    std::vector<Mp6> rows;
    for (int p : ps) {
        RunSpec rs;
        rs.declNtsc = rs.cfgNtsc = ntsc;
        rs.P = rs.declP = p;
        bool hi = ntsc ? p >= 124 : p >= 123;
        rs.kind = hi ? ALLNEXT : REUSE;
        rs.mp5 = ntsc && p <= 113;
        rs.mp6out = &rows;
        run_one(ck, dir, rs, ref);
    }
    mp6_summary(ck, ntsc ? "NTSC" : "PAL", rows);
}

// One contrasting case. Returns false when mv names no selector of this file.
bool run_mutation(Checks& ck, const std::string& dir, const std::string& mv) {
    auto spec = [](bool ntsc, int P, Kind k) { RunSpec r; r.declNtsc = r.cfgNtsc = ntsc; r.P = r.declP = P; r.kind = k; return r; };
    if (mv == "MP1") {
        Ref ref = do_ref(ck, dir, true);
        for (int p : {119, 64}) run_one(ck, dir, spec(true, p, ALLNEXT), ref);
    } else if (mv == "MP2") {
        Ref ref = do_ref(ck, dir, true);
        for (int p : {124, 64}) run_one(ck, dir, spec(true, p, REUSE), ref);
    } else if (mv == "MP3") {
        Ref ref = do_ref(ck, dir, false);
        for (int p : {119, 64}) run_one(ck, dir, spec(false, p, ALLNEXT), ref);
    } else if (mv == "MP4") {
        Ref ref = do_ref(ck, dir, false);
        for (int p : {124, 64}) run_one(ck, dir, spec(false, p, REUSE), ref);
    } else if (mv == "MP5CH1") {
        Ref ref = do_ref(ck, dir, true);
        RunSpec r = spec(true, 64, NONE);
        r.prog = "prog_minper_mp5ch1"; r.declDma = 0x0203; r.nch = 2; r.mp5 = true;
        run_one(ck, dir, r, ref);
    } else if (mv == "MC1STD") {
        Ref rn = do_ref(ck, dir, true), rp = do_ref(ck, dir, false);
        RunSpec a = spec(true, 124, ALLNEXT); a.cfgNtsc = false; a.tag = "NTSC declared, harness PAL, P=124";
        run_one(ck, dir, a, rn);
        RunSpec b = spec(false, 124, ALLNEXT); b.cfgNtsc = true; b.tag = "PAL declared, harness NTSC, P=124";
        run_one(ck, dir, b, rp);
    } else if (mv == "MC2DMA" || mv == "MC2ADK" || mv == "MC4LEN" || mv == "MC4CPU" || mv == "MC5MARK" || mv == "MC5STOP" || mv == "FIXMP") {
        Ref ref = do_ref(ck, dir, false);
        RunSpec r = spec(false, 124, ALLNEXT);
        std::string p = "prog_minper_";
        for (char ch : mv) p += (char)std::tolower(ch);
        r.prog = mv == "FIXMP" ? "prog_minper_fixmp" : p;
        if (mv == "MC5STOP") { r.P = r.declP = 64; r.kind = REUSE; }
        run_one(ck, dir, r, ref);
    } else if (mv == "FIXDONE") {
        Ref ref = do_ref(ck, dir, false);
        RunSpec r = spec(false, 124, ALLNEXT);
        r.prog = "prog_nodone";
        run_one(ck, dir, r, ref);
    } else if (mv == "MC3LEN" || mv == "MC3OTH" || mv == "MC3A" || mv == "MC3B" || mv == "MC3F4") {
        std::string p = "prog_minper_";
        for (char ch : mv) p += (char)std::tolower(ch);
        do_ref(ck, dir, false, p, kPref, " (" + mv + ")");
        do_ref(ck, dir, true, p, kPref, " (" + mv + ")");
    } else if (mv == "MC3TIME") {
        do_ref(ck, dir, false, "prog_minper", 448, " (AUD0PER 448, declared 447)");
        do_ref(ck, dir, true, "prog_minper", 448, " (AUD0PER 448, declared 447)");
    } else if (mv == "MP6GREEN") {
        for (bool ntsc : {true, false}) {
            Ref ref = do_ref(ck, dir, ntsc);
            std::vector<Mp6> rows;
            for (int p : {64, 124}) {
                RunSpec r = spec(ntsc, p, NONE);
                r.mp6out = &rows;
                run_one(ck, dir, r, ref);
            }
            mp6_summary(ck, ntsc ? "NTSC" : "PAL", rows);
        }
    } else if (mv == "MP6LEN") {
        for (bool ntsc : {true, false}) {
            Ref ref = do_ref(ck, dir, ntsc);
            std::vector<Mp6> rows;
            RunSpec r = spec(ntsc, 64, NONE);
            r.prog = "prog_minper_mp6len"; r.mp6out = &rows;
            run_one(ck, dir, r, ref);
            mp6_summary(ck, ntsc ? "NTSC" : "PAL", rows);
        }
    } else if (mv == "MC6BELOW") {
        Ref ref = do_ref(ck, dir, false);
        RunSpec r = spec(false, 101, REUSE); r.declP = 100;
        r.tag = "PAL AUD0PER 101, declared 100";
        run_one(ck, dir, r, ref);
    } else if (mv == "MC6PER") {
        Ref ref = do_ref(ck, dir, false);
        RunSpec r = spec(false, 129, ALLNEXT); r.declP = 128;
        r.tag = "PAL AUD0PER 129, declared 128";
        run_one(ck, dir, r, ref);
    } else {
        return false;
    }
    return true;
}

} // namespace minper
