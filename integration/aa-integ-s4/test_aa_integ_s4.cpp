// test_aa_integ_s4.cpp
//
// aa-integ-s4: see SPECIFICATION.md. Bit-plane fetch window (DDFSTRT, DDFSTOP),
// display window (DIWSTRT, DIWSTOP, DIWHIGH) and DMA control (DMACON, DMACONR),
// graded at the ports of nanomig.v only.
//
// nanomig.mk builds the TG68K core with ENABLE_AGA and the fx68k core without
// it: every proposition and precondition is graded (PASS:/FAIL:) on the build made
// with ENABLE_AGA only; on the other build the identical check prints as RECORDED:
// (Grader, below), never as PASS or FAIL.
//
// One program (prog_s4) interprets the command list that the test places in Chip
// memory before the program starts; the list is the stimulus of a configuration.
//
// NMH_MUTATE selects one mutation in place of the normal run: it names the label
// of one assertion or precondition (the first token of its expect label), and only
// the proposition that owns it runs, with a changed stimulus and the unchanged
// comparator. The normal run (NMH_MUTATE unset) never runs a mutant.
// NMH_ONLY=Pn runs the proposition Pn alone, without a mutation.
//
// MUTATION_RECORDS_BEGIN
// MUTATION_RECORDS_END

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- the memory layout of SPECIFICATION.md 2.3 ------------------------------
constexpr uint32_t kProg = 0x080400, kProgEnd = 0x08C000;
constexpr uint32_t kCop = 0x08C000, kCopEnd = 0x08C800;
constexpr uint32_t kSprBase = 0x090000, kSprStep = 0x1000, kSprLen = 40;
constexpr uint32_t kBltS = 0x098000, kBltSEnd = 0x09A000;
constexpr uint32_t kBltD = 0x09C000, kBltDEnd = 0x09E000;
constexpr uint32_t kScr = 0x0A0000, kScrEnd = 0x0A0000 + 40000;
constexpr uint32_t kB1 = 0x0B0000, kB2 = 0x0C8000, kB3 = 0x0D1000, kB4 = 0x0DA000, kZ = 0x0E3000;
constexpr uint32_t kPlaneLen = 0x8000;
constexpr uint32_t kMark = 0x0C0000, kMarkEnd = 0x0C0800;
constexpr uint32_t kMarkIdx = 0x0C0000, kMarkFix = 0x0C0002, kMarkEndWord = 0x0C0004, kMarkDma = 0x0C0100;
constexpr uint32_t kSamp = 0x0C2000, kSampEnd = 0x0C6000;
constexpr uint32_t kParam = 0x0F0000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint16_t kFixValue = 0xBEEF;
constexpr uint16_t kEndMark = 0xE0E0;

// custom registers (offsets from 0xDFF000), register list p.6-p.10
enum : uint16_t {
    DMACONR = 0x002, BLTCON0 = 0x040, BLTCON1 = 0x042, BLTAFWM = 0x044, BLTALWM = 0x046,
    BLTAPTH = 0x050, BLTAPTL = 0x052, BLTDPTH = 0x054, BLTDPTL = 0x056, BLTSIZE = 0x058,
    BLTAMOD = 0x064, BLTDMOD = 0x066, COP1LCH = 0x080, COP1LCL = 0x082, COPJMP1 = 0x088,
    DIWSTRT = 0x08E, DIWSTOP = 0x090, DDFSTRT = 0x092, DDFSTOP = 0x094, DMACON = 0x096,
    BPL1PTH = 0x0E0, BPL1PTL = 0x0E2, BPL2PTH = 0x0E4, BPL2PTL = 0x0E6, BPL3PTH = 0x0E8,
    BPL3PTL = 0x0EA, BPL4PTH = 0x0EC, BPL4PTL = 0x0EE, SPR0PTH = 0x120,
    BPLCON0 = 0x100, BPLCON1 = 0x102, BPLCON2 = 0x104, BPLCON3 = 0x106, BPL1MOD = 0x108,
    BPL2MOD = 0x10A, BPLCON4 = 0x10C, COLOR00 = 0x180, COLOR01 = 0x182, BEAMCON0 = 0x1DC,
    DIWHIGH = 0x1E4, FMODE = 0x1FC
};

constexpr uint64_t kBootBudget = 8'000'000;

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
bool g_graded = true;        // both cores run the AA chip set (chipset_config 0x10) and are graded
std::string g_dir = ".";
const char* g_mutate = nullptr;

// The label of a line printed as RECORDED on the build without ENABLE_AGA
// carries no claim id: the claim is asserted on the other build's lines.
std::string without_ids(const std::string& s) {
    std::string out, tok;
    auto flush = [&]() {
        if (tok.rfind("opus:", 0) == 0 || tok.rfind("sol:", 0) == 0) {
            if (!tok.empty() && tok.back() == ':') out += ":";
        } else out += tok;
        tok.clear();
    };
    for (char c : s) {
        if (c == ' ') { flush(); out += ' '; }
        else tok += c;
    }
    flush();
    return out;
}

// Every proposition and precondition goes through Grader: PASS:/FAIL: on the
// graded build, RECORDED: on the other, whatever the condition shows.
class Grader {
public:
    void expect(bool cond, const std::string& label) {
        if (g_graded) checks.expect(cond, label);
        else checks.record(without_ids(label) + " [build without ENABLE_AGA: would have " +
                           (cond ? "PASSed" : "FAILed") + "]");
    }
    // A line whose precondition does not hold is NOT GRADED, never PASS or FAIL.
    void expect_if(bool pre, bool cond, const std::string& label) {
        if (pre) expect(cond, label);
        else checks.record("NOT GRADED: " + without_ids(label) + " [a precondition does not hold]");
    }
};
Grader g;

bool mutating(const char* label) { return g_mutate && std::string(g_mutate) == label; }
bool mutating_any(std::initializer_list<const char*> l) {
    for (auto s : l) if (mutating(s)) return true;
    return false;
}

// Runs the proposition `prop` in a normal run, and in a mutation run only when
// the mutation names one of its labels (the label is the proposition's own name,
// or begins with it followed by '-').
bool want(const char* prop) {
    const char* only = std::getenv("NMH_ONLY");
    if (only && std::string(only) != prop) return false;
    if (!g_mutate) return true;
    std::string m = g_mutate, p = prop;
    // the joint assertions run the propositions they join
    if (m == "J1" && (p == "P1" || p == "P2")) return true;
    if (m == "J2" && (p == "P8" || p == "P9")) return true;
    if (m == "J3" && (p == "P10" || p == "P11")) return true;
    if (m == "RF-RUNS" && (p == "P1" || p == "P2")) return true;
    if (m == "CONFIG" && p == "P17") return true;
    return m == p || m.compare(0, p.size() + 1, p + "-") == 0;
}

std::string hexs(uint32_t v, int digits = 6) {
    char b[16];
    std::snprintf(b, sizeof b, "%0*X", digits, v);
    return b;
}
std::string dec(long long v) { return std::to_string(v); }
void dbg(const std::string& s) { if (std::getenv("NMH_DEBUG")) std::cout << "DEBUG " << s << std::endl; }

// ---- the command list -------------------------------------------------------

struct Script {
    std::vector<uint16_t> w;
    void W(uint16_t reg, uint16_t val)  { w.insert(w.end(), {1, reg, val}); }
    void WVB()                          { w.push_back(2); }
    void WL30()                         { w.push_back(3); }
    void MARK(uint16_t idx)             { w.insert(w.end(), {4, idx}); }
    void CPYD()                         { w.push_back(5); }
    void SAMPLE(uint16_t n)             { w.insert(w.end(), {6, n}); }
    void SSTORE(uint16_t v)             { w.insert(w.end(), {7, v}); }
    void STORE(uint32_t a, uint16_t v)  { w.insert(w.end(), {8, uint16_t(a >> 16), uint16_t(a), v}); }
    void COPYW(uint32_t a, const std::vector<uint16_t>& v) {
        w.insert(w.end(), {9, uint16_t(a >> 16), uint16_t(a), uint16_t(v.size())});
        w.insert(w.end(), v.begin(), v.end());
    }
    void FILLL(uint32_t a, uint16_t nlong, uint32_t first) {
        w.insert(w.end(), {10, uint16_t(a >> 16), uint16_t(a), nlong, uint16_t(first >> 16), uint16_t(first)});
    }
    void RUN20(uint16_t size, uint16_t n) { w.insert(w.end(), {11, size, n}); }
    void WBLT(uint16_t reg, uint16_t val, uint16_t idx)  { w.insert(w.end(), {12, reg, val, idx}); }
    void WSAMP(uint16_t reg, uint16_t val, uint16_t n)   { w.insert(w.end(), {13, reg, val, n}); }
    void ZERO(uint32_t a, uint16_t nlong) { w.insert(w.end(), {14, uint16_t(a >> 16), uint16_t(a), nlong}); }
    void WLN(uint16_t n) { w.insert(w.end(), {15, n}); }
    void END() { w.push_back(0); }
};

// ---- Copper lists (p.20) ----------------------------------------------------

std::vector<uint16_t> cop_move(uint16_t reg, uint16_t data) { return {reg, data}; }
std::vector<uint16_t> cop_wait(int v, int h) { return {uint16_t(((v & 0xFF) << 8) | (h & 0xFE) | 1), 0xFFFE}; }
void append(std::vector<uint16_t>& a, const std::vector<uint16_t>& b) { a.insert(a.end(), b.begin(), b.end()); }

// What the analysis needs to know about a list: the byte offset (from COP) of
// the first word of each instruction it anchors on.
struct CopList {
    std::vector<uint16_t> w;
    uint32_t off_wait[0x100] = {};   // offset of WAIT (v,0x04) for v in the RF-L range
    uint32_t off_w40 = 0, off_wc0 = 0, off_end = 0;
    uint32_t bytes() const { return uint32_t(w.size() * 2); }
};

// The marker block of 2.5.
std::vector<uint16_t> marker_block(uint16_t clear_val = 0x0C00, uint16_t set_val = 0x0C20) {
    std::vector<uint16_t> l;
    append(l, cop_wait(0x20, 0x1C)); append(l, cop_move(BPLCON3, clear_val));
    append(l, cop_wait(0x21, 0x1C)); append(l, cop_move(BPLCON3, set_val));
    append(l, cop_wait(0x24, 0x1C)); append(l, cop_move(BPLCON3, clear_val));
    append(l, cop_wait(0x25, 0x1C)); append(l, cop_move(BPLCON3, set_val));
    return l;
}

// List L-A of 2.5. `planes` is the number of BPLxPT pairs loaded, `base` the
// address they are loaded with, `marker` adds the marker block, `m40`/`mc0`
// the horizontal positions of the two RF-M WAITs, `lh` the horizontal position
// of every RF-L WAIT, `first_lh` the position of the WAIT of line 0x3E only.
struct ListOpts {
    uint32_t base[4] = {kB1, kB2, 0, 0};
    int planes = 1;
    bool marker = false;
    int h_c0 = 0xC0;
    int lh = 0x04;
    std::map<int, int> lh_line;       // per-line override of the RF-L WAIT's position
    uint16_t marker_clear = 0x0C00, marker_set = 0x0C20;
};

CopList list_a(const ListOpts& o) {
    CopList c;
    for (int p = 0; p < o.planes; ++p) {
        append(c.w, cop_move(BPL1PTH + 4 * p, uint16_t(o.base[p] >> 16)));
        append(c.w, cop_move(BPL1PTL + 4 * p, uint16_t(o.base[p])));
    }
    if (o.marker) append(c.w, marker_block(o.marker_clear, o.marker_set));
    c.off_w40 = c.bytes(); append(c.w, cop_wait(0x3C, 0x40));
    c.off_wc0 = c.bytes(); append(c.w, cop_wait(0x3C, o.h_c0));
    for (int v = 0x3E; v <= 0xF2; ++v) {
        c.off_wait[v] = c.bytes();
        auto it = o.lh_line.find(v);
        append(c.w, cop_wait(v, it != o.lh_line.end() ? it->second : o.lh));
    }
    c.off_end = c.bytes();
    c.w.push_back(0xFFFF); c.w.push_back(0xFFFE);
    return c;
}

// List L-V of 2.5.
CopList list_v(uint32_t zbase = kZ) {
    CopList c;
    append(c.w, cop_move(BPL1PTH, uint16_t(zbase >> 16)));
    append(c.w, cop_move(BPL1PTL, uint16_t(zbase)));
    append(c.w, marker_block());
    c.w.push_back(0xFFFF); c.w.push_back(0xFFFE);
    c.off_end = c.bytes() - 4;
    return c;
}

// ---- running a program ------------------------------------------------------

struct Acc { uint64_t cyc; uint32_t byte; bool wr; uint16_t data; };
struct Mark { uint64_t cyc; uint16_t data; };

struct Row { uint64_t cyc = 0; int first_red = -1; int nred = 0, nother = 0; };
struct VFrame { uint64_t cyc = 0; std::vector<Row> rows; };

struct RunData {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<Acc> acc;            // every port access, held bus cycles collapsed
    std::vector<Mark> marks;         // writes to MARK + 0 (the configuration indices)
    std::vector<Acc> samp;           // writes to SAMP
    std::vector<Acc> dmacon_copies;  // writes to MARK + 0x100 and up
    std::vector<Acc> endmarks;       // writes to MARK + 4
    uint64_t period = 0;
    uint64_t ncycles = 0;
    std::optional<uint16_t> fix;     // the word at MARK + 2 at the end of the run
    std::vector<VFrame> frames;
};

nmh::Config g_cfg_used;
bool g_cfg_set = false;

nmh::Config make_config() {
    nmh::Config cfg;
    cfg.chipset_config = 0x10;     // AA selected (bit 4)
    cfg.memory_config  = 0x03;     // 2.0 MB Chip memory
    if (mutating("CONFIG")) cfg.chipset_config = 0x08;
    g_cfg_used = cfg; g_cfg_set = true;
    return cfg;
}

bool config_ok(const nmh::Config& c) { return c.chipset_config == 0x10 && c.memory_config == 0x03; }

int g_runs = 0, g_runs_done = 0;

std::shared_ptr<RunData> run_script(const Script& s, bool video = false, uint64_t budget = 120'000'000) {
    auto r = std::make_shared<RunData>();
    ++g_runs;
    nmh::NanomigHarness h(make_config());
    r->loaded = h.load_program(g_dir + "/prog_s4.bin") > 0;
    if (!r->loaded) return r;
    for (size_t i = 0; i < s.w.size(); ++i) h.poke_word(kParam / 2 + uint32_t(i), s.w[i]);
    r->released = h.boot(kBootBudget).cpu_released;
    r->done = h.run_until_write(kDoneAddr, kDoneValue, budget);
    if (r->done) ++g_runs_done;
    const auto& log = h.ram_log();
    uint64_t period = 0;
    for (size_t i = 1; i < log.size(); ++i) {
        uint64_t d = log[i].cycle - log[i - 1].cycle;
        if (d > 0 && (period == 0 || d < period)) period = d;
    }
    r->period = period;
    std::vector<nmh::RamAccess> col = period ? nmh::coalesce_adjacent(log, period) : log;
    for (const auto& a : col) {
        Acc x{a.cycle, a.word_addr * 2, a.is_write, a.data};
        r->acc.push_back(x);
        if (!x.wr) continue;
        if (x.byte == kMarkIdx) r->marks.push_back({x.cyc, x.data});
        else if (x.byte == kMarkEndWord) r->endmarks.push_back(x);
        else if (x.byte >= kMarkDma && x.byte < kMarkEnd) r->dmacon_copies.push_back(x);
        else if (x.byte >= kSamp && x.byte < kSampEnd) r->samp.push_back(x);
    }
    r->ncycles = h.cycles();
    r->fix = h.peek_logical(kMarkFix);
    if (video) {
        const auto& v = h.video();
        bool pvs = false, phs = false;
        VFrame* fr = nullptr;
        Row* row = nullptr;
        int x = 0;
        for (size_t i = 0; i < v.size(); ++i) {
            const auto& p = v[i];
            if (p.vs && !pvs) { r->frames.emplace_back(); fr = &r->frames.back(); fr->cyc = i; row = nullptr; }
            if (p.hs && !phs && fr) { fr->rows.emplace_back(); row = &fr->rows.back(); row->cyc = i; x = 0; }
            pvs = p.vs; phs = p.hs;
            if (!row) continue;
            bool red = p.r == 0xF && p.g == 0 && p.b == 0;
            bool black = p.r == 0 && p.g == 0 && p.b == 0;
            if (red) { if (row->first_red < 0) row->first_red = x; ++row->nred; }
            else if (!black) ++row->nother;
            ++x;
        }
    }
    return r;
}

// ---- reading the accesses -----------------------------------------------------

// The accesses of `r` with time in [t0,t1) whose byte address lies in [lo,hi)
// and whose direction is `wr`.
std::vector<Acc> accs(const RunData& r, uint64_t t0, uint64_t t1, uint32_t lo, uint32_t hi, bool wr) {
    std::vector<Acc> out;
    auto it = std::lower_bound(r.acc.begin(), r.acc.end(), t0, [](const Acc& a, uint64_t t) { return a.cyc < t; });
    for (; it != r.acc.end() && it->cyc < t1; ++it)
        if (it->wr == wr && it->byte >= lo && it->byte < hi) out.push_back(*it);
    return out;
}
std::vector<Acc> reads(const RunData& r, uint64_t t0, uint64_t t1, uint32_t lo, uint32_t hi) { return accs(r, t0, t1, lo, hi, false); }
std::vector<Acc> writes(const RunData& r, uint64_t t0, uint64_t t1, uint32_t lo, uint32_t hi) { return accs(r, t0, t1, lo, hi, true); }

