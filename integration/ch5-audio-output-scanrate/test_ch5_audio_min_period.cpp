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
//   MP1 MP2 MP3 MP4 MP5CH1 MC1STD MC2DMA MC2ADK MC3VOL MC3TIME MC4LEN MC4CPU
//   MC5MARK MC5STOP MC6PER FIXMP
// A selector changes only the stimulus (the period value, the harness standard,
// the program); the comparator of the line it turns red is the normal one.
//
// Every selector below reached DONE on both cores; each MUTATION line states
// the result measured on fx68k and on TG68K (identical on both unless said).
//
// MUTATION MP1 stimulus red NMH_MUTATE=MP1: the NTSC MP1 comparator run at P = 123 and P = 64 in place of the S_hi values
//   measured: fx68k and TG68K: P = 123 stays green (every sample period is next); P = 64 red (next 528 of 936 classified,
//   reuse 204, repeat 204); 21 passed, 1 failed
// MUTATION MP2 stimulus red NMH_MUTATE=MP2: the NTSC MP2 comparator run at P = 124 and P = 64
//   measured: fx68k and TG68K: P = 124 red in (a) and (b) (next 482 of 482, no reuse); P = 64 green; 23 passed, 2 failed
// MUTATION MP3 stimulus red NMH_MUTATE=MP3: the PAL MP3 comparator run at P = 122 and P = 64
//   measured: fx68k and TG68K: P = 122 stays green (every sample period is next); P = 64 red (next 528 of 936, reuse 204); 21 passed, 1 failed
// MUTATION MP4 stimulus red NMH_MUTATE=MP4: the PAL MP4 comparator run at P = 124 and P = 64
//   measured: fx68k and TG68K: P = 124 red in (a) (next 482 of 482); P = 64 green (next 528 of 936); 22 passed, 1 failed
// MUTATION MP5 stimulus red NMH_MUTATE=MP5CH1: NTSC P = 64 with channel 1 also set up on the same table, period 64, DMACON $8203
//   measured: fx68k and TG68K: MP5 red (most table reads in one line 2; MC1, MC2, MC4 with two slot offsets, MC5 green); 14 passed, 1 failed
// MUTATION MC1 stimulus red NMH_MUTATE=MC1STD: P = 124 declared NTSC with the harness PAL, and declared PAL with the harness NTSC
//   measured: fx68k and TG68K: MC1 red in both (R+4 = 312 and 262); the other preconditions of both runs green; 28 passed, 2 failed
// MUTATION MC2 stimulus red NMH_MUTATE=MC2DMA: PAL P = 124 with channel 3 on a second table and DMACON $8209
//   measured: fx68k and TG68K: MC2 red (DMACONR bits 9-0 = 521); MC1, MC4, MC5, MC6 green; the proposition not graded; 14 passed, 1 failed
// MUTATION MC2 stimulus red NMH_MUTATE=MC2ADK: PAL P = 124 with ADKCON $8080 written after step 3
//   measured: fx68k and TG68K: MC2 red (ADKCONR bits 7-0 = 128); MC1, MC4, MC5, MC6 green; 14 passed, 1 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3VOL: both reference runs with AUD0VOL <- 0
//   measured: fx68k and TG68K: MC3 item 2 red in PAL and NTSC (no level change in the run); items 3 and 4 not graded; 6 passed, 2 failed
// MUTATION MC3 stimulus red NMH_MUTATE=MC3TIME: both reference runs with AUD0PER 894 and the declaration left at 447
//   measured: fx68k and TG68K: MC3 anchor F1 (c) red in PAL and NTSC (tick 16 cycles, lines of 113.5 ticks); items 1, 2 and anchors (a), (b) green; 14 passed, 2 failed
// MUTATION MC4 stimulus red NMH_MUTATE=MC4LEN: PAL P = 124 with AUD0LEN 65 and the bytes 0 and 2 at T+128
//   measured: fx68k and TG68K: MC4 red (fetches off the table); MC1, MC2, MC5, MC6 green; 14 passed, 1 failed
// MUTATION MC4 stimulus red NMH_MUTATE=MC4CPU: PAL P = 124 with one CPU read of the word at T+64 in the middle of the wait
//   measured: fx68k and TG68K: MC4 red (a read off the channel offset); MC1, MC2, MC5, MC6 green; 14 passed, 1 failed
// MUTATION MC5 stimulus red NMH_MUTATE=MC5MARK: PAL P = 124 without the done-marker store
//   measured: fx68k and TG68K: MC5 red (marker absent); MC1, MC2, MC4, MC6 green; 14 passed, 1 failed
// MUTATION MC5 stimulus red NMH_MUTATE=MC5STOP: PAL P = 64 with DMACON <- $0001 after 100 lines of the wait
//   measured: fx68k and TG68K: MC5 red (no fetch in the first and last tenth, last fetch before the output window ends); MC4 red as well
//   (no wrap in the port window); MC1, MC2 green; 12 passed, 2 failed
// MUTATION MC6 stimulus red NMH_MUTATE=MC6PER: PAL with AUD0PER 129 written while 128 is declared
//   measured: fx68k and TG68K: MC6 red (largest distance from the 128-tick grid 64 ticks); MC1, MC2, MC4, MC5 green; 14 passed, 1 failed
// MUTATION FIXTURE-CONST stimulus red NMH_MUTATE=FIXMP: PAL P = 124 with the fixture constant $DEAD in place of $BEEF
//   measured: fx68k and TG68K: FIXTURE-CONST red; behind it MC1-MC6 and the proposition not graded (RECORDED); 9 passed, 1 failed

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
inline int window(int P)  { return std::max(60000, 256 * P); }
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
        ck.record("MC3 " + tag + ": items 1, 3 and 4 not graded, the run holds no table fetch");
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
        ck.record("MC3 " + tag + ": items 3 and 4 not graded, no level cycle appears");
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
    if (enough) {
        std::set<uint16_t> dist;
        for (size_t i = 0; i < 256; ++i) dist.insert(chg[i0 + i].v);
        bool per = dist.size() == 128;
        for (size_t i = 0; per && i + 128 < 256; ++i) if (chg[i0 + i].v != chg[i0 + i + 128].v) per = false;
        std::set<uint16_t> first128;
        for (size_t i = 0; i < 128; ++i) first128.insert(chg[i0 + i].v);
        item2 = per && first128.size() == 128;
        for (size_t i = 0; i < 128; ++i) r.cyc.push_back(chg[i0 + i].v);
    }
    ck.expect(item1, "MC3 " + tag + " item 1: the output that does not carry channel 0 is constant throughout the window");
    ck.expect(item2, "MC3 " + tag + " item 2: the channel-0 output shows exactly 128 distinct levels, repeating every 128 level runs, each level once per period (p.140)");
    if (!item2) {
        ck.record("MC3 " + tag + ": items 3 and 4 not graded, no level cycle appears");
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
            double rel = std::fabs(mean - 2.0 * r.D) / (2.0 * r.D);
            b_ok = rel <= 0.005;
            ck.note("MC3 " + tag + ": mean spacing of table fetches over " + S((long)cnt) + " fetches = " + F(mean) + " cycles, 2 * D_ref = " +
                    F(2.0 * r.D) + " cycles, difference " + F(rel * 100.0, 3) + " %");
        } else {
            ck.note("MC3 " + tag + ": the port window holds " + S((long)wraps.size()) + " wraps, fewer than the 2 that give a whole pass");
        }
    }
    ck.expect(b_ok, "MC3 " + tag + " anchor F1 (b): the mean spacing of consecutive table fetches over whole passes is 2 * D_ref to within 0.5 % (p.138, p.139, p.165)");
    bool c_ok = false;
    std::vector<double> ll;
    if (pw.ok) {
        for (size_t i = 0; i + 1 < L.start.size(); ++i)
            if (L.start[i] >= pw.pw0 && L.start[i + 1] <= pw.pw1) ll.push_back(double(L.start[i + 1] - L.start[i]) / r.tickc);
    }
    if (!ll.empty()) {
        double m = 0; for (double x : ll) m += x; m /= double(ll.size());
        c_ok = ntsc ? (m >= 226.8 && m <= 228.2) : (m >= 226.8 && m <= 227.7);
        size_t n227 = 0, n228 = 0, nalt = 0;
        for (size_t i = 0; i < ll.size(); ++i) {
            if (std::fabs(ll[i] - 227.0) <= 0.2) ++n227;
            else if (std::fabs(ll[i] - 228.0) <= 0.2) ++n228;
            if (i > 0 && std::fabs(ll[i] - ll[i - 1]) >= 0.5) ++nalt;
        }
        ck.note("MC3 " + tag + ": " + S((long)ll.size()) + " complete lines in the port window, mean length " + F(m) + " ticks; lines of 227 ticks " + S((long)n227) +
                ", of 228 ticks " + S((long)n228) + ", lines whose length differs from the line before " + S((long)nalt) + " (the alternation is reported, not graded); "
                "refresh clusters checked " + S((long)L.groups) + ", not four accesses two slots apart " + S((long)L.bad) + "; reads below word $1000 from the first fetch on: " +
                S((long)low_reads(c, f0)) + " (cross-check only)");
    }
    ck.expect(c_ok, "MC3 " + tag + " anchor F1 (c): the mean line length is " + std::string(ntsc ? "between 226.8 and 228.2 ticks (p.23, p.194)" : "between 226.8 and 227.7 ticks (p.23, p.304)"));
    ck.expect(r.tickc > 0, "MC3 " + tag + " item 4: the output resolution s is known");
    r.cal = true;
    r.ok = item1 && item2 && a_ok && b_ok && c_ok;
    return r;
}

