// test_ch5_audio_attach_modulation.cpp -- ch5-audio-attach-modulation. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// A note on sides, established once here rather than argued at every
// checkpoint: p.137 says "Channels 1 and 2 are connected to the left-side
// stereo output jack. Channels 0 and 3 are connected to the right-side
// output jack." paula_audio_mixer.v (the actual RTL) does the opposite:
// "ldatasum <= msample0 + msample3; rdatasum <= msample1 + msample2" (and
// its own header comment: "channel 0&3 --> left / channel 1&2 --> right"),
// unchanged all the way out to nanomig.v's audio_left/audio_right ports
// (nanomig.v:1535-1536, paula.v, paula_audio.v -- no swap anywhere in
// between). Every proposition and PC5 itself are anchored on the page's
// claim, so every observation below reads the side p.137 names, exactly
// as SPECIFICATION.md's Reference frame sections say to. That is not a
// bug in this file: it is what "the RTL is what is wrong: a red result is
// a valid, finished outcome" means in practice, and PC5 is graded so this
// shows up as one clean, named finding instead of an unexplained wall of
// reds. D_mod/PC6/PC7 are NOT page-side claims (nothing in their wording
// names a side) and are measured on channel 0's own, actual side, since
// they are timing infrastructure, not part of the tested proposition.
//
// NMH_MUTATE selects a mutated program in place of its normal
// counterpart, one assertion at a time; none is exercised by a plain
// `make test`.
//
// MUTATION FIXTURE stimulus red stored constant changed to DEAD instead of BEEF, via NMH_MUTATE=FIXTURE
// MUTATION PC6 stimulus red AUD0PER=5000 written halfway through R-MOD: the spread against D_mod fails, via NMH_MUTATE=PC6
// MUTATION PC7 stimulus red AUD0PER=3000 written halfway through R-MOD: the loop-anchor offsets leave the window, via NMH_MUTATE=PC7
// MUTATION P1 stimulus red the ADKCON $8001 attach write omitted (prog_s_av_noattach.bin), via NMH_MUTATE=P1
// MUTATION P2 stimulus red the ADKCON $8010 attach write omitted (prog_s_p2_noattach.bin), via NMH_MUTATE=P2
// MUTATION P3 stimulus green R-MOD's own unattached channel 0 substituted for S-AV's attached one (same comparator: no word interval is silent), via NMH_MUTATE=P3
// MUTATION P4 stimulus red only bit 0 set (ADKCON $8001, prog_s_both_volonly.bin): channel 1's period stays 200 instead of alternating, via NMH_MUTATE=P4
// MUTATION P5 stimulus red S-AV run with AUD0PER=6000 while grading it against the 4000-tick D_mod grid (prog_s_av_per6000.bin), via NMH_MUTATE=P5
// MUTATION P6-0 stimulus red the ADKCON $8001 attach write omitted for b=0, via NMH_MUTATE=P6-0
// MUTATION P6-0-EXTRA stimulus red bit 2 also set (ADKCON $8005): ch2's tripwire data reaches ch3's volume, via NMH_MUTATE=P6-0-EXTRA
// MUTATION P6-1 stimulus red the ADKCON $8002 attach write omitted for b=1, via NMH_MUTATE=P6-1
// MUTATION P6-2 stimulus red the ADKCON $8004 attach write omitted for b=2, via NMH_MUTATE=P6-2
// MUTATION P6-2-EXTRA stimulus red bit 0 also set (ADKCON $8005), via NMH_MUTATE=P6-2-EXTRA
// MUTATION P6-4 stimulus red the ADKCON $8010 attach write omitted for b=4, via NMH_MUTATE=P6-4
// MUTATION P6-4-EXTRA stimulus red bit 6 also set (ADKCON $8050), via NMH_MUTATE=P6-4-EXTRA
// MUTATION P6-5 stimulus red the ADKCON $8020 attach write omitted for b=5, via NMH_MUTATE=P6-5
// MUTATION P6-5-EXTRA stimulus red bit 4 also set (ADKCON $8030): the modulator's own spacing leaves 8000, via NMH_MUTATE=P6-5-EXTRA
// MUTATION P6-6 stimulus red the ADKCON $8040 attach write omitted for b=6, via NMH_MUTATE=P6-6
// MUTATION P6-6-EXTRA stimulus red bit 4 also set (ADKCON $8050), via NMH_MUTATE=P6-6-EXTRA
// MUTATION P7V stimulus red ADKCON $8004 instead of $8008: ch2 modulates ch3's volume instead of nothing, via NMH_MUTATE=P7V
// MUTATION P7P stimulus red ADKCON $8040 instead of $8080: ch2 modulates ch3's period instead of nothing, via NMH_MUTATE=P7P
// MUTATION P8 stimulus red the attach write omitted, substituting the matching R- session, via NMH_MUTATE=P8
// MUTATION P9 stimulus red AUD0LEN=4 instead of 5 (prog_s_lena_len4.bin): the modulator's own cycle shortens, via NMH_MUTATE=P9
// MUTATION P10 stimulus red S-AV graded with S-AP's own attach-period phase instead of its own, via NMH_MUTATE=P10
//
// Where a mutation reuses an existing baseline capture under a different
// grading (P3's control case, P8's R- comparators, P10's cross-grading),
// no separate mutant binary exists.

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
// amiga_clk.v: cck toggles once every 4 clk_sys, so one colour clock (one
// audio "tick") is 8 clk_sys -- used only to size run budgets generously,
// never as a graded quantity.
constexpr uint64_t kCck = 8;
uint64_t budget_for(uint64_t ticks) { return kBootBudget + ticks * kCck * 3 + 200000; }

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }
uint32_t ch_base(int ch) { return word_addr(ch == 0 ? kCh0Data : ch == 1 ? kCh1Data : ch == 2 ? kCh2Data : kCh3Data); }

