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
// Every capture -- normal and mutant -- asserts that its program reached
// DONE (a `DONE [<label>]` line), so a mutant that goes red from a dead
// setup (a crash, a halted CPU, an emptied population) is distinguishable
// from one that goes red through the clause under test.
//
// The fixture requirement ("a program-stored constant asserted first") is
// met by its own dedicated line, printed before anything else: MARK1 read
// back through peek_logical(), the same call PC-ID's ID relies on.
//
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXTURE: prog_h_h1_fixture.bin
//   (FIXTURE_MUTANT) stores $DEAD instead of $A5A5 to MARK1; the rest of
//   the program, including DONE, still runs.
// MUTATION PC-ID stimulus red NMH_MUTATE=X-OCS: harness set to OCS
//   (chipset_config clears the ECS bit); VPOSR bits 14-8 read the
//   original-chipset value instead of the enhanced-Agnus PAL value.
// MUTATION PC-ID stimulus red NMH_MUTATE=X-NTSC: harness set to NTSC
//   (chipset_config sets the NTSC bit); VPOSR bits 14-8 read the
//   enhanced-Agnus NTSC value instead of the PAL value.
// MUTATION PC-MARK stimulus red NMH_MUTATE=X-HALT-H: MARK1's write omitted
//   (prog_h_h1_halt.bin, MARK_OMIT); no marker appears, while the rest of
//   the program, including DONE, still runs.
// MUTATION PC-MARK stimulus red NMH_MUTATE=X-HALT-S: MARK1's write omitted
//   (prog_s_sa_halt.bin, MARK_OMIT); no marker appears, while the rest of
//   the program, including DONE, still runs.
// MUTATION P1-LINES stimulus red NMH_MUTATE=X-SHORT: H1's sampling loop
//   shortened to 1024 samples (prog_h_h1_short.bin); fewer than 320 wraps
//   appear in the graded range.
// MUTATION P2-LINES stimulus red NMH_MUTATE=X-SHORT: the same shortened H1
//   run (the MARK1..MARK2 window shrinks with it); fewer than 320 graded
//   refresh periods.
// MUTATION P1-COVER stimulus red NMH_MUTATE=X-BURST: H1's and H2's
//   sampling loops replaced with a read-until-wrap-then-store-4 burst
//   (prog_h_h1_burst.bin, prog_h_h2_burst.bin); consecutive graded sample
//   stores are a whole line apart between bursts.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL: SA with PLANE
//   holding only 32 lines of pattern instead of 256 (prog_s_sa_nobpl.bin,
//   SHORT_PLANE); bitplane DMA still runs, so the population is a live,
//   reduced but nonempty one, not an emptied one.
// MUTATION A4 stimulus red NMH_MUTATE=X-A4DOUBLE: SA's plane data changed
//   so every line carries two transitions into white and two out of white
//   (prog_s_sa_a4double.bin), instead of one of each, while every line
//   still carries white and the program still reaches DONE.
// MUTATION A1 stimulus red NMH_MUTATE=A1: H1's/H2's own graded samples
//   (VARBEAMEN=1, bounded by N) substituted into A1 [H1c]'s/A1 [H2c]'s
//   unchanged comparator (which expects the free-running $E2-ish range
//   that only VARBEAMEN=0 produces), AND H1c's/H2c's own graded samples
//   (VARBEAMEN=0, unbounded) substituted into P1 [H1]'s/P1 [H2]'s
//   unchanged any-exceeds comparator, AND H1's/H2's own samples substituted
//   into P1 [H1c]'s/P1 [H2c]'s unchanged contrast comparator: one live
//   cross-substitution, six printed lines go red.
// MUTATION A2 stimulus red NMH_MUTATE=X-AUD0: H1, H2, H1c and H2c each
//   with audio channel 0 DMA started (prog_h_h1_aud0.bin and its H2/H1c/
//   H2c counterparts); a non-refresh DMA access appears at the port in
//   every one, so no graded period's group has exactly 4 members.
// MUTATION A3 stimulus red NMH_MUTATE=A3: H1's and H2's own graded periods
//   (VARBEAMEN=1, T~N counts) substituted into anchors A3 [H1c] and A3
//   [H2c]'s unchanged 227/228-CCK comparator.
// MUTATION P2(a) [H1] stimulus red NMH_MUTATE=P2A: H1 with HTOTAL written
//   again mid-loop, to $00C0 in place of $00B0 (prog_h_h1_retotal.bin,
//   RETOTAL_MID); p.280 states no exception for a second write, so the
//   graded periods after that point are page-predicted to differ from
//   those before it.
// MUTATION P2(b) stimulus red NMH_MUTATE=P2B: H1c's/H2c's own periods
//   (VARBEAMEN=0, d=0) substituted into P2(b)'s unchanged delta comparator.
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
//   any-exceeds comparator, run unconditionally -- see NMH_MUTATE=A1
//   above for the live substitution that proves it.
// MUTATION P1 [H2] stimulus red via H2c, as H1/H1c.
// MUTATION P1 [H1c] stimulus red via H1 (VARBEAMEN=1), as above.
// MUTATION P1 [H2c] stimulus red via H2, as H1c.
// MUTATION P2(b) stimulus red via H1c vs H2c (P2 contrast, below): the
//   same all-equal-pooled comparator gives d=0 where P2(b) requires d=16
//   -- see NMH_MUTATE=P2B above for the live substitution into P2(b)
//   itself.
// MUTATION A1 [H2c] stimulus red via NMH_MUTATE=A1 (H2 substituted; see
//   above).
// MUTATION A3 [H2c] stimulus red via NMH_MUTATE=A3 (H2 substituted; see
//   above).
//
// A2 is also asserted for H1c and H2c, each with its own X-AUD0 mutation
// (above): the page's own 4-refresh-slot count (p.194, p.285) is fixed by
// the chipset's slot allocation and not a function of VARBEAMEN, so the
// same live audio stimulus applies to every H run.
//
// A4-DROPPED is not exercised: no page gives HSSTRT/HSSTOP a layout or
// scale (SPECIFICATION.md page note 7) from which to choose a stimulus
// that is page-predicted -- rather than tuned against this RTL -- to land
// the sync pulse on the landmark; a `record()` line below states this.

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
// length, SPECIFICATION.md sec.3). Every other check that consumes this
// ratio (P1-COVER, P2-LINES, A2, P2(a)/(b)) is gated on A3: sec.9 below.
constexpr uint64_t kCck = 8;

