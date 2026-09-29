// test_ch8_light_pen_beam.cpp
//
// Integration package ch8-light-pen-beam: see SPECIFICATION.md. Program
// prog.s (and its stimulus variants) drive nanomig.v only through
// nanomig_harness.h's ports (Config, load_program/boot/run_until_write,
// peek_logical(), ram_log(), video(), ports()) and are graded only
// against what those ports showed.
//
// Design. The program's phase A never receives an input change in any
// run this file makes (SPECIFICATION.md section 6): every schedule
// leaves field A alone. That lets one pass fit the beam model from the
// run's own phase-A samples (harvested up to the M1 marker) and then
// compute every later pulse's target cycle from that same model before
// injecting it -- SPECIFICATION.md 5.6 ("It may take the model from a
// first run without input changes"; a run's own phase A already is one).
//
// Every stimulus mutation below is one line per (assertion or
// precondition, NMH_MUTATE value): the label passed to expect(), the
// value that selects it, and what the stimulus changes. Where more than
// one mutation targets the same label, each gets its own line. Every
// core's own replay result, and the clause it goes red through, is this
// package's report, not this file (SPECIFICATION.md records only
// per-core measurements, not process history).
//
// MUTATION CP1(A1) stimulus red NMH_MUTATE=K17: a pin-6 pulse (both ports) added at (200,100) in every phase-A field
// MUTATION CP1(A2) stimulus red NMH_MUTATE=K17: same stimulus -- the largest read V per field is pinned at 200, not L
// MUTATION CP1(A2) stimulus red NMH_MUTATE=K15: the harness Config is switched to NTSC while graded with the PAL run's own expectations
// MUTATION CP2 stimulus red NMH_MUTATE=K5: the phase-B sampling loop gets a delay of at least 300 colour clocks per sample
// MUTATION CP3 stimulus red NMH_MUTATE=K11: pin 6 of both ports held closed from before boot through one field after M0, then released
// MUTATION CP4 stimulus red NMH_MUTATE=K12: every pulse placed one line later than SPECIFICATION.md section 6 says
// MUTATION CP5 stimulus red NMH_MUTATE=K13: the phase-B buffer shortened so phase B ends within field B19
// MUTATION CP6 stimulus red NMH_MUTATE=K14: VHPOSW written once (line 100, H 0) directly after the wait150 that ends phase B
// MUTATION P1 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000 (bit 3 never set)
// MUTATION P1 stimulus red NMH_MUTATE=K9b: B4's and B5's pin-6 pulses moved to pin 9 of the same port
// MUTATION P2(i) stimulus red NMH_MUTATE=K9: B18's pin-1 pulse (line 40) replaced by a pin-6 pulse
// MUTATION P2(ii) stimulus red NMH_MUTATE=K9b: B18's pin-6 pulse moved to pin 9
// MUTATION P2(ii) stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P3(a) stimulus red NMH_MUTATE=K16: the B11 pulse moved to (41,100) instead of (40,100)
// MUTATION P3(a) stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P3(b) stimulus red NMH_MUTATE=K16: the B7 pulse moved to (100,122) instead of (100,102)
// MUTATION P3(b) stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P3(c) stimulus red NMH_MUTATE=K16: the B7 pulse moved to (100,122) instead of (100,102)
// MUTATION P3(c) stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P4 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P5 stimulus red NMH_MUTATE=K8: B14's first pulse (line 80) removed
// MUTATION P5 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P6 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P7 stimulus red NMH_MUTATE=K2: pin-6 pulses (both ports) added at (100,100) in B2 and B3
// MUTATION P7 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P7(c) unfalsifiable at this boundary: every latch the pages describe comes after the last current read of its field, so no input makes the pages predict m(F_k) < m(R_k) (SPECIFICATION.md section 8, P7)
// MUTATION P8 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P8 stimulus red NMH_MUTATE=K2: pin-6 pulses (both ports) added at (100,100) in B2 and B3
// MUTATION P9 stimulus red NMH_MUTATE=K6: the line-10 pulse of B15 and the line-19 pulse of B17 moved to line 30
// MUTATION P9 stimulus red NMH_MUTATE=K7: the line-32 pulses of B15 and B17 moved to line 12
// MUTATION P9 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P10(a) stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P10(b) stimulus red NMH_MUTATE=K3: phase C's BPLCON0 write uses $0008 instead of $0000
// MUTATION P10(b) stimulus red NMH_MUTATE=K4: phase A's BPLCON0 write uses $0008 instead of $0000
// MUTATION P11 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P11 stimulus red NMH_MUTATE=K2: pin-6 pulses (both ports) added at (100,100) in B2 and B3
// MUTATION P12 stimulus red NMH_MUTATE=K1: phase B's BPLCON0 write stays $0000
// MUTATION P12 stimulus red NMH_MUTATE=K2: pin-6 pulses (both ports) added at (100,100) in B2 and B3
// MUTATION P13(port1) stimulus red NMH_MUTATE=K10: phase D drives pin 9 (not pin 5) in the windows the main run drives pin 5
// MUTATION P13(port1) stimulus red NMH_MUTATE=K10b: phase D holds port-1 pin 5 low throughout instead of only 2-4ms
// MUTATION P13(pin9-control) unfalsifiable at this boundary: no input this harness exposes reaches port 2's pin 5 (POTGOR bit 12) at all, so there is no stimulus whose mutated outcome the pages predict differently for this control
// MUTATION FIXTURE unfalsifiable at this boundary: the fixture asserts the program-stored M0 marker before any other observation is made; there is no earlier point in the program for NMH_MUTATE to corrupt it from

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <numeric>
#include <algorithm>
#include <optional>
#include <functional>
#include <memory>
#include <iostream>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- the assertion call form is fixed by BRIEF.md ----------------------
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
Checks checks;

