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
// The P4 comparator contrasts (prog_p4_ctrl_cop1b.bin, prog_p4_ctrl_cop1c.bin)
// are not gated by NMH_MUTATE: like prog_p1_off.bin/prog_p2_off.bin, they
// are this package's own contrasting cases and run in every pass.
//
// Every mutation below measured a FAIL on exactly the one line it names, on
// both cores, with every other line unchanged from the normal run (summary
// 95 passed, 6 failed, against the normal run's 96 passed, 5 failed), with
// three exceptions: the P4 lines already red or recorded on every normal
// run (P4, P4-Run2, the P4-Run3 precondition, P4-Run3-reorder,
// P4-Run3-PAIR) stay exactly as they are; NMH_MUTATE=P4-RUN3-PAIR-A instead
// measures a PASS on the line it names, a green demonstration on an
// already-red line (summary 97 passed, 4 failed); and NMH_MUTATE=P3-A
// additionally turns nine other case lines from PASS to RECORDED, not just
// its own P3a (summary 85 passed, 6 failed -- its own entry below says so).
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
//   and the DMACON write, in the run substituted for Run 1 -- measured FAIL
//   on "P4c precondition: ...", both cores
// MUTATION P4-Run3-PAIR comparator green NMH_MUTATE=P4-RUN3-PAIR-A: this
//   line is already red on every normal run (Run 3 does not reach B);
//   P4-CTRL's and P4-CTRL-C's own captures (COP1LC/COPJMP1, not
//   COP2LC/COPJMP2) graded in place of Run 3's and Run 3-reorder's own --
//   measured PASS on "P4-Run3-PAIR ...", both cores: the same paired
//   comparator reads true for a live B/C pair
// MUTATION P3a stimulus red NMH_MUTATE=P3-A: Base's second DSKLEN=$C064
//   write left out, via prog_p3_mut_a.bin -- measured FAIL on "P3a
//   precondition: ...", both cores. Exception to the blanket rule above:
//   with region DSK silent in this run, precondition (a) fails, so the
//   claim ch2-stopping-the-copper.c8/COP2-stopping-the-copper.c11 is not
//   graded whole by the ten cases; P3-DSK becomes RECORDED as inconclusive
//   and the other nine case lines (P3-BPL, P3-SPR, P3-AUD0, P3-AUD1,
//   P3-AUD2, P3-AUD3, P3-BLT, P3-COP, P3-M) also become RECORDED with
//   their ids withdrawn, each naming its own comparator's own reading --
//   measured PASS on all nine, both cores (the mutation touches only
//   Base's own DSK access, not the nine channels' own case runs).
//   P3-BASE is unaffected (its own claim excludes DSK). Summary: 85 passed,
//   6 failed on both cores (ten lines leave the PASS/FAIL tally, P3a itself
//   turns FAIL).
// MUTATION P3b stimulus red NMH_MUTATE=P3-B: Base's DMACON = $83FE (AUD0EN
//   left clear) graded against P = $3FF, via prog_p3_mut_b.bin -- measured
//   FAIL on "P3b precondition: ...", both cores, with every channel but
//   AUD0 still running
// MUTATION P3c stimulus red NMH_MUTATE=P3-C: SPR1PT moved to AUD0's region
//   start (AUD0LC unchanged, at its own Base placement 256 bytes above that
//   same start), via prog_p3_mut_c.bin -- measured FAIL on "P3c
//   precondition: ...", both cores
// MUTATION P3d stimulus red NMH_MUTATE=P3-D: Base's sampler replaced with
//   2.4's own sparse contrast, via prog_p3_mut_d.bin -- measured FAIL on
//   "P3d precondition: ...", both cores
// MUTATION P3-BASE stimulus red NMH_MUTATE=P3-BASE-SWAP: Case M's own
//   capture (DMAEN clear, no region has any access) graded in place of
//   Base's -- measured FAIL on "P3-BASE ...", both cores
// MUTATION P3-BPL stimulus red NMH_MUTATE=P3-BPL-SWAP: Base's own capture
//   graded in place of Case BPL's -- measured FAIL on "P3-BPL ...", both
//   cores
// MUTATION P3-SPR stimulus red NMH_MUTATE=P3-SPR-SWAP: Base's own capture
//   graded in place of Case SPR's -- measured FAIL on "P3-SPR ...", both
//   cores
// MUTATION P3-AUD0 stimulus red NMH_MUTATE=P3-AUD0-SWAP: Base's own capture
//   graded in place of Case AUD0's -- measured FAIL on "P3-AUD0 ...", both
//   cores
// MUTATION P3-AUD1 stimulus red NMH_MUTATE=P3-AUD1-SWAP: Base's own capture
//   graded in place of Case AUD1's -- measured FAIL on "P3-AUD1 ...", both
//   cores
// MUTATION P3-AUD2 stimulus red NMH_MUTATE=P3-AUD2-SWAP: Base's own capture
//   graded in place of Case AUD2's -- measured FAIL on "P3-AUD2 ...", both
//   cores
// MUTATION P3-AUD3 stimulus red NMH_MUTATE=P3-AUD3-SWAP: Base's own capture
//   graded in place of Case AUD3's -- measured FAIL on "P3-AUD3 ...", both
//   cores
// MUTATION P3-DSK stimulus red NMH_MUTATE=P3-DSK-SWAP: Base's own capture
//   graded in place of the DSK case's -- measured FAIL on "P3-DSK ...",
//   both cores
// MUTATION P3-BLT stimulus red NMH_MUTATE=P3-BLT-SWAP: Base's own capture
//   graded in place of Case BLT's -- measured FAIL on "P3-BLT ...", both
//   cores
// MUTATION P3-COP stimulus red NMH_MUTATE=P3-COP-SWAP: Base's own capture
//   graded in place of Case COP's -- measured FAIL on "P3-COP ...", both
//   cores
// MUTATION P3-M stimulus red NMH_MUTATE=P3-M-SWAP: Base's own capture
//   graded in place of Case M's -- measured FAIL on "P3-M ...", both cores
// MUTATION P3-DSKBYTR stimulus red NMH_MUTATE=P3-DSKBYTR-SWAP: Base's own
//   DSKBYTR value graded in place of the DSK case's and case M's own --
//   measured FAIL on "P3-DSKBYTR ...", both cores
// MUTATION P3-REFRESH comparator red NMH_MUTATE=P3-REFRESH-A: a synthetic
//   region containing word address 0 added to the checked region list --
//   measured FAIL on "P3-REFRESH ...", both cores. Comparator
//   demonstration, not a stimulus mutation: refresh is always at word
//   address 0, an invariant no captured program can violate, so this shows
//   the same region-overlap comparator catches a genuine violation.
//
// P5, P7, P9, P11 and P12 are not built in this package: SPECIFICATION.md's
// measured section lists them as not built.
//
// MUTATION P6a stimulus red NMH_MUTATE=P6-A: the C3 MOVE placed after the
//   final WAIT, via prog_p6_mut_a.bin -- measured FAIL on "P6a
//   precondition: ...", both cores
// MUTATION P6b stimulus red NMH_MUTATE=P6-B: DMACON=$8200 (COPEN clear),
//   via prog_p6_mut_b.bin -- measured FAIL on "P6b precondition: ...",
//   both cores
// MUTATION P6c stimulus NMH_MUTATE=P6-C: LACE set (BPLCON0 bit 2), via
//   prog_p6_mut_c.bin -- measured PASS (not FAIL) on "P6c precondition:
//   ...", both cores: this mutation did not redden the check it targets on
//   this build. Reported as measured, not tuned to match the prediction.
// MUTATION P8a stimulus red NMH_MUTATE=P8-A: INTENA=$C020 with a level-3
//   handler that clears bit 5, via prog_p8_cop_mut_a.bin -- measured FAIL
//   on "P8a precondition: ...", both cores
// MUTATION P8b stimulus red NMH_MUTATE=P8-B: the VB case's clear skipped,
//   via prog_p8_vb_mut_b.bin -- measured FAIL on "P8b precondition: ...",
//   both cores
// MUTATION P8c stimulus red NMH_MUTATE=P8-C: DMACON=$8200 (no COPEN) in
//   the COP case, via prog_p8_cop_mut_c.bin -- measured FAIL on "P8c
//   precondition: ...", both cores
// MUTATION P10a stimulus red NMH_MUTATE=P10-A: the second WAIT changed from
//   $5001 to $5A01, via prog_p10_mut_a.bin -- measured FAIL on "P10a
//   precondition: ...", both cores
// MUTATION P10b stimulus red NMH_MUTATE=P10-B: step 2 writes INTREQ=$8004,
//   via prog_p10_mut_b.bin -- measured FAIL on "P10b precondition: ...",
//   both cores
// MUTATION P10c stimulus red NMH_MUTATE=P10-C: INTENA=$C004 with a level-1
//   handler that clears bit 2, via prog_p10_mut_c.bin -- measured FAIL on
//   "P10c precondition: ...", both cores

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
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
constexpr uint32_t kData1 = 0x002000; // copctl.i DATA1 (P8 BLIT case)

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

// ---- full run: keeps the harness alive so video() (P5, P6, P11) is
// available too, unlike Capture/run_capture above (log()/peek only). -----
struct FullRun {
    std::unique_ptr<nmh::NanomigHarness> h;
    bool loaded = false;
    bool done = false;
};

