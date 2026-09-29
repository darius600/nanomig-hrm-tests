// test_ch2_copper_beam.cpp -- ch2-copper-beam. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to (RF1-RF3, P1, P2, P7, P8; P3-P6 and P9 are not built in
// this pass -- see the report to the lead).
//
// NMH_MUTATE selects a mutated program or a re-grading of an existing
// capture in place of the normal one, one assertion or precondition at a
// time; none is exercised by a plain `make test`.
//
// MUTATION P1-COUNT-PAL stimulus red the PAL check regraded against the NTSC capture, via NMH_MUTATE=P1-STD-PAL
// MUTATION P1-COUNT-NTSC stimulus red the NTSC check regraded against the PAL capture, via NMH_MUTATE=P1-STD-NTSC
// MUTATION P1-COUNT-PAL stimulus red A-LACE (PAL, LACE set): a field with 313 distinct V values appears, via NMH_MUTATE=P1-LACE
// MUTATION T1 stimulus red A-slow (a delay of at least 300 slots between trace samples), via NMH_MUTATE=P1-T1
// MUTATION T2 stimulus red A-hold (one read of $DFF004 held and stored every step), via NMH_MUTATE=P1-T2
// MUTATION R1 stimulus red A-noCop (DMACON=$0080 written after the third field boundary), via NMH_MUTATE=P1-R1
// MUTATION completion-word stimulus red A-nodone (the program halts before step 6/7), via NMH_MUTATE=P1-DONE
// MUTATION P2-PAL stimulus GREEN (not red -- see the note below) the PAL check regraded against the NTSC capture, via NMH_MUTATE=P2-STD-PAL
// MUTATION P2-NTSC stimulus red the NTSC check regraded against the PAL capture, via NMH_MUTATE=P2-STD-NTSC
// MUTATION P2-PAL stimulus red A-shift (WAIT($80,$42) in place of WAIT($80,$40)): L($7F) and L($80) shift by 2, via NMH_MUTATE=P2-SHIFT
// MUTATION P2-waited stimulus red an extra, already-satisfied WAIT($7F,$40) inserted after MARK($7F): its own delay equals lambda_min, via NMH_MUTATE=P2-WAITED
// MUTATION A-anchor stimulus red A-anchor (every WAIT(v,$40) becomes WAIT(v+1,$40)): m_v lies on line v+1, via NMH_MUTATE=ANCHOR
// MUTATION P7 stimulus red A-noprep (PAL): WAIT($00,$60) is fetched on line 255 and ends at once, via NMH_MUTATE=P7-NOPREP-PAL
// MUTATION P7 stimulus red A-noprep (NTSC): WAIT($00,$60) is fetched on line 255 and ends at once, via NMH_MUTATE=P7-NOPREP-NTSC
//
// P2-STD-PAL does not redden P2-PAL: measured, both cores, the NTSC
// capture's own L(v) values are also all 227 (P2-NTSC's own finding, same
// capture), so grading them against the PAL expectation stays green.
// P2-PAL's own red mutation is P2-SHIFT, which does redden it (measured,
// both cores).
//
// P8 (NTSC-only) is graded against the measured field length directly (no
// dedicated mutation beyond the shared T1/T2/R1/anchor ones above): its
// main clauses are already red on the normal run (see the report to the
// lead), so no contrasting case can show a different, "proven-live" input
// through the same comparator going green at this boundary; this is
// recorded in its own precondition note.

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

// ---- the fixed assertion call form (BRIEF.md) --------------------------
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

