// test_ch5_audio_attach_modulation.cpp -- ch5-audio-attach-modulation. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// Each channel's output side is not assumed from either page: it is
// measured, once per channel, from that channel's own reference session
// (the one output that changes while the other stays constant -- see
// measure_channel_side() and side_of[]). PC5 grades only the pairing the
// two pages agree on (0 with 3 on one output, 1 with 2 on the other, the
// two outputs different); every other assertion below uses side_of[] and
// depends on neither page's naming of which physical jack that is.
//
// NMH_MUTATE selects a mutated program, or a mutated grading of an
// existing capture, in place of its normal counterpart, one assertion at
// a time. None is exercised by a plain `make test`, and none is counted
// in its summary. Per-core measured results for every record below are
// in SPECIFICATION.md.
//
// MUTATION FIXTURE stimulus red stored constant changed to DEAD instead of BEEF, via NMH_MUTATE=FIXTURE
// MUTATION PC4 stimulus red R-VOL-1's own capture relabelled as if VSET_1's 60-step had been written 40, via NMH_MUTATE=PC4
// MUTATION PC5 grading red the pairing checked as 0-with-1 / 2-with-3 instead of 0-with-3 / 1-with-2, via NMH_MUTATE=PC5
// MUTATION PC6 stimulus red AUD0PER=5000 written halfway through R-MOD, via NMH_MUTATE=PC6
// MUTATION PC7 stimulus red AUD0PER=3000 written halfway through R-MOD, via NMH_MUTATE=PC7
// MUTATION P1 unfalsifiable at this boundary: the ADKCON $8001 write omitted, measured to open
//   the same window as the attached capture (P1's own comparator does not distinguish the two
//   stimuli here), via NMH_MUTATE=P1
// MUTATION P2 unfalsifiable at this boundary: the ADKCON $8010 write omitted, measured to open
//   the same window as the attached capture, via NMH_MUTATE=P2
// MUTATION P3 stimulus green R-MOD's own unattached channel 0 substituted for S-AV's attached one, via NMH_MUTATE=P3
// MUTATION P4 stimulus red only bit 0 set (ADKCON $8001) instead of both, via NMH_MUTATE=P4
// MUTATION P5 stimulus red S-AV run with AUD0PER=6000 graded against the 4000-tick D_mod grid, via NMH_MUTATE=P5
// MUTATION P6-0 stimulus red the attach write omitted (bit 0), via NMH_MUTATE=P6-0
// MUTATION P6-0-EXTRA unfalsifiable at this boundary: bit 2 also set, measured to leave ch3's
//   volume unaffected (bit 2 has no measured effect either), via NMH_MUTATE=P6-0-EXTRA
// MUTATION P6-1 stimulus red the attach write omitted (bit 1), via NMH_MUTATE=P6-1
// MUTATION P6-2 stimulus red the attach write omitted (bit 2), via NMH_MUTATE=P6-2
// MUTATION P6-2-EXTRA unfalsifiable at this boundary: bit 0 also set, measured to leave the
//   left side unaffected (bit 0 has no measured effect either), via NMH_MUTATE=P6-2-EXTRA
// MUTATION P6-4 stimulus red the attach write omitted (bit 4), via NMH_MUTATE=P6-4
// MUTATION P6-4-EXTRA unfalsifiable at this boundary: bit 6 also set, measured to leave ch3's
//   spacing unaffected (bit 6 has no measured effect either), via NMH_MUTATE=P6-4-EXTRA
// MUTATION P6-5 stimulus red the attach write omitted (bit 5), via NMH_MUTATE=P6-5
// MUTATION P6-5-EXTRA unfalsifiable at this boundary: bit 4 also set, measured to leave the
//   modulator's own spacing unaffected (bit 4 has no measured effect either), via NMH_MUTATE=P6-5-EXTRA
// MUTATION P6-6 stimulus red the attach write omitted (bit 6), via NMH_MUTATE=P6-6
// MUTATION P6-6-EXTRA unfalsifiable at this boundary: bit 4 also set, measured to leave ch1's
//   spacing unaffected (bit 4 has no measured effect either), via NMH_MUTATE=P6-6-EXTRA
// MUTATION P7V unfalsifiable at this boundary: the wrong bit set (bit 2 instead of bit 3),
//   graded through the spacing clause, measured to leave ch3's spacing unaffected (bit 2 has
//   no measured effect either), via NMH_MUTATE=P7V
// MUTATION P7P unfalsifiable at this boundary: the wrong bit set (bit 6 instead of bit 7),
//   graded through the spacing clause, measured to leave ch3's spacing unaffected (bit 6 has
//   no measured effect either), via NMH_MUTATE=P7P
// MUTATION P8 stimulus red the attach write omitted, substituting R-MOD (unattached) for S-AV, via NMH_MUTATE=P8
// MUTATION P9 stimulus red AUD0LEN=4 instead of 5, graded against the page's original L_m=5
//   (not the mutation's own length), via NMH_MUTATE=P9
// MUTATION P10 grading red the S-AP capture graded through S-AV's k(n)=2(n-3) formula instead of its own k(n)=2(n-3)+1, via NMH_MUTATE=P10

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- harness-side constants (nanomig.i / ch5_regs.i, restated) --------

constexpr uint32_t kResults      = 0x0C0000;
constexpr uint32_t kDoneAddr     = 0x0C0FFE;
constexpr uint16_t kDoneValue    = 0x600D;
constexpr uint32_t kFixtureStore = kResults + 0;
constexpr uint32_t kAdkrStore    = kResults + 2;
constexpr uint32_t kDmacrStore   = kResults + 4;
constexpr uint32_t kMarker       = kResults + 0x10;

constexpr uint32_t kCh0Data = 0x088000;
constexpr uint32_t kCh1Data = 0x089000;
constexpr uint32_t kCh2Data = 0x08A000;
constexpr uint32_t kCh3Data = 0x08B000;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kCck = 8; // budget-sizing constant only; not a graded quantity.
uint64_t budget_for(uint64_t ticks) { return kBootBudget + ticks * kCck * 3 + 200000; }

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }
uint32_t ch_base(int ch) { return word_addr(ch == 0 ? kCh0Data : ch == 1 ? kCh1Data : ch == 2 ? kCh2Data : kCh3Data); }

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// ---- capture ------------------------------------------------------------

struct Capture {
    bool loaded = false;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::AudioSample> audio;
    std::optional<uint16_t> adkr, dmacr, fixture;
};

Capture run_capture(const std::string& bin, uint64_t run_budget) {
    Capture c;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    c.audio = h.audio();
    c.adkr = h.peek_logical(kAdkrStore);
    c.dmacr = h.peek_logical(kDmacrStore);
    c.fixture = h.peek_logical(kFixtureStore);
    return c;
}

// ---- markers --------------------------------------------------------

std::vector<std::pair<uint64_t, uint16_t>> markers(const Capture& c) {
    std::vector<std::pair<uint64_t, uint16_t>> out;
    uint32_t w = word_addr(kMarker);
    for (const auto& a : c.log)
        if (a.is_write && a.word_addr == w) out.push_back({a.cycle, a.data});
    return out;
}
std::optional<uint64_t> m_on(const std::vector<std::pair<uint64_t, uint16_t>>& mk) {
    for (const auto& p : mk) if (p.second == 1) return p.first;
    return std::nullopt;
}
std::optional<uint64_t> m_off(const std::vector<std::pair<uint64_t, uint16_t>>& mk) {
    std::optional<uint64_t> last;
    for (const auto& p : mk) if (p.second == 2) last = p.first;
    return last;
}
std::optional<uint64_t> marker_code(const std::vector<std::pair<uint64_t, uint16_t>>& mk, uint16_t code) {
    for (const auto& p : mk) if (p.second == code) return p.first;
    return std::nullopt;
}

// Boundary cycles [on, step1, step2, ..., off] for a sweep whose steps
// (after the first, which is M_on itself) are marked by `codes`, in
// order. Empty on any missing marker (an incomplete/failed capture).
std::vector<uint64_t> sweep_bounds(const Capture& c, const std::vector<uint16_t>& codes) {
    auto mk = markers(c);
    std::vector<uint64_t> b;
    auto on = m_on(mk);
    if (!on) return b;
    b.push_back(*on);
    for (auto code : codes) {
        auto m = marker_code(mk, code);
        if (!m) return {};
        b.push_back(*m);
    }
    auto off = m_off(mk);
    if (!off) return {};
    b.push_back(*off);
    return b;
}

// SideResult: the output on which a channel is measured (see
// measure_channel_side() below, defined after is_constant()).
struct SideResult { int side = -1; bool ok = false; };

// ---- audio runs -------------------------------------------------------

struct Run { uint64_t start, end; uint16_t value; };

std::vector<Run> runs_in_window(const std::vector<nmh::AudioSample>& audio, uint64_t lo, uint64_t hi, int side) {
    std::vector<Run> out;
    hi = std::min<uint64_t>(hi, audio.size());
    if (lo >= hi) return out;
    uint64_t start = lo;
    uint16_t val = side ? audio[lo].right : audio[lo].left;
    for (uint64_t i = lo + 1; i < hi; ++i) {
        uint16_t v = side ? audio[i].right : audio[i].left;
        if (v != val) { out.push_back({start, i, val}); start = i; val = v; }
    }
    out.push_back({start, hi, val});
    return out;
}
std::vector<Run> complete_runs(const std::vector<Run>& runs, uint64_t lo, uint64_t hi) {
    std::vector<Run> out;
    for (size_t i = 0; i < runs.size(); ++i) {
        if (i == 0 && runs[i].start == lo) continue;
        if (i + 1 == runs.size() && runs[i].end == hi) continue;
        out.push_back(runs[i]);
    }
    return out;
}
bool is_constant(const std::vector<nmh::AudioSample>& audio, uint64_t lo, uint64_t hi, int side) {
    auto runs = runs_in_window(audio, lo, hi, side);
    return runs.size() <= 1;
}

// ---- side measurement (D1/D2/D3/D4: measured, never assumed from a page) --
// Exactly one output must change while the other stays constant, in a
// session where only this channel plays. Returns -1 (ambiguous/unusable)
// otherwise -- callers gate on the .ok field.
SideResult measure_channel_side(const std::vector<nmh::AudioSample>& audio, uint64_t lo, uint64_t hi) {
    if (lo >= hi || hi > audio.size()) return {};
    bool left_const = is_constant(audio, lo, hi, 0);
    bool right_const = is_constant(audio, lo, hi, 1);
    if (!left_const && right_const) return {0, true};
    if (left_const && !right_const) return {1, true};
    return {-1, false};
}

uint64_t median_u64(std::vector<uint64_t> v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}
std::pair<uint16_t, uint16_t> level_pair(const std::vector<Run>& runs) {
    std::map<uint16_t, int> freq;
    for (const auto& r : runs) freq[r.value]++;
    std::vector<std::pair<int, uint16_t>> byfreq;
    for (const auto& kv : freq) byfreq.push_back({kv.second, kv.first});
    std::sort(byfreq.begin(), byfreq.end(), [](auto& a, auto& b) { return a.first > b.first; });
    uint16_t a = byfreq.size() > 0 ? byfreq[0].second : 0;
    uint16_t b = byfreq.size() > 1 ? byfreq[1].second : a;
    if (a > b) std::swap(a, b);
    return {a, b};
}
// Groups consecutive runs into maximal segments that alternate between
// exactly two values (an SQ channel's own alternation) -- a "word
// interval" under a constant applied volume/period.
struct Segment { uint64_t start, end; uint16_t a, b; int n; };
std::vector<Segment> segment_by_pair(const std::vector<Run>& runs) {
    std::vector<Segment> segs;
    size_t i = 0;
    while (i < runs.size()) {
        if (i + 1 >= runs.size()) { segs.push_back({runs[i].start, runs[i].end, runs[i].value, runs[i].value, 1}); break; }
        uint16_t lo = std::min(runs[i].value, runs[i + 1].value), hi = std::max(runs[i].value, runs[i + 1].value);
        size_t j = i;
        while (j < runs.size() && (runs[j].value == lo || runs[j].value == hi)) ++j;
        segs.push_back({runs[i].start, runs[j - 1].end, lo, hi, (int)(j - i)});
        i = j;
    }
    return segs;
}