const char* g_mutate = std::getenv("NMH_MUTATE");

// ---- program-side constants (mirrors prog.s / nanomig.i) --------------
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint32_t kCodeLo    = 0x080400, kCodeHi = 0x080600; // word-addr range holds code bytes
constexpr uint32_t kMark      = 0x100000;
constexpr uint16_t kM0 = 0xA000, kM1 = 0xA001, kM2 = 0xA002, kM3 = 0xA003, kM4 = 0xA004;
constexpr uint32_t kBufAcount = 22000, kBufBcount = 90000, kBufBcountK13 = 60000, kBufBcountK5 = 2000, kBufCcount = 13000, kBufDcount = 3500;
constexpr uint32_t kBufA = 0x100010;
constexpr uint32_t kBufAEnd = kBufA + 4 * kBufAcount;
constexpr uint32_t kBufB = kBufAEnd;
uint32_t g_bufBcount = kBufBcount; // set per-run (K13 shortens it)
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 20'000'000;

// joystick / mouse bit masks. The physical port each nanomig.v input
// reaches, and which POTGOR/CIAAPRA bit each pot line and fire button
// lands on, is measured at the ports (SPECIFICATION.md section 3 "Port
// mapping"): joystick0 reaches port 2, joystick1 reaches port 1.
constexpr uint8_t kBitFire  = 0x10; // pin 6 (beam trigger)
constexpr uint8_t kBitFire2 = 0x20; // pin 9
constexpr uint8_t kBitUp    = 0x08; // pin 1
constexpr uint8_t kBitDown  = 0x04; // pin 2
constexpr uint8_t kBitLeft  = 0x02; // pin 3
constexpr uint8_t kBitRight = 0x01; // pin 4
constexpr uint8_t kMouseMid = 0x04; // mouse_buttons bit 2: port-1 pin 5 pot line

// ---- Config ------------------------------------------------------------
nmh::Config make_config(bool ecs, bool ntsc) {
    nmh::Config c;
    c.chipset_config = (ecs ? 0x08 : 0x00) | (ntsc ? 0x02 : 0x00);
    return c;
}

