// test_ch2_interlace_fields.cpp
//
// Integration package ch2-interlace-fields: see SPECIFICATION.md. Program I
// and Program T each drive nanomig.v only through nanomig_harness.h's
// ports (Config, load_program/boot/run_until_write, peek_logical(),
// ram_log()) and are graded only against what those ports showed.
//
// Design note: unlike the shorter ch6 blitter packages, Program I and
// Program T each need to observe many complete video fields (8 PAL fields
// / 13 NTSC-or-PAL fields), so a single run of either program already
// costs many million clk_sys cycles. Re-running the whole program once
// per assertion (as ch6-blitter-geometry does) would multiply that cost
// by the number of assertions. Instead, the *normal* run loads Program I
// once and Program T once, and every assertion and precondition below is
// graded from that one shared measurement -- ram_log() and peek_logical()
// are read-only, so sharing them cannot let one assertion affect another's
// verdict. A *mutation* run (NMH_MUTATE set) still loads only the one
// program its label calls for (I or T), in isolation, and grades only that
// one label; it never mixes a mutant's data into another assertion.
//
// Measured column: the verdict this file's own "stimulus red"/"contrast
// stays green" checks.expect() produced when actually run with
// NMH_MUTATE=<label> against both cores' builds. Where it disagrees with
// the record's own prediction, that disagreement is kept, not tuned away.
//
// MUTATION P1 stimulus red NMH_MUTATE=P1: LACE=0 (I-a) -- measured: red on fx68k, red on tg68k
// MUTATION PI-3 stimulus red NMH_MUTATE=PI-3: LACE=0 (I-a) -- measured: red on fx68k, red on tg68k
// MUTATION P4a stimulus red NMH_MUTATE=P4a: both lists omit the BPL1PT MOVEs (I-c) -- measured: red on fx68k, red on tg68k
// MUTATION P4b stimulus red NMH_MUTATE=P4b: LISTB writes PIC, not PIC+40 (I-b) -- measured: red on fx68k, red on tg68k; contrast P4e measured green on fx68k, green on tg68k
// MUTATION P4c stimulus red NMH_MUTATE=P4c: LISTB writes PIC, not PIC+40 (I-b) -- measured: red on fx68k, red on tg68k; contrast P4e measured green on fx68k, green on tg68k
// MUTATION P4d stimulus red NMH_MUTATE=P4d: LISTB writes PIC, not PIC+40 (I-b) -- measured: red on fx68k, red on tg68k; contrast P4e measured green on fx68k, green on tg68k
// MUTATION P4e stimulus red NMH_MUTATE=P4e: the handler never repoints COP1LC (I-d) -- measured: red on fx68k, red on tg68k; contrast PI-5 measured green on fx68k, green on tg68k
// MUTATION P5 stimulus red NMH_MUTATE=P5: both lists omit the BPL1PT MOVEs (I-c) -- measured: red on fx68k, red on tg68k
// MUTATION P6 stimulus red NMH_MUTATE=P6: both lists omit the SPR0PT MOVEs (I-e) -- measured: red on fx68k, red on tg68k
// MUTATION PI-1 stimulus red NMH_MUTATE=PI-1: M1 waits between samples (I-j) -- measured: red on fx68k, red on tg68k
// MUTATION PI-2 stimulus red NMH_MUTATE=PI-2: DMACON leaves COPEN clear (I-f) -- measured: red on fx68k, red on tg68k
// MUTATION PI-4 stimulus red NMH_MUTATE=PI-4: DMACON leaves BPLEN clear (I-h) -- measured: red on fx68k, red on tg68k
// MUTATION PI-5 stimulus red NMH_MUTATE=PI-5: INTENA leaves COPER clear (I-g) -- measured: red on fx68k, red on tg68k
// MUTATION PI-6 stimulus red NMH_MUTATE=PI-6: PIC filled with $0000 (I-k) -- measured: red on fx68k, red on tg68k
// MUTATION PI-7 stimulus red NMH_MUTATE=PI-7: BPL1PT MOVEs placed after the WAIT (I-l) -- measured: GREEN on fx68k, GREEN on tg68k (mismatch: WAIT $2D01 is satisfied at once at this restart, ~36 colour clocks in, not after ~45 lines; see the PI-7 dispatch code and SPECIFICATION.md's Build section)
// MUTATION PI-8 stimulus red NMH_MUTATE=PI-8: DMACON leaves SPREN clear (I-i) -- measured: red on fx68k, red on tg68k
// MUTATION P2 stimulus red NMH_MUTATE=P2: PAL selected instead of NTSC (T-b) -- measured: red on fx68k, red on tg68k
// MUTATION P3 stimulus red NMH_MUTATE=P3: PAL selected instead of NTSC (T-b) -- measured: red on fx68k, red on tg68k
// MUTATION PT-1 stimulus red NMH_MUTATE=PT-1: M1 waits between samples (T-f) -- measured: red on fx68k, red on tg68k
// MUTATION PT-2 stimulus red NMH_MUTATE=PT-2: DIWSTOP is the normal NTSC value (T-e) -- measured: red on fx68k, red on tg68k
// MUTATION PT-3 stimulus red NMH_MUTATE=PT-3: line 100's mark uses H=$C0 (T-d) -- measured: red on fx68k, red on tg68k
// MUTATION PT-4 stimulus red NMH_MUTATE=PT-4: the first WAIT is (0,$00) (T-c) -- measured: red on fx68k, red on tg68k
// MUTATION PT-5 stimulus red NMH_MUTATE=PT-5: line 10's WAIT+MOVE is deleted (T-g) -- measured: red on fx68k, red on tg68k
// MUTATION PT-6 stimulus red NMH_MUTATE=PT-6: LACE=0 (T-a) -- measured: red on fx68k, red on tg68k
// MUTATION PT-7 stimulus red NMH_MUTATE=PT-7: DMACON leaves COPEN clear (T-h) -- measured: red on fx68k, red on tg68k

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <optional>
#include <memory>
#include <iostream>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

constexpr uint32_t kDoneAddr   = 0x0C0FFE;
constexpr uint16_t kDoneValue  = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudgetI = 16'000'000;
constexpr uint64_t kRunBudgetT = 20'000'000;

// One colour clock (p194, p24) is 8 clk_sys cycles: 4 clk_sys cycles per
// the harness's own 7 MHz RAM-port sample point (nanomig_harness.h), and
// the colour clock is half of that again. A fixed clock-generator ratio,
// not a value fitted to a measurement (SPECIFICATION.md 2.3).
constexpr uint64_t kClkSysPerColourClock = 8;