// nanomig.i's DONE macro: the last thing every program does, after the
// measured work, is store DONE_VALUE at DONE_ADDR and hold. Every capture
// below asserts this write happened.
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

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

bool all_equal(const std::vector<uint64_t>& v) {
    if (v.empty()) return false;
    for (auto p : v) if (p != v[0]) return false;
    return true;
}

bool all_equal_i64(const std::vector<int64_t>& v) {
    if (v.empty()) return false;
    for (auto p : v) if (p != v[0]) return false;
    return true;
}

// Anchor A3 (SPECIFICATION.md P2 sec.7): every graded period, not just the
// first, must be exactly divisible by kCck and convert to 227 or 228 CCK
// (p.23's PAL count, page note 3).
bool a3_all_ok(const std::vector<uint64_t>& periods) {
    if (periods.empty()) return false;
    for (auto raw : periods) {
        if (raw % kCck != 0) return false;
        uint64_t v = raw / kCck;
        if (v != 227 && v != 228) return false;
    }
    return true;
}

// ==== Program H capture ===================================================

struct HCapture {
    bool loaded = false;
    uint16_t id = 0;
    std::optional<uint64_t> mark1, mark2, done;
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

    const uint32_t mark1_w = kHMark1 / 2, mark2_w = kHMark2 / 2, done_w = kDoneAddr / 2;
    const uint32_t samp_lo = kHSamples / 2, samp_hi = samp_lo + (uint32_t)kHSampleCount;
    for (const auto& a : c.log) {
        if (!a.is_write) continue;
        if (a.word_addr == mark1_w && a.data == 0xA5A5 && !c.mark1) { c.mark1 = a.cycle; continue; }
        if (a.word_addr == mark2_w && a.data == 0x5A5A && !c.mark2) { c.mark2 = a.cycle; continue; }
        if (a.word_addr == done_w && a.data == kDoneValue && !c.done) { c.done = a.cycle; continue; }
        if (a.word_addr >= samp_lo && a.word_addr < samp_hi) c.samples.push_back({a.cycle, a.data});
    }
    return c;
}