// ---- one Sample of VPOSR:VHPOSR (long) or POTGOR (word) ---------------
struct Sample {
    int64_t a = 0, b = 0; // read-instant bounds, clk_sys cycles
    int V = 0, H = 0;     // decoded position (long samples)
    uint16_t raw16 = 0;   // decoded POTGOR word (word samples)
    uint32_t raw32 = 0;
};

// Finds, for a target cycle `b`, the last chip-RAM read in [lo,hi) before it
// -- the earlier bound `a` (SPECIFICATION.md 5.2's "otherwise" branch).
int64_t last_code_read_before(const std::vector<int64_t>& sorted_code_reads, int64_t b) {
    auto it = std::upper_bound(sorted_code_reads.begin(), sorted_code_reads.end(), b - 1);
    if (it == sorted_code_reads.begin()) return b; // degenerate: no fetch seen, a==b
    return *std::prev(it);
}

std::vector<int64_t> code_read_cycles(const std::vector<nmh::RamAccess>& log) {
    std::vector<int64_t> out;
    for (const auto& a : log) if (!a.is_write && a.word_addr >= kCodeLo/2 && a.word_addr < kCodeHi/2) out.push_back((int64_t)a.cycle);
    std::sort(out.begin(), out.end());
    return out;
}

// Extracts long (VPOSR:VHPOSR) samples written into [buf_word_lo,buf_word_hi).
std::vector<Sample> extract_long_samples(const std::vector<nmh::RamAccess>& log,
                                          const std::vector<int64_t>& coderd,
                                          uint32_t buf_byte_lo, uint32_t buf_byte_hi) {
    uint32_t lo = buf_byte_lo / 2, hi = buf_byte_hi / 2;
    std::map<uint32_t, std::pair<int64_t, uint16_t>> hi_w, lo_w;
    for (const auto& acc : log) {
        if (!acc.is_write || acc.word_addr < lo || acc.word_addr >= hi) continue;
        uint32_t idx = acc.word_addr - lo;
        if (idx % 2 == 0) hi_w[idx] = {(int64_t)acc.cycle, acc.data};
        else lo_w[idx] = {(int64_t)acc.cycle, acc.data};
    }
    std::vector<Sample> out;
    size_t n = (hi - lo) / 2;
    for (size_t i = 0; i < n; ++i) {
        auto ih = hi_w.find((uint32_t)(2 * i));
        auto il = lo_w.find((uint32_t)(2 * i + 1));
        if (ih == hi_w.end() || il == lo_w.end()) break;
        int64_t b = std::min(ih->second.first, il->second.first);
        Sample s;
        s.b = b;
        s.a = last_code_read_before(coderd, b);
        s.raw32 = (uint32_t(ih->second.second) << 16) | il->second.second;
        s.V = int(((s.raw32 >> 16) & 1) << 8 | ((s.raw32 >> 8) & 0xFF));
        s.H = int(s.raw32 & 0xFF);
        out.push_back(s);
    }
    return out;
}

// Extracts word (POTGOR) samples written into [buf_word_lo,buf_word_hi).
std::vector<Sample> extract_word_samples(const std::vector<nmh::RamAccess>& log,
                                          const std::vector<int64_t>& coderd,
                                          uint32_t buf_byte_lo, uint32_t buf_byte_hi) {
    uint32_t lo = buf_byte_lo / 2, hi = buf_byte_hi / 2;
    std::map<uint32_t, std::pair<int64_t, uint16_t>> w;
    for (const auto& acc : log) {
        if (!acc.is_write || acc.word_addr < lo || acc.word_addr >= hi) continue;
        w[acc.word_addr - lo] = {(int64_t)acc.cycle, acc.data};
    }
    std::vector<Sample> out;
    size_t n = hi - lo;
    for (size_t i = 0; i < n; ++i) {
        auto it = w.find((uint32_t)i);
        if (it == w.end()) break;
        Sample s;
        s.b = it->second.first;
        s.a = last_code_read_before(coderd, s.b);
        s.raw16 = it->second.second;
        out.push_back(s);
    }
    return out;
}

