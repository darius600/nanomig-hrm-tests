// test_ch2_copper_move.cpp -- ch2-copper-move. See SPECIFICATION.md for
// every proposition, its stimulus, its observation and its expected value.
// This file only implements what SPECIFICATION.md already committed to.
//
// NMH_MUTATE selects a mutated program (or a mutated grading path) in
// place of its normal counterpart, one assertion at a time; none is
// exercised by a plain `make test`.
//
// MUTATION P4-p1 stimulus red one extra word write lands between markers Ma and Mb, via NMH_MUTATE=P4-P1
// MUTATION P4-p2 stimulus red the clearing loop runs after Mb instead of before Ma, via NMH_MUTATE=P4-P2
// MUTATION P4-W1,P4-W2,P4-W3,P4-W4,P4-W5,P4-W6-D0,P4-W6-D1,P4-W7 stimulus red D0 and D1's halves exchanged, and W1's own immediate written pre-swapped, via NMH_MUTATE=P4-SWAP
// MUTATION P3-p1,P3-i,P3-iii stimulus red the WAIT is line 0 (satisfied at once); (i) and (iii) are the
//   page's own consequence of an instantly-satisfied WAIT, not a separate fault, via NMH_MUTATE=P3-P1
// MUTATION P3-p2 stimulus red the clearing MOVE is removed (list starts with the WAIT), via NMH_MUTATE=P3-P2
// MUTATION P3-ii stimulus red (C1) the WAIT is line $A0 (160) instead of $64, via NMH_MUTATE=P3-C1
// MUTATION P3-fixture not exercised (C2) the WAIT is replaced by a park (never satisfied), so neither a
//   WAIT-shaped nor a set-MOVE fetch ever occurs; P3-ii and P3-iv are recorded, not graded, via NMH_MUTATE=P3-C2
// MUTATION G1 stimulus red (demonstrated once, on P3) the no-COPEN baseline also executes one CPU read inside the Copper region, via NMH_MUTATE=P3-G1
// MUTATION G2 stimulus red (demonstrated on P3's own G1 baseline, prog_p3_nocopen.bin: DMACON started $8200) bit 7 of DMACONR reads 0
// MUTATION G3 stimulus red (demonstrated once, on P3) COP1LCH is loaded with the decoy park D instead of L_3, via NMH_MUTATE=P3-G3
// MUTATION G4 stimulus red (demonstrated once, on P3) the set-MOVE's data word diverges from $8004, via NMH_MUTATE=P3-G4
// MUTATION G5 not exercised (demonstrated once, on P3) the same program run with NTSC selected, checked
//   with the PAL rule, via a second Config on the normal binary: this boundary's own mean line length stays
//   227.0 under NTSC selection too (both cores), so the page's own 227/228 alternation is not reproduced here
// MUTATION G6 stimulus red (demonstrated once, on P3) the monitor loop gets extra delay per iteration, via
//   NMH_MUTATE=P3-G6; on TG68K the same reduced sampling also carries away I4 and G5, a natural consequence
//   of too few samples per line for either to measure anything, not a separate fault
// MUTATION P1-p1 stimulus red half-step a_5's WAIT is replaced by a park, via NMH_MUTATE=P1-P1
// MUTATION P1-p2 stimulus red the $D555 write is omitted, via NMH_MUTATE=P1-P2
// MUTATION P1-set,P1-bit0 stimulus red (C1) b_0's IR1 is $009C (INTREQ) instead of INTENA, via NMH_MUTATE=P1-C1
// MUTATION P1-set,P1-bit0 stimulus red (C2) b_0's IR2 is $8002 instead of $8001, via NMH_MUTATE=P1-C2
// MUTATION P1-clear,P1-bit0 stimulus red b_0's IR2 is $0002 instead of $0001 in the clear list, via NMH_MUTATE=P1-CC1
//
// P1-B (the COP2LC 40-target address chain) and P2, P5, P6 and P7 are not
// built in this package.

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <functional>
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

constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kDoneRunBudget = 3'000'000;
// Decoy park D ($0E8000-$0E8003, SPECIFICATION.md 4.2), written by every
// program in this package so any Copper landing there (G3's own mutation)
// reads a well-formed, never-satisfied WAIT rather than uninitialized RAM.
constexpr uint32_t kDecoyD = 0x0E8000;

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

struct Capture {
    bool loaded = false;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
};

Capture run_capture(const std::string& bin, uint64_t run_budget, nmh::Config cfg = nmh::Config{}) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    return c;
}

// Every write in the log at word address `w`, in cycle order.
std::vector<uint64_t> write_cycles(const std::vector<nmh::RamAccess>& log, uint32_t w) {
    std::vector<uint64_t> out;
    for (const auto& a : log) if (a.is_write && a.word_addr == w) out.push_back(a.cycle);
    return out;
}

// Every write in the log whose word address lies in [lo,hi), in cycle order.
std::vector<nmh::RamAccess> writes_in_range(const std::vector<nmh::RamAccess>& log, uint32_t lo, uint32_t hi) {
    std::vector<nmh::RamAccess> out;
    for (const auto& a : log) if (a.is_write && a.word_addr >= lo && a.word_addr < hi) out.push_back(a);
    return out;
}

// The data of the first write at word address `w` after cycle `after`
// (strict), or nullopt.
std::optional<uint16_t> data_after(const std::vector<nmh::RamAccess>& log, uint32_t w, uint64_t after) {
    for (const auto& a : log) if (a.is_write && a.word_addr == w && a.cycle > after) return a.data;
    return std::nullopt;
}

// Whether some write at word address `w`, before cycle `before` (strict),
// carries the data `want`.
bool wrote_before(const std::vector<nmh::RamAccess>& log, uint32_t w, uint64_t before, uint16_t want) {
    for (const auto& a : log) if (a.is_write && a.word_addr == w && a.cycle < before && a.data == want) return true;
    return false;
}

} // namespace

