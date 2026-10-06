// test_ch2_copper_beam.cpp -- ch2-copper-beam. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file implements what SPECIFICATION.md specifies: RF1-RF4,
// P1 to P9.
//
// NMH_MUTATE selects a mutated program or a re-grading of an existing
// capture in place of the normal one, one assertion or precondition at a
// time; none is exercised by a plain `make test`. A selected mutation runs
// only the section that owns the mutated line, with the unchanged
// comparators of that section.
//
// The mutation records: one line per label, the value of NMH_MUTATE that selects the variant, and what it changes. SPECIFICATION.md 8.6 gives the verdicts of both cores.
//
// MUTATION FIX-LOAD none impossible apparatus check: it fails only when the harness cannot load the program or release the CPU, which no stimulus of this package varies (not exercised, by a scope decision)
// MUTATION FIX-CONST stimulus red NMH_MUTATE=A-NOFIX-PAL: the program leaves out the store of $A5C3 to $0C0002, so the port logs no such write and the dependent lines are NOT GRADED
// MUTATION PRE-DONE stimulus red NMH_MUTATE=A-NODONE-PAL: the program starts the Copper and halts before the trace and before DONE, so the completion word is never written
// MUTATION PRE-S1A none impossible unfalsifiable at this boundary: no input of the programs changes the refresh slots (RF1)
// MUTATION PRE-S1B none impossible unfalsifiable at this boundary: no input of the programs changes the refresh slots, so no two accesses can be made to share one (RF1)
// MUTATION PRE-L1 stimulus red NMH_MUTATE=A-NOOVL-PAL: CIA-A is not written (step 2 left out), the overlay stays on, the CPU's writes of the list never reach Chip memory and the port logs none; the program still reaches DONE
// MUTATION PRE-T1 stimulus green NMH_MUTATE=A-COPOFF-PAL: Copper DMA is never enabled (DMACON = $8200), so no restart interrupts the trace and every step lasts 12 or 13 slots; the unchanged comparator passes where the normal run is red
// MUTATION PRE-T1E stimulus red NMH_MUTATE=A-SLOW-PAL: a delay of 100 dbra iterations (at least 300 slots) between 400 samples, so the events fall between samples more than 16 slots apart
// MUTATION PRE-T2 stimulus red NMH_MUTATE=A-HOLD-PAL: one read of $DFF004 after polling H >= $20 is stored in every step, so H never increases between trusted samples
// MUTATION PRE-R1 stimulus red NMH_MUTATE=A-NOCOP-PAL: DMACON = $0080 (COPEN cleared) is written after the third field boundary the trace sees, so the later fields have no restart
// MUTATION PRE-FIELDS stimulus red NMH_MUTATE=A-SHORT-PAL: the trace buffer holds 64 samples, far fewer than 6 complete fields
// MUTATION PRE-STD stimulus red NMH_MUTATE=A-STD-NTSC-LABEL-PALCAP: the PAL capture is graded under the NTSC line, and its chip identification names PAL
// MUTATION PRE-ANCHOR stimulus red NMH_MUTATE=A-ANCHOR-PAL: every WAIT(v,$40) becomes WAIT(v+1,$40), so m_v lies on line v+1
// MUTATION PRE-P2WAITED stimulus red NMH_MUTATE=A-P2WAITED-PAL: an extra WAIT($7F,$40) after MARK($7F) is fetched after its target has passed, so its delay equals lambda_min
// MUTATION PRE-E0 stimulus red NMH_MUTATE=A-NOPREP-PAL: WAIT($FF,$E0), MARK($A0E0) and the padding are left out, so m_E0 does not exist
// MUTATION PRE-P7L256 stimulus red NMH_MUTATE=A-NOPREP-PAL: WAIT($FF,$E0) and the padding are left out, so the second word of WAIT(0,$60) is fetched on line 255
// MUTATION P1-ORIGIN none impossible not exercised, by a scope decision: a write to VPOSW or VHPOSW (p.216) would move the counter and the pages would then predict the written position, not (0,0); the package never writes them
// MUTATION P2-PAL stimulus red NMH_MUTATE=A-SHIFT-PAL: WAIT($80,$42) in place of WAIT($80,$40), so L($7F) and L($80) differ from 227
// MUTATION P2-NTSC none impossible already red on both cores, and no input of this package gives a line of 228 slots, so no capture can pass the comparator; neither unfalsifiable nor a scope decision
// MUTATION PRE-P3ODDEND stimulus red NMH_MUTATE=A-SHORT-NTSC: the trace buffer holds 64 samples, so the odd count $E1 is not read
// MUTATION PRE-P3COVER none impossible already red on both cores, and no capture of this package has a line of 228 slots, so no input passes the comparator; neither unfalsifiable nor a scope decision
// MUTATION P3-PAL stimulus red NMH_MUTATE=A-SHORT-PAL: the trace buffer holds 64 samples, so most even counts are never read
// MUTATION P3-NTSC-SET none impossible already red on both cores, and no capture of this package reads $E3, so no input passes the comparator; neither unfalsifiable nor a scope decision
// MUTATION P3-NTSC-LONGONLY none impossible NOT GRADED on both cores because PRE-P3COVER fails, and no input exists for which the pages predict $E3 on a short line
// MUTATION P7 stimulus red NMH_MUTATE=A-TAILSHIFT-PAL: every tail WAIT(n,$60) becomes WAIT(n+1,$60), so m_(256+n) lies on line 257+n
// MUTATION P7-WAITED stimulus red NMH_MUTATE=A-LONGPAD-PAL: 16 further padding moves put the fetch of WAIT(0,$60) after count $60, so it is already satisfied
// MUTATION P8-LINES stimulus red NMH_MUTATE=A-TAILSHIFT-NTSC: every tail WAIT(n,$60) becomes WAIT(n+1,$60), so m_(256+n) lies on line 257+n
// MUTATION P8-AFTER none impossible already red on both cores, and no input of this package changes the length of a field, so no capture has line 0 after line 261; neither unfalsifiable nor a scope decision
// MUTATION P8-NO262 stimulus green NMH_MUTATE=A-TAILPAST-NTSC: the last tail WAIT is WAIT(7,$60), a line that no field has, so m_262 is not fetched through the unchanged comparator where the normal run is red
// MUTATION P8 none impossible already red on both cores through P8-AFTER, which no input of this package passes; neither unfalsifiable nor a scope decision
// MUTATION PRE-C-R1LINE stimulus red NMH_MUTATE=C-CR1LINE-PAL: WAIT(w_5,$C0) becomes WAIT(w_5+1,$C0), so r1_5 lies on line w_5+1
// MUTATION PRE-C-WAITED stimulus red NMH_MUTATE=C-CLATE-PAL: the reference of k = 90 is WAIT(u_90,$C4), so the following WAIT(u_90,$B4) is fetched after its target
// MUTATION P4-A stimulus red NMH_MUTATE=C-CSHIFT-PAL: WAIT(u_50,$66) in place of WAIT(u_50,$64), so D_50 changes by 2 slots
// MUTATION P4-B stimulus red NMH_MUTATE=C-CSHIFTEND-PAL: WAIT(u_106,$D6) in place of WAIT(u_106,$D4), so D_106 is not below D_107
// MUTATION P4-C stimulus red NMH_MUTATE=C-CNEXT-PAL: q_5 is WAIT(w_5+1,$50), so the WAIT of r2_5 is fetched after its target and did not wait
// MUTATION P4 stimulus red NMH_MUTATE=C-CSHIFT-PAL: WAIT(u_50,$66) in place of WAIT(u_50,$64), so the aggregate fails through P4-A
// MUTATION PRE-B-CHAIN stimulus red NMH_MUTATE=B-BCHAIN-PAL: the COP1LC moves of L_3 name L_5, so the restart after L_3 fetches L_5 and not L_4
// MUTATION PRE-B-L255 stimulus red NMH_MUTATE=B-BLATE-PAL: in L_1 a WAIT($FF,$E0) and 16 padding moves precede WAIT($FF,h_1), so its second word is fetched on line 256
// MUTATION PRE-B-WAITED stimulus red NMH_MUTATE=B-BWAITED-PAL: in L_4 a WAIT($FF,$80) precedes WAIT($FF,$40), so the latter is fetched after its target
// MUTATION P6-LINE255 stimulus red NMH_MUTATE=B-B254-PAL: in L_2 the first WAIT is for line $FE and a WAIT($FF,$30) follows it, so a_2 lies on line 254 while the preconditions still hold
// MUTATION P6-CONTROL stimulus red NMH_MUTATE=B-BFEOFF-PAL: L_FE waits for ($FF,$FE), so $B300 is not fetched
// MUTATION P6-LARGEST none impossible already red on both cores: the first word after a pending WAIT is fetched just before the next restart in every field, so x_j is fetched for every h_j above $E2 and no input of this package passes the comparator; neither unfalsifiable nor a scope decision
// MUTATION P6-NEVER none impossible already red on both cores for the reason of P6-LARGEST: no field of this package has a WAIT that never ends without a fetch before the next restart; neither unfalsifiable nor a scope decision
// MUTATION PRE-P5-CALLO stimulus red NMH_MUTATE=D-DFLATLO-PAL: every plane word of D-lo is $FFFF, so there are no alternating runs
// MUTATION PRE-P5-CALHI stimulus red NMH_MUTATE=D-DFLATHI-PAL: every plane word of D-hi is $FFFF, so there are no alternating runs
// MUTATION PRE-P5-EDGES stimulus red NMH_MUTATE=D-DDROP-PAL: the second MOVE($180,B) of line $50 is left out, so line $50 has no A to B edge
// MUTATION PRE-P5-WAITED stimulus red NMH_MUTATE=D-DLATE-PAL: the second WAIT of line $50 is at $3A, already satisfied when it is fetched
// MUTATION P5-LO stimulus red NMH_MUTATE=D-DSHIFT-PAL: the second WAIT of line $50 is at $36+2*24+2, so w_$50 is 4 * 25 * P_lo while n_$50 is 24
// MUTATION P5-HI stimulus red NMH_MUTATE=D-DSHIFT-PAL: the second WAIT of line $50 is at $36+2*24+2, so w_$50 is 8 * 25 * P_hi while n_$50 is 24
// MUTATION PRE-P9-PLO stimulus red NMH_MUTATE=E-PLOFLAT-PAL: the calibration run D-lo has every plane word $FFFF, so P_lo is not valid
// MUTATION PRE-P9-LINES stimulus red NMH_MUTATE=E-ENOBPL-PAL: bitplane DMA is left off, so no output line shows COLOR01
// MUTATION PRE-P9-ONE0 stimulus red NMH_MUTATE=E-EBLACKGAP-PAL: COLOR00 = $000 and the 10th plane word of every line is $0000, so each window line has a second run of 0
// MUTATION P9-A-E stimulus red NMH_MUTATE=E-EWIDEWIN-PAL: DIWSTRT $2C71 and DIWSTOP $2CD1 with 22 words of $FFFF per line, so the COLOR01 run is 352 * P_lo
// MUTATION P9-A-EW stimulus red NMH_MUTATE=E-EWWIDEWIN-PAL: the E-widefetch capture is replaced by E-widewindow, so its COLOR01 run is 352 * P_lo
// MUTATION P9-A stimulus red NMH_MUTATE=E-EWIDEWIN-PAL: DIWSTRT $2C71 and DIWSTOP $2CD1 with 22 words of $FFFF per line, so the aggregate fails through P9-A-E
// MUTATION P9-B stimulus red NMH_MUTATE=E-EBLACK-PAL: COLOR00 = $000, so no COLOR00 pixel is distinguishable from the blanking on either side

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
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

// ---- the fixed assertion call form --------------------------------------
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

// ---- memory layout (SPECIFICATION.md 3.6) --------------------------------
constexpr uint32_t kListBase    = 0x010000;
constexpr uint32_t kListBaseW   = kListBase / 2;
constexpr uint32_t kListWords   = 0x8000;
constexpr uint32_t kTraceBase   = 0x100000;
constexpr uint32_t kTraceBaseW  = kTraceBase / 2;
constexpr uint32_t kTraceMaxW   = 0x080000;
constexpr uint32_t kResultsW    = 0x0C0000 / 2;
constexpr uint32_t kFixtureW    = 0x0C0002 / 2;
constexpr uint32_t kDoneAddr    = 0x0C0FFE;
constexpr uint16_t kDoneValue   = 0x600D;
constexpr uint16_t kFixtureValue = 0xA5C3;

// chipset_config values: ECS in every run. The second value of each pair
// is the harness input that gives an NTSC chip identification; PRE-STD
// checks the identification of every run and nothing else here assumes it.
constexpr uint8_t kECS_PAL  = 0x08;
constexpr uint8_t kECS_NTSC = 0x0A;
constexpr uint8_t kOCS_PAL  = 0x00;
constexpr uint8_t kOCS_NTSC = 0x02;

constexpr uint64_t kBootBudget    = 6'000'000;
constexpr uint64_t kRunBudgetA    = 20'000'000;
constexpr uint64_t kRunBudgetB    = 30'000'000;
constexpr uint64_t kRunBudgetC    = 14'000'000;
constexpr uint64_t kRunBudgetDE   = 24'000'000;
constexpr uint64_t kNoDoneBudget  = 3'000'000;