// ------------------------------------------------------------------- runs

enum Kind { NONE = 0, ALLNEXT = 1, REUSE = 2 };

struct RunSpec {
    bool declNtsc = true, cfgNtsc = true;
    int P = 128, declP = 128;
    std::string prog = "prog_minper";
    unsigned declDma = 0x0201;
    int nch = 1;
    Kind kind = NONE;
    bool mp5 = false;
    bool stop_early = false;
    std::string tag;
};

struct Cls { int n = 0, next = 0, reuse = 0, repeat = 0, skip = 0, unm = 0, other = 0; bool ok = false; };

Cls classify(const Cap& c, const Ref& r, bool right, uint64_t t_start, int P, double tickc, int N) {
    Cls k;
    long m = -1;
    bool have = false;
    for (int j = 0; j < N; ++j) {
        size_t at = (size_t)(double(t_start) + (double(j) + 0.5) * double(P) * tickc);
        if (at >= c.audio.size()) return k;
        auto it = r.idx.find(side_val(c.audio[at], right));
        int cj = it == r.idx.end() ? -1 : it->second;
        if (!have) {                          // c_1 is not classified; an unmapped one is counted
            if (cj >= 0) { m = cj; have = true; }
            else { ++k.n; ++k.unm; }
            continue;
        }
        ++k.n;
        if (cj < 0) { ++k.unm; continue; }
        int d = ((cj - (int)m) % 128 + 128) % 128;
        if (d == 1) { ++k.next; m = cj; }
        else if (d == 0) ++k.reuse;
        else if (d == 127) ++k.repeat;
        else if (d >= 2 && d <= 63) { ++k.skip; m = cj; }
        else ++k.other;
    }
    k.ok = true;
    return k;
}