namespace {

// ---- memory layout (SPECIFICATION.md 3.6) ------------------------------
constexpr uint32_t kListBase    = 0x010000;
constexpr uint32_t kListBaseW   = kListBase / 2;
constexpr uint32_t kTraceBase   = 0x100000;
constexpr uint32_t kTraceBaseW  = kTraceBase / 2;
constexpr uint32_t kTraceMaxW   = 0x080000; // trace region is $100000-$1FFFFF
constexpr uint32_t kDoneAddr    = 0x0C0FFE;
constexpr uint16_t kDoneValue   = 0x600D;

constexpr uint8_t kECS_PAL  = 0x08;
constexpr uint8_t kECS_NTSC = 0x0A;
constexpr uint8_t kOCS_PAL  = 0x00;
constexpr uint8_t kOCS_NTSC = 0x02;

constexpr uint64_t kBootBudget   = 6'000'000;
constexpr uint64_t kRunBudget    = 20'000'000;
constexpr uint64_t kNoDoneBudget = 3'000'000;

// ---- capture ------------------------------------------------------------
struct Capture {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> raw;  // for S1/refresh (see measure_slots)
    std::vector<nmh::RamAccess> log;  // coalesced, for everything else
};

Capture run_capture(const std::string& bin, uint8_t chipset_config, uint64_t run_budget) {
    Capture c;
    nmh::Config cfg;
    cfg.chipset_config = chipset_config;
    nmh::NanomigHarness h(cfg);
    size_t loaded = h.load_program(bin);
    c.loaded = loaded > 0;
    if (!c.loaded) return c;
    auto boot = h.boot(kBootBudget);
    c.released = boot.cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.raw = h.ram_log();
    c.log = nmh::coalesce(c.raw);
    return c;
}

// ---- RF1: memory slots (SPECIFICATION.md 3.2) ---------------------------
//
// Refresh reads are RAM-port reads at word address 0 (Figure 6-9: four per
// line, each two slots after the one before). They are identified from the
// RAW (uncoalesced) log: coalesce() merges consecutive same-address,
// same-data entries regardless of how many real cycles separate them, and
// two genuinely distinct refresh reads at address 0 (always data 0000)
// with nothing else logged between them would otherwise be merged into
// one, silently understating the count and corrupting the spacing
// measurement -- observed directly while calibrating this package: the
// coalesced log showed refresh groups of 3 with double-width gaps where
// the raw log showed clean groups of 4.
struct SlotRef {
    bool ok = false;
    uint64_t c_R = 0;
    int64_t S = 0;
    bool spacing_consistent = true;
    size_t groups_found = 0;
};

int64_t floordiv(int64_t a, int64_t b) {
    int64_t q = a / b, r = a % b;
    if (r != 0 && ((r < 0) != (b < 0))) --q;
    return q;
}

SlotRef measure_slots(const std::vector<nmh::RamAccess>& raw) {
    SlotRef r;
    std::vector<uint64_t> reads;
    for (auto& a : raw) if (!a.is_write && a.word_addr == 0) reads.push_back(a.cycle);
    if (reads.size() < 8) return r;
    std::vector<std::vector<uint64_t>> groups;
    groups.push_back({reads[0]});
    for (size_t i = 1; i < reads.size(); ++i) {
        if (reads[i] - reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(reads[i]);
    }
    std::vector<int64_t> spacings;
    std::vector<uint64_t> firsts;
    for (auto& g : groups) {
        if (g.size() != 4) continue;
        int64_t sp = (int64_t)g[1] - (int64_t)g[0];
        bool ok = sp > 0;
        for (size_t i = 1; ok && i < 4; ++i)
            if ((int64_t)g[i] - (int64_t)g[i - 1] != sp) ok = false;
        if (!ok) continue;
        spacings.push_back(sp);
        firsts.push_back(g[0]);
    }
    r.groups_found = spacings.size();
    if (spacings.empty()) return r;
    int64_t sp0 = spacings[0];
    for (auto sp : spacings) if (sp != sp0) r.spacing_consistent = false;
    r.S = sp0 / 2;
    r.c_R = firsts[0];
    r.ok = r.S > 0;
    return r;
}

int64_t slot_of(const SlotRef& ref, uint64_t cycle) { return floordiv((int64_t)cycle - (int64_t)ref.c_R, ref.S); }

// S1's second clause: no two logged accesses (any master), merged per bus
// cycle, occupy the same slot. Checked over consecutive entries of the
// (coalesced) log: slot() is non-decreasing in cycle, so a same-slot pair
// anywhere in the log implies a same-slot ADJACENT pair too.
bool s1_no_collision(const SlotRef& ref, const std::vector<nmh::RamAccess>& log, size_t* violations) {
    *violations = 0;
    int64_t prev = INT64_MIN;
    bool ok = true;
    for (auto& a : log) {
        int64_t s = slot_of(ref, a.cycle);
        if (s == prev) { ok = false; ++*violations; }
        prev = s;
    }
    return ok;
}

// ---- RF2: beam trace ------------------------------------------------------
struct Sample { uint64_t cyc; int64_t w; int V, H; bool trusted; };

std::vector<Sample> decode_trace(const SlotRef& ref, const std::vector<nmh::RamAccess>& log) {
    std::vector<const nmh::RamAccess*> ws;
    for (auto& a : log)
        if (a.is_write && a.word_addr >= kTraceBaseW && a.word_addr < kTraceBaseW + kTraceMaxW) ws.push_back(&a);
    std::vector<Sample> out;
    out.reserve(ws.size() / 2);
    for (size_t i = 0; i + 1 < ws.size(); i += 2) {
        uint16_t hi = ws[i]->data, lo = ws[i + 1]->data;
        Sample s;
        s.cyc = std::max(ws[i]->cycle, ws[i + 1]->cycle);
        s.w = ref.ok ? slot_of(ref, s.cyc) : 0;
        int V8 = hi & 1;
        s.V = V8 * 256 + ((lo >> 8) & 0xFF);
        s.H = lo & 0xFF;
        s.trusted = s.H >= 16;
        out.push_back(s);
    }
    return out;
}

std::vector<Sample> trusted_of(const std::vector<Sample>& s) {
    std::vector<Sample> t;
    for (auto& x : s) if (x.trusted) t.push_back(x);
    return t;
}

struct RF2Precond { bool t1 = true; size_t t1_violations = 0, t1_total = 0; bool t2 = true; size_t t2_violations = 0, t2_total = 0; };

RF2Precond check_rf2(const std::vector<Sample>& all, const std::vector<Sample>& trusted) {
    RF2Precond r;
    for (size_t i = 0; i + 1 < all.size(); ++i) {
        ++r.t1_total;
        if (all[i + 1].w - all[i].w > 16) { r.t1 = false; ++r.t1_violations; }
    }
    for (size_t i = 0; i + 1 < trusted.size(); ++i) {
        ++r.t2_total;
        int V0 = trusted[i].V, V1 = trusted[i + 1].V, H0 = trusted[i].H, H1 = trusted[i + 1].H;
        bool ok = (V0 == V1) ? (H1 > H0) : (V1 == V0 + 1 || V1 == 0);
        if (!ok) { r.t2 = false; ++r.t2_violations; }
    }
    return r;
}

// "Line of an event" (SPECIFICATION.md RF2): trusted must be sorted by w
// (it is: samples are taken in time order and w is non-decreasing).
std::optional<int> line_of_event(const std::vector<Sample>& trusted, int64_t t) {
    auto it = std::upper_bound(trusted.begin(), trusted.end(), t,
        [](int64_t val, const Sample& s) { return val < s.w; });
    if (it == trusted.begin()) return std::nullopt;
    const Sample& sminus = *(it - 1);
    auto it2 = std::lower_bound(trusted.begin(), trusted.end(), t + 16,
        [](const Sample& s, int64_t val) { return s.w < val; });
    if (it2 == trusted.end()) return std::nullopt;
    const Sample& splus = *it2;
    if (sminus.V == splus.V) return sminus.V;
    return std::nullopt;
}

// ---- RF3: fields and Copper restarts -------------------------------------
std::vector<size_t> trace_wraps(const std::vector<Sample>& trusted) {
    std::vector<size_t> w;
    for (size_t i = 1; i < trusted.size(); ++i) if (trusted[i].V < trusted[i - 1].V) w.push_back(i);
    return w;
}

std::vector<uint64_t> restart_cycles(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> v;
    for (auto& a : log) if (!a.is_write && a.word_addr == kListBaseW) v.push_back(a.cycle);
    return v;
}

struct FieldSpan { size_t lo, hi; }; // trusted[] index range, half-open

// Complete fields begin after the first restart (restarts[1]; restarts[0]
// is the run's first fetch, not a restart).
std::vector<FieldSpan> complete_fields(const std::vector<Sample>& trusted, const std::vector<uint64_t>& restarts) {
    std::vector<FieldSpan> out;
    if (restarts.size() < 2) return out;
    uint64_t first_restart = restarts[1];
    auto wraps = trace_wraps(trusted);
    std::vector<size_t> kept;
    for (auto idx : wraps) if (trusted[idx].cyc >= first_restart) kept.push_back(idx);
    for (size_t k = 0; k + 1 < kept.size(); ++k) out.push_back({kept[k], kept[k + 1]});
    return out;
}

struct R1Check { bool ok = true; size_t violations = 0, spans = 0; };

R1Check check_r1(const std::vector<Sample>& trusted, const std::vector<uint64_t>& restarts) {
    R1Check r;
    auto wraps = trace_wraps(trusted);
    for (size_t k = 0; k + 1 < wraps.size(); ++k) {
        uint64_t lo = trusted[wraps[k]].cyc, hi = trusted[wraps[k + 1]].cyc;
        size_t cnt = 0;
        for (size_t ri = 1; ri < restarts.size(); ++ri) if (restarts[ri] >= lo && restarts[ri] < hi) ++cnt;
        ++r.spans;
        if (cnt != 1) { r.ok = false; ++r.violations; }
    }
    return r;
}

// ---- L1: the Copper's first fetches read what the CPU wrote -------------
bool check_l1(const std::vector<nmh::RamAccess>& log) {
    std::optional<uint64_t> w0, w1, r0, r0d, r1, r1d;
    for (auto& a : log) {
        if (a.is_write && a.word_addr == kListBaseW && !w0) w0 = a.data;
        if (a.is_write && a.word_addr == kListBaseW + 1 && !w1) w1 = a.data;
        if (!a.is_write && a.word_addr == kListBaseW && !r0) { r0 = a.cycle; r0d = a.data; }
        if (!a.is_write && a.word_addr == kListBaseW + 1 && !r1) { r1 = a.cycle; r1d = a.data; }
    }
    return w0 && w1 && r0d && r1d && *w0 == *r0d && *w1 == *r1d && *w0 == 0x0001 && *w1 == 0xFFFE;
}

// ---- Run A list layout (SPECIFICATION.md 5, Run A) -----------------------
struct ListAOffsets {
    int v_start = 0x30, v_end = 0xFF;
    bool has_prep = true;
    int tail_n = 0x37;

    int v_idx(int v) const { return v - v_start; }
    int vwait0(int v) const { return 4 + 4 * v_idx(v); }
    int vwait1(int v) const { return vwait0(v) + 1; }
    int vmark0(int v) const { return vwait0(v) + 2; }
    int vmark1(int v) const { return vwait0(v) + 3; }
    int after_vloop() const { return 4 + 4 * (v_end - v_start + 1); }
    int e2wait0() const { return after_vloop(); }
    int e2wait1() const { return after_vloop() + 1; }
    int e2mark0() const { return after_vloop() + 2; }
    int tailbase() const { return has_prep ? after_vloop() + 4 + 32 : after_vloop(); }
    int tailwait0(int n) const { return tailbase() + 4 * n; }
    int tailwait1(int n) const { return tailwait0(n) + 1; }
    int tailmark0(int n) const { return tailwait0(n) + 2; }
    int tailmark1(int n) const { return tailwait0(n) + 3; }

    uint32_t addr(int off) const { return kListBaseW + (uint32_t)off; }
};

using ReadIndex = std::unordered_map<uint32_t, std::vector<uint64_t>>;

ReadIndex build_read_index(const std::vector<nmh::RamAccess>& log) {
    ReadIndex idx;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.word_addr < kListBaseW || a.word_addr >= kListBaseW + 0x8000) continue;
        idx[a.word_addr].push_back(a.cycle);
    }
    return idx;
}