// ---------------------------------------------------------------------
// Shared instruments (SPECIFICATION.md 4.4): I1 (Copper instruction
// fetches), I2 (the monitor loop's beam/INTENAR samples) and I4 (the
// clk_sys-per-memory-cycle ratio), used by every Copper-driving proposition
// below (P1-A, P3).
namespace instr {

// p.23: "In PAL, they are all 227 long" (227 colour clocks = memory
// cycles per line).
constexpr uint64_t kPalLineCck = 227;

// One I2 sample: INTENAR, VPOSR, VHPOSR, in that order, and the port cycle
// of the INTENAR store (I2: "Its time s is the port time of the INTENAR
// store").
struct Sample {
    uint64_t s;
    uint16_t intenar, vposr, vhposr;
    int V() const { return 256 * (vposr & 1) + ((vhposr >> 8) & 0xFF); }
    int H() const { return vhposr & 0xFF; }
};

// Parses every I2 sample from the writes the monitor loop made into
// [buf_lo_word, buf_hi_word): three consecutive words per sample, in the
// order the loop wrote them (INTENAR, VPOSR, VHPOSR), which is also their
// cycle order.
std::vector<Sample> parse_samples(const std::vector<nmh::RamAccess>& log,
                                   uint32_t buf_lo_word, uint32_t buf_hi_word) {
    std::vector<uint16_t> data;
    std::vector<uint64_t> cyc;
    for (const auto& a : log) {
        if (a.is_write && a.word_addr >= buf_lo_word && a.word_addr < buf_hi_word) {
            data.push_back(a.data);
            cyc.push_back(a.cycle);
        }
    }
    std::vector<Sample> out;
    for (size_t i = 0; i + 2 < data.size(); i += 3)
        out.push_back({cyc[i], data[i], data[i + 1], data[i + 2]});
    return out;
}

// Discards samples whose V differs by more than 1 from both neighbours
// (torn reads at the V8 boundary, I2).
std::vector<Sample> discard_torn(const std::vector<Sample>& in) {
    std::vector<Sample> out;
    for (size_t i = 0; i < in.size(); ++i) {
        bool torn = false;
        if (i > 0 && i + 1 < in.size()) {
            int dv_prev = std::abs(in[i].V() - in[i - 1].V());
            int dv_next = std::abs(in[i].V() - in[i + 1].V());
            if (dv_prev > 1 && dv_next > 1) torn = true;
        }
        if (!torn) out.push_back(in[i]);
    }
    return out;
}

struct Span { size_t begin, end; }; // [begin,end) indices into a Sample vector: one field

// A wrap is where V falls by more than 200 between consecutive kept
// samples (I2). Splits into fields at every wrap.
std::vector<Span> find_fields(const std::vector<Sample>& s) {
    std::vector<Span> out;
    size_t start = 0;
    for (size_t i = 1; i < s.size(); ++i) {
        if (s[i - 1].V() - s[i].V() > 200) { out.push_back({start, i}); start = i; }
    }
    if (start < s.size()) out.push_back({start, s.size()});
    return out;
}

// The indices, within one field, of every sample that starts a new line:
// the first sample of each new V that persists into the following sample
// (a single-sample blip that immediately reverts is a torn read at the V8
// boundary that discard_torn's neighbour rule did not catch). Shared by
// G5 (line spacing) and G6 (samples per line), so both measure the same
// lines.
std::vector<size_t> line_start_indices(const std::vector<Sample>& samples, const Span& fld) {
    std::vector<size_t> out;
    int last_v = -1;
    for (size_t i = fld.begin; i < fld.end; ++i) {
        if (samples[i].V() == last_v) continue;
        bool persists = (i + 1 >= fld.end) || samples[i + 1].V() == samples[i].V();
        if (!persists) continue;
        out.push_back(i);
        last_v = samples[i].V();
    }
    return out;
}

bool in_regions(uint32_t w, const std::vector<std::pair<uint32_t, uint32_t>>& regions) {
    for (const auto& r : regions) if (w >= r.first && w < r.second) return true;
    return false;
}

// I1: a Copper read is a read at the port whose address lies in a Copper
// region.
std::vector<nmh::RamAccess> copper_reads(const std::vector<nmh::RamAccess>& log,
                                          const std::vector<std::pair<uint32_t, uint32_t>>& regions) {
    std::vector<nmh::RamAccess> out;
    for (const auto& a : log) if (!a.is_write && in_regions(a.word_addr, regions)) out.push_back(a);
    return out;
}

// I1: an instruction fetch at X is a Copper read at X followed, as the
// next Copper read, by a read at X+2. `reads` must already be in cycle
// order (true of copper_reads()'s output, since ram_log() is itself
// cycle-ordered).
struct Fetch { uint32_t byte_addr; uint64_t t1, t2; uint16_t ir1, ir2; };

struct FetchResult {
    std::vector<Fetch> fetches;
    std::vector<nmh::RamAccess> unpaired;
};

FetchResult pair_fetches(const std::vector<nmh::RamAccess>& reads) {
    FetchResult r;
    size_t i = 0;
    while (i < reads.size()) {
        if (i + 1 < reads.size() && reads[i + 1].word_addr == reads[i].word_addr + 1) {
            r.fetches.push_back({reads[i].word_addr * 2, reads[i].cycle, reads[i + 1].cycle,
                                  reads[i].data, reads[i + 1].data});
            i += 2;
        } else {
            r.unpaired.push_back(reads[i]);
            i += 1;
        }
    }
    return r;
}

const Fetch* find_by_addr(const std::vector<Fetch>& v, uint32_t addr) {
    for (const auto& f : v) if (f.byte_addr == addr) return &f;
    return nullptr;
}

const Fetch* find_if(const std::vector<Fetch>& v, const std::function<bool(const Fetch&)>& pred) {
    for (const auto& f : v) if (pred(f)) return &f;
    return nullptr;
}

// A list execution: every fetch from one occurrence of `start_addr` (the
// list's own first word, i.e. the Copper's restart point) up to, but not
// including, the next. A fetch belongs to whichever execution's own start
// it follows -- defined this way (by the restart fetch itself, not by any
// sample) so that a restart which happens to precede that field's own next
// I2 sample never gets attributed to the wrong execution.
struct Exec { std::vector<Fetch> fetches; };

std::vector<Exec> split_executions(const std::vector<Fetch>& all, uint32_t start_addr) {
    std::vector<Exec> out;
    for (const auto& f : all) {
        if (f.byte_addr == start_addr) out.push_back(Exec{});
        if (!out.empty()) out.back().fetches.push_back(f);
    }
    return out;
}

// G4: every Copper read's data equals the word this specification
// prescribes for that address (SPECIFICATION.md 4.5), judged against the
// reference table of the run actually under test -- a contrasting case's
// own words, never the main run's (specification 4.5's "prescribed" rule).
bool check_g4(const std::vector<Fetch>& fetches, const std::vector<std::pair<uint32_t, uint16_t>>& ref) {
    for (const auto& f : fetches) {
        for (const auto& kv : ref) {
            if (kv.first == f.byte_addr && f.ir1 != kv.second) return false;
            if (kv.first == f.byte_addr + 2 && f.ir2 != kv.second) return false;
        }
    }
    return true;
}

// I4: the clk_sys-per-memory-cycle ratio, measured from the capture
// itself (p.24: "One horizontal count takes one cycle of the system
// clock"), never assumed. For every run of two or more same-V samples
// (a "line"), the ratio is (last sample's cycle - first sample's cycle) /
// (last sample's H - first sample's H), rounded to the nearest integer.
// A single sample's own INTENAR-to-VHPOSR gap can vary by a memory cycle
// or so (I2: they are two separate register reads within the same loop
// iteration), but that gap appears once at each end of a multi-sample
// line, so it shrinks to a small fraction of the whole line's own ratio.
// I4 passes only if every line's own ratio rounds to the same integer
// (spec 4.4: "asserts that the ratio is constant over the capture").
struct Ratio { bool ok = false; uint64_t kCck = 0; int lines = 0; int off = 0; };

Ratio measure_ratio(const std::vector<Sample>& samples, const std::vector<Span>& fields) {
    Ratio r;
    std::vector<uint64_t> per_line;
    for (const auto& fld : fields) {
        size_t start = fld.begin;
        for (size_t i = fld.begin + 1; i <= fld.end; ++i) {
            bool boundary = (i == fld.end) || samples[i].V() != samples[start].V();
            if (!boundary) continue;
            if (i - 1 > start) {
                int dh = samples[i - 1].H() - samples[start].H();
                if (dh > 0) {
                    uint64_t dc = samples[i - 1].s - samples[start].s;
                    double ratio = (double)dc / (double)dh;
                    per_line.push_back((uint64_t)(ratio + 0.5));
                }
            }
            start = i;
        }
    }
    if (per_line.empty()) return r;
    std::sort(per_line.begin(), per_line.end());
    uint64_t best = per_line[0]; size_t best_n = 0;
    size_t i = 0;
    while (i < per_line.size()) {
        size_t j = i;
        while (j < per_line.size() && per_line[j] == per_line[i]) ++j;
        if (j - i > best_n) { best_n = j - i; best = per_line[i]; }
        i = j;
    }
    r.kCck = best;
    r.lines = (int)per_line.size();
    r.off = (int)(per_line.size() - best_n);
    r.ok = (r.off == 0);
    return r;
}

// G5: the monitor's beam readings are sane (PAL): H in $00-$E2, V
// non-decreasing per field, successive line starts 227 +/- tolerance
// memory cycles apart on average (p.23). Line starts are the first
// sample of each new V that persists into the following sample (a
// single-sample blip that immediately reverts is a torn read at the V8
// boundary that discard_torn's neighbour rule did not catch). A field's
// own first line is excluded: the field boundary is a beam wrap, not a
// sample, so the first detected "line start" can be mid-line.
//
// A sample's own H reading says directly how far past the true line
// boundary it was taken (I2: the beam values are read shortly after s,
// in program order); over a whole line's own samples, the smallest
// (cycle - kCck*H) is the least lag-corrupted estimate of the true
// boundary (jitter only ever adds to a read's own time, never subtracts,
// so it is a lower bound approached, not a random walk around it). Per
// pair this still carries the residual jitter of a single sample's own
// three separate register reads (I2: INTENAR, then VPOSR, then VHPOSR)
// and the rare torn read at the V/H boundary the neighbour rule does not
// reach either -- a gross bound (40 memory cycles) catches only a
// genuinely skipped line or lost sync. The 227-cycle line length itself
// is checked on the *mean* of every corrected gap in the capture, which
// only random, symmetric jitter cannot move by more than a small
// fraction of one memory cycle: tight enough that NTSC's own 227/228
// alternation (mean 227.5) or a wrong ratio of 7 or 9 (mean 259 or 202)
// both fail outright.
constexpr int64_t kGrossLineTolerance = 40;
constexpr double kMeanLineTolerance = 0.3;

struct G5Result {
    bool ok = true;
    int h_violations = 0, v_violations = 0, gap_violations = 0;
    int diag_h = -1; int64_t diag_gap = 0; double mean_line = 0.0; int pairs = 0;
};

G5Result check_g5(const std::vector<Sample>& samples, const std::vector<Span>& fields, uint64_t kCck) {
    G5Result g;
    if (kCck == 0) { g.ok = false; return g; }

    double sum = 0.0;
    for (const auto& fld : fields) {
        for (size_t i = fld.begin; i < fld.end; ++i) {
            if (samples[i].H() > 0xE2) { g.ok = false; ++g.h_violations; if (g.diag_h < 0) g.diag_h = samples[i].H(); }
            if (i > fld.begin && samples[i].V() < samples[i - 1].V()) { g.ok = false; ++g.v_violations; }
        }
        auto idxs = line_start_indices(samples, fld);
        std::vector<int64_t> anchor(idxs.size());
        for (size_t k = 0; k < idxs.size(); ++k) {
            size_t end = (k + 1 < idxs.size()) ? idxs[k + 1] : fld.end;
            int64_t best = INT64_MAX;
            for (size_t i = idxs[k]; i < end; ++i) {
                int64_t v = (int64_t)samples[i].s - (int64_t)kCck * (int64_t)samples[i].H();
                if (v < best) best = v;
            }
            anchor[k] = best;
        }
        // anchor[0] is the field's own first, possibly-partial line.
        for (size_t k = 2; k < anchor.size(); ++k) {
            double gap_mem = (double)(anchor[k] - anchor[k - 1]) / (double)kCck;
            sum += gap_mem;
            ++g.pairs;
            int64_t delta = (int64_t)(gap_mem - (double)kPalLineCck);
            if (delta < 0) delta = -delta;
            if (delta > kGrossLineTolerance) {
                g.ok = false; ++g.gap_violations;
                if (!g.diag_gap) g.diag_gap = (int64_t)gap_mem;
            }
        }
    }
    if (g.pairs > 0) {
        g.mean_line = sum / (double)g.pairs;
        double dev = g.mean_line - (double)kPalLineCck;
        if (dev < 0) dev = -dev;
        if (dev > kMeanLineTolerance) g.ok = false;
    } else {
        g.ok = false;
    }
    return g;
}

// G6: every interval that a check counts holds at least 5 kept samples
// (SPECIFICATION.md 4.5). The finest interval any check in this package
// counts is one line (G5's own unit); checked over every full line but a
// field's own first, possibly-partial one, matching G5's own line spans.
struct G6Result { bool ok = true; int min_count = -1; };

G6Result check_g6(const std::vector<Sample>& samples, const std::vector<Span>& fields) {
    G6Result g;
    for (const auto& fld : fields) {
        auto idxs = line_start_indices(samples, fld);
        for (size_t i = 1; i + 1 < idxs.size(); ++i) {
            int n = (int)(idxs[i + 1] - idxs[i]);
            if (g.min_count < 0 || n < g.min_count) g.min_count = n;
            if (n < 5) g.ok = false;
        }
    }
    if (g.min_count < 0) g.ok = false;
    return g;
}

} // namespace instr

