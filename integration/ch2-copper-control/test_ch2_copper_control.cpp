// test_ch2_copper_control.cpp -- ch2-copper-control. See SPECIFICATION.md
// for every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// NMH_MUTATE selects a mutated program, or a swapped capture, in place of
// its normal counterpart, one precondition or assertion at a time; none is
// exercised by a plain `make test`. The normal run prints only the
// fixture, the preconditions and the assertions.
//
// The P4 comparator contrast (prog_p4_ctrl_cop1b.bin) is not gated by
// NMH_MUTATE: like prog_p1_off.bin/prog_p2_off.bin, it is one of this
// package's own contrasting cases and runs in every pass.
//
// Every mutation below measured a FAIL on exactly the one line it names,
// on both cores, with every other line unchanged from the normal run (the
// two already-red P4 lines aside).
//
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXTURE: stored constant changed
//   to $DEAD instead of $BEEF -- measured FAIL on "FIXTURE: ...", both cores
// MUTATION P1 stimulus red NMH_MUTATE=P1-SWAP: prog_p1_off.bin's capture
//   (COPEN clear) graded in place of prog_p1.bin's -- measured FAIL on
//   "P1 ch2-stopping-the-copper.c10,...", both cores
// MUTATION P1-OFF stimulus red NMH_MUTATE=P1OFF-SWAP: prog_p1.bin's capture
//   (COPEN set) graded in place of prog_p1_off.bin's -- measured FAIL on
//   "P1-OFF ch2-stopping-the-copper.c10,...", both cores
// MUTATION P1a stimulus red NMH_MUTATE=P1-A: DMACON=$8280 written before
//   COP1LC/COPJMP1 are written (before the manual's own step 3, p.30) --
//   measured FAIL on "P1a precondition: ...", both cores
// MUTATION P1b stimulus red NMH_MUTATE=P1-B: DMACON=$8080 (COPEN without
//   DMAEN) written instead of $8280 at step 5 -- measured FAIL on "P1b
//   precondition: ...", both cores
// MUTATION P1c stimulus red NMH_MUTATE=P1-C: a fixed delay inserted between
//   samples, sparse enough to fail the density anchor B2/B3 -- measured FAIL
//   on "P1c precondition: ...", both cores
// MUTATION P2 stimulus red NMH_MUTATE=P2-SWAP: prog_p2_off.bin's capture
//   graded in place of prog_p2.bin's -- measured FAIL on "P2
//   ch2-stopping-the-copper.c7.s2,...", both cores
// MUTATION P2-OFF stimulus red NMH_MUTATE=P2OFF-SWAP: prog_p2.bin's capture
//   graded in place of prog_p2_off.bin's -- measured FAIL on "P2-OFF
//   ch2-stopping-the-copper.c7.s2,...", both cores
// MUTATION P2a stimulus red NMH_MUTATE=P2-A: L2's MOVE replaced by
//   $0180,$0F00 (COLOR00), which never touches INTREQ -- measured FAIL on
//   "P2a precondition: ...", both cores
// MUTATION P2b stimulus red NMH_MUTATE=P2-B: INTENA=$8010 (bit 4 set, the
//   master enable bit 14 left clear) -- measured FAIL on "P2b
//   precondition: ...", both cores
// MUTATION P2c stimulus red NMH_MUTATE=P2-C: a fixed delay inserted between
//   phase-2 samples, sparse enough to fail the density anchor B2/B3 --
//   measured FAIL on "P2c precondition: ...", both cores
// MUTATION P4a stimulus red NMH_MUTATE=P4-A: DMACON=$8080 written before the
//   DMACONR read -- measured FAIL on "P4a precondition: ...", both cores
// MUTATION P4b stimulus red NMH_MUTATE=P4-B: the whole Run 1 sequence,
//   DMACON write included, run once early -- measured FAIL on "P4b
//   precondition: ...", both cores
// MUTATION P4c stimulus red NMH_MUTATE=P4-C: a frame wrap forced between M1
//   and the DMACON write -- measured FAIL on "P4c precondition: ...", both
//   cores

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// nanomig.i, restated.
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;

// copctl.i, restated: this package's own low-memory (post-CLEAR_OVL) and
// scratch layout.
constexpr uint32_t kList1   = 0x001000;
constexpr uint32_t kList2   = 0x001200;
constexpr uint32_t kList3   = 0x001400;
constexpr uint32_t kMarkBuf = 0x0C1000;
constexpr uint32_t kRecBuf  = 0x0C4000;

constexpr uint64_t kSamplerRunBudget = 24'000'000; // a handful of PAL frames

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label.c_str());
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::printf("MEASURED: %s\n", msg.c_str()); }
    void record(const std::string& msg) { std::printf("RECORDED: %s\n", msg.c_str()); }
    int passed = 0, failed = 0;
};

// ---- capture: one simulation run, plus the RAM-port log and any words the
// program stored that a proposition wants to read back. ----------------
struct Capture {
    size_t   loaded = 0;
    bool     released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
};

Capture run_capture(const std::string& bin, uint64_t run_budget, const std::vector<uint32_t>& peek_addrs) {
    Capture c;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin);
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    for (uint32_t a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    return c;
}

// ---- SPECIFICATION.md 2.3: channel-fetch classification ---------------
//
// A read at a non-zero word address that is not a 68000 access. Word
// address 0 is memory refresh (measured: every refresh access reads word
// address 0, never any other address), and none of this package's
// list/data regions include word address 0. The 68000 never reads inside
// a list or data region (only writes it once, to construct it), so any
// read there, other than at word address 0, is unambiguously a channel
// fetch.
bool is_channel_read(const nmh::RamAccess& a, uint32_t byte_lo, uint32_t byte_hi) {
    if (a.is_write || a.word_addr == 0) return false;
    uint32_t b = a.word_addr * 2;
    return b >= byte_lo && b < byte_hi;
}