std::optional<uint64_t> fetch_in(const ReadIndex& idx, uint32_t addr, uint64_t lo, uint64_t hi) {
    auto it = idx.find(addr);
    if (it == idx.end()) return std::nullopt;
    auto lb = std::lower_bound(it->second.begin(), it->second.end(), lo);
    if (lb == it->second.end() || *lb >= hi) return std::nullopt;
    return *lb;
}

// Restart-bounded field windows for Copper-fetch lookups: field k (0-based)
// is [restarts[k+1], restarts[k+2]).
struct RWin { uint64_t lo, hi; };
std::vector<RWin> restart_windows(const std::vector<uint64_t>& restarts) {
    std::vector<RWin> out;
    for (size_t k = 0; k + 2 < restarts.size(); ++k) out.push_back({restarts[k + 1], restarts[k + 2]});
    return out;
}

// ---- a whole Run A analysis bundle, built once per capture ---------------
struct Analysis {
    Capture cap;
    SlotRef sref;
    std::vector<Sample> all, trusted;
    RF2Precond rf2;
    std::vector<uint64_t> restarts;
    std::vector<FieldSpan> fields;      // trace-wrap based, complete fields
    std::vector<RWin> rwins;            // restart based, complete fields
    R1Check r1;
    bool l1 = false;
    ReadIndex ridx;
    bool s1_spacing = false;
    bool s1_collision = false;
    size_t s1_violations = 0;
};

Analysis analyze(Capture&& cap) {
    Analysis a;
    a.cap = std::move(cap);
    a.sref = measure_slots(a.cap.raw);
    a.all = decode_trace(a.sref, a.cap.log);
    a.trusted = trusted_of(a.all);
    a.rf2 = check_rf2(a.all, a.trusted);
    a.restarts = restart_cycles(a.cap.log);
    a.fields = complete_fields(a.trusted, a.restarts);
    a.rwins = restart_windows(a.restarts);
    a.r1 = check_r1(a.trusted, a.restarts);
    a.l1 = check_l1(a.cap.log);
    a.ridx = build_read_index(a.cap.log);
    a.s1_spacing = a.sref.ok && a.sref.spacing_consistent;
    a.s1_collision = s1_no_collision(a.sref, a.cap.log, &a.s1_violations);
    return a;
}