bool is_cpu_access_h(const nmh::RamAccess& a) {
    if (!a.is_write) return a.word_addr >= kProgLoW && a.word_addr < kProgHiW;
    const uint32_t mark1_w = kHMark1 / 2, mark2_w = kHMark2 / 2, id_w = kHId / 2, done_w = kDoneAddr / 2;
    const uint32_t samp_lo = kHSamples / 2, samp_hi = samp_lo + (uint32_t)kHSampleCount;
    return a.word_addr == mark1_w || a.word_addr == mark2_w || a.word_addr == id_w || a.word_addr == done_w ||
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

// ==== Program S capture ==================================================

struct SCapture {
    bool loaded = false;
    uint16_t id = 0;
    std::optional<uint64_t> mark1, done;
    std::vector<nmh::VideoSample> video;
    std::vector<uint64_t> refresh;  // anchor A5: memory-refresh (word address 0) access cycles
};

// Anchor A5 (SPECIFICATION.md P3 sec.7): memory-refresh accesses always
// read chip address (word) 0, exactly like refresh_groups() above relies
// on for Program H -- a fixed per-chip-line landmark that DIWSTRT,
// DDFSTRT, HSSTRT and HSSTOP none of them touch. Clustered the same way
// refresh_groups() clusters: a wait-stated bus cycle can repeat the same
// logical access across several adjacent samples.
std::vector<uint64_t> refresh_cycles_s(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> cycles;
    for (const auto& a : log) if (!a.is_write && a.word_addr == 0) cycles.push_back(a.cycle);
    std::sort(cycles.begin(), cycles.end());
    std::vector<uint64_t> out;
    for (auto c : cycles) if (out.empty() || c - out.back() >= 32 * kCck) out.push_back(c);
    return out;
}

SCapture run_s(const std::string& bin, const nmh::Config& cfg) {
    SCapture c;
    nmh::NanomigHarness h(cfg);
    if (h.load_program(bin) == 0) return c;
    c.loaded = true;
    h.boot(kBootBudget);
    h.run_cycles(kSRunBudget);
    if (auto w = h.peek_logical(kSId)) c.id = *w;
    const uint32_t mark1_w = kSMark1 / 2, done_w = kDoneAddr / 2;
    for (const auto& a : h.ram_log()) {
        if (!a.is_write) continue;
        if (a.word_addr == mark1_w && a.data == 0xA5A5 && !c.mark1) { c.mark1 = a.cycle; continue; }
        if (a.word_addr == done_w && a.data == kDoneValue && !c.done) { c.done = a.cycle; continue; }
    }
    c.refresh = refresh_cycles_s(h.ram_log());
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
    // dL, dT: the landmark M's distance to L and to T (SPECIFICATION.md P3
    // sec.3). dMR: M's own distance to R, the nearest memory-refresh access
    // before M (anchor A5, sec.7) -- a landmark HSSTRT/HSSTOP never touch,
    // used to check that M itself does not move (below; see A5's own
    // comment at its call site for why this, not dL/dT, is where R is
    // used).
    int64_t dL = 0, dT = 0, dMR = 0;
    int64_t white_run = -1;
    int into_white = 0, out_white = 0;
};

// One canonical output line: [L, nextL). T is the sole opposite-polarity
// transition strictly between them (anchor A4's "exactly one trailing
// edge"); M is the first white pixel after L. R, passed in by the caller
// (graded_lines_of()), is the nearest memory-refresh access before M.
LineMetric measure_span(const std::vector<nmh::VideoSample>& v, size_t L, size_t nextL, bool pulse_is_zero,
                         uint64_t R) {
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
    m.dMR = (int64_t)M - (int64_t)R;
    if (white_end != SIZE_MAX) m.white_run = (int64_t)(white_end - M);
    m.ok = true;
    return m;
}

// Graded lines of one S run, split into the two doubled-line parity
// classes (SPECIFICATION.md P3 sec.7, "Doubled lines"). dropped_white
// counts lines that carry a white pixel but that measure_span() could not
// turn into a LineMetric (0 or >=2 trailing HSYNC edges, no black-to-white
// transition strictly after L, or no refresh observed before L) --
// candidates for A4's own population that measurement silently drops
// instead of grading.
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

    // Anchor A5 (SPECIFICATION.md P3 sec.7): white content (M) is driven
    // by DDFSTRT/DIWSTRT/PLANE data alone, never by HSSTRT/HSSTOP, so its
    // absolute position is identical for "the same" chip line across all
    // six S runs -- unlike a count of L-edges from the field's own start,
    // which HSSTRT can shift enough to move a boundary line in or out of
    // the field differently between runs (measured directly: edge counts
    // from field->begin disagreed by one line between SA and SB). M is
    // therefore found first, by a direct scan of the field's own video
    // content, not by iterating per-L windows; each M then independently
    // selects R (the nearest refresh access before M, itself content-
    // independent) and its own enclosing [L, nextL) window.
    size_t idx = 0;
    for (size_t i = std::max<size_t>(field->begin, 1); i < field->end && i < c.video.size(); ++i) {
        if (!is_white(c.video[i]) || is_white(c.video[i - 1])) continue;
        size_t M = i;

        auto lit = std::upper_bound(edges.begin(), edges.end(), M);
        if (lit == edges.begin() || lit == edges.end()) { ++idx; continue; }
        size_t L = *(lit - 1), nextL = *lit;
        if (L < field->begin || nextL > field->end) { ++idx; continue; }

        auto rit = std::upper_bound(c.refresh.begin(), c.refresh.end(), (uint64_t)M);
        if (rit == c.refresh.begin()) { ++idx; continue; }
        uint64_t R = *(rit - 1);

        LineMetric m = measure_span(c.video, L, nextL, pulse_is_zero, R);
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
    // anything else reads one. MUTATION FIXTURE: see header.
    HCapture fixture_cap = (mv == "FIXTURE") ? run_h(dir + "/prog_h_h1_fixture.bin", default_cfg()) : h1;
    checks.expect(fixture_cap.loaded && fixture_cap.mark1_peek_ok && fixture_cap.mark1_peek == 0xA5A5,
        "FIXTURE: peek_logical(MARK1) [H1] == $A5A5, the literal Program H stores before anything else is read back");
    if (mv == "FIXTURE")
        checks.expect(fixture_cap.loaded && fixture_cap.done.has_value(),
            "DONE [H1,FIXTURE]: the program reaches DONE");

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
    checks.record(
        "opus:8-appA-oos.c4-WITHDRAWN [p280]: HSSTOP -- the claim bundles "
        "write-only, Agnus, ECS-mark and the address with the stop-position "
        "functional effect (SPECIFICATION.md sec.1, \"Parts of c4, c5 and c6 "
        "that have no proposition of their own\"); none of the bundled parts "
        "is independently observable. P4 grades the functional effect alone, "
        "unlabelled.");
    checks.record(
        "opus:8-appA-oos.c5-WITHDRAWN [p280]: HSSTRT -- as c4, for the "
        "start-position effect. P3 grades the functional effect alone, "
        "unlabelled.");
    checks.record(
        "opus:8-appA-oos.c6-WITHDRAWN [p280]: HTOTAL -- as c4, for the "
        "highest-count effect. P1 and P2 grade the functional effect alone, "
        "unlabelled.");

    // ---- PC-ID, PC-MARK, DONE: shared preconditions, every H-group run --
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
            checks.expect(r.c->loaded && r.c->done.has_value(),
                "DONE [" + std::string(r.label) + "]: the program reaches DONE");
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
        checks.expect(cap.loaded && cap.done.has_value(), "DONE [H1,X-OCS]: the program reaches DONE");
    }
    if (mv == "X-NTSC") {
        HCapture cap = run_h(dir + "/prog_h_h1.bin", ntsc_cfg());
        checks.expect(cap.loaded && pc_id_pal(cap.id),
            "PC-ID [H1, X-NTSC] p.299: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
        checks.expect(cap.loaded && cap.done.has_value(), "DONE [H1,X-NTSC]: the program reaches DONE");
    }
    if (mv == "X-HALT-H") {
        HCapture cap = run_h(dir + "/prog_h_h1_halt.bin", default_cfg());
        checks.expect(cap.loaded && cap.mark1.has_value() && cap.mark2.has_value(),
            "PC-MARK [H1, X-HALT-H]: MARK1 appears at the port, and MARK2 later");
        checks.expect(cap.loaded && cap.done.has_value(),
            "DONE [H1,X-HALT-H]: the program reaches DONE (the marker is omitted, not the rest of the program)");
    }

    // X-SHORT/X-BURST substitute an alternate capture for one specific
    // precondition (P1-LINES/P2-LINES and P1-COVER respectively); nothing
    // else in this run changes. Their own liveness (PC-ID, PC-MARK, DONE)
    // is asserted here too, since these captures are otherwise untested.
    HCapture h1_short, h1_burst, h2_burst;
    if (mv == "X-SHORT") {
        h1_short = run_h(dir + "/prog_h_h1_short.bin", default_cfg());
        checks.expect(h1_short.loaded && pc_id_pal(h1_short.id),
            "PC-ID [H1,X-SHORT]: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
        checks.expect(h1_short.loaded && h1_short.mark1.has_value() && h1_short.mark2.has_value(),
            "PC-MARK [H1,X-SHORT]: MARK1 appears at the port, and MARK2 later");
        checks.expect(h1_short.loaded && h1_short.done.has_value(), "DONE [H1,X-SHORT]: the program reaches DONE");
    }
    if (mv == "X-BURST") {
        h1_burst = run_h(dir + "/prog_h_h1_burst.bin", default_cfg());
        h2_burst = run_h(dir + "/prog_h_h2_burst.bin", default_cfg());
        checks.expect(h1_burst.loaded && pc_id_pal(h1_burst.id),
            "PC-ID [H1,X-BURST]: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
        checks.expect(h1_burst.loaded && h1_burst.mark1.has_value() && h1_burst.mark2.has_value(),
            "PC-MARK [H1,X-BURST]: MARK1 appears at the port, and MARK2 later");
        checks.expect(h1_burst.loaded && h1_burst.done.has_value(), "DONE [H1,X-BURST]: the program reaches DONE");
        checks.expect(h2_burst.loaded && pc_id_pal(h2_burst.id),
            "PC-ID [H2,X-BURST]: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
        checks.expect(h2_burst.loaded && h2_burst.mark1.has_value() && h2_burst.mark2.has_value(),
            "PC-MARK [H2,X-BURST]: MARK1 appears at the port, and MARK2 later");
        checks.expect(h2_burst.loaded && h2_burst.done.has_value(), "DONE [H2,X-BURST]: the program reaches DONE");
    }

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

    // ---- P2 analysis for all four H-group runs, and anchors A1/A3, ------
    // established before any check that consumes kCck (P1-COVER, P2-LINES,
    // A2, P2(a)/(b)) so those checks can gate on A3 (sec.9 below).
    P2Data d1 = analyze_p2(h1), d2 = analyze_p2(h2), d1c = analyze_p2(h1c), d2c = analyze_p2(h2c);

    // ---- Anchor A1: in H1c/H2c (VARBEAMEN=0), the largest graded H lies
    // between $E2-16 and $E3 (p.23). MUTATION A1 stimulus red
    // NMH_MUTATE=A1: see header -- one substitution, six printed lines.
    Extracted ex_h1c_check = extract(h1c), ex_h2c_check = extract(h2c);
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

    // ---- Anchor A3: establishes kCck. Every graded period (not just the
    // first) must be exactly divisible by kCck and convert to 227 or 228
    // CCK (p.23). MUTATION A3 stimulus red NMH_MUTATE=A3: see header.
    bool a3_h1c_ok, a3_h2c_ok, a3_h1c_ok_real, a3_h2c_ok_real;
    {
        const auto& periods = (mv == "A3") ? d1.periods : d1c.periods;
        a3_h1c_ok = pc_h1c && a3_all_ok(periods);
        checks.expect(a3_h1c_ok,
            "A3 [H1c] [p23,p194,p304]: every graded period (VARBEAMEN=0) is exactly divisible by "
            "the clk_sys-per-CCK ratio and converts to 227 or 228 CCK");
        a3_h1c_ok_real = pc_h1c && a3_all_ok(d1c.periods);
    }
    {
        const auto& periods = (mv == "A3") ? d2.periods : d2c.periods;
        a3_h2c_ok = pc_h2c && a3_all_ok(periods);
        checks.expect(a3_h2c_ok,
            "A3 [H2c] [p23,p194,p304]: every graded period (VARBEAMEN=0) is exactly divisible by "
            "the clk_sys-per-CCK ratio and converts to 227 or 228 CCK");
        a3_h2c_ok_real = pc_h2c && a3_all_ok(d2c.periods);
    }

    // ---- Preconditions for P1: P1-LINES [H1]/[H2] (kCck-independent), ---
    // then P1-COVER [H1]/[H2], gated on A3 (sec.9: every kCck consumer
    // records "precondition not met" unless A3 holds).
    Extracted ex_h1 = extract(h1), ex_h2 = extract(h2);

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
                              uint16_t N, bool kcck_precond, bool& real_ok) -> bool {
        Extracted ex = (mv == "X-BURST") ? extract(burst_cap) : ex_normal;
        bool have_ex = ex.ok;
        uint64_t max_gap = 0;
        bool near_end = false;
        if (have_ex) {
            size_t gs = ex.wraps[1];
            for (size_t i = gs + 1; i < ex.cyc.size(); ++i)
                max_gap = std::max(max_gap, ex.cyc[i] - ex.cyc[i - 1]);
            for (size_t i = gs; i < ex.hf.size(); ++i)
                if (ex.hf[i] >= (uint16_t)(N - 16)) near_end = true;
            checks.note(std::string("P1-COVER [") + label + "]: largest consecutive-sample gap in "
                        "the graded range = " + std::to_string(max_gap / kCck) + " CCK");
        }
        bool cover_ok = false;
        if (!kcck_precond) {
            checks.record(std::string("P1-COVER [") + label + "]: precondition not met (the kCck "
                          "conversion anchor A3 does not hold)");
        } else {
            cover_ok = have_ex && (max_gap <= 16 * kCck) && near_end;
            checks.expect(cover_ok,
                std::string("P1-COVER [") + label + "]: consecutive graded sample stores are never "
                "more than 16 CCK apart, and at least one graded sample has H >= N-16");
        }
        if (ex_normal.ok) {
            size_t gs2 = ex_normal.wraps[1];
            uint64_t max_gap2 = 0;
            for (size_t i = gs2 + 1; i < ex_normal.cyc.size(); ++i)
                max_gap2 = std::max(max_gap2, ex_normal.cyc[i] - ex_normal.cyc[i - 1]);
            bool near_end2 = false;
            for (size_t i = gs2; i < ex_normal.hf.size(); ++i)
                if (ex_normal.hf[i] >= (uint16_t)(N - 16)) near_end2 = true;
            real_ok = kcck_precond && (max_gap2 <= 16 * kCck) && near_end2;
        } else {
            real_ok = false;
        }
        return cover_ok;
    };
    bool p1cover_h1_real = false, p1cover_h2_real = false;
    p1_cover_check("H1", ex_h1, h1_burst, 0x00B0, a3_h1c_ok_real, p1cover_h1_real);
    p1_cover_check("H2", ex_h2, h2_burst, 0x00C0, a3_h2c_ok_real, p1cover_h2_real);

    // ---- Preconditions for P2: P2-LINES [H1]/[H2], gated on A3. ---------
    bool p2lines_h1 = false, p2lines_h2 = false;
    P2Data d1_for_lines = (mv == "X-SHORT") ? analyze_p2(h1_short) : d1;
    if (!a3_h1c_ok_real) {
        checks.record("P2-LINES [H1]: precondition not met (A3 [H1c] does not hold)");
    } else {
        p2lines_h1 = d1_for_lines.ok && d1_for_lines.periods.size() >= 320;
        checks.expect(p2lines_h1, "P2-LINES [H1]: at least 320 graded periods");
    }
    bool p2lines_h1_real = a3_h1c_ok_real && d1.ok && d1.periods.size() >= 320;
    if (!a3_h2c_ok_real) {
        checks.record("P2-LINES [H2]: precondition not met (A3 [H2c] does not hold)");
    } else {
        p2lines_h2 = d2.ok && d2.periods.size() >= 320;
        checks.expect(p2lines_h2, "P2-LINES [H2]: at least 320 graded periods");
    }
    bool p2lines_h2_real = a3_h2c_ok_real && p2lines_h2;

    // ---- Anchor A2: every graded period has exactly 4 refresh accesses,
    // gated on A3 (kCck feeds refresh_groups()'s 32-CCK clustering bound).
    // MUTATION A2 stimulus red NMH_MUTATE=X-AUD0: see header -- all four
    // H-group runs, each with its own audio-DMA capture.
    HCapture h1_aud0, h2_aud0, h1c_aud0, h2c_aud0;
    if (mv == "X-AUD0") {
        h1_aud0  = run_h(dir + "/prog_h_h1_aud0.bin",  default_cfg());
        h2_aud0  = run_h(dir + "/prog_h_h2_aud0.bin",  default_cfg());
        h1c_aud0 = run_h(dir + "/prog_h_h1c_aud0.bin", default_cfg());
        h2c_aud0 = run_h(dir + "/prog_h_h2c_aud0.bin", default_cfg());
        checks.expect(h1_aud0.loaded  && h1_aud0.done.has_value(),  "DONE [H1,X-AUD0]: the program reaches DONE");
        checks.expect(h2_aud0.loaded  && h2_aud0.done.has_value(),  "DONE [H2,X-AUD0]: the program reaches DONE");
        checks.expect(h1c_aud0.loaded && h1c_aud0.done.has_value(), "DONE [H1c,X-AUD0]: the program reaches DONE");
        checks.expect(h2c_aud0.loaded && h2c_aud0.done.has_value(), "DONE [H2c,X-AUD0]: the program reaches DONE");
    }
    auto a2_check = [&](const char* label, P2Data& d, bool precond, const char* why) -> bool {
        if (!precond) {
            checks.record(std::string("A2 [") + label + "]: precondition not met" + why);
            return false;
        }
        bool ok = d.ok && d.a2_all_four;
        checks.expect(ok,
            "A2 [" + std::string(label) + "] [p194,p285]: every graded period contains exactly "
            "4 refresh accesses");
        return ok;
    };
    P2Data d1_for_a2  = (mv == "X-AUD0") ? analyze_p2(h1_aud0)  : d1;
    P2Data d2_for_a2  = (mv == "X-AUD0") ? analyze_p2(h2_aud0)  : d2;
    P2Data d1c_for_a2 = (mv == "X-AUD0") ? analyze_p2(h1c_aud0) : d1c;
    P2Data d2c_for_a2 = (mv == "X-AUD0") ? analyze_p2(h2c_aud0) : d2c;
    bool a2_h1  = a2_check("H1",  d1_for_a2,  a3_h1c_ok_real, " (A3 [H1c] does not hold)");
    bool a2_h2  = a2_check("H2",  d2_for_a2,  a3_h2c_ok_real, " (A3 [H2c] does not hold)");
    bool a2_h1c = a2_check("H1c", d1c_for_a2, a3_h1c_ok_real, " (A3 [H1c] does not hold)");
    bool a2_h2c = a2_check("H2c", d2c_for_a2, a3_h2c_ok_real, " (A3 [H2c] does not hold)");
    (void)a2_h1; (void)a2_h2; (void)a2_h1c; (void)a2_h2c;

    uint64_t T1_raw = d1.ok && !d1.periods.empty() ? d1.periods[0] : 0;
    uint64_t T2_raw = d2.ok && !d2.periods.empty() ? d2.periods[0] : 0;
    uint64_t T1c_raw = d1c.ok && !d1c.periods.empty() ? d1c.periods[0] : 0;
    uint64_t T2c_raw = d2c.ok && !d2c.periods.empty() ? d2c.periods[0] : 0;

    // ---- P1: with VARBEAMEN set, no H readback exceeds HTOTAL ------------
    // opus:8-appA-oos.c6 bundles this with address/write-only/Agnus/
    // ECS-mark clauses SPECIFICATION.md sec.1 declines, so no id is
    // carried here. MUTATION: see NMH_MUTATE=A1 in the header -- one
    // substitution feeds all four of these lines.
    {
        struct Run { const char* label; HCapture* cap; HCapture* mutate_cap; uint16_t N; bool contrast; bool precond; };
        Run runs[] = {{"H1", &h1, &h1c, 0x00B0, false, pc_h1 && p1lines_h1_real && p1cover_h1_real},
                      {"H2", &h2, &h2c, 0x00C0, false, pc_h2 && p1lines_h2_real && p1cover_h2_real},
                      {"H1c", &h1c, &h1, 0x00B0, true, pc_h1c && a1_h1c_ok_real},
                      {"H2c", &h2c, &h2, 0x00C0, true, pc_h2c && a1_h2c_ok_real}};
        for (auto& r : runs) {
            Extracted ex = extract((mv == "A1") ? *r.mutate_cap : *r.cap);
            if (!ex.ok) {
                checks.expect(false, std::string("P1 [") + r.label + "] [p280]: "
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
                    "P1 [" + std::string(r.label) + "] [p280]: while VARBEAMEN is 1, no graded "
                    "horizontal-position readback exceeds HTOTAL");
            } else {
                checks.expect(any_exceeds,
                    "P1 [" + std::string(r.label) + "] [p280] (contrast, VARBEAMEN=0): at least one "
                    "graded readback exceeds N, since HTOTAL's condition does not hold");
            }
        }
    }

    // ---- P2: with VARBEAMEN set, every line lasts N or N+1 CCK -----------
    // No single graded line establishes the whole proposition (each of
    // P2(a)/(b)/(c) grades one clause only); none of them carries
    // opus:8-appA-oos.c6 (sec.1). The conjunction below is the whole
    // proposition, still unlabelled for the same reason.
    {
        // P2(a) [H1]. MUTATION P2(a) [H1] stimulus red NMH_MUTATE=P2A: see header.
        HCapture h1_retotal;
        if (mv == "P2A") {
            h1_retotal = run_h(dir + "/prog_h_h1_retotal.bin", default_cfg());
            checks.expect(h1_retotal.loaded && h1_retotal.done.has_value(),
                "DONE [H1,P2A]: the program reaches DONE");
        }
        P2Data d1_for_a = (mv == "P2A") ? analyze_p2(h1_retotal) : d1;
        if (mv == "P2A" && d1_for_a.ok)
            checks.note("P2 [H1,P2A]: " + std::to_string(d1_for_a.periods.size()) + " graded periods, first=" +
                        (d1_for_a.periods.empty() ? std::string("n/a") : std::to_string(d1_for_a.periods[0])) +
                        " clk_sys cycles");
        if (d1.ok && !d1.periods.empty())
            checks.note("P2 [H1]: " + std::to_string(d1.periods.size()) + " graded periods, first=" +
                        std::to_string(d1.periods[0]) + " clk_sys cycles");
        bool p2a_h1_ok = false;
        if (!(pc_h1 && p2lines_h1_real)) {
            checks.record("P2(a) [H1]: precondition not met");
        } else {
            p2a_h1_ok = d1_for_a.ok && !d1_for_a.periods.empty() && all_equal(d1_for_a.periods);
            checks.expect(p2a_h1_ok,
                "P2(a) [H1] [p280,p304]: while VARBEAMEN is 1, every graded line period has one "
                "and the same value");
        }

        if (d2.ok && !d2.periods.empty())
            checks.note("P2 [H2]: " + std::to_string(d2.periods.size()) + " graded periods, first=" +
                        std::to_string(d2.periods[0]) + " clk_sys cycles");
        bool p2a_h2_ok = false;
        if (!(pc_h2 && p2lines_h2_real)) {
            checks.record("P2(a) [H2]: precondition not met");
        } else {
            p2a_h2_ok = d2.ok && !d2.periods.empty() && all_equal(d2.periods);
            checks.expect(p2a_h2_ok,
                "P2(a) [H2] [p280,p304]: while VARBEAMEN is 1, every graded line period has one "
                "and the same value");
        }

        if (d1c.ok && !d1c.periods.empty())
            checks.note("P2 [H1c]: " + std::to_string(d1c.periods.size()) + " graded periods, first=" +
                        std::to_string(d1c.periods[0]) + " clk_sys cycles");
        if (d2c.ok && !d2c.periods.empty())
            checks.note("P2 [H2c]: " + std::to_string(d2c.periods.size()) + " graded periods, first=" +
                        std::to_string(d2c.periods[0]) + " clk_sys cycles");

        bool have_t1t2 = pc_h1 && pc_h2 && p2lines_h1_real && p2lines_h2_real &&
                         d1.ok && !d1.periods.empty() && d2.ok && !d2.periods.empty();

        // P2(b). MUTATION P2(b) stimulus red NMH_MUTATE=P2B: see header.
        uint64_t t1_for_b = (mv == "P2B") ? T1c_raw : T1_raw;
        uint64_t t2_for_b = (mv == "P2B") ? T2c_raw : T2_raw;
        bool p2b_ok = false;
        if (!have_t1t2) {
            checks.record("P2(b): precondition not met");
        } else {
            p2b_ok = (int64_t)(t2_for_b - t1_for_b) == 16 * (int64_t)kCck;
            checks.expect(p2b_ok, "P2(b) [p280,p304]: T2 - T1 = 16 CCK ($C0 - $B0)");
        }

        // P2(c). MUTATION P2(c) stimulus red NMH_MUTATE=P2C: see header.
        bool p2c_ok = false;
        if (!have_t1t2) {
            checks.record("P2(c): precondition not met");
        } else if (!a3_h1c_ok_real) {
            checks.record("P2(c): precondition not met (A3 [H1c], the unit-conversion anchor, does not hold)");
        } else {
            uint64_t t1_for_c = (mv == "P2C") ? T1c_raw : T1_raw;
            uint64_t t1_cck = t1_for_c / kCck;
            p2c_ok = t1_for_c != 0 && (t1_cck >= 0x00B0) && (t1_cck - 0x00B0 <= 1);
            checks.expect(p2c_ok,
                "P2(c) [p280,p304]: T1 (converted to CCK) - $B0 is 0 or 1 (page note 2)");
        }

        // Conjunction: the whole proposition, held only when P2(a) [H1],
        // P2(a) [H2], P2(b) and P2(c) all hold together.
        if (!(pc_h1 && pc_h2 && p2lines_h1_real && p2lines_h2_real && a3_h1c_ok_real)) {
            checks.record("P2: precondition not met");
        } else {
            checks.expect(p2a_h1_ok && p2a_h2_ok && p2b_ok && p2c_ok,
                "P2 [p280,p304]: while VARBEAMEN is 1, every line has one and the same length, that "
                "length is N or N+1 CCK for HTOTAL=N, and changing HTOTAL by d changes the length by "
                "exactly d (conjunction of P2(a) [H1], P2(a) [H2], P2(b), P2(c))");
        }

        // P2 contrast: every graded period in H1c equals every graded period
        // in H2c (not just periods[0]), through the unenabled comparator.
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

    // ---- PC-ID, PC-MARK, DONE: shared preconditions, every S-group run --
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
            checks.expect(r.c->loaded && r.c->done.has_value(),
                "DONE [" + std::string(r.label) + "]: the program reaches DONE");
            *r.pc = id_ok && mark_ok;
        }
    }
    if (mv == "X-HALT-S") {
        SCapture cap = run_s(dir + "/prog_s_sa_halt.bin", default_cfg());
        checks.expect(cap.loaded && cap.mark1.has_value(),
            "PC-MARK [SA, X-HALT-S]: MARK1 appears at the port");
        checks.expect(cap.loaded && cap.done.has_value(),
            "DONE [SA,X-HALT-S]: the program reaches DONE (the marker is omitted, not the rest of the program)");
    }

    // ---- Shared: graded lines per run, X-NOBPL substituted for SA when mutating ----
    SCapture sa_nobpl;
    if (mv == "X-NOBPL") {
        sa_nobpl = run_s(dir + "/prog_s_sa_nobpl.bin", default_cfg());
        checks.expect(sa_nobpl.loaded && pc_id_pal(sa_nobpl.id),
            "PC-ID [SA,X-NOBPL]: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
        checks.expect(sa_nobpl.loaded && sa_nobpl.mark1.has_value(),
            "PC-MARK [SA,X-NOBPL]: MARK1 appears at the port");
        checks.expect(sa_nobpl.loaded && sa_nobpl.done.has_value(), "DONE [SA,X-NOBPL]: the program reaches DONE");
    }
    const SCapture& sa_for_lines = (mv == "X-NOBPL") ? sa_nobpl : sa;

    GradedLines gsa = graded_lines_of(sa_for_lines), gsb = graded_lines_of(sb), gsc = graded_lines_of(sc);
    GradedLines gsa0 = graded_lines_of(sa0), gsb0 = graded_lines_of(sb0), gsc0 = graded_lines_of(sc0);
    // The real (unmutated) SA capture, used for gating and for checks that
    // are not X-NOBPL's own target.
    GradedLines gsa_real = graded_lines_of(sa);
    if (mv == "X-NOBPL")
        checks.note("P3/P4-LINES [SA,X-NOBPL]: " + std::to_string(gsa.total) +
                    " graded output lines (live, reduced population)");

    // ---- P3/P4-LINES: p.64's programmed playfield is 256 chip lines, and
    // the harness doubles each into two output lines, so the exact
    // page/harness-anchored population is 256 lines in each parity class.
    bool p34lines_sa, p34lines_sb, p34lines_sc, p34lines_sa0, p34lines_sb0, p34lines_sc0;
    {
        struct Row { const char* label; GradedLines* g; bool* ok; };
        Row rows[] = {{"SA", &gsa, &p34lines_sa}, {"SB", &gsb, &p34lines_sb}, {"SC", &gsc, &p34lines_sc},
                      {"SA0", &gsa0, &p34lines_sa0}, {"SB0", &gsb0, &p34lines_sb0}, {"SC0", &gsc0, &p34lines_sc0}};
        for (auto& r : rows) {
            checks.note(std::string("P3/P4-LINES [") + r.label + "]: " + std::to_string(r.g->cls[0].size()) +
                        " class-0, " + std::to_string(r.g->cls[1].size()) + " class-1, " +
                        std::to_string(r.g->total) + " total graded output lines");
            *r.ok = r.g->ok && r.g->cls[0].size() == 256 && r.g->cls[1].size() == 256;
            checks.expect(*r.ok,
                "P3/P4-LINES [" + std::string(r.label) + "] [p64]: exactly 256 graded output lines "
                "in each parity class (512 total)");
        }
    }
    // P34-LINES [SA]'s real (unmutated) counterpart, for gating P3/P4: a
    // precondition mutation (X-NOBPL) must not also silently gate the
    // proposition itself, which is graded on SA's own real capture.
    bool p34lines_sa_real = gsa_real.ok && gsa_real.cls[0].size() == 256 && gsa_real.cls[1].size() == 256;

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
        a4_ok = all_singletons && (wr.empty() ? false : all_equal_i64(wr));
        white_run = wr.empty() ? -1 : median_of(wr);
        return cons;
    };
    // Anchor A5 helper: the landmark M's own median distance to R (the
    // nearest memory-refresh access before it) within a class.
    auto class_dmr = [&](std::vector<LineMetric>& cls, int64_t& med_dMR) -> bool {
        if (cls.empty()) return false;
        std::vector<int64_t> v;
        for (auto& m : cls) v.push_back(m.dMR);
        med_dMR = median_of(v);
        return consistent(v);
    };

    bool precond_s_all = pc_sa && pc_sb && pc_sc && pc_sa0 && pc_sb0 && pc_sc0;

    // ---- Anchor A4: line well-formedness and white-run-width consistency,
    // all six S runs together (SPECIFICATION.md P3 sec.7). Exact invariant:
    // the pages give no tolerance band for the white run's width, so it
    // must match exactly, not to within an invented +-1. MUTATION A4
    // stimulus red NMH_MUTATE=X-A4DOUBLE: see header.
    {
        SCapture sa_a4d;
        if (mv == "X-A4DOUBLE") {
            sa_a4d = run_s(dir + "/prog_s_sa_a4double.bin", default_cfg());
            checks.expect(sa_a4d.loaded && sa_a4d.done.has_value(), "DONE [SA,X-A4DOUBLE]: the program reaches DONE");
        }
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
            if (all_ok && all_equal_i64(wrs)) green_any = true;
        }
        if (!precond_s_all) {
            checks.record("A4: precondition not met");
        } else {
            checks.expect(green_any,
                "A4 [p59,p60]: every graded line has exactly one trailing edge between consecutive "
                "leading edges, exactly one transition into white and one out of white, and the "
                "white run's length is exactly the same across all six S runs, in at least one "
                "parity class");
        }
    }

    // ---- A4-DROPPED: the quantifier "every" in A4 is not graded by A4
    // itself, since measure_span()/graded_lines_of() silently drop a
    // white-carrying line that fails measurement instead of grading it red.
    // Counts such drops across all six S runs; requires 0. Not exercised
    // (see header): no page gives HSSTRT/HSSTOP a layout or scale from
    // which to choose a page-predicted, rather than RTL-tuned, stimulus.
    {
        size_t dropped = gsa_real.dropped_white + gsb.dropped_white + gsc.dropped_white +
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
        checks.record(
            "A4-DROPPED-NOT-EXERCISED: no page gives HSSTRT/HSSTOP a layout or scale (page note 7) "
            "from which to choose a stimulus that is page-predicted, rather than tuned against this "
            "RTL, to land the sync pulse on the landmark; not exercised, by a scope decision.");
    }

    // ---- Anchor A5: the landmark M's own position relative to R, the
    // nearest memory-refresh access before it (p.194, p.285) -- a landmark
    // independent of DIWSTRT, DDFSTRT, HSSTRT and HSSTOP alike, used to
    // check that M itself does not move. Graded on the three VARHSYEN=0
    // contrast runs (SA0/SB0/SC0), where nothing is expected to move: if
    // dMR (M-R) is not identical there, the apparatus itself is not
    // precise enough to use as an independent reference at all. It is
    // exact, not to a tolerance the pages do not give.
    bool a5_apparatus_ok = false;
    {
        bool any_class = false;
        for (int cls = 0; cls < 2; ++cls) {
            int64_t mA0, mB0, mC0;
            bool okA0 = class_dmr(gsa0.cls[cls], mA0);
            bool okB0 = class_dmr(gsb0.cls[cls], mB0);
            bool okC0 = class_dmr(gsc0.cls[cls], mC0);
            checks.note("A5 class " + std::to_string(cls) + " (VARHSYEN=0): dMR SA0=" +
                        (okA0 ? std::to_string(mA0) : std::string("n/a")) + " SB0=" +
                        (okB0 ? std::to_string(mB0) : std::string("n/a")) + " SC0=" +
                        (okC0 ? std::to_string(mC0) : std::string("n/a")));
            if (okA0 && okB0 && okC0 && mA0 == mB0 && mB0 == mC0) any_class = true;
        }
        if (!(pc_sa0 && pc_sb0 && pc_sc0)) {
            checks.record("A5: precondition not met");
        } else {
            a5_apparatus_ok = any_class;
            checks.expect(a5_apparatus_ok,
                "A5 [p194,p285]: with VARHSYEN=0 (nothing expected to move), the landmark's distance "
                "to the nearest memory-refresh access is exactly the same in SA0, SB0 and SC0, in at "
                "least one parity class");
        }
        // With VARHSYEN=1, dMR is printed but not graded: SPECIFICATION.md
        // P3 sec.7 records why an independent reference cannot be used to
        // attribute the live SA/SB/SC move to a specific edge at this
        // boundary.
        for (int cls = 0; cls < 2; ++cls) {
            int64_t mA, mB, mC;
            bool okA = class_dmr(gsa_real.cls[cls], mA);
            bool okB = class_dmr(gsb.cls[cls], mB);
            bool okC = class_dmr(gsc.cls[cls], mC);
            checks.note("A5 class " + std::to_string(cls) + " (VARHSYEN=1, not graded): dMR SA=" +
                        (okA ? std::to_string(mA) : std::string("n/a")) + " SB=" +
                        (okB ? std::to_string(mB) : std::string("n/a")) + " SC=" +
                        (okC ? std::to_string(mC) : std::string("n/a")));
        }
        checks.record(
            "A5-NOT-DISPOSITIVE: with VARHSYEN=1, dMR itself changes between SA/SB/SC even though "
            "DIWSTRT/DDFSTRT (hence M's chip-side position) are unchanged, so the harness's video "
            "output does not preserve a fixed relationship between the sync edges and this "
            "independent reference once the sync timing itself changes; A5 cannot attribute P3's or "
            "P4's live move to a specific edge, only confirm the apparatus is exact when nothing "
            "moves (above).");
    }

    // ---- P3: HSSTRT alone sets the pulse's leading edge -------------------
    // dL, dT are the landmark M's own distance to L and to T
    // (SPECIFICATION.md P3 sec.3); anchor A5 (above) confirms the apparatus
    // is exact when nothing moves, but cannot attribute which of L or M
    // moved once HSSTRT genuinely changes the sync timing (recorded above,
    // A5-NOT-DISPOSITIVE). The pages give no size or direction for the
    // move, so it is graded as exact inequality, and the untouched edge as
    // exact equality (sec.6 point 4). MUTATION P3 stimulus red
    // NMH_MUTATE=P3-NOMOVE: SB's graded lines replaced by SA's own (s
    // equal to SA's, so no move).
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
                bool b_ok = medLB != medLA;
                bool c_ok = medTB == medTA;
                bool contrast_ok = medLB0 == medLA0 && medTB0 == medTA0;
                checks.note("P3 class " + std::to_string(cls) + ": dL(SA)=" + std::to_string(medLA) +
                            " dL(SB)=" + std::to_string(medLB) + " dT(SA)=" + std::to_string(medTA) +
                            " dT(SB)=" + std::to_string(medTB) + " dL(SA0)=" + std::to_string(medLA0) +
                            " dL(SB0)=" + std::to_string(medLB0) + " dT(SA0)=" + std::to_string(medTA0) +
                            " dT(SB0)=" + std::to_string(medTB0));
                if (b_ok && c_ok && contrast_ok && a4a && a4b && a4a0 && a4b0) green_any = true;
            }
            checks.expect(green_any,
                "P3 [p280,p304,p305]: while VARHSYEN and VARBEAMEN are both 1, dL (the landmark's "
                "distance to the leading edge) differs between SA and SB (exact inequality) and dT "
                "(its distance to the trailing edge) is exactly unchanged, and with VARHSYEN=0 "
                "neither changes, in at least one parity class -- A5 records why this cannot also "
                "attribute the difference to the leading edge specifically");
        }
    }

    // ---- P4: HSSTOP alone sets the pulse's trailing edge -----------------
    // As P3, with T the moved edge and L the untouched one. MUTATION P4
    // stimulus red NMH_MUTATE=P4-NOMOVE: SC's graded lines replaced by
    // SA's own (e equal to SA's, so no move).
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
                bool b_ok = medTC != medTA;
                bool c_ok = medLC == medLA;
                bool contrast_ok = medTC0 == medTA0 && medLC0 == medLA0;
                checks.note("P4 class " + std::to_string(cls) + ": dT(SA)=" + std::to_string(medTA) +
                            " dT(SC)=" + std::to_string(medTC) + " dL(SA)=" + std::to_string(medLA) +
                            " dL(SC)=" + std::to_string(medLC) + " dT(SA0)=" + std::to_string(medTA0) +
                            " dT(SC0)=" + std::to_string(medTC0) + " dL(SA0)=" + std::to_string(medLA0) +
                            " dL(SC0)=" + std::to_string(medLC0));
                if (b_ok && c_ok && contrast_ok && a4a && a4c && a4a0 && a4c0) green_any = true;
            }
            checks.expect(green_any,
                "P4 [p280,p304,p305]: while VARHSYEN and VARBEAMEN are both 1, dT (the landmark's "
                "distance to the trailing edge) differs between SA and SC (exact inequality) and dL "
                "(its distance to the leading edge) is exactly unchanged, and with VARHSYEN=0 "
                "neither changes, in at least one parity class -- A5 records why this cannot also "
                "attribute the difference to the trailing edge specifically");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
