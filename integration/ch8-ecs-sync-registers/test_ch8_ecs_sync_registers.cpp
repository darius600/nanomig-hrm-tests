// test_ch8_ecs_sync_registers.cpp -- ch8-ecs-sync-registers. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// NMH_MUTATE selects a mutated program or configuration in place of its
// normal counterpart, one precondition at a time; none is exercised by a
// plain `make test`. P1, P2, P3 and P4 each carry their own mutation
// already, unconditionally: the contrasting runs (H1c/H2c for P1/P2,
// SA0/SB0/SC0 for P3/P4) that SPECIFICATION.md's own Runs table lists --
// these are the page's contrasting case (VARBEAMEN=0 / VARHSYEN=0) and run
// on every `make test`, never behind NMH_MUTATE. NMH_MUTATE instead gates
// the *precondition* contrast runs, which SPECIFICATION.md lists
// separately (sec.5, "Precondition contrast runs") because they are not
// part of any proposition's own graded population.
//
// The fixture requirement ("a program-stored constant asserted first") is
// met by PC-MARK itself: MARK1 is a known literal ($A5A5) stored before
// anything else is read back, and every later check depends on having
// found it. A separate synthetic fixture would duplicate this.
//
// MUTATION PC-ID stimulus red harness set to OCS (chipset_config clears
//   the ECS bit): VPOSR's bits 14-8 read the original-chipset value
//   instead of the enhanced-Agnus PAL value, via NMH_MUTATE=X-OCS
// MUTATION PC-ID stimulus red harness set to NTSC (chipset_config sets the
//   NTSC bit): VPOSR's bits 14-8 read the enhanced-Agnus NTSC value
//   instead of the PAL value, via NMH_MUTATE=X-NTSC
// MUTATION PC-MARK stimulus red BRA.S * inserted before H1's HTOTAL write
//   (prog_h_h1_halt.s): neither MARK1 nor MARK2 is ever written, via
//   NMH_MUTATE=X-HALT-H
// MUTATION PC-MARK stimulus red BRA.S * inserted before SA's HSSTRT write
//   (prog_s_sa_halt.s): MARK1 is never written, via NMH_MUTATE=X-HALT-S
// MUTATION P1-LINES stimulus red H1's sampling loop shortened to 1024
//   samples (prog_h_h1_short.s): fewer than 320 wraps appear in the graded
//   range, via NMH_MUTATE=X-SHORT
// MUTATION P2-LINES stimulus red the same shortened H1 run (the MARK1..
//   MARK2 window shrinks with it): fewer than 320 graded refresh periods,
//   via NMH_MUTATE=X-SHORT
// MUTATION P1-COVER stimulus red H1's sampling loop replaced with a
//   read-until-wrap-then-store-4 burst (prog_h_h1_burst.s): consecutive
//   graded sample stores are a whole line apart between bursts, via
//   NMH_MUTATE=X-BURST
// MUTATION P3/P4-LINES stimulus red SA with DMACON's SET write omitting
//   BPLEN (prog_s_sa_nobpl.s): no bitplane DMA, no white pixel, no line
//   carries the landmark, via NMH_MUTATE=X-NOBPL
// MUTATION A4 stimulus red the same NOBPL run reused through A4's own
//   comparator: zero transitions into white instead of exactly one, via
//   NMH_MUTATE=X-NOBPL
// MUTATION A1 stimulus red H1's own graded samples (VARBEAMEN=1, bounded
//   by N=$B0) substituted into anchor A1's unchanged comparator, which
//   expects the free-running $E2-ish range that only VARBEAMEN=0 (H1c)
//   produces, via NMH_MUTATE=A1
// MUTATION A3 stimulus red H1's own graded period (VARBEAMEN=1, T~N=$B0
//   counts) substituted into anchor A3's unchanged 227/228-CCK comparator,
//   via NMH_MUTATE=A3
//
// A2 (every graded period contains exactly 4 refresh accesses) is not
// exercised by a dedicated mutation: refresh's own slot count is fixed by
// p.194/p.285 and is not a function of any register this package writes,
// so no stimulus this builder controls is expected to change it --
// unfalsifiable at this boundary, by a scope decision.

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// One colour clock ("memory access cycle", p.194) is 8 clk_sys cycles:
// amiga_clk.v's clk7_cnt is a free-running 2-bit counter incrementing once
// per clk_sys cycle, and cck toggles exactly when clk7_cnt==2'b01
// (amiga_clk.v:33-35) -- once every 4 clk_sys cycles, so a full cck period
// (two toggles) is 8 clk_sys cycles. This ratio is itself checked at
// runtime by anchor A3 below (it must convert measured periods to 227 or
// 228, the page's own PAL line length).
constexpr uint64_t kCck = 8;

// ---- Program H addresses (prog_h.s) -----------------------------------
constexpr uint32_t kHMark1   = 0x0C0100;
constexpr uint32_t kHMark2   = 0x0C0102;
constexpr uint32_t kHId      = 0x0C0104;
constexpr uint32_t kHSamples = 0x0C4000;
constexpr size_t   kHSampleCount = 16384;

// ---- Program S addresses (prog_s.s) ------------------------------------
constexpr uint32_t kSMark1 = 0x0C0100;
constexpr uint32_t kSId    = 0x0C0102;