// ---- colour-clock grid (SPECIFICATION.md 5.2) --------------------------
int64_t derive_colour_clock_len(const std::vector<nmh::RamAccess>& log, int64_t upto_cycle) {
    std::vector<int64_t> cyc;
    for (const auto& a : log) { if ((int64_t)a.cycle > upto_cycle) break; cyc.push_back((int64_t)a.cycle); }
    std::sort(cyc.begin(), cyc.end());
    cyc.erase(std::unique(cyc.begin(), cyc.end()), cyc.end());
    int64_t g = 0;
    for (size_t i = 1; i < cyc.size(); ++i) {
        int64_t d = cyc[i] - cyc[i - 1];
        if (d <= 0) continue;
        g = g ? std::gcd(g, d) : d;
    }
    return g > 0 ? g : 8;
}

// ---- the beam model (SPECIFICATION.md 5.3-5.5) --------------------------
struct BeamModel {
    int64_t colour_clock_len = 8;
    int64_t line_len_cck = 0;
    int L = 0; // last model line; field has L+1 lines (0..L)
    int64_t field_len_clksys = 0;
    int64_t anchor_cycle = 0; // clk_sys at which model V=0,H=0
    int64_t u_cck = 0;        // model uncertainty (max b-a among fit samples), in CCK
    bool ok = false;

    int64_t field_start(int64_t field_idx) const { return anchor_cycle + field_idx * field_len_clksys; }
    // floor-divide, correct for negative numerators
    static int64_t fdiv(int64_t n, int64_t d) { int64_t q = n / d, r = n % d; if (r != 0 && ((r < 0) != (d < 0))) --q; return q; }
    static int64_t fmod(int64_t n, int64_t d) { int64_t r = n % d; if (r != 0 && ((r < 0) != (d < 0))) r += d; return r; }
    int64_t field_index_of(int64_t cycle) const { return fdiv(cycle - anchor_cycle, field_len_clksys); }
    // model (V,H) at cycle t
    void pos_at(int64_t t, int* Vout, int* Hout) const {
        int64_t within = fmod(t - anchor_cycle, field_len_clksys);
        int64_t line_len_clk = line_len_cck * colour_clock_len;
        int64_t line = within / line_len_clk;
        int64_t rem = within % line_len_clk;
        int64_t h = rem / colour_clock_len;
        if (Vout) *Vout = int(line);
        if (Hout) *Hout = int(h);
    }
    int64_t cycle_of(int64_t field_idx, int line, int H) const {
        return field_start(field_idx) + (int64_t)line * line_len_cck * colour_clock_len + (int64_t)H * colour_clock_len;
    }
    // "current": does (V,H) equal the model position for some t in [a,b]?
    bool is_current(const Sample& s) const {
        for (int64_t t = s.a; t <= s.b; t += std::max<int64_t>(1, colour_clock_len)) {
            int mv, mh; pos_at(t, &mv, &mh);
            if (mv == s.V && mh == s.H) return true;
        }
        int mv, mh; pos_at(s.b, &mv, &mh);
        return mv == s.V && mh == s.H;
    }
    // torn (SPECIFICATION.md 5.3): a sample whose two register reads
    // straddle a V8 change combines V8 of one line with V7-V0 of another.
    // These changes happen at model line 255 to 256 and at L to 0. A
    // sample whose [a-2, b] (colour clocks) contains such a change is
    // exempt. The margin is the fixed two colour clocks the page's own
    // register pair spans (VPOSR then VHPOSR), not the model's fitted
    // uncertainty u -- u bounds the read instant within a sample, not how
    // far a V8 change can sit from the reported bound `a`.
    bool is_torn(const Sample& s) const {
        int64_t t0 = s.a - 2 * colour_clock_len, t1 = s.b;
        int v0, v1; pos_at(t0, &v0, nullptr); pos_at(t1, &v1, nullptr);
        if (v0 != v1) return true; // crossed a line boundary (255/256 or L/0) within [a-2,b]
        // Direct value check for the same thing 5.3 describes ("combine V8
        // of one line with V7-V0 of another"): if the raw reading matches
        // no model position in [a,b], but the reading with V8 flipped
        // does, the two register reads straddled exactly that V8 change --
        // regardless of how the margin above placed the boundary.
        if (!is_current(s)) {
            Sample flipped = s;
            flipped.V ^= 256;
            if (is_current(flipped)) return true;
        }
        return false;
    }
};