// ---- Program I memory layout, mirrors prog_i.s -----------------------
constexpr uint32_t kI_PIC        = 0x082000;
constexpr uint32_t kI_PIC_END    = kI_PIC + 20480;
constexpr uint32_t kI_SPR        = 0x087000;
constexpr uint32_t kI_LISTA      = 0x080800;
constexpr uint32_t kI_LISTB      = 0x080900;
constexpr uint32_t kI_HCNT       = 0x087222;
constexpr int       kI_K         = 8;
constexpr uint32_t kI_M1_STARTW  = 0x087224;
constexpr uint32_t kI_M1_DONEW   = 0x087226;
constexpr uint32_t kI_M1_RECBASE = 0x087230;
constexpr uint32_t kI_FIXMARK    = 0x087400;

// ---- Program T memory layout, mirrors prog_t.s -----------------------
constexpr uint32_t kT_PLANE      = 0x082000;
constexpr uint32_t kT_PLANE_END  = kT_PLANE + 11000;
constexpr uint32_t kT_LISTT      = 0x080800;
constexpr int       kT_K         = 13;
constexpr uint32_t kT_M1_STARTW  = 0x086000;
constexpr uint32_t kT_M1_DONEW   = 0x086002;
constexpr uint32_t kT_M1_RECBASE = 0x086010;
constexpr uint32_t kT_FIXMARK    = 0x086100;

// The assertion call form is fixed by BRIEF.md.
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

// ---- one harness run ---------------------------------------------------

struct Run {
    std::unique_ptr<nmh::NanomigHarness> h;
    size_t loaded = 0;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log; // coalesced
};

Run run_program(const std::string& path, const nmh::Config& cfg, uint64_t run_budget) {
    Run r;
    r.h = std::make_unique<nmh::NanomigHarness>(cfg);
    r.loaded = r.h->load_program(path);
    if (r.loaded) {
        r.released = r.h->boot(kBootBudget).cpu_released;
        r.done = r.h->run_until_write(kDoneAddr, kDoneValue, kBootBudget + run_budget);
    }
    r.log = nmh::coalesce(r.h->ram_log());
    return r;
}

bool ok(const Run& r) { return r.loaded > 0 && r.released && r.done.has_value(); }

std::optional<uint16_t> W(const Run& r, uint32_t byte_addr) { return r.h->peek_logical(byte_addr); }

std::vector<uint64_t> write_cycles(const Run& r, uint32_t byte_addr) {
    uint32_t wa = byte_addr / 2;
    std::vector<uint64_t> out;
    for (const auto& a : r.log) if (a.is_write && a.word_addr == wa) out.push_back(a.cycle);
    return out;
}
std::vector<uint64_t> write_cycles_of(const Run& r, uint32_t byte_addr, uint16_t data) {
    uint32_t wa = byte_addr / 2;
    std::vector<uint64_t> out;
    for (const auto& a : r.log) if (a.is_write && a.word_addr == wa && a.data == data) out.push_back(a.cycle);
    return out;
}
size_t read_count_range(const Run& r, uint32_t lo_addr, uint32_t hi_addr_excl) {
    uint32_t lo = lo_addr / 2, hi = hi_addr_excl / 2;
    size_t n = 0;
    for (const auto& a : r.log) if (!a.is_write && a.word_addr >= lo && a.word_addr < hi) ++n;
    return n;
}

// ---- Measurement M1 (SPECIFICATION.md 2.5) -----------------------------

struct M1Record {
    bool present = false;
    uint16_t firstV = 0xFFFF, lastV = 0xFFFF, gap = 1, lofOR = 0, lofAND = 0;
    uint64_t closeCycle = 0; // port time of the write that closed this record (its last word)
};

std::vector<M1Record> read_m1(const Run& r, uint32_t recbase, int k) {
    std::vector<M1Record> out(k);
    for (int i = 0; i < k; ++i) {
        uint32_t base = recbase + i * 10;
        auto fv = W(r, base + 0), lv = W(r, base + 2), gp = W(r, base + 4),
             lo = W(r, base + 6), la = W(r, base + 8);
        if (fv && lv && gp && lo && la) {
            M1Record m;
            m.present = true;
            m.firstV = *fv; m.lastV = *lv; m.gap = *gp; m.lofOR = *lo; m.lofAND = *la;
            auto wc = write_cycles(r, base + 8);
            if (!wc.empty()) m.closeCycle = wc.front();
            out[i] = m;
        }
    }
    return out;
}

uint64_t start_cycle_of(const Run& r, uint32_t startw_addr) {
    auto wc = write_cycles_of(r, startw_addr, 0xFACE);
    return wc.empty() ? 0 : wc.front();
}
bool m1_completed(const Run& r, uint32_t donew_addr) {
    return !write_cycles_of(r, donew_addr, 0xD1CE).empty();
}
bool m1_all_valid(const std::vector<M1Record>& recs) {
    for (const auto& m : recs) if (!m.present || m.firstV != 0 || m.gap != 0 || m.lastV < 255) return false;
    return true;
}

// ---- Measurement M2: the Copper restart as field anchor ---------------

struct Restart { uint64_t cycle; char list; }; // list is 'A'/'B' (Program I) or ' ' (Program T)

std::vector<Restart> find_restarts(const Run& r, uint32_t listA, uint32_t listB /*0 if single-list*/) {
    std::vector<Restart> out;
    uint32_t wa = listA / 2, wb = listB / 2;
    for (const auto& a : r.log) {
        if (a.is_write) continue;
        if (a.word_addr == wa) out.push_back({a.cycle, listB ? 'A' : ' '});
        else if (listB && a.word_addr == wb) out.push_back({a.cycle, 'B'});
    }
    std::sort(out.begin(), out.end(), [](const Restart& x, const Restart& y) { return x.cycle < y.cycle; });
    return out;
}

// The port-side field index j such that restarts[j].cycle <= t (< the next
// restart's cycle, if any) -- the restart interval containing time t.
int field_index_for_time(const std::vector<Restart>& rs, uint64_t t) {
    int j = -1;
    for (size_t i = 0; i < rs.size(); ++i) { if (rs[i].cycle <= t) j = (int)i; else break; }
    return j;
}

uint64_t interval_end(const std::vector<Restart>& rs, int idx) {
    return (idx + 1 < (int)rs.size()) ? rs[idx + 1].cycle : UINT64_MAX;
}

