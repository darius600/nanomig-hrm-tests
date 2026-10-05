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
// once (plus the LACE-clear control I-a) and Program T once, and every
// assertion and precondition below is graded from those shared
// measurements -- ram_log() and peek_logical() are read-only, so sharing
// them cannot let one assertion affect another's verdict.
//
// Mutation runs (NMH_MUTATE=<label>) load the one variant program the label
// calls for, run the label's own assertion function -- the same function,
// with the same label text and the same comparator the normal run uses --
// and print its line, which must be FAIL. The live controls of a mutation
// run (the preconditions that must hold, and the contrasts the page names)
// print before it as "<label>/<control>: ..." PASS lines; they are not
// assertions of the package. A precondition that fails in a mutation run
// turns the labelled line into NOT GRADED, never PASS or FAIL.
//
// P3 is red in the normal run (the measured last line of every NTSC field is
// 227 colour clocks, p23-p24 give 227 and 228 alternating by field type);
// it stays graded. No input in this package makes its comparator green.
//
// MUTATION FIX-I stimulus red: NMH_MUTATE=FIX-I runs prog_i_n (FIXMARK stores $DEAD, not $BEEF, I-n); the known-value comparator prints FAIL
// MUTATION PI-1 stimulus red: NMH_MUTATE=PI-1 runs prog_i_j (M1 waits between samples, I-j); PI-1's comparator prints FAIL
// MUTATION PI-2 stimulus red: NMH_MUTATE=PI-2 runs prog_i_f (DMACON leaves COPEN clear, I-f); PI-2's comparator prints FAIL
// MUTATION PI-4 stimulus red: NMH_MUTATE=PI-4 runs prog_i_h (DMACON leaves BPLEN clear, I-h); PI-4's comparator prints FAIL
// MUTATION PI-5 stimulus red: NMH_MUTATE=PI-5 runs prog_i_g (INTENA leaves COPER clear, I-g); PI-5's comparator prints FAIL
// MUTATION PI-6 stimulus red: NMH_MUTATE=PI-6 runs prog_i_k (PIC filled with $0000, I-k); PI-6's comparator prints FAIL; control PI-1
// MUTATION PI-7(bpl) stimulus red: NMH_MUTATE=PI-7(bpl) runs prog_i_l (the BPL1PT MOVEs sit after the WAIT, I-l); the comparator locates the MOVEs in the assembled list and prints FAIL
// MUTATION PI-7(spr) stimulus red: NMH_MUTATE=PI-7(spr) runs prog_i_m (the SPR0PT MOVEs sit after the WAIT, I-m); the comparator locates the MOVEs in the assembled list and prints FAIL
// MUTATION PI-8 stimulus red: NMH_MUTATE=PI-8 runs prog_i_i (DMACON leaves SPREN clear, I-i); PI-8's comparator prints FAIL
// MUTATION PI-9 stimulus red: NMH_MUTATE=PI-9 runs prog_i_o as the control run (LACE clear and M1 waits between samples, I-o); PI-9's comparator prints FAIL
// MUTATION PI-3 stimulus red: NMH_MUTATE=PI-3 runs prog_i_a (LACE clear, I-a); P1's comparator under PI-3's label prints FAIL; controls: all 8 lengths equal, PI-1, PI-2, PI-4
// MUTATION P1 stimulus red: NMH_MUTATE=P1 runs prog_i_a (LACE clear, I-a); P1's comparator prints FAIL; controls: all 8 lengths equal, PI-1, PI-2, PI-4
// MUTATION P1-NOLACE stimulus red: NMH_MUTATE=P1-NOLACE runs prog_i_a as the interlace run (LACE clear, I-a) and NMH_MUTATE=P1-NOLACE(ctrl) runs prog_i as the control run (LACE set); the unchanged conjunction prints FAIL; the other half is a live control; controls PI-1, PI-2, PI-4, PI-9
// MUTATION P4a stimulus red: NMH_MUTATE=P4a runs prog_i_b (LISTB writes PIC, not PIC+40, I-b); P4a's comparator prints FAIL; controls PI-1..PI-8, P4e
// MUTATION P4b stimulus red: NMH_MUTATE=P4b runs prog_i_b (I-b); P4b's comparator prints FAIL; controls PI-1..PI-6, P4e
// MUTATION P4c stimulus red: NMH_MUTATE=P4c runs prog_i_b (I-b); P4c's comparator prints FAIL; controls PI-1..PI-6, P4e
// MUTATION P4d stimulus red: NMH_MUTATE=P4d runs prog_i_b (I-b) and NMH_MUTATE=P4d(step) runs prog_i_p (both lists keep PIC and PIC+40 but BPL1MOD = BPL2MOD = 38, I-p, so A-group i to B-group i still rises by 40 and B-group i to A-group i+1 does not); P4d's comparator prints FAIL; the three clauses of the comparator print as measured lines; controls PI-1..PI-6, P4e
// MUTATION P4e stimulus red: NMH_MUTATE=P4e runs prog_i_d (the handler never repoints COP1LC, I-d); P4e's comparator prints FAIL; controls PI-1..PI-6
// MUTATION P5 stimulus red: NMH_MUTATE=P5 runs prog_i_c (both lists omit the BPL1PT MOVEs, I-c); P5's comparator prints FAIL through the first 20-word fetch measured wherever it lies; controls: the search populates every field, PI-1, PI-2, PI-6; PI-4 and PI-7(bpl) do not apply
// MUTATION P5-NOREWRITE stimulus red: NMH_MUTATE=P5-NOREWRITE runs prog_i_c as the rewrite run (both lists omit the BPL1PT MOVEs, I-c) and NMH_MUTATE=P5-NOREWRITE(ctrl) runs prog_i as the no-rewrite run (the lists rewrite, I-0); the unchanged conjunction prints FAIL; the other half is a live control; controls PI-1, PI-2, PI-10
// MUTATION PI-10 stimulus red: NMH_MUTATE=PI-10 runs prog_i_q as the no-rewrite run (I-c and M1 waits between samples, I-q); PI-10's comparator prints FAIL
// MUTATION P6 stimulus red: NMH_MUTATE=P6 runs prog_i_e (both lists omit the SPR0PT MOVEs, I-e); P6's comparator prints FAIL; controls PI-1, PI-2, PI-8
// MUTATION P6-NOREWRITE stimulus red: NMH_MUTATE=P6-NOREWRITE runs prog_i_e as the rewrite run (both lists omit the SPR0PT MOVEs, I-e) and NMH_MUTATE=P6-NOREWRITE(ctrl) runs prog_i as the no-rewrite run (the lists rewrite, I-0); the unchanged conjunction prints FAIL; the other half is a live control; controls PI-1, PI-2, PI-8, PI-11
// MUTATION PI-11 stimulus red: NMH_MUTATE=PI-11 runs prog_i_r as the no-rewrite run (I-e and M1 waits between samples, I-r); PI-11's comparator prints FAIL
// MUTATION FIX-T stimulus red: NMH_MUTATE=FIX-T runs prog_t_i (FIXMARK stores $DEAD, not $BEEF, T-i); the known-value comparator prints FAIL
// MUTATION PT-1 stimulus red: NMH_MUTATE=PT-1 runs prog_t_f (M1 waits between samples, T-f); PT-1's comparator prints FAIL
// MUTATION PT-2 stimulus red: NMH_MUTATE=PT-2 runs prog_t_e (DIWSTOP is the normal NTSC value, T-e); PT-2's comparator prints FAIL
// MUTATION PT-3 stimulus red: NMH_MUTATE=PT-3 runs prog_t_d (line 100's mark uses H=$C0, T-d); PT-3's comparator prints FAIL
// MUTATION PT-5 stimulus red: NMH_MUTATE=PT-5 runs prog_t_g (line 10's WAIT uses H=$40 and every mark stays live, T-g); PT-5's comparator prints FAIL through measured line lengths outside {227,228}; controls PT-1, PT-3, PT-4 and every mark present
// MUTATION PT-7 stimulus red: NMH_MUTATE=PT-7 runs prog_t_h (DMACON leaves COPEN clear, T-h); PT-7's comparator prints FAIL
// MUTATION P2 stimulus red: NMH_MUTATE=P2 runs prog_t with PAL selected instead of NTSC (T-b); P2's comparator prints FAIL; controls PT-1, PT-2, PT-7
// GATE PT-4 (printed as a MEASURED gate state, not graded; no assertion, so no MUTATION record): NMH_MUTATE=PT-4 runs prog_t_c (the first WAIT is (0,$00), T-c); the gate prints closed through line 0-to-1, PT-5 and P3 print NOT GRADED; controls PT-1, PT-3
// GATE PT-6 (printed as a MEASURED gate state, not graded; no assertion, so no MUTATION record): NMH_MUTATE=PT-6 runs prog_t_a (LACE clear, T-a); the gate prints closed and P3 prints "not graded, gate closed"; control PT-1
// MUTATION P3 none impossible: already red in the normal run and not exercised further: NMH_MUTATE=P3 runs prog_t with PAL selected (T-b) and P3's comparator prints FAIL again; no input in this package makes P3's comparator green, so what is shown is that the controls PT-1..PT-7 hold while it is red, not that the comparator can pass

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

