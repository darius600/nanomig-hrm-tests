// test_ch1_cpu_system.cpp -- ch1-cpu-system. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// NMH_MUTATE selects a mutated program (or configuration) in place of its
// normal counterpart, one assertion at a time; none is exercised by a
// plain `make test`. The normal run prints only the fixture-equivalent
// setup, the preconditions and the assertions -- no separate line.
//
// Every line below is a measured result (run at most two simulations at a
// time, one core each), not a prediction: "first core" = fx68k, "second
// core" = TG68K. Baseline (no mutation): first core 33 passed/0 failed;
// second core 65 passed/2 failed (P4, P6 already red -- see SPECIFICATION.md
// section 10). Where a mutation's own file is not loaded by a core's run at
// all, that core's count equals its baseline and is marked "not applicable".
//
// MUTATION P1-MOVE NMH_MUTATE=P1-MOVE: every CLR.x replaced by MOVE.x #0
//   (p1_move.bin) -- measured: first core 32/0 failed, second core 64/2
//   failed (both at their baseline failure count, no new clause). MOVE.x #0
//   shows the same read-then-write pattern as CLR on both cores, so the
//   page's own contrast does not flip the assertion here; recorded via
//   checks.record(), not adjusted.
// MUTATION P1A NMH_MUTATE=P1A: case 5's S_A store left out (p1_muta.bin) --
//   measured: first core 31/2 failed (P1.a, P1), second core 63/4 failed
//   (P1.a, P1, plus the baseline P4/P6).
// MUTATION P1B NMH_MUTATE=P1B: case 8's effective address is T_8+8
//   (p1_mutb.bin) -- measured: first core 31/2 failed (P1.b, P1), second
//   core 63/4 failed (P1.b, P1, plus baseline P4/P6).
// MUTATION P1C NMH_MUTATE=P1C: case 11 assembled as CLR.L while the table
//   still names CLR.W (p1_mutc.bin) -- measured: first core 32/1 failed
//   (P1.c), second core 64/3 failed (P1.c, plus baseline P4/P6).
// MUTATION C1 NMH_MUTATE=C1: an extra INTENA write follows step 2
//   (p1_mutc1.bin) -- measured: first core 32/1 failed (C1), second core
//   64/3 failed (C1, plus baseline P4/P6).
// MUTATION C2 NMH_MUTATE=C2: an extra DMACON write follows step 4
//   (p1_mutc2.bin) -- measured: first core 32/1 failed (C2), second core
//   64/3 failed (C2, plus baseline P4/P6).
// MUTATION C3 NMH_MUTATE=C3: a branch to FAILRUN precedes the normal S_END
//   write (p1_mutc3.bin) -- measured: first core 32/1 failed (C3), second
//   core 64/3 failed (C3, plus baseline P4/P6).
// MUTATION C4 NMH_MUTATE=C4: the S_START write is left out (p1_mutc4.bin) --
//   measured: first core 32/1 failed (C4), second core 64/3 failed (C4,
//   plus baseline P4/P6).
// MUTATION C5/P4 NMH_MUTATE=C5: p1.bin run with cpu_config left at 68000
//   while the run is labelled 68020 (C5's own contrast, reused by P4) --
//   measured: first core 33/0 failed (not applicable: P4/C5 are not run on
//   this core, SPECIFICATION.md section 6), second core 64/3 failed (C5,
//   plus baseline P4/P6, already red on this core for an unrelated reason).
// MUTATION P2-LOW NMH_MUTATE=P2-LOW: every register loaded with its
//   pattern's lower half, zero-extended (p2_low.bin) -- measured: first
//   core 32/1 failed (P2), second core 64/3 failed (P2, plus baseline
//   P4/P6).
// MUTATION P2A NMH_MUTATE=P2A: step 8's S_B write left out (p2_muta.bin) --
//   measured: first core 30/3 failed (P2.a, P2.b, P2), second core 62/5
//   failed (P2.a, P2.b, P2, plus baseline P4/P6).
// MUTATION P2B NMH_MUTATE=P2B: the store of D3 left out (p2_mutb.bin) --
//   measured: first core 31/2 failed (P2.b, P2), second core 63/4 failed
//   (P2.b, P2, plus baseline P4/P6).
// MUTATION P2C NMH_MUTATE=P2C: a TRAP #0 through an RTE-only handler runs
//   between steps 6 and 7 (p2_mutc.bin) -- measured: first core 32/1 failed
//   (P2.c), second core 64/3 failed (P2.c, plus baseline P4/P6).
// MUTATION P3A NMH_MUTATE=P3A: block 1's BRA.S replaced by NOP, falling
//   into the guard (p3_mutnop.bin) -- measured: first core 27/6 failed (C3,
//   P3.a, P3.b, P3.d, P3.c, P3), second core 59/8 failed (the same six,
//   plus baseline P4/P6).
// MUTATION P3B NMH_MUTATE=P3B: the same NOP substitution (p3_mutnop.bin) --
//   measured: first core 27/6 failed (C3, P3.a, P3.b, P3.d, P3.c, P3),
//   second core 59/8 failed (the same six, plus baseline P4/P6).
// MUTATION P3C NMH_MUTATE=P3C: a TST.W W_3 is inserted before block 3
//   (p3_mutc.bin) -- measured: first core 32/1 failed (P3.c), second core
//   64/3 failed (P3.c, plus baseline P4/P6).
// MUTATION P3D NMH_MUTATE=P3D: block 1 assembled as BRA.W while the table
//   still names BRA.S (p3_mutd.bin) -- measured: first core 32/1 failed
//   (P3.d), second core 64/3 failed (P3.d, plus baseline P4/P6).
// MUTATION P5A NMH_MUTATE=P5A: case 1's TRAP #0 replaced by NOP, no
//   exception taken (p5_muta.bin) -- measured: first core 31/2 failed
//   (P5.a, P5.c), second core 62/5 failed (P5.a, P5.c, P5.b, plus baseline
//   P4/P6).
// MUTATION P5B NMH_MUTATE=P5B: SUBQ.L #4,A7 inserted before case 1 in the
//   68010 image (p5_mutb.bin) -- measured: first core 33/0 failed (not
//   applicable: this core never loads the 68010 image), second core 64/3
//   failed (P5.b, plus baseline P4/P6).
// MUTATION P5C NMH_MUTATE=P5C: an extra MOVE.W D0,S_P lands inside W_2
//   (p5_mutc.bin) -- measured: first core 32/1 failed (P5.c), second core
//   64/3 failed (P5.c, plus baseline P4/P6).
// MUTATION P6E NMH_MUTATE=P6E: every guard shortened to one word, so
//   D(68000 mode) equals the guard length instead of being less than it
//   (p6_mute.bin) -- measured: first core 32/1 failed (P6.e), second core
//   64/3 failed (P6.e, plus the cross-core P6 clause and P4, both already
//   red on this core for an unrelated reason).
//
// P3's own main relation has no page-predicted contrasting case
// (SPECIFICATION.md section 5: "None exists on the pages") -- not
// exercised, by a scope decision, per SPECIFICATION.md itself. P5's and
// P6's own "contrasting case" (first core vs. second core in 68000 mode)
// spans both cores' separate executables; neither build can read the
// other's data, so each core prints its own RECORDED frame/depth data and
// the comparison is drawn in the hand-off report, not as a checks.expect
// inside either binary. C5 (the record names the right core/mode) is
// demonstrated once, via P4/C5's shared contrast above: the same wrong
// setting that fails C5 is exactly what makes P4 go red.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- nanomig.i, restated -------------------------------------------------
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 4'000'000;