// ---------------------------------------------------------------------
// P4 [p25]: a 680x0 long-word write puts the most significant word at the
// lower address. See prog_p4.s.
namespace p4 {

constexpr uint32_t kScr    = 0x0E6000;
constexpr uint32_t kMarkA  = 0x0E7000;
constexpr uint32_t kMarkB  = 0x0E7002;
constexpr uint64_t kBudget = kBootBudget + kDoneRunBudget;

struct LongCheck { uint32_t x; uint16_t hi; uint16_t lo; const char* id; const char* name; };

void run(Checks& checks, const std::string& dir, const std::string& mutate) {
    std::string bin = dir + "/prog_p4.bin";
    if (mutate == "P4-SWAP") bin = dir + "/prog_p4_mut_swap.bin";
    else if (mutate == "P4-P1") bin = dir + "/prog_p4_mut_extra.bin";
    else if (mutate == "P4-P2") bin = dir + "/prog_p4_mut_clearlate.bin";
    Capture c = run_capture(bin, kBudget);
    bool ran = c.loaded && c.done.has_value();
    checks.expect(ran, "P4-fixture: prog_p4.bin ran to DONE");
    if (!ran) return;

    auto ma = write_cycles(c.log, word_addr(kMarkA));
    auto mb = write_cycles(c.log, word_addr(kMarkB));
    bool markers_ok = ma.size() == 1 && mb.size() == 1 && ma[0] < mb[0];
    checks.expect(markers_ok, "P4-fixture: markers Ma and Mb each written exactly once, Ma before Mb");
    if (!markers_ok) return;
    const uint64_t t_ma = ma[0], t_mb = mb[0];

    // The unchanged comparator: every form is graded against the same
    // main-run expectation, D0 = $A5C33C5A / D1 = $0F1EE1F0 / W1's own
    // immediate = $A5C33C5A, regardless of NMH_MUTATE. Under P4-SWAP the
    // program actually writes D0, D1 and W1's own immediate pre-exchanged
    // (SPECIFICATION.md P4.6), so every one of the eight forms goes red
    // through this same, unmoved comparator.
    const uint16_t hi = 0xA5C3;
    const uint16_t lo = 0x3C5A;
    const std::vector<LongCheck> forms = {
        {kScr + 0x00, 0xA5C3, 0x3C5A, "P4-W1", "W1 MOVE.L #imm,(A0) at $0E6000"},
        {kScr + 0x10, hi,     lo,     "P4-W2", "W2 MOVE.L D0,(A1)+ at $0E6010"},
        {kScr + 0x20, hi,     lo,     "P4-W3", "W3 MOVE.L D0,-(A2) at $0E6020"},
        {kScr + 0x30, hi,     lo,     "P4-W4", "W4 MOVE.L D0,8(A3) at $0E6030"},
        {kScr + 0x40, hi,     lo,     "P4-W5", "W5 MOVE.L D0,abs.L at $0E6040"},
        {kScr + 0x50, hi,     lo,     "P4-W6-D0", "W6 MOVEM.L D0/D1,(A4), D0 at $0E6050"},
        {kScr + 0x54, 0x0F1E, 0xE1F0, "P4-W6-D1", "W6 MOVEM.L D0/D1,(A4), D1 at $0E6054"},
        {kScr + 0x62, hi,     lo,     "P4-W7", "W7 MOVE.L D0,(A5) at $0E6062 (addr = 2 mod 4)"},
    };

    for (const auto& f : forms) {
        auto hi_w = data_after(c.log, word_addr(f.x), t_ma);
        auto lo_w = data_after(c.log, word_addr(f.x) + 1, t_ma);
        bool ok = hi_w && lo_w && *hi_w == f.hi && *lo_w == f.lo;
        checks.expect(ok,
            std::string(f.id) + " [p25] ch2-location-registers.c11,COP2-location-registers.c13: " +
            f.name + " -- most significant word at the lower address");
    }

    // P4-p1 precondition: exactly the 16 word writes above land strictly
    // between Ma and Mb, and nothing else.
    auto between = writes_in_range(c.log, 0, 0x200000);
    size_t n_between = 0;
    bool only_expected = true;
    for (const auto& a : between) {
        if (a.cycle <= t_ma || a.cycle >= t_mb) continue;
        ++n_between;
        bool expected_addr = false;
        for (const auto& f : forms) {
            uint32_t w = word_addr(f.x);
            if (a.word_addr == w || a.word_addr == w + 1) { expected_addr = true; break; }
        }
        if (!expected_addr) only_expected = false;
    }
    bool p4p1 = only_expected && n_between == 16;
    checks.expect(p4p1,
        "P4-p1 precondition: between Ma and Mb, exactly two word writes per long word listed (16 total), nothing else");

    // P4-p2 precondition: the clearing writes to $0000 precede Ma, and
    // they actually carry the data $0000 (not merely some write, of any
    // value, before Ma).
    bool cleared_before = true;
    for (uint32_t off = 0; off < 0x80; off += 2)
        if (!wrote_before(c.log, word_addr(kScr + off), t_ma, 0x0000)) { cleared_before = false; break; }
    checks.expect(cleared_before, "P4-p2 precondition: the clearing writes to $0000 precede marker Ma and carry the data $0000");

    checks.record("ch2-location-registers.c11 and COP2-location-registers.c13 ('when you write long words with "
                  "the 680x0') are graded for the 68000 only here; a 68020 run of W1-W7 is not built.");
}

} // namespace p4

