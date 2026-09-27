// test_ch8_ecs_sync_registers.cpp -- ch8-ecs-sync-registers. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// NMH_MUTATE selects a mutated program, configuration or comparator input
// in place of its normal counterpart, one assertion or precondition at a
// time; none is exercised by a plain `make test`. P1, P2, P3 and P4 each
// carry their own mutation already, unconditionally: the contrasting runs
// (H1c/H2c for P1/P2, SA0/SB0/SC0 for P3/P4) that SPECIFICATION.md's own
// Runs table lists -- these are the page's contrasting case (VARBEAMEN=0
// for P1/P2, VARHSYEN=0 for P3/P4) and run on every `make test`, never
// behind NMH_MUTATE.
//
// The fixture requirement ("a program-stored constant asserted first") is
// met by its own dedicated line, printed before anything else: MARK1 read
// back through peek_logical(), the same call PC-ID's ID relies on.
//
// MUTATION PC-ID stimulus red NMH_MUTATE=X-OCS: harness set to OCS
//   (chipset_config clears the ECS bit); VPOSR bits 14-8 read the
//   original-chipset value instead of the enhanced-Agnus PAL value.
// MUTATION PC-ID stimulus red NMH_MUTATE=X-NTSC: harness set to NTSC
//   (chipset_config sets the NTSC bit); VPOSR bits 14-8 read the
//   enhanced-Agnus NTSC value instead of the PAL value.
// MUTATION PC-MARK stimulus red NMH_MUTATE=X-HALT-H: BRA.S * inserted
//   before H1's HTOTAL write (prog_h_h1_halt.s); neither MARK1 nor MARK2
//   is ever written.
// MUTATION PC-MARK stimulus red NMH_MUTATE=X-HALT-S: BRA.S * inserted
//   before SA's HSSTRT write (prog_s_sa_halt.s); MARK1 is never written.
// MUTATION P1-LINES stimulus red NMH_MUTATE=X-SHORT: H1's sampling loop
//   shortened to 1024 samples (prog_h_h1_short.s); fewer than 320 wraps
//   appear in the graded range.
// MUTATION P2-LINES stimulus red NMH_MUTATE=X-SHORT: the same shortened H1
//   run (the MARK1..MARK2 window shrinks with it); fewer than 320 graded
//   refresh periods.
// MUTATION P1-COVER stimulus red NMH_MUTATE=X-BURST: H1's and H2's
//   sampling loops replaced with a read-until-wrap-then-store-4 burst
//   (prog_h_h1_burst.s, prog_h_h2_burst.s); consecutive graded sample
//   stores are a whole line apart between bursts.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL: SA with DMACON's
//   SET write omitting BPLEN (prog_s_sa_nobpl.s); no bitplane DMA, no
//   white pixel, no line carries the landmark.
// MUTATION A4 stimulus red NMH_MUTATE=X-A4DOUBLE: SA's plane data changed
//   so every line carries two transitions into white and two out of white
//   (prog_s_sa_a4double.s), instead of one of each, while every line still
//   carries white and MARK1 is still reached.
// MUTATION A4-DROPPED stimulus red NMH_MUTATE=X-A4DROP: SA-shaped run
//   with the HSYNC pulse repositioned past the landmark, s=$00C8/e=$00D8
//   (prog_s_sa_swap.s).
// MUTATION A1 stimulus red NMH_MUTATE=A1: H1's and H2's own graded samples
//   (VARBEAMEN=1, bounded by N) substituted into anchors A1 [H1c] and A1
//   [H2c]'s unchanged comparator, which expects the free-running $E2-ish
//   range that only VARBEAMEN=0 produces.
// MUTATION A2 stimulus red NMH_MUTATE=X-AUD0: H1 with audio channel 0 DMA
//   started (prog_h_h1_aud0.s); a non-refresh DMA access appears at the
//   port, so a graded period's group no longer has exactly 4 members.
// MUTATION A3 stimulus red NMH_MUTATE=A3: H1's and H2's own graded periods
//   (VARBEAMEN=1, T~N counts) substituted into anchors A3 [H1c] and A3
//   [H2c]'s unchanged 227/228-CCK comparator.
// MUTATION P2(a) [H1] stimulus red NMH_MUTATE=P2A: H1's own graded
//   periods pooled with H2's (T1 != T2 by construction, P2(b)) fed
//   through the same all-equal comparator P2(a) [H1] uses.
// MUTATION P2(c) stimulus red NMH_MUTATE=P2C: H1c's own period (VARBEAMEN
//   =0, ~227 CCK) substituted into the same N/N+1 comparator meant for T1.
// MUTATION P3 stimulus red NMH_MUTATE=P3-NOMOVE: SB's graded lines
//   replaced by SA's own (s equal to SA's, so no move; clause (b) fails).
// MUTATION P4 stimulus red NMH_MUTATE=P4-NOMOVE: SC's graded lines
//   replaced by SA's own (e equal to SA's, so no move; clause (b) fails).
//
// The following already run, unconditionally, as the always-run contrast
// their own comparator shares with a graded proposition, and are not
// repeated behind a separate NMH_MUTATE value:
// MUTATION P1 [H1] stimulus red via H1c (VARBEAMEN=0), the same
//   any-exceeds comparator, run unconditionally.
// MUTATION P1 [H2] stimulus red via H2c, as H1/H1c.
// MUTATION P1 [H1c] stimulus red via H1 (VARBEAMEN=1) substituted into the
//   same any-exceeds comparator -- see NMH_MUTATE=A1 above, which performs
//   exactly this substitution for anchor A1.
// MUTATION P1 [H2c] stimulus red via H2, as H1c.
// MUTATION P2(b) stimulus red via H1c vs H2c (P2 contrast, below): the
//   same all-equal-pooled comparator gives d=0 where P2(b) requires d=16.
// MUTATION A1 [H2c] stimulus red via NMH_MUTATE=A1 (H2 substituted; see
//   above).
// MUTATION A3 [H2c] stimulus red via NMH_MUTATE=A3 (H2 substituted; see
//   above).
//
// A2 is also asserted for H1c and H2c; the page's own 4-refresh-slot count
// (p.194, p.285) is fixed by the chipset's slot allocation and is not a
// function of any register this package writes for those two runs, so it
// is not exercised by a dedicated mutation there -- unfalsifiable at this
// boundary, by a scope decision.

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