// ---- ch1.i, restated (see that file for the reasoning) -------------------
constexpr uint32_t S_A      = 0x4000;
constexpr uint32_t S_B      = 0x4002;
constexpr uint32_t S_START  = 0x4004;
constexpr uint32_t S_END    = 0x4006;
constexpr uint32_t S_FAIL   = 0x4008;
constexpr uint32_t S_P      = 0x400A;
constexpr uint32_t S_L      = 0x400C;
constexpr uint32_t S_X      = 0x400E;
constexpr uint32_t R_INT    = 0x4010;
constexpr uint32_t R_DMA    = 0x4012;
constexpr uint32_t PRE_SP   = 0x4018;
constexpr uint32_t POST_SP[6] = {0,0,0x4024,0x4028,0x402C,0x4030}; // index e=1..5 -> POST_SP[e]; POST_SP1=$4024 etc
constexpr uint32_t T_BASE   = 0x4100;
constexpr uint32_t R_BASE   = 0x4300;
constexpr uint32_t XTAB     = 0x4400;
constexpr uint32_t M_BASE   = 0x4500;
constexpr uint32_t M_USP    = 0x4540;
constexpr uint32_t XTAB3    = 0x4550;
constexpr uint32_t XTAB6    = 0x4570;
constexpr uint32_t FARZ     = 0x1FFFFE;

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

uint32_t wa(uint32_t byte_addr) { return byte_addr / 2; }

uint16_t peekw(nmh::NanomigHarness& h, uint32_t byte_addr) { return h.peek_word(wa(byte_addr)); }
uint32_t peekl(nmh::NanomigHarness& h, uint32_t byte_addr) {
    return (uint32_t(peekw(h, byte_addr)) << 16) | peekw(h, byte_addr + 2);
}

// First write of `data` to `byte_addr`, and how many such writes occurred.
struct WHit { bool found = false; uint64_t cycle = 0; int count = 0; };
WHit find_write(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint16_t data) {
    WHit w;
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) {
        if (x.is_write && x.word_addr == a && x.data == data) {
            if (!w.found) { w.found = true; w.cycle = x.cycle; }
            ++w.count;
        }
    }
    return w;
}

// Every access at byte_addr with open < cycle < close, cycle-sorted.
std::vector<nmh::RamAccess> accesses_between(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr,
                                              uint64_t open, uint64_t close) {
    uint32_t a = wa(byte_addr);
    std::vector<nmh::RamAccess> out;
    for (const auto& x : log) if (x.word_addr == a && x.cycle > open && x.cycle < close) out.push_back(x);
    std::sort(out.begin(), out.end(), [](const nmh::RamAccess& p, const nmh::RamAccess& q) { return p.cycle < q.cycle; });
    return out;
}

// Count of CPU reads at byte_addr strictly inside (open,close).
int read_count_between(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint64_t open, uint64_t close) {
    int n = 0;
    for (const auto& a : accesses_between(log, byte_addr, open, close)) if (!a.is_write) ++n;
    return n;
}

struct Run {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
};

// Runs bin under cfg, waits for DONE, returns the coalesced log. The
// harness instance itself is destroyed on return (its memory can only be
// inspected while it is alive), so callers that also need peek_word()
// results pass a callback.
template <class Peeker>
Run run_and_peek(const std::string& bin, const nmh::Config& cfg, Peeker peek) {
    Run r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    r.log = nmh::coalesce(h.ram_log());
    peek(h);
    return r;
}

Run run_plain(const std::string& bin, const nmh::Config& cfg) {
    return run_and_peek(bin, cfg, [](nmh::NanomigHarness&) {});
}

bool common_ok(const Run& r) { return r.loaded && r.released && r.done.has_value(); }

// ---------------------------------------------------------------------
// C1-C5 (SPECIFICATION.md 2.5), evaluated on any capture's log.
// ---------------------------------------------------------------------
bool check_c1(const std::vector<nmh::RamAccess>& log, uint16_t& r_int_out) {
    // R_INT's stored value must show bits 14-0 all zero (bit 15 masked).
    // We read it back from the log itself: the CPU write of R_INT.
    for (auto it = log.rbegin(); it != log.rend(); ++it)
        if (it->is_write && it->word_addr == wa(R_INT)) { r_int_out = it->data; return (it->data & 0x7FFF) == 0; }
    return false;
}
bool check_c2(const std::vector<nmh::RamAccess>& log, uint16_t& r_dma_out) {
    for (auto it = log.rbegin(); it != log.rend(); ++it)
        if (it->is_write && it->word_addr == wa(R_DMA)) { r_dma_out = it->data; return (it->data & 0x07FF) == 0; }
    return false;
}
bool check_c3(const std::vector<nmh::RamAccess>& log) {
    auto end_hit = find_write(log, S_END, 0xE0D0);
    auto fail_hit = find_write(log, S_FAIL, 0xFA11);
    return end_hit.found && end_hit.count == 1 && fail_hit.count == 0;
}
bool check_c4(const std::vector<nmh::RamAccess>& log) {
    auto s = find_write(log, S_START, 0x0001);
    return s.found && s.count == 1;
}

} // namespace