// ---------------------------------------------------------------------
// P3 [p19]: a MOVE after a WAIT is performed once the WAIT is satisfied,
// and not before. See prog_p3.s. Also demonstrates the common instruments
// G1-G6 (SPECIFICATION.md 4.5) and their own mutations; P1-A reuses the
// same, unchanged instrument code and restates every one of G1-G6 on its
// own captures without repeating their dedicated red mutations here -- a
// scope decision, since G1-G6 are the shared measuring apparatus.
namespace p3 {

constexpr uint32_t kL3        = 0x0E2000;
constexpr uint32_t kMarkM0    = 0x0E7000;
constexpr uint32_t kMarkFinal = 0x0E7002;
constexpr uint32_t kDmaconrSave = 0x0E7100;
constexpr uint32_t kLogLo = 0x110000, kLogHi = 0x140000;
// G6's own mutation adds delay to every monitor-loop iteration, so this
// run's own budget carries headroom beyond the plain 30000-iteration cost.
constexpr uint64_t kBudget = kBootBudget + 60'000'000;
// nanomig.i's INTENA equate ($09A), restated for the C++ side (that header
// is assembly-only).
constexpr uint16_t kIntenaAddr = 0x009A;

const std::vector<std::pair<uint32_t, uint32_t>> kRegions = {
    {word_addr(kL3), word_addr(kL3) + 8},
    {word_addr(kDecoyD), word_addr(kDecoyD) + 2},
};

// G4's own reference table for a given variant: the list content that
// variant's own program actually assembles (specification 4.5: a
// contrasting case is judged by its own words). P3-G4 is the one
// exception -- its whole point is that the actual data diverges from
// what this specification prescribes, so it stays graded against the
// NORMAL table.
std::vector<std::pair<uint32_t, uint16_t>> reference_words(const std::string& mutate) {
    std::vector<std::pair<uint32_t, uint16_t>> out;
    uint32_t addr = kL3;
    if (mutate != "P3-P2") {
        out.push_back({addr, kIntenaAddr}); out.push_back({addr + 2, 0x7FFF});
        addr += 4;
    }
    if (mutate == "P3-C2") {
        out.push_back({addr, 0xFFFF}); out.push_back({addr + 2, 0xFFFE});
    } else if (mutate == "P3-P1") {
        out.push_back({addr, 0x0001}); out.push_back({addr + 2, 0xFF00});
    } else if (mutate == "P3-C1") {
        out.push_back({addr, 0xA001}); out.push_back({addr + 2, 0xFF00});
    } else {
        out.push_back({addr, 0x6401}); out.push_back({addr + 2, 0xFF00});
    }
    addr += 4;
    out.push_back({addr, kIntenaAddr}); out.push_back({addr + 2, 0x8004});
    addr += 4;
    out.push_back({addr, 0xFFFF}); out.push_back({addr + 2, 0xFFFE});
    // The decoy park D, common to every variant.
    out.push_back({kDecoyD, 0xFFFF}); out.push_back({kDecoyD + 2, 0xFFFE});
    return out;
}

void run(Checks& checks, const std::string& dir, const std::string& mutate) {
    std::string bin = dir + "/prog_p3.bin";
    if (mutate == "P3-P1") bin = dir + "/prog_p3_mut_wait0.bin";
    else if (mutate == "P3-P2") bin = dir + "/prog_p3_mut_noclear.bin";
    else if (mutate == "P3-C1") bin = dir + "/prog_p3_c1.bin";
    else if (mutate == "P3-C2") bin = dir + "/prog_p3_c2.bin";
    else if (mutate == "P3-G3") bin = dir + "/prog_p3_g3.bin";
    else if (mutate == "P3-G4") bin = dir + "/prog_p3_g4.bin";
    else if (mutate == "P3-G6") bin = dir + "/prog_p3_g6.bin";

    nmh::Config cfg;
    if (mutate == "P3-G5") cfg.chipset_config = 0x0A; // ECS/NTSC (G5's own contrasting stimulus)

    Capture c = run_capture(bin, kBudget, cfg);
    bool ran = c.loaded && c.done.has_value();
    checks.expect(ran, "P3-fixture: the program ran to DONE");
    if (!ran) return;

    // ---- I2 samples, fields --------------------------------------------
    auto raw = instr::parse_samples(c.log, word_addr(kLogLo), word_addr(kLogHi));
    auto samples = instr::discard_torn(raw);
    auto fields = instr::find_fields(samples);
    checks.note("P3: " + std::to_string(samples.size()) + " kept samples, " +
                std::to_string(fields.size()) + " fields");

    // ---- I1 fetches, list executions ------------------------------------
    auto reads = instr::copper_reads(c.log, kRegions);
    auto fr = instr::pair_fetches(reads);
    auto execs = instr::split_executions(fr.fetches, kL3);

    // ---- I4: the clk_sys-per-memory-cycle ratio -------------------------
    auto ratio = instr::measure_ratio(samples, fields);
    checks.expect(ratio.ok,
        "I4 precondition (P3) [p24] ch2-the-wait-instruction: the clk_sys-per-memory-cycle ratio, "
        "measured per line from its own first-to-last sample, is the same integer over the whole capture");
    checks.note("I4 (P3): kCck=" + std::to_string(ratio.kCck) + " (" + std::to_string(ratio.lines - ratio.off) +
                "/" + std::to_string(ratio.lines) + " lines agree)");
    uint64_t kCck = ratio.ok ? ratio.kCck : 0;

    // ---- G1: Copper reads are identifiable (baseline: DMACON $8200) ----
    Capture base;
    {
        std::string base_bin = dir + (mutate == "P3-G1" ? "/prog_p3_nocopen_mut.bin" : "/prog_p3_nocopen.bin");
        base = run_capture(base_bin, kBudget);
        bool base_ran = base.loaded && base.done.has_value();
        auto base_reads = instr::copper_reads(base.log, kRegions);
        checks.expect(base_ran && base_reads.empty(),
            "G1 precondition: with COPEN off, no read lands in the Copper region over the same capture");

        // G2's own mutation (defect 16): the same no-COPEN baseline
        // already shows DMACONR's bit 7 (COPEN) reading 0, the natural
        // contrast to the main run's G2 (bits 9 and 7 set).
        std::optional<uint16_t> base_dmaconr;
        for (const auto& a : base.log) if (a.is_write && a.word_addr == word_addr(kDmaconrSave)) base_dmaconr = a.data;
        checks.note(std::string("G2 mutation (prog_p3_nocopen.bin): DMACONR bit 7 (COPEN) reads ") +
                    (base_dmaconr && (*base_dmaconr & 0x0080) ? "1 (unexpected)" : "0, as predicted"));
    }

    // ---- G2: Copper DMA is really on -----------------------------------
    {
        std::optional<uint16_t> val;
        for (const auto& a : c.log)
            if (a.is_write && a.word_addr == word_addr(kDmaconrSave)) val = a.data;
        bool g2 = val.has_value() && (*val & 0x0200) && (*val & 0x0080);
        checks.expect(g2, "G2 precondition: DMACONR (stored at $0E7100) has bits 9 and 7 set");
    }

    // ---- G3: Copper reads are identified over every Copper region of
    // 4.2 that this run uses (at least D), and the first fetch after M0
    // is at L_3 --------------------------------------------------------
    {
        auto m0w = write_cycles(c.log, word_addr(kMarkM0));
        bool g3 = !m0w.empty() && !fr.fetches.empty();
        if (g3) {
            const instr::Fetch* first_after = nullptr;
            for (const auto& f : fr.fetches) if (f.t1 > m0w[0]) { first_after = &f; break; }
            g3 = first_after && first_after->byte_addr == kL3;
        }
        checks.expect(g3, "G3 precondition: the first Copper instruction fetch after marker M0 is at L_3 ($0E2000)");
    }

    // ---- G4: every Copper read's data is the word this run's own words
    // prescribe (specification 4.5: a contrasting case by its own words,
    // except P3-G4 itself, whose whole point is to diverge) ------------
    {
        auto ref = reference_words(mutate);
        bool g4 = instr::check_g4(fr.fetches, ref);
        checks.expect(g4, "G4 precondition: every Copper read's data equals the word this run's own setup prescribes for that address");
    }

    // ---- G5: the monitor's beam readings are sane (PAL rule) -----------
    {
        auto g5 = instr::check_g5(samples, fields, kCck);
        checks.note("G5 (P3): mean line length=" + std::to_string(g5.mean_line) + " memory cycles over " +
                    std::to_string(g5.pairs) + " pairs (227 +/- " + std::to_string(instr::kMeanLineTolerance) + ")");
        if (!g5.ok) checks.note("G5 diagnostic: h_violations=" + std::to_string(g5.h_violations) +
                                 " (first H=" + std::to_string(g5.diag_h) + ") v_violations=" + std::to_string(g5.v_violations) +
                                 " gap_violations=" + std::to_string(g5.gap_violations) + " (first gap=" + std::to_string(g5.diag_gap) +
                                 " mem cycles)");
        checks.expect(g5.ok, "G5 precondition: beam readings are sane -- H in $00-$E2, V non-decreasing per field, PAL lines 227 memory cycles apart on average");
    }

    // ---- G6: every counted interval (one line) holds at least 5 kept
    // samples, and the specified field count (3) is met -------------------
    {
        auto g6 = instr::check_g6(samples, fields);
        if (!g6.ok) checks.note("G6 diagnostic: smallest line holds " + std::to_string(g6.min_count) + " samples");
        checks.expect(g6.ok, "G6 precondition: every line holds at least 5 kept samples");
        checks.expect(fields.size() >= 3, "G6-fields precondition: at least 3 fields are captured (specification P3.2)");
    }

    if (mutate == "P3-G5") {
        checks.record("MUTATION P3-G5: with the same config bit ECS/PAL captures use except NTSC selected, "
                       "the mean line length measured here is still 227.0 (both cores); p.23's own 227/228 "
                       "alternation is not exercised at this boundary, so G5's own contrasting case is "
                       "unfalsifiable here, not a defect in the check.");
        return; // demonstrates G5 only; nothing below applies under NTSC
    }

    // ---- P3-fixture: a WAIT-shaped fetch and a set-MOVE fetch each
    // occurred at least once, located by content in this run's own list,
    // not by a fixed address (defect 7) -- "clear-MOVE fetched" is left
    // to P3-p2 below, not bundled into this gate.
    auto is_wait_shaped = [](const instr::Fetch& f) { return (f.ir1 & 0x0001) && f.ir2 == 0xFF00; };
    auto is_set_shaped   = [&](const instr::Fetch& f) { return f.ir1 == kIntenaAddr && (f.ir2 & 0x8000) != 0; };
    auto is_clear_shaped = [&](const instr::Fetch& f) { return f.ir1 == kIntenaAddr && (f.ir2 & 0x8000) == 0; };

    const instr::Fetch* any_wait = instr::find_if(fr.fetches, is_wait_shaped);
    const instr::Fetch* any_set  = instr::find_if(fr.fetches, is_set_shaped);
    bool have_fixture = any_wait && any_set;
    checks.expect(have_fixture, "P3-fixture: a WAIT-shaped fetch and a set-MOVE fetch each occurred at least once");
    if (!have_fixture) {
        if (mutate == "P3-C2")
            checks.record("MUTATION P3-C2: neither a WAIT-shaped fetch nor a set-MOVE fetch ever occurs (the "
                           "Copper parks permanently at the WAIT's own address before reaching the set-MOVE); "
                           "P3-ii and P3-iv are not evaluated for this stimulus, consistent with specification "
                           "P3.6's own prediction (bit 2 stays 0 all field, the set-MOVE is never fetched).");
        else if (mutate == "P3-G3")
            checks.record("MUTATION P3-G3: the Copper is diverted to the decoy park D and never fetches from "
                           "L_3; P3-p1, P3-p2, P3-i, P3-ii, P3-iii and P3-iv are not evaluated for this stimulus "
                           "(G3 alone carries it).");
        return;
    }

    // ---- P3-p1: the Copper really waited, within one execution ----------
    {
        bool p1 = false;
        for (size_t e = 0; e < execs.size(); ++e) {
            const auto& ef = execs[e].fetches;
            const instr::Fetch* w = instr::find_if(ef, is_wait_shaped);
            const instr::Fetch* s = instr::find_if(ef, is_set_shaped);
            if (!w || !s) continue;
            int wait_line = (w->ir1 >> 8) & 0xFF;
            for (const auto& sm : samples)
                if (sm.s > w->t2 && sm.s < s->t1 && sm.V() < wait_line) { p1 = true; break; }
            if (p1) break;
        }
        checks.expect(p1, "P3-p1 precondition: within one execution, a sample between the WAIT's IR2 read and the set-MOVE's IR1 read reads a line below the WAIT's own line");
    }

    // ---- P3-p2: the clearing MOVE precedes the WAIT, in every execution
    // that has a WAIT-shaped fetch ----------------------------------------
    {
        bool p2 = true;
        for (const auto& e : execs) {
            const instr::Fetch* w = instr::find_if(e.fetches, is_wait_shaped);
            if (!w) continue;
            const instr::Fetch* cl = instr::find_if(e.fetches, is_clear_shaped);
            if (!cl || !(cl->t2 < w->t1)) { p2 = false; break; }
        }
        checks.expect(p2, "P3-p2 precondition: the clearing MOVE is fetched, before the WAIT, in every execution");
    }

    // ---- (i) and (iii): each V-wrap field (`fields`, I2) is graded
    // against its OWN execution's own clear-move and set-move fetches,
    // matched positionally (field f <-> execution f: both a beam wrap and
    // a list restart happen exactly once per field, a few memory cycles
    // apart) and found by content, not by a fixed address (defect 3).
    // Bounding a field's population strictly to that one field's own
    // sample span keeps a sample that reads V=0 because the beam has
    // wrapped, while the Copper has not yet re-fetched that field's own
    // clear-move, out of the PREVIOUS execution's population: such a
    // sample is excluded here (from neither field), not compared.
    bool i_ok = true; int i_violations = 0; uint64_t diag_i_s = 0; int diag_i_v = -1;
    bool ii_ok = true;
    bool iii_ok = true;
    for (const auto& s : samples) if (s.V() >= 0x66 && !(s.intenar & 0x0004)) ii_ok = false;
    for (size_t f = 0; f < fields.size(); ++f) {
        if (f >= execs.size()) continue;
        const instr::Fetch* cl = instr::find_if(execs[f].fetches, is_clear_shaped);
        const instr::Fetch* set_f = instr::find_if(execs[f].fetches, is_set_shaped);
        for (size_t i = fields[f].begin; i < fields[f].end; ++i) {
            const auto& s = samples[i];
            bool bit2 = (s.intenar & 0x0004) != 0;
            if (cl && s.V() < 0x64 && bit2 && s.s > cl->t2) {
                i_ok = false; ++i_violations;
                if (!diag_i_s) { diag_i_s = s.s; diag_i_v = s.V(); }
            }
            if (set_f && s.s > set_f->t1 && s.V() < 0x64) iii_ok = false;
        }
    }
    if (!i_ok) checks.note("P3-i diagnostic: " + std::to_string(i_violations) +
                            " violating samples, first at cycle " + std::to_string(diag_i_s) +
                            " V=" + std::to_string(diag_i_v));
    checks.expect(i_ok,
        "P3-i [p19] ch2-about-the-copper.c13.s2,ch2-about-the-copper.c18.s2: every sample below line $64, in its own field, reads INTENAR bit 2 clear from that field's own clear-MOVE onward");
    checks.expect(ii_ok,
        "P3-ii [p19] ch2-about-the-copper.c13.s2,ch2-about-the-copper.c18.s2: every sample at or after line $66 reads INTENAR bit 2 set");
    checks.expect(iii_ok,
        "P3-iii [p19] ch2-about-the-copper.c13.s2,ch2-about-the-copper.c18.s2: every sample after the set-MOVE's IR1 read, in its own field, reads line >= $64");

    // ---- (iv): the set-MOVE is fetched exactly once per execution but
    // the last ---------------------------------------------------------
    {
        bool iv_ok = true;
        for (size_t e = 0; e + 1 < execs.size(); ++e) {
            int count = 0;
            for (const auto& f : execs[e].fetches) if (is_set_shaped(f)) ++count;
            if (count != 1) iv_ok = false;
        }
        checks.expect(iv_ok,
            "P3-iv [p19] ch2-about-the-copper.c13.s2: the set-MOVE is fetched exactly once per execution");
    }
}

} // namespace p3