// 2.3: during [t0,t1) every access in a region has the direction the table gives.
struct Region { uint32_t lo, hi; bool wr; };
bool regions_ok(const RunData& r, uint64_t t0, uint64_t t1, const std::vector<Region>& regs) {
    for (const auto& rg : regs)
        if (!accs(r, t0, t1, rg.lo, rg.hi, !rg.wr).empty()) return false;
    return true;
}

bool fixture_ok(const RunData& r) { return r.loaded && r.released && r.done && r.fix && *r.fix == kFixValue; }

// ---- the start of every program: 2.1 ------------------------------------------

// The program's own first instructions are the clearing writes of 2.1; the
// list starts with the fixture constant.
void script_start(Script& s) {
    s.STORE(kMarkFix, kFixValue);
}

// Fill a bit-plane region with data, written by the CPU before DMA is enabled (2.3).
void script_fill_planes(Script& s, int nplanes, uint16_t nlong = 0x1000) {
    const uint32_t b[4] = {kB1, kB2, kB3, kB4};
    for (int p = 0; p < nplanes; ++p) s.FILLL(b[p], nlong, 0x10001u * (p + 1));
}

// The Copper start of 2.1: COP1LC, its strobe, only then COPEN.
void script_copper_start(Script& s, const CopList& c) {
    s.COPYW(kCop, c.w);
    s.W(COP1LCH, uint16_t(kCop >> 16));
    s.W(COP1LCL, uint16_t(kCop));
    s.W(COPJMP1, 0);
}

// ---- the command list as data: what the program was asked to do ------------------

struct Cmd { uint16_t op; std::vector<uint16_t> a; };

std::vector<Cmd> parse_script(const Script& s) {
    static const int fixed[16] = {0, 2, 0, 0, 1, 0, 1, 1, 3, 3, 5, 2, 3, 3, 3, 1};
    std::vector<Cmd> out;
    size_t i = 0;
    while (i < s.w.size()) {
        Cmd c; c.op = s.w[i++];
        int n = c.op < 16 ? fixed[c.op] : 0;
        if (c.op == 9) { n = 3 + (i + 2 < s.w.size() ? s.w[i + 2] : 0); }
        for (int j = 0; j < n && i < s.w.size(); ++j) c.a.push_back(s.w[i++]);
        out.push_back(c);
        if (c.op == 0) break;
    }
    return out;
}

// Number of writes of `reg` in the list.
int script_writes(const Script& s, uint16_t reg) {
    int n = 0;
    for (const auto& c : parse_script(s)) if ((c.op == 1 || c.op == 12 || c.op == 13) && c.a.size() >= 2 && c.a[0] == reg) ++n;
    return n;
}

// The value the last write of `reg` in the list gives (the list's own record of the stimulus).
std::optional<uint16_t> script_value(const Script& s, uint16_t reg) {
    std::optional<uint16_t> v;
    for (const auto& c : parse_script(s)) if ((c.op == 1 || c.op == 12 || c.op == 13) && c.a.size() >= 2 && c.a[0] == reg) v = c.a[1];
    return v;
}

// 2.3: the program contains no instruction that writes into one of `ranges` after it has enabled DMA.
bool script_clean(const Script& s, const std::vector<std::pair<uint32_t, uint32_t>>& ranges) {
    bool dma = false;
    for (const auto& c : parse_script(s)) {
        if (c.op == 1 && c.a[0] == DMACON && (c.a[1] & 0x8000) && (c.a[1] & 0x0200)) dma = true;
        if (!dma) continue;
        uint32_t a = 0; bool wr = false;
        if ((c.op == 8 || c.op == 9 || c.op == 10 || c.op == 14) && c.a.size() >= 2) { a = (uint32_t(c.a[0]) << 16) | c.a[1]; wr = true; }
        if (c.op == 11) { a = kScr; wr = true; }
        if (!wr) continue;
        for (const auto& rg : ranges) if (a >= rg.first && a < rg.second) return false;
    }
    return true;
}

// ---- S-FETCH runs and the reference frames RF-M, RF-L (2.4, 2.5) -------------------

struct FCfg {
    uint16_t bplcon0 = 0x1000, strt = 0x0038, stop = 0x00D0, fmode = 0, mod1 = 0, mod2 = 0, diwstrt = 0x4081;
    // what the program writes where it differs from the nominal values above
    std::optional<uint16_t> w_strt, w_stop, w_fmode, w_diwstrt, w_bplcon0, w_mod1, w_mod2;
    bool stray_b1 = false;          // Pn-reg contrast: the CPU writes to B1 during this field
    bool patch_wait3e = false;       // RF-L contrast: the CPU rewrites WAIT (0x3E,..) to position 0x0C in this field
};

struct FetchOpts {
    ListOpts list;
    bool fill = true;
    bool dma_copy_first = false;     // the DMACONR copy is stored before the DMACON write
    bool no_wl30 = false;            // the loop leaves out "wait for V7-V0 >= 0x30"
    bool no_fixture = false;
    uint16_t dmacon = 0x8380;
};

struct FField {
    uint64_t m = 0, T0 = 0, T1 = 0;
    bool have = false, single = false;
    uint64_t t40 = 0, tc0 = 0;
    std::map<int, uint64_t> A;       // v -> A(v), v = 0x3E ... 0xF2
    bool cop_ok = false;
    int after_end = 0; uint32_t after_addr = 0;
};

struct FetchRun {
    std::shared_ptr<RunData> r;
    CopList list;
    std::vector<FCfg> cfg;
    std::vector<FField> f;
    bool fix = false;
    // RF-M and RF-L results over the fields of the run
    bool rfm = false, rfl = false, fields_ok = false;
    int64_t P = 0, L = 0, a0 = 0;    // system clock cycles
    uint16_t dmacon_copy = 0;
    Script sc;
};

// Registry for "the same in every field and every run" (2.4).
struct FrameReg { std::string run; int64_t P, L, a0; };
std::vector<FrameReg> g_frames;

Script fetch_script(const std::vector<FCfg>& cfg, const CopList& cl, const FetchOpts& o) {
    Script s;
    if (!o.no_fixture) script_start(s);
    s.W(BEAMCON0, 0x0020);
    int nplanes = (cfg[0].bplcon0 >> 12) & 7;
    if (o.fill) script_fill_planes(s, std::max(1, nplanes));
    s.W(BPLCON0, cfg[0].bplcon0);
    s.W(BPLCON1, 0); s.W(BPLCON2, 0); s.W(BPLCON3, 0x0C00); s.W(BPLCON4, 0x0011);
    s.W(FMODE, 0);
    s.W(DIWSTRT, cfg[0].diwstrt); s.W(DIWSTOP, 0xF0C1);
    s.W(COLOR00, 0);
    s.W(DDFSTRT, cfg[0].strt); s.W(DDFSTOP, cfg[0].stop);
    s.W(BPL1MOD, cfg[0].mod1); s.W(BPL2MOD, cfg[0].mod2);
    script_copper_start(s, cl);
    if (o.dma_copy_first) s.CPYD();
    s.W(DMACON, o.dmacon);
    if (!o.dma_copy_first) s.CPYD();
    s.WL30();
    for (size_t k = 0; k < cfg.size(); ++k) {
        const FCfg& c = cfg[k];
        s.WVB();
        s.W(DDFSTRT, c.w_strt.value_or(c.strt));
        s.W(DDFSTOP, c.w_stop.value_or(c.stop));
        s.W(BPLCON0, c.w_bplcon0.value_or(c.bplcon0));
        s.W(FMODE, c.w_fmode.value_or(c.fmode));
        s.W(BPL1MOD, c.w_mod1.value_or(c.mod1));
        s.W(BPL2MOD, c.w_mod2.value_or(c.mod2));
        s.W(DIWSTRT, c.w_diwstrt.value_or(c.diwstrt));
        if (c.patch_wait3e) s.COPYW(kCop + cl.off_wait[0x3E], {uint16_t((0x3E << 8) | 0x0D)});
        s.MARK(uint16_t(k));
        if (c.stray_b1) s.STORE(kB1 + 0x7000, 0x1234);
        if (!o.no_wl30) s.WL30();
    }
    s.WVB();
    s.END();
    return s;
}

// The configuration's field and its anchors: T0 is the Copper fetch from the start
// of the list that precedes the configuration's store to MARK, T1 the one after it.
FetchRun fetch_analyse(std::shared_ptr<RunData> r, const CopList& cl, const std::vector<FCfg>& cfg, const std::string& name) {
    FetchRun fr;
    fr.r = r; fr.list = cl; fr.cfg = cfg;
    fr.fix = fixture_ok(*r);
    std::vector<uint64_t> c0;
    for (const auto& a : r->acc) if (!a.wr && a.byte == kCop) c0.push_back(a.cyc);
    auto first_read = [&](uint32_t addr, uint64_t t0, uint64_t t1, int& count) {
        uint64_t t = 0; count = 0;
        for (const auto& a : reads(*r, t0, t1, addr, addr + 2)) { if (!count) t = a.cyc; ++count; }
        return t;
    };
    bool all_single = true, all_have = true, all_cop = true;
    for (size_t k = 0; k < cfg.size(); ++k) {
        FField f;
        for (const auto& m : r->marks) if (m.data == k) { f.m = m.cyc; break; }
        if (f.m) {
            auto it = std::upper_bound(c0.begin(), c0.end(), f.m);
            if (it != c0.begin() && it != c0.end()) { f.T0 = *(it - 1); f.T1 = *it; f.have = true; }
        }
        if (f.have) {
            f.single = true;
            int n = 0;
            f.t40 = first_read(kCop + cl.off_wc0, f.T0, f.T1, n); f.single = f.single && n == 1;
            f.tc0 = first_read(kCop + cl.off_wait[0x3E], f.T0, f.T1, n); f.single = f.single && n == 1;
            for (int v = 0x3E; v <= 0xF2; ++v) {
                f.A[v] = first_read(kCop + cl.off_wait[v] + 4, f.T0, f.T1, n);
                f.single = f.single && n == 1;
            }
            // both words of END are fetched; a read after them is measured, not graded
            auto cr = reads(*r, f.T0, f.T1, kCop, kCopEnd);
            f.cop_ok = !cr.empty();
            uint64_t t_end = 0;
            for (const auto& a : cr) if (a.byte == kCop + cl.off_end + 2) t_end = a.cyc;
            if (!t_end) f.cop_ok = false;
            for (const auto& a : cr) if (t_end && a.cyc > t_end) { ++f.after_end; f.after_addr = a.byte; }
        }
        all_have = all_have && f.have;
        all_single = all_single && f.single;
        all_cop = all_cop && f.cop_ok;
        fr.f.push_back(f);
    }
    fr.fields_ok = all_have && all_single;
    // RF-M: tC0 - t40 is a positive multiple of 128 system clock cycles; P equal in every field
    bool rfm = fr.fields_ok;
    bool rfl = fr.fields_ok && all_cop;
    int64_t P = -1, L = -1, a0 = -1;
    for (const auto& f : fr.f) {
        if (!f.have || !f.single) continue;
        int64_t d = int64_t(f.tc0) - int64_t(f.t40);
        if (d <= 0 || d % 128 != 0) rfm = false;
        else { int64_t p = d / 128; if (P < 0) P = p; else if (p != P) rfm = false; }
        int64_t a = int64_t(f.A.at(0x3E)) - int64_t(f.T0);
        if (a0 < 0) a0 = a; else if (a != a0) { rfl = false; dbg("a0 differs " + std::to_string(a) + " vs " + std::to_string(a0)); }
        for (int v = 0x3E; v <= 0xF1; ++v) {
            int64_t l = int64_t(f.A.at(v + 1)) - int64_t(f.A.at(v));
            if (L < 0) L = l; else if (l != L) { rfl = false; dbg("L differs at " + std::to_string(v) + ": " + std::to_string(l) + " vs " + std::to_string(L)); }
        }
        dbg("field m=" + std::to_string(f.m) + " T0=" + std::to_string(f.T0) + " T1=" + std::to_string(f.T1) + " a0=" + std::to_string(a) + " cop_ok=" + std::to_string((int)f.cop_ok));
    }
    fr.rfm = rfm; fr.rfl = rfl;
    fr.P = P; fr.L = L; fr.a0 = a0;
    if (!r->dmacon_copies.empty()) fr.dmacon_copy = r->dmacon_copies[0].data;
    g_frames.push_back({name, P, L, a0});
    return fr;
}

struct LineF { bool fetched = false; int64_t f = 0, l = 0; uint32_t s = 0, e = 0; int n = 0; std::vector<uint32_t> addr; };

// Per line v (RF-L: [A(v),A(v+1)), v = 0x3E ... 0xF1) the reads in [lo,hi).
std::map<int, LineF> fetch_lines(const FetchRun& fr, int k, uint32_t lo, uint32_t hi) {
    std::map<int, LineF> out;
    const FField& f = fr.f[k];
    if (!f.have || !f.single) return out;
    for (int v = 0x3E; v <= 0xF1; ++v) {
        LineF L;
        auto rd = reads(*fr.r, f.A.at(v), f.A.at(v + 1), lo, hi);
        L.n = int(rd.size());
        if (!rd.empty()) {
            L.fetched = true;
            L.f = int64_t(rd.front().cyc) - int64_t(f.A.at(v));
            L.l = int64_t(rd.back().cyc) - int64_t(f.A.at(v));
            L.s = rd.front().byte; L.e = rd.back().byte;
            for (const auto& a : rd) L.addr.push_back(a.byte);
        }
        out[v] = L;
    }
    return out;
}

std::set<int> fetched_set(const std::map<int, LineF>& m) {
    std::set<int> s;
    for (const auto& p : m) if (p.second.fetched) s.insert(p.first);
    return s;
}

// Every read of [lo,hi) during the field lies within [A(0x3E), A(0xF2)).
bool reads_within(const FetchRun& fr, int k, uint32_t lo, uint32_t hi) {
    const FField& f = fr.f[k];
    if (!f.have || !f.single) return false;
    auto all = reads(*fr.r, f.T0, f.T1, lo, hi);
    for (const auto& a : all) if (a.cyc < f.A.at(0x3E) || a.cyc >= f.A.at(0xF2)) return false;
    return true;
}

// 2.3: during the field the regions in use are accessed in their direction only.
bool field_regions_ok(const FetchRun& fr, int k, const std::vector<Region>& regs) {
    const FField& f = fr.f[k];
    if (!f.have) return false;
    return regions_ok(*fr.r, f.T0, f.T1, regs);
}

struct CfgStat { bool constant = false; bool any = false; int64_t f = 0, l = 0; int n = 0; };

CfgStat cfg_stat(const std::map<int, LineF>& m) {
    CfgStat s;
    bool first = true; s.constant = true;
    for (const auto& p : m) {
        const LineF& L = p.second;
        if (!L.fetched) continue;
        s.any = true;
        if (first) { s.f = L.f; s.l = L.l; s.n = L.n; first = false; }
        else if (L.f != s.f || L.l != s.l) s.constant = false;
    }
    if (!s.any) s.constant = false;
    return s;
}

// ---- fetch propositions --------------------------------------------------------

std::map<std::string, FetchRun> g_fetch_cache;

FetchRun& fetch_run(const std::string& key, std::vector<FCfg> cfg, FetchOpts fo) {
    auto it = g_fetch_cache.find(key);
    if (it != g_fetch_cache.end()) return it->second;
    if (mutating("FIXTURE")) fo.no_fixture = true;
    CopList cl = list_a(fo.list);
    Script s = fetch_script(cfg, cl, fo);
    auto r = run_script(s);
    g_fetch_cache[key] = fetch_analyse(r, cl, cfg, key);
    g_fetch_cache[key].sc = s;
    return g_fetch_cache[key];
}

// The contrasts of the preconditions of 3 P1, applied to the configurations of a run
// that a proposition owns. `idx` are the run's configurations the proposition grades.
void fetch_pre_mutations(const std::string& pn, std::vector<FCfg>& cfg, FetchOpts& fo, const std::vector<int>& idx) {
    if (mutating((pn + "-i-RFM").c_str())) fo.list.h_c0 = 0xBE;
    if (mutating((pn + "-i-RFL").c_str())) cfg[idx[1 % idx.size()]].patch_wait3e = true;
    if (mutating((pn + "-ii").c_str())) cfg[idx[idx.size() / 2]].w_diwstrt = 0x5081;
    if (mutating((pn + "-iii").c_str())) cfg[idx[idx.size() / 2]].w_fmode = 0x0001;
    if (mutating((pn + "-iv").c_str())) for (auto& c : cfg) c.w_diwstrt = 0x3D81;
    if (mutating((pn + "-v").c_str())) fo.dma_copy_first = true;
    if (mutating((pn + "-reg").c_str())) cfg[idx[0]].stray_b1 = true;
    if (mutating((pn + "-fix").c_str())) fo.no_fixture = true;
}

struct FPre { bool fix, rfm, rfl, ii, iii, iv, v, reg; bool all() const { return fix && rfm && rfl && ii && iii && iv && v && reg; } };