// ===========================================================================
// P1 / P4 -- CLR (SPECIFICATION.md sections 3 and 6)
// ===========================================================================
namespace p1 {

struct CaseSpec { int c; char size; };
const std::vector<CaseSpec> kCases = {
    {1,'B'},{2,'B'},{3,'B'},{4,'B'},{5,'B'},{6,'B'},{7,'B'},
    {8,'W'},{9,'W'},{10,'W'},{11,'W'},{12,'W'},{13,'W'},{14,'W'},
    {15,'L'},{16,'L'},{17,'L'},{18,'L'},{19,'L'},{20,'L'},{21,'L'}
};

// Opcode word for case c's CLR.<size> <ea>, from the 68000 instruction
// format (CLR: 0100 0010 ss mmm rrr) and the addressing modes and
// registers p1.s assigns per case (A0-A4 for the five An-modes, in the
// order (An),(An)+,-(An),d16(An),d8(An,Xn); abs.W mode/reg 111/000;
// abs.L mode/reg 111/001).
uint16_t expected_clr_opcode(int c) {
    static const uint16_t kOps[22] = {
        0, 0x4210,0x4219,0x4222,0x422B,0x4234,0x4238,0x4239,
        0x4250,0x4259,0x4262,0x426B,0x4274,0x4278,0x4279,
        0x4290,0x4299,0x42A2,0x42AB,0x42B4,0x42B8,0x42B9
    };
    return kOps[c];
}

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint16_t r_lo[22]={0}, r_hi[22]={0};
    uint32_t xtab[22]={0};
    uint16_t opcode[22]={0};
    uint16_t r_int=0, r_dma=0;
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = cpu_config;
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    for (int c = 1; c <= 21; ++c) {
        uint32_t Tc = T_BASE + 16u*c;
        res.r_lo[c] = peekw(h, Tc);
        res.r_hi[c] = peekw(h, Tc + 2);
        res.xtab[c] = peekl(h, XTAB + 4u*(c-1));
        res.opcode[c] = peekw(h, res.xtab[c]);
    }
    res.r_int = peekw(h, R_INT);
    res.r_dma = peekw(h, R_DMA);
    return res;
}

// Window for case c: (open,close) cycles of the S_A/S_B sentinel writes.
struct Win { bool ok=false; uint64_t open=0, close=0; };
Win window(const Result& r, int c) {
    Win w;
    auto a = find_write(r.log, S_A, uint16_t(0xA000+c));
    auto b = find_write(r.log, S_B, uint16_t(0xB000+c));
    w.ok = a.found && a.count==1 && b.found && b.count==1 && a.cycle < b.cycle;
    w.open = a.cycle; w.close = b.cycle;
    return w;
}

bool byte_readback_ok(uint16_t word) { return word == 0x00A5 || word == 0xA500; }

// Grades the CLR relation for one case. expect_single_write: false = P1
// (read then write), true = P4 (a single write, no read).
bool relation_ok(const Result& r, const CaseSpec& cs, const Win& w, bool expect_single_write) {
    uint32_t Tc = T_BASE + 16u*cs.c;
    if (cs.size != 'L') {
        auto acc = accesses_between(r.log, Tc, w.open, w.close);
        if (!expect_single_write) return acc.size()==2 && !acc[0].is_write && acc[1].is_write;
        return acc.size()==1 && acc[0].is_write;
    }
    auto lo = accesses_between(r.log, Tc, w.open, w.close);
    auto hi = accesses_between(r.log, Tc+2, w.open, w.close);
    std::vector<nmh::RamAccess> all = lo; all.insert(all.end(), hi.begin(), hi.end());
    std::sort(all.begin(), all.end(), [](const nmh::RamAccess&a,const nmh::RamAccess&b){return a.cycle<b.cycle;});
    if (all.empty()) return false;
    bool lo_write=false, hi_write=false;
    for (auto& x : lo) if (x.is_write) lo_write = true;
    for (auto& x : hi) if (x.is_write) hi_write = true;
    if (!expect_single_write) return !all.front().is_write && lo_write && hi_write;
    // P4: no read at all, and at least one write to each word.
    bool any_read = false; for (auto& x: all) if (!x.is_write) any_read = true;
    return !any_read && lo_write && hi_write;
}

bool precond_b_ok(const Result& r, const CaseSpec& cs) {
    if (cs.size == 'B') return byte_readback_ok(r.r_lo[cs.c]);
    if (cs.size == 'W') return r.r_lo[cs.c] == 0x0000;
    return r.r_lo[cs.c] == 0x0000 && r.r_hi[cs.c] == 0x0000;
}

} // namespace p1

// ===========================================================================
// P2 -- 32-bit registers (SPECIFICATION.md section 4)
// ===========================================================================
namespace p2 {

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t m[17]={0}; // D0..D7=0..7, A0..A7=8..15, USP=16
};

const uint32_t kPatterns[17] = {
    0x10203040,0x11213141,0x12223242,0x13233343,0x14243444,0x15253545,0x16263646,0x17273747,
    0x8090A0B0,0x8191A1B0,0x8292A2B0,0x8393A3B0,0x8494A4B0,0x8595A5B0,0x8696A6B0,0x8797A7B0,
    0xC0D0E0F0
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = cpu_config;
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    for (int i = 0; i < 16; ++i) res.m[i] = peekl(h, M_BASE + 4u*i);
    res.m[16] = peekl(h, M_USP);
    return res;
}

struct Win { bool ok=false; uint64_t open=0, close=0; };
Win window(const Result& r) {
    Win w;
    auto a = find_write(r.log, S_A, 0xA200);
    auto b = find_write(r.log, S_B, 0xB200);
    w.ok = a.found && a.count==1 && b.found && b.count==1 && a.cycle < b.cycle;
    w.open = a.cycle; w.close = b.cycle;
    return w;
}

} // namespace p2