// Every program in this package is well under 512 bytes; PROGRAM_ORG is
// fixed at $080400 (nanomig.i). A read in this range is a CPU instruction
// fetch, not a DMA access.
constexpr uint32_t kProgLoW = 0x080400 / 2;
constexpr uint32_t kProgHiW = (0x080400 + 0x800) / 2;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kHRunBudget = 4'000'000;
constexpr uint64_t kSRunBudget = 4'000'000;

nmh::Config default_cfg() { return nmh::Config{}; }               // ECS, PAL (chipset_config=0x08)
nmh::Config ocs_cfg()     { nmh::Config c; c.chipset_config = 0x00; return c; }  // OCS, PAL
nmh::Config ntsc_cfg()    { nmh::Config c; c.chipset_config = 0x0A; return c; }  // ECS, NTSC

// PC-ID: p.299's enhanced-Agnus PAL chip-identification value, accepted
// under either radix reading of the manual's un-prefixed digits (page
// note 9).
bool pc_id_pal(uint16_t id) { uint16_t f = (id >> 8) & 0x7F; return f == 0x20 || f == 0x14; }

// ==== Program H capture ===================================================

struct HCapture {
    bool loaded = false;
    uint16_t id = 0;
    std::optional<uint64_t> mark1, mark2;
    std::vector<std::pair<uint64_t, uint16_t>> samples;  // (cycle, raw VHPOSR value), store order
    std::vector<nmh::RamAccess> log;
};

HCapture run_h(const std::string& bin, const nmh::Config& cfg) {
    HCapture c;
    nmh::NanomigHarness h(cfg);
    if (h.load_program(bin) == 0) return c;
    c.loaded = true;
    h.boot(kBootBudget);
    h.run_cycles(kHRunBudget);
    c.log = h.ram_log();
    if (auto w = h.peek_logical(kHId)) c.id = *w;

    const uint32_t mark1_w = kHMark1 / 2, mark2_w = kHMark2 / 2;
    const uint32_t samp_lo = kHSamples / 2, samp_hi = samp_lo + (uint32_t)kHSampleCount;
    for (const auto& a : c.log) {
        if (!a.is_write) continue;
        if (a.word_addr == mark1_w && a.data == 0xA5A5 && !c.mark1) { c.mark1 = a.cycle; continue; }
        if (a.word_addr == mark2_w && a.data == 0x5A5A && !c.mark2) { c.mark2 = a.cycle; continue; }
        if (a.word_addr >= samp_lo && a.word_addr < samp_hi) c.samples.push_back({a.cycle, a.data});
    }
    return c;
}

bool is_cpu_access_h(const nmh::RamAccess& a) {
    if (!a.is_write) return a.word_addr >= kProgLoW && a.word_addr < kProgHiW;
    const uint32_t mark1_w = kHMark1 / 2, mark2_w = kHMark2 / 2, id_w = kHId / 2;
    const uint32_t samp_lo = kHSamples / 2, samp_hi = samp_lo + (uint32_t)kHSampleCount;
    return a.word_addr == mark1_w || a.word_addr == mark2_w || a.word_addr == id_w ||
           (a.word_addr >= samp_lo && a.word_addr < samp_hi);
}

// Refresh groups: a maximal run of non-CPU RAM-port accesses whose
// neighbours are less than 32 CCK apart (SPECIFICATION.md P2 sec.3).
std::vector<std::vector<uint64_t>> refresh_groups(const std::vector<nmh::RamAccess>& log,
                                                    uint64_t lo, uint64_t hi) {
    // Deliberately not coalesced: coalesce() would also merge two genuinely
    // separate refresh accesses that share the same address (refresh always
    // reads chip address 0, gary.v/agnus.v's dma_ref branch), which this
    // anchor's per-group count depends on being distinct.
    std::vector<uint64_t> cycles;
    for (const auto& a : log)
        if (a.cycle > lo && a.cycle < hi && !is_cpu_access_h(a)) cycles.push_back(a.cycle);
    std::sort(cycles.begin(), cycles.end());
    std::vector<std::vector<uint64_t>> groups;
    for (auto c : cycles) {
        if (groups.empty() || c - groups.back().back() >= 32 * kCck) groups.push_back({c});
        else groups.back().push_back(c);
    }
    return groups;
}

// Result of P2's per-run analysis: graded periods (clk_sys cycles between
// consecutive graded groups' first access) and whether every graded group
// has exactly 4 members (anchor A2).
struct P2Data {
    bool ok = false;
    std::vector<uint64_t> periods;
    bool a2_all_four = false;
};

P2Data analyze_p2(const HCapture& c) {
    P2Data d;
    if (!c.loaded || !c.mark1 || !c.mark2) return d;
    auto groups = refresh_groups(c.log, *c.mark1, *c.mark2);
    // first group starting after MARK1:
    size_t idx0 = groups.size();
    for (size_t i = 0; i < groups.size(); ++i)
        if (groups[i].front() > *c.mark1) { idx0 = i; break; }
    if (idx0 == groups.size() || idx0 + 1 >= groups.size()) return d;
    size_t first = idx0 + 1;  // the *second* group that starts after MARK1
    size_t last = groups.size();
    for (size_t i = first; i < groups.size(); ++i)
        if (groups[i].front() < *c.mark2) last = i; else break;
    if (last == groups.size() || last <= first) return d;
    d.ok = true;
    d.a2_all_four = true;
    // groups[first..last-1] each start one graded period and their own members
    // populate it; groups[last] only marks where the last graded period ends
    // (SPECIFICATION.md P2 sec.3), so its own size is not itself graded.
    for (size_t i = first; i < last; ++i) if (groups[i].size() != 4) d.a2_all_four = false;
    for (size_t i = first; i < last; ++i) d.periods.push_back(groups[i + 1].front() - groups[i].front());
    return d;
}