// Prints the preconditions (i)-(v) and the region precondition of 2.3 of the
// proposition `pn` over the configurations `idx` of the run, plane region [lo,hi).
FPre fetch_pre(const std::string& pn, FetchRun& fr, const std::vector<int>& idx, uint32_t lo, uint32_t hi, const std::string& what,
               bool step_check = true) {
    FPre p{};
    {
        int ae = 0; uint32_t aa = 0;
        for (const auto& f : fr.f) { ae += f.after_end; if (f.after_end) aa = f.after_addr; }
        checks.note(pn + " measured: Copper reads after the second word of the list's END: " + dec(ae) + " in " + dec((long long)fr.f.size()) + " fields" + (ae ? ", at byte address 0x" + hexs(aa) + ", inside COP" : ""));
    }
    p.fix = fr.fix;
    g.expect(p.fix, pn + "-fix: the program stores the constant $BEEF first and the harness reads it back at the end of the run, before any other result of the run is used");
    p.rfm = fr.rfm;
    g.expect(p.rfm, pn + "-i-RFM: in every field of the run the Copper's fetches after WAIT (0x3C,0x40) and WAIT (0x3C,0xC0) are a positive multiple of 128 system clock cycles apart, and P, the system clock cycles per memory cycle, is " + dec(fr.P) + " in every field");
    p.rfl = fr.rfl;
    g.expect(p.rfl, pn + "-i-RFL: in every field of the run the fetch after each WAIT (v,0x04), v = 0x3E ... 0xF2, is read once, A(v+1) - A(v) is the same line period L = " + dec(fr.L) + " for v = 0x3E ... 0xF1, A(0x3E) - T0 is the same in every field, and both words of the list's END are fetched");
    std::set<int> ref;
    bool ii = true, first = true;
    for (int k : idx) {
        auto lm = fetch_lines(fr, k, lo, hi);
        auto fs = fetched_set(lm);
        if (first) { ref = fs; first = false; }
        else if (fs != ref) ii = false;
    }
    p.ii = ii && ref.size() >= 100;
    g.expect(p.ii, pn + "-ii: the set of fetched lines is the same in every configuration of the proposition and has at least 100 lines (" + dec((long long)ref.size()) + " lines)");
    bool iii = true;
    for (int k : idx) {
        auto lm = fetch_lines(fr, k, lo, hi);
        for (const auto& l : lm) {
            if (!l.second.fetched) continue;
            for (size_t j = 1; j < l.second.addr.size(); ++j)
                if (l.second.addr[j] != l.second.addr[j - 1] + 2) iii = false;
        }
    }
    p.iii = iii && !ref.empty() && step_check;
    g.expect(p.iii, pn + "-iii: within every fetched line consecutive " + what + " reads are at consecutive word addresses, 2 bytes apart");
    bool iv = true;
    for (int k : idx) iv = iv && reads_within(fr, k, lo, hi);
    p.iv = iv;
    g.expect(p.iv, pn + "-iv: every " + what + " read lies within [A(0x3E), A(0xF2))");
    p.v = (fr.dmacon_copy & 0x0380) == 0x0380;
    g.expect(p.v, pn + "-v: the stored DMACONR shows DMAEN, BPLEN and COPEN set");
    bool reg = script_clean(fr.sc, {{kCop, kCopEnd}, {kB1, kB1 + kPlaneLen}, {kB2, kB2 + kPlaneLen}});
    for (int k : idx)
        reg = reg && field_regions_ok(fr, k, {{kCop, kCopEnd, false}, {kB1, kB1 + kPlaneLen, false}, {kB2, kB2 + kPlaneLen, false}});
    p.reg = reg;
    g.expect(p.reg, pn + "-reg: during every observed field the regions COP, B1 and B2 are read only, and no instruction of the program writes into them after DMA is enabled");
    return p;
}

// f, l per configuration of the proposition
struct CS { CfgStat s; };

std::vector<CfgStat> cfg_stats(FetchRun& fr, const std::vector<int>& idx, uint32_t lo, uint32_t hi) {
    std::vector<CfgStat> out;
    for (int k : idx) out.push_back(cfg_stat(fetch_lines(fr, k, lo, hi)));
    return out;
}

constexpr const char* kP1Claims = "sol:AA-ddf.c1 opus:AA-ddfstrt.c9";
constexpr const char* kP2Claims = "sol:AA-ddf.c2 opus:AA-ddfstop.c9";
constexpr const char* kP3Claims = "opus:AA-ddfstrt.t2 opus:AA-ddfstrt.t3 opus:AA-ddfstrt.t4 sol:AA-ddfstrt.c6 sol:AA-ddfstrt.c7 sol:AA-ddfstrt.c8";
constexpr const char* kP4Claims = "opus:AA-ddfstop.t1 opus:AA-ddfstop.t2 sol:AA-ddfstop.c5 sol:AA-ddfstop.c6";

bool g_p1_ok = false, g_p1_graded = false, g_p2_ok = false, g_p2_graded = false;

// P1 and P2: the same comparison on DDFSTRT (first read) or DDFSTOP (last read).
bool ddf_weights(const std::vector<CfgStat>& st, const std::vector<uint16_t>& val, int64_t P, bool strt) {
    bool ok = !st.empty();
    for (const auto& s : st) ok = ok && s.any && s.constant;
    if (!ok) return false;
    for (size_t a = 0; a < st.size(); ++a)
        for (size_t b = 0; b < st.size(); ++b) {
            int64_t d = int64_t(val[b]) - int64_t(val[a]);
            if (strt) { if (st[b].f - st[a].f != d * P || st[b].l != st[a].l) ok = false; }
            else      { if (st[b].l - st[a].l != d * P || st[b].f != st[a].f) ok = false; }
        }
    return ok;
}

std::string note_fl(const std::vector<CfgStat>& st, const std::vector<uint16_t>& val) {
    std::string s;
    for (size_t i = 0; i < st.size(); ++i)
        s += (i ? "; " : "") + ("0x" + hexs(val[i], 4)) + " f=" + dec(st[i].f) + " l=" + dec(st[i].l) + " n=" + dec(st[i].n);
    return s;
}

const std::vector<uint16_t> kR1Strt = {0x28, 0x30, 0x38, 0x40, 0x60, 0x68, 0xB8};
const std::vector<int> kR1All = {0, 1, 2, 3, 4, 5, 6}, kR1Sub = {0, 1, 2, 3};

// the run of P1 and P3: seven DDFSTRT values
FetchRun& run_r1() {
    std::vector<FCfg> cfg;
    for (auto v : kR1Strt) { FCfg c; c.strt = v; c.stop = 0xD0; cfg.push_back(c); }
    FetchOpts fo;
    fetch_pre_mutations("P1", cfg, fo, kR1All);
    if (mutating("RF-RUNS")) fo.list.lh = 0x0C;
    if (mutating("P1") || mutating("J1")) cfg[5].w_strt = 0x58;          // 0x0068 is written as 0x0058
    if (mutating("P3")) cfg[2].w_strt = 0x40;          // the normal row's 0x0038 is written as the narrow row's 0x0040
    return fetch_run("R1", cfg, fo);
}

void proposition_p1_p3() {
    const bool w1 = want("P1"), w3 = want("P3");
    if (!w1 && !w3) return;
    const std::vector<uint16_t>& strt = kR1Strt;
    const std::vector<int>& all = kR1All; const std::vector<int>& sub = kR1Sub;
    FetchRun& fr = run_r1();

    FPre p = fetch_pre("P1", fr, all, kB1, kB1 + kPlaneLen, "B1");
    if (w1) {
        auto st = cfg_stats(fr, all, kB1, kB1 + kPlaneLen);
        std::vector<uint16_t> val(strt.begin(), strt.end());
        bool ok = ddf_weights(st, val, fr.P, true);
        checks.note("P1 measured (system clock cycles from A(v)): " + note_fl(st, val) + "; P = " + dec(fr.P));
        bool pre = config_ok(g_cfg_used) && p.all();
        g.expect_if(pre, ok, std::string("P1: ") + kP1Claims + ": changing DDFSTRT from a to b (H3 = H2 = 0) moves the first B1 read of every fetched line by exactly (b - a) memory cycles and leaves the last read where it was, for the seven values 0x0028 0x0030 0x0038 0x0040 0x0060 0x0068 0x00B8, f and l constant over all fetched lines");
        g_p1_ok = ok; g_p1_graded = pre;
    }
    if (w3) {
        auto st = cfg_stats(fr, sub, kB1, kB1 + kPlaneLen);
        bool ok = !st.empty();
        for (const auto& s : st) ok = ok && s.any && s.constant;
        if (ok) {
            for (int i = 1; i < 4; ++i) ok = ok && (st[i].f - st[i - 1].f == 8 * fr.P) && st[i].l == st[0].l;
        }
        bool pre = config_ok(g_cfg_used) && p.all();
        g.expect_if(pre, ok, std::string("P3: ") + kP3Claims + ": the DDFSTRT rows extra wide 0x0028, wide 0x0030, normal 0x0038 and narrow 0x0040 place the first B1 read in that order, each exactly 8 memory cycles after the one before, and the last read is the same in all four");
    }
}

const std::vector<uint16_t> kR2Stop = {0x58, 0x98, 0xB8, 0xC8, 0xD0, 0xD8};
const std::vector<int> kR2All = {0, 1, 2, 3, 4, 5}, kR2Sub = {3, 4, 5};

FetchRun& run_r2() {
    std::vector<FCfg> cfg;
    for (auto v : kR2Stop) { FCfg c; c.strt = 0x38; c.stop = v; cfg.push_back(c); }
    FetchOpts fo;
    fetch_pre_mutations("P2", cfg, fo, kR2All);
    if (mutating("P2") || mutating("J1")) cfg[1].w_stop = 0xA8;          // 0x0098 is written as 0x00A8
    if (mutating("P4")) cfg[4].w_stop = 0xD8;          // the normal row's 0x00D0 is written as the wide row's 0x00D8
    return fetch_run("R2", cfg, fo);
}

void proposition_p2_p4() {
    const bool w2 = want("P2"), w4 = want("P4");
    if (!w2 && !w4) return;
    const std::vector<uint16_t>& stop = kR2Stop;
    const std::vector<int>& all = kR2All; const std::vector<int>& sub = kR2Sub;
    FetchRun& fr = run_r2();

    FPre p = fetch_pre("P2", fr, all, kB1, kB1 + kPlaneLen, "B1");
    if (w2) {
        auto st = cfg_stats(fr, all, kB1, kB1 + kPlaneLen);
        std::vector<uint16_t> val(stop.begin(), stop.end());
        bool ok = ddf_weights(st, val, fr.P, false);
        checks.note("P2 measured (system clock cycles from A(v)): " + note_fl(st, val) + "; P = " + dec(fr.P));
        bool pre = config_ok(g_cfg_used) && p.all();
        g.expect_if(pre, ok, std::string("P2: ") + kP2Claims + ": changing DDFSTOP from c to d (H3 = H2 = 0) moves the last B1 read of every fetched line by exactly (d - c) memory cycles and leaves the first read where it was, for the six values 0x0058 0x0098 0x00B8 0x00C8 0x00D0 0x00D8, f and l constant over all fetched lines");
        g_p2_ok = ok; g_p2_graded = pre;
    }
    if (w4) {
        auto st = cfg_stats(fr, sub, kB1, kB1 + kPlaneLen);
        bool ok = !st.empty();
        for (const auto& s : st) ok = ok && s.any && s.constant;
        if (ok) {
            for (int i = 1; i < 3; ++i) ok = ok && (st[i].l - st[i - 1].l == 8 * fr.P) && st[i].f == st[0].f;
        }
        bool pre = config_ok(g_cfg_used) && p.all();
        g.expect_if(pre, ok, std::string("P4: ") + kP4Claims + ": the DDFSTOP rows narrow 0x00C8, normal 0x00D0 and wide 0x00D8 place the last B1 read in that order, each exactly 8 memory cycles after the one before, and the first read is the same in all three");
    }
}

// P5: hires, bit 2 of the DDF registers.
void proposition_p5() {
    if (!want("P5")) return;
    // L0, H1, H2, H3 as in the specification; C1-C3 are hires configurations with bit 2 clear in both
    // registers, the control: the same comparators on the weight of bit 3.
    std::vector<FCfg> cfg(7);
    cfg[0].bplcon0 = 0x1000; cfg[0].strt = 0x38; cfg[0].stop = 0xD0;
    cfg[1].bplcon0 = 0x9000; cfg[1].strt = 0x38; cfg[1].stop = 0xD4;
    cfg[2].bplcon0 = 0x9000; cfg[2].strt = 0x3C; cfg[2].stop = 0xD4;
    cfg[3].bplcon0 = 0x9000; cfg[3].strt = 0x3C; cfg[3].stop = 0xD0;
    cfg[4].bplcon0 = 0x9000; cfg[4].strt = 0x30; cfg[4].stop = 0xD0;
    cfg[5].bplcon0 = 0x9000; cfg[5].strt = 0x38; cfg[5].stop = 0xD0;
    cfg[6].bplcon0 = 0x9000; cfg[6].strt = 0x38; cfg[6].stop = 0xC8;
    FetchOpts fo;
    const std::vector<int> hi = {1, 2, 3, 4, 5, 6}, all = {0, 1, 2, 3, 4, 5, 6};
    fetch_pre_mutations("P5", cfg, fo, hi);
    if (mutating("P5")) cfg[2].w_strt = 0x38;          // H2 writes 0x0038 in place of 0x003C: bit 2 of DDFSTRT stays clear
    if (mutating("P5-vi")) for (int i : hi) cfg[i].w_bplcon0 = 0x1000;  // the hires configurations run in lores
    if (mutating("P5-ctl")) cfg[4].w_strt = 0x38;      // C1 writes C2's DDFSTRT
    FetchRun& fr = fetch_run("R5", cfg, fo);
    FPre p = fetch_pre("P5", fr, hi, kB1, kB1 + kPlaneLen, "B1");
    auto st = cfg_stats(fr, all, kB1, kB1 + kPlaneLen);
    bool vi = st[0].any;
    for (int i : hi) vi = vi && st[i].any && st[i].n > st[0].n;
    g.expect(vi, "P5-vi: hires is in force: the number of B1 reads per fetched line is greater in each hires configuration than in the lores configuration (L0 n=" + dec(st[0].n) + ", H1 n=" + dec(st[1].n) + ", H2 n=" + dec(st[2].n) + ", H3 n=" + dec(st[3].n) + ", C1 n=" + dec(st[4].n) + ", C2 n=" + dec(st[5].n) + ", C3 n=" + dec(st[6].n) + ")");
    bool cst = true;
    for (int i : hi) cst = cst && st[i].constant;
    bool half_strt = cst && (st[2].f - st[1].f == 4 * fr.P) && st[2].l == st[1].l;
    bool half_stop = cst && (st[2].l - st[3].l == 4 * fr.P) && st[2].f == st[3].f;
    bool ok = half_strt && half_stop;
    // the control: the weight of bit 3 in hires through the same two comparators
    bool ctl = cst && (st[5].f - st[4].f == 8 * fr.P) && st[5].l == st[4].l && (st[5].l - st[6].l == 8 * fr.P) && st[5].f == st[6].f;
    checks.note("P5 measured (system clock cycles from A(v)): H1 f=" + dec(st[1].f) + " l=" + dec(st[1].l) + "; H2 f=" + dec(st[2].f) + " l=" + dec(st[2].l) + "; H3 f=" + dec(st[3].f) + " l=" + dec(st[3].l) + "; C1 f=" + dec(st[4].f) + " l=" + dec(st[4].l) + "; C2 f=" + dec(st[5].f) + " l=" + dec(st[5].l) + "; C3 f=" + dec(st[6].f) + " l=" + dec(st[6].l) + "; P = " + dec(fr.P));
    checks.note(std::string("P5 measured: the DDFSTRT half (H2 against H1) ") + (half_strt ? "holds" : "does not hold") + ", the DDFSTOP half (H2 against H3) " + (half_stop ? "holds" : "does not hold"));
    bool pre = config_ok(g_cfg_used) && p.all() && vi;
    g.expect_if(pre, ctl, "P5-ctl: control, hires with bit 2 of both registers clear: DDFSTRT 0x0038 against 0x0030 moves the first B1 read 8 memory cycles later and leaves the last read, and DDFSTOP 0x00D0 against 0x00C8 moves the last read 8 memory cycles later and leaves the first read");
    g.expect_if(pre, ok, "P5: opus:AA-ddf.use-b02: in hires with FMODE = 0, setting bit 2 of DDFSTRT (H3) moves the first B1 read of every fetched line 4 memory cycles later and leaves the last read, and setting bit 2 of DDFSTOP moves the last read 4 memory cycles later and leaves the first read");
}