// ---------------------------------------------------------------------
// P1-A [p19-21,26,36,274-275]: a Copper MOVE writes the 16 bits of its
// second instruction word, unchanged and bit n to bit n, into the
// register its first word addresses. Instrument A only (INTENA read back
// through INTENAR). See prog_p1a.s. Instrument B (COP2LC, the 40-target
// chain) is not built in this package.
namespace p1a {

constexpr uint32_t kLA = 0x0E0000;
constexpr uint32_t kMarkM0 = 0x0E7000;
constexpr uint32_t kDmaconrSave = 0x0E7100;
constexpr uint32_t kP1P2V1 = 0x0E7102, kP1P2V2 = 0x0E7104, kP1P2V3 = 0x0E7106;
constexpr uint32_t kLogLo = 0x110000, kLogHi = 0x140000;
constexpr uint64_t kBudget = kBootBudget + 30'000'000;
// 64 half-steps (WAIT+MOVE, 4 words each) plus the closing park (2 words).
constexpr uint32_t kListWords = 64 * 4 + 2;

std::vector<uint16_t> walking_table() {
    std::vector<uint16_t> P(32, 0);
    for (int i = 0; i < 15; ++i) P[i] = (uint16_t)(1u << i);
    for (int i = 0; i < 15; ++i) P[15 + i] = (uint16_t)(0x7FFF & ~(1u << i));
    P[30] = 0x0000;
    P[31] = 0x7FFF;
    return P;
}

// G4's own reference table for a P1-A capture: the 258 words the list
// actually assembles to for this run (is_set, mutate), matching
// prog_p1a.s and the p1a_list_*.i files word for word.
std::vector<std::pair<uint32_t, uint16_t>> reference_words(bool is_set, const std::string& mutate) {
    std::vector<std::pair<uint32_t, uint16_t>> out;
    auto P = walking_table();
    uint32_t addr = kLA;
    for (int i = 0; i < 32; ++i) {
        int wait_line_a = 0x30 + 4 * i;
        bool park_a = is_set && mutate == "P1-P1" && i == 5; // P1-p1's own mutation
        if (park_a) { out.push_back({addr, 0xFFFF}); out.push_back({addr + 2, 0xFFFE}); }
        else {
            out.push_back({addr, (uint16_t)(((wait_line_a & 0xFF) << 8) | 1)});
            out.push_back({addr + 2, 0xFF00});
        }
        addr += 4;
        out.push_back({addr, 0x009A});
        out.push_back({addr + 2, (uint16_t)(is_set ? 0x7FFF : 0xFFFF)});
        addr += 4;

        int wait_line_b = wait_line_a + 2;
        out.push_back({addr, (uint16_t)(((wait_line_b & 0xFF) << 8) | 1)});
        out.push_back({addr + 2, 0xFF00});
        addr += 4;
        uint16_t ir1_b = 0x009A;
        uint16_t ir2_b = is_set ? (uint16_t)(0x8000 | P[i]) : P[i];
        if (i == 0) {
            if (is_set && mutate == "P1-C1") ir1_b = 0x009C;
            if (is_set && mutate == "P1-C2") ir2_b = 0x8002;
            if (!is_set && mutate == "P1-CC1") ir2_b = 0x0002;
        }
        out.push_back({addr, ir1_b});
        out.push_back({addr + 2, ir2_b});
        addr += 4;
    }
    out.push_back({addr, 0xFFFF});
    out.push_back({addr + 2, 0xFFFE});
    return out;
}

struct Run {
    Capture c;
    std::vector<instr::Sample> samples;
    std::vector<instr::Span> fields;
    std::vector<instr::Fetch> fetches;      // every paired fetch in [kLA, kLA+kListWords)
    std::vector<instr::Exec> execs;         // fetches split at kLA (list executions)
    instr::Ratio ratio;
    uint64_t kCck = 0;
    bool ok = false;
};

Run capture_and_index(const std::string& bin) {
    Run r;
    r.c = run_capture(bin, kBudget);
    r.ok = r.c.loaded && r.c.done.has_value();
    if (!r.ok) return r;
    auto raw = instr::parse_samples(r.c.log, word_addr(kLogLo), word_addr(kLogHi));
    r.samples = instr::discard_torn(raw);
    r.fields = instr::find_fields(r.samples);
    std::vector<std::pair<uint32_t, uint32_t>> regions = {{word_addr(kLA), word_addr(kLA) + kListWords}};
    auto reads = instr::copper_reads(r.c.log, regions);
    auto fr = instr::pair_fetches(reads);
    r.fetches = fr.fetches;
    r.execs = instr::split_executions(r.fetches, kLA);
    r.ratio = instr::measure_ratio(r.samples, r.fields);
    r.kCck = r.ratio.ok ? r.ratio.kCck : 0;
    return r;
}

struct GradeResult {
    bool ok = true; int violations = 0; int covered = 0; std::string first_desc;
    std::array<int, 64> k_covered{}; std::array<int, 64> k_violations{};
    int min_interval = -1;
};

// Grades every hold interval, over every list execution but the last
// (which the capture's own fixed cutoff can leave mid-list), against the
// table P1's proposition text gives (SPECIFICATION.md P1.4). Each
// half-step's own MOVE is found by its address ($0E0004+8h) within its own
// execution, not by position, so a restart fetch that happens to precede
// that field's own next I2 sample never shifts every later half-step's
// anchor (defect 1). A half-step whose own MOVE fetch, or whose next
// half-step's MOVE fetch, is missing (P1-p1 covers whether it should be
// present) contributes no graded population for that one half-step.
GradeResult grade(const std::vector<instr::Exec>& execs, const std::vector<uint16_t>& P, bool is_set,
                   const std::vector<instr::Sample>& samples, uint64_t kM_cycles) {
    GradeResult g;
    if (execs.size() < 2) { g.ok = false; g.first_desc = "fewer than 2 list executions captured"; return g; }
    for (size_t e = 0; e + 1 < execs.size(); ++e) {
        for (int k = 0; k < 64; ++k) {
            uint32_t mv_addr = kLA + 4 + 8u * (uint32_t)k;
            const instr::Fetch* mv = instr::find_by_addr(execs[e].fetches, mv_addr);
            if (!mv) continue;
            const instr::Fetch* nextmv = (k < 63)
                ? instr::find_by_addr(execs[e].fetches, kLA + 4 + 8u * (uint32_t)(k + 1))
                : instr::find_by_addr(execs[e + 1].fetches, kLA + 4);
            if (!nextmv) continue;
            uint64_t lo = mv->t2 + kM_cycles;
            uint64_t hi = nextmv->t2;
            if (hi <= lo) continue;

            bool is_a = (k % 2 == 0);
            int i = is_a ? k / 2 : (k - 1) / 2;
            uint16_t expected = is_a ? (is_set ? 0x0000 : 0x7FFF)
                                     : (is_set ? P[i] : (uint16_t)(0x7FFF & ~P[i]));

            int covered_here = 0;
            for (const auto& s : samples) {
                if (s.s <= lo || s.s > hi) continue;
                ++covered_here; ++g.covered;
                uint16_t got = s.intenar & 0x7FFF;
                if (got != expected) {
                    g.ok = false; ++g.violations; ++g.k_violations[k];
                    if (g.first_desc.empty()) {
                        char buf[160];
                        std::snprintf(buf, sizeof buf, "execution %zu half-step %s%d: expected $%04X got $%04X",
                                      e, is_a ? "a_" : "b_", i, expected, got);
                        g.first_desc = buf;
                    }
                }
            }
            g.k_covered[k] += covered_here;
            if (covered_here > 0 && (g.min_interval < 0 || covered_here < g.min_interval)) g.min_interval = covered_here;
        }
    }
    return g;
}

void run(Checks& checks, const std::string& dir, const std::string& mutate) {
    std::string set_bin = dir + "/prog_p1a.bin";
    if (mutate == "P1-C1") set_bin = dir + "/prog_p1a_c1.bin";
    else if (mutate == "P1-C2") set_bin = dir + "/prog_p1a_c2.bin";
    else if (mutate == "P1-P1") set_bin = dir + "/prog_p1a_mut_p1p1.bin";
    else if (mutate == "P1-P2") set_bin = dir + "/prog_p1a_mut_p1p2.bin";
    std::string clear_bin = dir + "/prog_p1a_clear.bin";
    if (mutate == "P1-CC1") clear_bin = dir + "/prog_p1a_clear_c1.bin";

    Run set_run = capture_and_index(set_bin);
    checks.expect(set_run.ok, "P1-fixture: the set run ran to DONE");
    Run clear_run = capture_and_index(clear_bin);
    checks.expect(clear_run.ok, "P1-fixture: the clear run ran to DONE");
    if (!set_run.ok || !clear_run.ok) return;

    checks.note("P1-A: set run " + std::to_string(set_run.samples.size()) + " samples/" +
                std::to_string(set_run.fields.size()) + " fields; clear run " +
                std::to_string(clear_run.samples.size()) + " samples/" + std::to_string(clear_run.fields.size()) + " fields");

    // G1's own baseline (no COPEN), shared by both captures: the fact
    // ("no read lands in the Copper region while COPEN is off") does not
    // depend on list content, only on DMACON, so one baseline serves
    // both labels below. Its own red mutation stays demonstrated on P3
    // (this file's header, and P3's own scope-decision comment).
    Capture g1_baseline = run_capture(dir + "/prog_p1a_nocopen.bin", kBudget);
    std::vector<std::pair<uint32_t, uint32_t>> regions = {{word_addr(kLA), word_addr(kLA) + kListWords}};
    bool g1_base_ran = g1_baseline.loaded && g1_baseline.done.has_value();
    bool g1_empty = instr::copper_reads(g1_baseline.log, regions).empty();

    auto check_all = [&](const Run& r, const char* label, bool is_set) {
        checks.expect(g1_base_ran && g1_empty,
            std::string("G1 precondition (") + label + "): with COPEN off, no read lands in the Copper region over the same capture");

        std::optional<uint16_t> dmaconr;
        for (const auto& a : r.c.log) if (a.is_write && a.word_addr == word_addr(kDmaconrSave)) dmaconr = a.data;
        bool g2 = dmaconr.has_value() && (*dmaconr & 0x0200) && (*dmaconr & 0x0080);
        checks.expect(g2, std::string("G2 precondition (") + label + "): DMACONR has bits 9 and 7 set");

        auto m0w = write_cycles(r.c.log, word_addr(kMarkM0));
        bool g3 = !m0w.empty();
        if (g3) {
            const instr::Fetch* first_after = nullptr;
            for (const auto& f : r.fetches) if (f.t1 > m0w[0]) { first_after = &f; break; }
            g3 = first_after && first_after->byte_addr == kLA;
        }
        checks.expect(g3, std::string("G3 precondition (") + label + "): the first Copper instruction fetch after marker M0 is at L_A ($0E0000)");

        auto ref = reference_words(is_set, mutate);
        bool g4 = instr::check_g4(r.fetches, ref);
        checks.expect(g4, std::string("G4 precondition (") + label + "): every Copper read's data equals the word this run's own list prescribes for that address");

        auto g5 = instr::check_g5(r.samples, r.fields, r.kCck);
        checks.note(std::string("G5 (") + label + "): mean line length=" + std::to_string(g5.mean_line) +
                    " memory cycles over " + std::to_string(g5.pairs) + " pairs (227 +/- " +
                    std::to_string(instr::kMeanLineTolerance) + ")");
        checks.expect(g5.ok, std::string("G5 precondition (") + label + "): beam readings are sane -- H in $00-$E2, V non-decreasing per field, PAL lines 227 memory cycles apart on average");

        auto g6 = instr::check_g6(r.samples, r.fields);
        checks.expect(g6.ok, std::string("G6 precondition (") + label + "): every line holds at least 5 kept samples");
    };
    check_all(set_run, "set run", true);
    check_all(clear_run, "clear run", false);

    // ---- I4: the clk_sys-per-memory-cycle ratio, per capture, graded as
    // the precondition the hold-interval margin (M) depends on -----------
    checks.note("I4 (set run): kCck=" + std::to_string(set_run.ratio.kCck));
    checks.note("I4 (clear run): kCck=" + std::to_string(clear_run.ratio.kCck));
    checks.expect(set_run.ratio.ok, "I4 precondition (set run) [p24]: the clk_sys-per-memory-cycle ratio is the same integer over the whole capture");
    checks.expect(clear_run.ratio.ok, "I4 precondition (clear run) [p24]: the clk_sys-per-memory-cycle ratio is the same integer over the whole capture");
    uint64_t kM_set = 32 * set_run.kCck;
    uint64_t kM_clear = 32 * clear_run.kCck;

    // ---- P1-p1: every half-step's WAIT and MOVE instruction fetches are
    // present, at their own address, in list order, per execution but the
    // last, on both runs (P1.5 applies P1-p1 to both) ---------------------
    auto check_p1p1 = [&](const Run& r, const char* label) {
        bool ok = true; int violations = 0;
        for (size_t e = 0; e + 1 < r.execs.size(); ++e) {
            const auto& ef = r.execs[e].fetches;
            bool exec_ok = ef.size() >= 128;
            if (exec_ok) for (int j = 0; j < 128 && exec_ok; ++j)
                if (ef[j].byte_addr != kLA + 4u * (uint32_t)j) exec_ok = false;
            if (!exec_ok) { ok = false; ++violations; }
        }
        if (!ok) checks.note(std::string("P1-p1 diagnostic (") + label + "): " + std::to_string(violations) + " execution(s) with a missing, extra or out-of-order fetch");
        checks.expect(ok, std::string("P1-p1 precondition (") + label + "): every half-step's WAIT and MOVE instruction fetches are present, at their own address, in list order, in every execution");
    };
    check_p1p1(set_run, "set run");
    check_p1p1(clear_run, "clear run");

    // ---- P1-p2: the instrument works, masked (p.218: bit 15 has no
    // meaning in the read-only addresses) ---------------------------------
    {
        std::optional<uint16_t> v1, v2, v3;
        for (const auto& a : set_run.c.log) {
            if (a.is_write && a.word_addr == word_addr(kP1P2V1)) v1 = a.data;
            if (a.is_write && a.word_addr == word_addr(kP1P2V2)) v2 = a.data;
            if (a.is_write && a.word_addr == word_addr(kP1P2V3)) v3 = a.data;
        }
        bool p1p2 = v1.has_value() && v2.has_value() && v3.has_value() &&
                    (*v1 & 0x7FFF) == 0x0000 && (*v2 & 0x7FFF) == 0x5555 && (*v3 & 0x7FFF) == 0x0000;
        checks.expect(p1p2, "P1-p2 precondition: the CPU's own INTENA writes read back (masked $7FFF) as $0000, $5555 and $0000");
    }

    // ---- P1 (instrument A) main assertions --------------------------------
    auto P = walking_table();
    GradeResult gset = grade(set_run.execs, P, true, set_run.samples, kM_set);
    GradeResult gclear = grade(clear_run.execs, P, false, clear_run.samples, kM_clear);

    if (!gset.ok) checks.note("P1-set diagnostic: " + std::to_string(gset.violations) +
                               "/" + std::to_string(gset.covered) + " graded samples violate, first: " + gset.first_desc);
    checks.expect(gset.ok,
        "P1-set [p19,p20,p21,p26,p36,p274,p275] ch2-about-the-copper.c13.s2,"
        "ch2-what-is-a-copper-instruction.c4.s2,ch2-the-move-instruction.c1.s2,"
        "COP2-copins-fields-p275.c5,COP2-copins-fields-p275.c6,"
        "ch2-appa-copins.c8.s2,ch2-summary-of-copper-instructions.c98,ch2-summary-of-copper-instructions.c99,"
        "ch2-summary-of-copper-instructions.c101: every hold interval, in "
        "every execution, shows the state the table predicts (a_i clears INTENA to $0000, b_i sets it to P_i)");

    if (!gclear.ok) checks.note("P1-clear diagnostic: " + std::to_string(gclear.violations) +
                                 "/" + std::to_string(gclear.covered) + " graded samples violate, first: " + gclear.first_desc);
    checks.expect(gclear.ok,
        "P1-clear [p19,p20,p21,p26,p36,p274,p275] ch2-about-the-copper.c13.s2,ch2-what-is-a-copper-instruction.c4.s2,"
        "ch2-the-move-instruction.c1.s2,COP2-copins-fields-p275.c5,COP2-copins-fields-p275.c6,"
        "COP2-copins-fields-p275.c9: every hold interval, in every execution, shows the state the table "
        "predicts (a_i sets INTENA to $7FFF, b_i clears it to $7FFF AND NOT P_i)");

    checks.record("ch2-about-the-copper.c6.s2 ('most of the chip control registers') is graded on P1-set "
                  "for INTENA only, as section 2 of the specification states.");
    checks.record("ch2-appa-copins.c116 ('directly from RAM') is P2's clause (specification P2.4); "
                  "declined on both P1-A lines until P2 is built.");
    checks.record("COP2-copins-fields-p275.c9 and ch2-summary-of-copper-instructions.c100 ('the destination is "
                  "the address supplied by the DA field') are declined on P1-set: only DA=$09A (INTENA) is "
                  "graded here; P1-B (not built) adds $084, $086 and $08A.");

    checks.expect(set_run.fields.size() >= 3, "G6-fields precondition (set run): at least 3 fields are captured (specification P1.2)");
    checks.expect(clear_run.fields.size() >= 3, "G6-fields precondition (clear run): at least 3 fields are captured (specification P1.2)");

    // ---- The 64 Table 2-2 / Appendix A IR2 row ids: one assertion per
    // IR2 bit, graded over both runs, each carrying that bit's four row
    // ids (defects 21, 25). Bits 0-14: walking one at b_n, walking zero
    // at b_(15+n), in every execution, both runs -- isolating exactly the
    // hold intervals that exercise that bit. Bit 15: neither run alone
    // establishes it (the set run only ever writes IR2 bit 15 = 1, the
    // clear run only ever writes 0); the two runs' own grade, taken
    // together, do.
    // A half-step's own MOVE fetch can be legitimately absent this run (a
    // different precondition's own mutation, P1-p1's for instance, breaks
    // the list from some half-step on); an empty population is then not
    // itself a violation here (grade() already applies the same rule),
    // only something P1-p1 itself catches -- so a bit's own check reads
    // its covered population where one exists and is silent where none
    // does, never failing on absence alone.
    auto bit_ok = [&](int k) {
        return gset.k_violations[k] == 0 && gclear.k_violations[k] == 0;
    };
    auto bit_covered = [&](int k) { return gset.k_covered[k] > 0 && gclear.k_covered[k] > 0; };
    for (int n = 0; n < 15; ++n) {
        int k = 2 * (15 + n) + 1; // b_(15+n): k = 2*i+1, i=15+n
        int kb = 2 * n + 1;       // b_n: k = 2*i+1, i=n
        bool ok = bit_ok(kb) && bit_ok(k);
        bool covered = bit_covered(kb) && bit_covered(k);
        int K = 2 + 6 * (15 - n), J = 15 + 6 * (15 - n);
        char label[320];
        std::snprintf(label, sizeof label,
            "P1-bit%d [p36,p274-275] ch2-summary-of-copper-instructions.c%d,COP2-summary-of-copper-instructions.c%d,"
            "COP2-copins-encoding.c%d,ch2-appa-copins.c%d: IR2 bit %d is carried unchanged into register bit %d, "
            "walking one (b_%d) and walking zero (b_%d), every execution, both runs",
            n, K, K, K, J, n, n, n, 15 + n);
        if (!covered) checks.note(std::string("P1-bit") + std::to_string(n) + " diagnostic: empty population in at least one run (no violation on that account alone)");
        checks.expect(ok, label);
    }
    {
        // Isolated to the a_i half-steps (even k): these are the ones
        // whose own IR2 is fixed ($7FFF set run / $FFFF clear run) and
        // whose result depends on nothing but bit 15's own SET/CLR role,
        // so this stays unaffected by a b_0-only mutation (C1, C2, CC1)
        // -- unlike gset.ok/gclear.ok as a whole, which also cover b_i. An
        // a_i absent this run (P1-p1's own mutation breaks the list from
        // some half-step on) is, again, not itself a violation here.
        bool ok15 = true;
        for (int k = 0; k < 64; k += 2) {
            if (gset.k_violations[k] != 0) ok15 = false;
            if (gclear.k_violations[k] != 0) ok15 = false;
        }
        checks.expect(ok15,
            "P1-bit15 [p21,p36,p274-275] ch2-the-move-instruction.c7,ch2-summary-of-copper-instructions.c2,"
            "COP2-summary-of-copper-instructions.c2,COP2-copins-encoding.c2,ch2-appa-copins.c15: IR2 bit 15 is "
            "carried unchanged into the SET/CLR control input -- the set run's own grade (bit 15 = 1 throughout) "
            "and the clear run's own grade (bit 15 = 0 throughout), together, establish it");
    }
}

} // namespace p1a

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    p4::run(checks, dir, mv);
    p3::run(checks, dir, mv);
    p1a::run(checks, dir, mv);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