int mode_of(std::vector<int64_t>& v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    int64_t best = v[0]; size_t bestCount = 1, curCount = 1;
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i] == v[i - 1]) ++curCount; else curCount = 1;
        if (curCount > bestCount) { bestCount = curCount; best = v[i]; }
    }
    return (int)best;
}

// video vs-edge field length (undoubled; nanomig_harness.h find_frames()).
int64_t field_len_from_video(const std::vector<nmh::VideoSample>& vid) {
    auto spans = nmh::find_frames(vid);
    std::vector<int64_t> lens;
    for (size_t i = 1; i + 1 < spans.size(); ++i) // skip first/last partial
        lens.push_back((int64_t)(spans[i].end - spans[i].begin));
    if (lens.empty()) return 0;
    return mode_of(lens);
}

// Fits the beam model from phase-A samples (bit 3 clear, running counter).
//
// The colour-clock length is derived directly from its own definition: the
// clk_sys span between two same-line samples divided by the read H they
// differ by (H is already in colour-clock units at the ports). This is
// more robust than a GCD over every RAM-port access, which also contains
// finer sub-colour-clock timing (the port's own sampling grid is twice as
// fine as one colour clock) and so under-measures the period.
BeamModel fit_model(const std::vector<Sample>& phaseA, const std::vector<nmh::RamAccess>& log,
                     const std::vector<nmh::VideoSample>& vid, bool ntsc) {
    BeamModel m;
    if (phaseA.size() < 20) return m;

    // split into fields by V decreasing
    std::vector<int> fieldOf(phaseA.size(), 0);
    int fid = 0;
    for (size_t i = 1; i < phaseA.size(); ++i) {
        if (phaseA[i].V < phaseA[i - 1].V) ++fid;
        fieldOf[i] = fid;
    }
    int nfields = fid + 1;
    std::vector<int> Lfield(nfields, 0);
    for (size_t i = 0; i < phaseA.size(); ++i) Lfield[fieldOf[i]] = std::max(Lfield[fieldOf[i]], phaseA[i].V);
    // canonical L: mode across "complete" fields (all but first & last)
    std::vector<int64_t> Lcandidates;
    for (int f = 1; f < nfields - 1; ++f) Lcandidates.push_back(Lfield[f]);
    if (Lcandidates.empty()) for (int f = 0; f < nfields; ++f) Lcandidates.push_back(Lfield[f]);
    m.L = mode_of(Lcandidates);

    int Vrun = ntsc ? 21 : 29;

    // colour_clock_len: dV==0 (same line) consecutive pairs, dividing the
    // clk_sys span by the read H delta.
    std::vector<int64_t> cckEst;
    for (size_t i = 1; i < phaseA.size(); ++i) {
        if (fieldOf[i] != fieldOf[i - 1]) continue;
        if (phaseA[i].V != phaseA[i - 1].V) continue;
        int64_t dH = phaseA[i].H - phaseA[i - 1].H;
        if (dH <= 0) continue;
        int64_t dclk = phaseA[i].a - phaseA[i - 1].a;
        if (dclk % dH != 0) continue; // keep only clean divisions
        int64_t est = dclk / dH;
        if (est > 0 && est < 100) cckEst.push_back(est);
    }
    m.colour_clock_len = cckEst.empty() ? derive_colour_clock_len(log, phaseA.back().b) : mode_of(cckEst);

    m.field_len_clksys = field_len_from_video(vid);
    if (m.field_len_clksys > 0 && m.L > 0 && m.field_len_clksys % (int64_t)(m.L + 1) == 0) {
        // exact line length in clk_sys from the two most reliable
        // independent measurements (vsync-derived field length, and the
        // real largest read line per field).
        int64_t line_len_clksys_exact = m.field_len_clksys / (m.L + 1);
        if (m.colour_clock_len > 0 && line_len_clksys_exact % m.colour_clock_len == 0)
            m.line_len_cck = line_len_clksys_exact / m.colour_clock_len;
    }
    if (m.line_len_cck <= 0) {
        // fallback: dV==1 pairs.
        std::vector<int64_t> lenEst;
        for (size_t i = 1; i < phaseA.size(); ++i) {
            if (fieldOf[i] != fieldOf[i - 1]) continue;
            int dV = phaseA[i].V - phaseA[i - 1].V;
            if (dV != 1) continue;
            if (phaseA[i - 1].V < Vrun || phaseA[i - 1].V > m.L - 1) continue;
            int64_t dcck = (phaseA[i].a - phaseA[i - 1].a) / m.colour_clock_len;
            int64_t est = dcck - (phaseA[i].H - phaseA[i - 1].H);
            if (est > 0 && est < 1000) lenEst.push_back(est);
        }
        if (lenEst.empty()) return m;
        m.line_len_cck = mode_of(lenEst);
    }
    if (m.field_len_clksys <= 0) m.field_len_clksys = (int64_t)(m.L + 1) * m.line_len_cck * m.colour_clock_len;

    // anchor from fit samples in [Vrun,L-1]
    std::vector<int64_t> anchors;
    int64_t maxUb = 0;
    for (size_t i = 0; i < phaseA.size(); ++i) {
        if (phaseA[i].V < Vrun || phaseA[i].V > m.L - 1) continue;
        int64_t est = phaseA[i].a - ((int64_t)phaseA[i].V * m.line_len_cck + phaseA[i].H) * m.colour_clock_len;
        anchors.push_back(est);
        maxUb = std::max(maxUb, phaseA[i].b - phaseA[i].a);
    }
    if (anchors.empty()) return m;
    // mode of anchors rounded to nearest field_len (robust central estimate)
    std::sort(anchors.begin(), anchors.end());
    m.anchor_cycle = anchors[anchors.size() / 2]; // median
    m.u_cck = (maxUb + m.colour_clock_len - 1) / m.colour_clock_len;
    m.ok = true;
    return m;
}