// section 2.6's own matching rule: M1 record k <-> the restart interval
// containing the midpoint of the writes that open and close it.
std::vector<int> match_m1_to_restarts(const std::vector<M1Record>& m1, uint64_t start_cycle,
                                       const std::vector<Restart>& rs) {
    std::vector<int> out(m1.size(), -1);
    for (size_t k = 0; k < m1.size(); ++k) {
        if (!m1[k].present) continue;
        uint64_t opening = (k == 0) ? start_cycle : m1[k - 1].closeCycle;
        uint64_t closing = m1[k].closeCycle;
        uint64_t mid = opening + (closing - opening) / 2;
        out[k] = field_index_for_time(rs, mid);
    }
    return out;
}

// ---- bitplane fetch grouping (20-word lines, SPECIFICATION.md 2.7) ----

struct BplGroup { uint64_t cycle; uint32_t addr; int len; };

std::vector<BplGroup> group_reads_in(const Run& r, uint32_t lo_addr, uint32_t hi_addr_excl,
                                      uint64_t begin, uint64_t end) {
    uint32_t lo = lo_addr / 2, hi = hi_addr_excl / 2;
    std::vector<BplGroup> out;
    uint32_t expect = 0; bool open = false;
    for (const auto& a : r.log) {
        if (a.cycle < begin || a.cycle >= end) { open = false; continue; }
        if (a.is_write) continue;
        if (a.word_addr < lo || a.word_addr >= hi) continue;
        if (open && a.word_addr == expect) { out.back().len++; expect++; }
        else { out.push_back({a.cycle, a.word_addr, 1}); expect = a.word_addr + 1; open = true; }
    }
    return out;
}

// Program T sets BPL1MOD/BPL2MOD = 0 (section 4.3): the bitplane pointer
// advances with no modulo skip at all, so a field's fetches form one
// single, address-contiguous run with no gap between lines (unlike
// Program I's interlaced BPL1MOD=40, which leaves a genuine 40-byte
// address gap between each 20-word line -- group_reads_in() above finds
// those gaps). Here the 20-word lines are instead cut by count, in fetch
// order; each cut is still checked for 20 consecutive ascending
// addresses (section 2.7's own wording), just not relied on to find the
// cut points.
std::vector<BplGroup> chunk_reads_in(const Run& r, uint32_t lo_addr, uint32_t hi_addr_excl,
                                      uint64_t begin, uint64_t end, int chunk_size) {
    uint32_t lo = lo_addr / 2, hi = hi_addr_excl / 2;
    std::vector<std::pair<uint64_t, uint32_t>> reads;
    for (const auto& a : r.log) {
        if (a.is_write) continue;
        if (a.cycle < begin || a.cycle >= end) continue;
        if (a.word_addr < lo || a.word_addr >= hi) continue;
        reads.push_back({a.cycle, a.word_addr});
    }
    std::vector<BplGroup> out;
    for (size_t i = 0; i + (size_t)chunk_size <= reads.size(); i += (size_t)chunk_size) {
        bool consecutive = true;
        for (int j = 1; j < chunk_size; ++j)
            if (reads[i + (size_t)j].second != reads[i].second + (uint32_t)j) consecutive = false;
        out.push_back({reads[i].first, reads[i].second, consecutive ? chunk_size : -1});
    }
    return out;
}

//=========================================================================
// Program I: P1, P4, P5, P6 and their preconditions PI-1..PI-8
//=========================================================================

nmh::Config i_config() {
    nmh::Config c;
    c.chipset_config = 0x08; // ECS, PAL (chipset_config[1]=0)
    return c;
}

struct IField {
    int m1 = -1;          // index into m1 records
    int restart = -1;     // index into restarts
    char list = '?';
    uint64_t begin = 0, end = 0;
    std::vector<BplGroup> bpl;
};

struct IAnalysis {
    Run r;
    std::vector<M1Record> m1;
    uint64_t startCycle = 0;
    bool completed = false;
    std::vector<Restart> restarts;
    std::vector<IField> fields; // kI_K entries, in M1/field order 0..K-1
    bool matched = false;
};

IAnalysis analyze_i(const std::string& dir, const std::string& prog) {
    IAnalysis a;
    a.r = run_program(dir + "/" + prog + ".bin", i_config(), kRunBudgetI);
    a.m1 = read_m1(a.r, kI_M1_RECBASE, kI_K);
    a.startCycle = start_cycle_of(a.r, kI_M1_STARTW);
    a.completed = m1_completed(a.r, kI_M1_DONEW);
    a.restarts = find_restarts(a.r, kI_LISTA, kI_LISTB);
    auto idx = match_m1_to_restarts(a.m1, a.startCycle, a.restarts);
    a.fields.resize(kI_K);
    a.matched = true;
    for (int k = 0; k < kI_K; ++k) {
        IField f;
        f.m1 = k;
        f.restart = idx[k];
        if (f.restart < 0 || f.restart >= (int)a.restarts.size()) { a.matched = false; continue; }
        f.list = a.restarts[f.restart].list;
        f.begin = a.restarts[f.restart].cycle;
        f.end = interval_end(a.restarts, f.restart);
        f.bpl = group_reads_in(a.r, kI_PIC, kI_PIC_END, f.begin, f.end);
        a.fields[k] = f;
    }
    return a;
}

bool pi1_valid(const IAnalysis& a) { return ok(a.r) && a.completed && m1_all_valid(a.m1); }

bool pi2_anchor(const IAnalysis& a) {
    if (!a.m1.back().present) return false;
    uint64_t lastClose = a.m1.back().closeCycle;
    int n = 0;
    for (const auto& rs : a.restarts) if (rs.cycle > a.startCycle && rs.cycle <= lastClose) ++n;
    if (n != kI_K) return false;
    for (const auto& f : a.fields) if (f.list != 'A' && f.list != 'B') return false;
    return true;
}

bool pi4_groups(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        if (f.bpl.size() != 256) return false;
        for (const auto& g : f.bpl) if (g.len != 20) return false;
    }
    return true;
}

bool p1_alternation(const IAnalysis& a, std::vector<int>* Ns = nullptr) {
    if (!a.m1.back().present) return false;
    std::vector<int> N(kI_K);
    for (int k = 0; k < kI_K; ++k) {
        if (!a.m1[k].present) return false;
        N[k] = a.m1[k].lastV + 1;
        if (N[k] != 312 && N[k] != 313) return false;
    }
    for (int k = 0; k + 1 < kI_K; ++k) if (N[k] == N[k + 1] || N[k] + N[k + 1] != 625) return false;
    if (Ns) *Ns = N;
    return true;
}