// Custom-register offsets (Appendix B) the lists are searched for.
constexpr uint16_t kBPL1PTH = 0x0E0, kBPL1PTL = 0x0E2, kSPR0PTH = 0x120, kSPR0PTL = 0x122;

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
    // A live control of a mutation run: printed like a graded line, but it is
    // not an assertion of the package -- it only shows that the mutation
    // changed the one thing it was meant to change.
    void control(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0, failed = 0;
};
Checks checks;

const char* g_mutate = std::getenv("NMH_MUTATE");

// A control line of mutation run <label>: "<label>/<what>: live control".
void ctl(const std::string& label, const std::string& what, bool c) {
    checks.control(c, label + "/" + what + ": live control, expected to hold in this run");
}

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

// The byte offset, inside the Copper list at listbase, of the MOVE to custom
// register `reg`, read from the list as the program assembled it (stimulus
// data in chip memory, not a fetch); UINT32_MAX if the list holds none.
uint32_t list_move_offset(const Run& r, uint32_t listbase, uint16_t reg) {
    for (uint32_t off = 0; off < 0x100; off += 4) {
        auto w1 = W(r, listbase + off);
        if (!w1 || *w1 == 0xFFFF) return UINT32_MAX;
        if (!(*w1 & 1) && (*w1 & 0x1FE) == reg) return off;
    }
    return UINT32_MAX;
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
// Program I: P1, P4, P5, P6 and their preconditions PI-1..PI-9
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

bool fix_i(const IAnalysis& a) {
    return ok(a.r) && W(a.r, kI_FIXMARK) && *W(a.r, kI_FIXMARK) == 0xBEEF &&
           !write_cycles_of(a.r, kI_FIXMARK, 0xBEEF).empty() &&
           write_cycles_of(a.r, kI_FIXMARK, 0xBEEF).front() < a.startCycle;
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

// All 8 recorded field lengths of a run are present and equal.
bool all_lengths_equal(const IAnalysis& a, std::vector<int>* Ns = nullptr) {
    if (!a.m1.back().present) return false;
    std::vector<int> N;
    for (int k = 0; k < kI_K; ++k) {
        if (!a.m1[(size_t)k].present) return false;
        N.push_back(a.m1[(size_t)k].lastV + 1);
    }
    for (size_t k = 1; k < N.size(); ++k) if (N[k] != N[0]) return false;
    if (Ns) *Ns = N;
    return true;
}

// The LACE-clear control run is a valid fixture: PI-1 and PI-2 on that run.
bool pi9_control_valid(const IAnalysis& c) { return pi1_valid(c) && pi2_anchor(c); }

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

// Finds the port cycle of the earliest read of word address `wa` within
// [begin,end), or UINT64_MAX if there is none.
uint64_t first_read_cycle(const Run& r, uint32_t wa, uint64_t begin, uint64_t end) {
    for (const auto& acc : r.log)
        if (!acc.is_write && acc.word_addr == wa && acc.cycle >= begin && acc.cycle < end) return acc.cycle;
    return UINT64_MAX;
}

// The Copper fetches of the two MOVEs that write `regH` and `regL` are found
// at the offsets the assembled list holds them at (list_move_offset), so the
// same comparator serves the normal list and the variants that move them.
bool move_pair_fetched_before(const IAnalysis& a, uint16_t regH, uint16_t regL, bool bitplane) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        uint32_t listbase = (f.list == 'A') ? kI_LISTA : kI_LISTB;
        uint32_t offH = list_move_offset(a.r, listbase, regH);
        uint32_t offL = list_move_offset(a.r, listbase, regL);
        if (offH == UINT32_MAX || offL == UINT32_MAX) return false;
        uint64_t cycH = first_read_cycle(a.r, (listbase + offH) / 2, f.begin, f.end);
        uint64_t cycL = first_read_cycle(a.r, (listbase + offL) / 2, f.begin, f.end);
        if (cycH == UINT64_MAX || cycL == UINT64_MAX) return false;
        uint64_t firstFetch = UINT64_MAX;
        if (bitplane) {
            if (!f.bpl.empty()) firstFetch = f.bpl.front().cycle;
        } else {
            for (const auto& acc : a.r.log)
                if (!acc.is_write && acc.word_addr * 2 >= kI_SPR && acc.word_addr * 2 < kI_SPR + 16 &&
                    acc.cycle >= f.begin && acc.cycle < f.end) { firstFetch = acc.cycle; break; }
        }
        if (firstFetch == UINT64_MAX || cycH >= firstFetch) return false;
    }
    return true;
}
bool pi7_bpl(const IAnalysis& a) { return move_pair_fetched_before(a, kBPL1PTH, kBPL1PTL, true); }
bool pi7_spr(const IAnalysis& a) { return move_pair_fetched_before(a, kSPR0PTH, kSPR0PTL, false); }

bool pi8_spr_fetched(const IAnalysis& a) { return read_count_range(a.r, kI_SPR, kI_SPR + 16) > 0; }

// The first bitplane fetch of every field is at the address its list wrote.
bool p4a_each(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        if (f.bpl.empty()) return false;
        uint32_t want = (f.list == 'A') ? kI_PIC : kI_PIC + 40;
        if (f.bpl.front().addr * 2 != want) return false;
    }
    return true;
}

// Both kinds of field occur among the recorded fields.
bool both_kinds(const IAnalysis& a) {
    bool hasA = false, hasB = false;
    for (const auto& f : a.fields) { if (f.list == 'A') hasA = true; if (f.list == 'B') hasB = true; }
    return hasA && hasB;
}

// Alternation without a fixed first list: no two consecutive fields run
// the same list, and every field runs one of the two.
bool p4e(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) if (f.list != 'A' && f.list != 'B') return false;
    for (int k = 0; k + 1 < kI_K; ++k) if (a.fields[(size_t)k].list == a.fields[(size_t)k + 1].list) return false;
    return true;
}

bool p4a(const IAnalysis& a) { return both_kinds(a) && p4a_each(a); }

// For every two consecutive recorded fields, the B-field's starting address
// is 40 bytes past the A-field's, whichever of the two comes first; both
// orders (A then B, B then A) occur. Alternation is part of the comparator,
// so a run in which one kind never occurs cannot pass.
bool p4b(const IAnalysis& a) {
    if (!p4e(a)) return false;
    int ab = 0, ba = 0;
    for (int k = 0; k + 1 < kI_K; ++k) {
        const auto& f0 = a.fields[(size_t)k];
        const auto& f1 = a.fields[(size_t)k + 1];
        if (f0.bpl.empty() || f1.bpl.empty()) return false;
        const auto& fa = (f0.list == 'A') ? f0 : f1;
        const auto& fb = (f0.list == 'A') ? f1 : f0;
        if ((int64_t)fb.bpl.front().addr * 2 - (int64_t)fa.bpl.front().addr * 2 != 40) return false;
        if (f0.list == 'A') ++ab; else ++ba;
    }
    return ab >= 1 && ba >= 1;
}

bool p4c(const IAnalysis& a) {
    if (!p4e(a)) return false;
    for (const auto& f : a.fields) {
        if (f.bpl.size() != 256) return false;
        uint32_t base = (f.list == 'A') ? kI_PIC : (kI_PIC + 40);
        for (int i = 0; i < 256; ++i)
            if (f.bpl[(size_t)i].addr * 2 != base + 80u * (uint32_t)i) return false;
    }
    return true;
}