void run_one(Checks& ck, const std::string& dir, const RunSpec& rs, const Ref& ref) {
    const std::string sn = rs.declNtsc ? "NTSC" : "PAL";
    const std::string tag = rs.tag.empty() ? sn + " P=" + S(rs.P) : rs.tag;
    const int N = nrun(rs.declP);
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
    bool mc4 = false;
    std::string mc4why;
    std::vector<uint64_t> fe;                 // log indices of reads at the channel offsets, in the port window
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
        std::set<long> allowed;
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
        mc4 = viol_off == 0 && (below || viol_order == 0) && nwraps >= 1 && cpu_writes == 0 && L.bad == 0 && L.groups > 0 && !allowed.empty();
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
    bool mc5 = false;
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
                if (a.is_write || !in_t(a.word_addr)) continue;
                if (a.cycle >= a0 && a.cycle < a1) f = true;
                if (a.cycle >= b0 && a.cycle < b1) l = true;
            }
            tenths = f && l;
        }
        lastafter = ow ? pw.last_fetch > t_end : (pw.ok && pw.last_fetch > pw.pw1);
        mc5 = marker && lines && tenths && lastafter;
        ck.note("MC5 " + tag + ": marker " + std::string(marker ? "present" : "absent") + ", R+6 = " + S(c.rlines) + " (N_run " + S(N) +
                "), fetches in the first and last tenth of " + S((long)wl.size()) + " window lines: " + (tenths ? "yes" : "no") +
                ", last fetch after the output window: " + (lastafter ? "yes" : "no"));
    }
    ck.expect(mc5, "MC5 " + tag + ": the run is complete: marker $A5C3, " + S(N) + " lines counted, table fetches in the first and last tenth of the port window, "
                   "the last fetch after the output window");

    // ---- MP5 (port based, MC1-MC5)
    if (rs.mp5) {
        const std::string ids = "sol:5-p141.c3, opus:5-rate.c6";
        long maxl = 0, nl = 0, tot = 0;
        if (pw.ok) {
            for (size_t i = 0; i + 1 < L.start.size(); ++i) {
                if (!(L.start[i] >= pw.pw0 && L.start[i + 1] <= pw.pw1)) continue;
                long cnt = 0;
                for (const auto& a : c.log)
                    if (!a.is_write && in_t(a.word_addr) && a.cycle >= L.start[i] && a.cycle < L.start[i + 1]) ++cnt;
                maxl = std::max(maxl, cnt); ++nl; tot += cnt;
            }
        }
        double secs = pw.ok ? double(pw.pw1 - pw.pw0) / tk * (rs.declNtsc ? 0.279365e-6 : 0.281937e-6) : 1.0;
        ck.note("MP5 " + tag + ": " + S(nl) + " complete lines, most table reads in one line " + S(maxl) + ", " + S(tot) +
                " reads in the window, 2 * reads / window length = " + F(2.0 * double(tot) / secs, 0) + " samples/second (reported, not graded)");
        if (!(ref.ok && mc1 && mc2 && mc4 && mc5)) {
            ck.record("MP5 " + tag + ": precondition failed (" + std::string(ref.ok ? "" : "MC3 ") + (mc1 ? "" : "MC1 ") + (mc2 ? "" : "MC2 ") +
                      (mc4 ? "" : "MC4 ") + (mc5 ? "" : "MC5 ") + "), not graded; the counts above are information only");
        } else {
            ck.expect(maxl <= 1 && nl > 0, "MP5 " + tag + " (p.141; " + ids + "): no line holds more than one data fetch of the channel");
        }
    }

    if (rs.kind == NONE) return;
    const std::string id = rs.declNtsc ? (rs.kind == ALLNEXT ? "MP1" : "MP2") : (rs.kind == ALLNEXT ? "MP3" : "MP4");
    // ---- MC6
    bool mc6 = false;
    double mc6dmax = 0;
    double s = ref.s;
    if (ow) {
        bool res = s <= double(rs.declP) / 2.0;
        double dmax = 0; long n = 0;
        for (const auto& x : chg) {
            if (x.t < t_start || x.t >= t_end) continue;
            if (rs.stop_early && x.t > pw.last_fetch) continue;
            double xt = double(x.t - t_start) / tk;
            double r = std::fmod(xt, double(rs.declP));
            dmax = std::max(dmax, std::min(r, double(rs.declP) - r));
            ++n;
        }
        mc6 = res && dmax <= s + 1e-9;
        mc6dmax = dmax;
        ck.note("MC6 " + tag + ": s = " + F(s, 4) + " ticks (P/2 = " + F(double(rs.declP) / 2.0, 1) + "), " + S(n) + " level changes in the window, largest distance from the grid " +
                F(dmax, 4) + " ticks");
    }
    if (below) {
        ck.note("MC6 " + tag + ": below the minimum the P grid is a result, not a precondition; s <= P/2: " + std::string(s <= double(rs.declP) / 2.0 ? "yes" : "no") +
                ", largest distance of a level change from the grid " + F(mc6dmax, 4) + " ticks");
        mc6 = ow;
    } else {
        ck.expect(mc6, "MC6 " + tag + ": the period is in effect and the output resolved: s <= P/2 and every level change lies within s of the P grid (p.140, p.164)");
    }
    const bool pre = ref.ok && mc1 && mc2 && mc4 && mc5 && mc6;
    if (!pre) {
        ck.record(id + " " + tag + ": precondition failed (" + std::string(ref.ok ? "" : "MC3 ") + (mc1 ? "" : "MC1 ") + (mc2 ? "" : "MC2 ") +
                  (mc4 ? "" : "MC4 ") + (mc5 ? "" : "MC5 ") + (mc6 ? "" : "MC6 ") + "), not graded; the counts below are information only");
    }
    if (!ow) return;
    const int Np = (int)std::floor(double(window(rs.declP)) / double(rs.declP));
    Cls k = classify(c, ref, ref.right, t_start, rs.declP, tk, Np);
    ck.note(id + " " + tag + ": " + S(Np) + " sample periods, classified " + S(k.n) + ": next " + S(k.next) + ", reuse " + S(k.reuse) + ", repeat (m-1) " +
            S(k.repeat) + ", skip " + S(k.skip) + ", unmapped " + S(k.unm) + ", other " + S(k.other));
    if (!k.ok) { ck.record(id + " " + tag + ": not graded, the window ran past the capture"); return; }
    if (!pre) return;
    const std::string win = " within W = " + S(window(rs.declP)) + " ticks (a stimulus choice)";
    if (rs.kind == ALLNEXT) {
        std::string ids = rs.declNtsc ? "sol:5-p141.c9, opus:5-rate.c14, sol:5-p141.c4, opus:5-rate.c8"
                                      : "sol:5-p141.c12, opus:5-rate.c18, sol:5-p141.c11, opus:5-rate.c17";
        std::string sub = (!rs.declNtsc && rs.P == 123) ? " [MP3-123]" : "";
        ck.expect(k.next == k.n && k.n > 0, id + " " + tag + sub + " (p.141; " + ids + "): every classified sample period carries the next sample" + win);
    } else if (rs.declNtsc) {
        std::string ids_a = "sol:5-p141.c10, opus:5-rate.c15";
        std::string ids_b = "sol:5-p141.c10";
        std::string sub;
        if (rs.P <= 113) { sub = " [MP2-lo]"; ids_a += ", sol:5-p141.c3, opus:5-rate.c6"; ids_b += ", sol:5-p141.c3, opus:5-rate.c6"; }
        else if (rs.P <= 123) { sub = " [MP2-hi]"; ids_a += ", sol:5-p141.c4, opus:5-rate.c8"; ids_b += ", sol:5-p141.c4, opus:5-rate.c8"; }
        ck.expect(k.next < k.n, id + " " + tag + sub + " (a) (p.141; " + ids_a + "): at least one sample period does not carry the next sample" + win);
        ck.expect(k.reuse >= 1, id + " " + tag + sub + " (b) (p.141; " + ids_b + "): at least one sample period reuses the sample just played" + win);
    } else {
        ck.expect(k.next < k.n, id + " " + tag + " (a) (p.141; sol:5-p141.c11, opus:5-rate.c17, sol:5-p141.c12, opus:5-rate.c18): at least one sample period does not carry the next sample" + win);
        ck.note(id + " " + tag + " (b): reuse periods " + S(k.reuse) + " (PAL reuse is reported, not graded)");
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
    if (ntsc) {
        ck.record("p.141 (the reason clause 'In order to save buffers'): no output shows a reason; the rate of 28,867 samples/second is graded by MP1 and MP2-hi, the reason is not");
        ck.record("p.141 (31,469 as a rate that is reached): the page calls it a theoretical maximum and designs for 28,867; only the bound is graded (MP5), nothing checks that 31,469 is reached");
        ck.record("p.141 (periods 429 to 65535 inside 'at least 124'): covered by the proposition, not in the required set; graded on the tested values only");
        ck.record("p.141 (how often the previous sample is reused below the minimum): the page does not say; only 'at least once' is graded, the counts are reported");
        ck.record("p.141 (what happens to the sample that arrives too late): the page does not say; skips are counted, not graded");
        ck.record("p.141 (the PAL figure of the theoretical maximum): the page gives none; MP5 is NTSC only");
    }
    ck.record(std::string(ntsc ? "NTSC" : "PAL") + " period 0: the pages do not say what the value 0 does (p.140 'the maximum is 65535', p.159 'its full range 124-65536'); excluded");
}