bool pi5_hcnt(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        int n = 0;
        for (const auto& e : write_cycles(a.r, kI_HCNT))
            if (e >= f.begin && e < f.end) ++n;
        if (n != 1) return false;
    }
    return true;
}

bool pi6_tags(const IAnalysis& a) {
    for (const auto& acc : a.r.log) {
        if (acc.is_write) continue;
        if (acc.word_addr * 2 < kI_PIC || acc.word_addr * 2 >= kI_PIC_END) continue;
        uint32_t line = (acc.word_addr * 2 - kI_PIC) / 40;
        if (acc.data != (uint16_t)(0x8000 | line)) return false;
    }
    return true;
}

// lateLayout selects I-l's own list layout (prog_i_l.s): the BPL1PT MOVEs
// sit after SPR0PT/SPR1-7PT and the WAIT $2D01,$FF00 (offset 36 bytes),
// not at the list's first word (offset 0) as in the normal (I-0) layout.
bool pi7_bpl(const IAnalysis& a, bool lateLayout = false) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        uint32_t listbase = (f.list == 'A') ? kI_LISTA : kI_LISTB;
        uint32_t wa = (listbase + (lateLayout ? 36u : 0u)) / 2;
        uint64_t ptrCycle = UINT64_MAX;
        for (const auto& acc : a.r.log)
            if (!acc.is_write && acc.word_addr == wa && acc.cycle >= f.begin && acc.cycle < f.end) { ptrCycle = acc.cycle; break; }
        if (ptrCycle == UINT64_MAX) return false;
        if (f.bpl.empty() || ptrCycle >= f.bpl.front().cycle) return false;
    }
    return true;
}

// lateLayout: under I-l, SPR0PTH is the list's first word (offset 0),
// not offset 4 as in the normal layout (which has the BPL1PT MOVEs first).
bool pi7_spr(const IAnalysis& a, bool lateLayout = false) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        uint32_t listbase = (f.list == 'A') ? kI_LISTA : kI_LISTB;
        uint32_t wa = (listbase + (lateLayout ? 0u : 4u)) / 2;
        uint64_t ptrCycle = UINT64_MAX;
        for (const auto& acc : a.r.log)
            if (!acc.is_write && acc.word_addr == wa && acc.cycle >= f.begin && acc.cycle < f.end) { ptrCycle = acc.cycle; break; }
        if (ptrCycle == UINT64_MAX) return false;
        uint64_t firstSpr = UINT64_MAX;
        for (const auto& acc : a.r.log)
            if (!acc.is_write && acc.word_addr * 2 >= kI_SPR && acc.word_addr * 2 < kI_SPR + 16 &&
                acc.cycle >= f.begin && acc.cycle < f.end) { firstSpr = acc.cycle; break; }
        if (firstSpr == UINT64_MAX || ptrCycle >= firstSpr) return false;
    }
    return true;
}

bool pi8_spr_fetched(const IAnalysis& a) { return read_count_range(a.r, kI_SPR, kI_SPR + 16) > 0; }

bool p4a(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        if (f.bpl.empty()) return false;
        uint32_t want = (f.list == 'A') ? kI_PIC : kI_PIC + 40;
        if (f.bpl.front().addr * 2 != want) return false;
    }
    return true;
}

bool p4b(const IAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k + 1 < kI_K; k += 2) {
        const auto& fa = a.fields[(size_t)k];
        const auto& fb = a.fields[(size_t)k + 1];
        if (fa.list != 'A' || fb.list != 'B') return false;
        if (fa.bpl.empty() || fb.bpl.empty()) return false;
        if (fb.bpl.front().addr * 2 - fa.bpl.front().addr * 2 != 40) return false;
    }
    return true;
}

bool p4c(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        if (f.bpl.size() != 256) return false;
        uint32_t base = (f.list == 'A') ? kI_PIC : (kI_PIC + 40);
        for (int i = 0; i < 256; ++i)
            if (f.bpl[(size_t)i].addr * 2 != base + 80u * (uint32_t)i) return false;
    }
    return true;
}

bool p4d(const IAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k + 1 < kI_K; k += 2) {
        const auto& fa = a.fields[k];
        const auto& fb = a.fields[k + 1];
        if (fa.list != 'A' || fb.list != 'B') return false;
        if (fa.bpl.size() != 256 || fb.bpl.size() != 256) return false;
        for (int i = 0; i < 256; ++i) {
            uint32_t addrA = fa.bpl[(size_t)i].addr * 2, addrB = fb.bpl[(size_t)i].addr * 2;
            if (addrB - addrA != 40) return false;
        }
    }
    return true;
}

bool p4e(const IAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k < kI_K; ++k) if (a.fields[(size_t)k].list != (k % 2 == 0 ? 'A' : 'B')) return false;
    return true;
}

bool p5(const IAnalysis& a) { return p4a(a); } // same observation (first fetch address per field)

bool p6(const IAnalysis& a) {
    if (!a.matched) return false;
    const uint32_t words[4] = {kI_SPR + 4, kI_SPR + 6, kI_SPR + 8, kI_SPR + 10};
    for (const auto& f : a.fields) {
        for (uint32_t wbyte : words) {
            uint32_t wa = wbyte / 2;
            bool seen = false;
            for (const auto& acc : a.r.log)
                if (!acc.is_write && acc.word_addr == wa && acc.cycle >= f.begin && acc.cycle < f.end) { seen = true; break; }
            if (!seen) return false;
        }
    }
    return true;
}