// Section 5, P4 (d), over every two consecutive recorded fields (an A-field
// and a B-field, in either order). `set`: together they fetch memory lines
// 0..511 each exactly once; `ab`: A-group i to B-group i rises by 40 bytes;
// `ba`: B-group i to A-group i+1 rises by 40 bytes (the screen order
// A-group 0, B-group 0, A-group 1, ...).
struct P4dParts { bool set = false, ab = false, ba = false; };
P4dParts p4d_parts(const IAnalysis& a) {
    P4dParts p;
    if (!p4e(a)) return p;
    p.set = p.ab = p.ba = true;
    for (int k = 0; k + 1 < kI_K; ++k) {
        const auto& f0 = a.fields[(size_t)k];
        const auto& f1 = a.fields[(size_t)k + 1];
        const auto& fa = (f0.list == 'A') ? f0 : f1;
        const auto& fb = (f0.list == 'A') ? f1 : f0;
        if (fa.bpl.size() != 256 || fb.bpl.size() != 256) return P4dParts{};
        std::vector<bool> seen(512, false);
        int distinct = 0;
        for (int i = 0; i < 256; ++i) {
            for (const BplGroup* g : {&fa.bpl[(size_t)i], &fb.bpl[(size_t)i]}) {
                int64_t off = (int64_t)g->addr * 2 - (int64_t)kI_PIC;
                if (off < 0 || off % 40 != 0 || off / 40 >= 512) { p.set = false; continue; }
                size_t line = (size_t)(off / 40);
                if (seen[line]) p.set = false; else { seen[line] = true; ++distinct; }
            }
            int64_t addrA = (int64_t)fa.bpl[(size_t)i].addr * 2, addrB = (int64_t)fb.bpl[(size_t)i].addr * 2;
            if (addrB - addrA != 40) p.ab = false;
            if (i + 1 < 256 && (int64_t)fa.bpl[(size_t)i + 1].addr * 2 - addrB != 40) p.ba = false;
        }
        if (distinct != 512) p.set = false;
    }
    return p;
}
bool p4d(const IAnalysis& a) { P4dParts p = p4d_parts(a); return p.set && p.ab && p.ba; }

// The first 20-word bitplane fetch of every field, found wherever it lies in
// chip memory from PIC upward (the identity-mapped data area; the program's
// own code lies below PIC), so that a field whose pointer was not rewritten
// is measured at its real address and not through an empty window.
bool p5_first_groups(const IAnalysis& a, std::vector<BplGroup>& out) {
    out.clear();
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        auto groups = group_reads_in(a.r, kI_PIC, nmh::kChipIdentityHi, f.begin, f.end);
        const BplGroup* found = nullptr;
        for (const auto& g : groups) if (g.len == 20) { found = &g; break; }
        if (!found) return false;
        out.push_back(*found);
    }
    return true;
}
bool p5(const IAnalysis& a) {
    std::vector<BplGroup> firsts;
    if (!p5_first_groups(a, firsts)) return false;
    for (size_t k = 0; k < firsts.size(); ++k) {
        uint32_t want = (a.fields[k].list == 'A') ? kI_PIC : kI_PIC + 40;
        if (firsts[k].addr * 2 != want) return false;
    }
    return true;
}

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

// The "not rewritten" half of ch2-putting-together-a-copper-instruction-list.c3
// on the run whose lists omit the BPL1PT MOVEs: in no later field (index 1
// upward; the first recorded field may be the first field after DMA-on, which
// starts at the pointer the CPU wrote) does the first 20-word fetch start at
// the address the rewrite would have given. An unpopulated search is false.
bool p5_norewrite(const IAnalysis& c) {
    std::vector<BplGroup> firsts;
    if (!p5_first_groups(c, firsts) || firsts.size() != (size_t)kI_K) return false;
    for (size_t k = 1; k < firsts.size(); ++k) {
        uint32_t rewritten = (c.fields[k].list == 'A') ? kI_PIC : kI_PIC + 40;
        if (firsts[k].addr * 2 == rewritten) return false;
    }
    return true;
}

// The four data words of the sprite structure fetched in field f.
int sprite_words_fetched_in(const IAnalysis& a, const IField& f) {
    int n = 0;
    for (uint32_t wbyte : {kI_SPR + 4, kI_SPR + 6, kI_SPR + 8, kI_SPR + 10}) {
        uint32_t wa = wbyte / 2;
        for (const auto& acc : a.r.log)
            if (!acc.is_write && acc.word_addr == wa && acc.cycle >= f.begin && acc.cycle < f.end) { ++n; break; }
    }
    return n;
}

// The "not rewritten" half of ch2-putting-together-a-copper-instruction-list.c4
// on the run whose lists omit the SPR0PT MOVEs: none of the four data words is
// fetched in any later field (index 1 upward, as for p5_norewrite).
bool p6_norewrite(const IAnalysis& c) {
    if (!c.matched) return false;
    for (size_t k = 1; k < c.fields.size(); ++k)
        if (sprite_words_fetched_in(c, c.fields[k]) != 0) return false;
    return true;
}

// The no-rewrite runs are valid fixtures: PI-1 and PI-2 on the run, and the
// population the comparator reads is there (the 20-word search finds a group in
// every field of the I-c run; every field of the I-e run is a live bitplane
// field, PI-4, and the sprite structure is fetched at some time, PI-8).
bool pi10_ctrl_valid(const IAnalysis& c) {
    std::vector<BplGroup> firsts;
    return pi1_valid(c) && pi2_anchor(c) && p5_first_groups(c, firsts);
}
bool pi11_ctrl_valid(const IAnalysis& c) {
    return pi1_valid(c) && pi2_anchor(c) && pi4_groups(c) && pi8_spr_fetched(c);
}

// The preconditions of Program I on one run.
struct IPre { bool pi1, pi2, pi3, pi4, pi5, pi6, pi7b, pi7s, pi8; };
IPre i_pre(const IAnalysis& a) {
    return {pi1_valid(a), pi2_anchor(a), p1_alternation(a), pi4_groups(a), pi5_hcnt(a),
            pi6_tags(a), pi7_bpl(a), pi7_spr(a), pi8_spr_fetched(a)};
}
bool p1_gate(const IPre& p)  { return p.pi1 && p.pi2 && p.pi4; }
bool p5_gate(const IPre& p)  { return p.pi1 && p.pi2 && p.pi4 && p.pi6 && p.pi7b; }
bool p6_gate(const IPre& p)  { return p.pi1 && p.pi2 && p.pi7s && p.pi8; }
bool p4_gate(const IPre& p)  { return p.pi1 && p.pi2 && p.pi3 && p.pi4 && p.pi5 && p.pi6; }

// ---- Program I assertions: one function per label, used by the normal
// run and by that label's mutation run alike ----------------------------