// Preconditions common to every Run A capture used below. Prints its own
// PASS/FAIL lines; returns nothing (callers read the Analysis fields for
// finer-grained gating where a specific proposition needs it).
void report_common_preconditions(Checks& checks, const Analysis& a, const std::string& tag) {
    checks.expect(a.cap.loaded && a.cap.released, "FIXTURE " + tag + ": program loaded and CPU released [not graded -- apparatus check, no claim uid]");
    checks.expect(a.cap.done.has_value(), "PRE-DONE " + tag + ": the completion word was written");
    checks.expect(a.s1_spacing, "PRE-S1a " + tag + ": refresh spacing 2S is constant through the run (RF1)");
    checks.note("S1 " + tag + ": S=" + std::to_string(a.sref.S) + " clk_sys cycles/slot, " +
                std::to_string(a.sref.groups_found) + " refresh groups measured");
    checks.expect(a.s1_collision, "PRE-S1b " + tag + ": no two logged accesses occupy the same slot (RF1)");
    checks.expect(a.l1, "PRE-L1 " + tag + ": the Copper's first fetches read the words the CPU wrote in step 3");
    checks.expect(a.rf2.t1, "PRE-T1 " + tag + ": every consecutive sample pair is at most 16 slots apart (RF2)");
    checks.note("T1 " + tag + ": " + std::to_string(a.rf2.t1_violations) + " of " + std::to_string(a.rf2.t1_total) +
                " consecutive-sample gaps exceed 16 slots");
    checks.expect(a.rf2.t2, "PRE-T2 " + tag + ": the beam counter is live between trusted samples (RF2)");
    checks.note("T2 " + tag + ": " + std::to_string(a.rf2.t2_violations) + " of " + std::to_string(a.rf2.t2_total) +
                " trusted-sample transitions violate T2");
    checks.expect(a.r1.ok, "PRE-R1 " + tag + ": exactly one restart between consecutive field boundaries (RF3)");
    checks.note("R1 " + tag + ": " + std::to_string(a.r1.violations) + " of " + std::to_string(a.r1.spans) +
                " inter-boundary spans have restart count != 1");
    checks.expect(a.fields.size() >= 6, "PRE-FIELDS " + tag + ": at least 6 complete fields traced (measured " +
                  std::to_string(a.fields.size()) + ")");
}

// A-anchor precondition: every m_v (v=$30..$FF) lies on line v, using RF2's
// line-of-event bracketing against the capture's own trusted samples.
struct AnchorCheck { bool ok = true; size_t checked = 0, violations = 0; };