void run_program_i_assertions(const std::string& dir) {
    IAnalysis a = analyze_i(dir, "prog_i");

    checks.expect(ok(a.r) && W(a.r, kI_FIXMARK) && *W(a.r, kI_FIXMARK) == 0xBEEF &&
                      !write_cycles_of(a.r, kI_FIXMARK, 0xBEEF).empty() &&
                      write_cycles_of(a.r, kI_FIXMARK, 0xBEEF).front() < a.startCycle,
                  "FIX-I: Program I stores and delivers a known constant (BEEF) before M1 records anything");

    checks.expect(pi1_valid(a),
        "PI-1: Program I's fixture -- the completion word was written and all 8 M1 records are valid "
        "(first V is 0, gap flag clear, last V >= 255)");
    checks.expect(pi2_anchor(a),
        "PI-2: between the first and last recorded wrap there are exactly 8 Copper restarts, each the "
        "first word of LISTA or LISTB");
    checks.expect(pi4_groups(a),
        "PI-4: every recorded field contains exactly 256 groups of 20 consecutive ascending bitplane fetches");
    checks.expect(pi5_hcnt(a),
        "PI-5: HCNT rises by exactly 1 in every recorded field");
    checks.expect(pi6_tags(a),
        "PI-6: every bitplane fetch from inside PIC returns the tag ($8000 OR its memory line) of its address");
    checks.expect(pi7_bpl(a),
        "PI-7(bpl): in every field, the Copper fetch of the BPL1PT MOVEs precedes the field's first bitplane fetch");
    checks.expect(pi7_spr(a),
        "PI-7(spr): in every field, the Copper fetch of the SPR0PT MOVEs precedes the field's first sprite-0 fetch");
    checks.expect(pi8_spr_fetched(a),
        "PI-8: the sprite-0 structure at SPR is fetched at least once during the run");
    std::vector<int> N;
    bool p1ok = p1_alternation(a, &N);
    checks.expect(p1ok,
        "PI-3: interlace is active -- the check of P1 holds");
    checks.expect(p1ok,
        "P1 p24,p34 ch2-vertical-beam-position.c12 COP2-vertical-beam-position.c12 "
        "COP2-vertical-beam-position.c15 COP2-vertical-beam-position.c16 "
        "COP2-using-the-copper-in-interlaced-mode.c5 COP2-using-the-copper-in-interlaced-mode.c6 "
        "COP2-using-the-copper-in-interlaced-mode.c11 ch2-using-the-copper-in-interlaced-mode.c6 "
        "ch2-using-the-copper-in-interlaced-mode.c10: with LACE set, all 8 recorded PAL fields have "
        "312 or 313 lines, and every consecutive pair sums to 625 (alternates)");
    if (p1ok) {
        std::string s;
        for (size_t i = 0; i < N.size(); ++i) s += (i ? "," : "") + std::to_string(N[i]);
        checks.note("P1 field lengths N_0..N_7 = " + s);
    }

    checks.expect(p4a(a),
        "P4a p34 ch2-using-the-copper-in-interlaced-mode.c16 COP2-using-the-copper-in-interlaced-mode.c15: the "
        "first bitplane fetch of every A-field is at PIC and of every B-field at PIC+40");
    checks.expect(p4b(a),
        "P4b p34 ch2-using-the-copper-in-interlaced-mode.c17 ch2-using-the-copper-in-interlaced-mode.c18 "
        "COP2-using-the-copper-in-interlaced-mode.c16 COP2-using-the-copper-in-interlaced-mode.c17: "
        "every B-field's starting address is 40 bytes past the A-field directly before it (one picture line)");
    checks.expect(p4c(a),
        "P4c p34 ch2-using-the-copper-in-interlaced-mode.c8 ch2-using-the-copper-in-interlaced-mode.c9 "
        "ch2-using-the-copper-in-interlaced-mode.c10 ch2-using-the-copper-in-interlaced-mode.c13 "
        "ch2-using-the-copper-in-interlaced-mode.c14 COP2-using-the-copper-in-interlaced-mode.c8 "
        "COP2-using-the-copper-in-interlaced-mode.c9 COP2-using-the-copper-in-interlaced-mode.c12 "
        "COP2-using-the-copper-in-interlaced-mode.c13: in every A-field, fetch group i reads memory line 2i, "
        "and in every B-field, group i reads 2i+1");
    checks.expect(p4d(a),
        "P4d p34 ch2-using-the-copper-in-interlaced-mode.c15 ch2-using-the-copper-in-interlaced-mode.c19 "
        "COP2-using-the-copper-in-interlaced-mode.c18 COP2-using-the-copper-in-interlaced-mode.c19: every "
        "A-field/B-field pair together fetches memory lines 0..511 each exactly once, rising by 40 bytes at "
        "every step in screen order");
    checks.expect(p4e(a),
        "P4e p34,p35 ch2-using-the-copper-in-interlaced-mode.c11 ch2-using-the-copper-in-interlaced-mode.c12 "
        "ch2-using-the-copper-in-interlaced-mode.c20.s2: over the 8 recorded fields the restart list "
        "alternates LISTA, LISTB, LISTA, ... (order only -- the 16.6ms figure label is not graded)");

    checks.expect(p5(a),
        "P5 p27 ch2-putting-together-a-copper-instruction-list.c3 ch2-using-the-copper-in-interlaced-mode.c16 "
        "COP2-using-the-copper-in-interlaced-mode.c15: rewriting the bitplane pointer during vertical "
        "blanking starts every field's retrieval at the rewritten address");
    checks.expect(p6(a),
        "P6 p27 ch2-putting-together-a-copper-instruction-list.c4: rewriting the sprite-0 pointer during "
        "vertical blanking fetches the sprite's data in every recorded field");
}

// ---- Program I mutation runs (one program per invocation) -------------