// ==== Program S capture ==================================================

struct SCapture {
    bool loaded = false;
    uint16_t id = 0;
    std::optional<uint64_t> mark1;
    std::vector<nmh::VideoSample> video;
};

SCapture run_s(const std::string& bin, const nmh::Config& cfg) {
    SCapture c;
    nmh::NanomigHarness h(cfg);
    if (h.load_program(bin) == 0) return c;
    c.loaded = true;
    h.boot(kBootBudget);
    h.run_cycles(kSRunBudget);
    if (auto w = h.peek_logical(kSId)) c.id = *w;
    const uint32_t mark1_w = kSMark1 / 2;
    for (const auto& a : h.ram_log())
        if (a.is_write && a.word_addr == mark1_w && a.data == 0xA5A5) { c.mark1 = a.cycle; break; }
    c.video = h.video();
    return c;
}

bool is_white(const nmh::VideoSample& s) { return s.r == 15 && s.g == 15 && s.b == 15; }

// The graded field: the span (nmh::find_frames) between the second and
// third output VSYNC after MARK1 (SPECIFICATION.md P3/P4 sec.3).
std::optional<nmh::Span> graded_field(const std::vector<nmh::VideoSample>& v, uint64_t mark1) {
    auto frames = nmh::find_frames(v);
    size_t first = frames.size();
    for (size_t i = 0; i < frames.size(); ++i)
        if (frames[i].begin > mark1) { first = i; break; }
    if (first == frames.size() || first + 1 >= frames.size()) return std::nullopt;
    return frames[first + 1];
}

// The output line's own reference frame (SPECIFICATION.md P3 sec.7): "The
// output line runs from one HSYNC leading edge to the next." The pulse
// level is whichever level is held for less than half of the field (page
// wording, "less than half of the output line"; measured once over the
// whole graded field, since the polarity is a property of the run, not of
// one line). A leading edge is a transition INTO that level; for an
// active-low pulse that is a falling (1->0) transition, for an active-high
// pulse a rising (0->1) transition -- deliberately not assumed either way.
bool pulse_is_zero_over(const std::vector<nmh::VideoSample>& v, nmh::Span field) {
    size_t zeros = 0, ones = 0;
    for (size_t i = field.begin; i < field.end && i < v.size(); ++i) (v[i].hs ? ones : zeros)++;
    return zeros < ones;
}

std::vector<size_t> leading_edges(const std::vector<nmh::VideoSample>& v, nmh::Span field, bool pulse_is_zero) {
    std::vector<size_t> edges;
    size_t lo = field.begin > 0 ? field.begin - 1 : 0;
    size_t hi = std::min(field.end + 1, v.size());
    for (size_t i = std::max<size_t>(lo, 1); i < hi; ++i) {
        bool prev = v[i - 1].hs, cur = v[i].hs;
        if (pulse_is_zero ? (prev && !cur) : (!prev && cur)) edges.push_back(i);
    }
    return edges;
}

struct LineMetric {
    bool ok = false;
    int64_t dL = 0, dT = 0;
    int64_t white_run = -1;
    int into_white = 0, out_white = 0;
};

// One canonical output line: [L, nextL). T is the sole opposite-polarity
// transition strictly between them (anchor A4's "exactly one trailing
// edge"); M is the first white pixel after L (spec's own wording).
LineMetric measure_span(const std::vector<nmh::VideoSample>& v, size_t L, size_t nextL, bool pulse_is_zero) {
    LineMetric m;
    if (nextL <= L + 1 || nextL > v.size()) return m;
    size_t T = SIZE_MAX;
    int trailing_count = 0;
    for (size_t i = L + 1; i < nextL; ++i) {
        bool prev = v[i - 1].hs, cur = v[i].hs;
        bool trailing = pulse_is_zero ? (!prev && cur) : (prev && !cur);
        if (trailing) { if (T == SIZE_MAX) T = i; ++trailing_count; }
    }
    if (trailing_count != 1) return m;

    size_t M = SIZE_MAX, white_end = SIZE_MAX;
    for (size_t i = L + 1; i < nextL; ++i) {
        bool w = is_white(v[i]), pw = is_white(v[i - 1]);
        if (!pw && w) { m.into_white++; if (M == SIZE_MAX) M = i; }
        if (pw && !w) { m.out_white++; if (M != SIZE_MAX && white_end == SIZE_MAX) white_end = i; }
    }
    if (M == SIZE_MAX) return m;
    m.dL = (int64_t)M - (int64_t)L;
    m.dT = (int64_t)M - (int64_t)T;
    if (white_end != SIZE_MAX) m.white_run = (int64_t)(white_end - M);
    m.ok = true;
    return m;
}