// P7: pointer advance and modulo.
void proposition_p7() {
    if (!want("P7")) return;
    std::vector<FCfg> cfg(3);
    for (auto& c : cfg) { c.bplcon0 = 0x2000; c.strt = 0x38; c.stop = 0xD0; }
    cfg[0].mod1 = 0x0008; cfg[0].mod2 = 0x0018;
    cfg[1].stop = 0xD8; cfg[1].mod1 = 0x0008; cfg[1].mod2 = 0x0018;
    cfg[2].mod1 = 0x0018; cfg[2].mod2 = 0x0008;
    FetchOpts fo; fo.list.planes = 2;
    const std::vector<int> all = {0, 1, 2};
    if (mutating("P7-i-RFM")) fo.list.h_c0 = 0xBE;
    if (mutating("P7-i-RFL")) cfg[1].patch_wait3e = true;
    if (mutating("P7-ii")) cfg[1].w_fmode = 0x0001;
    if (mutating("P7-v")) cfg[1].w_diwstrt = 0x5081;
    if (mutating("P7-vi")) fo.dma_copy_first = true;
    if (mutating("P7-reg")) cfg[0].stray_b1 = true;
    if (mutating("P7-iii")) cfg[1].w_bplcon0 = 0x1000;
    if (mutating("P7-fix")) fo.no_fixture = true;
    if (mutating("P7")) cfg[0].w_mod1 = 0x0010;       // BPL1MOD of M-A is written as 0x0010
    if (mutating("P7-iv")) cfg[1].w_stop = 0xD0;      // M-B writes M-A's DDFSTOP
    FetchRun& fr = fetch_run("R7", cfg, fo);
    const long long mod[3][2] = {{8, 0x18}, {8, 0x18}, {0x18, 8}};

    // per configuration and plane: the lines, in order
    struct PL { std::map<int, LineF> l[2]; };
    std::vector<PL> pl(3);
    for (int k = 0; k < 3; ++k) { pl[k].l[0] = fetch_lines(fr, k, kB1, kB1 + kPlaneLen); pl[k].l[1] = fetch_lines(fr, k, kB2, kB2 + kPlaneLen); }

    g.expect(fr.fix, "P7-fix: the program stores the constant $BEEF first and the harness reads it back at the end of the run, before any other result of the run is used");
    g.expect(fr.rfm, "P7-i-RFM: in every field of the run the Copper's fetches after WAIT (0x3C,0x40) and WAIT (0x3C,0xC0) are a positive multiple of 128 system clock cycles apart, and P is " + dec(fr.P) + " in every field");
    g.expect(fr.rfl, "P7-i-RFL: in every field of the run the fetch after each WAIT (v,0x04), v = 0x3E ... 0xF2, is read once, A(v+1) - A(v) is the same L = " + dec(fr.L) + " for v = 0x3E ... 0xF1, A(0x3E) - T0 is the same in every field, and both words of the list's END are fetched");
    bool ii = true, iii = true, v5 = true;
    std::set<int> ref;
    int n_cfg[3] = {0, 0, 0};
    for (int k = 0; k < 3; ++k) {
        auto fs = fetched_set(pl[k].l[0]);
        if (k == 0) ref = fs; else if (fs != ref) v5 = false;
        if (fs.size() < 1) v5 = false;
        for (int p = 0; p < 2; ++p)
            for (const auto& l : pl[k].l[p])
                if (l.second.fetched)
                    for (size_t j = 1; j < l.second.addr.size(); ++j)
                        if (l.second.addr[j] != l.second.addr[j - 1] + 2) ii = false;
        int nn = -1;
        for (int v = 0x3E; v <= 0xF1; ++v) {
            const LineF& a = pl[k].l[0][v]; const LineF& b = pl[k].l[1][v];
            if (a.fetched != b.fetched) { iii = false; continue; }
            if (!a.fetched) continue;
            if (a.n != b.n) iii = false;
            if (nn < 0) nn = a.n; else if (a.n != nn) iii = false;
        }
        n_cfg[k] = nn;
    }
    bool iv = n_cfg[1] > n_cfg[0] && n_cfg[0] > 0;
    g.expect(ii && !ref.empty(), "P7-ii: within each fetched line consecutive reads of a plane are 2 bytes apart");
    g.expect(iii && !ref.empty(), "P7-iii: n_1(v) = n_2(v) on every line, and n is the same on every line of a configuration (n = " + dec(n_cfg[0]) + ", " + dec(n_cfg[1]) + ", " + dec(n_cfg[2]) + ")");
    g.expect(iv, "P7-iv: n(M-B) = " + dec(n_cfg[1]) + " is greater than n(M-A) = " + dec(n_cfg[0]));
    g.expect(v5 && ref.size() >= 100, "P7-v: the set of fetched lines is the same in all three configurations (" + dec((long long)ref.size()) + " lines)");
    bool dm = (fr.dmacon_copy & 0x0380) == 0x0380;
    g.expect(dm, "P7-vi: the stored DMACONR shows DMAEN, BPLEN and COPEN set");
    bool reg = script_clean(fr.sc, {{kCop, kCopEnd}, {kB1, kB1 + kPlaneLen}, {kB2, kB2 + kPlaneLen}});
    for (int k = 0; k < 3; ++k)
        reg = reg && field_regions_ok(fr, k, {{kCop, kCopEnd, false}, {kB1, kB1 + kPlaneLen, false}, {kB2, kB2 + kPlaneLen, false}});
    g.expect(reg, "P7-reg: during every observed field the regions COP, B1 and B2 are read only, and no instruction of the program writes into them after DMA is enabled");

    bool pre = fr.fix && config_ok(g_cfg_used) && fr.rfm && fr.rfl && ii && iii && iv && v5 && ref.size() >= 100 && dm && reg;
    // pitch of plane p in configuration k: s_p(v+1) - s_p(v) for consecutive fetched lines
    bool rel1 = true;
    long long pitch[3][2];
    for (int k = 0; k < 3; ++k)
        for (int p = 0; p < 2; ++p) {
            std::optional<long long> pt;
            bool cst = true;
            int pairs = 0;
            for (int v = 0x3E; v < 0xF1; ++v) {
                const LineF& a = pl[k].l[p][v]; const LineF& b = pl[k].l[p][v + 1];
                if (!a.fetched || !b.fetched) continue;
                ++pairs;
                if (int64_t(b.s) != int64_t(a.e) + 2 + mod[k][p]) rel1 = false;
                long long d = (long long)b.s - (long long)a.s;
                if (!pt) pt = d; else if (*pt != d) cst = false;
            }
            if (!pairs) rel1 = false;
            pitch[k][p] = pt.value_or(0);
            if (!cst) rel1 = false;
        }
    bool rel2 = pitch[0][0] - pitch[0][1] == -16;
    bool rel3 = true;
    for (int p = 0; p < 2; ++p) rel3 = rel3 && pitch[1][p] - pitch[0][p] == 2 * (n_cfg[1] - n_cfg[0]);
    bool rel4 = pitch[2][0] == pitch[0][1] && pitch[2][1] == pitch[0][0];
    checks.note("P7 measured (bytes): pitch plane 1 / plane 2: M-A " + dec(pitch[0][0]) + "/" + dec(pitch[0][1]) + ", M-B " + dec(pitch[1][0]) + "/" + dec(pitch[1][1]) + ", M-C " + dec(pitch[2][0]) + "/" + dec(pitch[2][1]));
    g.expect_if(pre, rel1 && rel2 && rel3 && rel4,
        "P7: opus:AA-ddf.c7 sol:AA-ddf.c4 sol:AA-ddf.c5: for each plane the first read address of each fetched line is the address after the last read of the previous fetched line plus that plane's modulo; in M-A the pitches of planes 1 and 2 differ by -16 bytes; the wider fetch of M-B raises each pitch by 2 x the extra words; M-C swaps the two planes' pitches");
}

// ---- S-WIN runs and the video instrument RF-V (2.4, 2.5) ---------------------------

struct WCfg {
    uint16_t diwstrt = 0x4081, diwstop = 0xF0C1;
    std::optional<uint16_t> diwhigh;         // written after DIWSTRT and DIWSTOP when set
    bool high_first = false;                 // the DIWHIGH write precedes the DIWSTRT and DIWSTOP writes
    std::optional<uint16_t> w_diwstrt, w_diwstop;   // what the program writes where it differs
    std::optional<uint16_t> w_diwhigh;
    bool patch_wait3e = false;               // RF-L contrast (list L-A)
    std::optional<uint16_t> bplen_off_at;    // the CPU clears BPLEN when V7-V0 reaches this value
    bool stray_write = false;                // region contrast: the CPU writes to the plane region during this field
};

struct WinOpts {
    bool list_a = false;                     // L-A with the marker block (P6); else L-V
    bool bplcon3_clear = false;              // RF-V contrast: BRDRBLNK clear throughout
    uint16_t bplcon0 = 0x1001;
    bool dma_copy_first = false;
    bool no_fixture = false;
    bool dmacon_noplane = false;
    uint16_t dmacon = 0x8380;
    ListOpts lo;
};

Script win_script(const std::vector<WCfg>& cfg, const CopList& cl, const WinOpts& o) {
    Script s;
    if (!o.no_fixture) script_start(s);
    s.W(BEAMCON0, 0x0020);
    s.ZERO(kZ, 0x1000);
    s.W(BPLCON0, o.bplcon0);
    s.W(BPLCON1, 0); s.W(BPLCON2, 0);
    s.W(BPLCON3, o.bplcon3_clear ? 0x0C00 : 0x0C20);
    s.W(BPLCON4, 0x0011); s.W(FMODE, 0);
    s.W(COLOR00, 0x0F00); s.W(COLOR01, 0x0F00);
    s.W(DDFSTRT, 0x0038); s.W(DDFSTOP, 0x00D0); s.W(BPL1MOD, 0);
    s.W(DIWSTRT, cfg[0].diwstrt); s.W(DIWSTOP, cfg[0].diwstop);
    script_copper_start(s, cl);
    if (o.dma_copy_first) s.CPYD();
    s.W(DMACON, o.dmacon);
    if (!o.dma_copy_first) s.CPYD();
    s.WL30();
    for (size_t k = 0; k < cfg.size(); ++k) {
        const WCfg& c = cfg[k];
        s.WVB();
        if (c.diwhigh && c.high_first) s.W(DIWHIGH, c.w_diwhigh.value_or(*c.diwhigh));
        s.W(DIWSTRT, c.w_diwstrt.value_or(c.diwstrt));
        s.W(DIWSTOP, c.w_diwstop.value_or(c.diwstop));
        if (c.diwhigh && !c.high_first) s.W(DIWHIGH, c.w_diwhigh.value_or(*c.diwhigh));
        s.MARK(uint16_t(k));
        if (c.stray_write) s.STORE(kZ + 0x7000, 0x1234);
        if (c.patch_wait3e) s.COPYW(kCop + cl.off_wait[0x3E], {uint16_t((0x3E << 8) | 0x0D)});
        if (c.bplen_off_at) { s.WLN(*c.bplen_off_at); s.W(DMACON, 0x0100); }
        s.WL30();
    }
    s.WVB();
    s.END();
    return s;
}

struct VF {
    bool found = false, rfv = false;
    int k = 0;
    std::set<int> win;
    std::string why;
};

int floor_div(int a, int b) { int q = a / b; if ((a % b != 0) && ((a < 0) != (b < 0))) --q; return q; }

// 2.4 RF-V for the configuration stored at time m.
VF video_field(const RunData& r, uint64_t m) {
    VF v;
    for (const auto& fr : r.frames) {
        std::vector<std::pair<int, int>> blocks;
        int n = int(fr.rows.size());
        for (int i = 0; i < n;) {
            if (fr.rows[i].nred > 0) { int j = i; while (j < n && fr.rows[j].nred > 0) ++j; blocks.push_back({i, j}); i = j; }
            else ++i;
        }
        if (blocks.size() < 2) continue;
        if (fr.rows[blocks[0].first].cyc <= m) continue;
        v.found = true;
        int r20 = blocks[0].first, r24 = blocks[1].first;
        bool ok = true;
        int d = r24 - r20;
        if (d % 4 != 0 || d / 4 < 1) { ok = false; v.why += "k not whole; "; }
        int k = d / 4 >= 1 ? d / 4 : 1;
        v.k = k;
        if (blocks[0].second - blocks[0].first != k) { ok = false; v.why += "block 1 height; "; }
        if (blocks[1].second - blocks[1].first != k) { ok = false; v.why += "block 2 height; "; }
        auto no_red = [&](int a, int b) { if (a < 0 || b > n) return false; for (int i = a; i < b; ++i) if (fr.rows[i].nred) return false; return true; };
        if (!no_red(r20 - 3 * k, r20)) { ok = false; v.why += "rows of 0x1D-0x1F; "; }
        if (!no_red(r20 + k, r24)) { ok = false; v.why += "rows of 0x21-0x23; "; }
        if (!no_red(r24 + k, r24 + 2 * k)) { ok = false; v.why += "rows of 0x25; "; }
        int mleft = 1 << 30;
        for (int b = 0; b < 2; ++b)
            for (int i = blocks[b].first; i < blocks[b].second; ++i) mleft = std::min(mleft, fr.rows[i].first_red);
        for (size_t b = 2; b < blocks.size(); ++b)
            for (int i = blocks[b].first; i < blocks[b].second; ++i) {
                if (fr.rows[i].first_red <= mleft) { ok = false; v.why += "window row reaches as far left as a marker row; "; }
                v.win.insert(0x20 + floor_div(i - r20, k));
            }
        v.rfv = ok;
        return v;
    }
    v.why = "no frame with marker rows after the store";
    return v;
}

struct WinRun {
    std::shared_ptr<RunData> r;
    Script sc;
    CopList cl;
    std::vector<WCfg> cfg;
    std::vector<VF> vf;
    bool fix = false, rfv = false;
    uint16_t dmacon_copy = 0;
    FetchRun fr;                              // the fetch analysis, for the run with list L-A
    bool has_fetch = false;
};

std::map<std::string, WinRun> g_win_cache;

WinRun& win_run(const std::string& key, std::vector<WCfg> cfg, WinOpts wo) {
    auto it = g_win_cache.find(key);
    if (it != g_win_cache.end()) return it->second;
    if (mutating("FIXTURE")) wo.no_fixture = true;
    WinRun w;
    if (wo.list_a) {
        wo.lo.planes = 1; wo.lo.base[0] = kZ; wo.lo.marker = true;
        if (wo.bplcon3_clear) { wo.lo.marker_clear = 0x0C00; wo.lo.marker_set = 0x0C00; }
        w.cl = list_a(wo.lo);
    } else {
        w.cl = list_v();
        if (wo.bplcon3_clear) {
            // the marker block moves BPLCON3 to 0x0C00 throughout
            CopList c;
            append(c.w, cop_move(BPL1PTH, uint16_t(kZ >> 16))); append(c.w, cop_move(BPL1PTL, uint16_t(kZ)));
            append(c.w, marker_block(0x0C00, 0x0C00));
            c.w.push_back(0xFFFF); c.w.push_back(0xFFFE);
            c.off_end = c.bytes() - 4;
            w.cl = c;
        }
    }
    w.cfg = cfg;
    w.sc = win_script(cfg, w.cl, wo);
    w.r = run_script(w.sc, true);
    w.fix = fixture_ok(*w.r);
    if (!w.r->dmacon_copies.empty()) w.dmacon_copy = w.r->dmacon_copies[0].data;
    bool rfv = !w.r->frames.empty();
    for (size_t k = 0; k < cfg.size(); ++k) {
        uint64_t m = 0;
        for (const auto& mk : w.r->marks) if (mk.data == k) { m = mk.cyc; break; }
        VF v = m ? video_field(*w.r, m) : VF{};
        if (!v.found || !v.rfv) { dbg("RF-V config " + std::to_string(k) + ": " + v.why); }
        rfv = rfv && v.found && v.rfv;
        w.vf.push_back(v);
    }
    w.rfv = rfv;
    if (wo.list_a) {
        std::vector<FCfg> fc(cfg.size());
        w.fr = fetch_analyse(w.r, w.cl, fc, key);
        w.has_fetch = true;
    }
    g_win_cache[key] = std::move(w);
    return g_win_cache[key];
}

int first_line(const std::set<int>& w, int lo, int hi) { for (int v : w) if (v >= lo && v <= hi) return v; return -1; }
int last_line(const std::set<int>& w, int lo, int hi) { int r = -1; for (int v : w) if (v >= lo && v <= hi) r = v; return r; }
bool contiguous(const std::set<int>& w, int lo, int hi) {
    int a = first_line(w, lo, hi), b = last_line(w, lo, hi);
    if (a < 0) return false;
    for (int v = a; v <= b; ++v) if (!w.count(v)) return false;
    return true;
}

// The window propositions' common preconditions: fixture, RF-V, DMACONR copy, regions, DIWHIGH writes.
struct WPre { bool fix, rfv, dm, reg; };

WPre win_pre(const std::string& pn, WinRun& w) {
    WPre p{};
    p.fix = w.fix;
    g.expect(p.fix, pn + "-fix: the program stores the constant $BEEF first and the harness reads it back at the end of the run, before any other result of the run is used");
    p.rfv = w.rfv;
    std::string kk;
    for (const auto& v : w.vf) kk += (kk.empty() ? "" : ", ") + dec(v.k);
    g.expect(p.rfv, pn + "-i-RFV: in every configuration's video field the two marker blocks are whole-number k output rows apart per beam line, each exactly k rows tall, the rows of lines 0x1D-0x1F, 0x21-0x23 and 0x25 contain no red, and red reaches further left on marker rows than on any window row (k = " + kk + ")");
    p.dm = (w.dmacon_copy & 0x0380) == 0x0380;
    g.expect(p.dm, pn + "-dm: the stored DMACONR shows DMAEN, BPLEN and COPEN set");
    bool reg = true;
    uint64_t t0 = w.r->marks.empty() ? 0 : w.r->marks.front().cyc;
    uint64_t t1 = w.r->ncycles;
    reg = reg && regions_ok(*w.r, t0, t1, {{kCop, kCopEnd, false}, {kZ, kZ + kPlaneLen, false}}) && script_clean(w.sc, {{kCop, kCopEnd}, {kZ, kZ + kPlaneLen}});
    p.reg = reg;
    g.expect(p.reg, pn + "-reg: during the observed fields the regions COP and Z are read only, and no instruction of the program writes into them after DMA is enabled");
    return p;
}