void run_standard(Checks& ck, const std::string& dir, bool ntsc) {
    records(ck, ntsc);
    Ref ref = do_ref(ck, dir, ntsc);
    std::vector<int> ps;
    if (ntsc) { for (int p = 1; p <= 123; ++p) ps.push_back(p); }
    else      { for (int p = 1; p <= 122; ++p) ps.push_back(p); ps.push_back(123); }
    for (int p : kShi) ps.push_back(p);
    for (int p : ps) {
        RunSpec rs;
        rs.declNtsc = rs.cfgNtsc = ntsc;
        rs.P = rs.declP = p;
        bool hi = ntsc ? p >= 124 : p >= 123;
        rs.kind = hi ? ALLNEXT : REUSE;
        rs.mp5 = ntsc && p <= 113;
        run_one(ck, dir, rs, ref);
    }
}

// One contrasting case. Returns false when mv names no selector of this file.
bool run_mutation(Checks& ck, const std::string& dir, const std::string& mv) {
    auto spec = [](bool ntsc, int P, Kind k) { RunSpec r; r.declNtsc = r.cfgNtsc = ntsc; r.P = r.declP = P; r.kind = k; return r; };
    if (mv == "MP1") {
        Ref ref = do_ref(ck, dir, true);
        for (int p : {123, 64}) run_one(ck, dir, spec(true, p, ALLNEXT), ref);
    } else if (mv == "MP2") {
        Ref ref = do_ref(ck, dir, true);
        for (int p : {124, 64}) run_one(ck, dir, spec(true, p, REUSE), ref);
    } else if (mv == "MP3") {
        Ref ref = do_ref(ck, dir, false);
        for (int p : {122, 64}) run_one(ck, dir, spec(false, p, ALLNEXT), ref);
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
        if (mv == "MC5STOP") { r.stop_early = true; r.P = r.declP = 64; }
        run_one(ck, dir, r, ref);
    } else if (mv == "MC3VOL") {
        do_ref(ck, dir, false, "prog_minper_mc3vol", kPref, " (AUD0VOL 0)");
        do_ref(ck, dir, true, "prog_minper_mc3vol", kPref, " (AUD0VOL 0)");
    } else if (mv == "MC3TIME") {
        do_ref(ck, dir, false, "prog_minper", 894, " (AUD0PER 894, declared 447)");
        do_ref(ck, dir, true, "prog_minper", 894, " (AUD0PER 894, declared 447)");
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