// ---- event-driven stimulus injection ------------------------------------
struct Event { int64_t cycle; std::function<void()> apply; };

struct PortState {
    uint8_t joy0 = 0, joy1 = 0, mbtn = 0;
    void push(nmh::NanomigHarness& h) {
        h.ports()->joystick0 = joy0;
        h.ports()->joystick1 = joy1;
        h.ports()->mouse_buttons = mbtn;
    }
};

// ---- microsecond -> colour clock (SPECIFICATION.md 5.7) -----------------
int64_t us_to_cck(double us, bool ntsc) {
    double period = ntsc ? 0.27936 : 0.28194;
    return (int64_t)std::ceil(us / period);
}

// ---- schedule building ----------------------------------------------------
// field_offset is relative to B0 (phase-B start field). PulseDef itself is
// defined in test_ch8_light_pen_beam_part2.inc, alongside the pulse
// catalogue that uses it.
void add_pulse_events(std::vector<Event>& ev, const BeamModel& m, int64_t B0_idx, PortState* state,
                       int field_offset, int line, int H, bool port0, bool port1, uint8_t bit) {
    int64_t t_on = m.cycle_of(B0_idx + field_offset, line, H);
    int64_t t_off = t_on + 4 * m.colour_clock_len;
    ev.push_back({t_on, [state, port0, port1, bit]() {
        if (port0) state->joy0 |= bit;
        if (port1) state->joy1 |= bit;
    }});
    ev.push_back({t_off, [state, port0, port1, bit]() {
        if (port0) state->joy0 &= (uint8_t)~bit;
        if (port1) state->joy1 &= (uint8_t)~bit;
    }});
}