FullRun run_full(const std::string& bin, uint64_t run_budget) {
    FullRun r;
    nmh::Config cfg;
    r.h = std::make_unique<nmh::NanomigHarness>(cfg);
    if (!r.h->load_program(bin)) return r;
    r.loaded = true;
    r.h->boot(kBootBudget);
    r.done = r.h->run_until_write(kDoneAddr, kDoneValue, run_budget).has_value();
    return r;
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

// P4 precondition (c): the program samples nothing between t(MS) and the
// first fetch (SPECIFICATION.md 5.3), so a check that only looks for a
// wrap or a low-V record strictly inside that stretch holds by
// construction, on every run, regardless of whether a vertical blank
// actually fell there. Bound the stretch instead by the records around it
// (2.4's own "line of a port event" construction, applied to t(MS) and the
// first fetch separately): a = the last record stored before t(MS); b =
// the record following the first record stored after the first fetch. No
// wrap lies between a and b, and both read V > 25 (p.219: vertical
// blanking ends at line 25 for PAL) -- so the stretch cannot have crossed
// a vertical blank even though the program itself samples nothing inside
// it.
bool bracket_holds(const std::vector<Rec>& r, uint64_t t_ms, uint64_t t_fetch) {
    auto a = last_record_before(r, t_ms);
    auto c = first_record_after(r, t_fetch);
    if (!a || !c) return false;
    size_t b = *c + 1;
    if (b >= r.size() || *a >= b) return false;
    for (size_t i = *a + 1; i <= b; ++i)
        if (r[i - 1].V >= 256 && r[i].V <= 1) return false; // a wrap lies between a and b
    return r[*a].V > 25 && r[b].V > 25;
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
        r.precond_c = bracket_holds(r.recs, *r.t_ms, r.fetch->cycle);
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

// ---- SPECIFICATION.md 2.3: refresh recognised by the `refresh` port
// (P3 and P5-P12; P1/P2/P4 use the word-address-0 signature instead) -----
//
// nanomig.v declares `refresh` beside `ram_address`/`ram_data` in its own
// port list (both under the RAM-port group), so it is read at the same
// point in the simulation loop nanomig_harness.h already samples the RAM
// port at. Capture here steps one clk_sys cycle at a time through
// ports()/run_cycles(1) (never ->rootp) and records refresh's value each
// cycle, indexed the same way RamAccess.cycle already is (both come from
// the same cycle_ counter), so refresh_at(cycle) can be asked about any
// cycle a RamAccess names.
struct CaptureR : Capture {
    std::vector<uint8_t> refresh; // refresh[i] = the port's value at cycle (refresh_base + i)
    uint64_t refresh_base = 0;
    bool refresh_at(uint64_t cycle) const {
        if (cycle < refresh_base) return false;
        uint64_t i = cycle - refresh_base;
        return i < refresh.size() && refresh[i] != 0;
    }
};

CaptureR run_capture_refresh(const std::string& bin, uint64_t run_budget,
                              const std::vector<uint32_t>& peek_addrs) {
    CaptureR c;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin);
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    const uint32_t done_w = kDoneAddr / 2;
    c.refresh_base = h.cycles();
    size_t seen = h.ram_log().size();
    uint64_t start = h.cycles();
    while (h.cycles() - start < run_budget) {
        h.run_cycles(1);
        c.refresh.push_back(h.ports()->refresh ? 1 : 0);
        const auto& log = h.ram_log();
        bool found = false;
        for (; seen < log.size(); ++seen)
            if (log[seen].is_write && log[seen].word_addr == done_w && log[seen].data == kDoneValue)
                { c.done = log[seen].cycle; found = true; break; }
        if (found) break;
    }
    c.log = h.ram_log();
    for (uint32_t a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    return c;
}

// ---- SPECIFICATION.md 2.8: restarts (P5, P6, P11) ----------------------
//
// A restart is the first Copper fetch after a fetch of the IR2 of a
// list's final WAIT ($FFFF,$FFFE). end_ir2_bytes names every list's own
// end-of-list IR2 byte address that can occur in this run.
struct Restart { uint64_t t_final; uint64_t t_restart; uint32_t addr_byte; };

std::vector<Restart> find_restarts(const std::vector<nmh::RamAccess>& log,
                                    const std::vector<uint32_t>& end_ir2_bytes) {
    std::vector<uint32_t> words;
    for (uint32_t b : end_ir2_bytes) words.push_back(b / 2);
    std::vector<Restart> out;
    for (size_t i = 0; i < log.size(); ++i) {
        const auto& a = log[i];
        if (a.is_write || a.word_addr == 0) continue;
        bool match = false;
        for (uint32_t w : words) if (a.word_addr == w) match = true;
        if (!match) continue;
        for (size_t j = i + 1; j < log.size(); ++j) {
            const auto& b = log[j];
            if (b.is_write || b.word_addr == 0) continue;
            out.push_back({a.cycle, b.cycle, b.word_addr * 2});
            break;
        }
    }
    return out;
}

// Count of channel reads at one exact word address within [lo,hi) -- P7's
// per-line execution counts, P6's per-restart MOVE-order check.
int count_channel_reads_at(const std::vector<nmh::RamAccess>& log, uint32_t word_addr,
                            uint64_t lo, uint64_t hi) {
    int n = 0;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr == word_addr && a.cycle >= lo && a.cycle < hi) ++n;
    return n;
}

// First channel read at one exact word address, strictly after after_t.
std::optional<uint64_t> first_channel_read_at(const std::vector<nmh::RamAccess>& log, uint32_t word_addr,
                                               uint64_t after_t) {
    for (const auto& a : log)
        if (!a.is_write && a.word_addr == word_addr && a.cycle > after_t) return a.cycle;
    return std::nullopt;
}

// Count/first/last of writes of one exact value inside a byte region and
// time window -- P12's blit-write tracking (region D1/D2 filled with
// $FFFF), P8's BLIT case.
struct WriteStats { int count = 0; std::optional<uint64_t> first, last; };
WriteStats writes_of_value_in(const std::vector<nmh::RamAccess>& log, uint16_t value,
                               uint32_t region_lo, uint32_t region_hi,
                               uint64_t lo, uint64_t hi) {
    WriteStats s;
    for (const auto& a : log) {
        if (!a.is_write || a.data != value) continue;
        uint32_t b = a.word_addr * 2;
        if (b < region_lo || b >= region_hi) continue;
        if (a.cycle < lo || a.cycle >= hi) continue;
        ++s.count;
        if (!s.first) s.first = a.cycle;
        s.last = a.cycle;
    }
    return s;
}

// ---- SPECIFICATION.md P8: one INTREQ bit polled across markers/events --
//
// A single pass over the poll records grades observation (i)-(iii) and
// precondition (b) together: zero_before is (i), later_one is (ii),
// after_first_one is (iii), first_after_mc_zero is precondition (b).
struct P8Grade {
    bool have_pre = false, first_after_mc_zero = false;
    bool zero_before = true, later_one = false, after_first_one = true;
};
P8Grade grade_p8_bit(const std::vector<Rec>& recs, uint64_t t_mc, uint64_t t_event, int bit) {
    P8Grade g;
    bool seen_one = false;
    std::optional<Rec> first_after_mc;
    for (const auto& r : recs) {
        if (r.t0 <= t_mc) continue;
        if (!first_after_mc) first_after_mc = r;
        bool bitval = ((r.intreqr >> bit) & 1) != 0;
        if (r.t1 < t_event && bitval) g.zero_before = false;
        if (r.t0 > t_event && bitval) g.later_one = true;
        if (seen_one && !bitval) g.after_first_one = false;
        if (bitval) seen_one = true;
    }
    g.have_pre = first_after_mc.has_value();
    if (first_after_mc) g.first_after_mc_zero = ((first_after_mc->intreqr >> bit) & 1) == 0;
    return g;
}

// ---- SPECIFICATION.md 2.5: reference frame V (P5, P6, P11) -------------
//
// Anchor V1: every one of the given video spans has the same sample count
// and the same doubler row count (nmh::find_lines).
bool anchor_V1(const std::vector<nmh::VideoSample>& video, const std::vector<nmh::Span>& spans) {
    if (spans.size() < 2) return false;
    std::vector<nmh::VideoSample> sub0(video.begin() + spans[0].begin, video.begin() + spans[0].end);
    size_t n0 = sub0.size();
    size_t rows0 = nmh::find_lines(sub0).size();
    for (size_t i = 1; i < spans.size(); ++i) {
        std::vector<nmh::VideoSample> sub(video.begin() + spans[i].begin, video.begin() + spans[i].end);
        if (sub.size() != n0) return false;
        if (nmh::find_lines(sub).size() != rows0) return false;
    }
    return true;
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
        auto ctrl_c = run_p4(dir + "/prog_p4_ctrl_cop1c.bin", kSamplerRunBudget);

        checks.expect(run1.ok, "P4 fixture: Run 1 (prog_p4_run1.bin) produced a decided first fetch after M1");
        checks.expect(run2.ok, "P4 fixture: Run 2 (prog_p4_run2.bin) produced a decided first fetch after M1");
        checks.expect(run3.ok, "P4 fixture: Run 3 (prog_p4_run3.bin) produced a decided first fetch after M1");
        checks.expect(run3r.ok,
            "P4 fixture: Run 3 reordered (prog_p4_run3_reorder.bin) produced a decided first fetch "
            "after M1");
        checks.expect(ctrl.ok,
            "P4 fixture: the comparator contrast (prog_p4_ctrl_cop1b.bin) produced a decided first "
            "fetch after M1");
        checks.expect(ctrl_c.ok,
            "P4 fixture: the C comparator contrast (prog_p4_ctrl_cop1c.bin) produced a decided first "
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

        // ---- The same rule, one assertion per run. None carries a claim id
        // of its own (SPECIFICATION.md section 1 lists only c1.s2 and c2.s2,
        // both on P4): P4-Run2 states P4's own clause (the strobe written
        // while Copper DMA is off sets the start address) for Run 2 alone;
        // P4-Run3 and P4-Run3-reorder grade a narrower clause -- the
        // location register's own value at the moment the strobe is
        // written, p.25 "is loaded into the Copper program counter" -- since
        // each rewrites the strobed register afterwards, before or after
        // the strobe. Green control for P4-Run2 and P4-Run3 (both expect
        // B): P4-CTRL below. Green control for P4-Run3-reorder (expects C):
        // P4-CTRL-C below.
        checks.expect(run2.ok && run2.fetch->name == 'B',
            "P4-Run2 [p.25,p.26,p.30,p.276]: the strobe written while Copper DMA is off sets the "
            "start address -- COP1LC=A, COP2LC=B, COPJMP2: expect B");

        // ---- P4-Run3's own clause (B against C) is reached only when the
        // first fetch is B or C: on this build the first fetch is A, the
        // same clause P4-Run2 already reads red through, so the clause is
        // not exercised. The same field this precondition reads
        // (.fetch->name) is shown taking the live values B and C by
        // P4-CTRL and P4-CTRL-C below, so the comparator is not vacuous.
        bool run3_reached = run3.ok && (run3.fetch->name == 'B' || run3.fetch->name == 'C');
        checks.expect(run3_reached,
            "P4-Run3 precondition: the first fetch is B or C (the labelled clause, B against C, "
            "is reached only then)");
        if (run3_reached) {
            checks.expect(run3.fetch->name == 'B',
                "P4-Run3 [p.25]: the location register's value at the strobe (\"is loaded into the "
                "Copper program counter\") decides the start address -- COP1LC=A, COP2LC=B, COPJMP2, "
                "then COP2LC=C with no further strobe: COP2LC held B when COPJMP2 was written, expect "
                "B, not C");
        } else {
            checks.record(
                std::string("P4-Run3 [p.25]: not reached -- the first fetch is neither B nor C (it is ") +
                (run3.ok ? std::string(1, run3.fetch->name) : std::string("undecided")) +
                "), so the labelled clause (B against C) was not exercised this run");
        }

        // ---- P4-Run3-reorder alone cannot isolate "the value at the
        // strobe": COP2LC holds C both at the strobe and at DMA-on, so its
        // own comparator repeats P4-Run2's clause with C rather than
        // contrasting an earlier value against a later one. Paired with
        // P4-Run3 (which does hold an earlier value, B, at the strobe) as
        // the specification's own pair (P4 items 4 and 6: "Run 2, Run 3 and
        // Run 3-reorder are graded together against this rule"), the two
        // runs together show the rule discriminating the two orderings.
        // Comparator and expected value (C) unchanged; only this rationale
        // and the P4-Run3-PAIR line below are new.
        checks.expect(run3r.ok && run3r.fetch->name == 'C',
            "P4-Run3-reorder [p.25]: COP2LC holds C both at the strobe and at DMA-on, so this line "
            "alone repeats P4-Run2's clause with C (see P4-Run3-PAIR for the ordering contrast with "
            "P4-Run3) -- COP1LC=A, COP2LC=B, then COP2LC=C moved before COPJMP2: expect C");

        // ---- P4-Run3-PAIR: Run 3 and Run 3-reorder graded together, as
        // the specification's own pair. Already red on this build (Run 3
        // does not reach B); its own green demonstration substitutes
        // P4-CTRL's and P4-CTRL-C's already-built captures (B and C
        // through COP1LC/COPJMP1, not through the COP2LC/COPJMP2 path Run
        // 3 and Run 3-reorder use) for Run 3's and Run 3-reorder's own,
        // showing the same paired comparator reads true for a live B/C
        // pair.
        bool mut_pair = (mv == "P4-RUN3-PAIR-A");
        bool pair_ok = mut_pair
            ? (ctrl.ok && ctrl.fetch->name == 'B' && ctrl_c.ok && ctrl_c.fetch->name == 'C')
            : (run3.ok && run3r.ok && run3.fetch->name == 'B' && run3r.fetch->name == 'C');
        checks.expect(pair_ok,
            "P4-Run3-PAIR [p.25]: Run 3 (COP2LC rewritten to C after the strobe: expect B) and Run "
            "3-reorder (COP2LC=C moved before the strobe: expect C), graded together as the "
            "specification's own pair");

        // ---- Comparator contrasts, always run (like the P1/P2 -OFF
        // cases): COP1LC loaded with another list, then COPJMP1 -- the
        // COP1LC/COPJMP1 path, not the COP2LC/COPJMP2 path Run 2, Run 3 and
        // Run 3-reorder use. P4-CTRL is P4's and P4-Run2's own
        // falsifiability demonstration and P4-Run3's green control (both
        // expect B). P4-CTRL-C is P4-Run3-reorder's and P4-Run3-PAIR's
        // green control for C.
        checks.expect(ctrl.ok && ctrl.fetch->name == 'B',
            "P4-CTRL [p.25]: COP1LC=B (not A), COPJMP1 -- the first fetch after DMA-on is B's IR1");
        checks.expect(ctrl_c.ok && ctrl_c.fetch->name == 'C',
            "P4-CTRL-C [p.25]: COP1LC=C (not A or B), COPJMP1 -- the first fetch after DMA-on is "
            "C's IR1");

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

        // ---- Precondition (c): the program samples nothing between t(MS)
        // and the first fetch, so a check that only looks for a wrap or a
        // low-V record strictly inside that stretch would hold by
        // construction on every run. Bound the stretch instead by the
        // records around it (2.4): the last record stored before t(MS) and
        // the record following the first record stored after the first
        // fetch lie in the same frame (no wrap between them) and both read
        // V > 25 (p.219), graded on every run's own capture -- Run 1, Run
        // 2, Run 3 and Run 3-reorder each write the location registers and
        // the strobe against their own beam record, so a vertical blank
        // could in principle intervene in any one of them even where it
        // does not in the others. The mutation substitutes
        // prog_p4_mut_c.bin (built on Run 1's own sequence) for Run 1's
        // capture; Run 2, Run 3 and Run 3-reorder are still graded on their
        // own normal captures, so the aggregate still goes red through the
        // one run the mutation changed.
        bool mut_c = (mv == "P4-C");
        P4Result c_case = mut_c ? run_p4(dir + "/prog_p4_mut_c.bin", kSamplerRunBudget) : run1;
        bool pre_c = c_case.ok && c_case.precond_c &&
                     run2.ok && run2.precond_c &&
                     run3.ok && run3.precond_c &&
                     run3r.ok && run3r.precond_c;
        checks.expect(pre_c,
            "P4c precondition: the last record stored before t(MS) and the record following the "
            "first record stored after the first fetch lie in the same frame and both read V > 25, "
            "in each of Run 1, Run 2, Run 3 and Run 3-reorder");
    }

    // ==== P3: DMACON controls every DMA channel [p.30, p.222, p.278] ====
    // ch2-stopping-the-copper.c8, COP2-stopping-the-copper.c11
    {
        // ---- Region table (this package's own $10000-$57FFF map,
        // SPECIFICATION.md P3 stimulus step 2): kP3PtrOffset is how far
        // above each region's own start the program writes that channel's
        // own pointer or location register (SPECIFICATION.md P3 stimulus),
        // which precondition (c) compares the lowest in-region access
        // against.
        struct P3Region { const char* name; uint32_t lo, hi; };
        enum { R_BPL = 0, R_SPR, R_AUD0, R_AUD1, R_AUD2, R_AUD3, R_DSK, R_BLT, R_COP, R_N };
        const P3Region kP3Regions[R_N] = {
            {"BPL",  0x010000, 0x018000}, {"SPR",  0x018000, 0x020000},
            {"AUD0", 0x020000, 0x028000}, {"AUD1", 0x028000, 0x030000},
            {"AUD2", 0x030000, 0x038000}, {"AUD3", 0x038000, 0x040000},
            {"DSK",  0x040000, 0x048000}, {"BLT",  0x048000, 0x050000},
            {"COP",  0x050000, 0x058000},
        };
        constexpr uint32_t kP3PtrOffset = 0x100;

        // ---- Apparatus check, part (i): the refresh port is never
        // asserted anywhere but word address 0 (SPECIFICATION.md 2.3: "a
        // refresh access is one made while the `refresh` output ... is
        // asserted", the recognition P3 and P5-P12 use, unlike P1/P2/P4's
        // own word-address-0 signature). Checked here against prog_p1.bin's
        // own captured log (already built, part of P1), requiring at least
        // one refresh-asserted read to exist -- otherwise the check would
        // pass vacuously on a log with no such read at all. The converse is
        // not asserted: a word-address-0 read can be something other than
        // refresh (measured: one such read, elsewhere in this same log,
        // where the refresh port reads clear) -- the port is the finer
        // signal, which is exactly why P3 and P5-P12 use it instead of the
        // address alone.
        CaptureR rc1 = run_capture_refresh(dir + "/prog_p1.bin", kSamplerRunBudget, {});
        bool refresh_ok = rc1.loaded && rc1.done.has_value();
        bool any_read = false, any_refresh_read = false;
        if (refresh_ok) {
            for (const auto& a : rc1.log) {
                if (a.is_write) continue;
                any_read = true;
                if (rc1.refresh_at(a.cycle)) {
                    any_refresh_read = true;
                    if (a.word_addr != 0) { refresh_ok = false; break; }
                }
            }
        }
        checks.expect(refresh_ok && any_read && any_refresh_read,
            "P3 apparatus (i) [SPECIFICATION.md 2.3]: every RAM read where the refresh port "
            "(nanomig.v's `refresh` output) reads asserted is at word address 0, and at least one "
            "such read exists, checked on prog_p1.bin's own captured log [not graded against the "
            "page -- apparatus check, no claim uid]");

        // ---- Apparatus check, part (ii): the same recognition checked
        // directly on a P3 capture, inside P3's own regions, rather than
        // inferred from part (i)'s different addresses. At least one
        // refresh-asserted read exists, and none of them falls inside any
        // of the nine regions. Comparator demonstration
        // (NMH_MUTATE=P3-REFRESH-A): refresh is always at word address 0,
        // an invariant no captured program can violate, so no stimulus
        // change can redden this check; a synthetic region containing word
        // address 0 is added to the checked list instead, showing the same
        // region-overlap comparator catches a genuine violation.
        bool mut_refresh_a = (mv == "P3-REFRESH-A");
        std::vector<std::pair<uint32_t, uint32_t>> refresh_regions;
        for (int i = 0; i < R_N; ++i) refresh_regions.push_back({kP3Regions[i].lo, kP3Regions[i].hi});
        if (mut_refresh_a) refresh_regions.push_back({0, 8});

        CaptureR rc3 = run_capture_refresh(dir + "/prog_p3_run_base.bin", kSamplerRunBudget, {});
        bool part2_ok = rc3.loaded && rc3.done.has_value();
        bool part2_any_refresh_read = false, part2_none_in_region = true;
        if (part2_ok) {
            for (const auto& a : rc3.log) {
                if (!rc3.refresh_at(a.cycle)) continue;
                if (!a.is_write) part2_any_refresh_read = true;
                uint32_t b = a.word_addr * 2;
                for (const auto& rgn : refresh_regions)
                    if (b >= rgn.first && b < rgn.second) part2_none_in_region = false;
            }
        }
        checks.expect(part2_ok && part2_any_refresh_read && part2_none_in_region,
            "P3-REFRESH (ii) [SPECIFICATION.md 2.3]: on prog_p3_run_base.bin's own captured log, at "
            "least one RAM access where the refresh port reads asserted exists, and none of them "
            "falls inside any of the nine regions [not graded against the page -- apparatus check, "
            "no claim uid]");

        struct P3CaseResult {
            Capture cap;
            bool ok = false;
            bool have_dskbytr = false, have_dmaconr = false;
            uint16_t dskbytr = 0, dmaconr = 0;
            bool region_access[R_N] = {};
            std::optional<uint32_t> region_lowest[R_N];
        };

        // Region-bounded access, refresh excluded by word address 0 (as
        // P1/P2/P4 already do): every one of this package's regions starts
        // at $10000 or above, so a word-address-0 access -- refresh's own
        // address, per the apparatus check just above, over a substantial
        // captured log -- can never fall inside one. The port and the
        // address method therefore agree on every region-bounded question
        // this proposition asks; run_capture (the fast path P1/P2/P4 use)
        // is used here rather than the slow, cycle-stepped refresh capture.
        auto run_p3_case = [&](const std::string& bin, uint64_t budget) -> P3CaseResult {
            P3CaseResult r;
            r.cap = run_capture(dir + bin, budget,
                                 {kResults + 4, kResults + 6, kResults + 8, kResults + 10});
            if (!r.cap.loaded || !r.cap.done.has_value()) return r;
            if (r.cap.peeks.size() >= 1 && r.cap.peeks[0]) { r.dskbytr = *r.cap.peeks[0]; r.have_dskbytr = true; }
            if (r.cap.peeks.size() >= 2 && r.cap.peeks[1]) { r.dmaconr = *r.cap.peeks[1]; r.have_dmaconr = true; }
            auto t_m2 = first_write_cycle(r.cap.log, kMarkBuf + 2);
            if (!t_m2) return r;
            uint32_t count = (r.cap.peeks.size() >= 4 && r.cap.peeks[2] && r.cap.peeks[3])
                                 ? (uint32_t(*r.cap.peeks[2]) << 16) | *r.cap.peeks[3] : 0;
            auto recs = decode_records(r.cap.log, kRecBuf, count);
            auto frames = record_frames(recs);
            int f0 = first_frame_after(recs, frames, *t_m2);
            if (f0 < 0) return r;
            // Observation window: t(M2) to the end of the first complete
            // frame after M2 (SPECIFICATION.md P3 Observation) -- not the
            // frame's own [lo,hi), whose low edge sits at the wrap before
            // M2's frame, well before t(M2) itself.
            auto frame_range = frame_time_range(recs, frames, (size_t)f0);
            uint64_t win_lo = *t_m2, win_hi = frame_range.second;
            for (const auto& a : r.cap.log) {
                if (a.cycle < win_lo || a.cycle >= win_hi) continue;
                if (a.word_addr == 0) continue; // refresh
                uint32_t b = a.word_addr * 2;
                for (int i = 0; i < R_N; ++i) {
                    if (b < kP3Regions[i].lo || b >= kP3Regions[i].hi) continue;
                    r.region_access[i] = true;
                    if (!r.region_lowest[i] || b < *r.region_lowest[i]) r.region_lowest[i] = b;
                }
            }
            r.ok = true;
            return r;
        };

        // ---- The eleven runs (SPECIFICATION.md P3 stimulus table). Each
        // is its own full run; off_region is the region cleared (-1: none,
        // Base; -2: every channel, Case M).
        struct P3Case { const char* label; const char* bin; uint16_t expected_p; int off_region; };
        const P3Case kP3Cases[] = {
            {"Base", "/prog_p3_run_base.bin", 0x3FF, -1},
            {"BPL",  "/prog_p3_off_bpl.bin",  0x2FF, R_BPL},
            {"SPR",  "/prog_p3_off_spr.bin",  0x3DF, R_SPR},
            {"AUD0", "/prog_p3_off_aud0.bin", 0x3FE, R_AUD0},
            {"AUD1", "/prog_p3_off_aud1.bin", 0x3FD, R_AUD1},
            {"AUD2", "/prog_p3_off_aud2.bin", 0x3FB, R_AUD2},
            {"AUD3", "/prog_p3_off_aud3.bin", 0x3F7, R_AUD3},
            {"DSK",  "/prog_p3_off_dsk.bin",  0x3EF, R_DSK},
            {"BLT",  "/prog_p3_off_blt.bin",  0x3BF, R_BLT},
            {"COP",  "/prog_p3_off_cop.bin",  0x37F, R_COP},
            {"M",    "/prog_p3_case_m.bin",   0x1FF, -2},
        };
        constexpr int kP3N = sizeof(kP3Cases) / sizeof(kP3Cases[0]);
        constexpr uint64_t kP3Budget = kSamplerRunBudget; // same margin P1/P2/P4 already need

        P3CaseResult case_results[kP3N];
        for (int i = 0; i < kP3N; ++i) case_results[i] = run_p3_case(kP3Cases[i].bin, kP3Budget);

        for (int i = 0; i < kP3N; ++i) {
            checks.expect(case_results[i].ok,
                std::string("P3 fixture: Case ") + kP3Cases[i].label + " (" + (kP3Cases[i].bin + 1) +
                    ") reaches DONE and captures one complete frame after M2");
        }

        // ---- Precondition (a): region DSK has at least one access in
        // Base (section 4, item 12: the pages do not say whether disk DMA
        // runs with no drive attached).
        bool mut_a = (mv == "P3-A");
        P3CaseResult a_case = mut_a ? run_p3_case("/prog_p3_mut_a.bin", kP3Budget) : case_results[0];
        bool dsk_established = a_case.ok && a_case.region_access[R_DSK];
        checks.expect(dsk_established,
            "P3a precondition: region DSK has at least one access in Base");

        // ---- Precondition (b): in every run, the stored DMACONR bits 9-0
        // equal that run's own pattern P (p.222).
        bool mut_b = (mv == "P3-B");
        P3CaseResult b_mut_case = mut_b ? run_p3_case("/prog_p3_mut_b.bin", kP3Budget) : P3CaseResult{};
        bool pre_b = true;
        for (int i = 0; i < kP3N; ++i) {
            const P3CaseResult& cr = (mut_b && i == 0) ? b_mut_case : case_results[i];
            if (!cr.ok || !cr.have_dmaconr || (cr.dmaconr & 0x03FF) != kP3Cases[i].expected_p) pre_b = false;
        }
        checks.expect(pre_b,
            "P3b precondition: in every one of the eleven runs, the stored DMACONR bits 9-0 equal "
            "that run's own pattern P [p.222]");

        // ---- Precondition (c): in Base, the lowest address accessed in
        // each region is the address the program wrote into that region's
        // own channel -- each channel's own pointer, kP3PtrOffset above its
        // region's own start (SPECIFICATION.md P3 stimulus; for SPR, the
        // lowest of SPR0PT-SPR7PT, which is also SPR0PT at that same
        // offset). Compared against that fixed, program-controlled address,
        // not the region's own start: a region's own start would leave
        // nothing below the pointer for another channel's accesses to land
        // in, so no such intrusion could ever be caught.
        bool mut_c = (mv == "P3-C");
        P3CaseResult c_case = mut_c ? run_p3_case("/prog_p3_mut_c.bin", kP3Budget) : case_results[0];
        bool pre_c = c_case.ok;
        for (int i = 0; i < R_N; ++i) {
            if (!c_case.region_lowest[i]) continue; // nothing accessed there: no violation to find
            if (*c_case.region_lowest[i] != kP3Regions[i].lo + kP3PtrOffset) pre_c = false;
        }
        checks.expect(pre_c,
            "P3c precondition: in Base, the lowest address accessed in each region is the address "
            "the program wrote into that region's own channel");

        // ---- Precondition (d): anchors B1 and B3 for the frame boundary,
        // over every one of the eleven runs' own graded frame (2.4's own
        // sparse-sampler contrast substitutes for the Base run only).
        bool mut_d = (mv == "P3-D");
        P3CaseResult d_mut_case = mut_d ? run_p3_case("/prog_p3_mut_d.bin", kP3Budget) : P3CaseResult{};
        bool pre_d = true;
        for (int i = 0; i < kP3N; ++i) {
            const P3CaseResult& src = (mut_d && i == 0) ? d_mut_case : case_results[i];
            if (!src.ok) { pre_d = false; continue; }
            auto t_m2 = first_write_cycle(src.cap.log, kMarkBuf + 2);
            uint32_t count = (src.cap.peeks.size() >= 4 && src.cap.peeks[2] && src.cap.peeks[3])
                                 ? (uint32_t(*src.cap.peeks[2]) << 16) | *src.cap.peeks[3] : 0;
            auto recs = decode_records(src.cap.log, kRecBuf, count);
            auto frames = record_frames(recs);
            int f0 = t_m2 ? first_frame_after(recs, frames, *t_m2) : -1;
            if (f0 < 0) { pre_d = false; continue; }
            bool b1 = anchor_B1(recs, frames[f0], 0x10, 0xFF);
            auto b23 = anchor_B2_B3(recs, frames[f0], 0x10, 0xFF);
            if (!b1 || !b23.second) pre_d = false;
        }
        checks.expect(pre_d,
            "P3d precondition: anchor checks B1 and B3 hold (lines $10-$FF) for the frame boundary, "
            "over every one of the eleven runs' own graded frame");

        // ---- Base coverage: "Base: every region other than DSK has at
        // least one access" (P3 item 4) -- the "enables" half of
        // "controls". Without this, a channel that never ran in any run
        // would leave every case line and P3-M green without ever being
        // exercised. Own mutation (NMH_MUTATE=P3-BASE-SWAP): Case M's
        // already-built capture (DMAEN clear, no region has any access)
        // graded in Base's place.
        bool mut_base_swap = (mv == "P3-BASE-SWAP");
        const P3CaseResult& base_for_coverage = mut_base_swap ? case_results[10] : case_results[0];
        bool base_cov_ok = base_for_coverage.ok;
        for (int r = 0; r < R_N; ++r) {
            if (r == R_DSK) continue;
            if (!base_for_coverage.region_access[r]) base_cov_ok = false;
        }
        checks.expect(base_cov_ok,
            "P3-BASE ch2-stopping-the-copper.c8,COP2-stopping-the-copper.c11 [p.30,p.222,p.278]: in "
            "Base, every region other than DSK has at least one access from t(M2) to the end of "
            "the first complete frame after M2");

        // ---- The claim-bearing cases: the nine channel cases and case M
        // (ch2-stopping-the-copper.c8, COP2-stopping-the-copper.c11 are
        // graded whole by these ten -- SPECIFICATION.md section 3, P3).
        // Each case's own mutation (NMH_MUTATE=P3-<label>-SWAP) grades
        // Base's own already-built capture in that case's own slot. When
        // precondition (a) fails this run (region DSK showed no access in
        // Base), the claim is not graded whole by these ten cases (DSK's
        // own case is left out), so every other case's own ids are
        // withdrawn to a record too, not just DSK's.
        for (int i = 1; i < kP3N; ++i) {
            const P3Case& kc = kP3Cases[i];
            std::string swap_key = std::string("P3-") + kc.label + "-SWAP";
            const P3CaseResult& cr = (mv == swap_key) ? case_results[0] : case_results[i];
            std::string tag = std::string("ch2-stopping-the-copper.c8,COP2-stopping-the-copper.c11 "
                                           "[p.30,p.222,p.278]");
            if (kc.off_region == -2) {
                bool all_off = cr.ok;
                for (int r = 0; r < R_N; ++r) if (cr.region_access[r]) all_off = false;
                if (dsk_established) {
                    checks.expect(all_off,
                        std::string("P3-M ") + tag + ": with DMAEN (bit 9) clear, no region has an "
                        "access from t(M2) to the end of the first complete frame after M2");
                } else {
                    checks.record(std::string("P3-M [ids withdrawn: ") + tag + "]: region DSK's own "
                        "case is not graded this run (precondition (a) failed), so the claim is not "
                        "graded whole by the ten cases; this case's own comparator read " +
                        (all_off ? "PASS" : "FAIL"));
                }
                continue;
            }
            int idx = kc.off_region;
            if (idx == R_DSK && !dsk_established) {
                checks.record(std::string("P3-DSK ") + tag + ": region DSK showed no access in "
                    "Base (precondition (a) failed on this build), so the DSK case is inconclusive "
                    "and not graded");
                continue;
            }
            bool ok = cr.ok && !cr.region_access[idx] &&
                      case_results[0].ok && case_results[0].region_access[idx];
            for (int r = 0; r < R_N; ++r) {
                if (r == idx) continue;
                if (r == R_DSK && !dsk_established) continue; // left out per precondition (a)
                if (!case_results[0].ok || !case_results[0].region_access[r]) continue; // Base was silent there too
                if (!cr.region_access[r]) ok = false;
            }
            char pat_buf[8];
            std::snprintf(pat_buf, sizeof pat_buf, "$%04X", kc.expected_p | 0x8000);
            std::string pat = pat_buf;
            std::string label = std::string("P3-") + kc.label + " " + tag + ": with DMACON=" + pat +
                " (region " + kc.label + "'s bit clear), region " + kc.label + " has no access "
                "from t(M2) to the end of the first complete frame after M2, region " + kc.label +
                " had one in Base, and every other region that had one in Base still does";
            if (dsk_established) {
                checks.expect(ok, label);
            } else {
                checks.record(std::string("P3-") + kc.label + " [ids withdrawn: " + tag + "]: "
                    "region DSK's own case is not graded this run (precondition (a) failed), so "
                    "the claim is not graded whole by the ten cases; this case's own comparator "
                    "read " + (ok ? "PASS" : "FAIL"));
            }
        }

        // ---- DSKBYTR bit 14 (DMAON, p.248): 1 in Base, 0 in the DSK case
        // and in case M. Own mutation (NMH_MUTATE=P3-DSKBYTR-SWAP): Base's
        // own already-built DSKBYTR value graded in the DSK case's and
        // case M's own slots.
        {
            bool mut_dskbytr = (mv == "P3-DSKBYTR-SWAP");
            const P3CaseResult& dsk_src = mut_dskbytr ? case_results[0] : case_results[7]; // "DSK" is index 7
            const P3CaseResult& m_src = mut_dskbytr ? case_results[0] : case_results[10];  // "M" is index 10
            bool ok = case_results[0].ok && case_results[0].have_dskbytr &&
                      (case_results[0].dskbytr & 0x4000) != 0 &&
                      dsk_src.ok && dsk_src.have_dskbytr && (dsk_src.dskbytr & 0x4000) == 0 &&
                      m_src.ok && m_src.have_dskbytr && (m_src.dskbytr & 0x4000) == 0;
            checks.expect(ok,
                "P3-DSKBYTR [p.248]: DSKBYTR bit 14 (DMAON) reads 1 in Base and 0 in the DSK case "
                "and in case M");
        }
    }

    // ==== P10: INTREQ bit 15 decides whether bits written as 1 are set
    // or cleared [p.35] ==== ch2-the-copper-and-the-680x0.c5,
    // COP2-the-copper-and-the-680x0.c5,.c6,.c7
    {
        auto sample_idx_on_line = [](const std::vector<Rec>& recs, int line) -> std::optional<size_t> {
            for (size_t i = 0; i < recs.size(); ++i) if (recs[i].V == line) return i;
            return std::nullopt;
        };
        auto p10_load = [&](const std::string& bin) {
            return run_capture(dir + bin, kSamplerRunBudget, {kResults + 4, kResults + 8, kResults + 10});
        };
        auto p10_decode = [&](const Capture& c) {
            uint32_t count = (c.peeks.size() >= 3 && c.peeks[1] && c.peeks[2])
                                  ? (uint32_t(*c.peeks[1]) << 16) | *c.peeks[2] : 0;
            return decode_records(c.log, kRecBuf, count);
        };
        const int p10_lines[5] = {0x38, 0x48, 0x58, 0x68, 0x78};

        Capture base = p10_load("/prog_p10_main.bin");
        bool base_ok = base.loaded && base.done.has_value();
        checks.expect(base_ok, "P10 fixture: prog_p10_main.bin reaches DONE");
        std::vector<Rec> recs = base_ok ? p10_decode(base) : std::vector<Rec>{};

        std::optional<size_t> idx[5];
        bool have_all = base_ok;
        for (int k = 0; k < 5; ++k) {
            idx[k] = sample_idx_on_line(recs, p10_lines[k]);
            if (!idx[k]) have_all = false;
        }
        checks.expect(have_all, "P10 fixture: all five sample lines ($38,$48,$58,$68,$78) have a record");

        if (have_all) {
            const int expected_b4[5] = {0, 1, 0, 0, 1};
            const int expected_b2[5] = {0, 1, 1, 0, 0};
            bool table_ok = true;
            for (int k = 0; k < 5; ++k) {
                int b4 = (recs[*idx[k]].intreqr >> 4) & 1;
                int b2 = (recs[*idx[k]].intreqr >> 2) & 1;
                if (b4 != expected_b4[k] || b2 != expected_b2[k]) table_ok = false;
            }
            checks.expect(table_ok,
                "P10 ch2-the-copper-and-the-680x0.c5,COP2-the-copper-and-the-680x0.c5,"
                "COP2-the-copper-and-the-680x0.c6,COP2-the-copper-and-the-680x0.c7 [p.35,p.218,p.280]: "
                "(bit 4, bit 2) at lines $38/$48/$58/$68/$78 read (0,0),(1,1),(0,1),(0,0),(1,0)");

            // ---- Main contrast: 2nd MOVE $0010->$8010, 3rd MOVE
            // $0004->$8004 (one combined run). No claim id.
            Capture main2 = p10_load("/prog_p10_main2.bin");
            bool main2_ok = main2.loaded && main2.done.has_value();
            if (main2_ok) {
                auto recs2 = p10_decode(main2);
                auto i58 = sample_idx_on_line(recs2, 0x58);
                auto i68 = sample_idx_on_line(recs2, 0x68);
                bool ok = i58 && i68 &&
                          ((recs2[*i58].intreqr >> 4) & 1) == 1 && ((recs2[*i58].intreqr >> 2) & 1) == 1 &&
                          ((recs2[*i68].intreqr >> 2) & 1) == 1;
                checks.expect(ok,
                    "P10-MAIN2 [p.35,p.218]: with the 2nd MOVE's data $0010->$8010, the $58 sample "
                    "reads (1,1); with the 3rd MOVE's data $0004->$8004 as well, the $68 sample's "
                    "bit 2 reads 1");
            } else {
                checks.expect(false, "P10-MAIN2 fixture: prog_p10_main2.bin reaches DONE");
            }

            // ---- Precondition (a): order -- MOVE k's IR2 fetch lies
            // after the store of sample k-1 and before the store of the
            // record just before sample k.
            bool mut_a = (mv == "P10-A");
            Capture cap_a;
            const Capture& a_cap = mut_a ? (cap_a = p10_load("/prog_p10_mut_a.bin")) : base;
            bool have_a = a_cap.loaded && a_cap.done.has_value();
            std::vector<Rec> recs_a = have_a ? p10_decode(a_cap) : std::vector<Rec>{};
            std::optional<size_t> idx_a[5];
            if (have_a) for (int k = 0; k < 5; ++k) { idx_a[k] = sample_idx_on_line(recs_a, p10_lines[k]); if (!idx_a[k]) have_a = false; }
            const uint32_t move_ir2_word[4] = {
                kList1 / 2 + 3, kList1 / 2 + 7, kList1 / 2 + 11, kList1 / 2 + 15,
            };
            bool order_ok = have_a;
            if (have_a) {
                for (int k = 0; k < 4; ++k) {
                    auto t_move = first_channel_read_at(a_cap.log, move_ir2_word[k], 0);
                    if (!t_move) { order_ok = false; continue; }
                    uint64_t lo = recs_a[*idx_a[k]].t1;
                    uint64_t hi = recs_a[*idx_a[k + 1] - 1].t1;
                    if (!(*t_move > lo && *t_move < hi)) order_ok = false;
                }
            }
            checks.expect(order_ok,
                "P10a precondition: each MOVE's IR2 fetch lies after the store of the previous "
                "sample and before the store of the record just before the next sample");

            // ---- Precondition (b): the $38 sample shows (0,0).
            bool mut_b = (mv == "P10-B");
            Capture cap_b;
            const Capture& b_cap = mut_b ? (cap_b = p10_load("/prog_p10_mut_b.bin")) : base;
            bool have_b = b_cap.loaded && b_cap.done.has_value();
            std::optional<size_t> idx_b;
            std::vector<Rec> recs_b = have_b ? p10_decode(b_cap) : std::vector<Rec>{};
            if (have_b) { idx_b = sample_idx_on_line(recs_b, 0x38); have_b = idx_b.has_value(); }
            bool pre_b = have_b && ((recs_b[*idx_b].intreqr & 0x14) == 0);
            checks.expect(pre_b, "P10b precondition: the $38 sample shows bit 4 = 0 and bit 2 = 0");

            // ---- Precondition (c): INTENAR bits 14-0 read 0 (read/stored
            // after step 2), and the 68000 writes INTREQ nowhere after
            // step 2 (true by construction of every program here).
            bool mut_c = (mv == "P10-C");
            Capture cap_c;
            const Capture& c_cap = mut_c ? (cap_c = p10_load("/prog_p10_mut_c.bin")) : base;
            bool pre_c = c_cap.loaded && c_cap.done.has_value() && !c_cap.peeks.empty() &&
                         c_cap.peeks[0] && ((*c_cap.peeks[0]) & 0x7FFF) == 0;
            checks.expect(pre_c, "P10c precondition: stored INTENAR bits 14-0 read 0");
        }
    }

    // ==== P8: the 680x0 can poll INTREQ flags that various devices set
    // [p.35] ==== ch2-the-copper-and-the-680x0.c2,
    // COP2-the-copper-and-the-680x0.c2,.c3
    {
        constexpr uint32_t kP8CopMoveIr2 = kList1 / 2 + 3; // LIST1+6
        auto p8_load = [&](const std::string& bin, const std::vector<uint32_t>& extra) {
            std::vector<uint32_t> a = {kResults + 4, kResults + 8, kResults + 10};
            for (auto e : extra) a.push_back(e);
            return run_capture(dir + bin, kSamplerRunBudget, a);
        };
        auto p8_recs = [&](const Capture& c) {
            uint32_t count = (c.peeks.size() >= 3 && c.peeks[1] && c.peeks[2])
                                  ? (uint32_t(*c.peeks[1]) << 16) | *c.peeks[2] : 0;
            return decode_records(c.log, kRecBuf, count);
        };

        // ---- COP case -------------------------------------------------
        {
            Capture base = p8_load("/prog_p8_cop.bin", {});
            bool base_ok = base.loaded && base.done.has_value();
            checks.expect(base_ok, "P8-COP fixture: prog_p8_cop.bin reaches DONE");
            if (base_ok) {
                auto recs = p8_recs(base);
                auto t_mc = first_write_cycle(base.log, kMarkBuf + 6);
                auto t_ev = first_channel_read_at(base.log, kP8CopMoveIr2, 0);
                bool have = t_mc.has_value() && t_ev.has_value();
                checks.expect(have, "P8-COP fixture: MC and the MOVE's IR2 fetch were both found");
                if (have) {
                    auto g = grade_p8_bit(recs, *t_mc, *t_ev, 4);
                    checks.expect(g.zero_before && g.later_one && g.after_first_one,
                        "P8-COP ch2-the-copper-and-the-680x0.c2,COP2-the-copper-and-the-680x0.c2,"
                        "COP2-the-copper-and-the-680x0.c3 [p.35,p.280,p.219]: every poll record after "
                        "MC and before the Copper's MOVE executes reads bit 4 = 0, a later record "
                        "reads 1, and every record after the first 1 reads 1");
                    checks.expect(g.have_pre && g.first_after_mc_zero,
                        "P8-COPb precondition: the first poll record stored after MC reads bit 4 = 0");
                }

                // device contrast: MOVE replaced by COLOR00. Bit 4 stays 0.
                Capture nomove = p8_load("/prog_p8_cop_nomove.bin", {});
                bool nomove_ok = nomove.loaded && nomove.done.has_value();
                if (nomove_ok) {
                    auto recs_n = p8_recs(nomove);
                    auto t_mc_n = first_write_cycle(nomove.log, kMarkBuf + 6);
                    bool ok = t_mc_n.has_value();
                    if (ok) for (const auto& r : recs_n) if (r.t0 > *t_mc_n && (r.intreqr & 0x10)) ok = false;
                    checks.expect(ok, "P8-COP-DEV [p.35]: with the MOVE replaced by $0180,$0F00 "
                                       "(COLOR00), bit 4 never reads 1 after MC");
                } else {
                    checks.expect(false, "P8-COP-DEV fixture: prog_p8_cop_nomove.bin reaches DONE");
                }

                // check (iii) contrast, always run.
                Capture iii = p8_load("/prog_p8_cop_mut_iii.bin", {});
                bool iii_ok = iii.loaded && iii.done.has_value();
                if (iii_ok) {
                    auto recs_i = p8_recs(iii);
                    auto t_mc_i = first_write_cycle(iii.log, kMarkBuf + 6);
                    auto t_ev_i = first_channel_read_at(iii.log, kP8CopMoveIr2, 0);
                    bool ok = t_mc_i.has_value() && t_ev_i.has_value();
                    if (ok) {
                        auto g = grade_p8_bit(recs_i, *t_mc_i, *t_ev_i, 4);
                        ok = !g.after_first_one; // (iii) must now fail
                    }
                    checks.expect(ok,
                        "P8-COP-III [p.280]: once a poll record reads bit 4 = 1, the program clears "
                        "it again with a write to INTREQ; a later record then reads 0, and check "
                        "(iii) fails");
                } else {
                    checks.expect(false, "P8-COP-III fixture: prog_p8_cop_mut_iii.bin reaches DONE");
                }

                // precondition (a) mutation, NMH_MUTATE=P8-A.
                bool mut_a = (mv == "P8-A");
                Capture cap_a;
                const Capture& a_cap = mut_a ? (cap_a = p8_load("/prog_p8_cop_mut_a.bin", {})) : base;
                bool pre_a = a_cap.loaded && a_cap.done.has_value() && !a_cap.peeks.empty() &&
                             a_cap.peeks[0] && ((*a_cap.peeks[0]) & 0x7FFF) == 0;
                checks.expect(pre_a, "P8a precondition: stored INTENAR bits 14-0 read 0 (COP case)");

                // precondition (c): the event (the Copper's MOVE IR2
                // fetch) happens inside the window. Contrast for (c),
                // NMH_MUTATE=P8-C: DMACON=$8200 (no COPEN) -- no IR2 fetch
                // occurs, and this same check goes red.
                bool mut_c = (mv == "P8-C");
                Capture cap_c;
                bool pre_c = t_ev.has_value();
                if (mut_c) {
                    cap_c = p8_load("/prog_p8_cop_mut_c.bin", {});
                    pre_c = cap_c.loaded && cap_c.done.has_value() &&
                            first_channel_read_at(cap_c.log, kP8CopMoveIr2, 0).has_value();
                }
                checks.expect(pre_c,
                    "P8c precondition: the event (the Copper's MOVE IR2 fetch) happens inside the "
                    "window (COP case)");
            }
        }

        // ---- VB case ----------------------------------------------------
        {
            Capture base = p8_load("/prog_p8_vb.bin", {});
            bool base_ok = base.loaded && base.done.has_value();
            checks.expect(base_ok, "P8-VB fixture: prog_p8_vb.bin reaches DONE");
            if (base_ok) {
                auto recs = p8_recs(base);
                auto frames = record_frames(recs);
                auto t_mc = first_write_cycle(base.log, kMarkBuf + 6);
                // event = the frame wrap (2.4): first V>=256 -> V<=1 decrease.
                std::optional<uint64_t> t_ev;
                std::optional<size_t> wrap_a_idx, wrap_b_idx;
                for (size_t i = 1; i < recs.size(); ++i)
                    if (recs[i - 1].V >= 256 && recs[i].V <= 1) { t_ev = recs[i].t0; wrap_a_idx = i - 1; wrap_b_idx = i; break; }
                bool have = t_mc.has_value() && t_ev.has_value();
                checks.expect(have, "P8-VB fixture: MC and a frame wrap were both found");
                if (have) {
                    auto g = grade_p8_bit(recs, *t_mc, *t_ev, 5);
                    checks.expect(g.zero_before && g.later_one && g.after_first_one,
                        "P8-VB ch2-the-copper-and-the-680x0.c2,COP2-the-copper-and-the-680x0.c2,"
                        "COP2-the-copper-and-the-680x0.c3 [p.35,p.280,p.219]: every poll record after "
                        "MC and before the frame wrap reads bit 5 = 0, a later record reads 1, and "
                        "every record after the first 1 reads 1");
                    checks.expect(g.have_pre && g.first_after_mc_zero,
                        "P8-VBb precondition: the first poll record stored after MC reads bit 5 = 0");
                    bool boundary_ok = recs[*wrap_a_idx].V == 0 || recs[*wrap_a_idx].V >= 300;
                    boundary_ok = boundary_ok && (recs[*wrap_b_idx].V == 0 || recs[*wrap_b_idx].V == 1);
                    checks.note(std::string("P8-VB wrap boundary records: V=") +
                                std::to_string(recs[*wrap_a_idx].V) + " -> V=" + std::to_string(recs[*wrap_b_idx].V));
                    checks.expect(boundary_ok,
                        "P8-VB-BOUNDARY [p.219]: the frame wrap falls between a record on the last "
                        "line of the old frame or line 0, and a record on line 0 or line 1");
                }

                // device contrast: poll only V=$40..$F0 of one frame. Bit
                // 5 stays 0 (no VERTB event; this removes the event too).
                Capture narrow = p8_load("/prog_p8_vb_narrow.bin", {});
                bool narrow_ok = narrow.loaded && narrow.done.has_value();
                if (narrow_ok) {
                    auto recs_w = p8_recs(narrow);
                    auto t_mc_w = first_write_cycle(narrow.log, kMarkBuf + 6);
                    bool ok = t_mc_w.has_value();
                    if (ok) for (const auto& r : recs_w) if (r.t0 > *t_mc_w && (r.intreqr & 0x20)) ok = false;
                    checks.expect(ok, "P8-VB-DEV [p.219]: polling only from V=$40 to V=$F0, bit 5 "
                                       "never reads 1");
                } else {
                    checks.expect(false, "P8-VB-DEV fixture: prog_p8_vb_narrow.bin reaches DONE");
                }

                // precondition (b) mutation, NMH_MUTATE=P8-B: skip the
                // clear. Line 0 has passed since the prologue's own clear,
                // so bit 5 already reads 1 at the first post-MC record.
                bool mut_b = (mv == "P8-B");
                Capture cap_b;
                const Capture& b_cap = mut_b ? (cap_b = p8_load("/prog_p8_vb_mut_b.bin", {})) : base;
                bool have_b = b_cap.loaded && b_cap.done.has_value();
                bool pre_b = have_b;
                if (have_b) {
                    auto recs_b = p8_recs(b_cap);
                    auto t_mc_b = first_write_cycle(b_cap.log, kMarkBuf + 6);
                    pre_b = t_mc_b.has_value();
                    if (pre_b) {
                        auto g = grade_p8_bit(recs_b, *t_mc_b, *t_mc_b, 5);
                        pre_b = g.have_pre && g.first_after_mc_zero;
                    }
                }
                checks.expect(pre_b, "P8b precondition: the first poll record stored after MC reads "
                                      "bit 5 = 0 (VB case)");
            }
        }

        // ---- BLIT case ----------------------------------------------------
        {
            Capture base = p8_load("/prog_p8_blit.bin", {kResults + 12});
            bool base_ok = base.loaded && base.done.has_value();
            checks.expect(base_ok, "P8-BLIT fixture: prog_p8_blit.bin reaches DONE");
            if (base_ok) {
                auto recs = p8_recs(base);
                auto t_mc = first_write_cycle(base.log, kMarkBuf + 6);
                auto ws = t_mc ? writes_of_value_in(base.log, 0xFFFF, kData1, kData1 + 0x1000, *t_mc, UINT64_MAX)
                                : WriteStats{};
                bool have = t_mc.has_value() && ws.last.has_value();
                checks.expect(have, "P8-BLIT fixture: MC and at least one write of $FFFF in region D "
                                     "were both found");
                if (have) {
                    // event = the last write in region D (SPECIFICATION.md
                    // P8 Observation).
                    auto g = grade_p8_bit(recs, *t_mc, *ws.last, 6);
                    checks.expect(g.zero_before && g.later_one && g.after_first_one,
                        "P8-BLIT ch2-the-copper-and-the-680x0.c2,COP2-the-copper-and-the-680x0.c2,"
                        "COP2-the-copper-and-the-680x0.c3 [p.35,p.280,p.220,p.187]: every poll record "
                        "after MC and before the last write in region D reads bit 6 = 0, a later "
                        "record reads 1, and every record after the first 1 reads 1");
                    checks.expect(g.have_pre && g.first_after_mc_zero,
                        "P8-BLITb precondition: the first poll record stored after MC reads bit 6 = 0");
                    checks.expect(ws.count == 2048 && ws.first && *ws.first > *t_mc,
                        "P8c precondition: 2048 writes of $FFFF appear in region D, the first after "
                        "MC (BLIT case)");
                }

                // device contrast: omit BLTSIZE. Bit 6 stays 0.
                Capture nosize = p8_load("/prog_p8_blit_nosize.bin", {});
                bool nosize_ok = nosize.loaded && nosize.done.has_value();
                if (nosize_ok) {
                    auto recs_n = p8_recs(nosize);
                    auto t_mc_n = first_write_cycle(nosize.log, kMarkBuf + 6);
                    bool ok = t_mc_n.has_value();
                    if (ok) for (const auto& r : recs_n) if (r.t0 > *t_mc_n && (r.intreqr & 0x40)) ok = false;
                    checks.expect(ok, "P8-BLIT-DEV [p.35]: omitting the BLTSIZE write, bit 6 never "
                                       "reads 1");
                } else {
                    checks.expect(false, "P8-BLIT-DEV fixture: prog_p8_blit_nosize.bin reaches DONE");
                }
            }
        }
    }

    // ==== P6: beam coordinates run left to right and top to bottom on the
    // screen [p.27] (no claim id) ====
    {
        auto is_color = [](const nmh::VideoSample& s, uint8_t r, uint8_t g, uint8_t b) {
            return s.r == r && s.g == g && s.b == b;
        };
        constexpr uint32_t kP6EndIr2 = kList1 + 30; // LIST1+28/+30 = END (main shape)

        // C0=$0F00 (r=F,g=0,b=0), C1=$00F0 (r=0,g=F,b=0), C2=$000F
        // (r=0,g=0,b=F), C3=$0FF0 (r=F,g=F,b=0).
        struct P6Result {
            bool ok = false;
            std::optional<size_t> r1, r3;      // row indices in the graded frame
            std::optional<size_t> i0, i1, i2;  // leftmost sample index of C0/C1/C2 on row r1
            bool all_four = false;
            nmh::Span frame{0, 0};
            std::vector<nmh::Span> lines;      // doubler rows inside the graded frame
        };

        // Runs `bin`, finds the second complete video frame after t(MJ),
        // and measures everything P6's checks need from it.
        auto analyze_p6 = [&](const std::string& bin) -> P6Result {
            P6Result r;
            FullRun run = run_full(dir + bin, kSamplerRunBudget);
            if (!run.loaded || !run.done) return r;
            const auto& log = run.h->ram_log();
            const auto& video = run.h->video();
            auto t_mj = first_write_cycle(log, kMarkBuf + 4);
            if (!t_mj) return r;
            auto vframes = nmh::find_frames(video);
            std::optional<size_t> f1;
            for (size_t i = 0; i < vframes.size(); ++i)
                if (vframes[i].begin > *t_mj) { f1 = i; break; }
            if (!f1 || *f1 + 1 >= vframes.size()) return r;
            r.frame = vframes[*f1 + 1];
            auto lines = nmh::find_lines(video);
            for (auto& ln : lines)
                if (ln.begin >= r.frame.begin && ln.end <= r.frame.end) r.lines.push_back(ln);

            auto first_row_with = [&](uint8_t cr, uint8_t cg, uint8_t cb) -> std::optional<size_t> {
                for (size_t i = 0; i < r.lines.size(); ++i)
                    for (size_t j = r.lines[i].begin; j < r.lines[i].end; ++j)
                        if (is_color(video[j], cr, cg, cb)) return i;
                return std::nullopt;
            };
            r.r1 = first_row_with(0x0, 0xF, 0x0); // first row with C1
            r.r3 = first_row_with(0xF, 0xF, 0x0); // first row with C3
            if (r.r1) {
                nmh::Span row = r.lines[*r.r1];
                for (size_t j = row.begin; j < row.end; ++j) {
                    if (!r.i0 && is_color(video[j], 0xF, 0x0, 0x0)) r.i0 = j;
                    if (!r.i1 && is_color(video[j], 0x0, 0xF, 0x0)) r.i1 = j;
                    if (!r.i2 && is_color(video[j], 0x0, 0x0, 0xF)) r.i2 = j;
                }
            }
            bool c0 = false, c1 = false, c2 = false, c3 = false;
            for (size_t j = r.frame.begin; j < r.frame.end; ++j) {
                if (is_color(video[j], 0xF, 0x0, 0x0)) c0 = true;
                if (is_color(video[j], 0x0, 0xF, 0x0)) c1 = true;
                if (is_color(video[j], 0x0, 0x0, 0xF)) c2 = true;
                if (is_color(video[j], 0xF, 0xF, 0x0)) c3 = true;
            }
            r.all_four = c0 && c1 && c2 && c3;
            r.ok = true;
            return r;
        };

        P6Result base = analyze_p6("/prog_p6_main.bin");
        checks.expect(base.ok, "P6 fixture: prog_p6_main.bin reaches DONE and yields a second "
                                "complete video frame after the enable");
        if (base.ok) {
            bool have_rows = base.r1.has_value() && base.r3.has_value();
            checks.expect(have_rows, "P6 fixture: row r1 (first with C1) and row r3 (first with "
                                      "C3) were both found");
            if (have_rows) {
                checks.expect(*base.r3 > *base.r1,
                    "P6 [p.27]: row r3 (first row with C3) is below row r1 (first row with C1)");
                bool order_ok = base.i0 && base.i1 && base.i2 && *base.i0 < *base.i1 && *base.i1 < *base.i2;
                checks.expect(order_ok,
                    "P6 [p.27]: on row r1, reading left to right, a C0 (red) run comes first, then "
                    "the C1 (green) run, then a C2 (blue) run");
            }

            // ---- Contrast (i): WAIT2 $60A1 -> $60C1. The right end of
            // the C1 run on row r1 (i.e. the start of C2, i2) moves to a
            // larger column.
            P6Result wc1 = analyze_p6("/prog_p6_wait2_c1.bin");
            bool wc1_ok = wc1.ok && wc1.i2.has_value() && base.i2.has_value() && *wc1.i2 > *base.i2;
            checks.expect(wc1_ok,
                "P6-i [p.27]: with the second WAIT changed from $60A1 to $60C1, the C2 run's own "
                "start on row r1 moves to a larger column than in the main run");

            // ---- Contrast (ii): WAIT3 $A061 -> $B061. r3 increases.
            P6Result wb0 = analyze_p6("/prog_p6_wait3_b0.bin");
            bool wb0_ok = wb0.ok && wb0.r3.has_value() && base.r3.has_value() && *wb0.r3 > *base.r3;
            checks.expect(wb0_ok,
                "P6-ii [p.27]: with the third WAIT changed from $A061 to $B061, row r3 (first row "
                "with C3) moves to a larger row index than in the main run");

            // ---- Precondition (a): all four colours appear.
            bool mut_a = (mv == "P6-A");
            P6Result a_res = mut_a ? analyze_p6("/prog_p6_mut_a.bin") : base;
            checks.expect(a_res.ok && a_res.all_four,
                "P6a precondition: all four colours (C0-C3) appear in the graded frame");

            // ---- Precondition (b): at least three restarts follow t(MJ),
            // and between each two consecutive restarts each of the four
            // MOVEs is executed exactly once, in list order.
            bool mut_b = (mv == "P6-B");
            Capture cap_b = run_capture(dir + (mut_b ? "/prog_p6_mut_b.bin" : "/prog_p6_main.bin"),
                                         kSamplerRunBudget, {});
            auto t_mj_b = first_write_cycle(cap_b.log, kMarkBuf + 4);
            bool pre_b = cap_b.loaded && cap_b.done.has_value() && t_mj_b.has_value();
            if (pre_b) {
                auto restarts = find_restarts(cap_b.log, {kP6EndIr2});
                std::vector<Restart> after;
                for (auto& r : restarts) if (r.t_restart > *t_mj_b) after.push_back(r);
                pre_b = after.size() >= 3;
                const uint32_t move_ir2[4] = {
                    kList1 / 2 + 1, kList1 / 2 + 5, kList1 / 2 + 9, kList1 / 2 + 13,
                };
                for (size_t i = 0; i + 1 < after.size() && pre_b; ++i) {
                    uint64_t lo = after[i].t_restart, hi = after[i + 1].t_final;
                    uint64_t last = lo;
                    for (int k = 0; k < 4 && pre_b; ++k) {
                        int n = count_channel_reads_at(cap_b.log, move_ir2[k], lo, hi);
                        if (n != 1) pre_b = false;
                        auto t = first_channel_read_at(cap_b.log, move_ir2[k], last);
                        if (!t || *t >= hi) pre_b = false; else last = *t;
                    }
                }
            }
            checks.expect(pre_b,
                "P6b precondition: at least three restarts follow t(MJ), and between each two "
                "consecutive restarts each of the four MOVEs is executed exactly once, in list "
                "order");

            // ---- Precondition (c): V1. Compared only over frames after
            // t(MJ): find_frames() include the leading, irregular-length
            // partial span from simulation start to the first vsync edge
            // (and several frames pass during boot(), before the CPU is
            // released), so the comparison basis has to start after the
            // run's own enable, the same anchor the main check already
            // uses.
            bool mut_c = (mv == "P6-C");
            FullRun run_c = run_full(dir + (mut_c ? "/prog_p6_mut_c.bin" : "/prog_p6_main.bin"),
                                      kSamplerRunBudget);
            bool pre_c = run_c.loaded && run_c.done;
            if (pre_c) {
                auto t_mj_vc = first_write_cycle(run_c.h->ram_log(), kMarkBuf + 4);
                auto vf = nmh::find_frames(run_c.h->video());
                std::optional<size_t> vc0;
                if (t_mj_vc) for (size_t i = 0; i < vf.size(); ++i) if (vf[i].begin > *t_mj_vc) { vc0 = i; break; }
                pre_c = vc0 && *vc0 + 3 < vf.size();
                if (pre_c) pre_c = anchor_V1(run_c.h->video(),
                    {vf[*vc0], vf[*vc0 + 1], vf[*vc0 + 2], vf[*vc0 + 3]});
            }
            checks.expect(pre_c, "P6c precondition: V1 holds (every one of four video frames after "
                                  "the enable has the same clk_sys sample count and the same doubler "
                                  "row count)");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