bool any_channel_read_in(const std::vector<nmh::RamAccess>& log, uint32_t byte_lo, uint32_t byte_hi,
                          uint64_t t_lo, uint64_t t_hi_excl) {
    for (const auto& a : log)
        if (is_channel_read(a, byte_lo, byte_hi) && a.cycle >= t_lo && a.cycle < t_hi_excl) return true;
    return false;
}

std::optional<uint64_t> first_write_cycle(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = byte_addr / 2;
    for (const auto& a : log) if (a.is_write && a.word_addr == w) return a.cycle;
    return std::nullopt;
}

// ---- SPECIFICATION.md 2.4: reference frame B (beam records) -----------
//
// Each record is 4 consecutive word WRITES by the 68000 program at
// RECBUF+8*i, +2, +4, +6 (INTREQR, then VPOSR/VHPOSR as one long word, then
// VPOSR read again, per copctl.i's SAMPLE macro): found by their own store
// times at the port, exactly as 2.4 describes ("each record's stores are
// visible at the port with their times"), not guessed from buffer content.
// VPOSR/VHPOSR is two bus reads (p.216), so a record is kept only if the
// two VPOSR reads agree on V8 (bit 0): otherwise the pair straddled a wrap
// and the record is torn.
struct Rec { uint64_t t0, t1; uint16_t intreqr; int V, H; };

std::vector<Rec> decode_records(const std::vector<nmh::RamAccess>& log, uint32_t base_byte_addr, uint32_t count) {
    std::unordered_map<uint32_t, std::pair<uint64_t, uint16_t>> w;
    uint32_t base_word = base_byte_addr / 2;
    uint32_t hi_word = base_word + 4u * count;
    for (const auto& a : log) {
        if (!a.is_write || a.word_addr < base_word || a.word_addr >= hi_word) continue;
        w.emplace(a.word_addr, std::make_pair(a.cycle, a.data));
    }
    std::vector<Rec> out;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t a0 = base_word + 4 * i, a1 = a0 + 1, a2 = a0 + 2, a3 = a0 + 3;
        auto it0 = w.find(a0), it1 = w.find(a1), it2 = w.find(a2), it3 = w.find(a3);
        if (it0 == w.end() || it1 == w.end() || it2 == w.end() || it3 == w.end()) break;
        if ((it1->second.second & 1) != (it3->second.second & 1)) continue; // torn: V8 disagrees
        Rec r;
        r.t0 = it0->second.first;
        r.t1 = it3->second.first;
        r.intreqr = it0->second.second;
        uint32_t raw = (uint32_t(it1->second.second) << 16) | it2->second.second;
        r.V = (raw >> 8) & 0x1FF;
        r.H = raw & 0xFF;
        out.push_back(r);
    }
    return out;
}

struct Span { size_t begin, end; };

// Complete frames only: [wraps[k], wraps[k+1]) for consecutive wrap points.
// A wrap is a decrease from V >= 256 to V <= 1 (p.24: the vertical position
// "appears to wrap to 0" past line 255; p.219: line 0 is the start of
// vertical blank) -- not any V decrease, which a torn record could also
// produce were the tear filter above ever incomplete.
std::vector<Span> record_frames(const std::vector<Rec>& r) {
    std::vector<size_t> wraps;
    for (size_t i = 1; i < r.size(); ++i)
        if (r[i - 1].V >= 256 && r[i].V <= 1) wraps.push_back(i);
    std::vector<Span> out;
    for (size_t k = 0; k + 1 < wraps.size(); ++k) out.push_back({wraps[k], wraps[k + 1]});
    return out;
}

std::optional<size_t> frame_index_of_record(const std::vector<Span>& frames, size_t idx) {
    for (size_t k = 0; k < frames.size(); ++k)
        if (idx >= frames[k].begin && idx < frames[k].end) return k;
    return std::nullopt;
}

// "Line of a port event at time t" (2.4): a = last record stored before t,
// b = the record following the first record stored after t (so that b's
// own read, not just its store, is guaranteed after t). Frame-of-event is
// the same construction, reported as a frame index instead of a V.
std::optional<size_t> last_record_before(const std::vector<Rec>& r, uint64_t t) {
    std::optional<size_t> a;
    for (size_t i = 0; i < r.size(); ++i) {
        if (r[i].t1 < t) a = i; else break;
    }
    return a;
}
std::optional<size_t> first_record_after(const std::vector<Rec>& r, uint64_t t) {
    for (size_t i = 0; i < r.size(); ++i) if (r[i].t0 > t) return i;
    return std::nullopt;
}

std::optional<size_t> frame_of_event(const std::vector<Rec>& r, const std::vector<Span>& frames, uint64_t t) {
    auto a = last_record_before(r, t);
    auto c = first_record_after(r, t);
    if (!a || !c || *c + 1 >= r.size()) return std::nullopt;
    size_t b = *c + 1;
    auto fa = frame_index_of_record(frames, *a);
    auto fb = frame_index_of_record(frames, b);
    if (fa && fb && *fa == *fb) return *fa;
    return std::nullopt;
}