// Pin 5 (the pen-pressed / middle-mouse-button pot line) has no digital
// joystick bit; port 1's is reached through mouse_buttons bit 2, measured
// at POTGOR bit 8 (SPECIFICATION.md section 3, "Port mapping"). Driving
// every other input this harness exposes during phase D and watching
// POTGOR (as that measurement does) finds none that reaches port 2's pin
// 5 (POTGOR bit 12), so only port 1 is pulsed here.
void add_mouse_pulse_events(std::vector<Event>& ev, const BeamModel& m, int64_t B0_idx, PortState* state,
                             int field_offset, int line, int H) {
    int64_t t_on = m.cycle_of(B0_idx + field_offset, line, H);
    int64_t t_off = t_on + 4 * m.colour_clock_len;
    ev.push_back({t_on, [state]() { state->mbtn |= kMouseMid; }});
    ev.push_back({t_off, [state]() { state->mbtn &= (uint8_t)~kMouseMid; }});
}

// One delivered input change, for CP4's own record: the cycle it was
// scheduled for and the cycle at which it was actually applied (the
// harness is stepped cycle-exactly to the scheduled cycle before the
// port word changes, so these are expected to be equal; the record
// exists so CP4 can assert that, not assume it).
struct AppliedEvent { int64_t sched, applied; uint8_t j0, j1, mb; };
std::vector<AppliedEvent> g_applied;

// A run's full result: log, model, field anchors, per-phase samples.
struct RunResult {
    bool loaded = false, released = false, done_ok = false;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::VideoSample> vid;
    BeamModel model;
    int64_t B0_idx = 0, C0_idx = 0, M3cyc = 0;
    std::vector<Sample> A, B, C, D;
    bool ntsc = false;
    uint8_t fire_byte = 0xFF;
    int64_t m0cyc = -1, m1cyc = -1, m2cyc = -1, m3cyc = -1, m4cyc = -1;
    std::vector<AppliedEvent> applied; // this run's own delivery record (CP4)
};

constexpr uint32_t kFireWordAddr = 0x100004 / 2;
int64_t find_mark_cycle(const std::vector<nmh::RamAccess>& log, uint16_t value) {
    uint32_t wa = kMark / 2;
    for (const auto& a : log) if (a.is_write && a.word_addr == wa && a.data == value) return (int64_t)a.cycle;
    return -1;
}

// Callback types: given the model + a field/cycle anchor + PortState, push
// this phase's Events (phase A is never touched by any schedule).
using ScheduleBFn = std::function<void(const BeamModel&, int64_t B0_idx, std::vector<Event>&, PortState*)>;
using ScheduleCFn = std::function<void(const BeamModel&, int64_t C0_idx, std::vector<Event>&, PortState*)>;
using ScheduleDFn = std::function<void(const BeamModel&, int64_t M3cyc, std::vector<Event>&, PortState*)>;

void run_phase_events(nmh::NanomigHarness& h, std::vector<Event>& events, PortState& state) {
    std::sort(events.begin(), events.end(), [](const Event& x, const Event& y) { return x.cycle < y.cycle; });
    for (auto& e : events) {
        int64_t now = (int64_t)h.cycles();
        int64_t delta = e.cycle - now;
        if (delta > 0) h.run_cycles((uint64_t)delta);
        e.apply();
        state.push(h);
        g_applied.push_back({e.cycle, (int64_t)h.cycles(), state.joy0, state.joy1, state.mbtn});
    }
}