// Mutations common to the window propositions: precondition contrasts that change the stimulus.
void win_pre_mutations(const std::string& pn, std::vector<WCfg>& cfg, WinOpts& wo, size_t k) {
    if (mutating((pn + "-i-RFV").c_str())) wo.bplcon3_clear = true;
    if (mutating((pn + "-dm").c_str())) wo.dma_copy_first = true;
    if (mutating((pn + "-reg").c_str())) cfg[k].stray_write = true;
    if (mutating((pn + "-fix").c_str())) wo.no_fixture = true;
}

// ---- P6: bit-plane DMA runs on exactly the lines of the display window ---------------

void proposition_p6() {
    if (!want("P6")) return;
    std::vector<WCfg> cfg(2);
    cfg[0].diwstrt = 0x4081; cfg[0].diwstop = 0xF0C1;
    cfg[1].diwstrt = 0x5081; cfg[1].diwstop = 0xA0C1;
    WinOpts wo; wo.list_a = true;
    win_pre_mutations("P6", cfg, wo, 0);
    if (mutating("P6-i-RFL")) cfg[1].patch_wait3e = true;
    if (mutating("P6-ii")) wo.bplcon0 = 0x0001;
    if (mutating("P6-iv")) cfg[1].w_diwstop = 0x10C1;
    if (mutating("P6")) cfg[1].bplen_off_at = 0x70;
    WinRun& w = win_run("W6", cfg, wo);

    WPre p = win_pre("P6", w);
    g.expect(w.fr.rfl, "P6-i-RFL: in every field of the run the fetch after each WAIT (v,0x04), v = 0x3E ... 0xF2, is read once, A(v+1) - A(v) is the same line period L = " + dec(w.fr.L) + " for v = 0x3E ... 0xF1, A(0x3E) - T0 is the same in every field, and both words of the list's END are fetched");
    bool rfm = w.fr.rfm;
    g.expect(rfm, "P6-i-RFM: in every field of the run the Copper's fetches after WAIT (0x3C,0x40) and WAIT (0x3C,0xC0) are a positive multiple of 128 system clock cycles apart, and P is " + dec(w.fr.P) + " in every field");
    std::vector<std::set<int>> fetch(2);
    bool ii = true, within = true;
    for (int k = 0; k < 2; ++k) {
        fetch[k] = fetched_set(fetch_lines(w.fr, k, kZ, kZ + kPlaneLen));
        ii = ii && !fetch[k].empty();
        within = within && reads_within(w.fr, k, kZ, kZ + kPlaneLen);
    }
    g.expect(ii && within, "P6-ii: Fetch is non-empty in both configurations (" + dec((long long)fetch[0].size()) + " and " + dec((long long)fetch[1].size()) + " lines) and every Z read lies within [A(0x3E), A(0xF2))");
    bool iv = true;
    for (int k = 0; k < 2; ++k) iv = iv && !w.vf[k].win.empty() && first_line(w.vf[k].win, 0, 0x1FF) >= 0x3E && last_line(w.vf[k].win, 0, 0x1FF) <= 0xF1;
    g.expect(iv, "P6-iv: Win lies inside 0x3E-0xF1 in both configurations");
    bool pre = config_ok(g_cfg_used) && p.fix && p.rfv && p.dm && p.reg && w.fr.rfl && rfm && ii && within && iv;
    bool eq = true;
    for (int k = 0; k < 2; ++k) {
        std::set<int> win;
        for (int v : w.vf[k].win) if (v >= 0x3E && v <= 0xF1) win.insert(v);
        eq = eq && win == fetch[k];
        checks.note("P6 measured, configuration " + dec(k + 1) + ": Fetch " + dec((long long)fetch[k].size()) + " lines (" + hexs(first_line(fetch[k], 0, 0x1FF), 3) + " - " + hexs(last_line(fetch[k], 0, 0x1FF), 3) + "), Win " + dec((long long)w.vf[k].win.size()) + " lines (" + hexs(first_line(w.vf[k].win, 0, 0x1FF), 3) + " - " + hexs(last_line(w.vf[k].win, 0, 0x1FF), 3) + "), k = " + dec(w.vf[k].k));
    }
    g.expect_if(pre, eq, "P6: opus:AA-ddf.c6 sol:AA-ddf.c3: the set of beam lines on which bit-plane DMA reads Z equals, line for line, the set of beam lines on which the display window shows red, in W1 (DIWSTRT 0x4081, DIWSTOP 0xF0C1) and in W2 (DIWSTRT 0x5081, DIWSTOP 0xA0C1)");
}

// ---- P8-P11: vertical start and stop, without DIWHIGH ---------------------------------

struct WinPropRun { WinRun* w; WPre p; bool ii; };

WinPropRun win_prop_pre(const std::string& pn, WinRun& w) {
    WinPropRun r; r.w = &w;
    r.p = win_pre(pn, w);
    int n = script_writes(w.sc, DIWHIGH);
    r.ii = n == 0;
    g.expect(r.ii, pn + "-ii: DIWHIGH is not written by the program (" + dec(n) + " writes of DIWHIGH in the command list)");
    return r;
}

bool g_p8_ok = false, g_p8_graded = false, g_p9_ok = false, g_p9_graded = false, g_p10_ok = false, g_p10_graded = false, g_p11_ok = false, g_p11_graded = false;

std::vector<WCfg> p8_cfg() {
    const uint16_t hi[9] = {0x40, 0x41, 0x42, 0x44, 0x48, 0x50, 0x60, 0x30, 0x70};
    std::vector<WCfg> cfg;
    for (auto h : hi) { WCfg c; c.diwstrt = uint16_t(h << 8 | 0x81); c.diwstop = 0xF0C1; cfg.push_back(c); }
    return cfg;
}

void proposition_p8() {
    if (!want("P8")) return;
    auto cfg = p8_cfg();
    WinOpts wo;
    win_pre_mutations("P8", cfg, wo, 0);
    if (mutating("P8")) cfg[4].w_diwstrt = 0x4C81;       // bits 15-8 of the fifth value are written as 0x4C in place of 0x48
    WinRun* w = nullptr;
    if (mutating("P8-ii")) {
        // the contrast writes DIWHIGH <- 0x2001 after DIWSTRT = 0x4081 (start V8 = 1), as the last configuration of its run
        cfg.back().diwhigh = 0x2001; cfg.back().diwstrt = 0x4081; cfg.back().w_diwstrt.reset();
    }
    w = &win_run("W8", cfg, wo);
    WinPropRun r = win_prop_pre("P8", *w);
    bool ok = true, same_last = true, cont = true;
    int last0 = -2;
    std::string note;
    for (size_t k = 0; k < cfg.size(); ++k) {
        const auto& win = w->vf[k].win;
        int f = first_line(win, 0x26, 0x12F), l = last_line(win, 0x26, 0x12F);
        if (f != (cfg[k].diwstrt >> 8)) ok = false;
        if (k == 0) last0 = l; else if (l != last0) same_last = false;
        if (!contiguous(win, 0x26, 0x12F)) cont = false;
        note += (k ? "; " : "") + hexs(cfg[k].diwstrt >> 8, 2) + ": " + hexs(f, 3) + "-" + hexs(l, 3);
    }
    checks.note("P8 measured (first-last line of Win): " + note);
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii;
    bool all = ok && same_last && cont;
    g.expect_if(pre, all, "P8: with DIWHIGH never written, the first line of the display window equals bits 15-8 of DIWSTRT for the values 0x40 0x41 0x42 0x44 0x48 0x50 0x60 0x30 0x70, so bits 8-14 weigh 1, 2, 4, 8, 16, 32 and 64 lines; the last line is the same in all nine configurations and Win is one contiguous run");
    g_p8_ok = all; g_p8_graded = pre;
}

std::vector<WCfg> p9_cfg() {
    const uint16_t hi[8] = {0xB0, 0xB1, 0xB2, 0xB4, 0xB8, 0xA0, 0x90, 0xF0};
    std::vector<WCfg> cfg;
    for (auto h : hi) { WCfg c; c.diwstrt = 0x4081; c.diwstop = uint16_t(h << 8 | 0xC1); cfg.push_back(c); }
    return cfg;
}

void proposition_p9() {
    if (!want("P9")) return;
    auto cfg = p9_cfg();
    WinOpts wo;
    win_pre_mutations("P9", cfg, wo, 0);
    if (mutating("P9")) cfg[3].w_diwstop = 0xB6C1;       // bits 15-8 of the fourth value are written as 0xB6 in place of 0xB4
    if (mutating("P9-ii")) { cfg.back().diwhigh = 0x2100; cfg.back().diwstop = 0xB0C1; cfg.back().w_diwstop.reset(); }
    WinRun& w = win_run("W9", cfg, wo);
    WinPropRun r = win_prop_pre("P9", w);
    bool ok = true;
    int base = last_line(w.vf[0].win, 0x26, 0x12F);
    std::string note;
    for (size_t k = 0; k < cfg.size(); ++k) {
        int l = last_line(w.vf[k].win, 0x26, 0x12F);
        int f = first_line(w.vf[k].win, 0x26, 0x12F);
        if (f != 0x40) ok = false;
        if (l < 0 || base < 0 || l - base != int(cfg[k].diwstop >> 8) - 0xB0) ok = false;
        note += (k ? "; " : "") + hexs(cfg[k].diwstop >> 8, 2) + ": " + hexs(f, 3) + "-" + hexs(l, 3);
    }
    checks.note("P9 measured (first-last line of Win): " + note);
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii;
    g.expect_if(pre, ok, "P9: with DIWHIGH never written, the last line of the display window of DIWSTOP bits 15-8 = x is x - 0xB0 lines from that of 0xB0, for x = 0xB1 0xB2 0xB4 0xB8 0xA0 0x90 0xF0, so bits 8-14 weigh 1, 2, 4, 8, 16, 32 and 64 lines, and the first line of Win is 0x40 in all eight configurations");
    g_p9_ok = ok; g_p9_graded = pre;
}

void proposition_p10() {
    if (!want("P10")) return;
    std::vector<WCfg> cfg(2);
    cfg[0].diwstrt = 0x4081; cfg[1].diwstrt = 0xC081;
    WinOpts wo;
    win_pre_mutations("P10", cfg, wo, 0);
    if (mutating("P10")) cfg[1].w_diwstrt = 0xC481;          // 0xC081 is written as 0xC481
    if (mutating("P10-ii")) { cfg.back().diwhigh = 0x2001; cfg.back().diwstrt = 0x4081; cfg.back().w_diwstrt.reset(); }
    WinRun& w = win_run("W10", cfg, wo);
    WinPropRun r = win_prop_pre("P10", w);
    int f0 = first_line(w.vf[0].win, 0x26, 0x12F), f1 = first_line(w.vf[1].win, 0x26, 0x12F);
    int l0 = last_line(w.vf[0].win, 0x26, 0x12F), l1 = last_line(w.vf[1].win, 0x26, 0x12F);
    checks.note("P10 measured (first-last line of Win): 0x4081: " + hexs(f0, 3) + "-" + hexs(l0, 3) + "; 0xC081: " + hexs(f1, 3) + "-" + hexs(l1, 3));
    bool ok = f0 == 0x40 && f1 == 0xC0 && l0 == l1 && l0 >= 0;
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii;
    g.expect_if(pre, ok, "P10: opus:AA-diwstrt.c5 sol:AA-diwstrt.c5: with DIWHIGH never written, the display window starts at line 0x40 for DIWSTRT = 0x4081 and at line 0xC0 for DIWSTRT = 0xC081 (V8 = 0 whatever V7 is), and its last line is the same in both");
    g_p10_ok = ok; g_p10_graded = pre;
}

void proposition_p11() {
    if (!want("P11")) return;
    std::vector<WCfg> cfg(2);
    cfg[0].diwstop = 0x90C1; cfg[1].diwstop = 0x10C1;
    WinOpts wo;
    win_pre_mutations("P11", cfg, wo, 0);
    if (mutating("P11")) cfg[1].w_diwstop = 0x90C1;          // 0x10C1 is written as 0x90C1
    if (mutating("P11-ii")) { cfg.back().diwhigh = 0x2100; cfg.back().diwstop = 0x90C1; cfg.back().w_diwstop.reset(); }
    WinRun& w = win_run("W11", cfg, wo);
    WinPropRun r = win_prop_pre("P11", w);
    int f0 = first_line(w.vf[0].win, 0x26, 0x12F), f1 = first_line(w.vf[1].win, 0x26, 0x12F);
    int l0 = last_line(w.vf[0].win, 0x26, 0x12F), l1 = last_line(w.vf[1].win, 0x26, 0x12F);
    checks.note("P11 measured (first-last line of Win): 0x90C1: " + hexs(f0, 3) + "-" + hexs(l0, 3) + "; 0x10C1: " + hexs(f1, 3) + "-" + hexs(l1, 3));
    bool ok = f0 == 0x40 && f1 == 0x40 && l0 >= 0 && l1 - l0 == 0x80;
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii;
    g.expect_if(pre, ok, "P11: opus:AA-diwstop.c5 sol:AA-diwstop.c5: with DIWHIGH never written, DIWSTOP = 0x10C1 puts the last line of the display window 0x80 lines after that of 0x90C1 (V8 = not V7), and the first line is 0x40 in both");
    g_p11_ok = ok; g_p11_graded = pre;
}

// ---- P12-P15: DIWHIGH --------------------------------------------------------------

// The preconditions of P12 to P15: fixture, RF-V, DMACONR, regions, and (ii) DIWHIGH is written after
// DIWSTRT and DIWSTOP in the same vertical blank, read from the command list.
bool diwhigh_last(const Script& s, size_t ncfg) {
    auto cmds = parse_script(s);
    size_t k = 0;
    int i_strt = -1, i_stop = -1, i_high = -1;
    int idx = 0;
    bool in = false, ok = true;
    for (const auto& c : cmds) {
        ++idx;
        if (c.op == 2) { in = true; i_strt = i_stop = i_high = -1; continue; }
        if (!in) continue;
        if (c.op == 1 && c.a[0] == DIWSTRT) i_strt = idx;
        if (c.op == 1 && c.a[0] == DIWSTOP) i_stop = idx;
        if (c.op == 1 && c.a[0] == DIWHIGH) i_high = idx;
        if (c.op == 4) {
            if (i_strt < 0 || i_stop < 0 || i_high < 0 || i_high < i_strt || i_high < i_stop) ok = false;
            ++k; in = false;
        }
    }
    return ok && k == ncfg;
}

struct HPre { WPre p; bool ii; };

HPre high_pre(const std::string& pn, WinRun& w) {
    HPre r;
    r.p = win_pre(pn, w);
    r.ii = diwhigh_last(w.sc, w.cfg.size());
    g.expect(r.ii, pn + "-ii: in every vertical blank of the run DIWHIGH is written after DIWSTRT and DIWSTOP");
    return r;
}

std::vector<WCfg> high_cfg(uint16_t strt, uint16_t stop, std::initializer_list<uint16_t> high) {
    std::vector<WCfg> cfg;
    for (auto h : high) { WCfg c; c.diwstrt = strt; c.diwstop = stop; c.diwhigh = h; cfg.push_back(c); }
    return cfg;
}

void high_pre_ii_mutation(const std::string& pn, std::vector<WCfg>& cfg) {
    if (mutating((pn + "-ii").c_str())) cfg.back().high_first = true;
}

void proposition_p12() {
    if (!want("P12")) return;
    auto cfg = high_cfg(0x2881, 0x2AC1, {0x2100, 0x2101});
    WinOpts wo;
    win_pre_mutations("P12", cfg, wo, 0);
    high_pre_ii_mutation("P12", cfg);
    if (mutating("P12")) cfg[1].w_diwhigh = 0x2100;      // 0x2101 is written as 0x2100: bit 0 stays clear
    if (mutating("P12-iii")) cfg[1].w_diwhigh = 0x2102;  // the second window starts at line 0x228
    WinRun& w = win_run("W12", cfg, wo);
    HPre r = high_pre("P12", w);
    int f0 = first_line(w.vf[0].win, 0x26, 0x12F), f1 = first_line(w.vf[1].win, 0x26, 0x12F);
    int l0 = last_line(w.vf[0].win, 0x26, 0x12F), l1 = last_line(w.vf[1].win, 0x26, 0x12F);
    checks.note("P12 measured (first-last line of Win): DIWHIGH 0x2100: " + hexs(f0, 3) + "-" + hexs(l0, 3) + "; 0x2101: " + hexs(f1, 3) + "-" + hexs(l1, 3));
    bool iii = f0 >= 0 && f1 >= 0;
    g.expect(iii, "P12-iii: both windows are non-empty");
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii && iii;
    g.expect_if(pre, f0 == 0x28 && f1 == 0x128 && l0 == l1, "P12: opus:AA-diwhigh.bit-b00: with DIWHIGH written after DIWSTRT = 0x2881 and DIWSTOP = 0x2AC1, setting bit 0 of DIWHIGH (0x2101 against 0x2100) moves the first line of the display window from 0x28 to 0x128 and leaves the last line");
}