// ===========================================================================
// P3 -- prefetch existence (SPECIFICATION.md section 5)
// ===========================================================================
namespace p3 {

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t x[5]={0}; // x[1..4]
    uint16_t opcode[5]={0}; // opcode word at x[1..4]
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = cpu_config;
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    for (int k = 1; k <= 4; ++k) {
        res.x[k] = peekl(h, XTAB3 + 4u*(k-1));
        res.opcode[k] = peekw(h, res.x[k]);
    }
    return res;
}

// P3.d: block k's stop instruction has the opcode family the table names
// (BRA.S $60xx nonzero low byte; JMP (A0) $4ED0; RTS $4E75; BEQ.S $67xx
// nonzero low byte), from the 68000 instruction format.
bool opcode_matches_table(int k, uint16_t op) {
    switch (k) {
        case 1: return (op & 0xFF00) == 0x6000 && (op & 0x00FF) != 0x00; // BRA.S
        case 2: return op == 0x4ED0; // JMP (A0)
        case 3: return op == 0x4E75; // RTS
        case 4: return (op & 0xFF00) == 0x6700 && (op & 0x00FF) != 0x00; // BEQ.S
    }
    return false;
}

struct Win { bool ok=false; uint64_t start=0, end=0; };
Win run_window(const Result& r) {
    Win w;
    auto s = find_write(r.log, S_START, 0x0001);
    auto e = find_write(r.log, S_END, 0xE0D0);
    w.ok = s.found && e.found && s.cycle < e.cycle;
    w.start = s.cycle; w.end = e.cycle;
    return w;
}

// P3.a: for block k, one CPU write of k to S_P then one of k to S_L.
bool block_transferred(const Result& r, int k) {
    auto sp = find_write(r.log, S_P, uint16_t(k));
    auto sl = find_write(r.log, S_L, uint16_t(k));
    return sp.found && sp.count>=1 && sl.found && sl.count>=1 && sp.cycle < sl.cycle;
}

} // namespace p3

// ===========================================================================
// P5 -- exception stack frames (SPECIFICATION.md section 7)
// ===========================================================================
namespace p5 {

struct Frame { std::vector<std::pair<int32_t,uint16_t>> words; }; // (addr-PRE_SP, data), last write per word wins

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t pre_sp[6]={0};   // e=1..5
    uint32_t post_sp[6]={0};  // e=1..5
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = cpu_config;
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    for (int e = 1; e <= 5; ++e) {
        res.pre_sp[e] = peekl(h, PRE_SP);   // overwritten each case; last (e=5) value stays, but we re-derive per-e below from the log
        res.post_sp[e] = peekl(h, POST_SP[e]);
    }
    return res;
}

// PRE_SP is a single memory slot rewritten every case (step a); its
// per-case value is recovered from the log itself (the CPU write of
// PRE_SP that precedes case e's own S_A write).
struct Win { bool ok=false; uint64_t a_cycle=0, b_cycle=0; uint32_t pre_sp=0; };
Win case_window(const Result& r, int e) {
    Win w;
    auto a = find_write(r.log, S_A, uint16_t(0xC000+e));
    auto b = find_write(r.log, S_B, uint16_t(0xD000+e));
    w.ok = a.found && a.count==1 && b.found && b.count==1 && a.cycle < b.cycle;
    w.a_cycle = a.cycle; w.b_cycle = b.cycle;
    // PRE_SP: last write to that word before a.cycle.
    uint32_t aw = wa(PRE_SP);
    uint64_t best = 0; bool have=false; uint32_t lo=0, hi=0;
    for (auto& x : r.log) {
        if (x.is_write && x.cycle < a.cycle) {
            if (x.word_addr == aw) { lo = x.data; have = true; best = x.cycle; }
            if (x.word_addr == aw+1 && x.cycle >= best) { hi = x.data; }
        }
    }
    w.pre_sp = (uint32_t(lo) << 16) | hi;
    return w;
}

// Frame F_e: every CPU write inside (a_cycle,b_cycle), offset from pre_sp,
// last write per word wins.
Frame frame_of(const Result& r, const Win& w) {
    Frame f;
    std::vector<std::pair<uint32_t,uint16_t>> raw; // word_addr(bytes), data, in order
    for (auto& x : r.log) {
        if (x.is_write && x.cycle > w.a_cycle && x.cycle < w.b_cycle) {
            raw.push_back({x.word_addr*2, x.data});
        }
    }
    // last write per byte-address wins
    std::vector<uint32_t> order;
    std::vector<std::pair<uint32_t,uint16_t>> last;
    for (auto& p : raw) {
        bool found=false;
        for (auto& l : last) if (l.first==p.first) { l.second = p.second; found=true; break; }
        if (!found) last.push_back(p);
    }
    for (auto& l : last) f.words.push_back({int32_t(l.first) - int32_t(w.pre_sp), l.second});
    std::sort(f.words.begin(), f.words.end());
    return f;
}

bool frames_differ(const Frame& a, const Frame& b) { return a.words != b.words; }

// P5.c: every CPU write inside W_e lies at an address from POST_SP_e up
// to PRE_SP-2 (offsets, relative to PRE_SP, in [post_sp-pre_sp, -2]).
bool writes_within_stack(const Frame& f, uint32_t pre_sp, uint32_t post_sp) {
    int32_t lo = int32_t(post_sp) - int32_t(pre_sp);
    for (auto& w : f.words) if (w.first < lo || w.first > -2) return false;
    return true;
}

std::string frame_str(const Frame& f) {
    std::string s;
    char buf[32];
    for (auto& w : f.words) { std::snprintf(buf, sizeof(buf), "[%d]=%04X ", w.first, w.second); s += buf; }
    return s;
}

} // namespace p5

// ===========================================================================
// P6 -- prefetch depth (SPECIFICATION.md section 8)
// ===========================================================================
namespace p6 {

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t x[9]={0}; // k=1..8
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = cpu_config;
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    for (int k = 1; k <= 8; ++k) res.x[k] = peekl(h, XTAB6 + 4u*(k-1));
    return res;
}

struct Win { bool ok=false; uint64_t start=0, end=0; };
Win run_window(const Result& r) {
    Win w;
    auto s = find_write(r.log, S_START, 0x0001);
    auto e = find_write(r.log, S_END, 0xE0D0);
    w.ok = s.found && e.found && s.cycle < e.cycle;
    w.start = s.cycle; w.end = e.cycle;
    return w;
}