// First complete frame whose own first record was stored strictly after t
// (e.g. after t(M2)): the frame-boundary analogue of "the window starts at
// t(M2)". Returns -1 if there is no such frame.
int first_frame_after(const std::vector<Rec>& r, const std::vector<Span>& frames, uint64_t t) {
    for (size_t k = 0; k < frames.size(); ++k)
        if (r[frames[k].begin].t0 > t) return (int)k;
    return -1;
}

// The time boundary between frame k-1 and frame k: the last record fully
// stored before the wrap record itself. A Copper restart's own fetch is
// causally tied to the wrap edge, so it is consistently measured a few
// hundred cycles before the wrap record's own store time; assigning that
// whole gap to the frame it starts (not the one it ends) is the right edge
// for a whole-frame membership question. Frame 0's own low boundary is 0.
uint64_t frame_lo_bound(const std::vector<Rec>& r, const std::vector<Span>& frames, size_t k) {
    if (k == 0) return 0;
    return r[frames[k].begin - 1].t1;
}

// A complete frame's own time range, built from frame_lo_bound on both
// ends so that adjacent frames' ranges share exactly one boundary point
// (disjoint, half-open) instead of each separately reaching into the gap
// around the wrap that lies between them.
std::pair<uint64_t, uint64_t> frame_time_range(const std::vector<Rec>& r, const std::vector<Span>& frames,
                                                size_t k) {
    uint64_t lo = frame_lo_bound(r, frames, k);
    uint64_t hi = (k + 1 < frames.size()) ? frame_lo_bound(r, frames, k + 1)
                  : ((frames[k].end < r.size()) ? r[frames[k].end].t0 : UINT64_MAX);
    return {lo, hi};
}

bool frame_has_read_at(const std::vector<Rec>& recs, const std::vector<Span>& frames, int frame_idx,
                        const std::vector<nmh::RamAccess>& log, uint32_t word_addr) {
    auto [lo, hi] = frame_time_range(recs, frames, (size_t)frame_idx);
    for (const auto& a : log) {
        if (a.is_write || a.word_addr != word_addr) continue;
        if (a.cycle >= lo && a.cycle < hi) return true;
    }
    return false;
}

// P4: the first channel-fetch read (strictly) after time t, among several
// named 8-byte list regions, tagged with which region it landed in.
struct TaggedFetch { uint64_t cycle; char name; };
std::optional<TaggedFetch> first_fetch_among(const std::vector<nmh::RamAccess>& log, uint64_t t,
                                              const std::vector<std::pair<uint32_t, char>>& regions) {
    std::optional<TaggedFetch> best;
    for (const auto& a : log) {
        if (a.is_write || a.word_addr == 0 || a.cycle <= t) continue;
        uint32_t b = a.word_addr * 2;
        for (const auto& rb : regions) {
            if (b >= rb.first && b < rb.first + 8 && (!best || a.cycle < best->cycle)) {
                best = TaggedFetch{a.cycle, rb.second};
            }
        }
    }
    return best;
}

// P4 precondition (c): a wrap (V >= 256 to V <= 1, as in record_frames)
// whose own store time lies strictly inside (lo, hi).
bool wrap_between(const std::vector<Rec>& r, uint64_t lo, uint64_t hi) {
    for (size_t i = 1; i < r.size(); ++i)
        if (r[i - 1].V >= 256 && r[i].V <= 1 && r[i].t0 > lo && r[i].t0 < hi) return true;
    return false;
}

// P4 precondition (c): a record showing vertical blanking (V <= 25, p.219)
// whose own store time lies strictly inside (lo, hi).
bool low_v_between(const std::vector<Rec>& r, uint64_t lo, uint64_t hi) {
    for (const auto& rec : r)
        if (rec.t0 > lo && rec.t0 < hi && rec.V <= 25) return true;
    return false;
}

// P4: one run's capture plus its decoded records and its first fetch (of
// A/B/C) after M1, reused both for the run's own assertion and for the
// preconditions, which all apply to this same style of run.
struct P4Result {
    Capture cap;
    bool ok = false;
    std::optional<uint64_t> t_ms;  // marker MS: precedes the first location-register write
    std::optional<uint64_t> t_m1;  // marker M1: precedes the DMACON write
    std::vector<Rec> recs;
    std::optional<TaggedFetch> fetch;   // first Copper fetch (A/B/C) after t(M1)
    bool precond_c = false;             // no wrap, no V<=25 record, between t(MS) and the fetch
};

P4Result run_p4(const std::string& bin, uint64_t budget) {
    P4Result r;
    r.cap = run_capture(bin, budget, {kResults + 4, kResults + 8, kResults + 10});
    if (!r.cap.loaded || !r.cap.done.has_value()) return r;
    r.t_ms = first_write_cycle(r.cap.log, kMarkBuf + 4);
    r.t_m1 = first_write_cycle(r.cap.log, kMarkBuf);
    if (!r.t_m1) return r;
    uint32_t count = (r.cap.peeks.size() >= 3 && r.cap.peeks[1] && r.cap.peeks[2])
                         ? (uint32_t(*r.cap.peeks[1]) << 16) | *r.cap.peeks[2] : 0;
    r.recs = decode_records(r.cap.log, kRecBuf, count);
    r.fetch = first_fetch_among(r.cap.log, *r.t_m1, {{kList1, 'A'}, {kList2, 'B'}, {kList3, 'C'}});
    r.ok = r.fetch.has_value();
    if (r.ok && r.t_ms)
        r.precond_c = !wrap_between(r.recs, *r.t_ms, r.fetch->cycle) &&
                      !low_v_between(r.recs, *r.t_ms, r.fetch->cycle);
    return r;
}