void proposition_p13() {
    if (!want("P13")) return;
    auto cfg = high_cfg(0x2881, 0x2AC1, {0x2100, 0x2102});
    WinOpts wo;
    win_pre_mutations("P13", cfg, wo, 0);
    high_pre_ii_mutation("P13", cfg);
    if (mutating("P13")) cfg[1].w_diwhigh = 0x2100;      // 0x2102 is written as 0x2100: bit 1 stays clear
    if (mutating("P13-iii")) cfg[0].w_diwhigh = 0x2102;  // the base configuration writes the varied value
    WinRun& w = win_run("W13", cfg, wo);
    HPre r = high_pre("P13", w);
    int f0 = first_line(w.vf[0].win, 0x1D, 0x1FF), l0 = last_line(w.vf[0].win, 0x1D, 0x1FF);
    int f1 = first_line(w.vf[1].win, 0x1D, 0x1FF);
    checks.note("P13 measured (first-last line of Win, from line 0x1D): DIWHIGH 0x2100: " + hexs(f0, 3) + "-" + hexs(l0, 3) + "; 0x2102: " + (f1 < 0 ? std::string("empty") : hexs(f1, 3)));
    bool iii = f0 >= 0;
    g.expect(iii, "P13-iii: the base window is non-empty");
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii && iii;
    g.expect_if(pre, f0 == 0x28 && f1 < 0, "P13: opus:AA-diwhigh.bit-b01: with DIWHIGH written after DIWSTRT = 0x2881 and DIWSTOP = 0x2AC1, DIWHIGH = 0x2100 gives a window from line 0x28, and setting bit 1 (0x2102, start V9) puts the start at line 0x228, which a PAL field does not reach, so no window line appears");
}

void proposition_p14() {
    if (!want("P14")) return;
    auto cfg = high_cfg(0x2881, 0x2AC1, {0x2000, 0x2100});
    WinOpts wo;
    win_pre_mutations("P14", cfg, wo, 0);
    high_pre_ii_mutation("P14", cfg);
    if (mutating("P14")) cfg[1].w_diwhigh = 0x2000;      // 0x2100 is written as 0x2000: bit 8 stays clear
    if (mutating("P14-iii")) { cfg[0].w_diwhigh = 0x2102; }  // the first window starts at 0x228
    WinRun& w = win_run("W14", cfg, wo);
    HPre r = high_pre("P14", w);
    int f0 = first_line(w.vf[0].win, 0x26, 0x12F), f1 = first_line(w.vf[1].win, 0x26, 0x12F);
    int l0 = last_line(w.vf[0].win, 0x26, 0x12F), l1 = last_line(w.vf[1].win, 0x26, 0x12F);
    checks.note("P14 measured (first-last line of Win): DIWHIGH 0x2000: " + hexs(f0, 3) + "-" + hexs(l0, 3) + "; 0x2100: " + hexs(f1, 3) + "-" + hexs(l1, 3));
    bool iii = f0 >= 0 && f1 >= 0;
    g.expect(iii, "P14-iii: both windows are non-empty");
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii && iii;
    g.expect_if(pre, f0 == 0x28 && f1 == 0x28 && l1 - l0 == 0x100, "P14: opus:AA-diwhigh.bit-b08: with DIWHIGH written after DIWSTRT = 0x2881 and DIWSTOP = 0x2AC1, setting bit 8 of DIWHIGH (0x2100 against 0x2000) moves the last line of the display window exactly 0x100 lines later, and the first line is 0x28 in both");
}

void proposition_p15() {
    if (!want("P15")) return;
    auto cfg = high_cfg(0x2881, 0x10C1, {0x2100, 0x2300});
    WinOpts wo;
    win_pre_mutations("P15", cfg, wo, 0);
    high_pre_ii_mutation("P15", cfg);
    if (mutating("P15")) cfg[1].w_diwhigh = 0x2100;      // 0x2300 is written as 0x2100: bit 9 stays clear
    if (mutating("P15-iii")) cfg[1].w_diwstrt = 0x2681;  // the varied window starts at line 0x26, before line 0x28
    WinRun& w = win_run("W15", cfg, wo);
    HPre r = high_pre("P15", w);
    const auto& b = w.vf[0].win;
    const auto& v = w.vf[1].win;
    int fb = first_line(b, 0x26, 0x120), lb = last_line(b, 0x26, 0x120);
    bool base_ok = fb == 0x28 && (lb == 0x10F || lb == 0x110) && contiguous(b, 0x26, 0x120);
    for (int x = lb + 1; x <= 0x120; ++x) if (b.count(x)) base_ok = false;
    bool var_ok = true;
    for (int x = 0x28; x <= 0x120; ++x) if (!v.count(x)) var_ok = false;
    checks.note("P15 measured (first-last line of Win up to 0x120): DIWHIGH 0x2100: " + hexs(fb, 3) + "-" + hexs(lb, 3) + "; 0x2300: " + hexs(first_line(v, 0x26, 0x120), 3) + "-" + hexs(last_line(v, 0x26, 0x120), 3));
    bool iii = true;
    for (int x = 0x1D; x < 0x28; ++x) if (v.count(x)) iii = false;
    iii = iii && fb >= 0;
    g.expect(iii, "P15-iii: before line 0x28 of the varied configuration's observed field no line is in Win, the base window had closed");
    bool pre = config_ok(g_cfg_used) && r.p.fix && r.p.rfv && r.p.dm && r.p.reg && r.ii && iii;
    g.expect_if(pre, base_ok && var_ok, "P15: opus:AA-diwhigh.bit-b09: with DIWHIGH written after DIWSTRT = 0x2881 and DIWSTOP = 0x10C1, DIWHIGH = 0x2100 closes the window at line 0x110 (the last line is 0x10F or 0x110, no later line up to 0x120 is in Win), and setting bit 9 (0x2300, stop 0x310) keeps every line from 0x28 through 0x120 in Win");
}

// ---- the joint assertions -------------------------------------------------------------

void joint_assertions() {
    if (want("J1")) {
        g.expect_if(g_p1_graded && g_p2_graded, g_p1_ok && g_p2_ok,
            "J1: opus:AA-ddf.c5 opus:AA-ddf.use-b03 opus:AA-ddf.use-b04 opus:AA-ddf.use-b05 opus:AA-ddf.use-b06 opus:AA-ddf.use-b07: bits 3-7 of DDFSTRT and of DDFSTOP (H4-H8) set the beginning and the end of the bit-plane fetch with the weights 8, 16, 32, 64 and 128 memory cycles: P1 and P2 hold");
    }
    if (want("J2")) {
        g.expect_if(g_p8_graded && g_p9_graded, g_p8_ok && g_p9_ok,
            "J2: opus:AA-diw.use-b08 opus:AA-diw.use-b09 opus:AA-diw.use-b10 opus:AA-diw.use-b11 opus:AA-diw.use-b12 opus:AA-diw.use-b13 opus:AA-diw.use-b14: bits 8-14 of DIWSTRT and of DIWSTOP are the vertical start and stop V0-V6: P8 and P9 hold");
    }
    if (want("J3")) {
        g.expect_if(g_p10_graded && g_p11_graded, g_p10_ok && g_p11_ok,
            "J3: opus:AA-diw.use-b15: bit 15 of DIWSTRT and of DIWSTOP is V7, with V8 = 0 for the start and V8 = not V7 for the stop: P10 and P11 hold");
    }
}

// ---- P16-P23: DMACON and DMACONR ----------------------------------------------------------

struct DRun {
    std::shared_ptr<RunData> r;
    Script sc;
    bool fix = false;
    uint16_t copy = 0;
};

DRun drun(const Script& s) {
    DRun d;
    d.sc = s;
    d.r = run_script(s);
    d.fix = fixture_ok(*d.r);
    if (!d.r->dmacon_copies.empty()) d.copy = d.r->dmacon_copies[0].data;
    return d;
}

// RF-F: consecutive spacings of the stores to MARK agree to within 1 %.
bool rff_ok(const RunData& r, size_t need) {
    std::vector<uint64_t> t;
    for (const auto& m : r.marks) t.push_back(m.cyc);
    if (t.size() < need) return false;
    t.resize(need);
    int64_t s0 = int64_t(t[1]) - int64_t(t[0]);
    if (s0 <= 0) return false;
    for (size_t i = 1; i + 1 < t.size(); ++i) {
        int64_t s = int64_t(t[i + 1]) - int64_t(t[i]);
        if (s <= 0 || std::llabs(s - s0) * 100 > s0) return false;
    }
    return true;
}

std::pair<uint64_t, uint64_t> field_k(const RunData& r, size_t k) {
    uint64_t a = 0, b = 0;
    for (const auto& m : r.marks) { if (m.data == k) a = m.cyc; if (m.data == k + 1) b = m.cyc; }
    return {a, b};
}

// The vertical-blank loop of 2.2 with `nmarks` stores to MARK; `body` writes the registers of each.
void vb_loop(Script& s, int nmarks, const std::function<void(Script&)>& body, bool skip_first_wl30 = false, bool stray_field0 = false) {
    s.WL30();
    for (int k = 0; k < nmarks; ++k) {
        s.WVB();
        body(s);
        s.MARK(uint16_t(k));
        if (stray_field0 && k == 0) s.STORE(kCop + 0x20, 0x1234);
        if (!(skip_first_wl30 && k == 0)) s.WL30();
    }
    s.WVB();
    s.END();
}

std::string kind_note(bool a, bool b) { return std::string(a ? "yes" : "no") + "/" + (b ? "yes" : "no"); }

// ---- P16 BPLEN ----

struct P16Opts { uint16_t dmacon; uint16_t bplcon0 = 0x4000; bool fix = true; bool skip_wl30 = false; bool copy_first = false; bool stray = false; };

Script s16(const P16Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    script_fill_planes(s, 4);
    s.W(BPLCON0, o.bplcon0); s.W(BPLCON1, 0); s.W(BPLCON2, 0); s.W(BPLCON3, 0x0C00); s.W(FMODE, 0);
    s.W(DIWSTRT, 0x4081); s.W(DIWSTOP, 0xF0C1); s.W(DDFSTRT, 0x0038); s.W(DDFSTOP, 0x00D0);
    s.W(BPL1MOD, 0); s.W(BPL2MOD, 0);
    if (o.copy_first) s.CPYD();
    s.W(DMACON, o.dmacon);
    if (!o.copy_first) s.CPYD();
    vb_loop(s, 3, [&](Script& t) {
        const uint32_t b[4] = {kB1, kB2, kB3, kB4};
        for (int p = 0; p < 4; ++p) { t.W(BPL1PTH + 4 * p, uint16_t(b[p] >> 16)); t.W(BPL1PTL + 4 * p, uint16_t(b[p])); }
    }, o.skip_wl30, false);
    if (o.stray) {
        // a CPU write into B1 during the observation: replaces the end of the list
        Script t = s;
        t.w.pop_back();                  // END
        t.STORE(kB1 + 0x7000, 0x1234); t.WL30(); t.END();
        return t;
    }
    return s;
}