void g_FIX_I(const IAnalysis& a) {
    checks.expect(fix_i(a),
                  "FIX-I: Program I stores and delivers a known constant (BEEF) before M1 records anything");
}
void g_PI1(const IAnalysis& a) {
    checks.expect(pi1_valid(a),
        "PI-1: Program I's fixture -- the completion word was written and all 8 M1 records are valid "
        "(first V is 0, gap flag clear, last V >= 255)");
}
void g_PI2(const IAnalysis& a) {
    checks.expect(pi2_anchor(a),
        "PI-2: between the first and last recorded wrap there are exactly 8 reads of LISTA's or LISTB's "
        "first word (located by address; whether each is the first Copper fetch after $FFFF,$FFFE, as "
        "section 2.6 defines a restart, is not itself checked)");
}
void g_PI4(const IAnalysis& a) {
    checks.expect(pi4_groups(a),
        "PI-4: every recorded field contains exactly 256 groups of 20 consecutive ascending bitplane fetches");
}
void g_PI5(const IAnalysis& a) {
    checks.expect(pi5_hcnt(a),
        "PI-5: HCNT rises by exactly 1 in every recorded field");
}
void g_PI6(const IAnalysis& a) {
    checks.expect(pi6_tags(a),
        "PI-6: every bitplane fetch from inside PIC returns the tag ($8000 OR its memory line) of its address");
}
void g_PI7bpl(const IAnalysis& a) {
    checks.expect(pi7_bpl(a),
        "PI-7(bpl): in every field, the Copper fetch of the BPL1PT MOVEs precedes the field's first bitplane fetch");
}
void g_PI7spr(const IAnalysis& a) {
    checks.expect(pi7_spr(a),
        "PI-7(spr): in every field, the Copper fetch of the SPR0PT MOVEs precedes the field's first sprite-0 fetch");
}
void g_PI8(const IAnalysis& a) {
    checks.expect(pi8_spr_fetched(a),
        "PI-8: the sprite-0 structure at SPR is fetched at least once during the run");
}
void g_PI9(const IAnalysis& ctrl) {
    checks.expect(pi9_control_valid(ctrl),
        "PI-9: the LACE-clear control run (I-a) is a valid fixture -- its completion word was written, all 8 "
        "of its M1 records are valid and its restart anchor holds (PI-1 and PI-2 on that run)");
}
void g_PI3(const IAnalysis& a) {
    checks.expect(p1_alternation(a),
        "PI-3: interlace is active -- the check of P1 holds");
}
void g_P1(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P1: NOT GRADED, a precondition (PI-1, PI-2 or PI-4) did not hold on this run"); return; }
    checks.expect(p1_alternation(a),
        "P1 p24,p34 COP2-vertical-beam-position.c12 COP2-vertical-beam-position.c15 "
        "COP2-vertical-beam-position.c16 COP2-using-the-copper-in-interlaced-mode.c5 "
        "COP2-using-the-copper-in-interlaced-mode.c6 COP2-using-the-copper-in-interlaced-mode.c11 "
        "ch2-using-the-copper-in-interlaced-mode.c6: with LACE set, all 8 recorded PAL fields have "
        "312 or 313 lines, and every consecutive pair sums to 625 (alternates)");
}
void g_P1_NOLACE(const IAnalysis& p1run, const IAnalysis& ctrl, bool pre) {
    if (!pre) { checks.record("P1-NOLACE: NOT GRADED, a precondition (PI-1, PI-2, PI-4 or PI-9) did not hold"); return; }
    checks.expect(p1_alternation(p1run) && all_lengths_equal(ctrl),
        "P1-NOLACE p24 ch2-vertical-beam-position.c12: long and short fields occur in interlace only -- "
        "with LACE set all 8 recorded PAL fields alternate between 312 and 313 lines, and with LACE "
        "clear (I-a) all 8 recorded field lengths are equal");
}
void g_P4a(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4a: NOT GRADED, a precondition (PI-1 to PI-6) did not hold on this run"); return; }
    checks.expect(p4a(a),
        "P4a p34 ch2-using-the-copper-in-interlaced-mode.c16 COP2-using-the-copper-in-interlaced-mode.c15: the "
        "first bitplane fetch of every A-field is at PIC and of every B-field at PIC+40, and both kinds of "
        "field occur");
}
void g_P4b(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4b: NOT GRADED, a precondition (PI-1 to PI-6) did not hold on this run"); return; }
    checks.expect(p4b(a),
        "P4b p34 ch2-using-the-copper-in-interlaced-mode.c17 ch2-using-the-copper-in-interlaced-mode.c18 "
        "COP2-using-the-copper-in-interlaced-mode.c16 COP2-using-the-copper-in-interlaced-mode.c17: "
        "for every two consecutive recorded fields, which alternate between the A-list and the B-list, the "
        "B-field's starting address is 40 bytes past the A-field's (one picture line), in both orders "
        "(A then B, B then A)");
}
void g_P4c(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4c: NOT GRADED, a precondition (PI-1 to PI-6) did not hold on this run"); return; }
    checks.expect(p4c(a),
        "P4c p34 ch2-using-the-copper-in-interlaced-mode.c10 ch2-using-the-copper-in-interlaced-mode.c13 "
        "ch2-using-the-copper-in-interlaced-mode.c14 COP2-using-the-copper-in-interlaced-mode.c12 "
        "COP2-using-the-copper-in-interlaced-mode.c13: the recorded fields alternate between the A-list and "
        "the B-list, in every A-field fetch group i reads memory line 2i, and in every B-field group i "
        "reads 2i+1");
}
void g_P4d(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4d: NOT GRADED, a precondition (PI-1 to PI-6) did not hold on this run"); return; }
    checks.expect(p4d(a),
        "P4d p34 ch2-using-the-copper-in-interlaced-mode.c15 ch2-using-the-copper-in-interlaced-mode.c19 "
        "COP2-using-the-copper-in-interlaced-mode.c18 COP2-using-the-copper-in-interlaced-mode.c19: every "
        "two consecutive recorded fields (an A-field and a B-field, in either order) together fetch memory "
        "lines 0..511 each exactly once, and in screen order (A-group 0, B-group 0, A-group 1, ...) the "
        "group addresses rise by 40 bytes at every step");
}
void g_P4e(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4e: NOT GRADED, a precondition (PI-1 to PI-6) did not hold on this run"); return; }
    checks.expect(p4e(a),
        "P4e p34,p35 ch2-using-the-copper-in-interlaced-mode.c20.s2: "
        "over the recorded fields the restart list strictly alternates between LISTA and LISTB, so every "
        "B-field is the field directly after an A-field (order only, whichever list runs first)");
}
void g_P5(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P5: NOT GRADED, a precondition (PI-1, PI-2, PI-4, PI-6 or PI-7(bpl)) did not hold on this run"); return; }
    checks.expect(p5(a),
        "P5 p27 ch2-using-the-copper-in-interlaced-mode.c16 "
        "COP2-using-the-copper-in-interlaced-mode.c15: rewriting the bitplane pointer during vertical "
        "blanking starts every field's retrieval at the rewritten address");
}
void g_P6(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P6: NOT GRADED, a precondition (PI-1, PI-2, PI-7(spr) or PI-8) did not hold on this run"); return; }
    checks.expect(p6(a),
        "P6 p27: rewriting the sprite-0 pointer during "
        "vertical blanking fetches the sprite's data in every recorded field");
}
void g_PI10(const IAnalysis& c) {
    checks.expect(pi10_ctrl_valid(c),
        "PI-10: the no-BPL1PT-rewrite run (I-c) is a valid fixture -- its completion word was written, all 8 "
        "of its M1 records are valid, its restart anchor holds and the first 20-word fetch of every one of "
        "its fields is found (PI-1 and PI-2 on that run, and the search P5-NOREWRITE reads is populated)");
}
void g_PI11(const IAnalysis& c) {
    checks.expect(pi11_ctrl_valid(c),
        "PI-11: the no-SPR0PT-rewrite run (I-e) is a valid fixture -- its completion word was written, all 8 "
        "of its M1 records are valid, its restart anchor holds, every one of its fields has 256 groups of 20 "
        "bitplane fetches and the sprite structure is fetched at least once (PI-1, PI-2, PI-4 and PI-8 on "
        "that run)");
}
void g_P5_NOREWRITE(const IAnalysis& a, const IAnalysis& c, bool pre) {
    if (!pre) { checks.record("P5-NOREWRITE: NOT GRADED, a precondition (PI-1, PI-2, PI-4, PI-6 or PI-7(bpl) on the rewrite run, or PI-10 on the no-rewrite run) did not hold"); return; }
    checks.expect(p5(a) && p5_norewrite(c),
        "P5-NOREWRITE p27 ch2-putting-together-a-copper-instruction-list.c3: the bitplane pointers must be "
        "rewritten during vertical blanking so the data is properly retrieved -- with the rewrite every "
        "recorded field starts at the rewritten address, and without it (I-c) no later recorded field "
        "starts there");
}
void g_P6_NOREWRITE(const IAnalysis& a, const IAnalysis& c, bool pre) {
    if (!pre) { checks.record("P6-NOREWRITE: NOT GRADED, a precondition (PI-1, PI-2, PI-7(spr) or PI-8 on the rewrite run, or PI-11 on the no-rewrite run) did not hold"); return; }
    checks.expect(p6(a) && p6_norewrite(c),
        "P6-NOREWRITE p27 ch2-putting-together-a-copper-instruction-list.c4: the sprite pointers must be "
        "rewritten during vertical blanking so the data is properly retrieved -- with the rewrite the "
        "sprite's data is fetched in every recorded field, and without it (I-e) no later recorded field "
        "fetches it");
}