// Anchor B1 (2.4): within a complete frame, V never decreases between
// consecutive records, checked over the graded line range [lo,hi] --
// vertical blank/sync itself is not a line any proposition in this package
// grades.
bool anchor_B1(const std::vector<Rec>& r, Span f, int lo, int hi) {
    for (size_t i = f.begin + 1; i < f.end; ++i) {
        if (r[i - 1].V < lo || r[i - 1].V > hi || r[i].V < lo || r[i].V > hi) continue;
        if (r[i].V < r[i - 1].V) return false;
    }
    return true;
}

// Anchors B2 (every line in [lo,hi] carries >= 7 records) and B3 (no line
// in [lo,hi] is missing): a density requirement of the test, not a page
// value (2.4). 7 is this package's own measured floor for the tear-checked
// SAMPLE/FAST_SAMPLE record (2.4): on both cores, an unrolled dense sampler
// reaches exactly 8 records on every graded line but one, and 7 on that
// one, in every captured frame.
std::pair<bool, bool> anchor_B2_B3(const std::vector<Rec>& r, Span f, int lo, int hi) {
    std::vector<int> cnt(hi - lo + 1, 0);
    for (size_t i = f.begin; i < f.end; ++i)
        if (r[i].V >= lo && r[i].V <= hi) ++cnt[r[i].V - lo];
    bool b2 = true, b3 = true;
    for (int c : cnt) { if (c == 0) b3 = false; if (c < 7) b2 = false; }
    return {b2, b3};
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ==== Fixture: a program-stored constant read back correctly, before
    // any proposition's own peek (DMACONR, INTENAR, record counts) is
    // trusted [not graded against the page -- apparatus check] ==========
    {
        bool mut = (mv == "FIXTURE");
        Capture c = run_capture(dir + (mut ? "/prog_fixture_mut.bin" : "/prog_fixture.bin"),
                                 kBootBudget, {kResults});
        bool ok = c.loaded && c.done.has_value() && !c.peeks.empty() && c.peeks[0] &&
                  *c.peeks[0] == 0xBEEF;
        checks.expect(ok, "FIXTURE: a program-stored constant is read back correctly through "
                           "peek_logical [not graded against the page -- apparatus check, no "
                           "claim uid]");
    }

    // ==== P1: DMACON bit 7 (COPEN) enables Copper DMA [p.30] ==============
    // ch2-stopping-the-copper.c10, COP2-stopping-the-copper.c13
    {
        Capture base = run_capture(dir + "/prog_p1.bin", kSamplerRunBudget,
                                    {kResults + 4, kResults + 8, kResults + 10});
        bool base_ok = base.loaded && base.done.has_value();
        checks.expect(base_ok, "P1 fixture: prog_p1.bin reaches DONE");

        Capture off = run_capture(dir + "/prog_p1_off.bin", kSamplerRunBudget, {kResults + 4});
        bool off_ok = off.loaded && off.done.has_value();
        checks.expect(off_ok, "P1-OFF fixture: prog_p1_off.bin reaches DONE");

        if (base_ok) {
            auto t_m1 = first_write_cycle(base.log, kMarkBuf);
            auto t_m2 = first_write_cycle(base.log, kMarkBuf + 2);
            bool have_markers = t_m1.has_value() && t_m2.has_value();
            checks.expect(have_markers, "P1 fixture: markers M1 and M2 were both written");

            uint32_t count = 0;
            if (base.peeks.size() >= 3 && base.peeks[1] && base.peeks[2])
                count = (uint32_t(*base.peeks[1]) << 16) | *base.peeks[2];
            auto recs = decode_records(base.log, kRecBuf, count);
            auto frames = record_frames(recs);

            if (have_markers) {
                // ---- Precondition (a): no Copper fetch in W0 = [start, t(M1))
                bool mut_a = (mv == "P1-A");
                Capture cap_a;
                if (mut_a) cap_a = run_capture(dir + "/prog_p1_mut_a.bin", kSamplerRunBudget, {});
                const std::vector<nmh::RamAccess>& log_a = mut_a ? cap_a.log : base.log;
                uint64_t t_m1_a = mut_a ? first_write_cycle(log_a, kMarkBuf).value_or(0) : *t_m1;
                bool pre_a = !any_channel_read_in(log_a, kList1, kList1 + 8, 0, t_m1_a);
                checks.expect(pre_a, "P1a precondition: no Copper fetch in L1's region before t(M1)");

                // ---- Precondition (b): stored DMACONR bit 9 = 1, bit 7 = 1
                bool mut_b = (mv == "P1-B");
                Capture cap_b;
                if (mut_b) cap_b = run_capture(dir + "/prog_p1_mut_b.bin", kSamplerRunBudget, {kResults + 4});
                auto dmaconr_b = mut_b ? (cap_b.peeks.empty() ? std::nullopt : cap_b.peeks[0]) : base.peeks[0];
                bool pre_b = dmaconr_b.has_value() && ((*dmaconr_b) & 0x0280) == 0x0280;
                checks.expect(pre_b, "P1b precondition: stored DMACONR bit 9 = 1 and bit 7 = 1 [p.222]");

                // ---- Precondition (c): anchors B1-B3 over the two complete
                // frames after M2, on lines $10-$FF.
                bool mut_c = (mv == "P1-C");
                Capture cap_c;
                if (mut_c) cap_c = run_capture(dir + "/prog_p1_mut_c.bin", kSamplerRunBudget,
                                                {kResults + 8, kResults + 10});
                const std::vector<nmh::RamAccess>& log_c = mut_c ? cap_c.log : base.log;
                uint32_t count_c = count;
                if (mut_c) {
                    count_c = (cap_c.peeks.size() >= 2 && cap_c.peeks[0] && cap_c.peeks[1])
                                  ? (uint32_t(*cap_c.peeks[0]) << 16) | *cap_c.peeks[1] : 0;
                }
                std::vector<Rec> recs_c = mut_c ? decode_records(log_c, kRecBuf, count_c) : recs;
                std::vector<Span> frames_c = mut_c ? record_frames(recs_c) : frames;
                uint64_t t_m2_c = mut_c ? first_write_cycle(log_c, kMarkBuf + 2).value_or(0) : *t_m2;
                int f0c = first_frame_after(recs_c, frames_c, t_m2_c);
                bool pre_c = f0c >= 0 && (size_t)(f0c + 1) < frames_c.size();
                if (pre_c) {
                    bool b1a = anchor_B1(recs_c, frames_c[f0c], 0x10, 0xFF);
                    bool b1b = anchor_B1(recs_c, frames_c[f0c + 1], 0x10, 0xFF);
                    auto d1 = anchor_B2_B3(recs_c, frames_c[f0c], 0x10, 0xFF);
                    auto d2 = anchor_B2_B3(recs_c, frames_c[f0c + 1], 0x10, 0xFF);
                    pre_c = b1a && b1b && d1.first && d1.second && d2.first && d2.second;
                }
                checks.expect(pre_c,
                    "P1c precondition: anchor checks B1-B3 hold (lines $10-$FF) over the two "
                    "complete frames after M2");

                // ---- Main assertion, graded both ways per the contrasting
                // case. Its own MUTATION: swap in prog_p1_off.bin's capture
                // (bit 7 clear) -- the assertion must find no fetch and go
                // red.
                bool mut_swap = (mv == "P1-SWAP");
                Capture cap_swap;
                if (mut_swap) cap_swap = run_capture(dir + "/prog_p1_off.bin", kSamplerRunBudget,
                                                      {kResults + 4, kResults + 8, kResults + 10});
                const Capture& main_cap = mut_swap ? cap_swap : base;
                uint32_t count_m = count;
                if (mut_swap)
                    count_m = (main_cap.peeks.size() >= 3 && main_cap.peeks[1] && main_cap.peeks[2])
                                  ? (uint32_t(*main_cap.peeks[1]) << 16) | *main_cap.peeks[2] : 0;
                std::vector<Rec> recs_m = mut_swap ? decode_records(main_cap.log, kRecBuf, count_m) : recs;
                std::vector<Span> frames_m = mut_swap ? record_frames(recs_m) : frames;
                uint64_t t_m2_m = mut_swap ? first_write_cycle(main_cap.log, kMarkBuf + 2).value_or(0) : *t_m2;

                int f0 = first_frame_after(recs_m, frames_m, t_m2_m);
                bool have_two_frames = f0 >= 0 && (size_t)(f0 + 1) < frames_m.size();
                checks.expect(have_two_frames, "P1 fixture: two complete frames after M2 were captured");
                if (have_two_frames) {
                    uint32_t ir1_word = kList1 / 2;
                    bool f0_fetch = frame_has_read_at(recs_m, frames_m, f0, main_cap.log, ir1_word);
                    bool f1_fetch = frame_has_read_at(recs_m, frames_m, f0 + 1, main_cap.log, ir1_word);
                    checks.expect(f0_fetch && f1_fetch,
                        "P1 ch2-stopping-the-copper.c10,COP2-stopping-the-copper.c13 [p.30]: with "
                        "DMACON=$8280 (COPEN set, DMAEN set), the Copper fetches L1's IR1 in each of "
                        "the two complete frames after M2");
                }
            }
        }

        // ---- Contrasting case (bit 7 = 0), graded in the normal run: no
        // Copper fetch anywhere after M2, and DMACONR bit 7 reads 0. Its own
        // MUTATION: swap in prog_p1.bin's capture (bit 7 set) -- the
        // assertion must find a fetch and go red.
        bool mut_swap_off = (mv == "P1OFF-SWAP");
        Capture cap_swap_off;
        if (mut_swap_off) cap_swap_off = run_capture(dir + "/prog_p1.bin", kSamplerRunBudget, {kResults + 4});
        const Capture& off_cap = mut_swap_off ? cap_swap_off : off;
        bool off_cap_ok = off_cap.loaded && off_cap.done.has_value();
        if (off_ok || mut_swap_off) {
            auto t_m2_off = first_write_cycle(off_cap.log, kMarkBuf + 2);
            bool no_fetch = t_m2_off.has_value() &&
                             !any_channel_read_in(off_cap.log, kList1, kList1 + 8, *t_m2_off, UINT64_MAX);
            bool dmaconr_bit7_clear = !off_cap.peeks.empty() && off_cap.peeks[0] &&
                                      (*off_cap.peeks[0] & 0x0080) == 0;
            checks.expect(off_cap_ok && t_m2_off.has_value() && no_fetch && dmaconr_bit7_clear,
                "P1-OFF ch2-stopping-the-copper.c10,COP2-stopping-the-copper.c13 [p.30,p.222]: with "
                "DMACON=$8200 (COPEN clear), the Copper makes no fetch in L1's region after M2, and "
                "DMACONR bit 7 reads 0");
        }
    }

    // ==== P2: Clearing COPEN stops instruction fetching and data placement
    // [p.30] ==== ch2-stopping-the-copper.c7.s2, COP2-stopping-the-copper.c10
    {
        constexpr uint32_t list_ir2 = kList1 / 2 + 1; // IR2 of L2's MOVE instruction

        Capture base = run_capture(dir + "/prog_p2.bin", kSamplerRunBudget,
                                    {kResults + 4, kResults + 8, kResults + 10});
        bool base_ok = base.loaded && base.done.has_value();
        checks.expect(base_ok, "P2 fixture: prog_p2.bin reaches DONE");

        Capture off = run_capture(dir + "/prog_p2_off.bin", kSamplerRunBudget,
                                   {kResults + 8, kResults + 10});
        bool off_ok = off.loaded && off.done.has_value();
        checks.expect(off_ok, "P2-OFF fixture: prog_p2_off.bin reaches DONE");

        if (base_ok) {
            auto t_m2 = first_write_cycle(base.log, kMarkBuf + 2);
            checks.expect(t_m2.has_value(), "P2 fixture: marker M2 was written");

            uint32_t count = 0;
            if (base.peeks.size() >= 3 && base.peeks[1] && base.peeks[2])
                count = (uint32_t(*base.peeks[1]) << 16) | *base.peeks[2];
            auto recs = decode_records(base.log, kRecBuf, count);
            auto frames = record_frames(recs);

            if (t_m2) {
                // ---- Precondition (a): a marker MPC sits right after the
                // phase-1 clear; L2's IR2 must be fetched after it, with at
                // least one record stored between the clear and that fetch,
                // bit 4 0 in every record before it and 1 in a record after.
                bool mut_a = (mv == "P2-A");
                Capture cap_a;
                if (mut_a) cap_a = run_capture(dir + "/prog_p2_mut_a.bin", kSamplerRunBudget,
                                                {kResults + 8, kResults + 10});
                const std::vector<nmh::RamAccess>& log_a = mut_a ? cap_a.log : base.log;
                uint32_t count_a = count;
                if (mut_a)
                    count_a = (cap_a.peeks.size() >= 2 && cap_a.peeks[0] && cap_a.peeks[1])
                                  ? (uint32_t(*cap_a.peeks[0]) << 16) | *cap_a.peeks[1] : 0;
                std::vector<Rec> recs_a = mut_a ? decode_records(log_a, kRecBuf, count_a) : recs;
                auto t_mpc = first_write_cycle(log_a, kMarkBuf + 6);
                std::optional<uint64_t> t_fetch;
                if (t_mpc)
                    for (const auto& a : log_a)
                        if (is_channel_read(a, kList1, kList1 + 8) && a.word_addr == list_ir2 &&
                            a.cycle > *t_mpc) { t_fetch = a.cycle; break; }
                bool pre_a = t_mpc.has_value() && t_fetch.has_value();
                if (pre_a) {
                    bool have_between = false, zero_before = true, one_after = false;
                    for (const auto& r : recs_a) {
                        if (r.t0 > *t_mpc && r.t1 < *t_fetch) have_between = true;
                        if (r.t0 > *t_mpc && r.t1 < *t_fetch && (r.intreqr & 0x10)) zero_before = false;
                        if (r.t0 > *t_fetch && (r.intreqr & 0x10)) one_after = true;
                    }
                    pre_a = have_between && zero_before && one_after;
                }
                checks.expect(pre_a,
                    "P2a precondition: after the marker at the phase-1 clear, at least one record "
                    "is stored before the first IR2 fetch of L2's MOVE, INTREQR bit 4 reads 0 in "
                    "every record before that fetch and 1 in a record after it");

                // ---- Precondition (b): INTENAR bits 14-0 read 0, read/stored
                // before phase 2.
                bool mut_b = (mv == "P2-B");
                Capture cap_b;
                if (mut_b) cap_b = run_capture(dir + "/prog_p2_mut_b.bin", kSamplerRunBudget, {kResults + 4});
                auto intenar_b = mut_b ? (cap_b.peeks.empty() ? std::nullopt : cap_b.peeks[0]) : base.peeks[0];
                bool pre_b = intenar_b.has_value() && ((*intenar_b) & 0x7FFF) == 0;
                checks.expect(pre_b, "P2b precondition: stored INTENAR bits 14-0 read 0 before phase 2");

                // ---- Precondition (c): anchors B1-B3 over the two complete
                // frames after M2.
                bool mut_c = (mv == "P2-C");
                Capture cap_c;
                if (mut_c) cap_c = run_capture(dir + "/prog_p2_mut_c.bin", kSamplerRunBudget,
                                                {kResults + 8, kResults + 10});
                const std::vector<nmh::RamAccess>& log_c = mut_c ? cap_c.log : base.log;
                uint32_t count_c = count;
                if (mut_c)
                    count_c = (cap_c.peeks.size() >= 2 && cap_c.peeks[0] && cap_c.peeks[1])
                                  ? (uint32_t(*cap_c.peeks[0]) << 16) | *cap_c.peeks[1] : 0;
                std::vector<Rec> recs_c = mut_c ? decode_records(log_c, kRecBuf, count_c) : recs;
                std::vector<Span> frames_c = mut_c ? record_frames(recs_c) : frames;
                uint64_t t_m2_c = mut_c ? first_write_cycle(log_c, kMarkBuf + 2).value_or(0) : *t_m2;
                int f0c = first_frame_after(recs_c, frames_c, t_m2_c);
                bool pre_c = f0c >= 0 && (size_t)(f0c + 1) < frames_c.size();
                if (pre_c) {
                    bool b1a = anchor_B1(recs_c, frames_c[f0c], 0x10, 0xFF);
                    bool b1b = anchor_B1(recs_c, frames_c[f0c + 1], 0x10, 0xFF);
                    auto d1 = anchor_B2_B3(recs_c, frames_c[f0c], 0x10, 0xFF);
                    auto d2 = anchor_B2_B3(recs_c, frames_c[f0c + 1], 0x10, 0xFF);
                    pre_c = b1a && b1b && d1.first && d1.second && d2.first && d2.second;
                }
                checks.expect(pre_c,
                    "P2c precondition: anchor checks B1-B3 hold (lines $10-$FF) over the two "
                    "complete frames after M2");

                // ---- Main assertion: zero fetches in L2's region from
                // t(M2) to the end of the second complete frame after M2,
                // and INTREQR bit 4 = 0 in every record after M2. Its own
                // MUTATION: swap in prog_p2_off.bin's capture -- fetches
                // continue, so the assertion must go red.
                bool mut_swap = (mv == "P2-SWAP");
                Capture cap_swap;
                if (mut_swap) cap_swap = run_capture(dir + "/prog_p2_off.bin", kSamplerRunBudget,
                                                      {kResults + 8, kResults + 10});
                const Capture& main_cap = mut_swap ? cap_swap : base;
                uint64_t t_m2_m = mut_swap ? first_write_cycle(main_cap.log, kMarkBuf + 2).value_or(0) : *t_m2;
                uint32_t count_m = count;
                if (mut_swap)
                    count_m = (main_cap.peeks.size() >= 2 && main_cap.peeks[0] && main_cap.peeks[1])
                                  ? (uint32_t(*main_cap.peeks[0]) << 16) | *main_cap.peeks[1] : 0;
                std::vector<Rec> recs_m = mut_swap ? decode_records(main_cap.log, kRecBuf, count_m) : recs;
                std::vector<Span> frames_m = mut_swap ? record_frames(recs_m) : frames;

                int f0 = first_frame_after(recs_m, frames_m, t_m2_m);
                bool have_two_frames = f0 >= 0 && (size_t)(f0 + 1) < frames_m.size();
                checks.expect(have_two_frames, "P2 fixture: two complete frames after M2 were captured");
                if (have_two_frames) {
                    auto w1 = frame_time_range(recs_m, frames_m, (size_t)f0 + 1);
                    bool no_fetch = !any_channel_read_in(main_cap.log, kList1, kList1 + 8, t_m2_m, w1.second);
                    bool have_post_m2 = false, bit4_always_zero = true;
                    for (const auto& r : recs_m) {
                        if (r.t0 > t_m2_m) {
                            have_post_m2 = true;
                            if (r.intreqr & 0x10) bit4_always_zero = false;
                        }
                    }
                    checks.expect(no_fetch && have_post_m2 && bit4_always_zero,
                        "P2 ch2-stopping-the-copper.c7.s2,COP2-stopping-the-copper.c10 [p.30]: after "
                        "clearing DMACON bit 7, the Copper makes no fetch in L2's region from t(M2) "
                        "through the end of the second complete frame after M2, and INTREQR bit 4 "
                        "reads 0 in every record stored after M2");
                }
            }
        }

        // ---- Contrasting case (main-off), graded in the normal run: with
        // DMACON = $0000 (no bit selected), fetches continue in each of the
        // two frames after M2, and bit 4 reads 1 again after the next
        // restart. Its own MUTATION: swap in prog_p2.bin's capture -- no
        // fetch continues, so the assertion must go red.
        bool mut_swap_off = (mv == "P2OFF-SWAP");
        Capture cap_swap_off;
        if (mut_swap_off) cap_swap_off = run_capture(dir + "/prog_p2.bin", kSamplerRunBudget,
                                                       {kResults + 8, kResults + 10});
        const Capture& off_cap = mut_swap_off ? cap_swap_off : off;
        bool off_cap_ok = off_cap.loaded && off_cap.done.has_value();
        if (off_ok || mut_swap_off) {
            auto t_m2_off = first_write_cycle(off_cap.log, kMarkBuf + 2);
            uint32_t count_off = 0;
            if (off_cap.peeks.size() >= 2 && off_cap.peeks[0] && off_cap.peeks[1])
                count_off = (uint32_t(*off_cap.peeks[0]) << 16) | *off_cap.peeks[1];
            auto recs_off = decode_records(off_cap.log, kRecBuf, count_off);
            auto frames_off = record_frames(recs_off);
            bool ok = off_cap_ok && t_m2_off.has_value();
            int f0o = -1;
            if (ok) {
                f0o = first_frame_after(recs_off, frames_off, *t_m2_off);
                ok = f0o >= 0 && (size_t)(f0o + 1) < frames_off.size();
            }
            if (ok) {
                uint32_t ir1_word = kList1 / 2;
                bool f0_fetch = frame_has_read_at(recs_off, frames_off, f0o, off_cap.log, ir1_word);
                bool f1_fetch = frame_has_read_at(recs_off, frames_off, f0o + 1, off_cap.log, ir1_word);
                bool bit4_returns = false;
                for (const auto& r : recs_off)
                    if (r.t0 > *t_m2_off && (r.intreqr & 0x10)) bit4_returns = true;
                ok = f0_fetch && f1_fetch && bit4_returns;
            }
            checks.expect(ok,
                "P2-OFF ch2-stopping-the-copper.c7.s2,COP2-stopping-the-copper.c10 [p.30,p.218]: with "
                "DMACON=$0000 (no bit selected, a no-op per p.218), the Copper keeps fetching L2's "
                "IR1 in each of the two frames after M2, and INTREQR bit 4 reads 1 again in a later "
                "record");
        }
    }

    // ==== P4: Loading a location register and writing its strobe before
    // enabling gives a known start address [p.30] ====
    // ch2-starting-the-copper-after-reset.c1.s2, .c2.s2
    {
        auto run1 = run_p4(dir + "/prog_p4_run1.bin", kSamplerRunBudget);
        auto run2 = run_p4(dir + "/prog_p4_run2.bin", kSamplerRunBudget);
        auto run3 = run_p4(dir + "/prog_p4_run3.bin", kSamplerRunBudget);
        auto run3r = run_p4(dir + "/prog_p4_run3_reorder.bin", kSamplerRunBudget);
        auto ctrl = run_p4(dir + "/prog_p4_ctrl_cop1b.bin", kSamplerRunBudget);

        checks.expect(run1.ok, "P4 fixture: Run 1 (prog_p4_run1.bin) produced a decided first fetch after M1");
        checks.expect(run2.ok, "P4 fixture: Run 2 (prog_p4_run2.bin) produced a decided first fetch after M1");
        checks.expect(run3.ok, "P4 fixture: Run 3 (prog_p4_run3.bin) produced a decided first fetch after M1");
        checks.expect(run3r.ok,
            "P4 fixture: Run 3 reordered (prog_p4_run3_reorder.bin) produced a decided first fetch "
            "after M1");
        checks.expect(ctrl.ok,
            "P4 fixture: the comparator contrast (prog_p4_ctrl_cop1b.bin) produced a decided first "
            "fetch after M1");

        // ---- Main assertion: one case per register, p.30's "one of ...
        // COP1LC or COP2LC" (P4 item 4). Run 1 strobes COP1LC/COPJMP1 and
        // predicts A; Run 2 strobes COP2LC/COPJMP2 and predicts B.
        bool run1_pass = run1.ok && run1.fetch->name == 'A';
        bool run2_pass = run2.ok && run2.fetch->name == 'B';
        checks.expect(run1_pass && run2_pass,
            "P4 ch2-starting-the-copper-after-reset.c1.s2,ch2-starting-the-copper-after-reset.c2.s2 "
            "[p.25,p.26,p.30,p.276]: the first Copper fetch after Copper DMA is turned on is at the "
            "address of the location register whose strobe was written -- Run 1 (COP1LC/COPJMP1, "
            "expect A) and Run 2 (COP2LC/COPJMP2, expect B)");

        // ---- The same rule, extended to Run 3 and Run 3-reorder. Neither
        // carries a claim id of its own (SPECIFICATION.md section 1 lists
        // only c1.s2 and c2.s2 for P4).
        checks.expect(run2.ok && run2.fetch->name == 'B' &&
                      run3.ok && run3.fetch->name == 'B' &&
                      run3r.ok && run3r.fetch->name == 'C',
            "P4-Run2,Run3,Run3-reorder [p.25,p.26,p.30,p.276]: the first Copper fetch after DMA-on "
            "is at the address of the location register whose strobe was written, in each of Run 2 "
            "(COP2LC=B, COPJMP2: expect B), Run 3 (...,COPJMP2, COP2LC=C with no further strobe: "
            "expect B, not C) and Run 3-reorder (COP2LC=C moved before COPJMP2: expect C)");

        // ---- Comparator contrast, always run (like the P1/P2 -OFF cases):
        // COP1LC loaded with the OTHER list, then COPJMP1. Doubles as the
        // falsifiability demonstration for the two assertions above: both
        // measure a red on this build through Run 2, so a different, live
        // input through the very same comparator must still reach its own
        // predicted address for the comparator to mean anything.
        checks.expect(ctrl.ok && ctrl.fetch->name == 'B',
            "P4-CTRL [p.25]: COP1LC=B (not A), COPJMP1 -- the first fetch after DMA-on is B's IR1");

        // ---- Precondition (a): the first stored DMACONR has bit 7 = 0
        bool mut_a = (mv == "P4-A");
        P4Result a_case = mut_a ? run_p4(dir + "/prog_p4_mut_a.bin", kSamplerRunBudget) : run1;
        bool pre_a = a_case.cap.peeks.size() >= 1 && a_case.cap.peeks[0] &&
                     ((*a_case.cap.peeks[0]) & 0x0080) == 0;
        checks.expect(pre_a, "P4a precondition: the first stored DMACONR has bit 7 = 0");

        // ---- Precondition (b): no Copper fetch in A, B or C before t(M1)
        bool mut_b = (mv == "P4-B");
        P4Result b_case = mut_b ? run_p4(dir + "/prog_p4_mut_b.bin", kSamplerRunBudget) : run1;
        bool pre_b = b_case.t_m1.has_value() &&
                     !any_channel_read_in(b_case.cap.log, kList1, kList1 + 8, 0, *b_case.t_m1) &&
                     !any_channel_read_in(b_case.cap.log, kList2, kList2 + 8, 0, *b_case.t_m1) &&
                     !any_channel_read_in(b_case.cap.log, kList3, kList3 + 8, 0, *b_case.t_m1);
        checks.expect(pre_b, "P4b precondition: no Copper fetch in A, B or C's region before t(M1)");

        // ---- Precondition (c): no frame wrap and no record with V <= 25
        // lies between t(MS) and the first fetch (p.219: vertical blanking
        // 0-25 for PAL)
        bool mut_c = (mv == "P4-C");
        P4Result c_case = mut_c ? run_p4(dir + "/prog_p4_mut_c.bin", kSamplerRunBudget) : run1;
        checks.expect(c_case.ok && c_case.precond_c,
            "P4c precondition: no frame wrap and no record with V <= 25 lies between t(MS) and the "
            "first fetch");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