void proposition_p16() {
    if (!want("P16")) return;
    P16Opts a, b;
    a.dmacon = 0x8300; b.dmacon = 0x8200;
    if (mutating("P16-fix")) { a.fix = false; b.fix = false; }
    if (mutating("P16-i")) a.skip_wl30 = true;
    if (mutating("P16-ii")) a.dmacon = 0x8100;
    if (mutating("P16-iii")) { a.bplcon0 = 0x0000; b.bplcon0 = 0x0000; }
    if (mutating("P16-reg")) a.stray = true;
    if (mutating("P16")) b.dmacon = 0x8300;           // the contrast run keeps BPLEN set
    DRun ra = drun(s16(a)), rb = drun(s16(b));
    g.expect(ra.fix && rb.fix, "P16-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    bool i = rff_ok(*ra.r, 3) && rff_ok(*rb.r, 3);
    g.expect(i, "P16-i: the spacings of the stores to MARK in the vertical-blank loop agree to within 1 % in both runs (RF-F)");
    bool ii = (ra.copy & 0x0300) == 0x0300 && (rb.copy & 0x0300) == 0x0200;
    g.expect(ii, "P16-ii: the stored DMACONR shows bit 9 (DMAEN) set and bit 8 as written (1 in the first run, 0 in the second)");
    auto bpu = [](const DRun& d) { auto v = script_value(d.sc, BPLCON0); return v ? (*v >> 12) & 7 : 0; };
    bool iii = bpu(ra) != 0 && bpu(rb) != 0;
    g.expect(iii, "P16-iii: BPU is not 0 in either run (BPLCON0 = 0x4000, BPU = 4)");
    bool reg = true;
    for (const DRun* d : {&ra, &rb}) {
        auto f = std::make_pair(field_k(*d->r, 0).first, field_k(*d->r, 1).second);
        reg = reg && regions_ok(*d->r, f.first, d->r->ncycles, {{kB1, kB1 + kPlaneLen, false}, {kB2, kB2 + kPlaneLen, false}, {kB3, kB3 + kPlaneLen, false}, {kB4, kB4 + kPlaneLen, false}})
              && script_clean(d->sc, {{kB1, kB1 + kPlaneLen}, {kB2, kB2 + kPlaneLen}, {kB3, kB3 + kPlaneLen}, {kB4, kB4 + kPlaneLen}});
    }
    g.expect(reg, "P16-reg: during the observation the regions B1-B4 are read only and no instruction of the program writes into them after DMA is enabled");
    const uint32_t reg_lo[4] = {kB1, kB2, kB3, kB4};
    bool on = true, off = true;
    std::string note;
    for (size_t f = 0; f < 2; ++f) {
        auto fa = field_k(*ra.r, f), fb = field_k(*rb.r, f);
        for (int p = 0; p < 4; ++p) {
            size_t na = reads(*ra.r, fa.first, fa.second, reg_lo[p], reg_lo[p] + kPlaneLen).size();
            size_t nb = reads(*rb.r, fb.first, fb.second, reg_lo[p], reg_lo[p] + kPlaneLen).size();
            if (na == 0) on = false;
            if (nb != 0) off = false;
            note += (note.empty() ? "" : ", ") + dec(na) + "/" + dec(nb);
        }
    }
    checks.note("P16 measured (reads per field and plane, BPLEN set / clear): " + note);
    bool pre = config_ok(g_cfg_used) && ra.fix && rb.fix && i && ii && iii && reg;
    g.expect_if(pre, on && off, "P16: opus:AA-dmacon.b08: with DMAEN set, each of the two fields after the DMACON write has at least one bit-plane read in each of B1, B2, B3 and B4 when BPLEN is set, and neither field has a read in B1-B4 when BPLEN is clear");
}

// ---- P17 COPEN ----

struct P17Opts { uint16_t dmacon; bool fix = true; bool skip_wl30 = false; bool stray = false; };

Script s17(const P17Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    s.COPYW(kCop, {COLOR00, 0x0000, 0xFFFF, 0xFFFE});
    s.W(COP1LCH, uint16_t(kCop >> 16)); s.W(COP1LCL, uint16_t(kCop)); s.W(COPJMP1, 0);
    s.W(DMACON, o.dmacon); s.CPYD();
    vb_loop(s, 3, [](Script&) {}, o.skip_wl30, o.stray);
    return s;
}

void proposition_p17() {
    if (!want("P17")) return;
    P17Opts a, b;
    a.dmacon = 0x8280; b.dmacon = 0x8200;
    if (mutating("P17-fix")) { a.fix = false; b.fix = false; }
    if (mutating("P17-i")) a.skip_wl30 = true;
    if (mutating("P17-ii")) a.dmacon = 0x8080;
    if (mutating("P17-iii")) a.stray = true;
    if (mutating("P17")) b.dmacon = 0x8280;
    DRun ra = drun(s17(a)), rb = drun(s17(b));
    g.expect(ra.fix && rb.fix, "P17-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    bool i = rff_ok(*ra.r, 3) && rff_ok(*rb.r, 3);
    g.expect(i, "P17-i: the spacings of the stores to MARK in the vertical-blank loop agree to within 1 % in both runs (RF-F)");
    bool ii = (ra.copy & 0x0280) == 0x0280 && (rb.copy & 0x0280) == 0x0200;
    g.expect(ii, "P17-ii: the stored DMACONR shows bit 9 (DMAEN) set and bit 7 as written (1 in the first run, 0 in the second)");
    bool iii = true;
    for (const DRun* d : {&ra, &rb}) {
        auto f = field_k(*d->r, 0);
        iii = iii && regions_ok(*d->r, f.first, d->r->ncycles, {{kCop, kCopEnd, false}}) && script_clean(d->sc, {{kCop, kCopEnd}});
    }
    g.expect(iii, "P17-iii: during the observation COP is read only and no instruction of the program writes into it after DMA is enabled");
    auto f0a = field_k(*ra.r, 0), f1a = field_k(*ra.r, 1), f0b = field_k(*rb.r, 0), f1b = field_k(*rb.r, 1);
    size_t na = reads(*ra.r, f0a.first, f1a.second, kCop, kCopEnd).size();
    size_t nb = reads(*rb.r, f0b.first, f1b.second, kCop, kCopEnd).size();
    checks.note("P17 measured (reads in COP in the two fields, COPEN set / clear): " + dec((long long)na) + " / " + dec((long long)nb));
    bool pre = config_ok(g_cfg_used) && ra.fix && rb.fix && i && ii && iii;
    g.expect_if(pre, na >= 1 && nb == 0, "P17: opus:AA-dmacon.b07: with DMAEN set, the Copper reads its list (at least one read in COP in the two fields after the DMACON write) when COPEN is set, and makes no read in COP when COPEN is clear");
}

// ---- P18 BLTEN ----

struct P18Opts { uint16_t dmacon; bool fix = true; bool stray = false; };

void blit_setup(Script& s, uint16_t con0) {
    s.W(BLTCON0, con0); s.W(BLTCON1, 0); s.W(BLTAFWM, 0xFFFF); s.W(BLTALWM, 0xFFFF);
    s.W(BLTAMOD, 0); s.W(BLTDMOD, 0);
    s.W(BLTAPTH, uint16_t(kBltS >> 16)); s.W(BLTAPTL, uint16_t(kBltS));
    s.W(BLTDPTH, uint16_t(kBltD >> 16)); s.W(BLTDPTL, uint16_t(kBltD));
}

Script s18(const P18Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    s.FILLL(kBltS, 0x0100, 0x00010001);
    s.W(DMACON, o.dmacon); s.CPYD();
    blit_setup(s, 0x09F0);
    s.WBLT(BLTSIZE, 0x0410, 1);
    if (o.stray) s.STORE(kBltS + 0x400, 0x1234);
    s.WVB(); s.WL30(); s.WVB(); s.WL30();
    s.STORE(kMarkEndWord, kEndMark);
    s.END();
    return s;
}

void proposition_p18() {
    if (!want("P18")) return;
    P18Opts a, b;
    a.dmacon = 0x8240; b.dmacon = 0x8200;
    if (mutating("P18-fix")) { a.fix = false; b.fix = false; }
    if (mutating("P18-i")) a.dmacon = 0x8040;
    if (mutating("P18-ii")) a.stray = true;
    if (mutating("P18")) b.dmacon = 0x8240;
    DRun ra = drun(s18(a)), rb = drun(s18(b));
    g.expect(ra.fix && rb.fix, "P18-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    bool i = (ra.copy & 0x0240) == 0x0240 && (rb.copy & 0x0240) == 0x0200;
    g.expect(i, "P18-i: the stored DMACONR shows bit 9 (DMAEN) set and bit 6 as written (1 in the first run, 0 in the second)");
    auto window = [](const DRun& d) {
        uint64_t t0 = 0, t1 = 0;
        for (const auto& m : d.r->marks) if (m.data == 1) t0 = m.cyc;
        if (!d.r->endmarks.empty()) t1 = d.r->endmarks.front().cyc;
        return std::make_pair(t0, t1);
    };
    bool ii = true;
    for (const DRun* d : {&ra, &rb}) {
        auto w = window(*d);
        ii = ii && w.first && w.second && regions_ok(*d->r, w.first, w.second, {{kBltS, kBltSEnd, false}}) && script_clean(d->sc, {{kBltS, kBltSEnd}, {kBltD, kBltDEnd}});
    }
    g.expect(ii, "P18-ii: between the MARK store after the BLTSIZE write and the end marker BLT_S is read only and no instruction of the program writes into BLT_S or BLT_D after DMA is enabled");
    auto wa = window(ra), wb = window(rb);
    size_t sa = reads(*ra.r, wa.first, wa.second, kBltS, kBltSEnd).size(), da = writes(*ra.r, wa.first, wa.second, kBltD, kBltDEnd).size();
    size_t sb = reads(*rb.r, wb.first, wb.second, kBltS, kBltSEnd).size(), db = writes(*rb.r, wb.first, wb.second, kBltD, kBltDEnd).size();
    checks.note("P18 measured (BLT_S reads / BLT_D writes): BLTEN set " + dec((long long)sa) + " / " + dec((long long)da) + ", BLTEN clear " + dec((long long)sb) + " / " + dec((long long)db));
    bool pre = config_ok(g_cfg_used) && ra.fix && rb.fix && i && ii;
    g.expect_if(pre, sa >= 1 && da >= 1 && sb == 0 && db == 0, "P18: opus:AA-dmacon.b06: with DMAEN set, a started blit (BLTSIZE 0x0410, A and D channels) reads BLT_S and writes BLT_D (at least one of each) when BLTEN is set, and makes no read in BLT_S and no write in BLT_D when BLTEN is clear");
}

// ---- P19 SPREN ----

struct P19Opts { uint16_t dmacon; bool fix = true; bool skip_wl30 = false; bool stray = false; };

Script s19(const P19Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    for (int x = 0; x < 8; ++x) {
        std::vector<uint16_t> d = {uint16_t(0x6040 + 8 * x), 0x6800};
        for (int l = 0; l < 8; ++l) { d.push_back(0xFFFF); d.push_back(0x0000); }
        d.push_back(0); d.push_back(0);
        s.COPYW(kSprBase + x * kSprStep, d);
    }
    s.W(BPLCON3, 0x0C00); s.W(FMODE, 0);
    s.W(DMACON, o.dmacon); s.CPYD();
    vb_loop(s, 3, [](Script& t) {
        for (int x = 0; x < 8; ++x) { uint32_t a = kSprBase + x * kSprStep; t.W(SPR0PTH + 4 * x, uint16_t(a >> 16)); t.W(SPR0PTH + 4 * x + 2, uint16_t(a)); }
    }, o.skip_wl30, false);
    if (o.stray) { s.w.pop_back(); s.STORE(kSprBase + 0x10, 0x1234); s.WL30(); s.END(); }
    return s;
}

void proposition_p19() {
    if (!want("P19")) return;
    P19Opts a, b;
    a.dmacon = 0x8220; b.dmacon = 0x8200;
    if (mutating("P19-fix")) { a.fix = false; b.fix = false; }
    if (mutating("P19-i")) a.skip_wl30 = true;
    if (mutating("P19-ii")) a.dmacon = 0x8020;
    if (mutating("P19-reg")) a.stray = true;
    if (mutating("P19")) b.dmacon = 0x8220;
    DRun ra = drun(s19(a)), rb = drun(s19(b));
    g.expect(ra.fix && rb.fix, "P19-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    bool i = rff_ok(*ra.r, 3) && rff_ok(*rb.r, 3);
    g.expect(i, "P19-i: the spacings of the stores to MARK in the vertical-blank loop agree to within 1 % in both runs (RF-F)");
    bool ii = (ra.copy & 0x0220) == 0x0220 && (rb.copy & 0x0220) == 0x0200;
    g.expect(ii, "P19-ii: the stored DMACONR shows bit 9 (DMAEN) set and bit 5 as written (1 in the first run, 0 in the second)");
    std::vector<Region> regs;
    std::vector<std::pair<uint32_t, uint32_t>> rr;
    for (int x = 0; x < 8; ++x) { regs.push_back({kSprBase + x * kSprStep, kSprBase + x * kSprStep + 0x100, false}); rr.push_back({kSprBase + x * kSprStep, kSprBase + x * kSprStep + 0x100}); }
    bool reg = true;
    for (const DRun* d : {&ra, &rb}) reg = reg && regions_ok(*d->r, field_k(*d->r, 0).first, d->r->ncycles, regs) && script_clean(d->sc, rr);
    g.expect(reg, "P19-reg: during the observation the regions SPR0-SPR7 are read only and no instruction of the program writes into them after DMA is enabled");
    bool on = true, off = true;
    std::string note;
    for (size_t f = 0; f < 2; ++f) {
        auto fa = field_k(*ra.r, f), fb = field_k(*rb.r, f);
        for (int x = 0; x < 8; ++x) {
            uint32_t lo = kSprBase + x * kSprStep;
            size_t na = reads(*ra.r, fa.first, fa.second, lo, lo + 0x100).size();
            size_t nb = reads(*rb.r, fb.first, fb.second, lo, lo + 0x100).size();
            if (na == 0) on = false;
            if (nb != 0) off = false;
            note += (note.empty() ? "" : ",") + dec(na) + "/" + dec(nb);
        }
    }
    checks.note("P19 measured (reads per field and sprite, SPREN set / clear): " + note);
    bool pre = config_ok(g_cfg_used) && ra.fix && rb.fix && i && ii && reg;
    g.expect_if(pre, on && off, "P19: opus:AA-dmacon.b05: with DMAEN set, each of the two fields after the DMACON write has at least one sprite read in each of the eight regions SPR0-SPR7 when SPREN is set, and no read in any of them when SPREN is clear");
}

// ---- P20 BLTPRI ------------------------------------------------------------------------------

struct P20Opts {
    uint16_t dmacon = 0x8640;
    uint16_t size = 0x2020;
    uint16_t iterations = 20000;
    bool fix = true;
    bool wait_after = false;     // wait for the blit to end before the end marker
    bool copy_first = false;     // the DMACONR copy is stored before the DMACON write
    bool bpl_on = false;         // (iii) contrast: bit-plane DMA runs while the blit runs
};

Script s20(const P20Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    s.FILLL(kBltS, 0x0800, 0x00010001);
    if (o.bpl_on) {
        s.ZERO(kZ, 0x1000);
        s.W(BPLCON0, 0x1000); s.W(BPLCON1, 0); s.W(BPLCON2, 0); s.W(BPLCON3, 0x0C00); s.W(FMODE, 0);
        s.W(DIWSTRT, 0x4081); s.W(DIWSTOP, 0xF0C1); s.W(DDFSTRT, 0x0038); s.W(DDFSTOP, 0x00D0);
        s.W(BPL1MOD, 0); s.W(BPL2MOD, 0);
        s.W(BPL1PTH, uint16_t(kZ >> 16)); s.W(BPL1PTL, uint16_t(kZ));
    }
    if (o.copy_first) s.CPYD();
    s.W(DMACON, o.bpl_on ? uint16_t(o.dmacon | 0x0100) : o.dmacon);
    if (!o.copy_first) s.CPYD();
    blit_setup(s, 0x09F0);
    if (o.bpl_on) { s.WVB(); s.WLN(0x60); }
    s.RUN20(o.size, o.iterations);
    if (o.wait_after) { s.WVB(); s.WL30(); }
    s.STORE(kMarkEndWord, kEndMark);
    s.END();
    return s;
}

struct P20Res {
    DRun d;
    uint64_t first = 0, last = 0;
    size_t nS = 0, nD = 0;
    size_t cpu_in = 0;
    bool loop_ok = false;
};

int64_t measured_P() {
    FetchRun& fr = run_r1();
    return fr.P;
}

P20Res analyse20(const Script& s, int64_t P) {
    P20Res r;
    r.d = drun(s);
    const RunData& d = *r.d.r;
    uint64_t f = ~0ull, l = 0;
    for (const auto& a : d.acc) {
        bool blit = (!a.wr && a.byte >= kBltS && a.byte < kBltSEnd) || (a.wr && a.byte >= kBltD && a.byte < kBltDEnd);
        if (!blit) continue;
        f = std::min(f, a.cyc); l = std::max(l, a.cyc);
        if (a.wr) ++r.nD; else ++r.nS;
    }
    if (f == ~0ull) f = 0;
    r.first = f; r.last = l;
    // the CPU's accesses: reads in PROG, writes in SCR and MARK
    std::vector<uint64_t> cpu, scr;
    for (const auto& a : d.acc) {
        if (!a.wr && a.byte >= kProg && a.byte < kProgEnd) cpu.push_back(a.cyc);
        if (a.wr && a.byte >= kScr && a.byte < kScrEnd) { cpu.push_back(a.cyc); scr.push_back(a.cyc); }
        if (a.wr && a.byte >= kMark && a.byte < kMarkEnd) cpu.push_back(a.cyc);
    }
    for (uint64_t t : cpu) if (t > r.first && t < r.last) { ++r.cpu_in; if (r.cpu_in <= 6) dbg("CPU access in I at +" + std::to_string(t - r.first) + " of " + std::to_string(r.last - r.first)); }
    // (ii): an access within 64 memory cycles before I and after it, and SCR writes after I
    uint64_t win = uint64_t(64 * P);
    bool before = false, after = false, scr_after = false;
    for (uint64_t t : cpu) {
        if (t < r.first && r.first - t < win) before = true;
        if (t > r.last && t - r.last < win) after = true;
    }
    for (uint64_t t : scr) if (t > r.last) scr_after = true;
    r.loop_ok = before && after && scr_after;
    return r;
}

void proposition_p20() {
    if (!want("P20")) return;
    P20Opts a, b;                         // a: BLTPRI set, b: BLTPRI clear
    b.dmacon = 0x8240;
    if (mutating("P20-fix")) { a.fix = false; b.fix = false; }
    if (mutating("P20-i")) a.size = 0x2010;
    if (mutating("P20-ii")) { b.iterations = 10; b.wait_after = true; }
    if (mutating("P20-iii")) a.bpl_on = true;
    if (mutating("P20-iv")) a.copy_first = true;
    if (mutating("P20")) a.dmacon = 0x8240;            // the BLTPRI run keeps BLTPRI clear
    int64_t P = measured_P();
    P20Res ra = analyse20(s20(a), P), rb = analyse20(s20(b), P);
    g.expect(ra.d.fix && rb.d.fix, "P20-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    const size_t want_n = 4096;
    bool i = ra.nS == want_n && ra.nD == want_n && rb.nS == want_n && rb.nD == want_n;
    g.expect(i, "P20-i: the blit completes in both runs: 4096 reads in BLT_S and 4096 writes in BLT_D (height 128 x width 32); measured " + dec((long long)ra.nS) + "/" + dec((long long)ra.nD) + " and " + dec((long long)rb.nS) + "/" + dec((long long)rb.nD));
    bool ii = ra.loop_ok && rb.loop_ok;
    g.expect(ii, "P20-ii: the CPU is inside its loop throughout the blit: it makes an access less than 64 memory cycles (P = " + dec(P) + " system clock cycles each) before the blit's first access and after its last, and SCR writes continue after the blit, in both runs");
    bool iii = true;
    for (const P20Res* r : {&ra, &rb}) {
        const RunData& d = *r->d.r;
        for (uint32_t base : {kB1, kB2, kB3, kB4, kZ, kCop})
            iii = iii && accs(d, r->first, r->last, base, base + 0x8000, false).empty() && accs(d, r->first, r->last, base, base + 0x8000, true).empty();
        for (int x = 0; x < 8; ++x) {
            uint32_t lo = kSprBase + x * kSprStep;
            iii = iii && accs(d, r->first, r->last, lo, lo + 0x100, false).empty();
        }
    }
    g.expect(iii, "P20-iii: no bit-plane, sprite or Copper access (regions B1-B4, Z, SPR0-SPR7, COP) occurs in the blit's interval in either run");
    auto wr = [](const DRun& d) { auto v = script_value(d.sc, DMACON); return v ? *v : uint16_t(0); };
    bool iv = ((ra.d.copy ^ wr(ra.d)) & 0x0640) == 0 && ((rb.d.copy ^ wr(rb.d)) & 0x0640) == 0 && (ra.d.copy & 0x0200) && (rb.d.copy & 0x0200);
    g.expect(iv, "P20-iv: the stored DMACONR shows bits 10, 9 and 6 as written (BLTPRI set in the first run and clear in the second, DMAEN and BLTEN set in both)");
    checks.note("P20 measured (CPU accesses inside the blit's interval, BLTPRI set / clear): " + dec((long long)ra.cpu_in) + " / " + dec((long long)rb.cpu_in));
    bool pre = config_ok(g_cfg_used) && ra.d.fix && rb.d.fix && i && ii && iii && iv;
    g.expect_if(pre, ra.cpu_in == 0 && rb.cpu_in >= 1, "P20: opus:AA-dmacon.b10: with BLTPRI set the CPU makes no Chip-memory access (no PROG read, SCR write or MARK write) between the first and the last access of a running blit, and with BLTPRI clear it makes at least one");
}

// ---- P21-P23: BBUSY and BZERO ----------------------------------------------------------

struct Samp { uint64_t t_prev, t; uint16_t v; };

// The samples of a run: write j of SAMP is bracketed by write j-1; the first write is the bracketing store.
std::vector<Samp> samples(const RunData& r) {
    std::vector<Samp> out;
    for (size_t j = 1; j < r.samp.size(); ++j) out.push_back({r.samp[j - 1].cyc, r.samp[j].cyc, r.samp[j].data});
    return out;
}

struct BlitWin { uint64_t first = 0, last = 0; size_t n = 0; };
BlitWin blit_window(const RunData& r) {
    BlitWin w;
    uint64_t f = ~0ull, l = 0;
    for (const auto& a : r.acc) if (a.wr && a.byte >= kBltD && a.byte < kBltDEnd) { f = std::min(f, a.cyc); l = std::max(l, a.cyc); ++w.n; }
    if (f != ~0ull) { w.first = f; w.last = l; }
    return w;
}
bool inside(const Samp& s, const BlitWin& w) { return w.n && s.t_prev > w.first && s.t_prev < w.last && s.t > w.first && s.t < w.last; }

struct P21Opts {
    uint16_t dmacon = 0x8240;
    uint16_t size = 0x2020;
    int phase_a = 16;
    bool fix = true;
    bool copy_first = false;
    bool wait_before_b = false;         // (ii) contrast: two vertical blanks between BLTSIZE and phase B
    bool blit_in_a = false;             // mutation: BLTSIZE is written at the start of phase A
};

Script s21(const P21Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    if (o.copy_first) s.CPYD();
    s.W(DMACON, o.dmacon);
    if (!o.copy_first) s.CPYD();
    s.W(BLTCON0, 0x0100); s.W(BLTCON1, 0); s.W(BLTDMOD, 0);
    s.W(BLTDPTH, uint16_t(kBltD >> 16)); s.W(BLTDPTL, uint16_t(kBltD));
    s.SSTORE(0xFFFF);
    if (o.blit_in_a) {
        s.WSAMP(BLTSIZE, o.size, uint16_t(o.phase_a));
        s.SAMPLE(3000);
    } else {
        s.SAMPLE(uint16_t(o.phase_a));
        if (o.wait_before_b) {
            s.W(BLTSIZE, o.size);
            s.WVB(); s.WL30(); s.WVB(); s.WL30();
            s.SAMPLE(3000);
        } else s.WSAMP(BLTSIZE, o.size, 3000);
    }
    s.WVB(); s.WL30();
    s.SAMPLE(16);
    s.END();
    return s;
}

struct P21Res { DRun d; BlitWin w; std::vector<Samp> sm; int a = 0; };

P21Res analyse21(const P21Opts& o) {
    P21Res r;
    r.a = o.phase_a;
    r.d = drun(s21(o));
    r.w = blit_window(*r.d.r);
    r.sm = samples(*r.d.r);
    return r;
}

bool bbusy(const Samp& s) { return (s.v & 0x4000) != 0; }

void proposition_p21() {
    if (!want("P21")) return;
    P21Opts a, b;                         // a: phase A of 16 samples, b: phase A of 4000 samples
    b.phase_a = 4000;
    if (mutating("P21-fix")) { a.fix = false; b.fix = false; }
    if (mutating("P21-i")) a.size = 0x2010;
    if (mutating("P21-ii")) a.wait_before_b = true;
    if (mutating("P21-iii")) a.copy_first = true;
    if (mutating("P21")) a.blit_in_a = true;
    P21Res ra = analyse21(a), rb = analyse21(b);
    g.expect(ra.d.fix && rb.d.fix, "P21-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    bool i = ra.w.n == 4096 && rb.w.n == 4096;
    g.expect(i, "P21-i: the blit completes in both runs: 4096 writes in BLT_D; measured " + dec((long long)ra.w.n) + " and " + dec((long long)rb.w.n));
    auto count_in = [](const P21Res& r) {
        int n = 0;
        for (size_t j = size_t(r.a); j < r.sm.size() && j < size_t(r.a) + 3000; ++j) if (inside(r.sm[j], r.w)) ++n;
        return n;
    };
    int ina = count_in(ra), inb = count_in(rb);
    bool ii = ina >= 1 && inb >= 1;
    g.expect(ii, "P21-ii: at least one phase-B sample is inside the blit in each run (" + dec(ina) + " and " + dec(inb) + ")");
    bool iii = (ra.d.copy & 0x0240) == 0x0240 && (rb.d.copy & 0x0240) == 0x0240;
    g.expect(iii, "P21-iii: the stored DMACONR shows DMAEN and BLTEN set in both runs");
    bool zero_a = true, one_b = true, zero_c = true;
    for (const P21Res* r : {&ra, &rb}) {
        size_t na = size_t(r->a);
        for (size_t j = 0; j < r->sm.size(); ++j) {
            const Samp& s = r->sm[j];
            if (j < na) { if (bbusy(s)) zero_a = false; }
            else if (j < na + 3000) { if (inside(s, r->w) && !bbusy(s)) one_b = false; }
            else { if (bbusy(s)) zero_c = false; }
        }
        if (r->sm.size() != na + 3000 + 16) zero_c = false;
    }
    checks.note("P21 measured: phase-B samples inside the blit " + dec(ina) + " and " + dec(inb) + "; phase A all clear: " + (zero_a ? "yes" : "no") + ", inside samples all set: " + (one_b ? "yes" : "no") + ", phase C all clear: " + (zero_c ? "yes" : "no"));
    bool pre = config_ok(g_cfg_used) && ra.d.fix && rb.d.fix && i && ii && iii;
    g.expect_if(pre, zero_a && one_b && zero_c, "P21: opus:AA-dmacon.b14: DMACONR bit 14 (BBUSY) reads 0 in every sample before the blit is started (16 samples, and 4000 samples in the second run), 1 in every sample inside the running blit (BLT_D writes 0x2020), and 0 in every sample after it has ended");
}

// P22: BBUSY cannot be written.
struct P22Opts {
    bool fix = true;
    bool copy_first = false;
    int b1 = 200;
    bool ctl = false;                   // the same writes aimed at BLTPRI
    uint16_t w1 = 0xC000, w2 = 0x4000, w3 = 0xC000;   // before s1, between B1 and B2, before s3 (see below)
    bool early_blit = false;            // mutation: the blit starts before s0
    bool wait_end = false;              // (ii) contrast: the program waits for the blit to end before B1
    uint16_t size = 0x2020;
};

Script s22(const P22Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    if (o.copy_first) s.CPYD();
    s.W(DMACON, 0x8240);
    if (!o.copy_first) s.CPYD();
    s.W(BLTCON0, 0x0100); s.W(BLTCON1, 0); s.W(BLTDMOD, 0);
    s.W(BLTDPTH, uint16_t(kBltD >> 16)); s.W(BLTDPTL, uint16_t(kBltD));
    if (o.early_blit) s.W(BLTSIZE, o.size);
    s.SSTORE(0xFFFF);
    s.SAMPLE(1);                         // s0
    s.W(DMACON, o.ctl ? 0x8400 : o.w1);
    s.SAMPLE(1);                         // s1
    if (o.early_blit) s.SAMPLE(uint16_t(o.b1));
    else if (o.wait_end) { s.W(BLTSIZE, o.size); s.WVB(); s.WL30(); s.WVB(); s.WL30(); s.SAMPLE(uint16_t(o.b1)); }
    else s.WSAMP(BLTSIZE, o.size, uint16_t(o.b1));        // B1
    s.W(DMACON, o.w2);
    s.SAMPLE(1000);                      // B2
    s.WVB(); s.WL30();
    s.SAMPLE(1);                         // s2
    s.W(DMACON, o.ctl ? 0x0400 : o.w3);
    s.SAMPLE(1);                         // s3
    s.END();
    return s;
}

void proposition_p22() {
    if (!want("P22")) return;
    P22Opts a, c;
    c.ctl = true;
    if (mutating("P22-fix")) { a.fix = false; c.fix = false; }
    if (mutating("P22-i")) a.size = 0x2010;
    if (mutating("P22-ii")) a.wait_end = true;
    if (mutating("P22-iii")) a.copy_first = true;
    if (mutating("P22-iv")) a.b1 = 3000;
    if (mutating("P22")) a.early_blit = true;
    if (mutating("P22-ctl")) c.ctl = false;
    DRun da = drun(s22(a)), dc = drun(s22(c));
    BlitWin wa = blit_window(*da.r), wc = blit_window(*dc.r);
    auto sa = samples(*da.r), sc = samples(*dc.r);
    // indices: s0 = 0, s1 = 1, B1 = 2 .. 2+b1-1, B2 = 2+b1 .. 2+b1+999, s2, s3
    size_t nb1 = size_t(a.b1);
    bool shape = sa.size() == 2 + nb1 + 1000 + 2 && sc.size() == 2 + size_t(c.b1) + 1000 + 2;
    g.expect(da.fix && dc.fix, "P22-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    bool i = wa.n == 4096;
    g.expect(i, "P22-i: the blit completes: 4096 writes in BLT_D; measured " + dec((long long)wa.n));
    int in_b = 0;
    for (size_t j = 2; shape && j < 2 + nb1 + 1000; ++j) if (inside(sa[j], wa)) ++in_b;
    g.expect(in_b >= 1, "P22-ii: at least one sample of B1 or B2 is inside the blit (" + dec(in_b) + ")");
    bool iii = (da.copy & 0x0240) == 0x0240;
    g.expect(iii, "P22-iii: the stored DMACONR shows DMAEN and BLTEN set");
    bool iv = shape && inside(sa[1 + nb1], wa) && inside(sa[2 + nb1], wa);
    g.expect(iv, "P22-iv: the last B1 sample and the first B2 sample are both inside the blit, so the clearing write fell while the bit was 1");
    bool pre = config_ok(g_cfg_used) && da.fix && i && in_b >= 1 && iii && iv && shape;
    bool z = shape;
    if (shape) {
        z = !bbusy(sa[0]) && !bbusy(sa[1]) && !bbusy(sa[sa.size() - 2]) && !bbusy(sa[sa.size() - 1]);
        z = z && bbusy(sa[1 + nb1]);
        for (size_t j = 2 + nb1; j < 2 + nb1 + 1000; ++j) if (inside(sa[j], wa) && !bbusy(sa[j])) z = false;
        z = z && (sa[1].v & 0x0240) == 0x0240;
        for (size_t j = 2 + nb1; j < 2 + nb1 + 1000; ++j) if ((sa[j].v & 0x0240) != 0x0240) z = false;
    }
    g.expect_if(pre, z, "P22: opus:AA-dmacon.b14b: DMACON writes of bit 14 (0xC000 with SET/CLR 1 before a blit, 0x4000 with SET/CLR 0 while the blit runs, 0xC000 after it) leave DMACONR bit 14 as it was: 0 in s0, s1, s2 and s3, 1 in the last B1 sample and in every B2 sample inside the blit, and bits 9 and 6 read 1 in s1 and in every B2 sample");
    // the control: the same writes aimed at BLTPRI
    bool cshape = sc.size() == 2 + size_t(c.b1) + 1000 + 2;
    bool ctl = cshape && (sc[1].v & 0x0400) && (sc[sc.size() - 2].v & 0x0400) && !(sc[sc.size() - 1].v & 0x0400);
    g.expect_if(config_ok(g_cfg_used) && dc.fix && cshape, ctl, "P22-ctl: control, the same writes aimed at BLTPRI at the idle positions: DMACON 0x8400 between s0 and s1 and 0x0400 between s2 and s3 make DMACONR bit 10 read 1 in s1 and s2 and 0 in s3");
}

// P23: BZERO cannot be written.
struct P23Opts {
    bool fix = true;
    bool copy_first = false;
    uint16_t lf_x = 0x0100, lf_y = 0x01FF;
    bool ctl = false;
    bool blit_between = false;          // mutation: blit Y runs between z0 and z1
    uint16_t size = 0x0410;
};

Script s23(const P23Opts& o) {
    Script s;
    if (o.fix) script_start(s);
    if (o.copy_first) s.CPYD();
    s.W(DMACON, 0x8240);
    if (!o.copy_first) s.CPYD();
    auto blit = [&](uint16_t con0) {
        s.W(BLTCON0, con0); s.W(BLTCON1, 0); s.W(BLTDMOD, 0);
        s.W(BLTDPTH, uint16_t(kBltD >> 16)); s.W(BLTDPTL, uint16_t(kBltD));
        s.W(BLTSIZE, o.size);
    };
    auto group = [&](bool mid_blit) {
        s.WVB(); s.WL30();
        s.SSTORE(0xFFFF);
        s.SAMPLE(1);
        if (mid_blit) { blit(o.lf_y); s.WVB(); s.WL30(); }
        s.W(DMACON, o.ctl ? 0x8400 : 0xA000);
        s.SAMPLE(1);
        s.W(DMACON, o.ctl ? 0x0400 : 0x2000);
        s.SAMPLE(1);
    };
    blit(o.lf_x);
    group(o.blit_between);
    blit(o.lf_y);
    group(false);
    s.END();
    return s;
}

void proposition_p23() {
    if (!want("P23")) return;
    P23Opts a, c;
    c.ctl = true;
    if (mutating("P23-fix")) { a.fix = false; c.fix = false; }
    if (mutating("P23-i")) a.size = 0x0408;      // each blit writes 128 words
    if (mutating("P23-ii")) { a.lf_y = 0x0100; }
    if (mutating("P23-iii")) a.copy_first = true;
    if (mutating("P23")) a.blit_between = true;
    if (mutating("P23-ctl")) c.ctl = false;
    DRun da = drun(s23(a)), dc = drun(s23(c));
    auto sa = samples(*da.r), sc = samples(*dc.r);
    // samples: the bracketing store of each group is a write to SAMP, so the writes are
    // [bracket, z0, z1, z2, bracket, y0, y1, y2] and the samples exclude the first write of the run only
    std::vector<uint16_t> v;
    for (const auto& w : da.r->samp) v.push_back(w.data);
    bool shape = v.size() == 8;
    g.expect(da.fix && dc.fix, "P23-fix: the program stores the constant $BEEF first and the harness reads it back at the end of each run, before any other result of the run is used");
    // (i) each blit writes its 256 words before its first sample, every word the value its minterm gives
    bool i = shape;
    size_t nx = 0, ny = 0;
    std::vector<uint16_t> con0;
    for (const auto& c : parse_script(da.sc)) if (c.op == 1 && c.a[0] == BLTCON0) con0.push_back(c.a[1]);
    i = i && con0.size() >= 2 && ((con0[0] & 0xFF) == 0x00 || (con0[0] & 0xFF) == 0xFF) && ((con0[1] & 0xFF) == 0x00 || (con0[1] & 0xFF) == 0xFF);
    const uint16_t ex_x = (!con0.empty() && (con0[0] & 0xFF)) ? 0xFFFF : 0x0000, ex_y = (con0.size() > 1 && (con0[1] & 0xFF)) ? 0xFFFF : 0x0000;
    if (shape) {
        uint64_t t_z = da.r->samp[0].cyc, t_z2 = da.r->samp[3].cyc, t_y = da.r->samp[4].cyc;
        for (const auto& acc : da.r->acc) {
            if (!(acc.wr && acc.byte >= kBltD && acc.byte < kBltDEnd)) continue;
            if (acc.cyc < t_z) { ++nx; if (acc.data != ex_x) i = false; }
            else if (acc.cyc > t_z2 && acc.cyc < t_y) { ++ny; if (acc.data != ex_y) i = false; }
        }
        i = i && nx == 256 && ny == 256;
    }
    g.expect(i, "P23-i: each blit writes its 256 words before its first sample, every word the value its minterm select gives with no source enabled, 0x0000 for LF7-LF0 = 0x00 and 0xFFFF for LF7-LF0 = 0xFF; measured " + dec((long long)nx) + " and " + dec((long long)ny) + " words");
    bool ii = shape && ((v[1] & 0x2000) != (v[5] & 0x2000));
    g.expect(ii, "P23-ii: bit 13 of z0 differs from bit 13 of y0");
    bool iii = (da.copy & 0x0240) == 0x0240;
    g.expect(iii, "P23-iii: the stored DMACONR shows DMAEN and BLTEN set");
    bool eq = shape && ((v[1] & 0x2000) == (v[2] & 0x2000)) && ((v[2] & 0x2000) == (v[3] & 0x2000)) &&
              ((v[5] & 0x2000) == (v[6] & 0x2000)) && ((v[6] & 0x2000) == (v[7] & 0x2000));
    if (shape) checks.note("P23 measured: DMACONR bit 13 in z0 z1 z2 y0 y1 y2: " + dec((v[1] >> 13) & 1) + dec((v[2] >> 13) & 1) + dec((v[3] >> 13) & 1) + " " + dec((v[5] >> 13) & 1) + dec((v[6] >> 13) & 1) + dec((v[7] >> 13) & 1));
    bool pre = config_ok(g_cfg_used) && da.fix && shape && i && ii && iii;
    g.expect_if(pre, eq, "P23: opus:AA-dmacon.b13b: DMACON writes of bit 13 (0xA000 with SET/CLR 1, 0x2000 with SET/CLR 0) leave DMACONR bit 13 as it was: it is equal in z0, z1 and z2 after a blit of zeros and equal in y0, y1 and y2 after a blit of ones");
    std::vector<uint16_t> cv;
    for (const auto& w : dc.r->samp) cv.push_back(w.data);
    bool cshape = cv.size() == 8;
    bool ctl = cshape && (cv[2] & 0x0400) && !(cv[3] & 0x0400) && (cv[6] & 0x0400) && !(cv[7] & 0x0400);
    g.expect_if(config_ok(g_cfg_used) && dc.fix && cshape, ctl, "P23-ctl: control, DMACON 0x8400 and 0x0400 at the same positions make DMACONR bit 10 read 1 in the sample after the first write and 0 after the second, in both groups");
}


// RF-M and RF-L: P, L and A(0x3E) - T0 are the same in every field and every run of the package.
void rf_runs() {
    if (g_mutate && !mutating("RF-RUNS")) return;
    bool ok = !g_frames.empty();
    int64_t P = -1, L = -1, a0 = -1;
    int n = 0;
    for (const auto& f : g_frames) {
        if (f.P <= 0 || f.L <= 0) { ok = false; continue; }
        ++n;
        if (P < 0) { P = f.P; L = f.L; a0 = f.a0; }
        else if (f.P != P || f.L != L || f.a0 != a0) ok = false;
    }
    g.expect(ok && n >= 2, "RF-RUNS: P, L and A(0x3E) - T0 are the same in every run of the package (" + dec(n) + " runs: P = " + dec(P) + ", L = " + dec(L) + ", A(0x3E) - T0 = " + dec(a0) + " system clock cycles)");
}

}  // namespace

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    if (g_mutate && !*g_mutate) g_mutate = nullptr;
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    g_graded = true;   // AA is selected by chipset_config on every run, whichever CPU core
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);

    if (!g_mutate || mutating("CONFIG")) g.expect(config_ok(make_config()), "CONFIG: the harness configuration selects the AA chip set (chipset_config 0x10) and 2 MB of Chip memory (memory_config 0x03)");
    proposition_p1_p3();
    proposition_p2_p4();
    proposition_p5();
    proposition_p6();
    proposition_p7();
    proposition_p8();
    proposition_p9();
    proposition_p10();
    proposition_p11();
    proposition_p12();
    proposition_p13();
    proposition_p14();
    proposition_p15();
    proposition_p16();
    proposition_p17();
    proposition_p18();
    proposition_p19();
    proposition_p20();
    proposition_p21();
    proposition_p22();
    proposition_p23();
    joint_assertions();
    rf_runs();
    checks.note("runs started: " + dec(g_runs) + ", reached DONE: " + dec(g_runs_done));

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