void run_program_i_assertions(const std::string& dir) {
    IAnalysis a = analyze_i(dir, "prog_i");
    IPre p = i_pre(a);

    g_FIX_I(a);
    g_PI1(a);
    g_PI2(a);
    g_PI4(a);
    g_PI5(a);
    g_PI6(a);
    g_PI7bpl(a);
    g_PI7spr(a);
    g_PI8(a);
    g_PI3(a);
    g_P1(a, p1_gate(p));
    std::vector<int> N;
    if (p1_alternation(a, &N)) {
        std::string s;
        for (size_t i = 0; i < N.size(); ++i) s += (i ? "," : "") + std::to_string(N[i]);
        checks.note("P1 field lengths N_0..N_7 = " + s);
    }

    // The LACE-clear control run (I-a), run here (not as a mutation) so that
    // ch2-vertical-beam-position.c12's "(interlace only)" quantifier gets a
    // graded line of its own, behind its own fixture precondition.
    IAnalysis ctrl = analyze_i(dir, "prog_i_a");
    g_PI9(ctrl);
    g_P1_NOLACE(a, ctrl, p1_gate(p) && pi9_control_valid(ctrl));
    std::vector<int> ctrlN;
    if (all_lengths_equal(ctrl, &ctrlN)) checks.note("LACE=0 control: N=" + std::to_string(ctrlN[0]) + " on all 8 fields");

    g_P4a(a, p4_gate(p));
    g_P4b(a, p4_gate(p));
    g_P4c(a, p4_gate(p));
    checks.record("P4c: ch2-using-the-copper-in-interlaced-mode.c8, .c9, COP2-using-the-copper-in-interlaced-mode.c8 "
        "and .c9 are not graded here -- their says is about what a scan displays on screen, and P4c grades "
        "only fetch addresses");
    g_P4d(a, p4_gate(p));
    g_P4e(a, p4_gate(p));
    checks.record("P4e: ch2-using-the-copper-in-interlaced-mode.c11 (Figure 2-1's odd field is the field at "
        "time t) is not graded -- P4e accepts either list as the first field and ties no field to the odd "
        "field or to time t; ch2-using-the-copper-in-interlaced-mode.c12 is not graded either, its 16.6ms "
        "figure is declined (section 1) and the id bundles that figure with the ordering claim P4e does grade");

    g_P5(a, p5_gate(p));
    g_P6(a, p6_gate(p));

    // The no-rewrite runs (I-c, I-e), run here (not as mutations) so that the
    // "must be rewritten" half of c3 and c4 gets a graded line of its own,
    // each behind its own fixture precondition.
    {
        IAnalysis c5 = analyze_i(dir, "prog_i_c");
        g_PI10(c5);
        g_P5_NOREWRITE(a, c5, p5_gate(p) && pi10_ctrl_valid(c5));
        std::vector<BplGroup> firsts;
        if (p5_first_groups(c5, firsts)) {
            std::string m;
            for (size_t k = 0; k < firsts.size(); ++k) m += (k ? "," : "") + std::to_string(firsts[k].addr * 2);
            checks.note("P5-NOREWRITE: first 20-word fetch address of fields 0.." + std::to_string(kI_K - 1) +
                        " of the no-rewrite run = " + m + " (PIC = " + std::to_string(kI_PIC) + ")");
        }
    }
    {
        IAnalysis c6 = analyze_i(dir, "prog_i_e");
        g_PI11(c6);
        g_P6_NOREWRITE(a, c6, p6_gate(p) && pi11_ctrl_valid(c6));
        if (c6.matched) {
            std::string m;
            for (size_t k = 0; k < c6.fields.size(); ++k)
                m += (k ? "," : "") + std::to_string(sprite_words_fetched_in(c6, c6.fields[k]));
            checks.note("P6-NOREWRITE: sprite data words (of 4) fetched in fields 0.." + std::to_string(kI_K - 1) +
                        " of the no-rewrite run = " + m);
        }
    }
}

// ---- Program I mutation runs (one variant program per invocation) ------

// The controls every label of the P4 family prints: its preconditions and P4e.
void p4_controls(const std::string& label, const IPre& p) {
    ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2); ctl(label, "PI-3", p.pi3); ctl(label, "PI-4", p.pi4);
    ctl(label, "PI-5", p.pi5); ctl(label, "PI-6", p.pi6);
}