AnchorCheck check_anchor(const Analysis& a, const ListAOffsets& off) {
    AnchorCheck r;
    for (auto& w : a.rwins) {
        for (int v = off.v_start; v <= off.v_end; ++v) {
            auto fc = fetch_in(a.ridx, off.addr(off.vmark0(v)), w.lo, w.hi);
            if (!fc) continue;
            int64_t t = slot_of(a.sref, *fc);
            auto line = line_of_event(a.trusted, t);
            ++r.checked;
            if (!line || *line != v) { r.ok = false; ++r.violations; }
        }
    }
    return r;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    ListAOffsets loPAL;  loPAL.tail_n = 0x37;
    ListAOffsets loNTSC; loNTSC.tail_n = 6;

    // ---- base captures (ECS), PAL and NTSC ------------------------------
    Analysis pal  = analyze(run_capture(dir + "/prog_a.bin", kECS_PAL, kRunBudget));
    Analysis ntsc = analyze(run_capture(dir + "/prog_a_ntsc.bin", kECS_NTSC, kRunBudget));

    report_common_preconditions(checks, pal, "PAL");
    report_common_preconditions(checks, ntsc, "NTSC");

    AnchorCheck anchPAL = check_anchor(pal, loPAL);
    AnchorCheck anchNTSC = check_anchor(ntsc, loNTSC);
    checks.expect(anchPAL.ok, "PRE-ANCHOR PAL: every m_v (v=$30..$FF) lies on line v (RF2), " +
                  std::to_string(anchPAL.checked - anchPAL.violations) + "/" + std::to_string(anchPAL.checked) + " matched");
    checks.expect(anchNTSC.ok, "PRE-ANCHOR NTSC: every m_v (v=$30..$FF) lies on line v (RF2), " +
                  std::to_string(anchNTSC.checked - anchNTSC.violations) + "/" + std::to_string(anchNTSC.checked) + " matched");

    // ---- OCS cross-check (SPECIFICATION.md 3.1) --------------------------
    {
        Analysis palOCS  = analyze(run_capture(dir + "/prog_a.bin", kOCS_PAL, kRunBudget));
        Analysis ntscOCS = analyze(run_capture(dir + "/prog_a_ntsc.bin", kOCS_NTSC, kRunBudget));
        auto field_lens = [](const Analysis& a) {
            std::set<int> lens;
            for (auto& f : a.fields) {
                std::set<int> vs;
                for (size_t i = f.lo; i < f.hi; ++i) vs.insert(a.trusted[i].V);
                lens.insert((int)vs.size());
            }
            return lens;
        };
        auto palECSlens = field_lens(pal), palOCSlens = field_lens(palOCS);
        auto ntscECSlens = field_lens(ntsc), ntscOCSlens = field_lens(ntscOCS);
        bool palMatch = palECSlens == palOCSlens;
        bool ntscMatch = ntscECSlens == ntscOCSlens;
        auto setstr = [](const std::set<int>& s) {
            std::string o; for (int v : s) o += (o.empty() ? "" : ",") + std::to_string(v); return o.empty() ? "(none)" : o;
        };
        checks.note("OCS-CHECK PAL: ECS field-length set {" + setstr(palECSlens) + "} vs OCS {" + setstr(palOCSlens) +
                    "} -- " + (palMatch ? "match, assumption checked" : "MISMATCH, recorded not graded"));
        checks.note("OCS-CHECK NTSC: ECS field-length set {" + setstr(ntscECSlens) + "} vs OCS {" + setstr(ntscOCSlens) +
                    "} -- " + (ntscMatch ? "match, assumption checked" : "MISMATCH, recorded not graded"));
    }

    // =====================================================================
    // P1. A non-interlaced field has 262 lines (NTSC) or 312 lines (PAL),
    //     and the beam starts each field at (0,0). [p.24, p.34, p.27]
    // =====================================================================
    {
        auto count_check = [&](const Analysis& a, int expected) {
            bool all_ok = !a.fields.empty();
            size_t bad = 0;
            std::set<int> seen;
            for (auto& f : a.fields) {
                std::set<int> vs;
                for (size_t i = f.lo; i < f.hi; ++i) vs.insert(a.trusted[i].V);
                seen.insert((int)vs.size());
                if ((int)vs.size() != expected) { all_ok = false; ++bad; }
            }
            std::string seenstr; for (int v : seen) seenstr += (seenstr.empty() ? "" : ",") + std::to_string(v);
            checks.record("P1-COUNT distinct-V-per-field values measured: {" + seenstr + "} over " +
                          std::to_string(a.fields.size()) + " complete fields, " + std::to_string(bad) + " != " + std::to_string(expected));
            return all_ok;
        };

        if (mv == "P1-STD-PAL") {
            checks.note("P1-COUNT-PAL, main contrasting case: the NTSC capture graded against the PAL expectation (312)");
            bool r = count_check(ntsc, 312);
            checks.expect(r, "P1-COUNT-PAL COP2-vertical-beam-position.c4,COP2-using-the-copper-in-interlaced-mode.c4 [p.24,p.34]: "
                          "every complete field has 312 distinct V values");
        } else if (mv == "P1-STD-NTSC") {
            checks.note("P1-COUNT-NTSC, main contrasting case: the PAL capture graded against the NTSC expectation (262)");
            bool r = count_check(pal, 262);
            checks.expect(r, "P1-COUNT-NTSC COP2-vertical-beam-position.c3,COP2-using-the-copper-in-interlaced-mode.c2 [p.24,p.34]: "
                          "every complete field has 262 distinct V values");
        } else if (mv == "P1-LACE") {
            checks.note("P1-COUNT-PAL, A-LACE: PAL with LACE set, graded against the same 312 expectation");
            Analysis lace = analyze(run_capture(dir + "/prog_a_lace.bin", kECS_PAL, kRunBudget));
            bool r = count_check(lace, 312);
            checks.expect(r, "P1-COUNT-PAL COP2-vertical-beam-position.c4,COP2-using-the-copper-in-interlaced-mode.c4 [p.24,p.34]: "
                          "every complete field has 312 distinct V values");
        } else {
            bool rPAL = count_check(pal, 312);
            checks.expect(rPAL, "P1-COUNT-PAL COP2-vertical-beam-position.c4,COP2-using-the-copper-in-interlaced-mode.c4 [p.24,p.34]: "
                          "every complete field has 312 distinct V values");
            bool rNTSC = count_check(ntsc, 262);
            checks.expect(rNTSC, "P1-COUNT-NTSC COP2-vertical-beam-position.c3,COP2-using-the-copper-in-interlaced-mode.c2 [p.24,p.34]: "
                          "every complete field has 262 distinct V values");
        }

        auto origin_check = [&](const Analysis& a) {
            bool all_ok = !a.fields.empty();
            bool h0 = false;
            for (auto& s : a.all) if (s.H == 0) { h0 = true; break; }
            for (auto& f : a.fields) if (a.trusted[f.lo].V != 0) all_ok = false;
            return all_ok && h0;
        };
        bool oPAL = origin_check(pal);
        checks.expect(oPAL, "P1-ORIGIN-PAL COP2-putting-together-a-copper-instruction-list.c7 [p.27]: "
                      "every complete field's first trusted sample has V=0, and H=0 occurs in the trace");
        bool oNTSC = origin_check(ntsc);
        checks.expect(oNTSC, "P1-ORIGIN-NTSC COP2-putting-together-a-copper-instruction-list.c7 [p.27]: "
                      "every complete field's first trusted sample has V=0, and H=0 occurs in the trace");
    }

    // These four re-run the package's own precondition check (same label,
    // same comparator) against a mutated capture: the label naturally
    // prints FAIL where the normal run printed PASS.
    if (mv == "P1-T1") {
        Analysis slow = analyze(run_capture(dir + "/prog_a_slow.bin", kECS_PAL, kRunBudget));
        checks.note("T1 PAL, A-slow: " + std::to_string(slow.rf2.t1_violations) + " of " + std::to_string(slow.rf2.t1_total) +
                    " consecutive-sample gaps exceed 16 slots");
        checks.expect(slow.rf2.t1, "PRE-T1 PAL: every consecutive sample pair is at most 16 slots apart (RF2)");
    }
    if (mv == "P1-T2") {
        Analysis hold = analyze(run_capture(dir + "/prog_a_hold.bin", kECS_PAL, kRunBudget));
        checks.note("T2 PAL, A-hold: " + std::to_string(hold.rf2.t2_violations) + " of " + std::to_string(hold.rf2.t2_total) +
                    " trusted-sample transitions violate T2");
        checks.expect(hold.rf2.t2, "PRE-T2 PAL: the beam counter is live between trusted samples (RF2)");
    }
    if (mv == "P1-R1") {
        Analysis nocop = analyze(run_capture(dir + "/prog_a_nocop.bin", kECS_PAL, kRunBudget));
        checks.note("R1 PAL, A-noCop: " + std::to_string(nocop.r1.violations) + " of " + std::to_string(nocop.r1.spans) +
                    " inter-boundary spans have restart count != 1");
        checks.expect(nocop.r1.ok, "PRE-R1 PAL: exactly one restart between consecutive field boundaries (RF3)");
    }
    if (mv == "P1-DONE") {
        Capture nodone = run_capture(dir + "/prog_a_nodone.bin", kECS_PAL, kNoDoneBudget);
        checks.expect(nodone.done.has_value(), "PRE-DONE PAL: the completion word was written");
    }

    // =====================================================================
    // P2. The Copper sees every PAL line as 227 counts; in NTSC, lines
    //     alternate between 228 and 227. [p.23, p.24]
    // =====================================================================
    {
        // L(v) for v = v_start..v_end-1 within one field window, using the
        // main loop's markers; and the tail-loop wraparound part, v=256..
        // (256+tail_n-1), using the tail markers.
        auto time_of = [&](const Analysis& a, const ListAOffsets& off, const RWin& w, int off_word) -> std::optional<int64_t> {
            auto fc = fetch_in(a.ridx, off.addr(off_word), w.lo, w.hi);
            if (!fc) return std::nullopt;
            return slot_of(a.sref, *fc);
        };

        struct LResult { bool all_pass = true; size_t measured = 0; std::set<int64_t> valuesPAL; bool alt_ok = true; };

        auto grade_pal = [&](const Analysis& a, const ListAOffsets& off) {
            LResult r;
            for (auto& w : a.rwins) {
                std::vector<int64_t> Ls;
                for (int v = off.v_start; v < off.v_end; ++v) {
                    auto t0 = time_of(a, off, w, off.vmark0(v));
                    auto t1 = time_of(a, off, w, off.vmark0(v + 1));
                    if (!t0 || !t1) continue;
                    int64_t L = *t1 - *t0;
                    ++r.measured;
                    r.valuesPAL.insert(L);
                    if (L != 227) r.all_pass = false;
                }
                for (int n = 0; n + 1 < off.tail_n + 1; ++n) {
                    auto t0 = time_of(a, off, w, off.tailmark0(n));
                    auto t1 = time_of(a, off, w, off.tailmark0(n + 1));
                    if (!t0 || !t1) continue;
                    int64_t L = *t1 - *t0;
                    ++r.measured;
                    r.valuesPAL.insert(L);
                    if (L != 227) r.all_pass = false;
                }
            }
            return r;
        };

        auto grade_ntsc = [&](const Analysis& a, const ListAOffsets& off) {
            LResult r;
            for (auto& w : a.rwins) {
                std::vector<int64_t> Ls;
                for (int v = off.v_start; v < off.v_end; ++v) {
                    auto t0 = time_of(a, off, w, off.vmark0(v));
                    auto t1 = time_of(a, off, w, off.vmark0(v + 1));
                    if (!t0 || !t1) { Ls.push_back(INT64_MIN); continue; }
                    Ls.push_back(*t1 - *t0);
                }
                for (int n = 0; n + 1 < off.tail_n; ++n) {
                    auto t0 = time_of(a, off, w, off.tailmark0(n));
                    auto t1 = time_of(a, off, w, off.tailmark0(n + 1));
                    Ls.push_back((t0 && t1) ? (*t1 - *t0) : INT64_MIN);
                }
                for (size_t i = 0; i < Ls.size(); ++i) {
                    if (Ls[i] == INT64_MIN) continue;
                    ++r.measured;
                    r.valuesPAL.insert(Ls[i]);
                    if (Ls[i] != 227 && Ls[i] != 228) r.all_pass = false;
                    if (i + 1 < Ls.size() && Ls[i + 1] != INT64_MIN && Ls[i] == Ls[i + 1]) r.alt_ok = false;
                }
            }
            return r;
        };

        auto setstr = [](const std::set<int64_t>& s) {
            std::string o; for (auto v : s) o += (o.empty() ? "" : ",") + std::to_string(v); return o.empty() ? "(none)" : o;
        };

        if (mv == "P2-STD-PAL") {
            checks.note("P2-PAL, main contrasting case: the NTSC capture graded against the PAL expectation. This does NOT redden P2-PAL: "
                        "the NTSC capture's own L(v) values are also all 227 (P2-NTSC's own finding, same capture), so grading them against "
                        "the PAL expectation stays green; P2-PAL's own red mutation is P2-SHIFT");
            LResult r = grade_pal(ntsc, loNTSC);
            checks.record("P2-PAL L(v) values measured against NTSC capture: {" + setstr(r.valuesPAL) + "} over " + std::to_string(r.measured) + " pairs");
            checks.expect(r.all_pass, "P2-PAL COP2-horizontal-beam-position.c15 [p.23,p.24]: L(v)=227 for every measured v in every complete field");
        } else if (mv == "P2-STD-NTSC") {
            checks.note("P2-NTSC, main contrasting case: the PAL capture graded against the NTSC expectation");
            LResult r = grade_ntsc(pal, loPAL);
            checks.record("P2-NTSC L(v) values measured against PAL capture: {" + setstr(r.valuesPAL) + "} over " + std::to_string(r.measured) + " pairs");
            checks.expect(r.all_pass && r.alt_ok, "P2-NTSC COP2-horizontal-beam-position.c10,c11,c12,c14,COP2-vertical-beam-position.c11 "
                          "[p.23,p.24]: L(v) in {227,228} for every measured v, and L(v)!=L(v+1) for every measured consecutive pair, in every "
                          "complete field");
        } else if (mv == "P2-SHIFT") {
            checks.note("P2-PAL, A-shift: WAIT($80,$42) in place of WAIT($80,$40)");
            Analysis shift = analyze(run_capture(dir + "/prog_a_shift.bin", kECS_PAL, kRunBudget));
            LResult r = grade_pal(shift, loPAL);
            checks.record("P2-PAL L(v) values measured, A-shift: {" + setstr(r.valuesPAL) + "} over " + std::to_string(r.measured) + " pairs");
            checks.expect(r.all_pass, "P2-PAL COP2-horizontal-beam-position.c15 [p.23,p.24]: L(v)=227 for every measured v in every complete field");
        } else {
            LResult rPAL = grade_pal(pal, loPAL);
            checks.record("P2-PAL L(v) values measured: {" + setstr(rPAL.valuesPAL) + "} over " + std::to_string(rPAL.measured) + " pairs");
            checks.expect(rPAL.all_pass, "P2-PAL COP2-horizontal-beam-position.c15 [p.23,p.24]: L(v)=227 for every measured v in every complete field");

            LResult rNTSC = grade_ntsc(ntsc, loNTSC);
            checks.record("P2-NTSC L(v) values measured: {" + setstr(rNTSC.valuesPAL) + "} over " + std::to_string(rNTSC.measured) + " pairs");
            checks.expect(rNTSC.all_pass && rNTSC.alt_ok, "P2-NTSC COP2-horizontal-beam-position.c10,c11,c12,c14,COP2-vertical-beam-position.c11 "
                          "[p.23,p.24]: L(v) in {227,228} for every measured v, and L(v)!=L(v+1) for every measured consecutive pair, in every "
                          "complete field");
        }
    }

    if (mv == "P2-WAITED") {
        Analysis w = analyze(run_capture(dir + "/prog_a_p2waited.bin", kECS_PAL, kRunBudget));
        // the extra WAIT($7F,$40) sits at word offset vmark1(0x7F)+1 ==
        // vwait0(0x7F)+4 in this program's own layout (see prog_a.s).
        ListAOffsets off = loPAL;
        bool any_checked = false, all_lambda = true;
        int64_t lambda_min = INT64_MIN;
        for (auto& win : w.rwins) {
            auto l0 = fetch_in(w.ridx, off.addr(0), win.lo, win.hi);
            auto l1 = fetch_in(w.ridx, off.addr(2), win.lo, win.hi);
            if (l0 && l1) lambda_min = slot_of(w.sref, *l1) - slot_of(w.sref, *l0);
            int extra_wait1 = off.vmark1(0x7F) + 1; // second word of the inserted WAIT
            int next_instr0 = off.vwait0(0x80);      // shifted by +2 words from the base layout
            auto ew1 = fetch_in(w.ridx, off.addr(extra_wait1), win.lo, win.hi);
            auto nx0 = fetch_in(w.ridx, off.addr(next_instr0), win.lo, win.hi);
            if (!ew1 || !nx0 || lambda_min == INT64_MIN) continue;
            int64_t delay = slot_of(w.sref, *nx0) - slot_of(w.sref, *ew1);
            any_checked = true;
            if (delay > lambda_min) all_lambda = false;
        }
        checks.expect(any_checked && all_lambda, "P2-WAITED [p.22]: MUTATION stimulus red, the extra WAIT($7F,$40) inserted after MARK($7F) is "
                      "already satisfied when fetched -- its delay equals lambda_min");
    }

    if (mv == "ANCHOR") {
        Analysis anch = analyze(run_capture(dir + "/prog_a_anchor.bin", kECS_PAL, kRunBudget));
        ListAOffsets off = loPAL; off.v_end = 0xFE;
        AnchorCheck a = check_anchor(anch, off);
        checks.note("PRE-ANCHOR PAL, A-anchor: " + std::to_string(a.checked - a.violations) + "/" + std::to_string(a.checked) + " matched");
        checks.expect(a.ok, "PRE-ANCHOR PAL: every m_v (v=$30..$FF) lies on line v (RF2)");
    }

    // =====================================================================
    // P7. In the Copper's comparison, line 256 appears as zero, and line
    //     256+n as n; the beam counter itself does not wrap. [p.23, p.24]
    // =====================================================================
    {
        // PRE-E2-REACHED: after WAIT($FF,$40) (m_$FF) is fetched at line
        // 255, does the list ever fetch m_256 (the first tail marker) in
        // the same field? This is the padding recipe's own working
        // assumption (SPECIFICATION.md 5, Run A: "The 16 padding moves put
        // the next WAIT's fetch well inside line 256").
        auto e2_reached = [&](const Analysis& a, const ListAOffsets& off) {
            size_t reached = 0, fields = 0;
            for (auto& w : a.rwins) {
                ++fields;
                if (fetch_in(a.ridx, off.addr(off.tailmark0(0)), w.lo, w.hi)) ++reached;
            }
            return std::make_pair(reached, fields);
        };
        auto [reachedPAL, fieldsPAL] = e2_reached(pal, loPAL);
        checks.expect(reachedPAL == fieldsPAL && fieldsPAL > 0,
                      "PRE-E2-REACHED PAL [p.23]: m_256 (the first tail marker) is fetched within the same field as m_$FF, in every field (" +
                      std::to_string(reachedPAL) + "/" + std::to_string(fieldsPAL) + ")");
        auto [reachedNTSC, fieldsNTSC] = e2_reached(ntsc, loNTSC);
        checks.expect(reachedNTSC == fieldsNTSC && fieldsNTSC > 0,
                      "PRE-E2-REACHED NTSC [p.23]: m_256 (the first tail marker) is fetched within the same field as m_$FF, in every field (" +
                      std::to_string(reachedNTSC) + "/" + std::to_string(fieldsNTSC) + ")");

        // P7's own claimed range excludes n = tail_n in NTSC (m_262: P8's
        // territory, never fetched); PAL's range is the whole tail loop.
        auto grade_p7 = [&](const Analysis& a, const ListAOffsets& off, int n_max) {
            bool all_ok = true; size_t checked = 0;
            bool waited_ok = true; size_t waited_checked = 0; int64_t lambda_min = INT64_MIN;
            for (auto& w : a.rwins) {
                auto l0 = fetch_in(a.ridx, off.addr(0), w.lo, w.hi);
                auto l1 = fetch_in(a.ridx, off.addr(2), w.lo, w.hi);
                if (l0 && l1) lambda_min = slot_of(a.sref, *l1) - slot_of(a.sref, *l0);
                for (int n = 0; n <= n_max; ++n) {
                    auto fc = fetch_in(a.ridx, off.addr(off.tailmark0(n)), w.lo, w.hi);
                    if (!fc) continue;
                    ++checked;
                    int64_t t = slot_of(a.sref, *fc);
                    auto line = line_of_event(a.trusted, t);
                    if (!line || *line != 256 + n) all_ok = false;
                    if (n == 0 && lambda_min != INT64_MIN) {
                        auto w0 = fetch_in(a.ridx, off.addr(off.tailwait1(0)), w.lo, w.hi);
                        auto nx = fetch_in(a.ridx, off.addr(off.tailmark0(0)), w.lo, w.hi);
                        if (w0 && nx) {
                            ++waited_checked;
                            int64_t delay = slot_of(a.sref, *nx) - slot_of(a.sref, *w0);
                            if (delay <= lambda_min) waited_ok = false;
                        }
                    }
                }
            }
            return std::make_pair(all_ok && checked > 0, waited_ok && waited_checked > 0);
        };

        if (mv == "P7-NOPREP-PAL") {
            Analysis np = analyze(run_capture(dir + "/prog_a_noprep_pal.bin", kECS_PAL, kRunBudget));
            ListAOffsets off = loPAL; off.has_prep = false;
            auto [ok, waited] = grade_p7(np, off, off.tail_n);
            checks.note("P7-PAL, A-noprep: WAIT($00,$60) is fetched on line 255 and ends at once, so m_256 no longer lies on line 256 -- the "
                        "base run is already red here for a different reason (PRE-E2-REACHED); this reddens the same comparator through a "
                        "genuine per-marker mismatch instead of an empty population");
            checks.expect(ok, "P7-PAL COP2-the-wait-instruction.c22,c23,COP2-vertical-beam-position.c7 [p.23,p.24]: every m_(256+n) lies on "
                          "line 256+n for n=0..55");
        } else if (mv == "P7-NOPREP-NTSC") {
            Analysis np = analyze(run_capture(dir + "/prog_a_noprep_ntsc.bin", kECS_NTSC, kRunBudget));
            ListAOffsets off = loNTSC; off.has_prep = false;
            auto [ok, waited] = grade_p7(np, off, 5);
            checks.note("P7-NTSC, A-noprep: WAIT($00,$60) is fetched on line 255 and ends at once, so m_256 no longer lies on line 256 -- the "
                        "base run is already red here for a different reason (PRE-E2-REACHED); this reddens the same comparator through a "
                        "genuine per-marker mismatch instead of an empty population");
            checks.expect(ok, "P7-NTSC COP2-the-wait-instruction.c22,c23,COP2-vertical-beam-position.c7 [p.23,p.24]: every m_(256+n) lies "
                          "on line 256+n for n=0..5");
        } else {
            auto [okPAL, waitedPAL] = grade_p7(pal, loPAL, loPAL.tail_n);
            checks.expect(okPAL, "P7-PAL COP2-the-wait-instruction.c22,c23,COP2-vertical-beam-position.c7 [p.23,p.24]: every m_(256+n) lies on "
                          "line 256+n for n=0..55");
            checks.expect(waitedPAL, "P7-PAL-WAITED [p.22]: WAIT($00,$60) waited (its delay exceeds lambda_min)");

            auto [okNTSC, waitedNTSC] = grade_p7(ntsc, loNTSC, 5);
            checks.expect(okNTSC, "P7-NTSC COP2-the-wait-instruction.c22,c23,COP2-vertical-beam-position.c7 [p.23,p.24]: every m_(256+n) lies "
                          "on line 256+n for n=0..5");
            checks.expect(waitedNTSC, "P7-NTSC-WAITED [p.22]: WAIT($00,$60) waited (its delay exceeds lambda_min)");
        }
    }

    // =====================================================================
    // P8 (NTSC only). After line 255, vertical positions 0-5 are the last
    //     six lines of the field before vertical blanking (256..261);
    //     there is no line 262. [p.24]
    // =====================================================================
    {
        const ListAOffsets& off = loNTSC;
        bool m256_261_ok = true; size_t checked261 = 0;
        bool after261_is0 = true; size_t checked_after = 0;
        bool m262_absent = true; size_t checked262 = 0;
        for (auto& w : ntsc.rwins) {
            for (int n = 0; n <= 5; ++n) {
                auto fc = fetch_in(ntsc.ridx, off.addr(off.tailmark0(n)), w.lo, w.hi);
                if (!fc) continue;
                ++checked261;
                auto line = line_of_event(ntsc.trusted, slot_of(ntsc.sref, *fc));
                if (!line || *line != 256 + n) m256_261_ok = false;
            }
            // trusted sample after V=261 is on line 0
            for (size_t i = 0; i + 1 < ntsc.trusted.size(); ++i) {
                if (ntsc.trusted[i].cyc < w.lo || ntsc.trusted[i].cyc >= w.hi) continue;
                if (ntsc.trusted[i].V == 261 && ntsc.trusted[i + 1].cyc < w.hi) {
                    ++checked_after;
                    if (ntsc.trusted[i + 1].V != 0) after261_is0 = false;
                }
            }
            // m_262 (n=6) must not be fetched between m_261's fetch and the
            // window's own end (the next field boundary/restart).
            auto f261 = fetch_in(ntsc.ridx, off.addr(off.tailmark0(5)), w.lo, w.hi);
            if (f261) {
                ++checked262;
                auto f262 = fetch_in(ntsc.ridx, off.addr(off.tailmark0(6)), *f261, w.hi);
                if (f262) m262_absent = false;
            }
        }
        checks.expect(m256_261_ok && checked261 > 0, "P8 COP2-vertical-beam-position.c9 [p.24]: m_(256+n) lies on line 256+n for n=0..5, every complete field");
        checks.expect(after261_is0 && checked_after > 0, "P8 COP2-vertical-beam-position.c9 [p.24]: the trusted sample after line 261 is on line 0");
        checks.expect(m262_absent && checked262 > 0, "P8 COP2-vertical-beam-position.c9 [p.24]: m_262 is not fetched between m_261 and the next field boundary");
        checks.record("P8-SCOPE: contrasting case \"select PAL\" is covered by P1-COUNT-NTSC's own mismatch; no separate P8 mutation is built "
                      "in this pass -- see the report to the lead");
    }

    checks.record("SCOPE: P3, P4, P5, P6 and P9 (Runs B, C, D, E) are not built in this pass -- see the report to the lead");

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