void run_i_mutation(const std::string& dir, const std::string& label) {
    static const std::map<std::string, std::string> kProg = {
        {"P1", "prog_i_a"}, {"PI-3", "prog_i_a"},
        {"P4a", "prog_i_c"},
        {"P4b", "prog_i_b"}, {"P4c", "prog_i_b"}, {"P4d", "prog_i_b"},
        {"P4e", "prog_i_d"},
        {"P5", "prog_i_c"}, {"PI-7", "prog_i_l"},
        {"P6", "prog_i_e"},
        {"PI-1", "prog_i_j"}, {"PI-2", "prog_i_f"}, {"PI-4", "prog_i_h"},
        {"PI-5", "prog_i_g"}, {"PI-6", "prog_i_k"}, {"PI-8", "prog_i_i"},
    };
    auto it = kProg.find(label);
    if (it == kProg.end()) { checks.record("NMH_MUTATE=" + label + " names no Program-I mutation"); return; }
    IAnalysis a = analyze_i(dir, it->second);

    if (label == "P1" || label == "PI-3") {
        std::vector<int> N;
        bool eq = a.m1.back().present;
        if (eq) { for (int k = 0; k < kI_K; ++k) { if (!a.m1[(size_t)k].present) { eq = false; break; } N.push_back(a.m1[(size_t)k].lastV + 1); } }
        bool allEqual = eq; for (size_t k = 1; k < N.size(); ++k) if (N[k] != N[0]) allEqual = false;
        checks.expect(!p1_alternation(a), label + " MUTATION stimulus red: LACE=0 -- P1's own alternation check fails");
        if (allEqual) checks.record(label + " MUTATION contrast: all 8 non-interlaced field lengths equal N=" + std::to_string(N.empty()?0:N[0]) + " (recorded, not graded)");
    } else if (label == "P4a") {
        checks.expect(!p4a(a), label + " MUTATION stimulus red: both lists omit the BPL1PT MOVEs -- only the first field starts at the rewritten address");
    } else if (label == "P4e") {
        checks.expect(!p4e(a), label + " MUTATION stimulus red: the handler never repoints COP1LC -- every field runs LISTA, the restart list never alternates");
        checks.expect(pi5_hcnt(a), label + " MUTATION contrast stays green: PI-5 (the handler still runs and still counts)");
    } else if (label == "P4b" || label == "P4c" || label == "P4d") {
        checks.expect(!(p4b(a) && p4c(a) && p4d(a)), label + " MUTATION stimulus red: LISTB writes PIC (not PIC+40) -- P4b/P4c/P4d go red");
        checks.expect(p4e(a), label + " MUTATION contrast stays green: P4e (the restart list still alternates LISTA, LISTB)");
        // Section 5's own contrast note also claims (a) stays green under
        // I-b ("every field still starts where its own list pointed").
        // Measured: p4a (as its proposition is literally worded -- "of
        // every B-field at PIC + 40") goes red on both cores too, since
        // under I-b every B-field starts at PIC, not PIC + 40. Not
        // asserted either way; see the halt reported for P4a/I-b.
    } else if (label == "P5") {
        checks.expect(!p5(a), label + " MUTATION stimulus red: both lists omit the BPL1PT MOVEs -- P5 goes red");
    } else if (label == "P6") {
        checks.expect(!p6(a), label + " MUTATION stimulus red: both lists omit the SPR0PT MOVEs -- P6 goes red");
    } else if (label == "PI-1") {
        checks.expect(!pi1_valid(a), label + " MUTATION stimulus red: M1 waits between samples -- records are no longer all valid");
    } else if (label == "PI-2") {
        checks.expect(!pi2_anchor(a), label + " MUTATION stimulus red: DMACON leaves COPEN clear -- no Copper restarts occur");
    } else if (label == "PI-4") {
        checks.expect(!pi4_groups(a), label + " MUTATION stimulus red: DMACON leaves BPLEN clear -- no bitplane fetch groups occur");
    } else if (label == "PI-5") {
        checks.expect(!pi5_hcnt(a), label + " MUTATION stimulus red: INTENA leaves COPER clear -- the handler never runs, HCNT never rises");
    } else if (label == "PI-6") {
        checks.expect(!pi6_tags(a), label + " MUTATION stimulus red: PIC filled with $0000 -- fetched values no longer match the tag formula");
    } else if (label == "PI-7") {
        // Measured (both cores): this stays GREEN, not red -- the moved
        // BPL1PT MOVE's own fetch lands only ~288 clk_sys cycles (~36
        // colour clocks, well under one line) after the restart, and
        // ~80,000 cycles (~44 lines) before the field's first bitplane
        // fetch: WAIT $2D01 (V>=45, horizontal ignored) is satisfied
        // essentially at once, not after ~45 lines. Reported as measured,
        // not tuned to pass: see SPECIFICATION.md's Build section.
        checks.expect(!(pi7_bpl(a, true) && pi7_spr(a, true)), label + " MUTATION stimulus red: the BPL1PT MOVEs are placed after the WAIT -- the pointer fetch no longer precedes the field's first bitplane fetch");
    } else if (label == "PI-8") {
        checks.expect(!pi8_spr_fetched(a), label + " MUTATION stimulus red: DMACON leaves SPREN clear -- the sprite structure is never fetched");
    }
}

//=========================================================================
// Program T: P2, P3 and their preconditions PT-1..PT-7
//=========================================================================

nmh::Config t_config(bool ntsc) {
    nmh::Config c;
    c.chipset_config = ntsc ? 0x0A : 0x08; // ECS + (bit1: NTSC=1/PAL=0)
    return c;
}

// address of pair p's MOVE-opcode word (p is the assembled sequence
// index, which equals v unless T_SKIP10 removed v==10 from the stream).
// Pair 0 is followed by the one-time BPL1PTH/BPL1PTL MOVE pair (2 words
// each, 8 bytes total, listt_body.i), which shifts every later pair by a
// fixed +8 bytes.
uint32_t listt_move_addr(int p) {
    uint32_t off = (p == 0) ? 0u : (8u * (uint32_t)p + 8u);
    return kT_LISTT + off + 4;
}

std::vector<int> listt_v_of_p(bool skip10) {
    std::vector<int> out;
    for (int v = 0; v < 255; ++v) { if (skip10 && v == 10) continue; out.push_back(v); }
    return out;
}

struct TField {
    int m1 = -1, restart = -1;
    uint64_t begin = 0, end = 0;
    std::vector<BplGroup> bpl;
    int N = -1; // from M1
};

struct TAnalysis {
    Run r;
    std::vector<M1Record> m1;
    uint64_t startCycle = 0;
    bool completed = false;
    std::vector<Restart> restarts;
    std::vector<TField> fields;
    bool matched = false;
    std::map<uint32_t, std::vector<uint64_t>> readsByAddr; // word_addr -> cycles, ascending
};

TAnalysis analyze_t(const std::string& dir, const std::string& prog, bool ntsc) {
    TAnalysis a;
    a.r = run_program(dir + "/" + prog + ".bin", t_config(ntsc), kRunBudgetT);
    a.m1 = read_m1(a.r, kT_M1_RECBASE, kT_K);
    a.startCycle = start_cycle_of(a.r, kT_M1_STARTW);
    a.completed = m1_completed(a.r, kT_M1_DONEW);
    a.restarts = find_restarts(a.r, kT_LISTT, 0);
    for (const auto& acc : a.r.log) if (!acc.is_write) a.readsByAddr[acc.word_addr].push_back(acc.cycle);
    for (auto& kv : a.readsByAddr) std::sort(kv.second.begin(), kv.second.end());
    auto idx = match_m1_to_restarts(a.m1, a.startCycle, a.restarts);
    a.fields.resize(kT_K);
    a.matched = true;
    for (int k = 0; k < kT_K; ++k) {
        TField f;
        f.m1 = k;
        f.restart = idx[k];
        if (f.restart < 0 || f.restart >= (int)a.restarts.size()) { a.matched = false; continue; }
        f.begin = a.restarts[(size_t)f.restart].cycle;
        f.end = interval_end(a.restarts, f.restart);
        f.bpl = chunk_reads_in(a.r, kT_PLANE, kT_PLANE_END, f.begin, f.end, 20);
        if (a.m1[(size_t)k].present) f.N = a.m1[(size_t)k].lastV + 1;
        a.fields[(size_t)k] = f;
    }
    return a;
}