// Graded lines of one S run, split into the two doubled-line parity
// classes (SPECIFICATION.md P3 sec.7, "Doubled lines").
struct GradedLines { bool ok = false; std::vector<LineMetric> cls[2]; size_t total = 0; };

GradedLines graded_lines_of(const SCapture& c) {
    GradedLines g;
    if (!c.loaded || !c.mark1) return g;
    auto field = graded_field(c.video, *c.mark1);
    if (!field) return g;
    bool pulse_is_zero = pulse_is_zero_over(c.video, *field);
    auto edges = leading_edges(c.video, *field, pulse_is_zero);
    size_t idx = 0;
    for (size_t i = 0; i + 1 < edges.size(); ++i) {
        size_t L = edges[i], nextL = edges[i + 1];
        if (L < field->begin || nextL > field->end) continue;
        bool has_white = false;
        for (size_t j = L; j < nextL; ++j) if (is_white(c.video[j])) { has_white = true; break; }
        if (!has_white) { ++idx; continue; }
        LineMetric m = measure_span(c.video, L, nextL, pulse_is_zero);
        if (m.ok) { g.cls[idx % 2].push_back(m); g.total++; }
        ++idx;
    }
    g.ok = true;
    return g;
}

std::string hex_word(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "%04X", v);
    return std::string(buf);
}