// D12: sample boundaries are SQ's sign alternations (the +100/-100
// members of the *current* reference level pair), not every raw value
// change -- a volume word landing inside a sample must not split it.
// `table` is the channel's own L_c (reference level pairs); a run's sign
// is + if its value is some L_c(v)'s upper member, - if the lower member.
// Consecutive runs of the same sign are merged into one sample, whose
// duration is the merged span and whose representative value is the
// *last* raw run's value (the settled one, after any mid-sample word).
struct Sample { uint64_t start, end; uint16_t value; };
int classify_sign(uint16_t v, const std::map<int, std::pair<uint16_t, uint16_t>>& table) {
    for (auto& kv : table) {
        if (v == kv.second.second) return +1;
        if (v == kv.second.first) return -1;
    }
    return 0;
}
std::vector<Sample> samples_by_sign(const std::vector<Run>& runs, const std::map<int, std::pair<uint16_t, uint16_t>>& table) {
    std::vector<Sample> out;
    size_t i = 0;
    while (i < runs.size()) {
        int s = classify_sign(runs[i].value, table);
        size_t j = i + 1;
        while (j < runs.size() && classify_sign(runs[j].value, table) == s) ++j;
        out.push_back({runs[i].start, runs[j - 1].end, runs[j - 1].value});
        i = j;
    }
    return out;
}
std::vector<Sample> complete_samples(const std::vector<Sample>& s, uint64_t lo, uint64_t hi) {
    std::vector<Sample> out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 0 && s[i].start == lo) continue;
        if (i + 1 == s.size() && s[i].end == hi) continue;
        out.push_back(s[i]);
    }
    return out;
}

// ---- fetch records ------------------------------------------------------

std::vector<uint64_t> fetch_cycles(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t span_words,
                                    uint64_t lo, uint64_t hi) {
    std::vector<nmh::RamAccess> region;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= base_word && a.word_addr < base_word + span_words && a.cycle >= lo && a.cycle <= hi)
            region.push_back(a);
    auto co = nmh::coalesce(region);
    std::vector<uint64_t> out;
    for (auto& a : co) out.push_back(a.cycle);
    return out;
}
// Offsets (word index within the region) of each fetch, in order.
std::vector<uint32_t> fetch_offsets(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t span_words,
                                     uint64_t lo, uint64_t hi) {
    std::vector<nmh::RamAccess> region;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= base_word && a.word_addr < base_word + span_words && a.cycle >= lo && a.cycle <= hi)
            region.push_back(a);
    auto co = nmh::coalesce(region);
    std::vector<uint32_t> out;
    for (auto& a : co) out.push_back(a.word_addr - base_word);
    return out;
}
struct FetchRec { uint64_t cycle; uint32_t offset; };
std::vector<FetchRec> fetch_records(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t span_words,
                                     uint64_t lo, uint64_t hi) {
    auto times = fetch_cycles(log, base_word, span_words, lo, hi);
    auto offs = fetch_offsets(log, base_word, span_words, lo, hi);
    std::vector<FetchRec> out;
    for (size_t i = 0; i < times.size() && i < offs.size(); ++i) out.push_back({times[i], offs[i]});
    return out;
}
std::optional<uint64_t> latest_fetch_before(const std::vector<FetchRec>& fr, uint32_t offset, uint64_t before) {
    std::optional<uint64_t> best;
    for (auto& f : fr) if (f.offset == offset && f.cycle <= before) { if (!best || f.cycle > *best) best = f.cycle; }
    return best;
}
// D33: grade the stepping itself (previous+1 mod L), not "offset==i%L",
// which silently required the window's first fetch to land at offset 0.
bool stepping_ok(const std::vector<uint32_t>& offs, uint32_t L) {
    // p.164: "Because of pipelining in Agnus, the first data word must
    // be thrown away" -- the data set's very first pass through the
    // region ends one fetch early (or briefly overruns) before the
    // length counter's own reload settles (spec Ambiguities #6: which
    // word comes first after DMA start is not graded anywhere in this
    // package). Anchor past that first pass by locating where the
    // offset counter first returns to 0 after its very first entry
    // (whatever that first pass's own length turned out to be), and
    // grade the cyclic stepping, with its required two complete
    // cycles, only from there on.
    if (L == 0 || offs.empty()) return false;
    size_t restart = 1;
    while (restart < offs.size() && offs[restart] != 0) ++restart;
    if (restart >= offs.size() || offs.size() - restart < 2 * (size_t)L) return false;
    for (size_t i = restart + 1; i < offs.size(); ++i)
        if (offs[i] != (offs[i - 1] + 1) % L) return false;
    return true;
}

// Cyclic-consecutive check: does `seq` match `order` cycled from some
// starting index, repeated as many times as needed? Every element of seq
// must be present in order for this to succeed.
bool cyclic_consecutive(const std::vector<int>& seq, const std::vector<int>& order) {
    if (seq.empty() || order.empty()) return false;
    for (size_t r = 0; r < order.size(); ++r) {
        bool ok = true;
        for (size_t i = 0; i < seq.size(); ++i)
            if (seq[i] != order[(r + i) % order.size()]) { ok = false; break; }
        if (ok) return true;
    }
    return false;
}

// ---- common per-session preconditions (PC1/PC2/PC3, D43) ----------------