// The Copper starts running LISTT as soon as DMACON sets COPEN (CPU
// sequence step 5), well before M1 even starts sampling (step 7), so the
// list restarts several times before M1's own field 0 opens. The k-th
// *global* occurrence of a given address is therefore not field k's own
// mark: this looks it up within field k's own restart interval instead
// (a.fields[k].begin/.end, from M2).
std::optional<uint64_t> copper_mark(const TAnalysis& a, int k, int v, bool skip10) {
    if (skip10 && v == 10) return std::nullopt;
    if (k < 0 || k >= (int)a.fields.size()) return std::nullopt;
    int p = v; if (skip10 && v > 10) p = v - 1;
    uint32_t addr = listt_move_addr(p) / 2;
    auto it = a.readsByAddr.find(addr);
    if (it == a.readsByAddr.end()) return std::nullopt;
    uint64_t begin = a.fields[(size_t)k].begin, end = a.fields[(size_t)k].end;
    for (uint64_t c : it->second) if (c >= begin && c < end) return c;
    return std::nullopt;
}

bool pt1_valid(const TAnalysis& a) { return ok(a.r) && a.completed && m1_all_valid(a.m1); }

bool pt2_groups(const TAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        if (f.N < 0) return false;
        if ((int)f.bpl.size() != f.N - 44) return false;
        for (const auto& g : f.bpl) if (g.len != 20) return false;
    }
    return true;
}

bool pt7_anchor(const TAnalysis& a) {
    if (!a.m1.back().present) return false;
    uint64_t lastClose = a.m1.back().closeCycle;
    int n = 0;
    for (const auto& rs : a.restarts) if (rs.cycle > a.startCycle && rs.cycle <= lastClose) ++n;
    return n == kT_K;
}

// Δ = m_c(k,v) - m_b(k,v) for v = 44..254; returns true and *delta if a
// single value holds everywhere it is defined.
bool pt3_delta(const TAnalysis& a, int64_t* delta_out = nullptr) {
    if (!a.matched) return false;
    bool have = false; int64_t delta = 0;
    for (int k = 0; k < kT_K; ++k) {
        const auto& f = a.fields[(size_t)k];
        if (f.N < 0) return false;
        for (int v = 44; v <= 254 && v <= f.N - 1; ++v) {
            auto mc = copper_mark(a, k, v, false);
            int i = v - 44;
            if (!mc || i >= (int)f.bpl.size()) return false;
            int64_t d = (int64_t)*mc - (int64_t)f.bpl[(size_t)i].cycle;
            if (!have) { delta = d; have = true; } else if (d != delta) return false;
        }
    }
    if (have && delta_out) *delta_out = delta;
    return have;
}

bool pt4_restart_wait(const TAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k < kT_K; ++k) {
        const auto& f = a.fields[(size_t)k];
        auto mc0 = copper_mark(a, k, 0, false);
        auto mc1 = copper_mark(a, k, 1, false);
        if (!mc0 || !mc1) return false;
        int64_t sinceRestart = (int64_t)*mc0 - (int64_t)f.begin;
        if (sinceRestart <= 6) return false;
        int64_t line01 = (int64_t)*mc1 - (int64_t)*mc0;
        if (line01 != 227 * (int64_t)kClkSysPerColourClock && line01 != 228 * (int64_t)kClkSysPerColourClock) return false;
    }
    return true;
}

// line lengths (colour clocks) for v = 0..N_k-1 of field k, using the
// formulas of section 2.7. Needs field k+1's v=0 Copper mark for the
// last line, so only k = 0..K-2 are computable this way.
bool line_lengths(const TAnalysis& a, int k, int64_t delta, std::vector<int64_t>& out) {
    const auto& f = a.fields[(size_t)k];
    if (f.N < 0) return false;
    out.clear();
    for (int v = 0; v <= 253 && v + 1 <= f.N - 1; ++v) {
        auto c0 = copper_mark(a, k, v, false), c1 = copper_mark(a, k, v + 1, false);
        if (!c0 || !c1) return false;
        out.push_back(((int64_t)*c1 - (int64_t)*c0) / (int64_t)kClkSysPerColourClock);
    }
    for (int v = 254; v + 1 <= f.N - 2; ++v) {
        int i0 = v - 44, i1 = v + 1 - 44;
        if (i1 >= (int)f.bpl.size()) return false;
        out.push_back(((int64_t)f.bpl[(size_t)i1].cycle - (int64_t)f.bpl[(size_t)i0].cycle) / (int64_t)kClkSysPerColourClock);
    }
    // last line: v = N_k - 1
    if (k + 1 >= kT_K) return false; // needs the next field
    auto c_next0 = copper_mark(a, k + 1, 0, false);
    int iLast = (f.N - 1) - 44;
    if (!c_next0 || iLast < 0 || iLast >= (int)f.bpl.size()) return false;
    int64_t lastLen = ((int64_t)*c_next0 - (int64_t)f.bpl[(size_t)iLast].cycle - delta) / (int64_t)kClkSysPerColourClock;
    out.push_back(lastLen);
    return true;
}

bool pt5_line_lengths(const TAnalysis& a) {
    int64_t delta;
    if (!pt3_delta(a, &delta)) return false;
    for (int k = 0; k + 1 < kT_K; ++k) {
        std::vector<int64_t> lens;
        if (!line_lengths(a, k, delta, lens)) return false;
        for (int64_t L : lens) if (L != 227 && L != 228) return false;
    }
    return true;
}

bool p2(const TAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k < kT_K; ++k) {
        if (a.fields[(size_t)k].N != 262 && a.fields[(size_t)k].N != 263) return false;
    }
    for (int k = 0; k + 1 < kT_K; ++k)
        if (a.fields[(size_t)k].N == 263 && a.fields[(size_t)k + 1].N == 263) return false;
    return true;
}

bool pt6_gate(const TAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k + 1 < kT_K; ++k) if (a.fields[(size_t)k].N == a.fields[(size_t)k + 1].N) return false;
    return true;
}