// Returns false if `label` names no Program-I mutation.
bool run_i_mutation(const std::string& dir, const std::string& label) {
    static const std::map<std::string, std::string> kProg = {
        {"P1", "prog_i_a"}, {"PI-3", "prog_i_a"},
        {"P1-NOLACE", "prog_i_a"}, {"P1-NOLACE(ctrl)", "prog_i"}, {"PI-9", "prog_i_o"},
        {"P4a", "prog_i_b"},
        {"P4b", "prog_i_b"}, {"P4c", "prog_i_b"}, {"P4d", "prog_i_b"}, {"P4d(step)", "prog_i_p"},
        {"P4e", "prog_i_d"},
        {"P5", "prog_i_c"}, {"P5-NOREWRITE", "prog_i_c"}, {"P5-NOREWRITE(ctrl)", "prog_i"},
        {"P6-NOREWRITE", "prog_i_e"}, {"P6-NOREWRITE(ctrl)", "prog_i"},
        {"PI-10", "prog_i_q"}, {"PI-11", "prog_i_r"},
        {"PI-7(bpl)", "prog_i_l"}, {"PI-7(spr)", "prog_i_m"},
        {"P6", "prog_i_e"},
        {"PI-1", "prog_i_j"}, {"PI-2", "prog_i_f"}, {"PI-4", "prog_i_h"},
        {"PI-5", "prog_i_g"}, {"PI-6", "prog_i_k"}, {"PI-8", "prog_i_i"},
        {"FIX-I", "prog_i_n"},
    };
    auto it = kProg.find(label);
    if (it == kProg.end()) return false;
    IAnalysis a = analyze_i(dir, it->second);
    IPre p = i_pre(a);
    if (label == "P1" || label == "PI-3") {
        ctl(label, "all 8 non-interlaced field lengths are equal", all_lengths_equal(a));
        ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2); ctl(label, "PI-4", p.pi4);
        if (label == "P1") g_P1(a, p1_gate(p));
        else {
            // The gate PI-3 closes P4: with LACE clear, the retrieval lines
            // of P4 are not graded (they would not depend on interlace).
            g_PI3(a);
            g_P4c(a, p4_gate(p));
        }
        std::vector<int> N;
        if (all_lengths_equal(a, &N)) checks.note(label + " mutation: N=" + std::to_string(N[0]) + " on all 8 fields");
    } else if (label == "P1-NOLACE" || label == "P1-NOLACE(ctrl)") {
        // The interlace run and the control run are both the variant program:
        // I-a as the interlace run (the first half of the conjunction fails,
        // the second half is the live control), I-0 as the control run (the
        // second half fails, the first half is the live control).
        const IAnalysis& ctrl = a;  // the same run serves as both
        bool pi9 = pi9_control_valid(ctrl);
        ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2); ctl(label, "PI-4", p.pi4);
        ctl(label, "PI-9", pi9);
        if (label == "P1-NOLACE") ctl(label, "the LACE-clear half", all_lengths_equal(ctrl));
        else ctl(label, "the interlace half", p1_alternation(a));
        g_P1_NOLACE(a, ctrl, p1_gate(p) && pi9);
    } else if (label == "PI-9") {
        g_PI9(a);
    } else if (label == "P4a") {
        ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2); ctl(label, "PI-3", p.pi3); ctl(label, "PI-4", p.pi4);
        ctl(label, "PI-5", p.pi5); ctl(label, "PI-6", p.pi6);
        ctl(label, "PI-7(bpl)", p.pi7b); ctl(label, "PI-7(spr)", p.pi7s); ctl(label, "PI-8", p.pi8);
        ctl(label, "P4e", p4e(a));
        g_P4a(a, p4_gate(p));
    } else if (label == "P4b") {
        p4_controls(label, p); ctl(label, "P4e", p4e(a));
        g_P4b(a, p4_gate(p));
    } else if (label == "P4c") {
        p4_controls(label, p); ctl(label, "P4e", p4e(a));
        g_P4c(a, p4_gate(p));
    } else if (label == "P4d" || label == "P4d(step)") {
        p4_controls(label, p); ctl(label, "P4e", p4e(a));
        g_P4d(a, p4_gate(p));
        P4dParts q = p4d_parts(a);
        checks.note(label + " mutation measured: lines 0..511 each exactly once = " + (q.set ? "yes" : "no") +
                    ", A-group i to B-group i rises by 40 = " + (q.ab ? "yes" : "no") +
                    ", B-group i to A-group i+1 rises by 40 = " + (q.ba ? "yes" : "no"));
    } else if (label == "P4e") {
        p4_controls(label, p);
        g_P4e(a, p4_gate(p));
    } else if (label == "PI-10") {
        g_PI10(a);
    } else if (label == "PI-11") {
        g_PI11(a);
    } else if (label == "P5-NOREWRITE" || label == "P5-NOREWRITE(ctrl)") {
        // One run serves as both: I-c as the rewrite run (the first half goes
        // red, PI-4, PI-6 and PI-7(bpl) do not apply as in the P5 run below,
        // and the second half is a live control), I-0 as the no-rewrite run
        // (the second half goes red, the first half is a live control).
        const IAnalysis& c = a;
        bool pi10 = pi10_ctrl_valid(c);
        ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2);
        ctl(label, "PI-10", pi10);
        bool gate;
        if (label == "P5-NOREWRITE") {
            ctl(label, "the no-rewrite half", p5_norewrite(c));
            checks.record("P5-NOREWRITE/PI-4, PI-6 and PI-7(bpl): not applicable in this run, the lists hold no BPL1PT MOVEs and the fetches lie outside PIC's window");
            gate = p.pi1 && p.pi2;
        } else {
            ctl(label, "PI-4", p.pi4); ctl(label, "PI-6", p.pi6); ctl(label, "PI-7(bpl)", p.pi7b);
            ctl(label, "the rewrite half", p5(a));
            gate = p5_gate(p);
        }
        g_P5_NOREWRITE(a, c, gate && pi10);
    } else if (label == "P6-NOREWRITE" || label == "P6-NOREWRITE(ctrl)") {
        const IAnalysis& c = a;
        bool pi11 = pi11_ctrl_valid(c);
        ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2); ctl(label, "PI-8", p.pi8);
        ctl(label, "PI-11", pi11);
        bool gate;
        if (label == "P6-NOREWRITE") {
            ctl(label, "the no-rewrite half", p6_norewrite(c));
            checks.record("P6-NOREWRITE/PI-7(spr): not applicable in this run, the lists hold no SPR0PT MOVEs");
            gate = p.pi1 && p.pi2 && p.pi8;
        } else {
            ctl(label, "PI-7(spr)", p.pi7s);
            ctl(label, "the rewrite half", p6(a));
            gate = p6_gate(p);
        }
        g_P6_NOREWRITE(a, c, gate && pi11);
    } else if (label == "P5") {
        // I-c: no BPL1PT rewrite, so there is no PI-7(bpl) to hold, and the
        // fields' fetches leave the PIC window PI-4 counts in. The other
        // preconditions hold. The first 20-word fetch of every field is found
        // wherever it lies, so the comparator goes red through an address
        // mismatch and not through an empty population.
        std::vector<BplGroup> firsts;
        bool haveAll = p5_first_groups(a, firsts);
        ctl(label, "the 20-word search populates every field", haveAll);
        ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2); ctl(label, "PI-6", p.pi6);
        checks.record("P5/PI-4 and P5/PI-7(bpl): not applicable in this run, the lists hold no BPL1PT MOVEs and the fetches lie outside PIC's window");
        g_P5(a, p.pi1 && p.pi2 && p.pi6);
        if (haveAll && firsts.size() >= 2) {
            // p61: the next field starts at the last fetch of the previous
            // field + 2 + modulo (BPL1MOD = 40). Computed from field 0's last
            // 20-word group, recorded, not graded.
            auto groups0 = group_reads_in(a.r, kI_PIC, nmh::kChipIdentityHi, a.fields[0].begin, a.fields[0].end);
            const BplGroup* last = nullptr;
            for (const auto& g : groups0) if (g.len == 20) last = &g;
            if (last) {
                uint32_t lastFetch = (last->addr + 20u - 1u) * 2u;
                uint32_t predicted = lastFetch + 2u + 40u;
                uint32_t measured = firsts[1].addr * 2;
                checks.note(label + " mutation measured: p61's predicted first address of field 1 = " +
                            std::to_string(predicted) + ", field 1's first fetch = " + std::to_string(measured));
            }
        }
    } else if (label == "P6") {
        // I-e: no SPR0PT rewrite, so there is no PI-7(spr) to hold. PI-8
        // shows the sprite structure was fetched earlier in the same run.
        ctl(label, "PI-1", p.pi1); ctl(label, "PI-2", p.pi2); ctl(label, "PI-8", p.pi8);
        checks.record("P6/PI-7(spr): not applicable in this run, the lists hold no SPR0PT MOVEs");
        g_P6(a, p.pi1 && p.pi2 && p.pi8);
    } else if (label == "PI-1") {
        g_PI1(a);
        if (a.m1[0].present) checks.note(label + " mutation measured: M1 record 0's first V = " + std::to_string(a.m1[0].firstV));
    } else if (label == "PI-2") {
        g_PI2(a);
    } else if (label == "PI-4") {
        g_PI4(a);
    } else if (label == "PI-5") {
        g_PI5(a);
    } else if (label == "PI-6") {
        ctl(label, "PI-1", p.pi1);
        g_PI6(a);
    } else if (label == "PI-7(bpl)") {
        g_PI7bpl(a);
        if (a.matched && !a.fields.empty()) {
            const auto& f = a.fields[0];
            uint32_t base = (f.list == 'A') ? kI_LISTA : kI_LISTB;
            uint32_t off = list_move_offset(a.r, base, kBPL1PTH);
            uint64_t bplhCycle = off == UINT32_MAX ? UINT64_MAX : first_read_cycle(a.r, (base + off) / 2, f.begin, f.end);
            if (bplhCycle != UINT64_MAX && !f.bpl.empty()) {
                int64_t sinceRestartH = ((int64_t)bplhCycle - (int64_t)f.begin) / (int64_t)kClkSysPerColourClock;
                int64_t firstBplSinceRestart = ((int64_t)f.bpl.front().cycle - (int64_t)f.begin) / (int64_t)kClkSysPerColourClock;
                checks.note(label + " mutation field 0 measured: BPL1PTH fetch at " + std::to_string(sinceRestartH) +
                            " colour clocks after the restart, field's first bitplane fetch at " +
                            std::to_string(firstBplSinceRestart) + " colour clocks after the restart");
            }
        }
    } else if (label == "PI-7(spr)") {
        g_PI7spr(a);
    } else if (label == "PI-8") {
        g_PI8(a);
    } else if (label == "FIX-I") {
        g_FIX_I(a);
    }
    return true;
}

//=========================================================================
// Program T: P2, P3 and their preconditions PT-1..PT-7
//=========================================================================

nmh::Config t_config(bool ntsc) {
    nmh::Config c;
    c.chipset_config = ntsc ? 0x0A : 0x08; // ECS + (bit1: NTSC=1/PAL=0)
    return c;
}