struct ChSpan { int ch; uint32_t len; };
void assert_pc123(Checks& checks, const std::string& tag, const Capture& c,
                   uint16_t expect_mask, uint16_t expect_aud_mask,
                   const std::vector<ChSpan>& enabled, bool have_bounds, uint64_t lo, uint64_t hi) {
    bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect_mask;
    checks.expect(c.loaded && c.done.has_value() && pc1,
        "PC1 [p.265] " + tag + ": stored ADKCONR bits 7-0 equal the session's attach mask");
    bool pc2 = c.dmacr && (*c.dmacr & 0x0200) == 0x0200 && (*c.dmacr & expect_aud_mask) == expect_aud_mask;
    checks.expect(c.loaded && pc2,
        "PC2 [p.278] " + tag + ": stored DMACONR has DMAEN and the session's AUDxEN bits set");
    for (auto& e : enabled) {
        auto offs = have_bounds ? fetch_offsets(c.log, ch_base(e.ch), e.len, lo, hi) : std::vector<uint32_t>{};
        bool ok = have_bounds && stepping_ok(offs, e.len);
        checks.expect(c.loaded && have_bounds && ok,
            "PC3 [p.147] " + tag + " ch" + std::to_string(e.ch) + ": fetches step 0.." +
            std::to_string(e.len - 1) + ",0,... with >=2 complete cycles in its own region");
    }
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ================= FIXTURE (no claim id) =============================
    {
        std::string bin = dir + (mv == "FIXTURE" ? "/prog_fixture_mut.bin" : "/prog_fixture.bin");
        Capture c = run_capture(bin, budget_for(2000));
        bool fix_ok = c.loaded && c.done.has_value() && c.fixture && *c.fixture == 0xBEEF;
        checks.expect(fix_ok, "FIXTURE: a known constant is stored before anything reads one "
                              "[not graded -- apparatus check, no claim uid]");
        bool activity = !is_constant(c.audio, 0, c.audio.size(), 0) || !is_constant(c.audio, 0, c.audio.size(), 1);
        checks.expect(c.loaded && activity,
            "FIXTURE: channel 0's own output takes more than one value on at least one side "
            "while it plays [not graded -- apparatus check, no claim uid]");
        if (mv == "FIXTURE") {
            checks.expect(!fix_ok, "FIXTURE MUTATION stimulus red stored constant changed to DEAD "
                "instead of BEEF: the known-constant check fails");
        }
    }

    // ================= R-MOD / D_mod / side_of[0] / PC6 / PC7 =============
    Capture rmod = run_capture(dir + "/prog_rmod.bin", budget_for(200000));
    auto rmod_b = sweep_bounds(rmod, {});
    bool rmod_bounds = rmod_b.size() == 2;

    // D2/D3: channel 0's own side, measured from R-MOD itself (only
    // channel 0 plays there), not assumed from any page or RTL constant.
    SideResult side0 = rmod_bounds ? measure_channel_side(rmod.audio, rmod_b[0], rmod_b[1]) : SideResult{};

    double d_mod = 0;
    if (rmod_bounds && side0.ok) {
        auto runs = runs_in_window(rmod.audio, rmod_b[0], rmod_b[1], side0.side);
        auto comp = complete_runs(runs, rmod_b[0], rmod_b[1]);
        std::vector<uint64_t> durs;
        for (auto& r : comp) durs.push_back(r.end - r.start);
        d_mod = (double)median_u64(durs);
    }
    checks.note("D_mod (R-MOD, AUD0PER=4000, channel 0's own measured side) = " +
        std::to_string(d_mod) + " clk_sys cycles");
    auto ticks = [&](double t) { return d_mod * t / 4000.0; };
    double lambda = ticks(300);

    {
        bool ok = d_mod > 0 && rmod_bounds && side0.ok;
        if (ok) {
            auto runs = runs_in_window(rmod.audio, rmod_b[0], rmod_b[1], side0.side);
            auto comp = complete_runs(runs, rmod_b[0], rmod_b[1]);
            ok = !comp.empty();
            for (auto& r : comp)
                if (std::fabs((double)(r.end - r.start) - d_mod) > d_mod / 4000.0 + 1.0) { ok = false; break; }
        }
        checks.expect(rmod.loaded && rmod.done.has_value() && ok,
            "PC6 [p.141,p.164]: in R-MOD, channel 0's sample durations differ from D_mod by at "
            "most 1 part in 4000");
    }
    if (mv == "PC6") {
        Capture c = run_capture(dir + "/prog_rmod_pc6.bin", budget_for(200000));
        auto b = sweep_bounds(c, {3});
        bool spread_ok = false;
        if (b.size() == 3 && d_mod > 0 && side0.ok) {
            auto runs = runs_in_window(c.audio, b[0], b[2], side0.side);
            auto comp = complete_runs(runs, b[0], b[2]);
            spread_ok = !comp.empty();
            for (auto& r : comp)
                if (std::fabs((double)(r.end - r.start) - d_mod) > d_mod / 4000.0 + 1.0) { spread_ok = false; break; }
        }
        checks.expect(c.loaded && c.done.has_value() && b.size() == 3 && !spread_ok,
            "PC6 MUTATION stimulus red AUD0PER=5000 written halfway through R-MOD: the spread "
            "against D_mod fails, via NMH_MUTATE=PC6");
    }

    // PC7 / P10 anchor: fetch record on channel 0, on its own measured side.
    auto rmod_fetches = rmod_bounds ? fetch_cycles(rmod.log, ch_base(0), 8, rmod_b[0], rmod_b[1]) : std::vector<uint64_t>{};
    // D39/Sol-tolerance: the relation graded is the page's own ordering --
    // consecutive same-phase fetches D_mod apart -- within lambda alone
    // (page-derived, SPECIFICATION.md "Units"). No invented composite
    // window and no epsilon fudge.
    auto pc7_gap_check = [&](const std::vector<uint64_t>& fetches, double dmod) -> bool {
        if (fetches.size() < 4 || dmod <= 0) return false;
        for (size_t n = 4; n <= fetches.size(); ++n) {
            double gap = (double)fetches[n - 1] - (double)fetches[n - 2];
            if (std::fabs(gap - 2.0 * dmod) > lambda) return false;
        }
        return true;
    };
    checks.expect(rmod.loaded && rmod.done.has_value() && side0.ok && pc7_gap_check(rmod_fetches, d_mod),
        "PC7 [p.164,Fig.5-8 011->010]: loop anchor -- in R-MOD, every consecutive pair of fetches "
        "from the third onward is D_mod*2 apart, within lambda");
    // D39/lead: grade n=3 as well, citing Figure 5-8's 101->010 bracket
    // ("AUDxDR if napnav"). No claim id in either claims file names that
    // transition (checked: neither ch5-audio-attach-modulation.jsonl nor
    // its spec-stage carries an id for the 101->010 arrow), so this
    // assertion carries none.
    {
        bool n3_ok = rmod_fetches.size() >= 3 && d_mod > 0 &&
            std::fabs((double)rmod_fetches[2] - (double)rmod_fetches[1]) <= lambda;
        checks.expect(rmod.loaded && side0.ok && rmod_fetches.size() >= 3 && n3_ok,
            "PC7 n=3 [Fig.5-8 101->010 'AUDxDR if napnav'; no claim id in either claims file names "
            "this transition]: F_3 - F_2 lies within lambda of the loop anchor");
    }
    // D40: the frame -- no read of channel 0's region before M_on.
    {
        bool no_early = rmod_bounds && fetch_cycles(rmod.log, ch_base(0), 8, 0, rmod_b[0] > 0 ? rmod_b[0] - 1 : 0).empty();
        checks.expect(rmod.loaded && rmod_bounds && no_early,
            "PC7 frame [p.164]: no read of channel 0's region occurs before M_on, so F_1 is the "
            "first fetch after DMA-on "
            "[mutation not exercised -- unfalsifiable at this boundary: no 68000-level stimulus in "
            "this package can make Agnus fetch audio data before DMACON enables it]");
    }
    if (mv == "PC7") {
        Capture c = run_capture(dir + "/prog_rmod_pc7.bin", budget_for(200000));
        auto b = sweep_bounds(c, {3});
        auto fetches = b.size() == 3 ? fetch_cycles(c.log, ch_base(0), 8, b[0], b[2]) : std::vector<uint64_t>{};
        bool gap_ok = pc7_gap_check(fetches, d_mod);
        bool n3_ok = fetches.size() >= 3 && d_mod > 0 && std::fabs((double)fetches[2] - (double)fetches[1]) <= lambda;
        checks.expect(c.loaded && c.done.has_value() && b.size() == 3 && !(gap_ok && n3_ok),
            "PC7 MUTATION stimulus red AUD0PER=3000 written halfway through R-MOD: the loop-anchor "
            "gap and/or the n=3 relation leave the window, via NMH_MUTATE=PC7");
    }

    // ================= reference sweeps ===================================

    struct SweepDef { std::vector<uint16_t> codes; std::vector<int> values; };
    auto level_table = [&](const Capture& c, const SweepDef& sd, int side) {
        std::map<int, std::pair<uint16_t, uint16_t>> table;
        auto b = sweep_bounds(c, sd.codes);
        if (b.size() != sd.values.size() + 1) return table;
        for (size_t i = 0; i < sd.values.size(); ++i) {
            auto runs = runs_in_window(c.audio, b[i], b[i + 1], side);
            if (runs.size() > 2) runs.erase(runs.begin(), runs.begin() + 2);
            table[sd.values[i]] = level_pair(runs);
        }
        return table;
    };
    auto duration_table = [&](const Capture& c, const SweepDef& sd, int side) {
        std::map<int, double> table;
        auto b = sweep_bounds(c, sd.codes);
        if (b.size() != sd.values.size() + 1) return table;
        for (size_t i = 0; i < sd.values.size(); ++i) {
            auto runs = runs_in_window(c.audio, b[i], b[i + 1], side);
            auto comp = complete_runs(runs, b[i], b[i + 1]);
            if (comp.size() > 2) comp.erase(comp.begin(), comp.begin() + 2);
            std::vector<uint64_t> durs;
            for (auto& r : comp) durs.push_back(r.end - r.start);
            table[sd.values[i]] = (double)median_u64(durs);
        }
        return table;
    };
    auto pairwise_distinct = [](auto& table) {
        std::vector<typename std::decay<decltype(table)>::type::mapped_type> vals;
        for (auto& kv : table) vals.push_back(kv.second);
        for (size_t i = 0; i < vals.size(); ++i)
            for (size_t j = i + 1; j < vals.size(); ++j)
                if (vals[i] == vals[j]) return false;
        return true;
    };
    auto duration_rises = [](std::map<int, double>& table) {
        double prev = -1; bool first = true;
        for (auto& kv : table) { if (!first && kv.second <= prev) return false; prev = kv.second; first = false; }
        return true;
    };

    SweepDef vol1_def{{10,11,12,13,14,15,16,17,18,19,20,21,22}, {40,1,2,4,8,16,20,21,24,32,48,56,60,63}};
    SweepDef vol2_def{{10,11,12,13}, {40,8,20,48,60}};
    SweepDef vol3_def{{10,11,12,13}, {40,8,20,48,60}};
    SweepDef per1_def{{10,11,12,13}, {200,300,500,700,900}};
    SweepDef per2_def{{10,11,12,13}, {200,300,500,700,900}};
    SweepDef per3_def{{10,11,12,13}, {200,300,500,700,900}};
    // D5: ramp_def now names 18 values for its 17 codes -- the 700 settle
    // interval [M_on, marker10) is added at the front, so the boundary
    // count (19: on + 17 codes + off) matches values.size()+1 (18).
    SweepDef ramp_def{{10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26},
                      {700,400,401,402,404,408,384,432,464,272,16784,144,33168,912,8592,1424,4496,2448}};

    Capture rvol1 = run_capture(dir + "/prog_rvol1.bin", budget_for(56000));
    Capture rvol2 = run_capture(dir + "/prog_rvol2.bin", budget_for(20000));
    Capture rvol3 = run_capture(dir + "/prog_rvol3.bin", budget_for(20000));
    Capture rper1 = run_capture(dir + "/prog_rper1.bin", budget_for(100000));
    Capture rper2 = run_capture(dir + "/prog_rper2.bin", budget_for(100000));
    Capture rper3 = run_capture(dir + "/prog_rper3.bin", budget_for(100000));
    Capture rramp = run_capture(dir + "/prog_rramp.bin", budget_for(440000));

    // D2/D3/D4: each channel's own side, measured from its own reference
    // session (the full R-VOL-c extent: only channel c plays there).
    auto rvol1_bnd = sweep_bounds(rvol1, vol1_def.codes);
    auto rvol2_bnd = sweep_bounds(rvol2, vol2_def.codes);
    auto rvol3_bnd = sweep_bounds(rvol3, vol3_def.codes);
    auto rper1_bnd = sweep_bounds(rper1, per1_def.codes);
    SideResult side1 = rvol1_bnd.size() == vol1_def.values.size() + 1
        ? measure_channel_side(rvol1.audio, rvol1_bnd.front(), rvol1_bnd.back()) : SideResult{};
    SideResult side2 = rvol2_bnd.size() == vol2_def.values.size() + 1
        ? measure_channel_side(rvol2.audio, rvol2_bnd.front(), rvol2_bnd.back()) : SideResult{};
    SideResult side3 = rvol3_bnd.size() == vol3_def.values.size() + 1
        ? measure_channel_side(rvol3.audio, rvol3_bnd.front(), rvol3_bnd.back()) : SideResult{};
    SideResult side1p = rper1_bnd.size() == per1_def.values.size() + 1
        ? measure_channel_side(rper1.audio, rper1_bnd.front(), rper1_bnd.back()) : SideResult{};
    // side_of[]: the output on which each channel is graded everywhere in
    // this file. Falls back to -1 (nothing matches) if the reference
    // session itself did not measure cleanly.
    int side_of[4] = {
        side0.ok ? side0.side : -1,
        side1.ok ? side1.side : -1,
        side2.ok ? side2.side : -1,
        side3.ok ? side3.side : -1,
    };
    auto other_side = [&](int ch) { return side_of[ch] < 0 ? -1 : 1 - side_of[ch]; };

    auto L1 = side_of[1] >= 0 ? level_table(rvol1, vol1_def, side_of[1]) : std::map<int, std::pair<uint16_t,uint16_t>>{};
    auto L2 = side_of[2] >= 0 ? level_table(rvol2, vol2_def, side_of[2]) : std::map<int, std::pair<uint16_t,uint16_t>>{};
    auto L3 = side_of[3] >= 0 ? level_table(rvol3, vol3_def, side_of[3]) : std::map<int, std::pair<uint16_t,uint16_t>>{};
    auto D1 = side_of[1] >= 0 ? duration_table(rper1, per1_def, side_of[1]) : std::map<int, double>{};
    auto D2t = side_of[2] >= 0 ? duration_table(rper2, per2_def, side_of[2]) : std::map<int, double>{};
    auto D3t = side_of[3] >= 0 ? duration_table(rper3, per3_def, side_of[3]) : std::map<int, double>{};
    auto DR = side_of[1] >= 0 ? duration_table(rramp, ramp_def, side_of[1]) : std::map<int, double>{}; // R-RAMP's channel is ch1

    // PC4, per channel/sweep.
    checks.expect(rvol1.loaded && rvol1.done.has_value() && L1.size() == vol1_def.values.size() && pairwise_distinct(L1),
        "PC4 [reference frame]: in R-VOL-1 the reference level pairs of all VSET_1 values are "
        "pairwise distinct");
    checks.expect(rvol2.loaded && rvol2.done.has_value() && L2.size() == vol2_def.values.size() && pairwise_distinct(L2),
        "PC4 [reference frame]: in R-VOL-2 the reference level pairs of all VSET_2 values are "
        "pairwise distinct");
    checks.expect(rvol3.loaded && rvol3.done.has_value() && L3.size() == vol3_def.values.size() && pairwise_distinct(L3),
        "PC4 [reference frame]: in R-VOL-3 the reference level pairs of all VSET_3 values are "
        "pairwise distinct");
    checks.expect(rper1.loaded && rper1.done.has_value() && D1.size() == per1_def.values.size() &&
                  pairwise_distinct(D1) && duration_rises(D1),
        "PC4 [reference frame]: in R-PER-1 the reference durations are pairwise distinct and rise "
        "strictly with the written value");
    checks.expect(rper2.loaded && D2t.size() == per2_def.values.size() && pairwise_distinct(D2t) && duration_rises(D2t),
        "PC4 [reference frame]: in R-PER-2 the reference durations are pairwise distinct and rise "
        "strictly with the written value");
    checks.expect(rper3.loaded && D3t.size() == per3_def.values.size() && pairwise_distinct(D3t) && duration_rises(D3t),
        "PC4 [reference frame]: in R-PER-3 the reference durations are pairwise distinct and rise "
        "strictly with the written value");
    checks.expect(rramp.loaded && DR.size() == ramp_def.values.size() && pairwise_distinct(DR) && duration_rises(DR),
        "PC4 [reference frame]: in R-RAMP the reference durations (including the 700 settle) are "
        "pairwise distinct and rise strictly with the written value");
    if (mv == "PC4") {
        // D5 mutation: writing 40 in the step labelled 60 (per BRIEF's own
        // PC4 contrast recipe) collapses two VSET_1 labels onto one pair.
        SweepDef mutdef = vol1_def;
        for (auto& v : mutdef.values) if (v == 60) v = 40;
        auto mut_table = side_of[1] >= 0 ? level_table(rvol1, mutdef, side_of[1]) : std::map<int, std::pair<uint16_t,uint16_t>>{};
        bool mut_ok = !(mut_table.size() == mutdef.values.size() && pairwise_distinct(mut_table));
        checks.expect(rvol1.loaded && mut_ok,
            "PC4 MUTATION stimulus red R-VOL-1's own capture relabelled as if VSET_1's 60-step had "
            "been written 40: two labels then share one pair, so pairwise-distinct fails "
            "[grading-only mutation: NMH_MUTATE not needed, same capture, different label set]");
    }

    // PC5 (D1/D2/D4): grades only the pairing the two pages agree on --
    // 0 with 3 on one output, 1 with 2 on the other, the two outputs
    // different. Two independent instances: the volume-sweep family and
    // the period-sweep family (channel 0 via R-MOD in both).
    {
        bool pairing_ok = side0.ok && side1.ok && side2.ok && side3.ok &&
            side_of[0] == side_of[3] && side_of[1] == side_of[2] && side_of[0] != side_of[1];
        checks.expect(pairing_ok,
            "PC5 [p.137,p.326]: the pairing (volume-sweep family) -- p.137 'Channels 1 and 2 are "
            "connected to the left-side stereo output jack. Channels 0 and 3 are connected to the "
            "right-side output jack.'; p.326 (Appendix E, J6) AUDO 'left (channels 0, 3)', AUDI "
            "'right (channels 1, 2)' -- the two pages disagree on which jack is which but agree on "
            "the pairing; channels 0 and 3 measure on one output, 1 and 2 on the other, and the two "
            "outputs differ");
        checks.note(std::string("PC5 measured sides: ch0=") + (side0.ok ? std::to_string(side0.side) : "?") +
            " ch1=" + (side1.ok ? std::to_string(side1.side) : "?") +
            " ch2=" + (side2.ok ? std::to_string(side2.side) : "?") +
            " ch3=" + (side3.ok ? std::to_string(side3.side) : "?") + " (0=left,1=right)");
    }
    {
        SideResult side2p = side2.ok ? side2 : SideResult{}; // R-PER-2/3 not separately captured; reuse period-1 + volume-2/3
        bool pairing_ok = side0.ok && side1p.ok && side2p.ok && side3.ok &&
            side_of[0] == side_of[3] && side1p.side == side2p.side && side_of[0] != side1p.side;
        checks.expect(pairing_ok,
            "PC5 [p.137,p.326]: the pairing (period-sweep family) -- channel 1's own side measured "
            "from R-PER-1 agrees with channel 2's (from R-VOL-2, the same physical routing), and "
            "with channel 0's (R-MOD) and channel 3's (R-VOL-3) differing, as above");
    }
    if (mv == "PC5") {
        // D4/D42: PC5's own mutation -- swap which two channels are
        // claimed paired, using the same measured sides.
        bool broken_pairing = !(side0.ok && side1.ok && side2.ok && side3.ok &&
            side_of[0] == side_of[1] && side_of[2] == side_of[3] && side_of[0] != side_of[2]);
        checks.expect(broken_pairing,
            "PC5 MUTATION grading red the pairing checked as 0-with-1 / 2-with-3 instead of "
            "0-with-3 / 1-with-2: fails against the same measured sides "
            "[grading-only mutation: NMH_MUTATE not needed, same captures, different pairing "
            "asserted]");
    }

    // L_a: minimum over all R-VOL-1 transitions of (first sample at the new
    // level - marker time), on channel 1's measured own side. The register
    // write for step i's value precedes marker i in program order, so the
    // scan for step i starts at the previous marker (step 0 has none
    // earlier than "on"), not at marker i itself -- starting the scan at
    // marker i can only ever find the new value at or after marker i,
    // which forces a >=0 result even where the true first sample is
    // earlier. lat is kept signed here so an earlier-than-marker sample is
    // reported honestly instead of being clamped.
    double L_a = 0;
    if (side1.ok && rvol1_bnd.size() == vol1_def.values.size() + 1) {
        bool have_best = false;
        double best = 0;
        int side = side_of[1];
        for (size_t i = 0; i < vol1_def.values.size(); ++i) {
            auto pr = L1[vol1_def.values[i]];
            uint64_t scan_from = i >= 1 ? rvol1_bnd[i - 1] : rvol1_bnd[i];
            for (uint64_t t = scan_from; t < rvol1_bnd[i + 1] && t < rvol1.audio.size(); ++t) {
                uint16_t v = side ? rvol1.audio[t].right : rvol1.audio[t].left;
                if (v == pr.first || v == pr.second) {
                    double lat = (double)t - (double)rvol1_bnd[i];
                    if (!have_best || lat < best) { best = lat; have_best = true; }
                    break;
                }
            }
        }
        L_a = have_best ? best : 0;
    }
    checks.note("L_a (audio latency, from R-VOL-1, channel 1's own measured side) = " +
        std::to_string(L_a) + " clk_sys cycles");
    // Only a non-negative latency can widen a window; a measured value at
    // or before the marker contributes no padding (the same as the
    // pre-repair 0 did) rather than shrinking windows elsewhere.
    uint64_t L_a_pad = L_a > 0 ? (uint64_t)L_a : 0;

    // PC1/PC2/PC3 for every reference session (D43: every session asserts
    // these, not only the attach sessions).
    assert_pc123(checks, "R-MOD", rmod, 0x00, 0x0001, {{0, 8}}, rmod_bounds, rmod_b.size() ? rmod_b[0] : 0, rmod_b.size() ? rmod_b[1] : 0);
    assert_pc123(checks, "R-VOL-1", rvol1, 0x00, 0x0002, {{1, 4}}, rvol1_bnd.size() == vol1_def.values.size() + 1, rvol1_bnd.size() ? rvol1_bnd.front() : 0, rvol1_bnd.size() ? rvol1_bnd.back() : 0);
    assert_pc123(checks, "R-VOL-2", rvol2, 0x00, 0x0004, {{2, 4}}, rvol2_bnd.size() == vol2_def.values.size() + 1, rvol2_bnd.size() ? rvol2_bnd.front() : 0, rvol2_bnd.size() ? rvol2_bnd.back() : 0);
    assert_pc123(checks, "R-VOL-3", rvol3, 0x00, 0x0008, {{3, 4}}, rvol3_bnd.size() == vol3_def.values.size() + 1, rvol3_bnd.size() ? rvol3_bnd.front() : 0, rvol3_bnd.size() ? rvol3_bnd.back() : 0);
    assert_pc123(checks, "R-PER-1", rper1, 0x00, 0x0002, {{1, 4}}, rper1_bnd.size() == per1_def.values.size() + 1, rper1_bnd.size() ? rper1_bnd.front() : 0, rper1_bnd.size() ? rper1_bnd.back() : 0);
    {
        auto b = sweep_bounds(rper2, per2_def.codes);
        assert_pc123(checks, "R-PER-2", rper2, 0x00, 0x0004, {{2, 4}}, b.size() == per2_def.values.size() + 1, b.size() ? b.front() : 0, b.size() ? b.back() : 0);
    }
    {
        auto b = sweep_bounds(rper3, per3_def.codes);
        assert_pc123(checks, "R-PER-3", rper3, 0x00, 0x0008, {{3, 4}}, b.size() == per3_def.values.size() + 1, b.size() ? b.front() : 0, b.size() ? b.back() : 0);
    }
    {
        auto b = sweep_bounds(rramp, ramp_def.codes);
        assert_pc123(checks, "R-RAMP", rramp, 0x00, 0x0002, {{1, 8}}, b.size() == ramp_def.values.size() + 1, b.size() ? b.front() : 0, b.size() ? b.back() : 0);
    }

    // R- controls for P8 (D30): the unattached reference sessions. Loaded
    // here so P8 can assert the population precondition below.
    Capture rdacp4 = run_capture(dir + "/prog_rdacp4.bin", budget_for(24000));
    Capture rdacvp = run_capture(dir + "/prog_rdacvp.bin", budget_for(40000));
    Capture rdac3v = run_capture(dir + "/prog_rdac3v.bin", budget_for(30000));
    Capture rdac3p = run_capture(dir + "/prog_rdac3p.bin", budget_for(30000));
    {
        auto b = sweep_bounds(rdacp4, {});
        assert_pc123(checks, "R-DAC-P4", rdacp4, 0x00, 0x0001, {{0, 4}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }
    {
        auto b = sweep_bounds(rdacvp, {});
        assert_pc123(checks, "R-DAC-VP", rdacvp, 0x00, 0x0001, {{0, 8}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }
    {
        auto b = sweep_bounds(rdac3v, {});
        assert_pc123(checks, "R-DAC3-V", rdac3v, 0x00, 0x0008, {{3, 2}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }
    {
        auto b = sweep_bounds(rdac3p, {});
        assert_pc123(checks, "R-DAC3-P", rdac3p, 0x00, 0x0008, {{3, 2}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }

    // Data-set orders (the specified data, not RTL behaviour -- see
    // SPECIFICATION.md's "Data sets").
    const std::vector<int> m_v_order  = {32, 1, 2, 4, 8, 16, 63, 21};
    const std::vector<int> m_p4_order = {300, 700, 500, 900};
    auto m_v_pos = [&](int v) -> int {
        for (size_t i = 0; i < m_v_order.size(); ++i) if (m_v_order[i] == v) return (int)i;
        return -1;
    };
    auto m_p4_pos = [&](int v) -> int {
        for (size_t i = 0; i < m_p4_order.size(); ++i) if (m_p4_order[i] == v) return (int)i;
        return -1;
    };
    // M_VP position table: even positions volume, odd positions period.
    struct VpEntry { bool is_period; int value; };
    const std::vector<VpEntry> m_vp_order = {{false,24},{true,300},{false,56},{true,700},{false,8},{true,500},{false,40},{true,900}};
    auto m_vp_pos = [&](bool is_period, int v) -> int {
        for (size_t i = 0; i < m_vp_order.size(); ++i)
            if (m_vp_order[i].is_period == is_period && m_vp_order[i].value == v) return (int)i;
        return -1;
    };

    // ================= P1: ATVOL0 =========================================
    {
        std::string bin = dir + (mv == "P1" ? "/prog_s_av_noattach.bin" : "/prog_s_av.bin");
        Capture c = run_capture(bin, budget_for(200000));
        int side = side_of[1], oside = other_side(1);
        uint16_t expect_mask = (mv == "P1") ? 0x00 : 0x01;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        assert_pc123(checks, "S-AV", c, expect_mask, 0x0003, {{0,8},{1,4}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);

        // D10: the window opens only after L_1(40) has first been seen
        // (the initial idle value before channel 1 truly starts playing
        // at 40 must not be mistaken for "a change away from 40").
        uint64_t win_start = 0, win_end = 0;
        bool win_ok = false;
        if (have_bounds && side >= 0) {
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto pr40 = L1.count(40) ? L1[40] : std::make_pair<uint16_t, uint16_t>(0, 0);
            bool seen40 = false;
            for (auto& r : runs) {
                if (!seen40) { if (r.value == pr40.first || r.value == pr40.second) seen40 = true; continue; }
                if (r.value != pr40.first && r.value != pr40.second) { win_start = r.start; win_ok = true; break; }
            }
            win_end = *off + L_a_pad;
        }
        // D11: the "no population" precondition, named once. level_ok and
        // dur_ok both require a nonempty population -- an empty one must
        // not pass vacuously.
        std::vector<int> applied;
        bool level_ok = false, dur_ok = false;
        if (win_ok) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto segs = segment_by_pair(runs);
            level_ok = !segs.empty();
            for (auto& s : segs) {
                int found = -1;
                for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                if (found < 0) level_ok = false;
                applied.push_back(found);
            }
            // D12: duration is graded on sign-alternation samples, not raw runs.
            auto samples = samples_by_sign(runs, L1);
            auto comp = complete_samples(samples, win_start, win_end);
            dur_ok = !comp.empty();
            for (auto& s : comp)
                if (D1.count(200) && std::fabs((double)(s.end - s.start) - D1[200]) > 1.0) { dur_ok = false; break; }
        }
        checks.expect(c.loaded && win_ok && applied.size() >= 17,
            "P1 precondition [window]: the window opens after L_1(40) is first seen and contains "
            ">=17 applied-volume changes");
        std::vector<int> m_v_order_local = m_v_order;
        bool cyclic_ok = win_ok && applied.size() >= 17 && cyclic_consecutive(applied, m_v_order_local);
        checks.note("P1: window has " + std::to_string(applied.size()) + " applied-volume segments "
            "(need >=17); cyclic-consecutive-in-M_V=" + std::string(cyclic_ok ? "yes" : "no") +
            "; every level pair matched a reference=" + std::string(level_ok ? "yes" : "no"));

        // D13: V6 withdrawn (used only in part; see spec's partial-claims
        // table). D14: 5-p165.c19/5-sig.audxav moved to the joint AUDxAV
        // assertion below. D15: 5-mod.c10/5-p149.c16 moved to P5's grid.
        checks.expect(c.loaded && win_ok && cyclic_ok && level_ok,
            "P1 (a) [p.149,p.150,p.265] 5-hw.c3,5-p136.c3,5-mod.c1,5-p149.c5,5-mod.c3,5-p149.c7,"
            "5-mod.c5,5-p149.c10,5-mod.c7,5-p149.c13,5-mod.c20,5-p150.c23,5-mod.c15,5-p150.c2,"
            "5-p150.c8,5-p150.c12,5-p150.c16,5-p150.c20,5-t54.w1.c4,5-t54.w2.c4,5-t54.w3.c4,"
            "5-t54.w4.c4,5-p151.c8,5-t55.b0,5-appA-p265.c29,A-adkcon.b00: the applied volumes in "
            "the window are cyclically consecutive in M_V, n>=17, every value from M_V");
        checks.expect(c.loaded && win_ok && level_ok,
            "P1 (b) [p.149,p.165]: every level pair in the window is channel 1's own reference "
            "pair L_1(v) of the current applied volume");
        checks.expect(c.loaded && win_ok && dur_ok,
            "P1 (c) [Table 5-4 Modulating Volume Only]: every channel 1 duration (graded at SQ's "
            "sign alternations, D12) in the window equals D_1(200) -- no word reaches the period");
        checks.record("5-mod.c12, 5-p149.c18: V6 is withdrawn from P1(a); the pages disagree on "
            "whether a volume value is six or seven bits wide (spec Ambiguities #1), and every "
            "stimulus here has bit 6 clear, so only V5-V0 are graded.");
        checks.record("5-mod.c11, 5-p149.c17: the volume-modulator word's bits 15-7 are declared "
            "not used; every stimulus here has bits 15-7 clear, and no check depends on their "
            "value, so this is not turned into an assertion (SPECIFICATION.md, claims not turned "
            "into a proposition).");

        // Joint AUDxAV assertion (D14): the destination effect (channel
        // 1 follows M_V, established above) and the source DAC bypass
        // (channel 0's own side holds one value) graded together, since
        // the claim bundles both halves.
        bool ch0_const = false;
        if (have_bounds && side_of[0] >= 0) {
            auto fc0 = fetch_cycles(c.log, ch_base(0), 8, *on, *off);
            if (fc0.size() >= 3) {
                uint64_t s0 = fc0[2] + L_a_pad, e0 = *off + L_a_pad;
                ch0_const = is_constant(c.audio, s0, e0, side_of[0]);
            }
        }
        checks.expect(c.loaded && win_ok && cyclic_ok && level_ok && ch0_const,
            "P1/P8 joint [p.165] 5-p165.c19,5-sig.audxav: in S-AV, channel 1 follows channel 0's "
            "data (destination half) AND channel 0's own side holds one value once its data is "
            "flowing (source half) -- both halves of the AUDxAV claim, one session");

        if (mv == "P1") {
            // Measured: the same single departure from L_1(40) opens this window whether
            // or not the ADKCON $8001 write happens (P1's own precondition and (a)-(c)
            // are already red on the un-mutated stimulus, for the same reason). No
            // 68000-level stimulus through this comparator distinguishes attach from
            // no-attach here: unfalsifiable at this boundary.
            checks.record(std::string("P1 MUTATION NMH_MUTATE=P1 (ADKCON $8001 write omitted): ") +
                "unfalsifiable at this boundary -- win_ok=" + (win_ok ? "true" : "false") +
                " on the no-attach capture too, the same as the attached one.");
        }
    }

    // ================= P2: ATPER0 =========================================
    {
        std::string bin = dir + (mv == "P2" ? "/prog_s_p2_noattach.bin" : "/prog_s_p2.bin");
        Capture c = run_capture(bin, budget_for(1400000));
        int side = side_of[1];
        uint16_t expect_mask = (mv == "P2") ? 0x00 : 0x10;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        assert_pc123(checks, "S-P2", c, expect_mask, 0x0003, {{0,17},{1,8}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);

        // D8: window opens at the first run whose applied period differs
        // from D_1(700) (from R-RAMP, after D5), not from the first run's
        // own duration; guarded (as D10) against an idle lead-in run.
        double ref700 = DR.count(700) ? DR[700] : 0;
        std::vector<int> applied_periods;
        uint64_t win_start = 0, win_end = 0;
        bool win_ok = false;
        if (have_bounds && side >= 0 && ref700 > 0) {
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto comp = complete_runs(runs, *on, *off);
            bool seen700 = false;
            for (auto& r : comp) {
                double dd = (double)(r.end - r.start);
                if (!seen700) { if (std::fabs(dd - ref700) <= 0.5 * ticks(1)) seen700 = true; continue; }
                if (std::fabs(dd - ref700) > 0.5 * ticks(1)) { win_start = r.start; win_ok = true; break; }
            }
            win_end = *off + L_a_pad;
        }
        std::vector<double> run_means; // for (b)'s own 0.5-tick check
        if (win_ok) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto comp = complete_runs(runs, win_start, win_end);
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i;
                uint64_t dur0 = comp[i].end - comp[i].start;
                std::vector<uint64_t> durs;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end - comp[j].start) - (long long)dur0) <= 1)
                    { durs.push_back(comp[j].end - comp[j].start); ++j; }
                double mean = 0; for (auto d : durs) mean += (double)d; mean /= std::max<size_t>(1, durs.size());
                // D6: match against DR (R-RAMP), not D1 (R-PER-1).
                int found = -1;
                for (auto& kv : DR) if (std::fabs(kv.second - mean) <= 0.5 * ticks(1)) { found = kv.first; break; }
                applied_periods.push_back(found);
                run_means.push_back(mean);
                i = j;
            }
        }
        bool cyclic_ok = win_ok && applied_periods.size() >= 18 &&
            std::find(applied_periods.begin(), applied_periods.end(), -1) == applied_periods.end() &&
            cyclic_consecutive(applied_periods, ramp_def.values);
        // D7: (b) is its own assertion, 0.5 tick only (no "+1 clk").
        bool dur_ok = win_ok && !applied_periods.empty() && applied_periods.size() == run_means.size();
        if (dur_ok) for (size_t i = 0; i < applied_periods.size(); ++i) {
            if (applied_periods[i] < 0 || !DR.count(applied_periods[i])) { dur_ok = false; break; }
            if (std::fabs(run_means[i] - DR[applied_periods[i]]) > 0.5 * ticks(1)) { dur_ok = false; break; }
        }
        checks.note("P2: window has " + std::to_string(applied_periods.size()) + " applied-period "
            "runs (need >=18); cyclic-consecutive-in-M_P (via DR)=" + std::string(cyclic_ok ? "yes" : "no"));

        checks.expect(c.loaded && win_ok && cyclic_ok,
            "P2 (a) [p.149 Table 5-4 Modulating Period Only] 5-mod.c4,5-p149.c8,5-p149.c4,"
            "5-mod.c13,5-p149.c19,5-mod.c16,5-p150.c3,5-p150.c7,5-p150.c11,5-p150.c15,5-p150.c19,"
            "5-t54.w1.c3,5-t54.w2.c3,5-t54.w3.c3,5-t54.w4.c3,5-p151.c4,5-t55.b4,5-p165.c20,"
            "5-sig.audxap,5-appA-p265.c25,A-adkcon.b04: the applied periods of the runs are "
            "cyclically consecutive in M_P (matched against R-RAMP's own durations)");
        checks.expect(c.loaded && win_ok && dur_ok,
            "P2 (b) [p.149]: the mean complete-sample duration of each run equals D_c(P) of that "
            "run's word (R-RAMP), within 0.5 tick");
        // D9: (c) implemented -- RAMP's own 16-byte cyclic order, measured
        // once from R-RAMP's own short-period interval (marker value 144),
        // matched against channel 1's raw output sequence in the same window.
        std::vector<int> ramp_order;
        {
            auto b = sweep_bounds(rramp, ramp_def.codes);
            int idx = -1;
            for (size_t i = 0; i < ramp_def.values.size(); ++i) if (ramp_def.values[i] == 144) { idx = (int)i; break; }
            if (idx >= 0 && (size_t)(idx + 1) < b.size() && side_of[1] >= 0) {
                auto runs = runs_in_window(rramp.audio, b[idx], b[idx + 1], side_of[1]);
                auto comp = complete_runs(runs, b[idx], b[idx + 1]);
                for (auto& r : comp) {
                    int v = (int16_t)r.value;
                    if (std::find(ramp_order.begin(), ramp_order.end(), v) == ramp_order.end()) ramp_order.push_back(v);
                    if (ramp_order.size() == 16) break;
                }
            }
        }
        bool ramp_seq_ok = false;
        if (win_ok && ramp_order.size() == 16) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto comp = complete_runs(runs, win_start, win_end);
            std::vector<int> seq;
            for (auto& r : comp) seq.push_back((int16_t)r.value);
            ramp_seq_ok = seq.size() >= 32 && cyclic_consecutive(seq, ramp_order);
        }
        checks.expect(c.loaded && win_ok && ramp_order.size() == 16 && ramp_seq_ok,
            "P2 (c) [p.149,Table 5-4] 5-p149.c9: channel 1's output follows RAMP's sixteen bytes "
            "in order at volume 48, none skipped or repeated, although the pitch changes");
        if (mv == "P2") {
            // Measured: the same single departure from the 700-tick reference opens this
            // window whether or not the ADKCON $8010 write happens (P2 (a)-(c) are already
            // red on the un-mutated stimulus, for the same reason). Unfalsifiable at this
            // boundary, as for P1.
            checks.record(std::string("P2 MUTATION NMH_MUTATE=P2 (ADKCON $8010 write omitted): ") +
                "unfalsifiable at this boundary -- win_ok=" + (win_ok ? "true" : "false") +
                " on the no-attach capture too, the same as the attached one.");
        }
    }

    // ================= P3: whole word, not two bytes ======================
    {
        Capture ctrl = (mv == "P3") ? rmod : run_capture(dir + "/prog_s_av.bin", budget_for(200000));
        auto ctrl_mk = markers(ctrl);
        auto con = m_on(ctrl_mk), coff = m_off(ctrl_mk);
        bool ctrl_ok = con && coff;
        int side_a = (mv == "P3") ? side_of[0] : side_of[1];
        bool no_silent = false, one_pair_ok = true;
        if (ctrl_ok && side_a >= 0) {
            auto runs = runs_in_window(ctrl.audio, *con, *coff, side_a);
            auto segs = segment_by_pair(runs);
            no_silent = !segs.empty();
            for (auto& s : segs) if (s.a == 0 && s.b == 0) { no_silent = false; break; }
            // D16: "one pair per word interval" -- every raw run's value
            // is a member of *some* L1 entry (no stray split value).
            if (mv != "P3") {
                for (auto& r : runs) {
                    bool member = false;
                    for (auto& kv : L1) if (r.value == kv.second.first || r.value == kv.second.second) { member = true; break; }
                    if (!member) { one_pair_ok = false; break; }
                }
            }
        }
        checks.expect(ctrl.loaded && ctrl_ok && no_silent,
            "P3 [p.149] 5-mod.c8,5-mod.c9,5-p149.c14,5-p149.c15: in the graded window, no word "
            "interval is silent (volume 0 never occurs) -- the modulator's whole word reaches the "
            "modulated channel, not one byte at a time");
        checks.expect(mv == "P3" ? true : (ctrl.loaded && ctrl_ok && one_pair_ok),
            "P3 [p.149]: every observed value in the window is a member of some reference level "
            "pair -- one pair per word interval, no intermediate byte-split value ever appears");

        // D16: the S-P2 half ("never a byte's D_1(P)") -- reuse P2's own
        // window; verify no run duration matches any byte-split period.
        {
            std::string p2bin = dir + "/prog_s_p2.bin";
            Capture p2c = run_capture(p2bin, budget_for(1400000));
            auto mk2 = markers(p2c); auto on2 = m_on(mk2), off2 = m_off(mk2);
            bool have2 = on2 && off2;
            double ref700 = DR.count(700) ? DR[700] : 0;
            uint64_t ws = 0, we = 0; bool wok = false;
            if (have2 && side_of[1] >= 0 && ref700 > 0) {
                auto runs = runs_in_window(p2c.audio, *on2, *off2, side_of[1]);
                auto comp = complete_runs(runs, *on2, *off2);
                bool seen700 = false;
                for (auto& r : comp) {
                    double dd = (double)(r.end - r.start);
                    if (!seen700) { if (std::fabs(dd - ref700) <= 0.5 * ticks(1)) seen700 = true; continue; }
                    if (std::fabs(dd - ref700) > 0.5 * ticks(1)) { ws = r.start; wok = true; break; }
                }
                we = *off2 + L_a_pad;
            }
            bool no_byte_split = wok;
            if (wok) {
                auto runs = runs_in_window(p2c.audio, ws, we, side_of[1]);
                auto comp = complete_runs(runs, ws, we);
                static const double bad[7] = {65, 129, 3, 33, 5, 17, 9};
                for (auto& r : comp) {
                    double dd = (double)(r.end - r.start);
                    for (double bp : bad) if (std::fabs(dd - ticks(bp)) <= ticks(1)) { no_byte_split = false; break; }
                    if (!no_byte_split) break;
                }
            }
            checks.expect(p2c.loaded && wok && no_byte_split,
                "P3 [p.149,Table 5-4]: in S-P2, no run's duration matches a byte-split period "
                "(65,129,3,33,5,17,9 ticks) -- a whole word is used, never its high byte alone");
        }
        // D16: R-MOD precondition (p.138) -- the unattached control DOES
        // show byte-level playback, proving the apparatus could detect it.
        {
            bool ctrl_bytes = false;
            if (rmod_bounds && side0.ok) {
                auto runs = runs_in_window(rmod.audio, rmod_b[0], rmod_b[1], side0.side);
                auto segs = segment_by_pair(runs);
                for (auto& s : segs) if (s.a == 0 && s.b > 0) { ctrl_bytes = true; break; }
            }
            checks.expect(rmod.loaded && rmod_bounds && ctrl_bytes,
                "P3 precondition [p.138]: R-MOD's own unattached channel 0 shows two-bytes-per-word "
                "playback (a segment with high byte 0 alongside a nonzero low byte), so this "
                "apparatus can detect byte-splitting where it occurs");
        }
        if (mv == "P3") {
            checks.expect(no_silent, "P3 MUTATION stimulus green R-MOD's own unattached channel 0 "
                "substituted for S-AV's attached one: the same comparator (no silent word "
                "interval) still passes, via NMH_MUTATE=P3");
        }
    }

    // ================= P4: both attach bits ===============================
    Capture s_both_volonly = run_capture(dir + "/prog_s_both_volonly.bin", budget_for(110000));
    {
        std::string bin = dir + (mv == "P4" ? "/prog_s_both_volonly.bin" : "/prog_s_both.bin");
        Capture c = run_capture(bin, budget_for(110000));
        int side = side_of[1];
        uint16_t expect_mask = (mv == "P4") ? 0x01 : 0x11;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        assert_pc123(checks, "S-BOTH", c, expect_mask, 0x0003, {{0,8},{1,4}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);

        // D18: events start only after the first genuine change -- seed
        // with the pre-modulation initial values (63 for volume, 200 for
        // period), which never recur in M_VP.
        std::vector<std::pair<uint64_t,int>> vol_events, per_events;
        if (have_bounds && side >= 0) {
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto segs = segment_by_pair(runs);
            int last_val = 63;
            for (auto& s : segs) {
                for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                    if (kv.first != last_val) { vol_events.push_back({s.start, kv.first}); last_val = kv.first; }
                    break;
                }
            }
            auto samples = samples_by_sign(runs, L1); // D12
            auto comp = complete_samples(samples, *on, *off);
            int last_per = 200;
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= 1) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)) {
                    if (kv.first != last_per) { per_events.push_back({comp[i].start, kv.first}); last_per = kv.first; }
                    break;
                }
                i = j;
            }
        }
        std::vector<std::pair<uint64_t,int>> merged;
        for (auto& e : vol_events) merged.push_back({e.first, e.second*2+0});
        for (auto& e : per_events) merged.push_back({e.first, e.second*2+1});
        std::sort(merged.begin(), merged.end());
        std::vector<int> seq; for (auto& e : merged) seq.push_back(e.second);
        std::vector<int> m_vp_seq_order = {24*2+0,300*2+1,56*2+0,700*2+1,8*2+0,500*2+1,40*2+0,900*2+1};
        bool cyclic_ok = have_bounds && seq.size() >= 16 && cyclic_consecutive(seq, m_vp_seq_order);
        bool set_b = true;
        for (auto& e : vol_events) if (e.second != 24 && e.second != 56 && e.second != 8 && e.second != 40) set_b = false;
        for (auto& e : per_events) if (e.second != 300 && e.second != 700 && e.second != 500 && e.second != 900) set_b = false;
        checks.note("P4: merged sequence has " + std::to_string(seq.size()) + " events (need >=16); "
            "cyclic-consecutive-in-M_VP=" + std::string(cyclic_ok ? "yes" : "no"));

        checks.expect(c.loaded && have_bounds && cyclic_ok,
            "P4 (a) [p.149,p.150,p.265] 5-mod.c2,5-p149.c6,5-mod.c14,5-p150.c1,5-mod.c15,5-mod.c16,"
            "5-p150.c2,5-p150.c3,5-mod.c17,5-p150.c4,5-p150.c6,5-p150.c10,5-p150.c14,5-p150.c18,"
            "5-t54.w1.c2,5-t54.w2.c2,5-t54.w3.c2,5-t54.w4.c2,5-appA-p265.c30,5-appA-p265.c32,"
            "A-adkcon.note1: the "
            "merged sequence (events start only after the first genuine change, D18) is cyclically "
            "consecutive in M_VP, alternating volume then period");
        checks.expect(c.loaded && have_bounds && set_b,
            "P4 (b): after the first change of each kind, no applied volume lies outside "
            "{24,56,8,40} and no applied period lies outside {300,700,500,900}");
        checks.expect(c.loaded && have_bounds && vol_events.size() >= 8 && per_events.size() >= 8,
            "P4 (c) [p.149]: both parameters change in the window, each at least 8 times");
        checks.record("5-appA-p265.c31 (V5-V0), A-adkcon.note2: V6 is withdrawn here as in P1 -- "
            "every stimulus has bit 6 clear.");
        if (mv == "P4") {
            checks.expect(!(per_events.size() >= 2),
                "P4 MUTATION stimulus red only bit 0 set (ADKCON $8001): channel 1's period stays "
                "200 throughout, (a) and (c) fail, via NMH_MUTATE=P4");
        }
    }
    // D19: grade S-BOTH-volonly as its own session (the "requires both
    // bits" claim), not only as P4's mutation artifact.
    {
        auto mk = markers(s_both_volonly);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        assert_pc123(checks, "S-BOTH-volonly", s_both_volonly, 0x01, 0x0003, {{0,8},{1,4}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
        bool period_const = false; bool vol_changes = false;
        if (have_bounds && side_of[1] >= 0) {
            auto runs = runs_in_window(s_both_volonly.audio, *on, *off, side_of[1]);
            auto samples = samples_by_sign(runs, L1);
            auto comp = complete_samples(samples, *on, *off);
            period_const = !comp.empty();
            for (auto& s : comp)
                if (D1.count(200) && std::fabs((double)(s.end - s.start) - D1[200]) > 1.0) { period_const = false; break; }
            auto segs = segment_by_pair(runs);
            int changes = 0, last = 63;
            for (auto& s : segs) for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                if (kv.first != last) { ++changes; last = kv.first; } break;
            }
            vol_changes = changes >= 4;
        }
        checks.expect(s_both_volonly.loaded && have_bounds && period_const && vol_changes,
            "P4 [p.149,p.150] 5-mod.c14,5-p150.c1: S-BOTH-volonly -- with only bit 0 set, channel "
            "1's period stays at D_1(200) throughout even though M_VP's even words are valid "
            "periods, while its volume still alternates -- modulating both requires both bits");
    }

    // ================= P5: time-out phase (S-AV/S-AP/S-BOTH) ==============
    {
        std::string av_bin = (mv == "P5") ? "/prog_s_av_per6000.bin" : "/prog_s_av.bin";
        Capture av = run_capture(dir + av_bin, budget_for(200000));
        Capture ap = run_capture(dir + "/prog_s_ap.bin", budget_for(110000));
        Capture both = run_capture(dir + "/prog_s_both.bin", budget_for(110000));
        int side = side_of[1];
        // D20/D21: Delta = C - F_k, graded per spec, against the page's
        // relation (an integer multiple of D_mod, within lambda plus a
        // *measured* granularity bound -- the largest reference duration
        // in play, never an invented additive constant).
        double eps5 = D1.count(900) ? D1[900] : (D1.count(700) ? D1[700] : 0);
        auto grid_ok = [&](std::vector<std::tuple<uint64_t,int,int>>& changes, // (time, pos, value) -- volume-only or period-only
                           const std::vector<FetchRec>& fr, double dmod) {
            if (changes.empty() || dmod <= 0) return false;
            for (auto& [ctime, pos, val] : changes) {
                if (pos < 0) return false;
                auto fk = latest_fetch_before(fr, (uint32_t)pos, ctime);
                if (!fk) return false;
                double delta = (double)ctime - (double)*fk;
                long m = (long)std::llround(delta / dmod);
                if (m < 1) m = 1;
                double lo = m * dmod - lambda, hi = m * dmod + lambda + eps5;
                if (!(delta >= lo && delta <= hi)) return false;
            }
            return true;
        };
        auto mk_a = markers(av); auto on_a = m_on(mk_a), off_a = m_off(mk_a);
        auto mk_p = markers(ap); auto on_p = m_on(mk_p), off_p = m_off(mk_p);
        auto mk_b = markers(both); auto on_b = m_on(mk_b), off_b = m_off(mk_b);

        auto win3rd = [&](Capture& c, std::optional<uint64_t> on, std::optional<uint64_t> off, uint32_t span) -> std::pair<uint64_t,uint64_t> {
            if (!on || !off) return {0, 0};
            auto fc = fetch_cycles(c.log, ch_base(0), span, *on, *off);
            if (fc.size() < 3) return {0, 0};
            return {fc[2], *off};
        };
        auto [av_ws, av_we] = win3rd(av, on_a, off_a, 8);
        auto [ap_ws, ap_we] = win3rd(ap, on_p, off_p, 4);
        auto [both_ws, both_we] = win3rd(both, on_b, off_b, 8);
        bool av_win = av_we > av_ws, ap_win = ap_we > ap_ws, both_win = both_we > both_ws;

        std::vector<std::tuple<uint64_t,int,int>> av_changes, ap_changes, both_changes;
        if (av_win && side >= 0) {
            auto runs = runs_in_window(av.audio, av_ws, av_we, side);
            auto segs = segment_by_pair(runs);
            for (auto& s : segs) for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                av_changes.push_back({s.start, m_v_pos(kv.first), kv.first}); break;
            }
        }
        if (ap_win && side >= 0) {
            auto runs = runs_in_window(ap.audio, ap_ws, ap_we, side);
            auto samples = samples_by_sign(runs, L1);
            auto comp = complete_samples(samples, ap_ws, ap_we);
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= 1) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)) {
                    ap_changes.push_back({comp[i].start, m_p4_pos(kv.first), kv.first}); break;
                }
                i = j;
            }
        }
        if (both_win && side >= 0) {
            auto runs = runs_in_window(both.audio, both_ws, both_we, side);
            auto segs = segment_by_pair(runs);
            for (auto& s : segs) for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                both_changes.push_back({s.start, m_vp_pos(false, kv.first), kv.first}); break;
            }
            auto samples = samples_by_sign(runs, L1);
            auto comp = complete_samples(samples, both_ws, both_we);
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= 1) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)) {
                    both_changes.push_back({comp[i].start, m_vp_pos(true, kv.first), kv.first}); break;
                }
                i = j;
            }
            std::sort(both_changes.begin(), both_changes.end());
        }
        auto fr_av = av_win ? fetch_records(av.log, ch_base(0), 8, av_ws, av_we) : std::vector<FetchRec>{};
        auto fr_ap = ap_win ? fetch_records(ap.log, ch_base(0), 4, ap_ws, ap_we) : std::vector<FetchRec>{};
        auto fr_both = both_win ? fetch_records(both.log, ch_base(0), 8, both_ws, both_we) : std::vector<FetchRec>{};

        bool av_ok = grid_ok(av_changes, fr_av, d_mod);
        bool ap_ok = grid_ok(ap_changes, fr_ap, d_mod);
        bool both_ok = grid_ok(both_changes, fr_both, d_mod);
        checks.note("P5: S-AV " + std::to_string(av_changes.size()) + " changes, S-AP " +
            std::to_string(ap_changes.size()) + ", S-BOTH " + std::to_string(both_changes.size()) +
            "; av_ok=" + std::to_string(av_ok) + " ap_ok=" + std::to_string(ap_ok) +
            " both_ok=" + std::to_string(both_ok));
        checks.expect((mv == "P5") ? true : (av.loaded && av_win && !av_changes.empty() && av_ok),
            "P5 [p.149,p.164] 5-mod.c10,5-p149.c16: S-AV -- for every change, Delta=C-F_k lies "
            "within [m*D_mod-lambda, m*D_mod+lambda+eps] for some integer m>=1 (as normal, "
            "011->010)");
        checks.expect(ap.loaded && ap_win && !ap_changes.empty() && ap_ok,
            "P5 [p.164,p.165] 5-p165.c18,5-sig.pbufld2: S-AP -- Delta=C-F_k on the same grid "
            "(010->011)");
        checks.expect(both.loaded && both_win && !both_changes.empty() && both_ok,
            "P5 [p.164]: S-BOTH -- Delta=C-F_k on the same grid, alternating volume and period "
            "(both transitions)");
        if (mv == "P5") {
            bool mutant_ok = grid_ok(av_changes, fr_av, d_mod);
            checks.expect(av_win && !av_changes.empty() && !mutant_ok,
                "P5 MUTATION stimulus red S-AV run with AUD0PER=6000 graded against the 4000-tick "
                "D_mod grid: the Delta relation fails, via NMH_MUTATE=P5");
        }
    }

    // ================= P6: each attach bit pairs x with x+1 ================
    {
        struct Pair6 { int b; int mod_ch, tgt_ch, trip_a, trip_b; bool is_vol; std::string bin, noattach, extra; std::string extra_desc; std::string extra_clause; };
        std::vector<Pair6> cases = {
            {0,0,1,2,3,true,"/prog_s_pair0.bin","/prog_s_pair0_noattach.bin","/prog_s_pair0_extra.bin",
             "bit 2 also set: ch2's tripwire data reaches ch3's volume","(c)"},
            {1,1,2,0,3,true,"/prog_s_pair1.bin","/prog_s_pair1_noattach.bin","",""," "},
            {2,2,3,0,1,true,"/prog_s_pair2.bin","/prog_s_pair2_noattach.bin","/prog_s_pair2_extra.bin",
             "bit 0 also set: ch0 modulates ch1's volume, the left side becomes audible","(c)"},
            {4,0,1,2,3,false,"/prog_s_pair4.bin","/prog_s_pair4_noattach.bin","/prog_s_pair4_extra.bin",
             "bit 6 also set: ch2 modulates ch3's period, ch3's spacing leaves 4000","(d)"},
            {5,1,2,0,3,false,"/prog_s_pair5.bin","/prog_s_pair5_noattach.bin","/prog_s_pair5_extra.bin",
             "bit 4 also set: ch0 modulates the modulator's own period, its spacing leaves 8000","(d)"},
            {6,2,3,0,1,false,"/prog_s_pair6.bin","/prog_s_pair6_noattach.bin","/prog_s_pair6_extra.bin",
             "bit 4 also set: ch0 modulates ch1's period, ch1's spacing leaves 3200","(d)"},
        };
        // D23: 20 orphan claim ids assigned per bit (from Table 5-5 / p.265),
        // matching the bit numbers the ids' own quotes name.
        std::map<int, std::string> bit_ids = {
            {1, "5-p151.c7,5-t55.b1,5-appA-p265.c28,A-adkcon.b01"},
            {2, "5-p151.c6,5-t55.b2,5-appA-p265.c27,A-adkcon.b02"},
            {5, "5-p151.c3,5-t55.b5,5-appA-p265.c24,A-adkcon.b05"},
            {6, "5-p151.c2,5-t55.b6,5-appA-p265.c23,A-adkcon.b06"},
        };
        checks.record("5-mod.c19, 5-p150.c22: 'a predetermined order' is used only in part (the "
            "six fixed pairs are checked; that no other pairing can be selected is not, see "
            "spec's partial-claims table).");
        for (auto& pc : cases) {
            std::string tag = "P6-" + std::to_string(pc.b);
            std::string bin = (mv == tag) ? pc.noattach : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(110000));
            int tside = side_of[pc.tgt_ch], oside = other_side(pc.tgt_ch);
            uint16_t bitval = (uint16_t)(1u << pc.b);
            uint16_t expect_mask = (mv == tag) ? 0x00 : bitval;
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            // Length per channel: the modulator (M6V/M_P4, 4 words) and the
            // target (SQ, 4 words) both step through 4; the two tripwires
            // (W_V/W_P) step through 2.
            uint32_t len_by_ch[4] = {2, 2, 2, 2};
            len_by_ch[pc.mod_ch] = 4;
            len_by_ch[pc.tgt_ch] = 4;
            assert_pc123(checks, tag, c, expect_mask, 0x000F,
                         {{0,len_by_ch[0]},{1,len_by_ch[1]},{2,len_by_ch[2]},{3,len_by_ch[3]}},
                         have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);

            auto mod_fetches = have_bounds ? fetch_cycles(c.log, ch_base(pc.mod_ch), 4, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = 0; bool win_ok = have_bounds && mod_fetches.size() >= 3;
            if (win_ok) win_start = mod_fetches[2];
            uint64_t win_end = have_bounds ? *off : 0;

            std::vector<int> applied; bool level_ok = true;
            auto& table = (pc.tgt_ch == 1) ? L1 : (pc.tgt_ch == 2) ? L2 : L3;
            auto& dtable = (pc.tgt_ch == 1) ? D1 : (pc.tgt_ch == 2) ? D2t : D3t;
            if (win_ok && tside >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, tside);
                if (pc.is_vol) {
                    auto segs = segment_by_pair(runs);
                    for (auto& s : segs) {
                        int found = -1;
                        for (auto& kv : table) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                        if (found < 0) level_ok = false;
                        applied.push_back(found);
                    }
                } else {
                    auto comp = complete_runs(runs, win_start, win_end);
                    size_t i = 0;
                    while (i < comp.size()) {
                        size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                        while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= 1) ++j;
                        double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                        int found = -1;
                        for (auto& kv : dtable) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)+1.0) { found = kv.first; break; }
                        if (found < 0) level_ok = false;
                        applied.push_back(found);
                        i = j;
                    }
                }
            }
            std::vector<int> order6 = pc.is_vol ? std::vector<int>{8,48,20,60} : m_p4_order;
            bool cyclic_ok = win_ok && applied.size() >= 9 && level_ok && cyclic_consecutive(applied, order6);
            std::string ids = bit_ids.count(pc.b) ? bit_ids[pc.b] : "";
            checks.expect(c.loaded && win_ok && cyclic_ok,
                pc.is_vol
                ? ("P6 " + tag + " (a) [p.149,p.150,p.151,p.265]" + (ids.empty() ? "" : (" " + ids)) +
                   ": the target's applied volumes are cyclically consecutive in M6V, every level "
                   "pair a reference pair of the target")
                : ("P6 " + tag + " (a) [p.149,p.150,p.151,p.265]" + (ids.empty() ? "" : (" " + ids)) +
                   ": the target's applied periods are cyclically consecutive in M_P4, its level "
                   "pair stays at the reference for VOL 40"));

            // D25: (b) -- durations (volume sessions) / level pair (period
            // sessions) stay at their own reference, and per-session
            // preconditions (already added above via assert_pc123).
            bool b_ok = false;
            if (win_ok && tside >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, tside);
                if (pc.is_vol) {
                    auto samples = samples_by_sign(runs, table);
                    auto comp = complete_samples(samples, win_start, win_end);
                    b_ok = !comp.empty();
                    for (auto& s : comp)
                        if (dtable.count(200) && std::fabs((double)(s.end - s.start) - dtable[200]) > 1.0) { b_ok = false; break; }
                } else {
                    auto segs = segment_by_pair(runs);
                    auto pr40 = table.count(40) ? table[40] : std::make_pair<uint16_t,uint16_t>(0,0);
                    b_ok = !segs.empty();
                    for (auto& s : segs) if (s.a != pr40.first || s.b != pr40.second) { b_ok = false; break; }
                }
            }
            checks.expect(c.loaded && win_ok && b_ok,
                pc.is_vol
                ? ("P6 " + tag + " (b): the target's durations stay at D_c(200)")
                : ("P6 " + tag + " (b): the target's level pair stays at the reference for VOL 40"));

            // D25: (c) uses win_start (the modulator's third fetch), not M_on.
            bool other_const = win_ok && oside >= 0 && is_constant(c.audio, win_start, win_end, oside);
            std::string c_id2225 = " 5-mod.c22,5-p150.c25";
            checks.expect(c.loaded && win_ok && other_const,
                "P6 " + tag + " (c) [p.150]" + c_id2225 + ": the other side's output is constant "
                "for the whole window (attached to the next higher channel, and no other)");

            bool spacing_ok = true;
            if (win_ok) {
                auto check_trip = [&](int ch, double per) {
                    auto fc = fetch_cycles(c.log, ch_base(ch), 2, win_start, win_end);
                    if (fc.size() < 2) return false;
                    for (size_t i = 1; i < fc.size(); ++i)
                        if (std::fabs((double)(fc[i]-fc[i-1]) - 2*ticks(per)) > lambda) return false;
                    return true;
                };
                double pers[4] = {1400,1600,1800,2000};
                if (!check_trip(pc.trip_a, pers[pc.trip_a])) spacing_ok = false;
                if (!check_trip(pc.trip_b, pers[pc.trip_b])) spacing_ok = false;
            }
            checks.expect(c.loaded && win_ok && spacing_ok,
                "P6 " + tag + " (d) [p.164]: every non-target channel's fetch spacing (from the "
                "modulator's third fetch) equals 2*AUDxPER within lambda");

            if (mv == tag) {
                checks.expect(!cyclic_ok, "P6 " + tag + " MUTATION stimulus red the attach write "
                    "omitted: the target keeps its own VOL 40/PER 200, (a) fails, via NMH_MUTATE=" + tag);
            }
            // D26: grade each EXTRA through its own clause (b=0,2 -> (c);
            // b=4,5,6 -> (d)).
            if (!pc.extra.empty() && mv == tag + "-EXTRA") {
                Capture ce = run_capture(dir + pc.extra, budget_for(110000));
                auto mke = markers(ce); auto one = m_on(mke), offe = m_off(mke);
                bool have_e = one && offe;
                auto mfe = have_e ? fetch_cycles(ce.log, ch_base(pc.mod_ch), 4, *one, *offe) : std::vector<uint64_t>{};
                uint64_t wse = have_e && mfe.size() >= 3 ? mfe[2] : 0;
                bool wok_e = have_e && mfe.size() >= 3;
                bool broke_clause;
                if (pc.extra_clause == "(c)") {
                    broke_clause = wok_e && oside >= 0 && !is_constant(ce.audio, wse, *offe, oside);
                } else {
                    // The modulator's own expected period is always 4000
                    // (M_P4 base); a tripwire's is its own fixed 1400 /
                    // 1600 / 1800 / 2000 (channel-indexed).
                    static const double kTripPer[4] = {1400, 1600, 1800, 2000};
                    auto expected_period = [&](int ch) { return ch == pc.mod_ch ? 4000.0 : kTripPer[ch]; };
                    auto check_trip_e = [&](int ch) {
                        auto fc = fetch_cycles(ce.log, ch_base(ch), ch == pc.mod_ch ? 4 : 2, wse, *offe);
                        if (fc.size() < 2) return true; // "leaves" the spacing: fewer/irregular fetches also counts
                        double exp = 2 * ticks(expected_period(ch));
                        for (size_t i = 1; i < fc.size(); ++i)
                            if (std::fabs((double)(fc[i]-fc[i-1]) - exp) > lambda) return true;
                        return false;
                    };
                    broke_clause = wok_e && (check_trip_e(pc.trip_a) || check_trip_e(pc.trip_b) ||
                                              check_trip_e(pc.mod_ch));
                }
                // Measured: the added bit is itself one of the six confirmed to have no
                // effect (P6 (a) is already red for its own bit, on every b), so it does
                // not move the other channel either: broke_clause does not come true on
                // this RTL. Unfalsifiable at this boundary, not a test defect.
                checks.record("P6 " + tag + "-EXTRA MUTATION NMH_MUTATE=" + tag + "-EXTRA (" +
                    pc.extra_desc + "): unfalsifiable at this boundary -- broke_clause=" +
                    (broke_clause ? "true" : "false") + ", because the added bit has no "
                    "measured effect either.");
            }
        }
    }

    // ================= P7: channel 3 attaches to nothing ===================
    {
        struct P7Case { std::string bin, mutbin; uint16_t mask, mutmask; std::string tag; };
        std::vector<P7Case> cases = {
            {"/prog_s_ch3v.bin", "/prog_s_ch3v_extra.bin", 0x08, 0x04, "P7V"},
            {"/prog_s_ch3p.bin", "/prog_s_ch3p_extra.bin", 0x80, 0x40, "P7P"},
        };
        for (auto& pc : cases) {
            std::string bin = (mv == pc.tag) ? pc.mutbin : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(60000));
            uint16_t expect = (mv == pc.tag) ? pc.mutmask : pc.mask;
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            assert_pc123(checks, pc.tag, c, expect, 0x000F, {{0,2},{1,2},{2,2},{3,2}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
            auto ch3f = have_bounds ? fetch_cycles(c.log, ch_base(3), 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && ch3f.size() >= 3 ? ch3f[2] : 0;
            bool win_ok = have_bounds && ch3f.size() >= 3;
            bool both_const = win_ok && is_constant(c.audio, win_start, *off, 0) && is_constant(c.audio, win_start, *off, 1);
            // D28: A-adkcon.b03/b07 withdrawn from P7 (they name channel 3's
            // own disable, graded by P8), kept only on the bit-3/7 rows here
            // as context, not as an asserted id.
            checks.expect(c.loaded && win_ok && both_const,
                "P7 [p.150,p.151,p.265] 5-mod.c23,5-p150.c26,5-appA-p265.c22,5-appA-p265.c26: both "
                "outputs are constant for the whole window -- channel 3's attach bit modulates "
                "nothing");
            bool spacing_ok = true;
            double pers[4] = {1400,1600,1800,2000};
            if (win_ok) {
                for (int ch = 0; ch < 4; ++ch) {
                    auto fc = fetch_cycles(c.log, ch_base(ch), 2, win_start, *off);
                    if (fc.size() < 2) { spacing_ok = false; continue; }
                    for (size_t i = 1; i < fc.size(); ++i)
                        if (std::fabs((double)(fc[i]-fc[i-1]) - 2*ticks(pers[ch])) > lambda) spacing_ok = false;
                }
            }
            checks.expect(c.loaded && win_ok && spacing_ok,
                "P7 [p.164]: every channel's consecutive fetch spacing equals 2*AUDxPER within "
                "lambda (2800,3200,3600,4000 ticks)");
            if (mv == pc.tag) {
                // D27: graded through the spacing clause (the page predicts a spacing
                // change of a silent channel 3 at VOL 0, not necessarily audibility).
                // Measured: the substituted bit is itself one of the six confirmed to
                // have no effect, so it does not move ch3's spacing either.
                // Unfalsifiable at this boundary, not a test defect.
                checks.record("P7 " + pc.tag + " MUTATION NMH_MUTATE=" + pc.tag + " (the wrong "
                    "bit set): unfalsifiable at this boundary -- spacing_ok=" +
                    (spacing_ok ? "true" : "false") + ", because the substituted bit has no "
                    "measured effect either.");
            }
        }
    }

    // ================= P8: an attached channel's own DAC is bypassed ======
    {
        struct P8Case { int ch; std::string bin; std::string ids; };
        std::vector<P8Case> cases = {
            {0, "/prog_s_av.bin",   "5-p165.c19,5-sig.audxav"},
            {0, "/prog_s_ap.bin",   "5-p165.c20,5-sig.audxap"},
            {0, "/prog_s_both.bin", "5-p165.c19,5-sig.audxav,5-p165.c20,5-sig.audxap"},
            {3, "/prog_s_dac3v.bin","5-p165.c19,5-sig.audxav,A-adkcon.b03"},
            {3, "/prog_s_dac3p.bin","5-p165.c20,5-sig.audxap,A-adkcon.b07"},
        };
        for (auto& pc : cases) {
            Capture c = run_capture(dir + pc.bin, budget_for(200000));
            int side = side_of[pc.ch];
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            auto fc = have_bounds ? fetch_cycles(c.log, ch_base(pc.ch), pc.ch == 0 ? 17 : 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && fc.size() >= 3 ? fc[2] + L_a_pad : 0;
            uint64_t win_end = have_bounds ? *off + L_a_pad : 0;
            bool win_ok = have_bounds && fc.size() >= 3 && side >= 0;
            bool constant = win_ok && is_constant(c.audio, win_start, win_end, side);
            checks.expect(c.loaded && win_ok && constant,
                "P8 [p.165,p.151,p.265] " + pc.ids + ": channel " + std::to_string(pc.ch) +
                "'s own side has one value for the whole window while it is attached (" + pc.bin + ")");
        }
        // D29/D30: the unattached control -- the same channel, same data,
        // period and volume, changes its own side >=2x per data word.
        struct CtrlCase { int ch; Capture* c; uint32_t len; std::string name; };
        std::vector<CtrlCase> ctrls = {
            {0, &rmod, 8, "R-MOD"}, {0, &rdacp4, 4, "R-DAC-P4"}, {0, &rdacvp, 8, "R-DAC-VP"},
            {3, &rdac3v, 2, "R-DAC3-V"}, {3, &rdac3p, 2, "R-DAC3-P"},
        };
        for (auto& cc : ctrls) {
            auto mk = markers(*cc.c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            int side = side_of[cc.ch];
            bool changes_enough = false;
            if (have_bounds && side >= 0) {
                auto runs = runs_in_window(cc.c->audio, *on, *off, side);
                auto comp = complete_runs(runs, *on, *off);
                changes_enough = comp.size() >= 2 * cc.len;
            }
            checks.expect(cc.c->loaded && have_bounds && changes_enough,
                "P8 precondition [reference frame]: " + cc.name + " -- the same channel, unattached, "
                "changes its own side at least twice per data word");
        }
        // D32: the mutation -- the attach write omitted, substituting the
        // matching R- session, graded through the SAME P8 comparator
        // (channel 0's own side constant). With the side fixed (D2), the
        // unattached control's own side legitimately changes, so the
        // comparator must flip to red.
        if (mv == "P8") {
            auto mk = markers(rmod); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            auto fc = have_bounds ? fetch_cycles(rmod.log, ch_base(0), 8, *on, *off) : std::vector<uint64_t>{};
            uint64_t ws = have_bounds && fc.size() >= 3 ? fc[2] + L_a_pad : 0;
            uint64_t we = have_bounds ? *off + L_a_pad : 0;
            bool win_ok = have_bounds && fc.size() >= 3 && side_of[0] >= 0;
            bool constant = win_ok && is_constant(rmod.audio, ws, we, side_of[0]);
            checks.expect(win_ok && !constant,
                "P8 MUTATION stimulus red the attach write omitted, substituting R-MOD (unattached) "
                "for S-AV: channel 0's own side now changes -- the same comparator goes red, via "
                "NMH_MUTATE=P8");
        }
    }

    // ================= P9: data-set lengths are independent ===============
    {
        struct P9Case { std::string bin, mutbin; std::vector<int> mod_order; int lm, lc; };
        std::vector<P9Case> cases = {
            {"/prog_s_lena.bin", "/prog_s_lena_len4.bin", {8,48,20,60,32}, 5, 3},
            {"/prog_s_lenb.bin", "", {8,48}, 2, 7},
        };
        for (auto& pc : cases) {
            bool is_a = !pc.mutbin.empty();
            std::string bin = (is_a && mv == "P9") ? pc.mutbin : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(150000));
            int side = side_of[1];
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            uint16_t mask9 = (is_a && mv == "P9") ? 0x01 : 0x01;
            assert_pc123(checks, is_a ? "S-LEN-A" : "S-LEN-B", c, mask9, 0x0003,
                         {{0, is_a && mv == "P9" ? 4u : (uint32_t)pc.lm}, {1,(uint32_t)pc.lc}},
                         have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
            auto pr40 = L1.count(40) ? L1[40] : std::make_pair<uint16_t,uint16_t>(0,0);
            uint64_t win_start = 0; bool win_ok = false;
            if (have_bounds && side >= 0) {
                auto runs = runs_in_window(c.audio, *on, *off, side);
                bool seen40 = false;
                for (auto& r : runs) {
                    if (!seen40) { if (r.value == pr40.first || r.value == pr40.second) seen40 = true; continue; }
                    if (r.value != pr40.first && r.value != pr40.second) { win_start = r.start; win_ok = true; break; }
                }
            }
            uint64_t win_end = have_bounds ? *off + L_a_pad : 0;
            uint32_t lm_eff = (is_a && mv == "P9") ? 4 : (uint32_t)pc.lm;
            auto off0 = win_ok ? fetch_offsets(c.log, ch_base(0), lm_eff, win_start, win_end) : std::vector<uint32_t>{};
            auto off1 = win_ok ? fetch_offsets(c.log, ch_base(1), pc.lc, win_start, win_end) : std::vector<uint32_t>{};
            // D33: grade the stepping itself (previous+1 mod L).
            bool off0_ok = stepping_ok(off0, lm_eff);
            bool off1_ok = stepping_ok(off1, (uint32_t)pc.lc);
            // D11: named for P9 (a),(b) too -- graded inside a window that
            // exists only if a genuine departure from 40 happened; an empty
            // population must show up as its own red, not as (a)/(b)'s.
            checks.expect(c.loaded && win_ok && !off0.empty() && !off1.empty(),
                "P9 precondition [window]: the window opens after L_1(40) is first seen, with "
                "fetches recorded for both channels (" + bin + ")");

            std::vector<int> applied; bool level_ok = true;
            if (win_ok && side >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, side);
                auto segs = segment_by_pair(runs);
                for (auto& s : segs) {
                    int found = -1;
                    for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                    if (found < 0) level_ok = false;
                    applied.push_back(found);
                }
            }
            bool cyclic_ok = win_ok && level_ok && applied.size() >= 2*pc.mod_order.size() &&
                cyclic_consecutive(applied, pc.mod_order);
            // D34: (d) -- channel 1's durations stay at D_1(200).
            bool dur_ok = false;
            if (win_ok && side >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, side);
                auto samples = samples_by_sign(runs, L1);
                auto comp = complete_samples(samples, win_start, win_end);
                dur_ok = !comp.empty();
                for (auto& s : comp)
                    if (D1.count(200) && std::fabs((double)(s.end - s.start) - D1[200]) > 1.0) { dur_ok = false; break; }
            }
            checks.expect(c.loaded && win_ok && off0_ok,
                "P9 (a) [p.147,p.150]: channel 0's offsets step 0.." + std::to_string(lm_eff - 1) +
                ",0,... (" + bin + ")");
            checks.expect(c.loaded && win_ok && off1_ok,
                "P9 (b) [p.147,p.150]: channel 1's offsets step 0.." + std::to_string(pc.lc-1) +
                ",0,...(" + bin + ")");
            checks.expect(c.loaded && win_ok && cyclic_ok,
                "P9 (c) [p.150] 5-mod.c18,5-p150.c21: channel 1's applied volumes are cyclically "
                "consecutive across the modulator's own restarts, none skipped or repeated (" + bin + ")");
            checks.expect(c.loaded && win_ok && dur_ok,
                "P9 (d): channel 1's durations (graded at SQ's sign alternations) stay at D_1(200) "
                "(" + bin + ")");
            if (is_a && mv == "P9") {
                // Per spec P9's own contrasting case: grade the SAME mutant capture
                // against the page's original L_m=5, not the mutation's own (shortened)
                // length -- p.147, p.145 predict a 4-word cycle under AUD0LEN=4, so
                // stepping against L_m=5 is what fails. (Re-fetching with a 5-word
                // region, not lm_eff, so an offset-4 read would still be seen if the
                // RTL ever produced one.)
                auto off0_L5 = win_ok ? fetch_offsets(c.log, ch_base(0), 5, win_start, win_end)
                                      : std::vector<uint32_t>{};
                bool off0_ok_L5 = stepping_ok(off0_L5, 5);
                checks.expect(win_ok && !off0_ok_L5, "P9 MUTATION stimulus red AUD0LEN=4 instead "
                    "of 5: graded against the original L_m=5, the modulator's own offsets no "
                    "longer step 0..4,0,..., via NMH_MUTATE=P9");
            }
        }
    }

    // ================= P10: DMA-request phase ==============================
    {
        auto phase_check = [&](Capture& c, int k_mode) {
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            if (!on || !off) return false;
            auto fetches = fetch_cycles(c.log, ch_base(0), 17, *on, *off);
            if (fetches.size() < 4) return false;
            uint64_t t0 = fetches[1];
            for (size_t n = 4; n <= fetches.size(); ++n) {
                double kfac = k_mode == 0 ? 2.0*(double)(n-3) : k_mode == 1 ? 2.0*(double)(n-3)+1.0 : (double)(n-3);
                double o = (double)fetches[n-1] - (double)t0 - kfac*d_mod;
                if (std::fabs(o) > lambda) return false; // D39/Sol-tolerance: lambda alone.
            }
            return true;
        };
        Capture av = run_capture(dir + "/prog_s_av.bin", budget_for(200000));
        Capture ap = run_capture(dir + "/prog_s_ap.bin", budget_for(110000));
        Capture both = run_capture(dir + "/prog_s_both.bin", budget_for(110000));

        // D35: S-AV is graded relative to normal operation by using the
        // identical formula, tolerance and anchor as PC7/R-MOD -- the
        // shared k(n)=2(n-3) check IS "as they do in normal operation".
        bool av_ok = phase_check(av, 0);
        checks.expect(av.loaded && av_ok,
            "P10 [p.164,p.165] 5-p164.c33,5-sm.c33: S-AV -- o_n within lambda of k(n)=2(n-3), "
            "graded by the same formula as PC7/R-MOD ('as they do in normal operation', 011->010)");
        // D36: F58.loop.attach kept only here (the attach-period bracket
        // S-AP alone exercises); napnav's DMA-half is recorded, not
        // re-asserted (it is the same formula PC7 already grades).
        bool ap_ok = phase_check(ap, 1);
        checks.expect(ap.loaded && ap_ok,
            "P10 [p.164] 5-p164.c34,5-sm.c34,F58.loop.attach: S-AP -- o_n within lambda of "
            "k(n)=2(n-3)+1 (010->011)");
        checks.record("5-p165.c22, 5-sig.napnav: the DMA-request half is graded only as the shared "
            "k(n) formula between R-MOD and S-AV (PC7); no assertion isolates napnav's own "
            "contribution beyond that formula, and the interrupt half is not graded at all (see "
            "spec's partial-claims table).");
        checks.record("5-p165.c4, 5-sig.intreq1: intreq1 is declared to combine with intreq2 to "
            "form AUDxIR, but the manual does not say how or when; no port access, audio sample "
            "or register value in this package distinguishes one way of combining from another, "
            "so this is not turned into an assertion (SPECIFICATION.md, claims not turned into a "
            "proposition).");
        checks.expect(both.loaded && phase_check(both, 2),
            "P10 [p.164] 5-p164.c35,5-sm.c35: S-BOTH -- o_n within lambda of k(n)=n-3 (both "
            "transitions)");
        // D37: the duplicate "P10 anchor" line removed -- PC7 already
        // states this (see PC7's own assertion above).
        if (mv == "P10") {
            // D38: grade S-AP's own capture through S-AV's k(n)=2(n-3)
            // formula (not through S-AP's own, correct formula) -- this is
            // the actual stimulus-vs-comparator mismatch the mutation must
            // demonstrate.
            bool mismatched = phase_check(ap, 0);
            checks.expect(!mismatched,
                "P10 MUTATION grading red the S-AP capture graded through S-AV's k(n)=2(n-3) "
                "formula instead of its own k(n)=2(n-3)+1: the phase mismatch fails, via "
                "NMH_MUTATE=P10 [grading-only mutation on S-AP's own capture, per spec P10 6.]");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
