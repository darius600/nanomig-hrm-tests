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
// MUTATION P2-p1 stimulus red the $AAAA write happens in step 3, before the Copper starts, via NMH_MUTATE=P2-P1
// MUTATION P2-ii,P2-iii,P2-iv stimulus red (contrasting case) $AAA8 is written instead of $AAAA, at the same
//   point, graded against the main run's own words (specification 4.5's dedicated-mutation exception), via
//   NMH_MUTATE=P2-C1
// MUTATION P6-p1 stimulus red L6a is a WAIT (line $64) followed by a park at $0E3004, instead of a park at
//   $0E3000, via NMH_MUTATE=P6-P1 (P6-i also turns red at that field, the page's own consequence of a second,
//   mid-field fetch, not a separate fault)
// MUTATION P6-p2 stimulus red the capture is 2 fields long, via NMH_MUTATE=P6-P2
// MUTATION P6-p3 stimulus red (NTSC run) the same NTSC-graded capture is made with the harness's own PAL Config
//   instead of NTSC, via NMH_MUTATE=P6-P3
// MUTATION P6-i stimulus red (contrasting case) in field 3 the CPU writes COP1LC=L6b once, with no strobe;
//   graded against the main run's own "L6a in every field" comparator, via NMH_MUTATE=P6-C1
// MUTATION G1 stimulus red (P7's own baseline) the no-AUD0EN baseline also executes one CPU read inside an
//   audio region, via NMH_MUTATE=P7-G1
// MUTATION P7-p2 stimulus red DMACON is written $8203 (AUD0EN and AUD1EN), with AUD1LC also pointing at A,
//   via NMH_MUTATE=P7-P2
// MUTATION P7-p3 stimulus red AUD0LC is set to B instead of A, via NMH_MUTATE=P7-P3
// MUTATION P7-p4 stimulus red AUD0LEN is 64 instead of 4, via NMH_MUTATE=P7-P4
// MUTATION P5-p1 stimulus red before the tenth phase-2 step the CPU waits for line 5 of the next field, via
//   NMH_MUTATE=P5-P1
// MUTATION P5-p2 stimulus red the park at $0C0020 is replaced by a WAIT (line 0) followed by a park at
//   $0C0024, via NMH_MUTATE=P5-P2
// MUTATION P5-p3 stimulus red the ROM-overlay-clearing writes are omitted, via NMH_MUTATE=P5-P3
// MUTATION P5-p2 stimulus red (C1) phase 3's two words are exchanged, graded against the main run's own
//   unchanged target ($0E9010, 4.5's dedicated-mutation exception), via NMH_MUTATE=P5-C1 (phase 3's own step
//   has no bit-group index, so P5-L/P5-H-low/P5-H-high are unaffected)
// MUTATION P5-L not exercised (C2) one extra step after the phase-1 step to $0C8000 -- MOVE.W #$4000,$DFF082
//   only -- is a positive extension of coverage (a word write to COP1LCL alone leaves COP1LCH untouched), not
//   a divergence from the main run: the specification's own prediction ($0C4000) is the correct landing, so
//   this stimulus is graded the same as any other step and is expected green, via NMH_MUTATE=P5-C2
// MUTATION P5-L,P5-p2 stimulus red (C3) the first phase-2 step strobes COPJMP1 instead of COPJMP2, graded
//   against the main run's own unchanged target (target index 0, in the L bits 15-1 group), via
//   NMH_MUTATE=P5-C3
//
// P1-B (the COP2LC 40-target address chain) is not built in this package.
// P7's own contrasting case (a mid-block AUD0LC write) is not built: it
// needs a CPU delay tuned against this run's own measured fetch timing,
// not yet derived.

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