// Address of pair p's MOVE-opcode word. Pair 0 is followed by the one-time
// BPL1PTH/BPL1PTL MOVE pair (2 words each, 8 bytes total, listt_body.i),
// which shifts every later pair by a fixed +8 bytes.
uint32_t listt_move_addr(int p) {
    uint32_t off = (p == 0) ? 0u : (8u * (uint32_t)p + 8u);
    return kT_LISTT + off + 4;
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

bool fix_t(const TAnalysis& a) {
    return ok(a.r) && W(a.r, kT_FIXMARK) && *W(a.r, kT_FIXMARK) == 0xBEEF &&
           !write_cycles_of(a.r, kT_FIXMARK, 0xBEEF).empty() &&
           write_cycles_of(a.r, kT_FIXMARK, 0xBEEF).front() < a.startCycle;
}

// The Copper starts running LISTT as soon as DMACON sets COPEN (CPU
// sequence step 5), well before M1 even starts sampling (step 7), so the
// list restarts several times before M1's own field 0 opens. The k-th
// *global* occurrence of a given address is therefore not field k's own
// mark: this looks it up within field k's own restart interval instead
// (a.fields[k].begin/.end, from M2).
std::optional<uint64_t> copper_mark(const TAnalysis& a, int k, int v) {
    if (k < 0 || k >= (int)a.fields.size()) return std::nullopt;
    uint32_t addr = listt_move_addr(v) / 2;
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

// Delta = m_c(k,v) - m_b(k,v) for v = 44..254; returns true and *delta if a
// single value holds everywhere it is defined.
bool pt3_delta(const TAnalysis& a, int64_t* delta_out = nullptr) {
    if (!a.matched) return false;
    bool have = false; int64_t delta = 0;
    for (int k = 0; k < kT_K; ++k) {
        const auto& f = a.fields[(size_t)k];
        if (f.N < 0) return false;
        for (int v = 44; v <= 254 && v <= f.N - 1; ++v) {
            auto mc = copper_mark(a, k, v);
            int i = v - 44;
            if (!mc || i >= (int)f.bpl.size()) return false;
            int64_t d = (int64_t)*mc - (int64_t)f.bpl[(size_t)i].cycle;
            if (!have) { delta = d; have = true; } else if (d != delta) return false;
        }
    }
    if (have && delta_out) *delta_out = delta;
    return have;
}

// Gate of PT-5 and P3: in every field the Copper mark of line 1 follows the
// mark of line 0 by 227 or 228 colour clocks, so the two marks lie on
// consecutive lines and the mark of line 0 is anchored on line 0.
bool pt4_restart_wait(const TAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k < kT_K; ++k) {
        auto mc0 = copper_mark(a, k, 0);
        auto mc1 = copper_mark(a, k, 1);
        if (!mc0 || !mc1) return false;
        int64_t line01 = (int64_t)*mc1 - (int64_t)*mc0;
        if (line01 != 227 * (int64_t)kClkSysPerColourClock && line01 != 228 * (int64_t)kClkSysPerColourClock) return false;
    }
    return true;
}

// line lengths, in raw clk_sys (not colour clocks: dividing before
// comparing against 227/228 would let a value anywhere in an 8-wide band
// pass as either), for v = 0..N_k-1 of field k, using the formulas of
// section 2.7. Needs field k+1's v=0 Copper mark for the last line, so
// only k = 0..K-2 are computable this way. Returns false if a mark or a
// bitplane group the formulas need is missing.
bool line_lengths(const TAnalysis& a, int k, int64_t delta, std::vector<int64_t>& out) {
    const auto& f = a.fields[(size_t)k];
    if (f.N < 0) return false;
    out.clear();
    for (int v = 0; v <= 253 && v + 1 <= f.N - 1; ++v) {
        auto c0 = copper_mark(a, k, v), c1 = copper_mark(a, k, v + 1);
        if (!c0 || !c1) return false;
        out.push_back((int64_t)*c1 - (int64_t)*c0);
    }
    for (int v = 254; v + 1 <= f.N - 1; ++v) {
        int i0 = v - 44, i1 = v + 1 - 44;
        if (i1 >= (int)f.bpl.size()) return false;
        out.push_back((int64_t)f.bpl[(size_t)i1].cycle - (int64_t)f.bpl[(size_t)i0].cycle);
    }
    // last line: v = N_k - 1
    if (k + 1 >= kT_K) return false; // needs the next field
    auto c_next0 = copper_mark(a, k + 1, 0);
    int iLast = (f.N - 1) - 44;
    if (!c_next0 || iLast < 0 || iLast >= (int)f.bpl.size()) return false;
    int64_t lastLen = (int64_t)*c_next0 - (int64_t)f.bpl[(size_t)iLast].cycle - delta;
    out.push_back(lastLen);
    return true;
}

bool length_in_range(int64_t L) {
    return L == 227 * (int64_t)kClkSysPerColourClock || L == 228 * (int64_t)kClkSysPerColourClock;
}

// Every measured line length is 227 or 228 colour clocks. Delta is the one
// value PT-3 measured; the caller grades this only when PT-3 holds.
bool pt5_line_lengths(const TAnalysis& a, int64_t delta) {
    for (int k = 0; k + 1 < kT_K; ++k) {
        std::vector<int64_t> lens;
        if (!line_lengths(a, k, delta, lens)) return false;
        for (int64_t L : lens) if (!length_in_range(L)) return false;
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

// The four-field pattern at one phase over fields 0..11; the caller grades
// this only when PT-3 holds (delta) and the gate PT-6 is open.
bool p3(const TAnalysis& a, int64_t delta, int* phase_out = nullptr) {
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
            if (isLong != Qlong[slot] || xk != (int64_t)Qlen[slot] * (int64_t)kClkSysPerColourClock) allMatch = false;
        }
        if (allMatch) { if (phase_out) *phase_out = phase; return true; }
    }
    return false;
}

// The preconditions of Program T on one run.
struct TPre { bool pt1, pt2, pt3, pt4, pt5, pt6, pt7; int64_t delta; };
TPre t_pre(const TAnalysis& a) {
    TPre p{};
    p.pt1 = pt1_valid(a);
    p.pt2 = pt2_groups(a);
    p.pt3 = pt3_delta(a, &p.delta);
    p.pt4 = pt4_restart_wait(a);
    p.pt5 = p.pt3 && p.pt4 && pt5_line_lengths(a, p.delta);
    p.pt6 = pt6_gate(a);
    p.pt7 = pt7_anchor(a);
    return p;
}

// ---- Program T assertions: one function per label, used by the normal
// run and by that label's mutation run alike ----------------------------

void g_FIX_T(const TAnalysis& a) {
    checks.expect(fix_t(a),
                  "FIX-T: Program T stores and delivers a known constant (BEEF) before M1 records anything");
}
void g_PT1(const TAnalysis& a) {
    checks.expect(pt1_valid(a),
        "PT-1: Program T's fixture -- the completion word was written and all 13 M1 records are valid");
}
void g_PT2(const TAnalysis& a) {
    checks.expect(pt2_groups(a),
        "PT-2: in every field, the bitplane fetch groups equal N_k - 44 (20 consecutive ascending words "
        "each), independently confirming N_k from the bitplane fetches");
}
void g_PT3(const TAnalysis& a) {
    checks.expect(pt3_delta(a),
        "PT-3: the Copper-mark-to-bitplane-mark offset Delta has one single value on every line 44..254 "
        "of every field");
}
// PT-4 is a gate, not a graded line: p25 leaves open whether the restart lies
// at the start or at the end of vertical blanking, and an implementation that
// restarts at the end of blanking satisfies the (0,$E0) WAIT at once. The gate
// state is printed; PT-5 and P3 are NOT GRADED when it is closed.
void g_PT4(const TAnalysis& a) {
    bool open = pt4_restart_wait(a);
    int64_t lo = INT64_MAX, hi = INT64_MIN;
    int have = 0;
    if (a.matched) {
        for (int k = 0; k < kT_K; ++k) {
            auto mc0 = copper_mark(a, k, 0);
            auto mc1 = copper_mark(a, k, 1);
            if (!mc0 || !mc1) continue;
            int64_t d = (int64_t)*mc1 - (int64_t)*mc0;
            lo = std::min(lo, d); hi = std::max(hi, d); ++have;
        }
    }
    std::string m = std::string("PT-4 gate of PT-5 and P3 (in every field the Copper marks of line 0 and line 1 "
                                "are 227 or 228 colour clocks apart): ") + (open ? "open" : "closed") +
                    "; line 0-to-1 measured in " + std::to_string(have) + " of " + std::to_string(kT_K) + " fields";
    if (have) m += ", " + std::to_string(lo) + " to " + std::to_string(hi) + " clk_sys";
    checks.note(m);
}
void g_PT5(const TAnalysis& a, const TPre& t) {
    if (!(t.pt3 && t.pt4)) { checks.record("PT-5: NOT GRADED, a precondition (PT-3, the offset Delta the last-line length needs, or PT-4, the gate that anchors the line-0 marks) did not hold on this run"); return; }
    checks.expect(pt5_line_lengths(a, t.delta),
        "PT-5: every measured line length (section 2.7) is 227 or 228 colour clocks");
}
void g_PT7(const TAnalysis& a) {
    checks.expect(pt7_anchor(a),
        "PT-7: between the first and last recorded wrap there are exactly 13 reads of LISTT's first word "
        "(located by address; whether each is the first Copper fetch after $FFFF,$FFFE, as section 2.6 "
        "defines a restart, is not itself checked)");
}
void g_P2(const TAnalysis& a, bool pre) {
    if (!pre) { checks.record("P2: NOT GRADED, a precondition (PT-1, PT-2 or PT-7) did not hold on this run"); return; }
    checks.expect(p2(a),
        "P2 p24,p34: every NTSC interlaced field has 262 or 263 lines, and no two consecutive fields are "
        "both 263");
}
// PT-6 is a gate, not a graded line: p34 reads the NTSC fields as all 262
// lines, and an implementation that follows p34 closes the gate. P3 reports
// "not graded, gate closed" then; that is not a failure.
void g_PT6(const TAnalysis& a) {
    bool open = pt6_gate(a);
    std::string lens;
    for (int k = 0; k < kT_K; ++k) lens += (k ? "," : "") + (a.fields[(size_t)k].N < 0 ? std::string("?") : std::to_string(a.fields[(size_t)k].N));
    checks.note(std::string("PT-6 gate of P3 (consecutive field lengths alternate between 262 and 263): ") +
                (open ? "open" : "closed") + "; N_0..N_" + std::to_string(kT_K - 1) + " = " + lens);
}
void g_P3(const TAnalysis& a, const TPre& t) {
    if (!t.pt6) { checks.record("P3: not graded, gate closed (PT-6 did not hold on this run)"); return; }
    if (!(t.pt1 && t.pt2 && t.pt3 && t.pt4 && t.pt5 && t.pt7)) {
        checks.record("P3: NOT GRADED, a precondition (PT-1 to PT-5 or PT-7) did not hold on this run");
        return;
    }
    int phase = -1;
    bool p3ok = p3(a, t.delta, &phase);
    checks.expect(p3ok,
        "P3 p24 ch2-vertical-beam-position.c15 ch2-vertical-beam-position.c16 "
        "ch2-vertical-beam-position.c17 ch2-vertical-beam-position.c18 ch2-vertical-beam-position.c19 "
        "COP2-vertical-beam-position.c17 COP2-vertical-beam-position.c18 COP2-vertical-beam-position.c19 "
        "COP2-vertical-beam-position.c20 COP2-vertical-beam-position.c21 COP2-vertical-beam-position.c22: "
        "the NTSC 4-field pattern (short/short-line, long/long-line, short/long-line, long/short-line) "
        "repeats for 3 whole cycles at one fixed phase");
    if (p3ok) checks.note("P3 phase = " + std::to_string(phase));
}

void run_program_t_assertions(const std::string& dir) {
    TAnalysis a = analyze_t(dir, "prog_t", /*ntsc=*/true);
    TPre t = t_pre(a);

    g_FIX_T(a);
    g_PT1(a);
    g_PT2(a);
    g_PT3(a);
    g_PT4(a);
    g_PT5(a, t);
    g_PT7(a);
    g_P2(a, t.pt1 && t.pt2 && t.pt7);
    checks.record("P2: COP2-vertical-beam-position.c13 (\"the first field length ... is 262\"), "
        "COP2-vertical-beam-position.c14 (\"the next field length ... is 263\"), "
        "COP2-using-the-copper-in-interlaced-mode.c3 (\"an interlaced NTSC display has 524 lines\") and "
        ".c7 (\"262 lines at a time\") are not graded -- P2 allows 262 or 263 but requires neither an "
        "order, an alternation, nor a specific total, and the measurement (262 and 263 both occur) "
        "contradicts .c3 and .c7");
    g_PT6(a);
    g_P3(a, t);
}

struct TMutSpec { std::string prog; bool ntsc; };

// Returns false if `label` names no Program-T mutation.
bool run_t_mutation(const std::string& dir, const std::string& label) {
    static const std::map<std::string, TMutSpec> kProg = {
        {"P2", {"prog_t", false}},
        {"P3", {"prog_t", false}},
        {"PT-1", {"prog_t_f", true}},
        {"PT-2", {"prog_t_e", true}},
        {"PT-3", {"prog_t_d", true}},
        {"PT-4", {"prog_t_c", true}},
        {"PT-5", {"prog_t_g", true}},
        {"PT-6", {"prog_t_a", true}},
        {"PT-7", {"prog_t_h", true}},
        {"FIX-T", {"prog_t_i", true}},
    };
    auto it = kProg.find(label);
    if (it == kProg.end()) return false;
    TAnalysis a = analyze_t(dir, it->second.prog, it->second.ntsc);
    TPre t = t_pre(a);
    if (label == "P2") {
        ctl(label, "PT-1", t.pt1); ctl(label, "PT-2", t.pt2); ctl(label, "PT-7", t.pt7);
        g_P2(a, t.pt1 && t.pt2 && t.pt7);
    } else if (label == "P3") {
        // P3 is already red in the normal run. The comparator is shown with
        // every precondition live, in PAL, where p23 gives every line as 227
        // colour clocks and no field has 263 lines.
        ctl(label, "PT-1", t.pt1); ctl(label, "PT-2", t.pt2); ctl(label, "PT-3", t.pt3);
        ctl(label, "PT-4", t.pt4); ctl(label, "PT-5", t.pt5); ctl(label, "PT-7", t.pt7);
        ctl(label, "PT-6 (the gate; PAL fields alternate 312, 313)", t.pt6);
        g_P3(a, t);
        bool anyLongField = false;
        for (const auto& f : a.fields) if (f.N == 263) anyLongField = true;
        checks.note(label + " mutation measured: any field with N=263? " + (anyLongField ? "yes" : "no"));
        checks.record(label + " mutation: no input built in this package turns P3's own comparator green "
            "-- unfalsifiable at this boundary while NTSC never produces a long line and PAL never "
            "produces a 263-line field");
    } else if (label == "PT-1") {
        g_PT1(a);
        if (a.m1[0].present) checks.note(label + " mutation measured: M1 record 0's first V = " + std::to_string(a.m1[0].firstV));
    } else if (label == "PT-2") {
        g_PT2(a);
    } else if (label == "PT-3") {
        g_PT3(a);
    } else if (label == "PT-4") {
        // T-c: the gate closes, so PT-5 and P3 are NOT GRADED.
        ctl(label, "PT-1", t.pt1); ctl(label, "PT-3", t.pt3);
        g_PT4(a);
        ctl(label, "the gate PT-4 is closed", !t.pt4);
        g_PT5(a, t);
        g_P3(a, t);
    } else if (label == "PT-5") {
        // T-g: line 10's WAIT uses H=$40, so every Copper mark stays live and
        // the length of line 9 and of line 10 change. Line 10 lies below line
        // 44, so Delta (lines 44..254) is not touched.
        bool allMarks = t.pt3;
        size_t outside = 0, measured = 0;
        std::string first;
        for (int k = 0; k + 1 < kT_K && t.pt3; ++k) {
            std::vector<int64_t> lens;
            if (!line_lengths(a, k, t.delta, lens)) { allMarks = false; continue; }
            for (size_t v = 0; v < lens.size(); ++v) {
                ++measured;
                if (!length_in_range(lens[v])) {
                    ++outside;
                    if (first.empty())
                        first = "field " + std::to_string(k) + " line " + std::to_string(v) + " = " +
                                std::to_string(lens[v]) + " clk_sys (" + std::to_string(lens[v] / (int64_t)kClkSysPerColourClock) +
                                " colour clocks)";
                }
            }
        }
        ctl(label, "PT-1", t.pt1); ctl(label, "PT-3", t.pt3); ctl(label, "PT-4", t.pt4);
        ctl(label, "every mark the line lengths need is present", allMarks);
        g_PT5(a, t);
        checks.note(label + " mutation measured: " + std::to_string(outside) + " of " + std::to_string(measured) +
                    " line lengths outside 227 and 228 colour clocks; first: " + (first.empty() ? "none" : first));
    } else if (label == "PT-6") {
        // T-a: the gate closes, so P3 reports "not graded, gate closed".
        ctl(label, "PT-1", t.pt1);
        g_PT6(a);
        ctl(label, "the gate PT-6 is closed", !t.pt6);
        g_P3(a, t);
    } else if (label == "PT-7") {
        g_PT7(a);
    } else if (label == "FIX-T") {
        g_FIX_T(a);
    }
    return true;
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
        if (!run_i_mutation(dir, label) && !run_t_mutation(dir, label))
            checks.record("NMH_MUTATE=" + label + " names no mutation of this package");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