enum Tag { PAL = 0, NTSC = 1 };
const char* tagname(Tag t) { return t == PAL ? "PAL" : "NTSC"; }
uint8_t cfg_ecs(Tag t) { return t == PAL ? kECS_PAL : kECS_NTSC; }
uint8_t cfg_ocs(Tag t) { return t == PAL ? kOCS_PAL : kOCS_NTSC; }

std::string num(long long v) { return std::to_string(v); }
std::string hex2(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%02X", v & 0xFF); return b; }
std::string hex4(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%04X", v & 0xFFFF); return b; }

// ---- capture ---------------------------------------------------------------
struct Capture {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> raw;  // for S1/refresh (see measure_slots)
    std::vector<nmh::RamAccess> log;  // coalesced, for everything else
    std::vector<nmh::VideoSample> video;
};

Capture run_capture(const std::string& bin, uint8_t chipset_config, uint64_t run_budget, bool keep_video = false) {
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
    if (keep_video) c.video = h.video();
    return c;
}

// ---- RF1: memory slots (SPECIFICATION.md 3.2) ----------------------------
// Refresh reads are RAM-port reads at word address 0, four per line, each two
// slots after the one before (Figure 6-9). They are identified from the raw
// (uncoalesced) log: coalesce() merges consecutive same-address, same-data
// entries regardless of how many real cycles separate them.
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

// S1's second clause: no two logged accesses, merged per bus cycle, occupy
// the same slot. slot() is non-decreasing in cycle, so a same-slot pair
// anywhere in the log implies a same-slot ADJACENT pair too.
bool s1_no_collision(const SlotRef& ref, const std::vector<nmh::RamAccess>& log, size_t* violations) {
    *violations = 0;
    int64_t prev = INT64_MIN;
    bool ok = ref.ok;
    for (auto& a : log) {
        int64_t s = slot_of(ref, a.cycle);
        if (s == prev) { ok = false; ++*violations; }
        prev = s;
    }
    return ok;
}

// ---- RF2: beam trace -------------------------------------------------------
struct Sample { uint64_t cyc; int64_t w; int V, H, id; size_t idx; bool trusted; };

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
        s.id = (hi >> 8) & 0x7F;
        s.idx = out.size();
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

// Chip identification (SPECIFICATION.md 3.1, p.299): 00 and 20 name PAL, 10
// and 30 name NTSC. Returns 0 for PAL, 1 for NTSC, -1 for neither or mixed.
int std_of_id(int id) {
    if (id == 0x00 || id == 0x20) return 0;
    if (id == 0x10 || id == 0x30) return 1;
    return -1;
}

struct RF2Precond {
    bool t1 = true; size_t t1_violations = 0, t1_total = 0, t1_at_boundary = 0;
    bool t2 = true; size_t t2_violations = 0, t2_total = 0;
};

RF2Precond check_rf2(const std::vector<Sample>& all, const std::vector<Sample>& trusted) {
    RF2Precond r;
    for (size_t i = 0; i + 1 < all.size(); ++i) {
        ++r.t1_total;
        if (all[i + 1].w - all[i].w > 16) {
            r.t1 = false; ++r.t1_violations;
            if (all[i + 1].V < all[i].V) ++r.t1_at_boundary;
        }
    }
    for (size_t i = 0; i + 1 < trusted.size(); ++i) {
        ++r.t2_total;
        int V0 = trusted[i].V, V1 = trusted[i + 1].V, H0 = trusted[i].H, H1 = trusted[i + 1].H;
        bool ok = (V0 == V1) ? (H1 > H0) : (V1 == V0 + 1 || V1 == 0);
        if (!ok) { r.t2 = false; ++r.t2_violations; }
    }
    return r;
}

// ---- RF3: fields and Copper restarts ----------------------------------------
struct Restart { uint64_t cycle; uint32_t base; };
struct RWin { uint64_t lo, hi; uint32_t base; };
struct FieldSpan { size_t lo, hi; }; // trusted[] index range, half-open

std::vector<size_t> trace_wraps(const std::vector<Sample>& trusted) {
    std::vector<size_t> w;
    for (size_t i = 1; i < trusted.size(); ++i) if (trusted[i].V < trusted[i - 1].V) w.push_back(i);
    return w;
}

std::vector<Restart> restart_events(const std::vector<nmh::RamAccess>& log, const std::set<uint32_t>& bases) {
    std::vector<Restart> v;
    for (auto& a : log) if (!a.is_write && bases.count(a.word_addr)) v.push_back({a.cycle, a.word_addr});
    return v;
}

// Complete fields begin after the first restart (restarts[1]; restarts[0]
// is the run's first fetch, not a restart).
std::vector<FieldSpan> complete_fields(const std::vector<Sample>& trusted, const std::vector<Restart>& restarts) {
    std::vector<FieldSpan> out;
    if (restarts.size() < 2) return out;
    uint64_t first_restart = restarts[1].cycle;
    auto wraps = trace_wraps(trusted);
    std::vector<size_t> kept;
    for (auto idx : wraps) if (trusted[idx].cyc >= first_restart) kept.push_back(idx);
    for (size_t k = 0; k + 1 < kept.size(); ++k) out.push_back({kept[k], kept[k + 1]});
    return out;
}

// Restart-bounded field windows: window k is [restarts[k+1], restarts[k+2]).
std::vector<RWin> restart_windows(const std::vector<Restart>& restarts) {
    std::vector<RWin> out;
    for (size_t k = 0; k + 2 < restarts.size(); ++k) out.push_back({restarts[k + 1].cycle, restarts[k + 2].cycle, restarts[k + 1].base});
    return out;
}

struct R1Check { bool ok = true; size_t violations = 0, spans = 0; };

R1Check check_r1(const std::vector<Sample>& trusted, const std::vector<Restart>& restarts) {
    R1Check r;
    auto wraps = trace_wraps(trusted);
    for (size_t k = 0; k + 1 < wraps.size(); ++k) {
        uint64_t lo = trusted[wraps[k]].cyc, hi = trusted[wraps[k + 1]].cyc;
        size_t cnt = 0;
        for (size_t ri = 1; ri < restarts.size(); ++ri) if (restarts[ri].cycle >= lo && restarts[ri].cycle < hi) ++cnt;
        ++r.spans;
        if (cnt != 1) { r.ok = false; ++r.violations; }
    }
    if (r.spans == 0) r.ok = false;
    return r;
}

// ---- L1: the Copper's first fetches read what the CPU wrote ----------------
bool check_l1(const std::vector<nmh::RamAccess>& log, uint32_t base_w, uint16_t e0, uint16_t e1) {
    std::optional<uint64_t> w0, w1, r0, r0d, r1, r1d, w0c, w1c;
    for (auto& a : log) {
        if (a.is_write && a.word_addr == base_w && !w0) { w0 = a.data; w0c = a.cycle; }
        if (a.is_write && a.word_addr == base_w + 1 && !w1) { w1 = a.data; w1c = a.cycle; }
        if (!a.is_write && a.word_addr == base_w && !r0) { r0 = a.cycle; r0d = a.data; }
        if (!a.is_write && a.word_addr == base_w + 1 && !r1) { r1 = a.cycle; r1d = a.data; }
    }
    return w0 && w1 && r0d && r1d && *r0 > *w0c && *r1 > *w1c && *w0 == *r0d && *w1 == *r1d && *w0 == e0 && *w1 == e1;
}

bool check_fixture(const std::vector<nmh::RamAccess>& log) {
    for (auto& a : log) if (a.is_write && a.word_addr == kFixtureW && a.data == kFixtureValue) return true;
    return false;
}

// ---- the lists as the CPU wrote them -----------------------------------------
struct Instr {
    uint32_t a; uint16_t w1, w2;
    bool wait() const { return (w1 & 1) != 0; }
    int v() const { return w1 >> 8; }
    int h() const { return w1 & 0xFE; }
};

std::map<uint32_t, uint16_t> list_memory(const std::vector<nmh::RamAccess>& raw) {
    std::map<uint32_t, uint16_t> m;
    for (auto& a : raw)
        if (a.is_write && a.word_addr >= kListBaseW && a.word_addr < kListBaseW + kListWords) m[a.word_addr] = a.data;
    return m;
}

// The words the CPU wrote from base on, two per instruction, up to the first word it did not write.
// WAIT($FF,$FE) has the words of END (p.23), so the decoding does not stop at them.
std::vector<Instr> decode_list(const std::map<uint32_t, uint16_t>& mem, uint32_t base) {
    std::vector<Instr> out;
    uint32_t a = base;
    for (int guard = 0; guard < 4000; ++guard, a += 2) {
        auto i1 = mem.find(a), i2 = mem.find(a + 1);
        if (i1 == mem.end() || i2 == mem.end()) break;
        out.push_back({a, i1->second, i2->second});
    }
    return out;
}

// marker data -> address of the marker's first word (a MOVE to $1FE)
std::unordered_map<uint16_t, uint32_t> marker_map(const std::vector<Instr>& list) {
    std::unordered_map<uint16_t, uint32_t> m;
    for (auto& i : list) if (i.w1 == 0x01FE && !m.count(i.w2)) m[i.w2] = i.a;
    return m;
}

// ---- Copper fetches ------------------------------------------------------------
using ReadIndex = std::unordered_map<uint32_t, std::vector<uint64_t>>;

ReadIndex build_read_index(const std::vector<nmh::RamAccess>& log) {
    ReadIndex idx;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.word_addr < kListBaseW || a.word_addr >= kListBaseW + kListWords) continue;
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

// ---- one analysed trace run ---------------------------------------------------------
struct Analysis {
    Capture cap;
    SlotRef sref;
    std::vector<Sample> all, trusted;
    RF2Precond rf2;
    std::vector<Restart> restarts;
    std::vector<FieldSpan> fields;
    std::vector<RWin> rwins;
    R1Check r1;
    bool l1 = false, fixture = false;
    ReadIndex ridx;
    bool s1_spacing = false, s1_collision = false;
    size_t s1_violations = 0;
    std::map<uint32_t, uint16_t> mem;
    int std_class = -1;      // 0 PAL, 1 NTSC, -1 neither or mixed
    bool std_known = false;
};

Analysis analyze(Capture&& cap, const std::set<uint32_t>& bases, uint16_t l1w0, uint16_t l1w1) {
    Analysis a;
    a.cap = std::move(cap);
    a.sref = measure_slots(a.cap.raw);
    a.all = decode_trace(a.sref, a.cap.log);
    a.trusted = trusted_of(a.all);
    a.rf2 = check_rf2(a.all, a.trusted);
    a.restarts = restart_events(a.cap.log, bases);
    a.fields = complete_fields(a.trusted, a.restarts);
    a.rwins = restart_windows(a.restarts);
    a.r1 = check_r1(a.trusted, a.restarts);
    a.l1 = check_l1(a.cap.log, *bases.begin(), l1w0, l1w1);
    a.fixture = check_fixture(a.cap.log);
    a.ridx = build_read_index(a.cap.log);
    a.s1_spacing = a.sref.ok && a.sref.spacing_consistent;
    a.s1_collision = s1_no_collision(a.sref, a.cap.log, &a.s1_violations);
    a.mem = list_memory(a.cap.raw);
    int cls = -2;
    for (auto& s : a.all) {
        int c = std_of_id(s.id);
        if (cls == -2) cls = c; else if (cls != c) { cls = -1; break; }
    }
    a.std_class = cls == -2 ? -1 : cls;
    a.std_known = a.std_class >= 0;
    return a;
}

// "Line of an event" (SPECIFICATION.md RF2). trusted is sorted by w. The
// sample before s+ in the full trace must be written at or after t: that is
// the clause of T1 that the identification rests on.
struct LineId { int line = -1; int status = 1; }; // 0 identified, 1 outside the trace, 2 samples disagree, 3 step too long

LineId line_of(const Analysis& a, int64_t t) {
    LineId r;
    auto it = std::upper_bound(a.trusted.begin(), a.trusted.end(), t,
        [](int64_t val, const Sample& s) { return val < s.w; });
    if (it == a.trusted.begin()) return r;
    const Sample& sminus = *(it - 1);
    auto it2 = std::lower_bound(a.trusted.begin(), a.trusted.end(), t + 16,
        [](const Sample& s, int64_t val) { return s.w < val; });
    if (it2 == a.trusted.end()) return r;
    const Sample& splus = *it2;
    if (splus.idx == 0) return r;
    const Sample& prev = a.all[splus.idx - 1];
    if (prev.w < t) { r.status = 3; return r; }
    if (sminus.V != splus.V) { r.status = 2; return r; }
    r.line = sminus.V; r.status = 0;
    return r;
}

std::optional<int64_t> fetch_slot(const Analysis& a, uint32_t addr, const RWin& w) {
    auto fc = fetch_in(a.ridx, addr, w.lo, w.hi);
    if (!fc) return std::nullopt;
    return slot_of(a.sref, *fc);
}

// delay(W) of SPECIFICATION.md 3.5 for the WAIT whose second word is at
// wait_a + 1: the slot of the fetch of the word after it, minus the slot of
// the fetch of that second word.
std::optional<int64_t> wait_delay(const Analysis& a, uint32_t wait_a, const RWin& w) {
    auto s2 = fetch_slot(a, wait_a + 1, w);
    auto nx = fetch_slot(a, wait_a + 2, w);
    if (!s2 || !nx) return std::nullopt;
    return *nx - *s2;
}

// lambda_min of a window: the delay of the WAIT($00,$00) at the head of the
// list that runs in it.
std::optional<int64_t> lambda_min(const Analysis& a, const RWin& w) {
    return wait_delay(a, w.base, w);
}

struct WaitedResult { bool all = true; size_t checked = 0, expected = 0; };

// Every WAIT of `list` that satisfies pred has a delay above lambda_min, in
// every window whose list base is list_base.
template <class Pred>
WaitedResult waited_all(const Analysis& a, const std::vector<Instr>& list, uint32_t list_base, Pred pred) {
    WaitedResult r;
    for (auto& w : a.rwins) {
        if (w.base != list_base) continue;
        auto lam = lambda_min(a, w);
        for (auto& i : list) {
            if (!i.wait() || i.a == list_base || (i.w1 == 0xFFFF && i.w2 == 0xFFFE) || !pred(i)) continue;
            ++r.expected;
            auto d = wait_delay(a, i.a, w);
            if (!d || !lam) { r.all = false; continue; }
            ++r.checked;
            if (*d <= *lam) r.all = false;
        }
    }
    if (r.expected == 0 || r.checked < r.expected) r.all = false;
    return r;
}

// ---- common preconditions of every trace run ------------------------------------
struct Gate {
    bool fix = false, done = false, s1 = false, l1 = false, t1e = false, t2 = false, r1 = false, fields = false;
    bool core() const { return fix && done && s1 && l1 && t1e && t2 && r1 && fields; }
    bool trace() const { return fix && done && s1 && l1 && t1e && t2; }
    bool counts() const { return fix && done && s1 && l1 && t2; }
    std::string why_not_core() const {
        std::string s;
        if (!fix) s += " FIX";
        if (!done) s += " PRE-DONE";
        if (!s1) s += " PRE-S1";
        if (!l1) s += " PRE-L1";
        if (!t1e) s += " PRE-T1E";
        if (!t2) s += " PRE-T2";
        if (!r1) s += " PRE-R1";
        if (!fields) s += " PRE-FIELDS";
        return s;
    }
    std::string why_not_counts() const {
        std::string s;
        if (!fix) s += " FIX";
        if (!done) s += " PRE-DONE";
        if (!s1) s += " PRE-S1";
        if (!l1) s += " PRE-L1";
        if (!t2) s += " PRE-T2";
        return s;
    }
    std::string why_not_trace() const {
        std::string s;
        if (!fix) s += " FIX";
        if (!done) s += " PRE-DONE";
        if (!s1) s += " PRE-S1";
        if (!l1) s += " PRE-L1";
        if (!t1e) s += " PRE-T1E";
        if (!t2) s += " PRE-T2";
        return s;
    }
};

// gap_hits: the number of events of this run whose line identification
// failed because the step before s+ ended before the event (the clause of
// T1 the identification rests on).
Gate report_common(Checks& checks, const Analysis& a, Tag tag, const std::string& run, size_t min_fields,
                   size_t gap_hits, size_t events_identified) {
    Gate g;
    const std::string tg = tagname(tag);
    const std::string rn = " run " + run + ": ";
    checks.expect(a.cap.loaded && a.cap.released, "FIX-LOAD " + tg + rn + "program loaded and CPU released [not graded -- apparatus check, no claim uid]");
    checks.expect(a.fixture, "FIX-CONST " + tg + rn + "the known word $A5C3 stored by the program to $0C0002 appears on the Chip-memory port with that data [not graded -- apparatus check, no claim uid]");
    g.fix = a.cap.loaded && a.cap.released && a.fixture;
    g.done = a.cap.done.has_value();
    checks.expect(g.done, "PRE-DONE " + tg + rn + "the completion word was written");
    checks.expect(a.s1_spacing, "PRE-S1A " + tg + rn + "refresh spacing 2S is constant through the run (RF1)");
    checks.note("S1 " + tg + rn + "S=" + num(a.sref.S) + " clk_sys cycles/slot, " + num((long long)a.sref.groups_found) + " refresh groups measured");
    checks.expect(a.s1_collision, "PRE-S1B " + tg + rn + "no two logged accesses occupy the same slot (RF1)");
    g.s1 = a.s1_spacing && a.s1_collision;
    g.l1 = a.l1;
    checks.expect(a.l1, "PRE-L1 " + tg + rn + "the Copper's first fetches read the words the CPU wrote in step 3");
    checks.expect(a.rf2.t1, "PRE-T1 " + tg + rn + "every consecutive sample pair is at most 16 slots apart (RF2)");
    checks.note("T1 " + tg + rn + num((long long)a.rf2.t1_violations) + " of " + num((long long)a.rf2.t1_total) +
                " consecutive-sample gaps exceed 16 slots, " + num((long long)a.rf2.t1_at_boundary) + " of them across a field boundary");
    g.t1e = gap_hits == 0 && events_identified > 0;
    checks.expect(g.t1e, "PRE-T1E " + tg + rn + "no event of this run is bracketed by a step that ended before it (the clause of T1 that RF2's line identification uses): " +
                  num((long long)gap_hits) + " events hit such a step, " + num((long long)events_identified) + " events identified");
    g.t2 = a.rf2.t2;
    checks.expect(a.rf2.t2, "PRE-T2 " + tg + rn + "the beam counter is live between trusted samples (RF2)");
    checks.note("T2 " + tg + rn + num((long long)a.rf2.t2_violations) + " of " + num((long long)a.rf2.t2_total) +
                " trusted-sample transitions violate T2");
    g.r1 = a.r1.ok;
    checks.expect(a.r1.ok, "PRE-R1 " + tg + rn + "exactly one restart between consecutive field boundaries (RF3)");
    checks.note("R1 " + tg + rn + num((long long)a.r1.violations) + " of " + num((long long)a.r1.spans) +
                " inter-boundary spans have restart count != 1");
    g.fields = a.fields.size() >= min_fields;
    checks.expect(g.fields, "PRE-FIELDS " + tg + rn + "at least " + num((long long)min_fields) + " complete fields traced (measured " + num((long long)a.fields.size()) + ")");
    bool std_ok = a.std_class == (int)tag;
    checks.expect(std_ok, "PRE-STD " + tg + rn + "the chip identification in VPOSR bits 14-8 of every sample names " + tg + " (p.299)");
    return g;
}

// ---- field length of a trace ---------------------------------------------------------
std::set<int> field_lengths(const Analysis& a) {
    std::set<int> lens;
    for (auto& f : a.fields) {
        std::set<int> vs;
        for (size_t i = f.lo; i < f.hi; ++i) vs.insert(a.trusted[i].V);
        lens.insert((int)vs.size());
    }
    return lens;
}

std::string setstr(const std::set<int>& s) {
    std::string o; for (int v : s) o += (o.empty() ? "" : ",") + std::to_string(v); return o.empty() ? "(none)" : o;
}
std::string setstr64(const std::set<int64_t>& s) {
    std::string o; for (auto v : s) o += (o.empty() ? "" : ",") + std::to_string(v); return o.empty() ? "(none)" : o;
}

// OCS cross-check of SPECIFICATION.md 3.1: recorded, not graded.
void ocs_check(Checks& checks, const std::string& dir, const std::string& bin, Tag tag, const std::string& run,
               const Analysis& ecs, const std::set<uint32_t>& bases, uint16_t w0, uint16_t w1, uint64_t budget) {
    Analysis ocs = analyze(run_capture(dir + "/" + bin + ".bin", cfg_ocs(tag), budget), bases, w0, w1);
    auto e = field_lengths(ecs), o = field_lengths(ocs);
    checks.note("OCS-CHECK " + std::string(tagname(tag)) + " run " + run + ": ECS field-length set {" + setstr(e) + "} vs OCS {" + setstr(o) +
                "} -- " + (e == o ? "match, assumption checked" : "MISMATCH, recorded not graded"));
}

// ==== SECTION A ===============================================================
struct APlan { bool run[2] = {true, true}; std::string bin[2]; uint8_t cfg[2]; bool label_swap = false; };

struct ALists {
    std::vector<Instr> list;
    std::unordered_map<uint16_t, uint32_t> mk;
    std::optional<uint32_t> m(uint16_t d) const { auto it = mk.find(d); if (it == mk.end()) return std::nullopt; return it->second; }
};

struct AEvents {
    size_t identified = 0, gap_hits = 0, mismatches = 0, missing = 0;
};

// the line of marker data d in window w; counts the event in ev
LineId marker_line(const Analysis& a, const ALists& L, uint16_t d, const RWin& w, AEvents& ev) {
    LineId none;
    auto ma = L.m(d);
    if (!ma) { ++ev.missing; return none; }
    auto fs = fetch_slot(a, *ma, w);
    if (!fs) { ++ev.missing; return none; }
    LineId r = line_of(a, *fs);
    if (r.status == 0) ++ev.identified; else if (r.status == 3) ++ev.gap_hits; else ++ev.missing;
    return r;
}

struct P2Res {
    bool complete = true, all_227 = true, in_set = true, alt = true;
    size_t measured = 0, expected = 0;
    std::set<int64_t> values;
    std::vector<std::unordered_map<int, int64_t>> lines;   // per window: line -> L(line)
};

// L(v) = time(m_{v+1}) - time(m_v) for v = $30 ... $FE, and for the tail
// pairs of lines 256 + n, n = 0 ... tail_pairs-1 (SPECIFICATION.md P2).
P2Res grade_p2(const Analysis& a, const ALists& L, int tail_pairs) {
    P2Res r;
    for (auto& w : a.rwins) {
        std::unordered_map<int, int64_t> lm;
        std::vector<int64_t> main_part, tail_part;
        for (int v = 0x30; v < 0xFF; ++v) {
            ++r.expected;
            auto ma = L.m((uint16_t)v), mb = L.m((uint16_t)(v + 1));
            std::optional<int64_t> t0, t1;
            if (ma) t0 = fetch_slot(a, *ma, w);
            if (mb) t1 = fetch_slot(a, *mb, w);
            if (!t0 || !t1) { r.complete = false; main_part.push_back(INT64_MIN); continue; }
            int64_t l = *t1 - *t0;
            ++r.measured; r.values.insert(l); lm[v] = l; main_part.push_back(l);
        }
        for (int n = 0; n < tail_pairs; ++n) {
            ++r.expected;
            auto ma = L.m((uint16_t)(0x100 + n)), mb = L.m((uint16_t)(0x101 + n));
            std::optional<int64_t> t0, t1;
            if (ma) t0 = fetch_slot(a, *ma, w);
            if (mb) t1 = fetch_slot(a, *mb, w);
            if (!t0 || !t1) { r.complete = false; tail_part.push_back(INT64_MIN); continue; }
            int64_t l = *t1 - *t0;
            ++r.measured; r.values.insert(l); lm[256 + n] = l; tail_part.push_back(l);
        }
        for (auto* part : {&main_part, &tail_part}) {
            for (size_t i = 0; i < part->size(); ++i) {
                int64_t l = (*part)[i];
                if (l == INT64_MIN) continue;
                if (l != 227) r.all_227 = false;
                if (l != 227 && l != 228) r.in_set = false;
                if (i + 1 < part->size() && (*part)[i + 1] != INT64_MIN && (*part)[i + 1] == l) r.alt = false;
            }
        }
        r.lines.push_back(lm);
    }
    if (a.rwins.empty()) r.complete = false;
    return r;
}

int window_of(const Analysis& a, uint64_t cyc) {
    for (size_t k = 0; k < a.rwins.size(); ++k) if (cyc >= a.rwins[k].lo && cyc < a.rwins[k].hi) return (int)k;
    return -1;
}

struct P3Res {
    bool graded = true, ok = true, set_ok = true, e3_ok = true, odd_end_ok = false;
    std::set<int> hs;
    size_t lines_long = 0, lines_short = 0;
    std::vector<int> missing_even, missing_odd;
};

// P3: the horizontal count range (SPECIFICATION.md P3). The graded set is
// the lowest count, every even count up to $E2, the highest count the page
// gives, and nothing above it; the odd counts are measured (the sampler
// reads no odd count in part of the line, see the specification's build notes).
P3Res grade_p3(const Analysis& a, Tag expect, const P2Res& p2) {
    P3Res r;
    for (auto& s : a.all) r.hs.insert(s.H);
    const int maxH = expect == PAL ? 0xE2 : 0xE3;
    // line types near the end of a line (NTSC)
    std::set<std::pair<int, int>> long_lines, short_lines;
    bool e3_on_short = false;
    for (auto& s : a.trusted) {
        int k = window_of(a, s.cyc);
        if (k < 0 || k >= (int)p2.lines.size()) continue;
        auto it = p2.lines[k].find(s.V);
        if (it == p2.lines[k].end()) continue;
        if (s.H >= 0xE0) {
            if (it->second == 228) long_lines.insert({k, s.V});
            else if (it->second == 227) short_lines.insert({k, s.V});
        }
        if (s.H == 0xE3 && it->second == 227) e3_on_short = true;
    }
    r.lines_long = long_lines.size(); r.lines_short = short_lines.size();
    r.e3_ok = !e3_on_short;
    if (r.hs.empty()) { r.graded = false; r.ok = false; r.set_ok = false; return r; }
    bool within = true;
    for (int h : r.hs) if (h > maxH) within = false;
    bool even_all = true;
    for (int x = 0; x <= 0xE2; x += 2) if (!r.hs.count(x)) { even_all = false; r.missing_even.push_back(x); }
    for (int x = 1; x <= 0xE1; x += 2) if (!r.hs.count(x)) r.missing_odd.push_back(x);
    r.odd_end_ok = r.hs.count(0xE1) > 0;
    r.set_ok = within && even_all && r.hs.count(maxH) > 0;
    r.ok = r.set_ok && (expect == PAL || r.e3_ok);
    return r;
}

void section_A(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    APlan plan;
    plan.bin[PAL] = "prog_a"; plan.bin[NTSC] = "prog_a_ntsc";
    plan.cfg[PAL] = kECS_PAL; plan.cfg[NTSC] = kECS_NTSC;
    if (!normal) {
        // mv is "A-<VARIANT>-<PAL|NTSC>" or one of the standard swaps
        plan.run[PAL] = plan.run[NTSC] = false;
        if (mv == "A-STD-NTSC-LABEL-PALCAP") {            // the PAL capture graded under the NTSC label
            plan.run[NTSC] = true; plan.bin[NTSC] = "prog_a"; plan.cfg[NTSC] = kECS_PAL;
        } else if (mv == "A-STD-P8") {                    // the NTSC list on a PAL machine
            plan.run[NTSC] = true; plan.bin[NTSC] = "prog_a_ntsc"; plan.cfg[NTSC] = kECS_PAL;
        } else {
            std::string rest = mv.substr(2);              // "<VARIANT>-<PAL|NTSC>"
            size_t dash = rest.rfind('-');
            std::string var = rest.substr(0, dash), tg = rest.substr(dash + 1);
            for (auto& ch : var) ch = (char)std::tolower((unsigned char)ch);
            Tag t = tg == "NTSC" ? NTSC : PAL;
            plan.run[t] = true;
            plan.bin[t] = "prog_a_" + var + (t == NTSC ? "_ntsc" : "");
        }
    }
    const std::set<uint32_t> bases = {kListBaseW};
    for (int ti = 0; ti < 2; ++ti) {
        if (!plan.run[ti]) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        const bool nodone = plan.bin[ti].find("nodone") != std::string::npos;
        Analysis a = analyze(run_capture(dir + "/" + plan.bin[ti] + ".bin", plan.cfg[ti], nodone ? kNoDoneBudget : kRunBudgetA), bases, 0x0001, 0xFFFE);
        ALists L; L.list = decode_list(a.mem, kListBaseW); L.mk = marker_map(L.list);

        // ---- events, computed once -----------------------------------------
        AEvents ev;
        size_t anchor_checked = 0, anchor_bad = 0;
        for (auto& w : a.rwins) {
            for (int v = 0x30; v <= 0xFF; ++v) {
                if (!L.m((uint16_t)v)) continue;
                LineId r = marker_line(a, L, (uint16_t)v, w, ev);
                if (r.status == 3) continue;
                ++anchor_checked;
                if (r.status != 0 || r.line != v) ++anchor_bad;
            }
        }
        const int tail_main = tag == PAL ? 55 : 5;            // P2 pairs of lines 256 + n
        const int p7_max = tag == PAL ? 55 : 5;               // P7: n = 0 ... p7_max
        size_t p7_expected = 0, p7_identified = 0, p7_bad = 0;
        for (auto& w : a.rwins) {
            for (int n = 0; n <= p7_max; ++n) {
                ++p7_expected;
                if (!L.m((uint16_t)(0x100 + n))) continue;
                LineId r = marker_line(a, L, (uint16_t)(0x100 + n), w, ev);
                if (r.status == 3) continue;
                if (r.status != 0) continue;
                ++p7_identified;
                if (r.line != 256 + n) ++p7_bad;
            }
        }
        // m_E0 reached, and the line of the second word of WAIT(0,$60)
        size_t e0_reached = 0, w256_ok = 0, w256_checked = 0;
        auto e0 = L.m(0xA0E0);
        auto t0m = L.m(0x100);
        for (auto& w : a.rwins) {
            if (e0 && fetch_slot(a, *e0, w)) ++e0_reached;
            if (t0m) {
                auto fs = fetch_slot(a, *t0m - 1, w);
                if (fs) {
                    LineId r = line_of(a, *fs);
                    if (r.status == 3) ++ev.gap_hits;
                    else { ++w256_checked; if (r.status == 0 && r.line == 256) ++w256_ok; }
                }
            }
        }
        auto p2waited = waited_all(a, L.list, kListBaseW, [](const Instr& i) {
            return (i.h() == 0x40 && i.v() >= 0x30) || (i.h() == 0x60 && i.v() <= 0x37);
        });

        // ---- preconditions ---------------------------------------------------
        Gate g = report_common(checks, a, tag, "A", 6, ev.gap_hits, ev.identified);
        const bool anchor_ok = anchor_checked > 0 && anchor_bad == 0;
        checks.expect(anchor_ok, "PRE-ANCHOR " + tg + " run A: every m_v (v=$30..$FF) lies on line v (RF2), " +
                      num((long long)(anchor_checked - anchor_bad)) + "/" + num((long long)anchor_checked) + " matched");
        checks.expect(p2waited.all, "PRE-P2WAITED " + tg + " run A: every WAIT(v,$40) and every WAIT(n,$60) waited (delay above lambda_min), " +
                      num((long long)p2waited.checked) + "/" + num((long long)p2waited.expected) + " measured");
        const bool e0_ok = e0 && !a.rwins.empty() && e0_reached == a.rwins.size();
        checks.expect(e0_ok, "PRE-E0 " + tg + " run A: m_E0 (after WAIT($FF,$E0)) is fetched in every field (" +
                      num((long long)e0_reached) + "/" + num((long long)a.rwins.size()) + ")");
        const bool p7pre_ok = w256_checked > 0 && w256_ok == w256_checked && w256_checked == a.rwins.size();
        checks.expect(p7pre_ok, "PRE-P7L256 " + tg + " run A: the second word of the WAIT before m_256 is fetched on line 256 (RF2), " +
                      num((long long)w256_ok) + "/" + num((long long)a.rwins.size()) + " fields");
        const bool base_ok = g.core() && anchor_ok && p2waited.all;

        // ---- P1: field length (NOT GRADED) and origin ----------------------------
        const std::string fl = setstr(field_lengths(a));
        checks.note("P1-COUNT " + tg + " run A: distinct V values per complete field {" + fl + "} over " + num((long long)a.fields.size()) + " complete fields");
        if (tag == PAL) {
            checks.record("P1-COUNT PAL NOT GRADED COP2-vertical-beam-position.c4 COP2-using-the-copper-in-interlaced-mode.c4: the pages give two "
                          "field lengths. p.24: \"There are actually 262 NTSC (312 PAL) possible vertical positions.\" p.34: \"PAL has 312 lines normally\". "
                          "p.27: \"to the end of the display (226,262) NTSC (or (226,312) PAL)\", positions counted from (0,0), which are 263 and 313. "
                          "p.216 Table 7-5: V8 \"Allows PAL line counts (313) to appear in PAL versions of the Amiga.\"");
        } else {
            checks.record("P1-COUNT NTSC NOT GRADED COP2-vertical-beam-position.c3 COP2-using-the-copper-in-interlaced-mode.c2: the pages give two "
                          "field lengths. p.24: \"There are actually 262 NTSC (312 PAL) possible vertical positions.\" p.34: \"a normal NTSC display has 262 lines\". "
                          "p.27: \"to the end of the display (226,262) NTSC (or (226,312) PAL)\", positions counted from (0,0), which are 263 and 313. "
                          "p.216 Table 7-5 gives PAL only: V8 \"Allows PAL line counts (313) to appear in PAL versions of the Amiga.\"");
        }
        {
            bool even = false, odd = false, h0 = false;
            for (auto& s : a.all) { if (s.H & 1) odd = true; else even = true; if (s.H == 0) h0 = true; }
            const bool cover = even && odd;
            bool first_v0 = !a.fields.empty();
            for (auto& f : a.fields) if (a.trusted[f.lo].V != 0) first_v0 = false;
            bool lowest_present = false;
            const int lowest = cover ? 0 : (odd ? 1 : 0);
            for (auto& s : a.all) if (s.H == lowest) lowest_present = true;
            checks.note("P1-ORIGIN " + tg + " run A: parity coverage " + std::string(cover ? "yes" : "no") + " (even " + std::string(even ? "seen" : "absent") +
                        ", odd " + std::string(odd ? "seen" : "absent") + ")");
            if (!g.core()) {
                checks.record("P1-ORIGIN " + tg + " NOT GRADED COP2-putting-together-a-copper-instruction-list.c7: precondition failed:" + g.why_not_core());
            } else if (cover || !odd) {
                checks.expect(first_v0 && (cover ? h0 : lowest_present),
                              "P1-ORIGIN " + tg + " COP2-putting-together-a-copper-instruction-list.c7 [p.27]: every complete field's first trusted sample has V=0, and H=0 occurs in the trace");
            } else {
                checks.expect(first_v0 && lowest_present,
                              "P1-ORIGIN " + tg + " [p.27]: every complete field's first trusted sample has V=0, and the lowest H of the observed odd parity occurs in the trace");
                checks.record("P1-ORIGIN " + tg + " COP2-putting-together-a-copper-instruction-list.c7 not asserted whole: only odd counts are read, so count 0 cannot be read");
            }
        }

        // ---- P2 -------------------------------------------------------------------
        P2Res p2 = grade_p2(a, L, tail_main);
        checks.record("P2 " + tg + " run A: L(v) values measured {" + setstr64(p2.values) + "} over " + num((long long)p2.measured) + " of " + num((long long)p2.expected) + " pairs");
        if (tag == PAL) {
            if (!base_ok || !p2.complete) {
                checks.record("P2-PAL NOT GRADED COP2-horizontal-beam-position.c15: precondition or population failed:" + g.why_not_core() +
                              std::string(anchor_ok ? "" : " PRE-ANCHOR") + std::string(p2waited.all ? "" : " PRE-P2WAITED") + std::string(p2.complete ? "" : " population-incomplete"));
            } else {
                checks.expect(p2.all_227, "P2-PAL COP2-horizontal-beam-position.c15 [p.23,p.24]: L(v)=227 for every measured v in every complete field");
            }
        } else {
            if (!base_ok || !p2.complete) {
                checks.record("P2-NTSC NOT GRADED COP2-horizontal-beam-position.c10 COP2-horizontal-beam-position.c11 COP2-horizontal-beam-position.c12 "
                              "COP2-horizontal-beam-position.c14 COP2-vertical-beam-position.c11: precondition or population failed:" + g.why_not_core() +
                              std::string(anchor_ok ? "" : " PRE-ANCHOR") + std::string(p2waited.all ? "" : " PRE-P2WAITED") + std::string(p2.complete ? "" : " population-incomplete"));
            } else {
                checks.expect(p2.in_set && p2.alt,
                              "P2-NTSC COP2-horizontal-beam-position.c10 COP2-horizontal-beam-position.c11 COP2-horizontal-beam-position.c12 "
                              "COP2-horizontal-beam-position.c14 COP2-vertical-beam-position.c11 [p.23,p.24]: L(v) in {227,228} for every measured v, and "
                              "L(v)!=L(v+1) for every measured consecutive pair, in every complete field");
            }
        }

        // ---- P3 ---------------------------------------------------------------------
        {
            P3Res p3 = grade_p3(a, tag, p2);
            std::string hs; { int lo = 999, hi = -1; for (int h : p3.hs) { lo = std::min(lo, h); hi = std::max(hi, h); } hs = p3.hs.empty() ? "(none)" : hex2(lo) + ".." + hex2(hi) + ", " + num((long long)p3.hs.size()) + " distinct"; }
            std::string mo, me; { int c = 0; for (int x : p3.missing_odd) { if (c++ < 40) mo += " " + hex2(x); } c = 0; for (int x : p3.missing_even) { if (c++ < 40) me += " " + hex2(x); } }
            checks.note("P3 " + tg + " run A: H values observed " + hs + "; even values not observed (" + num((long long)p3.missing_even.size()) + "):" + (me.empty() ? std::string(" none") : me) +
                        "; odd values up to $E1 not observed (" + num((long long)p3.missing_odd.size()) + "):" + (mo.empty() ? std::string(" none") : mo) +
                        "; lines with H>=$E0 and L=228: " + num((long long)p3.lines_long) + ", with L=227: " + num((long long)p3.lines_short));
            const bool cov_ok = p3.lines_long >= 20 && p3.lines_short >= 20;
            if (tag == NTSC) {
                checks.expect(p3.odd_end_ok, "PRE-P3ODDEND NTSC run A: the odd count $E1, next below the last even count, is read at the end of a line");
                checks.expect(cov_ok, "PRE-P3COVER NTSC run A: H >= $E0 is observed on at least 20 distinct lines with L(v)=228 and at least 20 with L(v)=227 (" +
                              num((long long)p3.lines_long) + " and " + num((long long)p3.lines_short) + ")");
            }
            if (!g.counts()) {
                checks.record("P3-" + tg + " NOT GRADED: precondition failed:" + g.why_not_counts());
            } else if (tag == PAL) {
                checks.expect(p3.set_ok, "P3-PAL [p.23,p.27]: every H is at most $E2, and every even value from $00 to $E2 is observed");
            } else if (!p3.odd_end_ok) {
                checks.record("P3-NTSC-SET NOT GRADED: precondition failed: PRE-P3ODDEND");
            } else {
                checks.expect(p3.set_ok, "P3-NTSC-SET [p.23]: every H is at most $E3, and every even value from $00 to $E2 and the odd value $E3 are observed");
                const bool pop_ok = p2.complete && anchor_ok && p2waited.all;
                if (!(cov_ok && pop_ok)) {
                    checks.record("P3-NTSC-LONGONLY NOT GRADED COP2-horizontal-beam-position.c13: precondition failed:" +
                                  std::string(cov_ok ? "" : " PRE-P3COVER") + std::string(pop_ok ? "" : " P2-population-or-anchor-or-waited"));
                } else {
                    checks.expect(p3.e3_ok, "P3-NTSC-LONGONLY [p.23]: no sample with H=$E3 lies on a line with L(v)=227");
                }
            }
            if (tag == NTSC) checks.record("P3-NTSC COP2-horizontal-beam-position.c13 not asserted whole: the sampler reads no odd count in part of the line (" + num((long long)p3.missing_odd.size()) +
                                           " odd values up to $E1 are never read), so \"0-$E3\" is graded over the even counts, the lowest count and $E3 only");
        }

        // ---- P7 -----------------------------------------------------------------------
        {
            auto tm = L.m(0x100);
            std::optional<int64_t> dly; std::optional<int64_t> lam;
            bool waited = !a.rwins.empty(); size_t wchecked = 0;
            for (auto& w : a.rwins) {
                lam = lambda_min(a, w);
                if (!tm) { waited = false; break; }
                dly = wait_delay(a, *tm - 2, w);
                if (!dly || !lam) { waited = false; continue; }
                ++wchecked;
                if (*dly <= *lam) waited = false;
            }
            const bool pre = g.core() && anchor_ok && e0_ok && p7pre_ok;
            const bool pop = p7_identified == p7_expected;
            const std::string why = g.why_not_core() + std::string(anchor_ok ? "" : " PRE-ANCHOR") + std::string(e0_ok ? "" : " PRE-E0") + std::string(p7pre_ok ? "" : " PRE-P7L256");
            if (!pre || !pop) {
                checks.record("P7 " + tg + " NOT GRADED COP2-the-wait-instruction.c22 COP2-the-wait-instruction.c23 COP2-vertical-beam-position.c7: precondition or population failed:" + why +
                              std::string(pop ? "" : " population-incomplete (" + num((long long)p7_identified) + "/" + num((long long)p7_expected) + ")"));
                checks.record("P7-WAITED " + tg + " NOT GRADED: precondition failed:" + why);
            } else {
                checks.expect(p7_bad == 0, "P7 " + tg + " COP2-the-wait-instruction.c22 COP2-the-wait-instruction.c23 COP2-vertical-beam-position.c7 [p.23,p.24]: every m_(256+n) lies on line 256+n for n=0.." + num(p7_max));
                checks.expect(waited && wchecked == a.rwins.size(), "P7-WAITED " + tg + " [p.22]: WAIT($00,$60) waited (its delay exceeds lambda_min), " + num((long long)wchecked) + " fields");
            }
        }

        // ---- P8 (NTSC) --------------------------------------------------------------------
        if (tag == NTSC) {
            bool lines_ok = true; size_t lines_checked = 0;
            for (auto& w : a.rwins) {
                for (int n = 0; n <= 5; ++n) {
                    AEvents dummy;
                    LineId r = marker_line(a, L, (uint16_t)(0x100 + n), w, dummy);
                    ++lines_checked;
                    if (r.status != 0 || r.line != 256 + n) lines_ok = false;
                }
            }
            bool after_ok = true; size_t after_checked = 0;
            std::map<int, int> after_v;
            if (!a.fields.empty()) {
                for (size_t i = a.fields.front().lo; i + 1 < a.fields.back().hi && i + 1 < a.trusted.size(); ++i) {
                    if (a.trusted[i].V == 261 && a.trusted[i + 1].V != 261) {
                        ++after_checked; after_v[a.trusted[i + 1].V]++;
                        if (a.trusted[i + 1].V != 0) after_ok = false;
                    }
                }
            }
            if (after_checked < a.fields.size()) after_ok = false;
            bool no262 = true; size_t no262_checked = 0;
            auto m261 = L.m(0x105); auto m262 = L.m(0x106);
            for (auto& w : a.rwins) {
                if (!m261) break;
                auto f261 = fetch_in(a.ridx, *m261, w.lo, w.hi);
                if (!f261) continue;
                ++no262_checked;
                if (m262 && fetch_in(a.ridx, *m262, *f261, w.hi)) no262 = false;
            }
            std::string av; for (auto& kv : after_v) av += (av.empty() ? "" : ",") + std::to_string(kv.first) + "x" + std::to_string(kv.second);
            checks.note("P8 " + tg + " run A: the trusted sample after the last sample of line 261 has V (count): " + (av.empty() ? std::string("(none)") : av) + " over " + num((long long)after_checked) + " field boundaries");
            const bool pre = g.core() && anchor_ok && e0_ok && p7pre_ok;
            const std::string why = g.why_not_core() + std::string(anchor_ok ? "" : " PRE-ANCHOR") + std::string(e0_ok ? "" : " PRE-E0") + std::string(p7pre_ok ? "" : " PRE-P7L256");
            if (!pre || lines_checked == 0 || no262_checked == 0) {
                checks.record("P8 NTSC NOT GRADED COP2-vertical-beam-position.c9: precondition or population failed:" + why);
            } else {
                checks.expect(lines_ok, "P8-LINES NTSC [p.24]: m_(256+n) lies on line 256+n for n=0..5, every complete field");
                checks.expect(after_ok, "P8-AFTER NTSC [p.24,p.219]: the trusted sample after line 261 is on line 0, every complete field");
                checks.expect(no262, "P8-NO262 NTSC [p.24]: m_262 is not fetched between m_261 and the next field boundary, every complete field");
                checks.expect(lines_ok && after_ok && no262,
                              "P8 NTSC COP2-vertical-beam-position.c9 [p.24]: vertical 0 through 5 after line 255 are the last six lines, 256 to 261, and there is no line 262");
            }
        }

        if (normal) ocs_check(checks, dir, plan.bin[ti], tag, "A", a, bases, 0x0001, 0xFFFE, kRunBudgetA);
    }
}

// ==== SECTION C: horizontal position sweep (P4) =========================================
struct CPlan { bool run[2] = {true, true}; std::string bin[2]; uint8_t cfg[2]; };

CPlan make_plan(const std::string& prefix, const std::string& mv, bool normal) {
    CPlan p;
    p.bin[PAL] = prefix; p.bin[NTSC] = prefix + "_ntsc";
    p.cfg[PAL] = kECS_PAL; p.cfg[NTSC] = kECS_NTSC;
    if (!normal) {
        p.run[PAL] = p.run[NTSC] = false;
        std::string rest = mv.substr(2);                  // "<VARIANT>-<PAL|NTSC>"
        size_t dash = rest.rfind('-');
        std::string var = rest.substr(0, dash), tg = rest.substr(dash + 1);
        for (auto& ch : var) ch = (char)std::tolower((unsigned char)ch);
        Tag t = tg == "NTSC" ? NTSC : PAL;
        p.run[t] = true;
        p.bin[t] = prefix + "_" + var + (t == NTSC ? "_ntsc" : "");
    }
    return p;
}

void section_C(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    CPlan plan = make_plan("prog_c", mv, normal);
    const std::set<uint32_t> bases = {kListBaseW};
    for (int ti = 0; ti < 2; ++ti) {
        if (!plan.run[ti]) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        const bool nodone = plan.bin[ti].find("nodone") != std::string::npos;
        Analysis a = analyze(run_capture(dir + "/" + plan.bin[ti] + ".bin", plan.cfg[ti], nodone ? kNoDoneBudget : kRunBudgetC), bases, 0x0001, 0xFFFE);
        ALists L; L.list = decode_list(a.mem, kListBaseW); L.mk = marker_map(L.list);

        // ---- events -----------------------------------------------------------------
        AEvents ev;
        size_t r1_checked = 0, r1_bad = 0;
        size_t a_expected = 0, a_measured = 0, a_bad = 0;
        size_t b_expected = 0, b_measured = 0;
        bool b_strict = true;
        size_t c_expected = 0, c_measured = 0, c_bad = 0;
        std::map<int, std::set<int64_t>> off_by_k;             // D_k - (h_k - $10), k = 105..113
        std::map<int, std::set<std::string>> cdet;             // j -> E_j and the line of r2_j
        for (auto& w : a.rwins) {
            std::map<int, int64_t> D;
            for (int k = 1; k <= 113; ++k) {
                auto mm = L.m((uint16_t)(0xC100 + k)), mr = L.m((uint16_t)(0xC200 + k));
                std::optional<int64_t> tm, tr;
                if (mm) tm = fetch_slot(a, *mm, w);
                if (mr) tr = fetch_slot(a, *mr, w);
                if (tm && tr) D[k] = *tm - *tr;
            }
            for (int k = 1; k <= 104; ++k) {
                ++a_expected;
                auto it = D.find(k);
                if (it == D.end()) continue;
                ++a_measured;
                const int h = 2 * k, href = h <= 0x80 ? 0xC0 : 0x10;
                if (it->second != h - href) ++a_bad;
            }
            for (int k = 105; k <= 113; ++k) {
                ++b_expected;
                auto it = D.find(k);
                if (it == D.end()) continue;
                ++b_measured;
                off_by_k[k].insert(it->second - (2 * k - 0x10));
                if (k > 105 && D.count(k - 1) && !(D[k - 1] < it->second)) b_strict = false;
            }
            for (int j = 0; j < 14; ++j) {
                auto m1 = L.m((uint16_t)(0xC300 + j)), mq = L.m((uint16_t)(0xC400 + j)), m2 = L.m((uint16_t)(0xC500 + j));
                if (m1) {
                    auto fs = fetch_slot(a, *m1, w);
                    if (fs) {
                        LineId r = line_of(a, *fs);
                        if (r.status == 3) ++ev.gap_hits; else { ++r1_checked; if (r.status == 0) ++ev.identified; if (r.status != 0 || r.line != 0xA4 + 2 * j) ++r1_bad; }
                    }
                }
                ++c_expected;
                if (!mq || !m2) continue;
                auto tq = fetch_slot(a, *mq, w), t2 = fetch_slot(a, *m2, w);
                if (!tq || !t2) continue;
                LineId r = line_of(a, *t2);
                if (r.status == 3) { ++ev.gap_hits; continue; }
                if (r.status == 0) ++ev.identified;
                ++c_measured;
                cdet[j].insert("E=" + num(*t2 - *tq) + " line " + (r.status == 0 ? num(r.line) : std::string("?")));
                auto lam2 = lambda_min(a, w);
                auto d2 = wait_delay(a, *m2 - 2, w);
                const bool r2waited = d2 && lam2 && *d2 > *lam2;
                if (r.status != 0 || r.line != 0xA4 + 2 * j + 1 || !r2waited) ++c_bad;
            }
        }
        // the WAITs under test: the first-word and second-word WAITs of each line (markers $C1xx and $C2xx) and WAIT(w_j,g_j) (marker $C4xx)
        std::unordered_map<uint32_t, uint16_t> mdata;
        for (auto& i : L.list) if (i.w1 == 0x01FE) mdata[i.a] = i.w2;
        auto waited = waited_all(a, L.list, kListBaseW, [&](const Instr& i) {
            auto it = mdata.find(i.a + 2);
            if (it == mdata.end()) return false;
            const int hi = it->second >> 8;
            return hi == 0xC1 || hi == 0xC2 || hi == 0xC4;
        });

        // ---- preconditions --------------------------------------------------------------
        Gate g = report_common(checks, a, tag, "C", 3, ev.gap_hits, ev.identified);
        const size_t r1_expected = a.rwins.size() * 14;
        const bool r1_ok = r1_checked == r1_expected && r1_expected > 0 && r1_bad == 0;
        checks.expect(r1_ok, "PRE-C-R1LINE " + tg + " run C: r1_j lies on line w_j for every j, every complete field (RF2), " +
                      num((long long)(r1_checked - r1_bad)) + "/" + num((long long)r1_expected) + " matched");
        checks.expect(waited.all, "PRE-C-WAITED " + tg + " run C: every WAIT under test (the first-word and second-word WAITs of each line, and WAIT(w_j,g_j)) waited (delay above lambda_min), " +
                      num((long long)waited.checked) + "/" + num((long long)waited.expected) + " measured");
        const bool pre = g.core() && r1_ok && waited.all;
        const std::string why = g.why_not_core() + std::string(r1_ok ? "" : " PRE-C-R1LINE") + std::string(waited.all ? "" : " PRE-C-WAITED");

        // ---- P4 -------------------------------------------------------------------------------
        std::string offs;
        for (auto& kv : off_by_k) {
            offs += " k=" + num(kv.first) + ":";
            bool first = true;
            for (auto v : kv.second) { offs += (first ? "" : "/") + num(v); first = false; }
        }
        checks.note("P4 " + tg + " run C: D_k - (h_k - $10) for h_k = $D2..$E2 (reading A predicts 0 or 1, reading B -1 or 0):" + (offs.empty() ? std::string(" (none)") : offs));
        { std::string cd; for (auto& kv : cdet) { cd += " j=" + num(kv.first) + "(w=$" + hex2(0xA4 + 2 * kv.first) + ", expected line " + num(0xA4 + 2 * kv.first + 1) + "):"; bool f = true; for (auto& x : kv.second) { cd += (f ? "{" : "/") + x; f = false; } cd += "}"; }
          checks.note("P4-C " + tg + " run C: E_j and the line of r2_j per j:" + (cd.empty() ? std::string(" (none)") : cd)); }
        checks.record("P4-C " + tg + " run C: E_j = time(r2_j) - time(q_j) = $40 is not graded: where in the first counts of a line the Copper's next fetch falls is left open by the pages (p.20: the Copper's cycles do not collide with refresh, which occupies slots at the start of every line; SPECIFICATION.md 7, item 14); E_j is measured per j above");
        const bool pop = a_measured == a_expected && b_measured == b_expected && c_measured == c_expected && a_expected > 0;
        if (!pre || !pop) {
            checks.record("P4 " + tg + " NOT GRADED COP2-horizontal-beam-position.c3 COP2-vertical-beam-position.c23: precondition or population failed:" + why +
                          std::string(pop ? "" : " population-incomplete (a " + num((long long)a_measured) + "/" + num((long long)a_expected) + ", b " +
                                      num((long long)b_measured) + "/" + num((long long)b_expected) + ", c " + num((long long)c_measured) + "/" + num((long long)c_expected) + ")"));
        } else {
            const bool oka = a_bad == 0, okb = b_strict, okc = c_bad == 0;
            checks.expect(oka, "P4-A " + tg + " [p.22,p.23,p.24,p.275]: D_k = h_k - H_ref for every k with h_k <= $D0 (k=1..104), every complete field");
            checks.expect(okb, "P4-B " + tg + " [p.23,p.32]: D_k strictly increases with k for h_k = $D2..$E2 (k=105..113), every complete field");
            checks.expect(okc, "P4-C " + tg + " [p.23,p.30]: r2_j lies on line w_j+1 and its WAIT(w_j+1,$40) waited, so the WAIT for a position beyond $E2 on line w_j ended before count $40 of line w_j+1, for every j=0..13, every complete field");
            checks.expect(oka && okb && okc, "P4 " + tg + " COP2-horizontal-beam-position.c3 COP2-vertical-beam-position.c23 [p.23,p.24,p.275]: the Copper's horizontal "
                          "comparison advances one count per memory cycle, gives 113 positions $02..$E2, and has no position beyond $E2");
        }
        if (normal) ocs_check(checks, dir, plan.bin[ti], tag, "C", a, bases, 0x0001, 0xFFFE, kRunBudgetC);
    }
}

// ==== SECTION B: WAIT on line $FF beyond $E2 (P6) =====================================================
constexpr uint32_t kListInitW = kListBaseW;
constexpr uint32_t kListFEW   = 0x010200 / 2;
uint32_t list_b_base(int j) { return (0x010400 + 0x100 * j) / 2; }

void section_B(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    CPlan plan = make_plan("prog_b", mv, normal);
    std::set<uint32_t> bases = {kListInitW, kListFEW};
    for (int j = 0; j < 16; ++j) bases.insert(list_b_base(j));
    for (int ti = 0; ti < 2; ++ti) {
        if (!plan.run[ti]) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        Analysis a = analyze(run_capture(dir + "/" + plan.bin[ti] + ".bin", plan.cfg[ti], kRunBudgetB), bases, 0x0080, 0x0001);
        std::map<uint32_t, ALists> lists;
        for (uint32_t b : bases) { ALists l; l.list = decode_list(a.mem, b); l.mk = marker_map(l.list); lists[b] = l; }

        // every Copper fetch in the list region, in time order
        std::vector<uint64_t> allf;
        for (auto& kv : a.ridx) for (auto c : kv.second) allf.push_back(c);
        std::sort(allf.begin(), allf.end());

        // window i runs the list L_FE (i = 0), L_0 (i = 1), ...; the chain the lists name
        std::vector<uint32_t> expect_base;
        expect_base.push_back(kListFEW);
        for (size_t i = 0; i < a.rwins.size(); ++i) {
            int j = (int)i - 1;
            if (j < 0) continue;
            expect_base.push_back(list_b_base(j > 15 ? 15 : j));
        }
        bool chain_ok = a.rwins.size() >= 19;
        for (size_t i = 0; i < a.rwins.size() && i < expect_base.size(); ++i) if (a.rwins[i].base != expect_base[i]) chain_ok = false;
        size_t l15_windows = 0;
        for (auto& w : a.rwins) if (w.base == list_b_base(15)) ++l15_windows;
        chain_ok = chain_ok && l15_windows >= 3;

        AEvents ev;
        size_t l255_expected = 0, l255_ok = 0;           // PRE-B-L255
        size_t a_expected = 0, a_checked = 0, a_bad = 0; // P6-LINE255
        size_t waited_expected = 0, waited_ok = 0;       // PRE-B-WAITED
        bool x0_reached = false, x0_seen = false;
        int64_t x0_delta = 0; int x0_line = -1;
        bool larger_never = true; size_t larger_checked = 0, larger_expected = 0;
        bool fe_never = true; size_t fe_checked = 0, fe_windows = 0;
        std::string l255_detail;
        std::map<std::string, size_t> never_count;
        bool ctl_ok = false, ctl_seen = false; int64_t ctl_delta = 0;
        for (size_t wi = 0; wi < a.rwins.size(); ++wi) {
            const RWin& w = a.rwins[wi];
            const ALists& L = lists[w.base];
            int j = -1;
            if (w.base == kListFEW) j = -1;
            else for (int q = 0; q < 16; ++q) if (w.base == list_b_base(q)) j = q;
            if (w.base == kListFEW) {
                auto m0 = L.m(0xB300), m1 = L.m(0xB301);
                if (m0 && m1) {
                    auto t0 = fetch_slot(a, *m0, w), t1 = fetch_slot(a, *m1, w);
                    auto lamfe = lambda_min(a, w);
                    auto dfe = wait_delay(a, *m1 - 2, w);
                    ctl_seen = true;
                    if (t0 && t1) { ctl_delta = *t1 - *t0; ctl_ok = ctl_delta > 0 && dfe && lamfe && *dfe > *lamfe; }
                }
                continue;
            }
            if (j < 0) continue;
            const uint16_t da = (uint16_t)(0xB100 + (j == 15 ? 14 : j));        // L_15 is identical to L_14
            const uint16_t dx = (uint16_t)(0xB200 + (j == 15 ? 14 : j));
            auto ma = L.m(da), mx = L.m(dx);
            auto lam = lambda_min(a, w);
            ++l255_expected; ++a_expected; ++waited_expected;
            if (!ma || !mx) continue;
            auto tw = fetch_slot(a, *mx - 1, w);              // second word of the WAIT before x_j
            auto ta = fetch_slot(a, *ma, w);
            if (tw) {
                LineId r = line_of(a, *tw);
                if (r.status == 3) ++ev.gap_hits; else { if (r.status == 0) ++ev.identified; if (r.status == 0 && r.line == 255) ++l255_ok; }
                if (!(r.status == 0 && r.line == 255)) l255_detail += " window " + num((long long)wi) + " (L_" + num(j) + "): line " + (r.status == 0 ? num(r.line) : std::string("?(status ") + num(r.status) + ")");
            } else l255_detail += " window " + num((long long)wi) + " (L_" + num(j) + "): second word not fetched";
            if (ta) {
                LineId r = line_of(a, *ta);
                if (r.status == 3) ++ev.gap_hits; else { if (r.status == 0) ++ev.identified; ++a_checked; if (r.status != 0 || r.line != 255) ++a_bad; }
                auto d = wait_delay(a, *ma - 2, w);
                if (d && lam && *d > *lam) ++waited_ok;
            }
            // the fetch of x_j, and every Copper fetch after the WAIT's second word
            auto fx = fetch_in(a.ridx, *mx, w.lo, w.hi);
            size_t after = 0;
            if (tw) {
                auto fs2 = fetch_in(a.ridx, *mx - 1, w.lo, w.hi);
                if (fs2) for (auto it = std::upper_bound(allf.begin(), allf.end(), *fs2); it != allf.end() && *it < w.hi; ++it) ++after;
            }
            if (j == 0) {
                x0_seen = true;
                x0_reached = fx.has_value() && ta && slot_of(a.sref, *fx) > *ta;
                if (fx && ta) { x0_delta = slot_of(a.sref, *fx) - *ta; auto lx = line_of(a, slot_of(a.sref, *fx)); x0_line = lx.status == 0 ? lx.line : -2; }
            } else {
                ++larger_expected;
                if (tw) {
                    ++larger_checked;
                    if (fx || after > 0) larger_never = false;
                    const std::string key = std::string(fx ? "x_j fetched " + num(slot_of(a.sref, w.hi) - slot_of(a.sref, *fx)) + " slots before the next restart" : "x_j not fetched") +
                                            ", " + num((long long)after) + " Copper fetches after the WAIT's second word";
                    ++never_count[key];
                }
            }
            if (j == 14 || j == 15) {
                ++fe_windows;
                if (tw) { ++fe_checked; if (fx || after > 0) fe_never = false; }
            }
        }

        // ---- preconditions -------------------------------------------------------------------------
        Gate g = report_common(checks, a, tag, "B", 19, ev.gap_hits, ev.identified);
        checks.expect(chain_ok, "PRE-B-CHAIN " + tg + " run B: each restart fetches the list that the previous list named (L_FE, L_0 .. L_15, then L_15 again), at least 3 fields of L_15 (" +
                      num((long long)a.rwins.size()) + " fields, " + num((long long)l15_windows) + " of L_15)");
        const bool l255 = l255_expected > 0 && l255_ok == l255_expected;
        checks.expect(l255, "PRE-B-L255 " + tg + " run B: the second word of each WAIT($FF,h_j) is fetched on line 255 (RF2), " + num((long long)l255_ok) + "/" + num((long long)l255_expected) + " fields");
        if (!l255_detail.empty()) checks.note("PRE-B-L255 " + tg + " run B: fields not on line 255:" + l255_detail);
        const bool wt = waited_expected > 0 && waited_ok == waited_expected;
        checks.expect(wt, "PRE-B-WAITED " + tg + " run B: every WAIT($FF,$40) waited (delay above lambda_min), " + num((long long)waited_ok) + "/" + num((long long)waited_expected) + " fields");
        const bool pre = g.core() && chain_ok && l255 && wt;
        const std::string why = g.why_not_core() + std::string(chain_ok ? "" : " PRE-B-CHAIN") + std::string(l255 ? "" : " PRE-B-L255") + std::string(wt ? "" : " PRE-B-WAITED");

        // ---- P6 ----------------------------------------------------------------------------------------
        std::string rec = "j=0: x_0 " + std::string(x0_reached ? "fetched" : "not fetched") + " in the same field";
        if (x0_seen && x0_reached) rec += ", time(x_0)-time(a_0) = " + num(x0_delta) + " slots, " + (x0_line >= 0 ? "on line " + num(x0_line) : std::string("on a line the trace does not identify")) + " (reading A predicts $A2 or $A3 = 162 or 163, reading B $A1 or $A2 = 161 or 162, p.23 and P4(b))";
        std::string nd;
        for (auto& kv : never_count) nd += " " + num((long long)kv.second) + " of " + num((long long)larger_checked) + " fields: " + kv.first + ";";
        checks.note("P6 " + tg + " run B: " + rec + "; fields of h_j > $E2:" + (nd.empty() ? std::string(" none") : nd));
        if (!pre || a_checked != a_expected) {
            checks.record("P6 " + tg + " NOT GRADED COP2-the-wait-instruction.c26 COP2-the-wait-instruction.c27 COP2-the-wait-instruction.c28 COP2-the-wait-instruction.c29: precondition or population failed:" + why +
                          std::string(a_checked == a_expected ? "" : " population-incomplete"));
        } else {
            checks.expect(a_bad == 0, "P6-LINE255 " + tg + " COP2-the-wait-instruction.c27 [p.23]: a_j lies on line 255 in every field of every list L_0 .. L_15");
            checks.expect(x0_seen && x0_reached && larger_checked == larger_expected && larger_never,
                          "P6-LARGEST " + tg + " COP2-the-wait-instruction.c26 [p.23]: $FFE2 is reached (x_0 fetched in the same field, after a_0) and no larger value is (x_j not fetched for h_j = $E4 .. $FE)");
            checks.expect(fe_never && fe_windows >= 4 && fe_checked == fe_windows,
                          "P6-NEVER " + tg + " COP2-the-wait-instruction.c28 COP2-the-wait-instruction.c29 [p.23,p.22]: for $FFFE, x_j is never fetched and no Copper fetch occurs before the next restart, in every one of the at least 4 fields with h_j = $FE");
            if (ctl_seen) checks.expect(ctl_ok, "P6-CONTROL " + tg + " [p.23]: WAIT($FE,$FE) ends when line $FF begins: $B300 is fetched in the same field and the WAIT($FF,$40) after it waited (time($B301)-time($B300) = " + num(ctl_delta) + " slots; $40 is not graded, for the reason of P4-C)");
            else checks.record("P6-CONTROL " + tg + " NOT GRADED: the L_FE field was not measured");
        }
        if (normal) ocs_check(checks, dir, plan.bin[ti], tag, "B", a, bases, 0x0080, 0x0001, kRunBudgetB);
    }
}

// ==== SECTIONS D and E: video (P5, P9) ==========================================================
struct VRun { int col; int len; };

// RF4: the runs of one output line, taken cyclically (a run touching both the start and the end counts once)
std::vector<VRun> line_runs(const std::vector<nmh::VideoSample>& v, nmh::Span ln) {
    std::vector<VRun> r;
    for (size_t i = ln.begin; i < ln.end && i < v.size(); ++i) {
        int c = (v[i].r << 8) | (v[i].g << 4) | v[i].b;
        if (!r.empty() && r.back().col == c) ++r.back().len; else r.push_back({c, 1});
    }
    if (r.size() > 1 && r.front().col == r.back().col) { r.front().len += r.back().len; r.pop_back(); }
    return r;
}

struct VideoSet {
    std::vector<std::vector<std::vector<VRun>>> frames;   // [complete frame][output line] runs
    size_t complete = 0;
    bool r1_ok = true; size_t r1_bad = 0;
};

// complete output frames after the first restart; R1 per frame
VideoSet make_video_set(const Analysis& a) {
    VideoSet vs;
    const auto& v = a.cap.video;
    auto frames = nmh::find_frames(v);
    auto lines = nmh::find_lines(v);
    if (a.restarts.size() < 2) { vs.r1_ok = false; return vs; }
    const uint64_t first = a.restarts[1].cycle;
    size_t li = 0;
    for (size_t f = 0; f + 1 < frames.size(); ++f) {
        if (frames[f].begin < first) continue;
        size_t cnt = 0;
        for (size_t ri = 1; ri < a.restarts.size(); ++ri) if (a.restarts[ri].cycle >= frames[f].begin && a.restarts[ri].cycle < frames[f].end) ++cnt;
        if (cnt != 1) { vs.r1_ok = false; ++vs.r1_bad; }
        std::vector<std::vector<VRun>> fl;
        while (li < lines.size() && lines[li].begin < frames[f].begin) ++li;
        for (size_t k = li; k < lines.size() && lines[k].end <= frames[f].end; ++k) fl.push_back(line_runs(v, lines[k]));
        vs.frames.push_back(std::move(fl));
    }
    vs.complete = vs.frames.size();
    if (vs.complete == 0) vs.r1_ok = false;
    return vs;
}

struct VGate { bool fix = false, done = false, s1 = false, l1 = false, r1 = false, fields = false;
    bool ok() const { return fix && done && s1 && l1 && r1 && fields; }
    std::string why() const {
        std::string s;
        if (!fix) s += " FIX";
        if (!done) s += " PRE-DONE";
        if (!s1) s += " PRE-S1";
        if (!l1) s += " PRE-L1";
        if (!r1) s += " PRE-R1";
        if (!fields) s += " PRE-FIELDS";
        return s;
    }
};

int results_id(const Analysis& a) {
    for (auto& x : a.cap.log) if (x.is_write && x.word_addr == kResultsW) return (x.data >> 8) & 0x7F;
    return -1;
}

VGate report_common_video(Checks& checks, const Analysis& a, const VideoSet& vs, Tag tag, const std::string& run) {
    VGate g;
    const std::string tg = tagname(tag);
    const std::string rn = " run " + run + ": ";
    checks.expect(a.cap.loaded && a.cap.released, "FIX-LOAD " + tg + rn + "program loaded and CPU released [not graded -- apparatus check, no claim uid]");
    checks.expect(a.fixture, "FIX-CONST " + tg + rn + "the known word $A5C3 stored by the program to $0C0002 appears on the Chip-memory port with that data [not graded -- apparatus check, no claim uid]");
    g.fix = a.cap.loaded && a.cap.released && a.fixture;
    g.done = a.cap.done.has_value();
    checks.expect(g.done, "PRE-DONE " + tg + rn + "the completion word was written");
    checks.expect(a.s1_spacing, "PRE-S1A " + tg + rn + "refresh spacing 2S is constant through the run (RF1)");
    checks.note("S1 " + tg + rn + "S=" + num(a.sref.S) + " clk_sys cycles/slot, " + num((long long)a.sref.groups_found) + " refresh groups measured");
    checks.expect(a.s1_collision, "PRE-S1B " + tg + rn + "no two logged accesses occupy the same slot (RF1)");
    g.s1 = a.s1_spacing && a.s1_collision;
    g.l1 = a.l1;
    checks.expect(a.l1, "PRE-L1 " + tg + rn + "the Copper's first fetches read the words the CPU wrote in step 3");
    g.r1 = vs.r1_ok;
    checks.expect(vs.r1_ok, "PRE-R1 " + tg + rn + "exactly one restart between consecutive field boundaries (RF3), " + num((long long)vs.r1_bad) + " of " + num((long long)vs.complete) + " output fields without");
    g.fields = vs.complete >= 3;
    checks.expect(g.fields, "PRE-FIELDS " + tg + rn + "at least 3 complete fields traced (measured " + num((long long)vs.complete) + ")");
    const int id = results_id(a);
    checks.expect(std_of_id(id) == (int)tag, "PRE-STD " + tg + rn + "the chip identification in VPOSR bits 14-8 names " + tg + " (p.299), measured $" + hex2(id < 0 ? 0xFF : id));
    return g;
}

constexpr int kWhite = 0xFFF, kBlue = 0x00F, kYellow = 0xFF0, kGreen = 0x0F0;

struct Calib { bool valid = false; size_t lines = 0, bad = 0; std::set<int> lens; int plen = 0; };

// D-lo / D-hi: every output line that shows COLOR01 has nwhite COLOR01 runs, and all of them and the nwhite-1 COLOR00 runs
// between them have one common length. P is that length, common to all lines.
Calib calibrate(const VideoSet& vs, int white, int blue, size_t nwhite) {
    Calib c;
    for (auto& fr : vs.frames) for (auto& runs : fr) {
        size_t first = runs.size(), last = 0, nw = 0;
        for (size_t i = 0; i < runs.size(); ++i) if (runs[i].col == white) { if (first == runs.size()) first = i; last = i; ++nw; }
        if (nw == 0) continue;
        ++c.lines;
        bool ok = nw == nwhite && (last - first + 1) == 2 * nwhite - 1;
        int len = runs[first].len;
        for (size_t i = first; ok && i <= last; ++i) {
            const bool want_white = ((i - first) % 2) == 0;
            if (runs[i].col != (want_white ? white : blue)) ok = false;
            if (runs[i].len != len) ok = false;
        }
        if (!ok) ++c.bad; else c.lens.insert(len);
    }
    c.valid = c.lines > 0 && c.bad == 0 && c.lens.size() == 1;
    if (c.valid) c.plen = *c.lens.begin();
    return c;
}

struct SweepRes {
    bool edges_ok = true; size_t lines_with_a = 0, bad_lines = 0;
    std::set<int> present_all;                        // u present in every complete frame
    std::map<int, std::vector<int>> widths;           // u -> widths found
    size_t frames = 0;
};

SweepRes analyze_sweep(const VideoSet& vs) {
    SweepRes r;
    r.frames = vs.complete;
    std::map<int, size_t> frames_with;
    for (auto& fr : vs.frames) {
        std::set<int> seen;
        for (auto& runs : fr) {
            std::vector<size_t> ai;
            for (size_t i = 0; i < runs.size(); ++i) if ((runs[i].col >> 8) == 0xF && ((runs[i].col >> 4) & 0xF) <= 8) ai.push_back(i);
            if (ai.empty()) continue;
            ++r.lines_with_a;
            if (ai.size() != 1) { r.edges_ok = false; ++r.bad_lines; continue; }
            size_t i = ai[0];
            const int u = 0x40 + (runs[i].col & 0xFF);
            const bool nb = i > 0 && i + 1 < runs.size() && runs[i - 1].col == kGreen && runs[i + 1].col == kGreen;
            if (!nb) { r.edges_ok = false; ++r.bad_lines; continue; }
            seen.insert(u);
            r.widths[u].push_back(runs[i].len);
        }
        for (int u : seen) ++frames_with[u];
    }
    for (int u = 0x40; u <= 0xCB; ++u) if (frames_with[u] == vs.complete && vs.complete > 0) r.present_all.insert(u); else r.edges_ok = false;
    return r;
}

int sweep_n(int u) { return u <= 0x85 ? u - 0x38 : 77 - (u - 0x86); }

// one standard-screen output frame set: window lines (those that show COLOR01)
struct EGrade { size_t window_lines = 0; size_t bad_a = 0, bad_b = 0, bad_one0 = 0, windows_per_frame_min = 0; bool frames_have_lines = true; std::set<int> c1_lens; };

EGrade grade_e(const VideoSet& vs, int c1, int c0, int plo) {
    EGrade e;
    e.windows_per_frame_min = (size_t)-1;
    for (auto& fr : vs.frames) {
        size_t wl = 0;
        for (auto& runs : fr) {
            size_t n1 = 0, iw = 0;
            for (size_t i = 0; i < runs.size(); ++i) if (runs[i].col == c1) { ++n1; iw = i; }
            if (n1 == 0) continue;
            ++wl; ++e.window_lines;
            // (a): exactly one COLOR01 run of 320 * P_lo; every other run COLOR00 or 0
            bool a_ok = n1 == 1 && runs[iw].len == 320 * plo;
            e.c1_lens.insert(runs[iw].len);
            for (auto& r : runs) if (r.col != c1 && r.col != c0 && r.col != 0) a_ok = false;
            if (!a_ok) ++e.bad_a;
            // exactly one run of 0
            size_t nz = 0; for (auto& r : runs) if (r.col == 0) ++nz;
            if (nz != 1) ++e.bad_one0;
            // (b): after the COLOR01 run, cyclically: COLOR00, 0, COLOR00 and nothing else
            bool b_ok = false;
            if (n1 == 1 && runs.size() == 4 && c0 != 0) {
                const int a1 = runs[(iw + 1) % 4].col, a2 = runs[(iw + 2) % 4].col, a3 = runs[(iw + 3) % 4].col;
                b_ok = a1 == c0 && a2 == 0 && a3 == c0;
            }
            if (!b_ok) ++e.bad_b;
        }
        e.windows_per_frame_min = std::min(e.windows_per_frame_min, wl);
    }
    if (vs.frames.empty()) e.windows_per_frame_min = 0;
    return e;
}

struct DPlan { bool lo = true, hi = true, sweep = true, e = true, ew = true;
    std::string b_lo = "prog_dlo", b_hi = "prog_dhi", b_sw = "prog_dsweep", b_e = "prog_e", b_ew = "prog_ewidefetch"; };

struct DEData { Analysis a; VideoSet vs; };

DEData load_de(const std::string& dir, const std::string& bin, Tag tag, bool sweep, uint16_t w0, uint16_t w1) {
    DEData d;
    const std::set<uint32_t> bases = {kListBaseW};
    d.a = analyze(run_capture(dir + "/" + bin + (tag == NTSC ? "_ntsc" : "") + ".bin", cfg_ecs(tag), kRunBudgetDE, true), bases, w0, w1);
    (void)sweep;
    d.vs = make_video_set(d.a);
    return d;
}

void section_DE(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    DPlan plan;
    Tag only = PAL; bool one_tag = false;
    if (!normal) {
        plan.lo = plan.hi = plan.sweep = plan.e = plan.ew = false;
        std::string rest = mv.substr(2);                  // "<VARIANT>-<PAL|NTSC>"
        size_t dash = rest.rfind('-');
        std::string var = rest.substr(0, dash), tg = rest.substr(dash + 1);
        for (auto& ch : var) ch = (char)std::tolower((unsigned char)ch);
        only = tg == "NTSC" ? NTSC : PAL; one_tag = true;
        if (var == "dshift" || var == "ddrop" || var == "dlate") { plan.lo = plan.hi = plan.sweep = true; plan.b_sw = "prog_" + var; }
        else if (var == "dflatlo") { plan.lo = true; plan.b_lo = "prog_dflatlo"; }
        else if (var == "dflathi") { plan.hi = true; plan.b_hi = "prog_dflathi"; }
        else if (var == "ewidewin" || var == "eblack" || var == "eblackgap" || var == "enobpl") { plan.lo = plan.e = plan.ew = true; plan.b_e = "prog_" + var; }
        else if (var == "ewwidewin") { plan.lo = plan.e = plan.ew = true; plan.b_ew = "prog_ewidewin"; }
        else if (var == "ploflat") { plan.lo = plan.e = plan.ew = true; plan.b_lo = "prog_dflatlo"; }
        else { std::printf("unknown variant %s\n", var.c_str()); std::exit(2); }
    }
    for (int ti = 0; ti < 2; ++ti) {
        if (one_tag && (Tag)ti != only) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        Calib clo, chi; bool have_lo = false, have_hi = false;
        if (plan.lo) {
            DEData d = load_de(dir, plan.b_lo, tag, false, 0x00E0, 0x0002);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "D-lo");
            clo = calibrate(d.vs, kWhite, kBlue, 160); have_lo = true;
            checks.note("P5 " + tg + " D-lo: " + num((long long)clo.lines) + " window lines, " + num((long long)clo.bad) + " without 160 COLOR01 runs of one common length with the COLOR00 runs between; lengths {" + setstr(clo.lens) + "}");
            checks.expect(g.ok() && clo.valid, "PRE-P5-CALLO " + tg + " D-lo: every window line shows 160 COLOR01 runs, and all COLOR01 runs and the 159 COLOR00 runs between them have one common length P_lo (" + num(clo.plen) + ")");
            if (!g.ok()) checks.record("PRE-P5-CALLO " + tg + " preconditions failed:" + g.why());
        }
        if (plan.hi) {
            DEData d = load_de(dir, plan.b_hi, tag, false, 0x00E0, 0x0002);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "D-hi");
            chi = calibrate(d.vs, kWhite, kBlue, 320); have_hi = true;
            checks.note("P5 " + tg + " D-hi: " + num((long long)chi.lines) + " window lines, " + num((long long)chi.bad) + " without 320 COLOR01 runs of one common length with the COLOR00 runs between; lengths {" + setstr(chi.lens) + "}");
            checks.expect(g.ok() && chi.valid, "PRE-P5-CALHI " + tg + " D-hi: every window line shows 320 COLOR01 runs, and all COLOR01 runs and the 319 COLOR00 runs between them have one common length P_hi (" + num(chi.plen) + ")");
            if (!g.ok()) checks.record("PRE-P5-CALHI " + tg + " preconditions failed:" + g.why());
        }
        if (plan.sweep) {
            DEData d = load_de(dir, plan.b_sw, tag, true, 0x0001, 0xFFFE);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "D-sweep");
            std::vector<Instr> list = decode_list(d.a.mem, kListBaseW);
            auto waited = waited_all(d.a, list, kListBaseW, [](const Instr&) { return true; });
            SweepRes sw = analyze_sweep(d.vs);
            checks.expect(sw.edges_ok, "PRE-P5-EDGES " + tg + " D-sweep: A_u appears for every sweep line u in every complete field, and every output line that shows A_u shows exactly one run of it with COLOR00 ($0F0) before and after it (" +
                          num((long long)sw.bad_lines) + " of " + num((long long)sw.lines_with_a) + " output lines not so, " + num((long long)sw.present_all.size()) + " of 140 sweep lines present in every field)");
            checks.expect(waited.all, "PRE-P5-WAITED " + tg + " D-sweep: every sweep WAIT waited (delay above lambda_min), " + num((long long)waited.checked) + "/" + num((long long)waited.expected) + " measured");
            const bool pre = g.ok() && sw.edges_ok && waited.all;
            const std::string why = g.why() + std::string(sw.edges_ok ? "" : " PRE-P5-EDGES") + std::string(waited.all ? "" : " PRE-P5-WAITED");
            std::string lo_w, hi_w;
            if (!pre) {
                checks.record("P5 " + tg + " NOT GRADED COP2-horizontal-beam-position.c4 ch2-horizontal-beam-position.c4 COP2-horizontal-beam-position.c5 ch2-horizontal-beam-position.c5: precondition failed:" + why);
            } else {
                size_t lo_bad = 0, hi_bad = 0, total = 0;
                std::set<int> wset;
                for (auto& kv : sw.widths) for (int wv : kv.second) {
                    ++total; wset.insert(wv);
                    if (have_lo && clo.valid && wv != 4 * sweep_n(kv.first) * clo.plen) ++lo_bad;
                    if (have_hi && chi.valid && wv != 8 * sweep_n(kv.first) * chi.plen) ++hi_bad;
                }
                checks.note("P5 " + tg + " D-sweep: " + num((long long)total) + " A_u runs measured; P_lo=" + num(have_lo ? clo.plen : -1) + ", P_hi=" + num(have_hi ? chi.plen : -1));
                if (have_lo && clo.valid) {
                    checks.expect(lo_bad == 0, "P5-LO " + tg + " COP2-horizontal-beam-position.c4 ch2-horizontal-beam-position.c4 [p.23]: w_u = 4 * n_u * P_lo for every sweep line u, every output line, every complete field (" + num((long long)lo_bad) + " of " + num((long long)total) + " differ)");
                } else checks.record("P5-LO " + tg + " NOT GRADED COP2-horizontal-beam-position.c4 ch2-horizontal-beam-position.c4: P_lo is not valid (PRE-P5-CALLO)");
                if (have_hi && chi.valid) {
                    checks.expect(hi_bad == 0, "P5-HI " + tg + " COP2-horizontal-beam-position.c5 ch2-horizontal-beam-position.c5 [p.23]: w_u = 8 * n_u * P_hi for every sweep line u, every output line, every complete field (" + num((long long)hi_bad) + " of " + num((long long)total) + " differ)");
                } else checks.record("P5-HI " + tg + " NOT GRADED COP2-horizontal-beam-position.c5 ch2-horizontal-beam-position.c5: P_hi is not valid (PRE-P5-CALHI)");
            }
        }
        if (plan.e) {
            DEData d = load_de(dir, plan.b_e, tag, false, 0x00E0, 0x0002);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "E");
            const int c0 = (plan.b_e == "prog_eblack" || plan.b_e == "prog_eblackgap") ? 0 : kBlue;
            const int plo = have_lo && clo.valid ? clo.plen : 0;
            EGrade e = grade_e(d.vs, kYellow, c0, plo);
            checks.note("P9 " + tg + " E: " + num((long long)e.window_lines) + " window lines, COLOR01 run lengths {" + setstr(e.c1_lens) + "}, P_lo=" + num(plo));
            checks.expect(have_lo && clo.valid, "PRE-P9-PLO " + tg + " E: P_lo is valid in this configuration (PRE-P5-CALLO)");
            const bool lines_ok = e.window_lines > 0 && e.windows_per_frame_min >= 1;
            checks.expect(lines_ok, "PRE-P9-LINES " + tg + " E: every complete field has at least one window line (" + num((long long)e.window_lines) + " window lines, at least " + num((long long)(e.window_lines ? e.windows_per_frame_min : 0)) + " per field)");
            const bool one0 = e.window_lines > 0 && e.bad_one0 == 0;
            checks.expect(one0, "PRE-P9-ONE0 " + tg + " E: each window line has exactly one run of pixels of value 0 (" + num((long long)e.bad_one0) + " of " + num((long long)e.window_lines) + " do not)");
            const bool pre = g.ok() && have_lo && clo.valid && lines_ok && one0;
            const std::string why = g.why() + std::string(have_lo && clo.valid ? "" : " PRE-P9-PLO") + std::string(lines_ok ? "" : " PRE-P9-LINES") + std::string(one0 ? "" : " PRE-P9-ONE0");
            EGrade ew; bool have_ew = false; bool pre_ew = false;
            if (plan.ew) {
                DEData dw = load_de(dir, plan.b_ew, tag, false, 0x00E0, 0x0002);
                VGate gw = report_common_video(checks, dw.a, dw.vs, tag, "E-widefetch");
                ew = grade_e(dw.vs, kYellow, kBlue, plo); have_ew = true;
                const bool wl = ew.window_lines > 0 && ew.bad_one0 == 0 && ew.windows_per_frame_min >= 1;
                checks.expect(wl, "PRE-P9-ONE0 " + tg + " E-widefetch: each window line has exactly one run of pixels of value 0 (" + num((long long)ew.bad_one0) + " of " + num((long long)ew.window_lines) + " do not)");
                pre_ew = gw.ok() && wl;
            }
            if (!pre || (plan.ew && !pre_ew)) {
                checks.record("P9 " + tg + " NOT GRADED COP2-horizontal-beam-position.c7 COP2-horizontal-beam-position.c9 ch2-horizontal-beam-position.c8: precondition failed:" + why + std::string(plan.ew && !pre_ew ? " E-widefetch-preconditions" : ""));
            } else {
                checks.expect(e.bad_a == 0, "P9-A-E " + tg + " [p.23,p.57,p.59]: every window line of run E is one run of COLOR01 of length 320 * P_lo, and otherwise only COLOR00 and 0 pixels (" + num((long long)e.bad_a) + " of " + num((long long)e.window_lines) + " lines differ)");
                if (have_ew) checks.expect(ew.bad_a == 0, "P9-A-EW " + tg + " [p.23,p.57,p.59]: every window line of E-widefetch is one run of COLOR01 of length 320 * P_lo, and otherwise only COLOR00 and 0 pixels (" + num((long long)ew.bad_a) + " of " + num((long long)ew.window_lines) + " lines differ)");
                checks.expect(e.bad_b == 0, "P9-B " + tg + " [p.23]: going cyclically from the end of the COLOR01 run, a window line shows COLOR00, one run of 0, COLOR00 and nothing else (" + num((long long)e.bad_b) + " of " + num((long long)e.window_lines) + " lines differ)");
                if (have_ew) checks.expect(e.bad_a == 0 && ew.bad_a == 0,
                              "P9-A " + tg + " COP2-horizontal-beam-position.c7 COP2-horizontal-beam-position.c9 ch2-horizontal-beam-position.c8 [p.23,p.57,p.59]: the standard screen is 320 pixels wide and the rest of each of its lines shows only the background colour, in the standard stimulus and in E-widefetch");
            }
        }
    }
}


}  // namespace