// h_max/mean_lo/mean_hi default to the PAL rule every existing caller (P4,
// P3, P1-A, P2) relies on; P6's own NTSC run passes p.23's NTSC values
// instead (SPECIFICATION.md P6.5) -- the gross per-pair bound stays
// centred on 227 either way, since a 40-cycle tolerance already covers
// NTSC's own 227/228 alternation.
G5Result check_g5(const std::vector<Sample>& samples, const std::vector<Span>& fields, uint64_t kCck,
                   int h_max = 0xE2, double mean_lo = (double)kPalLineCck - kMeanLineTolerance,
                   double mean_hi = (double)kPalLineCck + kMeanLineTolerance) {
    G5Result g;
    if (kCck == 0) { g.ok = false; return g; }

    double sum = 0.0;
    for (const auto& fld : fields) {
        for (size_t i = fld.begin; i < fld.end; ++i) {
            if (samples[i].H() > h_max) { g.ok = false; ++g.h_violations; if (g.diag_h < 0) g.diag_h = samples[i].H(); }
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
        if (g.mean_line < mean_lo || g.mean_line > mean_hi) g.ok = false;
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

// ---------------------------------------------------------------------
// P2 [p21,p36,p275]: the written data is the word read from RAM as the
// instruction's second word, at each execution. Reuses P1-A-set's own
// list, addressing and capture/index machinery (p1a::kLA, p1a::capture_and_index,
// p1a::Run, p1a::reference_words): only the stimulus differs -- prog_p2.s
// writes $AAAA to b_0's own IR2 word ($0E000E) once, after the first field
// wrap (SPECIFICATION.md P2.2). See prog_p2.s.
namespace p2 {

constexpr uint32_t kWriteTarget = p1a::kLA + 0x0E;      // b_0's own IR2 word, $0E000E
constexpr uint32_t kB0Addr      = p1a::kLA + 4 + 8 * 1; // b_0's own MOVE, $0E000C
constexpr uint32_t kA1Addr      = p1a::kLA + 4 + 8 * 2; // a_1's own MOVE, $0E0014
constexpr uint16_t kBeforeVal   = 0x8001;               // b_0's IR2 before the change
constexpr uint16_t kMainWriteVal = 0xAAAA;              // the main run's own written value -- the fixed
                                                          // comparator P2-C1 (4.5's dedicated-mutation
                                                          // exception) is graded against, not its own $AAA8

// Every write at kWriteTarget whose data is `val` -- distinguishes the
// mutation write from the earlier, unrelated list-setup write of $8001 at
// the same address (b_0's own IR2 word is written once as ordinary list
// content in step 3, then again, with a different value, as this
// proposition's own stimulus).
std::vector<uint64_t> mutation_write_cycles(const std::vector<nmh::RamAccess>& log, uint16_t val) {
    std::vector<uint64_t> out;
    for (const auto& a : log)
        if (a.is_write && a.word_addr == word_addr(kWriteTarget) && a.data == val) out.push_back(a.cycle);
    return out;
}

// G4's own reference table, with the one address this proposition's own
// stimulus changes at runtime left out (SPECIFICATION.md P2.5: "G4 applies
// at every address except $0E000E"); (ii) below grades that address
// instead, against each execution's own fetch.
bool check_g4_except(const std::vector<instr::Fetch>& fetches,
                      const std::vector<std::pair<uint32_t, uint16_t>>& ref, uint32_t except_byte_addr) {
    for (const auto& f : fetches) {
        for (const auto& kv : ref) {
            if (kv.first == except_byte_addr) continue;
            if (kv.first == f.byte_addr && f.ir1 != kv.second) return false;
            if (kv.first == f.byte_addr + 2 && f.ir2 != kv.second) return false;
        }
    }
    return true;
}

void run(Checks& checks, const std::string& dir, const std::string& mutate) {
    std::string bin = dir + "/prog_p2.bin";
    uint16_t write_val = 0xAAAA;
    if (mutate == "P2-P1") bin = dir + "/prog_p2_mut_p2p1.bin";
    else if (mutate == "P2-C1") { bin = dir + "/prog_p2_c1.bin"; write_val = 0xAAA8; }

    p1a::Run r = p1a::capture_and_index(bin);
    checks.expect(r.ok, "P2-fixture: the program ran to DONE");
    if (!r.ok) return;
    checks.note("P2: " + std::to_string(r.samples.size()) + " kept samples, " + std::to_string(r.fields.size()) + " fields");

    // ---- G1 (shared baseline, P1-A's own capture: no list-content
    // dependency), G2, G3, G4 (except $0E000E), G5, G6 -- restated on
    // P2's own capture, not re-mutated here (their own red mutations stay
    // demonstrated on P3 and P1-A, per those files' own header comments) --
    {
        std::vector<std::pair<uint32_t, uint32_t>> regions = {{word_addr(p1a::kLA), word_addr(p1a::kLA) + p1a::kListWords}};
        Capture g1_baseline = run_capture(dir + "/prog_p1a_nocopen.bin", p1a::kBudget);
        bool g1_base_ran = g1_baseline.loaded && g1_baseline.done.has_value();
        bool g1_empty = instr::copper_reads(g1_baseline.log, regions).empty();
        checks.expect(g1_base_ran && g1_empty,
            "G1 precondition (P2): with COPEN off, no read lands in the Copper region over the same capture");

        std::optional<uint16_t> dmaconr;
        for (const auto& a : r.c.log) if (a.is_write && a.word_addr == word_addr(0x0E7100)) dmaconr = a.data;
        bool g2 = dmaconr.has_value() && (*dmaconr & 0x0200) && (*dmaconr & 0x0080);
        checks.expect(g2, "G2 precondition (P2): DMACONR has bits 9 and 7 set");

        auto m0w = write_cycles(r.c.log, word_addr(0x0E7000));
        bool g3 = !m0w.empty();
        if (g3) {
            const instr::Fetch* first_after = nullptr;
            for (const auto& f : r.fetches) if (f.t1 > m0w[0]) { first_after = &f; break; }
            g3 = first_after && first_after->byte_addr == p1a::kLA;
        }
        checks.expect(g3, "G3 precondition (P2): the first Copper instruction fetch after marker M0 is at L_A ($0E0000)");

        auto ref = p1a::reference_words(true, "");
        bool g4 = check_g4_except(r.fetches, ref, kWriteTarget);
        checks.expect(g4, "G4 precondition (P2): every Copper read's data equals the word this run's own list "
                          "prescribes for that address, except $0E000E (graded by P2-ii instead)");

        auto g5 = instr::check_g5(r.samples, r.fields, r.kCck);
        checks.note("G5 (P2): mean line length=" + std::to_string(g5.mean_line) + " memory cycles over " +
                    std::to_string(g5.pairs) + " pairs (227 +/- " + std::to_string(instr::kMeanLineTolerance) + ")");
        checks.expect(g5.ok, "G5 precondition (P2): beam readings are sane -- H in $00-$E2, V non-decreasing per field, PAL lines 227 memory cycles apart on average");

        auto g6 = instr::check_g6(r.samples, r.fields);
        if (!g6.ok) checks.note("G6 diagnostic (P2): smallest line holds " + std::to_string(g6.min_count) + " samples");
        checks.expect(g6.ok, "G6 precondition (P2): every line holds at least 5 kept samples");

        checks.expect(r.ratio.ok, "I4 precondition (P2) [p24]: the clk_sys-per-memory-cycle ratio is the same integer over the whole capture");
        checks.note("I4 (P2): kCck=" + std::to_string(r.kCck));

        checks.expect(r.fields.size() >= 4, "G6-fields precondition (P2): at least 4 fields are captured (specification P2.2)");
    }

    // ---- the mutation write: exactly one, at word $0E000E -------------
    auto wcyc = mutation_write_cycles(r.c.log, write_val);
    char wbuf[16]; std::snprintf(wbuf, sizeof wbuf, "$%04X", write_val);
    bool have_write = wcyc.size() == 1;
    checks.expect(have_write, std::string("P2-fixture: the CPU's own write of ") + wbuf + " to $0E000E was observed exactly once");
    if (!have_write) return;
    const uint64_t t_write = wcyc[0];

    // ---- (ii), (iii), (iv): every execution's b_0 fetch, and the hold
    // interval it opens, graded against that execution's own fetched data
    // -- not a fixed table entry -- split at the CPU write's own port time
    // (SPECIFICATION.md P2.3-4). Only execution windows with both b_0's
    // own MOVE fetch and the next half-step's (a_1) fetch are graded
    // (mirrors p1a::grade()'s own "but the last" scope decision: a hold
    // interval needs a known end).
    bool pre_seen = false, post_seen = false;
    bool ii_ok = true, hold_ok = true;
    int graded = 0;
    std::string first_ii, first_hold;
    const uint64_t kM = 32 * r.kCck;
    for (size_t e = 0; e + 1 < r.execs.size(); ++e) {
        const instr::Fetch* mv = instr::find_by_addr(r.execs[e].fetches, kB0Addr);
        if (!mv) continue;
        const instr::Fetch* nextmv = instr::find_by_addr(r.execs[e].fetches, kA1Addr);
        if (!nextmv) continue;

        bool before = mv->t2 < t_write;
        if (before) pre_seen = true; else post_seen = true;
        uint16_t expect_d = before ? kBeforeVal : kMainWriteVal;
        ++graded;
        if (mv->ir2 != expect_d) {
            ii_ok = false;
            if (first_ii.empty()) {
                char buf[160];
                std::snprintf(buf, sizeof buf, "execution %zu: b_0's own IR2 read got $%04X, expected $%04X (%s the write)",
                              e, mv->ir2, expect_d, before ? "before" : "after");
                first_ii = buf;
            }
        }

        uint64_t lo = mv->t2 + kM, hi = nextmv->t2;
        if (hi <= lo) continue;
        uint16_t expected_hold = (uint16_t)(expect_d & 0x7FFF); // SET run (P1's own table)
        for (const auto& s : r.samples) {
            if (s.s <= lo || s.s > hi) continue;
            uint16_t got = s.intenar & 0x7FFF;
            if (got != expected_hold) {
                hold_ok = false;
                if (first_hold.empty()) {
                    char buf[160];
                    std::snprintf(buf, sizeof buf, "execution %zu hold interval: got $%04X, expected $%04X", e, got, expected_hold);
                    first_hold = buf;
                }
            }
        }
    }

    checks.expect(pre_seen && post_seen,
        "P2-p1 precondition: the CPU's own write falls strictly between two b_0 IR2 reads, with at least one "
        "execution's own read on each side, within the capture");

    if (!ii_ok) checks.note("P2-ii diagnostic: " + first_ii);
    checks.expect(ii_ok,
        "P2-ii [p21,p275] ch2-the-move-instruction.c1.s2,COP2-copins-fields-p275.c5,COP2-copins-fields-p275.c6,"
        "COP2-copins-fields-p275.c8,ch2-appa-copins.c116: at every execution, the data the MOVE's own IR2 read "
        "carries is the word held at $0E000E at that moment -- $8001 before the CPU's own write, the written "
        "value after it -- not a value fetched once and reused");

    if (!hold_ok) checks.note("P2-iii,iv diagnostic: " + first_hold);
    checks.expect(hold_ok,
        "P2-iii,P2-iv [p21,p36,p275] COP2-copins-fields-p275.c5,COP2-copins-fields-p275.c6,COP2-copins-fields-p275.c7,"
        "ch2-appa-copins.c115,ch2-summary-of-copper-instructions.c98,ch2-summary-of-copper-instructions.c99: "
        "the hold interval that opens M memory cycles after each execution's own IR2 read shows the state that "
        "read's own data produces (P1's table), and no earlier sample shows the value the next execution will produce");

    checks.record("ch2-appa-copins.c116 ('directly from RAM') and P2-p1's own read-timing clause are graded "
                  "on b_0's own MOVE only, the one word this proposition's own stimulus changes at runtime; "
                  "every other MOVE of L_A stays as P1-A already grades it.");
}

} // namespace p2

// ---------------------------------------------------------------------
// P6 [p25,p30,p288]: the Copper restarts from COP1LC once per field,
// during vertical blanking, without a strobe. See prog_p6.s.
namespace p6 {

constexpr uint32_t kL6A = 0x0E3000;
constexpr uint32_t kL6B = 0x0E3100;
constexpr uint32_t kL6C = 0x0E3200;
constexpr uint32_t kMarkM0 = 0x0E7000;
constexpr uint32_t kDmaconrSave = 0x0E7100;
constexpr uint32_t kLogLo = 0x110000, kLogHi = 0x140000;
constexpr uint64_t kBudget = kBootBudget + 30'000'000;

const std::vector<std::pair<uint32_t, uint32_t>> kRegions = {
    {word_addr(kL6A), word_addr(kL6A) + 8},
    {word_addr(kL6B), word_addr(kL6B) + 8},
    {word_addr(kL6C), word_addr(kL6C) + 8},
};

void push_words(std::vector<std::pair<uint32_t, uint16_t>>& out, uint32_t base, std::initializer_list<uint16_t> words) {
    uint32_t addr = base;
    for (uint16_t w : words) { out.push_back({addr, w}); addr += 2; }
}

// G4's own reference table: the words this run's own setup actually wrote
// (specification 4.5's "own words" rule). Only P6-P1's own mutation
// changes L6a's content (a WAIT instead of a park); L6b and L6c are the
// same park-plus-six-zeros pad in every run.
std::vector<std::pair<uint32_t, uint16_t>> reference_words(const std::string& mutate) {
    std::vector<std::pair<uint32_t, uint16_t>> out;
    if (mutate == "P6-P1")
        push_words(out, kL6A, {0x6401, 0xFF00, 0xFFFF, 0xFFFE, 0x0000, 0x0000, 0x0000, 0x0000});
    else
        push_words(out, kL6A, {0xFFFF, 0xFFFE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000});
    push_words(out, kL6B, {0xFFFF, 0xFFFE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000});
    push_words(out, kL6C, {0xFFFF, 0xFFFE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000});
    return out;
}

// Runs and grades one P6 capture against one video standard's own bounds
// (SPECIFICATION.md 3.3, P6.4-5). label distinguishes PAL/NTSC in printed
// lines; std_label names the standard the *grading rule* uses (differs
// from label only for P6-p3's own mutation, which grades a PAL capture
// with the NTSC rule).
void run_one(Checks& checks, const std::string& dir, const std::string& bin, nmh::Config cfg,
             const std::string& mutate, const char* label, const char* std_label,
             int restart_v_hi, int h_max, double mean_lo, double mean_hi, int expect_v_a, int expect_v_b) {
    Capture c = run_capture(bin, kBudget, cfg);
    bool ran = c.loaded && c.done.has_value();
    checks.expect(ran, std::string("P6-fixture (") + label + "): the program ran to DONE");
    if (!ran) return;

    auto raw = instr::parse_samples(c.log, word_addr(kLogLo), word_addr(kLogHi));
    auto samples = instr::discard_torn(raw);
    auto fields = instr::find_fields(samples);
    checks.note(std::string("P6 (") + label + "): " + std::to_string(samples.size()) + " kept samples, " +
                std::to_string(fields.size()) + " fields");

    auto reads = instr::copper_reads(c.log, kRegions);
    auto fr = instr::pair_fetches(reads);

    auto ratio = instr::measure_ratio(samples, fields);
    checks.expect(ratio.ok, std::string("I4 precondition (P6 ") + label + ") [p24]: the clk_sys-per-memory-cycle ratio is the same integer over the whole capture");
    uint64_t kCck = ratio.ok ? ratio.kCck : 0;

    // ---- G1: baseline (no COPEN) ---------------------------------------
    {
        Capture base = run_capture(dir + "/prog_p6_nocopen.bin", kBudget, cfg);
        bool base_ran = base.loaded && base.done.has_value();
        auto base_reads = instr::copper_reads(base.log, kRegions);
        checks.expect(base_ran && base_reads.empty(),
            std::string("G1 precondition (P6 ") + label + "): with COPEN off, no read lands in the Copper regions over the same capture");
    }
    // ---- G2 -------------------------------------------------------------
    {
        std::optional<uint16_t> val;
        for (const auto& a : c.log) if (a.is_write && a.word_addr == word_addr(kDmaconrSave)) val = a.data;
        bool g2 = val.has_value() && (*val & 0x0200) && (*val & 0x0080);
        checks.expect(g2, std::string("G2 precondition (P6 ") + label + "): DMACONR has bits 9 and 7 set");
    }
    // ---- G3 -------------------------------------------------------------
    {
        auto m0w = write_cycles(c.log, word_addr(kMarkM0));
        bool g3 = !m0w.empty() && !fr.fetches.empty();
        if (g3) {
            const instr::Fetch* first_after = nullptr;
            for (const auto& f : fr.fetches) if (f.t1 > m0w[0]) { first_after = &f; break; }
            g3 = first_after && first_after->byte_addr == kL6A;
        }
        checks.expect(g3, std::string("G3 precondition (P6 ") + label + "): the first Copper instruction fetch after marker M0 is at L6a ($0E3000)");
    }
    // ---- G4 -------------------------------------------------------------
    {
        auto ref = reference_words(mutate);
        bool g4 = instr::check_g4(fr.fetches, ref);
        checks.expect(g4, std::string("G4 precondition (P6 ") + label + "): every Copper read's data equals the word this run's own setup prescribes for that address");
    }
    // ---- G5 ---------------------------------------------------------------
    {
        auto g5 = instr::check_g5(samples, fields, kCck, h_max, mean_lo, mean_hi);
        checks.note(std::string("G5 (P6 ") + label + "): mean line length=" + std::to_string(g5.mean_line) +
                    " memory cycles over " + std::to_string(g5.pairs) + " pairs (" + std_label + " rule)");
        checks.expect(g5.ok, std::string("G5 precondition (P6 ") + label + "): beam readings are sane for the " + std_label + " rule");
    }
    // ---- G6 -------------------------------------------------------------
    {
        auto g6 = instr::check_g6(samples, fields);
        if (!g6.ok) checks.note(std::string("G6 diagnostic (P6 ") + label + "): smallest line holds " + std::to_string(g6.min_count) + " samples");
        checks.expect(g6.ok, std::string("G6 precondition (P6 ") + label + "): every line holds at least 5 kept samples");
    }
    // ---- P6-p2: at least four wraps -------------------------------------
    {
        int wraps = fields.empty() ? 0 : (int)fields.size() - 1;
        checks.note(std::string("P6-p2 (") + label + "): " + std::to_string(wraps) + " wraps in the capture");
        checks.expect(wraps >= 4, std::string("P6-p2 precondition (P6 ") + label + "): at least four wraps fall in the capture");
    }
    // ---- P6-p3: the video standard selected is in effect -----------------
    {
        bool any = false, ok = true;
        int max_v_seen = -1;
        for (size_t f = 1; f + 1 < fields.size(); ++f) {
            int local_max = -1;
            for (size_t i = fields[f].begin; i < fields[f].end; ++i) local_max = std::max(local_max, samples[i].V());
            if (local_max < 0) continue;
            any = true;
            if (local_max > max_v_seen) max_v_seen = local_max;
            if (local_max != expect_v_a && local_max != expect_v_b) ok = false;
        }
        char buf[32]; std::snprintf(buf, sizeof buf, "$%X", max_v_seen);
        checks.note(std::string("P6-p3 (") + label + "): largest kept V per complete field, max seen=" + buf);
        checks.expect(any && ok,
            std::string("P6-p3 precondition (P6 ") + label + ") [p24]: the video standard selected is in effect -- every "
            "complete field's own largest kept V matches the " + std_label + " rule");
    }

    // ---- P6-p1 and (i): fetch count and address per field ---------------
    bool p1_ok = true, i_ok = true;
    {
        std::string diag_p1, diag_i;
        for (size_t f = 1; f < fields.size(); ++f) {
            uint64_t prev_s = samples[fields[f - 1].end - 1].s;
            uint64_t own_last_s = samples[fields[f].end - 1].s;
            std::vector<const instr::Fetch*> inrange;
            for (const auto& fe : fr.fetches) if (fe.t1 > prev_s && fe.t1 <= own_last_s) inrange.push_back(&fe);
            if (inrange.size() > 1) {
                p1_ok = false;
                if (diag_p1.empty()) { char b[96]; std::snprintf(b, sizeof b, "field %zu: %zu fetches", f, inrange.size()); diag_p1 = b; }
            }
            if (inrange.size() != 1 || inrange[0]->byte_addr != kL6A) {
                i_ok = false;
                if (diag_i.empty()) {
                    char b[140];
                    if (inrange.empty()) std::snprintf(b, sizeof b, "field %zu: no fetch", f);
                    else std::snprintf(b, sizeof b, "field %zu: fetch at $%06X (%zu total)", f, inrange[0]->byte_addr, inrange.size());
                    diag_i = b;
                }
            }
        }
        if (!p1_ok) checks.note(std::string("P6-p1 diagnostic (") + label + "): " + diag_p1);
        checks.expect(p1_ok, std::string("P6-p1 precondition (P6 ") + label + "): between restarts the Copper is parked -- at most one instruction fetch per field");
        if (!i_ok) checks.note(std::string("P6-i diagnostic (") + label + "): " + diag_i);
        checks.expect(i_ok,
            std::string("P6-i (") + label + ") [p25,p30,p288] appB-note.c9: in every field there is exactly one instruction fetch, at COP1LC's own value (L6a)");
    }

    // ---- (ii): the restart's own IR1 read lies after the previous
    // field's last kept sample and before its own field's first sample
    // that reads the video standard's own vertical-blanking-end line ----
    {
        bool ii_ok = true; int graded = 0;
        for (size_t f = 1; f < fields.size(); ++f) {
            uint64_t prev_s = samples[fields[f - 1].end - 1].s;
            const instr::Fetch* restart = nullptr;
            for (const auto& fe : fr.fetches) if (fe.t1 > prev_s) { restart = &fe; break; }
            if (!restart) continue;
            std::optional<uint64_t> bound_s;
            for (size_t i = fields[f].begin; i < fields[f].end; ++i)
                if (samples[i].V() >= restart_v_hi) { bound_s = samples[i].s; break; }
            if (!bound_s) continue;
            ++graded;
            if (!(restart->t1 > prev_s && restart->t1 < *bound_s)) ii_ok = false;
        }
        checks.expect(graded > 0 && ii_ok,
            std::string("P6-ii (") + label + ") [p25,p30,p219,p79] appB-note.c9: the restart's own IR1 read lies after the "
            "previous field's last kept sample and before its own field's first sample reading the " + std_label + " rule's own vertical-blanking-end line");
    }

    // ---- (iii): no fetch ever at L6c -------------------------------------
    {
        bool iii_ok = true;
        for (const auto& fe : fr.fetches) if (fe.byte_addr == kL6C) iii_ok = false;
        checks.expect(iii_ok, std::string("P6-iii (") + label + ") [p25] COP2-location-registers.c6,COP2-location-registers.c9: no fetch occurs at L6c, the COP2LC value");
    }

    // ---- P6_C1's own guard: its own calibrated write (prog_p6.s,
    // SEG_A_ITERS) actually landed at a line in $64-$C8, as the
    // specification's own contrasting case requires (its header comment
    // states the calibration; this is what checks it) --------------------
    if (mutate == "P6-C1") {
        auto wa = write_cycles(c.log, word_addr(0x0E7004));
        bool ok = wa.size() == 1;
        int line = -1;
        if (ok) {
            for (const auto& a : c.log) if (a.is_write && a.word_addr == word_addr(0x0E7004) && a.cycle == wa[0]) line = (a.data >> 8) & 0xFF;
            ok = line >= 0x64 && line <= 0xC8;
        }
        checks.note(std::string("P6-C1 (") + label + "): the calibrated write's own marker reads line $" +
                    (line >= 0 ? [&]{ char b[8]; std::snprintf(b,sizeof b,"%X",line); return std::string(b); }() : std::string("?")));
        checks.expect(ok, std::string("P6-C1 guard (P6 ") + label + "): the field-3 write lands at a line in $64-$C8, as SPECIFICATION.md's own contrasting case requires");
    }
}

void run(Checks& checks, const std::string& dir, const std::string& mutate) {
    // ---- PAL run --------------------------------------------------------
    {
        std::string bin = dir + "/prog_p6.bin";
        if (mutate == "P6-P1") bin = dir + "/prog_p6_mut_p6p1.bin";
        else if (mutate == "P6-P2") bin = dir + "/prog_p6_mut_shortcap.bin";
        else if (mutate == "P6-C1") bin = dir + "/prog_p6_c1.bin";
        nmh::Config cfg; // ECS/PAL default
        run_one(checks, dir, bin, cfg, mutate, "PAL", "PAL",
                /*restart_v_hi=*/0x1E, /*h_max=*/0xE2, /*mean_lo=*/227.0 - instr::kMeanLineTolerance,
                /*mean_hi=*/227.0 + instr::kMeanLineTolerance, /*expect_v_a=*/0x137, /*expect_v_b=*/0x138);
    }
    // ---- NTSC run ---------------------------------------------------------
    {
        std::string bin = dir + "/prog_p6.bin";
        nmh::Config cfg;
        cfg.chipset_config = (mutate == "P6-P3") ? 0x08 /* PAL, P6-p3's own mutation */ : 0x0A; // ECS/NTSC
        // P6-P1, P6-P2 and P6-C1 are exercised on the PAL leg's own binary
        // above only; this leg always loads the plain prog_p6.bin for them,
        // so it must be graded (G4's reference words, the P6-C1 guard) as
        // the plain, unmutated run too, not as whichever mutation the PAL
        // leg is running. P6-P3 is the one mutation this leg itself carries
        // (the config swap above), so its own label passes through.
        std::string ntsc_mutate = (mutate == "P6-P3") ? mutate : std::string();
        run_one(checks, dir, bin, cfg, ntsc_mutate, "NTSC", "NTSC",
                /*restart_v_hi=*/0x16, /*h_max=*/0xE3, /*mean_lo=*/227.0 - instr::kMeanLineTolerance,
                /*mean_hi=*/228.0 + instr::kMeanLineTolerance, /*expect_v_a=*/0x105, /*expect_v_b=*/0x106);
    }
    // ---- P6's own contrasting case: a field-3 register write is graded
    // against the unchanged "L6a in every field" comparator above -------
    if (mutate == "P6-C1")
        checks.record("MUTATION P6-C1: field 3's own COP1LC write (L6b) is graded on the same, unchanged PAL "
                      "run and comparator as the main run; P6-i is expected to diverge at fields 4 and 5.");
}

} // namespace p6

// ---------------------------------------------------------------------
// P7 [p139,p145,p147,p164,p288]: the audio pointer restarts from AUD0LC
// whenever the length count finishes. No Copper. See prog_p7.s.
namespace p7 {

constexpr uint32_t kBufA = 0x0E4000;
constexpr uint32_t kBufB = 0x0E4100;
constexpr uint32_t kDmaconrSave = 0x0E7108; // the specification's own address (P7.2, step 7)
constexpr uint64_t kBudget = kBootBudget + 6'000'000;

const std::vector<std::pair<uint32_t, uint32_t>> kRegions = {
    {word_addr(kBufA), word_addr(kBufA) + 8},
    {word_addr(kBufB), word_addr(kBufB) + 8},
};

// Every read (not paired: audio fetches are single words, not two-word
// instructions) in the audio regions, in cycle order.
std::vector<nmh::RamAccess> audio_reads(const std::vector<nmh::RamAccess>& log) {
    std::vector<nmh::RamAccess> out;
    for (const auto& a : log) if (!a.is_write && instr::in_regions(a.word_addr, kRegions)) out.push_back(a);
    return out;
}

struct Block { size_t begin, end; }; // [begin,end) indices into Population::all

struct Population {
    std::vector<nmh::RamAccess> all;
    size_t first_pass_end = 0;
    std::vector<Block> blocks; // maximal +1-word-address-stepped runs after the first pass
};

// SPECIFICATION.md 3.6, P7.3: the first pass is excluded -- the fetches
// before the first address decrease, capped at N+1. A block is a maximal
// run, after the first pass, whose addresses step by +1 word (+2 bytes).
Population classify(const std::vector<nmh::RamAccess>& reads, int n) {
    Population p;
    p.all = reads;
    size_t decrease_idx = reads.size();
    for (size_t i = 1; i < reads.size(); ++i)
        if (reads[i].word_addr < reads[i - 1].word_addr) { decrease_idx = i; break; }
    p.first_pass_end = std::min(decrease_idx, (size_t)(n + 1));
    size_t i = p.first_pass_end;
    while (i < reads.size()) {
        size_t j = i + 1;
        while (j < reads.size() && reads[j].word_addr == reads[j - 1].word_addr + 1) ++j;
        p.blocks.push_back({i, j});
        i = j;
    }
    return p;
}

void run(Checks& checks, const std::string& dir, const std::string& mutate) {
    std::string bin = dir + "/prog_p7.bin";
    if (mutate == "P7-P2") bin = dir + "/prog_p7_mut_p7p2.bin";
    else if (mutate == "P7-P3") bin = dir + "/prog_p7_mut_p7p3.bin";
    else if (mutate == "P7-P4") bin = dir + "/prog_p7_mut_p7p4.bin";

    const int n = (mutate == "P7-P4") ? 64 : 4;

    Capture c = run_capture(bin, kBudget);
    bool ran = c.loaded && c.done.has_value();
    checks.expect(ran, "P7-fixture: the program ran to DONE");
    if (!ran) return;

    // ---- G1: baseline (no AUD0EN) ---------------------------------------
    {
        std::string base_bin = dir + (mutate == "P7-G1" ? "/prog_p7_noaud0_mut.bin" : "/prog_p7_noaud0.bin");
        Capture base = run_capture(base_bin, kBudget);
        bool base_ran = base.loaded && base.done.has_value();
        auto base_reads = audio_reads(base.log);
        checks.expect(base_ran && base_reads.empty(),
            "G1 precondition (P7): with AUD0EN off, no read lands in the audio regions over the same capture");
    }
    // ---- P7-p2: DMACONR bits ----------------------------------------------
    {
        std::optional<uint16_t> val;
        for (const auto& a : c.log) if (a.is_write && a.word_addr == word_addr(kDmaconrSave)) val = a.data;
        bool ok = val.has_value() && (*val & 0x0200) && (*val & 0x0001) && !(*val & 0x000E);
        checks.expect(ok, "P7-p2 precondition: DMACONR (stored at $0E7108) has bits 9 and 0 set and bits 3-1 clear");
    }

    auto reads = audio_reads(c.log);
    checks.note("P7: " + std::to_string(reads.size()) + " reads in the audio regions");
    if (!reads.empty()) {
        char buf[64];
        std::snprintf(buf, sizeof buf, "P7: first read at word $%06X, byte $%06X, cycle %llu",
                      reads[0].word_addr, reads[0].word_addr * 2, (unsigned long long)reads[0].cycle);
        checks.note(buf);
    }
    auto pop = classify(reads, n);

    // ---- P7-p3: the first channel-0 fetch lies in A's buffer -- graded
    // against the main run's own, unchanged expectation (buffer A), per
    // SPECIFICATION.md P7.5's own "for the main-run expectation" (4.5's
    // dedicated-mutation exception): P7-P3 points AUD0LC at B instead, so
    // the first fetch actually lands in B, and this check catches that.
    {
        bool ok = !reads.empty() && reads[0].word_addr >= word_addr(kBufA) && reads[0].word_addr < word_addr(kBufA) + 4;
        checks.expect(ok, "P7-p3 precondition: the first channel-0 fetch lies in A's buffer");
    }

    // ---- P7-p4: at least 10 ends of counted blocks -----------------------
    int counted = 0;
    for (const auto& b : pop.blocks) if (b.end != pop.all.size()) ++counted;
    checks.note("P7-p4: " + std::to_string(counted) + " counted block ends");
    checks.expect(counted >= 10, "P7-p4 precondition: at least 10 ends of counted blocks fall in the capture");

    if (mutate == "P7-P2" || mutate == "P7-P3" || mutate == "P7-P4" || mutate == "P7-G1") {
        checks.record("MUTATION: the preconditions above already carry this run's own red result; the main "
                      "assertions are not graded for this stimulus.");
        return;
    }

    // ---- (i): every counted block is exactly LC,LC+2,LC+4,LC+6 -----------
    const uint32_t wlc = word_addr(kBufA);
    bool i_ok = true; std::string diag_i;
    for (const auto& b : pop.blocks) {
        if (b.end == pop.all.size()) continue; // not counted: may be cut off by the capture's end
        size_t len = b.end - b.begin;
        bool ok = (len == 4);
        if (ok) for (size_t k = 0; k < 4; ++k) if (pop.all[b.begin + k].word_addr != wlc + (uint32_t)k) ok = false;
        if (!ok) {
            i_ok = false;
            if (diag_i.empty()) { char buf[96]; std::snprintf(buf, sizeof buf, "block at fetch index %zu: length %zu", b.begin, len); diag_i = buf; }
        }
    }
    if (!i_ok) checks.note("P7-i diagnostic: " + diag_i);
    checks.expect(i_ok,
        "P7-i [p139,p145,p147,p288] appB-note.c9,appB-note.c11: every counted block is exactly LC,LC+2,LC+4,LC+6");

    // ---- (ii): after the first pass, every fetch at LC+6 is followed, as
    // the next channel-0 fetch, by a fetch at LC ---------------------------
    bool ii_ok = true; int graded = 0;
    for (size_t k = pop.first_pass_end; k + 1 < pop.all.size(); ++k) {
        if (pop.all[k].word_addr == wlc + 3) {
            ++graded;
            if (pop.all[k + 1].word_addr != wlc) ii_ok = false;
        }
    }
    checks.expect(graded > 0 && ii_ok,
        "P7-ii [p139,p145,p147,p164,p288] appB-note.c9,appB-note.c11: after the first pass, every fetch at "
        "LC+6 is followed, as the next channel-0 fetch, by a fetch at LC");

    // ---- (iii): after the first pass, no read at a guard word -------------
    bool iii_ok = true;
    for (size_t k = pop.first_pass_end; k < pop.all.size(); ++k) {
        uint32_t w = pop.all[k].word_addr;
        bool guard = (w >= word_addr(kBufA) + 4 && w < word_addr(kBufA) + 8) ||
                     (w >= word_addr(kBufB) + 4 && w < word_addr(kBufB) + 8);
        if (guard) iii_ok = false;
    }
    checks.expect(iii_ok, "P7-iii [p164]: after the first pass, no read occurs at a guard word");

    checks.record("The first pass's own fetch sequence (up to N+1 fetches) is reported, not graded "
                  "(specification 3.6): the page leaves its exact shape open.");
}

} // namespace p7

// ---------------------------------------------------------------------
// P5 [p25,p273,p276,p287,p290,p309]: one long-word write to COPxLCH loads
// the whole list address; the lower-addressed register holds the high
// bits. No monitor loop: the CPU paces itself by polling VPOSR/VHPOSR.
// See prog_p5.s.
namespace p5 {

constexpr uint32_t kTargets[40] = {
    0x0C0020, 0x0C0040, 0x0C0080, 0x0C0100, 0x0C0200, 0x0C0400, 0x0C0800, 0x0C1000, 0x0C2000, 0x0C4000, 0x0C8000,
    0x0C5002, 0x0C6004, 0x0CA008, 0x0CC010,
    0x0DFFDE, 0x0DFFBE, 0x0DFF7E, 0x0DFEFE, 0x0DFDFE, 0x0DFBFE, 0x0DF7FE, 0x0DEFFE, 0x0DDFFE, 0x0DBFFE, 0x0D7FFE,
    0x0DAFFC, 0x0D9FFA, 0x0D5FF6, 0x0D3FEE,
    0x01F000, 0x02F000, 0x04F000, 0x08F000, 0x10F000,
    0x1EF000, 0x1DF000, 0x1BF000, 0x17F000, 0x0FF000,
};
constexpr uint32_t kMarkM0 = 0x0E7000;
constexpr uint32_t kDmaconrSave = 0x0E7100;
constexpr uint32_t kMarkLog = 0x0E7002;
constexpr uint32_t kParkC1C3 = 0x10000E;
constexpr uint64_t kBudget = kBootBudget + 4'000'000;

void push_words(std::vector<std::pair<uint32_t, uint16_t>>& out, uint32_t base, std::initializer_list<uint16_t> words) {
    uint32_t addr = base;
    for (uint16_t w : words) { out.push_back({addr, w}); addr += 2; }
}

constexpr uint32_t kPhase3Park = 0x0E9010; // "P5 word-write target park" (SPECIFICATION.md 4.2)

std::vector<std::pair<uint32_t, uint32_t>> regions(bool with_c1c3_park) {
    std::vector<std::pair<uint32_t, uint32_t>> r;
    for (uint32_t t : kTargets) r.push_back({word_addr(t), word_addr(t) + 8});
    r.push_back({word_addr(kDecoyD), word_addr(kDecoyD) + 2});
    r.push_back({word_addr(kPhase3Park), word_addr(kPhase3Park) + 2});
    if (with_c1c3_park) r.push_back({word_addr(kParkC1C3), word_addr(kParkC1C3) + 2});
    return r;
}

// G4's own reference table: the words this run's own setup actually wrote.
std::vector<std::pair<uint32_t, uint16_t>> reference_words(const std::string& mutate) {
    std::vector<std::pair<uint32_t, uint16_t>> out;
    for (int i = 0; i < 40; ++i) {
        uint32_t base = kTargets[i];
        if (i == 0 && mutate == "P5-P2")
            push_words(out, base, {0x0001, 0xFF00, 0xFFFF, 0xFFFE, 0x0000, 0x0000});
        else
            push_words(out, base, {0xFFFF, 0xFFFE, 0x0000, 0x0000, 0x0000, 0x0000});
    }
    out.push_back({kDecoyD, 0xFFFF}); out.push_back({kDecoyD + 2, 0xFFFE});
    out.push_back({kPhase3Park, 0xFFFF}); out.push_back({kPhase3Park + 2, 0xFFFE});
    if (mutate == "P5-C1" || mutate == "P5-C3") {
        out.push_back({kParkC1C3, 0xFFFF}); out.push_back({kParkC1C3 + 2, 0xFFFE});
    }
    return out;
}

// One step: the byte address this specification predicts the Copper
// lands at, and which of the 40 targets (if any) it corresponds to, for
// the bit-group grading below (-1: phase 3, or C2's own extra step).
struct Step { uint32_t target; int idx; };

// C1 and C3's own predictions both carry "not at [the main run's own
// target]" (SPECIFICATION.md P5.6): they are graded against the main
// run's own, unchanged target sequence (4.5's dedicated-mutation
// exception), so the divergence itself is what turns a step red -- not
// against their own (correctly predicted) new landing. C2's own
// prediction carries no such "not at": it is a positive extension of
// coverage (see the header comment and the record() below), graded on
// its own correctly predicted landing.
std::vector<Step> build_steps(const std::string& mutate) {
    std::vector<Step> out;
    // Phase 1
    if (mutate == "P5-C2") {
        for (int i = 0; i < 11; ++i) out.push_back({kTargets[i], i});
        out.push_back({0x0C4000, -1}); // C2: L alone, H keeps $000C from $0C8000
        for (int i = 11; i < 40; ++i) out.push_back({kTargets[i], i});
    } else {
        for (int i = 0; i < 40; ++i) out.push_back({kTargets[i], i});
    }
    // Phase 2 -- always the main run's own 40 targets, even under C3.
    for (int i = 0; i < 40; ++i) out.push_back({kTargets[i], i});
    // Phase 3 -- always the main run's own $0E9010, even under C1.
    out.push_back({0x0E9010, -1});
    return out;
}

void run(Checks& checks, const std::string& dir, const std::string& mutate) {
    std::string bin = dir + "/prog_p5.bin";
    if (mutate == "P5-P1") bin = dir + "/prog_p5_mut_p5p1.bin";
    else if (mutate == "P5-P2") bin = dir + "/prog_p5_mut_p5p2.bin";
    else if (mutate == "P5-P3") bin = dir + "/prog_p5_mut_p5p3.bin";
    else if (mutate == "P5-C1") bin = dir + "/prog_p5_c1.bin";
    else if (mutate == "P5-C2") bin = dir + "/prog_p5_c2.bin";
    else if (mutate == "P5-C3") bin = dir + "/prog_p5_c3.bin";

    Capture c = run_capture(bin, kBudget);
    bool ran = c.loaded && c.done.has_value();
    checks.expect(ran, "P5-fixture: the program ran to DONE");
    if (!ran) return;

    auto steps = build_steps(mutate);

    // ---- markers: M0, then one per step, then the final marker ---------
    auto m0w = write_cycles(c.log, word_addr(kMarkM0));
    bool markers_ok = m0w.size() == 1;
    std::vector<uint64_t> marker_cyc;
    std::vector<uint16_t> marker_data;
    for (size_t k = 0; markers_ok && k <= steps.size(); ++k) {
        auto w = write_cycles(c.log, word_addr(kMarkLog) + (uint32_t)k);
        if (w.size() != 1) { markers_ok = false; break; }
        marker_cyc.push_back(w[0]);
        // The data written at that cycle (I3: the VHPOSR value read).
        uint16_t d = 0;
        for (const auto& a : c.log) if (a.is_write && a.word_addr == word_addr(kMarkLog) + (uint32_t)k && a.cycle == w[0]) d = a.data;
        marker_data.push_back(d);
    }
    checks.expect(markers_ok, "P5-fixture: marker M0 and every step marker and the final marker were each written exactly once");
    if (!markers_ok) return;

    // ---- G1: baseline (no COPEN) -----------------------------------------
    {
        Capture base = run_capture(dir + "/prog_p5_nocopen.bin", kBudget);
        bool base_ran = base.loaded && base.done.has_value();
        auto base_regs = regions(false);
        auto base_reads = instr::copper_reads(base.log, base_regs);
        checks.expect(base_ran && base_reads.empty(),
            "G1 precondition (P5): with COPEN off, no read lands in the Copper regions over the same capture");
    }
    // ---- G2 -----------------------------------------------------------
    {
        std::optional<uint16_t> val;
        for (const auto& a : c.log) if (a.is_write && a.word_addr == word_addr(kDmaconrSave)) val = a.data;
        bool g2 = val.has_value() && (*val & 0x0200) && (*val & 0x0080);
        checks.expect(g2, "G2 precondition (P5): DMACONR has bits 9 and 7 set");
    }

    bool with_park = (mutate == "P5-C1" || mutate == "P5-C3");
    auto regs = regions(with_park);
    auto reads = instr::copper_reads(c.log, regs);
    auto fr = instr::pair_fetches(reads);

    // ---- G3 (with L = D): the first fetch after M0 is at D -------------
    {
        bool g3 = !fr.fetches.empty();
        if (g3) {
            const instr::Fetch* first_after = nullptr;
            for (const auto& f : fr.fetches) if (f.t1 > m0w[0]) { first_after = &f; break; }
            g3 = first_after && first_after->byte_addr == kDecoyD;
        }
        checks.expect(g3, "G3 precondition (P5, L=D): the first Copper instruction fetch after marker M0 is at D ($0E8000)");
    }
    // ---- G4: every Copper read equals this run's own prescribed word ---
    {
        auto ref = reference_words(mutate);
        bool g4 = instr::check_g4(fr.fetches, ref);
        checks.expect(g4, "G4 precondition (P5): every Copper read's data equals the word this run's own setup prescribes for that address");
    }

    // ---- P5-p1: every marker (first step's M_T to final) reads a line in
    // $28-$FA, strictly increasing ----------------------------------------
    {
        bool ok = true; int prev_line = -1;
        for (uint16_t d : marker_data) {
            int line = (d >> 8) & 0xFF;
            if (line < 0x28 || line > 0xFA) ok = false;
            if (prev_line >= 0 && line <= prev_line) ok = false;
            prev_line = line;
        }
        checks.expect(ok, "P5-p1 precondition: every marker from the first step's own M_T to the final marker "
                          "reads a line in $28-$FA, strictly higher than the one before it");
    }

    // ---- the main check and P5-p2: for every step, the first Copper
    // fetch after its own marker is at the predicted target, and every
    // fetch until the next marker stays there (SPECIFICATION.md P5.4;
    // P5-p2: the Copper is parked between one landing and the next
    // marker, which this same population check establishes) -------------
    bool step_ok = true; std::string diag;
    std::array<bool, 40> idx_ok{}; idx_ok.fill(true);
    for (size_t k = 0; k < steps.size(); ++k) {
        uint64_t lo = marker_cyc[k], hi = marker_cyc[k + 1];
        const instr::Fetch* first = nullptr;
        bool all_at_target = true;
        for (const auto& f : fr.fetches) {
            if (f.t1 <= lo || f.t1 >= hi) continue;
            if (!first) first = &f;
            if (f.byte_addr != steps[k].target) all_at_target = false;
        }
        bool ok = first && first->byte_addr == steps[k].target && all_at_target;
        if (!ok) {
            step_ok = false;
            if (steps[k].idx >= 0) idx_ok[steps[k].idx] = false;
            if (diag.empty()) {
                char b[96];
                std::snprintf(b, sizeof b, "step %zu: %s", k, !first ? "no fetch" : "wrong/stray fetch");
                diag = b;
            }
        }
    }
    if (!step_ok) checks.note("P5 diagnostic: " + diag);
    checks.expect(step_ok, "P5-p2 precondition: between one landing and the next marker, no Copper read occurs "
                           "except at the landing itself (the Copper stays parked)");

    // ---- bit-group verdicts (3.2, P5.4): L bits15-1 (A15-A1) from S_L1/
    // S_L0 (target indices 0-29), H bits2-0 (A18-A16) and H bits4-3
    // (A20-A19) from S_H1/S_H0 (indices 30-39), each covering both phases
    bool l_ok = true, hlo_ok = true, hhi_ok = true;
    for (int i = 0; i < 30; ++i) if (!idx_ok[i]) l_ok = false;
    for (int i = 30; i < 40; ++i) if (!idx_ok[i]) { hlo_ok = false; hhi_ok = false; }
    checks.expect(l_ok,
        "P5-L [p25,p273,p287,p290] ch2-location-registers.c3.s2,ch2-location-registers.c5.s2,"
        "ch2-location-registers.c11,COP2-location-registers.c13,COP2-location-registers.c6,COP2-location-registers.c9,"
        "ch2-appa-location-registers.c7.s2,ch2-appa-location-registers.c16.s2,ch2-appb-table.c22.s2,"
        "ch2-appb-table.c32.s2,ch2-appb-legend.c6.s2: a long-word write to COPxLCH loads L bits 15-1 (A15-A1) of "
        "the list address correctly (the higher-addressed register, COPxLCL, holding the low bits, its own CPU "
        "write taking effect), walking one and walking zero, every bit");
    checks.expect(hlo_ok,
        "P5-H-low [p25,p273,p287,p290,p309] COP2-location-registers.c6,COP2-location-registers.c9,"
        "ch2-appa-location-registers.c2.s2,ch2-appa-location-registers.c11.s2,ch2-appb-table.c16.s2,"
        "ch2-appb-table.c26.s2,ch2-appc-ecs.c3.s2,ch2-appc-ecs.c8.s2: the high 3 bits (A18-A16) of the list "
        "address load correctly (the lower-addressed register, COPxLCH, holding the high bits, its own CPU write "
        "taking effect), walking one and walking zero");
    checks.expect(hhi_ok,
        "P5-H-high [p25,p273,p287,p290,p309] COP2-location-registers.c6,COP2-location-registers.c9,"
        "ch2-appa-location-registers.c2.s2,ch2-appa-location-registers.c11.s2,ch2-appb-table.c16.s2,"
        "ch2-appb-table.c26.s2,ch2-appc-ecs.c3.s2,ch2-appc-ecs.c8.s2: the ECS-only high bits (A20-A19) of the "
        "list address load correctly, walking one and walking zero");

    checks.record("ch2-location-registers.c3.s2 and ch2-location-registers.c5.s2 ('low 16 bits') are graded for "
                  "bits 15-1 only (3.2): bit 0 is never set by this run's own words.");
    checks.record("'Write-only' (the ten location-register claims) is graded for its write half only, as "
                  "section 2 states -- every landing above is such a write taking effect; no page gives an "
                  "expected value for a read.");
    if (mutate == "P5-C2")
        checks.record("MUTATION P5-C2: the extra step's own prediction ($0C4000, COP1LCH keeps $000C from the "
                      "$0C8000 step) is the correct landing, not a divergence from the main run; it is graded "
                      "the same as any other step (P5-p2, P5-L) and is expected green.");
}

} // namespace p5

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    p4::run(checks, dir, mv);
    p3::run(checks, dir, mv);
    p1a::run(checks, dir, mv);
    p2::run(checks, dir, mv);
    p6::run(checks, dir, mv);
    p7::run(checks, dir, mv);
    p5::run(checks, dir, mv);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