int64_t median_of(std::vector<int64_t> v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

bool consistent(const std::vector<int64_t>& v) {
    if (v.empty()) return false;
    auto mm = std::minmax_element(v.begin(), v.end());
    return (*mm.second - *mm.first) <= 2;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ---- Claims without a proposition (SPECIFICATION.md sec.1) ----------
    checks.record(
        "opus:8-appA-oos.c1-WITHDRAWN [p280]: HBSTOP -- no page states the "
        "condition under which the programmed position takes effect, or what "
        "HBLANK does to R/G/B; not testable at nanomig.v's ports.");
    checks.record(
        "opus:8-appA-oos.c2-WITHDRAWN [p280]: HBSTRT -- as c1.");
    checks.record(
        "opus:8-appA-oos.c3-WITHDRAWN [p280]: HCENTER -- no page names the "
        "enabling condition, the field or the VSYNC edge it positions; the "
        "effect lies in sub-line VSYNC timing the test sees only after the "
        "line doubler.");
    checks.record(
        "opus:8-display.c7-WITHDRAWN [p260]: \"analog RGB\" names a connector "
        "and a kind of monitor; the test sees digital R/G/B and separate H/V "
        "sync, never a connector, and the colour count follows from the "
        "harness's fixed 4-bit outputs, not from the chips.");

    // ======================================================================
    // Program H group: P1, P2. Runs H1, H2, H1c, H2c (SPECIFICATION.md
    // sec.5) always execute -- H1c/H2c are the page's own contrasting case
    // (VARBEAMEN=0) and are P1's and P2's own mutation.
    // ======================================================================

    HCapture h1  = run_h(dir + "/prog_h_h1.bin",  default_cfg());
    HCapture h2  = run_h(dir + "/prog_h_h2.bin",  default_cfg());
    HCapture h1c = run_h(dir + "/prog_h_h1c.bin", default_cfg());
    HCapture h2c = run_h(dir + "/prog_h_h2c.bin", default_cfg());

    // ---- PC-ID, PC-MARK: shared preconditions, every H-group run --------
    {
        struct Row { const char* label; const HCapture* c; bool needs_mark2; };
        Row rows[] = {{"H1", &h1, true}, {"H2", &h2, true}, {"H1c", &h1c, true}, {"H2c", &h2c, true}};
        for (auto& r : rows) {
            checks.expect(r.c->loaded && pc_id_pal(r.c->id),
                "PC-ID [" + std::string(r.label) + "] p.299: VPOSR bits 14-8 read the enhanced-Agnus "
                "PAL chip-identification value ($20 or 20 decimal)");
            bool mark_ok = r.c->loaded && r.c->mark1.has_value() &&
                           (!r.needs_mark2 || (r.c->mark2.has_value() && *r.c->mark2 > *r.c->mark1));
            checks.expect(mark_ok,
                "PC-MARK [" + std::string(r.label) + "]: MARK1 appears at the port, and MARK2 later");
        }
    }
    // ---- PC-ID mutations: X-OCS, X-NTSC (extra capture of H1 only) ------
    {
        HCapture cap = (mv == "X-OCS") ? run_h(dir + "/prog_h_h1.bin", ocs_cfg()) : h1;
        checks.expect(cap.loaded && pc_id_pal(cap.id),
            "PC-ID [H1, X-OCS] p.299: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
    }
    {
        HCapture cap = (mv == "X-NTSC") ? run_h(dir + "/prog_h_h1.bin", ntsc_cfg()) : h1;
        checks.expect(cap.loaded && pc_id_pal(cap.id),
            "PC-ID [H1, X-NTSC] p.299: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
    }
    // ---- PC-MARK mutation: X-HALT-H --------------------------------------
    {
        HCapture cap = (mv == "X-HALT-H") ? run_h(dir + "/prog_h_h1_halt.bin", default_cfg()) : h1;
        checks.expect(cap.loaded && cap.mark1.has_value() && cap.mark2.has_value(),
            "PC-MARK [H1, X-HALT-H]: MARK1 appears at the port, and MARK2 later");
    }

    // X-SHORT/X-BURST substitute H1's own capture for specific preconditions
    // (P1-LINES/P2-LINES and P1-COVER respectively); nothing else changes.
    HCapture h1_short, h1_burst;
    if (mv == "X-SHORT") h1_short = run_h(dir + "/prog_h_h1_short.bin", default_cfg());
    if (mv == "X-BURST") h1_burst = run_h(dir + "/prog_h_h1_burst.bin", default_cfg());

    struct Extracted {
        bool ok = false;
        std::vector<uint16_t> hf;
        std::vector<uint64_t> cyc;
        std::vector<size_t> wraps;
    };
    auto extract = [](const HCapture& c) {
        Extracted e;
        if (!c.loaded || c.samples.empty()) return e;
        e.hf.reserve(c.samples.size());
        e.cyc.reserve(c.samples.size());
        for (auto& s : c.samples) { e.hf.push_back(s.second & 0x00FF); e.cyc.push_back(s.first); }
        for (size_t i = 1; i < e.hf.size(); ++i) if (e.hf[i] < e.hf[i - 1]) e.wraps.push_back(i);
        e.ok = e.wraps.size() >= 2;
        return e;
    };

    // ---- P1: with VARBEAMEN set, no H readback exceeds HTOTAL ------------
    {
        struct Run { const char* label; HCapture* cap; uint16_t N; bool contrast; };
        Run runs[] = {{"H1", &h1, 0x00B0, false}, {"H2", &h2, 0x00C0, false},
                      {"H1c", &h1c, 0x00B0, true}, {"H2c", &h2c, 0x00C0, true}};
        for (auto& r : runs) {
            Extracted ex = extract(*r.cap);
            if (!ex.ok) {
                checks.expect(false, std::string("P1 [") + r.label + "] [p280] opus:8-appA-oos.c6: "
                    "fewer than two wraps observed, no graded range");
                continue;
            }
            size_t graded_start = ex.wraps[1];
            uint16_t maxH = 0;
            bool any_exceeds = false;
            for (size_t i = graded_start; i < ex.hf.size(); ++i) {
                maxH = std::max(maxH, ex.hf[i]);
                if (ex.hf[i] > r.N) any_exceeds = true;
            }
            checks.note(std::string("P1 [") + r.label + "]: max graded H=$" + hex_word(maxH) +
                        ", N=$" + hex_word(r.N));

            if (!r.contrast) {
                checks.expect(!any_exceeds,
                    "P1 [" + std::string(r.label) + "] [p280] opus:8-appA-oos.c6: while VARBEAMEN is 1, "
                    "no graded horizontal-position readback exceeds HTOTAL");
            } else {
                checks.expect(any_exceeds,
                    "P1 [" + std::string(r.label) + "] [p280] opus:8-appA-oos.c6 (contrast, VARBEAMEN=0): "
                    "at least one graded readback exceeds N, since HTOTAL's condition does not hold");
                if (std::string(r.label) == "H2c") {
                    bool a1_ok = maxH >= (0xE2 - 16) && maxH <= 0xE3;
                    checks.expect(a1_ok,
                        "A1 [H2c] [p23]: the largest graded H with VARBEAMEN=0 lies between $E2-16 and $E3");
                }
            }

            // P1-LINES for H2 only; H1's own (possibly X-SHORT-substituted) check follows.
            if (std::string(r.label) == "H2") {
                bool lines_ok = (ex.wraps.size() - 1) >= 320;
                checks.expect(lines_ok,
                    "P1-LINES [H2]: the graded samples contain at least 320 wraps");
            }
        }

        // Anchor A1 [H1c], substituted with H1's own (VARBEAMEN=1) samples under NMH_MUTATE=A1,
        // fed through this same, unchanged comparator.
        {
            Extracted ex = extract((mv == "A1") ? h1 : h1c);
            uint16_t maxH = 0;
            if (ex.ok) for (size_t i = ex.wraps[1]; i < ex.hf.size(); ++i) maxH = std::max(maxH, ex.hf[i]);
            bool a1_ok = ex.ok && maxH >= (0xE2 - 16) && maxH <= 0xE3;
            checks.expect(a1_ok,
                "A1 [H1c] [p23]: the largest graded H with VARBEAMEN=0 lies between $E2-16 and $E3");
        }

        // P1-LINES [H1], substituted with the shortened capture under X-SHORT.
        {
            Extracted ex = extract((mv == "X-SHORT") ? h1_short : h1);
            bool lines_ok = ex.ok && (ex.wraps.size() - 1) >= 320;
            checks.expect(lines_ok, "P1-LINES [H1]: the graded samples contain at least 320 wraps");
        }

        // P1-COVER (H1 only): consecutive graded sample stores never more than 16 CCK
        // apart, and at least one graded sample has H >= N-16. X-BURST substitutes H1's capture.
        {
            Extracted ex = extract((mv == "X-BURST") ? h1_burst : h1);
            bool cover_ok = false;
            uint64_t max_gap_cck = 0;
            if (ex.ok) {
                size_t gs = ex.wraps[1];
                for (size_t i = gs + 1; i < ex.cyc.size(); ++i)
                    max_gap_cck = std::max(max_gap_cck, (ex.cyc[i] - ex.cyc[i - 1]) / kCck);
                bool near_end = false;
                for (size_t i = gs; i < ex.hf.size(); ++i)
                    if (ex.hf[i] >= (uint16_t)(0x00B0 - 16)) near_end = true;
                cover_ok = (max_gap_cck <= 16) && near_end;
            }
            checks.note("P1-COVER [H1]: largest consecutive-sample gap in the graded range = " +
                        std::to_string(max_gap_cck) + " CCK");
            checks.expect(cover_ok,
                "P1-COVER [H1]: consecutive graded sample stores are never more than 16 CCK apart, "
                "and at least one graded sample has H >= N-16");
        }
    }

    // ---- P2: with VARBEAMEN set, every line lasts N or N+1 CCK -----------
    {
        P2Data d1 = analyze_p2(h1), d2 = analyze_p2(h2), d1c = analyze_p2(h1c), d2c = analyze_p2(h2c);
        P2Data d1_for_lines = (mv == "X-SHORT") ? analyze_p2(h1_short) : d1;

        auto all_equal = [](const std::vector<uint64_t>& v) {
            if (v.empty()) return false;
            for (auto p : v) if (p != v[0]) return false;
            return true;
        };

        struct PRow { const char* label; P2Data* d; };
        PRow prows[] = {{"H1", &d1}, {"H2", &d2}, {"H1c", &d1c}, {"H2c", &d2c}};
        for (auto& pr : prows) {
            if (pr.d->ok && !pr.d->periods.empty())
                checks.note(std::string("P2 [") + pr.label + "]: " + std::to_string(pr.d->periods.size()) +
                            " graded periods, first=" + std::to_string(pr.d->periods[0]) + " clk_sys cycles");
            checks.expect(pr.d->ok && !pr.d->periods.empty() && all_equal(pr.d->periods),
                "P2(a) [" + std::string(pr.label) + "] [p280,p304] opus:8-appA-oos.c6: while VARBEAMEN "
                "is 1, every graded line period has one and the same value");
            checks.expect(pr.d->ok && pr.d->a2_all_four,
                "A2 [" + std::string(pr.label) + "] [p194,p285]: every graded period contains exactly "
                "4 refresh accesses");
        }
        // P2-LINES [H1]: at least 320 graded periods. X-SHORT substitutes H1's capture.
        checks.expect(d1_for_lines.ok && d1_for_lines.periods.size() >= 320,
            "P2-LINES [H1]: at least 320 graded periods");
        checks.expect(d2.ok && d2.periods.size() >= 320,
            "P2-LINES [H2]: at least 320 graded periods");
        checks.record(
            "A2-NOT-EXERCISED: no dedicated mutation -- refresh's own 4-slot count (p.194,p.285) is "
            "fixed by the chipset's slot allocation, not by HTOTAL or BEAMCON0; unfalsifiable at this "
            "boundary, by a scope decision.");

        uint64_t T1_raw = d1.ok && !d1.periods.empty() ? d1.periods[0] : 0;
        uint64_t T2_raw = d2.ok && !d2.periods.empty() ? d2.periods[0] : 0;
        // Anchor A3 mutation (X-A3): substitute H1's own (VARBEAMEN=1) period into the
        // unchanged 227/228-CCK comparator meant for H1c/H2c.
        uint64_t T1c_raw = d1c.ok && !d1c.periods.empty() ? d1c.periods[0] : 0;
        uint64_t T2c_raw = d2c.ok && !d2c.periods.empty() ? d2c.periods[0] : 0;

        bool have_t1t2 = d1.ok && !d1.periods.empty() && d2.ok && !d2.periods.empty();
        checks.expect(have_t1t2 && (int64_t)(T2_raw - T1_raw) == 16 * (int64_t)kCck,
            "P2(b) [p280,p304] opus:8-appA-oos.c6: T2 - T1 = 16 CCK ($C0 - $B0)");

        uint64_t t1_cck = T1_raw / kCck;
        bool c_ok = have_t1t2 && (t1_cck >= 0x00B0) && (t1_cck - 0x00B0 <= 1);
        checks.expect(c_ok,
            "P2(c) [p280,p304] opus:8-appA-oos.c6: T1 (converted to CCK) - $B0 is 0 or 1 (page note 2)");

        bool have_contrast = d1c.ok && !d1c.periods.empty() && d2c.ok && !d2c.periods.empty();
        checks.expect(have_contrast && T1c_raw == T2c_raw,
            "P2 contrast [p280,p304] opus:8-appA-oos.c6 (VARBEAMEN=0): every graded period in H1c "
            "equals every graded period in H2c -- HTOTAL has no effect");

        // Anchor A3 (H2c): every graded period is 227 or 228 CCK.
        {
            uint64_t cck_v = T2c_raw / kCck;
            checks.expect(T2c_raw != 0 && (cck_v == 227 || cck_v == 228),
                "A3 [H2c] [p23,p194,p304]: every graded period converts to 227 or 228 CCK (VARBEAMEN=0)");
        }
        // Anchor A3 (H1c), substituted with H1's own (VARBEAMEN=1) period under NMH_MUTATE=A3.
        {
            uint64_t raw = (mv == "A3") ? T1_raw : T1c_raw;
            uint64_t cck_v = raw / kCck;
            checks.expect(raw != 0 && (cck_v == 227 || cck_v == 228),
                "A3 [H1c] [p23,p194,p304]: every graded period converts to 227 or 228 CCK (VARBEAMEN=0)");
        }
    }

    // ======================================================================
    // Program S group: P3, P4. Runs SA, SB, SC, SA0, SB0, SC0
    // (SPECIFICATION.md sec.5) always execute -- SA0/SB0/SC0 are the page's
    // own contrasting case (VARHSYEN=0) and are P3's and P4's own mutation.
    // ======================================================================

    SCapture sa  = run_s(dir + "/prog_s_sa.bin",  default_cfg());
    SCapture sb  = run_s(dir + "/prog_s_sb.bin",  default_cfg());
    SCapture sc  = run_s(dir + "/prog_s_sc.bin",  default_cfg());
    SCapture sa0 = run_s(dir + "/prog_s_sa0.bin", default_cfg());
    SCapture sb0 = run_s(dir + "/prog_s_sb0.bin", default_cfg());
    SCapture sc0 = run_s(dir + "/prog_s_sc0.bin", default_cfg());

    // ---- PC-ID, PC-MARK: shared preconditions, every S-group run --------
    {
        struct Row { const char* label; const SCapture* c; };
        Row rows[] = {{"SA", &sa}, {"SB", &sb}, {"SC", &sc}, {"SA0", &sa0}, {"SB0", &sb0}, {"SC0", &sc0}};
        for (auto& r : rows) {
            checks.expect(r.c->loaded && pc_id_pal(r.c->id),
                "PC-ID [" + std::string(r.label) + "] p.299: VPOSR bits 14-8 read the enhanced-Agnus "
                "PAL chip-identification value ($20 or 20 decimal)");
            checks.expect(r.c->loaded && r.c->mark1.has_value(),
                "PC-MARK [" + std::string(r.label) + "]: MARK1 appears at the port");
        }
    }
    // ---- PC-MARK mutation: X-HALT-S --------------------------------------
    {
        SCapture cap = (mv == "X-HALT-S") ? run_s(dir + "/prog_s_sa_halt.bin", default_cfg()) : sa;
        checks.expect(cap.loaded && cap.mark1.has_value(),
            "PC-MARK [SA, X-HALT-S]: MARK1 appears at the port");
    }

    // ---- Shared: graded lines per run, X-NOBPL substituted for SA when mutating ----
    SCapture sa_nobpl;
    if (mv == "X-NOBPL") sa_nobpl = run_s(dir + "/prog_s_sa_nobpl.bin", default_cfg());
    const SCapture& sa_for_lines = (mv == "X-NOBPL") ? sa_nobpl : sa;

    GradedLines gsa = graded_lines_of(sa_for_lines), gsb = graded_lines_of(sb), gsc = graded_lines_of(sc);
    GradedLines gsa0 = graded_lines_of(sa0), gsb0 = graded_lines_of(sb0), gsc0 = graded_lines_of(sc0);

    {
        struct Row { const char* label; GradedLines* g; };
        Row rows[] = {{"SA", &gsa}, {"SB", &gsb}, {"SC", &gsc}, {"SA0", &gsa0}, {"SB0", &gsb0}, {"SC0", &gsc0}};
        for (auto& r : rows) {
            checks.note(std::string("P3/P4-LINES [") + r.label + "]: " + std::to_string(r.g->total) +
                        " graded output lines");
            checks.expect(r.g->ok && r.g->total >= 400,
                "P3/P4-LINES [" + std::string(r.label) + "]: at least 400 graded output lines");
        }
    }

    // A4, dL/dT helper: evaluate one metric pair across a class.
    auto class_ok = [&](std::vector<LineMetric>& cls, int64_t& med_dL, int64_t& med_dT,
                         bool& a4_ok, int64_t& white_run) -> bool {
        if (cls.empty()) return false;
        std::vector<int64_t> dls, dts, wr;
        bool all_singletons = true;
        for (auto& m : cls) {
            dls.push_back(m.dL); dts.push_back(m.dT);
            if (m.white_run >= 0) wr.push_back(m.white_run);
            if (m.into_white != 1 || m.out_white != 1) all_singletons = false;
        }
        bool cons = consistent(dls) && consistent(dts);
        med_dL = median_of(dls); med_dT = median_of(dts);
        a4_ok = all_singletons && (wr.empty() ? false : consistent(wr));
        white_run = wr.empty() ? -1 : median_of(wr);
        return cons;
    };

    // ---- Anchor A4: line well-formedness and white-run consistency, all six
    // S runs together (SPECIFICATION.md P3 sec.7). Graded independently of
    // whether P3/P4's own effect shows up: a mutation (X-NOBPL) substitutes
    // SA's capture, which then has no white line at all, going red through
    // this same, unchanged comparator.
    {
        bool green_any = false;
        struct RunPack { const char* label; GradedLines* g; };
        RunPack packs[] = {{"SA", &gsa}, {"SB", &gsb}, {"SC", &gsc},
                           {"SA0", &gsa0}, {"SB0", &gsb0}, {"SC0", &gsc0}};
        for (int cls = 0; cls < 2; ++cls) {
            bool all_ok = true;
            std::vector<int64_t> wrs;
            std::string dump;
            for (auto& p : packs) {
                int64_t medL, medT, wr; bool a4ok;
                bool ok = class_ok(p.g->cls[cls], medL, medT, a4ok, wr);
                dump += std::string(p.label) + "=" + (ok && a4ok ? std::to_string(wr) : std::string("n/a")) + " ";
                if (!ok || !a4ok) { all_ok = false; continue; }
                wrs.push_back(wr);
            }
            checks.note("A4 class " + std::to_string(cls) + ": white run " + dump);
            if (all_ok && consistent(wrs)) green_any = true;
        }
        checks.expect(green_any,
            "A4 [p59,p60]: every graded line has exactly one trailing edge between consecutive "
            "leading edges, exactly one transition into white and one out of white, and the white "
            "run's length is the same, to +-1 output pixel clock, across all six S runs, in at "
            "least one parity class");
    }

    // ---- P3: HSSTRT alone sets the pulse's leading edge ------------------
    {
        bool green_any = false;
        for (int cls = 0; cls < 2; ++cls) {
            int64_t medLA, medTA, medLB, medTB, wra, wrb; bool a4a, a4b;
            bool okA = class_ok(gsa.cls[cls], medLA, medTA, a4a, wra);
            bool okB = class_ok(gsb.cls[cls], medLB, medTB, a4b, wrb);
            int64_t medLA0, medTA0, medLB0, medTB0, wra0, wrb0; bool a4a0, a4b0;
            bool okA0 = class_ok(gsa0.cls[cls], medLA0, medTA0, a4a0, wra0);
            bool okB0 = class_ok(gsb0.cls[cls], medLB0, medTB0, a4b0, wrb0);
            if (!(okA && okB && okA0 && okB0)) continue;
            bool b_ok = std::llabs(medLB - medLA) >= 4;
            bool c_ok = std::llabs(medTB - medTA) <= 1;
            bool contrast_ok = std::llabs(medLB0 - medLA0) <= 1 && std::llabs(medTB0 - medTA0) <= 1;
            checks.note("P3 class " + std::to_string(cls) + ": dL(SA)=" + std::to_string(medLA) +
                        " dL(SB)=" + std::to_string(medLB) + " dT(SA)=" + std::to_string(medTA) +
                        " dT(SB)=" + std::to_string(medTB) + " dL(SA0)=" + std::to_string(medLA0) +
                        " dL(SB0)=" + std::to_string(medLB0) + " dT(SA0)=" + std::to_string(medTA0) +
                        " dT(SB0)=" + std::to_string(medTB0));
            if (b_ok && c_ok && contrast_ok && a4a && a4b && a4a0 && a4b0) green_any = true;
        }
        checks.expect(green_any,
            "P3 [p280] opus:8-appA-oos.c5: while VARHSYEN is 1, HSSTRT alone moves the leading edge "
            "of the HSYNC pulse (>=4 output pixel clocks) and leaves the trailing edge in place "
            "(<=1), and with VARHSYEN=0 neither edge moves, in at least one parity class");
    }

    // ---- P4: HSSTOP alone sets the pulse's trailing edge -----------------
    {
        bool green_any = false;
        for (int cls = 0; cls < 2; ++cls) {
            int64_t medLA, medTA, medLC, medTC, wra, wrc; bool a4a, a4c;
            bool okA = class_ok(gsa.cls[cls], medLA, medTA, a4a, wra);
            bool okC = class_ok(gsc.cls[cls], medLC, medTC, a4c, wrc);
            int64_t medLA0, medTA0, medLC0, medTC0, wra0, wrc0; bool a4a0, a4c0;
            bool okA0 = class_ok(gsa0.cls[cls], medLA0, medTA0, a4a0, wra0);
            bool okC0 = class_ok(gsc0.cls[cls], medLC0, medTC0, a4c0, wrc0);
            if (!(okA && okC && okA0 && okC0)) continue;
            bool b_ok = std::llabs(medTC - medTA) >= 4;
            bool c_ok = std::llabs(medLC - medLA) <= 1;
            bool contrast_ok = std::llabs(medTC0 - medTA0) <= 1 && std::llabs(medLC0 - medLA0) <= 1;
            checks.note("P4 class " + std::to_string(cls) + ": dT(SA)=" + std::to_string(medTA) +
                        " dT(SC)=" + std::to_string(medTC) + " dL(SA)=" + std::to_string(medLA) +
                        " dL(SC)=" + std::to_string(medLC) + " dT(SA0)=" + std::to_string(medTA0) +
                        " dT(SC0)=" + std::to_string(medTC0) + " dL(SA0)=" + std::to_string(medLA0) +
                        " dL(SC0)=" + std::to_string(medLC0));
            if (b_ok && c_ok && contrast_ok && a4a && a4c && a4a0 && a4c0) green_any = true;
        }
        checks.expect(green_any,
            "P4 [p280] opus:8-appA-oos.c4: while VARHSYEN is 1, HSSTOP alone moves the trailing edge "
            "of the HSYNC pulse (>=4 output pixel clocks) and leaves the leading edge in place "
            "(<=1), and with VARHSYEN=0 neither edge moves, in at least one parity class");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