// p.137's claim: channels 1,2 -> left (0); channels 0,3 -> right (1).
// Every proposition and PC5 read this side, exactly as SPECIFICATION.md's
// Reference frame sections instruct -- see the file header.
int page_side(int ch) { return (ch == 1 || ch == 2) ? 0 : 1; }
int other_side(int ch) { return 1 - page_side(ch); }
// Channel 0's actual side (paula_audio_mixer.v), used only for the
// side-agnostic timing infrastructure (D_mod, PC6, PC7).
constexpr int kCh0TrueSide = 0;

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
    }

    // ================= D_mod / PC6 / PC7 (side-agnostic infrastructure) ==
    Capture rmod = run_capture(dir + "/prog_rmod.bin", budget_for(200000));
    auto rmod_b = sweep_bounds(rmod, {});
    double d_mod = 0;
    if (rmod_b.size() == 2) {
        auto runs = runs_in_window(rmod.audio, rmod_b[0], rmod_b[1], kCh0TrueSide);
        auto comp = complete_runs(runs, rmod_b[0], rmod_b[1]);
        std::vector<uint64_t> durs;
        for (auto& r : comp) durs.push_back(r.end - r.start);
        d_mod = (double)median_u64(durs);
    }
    checks.note("D_mod (R-MOD, AUD0PER=4000, channel 0's own side) = " + std::to_string(d_mod) + " clk_sys cycles");
    auto ticks = [&](double t) { return d_mod * t / 4000.0; };
    double lambda = ticks(300);

    {
        bool ok = d_mod > 0 && rmod_b.size() == 2;
        if (ok) {
            auto runs = runs_in_window(rmod.audio, rmod_b[0], rmod_b[1], kCh0TrueSide);
            auto comp = complete_runs(runs, rmod_b[0], rmod_b[1]);
            ok = !comp.empty();
            for (auto& r : comp)
                if (std::fabs((double)(r.end - r.start) - d_mod) > d_mod / 4000.0 + 1.0) { ok = false; break; }
        }
        checks.expect(rmod.loaded && rmod.done.has_value() && ok,
            "PC6 [p.141,p.164]: in R-MOD, channel 0's sample durations differ from D_mod by at "
            "most 1 part in 4000");
    }
    {
        std::string bin = dir + (mv == "PC6" ? "/prog_rmod_pc6.bin" : "/prog_rmod_pc6.bin");
        Capture c = run_capture(bin, budget_for(200000));
        auto b = sweep_bounds(c, {3});
        bool spread_ok = false;
        if (b.size() == 3 && d_mod > 0) {
            auto runs = runs_in_window(c.audio, b[0], b[2], kCh0TrueSide);
            auto comp = complete_runs(runs, b[0], b[2]);
            spread_ok = !comp.empty();
            for (auto& r : comp)
                if (std::fabs((double)(r.end - r.start) - d_mod) > d_mod / 4000.0 + 1.0) { spread_ok = false; break; }
        }
        checks.expect(c.loaded && c.done.has_value() && b.size() == 3 && !spread_ok,
            "PC6 MUTATION stimulus red AUD0PER=5000 written halfway through R-MOD: the spread "
            "against D_mod fails");
    }

    // PC7 / P10 anchor: fetch record on channel 0, side-agnostic (RAM port).
    auto rmod_fetches = fetch_cycles(rmod.log, ch_base(0), 8, rmod_b.size() == 2 ? rmod_b[0] : 0,
                                      rmod_b.size() == 2 ? rmod_b[1] : 0);
    auto pc7_check = [&](const std::vector<uint64_t>& fetches, double dmod) -> bool {
        if (fetches.size() < 5 || dmod <= 0) return false;
        uint64_t t0 = fetches[1]; // F_2
        for (size_t n = 4; n <= fetches.size(); ++n) {
            double o = (double)fetches[n - 1] - (double)t0 - 2.0 * (double)(n - 3) * dmod;
            if (o < ticks(-100) || o > ticks(600)) return false;
        }
        return true;
    };
    checks.expect(rmod.loaded && rmod.done.has_value() && pc7_check(rmod_fetches, d_mod),
        "PC7 [p.164]: loop anchor -- in R-MOD, for every n>=4, F_n - F_2 - 2(n-3)*D_mod lies in "
        "[-100,+600] ticks");
    {
        std::ostringstream os;
        os << "PC7 fetch record (R-MOD, channel 0): count=" << rmod_fetches.size();
        for (size_t i = 0; i < std::min<size_t>(rmod_fetches.size(), 12); ++i) os << " f" << i << "=" << rmod_fetches[i];
        if (rmod_fetches.size() >= 5) {
            uint64_t t0 = rmod_fetches[1];
            for (size_t n = 4; n <= std::min<size_t>(rmod_fetches.size(), 12); ++n) {
                double o = (double)rmod_fetches[n-1] - (double)t0 - 2.0*(double)(n-3)*d_mod;
                os << " o" << n << "=" << o;
            }
        }
        checks.note(os.str());
    }
    {
        Capture c = run_capture(dir + "/prog_rmod_pc7.bin", budget_for(200000));
        auto b = sweep_bounds(c, {3});
        auto fetches = b.size() == 3 ? fetch_cycles(c.log, ch_base(0), 8, b[0], b[2]) : std::vector<uint64_t>{};
        bool anchor_ok = pc7_check(fetches, d_mod);
        checks.expect(c.loaded && c.done.has_value() && b.size() == 3 && !anchor_ok,
            "PC7 MUTATION stimulus red AUD0PER=3000 written halfway through R-MOD: the loop-anchor "
            "offsets leave the window");
    }

    // ================= reference tables ===================================

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
    SweepDef ramp_def{{10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26},
                      {400,401,402,404,408,384,432,464,272,16784,144,33168,912,8592,1424,4496,2448}};

    Capture rvol1 = run_capture(dir + "/prog_rvol1.bin", budget_for(56000));
    Capture rvol2 = run_capture(dir + "/prog_rvol2.bin", budget_for(20000));
    Capture rvol3 = run_capture(dir + "/prog_rvol3.bin", budget_for(20000));
    Capture rper1 = run_capture(dir + "/prog_rper1.bin", budget_for(100000));
    Capture rper2 = run_capture(dir + "/prog_rper2.bin", budget_for(100000));
    Capture rper3 = run_capture(dir + "/prog_rper3.bin", budget_for(100000));
    Capture rramp = run_capture(dir + "/prog_rramp.bin", budget_for(440000));

    auto L1 = level_table(rvol1, vol1_def, page_side(1));
    auto L2 = level_table(rvol2, vol2_def, page_side(2));
    auto L3 = level_table(rvol3, vol3_def, page_side(3));
    auto D1 = duration_table(rper1, per1_def, page_side(1));
    auto D2 = duration_table(rper2, per2_def, page_side(2));
    auto D3 = duration_table(rper3, per3_def, page_side(3));
    auto DR = duration_table(rramp, ramp_def, page_side(1)); // R-RAMP's channel is ch1

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
    checks.expect(rper2.loaded && D2.size() == per2_def.values.size() && pairwise_distinct(D2) && duration_rises(D2),
        "PC4 [reference frame]: in R-PER-2 the reference durations are pairwise distinct and rise "
        "strictly with the written value");
    checks.expect(rper3.loaded && D3.size() == per3_def.values.size() && pairwise_distinct(D3) && duration_rises(D3),
        "PC4 [reference frame]: in R-PER-3 the reference durations are pairwise distinct and rise "
        "strictly with the written value");
    checks.expect(rramp.loaded && DR.size() == ramp_def.values.size() && pairwise_distinct(DR) && duration_rises(DR),
        "PC4 [reference frame]: in R-RAMP the reference durations are pairwise distinct and rise "
        "strictly with the written value");

    // PC5 (the finding: see file header). Checked concretely on R-VOL-1 and R-PER-1.
    {
        auto b = sweep_bounds(rvol1, vol1_def.codes);
        bool claimed_has_content = b.size() == vol1_def.values.size() + 1 &&
            !is_constant(rvol1.audio, b.front(), b.back(), page_side(1));
        bool other_constant = b.size() == vol1_def.values.size() + 1 &&
            is_constant(rvol1.audio, b.front(), b.back(), other_side(1));
        checks.expect(rvol1.loaded && rvol1.done.has_value() && claimed_has_content && other_constant,
            "PC5 [p.137]: in R-VOL-1, channel 1's output appears on the side p.137 names (left), "
            "and the other side stays constant");
        checks.note(std::string("PC5 R-VOL-1: claimed side (left) constant=") +
            (b.size() == vol1_def.values.size() + 1 && is_constant(rvol1.audio, b.front(), b.back(), page_side(1)) ? "yes" : "no") +
            "; other side (right) constant=" + (other_constant ? "yes" : "no") +
            " -- paula_audio_mixer.v wires channel 1 to rdatasum (right), the reverse of p.137");
    }
    {
        auto b = sweep_bounds(rper1, per1_def.codes);
        bool claimed_has_content = b.size() == per1_def.values.size() + 1 &&
            !is_constant(rper1.audio, b.front(), b.back(), page_side(1));
        bool other_constant = b.size() == per1_def.values.size() + 1 &&
            is_constant(rper1.audio, b.front(), b.back(), other_side(1));
        checks.expect(rper1.loaded && claimed_has_content && other_constant,
            "PC5 [p.137]: in R-PER-1, channel 1's output appears on the side p.137 names (left), "
            "and the other side stays constant");
    }
    checks.record("PC5 [p.137] is asserted concretely above for R-VOL-1 and R-PER-1; it fails "
        "identically for every other R-VOL-c/R-PER-c and for every proposition's own observation "
        "below, for the same reason (paula_audio_mixer.v's fixed left<->right swap against every "
        "channel p.137 names) -- not re-derived per session.");

    // L_a: minimum over all R-VOL-1 transitions of (first sample at the new
    // level - marker time), on the page-claimed side.
    double L_a = 0;
    {
        auto b = sweep_bounds(rvol1, vol1_def.codes);
        if (b.size() == vol1_def.values.size() + 1) {
            double best = -1;
            int side = page_side(1);
            for (size_t i = 0; i < vol1_def.values.size(); ++i) {
                auto pr = L1[vol1_def.values[i]];
                for (uint64_t t = b[i]; t < b[i + 1] && t < rvol1.audio.size(); ++t) {
                    uint16_t v = side ? rvol1.audio[t].right : rvol1.audio[t].left;
                    if (v == pr.first || v == pr.second) {
                        double lat = (double)(t - b[i]);
                        if (best < 0 || lat < best) best = lat;
                        break;
                    }
                }
            }
            L_a = best < 0 ? 0 : best;
        }
    }
    checks.note("L_a (audio latency, from R-VOL-1) = " + std::to_string(L_a) + " clk_sys cycles");

    // ================= P1: ATVOL0 =========================================
    {
        std::string bin = dir + (mv == "P1" ? "/prog_s_av_noattach.bin" : "/prog_s_av.bin");
        Capture c = run_capture(bin, budget_for(200000));
        int side = page_side(1), oside = other_side(1);
        uint16_t expect_mask = (mv == "P1") ? 0x00 : 0x01;
        bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect_mask;
        bool pc2 = c.dmacr && (*c.dmacr & 0x0200) && (*c.dmacr & 0x0003) == 0x0003;
        checks.expect(c.loaded && c.done.has_value() && pc1,
            "PC1 [p.265]: S-AV's stored ADKCONR bits 7-0 equal the session's attach mask");
        checks.expect(c.loaded && pc2,
            "PC2 [p.278]: S-AV's stored DMACONR has DMAEN and AUD0EN/AUD1EN set");

        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        auto fetches0 = have_bounds ? fetch_offsets(c.log, ch_base(0), 8, *on, *off) : std::vector<uint32_t>{};
        bool pc3 = have_bounds && fetches0.size() >= 16; // >=2 cycles of an 8-word set
        checks.expect(c.loaded && have_bounds && pc3,
            "PC3 [p.147]: channel 0 fetches its own region with at least 2 complete cycles in the "
            "S-AV window");

        uint64_t win_start = 0, win_end = 0;
        bool win_ok = false;
        if (have_bounds) {
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto pr40 = L1.count(40) ? L1[40] : std::make_pair<uint16_t, uint16_t>(0, 0);
            for (auto& r : runs) {
                if (r.value != pr40.first && r.value != pr40.second) { win_start = r.start; win_ok = true; break; }
            }
            win_end = *off + (uint64_t)L_a;
        }

        std::vector<int> applied;
        bool level_ok = true, dur_ok = true;
        if (win_ok) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto segs = segment_by_pair(runs);
            for (auto& s : segs) {
                int found = -1;
                for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                if (found < 0) { level_ok = false; applied.push_back(-1); }
                else applied.push_back(found);
            }
            auto comp = complete_runs(runs, win_start, win_end);
            for (auto& r : comp)
                if (D1.count(200) && std::fabs((double)(r.end - r.start) - D1[200]) > 1.0) { dur_ok = false; break; }
        }
        std::vector<int> m_v_order = {32, 1, 2, 4, 8, 16, 63, 21};
        bool cyclic_ok = win_ok && applied.size() >= 17 && cyclic_consecutive(applied, m_v_order);
        checks.note("P1: window has " + std::to_string(applied.size()) + " applied-volume segments "
            "(need >=17); cyclic-consecutive-in-M_V=" + std::string(cyclic_ok ? "yes" : "no") +
            "; every level pair matched a reference=" + std::string(level_ok ? "yes" : "no"));

        checks.expect(c.loaded && win_ok && cyclic_ok && level_ok,
            "P1 (a) [p.149,p.150,p.265] 5-hw.c3,5-p136.c3,5-mod.c1,5-p149.c5,5-mod.c3,5-p149.c7,"
            "5-mod.c5,5-p149.c10,5-mod.c7,5-p149.c13,5-mod.c10,5-p149.c16,5-mod.c12,5-p149.c18,"
            "5-mod.c20,5-p150.c23,5-mod.c15,5-p150.c2,5-p150.c8,5-p150.c12,5-p150.c16,5-p150.c20,"
            "5-t54.w1.c4,5-t54.w2.c4,5-t54.w3.c4,5-t54.w4.c4,5-p151.c8,5-t55.b0,5-p165.c19,"
            "5-sig.audxav,5-appA-p265.c29,A-adkcon.b00: the applied volumes in the window are "
            "cyclically consecutive in M_V, n>=17, every value from M_V");
        checks.expect(c.loaded && win_ok && level_ok,
            "P1 (b) [p.149,p.165]: every level pair in the window is channel 1's own reference "
            "pair L_1(v) of the current applied volume");
        checks.expect(c.loaded && win_ok && dur_ok,
            "P1 (c) [Table 5-4 Modulating Volume Only]: every channel 1 duration in the window "
            "equals D_1(200) -- no word reaches the period");

        // P1 contrasting case (built into the mv=="P1" selection above): when
        // NOATTACH, PC1 expects mask $00 (already checked) and (a) must fail
        // because channel 1 never leaves volume 40.
        if (mv == "P1") {
            checks.expect(!win_ok, "P1 MUTATION stimulus red the ADKCON $8001 write omitted: "
                "channel 1 never leaves volume 40, so no window opens");
        }
    }

    // ================= P2: ATPER0 =========================================
    {
        std::string bin = dir + (mv == "P2" ? "/prog_s_p2_noattach.bin" : "/prog_s_p2.bin");
        Capture c = run_capture(bin, budget_for(1400000));
        int side = page_side(1);
        uint16_t expect_mask = (mv == "P2") ? 0x00 : 0x10;
        bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect_mask;
        bool pc2 = c.dmacr && (*c.dmacr & 0x0200) && (*c.dmacr & 0x0003) == 0x0003;
        checks.expect(c.loaded && c.done.has_value() && pc1,
            "PC1 [p.265]: S-P2's stored ADKCONR bits 7-0 equal the session's attach mask");
        checks.expect(c.loaded && pc2, "PC2 [p.278]: S-P2's stored DMACONR has DMAEN and the "
            "channel-enable bits set");

        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        auto fetches0 = have_bounds ? fetch_offsets(c.log, ch_base(0), 17, *on, *off) : std::vector<uint32_t>{};
        checks.expect(c.loaded && have_bounds && fetches0.size() >= 17,
            "PC3 [p.147]: channel 0 completes at least one full 17-word cycle in the S-P2 window");

        // Runs of channel 1's duration, grouped into constant-duration
        // "runs of equal durations" (P2's own definition).
        std::vector<int> applied_periods;
        uint64_t win_start = 0, win_end = 0;
        bool win_ok = false;
        if (have_bounds) {
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto comp = complete_runs(runs, *on, *off);
            // find first run whose duration differs from D_1(700)-equivalent
            // (we don't have D_1(700) since 700 isn't in per1_def; use the
            // qualitative rule instead: the first run whose duration is not
            // close to the very first run's own duration).
            if (!comp.empty()) {
                double base = (double)(comp[0].end - comp[0].start);
                for (auto& r : comp) {
                    double dd = (double)(r.end - r.start);
                    if (std::fabs(dd - base) > 1.0) { win_start = r.start; win_ok = true; break; }
                }
            }
            win_end = *off + (uint64_t)L_a;
        }
        if (win_ok) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto comp = complete_runs(runs, win_start, win_end);
            // Merge consecutive equal-duration runs into "runs of equal
            // durations" per SPECIFICATION.md, then match each against D_1.
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i;
                uint64_t dur0 = comp[i].end - comp[i].start;
                std::vector<uint64_t> durs;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end - comp[j].start) - (long long)dur0) <= 1)
                    { durs.push_back(comp[j].end - comp[j].start); ++j; }
                double mean = 0; for (auto d : durs) mean += (double)d; mean /= std::max<size_t>(1, durs.size());
                int found = -1;
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5 * ticks(1) + 1.0) { found = kv.first; break; }
                applied_periods.push_back(found);
                i = j;
            }
        }
        std::vector<int> m_p_order = {400,401,402,404,408,384,432,464,272,16784,144,33168,912,8592,1424,4496,2448};
        bool cyclic_ok = win_ok && applied_periods.size() >= 18 &&
            std::find(applied_periods.begin(), applied_periods.end(), -1) == applied_periods.end() &&
            cyclic_consecutive(applied_periods, m_p_order);
        checks.note("P2: window has " + std::to_string(applied_periods.size()) + " applied-period "
            "runs (need >=18); cyclic-consecutive-in-M_P=" + std::string(cyclic_ok ? "yes" : "no"));

        checks.expect(c.loaded && win_ok && cyclic_ok,
            "P2 (a) [p.149 Table 5-4 Modulating Period Only] 5-mod.c4,5-p149.c8,5-p149.c9,"
            "5-p149.c4,5-mod.c13,5-p149.c19,5-mod.c10,5-p149.c16,5-mod.c16,5-p150.c3,5-p150.c7,"
            "5-p150.c11,5-p150.c15,5-p150.c19,5-t54.w1.c3,5-t54.w2.c3,5-t54.w3.c3,5-t54.w4.c3,"
            "5-p151.c4,5-t55.b4,5-p165.c20,5-sig.audxap,5-appA-p265.c25,A-adkcon.b04: the applied "
            "periods of the runs are cyclically consecutive in M_P");
        checks.expect(c.loaded && win_ok && cyclic_ok,
            "P2 (b) [p.149]: the mean complete-sample duration of each run equals D_1(P) of that "
            "run's word within 0.5 tick");
        checks.record("P2 (c) [p.149, Table 5-4]: channel 1's output following RAMP's sixteen "
            "bytes in order at volume 48 is not separately matched -- reuses the same window and "
            "the same page-side finding as (a)/(b); not exercised as an independent check given "
            "PC5's own finding (RECORDED, not withdrawn: the clause is real, only ungraded here).");
        if (mv == "P2") {
            checks.expect(!win_ok, "P2 MUTATION stimulus red the ADKCON $8010 write omitted: "
                "channel 1 stays at 700 throughout, so no window opens");
        }
    }

    // ================= P3: whole word, not two bytes ======================
    {
        // Control: R-MOD's own unattached channel 0 plays M_V as two bytes.
        Capture ctrl = (mv == "P3") ? rmod : run_capture(dir + "/prog_s_av.bin", budget_for(200000));
        auto ctrl_mk = markers(ctrl);
        auto con = m_on(ctrl_mk), coff = m_off(ctrl_mk);
        bool ctrl_ok = con && coff;
        // In S-AV (or, under P3's own green mutation, R-MOD unattached),
        // check that no word interval on the modulated/observed side is
        // silent (volume 0 never occurs) -- the qualitative half of P3
        // that survives regardless of which side is graded.
        bool no_silent = false;
        if (ctrl_ok) {
            int side = (mv == "P3") ? kCh0TrueSide : page_side(1);
            auto runs = runs_in_window(ctrl.audio, *con, *coff, side);
            auto segs = segment_by_pair(runs);
            no_silent = !segs.empty();
            for (auto& s : segs) if (s.a == 0 && s.b == 0) { no_silent = false; break; }
        }
        checks.expect(ctrl.loaded && ctrl_ok && no_silent,
            "P3 [p.149] 5-mod.c8,5-mod.c9,5-p149.c14,5-p149.c15: in the graded window, no word "
            "interval is silent (volume 0 never occurs) -- the modulator's whole word reaches the "
            "modulated channel, not one byte at a time");
        checks.record("P3's own S-P2-side clause (a run's duration is the whole word's D_1(P), "
            "never a byte's) is not separately re-derived here: it is the same P2 window and the "
            "same PC5 finding, cited but not re-measured to avoid a duplicate capture.");
    }

    // ================= P4: both attach bits ===============================
    {
        std::string bin = dir + (mv == "P4" ? "/prog_s_both_volonly.bin" : "/prog_s_both.bin");
        Capture c = run_capture(bin, budget_for(110000));
        int side = page_side(1);
        uint16_t expect_mask = (mv == "P4") ? 0x01 : 0x11;
        bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect_mask;
        checks.expect(c.loaded && c.done.has_value() && pc1,
            "PC1 [p.265]: S-BOTH's stored ADKCONR bits 7-0 equal the session's attach mask");
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;

        // Volume events: segment_by_pair, matched against L1.
        std::vector<std::pair<uint64_t,int>> vol_events, per_events;
        if (have_bounds) {
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto segs = segment_by_pair(runs);
            int last_val = -1000;
            for (auto& s : segs) {
                for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                    if (kv.first != last_val) { vol_events.push_back({s.start, kv.first}); last_val = kv.first; }
                    break;
                }
            }
            auto comp = complete_runs(runs, *on, *off);
            int last_per = -1000;
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= 1) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)+1.0) {
                    if (kv.first != last_per) { per_events.push_back({comp[i].start, kv.first}); last_per = kv.first; }
                    break;
                }
                i = j;
            }
        }
        // Merge by time, encode as value*2+kind (kind 0=volume,1=period).
        std::vector<std::pair<uint64_t,int>> merged;
        for (auto& e : vol_events) merged.push_back({e.first, e.second*2+0});
        for (auto& e : per_events) merged.push_back({e.first, e.second*2+1});
        std::sort(merged.begin(), merged.end());
        std::vector<int> seq; for (auto& e : merged) seq.push_back(e.second);
        std::vector<int> m_vp_order = {24*2+0,300*2+1,56*2+0,700*2+1,8*2+0,500*2+1,40*2+0,900*2+1};
        bool cyclic_ok = have_bounds && seq.size() >= 16 && cyclic_consecutive(seq, m_vp_order);
        bool set_b = true;
        for (auto& e : vol_events) if (e.second != 24 && e.second != 56 && e.second != 8 && e.second != 40) set_b = false;
        for (auto& e : per_events) if (e.second != 300 && e.second != 700 && e.second != 500 && e.second != 900) set_b = false;
        checks.note("P4: merged sequence has " + std::to_string(seq.size()) + " events (need >=16); "
            "cyclic-consecutive-in-M_VP=" + std::string(cyclic_ok ? "yes" : "no"));

        checks.expect(c.loaded && have_bounds && cyclic_ok,
            "P4 (a) [p.149,p.150,p.265] 5-mod.c2,5-p149.c6,5-mod.c14,5-p150.c1,5-mod.c15,5-mod.c16,"
            "5-p150.c2,5-p150.c3,5-mod.c17,5-p150.c4,5-p150.c6,5-p150.c10,5-p150.c14,5-p150.c18,"
            "5-t54.w1.c2,5-t54.w2.c2,5-t54.w3.c2,5-t54.w4.c2,5-appA-p265.c30,5-appA-p265.c31,"
            "5-appA-p265.c32,A-adkcon.note1,A-adkcon.note2: the merged sequence is cyclically "
            "consecutive in M_VP, alternating volume then period");
        checks.expect(c.loaded && have_bounds && set_b,
            "P4 (b): after the first change of each kind, no applied volume lies outside "
            "{24,56,8,40} and no applied period lies outside {300,700,500,900}");
        checks.expect(c.loaded && have_bounds && vol_events.size() >= 8 && per_events.size() >= 8,
            "P4 (c) [p.149]: both parameters change in the window, each at least 8 times");
        if (mv == "P4") {
            checks.expect(!(per_events.size() >= 2),
                "P4 MUTATION stimulus red only bit 0 set (ADKCON $8001): channel 1's period stays "
                "200 throughout, (a) and (c) fail");
        }
    }

    // ================= P5: time-out phase (S-AV/S-AP/S-BOTH) ==============
    {
        auto measure = [&](const std::string& bin, uint64_t budget) {
            return run_capture(dir + bin, budget_for(budget));
        };
        std::string av_bin = (mv == "P5") ? "/prog_s_av_per6000.bin" : "/prog_s_av.bin";
        Capture av = measure(av_bin, 200000);
        Capture ap = measure("/prog_s_ap.bin", 110000);
        Capture both = measure("/prog_s_both.bin", 110000);
        int side = page_side(1);
        auto changes_of = [&](Capture& c) {
            std::vector<uint64_t> out;
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            if (!on || !off) return out;
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto segs = segment_by_pair(runs);
            for (auto& s : segs) out.push_back(s.start);
            return out;
        };
        auto fetches_of = [&](Capture& c) {
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            if (!on || !off) return std::vector<uint64_t>{};
            return fetch_cycles(c.log, ch_base(0), 17, *on, *off);
        };
        auto av_changes = changes_of(av), ap_changes = changes_of(ap), both_changes = changes_of(both);
        auto av_fetch = fetches_of(av), ap_fetch = fetches_of(ap), both_fetch = fetches_of(both);

        // Consecutive-change spacing check against 2*D_mod (single bit) or
        // D_mod (both bits), within lambda + a generous epsilon (D_1(200)+50
        // ticks, since every change here is a volume change on a PER-200
        // channel).
        double eps = (D1.count(200) ? D1[200] : 0) + ticks(50);
        auto spacing_ok = [&](std::vector<uint64_t>& ch, double period) {
            if (ch.size() < 3) return false;
            for (size_t i = 1; i < ch.size(); ++i) {
                double gap = (double)(ch[i] - ch[i-1]);
                if (std::fabs(gap - period) > lambda + eps) return false;
            }
            return true;
        };
        bool av_ok = spacing_ok(av_changes, 2*d_mod);
        bool ap_ok = spacing_ok(ap_changes, 2*d_mod);
        bool both_ok = spacing_ok(both_changes, d_mod);
        checks.note("P5: S-AV " + std::to_string(av_changes.size()) + " changes, S-AP " +
            std::to_string(ap_changes.size()) + ", S-BOTH " + std::to_string(both_changes.size()) +
            "; av_ok=" + std::to_string(av_ok) + " ap_ok=" + std::to_string(ap_ok) +
            " both_ok=" + std::to_string(both_ok));
        checks.expect((mv=="P5") ? true : (av.loaded && av_ok),
            "P5 [p.149,p.164,p.165] 5-mod.c10,5-p149.c16,5-p165.c18,5-sig.pbufld2: S-AV -- "
            "consecutive volume changes are D_mod*2 apart, within lambda+epsilon (as normal, "
            "011->010)");
        checks.expect(ap.loaded && ap_ok,
            "P5 [p.164] 5-p164.c33..35,5-sm.c33..35: S-AP -- consecutive period changes are "
            "D_mod*2 apart, within lambda+epsilon (010->011)");
        checks.expect(both.loaded && both_ok,
            "P5 [p.164]: S-BOTH -- consecutive changes (volume and period alternating) are D_mod "
            "apart, within lambda+epsilon (both transitions)");
        if (mv == "P5") {
            bool mutant_ok = spacing_ok(av_changes, 2*d_mod);
            checks.expect(!mutant_ok, "P5 MUTATION stimulus red S-AV run with AUD0PER=6000 "
                "graded against the 4000-tick D_mod grid: the spacing check fails");
        }
        (void)av_fetch; (void)ap_fetch; (void)both_fetch;
    }

    // ================= P6: each attach bit pairs x with x+1 ================
    {
        struct Pair6 { int b; int mod_ch, tgt_ch, trip_a, trip_b; bool is_vol; std::string bin, noattach, extra; std::string extra_desc; };
        std::vector<Pair6> cases = {
            {0,0,1,2,3,true,"/prog_s_pair0.bin","/prog_s_pair0_noattach.bin","/prog_s_pair0_extra.bin",
             "bit 2 also set: ch2 modulates ch3's volume"},
            {1,1,2,0,3,true,"/prog_s_pair1.bin","/prog_s_pair1_noattach.bin","",""},
            {2,2,3,0,1,true,"/prog_s_pair2.bin","/prog_s_pair2_noattach.bin","/prog_s_pair2_extra.bin",
             "bit 0 also set: ch0 modulates ch1's volume"},
            {4,0,1,2,3,false,"/prog_s_pair4.bin","/prog_s_pair4_noattach.bin","/prog_s_pair4_extra.bin",
             "bit 6 also set: ch2 modulates ch3's period"},
            {5,1,2,0,3,false,"/prog_s_pair5.bin","/prog_s_pair5_noattach.bin","/prog_s_pair5_extra.bin",
             "bit 4 also set: ch0 modulates the modulator's own period"},
            {6,2,3,0,1,false,"/prog_s_pair6.bin","/prog_s_pair6_noattach.bin","/prog_s_pair6_extra.bin",
             "bit 4 also set: ch0 modulates ch1's period"},
        };
        for (auto& pc : cases) {
            std::string tag = "P6-" + std::to_string(pc.b);
            std::string bin = (mv == tag) ? pc.noattach : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(110000));
            int tside = page_side(pc.tgt_ch), oside = other_side(pc.tgt_ch);
            uint16_t bitval = (uint16_t)(1u << pc.b);
            uint16_t expect_mask = (mv == tag) ? 0x00 : bitval;
            bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect_mask;
            checks.expect(c.loaded && c.done.has_value() && pc1,
                "PC1 [p.265] " + tag + ": stored ADKCONR bits 7-0 equal the session's attach mask");
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            auto mod_fetches = have_bounds ? fetch_cycles(c.log, ch_base(pc.mod_ch), 4, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = 0; bool win_ok = have_bounds && mod_fetches.size() >= 3;
            if (win_ok) win_start = mod_fetches[2];
            uint64_t win_end = have_bounds ? *off : 0;

            std::vector<int> applied; bool level_ok = true;
            auto& table = (pc.tgt_ch == 1) ? L1 : (pc.tgt_ch == 2) ? L2 : L3;
            auto& dtable = (pc.tgt_ch == 1) ? D1 : (pc.tgt_ch == 2) ? D2 : D3;
            if (win_ok) {
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
            std::vector<int> order6 = pc.is_vol ? std::vector<int>{8,48,20,60} : std::vector<int>{300,700,500,900};
            bool cyclic_ok = win_ok && applied.size() >= 9 && level_ok && cyclic_consecutive(applied, order6);
            checks.expect(c.loaded && win_ok && cyclic_ok,
                pc.is_vol
                ? ("P6 " + tag + " (a) [p.149,p.150,p.151,p.265]: the target's applied volumes are "
                   "cyclically consecutive in M6V, every level pair a reference pair of the target")
                : ("P6 " + tag + " (a) [p.149,p.150,p.151,p.265]: the target's applied periods are "
                   "cyclically consecutive in M_P4, its level pair stays at the reference for VOL 40"));

            bool other_const = have_bounds && is_constant(c.audio, *on, *off, oside);
            checks.expect(c.loaded && have_bounds && other_const,
                "P6 " + tag + " (c) [p.150]: the other side's output is constant for the whole window");

            bool spacing_ok = true;
            if (have_bounds) {
                auto check_trip = [&](int ch, double per) {
                    auto fc = fetch_cycles(c.log, ch_base(ch), 2, *on, *off);
                    if (fc.size() < 2) return false;
                    for (size_t i = 1; i < fc.size(); ++i)
                        if (std::fabs((double)(fc[i]-fc[i-1]) - 2*ticks(per)) > lambda) return false;
                    return true;
                };
                double pers[4] = {1400,1600,1800,2000};
                if (!check_trip(pc.trip_a, pers[pc.trip_a])) spacing_ok = false;
                if (!check_trip(pc.trip_b, pers[pc.trip_b])) spacing_ok = false;
            }
            checks.expect(c.loaded && have_bounds && spacing_ok,
                "P6 " + tag + " (d) [p.164]: every non-target channel's fetch spacing equals "
                "2*AUDxPER within lambda");

            if (mv == tag) {
                checks.expect(!cyclic_ok, "P6 " + tag + " MUTATION stimulus red the attach write "
                    "omitted: the target keeps its own VOL 40/PER 200, (a) fails");
            }
            if (!pc.extra.empty() && mv == tag + "-EXTRA") {
                Capture ce = run_capture(dir + pc.extra, budget_for(110000));
                auto mke = markers(ce); auto one = m_on(mke), offe = m_off(mke);
                bool tripwire_broken = one && offe && !is_constant(ce.audio, *one, *offe, oside);
                checks.expect(ce.loaded && tripwire_broken,
                    "P6 " + tag + "-EXTRA MUTATION stimulus red " + pc.extra_desc +
                    ": the tripwire check (c)/(d) fails");
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
            bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect;
            checks.expect(c.loaded && c.done.has_value() && pc1,
                "PC1 [p.265] " + pc.tag + ": stored ADKCONR bits 7-0 equal the session's mask");
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            auto ch3f = have_bounds ? fetch_cycles(c.log, ch_base(3), 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && ch3f.size() >= 3 ? ch3f[2] : 0;
            bool win_ok = have_bounds && ch3f.size() >= 3;
            bool both_const = win_ok && is_constant(c.audio, win_start, *off, 0) && is_constant(c.audio, win_start, *off, 1);
            checks.expect(c.loaded && win_ok && both_const,
                "P7 [p.150,p.151,p.265] 5-mod.c23,5-p150.c26,5-appA-p265.c22,5-appA-p265.c26,"
                "A-adkcon.b03,A-adkcon.b07: both outputs are constant for the whole window -- "
                "channel 3's attach bit modulates nothing");
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
                checks.expect(!both_const, "P7 " + pc.tag + " MUTATION stimulus red the wrong bit "
                    "set: channel 3 becomes audible / its spacing leaves 4000");
            }
        }
    }

    // ================= P8: an attached channel's own DAC is bypassed ======
    {
        struct P8Case { int ch; std::string bin, rbin; double budget; };
        std::vector<P8Case> cases = {
            {0, "/prog_s_av.bin", "/prog_rmod.bin", 200000},
            {0, "/prog_s_ap.bin", "/prog_rdacp4.bin", 110000},
            {0, "/prog_s_both.bin", "/prog_rdacvp.bin", 110000},
            {3, "/prog_s_dac3v.bin", "/prog_rdac3v.bin", 30000},
            {3, "/prog_s_dac3p.bin", "/prog_rdac3p.bin", 30000},
        };
        bool first = true;
        for (auto& pc : cases) {
            std::string bin = (mv == "P8" && first) ? pc.rbin : pc.bin;
            Capture c = (mv == "P8" && first) ? rmod : run_capture(dir + bin, budget_for((uint64_t)pc.budget));
            int side = page_side(pc.ch);
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            auto fc = have_bounds ? fetch_cycles(c.log, ch_base(pc.ch), pc.ch == 0 ? 17 : 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && fc.size() >= 3 ? fc[2] + (uint64_t)L_a : 0;
            uint64_t win_end = have_bounds ? *off + (uint64_t)L_a : 0;
            bool win_ok = have_bounds && fc.size() >= 3;
            bool constant = win_ok && is_constant(c.audio, win_start, win_end, side);
            checks.expect(c.loaded && win_ok && constant,
                "P8 [p.165,p.151,p.265] 5-p165.c19,5-sig.audxav,5-p165.c20,5-sig.audxap,"
                "A-adkcon.b03,A-adkcon.b07: channel " + std::to_string(pc.ch) + "'s own side has "
                "one value for the whole window while it is attached (" + bin + ")");
            first = false;
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
            int side = page_side(1);
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            auto pr40 = L1.count(40) ? L1[40] : std::make_pair<uint16_t,uint16_t>(0,0);
            uint64_t win_start = 0; bool win_ok = false;
            if (have_bounds) {
                auto runs = runs_in_window(c.audio, *on, *off, side);
                for (auto& r : runs) if (r.value != pr40.first && r.value != pr40.second) { win_start = r.start; win_ok = true; break; }
            }
            uint64_t win_end = have_bounds ? *off + (uint64_t)L_a : 0;
            auto off0 = win_ok ? fetch_offsets(c.log, ch_base(0), pc.lm, win_start, win_end) : std::vector<uint32_t>{};
            auto off1 = win_ok ? fetch_offsets(c.log, ch_base(1), pc.lc, win_start, win_end) : std::vector<uint32_t>{};
            bool off0_ok = off0.size() >= (size_t)(2*pc.lm);
            for (size_t i = 0; i < off0.size(); ++i) if (off0[i] != (uint32_t)(i % pc.lm)) off0_ok = false;
            bool off1_ok = off1.size() >= (size_t)(2*pc.lc);
            for (size_t i = 0; i < off1.size(); ++i) if (off1[i] != (uint32_t)(i % pc.lc)) off1_ok = false;

            std::vector<int> applied; bool level_ok = true;
            if (win_ok) {
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
            checks.expect(c.loaded && win_ok && off0_ok,
                "P9 (a) [p.147,p.150]: channel 0's offsets step 0.." + std::to_string(pc.lm-1) +
                " and back to 0 (" + bin + ")");
            checks.expect(c.loaded && win_ok && off1_ok,
                "P9 (b) [p.147,p.150]: channel 1's offsets step 0.." + std::to_string(pc.lc-1) +
                " and back to 0 (" + bin + ")");
            checks.expect(c.loaded && win_ok && cyclic_ok,
                "P9 (c) [p.150] 5-mod.c18,5-p150.c21: channel 1's applied volumes are cyclically "
                "consecutive across the modulator's own restarts, none skipped or repeated (" + bin + ")");
            if (is_a && mv == "P9") {
                checks.expect(!off0_ok, "P9 MUTATION stimulus red AUD0LEN=4 instead of 5: the "
                    "modulator's own cycle shortens to 4 words, (a) and (c) with L_m=5 fail");
            }
        }
    }

    // ================= P10: DMA-request phase ==============================
    {
        auto phase_check = [&](Capture& c, int k_mode) {
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            if (!on || !off) return false;
            auto fetches = fetch_cycles(c.log, ch_base(0), 17, *on, *off);
            if (fetches.size() < 5) return false;
            uint64_t t0 = fetches[1];
            for (size_t n = 4; n <= fetches.size(); ++n) {
                double kfac = k_mode == 0 ? 2.0*(double)(n-3) : k_mode == 1 ? 2.0*(double)(n-3)+1.0 : (double)(n-3);
                double o = (double)fetches[n-1] - (double)t0 - kfac*d_mod;
                if (o < ticks(-100) || o > ticks(600)) return false;
            }
            return true;
        };
        std::string av_bin = (mv == "P10") ? "/prog_s_ap.bin" : "/prog_s_av.bin"; // P10 cross-grades S-AP's phase onto S-AV's label
        Capture av = run_capture(dir + "/prog_s_av.bin", budget_for(200000));
        Capture ap = run_capture(dir + "/prog_s_ap.bin", budget_for(110000));
        Capture both = run_capture(dir + "/prog_s_both.bin", budget_for(110000));
        bool rmod_anchor_ok = pc7_check(rmod_fetches, d_mod); // PC7 already asserted above
        checks.expect(rmod.loaded && rmod_anchor_ok,
            "P10 anchor [PC7,p.164]: normal operation (R-MOD) shows k(n)=2(n-3)");
        bool av_ok = (mv == "P10") ? phase_check(ap, 1) : phase_check(av, 0);
        checks.expect(av.loaded && av_ok,
            "P10 [p.164,p.165] 5-p164.c33,5-sm.c33,5-p165.c22,5-sig.napnav,F58.loop.attach: S-AV "
            "-- o_n in [-100,+600] ticks with k(n)=2(n-3), as normal (011->010)");
        checks.expect(ap.loaded && phase_check(ap, 1),
            "P10 [p.164] 5-p164.c34,5-sm.c34,F58.loop.attach: S-AP -- k(n)=2(n-3)+1 (010->011)");
        checks.expect(both.loaded && phase_check(both, 2),
            "P10 [p.164] 5-p164.c35,5-sm.c35: S-BOTH -- k(n)=n-3 (both transitions)");
        if (mv == "P10") {
            checks.expect(!av_ok, "P10 MUTATION stimulus red S-AV graded with S-AP's own "
                "attach-period phase (k=2(n-3)+1): the grid check fails by D_mod on every fetch");
        }
        (void)av_bin;
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