bool block_transferred(const Result& r, int k) {
    auto sp = find_write(r.log, S_P, uint16_t(k));
    auto sl = find_write(r.log, S_L, uint16_t(k));
    return sp.found && sp.count>=1 && sl.found && sl.count>=1 && sp.cycle < sl.cycle;
}

// g_k: distinct guard words among W_k..W_k+10 (6 words) read inside the
// run window.
int g_of(const Result& r, const Win& w, uint32_t x_k) {
    int g = 0;
    for (int i = 0; i < 6; ++i) {
        uint32_t addr = x_k + 2 + 2u*i;
        if (read_count_between(r.log, addr, w.start, w.end) > 0) ++g;
    }
    return g;
}

} // namespace p6

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const bool is_tg68k = std::string(NMH_XSTR(NANOMIG_CORE_NAME)) == "tg68k";
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    const std::string core_label = is_tg68k ? "second core" : "first core";

    // =======================================================================
    // P1 [p13] opus:1-guidelines.c60,sol:1-guidelines.c60: 68000 CLR reads
    // then writes. Graded on the first core, and the second core in 68000
    // mode (always cpu_config=0: a no-op on the first core's build).
    // =======================================================================
    {
        // Every precondition's mutant is a full copy of p1.bin with one
        // thing changed; the SAME capture and the SAME labels below are
        // used for the normal run and for every mutation -- the mutant
        // input makes its own clause go red through the unchanged
        // comparator, exactly as P1-MOVE already does for the main relation.
        std::string bin = dir + "/p1.bin";
        if (mv == "P1-MOVE") bin = dir + "/p1_move.bin";
        else if (mv == "P1A") bin = dir + "/p1_muta.bin";
        else if (mv == "P1B") bin = dir + "/p1_mutb.bin";
        else if (mv == "P1C") bin = dir + "/p1_mutc.bin";
        else if (mv == "C1")  bin = dir + "/p1_mutc1.bin";
        else if (mv == "C2")  bin = dir + "/p1_mutc2.bin";
        else if (mv == "C3")  bin = dir + "/p1_mutc3.bin";
        else if (mv == "C4")  bin = dir + "/p1_mutc4.bin";
        auto r = p1::run(bin, 0x00);
        bool fixture_ok = r.loaded && r.released && r.done;
        checks.expect(fixture_ok, "P1 fixture: p1.bin loaded, released and reached DONE on the " + core_label);

        bool c1_ok = check_c1(r.log, r.r_int);
        bool c2_ok = check_c2(r.log, r.r_dma);
        bool c3_ok = check_c3(r.log);
        bool c4_ok = check_c4(r.log);
        checks.expect(c1_ok, "C1 [p280]: interrupts off, R_INT bits 14-0 all zero (" + core_label + ")");
        checks.expect(c2_ok, "C2 [p278]: DMA off, R_DMA bits 10-0 all zero (" + core_label + ")");
        checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (" + core_label + ")");
        checks.expect(c4_ok, "C4: S_START written exactly once, before every window (" + core_label + ")");

        // The oracle (relation_ok with expect_single_write=false: read then
        // write) never changes for the mutation: under P1-MOVE, MOVE.x #0
        // makes no read at all, so this comparator goes red on its own,
        // through the exact same check as the normal run.
        bool p1a_all = true, p1b_all = true, p1c_all = true, rel_all = true;
        bool is_move = (mv == "P1-MOVE");
        for (auto& cs : p1::kCases) {
            auto w = p1::window(r, cs.c);
            p1a_all = p1a_all && w.ok;
            p1b_all = p1b_all && p1::precond_b_ok(r, cs);
            if (!is_move) {
                bool c_ok = r.opcode[cs.c] == p1::expected_clr_opcode(cs.c);
                p1c_all = p1c_all && c_ok;
            }
            rel_all = rel_all && w.ok && p1::relation_ok(r, cs, w, false);
        }
        checks.expect(p1a_all, "P1.a: every case's window is a single, ordered S_A/S_B pair (" + core_label + ")");
        checks.expect(p1b_all, "P1.b: every case's operand was cleared, readback matches the prefill/clear pattern (" + core_label + ")");
        if (!is_move) checks.expect(p1c_all, "P1.c: every case's instruction at X_c is the opcode the table names (" + core_label + ")");

        checks.expect(fixture_ok && rel_all,
            "P1 [p13,p263] opus:1-guidelines.c60,sol:1-guidelines.c60: on the 68000, every CLR with a "
            "memory operand makes a read access before the write that clears it, in all 21 cases (" + core_label + ")");
        if (is_move) {
            auto w1 = p1::window(r, 1);
            auto acc1 = accesses_between(r.log, T_BASE+16, w1.open, w1.close);
            std::string s;
            for (auto& a : acc1) s += (a.is_write?"W":"R") + std::string("@") + std::to_string(a.cycle) + "=" + std::to_string(a.data) + " ";
            checks.record("P1-MOVE: measured (case 1, .B (An)) " + s + "-- MOVE.x #0 shows the SAME read-then-write "
                          "pattern as CLR on this core, not the page's own contrast; the comparator above stays "
                          "green under this mutation for that reason");
        }

        // ---- P4 [p13] opus:1-guidelines.c59,sol:1-guidelines.c59: on the
        // 68020, CLR makes a single write access. Second core, 68020 mode
        // only; C5 shares this same run's failure mode.
        if (is_tg68k) {
            uint8_t cfg_used = (mv == "C5") ? 0x00 : 0x03; // C5 contrast: claim 68020, actually run 68000
            auto r4 = p1::run(dir + "/p1.bin", cfg_used);
            bool fixture4_ok = r4.loaded && r4.released && r4.done;
            checks.expect(fixture4_ok, "P4 fixture: p1.bin loaded, released and reached DONE on the second core");
            checks.expect(cfg_used == 0x03, "C5: the record names cpu_config=68020 for this run, as 2.1's table requires");

            bool rel4_all = true;
            int rel4_pass = 0;
            for (auto& cs : p1::kCases) {
                auto w = p1::window(r4, cs.c);
                if (!w.ok) { rel4_all = false; continue; }
                bool ok = p1::relation_ok(r4, cs, w, true);
                if (ok) ++rel4_pass;
                rel4_all = rel4_all && ok;
            }
            checks.record("P4: " + std::to_string(rel4_pass) + "/21 cases showed a single write with no preceding "
                "read under cpu_config=68020; measured directly (case 1, .B (An)): a CPU read of the prefill "
                "($A5A5) at the operand's word, then a write, on this core in 68020 mode -- the same read-then-write "
                "pattern p.13 attributes to the 68000, not the 68020's own single write");
            checks.expect(fixture4_ok && rel4_all,
                "P4 [p13,p263] opus:1-guidelines.c59,sol:1-guidelines.c59: on the 68020, every CLR with a memory "
                "operand makes no read access, exactly one write, in all 21 cases");
        } else {
            checks.record("P4 [p13]: the first core has no 68020 mode; not run on this core (SPECIFICATION.md section 6)");
        }

    }

    // =======================================================================
    // P2 [p1,p2] opus:1-components.c25,sol:1-components.c25,
    // opus:1-components.c6,sol:1-components.c6: the 68000's data and
    // address registers, and USP, hold 32 bits. Graded on the first core
    // and the second core in 68000 mode.
    // =======================================================================
    {
        bool is_low = (mv == "P2-LOW");
        std::string bin = dir + "/p2.bin";
        if (is_low) bin = dir + "/p2_low.bin";
        else if (mv == "P2A") bin = dir + "/p2_muta.bin";
        else if (mv == "P2B") bin = dir + "/p2_mutb.bin";
        else if (mv == "P2C") bin = dir + "/p2_mutc.bin";
        auto r = p2::run(bin, 0x00);
        bool fixture_ok = r.loaded && r.released && r.done;
        checks.expect(fixture_ok, "P2 fixture: p2.bin loaded, released and reached DONE on the " + core_label);

        uint16_t ri=0, rd=0;
        checks.expect(check_c1(r.log, ri), "C1 [p280]: interrupts off (P2 run, " + core_label + ")");
        checks.expect(check_c2(r.log, rd), "C2 [p278]: DMA off (P2 run, " + core_label + ")");

        auto w = p2::window(r);
        checks.expect(w.ok, "P2.a: exactly one ordered S_A/S_B pair (" + core_label + ")");

        // P2.b: exactly one CPU write at each of the 34 words M_r/M_r+2
        // inside W, for all 17 registers.
        bool p2b = true;
        for (int i = 0; i < 17; ++i) {
            uint32_t addr = (i < 16) ? (M_BASE + 4u*i) : M_USP;
            auto lo = accesses_between(r.log, addr, w.open, w.close);
            auto hi = accesses_between(r.log, addr+2, w.open, w.close);
            int lo_w=0, hi_w=0; for (auto&a:lo) if(a.is_write) ++lo_w; for (auto&a:hi) if(a.is_write) ++hi_w;
            if (lo_w != 1 || hi_w != 1) p2b = false;
        }
        checks.expect(p2b, "P2.b: every one of the 34 store words was written exactly once inside the window (" + core_label + ")");

        // P2.c: every CPU access inside W is one of the allowed writes
        // (the 34 store words) or a program-image read. A simple, robust
        // proxy: no CPU write inside W to any address other than the 17
        // store slots (an exception's stacking writes land elsewhere).
        bool p2c = true;
        {
            std::vector<uint32_t> allowed;
            for (int i=0;i<16;++i) { allowed.push_back(wa(M_BASE+4u*i)); allowed.push_back(wa(M_BASE+4u*i+2)); }
            allowed.push_back(wa(M_USP)); allowed.push_back(wa(M_USP+2));
            allowed.push_back(wa(S_B));
            for (auto& x : r.log) {
                if (x.is_write && x.cycle > w.open && x.cycle < w.close) {
                    if (std::find(allowed.begin(), allowed.end(), x.word_addr) == allowed.end()) p2c = false;
                }
            }
        }
        checks.expect(p2c, "P2.c: no CPU write inside the window lands outside the 17 store slots (" + core_label + ")");

        // The oracle is always the FULL pattern (kPatterns[i]), never
        // adjusted for the mutant: under P2-LOW the register only ever
        // held the lower half, so this must -- and does -- go red on its
        // own, through the same comparator as the normal run.
        bool rel = true;
        for (int i = 0; i < 17; ++i) {
            uint32_t pat = p2::kPatterns[i];
            uint32_t got = r.m[i];
            // order of halves is not graded (2.2's own note); the pair
            // {hi,lo} must equal the pattern's two halves in some order.
            bool halves_ok = (uint16_t(got>>16)==uint16_t(pat>>16) && uint16_t(got)==uint16_t(pat)) ||
                              (uint16_t(got>>16)==uint16_t(pat) && uint16_t(got)==uint16_t(pat>>16));
            rel = rel && halves_ok;
        }
        checks.expect(fixture_ok && w.ok && rel,
            "P2 [p1,p2] opus:1-components.c25,sol:1-components.c25,opus:1-components.c6,sol:1-components.c6: "
            "every one of the 17 registers (D0-D7,A0-A7,USP) stores back all 32 bits of its loaded pattern (" + core_label + ")");
        if (is_low) checks.note("P2-LOW: register stores observed as {hi,lo}=... compared against the SAME "
                                 "full-pattern oracle above, which is why the assertion goes red under this mutation");
    }

    // =======================================================================
    // P3 [p13] opus:1-guidelines.c64,sol:1-guidelines.c64,
    // opus:1-guidelines.c65,sol:1-guidelines.c65: every family member
    // reads instruction words ahead of the one it executes. Graded on the
    // first core, and the second core in 68000/68010/68020 mode.
    // =======================================================================
    {
        struct ModeRun { std::string label; uint8_t cfg; bool applicable; };
        std::vector<ModeRun> modes;
        if (is_tg68k) {
            modes = {{"second core, 68000 mode", 0x00, true},
                      {"second core, 68010 mode", 0x01, true},
                      {"second core, 68020 mode", 0x03, true}};
        } else {
            modes = {{"first core", 0x00, true}};
        }

        for (auto& m : modes) {
            // P3's own precondition mutants are built as 68000-mode images;
            // they only apply to the 68000-mode iteration, reusing every
            // label below so the mutation makes its own clause go red
            // through the unchanged comparator (as P1-MOVE etc. already do).
            std::string bin = dir + "/p3.bin";
            if (m.cfg == 0x00) {
                if (mv == "P3A" || mv == "P3B") bin = dir + "/p3_mutnop.bin";
                else if (mv == "P3C") bin = dir + "/p3_mutc.bin";
                else if (mv == "P3D") bin = dir + "/p3_mutd.bin";
            }
            auto r = p3::run(bin, m.cfg);
            bool fixture_ok = r.loaded && r.released && r.done;
            checks.expect(fixture_ok, "P3 fixture: p3.bin loaded, released and reached DONE (" + m.label + ")");

            auto win = p3::run_window(r);
            uint16_t ri=0, rd=0;
            checks.expect(check_c1(r.log, ri), "C1 [p280]: interrupts off (P3 run, " + m.label + ")");
            checks.expect(check_c2(r.log, rd), "C2 [p278]: DMA off (P3 run, " + m.label + ")");
            checks.expect(check_c3(r.log), "C3: S_END written exactly once, S_FAIL never written (P3 run, " + m.label + ")");
            checks.expect(check_c4(r.log), "C4: S_START written exactly once (P3 run, " + m.label + ")");

            bool p3a = true, p3b = true, p3d = true;
            int reads_at_w[5] = {0,0,0,0,0};
            for (int k = 1; k <= 4; ++k) {
                p3a = p3a && p3::block_transferred(r, k);
                p3d = p3d && p3::opcode_matches_table(k, r.opcode[k]);
                uint32_t Wk = r.x[k] + 2;
                int n = win.ok ? read_count_between(r.log, Wk, win.start, win.end) : 0;
                reads_at_w[k] = n;
                if (win.ok) {
                    auto sx = find_write(r.log, S_X, 0xDEAD);
                    auto sf = find_write(r.log, S_FAIL, 0xFA11);
                    p3b = p3b && sx.count==0 && sf.count==0;
                }
            }
            checks.expect(p3a, "P3.a: every block's stop instruction executed and transferred control (" + m.label + ")");
            checks.expect(p3b, "P3.b: no guard word was executed, S_X and S_FAIL never written (" + m.label + ")");
            checks.expect(p3d, "P3.d: every block's stop instruction at X_k is the opcode family the table names (" + m.label + ")");

            // P3.c proxy: no CPU read at any W_k before that block's own
            // S_P write (see this file's header note on the static check).
            bool p3c = true;
            for (int k = 1; k <= 4; ++k) {
                auto spk = find_write(r.log, S_P, uint16_t(k));
                if (!spk.found) { p3c = false; continue; }
                uint32_t Wk = r.x[k] + 2;
                for (auto& x : r.log) if (x.word_addr == wa(Wk) && x.cycle < spk.cycle) p3c = false;
            }
            checks.expect(p3c, "P3.c: no CPU read of a guard word occurs before its own block starts (" + m.label + ")");

            int reads_at_z = win.ok ? read_count_between(r.log, FARZ, win.start, win.end) : -1;
            checks.record("P3 measured reads per guard word W_1..W_4 (" + m.label + "): " +
                std::to_string(reads_at_w[1]) + "," + std::to_string(reads_at_w[2]) + "," +
                std::to_string(reads_at_w[3]) + "," + std::to_string(reads_at_w[4]) +
                "; far word Z reads: " + std::to_string(reads_at_z) + " (calibration, not a page prediction)");

            bool any_read_somewhere = (reads_at_w[1]>0)||(reads_at_w[2]>0)||(reads_at_w[3]>0)||(reads_at_w[4]>0);
            checks.expect(fixture_ok && win.ok && any_read_somewhere,
                "P3 [p13] opus:1-guidelines.c64,sol:1-guidelines.c64,opus:1-guidelines.c65,sol:1-guidelines.c65: "
                "at least one CPU read at a guard word W_k inside the run, showing the CPU loaded ahead of the "
                "instruction it executes (" + m.label + ")");
            checks.expect(reads_at_z == 0,
                "P3 calibration: the far word Z (never referenced, " + std::to_string(FARZ) +
                ") shows zero CPU reads, so the counter can report absence (" + m.label + ")");
        }
    }

    // =======================================================================
    // P5 [p13] opus:1-guidelines.c52,sol:1-guidelines.c52: the exception
    // stack frame differs between 68000-family members.
    // =======================================================================
    {
        std::string bin0 = dir + "/p5_68000.bin";
        if (mv == "P5A") bin0 = dir + "/p5_muta.bin";
        else if (mv == "P5C") bin0 = dir + "/p5_mutc.bin";
        auto r0 = p5::run(bin0, 0x00);
        bool fixture0_ok = r0.loaded && r0.released;
        checks.expect(fixture0_ok, "P5 fixture: the 68000-image loaded and released (" + core_label + ", 68000 mode)");

        p5::Frame frames0[6];
        bool p5a0 = true, p5c_normal = true;
        for (int e = 1; e <= 5; ++e) {
            auto w = p5::case_window(r0, e);
            p5a0 = p5a0 && w.ok;
            if (w.ok) {
                // vector read check: a CPU read at the installed vector's
                // address occurred inside the window.
                uint32_t vec = (e==1)?0x80 : (e==2)?0x10 : (e==3)?0x28 : (e==4)?0x1C : 0x20;
                int vr = read_count_between(r0.log, vec, w.a_cycle, w.b_cycle);
                p5a0 = p5a0 && vr > 0;
                frames0[e] = p5::frame_of(r0, w);
                p5c_normal = p5c_normal && p5::writes_within_stack(frames0[e], w.pre_sp, r0.post_sp[e]);
            } else {
                p5c_normal = false;
            }
        }
        checks.expect(fixture0_ok && p5a0, "P5.a: every case was taken through its installed vector (68000 mode, " + core_label + ")");
        checks.expect(fixture0_ok && p5c_normal,
            "P5.c: every CPU write inside each case's window lies between POST_SP_e and PRE_SP-2 (68000 mode, " + core_label + ")");
        for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (68000 mode, " + core_label + "): " + p5::frame_str(frames0[e]));

        // ---- 68010/68020 runs: only meaningful on the second core ----
        p5::Frame frames1[6], frames2[6];
        bool have_10 = false, have_20 = false;
        if (is_tg68k) {
            std::string vbrbin = dir + (mv=="P5B" ? "/p5_mutb.bin" : "/p5_vbr.bin");
            auto r1 = p5::run(vbrbin, 0x01);
            auto r2 = p5::run(dir + "/p5_vbr.bin", 0x03);
            bool fixture1_ok = r1.loaded && r1.released;
            bool fixture2_ok = r2.loaded && r2.released;
            checks.expect(fixture1_ok, "P5 fixture: the VBR image loaded and released (second core, 68010 mode)");
            checks.expect(fixture2_ok, "P5 fixture: the VBR image loaded and released (second core, 68020 mode)");

            bool p5a1=true, p5a2=true;
            uint32_t pre_sp_1 = 0;
            for (int e = 1; e <= 5; ++e) {
                auto w1 = p5::case_window(r1, e);
                auto w2 = p5::case_window(r2, e);
                p5a1 = p5a1 && w1.ok;
                p5a2 = p5a2 && w2.ok;
                if (w1.ok) { frames1[e] = p5::frame_of(r1, w1); if (e==1) pre_sp_1 = w1.pre_sp; }
                if (w2.ok) { frames2[e] = p5::frame_of(r2, w2); }
            }
            checks.expect(fixture1_ok && p5a1, "P5.a: every case was taken through its installed vector (68010 mode)");
            checks.expect(fixture2_ok && p5a2, "P5.a: every case was taken through its installed vector (68020 mode)");
            for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (68010 mode): " + p5::frame_str(frames1[e]));
            for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (68020 mode): " + p5::frame_str(frames2[e]));
            have_10 = fixture1_ok && p5a1;
            have_20 = fixture2_ok && p5a2;

            // P5.b: PRE_SP equal across the 68000/68010/68020 runs (static
            // image check: only the VBR-vs-NOP words differ, asserted by
            // construction of p5.s/p5_vbr.s -- see that file). The SAME
            // equality oracle is used regardless of NMH_MUTATE: under P5B,
            // r1 comes from p5_mutb.bin (SUBQ.L #4,A7 inserted), so PRE_SP
            // there genuinely differs and this goes red on its own.
            auto w0_1 = p5::case_window(r0, 1);
            bool p5b = w0_1.ok && (pre_sp_1 == w0_1.pre_sp);
            checks.expect(p5b, "P5.b: PRE_SP is the same in every run");

            if (have_10 && have_20) {
                bool differ_00_10 = false, differ_00_20 = false, differ_10_20 = false;
                for (int e=1;e<=5;++e) {
                    if (p5::frames_differ(frames0[e], frames1[e])) differ_00_10 = true;
                    if (p5::frames_differ(frames0[e], frames2[e])) differ_00_20 = true;
                    if (p5::frames_differ(frames1[e], frames2[e])) differ_10_20 = true;
                }
                checks.expect(differ_00_10,
                    "P5 [p13] opus:1-guidelines.c52,sol:1-guidelines.c52: for the pair (68000,68010), at least one "
                    "exception writes a different frame (second core)");
                checks.expect(differ_00_20,
                    "P5 [p13] opus:1-guidelines.c52,sol:1-guidelines.c52: for the pair (68000,68020), at least one "
                    "exception writes a different frame (second core)");
                checks.expect(differ_10_20,
                    "P5 [p13] opus:1-guidelines.c52,sol:1-guidelines.c52: for the pair (68010,68020), at least one "
                    "exception writes a different frame (second core)");
            }

        } else {
            checks.record("P5 CONTRAST data (first core, 68000 mode) recorded above as F_1..F_5; compare against "
                           "the second core's own 68000-mode F_1..F_5 in the hand-off report (both must be equal, "
                           "since two 68000s are predicted to write the same frame)");
        }

    }

    // =======================================================================
    // P6 [p13] opus:1-guidelines.c67,sol:1-guidelines.c67: the 68020
    // prefetches more words than the 68000. Graded on the second core
    // only (68020 vs 68000); first core recorded for the report-level
    // contrast.
    // =======================================================================
    {
        std::string bin = dir + (mv == "P6E" ? "/p6_mute.bin" : "/p6.bin");
        auto r = p6::run(bin, 0x00);
        bool fixture_ok = r.loaded && r.released && r.done;
        checks.expect(fixture_ok, "P6 fixture: p6.bin loaded, released and reached DONE (" + core_label + ", 68000 mode)");

        auto win = p6::run_window(r);
        bool p3ad_ok = win.ok;
        for (int k = 1; k <= 8; ++k) p3ad_ok = p3ad_ok && p6::block_transferred(r, k);
        checks.expect(p3ad_ok, "P3.a-P3.d (reused, SPECIFICATION.md section 8): every one of the 8 blocks transferred control (" + core_label + ", 68000 mode)");

        const int guard_len = (mv == "P6E") ? 1 : 6; // a fixed, known property of which binary was assembled
        int D0 = 0;
        for (int k = 1; k <= 8; ++k) D0 = std::max(D0, win.ok ? p6::g_of(r, win, r.x[k]) : 0);
        checks.record("P6 D(68000 mode, " + core_label + ") = " + std::to_string(D0) + " (guard length " + std::to_string(guard_len) + ")");

        checks.expect(win.ok && D0 < guard_len,
            std::string("P6.e: D(68000 mode) is less than the guard length") +
            (mv=="P6E" ? " -- MUTATION CHECK: with a one-word guard, D0 equals it instead, so this goes red" : " (six words)"));

        if (is_tg68k) {
            auto r20 = p6::run(dir + "/p6.bin", 0x03);
            auto win20 = p6::run_window(r20);
            int D20 = 0;
            for (int k = 1; k <= 8; ++k) D20 = std::max(D20, win20.ok ? p6::g_of(r20, win20, r20.x[k]) : 0);
            checks.record("P6 D(68020 mode, second core) = " + std::to_string(D20));
            checks.expect(win20.ok && D20 > D0,
                "P6 [p13] opus:1-guidelines.c67,sol:1-guidelines.c67: D(68020 mode) > D(68000 mode), same image, "
                "second core");
        } else {
            checks.record("P6 CONTRAST data (first core) recorded above as D(68000 mode); compare against the "
                           "second core's own 68000-mode D in the hand-off report (neither side is predicted "
                           "'more advanced', so 'more' must not hold)");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