bool p3(const TAnalysis& a, int* phase_out = nullptr) {
    if (!pt6_gate(a)) return false;
    int64_t delta;
    if (!pt3_delta(a, &delta)) return false;
    // Q[phase] = (isLong, lastLineLen)
    static const bool Qlong[4]  = {false, true, false, true};
    static const int  Qlen[4]   = {227, 228, 228, 227};
    for (int phase = 0; phase < 4; ++phase) {
        bool allMatch = true;
        for (int k = 0; k <= 11 && allMatch; ++k) {
            if (k + 1 >= kT_K) { allMatch = false; break; }
            bool isLong = a.fields[(size_t)k].N == 263;
            std::vector<int64_t> lens;
            if (!line_lengths(a, k, delta, lens) || lens.empty()) { allMatch = false; break; }
            int64_t xk = lens.back();
            int slot = (k + phase) % 4;
            if (isLong != Qlong[slot] || xk != Qlen[slot]) allMatch = false;
        }
        if (allMatch) { if (phase_out) *phase_out = phase; return true; }
    }
    return false;
}

void run_program_t_assertions(const std::string& dir) {
    TAnalysis a = analyze_t(dir, "prog_t", /*ntsc=*/true);

    checks.expect(ok(a.r) && W(a.r, kT_FIXMARK) && *W(a.r, kT_FIXMARK) == 0xBEEF &&
                      !write_cycles_of(a.r, kT_FIXMARK, 0xBEEF).empty() &&
                      write_cycles_of(a.r, kT_FIXMARK, 0xBEEF).front() < a.startCycle,
                  "FIX-T: Program T stores and delivers a known constant (BEEF) before M1 records anything");

    checks.expect(pt1_valid(a),
        "PT-1: Program T's fixture -- the completion word was written and all 13 M1 records are valid");
    checks.expect(pt2_groups(a),
        "PT-2: in every field, the bitplane fetch groups equal N_k - 44 (20 consecutive ascending words "
        "each), independently confirming N_k from the bitplane fetches");
    checks.expect(pt3_delta(a),
        "PT-3: the Copper-mark-to-bitplane-mark offset Delta has one single value on every line 44..254 "
        "of every field");
    checks.expect(pt4_restart_wait(a),
        "PT-4: in every field, the Copper restarts before its (0,$E0) WAIT is satisfied (mark follows the "
        "restart by more than 6 colour clocks) and really waited for it (line 0-to-1 is 227 or 228)");
    checks.expect(pt5_line_lengths(a),
        "PT-5: every measured line length (section 2.7) is 227 or 228 colour clocks");
    checks.expect(pt7_anchor(a),
        "PT-7: between the first and last recorded wrap there are exactly 13 Copper restarts, each the "
        "first word of LISTT");

    bool p2ok = p2(a);
    checks.expect(p2ok,
        "P2 p24,p34 COP2-vertical-beam-position.c13 COP2-using-the-copper-in-interlaced-mode.c3 "
        "COP2-using-the-copper-in-interlaced-mode.c7 COP2-vertical-beam-position.c14: every NTSC "
        "interlaced field has 262 or 263 lines, and no two consecutive fields are both 263");

    bool gate = pt6_gate(a);
    checks.expect(gate,
        "PT-6: consecutive field lengths alternate between 262 and 263 (the gate P3 needs)");
    if (gate) {
        int phase = -1;
        bool p3ok = p3(a, &phase);
        checks.expect(p3ok,
            "P3 p24 ch2-vertical-beam-position.c15 ch2-vertical-beam-position.c16 "
            "ch2-vertical-beam-position.c17 ch2-vertical-beam-position.c18 ch2-vertical-beam-position.c19 "
            "COP2-vertical-beam-position.c17 COP2-vertical-beam-position.c18 COP2-vertical-beam-position.c19 "
            "COP2-vertical-beam-position.c20 COP2-vertical-beam-position.c21 COP2-vertical-beam-position.c22: "
            "the NTSC 4-field pattern (short/short-line, long/long-line, short/long-line, long/short-line) "
            "repeats for 3 whole cycles at one fixed phase");
        if (p3ok) checks.note("P3 phase = " + std::to_string(phase));
    } else {
        checks.record("P3: not graded, gate closed (PT-6 did not hold on this run)");
    }
}

void run_t_mutation(const std::string& dir, const std::string& label) {
    static const std::map<std::string, std::pair<std::string,bool>> kProg = {
        {"P2", {"prog_t", false}},
        {"P3", {"prog_t", false}},
        {"PT-1", {"prog_t_f", true}},
        {"PT-2", {"prog_t_e", true}},
        {"PT-3", {"prog_t_d", true}},
        {"PT-4", {"prog_t_c", true}},
        {"PT-5", {"prog_t_g", true}},
        {"PT-6", {"prog_t_a", true}},
        {"PT-7", {"prog_t_h", true}},
    };
    auto it = kProg.find(label);
    if (it == kProg.end()) { checks.record("NMH_MUTATE=" + label + " names no Program-T mutation"); return; }
    TAnalysis a = analyze_t(dir, it->second.first, it->second.second);

    if (label == "P2" || label == "P3") {
        checks.expect(!p2(a), label + " MUTATION stimulus red: PAL selected instead of NTSC -- P2's {262,263} check fails");
    } else if (label == "PT-1") {
        checks.expect(!pt1_valid(a), label + " MUTATION stimulus red: M1 waits between samples -- records are no longer all valid");
    } else if (label == "PT-2") {
        checks.expect(!pt2_groups(a), label + " MUTATION stimulus red: DIWSTOP is the normal NTSC value -- the display window closes before the field ends, so groups no longer equal N_k-44");
    } else if (label == "PT-3") {
        checks.expect(!pt3_delta(a), label + " MUTATION stimulus red: line 100's mark uses H=$C0 -- Delta is no longer single-valued");
    } else if (label == "PT-4") {
        checks.expect(!pt4_restart_wait(a), label + " MUTATION stimulus red: the first WAIT is (0,$00) -- it is satisfied at once, so the restart-to-mark margin collapses");
    } else if (label == "PT-5") {
        checks.expect(!pt5_line_lengths(a), label + " MUTATION stimulus red: line 10's WAIT+MOVE is deleted -- one measured line length is no longer 227 or 228");
    } else if (label == "PT-6") {
        checks.expect(!pt6_gate(a), label + " MUTATION stimulus red: LACE=0 -- field lengths no longer alternate, the gate closes");
    } else if (label == "PT-7") {
        checks.expect(!pt7_anchor(a), label + " MUTATION stimulus red: DMACON leaves COPEN clear -- no Copper restarts occur");
    }
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));

    if (!g_mutate) {
        run_program_i_assertions(dir);
        run_program_t_assertions(dir);
    } else {
        std::string label = g_mutate;
        run_i_mutation(dir, label);
        run_t_mutation(dir, label);
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