// The claims SPECIFICATION.md section 1 does not turn into a proposition, declined with their reasons.
void record_declined(Checks& checks) {
    checks.record("DECLINED ch2-horizontal-beam-position.c14 COP2-horizontal-beam-position.c16 [p.23]: the display's line length of 227 1/2 colour clocks reaches the test only through a line "
                  "doubler, and the sentence is unclear for PAL (SPECIFICATION.md section 1 and 7 item 6)");
    checks.record("DECLINED ch2-vertical-beam-position.c21 COP2-vertical-beam-position.c24 [p.24]: \"processor is twice this\": the processor clock is internal to the CPU and no page "
                  "converts a bus access into processor clock cycles (SPECIFICATION.md section 1)");
    checks.record("DECLINED COP2-copins-timing.c6 [p.275]: a duration in nanoseconds is not observable, the harness states no clock period (SPECIFICATION.md section 1)");
    checks.record("DECLINED COP2-putting-together-a-copper-instruction-list.c9 COP2-putting-together-a-copper-instruction-list.c10 [p.27]: the end-of-display coordinate (226,262) NTSC and (226,312) PAL: "
                  "counted from line 0 the last line is 261 or 311 by p.24 and p.34, and no page says whether an NTSC field ends on a long line (SPECIFICATION.md section 1 and 7 item 1)");
    checks.record("DECLINED COP2-horizontal-beam-position.c8 ch2-horizontal-beam-position.c7 [p.23]: the unused portion $04 to $47 is a pair of counter positions, and no page gives the delay "
                  "from a counter value to a displayed pixel (SPECIFICATION.md section 1 and 7 item 5)");
    checks.record("DECLINED COP2-horizontal-beam-position.c6 ch2-horizontal-beam-position.c6 [p.23]: horizontal blanking $0F to $35 is a range of counter positions, not observable against the "
                  "displayed pixels for the same reason (SPECIFICATION.md section 1)");
    checks.record("DECLINED COP2-vertical-beam-position.c10 [p.24]: Copper execution over all 262 lines depends on where the Copper restarts, which the pages state two ways (SPECIFICATION.md section 1 and 7 item 8)");
}

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    if (mv.empty()) {
        const char* only_env = std::getenv("NMH_ONLY");     // development aid: run one section of the normal run
        const std::string only = only_env ? only_env : "ACBD";
        record_declined(checks);
        if (only.find('A') != std::string::npos) section_A(checks, dir, "", true);
        if (only.find('C') != std::string::npos) section_C(checks, dir, "", true);
        if (only.find('B') != std::string::npos) section_B(checks, dir, "", true);
        if (only.find('D') != std::string::npos) section_DE(checks, dir, "", true);
    } else if (mv.rfind("A-", 0) == 0) {
        section_A(checks, dir, mv, false);
    } else if (mv.rfind("C-", 0) == 0) {
        section_C(checks, dir, mv, false);
    } else if (mv.rfind("B-", 0) == 0) {
        section_B(checks, dir, mv, false);
    } else if (mv.rfind("D-", 0) == 0 || mv.rfind("E-", 0) == 0) {
        section_DE(checks, dir, mv, false);
    } else {
        std::printf("unknown NMH_MUTATE value %s\n", mv.c_str());
        return 2;
    }
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