// One colour clock ("memory access cycle", p.194) is 8 clk_sys cycles.
// Neither the page nor the harness header defines this ratio; it is read
// off the implementation, and it is checked at runtime by anchor A3 (its
// measured periods must convert to 227 or 228, the page's own PAL line
// length, SPECIFICATION.md sec.3).
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
    bool mark1_peek_ok = false;
    uint16_t mark1_peek = 0;
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
    if (auto w = h.peek_logical(kHMark1)) { c.mark1_peek = *w; c.mark1_peek_ok = true; }

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
    // reads chip address 0), which this anchor's per-group count depends on
    // being distinct.
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

bool all_equal(const std::vector<uint64_t>& v) {
    if (v.empty()) return false;
    for (auto p : v) if (p != v[0]) return false;
    return true;
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
// classes (SPECIFICATION.md P3 sec.7, "Doubled lines"). dropped_white
// counts lines that carry a white pixel but that measure_span() could not
// turn into a LineMetric (0 or >=2 trailing HSYNC edges, or no
// black-to-white transition strictly after L) -- candidates for A4's own
// population that measurement silently drops instead of grading.
struct GradedLines {
    bool ok = false;
    std::vector<LineMetric> cls[2];
    size_t total = 0;
    size_t dropped_white = 0;
};

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
        else { g.dropped_white++; }
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

    // ======================================================================
    // Program H group: P1, P2. Runs H1, H2, H1c, H2c (SPECIFICATION.md
    // sec.5) always execute -- H1c/H2c are the page's own contrasting case
    // (VARBEAMEN=0) and are P1's and P2's own mutation. Captured first: the
    // fixture needs H1's own peek_logical read.
    // ======================================================================

    HCapture h1  = run_h(dir + "/prog_h_h1.bin",  default_cfg());
    HCapture h2  = run_h(dir + "/prog_h_h2.bin",  default_cfg());
    HCapture h1c = run_h(dir + "/prog_h_h1c.bin", default_cfg());
    HCapture h2c = run_h(dir + "/prog_h_h2c.bin", default_cfg());

    // ---- Fixture: a program-stored constant asserted first, through
    // peek_logical() -- the same call PC-ID's ID depends on -- before
    // anything else reads one.
    checks.expect(h1.loaded && h1.mark1_peek_ok && h1.mark1_peek == 0xA5A5,
        "FIXTURE: peek_logical(MARK1) [H1] == $A5A5, the literal Program H stores before anything else is read back");

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

    // ---- PC-ID, PC-MARK: shared preconditions, every H-group run --------
    bool pc_h1 = false, pc_h2 = false, pc_h1c = false, pc_h2c = false;
    {
        struct Row { const char* label; const HCapture* c; bool* pc; };
        Row rows[] = {{"H1", &h1, &pc_h1}, {"H2", &h2, &pc_h2},
                      {"H1c", &h1c, &pc_h1c}, {"H2c", &h2c, &pc_h2c}};
        for (auto& r : rows) {
            bool id_ok = r.c->loaded && pc_id_pal(r.c->id);
            checks.expect(id_ok,
                "PC-ID [" + std::string(r.label) + "] p.299: VPOSR bits 14-8 read the enhanced-Agnus "
                "PAL chip-identification value ($20 or 20 decimal)");
            bool mark_ok = r.c->loaded && r.c->mark1.has_value() &&
                           r.c->mark2.has_value() && *r.c->mark2 > *r.c->mark1;
            checks.expect(mark_ok,
                "PC-MARK [" + std::string(r.label) + "]: MARK1 appears at the port, and MARK2 later");
            *r.pc = id_ok && mark_ok;
        }
    }
    // ---- PC-ID/PC-MARK mutations: printed only under their own NMH_MUTATE,
    // not on every normal run (they would otherwise pass under a label
    // naming a configuration that did not run).
    if (mv == "X-OCS") {
        HCapture cap = run_h(dir + "/prog_h_h1.bin", ocs_cfg());
        checks.expect(cap.loaded && pc_id_pal(cap.id),
            "PC-ID [H1, X-OCS] p.299: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
    }
    if (mv == "X-NTSC") {
        HCapture cap = run_h(dir + "/prog_h_h1.bin", ntsc_cfg());
        checks.expect(cap.loaded && pc_id_pal(cap.id),
            "PC-ID [H1, X-NTSC] p.299: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
    }
    if (mv == "X-HALT-H") {
        HCapture cap = run_h(dir + "/prog_h_h1_halt.bin", default_cfg());
        checks.expect(cap.loaded && cap.mark1.has_value() && cap.mark2.has_value(),
            "PC-MARK [H1, X-HALT-H]: MARK1 appears at the port, and MARK2 later");
    }

    // X-SHORT/X-BURST substitute an alternate capture for one specific
    // precondition (P1-LINES/P2-LINES and P1-COVER respectively); nothing
    // else in this run changes.
    HCapture h1_short, h1_burst, h2_burst;
    if (mv == "X-SHORT") h1_short = run_h(dir + "/prog_h_h1_short.bin", default_cfg());
    if (mv == "X-BURST") { h1_burst = run_h(dir + "/prog_h_h1_burst.bin", default_cfg());
                           h2_burst = run_h(dir + "/prog_h_h2_burst.bin", default_cfg()); }

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

    // ---- Preconditions for P1: P1-LINES [H1]/[H2], P1-COVER [H1]/[H2] ---
    // (SPECIFICATION.md sec.3: "Every graded run asserts these first";
    // printed, and their real (unmutated) result captured for gating,
    // before P1 itself.)
    Extracted ex_h1 = extract(h1), ex_h2 = extract(h2);

    // "_real" variants always use H1's/H2's own unmutated capture: they
    // gate the propositions, which are themselves always graded on the
    // real capture. A precondition mutation (X-SHORT, X-BURST, X-NOBPL)
    // substitutes data for its own printed precondition line only -- it
    // must not also silently gate an unrelated proposition to "precondition
    // not met" as a side effect of that substitution.
    bool p1lines_h1 = false, p1lines_h2 = false, p1lines_h1_real = false;
    {
        Extracted ex = extract((mv == "X-SHORT") ? h1_short : h1);
        p1lines_h1 = ex.ok && (ex.wraps.size() - 1) >= 320;
        checks.expect(p1lines_h1, "P1-LINES [H1]: the graded samples contain at least 320 wraps");
        p1lines_h1_real = ex_h1.ok && (ex_h1.wraps.size() - 1) >= 320;
    }
    {
        p1lines_h2 = ex_h2.ok && (ex_h2.wraps.size() - 1) >= 320;
        checks.expect(p1lines_h2, "P1-LINES [H2]: the graded samples contain at least 320 wraps");
    }
    bool p1lines_h2_real = p1lines_h2;  // H2 is never X-SHORT's target

    auto p1_cover_check = [&](const char* label, Extracted& ex_normal, const HCapture& burst_cap,
                              uint16_t N, bool& real_ok) -> bool {
        Extracted ex = (mv == "X-BURST") ? extract(burst_cap) : ex_normal;
        bool cover_ok = false;
        uint64_t max_gap = 0;
        if (ex.ok) {
            size_t gs = ex.wraps[1];
            for (size_t i = gs + 1; i < ex.cyc.size(); ++i)
                max_gap = std::max(max_gap, ex.cyc[i] - ex.cyc[i - 1]);
            bool near_end = false;
            for (size_t i = gs; i < ex.hf.size(); ++i)
                if (ex.hf[i] >= (uint16_t)(N - 16)) near_end = true;
            cover_ok = (max_gap <= 16 * kCck) && near_end;
        }
        checks.note(std::string("P1-COVER [") + label + "]: largest consecutive-sample gap in the "
                    "graded range = " + std::to_string(max_gap / kCck) + " CCK");
        checks.expect(cover_ok,
            std::string("P1-COVER [") + label + "]: consecutive graded sample stores are never more "
            "than 16 CCK apart, and at least one graded sample has H >= N-16");
        if (ex_normal.ok) {
            size_t gs2 = ex_normal.wraps[1];
            uint64_t max_gap2 = 0;
            for (size_t i = gs2 + 1; i < ex_normal.cyc.size(); ++i)
                max_gap2 = std::max(max_gap2, ex_normal.cyc[i] - ex_normal.cyc[i - 1]);
            bool near_end2 = false;
            for (size_t i = gs2; i < ex_normal.hf.size(); ++i)
                if (ex_normal.hf[i] >= (uint16_t)(N - 16)) near_end2 = true;
            real_ok = (max_gap2 <= 16 * kCck) && near_end2;
        } else {
            real_ok = false;
        }
        return cover_ok;
    };
    bool p1cover_h1_real = false, p1cover_h2_real = false;
    p1_cover_check("H1", ex_h1, h1_burst, 0x00B0, p1cover_h1_real);
    p1_cover_check("H2", ex_h2, h2_burst, 0x00C0, p1cover_h2_real);

    // ---- Preconditions for P2: P2-LINES [H1]/[H2] ------------------------
    P2Data d1 = analyze_p2(h1), d2 = analyze_p2(h2), d1c = analyze_p2(h1c), d2c = analyze_p2(h2c);
    P2Data d1_for_lines = (mv == "X-SHORT") ? analyze_p2(h1_short) : d1;
    bool p2lines_h1 = d1_for_lines.ok && d1_for_lines.periods.size() >= 320;
    bool p2lines_h2 = d2.ok && d2.periods.size() >= 320;
    checks.expect(p2lines_h1, "P2-LINES [H1]: at least 320 graded periods");
    checks.expect(p2lines_h2, "P2-LINES [H2]: at least 320 graded periods");
    bool p2lines_h1_real = d1.ok && d1.periods.size() >= 320;
    bool p2lines_h2_real = p2lines_h2;  // H2 is never X-SHORT's target

    // ---- Anchors A1, A2, A3 (printed, and gated on their own captures,
    // before P1 and P2 use them; SPECIFICATION.md P1/P2 sec.7) -----------
    Extracted ex_h1c_check = extract(h1c), ex_h2c_check = extract(h2c);
    // a1_*_ok reflects NMH_MUTATE=A1's substitution (for the anchor's own
    // printed line); a1_*_ok_real always uses H1c's/H2c's own capture and
    // gates P1 [H1c]/[H2c], which are themselves always graded on the real
    // capture -- the A1 mutation must not also silently gate them.
    bool a1_h1c_ok, a1_h2c_ok, a1_h1c_ok_real, a1_h2c_ok_real;
    {
        Extracted ex = extract((mv == "A1") ? h1 : h1c);
        uint16_t maxH = 0;
        if (ex.ok) for (size_t i = ex.wraps[1]; i < ex.hf.size(); ++i) maxH = std::max(maxH, ex.hf[i]);
        a1_h1c_ok = pc_h1c && ex.ok && maxH >= (0xE2 - 16) && maxH <= 0xE3;
        checks.expect(a1_h1c_ok,
            "A1 [H1c] [p23]: the largest graded H with VARBEAMEN=0 lies between $E2-16 and $E3");
        uint16_t maxHr = 0;
        if (ex_h1c_check.ok) for (size_t i = ex_h1c_check.wraps[1]; i < ex_h1c_check.hf.size(); ++i)
            maxHr = std::max(maxHr, ex_h1c_check.hf[i]);
        a1_h1c_ok_real = pc_h1c && ex_h1c_check.ok && maxHr >= (0xE2 - 16) && maxHr <= 0xE3;
    }
    {
        Extracted ex = extract((mv == "A1") ? h2 : h2c);
        uint16_t maxH = 0;
        if (ex.ok) for (size_t i = ex.wraps[1]; i < ex.hf.size(); ++i) maxH = std::max(maxH, ex.hf[i]);
        a1_h2c_ok = pc_h2c && ex.ok && maxH >= (0xE2 - 16) && maxH <= 0xE3;
        checks.expect(a1_h2c_ok,
            "A1 [H2c] [p23]: the largest graded H with VARBEAMEN=0 lies between $E2-16 and $E3");
        uint16_t maxHr = 0;
        if (ex_h2c_check.ok) for (size_t i = ex_h2c_check.wraps[1]; i < ex_h2c_check.hf.size(); ++i)
            maxHr = std::max(maxHr, ex_h2c_check.hf[i]);
        a1_h2c_ok_real = pc_h2c && ex_h2c_check.ok && maxHr >= (0xE2 - 16) && maxHr <= 0xE3;
    }

    auto a2_check = [&](const char* label, P2Data& d) -> bool {
        bool ok = d.ok && d.a2_all_four;
        checks.expect(ok,
            "A2 [" + std::string(label) + "] [p194,p285]: every graded period contains exactly "
            "4 refresh accesses");
        return ok;
    };
    P2Data d1_for_a2 = (mv == "X-AUD0") ? analyze_p2(run_h(dir + "/prog_h_h1_aud0.bin", default_cfg())) : d1;
    bool a2_h1  = a2_check("H1", d1_for_a2);
    bool a2_h2  = a2_check("H2", d2);
    bool a2_h1c = a2_check("H1c", d1c);
    bool a2_h2c = a2_check("H2c", d2c);
    (void)a2_h1; (void)a2_h2; (void)a2_h1c; (void)a2_h2c;
    checks.record(
        "A2-NOT-EXERCISED [H1c,H2c]: no dedicated mutation -- refresh's own 4-slot count (p.194,p.285) "
        "is fixed by the chipset's slot allocation, not by any register this package writes for these "
        "two runs; unfalsifiable at this boundary, by a scope decision.");

    uint64_t T1_raw = d1.ok && !d1.periods.empty() ? d1.periods[0] : 0;
    uint64_t T2_raw = d2.ok && !d2.periods.empty() ? d2.periods[0] : 0;
    uint64_t T1c_raw = d1c.ok && !d1c.periods.empty() ? d1c.periods[0] : 0;
    uint64_t T2c_raw = d2c.ok && !d2c.periods.empty() ? d2c.periods[0] : 0;

    // a3_h1c_ok_real: A3 [H1c] computed on H1c's own capture always, never
    // substituted -- P2(c) gates on this, not on the mutation-sensitive
    // a3_h1c_ok (see NMH_MUTATE=A3, which must not also silently gate
    // P2(c) as a side effect of substituting into the anchor's own line).
    bool a3_h1c_ok, a3_h2c_ok, a3_h1c_ok_real;
    {
        uint64_t raw = (mv == "A3") ? T1_raw : T1c_raw;
        uint64_t cck_v = raw / kCck;
        a3_h1c_ok = pc_h1c && raw != 0 && (cck_v == 227 || cck_v == 228);
        checks.expect(a3_h1c_ok,
            "A3 [H1c] [p23,p194,p304]: every graded period converts to 227 or 228 CCK (VARBEAMEN=0)");
        uint64_t cck_r = T1c_raw / kCck;
        a3_h1c_ok_real = pc_h1c && T1c_raw != 0 && (cck_r == 227 || cck_r == 228);
    }
    {
        uint64_t raw = (mv == "A3") ? T2_raw : T2c_raw;
        uint64_t cck_v = raw / kCck;
        a3_h2c_ok = pc_h2c && raw != 0 && (cck_v == 227 || cck_v == 228);
        checks.expect(a3_h2c_ok,
            "A3 [H2c] [p23,p194,p304]: every graded period converts to 227 or 228 CCK (VARBEAMEN=0)");
    }

    // ---- P1: with VARBEAMEN set, no H readback exceeds HTOTAL ------------
    // MUTATION P1 stimulus red via the always-run contrast: H1c/H2c (VARBEAMEN=0)
    // run unconditionally and are graded a few lines below, through the same
    // "any_exceeds" comparator P1 [H1]/[H2] use.
    {
        struct Run { const char* label; HCapture* cap; uint16_t N; bool contrast; bool precond; };
        Run runs[] = {{"H1", &h1, 0x00B0, false, pc_h1 && p1lines_h1_real && p1cover_h1_real},
                      {"H2", &h2, 0x00C0, false, pc_h2 && p1lines_h2_real && p1cover_h2_real},
                      {"H1c", &h1c, 0x00B0, true, pc_h1c && a1_h1c_ok_real},
                      {"H2c", &h2c, 0x00C0, true, pc_h2c && a1_h2c_ok_real}};
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

            if (!r.precond) {
                checks.record(std::string("P1 [") + r.label + "]: precondition not met");
                continue;
            }
            if (!r.contrast) {
                checks.expect(!any_exceeds,
                    "P1 [" + std::string(r.label) + "] [p280] opus:8-appA-oos.c6: while VARBEAMEN is 1, "
                    "no graded horizontal-position readback exceeds HTOTAL");
            } else {
                checks.expect(any_exceeds,
                    "P1 [" + std::string(r.label) + "] [p280] opus:8-appA-oos.c6 (contrast, VARBEAMEN=0): "
                    "at least one graded readback exceeds N, since HTOTAL's condition does not hold");
            }
        }
    }

    // ---- P2: with VARBEAMEN set, every line lasts N or N+1 CCK -----------
    // MUTATION P2(b) stimulus red via the always-run contrast: P2 contrast,
    // below, is the same all-equal machinery applied across H1c and H2c and
    // shows d=0 where P2(b) requires d=16.
    {
        // P2(a) [H1]/[H2]: while VARBEAMEN is 1, every graded period has one
        // and the same value (opus:8-appA-oos.c6). MUTATION P2(a) [H1]
        // stimulus red NMH_MUTATE=P2A: see header.
        std::vector<uint64_t> d1_pooled_periods = d1.periods;
        if (mv == "P2A" && d2.ok) d1_pooled_periods.insert(d1_pooled_periods.end(), d2.periods.begin(), d2.periods.end());
        P2Data d1_for_a = d1;
        d1_for_a.periods = d1_pooled_periods;
        if (d1.ok && !d1.periods.empty())
            checks.note("P2 [H1]: " + std::to_string(d1.periods.size()) + " graded periods, first=" +
                        std::to_string(d1.periods[0]) + " clk_sys cycles");
        if (!(pc_h1 && p2lines_h1_real)) {
            checks.record("P2(a) [H1]: precondition not met");
        } else {
            checks.expect(d1_for_a.ok && !d1_for_a.periods.empty() && all_equal(d1_for_a.periods),
                "P2(a) [H1] [p280,p304] opus:8-appA-oos.c6: while VARBEAMEN is 1, every graded line "
                "period has one and the same value");
        }
        if (d2.ok && !d2.periods.empty())
            checks.note("P2 [H2]: " + std::to_string(d2.periods.size()) + " graded periods, first=" +
                        std::to_string(d2.periods[0]) + " clk_sys cycles");
        if (!(pc_h2 && p2lines_h2_real)) {
            checks.record("P2(a) [H2]: precondition not met");
        } else {
            checks.expect(d2.ok && !d2.periods.empty() && all_equal(d2.periods),
                "P2(a) [H2] [p280,p304] opus:8-appA-oos.c6: while VARBEAMEN is 1, every graded line "
                "period has one and the same value");
        }
        if (d1c.ok && !d1c.periods.empty())
            checks.note("P2 [H1c]: " + std::to_string(d1c.periods.size()) + " graded periods, first=" +
                        std::to_string(d1c.periods[0]) + " clk_sys cycles");
        if (d2c.ok && !d2c.periods.empty())
            checks.note("P2 [H2c]: " + std::to_string(d2c.periods.size()) + " graded periods, first=" +
                        std::to_string(d2c.periods[0]) + " clk_sys cycles");

        bool have_t1t2 = pc_h1 && pc_h2 && p2lines_h1_real && p2lines_h2_real &&
                         d1.ok && !d1.periods.empty() && d2.ok && !d2.periods.empty();
        if (!have_t1t2) {
            checks.record("P2(b): precondition not met");
        } else {
            checks.expect((int64_t)(T2_raw - T1_raw) == 16 * (int64_t)kCck,
                "P2(b) [p280,p304] opus:8-appA-oos.c6: T2 - T1 = 16 CCK ($C0 - $B0)");
        }

        // P2(c). MUTATION P2(c) stimulus red NMH_MUTATE=P2C: see header.
        if (!have_t1t2) {
            checks.record("P2(c): precondition not met");
        } else if (!a3_h1c_ok_real) {
            checks.record("P2(c): precondition not met (A3 [H1c], the unit-conversion anchor, does not hold)");
        } else {
            uint64_t t1_for_c = (mv == "P2C") ? T1c_raw : T1_raw;
            uint64_t t1_cck = t1_for_c / kCck;
            bool c_ok = t1_for_c != 0 && (t1_cck >= 0x00B0) && (t1_cck - 0x00B0 <= 1);
            checks.expect(c_ok,
                "P2(c) [p280,p304] opus:8-appA-oos.c6: T1 (converted to CCK) - $B0 is 0 or 1 (page note 2)");
        }

        // P2 contrast: folds the former mislabelled "P2(a) [H1c]/[H2c]"
        // checks -- every graded period in H1c equals every graded period in
        // H2c (not just periods[0]), through the unenabled comparator.
        if (!(pc_h1c && pc_h2c)) {
            checks.record("P2 contrast: precondition not met");
        } else {
            std::vector<uint64_t> pooled = d1c.periods;
            pooled.insert(pooled.end(), d2c.periods.begin(), d2c.periods.end());
            checks.expect(d1c.ok && !d1c.periods.empty() && d2c.ok && !d2c.periods.empty() &&
                          all_equal(pooled),
                "P2 contrast [p280,p304] (VARBEAMEN=0): every graded period in H1c equals every "
                "graded period in H2c -- HTOTAL has no effect");
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
    bool pc_sa = false, pc_sb = false, pc_sc = false, pc_sa0 = false, pc_sb0 = false, pc_sc0 = false;
    {
        struct Row { const char* label; const SCapture* c; bool* pc; };
        Row rows[] = {{"SA", &sa, &pc_sa}, {"SB", &sb, &pc_sb}, {"SC", &sc, &pc_sc},
                      {"SA0", &sa0, &pc_sa0}, {"SB0", &sb0, &pc_sb0}, {"SC0", &sc0, &pc_sc0}};
        for (auto& r : rows) {
            bool id_ok = r.c->loaded && pc_id_pal(r.c->id);
            checks.expect(id_ok,
                "PC-ID [" + std::string(r.label) + "] p.299: VPOSR bits 14-8 read the enhanced-Agnus "
                "PAL chip-identification value ($20 or 20 decimal)");
            bool mark_ok = r.c->loaded && r.c->mark1.has_value();
            checks.expect(mark_ok,
                "PC-MARK [" + std::string(r.label) + "]: MARK1 appears at the port");
            *r.pc = id_ok && mark_ok;
        }
    }
    if (mv == "X-HALT-S") {
        SCapture cap = run_s(dir + "/prog_s_sa_halt.bin", default_cfg());
        checks.expect(cap.loaded && cap.mark1.has_value(),
            "PC-MARK [SA, X-HALT-S]: MARK1 appears at the port");
    }

    // ---- Shared: graded lines per run, X-NOBPL substituted for SA when mutating ----
    SCapture sa_nobpl;
    if (mv == "X-NOBPL") sa_nobpl = run_s(dir + "/prog_s_sa_nobpl.bin", default_cfg());
    const SCapture& sa_for_lines = (mv == "X-NOBPL") ? sa_nobpl : sa;

    GradedLines gsa = graded_lines_of(sa_for_lines), gsb = graded_lines_of(sb), gsc = graded_lines_of(sc);
    GradedLines gsa0 = graded_lines_of(sa0), gsb0 = graded_lines_of(sb0), gsc0 = graded_lines_of(sc0);
    // The real (unmutated) SA capture, used for gating and for checks that
    // are not X-NOBPL's own target.
    GradedLines gsa_real = graded_lines_of(sa);

    bool p34lines_sa, p34lines_sb, p34lines_sc, p34lines_sa0, p34lines_sb0, p34lines_sc0;
    {
        struct Row { const char* label; GradedLines* g; bool* ok; };
        Row rows[] = {{"SA", &gsa, &p34lines_sa}, {"SB", &gsb, &p34lines_sb}, {"SC", &gsc, &p34lines_sc},
                      {"SA0", &gsa0, &p34lines_sa0}, {"SB0", &gsb0, &p34lines_sb0}, {"SC0", &gsc0, &p34lines_sc0}};
        for (auto& r : rows) {
            checks.note(std::string("P3/P4-LINES [") + r.label + "]: " + std::to_string(r.g->total) +
                        " graded output lines");
            *r.ok = r.g->ok && r.g->total >= 400;
            checks.expect(*r.ok,
                "P3/P4-LINES [" + std::string(r.label) + "]: at least 400 graded output lines");
        }
    }
    // P34-LINES [SA]'s real (unmutated) counterpart, for gating P3/P4: a
    // precondition mutation (X-NOBPL) must not also silently gate the
    // proposition itself, which is graded on SA's own real capture.
    bool p34lines_sa_real = gsa_real.ok && gsa_real.total >= 400;

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

    bool precond_s_all = pc_sa && pc_sb && pc_sc && pc_sa0 && pc_sb0 && pc_sc0;

    // ---- Anchor A4: line well-formedness and white-run consistency, all six
    // S runs together (SPECIFICATION.md P3 sec.7). MUTATION A4 stimulus red
    // NMH_MUTATE=X-A4DOUBLE: see header -- keeps the population (every line
    // still carries white) and goes red through the singleton clause itself.
    {
        SCapture sa_a4d;
        if (mv == "X-A4DOUBLE") sa_a4d = run_s(dir + "/prog_s_sa_a4double.bin", default_cfg());
        GradedLines gsa_a4 = (mv == "X-A4DOUBLE") ? graded_lines_of(sa_a4d) : gsa_real;

        bool green_any = false;
        struct RunPack { const char* label; GradedLines* g; };
        RunPack packs[] = {{"SA", &gsa_a4}, {"SB", &gsb}, {"SC", &gsc},
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
        if (!precond_s_all) {
            checks.record("A4: precondition not met");
        } else {
            checks.expect(green_any,
                "A4 [p59,p60]: every graded line has exactly one trailing edge between consecutive "
                "leading edges, exactly one transition into white and one out of white, and the white "
                "run's length is the same, to +-1 output pixel clock, across all six S runs, in at "
                "least one parity class");
        }
    }

    // ---- A4-DROPPED: the quantifier "every" in A4 is not graded by A4
    // itself, since measure_span()/graded_lines_of() silently drop a
    // white-carrying line that fails measurement instead of grading it red.
    // Counts such drops across all six S runs; requires 0.
    // MUTATION A4-DROPPED stimulus red NMH_MUTATE=X-A4DROP: see header.
    {
        SCapture sa_swap;
        if (mv == "X-A4DROP") sa_swap = run_s(dir + "/prog_s_sa_swap.bin", default_cfg());
        GradedLines gsa_drop = (mv == "X-A4DROP") ? graded_lines_of(sa_swap) : gsa_real;

        size_t dropped = gsa_drop.dropped_white + gsb.dropped_white + gsc.dropped_white +
                         gsa0.dropped_white + gsb0.dropped_white + gsc0.dropped_white;
        checks.note("A4-DROPPED: white-carrying graded lines that failed measurement = " +
                    std::to_string(dropped));
        if (!precond_s_all) {
            checks.record("A4-DROPPED: precondition not met");
        } else {
            checks.expect(dropped == 0,
                "A4-DROPPED [p59,p60]: no white-carrying graded line is dropped by measurement "
                "instead of graded (0 required)");
        }
    }

    // ---- P3: HSSTRT alone sets the pulse's leading edge ------------------
    // MUTATION P3 stimulus red NMH_MUTATE=P3-NOMOVE: SB's graded lines
    // replaced by SA's own (s equal to SA's s, so no move).
    {
        GradedLines& gsb_use = (mv == "P3-NOMOVE") ? gsa_real : gsb;
        bool precond = pc_sa && pc_sb && pc_sa0 && pc_sb0 &&
                       p34lines_sa_real && p34lines_sb && p34lines_sa0 && p34lines_sb0;
        if (!precond) {
            checks.record("P3: precondition not met");
        } else {
            bool green_any = false;
            for (int cls = 0; cls < 2; ++cls) {
                int64_t medLA, medTA, medLB, medTB, wra, wrb; bool a4a, a4b;
                bool okA = class_ok(gsa_real.cls[cls], medLA, medTA, a4a, wra);
                bool okB = class_ok(gsb_use.cls[cls], medLB, medTB, a4b, wrb);
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
                "P3 [p280] opus:8-appA-oos.c5: while VARHSYEN and VARBEAMEN are both 1, HSSTRT alone "
                "moves the leading edge of the HSYNC pulse (>=4 output pixel clocks) and leaves the "
                "trailing edge in place (<=1), and with VARHSYEN=0 neither edge moves, in at least "
                "one parity class");
        }
    }

    // ---- P4: HSSTOP alone sets the pulse's trailing edge -----------------
    // MUTATION P4 stimulus red NMH_MUTATE=P4-NOMOVE: SC's graded lines
    // replaced by SA's own (e equal to SA's e, so no move).
    {
        GradedLines& gsc_use = (mv == "P4-NOMOVE") ? gsa_real : gsc;
        bool precond = pc_sa && pc_sc && pc_sa0 && pc_sc0 &&
                       p34lines_sa_real && p34lines_sc && p34lines_sa0 && p34lines_sc0;
        if (!precond) {
            checks.record("P4: precondition not met");
        } else {
            bool green_any = false;
            for (int cls = 0; cls < 2; ++cls) {
                int64_t medLA, medTA, medLC, medTC, wra, wrc; bool a4a, a4c;
                bool okA = class_ok(gsa_real.cls[cls], medLA, medTA, a4a, wra);
                bool okC = class_ok(gsc_use.cls[cls], medLC, medTC, a4c, wrc);
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
                "P4 [p280] opus:8-appA-oos.c4: while VARHSYEN and VARBEAMEN are both 1, HSSTOP alone "
                "moves the trailing edge of the HSYNC pulse (>=4 output pixel clocks) and leaves the "
                "leading edge in place (<=1), and with VARHSYEN=0 neither edge moves, in at least "
                "one parity class");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