RunResult run_one(const std::string& bin, bool ecs, bool ntsc, uint32_t bufBcount,
                   const ScheduleBFn& schedB, const ScheduleCFn& schedC, const ScheduleDFn& schedD) {
    RunResult r;
    r.ntsc = ntsc;
    g_applied.clear();
    nmh::Config cfg = make_config(ecs, ntsc);
    auto h = std::make_unique<nmh::NanomigHarness>(cfg);
    size_t loaded = h->load_program(bin);
    r.loaded = loaded > 0;
    if (!r.loaded) return r;
    auto boot = h->boot(kBootBudget);
    r.released = boot.cpu_released;
    if (!r.released) { r.log = h->ram_log(); return r; }

    // Run through phase A: stop when M1 (end of phase A) is written.
    auto m1cyc = h->run_until_write(kMark, kM1, kRunBudget);
    if (!m1cyc) { r.log = h->ram_log(); return r; }

    auto coderd = code_read_cycles(h->ram_log());
    uint32_t bufAend = kBufA + 4 * kBufAcount;
    r.A = extract_long_samples(h->ram_log(), coderd, kBufA, bufAend);
    r.model = fit_model(r.A, h->ram_log(), h->video(), ntsc);
    r.B0_idx = r.model.ok ? r.model.field_index_of((int64_t)*m1cyc) : 0;

    PortState state;
    if (r.model.ok) {
        std::vector<Event> evB;
        schedB(r.model, r.B0_idx, evB, &state);
        run_phase_events(*h, evB, state);
    }

    auto m2cyc = h->run_until_write(kMark, kM2, kRunBudget);
    if (!m2cyc) { r.log = h->ram_log(); r.vid = h->video(); return r; }
    r.C0_idx = r.model.ok ? r.model.field_index_of((int64_t)*m2cyc) : 0;

    if (r.model.ok) {
        std::vector<Event> evC;
        schedC(r.model, r.C0_idx, evC, &state);
        run_phase_events(*h, evC, state);
    }

    auto m3cyc = h->run_until_write(kMark, kM3, kRunBudget);
    r.M3cyc = m3cyc ? (int64_t)*m3cyc : 0;

    if (r.model.ok && m3cyc) {
        std::vector<Event> evD;
        schedD(r.model, r.M3cyc, evD, &state);
        run_phase_events(*h, evD, state);
    }

    // SPECIFICATION.md's program (section 4) ends "move.w #M4,MARK" then
    // ".stop bra.s .stop": an infinite loop, with no DONE call. CP5's own
    // completion criterion is "M0 to M4 appear in order", so M4 being
    // written is this package's completion signal.
    auto m4cyc = h->run_until_write(kMark, kM4, kRunBudget);
    r.done_ok = m4cyc.has_value();
    r.log = h->ram_log();
    r.vid = h->video();

    auto coderd2 = code_read_cycles(r.log);
    r.A = extract_long_samples(r.log, coderd2, kBufA, bufAend);
    uint32_t bufBend = kBufB + 4 * bufBcount;
    uint32_t bufC = bufBend;
    uint32_t bufCend = bufC + 4 * kBufCcount;
    uint32_t bufD = bufCend;
    uint32_t bufDend = bufD + 2 * kBufDcount;
    r.B = extract_long_samples(r.log, coderd2, kBufB, bufBend);
    r.C = extract_long_samples(r.log, coderd2, bufC, bufCend);
    r.D = extract_word_samples(r.log, coderd2, bufD, bufDend);
    if (auto w = h->peek_logical(0x100004)) r.fire_byte = uint8_t((*w >> 8) & 0xFF);
    r.m0cyc = find_mark_cycle(r.log, kM0);
    r.m1cyc = find_mark_cycle(r.log, kM1);
    r.m2cyc = find_mark_cycle(r.log, kM2);
    r.m3cyc = find_mark_cycle(r.log, kM3);
    r.m4cyc = find_mark_cycle(r.log, kM4);
    r.applied = g_applied;
    return r;
}

// convenience: does the sample list contain > 0 entries with expected structure
bool ok_run(const RunResult& r) { return r.loaded && r.released && r.done_ok && r.model.ok; }

} // namespace

// ===========================================================================
// Preconditions, propositions, main runs and mutation dispatch.
// ===========================================================================
#include "test_ch8_light_pen_beam_part2.inc"

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    if (g_mutate) {
        run_mutation(dir, g_mutate);
    } else {
        run_main(dir);
    }
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
